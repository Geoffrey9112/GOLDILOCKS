/*******************************************************************************
 * zlDef.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zlDef.h 12380 2014-05-15 02:00:20Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZL_DEF_H_
#define _ZL_DEF_H_ 1

/**
 * @file zlDef.h
 * @brief GOLDILOCKS Library Internal Types.
 */

#include <zllConstDef.h>
#include <zllCursorDef.h>

#define ZLB_WARM_STATE_INIT      0
#define ZLB_WARM_STATE_WARMMING  1
#define ZLB_WARM_STATE_WARMED    2
#define ZLB_WARM_STATE_COOLING   3

/*
 * ODBC connections have the following states.
 * C0 : Unallocated environment, unallocated connection
 * C1 : Allocated environment, unallocated connection
 * C2 : Allocated environment, allocated connection
 * C3 : Connection function needs data
 * C4 : Connected connection
 * C5 : Connected connection, allocated statement
 * C6 : Connected connection, transaction in progress.
 *      It is possible for a connection to be in state C6
 *      with no statements allocated on the connection.
 *      For example, suppose the connection is in manual commit mode
 *      and is in state C4. If a statement is allocated,
 *      executed (starting a transaction), and then freed,
 *      the transaction remains active but there are no statements on the connection.
 */

#define ZLC_TRANSITION_STATE_C0 0
#define ZLC_TRANSITION_STATE_C1 1
#define ZLC_TRANSITION_STATE_C2 2
#define ZLC_TRANSITION_STATE_C3 3
#define ZLC_TRANSITION_STATE_C4 4
#define ZLC_TRANSITION_STATE_C5 5
#define ZLC_TRANSITION_STATE_C6 6

/*
 * ODBC statements have the following states.
 * S0 : Unallocated statement. (The connection state must be C4, C5, or C6.
 *      For more information, see Connection Transitions.)
 * S1 : Allocated statement.
 * S2 : Prepared statement. No result set will be created.
 * S3 : Prepared statement. A (possibly empty) result set will be created.
 * S4 : Statement executed and no result set was created.
 * S5 : Statement executed and a (possibly empty) result set was created.
 *      The cursor is open and positioned before the first row of the result set.
 * S6 : Cursor positioned with SQLFetch or SQLFetchScroll.
 * S7 : Cursor positioned with SQLExtendedFetch.
 * S8 : Function needs data. SQLParamData has not been called.
 * S9 : Function needs data. SQLPutData has not been called.
 * S10 : Function needs data. SQLPutData has been called.
 * S11 : Still executing. A statement is left in this state after a function
 *       that is executed asynchronously returns SQL_STILL_EXECUTING.
 *       A statement is temporarily in this state while any function
 *       that accepts a statement handle is executing.
 *       Temporary residence in state S11 is not shown in any state tables
 *       except the state table for SQLCancel.
 *       While a statement is temporarily in state S11,
 *       the function can be canceled by calling SQLCancel from another thread.
 * S12 : Asynchronous execution canceled.
 *       In S12, an application must call the canceled function until
 *       it returns a value other than SQL_STILL_EXECUTING.
 *       The function was canceled successfully only if the function returns SQL_ERROR
 *       and SQLSTATE HY008 (Operation canceled). If it returns any other value,
 *       such as SQL_SUCCESS, the cancel operation failed and
 *       the function executed normally.
 */
#define ZLS_TRANSITION_STATE_S0  0
#define ZLS_TRANSITION_STATE_S1  1
#define ZLS_TRANSITION_STATE_S2  2
#define ZLS_TRANSITION_STATE_S3  3
#define ZLS_TRANSITION_STATE_S4  4
#define ZLS_TRANSITION_STATE_S5  5
#define ZLS_TRANSITION_STATE_S6  6
#define ZLS_TRANSITION_STATE_S7  7
#define ZLS_TRANSITION_STATE_S8  8
#define ZLS_TRANSITION_STATE_S9  9
#define ZLS_TRANSITION_STATE_S10 10
#define ZLS_TRANSITION_STATE_S11 11
#define ZLS_TRANSITION_STATE_S12 12

/*
 * ODBC descriptors have the following three states.
 * D0  : Unallocated descriptor
 * D1i : Implicitly allocated descriptor
 * D1e : Explicitly allocated descriptor
 */
#define ZLD_TRANSITION_STATE_D0  0
#define ZLD_TRANSITION_STATE_D1i 1
#define ZLD_TRANSITION_STATE_D1e 2

