/*******************************************************************************
 * qlncCostBasedOptimizer.h
 *
 * Copyright (c) 2013, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *
 *
 ******************************************************************************/

#ifndef _QLNCCOSTBASEDOPTIMIZER_H_
#define _QLNCCOSTBASEDOPTIMIZER_H_ 1

/**
 * @file qlncCostBasedOptimizer.h
 * @brief SQL Processor Layer Cost Based Optimizer
 */

#include <qlDef.h>

/**
 * @addtogroup qlnc
 * @{
 */


/*******************************************************************************
 * DEFINITIONS
 ******************************************************************************/

/**< @todo 아래 값은 추후 optimizer property로 변경해야 함 */
#define QLNC_MAX_DISTRIBUTED_CNF_COUNT          1000                /**< CNF 형태의 조건을 DNF로 바꿀 최대 배분 결과 OR 하위의 argument 개수 */

#define QLNC_OUTPUT_COST_PER_ROW                (stlFloat64)1.0E-8  /**< Row당 Output 비용 */
#define QLNC_READ_COST_PER_ROW                  (stlFloat64)2.0E-8  /**< Row당 읽기 비용 */
#define QLNC_FILTER_PREPARE_COST                (stlFloat64)3.5E-8  /**< Filter 처리 준비작업 비용 */
#define QLNC_PHYSICAL_FILTER_COST_PER_ROW       (stlFloat64)2.0E-9  /**< Row당 Physical Filter 처리 비용 */
#define QLNC_LOGICAL_FILTER_COST_PER_ROW        (stlFloat64)3.3E-8  /**< Row당 Logical Filter 처리 비용 */

#define QLNC_UNIQ_IDX_RANGE_COST_PER_SEARCH     (stlFloat64)3.0E-7  /**< Search당 Index Range 처리 비용 */
#define QLNC_NON_UNIQ_IDX_RANGE_COST_PER_SEARCH (stlFloat64)9.0E-6  /**< Search당 Index Range 처리 비용 */
#define QLNC_INDEX_TO_TABLE_COST_PER_ROW        (stlFloat64)2.3E-8  /**< Row당 Index에서 Table로 접근하는 비용 */


#define QLNC_SORT_INSTANT_BUILD_COST_PER_ROW    (stlFloat64)2.5E-7  /**< Row당 Sort Instant 생성 비용 */
#define QLNC_SORT_KEY_RADIX_SORT_COST_PER_KEY   (stlFloat64)7.3E-8  /**< Row당 Sort Key Radix Sort 비용 */
#define QLNC_SORT_KEY_COMPARE_COST_PER_KEY      (stlFloat64)1.0E-8  /**< Row당 Sort Key Compare 비용 */



#define QLNC_HASH_INSTANT_BUILD_COST_PER_ROW    (stlFloat64)6.0E-8  /**< Row당 Hash Instant 생성 비용 */
#define QLNC_HASH_KEY_FILTER_COST_PER_ROW_SEC1  (stlFloat64)2.5E-8  /**< 구간1의 Row당 Hash Key Filter 처리 비용 */
#define QLNC_HASH_KEY_FILTER_COST_PER_ROW_SEC2  (stlFloat64)6.0E-8  /**< 구간2의 Row당 Hash Key Filter 처리 비용 */
#define QLNC_HASH_KEY_FILTER_COST_PER_ROW_SEC3  (stlFloat64)1.2E-7  /**< 구간3의 Row당 Hash Key Filter 처리 비용 */

#define QLNC_HASH_SECTION1_MAX_ROW_COUNT        (stlFloat64)8.0E+4  /**< Hash의 구간1에 해당하는 Max Row Count */
#define QLNC_HASH_SECTION2_MAX_ROW_COUNT        (stlFloat64)1.0E+7  /**< Hash의 구간2에 해당하는 Max Row Count */


//////////////////////////////////////////////////

#define QLNC_DEFAULT_COLUMN_CARDINALITY (stlFloat64)10.0    /**< Default Column Cardinality */

#define QLNC_DEFAULT_OPER_RESULT_RATE   (stlFloat64)0.5     /**< Equal, Not Equal, Range 조건이 아닌 경우 결과 개수 비율 */
#define QLNC_EQUAL_OPER_RESULT_RATE     (stlFloat64)0.1     /**< Equal(=) 조건에 의한 결과 개수 비율 */
#define QLNC_NOT_EQUAL_OPER_RESULT_RATE ((stlFloat64)1.0 - QLNC_EQUAL_OPER_RESULT_RATE) /**< Not Equal 조건에 의한 결과 개수 비율 */
#define QLNC_RANGE_OPER_RESULT_RATE     (stlFloat64)0.33    /**< Range(>, >=, <, <=) 조건에 의한 결과 개수 비율 */
#define QLNC_LIKE_OPER_RESULT_RATE      (stlFloat64)0.11    /**< Like 조건에 의한 결과 개수 비율 */
#define QLNC_IS_OPER_RESULT_RATE        (stlFloat64)0.13    /**< Equal(IS TRUE) 조건에 의한 결과 개수 비율 */
#define QLNC_IS_NOT_OPER_RESULT_RATE    ((stlFloat64)1.0 - QLNC_IS_OPER_RESULT_RATE)    /**< Not Equal 조건에 의한 결과 개수 비율 */

#define QLNC_DEFAULT_CONCAT_DUP_RATE    (stlFloat64)0.1     /**< Index Combine 등에의해 CONCAT노드를 사용할 시 결과 중 중복 비율 */

#define QLNC_DEFAULT_COLUMN_DUP_RATE    (stlFloat64)0.1     /**< Column의 중복 비율 */
#define QLNC_DEFAULT_COLUMN_NULL_RATE   (stlFloat64)0.1     /**< Column의 Null 비율 */

//////////////////////////////////////////////////

#define QLNC_THRESHOLD_FOR_STOPPING_INNER_JOIN_OPTIMIZER     1024   /**< Inner Join에서 Optimizer처리시 수행을 중단할 한계값 */
#define QLNC_THRESHOLD_FOR_SWITCHING_INNER_JOIN_OPTIMIZER       7   /**< Inner Join에서 Optimizer처리 방법을 변경할 table count 한계값 */

