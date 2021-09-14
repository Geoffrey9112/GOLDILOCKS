/*******************************************************************************
 * qlnoNode.h
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

#ifndef _QLNONODE_H_
#define _QLNONODE_H_ 1

/**
 * @file qlnoNode.h
 * @brief SQL Processor Layer Optimization Node
 */

#include <qlDef.h>

/**
 * @addtogroup qlno
 * @{
 */


/***************************************************
 * STATEMENT Nodes
 ***************************************************/ 

extern const qlnoCodeOptNodeFuncPtr qlnoCodeOptNodeFuncList[ QLNC_PLAN_TYPE_MAX ];
extern const qlnoAddExprNodeFuncPtr qlnoAddExprNodeFuncList[ QLL_PLAN_NODE_TYPE_MAX ];
extern const qlnoCompleteNodeFuncPtr qlnoCompleteNodeFuncList[ QLL_PLAN_NODE_TYPE_MAX ];
extern const qlndDataOptimizeNodeFuncPtr qlndDataOptNodeFuncList[ QLL_PLAN_NODE_TYPE_MAX ];


#define QLNO_CODE_OPTIMIZE( _aCodeOptParamInfo, _aEnv )                                             \
    {                                                                                               \
        STL_DASSERT( ( (_aCodeOptParamInfo)->mCandPlan->mPlanType >= QLNC_PLAN_TYPE_QUERY_SET ) &&  \
                     ( (_aCodeOptParamInfo)->mCandPlan->mPlanType < QLNC_PLAN_TYPE_MAX ) );         \
        STL_DASSERT( qlnoCodeOptNodeFuncList[ (_aCodeOptParamInfo)->mCandPlan->mPlanType ] != NULL );\
        STL_TRY( qlnoCodeOptNodeFuncList[ (_aCodeOptParamInfo)->mCandPlan->mPlanType ]( (_aCodeOptParamInfo),\
                                                                                        (_aEnv) )   \
                 == STL_SUCCESS );                                                                  \
    }

#define QLNO_ADD_EXPR( aDBCStmt, aSQLStmt, aNodeList, aOptNode, aQueryExprList, aEnv )  \
    {                                                                                   \
        STL_DASSERT( ( (aOptNode)->mType >= QLL_PLAN_NODE_STMT_SELECT_TYPE ) &&         \
                     ( (aOptNode)->mType < QLL_PLAN_NODE_TYPE_MAX ) );                  \
        STL_DASSERT( qlnoAddExprNodeFuncList[ (aOptNode)->mType ] != NULL );            \
        STL_TRY( qlnoAddExprNodeFuncList[ (aOptNode)->mType ]( (aDBCStmt),              \
                                                               (aSQLStmt),              \
                                                               (aNodeList),             \
                                                               (aOptNode),              \
                                                               (aQueryExprList),        \
                                                               (aEnv) )                 \
                 == STL_SUCCESS );                                                      \
    }

#define QLNO_COMPLETE( aDBCStmt, aSQLStmt, aNodeList, aOptNode, aQueryExprList, aEnv )  \
    {                                                                                   \
        STL_DASSERT( ( (aOptNode)->mType >= QLL_PLAN_NODE_STMT_SELECT_TYPE ) &&         \
                     ( (aOptNode)->mType < QLL_PLAN_NODE_TYPE_MAX ) );                  \
        STL_DASSERT( qlnoCompleteNodeFuncList[ (aOptNode)->mType ] != NULL );           \
        STL_TRY( qlnoCompleteNodeFuncList[ (aOptNode)->mType ]( (aDBCStmt),             \
                                                                (aSQLStmt),             \
                                                                (aNodeList),            \
                                                                (aOptNode),             \
                                                                (aQueryExprList),       \
                                                                (aEnv) )                \
                 == STL_SUCCESS );                                                      \
    }


#define QLND_DATA_OPTIMIZE( _aTransID, _aDBCStmt, _aSQLStmt, _aNode, _aDataArea, _aEnv )\
    {                                                                                   \
        STL_DASSERT( ( (_aNode)->mType >= QLL_PLAN_NODE_STMT_SELECT_TYPE ) &&           \
                     ( (_aNode)->mType < QLL_PLAN_NODE_TYPE_MAX ) );                    \
        STL_DASSERT( qlndDataOptNodeFuncList[ (_aNode)->mType ] != NULL );              \
        STL_TRY( qlndDataOptNodeFuncList[ (_aNode)->mType ]( (_aTransID),               \
                                                             (_aDBCStmt),               \
                                                             (_aSQLStmt),               \
                                                             (_aNode),                  \
                                                             (_aDataArea),              \
                                                             (_aEnv) )                  \
                 == STL_SUCCESS );                                                      \
    }


/**
 * @brief DML STATEMENT Optimization Node
 * @remark SELECT / DELETE / UPDATE / INSERT statement는
 *    <BR> qlnoDmlStmt구조로 Mapping 될 수 있다.
 */
struct qlnoDmlStmt
{
    /* Cached Init Plan Node */
    qlvInitNode          * mCachedInitNode;      /**< Cached Init Plan Node */

    /* Total Nodes */ 
    qllOptNodeList       * mOptNodeList;         /**< 전체 Optimization Node List */

    /* Table Statistic List */
    qloValidTblStatList  * mValidTblStatList;    /**< Recompile 체크를 위한 table statistic 정보 리스트 */
};


/**
 * @brief SELECT STATEMENT Optimization Node
 */
struct qlnoSelectStmt
{
    /* Cached Init Plan Node */
    qlvInitNode          * mCachedInitNode;      /**< Cached Init Plan Node */

    /* Total Nodes */ 
    qllOptNodeList       * mOptNodeList;         /**< 전체 Optimization Node List */

    /* Table Statistic List */
    qloValidTblStatList  * mValidTblStatList;    /**< Recompile 체크를 위한 table statistic 정보 리스트 */

    /* Query Node */
    qllOptimizationNode  * mQueryNode;           /**< Query Optimization Node */

    /* INTO 절 관련 */
    stlInt32               mTargetCnt;           /**< Target Column Count */ 
    qlvRefExprList       * mTargetColList;       /**< Target Column List */
    qlvInitTarget        * mTargets;             /**< Target List : array */
    qlvInitExpression    * mIntoTargetArray;     /**< INTO clause target array */

    /* Value Block 관련 */
    qloInitStmtExprList  * mStmtExprList;        /**< Statement 단위 Expression 정보 */

    /* OFFSET .. LIMIT 절 관련 */
    stlInt64               mSkipCnt;             /**< Skip(Offset) Count */
    stlInt64               mFetchCnt;            /**< Fetch Count */
    qlvInitExpression    * mResultSkip;          /**< Skip(Offset) Expression */
    qlvInitExpression    * mResultLimit;         /**< Limit(Fetch) Expression */

    /* Result Set 생성을 위한 Cursor 정보 */
    qlvRefTableList      * mScanTableList;       /**< scan table list */
    qlvRefTableList      * mLockTableList;       /**< lock table list */

    /*
     * Complete Node Function에서 구축할 정보
     */