typedef struct zlnEnv         zlnEnv;
typedef struct zlcDbc         zlcDbc;
typedef struct zlsStmt        zlsStmt;
typedef struct zldDesc        zldDesc;
typedef struct zldDescElement zldDescElement;

#define ZL_REGION_INIT_SIZE (8 * 1024)
#define ZL_REGION_NEXT_SIZE (8 * 1024)

/*
 * Diagnostic
 */

typedef struct zldDiag
{
    stlRingHead    mDiagRing;
    stlAllocator   mRegionMem;
    stlChar      * mDynamicFunction;
    stlInt16       mReturnCode;
    stlInt16       mCurrentRec;
    stlInt32       mSpinLock;
    stlInt32       mDynamicFunctionCode;
    stlInt32       mNumber;
    SQLLEN         mCursorRowCount;
    SQLLEN         mRowCount;
} zldDiag;

typedef struct zldDiagElement
{
    stlRingEntry   mLink;
    stlChar      * mConnectionName;
    stlChar      * mMessageText;
    stlChar      * mServerName;
    stlInt32       mColumnNumber;
    stlInt32       mNative;
    SQLLEN         mRowNumber;
    stlChar        mSqlState[6];
    stlChar        mClassOrigin[9];
    stlChar        mSubClassOrigin[9];
} zldDiagElement;

/*
 * Descriptor
 */
#define ZLD_DESC_TYPE_ARD     0
#define ZLD_DESC_TYPE_APD     1
#define ZLD_DESC_TYPE_IRD     2
#define ZLD_DESC_TYPE_IPD     3
#define ZLD_DESC_TYPE_UNKNOWN 4

#define ZLD_DEFAULT_MEMORY_SIZE 1024

struct zldDesc
{
    stlRingHead   mDescRecRing;
    zldDiag       mDiag;
    zlsStmt     * mStmt;
    stlUInt16   * mArrayStatusPtr;
    SQLLEN      * mBindOffsetPtr;
    SQLULEN     * mRowProcessed;
    stlInt16      mTransition;
    stlBool       mChanged;
    stlChar       mAlign[1];
    stlInt16      mType;
    stlInt16      mDescType;
    stlInt32      mBindType;
    stlInt16      mAllocType;
    stlInt16      mCount;
    SQLULEN       mArraySize;
    SQLULEN       mPutDataArraySize;
};

typedef stlStatus (*zlvSqlToCFunc) ( zlsStmt           * aStmt,
                                     dtlValueBlockList * aValueBlockList,
                                     dtlDataValue      * aDataValue,
                                     SQLLEN            * aOffset,
                                     void              * aTargetValuePtr,
                                     SQLLEN            * aIndicator,
                                     zldDescElement    * aArdRec,
                                     SQLRETURN         * aReturn,
                                     stlErrorStack     * aErrorStack );

typedef stlStatus (*zlvCtoSqlFunc) ( zlsStmt        * aStmt,
                                     void           * aParameterValuePtr,
                                     SQLLEN         * aIndicator,
                                     zldDescElement * aApdRec,
                                     zldDescElement * aIpdRec,
                                     dtlDataValue   * aDataValue,
                                     stlBool        * aSuccessWithInfo,
                                     stlErrorStack  * aErrorStack );

struct zldDescElement
{
    stlRingEntry        mLink;
    zlvSqlToCFunc       mSqlToCFunc;
    zlvCtoSqlFunc       mCtoSqlFunc;
    stlAllocator        mAllocator;
    dtlValueBlockList * mInputValueBlock;
    dtlValueBlockList * mOutputValueBlock;
    stlChar           * mBaseColumnName;
    stlChar           * mBaseTableName;
    stlChar           * mCatalogName;
    void              * mDataPtr;
    void             ** mPutDataBuffer;
    SQLULEN           * mPutDataBufferSize;
    SQLLEN            * mPutDataIndicator;
    stlChar           * mLabel;
    stlChar           * mLiteralPrefix;
    stlChar           * mLiteralSuffix;
    stlChar           * mLocalTypeName;
    stlChar           * mName;
    SQLLEN            * mIndicatorPtr;
    SQLLEN            * mOctetLengthPtr;
    stlChar           * mSchemaName;
    stlChar           * mTableName;
    stlChar           * mTypeName;
    stlBool             mChanged;
    stlBool             mAlign;
    stlInt16            mConsiceType;
    stlInt16            mDatetimeIntervalCode;
    stlInt16            mFixedPrecScale;
    stlInt16            mNullable;
    stlInt16            mParameterType;
    stlInt16            mPrecision;
    stlInt16            mRowver;
    stlInt16            mScale;
    stlInt16            mSearchable;
    stlInt16            mType;
    stlInt16            mUnnamed;
    stlInt16            mUnsigned;
    stlInt16            mUpdatable;
    stlInt32            mAutoUniqueValue;
    stlInt32            mCaseSensitive;
    stlInt32            mDatetimeIntervalPrecision;
    stlInt32            mNumPrecRadix;
    dtlStringLengthUnit mStringLengthUnit;
    SQLLEN              mDisplaySize;
    SQLULEN             mLength;
    SQLLEN              mOctetLength;
};

