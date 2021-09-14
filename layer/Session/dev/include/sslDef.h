/*******************************************************************************
 * sslDef.h
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

#ifndef _SSLDEF_H_
#define _SSLDEF_H_ 1

/**
 * @file sslDef.h
 * @brief Session Layer Definition
 */

#include <sslConstDef.h>

/*******************************************************************************
 * Error 
 ******************************************************************************/

/**
 * @defgroup sslDef Definition
 * @ingroup ssExternal
 * @{
 */

/** @} ssExternal */


/**
 * @defgroup sslError Error
 * @ingroup ssExternal
 * @{
 */

#define SSL_ERRCODE_INVALID_STATEMENT STL_MAKE_ERRCODE( STL_ERROR_MODULE_SESSION, 0 )

#define SSL_ERRCODE_INVALID_ATTRIBUTE_VALUE STL_MAKE_ERRCODE( STL_ERROR_MODULE_SESSION, 1 )

#define SSL_ERRCODE_INVALID_ATTRIBUTE_OPTION_IDENTIFIER STL_MAKE_ERRCODE( STL_ERROR_MODULE_SESSION, 2 )

#define SSL_ERRCODE_TRANSACTION_IS_READ_ONLY STL_MAKE_ERRCODE( STL_ERROR_MODULE_SESSION, 3 )

#define SSL_ERRCODE_STATEMENT_CANNOT_EXECUTE_AS_ATOMIC_ACTION STL_MAKE_ERRCODE( STL_ERROR_MODULE_SESSION, 4 )

#define SSL_ERRCODE_INVALID_PREPARABLE_PHASE STL_MAKE_ERRCODE( STL_ERROR_MODULE_SESSION, 5 )

#define SSL_ERRCODE_COMMUNICATION_LINK_FAILURE STL_MAKE_ERRCODE( STL_ERROR_MODULE_SESSION, 6 )

#define SSL_ERRCODE_NOT_IMPLEMENTED STL_MAKE_ERRCODE( STL_ERROR_MODULE_SESSION, 7 )

#define SSL_ERRCODE_LIMIT_ON_THE_NUMBER_OF_HANDLES_EXCEEDED STL_MAKE_ERRCODE( STL_ERROR_MODULE_SESSION, 8 )

#define SSL_ERRCODE_NOT_ALLOW_COMMAND_IN_XA STL_MAKE_ERRCODE( STL_ERROR_MODULE_SESSION, 9 )

#define SSL_ERRCODE_INVALID_DESCRIPTOR_INDEX STL_MAKE_ERRCODE( STL_ERROR_MODULE_SESSION, 10 )

#define SSL_ERRCODE_CHARACTERSET_NOT_EQUAL STL_MAKE_ERRCODE( STL_ERROR_MODULE_SESSION, 11 )

#define SSL_MAX_ERROR     12

/** @} sslError */

/**
 * @defgroup sslStatement Statement
 * @ingroup ssExternal
 */

typedef struct sslDCLContext
{
    sslDCLType      mType;
    stlInt16        mGMTOffset;
    dtlCharacterSet mCharacterSet;
    dtlCharacterSet mNCharCharacterSet;
    /**
     * @todo ALTER SESSION SET DATE_FORMAT이 지원되면 DATE_FORMAT 정보도 필요하다.
     */
} sslDCLContext;

#define SSL_INIT_DCL_CONTEXT( aContext )                        \
    (aContext)->mType              = SSL_DCL_TYPE_NONE;         \
    (aContext)->mGMTOffset         = 0;                         \
    (aContext)->mCharacterSet      = DTL_UTF8;                  \
    (aContext)->mNCharCharacterSet = DTL_CHARACTERSET_MAX

/**
 * @brief Statement/Session Attribute가 가질수 있는 최대 버퍼 크기
 */
#define SSL_ATTRIBUTE_BUFFER_SIZE   ( 8192 )

#define SSL_STATEMENT_SLOT_PRECISION 16
#define SSL_STATEMENT_SEQ_PRECISION  48

#define SSL_STATEMENT_SLOT_MASK      STL_UINT64_C(0xFFFF000000000000)

#define SSL_GET_STATEMENT_SLOT( aStatementId )                                          \
    ( ((aStatementId) & SSL_STATEMENT_SLOT_MASK ) >> SSL_STATEMENT_SEQ_PRECISION )

