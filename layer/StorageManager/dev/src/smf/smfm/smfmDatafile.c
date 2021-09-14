/*******************************************************************************
 * smfmDatafile.c
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
#include <smf.h>
#include <smfDef.h>
#include <smfmDatafile.h>
#include <smfTbsMgr.h>
#include <smp.h>
#include <smg.h>
#include <smr.h>
#include <smxm.h>
#include <smfmRecovery.h>

/**
 * @file smfmDatafile.c
 * @brief Storage Manager Layer Memory Datafile Component Internal Routines
 */

/**
 * @addtogroup smfmDatafile
 * @{
 */

smfDatafileFuncs gMemoryDatafileFuncs =
{
    smfmCreate,
    smfmPrepare,
    smfmLoad,
    smfmDrop,
    smfmExtend,
    smfmWritePage,
    smfmCreateRedo,
    smfmCreateUndo,
    smfmDropRedo,
    smfmDropUndo
};

stlStatus smfmCreate( smxlTransId        aTransId,
                      stlInt32           aTbsPhysicalId,
                      stlInt32           aHintTbsId,
                      smfTbsInfo       * aTbsInfo,
                      smlDatafileAttr  * aDatafileAttr,
                      stlBool            aUndoLogging,
                      smfDatafileInfo ** aDatafileInfo,
                      smlEnv           * aEnv )
{
    stlUInt64         sReqSize;
    stlUInt64         sPageCount;
    stlUInt64         sTotalPageCount;
    stlUInt64         sWrittenPageCount = 0;
    smfDatafileInfo * sDatafileInfo;
    stlFile           sFile;
    stlChar         * sChunk;
    stlUInt32         sState = 0;
    stlUInt32         sFileState = 0;
    stlChar           sDatafileName[STL_MAX_FILE_PATH_LENGTH];
    stlChar           sSystemTbsDir[STL_MAX_FILE_PATH_LENGTH];
    stlInt32          sDatafileNameLen;
    stlBool           sFileExist;
    smlDatafileAttr   sDatafileAttr;
    stlBool           sTempTbs;
    stlInt32          sFlag;
    smfDatafileHeader sDatafileHeader;
    stlFileInfo       sFileInfo;
    stlOffset         sFileOffset;
    stlInt64          sDatabaseIo;
    knlRegionMark     sMemMark;
    stlUInt64         sBulkIoBytes;
    
    STL_TRY_THROW( aTbsInfo->mTbsPersData.mNewDatafileId < SML_DATAFILE_MAX_COUNT,
                   RAMP_ERR_EXCEED_DATAFILE_LIMIT );

    STL_TRY( smfmFillDatafileAttr( aTbsInfo,
                                   aDatafileAttr,
                                   &sDatafileAttr,
                                   aEnv )
             == STL_SUCCESS );
    
    STL_TRY( smgAllocShmMem4Mount( STL_SIZEOF( smfDatafileInfo ),
                                   (void**)&sDatafileInfo,
                                   aEnv ) == STL_SUCCESS );
    sState = 1;

    SMF_INIT_DATAFILE_INFO( sDatafileInfo );

    sTempTbs = SML_TBS_IS_TEMPORARY( aTbsInfo->mTbsPersData.mAttr );

    sDatabaseIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_DATABASE_FILE_IO,
                                                 KNL_ENV(aEnv) );

    /**
     * Persistent Tablespace만이 데이터파일을 갖는다
     */
    if( sTempTbs == STL_FALSE )
    {
        STL_TRY( knlGetPropertyValueByName( "SYSTEM_TABLESPACE_DIR",
                                            sSystemTbsDir,
                                            KNL_ENV( aEnv ) )
                 == STL_SUCCESS );

        STL_TRY( stlMergeAbsFilePath( sSystemTbsDir,
                                      sDatafileAttr.mName,
                                      sDatafileName,
                                      STL_MAX_FILE_PATH_LENGTH,
                                      &sDatafileNameLen,
                                      KNL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );
    
        stlStrncpy( sDatafileInfo->mDatafilePersData.mName,
                    sDatafileName,
                    STL_MAX_FILE_PATH_LENGTH );

        /* file manager에 file name을 add한 후 undo logging하기 전에
         * rollback하는 경우 add된 file name을 제거하기 위해 pending op를 등록한다. */
        STL_TRY( smgAddPendOp( NULL,
                               SMG_PEND_CREATE_DATAFILE,
                               SML_PEND_ACTION_ROLLBACK,
                               (void*)&(sDatafileInfo->mDatafilePersData),
                               STL_SIZEOF( smfDatafilePersData ),
                               0,
                               aEnv ) == STL_SUCCESS );

        /* database에서 이미 사용 중인지 체크하고 사용중이지 않으면 추가한다. */
        STL_TRY( knlAddDbFile( sDatafileInfo->mDatafilePersData.mName,
                               KNL_DATABASE_FILE_TYPE_DATAFILE,
                               KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
    }
    else
    {
        stlStrncpy( sDatafileInfo->mDatafilePersData.mName,
                    sDatafileAttr.mName,
                    STL_MAX_FILE_PATH_LENGTH );
    }

    STL_TRY( stlExistFile( sDatafileInfo->mDatafilePersData.mName,
                           &sFileExist,
                           KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    if( sDatafileAttr.mReuse == STL_TRUE )
    {
        if( sFileExist == STL_TRUE )
        {
            STL_TRY( stlGetFileStatByName( &sFileInfo,
                                           sDatafileInfo->mDatafilePersData.mName,
                                           STL_FINFO_SIZE,
                                           KNL_ERROR_STACK(aEnv) )
                     == STL_SUCCESS );
            
            if( (aDatafileAttr->mValidFlags & SML_DATAFILE_SIZE_MASK) !=
                SML_DATAFILE_SIZE_NO )
            {
                if( sDatafileAttr.mSize < sFileInfo.mSize )
                {
                    /**
                     * 데이터파일이 축소되어야 하는 경우
                     */
                    sFlag = STL_FOPEN_LARGEFILE | STL_FOPEN_WRITE | STL_FOPEN_READ;

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
                        
                    sFileOffset = sDatafileAttr.mSize;

                    STL_TRY( stlTruncateFile( &sFile,
                                              sFileOffset,
                                              KNL_ERROR_STACK(aEnv) )
                             == STL_SUCCESS );

                    sFileState = 0;
                    STL_TRY( smgCloseFile( &sFile,
                                           aEnv )
                             == STL_SUCCESS );
                }
            }
        }
    }
    else
    {
        STL_TRY_THROW( sFileExist == STL_FALSE, RAMP_ERR_AREADY_EXIST_FILE );
    }
    
    sDatafileInfo->mDatafilePersData.mID = aTbsInfo->mTbsPersData.mNewDatafileId;

    /**
     * File Header(1 page)를 제외한 공간
     */
    sTotalPageCount = (sDatafileAttr.mSize / SMP_PAGE_SIZE) - 1;
    
    sReqSize = sTotalPageCount * SMP_PAGE_SIZE;
    sReqSize += sTotalPageCount * STL_SIZEOF( smpCtrlHeader );

    sDatafileInfo->mShmState = SMF_DATAFILE_SHM_STATE_NULL;
    sDatafileInfo->mShmSize = sReqSize;
    sDatafileInfo->mDatafilePersData.mAutoExtend = sDatafileAttr.mAutoExtend;
    sDatafileInfo->mDatafilePersData.mSize = sTotalPageCount * SMP_PAGE_SIZE;
    sDatafileInfo->mDatafilePersData.mNextSize = 0; /**< Memory Tablespace에서는 의미없음 */
    sDatafileInfo->mDatafilePersData.mMaxSize = 0;  /**< Memory Tablespace에서는 의미없음 */
    sDatafileInfo->mDatafilePersData.mState = SMF_DATAFILE_STATE_INIT;
    sDatafileInfo->mDatafilePersData.mTimestamp = stlNow();
    sDatafileInfo->mDatafilePersData.mCheckpointLsn = SMR_INVALID_LSN;
    sDatafileInfo->mDatafilePersData.mCheckpointLid = SMR_INVALID_LID;
    sDatafileInfo->mDatafilePersData.mCreationLsn = smrGetLastCheckpointLsn();
    sDatafileInfo->mDatafilePersData.mCreationLid = smrGetLastCheckpointLid();
    sDatafileInfo->mMaxPageCount = sTotalPageCount;

    STL_TRY( smfChooseIoGroupId( sDatafileInfo->mDatafilePersData.mName,
                                 &sDatafileInfo->mIoGroupIdx,
                                 aEnv)
             == STL_SUCCESS );
    
    if( sTempTbs == STL_FALSE )
    {
        sFlag = ( STL_FOPEN_CREATE    |
                  STL_FOPEN_LARGEFILE |
                  STL_FOPEN_WRITE     |
                  STL_FOPEN_READ );

        if( sDatabaseIo == 0 )
        {
            sFlag |= STL_FOPEN_DIRECT;
        }

        if( sDatafileAttr.mReuse == STL_FALSE )
        {
            sFlag |= STL_FOPEN_EXCL;
        }

        sBulkIoBytes = knlGetPropertyBigIntValueByID( KNL_PROPERTY_BULK_IO_PAGE_COUNT,
                                                      KNL_ENV(aEnv) );
        sBulkIoBytes *= SMP_PAGE_SIZE;

        /**
         * 로그를 기록하기 전에 미리 파일을 생성한다.
         * - 로그 기록후 파일 생성 실패를 막기 위함이다.
         * - 만약 파일이 생성되고, 로그가 기록되기 전에 비정상 종료되는 경우는
         *   파일 leak이 발생할수 있다.
         */
        
        STL_TRY( smgOpenFile( &sFile,
                              sDatafileInfo->mDatafilePersData.mName,
                              sFlag,
                              STL_FPERM_OS_DEFAULT,
                              aEnv )
                 == STL_SUCCESS );
        sFileState = 2;

        /**
         * Redo/Undo Logging
         * - REUSE가 FALSE인 경우 물리적 파일에 대한 UNDO는 책임지지 않는다.
         * - Datafile 생성 이전에 logging
         */

        if( aUndoLogging == STL_TRUE )
        {
            if( sDatafileAttr.mReuse == STL_TRUE )
            {
                aUndoLogging = STL_FALSE;
            }
        }
        
        STL_TRY( smfmWriteLogCreate( aTransId,
                                     SMR_LOG_MEMORY_FILE_CREATE,
                                     SMF_UNDO_LOG_MEMORY_FILE_CREATE,
                                     aTbsPhysicalId,
                                     aHintTbsId,
                                     &sDatafileInfo->mDatafilePersData,
                                     aUndoLogging,
                                     aEnv ) == STL_SUCCESS );
        sFileState = 1;

        sDatafileHeader.mTbsPhysicalId = aTbsPhysicalId;
        sDatafileHeader.mDatafileId = sDatafileInfo->mDatafilePersData.mID;
        sDatafileHeader.mCheckpointLsn = SMR_INVALID_LSN;
        sDatafileHeader.mCheckpointLid = SMR_INVALID_LID;
        sDatafileHeader.mCreationTime = sDatafileInfo->mDatafilePersData.mTimestamp;
    
        /**
         * write FILE HEADER
         */

        STL_TRY( smfWriteDatafileHeader( &sFile,
                                         &sDatafileHeader,
                                         aEnv )
                 == STL_SUCCESS );

        /**
         * allocate buffer
         */
        
        KNL_INIT_REGION_MARK(&sMemMark);
        STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                                   &sMemMark,
                                   KNL_ENV(aEnv) ) == STL_SUCCESS );
        sState = 2;
        
        STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                    STL_MIN( sTotalPageCount * SMP_PAGE_SIZE,
                                             sBulkIoBytes ) +
                                    SMF_DIRECT_IO_DEFAULT_BLOCKSIZE,
                                    (void**)&sChunk,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sChunk = (stlChar*)STL_ALIGN( STL_INT_FROM_POINTER(sChunk),
                                      SMF_DIRECT_IO_DEFAULT_BLOCKSIZE );
        
        stlMemset( sChunk,
                   0x00,
                   STL_MIN( sTotalPageCount * SMP_PAGE_SIZE,
                            sBulkIoBytes ) );

        /**
         * REUSE 옵션에서 Extend되는 부분만 초기화 하는 작업 변경
         * Datafile 전체를 초기화 한다.
         */ 
        sFileOffset = SMP_PAGE_SIZE;
        STL_TRY( stlSeekFile( &sFile,
                              STL_FSEEK_SET,
                              &sFileOffset,
                              KNL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );

        while( sWrittenPageCount < sTotalPageCount )
        {
            sPageCount = STL_MIN( (sTotalPageCount - sWrittenPageCount),
                                  (sBulkIoBytes / SMP_PAGE_SIZE) );
            
            STL_TRY( stlWriteFile( &sFile,
                                   sChunk,
                                   sPageCount * SMP_PAGE_SIZE,
                                   NULL,
                                   KNL_ERROR_STACK( aEnv ) )
                     == STL_SUCCESS );

            sWrittenPageCount += sPageCount;

            STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );
        }
        
        sFileState = 0;
        STL_TRY( smgCloseFile( &sFile,
                               aEnv )
                 == STL_SUCCESS );

        sState = 1;
        STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                           &sMemMark,
                                           STL_TRUE, /* aFreeChunk */
                                           (knlEnv*)aEnv ) == STL_SUCCESS );
        
    }
    
    *aDatafileInfo = sDatafileInfo;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_EXCEED_DATAFILE_LIMIT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_EXCEED_DATAFILE_LIMIT,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }
    
    STL_CATCH( RAMP_ERR_AREADY_EXIST_FILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_ALREADY_EXIST_FILE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      sDatafileInfo->mDatafilePersData.mName );
    }
    
    STL_FINISH;

    switch( sFileState )
    {
        case 2:
            (void)smgCloseFile( &sFile, aEnv );
            (void)stlRemoveFile( sDatafileInfo->mDatafilePersData.mName,
                                 KNL_ERROR_STACK(aEnv) );
            break;
        case 1:
            (void)smgCloseFile( &sFile, aEnv );
        default:
            break;
    }
        
    switch( sState )
    {
        case 2:
            (void)knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                            &sMemMark,
                                            STL_TRUE, /* aFreeChunk */
                                            (knlEnv*)aEnv );
        case 1:
            (void)smgFreeShmMem4Mount( (void*)sDatafileInfo, aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}