    /* Target Descriptor 관련 */
    qllTarget            * mTargetInfo;          /**< Target Info */
};


/**
 * @brief INSERT STATEMENT Optimization Node
 */
struct qlnoInsertStmt
{
    /* Cached Init Plan Node */
    qlvInitNode            * mCachedInitNode;        /**< Cached Init Plan Node */

    /* Total Nodes */ 
    qllOptNodeList         * mOptNodeList;           /**< 전체 Optimization Node List */

    /* Table Statistic List */
    qloValidTblStatList  * mValidTblStatList;    /**< Recompile 체크를 위한 table statistic 정보 리스트 */

    /* Sub-Query Node */
    stlBool                  mHasQuery;              /**< INSERT SELECT 구문인지 여부 */
    qllOptimizationNode    * mQueryNode;             /**< Query Optimization Node */

    /* Insert Values 관련 */
    stlInt32                 mInsertColumnCnt;       /**< Insert 할 Column 개수 */
    stlInt32                 mInsertRowCount;        /**< Insert 할 Row 개수 */
    qlvRefExprList         * mInsertColExprList;     /**< Insert 할 Column Expression List */
    knlExprStack          ** mInsertValueStackList;  /**< multi row : array[row][column] */
    qlvInitExpression     ** mInsertValueExpr;       /**< Insert 할 Value Expression List */
    stlInt32                 mInsertValueCount;      /**< Insert 할 Value Expression Count */

    qllOptimizationNode    * mInsertValueChildNode;  /**< Insert Values에 존재하는 SubQuery Expression을 위한 Child Node */

    /* Table 관련 */
    stlChar                * mTableName;             /**< Base Table Name */
    ellDictHandle          * mTableHandle;           /**< Table Handle */
    ellDictHandle          * mColumnHandle;          /**< Column Handle */
    void                   * mPhysicalHandle;        /**< Table Physical Handle */

    /* Value Block 관련 */
    qloInitStmtExprList    * mStmtExprList;          /**< Statement 단위 Expression 정보 */
    qloInitExprList        * mQueryExprList;         /**< Query 단위 Expression 정보 */

    qlvInitExpression      * mRowIdColumn;           /**< RowId Column : Returning절에 올 수 있음. */

    /* Related Object 관련 */
    stlBool                  mHasCheck;              /**< 관련 Not NULL 또는 Check Constraint 여부 */
    stlBool                  mHasIndex;              /**< 관련 PK/UK/FK Constraint 또는 Unique/Non-Unique Index 여부 */
    qlvInitInsertRelObject * mRelObject;             /**< Insert 대상 Related Object의 Init Plan */

    /* RETURN INTO 절 관련 */
    stlInt32                 mReturnExprCnt;         /**< RETURN clause Expression Count */ 
    qlvRefExprList         * mReturnExprList;        /**< RETURN clause Expression List */
    qlvInitTarget          * mReturnTarget;          /**< RETURN target array */
    qlvInitExpression      * mIntoTargetArray;       /**< INTO clause Target Array */

    qlvRefExprList         * mReturnOuterColumnList; /**< Return절에 있는 Outer Column들의 List */
    qllOptimizationNode    * mReturnChildNode;       /**< Return절에 존재하는 SubQuery Expression을 위한 Child Node */

    /*
     * Complete Node Function에서 구축할 정보
     */

    /* RETURN INTO 절 관련 */
    knlExprStack           * mReturnExprStack;       /**< RETURN clause Expression Stack */

    /* Target Descriptor 관련 */
    qllTarget              * mTargetInfo;            /**< Target Info */
};


/**
 * @brief UPDATE STATEMENT Optimization Node
 */
struct qlnoUpdateStmt
{
    /* Cached Init Plan Node */
    qlvInitNode            * mCachedInitNode;        /**< Cached Init Plan Node */

    /* Total Nodes */ 
    qllOptNodeList         * mOptNodeList;           /**< 전체 Optimization Node List */

    /* Table Statistic List */
    qloValidTblStatList  * mValidTblStatList;    /**< Recompile 체크를 위한 table statistic 정보 리스트 */

    /* Update Values 관련 */
    stlInt32                 mSetColumnCnt;          /**< SET Column 개수 */
    qlvInitExpression     ** mSetExprArray;          /**< SET Column Value Expression Array */
    qlvRefExprList         * mSetColExprList;        /**< SET Column Expression List */

    qlvRefExprList         * mSetOuterColumnList;    /**< Set절에 있는 Outer Column들의 List */
    qllOptimizationNode    * mSetChildNode;          /**< Set절에 존재하는 SubQuery Expression을 위한 Child Node */

    /* Table 의 전체 Column Count 만큼 Array 로 관리됨 */
    stlInt32                 mTotalColumnCnt;        /**< Total Column Count */
    stlInt32               * mSetCodeStackIdx;       /**< SetCodeStack의 idx번호 (Used Column 여부 포함) */
    
    /* Table 관련 */
    stlChar                * mTableName;             /**< Base Table Name */
    ellDictHandle          * mTableHandle;           /**< Table Handle */
    ellDictHandle          * mSetColumnHandle;       /**< Column Handle */
    void                   * mPhysicalHandle;        /**< Table Physical Handle */

    /* Scan 관련 */
    qllOptimizationNode    * mScanNode;              /**< Scan Node */
    
    /* Value Block 관련 */
    qloInitStmtExprList    * mStmtExprList;          /**< Statement 단위 Expression 정보 */
    qloInitExprList        * mQueryExprList;         /**< Query 단위 Expression 정보 */

    qlvInitExpression      * mRowIdColumn;           /**< Write Table RowId Column */

    /* Update 대상 Table 관련 */
    qlvInitNode            * mReadTableNode;         /**< Table Node for Reading */
    qlvInitNode            * mWriteTableNode;        /**< Table Node for Writing  */

    /* Related Object 관련 */
    stlBool                  mHasCheck;              /**< 관련 Not NULL 또는 Check Constraint 여부 */
    stlBool                  mHasIndex;              /**< 관련 PK/UK/FK Constraint 또는 Unique/Non-Unique Index 여부 */
    qlvInitUpdateRelObject * mRelObject;             /**< Update 대상 Related Object의 Init Plan */

    /* OFFSET .. LIMIT 절 관련 */
    stlInt64                 mSkipCnt;               /**< Skip(Offset) Count */
    stlInt64                 mFetchCnt;              /**< Fetch Count */
    qlvInitExpression      * mResultSkip;            /**< Skip(Offset) Expression */
    qlvInitExpression      * mResultLimit;           /**< Limit(Fetch) Expression */

    /* RETURN INTO 절 관련 */
    stlInt32                 mReturnExprCnt;         /**< RETURN clause Expression Count */ 
    qlvRefExprList         * mReturnExprList;        /**< RETURN clause Expression List */
    qlvInitTarget          * mReturnTarget;          /**< RETURN target array */
    qlvInitExpression      * mIntoTargetArray;       /**< INTO clause Target Array */
    qlvRefExprList         * mShareReadColList;      /**< Share Read Column Expression List For RETURN clause */
    qlvRefExprList         * mShareWriteColList;     /**< Share Write Column Expression List For RETURN clause */

