/*******************************************************************************
 * smfTbsMgr.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stl.h>
#include <dtl.h>
#include <knl.h>
#include <scl.h>
#include <smlDef.h>
#include <smDef.h>
#include <smg.h>
#include <sma.h>
#include <smfDef.h>
#include <smf.h>
#include <smp.h>
#include <smr.h>
#include <smd.h>
#include <smxl.h>
#include <smxm.h>
#include <smfTbsMgr.h>
#include <smfRecovery.h>
#include <smlGeneral.h>

/**
 * @file smfTbsMgr.c
 * @brief Storage Manager Layer Tablespace Manager Internal Routines
 */

extern smfDatafileFuncs   gMemoryDatafileFuncs;

/**
 * @addtogroup smf
 * @{
 */

smfDatafileFuncs * gDatafileFuncs[] =
{
    &gMemoryDatafileFuncs
};

smfInitialTbsData gSmfNoMountTbsInfoArray[] = 
{
    {
        SML_MEMORY_DICT_SYSTEM_TBS_ID,
        ( SML_TBS_DEVICE_MEMORY |
          SML_TBS_TEMPORARY_NO  |
          SML_TBS_USAGE_DICT ),
        SMB_DEFAULT_SYSTEM_MEMORY_DICT_TBS_EXTSIZE / SMP_PAGE_SIZE,
        SML_MEMORY_DICT_SYSTEM_TBS_NAME
    },
    {
        SML_MEMORY_UNDO_SYSTEM_TBS_ID,
        ( SML_TBS_DEVICE_MEMORY |
          SML_TBS_TEMPORARY_NO  |
          SML_TBS_USAGE_UNDO ),
        SMB_DEFAULT_SYSTEM_MEMORY_UNDO_TBS_EXTSIZE / SMP_PAGE_SIZE,
        SML_MEMORY_UNDO_SYSTEM_TBS_NAME
    },
    {
        SML_MEMORY_DATA_SYSTEM_TBS_ID,
        ( SML_TBS_DEVICE_MEMORY |
          SML_TBS_TEMPORARY_NO  |
          SML_TBS_USAGE_DATA ),
        SMB_DEFAULT_SYSTEM_MEMORY_DATA_TBS_EXTSIZE / SMP_PAGE_SIZE,
        SML_MEMORY_DATA_SYSTEM_TBS_NAME
    },
    {
        SML_MEMORY_TEMP_SYSTEM_TBS_ID,
        ( SML_TBS_DEVICE_MEMORY |
          SML_TBS_TEMPORARY_YES | 
          SML_TBS_USAGE_TEMPORARY ),
        SMB_DEFAULT_SYSTEM_MEMORY_TEMP_TBS_EXTSIZE / SMP_PAGE_SIZE,
        SML_MEMORY_TEMP_SYSTEM_TBS_NAME
    },
    
    {
        0,
        0,
        0,
        NULL
    }
};

