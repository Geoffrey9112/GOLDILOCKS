/*******************************************************************************
 * qlgFixedTable.c
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

#ifndef _QLG_FIXED_TABLE_API_H_
#define _QLG_FIXED_TABLE_API_H_ 1

/**
 * @file qlgFixedTable.c
 * @brief SQL layer 의 Fixed Table
 */

#define QLG_STATE_STRING_SIZE     (30)
#define QLG_STMT_TYPE_NAME_SIZE   (128)

stlStatus qlgRegisterFixedTables( qllEnv * aEnv );


/**********************************************************
 * X$SQL_SYSTEM_INFO
 **********************************************************/

#define QLG_SQL_SYSTEM_INFO_ROW_COUNT  (9)


/**
 * @brief X$SQL_SYSTEM_INFO 의 레코드를 구성하기 위한 경로 관리 정보
 * @remarks
 */
typedef struct qlgFxSqlSystemInfoPathCtrl
{
    stlInt32      mIterator;
    stlInt64      mValues[QLG_SQL_SYSTEM_INFO_ROW_COUNT];
} qlgFxSqlSystemInfoPathCtrl;

stlStatus openFxQlgSystemInfoCallback( void   * aStmt,
                                       void   * aDumpObjHandle,
                                       void   * aPathCtrl,
                                       knlEnv * aEnv );

stlStatus closeFxQlgSystemInfoCallback( void   * aDumpObjHandle,
                                        void   * aPathCtrl,
                                        knlEnv * aEnv );

stlStatus buildRecordFxQlgSystemInfoCallback( void              * aDumpObjHandle,
                                              void              * aPathCtrl,
                                              knlValueBlockList * aValueList,
                                              stlInt32            aBlockIdx,
                                              stlBool           * aTupleExist,
                                              knlEnv            * aEnv );

/**
 * @brief X$SQL_SYSTEM_INFO 의 Table Definition
 */

extern knlFxTableDesc gQlgFxSqlSystemInfoTabDesc;

/**********************************************************
 * X$SQL_PROC_ENV
 **********************************************************/

#define QLG_SQL_PROC_ENV_ROW_COUNT  (13)

/**
 * @brief X$SQL_PROC_ENV 의 레코드를 구성하기 위한 경로 관리 정보
 * @remarks
 */
typedef struct qlgFxSqlProcEnvPathCtrl
{
    void        * mCurrEnv;
    stlUInt32     mProcId;
    stlInt32      mIterator;
    stlInt64      mValues[QLG_SQL_PROC_ENV_ROW_COUNT];
} qlgFxSqlProcEnvPathCtrl;


stlStatus openFxQlgProcEnvCallback( void   * aStmt,
                                    void   * aDumpObjHandle,
                                    void   * aPathCtrl,
                                    knlEnv * aEnv );

stlStatus closeFxQlgProcEnvCallback( void   * aDumpObjHandle,
                                     void   * aPathCtrl,
                                     knlEnv * aEnv );

stlStatus buildRecordFxQlgProcEnvCallback( void              * aDumpObjHandle,
                                           void              * aPathCtrl,
                                           knlValueBlockList * aValueList,
                                           stlInt32            aBlockIdx,
                                           stlBool           * aTupleExist,
                                           knlEnv            * aEnv );

/**
 * @brief X$SQL_PROC_ENV 의 Table Definition
 */

extern knlFxTableDesc gQlgFxSqlProcEnvTabDesc;


/**********************************************************
 * X$SQL_SESS_ENV
 **********************************************************/

#define QLG_SQL_SESS_ENV_ROW_COUNT  (15)

/**
 * @brief X$SQL_SESS_ENV 의 레코드를 구성하기 위한 경로 관리 정보
 * @remarks
 */
typedef struct qlgFxSqlSessEnvPathCtrl
{
    knlSessionEnv * mCurrEnv;
    stlUInt32       mSessId;
    stlInt32        mIterator;
    stlInt64        mValues[QLG_SQL_SESS_ENV_ROW_COUNT];
} qlgFxSqlSessEnvPathCtrl;

stlStatus openFxQlgSessEnvCallback( void   * aStmt,
                                    void   * aDumpObjHandle,
                                    void   * aPathCtrl,
                                    knlEnv * aEnv );

stlStatus closeFxQlgSessEnvCallback( void   * aDumpObjHandle,
                                     void   * aPathCtrl,
                                     knlEnv * aEnv );