#define QLNC_GET_ADJUST_VALUE_BY_PAGE_COUNT( _aPageCount )      \
    ( 1 + ((_aPageCount) * (stlFloat64)0.00000001) )

#define QLNC_IS_INDEX_EQUAL_FILTER( _aFuncID )          \
    ( ((_aFuncID) == KNL_BUILTIN_FUNC_IS_EQUAL) ||      \
      ((_aFuncID) == KNL_BUILTIN_FUNC_IS) ||            \
      ((_aFuncID) == KNL_BUILTIN_FUNC_IS_NULL) ||       \
      ((_aFuncID) == KNL_BUILTIN_FUNC_IS_UNKNOWN) )

#define QLNC_IS_INDEXABLE_LIKE( _aFilter, _aNodeID, _aColumnID )                                    \
    ( ((qlncExprFunction* )(_aFilter)->mExpr)->mArgs[0]->mType == QLNC_EXPR_TYPE_COLUMN             \
      ? ( ((qlncExprColumn*)(((qlncExprFunction* )(_aFilter)->mExpr)->mArgs[0]))->mNode->mNodeID == \
          (_aNodeID) )                                                                              \
      && ( ((qlncExprColumn*)(((qlncExprFunction* )(_aFilter)->mExpr)->mArgs[0]))->mColumnID ==     \
           (_aColumnID) )                                                                           \
      : STL_FALSE )


#define QLNC_SET_JOIN_CHILD_NODE_POS( _aLeftNode, _aRightNode, _aFixedPos )                     \
    {                                                                                           \
        if( ((_aLeftNode)->mNodeType == QLNC_NODE_TYPE_BASE_TABLE) ||                           \
            ((_aLeftNode)->mNodeType == QLNC_NODE_TYPE_SUB_TABLE) )                             \
        {                                                                                       \
            (_aFixedPos) = STL_TRUE;                                                            \
        }                                                                                       \
        else                                                                                    \
        {                                                                                       \
            if( ( (_aRightNode)->mNodeType == QLNC_NODE_TYPE_JOIN_TABLE ) ||                    \
                ( (_aRightNode)->mNodeType == QLNC_NODE_TYPE_INNER_JOIN_TABLE ) ||              \
                ( (_aRightNode)->mNodeType == QLNC_NODE_TYPE_OUTER_JOIN_TABLE ) )               \
            {                                                                                   \
                (_aFixedPos) = STL_FALSE;                                                       \
            }                                                                                   \
            else                                                                                \
            {                                                                                   \
                qlncJoinHint    * _sJoinHint;                                                   \
                if( (_aRightNode)->mNodeType == QLNC_NODE_TYPE_BASE_TABLE )                     \
                {                                                                               \
                    _sJoinHint = ((qlncBaseTableNode*)(_aRightNode))->mJoinHint;                \
                }                                                                               \
                else                                                                            \
                {                                                                               \
                    STL_DASSERT( (_aRightNode)->mNodeType == QLNC_NODE_TYPE_SUB_TABLE );        \
                    _sJoinHint = ((qlncSubTableNode*)(_aRightNode))->mJoinHint;                 \
                }                                                                               \
                                                                                                \
                if( _sJoinHint->mFixedPos == STL_TRUE )                                         \
                {                                                                               \
                    if( _sJoinHint->mIsLeftPos == STL_TRUE )                                    \
                    {                                                                           \
                        QLNC_SWAP_NODE( _aLeftNode, _aRightNode );                              \
                    }                                                                           \
                    (_aFixedPos) = STL_TRUE;                                                    \
                }                                                                               \
                else                                                                            \
                {                                                                               \
                    (_aFixedPos) = STL_FALSE;                                                   \
                }                                                                               \
            }                                                                                   \
        }                                                                                       \
    }

#define QLNC_COPY_INNER_JOIN_NODE_INFO( _aDstInfo, _aSrcInfo )  \
{                                                               \
    (_aDstInfo)->mNode          = (_aSrcInfo)->mNode;           \
    (_aDstInfo)->mAnalyzedNode  = (_aSrcInfo)->mAnalyzedNode;   \
    (_aDstInfo)->mIsFixed       = (_aSrcInfo)->mIsFixed;        \
    (_aDstInfo)->mIsAnalyzed    = (_aSrcInfo)->mIsAnalyzed;     \
}

#define QLNC_SET_BASIC_NL_COST( _aLeftNode,             \
                                _aRightNode,            \
                                _aFilterCost,           \
                                _aBuildCost,            \
                                _aReadIOCost,           \
                                _aOutputIOCost )        \
{                                                       \
    /* Filter Cost */                                   \
    (_aFilterCost) =                                    \
        ( (_aLeftNode)->mBestCost->mTotalCost -         \
          (_aLeftNode)->mBestCost->mBuildCost );        \
                                                        \
    (_aFilterCost) +=                                   \
        ( (_aLeftNode)->mBestCost->mResultRowCount *    \
          ( (_aRightNode)->mBestCost->mTotalCost -      \
            (_aRightNode)->mBestCost->mBuildCost ) );   \
                                                        \
    (_aFilterCost) +=                                   \
        ( (_aLeftNode)->mBestCost->mReadIOCost +        \
          (_aRightNode)->mBestCost->mReadIOCost );      \
                                                        \
    (_aFilterCost) +=                                   \
        ( (_aLeftNode)->mBestCost->mOutputIOCost +      \
          (_aRightNode)->mBestCost->mOutputIOCost );    \
                                                        \
    /* Build Cost */                                    \
    (_aBuildCost) =                                     \
        ( (_aLeftNode)->mBestCost->mBuildCost +         \
          (_aRightNode)->mBestCost->mBuildCost );       \
                                                        \
    /* Read IO Cost */                                  \
    (_aReadIOCost) = (stlFloat64)0.0;                   \
                                                        \
    /* Output IO Cost */                                \
    (_aOutputIOCost) = (stlFloat64)0.0;                 \
}