/**
 * @brief Tablespace를 생성한다.
 * @param[in] aStatement Statement 포인터
 * @param[in] aTbsAttr Tablespace Attribute
 * @param[in] aUndoLogging Undo Logging 여부 ( 데이터베이스 생성시에는 Undo Logging을 할수 없음 )
 * @param[out] aTbsId Tablespace 아이디
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smfCreateTbs( smlStatement * aStatement,
                        smlTbsAttr   * aTbsAttr,
                        stlBool        aUndoLogging,
                        smlTbsId     * aTbsId,
                        smlEnv       * aEnv )
{
    smlTbsId           sTbsId;
    smfTbsInfo       * sTbsInfo = NULL;
    smlDatafileAttr  * sDatafileAttr;
    smfDatafileInfo  * sDatafileInfo;
    stlUInt32          sTbsTypeId;
    stlInt32           i;
    stlBool            sIsTimedOut;
    stlInt64           sExtSize;
    stlInt32           sState = 0;
    stlInt32           sFileState = 0;
    smfTbsPendingArgs  sPendingArgs;
    knlRegionMark      sMemMark;
    stlChar         ** sDatafileNameArray;
    smfDatafileHeader  sDatafileHeader;
    stlFile            sFile;
    stlInt32           sFlag;
    stlInt64           sDatabaseIo;
    stlInt16           sBackupLevel;
    stlUInt16          sShmIndex;
    smfTbsPersData     sAfterTbsPersData;
    stlInt32           sDatafileCount = 0;
    
    STL_TRY( smgAllocShmMem4Mount( STL_SIZEOF( smfTbsInfo ),
                                   (void**)&sTbsInfo,
                                   aEnv ) == STL_SUCCESS );

    SMF_INIT_TBS_INFO( sTbsInfo );
    
    /**
     * Tablespace Attribute 정리
     * - mAttr이 설정되지 않는 경우는 LOGGING | MEMORY 이다.
     */

    STL_DASSERT( stlStrlen( aTbsAttr->mName ) > 0 );

    stlStrncpy( sTbsInfo->mTbsPersData.mName,
                aTbsAttr->mName,
                STL_MAX_SQL_IDENTIFIER_LENGTH );
    
    sTbsInfo->mTbsPersData.mAttr = aTbsAttr->mAttr;

    if( (aTbsAttr->mValidFlags & SML_TBS_ATTR_MASK) == SML_TBS_ATTR_NO )
    {
        sTbsInfo->mTbsPersData.mAttr = SML_TBS_DEFAULT_ATTR;
    }

    if( (aTbsAttr->mValidFlags & SML_TBS_EXTSIZE_MASK) == SML_TBS_EXTSIZE_NO )
    {
        sExtSize = SML_TBS_DEFAULT_EXTSIZE;
    }
    else
    {
        /**
         * Extent class 결정
         * 1. (8 page)  >= aTbsAttr->mExtSize              ==> 8 page
         * 2. (8 page)  <  aTbsAttr->mExtSize <= (16 page) ==> 16 page
         * 3. (16 page) <  aTbsAttr->mExtSize <= (32 page) ==> 32 page
         * 4. (32 page) <  aTbsAttr->mExtSize <= (64 page) ==> 64 page
         * 5. (64 page) <  aTbsAttr->mExtSize              ==> 128 page
         */
        
        if( aTbsAttr->mExtSize <= SML_EXTSIZE_CLASS_1 )
        {
            sExtSize = SML_EXTSIZE_CLASS_1;
        }
        else if( aTbsAttr->mExtSize <= SML_EXTSIZE_CLASS_2 )
        {
            sExtSize = SML_EXTSIZE_CLASS_2;
        }
        else if( aTbsAttr->mExtSize <= SML_EXTSIZE_CLASS_3 )
        {
            sExtSize = SML_EXTSIZE_CLASS_3;
        }
        else if( aTbsAttr->mExtSize <= SML_EXTSIZE_CLASS_4 )
        {
            sExtSize = SML_EXTSIZE_CLASS_4;
        }
        else
        {
            sExtSize = SML_EXTSIZE_CLASS_5;
        }
    }
    
    if( (aTbsAttr->mValidFlags & SML_TBS_LOGGING_MASK) == SML_TBS_LOGGING_YES )
    {
        sTbsInfo->mTbsPersData.mIsLogging = aTbsAttr->mIsLogging;
    }
    else
    {
        if( (sTbsInfo->mTbsPersData.mAttr & SML_TBS_TEMPORARY_MASK)
            == SML_TBS_TEMPORARY_YES )
        {
            sTbsInfo->mTbsPersData.mIsLogging = STL_FALSE;
        }
        else
        {
            sTbsInfo->mTbsPersData.mIsLogging = STL_TRUE;
        }
    }

    if( (aTbsAttr->mValidFlags & SML_TBS_ONLINE_MASK) == SML_TBS_ONLINE_YES )
    {
        sTbsInfo->mTbsPersData.mIsOnline = aTbsAttr->mIsOnline;

        if( aTbsAttr->mIsOnline == STL_FALSE )
        {
            sTbsInfo->mTbsPersData.mOfflineState = SMF_OFFLINE_STATE_CONSISTENT;
        }
    }
    else
    {
        sTbsInfo->mTbsPersData.mIsOnline = STL_TRUE;
    }

    /**
     * Tablespace Incremental Backup 정보 초기화
     */
    for( sBackupLevel = 0;
         sBackupLevel < SML_INCREMENTAL_BACKUP_LEVEL_MAX;
         sBackupLevel++ )
    {
        sTbsInfo->mTbsPersData.mIncBackup.mBackupLsn[sBackupLevel] = SMR_INVALID_LSN;
    }

    sTbsInfo->mTbsPersData.mIncBackup.mBackupSeq = 0;

    /**
     * volatile online flag 동기화
     */
    sTbsInfo->mIsOnline4Scan = sTbsInfo->mTbsPersData.mIsOnline;
    sTbsInfo->mIsProceedMediaRecovery = STL_FALSE;
    sTbsInfo->mIsProceedRestore = STL_FALSE;

    /**
     * Tablespace의 Backup을 위한 정보 초기화
     */
    SMF_INIT_TBS_BACKUP_INFO( &(sTbsInfo->mBackupInfo), aEnv );

    sTbsInfo->mTbsPersData.mPageCountInExt = ( STL_ALIGN( sExtSize, 4 * SMP_PAGE_SIZE )
                                               / SMP_PAGE_SIZE );

    sDatafileAttr = aTbsAttr->mDatafileAttr;

    /**
     * 물리적 데이터파일 생성
     * @todo : 같은 이름의 서로 다른 크기의 Datafile을 가지는 두 tablespace가 동시에
     * REUSE 옵션으로 생성되면 결과가 틀려질 수 있다.
     */
    while( sDatafileAttr != NULL )
    {
        sTbsTypeId = SML_TBS_DEVICE_TYPE_ID( aTbsAttr->mAttr );

        STL_TRY( gDatafileFuncs[sTbsTypeId]->mCreate( SMA_GET_TRANS_ID( aStatement ),
                                                      SMF_INVALID_PHYSICAL_TBS_ID,
                                                      gSmfWarmupEntry->mDbPersData.mNewTbsId,
                                                      sTbsInfo,
                                                      sDatafileAttr,
                                                      aUndoLogging,
                                                      &sDatafileInfo,
                                                      aEnv )
                 == STL_SUCCESS );
        
        sTbsInfo->mDatafileInfoArray[sDatafileInfo->mDatafilePersData.mID] = sDatafileInfo;
        sTbsInfo->mTbsPersData.mNewDatafileId++;
        sDatafileCount++;

        sDatafileAttr = sDatafileAttr->mNext;
    }
    
    KNL_INIT_REGION_MARK(&sMemMark);
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               (knlEnv*)aEnv ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                STL_SIZEOF( stlChar* ) * sDatafileCount,
                                (void**)&sDatafileNameArray,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    for( i = 0; i < sDatafileCount; i++ )
    {
        sDatafileNameArray[i] = sTbsInfo->mDatafileInfoArray[i]->mDatafilePersData.mName;
    }

    /**
     * New Tablespace Id 획득
     */
    STL_TRY( knlAcquireLatch( &gSmfWarmupEntry->mLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              (knlEnv*)aEnv ) == STL_SUCCESS );
    sState = 2;

    sTbsId = gSmfWarmupEntry->mDbPersData.mNewTbsId;
    gSmfWarmupEntry->mDbPersData.mNewTbsId++;

    /*
     * Create tablespace rollback 시 tablespace latch를 이용하기 때문에
     * Warmup entry에 tbs info를 연결하기 전에 latch를 초기화해야 한다.
     */ 
    STL_TRY( knlInitLatch( &sTbsInfo->mLatch,
                           STL_TRUE,
                           (knlEnv*)aEnv ) == STL_SUCCESS );

    sTbsInfo->mTbsPersData.mTbsId = sTbsId;
    sTbsInfo->mTbsPersData.mState = SMF_TBS_STATE_INIT;
    stlMemBarrier();
    
    gSmfWarmupEntry->mTbsInfoArray[sTbsId] = sTbsInfo;
    
    sState = 1;
    STL_TRY( knlReleaseLatch( &gSmfWarmupEntry->mLatch,
                              (knlEnv*)aEnv ) == STL_SUCCESS );

    /**
     * @note Create Tablespace 실패시 Memory Leak이 발생할수 있지만 이를 허용한다.
     * - 이는 Control latch를 획득한 상태에서 로깅을 할수 없기 때문이다.
     */
    /**
     * Tablespace Logging
     * - Tablespace Redo Log는 하나의 Tablespace Redo Log와
     *   N개의 Datafile Redo Log로 나뉘어 기록된다.
     * - Tablespace Undo Log는 하나의 Tablespace Undo Log만 기록한다.
     *   ( 즉, 하나의 Undo Log를 이용해서 Logical Undo를 수행한다 )
     * @todo Device type에 따라 분리되어야 한다.
     */
    
    stlMemcpy( &sAfterTbsPersData,
               &sTbsInfo->mTbsPersData,
               STL_SIZEOF( smfTbsPersData ) );
    
    sAfterTbsPersData.mState = SMF_TBS_STATE_CREATING;
    sAfterTbsPersData.mNewDatafileId = 0;

    STL_TRY( smfWriteLogTbs( SMA_GET_TRANS_ID( aStatement ),
                             SMR_LOG_MEMORY_TBS_CREATE,
                             SMF_UNDO_LOG_MEMORY_TBS_CREATE,
                             &sTbsInfo->mTbsPersData,
                             &sAfterTbsPersData,
                             aUndoLogging,
                             aEnv )
             == STL_SUCCESS );

    for( i = 0; i < SML_DATAFILE_MAX_COUNT; i++ )
    {
        sDatafileInfo = sTbsInfo->mDatafileInfoArray[i];
        
        if( sDatafileInfo == NULL )
        {
            continue;
        }
        
        /**
         * write FILE HEADER
         */

        if( SML_TBS_IS_MEMORY_TEMPORARY( sTbsInfo->mTbsPersData.mAttr ) == STL_FALSE )
        {
            sDatafileHeader.mTbsPhysicalId = sTbsId;
            sDatafileHeader.mDatafileId = sDatafileInfo->mDatafilePersData.mID;
            smrGetConsistentChkptLsnAndLid( &sDatafileHeader.mCheckpointLsn,
                                            &sDatafileHeader.mCheckpointLid );
            sDatafileHeader.mCreationTime = sDatafileInfo->mDatafilePersData.mTimestamp;

            /**
             * Redo를 위해 CheckpointLsn과 CheckpointLid를 Pers data에 박는다.
             */

            sDatafileInfo->mDatafilePersData.mCheckpointLsn = sDatafileHeader.mCheckpointLsn;
            sDatafileInfo->mDatafilePersData.mCheckpointLid = sDatafileHeader.mCheckpointLid;
            sDatafileInfo->mDatafilePersData.mCreationLsn = sDatafileHeader.mCheckpointLsn;
            sDatafileInfo->mDatafilePersData.mCreationLid = sDatafileHeader.mCheckpointLid;

            sFlag = STL_FOPEN_LARGEFILE | STL_FOPEN_WRITE | STL_FOPEN_READ;

            sDatabaseIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_DATABASE_FILE_IO,
                                                         KNL_ENV(aEnv) );

            if( sDatabaseIo == 0 )
            {
                sFlag |= STL_FOPEN_DIRECT;
            }

            STL_TRY( smgOpenFile( &sFile,
                                  sDatafileInfo->mDatafilePersData.mName,
                                  sFlag,
                                  STL_FPERM_OS_DEFAULT,
                                  aEnv )
                     == STL_SUCCESS );
            sFileState = 1;
        
            STL_TRY( smfWriteDatafileHeader( &sFile,
                                             &sDatafileHeader,
                                             aEnv )
                     == STL_SUCCESS );
    
            sFileState = 0;
            STL_TRY( smgCloseFile( &sFile,
                                   aEnv )
                     == STL_SUCCESS );
        }

        /**
         * 데이터파일 공유 메모리를 생성한다.
         */
    
        STL_TRY( knlCreateDatabaseArea( NULL,  /* aName */
                                        &sShmIndex,
                                        sDatafileInfo->mShmSize,
                                        (knlEnv*)aEnv ) == STL_SUCCESS );

        sDatafileInfo->mShmState = SMF_DATAFILE_SHM_STATE_LOADED;
        sDatafileInfo->mShmIndex = sShmIndex;

        sDatafileInfo->mChunk    = KNL_MAKE_LOGICAL_ADDR( sShmIndex, 0 );
        sDatafileInfo->mPchArray = KNL_MAKE_LOGICAL_ADDR( sShmIndex,
                                                          sDatafileInfo->mDatafilePersData.mSize );

        sDatafileInfo->mDatafilePersData.mState = SMF_DATAFILE_STATE_CREATING;

        /**
         * Datafile Logging
         * - Datafile 관련해서는 Redo Log만 기록한다.
         * - Create Tablespace Undo는 Datafile과 함께 Logical Undo를 가정한다.
         */
        STL_TRY( smfWriteLogDatafile( SMA_GET_TRANS_ID( aStatement ),
                                      sTbsId,
                                      SMR_LOG_MEMORY_DATAFILE_ADD,
                                      SMF_UNDO_LOG_MEMORY_DATAFILE_ADD,
                                      &sDatafileInfo->mDatafilePersData,
                                      &sDatafileInfo->mDatafilePersData,
                                      STL_FALSE,  /* aUndoLogging */
                                      aEnv )
                 == STL_SUCCESS );
    }

    KNL_BREAKPOINT( KNL_BREAKPOINT_SMFCREATETBS_BEFORE_ADD_TBSINFO_TO_CONTROLFILE,
                    (knlEnv*)aEnv );

    /**
     * Control file info에 기록
     */
    STL_TRY( knlAcquireLatch( &gSmfWarmupEntry->mLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              (knlEnv*)aEnv ) == STL_SUCCESS );
    sState = 2;

    /**
     * validate datafile name
     */

    STL_TRY( smfValidateDatafileNames( sDatafileNameArray,
                                       sDatafileCount,
                                       gSmfWarmupEntry->mDbPersData.mNewTbsId,
                                       aEnv )
             == STL_SUCCESS );

    /**
     * 다른 Session에서 접근 가능하게 한다.
     */
    
    sTbsInfo->mTbsPersData.mState = SMF_TBS_STATE_CREATING;

    if( smxlGetDataStoreMode() == SML_DATA_STORE_MODE_TDS )
    {
        STL_TRY( smfChangeDatafileHeader( aStatement,
                                          SMF_CHANGE_DATAFILE_HEADER_FOR_GENERAL,
                                          sTbsId,
                                          SML_INVALID_DATAFILE_ID,
                                          SMR_INVALID_LID,
                                          SMR_INVALID_LSN,
                                          aEnv ) == STL_SUCCESS );
    }

    sState = 1;
    STL_TRY( knlReleaseLatch( &gSmfWarmupEntry->mLatch,
                              (knlEnv*)aEnv ) == STL_SUCCESS );

    STL_TRY( smfSaveCtrlFile( aEnv ) == STL_SUCCESS );

    /**
     * Tablespace Pending Operation 등록
     */

    stlMemset( &sPendingArgs, 0x00, STL_SIZEOF(smfTbsPendingArgs) );
    sPendingArgs.mTransId = SMA_GET_TRANS_ID( aStatement );
    sPendingArgs.mTbsId = sTbsId;
                                   
    STL_TRY( smgAddPendOp( aStatement,
                           SMG_PEND_CREATE_TABLESPACE,
                           SML_PEND_ACTION_COMMIT,
                           (void*)&sPendingArgs,
                           STL_SIZEOF( smfTbsPendingArgs ),
                           0,
                           aEnv ) == STL_SUCCESS );
    
    *aTbsId = sTbsId;

    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       (knlEnv*)aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sFileState == 1 )
    {
        (void)smgCloseFile( &sFile, aEnv );
    }
    
    switch( sState )
    {
        case 2:
            (void)knlReleaseLatch( &gSmfWarmupEntry->mLatch, KNL_ENV(aEnv) );
        case 1:
            (void)knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                            &sMemMark,
                                            STL_FALSE, /* aFreeChunk */
                                            (knlEnv*)aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief Tablespace를 삭제한다.
 * @param[in]     aStatement    Statement 포인터
 * @param[in]     aTbsId        삭제할 Tablespace 아이디
 * @param[in]     aAndDatafiles 데이터파일의 삭제 여부
 * @param[in,out] aEnv          Environment 정보
 */
stlStatus smfDropTbs( smlStatement * aStatement,
                      smlTbsId       aTbsId,
                      stlBool        aAndDatafiles,
                      smlEnv       * aEnv )
{
    smfTbsInfo       * sTbsInfo;
    smfDatafileInfo  * sDatafileInfo;
    stlUInt32          sState = 0;
    stlUInt32          i;
    smfTbsPersData     sAfterTbsPersData;
    smfTbsPendingArgs  sPendingArgs;
    smfDatafilePersData sAfterDatafilePersData;
    
    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];

    for( i = 0; i < SML_DATAFILE_MAX_COUNT; i++ )
    {
        sDatafileInfo = sTbsInfo->mDatafileInfoArray[i];
        
        if( sDatafileInfo == NULL )
        {
            continue;
        }

        if( (sDatafileInfo->mDatafilePersData.mState == SMF_DATAFILE_STATE_INIT) ||
            (sDatafileInfo->mDatafilePersData.mState == SMF_DATAFILE_STATE_DROPPED) )
        {
            continue;
        }

        stlMemcpy( &sAfterDatafilePersData,
                   &sDatafileInfo->mDatafilePersData,
                   STL_SIZEOF( smfDatafilePersData ) );
    
        sAfterDatafilePersData.mState = SMF_DATAFILE_STATE_DROPPING;

        /**
         * Datafile Logging
         * - Redo Log만 기록한다.
         */

        STL_TRY( smfWriteLogDatafile( SMA_GET_TRANS_ID( aStatement ),
                                      aTbsId,
                                      SMR_LOG_MEMORY_DATAFILE_DROP,
                                      SMF_UNDO_LOG_MEMORY_DATAFILE_DROP,
                                      NULL,      /* aBeforeDatafilePersData */
                                      &sAfterDatafilePersData,
                                      STL_FALSE, /* aUndoLogging */
                                      aEnv )
                 == STL_SUCCESS );
    }

    stlMemcpy( &sAfterTbsPersData,
               &sTbsInfo->mTbsPersData,
               STL_SIZEOF( smfTbsPersData ) );
        
    sAfterTbsPersData.mState = SMF_TBS_STATE_DROPPING;
    
    /**
     * Tablespace Logging
     * - Tablespace Redo Log는 하나의 Tablespace Redo Log와
     *   N개의 Datafile Redo Log로 나뉘어 기록된다.
     * - Tablespace Undo Log는 하나의 Tablespace Undo Log만 기록한다.
     *   ( 즉, 하나의 Undo Log를 이용해서 Logical Undo를 수행한다 )
     * @todo Device type에 따라 분리되어야 한다.
     */
    STL_TRY( smfWriteLogTbs( SMA_GET_TRANS_ID( aStatement ),
                             SMR_LOG_MEMORY_TBS_DROP,
                             SMF_UNDO_LOG_MEMORY_TBS_DROP,
                             &sTbsInfo->mTbsPersData,
                             &sAfterTbsPersData,
                             STL_TRUE, /* aUndoLogging */
                             aEnv )
             == STL_SUCCESS );

    STL_TRY( smfAcquireTablespace( sTbsInfo, aEnv ) == STL_SUCCESS );
    sState = 1;

    sTbsInfo->mTbsPersData.mState = SMF_TBS_STATE_DROPPING;
    
    sState = 0;
    STL_TRY( smfReleaseTablespace( sTbsInfo, aEnv ) == STL_SUCCESS );

    KNL_BREAKPOINT( KNL_BREAKPOINT_SMFDROPTBS_BEFORE_ADD_COMMIT_PENDING,
                    (knlEnv*)aEnv );

    stlMemset( &sPendingArgs, 0x00, STL_SIZEOF(smfTbsPendingArgs) );
    sPendingArgs.mTransId = SMA_GET_TRANS_ID( aStatement );
    sPendingArgs.mTbsId = aTbsId;
    sPendingArgs.mAndDatafiles = aAndDatafiles;
                                   
    STL_TRY( smgAddPendOp( aStatement,
                           SMG_PEND_DROP_TABLESPACE,
                           SML_PEND_ACTION_COMMIT,
                           (void*)&sPendingArgs,
                           STL_SIZEOF( smfTbsPendingArgs ),
                           22,
                           aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smfReleaseTablespace( sTbsInfo, aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief 주어진 Tablespace의 이름을 변경한다.
 * @param[in]     aStatement  Statement 포인터
 * @param[in]     aTbsId 대상 Tablespace identifier
 * @param[in]     aTbsName    변경될 Tablespace 이름
 * @param[in,out] aEnv         Environment 정보
 */
stlStatus smfRenameTbs( smlStatement * aStatement,
                        smlTbsId       aTbsId,
                        stlChar      * aTbsName,
                        smlEnv       * aEnv )
{
    smfTbsInfo    * sTbsInfo;
    stlUInt32       sState = 0;

    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];

    STL_DASSERT( stlStrlen( aTbsName ) < STL_MAX_SQL_IDENTIFIER_LENGTH );

    STL_TRY( smfWriteLogRenameTbs( SMA_GET_TRANS_ID( aStatement ),
                                   aTbsId,
                                   SMR_LOG_MEMORY_TBS_RENAME,
                                   SMF_UNDO_LOG_MEMORY_TBS_RENAME,
                                   sTbsInfo->mTbsPersData.mName,
                                   aTbsName,
                                   STL_TRUE, /* aUndoLogging */
                                   aEnv )
             == STL_SUCCESS );
                                   
    STL_TRY( smfAcquireTablespace( sTbsInfo, aEnv ) == STL_SUCCESS );
    sState = 1;
    
    stlStrcpy( sTbsInfo->mTbsPersData.mName, aTbsName );

    sState = 1;
    STL_TRY( smfReleaseTablespace( sTbsInfo, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void)smfReleaseTablespace( sTbsInfo, aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}

/**
 * @brief Open Database Resetlogs 이후 Dictionary Tbs에 등록되지 않은 Tbs 삭제.
 * @param[in] aTbsId  tablespace id
 * @param[in,out] aEnv environment
 */ 
stlStatus smfDropUnregisteredTbs( smlTbsId   aTbsId,
                                  smlEnv   * aEnv )
{
    smfTbsInfo      * sTbsInfo;
    smfDatafileInfo * sDatafileInfo;
    stlBool           sTbsLatchOn = STL_FALSE;
    stlInt32          i;
    stlUInt32         sTbsTypeId;
    
    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];

    sTbsTypeId = SML_TBS_DEVICE_TYPE_ID( sTbsInfo->mTbsPersData.mAttr );

    STL_TRY( smfAcquireTablespace( sTbsInfo,
                                   aEnv ) == STL_SUCCESS );
    sTbsLatchOn = STL_TRUE;

    for( i = 0; i < SML_DATAFILE_MAX_COUNT; i++ )
    {
        sDatafileInfo = sTbsInfo->mDatafileInfoArray[i];

        if( sDatafileInfo == NULL )
        {
            continue;
        }

        if( (sDatafileInfo->mDatafilePersData.mState == SMF_DATAFILE_STATE_INIT) ||
            (sDatafileInfo->mDatafilePersData.mState == SMF_DATAFILE_STATE_DROPPED) )
        {
            continue;
        }

        STL_TRY( knlLogMsg( NULL,
                            KNL_ENV( aEnv ),
                            KNL_LOG_LEVEL_INFO,
                            ".... TABLESPACE(%d), DATAFILE(%s) : Dropped Datafile\n",
                            aTbsId,
                            sDatafileInfo->mDatafilePersData.mName )
                 == STL_SUCCESS );

        STL_TRY( gDatafileFuncs[sTbsTypeId]->mDrop( SML_INVALID_TRANSID,
                                                    sTbsInfo,
                                                    sDatafileInfo,
                                                    aEnv )
                 == STL_SUCCESS );

        sDatafileInfo->mDatafilePersData.mState = SMF_DATAFILE_STATE_DROPPED;
    }

    sTbsInfo->mTbsPersData.mState = SMF_TBS_STATE_DROPPED;
    
    sTbsLatchOn = STL_FALSE;
    STL_TRY( smfReleaseTablespace( sTbsInfo,
                                   aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sTbsLatchOn == STL_TRUE )
    {
        (void) smfReleaseTablespace( sTbsInfo, aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief 주어진 Tablespace를 Offline으로 변경한다.
 * @param[in]     aStatement       Statement 포인터
 * @param[in]     aTbsId           Tablespace identifier
 * @param[in]     aOfflineBehavior Offline 동작
 * @param[in,out] aEnv             Environment 정보
 * @note 래치가 획득된 상태에서 해당 함수를 사용해서는 안된다.
 */
stlStatus smfOfflineTbs( smlStatement       * aStatement,
                         smlTbsId             aTbsId,
                         smlOfflineBehavior   aOfflineBehavior,
                         smlEnv             * aEnv )
{
    smfTbsInfo        * sTbsInfo;
    stlUInt32           sState = 0;
    smfTbsPendingArgs   sPendingArgs;
    smgPendOp           sPendOp;
    stlInt32            sOfflineState = SMF_OFFLINE_STATE_INCONSISTENT;

    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];

    if( aOfflineBehavior == SML_OFFLINE_BEHAVIOR_NORMAL )
    {
        if( (knlGetCurrStartupPhase() >= KNL_STARTUP_PHASE_OPEN) &&
            (smxlGetDataStoreMode() == SML_DATA_STORE_MODE_TDS) )
        {
            STL_TRY( smrFlushAllLogs( STL_FALSE /* aForceDiskIo */,
                                      aEnv )
                     == STL_SUCCESS );
        }
    }
    
    /**
     * Tablespace Pending Operation 등록
     */

    if( aStatement != NULL )
    {
        /*
         * Tablespace를 flush하는 중에 exception이 발생할 경우
         * NULL statement를 이용하여 tablespace offline이 수행된다.
         * 이경우 pending operation을 등록하지 않고 offline을 수행하며
         * 만약 offline중에 exception이 발생할 경우 rollback을 직접 수행한다.
         */ 
        stlMemset( &sPendingArgs, 0x00, STL_SIZEOF(smfTbsPendingArgs) );
        sPendingArgs.mTransId = SMA_GET_TRANS_ID( aStatement );
        sPendingArgs.mTbsId = aTbsId;
        sPendingArgs.mOfflineState = SMF_OFFLINE_STATE_NULL;
                                   
        STL_TRY( smgAddPendOp( aStatement,
                               SMG_PEND_OFFLINE_TABLESPACE,
                               SML_PEND_ACTION_ROLLBACK,
                               (void*)&sPendingArgs,
                               STL_SIZEOF( smfTbsPendingArgs ),
                               0,
                               aEnv ) == STL_SUCCESS );
    }
    
    STL_TRY( smfAcquireTablespace( sTbsInfo, aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY_THROW( sTbsInfo->mTbsPersData.mIsOnline == STL_TRUE,
                   RAMP_ERR_NOT_ONLINE_TABLESPACE );
    
    /**
     * 2가지 경우에 Offline Tablespace는 Media recovery가 필요없다.
     * - OPEN 상태에서 OFFLINE NORMAL로 offline 시키는 경우
     * - MOUNT 상테에서 이전 종료시 서버가 SHUTDOWN ABORT로 종료되지 않은 경우
     */
    if( knlGetCurrStartupPhase() >= KNL_STARTUP_PHASE_OPEN )
    {
        if( aOfflineBehavior == SML_OFFLINE_BEHAVIOR_NORMAL )
        {
            sOfflineState = SMF_OFFLINE_STATE_CONSISTENT;
        }
    }
    else
    {
        if( smfGetServerState() == SML_SERVER_STATE_SHUTDOWN )
        {
            sOfflineState = SMF_OFFLINE_STATE_CONSISTENT;
        }
    }
    
    sTbsInfo->mIsOnline4Scan = STL_FALSE;
    
    if( aStatement == NULL )
    {
        sTbsInfo->mTbsPersData.mIsOnline = STL_FALSE;
        sTbsInfo->mTbsPersData.mOfflineState = sOfflineState;
    }
    else
    {
        /**
         * pending operation으로 처리한다.
         */
    }
    
    if( knlGetCurrStartupPhase() >= KNL_STARTUP_PHASE_OPEN )
    {
        sTbsInfo->mTbsPersData.mOfflineLsn = smrGetSystemLsn();
    }
    else
    {
        sTbsInfo->mTbsPersData.mOfflineLsn = SMR_INIT_LSN;
    }

    if( aStatement != NULL )
    {
        /*
         * Tablespace를 flush하는 중에 exception이 발생할 경우
         * NULL statement를 이용하여 tablespace offline이 수행된다.
         * 이경우 pending operation을 등록하지 않고 offline을 수행하며
         * 만약 offline중에 exception이 발생할 경우 rollback을 직접 수행한다.
         */ 
        stlMemset( &sPendingArgs, 0x00, STL_SIZEOF(smfTbsPendingArgs) );
        sPendingArgs.mTransId = SMA_GET_TRANS_ID( aStatement );
        sPendingArgs.mTbsId = aTbsId;
        sPendingArgs.mOfflineState = sOfflineState;
                                   
        STL_TRY( smgAddPendOp( aStatement,
                               SMG_PEND_OFFLINE_TABLESPACE,
                               SML_PEND_ACTION_COMMIT,
                               (void*)&sPendingArgs,
                               STL_SIZEOF( smfTbsPendingArgs ),
                               0,
                               aEnv ) == STL_SUCCESS );
    }    

    STL_TRY( smfSaveCtrlFile( aEnv ) == STL_SUCCESS );
    
    sState = 1;
    STL_TRY( smfReleaseTablespace( sTbsInfo, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_ONLINE_TABLESPACE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_NOT_ONLINE_TABLESPACE,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      sTbsInfo->mTbsPersData.mName );
    }

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void)smfReleaseTablespace( sTbsInfo, aEnv );
        default:
            break;
    }

    if( aStatement == NULL )
    {
        /*
         * Tablespace flush 중에 exception발생으로 tablespace offline을
         * 수행하다가 exception이 발생했을 경우 직접 rollback 한다.
         */ 
        stlMemset( &sPendOp, 0x00, STL_SIZEOF(smgPendOp) );
        stlMemset( &sPendingArgs, 0x00, STL_SIZEOF(smfTbsPendingArgs) );

        sPendingArgs.mTbsId = aTbsId;
        sPendOp.mArgs = &sPendingArgs;

        (void)smfOfflineTbsPending( SML_PEND_ACTION_ROLLBACK,
                                    &sPendOp,
                                    aEnv );
    }
    
    return STL_FAILURE;
}

/**
 * @brief 주어진 Offline Tablespace 에 Relation Identifier를 설정한다.
 * @param[in]     aStatement       Statement 포인터
 * @param[in]     aTbsId           Tablespace identifier
 * @param[in]     aRelationId      Pending Relation Identifier
 * @param[in,out] aEnv             Environment 정보
 * @remarks 해당 함수는 MOUNT 에서 OPEN 으로 전이되는 과정에서 호출되는 함수이다.
 */
stlStatus smfSetOfflineRelationId( smlStatement       * aStatement,
                                   smlTbsId             aTbsId,
                                   stlInt64             aRelationId,
                                   smlEnv             * aEnv )
{
    smfTbsInfo    * sTbsInfo;
    stlUInt32       sState = 0;

    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];

    STL_TRY( smfAcquireTablespace( sTbsInfo, aEnv ) == STL_SUCCESS );
    sState = 1;

    sTbsInfo->mTbsPersData.mRelationId = aRelationId;

    STL_TRY( smfSaveCtrlFile( aEnv ) == STL_SUCCESS );
    
    sState = 1;
    STL_TRY( smfReleaseTablespace( sTbsInfo, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void)smfReleaseTablespace( sTbsInfo, aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}

/**
 * @brief smpStartupPreOpen에서 Page Validation 이후 Tablespace의 Corruption 확인.
 * @param[in,out] aEnv   envrionment 정보 
 * @remarks MOUNT 에서 OPEN 으로 전이되는 과정에서 모든 Tablespace의 TbsInfo 확인하여 Corruption 체크.
 */ 
stlStatus smfVerifyDatabaseCorruption( smlEnv * aEnv )
{
    smlTbsId            sTbsId = SML_INVALID_TBS_ID;
    smfTbsInfo        * sTbsInfo;
    stlBool             sTbsExist;

    /**
     * 1 step: Corrupted Tbs를 Shrink한다.
     */
    STL_TRY( smfFirstTbs( &sTbsId,
                          &sTbsExist,
                          aEnv ) == STL_SUCCESS );

    while( sTbsExist == STL_TRUE )
    {
        sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[sTbsId];

        /**
         * Skip Offline Tablespace
         *      Not Created Tbs
         *      Temporary tablespace 
         */
        if( (SMF_IS_ONLINE_TBS( sTbsId ) != STL_TRUE) ||
            (SML_TBS_IS_TEMPORARY( sTbsInfo->mTbsPersData.mAttr ) == STL_TRUE) ||
            (smfGetTbsState( sTbsId ) != SMF_TBS_STATE_CREATED) )
        {
            STL_TRY( smfNextTbs( &sTbsId,
                                 &sTbsExist,
                                 aEnv ) == STL_SUCCESS );
            continue;
        }

        STL_TRY( smpShrinkCorruptedTbs( sTbsId,
                                        aEnv )
                 == STL_SUCCESS );

        STL_TRY( smfNextTbs( &sTbsId,
                             &sTbsExist,
                             aEnv ) == STL_SUCCESS );
    }
    
    /**
     * 2 step: Corruption Object 확인.
     */
    STL_TRY( smfFirstTbs( &sTbsId,
                          &sTbsExist,
                          aEnv ) == STL_SUCCESS );

    while( sTbsExist == STL_TRUE )
    {
        sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[sTbsId];
        
        /**
         * Skip Offline Tablespace
         *      Not Created Tbs
         *      Temporary tablespace 
         */
        if( (SMF_IS_ONLINE_TBS( sTbsId ) != STL_TRUE) ||
            (SML_TBS_IS_TEMPORARY( sTbsInfo->mTbsPersData.mAttr ) == STL_TRUE) ||
            (smfGetTbsState( sTbsId ) != SMF_TBS_STATE_CREATED) )
        {
            STL_TRY( smfNextTbs( &sTbsId,
                                 &sTbsExist,
                                 aEnv ) == STL_SUCCESS );
            continue;
        }

        STL_TRY( smfVerifyTbsCorruption( sTbsId,
                                         aEnv )
                 == STL_SUCCESS );

        STL_TRY( smfNextTbs( &sTbsId,
                             &sTbsExist,
                             aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Tablespace에 속한 Datafile의 Page 깨짐 상태를 확인한다.
 * @param[in] aTbsId  Tablespace Id
 * @param[out] aIsCorrupted  Page Corruption Flag
 * @param[in,out] aEnv  Environment
 */ 
stlStatus smfIsCorruptedTbs( smlTbsId   aTbsId,
                             stlBool  * aIsCorrupted,
                             smlEnv   * aEnv )
{
    smlDatafileId     sDatafileId = SML_INVALID_DATAFILE_ID;
    smfDatafileInfo * sDatafileInfo;
    stlBool           sExist = STL_FALSE;

    STL_TRY( smfFirstDatafile( aTbsId,
                               &sDatafileId,
                               &sExist,
                               aEnv )
             == STL_SUCCESS );

    while( sExist == STL_TRUE )
    {
        sDatafileInfo = smfGetDatafileInfo( aTbsId,
                                            sDatafileId );

        STL_DASSERT( sDatafileInfo != NULL );

        if( (sDatafileInfo->mDatafilePersData.mState != SMF_DATAFILE_STATE_CREATING) &&
            (sDatafileInfo->mDatafilePersData.mState != SMF_DATAFILE_STATE_CREATED) )
        {
            STL_TRY( smfNextDatafile( aTbsId,
                                      &sDatafileId,
                                      &sExist,
                                      aEnv ) == STL_SUCCESS );
            continue;
        }
        
        if( sDatafileInfo->mCorruptionCount != 0 )
        {
            *aIsCorrupted = STL_TRUE;
            break;
        }

        STL_TRY( smfNextDatafile( aTbsId,
                                  &sDatafileId,
                                  &sExist,
                                  aEnv ) == STL_SUCCESS );
    }
             
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Tablespace에 속한 Datafile의 Page 깨짐 상태를 확인하고 필요하다면 Error 처리한다.
 * @param[in] aTbsId  Tablespace Id
 * @param[in,out] aEnv  Environment
 */ 
stlStatus smfVerifyTbsCorruption( smlTbsId  aTbsId,
                                  smlEnv  * aEnv )
{
    smfTbsInfo      * sTbsInfo;
    smfDatafileInfo * sDatafileInfo;
    smlDatafileId     sDatafileId = SML_INVALID_DATAFILE_ID;
    stlBool           sExist = STL_FALSE;

    STL_TRY( smfFirstDatafile( aTbsId,
                               &sDatafileId,
                               &sExist,
                               aEnv ) == STL_SUCCESS );

    while( sExist == STL_TRUE )
    {
        sDatafileInfo = smfGetDatafileInfo( aTbsId,
                                            sDatafileId );

        STL_DASSERT( sDatafileInfo != NULL );

        if( (sDatafileInfo->mDatafilePersData.mState != SMF_DATAFILE_STATE_CREATING) &&
            (sDatafileInfo->mDatafilePersData.mState != SMF_DATAFILE_STATE_CREATED) )
        {
            STL_TRY( smfNextDatafile( aTbsId,
                                      &sDatafileId,
                                      &sExist,
                                      aEnv ) == STL_SUCCESS );
            continue;
        }

        STL_TRY_THROW( sDatafileInfo->mCorruptionCount == 0, RAMP_ERR_EXIST_CORRUPTION );

        STL_TRY( smfNextDatafile( aTbsId,
                                  &sDatafileId,
                                  &sExist,
                                  aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_EXIST_CORRUPTION )
    {
        (void) smpShrinkCorruptedTbs( aTbsId,
                                      aEnv );

        sTbsInfo = (smfTbsInfo *)smfGetTbsHandle( aTbsId );

        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_DATAFILE_REQUIRED_RECOVERY,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      sDatafileInfo->mDatafilePersData.mName,
                      sTbsInfo->mTbsPersData.mName );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Tablespace를 Online으로 변경하기 위해 validation check를 수행한다.
 * @param[in]     aTbsId Validation check할 tablespace Id
 * @param[in,out] aEnv   Environment 정보
 */
stlStatus smfValidateOnlineTbs( smlTbsId   aTbsId,
                                smlEnv   * aEnv )
{
    smfTbsInfo       * sTbsInfo;
    smfDatafileInfo  * sDatafileInfo;
    smlDatafileId      sDatafileId = SML_INVALID_DATAFILE_ID;
    stlInt32           sFileState = 0;
    smfDatafileHeader  sDatafileHeader;
    stlFile            sFile;
    stlBool            sDatafileExist;
    stlInt32           sFlag;
    stlInt64           sDatabaseIo; 

    sFlag = STL_FOPEN_LARGEFILE | STL_FOPEN_READ;

    sDatabaseIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_DATABASE_FILE_IO,
                                                 KNL_ENV(aEnv) );

    if( sDatabaseIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }

    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];

    /**
     * Offline Tablespace를 Online Tablespace로 변경할 때
     * Offline behavior를 확인한다. 만약 Immediate로 offline
     * 되었을 경우 Media Recover를 수행해야 한다.
     */
    STL_TRY_THROW( sTbsInfo->mTbsPersData.mOfflineState !=
                   SMF_OFFLINE_STATE_INCONSISTENT,
                   RAMP_ERR_NEED_MEDIA_RECOVERY );

    /**
     * Offline 후 Old datafile을 복사했을 경우를 detect한다.
     * controlfile의 datafile persData에 기록된 마지막으로 flush된 Lsn과
     * datafile file header에 기록된 checkpoint Lsn을 비교하여,
     * 만약 동일하지 않으면 media recover를 수행한다. 특별히,
     * datafile persData에 기록된 mCheckpointLsn이 INVALID_LSN일 때는
     * MOUNT단계에서 추가된 datafile이기 때문에 무시한다.
     */
    STL_TRY( smfFirstDatafile( aTbsId,
                               &sDatafileId,
                               &sDatafileExist,
                               aEnv ) == STL_SUCCESS );

    while( sDatafileExist == STL_TRUE )
    {
        sDatafileInfo = sTbsInfo->mDatafileInfoArray[sDatafileId];

        STL_TRY( smgOpenFile( &sFile,
                              sDatafileInfo->mDatafilePersData.mName,
                              sFlag,
                              STL_FPERM_OS_DEFAULT,
                              aEnv )
                 == STL_SUCCESS );
        sFileState = 1;

        STL_TRY( smfReadDatafileHeader( &sFile,
                                        sDatafileInfo->mDatafilePersData.mName,
                                        &sDatafileHeader,
                                        SML_ENV(aEnv) )
                 == STL_SUCCESS );

        sFileState = 0;
        STL_TRY( smgCloseFile( &sFile,
                               aEnv )
                 == STL_SUCCESS );

        /**
         * Datafile의 Checkpoint Lsn과 Controlfile의 CheckpointLsn이
         * 다르다면 Media recovery가 필요하다.
         */
        STL_TRY_THROW( ((sDatafileInfo->mDatafilePersData.mCheckpointLsn
                         == SMR_INVALID_LSN) ||
                        (sDatafileHeader.mCheckpointLsn
                         == sDatafileInfo->mDatafilePersData.mCheckpointLsn)),
                       RAMP_ERR_NEED_MEDIA_RECOVERY );

        STL_TRY( smfNextDatafile( aTbsId,
                                  &sDatafileId,
                                  &sDatafileExist,
                                  aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NEED_MEDIA_RECOVERY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_NEED_MEDIA_RECOVERY,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      sTbsInfo->mTbsPersData.mName );
    }

    STL_FINISH;

    if( sFileState == 1 )
    {
        (void)smgCloseFile( &sFile, aEnv );
    }

    return STL_FAILURE;
}

stlInt64 smfGetPendingRelationId( smlTbsId aTbsId )
{
    smfTbsInfo  * sTbsInfo;
    
    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];

    return sTbsInfo->mTbsPersData.mRelationId;
}

/**
 * @brief 주어진 Tablespace를 Online으로 변경한다.
 * @param[in]     aStatement  Statement 포인터
 * @param[in]     aTbsId      Tablespace identifier
 * @param[in,out] aEnv        Environment 정보
 */
stlStatus smfOnlineTbs( smlStatement * aStatement,
                        smlTbsId       aTbsId,
                        smlEnv       * aEnv )
{
    smfTbsInfo       * sTbsInfo;
    stlUInt32          sState = 0;
    smfTbsPendingArgs  sPendingArgs;
    stlBool            sPendingOpAdded = STL_FALSE;
    stlBool            sIsSuccess;

    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];

    if( smxlGetDataStoreMode() == SML_DATA_STORE_MODE_TDS )
    {
        STL_TRY( smrFlushAllLogs( STL_FALSE /* aForceDiskIo */,
                                  aEnv )
                 == STL_SUCCESS );
    }

    /**
     * Backup waiting을 설정한 후 ROLLBACK pending operation을 등록한다.
     * 만약 Backup waiting을 설정한 후 OnlineTbs가 실패하면
     * 1. ROLLBACK pending operation이 등록되어 있으면 pending operation에서
     *    backup waiting을 reset한다.
     * 2. ROLLBACK pending operation을 등록하지 못했으면 exception에서 reset한다.
     */
    STL_TRY( smfSetBackupWaitingState( aTbsId, &sIsSuccess, aEnv ) == STL_SUCCESS );

    /**
     * backup이 진행중이면 exception
     */
    STL_TRY_THROW( sIsSuccess == STL_TRUE, RAMP_ERR_BACKUP_IN_PROGRESS );
    sState = 1;

    /**
     * Tablespace Pending Operation 등록
     */
    
    stlMemset( &sPendingArgs, 0x00, STL_SIZEOF(smfTbsPendingArgs) );
    sPendingArgs.mTransId = SMA_GET_TRANS_ID( aStatement );
    sPendingArgs.mTbsId = aTbsId;
    sPendingArgs.mOfflineState = sTbsInfo->mTbsPersData.mOfflineState;
                                   
    STL_TRY( smgAddPendOp( aStatement,
                           SMG_PEND_ONLINE_TABLESPACE,
                           SML_PEND_ACTION_ROLLBACK,
                           (void*)&sPendingArgs,
                           STL_SIZEOF( smfTbsPendingArgs ),
                           STL_SIZEOF( smpFlushTbsArg ),
                           aEnv ) == STL_SUCCESS );
    sPendingOpAdded = STL_TRUE;

    /**
     * online
     */
    
    STL_TRY( smfAcquireTablespace( sTbsInfo, aEnv ) == STL_SUCCESS );
    sState = 2;

    STL_TRY_THROW( sTbsInfo->mTbsPersData.mIsOnline == STL_FALSE,
                   RAMP_ERR_NOT_OFFLINE_TABLESPACE );

    sTbsInfo->mTbsPersData.mIsOnline = STL_TRUE;
    sTbsInfo->mTbsPersData.mOfflineState = SMF_OFFLINE_STATE_NULL;

    STL_TRY( smfSaveCtrlFile( aEnv ) == STL_SUCCESS );
    
    sState = 1;
    STL_TRY( smfReleaseTablespace( sTbsInfo, aEnv ) == STL_SUCCESS );

    /**
     * loading tablespace
     */
    
    STL_TRY( smfParallelLoad( aTbsId,
                              aEnv ) == STL_SUCCESS );

    KNL_BREAKPOINT( KNL_BREAKPOINT_SMFONLINETBS_AFTER_PARALLELLOAD,
                    KNL_ENV(aEnv) );
                                   
    /**
     * Tablespace Pending Operation 등록
     */

    stlMemset( &sPendingArgs, 0x00, STL_SIZEOF(smfTbsPendingArgs) );
    sPendingArgs.mTransId = SMA_GET_TRANS_ID( aStatement );
    sPendingArgs.mTbsId = aTbsId;
                                   
    STL_TRY( smgAddPendOp( aStatement,
                           SMG_PEND_ONLINE_TABLESPACE,
                           SML_PEND_ACTION_COMMIT,
                           (void*)&sPendingArgs,
                           STL_SIZEOF( smfTbsPendingArgs ),
                           STL_SIZEOF( smpFlushTbsArg ),
                           aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_BACKUP_IN_PROGRESS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_BACKUP_IN_PROGRESS,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      sTbsInfo->mTbsPersData.mName );
    }

    STL_CATCH( RAMP_ERR_NOT_OFFLINE_TABLESPACE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_NOT_OFFLINE_TABLESPACE,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      sTbsInfo->mTbsPersData.mName );
    }

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)smfReleaseTablespace( sTbsInfo, aEnv );
        case 1:
            if( sPendingOpAdded == STL_FALSE )
            {
                (void)smfResetBackupWaitingState( aTbsId, aEnv );
            }
        default:
            break;
    }
    
    return STL_FAILURE;
}

/**
 * @brief 주어진 테이블스페이스에 Datafile을 추가한다.
 * @param[in] aStatement Statement 포인터
 * @param[in] aTbsId 대상 Tablespace 아이디
 * @param[in] aDatafileAttr 추가할 Datafile 정보
 * @param[out] aDatafileId 추가된 Datafile 아이디
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smfAddDatafile( smlStatement    * aStatement,
                          smlTbsId          aTbsId,
                          smlDatafileAttr * aDatafileAttr,
                          smlDatafileId   * aDatafileId,
                          smlEnv          * aEnv )
{
    smfDatafileHeader      sDatafileHeader;
    stlFile                sFile;
    smfTbsInfo           * sTbsInfo;
    smfDatafileInfo      * sDatafileInfo = NULL;
    stlUInt32              sTbsTypeId;
    stlUInt32              sState = 0;
    stlInt32               sFileState = 0;
    smfDatafilePendingArgs sPendingArgs;
    stlBool                sIsTimedOut;
    stlChar              * sDatafileName;
    stlInt32               sFlag;
    stlInt64               sDatabaseIo;
    stlUInt16              sShmIndex;
    smfDatafilePersData    sAfterDatafilePersData;

    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];

    sTbsTypeId = SML_TBS_DEVICE_TYPE_ID( sTbsInfo->mTbsPersData.mAttr );
    STL_TRY( gDatafileFuncs[sTbsTypeId]->mCreate( SMA_GET_TRANS_ID( aStatement ),
                                                  aTbsId,
                                                  aTbsId, /* aHintTbsId */
                                                  sTbsInfo,
                                                  aDatafileAttr,
                                                  STL_TRUE,    /* aUndoLogging */
                                                  &sDatafileInfo,
                                                  aEnv )
             == STL_SUCCESS );

    /**
     * write FILE HEADER
     */

    if( SML_TBS_IS_MEMORY_TEMPORARY( sTbsInfo->mTbsPersData.mAttr ) == STL_FALSE )
    {
        sDatafileHeader.mTbsPhysicalId = aTbsId;
        sDatafileHeader.mDatafileId = sDatafileInfo->mDatafilePersData.mID;

        smrGetConsistentChkptLsnAndLid( &sDatafileHeader.mCheckpointLsn,
                                        &sDatafileHeader.mCheckpointLid );

        sDatafileHeader.mCreationTime = sDatafileInfo->mDatafilePersData.mTimestamp;

        /**
         * Redo를 위해 CheckpointLsn과 CheckpointLid를 Pers data에 박는다.
         */

        sDatafileInfo->mDatafilePersData.mCheckpointLsn = sDatafileHeader.mCheckpointLsn;
        sDatafileInfo->mDatafilePersData.mCheckpointLid = sDatafileHeader.mCheckpointLid;
        sDatafileInfo->mDatafilePersData.mCreationLsn = sDatafileHeader.mCheckpointLsn;
        sDatafileInfo->mDatafilePersData.mCreationLid = sDatafileHeader.mCheckpointLid;

        sFlag = STL_FOPEN_LARGEFILE | STL_FOPEN_WRITE | STL_FOPEN_READ;

        sDatabaseIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_DATABASE_FILE_IO,
                                                     KNL_ENV(aEnv) );

        if( sDatabaseIo == 0 )
        {
            sFlag |= STL_FOPEN_DIRECT;
        }

        STL_TRY( smgOpenFile( &sFile,
                              sDatafileInfo->mDatafilePersData.mName,
                              sFlag,
                              STL_FPERM_OS_DEFAULT,
                              aEnv )
                 == STL_SUCCESS );
        sFileState = 1;
        
        STL_TRY( smfWriteDatafileHeader( &sFile,
                                         &sDatafileHeader,
                                         aEnv )
                 == STL_SUCCESS );

        sFileState = 0;
        STL_TRY( smgCloseFile( &sFile,
                               aEnv )
                 == STL_SUCCESS );
    }

    /**
     * 데이터파일 공유 메모리를 생성한다.
     */
    
    STL_TRY( knlCreateDatabaseArea( NULL,  /* aName */
                                    &sShmIndex,
                                    sDatafileInfo->mShmSize,
                                    (knlEnv*)aEnv ) == STL_SUCCESS );

    sDatafileInfo->mShmState = SMF_DATAFILE_SHM_STATE_LOADED;
    sDatafileInfo->mShmIndex = sShmIndex;

    sDatafileInfo->mChunk    = KNL_MAKE_LOGICAL_ADDR( sShmIndex, 0 );
    sDatafileInfo->mPchArray = KNL_MAKE_LOGICAL_ADDR( sShmIndex,
                                                      sDatafileInfo->mDatafilePersData.mSize );

    STL_TRY( knlAcquireLatch( &gSmfWarmupEntry->mLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              (knlEnv*)aEnv ) == STL_SUCCESS );
    sState = 1;

    /**
     * database에 연결한다.
     */
    
    sTbsInfo->mTbsPersData.mNewDatafileId++;
    stlMemBarrier();
    sTbsInfo->mDatafileInfoArray[sDatafileInfo->mDatafilePersData.mID] = sDatafileInfo;
    
    sState = 0;
    STL_TRY( knlReleaseLatch( &gSmfWarmupEntry->mLatch,
                              (knlEnv*)aEnv ) == STL_SUCCESS );

    /**
     * @note Add Datafile 실패시 Memory Leak이 발생할수 있지만 이를 허용한다.
     * - 이는 Control latch를 획득한 상태에서 로깅을 할수 없기 때문이다.
     */
    
    stlMemcpy( &sAfterDatafilePersData,
               &sDatafileInfo->mDatafilePersData,
               STL_SIZEOF( smfDatafilePersData ) );
    
    sAfterDatafilePersData.mState = SMF_DATAFILE_STATE_CREATING;
    
    STL_TRY( smfWriteLogDatafile( SMA_GET_TRANS_ID( aStatement ),
                                  aTbsId,
                                  SMR_LOG_MEMORY_DATAFILE_ADD,
                                  SMF_UNDO_LOG_MEMORY_DATAFILE_ADD,
                                  &sDatafileInfo->mDatafilePersData,
                                  &sAfterDatafilePersData,
                                  STL_TRUE, /* aUndoLogging */
                                  aEnv )
             == STL_SUCCESS );

    KNL_BREAKPOINT( KNL_BREAKPOINT_SMFADDDATAFILE_AFTER_LOGGING,
                    KNL_ENV(aEnv) );
                                   
    STL_TRY( knlAcquireLatch( &gSmfWarmupEntry->mLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              (knlEnv*)aEnv ) == STL_SUCCESS );
    sState = 1;

    sDatafileName = sDatafileInfo->mDatafilePersData.mName;
    STL_TRY( smfValidateDatafileNames( (stlChar**)&sDatafileName,
                                       1,  /* aDatafileCount */
                                       aTbsId,
                                       aEnv )
             == STL_SUCCESS );
    
    /**
     * 다른 Session에서 볼수 있도록 한다.
     */
    
    sDatafileInfo->mDatafilePersData.mState = SMF_DATAFILE_STATE_CREATING;

    if( (knlGetCurrStartupPhase() >= KNL_STARTUP_PHASE_OPEN) &&
        (smxlGetDataStoreMode() == SML_DATA_STORE_MODE_TDS) )
    {
        STL_TRY( smfChangeDatafileHeader( aStatement,
                                          SMF_CHANGE_DATAFILE_HEADER_FOR_GENERAL,
                                          aTbsId,
                                          sDatafileInfo->mDatafilePersData.mID,
                                          SMR_INVALID_LID,
                                          SMR_INVALID_LSN,
                                          aEnv ) == STL_SUCCESS );
    }

    sState = 0;
    STL_TRY( knlReleaseLatch( &gSmfWarmupEntry->mLatch,
                              (knlEnv*)aEnv ) == STL_SUCCESS );

    STL_TRY( smfSaveCtrlFile( aEnv ) == STL_SUCCESS );

    /**
     * Add Datafile Commit Pending Operation 등록
     */

    stlMemset( &sPendingArgs, 0x00, STL_SIZEOF(smfDatafilePendingArgs) );
    sPendingArgs.mTbsId = aTbsId;
    sPendingArgs.mDatafileId = sDatafileInfo->mDatafilePersData.mID;
    sPendingArgs.mTransId = SMA_GET_TRANS_ID( aStatement );

    STL_TRY( smgAddPendOp( aStatement,
                           SMG_PEND_ADD_DATAFILE,
                           SML_PEND_ACTION_COMMIT,
                           (void*)&sPendingArgs,
                           STL_SIZEOF( smfDatafilePendingArgs ),
                           0,
                           aEnv ) == STL_SUCCESS );

    *aDatafileId = sDatafileInfo->mDatafilePersData.mID;
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sFileState == 1 )
    {
        (void)smgCloseFile( &sFile, aEnv );
    }

    switch( sState )
    {
        case 1:
            (void)knlReleaseLatch( &gSmfWarmupEntry->mLatch, (knlEnv*)aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}

/**
 * @brief Datafile을 삭제한다.
 * @param[in] aStatement Statement 포인터
 * @param[in] aTbsId 대상 Tablespace 아이디
 * @param[in] aDatafileId 삭제할 Datafile 아이디
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smfDropDatafile( smlStatement    * aStatement,
                           smlTbsId          aTbsId,
                           smlDatafileId     aDatafileId,
                           smlEnv          * aEnv )
{
    smfTbsInfo           * sTbsInfo;
    smfDatafileInfo      * sDatafileInfo;
    stlUInt32              sState = 0;
    smfDatafilePendingArgs sPendingArgs;
    smfDatafilePersData    sAfterDatafilePersData;

    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];
    sDatafileInfo = sTbsInfo->mDatafileInfoArray[aDatafileId];

    stlMemcpy( &sAfterDatafilePersData,
               &sDatafileInfo->mDatafilePersData,
               STL_SIZEOF( smfDatafilePersData ) );
    
    sAfterDatafilePersData.mState = SMF_DATAFILE_STATE_DROPPING;
        
    /**
     * Datafile Logging
     * - Tablespace에 Lock이 획득된 상태이기 때문에 Control file의 락없이
     *   DatafileInfo에 접근해도 된다.
     * - Checkpoint와의 deadlock가능성 때문에 Control file의 락없이 로깅을 해야 한다.
     */
    STL_TRY( smfWriteLogDatafile( SMA_GET_TRANS_ID( aStatement ),
                                  aTbsId,
                                  SMR_LOG_MEMORY_DATAFILE_DROP,
                                  SMF_UNDO_LOG_MEMORY_DATAFILE_DROP,
                                  &sDatafileInfo->mDatafilePersData,
                                  &sAfterDatafilePersData,
                                  STL_TRUE, /* aUndoLogging */
                                  aEnv )
             == STL_SUCCESS );
    
    STL_TRY( smfAcquireTablespace( sTbsInfo, aEnv ) == STL_SUCCESS );
    sState = 1;

    sDatafileInfo = sTbsInfo->mDatafileInfoArray[aDatafileId];
    sDatafileInfo->mDatafilePersData.mState = SMF_DATAFILE_STATE_DROPPING;
    
    /**
     * Offline Tablespace의 Datafile은 삭제할수 없다.
     */
    STL_TRY_THROW( SMF_IS_ONLINE_TBS( aTbsId ) == STL_TRUE,
                   RAMP_ERR_DROP_OFFLINE_DATAFILE );
    
    stlMemset( &sPendingArgs, 0x00, STL_SIZEOF(smfDatafilePendingArgs) );
    sPendingArgs.mTbsId = aTbsId;
    sPendingArgs.mDatafileId = aDatafileId;
    sPendingArgs.mTransId = SMA_GET_TRANS_ID( aStatement );
    
    STL_TRY( smgAddPendOp( aStatement,
                           SMG_PEND_DROP_DATAFILE,
                           SML_PEND_ACTION_COMMIT,
                           (void*)&sPendingArgs,
                           STL_SIZEOF( smfDatafilePendingArgs ),
                           24,
                           aEnv ) == STL_SUCCESS );
   
    sState = 0;
    STL_TRY( smfReleaseTablespace( sTbsInfo, aEnv ) == STL_SUCCESS );

    KNL_BREAKPOINT( KNL_BREAKPOINT_SMFDROPDATAFILE_AFTER_LOGGING,
                    KNL_ENV(aEnv) );
                                   
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_DROP_OFFLINE_DATAFILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_DROP_OFFLINE_DATAFILE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      sDatafileInfo->mDatafilePersData.mName );
    }
    
    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void)smfReleaseTablespace( sTbsInfo, aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}

/**
 * @brief Datafile들의 이름을 변경한다.
 * @param[in]     aStatement       Statement 포인터
 * @param[in]     aTbsId           대상 Tablespace Identifier
 * @param[in]     aOrgDatafileAttr 변경전 Datafile 속성 구조체
 * @param[in]     aNewDatafileAttr 변경후 Datafile 속성 구조체
 * @param[in,out] aEnv             Environment 정보
 */
stlStatus smfRenameDatafiles( smlStatement    * aStatement,
                              smlTbsId          aTbsId,
                              smlDatafileAttr * aOrgDatafileAttr,
                              smlDatafileAttr * aNewDatafileAttr,
                              smlEnv          * aEnv )
{
    stlUInt32         sState = 0;
    smfTbsInfo      * sTbsInfo;
    smfDatafileInfo * sDatafileInfo;
    stlBool           sFound;
    smlDatafileAttr * sOrgAttr;
    smlDatafileAttr * sNewAttr;
    stlChar           sOrgFileName[STL_MAX_FILE_PATH_LENGTH];
    stlChar           sSystemTbsDir[STL_MAX_FILE_PATH_LENGTH];
    stlInt32          sFileNameLen;
    stlInt32          sDatafileCount = 0;
    smlDatafileId   * sDatafileIdArray;
    stlChar        ** sDatafileNameArray;
    stlInt32          i;
    stlBool           sIsTimedOut;
    knlRegionMark     sMemMark;
    stlBool           sFileExist;

    /**
     * - MOUNT 상태이거나 OFFLINE tablespace여야 한다.
     * - DS모드와 CDS모드는 OPEN상태에서도 가능하다.
     */

    if( knlGetCurrStartupPhase() >= KNL_STARTUP_PHASE_OPEN )
    {
        if( smxlGetDataStoreMode() == SML_DATA_STORE_MODE_TDS )
        {
            if( SML_TBS_IS_MEMORY_TEMPORARY( SMF_GET_TBS_ATTR(aTbsId) ) == STL_FALSE )
            {
                STL_TRY_THROW( SMF_IS_ONLINE_TBS( aTbsId ) == STL_FALSE, RAMP_ERR_INTERNAL );
            }
        }
    }
    
    sOrgAttr = aOrgDatafileAttr;
    sNewAttr = aNewDatafileAttr;
    
    while( sOrgAttr != NULL )
    {
        STL_TRY_THROW( sNewAttr != NULL, RAMP_ERR_INTERNAL );
        
        sDatafileCount++;
        
        sOrgAttr = sOrgAttr->mNext;
        sNewAttr = sNewAttr->mNext;
    }

    /**
     * prepare memory
     */
    
    KNL_INIT_REGION_MARK(&sMemMark);
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               (knlEnv*)aEnv ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                STL_SIZEOF( smlDatafileId ) * sDatafileCount,
                                (void**)&sDatafileIdArray,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                STL_SIZEOF( stlChar* ) * sDatafileCount,
                                (void**)&sDatafileNameArray,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlGetPropertyValueByName( "SYSTEM_TABLESPACE_DIR",
                                        sSystemTbsDir,
                                        KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    
    sNewAttr = aNewDatafileAttr;
    sOrgAttr = aOrgDatafileAttr;
    
    for( i = 0; i < sDatafileCount; i++ )
    {
        STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                    STL_MAX_FILE_PATH_LENGTH,
                                    (void**)&sDatafileNameArray[i],
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        if( SML_TBS_IS_MEMORY_TEMPORARY( SMF_GET_TBS_ATTR(aTbsId) ) == STL_FALSE )
        {
            STL_TRY( stlMergeAbsFilePath( sSystemTbsDir,
                                          sNewAttr->mName,
                                          sDatafileNameArray[i],
                                          STL_MAX_FILE_PATH_LENGTH,
                                          &sFileNameLen,
                                          KNL_ERROR_STACK( aEnv ) )
                     == STL_SUCCESS );
        
            STL_TRY( stlMergeAbsFilePath( sSystemTbsDir,
                                          sOrgAttr->mName,
                                          sOrgFileName,
                                          STL_MAX_FILE_PATH_LENGTH,
                                          &sFileNameLen,
                                          KNL_ERROR_STACK( aEnv ) )
                     == STL_SUCCESS );
            
            STL_TRY( stlExistFile( sDatafileNameArray[i],
                                   &sFileExist,
                                   KNL_ERROR_STACK(aEnv) )
                     == STL_SUCCESS );
        
            STL_TRY_THROW( sFileExist == STL_TRUE, RAMP_ERR_FILE_NOT_EXIST );
        }
        else
        {
            stlStrncpy( sOrgFileName, sOrgAttr->mName, STL_MAX_FILE_PATH_LENGTH );
            stlStrncpy( sDatafileNameArray[i], sNewAttr->mName, STL_MAX_FILE_PATH_LENGTH );
        }

        STL_TRY( smfFindDatafileId( aTbsId,
                                    sOrgFileName,
                                    &sDatafileIdArray[i],
                                    &sFound,
                                    aEnv )
                 == STL_SUCCESS );

        STL_TRY_THROW( sFound == STL_TRUE, RAMP_ERR_INTERNAL );

        sOrgAttr = sOrgAttr->mNext;
        sNewAttr = sNewAttr->mNext;
    }

    /**
     * 락을 획득한 상태에서 aNewName이 다른 테이블스페이스에서
     * 사용되고 있는지 검사한다.
     */

    STL_TRY( knlAcquireLatch( &gSmfWarmupEntry->mLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              (knlEnv*)aEnv ) == STL_SUCCESS );
    sState = 2;

    /**
     * validate new datafile name
     */

    STL_TRY( smfValidateDatafileNames( sDatafileNameArray,
                                       sDatafileCount,
                                       aTbsId,
                                       aEnv )
             == STL_SUCCESS);
    /**
     * execute rename datafile
     */

    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];
    sOrgAttr = aOrgDatafileAttr;
    
    for( i = 0; i < sDatafileCount; i++ )
    {
        sDatafileInfo = sTbsInfo->mDatafileInfoArray[sDatafileIdArray[i]];

        STL_TRY( smgAddPendOp( NULL,
                               SMG_PEND_RENAME_DATAFILE,
                               SML_PEND_ACTION_ROLLBACK,
                               (void*)sDatafileNameArray[i],
                               STL_MAX_FILE_PATH_LENGTH,
                               0,
                               aEnv ) == STL_SUCCESS );

        /* new datafile name을 file manager에 add하고 olg datafile name을 제거한다. */
        STL_TRY( knlAddDbFile( sDatafileNameArray[i],
                               KNL_DATABASE_FILE_TYPE_DATAFILE,
                               KNL_ENV(aEnv) ) == STL_SUCCESS );

        stlStrcpy( sDatafileInfo->mDatafilePersData.mName, sDatafileNameArray[i] );

        STL_TRY( smgAddPendOp( NULL,
                               SMG_PEND_RENAME_DATAFILE,
                               SML_PEND_ACTION_COMMIT,
                               (void*)sOrgAttr->mName,
                               STL_MAX_FILE_PATH_LENGTH,
                               0,
                               aEnv ) == STL_SUCCESS );

        sOrgAttr = sOrgAttr->mNext;
    }

    STL_TRY( smfSaveCtrlFile( aEnv ) == STL_SUCCESS );
    
    sState = 1;
    STL_TRY( knlReleaseLatch( &gSmfWarmupEntry->mLatch,
                              (knlEnv*)aEnv ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       (knlEnv*)aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INTERNAL,
                      "- smfRenameDatafile()",
                      KNL_ERROR_STACK(aEnv) );
        
        KNL_SYSTEM_FATAL_DASSERT( STL_FALSE, KNL_ENV(aEnv) );
    }

    STL_CATCH( RAMP_ERR_FILE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_FILE_NOT_EXIST,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      sDatafileNameArray[i] );
    }
        

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)knlReleaseLatch( &gSmfWarmupEntry->mLatch, KNL_ENV(aEnv) );
        case 1:
            (void)knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                            &sMemMark,
                                            STL_FALSE, /* aFreeChunk */
                                            (knlEnv*)aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}

/**
 * @brief Startup 시점에 Aging하지 못한 Tablespace 및 Datafile들을 삭제한다.
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smfRefineAtStartupPreOpen( smlEnv * aEnv )
{
    smlTbsId          sTbsId = SML_INVALID_TBS_ID;
    stlBool           sTbsExist;
    stlBool           sDatafileExist;
    smlDatafileId     sDatafileId = SML_INVALID_DATAFILE_ID;
    smfTbsInfo      * sTbsInfo;
    smfDatafileInfo * sDatafileInfo;
    
    STL_TRY( smfFirstTbs( &sTbsId,
                          &sTbsExist,
                          aEnv ) == STL_SUCCESS );
                 
    while( sTbsExist == STL_TRUE )
    {
        sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[sTbsId];
            
        if( (smfGetTbsState( sTbsId ) == SMF_TBS_STATE_DROPPED) ||
            (smfGetTbsState( sTbsId ) == SMF_TBS_STATE_AGING) )
        {
            STL_TRY( knlLogMsg( NULL,
                                KNL_ENV(aEnv),
                                KNL_LOG_LEVEL_INFO,
                                ".... TABLESPACE(%d) : Dropped Tablespace\n",
                                sTbsId )
                     == STL_SUCCESS );
            
            STL_TRY( smfFirstDatafile( sTbsId,
                                       &sDatafileId,
                                       &sDatafileExist,
                                       aEnv ) == STL_SUCCESS );

            while( sDatafileExist == STL_TRUE )
            {
                sDatafileInfo = sTbsInfo->mDatafileInfoArray[sDatafileId];
                    
                STL_TRY( smgFreeShmMem4Mount( sDatafileInfo,
                                              aEnv )
                         == STL_SUCCESS );
                    
                sTbsInfo->mDatafileInfoArray[sDatafileId] = NULL;
                
                STL_TRY( smfNextDatafile( sTbsId,
                                          &sDatafileId,
                                          &sDatafileExist,
                                          aEnv ) == STL_SUCCESS );
            }
            
            STL_TRY( smgFreeShmMem4Mount( sTbsInfo,
                                          aEnv )
                     == STL_SUCCESS );
            
            gSmfWarmupEntry->mTbsInfoArray[sTbsId] = NULL;
        }
        else
        {
            STL_TRY( smfFirstDatafile( sTbsId,
                                       &sDatafileId,
                                       &sDatafileExist,
                                       aEnv ) == STL_SUCCESS );

            while( sDatafileExist == STL_TRUE )
            {
                STL_DASSERT( smfGetDatafileState(sTbsId, sDatafileId) != SMF_DATAFILE_STATE_INIT );
                
                if( (smfGetDatafileState(sTbsId, sDatafileId) == SMF_DATAFILE_STATE_AGING) ||
                    (smfGetDatafileState(sTbsId, sDatafileId) == SMF_DATAFILE_STATE_DROPPED) )
                {
                    sDatafileInfo = sTbsInfo->mDatafileInfoArray[sDatafileId];
                    
                    STL_TRY( knlLogMsg( NULL,
                                        KNL_ENV(aEnv),
                                        KNL_LOG_LEVEL_INFO,
                                        ".... TABLESPACE(%d), DATAFILE(%s) : Dropped Datafile\n",
                                        sTbsId,
                                        sDatafileInfo->mDatafilePersData.mName )
                             == STL_SUCCESS );
    
                    STL_TRY( smgFreeShmMem4Mount( sDatafileInfo,
                                                  aEnv )
                             == STL_SUCCESS );
                    
                    sTbsInfo->mDatafileInfoArray[sDatafileId] = NULL;
                }
                
                STL_TRY( smfNextDatafile( sTbsId,
                                          &sDatafileId,
                                          &sDatafileExist,
                                          aEnv ) == STL_SUCCESS );
            }
        }
        
        STL_TRY( smfNextTbs( &sTbsId,
                             &sTbsExist,
                             aEnv ) == STL_SUCCESS );
    }
    
    STL_TRY( smfSaveCtrlFile( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Recovery 완료 후 Tablespace와 Datafile의 상태를 보정한다.
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smfRefineAtStartupOpen( smlEnv * aEnv )
{
    smlTbsId          sTbsId = SML_INVALID_TBS_ID;
    stlBool           sTbsExist;
    stlBool           sDatafileExist;
    smlDatafileId     sDatafileId = SML_INVALID_DATAFILE_ID;
    smfTbsInfo      * sTbsInfo;
    smfDatafileInfo * sDatafileInfo;

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV(aEnv),
                        KNL_LOG_LEVEL_INFO,
                        "[STARTUP-SM] RECOVER TABLESPACE AND DATAFILE STATE\n" )
             == STL_SUCCESS );

    STL_TRY( smfFirstTbs( &sTbsId,
                          &sTbsExist,
                          aEnv ) == STL_SUCCESS );
                 
    while( sTbsExist == STL_TRUE )
    {
        sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[sTbsId];

        /**
         * Tablespace 생성 중 CREATING 상태로 controlfile을 저장한 후
         * CREATED로 변경하지 못한 상태에서 장애가 발생하였을 경우
         * Refine 시 CREATED 상태로 설정한다.
         */
        if( sTbsInfo->mTbsPersData.mState == SMF_TBS_STATE_CREATING )
        {
            STL_TRY( knlLogMsg( NULL,
                                KNL_ENV(aEnv),
                                KNL_LOG_LEVEL_INFO,
                                ".... TABLESPACE(%d) : CREATING -> CREATED\n",
                                sTbsId )
                     == STL_SUCCESS );

            sTbsInfo->mTbsPersData.mState = SMF_TBS_STATE_CREATED;
        }

        /**
         * Recovery 완료 후 DROPPING 상태로 남아있는 Tablespace의 TbsInfo와
         * Datafile의 DatafileInfo를 정리한다.
         */
        if( smfGetTbsState( sTbsId ) == SMF_TBS_STATE_DROPPING )
        {
            STL_TRY( knlLogMsg( NULL,
                                KNL_ENV(aEnv),
                                KNL_LOG_LEVEL_INFO,
                                ".... TABLESPACE(%d) : Dropped Tablespace\n",
                                sTbsId )
                     == STL_SUCCESS );
            
            /* Memory에 적재된 tablespace datafile들을 제거한다. */
            STL_TRY( smpShrinkTablespace( sTbsId, aEnv ) == STL_SUCCESS );

            STL_TRY( smfFirstDatafile( sTbsId,
                                       &sDatafileId,
                                       &sDatafileExist,
                                       aEnv ) == STL_SUCCESS );

            while( sDatafileExist == STL_TRUE )
            {
                sDatafileInfo = sTbsInfo->mDatafileInfoArray[sDatafileId];
                    
                STL_TRY( smgFreeShmMem4Mount( sDatafileInfo,
                                              aEnv )
                         == STL_SUCCESS );
                    
                sTbsInfo->mDatafileInfoArray[sDatafileId] = NULL;
                
                STL_TRY( smfNextDatafile( sTbsId,
                                          &sDatafileId,
                                          &sDatafileExist,
                                          aEnv ) == STL_SUCCESS );
            }
            
            STL_TRY( smgFreeShmMem4Mount( sTbsInfo,
                                          aEnv )
                     == STL_SUCCESS );

            gSmfWarmupEntry->mTbsInfoArray[sTbsId] = NULL;
            sTbsInfo = NULL;
        }
        else
        {
            STL_TRY( smfFirstDatafile( sTbsId,
                                       &sDatafileId,
                                       &sDatafileExist,
                                       aEnv ) == STL_SUCCESS );

            while( sDatafileExist == STL_TRUE )
            {
                STL_DASSERT( smfGetDatafileState(sTbsId, sDatafileId) != SMF_DATAFILE_STATE_INIT );
                
                if( smfGetDatafileState(sTbsId, sDatafileId) == SMF_DATAFILE_STATE_DROPPING )
                {
                    sDatafileInfo = sTbsInfo->mDatafileInfoArray[sDatafileId];
                    
                    STL_TRY( knlLogMsg( NULL,
                                        KNL_ENV(aEnv),
                                        KNL_LOG_LEVEL_INFO,
                                        ".... TABLESPACE(%d), DATAFILE(%s) : Dropped Datafile\n",
                                        sTbsId,
                                        sDatafileInfo->mDatafilePersData.mName )
                             == STL_SUCCESS );

                    /* Memory에 적재된 Tablespace datafile을 제거한다. */
                    STL_TRY( smpShrinkDatafile( sTbsId,
                                                sDatafileId,
                                                aEnv ) == STL_SUCCESS );

                    STL_TRY( smgFreeShmMem4Mount( sDatafileInfo,
                                                  aEnv )
                             == STL_SUCCESS );

                    sTbsInfo->mDatafileInfoArray[sDatafileId] = NULL;
                }
                
                STL_TRY( smfNextDatafile( sTbsId,
                                          &sDatafileId,
                                          &sDatafileExist,
                                          aEnv ) == STL_SUCCESS );
            }
        }

        if( (sTbsInfo != NULL) &&
            (smfGetTbsState( sTbsId ) != SMF_TBS_STATE_AGING) )
        {
            STL_TRY( smfFirstDatafile( sTbsId,
                                       &sDatafileId,
                                       &sDatafileExist,
                                       aEnv ) == STL_SUCCESS );

            while( sDatafileExist == STL_TRUE )
            {
                STL_DASSERT( smfGetDatafileState(sTbsId, sDatafileId) != SMF_DATAFILE_STATE_INIT );
                
                if( (smfGetDatafileState(sTbsId, sDatafileId) != SMF_DATAFILE_STATE_AGING) &&
                    (smfGetDatafileState(sTbsId, sDatafileId) != SMF_DATAFILE_STATE_DROPPED) )
                {
                    sDatafileInfo = sTbsInfo->mDatafileInfoArray[sDatafileId];

                    /**
                     * Datafile 생성 중 CREATING 상태로 controlfile을 저장한 후
                     * CREATED로 변경하지 못한 상태에서 장애가 발생하였을 경우
                     * Refine 시 CREATED 상태로 설정한다.
                     */
                    if( sDatafileInfo->mDatafilePersData.mState == SMF_DATAFILE_STATE_CREATING )
                    {
                        STL_TRY( knlLogMsg( NULL,
                                            KNL_ENV(aEnv),
                                            KNL_LOG_LEVEL_INFO,
                                            ".... TABLESPACE(%d), DATAFILE(%s) : CREATING -> CREATED\n",
                                            sTbsId,
                                            sDatafileInfo->mDatafilePersData.mName )
                                 == STL_SUCCESS );

                        sDatafileInfo->mDatafilePersData.mState = SMF_DATAFILE_STATE_CREATED;
                    }
                }
                
                STL_TRY( smfNextDatafile( sTbsId,
                                          &sDatafileId,
                                          &sDatafileExist,
                                          aEnv ) == STL_SUCCESS );
            }
        }
        
        STL_TRY( smfNextTbs( &sTbsId,
                             &sTbsExist,
                             aEnv ) == STL_SUCCESS );
    }
    
    STL_TRY( smfSaveCtrlFile( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Datafile의 Pch Array의 물리적 시작주소를 얻는다.
 * @param[in] aTbsId 대상 Tablespace 아이디
 * @param[in] aDatafileId 대상 Datafile 아이디
 * @param[out] aPchArray 찾은 Pch Array 포인터
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smfGetPchArray( smlTbsId         aTbsId,
                          smlDatafileId    aDatafileId,
                          void          ** aPchArray,
                          smlEnv         * aEnv )
{
    smfTbsInfo     * sTbsInfo;
    knlLogicalAddr   sPchArray;
    
    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];
    sPchArray = sTbsInfo->mDatafileInfoArray[aDatafileId]->mPchArray;

    STL_TRY( knlGetPhysicalAddr( sPchArray,
                                 aPchArray,
                                 (knlEnv*)aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Datafile의 Pch Array의 물리적 시작주소를 얻는다.
 * @param[in] aTbsId 대상 Tablespace 아이디
 * @param[in] aDatafileId 대상 Datafile 아이디
 * @param[in] aPageSeqId 대상 Page Sequence Id
 * @param[out] aPch 찾은 Pch Array Physical 포인터
 * @param[out] aFrame 찾은 Frame Physical 포인터
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smfGetPchAndFrame( smlTbsId         aTbsId,
                             smlDatafileId    aDatafileId,
                             stlInt64         aPageSeqId,
                             void          ** aPch,
                             void          ** aFrame,
                             smlEnv         * aEnv )
{
    smfDatafileInfo * sDatafileInfo;
    void            * sPhysicalChunk;
    stlBool           sIsValid = STL_TRUE;
    
    sDatafileInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId]->mDatafileInfoArray[aDatafileId];

    /**
     * Datafile Information에 설정된 shared memory sequence가 Attach될 당시
     * 설정된 sequence가 다르다면 shared memory를 reset할 필요가 있다.
     */
    
    STL_TRY( knlIsValidShmSegment( sDatafileInfo->mShmIndex,
                                   &sIsValid,
                                   KNL_ENV(aEnv) )
             == STL_SUCCESS );

    if( sIsValid ==  STL_FALSE )
    {
        /**
         *  ----------------------------------------------------> t
         *        ^                 ^                       ^
         *        |                 |                       |
         *
         *   (session 1)        (session 2)          (session 1)
         *   start select    drop tablespace &       recompile &
         *                   create tablespace &     fetch <== access wrong shared segment
         *                   create same table       
         */
        STL_TRY( knlReattachShmSegment( sDatafileInfo->mShmIndex,
                                        KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    
    STL_TRY( knlGetPhysicalAddr( sDatafileInfo->mChunk,
                                 &sPhysicalChunk,
                                 (knlEnv*)aEnv ) == STL_SUCCESS );

    *aFrame = ( sPhysicalChunk + (SMP_PAGE_SIZE * aPageSeqId) );
    *aPch   = ( sPhysicalChunk +
                (SMP_PAGE_SIZE * sDatafileInfo->mMaxPageCount) +
                (STL_SIZEOF(smpCtrlHeader) * aPageSeqId) );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Tablespace Handle을 반환한다.
 * @param[in] aTbsId 대상 Tablespace Identifier
 * @return 찾은 Tablespace Handle
 */
void * smfGetTbsHandle( smlTbsId aTbsId )
{
    return gSmfWarmupEntry->mTbsInfoArray[aTbsId];
}

/**
 * @brief 주어진 Tablespace Handle로부터 Tablespace Identifier를 반환한다.
 * @param[in] aTbsHandle 대상 Tablespace Handle
 * @return Tablespace Identifier
 */
smlTbsId smfGetTbsIdByHandle( void * aTbsHandle )
{
    smfTbsInfo * sTbsInfo;

    sTbsInfo = (smfTbsInfo*)aTbsHandle;

    return sTbsInfo->mTbsPersData.mTbsId;
}

/**
 * @brief 주어진 테이블스페이스가 존재하는지 여부를 반환한다.
 * @param[in] aTbsId 대상 Tablespace 아이디
 * @return 존재 여부
 */
stlBool smfExistTbs( smlTbsId aTbsId )
{
    if( gSmfWarmupEntry->mTbsInfoArray[aTbsId] == NULL )
    {
        return STL_FALSE;
    }

    return STL_TRUE;
}

/**
 * @brief 주어진 테이블스페이스가 유효한지 여부를 반환한다.
 * @param[in] aTbsId 대상 Tablespace 아이디
 * @return 존재 여부
 */
stlBool smfIsValidTbs( smlTbsId aTbsId )
{
    smfTbsInfo * sTbsInfo;
    
    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];
    
    if( sTbsInfo == NULL )
    {
        return STL_FALSE;
    }

    if( sTbsInfo->mTbsPersData.mState == SMF_TBS_STATE_DROPPED )
    {
        return STL_FALSE;
    }
    
    return STL_TRUE;
}

/**
 * @brief 주어진 데이터파일이 존재하는지 여부를 반환한다.
 * @param[in] aTbsId      대상 Tablespace 아이디
 * @param[in] aDatafileId 대상 Datafile 아이디
 * @return 존재 여부
 */
stlBool smfExistDatafile( smlTbsId      aTbsId,
                          smlDatafileId aDatafileId )
{
    smfTbsInfo * sTbsInfo;
    
    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];
    
    if( sTbsInfo == NULL )
    {
        return STL_FALSE;
    }

    if( sTbsInfo->mDatafileInfoArray[aDatafileId] == NULL )
    {
        return STL_FALSE;
    }

    return STL_TRUE;
}

/**
 * @brief 주어진 테이블스페이스와 데이터파일이 REDO 가능한 상태인지 검사한다.
 * @param[in] aTbsId      대상 Tablespace 아이디
 * @param[in] aDatafileId 대상 Datafile 아이디
 * @return 존재 여부
 */
stlBool smfIsRedoableDatafile( smlTbsId      aTbsId,
                               smlDatafileId aDatafileId )
{
    smfTbsInfo * sTbsInfo;
    
    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];
    
    /**
     * 아래 경우는 REDO 하지 않는다.
     * - 존재하지 않는 테이블스페이스 또는 데이터파일
     *  (Controlfile에 없지만 redo log가 삭제된 Tablespace/Datafile을 참조하고 있을수도 있다)
     * - Restart Recovery 시 OFFLINE TABLESPACE
     * - AGING 상태의 TABLESPACE 또는 DATAFILE
     * - DROPPED 상태의 TABLESPACE 또는 DATAFILE
     */
    
    if( sTbsInfo == NULL )
    {
        return STL_FALSE;
    }

    STL_DASSERT( smfGetTbsState( aTbsId ) != SMF_TBS_STATE_INIT );
                
    if( smfGetTbsState( aTbsId ) == SMF_TBS_STATE_AGING )
    {
        return STL_FALSE;
    }

    if( smfGetTbsState( aTbsId ) == SMF_TBS_STATE_DROPPED )
    {
        return STL_FALSE;
    }

    /**
     * Offline Tablespace라도 Media Recovery 중에는 Recovery 대상이다.
     */
    if( (SMF_IS_ONLINE_TBS( aTbsId ) == STL_FALSE) &&
        (SMF_IS_TBS_PROCEED_MEDIA_RECOVERY( aTbsId ) == STL_FALSE) )
    {
        return STL_FALSE;
    }
    
    if( sTbsInfo->mDatafileInfoArray[aDatafileId] == NULL )
    {
        return STL_FALSE;
    }

    STL_DASSERT( smfGetDatafileState( aTbsId, aDatafileId ) != SMF_DATAFILE_STATE_INIT );
    
    if( smfGetDatafileState( aTbsId, aDatafileId ) == SMF_DATAFILE_STATE_AGING )
    {
        return STL_FALSE;
    }
    
    if( smfGetDatafileState( aTbsId, aDatafileId ) == SMF_DATAFILE_STATE_DROPPED )
    {
        return STL_FALSE;
    }
    
    return STL_TRUE;
}

/**
 * @brief 주어진 데이터파일이 유효한지 여부를 반환한다.
 * @param[in] aTbsId      대상 Tablespace 아이디
 * @param[in] aDatafileId 대상 Datafile 아이디
 * @return 존재 여부
 */
stlBool smfIsValidDatafile( smlTbsId      aTbsId,
                            smlDatafileId aDatafileId )
{
    smfTbsInfo      * sTbsInfo;
    smfDatafileInfo * sDatafileInfo;
    
    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];
    STL_DASSERT( sTbsInfo != NULL );
    sDatafileInfo = sTbsInfo->mDatafileInfoArray[aDatafileId];

    if( sDatafileInfo == NULL )
    {
        return STL_FALSE;
    }

    STL_DASSERT( sDatafileInfo->mDatafilePersData.mState != SMF_DATAFILE_STATE_INIT );
    
    if( sDatafileInfo->mDatafilePersData.mState == SMF_DATAFILE_STATE_DROPPED )
    {
        return STL_FALSE;
    }
    
    return STL_TRUE;
}

stlUInt64 smfGetTimestamp( smlTbsId      aTbsId,
                           smlDatafileId aDatafileId )
{
    smfTbsInfo * sTbsInfo;
    
    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];

    return sTbsInfo->mDatafileInfoArray[aDatafileId]->mDatafilePersData.mTimestamp;
}

/**
 * @brief 주어진 데이터파일의 최대 페이지 개수를 얻는다.
 * @param[in] aTbsId 대상 Tablespace 아이디
 * @param[in] aDatafileId 대상 Datafile 아이디
 * @return 최대 페이지 개수 
 */
stlUInt32 smfGetMaxPageCount( smlTbsId      aTbsId,
                              smlDatafileId aDatafileId )
{
    smfTbsInfo * sTbsInfo;
    
    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];

    return sTbsInfo->mDatafileInfoArray[aDatafileId]->mMaxPageCount;
}

/**
 * @brief MOUNT 단계에서 주어진 데이터파일의 최대 페이지 개수를 얻는다.
 * @param[in] aTbsId 대상 Tablespace 아이디
 * @param[in] aDatafileId 대상 Datafile 아이디
 * @return 최대 페이지 개수 
 */
stlUInt32 smfGetMaxPageCountAtMount( smlTbsId      aTbsId,
                                     smlDatafileId aDatafileId )
{
    smfTbsInfo * sTbsInfo;
    
    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];

    return (sTbsInfo->mDatafileInfoArray[aDatafileId]->mDatafilePersData.mSize) / SMP_PAGE_SIZE;
}

/**
 * @brief 주어진 테이블스페이스의 상태 정보를 구한다.
 * @param[in] aTbsId 대상 Tablespace 아이디
 * @return 테이블스페이스 상태정보
 * @see smfTbsState
 */
inline stlUInt16 smfGetTbsState( smlTbsId aTbsId )
{
    smfTbsInfo * sTbsInfo;
    
    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];

    return sTbsInfo->mTbsPersData.mState;
}