stlStatus buildRecordFxQlgSessEnvCallback( void              * aDumpObjHandle,
                                           void              * aPathCtrl,
                                           knlValueBlockList * aValueList,
                                           stlInt32            aBlockIdx,
                                           stlBool           * aTupleExist,
                                           knlEnv            * aEnv );


/**
 * @brief X$SQL_SESS_ENV 의 Table Definition
 */

extern knlFxTableDesc gQlgFxSqlSessEnvTabDesc;


/**********************************************************
 * X$SQL_SYSTEM_STAT_EXEC_STMT
 **********************************************************/


extern knlFxTableDesc gQlgSystExecTabDesc;

stlStatus openFxQlgSystExecCallback( void   * aStmt,
                                     void   * aDumpObjHandle,
                                     void   * aPathCtrl,
                                     knlEnv * aEnv );

stlStatus closeFxQlgSystExecCallback( void   * aDumpObjHandle,
                                      void   * aPathCtrl,
                                      knlEnv * aEnv );

stlStatus buildRecordFxQlgSystExecCallback( void              * aDumpObjHandle,
                                            void              * aPathCtrl,
                                            knlValueBlockList * aValueList,
                                            stlInt32            aBlockIdx,
                                            stlBool           * aTupleExist,
                                            knlEnv            * aEnv );


/**
 * @brief X$SQL_SYSTEM_STAT_EXEC_STMT 에서 하나의 레코드
 */
typedef struct qlgSystExecFxRecord
{
    stlChar         * mStmtTypeName;        /**< SQL Stmt Type Name */
    stlInt32          mStmtTypeID;          /**< SQL Stmt Type ID */
    stlInt64          mStmtExecCnt;         /**< SQL Stmt Execute Count */

} qlgSystExecFxRecord;

/**
 * @brief X$SQL_SYST_EXEC_STMT 의 Path Controller
 */
typedef struct qlgSystExecPathCtrl
{
    stlInt32   mStmtTypeID;        /**< SQL Stmt Type ID */
    stlInt64   mStmtExecCnt;       /**< SQL Execution Count */
} qlgSystExecPathCtrl;





/**********************************************************
 * X$SQL_PROC_STAT_EXEC_STMT
 **********************************************************/


extern knlFxTableDesc gQlgProcExecTabDesc;

stlStatus openFxQlgProcExecCallback( void   * aStmt,
                                     void   * aDumpObjHandle,
                                     void   * aPathCtrl,
                                     knlEnv * aEnv );

stlStatus closeFxQlgProcExecCallback( void   * aDumpObjHandle,
                                      void   * aPathCtrl,
                                      knlEnv * aEnv );

stlStatus buildRecordFxQlgProcExecCallback( void              * aDumpObjHandle,
                                            void              * aPathCtrl,
                                            knlValueBlockList * aValueList,
                                            stlInt32            aBlockIdx,
                                            stlBool           * aTupleExist,
                                            knlEnv            * aEnv );


/**
 * @brief X$SQL_PROC_STAT_EXEC_STMT 에서 하나의 레코드
 */
typedef struct qlgProcExecFxRecord
{
    stlUInt32 mID;                               /**< 해당 Environment의 ID */
    stlChar   mState[QLG_STATE_STRING_SIZE+1];   /**< 해당 Environment 사용 여부 */
    
    stlChar         * mStmtTypeName;        /**< SQL Stmt Type Name */
    stlInt32          mStmtTypeID;          /**< SQL Stmt Type ID */
    stlInt64          mStmtExecCnt;         /**< SQL Stmt Execute Count */

} qlgProcExecFxRecord;


/**
 * @brief X$SQL_PROC_STAT_EXEC_STMT 의 Path Controller
 */
typedef struct qlgProcExecPathCtrl
{
    void *     mProcEnv;           /**< 읽어야 할 Process Environment 주소 */
    stlInt32   mStmtTypeID;        /**< SQL Stmt Type ID */
} qlgProcExecPathCtrl;




/**********************************************************
 * X$SQL_SESS_STAT_EXEC_STMT
 **********************************************************/


extern knlFxTableDesc gQlgSessExecTabDesc;

stlStatus openFxQlgSessExecCallback( void   * aStmt,
                                     void   * aDumpObjHandle,
                                     void   * aPathCtrl,
                                     knlEnv * aEnv );

stlStatus closeFxQlgSessExecCallback( void   * aDumpObjHandle,
                                      void   * aPathCtrl,
                                      knlEnv * aEnv );