    qlvRefExprList         * mReturnOuterColumnList; /**< Return절에 있는 Outer Column들의 List */
    qllOptimizationNode    * mReturnChildNode;       /**< Return절에 존재하는 SubQuery Expression을 위한 Child Node */

    /*
     * Complete Node Function에서 구축할 정보
     */
    
    /* SET 관련 */
    knlExprStack           * mSetCodeStack;          /**< SET 절 Expression Stack: array */

    /* RETURN INTO 절 관련 */
    knlExprStack           * mReturnExprStack;       /**< RETURN clause Expression Stack */

    /* Target Descriptor 관련 */
    qllTarget              * mTargetInfo;            /**< Target Info */
};


/**
 * @brief DELETE STATEMENT Optimization Node
 */
struct qlnoDeleteStmt
{
    /* Cached Init Plan Node */
    qlvInitNode            * mCachedInitNode;        /**< Cached Init Plan Node */

    /* Total Nodes */ 
    qllOptNodeList         * mOptNodeList;           /**< 전체 Optimization Node List */

    /* Table Statistic List */
    qloValidTblStatList  * mValidTblStatList;    /**< Recompile 체크를 위한 table statistic 정보 리스트 */

    /* Table 관련 */
    stlChar                * mTableName;             /**< Base Table Name */
    ellDictHandle          * mTableHandle;           /**< Table Handle */
    void                   * mPhysicalHandle;        /**< Table Physical Handle */

    /* Scan 관련 */
    qllOptimizationNode    * mScanNode;              /**< Scan Node */

    /* Value Block 관련 */
    qloInitStmtExprList    * mStmtExprList;          /**< Statement 단위 Expression 정보 */
    qloInitExprList        * mQueryExprList;         /**< Query 단위 Expression 정보 */

    /* Delete 대상 Table 관련 */
    qlvInitNode            * mReadTableNode;         /**< Table Node for Reading */

    /* Related Object 관련 */
    stlBool                  mHasIndex;          /**< 관련 PK/UK/FK Constraint 또는 Unique/Non-Unique Index 여부 */
    stlBool                  mHasCheck;          /**< 관련 deferrable NOT NULL / CHECK Constraint  */
    
    qlvInitDeleteRelObject * mRelObject;             /**< Delete 대상 Related Object의 Init Plan */

    /* OFFSET .. LIMIT 절 관련 */
    stlInt64                 mSkipCnt;               /**< Skip(Offset) Count */
    stlInt64                 mFetchCnt;              /**< Fetch Count */
    qlvInitExpression      * mResultSkip;            /**< Skip(Offset) Expression */
    qlvInitExpression      * mResultLimit;           /**< Limit(Fetch) Expression */

    /* RETURN INTO 절 관련 */
    stlInt32                 mReturnExprCnt;         /**< RETURN clause Expression Count */ 
    qlvRefExprList         * mReturnExprList;        /**< RETURN clause Expression List */
    qlvInitTarget          * mReturnTarget;          /**< RETURN target array */
    qlvInitExpression      * mIntoTargetArray;       /**< INTO clause Target Array */

    qlvRefExprList         * mReturnOuterColumnList; /**< Return절에 있는 Outer Column들의 List */
    qllOptimizationNode    * mReturnChildNode;       /**< Return절에 존재하는 SubQuery Expression을 위한 Child Node */

    /*
     * Complete Node Function에서 구축할 정보
     */

    /* RETURN INTO 절 관련 */
    knlExprStack           * mReturnExprStack;       /**< RETURN clause Expression Stack */

    /* Target Descriptor 관련 */
    qllTarget              * mTargetInfo;            /**< Target Info */
};

    
/***************************************************
 * QUERY EXPRESSION Nodes
 ***************************************************/ 

/**
 * @brief QUERY SET optiomization node
 */
struct qlnoQuerySet
{
    /* Child Node */
    qllOptimizationNode  * mChildNode;             /**< Child Optimization Node */

    stlBool                mNeedRowBlock;          /**< Row Block이 필요한지 여부 */
    stlBool                mIsRootSet;             /**< Root Query Set 여부 */
    stlBool                mNeedRebuild;           /**< Rebuild가 필요한지 여부 */
    
    /* Target */
    stlInt32               mTargetCount;           /**< Target Count */
    qlvRefExprList       * mTargetList;            /**< Target List */
    qlvInitTarget        * mSetTargets;            /**< Set Expression Target List : array */

    /* Set Relation Column */
    qlvRefExprList       * mColumnList;            /**< Set Column List */
    
    /* OFFSET .. LIMIT 절 관련 */
    stlInt64               mSkipCnt;               /**< Skip(Offset) Count */
    stlInt64               mFetchCnt;              /**< Fetch Count */
    qlvInitExpression    * mResultSkip;            /**< Skip(Offset) Expression */
    qlvInitExpression    * mResultLimit;           /**< Limit(Fetch) Expression */
};

/**
 * @brief Set Operation node
 */
struct qlnoSetOP
{
    stlBool                mNeedRowBlock;          /**< Row Block이 필요한지 여부 */
    stlBool                mNeedRebuild;           /**< Rebuild가 필요한지 여부 */

    /* Set Operator */
    stlUInt8               mSetType;               /**< Set Operator Type : qlvSetType */
    stlUInt8               mSetOption;             /**< Set Option : qlvSetOption */

    /* Query Set */
    stlInt32               mRootQuerySetIdx;       /**< Root Query Set Idx */
    qlnoQuerySet         * mRootQuerySet;          /**< Root Query Set */
    
    /* Child Node */
    stlInt32               mChildCount;            /**< Child Count, >= 2 */
    stlInt32               mCurChildIdx;           /**< only on code optimization */
    qllOptimizationNode ** mChildNodeArray;        /**< Child Optimization Node Array */
};


/**
 * @brief QUERY SPECIFICATION Optimization Node
 */
struct qlnoQuerySpec
{
    /* Child Node */
    qllOptimizationNode  * mChildNode;             /**< Child Optimization Node */
    
    stlBool                mNeedRowBlock;          /**< Row Block이 필요한지 여부 */

    /* Target 관련 */
    stlInt32               mTargetCount;           /**< Target Count */
    qlvInitTarget        * mTargets;               /**< Target List : array */
    
    /* Value Block 관련 */
    qlvRefExprList       * mTargetColList;         /**< Target Column List */
    qloInitExprList      * mQueryExprList;         /**< Query 단위 Expression 정보 */

    /* With 관련 */
    void                 * mWith;                  /**< WITH Clause : ??? */

    /* OFFSET .. LIMIT 절 관련 */
    stlInt64               mSkipCnt;               /**< Skip(Offset) Count */
    stlInt64               mFetchCnt;              /**< Fetch Count */
    qlvInitExpression    * mResultSkip;            /**< Skip(Offset) Expression */
    qlvInitExpression    * mResultLimit;           /**< Limit(Fetch) Expression */