/**
 * @brief 주어진 데이터파일의 상태 정보를 구한다.
 * @param[in] aTbsId 대상 Tablespace 아이디
 * @param[in] aDatafileId 대상 Datafile 아이디
 * @return 데이터파일 상태정보
 * @see smfDatafileState
 */
inline stlUInt16 smfGetDatafileState( smlTbsId      aTbsId,
                                      smlDatafileId aDatafileId )
{
    smfDatafileInfo * sDatafileInfo;
    
    sDatafileInfo = smfGetDatafileInfo( aTbsId, aDatafileId );

    return sDatafileInfo->mDatafilePersData.mState;
}

/**
 * @brief 데이터파일 이름을 구한다.
 * @param[in] aTbsId 대상 Tablespace 아이디
 * @param[in] aDatafileId 대상 Datafile 아이디
 * @return 데이터파일 이름
 */
inline stlChar* smfGetDatafileName( smlTbsId      aTbsId,
                                    smlDatafileId aDatafileId )
{
    smfDatafileInfo * sDatafileInfo;
    
    sDatafileInfo = smfGetDatafileInfo( aTbsId, aDatafileId );

    return sDatafileInfo->mDatafilePersData.mName;
}

/**
 * @brief 주어진 데이터파일의 공유 메모리 인덱스를 얻는다.
 * @param[in] aTbsId 대상 Tablespace 아이디
 * @param[in] aDatafileId 대상 Datafile 아이디
 * @return 공유 메모리 인덱스
 */