#define SSL_GET_STATEMENT_SEQ( aStatementId )           \
    ( (aStatementId) & ~SSL_STATEMENT_SLOT_MASK )

#define SSL_INCREASE_STATEMENT_SEQ( aSessionEnv )                             \

#define SSL_MAKE_STATEMENT_ID( aSlot, aSeq )                            \
    ( ((stlUInt64)(aSlot) << SSL_STATEMENT_SEQ_PRECISION ) + aSeq )

#define  SSL_OPTIMIZE_DECISION_NONE               ( 0 )
#define  SSL_OPTIMIZE_DECISION_FIRST_EXECUTION    ( 0x00000001 )
#define  SSL_OPTIMIZE_DECISION_BIND_INFO          ( 0x00000002 )
#define  SSL_OPTIMIZE_DECISION_ARRAY_SIZE         ( 0x00000004 )
#define  SSL_OPTIMIZE_DECISION_ATOMIC_EXECUTION   ( 0x00000008 )

/**
 * @brief Session Statement
 */
typedef struct sslStatement
{
    stlRingEntry             mLink;
    qllDBCStmt               mDbcStmt;
    qllStatement             mSqlStmt;
    qllTarget              * mTargetInfo;
    qllResultSetDesc       * mResultSetCursor;
    stlInt64                 mRowsetFirstRow;   /**< Cursor가 갖고 있는 rowset의 첫 row 번호 */
    stlInt64                 mRowsetCurrentPos; /**< rowset에서 현재 row의 위치 (0 base) */
    stlInt64                 mRowsetLastPos;    /**< rowset에서 마지막 row의 위치 (0 base) */
    smlStatement           * mCursorStmt;       /**< Unnamed cursor statement */
    smlScn                   mViewScn;          /**< statement view scn */
    ellCursorInstDesc      * mCursorDesc;
    ellCursorStandardType    mCursorType;
    ellCursorSensitivity     mSensitivity;
    ellCursorScrollability   mScrollability;
    ellCursorHoldability     mHoldability;
    ellCursorUpdatability    mUpdatability;
    stlChar                  mCursorName[STL_MAX_SQL_IDENTIFIER_LENGTH];

    knlBindContext         * mBindContext;
    knlRegionMem             mParamRegionMem;
    knlRegionMem             mColumnRegionMem;
    stlInt64                 mId;
    sslExplainType           mExplainType;
    stlBool                  mIsEndOfScan;
    stlUInt64                mCurrArraySize;
    stlUInt64                mNextArraySize;
    stlBool                  mAtomicExecute;
    stlBool                  mIsRecompile;
    stlBool                  mTargetChanged;
    stlBool                  mNeedOptimization;
    stlInt32                 mOptimizeDecision;
    stlInt32                 mQueryTimeout;
} sslStatement;

/**
 * @brief FetchScroll에서 first row idx를 위한 상수
 */
#define SSL_CURSOR_POSITION_BEFORE_FIRST    ( STL_INT64_MIN )
#define SSL_CURSOR_POSITION_AFTER_END       ( STL_INT64_MAX )

/**
 * @brief unknown last row idx for sslFetch function
 */
#define SSL_LAST_ROW_IDX_UNKNOWN     ( -1 )



/** @} sslStatement */

/**
 * @defgroup sslXaCommand XA command
 * @ingroup ssExternal
 */

/**
 * @brief flags definitions for the RM switch
 */

#define SSL_TM_NOFLAGS   ( STL_INT64_C(0x00000000) )  /**< no resource manager features selected */
#define SSL_TM_REGISTER  ( STL_INT64_C(0x00000001) )  /**< resource manager dynamically registers */
#define SSL_TM_NOMIGRATE ( STL_INT64_C(0x00000002) )  /**< resource manager does not support migration */
#define SSL_TM_USEASYNC  ( STL_INT64_C(0x00000004) )  /**< resource manager supports asynch operations */

/**
 * @brief flags definitions for xa_ and ax_ routines
 */