    /*
     * Complete Node Function에서 구축할 정보
     */

    /* Target 관련 */
    knlExprStack         * mTargetsCodeStack;      /**< Target 절 : array */
};


/**
 * @brief SUB-QUERY Optimization Node
 */
struct qlnoSubQuery
{
    /* Child Node */
    qllOptimizationNode  * mChildNode;             /**< Child Optimization Node */

    stlBool                mNeedRowBlock;          /**< Row Block이 필요한지 여부 */

    /* Validation Node */
    qlvInitSubTableNode  * mInitNode;              /**< Sub Table Init Node */
    
    /* Target */
    stlInt32               mTargetCount;           /**< Target Count */
    qlvRefExprList       * mTargetList;            /**< Target List */

    /* Outer Column */
    qlvRefExprList       * mOuterColumnList;       /**< Outer Column List */

    /* Filter 관련 */
    qlvInitExpression    * mFilterExpr;            /**< Filter Expression */

    
    /*
     * Complete Node Function에서 구축할 정보
     */

    /* Filter 관련 */
    knlPredicateList     * mFilterPred;            /**< Filter Predicate */
};


/**
 * @brief SUB-QUERY LIST Optimization Node
 */
struct qlnoSubQueryList 
{
    /* Child Node */
    qllOptimizationNode   * mChildNode;            /**< Child Optimization Node */

    stlBool                 mNeedRowBlock;         /**< Row Block이 필요한지 여부 */

    /* SubQuery 관련 */
    qlvRefExprList        * mSubQueryExprList;     /**< Sub Query Expression List */

    /* Outer Column 관련 */
    qlvRefExprList        * mOuterColumnList;      /**< Outer Column Expression List */

    /* SubQuery Node */
    qllOptimizationNode  ** mSubQueryNodes;        /**< SubQuery Optimization Node */
    stlInt32                mSubQueryNodeCnt;      /**< SubQuery Optimization Node Count */
};


/**
 * @brief SUB-QUERY FUNCTION Optimization Node
 */
struct qlnoSubQueryFunc
{
    /* Child Node */
    qllOptimizationNode  * mChildNode;           /**< Child Optimization Node : 상위 노드에서 설정 */
    
    /* Operand Node */
    qllOptimizationNode  * mRightOperandNode;    /**< Right Operand Optimization Node */

    /* Func 관련 */
    qlvInitExpression    * mFuncExpr;            /**< Subquery Function Expression : list function */
    qlvInitExpression    * mResultExpr;          /**< Subquery Function Result Expression : Outer Column */

    
    /*
     * Complete Node Function에서 구축할 정보
     */
    
    knlExprStack        ** mLeftOperCodeStack;   /**< Left Operand Expression Stack : Array */
};


/**
 * @brief SUB-QUERY FILTER Optimization Node
 */
struct qlnoSubQueryFilter
{
    /* Related Node 관련 */
    qllOptimizationNode   * mChildNode;             /**< Child Optimization Node */

    stlBool                 mNeedRowBlock;          /**< Row Block이 필요한지 여부 */

    /* And Filter Node 관련 */
    qllOptimizationNode  ** mAndFilterNodeArr;      /**< And Filter Optimization Node */

    /* Filter 관련 */
    qlvInitExpression    ** mFilterExprArr;         /**< And Filter Expression */
    stlInt32                mAndFilterCnt;          /**< And Filter Expression Count */

    /*
     * Complete Node Function에서 구축할 정보
     */

    /* Filter 관련 */
    knlPredicateList     ** mFilterPredArr;         /**< Filter Predicate */
};


// /**
//  * @brief VIEW Optimization Node
//  */
// struct qlnoView
// {
//     /* Child Node */
//     qllOptimizationNode  * mChildNode;             /**< Child Optimization Node */

//     /* View Definition */
//     stlInt32               mViewType;              /**< 미정 */
//     void                 * mViewStmt;              /**< 미정 */
// };


/**
 * @brief FILTER Optimization Node
 */
struct qlnoFilter
{
    /* Child Node */
    qllOptimizationNode  * mChildNode;             /**< Child Optimization Node */

    /* Value Block List 관련 */
    qlvRefExprList       * mRefValueExprList;      /**< Referenced Value Expression List */

    /*
     * Complete Node Function에서 구축할 정보
     */

    /* Filter */
    knlPredicateList   * mFilterPred;            /**< Filter Predicate */
};


/***************************************************
 * ORDER BY, GROUP BY, AGGREGATION Nodes
 ***************************************************/ 

/**
 * @brief HASH Optimization Node
 */
struct qlnoHash
{
    /* Child Node */
    qllOptimizationNode  * mChildNode;             /**< Child Optimization Node */

    /* Hash */
    void                 * mHashOption;            /**< Hash Option : 미정 */
    void                 * mHashFunction;          /**< Hash Function : 미정 */

    /* Value Block List 관련 */
    qlvRefExprList       * mKeyColExprList;        /**< Hash Key Column Expression List */
    qlvRefExprList       * mValueColExprList;      /**< Hash에 저장될
                                                    * Value Column Expression List */
};


/**
 * @brief AGGREGATION Optimization Node
 */
struct qlnoAggregation
{
    /* Child Node */
    qllOptimizationNode  * mChildNode;             /**< Child Optimization Node */
    
    /* Aggregation 관련 */
    qlvAggregation       * mAggregation;
    
    stlInt32               mAggrFuncCnt;           /**< Aggregation Function Count */
    stlInt32               mAggrDistFuncCnt;       /**< Aggregation Distinct Function Count */

    qlvRefExprList       * mAggrFuncList;          /**< Aggregation Function List */
    qlvRefExprList       * mAggrDistFuncList;      /**< Aggregation Distinct Function List */
    
    /*
     * Complete Node Function에서 구축할 정보
     */

    /* Aggregation 관련 */
    qlnfAggrOptInfo      * mAggrOptInfo;           /**< Aggregation Optimization Info : array */
    qlnfAggrOptInfo      * mAggrDistOptInfo;       /**< Aggregation Distinct Optimization Info : array */
};


/**
 * @brief Group Optimization Node (no materialization)
 */
struct qlnoGroup
{
    stlBool                    mNeedRowBlock;             /**< Row Block이 필요한지 여부 */

    /* Instant Node 관련 */
    qlvInitInstantNode       * mInstantNode;              /**< Instant Node */

    /* Child Node */
    qllOptimizationNode      * mChildNode;                /**< Child Optimization Node */

    /* Base Relation 관련 */
    qllOptNodeList           * mSrcRelationList;          /**< Row Block을 참조하여야 하는 Base Relation Node List */

    /* Outer Column */
    qlvRefExprList           * mOuterColumnList;          /**< Outer Column List */

    /* Filter 관련 (HAVING) */
    qlvInitExpression        * mTotalFilterExpr;          /**< Total Filter Expression */

    /* Aggregation 관련 */
    stlInt32                   mAggrFuncCnt;              /**< Aggregation Function Count */
    stlInt32                   mAggrDistFuncCnt;          /**< Aggregation Distinct Function Count */