#define QLNC_SET_BASIC_MERGE_COST( _aLeftNode,          \
                                   _aRightNode,         \
                                   _aFilterCost,        \
                                   _aBuildCost,         \
                                   _aReadIOCost,        \
                                   _aOutputIOCost )     \
{                                                       \
    /* Filter Cost */                                   \
    (_aFilterCost) =                                    \
        ( (_aLeftNode)->mBestCost->mTotalCost -         \
          (_aLeftNode)->mBestCost->mNextFilterCost -    \
          (_aLeftNode)->mBestCost->mBuildCost );        \
    (_aFilterCost) +=                                   \
        ( (_aLeftNode)->mBestCost->mNextFilterCost *    \
          (_aLeftNode)->mBestCost->mResultRowCount );   \
                                                        \
    (_aFilterCost) +=                                   \
        ( (_aRightNode)->mBestCost->mTotalCost -        \
          (_aRightNode)->mBestCost->mNextFilterCost -   \
          (_aRightNode)->mBestCost->mBuildCost );       \
    (_aFilterCost) +=                                   \
        ( (_aRightNode)->mBestCost->mNextFilterCost *   \
          (_aRightNode)->mBestCost->mResultRowCount );  \
                                                        \
    /* Build Cost */                                    \
    (_aBuildCost) =                                     \
        ( (_aLeftNode)->mBestCost->mBuildCost +         \
          (_aRightNode)->mBestCost->mBuildCost );       \
                                                        \
    /* Read IO Cost */                                  \
    (_aReadIOCost) = (stlFloat64)0.0;                   \
                                                        \
    /* Output IO Cost */                                \
    (_aOutputIOCost) = (stlFloat64)0.0;                 \
}

#define QLNC_SET_BASIC_HASH_COST( _aLeftNode,           \
                                  _aRightNode,          \
                                  _aFilterCost,         \
                                  _aBuildCost,          \
                                  _aReadIOCost,         \
                                  _aOutputIOCost )      \
{                                                       \
    /* Filter Cost */                                   \
    (_aFilterCost) =                                    \
        ( (_aLeftNode)->mBestCost->mTotalCost -         \
          (_aLeftNode)->mBestCost->mBuildCost );        \
                                                        \
    (_aFilterCost) +=                                   \
        ( (_aLeftNode)->mBestCost->mResultRowCount *    \
          ( (_aRightNode)->mBestCost->mTotalCost -      \
            (_aRightNode)->mBestCost->mBuildCost ) );   \
                                                        \
    (_aFilterCost) +=                                   \
        ( (_aLeftNode)->mBestCost->mReadIOCost +        \
          (_aRightNode)->mBestCost->mReadIOCost );      \
                                                        \
    (_aFilterCost) +=                                   \
        ( (_aLeftNode)->mBestCost->mOutputIOCost +      \
          (_aRightNode)->mBestCost->mOutputIOCost );    \
                                                        \
    /* Build Cost */                                    \
    (_aBuildCost) =                                     \
        ( (_aLeftNode)->mBestCost->mBuildCost +         \
          (_aRightNode)->mBestCost->mBuildCost );       \
                                                        \
    /* Read IO Cost */                                  \
    (_aReadIOCost) = (stlFloat64)0.0;                   \
                                                        \
    /* Output IO Cost */                                \
    (_aOutputIOCost) = (stlFloat64)0.0;                 \
}

#define QLNC_SET_TOTAL_COST( _aCostCommon )     \
{                                               \
    (_aCostCommon)->mTotalCost =                \
        ( (_aCostCommon)->mFirstFilterCost +    \
          (_aCostCommon)->mNextFilterCost +     \
          (_aCostCommon)->mBuildCost +          \
          (_aCostCommon)->mReadIOCost +         \
          (_aCostCommon)->mOutputIOCost );      \
}

#define QLNC_ADJUST_SEMI_JOIN_COST_BY_WEIGHT_VALUE( _aCostCommon, _aWeightValue )   \
{                                                                                   \
    (_aCostCommon)->mFirstFilterCost *= (_aWeightValue);                            \
    (_aCostCommon)->mNextFilterCost *= (_aWeightValue);                             \
    (_aCostCommon)->mReadIOCost *= (_aWeightValue);                                 \
    (_aCostCommon)->mOutputIOCost *= (_aWeightValue);                               \
    QLNC_SET_TOTAL_COST( (_aCostCommon) );                                          \
}

#define QLNC_SET_PUSH_FILTER( _aParamInfo, _aDstAndFilter, _aPushAndFilter, _aEnv ) \
{                                                                                   \
    if( (_aDstAndFilter) != NULL )                                                  \
    {                                                                               \
        STL_TRY( qlncAppendOrFilterToAndFilterTail( (_aParamInfo),                  \
                                                    (_aDstAndFilter),               \
                                                    (_aPushAndFilter)->mOrCount,    \
                                                    (_aPushAndFilter)->mOrFilters,  \
                                                    (_aEnv) )                       \
                 == STL_SUCCESS );                                                  \
    }                                                                               \
    else                                                                            \
    {                                                                               \
        STL_TRY( qlncCopyAndFilter( (_aParamInfo),                                  \
                                    (_aPushAndFilter),                              \
                                    &(_aDstAndFilter),                              \
                                    aEnv )                                          \
                 == STL_SUCCESS );                                                  \
    }                                                                               \
}


/*******************************************************************************
 * SRUCTURES
 ******************************************************************************/

/**
 * @brief Cost Base Optimizer Parameter Information
 */
struct qlncCBOptParamInfo
{
    qlncParamInfo         mParamInfo;
    qlncNodeCommon      * mLeftNode;
    qlncNodeCommon      * mRightNode;
    qlncAndFilter       * mJoinCond;
    qlncAndFilter       * mFilter;
    qlncCostCommon      * mCost;
};


/**
 * @brief Inner Join Node Info
 */
struct qlncInnerJoinNodeInfo
{
    qlncNodeCommon  * mNode;
    qlncNodeCommon  * mAnalyzedNode;
    knlRegionMark     mRegionMemMarkForSparse;
    qlncAndFilter   * mRemainAndFilterForSparse;
    stlBool           mIsFixed;
    stlBool           mIsAnalyzed;
    stlChar           mPadding[6];
};


/**
 * @brief Common Cost Information
 */