stlStatus buildRecordFxQlgSessExecCallback( void              * aDumpObjHandle,
                                            void              * aPathCtrl,
                                            knlValueBlockList * aValueList,
                                            stlInt32            aBlockIdx,
                                            stlBool           * aTupleExist,
                                            knlEnv            * aEnv );


/**
 * @brief X$SQL_SESS_STAT_EXEC_STMT 에서 하나의 레코드
 */
typedef struct qlgSessExecFxRecord
{
    stlUInt32 mID;                               /**< 해당 Environment의 ID */
    stlChar   mState[QLG_STATE_STRING_SIZE+1];   /**< 해당 Environment 사용 여부 */
    
    stlChar         * mStmtTypeName;        /**< SQL Stmt Type Name */
    stlInt32          mStmtTypeID;          /**< SQL Stmt Type ID */
    stlInt64          mStmtExecCnt;         /**< SQL Stmt Execute Count */

} qlgSessExecFxRecord;


/**
 * @brief X$SQL_SESS_STAT_EXEC_STMT 의 Path Controller
 */
typedef struct qlgSessExecPathCtrl
{
    void *     mSessEnv;           /**< 읽어야 할 Session Environment 주소 */
    stlInt32   mStmtTypeID;        /**< SQL Stmt Type ID */
} qlgSessExecPathCtrl;




/**********************************************************
 * X$SQL_KEYWORDS
 **********************************************************/


extern knlFxTableDesc gQlgKeywordTabDesc;
stlStatus openFxQlgKeywordCallback( void   * aStmt,
                                    void   * aDumpObjHandle,
                                    void   * aPathCtrl,
                                    knlEnv * aEnv );

stlStatus closeFxQlgKeywordCallback( void   * aDumpObjHandle,
                                     void   * aPathCtrl,
                                     knlEnv * aEnv );

stlStatus buildRecordFxQlgKeywordCallback( void              * aDumpObjHandle,
                                           void              * aPathCtrl,
                                           knlValueBlockList * aValueList,
                                           stlInt32            aBlockIdx,
                                           stlBool           * aTupleExist,
                                           knlEnv            * aEnv );


/**
 * @brief X$SQL_KEYWORDS 에서 하나의 레코드
 */
typedef struct qlgKeywordFxRecord
{
    const stlChar   * mName;            /**< SQL Keyword */
    stlInt32          mCategory;        /**< SQL Keyword Category */

} qlgKeywordFxRecord;


/**
 * @brief X$SQL_KEYWORDS 의 Path Controller
 */
typedef struct qlgKeywordPathCtrl
{
    stlInt32  mCurrKeyIdx;                /**< Current SQL Keyword */
} qlgKeywordPathCtrl;




/**********************************************************
 * X$BUILTIN_AGGREAGTION
 **********************************************************/


/**
 * @brief X$BUILTIN_AGGREGATION 을 구성하기 위한 자료구조
 * @remarks
 */
typedef struct qlgFixedBuiltInAggr
{
    stlInt32    mFuncID;             /**< Function ID */
    stlChar   * mFuncName;           /**< Function Name */
    stlInt32    mMinArgCnt;          /**< Minimum Argument Count */
    stlInt32    mMaxArgCnt;          /**< Maximum Argument Count */
} qlgFixedBuiltInAggr;


/**
 * @brief X$BUILTIN_AGGREGATION 를 위한 Column Definition
 */
extern knlFxColumnDesc gQlgFixedBuiltInAggrColDesc[];

/**
 * @brief X$BUILTIN_AGGREGATION 의 레코드를 구성하기 위한 경로 관리 정보
 * @remarks
 */
typedef struct qlgFixedBuiltInAggrPathCtrl
{
    stlInt32    mNO;
} qlgFixedBuiltInAggrPathCtrl;


stlStatus qlgOpenFixedBuiltInAggrCallback( void   * aStmt,
                                           void   * aDumpObjHandle,
                                           void   * aPathCtrl,
                                           knlEnv * aKnlEnv );

stlStatus qlgCloseFixedBuiltInAggrCallback( void   * aDumpObjHandle,
                                            void   * aPathCtrl,
                                            knlEnv * aKnlEnv );

stlStatus qlgBuildRecordBuiltInAggrCallback( void              * aDumpObjHandle,
                                             void              * aPathCtrl,
                                             knlValueBlockList * aValueList,
                                             stlInt32            aBlockIdx,
                                             stlBool           * aIsExist,
                                             knlEnv            * aKnlEnv );

/**
 * @brief X$BUILTIN_AGGREGATION 의 Table Definition
 */

