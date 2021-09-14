/*******************************************************************************
 * qloOptimization.h
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

#ifndef _QLOOPTIMIZATION_H_
#define _QLOOPTIMIZATION_H_ 1


/**
 * @file qloOptimization.h
 * @brief SQL Processor Layer Optimization 
 */

#include <qlDef.h>


/**
 * @addtogroup qlo
 * @{
 */

typedef stlStatus (*qloDataOptimizeExprFuncPtr) ( qloDataOptExprInfo  * aDataOptInfo,
                                                  qlvInitExpression   * aInitExpr,
                                                  qllEnv              * aEnv );

extern const qloDataOptimizeExprFuncPtr qloDataOptExprFuncList[ QLV_EXPR_TYPE_MAX ];


/**
 * @brief statement expression list
 */
struct qloInitStmtExprList
{
    qlvInitStmtExprList  * mInitExprList;        /**< Validation 과정에서 구축된 Statement 단위 Expression List */
    
    qlvRefExprList       * mAllExprList;         /**< 모든 Expression List */
    qlvRefExprList       * mColumnExprList;      /**< Column Column 목록 */
    qlvRefExprList       * mRowIdColumnExprList; /**< RowId Pseudo Column 목록 */    

    knlExprStack         * mConstExprStack;      /**< constant expression stack */

    qloExprInfo          * mTotalExprInfo;       /**< total expression info : array */
};


/**
 * @brief expression list
 */
struct qloInitExprList
{
    qloInitStmtExprList  * mStmtExprList;        /**< Statement 단위 Expression List */

    qlvInitExprList      * mInitExprList;        /**< Validation 과정에서 구축된 Query 단위 Expression List */
};


/**
 * @brief Data Optimization Info For Expression
 */
struct qloDataOptExprInfo
{
    qloInitStmtExprList    * mStmtExprList;
    qllDataArea            * mDataArea;
    qllStatement           * mSQLStmt;
    qlvInitNode            * mWriteRelationNode;
    knlBindContext         * mBindContext;
    knlRegionMem           * mRegionMem;
};


/**
 * @brief 각 Expression에 대한 type 정보 : Execution Code Area
 */
struct qloDBType
{
    /* type 정보 */
    stlUInt8             mDBType;                  /**< DB Type (cast : target type) : dtlDataType */
    stlUInt8             mSourceDBType;            /**< Source DB Type : dtlDataType */

    /* cast 정보 */
    stlBool              mIsNeedCast;              /**< Cast 필요 여부 */
    stlUInt8             mBlank[5];
    stlUInt32            mCastFuncIdx;             /**< Cast Function Idx : stlUInt32 */

    stlUInt8             mStringLengthUnit;        /**< only for STRING 계열 Type : dtlStringLengthUnit */
    stlUInt8             mIntervalIndicator;       /**< only for INTERVAL Type : dtlIntervalIndicator */
    stlUInt8             mSourceStringLengthUnit;  /**< Source DB Type의 STRING 계열 Type : dtlStringLengthUnit */
    stlUInt8             mSourceIntervalIndicator; /**< Source DB Type의 INTERVAL Type : dtlIntervalIndicator */

    stlInt32             mArgNum1;                 /**< 첫번째 숫자 (TYPE 마다 용도가 다름) : stlInt64 */
    stlInt32             mArgNum2;                 /**< 두번째 숫자 (TYPE 마다 용도가 다름) : stlInt64 */
    stlInt32             mSourceArgNum1;           /**< Source DB Type의 첫번째 숫자 : stlInt64 */
    stlInt32             mSourceArgNum2;           /**< Source DB Type의 두번째 숫자 : stlInt64 */
};


/**
 * @brief Function Expression의 정보 : Execution Code Area
 */
struct qloFunctionInfo
{
    knlBuiltInFunc          mFuncID;           /**< function ID */
    stlUInt32               mFuncIdx;          /**< function Idx */
    stlUInt32               mResultBufferSize; /**< fuction result buffer의 크기 */
    dtlFuncReturnTypeClass  mReturnType;       /**< return value type class */
};


/**
 * @brief List Function Stack Info
 */
struct qloListFuncInfo
{
    knlListFuncType    mListFuncType;
    stlUInt16          mValueCount;
    stlChar            mPadding[2];
    knlListStack     * mListStack;
};


/**
 * @brief 모든 Expression에 대한 정보 : Execution Code Area
 * 
 * @remark knlExprContext(Data Area)를 구성하기 위한 모든 정보를 수록
 */
struct qloExprInfo
{
    stlInt32            mExprCnt;        /**< expression 수 */
    qloDBType         * mExprDBType;     /**< 각 expression's DB type : array */
    qloFunctionInfo   * mExprFunc;       /**< 각 function expression 구성 정보 */
    qloListFuncInfo   * mListFunc;       /**< 각 list func 의 list column stack 구성 정보 */   
};


/**
 * @brief Output Parameter 를 위한 Data 영역
 */
struct qloDataOutParam
{
    knlValueBlockList   * mOutBlock;      /**< Output Paramter Value Block */
    knlCastFuncInfo       mAssignInfo;      /**< Assign Information */
};


/**
 * @brief Table을 기준으로 Column Value Block List 관리
 */
struct qloColumnBlockItem
{
    qlvInitNode         * mRelationNode;
    knlValueBlockList   * mColumnValueBlock;
    qloColumnBlockItem  * mNextRelation;
};

/**
 * @brief Table을 기준으로 Column Value Block List 관리
 */
struct qloColumnBlockList
{
    qloColumnBlockItem  * mHead;
};


/**
 * @brief Table Stat Info Item
 */
struct qloValidTblStatItem
{
    ellDictHandle       * mTableHandle;
    void                * mTablePhyHandle;
    stlFloat64            mPageCount;
    qloValidTblStatItem * mNext;
};


/**
 * @ brief Table Stat Info List
 */
struct qloValidTblStatList
{
    stlInt32              mCount;
    qloValidTblStatItem * mHead;
    qloValidTblStatItem * mTail;
};


/**
 * @brief Code Optimize의 모든 함수에서 공통으로 사용하는 Parameter 변수 관리
 */
struct qloCodeOptParamInfo
{
    qllDBCStmt              * mDBCStmt;         /**< DBC Statement */
    qllStatement            * mSQLStmt;         /**< SQL Statement */
    qloInitExprList         * mQueryExprList;   /**< Query Expression List */
    qllOptNodeList          * mOptNodeList;     /**< Optimization Node List */
    qllEstimateCost         * mOptCost;         /**< Optimization Cost */
    qllOptimizationNode     * mOptQueryNode;    /**< Optimization Query Node */
    qlncPlanCommon          * mCandPlan;        /**< Candidate Plan (Input) */
    qllOptimizationNode     * mOptNode;         /**< Optimization Node (Output) */
    stlInt32                  mRootQuerySetIdx; /**< Root Query Set Index (for Query Set Only) */
    void                    * mRootQuerySet;    /**< Root Query Set Node (for Query Set Only) */
};


#define QLO_CODE_OPT_DEFAULT_PARAM_VALIDATE( _aCodeOptParamInfo, _aEnv )                            \
    {                                                                                               \
        STL_PARAM_VALIDATE( (_aCodeOptParamInfo) != NULL, QLL_ERROR_STACK(_aEnv) );                 \
        STL_PARAM_VALIDATE( (_aCodeOptParamInfo)->mDBCStmt != NULL, QLL_ERROR_STACK(_aEnv) );       \
        STL_PARAM_VALIDATE( (_aCodeOptParamInfo)->mSQLStmt != NULL, QLL_ERROR_STACK(_aEnv) );       \
        STL_PARAM_VALIDATE( (_aCodeOptParamInfo)->mQueryExprList != NULL, QLL_ERROR_STACK(_aEnv) ); \
        STL_PARAM_VALIDATE( (_aCodeOptParamInfo)->mOptNodeList != NULL, QLL_ERROR_STACK(_aEnv) );   \
        STL_PARAM_VALIDATE( (_aCodeOptParamInfo)->mOptCost != NULL, QLL_ERROR_STACK(_aEnv) );       \
        STL_PARAM_VALIDATE( (_aCodeOptParamInfo)->mCandPlan != NULL, QLL_ERROR_STACK(_aEnv) );      \
    }


#define QLO_GET_DB_TYPE( _aDataOptInfo, _aOffset )                                      \
    ( & (_aDataOptInfo)->mStmtExprList->mTotalExprInfo->mExprDBType[ (_aOffset) ] )

#define QLO_GET_CONTEXT( _aDataOptInfo, _aOffset )                              \
    ( & (_aDataOptInfo)->mDataArea->mExprDataContext->mContexts[ (_aOffset) ] )


/*******************************************************************************
 * Add Expression Functions
 ******************************************************************************/

stlStatus qloAddExpr( qlvInitExpression   * aInitExpr,
                      qlvRefExprList      * aRefExprList,
                      knlRegionMem        * aRegionMem,
                      qllEnv              * aEnv );

typedef stlStatus (*qloAddExprFuncPtr) ( qlvInitExpression   * aInitExpr,
                                         qlvRefExprList      * aRefExprList,
                                         knlRegionMem        * aRegionMem,
                                         qllEnv              * aEnv );

stlStatus qloAddExprValue( qlvInitExpression   * aInitExpr,
                           qlvRefExprList      * aRefExprList,
                           knlRegionMem        * aRegionMem,
                           qllEnv              * aEnv );

stlStatus qloAddExprBindParam( qlvInitExpression   * aInitExpr,
                               qlvRefExprList      * aRefExprList,
                               knlRegionMem        * aRegionMem,
                               qllEnv              * aEnv );

stlStatus qloAddExprColumn( qlvInitExpression   * aInitExpr,
                            qlvRefExprList      * aRefExprList,
                            knlRegionMem        * aRegionMem,
                            qllEnv              * aEnv );

stlStatus qloAddExprFunction( qlvInitExpression   * aInitExpr,
                              qlvRefExprList      * aRefExprList,
                              knlRegionMem        * aRegionMem,
                              qllEnv              * aEnv );

stlStatus qloAddExprCast( qlvInitExpression   * aInitExpr,
                          qlvRefExprList      * aRefExprList,
                          knlRegionMem        * aRegionMem,
                          qllEnv              * aEnv );

stlStatus qloAddExprPseudoColumn( qlvInitExpression   * aInitExpr,
                                  qlvRefExprList      * aRefExprList,
                                  knlRegionMem        * aRegionMem,
                                  qllEnv              * aEnv );

stlStatus qloAddExprPseudoArgs( qlvInitExpression   * aInitExpr,
                                qlvRefExprList      * aRefExprList,
                                knlRegionMem        * aRegionMem,
                                qllEnv              * aEnv );

stlStatus qloAddExprConstExpr( qlvInitExpression   * aInitExpr,
                               qlvRefExprList      * aRefExprList,
                               knlRegionMem        * aRegionMem,
                               qllEnv              * aEnv );

stlStatus qloAddExprReference( qlvInitExpression   * aInitExpr,
                               qlvRefExprList      * aRefExprList,
                               knlRegionMem        * aRegionMem,
                               qllEnv              * aEnv );

stlStatus qloAddExprSubQuery( qlvInitExpression   * aInitExpr,
                              qlvRefExprList      * aRefExprList,
                              knlRegionMem        * aRegionMem,
                              qllEnv              * aEnv );

stlStatus qloAddExprInnerColumn( qlvInitExpression   * aInitExpr,
                                 qlvRefExprList      * aRefExprList,
                                 knlRegionMem        * aRegionMem,
                                 qllEnv              * aEnv );

stlStatus qloAddExprOuterColumn( qlvInitExpression   * aInitExpr,
                                 qlvRefExprList      * aRefExprList,
                                 knlRegionMem        * aRegionMem,
                                 qllEnv              * aEnv );

stlStatus qloAddExprRowIdColumn( qlvInitExpression   * aInitExpr,
                                 qlvRefExprList      * aRefExprList,
                                 knlRegionMem        * aRegionMem,
                                 qllEnv              * aEnv );

stlStatus qloAddExprAggregation( qlvInitExpression   * aInitExpr,
                                 qlvRefExprList      * aRefExprList,
                                 knlRegionMem        * aRegionMem,
                                 qllEnv              * aEnv );

stlStatus qloAddExprCaseExpr( qlvInitExpression   * aInitExpr,
                              qlvRefExprList      * aRefExprList,
                              knlRegionMem        * aRegionMem,
                              qllEnv              * aEnv );

stlStatus qloAddExprListFunction( qlvInitExpression   * aInitExpr,
                                  qlvRefExprList      * aRefExprList,
                                  knlRegionMem        * aRegionMem,
                                  qllEnv              * aEnv );

stlStatus qloAddExprListColumn( qlvInitExpression   * aInitExpr,
                                qlvRefExprList      * aRefExprList,
                                knlRegionMem        * aRegionMem,
                                qllEnv              * aEnv );

stlStatus qloAddExprRowExpr( qlvInitExpression   * aInitExpr,
                             qlvRefExprList      * aRefExprList,
                             knlRegionMem        * aRegionMem,
                             qllEnv              * aEnv );