struct qlncCostCommon
{
    stlFloat64        mTotalCost;           /**< 전체 비용 */
    stlFloat64        mFirstFilterCost;     /**< 처음 레코드를 찾는데 드는 비용 */
    stlFloat64        mNextFilterCost;      /**< 다음 레코드를 찾는데 드는 비용 */
    stlFloat64        mBuildCost;           /**< Instant 등을 Build 하는데 드는 비용 */
    stlFloat64        mReadIOCost;          /**< 하위 노드 및 현재 노드에서 결과를 위해 드는 비용 */
    stlFloat64        mOutputIOCost;        /**< 상위 노드로 결과를 올리는데 드는 비용 */
    stlFloat64        mResultRowCount;      /**< 결과 Row 개수 */
};


/**
 * @brief Query Spec Cost
 */
struct qlncQuerySpecCost
{
    qlncCostCommon    mCostCommon;

    qlncNodeCommon  * mChildNode;
};


/**
 * @brief Table Scan Cost
 */
struct qlncTableScanCost
{
    qlncCostCommon    mCostCommon;

    qlncAndFilter   * mTableAndFilter;
};


/**
 * @brief Index Scan Cost
 */
struct qlncIndexScanCost
{
    qlncCostCommon        mCostCommon;

    ellDictHandle       * mIndexHandle;
    void                * mIndexPhyHandle;
    stlBool               mIsAsc;

    qlncAndFilter       * mIndexRangeAndFilter;
    qlncAndFilter       * mIndexKeyAndFilter;
    qlncAndFilter       * mTableAndFilter;

    qlncListColMap      * mListColMap;   /* for IN Key Range */

    stlBool               mNeedTableScan;
};


/**
 * @brief Index Combine Cost
 */
struct qlncIndexCombineCost
{
    qlncCostCommon        mCostCommon;

    stlInt32              mDnfAndFilterCount;   /**< Index Scan Count와 동일 */
    qlncAndFilter       * mDnfAndFilterArr;

    qlncIndexScanCost   * mIndexScanCostArr;
};


/**
 * @brief Rowid Scan Cost
 */
struct qlncRowidScanCost
{
    qlncCostCommon    mCostCommon;

    qlncAndFilter   * mRowidAndFilter;
    qlncAndFilter   * mTableAndFilter;
};


/**
 * @brief Filter Cost
 */
struct qlncFilterCost
{
    qlncCostCommon    mCostCommon;

    qlncNodeCommon  * mMaterializedNode;

    qlncAndFilter   * mFilter;
};


/**
 * @brief Nested Loops Join Cost
 */
struct qlncNestedJoinCost
{
    qlncCostCommon    mCostCommon;

    qlncNodeCommon  * mLeftNode;
    qlncNodeCommon  * mRightNode;
    qlncNodeCommon  * mCopiedBaseNode;  /* For Trace Log */

    qlncAndFilter   * mJoinCondition;
    qlncAndFilter   * mSubQueryFilter;  /* For Full Outer Join */
    qlncAndFilter   * mJoinFilter;

    stlBool           mIsInverted;      /* For Semi/Anti-Semi Join (Inverted로 처리할지 여부) */
};


/**
 * @brief Sort Merge Join Cost
 */
struct qlncMergeJoinCost
{
    qlncCostCommon    mCostCommon;

    qlncNodeCommon  * mLeftNode;
    qlncNodeCommon  * mRightNode;

    qlncAndFilter   * mEquiJoinCondition;
    qlncAndFilter   * mNonEquiJoinCondition;
    qlncAndFilter   * mJoinFilter;

    stlInt32          mSortKeyCount;
    qlncExprCommon ** mLeftSortKeyArr;
    qlncExprCommon ** mRightSortKeyArr;
};


/**
 * @brief Hash Join Cost
 */
struct qlncHashJoinCost
{
    qlncCostCommon    mCostCommon;

    qlncNodeCommon  * mLeftNode;
    qlncNodeCommon  * mRightNode;

    qlncAndFilter   * mEquiJoinCondition;
    qlncAndFilter   * mNonEquiJoinCondition;
    qlncAndFilter   * mJoinFilter;

    stlInt32          mHashKeyCount;
    qlncExprCommon ** mLeftHashKeyArr;
    qlncExprCommon ** mRightHashKeyArr;

    stlBool           mIsInverted;      /* For Semi/Anti-Semi Join (Inverted로 처리할지 여부) */
};


/**
 * @brief Join Combine Cost
 */
struct qlncJoinCombineCost
{
    qlncCostCommon    mCostCommon;

    stlInt32          mDnfAndFilterCount;
    qlncAndFilter   * mDnfAndFilterArr;

    qlncNodeCommon ** mJoinNodePtrArr;
};


/**
 * @brief Flat Instant Cost
 */
struct qlncFlatInstantCost
{
    qlncCostCommon    mCostCommon;
};


/**
 * @brief Sort Instant Cost
 */
struct qlncSortInstantCost
{
    qlncCostCommon    mCostCommon;
};


/**
 * @brief Hash Instant Cost
 */
struct qlncHashInstantCost
{
    qlncCostCommon    mCostCommon;
};


/**
 * @brief Group Cost
 */
struct qlncGroupCost
{
    qlncCostCommon    mCostCommon;
};


/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/

typedef stlStatus (*qlncCostBasedOptimizerFuncPtr) ( qlncCBOptParamInfo * aCBOptParamInfo,
                                                     qllEnv             * aEnv );

extern const qlncCostBasedOptimizerFuncPtr qlncCostBasedOptimizerFuncList[ QLNC_NODE_TYPE_MAX ];

stlStatus qlncCBOptQuerySet( qlncCBOptParamInfo * aCBOptParamInfo,
                             qllEnv             * aEnv );

stlStatus qlncCBOptSetOP( qlncCBOptParamInfo    * aCBOptParamInfo,
                          qllEnv                * aEnv );

stlStatus qlncCBOptQuerySpec( qlncCBOptParamInfo    * aCBOptParamInfo,
                              qllEnv                * aEnv );

stlStatus qlncCBOptBaseTable( qlncCBOptParamInfo    * aCBOptParamInfo,
                              qllEnv                * aEnv );