    qlvRefExprList           * mTotalAggrFuncList;        /**< Total Aggregation Function List */
    qlvRefExprList           * mAggrFuncList;             /**< Aggregation Function List */
    qlvRefExprList           * mAggrDistFuncList;         /**< Aggregation Distinct Function List */


    /*
     * Complete Node Function에서 구축할 정보
     */

    /* Grouping Column 관련 */
    knlExprStack            ** mColExprStack;             /**< Column Expression Stack : array */
    stlBool                    mNeedEvalKeyCol;           /**< Key Column에 대한 Expression 평가 필요 여부 */

    /* Filter 관련 (HAVING) */
    knlExprStack             * mLogicalStack;             /**< Logical Table Filter Expression Stack */

    /* Predicate 관련 (HAVING) */
    knlPredicateList         * mPredicate;                /**< Filter Predicate */

    /* Aggregation 관련 */
    qlnfAggrOptInfo          * mAggrOptInfo;              /**< Aggregation Optimization Info : array */
    qlnfAggrOptInfo          * mAggrDistOptInfo;          /**< Aggregation Distinct Optimization Info : array */
};


/***************************************************
 * SCAN Nodes
 ***************************************************/ 

/**
 * @brief TABLE ACCESS Optimization Node
 */
struct qlnoTableAccess
{
    /* Relation 관련 */
    qlvInitNode               * mRelationNode;       /**< Relation Node */
    
    stlBool                     mNeedRowBlock;       /**< Row Block이 필요한지 여부 */

    /* Iterator 관련 */
    stlChar                   * mTableName;          /**< Base Table Name */
    ellDictHandle             * mTableHandle;        /**< Table Handle */
    stlChar                   * mDumpOption;         /**< Dump Option */

    /* Value Block List 관련 */ 
    qlvRefExprList            * mReadColList;        /**< Read Column List */
    qlvInitExpression         * mRowIdColumn;        /**< Read RowId Column */

    /* Outer Column */
    qlvRefExprList            * mOuterColumnList;    /**< Outer Column List */

    /* Filter 관련 */
    qlvInitExpression         * mTotalFilterExpr;    /**< Total Filter Expression */

    /* Aggregation 관련 */
    stlInt32                    mAggrFuncCnt;        /**< Aggregation Function Count */
    stlInt32                    mAggrDistFuncCnt;    /**< Aggregation Distinct Function Count */

    qlvRefExprList            * mTotalAggrFuncList;  /**< Total Aggregation Function List */
    qlvRefExprList            * mAggrFuncList;       /**< Aggregation Function List */
    qlvRefExprList            * mAggrDistFuncList;   /**< Aggregation Distinct Function List */
    
    /*
     * Complete Node Function에서 구축할 정보
     */

    /* Filter 관련 */
    qlvInitExpression         * mPhysicalFilterExpr;  /**< Physical Filter Expression */
    qlvInitExpression         * mLogicalFilterExpr;   /**< Logical Filter Expression */

    knlPredicateList          * mLogicalFilterPred;   /**< Logical Filter Predicate */

    /* Prepare Filter 관련 */
    knlExprStack              * mPrepareFilterStack;  /**< Prepare Filter Expression Stack */

    /* Aggregation 관련 */
    qlnfAggrOptInfo           * mAggrOptInfo;         /**< Aggregation Optimization Info : array */
    qlnfAggrOptInfo           * mAggrDistOptInfo;     /**< Aggregation Distinct Optimization Info : array */
};


/**
 * @brief INDEX ACCESS Optimization Node
 */
struct qlnoIndexAccess
{
    /* Relation 관련 */
    qlvInitNode               * mRelationNode;           /**< Relation Node */

    stlBool                     mNeedRowBlock;           /**< Row Block이 필요한지 여부 */

    /* Iterator 관련 */
    stlChar                   * mTableName;              /**< Base Table Name */
    stlChar                   * mIndexName;              /**< Index Name */
    ellDictHandle             * mTableHandle;            /**< Table Handle */
    ellDictHandle             * mIndexHandle;            /**< Index Handle */
    void                      * mIndexPhysicalHandle;    /**< Index Physical Handle */
    void                      * mTablePhysicalHandle;    /**< Table Physical Handle */
    smlIteratorScanDirection    mScanDirection;          /**< Scan Direction */

    /* Value Block List 관련 */ 
    qlvRefExprList            * mIndexReadColList;       /**< Read Index Column List */
    qlvRefExprList            * mTableReadColList;       /**< Read Table Column List */
    qlvRefExprList            * mTotalReadColList;       /**< Read Table Column List */

    qlvInitExpression         * mRowIdColumn;            /**< Read RowId Column */    

    /* Outer Column */
    qlvRefExprList            * mOuterColumnList;        /**< Outer Column List */
    
    /* Aggregation 관련 */
    stlInt32                    mAggrFuncCnt;            /**< Aggregation Function Count */
    stlInt32                    mAggrDistFuncCnt;        /**< Aggregation Distinct Function Count */

    qlvRefExprList            * mTotalAggrFuncList;      /**< Total Aggregation Function List */
    qlvRefExprList            * mAggrFuncList;           /**< Aggregation Function List */
    qlvRefExprList            * mAggrDistFuncList;       /**< Aggregation Distinct Function List */

    /* IN Key Range 관련 */
    stlInt32                    mListColMapCount;        /**< List Column Expression Count for IN Key Range */
    qlvInitExpression        ** mValueExpr;              /**< Value Expression Array for IN Key Range */
    qlvInitExpression        ** mListColExpr;            /**< List Column Expression Array for IN Key Range */

    /* Range & Filter 관련 */
    /**
     * @todo Index Scan Info 에서 Predicate 생성하는 것 밖으로 빼기 (complete에서 생성할 대상임)
     */
    qloIndexScanInfo          * mIndexScanInfo;          /**< Index Scan Info */

    /*
     * Complete Node Function에서 구축할 정보
     */

    /* Key Filter 관련 */
    knlExprStack              * mKeyLogicalStack;        /**< Logical Key Filter Expression Stack */

    /* Table Filter 관련 */
    knlExprStack              * mTableLogicalStack;      /**< Logical Table Filter Expression Stack */

    /* Prepare Range 관련 */
    knlExprStack              * mPrepareRangeStack;      /**< Prepare Range Expression Stack */
    knlExprStack              * mPrepareInKeyRangeStack; /**< Prepare IN Key Range Expression Stack */

    /* Prepare Filter 관련 */
    knlExprStack              * mPrepareKeyFilterStack;  /**< Prepare Key Filter Expression Stack */
    knlExprStack              * mPrepareFilterStack;     /**< Prepare Filter Expression Stack */

    /* Aggregation 관련 */
    qlnfAggrOptInfo           * mAggrOptInfo;            /**< Aggregation Optimization Info : array */
    qlnfAggrOptInfo           * mAggrDistOptInfo;        /**< Aggregation Distinct Optimization Info : array */
};


/**
 * @brief ROWID ACCESS Optimization Node
 */