/** @} qlo */


/*******************************************************************************
 * Code Optimizaion Common Functions
 ******************************************************************************/

/**
 * @defgroup qlocCommon Code Optimizaion Common Functions
 * @ingroup qloc
 * @{
 */


qloValidTblStatList * qloGetValidTblStatList( qllStatement  * aSQLStmt );

/** @} qlocCommon */


/*******************************************************************************
 * SELECT : Code Optimizaion
 ******************************************************************************/

/**
 * @defgroup qlocSelect SELECT Code Optimization
 * @ingroup qloc
 * @{
 */

stlStatus qloCodeOptimizeSelect( smlTransId     aTransID,
                                 qllDBCStmt   * aDBCStmt,
                                 qllStatement * aSQLStmt,
                                 qllEnv       * aEnv );

stlStatus qloCodeOptimizeSelectInternal( smlTransId     aTransID,
                                         qllDBCStmt   * aDBCStmt,
                                         qllStatement * aSQLStmt,
                                         qllEnv       * aEnv );

/** @} qlocSelect */


/**
 * @defgroup qlocSelectInto SELECT INTO Code Optimization
 * @ingroup qloc
 * @{
 */

stlStatus qloCodeOptimizeSelectInto( smlTransId     aTransID,
                                     qllDBCStmt   * aDBCStmt,
                                     qllStatement * aSQLStmt,
                                     qllEnv       * aEnv );

/** @} qlocSelectInto */


/*******************************************************************************
 * QUERY EXPR : Code Optimizaion
 ******************************************************************************/

/**
 * @defgroup qlocQueryExpr QueryExpr Code Optimization
 * @ingroup qloc
 * @{
 */


/****************************************************************************
 * Node Optimization Functions
 ***************************************************************************/

typedef stlStatus (*qlnoCodeOptNodeFuncPtr) ( qloCodeOptParamInfo   * aCodeOptParamInfo,
                                              qllEnv                * aEnv );

stlStatus qlnoCodeOptimizeQuerySet( qloCodeOptParamInfo * aCodeOptParamInfo,
                                    qllEnv              * aEnv );

stlStatus qlnoCodeOptimizeSetOP( qloCodeOptParamInfo    * aCodeOptParamInfo,
                                 qllEnv                 * aEnv );

stlStatus qlnoCodeOptimizeQuerySpec( qloCodeOptParamInfo    * aCodeOptParamInfo,
                                     qllEnv                 * aEnv );

stlStatus qlnoCodeOptimizeTableAccess( qloCodeOptParamInfo  * aCodeOptParamInfo,
                                       qllEnv               * aEnv );

stlStatus qlnoCodeOptimizeIndexAccess( qloCodeOptParamInfo  * aCodeOptParamInfo,
                                       qllEnv               * aEnv );

stlStatus qlnoCodeOptimizeIndexCombine( qloCodeOptParamInfo * aCodeOptParamInfo,
                                        qllEnv              * aEnv );

stlStatus qlnoCodeOptimizeRowidAccess( qloCodeOptParamInfo  * aCodeOptParamInfo,
                                       qllEnv               * aEnv );

stlStatus qlnoCodeOptimizeSubTable( qloCodeOptParamInfo     * aCodeOptParamInfo,
                                    qllEnv                  * aEnv );

stlStatus qlnoCodeOptimizeSubQueryList( qloCodeOptParamInfo     * aCodeOptParamInfo,
                                        qllEnv                  * aEnv );

stlStatus qlnoCodeOptimizeSubQueryFunc( qloCodeOptParamInfo     * aCodeOptParamInfo,
                                        qllEnv                  * aEnv );

stlStatus qlnoCodeOptimizeSubQueryFilter( qloCodeOptParamInfo     * aCodeOptParamInfo,
                                          qllEnv                  * aEnv );

stlStatus qlnoCodeOptimizeNestedLoops( qloCodeOptParamInfo  * aCodeOptParamInfo,
                                       qllEnv               * aEnv );

stlStatus qlnoCodeOptimizeMerge( qloCodeOptParamInfo    * aCodeOptParamInfo,
                                 qllEnv                 * aEnv );

stlStatus qlnoCodeOptimizeHash( qloCodeOptParamInfo * aCodeOptParamInfo,
                                qllEnv              * aEnv );

stlStatus qlnoCodeOptimizeJoinCombine( qloCodeOptParamInfo  * aCodeOptParamInfo,
                                       qllEnv               * aEnv );

stlStatus qlnoCodeOptimizeSortInstant( qloCodeOptParamInfo  * aCodeOptParamInfo,
                                       qllEnv               * aEnv );

#if 0
stlStatus qlnoCodeOptimizeSortGroupInstant( qloCodeOptParamInfo * aCodeOptParamInfo,
                                            qllEnv              * aEnv );
#endif

stlStatus qlnoCodeOptimizeSortJoinInstant( qloCodeOptParamInfo  * aCodeOptParamInfo,
                                           qllEnv               * aEnv );

#if 0
stlStatus qlnoCodeOptimizeHashInstant( qloCodeOptParamInfo  * aCodeOptParamInfo,
                                       qllEnv               * aEnv );
#endif

stlStatus qlnoCodeOptimizeGroupHashInstant( qloCodeOptParamInfo * aCodeOptParamInfo,
                                            qllEnv              * aEnv );

stlStatus qlnoCodeOptimizeHashJoinInstant( qloCodeOptParamInfo  * aCodeOptParamInfo,
                                           qllEnv               * aEnv );



#if 0
stlStatus qloCodeOptimizeFlatInstantAccess( qllDBCStmt              * aDBCStmt,
                                            qllStatement            * aSQLStmt,
                                            qllOptimizationNode     * aOptQueryNode,
                                            qllOptimizationNode     * aOptChildNode,
                                            qlvInitNode             * aInitNode,
                                            qlvInitExpression       * aWhereExpr,
                                            ellCursorScrollability    aScrollable,
                                            qloInitExprList         * aQueryExprList,
                                            qllOptNodeList          * aOptNodeList,
                                            qllOptimizationNode    ** aOptNode,
                                            qllEstimateCost         * aOptCost,
                                            qllEnv                  * aEnv );
#endif

stlStatus qloCodeOptimizeHashAggregation( qllDBCStmt              * aDBCStmt,
                                          qllStatement            * aSQLStmt,
                                          qllOptimizationNode     * aOptQueryNode,
                                          qllOptimizationNode     * aOptChildNode,
                                          qlvAggregation          * aInitNode,
                                          qloInitExprList         * aQueryExprList,
                                          qllOptNodeList          * aOptNodeList,
                                          qllOptimizationNode    ** aOptNode,
                                          qllEstimateCost         * aOptCost,
                                          qllEnv                  * aEnv );

stlStatus qlnoCodeOptimizeGroup( qloCodeOptParamInfo * aCodeOptParamInfo,
                                 qllEnv              * aEnv );


/****************************************************************************
 * Complete Building Node Functions
 ***************************************************************************/

/* Complete Optimization Node List */
stlStatus qlnoCompleteOptNodes( qllDBCStmt            * aDBCStmt,
                                qllStatement          * aSQLStmt,
                                qllOptNodeList        * aOptNodeList,
                                qloInitExprList       * aQueryExprList,
                                qllEnv                * aEnv );

/* Complete Node Function for Each Node */
typedef stlStatus (*qlnoCompleteNodeFuncPtr) ( qllDBCStmt            * aDBCStmt,
                                               qllStatement          * aSQLStmt,
                                               qllOptNodeList        * aOptNodeList,
                                               qllOptimizationNode   * aOptNode,
                                               qloInitExprList       * aQueryExprList,
                                               qllEnv                * aEnv );

stlStatus qlnoCompleteSelectStmt( qllDBCStmt            * aDBCStmt,
                                  qllStatement          * aSQLStmt,
                                  qllOptNodeList        * aOptNodeList,
                                  qllOptimizationNode   * aOptNode,
                                  qloInitExprList       * aQueryExprList,
                                  qllEnv                * aEnv );

stlStatus qlnoCompleteInsertStmt( qllDBCStmt            * aDBCStmt,
                                  qllStatement          * aSQLStmt,
                                  qllOptNodeList        * aOptNodeList,
                                  qllOptimizationNode   * aOptNode,
                                  qloInitExprList       * aQueryExprList,
                                  qllEnv                * aEnv );

stlStatus qlnoCompleteUpdateStmt( qllDBCStmt            * aDBCStmt,
                                  qllStatement          * aSQLStmt,
                                  qllOptNodeList        * aOptNodeList,
                                  qllOptimizationNode   * aOptNode,
                                  qloInitExprList       * aQueryExprList,
                                  qllEnv                * aEnv );

stlStatus qlnoCompleteDeleteStmt( qllDBCStmt            * aDBCStmt,
                                  qllStatement          * aSQLStmt,
                                  qllOptNodeList        * aOptNodeList,
                                  qllOptimizationNode   * aOptNode,
                                  qloInitExprList       * aQueryExprList,
                                  qllEnv                * aEnv );

// stlStatus qlnoCompleteMergeStmt( qllDBCStmt            * aDBCStmt,
//                                 qllStatement          * aSQLStmt,
//                                 qllOptNodeList        * aOptNodeList,
//                                 qllOptimizationNode   * aOptNode,
//                                 qloInitExprList       * aQueryExprList,
//                                 qllEnv                * aEnv );

// stlStatus qlnoCompleteCraeteAsSelectStmt( qllDBCStmt            * aDBCStmt,
//                                          qllStatement          * aSQLStmt,
//                                          qllOptNodeList        * aOptNodeList,
//                                          qllOptimizationNode   * aOptNode,
//                                          qloInitExprList       * aQueryExprList,
//                                          qllEnv                * aEnv );

// stlStatus qlnoCompleteDDLStmt( qllDBCStmt            * aDBCStmt,
//                               qllStatement          * aSQLStmt,
//                               qllOptNodeList        * aOptNodeList,
//                               qllOptimizationNode   * aOptNode,
//                               qloInitExprList       * aQueryExprList,
//                               qllEnv                * aEnv );

stlStatus qlnoCompleteQuerySpec( qllDBCStmt            * aDBCStmt,
                                 qllStatement          * aSQLStmt,
                                 qllOptNodeList        * aOptNodeList,
                                 qllOptimizationNode   * aOptNode,
                                 qloInitExprList       * aQueryExprList,
                                 qllEnv                * aEnv );

stlStatus qlnoCompleteQuerySet( qllDBCStmt            * aDBCStmt,
                                qllStatement          * aSQLStmt,
                                qllOptNodeList        * aOptNodeList,
                                qllOptimizationNode   * aOptNode,
                                qloInitExprList       * aQueryExprList,
                                qllEnv                * aEnv );

stlStatus qlnoCompleteSetOP( qllDBCStmt            * aDBCStmt,
                             qllStatement          * aSQLStmt,
                             qllOptNodeList        * aOptNodeList,
                             qllOptimizationNode   * aOptNode,
                             qloInitExprList       * aQueryExprList,
                             qllEnv                * aEnv );

stlStatus qlnoCompleteSubQuery( qllDBCStmt            * aDBCStmt,
                                qllStatement          * aSQLStmt,
                                qllOptNodeList        * aOptNodeList,
                                qllOptimizationNode   * aOptNode,
                                qloInitExprList       * aQueryExprList,
                                qllEnv                * aEnv );

stlStatus qlnoCompleteSubQueryList( qllDBCStmt            * aDBCStmt,
                                    qllStatement          * aSQLStmt,
                                    qllOptNodeList        * aOptNodeList,
                                    qllOptimizationNode   * aOptNode,
                                    qloInitExprList       * aQueryExprList,
                                    qllEnv                * aEnv );

stlStatus qlnoCompleteSubQueryFunc( qllDBCStmt            * aDBCStmt,
                                    qllStatement          * aSQLStmt,
                                    qllOptNodeList        * aOptNodeList,
                                    qllOptimizationNode   * aOptNode,
                                    qloInitExprList       * aQueryExprList,
                                    qllEnv                * aEnv );

stlStatus qlnoCompleteSubQueryFilter( qllDBCStmt            * aDBCStmt,
                                      qllStatement          * aSQLStmt,
                                      qllOptNodeList        * aOptNodeList,
                                      qllOptimizationNode   * aOptNode,
                                      qloInitExprList       * aQueryExprList,
                                      qllEnv                * aEnv );

// stlStatus qlnoCompleteDelete( qllDBCStmt            * aDBCStmt,
//                              qllStatement          * aSQLStmt,
//                              qllOptNodeList        * aOptNodeList,
//                              qllOptimizationNode   * aOptNode,
//                              qloInitExprList       * aQueryExprList,
//                              qllEnv                * aEnv );

// stlStatus qlnoCompleteUpdate( qllDBCStmt            * aDBCStmt,
//                              qllStatement          * aSQLStmt,
//                              qllOptNodeList        * aOptNodeList,
//                              qllOptimizationNode   * aOptNode,
//                              qloInitExprList       * aQueryExprList,
//                              qllEnv                * aEnv );