stlStatus qlncCBOptCopiedBaseTable( qlncCBOptParamInfo  * aCBOptParamInfo,
                                    qllEnv              * aEnv );

stlStatus qlncCBOptBaseTableInternal( qlncCBOptParamInfo    * aCBOptParamInfo,
                                      qllEnv                * aEnv );

stlStatus qlncCBOptSubTable( qlncCBOptParamInfo     * aCBOptParamInfo,
                             qllEnv                 * aEnv );

stlStatus qlncCBOptJoinTable( qlncCBOptParamInfo    * aCBOptParamInfo,
                              qllEnv                * aEnv );

#if 0
stlStatus qlncCBOptFlatInstant( qlncCBOptParamInfo  * aCBOptParamInfo,
                                qllEnv              * aEnv );
#endif

stlStatus qlncCBOptSortInstant( qlncCBOptParamInfo  * aCBOptParamInfo,
                                qllEnv              * aEnv );

stlStatus qlncCBOptHashInstant( qlncCBOptParamInfo  * aCBOptParamInfo,
                                qllEnv              * aEnv );

stlStatus qlncCBOptInnerJoinTable( qlncCBOptParamInfo   * aCBOptParamInfo,
                                   qllEnv               * aEnv );

stlStatus qlncCBOptOuterJoinTable( qlncCBOptParamInfo   * aCBOptParamInfo,
                                   qllEnv               * aEnv );

stlStatus qlncCBOptSemiJoinTable( qlncCBOptParamInfo    * aCBOptParamInfo,
                                  qllEnv                * aEnv );

stlStatus qlncCBOptAntiSemiJoinTable( qlncCBOptParamInfo    * aCBOptParamInfo,
                                      qllEnv                * aEnv );

/* Best Join Table Tree 찾기위한 함수들 */

stlStatus qlncCBOptBestInnerJoinTable( qlncCBOptParamInfo   * aCBOptParamInfo,
                                       qllEnv               * aEnv );

stlStatus qlncCBOptFindBestInnerJoinTable( qlncCBOptParamInfo       * aCBOptParamInfo,
                                           stlInt32                   aNodeCount,
                                           qlncInnerJoinNodeInfo    * aInnerJoinNodeInfoArr,
                                           stlInt32                   aCurNodeIdx,
                                           stlInt32                 * aSeqNum,
                                           qlncAndFilter            * aJoinAndFilter,
                                           qlncNodeCommon          ** aBestInnerJoinNode,
                                           qllEnv                   * aEnv );

stlStatus qlncCBOptFindBestInnerJoinTable_Internal( qlncCBOptParamInfo      * aCBOptParamInfo,
                                                    stlInt32                  aNodeCount,
                                                    qlncInnerJoinNodeInfo   * aInnerJoinNodeInfoArr,
                                                    stlInt32                  aCurNodeIdx,
                                                    qlncAndFilter           * aJoinAndFilter,
                                                    qlncNodeCommon         ** aBestInnerJoinNode,
                                                    qllEnv                  * aEnv );

stlStatus qlncCBOptFindBestInnerJoinTable_Sparse( qlncCBOptParamInfo        * aCBOptParamInfo,
                                                  stlInt32                    aNodeCount,
                                                  qlncInnerJoinNodeInfo     * aInnerJoinNodeInfoArr,
                                                  stlInt32                    aCurNodeIdx,
                                                  stlInt32                  * aSeqNum,
                                                  qlncAndFilter             * aJoinAndFilter,
                                                  qlncNodeCommon           ** aBestInnerJoinNode,
                                                  qllEnv                    * aEnv );

stlStatus qlncCBOptBestOuterJoinTable( qlncCBOptParamInfo   * aCBOptParamInfo,
                                       qllEnv               * aEnv );

stlStatus qlncCBOptBestSemiJoinTable( qlncCBOptParamInfo    * aCBOptParamInfo,
                                      stlFloat64              aWeightValue,
                                      qllEnv                * aEnv );

stlStatus qlncCBOptBestAntiSemiJoinTable( qlncCBOptParamInfo    * aCBOptParamInfo,
                                          stlFloat64              aWeightValue,
                                          qllEnv                * aEnv );

/* Base Table Cost 계산 */
stlStatus qlncCBOptTableScanCost( qlncCBOptParamInfo    * aCBOptParamInfo,
                                  qllEnv                * aEnv );

stlStatus qlncCBOptIndexScanCost( qlncCBOptParamInfo    * aCBOptParamInfo,
                                  qlncIndexScanInfo     * aIndexScanInfo,
                                  stlBool                 aIsScanAsc,
                                  qlncExprCommon        * aInKeyRangeExpr,
                                  qllEnv                * aEnv );

stlStatus qlncCBOptIndexCombineCost( qlncCBOptParamInfo * aCBOptParamInfo,
                                     qllEnv             * aEnv );

stlStatus qlncCBOptRowidScanCost( qlncCBOptParamInfo    * aCBOptParamInfo,
                                  qllEnv                * aEnv );

/* Sub Table Cost 계산 */
stlStatus qlncCBOptFilterCost( qlncCBOptParamInfo   * aCBOptParamInfo,
                               qllEnv               * aEnv );

/* Join Table Cost 계산 */
stlStatus qlncCBOptInnerJoinCost( qlncCBOptParamInfo    * aCBOptParamInfo,
                                  qlncInnerJoinNodeInfo * aInnerJoinNodeInfoArr,
                                  stlInt32                aNodeCount,
                                  stlInt32                aNodeIdx,
                                  qllEnv                * aEnv );

stlStatus qlncCBOptOuterJoinCost( qlncCBOptParamInfo    * aCBOptParamInfo,
                                  qlncJoinDirect          aJoinDirect,
                                  qllEnv                * aEnv );

stlStatus qlncCBOptSemiJoinCost( qlncCBOptParamInfo     * aCBOptParamInfo,
                                 qlncJoinDirect           aJoinDirect,
                                 stlFloat64               aWeightValue,
                                 qllEnv                 * aEnv );