struct qlnoRowIdAccess
{
    /* Relation 관련 */
    qlvInitNode               * mRelationNode;       /**< Relation Node */
    
    stlBool                     mNeedRowBlock;       /**< Row Block이 필요한지 여부 */

    stlChar                   * mTableName;          /**< Base Table Name */
    ellDictHandle             * mTableHandle;        /**< Table Handle */
    void                      * mPhysicalHandle;     /**< Table Physical Handle */

    /* Value Block List 관련 */ 
    qlvRefExprList            * mReadColList;        /**< Read Column List */

    qlvInitExpression         * mRowIdColumn;        /**< Read RowId Column */

    /* Outer Column */
    qlvRefExprList            * mOuterColumnList;    /**< Outer Column List */

    /* RowId Scan Expression 관련 */
    qlvInitExpression         * mRowIdScanExpr;      /**< RowId Scan Expression */

    /* Filter 관련 */
    qlvInitExpression         * mTotalFilterExpr;    /**< Total Filter Expression */

    /* Aggregation 관련 */
    stlInt32                    mAggrFuncCnt;        /**< Aggregation Function Count */

    qlvRefExprList            * mTotalAggrFuncList;  /**< Total Aggregation Function List */
    qlvRefExprList            * mAggrFuncList;       /**< Aggregation Function List */

    /*
     * Complete Node Function에서 구축할 정보
     */

    /* Filter 관련 */
    qlvInitExpression        * mPhysicalFilterExpr;  /**< Physical Filter Expression */
    qlvInitExpression        * mLogicalFilterExpr;   /**< Logical Filter Expression */    
    
    knlExprStack             * mLogicalStack;        /**< Logical Filter Expression Stack */
    
    /* Prepare Filter 관련 */
    knlExprStack             * mPrepareFilterStack;  /**< Prepare Filter Expression Stack */

    /* Aggregation 관련 */
    qlnfAggrOptInfo          * mAggrOptInfo;         /**< Aggregation Optimization Info : array */
};


/**
 * @brief FAST DUAL Optimization Node
 */
struct qlnoFastDualAccess
{
    // Nothing
};


/**
 * @brief INSTANT TABLE Optimization Node
 */
struct qlnoInstant
{
    /* Node Type */
    qlvNodeType                mType;                     /**< Node Type */

    stlBool                    mNeedRowBlock;             /**< Row Block이 필요한지 여부 */

    /* Materialization 여부 */
    stlBool                    mNeedRebuild;              /**< Rebuild 필요 여부 */

    /* Skip Hit Record */
    stlBool                    mIsSkipHitRecord;          /**< 이미 탐색된 record에 대한 skip 여부 */
    /* Scroll 여부 */
    ellCursorScrollability     mScrollable;               /**< Scrollability */

    /* Child Node */
    qllOptimizationNode      * mChildNode;                /**< Child Optimization Node */

    /* Instant Node 관련 */
    qlvInitInstantNode       * mInstantNode;              /**< Instant Node */

    /* Base Relation 관련 */
    qllOptNodeList           * mSrcRelationList;          /**< Row Block을 참조하여야 하는 Base Relation Node List */
    
    /* Outer Column */
    qlvRefExprList           * mOuterColumnList;          /**< Outer Column List */

    /* Total Filter 관련 */
    qlvInitExpression        * mTotalFilterExpr;          /**< Total Filter Expression */
    
    /* Index Scan 관련 */
    smlSortTableAttr           mSortTableAttr;            /**< Sort Instant Table 생성 Attribute */
    qloIndexScanInfo         * mIndexScanInfo;            /**< Index Scan Info */

    /* Hash Scan 관련 */
    smlIndexAttr               mHashTableAttr;            /**< Hash Instant Table 생성 Attribute */
    qloHashScanInfo          * mHashScanInfo;             /**< Hash Scan Info */

    /* Aggregation 관련 */
    stlInt32                   mAggrFuncCnt;              /**< Aggregation Function Count */
    stlInt32                   mAggrDistFuncCnt;          /**< Aggregation Distinct Function Count */

    qlvRefExprList           * mTotalAggrFuncList;        /**< Total Aggregation Function List */
    qlvRefExprList           * mAggrFuncList;             /**< Aggregation Function List */
    qlvRefExprList           * mAggrDistFuncList;         /**< Aggregation Distinct Function List */

    /* Nested Aggregation 관련 */
    stlInt32                   mNestedAggrFuncCnt;        /**< Nested Aggregation Function Count */
    stlInt32                   mNestedAggrDistFuncCnt;    /**< Nested Aggregation Distinct Function Count */
    
    qlvRefExprList           * mNestedTotalAggrFuncList;  /**< Total Nested Aggregation Function List */
    qlvRefExprList           * mNestedAggrFuncList;       /**< Nested Aggregation Function List */
    qlvRefExprList           * mNestedAggrDistFuncList;   /**< Nested Aggregation Distinct Function List */

    
    /**
     * complete Node Function에서 구축할 정보 (Common)
     */

    /* Column 관련 */
    qlvInstantColDesc        * mColumnDesc;               /**< Column Description : array */
    knlExprStack            ** mColExprStack;             /**< Column Expression Stack : array */
    stlBool                    mNeedEvalKeyCol;           /**< Key Column에 대한 Expression 평가 필요 여부 */
    stlBool                    mNeedEvalRecCol;           /**< Record Column에 대한 Expression 평가 필요 여부 */
    stlBool                    mNeedEvalInsertCol;        /**< Key Column에 대한 Expression 평가 필요 여부 */

    /* Predicate 관련 */
    knlPredicateList         * mPredicate;                /**< Range or Filter Predicate */

    /* Table Filter 관련 */
    qlvInitExpression        * mTablePhysicalFilterExpr;  /**< Physical Filter Expression */
    qlvInitExpression        * mTableLogicalFilterExpr;   /**< Logical Filter Expression */
    knlExprStack             * mTableLogicalStack;        /**< Logical Table Filter Expression Stack */

    /* Index Key Filter 관련 */
    knlExprStack             * mIndexKeyLogicalStack;     /**< Logical Key Filter Expression Stack */

    /* Aggregation 관련 */
    qlnfAggrOptInfo          * mAggrOptInfo;              /**< Aggregation Optimization Info : array */
    qlnfAggrOptInfo          * mAggrDistOptInfo;          /**< Aggregation Distinct Optimization Info : array */

    /* Nested Aggregation 관련 */
    qlnfAggrOptInfo          * mNestedAggrOptInfo;        /**< Nested Aggregation Optimization Info : array */
    qlnfAggrOptInfo          * mNestedAggrDistOptInfo;    /**< Nested Aggregation Distinct Optimization Info : array */

    /* Prepare Range & Hash Key 관련 */
    knlExprStack             * mPrepareRangeStack;        /**< Prepare Range/Hash-Key Expression Stack */

    /* Prepare Filter 관련 */
    knlExprStack             * mPrepareKeyFilterStack;    /**< Prepare Key Filter Expression Stack */
    knlExprStack             * mPrepareFilterStack;       /**< Prepare Filter Expression Stack */
};


