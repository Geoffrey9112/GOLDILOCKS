/*******************************************************************************
 * smgDef.h
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


#ifndef _SMGDEF_H_
#define _SMGDEF_H_ 1

/**
 * @file smgDef.h
 * @brief Storage Manager Layer Startup Internal Definition
 */

/**
 * @defgroup smgInternal Component Internal Routines
 * @ingroup smg
 * @{
 */

typedef struct smgWarmupEntry
{
    knlDynamicMem  mDatabaseMem4Open;
    knlDynamicMem  mDatabaseMem4Mount;
    stlUInt32      mCheckpointEnvId;
    stlUInt32      mLogFlusherEnvId;
    stlUInt32      mPageFlusherEnvId;
    stlUInt32      mAgerEnvId;
    stlUInt32      mCleanupEnvId;
    stlUInt32      mArchivelogEnvId;
    stlUInt32      mIoSlaveEnvId[SML_MAX_PARALLEL_IO_GROUP];
    stlInt64       mIoSlaveCount;
    stlInt16       mAgerState;
    stlChar        mPadding[2];
} smgWarmupEntry;

/**
 * @brief X$SM_PROC_ENV를 위한 자료구조
 */
typedef struct smgFxEnv
{
    stlUInt32 mID;
    stlUInt32 mCheckpointEnvId;
    stlUInt32 mAgerEnvId;
    stlUInt32 mCleanupEnvId;
    stlUInt32 mArchivelogEnvId;
    stlInt16  mMiniTransStackTop;
} smgFxEnv;

#define SMG_SYSTEM_INFO_ROW_COUNT   (37)
#define SMG_PROC_ENV_ROW_COUNT      (5)
#define SMG_SESSION_ENV_ROW_COUNT   (7)
#define SMG_SESSION_STAT_ROW_COUNT  (10)

/**
 * @brief X$SM_SYSTEM_INFO를 위한 자료구조
 */
typedef struct smgFxSystemInfoPathCtrl
{
    stlInt32 mIterator;
    stlInt64 mValues[SMG_SYSTEM_INFO_ROW_COUNT];
} smgFxSystemInfoPathCtrl;

/**
 * @brief X$SM_SESS_ENV를 위한 자료구조
 */
typedef struct smgFxSessionEnvPathCtrl
{
    knlSessionEnv * mCurEnv;
    stlUInt32       mSessId;
    stlInt32        mIterator;
    stlInt64        mValues[SMG_SESSION_ENV_ROW_COUNT];
} smgFxSessionEnvPathCtrl;

/**
 * @brief X$SM_SESS_STAT를 위한 자료구조
 */
typedef struct smgFxSessionStatPathCtrl
{
    knlSessionEnv * mCurEnv;
    stlUInt32       mSessId;
    stlInt32        mIterator;
    stlInt64        mValues[SMG_SESSION_STAT_ROW_COUNT];
} smgFxSessionStatPathCtrl;

/**
 * @brief X$SM_PROC_ENV를 위한 자료구조
 */
typedef struct smgFxProcEnvPathCtrl
{
    knlEnv     * mCurEnv;
    stlUInt32    mEnvId;
    stlInt32     mIterator;
    stlInt64     mValues[SMG_PROC_ENV_ROW_COUNT];
} smgFxProcEnvPathCtrl;

/**
 * @addtogroup smrUndoLogType
 * @{
 */
#define SMG_UNDO_SUPPL_LOG_DDL              0  /**< supplemental undo log */
/** @} */

#define SMG_UNDO_SUPPL_LOG_MAX_COUNT       (1)

/** @} */
    
#endif /* _SMGDEF_H_ */