stlStatus qlncCBOptAntiSemiJoinCost( qlncCBOptParamInfo * aCBOptParamInfo,
                                     qlncJoinDirect       aJoinDirect,
                                     stlFloat64           aWeightValue,
                                     qllEnv             * aEnv );

stlStatus qlncCBOptNestedInnerJoinCost( qlncCBOptParamInfo  * aCBOptParamInfo,
                                        stlFloat64            aExpectedResultRowCount,
                                        stlBool               aPossibleNestedJoin,
                                        stlBool               aPossibleIndexNestedJoin,
                                        qllEnv              * aEnv );

stlStatus qlncCBOptMergeInnerJoinCost( qlncCBOptParamInfo   * aCBOptParamInfo,
                                       stlFloat64             aExpectedResultRowCount,
                                       qllEnv               * aEnv );

stlStatus qlncCBOptHashInnerJoinCost( qlncCBOptParamInfo    * aCBOptParamInfo,
                                      stlFloat64              aExpectedResultRowCount,
                                      qllEnv                * aEnv );

stlStatus qlncCBOptInnerJoinCombineCost( qlncCBOptParamInfo     * aCBOptParamInfo,
                                         qlncInnerJoinNodeInfo  * aInnerJoinNodeInfoArr,
                                         stlInt32                 aNodeCount,
                                         stlInt32                 aNodeIdx,
                                         stlFloat64               aExpectedResultRowCount,
                                         qllEnv                 * aEnv );

stlStatus qlncCBOptRecomputeNodeCostByPushFilter( qlncParamInfo     * aParamInfo,
                                                  qlncNodeCommon    * aNode,
                                                  qlncAndFilter     * aAndFilter,
                                                  qlncNodeCommon   ** aNewNode,
                                                  qllEnv            * aEnv );

stlStatus qlncCBOptNestedOuterJoinCost( qlncCBOptParamInfo  * aCBOptParamInfo,
                                        qlncJoinDirect        aJoinDirect,
                                        stlFloat64            aExpectedResultRowCount,
                                        qllEnv              * aEnv );

stlStatus qlncCBOptMergeOuterJoinCost( qlncCBOptParamInfo   * aCBOptParamInfo,
                                       qlncJoinDirect         aJoinDirect,
                                       stlFloat64             aExpectedResultRowCount,
                                       qllEnv               * aEnv );

stlStatus qlncCBOptHashOuterJoinCost( qlncCBOptParamInfo    * aCBOptParamInfo,
                                      qlncJoinDirect          aJoinDirect,
                                      stlFloat64              aExpectedResultRowCount,
                                      qllEnv                * aEnv );

stlStatus qlncCBOptNestedSemiJoinCost( qlncCBOptParamInfo   * aCBOptParamInfo,
                                       qlncJoinDirect         aJoinDirect,
                                       stlBool                aIsAntiSemi,
                                       stlFloat64             aExpectedResultRowCount,
                                       qllEnv               * aEnv );

stlStatus qlncCBOptInvertedNestedSemiJoinCost( qlncCBOptParamInfo   * aCBOptParamInfo,
                                               qlncJoinDirect         aJoinDirect,
                                               stlFloat64             aExpectedResultRowCount,
                                               qllEnv               * aEnv );

stlStatus qlncCBOptMergeSemiJoinCost( qlncCBOptParamInfo    * aCBOptParamInfo,
                                      qlncJoinDirect          aJoinDirect,
                                      stlFloat64              aExpectedResultRowCount,
                                      qllEnv                * aEnv );

stlStatus qlncCBOptHashSemiJoinCost( qlncCBOptParamInfo     * aCBOptParamInfo,
                                     qlncJoinDirect           aJoinDirect,
                                     stlFloat64               aExpectedResultRowCount,
                                     qllEnv                 * aEnv );

stlStatus qlncCBOptInvertedHashSemiJoinCost( qlncCBOptParamInfo * aCBOptParamInfo,
                                             qlncJoinDirect       aJoinDirect,
                                             stlFloat64           aExpectedResultRowCount,
                                             qllEnv             * aEnv );

/* Instant Cost 계산 */
#if 0
stlStatus qlncCBOptFlatInstantCost( qlncCBOptParamInfo  * aCBOptParamInfo,
                                    qllEnv              * aEnv );
#endif

stlStatus qlncCBOptSortInstantCost( qlncCBOptParamInfo  * aCBOptParamInfo,
                                    qllEnv              * aEnv );

stlStatus qlncCBOptHashInstantCost( qlncCBOptParamInfo  * aCBOptParamInfo,
                                    qllEnv              * aEnv );

/* Group Cost 계산 */
stlStatus qlncCBOptGroupCost( qlncCBOptParamInfo    * aCBOptParamInfo,
                              qllEnv                * aEnv );

/* SubQuery Cost 계산 함수들 */
extern const qlncCostBasedOptimizerFuncPtr qlncCBOptSubQueryFuncList[ QLNC_NODE_TYPE_MAX ];

stlStatus qlncCBOptSubQueryQuerySet( qlncCBOptParamInfo * aCBOptParamInfo,
                                     qllEnv             * aEnv );

stlStatus qlncCBOptSubQuerySetOP( qlncCBOptParamInfo    * aCBOptParamInfo,
                                  qllEnv                * aEnv );

stlStatus qlncCBOptSubQueryQuerySpec( qlncCBOptParamInfo    * aCBOptParamInfo,
                                      qllEnv                * aEnv );

stlStatus qlncCBOptSubQueryBaseTable( qlncCBOptParamInfo    * aCBOptParamInfo,
                                      qllEnv                * aEnv );

stlStatus qlncCBOptSubQuerySubTable( qlncCBOptParamInfo     * aCBOptParamInfo,
                                     qllEnv                 * aEnv );

stlStatus qlncCBOptSubQueryJoinTable( qlncCBOptParamInfo    * aCBOptParamInfo,
                                      qllEnv                * aEnv );

#if 0
stlStatus qlncCBOptSubQueryFlatInstant( qlncCBOptParamInfo  * aCBOptParamInfo,
                                        qllEnv              * aEnv );
#endif

stlStatus qlncCBOptSubQuerySortInstant( qlncCBOptParamInfo  * aCBOptParamInfo,
                                        qllEnv              * aEnv );

