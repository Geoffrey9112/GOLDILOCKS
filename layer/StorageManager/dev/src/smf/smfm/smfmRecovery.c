/*******************************************************************************
 * smfmRecovery.c
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
#include <smf.h>
#include <smfDef.h>
#include <smxm.h>
#include <smxl.h>
#include <smxlDef.h>
#include <smr.h>
#include <smfTbsMgr.h>

/**
 * @file smfmDatafile.c
 * @brief Storage Manager Layer Memory Datafile Recovery Component Internal Routines
 */

/**
 * @addtogroup smfmRecovery
 * @{
 */

stlStatus smfmWriteLogCreate( smxlTransId           aTransId,
                              stlUInt16             aRedoLogType,
                              stlUInt16             aUndoLogType,
                              stlInt32              aTbsPhysicalId,
                              stlInt32              aHintTbsId,
                              smfDatafilePersData * aDatafilePersData,
                              stlBool               aUndoLogging,
                              smlEnv              * aEnv )
{
    stlInt32       sState = 0;
    smxmTrans      sMiniTrans;
    smlRid         sUndoRid;
    stlChar        sLogBody[STL_SIZEOF(stlInt32) + STL_SIZEOF(smfDatafilePersData)];
    stlInt32       sLogOffset = 0;
    
    /**
     * Redo Log 기록
     */

    STL_TRY( smxmBegin( &sMiniTrans,
                        aTransId,
                        SML_INVALID_RID,
                        SMXM_ATTR_REDO,
                        aEnv )
             == STL_SUCCESS );
    sState = 1;

    STL_WRITE_MOVE_INT32( (stlChar*)sLogBody, &aTbsPhysicalId, sLogOffset );
    STL_WRITE_MOVE_INT32( (stlChar*)sLogBody, &aHintTbsId, sLogOffset );
    STL_WRITE_MOVE_BYTES( (stlChar*)sLogBody,
                          aDatafilePersData,
                          STL_SIZEOF( smfDatafilePersData ),
                          sLogOffset );
    
    STL_TRY( smxmWriteTbsLog( &sMiniTrans,
                              SMG_COMPONENT_DATAFILE,
                              aRedoLogType,
                              SMR_REDO_TARGET_CTRL,
                              (void*)sLogBody,
                              sLogOffset,
                              0,   /* aTbsId */
                              0,   /* aPageId */
                              0,   /* aOffset */
                              aEnv )
             == STL_SUCCESS );
    
    /**
     * Undo Log 기록
     * Database 생성시에는 Undo Logging을 할수 없다
     */

    if( aUndoLogging == STL_TRUE )
    {
        KNL_BREAKPOINT( KNL_BREAKPOINT_SMFMWRITELOGCREATE_BEFORE_UNDOLOGGING,
                        KNL_ENV(aEnv) );

        STL_TRY( smxlInsertUndoRecord( &sMiniTrans,
                                       SMG_COMPONENT_DATAFILE,
                                       aUndoLogType,
                                       (stlChar*)sLogBody,
                                       sLogOffset,
                                       SML_INVALID_RID,
                                       &sUndoRid,
                                       aEnv ) == STL_SUCCESS );
        
    }

    /**
     * 디스크의 데이터 파일 생성과 동시에 서버가 비정상 종료되는 경우를
     * 대비해 Undo Log를 디스크로 기록해야 한다.
     * 디스크의 데이터 파일 생성에 대한 WAL을 지키기 위한 방법이다.
     */
    
    sState = 0;
    STL_TRY( smxmSyncCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

    KNL_BREAKPOINT( KNL_BREAKPOINT_SMFMWRITELOGCREATE_AFTER_UNDOLOGGING,
                    KNL_ENV(aEnv) );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void)smxmRollback( &sMiniTrans, aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}

stlStatus smfmCreateRedo( smxlTransId           aTransId,
                          stlInt32              aTbsPhysicalId,
                          stlInt32              aHintTbsId,
                          smfDatafilePersData * aDatafilePersData,
                          stlBool               aForRestore,
                          stlBool               aClrLogging,
                          smlEnv              * aEnv )
{
    stlFile           sFile;
    smfDatafileHeader sDatafileHeader;
    stlUInt64         sFormattedSize = 0;
    stlUInt64         sSize;
    stlInt32          sFlag;
    stlUInt32         sState = 0;
    stlOffset         sFileOffset;
    stlBool           sFileExist;
    knlRegionMark     sMemMark;
    stlChar         * sBlock;
    stlChar         * sAlignedBlock;
    stlInt64          sDatabaseIo;
    smfDatafileInfo * sDatafileInfo;
    stlBool           sIsRemoved;
    smfTbsInfo      * sTbsInfo = NULL;
    stlBool           sFound = STL_FALSE;
    stlBool           sTbsExist = STL_FALSE;
    smlTbsId          sTbsId;
    smlDatafileId     sDatafileId;
    
    sFlag = ( STL_FOPEN_LARGEFILE |
              STL_FOPEN_WRITE     |
              STL_FOPEN_READ );

    sDatabaseIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_DATABASE_FILE_IO,
                                                 KNL_ENV(aEnv) );

    if( sDatabaseIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }

    STL_TRY( stlExistFile( aDatafilePersData->mName,
                           &sFileExist,
                           KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    /**
     * Restore에 의한 create redo는 dictionary에 영향을 주지 않는다.
     */
    if( aForRestore != STL_TRUE )
    {

        /**
         * TbsInfo를 찾고, Offline Tbs라면 Skip 한다.
         */
        STL_TRY( smfFirstTbs( &sTbsId,
                              &sTbsExist,
                              aEnv ) == STL_SUCCESS );

        while( sTbsExist == STL_TRUE )
        {
            STL_TRY( smfFindDatafileId( sTbsId,
                                        aDatafilePersData->mName,
                                        &sDatafileId,
                                        &sFound,
                                        aEnv ) == STL_SUCCESS );

            if( sFound == STL_TRUE )
            {
                break;
            }
        
            STL_TRY( smfNextTbs( &sTbsId,
                                 &sTbsExist,
                                 aEnv ) == STL_SUCCESS );
        }

        if( sFound == STL_TRUE )
        {
            sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[sTbsId];

            STL_TRY_THROW( sTbsInfo->mTbsPersData.mIsOnline == STL_TRUE,
                           RAMP_SKIP_REDO );
        }

        if( sFileExist == STL_TRUE )
        {
            STL_TRY( smgOpenFile( &sFile,
                                  aDatafilePersData->mName,
                                  sFlag,
                                  STL_FPERM_OS_DEFAULT,
                                  aEnv )
                     == STL_SUCCESS );
            sState = 1;

            STL_TRY( smfReadDatafileHeader( &sFile,
                                            aDatafilePersData->mName,
                                            &sDatafileHeader,
                                            SML_ENV(aEnv) )
                     == STL_SUCCESS );

            /* Tablespace WarmupEntry에 동일한 tablespace, datafile 정보가
             * 존재하면 redo를 skip한다. */
            if( smfGetTbsHandle(sDatafileHeader.mTbsPhysicalId) != NULL )
            {
                if( (smfGetTbsState( sDatafileHeader.mTbsPhysicalId ) != SMF_TBS_STATE_DROPPING) &&
                    (smfGetTbsState( sDatafileHeader.mTbsPhysicalId ) != SMF_TBS_STATE_DROPPED)  &&
                    (smfGetTbsState( sDatafileHeader.mTbsPhysicalId ) != SMF_TBS_STATE_AGING) )
                {
                    sDatafileInfo = smfGetDatafileInfo( sDatafileHeader.mTbsPhysicalId,
                                                        sDatafileHeader.mDatafileId );

                    if( sDatafileInfo != NULL )
                    {
                        if( (smfGetDatafileState( sDatafileHeader.mTbsPhysicalId,
                                                  sDatafileHeader.mDatafileId ) != SMF_TBS_STATE_DROPPING) &&
                            (smfGetDatafileState( sDatafileHeader.mTbsPhysicalId,
                                                  sDatafileHeader.mDatafileId ) != SMF_TBS_STATE_DROPPED)  &&
                            (smfGetDatafileState( sDatafileHeader.mTbsPhysicalId,
                                                  sDatafileHeader.mDatafileId ) != SMF_TBS_STATE_AGING) )
                        {
                            sState = 0;
                            STL_TRY( smgCloseFile( &sFile,
                                                   aEnv )
                                     == STL_SUCCESS );

                            STL_THROW( RAMP_SKIP_REDO );
                        }
                    }
                }
            }
   
            sFileOffset = aDatafilePersData->mSize;
            STL_TRY( stlTruncateFile( &sFile,
                                      sFileOffset,
                                      KNL_ERROR_STACK(aEnv) )
                     == STL_SUCCESS );

            sState = 0;
            STL_TRY( smgCloseFile( &sFile,
                                   aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /**
             * aTbsPhysicalId가
             *  INVALID 라면 CREATE TBS Log로 HintId를 사용하여 Skip 결정.
             *  INVALID 아니면 ADD DATAFILE에 대한 Log로 Datafile의 Id로 Skip 결정.
             */
            if( aTbsPhysicalId == SMF_INVALID_PHYSICAL_TBS_ID )
            { 
                if( aHintTbsId < gSmfWarmupEntry->mDbPersData.mNewTbsId )
                {
                    STL_TRY( knlLogMsg( NULL,
                                        KNL_ENV( aEnv ),
                                        KNL_LOG_LEVEL_INFO,
                                        ".... skip redo create datafile(%d, %s), "
                                        "tablespace(%d)\n",
                                        aDatafilePersData->mID,
                                        aDatafilePersData->mName,
                                        aTbsPhysicalId ) == STL_SUCCESS );

                    STL_THROW( RAMP_SKIP_REDO );
                }
            }
            else
            {
                sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsPhysicalId];
                if( aDatafilePersData->mID < sTbsInfo->mTbsPersData.mNewDatafileId )
                {
                    STL_TRY( knlLogMsg( NULL,
                                        KNL_ENV( aEnv ),
                                        KNL_LOG_LEVEL_INFO,
                                        ".... skip redo create datafile(%d, %s), "
                                        "tablespace(%d)\n",
                                        aDatafilePersData->mID,
                                        aDatafilePersData->mName,
                                        aTbsPhysicalId ) == STL_SUCCESS );

                    STL_THROW( RAMP_SKIP_REDO );
                }
            }
        }
    }
    else
    {
        STL_TRY_THROW( sFileExist == STL_FALSE, RAMP_SKIP_REDO );
    }

    sFlag |= STL_FOPEN_CREATE;

    /* 만약 database에서 사용중이라면 지우고 새로 추가한다. */
    STL_TRY( knlRemoveDbFile( aDatafilePersData->mName,
                              &sIsRemoved,
                              KNL_ENV(aEnv) ) == STL_SUCCESS );

    STL_TRY( knlAddDbFile( aDatafilePersData->mName,
                           KNL_DATABASE_FILE_TYPE_DATAFILE,
                           KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    /**
     * 데이터 파일 생성
     */
    STL_TRY( smgOpenFile( &sFile,
                          aDatafilePersData->mName,
                          sFlag,
                          STL_FPERM_OS_DEFAULT,
                          aEnv )
             == STL_SUCCESS );
    sState = 1;

    sDatafileHeader.mTbsPhysicalId = aTbsPhysicalId;
    sDatafileHeader.mDatafileId = aDatafilePersData->mID;
    sDatafileHeader.mCheckpointLsn = aDatafilePersData->mCreationLsn;
    sDatafileHeader.mCheckpointLid = aDatafilePersData->mCreationLid;
    sDatafileHeader.mCreationTime = aDatafilePersData->mTimestamp;

    /**
     * write FILE HEADER
     */

    STL_TRY( smfWriteDatafileHeader( &sFile,
                                     &sDatafileHeader,
                                     aEnv )
             == STL_SUCCESS );

    /**
     * Format Datafile
     */

    KNL_INIT_REGION_MARK( &sMemMark );

    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               (knlEnv*)aEnv )
             == STL_SUCCESS );

    /**
     * recovery시에는 BULK_IO_PAGE_COUNT property를 사용하지 않는다.
     * 사용자에 의해서 큰 메모리 할당을 유도할수 있으며 recovery가
     * 실패할수 있다.
     */
    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                STL_BULK_IO_BYTES + SMP_PAGE_SIZE,
                                (void**)&sBlock,
                                KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    sState = 2;

    sAlignedBlock = (stlChar*)STL_ALIGN( (stlInt64)sBlock,
                                         SMP_PAGE_SIZE );
    stlMemset( sAlignedBlock, 0x00, STL_BULK_IO_BYTES );

    /**
     * Datafile 전체를 초기화 한다.
     */ 
    sFileOffset = SMP_PAGE_SIZE;
    STL_TRY( stlSeekFile( &sFile,
                          STL_FSEEK_SET,
                          &sFileOffset,
                          KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    while( sFormattedSize < aDatafilePersData->mSize )
    {
        sSize = STL_MIN( (aDatafilePersData->mSize - sFormattedSize),
                         STL_BULK_IO_BYTES );

        STL_TRY( stlWriteFile( &sFile,
                               sAlignedBlock,
                               sSize,
                               NULL,
                               KNL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );

        sFormattedSize += sSize;
    }

    sState = 1;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_TRUE, /* aFreeChunk */
                                       (knlEnv*)aEnv ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( smgCloseFile( &sFile,
                           aEnv )
             == STL_SUCCESS );

    STL_RAMP( RAMP_SKIP_REDO );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2 :
            (void)knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                            &sMemMark,
                                            STL_TRUE, /* aFreeChunk */
                                            (knlEnv*)aEnv );
        case 1 :
            (void)smgCloseFile( &sFile, aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus smfmCreateUndo( smxlTransId           aTransId,
                          stlInt32              aTbsPhysicalId,
                          stlInt32              aHintTbsId,
                          smfDatafilePersData * aDatafilePersData,
                          stlBool               aForRestore,
                          stlBool               aClrLogging,
                          smlEnv              * aEnv )
{
    stlBool            sFileExist = STL_FALSE;
    stlBool            sIsRemoved;
    smfDatafileHeader  sDatafileHeader;
    stlFile            sFile;
    stlInt32           sState = 0;
    stlInt32           sFlag = 0;
    stlInt64           sDatabaseIo;

    /* database file manager에서 file name을 제거한다. */
    STL_TRY( knlRemoveDbFile( aDatafilePersData->mName,
                              &sIsRemoved,
                              KNL_ENV(aEnv) ) == STL_SUCCESS );

    /**
     * 파일 존재 여부를 검사한다.
     */
    STL_TRY( stlExistFile( aDatafilePersData->mName,
                           &sFileExist,
                           KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    STL_TRY_THROW( sFileExist == STL_TRUE, RAMP_SKIP_UNDO );

    /**
     * Compensation Log 기록
     * - CLR Redo시에는 로그를 기록하지 않는다.
     */
    if( aClrLogging == STL_TRUE )
    {
        STL_TRY( smfmWriteLogCreate( aTransId,
                                     SMR_LOG_MEMORY_FILE_CREATE_CLR,
                                     SMF_LOG_INVALID,
                                     aTbsPhysicalId,
                                     aHintTbsId,
                                     aDatafilePersData,
                                     STL_FALSE, /* aUndoLogging */
                                     aEnv ) == STL_SUCCESS );
    }

    /**
     * 해당 파일이 지워야하는 파일아이디와 일치하는지 검사한다.
     */
    sFlag = STL_FOPEN_WRITE | STL_FOPEN_READ | STL_FOPEN_LARGEFILE;
        
    sDatabaseIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_DATABASE_FILE_IO,
                                                 KNL_ENV(aEnv) );

    if( sDatabaseIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }

    STL_TRY( smgOpenFile( &sFile,
                          aDatafilePersData->mName,
                          sFlag,
                          STL_FPERM_OS_DEFAULT,
                          aEnv )
             == STL_SUCCESS );
    sState = 1;
        
    STL_TRY( smfReadDatafileHeader( &sFile,
                                    aDatafilePersData->mName,
                                    &sDatafileHeader,
                                    aEnv )
             == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( smgCloseFile( &sFile,
                           aEnv )
             == STL_SUCCESS );

    /**
     * 아래 2가지 경우에 파일을 물리적으로 삭제할수 있다.
     * - Tablespace 생성도중 실패한 경우
     * - Physical Tablespace Identifier가 같고 Datafile Identifier가 같은 경우
     */
    if( (sDatafileHeader.mTbsPhysicalId == SMF_INVALID_PHYSICAL_TBS_ID) ||
        (aTbsPhysicalId == SMF_INVALID_PHYSICAL_TBS_ID) )
    {
        if( stlRemoveFile( aDatafilePersData->mName,
                           KNL_ERROR_STACK( aEnv ) )
            == STL_FAILURE )
        {
            stlPopError( KNL_ERROR_STACK( aEnv ) );
        }
    }
    else
    {
        if( (sDatafileHeader.mTbsPhysicalId == aTbsPhysicalId) &&
            (sDatafileHeader.mDatafileId == aDatafilePersData->mID) )
        {
            if( stlRemoveFile( aDatafilePersData->mName,
                               KNL_ERROR_STACK( aEnv ) )
                == STL_FAILURE )
            {
                stlPopError( KNL_ERROR_STACK( aEnv ) );
            }
        }
    }

    STL_RAMP( RAMP_SKIP_UNDO );
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smgCloseFile( &sFile, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smfmDropRedo( smxlTransId           aTransId,
                        stlInt32              aTbsPhysicalId,
                        stlInt32              aHintTbsId,
                        smfDatafilePersData * aDatafilePersData,
                        stlBool               aForRestore,
                        stlBool               aClrLogging,
                        smlEnv              * aEnv )
{
    STL_TRY( smfmCreateUndo( aTransId,
                             aTbsPhysicalId,
                             aHintTbsId,
                             aDatafilePersData,
                             aForRestore,
                             aClrLogging,
                             aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smfmDropUndo( smxlTransId           aTransId,
                        stlInt32              aTbsPhysicalId,
                        stlInt32              aHintTbsId,
                        smfDatafilePersData * aDatafilePersData,
                        stlBool               aForRestore,
                        stlBool               aClrLogging,
                        smlEnv              * aEnv )
{
    /**
     * nothing to do
     */
    
    return STL_SUCCESS;
}

/** @} */
