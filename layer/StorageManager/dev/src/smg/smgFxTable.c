/*******************************************************************************
 * smgFxTable.c
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
#include <smr.h>
#include <smf.h>
#include <smxl.h>
#include <smkl.h>
#include <smp.h>
#include <smd.h>
#include <smgDef.h>
#include <smgFxTable.h>

extern smgWarmupEntry  * gSmgWarmupEntry;

/**
 * @file smgFxTable.c
 * @brief Storage Manager Layer General Fixed Table Internal Routines
 */

/**
 * @addtogroup smgFxTable
 * @{
 */

knlFxProcInfo gSmgProcEnvRows[SMG_PROC_ENV_ROW_COUNT] =
{
    {
        "TRANS_STACK_TOP",
        0,
        KNL_STAT_CATEGORY_NONE,
        0
    },
    {
        "OPERATION_MEMORY_TOTAL_SIZE",
        0,
        KNL_STAT_CATEGORY_TOTAL_MEMORY,
        0
    },
    {
        "OPERATION_MEMORY_INIT_SIZE",
        0,
        KNL_STAT_CATEGORY_INIT_MEMORY,
        0
    },
    {
        "LOG_MEMORY_TOTAL_SIZE",
        0,
        KNL_STAT_CATEGORY_TOTAL_MEMORY,
        0
    },
    {
        "LOG_MEMORY_INIT_SIZE",
        0,
        KNL_STAT_CATEGORY_INIT_MEMORY,
        0
    }
};