extern knlFxTableDesc gQlgFixedBuiltInAggrTabDesc;



/**********************************************************
 * X$FIXED_VIEW
 **********************************************************/


/**
 * @brief X$FIXED_VIEW 을 구성하기 위한 자료구조
 * @remarks
 */
typedef struct qlgFixedView
{
    stlChar   * mViewName;           /**< Function Name */
    stlChar   * mStartupPhase;       /**< Startup Phase */
} qlgFixedView;


/**
 * @brief X$FIXED_VIEW 를 위한 Column Definition
 */
extern knlFxColumnDesc gQlgFixedViewColDesc[];

/**
 * @brief X$FIXED_VIEW 의 레코드를 구성하기 위한 경로 관리 정보
 * @remarks
 */
typedef struct qlgFixedViewPathCtrl
{
    knlStartupPhase  mStartupPhase;
    stlInt32         mNO;
} qlgFixedViewPathCtrl;


stlStatus qlgOpenFixedViewCallback( void   * aStmt,
                                    void   * aDumpObjHandle,
                                    void   * aPathCtrl,
                                    knlEnv * aKnlEnv );

stlStatus qlgCloseFixedViewCallback( void   * aDumpObjHandle,
                                     void   * aPathCtrl,
                                     knlEnv * aKnlEnv );

stlStatus qlgBuildRecordFixedViewCallback( void              * aDumpObjHandle,
                                           void              * aPathCtrl,
                                           knlValueBlockList * aValueList,
                                           stlInt32            aBlockIdx,
                                           stlBool           * aIsExist,
                                           knlEnv            * aKnlEnv );

/**
 * @brief X$FIXED_VIEW 의 Table Definition
 */

extern knlFxTableDesc gQlgFixedViewTabDesc;


/**********************************************************
 * X$SQL_CACHE_PLAN
 **********************************************************/

/**
 * @brief X$SQL_CACHE_PLAN 을 구성하기 위한 자료구조
 * @remarks
 */
typedef struct qlgSqlCachePlan
{
    void         * mSqlHandle;
    stlInt64       mRefCount;
    stlInt32       mPlanIdx;
    stlInt64       mPlanSize;
    stlBool        mIsAtomic;
    stlBool        mHasHintError;
    stlInt32       mBindCount;
    stlChar      * mPlanString;
} qlgSqlCachePlan;


/**
 * @brief X$SQL_CACHE_PLAN 를 위한 Column Definition
 */
extern knlFxColumnDesc gQlgSqlCachePlanColDesc[];

/**
 * @brief X$SQL_CACHE_PLAN 의 레코드를 구성하기 위한 경로 관리 정보
 * @remarks
 */
typedef struct qlgSqlCachePlanPathCtrl
{
    stlBool                    mSetFlangeIterator;
    stlBool                    mSetPlanIterator;
    stlBool                    mSetMemMark;

    knlRegionMark              mRegionMemMark;
    
    knlFlangeIterator          mFlangeIterator;
    knlSqlHandle             * mSqlHandle;
    stlInt64                   mSqlHandleRefCount;
    knlPlanIterator            mPlanIterator;
} qlgSqlCachePlanPathCtrl;


stlStatus qlgOpenSqlCachePlanCallback( void   * aStmt,
                                       void   * aDumpObjHandle,
                                       void   * aPathCtrl,
                                       knlEnv * aKnlEnv );

stlStatus qlgCloseSqlCachePlanCallback( void   * aDumpObjHandle,
                                        void   * aPathCtrl,
                                        knlEnv * aKnlEnv );

stlStatus qlgBuildRecordSqlCachePlanCallback( void              * aDumpObjHandle,
                                              void              * aPathCtrl,
                                              knlValueBlockList * aValueList,
                                              stlInt32            aBlockIdx,
                                              stlBool           * aIsExist,
                                              knlEnv            * aKnlEnv );

stlStatus qlgExplainPlanForPlanCache( qllNodeType            aStmtType,
                                      qlvInitPlan          * aInitPlan,
                                      qllOptimizationNode  * aCodePlan,
                                      stlInt32               aBindCount,
                                      knlRegionMem         * aRegionMem,
                                      stlChar             ** aExplainPlanText,
                                      knlEnv               * aKnlEnv );

/**
 * @brief X$SQL_CACHE_PLAN 의 Table Definition
 */

extern knlFxTableDesc gQlgSqlCachePlanTabDesc;


#endif /* _QLG_FIXED_TABLE_API_H_ */

