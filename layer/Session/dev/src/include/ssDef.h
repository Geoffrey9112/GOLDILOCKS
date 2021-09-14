/*******************************************************************************
 * ssDef.h
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


#ifndef _SSDEF_H_
#define _SSDEF_H_ 1

/**
 * @file ssDef.h
 * @brief Session Layer Internal Definitions
 */

/**
 * @defgroup ssInternal Session Layer Internal Definitions
 * @internal
 * @{
 */

/**
 * @addtogroup sssSession
 * @{
 */

#define SS_CONNECTION_TYPE_LENGTH 8
#define SS_SERVER_TYPE_LENGTH     10
#define SS_STATUS_LENGTH          15
#define SS_WATCH_LENGTH           8
#define SS_PROGRAM_LENGTH         64

/*
 * X$SESSION
 */
typedef struct sssSessionPathControl
{
    void * mSession;
} sssSessionPathControl;

typedef struct sssSessionFxRecord
{
    stlUInt32 mID;
    stlUInt64 mSerial;
    stlInt64  mUserId;
    stlInt64  mTransId;
    stlInt16  mTopLayer;
    stlChar   mConnectionType[SS_CONNECTION_TYPE_LENGTH];
    stlChar   mWatch[SS_WATCH_LENGTH];
    stlChar   mStatus[SS_STATUS_LENGTH];
    stlChar   mServerType[SS_SERVER_TYPE_LENGTH];
    stlInt64  mClientProcId;
    stlInt64  mServerProcId;
    stlTime   mLogonTime;
    stlChar   mProgram[SS_PROGRAM_LENGTH];
    stlChar   mClientAddress[STL_MAX_HOST_LEN + 1];
    stlInt64  mClientPort;
} sssSessionFxRecord;

/*
 * X$STATEMENT
 */

typedef struct sssStmtPathControl
{
    knlSessionEnv * mCurEnv;
    stlInt32        mCurSlot;
} sssStmtPathControl;

typedef struct sssFxStmt
{
    stlInt64        mId;
    stlUInt32       mSessId;
    stlInt64        mViewScn;
    stlChar         mStmtSql[QLL_MAX_STMT_SQL_SIZE];
    stlTime         mStartTime;
} sssFxStmt;

/*
 * X$SS_SESS_STAT
 */

#define SSS_SS_SESS_ENV_ROW_COUNT (2)

typedef struct sssSessStatPathControl
{
    knlSessionEnv * mCurEnv;
    stlUInt32       mSessId;
    stlInt32        mIterator;
    stlInt64        mValues[SSS_SS_SESS_ENV_ROW_COUNT];
} sssSessStatPathControl;

/*
 * X$SS_STMT_STAT
 */

#define SSS_SS_STMT_STAT_ROW_COUNT (12)

typedef struct sssStmtStatPathControl
{
    knlSessionEnv * mCurEnv;
    stlUInt32       mSessId;
    stlInt32        mSlot;
    stlInt64        mStmtId;
    stlInt32        mIterator;
    stlInt64        mValues[SSS_SS_STMT_STAT_ROW_COUNT];
} sssStmtStatPathControl;

#define SSS_LOGIN_LOGGER_NAME     "login.trc"
#define SSS_LOGIN_LOGGER_MAXSIZE  (10*1024*1024)

/** @} */

#define SSR_HAS_CURSOR_NAME( aStatement )                               \
    ( (qllNeedFetch( &((aStatement)->mSqlStmt) ) == STL_TRUE ) &&       \
      (aStatement)->mCursorDesc != NULL ) ? STL_TRUE : STL_FALSE        \

/**
 * @addtogroup ssxContext
 * @{
 */

#define SSX_XA_LOGGER_NAME     "xa.trc"
#define SSX_XA_LOGGER_MAXSIZE  (10*1024*1024)

#define SSX_CONTEXT_ARRAY_SIZE  ( 1024 )

typedef struct ssxContextArray
{
    stlRingEntry mContextArrayLink;
    knlXaContext mContext[SSX_CONTEXT_ARRAY_SIZE];
} ssxContextArray;

typedef struct ssxLocalTrans
{
    smlTransId mSslTransId;
    stlInt64   mSmlTransId;
} ssxLocalTrans;

/** @} */

/**
 * @addtogroup ssxFxTable
 * @{
 */

typedef struct ssxFxGlobalTrans
{
    stlChar       mXid[STL_XID_DATA_SIZE * 3];
    smlTransId    mTransId;
    stlChar       mState[32];
    stlChar       mAssociateState[32];
    stlTime       mBeginTime;
    stlBool       mRepreparable;
} ssxFxGlobalTrans;

typedef struct ssxFxGlobalTransPathCtrl
{
    ssxContextArray * mCurContextArray;
    stlInt64          mCurPosInContextArray;
} ssxFxGlobalTransPathCtrl;

/** @} */

/**
 * @addtogroup ssgStartup
 * @{
 */

#define SSG_DATABASE_SHMMEM_CHUNK_SIZE   ( 1024 * 1024 )

typedef struct ssgWarmupEntry
{
    knlDynamicMem   mShmMem;
    stlRingHead     mContextArrList;
    stlRingHead     mContextFreeList;
    knlLatch        mContextLatch;
    knlTraceLogger  mXaLogger;
    knlTraceLogger  mLoginLogger;
} ssgWarmupEntry;

/** @} */


/** @} */

#endif /* _SSDEF_H_ */