/***************************************************
 * JOIN Nodes
 ***************************************************/ 

/**
 * @brief NESTED LOOPS JOIN Optimization Node
 */
struct qlnoNestedLoopsJoin
{
    /* Child Nodes */
    qllOptimizationNode     * mLeftChildNode;           /**< Left Child Optimization Node */
    qllOptimizationNode     * mRightChildNode;          /**< Right Child Optimization Node */

    stlBool                   mNeedRowBlock;            /**< Row Block이 필요한지 여부 */

    qlvJoinType               mJoinType;                /**< Join Type */

    /* Value Block List 관련 */
    qlvRefExprList          * mJoinColList;             /**< JOIN 대상 Column List */
    qlvInitJoinTableNode    * mJoinTableNode;           /**< JOIN Table Node */

    qlvRefExprList          * mLeftOuterColumnList;     /**< Outer Column List */
    qlvRefExprList          * mRightOuterColumnList;    /**< Outer Column List */

    qlvRefExprList          * mWhereOuterColumnList;    /**< Where Filter Outer Column List */

    /* Outer Column */
    qlvRefExprList          * mOuterColumnList;         /**< Outer Column List */

    /**
     * Full Outer Join에서 Join Condition에 존재하는 SubQuery Filter를 처리하기 위한 변수들
     */

    qllOptimizationNode     * mStmtSubQueryNode;        /**< Statement단위 처리 SubQuery Optimization Node */

    /* And Filter Node 관련 */
    qllOptimizationNode    ** mSubQueryAndFilterNodeArr;/**< And Filter Optimization Node */

    /* Filter 관련 */
    qlvInitExpression      ** mSubQueryFilterExprArr;   /**< And Filter Expression */
    stlInt32                  mSubQueryAndFilterCnt;    /**< And Filter Expression Count */


    /*
     * Complete Node Function에서 구축할 정보
     */

    /* Filter 관련 */
    qlvInitExpression       * mTotalFilterExpr;         /**< Total Filter Expression */

    qlvInitExpression       * mPhysicalFilterExpr;      /**< Physical Filter Expression */
    qlvInitExpression       * mLogicalFilterExpr;       /**< Logical Filter Expression */

    knlPredicateList        * mLogicalFilterPred;       /**< Logical Filter Predicate */

    qlvInitExpression       * mJoinFilterExpr;          /**< JOIN Filter Expression */
    knlPredicateList        * mJoinFilterPred;          /**< JOIN Predicate */

    knlExprStack            * mConstExprStack;          /**< constant expression stack */

    /* SubQuery Filter 관련 */
    knlPredicateList       ** mSubQueryFilterPredArr;   /**< SubQuery Filter Predicate */

};


/**
 * @brief SORT MERGE JOIN Optimization Node
 */
struct qlnoSortMergeJoin
{
    /* Child Nodes */
    qllOptimizationNode     * mLeftChildNode;           /**< Left Child Optimization Node */
    qllOptimizationNode     * mRightChildNode;          /**< Right Child Optimization Node */

    stlBool                   mNeedRowBlock;            /**< Row Block이 필요한지 여부 */

    qlvJoinType               mJoinType;                /**< Join Type */

    /* Value Block List 관련 */
    qlvRefExprList          * mJoinColList;             /**< JOIN 대상 Column List */
    qlvInitJoinTableNode    * mJoinTableNode;           /**< JOIN Table Node */

    qlvInitInstantNode        mLeftSortInstant;         /**< Left Sort Instant */
    qlvInitInstantNode        mRightSortInstant;        /**< Right Sort Instant */

    qlvRefExprList          * mLeftSortKeyList;         /**< Equi-Join인 경우 Left Sort Key List */
    qlvRefExprList          * mRightSortKeyList;        /**< Equi-Join인 경우 Right Sort Key List */
    qloDBType              ** mLeftSortKeyDBTypePtrArr; /**< Left Sort Key List에 대한 DBType Ptr Arr */
    qloDBType              ** mRightSortKeyDBTypePtrArr;/**< Right Sort Key List에 대한 DBType Ptr Arr */
    qlvRefExprList          * mLeftOuterColumnList;     /**< Outer Column List */
    qlvRefExprList          * mRightOuterColumnList;    /**< Outer Column List */

    /* Outer Column */
    qlvRefExprList          * mOuterColumnList;         /**< Outer Column List */

    /*
     * Complete Node Function에서 구축할 정보
     */

    /* Filter 관련 */
    qlvInitExpression       * mWhereFilterExpr;         /**< Where Filter Expression */
    knlPredicateList        * mWhereFilterPred;         /**< Where Filter Predicate */

    qlvInitExpression       * mJoinFilterExpr;          /**< JOIN Filter Expression */
    knlPredicateList        * mJoinFilterPred;          /**< JOIN Predicate */

    // knlExprStack            * mConstExprStack;          /**< constant expression stack */
};


/**
 * @brief HASH JOIN Optimization Node
 */
struct qlnoHashJoin
{
    /* Child Nodes */
    qllOptimizationNode     * mLeftChildNode;           /**< Left Child Optimization Node */
    qllOptimizationNode     * mRightChildNode;          /**< Right Child Optimization Node */

    stlBool                   mNeedRowBlock;            /**< Row Block이 필요한지 여부 */

    qlvJoinType               mJoinType;                /**< Join Type */

    /* Value Block List 관련 */
    qlvRefExprList          * mJoinColList;             /**< JOIN 대상 Column List */
    qlvInitJoinTableNode    * mJoinTableNode;           /**< JOIN Table Node */

    qlvInitInstantNode        mRightHashInstant;        /**< Right Hash Instant */

    qlvRefExprList          * mLeftOuterColumnList;     /**< Outer Column List */
    qlvRefExprList          * mRightOuterColumnList;    /**< Outer Column List */

    /* Outer Column */
    qlvRefExprList          * mOuterColumnList;         /**< Outer Column List */

    /*
     * Complete Node Function에서 구축할 정보
     */

    /* Filter 관련 */
    qlvInitExpression       * mWhereFilterExpr;         /**< Where Filter Expression */
    knlPredicateList        * mWhereFilterPred;         /**< Where Filter Predicate */

    qlvInitExpression       * mJoinFilterExpr;          /**< JOIN Filter Expression */
    knlPredicateList        * mJoinFilterPred;          /**< JOIN Predicate */
};


/***************************************************
 * CONCAT Node
 ***************************************************/ 
/**
 * @brief CONCAT Optimization Node
 */
struct qlnoConcat
{
    /* Child Nodes */
    qllOptimizationNode    ** mChildNodeArr;            /**< Right Child Optimization Node */

    stlBool                   mNeedRowBlock;            /**< Row Block이 필요한지 여부 */

    stlInt32                  mChildCount;              /**< Child Count */
    ellDictHandle           * mTableHandle;             /**< Table Handle */

    /* Value Block List 관련 */
    qlvRefExprList          * mConcatColList;           /**< Concat 결과 Column List */

    /* Outer Column */
    qlvRefExprList          * mOuterColumnList;         /**< Outer Column List */
    
