/*******************************************************************************
 * slDef.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: slDef.h 13675 2014-10-06 14:17:27Z lym999 $
 *
 * NOTES
 *
 *
 ******************************************************************************/


#ifndef _SLDEF_H_
#define _SLDEF_H_ 1

/**
 * @file slDef.h
 * @brief Gliese Server Difinitions
 */

/**
 * @defgroup slInternal Gliese Server Routines
 * @{
 */

#define SL_POLL_TIMEOUT STL_SET_SEC_TIME(1)

/**
 * @addtogroup slBoot
 * @{
 */

#define SLP_ATTR_BUFFER_MAX_SIZE    (8192)
#define SLP_MAX_COLUMN_NAME_LEN     (128)

#define SLB_ENV_STATE_INIT          0
#define SLB_ENV_STATE_CREATING      1
#define SLB_ENV_STATE_CREATED       2
#define SLB_ENV_STATE_DROPPING      3

#define SLB_BOOT_STATE_INIT         0
#define SLB_BOOT_STATE_WARMMING     1
#define SLB_BOOT_STATE_WARMED       2
#define SLB_BOOT_STATE_COOLING      3

#define SLB_CAS_INIT                0
#define SLB_CAS_LOCKED              1

/* */
#define SLB_SESSION_MEM_INIT_SIZE       ( 1024 * STL_SIZEOF(sllHandle) )
#define SLB_SESSION_MEM_NEXT_SIZE       ( 1024 * STL_SIZEOF(sllHandle) )

#define SL_CM_BUFFER_SIZE ( CML_BUFFER_SIZE )

#define SL_ENV_DB_HOME            "GOLDILOCKS_DATA"
#define SL_UNIXDOMAIN_NAME        "goldilocks-unix"

/* explain plan values */
#define SL_SQL_EXPLAIN_PLAN_OFF                0
#define SL_SQL_EXPLAIN_PLAN_ON                 1
#define SL_SQL_EXPLAIN_PLAN_ON_VERBOSE         2
#define SL_SQL_EXPLAIN_PLAN_ONLY               3

/* atomic execution values */
#define SL_SQL_ATOMIC_EXECUTION_OFF            0
#define SL_SQL_ATOMIC_EXECUTION_ON             1

typedef enum
{
    SL_STATE_INIT = 0,
    SL_STATE_RUN,
    SL_STATE_DOWN,
    SL_STATE_STOPPED
} slState;

typedef enum
{
    SL_SERVER_STATUS_INIT = 0,
    SL_SERVER_STATUS_START,
    SL_SERVER_STATUS_SHUTDOWN,
    SL_SERVER_STATUS_EXIT
} slServerStatus;


/**
 * @brief Gliese describe column
 */
typedef struct slDescCol
{
    stlChar         mColumnName[SLP_MAX_COLUMN_NAME_LEN];
    stlInt16        mType;
    stlInt64        mColumnSize;
    stlInt16        mDecimalDigit;
    stlBool         mNullable;
} slDescCol;

#define SLB_SYSTEM_INFO_ROW_COUNT   ( 6 )
#define SLB_STATUS_LEN              ( 16 )
#define SLB_TYPE_LEN                ( 16 )

typedef struct slbFxSystemInfoPathCtrl
{
    stlInt32 mIterator;
    stlInt64 mValues[SLB_SYSTEM_INFO_ROW_COUNT];
} slbFxSystemInfoPathCtrl;

typedef struct slbFxDispatcher
{
    stlInt64 mProcId;
    stlInt64 mResponseJobCount;
    stlBool  mAccept;
    stlTime  mStartTime;
    stlInt64 mCurrentConnections;
    stlInt64 mConnections;
    stlInt64 mConnectionsHighwater;
    stlInt64 mMaxConnections;
    stlChar  mRecvStatus[SLB_STATUS_LEN];
    stlInt64 mTotalRecvBytes;
    stlInt64 mTotalRecvUnits;
    stlInt64 mRecvIdle;
    stlInt64 mRecvBusy;
    stlChar  mSendStatus[SLB_STATUS_LEN];
    stlInt64 mTotalSendBytes;
    stlInt64 mTotalSendUnits;
    stlInt64 mSendIdle;
    stlInt64 mSendBusy;
} slbFxDispatcher;

typedef struct slbFxSharedServer
{
    stlInt64 mProcId;
    stlInt64 mProcessedJobCount;
    stlChar  mStatus[SLB_STATUS_LEN];
    stlInt64 mIdle;
    stlInt64 mBusy;
    /**
     * Shared-server에서 byte, unit수는 체크하기 어려움.
     */
} slbFxSharedServer;

typedef struct slbFxBalancer
{
    stlInt64 mProcId;
    stlInt64 mCurrentConnections;
    stlInt64 mConnections;
    stlInt64 mConnectionsHighwater;
    stlInt64 mMaxConnections;
    stlChar  mStatus[SLB_STATUS_LEN];
} slbFxBalancer;

typedef struct slbFxQueue
{
    stlChar  mType[SLB_TYPE_LEN];
    stlInt64 mIndex;
    stlInt64 mQueued;
    stlInt64 mWait;
    stlInt64 mTotalQueue;
} slbFxQueue;


/** @} */

/**
 * @brief Gliese describe column
 */

/** @} */

extern stlErrorRecord       gServerLibraryErrorTable[];

/** @} */

extern sllSharedMemory  * gSlSharedMemory;

/** @} */

/** @} */

#endif /* _SLDEF_H_ */