stlStatus qlncCBOptSubQueryHashInstant( qlncCBOptParamInfo  * aCBOptParamInfo,
                                        qllEnv              * aEnv );

stlStatus qlncCBOptSubQueryExprList( qlncParamInfo      * aParamInfo,
                                     qlncRefExprList    * aSubQueryExprList,
                                     qllEnv             * aEnv );

stlStatus qlncCBOptSubQueryAndFilter( qlncParamInfo     * aParamInfo,
                                      qlncAndFilter     * aAndFilter,
                                      qllEnv            * aEnv );



/* 공통 함수 */
stlStatus qlncCBOptAndFilterCostAndRowCount( stlChar        * aSQLString,
                                             qlncNodeCommon * aTableNode,
                                             qlncAndFilter  * aAndFilter,
                                             stlFloat64       aOrgRowCount,
                                             stlFloat64     * aFilterCost,
                                             stlFloat64     * aPhysicalRowCount,
                                             stlFloat64     * aResultRowCount,
                                             qllEnv         * aEnv );

stlFloat64 qlncGetLogicalFilterCost( qlncExprCommon * aFilter,
                                     stlFloat64       aRowCount );

stlFloat64 qlncGetExpectedResultCountByFilter( qlncExprCommon   * aFilter,
                                               stlBool            aIncludeStmtSubQuery,
                                               stlFloat64         aOrgRowCount );

void qlncCBOptRowidAndFilterCostAndRowCount( qlncAndFilter  * aAndFilter,
                                             stlFloat64       aOrgRowCount,
                                             stlFloat64     * aFilterCost,
                                             stlFloat64     * aResultRowCount );

stlBool qlncIsOnlyIndexColumnFilter( qlncNodeCommon     * aNode,
                                     qlncOrFilter       * aOrFilter,
                                     ellIndexKeyDesc    * aIndexKeyDesc,
                                     stlInt32             aIndexKeyCount );

stlInt32 qlncGetDistribCnfCount( qlncAndFilter  * aCnfAndFilter );

stlStatus qlncConvertCnf2Dnf( qlncParamInfo     * aParamInfo,
                              qlncAndFilter     * aCnfAndFilter,
                              stlInt32          * aDnfAndFilterCount,
                              qlncAndFilter    ** aDnfAndFilterArr,
                              qllEnv            * aEnv );

stlBool qlncIsExistJoinCondOnInnerJoinNode( qlncInnerJoinNodeInfo   * aInnerJoinNodeInfoArr,
                                            stlInt32                  aNodeCount,
                                            qlncAndFilter           * aAndFilter );

stlStatus qlncFindUsableFilterOnInnerJoinNode( qlncParamInfo            * aParamInfo,
                                               qlncInnerJoinTableNode   * aJoinNode,
                                               qlncAndFilter            * aJoinAndFilter,
                                               stlBool                    aIsLastJoinNode,
                                               qllEnv                   * aEnv );

stlStatus qlncIsPossibleMergeJoinCondition( stlChar             * aSQLString,
                                            qlncBooleanFilter   * aBooleanFilter,
                                            qlncNodeCommon      * aRightNode,
                                            stlBool             * aIsPossible,
                                            qllEnv              * aEnv );

stlStatus qlncIsPossibleHashJoinCondition( stlChar              * aSQLString,
                                           qlncBooleanFilter    * aBooleanFilter,
                                           qlncNodeCommon       * aHashNode,
                                           qlncAndFilter        * aEquiJoinCondition,
                                           stlBool              * aIsPossible,
                                           qllEnv               * aEnv );

stlBool qlncIsPossibleEquiHashJoinCondition( qlncExprCommon  * aColExpr,
                                             qlncNodeCommon  * aHashNode,
                                             qlncAndFilter   * aEquiJoinCondition );

stlStatus qlncIsPossibleInvertedNestedSemiJoin( qlncParamInfo           * aParamInfo,
                                                qlncSemiJoinTableNode   * aSemiJoinTableNode,
                                                stlBool                 * aOutIsPossible,
                                                qllEnv                  * aEnv );

stlStatus qlncFindPossibleJoinOperation( stlChar        * aSQLString,
                                         qlncAndFilter  * aJoinCondition,
                                         qlncJoinType     aJoinType,
                                         qlncNodeCommon * aLeftNode,
                                         qlncNodeCommon * aRightNode,
                                         stlBool          aIsSemiJoin,
                                         stlBool        * aPossibleNestedJoin,
                                         stlBool        * aPossibleIndexNestedJoin,
                                         stlBool        * aPossibleInvertedNestedJoin,
                                         stlBool        * aPossibleMergeJoin,
                                         stlBool        * aPossibleHashJoin,
                                         stlBool        * aPossibleInvertedHashJoin,
                                         qllEnv         * aEnv );

stlFloat64 qlncGetExpectedAndFilterResultCount( qlncAndFilter   * aAndFilter,
                                                stlBool           aIncludeStmtSubQuery,
                                                stlFloat64        aOrgRowCount );

stlFloat64 qlncGetExpectedOrFilterResultRowCount( qlncOrFilter  * aOrFilter,
                                                  stlBool         aIncludeStmtSubQuery,
                                                  stlFloat64      aOrgRowCount );

stlStatus qlncGetExpectedInnerJoinResultRowCount( qlncCBOptParamInfo    * aCBOptParamInfo,
                                                  qlncInnerJoinNodeInfo * aInnerJoinNodeInfoArr,
                                                  stlInt32                aNodeCount,
                                                  stlFloat64            * aResultRowCount,
                                                  qllEnv                * aEnv );

stlStatus qlncGetExpectedOuterJoinResultRowCount( qlncCBOptParamInfo    * aCBOptParamInfo,
                                                  qlncJoinDirect          aJoinDirect,
                                                  stlFloat64            * aResultRowCount,
                                                  qllEnv                * aEnv );

stlStatus qlncGetExpectedSemiJoinResultRowCount( qlncCBOptParamInfo     * aCBOptParamInfo,
                                                 qlncJoinDirect           aJoinDirect,
                                                 stlFloat64             * aResultRowCount,
                                                 qllEnv                 * aEnv );