stlStatus smfmPrepare( smfTbsInfo       * aTbsInfo,
                       stlFile          * aFile,
                       smfCtrlBuffer    * aBuffer,
                       smfDatafileInfo ** aDatafileInfo,
                       smlEnv           * aEnv )
{
    smfDatafileInfo * sDatafileInfo;
    stlUInt32         sState = 0;
    stlBool           sTempTbs;
    stlUInt16         sShmIndex;
    stlUInt64         sReqSize;
    stlUInt64         sPageCount;
    stlChar         * sChunk;
    
    STL_TRY( smgAllocShmMem4Mount( STL_SIZEOF( smfDatafileInfo ),
                                   (void**)&sDatafileInfo,
                                   aEnv ) == STL_SUCCESS );
    sState = 1;

    SMF_INIT_DATAFILE_INFO( sDatafileInfo );

    STL_TRY( smfReadCtrlFile( aFile,
                              aBuffer,
                              (stlChar *)&sDatafileInfo->mDatafilePersData,
                              STL_SIZEOF( smfDatafilePersData ),
                              aEnv ) == STL_SUCCESS );

    sTempTbs = SML_TBS_IS_MEMORY_TEMPORARY( aTbsInfo->mTbsPersData.mAttr );

    if( sTempTbs == STL_TRUE )
    {
        /**
         * Temporary Tablespace 는 Prepare 단계에서 초기화 한다.
         */
        
        sPageCount = sDatafileInfo->mDatafilePersData.mSize / SMP_PAGE_SIZE;
        sReqSize = sPageCount * SMP_PAGE_SIZE;
        sReqSize += sPageCount * STL_SIZEOF( smpCtrlHeader );

        STL_TRY( knlCreateDatabaseArea( NULL, /* aName */
                                        &sShmIndex,
                                        sReqSize,
                                        (knlEnv*)aEnv ) == STL_SUCCESS );
        sState = 2;

        sDatafileInfo->mShmState = SMF_DATAFILE_SHM_STATE_ALLOCATED;

        sDatafileInfo->mChunk    = KNL_MAKE_LOGICAL_ADDR( sShmIndex, 0 );
        sDatafileInfo->mPchArray = KNL_MAKE_LOGICAL_ADDR( sShmIndex,
                                                          sPageCount * SMP_PAGE_SIZE );
        sDatafileInfo->mMaxPageCount = sPageCount;
        sDatafileInfo->mShmIndex = sShmIndex;
        sDatafileInfo->mDatafilePersData.mTimestamp = stlNow();

        STL_TRY( knlGetPhysicalAddr( sDatafileInfo->mChunk,
                                     (void**)&sChunk,
                                     (knlEnv*)aEnv ) == STL_SUCCESS );
    }
    
    /**
     * AGING 상태라면 파일을 읽어서 공유 메모리에 적재할 필요가 없다.
     * - 향후 Refine 단계에서 Datafile 정보를 삭제한다.
     */
    if( aTbsInfo->mTbsPersData.mState == SMF_TBS_STATE_AGING )
    {
        sDatafileInfo->mChunk = 0;
        sDatafileInfo->mPchArray = 0;
        sDatafileInfo->mShmIndex = 0;
        sDatafileInfo->mMaxPageCount = 0;
    }
    
    *aDatafileInfo = sDatafileInfo;
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)knlDestroyDatabaseArea( sShmIndex, (knlEnv*)aEnv );
        case 1:
            (void)smgFreeShmMem4Mount( (void*)sDatafileInfo, aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}