// stlStatus qlnoCompleteIndexBuild( qllDBCStmt            * aDBCStmt,
//                                 qllStatement          * aSQLStmt,
//                                 qllOptNodeList        * aOptNodeList,
//                                 qllOptimizationNode   * aOptNode,
//                                 qloInitExprList       * aQueryExprList,
//                                 qllEnv                * aEnv );

// stlStatus qlnoCompleteForUpdate( qllDBCStmt            * aDBCStmt,
//                                 qllStatement          * aSQLStmt,
//                                 qllOptNodeList        * aOptNodeList,
//                                 qllOptimizationNode   * aOptNode,
//                                 qloInitExprList       * aQueryExprList,
//                                 qllEnv                * aEnv );

// stlStatus qlnoCompleteMerge( qllDBCStmt            * aDBCStmt,
//                             qllStatement          * aSQLStmt,
//                             qllOptNodeList        * aOptNodeList,
//                             qllOptimizationNode   * aOptNode,
//                             qloInitExprList       * aQueryExprList,
//                             qllEnv                * aEnv );

// stlStatus qlnoCompleteView( qllDBCStmt            * aDBCStmt,
//                            qllStatement          * aSQLStmt,
//                            qllOptNodeList        * aOptNodeList,
//                            qllOptimizationNode   * aOptNode,
//                            qloInitExprList       * aQueryExprList,
//                            qllEnv                * aEnv );

// stlStatus qlnoCompleteViewPushedPred( qllDBCStmt            * aDBCStmt,
//                                      qllStatement          * aSQLStmt,
//                                      qllOptNodeList        * aOptNodeList,
//                                      qllOptimizationNode   * aOptNode,
//                                      qloInitExprList       * aQueryExprList,
//                                      qllEnv                * aEnv );

// stlStatus qlnoCompleteFilter( qllDBCStmt            * aDBCStmt,
//                              qllStatement          * aSQLStmt,
//                              qllOptNodeList        * aOptNodeList,
//                              qllOptimizationNode   * aOptNode,
//                              qloInitExprList       * aQueryExprList,
//                              qllEnv                * aEnv );

// stlStatus qlnoCompleteInList( qllDBCStmt            * aDBCStmt,
//                              qllStatement          * aSQLStmt,
//                              qllOptNodeList        * aOptNodeList,
//                              qllOptimizationNode   * aOptNode,
//                              qloInitExprList       * aQueryExprList,
//                              qllEnv                * aEnv );

// stlStatus qlnoCompleteQuerySet( qllDBCStmt            * aDBCStmt,
//                                qllStatement          * aSQLStmt,
//                                qllOptNodeList        * aOptNodeList,
//                                qllOptimizationNode   * aOptNode,
//                                qloInitExprList       * aQueryExprList,
//                                qllEnv                * aEnv );

stlStatus qlnoCompleteConcat( qllDBCStmt            * aDBCStmt,
                              qllStatement          * aSQLStmt,
                              qllOptNodeList        * aOptNodeList,
                              qllOptimizationNode   * aOptNode,
                              qloInitExprList       * aQueryExprList,
                              qllEnv                * aEnv );

stlStatus qlnoCompleteNestedLoopsJoin( qllDBCStmt            * aDBCStmt,
                                       qllStatement          * aSQLStmt,
                                       qllOptNodeList        * aOptNodeList,
                                       qllOptimizationNode   * aOptNode,
                                       qloInitExprList       * aQueryExprList,
                                       qllEnv                * aEnv );

stlStatus qlnoCompleteSortMergeJoin( qllDBCStmt            * aDBCStmt,
                                     qllStatement          * aSQLStmt,
                                     qllOptNodeList        * aOptNodeList,
                                     qllOptimizationNode   * aOptNode,
                                     qloInitExprList       * aQueryExprList,
                                     qllEnv                * aEnv );

stlStatus qlnoCompleteHashJoin( qllDBCStmt            * aDBCStmt,
                                qllStatement          * aSQLStmt,
                                qllOptNodeList        * aOptNodeList,
                                qllOptimizationNode   * aOptNode,
                                qloInitExprList       * aQueryExprList,
                                qllEnv                * aEnv );

// stlStatus qlnoCompleteIndexJoin( qllDBCStmt            * aDBCStmt,
//                                 qllStatement          * aSQLStmt,
//                                 qllOptNodeList        * aOptNodeList,
//                                 qllOptimizationNode   * aOptNode,
//                                 qloInitExprList       * aQueryExprList,
//                                 qllEnv                * aEnv );

// stlStatus qlnoCompleteConnectBy( qllDBCStmt            * aDBCStmt,
//                                 qllStatement          * aSQLStmt,
//                                 qllOptNodeList        * aOptNodeList,
//                                 qllOptimizationNode   * aOptNode,
//                                 qloInitExprList       * aQueryExprList,
//                                 qllEnv                * aEnv );

// stlStatus qlnoCompletePseudoColumn( qllDBCStmt            * aDBCStmt,
//                                    qllStatement          * aSQLStmt,
//                                    qllOptNodeList        * aOptNodeList,
//                                    qllOptimizationNode   * aOptNode,
//                                    qloInitExprList       * aQueryExprList,
//                                    qllEnv                * aEnv );

// stlStatus qlnoCompleteCount( qllDBCStmt            * aDBCStmt,
//                             qllStatement          * aSQLStmt,
//                             qllOptNodeList        * aOptNodeList,
//                             qllOptimizationNode   * aOptNode,
//                             qloInitExprList       * aQueryExprList,
//                             qllEnv                * aEnv );

// stlStatus qlnoCompleteWindow( qllDBCStmt            * aDBCStmt,
//                              qllStatement          * aSQLStmt,
//                              qllOptNodeList        * aOptNodeList,
//                              qllOptimizationNode   * aOptNode,
//                              qloInitExprList       * aQueryExprList,
//                              qllEnv                * aEnv );

// stlStatus qlnoCompleteHash( qllDBCStmt            * aDBCStmt,
//                            qllStatement          * aSQLStmt,
//                            qllOptNodeList        * aOptNodeList,
//                            qllOptimizationNode   * aOptNode,
//                            qloInitExprList       * aQueryExprList,
//                            qllEnv                * aEnv );

stlStatus qlnoCompleteTableAccess( qllDBCStmt            * aDBCStmt,
                                   qllStatement          * aSQLStmt,
                                   qllOptNodeList        * aOptNodeList,
                                   qllOptimizationNode   * aOptNode,
                                   qloInitExprList       * aQueryExprList,
                                   qllEnv                * aEnv );

stlStatus qlnoCompleteIndexAccess( qllDBCStmt            * aDBCStmt,
                                   qllStatement          * aSQLStmt,
                                   qllOptNodeList        * aOptNodeList,
                                   qllOptimizationNode   * aOptNode,
                                   qloInitExprList       * aQueryExprList,
                                   qllEnv                * aEnv );

stlStatus qlnoCompleteRowIdAccess( qllDBCStmt            * aDBCStmt,
                                   qllStatement          * aSQLStmt,
                                   qllOptNodeList        * aOptNodeList,
                                   qllOptimizationNode   * aOptNode,
                                   qloInitExprList       * aQueryExprList,
                                   qllEnv                * aEnv );


// stlStatus qlnoCompleteFastDual( qllDBCStmt            * aDBCStmt,
//                                qllStatement          * aSQLStmt,
//                                qllOptNodeList        * aOptNodeList,
//                                qllOptimizationNode   * aOptNode,
//                                qloInitExprList       * aQueryExprList,
//                                qllEnv                * aEnv );

#if 0
stlStatus qlnoCompleteFlatInstantAccess( qllDBCStmt            * aDBCStmt,
                                         qllStatement          * aSQLStmt,
                                         qllOptNodeList        * aOptNodeList,
                                         qllOptimizationNode   * aOptNode,
                                         qloInitExprList       * aQueryExprList,
                                         qllEnv                * aEnv );
#endif

stlStatus qlnoCompleteSortInstantAccess( qllDBCStmt            * aDBCStmt,
                                         qllStatement          * aSQLStmt,
                                         qllOptNodeList        * aOptNodeList,
                                         qllOptimizationNode   * aOptNode,
                                         qloInitExprList       * aQueryExprList,
                                         qllEnv                * aEnv );

stlStatus qlnoCompleteHashInstantAccess( qllDBCStmt            * aDBCStmt,
                                         qllStatement          * aSQLStmt,
                                         qllOptNodeList        * aOptNodeList,
                                         qllOptimizationNode   * aOptNode,
                                         qloInitExprList       * aQueryExprList,
                                         qllEnv                * aEnv );

stlStatus qlnoCompleteGroupHashInstantAccess( qllDBCStmt            * aDBCStmt,
                                              qllStatement          * aSQLStmt,
                                              qllOptNodeList        * aOptNodeList,
                                              qllOptimizationNode   * aOptNode,
                                              qloInitExprList       * aQueryExprList,
                                              qllEnv                * aEnv );

stlStatus qlnoCompleteGroupSortInstantAccess( qllDBCStmt            * aDBCStmt,
                                              qllStatement          * aSQLStmt,
                                              qllOptNodeList        * aOptNodeList,
                                              qllOptimizationNode   * aOptNode,
                                              qloInitExprList       * aQueryExprList,
                                              qllEnv                * aEnv );

stlStatus qlnoCompleteHashAggregation( qllDBCStmt            * aDBCStmt,
                                       qllStatement          * aSQLStmt,
                                       qllOptNodeList        * aOptNodeList,
                                       qllOptimizationNode   * aOptNode,
                                       qloInitExprList       * aQueryExprList,
                                       qllEnv                * aEnv );

stlStatus qlnoCompleteGroup( qllDBCStmt            * aDBCStmt,
                             qllStatement          * aSQLStmt,
                             qllOptNodeList        * aOptNodeList,
                             qllOptimizationNode   * aOptNode,
                             qloInitExprList       * aQueryExprList,
                             qllEnv                * aEnv );

stlStatus qlnoCompleteSortJoinInstantAccess( qllDBCStmt            * aDBCStmt,
                                             qllStatement          * aSQLStmt,
                                             qllOptNodeList        * aOptNodeList,
                                             qllOptimizationNode   * aOptNode,
                                             qloInitExprList       * aQueryExprList,
                                             qllEnv                * aEnv );

stlStatus qlnoCompleteHashJoinInstantAccess( qllDBCStmt            * aDBCStmt,
                                             qllStatement          * aSQLStmt,
                                             qllOptNodeList        * aOptNodeList,
                                             qllOptimizationNode   * aOptNode,
                                             qloInitExprList       * aQueryExprList,
                                             qllEnv                * aEnv );


/****************************************************************************
 * Add Expression Node Functions
 ***************************************************************************/

/* Add Expr Node Function for Each Node */
typedef stlStatus (*qlnoAddExprNodeFuncPtr) ( qllDBCStmt            * aDBCStmt,
                                              qllStatement          * aSQLStmt,
                                              qllOptNodeList        * aOptNodeList,
                                              qllOptimizationNode   * aOptNode,
                                              qloInitExprList       * aQueryExprList,
                                              qllEnv                * aEnv );

stlStatus qlnoAddExprSelectStmt( qllDBCStmt            * aDBCStmt,
                                 qllStatement          * aSQLStmt,
                                 qllOptNodeList        * aOptNodeList,
                                 qllOptimizationNode   * aOptNode,
                                 qloInitExprList       * aQueryExprList,
                                 qllEnv                * aEnv );

stlStatus qlnoAddExprInsertStmt( qllDBCStmt            * aDBCStmt,
                                 qllStatement          * aSQLStmt,
                                 qllOptNodeList        * aOptNodeList,
                                 qllOptimizationNode   * aOptNode,
                                 qloInitExprList       * aQueryExprList,
                                 qllEnv                * aEnv );

stlStatus qlnoAddExprUpdateStmt( qllDBCStmt            * aDBCStmt,
                                 qllStatement          * aSQLStmt,
                                 qllOptNodeList        * aOptNodeList,
                                 qllOptimizationNode   * aOptNode,
                                 qloInitExprList       * aQueryExprList,
                                 qllEnv                * aEnv );

stlStatus qlnoAddExprDeleteStmt( qllDBCStmt            * aDBCStmt,
                                 qllStatement          * aSQLStmt,
                                 qllOptNodeList        * aOptNodeList,
                                 qllOptimizationNode   * aOptNode,
                                 qloInitExprList       * aQueryExprList,
                                 qllEnv                * aEnv );

stlStatus qlnoAddExprQuerySpec( qllDBCStmt            * aDBCStmt,
                                qllStatement          * aSQLStmt,
                                qllOptNodeList        * aOptNodeList,
                                qllOptimizationNode   * aOptNode,
                                qloInitExprList       * aQueryExprList,
                                qllEnv                * aEnv );

