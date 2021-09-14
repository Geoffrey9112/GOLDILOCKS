/*******************************************************************************
 * qlncPostOptimizer.c
 *
 * Copyright (c) 2014, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file qlncPostOptimizer.c
 * @brief SQL Processor Layer SELECT statement post optimization
 */

#include <qll.h>
#include <qlDef.h>



/**
 * @addtogroup qlnc
 * @{
 */


/**
 * @brief Post Optimizer를 수행한다.
 * @param[in]   aParamInfo  Parameter Info
 * @param[in]   aNode       Candidate Node
 * @param[in]   aEnv        Environment
 */
stlStatus qlncPostOptimizer( qlncParamInfo  * aParamInfo,
                             qlncNodeCommon * aNode,
                             qllEnv         * aEnv )
{
    qlncNodeCommon  * sOutNode  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode != NULL, QLL_ERROR_STACK(aEnv) );


    /* Convert SubQuery to Semi/Anti-Semi Join */

    if( QLCR_NEED_TO_SET_ROWBLOCK( aParamInfo->mSQLStmt ) == STL_FALSE )
    {
        STL_TRY( qlncPostOptSubQueryUnnest( aParamInfo,
                                            aNode,
                                            1,
                                            &sOutNode,
                                            aEnv )
                 == STL_SUCCESS );

        /* 최상위 노드는 변경되지 않아야 한다. */
        STL_DASSERT( aNode == sOutNode );
    }

    /* Post Filter Push Down */
    STL_TRY( qlncPostOptFilterPushDown( aParamInfo,
                                        aNode,
                                        QLNC_JOIN_TYPE_INVALID,
                                        aEnv )
             == STL_SUCCESS );

    /* Group by Pre-ordered Node */
    /**
     * Pre-ordered Node를 이용한 Group by의 경우 반드시 마지막에 해야 한다.
     * 이유) 위 post opt에 의해 노드 구조가 바뀌거나 노드의 필터 등의 정보가
     *       바뀔 수 있다.
     */

    STL_TRY( qlncPostOptGroupByPreorderedNode( aParamInfo,
                                               aNode,
                                               NULL,
                                               aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief DML 구문에 대한 Post Optimizer를 수행한다.
 * @param[in]   aParamInfo  Parameter Info
 * @param[in]   aNode       Candidate Node
 * @param[out]  aOutNode    Candidate Output Node
 * @param[in]   aEnv        Environment
 */
stlStatus qlncPostOptimizer_DML( qlncParamInfo      * aParamInfo,
                                 qlncNodeCommon     * aNode,
                                 qlncNodeCommon    ** aOutNode,
                                 qllEnv             * aEnv )
{
    qlncNodeCommon  * sOutNode  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode != NULL, QLL_ERROR_STACK(aEnv) );


    /* Convert SubQuery to Semi/Anti-Semi Join */
    STL_TRY( qlncPostOptSubQueryUnnest( aParamInfo,
                                        aNode,
                                        1,
                                        &sOutNode,
                                        aEnv )
             == STL_SUCCESS );

    /* Post Filter Push Down */
    STL_TRY( qlncPostOptFilterPushDown( aParamInfo,
                                        sOutNode,
                                        QLNC_JOIN_TYPE_INVALID,
                                        aEnv )
             == STL_SUCCESS );

    /* Recompute Node */

    /* Output 설정 */
    *aOutNode = sOutNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SubQuery Filter에 대하여 Semi/Anti-Semi Join으로 변경한다.
 * @param[in]   aParamInfo      Parameter Info
 * @param[in]   aNode           Candidate Node
 * @param[in]   aWeightValue    Weight Value
 * @param[out]  aOutNode        Output Candidate Node
 * @param[in]   aEnv            Environment
 */
stlStatus qlncPostOptSubQueryUnnest( qlncParamInfo      * aParamInfo,
                                     qlncNodeCommon     * aNode,
                                     stlFloat64           aWeightValue,
                                     qlncNodeCommon    ** aOutNode,
                                     qllEnv             * aEnv )
{
    stlInt32          i;
    qlncNodeCommon  * sOutNode              = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode != NULL, QLL_ERROR_STACK(aEnv) );


    switch( aNode->mNodeType )
    {
        case QLNC_NODE_TYPE_QUERY_SET:
            {
                STL_TRY( qlncPostOptSubQueryUnnest(
                             aParamInfo,
                             ((qlncQuerySet*)aNode)->mChildNode,
                             aWeightValue,
                             &(((qlncQuerySet*)aNode)->mChildNode),
                             aEnv )
                         == STL_SUCCESS );

                sOutNode = aNode;

                break;
            }
        case QLNC_NODE_TYPE_SET_OP:
            {
                for( i = 0; i < ((qlncSetOP*)aNode)->mChildCount; i++ )
                {
                    STL_TRY( qlncPostOptSubQueryUnnest(
                                 aParamInfo,
                                 ((qlncSetOP*)aNode)->mChildNodeArr[i],
                                 aWeightValue,
                                 &(((qlncSetOP*)aNode)->mChildNodeArr[i]),
                                 aEnv )
                             == STL_SUCCESS );
                }

                sOutNode = aNode;

                break;
            }
        case QLNC_NODE_TYPE_QUERY_SPEC:
            {
                /* No Query Transformation Hint가 주어졌을 경우 더이상 수행하지 않는다. */
                if( ((qlncQuerySpec*)aNode)->mQueryTransformHint->mAllowedQueryTransform == STL_FALSE )
                {
                    sOutNode = aNode;
                    STL_THROW( RAMP_FINISH );
                }

                STL_TRY( qlncPostOptSubQueryUnnest(
                             aParamInfo,
                             ((qlncQuerySpecCost*)(aNode->mBestCost))->mChildNode,
                             aWeightValue,
                             &(((qlncQuerySpecCost*)(aNode->mBestCost))->mChildNode),
                             aEnv )
                         == STL_SUCCESS );

                sOutNode = aNode;

                break;
            }
        case QLNC_NODE_TYPE_BASE_TABLE:
            {
                STL_TRY( qlncPostOptSQUBaseTable( aParamInfo,
                                                  aNode,
                                                  aWeightValue,
                                                  &sOutNode,
                                                  aEnv )
                         == STL_SUCCESS );

                break;
            }
        case QLNC_NODE_TYPE_SUB_TABLE:
            {
                STL_TRY( qlncPostOptSQUSubTable( aParamInfo,
                                                 aNode,
                                                 aWeightValue,
                                                 &sOutNode,
                                                 aEnv )
                         == STL_SUCCESS );

                break;
            }
        case QLNC_NODE_TYPE_JOIN_TABLE:
            {
                if( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_INNER_JOIN )
                {
                    STL_TRY( qlncPostOptSubQueryUnnest(
                                 aParamInfo,
                                 (qlncNodeCommon*)(((qlncJoinTableNode*)aNode)->mBestInnerJoinTable),
                                 aWeightValue,
                                 &sOutNode,
                                 aEnv )
                             == STL_SUCCESS );
                }
                else if( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_OUTER_JOIN )
                {
                    STL_TRY( qlncPostOptSubQueryUnnest(
                                 aParamInfo,
                                 (qlncNodeCommon*)(((qlncJoinTableNode*)aNode)->mBestOuterJoinTable),
                                 aWeightValue,
                                 &sOutNode,
                                 aEnv )
                             == STL_SUCCESS );
                }
                else if( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_SEMI_JOIN )
                {
                    STL_TRY( qlncPostOptSubQueryUnnest(
                                 aParamInfo,
                                 (qlncNodeCommon*)(((qlncJoinTableNode*)aNode)->mBestSemiJoinTable),
                                 aWeightValue,
                                 &sOutNode,
                                 aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    STL_DASSERT( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_ANTI_SEMI_JOIN );
                    STL_TRY( qlncPostOptSubQueryUnnest(
                                 aParamInfo,
                                 (qlncNodeCommon*)(((qlncJoinTableNode*)aNode)->mBestAntiSemiJoinTable),
                                 aWeightValue,
                                 &sOutNode,
                                 aEnv )
                             == STL_SUCCESS );
                }

                if( sOutNode->mNodeType != QLNC_NODE_TYPE_JOIN_TABLE )
                {
                    /* Best Opt Node 초기화 */
                    ((qlncJoinTableNode*)aNode)->mBestInnerJoinTable = NULL;
                    ((qlncJoinTableNode*)aNode)->mBestOuterJoinTable = NULL;
                    ((qlncJoinTableNode*)aNode)->mBestSemiJoinTable = NULL;
                    ((qlncJoinTableNode*)aNode)->mBestAntiSemiJoinTable = NULL;

                    /* Node Type에 따른 설정 */
                    switch( sOutNode->mNodeType )
                    {
                        case QLNC_NODE_TYPE_INNER_JOIN_TABLE:
                            ((qlncJoinTableNode*)aNode)->mBestInnerJoinTable =
                                (qlncInnerJoinTableNode*)sOutNode;
                            aNode->mBestOptType = QLNC_BEST_OPT_TYPE_INNER_JOIN;
                            aNode->mBestCost = sOutNode->mBestCost;
                            break;

                        case QLNC_NODE_TYPE_OUTER_JOIN_TABLE:
                            ((qlncJoinTableNode*)aNode)->mBestOuterJoinTable =
                                (qlncOuterJoinTableNode*)sOutNode;
                            aNode->mBestOptType = QLNC_BEST_OPT_TYPE_OUTER_JOIN;
                            aNode->mBestCost = sOutNode->mBestCost;
                            break;

                        case QLNC_NODE_TYPE_SEMI_JOIN_TABLE:
                            ((qlncJoinTableNode*)aNode)->mBestSemiJoinTable =
                                (qlncSemiJoinTableNode*)sOutNode;
                            aNode->mBestOptType = QLNC_BEST_OPT_TYPE_SEMI_JOIN;
                            aNode->mBestCost = sOutNode->mBestCost;
                            break;

                        case QLNC_NODE_TYPE_ANTI_SEMI_JOIN_TABLE:
                            ((qlncJoinTableNode*)aNode)->mBestAntiSemiJoinTable =
                                (qlncAntiSemiJoinTableNode*)sOutNode;
                            aNode->mBestOptType = QLNC_BEST_OPT_TYPE_ANTI_SEMI_JOIN;
                            aNode->mBestCost = sOutNode->mBestCost;
                            break;

                        default:
                            STL_DASSERT( 0 );
                            break;
                    }

                    sOutNode = aNode;
                }

                break;
            }
        case QLNC_NODE_TYPE_INNER_JOIN_TABLE:
        case QLNC_NODE_TYPE_OUTER_JOIN_TABLE:
        case QLNC_NODE_TYPE_SEMI_JOIN_TABLE:
        case QLNC_NODE_TYPE_ANTI_SEMI_JOIN_TABLE:
            {
                STL_TRY( qlncPostOptSQUJoinTable( aParamInfo,
                                                  aNode,
                                                  aWeightValue,
                                                  &sOutNode,
                                                  aEnv )
                         == STL_SUCCESS );

                break;
            }
        case QLNC_NODE_TYPE_SORT_INSTANT:
            {
                /* Filter에 존재하는 SubQuery Expression 대한 SubQuery Unnest 수행 */
                QLNC_UNNEST_AND_FILTER( ((qlncSortInstantNode*)aNode)->mFilter );

                STL_TRY( qlncPostOptSubQueryUnnest(
                             aParamInfo,
                             ((qlncSortInstantNode*)aNode)->mChildNode,
                             aWeightValue,
                             &(((qlncSortInstantNode*)aNode)->mChildNode),
                             aEnv )
                         == STL_SUCCESS );

                sOutNode = aNode;

                break;
            }
        case QLNC_NODE_TYPE_HASH_INSTANT:
            {
                /* Filter에 존재하는 SubQuery Expression 대한 SubQuery Unnest 수행 */
                QLNC_UNNEST_AND_FILTER( ((qlncHashInstantNode*)aNode)->mFilter );

                STL_TRY( qlncPostOptSubQueryUnnest(
                             aParamInfo,
                             ((qlncHashInstantNode*)aNode)->mChildNode,
                             aWeightValue,
                             &(((qlncHashInstantNode*)aNode)->mChildNode),
                             aEnv )
                         == STL_SUCCESS );

                sOutNode = aNode;

                break;
            }
        default:
            {
                STL_DASSERT( 0 );

                sOutNode = aNode;

                break;
            }
    }


    STL_RAMP( RAMP_FINISH );


    /* Output 설정 */
    *aOutNode = sOutNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Base Table Node에 대한 SubQuery Unnest를 수행한다.
 * @param[in]   aParamInfo      Parameter Info
 * @param[in]   aWeightValue    Weight Value
 * @param[in]   aNode           Candidate Node
 * @param[out]  aOutNode        Output Candidate Node
 * @param[in]   aEnv            Environment
 */
stlStatus qlncPostOptSQUBaseTable( qlncParamInfo    * aParamInfo,
                                   qlncNodeCommon   * aNode,
                                   stlFloat64         aWeightValue,
                                   qlncNodeCommon  ** aOutNode,
                                   qllEnv           * aEnv )
{
    qlncRefExprList   sSubQueryFilterList;
    qlncAndFilter   * sSubQueryAndFilter    = NULL;
    qlncNodeCommon  * sTmpNode              = NULL;
    qlncNodeCommon  * sOutNode              = NULL;
    qlncRefExprItem * sRefExprItem          = NULL;
    qlncAndFilter     sTmpAndFilter;

    qlncCBOptParamInfo    sCBOptParamInfo;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE,
                        QLL_ERROR_STACK(aEnv) );


    QLNC_INIT_AND_FILTER( &sTmpAndFilter );
    if( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_TABLE_SCAN )
    {
        qlncTableScanCost   * sTableScanCost    = NULL;

        sTableScanCost = (qlncTableScanCost*)(aNode->mBestCost);

        /* Filter에 존재하는 SubQuery Expression 대한 SubQuery Unnest 수행 */
        QLNC_UNNEST_AND_FILTER( sTableScanCost->mTableAndFilter );

        /* Unnest 가능한 SubQuery Filter List 추출 */
        QLNC_INIT_LIST( &sSubQueryFilterList );
        STL_TRY( qlncPostOptExtractSubQueryFilter(
                     aParamInfo,
                     aNode,
                     sTableScanCost->mTableAndFilter,
                     &(sTableScanCost->mTableAndFilter),
                     &sSubQueryFilterList,
                     &sSubQueryAndFilter,
                     aEnv )
                 == STL_SUCCESS );

        if( (sTableScanCost->mTableAndFilter != NULL) &&
            (sTableScanCost->mTableAndFilter->mOrCount > 0) )
        {
            STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                        &sTmpAndFilter,
                                                        sTableScanCost->mTableAndFilter->mOrCount,
                                                        sTableScanCost->mTableAndFilter->mOrFilters,
                                                        aEnv )
                     == STL_SUCCESS );
        }
    }
    else if( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_INDEX_SCAN )
    {
        stlInt32              i;
        qlncExprFunction    * sFunction         = NULL;
        qlncIndexScanCost   * sIndexScanCost    = NULL;

        sIndexScanCost = (qlncIndexScanCost*)(aNode->mBestCost);

        /* Filter에 존재하는 SubQuery Expression 대한 SubQuery Unnest 수행 */
        QLNC_UNNEST_AND_FILTER( sIndexScanCost->mIndexRangeAndFilter );
        QLNC_UNNEST_AND_FILTER( sIndexScanCost->mIndexKeyAndFilter );
        QLNC_UNNEST_AND_FILTER( sIndexScanCost->mTableAndFilter );

        /* Unnest 가능한 SubQuery Filter List 추출 */
        QLNC_INIT_LIST( &sSubQueryFilterList );
        STL_TRY( qlncPostOptExtractSubQueryFilter(
                     aParamInfo,
                     aNode,
                     sIndexScanCost->mTableAndFilter,
                     &(sIndexScanCost->mTableAndFilter),
                     &sSubQueryFilterList,
                     &sSubQueryAndFilter,
                     aEnv )
                 == STL_SUCCESS );

        if( (sIndexScanCost->mIndexRangeAndFilter != NULL) &&
            (sIndexScanCost->mIndexRangeAndFilter->mOrCount > 0) )
        {
            STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                        &sTmpAndFilter,
                                                        sIndexScanCost->mIndexRangeAndFilter->mOrCount,
                                                        sIndexScanCost->mIndexRangeAndFilter->mOrFilters,
                                                        aEnv )
                     == STL_SUCCESS );

            /* Range Filter에는 In Key Range가 변형된 형태로 들어가 있을 수 있다. */
            if( sIndexScanCost->mListColMap != NULL )
            {
                for( i = 0; i < sTmpAndFilter.mOrCount; i++ )
                {
                    STL_DASSERT( sTmpAndFilter.mOrFilters[i]->mFilterCount == 1 );
                    STL_DASSERT( sTmpAndFilter.mOrFilters[i]->mFilters[0]->mType
                                 == QLNC_EXPR_TYPE_BOOLEAN_FILTER );
                    STL_DASSERT( ((qlncBooleanFilter*)(sTmpAndFilter.mOrFilters[i]->mFilters[0]))->mExpr->mType
                                 == QLNC_EXPR_TYPE_FUNCTION );

                    sFunction =
                        (qlncExprFunction*)(((qlncBooleanFilter*)(sTmpAndFilter.mOrFilters[i]->mFilters[0]))->mExpr);

                    /* In Key Range가 변경되면 항상 column = value 형태가 된다.
                     * 참고로 i1 = 1 같은 Filter는 column = const expr 형태이다.*/
                    if( (sFunction->mFuncId == KNL_BUILTIN_FUNC_IS_EQUAL) &&
                        (sFunction->mArgs[0]->mType == QLNC_EXPR_TYPE_COLUMN) &&
                        (sFunction->mArgs[1]->mType == QLNC_EXPR_TYPE_VALUE) )
                    {
                        sTmpAndFilter.mOrFilters[i] = NULL;
                    }
                }

                STL_TRY( qlncCompactAndFilter( &sTmpAndFilter, aEnv )
                         == STL_SUCCESS );

                STL_DASSERT( sIndexScanCost->mListColMap->mOrgFunc != NULL );
                STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                            &sTmpAndFilter,
                                                            1,
                                                            (qlncOrFilter**)(&(sIndexScanCost->mListColMap->mOrgFunc)),
                                                            aEnv )
                         == STL_SUCCESS );
            }
        }

        if( (sIndexScanCost->mIndexKeyAndFilter != NULL) &&
            (sIndexScanCost->mIndexKeyAndFilter->mOrCount > 0) )
        {
            STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                        &sTmpAndFilter,
                                                        sIndexScanCost->mIndexKeyAndFilter->mOrCount,
                                                        sIndexScanCost->mIndexKeyAndFilter->mOrFilters,
                                                        aEnv )
                     == STL_SUCCESS );
        }

        if( (sIndexScanCost->mTableAndFilter != NULL) &&
            (sIndexScanCost->mTableAndFilter->mOrCount > 0) )
        {
            STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                        &sTmpAndFilter,
                                                        sIndexScanCost->mTableAndFilter->mOrCount,
                                                        sIndexScanCost->mTableAndFilter->mOrFilters,
                                                        aEnv )
                     == STL_SUCCESS );
        }
    }
    else if( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_INDEX_COMBINE )
    {
        stlInt32      i;
        qlncIndexCombineCost    * sIndexCombineCost = NULL;

        sIndexCombineCost = (qlncIndexCombineCost*)(aNode->mBestCost);

        /* Index Combine인 경우 Index Scan Node에 대하여 Unnest를 하면 안된다. */
        for( i = 0; i < sIndexCombineCost->mDnfAndFilterCount; i++ )
        {
            /* Filter에 존재하는 SubQuery Expression 대한 SubQuery Unnest 수행 */
            QLNC_UNNEST_AND_FILTER( sIndexCombineCost->mIndexScanCostArr[i].mIndexRangeAndFilter );
            QLNC_UNNEST_AND_FILTER( sIndexCombineCost->mIndexScanCostArr[i].mIndexKeyAndFilter );
            QLNC_UNNEST_AND_FILTER( sIndexCombineCost->mIndexScanCostArr[i].mTableAndFilter );
        }

        sOutNode = aNode;
        STL_THROW( RAMP_FINISH );
    }
    else
    {
        qlncRowidScanCost   * sRowidScanCost    = NULL;

        STL_DASSERT( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_ROWID_SCAN );
        sRowidScanCost = (qlncRowidScanCost*)(aNode->mBestCost);

        /* Filter에 존재하는 SubQuery Expression 대한 SubQuery Unnest 수행 */
        QLNC_UNNEST_AND_FILTER( sRowidScanCost->mRowidAndFilter );
        QLNC_UNNEST_AND_FILTER( sRowidScanCost->mTableAndFilter );

        /* Unnest 가능한 SubQuery Filter List 추출 */
        QLNC_INIT_LIST( &sSubQueryFilterList );
        STL_TRY( qlncPostOptExtractSubQueryFilter(
                     aParamInfo,
                     aNode,
                     sRowidScanCost->mTableAndFilter,
                     &(sRowidScanCost->mTableAndFilter),
                     &sSubQueryFilterList,
                     &sSubQueryAndFilter,
                     aEnv )
                 == STL_SUCCESS );

        if( (sRowidScanCost->mRowidAndFilter != NULL) &&
            (sRowidScanCost->mRowidAndFilter->mOrCount > 0) )
        {
            STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                        &sTmpAndFilter,
                                                        sRowidScanCost->mRowidAndFilter->mOrCount,
                                                        sRowidScanCost->mRowidAndFilter->mOrFilters,
                                                        aEnv )
                     == STL_SUCCESS );
        }

        if( (sRowidScanCost->mTableAndFilter != NULL) &&
            (sRowidScanCost->mTableAndFilter->mOrCount > 0) )
        {
            STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                        &sTmpAndFilter,
                                                        sRowidScanCost->mTableAndFilter->mOrCount,
                                                        sRowidScanCost->mTableAndFilter->mOrFilters,
                                                        aEnv )
                     == STL_SUCCESS );
        }
    }

    if( sTmpAndFilter.mOrCount > 0 )
    {
        if( ((qlncBaseTableNode*)aNode)->mFilter == NULL )
        {
            /* @todo filter가 NULL인 경우가 없는 것으로 판단됨 */

            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncAndFilter ),
                                        (void**) &(((qlncBaseTableNode*)aNode)->mFilter),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
        }

        QLNC_INIT_AND_FILTER( ((qlncBaseTableNode*)aNode)->mFilter );

        STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                    ((qlncBaseTableNode*)aNode)->mFilter,
                                                    sTmpAndFilter.mOrCount,
                                                    sTmpAndFilter.mOrFilters,
                                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        ((qlncBaseTableNode*)aNode)->mFilter = NULL;
    }

    /* Recompute Base Table Node */
    if( sSubQueryFilterList.mCount > 0 )
    {
        /* Optimizer를 수행하도록 하기 위한 초기화 */
        ((qlncBaseTableNode*)aNode)->mNodeCommon.mBestOptType = QLNC_BEST_OPT_TYPE_NA;
        ((qlncBaseTableNode*)aNode)->mNodeCommon.mBestCost = NULL;
        ((qlncBaseTableNode*)aNode)->mNodeCommon.mIsAnalyzedCBO = STL_FALSE;

        ((qlncBaseTableNode*)aNode)->mTableScanCost = NULL;
        ((qlncBaseTableNode*)aNode)->mIndexScanCostCount = 0;
        ((qlncBaseTableNode*)aNode)->mIndexScanCostArr = NULL;
        ((qlncBaseTableNode*)aNode)->mIndexCombineCost = NULL;
        ((qlncBaseTableNode*)aNode)->mRowidScanCost = NULL;

        /* Base Table Node Cost 계산 */
        sCBOptParamInfo.mParamInfo = *aParamInfo;
        sCBOptParamInfo.mLeftNode = aNode;
        sCBOptParamInfo.mRightNode = NULL;
        sCBOptParamInfo.mJoinCond = NULL;
        sCBOptParamInfo.mFilter = NULL;
        sCBOptParamInfo.mCost = NULL;

        STL_TRY( qlncCBOptCopiedBaseTable( &sCBOptParamInfo,
                                           aEnv )
                 == STL_SUCCESS );
    }


    /* Unnest 수행 */
    sTmpNode = aNode;
    sRefExprItem = sSubQueryFilterList.mHead;
    while( sRefExprItem != NULL )
    {
        STL_TRY( qlncPostOptSQUMakeUnnestNode( aParamInfo,
                                               sTmpNode,
                                               aWeightValue,
                                               sRefExprItem->mExpr,
                                               (sRefExprItem->mNext == NULL
                                                ? sSubQueryAndFilter : NULL),
                                               &sTmpNode,
                                               aEnv )
                 == STL_SUCCESS );

        sRefExprItem = sRefExprItem->mNext;
    }

    sOutNode = sTmpNode;


    STL_RAMP( RAMP_FINISH );


    /**
     * Output 설정
     */

    *aOutNode = sOutNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Sub Table Node에 대한 SubQuery Unnest를 수행한다.
 * @param[in]   aParamInfo      Parameter Info
 * @param[in]   aNode           Candidate Node
 * @param[in]   aWeightValue    Weight Value
 * @param[out]  aOutNode        Output Candidate Node
 * @param[in]   aEnv            Environment
 */
stlStatus qlncPostOptSQUSubTable( qlncParamInfo     * aParamInfo,
                                  qlncNodeCommon    * aNode,
                                  stlFloat64          aWeightValue,
                                  qlncNodeCommon   ** aOutNode,
                                  qllEnv            * aEnv )
{
    qlncRefExprList   sSubQueryFilterList;
    qlncAndFilter   * sSubQueryAndFilter    = NULL;
    qlncNodeCommon  * sTmpNode              = NULL;
    qlncNodeCommon  * sOutNode              = NULL;
    qlncRefExprItem * sRefExprItem          = NULL;
    qlncFilterCost  * sFilterCost           = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE,
                        QLL_ERROR_STACK(aEnv) );


    STL_DASSERT( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_FILTER );

    sFilterCost = (qlncFilterCost*)(aNode->mBestCost);

    if( sFilterCost->mMaterializedNode != NULL )
    {
        /* Materialized Node에 대하여 수행 */
        STL_TRY( qlncPostOptSubQueryUnnest(
                     aParamInfo,
                     sFilterCost->mMaterializedNode,
                     aWeightValue,
                     &(sFilterCost->mMaterializedNode),
                     aEnv )
                 == STL_SUCCESS );

        sOutNode = aNode;
    }
    else
    {
        /* Filter에 존재하는 SubQuery Expression 대한 SubQuery Unnest 수행 */
        QLNC_UNNEST_AND_FILTER( sFilterCost->mFilter );

        /* Query Node에 대하여 먼저 수행 */
        STL_TRY( qlncPostOptSubQueryUnnest(
                     aParamInfo,
                     ((qlncSubTableNode*)aNode)->mQueryNode,
                     aWeightValue,
                     &(((qlncSubTableNode*)aNode)->mQueryNode),
                     aEnv )
                 == STL_SUCCESS );

        /* Unnest 가능한 SubQuery Filter List 추출 */
        QLNC_INIT_LIST( &sSubQueryFilterList );
        STL_TRY( qlncPostOptExtractSubQueryFilter(
                     aParamInfo,
                     aNode,
                     sFilterCost->mFilter,
                     &(sFilterCost->mFilter),
                     &sSubQueryFilterList,
                     &sSubQueryAndFilter,
                     aEnv )
                 == STL_SUCCESS );

        /* Unnest 수행 */
        sTmpNode = aNode;
        sRefExprItem = sSubQueryFilterList.mHead;
        while( sRefExprItem != NULL )
        {
            STL_TRY( qlncPostOptSQUMakeUnnestNode( aParamInfo,
                                                   sTmpNode,
                                                   aWeightValue,
                                                   sRefExprItem->mExpr,
                                                   (sRefExprItem->mNext == NULL
                                                    ? sSubQueryAndFilter : NULL),
                                                   &sTmpNode,
                                                   aEnv )
                     == STL_SUCCESS );

            sRefExprItem = sRefExprItem->mNext;
        }

        sOutNode = sTmpNode;
    }


    /**
     * Output 설정
     */

    *aOutNode = sOutNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Join Table Node에 대한 SubQuery Unnest를 수행한다.
 * @param[in]   aParamInfo      Parameter Info
 * @param[in]   aNode           Candidate Node
 * @param[in]   aWeightValue    Weight Value
 * @param[out]  aOutNode        Output Candidate Node
 * @param[in]   aEnv            Environment
 */
