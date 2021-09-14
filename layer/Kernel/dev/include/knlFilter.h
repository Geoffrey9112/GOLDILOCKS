/*******************************************************************************
 * knlFilter.h
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


#ifndef _KNL_FILTER_H_
#define _KNL_FILTER_H_ 1

/**
 * @file knlFilter.h
 * @brief Kernel Layer Filter Routines
 */

#include <knlExpression.h>


/**
 * @addtogroup knlFilter
 * @{
 */

/**
 * @brief SCAN/FILTER를 수행하기 위한 dtlExprEntry로 구성된 stack
 *
 * @remark < Comparison Stack과 Expr Result Stack >
 * <BR>   
 * <BR>   Comp Stack                Expr Result Stack
 * <BR>  ============                 ============   
 * <BR>  |          |                 |          |                 
 * <BR>  |  value2  |                 |          |
 * <BR>  |  value1  |                 |          |
 * <BR>  |  func1   |                 |          |
 * <BR>  ============                 ============
 * <BR>
 * <BR>  pop value2         =>        push value2 ( check action for value2 )
 * <BR>  pop value1         =>        push value1 ( check action for value1 )
 * <BR>
 * <BR>   Comp Stack                Expr Result Stack
 * <BR>  ============                 ============   
 * <BR>  |          |                 |          |                 
 * <BR>  |          |                 |          |
 * <BR>  |          |                 |  value1  |
 * <BR>  |  func1   |                 |  value2  |
 * <BR>  ============                 ============
 * <BR>
 * <BR>  pop func1          &         pop value1, value2
 * <BR>                               =>  execute func1( value1, value2)
 * <BR>                               =>  push result to Expr Result Stack ( check action for result )
 *
 * <BR>   Comp Stack                Expr Result Stack
 * <BR>  ============                 ============   
 * <BR>  |          |                 |          |                 
 * <BR>  |          |                 |          |
 * <BR>  |          |                 |          |
 * <BR>  |          |                 |  result  |  : result is dtlDataType value
 * <BR>  ============                 ============
 */


/*******************************************************************************
 * 최종 버전
 ******************************************************************************/

/**
 * @brief filter/range condition이 가지는 comparison stack의 기본 entry 개수
 */
#define KNL_CONDITION_STACK_DEFAULT_ENTRY_COUNT    ( DTL_FUNC_INPUTARG_VARIABLE_CNT * 3 )


/**
 * @brief contidion type
 */
typedef enum knlConditionType
{
    KNL_CONDITION_TYPE_RANGE = 0,    /**< range for index scan */
    KNL_CONDITION_TYPE_FILTER,       /**< filter for table scan */
    KNL_CONDITION_TYPE_MAX
} knlConditionType;


/**
 * @brief 
 */
typedef struct knlRangeElement
{
    stlInt32     mToalSize;          /**< 할당된 range element 크기 */
    stlInt32     mCount;             /**< 사용된 range element 개수 */
    stlUInt32  * mKeyColumns;        /**< Key Columns */
    stlUInt32  * mRangeValues;       /**< Range Values */
    stlBool    * mIsIncludeEqual;    /**< Is Include Equal condition : array */
    stlBool    * mIsAsc;             /**< Is Ascend Order : array */
    stlBool    * mIsNullFirst;       /**< Is Nulls First : array */
    stlBool    * mIsNullAlwaysStop;  /**< NULL 상수에 대한 Key Range STOP 여부 */ 
} knlRangeElement;


/**
 * @brief range predicate
 */
typedef struct knlRangePredicate
{
    knlRangeElement    * mMinRange;
    knlRangeElement    * mMaxRange;
    knlExprStack       * mKeyFilter;     /**< @todo key filter OR-list 만들기 */
} knlRangePredicate;


/**
 * @brief predicate
 */
typedef struct knlPredicate
{
    knlConditionType   mType;
    
    union
    {
        knlRangePredicate   * mRange;
        knlExprStack        * mFilter;
    } mCond;
    
} knlPredicate;


/**
 * @brief predicate의 list
 * 
 * @remark filter를 위한 predicate list는 하나의 element만으로 구성됨
 */
typedef struct knlPredicateList
{
    knlPredicate               mPredicate;
 
    struct knlPredicateList  * mNext;
} knlPredicateList;

/**
 * @brief Block Join Fetch Type 유형
 */
