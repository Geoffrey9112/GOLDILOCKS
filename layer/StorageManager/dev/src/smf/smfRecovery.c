/*******************************************************************************
 * smfRecovery.c
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
#include <smg.h>
#include <smp.h>
#include <smfDef.h>
#include <smxm.h>
#include <smxl.h>
#include <smxlDef.h>
#include <smr.h>
#include <smfTbsMgr.h>
#include <smfRecovery.h>

extern smfDatafileFuncs * gDatafileFuncs[];
/**
 * @file smfmDatafile.c
 * @brief Storage Manager Layer Memory Tablespace Recovery Component Internal Routines
 */

/**
 * @addtogroup smfRecovery
 * @{
 */

stlStatus smfWriteLogTbs( smxlTransId      aTransId,
                          stlUInt16        aRedoLogType,
                          stlUInt16        aUndoLogType,
                          smfTbsPersData * aBeforeTbsPersData,
                          smfTbsPersData * aAfterTbsPersData,
                          stlBool          aUndoLogging,
                          smlEnv         * aEnv )
{
    stlInt32    sState = 0;
    smxmTrans   sMiniTrans;
    smlRid      sUndoRid;
    smlRid      sTargetRid;
    stlInt32    sUndoLogSize = 0;

    /**
     * Redo Log 기록
     */

    sTargetRid = (smlRid){ aAfterTbsPersData->mTbsId, 0, 0 };
    
    STL_TRY( smxmBegin( &sMiniTrans,
                        aTransId,
                        sTargetRid,
                        SMXM_ATTR_REDO,
                        aEnv )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( smxmWriteTbsLog( &sMiniTrans,
                              SMG_COMPONENT_DATAFILE,
                              aRedoLogType,
                              SMR_REDO_TARGET_CTRL,
                              (void*)aAfterTbsPersData,
                              STL_SIZEOF( smfTbsPersData ),
                              aAfterTbsPersData->mTbsId,
                              0,   /* aPageId */
                              0,   /* aOffset */
                              aEnv )
             == STL_SUCCESS );
    
    /**
     * Undo Log 기록
     * - Database 생성시에는 Undo Logging을 할수 없다
     */

    if( aBeforeTbsPersData != NULL )
    {
        sUndoLogSize = STL_SIZEOF( smfTbsPersData );
    }

    if( aUndoLogging == STL_TRUE )
    {
        KNL_BREAKPOINT( KNL_BREAKPOINT_SMFWRITELOGTBS_BEFORE_UNDOLOGGING,
                        KNL_ENV(aEnv) );

        STL_TRY( smxlInsertUndoRecord( &sMiniTrans,
                                       SMG_COMPONENT_DATAFILE,
                                       aUndoLogType,
                                       (stlChar*)aBeforeTbsPersData,
                                       sUndoLogSize,
                                       sTargetRid,
                                       &sUndoRid,
                                       aEnv ) == STL_SUCCESS );
        
    }
        
    sState = 0;
    STL_TRY( smxmSyncCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

    KNL_BREAKPOINT( KNL_BREAKPOINT_SMFWRITELOGTBS_AFTER_UNDOLOGGING,
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

stlStatus smfWriteLogRenameTbs( smxlTransId   aTransId,
                                smlTbsId      aTbsId,
                                stlUInt16     aRedoLogType,
                                stlUInt16     aUndoLogType,
                                stlChar     * aBeforeName,
                                stlChar     * aAfterName,
                                stlBool       aUndoLogging,
                                smlEnv      * aEnv )
{
    stlInt32     sState = 0;
    smxmTrans    sMiniTrans;
    smlRid       sUndoRid;
    smlRid       sTargetRid;

    /**
     * Redo Log 기록
     */

    sTargetRid = (smlRid){ aTbsId, 0, 0 };
    
    STL_TRY( smxmBegin( &sMiniTrans,
                        aTransId,
                        sTargetRid,
                        SMXM_ATTR_REDO,
                        aEnv )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( smxmWriteTbsLog( &sMiniTrans,
                              SMG_COMPONENT_DATAFILE,
                              aRedoLogType,
                              SMR_REDO_TARGET_CTRL,
                              (void*)aAfterName,
                              stlStrlen( aAfterName ),
                              aTbsId,
                              0,   /* aPageId */
                              0,   /* aOffset */
                              aEnv )
             == STL_SUCCESS );
    
    /**
     * Undo Log 기록
     * - Database 생성시에는 Undo Logging을 할수 없다
     */

    if( aUndoLogging == STL_TRUE )
    {
        KNL_BREAKPOINT( KNL_BREAKPOINT_SMFWRITELOGRENAMETBS_BEFORE_UNDOLOGGING,
                        KNL_ENV(aEnv) );

        STL_TRY( smxlInsertUndoRecord( &sMiniTrans,
                                       SMG_COMPONENT_DATAFILE,
                                       aUndoLogType,
                                       (stlChar*)aBeforeName,
                                       stlStrlen(aBeforeName),
                                       sTargetRid,
                                       &sUndoRid,
                                       aEnv ) == STL_SUCCESS );
    }
        
    sState = 0;
    STL_TRY( smxmSyncCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

    KNL_BREAKPOINT( KNL_BREAKPOINT_SMFWRITELOGRENAMETBS_AFTER_UNDOLOGGING,
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


stlStatus smfWriteLogDatafile( smxlTransId           aTransId,
                               smlTbsId              aTbsId,
                               stlUInt16             aRedoLogType,
                               stlUInt16             aUndoLogType,
                               smfDatafilePersData * aBeforeDatafilePersData,
                               smfDatafilePersData * aAfterDatafilePersData,
                               stlBool               aUndoLogging,
                               smlEnv              * aEnv )
{
    stlInt32     sState = 0;
    smxmTrans    sMiniTrans;
    smlRid       sUndoRid;
    smlRid       sTargetRid;
    stlInt32     sUndoLogSize = 0;

    /**
     * Redo Log 기록
     */

    sTargetRid = (smlRid){ aTbsId, 0, 0 };
    
    STL_TRY( smxmBegin( &sMiniTrans,
                        aTransId,
                        sTargetRid,
                        SMXM_ATTR_REDO,
                        aEnv )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( smxmWriteTbsLog( &sMiniTrans,
                              SMG_COMPONENT_DATAFILE,
                              aRedoLogType,
                              SMR_REDO_TARGET_CTRL,
                              (void*)aAfterDatafilePersData,
                              STL_SIZEOF( smfDatafilePersData ),
                              aTbsId,
                              0,   /* aPageId */
                              0,   /* aOffset */
                              aEnv )
             == STL_SUCCESS );
    
    /**
     * Undo Log 기록
     * - Database 생성시에는 Undo Logging을 할수 없다
     */

    if( aBeforeDatafilePersData != NULL )
    {
        sUndoLogSize = STL_SIZEOF( smfDatafilePersData );
    }

    if( aUndoLogging == STL_TRUE )
    {
        KNL_BREAKPOINT( KNL_BREAKPOINT_SMFWRITELOGDATAFILE_BEFORE_UNDOLOGGING,
                        KNL_ENV(aEnv) );

        STL_TRY( smxlInsertUndoRecord( &sMiniTrans,
                                       SMG_COMPONENT_DATAFILE,
                                       aUndoLogType,
                                       (stlChar*)aBeforeDatafilePersData,
                                       sUndoLogSize,
                                       sTargetRid,
                                       &sUndoRid,
                                       aEnv ) == STL_SUCCESS );
    }
        
    sState = 0;
    STL_TRY( smxmSyncCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

    KNL_BREAKPOINT( KNL_BREAKPOINT_SMFWRITELOGDATAFILE_AFTER_UNDOLOGGING,
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


stlStatus smfCreateTbsRedo( smfTbsPersData * aTbsPersData,
                            smlEnv         * aEnv )
{
    smfTbsInfo * sTbsInfo;

    /**
     * 이미 Tablespace가 존재한다면 REDO를 skip한다
     */
    STL_TRY_THROW( gSmfWarmupEntry->mTbsInfoArray[aTbsPersData->mTbsId] == NULL,
                   RAMP_SKIP_REDO );

    /**
     *  TbsId가 NewTbsId보다 작다면 Drop 된 Tbs라 판단하고 skip한다.
     */
    if( aTbsPersData->mTbsId < gSmfWarmupEntry->mDbPersData.mNewTbsId )
    {
        STL_TRY( knlLogMsg( NULL,
                            KNL_ENV( aEnv ),
                            KNL_LOG_LEVEL_INFO,
                            ".... skip to redo create tablespace(%d, %s)\n",
                            aTbsPersData->mTbsId,
                            aTbsPersData->mName ) == STL_SUCCESS );
        
        STL_THROW( RAMP_SKIP_REDO );
    }

    STL_TRY( smgAllocShmMem4Mount( STL_SIZEOF( smfTbsInfo ),
                                   (void**)&sTbsInfo,
                                   aEnv ) == STL_SUCCESS );

    SMF_INIT_TBS_INFO( sTbsInfo );

    STL_TRY( knlInitLatch( &sTbsInfo->mLatch,
                           STL_TRUE,
                           (knlEnv*)aEnv ) == STL_SUCCESS );

    stlMemcpy( &sTbsInfo->mTbsPersData,
               aTbsPersData,
               STL_SIZEOF( smfTbsPersData ) );

    SMF_INIT_TBS_BACKUP_INFO( &(sTbsInfo->mBackupInfo), aEnv );

    gSmfWarmupEntry->mTbsInfoArray[aTbsPersData->mTbsId] = sTbsInfo;

    /**
     * Controlfile의 NewTbsId보다 CreateRedo하는 TbsId가 크거나 같으면
     * Controlfile의 NewTbsId를 CreateRedo하는 TbsId + 1로 설정한다.
     */
    if( aTbsPersData->mTbsId >= gSmfWarmupEntry->mDbPersData.mNewTbsId )
    {
        gSmfWarmupEntry->mDbPersData.mNewTbsId = aTbsPersData->mTbsId + 1;
    }

    gSmfWarmupEntry->mDbPersData.mTbsCount++;

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV(aEnv),
                        KNL_LOG_LEVEL_INFO,
                        ".... redo create tablespace(%d, %s)\n",
                        aTbsPersData->mTbsId,
                        aTbsPersData->mName )
             == STL_SUCCESS );
    
    STL_RAMP( RAMP_SKIP_REDO );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smfCreateTbsUndo( smxlTransId      aTransId,
                            stlInt32         aTbsTypeId,
                            smfTbsPersData * aTbsPersData,
                            stlBool          aClrLogging,
                            smlEnv         * aEnv )
{
    smfTbsInfo      * sTbsInfo;
    smfDatafileInfo * sDatafileInfo;
    stlInt32          sState = 0;
    stlInt32          i;

    /**
     * Tablespace가 존재하지 않는다면 UNDO를 skip한다
     */
    
    STL_TRY_THROW( gSmfWarmupEntry->mTbsInfoArray[aTbsPersData->mTbsId] != NULL,
                   RAMP_SKIP_UNDO );

    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsPersData->mTbsId];

    for( i = 0; i < SML_DATAFILE_MAX_COUNT; i++ )
    {
        sDatafileInfo = sTbsInfo->mDatafileInfoArray[i];
        
        if( sDatafileInfo == NULL )
        {
            continue;
        }

        STL_TRY( smfAddDatafileUndo( aTransId,
                                     aTbsTypeId,
                                     aTbsPersData->mTbsId,
                                     &sDatafileInfo->mDatafilePersData,
                                     STL_TRUE,  /* aClrLogging */
                                     aEnv )
                 == STL_SUCCESS );
    }
    
    /**
     * Compensation Log 기록
     * - CLR Redo시에는 로그를 기록하지 않는다.
     */

    if( aClrLogging == STL_TRUE )
    {
        STL_TRY( smfWriteLogTbs( aTransId,
                                 SMR_LOG_MEMORY_TBS_CREATE_CLR,
                                 SMF_LOG_INVALID,
                                 NULL,      /* aBeforeTbsPersData */
                                 aTbsPersData,
                                 STL_FALSE, /* aUndoLogging */
                                 aEnv )
                 == STL_SUCCESS );
    }

    STL_TRY( smfAcquireTablespace( sTbsInfo, aEnv ) == STL_SUCCESS );
    sState = 1;

    gSmfWarmupEntry->mTbsInfoArray[aTbsPersData->mTbsId] = NULL;

    sState = 0;
    STL_TRY( smfReleaseTablespace( sTbsInfo, aEnv ) == STL_SUCCESS );

    STL_TRY( smfSaveCtrlFile( aEnv ) == STL_SUCCESS );

    STL_TRY( smgFreeShmMem4Mount( (void*)sTbsInfo,
                                  aEnv ) == STL_SUCCESS );

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV(aEnv),
                        KNL_LOG_LEVEL_INFO,
                        ".... undo create tablespace(%d)\n",
                        aTbsPersData->mTbsId )
             == STL_SUCCESS );
    
    STL_RAMP( RAMP_SKIP_UNDO );
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smfReleaseTablespace( sTbsInfo, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smfDropTbsRedo( smfTbsPersData * aTbsPersData,
                          smlEnv         * aEnv )
{
    smfTbsInfo * sTbsInfo;

    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsPersData->mTbsId];
    
    STL_TRY_THROW( ( sTbsInfo != NULL ) &&
                   ( smfGetTbsState( aTbsPersData->mTbsId ) != SMF_TBS_STATE_AGING ) &&
                   ( smfGetTbsState( aTbsPersData->mTbsId ) != SMF_TBS_STATE_DROPPED ),
                   RAMP_SKIP_REDO );
    
    /**
     * Tablespace Info 메모리는 Refine 단계에서 삭제한다.
     */

    stlMemcpy( &sTbsInfo->mTbsPersData,
               aTbsPersData,
               STL_SIZEOF( smfTbsPersData ) );

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV(aEnv),
                        KNL_LOG_LEVEL_INFO,
                        ".... redo drop tablespace(%d)\n",
                        aTbsPersData->mTbsId )
             == STL_SUCCESS );
    
    STL_RAMP( RAMP_SKIP_REDO );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smfDropTbsUndo( smxlTransId      aTransId,
                          smfTbsPersData * aTbsPersData,
                          stlBool          aClrLogging,
                          smlEnv         * aEnv )
{
    smfTbsInfo          * sTbsInfo;
    smfDatafileInfo     * sDatafileInfo;
    smfDatafilePersData   sDatafilePersData;
    stlInt32              sState = 0;
    stlInt32              i;

    /**
     * Tablespace가 존재하지 않는다면 UNDO를 skip한다
     */
    STL_TRY_THROW( gSmfWarmupEntry->mTbsInfoArray[aTbsPersData->mTbsId] != NULL,
                   RAMP_SKIP_UNDO );

    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsPersData->mTbsId];

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
         * Drop Tablespace 연산 당시 Datafile 정보를 변경하지 않았기 때문에
         * 현재 Datafile 정보를 그대로 저장한다.
         */
        stlMemcpy( &sDatafilePersData,
                   &sDatafileInfo->mDatafilePersData,
                   STL_SIZEOF( smfDatafilePersData ) );
                   
        STL_TRY( smfDropDatafileUndo( aTransId,
                                      aTbsPersData->mTbsId,
                                      &sDatafilePersData,
                                      STL_TRUE, /* aClrLogging */
                                      aEnv )
                 == STL_SUCCESS );
    }
    
    /**
     * Compensation Log 기록
     * - CLR Redo시에는 로그를 기록하지 않는다.
     */

    if( aClrLogging == STL_TRUE )
    {
        STL_TRY( smfWriteLogTbs( aTransId,
                                 SMR_LOG_MEMORY_TBS_DROP_CLR,
                                 SMF_LOG_INVALID,
                                 NULL,      /* aBeforeTbsPersData */
                                 aTbsPersData,
                                 STL_FALSE, /* aUndoLogging */
                                 aEnv )
                 == STL_SUCCESS );
    }

    STL_TRY( smfAcquireTablespace( sTbsInfo, aEnv ) == STL_SUCCESS );
    sState = 1;

    /**
     * 이전 이미지로 재구성한다.
     */
    
    stlMemcpy( &sTbsInfo->mTbsPersData,
               aTbsPersData,
               STL_SIZEOF( smfTbsPersData ) );
    
    sState = 0;
    STL_TRY( smfReleaseTablespace( sTbsInfo, aEnv ) == STL_SUCCESS );

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV(aEnv),
                        KNL_LOG_LEVEL_INFO,
                        ".... undo drop tablespace(%d)\n",
                        aTbsPersData->mTbsId )
             == STL_SUCCESS );
    
    STL_RAMP( RAMP_SKIP_UNDO );
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smfReleaseTablespace( sTbsInfo, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smfRenameTbsRedo( smlTbsId     aTbsId,
                            stlChar    * aTbsName,
                            stlInt32     aNameLen,
                            smlEnv     * aEnv )
{
    smfTbsInfo * sTbsInfo;

    STL_TRY_THROW( smfIsRedoableDatafile( aTbsId, 0 ) == STL_TRUE,
                   RAMP_SKIP_REDO );
    
    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];
    
    stlMemcpy( sTbsInfo->mTbsPersData.mName,
               aTbsName,
               aNameLen );

    sTbsInfo->mTbsPersData.mName[aNameLen] = 0;

    STL_RAMP( RAMP_SKIP_REDO );
    
    return STL_SUCCESS;
}

stlStatus smfRenameTbsUndo( smxlTransId   aTransId,
                            smlTbsId      aTbsId,
                            stlChar     * aTbsName,
                            stlInt32      aNameLen,
                            stlBool       aClrLogging,
                            smlEnv      * aEnv )
{
    smfTbsInfo  * sTbsInfo;
    stlInt32      sState = 0;
    stlChar       sTbsName[STL_MAX_SQL_IDENTIFIER_LENGTH];

    /**
     * Tablespace가 존재하지 않는다면 UNDO를 skip한다
     */
    STL_TRY_THROW( gSmfWarmupEntry->mTbsInfoArray[aTbsId] != NULL,
                   RAMP_SKIP_UNDO );

    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];

    /**
     * Compensation Log 기록
     * - CLR Redo시에는 로그를 기록하지 않는다.
     */

    if( aClrLogging == STL_TRUE )
    {
        stlMemset( sTbsName, 0x00, STL_MAX_SQL_IDENTIFIER_LENGTH );
        stlMemcpy( sTbsName, aTbsName, aNameLen );
        sTbsName[aNameLen] = '\0';
        
        STL_TRY( smfWriteLogRenameTbs( aTransId,
                                       aTbsId,
                                       SMR_LOG_MEMORY_TBS_RENAME_CLR,
                                       SMF_LOG_INVALID,
                                       NULL,      /* aBeforeName */
                                       sTbsName,
                                       STL_FALSE, /* aUndoLogging */
                                       aEnv )
                 == STL_SUCCESS );
    }

    STL_TRY( smfAcquireTablespace( sTbsInfo, aEnv ) == STL_SUCCESS );
    sState = 1;

    /**
     * 이전 이미지로 재구성한다.
     */
    
    stlMemcpy( &sTbsInfo->mTbsPersData.mName,
               aTbsName,
               aNameLen );

    sTbsInfo->mTbsPersData.mName[aNameLen] = '\0';
    
    sState = 0;
    STL_TRY( smfReleaseTablespace( sTbsInfo, aEnv ) == STL_SUCCESS );

    STL_RAMP( RAMP_SKIP_UNDO );
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smfReleaseTablespace( sTbsInfo, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smfAddDatafileRedo( smlTbsId              aTbsId,
                              smfDatafilePersData * aDatafilePersData,
                              smlEnv              * aEnv )
{
    smfTbsInfo      * sTbsInfo;
    smfDatafileInfo * sDatafileInfo;
    smfDatafileHeader sDatafileHeader;
    stlUInt64         sReqSize;
    stlUInt64         sPageCount;
    stlUInt32         sTbsTypeId;
    stlInt32          sState = 0;
    stlUInt16         sShmIndex;
    stlFile           sFile;
    stlInt32          sFlag;
    stlInt64          sDatabaseIo;

    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];

    /**
     * Tablespace Info가 NULL이면 redo skip
     */
    STL_TRY_THROW( sTbsInfo != NULL, RAMP_SKIP_REDO );

    /**
     * Temp tablespace는 datafile 없음
     */
    STL_TRY_THROW( SML_TBS_IS_TEMPORARY( sTbsInfo->mTbsPersData.mAttr ) == STL_FALSE,
                   RAMP_SKIP_REDO );

    /**
     * ID가 NewDatafileId보다 작다면 Skip.
     */
    STL_TRY_THROW( aDatafilePersData->mID >= sTbsInfo->mTbsPersData.mNewDatafileId,
                   RAMP_SKIP_REDO );

    if( sTbsInfo->mDatafileInfoArray[aDatafilePersData->mID] == NULL )
    {
        STL_TRY( smgAllocShmMem4Mount( STL_SIZEOF( smfDatafileInfo ),
                                       (void**)&sDatafileInfo,
                                       aEnv ) == STL_SUCCESS );

        stlMemcpy( &sDatafileInfo->mDatafilePersData,
                   aDatafilePersData,
                   STL_SIZEOF( smfDatafileInfo ) );

        sPageCount = aDatafilePersData->mSize / SMP_PAGE_SIZE;
        sReqSize = aDatafilePersData->mSize + (sPageCount * STL_SIZEOF(smpCtrlHeader));

        STL_TRY( knlCreateDatabaseArea( NULL,  /* aName */
                                        &sShmIndex,
                                        sReqSize,
                                        (knlEnv*)aEnv ) == STL_SUCCESS );

        sDatafileInfo->mShmState = SMF_DATAFILE_SHM_STATE_ALLOCATED;
        sDatafileInfo->mMaxPageCount = sPageCount;
        sDatafileInfo->mShmIndex = sShmIndex;
        sDatafileInfo->mChunk    = KNL_MAKE_LOGICAL_ADDR( sShmIndex, 0 );
        sDatafileInfo->mPchArray = KNL_MAKE_LOGICAL_ADDR( sShmIndex,
                                                          sPageCount * SMP_PAGE_SIZE );
    
        sTbsInfo->mDatafileInfoArray[aDatafilePersData->mID] = sDatafileInfo;

        /**
         * 데이터베이스를 Load한다.
         */

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
        sState = 1;

        STL_TRY( smfReadDatafileHeader( &sFile,
                                        sDatafileInfo->mDatafilePersData.mName,
                                        &sDatafileHeader,
                                        SML_ENV(aEnv) )
                 == STL_SUCCESS );

        /**
         * 다른 Tablespace에서 사용중인 datafile의 경우 skip한다.
         * - Datafile header의 TbsPhysicalId와 aTbsId가 다르고 INVALID_TBS_ID가 아닌 경우
         */

        if( (sDatafileHeader.mTbsPhysicalId != aTbsId) &&
            (sDatafileHeader.mTbsPhysicalId != SMF_INVALID_PHYSICAL_TBS_ID) )
        {
            sState = 0;
            STL_TRY( smgCloseFile( &sFile,
                                   aEnv )
                     == STL_SUCCESS );

            STL_THROW( RAMP_SKIP_REDO );
        }

        sTbsInfo->mTbsPersData.mNewDatafileId++;
        
        sDatafileHeader.mTbsPhysicalId = aTbsId;
        sDatafileHeader.mDatafileId = aDatafilePersData->mID;
        sDatafileHeader.mCheckpointLsn = aDatafilePersData->mCheckpointLsn;
        sDatafileHeader.mCheckpointLid = aDatafilePersData->mCheckpointLid;
        sDatafileHeader.mCreationTime = aDatafilePersData->mTimestamp;

        /**
         * write FILE HEADER
         */

        STL_TRY( smfWriteDatafileHeader( &sFile,
                                         &sDatafileHeader,
                                         aEnv )
                 == STL_SUCCESS );

        sState = 0;
        STL_TRY( smgCloseFile( &sFile,
                               aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY_THROW( smfIsRedoableDatafile( aTbsId, aDatafilePersData->mID ) == STL_TRUE,
                       RAMP_SKIP_REDO );

        sDatafileInfo = sTbsInfo->mDatafileInfoArray[aDatafilePersData->mID];
    }

    /**
     * 데이터베이스를 Load한다.
     */

    sTbsTypeId = SML_TBS_DEVICE_TYPE_ID( sTbsInfo->mTbsPersData.mAttr );
    STL_TRY( gDatafileFuncs[sTbsTypeId]->mLoad( sTbsInfo,
                                                sDatafileInfo,
                                                aEnv ) == STL_SUCCESS );

    /**
     * Load Group을 설정할수 없기 때문에 default group에 설정한다.
     */
    sDatafileInfo->mIoGroupIdx = 0;

    /**
     * PCH List를 구성한다
     */
    
    STL_TRY( smpInit( aTbsId,
                      aDatafilePersData->mID,
                      aEnv )
             == STL_SUCCESS );
    
    STL_TRY( smpExtend( NULL,  /* aStatement */
                        aTbsId,
                        aDatafilePersData->mID,
                        STL_FALSE,   /* aNeedPending */
                        aEnv )
             == STL_SUCCESS );
    
    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV(aEnv),
                        KNL_LOG_LEVEL_INFO,
                        ".... redo add datafile(%s)\n",
                        aDatafilePersData->mName )
             == STL_SUCCESS );
    
    STL_RAMP( RAMP_SKIP_REDO );
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void) smgCloseFile( &sFile, aEnv );
    }

    return STL_FAILURE;
}


stlStatus smfAddDatafileUndo( smxlTransId           aTransId,
                              stlInt32              aTbsTypeId,
                              smlTbsId              aTbsId,
                              smfDatafilePersData * aDatafilePersData,
                              stlBool               aClrLogging,
                              smlEnv              * aEnv )
{
    smfTbsInfo  * sTbsInfo;
    stlInt32      sState = 0;
    stlBool       sIsRemoved;

    STL_TRY( knlRemoveDbFile( aDatafilePersData->mName,
                              &sIsRemoved,
                              KNL_ENV(aEnv) ) == STL_SUCCESS );

    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];

    /**
     * 테이블스페이스가 존재하지 않는다면 UNDO를 skip한다
     */

    STL_TRY_THROW( sTbsInfo != NULL, RAMP_SKIP_UNDO );

    /**
     * 데이터파일이 존재하지 않는다면 UNDO를 skip한다
     */

    STL_TRY_THROW( sTbsInfo->mDatafileInfoArray[aDatafilePersData->mID] != NULL,
                   RAMP_SKIP_UNDO );

    /**
     * Compensation Log 기록
     * - CLR Redo시에는 로그를 기록하지 않는다.
     */

    if( aClrLogging == STL_TRUE )
    {
        STL_TRY( smfWriteLogDatafile( aTransId,
                                      aTbsId,
                                      SMR_LOG_MEMORY_DATAFILE_ADD_CLR,
                                      SMF_LOG_INVALID,
                                      NULL,      /* aBeforeDatafilePersData */
                                      aDatafilePersData,
                                      STL_FALSE, /* aUndoLogging */
                                      aEnv )
                 == STL_SUCCESS );
    }
    
    /**
     * PCH List를 삭제한다.
     */
    
    STL_TRY( smpShrink( NULL,   /* aStatement */
                        aTbsId,
                        aDatafilePersData->mID,
                        SMP_PCHARRAY_STATE_DROPPED, /* aPchArrayState */
                        aEnv )
             == STL_SUCCESS );
    
    /**
     * 데이터베이스 영역의 공유메모리를 삭제한다
     */
    
    STL_TRY( gDatafileFuncs[aTbsTypeId]->mDrop( aTransId,
                                                sTbsInfo,
                                                sTbsInfo->mDatafileInfoArray[aDatafilePersData->mID],
                                                aEnv )
             == STL_SUCCESS );

    STL_TRY( smfAcquireTablespace( sTbsInfo, aEnv ) == STL_SUCCESS );
    sState = 1;

    sTbsInfo->mDatafileInfoArray[aDatafilePersData->mID] = NULL;

    sState = 0;
    STL_TRY( smfReleaseTablespace( sTbsInfo, aEnv ) == STL_SUCCESS );

    STL_TRY( smfSaveCtrlFile( aEnv ) == STL_SUCCESS );

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV(aEnv),
                        KNL_LOG_LEVEL_INFO,
                        ".... undo add datafile(%s)\n",
                        aDatafilePersData->mName )
             == STL_SUCCESS );
    
    STL_RAMP( RAMP_SKIP_UNDO );
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smfReleaseTablespace( sTbsInfo, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smfDropDatafileRedo( smlTbsId              aTbsId,
                               smfDatafilePersData * aDatafilePersData,
                               smlEnv              * aEnv )
{
    smfTbsInfo  * sTbsInfo;
    stlBool       sIsRemoved;

    STL_TRY( knlRemoveDbFile( aDatafilePersData->mName,
                              &sIsRemoved,
                              KNL_ENV(aEnv) ) == STL_SUCCESS );

    STL_TRY_THROW( smfIsRedoableDatafile( aTbsId, aDatafilePersData->mID ) == STL_TRUE,
                   RAMP_SKIP_REDO );
    
    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];

    /**
     * 데이터파일이 존재하지 않는다면 REDO를 skip한다
     */

    STL_TRY_THROW( sTbsInfo->mDatafileInfoArray[aDatafilePersData->mID] != NULL,
                   RAMP_SKIP_REDO );

    /**
     * PCH List와 데이터베이스 메모리는 Refine 단계에서 삭제한다.
     */

    stlMemcpy( sTbsInfo->mDatafileInfoArray[aDatafilePersData->mID],
               aDatafilePersData,
               STL_SIZEOF( smfDatafilePersData ) );

    /**
     * PCH List를 삭제한다.
     */
    
    STL_TRY( smpShrink( NULL,   /* aStatement */
                        aTbsId,
                        aDatafilePersData->mID,
                        SMP_PCHARRAY_STATE_DROPPED, /* aPchArrayState */
                        aEnv )
             == STL_SUCCESS );
    
    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV(aEnv),
                        KNL_LOG_LEVEL_INFO,
                        ".... redo drop datafile(%s)\n",
                        aDatafilePersData->mName )
             == STL_SUCCESS );
    
    STL_RAMP( RAMP_SKIP_REDO );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smfDropDatafileUndo( smxlTransId           aTransId,
                               smlTbsId              aTbsId,
                               smfDatafilePersData * aDatafilePersData,
                               stlBool               aClrLogging,
                               smlEnv              * aEnv )
{
    smfTbsInfo  * sTbsInfo;
    stlInt32      sState = 0;

    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];

    /**
     * 테이블스페이스가 존재하지 않는다면 UNDO를 skip한다
     */

    STL_TRY_THROW( sTbsInfo != NULL, RAMP_SKIP_UNDO );

    /**
     * 데이터파일이 존재하지 않는다면 UNDO를 skip한다
     */

    STL_TRY_THROW( sTbsInfo->mDatafileInfoArray[aDatafilePersData->mID] != NULL,
                   RAMP_SKIP_UNDO );

    /**
     * Compensation Log 기록
     * - CLR Redo시에는 로그를 기록하지 않는다.
     */

    if( aClrLogging == STL_TRUE )
    {
        STL_TRY( smfWriteLogDatafile( aTransId,
                                      aTbsId,
                                      SMR_LOG_MEMORY_DATAFILE_DROP_CLR,
                                      SMF_LOG_INVALID,
                                      NULL,      /* aBeforeDatafilePersData */
                                      aDatafilePersData,
                                      STL_FALSE, /* aUndoLogging */
                                      aEnv )
                 == STL_SUCCESS );
    }
    
    STL_TRY( smfAcquireTablespace( sTbsInfo, aEnv ) == STL_SUCCESS );
    sState = 1;

    stlMemcpy( sTbsInfo->mDatafileInfoArray[aDatafilePersData->mID],
               aDatafilePersData,
               STL_SIZEOF( smfDatafilePersData ) );

    sState = 0;
    STL_TRY( smfReleaseTablespace( sTbsInfo, aEnv ) == STL_SUCCESS );

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV(aEnv),
                        KNL_LOG_LEVEL_INFO,
                        ".... undo drop datafile(%s)\n",
                        aDatafilePersData->mName )
             == STL_SUCCESS );
    
    STL_RAMP( RAMP_SKIP_UNDO );
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smfReleaseTablespace( sTbsInfo, aEnv );
    }

    return STL_FAILURE;
}

/** @} */