stlStatus smfmLoad( smfTbsInfo      * aTbsInfo,
                    smfDatafileInfo * aDatafileInfo,
                    smlEnv          * aEnv )
{
    stlChar         * sChunk;
    stlFile           sFile;
    stlUInt32         sState = 0;
    stlUInt16         sShmIndex;
    stlUInt64         sReqSize;
    stlUInt64         sPageCount;
    stlOffset         sOffset;
    stlUInt64         sTotalPageCount;
    stlUInt64         sReadPageCount = 0;
    smfDatafileHeader sDatafileHeader;
    stlInt32          sFlag;
    stlInt64          sDatabaseIo;
    stlUInt64         sReadBytes;
    stlUInt64         sBulkIoPageCount;
    
    STL_ASSERT( SML_TBS_IS_TEMPORARY( aTbsInfo->mTbsPersData.mAttr ) == STL_FALSE );
    STL_ASSERT( aTbsInfo->mTbsPersData.mState != SMF_TBS_STATE_AGING );

    STL_TRY( knlBindNumaMasterNode( KNL_ENV(aEnv) ) == STL_SUCCESS );
    sState = 1;
    
    sPageCount = aDatafileInfo->mDatafilePersData.mSize / SMP_PAGE_SIZE;
    sReqSize = sPageCount * SMP_PAGE_SIZE;
    sReqSize += sPageCount * STL_SIZEOF( smpCtrlHeader );

    if( aDatafileInfo->mShmState < SMF_DATAFILE_SHM_STATE_ALLOCATED )
    {
        STL_TRY( knlCreateDatabaseArea( NULL, /* aName */
                                        &sShmIndex,
                                        sReqSize,
                                        (knlEnv*)aEnv ) == STL_SUCCESS );

        aDatafileInfo->mChunk    = KNL_MAKE_LOGICAL_ADDR( sShmIndex, 0 );
        aDatafileInfo->mPchArray = KNL_MAKE_LOGICAL_ADDR( sShmIndex,
                                                          sPageCount * SMP_PAGE_SIZE );
        aDatafileInfo->mMaxPageCount = sPageCount;
        aDatafileInfo->mShmIndex = sShmIndex;
        aDatafileInfo->mShmState = SMF_DATAFILE_SHM_STATE_ALLOCATED;
    }

    STL_TRY( knlGetPhysicalAddr( aDatafileInfo->mChunk,
                                 (void**)&sChunk,
                                 (knlEnv*)aEnv ) == STL_SUCCESS );

    /**
     * 이미 Load 된 경우는 올리지 않는다.
     */
    
    if( aDatafileInfo->mShmState < SMF_DATAFILE_SHM_STATE_LOADED )
    {
        STL_TRY( knlLogMsg( NULL,
                            KNL_ENV(aEnv),
                            KNL_LOG_LEVEL_INFO,
                            ".... LOAD DATAFILE(%s)\n",
                            aDatafileInfo->mDatafilePersData.mName )
                 == STL_SUCCESS );

        sFlag = STL_FOPEN_LARGEFILE | STL_FOPEN_READ;

        sDatabaseIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_DATABASE_FILE_IO,
                                                     KNL_ENV(aEnv) );

        if( sDatabaseIo == 0 )
        {
            sFlag |= STL_FOPEN_DIRECT;
        }

        STL_TRY( smgOpenFile( &sFile,
                              aDatafileInfo->mDatafilePersData.mName,
                              sFlag,
                              STL_FPERM_OS_DEFAULT,
                              aEnv )
                 == STL_SUCCESS );
        sState = 2;

        STL_TRY( knlGetPhysicalAddr( aDatafileInfo->mChunk,
                                     (void**)&sChunk,
                                     (knlEnv*)aEnv ) == STL_SUCCESS );

        STL_TRY( smfReadDatafileHeader( &sFile,
                                        aDatafileInfo->mDatafilePersData.mName,
                                        &sDatafileHeader,
                                        aEnv )
                 == STL_SUCCESS );

        aDatafileInfo->mDatafilePersData.mTimestamp = sDatafileHeader.mCreationTime;

        sOffset = SMP_PAGE_SIZE;
        STL_TRY( stlSeekFile( &sFile,
                              STL_FSEEK_SET,
                              &sOffset,
                              KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

        sBulkIoPageCount = knlGetPropertyBigIntValueByID( KNL_PROPERTY_BULK_IO_PAGE_COUNT,
                                                          KNL_ENV(aEnv) );

        /**
         * read datafile
         */
        sTotalPageCount = aDatafileInfo->mMaxPageCount;
    
        while( sReadPageCount < sTotalPageCount )
        {
            sPageCount = STL_MIN( (sTotalPageCount - sReadPageCount),
                                  sBulkIoPageCount );
            
            STL_TRY( stlReadFile( &sFile,
                                  sChunk + (sReadPageCount * SMP_PAGE_SIZE),
                                  sPageCount * SMP_PAGE_SIZE,
                                  &sReadBytes,
                                  KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

            STL_DASSERT( sReadBytes == (sPageCount * SMP_PAGE_SIZE) );

            sReadPageCount += sPageCount;

            STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );
        }
    
        sState = 1;
        STL_TRY( smgCloseFile( &sFile,
                               aEnv )
                 == STL_SUCCESS );
        
        aDatafileInfo->mShmState = SMF_DATAFILE_SHM_STATE_LOADED;
    }

    sState = 0;
    STL_TRY( knlUnbindNumaNode( STL_FALSE,  /* aIsCleanup */
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)smgCloseFile( &sFile, aEnv );
        case 1:
            (void)knlUnbindNumaNode( STL_FALSE, KNL_ENV(aEnv) );
        default:
            break;
    }
    
    return STL_FAILURE;
}

stlStatus smfmDrop( smxlTransId       aTransId,
                    smfTbsInfo      * aTbsInfo,
                    smfDatafileInfo * aDatafileInfo,
                    smlEnv          * aEnv )
{
    stlBool  sIsRemoved;

    /* database file manager에서 file name을 제거한다. */
    STL_TRY( knlRemoveDbFile( aDatafileInfo->mDatafilePersData.mName,
                              &sIsRemoved,
                              KNL_ENV(aEnv) ) == STL_SUCCESS );

    /**
     * 데이터베이스 공유 메모리만 삭제한다.
     * - 데이터파일 정보는 그대로 유지한다.
     * - 향후 Refine 단계에서 제거한다.
     */

    if( aDatafileInfo->mShmState >= SMF_DATAFILE_SHM_STATE_ALLOCATED )
    {
        STL_TRY( knlDestroyDatabaseArea( aDatafileInfo->mShmIndex,
                                         KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smfmExtend( smxmTrans        *aMiniTrans,
                      smlTbsId          aTbsId,
                      smlDatafileId     aDatafileId,
                      smlEnv           *aEnv )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  SML_ERRCODE_DATAFILE_EXTEND,
                  NULL,
                  KNL_ERROR_STACK( aEnv ),
                  aDatafileId, aTbsId, smxmGetTransId( aMiniTrans ) );
    
    return STL_FAILURE;
}

stlStatus smfmWritePage( smfTbsInfo      * aTbsInfo,
                         smfDatafileInfo * aDatafileInfo,
                         stlUInt32         aPageSeqId,
                         stlInt32          aPageCount,
                         void            * aPage,
                         smlEnv          * aEnv )
{
    stlFile     sFile;
    stlOffset   sOffset;
    stlInt32    sFlag = 0;
    stlInt32    sState = 0;
    stlInt64    sDatabaseIo;
    stlUInt64   sWrittenBytes;

    sFlag = (STL_FOPEN_WRITE | STL_FOPEN_READ | STL_FOPEN_LARGEFILE);
    
    sDatabaseIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_DATABASE_FILE_IO,
                                                 KNL_ENV(aEnv) );

    if( sDatabaseIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }
        
    STL_TRY( smgOpenFile( &sFile,
                          aDatafileInfo->mDatafilePersData.mName,
                          sFlag,
                          STL_FPERM_OS_DEFAULT,
                          aEnv )
             == STL_SUCCESS );
    sState = 1;

    sOffset = ((stlInt64)aPageSeqId + 1) * SMP_PAGE_SIZE;
    STL_TRY( stlSeekFile( &sFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
    
    STL_TRY( stlWriteFile( &sFile,
                           aPage,
                           ((stlInt64)aPageCount) * SMP_PAGE_SIZE,
                           &sWrittenBytes,
                           KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    STL_DASSERT( sWrittenBytes == (((stlInt64)aPageCount) * SMP_PAGE_SIZE) );

    sState = 0;
    STL_TRY( smgCloseFile( &sFile, aEnv ) == STL_SUCCESS );
                              
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smgCloseFile( &sFile, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smfmFillDatafileAttr( smfTbsInfo      * aTbsInfo,
                                smlDatafileAttr * aOrgDatafileAttr,
                                smlDatafileAttr * aRefinedDatafileAttr,
                                smlEnv          * aEnv )
{
    stlInt64  sTbsSize;
    
    /**
     * Persistent 테이블 스페이스라면 Datafile Name은 설정되어 있어야 한다.
     */
    if( SML_TBS_IS_TEMPORARY( aTbsInfo->mTbsPersData.mAttr ) == STL_TRUE )
    {
        STL_PARAM_VALIDATE( (aOrgDatafileAttr->mValidFlags & SML_DATAFILE_NAME_MASK) ==
                            SML_DATAFILE_NAME_NO, KNL_ERROR_STACK( aEnv ) );
    }

    *aRefinedDatafileAttr = *aOrgDatafileAttr;
    
    /**
     * AUTOEXTEND를 지정하지 않았다면 AUTOEXTEND OFF이다.
     */
    if( (aOrgDatafileAttr->mValidFlags & SML_DATAFILE_AUTOEXTEND_MASK) ==
        SML_DATAFILE_AUTOEXTEND_NO )
    {
        aRefinedDatafileAttr->mAutoExtend = STL_FALSE;
    }
    else
    {
        /**
         * Memory datafile은 무조건 AutoExtend Off이다
         */
        aRefinedDatafileAttr->mAutoExtend = STL_FALSE;
    }
    
    /**
     * SIZE를 지정하지 않았다면...
     */
    if( (aOrgDatafileAttr->mValidFlags & SML_DATAFILE_SIZE_MASK) ==
        SML_DATAFILE_SIZE_NO )
    {
        if( SML_TBS_IS_TEMPORARY( aTbsInfo->mTbsPersData.mAttr ) == STL_FALSE )
        {
            sTbsSize = knlGetPropertyBigIntValueByID( KNL_PROPERTY_USER_MEMORY_DATA_TABLESPACE_SIZE,
                                                      KNL_ENV( aEnv ) );
        }
        else
        {
            sTbsSize = knlGetPropertyBigIntValueByID( KNL_PROPERTY_USER_MEMORY_TEMP_TABLESPACE_SIZE,
                                                      KNL_ENV( aEnv ) );
        }
        
        aRefinedDatafileAttr->mSize = sTbsSize;
    }

    /**
     * REUSE를 지정하지 않았다면...
     */
    if( (aOrgDatafileAttr->mValidFlags & SML_DATAFILE_REUSE_MASK) ==
        SML_DATAFILE_REUSE_NO )
    {
        aRefinedDatafileAttr->mReuse = STL_FALSE;
    }
    
    /**
     * Memory Tablespace에서는 MAXSIZE와 NEXTSIZE는 무시한다.
     */
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