stlStatus qlnoAddExprQuerySet( qllDBCStmt            * aDBCStmt,
                               qllStatement          * aSQLStmt,
                               qllOptNodeList        * aOptNodeList,
                               qllOptimizationNode   * aOptNode,
                               qloInitExprList       * aQueryExprList,
                               qllEnv                * aEnv );

stlStatus qlnoAddExprSetOP( qllDBCStmt            * aDBCStmt,
                            qllStatement          * aSQLStmt,
                            qllOptNodeList        * aOptNodeList,
                            qllOptimizationNode   * aOptNode,
                            qloInitExprList       * aQueryExprList,
                            qllEnv                * aEnv );

stlStatus qlnoAddExprSubQuery( qllDBCStmt            * aDBCStmt,
                               qllStatement          * aSQLStmt,
                               qllOptNodeList        * aOptNodeList,
                               qllOptimizationNode   * aOptNode,
                               qloInitExprList       * aQueryExprList,
                               qllEnv                * aEnv );

stlStatus qlnoAddExprSubQueryList( qllDBCStmt            * aDBCStmt,
                                   qllStatement          * aSQLStmt,
                                   qllOptNodeList        * aOptNodeList,
                                   qllOptimizationNode   * aOptNode,
                                   qloInitExprList       * aQueryExprList,
                                   qllEnv                * aEnv );

stlStatus qlnoAddExprSubQueryFunc( qllDBCStmt            * aDBCStmt,
                                   qllStatement          * aSQLStmt,
                                   qllOptNodeList        * aOptNodeList,
                                   qllOptimizationNode   * aOptNode,
                                   qloInitExprList       * aQueryExprList,
                                   qllEnv                * aEnv );

stlStatus qlnoAddExprSubQueryFilter( qllDBCStmt            * aDBCStmt,
                                     qllStatement          * aSQLStmt,
                                     qllOptNodeList        * aOptNodeList,
                                     qllOptimizationNode   * aOptNode,
                                     qloInitExprList       * aQueryExprList,
                                     qllEnv                * aEnv );

stlStatus qlnoAddExprConcat( qllDBCStmt            * aDBCStmt,
                             qllStatement          * aSQLStmt,
                             qllOptNodeList        * aOptNodeList,
                             qllOptimizationNode   * aOptNode,
                             qloInitExprList       * aQueryExprList,
                             qllEnv                * aEnv );

stlStatus qlnoAddExprNestedLoopsJoin( qllDBCStmt            * aDBCStmt,
                                      qllStatement          * aSQLStmt,
                                      qllOptNodeList        * aOptNodeList,
                                      qllOptimizationNode   * aOptNode,
                                      qloInitExprList       * aQueryExprList,
                                      qllEnv                * aEnv );

stlStatus qlnoAddExprSortMergeJoin( qllDBCStmt            * aDBCStmt,
                                    qllStatement          * aSQLStmt,
                                    qllOptNodeList        * aOptNodeList,
                                    qllOptimizationNode   * aOptNode,
                                    qloInitExprList       * aQueryExprList,
                                    qllEnv                * aEnv );

stlStatus qlnoAddExprHashJoin( qllDBCStmt            * aDBCStmt,
                               qllStatement          * aSQLStmt,
                               qllOptNodeList        * aOptNodeList,
                               qllOptimizationNode   * aOptNode,
                               qloInitExprList       * aQueryExprList,
                               qllEnv                * aEnv );

stlStatus qlnoAddExprTableAccess( qllDBCStmt            * aDBCStmt,
                                  qllStatement          * aSQLStmt,
                                  qllOptNodeList        * aOptNodeList,
                                  qllOptimizationNode   * aOptNode,
                                  qloInitExprList       * aQueryExprList,
                                  qllEnv                * aEnv );

stlStatus qlnoAddExprIndexAccess( qllDBCStmt            * aDBCStmt,
                                  qllStatement          * aSQLStmt,
                                  qllOptNodeList        * aOptNodeList,
                                  qllOptimizationNode   * aOptNode,
                                  qloInitExprList       * aQueryExprList,
                                  qllEnv                * aEnv );

stlStatus qlnoAddExprRowIdAccess( qllDBCStmt            * aDBCStmt,
                                  qllStatement          * aSQLStmt,
                                  qllOptNodeList        * aOptNodeList,
                                  qllOptimizationNode   * aOptNode,
                                  qloInitExprList       * aQueryExprList,
                                  qllEnv                * aEnv );

// stlStatus qlnoAddExprFastDual( qllDBCStmt            * aDBCStmt,
//                                qllStatement          * aSQLStmt,
// qllOptNodeList        * aOptNodeList,
//                                qllOptimizationNode   * aOptNode,
//                                qloInitExprList       * aQueryExprList,
//                                qllEnv                * aEnv );

#if 0
stlStatus qlnoAddExprFlatInstantAccess( qllDBCStmt            * aDBCStmt,
                                        qllStatement          * aSQLStmt,
                                        qllOptNodeList        * aOptNodeList,
                                        qllOptimizationNode   * aOptNode,
                                        qloInitExprList       * aQueryExprList,
                                        qllEnv                * aEnv );
#endif

stlStatus qlnoAddExprSortInstantAccess( qllDBCStmt            * aDBCStmt,
                                        qllStatement          * aSQLStmt,
                                        qllOptNodeList        * aOptNodeList,
                                        qllOptimizationNode   * aOptNode,
                                        qloInitExprList       * aQueryExprList,
                                        qllEnv                * aEnv );

#if 0
stlStatus qlnoAddExprHashInstantAccess( qllDBCStmt            * aDBCStmt,
                                        qllStatement          * aSQLStmt,
                                        qllOptNodeList        * aOptNodeList,
                                        qllOptimizationNode   * aOptNode,
                                        qloInitExprList       * aQueryExprList,
                                        qllEnv                * aEnv );
#endif

stlStatus qlnoAddExprGroupHashInstantAccess( qllDBCStmt            * aDBCStmt,
                                             qllStatement          * aSQLStmt,
                                             qllOptNodeList        * aOptNodeList,
                                             qllOptimizationNode   * aOptNode,
                                             qloInitExprList       * aQueryExprList,
                                             qllEnv                * aEnv );

#if 0
stlStatus qlnoAddExprGroupSortInstantAccess( qllDBCStmt            * aDBCStmt,
                                             qllStatement          * aSQLStmt,
                                             qllOptNodeList        * aOptNodeList,
                                             qllOptimizationNode   * aOptNode,
                                             qloInitExprList       * aQueryExprList,
                                             qllEnv                * aEnv );
#endif

stlStatus qlnoAddExprHashAggregation( qllDBCStmt            * aDBCStmt,
                                      qllStatement          * aSQLStmt,
                                      qllOptNodeList        * aOptNodeList,
                                      qllOptimizationNode   * aOptNode,
                                      qloInitExprList       * aQueryExprList,
                                      qllEnv                * aEnv );

stlStatus qlnoAddExprGroup( qllDBCStmt            * aDBCStmt,
                            qllStatement          * aSQLStmt,
                            qllOptNodeList        * aOptNodeList,
                            qllOptimizationNode   * aOptNode,
                            qloInitExprList       * aQueryExprList,
                            qllEnv                * aEnv );

stlStatus qlnoAddExprSortJoinInstantAccess( qllDBCStmt            * aDBCStmt,
                                            qllStatement          * aSQLStmt,
                                            qllOptNodeList        * aOptNodeList,
                                            qllOptimizationNode   * aOptNode,
                                            qloInitExprList       * aQueryExprList,
                                            qllEnv                * aEnv );

stlStatus qlnoAddExprHashJoinInstantAccess( qllDBCStmt            * aDBCStmt,
                                            qllStatement          * aSQLStmt,
                                            qllOptNodeList        * aOptNodeList,
                                            qllOptimizationNode   * aOptNode,
                                            qloInitExprList       * aQueryExprList,
                                            qllEnv                * aEnv );


/** @} qlocQueryExpr */


/*******************************************************************************
 * INSERT : Code Optimizaion 
 ******************************************************************************/

/**
 * @defgroup qlocInsert INSERT VALUES Code Optimization
 * @ingroup qloc
 * @{
 */

stlStatus qloCodeOptimizeInsertValues( smlTransId     aTransID,
                                       qllDBCStmt   * aDBCStmt,
                                       qllStatement * aSQLStmt,
                                       qllEnv       * aEnv );

/** @} qlocInsert */

/**
 * @defgroup qlocInsertSelect INSERT .. SELECT Code Optimization
 * @ingroup qloc
 * @{
 */

stlStatus qloCodeOptimizeInsertSelect( smlTransId     aTransID,
                                       qllDBCStmt   * aDBCStmt,
                                       qllStatement * aSQLStmt,
                                       qllEnv       * aEnv );

/** @} qlocInsertSelect */


/**
 * @defgroup qlocInsertReturnInto INSERT RETURNING INTO Code Optimization
 * @ingroup qloc
 * @{
 */

stlStatus qloCodeOptimizeInsertReturnInto( smlTransId     aTransID,
                                           qllDBCStmt   * aDBCStmt,
                                           qllStatement * aSQLStmt,
                                           qllEnv       * aEnv );

/** @} qlocInsertReturnInto */



/**
 * @defgroup qlocInsertReturnQuery INSERT RETURNING Query Code Optimization
 * @ingroup qloc
 * @{
 */

stlStatus qloCodeOptimizeInsertReturnQuery( smlTransId     aTransID,
                                            qllDBCStmt   * aDBCStmt,
                                            qllStatement * aSQLStmt,
                                            qllEnv       * aEnv );

/** @} qlocInsertReturnQuery */



/*******************************************************************************
 * UPDATE : Code Optimizaion
 ******************************************************************************/

/**
 * @defgroup qlocUpdate UPDATE Code Optimization
 * @ingroup qloc
 * @{
 */

stlStatus qloCodeOptimizeUpdate( smlTransId     aTransID,
                                 qllDBCStmt   * aDBCStmt,
                                 qllStatement * aSQLStmt,
                                 qllEnv       * aEnv );

/** @} qlocUpdate */


/**
 * @defgroup qlocUpdateReturnInto UPDATE RETURNING INTO Code Optimization
 * @ingroup qloc
 * @{
 */

stlStatus qloCodeOptimizeUpdateReturnInto( smlTransId     aTransID,
                                           qllDBCStmt   * aDBCStmt,
                                           qllStatement * aSQLStmt,
                                           qllEnv       * aEnv );

/** @} qlocUpdateReturnInto */


/**
 * @defgroup qlocUpdateReturnQuery UPDATE RETURNING Query Code Optimization
 * @ingroup qloc
 * @{
 */

stlStatus qloCodeOptimizeUpdateReturnQuery( smlTransId     aTransID,
                                            qllDBCStmt   * aDBCStmt,
                                            qllStatement * aSQLStmt,
                                            qllEnv       * aEnv );

/** @} qlocUpdateReturnQuery */


/**
 * @defgroup qlocUpdatePositioned UPDATE CURRENT OF Code Optimization
 * @ingroup qloc
 * @{
 */

stlStatus qloCodeOptimizeUpdatePositioned( smlTransId     aTransID,
                                           qllDBCStmt   * aDBCStmt,
                                           qllStatement * aSQLStmt,
                                           qllEnv       * aEnv );

/** @} qlocUpdatePositioned */


/*******************************************************************************
 * DELETE : Code Optimizaion
 ******************************************************************************/

/**
 * @defgroup qlocDelete DELETE Code Optimization
 * @ingroup qloc
 * @{
 */

stlStatus qloCodeOptimizeDelete( smlTransId     aTransID,
                                 qllDBCStmt   * aDBCStmt,
                                 qllStatement * aSQLStmt,
                                 qllEnv       * aEnv );

/** @} qlocDelete */


/**
 * @defgroup qlocDeleteReturnInto DELETE RETURNING INTO Code Optimization
 * @ingroup qloc
 * @{
 */

stlStatus qloCodeOptimizeDeleteReturnInto( smlTransId     aTransID,
                                           qllDBCStmt   * aDBCStmt,
                                           qllStatement * aSQLStmt,
                                           qllEnv       * aEnv );

/** @} qlocDeleteReturnInto */


/**
 * @defgroup qlocDeleteReturnQuery DELETE RETURNING Query Code Optimization
 * @ingroup qloc
 * @{
 */

stlStatus qloCodeOptimizeDeleteReturnQuery( smlTransId     aTransID,
                                            qllDBCStmt   * aDBCStmt,
                                            qllStatement * aSQLStmt,
                                            qllEnv       * aEnv );

/** @} qlocDeleteReturnQuery */


/**
 * @defgroup qlocDeletePositioned DELETE CURRENT OF Code Optimization
 * @ingroup qloc
 * @{
 */

stlStatus qloCodeOptimizeDeletePositioned( smlTransId     aTransID,
                                           qllDBCStmt   * aDBCStmt,
                                           qllStatement * aSQLStmt,
                                           qllEnv       * aEnv );

/** @} qlocDeletePositioned */


/*******************************************************************************
 * Expression : Code Optimizaion
 ******************************************************************************/