stlStatus qlncGetExpectedAntiSemiJoinResultRowCount( qlncCBOptParamInfo     * aCBOptParamInfo,
                                                     qlncJoinDirect           aJoinDirect,
                                                     stlFloat64             * aResultRowCount,
                                                     qllEnv                 * aEnv );

stlFloat64 qlncGetCardEquiJoinColumnList( qlncBaseTableNode     * aCandBaseTable,
                                          stlInt32                aFindIndexKeyCount,
                                          qlncRefColumnList     * aRefColumnList,
                                          stlInt32                aEquiJoinCondCount,
                                          qlncBooleanFilter    ** aEquiJoinCondArr,
                                          stlFloat64            * aResultRate,
                                          stlBool               * aIsMatchedUnique );

stlBool qlncHasSubQuery( qlncNodeCommon * aNode );

void qlncFindIndexForSortKey( qlncBaseTableNode         * aBaseTableNode,
                              stlInt32                    aSortKeyCount,
                              qlncExprCommon           ** aSortKeyArr,
                              stlBool                     aNeedFullMatch,
                              stlBool                     aNeedOnlyUnique,
                              qlncIndexScanInfo        ** aUsableIndexScanInfo,
                              ellIndexColumnOrdering    * aIndexOrdering );

stlStatus qlncSeperateJoinConditionForMergeJoin( qlncParamInfo  * aParamInfo,
                                                 qlncAndFilter  * aJoinCondition,
                                                 qlncAndFilter ** aEquiJoinCondition,
                                                 qlncAndFilter ** aNonEquiJoinCondition,
                                                 qllEnv         * aEnv );

stlStatus qlncSeperateJoinConditionForHashJoin( qlncParamInfo   * aParamInfo,
                                                qlncNodeCommon  * aHashNode,
                                                qlncAndFilter   * aJoinCondition,
                                                qlncAndFilter  ** aEquiJoinCondition,
                                                qlncAndFilter  ** aNonEquiJoinCondition,
                                                qllEnv          * aEnv );

stlBool qlncHasKeyColForRange( qlncExprCommon  * aExpr,
                               qlncNodeCommon  * aTableNode );

stlBool qlncIsExistColumnInExpr( qlncExprCommon  * aExpr,
                                 qlncNodeCommon  * aTableNode );

stlStatus qlncAdjustAndFilterByCost( qlncParamInfo  * aParamInfo,
                                     qlncAndFilter  * aAndFilter,
                                     qllEnv         * aEnv );

stlStatus qlncPushDownSubQueryFilter( qlncParamInfo     * aParamInfo,
                                      qlncNodeCommon    * aNode,
                                      qlncNodeCommon    * aParentNode,
                                      qllEnv            * aEnv );

stlStatus qlncFindNodeForPushDownSubQueryFilter( qlncParamInfo      * aParamInfo,
                                                 qlncNodeCommon     * aCurNode,
                                                 qlncNodeCommon     * aParentNode,
                                                 qlncPushSubqType     aPushSubqType,
                                                 stlBool              aIsConstSubQuery,
                                                 stlFloat64         * aBestRowCount,
                                                 qlncOrFilter       * aOrFilter,
                                                 qlncNodeCommon    ** aFoundNode,
                                                 qlncNodeCommon    ** aFoundParentNode,
                                                 qlncOrFilter      ** aOutOrFilter,
                                                 qllEnv             * aEnv );

qlncNodeCommon * qlncFindParentNodeByCostOpt( qlncNodeCommon    * aCurNode,
                                              qlncNodeCommon    * aParentNode,
                                              qlncNodeCommon    * aFindNode);

stlBool qlncIsUsableOrFilterOnNode( qlncNodeCommon  * aNode,
                                    qlncOrFilter    * aOrFilter );

void qlncGetPushSubqType( qlncRefExprList   * aSubQueryExprList,
                          qlncPushSubqType  * aPushSubqType );

stlStatus qlncCBOptMakeSortInstantForNestedLoops( qlncParamInfo     * aParamInfo,
                                                  smlSortTableAttr  * aSortTableAttr,
                                                  qlncNodeCommon    * aChildNode,
                                                  qlncAndFilter     * aJoinCondition,
                                                  stlBool             aPushFilterToInstant,
                                                  qlncAndFilter    ** aOutJoinCondition,
                                                  qlncNodeCommon   ** aOutNode,
                                                  qllEnv            * aEnv );

stlBool qlncIsPossibleInKeyRange( qlncIndexScanCost * aIndexScanCost );

stlStatus qlncNeedRebuildInstant( qlncNodeCommon    * aInstantNode,
                                  stlBool           * aNeedRebuild,
                                  qllEnv            * aEnv );

stlStatus qlncCBOptHashInstantForPreorderedGroupBy( qlncParamInfo   * aParamInfo,
                                                    qlncNodeCommon  * aNode,
                                                    qllEnv          * aEnv );

stlStatus qlncCBOptRecomputeIndexScanCost( qlncParamInfo        * aParamInfo,
                                           qlncBaseTableNode    * aBaseTableNode,
                                           stlInt32               aIndexScanInfoIdx,
                                           qllEnv               * aEnv );

stlStatus qlncCBOptSeperateSortKeyFromJoinCondition( qlncParamInfo      * aParamInfo,
                                                     qlncNodeCommon     * aNode,
                                                     qlncAndFilter      * aJoinCondition,
                                                     stlInt32           * aSortKeyCount,
                                                     qlncExprCommon   *** aLeftSortKeyArr,
                                                     qlncExprCommon   *** aRightSortKeyArr,
                                                     qlncAndFilter      * aSortKeyAndFilter,
                                                     qlncAndFilter      * aNonSortKeyAndFilter,
                                                     qllEnv             * aEnv );

stlStatus qlncCBOptNeedUniqueCheckForINL( qlncParamInfo     * aParamInfo,
                                          qlncNodeCommon    * aNode,
                                          qlncAndFilter     * aJoinCondition,
                                          stlBool           * aNeed,
                                          qllEnv            * aEnv );


/** @} qlnc */

#endif /* _QLNCCOSTBASEDOPTIMIZER_H_ */