stlStatus qlncPostOptSQUJoinTable( qlncParamInfo    * aParamInfo,
                                   qlncNodeCommon   * aNode,
                                   stlFloat64         aWeightValue,
                                   qlncNodeCommon  ** aOutNode,
                                   qllEnv           * aEnv )
{
    qlncRefExprList   sSubQueryFilterList;
    qlncAndFilter   * sSubQueryAndFilter    = NULL;
    qlncNodeCommon  * sTmpNode              = NULL;
    qlncNodeCommon  * sOutNode              = NULL;
    qlncRefExprItem * sRefExprItem          = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aNode->mNodeType == QLNC_NODE_TYPE_INNER_JOIN_TABLE) ||
                        (aNode->mNodeType == QLNC_NODE_TYPE_OUTER_JOIN_TABLE) ||
                        (aNode->mNodeType == QLNC_NODE_TYPE_SEMI_JOIN_TABLE) ||
                        (aNode->mNodeType == QLNC_NODE_TYPE_ANTI_SEMI_JOIN_TABLE),
                        QLL_ERROR_STACK(aEnv) );


#define _QLNC_SET_SORT_INSTANT( _aChildNode, _aSortKeyArr, _aSortKeyCount, _aInstantType, _aOutNode ) \
    {                                                                                               \
        qlncCBOptParamInfo    _sCBOptParamInfo;                                                     \
        qlncNodeCommon      * _sCopiedNode      = NULL;                                             \
        stlUInt8            * _sSortKeyFlags    = NULL;                                             \
        stlInt32              _i;                                                                   \
        smlSortTableAttr      _sSortTableAttr   = { STL_FALSE,     /* not top-down */               \
                                                    STL_FALSE,     /* not volatile */               \
                                                    STL_TRUE,      /* leaf-only */                  \
                                                    STL_FALSE,     /* not unique */                 \
                                                    STL_FALSE };   /* not null excluded */          \
                                                                                                    \
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),                                     \
                                    STL_SIZEOF( stlUInt8 ) * (_aSortKeyCount),                      \
                                    (void**) &_sSortKeyFlags,                                       \
                                    KNL_ENV(aEnv) )                                                 \
                 == STL_SUCCESS );                                                                  \
                                                                                                    \
        stlMemset( _sSortKeyFlags, 0x00, STL_SIZEOF( stlInt8 ) * (_aSortKeyCount) );                \
                                                                                                    \
        for( _i = 0; _i < (_aSortKeyCount); _i++ )                                                  \
        {                                                                                           \
            DTL_SET_INDEX_COLUMN_FLAG( _sSortKeyFlags[_i],                                          \
                                       DTL_KEYCOLUMN_INDEX_ORDER_ASC,                               \
                                       DTL_KEYCOLUMN_INDEX_NULLABLE_TRUE,                           \
                                       DTL_KEYCOLUMN_INDEX_NULL_ORDER_LAST );                       \
        }                                                                                           \
                                                                                                    \
        STL_TRY( qlncCreateSortInstantForMergeJoin( aParamInfo,                                     \
                                                    (_aInstantType),                                \
                                                    &_sSortTableAttr,                               \
                                                    ELL_CURSOR_SCROLLABILITY_NO_SCROLL,             \
                                                    (_aSortKeyCount),                               \
                                                    (_aSortKeyArr),                                 \
                                                    _sSortKeyFlags,                                 \
                                                    (_aChildNode),                                  \
                                                    &_sCopiedNode,                                  \
                                                    aEnv )                                          \
                 == STL_SUCCESS );                                                                  \
                                                                                                    \
        /* Sort Instant Cost 계산 */                                                                \
        _sCBOptParamInfo.mParamInfo = *aParamInfo;                                                  \
        _sCBOptParamInfo.mLeftNode = _sCopiedNode;                                                  \
        _sCBOptParamInfo.mRightNode = NULL;                                                         \
        _sCBOptParamInfo.mJoinCond = NULL;                                                          \
        _sCBOptParamInfo.mFilter = NULL;                                                            \
        _sCBOptParamInfo.mCost = NULL;                                                              \
        STL_TRY( qlncCBOptSortInstant( &_sCBOptParamInfo,                                           \
                                       aEnv )                                                       \
                 == STL_SUCCESS );                                                                  \
                                                                                                    \
        (_aOutNode) = _sCopiedNode;                                                                 \
    }


    if( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_NESTED_LOOPS )
    {
        qlncNestedJoinCost  * sNestedJoinCost   = NULL;

        sNestedJoinCost = ((qlncNestedJoinCost*)(aNode->mBestCost));

        /* Filter에 존재하는 SubQuery Expression 대한 SubQuery Unnest 수행 */
        QLNC_UNNEST_AND_FILTER( sNestedJoinCost->mJoinCondition );
        QLNC_UNNEST_AND_FILTER( sNestedJoinCost->mJoinFilter );

        /* Left Node에 대한 SubQuery Unnest 수행 */
        STL_TRY( qlncPostOptSubQueryUnnest(
                     aParamInfo,
                     sNestedJoinCost->mLeftNode,
                     aWeightValue,
                     &(sNestedJoinCost->mLeftNode),
                     aEnv )
                 == STL_SUCCESS );

        /* Right Node에 대한 SubQuery Unnest 수행 */
        STL_TRY( qlncPostOptSubQueryUnnest(
                     aParamInfo,
                     sNestedJoinCost->mRightNode,
                     aWeightValue * sNestedJoinCost->mLeftNode->mBestCost->mResultRowCount,
                     &(sNestedJoinCost->mRightNode),
                     aEnv )
                 == STL_SUCCESS );

        /* Unnest 가능한 SubQuery Filter List 추출 */
        QLNC_INIT_LIST( &sSubQueryFilterList );
        STL_TRY( qlncPostOptExtractSubQueryFilter(
                     aParamInfo,
                     aNode,
                     sNestedJoinCost->mJoinFilter,
                     &(sNestedJoinCost->mJoinFilter),
                     &sSubQueryFilterList,
                     &sSubQueryAndFilter,
                     aEnv )
                 == STL_SUCCESS );
    }
    else if( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_MERGE )
    {
        qlncMergeJoinCost   * sMergeJoinCost    = NULL;

        sMergeJoinCost = ((qlncMergeJoinCost*)(aNode->mBestCost));

        /* Filter에 존재하는 SubQuery Expression 대한 SubQuery Unnest 수행 */
        QLNC_UNNEST_AND_FILTER( sMergeJoinCost->mEquiJoinCondition );
        QLNC_UNNEST_AND_FILTER( sMergeJoinCost->mNonEquiJoinCondition );
        QLNC_UNNEST_AND_FILTER( sMergeJoinCost->mJoinFilter );

        /* Left Node에 대한 SubQuery Unnest 수행 */
        if( sMergeJoinCost->mLeftNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE )
        {
            STL_TRY( qlncPostOptSubQueryUnnest(
                         aParamInfo,
                         sMergeJoinCost->mLeftNode,
                         aWeightValue,
                         &(sMergeJoinCost->mLeftNode),
                         aEnv )
                     == STL_SUCCESS );

            /* Index Scan에서 바뀌었으면 Sort Instant를 추가해준다. */
            if( sMergeJoinCost->mLeftNode->mNodeType != QLNC_NODE_TYPE_BASE_TABLE )
            {
                _QLNC_SET_SORT_INSTANT( sMergeJoinCost->mLeftNode,
                                        sMergeJoinCost->mLeftSortKeyArr,
                                        sMergeJoinCost->mSortKeyCount,
                                        QLNC_INSTANT_TYPE_NORMAL,
                                        sMergeJoinCost->mLeftNode );
            }
        }
        else
        {
            STL_TRY( qlncPostOptSubQueryUnnest(
                         aParamInfo,
                         sMergeJoinCost->mLeftNode,
                         aWeightValue,
                         &(sMergeJoinCost->mLeftNode),
                         aEnv )
                     == STL_SUCCESS );
        }

        /* Right Node에 대한 SubQuery Unnest 수행 */
        if( sMergeJoinCost->mRightNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE )
        {
            STL_TRY( qlncPostOptSubQueryUnnest(
                         aParamInfo,
                         sMergeJoinCost->mRightNode,
                         aWeightValue,
                         &(sMergeJoinCost->mRightNode),
                         aEnv )
                     == STL_SUCCESS );

            /* Index Scan에서 바뀌었으면 Sort Join Instant를 추가해준다. */
            if( sMergeJoinCost->mRightNode->mNodeType != QLNC_NODE_TYPE_BASE_TABLE )
            {
                _QLNC_SET_SORT_INSTANT( sMergeJoinCost->mRightNode,
                                        sMergeJoinCost->mRightSortKeyArr,
                                        sMergeJoinCost->mSortKeyCount,
                                        QLNC_INSTANT_TYPE_JOIN,
                                        sMergeJoinCost->mRightNode );
            }
        }
        else
        {
            STL_TRY( qlncPostOptSubQueryUnnest(
                         aParamInfo,
                         sMergeJoinCost->mRightNode,
                         aWeightValue,
                         &(sMergeJoinCost->mRightNode),
                         aEnv )
                     == STL_SUCCESS );
        }

        /* Unnest 가능한 SubQuery Filter List 추출 */
        QLNC_INIT_LIST( &sSubQueryFilterList );
        STL_TRY( qlncPostOptExtractSubQueryFilter(
                     aParamInfo,
                     aNode,
                     sMergeJoinCost->mJoinFilter,
                     &(sMergeJoinCost->mJoinFilter),
                     &sSubQueryFilterList,
                     &sSubQueryAndFilter,
                     aEnv )
                 == STL_SUCCESS );

    }
    else if( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_JOIN_COMBINE )
    {
        /* Join Combine인 경우 하위 Node에 SubQuery Filter가 존재하지 않는다. */
        sOutNode = aNode;
        STL_THROW( RAMP_FINISH );
    }
    else
    {
        qlncHashJoinCost    * sHashJoinCost = NULL;

        STL_DASSERT( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_HASH );
        sHashJoinCost = ((qlncHashJoinCost*)(aNode->mBestCost));

        /* Filter에 존재하는 SubQuery Expression 대한 SubQuery Unnest 수행 */
        QLNC_UNNEST_AND_FILTER( sHashJoinCost->mEquiJoinCondition );
        QLNC_UNNEST_AND_FILTER( sHashJoinCost->mNonEquiJoinCondition );
        QLNC_UNNEST_AND_FILTER( sHashJoinCost->mJoinFilter );

        /* Left Node에 대한 SubQuery Unnest 수행 */
        STL_TRY( qlncPostOptSubQueryUnnest(
                     aParamInfo,
                     sHashJoinCost->mLeftNode,
                     aWeightValue,
                     &(sHashJoinCost->mLeftNode),
                     aEnv )
                 == STL_SUCCESS );

        /* Right Node에 대한 SubQuery Unnest 수행 */
        STL_TRY( qlncPostOptSubQueryUnnest(
                     aParamInfo,
                     sHashJoinCost->mRightNode,
                     aWeightValue * sHashJoinCost->mLeftNode->mBestCost->mResultRowCount,
                     &(sHashJoinCost->mRightNode),
                     aEnv )
                 == STL_SUCCESS );

        /* Unnest 가능한 SubQuery Filter List 추출 */
        QLNC_INIT_LIST( &sSubQueryFilterList );
        STL_TRY( qlncPostOptExtractSubQueryFilter(
                     aParamInfo,
                     aNode,
                     sHashJoinCost->mJoinFilter,
                     &(sHashJoinCost->mJoinFilter),
                     &sSubQueryFilterList,
                     &sSubQueryAndFilter,
                     aEnv )
                 == STL_SUCCESS );
    }

    /* Unnest 수행 */
    sTmpNode = aNode;
    sRefExprItem = sSubQueryFilterList.mHead;
    while( sRefExprItem != NULL )
    {
        STL_TRY( qlncPostOptSQUMakeUnnestNode( aParamInfo,
                                               sTmpNode,
                                               aWeightValue,
                                               sRefExprItem->mExpr,
                                               (sRefExprItem->mNext == NULL
                                                ? sSubQueryAndFilter : NULL),
                                               &sTmpNode,
                                               aEnv )
                 == STL_SUCCESS );

        sRefExprItem = sRefExprItem->mNext;
    }

    sOutNode = sTmpNode;


    STL_RAMP( RAMP_FINISH );


    /**
     * Output 설정
     */

    *aOutNode = sOutNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Expression에서 SubQuery가 존재하면 해당 SubQuery의 Query Node에 대한 Unnest를 수행한다.
 * @param[in]   aParamInfo  Parameter Info
 * @param[in]   aExpr       Expression
 * @param[in]   aEnv        Environment
 */