/**
 * @defgroup qlocExpression Expression Code Optimization
 * @ingroup qloc
 * @{
 */


/* Expression : Code Optimizaion */
stlStatus qloCodeOptimizeExpression( stlChar             * aSQLString,
                                     qlvInitExpression   * aInitExprCode,
                                     knlBindContext      * aBindContext,
                                     knlExprStack        * aExeCodeExpr,
                                     knlExprStack        * aConstExpr,
                                     qloExprInfo         * aExprInfo,
                                     knlExprEntry       ** aExprEntry,
                                     knlRegionMem        * aRegionMem,
                                     qllEnv              * aEnv );

stlStatus qloCodeOptimizeColumn( stlChar            * aSQLString,
                                 qlvInitExpression  * aInitColumnCode,
                                 knlExprStack       * aExeExprCodeStack,
                                 knlExprStack       * aConstExprCodeStack,
                                 stlUInt32            aGroupEntryNo,
                                 knlBuiltInFunc       aRelatedFuncId,
                                 qloExprInfo        * aExprInfo,
                                 knlExprEntry      ** aExprEntry,
                                 knlRegionMem       * aRegionMem,
                                 qllEnv             * aEnv );

stlStatus qloCodeOptimizeValue( stlChar            * aSQLString,
                                qlvInitExpression  * aInitValueCode,
                                knlExprStack       * aExeExprCodeStack,
                                knlExprStack       * aConstExprCodeStack,
                                stlUInt32            aGroupEntryNo,
                                knlBuiltInFunc       aRelatedFuncId,
                                qloExprInfo        * aExprInfo,
                                knlExprEntry      ** aExprEntry,
                                knlRegionMem       * aRegionMem,
                                qllEnv             * aEnv );

stlStatus qloCodeOptimizeBindParam( stlChar            * aSQLString,
                                    qlvInitExpression  * aInitValueCode,
                                    knlBindContext     * aBindContext,
                                    knlExprStack       * aExeExprCodeStack,
                                    knlExprStack       * aConstExprCodeStack,
                                    stlUInt32            aGroupEntryNo,
                                    knlBuiltInFunc       aRelatedFuncId,
                                    qloExprInfo        * aExprInfo,
                                    knlExprEntry      ** aExprEntry,
                                    knlRegionMem       * aRegionMem,
                                    qllEnv             * aEnv );

stlStatus qloCodeOptimizeFunction( stlChar            * aSQLString,
                                   qlvInitExpression  * aInitValueCode,
                                   knlBindContext     * aBindContext,
                                   knlExprStack       * aExeExprCodeStack,
                                   knlExprStack       * aConstExprCodeStack,
                                   stlUInt32            aGroupEntryNo,
                                   knlBuiltInFunc       aRelatedFuncId,
                                   qloExprInfo        * aExprInfo,
                                   knlExprEntry      ** aExprEntry,
                                   knlRegionMem       * aRegionMem,
                                   qllEnv             * aEnv );

stlStatus qloCodeOptimizeCast( stlChar            * aSQLString,
                               qlvInitExpression  * aInitCastCode,
                               knlBindContext     * aBindContext,
                               knlExprStack       * aExeExprCodeStack,
                               knlExprStack       * aConstExprCodeStack,
                               stlUInt32            aGroupEntryNo,
                               knlBuiltInFunc       aRelatedFuncId,
                               qloExprInfo        * aExprInfo,
                               knlExprEntry      ** aExprEntry,
                               knlRegionMem       * aRegionMem,
                               qllEnv             * aEnv );

stlStatus qloCodeOptimizeCaseExpr( stlChar            * aSQLString,
                                   qlvInitExpression  * aInitCaseExprCode,
                                   knlBindContext     * aBindContext,
                                   knlExprStack       * aExeExprCodeStack,
                                   knlExprStack       * aConstExprCodeStack,
                                   stlUInt32            aGroupEntryNo,
                                   knlBuiltInFunc       aRelatedFuncId,
                                   qloExprInfo        * aExprInfo,
                                   knlExprEntry      ** aExprEntry,
                                   knlRegionMem       * aRegionMem,
                                   qllEnv             * aEnv );

stlStatus qloCodeOptimizePseudoCol( stlChar            * aSQLString,
                                    qlvInitExpression  * aInitValueCode,
                                    knlExprStack       * aExeExprCodeStack,
                                    knlExprStack       * aConstExprCodeStack,
                                    stlUInt32            aGroupEntryNo,
                                    knlBuiltInFunc       aRelatedFuncId,
                                    qloExprInfo        * aExprInfo,
                                    knlExprEntry      ** aExprEntry,
                                    knlRegionMem       * aRegionMem,
                                    qllEnv             * aEnv );

stlStatus qloCodeOptimizeConstExpr( stlChar            * aSQLString,
                                    qlvInitExpression  * aInitCastCode,
                                    knlBindContext     * aBindContext,
                                    knlExprStack       * aExeExprCodeStack,
                                    knlExprStack       * aConstExprCodeStack,
                                    stlUInt32            aGroupEntryNo,
                                    knlBuiltInFunc       aRelatedFuncId,
                                    qloExprInfo        * aExprInfo,
                                    knlExprEntry      ** aExprEntry,
                                    knlRegionMem       * aRegionMem,
                                    qllEnv             * aEnv );

stlStatus qloCodeOptimizeReference( stlChar            * aSQLString,
                                    qlvInitExpression  * aInitCastCode,
                                    knlBindContext     * aBindContext,
                                    knlExprStack       * aExeExprCodeStack,
                                    knlExprStack       * aConstExprCodeStack,
                                    stlUInt32            aGroupEntryNo,
                                    knlBuiltInFunc       aRelatedFuncId,
                                    qloExprInfo        * aExprInfo,
                                    knlExprEntry      ** aExprEntry,
                                    knlRegionMem       * aRegionMem,
                                    qllEnv             * aEnv );

stlStatus qloCodeOptimizeSubQueryExpr( stlChar            * aSQLString,
                                       qlvInitExpression  * aInitSubQueryCode,
                                       knlBindContext     * aBindContext,
                                       knlExprStack       * aExeExprCodeStack,
                                       knlExprStack       * aConstExprCodeStack,
                                       stlUInt32            aGroupEntryNo,
                                       knlBuiltInFunc       aRelatedFuncId,
                                       qloExprInfo        * aExprInfo,
                                       knlExprEntry      ** aExprEntry,
                                       knlRegionMem       * aRegionMem,
                                       qllEnv             * aEnv );

stlStatus qloCodeOptimizeInnerColumn( stlChar            * aSQLString,
                                      qlvInitExpression  * aInitExprCode,
                                      knlBindContext     * aBindContext,
                                      knlExprStack       * aExeExprCodeStack,
                                      knlExprStack       * aConstExprCodeStack,
                                      stlUInt32            aGroupEntryNo,
                                      knlBuiltInFunc       aRelatedFuncId,
                                      qloExprInfo        * aExprInfo,
                                      knlExprEntry      ** aExprEntry,
                                      knlRegionMem       * aRegionMem,
                                      qllEnv             * aEnv );

stlStatus qloCodeOptimizeOuterColumn( stlChar            * aSQLString,
                                      qlvInitExpression  * aInitExprCode,
                                      knlBindContext     * aBindContext,
                                      knlExprStack       * aExeExprCodeStack,
                                      knlExprStack       * aConstExprCodeStack,
                                      stlUInt32            aGroupEntryNo,
                                      knlBuiltInFunc       aRelatedFuncId,
                                      qloExprInfo        * aExprInfo,
                                      knlExprEntry      ** aExprEntry,
                                      knlRegionMem       * aRegionMem,
                                      qllEnv             * aEnv );

stlStatus qloCodeOptimizeRowIdColumn( stlChar            * aSQLString,
                                      qlvInitExpression  * aInitRowIdColumnCode,
                                      knlExprStack       * aExeExprCodeStack,
                                      knlExprStack       * aConstExprCodeStack,
                                      stlUInt32            aGroupEntryNo,
                                      knlBuiltInFunc       aRelatedFuncId,
                                      qloExprInfo        * aExprInfo,
                                      knlExprEntry      ** aExprEntry,
                                      knlRegionMem       * aRegionMem,
                                      qllEnv             * aEnv );

stlStatus qloCodeOptimizeAggregation( stlChar            * aSQLString,
                                      qlvInitExpression  * aInitFuncCode,
                                      knlBindContext     * aBindContext,
                                      knlExprStack       * aExeExprCodeStack,
                                      knlExprStack       * aConstExprCodeStack,
                                      stlUInt32            aGroupEntryNo,
                                      knlBuiltInFunc       aRelatedFuncId,
                                      qloExprInfo        * aExprInfo,
                                      knlExprEntry      ** aExprEntry,
                                      knlRegionMem       * aRegionMem,
                                      qllEnv             * aEnv );

stlStatus qloCodeOptimizeListColumn( stlChar            * aSQLString,
                                     qlvInitExpression  * aInitColumnCode,
                                     knlBindContext     * aBindContext,
                                     knlExprStack       * aExeExprCodeStack,
                                     knlExprStack       * aConstExprCodeStack,
                                     stlUInt32            aGroupEntryNo,
                                     knlBuiltInFunc       aRelatedFuncId,
                                     qloExprInfo        * aExprInfo,
                                     knlExprEntry      ** aExprEntry,
                                     knlRegionMem       * aRegionMem,
                                     qllEnv             * aEnv );

stlStatus qloCodeOptimizeListFunction( stlChar            * aSQLString,
                                       qlvInitExpression  * aInitColumnCode,
                                       knlBindContext     * aBindContext,
                                       knlExprStack       * aExeExprCodeStack,
                                       knlExprStack       * aConstExprCodeStack,
                                       stlUInt32            aGroupEntryNo,
                                       knlBuiltInFunc       aRelatedFuncId,
                                       qloExprInfo        * aExprInfo,
                                       knlExprEntry      ** aExprEntry,
                                       knlRegionMem       * aRegionMem,
                                       qllEnv             * aEnv );

stlStatus qloCodeOptimizeRowExpr( stlChar            * aSQLString,
                                  qlvInitExpression  * aInitColumnCode,
                                  knlBindContext     * aBindContext,
                                  knlExprStack       * aExeExprCodeStack,
                                  knlExprStack       * aConstExprCodeStack,
                                  stlUInt32            aGroupEntryNo,
                                  knlBuiltInFunc       aRelatedFuncId,
                                  qloExprInfo        * aExprInfo,
                                  knlExprEntry      ** aExprEntry,
                                  knlRegionMem       * aRegionMem,
                                  qllEnv             * aEnv );

/* Expression : Common */
stlStatus qloAddExprStackEntry( stlChar             * aSQLString,
                                qlvInitExpression   * aInitExprCode,
                                knlBindContext      * aBindContext,
                                knlExprStack        * aExeCodeExpr,
                                knlExprStack        * aConstExpr,
                                stlUInt32             aGroupEntryNo,
                                knlBuiltInFunc        aRelatedFuncId,
                                qloExprInfo         * aExprInfo,
                                knlExprEntry       ** aExprEntry,
                                knlRegionMem        * aRegionMem,
                                qllEnv              * aEnv );

/* Expression Info. */
stlStatus qloCreateExprInfo( stlInt32            aListCount,
                             qloExprInfo      ** aExprInfo,
                             knlRegionMem      * aRegionMem,
                             qllEnv            * aEnv );

stlStatus qloSetExprDBType( qlvInitExpression     * aInitExprCode,
                            qloExprInfo           * aExprInfo,
                            dtlDataType             aDBType,
                            stlInt64                aPrecision,
                            stlInt64                aScale,
                            dtlStringLengthUnit     aStringLengthUnit,
                            dtlIntervalIndicator    aIntervalIndicator,
                            qllEnv                * aEnv );

stlStatus qloCastExprDBType( qlvInitExpression     * aInitExprCode,
                             qloExprInfo           * aExprInfo,
                             dtlDataType             aDBType,
                             stlInt64                aPrecision,
                             stlInt64                aScale,
                             dtlStringLengthUnit     aStringLengthUnit,
                             dtlIntervalIndicator    aIntervalIndicator,
                             stlBool                 aIsInFunction,
                             stlBool                 aIsFixedLengthCast,
                             qllEnv                * aEnv );

stlStatus qloSetExprFunc( qlvInitExpression       * aInitExprCode,
                          qloExprInfo             * aExprInfo,
                          knlExprEntry            * aExprEntry,
                          knlBuiltInFunc            aFuncID,
                          stlUInt32                 aFuncIdx,
                          stlUInt32                 aResultBufferSize,
                          dtlFuncReturnTypeClass    aReturnType,
                          qllEnv                  * aEnv );

stlStatus qloSetListFuncExpr( qlvInitExpression       * aInitExprCode,
                              qloExprInfo             * aExprInfo,
                              knlListStack            * aInitStack,
                              stlUInt32                 aRowCount,
                              stlBool                   aOnlyValue,
                              knlRegionMem            * aRegionMem,
                              qllEnv                  * aEnv );