#define SSL_TM_ASYNC       ( STL_INT64_C(0x80000000) )  /**< perform routine asynchronously */
#define SSL_TM_ONEPHASE    ( STL_INT64_C(0x40000000) )  /**< one-phase commit */
#define SSL_TM_FAIL        ( STL_INT64_C(0x20000000) )  /**< dissociates caller and marks branch rollback-only */
#define SSL_TM_NOWAIT      ( STL_INT64_C(0x10000000) )  /**< return if blocking condition exists */
#define SSL_TM_RESUME      ( STL_INT64_C(0x08000000) )  /**< resuming association */
#define SSL_TM_SUCCESS     ( STL_INT64_C(0x04000000) )  /**< dissociates caller from branch */
#define SSL_TM_SUSPEND     ( STL_INT64_C(0x02000000) )  /**< suspending, not ending, association */
#define SSL_TM_STARTRSCAN  ( STL_INT64_C(0x01000000) )  /**< start a recovery scan */
#define SSL_TM_ENDRSCAN    ( STL_INT64_C(0x00800000) )  /**< end a recovery scan */
#define SSL_TM_MULTIPLE    ( STL_INT64_C(0x00400000) )  /**< wait for any async operation */
#define SSL_TM_JOIN        ( STL_INT64_C(0x00200000) )  /**< joining existing branch */
#define SSL_TM_MIGRATE     ( STL_INT64_C(0x00100000) )  /**< intends to perform migration */

/**
 * @brief error definitions for XA
 */

#define SSL_XA_RBBASE       100                 /**< the inclusive lower bound of the rollback codes */
#define SSL_XA_RBROLLBACK   SSL_XA_RBBASE       /**< the rollback was caused by an unspecified reason */
#define SSL_XA_RBCOMMFAIL   SSL_XA_RBBASE+1     /**< the rollback was caused by a communication failure */
#define SSL_XA_RBDEADLOCK   SSL_XA_RBBASE+2     /**< a deadlock was detected */
#define SSL_XA_RBINTEGRITY  SSL_XA_RBBASE+3     /**< a condition that violates the integrity of the resources was detected */
#define SSL_XA_RBOTHER      SSL_XA_RBBASE+4     /**< the resource manager rolled back the transaction
                                                     branch for a reason not on this list */
#define SSL_XA_RBPROTO      SSL_XA_RBBASE+5     /**< a protocol error occurred in the resource manager */
#define SSL_XA_RBTIMEOUT    SSL_XA_RBBASE+6     /**< a transaction branch took too long */
#define SSL_XA_RBTRANSIENT  SSL_XA_RBBASE+7     /**< may retry the transaction branch */
#define SSL_XA_RBEND        SSL_XA_RBTRANSIENT  /**< the inclusive upper bound of the rollback codes */
#define SSL_XA_NOMIGRATE    9               /**< resumption must occur where suspension occurred */
#define SSL_XA_HEURHAZ      8               /**< the transaction branch may have been heuristically completed */
#define SSL_XA_HEURCOM      7               /**< the transaction branch has been heuristically committed */
#define SSL_XA_HEURRB       6               /**< the transaction branch has been heuristically rolled back */
#define SSL_XA_HEURMIX      5               /**< the transaction branch has been heuristically committed and rolled back */
#define SSL_XA_RETRY        4               /**< routine returned with no effect and may be re-issued */
#define SSL_XA_RDONLY       3               /**< the transaction branch was read-only and has been committed */
#define SSL_XA_OK           0               /**< normal execution */
#define SSL_XAER_ASYNC      -2              /**< asynchronous operation already outstanding */
#define SSL_XAER_RMERR      -3              /**< a resource manager error occurred in the transaction branch */
#define SSL_XAER_NOTA       -4              /**< the XID is not valid */
#define SSL_XAER_INVAL      -5              /**< invalid arguments were given */
#define SSL_XAER_PROTO      -6              /**< routine invoked in an improper context */
#define SSL_XAER_RMFAIL     -7              /**< resource manager unavailable */
#define SSL_XAER_DUPID      -8              /**< the XID already exists */
#define SSL_XAER_OUTSIDE    -9              /**< resource manager doing work outside transaction */

typedef enum
{
    SSL_XA_SCAN_STATE_NONE,          /**< don't scan */
    SSL_XA_SCAN_STATE_CONTEXT,       /**< scan memory contexts */
    SSL_XA_SCAN_STATE_DB,            /**< scan pending_trans */
} sslXaScanState;