typedef enum knlBlockJoinType
{
    KNL_BLOCK_JOIN_NA = 0,              /**< N/A */

    KNL_BLOCK_JOIN_INNER_JOIN,          /**< Inner Join Block Fetch */
    KNL_BLOCK_JOIN_LEFT_OUTER_JOIN,     /**< Left Outer Join Block Fetch */
    KNL_BLOCK_JOIN_ANTI_OUTER_JOIN,     /**< Anti Outer Join Block Fetch */
    KNL_BLOCK_JOIN_SEMI_JOIN,           /**< Semi Join Block Fetch */
    KNL_BLOCK_JOIN_ANTI_SEMI_JOIN,      /**< Anti Semi Join Block Fetch */

    KNL_BLOCK_JOIN_MAX
} knlBlockJoinType;

/*
 * init value stack
 */

stlStatus knlAddRange( stlBool             aIsMinRange,
                       stlUInt32           aKeyColOffset,
                       stlUInt32           aRangeValOffset,
                       stlBool             aIsIncludeEqual,
                       stlBool             aIsAsc,
                       stlBool             aIsNullFirst,
                       stlBool             aIsNullAlwaysStop,
                       knlPredicateList  * aRangePredList,
                       knlEnv            * aEnv );

stlStatus knlMakeRange( knlRangeElement      * aMinRangeValue,
                        knlRangeElement      * aMaxRangeValue,
                        knlExprStack         * aKeyFilter,
                        knlPredicateList    ** aRangePredList,
                        knlRegionMem         * aMemMgr,
                        knlEnv               * aEnv );

stlStatus knlCreateRangePred( stlInt32               aMaxRangeCnt,
                              stlBool                aIsCreateContext,
                              knlPredicateList    ** aRangePredList,
                              knlExprContextInfo  ** aContextInfo,
                              knlRegionMem         * aMemMgr,
                              knlEnv               * aEnv );

stlStatus knlMakeFilter( knlExprStack         * aLogicalFilter,
                         knlPredicateList    ** aFilterPredList,
                         knlRegionMem         * aMemMgr,  
                         knlEnv               * aEnv );

stlStatus knlGetCompareListFromRangeElem( knlRangeElement      * aRangeElem,
                                          knlExprContextInfo   * aContextInfo,
                                          knlRegionMem         * aMemMgr,
                                          knlCompareList      ** aCompList,
                                          knlEnv               * aEnv );

stlStatus knlCreateKeyCompareList( stlInt32               aIndexColCount,
                                   knlKeyCompareList   ** aKeyCompareList,
                                   knlRegionMem         * aRegionMem,
                                   knlEnv               * aEnv );

                                          
/**
 * Predicate List의 Filter Expression 얻기
 * input  : knlPredicateList
 * output : knlExprStack*
 */
#define KNL_PRED_GET_FILTER( pred )  ( (pred)->mPredicate.mCond.mFilter )

/**
 * Predicate List의 Key Filter Expression 얻기
 * input  : knlPredicateList
 * output : knlExprStack*
 */
#define KNL_PRED_GET_KEY_FILTER( pred )  ( (pred)->mPredicate.mCond.mRange->mKeyFilter )


/**
 * Predicate List의 Min-Range Range Element 얻기
 * input  : knlPredicateList
 * output : knlRangeElement*
 */
#define KNL_PRED_GET_MIN_RANGE( pred )  ( (pred)->mPredicate.mCond.mRange->mMinRange )

/**
 * Predicate List의 Max-Range Range Element 얻기
 * input  : knlPredicateList
 * output : knlRangeElement*
 */
#define KNL_PRED_GET_MAX_RANGE( pred )  ( (pred)->mPredicate.mCond.mRange->mMaxRange )

/**
 * Predicate List의 Min-Range Range Compare List 얻기
 * input  : knlPredicateList
 * output : knlRangeElement*
 */
#define KNL_PRED_GET_MIN_RANGE_COMP_LIST( pred )                        \
    ( (pred)->mPredicate.mCond.mRange->mMinRange == NULL                \
      ? NULL                                                            \
      : (pred)->mPredicate.mCond.mRange->mMinRange->mCompareList )

/**
 * Predicate List의 Max-Range Range Compare List 얻기
 * input  : knlPredicateList
 * output : knlRangeElement*
 */
#define KNL_PRED_GET_MAX_RANGE_COMP_LIST( pred )                        \
    ( (pred)->mPredicate.mCond.mRange->mMaxRange == NULL                \
      ? NULL                                                            \
      : (pred)->mPredicate.mCond.mRange->mMaxRange->mCompareList )


/**
 * Predicate List의 Predicate Type 얻기
 * input  : knlPredicateList
 * output : knlConditionType
 */
#define KNL_PRED_GET_PREDICATE_TYPE( pred )  ( (pred)->mPredicate.mType )


/** @} */

#endif /* _KNL_FILTER_H_ */