/* datatype validation */
stlStatus qloGetValueDataType( qlvInitValue  * aInitValueCode,
                               dtlDataType   * aDataType,
                               qllEnv        * aEnv );

/* logical filter vs physical filter */
stlStatus qloClassifyFilterExpr( stlChar             * aSQLString,
                                 qlvInitExpression   * aFilterExpr,
                                 qlvInitNode         * aRelationNode,
                                 knlRegionMem        * aRegionMem,
                                 knlBindContext      * aBindContext,
                                 qlvInitExpression  ** aLogicalFilter,
                                 qlvInitExpression  ** aPhysicalFilter,
                                 qllEnv              * aEnv );

stlStatus qloGetRowIdScanExprAndFilter( qlvInitExpression   * aWhereExpr,
                                        knlRegionMem        * aRegionMem,
                                        qlvInitExpression  ** aRowIdScanExpr,
                                        qlvInitExpression  ** aFilterExpr,
                                        qllEnv              * aEnv );

stlStatus qloClassifyRowIdScanExprAndFilter( qlvInitExpression  * aWhereExpr,
                                             knlRegionMem       * aRegionMem,
                                             qlvRefExprList     * aRowIdScanExprList,
                                             qlvRefExprList     * aFilterExprList,
                                             qllEnv             * aEnv );

/* logical filter vs physical filter */
stlBool qloIsExistColumnInExpr( qlvInitExpression  * aExpr,
                                qlvInitNode        * aNode );

stlStatus qloMakePhysicalFilterStack( qllDBCStmt          * aDBCStmt,
                                      qllStatement        * aSQLStmt,
                                      qloInitExprList     * aQueryExprList,
                                      qlvInitExpression   * aPhysicalFilterExpr,
                                      knlExprStack       ** aPreFilterExprStack,
                                      qllEnv              * aEnv );

stlStatus qloMakeRangeStack( qllDBCStmt          * aDBCStmt,
                             qllStatement        * aSQLStmt,
                             qloInitExprList     * aQueryExprList,
                             stlInt32              aColCount,
                             stlBool               aIsHashKey,
                             qlvRefExprList     ** aRangeExpr1,
                             qlvRefExprList     ** aRangeExpr2,
                             knlExprStack       ** aPreRangeExprStack,
                             qllEnv              * aEnv );

stlStatus qloMakeExprStackForInKeyRange( qllDBCStmt          * aDBCStmt,
                                         qllStatement        * aSQLStmt,
                                         qloInitExprList     * aQueryExprList,
                                         stlInt32              aListColCount,
                                         qlvInitExpression  ** aValueExpr,
                                         qlvInitExpression  ** aListColExpr,
                                         knlExprStack       ** aPreExprStack,
                                         qllEnv              * aEnv );

/** @} qlocExpression */




/*******************************************************************************
 * QUERY EXPR : Data Optimizaion for Building Execution Node Functions
 ******************************************************************************/

/**
 * @defgroup qlodDataOptNode QUERY EXPR Data Optimization
 * @ingroup qlod
 * @{
 */

/* Data Optimize Node Function for Each Node */
typedef stlStatus (*qlndDataOptimizeNodeFuncPtr) ( smlTransId              aTransID,
                                                   qllDBCStmt            * aDBCStmt,
                                                   qllStatement          * aSQLStmt,
                                                   qllOptimizationNode   * aOptNode,
                                                   qllDataArea           * aDataArea,
                                                   qllEnv                * aEnv );

stlStatus qlndDataOptimizeSelectStmt( smlTransId              aTransID,
                                      qllDBCStmt            * aDBCStmt,
                                      qllStatement          * aSQLStmt,
                                      qllOptimizationNode   * aOptNode,
                                      qllDataArea           * aDataArea,
                                      qllEnv                * aEnv );

stlStatus qlndDataOptimizeInsertStmt( smlTransId              aTransID,
                                      qllDBCStmt            * aDBCStmt,
                                      qllStatement          * aSQLStmt,
                                      qllOptimizationNode   * aOptNode,
                                      qllDataArea           * aDataArea,
                                      qllEnv                * aEnv );

stlStatus qlndDataOptimizeUpdateStmt( smlTransId              aTransID,
                                      qllDBCStmt            * aDBCStmt,
                                      qllStatement          * aSQLStmt,
                                      qllOptimizationNode   * aOptNode,
                                      qllDataArea           * aDataArea,
                                      qllEnv                * aEnv );

stlStatus qlndDataOptimizeDeleteStmt( smlTransId              aTransID,
                                      qllDBCStmt            * aDBCStmt,
                                      qllStatement          * aSQLStmt,
                                      qllOptimizationNode   * aOptNode,
                                      qllDataArea           * aDataArea,
                                      qllEnv                * aEnv );

// stlStatus qlndDataOptimizeMergeStmt( smlTransId              aTransID,
//                                 qllDBCStmt            * aDBCStmt,
//                                 qllStatement          * aSQLStmt,
//                                 qllOptimizationNode   * aOptNode,
//                                 // qllDataArea           * aDataArea,
//                                 qllEnv                * aEnv );

// stlStatus qlndDataOptimizeCraeteAsSelectStmt( smlTransId              aTransID,
//                                          qllDBCStmt            * aDBCStmt,
//                                          qllStatement          * aSQLStmt,
//                                          qllOptimizationNode   * aOptNode,
//                                          // qllDataArea           * aDataArea,
//                                          qllEnv                * aEnv );

// stlStatus qlndDataOptimizeDDLStmt( smlTransId              aTransID,
//                               qllDBCStmt            * aDBCStmt,
//                               qllStatement          * aSQLStmt,
//                               qllOptimizationNode   * aOptNode,
//                               // qllDataArea           * aDataArea,
//                               qllEnv                * aEnv );

stlStatus qlndDataOptimizeQuerySpec( smlTransId              aTransID,
                                     qllDBCStmt            * aDBCStmt,
                                     qllStatement          * aSQLStmt,
                                     qllOptimizationNode   * aOptNode,
                                     qllDataArea           * aDataArea,
                                     qllEnv                * aEnv );

// stlStatus qlndDataOptimizeDelete( smlTransId              aTransID,
//                              qllDBCStmt            * aDBCStmt,
//                              qllStatement          * aSQLStmt,
//                              qllOptimizationNode   * aOptNode,
//                              // qllDataArea           * aDataArea,
//                              qllEnv                * aEnv );

// stlStatus qlndDataOptimizeUpdate( smlTransId              aTransID,
//                              qllDBCStmt            * aDBCStmt,
//                              qllStatement          * aSQLStmt,
//                              qllOptimizationNode   * aOptNode,
//                              // qllDataArea           * aDataArea,
//                              qllEnv                * aEnv );

// stlStatus qlndDataOptimizeIndexBuild( smlTransId              aTransID,
//                                 qllDBCStmt            * aDBCStmt,
//                                 qllStatement          * aSQLStmt,
//                                 qllOptimizationNode   * aOptNode,
//                                 // qllDataArea           * aDataArea,
//                                 qllEnv                * aEnv );

// stlStatus qlndDataOptimizeForUpdate( smlTransId              aTransID,
//                                 qllDBCStmt            * aDBCStmt,
//                                 qllStatement          * aSQLStmt,
//                                 qllOptimizationNode   * aOptNode,
//                                 // qllDataArea           * aDataArea,
//                                 qllEnv                * aEnv );

// stlStatus qlndDataOptimizeMerge( smlTransId              aTransID,
//                             qllDBCStmt            * aDBCStmt,
//                             qllStatement          * aSQLStmt,
//                             qllOptimizationNode   * aOptNode,
//                             // qllDataArea           * aDataArea,
//                             qllEnv                * aEnv );

// stlStatus qlndDataOptimizeView( smlTransId              aTransID,
//                            qllDBCStmt            * aDBCStmt,
//                            qllStatement          * aSQLStmt,
//                            qllOptimizationNode   * aOptNode,
//                            // qllDataArea           * aDataArea,
//                            qllEnv                * aEnv );

// stlStatus qlndDataOptimizeViewPushedPred( smlTransId              aTransID,
//                                      qllDBCStmt            * aDBCStmt,
//                                      qllStatement          * aSQLStmt,
//                                      qllOptimizationNode   * aOptNode,
//                                      // qllDataArea           * aDataArea,
//                                      qllEnv                * aEnv );

// stlStatus qlndDataOptimizeFilter( smlTransId              aTransID,
//                              qllDBCStmt            * aDBCStmt,
//                              qllStatement          * aSQLStmt,
//                              qllOptimizationNode   * aOptNode,
//                              // qllDataArea           * aDataArea,
//                              qllEnv                * aEnv );

// stlStatus qlndDataOptimizeInList( smlTransId              aTransID,
//                              qllDBCStmt            * aDBCStmt,
//                              qllStatement          * aSQLStmt,
//                              qllOptimizationNode   * aOptNode,
//                              // qllDataArea           * aDataArea,
//                              qllEnv                * aEnv );

stlStatus qlndDataOptimizeQuerySet( smlTransId              aTransID,
                                    qllDBCStmt            * aDBCStmt,
                                    qllStatement          * aSQLStmt,
                                    qllOptimizationNode   * aOptNode,
                                    qllDataArea           * aDataArea,
                                    qllEnv                * aEnv );

stlStatus qlndDataOptimizeSetOP( smlTransId              aTransID,
                                 qllDBCStmt            * aDBCStmt,
                                 qllStatement          * aSQLStmt,
                                 qllOptimizationNode   * aOptNode,
                                 qllDataArea           * aDataArea,
                                 qllEnv                * aEnv );

stlStatus qlndDataOptimizeSubQuery( smlTransId              aTransID,
                                    qllDBCStmt            * aDBCStmt,
                                    qllStatement          * aSQLStmt,
                                    qllOptimizationNode   * aOptNode,
                                    qllDataArea           * aDataArea,
                                    qllEnv                * aEnv );

stlStatus qlndDataOptimizeSubQueryList( smlTransId              aTransID,
                                        qllDBCStmt            * aDBCStmt,
                                        qllStatement          * aSQLStmt,
                                        qllOptimizationNode   * aOptNode,
                                        qllDataArea           * aDataArea,
                                        qllEnv                * aEnv );

stlStatus qlndDataOptimizeSubQueryFunc( smlTransId              aTransID,
                                        qllDBCStmt            * aDBCStmt,
                                        qllStatement          * aSQLStmt,
                                        qllOptimizationNode   * aOptNode,
                                        qllDataArea           * aDataArea,
                                        qllEnv                * aEnv );

stlStatus qlndDataOptimizeSubQueryFilter( smlTransId              aTransID,
                                          qllDBCStmt            * aDBCStmt,
                                          qllStatement          * aSQLStmt,
                                          qllOptimizationNode   * aOptNode,
                                          qllDataArea           * aDataArea,
                                          qllEnv                * aEnv );

stlStatus qlndDataOptimizeConcat( smlTransId              aTransID,
                                  qllDBCStmt            * aDBCStmt,
                                  qllStatement          * aSQLStmt,
                                  qllOptimizationNode   * aOptNode,
                                  qllDataArea           * aDataArea,
                                  qllEnv                * aEnv );

stlStatus qlndDataOptimizeNestedLoopsJoin( smlTransId              aTransID,
                                           qllDBCStmt            * aDBCStmt,
                                           qllStatement          * aSQLStmt,
                                           qllOptimizationNode   * aOptNode,
                                           qllDataArea           * aDataArea,
                                           qllEnv                * aEnv );

stlStatus qlndDataOptimizeSortMergeJoin( smlTransId              aTransID,
                                         qllDBCStmt            * aDBCStmt,
                                         qllStatement          * aSQLStmt,
                                         qllOptimizationNode   * aOptNode,
                                         qllDataArea           * aDataArea,
                                         qllEnv                * aEnv );

stlStatus qlndDataOptimizeHashJoin( smlTransId              aTransID,
                                    qllDBCStmt            * aDBCStmt,
                                    qllStatement          * aSQLStmt,
                                    qllOptimizationNode   * aOptNode,
                                    qllDataArea           * aDataArea,
                                    qllEnv                * aEnv );

// stlStatus qlndDataOptimizeIndexJoin( smlTransId              aTransID,
//                                 qllDBCStmt            * aDBCStmt,
//                                 qllStatement          * aSQLStmt,
//                                 qllOptimizationNode   * aOptNode,
//                                 // qllDataArea           * aDataArea,
//                                 qllEnv                * aEnv );

// stlStatus qlndDataOptimizeConnectBy( smlTransId              aTransID,
//                                 qllDBCStmt            * aDBCStmt,
//                                 qllStatement          * aSQLStmt,
//                                 qllOptimizationNode   * aOptNode,
//                                 // qllDataArea           * aDataArea,
//                                 qllEnv                * aEnv );

// stlStatus qlndDataOptimizePseudoColumn( smlTransId              aTransID,
//                                    qllDBCStmt            * aDBCStmt,
//                                    qllStatement          * aSQLStmt,
//                                    qllOptimizationNode   * aOptNode,
//                                    // qllDataArea           * aDataArea,
//                                    qllEnv                * aEnv );

// stlStatus qlndDataOptimizeCount( smlTransId              aTransID,
//                             qllDBCStmt            * aDBCStmt,
//                             qllStatement          * aSQLStmt,
//                             qllOptimizationNode   * aOptNode,
//                             // qllDataArea           * aDataArea,
//                             qllEnv                * aEnv );

// stlStatus qlndDataOptimizeWindow( smlTransId              aTransID,
//                              qllDBCStmt            * aDBCStmt,
//                              qllStatement          * aSQLStmt,
//                              qllOptimizationNode   * aOptNode,
//                              // qllDataArea           * aDataArea,
//                              qllEnv                * aEnv );

// stlStatus qlndDataOptimizeHash( smlTransId              aTransID,
//                            qllDBCStmt            * aDBCStmt,
//                            qllStatement          * aSQLStmt,
//                            qllOptimizationNode   * aOptNode,
//                            // qllDataArea           * aDataArea,
//                            qllEnv                * aEnv );

stlStatus qlndDataOptimizeTableAccess( smlTransId              aTransID,
                                       qllDBCStmt            * aDBCStmt,
                                       qllStatement          * aSQLStmt,
                                       qllOptimizationNode   * aOptNode,
                                       qllDataArea           * aDataArea,
                                       qllEnv                * aEnv );

stlStatus qlndDataOptimizeIndexAccess( smlTransId              aTransID,
                                       qllDBCStmt            * aDBCStmt,
                                       qllStatement          * aSQLStmt,
                                       qllOptimizationNode   * aOptNode,
                                       qllDataArea           * aDataArea,
                                       qllEnv                * aEnv );

stlStatus qlndDataOptimizeRowIdAccess( smlTransId              aTransID,
                                       qllDBCStmt            * aDBCStmt,
                                       qllStatement          * aSQLStmt,
                                       qllOptimizationNode   * aOptNode,
                                       qllDataArea           * aDataArea,
                                       qllEnv                * aEnv );

// stlStatus qlndDataOptimizeFastDual( smlTransId              aTransID,
//                                qllDBCStmt            * aDBCStmt,
//                                qllStatement          * aSQLStmt,
//                                qllOptimizationNode   * aOptNode,
//                                // qllDataArea           * aDataArea,
//                                qllEnv                * aEnv );

#if 0
stlStatus qlndDataOptimizeFlatInstantAccess( smlTransId              aTransID,
                                             qllDBCStmt            * aDBCStmt,
                                             qllStatement          * aSQLStmt,
                                             qllOptimizationNode   * aOptNode,
                                             qllDataArea           * aDataArea,
                                             qllEnv                * aEnv );
#endif

stlStatus qlndDataOptimizeSortInstantAccess( smlTransId              aTransID,
                                             qllDBCStmt            * aDBCStmt,
                                             qllStatement          * aSQLStmt,
                                             qllOptimizationNode   * aOptNode,
                                             qllDataArea           * aDataArea,
                                             qllEnv                * aEnv );

#if 0
stlStatus qlndDataOptimizeHashInstantAccess( smlTransId              aTransID,
                                             qllDBCStmt            * aDBCStmt,
                                             qllStatement          * aSQLStmt,
                                             qllOptimizationNode   * aOptNode,
                                             qllDataArea           * aDataArea,
                                             qllEnv                * aEnv );
#endif

stlStatus qlndDataOptimizeGroupHashInstantAccess( smlTransId              aTransID,
                                                  qllDBCStmt            * aDBCStmt,
                                                  qllStatement          * aSQLStmt,
                                                  qllOptimizationNode   * aOptNode,
                                                  qllDataArea           * aDataArea,
                                                  qllEnv                * aEnv );

#if 0
stlStatus qlndDataOptimizeGroupSortInstantAccess( smlTransId              aTransID,
                                                  qllDBCStmt            * aDBCStmt,
                                                  qllStatement          * aSQLStmt,
                                                  qllOptimizationNode   * aOptNode,
                                                  qllDataArea           * aDataArea,
                                                  qllEnv                * aEnv );
#endif

stlStatus qlndDataOptimizeHashAggregation( smlTransId              aTransID,
                                           qllDBCStmt            * aDBCStmt,
                                           qllStatement          * aSQLStmt,
                                           qllOptimizationNode   * aOptNode,
                                           qllDataArea           * aDataArea,
                                           qllEnv                * aEnv );

stlStatus qlndDataOptimizeGroup( smlTransId              aTransID,
                                 qllDBCStmt            * aDBCStmt,
                                 qllStatement          * aSQLStmt,
                                 qllOptimizationNode   * aOptNode,
                                 qllDataArea           * aDataArea,
                                 qllEnv                * aEnv );

stlStatus qlndDataOptimizeSortJoinInstantAccess( smlTransId              aTransID,
                                                 qllDBCStmt            * aDBCStmt,
                                                 qllStatement          * aSQLStmt,
                                                 qllOptimizationNode   * aOptNode,
                                                 qllDataArea           * aDataArea,
                                                 qllEnv                * aEnv );

stlStatus qlndDataOptimizeHashJoinInstantAccess( smlTransId              aTransID,
                                                 qllDBCStmt            * aDBCStmt,
                                                 qllStatement          * aSQLStmt,
                                                 qllOptimizationNode   * aOptNode,
                                                 qllDataArea           * aDataArea,
                                                 qllEnv                * aEnv );

/** @} qlodDataOptNode */



/***************************************************
 * STATEMENT Nodes
 ***************************************************/ 

/*******************************************************************************
 * SELECT : Data Optimizaion
 ******************************************************************************/

/**
 * @defgroup qlodSelect SELECT Data Optimization
 * @ingroup qlod
 * @{
 */

stlStatus qloDataOptimizeSelect( smlTransId     aTransID,
                                 qllDBCStmt   * aDBCStmt,
                                 qllStatement * aSQLStmt,
                                 qllEnv       * aEnv );

/** @} qlodSelect */


/**
 * @defgroup qlodSelectInto SELECT INTO Data Optimization
 * @ingroup qlod
 * @{
 */

stlStatus qloDataOptimizeSelectInto( smlTransId     aTransID,
                                     qllDBCStmt   * aDBCStmt,
                                     qllStatement * aSQLStmt,
                                     qllEnv       * aEnv );

/** @} qlodSelectInto */


/*******************************************************************************
 * INSERT : Data Optimizaion 
 ******************************************************************************/

/**
 * @defgroup qlodInsert INSERT VALUES Data Optimization
 * @ingroup qlod
 * @{
 */

stlStatus qloDataOptimizeInsertValues( smlTransId     aTransID,
                                       qllDBCStmt   * aDBCStmt,
                                       qllStatement * aSQLStmt,
                                       qllEnv       * aEnv );

/** @} qlodInsert */


/**
 * @defgroup qlodInsertSelect INSERT .. SELECT Data Optimization
 * @ingroup qlod
 * @{
 */

stlStatus qloDataOptimizeInsertSelect( smlTransId     aTransID,
                                       qllDBCStmt   * aDBCStmt,
                                       qllStatement * aSQLStmt,
                                       qllEnv       * aEnv );

/** @} qlodInsertSelect */


/**
 * @defgroup qlodInsertReturnInto INSERT RETURNING INTO Data Optimization
 * @ingroup qlod
 * @{
 */

stlStatus qloDataOptimizeInsertReturnInto( smlTransId     aTransID,
                                           qllDBCStmt   * aDBCStmt,
                                           qllStatement * aSQLStmt,
                                           qllEnv       * aEnv );

/** @} qlodInsertReturnInto */


/**
 * @defgroup qlodInsertReturnQuery INSERT RETURNING Query Data Optimization
 * @ingroup qlod
 * @{
 */

stlStatus qloDataOptimizeInsertReturnQuery( smlTransId     aTransID,
                                            qllDBCStmt   * aDBCStmt,
                                            qllStatement * aSQLStmt,
                                            qllEnv       * aEnv );

/** @} qlodInsertReturnQuery */



/*******************************************************************************
 * UPDATE : Data Optimizaion
 ******************************************************************************/

/**
 * @defgroup qlodUpdate UPDATE Data Optimization
 * @ingroup qlod
 * @{
 */

stlStatus qloDataOptimizeUpdate( smlTransId     aTransID,
                                 qllDBCStmt   * aDBCStmt,
                                 qllStatement * aSQLStmt,
                                 qllEnv       * aEnv );

/** @} qlodUpdate */


/**
 * @defgroup qlodUpdateReturnInto UPDATE RETURNING INTO Data Optimization
 * @ingroup qlod
 * @{
 */

stlStatus qloDataOptimizeUpdateReturnInto( smlTransId     aTransID,
                                           qllDBCStmt   * aDBCStmt,
                                           qllStatement * aSQLStmt,
                                           qllEnv       * aEnv );

/** @} qlodUpdateReturnInto */


/**
 * @defgroup qlodUpdateReturnQuery UPDATE RETURNING Query Data Optimization
 * @ingroup qlod
 * @{
 */

stlStatus qloDataOptimizeUpdateReturnQuery( smlTransId     aTransID,
                                            qllDBCStmt   * aDBCStmt,
                                            qllStatement * aSQLStmt,
                                            qllEnv       * aEnv );

/** @} qlodUpdateReturnQuery */



/**
 * @defgroup qlodUpdatePositioned UPDATE CURRENT OF Data Optimization
 * @ingroup qlod
 * @{
 */

stlStatus qloDataOptimizeUpdatePositioned( smlTransId     aTransID,
                                           qllDBCStmt   * aDBCStmt,
                                           qllStatement * aSQLStmt,
                                           qllEnv       * aEnv );

/** @} qlodUpdatePositioned */



/*******************************************************************************
 * DELETE : Data Optimizaion
 ******************************************************************************/

/**
 * @defgroup qlodDelete DELETE Data Optimization
 * @ingroup qlod
 * @{
 */

stlStatus qloDataOptimizeDelete( smlTransId     aTransID,
                                 qllDBCStmt   * aDBCStmt,
                                 qllStatement * aSQLStmt,
                                 qllEnv       * aEnv );

/** @} qlodDelete */


/**
 * @defgroup qlodDeleteReturnInto DELETE RETURNING INTO Data Optimization
 * @ingroup qlod
 * @{
 */

stlStatus qloDataOptimizeDeleteReturnInto( smlTransId     aTransID,
                                           qllDBCStmt   * aDBCStmt,
                                           qllStatement * aSQLStmt,
                                           qllEnv       * aEnv );

/** @} qlodDeleteReturnInto */


/**
 * @defgroup qlodDeleteReturnQuery DELETE RETURNING QUERY Data Optimization
 * @ingroup qlod
 * @{
 */

stlStatus qloDataOptimizeDeleteReturnQuery( smlTransId     aTransID,
                                            qllDBCStmt   * aDBCStmt,
                                            qllStatement * aSQLStmt,
                                            qllEnv       * aEnv );

/** @} qlodDeleteReturnQuery */



/**
 * @defgroup qlodDeletePositioned DELETE CURRENT OF Data Optimization
 * @ingroup qlod
 * @{
 */

stlStatus qloDataOptimizeDeletePositioned( smlTransId     aTransID,
                                           qllDBCStmt   * aDBCStmt,
                                           qllStatement * aSQLStmt,
                                           qllEnv       * aEnv );

/** @} qlodDeletePositioned */



/*******************************************************************************
 * Expression : Data Optimizaion
 ******************************************************************************/

/**
 * @defgroup qlodExpression Expression Data Optimization
 * @ingroup qlod
 * @{
 */

stlStatus qloDataOptimizeExpressionAll( qloDataOptExprInfo  * aDataOptInfo,
                                        qllEnv              * aEnv );

stlStatus qloDataOptimizeExpression( qloDataOptExprInfo  * aDataOptInfo,
                                     qlvInitExpression   * aInitExpr,
                                     qllEnv              * aEnv );

stlStatus qloDataOptimizeValue( qloDataOptExprInfo  * aDataOptInfo,
                                qlvInitExpression   * aInitExpr,
                                qllEnv              * aEnv );

stlStatus qloDataOptimizeBindParam( qloDataOptExprInfo  * aDataOptInfo,
                                    qlvInitExpression   * aInitExpr,
                                    qllEnv              * aEnv );

stlStatus qloDataOptimizeColumn( qloDataOptExprInfo  * aDataOptInfo,
                                 qlvInitExpression   * aInitExpr,
                                 qllEnv              * aEnv );

stlStatus qloDataOptimizeFunction( qloDataOptExprInfo  * aDataOptInfo,
                                   qlvInitExpression   * aInitExpr,
                                   qllEnv              * aEnv );

// stlStatus qloDataOptimizeCast( qloDataOptExprInfo  * aDataOptInfo,
//                                qlvInitExpression   * aInitExpr,
//                                qllEnv              * aEnv );

stlStatus qloDataOptimizePseudoCol( qloDataOptExprInfo  * aDataOptInfo,
                                    qlvInitExpression   * aInitExpr,
                                    qllEnv              * aEnv );

stlStatus qloDataOptimizePseudoArg( qloDataOptExprInfo  * aDataOptInfo,
                                    qlvInitExpression   * aInitExpr,
                                    qllEnv              * aEnv );

stlStatus qloDataOptimizeConstExpr( qloDataOptExprInfo  * aDataOptInfo,
                                    qlvInitExpression   * aInitExpr,
                                    qllEnv              * aEnv );

stlStatus qloDataOptimizeReference( qloDataOptExprInfo  * aDataOptInfo,
                                    qlvInitExpression   * aInitExpr,
                                    qllEnv              * aEnv );

stlStatus qloDataOptimizeSubQuery( qloDataOptExprInfo  * aDataOptInfo,
                                   qlvInitExpression   * aInitExpr,
                                   qllEnv              * aEnv );

stlStatus qloDataOptimizeInnerColumn( qloDataOptExprInfo  * aDataOptInfo,
                                      qlvInitExpression   * aInitExpr,
                                      qllEnv              * aEnv );

stlStatus qloDataOptimizeOuterColumn( qloDataOptExprInfo  * aDataOptInfo,
                                      qlvInitExpression   * aInitExpr,
                                      qllEnv              * aEnv );

stlStatus qloDataOptimizeRowIdColumn( qloDataOptExprInfo  * aDataOptInfo,
                                      qlvInitExpression   * aInitExpr,
                                      qllEnv              * aEnv );

stlStatus qloDataOptimizeAggregation( qloDataOptExprInfo  * aDataOptInfo,
                                      qlvInitExpression   * aInitExpr,
                                      qllEnv              * aEnv );

// stlStatus qloDataOptimizeCaseExpr( qloDataOptExprInfo  * aDataOptInfo,
//                                    qlvInitExpression   * aInitExpr,
//                                    qllEnv              * aEnv );

stlStatus qloDataOptimizeListFunction( qloDataOptExprInfo  * aDataOptInfo,
                                       qlvInitExpression   * aInitExpr,
                                       qllEnv              * aEnv );

stlStatus qloDataOptimizeListColumn( qloDataOptExprInfo  * aDataOptInfo,
                                     qlvInitExpression   * aInitExpr,
                                     qllEnv              * aEnv );

stlStatus qloDataOptimizeRowExpr( qloDataOptExprInfo  * aDataOptInfo,
                                  qlvInitExpression   * aInitExpr,
                                  qllEnv              * aEnv );

stlStatus qloSetDataValueFromString( qllStatement         * aSQLStmt,
                                     dtlDataValue        ** aDataValue,
                                     dtlDataType            aDataType,
                                     stlInt64               aPrecision,
                                     stlInt64               aScale,
                                     dtlStringLengthUnit    aStringLengthUnit,
                                     dtlIntervalIndicator   aIntervalIndicator,
                                     stlChar              * aString,
                                     knlRegionMem         * aRegionMem,
                                     qllEnv               * aEnv );

stlStatus qloSetDataValueFromDateTimeLiteral( qllStatement         * aSQLStmt,
                                              dtlDataValue        ** aDataValue,
                                              dtlDataType            aDataType,
                                              stlInt64               aPrecision,
                                              stlInt64               aScale,
                                              dtlStringLengthUnit    aStringLengthUnit,
                                              dtlIntervalIndicator   aIntervalIndicator,
                                              stlChar              * aString,
                                              knlRegionMem         * aRegionMem,
                                              qllEnv               * aEnv );

stlStatus qloSetDataValueFromInteger( qllStatement   * aSQLStmt,
                                      dtlDataValue  ** aDataValue,
                                      dtlDataType      aDataType,
                                      stlInt64         aPrecision,
                                      stlInt64         aScale,
                                      stlInt64         aInteger,
                                      knlRegionMem   * aRegionMem,
                                      qllEnv         * aEnv );

stlStatus qloSetNullDataValue( dtlDataValue         ** aDataValue,
                               dtlDataType             aDataType,
                               stlInt64                aPrecision,
                               dtlStringLengthUnit     aStringLengthUnit,
                               knlRegionMem          * aRegionMem,
                               qllEnv                * aEnv );

stlStatus qloGetStmtFunctionBlock( knlValueBlockList   * aStmtValueBlock,
                                   knlBuiltInFunc        aFuncID,
                                   knlValueBlockList  ** aFuncValueBlock,
                                   qllEnv              * aEnv );

stlStatus qloDataSetInternalCastInfo( qllStatement      * aSQLStmt,
                                      knlRegionMem      * aRegionMem,
                                      qloDBType         * aDBType,
                                      knlCastFuncInfo  ** aCastFuncInfo,
                                      stlBool             aIsAllocCastFuncInfo,
                                      stlBool             aIsAllocCastResultBuf,
                                      qllEnv            * aEnv );

stlStatus qloDataSetInternalCastInfoBetweenValueBlock( qllStatement      * aSQLStmt,
                                                       knlValueBlock     * aSrcValueBlock,
                                                       knlValueBlock     * aDstValueBlock,
                                                       knlCastFuncInfo  ** aCastFuncInfo,
                                                       knlRegionMem      * aRegionMem,
                                                       qllEnv            * aEnv );

/** @} qlodExpression */



/*******************************************************************************
 * Candidate Plan
 ******************************************************************************/

/**
 * @defgroup qlopCandidate Candidate Plan 
 * @ingroup qlop
 * @{
 */


/**
 * @brief index에 대한 range/key-filter 및 out-filter 후보 expression 정보
 */
// typedef struct qloIndexScan
// {
//     stlBool               mIsExist;                 /**< Index Scan 생성 여부 */
//     stlBool               mEmptyValue;              /**< Index Scan 구축 불가시 반환 값
//                                                      * STL_TRUE  : Table Scan을 유도
//                                                      * STL_FALSE : 조건절의 결과는 항상 FALSE
//                                                      */
//     stlInt32              mKeyColCount;             /**< array count */
//     stlInt32              mReadKeyCnt;              /**< Composite Index 등에서 읽을 key 의 개수 */

//     // qlvInitRefColList   * mReadColumnList;/**< Read Column 목록 */
//     // knlPredicateList    * mRangePred;     /**< range predicate */

//     qlvRefExprList      * mMinRangeExpr;            /**< min-range expression list */
//     qlvRefExprList      * mMaxRangeExpr;            /**< max-range expression list */
    
//     qlvInitExpression   * mLogicalKeyFilterExpr;    /**< logical key-filter expression */
//     qlvInitExpression   * mLogicalTableFilterExpr;  /**< logical table-filter expression */

//     qlvInitExpression   * mPhysicalKeyFilterExpr;   /**< physical key-filter expression */
//     qlvInitExpression   * mPhysicalTableFilterExpr; /**< physical table-filter expression list */

//     /* range 없이 key-filter만 만들어 질 수도 있음 */
// } qloIndexScan;


typedef struct qloIndexScanInfo
{
    stlBool              mIsExist;                      /**< Index Scan 생성 여부 */
    stlBool              mEmptyValue;                   /**< Index Scan 구축 불가시 반환 값
                                                         * STL_TRUE  : Table Scan을 유도
                                                         * STL_FALSE : 조건절의 결과는 항상 FALSE
                                                         */
    stlInt32             mKeyColCount;                  /**< array count */
    stlInt32             mReadKeyCnt;                   /**< Composite Index 등에서 읽을 key 의 개수 */

    qlvRefExprList    ** mMinRangeExpr;                 /**< min-range expression list */
    qlvRefExprList    ** mMaxRangeExpr;                 /**< max-range expression list */
                                       
    qlvRefExprList     * mLogicalKeyFilterExprList;     /**< logical key-filter expression list */
    qlvRefExprList     * mLogicalTableFilterExprList;   /**< logical table-filter expression list */

    qlvRefExprList     * mPhysicalKeyFilterExprList;    /**< physical key-filter expression list */
    qlvRefExprList     * mPhysicalTableFilterExprList;  /**< physical table-filter expression list */

    qlvInitExpression  * mLogicalKeyFilter;             /**< logical key-filter expression list */
    qlvInitExpression  * mLogicalTableFilter;           /**< logical table-filter expression list */

    qlvInitExpression  * mNullValueExpr;                /**< NULL value expression (Constant Value) */
    qlvInitExpression  * mFalseValueExpr;               /**< FALSE value expression (Constant Value) */
} qloIndexScanInfo;


typedef struct qloHashScanInfo
{
    stlInt32             mHashColCount;                 /**< array count */

    qlvRefExprList    ** mHashFilterExpr;               /**< hash-filter expression list */

    qlvRefExprList     * mLogicalTableFilterExprList;   /**< logical table-filter expression list */
    qlvRefExprList     * mPhysicalTableFilterExprList;  /**< physical table-filter expression list */

    qlvInitExpression  * mLogicalTableFilter;           /**< logical table-filter expression list */
} qloHashScanInfo;


/**********************************************
 * Range 관련 Functions
 **********************************************/

stlStatus qloGetKeyRange( stlChar             * aSQLString,
                          ellDictHandle       * aIndexHandle,
                          qlvInitNode         * aInitNode, 
                          knlBindContext      * aBindContext,
                          qlvInitExpression   * aInitExprCode,
                          qloIndexScanInfo   ** aIndexScanInfo,
                          knlRegionMem        * aRegionMem,
                          qllEnv              * aEnv );


stlStatus qloClassifyPredExpr( stlChar             * aSQLString,
                               ellDictHandle       * aIndexHandle,
                               qlvInitNode         * aInitNode,
                               knlBindContext      * aBindContext,
                               qlvInitExpression   * aInitExprCode,
                               qloIndexScanInfo    * aIndexScanInfo,
                               qlvRefExprList      * aIndexColExprList,
                               qlvRefExprList      * aTableColExprList,
                               knlRegionMem        * aRegionMem,
                               qllEnv              * aEnv );

stlStatus qloExtractRange( ellDictHandle       * aIndexHandle,
                           qlvInitNode         * aInitNode,
                           qloIndexScanInfo    * aIndexScanInfo,
                           knlRegionMem        * aRegionMem,
                           qllEnv              * aEnv );


stlStatus qloLinkRangeExpr( qlvInitExpression  ** aExpr,
                            qlvRefExprList      * aExprList,
                            knlRegionMem        * aRegionMem,
                            qllEnv              * aEnv );

stlStatus qloGetRangeFromExpr( qlvInitExpression  * aExpr,
                               stlUInt32            aNullOffset,
                               stlUInt32            aFalseOffset,
                               stlUInt32          * aKeyColOffset,
                               stlUInt32          * aRangeValOffset,
                               stlBool            * aIsIncludeEqual,
                               stlBool            * aIsNullAlwaysStop,
                               stlBool            * aIsLikeFunc,
                               qllEnv             * aEnv );

stlStatus qloGetRefColListForFilter( ellDictHandle       * aIndexHandle,
                                     qlvInitNode         * aInitNode,
                                     qlvInitExpression   * aInitExprCode,
                                     qlvRefExprList      * aIndexColExprList,
                                     qlvRefExprList      * aTableColExprList,
                                     stlBool             * aHasRowIdCol,
                                     knlRegionMem        * aRegionMem,
                                     qllEnv              * aEnv );


stlStatus qloGetKeyHash( stlChar            * aSQLString,
                         ellDictHandle      * aIndexHandle,
                         qlvInitNode        * aInitNode, 
                         knlBindContext     * aBindContext,
                         qlvInitExpression  * aHashKeyExprCode,
                         qlvInitExpression  * aNonHashKeyExprCode,
                         qloHashScanInfo   ** aHashScanInfo,
                         knlRegionMem       * aRegionMem,
                         qllEnv             * aEnv );

stlStatus qloClassifyHashPredExpr( stlChar              * aSQLString,
                                   qlvInitNode          * aInitNode,
                                   knlBindContext       * aBindContext,
                                   qlvInitExpression    * aInitExprCode,
                                   stlBool                aIsHashKeyFilter,
                                   qloHashScanInfo      * aHashScanInfo,
                                   qlvRefExprList       * aHashColExprList,
                                   qlvRefExprList       * aTableColExprList,
                                   knlRegionMem         * aRegionMem,
                                   qllEnv               * aEnv );


/**********************************************
 * 기타 Functions
 **********************************************/

stlStatus qloGetDataType( stlChar            * aSQLString,
                          qlvInitExpression  * aInitExprCode,
                          knlBindContext     * aBindContext,
                          dtlDataType        * aDataType,
                          knlRegionMem       * aRegionMem,
                          qllEnv             * aEnv );

void qloRemoveExprAND( qlvInitExpression   * aExpr,
                       qlvInitExpression  ** aExprList,
                       stlInt32            * aExprCount );

void qloGetPhysicalFuncPtr( knlBuiltInFunc     aExprFuncId,
                            dtlDataType        aType1,
                            dtlDataType        aType2,
                            dtlPhysicalFunc  * aPhysicalFuncPtr,
                            stlBool            aReverse );

/** @} qlopCandidate */



#endif /* _QLOOPTIMIZATION_H_ */