stlUInt16 smfGetShmIndex( smlTbsId      aTbsId,
                          smlDatafileId aDatafileId )
{
    smfTbsInfo * sTbsInfo;
    
    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];

    return sTbsInfo->mDatafileInfoArray[aDatafileId]->mShmIndex;
}

/**
 * @brief 주어진 데이터파일의 공유 메모리 논리적 주소를 얻는다.
 * @param[in] aTbsId 대상 Tablespace 아이디
 * @param[in] aDatafileId 대상 Datafile 아이디
 * @return 공유 메모리 인덱스
 */
stlUInt64 smfGetShmChunk( smlTbsId      aTbsId,
                          smlDatafileId aDatafileId )
{
    smfTbsInfo * sTbsInfo;
    
    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];

    return sTbsInfo->mDatafileInfoArray[aDatafileId]->mChunk;
}

/**
 * @brief 데이터베이스내의 첫번째 테이블스페이스 아이디를 얻는다.
 * @param[out] aTbsId 찾은 Tablespace 아이디
 * @param[out] aExist 찾은 여부
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smfFirstTbs( smlTbsId * aTbsId,
                       stlBool  * aExist,
                       smlEnv   * aEnv )
{
    stlUInt32    i;
    smfTbsInfo * sTbsInfo;
    
    *aExist = STL_FALSE;
    
    for( i = 0; i < SML_TBS_MAX_COUNT; i++ )
    {
        sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[i];
        
        if( sTbsInfo == NULL )
        {
            continue;
        }
        
        if( (sTbsInfo->mTbsPersData.mState == SMF_TBS_STATE_INIT) ||
            (sTbsInfo->mTbsPersData.mState == SMF_TBS_STATE_DROPPED) )
        {
            continue;
        }

        *aExist = STL_TRUE;
        *aTbsId = i;
        break;
    }
    
    return STL_SUCCESS;
}

/**
 * @brief 데이터베이스내의 다음 테이블스페이스 아이디를 얻는다.
 * @param[in,out] aTbsId 찾고자하는 위치와 찾은 Tablespace 아이디
 * @param[out] aExist 찾은 여부
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smfNextTbs( smlTbsId * aTbsId,
                      stlBool  * aExist,
                      smlEnv   * aEnv )
{
    stlUInt32    i;
    smfTbsInfo * sTbsInfo;
    
    *aExist = STL_FALSE;
    
    for( i = *aTbsId + 1; i < SML_TBS_MAX_COUNT; i++ )
    {
        sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[i];
        
        if( sTbsInfo == NULL )
        {
            continue;
        }
        
        if( (sTbsInfo->mTbsPersData.mState == SMF_TBS_STATE_INIT) ||
            (sTbsInfo->mTbsPersData.mState == SMF_TBS_STATE_DROPPED) )
        {
            continue;
        }

        *aExist = STL_TRUE;
        *aTbsId = i;
        break;
    }
    
    return STL_SUCCESS;
}

/**
 * @brief 테이블스페이스내의 첫번째 Datafile 아이디를 얻는다.
 * @param[in] aTbsId 대상 Tablespace 아이디
 * @param[out] aDatafile 찾은 Datafile 아이디
 * @param[out] aExist 찾은 여부
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smfFirstDatafile( smlTbsId        aTbsId,
                            smlDatafileId * aDatafile,
                            stlBool       * aExist,
                            smlEnv        * aEnv )
{
    smfDatafileInfo * sDatafileInfo;
    smfTbsInfo      * sTbsInfo;
    stlUInt32         i;
    
    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];
    *aExist = STL_FALSE;
    
    for( i = 0; i < SML_DATAFILE_MAX_COUNT; i++ )
    {
        sDatafileInfo = sTbsInfo->mDatafileInfoArray[i];
        
        if( sDatafileInfo == NULL )
        {
            continue;
        }

        if( (sDatafileInfo->mDatafilePersData.mState == SMF_DATAFILE_STATE_DROPPED) ||
            (sDatafileInfo->mDatafilePersData.mState == SMF_DATAFILE_STATE_INIT) )
        {
            continue;
        }

        *aExist = STL_TRUE;
        *aDatafile = i;
        break;
    }

    return STL_SUCCESS;
}

/**
 * @brief 테이블스페이스내의 다음 Datafile 아이디를 얻는다.
 * @param[in] aTbsId 대상 Tablespace 아이디
 * @param[in,out] aDatafile 찾은 Datafile 아이디
 * @param[out] aExist 찾은 여부
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smfNextDatafile( smlTbsId        aTbsId,
                           smlDatafileId * aDatafile,
                           stlBool       * aExist,
                           smlEnv        * aEnv )
{
    smfDatafileInfo * sDatafileInfo;
    smfTbsInfo      * sTbsInfo;
    stlUInt32         i;

    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];
    *aExist = STL_FALSE;

    for( i = *aDatafile + 1; i < SML_DATAFILE_MAX_COUNT; i++ )
    {
        sDatafileInfo = sTbsInfo->mDatafileInfoArray[i];
        
        if( sDatafileInfo == NULL )
        {
            continue;
        }

        if( (sDatafileInfo->mDatafilePersData.mState == SMF_DATAFILE_STATE_DROPPED) ||
            (sDatafileInfo->mDatafilePersData.mState == SMF_DATAFILE_STATE_INIT) )
        {
            continue;
        }

        *aExist = STL_TRUE;
        *aDatafile = i;
        break;
    }

    return STL_SUCCESS;
}

/**
 * @brief 주어진 데이터파일 이름에 해당하는 Datafile 아이디를 얻는다.
 * @param[in] aTbsId 대상 Tablespace 아이디
 * @param[in] aDatafileName 찾을 Datafile 이름
 * @param[out] aDatafileId 찾은 Datafile 아이디
 * @param[out] aFound 찾은 여부
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smfFindDatafileId( smlTbsId        aTbsId,
                             stlChar       * aDatafileName,
                             smlDatafileId * aDatafileId,
                             stlBool       * aFound,
                             smlEnv        * aEnv )
{
    smlDatafileId     sDatafileId = SML_INVALID_DATAFILE_ID;
    smfDatafileInfo * sDatafileInfo;
    stlBool           sExist = STL_FALSE;

    *aFound = STL_FALSE;
    
    STL_TRY( smfFirstDatafile( aTbsId,
                               &sDatafileId,
                               &sExist,
                               aEnv ) == STL_SUCCESS );

    while( sExist == STL_TRUE )
    {
        sDatafileInfo = smfGetDatafileInfo( aTbsId, sDatafileId );

        if( stlStrcmp( sDatafileInfo->mDatafilePersData.mName, aDatafileName ) == 0 )
        {
            *aFound = STL_TRUE;
            *aDatafileId = sDatafileId;
        }
        
        STL_TRY( smfNextDatafile( aTbsId,
                                  &sDatafileId,
                                  &sExist,
                                  aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Tablespace에서 확장 가능한 Datafile을 찾는다.
 * @param[in] aTbsId 대상 Tablespace Identifier
 * @param[out] aDatafileId 확장 가능한 Datafile의 Identifier
 * @param[out] aFound 찾기 여부
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smfFindExtendibleDatafile( smlTbsId          aTbsId,
                                     smlDatafileId    *aDatafileId,
                                     stlBool          *aFound,
                                     smlEnv           *aEnv )
{
    smlDatafileId     sDatafileId = SML_INVALID_DATAFILE_ID;
    smfDatafileInfo * sDatafileInfo;
    stlBool           sExist = STL_FALSE;

    *aFound = STL_FALSE;
    
    STL_TRY( smfFirstDatafile( aTbsId,
                               &sDatafileId,
                               &sExist,
                               aEnv ) == STL_SUCCESS );

    while( sExist == STL_TRUE )
    {
        if( (smfGetDatafileState( aTbsId, sDatafileId ) == SMF_DATAFILE_STATE_DROPPING) ||
            (smfGetDatafileState( aTbsId, sDatafileId ) == SMF_DATAFILE_STATE_CREATING) ||
            (smfGetDatafileState( aTbsId, sDatafileId ) == SMF_DATAFILE_STATE_AGING) )
        {
            STL_TRY( smfNextDatafile( aTbsId,
                                      &sDatafileId,
                                      &sExist,
                                      aEnv ) == STL_SUCCESS );
            continue;
        }
        
        sDatafileInfo = smfGetDatafileInfo( aTbsId, sDatafileId );

        if( sDatafileInfo->mDatafilePersData.mAutoExtend == STL_TRUE )
        {
            if( sDatafileInfo->mDatafilePersData.mMaxSize >
                (sDatafileInfo->mDatafilePersData.mSize + sDatafileInfo->mDatafilePersData.mNextSize) )
            {
                *aFound = STL_TRUE;
                *aDatafileId = sDatafileId;
                break;
            }
        }
        
        STL_TRY( smfNextDatafile( aTbsId,
                                  &sDatafileId,
                                  &sExist,
                                  aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Datafile을 확장한다.
 * @param[in] aMiniTrans Mini transaction의 포인터
 * @param[in] aTbsId 대상 Tablespace Identifier
 * @param[in] aDatafileId 확장할 Datafile의 Identifier
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smfExtendDatafile( smxmTrans        *aMiniTrans,
                             smlTbsId          aTbsId,
                             smlDatafileId     aDatafileId,
                             smlEnv           *aEnv )
{
    stlUInt32         sTbsTypeId;
    
    sTbsTypeId = SML_TBS_DEVICE_TYPE_ID( aTbsId );

    STL_TRY( gDatafileFuncs[sTbsTypeId]->mExtend( aMiniTrans,
                                                  aTbsId,
                                                  aDatafileId,
                                                  aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 주어진 페이지를 디스크에 저장한다.
 * @param[in] aTbsId 대상 Tablespace 아이디
 * @param[in] aPageId Target 페이지 아이디
 * @param[in] aPage  Target Page
 * @param[in] aPageCount 저장할 페이지 개수
 * @param[in] aChecksumType Checksum type
 * @param[out] aTakenOffline Offline 되었는지 여부
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smfWritePage( smlTbsId     aTbsId,
                        smlPid       aPageId,
                        void       * aPage,
                        stlInt32     aPageCount,
                        stlInt64     aChecksumType,
                        stlBool    * aTakenOffline,
                        smlEnv     * aEnv )
{
    stlUInt32         sTbsTypeId;
    smfTbsInfo      * sTbsInfo;
    smfDatafileInfo * sDatafileInfo;
    stlInt64          i;
    
    *aTakenOffline = STL_FALSE;
    
    sTbsTypeId = SML_TBS_DEVICE_TYPE_ID( aTbsId );
    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];

    /**
     * Drop된 테이블스페이스라면 무시한다.
     */
    STL_TRY_THROW( sTbsInfo != NULL, RAMP_SUCCESS );
    STL_TRY_THROW( sTbsInfo->mTbsPersData.mState != SMF_TBS_STATE_DROPPED, RAMP_SUCCESS );
    STL_TRY_THROW( sTbsInfo->mTbsPersData.mState != SMF_TBS_STATE_AGING, RAMP_SUCCESS );
    STL_TRY_THROW( SML_TBS_IS_MEMORY_TEMPORARY(sTbsInfo->mTbsPersData.mAttr) == STL_FALSE, RAMP_SUCCESS );

    sDatafileInfo = sTbsInfo->mDatafileInfoArray[SMP_DATAFILE_ID(aPageId)];

    /**
     * Drop된 데이터파일이라면 무시한다.
     */
    STL_TRY_THROW( sDatafileInfo != NULL, RAMP_SUCCESS );
    STL_DASSERT( sDatafileInfo->mDatafilePersData.mState != SMF_DATAFILE_STATE_INIT );
    STL_TRY_THROW( sDatafileInfo->mDatafilePersData.mState != SMF_DATAFILE_STATE_DROPPED,
                   RAMP_SUCCESS );
    STL_TRY_THROW( sDatafileInfo->mDatafilePersData.mState != SMF_DATAFILE_STATE_AGING,
                   RAMP_SUCCESS );

    for( i = 0; i < aPageCount; i++ )
    {
        STL_TRY( smpMakePageChecksum( aPage + (SMP_PAGE_SIZE * i),
                                      aChecksumType,
                                      aEnv )
                 == STL_SUCCESS );
    }
    
    /**
     * 디스크에 기록
     */
    if( gDatafileFuncs[sTbsTypeId]->mWritePage( sTbsInfo,
                                                sDatafileInfo,
                                                SMP_PAGE_SEQ_ID(aPageId),
                                                aPageCount,
                                                aPage,
                                                aEnv )
        == STL_FAILURE )
    {
        /**
         * Drop된 테이블스페이스라면 무시한다.
         */
        if( (sTbsInfo == NULL) ||
            (sTbsInfo->mTbsPersData.mState == SMF_TBS_STATE_DROPPED) ||
            (sTbsInfo->mTbsPersData.mState == SMF_TBS_STATE_AGING) )
        {
            stlPopError( KNL_ERROR_STACK(aEnv) );
            STL_THROW( RAMP_SUCCESS );
        }
        
        sDatafileInfo = sTbsInfo->mDatafileInfoArray[SMP_DATAFILE_ID(aPageId)];

        /**
         * Drop된 데이터파일이라면 무시한다.
         */
        if( (sDatafileInfo == NULL) ||
            (sDatafileInfo->mDatafilePersData.mState == SMF_DATAFILE_STATE_DROPPED) ||
            (sDatafileInfo->mDatafilePersData.mState == SMF_DATAFILE_STATE_AGING) )
        {
            stlPopError( KNL_ERROR_STACK(aEnv) );
            STL_THROW( RAMP_SUCCESS );
        }

        /**
         * 그렇지 않은 경우는 파일에 문제가 발생한 경우이다.
         */

        if( (aTbsId == SML_MEMORY_DICT_SYSTEM_TBS_ID) ||
            (aTbsId == SML_MEMORY_UNDO_SYSTEM_TBS_ID) )
        {
            /**
             * Offline이 불가능한 테이블 스페이스라면 시스템을 비정상 종료시킨다.
             */

            knlSystemFatal( "can't write page to datafile", KNL_ENV(aEnv) );
        }
        else
        {
            /**
             * Offline이 가능한 테이블 스페이스라면
             */
            
            stlPushError( STL_ERROR_LEVEL_WARNING,
                          SML_ERRCODE_TABLESPACE_OFFLINE_ABNORMALLY,
                          NULL,
                          KNL_ERROR_STACK(aEnv),
                          sTbsInfo->mTbsPersData.mName );

            STL_TRY( knlLogMsg( NULL,
                                KNL_ENV(aEnv),
                                KNL_LOG_LEVEL_WARNING,
                                "[FLUSHER] failed to write page - The tablespace '%s' offlined\n",
                                sTbsInfo->mTbsPersData.mName )
                     == STL_SUCCESS );

            if( ((SMF_IS_ONLINE_TBS( aTbsId ) == STL_FALSE) &&
                 (SMF_GET_TBS_OFFLINE_STATE( aTbsId ) == SMF_OFFLINE_STATE_CONSISTENT)) ||
                (SMF_IS_ONLINE_TBS( aTbsId ) == STL_TRUE) )
            {
                /**
                 * setting offline
                 */

                STL_TRY( smfOfflineTbs( NULL,
                                        aTbsId,
                                        SML_OFFLINE_BEHAVIOR_IMMEDIATE,
                                        aEnv )
                         == STL_SUCCESS );
            }

            *aTakenOffline = STL_TRUE;
        }
    }

    gSmfWarmupEntry->mLastFlushedTbsId = aTbsId;
    gSmfWarmupEntry->mLastFlushedPid = aPageId;
    gSmfWarmupEntry->mFlushedPageCount = aPageCount;
    gSmfWarmupEntry->mPageIoCount++;

    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    /**
     * page flushing 도중 실패해서는 안된다.
     */
    KNL_ENTER_CRITICAL_SECTION( aEnv );
    STL_ASSERT( STL_FALSE );
    KNL_LEAVE_CRITICAL_SECTION( aEnv );

    return STL_FAILURE;
}

/**
 * @brief Datafile Layer의 System information을 얻는다.
 * @param[out] aSystemInfo System Information
 */
void smfGetSystemInfo( smfSystemInfo * aSystemInfo )
{
    aSystemInfo->mDataAccessMode = gSmfWarmupEntry->mDataAccessMode;
    aSystemInfo->mLastFlushedTbsId = gSmfWarmupEntry->mLastFlushedTbsId;
    aSystemInfo->mLastFlushedPid = gSmfWarmupEntry->mLastFlushedPid;
    aSystemInfo->mFlushedPageCount = gSmfWarmupEntry->mFlushedPageCount;
    aSystemInfo->mPageIoCount = gSmfWarmupEntry->mPageIoCount;
}

/**
 * @brief 주어진 테이블스페이스의 한 Extent가 가지는 Page 개수를 얻는다.
 * @param[in] aTbsId 대상 Tablespace 아이디
 * @return 한 Extent가 가지는 페이지 개수
 */
inline stlUInt16 smfGetPageCountInExt( smlTbsId aTbsId )
{
    STL_ASSERT( gSmfWarmupEntry->mTbsInfoArray[aTbsId] != NULL );
    return (gSmfWarmupEntry->mTbsInfoArray[aTbsId]->mTbsPersData.mPageCountInExt);
}

/**
 * @brief 첫번째 Tablespace 정보를 얻는다.
 * @param[out] aTbsPersData  대상 Tablespace 정보
 * @param[out] aIterator     Tablespace Iterator 포인터
 * @param[in,out] aEnv       Environment 정보
 */
stlStatus smfFirstTbsPersData( smfTbsPersData  * aTbsPersData,
                               void           ** aIterator,
                               smlEnv          * aEnv )
{
    stlUInt32    i;
    smfTbsInfo * sTbsInfo;
    
    *aIterator = NULL;
    
    for( i = 0; i < SML_TBS_MAX_COUNT; i++ )
    {
        sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[i];
        
        if( sTbsInfo == NULL )
        {
            continue;
        }
        
        if( (sTbsInfo->mTbsPersData.mState == SMF_TBS_STATE_DROPPED) ||
            (sTbsInfo->mTbsPersData.mState == SMF_TBS_STATE_INIT) )
        {
            continue;
        }

        *aIterator = (void*)sTbsInfo;
        *aTbsPersData = sTbsInfo->mTbsPersData;
        break;
    }
    
    return STL_SUCCESS;
}

/**
 * @brief 주어진 aIterator의 다음(next) Tablespace 정보를 얻는다.
 * @param[in,out] aIterator     Tablespace Iterator 포인터
 * @param[out]    aTbsPersData  대상 Tablespace 정보
 * @param[in,out] aEnv          Environment 정보
 */