/*
 * Env
 */

typedef struct zlnEnvList
{
    stlRingHead  mEnvRing;
    stlUInt32    mAtomic;
} zlnEnvList;

struct zlnEnv
{
    stlRingHead  mDbcRing;
    stlRingEntry mLink;
    zldDiag      mDiag;
    stlInt16     mType;
    stlUInt32    mAtomic;
    stlInt32     mAttrVersion;
};

/*
 * Dbc
 */

#define ZLC_INIT_SESSION_ID  (STL_UINT32_MAX)
#define ZLC_INIT_SESSION_SEQ (STL_UINT64_MAX)

#define ZLC_CM_BUFFER_SIZE   ( 32 * 1024 )

typedef enum zlcProtocolType
{
    ZLC_PROTOCOL_TYPE_DA = 0,
    ZLC_PROTOCOL_TYPE_TCP,

    ZLC_PROTOCOL_TYPE_MAX,
} zlcProtocolType;

typedef stlStatus (*zlvConvertSQLFunc) ( zlsStmt        * aStmt,
                                         stlChar        * aStatementText,
                                         stlInt32         aTextLength,
                                         stlChar       ** aSQL,
                                         stlErrorStack  * aErrorStack );

typedef stlStatus (*zlvConvertResultSetFunc) ( zlsStmt        * aStmt,
                                               SQLLEN         * aOffset,
                                               dtlDataValue   * aDataValue,
                                               void           * aTargetValuePtr,
                                               stlInt64         aTargetLength,
                                               SQLLEN         * aIndicator,
                                               stlErrorStack  * aErrorStack );

typedef stlStatus (*zlvConvertErrorFunc) ( zlsStmt        * aStmt,
                                           stlErrorData   * aErrorData,
                                           stlChar        * aExtraMessage,
                                           zldDiag        * aDiag,
                                           zldDiagElement * aDiagRec,
                                           stlErrorStack  * aErrorStack );

struct zlcDbc
{
    stlRingHead               mStmtRing;
    stlRingEntry              mLink;
    stlRingEntry              mDbcNameLink;   /**< Used in XA connection */
    stlChar                 * mDbcName;       /**< Used in XA connection */
    zlnEnv                  * mParentEnv;
    stlInt64                  mSessionSeq;
    zldDiag                   mDiag;
    stlChar                 * mServerName;
    stlAllocator              mAllocator;
    stlInt32                  mRmId;          /**< Used in XA connection */
    stlUInt32                 mSessionId;
    zlcProtocolType           mProtocolType;
    stlInt32                  mAttrAccessMode;
    stlChar                 * mAttrOldPwd; 
    stlInterval               mAttrLoginTimeout;
    stlUInt32                 mAttrTxnIsolation;
    stlInt32                  mStmtPreFetchSize;
    dtlStringLengthUnit       mStmtStringLengthUnit;
    stlInt16                  mType;
    stlInt16                  mTransition;
    stlInt32                  mTimezone;
    stlInt32                  mDBTimezone;
    stlBool                   mHasWithoutHoldCursor;
    stlBool                   mAttrAutoCommit;
    stlBool                   mBrokenConnection;
    stlBool                   mMetadataId;
    SQLULEN                   mStmtAttrMaxRows;
    stlChar                 * mDateFormat;
    stlInt64                * mDateFormatInfo;
    stlChar                 * mTimeFormat;
    stlInt64                * mTimeFormatInfo;
    stlChar                 * mTimeWithTimezoneFormat;
    stlInt64                * mTimeWithTimezoneFormatInfo;
    stlChar                 * mTimestampFormat;
    stlInt64                * mTimestampFormatInfo;
    stlChar                 * mTimestampWithTimezoneFormat;
    stlInt64                * mTimestampWithTimezoneFormatInfo;
    dtlCharacterSet           mCharacterSet;
    dtlCharacterSet           mNlsCharacterSet;
    dtlCharacterSet           mNlsNCharCharacterSet;
    zlvConvertSQLFunc         mConvertSQLFunc;
    zlvConvertResultSetFunc   mConvertResultFunc;
    zlvConvertErrorFunc       mConvertErrorFunc;
    stlThreadHandle           mThread;
    dtlFuncVector             mDTVector;      /**< DT attr 획득을 위한 Function Vector */
    dtlFuncVector             mNlsDTVector;   /**< Server의 DT attr 획득을 위한 Function Vector */
    cmlHandle                 mComm;
    cmlProtocolSentence       mProtocolSentence;
    cmlMemoryManager          mMemoryMgr;
    stlContext                mContext;
};