typedef struct sslXaRecoverScanInfo
{
    sslXaScanState      mScanState;
    knlRegionMark       mMemMark;
    stlChar             mValue[STL_XID_STRING_SIZE];
    smlFetchInfo        mFetchInfo;
    knlValueBlockList * mColumnList;
    smlRowBlock         mRowBlock;
    smlStatement      * mStatement;
    void              * mIterator;
    void              * mCurContextArray;
    stlInt64            mCurContextIdx;
} sslXaRecoverScanInfo;

/** @} sslXaCommand */

/**
 * @defgroup sslEnvironment Environment
 * @ingroup ssExternal
 */


/**
 * @defgroup sslProcEnv Process Environment
 * @ingroup sslEnvironment
 * @{
 */

/**
 * @brief Session 의 Process Environment 구조체
 */
typedef struct sslEnv
{
    qllEnv                mQllEnv;              /**< 하위 Layer 의 Process Environment */
    knlRegionMem          mOperationHeapMem;    /**< Operation 레벨 힙 메모리 할당/해제 */
    sslXaRecoverScanInfo  mXaRecoverScanInfo;
} sslEnv;

/**
 * @brief Session Environment
 */
#define SSL_ENV( aEnv )      ( (sslEnv *)aEnv )

/**
 * @brief Sessionn Environment에서 ErrorStack을 얻는 매크로
 */
#define SSL_ERROR_STACK( aEnv ) &((KNL_ENV(aEnv))->mErrorStack)

/** @} sslProcEnv */

/**
 * @defgroup sslSessEnv Session Environment
 * @ingroup sslEnvironment
 * @{
 */

#define SSL_PROGRAM_NAME (64)

/**
 * @brief Session의 Session Environment 구조체
 */
typedef struct sslSessionEnv
{
    qllSessionEnv        mQllSessionEnv; /**< 하위 Layer 의 Session Environment */
    smlTransId           mTransId;
    knlDynamicMem        mMemStatement;
    stlInt64             mStatementSeq;
    qllAccessMode        mDefaultAccessMode;
    qllAccessMode        mNextAccessMode;      /**< SET TRANSACTION에서 설정한 ACCESS MODE */
    sslIsolationLevel    mDefaultTxnIsolation;
    sslIsolationLevel    mCurrTxnIsolation;
    sslIsolationLevel    mNextTxnIsolation;    /**< SET TRANSACTION에서 설정한 ISOLATION LEVEL */
    qllUniqueMode        mDefaultUniqueMode;
    qllUniqueMode        mNextUniqueMode;      /**< SET TRANSACTION에서 설정한 UNIQUE INTEGRITY */
    stlProc              mClientProcId;        /** client process id for dedicate mode */
    stlChar              mClientAddress[STL_MAX_HOST_LEN + 1];
    stlInt64             mClientPort;
    stlChar              mProgram[SSL_PROGRAM_NAME];
    stlInt64             mMaxStmtCount;
    stlInt32             mActiveStmtCount;
    stlInt32             mNextStmtSlot;
    stlBool              mHasWithoutHoldCursor;
    knlXaContext       * mXaContext;
    sslStatement       * mLatestStatement;
    sslStatement      ** mStatementSlot;
} sslSessionEnv;

/**
 * @brief Session의 Session Environment 를 얻는다.
 * @param[in]  aEnv Process Environment 포인터
 */
#define SSL_SESS_ENV( aEnv )    ((sslSessionEnv*)(KNL_ENV(aEnv)->mSessionEnv))

/**
 * @brief Session의 Environment 를 얻는다.
 * @param[in]  aSessionEnv Session Environment 포인터
 */
#define SSL_WORKER_ENV( aSessionEnv ) ((sslEnv*)((knlSessionEnv*)(aSessionEnv))->mEnv)

/*
 * Statement 할당을 위한 메모리 크기의 단위
 */
#define SSL_SESSION_MEM_INIT_SIZE       ( 32 * 1024 )
#define SSL_SESSION_MEM_NEXT_SIZE       ( 32* 1024 )

#define SSL_INVALID_SESSION_ID          ( -1 )

/** @} sslSessEnv */

/** @} sslEnvironment */

#endif /* _SSLDEF_H_ */