stlStatus smfNextTbsPersData( smfTbsPersData  * aTbsPersData,
                              void           ** aIterator,
                              smlEnv          * aEnv )
{
    stlUInt32    i;
    smfTbsInfo * sTbsInfo;

    sTbsInfo = (smfTbsInfo*)(*aIterator);
    *aIterator = NULL;
    
    for( i = sTbsInfo->mTbsPersData.mTbsId + 1; i < SML_TBS_MAX_COUNT; i++ )
    {
        sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[i];
        
        if( sTbsInfo == NULL )
        {
            continue;
        }
        
        if( (sTbsInfo->mTbsPersData.mState == SMF_TBS_STATE_DROPPED) ||
            (sTbsInfo->mTbsPersData.mState == SMF_TBS_STATE_INIT) )
        {
            continue;
        }

        STL_DASSERT( sTbsInfo->mTbsPersData.mTbsId == i );
        
        *aIterator = (void*)sTbsInfo;
        *aTbsPersData = sTbsInfo->mTbsPersData;
        break;
    }
    
    return STL_SUCCESS;
}

stlStatus smfValidateTbs( smlEnv * aEnv )
{
    stlUInt32    i;
    smfTbsInfo * sTbsInfo;

    for( i = 0; i < SML_TBS_MAX_COUNT; i++ )
    {
        sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[i];
        
        if( sTbsInfo == NULL )
        {
            continue;
        }
        
        if( (sTbsInfo->mTbsPersData.mState == SMF_TBS_STATE_DROPPED) ||
            (sTbsInfo->mTbsPersData.mState == SMF_TBS_STATE_INIT) )
        {
            continue;
        }

        STL_ASSERT( sTbsInfo->mTbsPersData.mTbsId == i );
    }
    
    return STL_SUCCESS;
}


/**
 * @brief 주어진 테이블스페이스의 이름을 얻는다.
 * @param[in]     aTbsId     Tablespace Identifier
 * @param[out]    aTbsName   Tablespace Name
 * @param[in,out] aEnv       Environment 정보
 */
stlStatus smfGetTbsName( smlTbsId   aTbsId,
                         stlChar  * aTbsName,
                         smlEnv   * aEnv )
{
    smfTbsInfo  * sTbsInfo;
    stlInt32      sState = 0;

    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];
    
    if( sTbsInfo == NULL )
    {
        STL_DASSERT( STL_FALSE );
        stlStrcpy( aTbsName, "" );
    }
    
    STL_TRY( smfAcquireTablespace( sTbsInfo, aEnv ) == STL_SUCCESS );
    sState = 1;

    stlStrcpy( aTbsName, sTbsInfo->mTbsPersData.mName );
    
    sState = 0;
    STL_TRY( smfReleaseTablespace( sTbsInfo, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smfReleaseTablespace( sTbsInfo, aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief 주어진 datafile의 Shared memory 상태를 얻는다.
 * @param[in] aTbsId      Tablespace Identifier
 * @param[in] aDatafileId Datafile Identifier
 */
stlInt16 smfGetDatafileShmState( smlTbsId       aTbsId,
                                 smlDatafileId  aDatafileId )
{
    smfTbsInfo      * sTbsInfo;
    smfDatafileInfo * sDatafileInfo;

    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];
    STL_DASSERT( sTbsInfo != NULL );
    
    sDatafileInfo = sTbsInfo->mDatafileInfoArray[aDatafileId];
    STL_DASSERT( sDatafileInfo != NULL );

    return (sDatafileInfo->mShmState);
}

/**
 * @brief 주어진 datafile의 Shared memory 상태를 설정한다.
 * @param[in] aTbsId      Tablespace Identifier
 * @param[in] aDatafileId Datafile Identifier
 * @param[in] aShmState   Shared memory state
 */
void smfSetDatafileShmState( smlTbsId       aTbsId,
                             smlDatafileId  aDatafileId,
                             stlInt16       aShmState )
{
    smfTbsInfo      * sTbsInfo;
    smfDatafileInfo * sDatafileInfo;

    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];
    STL_DASSERT( sTbsInfo != NULL );
    
    sDatafileInfo = sTbsInfo->mDatafileInfoArray[aDatafileId];
    STL_DASSERT( sDatafileInfo != NULL );

    sDatafileInfo->mShmState = aShmState;
}

/**
 * @brief 주어진 datafile의 Page Corruption Count를 설정한다.
 * @param[in] aTbsId      Tablespace Identifier
 * @param[in] aDatafileId Datafile Identifier
 * @param[in] aCorruptionCount Page Corruption Count
 */
void smfSetDatafileCorruptionCount( smlTbsId       aTbsId,
                                    smlDatafileId  aDatafileId,
                                    stlUInt32      aCorruptionCount )
{
    smfTbsInfo      * sTbsInfo;
    smfDatafileInfo * sDatafileInfo;

    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];
    STL_DASSERT( sTbsInfo != NULL );
    
    sDatafileInfo = sTbsInfo->mDatafileInfoArray[aDatafileId];
    STL_DASSERT( sDatafileInfo != NULL );

    sDatafileInfo->mCorruptionCount = aCorruptionCount;
}

/**
 * @brief 주어진 datafile의 Page Corruption Count를 얻는다.
 * @param[in] aTbsId      Tablespace Identifier
 * @param[in] aDatafileId Datafile Identifier
 */
stlUInt32 smfGetDatafileCorruptionCount( smlTbsId       aTbsId,
                                         smlDatafileId  aDatafileId )
{
    smfTbsInfo      * sTbsInfo;
    smfDatafileInfo * sDatafileInfo;

    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];
    STL_DASSERT( sTbsInfo != NULL );
    
    sDatafileInfo = sTbsInfo->mDatafileInfoArray[aDatafileId];
    STL_DASSERT( sDatafileInfo != NULL );

    return sDatafileInfo->mCorruptionCount;
}

stlInt16 smfGetIoSlaveId( smlTbsId      aTbsId,
                          smlDatafileId aDatafileId )
{
    smfTbsInfo      * sTbsInfo;
    smfDatafileInfo * sDatafileInfo;

    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];
    STL_DASSERT( sTbsInfo != NULL );
    
    sDatafileInfo = sTbsInfo->mDatafileInfoArray[aDatafileId];
    STL_DASSERT( sDatafileInfo != NULL );

    return sDatafileInfo->mIoGroupIdx;
}

/**
 * @brief DB의 모든 Datafile Header를 판독하여 최소의 Chkpt Lsn을 구한다.
 * @param[out] aChkptLid    Checkpoint Lid
 * @param[out] aMinChkptLsn Minimum Checkpoint Lsn
 * @param[in,out] aEnv      Environment 정보
 */
stlStatus smfGetMinChkptLsn( smrLid    * aChkptLid,
                             smrLsn    * aMinChkptLsn,
                             smlEnv    * aEnv )
{
    smlTbsId          sTbsId = SML_INVALID_TBS_ID;
    smrLsn            sMinLsn = SMR_INIT_LSN;
    smrLid            sTbsLid;
    smrLid            sChkptLid = SMR_INVALID_LID;
    smrLsn            sTbsMinLsn;
    stlBool           sExist;
    smfTbsInfo      * sTbsInfo;

    STL_TRY( smfFirstTbs( &sTbsId, &sExist, aEnv ) == STL_SUCCESS );

    /**
     * 모든 tablespace의 모든 datafile에서 최소 checkpoint Lsn을 구한다.
     */
    while( sExist )
    {
        if( SMF_IS_ONLINE_TBS( sTbsId ) == STL_TRUE )
        {
            sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[sTbsId];

            /**
             * temporary tablespace는 복구에서 제외
             */
            if( (sTbsInfo != NULL) &&
                (SML_TBS_IS_TEMPORARY( sTbsInfo->mTbsPersData.mAttr ) != STL_TRUE) )
            {
                /**
                 * tablespace의 최소 checkpoint Lsn을 구한다.
                 */
                STL_TRY( smfGetTbsMinChkptLsn( sTbsId,
                                               &sTbsLid,
                                               &sTbsMinLsn,
                                               aEnv ) == STL_SUCCESS );

                if( (sMinLsn == SMR_INIT_LSN) ||
                    (sMinLsn > sTbsMinLsn) )
                {
                    sMinLsn = sTbsMinLsn;
                    sChkptLid = sTbsLid;
                }
            }
        }

        STL_TRY( smfNextTbs( &sTbsId,
                             &sExist,
                             aEnv ) == STL_SUCCESS );
    }

    *aMinChkptLsn = sMinLsn;
    *aChkptLid = sChkptLid;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Tablespace의 모든 Datafile Header를 판독하여 최소의 Chkpt Lsn을 구한다.
 * @param[in]  aTbsId       Tablespace Identifier
 * @param[out] aChkptLid    Checkpoint Lid
 * @param[out] aMinChkptLsn Minimum Checkpoint Lsn
 * @param[in,out] aEnv      Environment 정보
 */
stlStatus smfGetTbsMinChkptLsn( smlTbsId    aTbsId,
                                smrLid    * aChkptLid,
                                smrLsn    * aMinChkptLsn,
                                smlEnv    * aEnv )
{
    smfTbsInfo       * sTbsInfo;
    smfDatafileInfo  * sDatafileInfo;
    smlDatafileId      sDatafileId = SML_INVALID_DATAFILE_ID;
    stlInt32           sFileState = 0;
    smfDatafileHeader  sDatafileHeader;
    stlFile            sFile;
    stlBool            sDatafileExist;
    smrLid             sTbsRestoredLid;
    smrLid             sMinLid = SMR_INVALID_LID;
    smrLsn             sMinLsn = SMR_INIT_LSN;
    smrLsn             sTbsRestoredLsn;
    stlInt32           sFlag;
    stlInt64           sDatabaseIo;

    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];

    sFlag = STL_FOPEN_LARGEFILE | STL_FOPEN_WRITE | STL_FOPEN_READ;

    sDatabaseIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_DATABASE_FILE_IO,
                                                 KNL_ENV(aEnv) );

    if( sDatabaseIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }

    STL_TRY( smfFirstDatafile( aTbsId,
                               &sDatafileId,
                               &sDatafileExist,
                               aEnv ) == STL_SUCCESS );

    while( sDatafileExist == STL_TRUE )
    {
        sDatafileInfo = sTbsInfo->mDatafileInfoArray[sDatafileId];

        STL_ASSERT( sDatafileInfo != NULL );

        STL_TRY( smgOpenFile( &sFile,
                              sDatafileInfo->mDatafilePersData.mName,
                              sFlag,
                              STL_FPERM_OS_DEFAULT,
                              aEnv )
                 == STL_SUCCESS );
        sFileState = 1;

        STL_TRY( smfReadDatafileHeader( &sFile,
                                        sDatafileInfo->mDatafilePersData.mName,
                                        &sDatafileHeader,
                                        SML_ENV(aEnv) )
                 == STL_SUCCESS );

        /**
         * Incremental backup을 이용한 restore과정에서 재생성된 datafile의
         * checkpoint Lsn이 invalid Lsn이 될 수 있고, 이경우 pch array에
         * merge된 최종 restored Lsn, Lid를 이용하여 min chkpt Lsn을 구한다.
         */
        if( sDatafileHeader.mCheckpointLsn == SMR_INVALID_LSN )
        {
            STL_TRY( smfGetTbsMinRestoredChkptInfo( aTbsId,
                                                    &sTbsRestoredLsn,
                                                    &sTbsRestoredLid,
                                                    aEnv ) == STL_SUCCESS );

            if( (sMinLsn == SMR_INIT_LSN) ||
                (sMinLsn > sTbsRestoredLsn) )
            {
                sMinLsn = sTbsRestoredLsn;
                sMinLid = sTbsRestoredLid;
            }
        }
        else
        {
            if( (sMinLsn == SMR_INIT_LSN) ||
                (sMinLsn > sDatafileHeader.mCheckpointLsn) )
            {
                sMinLsn = sDatafileHeader.mCheckpointLsn;
                sMinLid = sDatafileHeader.mCheckpointLid;
            }
        }

        sFileState = 0;
        STL_TRY( smgCloseFile( &sFile,
                               aEnv )
                 == STL_SUCCESS );

        STL_TRY( smfNextDatafile( aTbsId,
                                  &sDatafileId,
                                  &sDatafileExist,
                                  aEnv ) == STL_SUCCESS );
    }

    /**
     * Tablespace의 Min Checkpoint Lsn은 반드시 valid한 Lsn이어야 한다.
     */
    STL_ASSERT( SMR_IS_VALID_LOG_LSN( sMinLsn ) == STL_TRUE );

    *aMinChkptLsn = sMinLsn;
    *aChkptLid = sMinLid;

    return STL_SUCCESS;

    STL_FINISH;

    if( sFileState == 1 )
    {
        (void)smgCloseFile( &sFile, aEnv );
    }
    
    return STL_FAILURE;
}

/** @} */

/**
 * @addtogroup smfTbsMgr
 * @{
 */

stlStatus smfFirstDatafilePersData( void                 * aTbsIterator,
                                    smfDatafilePersData  * aDatafilePersData,
                                    void                ** aIterator,
                                    smlEnv               * aEnv )
{
    smfDatafileInfo * sDatafileInfo;
    smfTbsInfo      * sTbsInfo;
    stlUInt32         i;
    
    sTbsInfo = (smfTbsInfo*)aTbsIterator;
    *aIterator = NULL;
    
    for( i = 0; i < SML_DATAFILE_MAX_COUNT; i++ )
    {
        sDatafileInfo = sTbsInfo->mDatafileInfoArray[i];
        
        if( sDatafileInfo == NULL )
        {
            continue;
        }

        if( (sDatafileInfo->mDatafilePersData.mState == SMF_DATAFILE_STATE_INIT) ||
            (sDatafileInfo->mDatafilePersData.mState == SMF_DATAFILE_STATE_DROPPED) )
        {
            continue;
        }

        *aIterator = (void*)sDatafileInfo;
        *aDatafilePersData = sDatafileInfo->mDatafilePersData;
        break;
    }

    return STL_SUCCESS;
}

stlStatus smfNextDatafilePersData( void                 * aTbsIterator,
                                   smfDatafilePersData  * aDatafilePersData,
                                   void                ** aIterator,
                                   smlEnv               * aEnv )
{
    smfDatafileInfo * sDatafileInfo;
    smfTbsInfo      * sTbsInfo;
    stlUInt32         i;
    
    sTbsInfo = (smfTbsInfo*)aTbsIterator;
    sDatafileInfo = (smfDatafileInfo*)*aIterator;

    *aIterator = NULL;
    
    for( i = sDatafileInfo->mDatafilePersData.mID + 1; i < SML_DATAFILE_MAX_COUNT; i++ )
    {
        sDatafileInfo = sTbsInfo->mDatafileInfoArray[i];
        
        if( sDatafileInfo == NULL )
        {
            continue;
        }

        if( (sDatafileInfo->mDatafilePersData.mState == SMF_DATAFILE_STATE_INIT) ||
            (sDatafileInfo->mDatafilePersData.mState == SMF_DATAFILE_STATE_DROPPED) )
        {
            continue;
        }

        *aIterator = (void*)sDatafileInfo;
        *aDatafilePersData = sDatafileInfo->mDatafilePersData;
        break;
    }

    return STL_SUCCESS;
}