#define ZLR_ROWCOUNT_UNKNOWN -1

#define ZLR_ROWSET_BEFORE_START STL_INT64_MIN
#define ZLR_ROWSET_AFTER_END    STL_INT64_MAX

/**
 * @todo 나중에 multiple resultsets 를 지원하려면 ring을 추가해야 한다.
 */
typedef struct zlrResult
{
    dtlValueBlockList * mTargetBuffer;
    stlAllocator        mAllocator;
    stlInt64            mRowCount;
    stlInt64            mPosition;          /* SQLSetPos를 통해 설정된 position */
    stlInt64            mBlockCurrentRow;   /* rowset에서 현재 row의 position */
    stlInt64            mBlockLastRow;      /* rowset에서 마지막 row의 position */
    stlInt64            mCurrentRowIdx;     /* 현재 row의 index */
    stlInt64            mRowsetFirstRowIdx; /* 서버로부터 받은 rowset의 첫 row index */
    stlInt64            mLastRowIdx;        /* result의 마지막 row index */
    stlInt8             mStatus;
    stlBool             mIsEndOfScan;
} zlrResult;

#define ZLR_INIT_RESULT( aArd, aResult )                                    \
{                                                                           \
    zldDescElement * __sArdRec;                                             \
                                                                            \
    STL_RING_FOREACH_ENTRY( &(aArd)->mDescRecRing, __sArdRec )              \
    {                                                                       \
        __sArdRec->mSqlToCFunc = zlvInvalidSqlToC;                          \
    }                                                                       \
                                                                            \
    (aResult)->mRowCount          = ZLR_ROWCOUNT_UNKNOWN;                   \
    (aResult)->mPosition          = 0;                                      \
    (aResult)->mBlockCurrentRow   = ZLR_ROWSET_BEFORE_START;                \
    (aResult)->mBlockLastRow      = ZLR_ROWSET_BEFORE_START;                \
    (aResult)->mCurrentRowIdx     = ZLR_ROWSET_BEFORE_START;                \
    (aResult)->mRowsetFirstRowIdx = 0;                                      \
    (aResult)->mLastRowIdx        = -1;                                     \
    (aResult)->mStatus            = ZLL_CURSOR_ROW_STATUS_NO_CHANGE;        \
    (aResult)->mIsEndOfScan       = STL_FALSE;                              \
}

#define ZLR_INIT_RESULT_WITH_BUFFER( aArd, aResult )                        \
{                                                                           \
    zldDescElement * __sArdRec;                                             \
                                                                            \
    STL_RING_FOREACH_ENTRY( &(aArd)->mDescRecRing, __sArdRec )              \
    {                                                                       \
        __sArdRec->mSqlToCFunc = zlvInvalidSqlToC;                          \
    }                                                                       \
                                                                            \
    (aResult)->mTargetBuffer      = NULL;                                   \
    (aResult)->mRowCount          = ZLR_ROWCOUNT_UNKNOWN;                   \
    (aResult)->mPosition          = 0;                                      \
    (aResult)->mBlockCurrentRow   = ZLR_ROWSET_BEFORE_START;                \
    (aResult)->mBlockLastRow      = ZLR_ROWSET_BEFORE_START;                \
    (aResult)->mCurrentRowIdx     = ZLR_ROWSET_BEFORE_START;                \
    (aResult)->mRowsetFirstRowIdx = 0;                                      \
    (aResult)->mLastRowIdx        = -1;                                     \
    (aResult)->mStatus            = ZLL_CURSOR_ROW_STATUS_NO_CHANGE;        \
    (aResult)->mIsEndOfScan       = STL_FALSE;                              \
}