stlStatus smgOpenEnvCallback( void   * aStmt,
                              void   * aDumpObjHandle,
                              void   * aPathCtrl,
                              knlEnv * aEnv )
{
    smgFxProcEnvPathCtrl * sPathCtrl;
    knlEnv               * sEnv;
    stlInt64             * sValues;

    sPathCtrl  = (smgFxProcEnvPathCtrl*)aPathCtrl;

    sPathCtrl->mCurEnv = NULL;
    sPathCtrl->mIterator = 0;
    sValues = sPathCtrl->mValues;
    
    sEnv = knlGetFirstEnv();

    while( sEnv != NULL )
    {
        if( knlIsUsedEnv( sEnv ) == STL_TRUE )
        {
            STL_TRY( knlGetEnvId( sEnv,
                                  &sPathCtrl->mEnvId )
                     == STL_SUCCESS );
            
            sPathCtrl->mCurEnv = sEnv;

            sValues[0] = ((smlEnv*)sEnv)->mMiniTransStackTop;
            sValues[1] = ((smlEnv*)sEnv)->mOperationHeapMem.mTotalSize;
            sValues[2] = ((smlEnv*)sEnv)->mOperationHeapMem.mInitSize;
            sValues[3] = ((smlEnv*)sEnv)->mLogHeapMem.mTotalSize;
            sValues[4] = ((smlEnv*)sEnv)->mLogHeapMem.mInitSize;
            
            break;
        }
        
        sEnv = knlGetNextEnv( sEnv );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smgCloseEnvCallback( void   * aDumpObjHandle,
                               void   * aPathCtrl,
                               knlEnv * aEnv )
{
    return STL_SUCCESS;
}

stlStatus smgBuildRecordEnvCallback( void              * aDumpObjHandle,
                                     void              * aPathCtrl,
                                     knlValueBlockList * aValueList,
                                     stlInt32            aBlockIdx,
                                     stlBool           * aTupleExist,
                                     knlEnv            * aEnv )
{
    smgFxProcEnvPathCtrl * sPathCtrl;
    knlFxProcInfo          sFxProcInfo;
    knlFxProcInfo        * sRowDesc;
    stlInt64             * sValues;
    knlEnv               * sEnv;

    *aTupleExist = STL_FALSE;
    
    sPathCtrl = (smgFxProcEnvPathCtrl*)aPathCtrl;
    sValues = sPathCtrl->mValues;

    if( sPathCtrl->mIterator >= SMG_PROC_ENV_ROW_COUNT )
    {
        sEnv = knlGetNextEnv( sPathCtrl->mCurEnv );

        while( sEnv != NULL )
        {
            if( knlIsUsedEnv( sEnv ) == STL_TRUE )
            {
                STL_TRY( knlGetEnvId( sEnv,
                                      &sPathCtrl->mEnvId )
                         == STL_SUCCESS );
            
                sValues[0] = ((smlEnv*)sEnv)->mMiniTransStackTop;
                sValues[1] = ((smlEnv*)sEnv)->mOperationHeapMem.mTotalSize;
                sValues[2] = ((smlEnv*)sEnv)->mOperationHeapMem.mInitSize;
                sValues[3] = ((smlEnv*)sEnv)->mLogHeapMem.mTotalSize;
                sValues[4] = ((smlEnv*)sEnv)->mLogHeapMem.mInitSize;
                
                break;
            }
        
            sEnv = knlGetNextEnv( sEnv );
        }

        sPathCtrl->mCurEnv = sEnv;
        sPathCtrl->mIterator = 0;
    }
    
    STL_TRY_THROW( sPathCtrl->mCurEnv != NULL, RAMP_SUCCESS );

    sRowDesc = &gSmgProcEnvRows[sPathCtrl->mIterator];

    sFxProcInfo.mName     = sRowDesc->mName;
    sFxProcInfo.mCategory = sRowDesc->mCategory;
    sFxProcInfo.mEnvId    = sPathCtrl->mEnvId;
    sFxProcInfo.mValue    = sValues[sPathCtrl->mIterator];

    STL_TRY( knlBuildFxRecord( gKnlProcInfoColumnDesc,
                               &sFxProcInfo,
                               aValueList,
                               aBlockIdx,
                               aEnv )
             == STL_SUCCESS );

    *aTupleExist = STL_TRUE;
    sPathCtrl->mIterator += 1;

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

knlFxTableDesc gSmgEnvTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    NULL,
    smgOpenEnvCallback,
    smgCloseEnvCallback,
    smgBuildRecordEnvCallback,
    STL_SIZEOF( smgFxProcEnvPathCtrl ),
    "X$SM_PROC_ENV",
    "process environment information of storage manager layer",
    gKnlProcInfoColumnDesc
};

/**
 * X$SM_SYSTEM_INFO
 */
knlFxSystemInfo gSmgSystemInfoRows[SMG_SYSTEM_INFO_ROW_COUNT] =
{
    {
        "SYSTEM_LSN",
        0,
        KNL_STAT_CATEGORY_NONE,
        "system lsn"
    },
    {
        "RECOVERED_LSN",
        0,
        KNL_STAT_CATEGORY_NONE,
        "last recovered lsn"
    },
    {
        "RECOVERED_LPSN",
        0,
        KNL_STAT_CATEGORY_NONE,
        "last recovered lpsn"
    },
    {
        "RECOVERED_TRANS_ID",
        0,
        KNL_STAT_CATEGORY_NONE,
        "last recovered transaction identifier"
    },
    {
        "RECOVERED_SEGMENT_ID",
        0,
        KNL_STAT_CATEGORY_NONE,
        "last recovered segment identifier"
    },
    {
        "CHECKPOINT_LSN",
        0,
        KNL_STAT_CATEGORY_NONE,
        "checkpoint lsn"
    },
    {
        "CHECKPOINT_STATE",
        0,
        KNL_STAT_CATEGORY_NONE,
        "checkpoint state( 0:idle, 1:progress )"
    },
    {
        "SYSTEM_SCN",
        0,
        KNL_STAT_CATEGORY_NONE,
        "system scn"
    },
    {
        "MIN_TRANS_VIEW_SCN",
        0,
        KNL_STAT_CATEGORY_NONE,
        "minimum transaction view scn"
    },
    {
        "AGABLE_SCN",
        0,
        KNL_STAT_CATEGORY_NONE,
        "agable scn"
    },
    {
        "AGABLE_STMT_SCN",
        0,
        KNL_STAT_CATEGORY_NONE,
        "agable statement scn"
    },
    {
        "AGABLE_VIEW_SCN",
        0,
        KNL_STAT_CATEGORY_NONE,
        "agable view scn"
    },
    {
        "TRY_STEAL_UNDO_PAGE_COUNT",
        0,
        KNL_STAT_CATEGORY_NONE,
        "try steal undp page count"
    },
    {
        "DATA_STORE_MODE",
        0,
        KNL_STAT_CATEGORY_NONE,
        "data store mode"
    },
    {
        "TRANSACTION_TABLE_SIZE",
        0,
        KNL_STAT_CATEGORY_NONE,
        "transaction table size"
    },
    {
        "UNDO_RELATION_COUNT",
        0,
        KNL_STAT_CATEGORY_NONE,
        "undo relation count"
    },
    {
        "AGER_ENV_ID",
        0,
        KNL_STAT_CATEGORY_NONE,
        "ager environment identifier"
    },
    {
        "CLEANUP_ENV_ID",
        0,
        KNL_STAT_CATEGORY_NONE,
        "cleanup environment identifier"
    },
    {
        "PAGE_FLUSHER_ENV_ID",
        0,
        KNL_STAT_CATEGORY_NONE,
        "page flusher environment identifier"
    },
    {
        "CHECKPOINTER_ENV_ID",
        0,
        KNL_STAT_CATEGORY_NONE,
        "checkpointer environment identifier"
    },
    {
        "ARCHIVE_LOGGER_ENV_ID",
        0,
        KNL_STAT_CATEGORY_NONE,
        "archive logger environment identifier"
    },
    {
        "STABLE_LSN",
        0,
        KNL_STAT_CATEGORY_NONE,
        "stable lsn"
    },
    {
        "PAGE_FLUSHING_TIME",
        0,
        KNL_STAT_CATEGORY_NONE,
        "The time(ms) it takes to flush all dirty pages"
    },
    {
        "TOTAL_FLUSHED_PAGE_COUNT",
        0,
        KNL_STAT_CATEGORY_NONE,
        "total flushed page count"
    },
    {
        "VERSION_CONFLICT_COUNT",
        0,
        KNL_STAT_CATEGORY_NONE,
        "version conflict count"
    },
    {
        "DATA_ACCESS_MODE",
        0,
        KNL_STAT_CATEGORY_NONE,
        "data access mode( 0: read only, 1: read write )"
    },
    {
        "FLUSHED_TBS_ID",
        0,
        KNL_STAT_CATEGORY_NONE,
        "last flushed tablespace"
    },
    {
        "FLUSHED_PAGE_ID",
        0,
        KNL_STAT_CATEGORY_NONE,
        "last flushed page"
    },
    {
        "FLUSHED_PAGE_COUNT",
        0,
        KNL_STAT_CATEGORY_NONE,
        "flushed page count"
    },
    {
        "PAGE_IO_COUNT",
        0,
        KNL_STAT_CATEGORY_NONE,
        "io count"
    },
    {
        "LOG_FLUSHING_STATE",
        0,
        KNL_STAT_CATEGORY_NONE,
        "log flushing state( 0 : enabled, 1 >= : disabled )"
    },
    {
        "OPEN_PHASE_MEMORY_TOTAL_SIZE",
        0,
        KNL_STAT_CATEGORY_TOTAL_MEMORY,
        "used memory size at open phase"
    },
    {
        "MOUNT_PHASE_MEMORY_TOTAL_SIZE",
        0,
        KNL_STAT_CATEGORY_TOTAL_MEMORY,
        "used memory size at mount phase"
    },
    {
        "LOG_MIRROR_SYNC_STATE",
        0,
        KNL_STAT_CATEGORY_NONE,
        "logmirror sync state( 0 : sync, 1 : blocked )"
    },
    {
        "LOCK_TABLE_SIZE",
        0,
        KNL_STAT_CATEGORY_NONE,
        "lock table size"
    },
    {
        "LOCK_WAIT_TABLE_SIZE",
        0,
        KNL_STAT_CATEGORY_NONE,
        "lock wait table size"
    },
    {
        "LOCK_HASH_TABLE_SIZE",
        0,
        KNL_STAT_CATEGORY_NONE,
        "lock hash table size"
    }
};

stlStatus smgOpenSystemInfoFxTableCallback( void   * aStmt,
                                            void   * aDumpObjHandle,
                                            void   * aPathCtrl,
                                            knlEnv * aKnlEnv )
{
    smgFxSystemInfoPathCtrl * sPathCtrl;
    smrRecoveryInfo           sRecoveryInfo;
    smrChkptInfo              sChkptInfo;
    smxlSystemInfo            sSmxlSystemInfo;
    smpSystemInfo             sSmpSystemInfo;
    smdSystemInfo             sSmdSystemInfo;
    smfSystemInfo             sSmfSystemInfo;
    smklSystemInfo            sSmklSystemInfo;
    stlInt64                * sValues;
    stlInt32                  sRowIdx = 0;

    sPathCtrl  = (smgFxSystemInfoPathCtrl*)aPathCtrl;
    sPathCtrl->mIterator = 0;

    sValues = sPathCtrl->mValues;

    /**
     * System Lsn 설정
     */
    sValues[sRowIdx++] = smrGetSystemLsn();
    
    /**
     * Recovery Info 설정
     */
    smrGetRecoveryInfo( &sRecoveryInfo );
    
    sValues[sRowIdx++] = sRecoveryInfo.mLogLsn;
    sValues[sRowIdx++] = sRecoveryInfo.mLogLpsn;
    sValues[sRowIdx++] = sRecoveryInfo.mLogTransId;
    SMS_MAKE_SEGMENT_ID( &sValues[sRowIdx++],
                         &sRecoveryInfo.mLogSegRid );

    /**
     * Checkpoint Info 설정
     */
    smrGetChkptInfo( &sChkptInfo );
    
    sValues[sRowIdx++] = sChkptInfo.mLastChkptLsn;
    sValues[sRowIdx++] = sChkptInfo.mChkptState;

    /**
     * Scn Info 설정
     */
    smxlGetSystemInfo( &sSmxlSystemInfo );
    
    sValues[sRowIdx++] = sSmxlSystemInfo.mSystemScn;
    sValues[sRowIdx++] = sSmxlSystemInfo.mMinTransViewScn;
    sValues[sRowIdx++] = sSmxlSystemInfo.mAgableScn;
    sValues[sRowIdx++] = sSmxlSystemInfo.mAgableStmtScn;
    sValues[sRowIdx++] = sSmxlSystemInfo.mAgableViewScn;
    sValues[sRowIdx++] = sSmxlSystemInfo.mTryStealUndoCount;
    sValues[sRowIdx++] = sSmxlSystemInfo.mDataStoreMode;
    sValues[sRowIdx++] = sSmxlSystemInfo.mTransTableSize;
    sValues[sRowIdx++] = sSmxlSystemInfo.mUndoRelCount;

    /**
     * System Thread Environment Identifier 설정
     */
    sValues[sRowIdx++] = gSmgWarmupEntry->mAgerEnvId;
    sValues[sRowIdx++] = gSmgWarmupEntry->mCleanupEnvId;
    sValues[sRowIdx++] = gSmgWarmupEntry->mPageFlusherEnvId;
    sValues[sRowIdx++] = gSmgWarmupEntry->mCheckpointEnvId;
    sValues[sRowIdx++] = gSmgWarmupEntry->mArchivelogEnvId;

    /**
     * Buffer Info 설정
     */
    smpGetSystemInfo( &sSmpSystemInfo );
    
    sValues[sRowIdx++] = sSmpSystemInfo.mMinFlushedLsn;
    sValues[sRowIdx++] = sSmpSystemInfo.mPageFlushingTime;
    sValues[sRowIdx++] = sSmpSystemInfo.mFlushedPageCount;

    smdGetSystemInfo( &sSmdSystemInfo );
    
    sValues[sRowIdx++] = sSmdSystemInfo.mVersionConflictCount;

    smfGetSystemInfo( &sSmfSystemInfo );

    sValues[sRowIdx++] = sSmfSystemInfo.mDataAccessMode;
    sValues[sRowIdx++] = sSmfSystemInfo.mLastFlushedTbsId;
    sValues[sRowIdx++] = sSmfSystemInfo.mLastFlushedPid;
    sValues[sRowIdx++] = sSmfSystemInfo.mFlushedPageCount;
    sValues[sRowIdx++] = sSmfSystemInfo.mPageIoCount;
    sValues[sRowIdx++] = smrGetLogFlushingState();

    /**
     * meory info 설정
     */
    sValues[sRowIdx++] = gSmgWarmupEntry->mDatabaseMem4Open.mTotalSize;
    sValues[sRowIdx++] = gSmgWarmupEntry->mDatabaseMem4Mount.mTotalSize;
    
    /**
     * Logmirror stat 설정
     */
    sValues[sRowIdx++] = sRecoveryInfo.mLogMirrorStat;

    /**
     * Lock Manager 설정
     */
    
    smklGetSystemInfo( &sSmklSystemInfo );
    
    sValues[sRowIdx++] = sSmklSystemInfo.mLockTableSize;
    sValues[sRowIdx++] = sSmklSystemInfo.mLockWaitTableSize;
    sValues[sRowIdx++] = sSmklSystemInfo.mLockHashTableSize;

    STL_DASSERT( sRowIdx == SMG_SYSTEM_INFO_ROW_COUNT );

    return STL_SUCCESS;
}

stlStatus smgCloseSystemInfoFxTableCallback( void   * aDumpObjHandle,
                                             void   * aPathCtrl,
                                             knlEnv * aKnlEnv )
{
    return STL_SUCCESS;
}

stlStatus smgBuildRecordSystemInfoFxTableCallback( void              * aDumpObjHandle,
                                                   void              * aPathCtrl,
                                                   knlValueBlockList * aValueList,
                                                   stlInt32            aBlockIdx,
                                                   stlBool           * aTupleExist,
                                                   knlEnv            * aKnlEnv )
{
    smgFxSystemInfoPathCtrl * sPathCtrl;
    knlFxSystemInfo           sFxSystemInfo;
    knlFxSystemInfo         * sRowDesc;
    stlInt64                * sValues;

    *aTupleExist = STL_FALSE;
    
    sPathCtrl = (smgFxSystemInfoPathCtrl*)aPathCtrl;
    sValues = sPathCtrl->mValues;

    sRowDesc = &gSmgSystemInfoRows[sPathCtrl->mIterator];

    if( sPathCtrl->mIterator < SMG_SYSTEM_INFO_ROW_COUNT )
    {
        sFxSystemInfo.mName     = sRowDesc->mName;
        sFxSystemInfo.mValue    = sValues[sPathCtrl->mIterator];
        sFxSystemInfo.mComment  = sRowDesc->mComment;
        sFxSystemInfo.mCategory = sRowDesc->mCategory;

        STL_TRY( knlBuildFxRecord( gKnlSystemInfoColumnDesc,
                                   &sFxSystemInfo,
                                   aValueList,
                                   aBlockIdx,
                                   aKnlEnv )
                 == STL_SUCCESS );

        *aTupleExist = STL_TRUE;
        sPathCtrl->mIterator += 1;
    }

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


knlFxTableDesc gSmgSystemInfoTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    NULL,
    smgOpenSystemInfoFxTableCallback,
    smgCloseSystemInfoFxTableCallback,
    smgBuildRecordSystemInfoFxTableCallback,
    STL_SIZEOF( smgFxSystemInfoPathCtrl ),
    "X$SM_SYSTEM_INFO",
    "system information of storage manager layer",
    gKnlSystemInfoColumnDesc
};

/**
 * X$SM_SESS_ENV
 */
knlFxSessionInfo gSmgSessionEnvRows[SMG_SESSION_ENV_ROW_COUNT] =
{
    {
        "MIN_SNAPSHOT_STMT_VIEW_SCN",
        0,
        KNL_STAT_CATEGORY_NONE,
        0
    },
    {
        "MIN_STMT_VIEW_SCN",
        0,
        KNL_STAT_CATEGORY_NONE,
        0
    },
    {
        "MIN_SNAPSHOT_STMT_BEGIN_TIME",
        0,
        KNL_STAT_CATEGORY_NONE,
        0
    },
    {
        "TRANS_ID",
        0,
        KNL_STAT_CATEGORY_NONE,
        0
    },
    {
        "TRANS_VIEW_SCN",
        0,
        KNL_STAT_CATEGORY_NONE,
        0
    },
    {
        "DISABLED_VIEW_SCN",
        0,
        KNL_STAT_CATEGORY_NONE,
        0
    },
    {
        "MIN_TRANS_VIEW_SCN",
        0,
        KNL_STAT_CATEGORY_NONE,
        0
    }
};

stlStatus smgOpenSessionEnvFxTableCallback( void   * aStmt,
                                            void   * aDumpObjHandle,
                                            void   * aPathCtrl,
                                            knlEnv * aKnlEnv )
{
    smgFxSessionEnvPathCtrl * sPathCtrl;
    knlSessionEnv           * sSessEnv;
    stlInt64                * sValues;

    sPathCtrl  = (smgFxSessionEnvPathCtrl*)aPathCtrl;

    sPathCtrl->mCurEnv = NULL;
    sPathCtrl->mIterator = 0;
    sValues = sPathCtrl->mValues;
    
    sSessEnv = knlGetFirstSessionEnv();

    while( sSessEnv != NULL )
    {
        if( knlIsUsedSessionEnv( sSessEnv ) == STL_TRUE )
        {
            STL_TRY( knlGetSessionEnvId( sSessEnv,
                                         &sPathCtrl->mSessId,
                                         aKnlEnv )
                     == STL_SUCCESS );
            
            sPathCtrl->mCurEnv = sSessEnv;

            sValues[0] = ((smlSessionEnv*)sSessEnv)->mMinSnapshotStmtViewScn;
            sValues[1] = ((smlSessionEnv*)sSessEnv)->mMinStmtViewScn;
            sValues[2] = ((smlSessionEnv*)sSessEnv)->mMinSnapshotStmtBeginTime;
            sValues[3] = ((smlSessionEnv*)sSessEnv)->mTransId;
            sValues[4] = ((smlSessionEnv*)sSessEnv)->mTransViewScn;
            sValues[5] = ((smlSessionEnv*)sSessEnv)->mDisabledViewScn;
            sValues[6] = ((smlSessionEnv*)sSessEnv)->mMinTransViewScn;
            
            break;
        }
        
        sSessEnv = knlGetNextSessionEnv( sSessEnv );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smgCloseSessionEnvFxTableCallback( void   * aDumpObjHandle,
                                             void   * aPathCtrl,
                                             knlEnv * aKnlEnv )
{
    return STL_SUCCESS;
}

stlStatus smgBuildRecordSessionEnvFxTableCallback( void              * aDumpObjHandle,
                                                   void              * aPathCtrl,
                                                   knlValueBlockList * aValueList,
                                                   stlInt32            aBlockIdx,
                                                   stlBool           * aTupleExist,
                                                   knlEnv            * aKnlEnv )
{
    smgFxSessionEnvPathCtrl * sPathCtrl;
    knlFxSessionInfo          sFxSessionInfo;
    knlFxSessionInfo        * sRowDesc;
    stlInt64                * sValues;
    knlSessionEnv           * sSessEnv;

    *aTupleExist = STL_FALSE;
    
    sPathCtrl = (smgFxSessionEnvPathCtrl*)aPathCtrl;
    sValues = sPathCtrl->mValues;

    if( sPathCtrl->mIterator >= SMG_SESSION_ENV_ROW_COUNT )
    {
        sSessEnv = knlGetNextSessionEnv( sPathCtrl->mCurEnv );

        while( sSessEnv != NULL )
        {
            if( knlIsUsedSessionEnv( sSessEnv ) == STL_TRUE )
            {
                STL_TRY( knlGetSessionEnvId( sSessEnv,
                                             &sPathCtrl->mSessId,
                                             aKnlEnv )
                         == STL_SUCCESS );
            
                sValues[0] = ((smlSessionEnv*)sSessEnv)->mMinSnapshotStmtViewScn;
                sValues[1] = ((smlSessionEnv*)sSessEnv)->mMinStmtViewScn;
                sValues[2] = ((smlSessionEnv*)sSessEnv)->mMinSnapshotStmtBeginTime;
                sValues[3] = ((smlSessionEnv*)sSessEnv)->mTransId;
                sValues[4] = ((smlSessionEnv*)sSessEnv)->mTransViewScn;
                sValues[5] = ((smlSessionEnv*)sSessEnv)->mDisabledViewScn;
                sValues[6] = ((smlSessionEnv*)sSessEnv)->mMinTransViewScn;
                
                break;
            }
        
            sSessEnv = knlGetNextSessionEnv( sSessEnv );
        }

        sPathCtrl->mCurEnv = sSessEnv;
        sPathCtrl->mIterator = 0;
    }
    
    STL_TRY_THROW( sPathCtrl->mCurEnv != NULL, RAMP_SUCCESS );

    sRowDesc = &gSmgSessionEnvRows[sPathCtrl->mIterator];

    sFxSessionInfo.mName     = sRowDesc->mName;
    sFxSessionInfo.mCategory = sRowDesc->mCategory;
    sFxSessionInfo.mSessId   = sPathCtrl->mSessId;
    sFxSessionInfo.mValue    = sValues[sPathCtrl->mIterator];

    STL_TRY( knlBuildFxRecord( gKnlSessionInfoColumnDesc,
                               &sFxSessionInfo,
                               aValueList,
                               aBlockIdx,
                               aKnlEnv )
             == STL_SUCCESS );

    *aTupleExist = STL_TRUE;
    sPathCtrl->mIterator += 1;

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

knlFxTableDesc gSmgSessionEnvTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    NULL,
    smgOpenSessionEnvFxTableCallback,
    smgCloseSessionEnvFxTableCallback,
    smgBuildRecordSessionEnvFxTableCallback,
    STL_SIZEOF( smgFxSessionEnvPathCtrl ),
    "X$SM_SESS_ENV",
    "session information of storage manager layer",
    gKnlSessionInfoColumnDesc
};

/**
 * X$SM_SESS_STAT
 */
knlFxSessionInfo gSmgSessionStatRows[SMG_SESSION_STAT_ROW_COUNT] =
{
    {
        "SEGMENT_RETRY_COUNT",
        0,
        KNL_STAT_CATEGORY_NONE,
        0
    },
    {
        "RELATION_RETRY_COUNT",
        0,
        KNL_STAT_CATEGORY_NONE,
        0
    },
    {
        "FIX_COUNT",
        0,
        KNL_STAT_CATEGORY_NONE,
        0
    },
    {
        "VERSION_CONFLICT_COUNT",
        0,
        KNL_STAT_CATEGORY_NONE,
        0
    },
    {
        "SESSION_MEMORY_TOTAL_SIZE",
        0,
        KNL_STAT_CATEGORY_TOTAL_MEMORY,
        0
    },
    {
        "SESSION_MEMORY_INIT_SIZE",
        0,
        KNL_STAT_CATEGORY_INIT_MEMORY,
        0
    },
    {
        "STATEMENT_MEMORY_TOTAL_SIZE",
        0,
        KNL_STAT_CATEGORY_TOTAL_MEMORY,
        0
    },
    {
        "STATEMENT_MEMORY_INIT_SIZE",
        0,
        KNL_STAT_CATEGORY_INIT_MEMORY,
        0
    },
    {
        "SESSION_INSTANT_MEMORY_TOTAL_SIZE",
        0,
        KNL_STAT_CATEGORY_TOTAL_MEMORY,
        0
    },
    {
        "SESSION_INSTANT_MEMORY_INIT_SIZE",
        0,
        KNL_STAT_CATEGORY_INIT_MEMORY,
        0
    }
};

stlStatus smgOpenSessionStatFxTableCallback( void   * aStmt,
                                             void   * aDumpObjHandle,
                                             void   * aPathCtrl,
                                             knlEnv * aKnlEnv )
{
    smgFxSessionStatPathCtrl * sPathCtrl;
    knlSessionEnv            * sSessEnv;
    stlInt64                 * sValues;
    stlInt32                   sRowIdx = 0;

    sPathCtrl  = (smgFxSessionStatPathCtrl*)aPathCtrl;

    sPathCtrl->mCurEnv = NULL;
    sPathCtrl->mIterator = 0;
    sValues = sPathCtrl->mValues;
    
    sSessEnv = knlGetFirstSessionEnv();

    while( sSessEnv != NULL )
    {
        if( knlIsUsedSessionEnv( sSessEnv ) == STL_TRUE )
        {
            sRowIdx = 0;
                
            STL_TRY( knlGetSessionEnvId( sSessEnv,
                                         &sPathCtrl->mSessId,
                                         aKnlEnv )
                     == STL_SUCCESS );
            
            sPathCtrl->mCurEnv = sSessEnv;

            sValues[sRowIdx++] = ((smlSessionEnv*)sSessEnv)->mSegmentRetryCount;
            sValues[sRowIdx++] = ((smlSessionEnv*)sSessEnv)->mRelationRetryCount;
#ifdef STL_DEBUG
            sValues[sRowIdx++] = ((smlSessionEnv*)sSessEnv)->mFixCount;
#else
            sValues[sRowIdx++] = 0;
#endif
            sValues[sRowIdx++] = ((smlSessionEnv*)sSessEnv)->mVersionConflictCount;
            sValues[sRowIdx++] = ((smlSessionEnv*)sSessEnv)->mSessionMem.mTotalSize;
            sValues[sRowIdx++] = ((smlSessionEnv*)sSessEnv)->mSessionMem.mInitSize;
            sValues[sRowIdx++] = ((smlSessionEnv*)sSessEnv)->mStatementMem.mTotalSize;
            sValues[sRowIdx++] = ((smlSessionEnv*)sSessEnv)->mStatementMem.mInitSize;
            sValues[sRowIdx++] = ((smlSessionEnv*)sSessEnv)->mDynamicMem.mTotalSize;
            sValues[sRowIdx++] = ((smlSessionEnv*)sSessEnv)->mDynamicMem.mInitSize;
            
            break;
        }
        
        sSessEnv = knlGetNextSessionEnv( sSessEnv );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smgCloseSessionStatFxTableCallback( void   * aDumpObjHandle,
                                              void   * aPathCtrl,
                                              knlEnv * aKnlEnv )
{
    return STL_SUCCESS;
}

stlStatus smgBuildRecordSessionStatFxTableCallback( void              * aDumpObjHandle,
                                                    void              * aPathCtrl,
                                                    knlValueBlockList * aValueList,
                                                    stlInt32            aBlockIdx,
                                                    stlBool           * aTupleExist,
                                                    knlEnv            * aKnlEnv )
{
    smgFxSessionStatPathCtrl * sPathCtrl;
    knlFxSessionInfo           sFxSessionInfo;
    knlFxSessionInfo         * sRowDesc;
    stlInt64                 * sValues;
    knlSessionEnv            * sSessEnv;
    stlInt32                   sRowIdx = 0;

    *aTupleExist = STL_FALSE;
    
    sPathCtrl = (smgFxSessionStatPathCtrl*)aPathCtrl;
    sValues = sPathCtrl->mValues;

    if( sPathCtrl->mIterator >= SMG_SESSION_STAT_ROW_COUNT )
    {
        sSessEnv = knlGetNextSessionEnv( sPathCtrl->mCurEnv );

        while( sSessEnv != NULL )
        {
            if( knlIsUsedSessionEnv( sSessEnv ) == STL_TRUE )
            {
                sRowIdx = 0;
                
                STL_TRY( knlGetSessionEnvId( sSessEnv,
                                             &sPathCtrl->mSessId,
                                             aKnlEnv )
                         == STL_SUCCESS );
            
                sValues[sRowIdx++] = ((smlSessionEnv*)sSessEnv)->mSegmentRetryCount;
                sValues[sRowIdx++] = ((smlSessionEnv*)sSessEnv)->mRelationRetryCount;
#ifdef STL_DEBUG
                sValues[sRowIdx++]= ((smlSessionEnv*)sSessEnv)->mFixCount;
#else
                sValues[sRowIdx++]= 0;
#endif
                sValues[sRowIdx++] = ((smlSessionEnv*)sSessEnv)->mVersionConflictCount;
                sValues[sRowIdx++]= ((smlSessionEnv*)sSessEnv)->mSessionMem.mTotalSize;
                sValues[sRowIdx++]= ((smlSessionEnv*)sSessEnv)->mSessionMem.mInitSize;
                sValues[sRowIdx++]= ((smlSessionEnv*)sSessEnv)->mStatementMem.mTotalSize;
                sValues[sRowIdx++]= ((smlSessionEnv*)sSessEnv)->mStatementMem.mInitSize;
                sValues[sRowIdx++] = ((smlSessionEnv*)sSessEnv)->mDynamicMem.mTotalSize;
                sValues[sRowIdx++] = ((smlSessionEnv*)sSessEnv)->mDynamicMem.mInitSize;
                
                break;
            }
        
            sSessEnv = knlGetNextSessionEnv( sSessEnv );
        }

        sPathCtrl->mCurEnv = sSessEnv;
        sPathCtrl->mIterator = 0;
    }
    
    STL_TRY_THROW( sPathCtrl->mCurEnv != NULL, RAMP_SUCCESS );

    sRowDesc = &gSmgSessionStatRows[sPathCtrl->mIterator];

    sFxSessionInfo.mName     = sRowDesc->mName;
    sFxSessionInfo.mCategory = sRowDesc->mCategory;
    sFxSessionInfo.mSessId   = sPathCtrl->mSessId;
    sFxSessionInfo.mValue    = sValues[sPathCtrl->mIterator];

    STL_TRY( knlBuildFxRecord( gKnlSessionInfoColumnDesc,
                               &sFxSessionInfo,
                               aValueList,
                               aBlockIdx,
                               aKnlEnv )
             == STL_SUCCESS );

    *aTupleExist = STL_TRUE;
    sPathCtrl->mIterator += 1;

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

knlFxTableDesc gSmgSessionStatTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    NULL,
    smgOpenSessionStatFxTableCallback,
    smgCloseSessionStatFxTableCallback,
    smgBuildRecordSessionStatFxTableCallback,
    STL_SIZEOF( smgFxSessionStatPathCtrl ),
    "X$SM_SESS_STAT",
    "session statistic information of storage manager layer",
    gKnlSessionInfoColumnDesc
};

/** @} */