stlStatus smfCreateTbsPending( stlUInt32   aActionFlag,
                               smgPendOp * aPendOp,
                               smlEnv    * aEnv )
{
    smlTbsId            sTbsId;
    smfTbsInfo        * sTbsInfo;
    smfDatafileInfo   * sDatafileInfo;
    stlInt32            sState = 0;
    stlInt32            i;
    smfTbsPendingArgs * sPendingArgs;

    sPendingArgs = (smfTbsPendingArgs*)aPendOp->mArgs;
    
    sTbsId = sPendingArgs->mTbsId;
    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[sTbsId];

    STL_TRY( smfAcquireTablespace( sTbsInfo, aEnv ) == STL_SUCCESS );
    sState = 1;

    sTbsInfo->mTbsPersData.mState = SMF_TBS_STATE_CREATED;
    
    for( i = 0; i < SML_DATAFILE_MAX_COUNT; i++ )
    {
        sDatafileInfo = sTbsInfo->mDatafileInfoArray[i];
        
        if( sDatafileInfo == NULL )
        {
            continue;
        }

        sDatafileInfo->mDatafilePersData.mState = SMF_DATAFILE_STATE_CREATED;
    }
    
    sState = 0;
    STL_TRY( smfReleaseTablespace( sTbsInfo, aEnv ) == STL_SUCCESS );

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV(aEnv),
                        KNL_LOG_LEVEL_INFO,
                        "[TABLESPACE] Create Tablespace(%d)\n",
                        sTbsId )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void)smfReleaseTablespace( sTbsInfo, aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus smfCreateDatafilePending( stlUInt32   aActionFlag,
                                    smgPendOp * aPendOp,
                                    smlEnv    * aEnv )
{
    smfDatafilePersData * sPendingArgs;
    stlBool               sIsRemoved;

    sPendingArgs = (smfDatafilePersData*)aPendOp->mArgs;

    /* database creation을 rollback하기 위해
     * file magager에 add된 datafile name을 제거한다. */
    STL_TRY( knlRemoveDbFile( sPendingArgs->mName,
                              &sIsRemoved,
                              KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smfRenameDatafilePending( stlUInt32   aActionFlag,
                                    smgPendOp * aPendOp,
                                    smlEnv    * aEnv )
{
    stlBool    sIsRemoved;

    if( aActionFlag == SML_PEND_ACTION_COMMIT )
    {
        /* datafile rename을 commit하기 위해 이전 datafile name을 제거한다. */
        STL_TRY( knlRemoveDbFile( aPendOp->mArgs,
                                  &sIsRemoved,
                                  KNL_ENV(aEnv) ) == STL_SUCCESS );
    }
    else
    {
        /* datafile rename을 rollback하기 위해 새로운 datafile name을 제거한다. */
        STL_TRY( knlRemoveDbFile( aPendOp->mArgs,
                                  &sIsRemoved,
                                  KNL_ENV(aEnv) ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

smfDatafileInfo * smfGetDatafileInfo( smlTbsId      aTbsId,
                                      smlDatafileId aDatafileId )
{
    smfTbsInfo * sTbsInfo;

    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];

    STL_DASSERT( sTbsInfo != NULL );

    return sTbsInfo->mDatafileInfoArray[aDatafileId];
}

stlStatus smfDropDatafilePending( stlUInt32   aActionFlag,
                                  smgPendOp * aPendOp,
                                  smlEnv    * aEnv )
{
    smfDatafilePendingArgs * sArgs;
    smlTbsId                 sTbsId;
    smlDatafileId            sDatafileId;
    smfTbsInfo             * sTbsInfo;
    smfDatafileInfo        * sDatafileInfo;
    stlUInt32                sTbsTypeId;
    stlUInt32                sState = 0;
    stlInt32                 sOffset = 0;
    stlChar                  sBuffer[24];
    smxlTransId              sTransId;
    smlScn                   sObjectScn;
    smlScn                   sObjectAgableScn;
    stlBool                  sIsRemoved;

    sArgs = (smfDatafilePendingArgs*)aPendOp->mArgs;
    sTbsId = sArgs->mTbsId;
    sDatafileId = sArgs->mDatafileId;
    sTransId = sArgs->mTransId;
    
    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[sTbsId];
    sDatafileInfo = sTbsInfo->mDatafileInfoArray[sDatafileId];

    STL_TRY( smfAcquireTablespace( sTbsInfo, aEnv ) == STL_SUCCESS );
    sState = 1;
    
    sTbsTypeId = SML_TBS_DEVICE_TYPE_ID( sTbsInfo->mTbsPersData.mAttr );

    sDatafileInfo->mDatafilePersData.mState = SMF_DATAFILE_STATE_AGING;

    sState = 0;
    STL_TRY( smfReleaseTablespace( sTbsInfo, aEnv ) == STL_SUCCESS );

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV(aEnv),
                        KNL_LOG_LEVEL_INFO,
                        "[DATAFILE] Drop Datafile(%s)\n",
                        sDatafileInfo->mDatafilePersData.mName )
             == STL_SUCCESS );
    
    STL_TRY( smfSaveCtrlFile( aEnv ) == STL_SUCCESS );

    /**
     * 물리적으로 데이터 파일을 삭제한다.
     * - Aging 상태에는 해당 파일에 접근이 불가능하기 때문에 Commit시
     *   삭제해도 무방하다.
     * - Temporary Tablespace는 Datafile을 갖지 않는다.
     */
    if( SML_TBS_IS_MEMORY_TEMPORARY( sTbsInfo->mTbsPersData.mAttr ) == STL_FALSE )
    {
        if( stlRemoveFile( sDatafileInfo->mDatafilePersData.mName,
                           KNL_ERROR_STACK( aEnv ) )
            == STL_FAILURE )
        {
            stlPopError( KNL_ERROR_STACK( aEnv ) );
        }

        STL_TRY( knlRemoveDbFile( sDatafileInfo->mDatafilePersData.mName,
                                  &sIsRemoved,
                                  KNL_ENV(aEnv) ) == STL_SUCCESS );
    }

    /**
     * Ager에 Event 추가
     */
    STL_TRY( smxlGetCommitScnFromCache( sTransId,
                                        &sObjectScn,
                                        aEnv )
             == STL_SUCCESS );

    sObjectAgableScn = smxlGetNextSystemScn();

    STL_WRITE_MOVE_INT64( sBuffer, &sObjectScn, sOffset );
    STL_WRITE_MOVE_INT64( sBuffer, &sObjectAgableScn, sOffset );
    STL_WRITE_MOVE_INT16( sBuffer, &sTbsId, sOffset );
    STL_WRITE_MOVE_INT16( sBuffer, &sDatafileId, sOffset );
    STL_WRITE_MOVE_INT32( sBuffer, &sTbsTypeId, sOffset );

    STL_DASSERT( sOffset <= STL_SIZEOF( sBuffer ) );
    STL_DASSERT( sOffset <= aPendOp->mEventMemSize );

    /**
     * Restart Recovery 단계에서는 직접 aging한다.
     */
    if( (smfGetServerState() == SML_SERVER_STATE_RECOVERING) ||
        (smrProceedMediaRecovery(aEnv) == STL_TRUE) )
    {
        STL_TRY( smfAcquireTablespace( sTbsInfo, aEnv ) == STL_SUCCESS );
        sState = 1;

        /**
         * 데이터베이스 영역의 공유메모리를 삭제한다
         */
        STL_TRY( gDatafileFuncs[sTbsTypeId]->mDrop( sTransId,
                                                    sTbsInfo,
                                                    sDatafileInfo,
                                                    aEnv )
                 == STL_SUCCESS );

        sDatafileInfo->mDatafilePersData.mState = SMF_DATAFILE_STATE_DROPPED;

        sState = 0;
        STL_TRY( smfReleaseTablespace( sTbsInfo, aEnv ) == STL_SUCCESS );

        if( aPendOp->mEventMem != NULL )
        {
            STL_TRY( knlFreeEnvEventMem( aPendOp->mEventMem,
                                         KNL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
    }
    else
    {
        STL_TRY( knlAddEnvEvent( SML_EVENT_DROP_DATAFILE,
                                 aPendOp->mEventMem,
                                 sBuffer,
                                 sOffset,
                                 SML_AGER_ENV_ID,
                                 KNL_EVENT_WAIT_NO,
                                 STL_TRUE,  /* aForceSuccess */
                                 KNL_ENV( aEnv ) ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smfReleaseTablespace( sTbsInfo, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smfDropDatafileAging( void      * aData,
                                stlUInt32   aDataSize,
                                stlBool   * aDone,
                                void      * aEnv )
{
    stlInt32          sOffset = 0;
    smlScn            sObjectScn;
    smlScn            sObjectAgableScn;
    smlTbsId          sTbsId;
    smlDatafileId     sDatafileId;
    stlInt32          sTbsTypeId;
    smxlTransId       sTransId;
    stlUInt64         sTransSeq;
    stlInt32          sState = 0;
    smfTbsInfo      * sTbsInfo;
    smfDatafileInfo * sDatafileInfo;
    
    *aDone = STL_FALSE;

    STL_READ_MOVE_INT64( &sObjectScn, aData, sOffset );
    STL_READ_MOVE_INT64( &sObjectAgableScn, aData, sOffset );
    STL_READ_MOVE_INT16( &sTbsId, aData, sOffset );
    STL_READ_MOVE_INT16( &sDatafileId, aData, sOffset );
    STL_READ_MOVE_INT32( &sTbsTypeId, aData, sOffset );

    if( sObjectAgableScn <= smxlGetAgableStmtScn( SML_ENV( aEnv ) ) )
    {
        STL_TRY( knlLogMsg( NULL,
                            KNL_ENV( aEnv ),
                            KNL_LOG_LEVEL_INFO,
                            "[AGER] aging datafile - object scn(%ld), object view scn(%ld), "
                            "tablespace id(%d), datafile id(%d)\n",
                            sObjectScn,
                            sObjectAgableScn,
                            sTbsId,
                            sDatafileId )
                 == STL_SUCCESS );

        STL_TRY( smxlAllocTrans( SML_TIL_READ_COMMITTED,
                                 STL_FALSE, /* aIsGlobalTrans */
                                 &sTransId,
                                 &sTransSeq,
                                 aEnv ) == STL_SUCCESS );
        sState = 1;

        smxlSetTransAttr( sTransId, SMXL_ATTR_UPDATABLE );
    
        sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[sTbsId];
        sDatafileInfo = sTbsInfo->mDatafileInfoArray[sDatafileId];
        
        STL_TRY( smfAcquireTablespace( sTbsInfo, aEnv ) == STL_SUCCESS );
        sState = 2;

        /**
         * 데이터베이스 영역의 공유메모리를 삭제한다
         */
    
        STL_TRY( gDatafileFuncs[sTbsTypeId]->mDrop( sTransId,
                                                    sTbsInfo,
                                                    sDatafileInfo,
                                                    aEnv )
                 == STL_SUCCESS );

        sDatafileInfo->mDatafilePersData.mState = SMF_DATAFILE_STATE_DROPPED;

        sState = 1;
        STL_TRY( smfReleaseTablespace( sTbsInfo, aEnv ) == STL_SUCCESS );

        sState = 0;
        STL_TRY( smxlCommit( sTransId,
                             NULL, /* aComment */
                             SML_TRANSACTION_CWM_WAIT,
                             aEnv ) == STL_SUCCESS );
        STL_TRY( smxlFreeTrans( sTransId,
                                SML_PEND_ACTION_COMMIT,
                                STL_FALSE, /* aCleanup */
                                SML_TRANSACTION_CWM_WAIT,
                                aEnv ) == STL_SUCCESS );
        
        STL_TRY( smfSaveCtrlFile( aEnv ) == STL_SUCCESS );
        
        *aDone = STL_TRUE;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)smfReleaseTablespace( sTbsInfo, aEnv );
        case 1:
            (void)smxlRollback( sTransId,
                                SML_TRANSACTION_CWM_NO_WAIT,
                                STL_FALSE, /* aCleanup */
                                aEnv );
            (void)smxlFreeTrans( sTransId,
                                 SML_PEND_ACTION_ROLLBACK,
                                 STL_FALSE, /* aCleanup */
                                 SML_TRANSACTION_CWM_NO_WAIT,
                                 aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}

stlStatus smfAddDatafilePending( stlUInt32   aActionFlag,
                                 smgPendOp * aPendOp,
                                 smlEnv    * aEnv )
{
    smfDatafilePendingArgs * sArgs;
    smlTbsId                 sTbsId;
    smlDatafileId            sDatafileId;
    smfTbsInfo             * sTbsInfo;
    smfDatafileInfo        * sDatafileInfo;
    stlInt32                 sState = 0;

    sArgs = (smfDatafilePendingArgs*)aPendOp->mArgs;
    sTbsId = sArgs->mTbsId;
    sDatafileId = sArgs->mDatafileId;
    
    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[sTbsId];
    sDatafileInfo = sTbsInfo->mDatafileInfoArray[sDatafileId];

    STL_TRY( smfAcquireTablespace( sTbsInfo, aEnv ) == STL_SUCCESS );
    sState = 1;

    sDatafileInfo->mDatafilePersData.mState = SMF_DATAFILE_STATE_CREATED;

    sState = 0;
    STL_TRY( smfReleaseTablespace( sTbsInfo, aEnv ) == STL_SUCCESS );

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV(aEnv),
                        KNL_LOG_LEVEL_INFO,
                        "[DATAFILE] add datafile(%s)\n",
                        sDatafileInfo->mDatafilePersData.mName )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void)smfReleaseTablespace( sTbsInfo, aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}

stlStatus smfDropTbsPending( stlUInt32   aActionFlag,
                             smgPendOp * aPendOp,
                             smlEnv    * aEnv )
{
    smlTbsId            sTbsId;
    smfTbsInfo        * sTbsInfo;
    stlUInt32           sState = 0;
    stlUInt32           sLatchState = 0;
    stlInt32            sOffset = 0;
    stlChar             sBuffer[24];
    smlScn              sObjectScn;
    smlScn              sObjectAgableScn;
    smfTbsPendingArgs * sPendingArgs;
    smxlTransId         sTransId;
    stlInt32            sTbsTypeId;
    smfDatafileInfo   * sDatafileInfo;
    stlInt32            i;
    stlBool             sIsTimedOut;
    stlBool             sIsRemoved;

    sPendingArgs = (smfTbsPendingArgs*)aPendOp->mArgs;
    
    sTbsId = sPendingArgs->mTbsId;
    sTransId = sPendingArgs->mTransId;
    
    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[sTbsId];

    STL_TRY( smfAcquireTablespace( sTbsInfo, aEnv ) == STL_SUCCESS );
    sState = 1;
    
    sTbsTypeId = SML_TBS_DEVICE_TYPE_ID( sTbsInfo->mTbsPersData.mAttr );
    sTbsInfo->mTbsPersData.mState = SMF_TBS_STATE_AGING;
    
    sState = 0;
    STL_TRY( smfReleaseTablespace( sTbsInfo, aEnv ) == STL_SUCCESS );

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV(aEnv),
                        KNL_LOG_LEVEL_INFO,
                        "[TABLESPACE] drop tablespace ( %d )\n",
                        sTbsId )
             == STL_SUCCESS );
    
    STL_TRY( smfSaveCtrlFile( aEnv ) == STL_SUCCESS );
    
    /**
     * 물리적으로 데이터 파일을 삭제한다.
     * - Aging 상태에는 해당 파일에 접근이 불가능하기 때문에 Commit시
     *   삭제해도 무방하다.
     * - Temporary Tablespace는 Datafile을 갖지 않는다.
     */

    if( SML_TBS_IS_MEMORY_TEMPORARY( sTbsInfo->mTbsPersData.mAttr ) == STL_FALSE )
    {
        for( i = 0; i < SML_DATAFILE_MAX_COUNT; i++ )
        {
            sDatafileInfo = sTbsInfo->mDatafileInfoArray[i];
        
            if( sDatafileInfo == NULL )
            {
                continue;
            }
            
            if( sDatafileInfo->mDatafilePersData.mState ==
                SMF_DATAFILE_STATE_DROPPED )
            {
                continue;
            }

            if( sPendingArgs->mAndDatafiles == STL_TRUE )
            {
                /**
                 * 에러를 무시한다.
                 * - 파일이 없어도 Drop Tablespace는 성공한다.
                 */
                if( stlRemoveFile( sDatafileInfo->mDatafilePersData.mName,
                                   KNL_ERROR_STACK( aEnv ) )
                    == STL_FAILURE )
                {
                    stlPopError( KNL_ERROR_STACK( aEnv ) );
                }
            }

            STL_TRY( knlRemoveDbFile( sDatafileInfo->mDatafilePersData.mName,
                                      &sIsRemoved,
                                      KNL_ENV(aEnv) ) == STL_SUCCESS );
        }
    }

    /**
     * Ager에 Event 추가
     */
    STL_TRY( smxlGetCommitScnFromCache( sTransId,
                                        &sObjectScn,
                                        aEnv )
             == STL_SUCCESS );

    sObjectAgableScn = smxlGetNextSystemScn();

    STL_WRITE_MOVE_INT64( sBuffer, &sObjectScn, sOffset );
    STL_WRITE_MOVE_INT64( sBuffer, &sObjectAgableScn, sOffset );
    STL_WRITE_MOVE_INT16( sBuffer, &sTbsId, sOffset );
    STL_WRITE_MOVE_INT32( sBuffer, &sTbsTypeId, sOffset );

    STL_DASSERT( sOffset < STL_SIZEOF( sBuffer ) );
    STL_DASSERT( sOffset <= aPendOp->mEventMemSize );

    /**
     * Restart Recovery 단계에서는 직접 aging한다.
     */
    if( (smfGetServerState() == SML_SERVER_STATE_RECOVERING) ||
        (smrProceedMediaRecovery(aEnv) == STL_TRUE) )
    {
        STL_TRY( knlAcquireLatch( &gSmfWarmupEntry->mLatch,
                                  KNL_LATCH_MODE_EXCLUSIVE,
                                  KNL_LATCH_PRIORITY_NORMAL,
                                  STL_INFINITE_TIME,
                                  &sIsTimedOut,
                                  (knlEnv*)aEnv ) == STL_SUCCESS );
        sLatchState = 1;
        
        for( i = 0; i < SML_DATAFILE_MAX_COUNT; i++ )
        {
            sDatafileInfo = sTbsInfo->mDatafileInfoArray[i];
        
            if( sDatafileInfo == NULL )
            {
                continue;
            }
            
            if( sDatafileInfo->mDatafilePersData.mState == SMF_DATAFILE_STATE_DROPPED )
            {
                continue;
            }

            /**
             * PCH List를 삭제한다.
             */
    
            STL_TRY( smpShrink( NULL,   /* aStatement */
                                sTbsId,
                                sDatafileInfo->mDatafilePersData.mID,
                                SMP_PCHARRAY_STATE_DROPPED, /* aPchArrayState */
                                aEnv )
                     == STL_SUCCESS );
    
            /**
             * 데이터베이스 영역의 공유메모리를 삭제한다
             */
            STL_TRY( gDatafileFuncs[sTbsTypeId]->mDrop( sTransId,
                                                        sTbsInfo,
                                                        sDatafileInfo,
                                                        aEnv )
                     == STL_SUCCESS );

            sDatafileInfo->mDatafilePersData.mState = SMF_DATAFILE_STATE_DROPPED;
        }

        sTbsInfo->mTbsPersData.mState = SMF_TBS_STATE_DROPPED;

        sLatchState = 0;
        STL_TRY( knlReleaseLatch( &gSmfWarmupEntry->mLatch,
                                  (knlEnv*)aEnv ) == STL_SUCCESS );

        if( aPendOp->mEventMem != NULL )
        {
            STL_TRY( knlFreeEnvEventMem( aPendOp->mEventMem,
                                         KNL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
    }
    else
    {
        STL_TRY( knlAddEnvEvent( SML_EVENT_DROP_TBS,
                                 aPendOp->mEventMem,
                                 sBuffer,
                                 sOffset,
                                 SML_AGER_ENV_ID,
                                 KNL_EVENT_WAIT_NO,
                                 STL_TRUE,  /* aForceSuccess */
                                 KNL_ENV( aEnv ) ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sLatchState == 1 )
    {
        (void)knlReleaseLatch( &gSmfWarmupEntry->mLatch, (knlEnv*)aEnv );
    }

    switch( sState )
    {
        case 1:
            (void)smfReleaseTablespace( sTbsInfo, aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus smfDropTbsAging( void      * aData,
                           stlUInt32   aDataSize,
                           stlBool   * aDone,
                           void      * aEnv )
{
    stlInt32          sOffset = 0;
    smlScn            sObjectScn;
    smlScn            sObjectAgableScn;
    smlTbsId          sTbsId;
    stlInt32          sTbsTypeId;
    smxlTransId       sTransId;
    stlUInt64         sTransSeq;
    stlInt32          sState = 0;
    smfTbsInfo      * sTbsInfo;
    smfDatafileInfo * sDatafileInfo;
    stlInt32          i;
    stlBool           sIsTimedOut;
    
    *aDone = STL_FALSE;

    STL_READ_MOVE_INT64( &sObjectScn, aData, sOffset );
    STL_READ_MOVE_INT64( &sObjectAgableScn, aData, sOffset );
    STL_READ_MOVE_INT16( &sTbsId, aData, sOffset );
    STL_READ_MOVE_INT32( &sTbsTypeId, aData, sOffset );

    if( sObjectAgableScn <= smxlGetAgableStmtScn( SML_ENV( aEnv ) ) )
    {
        STL_TRY( knlLogMsg( NULL,
                            KNL_ENV( aEnv ),
                            KNL_LOG_LEVEL_INFO,
                            "[AGER] aging tablespace - object scn(%ld), object view scn(%ld), "
                            "tablespace id(%d)\n",
                            sObjectScn,
                            sObjectAgableScn,
                            sTbsId )
                 == STL_SUCCESS );

        STL_TRY( smxlAllocTrans( SML_TIL_READ_COMMITTED,
                                 STL_FALSE, /* aIsGlobalTrans */
                                 &sTransId,
                                 &sTransSeq,
                                 aEnv ) == STL_SUCCESS );
        sState = 1;

        smxlSetTransAttr( sTransId, SMXL_ATTR_UPDATABLE );
    
        STL_TRY( knlAcquireLatch( &gSmfWarmupEntry->mLatch,
                                  KNL_LATCH_MODE_EXCLUSIVE,
                                  KNL_LATCH_PRIORITY_NORMAL,
                                  STL_INFINITE_TIME,
                                  &sIsTimedOut,
                                  (knlEnv*)aEnv ) == STL_SUCCESS );
        sState = 2;
        
        sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[sTbsId];
        
        for( i = 0; i < SML_DATAFILE_MAX_COUNT; i++ )
        {
            sDatafileInfo = sTbsInfo->mDatafileInfoArray[i];
        
            if( sDatafileInfo == NULL )
            {
                continue;
            }
            
            if( sDatafileInfo->mDatafilePersData.mState == SMF_DATAFILE_STATE_DROPPED )
            {
                continue;
            }

            /**
             * PCH List를 삭제한다.
             */
    
            STL_TRY( smpShrink( NULL,   /* aStatement */
                                sTbsId,
                                sDatafileInfo->mDatafilePersData.mID,
                                SMP_PCHARRAY_STATE_DROPPED, /* aPchArrayState */
                                aEnv )
                     == STL_SUCCESS );
    
            /**
             * 데이터베이스 영역의 공유메모리를 삭제한다
             */
    
            STL_TRY( gDatafileFuncs[sTbsTypeId]->mDrop( sTransId,
                                                        sTbsInfo,
                                                        sDatafileInfo,
                                                        aEnv )
                     == STL_SUCCESS );

            sDatafileInfo->mDatafilePersData.mState = SMF_DATAFILE_STATE_DROPPED;
        }

        sTbsInfo->mTbsPersData.mState = SMF_TBS_STATE_DROPPED;

        sState = 1;
        STL_TRY( knlReleaseLatch( &gSmfWarmupEntry->mLatch,
                                  (knlEnv*)aEnv ) == STL_SUCCESS );
        
        sState = 0;
        STL_TRY( smxlCommit( sTransId,
                             NULL, /* aComment */
                             SML_TRANSACTION_CWM_WAIT,
                             aEnv ) == STL_SUCCESS );
        STL_TRY( smxlFreeTrans( sTransId,
                                SML_PEND_ACTION_COMMIT,
                                STL_FALSE, /* aCleanup */
                                SML_TRANSACTION_CWM_WAIT,
                                aEnv ) == STL_SUCCESS );
        
        *aDone = STL_TRUE;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)knlReleaseLatch( &gSmfWarmupEntry->mLatch, (knlEnv*)aEnv );
        case 1:
            (void)smxlRollback( sTransId,
                                SML_TRANSACTION_CWM_NO_WAIT,
                                STL_FALSE, /* aCleanup */
                                aEnv );
            (void)smxlFreeTrans( sTransId,
                                 SML_PEND_ACTION_ROLLBACK,
                                 STL_FALSE, /* aCleanup */
                                 SML_TRANSACTION_CWM_NO_WAIT,
                                 aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}

stlStatus smfOnlineTbsPending( stlUInt32   aActionFlag,
                               smgPendOp * aPendOp,
                               smlEnv    * aEnv )
{
    smlTbsId            sTbsId;
    smfTbsInfo        * sTbsInfo;
    stlUInt32           sState = 1;
    smfTbsPendingArgs * sPendingArgs;
    smpFlushTbsArg      sArg;

    sPendingArgs = (smfTbsPendingArgs*)aPendOp->mArgs;
    
    sTbsId = sPendingArgs->mTbsId;
    
    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[sTbsId];

    /**
     * pending operation의 수행으로 tablespace가 dirty되어 있을수도 있는
     * offline으로 전환시 dirty된 페이지들을 datafile로 기록한다.
     */

    sArg.mTbsId = sTbsId;
    sArg.mDatafileId = SML_INVALID_DATAFILE_ID;
    sArg.mReason = SMF_CHANGE_DATAFILE_HEADER_FOR_GENERAL;

    STL_DASSERT( STL_SIZEOF(smpFlushTbsArg) <= aPendOp->mEventMemSize );

    STL_TRY( knlAddEnvEvent( SML_EVENT_PAGE_FLUSH_TBS,
                             aPendOp->mEventMem,
                             (void*)&sArg,                 /* aData */
                             STL_SIZEOF(smpFlushTbsArg),   /* aDataSize */
                             SML_PAGE_FLUSHER_ENV_ID,
                             KNL_EVENT_WAIT_BLOCKED,
                             STL_TRUE,  /* aForceSuccess */
                             KNL_ENV( aEnv ) ) == STL_SUCCESS );

    STL_TRY( smfAcquireTablespace( sTbsInfo, aEnv ) == STL_SUCCESS );
    sState = 2;

    if( (stlGetLastErrorCode( KNL_ERROR_STACK(aEnv) ) == SML_ERRCODE_TABLESPACE_OFFLINE_ABNORMALLY) || 
        (aActionFlag == SML_PEND_ACTION_ROLLBACK) )
    {
        /**
         * flush 동안 비정상적으로 offline이 된 경우이거나
         * transaction rollback 되는 경우
         */
        sTbsInfo->mTbsPersData.mIsOnline = STL_FALSE;
        sTbsInfo->mTbsPersData.mOfflineState = sPendingArgs->mOfflineState;
    }
    else
    {
        sTbsInfo->mIsOnline4Scan = STL_TRUE;
        
    }
    
    STL_TRY( smfSaveCtrlFile( aEnv ) == STL_SUCCESS );
    
    sState = 1;
    STL_TRY( smfReleaseTablespace( sTbsInfo, aEnv ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( smfResetBackupWaitingState( sTbsId, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)smfReleaseTablespace( sTbsInfo, aEnv );
        case 1:
            (void) smfResetBackupWaitingState( sTbsId, aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}


stlStatus smfOfflineTbsPending( stlUInt32   aActionFlag,
                                smgPendOp * aPendOp,
                                smlEnv    * aEnv )
{
    smlTbsId            sTbsId;
    smfTbsInfo        * sTbsInfo;
    stlUInt32           sState = 0;
    smfTbsPendingArgs * sPendingArgs;

    sPendingArgs = (smfTbsPendingArgs*)aPendOp->mArgs;
    
    sTbsId = sPendingArgs->mTbsId;
    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[sTbsId];

    STL_TRY( smfAcquireTablespace( sTbsInfo, aEnv ) == STL_SUCCESS );
    sState = 2;

    if( aActionFlag == SML_PEND_ACTION_ROLLBACK )
    {
        sTbsInfo->mIsOnline4Scan = STL_TRUE;
        sTbsInfo->mTbsPersData.mIsOnline = STL_TRUE;
        sTbsInfo->mTbsPersData.mOfflineState = SMF_OFFLINE_STATE_NULL;
    }
    else
    {
        sTbsInfo->mTbsPersData.mIsOnline = STL_FALSE;
        sTbsInfo->mIsOnline4Scan = STL_FALSE;
        sTbsInfo->mTbsPersData.mOfflineState = sPendingArgs->mOfflineState;
    }
    
    STL_TRY( smfSaveCtrlFile( aEnv ) == STL_SUCCESS );
    
    sState = 1;
    STL_TRY( smfReleaseTablespace( sTbsInfo, aEnv ) == STL_SUCCESS );
    
    if( aActionFlag == SML_PEND_ACTION_COMMIT )
    {
        sState = 0;
        STL_TRY( smfResetBackupWaitingState( sTbsId, aEnv ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)smfReleaseTablespace( sTbsInfo, aEnv );
        case 1:
            if( aActionFlag == SML_PEND_ACTION_COMMIT )
            {
                (void)smfResetBackupWaitingState( sTbsId, aEnv );
            }
        default:
            break;
    }

    return STL_FAILURE;
}


stlStatus smfCreateDbCtrlSection( stlFile  * aFile,
                                  stlSize  * aWrittenBytes,
                                  smlEnv   * aEnv )
{
    stlInt64      sTransTableSize;
    stlInt64      sUndoRelCount;
    stlSize       sWrittenBytes;
    smfCtrlBuffer sBuffer;

    SMF_INIT_CTRL_BUFFER( &sBuffer );

    sTransTableSize = knlGetPropertyBigIntValueByID( KNL_PROPERTY_TRANSACTION_TABLE_SIZE,
                                                     KNL_ENV( aEnv ) );

    sUndoRelCount = knlGetPropertyBigIntValueByID( KNL_PROPERTY_UNDO_RELATION_COUNT,
                                                   KNL_ENV( aEnv ) );

    SMF_INIT_DB_PERSISTENT_DATA( &gSmfWarmupEntry->mDbPersData,
                                 sTransTableSize,
                                 sUndoRelCount );

    STL_TRY( smfWriteCtrlFile( aFile,
                               &sBuffer,
                               (stlChar *)&gSmfWarmupEntry->mDbPersData,
                               STL_SIZEOF( smfDbPersData ),
                               &sWrittenBytes,
                               aEnv )
             == STL_SUCCESS );

    /**
     * Controlfile buffer에 남아 있는 내용을 disk에 flush한다.
     */
    STL_TRY( smfFlushCtrlFile( aFile, &sBuffer, aEnv ) == STL_SUCCESS );

    *aWrittenBytes = STL_ALIGN( (stlInt64)sWrittenBytes, SMF_CONTROLFILE_IO_BLOCK_SIZE );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smfWriteDbCtrlSection( stlFile  * aFile,
                                 stlSize  * aWrittenBytes,
                                 smlEnv   * aEnv )
{
    stlSize               sWrittenBytes = 0;
    stlInt32              sTotalWrittenBytes = 0;
    smfTbsPersData        sTbsPersData;
    void                * sTbsIterator;
    smfDatafilePersData   sDatafilePersData;
    void                * sDatafileIterator;
    stlInt32              sTbsCount = 0;
    smfCtrlBuffer         sBuffer;
    stlInt32              sDatafileCount = 0;

    SMF_INIT_CTRL_BUFFER( &sBuffer );

    /**
     * Database 관련 Persistent Information 기록
     */
    STL_TRY( smfFirstTbsPersData( &sTbsPersData,
                                  &sTbsIterator,
                                  aEnv ) == STL_SUCCESS );
                 
    while( 1 )
    {
        if( sTbsIterator == NULL )
        {
            break;
        }

        sTbsCount++;
        STL_TRY( smfNextTbsPersData( &sTbsPersData,
                                     &sTbsIterator,
                                     aEnv ) == STL_SUCCESS );
    }

    gSmfWarmupEntry->mDbPersData.mTbsCount = sTbsCount;

    STL_TRY( smfWriteCtrlFile( aFile,
                               &sBuffer,
                               (stlChar *)&gSmfWarmupEntry->mDbPersData,
                               STL_SIZEOF( smfDbPersData ),
                               &sWrittenBytes,
                               aEnv )
             == STL_SUCCESS );

    sTotalWrittenBytes = sWrittenBytes;

    STL_TRY( smfFirstTbsPersData( &sTbsPersData,
                                  &sTbsIterator,
                                  aEnv ) == STL_SUCCESS );
                 
    /**
     * Tablespace 관련 Persistent Information 기록
     */
    
    while( 1 )
    {
        if( sTbsIterator == NULL )
        {
            break;
        }

        /**
         * Datafile 개수를 계산한다.
         */
            
        sDatafileCount = 0;
        
        STL_TRY( smfFirstDatafilePersData( sTbsIterator,
                                           &sDatafilePersData,
                                           &sDatafileIterator,
                                           aEnv ) == STL_SUCCESS );

        while( 1 )
        {
            if( sDatafileIterator == NULL )
            {
                break;
            }
            
            sDatafileCount++;
            STL_TRY( smfNextDatafilePersData( sTbsIterator,
                                              &sDatafilePersData,
                                              &sDatafileIterator,
                                              aEnv ) == STL_SUCCESS );
        }

        sTbsPersData.mDatafileCount = sDatafileCount;

        /**
         * Tablespace 정보 기록
         */
        
        STL_TRY( smfWriteCtrlFile( aFile,
                                   &sBuffer,
                                   (stlChar *)&sTbsPersData,
                                   STL_SIZEOF( smfTbsPersData ),
                                   &sWrittenBytes,
                                   aEnv )
                 == STL_SUCCESS );
        sTotalWrittenBytes += sWrittenBytes;

        /**
         * Datafile 관련 Persistent Information 기록
         */
        
        STL_TRY( smfFirstDatafilePersData( sTbsIterator,
                                           &sDatafilePersData,
                                           &sDatafileIterator,
                                           aEnv ) == STL_SUCCESS );

        while( 1 )
        {
            if( sDatafileIterator == NULL )
            {
                break;
            }
            
            STL_TRY( smfWriteCtrlFile( aFile,
                                       &sBuffer,
                                       (stlChar *)&sDatafilePersData,
                                       STL_SIZEOF( smfDatafilePersData ),
                                       &sWrittenBytes,
                                       aEnv )
                     == STL_SUCCESS );
            sTotalWrittenBytes += sWrittenBytes;
            
            STL_TRY( smfNextDatafilePersData( sTbsIterator,
                                              &sDatafilePersData,
                                              &sDatafileIterator,
                                              aEnv ) == STL_SUCCESS );
        }

        STL_TRY( smfNextTbsPersData( &sTbsPersData,
                                     &sTbsIterator,
                                     aEnv ) == STL_SUCCESS );
    }

    /**
     * Controlfile buffer에 남아 있는 내용을 disk에 flush한다.
     */
    STL_TRY( smfFlushCtrlFile( aFile, &sBuffer, aEnv ) == STL_SUCCESS );

    *aWrittenBytes = STL_ALIGN( (stlInt64)sTotalWrittenBytes, SMF_CONTROLFILE_IO_BLOCK_SIZE );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smfValidateDatafileNames( stlChar   ** aDatafileNameArray,
                                    stlInt16     aDatafileCount,
                                    stlBool      aMyTbsId,
                                    smlEnv     * aEnv )
{
    smlDatafileId     sDatafileId = SML_INVALID_DATAFILE_ID;
    stlBool           sFound;
    smlTbsId          sTbsId = SML_INVALID_TBS_ID;
    stlBool           sTbsExist;
    stlInt32          i;
    
    STL_TRY( smfFirstTbs( &sTbsId,
                          &sTbsExist,
                          aEnv ) == STL_SUCCESS );
                 
    while( sTbsExist == STL_TRUE )
    {
        if( sTbsId == aMyTbsId )
        {
            STL_THROW( RAMP_NEXT_TBS );
        }

        if( (smfGetTbsState( sTbsId ) & SMF_TBS_STATE_AGING) == SMF_TBS_STATE_AGING )
        {
            STL_THROW( RAMP_NEXT_TBS );
        }

        for( i = 0; i < aDatafileCount; i++ )
        {
            STL_TRY( smfFindDatafileId( sTbsId,
                                        aDatafileNameArray[i],
                                        &sDatafileId,
                                        &sFound,
                                        aEnv )
                     == STL_SUCCESS );
        
            STL_TRY_THROW( sFound == STL_FALSE, RAMP_ERR_FILE_ALREADY_USED );
        }

        STL_RAMP( RAMP_NEXT_TBS );
        
        STL_TRY( smfNextTbs( &sTbsId,
                             &sTbsExist,
                             aEnv ) == STL_SUCCESS );
    }

    KNL_BREAKPOINT( KNL_BREAKPOINT_SMFVALIDATEDATAFILES_AFTER_VALIDATION,
                    KNL_ENV(aEnv) );
                                   
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_FILE_ALREADY_USED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_FILE_ALREADY_USED,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      aDatafileNameArray[i] );
    }

    STL_FINISH;

    return STL_FAILURE;
}
                              

/**
 * @brief Control File을 메모리에 구성한다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smfLoadDbCtrlSection( smlEnv * aEnv )
{
    stlFile           sFile;
    smfTbsInfo      * sTbsInfo;
    smfDatafileInfo * sDatafileInfo;
    stlUInt32         i;
    stlUInt32         j;
    stlUInt32         sTbsTypeId;
    stlUInt32         sState = 0;
    smfCtrlBuffer     sBuffer;

    STL_TRY( smfOpenCtrlSection( &sFile,
                                 SMF_CTRL_SECTION_DB,
                                 aEnv ) == STL_SUCCESS );
    sState = 1;

    SMF_INIT_CTRL_BUFFER( &sBuffer );

    /**
     * Logging 관련 Persistent Information 기록
     */
    STL_TRY( smfReadCtrlFile( &sFile,
                              &sBuffer,
                              (stlChar *)&gSmfWarmupEntry->mDbPersData,
                              STL_SIZEOF( smfDbPersData ),
                              aEnv ) == STL_SUCCESS );

    for( i = 0; i < gSmfWarmupEntry->mDbPersData.mTbsCount; i++ )
    {
        STL_TRY( smgAllocShmMem4Mount( STL_SIZEOF( smfTbsInfo ),
                                       (void**)&sTbsInfo,
                                       aEnv )
                 == STL_SUCCESS );
        sState = 2;
        
        SMF_INIT_TBS_INFO( sTbsInfo );

        STL_TRY( smfReadCtrlFile( &sFile,
                                  &sBuffer,
                                  (stlChar *)&sTbsInfo->mTbsPersData,
                                  STL_SIZEOF( smfTbsPersData ),
                                  aEnv ) == STL_SUCCESS );

        sTbsInfo->mIsOnline4Scan = sTbsInfo->mTbsPersData.mIsOnline;
        sTbsInfo->mIsProceedMediaRecovery = STL_FALSE;
        sTbsInfo->mIsProceedRestore = STL_FALSE;

        gSmfWarmupEntry->mTbsInfoArray[sTbsInfo->mTbsPersData.mTbsId] = sTbsInfo;
        sState = 1;

        STL_TRY( knlInitLatch( &sTbsInfo->mLatch,
                               STL_TRUE,
                               (knlEnv*)aEnv ) == STL_SUCCESS );

        SMF_INIT_TBS_BACKUP_INFO( &(sTbsInfo->mBackupInfo), aEnv );

        for( j = 0; j < sTbsInfo->mTbsPersData.mDatafileCount; j++ )
        {
            sTbsTypeId = SML_TBS_DEVICE_TYPE_ID( sTbsInfo->mTbsPersData.mAttr );
            
            STL_TRY( gDatafileFuncs[sTbsTypeId]->mPrepare( sTbsInfo,
                                                           &sFile,
                                                           &sBuffer,
                                                           &sDatafileInfo,
                                                           aEnv )
                     == STL_SUCCESS );
            
            sTbsInfo->mDatafileInfoArray[sDatafileInfo->mDatafilePersData.mID] = sDatafileInfo;
        }
    }
    
    sState = 0;
    STL_TRY( smfCloseCtrlSection( &sFile,
                                  aEnv ) == STL_SUCCESS );

    smfSetCtrlFileLoadState( SMF_CTRL_FILE_LOAD_STATE_DB );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smfCloseCtrlSection( &sFile, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smfParallelLoad( stlInt32   aPhysicalTbsId,
                           smlEnv   * aEnv )
{
    smfTbsPersData        sTbsPersData;
    void                * sTbsIterator;
    smfDatafilePersData   sDatafilePersData;
    void                * sDatafileIterator;
    stlInt64              sParallelFactor;
    smfParallelLoadJob  * sJob;
    smfParallelLoadJob  * sJobPrev;
    stlInt32              sState = 0;
    stlInt32              i;
    stlChar               sGroupDir[SML_MAX_PARALLEL_IO_GROUP][KNL_PROPERTY_STRING_MAX_LENGTH];
    stlChar               sPropertyName[KNL_PROPERTY_STRING_MAX_LENGTH];
    stlChar               sData[STL_SIZEOF(void*) * 2];
    stlInt64              sOffset = 0;
    smfParallelLoadArgs * sArgs;
    smfParallelLoadArgs * sArgsContext;
    stlBool               sBuildAgableScn;

    /**
     * Initialization
     */

    stlMemset( &sTbsPersData, 0x00, STL_SIZEOF(smfTbsPersData) );
    
    /**
     * Read property
     */

    sParallelFactor = knlGetPropertyBigIntValueByID( KNL_PROPERTY_PARALLEL_IO_FACTOR,
                                                     KNL_ENV( aEnv ) );

    STL_TRY( smgAllocShmMem4Mount( STL_SIZEOF(smfParallelLoadArgs) * sParallelFactor,
                                   (void**)&sArgs,
                                   aEnv )
             == STL_SUCCESS );
    sState = 1;
    
    for( i = 0; i < sParallelFactor; i++ )
    {
        stlSnprintf( sPropertyName,
                     KNL_PROPERTY_STRING_MAX_LENGTH,
                     "PARALLEL_IO_GROUP_%d",
                     i + 1 );
        
        STL_TRY( knlGetPropertyValueByName( sPropertyName,
                                            (void*)sGroupDir[i],
                                            KNL_ENV( aEnv ) )
                 == STL_SUCCESS );

        STL_RING_INIT_HEADLINK( &sArgs[i].mJobs,
                                STL_OFFSETOF( smfParallelLoadJob, mJobLink ) );

        STL_INIT_ERROR_STACK( &sArgs[i].mErrorStack );
    }
    
    /**
     * Distributes parallel jobs
     */
    
    STL_TRY( smfFirstTbsPersData( &sTbsPersData,
                                  &sTbsIterator,
                                  aEnv ) == STL_SUCCESS );
                 
    while( 1 )
    {
        if( sTbsIterator == NULL )
        {
            break;
        }

        /**
         * 아래 경우는 LOAD할 필요가 없다.
         * - Aging Tablespace
         * - Memory Temporary Tablespace
         */
        if( (SML_TBS_IS_MEMORY_TEMPORARY( sTbsPersData.mAttr ) == STL_TRUE) ||
            (sTbsPersData.mState == SMF_TBS_STATE_AGING) )
        {
            STL_TRY( smfNextTbsPersData( &sTbsPersData,
                                         &sTbsIterator,
                                         aEnv ) == STL_SUCCESS );
            continue;
        }

        if( aPhysicalTbsId == SMF_INVALID_PHYSICAL_TBS_ID )
        {
            /**
             * Startup 시에는 OFFLINE Tablespace를 LOAD하지 않는다.
             */
            if( sTbsPersData.mIsOnline != STL_TRUE )
            {
                STL_TRY( smfNextTbsPersData( &sTbsPersData,
                                             &sTbsIterator,
                                             aEnv ) == STL_SUCCESS );
                continue;
            }
        }
        else
        {
            /**
             * Reload의 경우(Media Recovery or Restore) 다른 Tablespace는 LOAD하지 않는다.
             */
            if( aPhysicalTbsId != sTbsPersData.mTbsId )
            {
                STL_TRY( smfNextTbsPersData( &sTbsPersData,
                                             &sTbsIterator,
                                             aEnv ) == STL_SUCCESS );
                continue;
            }
        }

        STL_TRY( smfFirstDatafilePersData( sTbsIterator,
                                           &sDatafilePersData,
                                           &sDatafileIterator,
                                           aEnv ) == STL_SUCCESS );

        while( 1 )
        {
            if( sDatafileIterator == NULL )
            {
                break;
            }

            /**
             * Aging 중이거나 이미 Load가 되었다면 skip한다.
             */
            if( (sDatafilePersData.mState >= SMF_DATAFILE_STATE_AGING) ||
                (((smfDatafileInfo*)sDatafileIterator)->mShmState >=
                 SMF_DATAFILE_SHM_STATE_LOADED) )
            {
                STL_TRY( smfNextDatafilePersData( sTbsIterator,
                                                  &sDatafilePersData,
                                                  &sDatafileIterator,
                                                  aEnv ) == STL_SUCCESS );
                continue;
            }

            STL_TRY( smgAllocShmMem4Mount( STL_SIZEOF( smfParallelLoadJob ),
                                           (void**)&sJob,
                                           aEnv )
                     == STL_SUCCESS );

            STL_RING_INIT_DATALINK( sJob, STL_OFFSETOF( smfParallelLoadJob, mJobLink ) );

            sJob->mTbsId = sTbsPersData.mTbsId;
            sJob->mDatafileId = sDatafilePersData.mID;
            
            /**
             * Datafile path와 Group path가 일치되는 Group을 선택한다.
             */
            for( i = 0; i < sParallelFactor; i++ )
            {
                if( stlStrncmp( sGroupDir[i],
                                sDatafilePersData.mName,
                                stlStrlen( sGroupDir[i] ) )
                    == 0 )
                {
                    break;
                }
            }

            if( i == sParallelFactor )
            {
                /**
                 * 적당한 Group이 없다면 0번 Group을 설정한다.
                 */

                sJob->mGroupId = 0;
                STL_RING_ADD_LAST( &sArgs[0].mJobs, sJob );
                
                knlLogMsg( NULL,
                           KNL_ENV(aEnv),
                           KNL_LOG_LEVEL_INFO,
                           ".... datafile '%s' assigned to the default group \n",
                           sDatafilePersData.mName );
            }
            else
            {
                sJob->mGroupId = i;
                STL_RING_ADD_LAST( &sArgs[i].mJobs, sJob );
                
                knlLogMsg( NULL,
                           KNL_ENV(aEnv),
                           KNL_LOG_LEVEL_INFO,
                           ".... datafile '%s' assigned to PARALLEL_IO_GROUP_%d \n",
                           sDatafilePersData.mName,
                           i + 1 );
            }
            
            STL_TRY( smfNextDatafilePersData( sTbsIterator,
                                              &sDatafilePersData,
                                              &sDatafileIterator,
                                              aEnv ) == STL_SUCCESS );
        }
        
        STL_TRY( smfNextTbsPersData( &sTbsPersData,
                                     &sTbsIterator,
                                     aEnv ) == STL_SUCCESS );
    }
    
    /**
     * Parallel load
     */

    for( i = 0; i < sParallelFactor; i++ )
    {
        if( STL_RING_IS_EMPTY( &sArgs[i].mJobs ) == STL_FALSE )
        {
            sArgsContext = &sArgs[i];
            
            sOffset = 0;
            STL_WRITE_MOVE_POINTER( sData, &sArgsContext, sOffset );
            
            STL_TRY( knlAddEnvEvent( SML_EVENT_DATAFILE_LOAD,
                                     NULL,
                                     sData,
                                     sOffset,
                                     SML_IO_SLAVE_ENV_ID(i),
                                     KNL_EVENT_WAIT_NO,
                                     STL_TRUE,  /* aForceSuccess */
                                     KNL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
    }
    
    for( i = 0; i < sParallelFactor; i++ )
    {
        if( STL_RING_IS_EMPTY( &sArgs[i].mJobs ) == STL_FALSE )
        {
            sBuildAgableScn = STL_FALSE;

            STL_TRY( knlAddEnvEvent( SML_EVENT_LOOPBACK,
                                     NULL,
                                     &sBuildAgableScn,
                                     STL_SIZEOF(stlBool),
                                     SML_IO_SLAVE_ENV_ID(i),
                                     KNL_EVENT_WAIT_POLLING,
                                     STL_TRUE,  /* aForceSuccess */
                                     KNL_ENV(aEnv) )
                     == STL_SUCCESS);
        }
    }
    
    /**
     * Waiting parallel loading
     */
    
    for( i = 0; i < sParallelFactor; i++ )
    {
        if( STL_RING_IS_EMPTY( &sArgs[i].mJobs ) == STL_FALSE )
        {
            if( stlGetErrorStackDepth( &sArgs[i].mErrorStack ) > 0 )
            {
                stlAppendErrors( KNL_ERROR_STACK( aEnv ),
                                 &sArgs[i].mErrorStack );

                STL_TRY( STL_FALSE );
            }
        }
        
        STL_RING_FOREACH_ENTRY_SAFE( &sArgs[i].mJobs, sJob, sJobPrev )
        {
            STL_RING_UNLINK( &sArgs[i].mJobs, sJob );
            (void) smgFreeShmMem4Mount( sJob, aEnv );
        }
    }

    sState = 0;
    STL_TRY( smgFreeShmMem4Mount( sArgs, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        for( i = 0; i < sParallelFactor; i++ )
        {
            STL_RING_FOREACH_ENTRY_SAFE( &sArgs[i].mJobs, sJob, sJobPrev )
            {
                STL_RING_UNLINK( &sArgs[i].mJobs, sJob );
                (void) smgFreeShmMem4Mount( sJob, aEnv );
            }
        }

        (void) smgFreeShmMem4Mount( sArgs, aEnv );
    }

    return STL_FAILURE;
}


stlStatus smfChooseIoGroupId( stlChar  * aDatafileName,
                              stlInt16 * aIoGroupId,
                              smlEnv   * aEnv )
{
    stlInt64    sParallelFactor;
    stlInt32    i;
    stlChar     sGroupDir[SML_MAX_PARALLEL_IO_GROUP][KNL_PROPERTY_STRING_MAX_LENGTH];
    stlChar     sPropertyName[KNL_PROPERTY_STRING_MAX_LENGTH];
    
    sParallelFactor = knlGetPropertyBigIntValueByID( KNL_PROPERTY_PARALLEL_IO_FACTOR,
                                                     KNL_ENV( aEnv ) );
    
    for( i = 0; i < sParallelFactor; i++ )
    {
        stlSnprintf( sPropertyName,
                     KNL_PROPERTY_STRING_MAX_LENGTH,
                     "PARALLEL_IO_GROUP_%d",
                     i + 1 );
        
        STL_TRY( knlGetPropertyValueByName( sPropertyName,
                                            (void*)sGroupDir[i],
                                            KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
    }
    
    for( i = 0; i < sParallelFactor; i++ )
    {
        if( stlStrncmp( sGroupDir[i],
                        aDatafileName,
                        stlStrlen( sGroupDir[i] ) )
            == 0 )
        {
            break;
        }
    }

    if( i == sParallelFactor )
    {
        *aIoGroupId = 0;
    }
    else
    {
        *aIoGroupId = i;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smfLoadEventHandler( void      * aData,
                               stlUInt32   aDataSize,
                               stlBool   * aDone,
                               void      * aEnv )
{
    stlInt32              sOffset = 0;
    stlUInt32             sTbsTypeId;
    smfTbsInfo          * sTbsInfo;
    smfDatafileInfo     * sDatafileInfo;
    smfParallelLoadJob  * sJob;
    smfParallelLoadArgs * sArgs;

    STL_READ_MOVE_POINTER( &sArgs, aData, sOffset );

    /**
     * Load datafiles
     */
    
    STL_RING_FOREACH_ENTRY( &sArgs->mJobs, sJob )
    {
        sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[sJob->mTbsId];
        sDatafileInfo = sTbsInfo->mDatafileInfoArray[sJob->mDatafileId];
        
        sTbsTypeId = SML_TBS_DEVICE_TYPE_ID( sTbsInfo->mTbsPersData.mAttr );
        
        STL_TRY( gDatafileFuncs[sTbsTypeId]->mLoad( sTbsInfo,
                                                    sDatafileInfo,
                                                    aEnv )
                 == STL_SUCCESS );

        sDatafileInfo->mIoGroupIdx = sJob->mGroupId;
    }

    *aDone = STL_TRUE;

    return STL_SUCCESS;

    STL_FINISH;

    stlAppendErrors( &sArgs->mErrorStack,
                     KNL_ERROR_STACK( aEnv ) );

    *aDone = STL_TRUE;

    return STL_FAILURE;
}


stlStatus smfAcquireTablespace( smfTbsInfo * aTbsInfo,
                                smlEnv     * aEnv )
{
    stlInt32 sState = 0;
    stlBool  sIsTimedOut;
    
    STL_TRY( knlAcquireLatch( &gSmfWarmupEntry->mLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              (knlEnv*)aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( knlAcquireLatch( &aTbsInfo->mLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              (knlEnv*)aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)knlReleaseLatch( &gSmfWarmupEntry->mLatch, (knlEnv*)aEnv );
    }

    return STL_FAILURE;
}

stlStatus smfReleaseTablespace( smfTbsInfo * aTbsInfo,
                                smlEnv     * aEnv )
{
    STL_TRY( knlReleaseLatch( &aTbsInfo->mLatch,
                              (knlEnv*)aEnv ) == STL_SUCCESS );

    STL_TRY( knlReleaseLatch( &gSmfWarmupEntry->mLatch,
                              (knlEnv*)aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smfWriteDatafileHeader( stlFile           * aFile,
                                  smfDatafileHeader * aDatafileHeader,
                                  smlEnv            * aEnv )
{
    stlOffset   sOffset;
    stlChar     sBuffer[SMP_PAGE_SIZE * 2];
    stlChar   * sPage;
    stlUInt32   sChecksum = STL_CRC32_INIT_VALUE;
    
    sPage = (stlChar*)( STL_ALIGN( (stlInt64)sBuffer, SMP_PAGE_SIZE ) );
    
    sOffset = 0;
    STL_TRY( stlSeekFile( aFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    STL_TRY( stlCrc32( aDatafileHeader,
                       STL_SIZEOF( smfDatafileHeader ),
                       &sChecksum,
                       KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    stlMemset( sPage, 0x00, SMP_PAGE_SIZE );
    stlMemcpy( sPage, aDatafileHeader, STL_SIZEOF(smfDatafileHeader) );
    stlMemcpy( sPage + STL_SIZEOF( smfDatafileHeader ), &sChecksum, STL_SIZEOF(stlUInt32) );
    
    STL_TRY( stlWriteFile( aFile,
                           sPage,
                           SMP_PAGE_SIZE,
                           NULL,
                           KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smfReadDatafileHeader( stlFile           * aFile,
                                 stlChar           * aFilePath,
                                 smfDatafileHeader * aDatafileHeader,
                                 smlEnv            * aEnv )
{
    stlOffset   sOffset;
    stlChar     sBuffer[SMP_PAGE_SIZE * 2];
    stlChar   * sPage;
    stlUInt32   sChecksum;
    stlUInt32   sCrc = STL_CRC32_INIT_VALUE;

    sPage = (stlChar*)( STL_ALIGN( (stlInt64)sBuffer, SMP_PAGE_SIZE ) );
    
    sOffset = 0;
    STL_TRY( stlSeekFile( aFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    STL_TRY( stlReadFile( aFile,
                          sPage,
                          SMP_PAGE_SIZE,
                          NULL,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
    
    stlMemcpy( aDatafileHeader, sPage, STL_SIZEOF(smfDatafileHeader) );

    stlMemcpy( &sChecksum, sPage + STL_SIZEOF(smfDatafileHeader), STL_SIZEOF(stlUInt32) );

    STL_TRY( stlCrc32( aDatafileHeader,
                       STL_SIZEOF( smfDatafileHeader ),
                       &sCrc,
                       KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS);

    STL_TRY_THROW( sChecksum == sCrc, RAMP_ERR_DATAFILE_IS_CORRUPTED );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_DATAFILE_IS_CORRUPTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_DATAFILE_IS_CORRUPTED,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      aFilePath );
    }

    STL_FINISH;

    return STL_FAILURE;
}

static stlChar * smfGetTbsAttrString( stlChar   * aString,
                                      stlUInt16   aAttr )
{
    aString[0] = '\0';
    
    switch( SML_TBS_DEVICE_TYPE( aAttr ) )
    {
        case SML_TBS_DEVICE_MEMORY:
            stlStrcpy( aString, "MEMORY" );
            break;
        case SML_TBS_DEVICE_DISK:
            stlStrcpy( aString, "DISK" );
            break;
        default:
            stlStrcpy( aString, "UNKNOWN MEDIA" );
            break;
    }

    if( SML_TBS_IS_TEMPORARY( aAttr ) )
    {
        stlStrcat( aString, " | TEMPORARY" );
    }
    else
    {
        stlStrcat( aString, " | PERSISTENT" );
    }

    switch( SML_TBS_USAGE_TYPE( aAttr ) )
    {
        case SML_TBS_USAGE_DICT:
            stlStrcat( aString, " | DICT" );
            break;
        case SML_TBS_USAGE_UNDO:
            stlStrcat( aString, " | UNDO" );
            break;
        case SML_TBS_USAGE_DATA:
            stlStrcat( aString, " | DATA" );
            break;
        case SML_TBS_USAGE_TEMPORARY:
            stlStrcat( aString, " | TEMPORARY" );
            break;
        default:
            stlStrcat( aString, " | UNKNOWN USAGE" );
            break;
    }

    return aString;
}

void smfTbsAttrString( stlUInt16   aAttr,
                       stlChar   * aString )
{
    (void)smfGetTbsAttrString( aString, aAttr );
}

static stlChar * smfGetTbsStateString( stlUInt16  aTbsState )
{
    stlChar * sString = NULL;

    switch( aTbsState )
    {
        case SMF_TBS_STATE_INIT :
            sString = "INIT";
            break;
        case SMF_TBS_STATE_CREATING :
            sString = "CREATING";
            break;
        case SMF_TBS_STATE_CREATED :
            sString = "CREATED";
            break;
        case SMF_TBS_STATE_DROPPING :
            sString = "DROPPING";
            break;
        case SMF_TBS_STATE_AGING :
            sString = "AGING";
            break;
        case SMF_TBS_STATE_DROPPED :
            sString = "DROPPED";
            break;
        default :
            sString = "UNKNOWN";
            break;
    }

    return sString;
}

void smfTbsStateString( stlUInt16   aTbsState,
                        stlChar   * aString )
{
    stlStrcpy( aString, smfGetTbsStateString( aTbsState ) );
}

static stlChar * smfGetTbsOfflineStateString( stlUInt16  aOfflineState )
{
    stlChar * sString = NULL;

    switch( aOfflineState )
    {
        case SMF_OFFLINE_STATE_NULL :
            sString = "";
            break;
        case SMF_OFFLINE_STATE_CONSISTENT :
            sString = "CONSISTENT";
            break;
        case SMF_OFFLINE_STATE_INCONSISTENT :
            sString = "INCONSISTENT";
            break;
        default :
            sString = "UNKNOWN";
            break;
    }

    return sString;
}

void smfTbsOfflineStateString( stlUInt16   aOfflineState,
                               stlChar   * aString )
{
    stlStrcpy( aString, smfGetTbsOfflineStateString( aOfflineState ) );
}

static stlChar * smfGetDataFileStateString( stlUInt16  aDatafileState )
{
    stlChar * sString = NULL;

    switch( aDatafileState )
    {
        case SMF_DATAFILE_STATE_INIT :
            sString = "INIT";
            break;
        case SMF_DATAFILE_STATE_CREATING :
            sString = "CREATING";
            break;
        case SMF_DATAFILE_STATE_CREATED :
            sString = "CREATED";
            break;
        case SMF_DATAFILE_STATE_DROPPING :
            sString = "DROPPING";
            break;
        case SMF_DATAFILE_STATE_AGING :
            sString = "AGING";
            break;
        case SMF_DATAFILE_STATE_DROPPED :
            sString = "DROPPED";
            break;
        default :
            sString = "UNKNOWN";
            break;
    }

    return sString;
}

void smfDatafileStateString( stlUInt16   aDatafileState,
                             stlChar   * aString )
{
    stlStrcpy( aString, smfGetDataFileStateString( aDatafileState ) );
}

/**
 * @brief Control File의 Database Section을 Dump한다.
 * @param[in] aFilePath Control File Path
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smfDumpDbCtrlSection( stlChar * aFilePath,
                                smlEnv  * aEnv )
{
    stlFile             sFile;
    stlUInt32           i;
    stlUInt32           j;
    stlUInt32           sState = 0;
    smfDbPersData       sDbPersData;
    smfTbsPersData      sTbsPersData;
    smfDatafilePersData sDatafilePersData;
    stlChar             sString[256];
    smfCtrlBuffer       sBuffer;
    stlBool             sIsValid;

    STL_TRY( smfOpenCtrlSectionWithPath( &sFile,
                                         aFilePath,
                                         SMF_CTRL_SECTION_DB,
                                         &sIsValid, /* aIsValid */
                                         aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY_THROW( sIsValid == STL_TRUE, RAMP_ERR_CTRLFILE_CORRUPTED );

    SMF_INIT_CTRL_BUFFER( &sBuffer );

    stlPrintf( "\n [DB SECTION]\n" );
    stlPrintf( "-----------------------------------------------------------\n" );
    
    STL_TRY( smfReadCtrlFile( &sFile,
                              &sBuffer,
                              (stlChar *)&sDbPersData,
                              STL_SIZEOF( smfDbPersData ),
                              aEnv ) == STL_SUCCESS );

    stlPrintf( "  [DATABASE]\n"
               "    TRANSACTION_TABLE_SIZE : %d\n"
               "    UNDO_RELATION_COUNT    : %d\n"
               "    TABLESPACE COUNT       : %d\n"
               "    NEW TABLESPACE ID      : %d\n",
               sDbPersData.mTransTableSize,
               sDbPersData.mUndoRelCount,
               sDbPersData.mTbsCount,
               sDbPersData.mNewTbsId );

    for( i = 0; i < sDbPersData.mTbsCount; i++ )
    {
        STL_TRY( smfReadCtrlFile( &sFile,
                                  &sBuffer,
                                  (stlChar *)&sTbsPersData,
                                  STL_SIZEOF( smfTbsPersData ),
                                  aEnv ) == STL_SUCCESS );

        stlPrintf( "\n  [TABLESPACE #%d] \n"
                   "   NAME          :  %s\n"
                   "   ATTRIBUTES    :  %s\n"
                   "   STATE         :  %s\n"
                   "   LOGGING STATE :  %s\n"
                   "   ONLINE STATE  :  %s\n"
                   "   EXTENT_SIZE   :  %d\n"
                   "   RELATION_ID   :  %ld\n",
                   sTbsPersData.mTbsId,
                   sTbsPersData.mName,
                   smfGetTbsAttrString( sString, sTbsPersData.mAttr ),
                   smfGetTbsStateString( sTbsPersData.mState ),
                   (sTbsPersData.mIsLogging == STL_TRUE) ? "LOGGING" : "NOLOGGING",
                   (sTbsPersData.mIsOnline == STL_TRUE) ? "ONLINE" : "OFFLINE",
                   sTbsPersData.mPageCountInExt,
                   sTbsPersData.mRelationId );
        
        for( j = 0; j < sTbsPersData.mDatafileCount; j++ )
        {
            STL_TRY( smfReadCtrlFile( &sFile,
                                      &sBuffer,
                                      (stlChar *)&sDatafilePersData,
                                      STL_SIZEOF( smfDatafilePersData ),
                                      aEnv ) == STL_SUCCESS );

            stlPrintf( "\n   [DATAFILE #%d]\n"
                       "     SIZE  :  %ld\n"
                       "     STATE :  %s\n"
                       "     NAME  :  \"%s\"\n"
                       "     CHKPT LSN : %ld\n"
                       "     CHKPT LID : (%ld, %d, %d)\n",
                       sDatafilePersData.mID,
                       sDatafilePersData.mSize,
                       smfGetDataFileStateString( sDatafilePersData.mState ),
                       sDatafilePersData.mName,
                       sDatafilePersData.mCheckpointLsn,
                       SMR_FILE_SEQ_NO( &sDatafilePersData.mCheckpointLid ),
                       SMR_BLOCK_SEQ_NO( &sDatafilePersData.mCheckpointLid ),
                       SMR_BLOCK_OFFSET( &sDatafilePersData.mCheckpointLid ) );
        }
    }
    
    sState = 0;
    STL_TRY( smfCloseCtrlSection( &sFile,
                                  aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_RAMP( RAMP_ERR_CTRLFILE_CORRUPTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_CTRLFILE_CORRUPTED,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      aFilePath );
    }

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smfCloseCtrlSection( &sFile, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smfSetTablespaceOfflineState( smlTbsId   aTbsId,
                                        stlInt8    aState,
                                        smrLsn     aOfflineLsn,
                                        smlEnv   * aEnv )
{
    smfTbsInfo  * sTbsInfo;

    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];
    sTbsInfo->mTbsPersData.mOfflineState = aState;
    sTbsInfo->mTbsPersData.mOfflineLsn = aOfflineLsn;
    sTbsInfo->mTbsPersData.mIsOnline = STL_FALSE;
    
    return STL_SUCCESS;
}

inline smrLsn smfGetOfflineLsn( smlTbsId    aTbsId )
{
    smrLsn        sOfflineLsn;
    smfTbsInfo  * sTbsInfo;

    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];

    sOfflineLsn = SMR_INIT_LSN;

    if( SMF_IS_ONLINE_TBS( aTbsId ) != STL_TRUE )
    {
        sOfflineLsn = sTbsInfo->mTbsPersData.mOfflineLsn;
    }

    return sOfflineLsn;
}

inline void smfSetOfflineLsn( smlTbsId aTbsId,
                              smrLsn   aLsn )
{
    smfTbsInfo * sTbsInfo;

    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];

    sTbsInfo->mTbsPersData.mOfflineLsn = aLsn;
}
 
/**
 * @brief 모든 Tablespace의 모든 Datafile header의 checkpoint Lsn을 변경한다.
 * @param[in]     aStatement     Statement 포인터
 * @param[in]     aReason        Datafile Header를 변경하는 원인
 * @param[in]     aTbsId         Tablespace identifier
 * @param[in]     aDatafileId    Datafile identifier
 * @param[in]     aChkptLid      Datafile Header에 기록할 checkpoint Lid
 * @param[in]     aChkptLsn      Datafile Header에 기록할 checkpoint Lsn
 * @param[in,out] aEnv           Environment 정보
 */
stlStatus smfChangeDatafileHeader( smlStatement  * aStatement,
                                   stlInt32        aReason,
                                   smlTbsId        aTbsId,
                                   smlDatafileId   aDatafileId,
                                   smrLid          aChkptLid,
                                   smrLsn          aChkptLsn,
                                   smlEnv        * aEnv )
{
    smrLid            sChkptLid;
    smrLsn            sChkptLsn;
    smlTbsId          sTbsId = SML_INVALID_TBS_ID;
    smlDatafileId     sDatafileId = SML_INVALID_DATAFILE_ID;
    stlBool           sTbsExist;
    stlBool           sFileExist;
    stlBool           sIsTimedOut;
    stlInt32          sState = 0;
    smfTbsInfo      * sTbsInfo;
    smfDatafileInfo * sDatafileInfo;

    sChkptLsn = aChkptLsn;
    sChkptLid = aChkptLid;

    /**
     * Checkpoint와 datafile header 기록 사이의 동시성 제어
     * - Checkpoint 시 최신 checkpoint 정보를 datafile header에 직접 기록할 때
     *   Flusher 에 의한 datafile header 변경 사이의 동시성 제어가 필요하다.
     */
    STL_TRY( knlAcquireLatch( &gSmfWarmupEntry->mLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_UINT64_MAX,
                              &sIsTimedOut,
                              (knlEnv*)aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( smfFirstTbs( &sTbsId, &sTbsExist, aEnv ) == STL_SUCCESS );

    /**
     * 모든 tablespace의 모든 datafile에 checkpoint Lsn을 기록한다.
     */
    while( sTbsExist )
    {
        if( aTbsId == SML_INVALID_TBS_ID )
        {
            /**
             * 모든 tablespace의 datafile header를 flush할 경우
             * Online Tablespace의 datafile header만 기록한다.
             */
            if( SMF_IS_ONLINE_TBS( sTbsId ) == STL_FALSE )
            {
                STL_TRY( smfNextTbs( &sTbsId,
                                     &sTbsExist,
                                     aEnv ) == STL_SUCCESS );

                continue;
            }
        }
        else
        {
            /**
             * 특정 tablespace의 datafile header만 flush할 경우
             */
            if( aTbsId != sTbsId )
            {
                STL_TRY( smfNextTbs( &sTbsId,
                                     &sTbsExist,
                                     aEnv ) == STL_SUCCESS );

                continue;
            }
        }

        sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[sTbsId];

        STL_TRY( smfFirstDatafile( sTbsId,
                                   &sDatafileId,
                                   &sFileExist,
                                   aEnv ) == STL_SUCCESS );

        while( sFileExist )
        {
            if( (aDatafileId != SML_INVALID_DATAFILE_ID) &&
                (aDatafileId != sDatafileId) )
            {
                STL_TRY( smfNextDatafile( sTbsId,
                                          &sDatafileId,
                                          &sFileExist,
                                          aEnv ) == STL_SUCCESS );

                continue;
            }

            /**
             * Checkpoint에 의한 datafile header의 기록이 아니거나
             * Checkpoint에 의해 flush된 datafile header의 기록일 때만
             * datafile header의 checkpoint Lsn을 변경한다.
             */
            if( (aReason != SMF_CHANGE_DATAFILE_HEADER_FOR_CHECKPOINT) ||
                (SMF_TBS_GET_SKIP_FLUSH_STATE(sTbsId, sDatafileId) != STL_TRUE) )
            {
                /**
                 * aChkptLsn이 Valid 하지 않다면 상황에 맞게 Checkpoint Lsn을 얻어야 한다.
                 * Reason이 CHECKPOINT, MEDIA RECOVERY라면 Valid한 값이 들어온다.
                 */ 
                if( SMR_IS_VALID_LOG_LSN(aChkptLsn) == STL_FALSE )
                {
                    if( aReason == SMF_CHANGE_DATAFILE_HEADER_FOR_RESTORE )
                    {
                        sDatafileInfo = sTbsInfo->mDatafileInfoArray[sDatafileId];

                        sChkptLsn = sDatafileInfo->mRestoredChkptLsn;
                        sChkptLid = sDatafileInfo->mRestoredChkptLid;
                    }
                    else
                    {
                        smrGetConsistentChkptLsnAndLid( &sChkptLsn, &sChkptLid );
                    }
                }

                STL_TRY( smfChangeDatafileHeaderInternal( aStatement,
                                                          sTbsId,
                                                          sDatafileId,
                                                          sChkptLid,
                                                          sChkptLsn,
                                                          aEnv ) == STL_SUCCESS );
            }

            STL_TRY( smfNextDatafile( sTbsId,
                                      &sDatafileId,
                                      &sFileExist,
                                      aEnv ) == STL_SUCCESS );
        }

        STL_TRY( smfNextTbs( &sTbsId,
                             &sTbsExist,
                             aEnv ) == STL_SUCCESS );
    }

    /**
     * Checkpoint에 의한 datafile header 변경 시에는 datafile header를 flush한 후
     * controlfile에 최신의 checkpoint Lsn, Lid를 설정한다.
     * 이때 Lsn과 Lid가 모두 설정되지 않은 상태에서 control file이 저장되지 않도록 한다.
     */
    if( (aReason == SMF_CHANGE_DATAFILE_HEADER_FOR_CHECKPOINT) ||
        (aReason == SMF_CHANGE_DATAFILE_HEADER_FOR_INCOMPLETE_RECOVERY) )
    {
        smrSetConsistentChkptLsnAndLid( aChkptLsn, aChkptLid );
    }

    sState = 0;
    STL_TRY( knlReleaseLatch( &gSmfWarmupEntry->mLatch,
                              (knlEnv*)aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)knlReleaseLatch( &gSmfWarmupEntry->mLatch, (knlEnv*)aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief Datafile header의 checkpoint Lsn을 변경한다.
 * @param[in]     aStatement     Statement 포인터
 * @param[in]     aTbsId         Tablespace identifier
 * @param[in]     aDatafileId    Datafile identifier
 * @param[in]     aChkptLid      Datafile Header에 기록할 checkpoint Lid
 * @param[in]     aChkptLsn      Datafile Header에 기록할 checkpoint Lsn
 * @param[in,out] aEnv           Environment 정보
 */
stlStatus smfChangeDatafileHeaderInternal( smlStatement  * aStatement,
                                           smlTbsId        aTbsId,
                                           smlDatafileId   aDatafileId,
                                           smrLid          aChkptLid,
                                           smrLsn          aChkptLsn,
                                           smlEnv        * aEnv )
{
    smfTbsInfo        * sTbsInfo;
    smfDatafileInfo   * sDatafileInfo;

    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];

    /**
     * Drop된 테이블스페이스는 무시한다.
     */
    STL_TRY_THROW( sTbsInfo != NULL, SKIP_WRITE_DATAFILE_HEADER );
    STL_TRY_THROW( sTbsInfo->mTbsPersData.mState != SMF_TBS_STATE_DROPPED,
                   SKIP_WRITE_DATAFILE_HEADER );
    STL_TRY_THROW( sTbsInfo->mTbsPersData.mState != SMF_TBS_STATE_AGING,
                   SKIP_WRITE_DATAFILE_HEADER );
    STL_TRY_THROW( SML_TBS_IS_MEMORY_TEMPORARY(sTbsInfo->mTbsPersData.mAttr) == STL_FALSE,
                   SKIP_WRITE_DATAFILE_HEADER );

    sDatafileInfo = sTbsInfo->mDatafileInfoArray[aDatafileId];

    /**
     * Drop된 데이터파일은 무시한다.
     */
    STL_TRY_THROW( sDatafileInfo != NULL, SKIP_WRITE_DATAFILE_HEADER );
    STL_DASSERT( sDatafileInfo->mDatafilePersData.mState != SMF_DATAFILE_STATE_INIT );
    STL_TRY_THROW( sDatafileInfo->mDatafilePersData.mState != SMF_DATAFILE_STATE_DROPPED,
                   SKIP_WRITE_DATAFILE_HEADER );
    STL_TRY_THROW( sDatafileInfo->mDatafilePersData.mState != SMF_DATAFILE_STATE_AGING,
                   SKIP_WRITE_DATAFILE_HEADER );

    /**
     * write FILE HEADER
     * - File header에 checkpoint Lsn을 설정하여 disk에 flush한다.
     */
    if( smfRewriteDatafileHeader( sDatafileInfo,
                                  aChkptLid,
                                  aChkptLsn,
                                  aEnv ) == STL_FAILURE )
    {
        /**
         * Drop된 테이블스페이스라면 무시한다.
         */
        if( (sTbsInfo == NULL) ||
            (sTbsInfo->mTbsPersData.mState == SMF_TBS_STATE_DROPPED) ||
            (sTbsInfo->mTbsPersData.mState == SMF_TBS_STATE_AGING) )
        {
            stlPopError( KNL_ERROR_STACK(aEnv) );
            STL_THROW( SKIP_WRITE_DATAFILE_HEADER );
        }

        /**
         * Drop된 데이터파일이라면 무시한다.
         */
        if( (sDatafileInfo == NULL) ||
            (sDatafileInfo->mDatafilePersData.mState == SMF_DATAFILE_STATE_DROPPED) ||
            (sDatafileInfo->mDatafilePersData.mState == SMF_DATAFILE_STATE_AGING) )
        {
            stlPopError( KNL_ERROR_STACK(aEnv) );
            STL_THROW( SKIP_WRITE_DATAFILE_HEADER );
        }

        /**
         * Transaction이 begin된 상태에서 datafile header가 기록되는 경우는
         * CREATE TABLESPACE 혹은 ADD DATAFILE의 DDL 뿐이다.
         */ 
        if( aStatement != NULL )
        {
            /**
             * CREATE TABLESPACE 혹은 ADD DATAFILE 에 의한 datafile header 변경 시
             * Datafile의 상태는 반드시 CREATING 이다.
             */
            STL_DASSERT( sDatafileInfo->mDatafilePersData.mState == SMF_DATAFILE_STATE_CREATING );

            /**
             * CREATE TABLESPACE, ADD DATAFILE 수행 중인 경우 exception처리
             */
            STL_THROW( RAMP_ERR_INVALID_DATAFILE );
        }

        /**
         * 그렇지 않은 경우는 파일에 문제가 발생한 경우이다.
         */

        if( (aTbsId == SML_MEMORY_DICT_SYSTEM_TBS_ID) ||
            (aTbsId == SML_MEMORY_UNDO_SYSTEM_TBS_ID) )
        {
            /**
             * Offline이 불가능한 테이블 스페이스라면 시스템을 비정상 종료시킨다.
             */

            knlSystemFatal( "can't write page to datafile", KNL_ENV(aEnv) );
        }
        else
        {
            /**
             * Offline이 가능한 테이블 스페이스라면 offline시키고 service를 계속한다.
             */

            stlPushError( STL_ERROR_LEVEL_WARNING,
                          SML_ERRCODE_TABLESPACE_OFFLINE_ABNORMALLY,
                          NULL,
                          KNL_ERROR_STACK(aEnv),
                          sTbsInfo->mTbsPersData.mName );

            STL_TRY( knlLogMsg( NULL,
                                KNL_ENV(aEnv),
                                KNL_LOG_LEVEL_WARNING,
                                "failed to write datafile header - The tablespace '%s' offlined\n",
                                sTbsInfo->mTbsPersData.mName )
                     == STL_SUCCESS );

            if( ((SMF_IS_ONLINE_TBS( aTbsId ) == STL_FALSE) &&
                 (SMF_GET_TBS_OFFLINE_STATE( aTbsId ) == SMF_OFFLINE_STATE_CONSISTENT)) ||
                (SMF_IS_ONLINE_TBS( aTbsId ) == STL_TRUE) )
            {
                STL_ASSERT( aStatement == NULL );

                /**
                 * setting offline
                 */

                STL_TRY( smfOfflineTbs( NULL,
                                        aTbsId,
                                        SML_OFFLINE_BEHAVIOR_IMMEDIATE,
                                        aEnv )
                         == STL_SUCCESS );
            }
        }
    }

    STL_RAMP( SKIP_WRITE_DATAFILE_HEADER );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_DATAFILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INVALID_DATAFILE,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      sDatafileInfo->mDatafilePersData.mName );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Datafile header의 checkpoint Lsn을 변경한다.
 * @param[in]     aDatafileInfo  Datafile 정보
 * @param[in]     aChkptLid      Datafile Header에 기록할 checkpoint Lid
 * @param[in]     aChkptLsn      Datafile Header에 기록할 checkpoint Lsn
 * @param[in,out] aEnv           Environment 정보
 */
stlStatus smfRewriteDatafileHeader( smfDatafileInfo * aDatafileInfo,
                                    smrLid            aChkptLid,
                                    smrLsn            aChkptLsn,
                                    smlEnv          * aEnv )
{
    smfDatafileHeader     sDatafileHeader;
    stlFile               sFile;
    stlInt32              sFileState = 0;
    stlInt32              sFlag;
    stlInt64              sDatabaseIo;

    sFlag = STL_FOPEN_LARGEFILE | STL_FOPEN_WRITE | STL_FOPEN_READ;

    sDatabaseIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_DATABASE_FILE_IO,
                                                 KNL_ENV(aEnv) );

    if( sDatabaseIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }

    KNL_BREAKPOINT( KNL_BREAKPOINT_SMFREWRITEDATAFILEHEADER_BEFORE_WRITE,
                    KNL_ENV(aEnv) );

    STL_TRY( smgOpenFile( &sFile,
                          aDatafileInfo->mDatafilePersData.mName,
                          sFlag,
                          STL_FPERM_OS_DEFAULT,
                          aEnv )
             == STL_SUCCESS );
    sFileState = 1;

    STL_TRY( smfReadDatafileHeader( &sFile,
                                    aDatafileInfo->mDatafilePersData.mName,
                                    &sDatafileHeader,
                                    SML_ENV(aEnv) )
             == STL_SUCCESS );

    sDatafileHeader.mCheckpointLsn = aChkptLsn;
    sDatafileHeader.mCheckpointLid = aChkptLid;

    STL_TRY( smfWriteDatafileHeader( &sFile,
                                     &sDatafileHeader,
                                     aEnv )
             == STL_SUCCESS );

    aDatafileInfo->mDatafilePersData.mCheckpointLsn = sDatafileHeader.mCheckpointLsn;
    aDatafileInfo->mDatafilePersData.mCheckpointLid = sDatafileHeader.mCheckpointLid;

    sFileState = 0;
    STL_TRY( smgCloseFile( &sFile,
                           aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sFileState == 1 )
    {
        (void)smgCloseFile( &sFile, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smfCheckDatafile4IncompleteMediaRecovery( smrLsn   aLsn,
                                                    smlEnv * aEnv )
{
    smfTbsInfo       * sTbsInfo;
    smfDatafileInfo  * sDatafileInfo;
    smfDatafileHeader  sDatafileHeader;
    smlDatafileId      sDatafileId = SML_INVALID_DATAFILE_ID;
    smlTbsId           sTbsId = SML_INVALID_TBS_ID;
    stlInt32           sFileState = 0;
    stlFile            sFile;
    stlBool            sExistTbs;
    stlBool            sExistFile;
    stlInt32           sFlag;
    stlInt64           sDatabaseIo;

    sFlag = STL_FOPEN_LARGEFILE | STL_FOPEN_READ;

    sDatabaseIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_DATABASE_FILE_IO,
                                                 KNL_ENV(aEnv) );

    if( sDatabaseIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }

    STL_TRY( smfFirstTbs( &sTbsId, &sExistTbs, aEnv ) == STL_SUCCESS );

    /**
     * 모든 tablespace의 모든 datafile에 checkpoint Lsn을 check한다.
     */
    while( sExistTbs == STL_TRUE )
    {
        /**
         * Online Tablespace가 아니면 skip
         */
        if( SMF_IS_ONLINE_TBS( sTbsId ) == STL_TRUE )
        {
            sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[sTbsId];

            /**
             * temporary tablespace는 제외
             */
            if( (sTbsInfo != NULL) &&
                (SML_TBS_IS_TEMPORARY( sTbsInfo->mTbsPersData.mAttr ) != STL_TRUE) )
            {
                STL_TRY( smfFirstDatafile( sTbsId,
                                           &sDatafileId,
                                           &sExistFile,
                                           aEnv ) == STL_SUCCESS );

                while( sExistFile == STL_TRUE )
                {
                    sDatafileInfo = sTbsInfo->mDatafileInfoArray[sDatafileId];

                    STL_TRY( smgOpenFile( &sFile,
                                          sDatafileInfo->mDatafilePersData.mName,
                                          sFlag,
                                          STL_FPERM_OS_DEFAULT,
                                          aEnv )
                             == STL_SUCCESS );
                    sFileState = 1;

                    STL_TRY( smfReadDatafileHeader( &sFile,
                                                    sDatafileInfo->mDatafilePersData.mName,
                                                    &sDatafileHeader,
                                                    SML_ENV(aEnv) )
                             == STL_SUCCESS );

                    sFileState = 0;
                    STL_TRY( smgCloseFile( &sFile,
                                           aEnv )
                             == STL_SUCCESS );

                    /**
                     * 불완전 복구 시 datafile이 마지막으로 redo된 log Lsn보다
                     * 크면 불완전 복구를 완료할 수 없다.
                     */
                    STL_TRY_THROW( sDatafileHeader.mCheckpointLsn < aLsn,
                                   RAMP_ERR_NOT_SUFFICIENT_OLD_BACKUP );

                    STL_TRY( smfNextDatafile( sTbsId,
                                              &sDatafileId,
                                              &sExistFile,
                                              aEnv ) == STL_SUCCESS );
                }
            }
        }

        STL_TRY( smfNextTbs( &sTbsId,
                             &sExistTbs,
                             aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_SUFFICIENT_OLD_BACKUP )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_NOT_SUFFICIENT_OLD_BACKUP,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      sDatafileInfo->mDatafilePersData.mName );
    }

    STL_FINISH;

    if( sFileState == 1 )
    {
        (void)smgCloseFile( &sFile, aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief 특정 Datafile의 DatafileHeader를 Buffer에 Copy를 한다.
 * @param[in]      aTbsId       Tablespace Id
 * @param[in]      aDatafileId  Datafile Id
 * @param[out]     aBuffer      Target Buffer
 * @param[in,out]  aEnv         Environment 정보
 */ 
stlStatus smfGetDatafileHeader( smlTbsId         aTbsId,
                                smlDatafileId    aDatafileId,
                                void           * aBuffer,
                                smlEnv         * aEnv )
{
    smfDatafileInfo  * sDatafileInfo;
    stlFile            sFile;
    stlChar            sBuffer[SMP_PAGE_SIZE * 2];
    stlChar          * sPage;
    stlChar          * sFileName;
    stlInt32           sFlag  = 0;
    stlInt32           sState = 0;
    stlInt64           sDatabaseIo;

    sFlag = ( STL_FOPEN_READ | STL_FOPEN_LARGEFILE );
    sDatabaseIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_DATABASE_FILE_IO,
                                                 KNL_ENV(aEnv) );
    if( sDatabaseIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }
    
    sDatafileInfo = smfGetDatafileInfo( aTbsId, aDatafileId );
    sFileName = sDatafileInfo->mDatafilePersData.mName;
    
    sPage = (stlChar*)( STL_ALIGN( (stlInt64)sBuffer, SMP_PAGE_SIZE ) );
    
    STL_TRY( smgOpenFile( &sFile,
                          sFileName,
                          sFlag,
                          STL_FPERM_OS_DEFAULT,
                          aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( stlReadFile( &sFile,
                          sPage,
                          SMP_PAGE_SIZE,
                          NULL,
                          KNL_ERROR_STACK(aEnv) ) == STL_SUCCESS );

    stlMemcpy( aBuffer, sPage, SMP_PAGE_SIZE );

    sState = 0;
    STL_TRY( smgCloseFile( &sFile,
                           aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void) smgCloseFile( &sFile,
                                 aEnv );
            break;
        default:
            break;
    }
    return STL_FAILURE;
}
 
stlStatus smfFillDummyTbsInfo( smfTbsInfo ** aTbsInfoArray,
                               smlEnv      * aEnv )
{
    smfInitialTbsData * sInitialTbsData;
    stlInt32            i;
    smfTbsInfo        * sTbsInfo;
    
    for( i = 0; gSmfNoMountTbsInfoArray[i].mName != NULL; i++ )
    {
        STL_TRY( knlCacheAlignedAllocFixedStaticArea( STL_SIZEOF(smfTbsInfo),
                                                      (void**)&sTbsInfo,
                                                      (knlEnv*)aEnv )
                 == STL_SUCCESS );

        stlMemset( sTbsInfo, 0x00, STL_SIZEOF(smfTbsInfo) );
        
        sInitialTbsData = &gSmfNoMountTbsInfoArray[i];
        
        sTbsInfo->mTbsPersData.mTbsId = sInitialTbsData->mTbsId;
        sTbsInfo->mTbsPersData.mAttr = sInitialTbsData->mAttr;
        sTbsInfo->mTbsPersData.mState = SMF_TBS_STATE_CREATED;
        sTbsInfo->mTbsPersData.mPageCountInExt = sInitialTbsData->mPageCountInExt;

        stlStrncpy( sTbsInfo->mTbsPersData.mName,
                    sInitialTbsData->mName,
                    STL_MAX_SQL_IDENTIFIER_LENGTH );
        
        aTbsInfoArray[i] = sTbsInfo;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smfValidateOfflineTablespace( smlOfflineBehavior   aOfflineBehavior,
                                        smlEnv             * aEnv )
{
    if( knlGetCurrStartupPhase() == KNL_STARTUP_PHASE_MOUNT )
    {
        /* MOUNT phase에서 validation */
        if( smfGetServerState() != SML_SERVER_STATE_SHUTDOWN )
        {
            /* 비정상 종료되었을 경우 OFFLINE IMMEDIATE만 가능 */
            STL_TRY_THROW( aOfflineBehavior == SML_OFFLINE_BEHAVIOR_IMMEDIATE,
                           RAMP_ERR_TABLESPACE_OFFLINE_NORMAL );

            /* NOARCHIVELOG mode이면 OFFLINE IMMEDIATE 불가능 */
            STL_TRY_THROW( smrGetArchivelogMode() == SML_ARCHIVELOG_MODE,
                           RAMP_ERR_TABLESPACE_OFFLINE_IMMEDIATE );
        }
        else
        {
            /* 서버가 정상 종료된 후 MOUNT phase에서는 ARCHIVELOG에 관계없이
             * Offline Tablespace를 수행할 수 있다. */
        }
    }
    else
    {
        /* OPEN phase에서 validation */
        if( aOfflineBehavior == SML_OFFLINE_BEHAVIOR_IMMEDIATE )
        {
            /* ARCHIVELOG mode에서만 OFFLINE IMMEDIATE가 가능 */
            STL_TRY_THROW( smrGetArchivelogMode() == SML_ARCHIVELOG_MODE,
                           RAMP_ERR_TABLESPACE_OFFLINE_IMMEDIATE );
        }
        else
        {
            /* OPEN phase에서 OFFLINE NORMAL은 ARCHIVELOG mode와 관계없이 수행 가능 */
        }
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_TABLESPACE_OFFLINE_NORMAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_TABLESPACE_OFFLINE_NORMAL,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_CATCH( RAMP_ERR_TABLESPACE_OFFLINE_IMMEDIATE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_TABLESPACE_OFFLINE_IMMEDIATE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus smfValidateOfflineTbsConsistency( smlEnv  * aEnv )
{
    smfTbsPersData        sTbsPersData;
    void                * sTbsIterator;

    stlMemset( &sTbsPersData, 0x00, STL_SIZEOF(smfTbsPersData) );
    
    STL_TRY( smfFirstTbsPersData( &sTbsPersData,
                                  &sTbsIterator,
                                  aEnv ) == STL_SUCCESS );

    while( sTbsIterator != NULL )
    {
        if( (sTbsPersData.mState == SMF_TBS_STATE_CREATED) ||
            (sTbsPersData.mState == SMF_TBS_STATE_CREATING) )
        {
            /* 생성된 후 drop되지 않은 offline tablespace들 중에
             * Media Recovery가 필요한 tablespace가 존재하는지 체크
             */ 
            if( (SML_TBS_IS_TEMPORARY( sTbsPersData.mAttr ) != STL_TRUE) &&
                (SMF_IS_ONLINE_TBS( sTbsPersData.mTbsId ) != STL_TRUE) )
            {
                STL_TRY_THROW( smfValidateOnlineTbs( sTbsPersData.mTbsId,
                                                     aEnv ) == STL_SUCCESS,
                               RAMP_ERR_NOARCHIVELOG_MODE );
            }
        }

        STL_TRY( smfNextTbsPersData( &sTbsPersData,
                                     &sTbsIterator,
                                     aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOARCHIVELOG_MODE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_CHANGE_NOARCHIVELOG_MODE,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Media Recovery 전에 Validation.
 * @param[in] aTbsId  tablespace id
 * @param[in] aLastLsn  online logfile의 last lsn
 * @param[in] aRecoveryObject  recovery object type
 * @param[in,out] aEnv  environment
 * @remark Recovery 대상인 Datafile은 최신 online log의 마지막 Log보다 큰 Lsn을 갖을 수 없다.
 */ 
stlStatus smfValidateMediaRecoveryDatafile( smlTbsId   aTbsId,
                                            smrLsn     aLastLsn,
                                            stlUInt8   aRecoveryObject,
                                            smlEnv   * aEnv)
{
    smfTbsPersData        sTbsPersData;
    void                * sTbsIterator;
    smfDatafilePersData   sDatafilePersData;
    void                * sDatafileIterator;
    stlInt32              sState = 0;
    stlFile               sFile;
    smfDatafileHeader     sDatafileHeader;
    stlInt32              sFlag;
    stlInt64              sDatabaseIo;
    stlBool               sFileExist;

    stlMemset( &sTbsPersData, 0x00, STL_SIZEOF(smfTbsPersData) );

    sFlag = STL_FOPEN_READ | STL_FOPEN_LARGEFILE;

    sDatabaseIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_DATABASE_FILE_IO,
                                                 KNL_ENV(aEnv) );

    if( sDatabaseIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }

    STL_TRY( smfFirstTbsPersData( &sTbsPersData,
                                  &sTbsIterator,
                                  aEnv ) == STL_SUCCESS );

    while( STL_TRUE )
    {
        if( sTbsIterator == NULL )
        {
            break;
        }

        if( (aTbsId != SML_INVALID_TBS_ID) &&
            (sTbsPersData.mTbsId != aTbsId ) )
        {
            STL_TRY( smfNextTbsPersData( &sTbsPersData,
                                         &sTbsIterator,
                                         aEnv ) == STL_SUCCESS );
            continue;
        }
        
        if( (SML_TBS_IS_MEMORY_TEMPORARY( sTbsPersData.mAttr ) == STL_TRUE) ||
            (sTbsPersData.mState == SMF_TBS_STATE_AGING)   ||
            (sTbsPersData.mState == SMF_TBS_STATE_DROPPING) )
        {
            STL_TRY( smfNextTbsPersData( &sTbsPersData,
                                         &sTbsIterator,
                                         aEnv ) == STL_SUCCESS );
            continue;
        }

        /**
         * Database 단위 복구에서는 Offline Tbs에 대해 검사하지 않는다.
         */ 
        if( (aRecoveryObject == SML_RECOVERY_OBJECT_TYPE_DATABASE) &&
            (SMF_IS_ONLINE_TBS( sTbsPersData.mTbsId ) == STL_FALSE) )
        {
            STL_TRY( smfNextTbsPersData( &sTbsPersData,
                                         &sTbsIterator,
                                         aEnv ) == STL_SUCCESS );
            continue;
        }
                

        STL_TRY( smfFirstDatafilePersData( sTbsIterator,
                                           &sDatafilePersData,
                                           &sDatafileIterator,
                                           aEnv ) == STL_SUCCESS );

        while( STL_TRUE )
        {
            if( sDatafileIterator == NULL )
            {
                break;
            }

            if( (sDatafilePersData.mState == SMF_DATAFILE_STATE_DROPPING) ||
                (sDatafilePersData.mState == SMF_DATAFILE_STATE_AGING) )
            {
                STL_TRY( smfNextDatafilePersData( sTbsIterator,
                                                  &sDatafilePersData,
                                                  &sDatafileIterator,
                                                  aEnv ) == STL_SUCCESS );
                continue;
            }

            STL_TRY( stlExistFile( sDatafilePersData.mName,
                                   &sFileExist,
                                   KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

            STL_TRY_THROW( sFileExist == STL_TRUE, RAMP_ERR_FILE_NOT_EXIST );

            STL_TRY( smgOpenFile( &sFile,
                                  sDatafilePersData.mName,
                                  sFlag,
                                  STL_FPERM_OS_DEFAULT,
                                  aEnv ) == STL_SUCCESS );
            sState = 1;

            /**
             * Header를 읽으면서 Header의 CRC 검사를 한다.
             */ 
            STL_TRY( smfReadDatafileHeader( &sFile,
                                            sDatafilePersData.mName,
                                            &sDatafileHeader,
                                            aEnv ) == STL_SUCCESS );
            
            sState = 0;
            STL_TRY( smgCloseFile( &sFile,
                                   aEnv ) == STL_SUCCESS );

            STL_TRY_THROW( SMR_IS_VALID_LOG_LSN(sDatafileHeader.mCheckpointLsn) == STL_TRUE,
                           RAMP_ERR_INVALID_DATAFILE );

            /**
             * Datafile Header의 CheckpointLsn 검사.
             */
            STL_TRY_THROW( sDatafileHeader.mCheckpointLsn <= aLastLsn,
                           RAMP_ERR_DATAFILE_MORE_RECENT );

            STL_TRY( smfNextDatafilePersData( sTbsIterator,
                                              &sDatafilePersData,
                                              &sDatafileIterator,
                                              aEnv ) == STL_SUCCESS );
        }
        
        STL_TRY( smfNextTbsPersData( &sTbsPersData,
                                     &sTbsIterator,
                                     aEnv ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_FILE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_FILE_NOT_EXIST,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      sDatafilePersData.mName );
    }
    
    STL_CATCH( RAMP_ERR_DATAFILE_MORE_RECENT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_DATAFILE_RECENT_THAN_REDO_LOGFILE,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      sDatafilePersData.mName );
    }

    STL_CATCH( RAMP_ERR_INVALID_DATAFILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INVALID_DATAFILE,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      sDatafilePersData.mName );
    }
    
    STL_FINISH;

    if( sState == 1 )
    {
        (void)smgCloseFile( &sFile, aEnv );
    }

    return STL_FAILURE;
}


/** @} */