/*
 * Stmt
 */

#define ZLS_DEFAULT_BIND_CONTEXT_COUNT (16)
#define ZLS_INVALID_STATEMENT_ID       (-1)
#define ZLS_CONCURRENCY_NA             (0)
#define ZLS_HOLDABILITY_NA             (-1)
#define ZLS_PARAM_COUNT_UNKNOWN        (-1)

#define ZLS_DEFAULT_PREFETCH_SIZE      (1000)

struct zlsStmt
{
    stlRingEntry        mLink;
    zlcDbc            * mParentDbc;
    stlChar           * mSQLBuffer;
    stlInt64            mSQLBufferSize;
    stlInt64            mStmtId;
    zldDiag             mDiag;
    zlrResult           mResult;
    stlAllocator        mAllocator;    
    zldDesc             mImpArd;
    zldDesc             mImpApd;
    zldDesc           * mArd;
    zldDesc           * mApd;
    zldDesc             mIrd;
    zldDesc             mIpd;
    zldDescElement    * mCurrentPutDataApdRec;
    zldDescElement    * mCurrentPutDataIpdRec;
    SQLULEN             mCurrentPutDataArrayIdx;
    SQLULEN             mRowsetSize;
    zldDescElement      mGetDataElement;
    SQLLEN              mGetDataOffset;
    SQLLEN              mRowCount;
    stlInt16            mParamCount;
    stlInt16            mOutParamCount;
    stlInt16            mType;
    stlInt16            mTransition;
    stlUInt16           mGetDataIdx;
    stlUInt16           mExplainPlanType;
    stlUInt16           mAtomicExecution;
    stlBool             mHasParamIN;
    stlBool             mHasParamOUT;
    stlInt32            mPreFetchSize;
    stlInt32            mReadBlockCount4Imp;
    stlInt64            mRowsetFirstRowIdx;
    stlInt64            mLastRowIdx;
    SQLULEN             mUsedRowsetSize;
    stlBool             mSetCursorName;
    stlBool             mParamChanged;
    stlBool             mNeedResultDescriptor;
    stlBool             mIsPrepared;
    stlBool             mMetadataId;
    stlInt8             mCursorConcurrency;
    stlInt8             mCursorScrollable;
    stlInt8             mCursorSensitivity;
    stlInt8             mCursorType;
    stlInt8             mCursorHoldable;
    stlInt8             mCursorConcurrencyOrg;
    stlInt8             mCursorSensitivityOrg;
    stlInt8             mCursorHoldableOrg;
    stlInt8             mPreparedCursorConcurrency;
    stlInt8             mPreparedCursorSensitivity;
    stlInt8             mPreparedCursorHoldable;
    stlInt32            mCurrentPutDataIdx;
    dtlStringLengthUnit mStringLengthUnit;
    stlBool             mRetrieveData;
    stlInt16            mNeedDataTransition;
    stlChar             mCursorName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLULEN             mMaxRows;
    stlDynamicAllocator mLongVaryingMem;
};

#define ZLS_INIT_DATA_AT_EXEC(aStmt)                                        \
{                                                                           \
    (aStmt)->mCurrentPutDataApdRec   = NULL;                                \
    (aStmt)->mCurrentPutDataIpdRec   = NULL;                                \
    (aStmt)->mCurrentPutDataIdx      = 0;                                   \
    (aStmt)->mCurrentPutDataArrayIdx = 0;                                   \
    (aStmt)->mNeedDataTransition     = ZLS_TRANSITION_STATE_S0;             \
}

#define ZLS_INIT_GET_DATA_REC(aStmt, aIndex)                                            \
    (aStmt)->mGetDataIdx    = (aIndex);                                                 \
    (aStmt)->mGetDataOffset = 0;                                                        \
    stlMemset( &((aStmt)->mGetDataElement), 0x00, STL_SIZEOF( zldDescElement ) );       \

#define ZLS_STMT_DT_VECTOR(aStmt)       &((aStmt)->mParentDbc->mDTVector)
#define ZLS_STMT_NLS_DT_VECTOR(aStmt)   &((aStmt)->mParentDbc->mNlsDTVector)
#define ZLS_STMT_DBC(aStmt)              ((aStmt)->mParentDbc)

#define ZLS_LONG_VARYING_MEMORY_INIT_SIZE (16*1024)
#define ZLS_LONG_VARYING_MEMORY_NEXT_SIZE (16*1024)

#endif /* _ZL_DEF_H_ */