    /* RowId Expression 관련 */
    qlvInitExpression       * mRowIdColExpr;            /**< 하위 노드에서 참조하는 RowId Column Expression */
    
    /* Hash Scan 관련 */
    smlIndexAttr              mHashTableAttr;           /**< Hash Instant Table 생성 Attribute */
};


/****************************************************
 * Common Functions
 ****************************************************/ 

stlStatus qlnoCreateOptNodeList( qllOptNodeList  ** aOptNodeList,
                                 knlRegionMem     * aRegionMem,
                                 qllEnv           * aEnv );

stlStatus qlnoAddToNodeList( qllOptNodeList       * aOptNodeList,
                             qllOptimizationNode  * aOptNode,
                             knlRegionMem         * aRegionMem,
                             qllEnv               * aEnv );

stlStatus qlnoCreateOptNode( qllOptNodeList        * aOptNodeList,
                             qllPlanNodeType         aNodeType,
                             void                  * aNode,
                             qllOptimizationNode   * aCurQueryNode,
                             qllEstimateCost       * aOptCost,
                             qllBindParamInfo      * aBindParamInfo,
                             qllOptimizationNode  ** aNewOptNode,
                             knlRegionMem          * aRegionMem,
                             qllEnv                * aEnv );

stlStatus qlnoGetRefBaseTableNodeList( qllOptNodeList       * aOptNodeList,
                                       qllOptimizationNode  * aOptNode,
                                       qllOptNodeList       * aBaseTableList,
                                       knlRegionMem         * aRegionMem,
                                       qllEnv               * aEnv );

stlStatus qlnoCreateBindParamInfo( qllStatement       * aSQLStmt,
                                   qllBindParamInfo  ** aBindParamInfo,
                                   knlRegionMem       * aRegionMem,
                                   qllEnv             * aEnv );

stlStatus qlnoValidateParamInfo( stlInt32               aBindCount,
                                 knlBindContext       * aBindContext,
                                 qllOptimizationNode  * aCodePlan,
                                 stlBool              * aIsValid,
                                 qllEnv               * aEnv );

stlStatus qlnoValidateParamType( qllStatement  * aSQLStmt,
                                 qlvInitPlan   * aInitPlan,
                                 stlBool       * aIsValid,
                                 qllEnv        * aEnv );

/* SELECT STATEMENT */

/* INSERT STATEMENT */

/* DELETE STATEMENT */

/* UPDATE STATEMENT */

/* CREATE TABLE STATEMENT */

/* DROP TABLE STATEMENT */

/* TRUNCATE TABLE STATEMENT */

/* MERGE TABLE STATEMENT */

/* Misc */


/****************************************************
 * QUERY SPEC node 관련 functions
 ****************************************************/

stlStatus qlnoMakeQuerySpec( qllOptNodeList         * aOptNodeList,
                             stlInt16                 aChildNodeIdx,
                             stlInt32                 aTargetCount,
                             knlExprStack           * aTargetsCodeStack,
                             qlvRefExprList         * aTargetColList,
                             qlvRefExprList         * aPseudoColList,
                             void                   * aWith,
                             stlInt64                 aSkipCnt,
                             stlInt64                 aFetchCnt,
                             qllEstimateCost        * aEstimateCost,
                             qllOptimizationNode   ** aOptNode,
                             knlRegionMem           * aRegionMem,
                             qllEnv                 * aEnv );


/**********************************************
 * Instant Table 관련 functions
 **********************************************/

/* Instant */
stlStatus qlnoInitInstant( qlvInitInstantNode  * aInstTable,
                           qlvNodeType           aInstTableType,
                           qlvRelationName     * aBaseRelName,
                           knlRegionMem        * aRegionMem,
                           qllEnv              * aEnv );

stlStatus qlnoAddKeyColToInstant( stlChar              * aSQLString,
                                  qlvInitInstantNode   * aInstTable,
                                  qlvInitExpression    * aOrgExpr,
                                  stlUInt8               aKeyFlag,
                                  qlvInitExpression   ** aColExpr,
                                  knlRegionMem         * aRegionMem,
                                  qllEnv               * aEnv );

stlStatus qlnoAddKeyColToInstantRestrict( stlChar              * aSQLString,
                                          qlvInitInstantNode   * aInstTable,
                                          qlvInitExpression    * aOrgExpr,
                                          stlUInt8               aKeyFlag,
                                          qlvInitExpression   ** aColExpr,
                                          knlRegionMem         * aRegionMem,
                                          qllEnv               * aEnv );

stlStatus qlnoAddRecColToInstant( qlvInitInstantNode   * aInstTable,
                                  qlvInitExpression    * aOrgExpr,
                                  qlvInitExpression   ** aColExpr,
                                  knlRegionMem         * aRegionMem,
                                  qllEnv               * aEnv );

stlStatus qlnoAddRecColToInstantRestrict( stlChar              * aSQLString,
                                          qlvInitInstantNode   * aInstTable,
                                          qlvInitExpression    * aOrgExpr,
                                          qlvInitExpression   ** aColExpr,
                                          knlRegionMem         * aRegionMem,
                                          qllEnv               * aEnv );

stlStatus qlnoAddReadColToInstant( qlvInitInstantNode   * aInstTable,
                                   qlvInitExpression    * aOrgExpr,
                                   qlvInitExpression   ** aColExpr,
                                   knlRegionMem         * aRegionMem,
                                   qllEnv               * aEnv );

stlStatus qlnoAddReadColToInstantRestrict( qlvInitInstantNode   * aInstTable,
                                           qlvInitExpression    * aOrgExpr,
                                           qlvInitExpression   ** aColExpr,
                                           knlRegionMem         * aRegionMem,
                                           qllEnv               * aEnv );

stlStatus qlnoIsExistExprFromInstant( qlvInitInstantNode   * aInstTable,
                                      qlvInitExpression    * aExpr,
                                      stlBool                aIsCheckKeyCol,
                                      stlBool              * aIsExist,
                                      qlvInitExpression   ** aSameExpr,
                                      knlRegionMem         * aRegionMem,
                                      qllEnv               * aEnv );

stlStatus qlnoSetIdxInstant( qlvInitInstantNode  * aInstTable,
                             qllEnv              * aEnv );

stlStatus qlnoAdjustJoinReadColumnOrder( knlRegionMem   * aRegionMem,
                                         qlvRefExprList * aReadColumnList,
                                         qllEnv         * aEnv );

stlStatus qlnoGetCompareType( stlChar               * aSQLString,
                              dtlDataType             aDataType1,
                              dtlDataType             aDataType2,
                              stlBool                 aIsConst1,
                              stlBool                 aIsConst2,
                              stlInt32                aExprPos,
                              const dtlCompareType ** aCompareType,
                              qllEnv                * aEnv );

stlStatus qlnoAllocAndCopyString( stlChar        * aSrcString,
                                  stlChar       ** aResultString,
                                  knlRegionMem   * aRegionMem,
                                  qllEnv         * aEnv );


/** @} qlno */

#endif /* _QLNONODE_H_ */