stlStatus qlncPostOptSQUExpr( qlncParamInfo     * aParamInfo,
                              qlncExprCommon    * aExpr,
                              qllEnv            * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );

    
    STL_TRY_THROW( aExpr != NULL, RAMP_FINISH );

    switch( aExpr->mType )
    {
        case QLNC_EXPR_TYPE_VALUE:
        case QLNC_EXPR_TYPE_BIND_PARAM:
        case QLNC_EXPR_TYPE_COLUMN:
            {
                /* Do Nothing */
                break;
            }
        case QLNC_EXPR_TYPE_FUNCTION:
            {
                stlInt32              i;
                qlncExprFunction    * sFunction = NULL;

                sFunction = (qlncExprFunction*)aExpr;
                for( i = 0; i < sFunction->mArgCount; i++ )
                {
                    STL_TRY( qlncPostOptSQUExpr( aParamInfo,
                                                 sFunction->mArgs[i],
                                                 aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_EXPR_TYPE_CAST:
            {
                stlInt32              i;
                qlncExprTypeCast    * sTypeCast = NULL;

                sTypeCast = (qlncExprTypeCast*)aExpr;
                for( i = 0; i < sTypeCast->mArgCount; i++ )
                {
                    STL_TRY( qlncPostOptSQUExpr( aParamInfo,
                                                 sTypeCast->mArgs[i],
                                                 aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT:
            {
                STL_TRY( qlncPostOptSQUExpr( aParamInfo,
                                             ((qlncExprConstExpr*)aExpr)->mOrgExpr,
                                             aEnv )
                         == STL_SUCCESS );

                break;
            }
        case QLNC_EXPR_TYPE_SUB_QUERY:
            {
                STL_TRY( qlncPostOptSubQueryUnnest( aParamInfo,
                                                    ((qlncExprSubQuery*)aExpr)->mNode,
                                                    1,
                                                    &(((qlncExprSubQuery*)aExpr)->mNode),
                                                    aEnv )
                         == STL_SUCCESS );

                break;
            }
        case QLNC_EXPR_TYPE_INNER_COLUMN:
            {
                if( ((qlncExprInnerColumn*)aExpr)->mOrgExpr != NULL )
                {
                    STL_TRY( qlncPostOptSQUExpr( aParamInfo,
                                                 ((qlncExprInnerColumn*)aExpr)->mOrgExpr,
                                                 aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_EXPR_TYPE_ROWID_COLUMN:
            {
                /* Do Nothing */
                break;
            }
        case QLNC_EXPR_TYPE_AGGREGATION:
            {
                stlInt32              i;
                qlncExprAggregation * sAggregation  = NULL;

                sAggregation = (qlncExprAggregation*)aExpr;
                for( i = 0; i < sAggregation->mArgCount; i++ )
                {
                    STL_TRY( qlncPostOptSQUExpr( aParamInfo,
                                                 sAggregation->mArgs[i],
                                                 aEnv )
                             == STL_SUCCESS );
                }

                STL_TRY( qlncPostOptSQUExpr( aParamInfo,
                                             sAggregation->mFilter,
                                             aEnv )
                         == STL_SUCCESS );

                break;
            }
        case QLNC_EXPR_TYPE_CASE_EXPR:
            {
                stlInt32              i;
                qlncExprCaseExpr    * sCaseExpr = NULL;

                sCaseExpr = (qlncExprCaseExpr*)aExpr;
                for( i = 0; i < sCaseExpr->mCount; i++ )
                {
                    STL_TRY( qlncPostOptSQUExpr( aParamInfo,
                                                 sCaseExpr->mWhenArr[i],
                                                 aEnv )
                             == STL_SUCCESS );

                    STL_TRY( qlncPostOptSQUExpr( aParamInfo,
                                                 sCaseExpr->mThenArr[i],
                                                 aEnv )
                             == STL_SUCCESS );
                }

                STL_TRY( qlncPostOptSQUExpr( aParamInfo,
                                             sCaseExpr->mDefResult,
                                             aEnv )
                         == STL_SUCCESS );

                break;
            }
        case QLNC_EXPR_TYPE_LIST_FUNCTION:
            {
                stlInt32              i;
                qlncExprListFunc    * sListFunc = NULL;

                sListFunc = (qlncExprListFunc*)aExpr;
                for( i = 0; i < sListFunc->mArgCount; i++ )
                {
                    STL_TRY( qlncPostOptSQUExpr( aParamInfo,
                                                 sListFunc->mArgs[i],
                                                 aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_EXPR_TYPE_LIST_COLUMN:
            {
                stlInt32          i;
                qlncExprListCol * sListCol  = NULL;

                sListCol = (qlncExprListCol*)aExpr;
                for( i = 0; i < sListCol->mArgCount; i++ )
                {
                    STL_TRY( qlncPostOptSQUExpr( aParamInfo,
                                                 sListCol->mArgs[i],
                                                 aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_EXPR_TYPE_ROW_EXPR:
            {
                stlInt32          i;
                qlncExprRowExpr * sRowExpr  = NULL;

                sRowExpr = (qlncExprRowExpr*)aExpr;
                for( i = 0; i < sRowExpr->mArgCount; i++ )
                {
                    STL_TRY( qlncPostOptSQUExpr( aParamInfo,
                                                 sRowExpr->mArgs[i],
                                                 aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_EXPR_TYPE_AND_FILTER:
            {
                stlInt32          i;
                qlncAndFilter   * sAndFilter    = NULL;

                sAndFilter = (qlncAndFilter*)aExpr;
                for( i = 0; i < sAndFilter->mOrCount; i++ )
                {
                    if( sAndFilter->mOrFilters[i]->mSubQueryExprList != NULL )
                    {
                        STL_TRY( qlncPostOptSQUExpr( aParamInfo,
                                                     (qlncExprCommon*)(sAndFilter->mOrFilters[i]),
                                                     aEnv )
                                 == STL_SUCCESS );
                    }
                }

                break;
            }
        case QLNC_EXPR_TYPE_OR_FILTER:
            {
                stlInt32          i;
                qlncOrFilter    * sOrFilter = NULL;

                sOrFilter = (qlncOrFilter*)aExpr;
                for( i = 0; i < sOrFilter->mFilterCount; i++ )
                {
                    STL_TRY( qlncPostOptSQUExpr( aParamInfo,
                                                 sOrFilter->mFilters[i],
                                                 aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_EXPR_TYPE_BOOLEAN_FILTER:
            {
                STL_TRY( qlncPostOptSQUExpr( aParamInfo,
                                             ((qlncBooleanFilter*)aExpr)->mExpr,
                                             aEnv )
                         == STL_SUCCESS );

                break;
            }
        case QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER:
            {
                STL_TRY( qlncPostOptSQUExpr( aParamInfo,
                                             ((qlncConstBooleanFilter*)aExpr)->mExpr,
                                             aEnv )
                         == STL_SUCCESS );

                break;
            }
        /* case QLNC_EXPR_TYPE_PSEUDO_COLUMN: */
        /* case QLNC_EXPR_TYPE_PSEUDO_ARGUMENT: */
        /* case QLNC_EXPR_TYPE_REFERENCE: */
        /* case QLNC_EXPR_TYPE_OUTER_COLUMN: */
        default:
            {
                STL_DASSERT( 0 );
                break;
            }
    }


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SubQuery Filter에 대한 Unnest Node를 생성한다.
 * @param[in]   aParamInfo          Parameter Info
 * @param[in]   aLeftNode           Unnest Node에서의 Left Candidate Node
 * @param[in]   aWeightValue        Weight Value
 * @param[in]   aSubQueryFilter     SubQuery Filter Expression
 * @param[in]   aSubQueryAndFilter  SubQuery And Filter Expression
 * @param[out]  aOutNode            Output Candidate Node
 * @param[in]   aEnv                Environment
 */
stlStatus qlncPostOptSQUMakeUnnestNode( qlncParamInfo   * aParamInfo,
                                        qlncNodeCommon  * aLeftNode,
                                        stlFloat64        aWeightValue,
                                        qlncExprCommon  * aSubQueryFilter,
                                        qlncAndFilter   * aSubQueryAndFilter,
                                        qlncNodeCommon ** aOutNode,
                                        qllEnv          * aEnv )
{
    stlInt32              i;
    stlBool               sHasGroupBy;
    qlncExprListFunc    * sListFunc     = NULL;
    qlncExprCommon      * sLeftExpr     = NULL;
    qlncExprSubQuery    * sSubQuery     = NULL;
    qlncQuerySpec       * sQuerySpec    = NULL;

    qlncNodeArray         sRefNodeArray;
    qlncJoinType          sSetJoinType;
    knlBuiltInFunc        sSetFuncId;
    qlncJoinOperType      sSetJoinOperType;
    stlBool               sIsInverted;
    stlBool               sIsNullAware;

    qlncAndFilter         sOuterAndFilter;

    qlncAndFilter       * sSemiJoinAndFilter    = NULL;
    qlncJoinTableNode   * sSemiJoinTable        = NULL;

    qlncCBOptParamInfo    sCBOptParamInfo;
    stlBool               sNeedSubTable;
    qlncNodeCommon      * sRightNode            = NULL;
    qlncNodeCommon      * sTmpNode              = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSubQueryFilter != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSubQueryFilter->mType == QLNC_EXPR_TYPE_LIST_FUNCTION,
                        QLL_ERROR_STACK(aEnv) );


    /* List Function 설정 */
    sListFunc = (qlncExprListFunc*)aSubQueryFilter;

    /* Null Aware여부 설정 */
    switch( sListFunc->mFuncId )
    {
        case KNL_BUILTIN_FUNC_NOT_IN:
        case KNL_BUILTIN_FUNC_NOT_EQUAL_ALL:
        case KNL_BUILTIN_FUNC_EQUAL_ALL:
        case KNL_BUILTIN_FUNC_LESS_THAN_ALL:
        case KNL_BUILTIN_FUNC_LESS_THAN_EQUAL_ALL:
        case KNL_BUILTIN_FUNC_GREATER_THAN_ALL:
        case KNL_BUILTIN_FUNC_GREATER_THAN_EQUAL_ALL:
            if( qlncPostOptSQUIsPossibleAntiSemi( aSubQueryFilter ) == STL_TRUE )
            {
                sIsNullAware = STL_FALSE;
            }
            else
            {
                sIsNullAware = STL_TRUE;
            }

            break;

        default:
            sIsNullAware = STL_FALSE;
            break;
    }

    /* Left Expression 설정 */
    if( (sListFunc->mFuncId == KNL_BUILTIN_FUNC_EXISTS) ||
        (sListFunc->mFuncId == KNL_BUILTIN_FUNC_NOT_EXISTS) )
    {
        STL_DASSERT( sListFunc->mArgCount == 1 );
        sLeftExpr = NULL;
    }
    else
    {
        if( ((qlncExprRowExpr*)(((qlncExprListCol*)(sListFunc->mArgs[1]))->mArgs[0]))->mArgCount == 1 )
        {
            sLeftExpr = ((qlncExprRowExpr*)(((qlncExprListCol*)(sListFunc->mArgs[1]))->mArgs[0]))->mArgs[0];
        }
        else
        {
            sLeftExpr = ((qlncExprListCol*)(sListFunc->mArgs[1]))->mArgs[0];
        }
    }

    /* SubQuery Expression 설정 */
    sSubQuery = (qlncExprSubQuery*)(((qlncExprRowExpr*)(((qlncExprListCol*)(sListFunc->mArgs[0]))->mArgs[0]))->mArgs[0]);

    /* Query Spec 설정 */
    sQuerySpec = (qlncQuerySpec*)(sSubQuery->mNode);


    /* Group By Node가 존재하는지 체크 */
    if( ((qlncQuerySpecCost*)(sQuerySpec->mNodeCommon.mBestCost))->mChildNode->mNodeType == QLNC_NODE_TYPE_SORT_INSTANT )
    {
        sTmpNode = ((qlncSortInstantNode*)(((qlncQuerySpecCost*)(sQuerySpec->mNodeCommon.mBestCost))->mChildNode))->mChildNode;
    }
    else
    {
        sTmpNode = ((qlncQuerySpecCost*)(sQuerySpec->mNodeCommon.mBestCost))->mChildNode;
    }

    if( sTmpNode->mNodeType == QLNC_NODE_TYPE_HASH_INSTANT )
    {
        if( ((qlncHashInstantNode*)sTmpNode)->mInstantType == QLNC_INSTANT_TYPE_DISTINCT )
        {
            sTmpNode = ((qlncHashInstantNode*)sTmpNode)->mChildNode;
            if( sTmpNode->mNodeType == QLNC_NODE_TYPE_HASH_INSTANT )
            {
                STL_DASSERT( ((qlncHashInstantNode*)sTmpNode)->mInstantType == QLNC_INSTANT_TYPE_GROUP );
                sHasGroupBy = STL_TRUE;
            }
            else
            {
                sHasGroupBy = STL_FALSE;
            }
        }
        else
        {
            STL_DASSERT( ((qlncHashInstantNode*)sTmpNode)->mInstantType == QLNC_INSTANT_TYPE_GROUP );
            sHasGroupBy = STL_TRUE;
        }
    }
    else
    {
        sHasGroupBy = STL_FALSE;
    }


    /**
     * Group By가 없는 Aggregation이 존재하거나
     * Group By가 있고 Nested Aggregation이 존재하는 경우
     * SubQuery의 Node를 SubTable Node로 감싼다. 
     */

    /* Aggregation이 존재하면 Sub Table이 필요 */
    sNeedSubTable = STL_FALSE;
    for( i = 0; i < sQuerySpec->mTargetCount; i++ )
    {
        if( qlncHasAggregationOnExpr( sQuerySpec->mTargetArr[i].mExpr,
                                      sQuerySpec->mNodeCommon.mNodeID,
                                      (sHasGroupBy == STL_TRUE ? STL_TRUE : STL_FALSE) )
            == STL_TRUE )
        {
            sNeedSubTable = STL_TRUE;
            break;
        }
    }

    /* SubTable이 필요한 경우에 대한 처리 */
    if( sNeedSubTable == STL_TRUE )
    {
        STL_TRY( qlncCreateSubTable( aParamInfo,
                                     sSubQuery->mNode,
                                     &sRightNode,
                                     aEnv )
                 == STL_SUCCESS );

        /* Sub Table Node에 대한 Cost 계산 */
        sCBOptParamInfo.mParamInfo = *aParamInfo;
        sCBOptParamInfo.mLeftNode = sRightNode;
        sCBOptParamInfo.mRightNode = NULL;
        sCBOptParamInfo.mJoinCond = NULL;
        sCBOptParamInfo.mFilter = NULL;
        sCBOptParamInfo.mCost = NULL;

        STL_TRY( qlncCBOptSubTable( &sCBOptParamInfo,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sRightNode = ((qlncQuerySpecCost*)(sQuerySpec->mNodeCommon.mBestCost))->mChildNode;
    }

    /* Node Array 초기화 */
    STL_TRY( qlncInitializeNodeArray( &QLL_CANDIDATE_MEM( aEnv ),
                                      &sRefNodeArray,
                                      aEnv )
             == STL_SUCCESS );

    /* Node Array 생성 */
    STL_TRY( qlncMakeNodeArrayOnCurrNode( &sRefNodeArray,
                                          aLeftNode,
                                          aEnv )
             == STL_SUCCESS );

    /* Semi-Join/Anti-Semi-Join을 위한 Outer And Filter 가져오기 */
    QLNC_INIT_AND_FILTER( &sOuterAndFilter );
    STL_TRY( qlncPostOptSQUExtractOuterAndFilter( aParamInfo,
                                                  sRightNode,
                                                  &sRefNodeArray,
                                                  &sOuterAndFilter,
                                                  aEnv )
             == STL_SUCCESS );

    /* Function에 따른 Join Type 및 Function ID 설정 */
    switch( sListFunc->mFuncId )
    {
        case KNL_BUILTIN_FUNC_IN:
            sSetFuncId = KNL_BUILTIN_FUNC_IS_EQUAL;
            sSetJoinType = QLNC_JOIN_TYPE_SEMI;
            break;

        case KNL_BUILTIN_FUNC_NOT_IN:
            sSetFuncId = KNL_BUILTIN_FUNC_IS_EQUAL;
            sSetJoinType = ( sIsNullAware == STL_TRUE
                             ? QLNC_JOIN_TYPE_ANTI_SEMI_NA
                             : QLNC_JOIN_TYPE_ANTI_SEMI );
            break;

        case KNL_BUILTIN_FUNC_EXISTS:
            sSetFuncId = KNL_BUILTIN_FUNC_IS_EQUAL;
            sSetJoinType = QLNC_JOIN_TYPE_SEMI;
            break;

        case KNL_BUILTIN_FUNC_NOT_EXISTS:
            sSetFuncId = KNL_BUILTIN_FUNC_IS_EQUAL;
            sSetJoinType = QLNC_JOIN_TYPE_ANTI_SEMI;
            break;

        case KNL_BUILTIN_FUNC_EQUAL_ANY:
            sSetFuncId = KNL_BUILTIN_FUNC_IS_EQUAL;
            sSetJoinType = QLNC_JOIN_TYPE_SEMI;
            break;

        case KNL_BUILTIN_FUNC_NOT_EQUAL_ANY:
            sSetFuncId = KNL_BUILTIN_FUNC_IS_NOT_EQUAL;
            sSetJoinType = QLNC_JOIN_TYPE_SEMI;
            break;

        case KNL_BUILTIN_FUNC_LESS_THAN_ANY:
            sSetFuncId = KNL_BUILTIN_FUNC_IS_LESS_THAN;
            sSetJoinType = QLNC_JOIN_TYPE_SEMI;
            break;

        case KNL_BUILTIN_FUNC_LESS_THAN_EQUAL_ANY:
            sSetFuncId = KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL;
            sSetJoinType = QLNC_JOIN_TYPE_SEMI;
            break;

        case KNL_BUILTIN_FUNC_GREATER_THAN_ANY:
            sSetFuncId = KNL_BUILTIN_FUNC_IS_GREATER_THAN;
            sSetJoinType = QLNC_JOIN_TYPE_SEMI;
            break;

        case KNL_BUILTIN_FUNC_GREATER_THAN_EQUAL_ANY:
            sSetFuncId = KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL;
            sSetJoinType = QLNC_JOIN_TYPE_SEMI;
            break;

        case KNL_BUILTIN_FUNC_EQUAL_ALL:
            sSetFuncId = KNL_BUILTIN_FUNC_IS_NOT_EQUAL;
            sSetJoinType = ( sIsNullAware == STL_TRUE
                             ? QLNC_JOIN_TYPE_ANTI_SEMI_NA
                             : QLNC_JOIN_TYPE_ANTI_SEMI );
            break;

        case KNL_BUILTIN_FUNC_NOT_EQUAL_ALL:
            sSetFuncId = KNL_BUILTIN_FUNC_IS_EQUAL;
            sSetJoinType = ( sIsNullAware == STL_TRUE
                             ? QLNC_JOIN_TYPE_ANTI_SEMI_NA
                             : QLNC_JOIN_TYPE_ANTI_SEMI );
            break;

        case KNL_BUILTIN_FUNC_LESS_THAN_ALL:
            sSetFuncId = KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL;
            sSetJoinType = ( sIsNullAware == STL_TRUE
                             ? QLNC_JOIN_TYPE_ANTI_SEMI_NA
                             : QLNC_JOIN_TYPE_ANTI_SEMI );
            break;

        case KNL_BUILTIN_FUNC_LESS_THAN_EQUAL_ALL:
            sSetFuncId = KNL_BUILTIN_FUNC_IS_GREATER_THAN;
            sSetJoinType = ( sIsNullAware == STL_TRUE
                             ? QLNC_JOIN_TYPE_ANTI_SEMI_NA
                             : QLNC_JOIN_TYPE_ANTI_SEMI );
            break;

        case KNL_BUILTIN_FUNC_GREATER_THAN_ALL:
            sSetFuncId = KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL;
            sSetJoinType = ( sIsNullAware == STL_TRUE
                             ? QLNC_JOIN_TYPE_ANTI_SEMI_NA
                             : QLNC_JOIN_TYPE_ANTI_SEMI );
            break;

        case KNL_BUILTIN_FUNC_GREATER_THAN_EQUAL_ALL:
            sSetFuncId = KNL_BUILTIN_FUNC_IS_LESS_THAN;
            sSetJoinType = ( sIsNullAware == STL_TRUE
                             ? QLNC_JOIN_TYPE_ANTI_SEMI_NA
                             : QLNC_JOIN_TYPE_ANTI_SEMI );
            break;

        default:
            STL_DASSERT( 0 );
            sSetFuncId = KNL_BUILTIN_FUNC_INVALID;
            sSetJoinType = QLNC_JOIN_TYPE_INVALID;
            break;
    }


    /**
     * Join Type 및 Join Operation Type, Function ID 설정
     */

    /* Join Hint에 의한 Join Operation Type 및 Join Type 설정 */
    if( sSetJoinType == QLNC_JOIN_TYPE_SEMI )
    {
        switch( sQuerySpec->mQueryTransformHint->mUnnestOperType )
        {
            case QLNC_UNNEST_OPER_TYPE_NL_ISJ:
                sIsInverted = STL_TRUE;
                sSetJoinOperType = QLNC_JOIN_OPER_TYPE_USE_NESTED;
                break;
            case QLNC_UNNEST_OPER_TYPE_NL_SJ:
                sIsInverted = STL_FALSE;
                sSetJoinOperType = QLNC_JOIN_OPER_TYPE_USE_NESTED;
                break;
            case QLNC_UNNEST_OPER_TYPE_MERGE_SJ:
                sIsInverted = STL_FALSE;
                sSetJoinOperType = QLNC_JOIN_OPER_TYPE_USE_MERGE;
                break;
            case QLNC_UNNEST_OPER_TYPE_HASH_ISJ:
                sIsInverted = STL_TRUE;
                sSetJoinOperType = QLNC_JOIN_OPER_TYPE_USE_HASH;
                break;
            case QLNC_UNNEST_OPER_TYPE_HASH_SJ:
                sIsInverted = STL_FALSE;
                sSetJoinOperType = QLNC_JOIN_OPER_TYPE_USE_HASH;
                break;
            case QLNC_UNNEST_OPER_TYPE_DEFAULT:
            case QLNC_UNNEST_OPER_TYPE_NL_AJ:
            case QLNC_UNNEST_OPER_TYPE_MERGE_AJ:
            case QLNC_UNNEST_OPER_TYPE_HASH_AJ:
            default:
                sIsInverted = STL_FALSE;
                sSetJoinOperType = QLNC_JOIN_OPER_TYPE_DEFAULT;
                break;
        }
    }
    else
    {
        STL_DASSERT( (sSetJoinType == QLNC_JOIN_TYPE_ANTI_SEMI) ||
                     (sSetJoinType == QLNC_JOIN_TYPE_ANTI_SEMI_NA) );

        switch( sQuerySpec->mQueryTransformHint->mUnnestOperType )
        {
            case QLNC_UNNEST_OPER_TYPE_NL_AJ:
                sSetJoinOperType = QLNC_JOIN_OPER_TYPE_USE_NESTED;
                sIsInverted = STL_FALSE;
                break;
            case QLNC_UNNEST_OPER_TYPE_MERGE_AJ:
                sIsInverted = STL_FALSE;
                sSetJoinOperType = QLNC_JOIN_OPER_TYPE_USE_MERGE;
                break;
            case QLNC_UNNEST_OPER_TYPE_HASH_AJ:
                sIsInverted = STL_FALSE;
                sSetJoinOperType = QLNC_JOIN_OPER_TYPE_USE_HASH;
                break;
            case QLNC_UNNEST_OPER_TYPE_DEFAULT:
            case QLNC_UNNEST_OPER_TYPE_NL_ISJ:
            case QLNC_UNNEST_OPER_TYPE_NL_SJ:
            case QLNC_UNNEST_OPER_TYPE_MERGE_SJ:
            case QLNC_UNNEST_OPER_TYPE_HASH_ISJ:
            case QLNC_UNNEST_OPER_TYPE_HASH_SJ:
            default:
                sIsInverted = STL_FALSE;
                sSetJoinOperType = QLNC_JOIN_OPER_TYPE_DEFAULT;
                break;
        }
    }

    /* 아직 SubQuery의 Node에 Outer Table을 참조하는 Column이 존재하면
     * Join Operation을 Nested Loops로 변경 */
    if( qlncIsExistOuterColumnByRefNodeOnCandNode( sRightNode, &sRefNodeArray )
        == STL_TRUE )
    {
        sSetJoinOperType = QLNC_JOIN_OPER_TYPE_USE_NESTED;
        sIsInverted = STL_FALSE;
    }


    /**
     * @todo 현재 Null Aware Anti Semi Join은 Hash Anti Semi Join만 가능하다.
     */

    if( sSetJoinType == QLNC_JOIN_TYPE_ANTI_SEMI_NA )
    {
        sSetJoinOperType = QLNC_JOIN_OPER_TYPE_USE_HASH;
        sIsInverted = STL_FALSE;
    }


    /**
     * Semi-Join/Anti-Semi-Join Filter 생성
     */

    if( (sSetJoinType == QLNC_JOIN_TYPE_SEMI) ||
        (sSetJoinType == QLNC_JOIN_TYPE_ANTI_SEMI) )
    {
        /* Outer And Filter에서 SubQuery Filter를 aSubQueryAndFilter로 이동 */
        for( i = 0; i < sOuterAndFilter.mOrCount; i++ )
        {
            if( sOuterAndFilter.mOrFilters[i]->mSubQueryExprList != NULL )
            {
                /* @todo 현재 OuterAndFilter에는 SubQuery가 존재하지 않는다. */

#if 0
                if( aSubQueryAndFilter == NULL )
                {
                    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                STL_SIZEOF( qlncAndFilter ),
                                                (void**) &aSubQueryAndFilter,
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );
                    QLNC_INIT_AND_FILTER( aSubQueryAndFilter );
                }

                STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                            aSubQueryAndFilter,
                                                            1,
                                                            &(sOuterAndFilter.mOrFilters[i]),
                                                            aEnv )
                         == STL_SUCCESS );

                sOuterAndFilter.mOrFilters[i] = NULL;
#endif
            }
        }

        STL_TRY( qlncCompactAndFilter( &sOuterAndFilter, aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_DASSERT( sSetJoinType == QLNC_JOIN_TYPE_ANTI_SEMI_NA );

        /* 모든 Outer And Filter를 Filter로 등록하도록 한다. */
        if( sOuterAndFilter.mOrCount > 0 )
        {
            if( aSubQueryAndFilter == NULL )
            {
                STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                            STL_SIZEOF( qlncAndFilter ),
                                            (void**) &aSubQueryAndFilter,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );
                QLNC_INIT_AND_FILTER( aSubQueryAndFilter );
            }

            STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                        aSubQueryAndFilter,
                                                        sOuterAndFilter.mOrCount,
                                                        sOuterAndFilter.mOrFilters,
                                                        aEnv )
                     == STL_SUCCESS );
            QLNC_INIT_AND_FILTER( &sOuterAndFilter );
        }
    }

    if( sNeedSubTable == STL_TRUE )
    {
        STL_TRY( qlncPostOptSQUMakeSemiJoinAndFilter(
                     aParamInfo,
                     sListFunc,
                     sLeftExpr,
                     ((qlncSubTableNode*)sRightNode)->mReadColumnList.mCount,
                     ((qlncSubTableNode*)sRightNode)->mReadColumnList.mHead,
                     NULL,
                     sSetFuncId,
                     &sOuterAndFilter,
                     &sSemiJoinAndFilter,
                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( qlncPostOptSQUMakeSemiJoinAndFilter(
                     aParamInfo,
                     sListFunc,
                     sLeftExpr,
                     sQuerySpec->mTargetCount,
                     NULL,
                     sQuerySpec->mTargetArr,
                     sSetFuncId,
                     &sOuterAndFilter,
                     &sSemiJoinAndFilter,
                     aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Semi-Join/Anti-Semi-Join Node 처리
     */

    /* Semi-Join/Anti-Semi-Join Node 생성 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncJoinTableNode ),
                                (void**) &sSemiJoinTable,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_INIT_NODE_COMMON( &(sSemiJoinTable->mNodeCommon),
                           *(aParamInfo->mGlobalNodeID),
                           QLNC_NODE_TYPE_JOIN_TABLE );
    (*(aParamInfo->mGlobalNodeID))++;  /* Global ID 증가 */

    STL_TRY( qlncInitializeNodeArray( &QLL_CANDIDATE_MEM( aEnv ),
                                      &(sSemiJoinTable->mNodeArray),
                                      aEnv )
             == STL_SUCCESS );

    sSemiJoinTable->mJoinCond = sSemiJoinAndFilter;
    sSemiJoinTable->mFilter = aSubQueryAndFilter;
    sSemiJoinTable->mJoinType = sSetJoinType;
    sSemiJoinTable->mJoinDirect = QLNC_JOIN_DIRECT_LEFT;
    sSemiJoinTable->mNamedColumnList = NULL;

    /* Semi-Join Hint 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncJoinHint ),
                                (void**) &(sSemiJoinTable->mSemiJoinHint),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    QLNC_INIT_JOIN_HINT( sSemiJoinTable->mSemiJoinHint );

    /* Right Node의 Join Operation Type 변경 */
    if( sRightNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE )
    {
        ((qlncBaseTableNode*)sRightNode)->mJoinHint->mIsInverted = sIsInverted;
        ((qlncBaseTableNode*)sRightNode)->mJoinHint->mJoinOperation = sSetJoinOperType;
    }
    else if( sRightNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE )
    {
        ((qlncSubTableNode*)sRightNode)->mJoinHint->mIsInverted = sIsInverted;
        ((qlncSubTableNode*)sRightNode)->mJoinHint->mJoinOperation = sSetJoinOperType;
    }
    else if( sRightNode->mNodeType == QLNC_NODE_TYPE_JOIN_TABLE )
    {
        ((qlncJoinTableNode*)sRightNode)->mSemiJoinHint->mIsInverted = sIsInverted;
        ((qlncJoinTableNode*)sRightNode)->mSemiJoinHint->mJoinOperation = sSetJoinOperType;
    }
    else if( sRightNode->mNodeType == QLNC_NODE_TYPE_SORT_INSTANT )
    {
        ((qlncSortInstantNode*)sRightNode)->mSemiJoinHint->mIsInverted = sIsInverted;
        ((qlncSortInstantNode*)sRightNode)->mSemiJoinHint->mJoinOperation = sSetJoinOperType;
    }
    else
    {
        STL_DASSERT( sRightNode->mNodeType == QLNC_NODE_TYPE_HASH_INSTANT );
        ((qlncHashInstantNode*)sRightNode)->mSemiJoinHint->mIsInverted = sIsInverted;
        ((qlncHashInstantNode*)sRightNode)->mSemiJoinHint->mJoinOperation = sSetJoinOperType;
    }

    /* Left Node가 Base Table이고, Index Scan이면 Merge Join에 의해 선택된 Index일 수 있으므로
     * Cost를 재계산하도록 한다. */
    if( aLeftNode->mBestOptType == QLNC_BEST_OPT_TYPE_INDEX_SCAN )
    {
        /* Optimizer를 수행하도록 하기 위한 초기화 */
        ((qlncBaseTableNode*)aLeftNode)->mNodeCommon.mBestOptType = QLNC_BEST_OPT_TYPE_NA;
        ((qlncBaseTableNode*)aLeftNode)->mNodeCommon.mBestCost = NULL;
        ((qlncBaseTableNode*)aLeftNode)->mNodeCommon.mIsAnalyzedCBO = STL_FALSE;

        ((qlncBaseTableNode*)aLeftNode)->mTableScanCost = NULL;
        ((qlncBaseTableNode*)aLeftNode)->mIndexScanCostCount = 0;
        ((qlncBaseTableNode*)aLeftNode)->mIndexScanCostArr = NULL;
        ((qlncBaseTableNode*)aLeftNode)->mIndexCombineCost = NULL;
        ((qlncBaseTableNode*)aLeftNode)->mRowidScanCost = NULL;

        /* Base Table Node Cost 계산 */
        sCBOptParamInfo.mParamInfo = *aParamInfo;
        sCBOptParamInfo.mLeftNode = aLeftNode;
        sCBOptParamInfo.mRightNode = NULL;
        sCBOptParamInfo.mJoinCond = NULL;
        sCBOptParamInfo.mFilter = NULL;
        sCBOptParamInfo.mCost = NULL;

        STL_TRY( qlncCBOptCopiedBaseTable( &sCBOptParamInfo,
                                           aEnv )
                 == STL_SUCCESS );
    }

    /* Left Node 및 Right Node, OutNode를 설정 */
    STL_TRY( qlncAddNodeToNodeArray( &(sSemiJoinTable->mNodeArray),
                                     aLeftNode,
                                     aEnv )
             == STL_SUCCESS );

    STL_TRY( qlncAddNodeToNodeArray( &(sSemiJoinTable->mNodeArray),
                                     sRightNode,
                                     aEnv )
             == STL_SUCCESS );

    /* Semi Join Table에 대한 Cost 계산 */
    sCBOptParamInfo.mParamInfo = *aParamInfo;
    sCBOptParamInfo.mLeftNode = (qlncNodeCommon*)sSemiJoinTable;
    sCBOptParamInfo.mRightNode = NULL;
    sCBOptParamInfo.mJoinCond = NULL;
    sCBOptParamInfo.mFilter = NULL;
    sCBOptParamInfo.mCost = NULL;

    if( sSemiJoinTable->mJoinType == QLNC_JOIN_TYPE_SEMI )
    {
        STL_TRY( qlncCBOptBestSemiJoinTable( &sCBOptParamInfo,
                                             aWeightValue,
                                             aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_DASSERT( (sSemiJoinTable->mJoinType == QLNC_JOIN_TYPE_ANTI_SEMI) ||
                     (sSemiJoinTable->mJoinType == QLNC_JOIN_TYPE_ANTI_SEMI_NA) );
        STL_TRY( qlncCBOptBestAntiSemiJoinTable( &sCBOptParamInfo,
                                                 aWeightValue,
                                                 aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Output 설정
     */

    *aOutNode = (qlncNodeCommon*)sSemiJoinTable;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Filter로부터 Semi/Anti-Semi Join으로 변경할 SubQuery Filter를 하나 추출한다.
 * @param[in]   aParamInfo              Parameter Info
 * @param[in]   aCurrNode               Current Candidate Node
 * @param[in]   aAndFilter              And Filter
 * @param[out]  aOutAndFilter           Output And Filter
 * @param[out]  aSubQueryFilterList     Output Semi SubQuery Boolean Filter List
 * @param[out]  aOutSubQueryAndFilter   Output Non-Semi SubQuery Boolean Filter List
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncPostOptExtractSubQueryFilter( qlncParamInfo       * aParamInfo,
                                            qlncNodeCommon      * aCurrNode,
                                            qlncAndFilter       * aAndFilter,
                                            qlncAndFilter      ** aOutAndFilter,
                                            qlncRefExprList     * aSubQueryFilterList,
                                            qlncAndFilter      ** aOutSubQueryAndFilter,
                                            qllEnv              * aEnv )
{
    stlInt32              i;
    stlInt32              j;
    qlncBooleanFilter   * sBooleanFilter        = NULL;
    qlncExprSubQuery    * sSubQuery             = NULL;
    qlncQuerySpec       * sQuerySpec            = NULL;
    qlncExprListFunc    * sListFunc             = NULL;
    qlncRefExprItem     * sRefExprItem          = NULL;
    qlncAndFilter       * sSubQueryAndFilter    = NULL;
    qlncRefNodeItem     * sRefNodeItem          = NULL;
    qlncNodeArray         sNodeArray;
    stlBool               sIsPossibleHashAntiSemi;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSubQueryFilterList != NULL, QLL_ERROR_STACK(aEnv) );


#define _QLNC_CHECK_OUTER_COLUMN_OF_GRAND_CHILD_NODE( _aChildNode, _aNodeArray )        \
    {                                                                                   \
        switch( (_aChildNode)->mNodeType )                                              \
        {                                                                               \
            case QLNC_NODE_TYPE_QUERY_SET:                                              \
            case QLNC_NODE_TYPE_SET_OP:                                                 \
            case QLNC_NODE_TYPE_QUERY_SPEC:                                             \
                STL_DASSERT( 0 );                                                       \
                STL_THROW( RAMP_SKIP );                                                 \
                break;                                                                  \
            case QLNC_NODE_TYPE_BASE_TABLE:                                             \
                break;                                                                  \
            case QLNC_NODE_TYPE_SUB_TABLE:                                              \
            case QLNC_NODE_TYPE_JOIN_TABLE:                                             \
            case QLNC_NODE_TYPE_INNER_JOIN_TABLE:                                       \
            case QLNC_NODE_TYPE_OUTER_JOIN_TABLE:                                       \
            case QLNC_NODE_TYPE_SEMI_JOIN_TABLE:                                        \
            case QLNC_NODE_TYPE_ANTI_SEMI_JOIN_TABLE:                                   \
                STL_TRY_THROW( qlncIsExistOuterColumnOnCandNode(                        \
                                   (_aChildNode),                                       \
                                   (_aNodeArray) )                                      \
                               == STL_FALSE,                                            \
                               RAMP_SKIP );                                             \
                break;                                                                  \
            case QLNC_NODE_TYPE_FLAT_INSTANT:                                           \
                STL_DASSERT( 0 );                                                       \
                STL_THROW( RAMP_SKIP );                                                 \
                break;                                                                  \
            case QLNC_NODE_TYPE_SORT_INSTANT:                                           \
                STL_TRY_THROW( qlncIsExistOuterColumnOnCandNode(                        \
                                   ((qlncSortInstantNode*)(_aChildNode))->mChildNode,   \
                                   (_aNodeArray) )                                      \
                               == STL_FALSE,                                            \
                               RAMP_SKIP );                                             \
                break;                                                                  \
            case QLNC_NODE_TYPE_HASH_INSTANT:                                           \
                STL_TRY_THROW( qlncIsExistOuterColumnOnCandNode(                        \
                                   ((qlncHashInstantNode*)(_aChildNode))->mChildNode,   \
                                   (_aNodeArray) )                                      \
                               == STL_FALSE,                                            \
                               RAMP_SKIP );                                             \
                break;                                                                  \
            default:                                                                    \
                STL_DASSERT( 0 );                                                       \
                STL_THROW( RAMP_SKIP );                                                 \
                break;                                                                  \
        }                                                                               \
    }


    QLNC_INIT_LIST( aSubQueryFilterList );
    sSubQueryAndFilter = NULL;

    STL_TRY_THROW( aAndFilter != NULL, RAMP_FINISH );

    /* Semi SubQuery Filter를 처리 */
    for( i = 0; i < aAndFilter->mOrCount; i++ )
    {
        /* SubQuery Filter가 존재해야 하고 Filter가 1개이어야 하며 Boolean Filter 이어야 한다. */
        STL_TRY_THROW( (aAndFilter->mOrFilters[i]->mSubQueryExprList != NULL) &&
                       (aAndFilter->mOrFilters[i]->mFilterCount == 1) &&
                       (aAndFilter->mOrFilters[i]->mFilters[0]->mType == QLNC_EXPR_TYPE_BOOLEAN_FILTER),
                       RAMP_SKIP );

        sBooleanFilter = (qlncBooleanFilter*)(aAndFilter->mOrFilters[i]->mFilters[0]);

        /* Outer Table에 대한 Reference Column이 존재해야 한다. */
        STL_TRY_THROW( sBooleanFilter->mRefNodeList.mCount > 0, RAMP_SKIP );

        /* Semi Join의 Left Node가 반드시 존재해야 한다. */
        sRefNodeItem = sBooleanFilter->mRefNodeList.mHead;
        while( sRefNodeItem != NULL )
        {
            if( qlncIsExistNode( aCurrNode, sRefNodeItem->mNode ) )
            {
                break;
            }

            sRefNodeItem = sRefNodeItem->mNext;
        }
        STL_TRY_THROW( sRefNodeItem != NULL, RAMP_SKIP );

        /* List Function이어야 한다. */
        STL_TRY_THROW( (sBooleanFilter->mExpr->mType == QLNC_EXPR_TYPE_LIST_FUNCTION) &&
                       (((qlncExprListFunc*)(sBooleanFilter->mExpr))->mArgCount <= 2),
                       RAMP_SKIP );

        sListFunc = (qlncExprListFunc*)(sBooleanFilter->mExpr);

        /* List Function의 좌측 Expression이 존재하는 경우 SubQuery가 아니어야 함 */
        if( sListFunc->mArgCount == 2 )
        {
            STL_TRY_THROW( ((qlncExprListCol*)(sListFunc->mArgs[1]))->mArgCount == 1,
                           RAMP_SKIP );

            for( j = 0; j < ((qlncExprRowExpr*)(((qlncExprListCol*)(sListFunc->mArgs[1]))->mArgs[0]))->mArgCount; j++ )
            {
                STL_TRY_THROW( ((qlncExprRowExpr*)(((qlncExprListCol*)(sListFunc->mArgs[1]))->mArgs[0]))->mArgs[j]->mType != QLNC_EXPR_TYPE_SUB_QUERY,
                               RAMP_SKIP );
            }
        }

        /* List Function의 우측 Expression이 Relation SubQuery이어야 함 */
        STL_TRY_THROW( (((qlncExprListCol*)(sListFunc->mArgs[0]))->mArgCount == 1) &&
                       (((qlncExprRowExpr*)(((qlncExprListCol*)(sListFunc->mArgs[0]))->mArgs[0]))->mArgCount == 1) &&
                       (((qlncExprRowExpr*)(((qlncExprListCol*)(sListFunc->mArgs[0]))->mArgs[0]))->mArgs[0]->mType == QLNC_EXPR_TYPE_SUB_QUERY),
                       RAMP_SKIP );

        /* SubQuery의 Node는 Query Spec이어야 한다. (Set 연산자가 아니어야 한다.) */
        sSubQuery = (qlncExprSubQuery*)(((qlncExprRowExpr*)(((qlncExprListCol*)(sListFunc->mArgs[0]))->mArgs[0]))->mArgs[0]);
        STL_TRY_THROW( sSubQuery->mNode->mNodeType == QLNC_NODE_TYPE_QUERY_SPEC,
                       RAMP_SKIP );

        /* SubQuery의 Query Spec에는 Offset과 Limit이 없어야 한다. */
        sQuerySpec = (qlncQuerySpec*)(sSubQuery->mNode);
        STL_TRY_THROW( (sQuerySpec->mOffset == NULL) && (sQuerySpec->mLimit == NULL),
                       RAMP_SKIP );

        /* SubQuery의 Query Spec에는 Target에 SubQuery가 존재하면 안된다. */
        STL_TRY_THROW( sQuerySpec->mNonFilterSubQueryExprList == NULL,
                       RAMP_SKIP );

        /* SubQuery의 Query Spec에 존재하는 Hint를 체크한다. */
        STL_TRY_THROW( sQuerySpec->mQueryTransformHint->mAllowedQueryTransform == STL_TRUE,
                       RAMP_SKIP );
        STL_TRY_THROW( sQuerySpec->mQueryTransformHint->mAllowedUnnest == STL_TRUE,
                       RAMP_SKIP );

        /* Query Spec의 Child Node가 Outer Join이 아니어야 한다. (결과가 달라질 수 있다.) */
        if( sQuerySpec->mChildNode->mNodeType == QLNC_NODE_TYPE_JOIN_TABLE )
        {
            STL_TRY_THROW( ((qlncJoinTableNode*)(sQuerySpec->mChildNode))->mJoinType
                           != QLNC_JOIN_TYPE_OUTER,
                           RAMP_SKIP );
        }


        /**
         * Query Spec의 Target에는 Outer Node를 참조하는 Column이 없어야 한다.
         */

        /* Node Array 초기화 */
        STL_TRY( qlncInitializeNodeArray( &QLL_CANDIDATE_MEM( aEnv ),
                                          &sNodeArray,
                                          aEnv )
                 == STL_SUCCESS );

        /* Node Array 생성 */
        STL_TRY( qlncMakeNodeArrayOnCurrNode( &sNodeArray,
                                              (qlncNodeCommon*)sQuerySpec,
                                              aEnv )
                 == STL_SUCCESS );

        for( j = 0; j < sQuerySpec->mTargetCount; j++ )
        {
            STL_TRY_THROW( qlncIsExistOuterColumnOnExpr(
                               sQuerySpec->mTargetArr[j].mExpr,
                               &sNodeArray )
                           == STL_FALSE,
                           RAMP_SKIP );
        }

        /* List Function에 대한 체크 */
        switch( sListFunc->mFuncId )
        {
            /* IN */
            case KNL_BUILTIN_FUNC_IN:
            /* EXISTS */
            case KNL_BUILTIN_FUNC_EXISTS:
            case KNL_BUILTIN_FUNC_NOT_EXISTS:
            /* ANY */
            case KNL_BUILTIN_FUNC_EQUAL_ANY:
            case KNL_BUILTIN_FUNC_NOT_EQUAL_ANY:
            case KNL_BUILTIN_FUNC_LESS_THAN_ANY:
            case KNL_BUILTIN_FUNC_LESS_THAN_EQUAL_ANY:
            case KNL_BUILTIN_FUNC_GREATER_THAN_ANY:
            case KNL_BUILTIN_FUNC_GREATER_THAN_EQUAL_ANY:
                STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                            STL_SIZEOF( qlncRefExprItem ),
                                            (void**) &sRefExprItem,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                sRefExprItem->mExpr = sBooleanFilter->mExpr;
                sRefExprItem->mNext = NULL;

                QLNC_APPEND_ITEM_TO_LIST( aSubQueryFilterList, sRefExprItem );

                aAndFilter->mOrFilters[i] = NULL;

                break;

            /* Anti-Semi로 풀리는 계열 */
            case KNL_BUILTIN_FUNC_NOT_IN:
            case KNL_BUILTIN_FUNC_NOT_EQUAL_ALL:
                /* NOT IN과 NOT EQUAL ALL은 동치이다. */
                STL_TRY( qlncPostOptSQUIsPossibleHashAntiSemi(
                             aParamInfo->mSQLStmt->mRetrySQL,
                             (qlncExprRowExpr*)(((qlncExprListCol*)(sListFunc->mArgs[1]))->mArgs[0]),
                             sQuerySpec,
                             sBooleanFilter->mExpr->mPosition,
                             &sIsPossibleHashAntiSemi,
                             aEnv )
                         == STL_SUCCESS );

                if( ( qlncPostOptSQUIsPossibleAntiSemi( sBooleanFilter->mExpr ) == STL_TRUE ) ||
                    ( sIsPossibleHashAntiSemi == STL_TRUE ) )
                {
                    /* Query Spec의 Child Node의 Child Node에 대하여 Outer Node를 참조하는 Column이
                     * 없어야 한다. */
                    _QLNC_CHECK_OUTER_COLUMN_OF_GRAND_CHILD_NODE(
                        sQuerySpec->mChildNode,
                        &sNodeArray );

                    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                STL_SIZEOF( qlncRefExprItem ),
                                                (void**) &sRefExprItem,
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    sRefExprItem->mExpr = sBooleanFilter->mExpr;
                    sRefExprItem->mNext = NULL;

                    QLNC_APPEND_ITEM_TO_LIST( aSubQueryFilterList, sRefExprItem );

                    aAndFilter->mOrFilters[i] = NULL;
                }

                break;

            /* ALL */
            case KNL_BUILTIN_FUNC_EQUAL_ALL:
            case KNL_BUILTIN_FUNC_LESS_THAN_ALL:
            case KNL_BUILTIN_FUNC_LESS_THAN_EQUAL_ALL:
            case KNL_BUILTIN_FUNC_GREATER_THAN_ALL:
            case KNL_BUILTIN_FUNC_GREATER_THAN_EQUAL_ALL:
                if( qlncPostOptSQUIsPossibleAntiSemi( sBooleanFilter->mExpr ) == STL_TRUE )
                {
                    /* Query Spec의 Child Node의 Child Node에 대하여 Outer Node를 참조하는 Column이
                     * 없어야 한다. */
                    _QLNC_CHECK_OUTER_COLUMN_OF_GRAND_CHILD_NODE(
                        sQuerySpec->mChildNode,
                        &sNodeArray );

                    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                STL_SIZEOF( qlncRefExprItem ),
                                                (void**) &sRefExprItem,
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    sRefExprItem->mExpr = sBooleanFilter->mExpr;
                    sRefExprItem->mNext = NULL;

                    QLNC_APPEND_ITEM_TO_LIST( aSubQueryFilterList, sRefExprItem );

                    aAndFilter->mOrFilters[i] = NULL;
                }
                break;

            default:
                break;
        }

        STL_RAMP( RAMP_SKIP );
    }

    /* Semi Join으로 풀 수 있는 SubQuery Filter가 존재하지 않으면 종료 */
    STL_TRY_THROW( aSubQueryFilterList->mCount > 0, RAMP_FINISH );

    /* Non Semi SubQuery Filter이고 Outer Table을 참조하는 Filter를 처리 */
    for( i = 0; i < aAndFilter->mOrCount; i++ )
    {
        if( aAndFilter->mOrFilters[i] == NULL )
        {
            continue;
        }

        if( aAndFilter->mOrFilters[i]->mSubQueryExprList != NULL )
        {
            sRefExprItem = aAndFilter->mOrFilters[i]->mSubQueryExprList->mHead;
            while( sRefExprItem != NULL )
            {
                STL_DASSERT( sRefExprItem->mExpr->mType == QLNC_EXPR_TYPE_SUB_QUERY );
                if( (((qlncExprSubQuery*)(sRefExprItem->mExpr))->mRefExprList != NULL) &&
                    (((qlncExprSubQuery*)(sRefExprItem->mExpr))->mRefExprList->mCount > 0) )
                {
                    if( sSubQueryAndFilter == NULL )
                    {
                        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                    STL_SIZEOF( qlncAndFilter ),
                                                    (void**) &sSubQueryAndFilter,
                                                    KNL_ENV(aEnv) )
                                 == STL_SUCCESS );
                        QLNC_INIT_AND_FILTER( sSubQueryAndFilter );
                    }

                    STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                                sSubQueryAndFilter,
                                                                1,
                                                                &(aAndFilter->mOrFilters[i]),
                                                                aEnv )
                             == STL_SUCCESS );

                    aAndFilter->mOrFilters[i] = NULL;

                    break;
                }

                sRefExprItem = sRefExprItem->mNext;
            }
        }
    }

    /* And Filter를 Compact함 */
    STL_TRY( qlncCompactAndFilter( aAndFilter,
                                   aEnv )
             == STL_SUCCESS );


    STL_RAMP( RAMP_FINISH );


    /* Out And Filter 설정 */
    if( (aAndFilter != NULL) &&
        (aAndFilter->mOrCount > 0) )
    {
        *aOutAndFilter = aAndFilter;
    }
    else
    {
        *aOutAndFilter = NULL;
    }

    /* Out SubQuery And Filter 설정 */
    *aOutSubQueryAndFilter = sSubQueryAndFilter;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SubQuery Unnest를 위해 Node의 Filter와 Filter의 주소를 추출한다.
 * @param[in]   aParamInfo      Parameter Info
 * @param[in]   aNode           Candidate Node
 * @param[in]   aRefNodeArray   Reference Node Array
 * @param[out]  aOuterAndFilter Outer And Filter
 * @param[in]   aEnv            Environment
 */
stlStatus qlncPostOptSQUExtractOuterAndFilter( qlncParamInfo    * aParamInfo,
                                               qlncNodeCommon   * aNode,
                                               qlncNodeArray    * aRefNodeArray,
                                               qlncAndFilter    * aOuterAndFilter,
                                               qllEnv           * aEnv )
{
    stlInt32              i;
    stlInt32              j;
    qlncCBOptParamInfo    sCBOptParamInfo;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefNodeArray != NULL, QLL_ERROR_STACK(aEnv) );


#define _QLNC_FIND_OUTER_TABLE_REFERENCE_FILTER( _aAndFilter )                          \
    {                                                                                   \
        if( (_aAndFilter) != NULL )                                                     \
        {                                                                               \
            for( i = 0; i < (_aAndFilter)->mOrCount; i++ )                              \
            {                                                                           \
                if( ( qlncIsExistOuterColumnByRefNodeOnExpr(                            \
                          (qlncExprCommon*)(_aAndFilter),                               \
                          aRefNodeArray )                                               \
                      == STL_TRUE ) &                                                   \
                    ( (_aAndFilter)->mOrFilters[i]->mSubQueryExprList == NULL ) )       \
                {                                                                       \
                    for( j = 0; j < (_aAndFilter)->mOrFilters[i]->mFilterCount; j++ )   \
                    {                                                                   \
                        if( (_aAndFilter)->mOrFilters[i]->mFilters[j]->mType            \
                            == QLNC_EXPR_TYPE_BOOLEAN_FILTER )                          \
                        {                                                               \
                            ((qlncBooleanFilter*)((_aAndFilter)->mOrFilters[i]->mFilters[j]))->mExpr->mIterationTime = DTL_ITERATION_TIME_FOR_EACH_EXPR;                                      \
                        }                                                               \
                    }                                                                   \
                                                                                        \
                    STL_TRY( qlncAppendOrFilterToAndFilterTail(                         \
                                 aParamInfo,                                            \
                                 aOuterAndFilter,                                       \
                                 1,                                                     \
                                 &((_aAndFilter)->mOrFilters[i]),                       \
                                 aEnv )                                                 \
                             == STL_SUCCESS );                                          \
                    (_aAndFilter)->mOrFilters[i] = NULL;                                \
                }                                                                       \
            }                                                                           \
                                                                                        \
            if( aOuterAndFilter->mOrCount > 0 )                                         \
            {                                                                           \
                STL_TRY( qlncCompactAndFilter( (_aAndFilter),                           \
                                               aEnv )                                   \
                         == STL_SUCCESS );                                              \
                                                                                        \
                if( (_aAndFilter)->mOrCount == 0 )                                      \
                {                                                                       \
                    (_aAndFilter) = NULL;                                               \
                }                                                                       \
            }                                                                           \
        }                                                                               \
    }

    /* Node Type에 따라 And Filter 및 And Filter 주소 추출 */
    if( aNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE )
    {
        /* Outer Table을 참조하는 Filter만 찾아냄 */
        QLNC_INIT_AND_FILTER( aOuterAndFilter );
        _QLNC_FIND_OUTER_TABLE_REFERENCE_FILTER( ((qlncBaseTableNode*)aNode)->mFilter );

        if( aOuterAndFilter->mOrCount > 0 )
        {
            /* Base Table Node의 Cost 계산 정보 초기화 */
            aNode->mBestOptType = QLNC_BEST_OPT_TYPE_NA;
            aNode->mBestCost = NULL;
            aNode->mIsAnalyzedCBO = STL_FALSE;

            ((qlncBaseTableNode*)aNode)->mTableScanCost = NULL;
            ((qlncBaseTableNode*)aNode)->mIndexScanCostCount = 0;
            ((qlncBaseTableNode*)aNode)->mIndexScanCostArr = NULL;
            ((qlncBaseTableNode*)aNode)->mIndexCombineCost = NULL;
            ((qlncBaseTableNode*)aNode)->mRowidScanCost = NULL;

            /* Base Table Node에 대한 Cost 재계산 */
            sCBOptParamInfo.mParamInfo = *aParamInfo;
            sCBOptParamInfo.mLeftNode = aNode;
            sCBOptParamInfo.mRightNode = NULL;
            sCBOptParamInfo.mJoinCond = NULL;
            sCBOptParamInfo.mFilter = NULL;
            sCBOptParamInfo.mCost = NULL;

            STL_TRY( qlncCBOptCopiedBaseTable( &sCBOptParamInfo,
                                               aEnv )
                     == STL_SUCCESS );
        }
    }
    else if( aNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE )
    {
        QLNC_INIT_AND_FILTER( aOuterAndFilter );
        _QLNC_FIND_OUTER_TABLE_REFERENCE_FILTER( ((qlncSubTableNode*)aNode)->mFilter );

        if( aOuterAndFilter->mOrCount > 0 )
        {
            /* @todo 현재 여기를 탈 수 없는 것으로 판단됨 */

            /* Sub Table Node의 Cost 계산 정보 초기화 */
            aNode->mBestOptType = QLNC_BEST_OPT_TYPE_NA;
            aNode->mBestCost = NULL;
            aNode->mIsAnalyzedCBO = STL_FALSE;

            ((qlncSubTableNode*)aNode)->mFilterCost = NULL;

            /* Sub Table Node에 대한 Cost 재계산 */
            sCBOptParamInfo.mParamInfo = *aParamInfo;
            sCBOptParamInfo.mLeftNode = aNode;
            sCBOptParamInfo.mRightNode = NULL;
            sCBOptParamInfo.mJoinCond = NULL;
            sCBOptParamInfo.mFilter = NULL;
            sCBOptParamInfo.mCost = NULL;

            STL_TRY( qlncCBOptSubTable( &sCBOptParamInfo,
                                        aEnv )
                     == STL_SUCCESS );
        }
    }
    else if( aNode->mNodeType == QLNC_NODE_TYPE_JOIN_TABLE )
    {
        /* Outer Join이면 안된다. */
        STL_DASSERT( ((qlncJoinTableNode*)aNode)->mJoinType != QLNC_JOIN_TYPE_OUTER );

        _QLNC_FIND_OUTER_TABLE_REFERENCE_FILTER( ((qlncJoinTableNode*)aNode)->mJoinCond );
        _QLNC_FIND_OUTER_TABLE_REFERENCE_FILTER( ((qlncJoinTableNode*)aNode)->mFilter );

        if( aOuterAndFilter->mOrCount > 0 )
        {
            /* @todo 현재 여기를 탈 수 없는 것으로 판단됨 */

            /* Join Table Node의 Cost 계산 정보 초기화 */
            aNode->mBestOptType = QLNC_BEST_OPT_TYPE_NA;
            aNode->mBestCost = NULL;
            aNode->mIsAnalyzedCBO = STL_FALSE;

            ((qlncJoinTableNode*)aNode)->mBestInnerJoinTable = NULL;
            ((qlncJoinTableNode*)aNode)->mBestOuterJoinTable = NULL;
            ((qlncJoinTableNode*)aNode)->mBestSemiJoinTable = NULL;
            ((qlncJoinTableNode*)aNode)->mBestAntiSemiJoinTable = NULL;

            /* Join Table Node에 대한 Cost 재계산 */
            sCBOptParamInfo.mParamInfo = *aParamInfo;
            sCBOptParamInfo.mLeftNode = aNode;
            sCBOptParamInfo.mRightNode = NULL;
            sCBOptParamInfo.mJoinCond = NULL;
            sCBOptParamInfo.mFilter = NULL;
            sCBOptParamInfo.mCost = NULL;

            STL_TRY( qlncCBOptJoinTable( &sCBOptParamInfo,
                                         aEnv )
                     == STL_SUCCESS );
        }
    }
    else if( aNode->mNodeType == QLNC_NODE_TYPE_SORT_INSTANT )
    {
        _QLNC_FIND_OUTER_TABLE_REFERENCE_FILTER( ((qlncSortInstantNode*)aNode)->mKeyFilter );
        _QLNC_FIND_OUTER_TABLE_REFERENCE_FILTER( ((qlncSortInstantNode*)aNode)->mRecFilter );

        if( aOuterAndFilter->mOrCount > 0 )
        {
            /* @todo 현재 여기를 탈 수 없는 것으로 판단됨 */

            ((qlncSortInstantNode*)aNode)->mFilter = NULL;

            /* Sort Instant Node의 Cost 계산 정보 초기화 */
            aNode->mBestOptType = QLNC_BEST_OPT_TYPE_NA;
            aNode->mBestCost = NULL;
            aNode->mIsAnalyzedCBO = STL_FALSE;

            ((qlncSortInstantNode*)aNode)->mSortInstantCost = NULL;

            /* Sort Instant Node에 대한 Cost 재계산 */
            sCBOptParamInfo.mParamInfo = *aParamInfo;
            sCBOptParamInfo.mLeftNode = aNode;
            sCBOptParamInfo.mRightNode = NULL;
            sCBOptParamInfo.mJoinCond = NULL;
            sCBOptParamInfo.mFilter = NULL;
            sCBOptParamInfo.mCost = NULL;

            STL_TRY( qlncCBOptSortInstant( &sCBOptParamInfo,
                                           aEnv )
                     == STL_SUCCESS );
        }
    }
    else
    {
        STL_DASSERT( aNode->mNodeType == QLNC_NODE_TYPE_HASH_INSTANT );

        _QLNC_FIND_OUTER_TABLE_REFERENCE_FILTER( ((qlncHashInstantNode*)aNode)->mKeyFilter );
        _QLNC_FIND_OUTER_TABLE_REFERENCE_FILTER( ((qlncHashInstantNode*)aNode)->mRecFilter );

        if( aOuterAndFilter->mOrCount > 0 )
        {
            ((qlncHashInstantNode*)aNode)->mFilter = NULL;

            /* Hash Instant Node의 Cost 계산 정보 초기화 */
            aNode->mBestOptType = QLNC_BEST_OPT_TYPE_NA;
            aNode->mBestCost = NULL;
            aNode->mIsAnalyzedCBO = STL_FALSE;

            ((qlncHashInstantNode*)aNode)->mHashInstantCost = NULL;

            /* Hash Instant Node에 대한 Cost 재계산 */
            sCBOptParamInfo.mParamInfo = *aParamInfo;
            sCBOptParamInfo.mLeftNode = aNode;
            sCBOptParamInfo.mRightNode = NULL;
            sCBOptParamInfo.mJoinCond = NULL;
            sCBOptParamInfo.mFilter = NULL;
            sCBOptParamInfo.mCost = NULL;

            STL_TRY( qlncCBOptHashInstant( &sCBOptParamInfo,
                                           aEnv )
                     == STL_SUCCESS );
        }
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Semi-Join/Anti-Semi-Join의 And Filter를 만든다.
 * @param[in]   aParamInfo          Parameter Info
 * @param[in]   aListFunc           List Function Expression
 * @param[in]   aLeftExpr           Left Expression
 * @param[in]   aRightTargetCount   Right Target Count
 * @param[in]   aRightRefColumnItem Right Reference Column Item
 * @param[in]   aRightTargetArr     Right Target Array
 * @param[in]   aSetFuncId          Function Id
 * @param[in]   aOuterAndFilter     Outer And Filter
 * @param[out]  aSemiJoinAndFilter  Semi-Join/Anti-Semi-Join And Filter
 * @param[in]   aEnv                Environment
 */
stlStatus qlncPostOptSQUMakeSemiJoinAndFilter( qlncParamInfo        * aParamInfo,
                                               qlncExprListFunc     * aListFunc,
                                               qlncExprCommon       * aLeftExpr,
                                               stlInt32               aRightTargetCount,
                                               qlncRefColumnItem    * aRightRefColumnItem,
                                               qlncTarget           * aRightTargetArr,
                                               knlBuiltInFunc         aSetFuncId,
                                               qlncAndFilter        * aOuterAndFilter,
                                               qlncAndFilter       ** aSemiJoinAndFilter,
                                               qllEnv               * aEnv )
{
    stlInt32              i;
    stlInt32              j;
    stlChar             * sTmpValue             = NULL;
    qlncExprFunction    * sFunction             = NULL;
    qlncExprRowExpr     * sRowExpr              = NULL;
    qlncBooleanFilter   * sBooleanFilter        = NULL;
    qlncAndFilter       * sSemiJoinAndFilter    = NULL;
    qlncRefNodeItem     * sRefNodeItem          = NULL;
    qlncRefColumnItem   * sRefColumnItem        = NULL;

    qlncConvertExprParamInfo      sConvertExprParamInfo;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRightTargetCount > 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aRightRefColumnItem != NULL) || (aRightTargetArr != NULL),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSetFuncId != KNL_BUILTIN_FUNC_INVALID,
                        QLL_ERROR_STACK(aEnv) );


#define _QLNC_MAKE_FUNCTION( _aArg1, _aArg2, _aFuncId, _aFunction )     \
    {                                                                   \
        /* Function 생성 */                                             \
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),         \
                                    STL_SIZEOF( qlncExprFunction ),     \
                                    (void**) &(_aFunction),             \
                                    KNL_ENV(aEnv) )                     \
                 == STL_SUCCESS );                                      \
                                                                        \
        /* Common Expression 정보 설정 */                               \
        sTmpValue = NULL;                                               \
        QLNC_SET_EXPR_COMMON( &(_aFunction)->mCommon,                   \
                              &QLL_CANDIDATE_MEM( aEnv ),               \
                              aEnv,                                     \
                              QLNC_EXPR_TYPE_FUNCTION,                  \
                              QLNC_EXPR_PHRASE_WHERE,                   \
                              DTL_ITERATION_TIME_FOR_EACH_EXPR,         \
                              0,                                        \
                              sTmpValue,                                \
                              DTL_TYPE_BOOLEAN );                       \
                                                                        \
        /* Function 정보 설정 */                                        \
        (_aFunction)->mFuncId = _aFuncId;                               \
        (_aFunction)->mArgCount = 2;                                    \
                                                                        \
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),         \
                                    STL_SIZEOF( qlncExprCommon* ) * 2,  \
                                    (void**) &((_aFunction)->mArgs),    \
                                    KNL_ENV(aEnv) )                     \
                 == STL_SUCCESS );                                      \
                                                                        \
        (_aFunction)->mArgs[0] = (qlncExprCommon*)(_aArg1);             \
        (_aFunction)->mArgs[1] = (qlncExprCommon*)(_aArg2);             \
    }

#define _QLNC_SET_REF_NODE_LIST_ON_BOOLEAN_FILTER( _aBooleanFilter, _aPossibleJoinCond )        \
    {                                                                                           \
        sConvertExprParamInfo.mParamInfo = *aParamInfo;                                         \
        sConvertExprParamInfo.mTableMapArr = NULL;                                              \
        sConvertExprParamInfo.mOuterTableMapArr = NULL;                                         \
        sConvertExprParamInfo.mInitExpr = NULL;                                                 \
        sConvertExprParamInfo.mCandExpr = NULL;                                                 \
        sConvertExprParamInfo.mExprPhraseFlag = 0;                                              \
        sConvertExprParamInfo.mSubQueryExprList = NULL;                                         \
        sConvertExprParamInfo.mExprSubQuery = NULL;                                             \
                                                                                                \
        if( (_aPossibleJoinCond) == STL_TRUE )                                                  \
        {                                                                                       \
            STL_DASSERT( ((_aBooleanFilter)->mExpr->mType == QLNC_EXPR_TYPE_FUNCTION) &&        \
                         (((qlncExprFunction*)((_aBooleanFilter)->mExpr))->mArgCount == 2) );   \
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),                             \
                                        STL_SIZEOF( qlncRefNodeList ),                          \
                                        (void**) &((_aBooleanFilter)->mLeftRefNodeList),        \
                                        KNL_ENV(aEnv) )                                         \
                     == STL_SUCCESS );                                                          \
            QLNC_INIT_LIST( (_aBooleanFilter)->mLeftRefNodeList );                              \
                                                                                                \
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),                             \
                                        STL_SIZEOF( qlncRefNodeList ),                          \
                                        (void**) &((_aBooleanFilter)->mRightRefNodeList),       \
                                        KNL_ENV(aEnv) )                                         \
                     == STL_SUCCESS );                                                          \
            QLNC_INIT_LIST( (_aBooleanFilter)->mRightRefNodeList );                             \
                                                                                                \
            sConvertExprParamInfo.mRefNodeList = (_aBooleanFilter)->mLeftRefNodeList;           \
            STL_TRY( qlncFindAddRefColumnToRefNodeList(                                         \
                         &sConvertExprParamInfo,                                                \
                         ((qlncExprFunction*)((_aBooleanFilter)->mExpr))->mArgs[0],             \
                         aEnv )                                                                 \
                     == STL_SUCCESS );                                                          \
                                                                                                \
            sConvertExprParamInfo.mRefNodeList = (_aBooleanFilter)->mRightRefNodeList;          \
            STL_TRY( qlncFindAddRefColumnToRefNodeList(                                         \
                         &sConvertExprParamInfo,                                                \
                         ((qlncExprFunction*)((_aBooleanFilter)->mExpr))->mArgs[1],             \
                         aEnv )                                                                 \
                     == STL_SUCCESS );                                                          \
                                                                                                \
            sConvertExprParamInfo.mRefNodeList = &((_aBooleanFilter)->mRefNodeList);            \
                                                                                                \
            /* Append Left Reference Node List */                                               \
            sRefNodeItem = (_aBooleanFilter)->mLeftRefNodeList->mHead;                          \
            while( sRefNodeItem != NULL )                                                       \
            {                                                                                   \
                sRefColumnItem = sRefNodeItem->mRefColumnList.mHead;                            \
                while( sRefColumnItem != NULL )                                                 \
                {                                                                               \
                    STL_TRY( qlncAddRefColumnToRefNodeList( &sConvertExprParamInfo,             \
                                                            sRefColumnItem->mExpr,              \
                                                            aEnv )                              \
                             == STL_SUCCESS );                                                  \
                                                                                                \
                    sRefColumnItem = sRefColumnItem->mNext;                                     \
                }                                                                               \
                                                                                                \
                sRefNodeItem = sRefNodeItem->mNext;                                             \
            }                                                                                   \
                                                                                                \
            /* Append Right Reference Node List */                                              \
            sRefNodeItem = (_aBooleanFilter)->mRightRefNodeList->mHead;                         \
            while( sRefNodeItem != NULL )                                                       \
            {                                                                                   \
                sRefColumnItem = sRefNodeItem->mRefColumnList.mHead;                            \
                while( sRefColumnItem != NULL )                                                 \
                {                                                                               \
                    STL_TRY( qlncAddRefColumnToRefNodeList( &sConvertExprParamInfo,             \
                                                            sRefColumnItem->mExpr,              \
                                                            aEnv )                              \
                             == STL_SUCCESS );                                                  \
                                                                                                \
                    sRefColumnItem = sRefColumnItem->mNext;                                     \
                }                                                                               \
                                                                                                \
                sRefNodeItem = sRefNodeItem->mNext;                                             \
            }                                                                                   \
                                                                                                \
            if( ((_aBooleanFilter)->mLeftRefNodeList->mCount == 0) ||                           \
                ((_aBooleanFilter)->mRightRefNodeList->mCount == 0) )                           \
            {                                                                                   \
                (_aBooleanFilter)->mPossibleJoinCond = STL_FALSE;                               \
                (_aBooleanFilter)->mLeftRefNodeList = NULL;                                     \
                (_aBooleanFilter)->mRightRefNodeList = NULL;                                    \
            }                                                                                   \
            else                                                                                \
            {                                                                                   \
                (_aBooleanFilter)->mPossibleJoinCond = STL_TRUE;                                \
            }                                                                                   \
        }                                                                                       \
        else                                                                                    \
        {                                                                                       \
            (_aBooleanFilter)->mLeftRefNodeList = NULL;                                         \
            (_aBooleanFilter)->mRightRefNodeList = NULL;                                        \
                                                                                                \
            /* Reference Node List 설정 */                                                      \
            sConvertExprParamInfo.mRefNodeList = &((_aBooleanFilter)->mRefNodeList);            \
            STL_TRY( qlncFindAddRefColumnToRefNodeList( &sConvertExprParamInfo,                 \
                                                        (_aBooleanFilter)->mExpr,               \
                                                        aEnv )                                  \
                     == STL_SUCCESS );                                                          \
        }                                                                                       \
    }

#define _QLNC_MAKE_BOOLEAN_FILTER( _aFuncId,                                                    \
                                   _aExpr,                                                      \
                                   _aPossibleJoinCond,                                          \
                                   _aLeftExpr,                                                  \
                                   _aRightExpr,                                                 \
                                   _aBooleanFilter )                                            \
    {                                                                                           \
        /* Boolean Filter 생성 */                                                               \
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),                                 \
                                    STL_SIZEOF( qlncBooleanFilter ),                            \
                                    (void**) &(_aBooleanFilter),                                \
                                    KNL_ENV(aEnv) )                                             \
                 == STL_SUCCESS );                                                              \
                                                                                                \
        /* Common Expression 정보 설정 */                                                       \
        sTmpValue = NULL;                                                                       \
        QLNC_SET_EXPR_COMMON( &(_aBooleanFilter)->mCommon,                                      \
                              &QLL_CANDIDATE_MEM( aEnv ),                                       \
                              aEnv,                                                             \
                              QLNC_EXPR_TYPE_BOOLEAN_FILTER,                                    \
                              QLNC_EXPR_PHRASE_WHERE,                                           \
                              DTL_ITERATION_TIME_FOR_EACH_EXPR,                                 \
                              0,                                                                \
                              sTmpValue,                                                        \
                              DTL_TYPE_BOOLEAN );                                               \
                                                                                                \
        (_aBooleanFilter)->mFuncID = (_aFuncId);                                                \
        (_aBooleanFilter)->mExpr = (qlncExprCommon*)(_aExpr);                                   \
        QLNC_INIT_LIST( &((_aBooleanFilter)->mRefNodeList) );                                   \
                                                                                                \
        (_aBooleanFilter)->mPossibleJoinCond = (_aPossibleJoinCond);                            \
        (_aBooleanFilter)->mIsPhysicalFilter = STL_FALSE;                                       \
        (_aBooleanFilter)->mSubQueryExprList = NULL;                                            \
                                                                                                \
        _QLNC_SET_REF_NODE_LIST_ON_BOOLEAN_FILTER( (_aBooleanFilter), (_aPossibleJoinCond) );   \
    }


    /* Semi Jin And Filter 생성 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncAndFilter ),
                                (void**) &sSemiJoinAndFilter,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_INIT_AND_FILTER( sSemiJoinAndFilter );

    /* Left Expression이 없는 경우(Exists 연산인 경우) Skip */
    STL_TRY_THROW( aLeftExpr != NULL, RAMP_FINISH );

    /* Left Expression과 Right Target으로 Filter 만들기 */
    if( aRightTargetCount == 1 )
    {
        STL_DASSERT( aLeftExpr->mType != QLNC_EXPR_TYPE_ROW_EXPR );

        /* Function 만들기  */
        _QLNC_MAKE_FUNCTION( aLeftExpr,
                             ( aRightRefColumnItem != NULL
                               ? aRightRefColumnItem->mExpr
                               : aRightTargetArr[0].mExpr ),
                             aSetFuncId,
                             sFunction );

        /* BooleanFilter 만들기 */
        _QLNC_MAKE_BOOLEAN_FILTER( aSetFuncId,
                                   sFunction,
                                   STL_TRUE,
                                   sFunction->mArgs[0],
                                   sFunction->mArgs[1],
                                   sBooleanFilter );

        /* Semi Join And Filter에 추가 */
        STL_TRY( qlncAppendFilterToAndFilter( aParamInfo,
                                              sSemiJoinAndFilter,
                                              (qlncExprCommon*)sBooleanFilter,
                                              aEnv )
                 == STL_SUCCESS );

        STL_THROW( RAMP_FINISH );
    }

    /* Row Expression인 경우 */
    if( (aSetFuncId == KNL_BUILTIN_FUNC_IS_EQUAL) ||
        (aSetFuncId == KNL_BUILTIN_FUNC_IS_NOT_EQUAL) )
    {
        /* And List로 풀 수 있는 경우 */
        STL_DASSERT( ((qlncExprRowExpr*)aLeftExpr)->mArgCount == aRightTargetCount );
        for( i = 0; i < aRightTargetCount; i++ )
        {
            /* Function 만들기  */
            _QLNC_MAKE_FUNCTION( ((qlncExprRowExpr*)aLeftExpr)->mArgs[i],
                                 ( aRightRefColumnItem != NULL
                                   ? aRightRefColumnItem->mExpr
                                   : aRightTargetArr[i].mExpr ),
                                 aSetFuncId,
                                 sFunction );

            /* BooleanFilter 만들기 */
            _QLNC_MAKE_BOOLEAN_FILTER( aSetFuncId,
                                       sFunction,
                                       STL_TRUE,
                                       sFunction->mArgs[0],
                                       sFunction->mArgs[1],
                                       sBooleanFilter );

            /* Semi Join And Filter에 추가 */
            STL_TRY( qlncAppendFilterToAndFilter( aParamInfo,
                                                  sSemiJoinAndFilter,
                                                  (qlncExprCommon*)sBooleanFilter,
                                                  aEnv )
                     == STL_SUCCESS );

            if( aRightRefColumnItem != NULL )
            {
                aRightRefColumnItem = aRightRefColumnItem->mNext;
            }
        }
    }
    else
    {
        /* And List로 풀 수 없는 경우 */

        /* Right Row Expression 찾기 */
        sRowExpr = (qlncExprRowExpr*)(((qlncExprListCol*)(aListFunc->mArgs[0]))->mArgs[0]);

        /* Right Row Expression 재설정 */
        sRowExpr->mArgCount = aRightTargetCount;

        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncExprCommon* ) * aRightTargetCount,
                                    (void**) &(sRowExpr->mArgs),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        if( aRightRefColumnItem != NULL )
        {
            for( i = 0; i < aRightTargetCount; i++ )
            {
                sRowExpr->mArgs[i] = aRightRefColumnItem->mExpr; 

                aRightRefColumnItem = aRightRefColumnItem->mNext;
            }
        }
        else
        {
            for( i = 0; i < aRightTargetCount; i++ )
            {
                sRowExpr->mArgs[i] = aRightTargetArr[i].mExpr; 
            }
        }

        /* List Function의 List Function Type 재설정 */
        if( aSetFuncId == KNL_BUILTIN_FUNC_IS_LESS_THAN )
        {
            aListFunc->mFuncId = KNL_BUILTIN_FUNC_IS_LESS_THAN_ROW;
        }
        else if( aSetFuncId == KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL )
        {
            aListFunc->mFuncId = KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL_ROW;
        }
        else if( aSetFuncId == KNL_BUILTIN_FUNC_IS_GREATER_THAN )
        {
            aListFunc->mFuncId = KNL_BUILTIN_FUNC_IS_GREATER_THAN_ROW;
        }
        else
        {
            STL_DASSERT( aSetFuncId == KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL );
            aListFunc->mFuncId = KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL_ROW;
        }

        /* BooleanFilter 만들기 */
        _QLNC_MAKE_BOOLEAN_FILTER( aListFunc->mFuncId,
                                   aListFunc,
                                   STL_FALSE,
                                   NULL,
                                   NULL,
                                   sBooleanFilter );

        /* Semi Join And Filter에 추가 */
        STL_TRY( qlncAppendFilterToAndFilter( aParamInfo,
                                              sSemiJoinAndFilter,
                                              (qlncExprCommon*)sBooleanFilter,
                                              aEnv )
                 == STL_SUCCESS );

    }


    STL_RAMP( RAMP_FINISH );


    /* Outer And Filter들을 Semi-Join/Anti-Semi-Join Filter에 추가 */
    if( aOuterAndFilter->mOrCount > 0 )
    {
        /* Boolean Filter에 Reference Node List들을 재설정 */
        for( i = 0; i < aOuterAndFilter->mOrCount; i++ )
        {
            for( j = 0; j < aOuterAndFilter->mOrFilters[i]->mFilterCount; j++ )
            {
                if( aOuterAndFilter->mOrFilters[i]->mFilters[j]->mType == QLNC_EXPR_TYPE_BOOLEAN_FILTER )
                {
                    sBooleanFilter = (qlncBooleanFilter*)(aOuterAndFilter->mOrFilters[i]->mFilters[j]);
                    QLNC_INIT_LIST( &(sBooleanFilter->mRefNodeList) );
                    sBooleanFilter->mLeftRefNodeList = NULL;
                    sBooleanFilter->mRightRefNodeList = NULL;

                    _QLNC_SET_REF_NODE_LIST_ON_BOOLEAN_FILTER(
                        sBooleanFilter,
                        ( (sBooleanFilter->mExpr->mType == QLNC_EXPR_TYPE_FUNCTION) &&
                          (qlncIsPossibleJoinCondition(
                               ((qlncExprFunction*)(sBooleanFilter->mExpr))->mFuncId )) ) );
                }
            }
        }

        STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                    sSemiJoinAndFilter,
                                                    aOuterAndFilter->mOrCount,
                                                    aOuterAndFilter->mOrFilters,
                                                    aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Output 설정
     */

    *aSemiJoinAndFilter = sSemiJoinAndFilter;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Anti-Semi Join으로 풀 수 있는지 판단한다.
 * @param[in]   aExpr   Candidate Expression
 */
stlBool qlncPostOptSQUIsPossibleAntiSemi( qlncExprCommon    * aExpr )
{
    stlInt32              i;
    qlncExprRowExpr     * sRowExpr  = NULL;
    qlncExprSubQuery    * sSubQuery = NULL;


    STL_DASSERT( aExpr->mType == QLNC_EXPR_TYPE_LIST_FUNCTION );
    STL_DASSERT( ((qlncExprListCol*)(((qlncExprListFunc*)aExpr)->mArgs[0]))->mArgCount == 1 );
    STL_DASSERT( ((qlncExprListCol*)(((qlncExprListFunc*)aExpr)->mArgs[1]))->mArgCount == 1 );
    STL_DASSERT( ((qlncExprRowExpr*)(((qlncExprListCol*)(((qlncExprListFunc*)aExpr)->mArgs[0]))->mArgs[0]))->mArgCount == 1 );
    STL_DASSERT( ((qlncExprRowExpr*)(((qlncExprListCol*)(((qlncExprListFunc*)aExpr)->mArgs[0]))->mArgs[0]))->mArgs[0]->mType == QLNC_EXPR_TYPE_SUB_QUERY );


    /* Left Node의 Row Expression 설정 */
    sRowExpr =
        (qlncExprRowExpr*)(((qlncExprListCol*)(((qlncExprListFunc*)aExpr)->mArgs[1]))->mArgs[0]);

    /* Row Expression에 존재하는 모든 Expression들은 NOT NULL이어야 한다. */
    for( i = 0; i < sRowExpr->mArgCount; i++ )
    {
        /* Nullable이면 안된다. */
        STL_TRY_THROW( sRowExpr->mArgs[i]->mNullable == STL_FALSE,
                       RAMP_FALSE );
    }

    /* Right Node의 SubQuery Expression 설정 */
    sSubQuery =
        (qlncExprSubQuery*)(((qlncExprRowExpr*)(((qlncExprListCol*)(((qlncExprListFunc*)aExpr)->mArgs[0]))->mArgs[0]))->mArgs[0]);

    /* SubQuery의 모든 Target들은 NOT NULL이어야 한다. */
    STL_DASSERT( sSubQuery->mNode->mNodeType == QLNC_NODE_TYPE_QUERY_SPEC );
    for( i = 0; i < ((qlncQuerySpec*)(sSubQuery->mNode))->mTargetCount; i++ )
    {
        /* Nullable이면 안된다. */
        STL_TRY_THROW( ((qlncQuerySpec*)(sSubQuery->mNode))->mTargetArr[i].mDataTypeInfo.mNullable == STL_FALSE,
                       RAMP_FALSE );
    }


    return STL_TRUE;

    STL_RAMP( RAMP_FALSE );

    return STL_FALSE;
}


/**
 * @brief Hash Anti-Semi Join이 가능한지 판단한다.
 * @param[in]   aSQLString      SQL String
 * @param[in]   aLeftRowExpr    Outer쪽의 Candidate Row Expression
 * @param[in]   aRightQuerySpec Inner쪽의 Query Spec Candidate Node
 * @param[in]   aExprPos        Expression의 Position
 * @param[out]  aIsPossible     Hash Anti-Semi Join 가능여부
 * @param[in]   aEnv            Environment
 */
stlStatus qlncPostOptSQUIsPossibleHashAntiSemi( stlChar         * aSQLString,
                                                qlncExprRowExpr * aLeftRowExpr,
                                                qlncQuerySpec   * aRightQuerySpec,
                                                stlInt32          aExprPos,
                                                stlBool         * aIsPossible,
                                                qllEnv          * aEnv )
{
    stlInt32                  i;
    stlBool                   sIsPossible;
    const dtlCompareType    * sCompType;


    STL_DASSERT( aLeftRowExpr != NULL );
    STL_DASSERT( aRightQuerySpec != NULL );
    STL_DASSERT( aLeftRowExpr->mArgCount == aRightQuerySpec->mTargetCount );


    sIsPossible = STL_FALSE;
    for( i = 0; i < aLeftRowExpr->mArgCount; i++ )
    {
        /* Right Expression이 Key Compare 가능 Type이어야 한다. */
        STL_TRY_THROW( dtlCheckKeyCompareType( aRightQuerySpec->mTargetArr[i].mExpr->mDataType )
                       == STL_TRUE, RAMP_FINISH );

        /* Left Expression과 Right Expression의 Compare가 가능해야 한다. */
        STL_TRY( qlnoGetCompareType( aSQLString,
                                     aRightQuerySpec->mTargetArr[i].mExpr->mDataType,
                                     aLeftRowExpr->mArgs[i]->mDataType,
                                     STL_FALSE,
                                     STL_TRUE,
                                     aExprPos,
                                     &sCompType,
                                     aEnv )
                 == STL_SUCCESS );

        STL_TRY_THROW( ( sCompType->mLeftType == aRightQuerySpec->mTargetArr[i].mExpr->mDataType ) &&
                       ( sCompType->mRightType != DTL_TYPE_NA ),
                       RAMP_FINISH );

        /* Cast는 가능하지만 Cast시 Data Loss가 발생하면 안된다. */
        STL_TRY_THROW( dtlIsNoDataLossForCast
                       [ aLeftRowExpr->mArgs[i]->mDataType ]
                       [ aRightQuerySpec->mTargetArr[i].mExpr->mDataType ]
                       == STL_TRUE,
                       RAMP_FINISH );
    }


    sIsPossible = STL_TRUE;


    STL_RAMP( RAMP_FINISH );


    *aIsPossible = sIsPossible;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Best Opt Node에 대하여 추가 Filter Push Down을 수행한다.
 * @param[in]   aParamInfo  Parameter Info
 * @param[in]   aNode       Candidate Node
 * @param[in]   aJoinType   Join Type
 * @param[in]   aEnv        Environment
 */
stlStatus qlncPostOptFilterPushDown( qlncParamInfo  * aParamInfo,
                                     qlncNodeCommon * aNode,
                                     qlncJoinType     aJoinType,
                                     qllEnv         * aEnv )
{
    stlInt32          i;
    qlncFilterCost  * sFilterCost   = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode != NULL, QLL_ERROR_STACK(aEnv) );


    switch( aNode->mBestOptType )
    {
        case QLNC_BEST_OPT_TYPE_QUERY_SET:
            STL_TRY( qlncPostOptFilterPushDown( aParamInfo,
                                                ((qlncQuerySet*)aNode)->mChildNode,
                                                QLNC_JOIN_TYPE_INVALID,
                                                aEnv )
                     == STL_SUCCESS );
            break;

        case QLNC_BEST_OPT_TYPE_SET_OP:
            for( i = 0; i < ((qlncSetOP*)aNode)->mChildCount; i++ )
            {
                STL_TRY( qlncPostOptFilterPushDown( aParamInfo,
                                                    ((qlncSetOP*)aNode)->mChildNodeArr[i],
                                                    QLNC_JOIN_TYPE_INVALID,
                                                    aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_BEST_OPT_TYPE_QUERY_SPEC:
            STL_TRY_THROW( ((qlncQuerySpec*)aNode)->mQueryTransformHint->mAllowedQueryTransform == STL_TRUE,
                           RAMP_FINISH );

            STL_TRY( qlncPostOptFilterPushDown( aParamInfo,
                                                ((qlncQuerySpecCost*)(aNode->mBestCost))->mChildNode,
                                                QLNC_JOIN_TYPE_INVALID,
                                                aEnv )
                     == STL_SUCCESS );

            if( ((qlncQuerySpec*)aNode)->mNonFilterSubQueryExprList != NULL )
            {
                STL_TRY( qlncPostOptFPDSubQueryExprList(
                             aParamInfo,
                             ((qlncQuerySpec*)aNode)->mNonFilterSubQueryExprList,
                             aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_BEST_OPT_TYPE_TABLE_SCAN:
            if( ((qlncTableScanCost*)(aNode->mBestCost))->mTableAndFilter != NULL )
            {
                STL_TRY( qlncPostOptFPDSubQueryOnAndFilter(
                             aParamInfo,
                             ((qlncTableScanCost*)(aNode->mBestCost))->mTableAndFilter,
                             aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_BEST_OPT_TYPE_INDEX_SCAN:
            if( ((qlncIndexScanCost*)(aNode->mBestCost))->mTableAndFilter != NULL )
            {
                STL_TRY( qlncPostOptFPDSubQueryOnAndFilter(
                             aParamInfo,
                             ((qlncIndexScanCost*)(aNode->mBestCost))->mTableAndFilter,
                             aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_BEST_OPT_TYPE_INDEX_COMBINE:
            for( i = 0; i < ((qlncIndexCombineCost*)(aNode->mBestCost))->mDnfAndFilterCount; i++ )
            {
                if( ((qlncIndexCombineCost*)(aNode->mBestCost))->mIndexScanCostArr[i].mTableAndFilter != NULL )
                {
                    STL_TRY( qlncPostOptFPDSubQueryOnAndFilter(
                                 aParamInfo,
                                 ((qlncIndexCombineCost*)(aNode->mBestCost))->mIndexScanCostArr[i].mTableAndFilter,
                                 aEnv )
                             == STL_SUCCESS );
                }
            }

            break;

        case QLNC_BEST_OPT_TYPE_ROWID_SCAN:
            if( ((qlncRowidScanCost*)(aNode->mBestCost))->mTableAndFilter != NULL )
            {
                STL_TRY( qlncPostOptFPDSubQueryOnAndFilter(
                             aParamInfo,
                             ((qlncRowidScanCost*)(aNode->mBestCost))->mTableAndFilter,
                             aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_BEST_OPT_TYPE_FILTER:
            sFilterCost = (qlncFilterCost*)(aNode->mBestCost);
            if( sFilterCost->mMaterializedNode == NULL )
            {
                if( sFilterCost->mFilter != NULL )
                {
                    STL_TRY( qlncPostOptFPDSubQueryOnAndFilter(
                                 aParamInfo,
                                 sFilterCost->mFilter,
                                 aEnv )
                             == STL_SUCCESS );

                    /* SubTable Filter Push Down */
                    STL_TRY( qlncPostOptFPDSubTableNode( aParamInfo,
                                                         (qlncSubTableNode*)aNode,
                                                         aEnv )
                             == STL_SUCCESS );
                }

                STL_TRY( qlncPostOptFilterPushDown( aParamInfo,
                                                    ((qlncSubTableNode*)aNode)->mQueryNode,
                                                    QLNC_JOIN_TYPE_INVALID,
                                                    aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                STL_TRY( qlncPostOptFilterPushDown( aParamInfo,
                                                    sFilterCost->mMaterializedNode,
                                                    QLNC_JOIN_TYPE_INVALID,
                                                    aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_BEST_OPT_TYPE_INNER_JOIN:
            STL_TRY( qlncPostOptFilterPushDown(
                         aParamInfo,
                         (qlncNodeCommon*)(((qlncJoinTableNode*)aNode)->mBestInnerJoinTable),
                         QLNC_JOIN_TYPE_INNER,
                         aEnv )
                     == STL_SUCCESS );

            break;

        case QLNC_BEST_OPT_TYPE_OUTER_JOIN:
            STL_TRY( qlncPostOptFilterPushDown(
                         aParamInfo,
                         (qlncNodeCommon*)(((qlncJoinTableNode*)aNode)->mBestOuterJoinTable),
                         QLNC_JOIN_TYPE_OUTER,
                         aEnv )
                     == STL_SUCCESS );

            break;

        case QLNC_BEST_OPT_TYPE_SEMI_JOIN:
            STL_TRY( qlncPostOptFilterPushDown(
                         aParamInfo,
                         (qlncNodeCommon*)(((qlncJoinTableNode*)aNode)->mBestSemiJoinTable),
                         QLNC_JOIN_TYPE_SEMI,
                         aEnv )
                     == STL_SUCCESS );

            break;

        case QLNC_BEST_OPT_TYPE_ANTI_SEMI_JOIN:
            STL_TRY( qlncPostOptFilterPushDown(
                         aParamInfo,
                         (qlncNodeCommon*)(((qlncJoinTableNode*)aNode)->mBestAntiSemiJoinTable),
                         ((qlncJoinTableNode*)aNode)->mJoinType,
                         aEnv )
                     == STL_SUCCESS );

            break;

        case QLNC_BEST_OPT_TYPE_NESTED_LOOPS:
            /* Nested Loops Join Filter Push Down */
            STL_TRY( qlncPostOptFPDNestedJoin( aParamInfo,
                                               aNode,
                                               aJoinType,
                                               aEnv )
                     == STL_SUCCESS );

            STL_TRY( qlncPostOptFilterPushDown(
                         aParamInfo,
                         ((qlncNestedJoinCost*)(aNode->mBestCost))->mLeftNode,
                         aJoinType,
                         aEnv )
                     == STL_SUCCESS );

            STL_TRY( qlncPostOptFilterPushDown(
                         aParamInfo,
                         ((qlncNestedJoinCost*)(aNode->mBestCost))->mRightNode,
                         aJoinType,
                         aEnv )
                     == STL_SUCCESS );

            if( ((qlncNestedJoinCost*)(aNode->mBestCost))->mJoinFilter != NULL )
            {
                STL_TRY( qlncPostOptFPDSubQueryOnAndFilter(
                             aParamInfo,
                             ((qlncNestedJoinCost*)(aNode->mBestCost))->mJoinFilter,
                             aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_BEST_OPT_TYPE_MERGE:
            /* Merge Join Filter Push Down */
            STL_TRY( qlncPostOptFPDMergeJoin( aParamInfo,
                                              aNode,
                                              aJoinType,
                                              aEnv )
                     == STL_SUCCESS );

            STL_TRY( qlncPostOptFilterPushDown(
                         aParamInfo,
                         ((qlncMergeJoinCost*)(aNode->mBestCost))->mLeftNode,
                         aJoinType,
                         aEnv )
                     == STL_SUCCESS );

            STL_TRY( qlncPostOptFilterPushDown(
                         aParamInfo,
                         ((qlncMergeJoinCost*)(aNode->mBestCost))->mRightNode,
                         aJoinType,
                         aEnv )
                     == STL_SUCCESS );

            if( ((qlncMergeJoinCost*)(aNode->mBestCost))->mJoinFilter != NULL )
            {
                STL_TRY( qlncPostOptFPDSubQueryOnAndFilter(
                             aParamInfo,
                             ((qlncMergeJoinCost*)(aNode->mBestCost))->mJoinFilter,
                             aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_BEST_OPT_TYPE_HASH:
            /* Hash Join Filter Push Down */
            STL_TRY( qlncPostOptFPDHashJoin( aParamInfo,
                                             aNode,
                                             aJoinType,
                                             aEnv )
                     == STL_SUCCESS );

            STL_TRY( qlncPostOptFilterPushDown(
                         aParamInfo,
                         ((qlncHashJoinCost*)(aNode->mBestCost))->mLeftNode,
                         aJoinType,
                         aEnv )
                     == STL_SUCCESS );

            STL_TRY( qlncPostOptFilterPushDown(
                         aParamInfo,
                         ((qlncHashJoinCost*)(aNode->mBestCost))->mRightNode,
                         aJoinType,
                         aEnv )
                     == STL_SUCCESS );

            if( ((qlncHashJoinCost*)(aNode->mBestCost))->mJoinFilter != NULL )
            {
                STL_TRY( qlncPostOptFPDSubQueryOnAndFilter(
                             aParamInfo,
                             ((qlncHashJoinCost*)(aNode->mBestCost))->mJoinFilter,
                             aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_BEST_OPT_TYPE_JOIN_COMBINE:
            for( i = 0; i < ((qlncJoinCombineCost*)(aNode->mBestCost))->mDnfAndFilterCount; i++ )
            {
                STL_TRY( qlncPostOptFilterPushDown(
                             aParamInfo,
                             ((qlncJoinCombineCost*)(aNode->mBestCost))->mJoinNodePtrArr[i],
                             aJoinType,
                             aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_BEST_OPT_TYPE_SORT_INSTANT:
            /**
             * Sort Instant는 Filter Push Down을 해서는 안된다.
             * => 이미 Instant로 생성되어 처리되므로 Instant에서 Filter로 처리해야 된다.
             */
            STL_TRY( qlncPostOptFilterPushDown( aParamInfo,
                                                ((qlncSortInstantNode*)aNode)->mChildNode,
                                                QLNC_JOIN_TYPE_INVALID,
                                                aEnv )
                     == STL_SUCCESS );

            if( ((qlncSortInstantNode*)aNode)->mNonFilterSubQueryExprList != NULL )
            {
                STL_TRY( qlncPostOptFPDSubQueryExprList(
                             aParamInfo,
                             ((qlncSortInstantNode*)aNode)->mNonFilterSubQueryExprList,
                             aEnv )
                         == STL_SUCCESS );
            }

            if( ((qlncSortInstantNode*)aNode)->mFilter != NULL )
            {
                STL_TRY( qlncPostOptFPDSubQueryOnAndFilter(
                             aParamInfo,
                             ((qlncSortInstantNode*)aNode)->mFilter,
                             aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_BEST_OPT_TYPE_HASH_INSTANT:
        case QLNC_BEST_OPT_TYPE_GROUP:
            /**
             * Hash Instant는 Filter Push Down을 해서는 안된다.
             * => 이미 Instant로 생성되어 처리되므로 Instant에서 Filter로 처리해야 된다.
             */
            STL_TRY( qlncPostOptFilterPushDown( aParamInfo,
                                                ((qlncHashInstantNode*)aNode)->mChildNode,
                                                QLNC_JOIN_TYPE_INVALID,
                                                aEnv )
                     == STL_SUCCESS );

            if( ((qlncHashInstantNode*)aNode)->mNonFilterSubQueryExprList != NULL )
            {
                STL_TRY( qlncPostOptFPDSubQueryExprList(
                             aParamInfo,
                             ((qlncHashInstantNode*)aNode)->mNonFilterSubQueryExprList,
                             aEnv )
                         == STL_SUCCESS );
            }

            if( ((qlncHashInstantNode*)aNode)->mFilter != NULL )
            {
                STL_TRY( qlncPostOptFPDSubQueryOnAndFilter(
                             aParamInfo,
                             ((qlncHashInstantNode*)aNode)->mFilter,
                             aEnv )
                         == STL_SUCCESS );
            }

            break;

        default:
            STL_DASSERT( 0 );

            break;
    }


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SubQuery Expression List에 대하여 Post Filter Push Down을 수행한다.
 * @param[in]   aParamInfo          Parameter Info
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aEnv                Environment
 */
stlStatus qlncPostOptFPDSubQueryExprList( qlncParamInfo     * aParamInfo,
                                          qlncRefExprList   * aSubQueryExprList,
                                          qllEnv            * aEnv )
{
    qlncRefExprItem * sRefExprItem  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSubQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );


    sRefExprItem = aSubQueryExprList->mHead;
    while( sRefExprItem != NULL )
    {
        STL_DASSERT( sRefExprItem->mExpr->mType == QLNC_EXPR_TYPE_SUB_QUERY );
        STL_TRY( qlncPostOptFilterPushDown( aParamInfo,
                                            ((qlncExprSubQuery*)(sRefExprItem->mExpr))->mNode,
                                            QLNC_JOIN_TYPE_INVALID,
                                            aEnv )
                 == STL_SUCCESS );

        sRefExprItem = sRefExprItem->mNext;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief And Filter에 존재하는 SubQuery에 대하여 Post Filter Push Down을 수행한다.
 * @param[in]   aParamInfo  Parameter Info
 * @param[in]   aAndFilter  AndFilter
 * @param[in]   aEnv        Environment
 */
stlStatus qlncPostOptFPDSubQueryOnAndFilter( qlncParamInfo  * aParamInfo,
                                             qlncAndFilter  * aAndFilter,
                                             qllEnv         * aEnv )
{
    stlInt32          i;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAndFilter != NULL, QLL_ERROR_STACK(aEnv) );


    for( i = 0; i < aAndFilter->mOrCount; i++ )
    {
        if( aAndFilter->mOrFilters[i]->mSubQueryExprList != NULL )
        {
            STL_TRY( qlncPostOptFPDSubQueryExprList(
                         aParamInfo,
                         aAndFilter->mOrFilters[i]->mSubQueryExprList,
                         aEnv )
                     == STL_SUCCESS );
        }
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Sub Table Node에 대한 Post Filter Push Down을 수행한다.
 * @param[in]   aParamInfo      Parameter Info
 * @param[in]   aSubTableNode   Candidate Sub Table Node
 * @param[in]   aEnv            Environment
 */
stlStatus qlncPostOptFPDSubTableNode( qlncParamInfo     * aParamInfo,
                                      qlncSubTableNode  * aSubTableNode,
                                      qllEnv            * aEnv )
{
    stlInt32          i;
    qlncFilterCost  * sFilterCost   = NULL;
    qlncAndFilter   * sPushFilter   = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSubTableNode != NULL, QLL_ERROR_STACK(aEnv) );


    /* Filter가 존재하는지 체크 */
    STL_TRY_THROW( aSubTableNode->mFilterCost->mFilter != NULL,
                   RAMP_FINISH );

    /**
     * @todo Query Node가 Set Operator인 경우 Filter Push Down은 향후 진행
     */

    STL_TRY_THROW( aSubTableNode->mQueryNode->mNodeType == QLNC_NODE_TYPE_QUERY_SPEC,
                   RAMP_FINISH );

    /* Query Spec Node에 Offset/Limit 구문이 있으면 안된다. */
    STL_TRY_THROW( (((qlncQuerySpec*)(aSubTableNode->mQueryNode))->mOffset == NULL) &&
                   (((qlncQuerySpec*)(aSubTableNode->mQueryNode))->mLimit == NULL),
                   RAMP_FINISH );

    /* 하위 노드의 Target에 Aggregation이 존재하면 Push 불가 */
    for( i = 0; i < ((qlncQuerySpec*)(aSubTableNode->mQueryNode))->mTargetCount; i++ )
    {
        if( qlncHasAggregationOnExpr(
                ((qlncQuerySpec*)(aSubTableNode->mQueryNode))->mTargetArr[i].mExpr,
                ((qlncQuerySpec*)(aSubTableNode->mQueryNode))->mNodeCommon.mNodeID,
                STL_FALSE )
            == STL_TRUE )
        {
            break;
        }
    }

    STL_TRY_THROW( i >= ((qlncQuerySpec*)(aSubTableNode->mQueryNode))->mTargetCount,
                   RAMP_FINISH );

    /* Push Filter 할당 및 초기화 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncAndFilter ),
                                (void**) &sPushFilter,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    QLNC_INIT_AND_FILTER( sPushFilter );

    /* Filter에서 SubTable의 Target Column에 해당하는 부분을 Query Spec의 Target Expr로 대체 */
    sFilterCost = aSubTableNode->mFilterCost;
    for( i = 0; i < sFilterCost->mFilter->mOrCount; i++ )
    {
        /* SubQuery를 갖는 Filter는 더이상 Push하지 않는다. */
        if( sFilterCost->mFilter->mOrFilters[i]->mSubQueryExprList != NULL )
        {
            continue;
        }

        STL_TRY( qlncFPDChangeInnerColumnToTargetExpr(
                     aParamInfo,
                     aSubTableNode->mNodeCommon.mNodeID,
                     ((qlncQuerySpec*)(aSubTableNode->mQueryNode))->mTargetCount,
                     ((qlncQuerySpec*)(aSubTableNode->mQueryNode))->mTargetArr,
                     (qlncExprCommon*)(sFilterCost->mFilter->mOrFilters[i]),
                     (qlncExprCommon**)&(sFilterCost->mFilter->mOrFilters[i]),
                     aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                    sPushFilter,
                                                    1,
                                                    &(sFilterCost->mFilter->mOrFilters[i]),
                                                    aEnv )
                 == STL_SUCCESS );

        sFilterCost->mFilter->mOrFilters[i] = NULL;
    }

    /* Filter Cost의 Filter Compact */
    STL_TRY( qlncCompactAndFilter( sFilterCost->mFilter,
                                   aEnv )
             == STL_SUCCESS );

    if( sFilterCost->mFilter->mOrCount == 0 )
    {
        sFilterCost->mFilter = NULL;
    }

    /* Push Filter가 없으면 종료 */
    STL_TRY_THROW( sPushFilter->mOrCount > 0, RAMP_FINISH );

    /* Push Filter에 대하여 Reference Node List에서 Outer Table Node를 제거한다. */
    STL_TRY( qlncRemoveOuterTableNodeFromRefNodeList(
                 aParamInfo,
                 (qlncExprCommon*)sPushFilter,
                 aSubTableNode->mQueryNode,
                 (qlncExprCommon**)&sPushFilter,
                 aEnv )
             == STL_SUCCESS );

    /* Query Node에 적용하여 재계산한다. */
    STL_TRY( qlncCBOptRecomputeNodeCostByPushFilter(
                 aParamInfo,
                 ((qlncQuerySpecCost*)(aSubTableNode->mQueryNode->mBestCost))->mChildNode,
                 sPushFilter,
                 &(((qlncQuerySpecCost*)(aSubTableNode->mQueryNode->mBestCost))->mChildNode),
                 aEnv )
             == STL_SUCCESS );


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Nested Loops Join에 대한 Post Filter Push Down을 수행한다.
 * @param[in]   aParamInfo      Parameter Info
 * @param[in]   aNode           Candidate Node
 * @param[in]   aJoinType       Join Type
 * @param[in]   aEnv            Environment
 */
stlStatus qlncPostOptFPDNestedJoin( qlncParamInfo   * aParamInfo,
                                    qlncNodeCommon  * aNode,
                                    qlncJoinType      aJoinType,
                                    qllEnv          * aEnv )
{
#if 0
    stlInt32              i;
    qlncNodeCommon      * sPushDownNode     = NULL;
    qlncCBOptParamInfo    sCBOptParamInfo;
    qlncInnerJoinNodeInfo sInnerJoinNodeInfoArr[2];
#endif

    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode != NULL, QLL_ERROR_STACK(aEnv) );


#if 0
            /* Join Condition 복사 */
            STL_TRY( qlncDuplicateExpr(
                         &(aCBOptParamInfo->mParamInfo),
                         (qlncExprCommon*)sJoinCondition,
                         (qlncExprCommon**)&(sJoinCondition),
                         aEnv )
                     == STL_SUCCESS );

            /* Push Filter에 대하여 Reference Node List에서 Outer Table Node를 제거한다. */
            STL_TRY( qlncRemoveOuterTableNodeReference( &(aCBOptParamInfo->mParamInfo),
                                                        (qlncExprCommon*)sJoinCondition,
                                                        sRightNode,
                                                        (qlncExprCommon**)&sJoinCondition,
                                                        aEnv )
                     == STL_SUCCESS );

            for( i = 0; i < sJoinCondition->mOrCount; i++ )
            {
                sPushDownNode = NULL;

                STL_TRY( qlncPostOptFindNodeForPushDownNonSubQueryFilter(
                             sRightNode,
                             sJoinCondition->mOrFilters[i],
                             &sPushDownNode,
                             aEnv )
                         == STL_SUCCESS );

                STL_DASSERT( sPushDownNode != NULL );

                /* Push Down 노드가 존재하는 경우 해당 Node에 대하여 Cost를 새로 계산함 */
                if( sPushDownNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE )
                {
                    /* Push Filter 설정 */
                    if( ((qlncBaseTableNode*)sPushDownNode)->mFilter == NULL )
                    {
                        STL_TRY( knlAllocRegionMem(
                                     &QLL_CANDIDATE_MEM( aEnv ),
                                     STL_SIZEOF( qlncAndFilter ),
                                     (void**) &(((qlncBaseTableNode*)sPushDownNode)->mFilter),
                                     KNL_ENV(aEnv) )
                                 == STL_SUCCESS );
                        QLNC_INIT_AND_FILTER( ((qlncBaseTableNode*)sPushDownNode)->mFilter );
                    }

                    STL_TRY( qlncAppendOrFilterToAndFilterTail(
                                 &(aCBOptParamInfo->mParamInfo),
                                 ((qlncBaseTableNode*)sPushDownNode)->mFilter,
                                 1,
                                 &(sJoinCondition->mOrFilters[i]),
                                 aEnv )
                             == STL_SUCCESS );

                    /* Optimizer를 수행하도록 하기 위한 초기화 */
                    ((qlncBaseTableNode*)sPushDownNode)->mNodeCommon.mBestOptType = QLNC_BEST_OPT_TYPE_NA;
                    ((qlncBaseTableNode*)sPushDownNode)->mNodeCommon.mBestCost = NULL;
                    ((qlncBaseTableNode*)sPushDownNode)->mNodeCommon.mIsAnalyzedCBO = STL_FALSE;

                    ((qlncBaseTableNode*)sPushDownNode)->mTableScanCost = NULL;
                    ((qlncBaseTableNode*)sPushDownNode)->mIndexScanCostCount = 0;
                    ((qlncBaseTableNode*)sPushDownNode)->mIndexScanCostArr = NULL;
                    ((qlncBaseTableNode*)sPushDownNode)->mIndexCombineCost = NULL;
                    ((qlncBaseTableNode*)sPushDownNode)->mRowidScanCost = NULL;

                    /* Base Table Node Cost 계산 */
                    sCBOptParamInfo.mParamInfo = aCBOptParamInfo->mParamInfo;
                    sCBOptParamInfo.mLeftNode = sPushDownNode;
                    sCBOptParamInfo.mRightNode = NULL;
                    sCBOptParamInfo.mJoinCond = NULL;
                    sCBOptParamInfo.mFilter = NULL;
                    sCBOptParamInfo.mCost = NULL;

                    STL_TRY( qlncCBOptCopiedBaseTable( &sCBOptParamInfo,
                                                       aEnv )
                             == STL_SUCCESS );
                }
                else if( sPushDownNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE )
                {
                    /* Push Filter 설정 */
                    if( ((qlncSubTableNode*)sPushDownNode)->mFilter == NULL )
                    {
                        STL_TRY( knlAllocRegionMem(
                                     &QLL_CANDIDATE_MEM( aEnv ),
                                     STL_SIZEOF( qlncAndFilter ),
                                     (void**) &(((qlncSubTableNode*)sPushDownNode)->mFilter),
                                     KNL_ENV(aEnv) )
                                 == STL_SUCCESS );
                        QLNC_INIT_AND_FILTER( ((qlncSubTableNode*)sPushDownNode)->mFilter );
                    }

                    STL_TRY( qlncAppendOrFilterToAndFilterTail(
                                 &(aCBOptParamInfo->mParamInfo),
                                 ((qlncSubTableNode*)sPushDownNode)->mFilter,
                                 1,
                                 &(sJoinCondition->mOrFilters[i]),
                                 aEnv )
                             == STL_SUCCESS );

                    /* Optimizer를 수행하도록 하기 위한 초기화 */
                    ((qlncSubTableNode*)sPushDownNode)->mNodeCommon.mBestOptType = QLNC_BEST_OPT_TYPE_NA;
                    ((qlncSubTableNode*)sPushDownNode)->mNodeCommon.mBestCost = NULL;
                    ((qlncSubTableNode*)sPushDownNode)->mNodeCommon.mIsAnalyzedCBO = STL_FALSE;

                    ((qlncSubTableNode*)sPushDownNode)->mFilterCost = NULL;

                    /* Sub Table Node Cost 계산 */
                    sCBOptParamInfo.mParamInfo = aCBOptParamInfo->mParamInfo;
                    sCBOptParamInfo.mLeftNode = sPushDownNode;
                    sCBOptParamInfo.mRightNode = NULL;
                    sCBOptParamInfo.mJoinCond = NULL;
                    sCBOptParamInfo.mFilter = NULL;
                    sCBOptParamInfo.mCost = NULL;

                    STL_TRY( qlncCBOptSubTable( &sCBOptParamInfo,
                                                aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    qlncAndFilter     sPushFilter;

                    STL_DASSERT( sPushDownNode->mNodeType == QLNC_NODE_TYPE_INNER_JOIN_TABLE );

                    /* Push Filter 설정 */
                    QLNC_INIT_AND_FILTER( &sPushFilter );

                    /* Push Filter에 기존 Inner Join Table Node의 Join Condition을 추가 */
                    if( ((qlncInnerJoinTableNode*)sPushDownNode)->mJoinCondition != NULL )
                    {
                        STL_TRY( qlncAppendOrFilterToAndFilterTail(
                                     &(aCBOptParamInfo->mParamInfo),
                                     &sPushFilter,
                                     ((qlncInnerJoinTableNode*)sPushDownNode)->mJoinCondition->mOrCount,
                                     ((qlncInnerJoinTableNode*)sPushDownNode)->mJoinCondition->mOrFilters,
                                     aEnv )
                                 == STL_SUCCESS );
                    }

                    /* Push Filter에 기존 Inner Join Table Node의 Join Filter를 추가 */
                    if( ((qlncInnerJoinTableNode*)sPushDownNode)->mJoinFilter != NULL )
                    {
                        STL_TRY( qlncAppendOrFilterToAndFilterTail(
                                     &(aCBOptParamInfo->mParamInfo),
                                     &sPushFilter,
                                     ((qlncInnerJoinTableNode*)sPushDownNode)->mJoinFilter->mOrCount,
                                     ((qlncInnerJoinTableNode*)sPushDownNode)->mJoinFilter->mOrFilters,
                                     aEnv )
                                 == STL_SUCCESS );
                    }

                    STL_TRY( qlncAppendOrFilterToAndFilterTail(
                                 &(aCBOptParamInfo->mParamInfo),
                                 &sPushFilter,
                                 1,
                                 &(sJoinCondition->mOrFilters[i]),
                                 aEnv )
                             == STL_SUCCESS );

                    /* Inner Join Node Info 설정 */
                    sInnerJoinNodeInfoArr[0].mNode = ((qlncInnerJoinTableNode*)sPushDownNode)->mLeftNode;
                    sInnerJoinNodeInfoArr[0].mAnalyzedNode = NULL;
                    sInnerJoinNodeInfoArr[0].mIsFixed = STL_FALSE;
                    sInnerJoinNodeInfoArr[0].mIsAnalyzed = STL_FALSE;

                    sInnerJoinNodeInfoArr[1].mNode = ((qlncInnerJoinTableNode*)sPushDownNode)->mRightNode;
                    sInnerJoinNodeInfoArr[1].mAnalyzedNode = NULL;
                    sInnerJoinNodeInfoArr[1].mIsFixed = STL_FALSE;
                    sInnerJoinNodeInfoArr[1].mIsAnalyzed = STL_FALSE;

                    /* Join Table Node Cost 계산 */
                    sCBOptParamInfo.mParamInfo = aCBOptParamInfo->mParamInfo;
                    sCBOptParamInfo.mLeftNode = ((qlncInnerJoinTableNode*)sPushDownNode)->mLeftNode;
                    sCBOptParamInfo.mRightNode = ((qlncInnerJoinTableNode*)sPushDownNode)->mRightNode;
                    sCBOptParamInfo.mJoinCond = (sPushFilter.mOrCount == 0 ? NULL : &sPushFilter);
                    sCBOptParamInfo.mFilter = NULL;
                    sCBOptParamInfo.mCost = NULL;

                    STL_TRY( qlncCBOptInnerJoinCost( &sCBOptParamInfo,
                                                     sInnerJoinNodeInfoArr,
                                                     2,
                                                     1,
                                                     aEnv )
                             == STL_SUCCESS );

                    stlMemcpy( sPushDownNode,
                               sCBOptParamInfo.mRightNode,
                               STL_SIZEOF( qlncInnerJoinTableNode ) );
                }
            }

            sJoinCondition = NULL;
#endif


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Merge Join에 대한 Post Filter Push Down을 수행한다.
 * @param[in]   aParamInfo      Parameter Info
 * @param[in]   aNode           Candidate Node
 * @param[in]   aJoinType       Join Type
 * @param[in]   aEnv            Environment
 */
stlStatus qlncPostOptFPDMergeJoin( qlncParamInfo    * aParamInfo,
                                   qlncNodeCommon   * aNode,
                                   qlncJoinType       aJoinType,
                                   qllEnv           * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode != NULL, QLL_ERROR_STACK(aEnv) );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Hash Join에 대한 Post Filter Push Down을 수행한다.
 * @param[in]   aParamInfo      Parameter Info
 * @param[in]   aNode           Candidate Node
 * @param[in]   aJoinType       Join Type
 * @param[in]   aEnv            Environment
 */
stlStatus qlncPostOptFPDHashJoin( qlncParamInfo     * aParamInfo,
                                  qlncNodeCommon    * aNode,
                                  qlncJoinType        aJoinType,
                                  qllEnv            * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode != NULL, QLL_ERROR_STACK(aEnv) );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


#if 0
/**
 * @brief Push Filter를 Child Node에 반영한다.
 * @param[in]   aParamInfo      Parameter Info
 * @param[in]   aChildNode      Child Candidate Node
 * @param[in]   aPushOrFilter   Push OR Filter
 * @param[in]   aEnv            Environment
 */
stlStatus qlncPostOptFPDApplyToChildNode( qlncParamInfo     * aParamInfo,
                                          qlncNodeCommon    * aChildNode,
                                          qlncOrFilter      * aPushOrFilter,
                                          qllEnv            * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aChildNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPushOrFilter != NULL, QLL_ERROR_STACK(aEnv) );


#define _QLNC_APPLY_PUSH_FILTER( _aDstFilter )          \
    {                                                   \
        if( (_aDstFilter) == NULL )                     \
        {                                               \
            STL_TRY( knlAllocRegionMem(                 \
                         &QLL_CANDIDATE_MEM( aEnv ),    \
                         STL_SIZEOF( qlncAndFilter ),   \
                         (void**) &(_aDstFilter),       \
                         KNL_ENV(aEnv) )                \
                     == STL_SUCCESS );                  \
            QLNC_INIT_AND_FILTER( (_aDstFilter) );      \
        }                                               \
                                                        \
        STL_TRY( qlncAppendOrFilterToAndFilterTail(     \
                     aParamInfo,                        \
                     (_aDstFilter),                     \
                     1,                                 \
                     &aPushOrFilter,                    \
                     aEnv )                             \
                 == STL_SUCCESS );                      \
    }


    switch( aChildNode->mNodeType )
    {
        case QLNC_NODE_TYPE_BASE_TABLE:
            {
                /* Base Table의 Filter에 추가 */
                _QLNC_APPLY_PUSH_FILTER( ((qlncBaseTableNode*)aChildNode)->mFilter );

                break;
            }
        case QLNC_NODE_TYPE_SUB_TABLE:
            {
                /* Sub Table의 Filter에 추가 */
                _QLNC_APPLY_PUSH_FILTER( ((qlncSubTableNode*)aChildNode)->mFilter );

                break;
            }
        case QLNC_NODE_TYPE_INNER_JOIN_TABLE:
            {
                /* Inner Join Table의 Join Filter에 추가 */
                _QLNC_APPLY_PUSH_FILTER( ((qlncInnerJoinTableNode*)aChildNode)->mJoinFilter );

                break;
            }
        case QLNC_NODE_TYPE_OUTER_JOIN_TABLE:
            {
                /* Outer Join Table의 Join Filter에 추가 */
                _QLNC_APPLY_PUSH_FILTER( ((qlncOuterJoinTableNode*)aChildNode)->mJoinFilter );

                break;
            }
        case QLNC_NODE_TYPE_SEMI_JOIN_TABLE:
            {
                /* Semi Join Table의 Join Filter에 추가 */
                _QLNC_APPLY_PUSH_FILTER( ((qlncSemiJoinTableNode*)aChildNode)->mJoinFilter );

                break;
            }
        case QLNC_NODE_TYPE_ANTI_SEMI_JOIN_TABLE:
            {
                /* Anti-Semi Join Table의 Join Filter에 추가 */
                _QLNC_APPLY_PUSH_FILTER( ((qlncAntiSemiJoinTableNode*)aChildNode)->mJoinFilter );

                break;
            }
        case QLNC_NODE_TYPE_SORT_INSTANT:
            {
                /* Sort Instant의 Filter에 추가 */
                _QLNC_APPLY_PUSH_FILTER( ((qlncSortInstantNode*)aChildNode)->mFilter );

                break;
            }
        case QLNC_NODE_TYPE_HASH_INSTANT:
            {
                /* Hash Instant의 Filter에 추가 */
                _QLNC_APPLY_PUSH_FILTER( ((qlncHashInstantNode*)aChildNode)->mFilter );

                break;
            }
        default:
            {
                STL_DASSERT( 0 );
                break;
            }
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
#endif


/**
 * @brief 현재노드부터 Child Node들 모두에 대한 Node Array를 만든다.
 * @param[in,out]   aNodeArray  Candidate Node Array
 * @param[in]       aExpr       Expression
 * @param[in]       aEnv        Environment
 */
stlStatus qlncPostOptMakeNodeArrayOnExpr( qlncNodeArray     * aNodeArray,
                                          qlncExprCommon    * aExpr,
                                          qllEnv            * aEnv )
{
    stlInt32      i;


    STL_PARAM_VALIDATE( aNodeArray != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );


    switch( aExpr->mType )
    {
        case QLNC_EXPR_TYPE_VALUE:
        case QLNC_EXPR_TYPE_BIND_PARAM:
        case QLNC_EXPR_TYPE_COLUMN:
        case QLNC_EXPR_TYPE_ROWID_COLUMN:
        case QLNC_EXPR_TYPE_PSEUDO_COLUMN:
        case QLNC_EXPR_TYPE_PSEUDO_ARGUMENT:
            {
                break;
            }
        case QLNC_EXPR_TYPE_FUNCTION:
            {
                qlncExprFunction    * sFunction = NULL;

                sFunction = (qlncExprFunction*)aExpr;
                for( i = 0; i < sFunction->mArgCount; i++ )
                {
                    STL_TRY( qlncPostOptMakeNodeArrayOnExpr( aNodeArray,
                                                             sFunction->mArgs[i],
                                                             aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_EXPR_TYPE_CAST:
            {
                qlncExprTypeCast    * sTypeCast = NULL;

                sTypeCast = (qlncExprTypeCast*)aExpr;
                for( i = 0; i < sTypeCast->mArgCount; i++ )
                {
                    STL_TRY( qlncPostOptMakeNodeArrayOnExpr( aNodeArray,
                                                             sTypeCast->mArgs[i],
                                                             aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT:
            {
                STL_TRY( qlncPostOptMakeNodeArrayOnExpr( aNodeArray,
                                                         ((qlncExprConstExpr*)aExpr)->mOrgExpr,
                                                         aEnv )
                         == STL_SUCCESS );
                break;
            }
        case QLNC_EXPR_TYPE_SUB_QUERY:
            {
                STL_TRY( qlncPostOptMakeNodeArrayOnCurrNode( aNodeArray,
                                                             ((qlncExprSubQuery*)aExpr)->mNode,
                                                             aEnv )
                         == STL_SUCCESS );
                break;
            }
        case QLNC_EXPR_TYPE_INNER_COLUMN:
            {
                if( ((qlncExprInnerColumn*)aExpr)->mOrgExpr != NULL )
                {
                    STL_TRY( qlncPostOptMakeNodeArrayOnExpr( aNodeArray,
                                                             ((qlncExprInnerColumn*)aExpr)->mOrgExpr,
                                                             aEnv )
                             == STL_SUCCESS );
                }
                break;
            }
        case QLNC_EXPR_TYPE_AGGREGATION:
            {
                qlncExprAggregation * sAggregation  = NULL;

                sAggregation = (qlncExprAggregation*)aExpr;
                for( i = 0; i < sAggregation->mArgCount; i++ )
                {
                    STL_TRY( qlncPostOptMakeNodeArrayOnExpr( aNodeArray,
                                                             sAggregation->mArgs[i],
                                                             aEnv )
                             == STL_SUCCESS );
                }

                if( sAggregation->mFilter != NULL )
                {
                    /* @todo Aggregation Filter는 향후 구현한다. */
                    STL_TRY( qlncPostOptMakeNodeArrayOnExpr( aNodeArray,
                                                             sAggregation->mFilter,
                                                             aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_EXPR_TYPE_CASE_EXPR:
            {
                qlncExprCaseExpr    * sCaseExpr = NULL;

                sCaseExpr = (qlncExprCaseExpr*)aExpr;
                for( i = 0; i < sCaseExpr->mCount; i++ )
                {
                    STL_TRY( qlncPostOptMakeNodeArrayOnExpr( aNodeArray,
                                                             sCaseExpr->mWhenArr[i],
                                                             aEnv )
                             == STL_SUCCESS );

                    STL_TRY( qlncPostOptMakeNodeArrayOnExpr( aNodeArray,
                                                             sCaseExpr->mThenArr[i],
                                                             aEnv )
                             == STL_SUCCESS );
                }

                STL_TRY( qlncPostOptMakeNodeArrayOnExpr( aNodeArray,
                                                         sCaseExpr->mDefResult,
                                                         aEnv )
                         == STL_SUCCESS );

                break;
            }
        case QLNC_EXPR_TYPE_LIST_FUNCTION:
            {
                qlncExprListFunc    * sListFunc = NULL;

                sListFunc = (qlncExprListFunc*)aExpr;
                for( i = 0; i < sListFunc->mArgCount; i++ )
                {
                    STL_TRY( qlncPostOptMakeNodeArrayOnExpr( aNodeArray,
                                                             sListFunc->mArgs[i],
                                                             aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_EXPR_TYPE_LIST_COLUMN:
            {
                qlncExprListCol * sListCol  = NULL;

                sListCol = (qlncExprListCol*)aExpr;
                for( i = 0; i < sListCol->mArgCount; i++ )
                {
                    STL_TRY( qlncPostOptMakeNodeArrayOnExpr( aNodeArray,
                                                             sListCol->mArgs[i],
                                                             aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_EXPR_TYPE_ROW_EXPR:
            {
                qlncExprRowExpr * sRowExpr  = NULL;

                sRowExpr = (qlncExprRowExpr*)aExpr;
                for( i = 0; i < sRowExpr->mArgCount; i++ )
                {
                    STL_TRY( qlncPostOptMakeNodeArrayOnExpr( aNodeArray,
                                                             sRowExpr->mArgs[i],
                                                             aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_EXPR_TYPE_AND_FILTER:
            {
                qlncAndFilter   * sAndFilter    = NULL;

                sAndFilter = (qlncAndFilter*)aExpr;
                for( i = 0; i < sAndFilter->mOrCount; i++ )
                {
                    STL_TRY( qlncPostOptMakeNodeArrayOnExpr( aNodeArray,
                                                             (qlncExprCommon*)(sAndFilter->mOrFilters[i]),
                                                             aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_EXPR_TYPE_OR_FILTER:
            {
                qlncOrFilter    * sOrFilter = NULL;

                sOrFilter = (qlncOrFilter*)aExpr;
                for( i = 0; i < sOrFilter->mFilterCount; i++ )
                {
                    STL_TRY( qlncPostOptMakeNodeArrayOnExpr( aNodeArray,
                                                             sOrFilter->mFilters[i],
                                                             aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_EXPR_TYPE_BOOLEAN_FILTER:
            {
                STL_TRY( qlncPostOptMakeNodeArrayOnExpr( aNodeArray,
                                                         ((qlncBooleanFilter*)aExpr)->mExpr,
                                                         aEnv )
                         == STL_SUCCESS );
                break;
            }
        case QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER:
            {
                STL_TRY( qlncPostOptMakeNodeArrayOnExpr( aNodeArray,
                                                         ((qlncConstBooleanFilter*)aExpr)->mExpr,
                                                         aEnv )
                         == STL_SUCCESS );
                break;
            }
        /* case QLNC_EXPR_TYPE_REFERENCE: */
        /* case QLNC_EXPR_TYPE_OUTER_COLUMN: */
        default:
            {
                STL_DASSERT( 0 );
                break;
            }
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 현재노드부터 Child Node들 모두에 대한 Node Array를 만든다.
 * @param[in,out]   aNodeArray  Candidate Node Array
 * @param[in]       aNode       Candidate Node
 * @param[in]       aEnv        Environment
 */
stlStatus qlncPostOptMakeNodeArrayOnCurrNode( qlncNodeArray     * aNodeArray,
                                              qlncNodeCommon    * aNode,
                                              qllEnv            * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aNodeArray != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode != NULL, QLL_ERROR_STACK(aEnv) );


#define _MAKE_NODE_ARRAY_ON_AND_FILTER( _aAndFilter )   \
    {                                                   \
        if( (_aAndFilter) != NULL )                     \
        {                                               \
            STL_TRY( qlncPostOptMakeNodeArrayOnExpr(    \
                         aNodeArray,                    \
                         (qlncExprCommon*)(_aAndFilter),\
                         aEnv )                         \
                     == STL_SUCCESS );                  \
        }                                               \
    }

    /* Node 추가 */
    STL_TRY( qlncAddNodeToNodeArray( aNodeArray,
                                     aNode,
                                     aEnv )
             == STL_SUCCESS );

    /* Best Cost에 대하여 계속 수행 */
    switch( aNode->mBestOptType )
    {
        case QLNC_BEST_OPT_TYPE_QUERY_SET:
            {
                STL_TRY( qlncPostOptMakeNodeArrayOnCurrNode( aNodeArray,
                                                             ((qlncQuerySet*)aNode)->mChildNode,
                                                             aEnv )
                         == STL_SUCCESS );
                break;
            }
        case QLNC_BEST_OPT_TYPE_SET_OP:
            {
                stlInt32      i;
                qlncSetOP   * sSetOP    = NULL;

                sSetOP = (qlncSetOP*)aNode;
                for( i = 0; i < sSetOP->mChildCount; i++ )
                {
                    STL_TRY( qlncPostOptMakeNodeArrayOnCurrNode( aNodeArray,
                                                                 sSetOP->mChildNodeArr[i],
                                                                 aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_BEST_OPT_TYPE_QUERY_SPEC:
            {
                stlInt32      i;

                STL_TRY( qlncPostOptMakeNodeArrayOnCurrNode( aNodeArray,
                                                             ((qlncQuerySpecCost*)(aNode->mBestCost))->mChildNode,
                                                             aEnv )
                         == STL_SUCCESS );

                for( i = 0; i < ((qlncQuerySpec*)aNode)->mTargetCount; i++ )
                {
                    STL_TRY( qlncPostOptMakeNodeArrayOnExpr( aNodeArray,
                                                             ((qlncQuerySpec*)aNode)->mTargetArr[i].mExpr,
                                                             aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_BEST_OPT_TYPE_TABLE_SCAN:
            {
                _MAKE_NODE_ARRAY_ON_AND_FILTER(
                    ((qlncTableScanCost*)(aNode->mBestCost))->mTableAndFilter );

                break;
            }
        case QLNC_BEST_OPT_TYPE_INDEX_SCAN:
            {
                _MAKE_NODE_ARRAY_ON_AND_FILTER(
                    ((qlncIndexScanCost*)(aNode->mBestCost))->mIndexRangeAndFilter );
                _MAKE_NODE_ARRAY_ON_AND_FILTER(
                    ((qlncIndexScanCost*)(aNode->mBestCost))->mIndexKeyAndFilter );
                _MAKE_NODE_ARRAY_ON_AND_FILTER(
                    ((qlncIndexScanCost*)(aNode->mBestCost))->mTableAndFilter );

                break;
            }
        case QLNC_BEST_OPT_TYPE_INDEX_COMBINE:
            {
                stlInt32      i;

                for( i = 0; i < ((qlncIndexCombineCost*)(aNode->mBestCost))->mDnfAndFilterCount; i++ )
                {
                    _MAKE_NODE_ARRAY_ON_AND_FILTER(
                        ((qlncIndexCombineCost*)(aNode->mBestCost))->mIndexScanCostArr[i].mIndexRangeAndFilter );
                    _MAKE_NODE_ARRAY_ON_AND_FILTER(
                        ((qlncIndexCombineCost*)(aNode->mBestCost))->mIndexScanCostArr[i].mIndexKeyAndFilter );
                    _MAKE_NODE_ARRAY_ON_AND_FILTER(
                        ((qlncIndexCombineCost*)(aNode->mBestCost))->mIndexScanCostArr[i].mTableAndFilter );
                }

                break;
            }
        case QLNC_BEST_OPT_TYPE_ROWID_SCAN:
            {
                _MAKE_NODE_ARRAY_ON_AND_FILTER(
                    ((qlncRowidScanCost*)(aNode->mBestCost))->mRowidAndFilter );
                _MAKE_NODE_ARRAY_ON_AND_FILTER(
                    ((qlncRowidScanCost*)(aNode->mBestCost))->mTableAndFilter );

                break;
            }
        case QLNC_BEST_OPT_TYPE_FILTER:
            {
                if( ((qlncFilterCost*)(aNode->mBestCost))->mMaterializedNode != NULL )
                {
                    /* @todo coverage를 위한 테스트 케이스를 만들기 어려움 */
                    STL_TRY( qlncPostOptMakeNodeArrayOnCurrNode(
                                 aNodeArray,
                                 ((qlncFilterCost*)(aNode->mBestCost))->mMaterializedNode,
                                 aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    STL_TRY( qlncPostOptMakeNodeArrayOnCurrNode(
                                 aNodeArray,
                                 ((qlncSubTableNode*)aNode)->mQueryNode,
                                 aEnv )
                             == STL_SUCCESS );

                    _MAKE_NODE_ARRAY_ON_AND_FILTER(
                        ((qlncFilterCost*)(aNode->mBestCost))->mFilter );
                }

                break;
            }
        case QLNC_BEST_OPT_TYPE_INNER_JOIN:
            {
                STL_TRY( qlncPostOptMakeNodeArrayOnCurrNode(
                             aNodeArray,
                             (qlncNodeCommon*)(((qlncJoinTableNode*)aNode)->mBestInnerJoinTable),
                             aEnv )
                         == STL_SUCCESS );

                break;
            }
        case QLNC_BEST_OPT_TYPE_OUTER_JOIN:
            {
                STL_TRY( qlncPostOptMakeNodeArrayOnCurrNode(
                             aNodeArray,
                             (qlncNodeCommon*)(((qlncJoinTableNode*)aNode)->mBestOuterJoinTable),
                             aEnv )
                         == STL_SUCCESS );

                break;
            }
        case QLNC_BEST_OPT_TYPE_SEMI_JOIN:
            {
                STL_TRY( qlncPostOptMakeNodeArrayOnCurrNode(
                             aNodeArray,
                             (qlncNodeCommon*)(((qlncJoinTableNode*)aNode)->mBestSemiJoinTable),
                             aEnv )
                         == STL_SUCCESS );

                break;
            }
        case QLNC_BEST_OPT_TYPE_ANTI_SEMI_JOIN:
            {
                STL_TRY( qlncPostOptMakeNodeArrayOnCurrNode(
                             aNodeArray,
                             (qlncNodeCommon*)(((qlncJoinTableNode*)aNode)->mBestAntiSemiJoinTable),
                             aEnv )
                         == STL_SUCCESS );

                break;
            }
        case QLNC_BEST_OPT_TYPE_NESTED_LOOPS:
            {
                STL_TRY( qlncPostOptMakeNodeArrayOnCurrNode(
                             aNodeArray,
                             ((qlncNestedJoinCost*)(aNode->mBestCost))->mLeftNode,
                             aEnv )
                         == STL_SUCCESS );

                STL_TRY( qlncPostOptMakeNodeArrayOnCurrNode(
                             aNodeArray,
                             ((qlncNestedJoinCost*)(aNode->mBestCost))->mRightNode,
                             aEnv )
                         == STL_SUCCESS );

                _MAKE_NODE_ARRAY_ON_AND_FILTER(
                    ((qlncNestedJoinCost*)(aNode->mBestCost))->mJoinCondition );
                _MAKE_NODE_ARRAY_ON_AND_FILTER(
                    ((qlncNestedJoinCost*)(aNode->mBestCost))->mJoinFilter );

                break;
            }
        case QLNC_BEST_OPT_TYPE_MERGE:
            {
                STL_TRY( qlncPostOptMakeNodeArrayOnCurrNode(
                             aNodeArray,
                             ((qlncMergeJoinCost*)(aNode->mBestCost))->mLeftNode,
                             aEnv )
                         == STL_SUCCESS );

                STL_TRY( qlncPostOptMakeNodeArrayOnCurrNode(
                             aNodeArray,
                             ((qlncMergeJoinCost*)(aNode->mBestCost))->mRightNode,
                             aEnv )
                         == STL_SUCCESS );

                _MAKE_NODE_ARRAY_ON_AND_FILTER(
                    ((qlncMergeJoinCost*)(aNode->mBestCost))->mEquiJoinCondition );
                _MAKE_NODE_ARRAY_ON_AND_FILTER(
                    ((qlncMergeJoinCost*)(aNode->mBestCost))->mNonEquiJoinCondition );
                _MAKE_NODE_ARRAY_ON_AND_FILTER(
                    ((qlncMergeJoinCost*)(aNode->mBestCost))->mJoinFilter );

                break;
            }
        case QLNC_BEST_OPT_TYPE_HASH:
            {
                STL_TRY( qlncPostOptMakeNodeArrayOnCurrNode(
                             aNodeArray,
                             ((qlncHashJoinCost*)(aNode->mBestCost))->mLeftNode,
                             aEnv )
                         == STL_SUCCESS );

                STL_TRY( qlncPostOptMakeNodeArrayOnCurrNode(
                             aNodeArray,
                             ((qlncHashJoinCost*)(aNode->mBestCost))->mRightNode,
                             aEnv )
                         == STL_SUCCESS );

                _MAKE_NODE_ARRAY_ON_AND_FILTER(
                    ((qlncHashJoinCost*)(aNode->mBestCost))->mEquiJoinCondition );
                _MAKE_NODE_ARRAY_ON_AND_FILTER(
                    ((qlncHashJoinCost*)(aNode->mBestCost))->mNonEquiJoinCondition );
                _MAKE_NODE_ARRAY_ON_AND_FILTER(
                    ((qlncHashJoinCost*)(aNode->mBestCost))->mJoinFilter );

                break;
            }
        case QLNC_BEST_OPT_TYPE_JOIN_COMBINE:
            {
                stlInt32      i;

                for( i = 0; i < ((qlncJoinCombineCost*)(aNode->mBestCost))->mDnfAndFilterCount; i++ )
                {
                    STL_TRY( qlncPostOptMakeNodeArrayOnCurrNode(
                                 aNodeArray,
                                 ((qlncJoinCombineCost*)(aNode->mBestCost))->mJoinNodePtrArr[i],
                                 aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_BEST_OPT_TYPE_SORT_INSTANT:
            {
                STL_TRY( qlncPostOptMakeNodeArrayOnCurrNode( aNodeArray,
                                                             ((qlncSortInstantNode*)aNode)->mChildNode,
                                                             aEnv )
                         == STL_SUCCESS );

                _MAKE_NODE_ARRAY_ON_AND_FILTER(
                    ((qlncSortInstantNode*)aNode)->mFilter );

                break;
            }
        case QLNC_BEST_OPT_TYPE_HASH_INSTANT:
        case QLNC_BEST_OPT_TYPE_GROUP:
            {
                STL_TRY( qlncPostOptMakeNodeArrayOnCurrNode( aNodeArray,
                                                             ((qlncHashInstantNode*)aNode)->mChildNode,
                                                             aEnv )
                         == STL_SUCCESS );

                _MAKE_NODE_ARRAY_ON_AND_FILTER(
                    ((qlncHashInstantNode*)aNode)->mFilter );

                break;
            }
        default:
            STL_DASSERT( 0 );
            break;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


#if 0
/**
 * @brief Non SubQuery Filter가 Push Down 가능한 Node를 찾는다.
 * @param[in]   aCurNode        Current Candidate Node
 * @param[in]   aOrFilter       Or Filter
 * @param[out]  aFoundNode      Found Candidate Node
 * @param[in]   aEnv            Environment
 */
stlStatus qlncPostOptFindNodeForPushDownNonSubQueryFilter( qlncNodeCommon   * aCurNode,
                                                           qlncOrFilter     * aOrFilter,
                                                           qlncNodeCommon  ** aFoundNode,
                                                           qllEnv           * aEnv )
{
    qlncNodeCommon  * sLeftNode     = NULL;
    qlncNodeCommon  * sRightNode    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCurNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOrFilter != NULL, QLL_ERROR_STACK(aEnv) );


    /* Left Node와 Right Node 찾기 */
    if( aCurNode->mBestOptType == QLNC_BEST_OPT_TYPE_NESTED_LOOPS )
    {
        sLeftNode = ((qlncNestedJoinCost*)(aCurNode->mBestCost))->mLeftNode;
        sRightNode = ((qlncNestedJoinCost*)(aCurNode->mBestCost))->mRightNode;
    }
    else if( aCurNode->mBestOptType == QLNC_BEST_OPT_TYPE_MERGE )
    {
        sLeftNode = ((qlncMergeJoinCost*)(aCurNode->mBestCost))->mLeftNode;
        sRightNode = ((qlncMergeJoinCost*)(aCurNode->mBestCost))->mRightNode;
    }
    else if( aCurNode->mBestOptType == QLNC_BEST_OPT_TYPE_HASH )
    {
        sLeftNode = ((qlncHashJoinCost*)(aCurNode->mBestCost))->mLeftNode;
        sRightNode = ((qlncHashJoinCost*)(aCurNode->mBestCost))->mRightNode;
    }
    else
    {
        STL_DASSERT( aCurNode->mBestOptType == QLNC_BEST_OPT_TYPE_JOIN_COMBINE );
        STL_THROW( RAMP_FINISH );
    }

    STL_DASSERT( (sLeftNode != NULL) && (sLeftNode->mIsAnalyzedCBO == STL_TRUE) );
    STL_DASSERT( (sRightNode != NULL) && (sRightNode->mIsAnalyzedCBO == STL_TRUE) );


    /* Left Node에 대한 처리 */
    STL_RAMP( RAMP_LEFT_RETRY );

    if( sLeftNode->mBestOptType != QLNC_BEST_OPT_TYPE_INDEX_COMBINE )
    {
        if( qlncIsUsableOrFilterOnNode( sLeftNode, aOrFilter ) == STL_TRUE )
        {
            *aFoundNode = sLeftNode;
        }
    }

    if( sLeftNode->mNodeType == QLNC_NODE_TYPE_INNER_JOIN_TABLE )
    {
        STL_TRY( qlncPostOptFindNodeForPushDownNonSubQueryFilter( sLeftNode,
                                                                  aOrFilter,
                                                                  aFoundNode,
                                                                  aEnv )
                 == STL_SUCCESS );
    }
    else if( sLeftNode->mNodeType == QLNC_NODE_TYPE_SORT_INSTANT )
    {
        sLeftNode = ((qlncSortInstantNode*)sLeftNode)->mChildNode;
        STL_THROW( RAMP_LEFT_RETRY );
    }
    else if( sLeftNode->mNodeType == QLNC_NODE_TYPE_HASH_INSTANT )
    {
        sLeftNode = ((qlncHashInstantNode*)sLeftNode)->mChildNode;
        STL_THROW( RAMP_LEFT_RETRY );
    }

    /* Right Node에 대한 처리 */
    STL_RAMP( RAMP_RIGHT_RETRY );

    if( sRightNode->mBestOptType != QLNC_BEST_OPT_TYPE_INDEX_COMBINE )
    {
        if( qlncIsUsableOrFilterOnNode( sRightNode, aOrFilter ) == STL_TRUE )
        {
            *aFoundNode = sRightNode;
        }
    }

    if( sRightNode->mNodeType == QLNC_NODE_TYPE_INNER_JOIN_TABLE )
    {
        STL_TRY( qlncPostOptFindNodeForPushDownNonSubQueryFilter( sRightNode,
                                                                  aOrFilter,
                                                                  aFoundNode,
                                                                  aEnv )
                 == STL_SUCCESS );
    }
    else if( sRightNode->mNodeType == QLNC_NODE_TYPE_SORT_INSTANT )
    {
        sRightNode = ((qlncSortInstantNode*)sRightNode)->mChildNode;
        STL_THROW( RAMP_RIGHT_RETRY );
    }
    else if( sRightNode->mNodeType == QLNC_NODE_TYPE_HASH_INSTANT )
    {
        sRightNode = ((qlncHashInstantNode*)sRightNode)->mChildNode;
        STL_THROW( RAMP_RIGHT_RETRY );
    }


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
#endif


/**
 * @brief Group By에 대하여 Pre-ordered Node를 이용한 변형을 수행한다.
 * @param[in]   aParamInfo  Parameter Info
 * @param[in]   aNode       Current Candidate Node
 * @param[in]   aOtherHint  Other Hint
 * @param[in]   aEnv        Environment
 */
stlStatus qlncPostOptGroupByPreorderedNode( qlncParamInfo   * aParamInfo,
                                            qlncNodeCommon  * aNode,
                                            qlncOtherHint   * aOtherHint,
                                            qllEnv          * aEnv )
{
    stlInt32          i;
    qlncFilterCost  * sFilterCost   = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode != NULL, QLL_ERROR_STACK(aEnv) );


    switch( aNode->mBestOptType )
    {
        case QLNC_BEST_OPT_TYPE_QUERY_SET:
            STL_TRY( qlncPostOptGroupByPreorderedNode( aParamInfo,
                                                       ((qlncQuerySet*)aNode)->mChildNode,
                                                       aOtherHint,
                                                       aEnv )
                     == STL_SUCCESS );
            break;

        case QLNC_BEST_OPT_TYPE_SET_OP:
            for( i = 0; i < ((qlncSetOP*)aNode)->mChildCount; i++ )
            {
                STL_TRY( qlncPostOptGroupByPreorderedNode( aParamInfo,
                                                           ((qlncSetOP*)aNode)->mChildNodeArr[i],
                                                           aOtherHint,
                                                           aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_BEST_OPT_TYPE_QUERY_SPEC:
            STL_TRY( qlncPostOptGroupByPreorderedNode( aParamInfo,
                                                       ((qlncQuerySpecCost*)(aNode->mBestCost))->mChildNode,
                                                       ((qlncQuerySpec*)aNode)->mOtherHint,
                                                       aEnv )
                     == STL_SUCCESS );

            if( ((qlncQuerySpec*)aNode)->mNonFilterSubQueryExprList != NULL )
            {
                STL_TRY( qlncPostOptGBPNSubQueryExprList(
                             aParamInfo,
                             ((qlncQuerySpec*)aNode)->mNonFilterSubQueryExprList,
                             aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_BEST_OPT_TYPE_TABLE_SCAN:
            if( ((qlncTableScanCost*)(aNode->mBestCost))->mTableAndFilter != NULL )
            {
                STL_TRY( qlncPostOptGBPNSubQueryOnAndFilter(
                             aParamInfo,
                             ((qlncTableScanCost*)(aNode->mBestCost))->mTableAndFilter,
                             aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_BEST_OPT_TYPE_INDEX_SCAN:
            if( ((qlncIndexScanCost*)(aNode->mBestCost))->mTableAndFilter != NULL )
            {
                STL_TRY( qlncPostOptGBPNSubQueryOnAndFilter(
                             aParamInfo,
                             ((qlncIndexScanCost*)(aNode->mBestCost))->mTableAndFilter,
                             aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_BEST_OPT_TYPE_INDEX_COMBINE:
            for( i = 0; i < ((qlncIndexCombineCost*)(aNode->mBestCost))->mDnfAndFilterCount; i++ )
            {
                if( ((qlncIndexCombineCost*)(aNode->mBestCost))->mIndexScanCostArr[i].mTableAndFilter != NULL )
                {
                    STL_TRY( qlncPostOptGBPNSubQueryOnAndFilter(
                                 aParamInfo,
                                 ((qlncIndexCombineCost*)(aNode->mBestCost))->mIndexScanCostArr[i].mTableAndFilter,
                                 aEnv )
                             == STL_SUCCESS );
                }
            }

            break;

        case QLNC_BEST_OPT_TYPE_ROWID_SCAN:
            if( ((qlncRowidScanCost*)(aNode->mBestCost))->mTableAndFilter != NULL )
            {
                STL_TRY( qlncPostOptGBPNSubQueryOnAndFilter(
                             aParamInfo,
                             ((qlncRowidScanCost*)(aNode->mBestCost))->mTableAndFilter,
                             aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_BEST_OPT_TYPE_FILTER:
            sFilterCost = (qlncFilterCost*)(aNode->mBestCost);
            if( sFilterCost->mMaterializedNode == NULL )
            {
                if( sFilterCost->mFilter != NULL )
                {
                    STL_TRY( qlncPostOptGBPNSubQueryOnAndFilter(
                                 aParamInfo,
                                 sFilterCost->mFilter,
                                 aEnv )
                             == STL_SUCCESS );
                }

                STL_TRY( qlncPostOptGroupByPreorderedNode( aParamInfo,
                                                           ((qlncSubTableNode*)aNode)->mQueryNode,
                                                           aOtherHint,
                                                           aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                STL_TRY( qlncPostOptGroupByPreorderedNode( aParamInfo,
                                                           sFilterCost->mMaterializedNode,
                                                           aOtherHint,
                                                           aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_BEST_OPT_TYPE_INNER_JOIN:
            STL_TRY( qlncPostOptGroupByPreorderedNode(
                         aParamInfo,
                         (qlncNodeCommon*)(((qlncJoinTableNode*)aNode)->mBestInnerJoinTable),
                         aOtherHint,
                         aEnv )
                     == STL_SUCCESS );

            break;

        case QLNC_BEST_OPT_TYPE_OUTER_JOIN:
            STL_TRY( qlncPostOptGroupByPreorderedNode(
                         aParamInfo,
                         (qlncNodeCommon*)(((qlncJoinTableNode*)aNode)->mBestOuterJoinTable),
                         aOtherHint,
                         aEnv )
                     == STL_SUCCESS );

            break;

        case QLNC_BEST_OPT_TYPE_SEMI_JOIN:
            STL_TRY( qlncPostOptGroupByPreorderedNode(
                         aParamInfo,
                         (qlncNodeCommon*)(((qlncJoinTableNode*)aNode)->mBestSemiJoinTable),
                         aOtherHint,
                         aEnv )
                     == STL_SUCCESS );

            break;

        case QLNC_BEST_OPT_TYPE_ANTI_SEMI_JOIN:
            STL_TRY( qlncPostOptGroupByPreorderedNode(
                         aParamInfo,
                         (qlncNodeCommon*)(((qlncJoinTableNode*)aNode)->mBestAntiSemiJoinTable),
                         aOtherHint,
                         aEnv )
                     == STL_SUCCESS );

            break;

        case QLNC_BEST_OPT_TYPE_NESTED_LOOPS:
            STL_TRY( qlncPostOptGroupByPreorderedNode(
                         aParamInfo,
                         ((qlncNestedJoinCost*)(aNode->mBestCost))->mLeftNode,
                         aOtherHint,
                         aEnv )
                     == STL_SUCCESS );

            STL_TRY( qlncPostOptGroupByPreorderedNode(
                         aParamInfo,
                         ((qlncNestedJoinCost*)(aNode->mBestCost))->mRightNode,
                         aOtherHint,
                         aEnv )
                     == STL_SUCCESS );

            if( ((qlncNestedJoinCost*)(aNode->mBestCost))->mJoinFilter != NULL )
            {
                STL_TRY( qlncPostOptGBPNSubQueryOnAndFilter(
                             aParamInfo,
                             ((qlncNestedJoinCost*)(aNode->mBestCost))->mJoinFilter,
                             aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_BEST_OPT_TYPE_MERGE:
            STL_TRY( qlncPostOptGroupByPreorderedNode(
                         aParamInfo,
                         ((qlncMergeJoinCost*)(aNode->mBestCost))->mLeftNode,
                         aOtherHint,
                         aEnv )
                     == STL_SUCCESS );

            STL_TRY( qlncPostOptGroupByPreorderedNode(
                         aParamInfo,
                         ((qlncMergeJoinCost*)(aNode->mBestCost))->mRightNode,
                         aOtherHint,
                         aEnv )
                     == STL_SUCCESS );

            if( ((qlncMergeJoinCost*)(aNode->mBestCost))->mJoinFilter != NULL )
            {
                STL_TRY( qlncPostOptGBPNSubQueryOnAndFilter(
                             aParamInfo,
                             ((qlncMergeJoinCost*)(aNode->mBestCost))->mJoinFilter,
                             aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_BEST_OPT_TYPE_HASH:
            STL_TRY( qlncPostOptGroupByPreorderedNode(
                         aParamInfo,
                         ((qlncHashJoinCost*)(aNode->mBestCost))->mLeftNode,
                         aOtherHint,
                         aEnv )
                     == STL_SUCCESS );

            STL_TRY( qlncPostOptGroupByPreorderedNode(
                         aParamInfo,
                         ((qlncHashJoinCost*)(aNode->mBestCost))->mRightNode,
                         aOtherHint,
                         aEnv )
                     == STL_SUCCESS );

            if( ((qlncHashJoinCost*)(aNode->mBestCost))->mJoinFilter != NULL )
            {
                STL_TRY( qlncPostOptGBPNSubQueryOnAndFilter(
                             aParamInfo,
                             ((qlncHashJoinCost*)(aNode->mBestCost))->mJoinFilter,
                             aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_BEST_OPT_TYPE_JOIN_COMBINE:
            for( i = 0; i < ((qlncJoinCombineCost*)(aNode->mBestCost))->mDnfAndFilterCount; i++ )
            {
                STL_TRY( qlncPostOptGroupByPreorderedNode(
                             aParamInfo,
                             ((qlncJoinCombineCost*)(aNode->mBestCost))->mJoinNodePtrArr[i],
                             aOtherHint,
                             aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_BEST_OPT_TYPE_SORT_INSTANT:
            STL_TRY( qlncPostOptGroupByPreorderedNode( aParamInfo,
                                                       ((qlncSortInstantNode*)aNode)->mChildNode,
                                                       aOtherHint,
                                                       aEnv )
                     == STL_SUCCESS );

            if( ((qlncSortInstantNode*)aNode)->mNonFilterSubQueryExprList != NULL )
            {
                STL_TRY( qlncPostOptGBPNSubQueryExprList(
                             aParamInfo,
                             ((qlncSortInstantNode*)aNode)->mNonFilterSubQueryExprList,
                             aEnv )
                         == STL_SUCCESS );
            }

            if( ((qlncSortInstantNode*)aNode)->mFilter != NULL )
            {
                STL_TRY( qlncPostOptGBPNSubQueryOnAndFilter(
                             aParamInfo,
                             ((qlncSortInstantNode*)aNode)->mFilter,
                             aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_BEST_OPT_TYPE_HASH_INSTANT:
        case QLNC_BEST_OPT_TYPE_GROUP:
            STL_TRY( qlncPostOptGroupByPreorderedNode( aParamInfo,
                                                       ((qlncHashInstantNode*)aNode)->mChildNode,
                                                       aOtherHint,
                                                       aEnv )
                     == STL_SUCCESS );

            if( ((qlncHashInstantNode*)aNode)->mNonFilterSubQueryExprList != NULL )
            {
                STL_TRY( qlncPostOptGBPNSubQueryExprList(
                             aParamInfo,
                             ((qlncHashInstantNode*)aNode)->mNonFilterSubQueryExprList,
                             aEnv )
                         == STL_SUCCESS );
            }

            if( ((qlncHashInstantNode*)aNode)->mFilter != NULL )
            {
                STL_TRY( qlncPostOptGBPNSubQueryOnAndFilter(
                             aParamInfo,
                             ((qlncHashInstantNode*)aNode)->mFilter,
                             aEnv )
                         == STL_SUCCESS );
            }

            if( ( ((qlncHashInstantNode*)aNode)->mInstantType == QLNC_INSTANT_TYPE_GROUP ) &&
                ( aOtherHint->mGroupOperType != QLNC_GROUP_OPER_TYPE_HASH ) )
            {
                STL_TRY( qlncPostOptGBPNGroupHashInstant( aParamInfo,
                                                          aNode,
                                                          aEnv )
                         == STL_SUCCESS );
            }

            if( ( ((qlncHashInstantNode*)aNode)->mInstantType == QLNC_INSTANT_TYPE_DISTINCT ) &&
                ( aOtherHint->mDistinctOperType != QLNC_DISTINCT_OPER_TYPE_HASH ) )
            {
                if( ((qlncHashInstantNode*)aNode)->mChildNode->mNodeType == QLNC_NODE_TYPE_HASH_INSTANT )
                {
                    /* Group By가 있는 Distinct인 경우 */
                    STL_TRY( qlncPostOptGBPNDistinctWithGroupHashInstant( aParamInfo,
                                                                          aNode,
                                                                          aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    /* Group By가 없는 Distinct인 경우 */
                    STL_TRY( qlncPostOptGBPNGroupHashInstant( aParamInfo,
                                                              aNode,
                                                              aEnv )
                             == STL_SUCCESS );
                }
            }

            break;

        default:
            STL_DASSERT( 0 );

            break;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SubQuery Expression List에 대하여 Group By Pre-ordered Node을 수행한다.
 * @param[in]   aParamInfo          Parameter Info
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aEnv                Environment
 */
stlStatus qlncPostOptGBPNSubQueryExprList( qlncParamInfo    * aParamInfo,
                                           qlncRefExprList  * aSubQueryExprList,
                                           qllEnv           * aEnv )
{
    qlncRefExprItem * sRefExprItem  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSubQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );


    sRefExprItem = aSubQueryExprList->mHead;
    while( sRefExprItem != NULL )
    {
        STL_DASSERT( sRefExprItem->mExpr->mType == QLNC_EXPR_TYPE_SUB_QUERY );
        STL_TRY( qlncPostOptGroupByPreorderedNode( aParamInfo,
                                                   ((qlncExprSubQuery*)(sRefExprItem->mExpr))->mNode,
                                                   NULL,
                                                   aEnv )
                 == STL_SUCCESS );

        sRefExprItem = sRefExprItem->mNext;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief And Filter에 존재하는 SubQuery에 대하여 Group By Pre-ordered Node을 수행한다.
 * @param[in]   aParamInfo  Parameter Info
 * @param[in]   aAndFilter  AndFilter
 * @param[in]   aEnv        Environment
 */
stlStatus qlncPostOptGBPNSubQueryOnAndFilter( qlncParamInfo * aParamInfo,
                                              qlncAndFilter * aAndFilter,
                                              qllEnv        * aEnv )
{
    stlInt32          i;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAndFilter != NULL, QLL_ERROR_STACK(aEnv) );


    for( i = 0; i < aAndFilter->mOrCount; i++ )
    {
        if( aAndFilter->mOrFilters[i]->mSubQueryExprList != NULL )
        {
            STL_TRY( qlncPostOptGBPNSubQueryExprList(
                         aParamInfo,
                         aAndFilter->mOrFilters[i]->mSubQueryExprList,
                         aEnv )
                     == STL_SUCCESS );
        }
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Group Hash Instant Node에 대하여 Group By Pre-ordered Node을 수행한다.
 * @param[in]   aParamInfo  Parameter Info
 * @param[in]   aNode       Current Candidate Node
 * @param[in]   aEnv        Environment
 */
stlStatus qlncPostOptGBPNGroupHashInstant( qlncParamInfo    * aParamInfo,
                                           qlncNodeCommon   * aNode,
                                           qllEnv           * aEnv )
{
    stlInt32              i;
    stlBool               sIsUsable;
    qlncHashInstantNode * sHashInstant  = NULL;
    qlncNodeCommon      * sNode         = NULL;
    qlncIndexStat       * sIndexStat    = NULL;
    qlncRefExprItem     * sRefExprItem  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode != NULL, QLL_ERROR_STACK(aEnv) );


    /* Hash Instant Node 설정 */
    sHashInstant = (qlncHashInstantNode*)aNode;

    /**
     * @todo Group By 자체가 제거 가능한지 판단해야 함
     */

    /* Group Key가 Constant로만 구성된 경우 하위노드 상관 없이 Group으로 변환 가능 */
    sRefExprItem = sHashInstant->mKeyColList->mHead;
    while( sRefExprItem != NULL )
    {
        STL_DASSERT( sRefExprItem->mExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN );

        if( ((qlncExprInnerColumn*)(sRefExprItem->mExpr))->mOrgExpr->mType
            == QLNC_EXPR_TYPE_COLUMN )
        {
            break;
        }
        else if( ( ((qlncExprInnerColumn*)(sRefExprItem->mExpr))->mOrgExpr->mType
                   != QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT ) &&
                 ( ((qlncExprInnerColumn*)(sRefExprItem->mExpr))->mOrgExpr->mType
                   != QLNC_EXPR_TYPE_VALUE ) )
        {
            /* Column도 Constant도 아니므로 불가 */
            STL_THROW( RAMP_FINISH );
        }

        sRefExprItem = sRefExprItem->mNext;
    }

    if( sRefExprItem == NULL )
    {
        STL_TRY( qlncCBOptHashInstantForPreorderedGroupBy(
                     aParamInfo,
                     aNode,
                     aEnv )
                 == STL_SUCCESS );

        STL_THROW( RAMP_FINISH );
    }

    /* Child Node가 Pre-ordered Node인지를 판별 */
    STL_TRY( qlncPostOptGBPNFindUsablePreorderedNode(
                 aParamInfo,
                 sHashInstant->mChildNode,
                 &sNode,
                 aEnv )
             == STL_SUCCESS );

    STL_TRY_THROW( sNode != NULL, RAMP_FINISH );

    if( sNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE )
    {
        if( sNode->mBestOptType == QLNC_BEST_OPT_TYPE_TABLE_SCAN )
        {
            /* case 1: Table Scan인 경우 */
            /* Filter가 존재하면 처리 불가 */
            /* @todo Filter가 존재하더라도 table scan -> index scan으로 변경시 증가하는 비용보다
             *       Group Hash Instant -> Group으로 변경시 감소하는 비용이 크면 강제 변경한다. */
            STL_TRY_THROW( ((qlncTableScanCost*)(sNode->mBestCost))->mTableAndFilter == NULL,
                           RAMP_FINISH );

            /* Group Key와 순서가 일치하는 인덱스를 찾는다. */
            for( i = 0; i < ((qlncBaseTableNode*)sNode)->mAccPathHint->mPossibleIndexScanCount; i++ )
            {
                STL_TRY( qlncPostOptGBPNIsUsableIndexForGroupBy(
                             aParamInfo,
                             sHashInstant->mKeyColList,
                             sNode->mNodeID,
                             &(((qlncBaseTableNode*)sNode)->mIndexStatArr[((qlncBaseTableNode*)sNode)->mAccPathHint->mPossibleIndexScanInfoArr[i].mOffset]),
                             &sIsUsable,
                             aEnv )
                         == STL_SUCCESS );

                /* 사용 가능한 Index이어야 한다. */
                if( sIsUsable == STL_TRUE )
                {
                    /* Index Scan Cost 재 계산 */
                    STL_TRY( qlncCBOptRecomputeIndexScanCost(
                                 aParamInfo,
                                 (qlncBaseTableNode*)sNode,
                                 i,
                                 aEnv )
                             == STL_SUCCESS );

                    /* Table 접근을 하지 않아야 한다. */
                    if( ((qlncBaseTableNode*)sNode)->mIndexScanCostArr[i].mNeedTableScan == STL_FALSE )
                    {
                        sNode->mBestOptType = QLNC_BEST_OPT_TYPE_INDEX_SCAN;
                        sNode->mBestCost = (qlncCostCommon*)(&(((qlncBaseTableNode*)sNode)->mIndexScanCostArr[i]));

                        STL_TRY( qlncCBOptHashInstantForPreorderedGroupBy(
                                     aParamInfo,
                                     aNode,
                                     aEnv )
                                 == STL_SUCCESS );

                        STL_THROW( RAMP_FINISH );
                    }
                }
            }
        }
        else if( sNode->mBestOptType == QLNC_BEST_OPT_TYPE_INDEX_SCAN )
        {
            /* case 2: Index Scan인 경우 */
            /**
             * @todo 현재는 해당 인덱스가 Group By에 사용될 수 있는지 판단하고,
             *       향후 다른 인덱스를 탈 경우의 cost 계산하여 판단하도록 한다.
             */
            for( i = 0; i < ((qlncBaseTableNode*)sNode)->mAccPathHint->mPossibleIndexScanCount; i++ )
            {
                sIndexStat =
                    &(((qlncBaseTableNode*)sNode)->mIndexStatArr[((qlncBaseTableNode*)sNode)->mAccPathHint->mPossibleIndexScanInfoArr[i].mOffset]);

                if( ellGetIndexID( sIndexStat->mIndexHandle ) ==
                    ellGetIndexID( ((qlncIndexScanCost*)(sNode->mBestCost))->mIndexHandle ) )
                {
                    STL_TRY( qlncPostOptGBPNIsUsableIndexForGroupBy(
                                 aParamInfo,
                                 sHashInstant->mKeyColList,
                                 sNode->mNodeID,
                                 sIndexStat,
                                 &sIsUsable,
                                 aEnv )
                             == STL_SUCCESS );

                    if( sIsUsable == STL_TRUE )
                    {
                        STL_TRY( qlncCBOptHashInstantForPreorderedGroupBy(
                                     aParamInfo,
                                     aNode,
                                     aEnv )
                                 == STL_SUCCESS );

                        STL_THROW( RAMP_FINISH );
                    }

                    break;
                }
            }

            STL_THROW( RAMP_FINISH );
        }
        else
        {
            /* Rowid Scan이나 Index Combine, In Key Range인 경우 처리 불가 */
            STL_THROW( RAMP_FINISH );
        }
    }
    else
    {
        /**
         * @todo 향후 SubTable이나 Sort Instant를 선택하여 반환되는 경우에 대해 구현해야 함
         */
        STL_DASSERT( (sNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE) ||
                     (sNode->mNodeType == QLNC_NODE_TYPE_SORT_INSTANT) );
    }


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Group By를 포함하는 Distinct에 대하여 Group By Pre-ordered Node을 수행한다.
 * @param[in]   aParamInfo  Parameter Info
 * @param[in]   aNode       Current Candidate Node
 * @param[in]   aEnv        Environment
 */
stlStatus qlncPostOptGBPNDistinctWithGroupHashInstant( qlncParamInfo    * aParamInfo,
                                                       qlncNodeCommon   * aNode,
                                                       qllEnv           * aEnv )
{
    stlInt32              i;
    stlInt32              sGroupKeyCount;
    stlInt32              sNonGroupKeyCount;
    qlncExprCommon     ** sGroupKeyArr      = NULL;
    qlncHashInstantNode * sHashInstant      = NULL;
    qlncRefExprItem     * sRefExprItem      = NULL;
    qlncExprCommon      * sExpr             = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode != NULL, QLL_ERROR_STACK(aEnv) );


    /* Hash Instant Node 설정 */
    sHashInstant = (qlncHashInstantNode*)aNode;

    /* Group Key와 Non Group Key로 분리한다. */
    sGroupKeyCount = 0;
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprCommon* ) * sHashInstant->mKeyColList->mCount,
                                (void**) &sGroupKeyArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sNonGroupKeyCount = 0;

    sRefExprItem = sHashInstant->mKeyColList->mHead;
    while( sRefExprItem != NULL )
    {
        sExpr = sRefExprItem->mExpr;
        while( (sExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN) &&
               (((qlncExprInnerColumn*)sExpr)->mOrgExpr != NULL) )
        {
            sExpr = ((qlncExprInnerColumn*)sExpr)->mOrgExpr;
        }

        if( sExpr->mType == QLNC_EXPR_TYPE_COLUMN )
        {
            sGroupKeyArr[sGroupKeyCount] = sExpr;
            sGroupKeyCount++;
        }
        else if( ( sExpr->mType != QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT ) &&
                 ( sExpr->mType != QLNC_EXPR_TYPE_VALUE ) )
        {
            /* Column도 Constant도 아닌 경우 */
            sNonGroupKeyCount++;
        }

        sRefExprItem = sRefExprItem->mNext;
    }


    if( sNonGroupKeyCount == 0 )
    {
        if( sGroupKeyCount == 0 )
        {
            /* Constant 값만 key로 구성된 경우 */
            STL_TRY( qlncCBOptHashInstantForPreorderedGroupBy(
                         aParamInfo,
                         aNode,
                         aEnv )
                     == STL_SUCCESS );

            STL_THROW( RAMP_FINISH );
        }
        else
        {
            /* Aggregation이 없는 Key들로만 구성된 경우
             * (모든 Key는 하위 Group By의 Key에 반드시 포함된다.) */
            STL_TRY( qlncPostOptGBPNGroupHashInstant( aParamInfo,
                                                      aNode,
                                                      aEnv )
                     == STL_SUCCESS );
        }
    }
    else
    {
        if( sGroupKeyCount == 0 )
        {
            /* Group Key가 Constant로만 구성된 경우 하위노드 상관 없이 Group으로 변환 가능 */
            sRefExprItem = ((qlncHashInstantNode*)(sHashInstant->mChildNode))->mKeyColList->mHead;
            while( sRefExprItem != NULL )
            {
                STL_DASSERT( sRefExprItem->mExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN );

                if( ((qlncExprInnerColumn*)(sRefExprItem->mExpr))->mOrgExpr->mType
                    == QLNC_EXPR_TYPE_COLUMN )
                {
                    break;
                }
                else if( ( ((qlncExprInnerColumn*)(sRefExprItem->mExpr))->mOrgExpr->mType
                           != QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT ) &&
                         ( ((qlncExprInnerColumn*)(sRefExprItem->mExpr))->mOrgExpr->mType
                           != QLNC_EXPR_TYPE_VALUE ) )
                {
                    /* Column도 Constant도 아니므로 불가 */
                    STL_THROW( RAMP_FINISH );
                }

                sRefExprItem = sRefExprItem->mNext;
            }

            if( sRefExprItem == NULL )
            {
                STL_TRY( qlncCBOptHashInstantForPreorderedGroupBy(
                             aParamInfo,
                             aNode,
                             aEnv )
                         == STL_SUCCESS );
            }

            STL_THROW( RAMP_FINISH );
        }
        else
        {
            /* 모든 Non Aggregation Key가 Group By의 Key 모두를 포함하고 있어야 한다. */
            sRefExprItem = ((qlncHashInstantNode*)(sHashInstant->mChildNode))->mKeyColList->mHead;
            while( sRefExprItem != NULL )
            {
                STL_DASSERT( sRefExprItem->mExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN );
                sExpr = ((qlncExprInnerColumn*)(sRefExprItem->mExpr))->mOrgExpr;

                if( sExpr->mType == QLNC_EXPR_TYPE_COLUMN )
                {
                    for( i = 0; i < sGroupKeyCount; i++ )
                    {
                        if( ( ((qlncExprColumn*)sExpr)->mNode->mNodeID
                              == ((qlncExprColumn*)sGroupKeyArr[i])->mNode->mNodeID ) &&
                            ( ((qlncExprColumn*)sExpr)->mColumnID
                              == (((qlncExprColumn*)sGroupKeyArr[i])->mColumnID) ) )
                        {
                            break;
                        }
                    }

                    /* Group Key에 해당하는 Column이 존재하지 않는 경우 */
                    if( i == sGroupKeyCount )
                    {
                        STL_THROW( RAMP_FINISH );
                    }
                }
                else
                {
                    STL_DASSERT( ( sExpr->mType == QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT ) ||
                                 ( sExpr->mType == QLNC_EXPR_TYPE_VALUE ) ||
                                 ( sExpr->mType == QLNC_EXPR_TYPE_FUNCTION ) );
                }

                sRefExprItem = sRefExprItem->mNext;
            }

            /* Group Key에 해당하는 Column이 모두 존재하는 경우 */
            STL_TRY( qlncCBOptHashInstantForPreorderedGroupBy(
                         aParamInfo,
                         aNode,
                         aEnv )
                     == STL_SUCCESS );

            STL_THROW( RAMP_FINISH );
        }
    }


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 주어진 노드로부터 사용가능한 preordered node를 찾는다.
 * @param[in]   aParamInfo  Parameter Info
 * @param[in]   aNode       Candidate Node
 * @param[out]  aUsableNode Usable Candidate Node
 * @param[in]   aEnv        Environment
 */
stlStatus qlncPostOptGBPNFindUsablePreorderedNode( qlncParamInfo    * aParamInfo,
                                                   qlncNodeCommon   * aNode,
                                                   qlncNodeCommon  ** aUsableNode,
                                                   qllEnv           * aEnv )
{
    qlncNodeCommon  * sNode         = NULL;
    qlncNodeCommon  * sUsableNode   = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aUsableNode != NULL, QLL_ERROR_STACK(aEnv) );


    sNode = aNode;
    sUsableNode = NULL;
    while( STL_TRUE )
    {
        switch( sNode->mNodeType )
        {
            case QLNC_NODE_TYPE_BASE_TABLE:
                /* Index가 없는 경우 처리 불가 */
                STL_TRY_THROW( ((qlncBaseTableNode*)sNode)->mAccPathHint->mPossibleIndexScanCount > 0,
                               RAMP_FINISH );

                sUsableNode = sNode;
                STL_THROW( RAMP_FINISH );

                break;

            case QLNC_NODE_TYPE_SUB_TABLE:
                /**
                 * @todo target의 위치를 이용하여 판단해야 함.
                 */
                STL_THROW( RAMP_FINISH );
                break;

            case QLNC_NODE_TYPE_JOIN_TABLE:
                /* Named Column List가 존재하는 경우 불가 */
                STL_TRY_THROW( (((qlncJoinTableNode*)sNode)->mNamedColumnList == NULL) ||
                               (((qlncJoinTableNode*)sNode)->mNamedColumnList->mCount > 0),
                               RAMP_FINISH );

                switch( sNode->mBestOptType )
                {
                    case QLNC_BEST_OPT_TYPE_INNER_JOIN:
                        sNode = (qlncNodeCommon*)(((qlncJoinTableNode*)sNode)->mBestInnerJoinTable);
                        break;
                    case QLNC_BEST_OPT_TYPE_OUTER_JOIN:
                        sNode = (qlncNodeCommon*)(((qlncJoinTableNode*)sNode)->mBestOuterJoinTable);
                        break;
                    case QLNC_BEST_OPT_TYPE_SEMI_JOIN:
                        sNode = (qlncNodeCommon*)(((qlncJoinTableNode*)sNode)->mBestSemiJoinTable);
                        break;
                    case QLNC_BEST_OPT_TYPE_ANTI_SEMI_JOIN:
                        sNode = (qlncNodeCommon*)(((qlncJoinTableNode*)sNode)->mBestAntiSemiJoinTable);
                        break;
                    default:
                        STL_DASSERT( 0 );
                        STL_THROW( RAMP_FINISH );
                        break;
                }
                break;

            case QLNC_NODE_TYPE_INNER_JOIN_TABLE:
            case QLNC_NODE_TYPE_SEMI_JOIN_TABLE:
            case QLNC_NODE_TYPE_ANTI_SEMI_JOIN_TABLE:
                /**
                 * @todo Join Type은 Nested Loops 또는 Hash이어야 한다.
                 *       향후 Merge에 대해서 Group 여부에 대한 처리를 판단할 수 있도록 함
                 */
                if( sNode->mBestOptType == QLNC_BEST_OPT_TYPE_NESTED_LOOPS )
                {
                    /* Inverted가 아니어야 한다. */
                    STL_TRY_THROW( ((qlncNestedJoinCost*)(sNode->mBestCost))->mIsInverted == STL_FALSE,
                                   RAMP_FINISH );

                    sNode = ((qlncNestedJoinCost*)(sNode->mBestCost))->mLeftNode;
                }
                else if( sNode->mBestOptType == QLNC_BEST_OPT_TYPE_MERGE )
                {
                    /* Index를 사용하는 경우에 한해서만 허용 */
                    if( ((qlncMergeJoinCost*)(sNode->mBestCost))->mLeftNode->mBestOptType
                        == QLNC_BEST_OPT_TYPE_INDEX_SCAN )
                    {
                        sNode = ((qlncMergeJoinCost*)(sNode->mBestCost))->mLeftNode;
                    }
                    else
                    {
                        STL_THROW( RAMP_FINISH );
                    }
                }
                else if( sNode->mBestOptType == QLNC_BEST_OPT_TYPE_HASH )
                {
                    /* Inverted가 아니어야 한다. */
                    STL_TRY_THROW( ((qlncHashJoinCost*)(sNode->mBestCost))->mIsInverted == STL_FALSE,
                                   RAMP_FINISH );

                    sNode = ((qlncHashJoinCost*)(sNode->mBestCost))->mLeftNode;
                }
                else
                {
                    STL_THROW( RAMP_FINISH );
                }
                break;

            case QLNC_NODE_TYPE_OUTER_JOIN_TABLE:
                /**
                 * @todo Join Type은 Nested Loops 또는 Hash이어야 한다.
                 *       향후 Merge에 대해서 Group 여부에 대한 처리를 판단할 수 있도록 함
                 */
                if( ((qlncOuterJoinTableNode*)sNode)->mJoinDirect == QLNC_JOIN_DIRECT_LEFT )
                {
                    if( sNode->mBestOptType == QLNC_BEST_OPT_TYPE_NESTED_LOOPS )
                    {
                        sNode = ((qlncNestedJoinCost*)(sNode->mBestCost))->mLeftNode;
                    }
                    else if( sNode->mBestOptType == QLNC_BEST_OPT_TYPE_MERGE )
                    {
                        /* Index를 사용하는 경우에 한해서만 허용 */
                        if( ((qlncMergeJoinCost*)(sNode->mBestCost))->mLeftNode->mBestOptType
                            == QLNC_BEST_OPT_TYPE_INDEX_SCAN )
                        {
                            sNode = ((qlncMergeJoinCost*)(sNode->mBestCost))->mLeftNode;
                        }
                        else
                        {
                            STL_THROW( RAMP_FINISH );
                        }
                    }
                    else if( sNode->mBestOptType == QLNC_BEST_OPT_TYPE_HASH )
                    {
                        sNode = ((qlncHashJoinCost*)(sNode->mBestCost))->mLeftNode;
                    }
                    else
                    {
                        STL_THROW( RAMP_FINISH );
                    }
                }
                else if( ((qlncOuterJoinTableNode*)sNode)->mJoinDirect == QLNC_JOIN_DIRECT_RIGHT )
                {
                    if( sNode->mBestOptType == QLNC_BEST_OPT_TYPE_NESTED_LOOPS )
                    {
                        sNode = ((qlncNestedJoinCost*)(sNode->mBestCost))->mRightNode;
                    }
                    else if( sNode->mBestOptType == QLNC_BEST_OPT_TYPE_MERGE )
                    {
                        /* Index를 사용하는 경우에 한해서만 허용 */
                        if( ((qlncMergeJoinCost*)(sNode->mBestCost))->mRightNode->mBestOptType
                            == QLNC_BEST_OPT_TYPE_INDEX_SCAN )
                        {
                            sNode = ((qlncMergeJoinCost*)(sNode->mBestCost))->mRightNode;
                        }
                        else
                        {
                            STL_THROW( RAMP_FINISH );
                        }
                    }
                    else if( sNode->mBestOptType == QLNC_BEST_OPT_TYPE_HASH )
                    {
                        sNode = ((qlncHashJoinCost*)(sNode->mBestCost))->mRightNode;
                    }
                    else
                    {
                        STL_THROW( RAMP_FINISH );
                    }
                }
                else
                {
                    STL_DASSERT( ((qlncOuterJoinTableNode*)sNode)->mJoinDirect == QLNC_JOIN_DIRECT_FULL );
                    STL_THROW( RAMP_FINISH );
                }
                break;

            case QLNC_NODE_TYPE_HASH_INSTANT:
            default:
                STL_DASSERT( 0 );
                STL_THROW( RAMP_FINISH );
                break;
        }
    }


    STL_RAMP( RAMP_FINISH );


    /* Output 설정 */
    *aUsableNode = sUsableNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Index가 Group By에 사용될 수 있는지 여부를 판단한다.
 * @param[in]   aParamInfo  Parameter Info
 * @param[in]   aKeyColList Group Key Column List
 * @param[in]   aNodeID     Index에 대한 Node ID
 * @param[in]   aIndexStat  Index Statistic
 * @param[out]  aIsUsable   사용 가능 여부
 * @param[in]   aEnv        Environment
 */
stlStatus qlncPostOptGBPNIsUsableIndexForGroupBy( qlncParamInfo     * aParamInfo,
                                                  qlncRefExprList   * aKeyColList,
                                                  stlInt32            aNodeID,
                                                  qlncIndexStat     * aIndexStat,
                                                  stlBool           * aIsUsable,
                                                  qllEnv            * aEnv )
{
    stlInt32          i;
    stlInt32          sGroupKeyCount;
    stlBool           sIsUsable;
    qlncRefExprItem * sRefExprItem  = NULL;
    qlncExprCommon  * sExpr         = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aKeyColList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexStat != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIsUsable != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * 가능 조건
     * 1. Constant값을 제외한 Group Key 개수 < Index Key 개수
     *    Group Key가 Index Key의 첫번째부터 순서대로 일치해야 한다.
     * 2. Constant값을 제외한 Group Key 개수 = Index Key 개수
     *    Group Key와 Index Key가 순서에 상관없이 모두 1:1 대응되어야 한다.
     * 3. Constant값을 제외한 Group Key 개수 > Index Key 개수
     *    Index를 이용하여 Group By를 할 수 없음
     * # Group Key는 Column이어야 한다. (Constant인 경우 제외)
     */

    sIsUsable = STL_FALSE;
    sGroupKeyCount = 0;

    /* Group Key Count 개수 파악 */
    sRefExprItem = aKeyColList->mHead;
    while( sRefExprItem != NULL )
    {
        STL_DASSERT( sRefExprItem->mExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN );
        sExpr = ((qlncExprInnerColumn*)(sRefExprItem->mExpr))->mOrgExpr;

        if( sExpr->mType == QLNC_EXPR_TYPE_COLUMN )
        {
           sGroupKeyCount++;
        }
        else if( (sExpr->mType != QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT) &&
                 (sExpr->mType != QLNC_EXPR_TYPE_VALUE) )
        {
            /* Column도 Constant도 아니므로 불가 */
            STL_THROW( RAMP_FINISH );
        }

        sRefExprItem = sRefExprItem->mNext;
    }

    for( i = 0; i < aIndexStat->mIndexKeyCount; i++ )
    {
        sRefExprItem = aKeyColList->mHead;
        while( sRefExprItem != NULL )
        {
            STL_DASSERT( sRefExprItem->mExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN );
            sExpr = ((qlncExprInnerColumn*)(sRefExprItem->mExpr))->mOrgExpr;

            if( sExpr->mType == QLNC_EXPR_TYPE_COLUMN )
            {
                if( ( ((qlncExprColumn*)sExpr)->mNode->mNodeID == aNodeID ) &&
                    ( ((qlncExprColumn*)sExpr)->mColumnID
                      == ellGetColumnID( &(aIndexStat->mIndexKeyDesc[i].mKeyColumnHandle) ) ) )
                {
                    break;
                }
            }
            else
            {
                STL_DASSERT( (sExpr->mType == QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT) ||
                             (sExpr->mType == QLNC_EXPR_TYPE_VALUE) );
            }

            sRefExprItem = sRefExprItem->mNext;
        }

        if( sRefExprItem == NULL )
        {
            /* Index Key에 해당하는 Column이 Group Key에 없는 경우 */
            break;
        }
    }

    /* Group Key가 모두 존재하지 않는 경우 불가 */
    STL_TRY_THROW( i == sGroupKeyCount, RAMP_FINISH );

    sIsUsable = STL_TRUE;


    STL_RAMP( RAMP_FINISH );


    /* Output 설정 */
    *aIsUsable = sIsUsable;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlnc */
