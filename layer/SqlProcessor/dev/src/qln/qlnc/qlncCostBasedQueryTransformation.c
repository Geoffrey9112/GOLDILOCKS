/*******************************************************************************
 * qlncCostBasedQueryTransformation.c
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

/**
 * @file qlncCostBasedQueryTransformation.c
 * @brief SQL Processor Layer SELECT statement Cost Based Query Transformation
 */

#include <qll.h>
#include <qlDef.h>



/**
 * @addtogroup qlnc
 * @{
 */


/**
 * @brief Cost Based Query Trasnformation을 수행한다.
 * @param[in]       aParamInfo  Parameter Info
 * @param[in,out]   aNode       Candidate Node
 * @param[in]       aEnv        Environment
 */
stlStatus qlncCostBasedQueryTransformation( qlncParamInfo   * aParamInfo,
                                            qlncNodeCommon  * aNode,
                                            qllEnv          * aEnv )
{
//    qlncNodeCommon  * sOutNode  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode != NULL, QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Cost Based Query Transformation (CBQT)
     ****************************************************************/

    /**
     * - complex view, sub-queries or union (all) queries에 대해 수행
     * - 각 변환에 대하여 Cost를 측정하여 Best를 선택해야 한다.
     * 1. Materialized View Rewrite (MVR)
     * 2. OR-Expansion (ORE)
     *    : OR를 Union All로 변환
     * 3. Star Transformation (ST)
     * 4. Join Predicate Push-down (JPPD)
     * 5. Filter Push-down (FPD)
     */

    /* Filter Pull Up on SubQuery Filter
     * (Join등 Table 노드의 Filter에 있는 Outer Table 참조 Filter 들을 SubQuery까지
     *  올리는 것) */

    /* N형, J형, A형, AJ형 등의 SubQuery에 대한 Unnest */
#if 0
    STL_TRY( qlncSubQueryUnnest( aParamInfo,
                                 aNode,
                                 &sOutNode,
                                 aEnv )
             == STL_SUCCESS );

    /* 최상위 노드는 SubQuery에 의한 변형이 없다. */
    STL_DASSERT( aNode == sOutNode );
#endif

    /* Outer Table의 Column을 참조하는 In SubQuery Expression을 변경한다. */
#if 0
    STL_TRY( qlncRewriteInRelationSubQuery( aParamInfo,
                                            aNode,
                                            aEnv )
             == STL_SUCCESS );
#endif


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

#if 0


/**
 * @brief In Relation SubQuery에 대해 Rewrite를 수행한다.
 * @param[in]   aParamInfo  Parameter Info
 * @param[in]   aNode       Candidate Node
 * @param[in]   aEnv        Environment
 */
stlStatus qlncRewriteInRelationSubQuery( qlncParamInfo  * aParamInfo,
                                         qlncNodeCommon * aNode,
                                         qllEnv         * aEnv )
{
    stlInt32          i;
    qlncNodeCommon  * sNode         = NULL;
    qlncRefExprItem * sRefExprItem  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode != NULL, QLL_ERROR_STACK(aEnv) );


    sNode = aNode;

    STL_RAMP( RAMP_RETRY );

    switch( sNode->mNodeType )
    {
        case QLNC_NODE_TYPE_QUERY_SET:
            sNode = ((qlncQuerySet*)sNode)->mChildNode;
            STL_THROW( RAMP_RETRY );
            break;

        case QLNC_NODE_TYPE_SET_OP:
            for( i = 0; i < ((qlncSetOP*)sNode)->mChildCount; i++ )
            {
                STL_TRY( qlncRewriteInRelationSubQuery( aParamInfo,
                                                        ((qlncSetOP*)sNode)->mChildNodeArr[i],
                                                        aEnv )
                         == STL_SUCCESS );
            }
            break;

        case QLNC_NODE_TYPE_QUERY_SPEC:
            for( i = 0; i < ((qlncQuerySpec*)sNode)->mTargetCount; i++ )
            {
                STL_TRY( qlncRewriteInRelationSubQueryNonFilter( aParamInfo,
                                                                 ((qlncQuerySpec*)sNode)->mTargetArr[i].mExpr,
                                                                 &(((qlncQuerySpec*)sNode)->mTargetArr[i].mExpr),
                                                                 aEnv )
                         == STL_SUCCESS );
            }

            STL_TRY( qlncRewriteInRelationSubQuery( aParamInfo,
                                                    ((qlncQuerySpec*)sNode)->mChildNode,
                                                    aEnv )
                     == STL_SUCCESS );
            break;

        case QLNC_NODE_TYPE_BASE_TABLE:
            STL_TRY( qlncRewriteInRelationSubQueryFilter( aParamInfo,
                                                          ((qlncBaseTableNode*)sNode)->mFilter,
                                                          aEnv )
                     == STL_SUCCESS );
            break;

        case QLNC_NODE_TYPE_SUB_TABLE:
            STL_TRY( qlncRewriteInRelationSubQueryFilter( aParamInfo,
                                                          ((qlncSubTableNode*)sNode)->mFilter,
                                                          aEnv )
                     == STL_SUCCESS );

            STL_TRY( qlncRewriteInRelationSubQuery( aParamInfo,
                                                    ((qlncSubTableNode*)sNode)->mQueryNode,
                                                    aEnv )
                     == STL_SUCCESS );
            break;

        case QLNC_NODE_TYPE_JOIN_TABLE:
            STL_TRY( qlncRewriteInRelationSubQueryFilter( aParamInfo,
                                                          ((qlncJoinTableNode*)sNode)->mJoinCond,
                                                          aEnv )
                     == STL_SUCCESS );
            STL_TRY( qlncRewriteInRelationSubQueryFilter( aParamInfo,
                                                          ((qlncJoinTableNode*)sNode)->mFilter,
                                                          aEnv )
                     == STL_SUCCESS );

            for( i = 0; i < ((qlncJoinTableNode*)sNode)->mNodeArray.mCurCnt; i++ )
            {
                STL_TRY( qlncRewriteInRelationSubQuery( aParamInfo,
                                                        ((qlncJoinTableNode*)sNode)->mNodeArray.mNodeArr[i],
                                                        aEnv )
                         == STL_SUCCESS );
            }
            break;

        case QLNC_NODE_TYPE_FLAT_INSTANT:
            STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
            break;

        case QLNC_NODE_TYPE_SORT_INSTANT:
            STL_DASSERT( ((qlncSortInstantNode*)sNode)->mKeyColList != NULL );
            sRefExprItem = ((qlncSortInstantNode*)sNode)->mKeyColList->mHead;
            while( sRefExprItem != NULL )
            {
                STL_TRY( qlncRewriteInRelationSubQueryNonFilter(
                             aParamInfo,
                             ((qlncExprInnerColumn*)sRefExprItem->mExpr)->mOrgExpr,
                             &(((qlncExprInnerColumn*)sRefExprItem->mExpr)->mOrgExpr),
                             aEnv )
                         == STL_SUCCESS );

                sRefExprItem = sRefExprItem->mNext;
            }

            if( ((qlncSortInstantNode*)sNode)->mRecColList != NULL )
            {
                sRefExprItem = ((qlncSortInstantNode*)sNode)->mKeyColList->mHead;
                while( sRefExprItem != NULL )
                {
                    STL_TRY( qlncRewriteInRelationSubQueryNonFilter(
                                 aParamInfo,
                                 ((qlncExprInnerColumn*)sRefExprItem->mExpr)->mOrgExpr,
                                 &(((qlncExprInnerColumn*)sRefExprItem->mExpr)->mOrgExpr),
                                 aEnv )
                             == STL_SUCCESS );

                    sRefExprItem = sRefExprItem->mNext;
                }
            }

            STL_TRY( qlncRewriteInRelationSubQuery( aParamInfo,
                                                    ((qlncSortInstantNode*)sNode)->mChildNode,
                                                    aEnv )
                     == STL_SUCCESS );
            break;

        case QLNC_NODE_TYPE_HASH_INSTANT:
            STL_DASSERT( ((qlncHashInstantNode*)sNode)->mKeyColList != NULL );
            sRefExprItem = ((qlncHashInstantNode*)sNode)->mKeyColList->mHead;
            while( sRefExprItem != NULL )
            {
                STL_TRY( qlncRewriteInRelationSubQueryNonFilter(
                             aParamInfo,
                             ((qlncExprInnerColumn*)sRefExprItem->mExpr)->mOrgExpr,
                             &(((qlncExprInnerColumn*)sRefExprItem->mExpr)->mOrgExpr),
                             aEnv )
                         == STL_SUCCESS );

                sRefExprItem = sRefExprItem->mNext;
            }

            if( ((qlncHashInstantNode*)sNode)->mRecColList != NULL )
            {
                sRefExprItem = ((qlncHashInstantNode*)sNode)->mKeyColList->mHead;
                while( sRefExprItem != NULL )
                {
                    STL_TRY( qlncRewriteInRelationSubQueryNonFilter(
                                 aParamInfo,
                                 ((qlncExprInnerColumn*)sRefExprItem->mExpr)->mOrgExpr,
                                 &(((qlncExprInnerColumn*)sRefExprItem->mExpr)->mOrgExpr),
                                 aEnv )
                             == STL_SUCCESS );

                    sRefExprItem = sRefExprItem->mNext;
                }
            }

            STL_TRY( qlncRewriteInRelationSubQuery( aParamInfo,
                                                    ((qlncHashInstantNode*)sNode)->mChildNode,
                                                    aEnv )
                     == STL_SUCCESS );
            break;

        default:
            STL_DASSERT( 0 );
            break;
    }


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      "qlncCBQTRewriteInRelationSubQuery()" );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Filter에 존재하는 In Relation SubQuery에 대하여 Rewrite를 수행한다.
 * @param[in]       aParamInfo  Parameter Info
 * @param[in,out]   aAndFilter  And Filter
 * @param[in]       aEnv        Environment
 */
stlStatus qlncRewriteInRelationSubQueryFilter( qlncParamInfo    * aParamInfo,
                                               qlncAndFilter    * aAndFilter,
                                               qllEnv           * aEnv )
{
    stlInt32      i, j;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );


    /* And Filter가 없는 경우 종료 */
    STL_TRY_THROW( aAndFilter != NULL, RAMP_FINISH );


    for( i = 0; i < aAndFilter->mOrCount; i++ )
    {
        /* SubQuery Expression이 없는 Or Filter는 Skip */
        if( aAndFilter->mOrFilters[i]->mSubQueryExprList == NULL )
        {
            continue;
        }

        for( j = 0; j < aAndFilter->mOrFilters[i]->mFilterCount; j++ )
        {
            if( aAndFilter->mOrFilters[i]->mFilters[j]->mType == QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER )
            {
                qlncConstBooleanFilter  * sConstBooleanFilter   = NULL;

                sConstBooleanFilter = (qlncConstBooleanFilter*)(aAndFilter->mOrFilters[i]->mFilters[j]);
                STL_TRY( qlncRewriteInRelationSubQueryNonFilter( aParamInfo,
                                                                 sConstBooleanFilter->mExpr,
                                                                 &(sConstBooleanFilter->mExpr),
                                                                 aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                qlncBooleanFilter   * sBooleanFilter    = NULL;

                sBooleanFilter = (qlncBooleanFilter*)(aAndFilter->mOrFilters[i]->mFilters[j]);
                if( sBooleanFilter->mExpr->mType == QLNC_EXPR_TYPE_AND_FILTER )
                {
                    STL_TRY( qlncRewriteInRelationSubQueryFilter( aParamInfo,
                                                                  (qlncAndFilter*)(sBooleanFilter->mExpr),
                                                                  aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    STL_TRY( qlncRewriteInRelationSubQueryNonFilter( aParamInfo,
                                                                     sBooleanFilter->mExpr,
                                                                     &(sBooleanFilter->mExpr),
                                                                     aEnv )
                             == STL_SUCCESS );

                    switch( sBooleanFilter->mFuncID )
                    {
                        case KNL_BUILTIN_FUNC_IN:
                        case KNL_BUILTIN_FUNC_NOT_IN:
                        case KNL_BUILTIN_FUNC_EQUAL_ANY:
                        case KNL_BUILTIN_FUNC_NOT_EQUAL_ANY:
                        case KNL_BUILTIN_FUNC_LESS_THAN_ANY:
                        case KNL_BUILTIN_FUNC_LESS_THAN_EQUAL_ANY:
                        case KNL_BUILTIN_FUNC_GREATER_THAN_ANY:
                        case KNL_BUILTIN_FUNC_GREATER_THAN_EQUAL_ANY:
                        case KNL_BUILTIN_FUNC_EQUAL_ALL:
                        case KNL_BUILTIN_FUNC_NOT_EQUAL_ALL:
                        case KNL_BUILTIN_FUNC_LESS_THAN_ALL:
                        case KNL_BUILTIN_FUNC_LESS_THAN_EQUAL_ALL:
                        case KNL_BUILTIN_FUNC_GREATER_THAN_ALL:
                        case KNL_BUILTIN_FUNC_GREATER_THAN_EQUAL_ALL:
                        case KNL_BUILTIN_FUNC_EXISTS:
                        case KNL_BUILTIN_FUNC_NOT_EXISTS:
                            /* Function ID가 변경됨 */
                            if( sBooleanFilter->mExpr->mType == QLNC_EXPR_TYPE_LIST_FUNCTION )
                            {
                                sBooleanFilter->mFuncID =
                                    ((qlncExprListFunc*)(sBooleanFilter->mExpr))->mFuncId;
                            }
                            else
                            {
                                STL_DASSERT( sBooleanFilter->mExpr->mType != QLNC_EXPR_TYPE_FUNCTION );
                                
                                sBooleanFilter->mFuncID = KNL_BUILTIN_FUNC_INVALID;
                            }
                            sBooleanFilter->mLeftRefNodeList = NULL;
                            sBooleanFilter->mRightRefNodeList = NULL;
                            sBooleanFilter->mPossibleJoinCond = STL_FALSE;
                            sBooleanFilter->mIsPhysicalFilter = STL_FALSE;
                            break;
                        default:
                            /* Do Nothing */
                            break;
                    }
                }
            }
        }
    }


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Non Filter에 존재하는 In Relation SubQuery에 대하여 Rewrite를 수행한다.
 * @param[in]   aParamInfo      Parameter Info
 * @param[in]   aNonFilterExpr  Non Filter Expression
 * @param[out]  aOutExpr        Output Expression
 * @param[in]   aEnv            Environment
 */
stlStatus qlncRewriteInRelationSubQueryNonFilter( qlncParamInfo     * aParamInfo,
                                                  qlncExprCommon    * aNonFilterExpr,
                                                  qlncExprCommon   ** aOutExpr,
                                                  qllEnv            * aEnv )
{
    stlInt32          i;
    qlncExprCommon  * sExpr     = NULL;
    qlncExprCommon  * sOutExpr  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNonFilterExpr != NULL, QLL_ERROR_STACK(aEnv) );


    sExpr = aNonFilterExpr;

    switch( sExpr->mType )
    {
        case QLNC_EXPR_TYPE_VALUE:
        case QLNC_EXPR_TYPE_BIND_PARAM:
        case QLNC_EXPR_TYPE_COLUMN:
            /* 더 이상 진행할 필요 없음 */
            sOutExpr = sExpr;
            break;

        case QLNC_EXPR_TYPE_FUNCTION:
            for( i = 0; i < ((qlncExprFunction*)sExpr)->mArgCount; i++ )
            {
                STL_TRY( qlncRewriteInRelationSubQueryNonFilter( aParamInfo,
                                                                 ((qlncExprFunction*)sExpr)->mArgs[i],
                                                                 &(((qlncExprFunction*)sExpr)->mArgs[i]),
                                                                 aEnv )
                         == STL_SUCCESS );
            }
            sOutExpr = sExpr;
            break;

        case QLNC_EXPR_TYPE_CAST:
            for( i = 0; i < DTL_CAST_INPUT_ARG_CNT; i++ )
            {
                STL_TRY( qlncRewriteInRelationSubQueryNonFilter( aParamInfo,
                                                                 ((qlncExprTypeCast*)sExpr)->mArgs[i],
                                                                 &(((qlncExprTypeCast*)sExpr)->mArgs[i]),
                                                                 aEnv )
                         == STL_SUCCESS );
            }
            sOutExpr = sExpr;
            break;

        case QLNC_EXPR_TYPE_PSEUDO_COLUMN:
        case QLNC_EXPR_TYPE_PSEUDO_ARGUMENT:
        case QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT:
        case QLNC_EXPR_TYPE_REFERENCE:
            /* 더 이상 진행할 필요 없음 */
            sOutExpr = sExpr;
            break;

        case QLNC_EXPR_TYPE_SUB_QUERY:
            STL_TRY( qlncRewriteInRelationSubQuery( aParamInfo,
                                                    ((qlncExprSubQuery *)sExpr)->mNode,
                                                    aEnv )
                     == STL_SUCCESS );
            sOutExpr = sExpr;
            break;

        case QLNC_EXPR_TYPE_INNER_COLUMN:
            /* 더 이상 진행할 필요 없음 */
            sOutExpr = sExpr;
            break;

        case QLNC_EXPR_TYPE_OUTER_COLUMN:
            /* 더 이상 진행할 필요 없음 */
            sOutExpr = sExpr;
            break;

        case QLNC_EXPR_TYPE_ROWID_COLUMN:
            /* 더 이상 진행할 필요 없음 */
            sOutExpr = sExpr;
            break;

        case QLNC_EXPR_TYPE_AGGREGATION:
            for( i = 0; i < ((qlncExprAggregation*)sExpr)->mArgCount; i++ )
            {
                STL_TRY( qlncRewriteInRelationSubQueryNonFilter( aParamInfo,
                                                                 ((qlncExprAggregation*)sExpr)->mArgs[i],
                                                                 &(((qlncExprAggregation*)sExpr)->mArgs[i]),
                                                                 aEnv )
                         == STL_SUCCESS );
            }

            if( ((qlncExprAggregation*)sExpr)->mFilter != NULL )
            {
                STL_TRY( qlncRewriteInRelationSubQueryFilter( aParamInfo,
                                                              (qlncAndFilter*)(((qlncExprAggregation*)sExpr)->mFilter),
                                                              aEnv )
                         == STL_SUCCESS );
            }
            sOutExpr = sExpr;
            break;

        case QLNC_EXPR_TYPE_CASE_EXPR:
            for( i = 0; i < ((qlncExprCaseExpr*)sExpr)->mCount; i++ )
            {
                STL_TRY( qlncRewriteInRelationSubQueryNonFilter( aParamInfo,
                                                                 ((qlncExprCaseExpr*)sExpr)->mWhenArr[i],
                                                                 &(((qlncExprCaseExpr*)sExpr)->mWhenArr[i]),
                                                                 aEnv )
                         == STL_SUCCESS );

                STL_TRY( qlncRewriteInRelationSubQueryNonFilter( aParamInfo,
                                                                 ((qlncExprCaseExpr*)sExpr)->mThenArr[i],
                                                                 &(((qlncExprCaseExpr*)sExpr)->mThenArr[i]),
                                                                 aEnv )
                         == STL_SUCCESS );
            }

            STL_TRY( qlncRewriteInRelationSubQueryNonFilter( aParamInfo,
                                                             ((qlncExprCaseExpr*)sExpr)->mDefResult,
                                                             &(((qlncExprCaseExpr*)sExpr)->mDefResult),
                                                             aEnv )
                     == STL_SUCCESS );
            sOutExpr = sExpr;
            break;

        case QLNC_EXPR_TYPE_LIST_FUNCTION:
            sOutExpr = sExpr;
            break;

        case QLNC_EXPR_TYPE_LIST_COLUMN:
        case QLNC_EXPR_TYPE_ROW_EXPR:
            /* 이미 In Function 아래에 있으므로 In Relation SubQuery가 나오지 않는다. */
            sOutExpr = sExpr;
            break;

        default:
            STL_DASSERT( 0 );
            break;
    }


    /**
     * Output 설정
     */

    *aOutExpr = sOutExpr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief In Relation SubQuery를 SubQuery로 Rewrite한다.
 * @param[in]   aParamInfo  Parameter Info
 * @param[in]   aExpr       Expression
 * @param[out]  aOutExpr    Output Expression
 * @param[in]   aEnv        Environment
 */
stlStatus qlncRewriteInRelationSubQueryCore( qlncParamInfo      * aParamInfo,
                                             qlncExprCommon     * aExpr,
                                             qlncExprCommon    ** aOutExpr,
                                             qllEnv             * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr->mType == QLNC_EXPR_TYPE_LIST_FUNCTION, QLL_ERROR_STACK(aEnv) );


    /**
     * List Function 별로 Rewrite 수행
     */
    
    switch( (((qlncExprListFunc*)aExpr)->mFuncId ) )
    {
        case KNL_BUILTIN_FUNC_IN :
        case KNL_BUILTIN_FUNC_NOT_IN :
            STL_TRY( qlncRewriteInRelationSubQueryIn( aParamInfo,
                                                      aExpr,
                                                      aOutExpr,
                                                      aEnv )
                     == STL_SUCCESS );
            break;
            
        case KNL_BUILTIN_FUNC_EXISTS :
        case KNL_BUILTIN_FUNC_NOT_EXISTS :
            STL_TRY( qlncRewriteInRelationSubQueryExists( aParamInfo,
                                                          aExpr,
                                                          aOutExpr,
                                                          aEnv )
                     == STL_SUCCESS );
            break;
                
        case KNL_BUILTIN_FUNC_EQUAL_ANY :
        case KNL_BUILTIN_FUNC_NOT_EQUAL_ANY :
        case KNL_BUILTIN_FUNC_LESS_THAN_ANY :
        case KNL_BUILTIN_FUNC_LESS_THAN_EQUAL_ANY :
        case KNL_BUILTIN_FUNC_GREATER_THAN_ANY :
        case KNL_BUILTIN_FUNC_GREATER_THAN_EQUAL_ANY :
            STL_TRY( qlncRewriteInRelationSubQueryAny( aParamInfo,
                                                       aExpr,
                                                       aOutExpr,
                                                       aEnv )
                     == STL_SUCCESS );
            break;
                
        case KNL_BUILTIN_FUNC_EQUAL_ALL :
        case KNL_BUILTIN_FUNC_NOT_EQUAL_ALL :
        case KNL_BUILTIN_FUNC_LESS_THAN_ALL :
        case KNL_BUILTIN_FUNC_LESS_THAN_EQUAL_ALL :
        case KNL_BUILTIN_FUNC_GREATER_THAN_ALL :
        case KNL_BUILTIN_FUNC_GREATER_THAN_EQUAL_ALL :
            STL_TRY( qlncRewriteInRelationSubQueryAll( aParamInfo,
                                                       aExpr,
                                                       aOutExpr,
                                                       aEnv )
                     == STL_SUCCESS );
            break;

        case KNL_BUILTIN_FUNC_IS_EQUAL_ROW :
        case KNL_BUILTIN_FUNC_IS_NOT_EQUAL_ROW :
        case KNL_BUILTIN_FUNC_IS_LESS_THAN_ROW :
        case KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL_ROW :
        case KNL_BUILTIN_FUNC_IS_GREATER_THAN_ROW :
        case KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL_ROW :
            STL_TRY( qlncRewriteInRelationSubQueryRowComp( aParamInfo,
                                                           aExpr,
                                                           aOutExpr,
                                                           aEnv )
                     == STL_SUCCESS );
            break;
                
        default :
            STL_DASSERT( 0 );
            break;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief IN(SubQuery) / NOT IN(SubQuery) 를 SubQuery로 Rewrite한다.
 * @param[in]   aParamInfo  Parameter Info
 * @param[in]   aExpr       Expression
 * @param[out]  aOutExpr    Output Expression
 * @param[in]   aEnv        Environment
 */
stlStatus qlncRewriteInRelationSubQueryIn( qlncParamInfo      * aParamInfo,
                                           qlncExprCommon     * aExpr,
                                           qlncExprCommon    ** aOutExpr,
                                           qllEnv             * aEnv )
{
    stlInt32                     i              = 0;
    qlncExprSubQuery           * sSubQuery      = NULL;
    qlncExprRowExpr            * sRowExpr       = NULL;
    qlncExprFunction           * sFunction      = NULL;
    qlncExprInnerColumn        * sInnerColumn   = NULL;
    qlncExprValue              * sValue         = NULL;
    qlncExprConstExpr          * sConstExpr     = NULL;
    qlncSubTableNode           * sSubTableNode  = NULL;
    qlncExprCommon             * sFilter        = NULL;
    qlncAndFilter              * sAndFilter     = NULL;
    qlncExprRowExpr            * sTmpRowExpr    = NULL;
    qlncQuerySpec              * sQuerySpec     = NULL;
    qlncExprValue              * sFetchValue    = NULL;
    qlncBooleanFilter          * sBooleanFilter = NULL;
    qlncRefNodeItem            * sRefNodeItem   = NULL;
    qlncRefColumnItem          * sRefColumnItem = NULL;
    qlncConvertExprParamInfo     sConvertExprParamInfo;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr->mType == QLNC_EXPR_TYPE_LIST_FUNCTION, QLL_ERROR_STACK(aEnv) );


    /* Row Expression 설정 */
    STL_DASSERT( ((qlncExprListFunc*)aExpr)->mArgCount == 2  );
    sRowExpr = (qlncExprRowExpr*)(((qlncExprListCol*)(((qlncExprListFunc*)aExpr)->mArgs[1]))->mArgs[0]);

    /* Right SubQuery Expression 설정 */
    sSubQuery = (qlncExprSubQuery*)(((qlncExprRowExpr*)(((qlncExprListCol*)(((qlncExprListFunc*)aExpr)->mArgs[0]))->mArgs[0]))->mArgs[0]);


    /**
     * Sub Table Node 생성
     */

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncSubTableNode ),
                                (void**) &sSubTableNode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_INIT_NODE_COMMON( &(sSubTableNode->mNodeCommon),
                           *(aParamInfo->mGlobalNodeID),
                           QLNC_NODE_TYPE_SUB_TABLE );
    (*(aParamInfo->mGlobalNodeID))++;   /* Global ID 증가 */

    sSubTableNode->mRelationName = NULL;
    sSubTableNode->mQueryNode = sSubQuery->mNode;
    sSubTableNode->mJoinHint = NULL;
    sSubTableNode->mFilter = NULL;
    QLNC_INIT_LIST( &(sSubTableNode->mReadColumnList) );


    /**
     * Filter 생성
     */

    /* And Filter 생성 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncAndFilter ),
                                (void**) &sAndFilter,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_SET_EXPR_COMMON( &sAndFilter->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_AND_FILTER,
                          aExpr->mExprPhraseFlag,
                          aExpr->mIterationTime,
                          aExpr->mPosition,
                          aExpr->mColumnName,
                          DTL_TYPE_BOOLEAN );
                              
    sAndFilter->mOrCount = 1;

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncOrFilter* ),
                                (void**) &(sAndFilter->mOrFilters),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Or Filter 생성 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncOrFilter ),
                                (void**) &(sAndFilter->mOrFilters[0]),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_SET_EXPR_COMMON( &sAndFilter->mOrFilters[0]->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_OR_FILTER,
                          aExpr->mExprPhraseFlag,
                          aExpr->mIterationTime,
                          aExpr->mPosition,
                          aExpr->mColumnName,
                          DTL_TYPE_BOOLEAN );

    sAndFilter->mOrFilters[0]->mFilterCount = 1;

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncRefExprList ),
                                (void**) &(sAndFilter->mOrFilters[0]->mSubQueryExprList),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    QLNC_INIT_LIST( sAndFilter->mOrFilters[0]->mSubQueryExprList );

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprCommon* ),
                                (void**) &(sAndFilter->mOrFilters[0]->mFilters),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Boolean Filter 생성 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncBooleanFilter ),
                                (void**) &sBooleanFilter,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_SET_EXPR_COMMON( &sBooleanFilter->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_BOOLEAN_FILTER,
                          aExpr->mExprPhraseFlag,
                          aExpr->mIterationTime,
                          aExpr->mPosition,
                          aExpr->mColumnName,
                          DTL_TYPE_BOOLEAN );

    sBooleanFilter->mFuncID = KNL_BUILTIN_FUNC_INVALID;
    sBooleanFilter->mExpr = NULL;
    QLNC_INIT_LIST( &(sBooleanFilter->mRefNodeList) );
    sBooleanFilter->mLeftRefNodeList = NULL;
    sBooleanFilter->mRightRefNodeList = NULL;
    sBooleanFilter->mPossibleJoinCond = STL_FALSE;
    sBooleanFilter->mIsPhysicalFilter = STL_FALSE;
    sBooleanFilter->mSubQueryExprList = NULL;

    sAndFilter->mOrFilters[0]->mFilters[0] = (qlncExprCommon*)sBooleanFilter;

    sConvertExprParamInfo.mParamInfo = *aParamInfo;
    sConvertExprParamInfo.mTableMapArr = NULL;
    sConvertExprParamInfo.mOuterTableMapArr = NULL;
    sConvertExprParamInfo.mInitExpr = NULL;
    sConvertExprParamInfo.mCandExpr = NULL;
    sConvertExprParamInfo.mRefNodeList = &(sBooleanFilter->mRefNodeList);
    sConvertExprParamInfo.mExprPhraseFlag = 0;
    sConvertExprParamInfo.mSubQueryExprList = NULL;
    sConvertExprParamInfo.mExprSubQuery = NULL;
    
    /* Filter 생성 */
    if( sRowExpr->mArgCount == 1 )
    {
        /* Equal / Not Equal Function으로 전환 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncExprFunction ),
                                    (void**) &sFunction,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncExprCommon* ) * 2,
                                    (void**) &(sFunction->mArgs),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        if( ((qlncExprListFunc*)aExpr)->mFuncId == KNL_BUILTIN_FUNC_IN )
        {
            sFunction->mFuncId = KNL_BUILTIN_FUNC_IS_EQUAL;
        }
        else
        {
            sFunction->mFuncId = KNL_BUILTIN_FUNC_IS_NOT_EQUAL;
        }

        /* Sub Table Node에 대한 Inner Column 만들기 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncExprInnerColumn ),
                                    (void**) &sInnerColumn,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        QLNC_SET_EXPR_COMMON( &sInnerColumn->mCommon,
                              &QLL_CANDIDATE_MEM( aEnv ),
                              aEnv,
                              QLNC_EXPR_TYPE_INNER_COLUMN,
                              sSubQuery->mTargetArr[0].mExpr->mExprPhraseFlag,
                              sSubQuery->mTargetArr[0].mExpr->mIterationTime,
                              sSubQuery->mTargetArr[0].mExpr->mPosition,
                              sSubQuery->mTargetArr[0].mExpr->mColumnName,
                              sSubQuery->mTargetArr[0].mExpr->mDataType );

        sInnerColumn->mNode = (qlncNodeCommon*)sSubTableNode;
        sInnerColumn->mIdx = 0;
        sInnerColumn->mOrgExpr = NULL;

        /* Sub Table의 Read Column List에 등록 */
        STL_TRY( qlncAddColumnToRefColumnList( aParamInfo,
                                               (qlncExprCommon*)sInnerColumn,
                                               &(sSubTableNode->mReadColumnList),
                                               aEnv )
                 == STL_SUCCESS );

        /* Row Expression의 Arg[0]에서 Reference Node List를 찾아 등록 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncRefNodeList ),
                                    (void**) &(sBooleanFilter->mLeftRefNodeList),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        QLNC_INIT_LIST( sBooleanFilter->mLeftRefNodeList );

        sConvertExprParamInfo.mRefNodeList = sBooleanFilter->mLeftRefNodeList;
        STL_TRY( qlncFindAddRefColumnToRefNodeList( &sConvertExprParamInfo,
                                                    (qlncExprCommon*)(sRowExpr->mArgs[0]),
                                                    aEnv )
                 == STL_SUCCESS );
        sConvertExprParamInfo.mRefNodeList = &(sBooleanFilter->mRefNodeList);

        if( sBooleanFilter->mLeftRefNodeList->mCount == 0 )
        {
            sBooleanFilter->mLeftRefNodeList = NULL;
            sBooleanFilter->mRightRefNodeList = NULL;

            /* Boolean Filter의 Reference Node List에 등록 */
            STL_TRY( qlncAddRefColumnToRefNodeList( &sConvertExprParamInfo,
                                                    (qlncExprCommon*)sInnerColumn,
                                                    aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /* Inner Column을 Right Reference Node List에 등록 */
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncRefNodeList ),
                                        (void**) &(sBooleanFilter->mRightRefNodeList),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            QLNC_INIT_LIST( sBooleanFilter->mRightRefNodeList );

            sConvertExprParamInfo.mRefNodeList = sBooleanFilter->mRightRefNodeList;
            STL_TRY( qlncAddRefColumnToRefNodeList( &sConvertExprParamInfo,
                                                    (qlncExprCommon*)sInnerColumn,
                                                    aEnv )
                     == STL_SUCCESS );
            sConvertExprParamInfo.mRefNodeList = &(sBooleanFilter->mRefNodeList);

            /* Append Left Reference Node List */
            sRefNodeItem = sBooleanFilter->mLeftRefNodeList->mHead;
            while( sRefNodeItem != NULL )
            {
                sRefColumnItem = sRefNodeItem->mRefColumnList.mHead;
                while( sRefColumnItem != NULL )
                {
                    STL_TRY( qlncAddRefColumnToRefNodeList( &sConvertExprParamInfo,
                                                            sRefColumnItem->mExpr,
                                                            aEnv )
                             == STL_SUCCESS );

                    sRefColumnItem = sRefColumnItem->mNext;
                }

                sRefNodeItem = sRefNodeItem->mNext;
            }

            /* Append Right Reference Node List */
            sRefNodeItem = sBooleanFilter->mRightRefNodeList->mHead;
            while( sRefNodeItem != NULL )
            {
                sRefColumnItem = sRefNodeItem->mRefColumnList.mHead;
                while( sRefColumnItem != NULL )
                {
                    STL_TRY( qlncAddRefColumnToRefNodeList( &sConvertExprParamInfo,
                                                            sRefColumnItem->mExpr,
                                                            aEnv )
                             == STL_SUCCESS );

                    sRefColumnItem = sRefColumnItem->mNext;
                }

                sRefNodeItem = sRefNodeItem->mNext;
            }
        }

        /* Function 정보 설정 */
        sFunction->mArgCount = 2;

        sFunction->mArgs[0] = sRowExpr->mArgs[0];
        sFunction->mArgs[1] = (qlncExprCommon*)sInnerColumn;

        QLNC_SET_EXPR_COMMON( &sFunction->mCommon,
                              &QLL_CANDIDATE_MEM( aEnv ),
                              aEnv,
                              QLNC_EXPR_TYPE_FUNCTION,
                              aExpr->mExprPhraseFlag,
                              aExpr->mIterationTime,
                              aExpr->mPosition,
                              aExpr->mColumnName,
                              DTL_TYPE_BOOLEAN );

        sFilter = (qlncExprCommon*)sFunction;

        /* Boolean Filter의 Expression에 Filter에 등록 */
        sBooleanFilter->mExpr = sFilter;
        sBooleanFilter->mFuncID = ((qlncExprFunction*)sFilter)->mFuncId;

        STL_THROW( RAMP_FINISH_SET_FILTER );
    }
    else
    {
        if( ((qlncExprListFunc*)aExpr)->mFuncId == KNL_BUILTIN_FUNC_IN )
        {
            ((qlncExprListFunc*)aExpr)->mFuncId = KNL_BUILTIN_FUNC_IS_EQUAL_ROW;
        }
        else
        {
            ((qlncExprListFunc*)aExpr)->mFuncId = KNL_BUILTIN_FUNC_IS_NOT_EQUAL_ROW;
        }
    }

    /* Right Row Expr의 Argument를 SubQuery의 Target들로 변경 */
    sTmpRowExpr = ((qlncExprRowExpr*)(((qlncExprListCol*)(((qlncExprListFunc*)aExpr)->mArgs[0]))->mArgs[0]));

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprCommon* ) * sSubQuery->mTargetCount,
                                (void**) &(sTmpRowExpr->mArgs),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sTmpRowExpr->mArgCount = sSubQuery->mTargetCount;

    for( i = 0; i < sSubQuery->mTargetCount; i++ )
    {
        /* Sub Table Node에 대한 Inner Column 만들기 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncExprInnerColumn ),
                                    (void**) &sInnerColumn,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        QLNC_SET_EXPR_COMMON( &sInnerColumn->mCommon,
                              &QLL_CANDIDATE_MEM( aEnv ),
                              aEnv,
                              QLNC_EXPR_TYPE_INNER_COLUMN,
                              sSubQuery->mTargetArr[i].mExpr->mExprPhraseFlag,
                              sSubQuery->mTargetArr[i].mExpr->mIterationTime,
                              sSubQuery->mTargetArr[i].mExpr->mPosition,
                              sSubQuery->mTargetArr[i].mExpr->mColumnName,
                              sSubQuery->mTargetArr[i].mExpr->mDataType );

        sInnerColumn->mNode = (qlncNodeCommon*)sSubTableNode;
        sInnerColumn->mIdx = i;
        sInnerColumn->mOrgExpr = NULL;

        /* Boolean Filter의 Reference Node List에 등록 */
        STL_TRY( qlncAddRefColumnToRefNodeList( &sConvertExprParamInfo,
                                                (qlncExprCommon*)sInnerColumn,
                                                aEnv )
                 == STL_SUCCESS );

        sTmpRowExpr->mArgs[i] = (qlncExprCommon*)sInnerColumn;
    }

    sFilter = aExpr;

    /* Boolean Filter의 Expression에 Filter에 등록 */
    sBooleanFilter->mExpr = sFilter;
    if( sFilter->mType == QLNC_EXPR_TYPE_LIST_FUNCTION )
    {
        sBooleanFilter->mFuncID = ((qlncExprListFunc*)sFilter)->mFuncId;
    }
    else
    {
        STL_DASSERT( sFilter->mType != QLNC_EXPR_TYPE_FUNCTION );
        sBooleanFilter->mFuncID = KNL_BUILTIN_FUNC_INVALID;
    }

    STL_RAMP( RAMP_FINISH_SET_FILTER );

    /* Sub Table Node의 Filter 설정 */
    sSubTableNode->mFilter = sAndFilter;


    /**
     * Target Expression 생성
     */

    /* Value Expression 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprValue ),
                                (void**) &sValue,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sValue->mValueType = QLNC_VALUE_TYPE_BOOLEAN;
    sValue->mInternalBindIdx = QLV_INTERNAL_BIND_VARIABLE_IDX_NA;
    sValue->mData.mInteger = STL_TRUE;

    QLNC_SET_EXPR_COMMON( &sValue->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_VALUE,
                          QLNC_EXPR_PHRASE_TARGET,
                          DTL_ITERATION_TIME_NONE,
                          aExpr->mPosition,
                          aExpr->mColumnName,
                          DTL_TYPE_BOOLEAN );

    /* Constant Result Expression 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprConstExpr ),
                                (void**) &sConstExpr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_SET_EXPR_COMMON( &sConstExpr->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT,
                          sValue->mCommon.mExprPhraseFlag,
                          sValue->mCommon.mIterationTime,
                          sValue->mCommon.mPosition,
                          sValue->mCommon.mColumnName,
                          DTL_TYPE_BOOLEAN );

    sConstExpr->mOrgExpr = (qlncExprCommon*)sValue;
    

    /**
     * Fetch Value Expression 생성
     */

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprValue ),
                                (void**) &sFetchValue,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sFetchValue->mValueType = QLNC_VALUE_TYPE_NUMERIC;
    sFetchValue->mInternalBindIdx = QLV_INTERNAL_BIND_VARIABLE_IDX_NA;
    QLNC_ALLOC_AND_COPY_STRING( sFetchValue->mData.mString,
                                "1",
                                &QLL_CANDIDATE_MEM( aEnv ),
                                aEnv );

    QLNC_SET_EXPR_COMMON( &sFetchValue->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_VALUE,
                          QLNC_EXPR_PHRASE_OFFSET_LIMIT,
                          DTL_ITERATION_TIME_NONE,
                          aExpr->mPosition,
                          aExpr->mColumnName,
                          DTL_TYPE_NATIVE_BIGINT );


    /**
     * Query Spec 생성
     */

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncQuerySpec ),
                                (void**) &sQuerySpec,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_INIT_NODE_COMMON( &(sQuerySpec->mNodeCommon),
                           *(aParamInfo->mGlobalNodeID),
                           QLNC_NODE_TYPE_QUERY_SPEC );
    (*(aParamInfo->mGlobalNodeID))++;   /* Global ID 증가 */

    sQuerySpec->mTableMapArr = NULL;
    sQuerySpec->mIsDistinct = STL_FALSE;
    sQuerySpec->mTargetCount = 1;

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncTarget ),
                                (void**) &(sQuerySpec->mTargetArr),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sQuerySpec->mTargetArr[0].mDisplayPos = 0;
    sQuerySpec->mTargetArr[0].mDisplayName = NULL;
    sQuerySpec->mTargetArr[0].mAliasName = NULL;
    sQuerySpec->mTargetArr[0].mExpr = (qlncExprCommon*)sConstExpr;

    sQuerySpec->mChildNode = (qlncNodeCommon*)sSubTableNode;
    sQuerySpec->mOffset = NULL;
    sQuerySpec->mLimit = (qlncExprCommon*)sFetchValue;
    sQuerySpec->mNonFilterSubQueryExprList = NULL;

    if( sSubTableNode->mQueryNode->mNodeType == QLNC_NODE_TYPE_QUERY_SPEC )
    {
        sQuerySpec->mQueryTransformHint =
            ((qlncQuerySpec*)(sSubTableNode->mQueryNode))->mQueryTransformHint;
        sQuerySpec->mOtherHint =
            ((qlncQuerySpec*)(sSubTableNode->mQueryNode))->mOtherHint;
    }
    else
    {
        /* Query Transformation Hint 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncQueryTransformHint ),
                                    (void**) &(sQuerySpec->mQueryTransformHint),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        QLNC_INIT_QUERY_TRANSFORM_HINT( sQuerySpec->mQueryTransformHint );

        /* Other Hint 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncOtherHint ),
                                    (void**) &(sQuerySpec->mOtherHint),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        QLNC_INIT_OTHER_HINT( sQuerySpec->mOtherHint );
    }

    /**
     * @todo Query Block ID에 대한 처리가 필요
     */


    /**
     * SubQuery Expression 정보를 새로 생성한 Query Spec을 이용하여 변경
     */

    sSubQuery->mNode = (qlncNodeCommon*)sQuerySpec;
    sSubQuery->mTargetCount = sQuerySpec->mTargetCount;
    sSubQuery->mTargetArr = sQuerySpec->mTargetArr;
    sSubQuery->mIsRelSubQuery = STL_TRUE;

    /* 현재 SubQuery의 Node에 대하여 In Reation SubQuery를 체크한다. */
    STL_TRY( qlncRewriteInRelationSubQuery( aParamInfo,
                                            sSubQuery->mNode,
                                            aEnv )
        == STL_SUCCESS );


    /**
     * Output 설정
     */

    *aOutExpr = (qlncExprCommon*)sSubQuery;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Exists(SubQuery) 를 SubQuery로 Rewrite한다.
 * @param[in]   aParamInfo  Parameter Info
 * @param[in]   aExpr       Expression
 * @param[out]  aOutExpr    Output Expression
 * @param[in]   aEnv        Environment
 */
stlStatus qlncRewriteInRelationSubQueryExists( qlncParamInfo      * aParamInfo,
                                               qlncExprCommon     * aExpr,
                                               qlncExprCommon    ** aOutExpr,
                                               qllEnv             * aEnv )
{
    stlBool               sReturnVal    = STL_FALSE;
    qlncExprSubQuery    * sSubQuery     = NULL;
    qlncExprInnerColumn * sInnerColumn  = NULL;
    qlncExprValue       * sValue        = NULL;
    qlncExprConstExpr   * sConstExpr    = NULL;
    qlncSubTableNode    * sSubTableNode = NULL;
    qlncQuerySpec       * sQuerySpec    = NULL;
    qlncExprValue       * sFetchValue   = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr->mType == QLNC_EXPR_TYPE_LIST_FUNCTION, QLL_ERROR_STACK(aEnv) );


    /* Row Expression 설정 */
    STL_DASSERT( ((qlncExprListFunc*)aExpr)->mArgCount == 1  );

    /* Right SubQuery Expression 설정 */
    sSubQuery = (qlncExprSubQuery*)(((qlncExprRowExpr*)(((qlncExprListCol*)(((qlncExprListFunc*)aExpr)->mArgs[0]))->mArgs[0]))->mArgs[0]);


    /**
     * Sub Table Node 생성
     */

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncSubTableNode ),
                                (void**) &sSubTableNode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_INIT_NODE_COMMON( &(sSubTableNode->mNodeCommon),
                           *(aParamInfo->mGlobalNodeID),
                           QLNC_NODE_TYPE_SUB_TABLE );
    (*(aParamInfo->mGlobalNodeID))++;   /* Global ID 증가 */

    sSubTableNode->mRelationName = NULL;
    sSubTableNode->mQueryNode = sSubQuery->mNode;
    sSubTableNode->mJoinHint = NULL;
    sSubTableNode->mFilter = NULL;
    QLNC_INIT_LIST( &(sSubTableNode->mReadColumnList) );


    /**
     * Target List를 없애고 Constant Value를 Target Column으로 설정한다.
     */

    if( ((qlncExprListFunc*)sFilter)->mFuncId == KNL_BUILTIN_FUNC_EXISTS )
    {
        sReturnVal  = STL_TRUE;
    }
    else
    {
        STL_DASSERT( ((qlncExprListFunc*)sFilter)->mFuncId ==
                     KNL_BUILTIN_FUNC_NOT_EXISTS );

        sReturnVal  = STL_FALSE;
    }

    /* Value Expression 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprValue ),
                                (void**) &sValue,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sValue->mValueType = QLNC_VALUE_TYPE_BOOLEAN;
    sValue->mInternalBindIdx = QLV_INTERNAL_BIND_VARIABLE_IDX_NA;
    sValue->mData.mInteger = sReturnVal;

    QLNC_SET_EXPR_COMMON( &sValue->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_VALUE,
                          QLNC_EXPR_PHRASE_TARGET,
                          DTL_ITERATION_TIME_NONE,
                          aExpr->mPosition,
                          aExpr->mColumnName,
                          DTL_TYPE_BOOLEAN );

    /* Constant Result Expression 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprConstExpr ),
                                (void**) &sConstExpr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_SET_EXPR_COMMON( &sConstExpr->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT,
                          sValue->mCommon.mExprPhraseFlag,
                          sValue->mCommon.mIterationTime,
                          sValue->mCommon.mPosition,
                          sValue->mCommon.mColumnName,
                          DTL_TYPE_BOOLEAN );

    sConstExpr->mOrgExpr = (qlncExprCommon*)sValue;

    /* Constant Result Expression을 Target Column으로 설정 */
    if( sSubQuery->mNode->mNodeType == QLNC_NODE_TYPE_QUERY_SPEC )
    {
        ((qlncQuerySpec*)(sSubQuery->mNode))->mTargetArr[0].mExpr = (qlncExprCommon*)sConstExpr;
        ((qlncQuerySpec*)(sSubQuery->mNode))->mTargetCount = 1;
    }
    else
    {
        STL_DASSERT( sSubQuery->mNode->mNodeType == QLNC_NODE_TYPE_QUERY_SET );

        /**
         * @todo Set 연산에 대한 처리 필요
         */
    }

    sSubQuery->mTargetArr[0].mExpr = (qlncExprCommon*) sConstExpr;
    sSubQuery->mTargetCount = 1;

    /* Sub Table Node에 대한 Inner Column 만들기 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprInnerColumn ),
                                (void**) &sInnerColumn,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_SET_EXPR_COMMON( &sInnerColumn->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_INNER_COLUMN,
                          sSubQuery->mTargetArr[0].mExpr->mExprPhraseFlag,
                          sSubQuery->mTargetArr[0].mExpr->mIterationTime,
                          sSubQuery->mTargetArr[0].mExpr->mPosition,
                          sSubQuery->mTargetArr[0].mExpr->mColumnName,
                          sSubQuery->mTargetArr[0].mExpr->mDataType );

    sInnerColumn->mNode = (qlncNodeCommon*)sSubTableNode;
    sInnerColumn->mIdx = 0;
    sInnerColumn->mOrgExpr = NULL;

    /* Sub Table의 Read Column List에 등록 */
    STL_TRY( qlncAddColumnToRefColumnList( aParamInfo,
                                           (qlncExprCommon*)sInnerColumn,
                                           &(sSubTableNode->mReadColumnList),
                                           aEnv )
             == STL_SUCCESS );

    /* Sub Table Node의 Filter 설정 */
    sSubTableNode->mFilter = NULL;


    /**
     * Target Expression 생성
     */

    /* Value Expression 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprValue ),
                                (void**) &sValue,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sValue->mValueType = QLNC_VALUE_TYPE_BOOLEAN;
    sValue->mInternalBindIdx = QLV_INTERNAL_BIND_VARIABLE_IDX_NA;
    sValue->mData.mInteger = sReturnVal;

    QLNC_SET_EXPR_COMMON( &sValue->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_VALUE,
                          QLNC_EXPR_PHRASE_TARGET,
                          DTL_ITERATION_TIME_NONE,
                          aExpr->mPosition,
                          aExpr->mColumnName,
                          DTL_TYPE_BOOLEAN );

    /* Constant Result Expression 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprConstExpr ),
                                (void**) &sConstExpr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_SET_EXPR_COMMON( &sConstExpr->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT,
                          sValue->mCommon.mExprPhraseFlag,
                          sValue->mCommon.mIterationTime,
                          sValue->mCommon.mPosition,
                          sValue->mCommon.mColumnName,
                          DTL_TYPE_BOOLEAN );

    sConstExpr->mOrgExpr = (qlncExprCommon*)sValue;
    

    /**
     * Fetch Value Expression 생성
     */

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprValue ),
                                (void**) &sFetchValue,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sFetchValue->mValueType = QLNC_VALUE_TYPE_NUMERIC;
    sFetchValue->mInternalBindIdx = QLV_INTERNAL_BIND_VARIABLE_IDX_NA;
    QLNC_ALLOC_AND_COPY_STRING( sFetchValue->mData.mString,
                                "1",
                                &QLL_CANDIDATE_MEM( aEnv ),
                                aEnv );

    QLNC_SET_EXPR_COMMON( &sFetchValue->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_VALUE,
                          QLNC_EXPR_PHRASE_OFFSET_LIMIT,
                          DTL_ITERATION_TIME_NONE,
                          aExpr->mPosition,
                          aExpr->mColumnName,
                          DTL_TYPE_NATIVE_BIGINT );


    /**
     * Query Spec 생성
     */

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncQuerySpec ),
                                (void**) &sQuerySpec,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_INIT_NODE_COMMON( &(sQuerySpec->mNodeCommon),
                           *(aParamInfo->mGlobalNodeID),
                           QLNC_NODE_TYPE_QUERY_SPEC );
    (*(aParamInfo->mGlobalNodeID))++;   /* Global ID 증가 */

    sQuerySpec->mTableMapArr = NULL;
    sQuerySpec->mIsDistinct = STL_FALSE;
    sQuerySpec->mTargetCount = 1;

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncTarget ),
                                (void**) &(sQuerySpec->mTargetArr),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sQuerySpec->mTargetArr[0].mDisplayPos = 0;
    sQuerySpec->mTargetArr[0].mDisplayName = NULL;
    sQuerySpec->mTargetArr[0].mAliasName = NULL;
    sQuerySpec->mTargetArr[0].mExpr = (qlncExprCommon*)sConstExpr;

    sQuerySpec->mChildNode = (qlncNodeCommon*)sSubTableNode;
    sQuerySpec->mOffset = NULL;
    sQuerySpec->mLimit = (qlncExprCommon*)sFetchValue;
    sQuerySpec->mNonFilterSubQueryExprList = NULL;

    if( sSubTableNode->mQueryNode->mNodeType == QLNC_NODE_TYPE_QUERY_SPEC )
    {
        sQuerySpec->mQueryTransformHint =
            ((qlncQuerySpec*)(sSubTableNode->mQueryNode))->mQueryTransformHint;
        sQuerySpec->mOtherHint =
            ((qlncQuerySpec*)(sSubTableNode->mQueryNode))->mOtherHint;
    }
    else
    {
        /* Query Transformation Hint 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncQueryTransformHint ),
                                    (void**) &(sQuerySpec->mQueryTransformHint),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        QLNC_INIT_QUERY_TRANSFORM_HINT( sQuerySpec->mQueryTransformHint );

        /* Other Hint 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncOtherHint ),
                                    (void**) &(sQuerySpec->mOtherHint),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        QLNC_INIT_OTHER_HINT( sQuerySpec->mOtherHint );
    }

    /**
     * @todo Query Block ID에 대한 처리가 필요
     */


    /**
     * SubQuery Expression 정보를 새로 생성한 Query Spec을 이용하여 변경
     */

    sSubQuery->mNode = (qlncNodeCommon*)sQuerySpec;
    sSubQuery->mTargetCount = sQuerySpec->mTargetCount;
    sSubQuery->mTargetArr = sQuerySpec->mTargetArr;
    sSubQuery->mIsRelSubQuery = STL_TRUE;

    /* 현재 SubQuery의 Node에 대하여 In Reation SubQuery를 체크한다. */
    STL_TRY( qlncRewriteInRelationSubQuery( aParamInfo,
                                            sSubQuery->mNode,
                                            aEnv )
        == STL_SUCCESS );


    /**
     * Output 설정
     */

    *aOutExpr = (qlncExprCommon*)sSubQuery;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief comp ANY(SubQuery) 를 SubQuery로 Rewrite한다.
 * @param[in]   aParamInfo  Parameter Info
 * @param[in]   aExpr       Expression
 * @param[out]  aOutExpr    Output Expression
 * @param[in]   aEnv        Environment
 */
stlStatus qlncRewriteInRelationSubQueryAny( qlncParamInfo      * aParamInfo,
                                            qlncExprCommon     * aExpr,
                                            qlncExprCommon    ** aOutExpr,
                                            qllEnv             * aEnv )
{
    stlInt32                     i              = 0;
    qlncExprSubQuery           * sSubQuery      = NULL;
//    qlncExprRowExpr            * sRowExpr       = NULL;
    qlncExprInnerColumn        * sInnerColumn   = NULL;
    qlncExprValue              * sValue         = NULL;
    qlncExprConstExpr          * sConstExpr     = NULL;
    qlncSubTableNode           * sSubTableNode  = NULL;
    qlncExprCommon             * sFilter        = NULL;
    qlncAndFilter              * sAndFilter     = NULL;
    qlncExprRowExpr            * sTmpRowExpr    = NULL;
    qlncQuerySpec              * sQuerySpec     = NULL;
    qlncExprValue              * sFetchValue    = NULL;
    qlncBooleanFilter          * sBooleanFilter = NULL;
    qlncConvertExprParamInfo     sConvertExprParamInfo;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr->mType == QLNC_EXPR_TYPE_LIST_FUNCTION, QLL_ERROR_STACK(aEnv) );


    /* Row Expression 설정 */
    STL_DASSERT( ((qlncExprListFunc*)aExpr)->mArgCount == 2  );
//    sRowExpr = (qlncExprRowExpr*)(((qlncExprListCol*)(((qlncExprListFunc*)aExpr)->mArgs[1]))->mArgs[0]);

    /* Right SubQuery Expression 설정 */
    sSubQuery = (qlncExprSubQuery*)(((qlncExprRowExpr*)(((qlncExprListCol*)(((qlncExprListFunc*)aExpr)->mArgs[0]))->mArgs[0]))->mArgs[0]);


    /**
     * Sub Table Node 생성
     */

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncSubTableNode ),
                                (void**) &sSubTableNode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_INIT_NODE_COMMON( &(sSubTableNode->mNodeCommon),
                           *(aParamInfo->mGlobalNodeID),
                           QLNC_NODE_TYPE_SUB_TABLE );
    (*(aParamInfo->mGlobalNodeID))++;   /* Global ID 증가 */

    sSubTableNode->mRelationName = NULL;
    sSubTableNode->mQueryNode = sSubQuery->mNode;
    sSubTableNode->mJoinHint = NULL;
    sSubTableNode->mFilter = NULL;
    QLNC_INIT_LIST( &(sSubTableNode->mReadColumnList) );


    /**
     * Filter 생성
     */

    /* And Filter 생성 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncAndFilter ),
                                (void**) &sAndFilter,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_SET_EXPR_COMMON( &sAndFilter->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_AND_FILTER,
                          aExpr->mExprPhraseFlag,
                          aExpr->mIterationTime,
                          aExpr->mPosition,
                          aExpr->mColumnName,
                          DTL_TYPE_BOOLEAN );
                              
    sAndFilter->mOrCount = 1;

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncOrFilter* ),
                                (void**) &(sAndFilter->mOrFilters),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Or Filter 생성 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncOrFilter ),
                                (void**) &(sAndFilter->mOrFilters[0]),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_SET_EXPR_COMMON( &sAndFilter->mOrFilters[0]->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_OR_FILTER,
                          aExpr->mExprPhraseFlag,
                          aExpr->mIterationTime,
                          aExpr->mPosition,
                          aExpr->mColumnName,
                          DTL_TYPE_BOOLEAN );

    sAndFilter->mOrFilters[0]->mFilterCount = 1;

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncRefExprList ),
                                (void**) &(sAndFilter->mOrFilters[0]->mSubQueryExprList),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    QLNC_INIT_LIST( sAndFilter->mOrFilters[0]->mSubQueryExprList );

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprCommon* ),
                                (void**) &(sAndFilter->mOrFilters[0]->mFilters),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Boolean Filter 생성 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncBooleanFilter ),
                                (void**) &sBooleanFilter,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_SET_EXPR_COMMON( &sBooleanFilter->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_BOOLEAN_FILTER,
                          aExpr->mExprPhraseFlag,
                          aExpr->mIterationTime,
                          aExpr->mPosition,
                          aExpr->mColumnName,
                          DTL_TYPE_BOOLEAN );

    sBooleanFilter->mFuncID = KNL_BUILTIN_FUNC_INVALID;
    sBooleanFilter->mExpr = NULL;
    QLNC_INIT_LIST( &(sBooleanFilter->mRefNodeList) );
    sBooleanFilter->mLeftRefNodeList = NULL;
    sBooleanFilter->mRightRefNodeList = NULL;
    sBooleanFilter->mPossibleJoinCond = STL_FALSE;
    sBooleanFilter->mIsPhysicalFilter = STL_FALSE;
    sBooleanFilter->mSubQueryExprList = NULL;

    sAndFilter->mOrFilters[0]->mFilters[0] = (qlncExprCommon*)sBooleanFilter;

    sConvertExprParamInfo.mParamInfo = *aParamInfo;
    sConvertExprParamInfo.mTableMapArr = NULL;
    sConvertExprParamInfo.mOuterTableMapArr = NULL;
    sConvertExprParamInfo.mInitExpr = NULL;
    sConvertExprParamInfo.mCandExpr = NULL;
    sConvertExprParamInfo.mRefNodeList = &(sBooleanFilter->mRefNodeList);
    sConvertExprParamInfo.mExprPhraseFlag = 0;
    sConvertExprParamInfo.mSubQueryExprList = NULL;
    sConvertExprParamInfo.mExprSubQuery = NULL;

    
    /**
     * @remark Row Comparison 고려하여 작성
     */


        
    /**
     * Left Value 에 대한 NVL 함수 생성 - (A)
     */

        

    /**
     * Rigth Value 에 대한 EXISTS 함수 생성 - (B)
     */



    /**
     * Compare Function 에 대한 Query Set(union all) 구성 - (C)
     *
     * (C) <=> (C-1) UNION ALL (C-2) LIMIT 1
     * 
     * @remark Default Return Value = FALSE
     */


    /**
     * Compare Function을 조건으로 하는 EXISTS 함수 생성 (C-1)
     *
     * @remark Default Return Value 없음
     */

        
    /**
     * Sub Query의 Target이 NULL 인 제거한 Compare Function을 만족하는 EXISTS 함수 생성 (C-1)
     *  - case 1 : Scalar 비교인 경우
     *             EXISTS( Target IS NULL )
     * 
     *  - case 2 : Row 비교인 경우 
     *             EXISTS( (Comp Result) IS NULL )
     *
     * @remark Default Return Value = FALSE
     */


        
    /**
     * (A) + (B) + (C) 를 이용한 AND Function 구성
     */


    /* Right Row Expr의 Argument를 SubQuery의 Target들로 변경 */
    sTmpRowExpr = ((qlncExprRowExpr*)(((qlncExprListCol*)(((qlncExprListFunc*)aExpr)->mArgs[0]))->mArgs[0]));

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprCommon* ) * sSubQuery->mTargetCount,
                                (void**) &(sTmpRowExpr->mArgs),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sTmpRowExpr->mArgCount = sSubQuery->mTargetCount;

    for( i = 0; i < sSubQuery->mTargetCount; i++ )
    {
        /* Sub Table Node에 대한 Inner Column 만들기 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncExprInnerColumn ),
                                    (void**) &sInnerColumn,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        QLNC_SET_EXPR_COMMON( &sInnerColumn->mCommon,
                              &QLL_CANDIDATE_MEM( aEnv ),
                              aEnv,
                              QLNC_EXPR_TYPE_INNER_COLUMN,
                              sSubQuery->mTargetArr[i].mExpr->mExprPhraseFlag,
                              sSubQuery->mTargetArr[i].mExpr->mIterationTime,
                              sSubQuery->mTargetArr[i].mExpr->mPosition,
                              sSubQuery->mTargetArr[i].mExpr->mColumnName,
                              sSubQuery->mTargetArr[i].mExpr->mDataType );

        sInnerColumn->mNode = (qlncNodeCommon*)sSubTableNode;
        sInnerColumn->mIdx = i;
        sInnerColumn->mOrgExpr = NULL;

        /* Boolean Filter의 Reference Node List에 등록 */
        STL_TRY( qlncAddRefColumnToRefNodeList( &sConvertExprParamInfo,
                                                (qlncExprCommon*)sInnerColumn,
                                                aEnv )
                 == STL_SUCCESS );

        sTmpRowExpr->mArgs[i] = (qlncExprCommon*)sInnerColumn;
    }

    sFilter = aExpr;

    /* Boolean Filter의 Expression에 Filter에 등록 */
    sBooleanFilter->mExpr = sFilter;
    if( sFilter->mType == QLNC_EXPR_TYPE_LIST_FUNCTION )
    {
        sBooleanFilter->mFuncID = ((qlncExprListFunc*)sFilter)->mFuncId;
    }
    else
    {
        STL_DASSERT( sFilter->mType != QLNC_EXPR_TYPE_FUNCTION );
        sBooleanFilter->mFuncID = KNL_BUILTIN_FUNC_INVALID;
    }

    /* Sub Table Node의 Filter 설정 */
    sSubTableNode->mFilter = sAndFilter;


    /**
     * Target Expression 생성
     */

    /* Value Expression 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprValue ),
                                (void**) &sValue,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sValue->mValueType = QLNC_VALUE_TYPE_BOOLEAN;
    sValue->mInternalBindIdx = QLV_INTERNAL_BIND_VARIABLE_IDX_NA;
    sValue->mData.mInteger = STL_TRUE;

    QLNC_SET_EXPR_COMMON( &sValue->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_VALUE,
                          QLNC_EXPR_PHRASE_TARGET,
                          DTL_ITERATION_TIME_NONE,
                          aExpr->mPosition,
                          aExpr->mColumnName,
                          DTL_TYPE_BOOLEAN );

    /* Constant Result Expression 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprConstExpr ),
                                (void**) &sConstExpr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_SET_EXPR_COMMON( &sConstExpr->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT,
                          sValue->mCommon.mExprPhraseFlag,
                          sValue->mCommon.mIterationTime,
                          sValue->mCommon.mPosition,
                          sValue->mCommon.mColumnName,
                          DTL_TYPE_BOOLEAN );

    sConstExpr->mOrgExpr = (qlncExprCommon*)sValue;
    

    /**
     * Fetch Value Expression 생성
     */

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprValue ),
                                (void**) &sFetchValue,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sFetchValue->mValueType = QLNC_VALUE_TYPE_NUMERIC;
    sFetchValue->mInternalBindIdx = QLV_INTERNAL_BIND_VARIABLE_IDX_NA;
    QLNC_ALLOC_AND_COPY_STRING( sFetchValue->mData.mString,
                                "1",
                                &QLL_CANDIDATE_MEM( aEnv ),
                                aEnv );

    QLNC_SET_EXPR_COMMON( &sFetchValue->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_VALUE,
                          QLNC_EXPR_PHRASE_OFFSET_LIMIT,
                          DTL_ITERATION_TIME_NONE,
                          aExpr->mPosition,
                          aExpr->mColumnName,
                          DTL_TYPE_NATIVE_BIGINT );


    /**
     * Query Spec 생성
     */

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncQuerySpec ),
                                (void**) &sQuerySpec,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_INIT_NODE_COMMON( &(sQuerySpec->mNodeCommon),
                           *(aParamInfo->mGlobalNodeID),
                           QLNC_NODE_TYPE_QUERY_SPEC );
    (*(aParamInfo->mGlobalNodeID))++;   /* Global ID 증가 */

    sQuerySpec->mTableMapArr = NULL;
    sQuerySpec->mIsDistinct = STL_FALSE;
    sQuerySpec->mTargetCount = 1;

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncTarget ),
                                (void**) &(sQuerySpec->mTargetArr),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sQuerySpec->mTargetArr[0].mDisplayPos = 0;
    sQuerySpec->mTargetArr[0].mDisplayName = NULL;
    sQuerySpec->mTargetArr[0].mAliasName = NULL;
    sQuerySpec->mTargetArr[0].mExpr = (qlncExprCommon*)sConstExpr;

    sQuerySpec->mChildNode = (qlncNodeCommon*)sSubTableNode;
    sQuerySpec->mOffset = NULL;
    sQuerySpec->mLimit = (qlncExprCommon*)sFetchValue;
    sQuerySpec->mNonFilterSubQueryExprList = NULL;

    if( sSubTableNode->mQueryNode->mNodeType == QLNC_NODE_TYPE_QUERY_SPEC )
    {
        sQuerySpec->mQueryTransformHint =
            ((qlncQuerySpec*)(sSubTableNode->mQueryNode))->mQueryTransformHint;
        sQuerySpec->mOtherHint =
            ((qlncQuerySpec*)(sSubTableNode->mQueryNode))->mOtherHint;
    }
    else
    {
        /* Query Transformation Hint 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncQueryTransformHint ),
                                    (void**) &(sQuerySpec->mQueryTransformHint),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        QLNC_INIT_QUERY_TRANSFORM_HINT( sQuerySpec->mQueryTransformHint );

        /* Other Hint 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncOtherHint ),
                                    (void**) &(sQuerySpec->mOtherHint),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        QLNC_INIT_OTHER_HINT( sQuerySpec->mOtherHint );
    }

    /**
     * @todo Query Block ID에 대한 처리가 필요
     */


    /**
     * SubQuery Expression 정보를 새로 생성한 Query Spec을 이용하여 변경
     */

    sSubQuery->mNode = (qlncNodeCommon*)sQuerySpec;
    sSubQuery->mTargetCount = sQuerySpec->mTargetCount;
    sSubQuery->mTargetArr = sQuerySpec->mTargetArr;
    sSubQuery->mIsRelSubQuery = STL_TRUE;

    /* 현재 SubQuery의 Node에 대하여 In Reation SubQuery를 체크한다. */
    STL_TRY( qlncRewriteInRelationSubQuery( aParamInfo,
                                            sSubQuery->mNode,
                                            aEnv )
        == STL_SUCCESS );


    /**
     * Output 설정
     */

    *aOutExpr = (qlncExprCommon*)sSubQuery;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief comp ALL(SubQuery) 를 SubQuery로 Rewrite한다.
 * @param[in]   aParamInfo  Parameter Info
 * @param[in]   aExpr       Expression
 * @param[out]  aOutExpr    Output Expression
 * @param[in]   aEnv        Environment
 */
stlStatus qlncRewriteInRelationSubQueryAll( qlncParamInfo      * aParamInfo,
                                            qlncExprCommon     * aExpr,
                                            qlncExprCommon    ** aOutExpr,
                                            qllEnv             * aEnv )
{
    stlInt32                     i              = 0;
    qlncExprSubQuery           * sSubQuery      = NULL;
//    qlncExprRowExpr            * sRowExpr       = NULL;
    qlncExprInnerColumn        * sInnerColumn   = NULL;
    qlncExprValue              * sValue         = NULL;
    qlncExprConstExpr          * sConstExpr     = NULL;
    qlncSubTableNode           * sSubTableNode  = NULL;
    qlncExprCommon             * sFilter        = NULL;
    qlncAndFilter              * sAndFilter     = NULL;
    qlncExprRowExpr            * sTmpRowExpr    = NULL;
    qlncQuerySpec              * sQuerySpec     = NULL;
    qlncExprValue              * sFetchValue    = NULL;
    qlncBooleanFilter          * sBooleanFilter = NULL;
    qlncConvertExprParamInfo     sConvertExprParamInfo;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr->mType == QLNC_EXPR_TYPE_LIST_FUNCTION, QLL_ERROR_STACK(aEnv) );


    /* Row Expression 설정 */
    STL_DASSERT( ((qlncExprListFunc*)aExpr)->mArgCount == 2  );
//    sRowExpr = (qlncExprRowExpr*)(((qlncExprListCol*)(((qlncExprListFunc*)aExpr)->mArgs[1]))->mArgs[0]);

    /* Right SubQuery Expression 설정 */
    sSubQuery = (qlncExprSubQuery*)(((qlncExprRowExpr*)(((qlncExprListCol*)(((qlncExprListFunc*)aExpr)->mArgs[0]))->mArgs[0]))->mArgs[0]);


    /**
     * Sub Table Node 생성
     */

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncSubTableNode ),
                                (void**) &sSubTableNode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_INIT_NODE_COMMON( &(sSubTableNode->mNodeCommon),
                           *(aParamInfo->mGlobalNodeID),
                           QLNC_NODE_TYPE_SUB_TABLE );
    (*(aParamInfo->mGlobalNodeID))++;   /* Global ID 증가 */

    sSubTableNode->mRelationName = NULL;
    sSubTableNode->mQueryNode = sSubQuery->mNode;
    sSubTableNode->mJoinHint = NULL;
    sSubTableNode->mFilter = NULL;
    QLNC_INIT_LIST( &(sSubTableNode->mReadColumnList) );


    /**
     * Filter 생성
     */

    /* And Filter 생성 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncAndFilter ),
                                (void**) &sAndFilter,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_SET_EXPR_COMMON( &sAndFilter->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_AND_FILTER,
                          aExpr->mExprPhraseFlag,
                          aExpr->mIterationTime,
                          aExpr->mPosition,
                          aExpr->mColumnName,
                          DTL_TYPE_BOOLEAN );
                              
    sAndFilter->mOrCount = 1;

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncOrFilter* ),
                                (void**) &(sAndFilter->mOrFilters),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Or Filter 생성 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncOrFilter ),
                                (void**) &(sAndFilter->mOrFilters[0]),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_SET_EXPR_COMMON( &sAndFilter->mOrFilters[0]->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_OR_FILTER,
                          aExpr->mExprPhraseFlag,
                          aExpr->mIterationTime,
                          aExpr->mPosition,
                          aExpr->mColumnName,
                          DTL_TYPE_BOOLEAN );

    sAndFilter->mOrFilters[0]->mFilterCount = 1;

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncRefExprList ),
                                (void**) &(sAndFilter->mOrFilters[0]->mSubQueryExprList),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    QLNC_INIT_LIST( sAndFilter->mOrFilters[0]->mSubQueryExprList );

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprCommon* ),
                                (void**) &(sAndFilter->mOrFilters[0]->mFilters),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Boolean Filter 생성 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncBooleanFilter ),
                                (void**) &sBooleanFilter,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_SET_EXPR_COMMON( &sBooleanFilter->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_BOOLEAN_FILTER,
                          aExpr->mExprPhraseFlag,
                          aExpr->mIterationTime,
                          aExpr->mPosition,
                          aExpr->mColumnName,
                          DTL_TYPE_BOOLEAN );

    sBooleanFilter->mFuncID = KNL_BUILTIN_FUNC_INVALID;
    sBooleanFilter->mExpr = NULL;
    QLNC_INIT_LIST( &(sBooleanFilter->mRefNodeList) );
    sBooleanFilter->mLeftRefNodeList = NULL;
    sBooleanFilter->mRightRefNodeList = NULL;
    sBooleanFilter->mPossibleJoinCond = STL_FALSE;
    sBooleanFilter->mIsPhysicalFilter = STL_FALSE;
    sBooleanFilter->mSubQueryExprList = NULL;

    sAndFilter->mOrFilters[0]->mFilters[0] = (qlncExprCommon*)sBooleanFilter;

    sConvertExprParamInfo.mParamInfo = *aParamInfo;
    sConvertExprParamInfo.mTableMapArr = NULL;
    sConvertExprParamInfo.mOuterTableMapArr = NULL;
    sConvertExprParamInfo.mInitExpr = NULL;
    sConvertExprParamInfo.mCandExpr = NULL;
    sConvertExprParamInfo.mRefNodeList = &(sBooleanFilter->mRefNodeList);
    sConvertExprParamInfo.mExprPhraseFlag = 0;
    sConvertExprParamInfo.mSubQueryExprList = NULL;
    sConvertExprParamInfo.mExprSubQuery = NULL;
    
    /**
     * @todo Comp ALL에 대한 기능 수정 후 Default 값을 TRUE로 변경
     */

    /* Right Row Expr의 Argument를 SubQuery의 Target들로 변경 */
    sTmpRowExpr = ((qlncExprRowExpr*)(((qlncExprListCol*)(((qlncExprListFunc*)aExpr)->mArgs[0]))->mArgs[0]));

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprCommon* ) * sSubQuery->mTargetCount,
                                (void**) &(sTmpRowExpr->mArgs),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sTmpRowExpr->mArgCount = sSubQuery->mTargetCount;

    for( i = 0; i < sSubQuery->mTargetCount; i++ )
    {
        /* Sub Table Node에 대한 Inner Column 만들기 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncExprInnerColumn ),
                                    (void**) &sInnerColumn,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        QLNC_SET_EXPR_COMMON( &sInnerColumn->mCommon,
                              &QLL_CANDIDATE_MEM( aEnv ),
                              aEnv,
                              QLNC_EXPR_TYPE_INNER_COLUMN,
                              sSubQuery->mTargetArr[i].mExpr->mExprPhraseFlag,
                              sSubQuery->mTargetArr[i].mExpr->mIterationTime,
                              sSubQuery->mTargetArr[i].mExpr->mPosition,
                              sSubQuery->mTargetArr[i].mExpr->mColumnName,
                              sSubQuery->mTargetArr[i].mExpr->mDataType );

        sInnerColumn->mNode = (qlncNodeCommon*)sSubTableNode;
        sInnerColumn->mIdx = i;
        sInnerColumn->mOrgExpr = NULL;

        /* Boolean Filter의 Reference Node List에 등록 */
        STL_TRY( qlncAddRefColumnToRefNodeList( &sConvertExprParamInfo,
                                                (qlncExprCommon*)sInnerColumn,
                                                aEnv )
                 == STL_SUCCESS );

        sTmpRowExpr->mArgs[i] = (qlncExprCommon*)sInnerColumn;
    }

    sFilter = aExpr;

    /* Boolean Filter의 Expression에 Filter에 등록 */
    sBooleanFilter->mExpr = sFilter;
    if( sFilter->mType == QLNC_EXPR_TYPE_LIST_FUNCTION )
    {
        sBooleanFilter->mFuncID = ((qlncExprListFunc*)sFilter)->mFuncId;
    }
    else
    {
        STL_DASSERT( sFilter->mType != QLNC_EXPR_TYPE_FUNCTION );
        sBooleanFilter->mFuncID = KNL_BUILTIN_FUNC_INVALID;
    }

    /* Sub Table Node의 Filter 설정 */
    sSubTableNode->mFilter = sAndFilter;


    /**
     * Target Expression 생성
     */

    /* Value Expression 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprValue ),
                                (void**) &sValue,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sValue->mValueType = QLNC_VALUE_TYPE_BOOLEAN;
    sValue->mInternalBindIdx = QLV_INTERNAL_BIND_VARIABLE_IDX_NA;
    sValue->mData.mInteger = STL_TRUE;

    QLNC_SET_EXPR_COMMON( &sValue->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_VALUE,
                          QLNC_EXPR_PHRASE_TARGET,
                          DTL_ITERATION_TIME_NONE,
                          aExpr->mPosition,
                          aExpr->mColumnName,
                          DTL_TYPE_BOOLEAN );

    /* Constant Result Expression 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprConstExpr ),
                                (void**) &sConstExpr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_SET_EXPR_COMMON( &sConstExpr->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT,
                          sValue->mCommon.mExprPhraseFlag,
                          sValue->mCommon.mIterationTime,
                          sValue->mCommon.mPosition,
                          sValue->mCommon.mColumnName,
                          DTL_TYPE_BOOLEAN );

    sConstExpr->mOrgExpr = (qlncExprCommon*)sValue;
    

    /**
     * Fetch Value Expression 생성
     */

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprValue ),
                                (void**) &sFetchValue,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sFetchValue->mValueType = QLNC_VALUE_TYPE_NUMERIC;
    sFetchValue->mInternalBindIdx = QLV_INTERNAL_BIND_VARIABLE_IDX_NA;
    QLNC_ALLOC_AND_COPY_STRING( sFetchValue->mData.mString,
                                "1",
                                &QLL_CANDIDATE_MEM( aEnv ),
                                aEnv );

    QLNC_SET_EXPR_COMMON( &sFetchValue->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_VALUE,
                          QLNC_EXPR_PHRASE_OFFSET_LIMIT,
                          DTL_ITERATION_TIME_NONE,
                          aExpr->mPosition,
                          aExpr->mColumnName,
                          DTL_TYPE_NATIVE_BIGINT );


    /**
     * Query Spec 생성
     */

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncQuerySpec ),
                                (void**) &sQuerySpec,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_INIT_NODE_COMMON( &(sQuerySpec->mNodeCommon),
                           *(aParamInfo->mGlobalNodeID),
                           QLNC_NODE_TYPE_QUERY_SPEC );
    (*(aParamInfo->mGlobalNodeID))++;   /* Global ID 증가 */

    sQuerySpec->mTableMapArr = NULL;
    sQuerySpec->mIsDistinct = STL_FALSE;
    sQuerySpec->mTargetCount = 1;

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncTarget ),
                                (void**) &(sQuerySpec->mTargetArr),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sQuerySpec->mTargetArr[0].mDisplayPos = 0;
    sQuerySpec->mTargetArr[0].mDisplayName = NULL;
    sQuerySpec->mTargetArr[0].mAliasName = NULL;
    sQuerySpec->mTargetArr[0].mExpr = (qlncExprCommon*)sConstExpr;

    sQuerySpec->mChildNode = (qlncNodeCommon*)sSubTableNode;
    sQuerySpec->mOffset = NULL;
    sQuerySpec->mLimit = (qlncExprCommon*)sFetchValue;
    sQuerySpec->mNonFilterSubQueryExprList = NULL;

    if( sSubTableNode->mQueryNode->mNodeType == QLNC_NODE_TYPE_QUERY_SPEC )
    {
        sQuerySpec->mQueryTransformHint =
            ((qlncQuerySpec*)(sSubTableNode->mQueryNode))->mQueryTransformHint;
        sQuerySpec->mOtherHint =
            ((qlncQuerySpec*)(sSubTableNode->mQueryNode))->mOtherHint;
    }
    else
    {
        /* Query Transformation Hint 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncQueryTransformHint ),
                                    (void**) &(sQuerySpec->mQueryTransformHint),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        QLNC_INIT_QUERY_TRANSFORM_HINT( sQuerySpec->mQueryTransformHint );

        /* Other Hint 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncOtherHint ),
                                    (void**) &(sQuerySpec->mOtherHint),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        QLNC_INIT_OTHER_HINT( sQuerySpec->mOtherHint );
    }

    /**
     * @todo Query Block ID에 대한 처리가 필요
     */


    /**
     * SubQuery Expression 정보를 새로 생성한 Query Spec을 이용하여 변경
     */

    sSubQuery->mNode = (qlncNodeCommon*)sQuerySpec;
    sSubQuery->mTargetCount = sQuerySpec->mTargetCount;
    sSubQuery->mTargetArr = sQuerySpec->mTargetArr;
    sSubQuery->mIsRelSubQuery = STL_TRUE;

    /* 현재 SubQuery의 Node에 대하여 In Reation SubQuery를 체크한다. */
    STL_TRY( qlncRewriteInRelationSubQuery( aParamInfo,
                                            sSubQuery->mNode,
                                            aEnv )
        == STL_SUCCESS );


    /**
     * Output 설정
     */

    *aOutExpr = (qlncExprCommon*)sSubQuery;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief row comp (SubQuery) 를 SubQuery로 Rewrite한다.
 * @param[in]   aParamInfo  Parameter Info
 * @param[in]   aExpr       Expression
 * @param[out]  aOutExpr    Output Expression
 * @param[in]   aEnv        Environment
 */
stlStatus qlncRewriteInRelationSubQueryRowComp( qlncParamInfo      * aParamInfo,
                                                qlncExprCommon     * aExpr,
                                                qlncExprCommon    ** aOutExpr,
                                                qllEnv             * aEnv )
{
    stlInt32                     i              = 0;
    qlncExprSubQuery           * sSubQuery      = NULL;
//    qlncExprRowExpr            * sRowExpr       = NULL;
    qlncExprInnerColumn        * sInnerColumn   = NULL;
    qlncExprValue              * sValue         = NULL;
    qlncExprConstExpr          * sConstExpr     = NULL;
    qlncSubTableNode           * sSubTableNode  = NULL;
    qlncExprCommon             * sFilter        = NULL;
    qlncAndFilter              * sAndFilter     = NULL;
    qlncExprRowExpr            * sTmpRowExpr    = NULL;
    qlncQuerySpec              * sQuerySpec     = NULL;
    qlncExprValue              * sFetchValue    = NULL;
    qlncBooleanFilter          * sBooleanFilter = NULL;
    qlncConvertExprParamInfo     sConvertExprParamInfo;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr->mType == QLNC_EXPR_TYPE_LIST_FUNCTION, QLL_ERROR_STACK(aEnv) );


    /* Row Expression 설정 */
    STL_DASSERT( ((qlncExprListFunc*)aExpr)->mArgCount == 2  );
//    sRowExpr = (qlncExprRowExpr*)(((qlncExprListCol*)(((qlncExprListFunc*)aExpr)->mArgs[1]))->mArgs[0]);

    /* Right SubQuery Expression 설정 */
    sSubQuery = (qlncExprSubQuery*)(((qlncExprRowExpr*)(((qlncExprListCol*)(((qlncExprListFunc*)aExpr)->mArgs[0]))->mArgs[0]))->mArgs[0]);


    /**
     * Sub Table Node 생성
     */

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncSubTableNode ),
                                (void**) &sSubTableNode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_INIT_NODE_COMMON( &(sSubTableNode->mNodeCommon),
                           *(aParamInfo->mGlobalNodeID),
                           QLNC_NODE_TYPE_SUB_TABLE );
    (*(aParamInfo->mGlobalNodeID))++;   /* Global ID 증가 */

    sSubTableNode->mRelationName = NULL;
    sSubTableNode->mQueryNode = sSubQuery->mNode;
    sSubTableNode->mJoinHint = NULL;
    sSubTableNode->mFilter = NULL;
    QLNC_INIT_LIST( &(sSubTableNode->mReadColumnList) );


    /**
     * Filter 생성
     */

    /* And Filter 생성 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncAndFilter ),
                                (void**) &sAndFilter,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_SET_EXPR_COMMON( &sAndFilter->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_AND_FILTER,
                          aExpr->mExprPhraseFlag,
                          aExpr->mIterationTime,
                          aExpr->mPosition,
                          aExpr->mColumnName,
                          DTL_TYPE_BOOLEAN );
                              
    sAndFilter->mOrCount = 1;

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncOrFilter* ),
                                (void**) &(sAndFilter->mOrFilters),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Or Filter 생성 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncOrFilter ),
                                (void**) &(sAndFilter->mOrFilters[0]),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_SET_EXPR_COMMON( &sAndFilter->mOrFilters[0]->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_OR_FILTER,
                          aExpr->mExprPhraseFlag,
                          aExpr->mIterationTime,
                          aExpr->mPosition,
                          aExpr->mColumnName,
                          DTL_TYPE_BOOLEAN );

    sAndFilter->mOrFilters[0]->mFilterCount = 1;

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncRefExprList ),
                                (void**) &(sAndFilter->mOrFilters[0]->mSubQueryExprList),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    QLNC_INIT_LIST( sAndFilter->mOrFilters[0]->mSubQueryExprList );

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprCommon* ),
                                (void**) &(sAndFilter->mOrFilters[0]->mFilters),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Boolean Filter 생성 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncBooleanFilter ),
                                (void**) &sBooleanFilter,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_SET_EXPR_COMMON( &sBooleanFilter->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_BOOLEAN_FILTER,
                          aExpr->mExprPhraseFlag,
                          aExpr->mIterationTime,
                          aExpr->mPosition,
                          aExpr->mColumnName,
                          DTL_TYPE_BOOLEAN );

    sBooleanFilter->mFuncID = KNL_BUILTIN_FUNC_INVALID;
    sBooleanFilter->mExpr = NULL;
    QLNC_INIT_LIST( &(sBooleanFilter->mRefNodeList) );
    sBooleanFilter->mLeftRefNodeList = NULL;
    sBooleanFilter->mRightRefNodeList = NULL;
    sBooleanFilter->mPossibleJoinCond = STL_FALSE;
    sBooleanFilter->mIsPhysicalFilter = STL_FALSE;
    sBooleanFilter->mSubQueryExprList = NULL;

    sAndFilter->mOrFilters[0]->mFilters[0] = (qlncExprCommon*)sBooleanFilter;

    sConvertExprParamInfo.mParamInfo = *aParamInfo;
    sConvertExprParamInfo.mTableMapArr = NULL;
    sConvertExprParamInfo.mOuterTableMapArr = NULL;
    sConvertExprParamInfo.mInitExpr = NULL;
    sConvertExprParamInfo.mCandExpr = NULL;
    sConvertExprParamInfo.mRefNodeList = &(sBooleanFilter->mRefNodeList);
    sConvertExprParamInfo.mExprPhraseFlag = 0;
    sConvertExprParamInfo.mSubQueryExprList = NULL;
    sConvertExprParamInfo.mExprSubQuery = NULL;
    
    /* Right Row Expr의 Argument를 SubQuery의 Target들로 변경 */
    sTmpRowExpr = ((qlncExprRowExpr*)(((qlncExprListCol*)(((qlncExprListFunc*)aExpr)->mArgs[0]))->mArgs[0]));

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprCommon* ) * sSubQuery->mTargetCount,
                                (void**) &(sTmpRowExpr->mArgs),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sTmpRowExpr->mArgCount = sSubQuery->mTargetCount;

    for( i = 0; i < sSubQuery->mTargetCount; i++ )
    {
        /* Sub Table Node에 대한 Inner Column 만들기 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncExprInnerColumn ),
                                    (void**) &sInnerColumn,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        QLNC_SET_EXPR_COMMON( &sInnerColumn->mCommon,
                              &QLL_CANDIDATE_MEM( aEnv ),
                              aEnv,
                              QLNC_EXPR_TYPE_INNER_COLUMN,
                              sSubQuery->mTargetArr[i].mExpr->mExprPhraseFlag,
                              sSubQuery->mTargetArr[i].mExpr->mIterationTime,
                              sSubQuery->mTargetArr[i].mExpr->mPosition,
                              sSubQuery->mTargetArr[i].mExpr->mColumnName,
                              sSubQuery->mTargetArr[i].mExpr->mDataType );

        sInnerColumn->mNode = (qlncNodeCommon*)sSubTableNode;
        sInnerColumn->mIdx = i;
        sInnerColumn->mOrgExpr = NULL;

        /* Boolean Filter의 Reference Node List에 등록 */
        STL_TRY( qlncAddRefColumnToRefNodeList( &sConvertExprParamInfo,
                                                (qlncExprCommon*)sInnerColumn,
                                                aEnv )
                 == STL_SUCCESS );

        sTmpRowExpr->mArgs[i] = (qlncExprCommon*)sInnerColumn;
    }

    sFilter = aExpr;

    /* Boolean Filter의 Expression에 Filter에 등록 */
    sBooleanFilter->mExpr = sFilter;
    if( sFilter->mType == QLNC_EXPR_TYPE_LIST_FUNCTION )
    {
        sBooleanFilter->mFuncID = ((qlncExprListFunc*)sFilter)->mFuncId;
    }
    else
    {
        STL_DASSERT( sFilter->mType != QLNC_EXPR_TYPE_FUNCTION );
        sBooleanFilter->mFuncID = KNL_BUILTIN_FUNC_INVALID;
    }

    /* Sub Table Node의 Filter 설정 */
    sSubTableNode->mFilter = sAndFilter;


    /**
     * Target Expression 생성
     */

    /* Value Expression 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprValue ),
                                (void**) &sValue,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sValue->mValueType = QLNC_VALUE_TYPE_BOOLEAN;
    sValue->mInternalBindIdx = QLV_INTERNAL_BIND_VARIABLE_IDX_NA;
    sValue->mData.mInteger = STL_TRUE;

    QLNC_SET_EXPR_COMMON( &sValue->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_VALUE,
                          QLNC_EXPR_PHRASE_TARGET,
                          DTL_ITERATION_TIME_NONE,
                          aExpr->mPosition,
                          aExpr->mColumnName,
                          DTL_TYPE_BOOLEAN );

    /* Constant Result Expression 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprConstExpr ),
                                (void**) &sConstExpr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_SET_EXPR_COMMON( &sConstExpr->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT,
                          sValue->mCommon.mExprPhraseFlag,
                          sValue->mCommon.mIterationTime,
                          sValue->mCommon.mPosition,
                          sValue->mCommon.mColumnName,
                          DTL_TYPE_BOOLEAN );

    sConstExpr->mOrgExpr = (qlncExprCommon*)sValue;
    

    /**
     * Fetch Value Expression 생성
     */

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprValue ),
                                (void**) &sFetchValue,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sFetchValue->mValueType = QLNC_VALUE_TYPE_NUMERIC;
    sFetchValue->mInternalBindIdx = QLV_INTERNAL_BIND_VARIABLE_IDX_NA;
    QLNC_ALLOC_AND_COPY_STRING( sFetchValue->mData.mString,
                                "1",
                                &QLL_CANDIDATE_MEM( aEnv ),
                                aEnv );

    QLNC_SET_EXPR_COMMON( &sFetchValue->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_VALUE,
                          QLNC_EXPR_PHRASE_OFFSET_LIMIT,
                          DTL_ITERATION_TIME_NONE,
                          aExpr->mPosition,
                          aExpr->mColumnName,
                          DTL_TYPE_NATIVE_BIGINT );


    /**
     * Query Spec 생성
     */

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncQuerySpec ),
                                (void**) &sQuerySpec,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_INIT_NODE_COMMON( &(sQuerySpec->mNodeCommon),
                           *(aParamInfo->mGlobalNodeID),
                           QLNC_NODE_TYPE_QUERY_SPEC );
    (*(aParamInfo->mGlobalNodeID))++;   /* Global ID 증가 */

    sQuerySpec->mTableMapArr = NULL;
    sQuerySpec->mIsDistinct = STL_FALSE;
    sQuerySpec->mTargetCount = 1;

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncTarget ),
                                (void**) &(sQuerySpec->mTargetArr),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sQuerySpec->mTargetArr[0].mDisplayPos = 0;
    sQuerySpec->mTargetArr[0].mDisplayName = NULL;
    sQuerySpec->mTargetArr[0].mAliasName = NULL;
    sQuerySpec->mTargetArr[0].mExpr = (qlncExprCommon*)sConstExpr;

    sQuerySpec->mChildNode = (qlncNodeCommon*)sSubTableNode;
    sQuerySpec->mOffset = NULL;
    sQuerySpec->mLimit = (qlncExprCommon*)sFetchValue;
    sQuerySpec->mNonFilterSubQueryExprList = NULL;

    if( sSubTableNode->mQueryNode->mNodeType == QLNC_NODE_TYPE_QUERY_SPEC )
    {
        sQuerySpec->mQueryTransformHint =
            ((qlncQuerySpec*)(sSubTableNode->mQueryNode))->mQueryTransformHint;
        sQuerySpec->mOtherHint =
            ((qlncQuerySpec*)(sSubTableNode->mQueryNode))->mOtherHint;
    }
    else
    {
        /* Query Transformation Hint 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncQueryTransformHint ),
                                    (void**) &(sQuerySpec->mQueryTransformHint),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        QLNC_INIT_QUERY_TRANSFORM_HINT( sQuerySpec->mQueryTransformHint );

        /* Other Hint 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncOtherHint ),
                                    (void**) &(sQuerySpec->mOtherHint),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        QLNC_INIT_OTHER_HINT( sQuerySpec->mOtherHint );
    }

    /**
     * @todo Query Block ID에 대한 처리가 필요
     */


    /**
     * SubQuery Expression 정보를 새로 생성한 Query Spec을 이용하여 변경
     */

    sSubQuery->mNode = (qlncNodeCommon*)sQuerySpec;
    sSubQuery->mTargetCount = sQuerySpec->mTargetCount;
    sSubQuery->mTargetArr = sQuerySpec->mTargetArr;
    sSubQuery->mIsRelSubQuery = STL_TRUE;

    /* 현재 SubQuery의 Node에 대하여 In Reation SubQuery를 체크한다. */
    STL_TRY( qlncRewriteInRelationSubQuery( aParamInfo,
                                            sSubQuery->mNode,
                                            aEnv )
        == STL_SUCCESS );


    /**
     * Output 설정
     */

    *aOutExpr = (qlncExprCommon*)sSubQuery;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SubQuery를 Join 형태로 Unnest 한다.
 * @param[in]   aParamInfo  Parameter Info
 * @param[in]   aNode       Candidate Node
 * @param[out]  aOutNode    Output Candidate Node
 * @param[in]   aEnv        Environment
 */
stlStatus qlncSubQueryUnnest( qlncParamInfo     * aParamInfo,
                              qlncNodeCommon    * aNode,
                              qlncNodeCommon   ** aOutNode,
                              qllEnv            * aEnv )
{
    stlInt32          i;
    qlncNodeCommon  * sNode     = NULL;
    qlncNodeCommon  * sOutNode  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode != NULL, QLL_ERROR_STACK(aEnv) );


    sNode = aNode;

    switch( sNode->mNodeType )
    {
        case QLNC_NODE_TYPE_QUERY_SET:
            STL_TRY( qlncSubQueryUnnest( aParamInfo,
                                         ((qlncQuerySet*)sNode)->mChildNode,
                                         &sOutNode,
                                         aEnv )
                     == STL_SUCCESS );
            ((qlncQuerySet*)sNode)->mChildNode = sOutNode;
            sOutNode = sNode;
            break;

        case QLNC_NODE_TYPE_SET_OP:
            for( i = 0; i < ((qlncSetOP*)sNode)->mChildCount; i++ )
            {
                STL_TRY( qlncSubQueryUnnest( aParamInfo,
                                             ((qlncSetOP*)sNode)->mChildNodeArr[i],
                                             &sOutNode,
                                             aEnv )
                         == STL_SUCCESS );
                ((qlncSetOP*)sNode)->mChildNodeArr[i] = sOutNode;
                sOutNode = sNode;
            }
            break;

        case QLNC_NODE_TYPE_QUERY_SPEC:
            if( ((qlncQuerySpec*)sNode)->mQueryTransformHint->mAllowedQueryTransform == STL_FALSE )
            {
                sOutNode = sNode;
                STL_THROW( RAMP_FINISH );
            }

            STL_TRY( qlncSubQueryUnnest( aParamInfo,
                                         ((qlncQuerySpec*)sNode)->mChildNode,
                                         &sOutNode,
                                         aEnv )
                     == STL_SUCCESS );
            ((qlncQuerySpec*)sNode)->mChildNode = sOutNode;
            sOutNode = sNode;
            break;

        case QLNC_NODE_TYPE_BASE_TABLE:
            if( ((qlncBaseTableNode*)sNode)->mFilter != NULL )
            {
                STL_TRY( qlncSQUTableFilter( aParamInfo,
                                             sNode,
                                             &sOutNode,
                                             aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                sOutNode = sNode;
            }
            break;

        case QLNC_NODE_TYPE_SUB_TABLE:
            STL_TRY( qlncSubQueryUnnest( aParamInfo,
                                         ((qlncSubTableNode*)sNode)->mQueryNode,
                                         &sOutNode,
                                         aEnv )
                     == STL_SUCCESS );

            if( sOutNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE )
            {
                sNode = sOutNode;
                if( ((qlncSubTableNode*)sNode)->mFilter != NULL )
                {
                    STL_TRY( qlncSQUTableFilter( aParamInfo,
                                                 sNode,
                                                 &sOutNode,
                                                 aEnv )
                             == STL_SUCCESS );
                }
            }
            else
            {
                ((qlncSubTableNode*)sNode)->mQueryNode = sOutNode;
                sOutNode = sNode;
            }
            break;

        case QLNC_NODE_TYPE_JOIN_TABLE:
            for( i = 0; i < ((qlncJoinTableNode*)sNode)->mNodeArray.mCurCnt; i++ )
            {
                STL_TRY( qlncSubQueryUnnest(
                             aParamInfo,
                             ((qlncJoinTableNode*)sNode)->mNodeArray.mNodeArr[i],
                             &sOutNode,
                             aEnv )
                         == STL_SUCCESS );
                ((qlncJoinTableNode*)sNode)->mNodeArray.mNodeArr[i] = sOutNode;
            }

            if( (((qlncJoinTableNode*)sNode)->mJoinType == QLNC_JOIN_TYPE_INNER) &&
                ( ( (((qlncJoinTableNode*)sNode)->mJoinCond != NULL) &&
                    (((qlncJoinTableNode*)sNode)->mJoinCond->mOrCount > 0) ) ||
                  ( (((qlncJoinTableNode*)sNode)->mFilter != NULL) &&
                    (((qlncJoinTableNode*)sNode)->mFilter->mOrCount > 0) ) ) )
            {
                STL_TRY( qlncSQUTableFilter( aParamInfo,
                                             sNode,
                                             &sOutNode,
                                             aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                sOutNode = sNode;
            }
            break;

        case QLNC_NODE_TYPE_FLAT_INSTANT:
            STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
            break;

        case QLNC_NODE_TYPE_SORT_INSTANT:
            STL_TRY( qlncSubQueryUnnest( aParamInfo,
                                         ((qlncSortInstantNode*)sNode)->mChildNode,
                                         &sOutNode,
                                         aEnv )
                     == STL_SUCCESS );
            ((qlncSortInstantNode*)sNode)->mChildNode = sOutNode;
            sOutNode = sNode;
            break;

        case QLNC_NODE_TYPE_HASH_INSTANT:
            STL_TRY( qlncSubQueryUnnest( aParamInfo,
                                         ((qlncHashInstantNode*)sNode)->mChildNode,
                                         &sOutNode,
                                         aEnv )
                     == STL_SUCCESS );
            ((qlncHashInstantNode*)sNode)->mChildNode = sOutNode;
            sOutNode = sNode;
            break;

        default:
            STL_DASSERT( 0 );
            break;
    }


    STL_RAMP( RAMP_FINISH );


    STL_DASSERT( sOutNode != NULL );

    /* Output 설정 */
    *aOutNode = sOutNode;


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      "qlncSubQueryUnnest()" );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Table Node에 대하여 SubQuery를 Join 형태로 Conversion 한다.
 * @param[in]   aParamInfo  Parameter Info
 * @param[in]   aNode       Candidate Node
 * @param[out]  aOutNode    Output Candidate Node
 * @param[in]   aEnv        Environment
 */
stlStatus qlncSQUTableFilter( qlncParamInfo     * aParamInfo,
                              qlncNodeCommon    * aNode,
                              qlncNodeCommon   ** aOutNode,
                              qllEnv            * aEnv )
{
    stlInt32              i;
    stlInt32              j;
    qlncNodeCommon      * sOutNode      = NULL;
    qlncExprListFunc    * sListFunc     = NULL;
    qlncExprCommon      * sLeftExpr     = NULL;
    qlncExprSubQuery    * sSubQuery     = NULL;
    qlncQuerySpec       * sQuerySpec    = NULL;

    qlncNodeArray         sNodeArray;
    knlBuiltInFunc        sCurrFilterFuncId;
    qlncJoinType          sSetJoinType;
    knlBuiltInFunc        sSetFuncId;
    qlncJoinOperType      sSetJoinOperType;

    stlInt32              sInnerTableColumnCount;
    stlInt32              sOuterTableColumnCount;
    stlBool               sHasAggregation;

    qlncAndFilter       * sAndFilter            = NULL;
    qlncAndFilter      ** sAndFilterPtr         = NULL;
    qlncAndFilter         sOuterAndFilter;

    qlncAndFilter       * sSemiJoinAndFilter    = NULL;
    qlncJoinTableNode   * sSemiJoinTable        = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode != NULL, QLL_ERROR_STACK(aEnv) );


    /* Join Table인 경우 Inner Join이어야만 한다. */
    if( aNode->mNodeType == QLNC_NODE_TYPE_JOIN_TABLE )
    {
        if( ((qlncJoinTableNode*)aNode)->mJoinType != QLNC_JOIN_TYPE_INNER )
        {
            sOutNode = aNode;
            STL_THROW( RAMP_FINISH );
        }
    }

    /* And Filter 추출 */
    STL_TRY( qlncSQUExtractAndFilter( aParamInfo,
                                      aNode,
                                      &sAndFilter,
                                      &sAndFilterPtr,
                                      aEnv )
             == STL_SUCCESS );

    STL_DASSERT( sAndFilter != NULL );

    /* Semi-Join으로 풀 수 있는 Filter가 존재하는지 검사 */
    for( i = 0; i < sAndFilter->mOrCount; i++ )
    {
        /* Filter가 1개이어야 하고(Or가 없어야 하고) Filter가 Boolean Filter이어야 한다. */
        STL_TRY_THROW( (sAndFilter->mOrFilters[i]->mFilterCount == 1) &&
                       (sAndFilter->mOrFilters[i]->mFilters[0]->mType == QLNC_EXPR_TYPE_BOOLEAN_FILTER),
                       RAMP_SKIP_FILTER );

        /* List Function이어야 한다. */
        STL_TRY_THROW( ((qlncBooleanFilter*)(sAndFilter->mOrFilters[i]->mFilters[0]))->mExpr->mType == QLNC_EXPR_TYPE_LIST_FUNCTION,
                       RAMP_SKIP_FILTER );

        sListFunc = (qlncExprListFunc*)(((qlncBooleanFilter*)(sAndFilter->mOrFilters[i]->mFilters[0]))->mExpr);

        /* List Function ID에 따른 검사 */
        switch( sListFunc->mFuncId )
        {
            case KNL_BUILTIN_FUNC_IN:
            case KNL_BUILTIN_FUNC_NOT_IN:
                /**
                 * 1. ArgCount가 2개이어야 하며,
                 * 2. 첫번째(우측연산자) List Column의 ArgCount가 1개 이어야 하며,
                 * 3. Row에 존재하는 ArgCount도 1개 이어야 하고,
                 * 4. Row에 존재하는 Arg가 SubQuery이어야 한다.
                 */

                STL_TRY_THROW( (sListFunc->mArgCount == 2) &&
                               (((qlncExprListCol*)(sListFunc->mArgs[0]))->mArgCount == 1) &&
                               (((qlncExprRowExpr*)(((qlncExprListCol*)(sListFunc->mArgs[0]))->mArgs[0]))->mArgCount == 1) &&
                               (((qlncExprRowExpr*)(((qlncExprListCol*)(sListFunc->mArgs[0]))->mArgs[0]))->mArgs[0]->mType == QLNC_EXPR_TYPE_SUB_QUERY),
                               RAMP_SKIP_FILTER );

                if( ((qlncExprRowExpr*)(((qlncExprListCol*)(sListFunc->mArgs[1]))->mArgs[0]))->mArgCount == 1 )
                {
                    sLeftExpr = ((qlncExprRowExpr*)(((qlncExprListCol*)(sListFunc->mArgs[1]))->mArgs[0]))->mArgs[0];
                }
                else
                {
                    sLeftExpr = ((qlncExprListCol*)(sListFunc->mArgs[1]))->mArgs[0];
                }

                sSubQuery = (qlncExprSubQuery*)(((qlncExprRowExpr*)(((qlncExprListCol*)(sListFunc->mArgs[0]))->mArgs[0]))->mArgs[0]);

                sCurrFilterFuncId = sListFunc->mFuncId;

                break;
            case KNL_BUILTIN_FUNC_EXISTS:
            case KNL_BUILTIN_FUNC_NOT_EXISTS:
                /**
                 * @todo Exists Function에 대한 구현필요
                 */
            default:
                STL_THROW( RAMP_SKIP_FILTER );
                break;
        }

        /* SubQuery의 Node는 Query Spec이어야 한다. (Set 연산자가 아니어야 한다.) */
        STL_TRY_THROW( sSubQuery->mNode->mNodeType == QLNC_NODE_TYPE_QUERY_SPEC,
                       RAMP_SKIP_FILTER );

        sQuerySpec = (qlncQuerySpec*)(sSubQuery->mNode);

        /* SubQuery의 Query Spec에는 Offset과 Limit이 없어야 한다. */
        STL_TRY_THROW( (sQuerySpec->mOffset == NULL) && (sQuerySpec->mLimit == NULL),
                       RAMP_SKIP_FILTER );

        /* SubQuery의 Query Spec에 존재하는 Hint를 체크한다. */
        STL_TRY_THROW( sQuerySpec->mQueryTransformHint->mAllowedQueryTransform == STL_TRUE,
                       RAMP_SKIP_FILTER );
        STL_TRY_THROW( sQuerySpec->mQueryTransformHint->mAllowedUnnest == STL_TRUE,
                       RAMP_SKIP_FILTER );

        /* SubQuery의 Query Spec Node에서 Child Node는 Table Node이어야 하며,
         * (Order By, Group By는 안된다.) */
        STL_TRY_THROW( (sQuerySpec->mChildNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE) ||
                       (sQuerySpec->mChildNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE) ||
                       (sQuerySpec->mChildNode->mNodeType == QLNC_NODE_TYPE_JOIN_TABLE),
                       RAMP_SKIP_FILTER );


        /**
         * @todo Order By, Group By인 경우 구현필요
         */

        /**
         * SubQuery 내의 Node 수집
         */

        /* Node Array 초기화 */
        STL_TRY( qlncInitializeNodeArray( &QLL_CANDIDATE_MEM( aEnv ),
                                          &sNodeArray,
                                          aEnv )
                 == STL_SUCCESS );

        /* Node Array 생성 */
        STL_TRY( qlncMakeNodeArrayOnCurrNode( &sNodeArray,
                                              sSubQuery->mNode,
                                              aEnv )
                 == STL_SUCCESS );

        /* Left Expression에는 Outer Table에 대한 Column이 하나 이상이어야 한다. */
        if( sLeftExpr->mType == QLNC_EXPR_TYPE_ROW_EXPR )
        {
            for( j = 0; j < ((qlncExprRowExpr*)sLeftExpr)->mArgCount; j++ )
            {
                sInnerTableColumnCount = 0;
                sOuterTableColumnCount = 0;
                sHasAggregation = STL_FALSE;
                (void)qlncSQUGetExprInfoFromExpr( ((qlncExprRowExpr*)sLeftExpr)->mArgs[j],
                                                  &sNodeArray,
                                                  &sInnerTableColumnCount,
                                                  &sOuterTableColumnCount,
                                                  &sHasAggregation );

                STL_TRY_THROW( sOuterTableColumnCount > 0, RAMP_SKIP_FILTER );
            }
        }
        else
        {
            sInnerTableColumnCount = 0;
            sOuterTableColumnCount = 0;
            sHasAggregation = STL_FALSE;
            (void)qlncSQUGetExprInfoFromExpr( sLeftExpr,
                                              &sNodeArray,
                                              &sInnerTableColumnCount,
                                              &sOuterTableColumnCount,
                                              &sHasAggregation );

            STL_TRY_THROW( sOuterTableColumnCount > 0, RAMP_SKIP_FILTER );
        }

        /* SubQuery에서 Query Spec의 Child Node의 Filter를 제외하고는
         * Outer Table을 참조하는 Filter가 존재하면 안된다. */
        if( sQuerySpec->mChildNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE )
        {
            /* Do Nothing */
        }
        else if( sQuerySpec->mChildNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE )
        {
            STL_TRY_THROW( qlncIsExistOuterColumnOnCandNode(
                               ((qlncSubTableNode*)(sQuerySpec->mChildNode))->mQueryNode,
                               &sNodeArray ) == STL_FALSE,
                           RAMP_SKIP_FILTER );
        }
        else if( sQuerySpec->mChildNode->mNodeType == QLNC_NODE_TYPE_JOIN_TABLE )
        {
            for( j = 0; j < ((qlncJoinTableNode*)(sQuerySpec->mChildNode))->mNodeArray.mCurCnt; j++ )
            {
                STL_TRY_THROW( qlncIsExistOuterColumnOnCandNode(
                                   ((qlncJoinTableNode*)(sQuerySpec->mChildNode))->mNodeArray.mNodeArr[j],
                                   &sNodeArray ) == STL_FALSE,
                               RAMP_SKIP_FILTER );
            }
        }
        else
        {
            STL_DASSERT( 0 );
        }

        /* SubQuery의 Target에 대한 검사 */
        for( j = 0; j < sQuerySpec->mTargetCount; j++ )
        {
            sInnerTableColumnCount = 0;
            sOuterTableColumnCount = 0;
            sHasAggregation = STL_FALSE;
            (void)qlncSQUGetExprInfoFromExpr( sQuerySpec->mTargetArr[j].mExpr,
                                              &sNodeArray,
                                              &sInnerTableColumnCount,
                                              &sOuterTableColumnCount,
                                              &sHasAggregation );

            /* SubQuery의 Target은 반드시 Column이 하나 이상이어야 하며,
             * SubQuery의 Target은 Outer Table을 참조해서는 안된다. */
            STL_TRY_THROW( (sInnerTableColumnCount > 0) && (sOuterTableColumnCount == 0),
                           RAMP_SKIP_FILTER );

            /* Aggregation이 있으면 안된다. */
            STL_TRY_THROW( sHasAggregation == STL_FALSE, RAMP_SKIP_FILTER );
        }

        /* Semi-Join/Anti-Semi-Join으로 풀 수 있다고 판단됨 */
        break;

        STL_RAMP( RAMP_SKIP_FILTER );
    }

    /* Semi-Join/Anti-Semi-Join으로 풀 수 없는 경우 종료 */
    if( i >= sAndFilter->mOrCount )
    {
        (*sAndFilterPtr) = sAndFilter;
        sOutNode = aNode;
        STL_THROW( RAMP_FINISH );
    }


    /**
     * Semi-Join/Anti-Semi-Join으로 풀 수 있는 경우임
     */

    /* 원본 Filter 처리 */
    sAndFilter->mOrFilters[i] = NULL;
    STL_TRY( qlncCompactAndFilter( sAndFilter,
                                   aEnv )
             == STL_SUCCESS );

    if( sAndFilter->mOrCount == 0 )
    {
        sAndFilter = NULL;
    }

    (*sAndFilterPtr) = sAndFilter;

    /* Semi-Join/Anti-Semi-Join Filter 생성 */
    STL_TRY( qlncSQUExtractAndFilter( aParamInfo,
                                      sQuerySpec->mChildNode,
                                      &sAndFilter,
                                      &sAndFilterPtr,
                                      aEnv )
             == STL_SUCCESS );

    /* Outer Table을 참조하는 Filter만 찾아냄 */
    QLNC_INIT_AND_FILTER( &sOuterAndFilter );
    if( sAndFilter != NULL )
    {
        for( i = 0; i < sAndFilter->mOrCount; i++ )
        {
            if( qlncIsExistOuterColumnOnExpr( (qlncExprCommon*)(sAndFilter->mOrFilters[i]),
                                              &sNodeArray )
                == STL_TRUE )
            {
                STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                            &sOuterAndFilter,
                                                            1,
                                                            &(sAndFilter->mOrFilters[i]),
                                                            aEnv )
                         == STL_SUCCESS );
                sAndFilter->mOrFilters[i] = NULL;
            }
        }

        /* Outer Table을 참조하는 Filter가 존재하면 And Filter를 Compact */
        if( sOuterAndFilter.mOrCount > 0 )
        {
            STL_TRY( qlncCompactAndFilter( sAndFilter,
                                           aEnv )
                     == STL_SUCCESS );

            if( sAndFilter->mOrCount == 0 )
            {
                sAndFilter = NULL;
            }
        }
    }

    /* 그대로 남겨야 하는 And Filter를 다시 설정 */
    (*sAndFilterPtr) = sAndFilter;


    /**
     * Join Type 및 Join Operation Type, Function ID 설정
     */

    /* Join Hint에 의한 Join Operation Type 및 Join Type 설정 */
    switch( sQuerySpec->mQueryTransformHint->mUnnestOperType )
    {
        case QLNC_UNNEST_OPER_TYPE_DEFAULT:
            sSetJoinOperType = QLNC_JOIN_OPER_TYPE_DEFAULT;
            sSetJoinType = QLNC_JOIN_TYPE_INVALID;
            break;
        case QLNC_UNNEST_OPER_TYPE_NL_SJ:
            sSetJoinOperType = QLNC_JOIN_OPER_TYPE_USE_NESTED;
            sSetJoinType = QLNC_JOIN_TYPE_SEMI;
            break;
        case QLNC_UNNEST_OPER_TYPE_NL_AJ:
            sSetJoinOperType = QLNC_JOIN_OPER_TYPE_USE_NESTED;
            sSetJoinType = QLNC_JOIN_TYPE_ANTI_SEMI;
            break;
        case QLNC_UNNEST_OPER_TYPE_MERGE_SJ:
            sSetJoinOperType = QLNC_JOIN_OPER_TYPE_USE_MERGE;
            sSetJoinType = QLNC_JOIN_TYPE_SEMI;
            break;
        case QLNC_UNNEST_OPER_TYPE_MERGE_AJ:
            sSetJoinOperType = QLNC_JOIN_OPER_TYPE_USE_MERGE;
            sSetJoinType = QLNC_JOIN_TYPE_ANTI_SEMI;
            break;
        case QLNC_UNNEST_OPER_TYPE_HASH_SJ:
            sSetJoinOperType = QLNC_JOIN_OPER_TYPE_USE_HASH;
            sSetJoinType = QLNC_JOIN_TYPE_SEMI;
            break;
        case QLNC_UNNEST_OPER_TYPE_HASH_AJ:
            sSetJoinOperType = QLNC_JOIN_OPER_TYPE_USE_HASH;
            sSetJoinType = QLNC_JOIN_TYPE_ANTI_SEMI;
            break;
        default:
            STL_DASSERT( 0 );
            sSetJoinOperType = QLNC_JOIN_OPER_TYPE_DEFAULT;
            sSetJoinType = QLNC_JOIN_TYPE_INVALID;
            break;
    }

    /* Function에 따른 Join Type 및 Function ID 설정 */
    switch( sCurrFilterFuncId )
    {
        case KNL_BUILTIN_FUNC_IN:
            {
                sSetFuncId = KNL_BUILTIN_FUNC_IS_EQUAL;
                sSetJoinType = QLNC_JOIN_TYPE_SEMI;

                break;
            }
        case KNL_BUILTIN_FUNC_NOT_IN:
            {
                sSetFuncId = KNL_BUILTIN_FUNC_IS_EQUAL;
                sSetJoinType = QLNC_JOIN_TYPE_ANTI_SEMI;

                break;
            }
        default:
            {
                STL_DASSERT( 0 );

                sSetFuncId = KNL_BUILTIN_FUNC_INVALID;
                sSetJoinType = QLNC_JOIN_TYPE_INVALID;

                break;
            }
    }


    /**
     * Semi-Join/Anti-Semi-Join Filter 생성
     */

    STL_TRY( qlncSQUMakeSemiJoinAndFilter( aParamInfo,
                                           sListFunc,
                                           sLeftExpr,
                                           sQuerySpec->mTargetCount,
                                           sQuerySpec->mTargetArr,
                                           sSetFuncId,
                                           &sOuterAndFilter,
                                           &sSemiJoinAndFilter,
                                           aEnv )
             == STL_SUCCESS );


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
    sSemiJoinTable->mFilter = NULL;
    sSemiJoinTable->mJoinType = sSetJoinType;
    sSemiJoinTable->mJoinDirect = QLNC_JOIN_DIRECT_LEFT;

    /* Semi-Join Hint 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncJoinHint ),
                                (void**) &(sSemiJoinTable->mSemiJoinHint),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    QLNC_INIT_JOIN_HINT( sSemiJoinTable->mSemiJoinHint );

    /* Right Node의 Join Operation Type 변경 */
    if( sQuerySpec->mChildNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE )
    {
        ((qlncBaseTableNode*)(sQuerySpec->mChildNode))->mJoinHint->mJoinOperation = sSetJoinOperType;
    }
    else if( sQuerySpec->mChildNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE )
    {
        ((qlncSubTableNode*)(sQuerySpec->mChildNode))->mJoinHint->mJoinOperation = sSetJoinOperType;
    }
    else
    {
        STL_DASSERT( sQuerySpec->mChildNode->mNodeType == QLNC_NODE_TYPE_JOIN_TABLE );
        ((qlncJoinTableNode*)(sQuerySpec->mChildNode))->mSemiJoinHint->mJoinOperation = sSetJoinOperType;
    }

    /* Left Node 및 Right Node, OutNode를 설정 */
    if( (aNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE) ||
        (aNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE) )
    {
        STL_TRY( qlncAddNodeToNodeArray( &(sSemiJoinTable->mNodeArray),
                                         aNode,
                                         aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlncAddNodeToNodeArray( &(sSemiJoinTable->mNodeArray),
                                         sQuerySpec->mChildNode,
                                         aEnv )
                 == STL_SUCCESS );

        sOutNode = (qlncNodeCommon*)sSemiJoinTable;
    }
    else
    {
        qlncNodeArray     sSemiJoinNodeArray;

        STL_DASSERT( aNode->mNodeType == QLNC_NODE_TYPE_JOIN_TABLE );

        /* Join Node Array에 속한 Node들 중에서
         * Semi-Join/Anti-Semi-Join Filter에서 참조되는 Node들을 찾아
         * 새로운 Node Array를 생성
         */

        /* Node Array 초기화 */
        STL_TRY( qlncInitializeNodeArray( &QLL_CANDIDATE_MEM( aEnv ),
                                          &sSemiJoinNodeArray,
                                          aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlncExtractNodeForSemiJoinFromExpr( (qlncExprCommon*)sSemiJoinAndFilter,
                                                     &(((qlncJoinTableNode*)aNode)->mNodeArray),
                                                     &sNodeArray,
                                                     &sSemiJoinNodeArray,
                                                     aEnv )
                 == STL_SUCCESS );
        STL_DASSERT( sSemiJoinNodeArray.mCurCnt > 0 );

        if( ((qlncJoinTableNode*)aNode)->mNodeArray.mCurCnt == 0 )
        {
            /* Join Node의 모든 Child Node가 Semin Join으로 가야 하는 경우 */
            for( i = 0; i < sSemiJoinNodeArray.mCurCnt; i++ )
            {
                STL_TRY( qlncAddNodeToNodeArray( &(((qlncJoinTableNode*)aNode)->mNodeArray),
                                                 sSemiJoinNodeArray.mNodeArr[i],
                                                 aEnv )
                         == STL_SUCCESS );
            }

            STL_TRY( qlncAddNodeToNodeArray( &(sSemiJoinTable->mNodeArray),
                                             aNode,
                                             aEnv )
                     == STL_SUCCESS );

            STL_TRY( qlncAddNodeToNodeArray( &(sSemiJoinTable->mNodeArray),
                                             sQuerySpec->mChildNode,
                                             aEnv )
                     == STL_SUCCESS );

            sOutNode = (qlncNodeCommon*)sSemiJoinTable;
        }
        else
        {
            STL_DASSERT( ((qlncJoinTableNode*)aNode)->mNodeArray.mCurCnt > 0 );

            if( sSemiJoinNodeArray.mCurCnt == 1 )
            {
                STL_TRY( qlncAddNodeToNodeArray( &(sSemiJoinTable->mNodeArray),
                                                 sSemiJoinNodeArray.mNodeArr[0],
                                                 aEnv )
                         == STL_SUCCESS );

                STL_TRY( qlncAddNodeToNodeArray( &(sSemiJoinTable->mNodeArray),
                                                 sQuerySpec->mChildNode,
                                                 aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                qlncJoinTableNode   * sJoinTable    = NULL;

                STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                            STL_SIZEOF( qlncJoinTableNode ),
                                            (void**) &sJoinTable,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                QLNC_INIT_NODE_COMMON( &(sJoinTable->mNodeCommon),
                                       *(aParamInfo->mGlobalNodeID),
                                       QLNC_NODE_TYPE_JOIN_TABLE );
                (*(aParamInfo->mGlobalNodeID))++;  /* Global ID 증가 */

                STL_TRY( qlncInitializeNodeArray( &QLL_CANDIDATE_MEM( aEnv ),
                                                  &(sJoinTable->mNodeArray),
                                                  aEnv )
                         == STL_SUCCESS );
                sJoinTable->mJoinCond = NULL;
                sJoinTable->mFilter = NULL;
                sJoinTable->mJoinType = QLNC_JOIN_TYPE_INNER;
                sJoinTable->mJoinDirect = QLNC_JOIN_DIRECT_LEFT;

                for( i = 0; i < sSemiJoinNodeArray.mCurCnt; i++ )
                {
                    STL_TRY( qlncAddNodeToNodeArray( &(sJoinTable->mNodeArray),
                                                     sSemiJoinNodeArray.mNodeArr[i],
                                                     aEnv )
                             == STL_SUCCESS );
                }

                STL_TRY( qlncAddNodeToNodeArray( &(sSemiJoinTable->mNodeArray),
                                                 (qlncNodeCommon*)sJoinTable,
                                                 aEnv )
                         == STL_SUCCESS );

                STL_TRY( qlncAddNodeToNodeArray( &(sSemiJoinTable->mNodeArray),
                                                 sQuerySpec->mChildNode,
                                                 aEnv )
                         == STL_SUCCESS );
            }

            STL_TRY( qlncAddNodeToNodeArray( &(((qlncJoinTableNode*)aNode)->mNodeArray),
                                             (qlncNodeCommon*)sSemiJoinTable,
                                             aEnv )
                     == STL_SUCCESS );

            sOutNode = aNode;
        }
    }


    STL_RAMP( RAMP_FINISH );


    STL_DASSERT( sOutNode != NULL );

    /* Output 설정 */
    *aOutNode = sOutNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SubQuery Unnest를 위해 Node의 Filter와 Filter의 주소를 추출한다.
 * @param[in]   aParamInfo      Parameter Info
 * @param[in]   aNode           Candidate Node
 * @param[out]  aAndFilter      And Filter
 * @param[out]  aAndFilterPtr   And Filter의 주소
 * @param[in]   aEnv            Environment
 */
stlStatus qlncSQUExtractAndFilter( qlncParamInfo    * aParamInfo,
                                   qlncNodeCommon   * aNode,
                                   qlncAndFilter   ** aAndFilter,
                                   qlncAndFilter  *** aAndFilterPtr,
                                   qllEnv           * aEnv )
{
    qlncAndFilter   * sAndFilter    = NULL;
    qlncAndFilter  ** sAndFilterPtr = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode != NULL, QLL_ERROR_STACK(aEnv) );


    /* Node Type에 따라 And Filter 및 And Filter 주소 추출 */
    if( aNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE )
    {
        sAndFilter = ((qlncBaseTableNode*)aNode)->mFilter;
        sAndFilterPtr = &(((qlncBaseTableNode*)aNode)->mFilter);
        ((qlncBaseTableNode*)aNode)->mFilter = NULL;
    }
    else if( aNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE )
    {
        sAndFilter = ((qlncSubTableNode*)aNode)->mFilter;
        sAndFilterPtr = &(((qlncSubTableNode*)aNode)->mFilter);
        ((qlncSubTableNode*)aNode)->mFilter = NULL;
    }
    else
    {
        STL_DASSERT( aNode->mNodeType == QLNC_NODE_TYPE_JOIN_TABLE );

        if( (((qlncJoinTableNode*)aNode)->mJoinCond != NULL) &&
            (((qlncJoinTableNode*)aNode)->mJoinCond->mOrCount > 0) )
        {
            if( (((qlncJoinTableNode*)aNode)->mFilter != NULL) &&
                (((qlncJoinTableNode*)aNode)->mFilter->mOrCount > 0) )
            {
                STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                            STL_SIZEOF( qlncAndFilter ),
                                            (void**) &sAndFilter,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );
                QLNC_INIT_AND_FILTER( sAndFilter );

                STL_TRY( qlncAppendOrFilterToAndFilterTail(
                             aParamInfo,
                             sAndFilter,
                             ((qlncJoinTableNode*)aNode)->mJoinCond->mOrCount,
                             ((qlncJoinTableNode*)aNode)->mJoinCond->mOrFilters,
                             aEnv )
                         == STL_SUCCESS );

                STL_TRY( qlncAppendOrFilterToAndFilterTail(
                             aParamInfo,
                             sAndFilter,
                             ((qlncJoinTableNode*)aNode)->mFilter->mOrCount,
                             ((qlncJoinTableNode*)aNode)->mFilter->mOrFilters,
                             aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                sAndFilter = ((qlncJoinTableNode*)aNode)->mJoinCond;
            }
        }
        else
        {
            if( (((qlncJoinTableNode*)aNode)->mFilter != NULL) &&
                (((qlncJoinTableNode*)aNode)->mFilter->mOrCount > 0) )
            {
                sAndFilter = ((qlncJoinTableNode*)aNode)->mFilter;
            }
            else
            {
                sAndFilter = NULL;
            }
        }

        sAndFilterPtr = &(((qlncJoinTableNode*)aNode)->mJoinCond);
        ((qlncJoinTableNode*)aNode)->mJoinCond = NULL;
        ((qlncJoinTableNode*)aNode)->mFilter = NULL;
    }


    /**
     * Output 설정
     */

    *aAndFilter = sAndFilter;
    *aAndFilterPtr = sAndFilterPtr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Node에 대해서 Inner Table의 Column과 Outer Table의 Column 수, Aggregation 존재여부를 검사한다.
 * @param[in]   aNode                   Candidate Node
 * @param[in]   aNodeArray              Candidate Node Array
 * @param[out]  aInnerTableColumnCount  Inner Table Column Count
 * @param[out]  aOuterTableColumnCount  Outer Table Column Count
 * @param[out]  aHasAggregation         Aggregation의 존재여부
 */
void qlncSQUGetExprInfoFromNode( qlncNodeCommon * aNode,
                                 qlncNodeArray  * aNodeArray,
                                 stlInt32       * aInnerTableColumnCount,
                                 stlInt32       * aOuterTableColumnCount,
                                 stlBool        * aHasAggregation )
{
    stlInt32      sInnerTableColumnCount;
    stlInt32      sOuterTableColumnCount;
    stlBool       sHasAggregation;


    STL_DASSERT( aNode != NULL );
    STL_DASSERT( aNodeArray != NULL );

    sInnerTableColumnCount = 0;
    sOuterTableColumnCount = 0;
    sHasAggregation = STL_FALSE;

    switch( aNode->mNodeType )
    {
        case QLNC_NODE_TYPE_QUERY_SET:
            {
                (void)qlncSQUGetExprInfoFromNode( ((qlncQuerySet*)aNode)->mChildNode,
                                                  aNodeArray,
                                                  &sInnerTableColumnCount,
                                                  &sOuterTableColumnCount,
                                                  &sHasAggregation );
                break;
            }
        case QLNC_NODE_TYPE_SET_OP:
            {
                stlInt32      i;
                qlncSetOP   * sSetOP    = NULL;

                sSetOP = (qlncSetOP*)aNode;
                for( i = 0; i < sSetOP->mChildCount; i++ )
                {
                    (void)qlncSQUGetExprInfoFromNode( sSetOP->mChildNodeArr[i],
                                                      aNodeArray,
                                                      &sInnerTableColumnCount,
                                                      &sOuterTableColumnCount,
                                                      &sHasAggregation );
                }

                break;
            }
        case QLNC_NODE_TYPE_QUERY_SPEC:
            {
                stlInt32          i;
                qlncQuerySpec   * sQuerySpec    = NULL;

                sQuerySpec = (qlncQuerySpec*)aNode;
                for( i = 0; i < sQuerySpec->mTargetCount; i++ )
                {
                    (void)qlncSQUGetExprInfoFromExpr( sQuerySpec->mTargetArr[i].mExpr,
                                                      aNodeArray,
                                                      &sInnerTableColumnCount,
                                                      &sOuterTableColumnCount,
                                                      &sHasAggregation );
                }

                (void)qlncSQUGetExprInfoFromNode( sQuerySpec->mChildNode,
                                                  aNodeArray,
                                                  &sInnerTableColumnCount,
                                                  &sOuterTableColumnCount,
                                                  &sHasAggregation );

                break;
            }
        case QLNC_NODE_TYPE_BASE_TABLE:
            {
                qlncBaseTableNode   * sBaseTableNode    = NULL;

                sBaseTableNode = (qlncBaseTableNode*)aNode;
                if( sBaseTableNode->mFilter != NULL )
                {
                    (void)qlncSQUGetExprInfoFromExpr( (qlncExprCommon*)(sBaseTableNode->mFilter),
                                                      aNodeArray,
                                                      &sInnerTableColumnCount,
                                                      &sOuterTableColumnCount,
                                                      &sHasAggregation );
                }

                break;
            }
        case QLNC_NODE_TYPE_SUB_TABLE:
            {
                qlncSubTableNode    * sSubTableNode = NULL;

                sSubTableNode = (qlncSubTableNode*)aNode;
                if( sSubTableNode->mFilter != NULL )
                {
                    (void)qlncSQUGetExprInfoFromExpr( (qlncExprCommon*)(sSubTableNode->mFilter),
                                                      aNodeArray,
                                                      &sInnerTableColumnCount,
                                                      &sOuterTableColumnCount,
                                                      &sHasAggregation );
                }

                (void)qlncSQUGetExprInfoFromNode( sSubTableNode->mQueryNode,
                                                  aNodeArray,
                                                  &sInnerTableColumnCount,
                                                  &sOuterTableColumnCount,
                                                  &sHasAggregation );

                break;
            }
        case QLNC_NODE_TYPE_JOIN_TABLE:
            {
                stlInt32              i;
                qlncJoinTableNode   * sJoinTableNode    = NULL;

                sJoinTableNode = (qlncJoinTableNode*)aNode;
                if( sJoinTableNode->mJoinCond != NULL )
                {
                    (void)qlncSQUGetExprInfoFromExpr( (qlncExprCommon*)(sJoinTableNode->mJoinCond),
                                                      aNodeArray,
                                                      &sInnerTableColumnCount,
                                                      &sOuterTableColumnCount,
                                                      &sHasAggregation );
                }

                if( sJoinTableNode->mFilter != NULL )
                {
                    (void)qlncSQUGetExprInfoFromExpr( (qlncExprCommon*)(sJoinTableNode->mFilter),
                                                      aNodeArray,
                                                      &sInnerTableColumnCount,
                                                      &sOuterTableColumnCount,
                                                      &sHasAggregation );
                }

                for( i = 0; i < sJoinTableNode->mNodeArray.mCurCnt; i++ )
                {
                    (void)qlncSQUGetExprInfoFromNode( sJoinTableNode->mNodeArray.mNodeArr[i],
                                                      aNodeArray,
                                                      &sInnerTableColumnCount,
                                                      &sOuterTableColumnCount,
                                                      &sHasAggregation );
                }

                break;
            }
        case QLNC_NODE_TYPE_FLAT_INSTANT:
            {
                STL_DASSERT( 0 );
                break;
            }
        case QLNC_NODE_TYPE_SORT_INSTANT:
            {
                qlncSortInstantNode * sSortInstantNode  = NULL;

                sSortInstantNode = (qlncSortInstantNode*)aNode;
                if( sSortInstantNode->mFilter != NULL )
                {
                    (void)qlncSQUGetExprInfoFromExpr( (qlncExprCommon*)(sSortInstantNode->mFilter),
                                                      aNodeArray,
                                                      &sInnerTableColumnCount,
                                                      &sOuterTableColumnCount,
                                                      &sHasAggregation );
                }

                (void)qlncSQUGetExprInfoFromNode( sSortInstantNode->mChildNode,
                                                  aNodeArray,
                                                  &sInnerTableColumnCount,
                                                  &sOuterTableColumnCount,
                                                  &sHasAggregation );

                break;
            }
        case QLNC_NODE_TYPE_HASH_INSTANT:
            {
                qlncHashInstantNode * sHashInstantNode  = NULL;

                sHashInstantNode = (qlncHashInstantNode*)aNode;
                if( sHashInstantNode->mFilter != NULL )
                {
                    (void)qlncSQUGetExprInfoFromExpr( (qlncExprCommon*)(sHashInstantNode->mFilter),
                                                      aNodeArray,
                                                      &sInnerTableColumnCount,
                                                      &sOuterTableColumnCount,
                                                      &sHasAggregation );
                }

                (void)qlncSQUGetExprInfoFromNode( sHashInstantNode->mChildNode,
                                                  aNodeArray,
                                                  &sInnerTableColumnCount,
                                                  &sOuterTableColumnCount,
                                                  &sHasAggregation );

                break;
            }
        case QLNC_NODE_TYPE_INNER_JOIN_TABLE:
            {
                qlncInnerJoinTableNode  * sInnerJoinTableNode   = NULL;

                sInnerJoinTableNode = (qlncInnerJoinTableNode*)aNode;
                if( sInnerJoinTableNode->mJoinCondition != NULL )
                {
                    (void)qlncSQUGetExprInfoFromExpr( (qlncExprCommon*)(sInnerJoinTableNode->mJoinCondition),
                                                      aNodeArray,
                                                      &sInnerTableColumnCount,
                                                      &sOuterTableColumnCount,
                                                      &sHasAggregation );
                }

                if( sInnerJoinTableNode->mJoinFilter != NULL )
                {
                    (void)qlncSQUGetExprInfoFromExpr( (qlncExprCommon*)(sInnerJoinTableNode->mJoinFilter),
                                                      aNodeArray,
                                                      &sInnerTableColumnCount,
                                                      &sOuterTableColumnCount,
                                                      &sHasAggregation );
                }

                (void)qlncSQUGetExprInfoFromNode( sInnerJoinTableNode->mLeftNode,
                                                  aNodeArray,
                                                  &sInnerTableColumnCount,
                                                  &sOuterTableColumnCount,
                                                  &sHasAggregation );

                (void)qlncSQUGetExprInfoFromNode( sInnerJoinTableNode->mRightNode,
                                                  aNodeArray,
                                                  &sInnerTableColumnCount,
                                                  &sOuterTableColumnCount,
                                                  &sHasAggregation );

                break;
            }
        case QLNC_NODE_TYPE_OUTER_JOIN_TABLE:
            {
                qlncOuterJoinTableNode  * sOuterJoinTableNode   = NULL;

                sOuterJoinTableNode = (qlncOuterJoinTableNode*)aNode;
                if( sOuterJoinTableNode->mJoinCondition != NULL )
                {
                    (void)qlncSQUGetExprInfoFromExpr( (qlncExprCommon*)(sOuterJoinTableNode->mJoinCondition),
                                                      aNodeArray,
                                                      &sInnerTableColumnCount,
                                                      &sOuterTableColumnCount,
                                                      &sHasAggregation );
                }

                if( sOuterJoinTableNode->mJoinFilter != NULL )
                {
                    (void)qlncSQUGetExprInfoFromExpr( (qlncExprCommon*)(sOuterJoinTableNode->mJoinFilter),
                                                      aNodeArray,
                                                      &sInnerTableColumnCount,
                                                      &sOuterTableColumnCount,
                                                      &sHasAggregation );
                }

                (void)qlncSQUGetExprInfoFromNode( sOuterJoinTableNode->mLeftNode,
                                                  aNodeArray,
                                                  &sInnerTableColumnCount,
                                                  &sOuterTableColumnCount,
                                                  &sHasAggregation );

                (void)qlncSQUGetExprInfoFromNode( sOuterJoinTableNode->mRightNode,
                                                  aNodeArray,
                                                  &sInnerTableColumnCount,
                                                  &sOuterTableColumnCount,
                                                  &sHasAggregation );

                break;
            }
        default:
            STL_DASSERT( 0 );
            break;
    }

    /* Output 설정 */
    (*aInnerTableColumnCount) += sInnerTableColumnCount;
    (*aOuterTableColumnCount) += sOuterTableColumnCount;
    if( (*aHasAggregation) == STL_FALSE )
    {
        (*aHasAggregation) = sHasAggregation;
    }
}


/**
 * @brief Expression에 대해서 Inner Table의 Column과 Outer Table의 Column 수, Aggregation 존재여부를 검사한다.
 * @param[in]   aExpr                   Expression
 * @param[in]   aNodeArray              Candidate Node Array
 * @param[out]  aInnerTableColumnCount  Inner Table Column Count
 * @param[out]  aOuterTableColumnCount  Outer Table Column Count
 * @param[out]  aHasAggregation         Aggregation의 존재여부
 */
void qlncSQUGetExprInfoFromExpr( qlncExprCommon * aExpr,
                                 qlncNodeArray  * aNodeArray,
                                 stlInt32       * aInnerTableColumnCount,
                                 stlInt32       * aOuterTableColumnCount,
                                 stlBool        * aHasAggregation )
{
    stlInt32      sInnerTableColumnCount;
    stlInt32      sOuterTableColumnCount;
    stlBool       sHasAggregation;

#define _QLNC_CHECK_COLUMN_COUNT( _aNode )                                  \
    {                                                                       \
        stlInt32      _i;                                                   \
        for( _i = 0; _i < aNodeArray->mCurCnt; _i++ )                       \
        {                                                                   \
            if( aNodeArray->mNodeArr[_i]->mNodeID == (_aNode)->mNodeID )    \
            {                                                               \
                break;                                                      \
            }                                                               \
        }                                                                   \
                                                                            \
        if( _i < aNodeArray->mCurCnt )                                      \
        {                                                                   \
            sInnerTableColumnCount++;                                       \
        }                                                                   \
        else                                                                \
        {                                                                   \
            sOuterTableColumnCount++;                                       \
        }                                                                   \
    }

    STL_DASSERT( aExpr != NULL );
    STL_DASSERT( aNodeArray != NULL );

    sInnerTableColumnCount = 0;
    sOuterTableColumnCount = 0;
    sHasAggregation = STL_FALSE;
    switch( aExpr->mType )
    {
        case QLNC_EXPR_TYPE_VALUE:
        case QLNC_EXPR_TYPE_BIND_PARAM:
        case QLNC_EXPR_TYPE_PSEUDO_COLUMN:
        case QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT:
            {
                break;
            }
        case QLNC_EXPR_TYPE_COLUMN:
            {
                _QLNC_CHECK_COLUMN_COUNT( ((qlncExprColumn*)aExpr)->mNode );
                break;
            }
        case QLNC_EXPR_TYPE_FUNCTION:
            {
                qlncExprFunction    * sCandFunc = (qlncExprFunction*)aExpr;
                stlInt32              i;

                sHasAggregation = STL_FALSE;
                for( i = 0; i < sCandFunc->mArgCount; i++ )
                {
                    (void)qlncSQUGetExprInfoFromExpr( sCandFunc->mArgs[i],
                                                      aNodeArray,
                                                      &sInnerTableColumnCount,
                                                      &sOuterTableColumnCount,
                                                      &sHasAggregation );
                }

                break;
            }
        case QLNC_EXPR_TYPE_CAST:
            {
                (void)qlncSQUGetExprInfoFromExpr( ((qlncExprTypeCast*)aExpr)->mArgs[0],
                                                  aNodeArray,
                                                  &sInnerTableColumnCount,
                                                  &sOuterTableColumnCount,
                                                  &sHasAggregation );

                break;
            }
        case QLNC_EXPR_TYPE_SUB_QUERY:
            {
                (void)qlncSQUGetExprInfoFromNode( ((qlncExprSubQuery*)aExpr)->mNode,
                                                  aNodeArray,
                                                  &sInnerTableColumnCount,
                                                  &sOuterTableColumnCount,
                                                  &sHasAggregation );

                sHasAggregation = STL_FALSE;

                break;
            }
        case QLNC_EXPR_TYPE_REFERENCE:
            {
                (void)qlncSQUGetExprInfoFromExpr( ((qlncExprReference*)aExpr)->mOrgExpr,
                                                  aNodeArray,
                                                  &sInnerTableColumnCount,
                                                  &sOuterTableColumnCount,
                                                  &sHasAggregation );

                break;
            }
        case QLNC_EXPR_TYPE_INNER_COLUMN:
            {
                if( (((qlncExprInnerColumn*)aExpr)->mNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE) ||
                    (((qlncExprInnerColumn*)aExpr)->mNode->mNodeType == QLNC_NODE_TYPE_SORT_INSTANT) ||
                    (((qlncExprInnerColumn*)aExpr)->mNode->mNodeType == QLNC_NODE_TYPE_HASH_INSTANT) )
                {
                    _QLNC_CHECK_COLUMN_COUNT( ((qlncExprInnerColumn*)aExpr)->mNode );
                }
                else
                {
                    STL_DASSERT( ((qlncExprInnerColumn*)aExpr)->mOrgExpr != NULL );
                    (void)qlncSQUGetExprInfoFromExpr( ((qlncExprInnerColumn*)aExpr)->mOrgExpr,
                                                      aNodeArray,
                                                      &sInnerTableColumnCount,
                                                      &sOuterTableColumnCount,
                                                      &sHasAggregation );
                }
                break;
            }
        case QLNC_EXPR_TYPE_OUTER_COLUMN:
            {
                (void)qlncSQUGetExprInfoFromExpr( ((qlncExprOuterColumn*)aExpr)->mOrgExpr,
                                                  aNodeArray,
                                                  &sInnerTableColumnCount,
                                                  &sOuterTableColumnCount,
                                                  &sHasAggregation );
                break;
            }
        case QLNC_EXPR_TYPE_ROWID_COLUMN:
            {
                _QLNC_CHECK_COLUMN_COUNT( ((qlncExprRowidColumn*)aExpr)->mNode );
                break;
            }
        case QLNC_EXPR_TYPE_AGGREGATION:
            {
                qlncExprAggregation * sCandAggregation  = (qlncExprAggregation*)aExpr;
                stlInt32              i;

                for( i = 0; i < sCandAggregation->mArgCount; i++ )
                {
                    (void)qlncSQUGetExprInfoFromExpr( sCandAggregation->mArgs[i],
                                                      aNodeArray,
                                                      &sInnerTableColumnCount,
                                                      &sOuterTableColumnCount,
                                                      &sHasAggregation );
                }

                sHasAggregation = STL_TRUE;

                break;
            }
        case QLNC_EXPR_TYPE_CASE_EXPR:
            {
                qlncExprCaseExpr    * sCandCaseExpr = (qlncExprCaseExpr*)aExpr;
                stlInt32              i;

                for( i = 0; i < sCandCaseExpr->mCount; i++ )
                {
                    (void)qlncSQUGetExprInfoFromExpr( sCandCaseExpr->mWhenArr[i],
                                                      aNodeArray,
                                                      &sInnerTableColumnCount,
                                                      &sOuterTableColumnCount,
                                                      &sHasAggregation );

                    (void)qlncSQUGetExprInfoFromExpr( sCandCaseExpr->mThenArr[i],
                                                      aNodeArray,
                                                      &sInnerTableColumnCount,
                                                      &sOuterTableColumnCount,
                                                      &sHasAggregation );
                }

                (void)qlncSQUGetExprInfoFromExpr( sCandCaseExpr->mDefResult,
                                                  aNodeArray,
                                                  &sInnerTableColumnCount,
                                                  &sOuterTableColumnCount,
                                                  &sHasAggregation );

                break;
            }
        case QLNC_EXPR_TYPE_LIST_FUNCTION:
            {
                qlncExprListFunc    * sCandListFunc = (qlncExprListFunc*)aExpr;
                stlInt32              i;

                for( i = 0; i < sCandListFunc->mArgCount; i++ )
                {
                    (void)qlncSQUGetExprInfoFromExpr( sCandListFunc->mArgs[i],
                                                      aNodeArray,
                                                      &sInnerTableColumnCount,
                                                      &sOuterTableColumnCount,
                                                      &sHasAggregation );
                }

                break;
            }
        case QLNC_EXPR_TYPE_LIST_COLUMN:
            {
                qlncExprListCol * sCandListCol  = (qlncExprListCol*)aExpr;
                stlInt32          i;

                for( i = 0; i < sCandListCol->mArgCount; i++ )
                {
                    (void)qlncSQUGetExprInfoFromExpr( sCandListCol->mArgs[i],
                                                      aNodeArray,
                                                      &sInnerTableColumnCount,
                                                      &sOuterTableColumnCount,
                                                      &sHasAggregation );
                }

                break;
            }
        case QLNC_EXPR_TYPE_ROW_EXPR:
            {
                qlncExprRowExpr * sCandRowExpr  = (qlncExprRowExpr*)aExpr;
                stlInt32          i;

                for( i = 0; i < sCandRowExpr->mArgCount; i++ )
                {
                    (void)qlncSQUGetExprInfoFromExpr( sCandRowExpr->mArgs[i],
                                                      aNodeArray,
                                                      &sInnerTableColumnCount,
                                                      &sOuterTableColumnCount,
                                                      &sHasAggregation );
                }

                break;
            }
        case QLNC_EXPR_TYPE_PSEUDO_ARGUMENT:
        case QLNC_EXPR_TYPE_AND_FILTER:
        case QLNC_EXPR_TYPE_OR_FILTER:
        case QLNC_EXPR_TYPE_BOOLEAN_FILTER:
        case QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER:
        default:
            STL_DASSERT( 0 );
            break;
    }

    /* Output 설정 */
    (*aInnerTableColumnCount) += sInnerTableColumnCount;
    (*aOuterTableColumnCount) += sOuterTableColumnCount;
    if( (*aHasAggregation) == STL_FALSE )
    {
        (*aHasAggregation) = sHasAggregation;
    }
}


/**
 * @brief Semi-Join/Anti-Semi-Join의 And Filter를 만든다.
 * @param[in]   aParamInfo          Parameter Info
 * @param[in]   aListFunc           List Function Expression
 * @param[in]   aLeftExpr           Left Expression
 * @param[in]   aRightTargetCount   Right Target Count
 * @param[in]   aRightTargetArr     Right Target Array
 * @param[in]   aSetFuncId          Function Id
 * @param[in]   aOuterAndFilter     Outer And Filter
 * @param[out]  aSemiJoinAndFilter  Semi-Join/Anti-Semi-Join And Filter
 * @param[in]   aEnv                Environment
 */
stlStatus qlncSQUMakeSemiJoinAndFilter( qlncParamInfo       * aParamInfo,
                                        qlncExprListFunc    * aListFunc,
                                        qlncExprCommon      * aLeftExpr,
                                        stlInt32              aRightTargetCount,
                                        qlncTarget          * aRightTargetArr,
                                        knlBuiltInFunc        aSetFuncId,
                                        qlncAndFilter       * aOuterAndFilter,
                                        qlncAndFilter      ** aSemiJoinAndFilter,
                                        qllEnv              * aEnv )
{
    stlInt32              i;
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
    STL_PARAM_VALIDATE( aLeftExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRightTargetCount > 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRightTargetArr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSetFuncId != KNL_BUILTIN_FUNC_INVALID, QLL_ERROR_STACK(aEnv) );


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

#define _QLNC_MAKE_BOOLEAN_FILTER( _aFuncId,                                                \
                                   _aExpr,                                                  \
                                   _aPossibleJoinCond,                                      \
                                   _aLeftExpr,                                              \
                                   _aRightExpr,                                             \
                                   _aBooleanFilter )                                        \
    {                                                                                       \
        /* Boolean Filter 생성 */                                                           \
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),                             \
                                    STL_SIZEOF( qlncBooleanFilter ),                        \
                                    (void**) &(_aBooleanFilter),                            \
                                    KNL_ENV(aEnv) )                                         \
                 == STL_SUCCESS );                                                          \
                                                                                            \
        /* Common Expression 정보 설정 */                                                   \
        sTmpValue = NULL;                                                                   \
        QLNC_SET_EXPR_COMMON( &(_aBooleanFilter)->mCommon,                                  \
                              &QLL_CANDIDATE_MEM( aEnv ),                                   \
                              aEnv,                                                         \
                              QLNC_EXPR_TYPE_BOOLEAN_FILTER,                                \
                              QLNC_EXPR_PHRASE_WHERE,                                       \
                              DTL_ITERATION_TIME_FOR_EACH_EXPR,                             \
                              0,                                                            \
                              sTmpValue,                                                    \
                              DTL_TYPE_BOOLEAN );                                           \
                                                                                            \
        (_aBooleanFilter)->mFuncID = (_aFuncId);                                            \
        (_aBooleanFilter)->mExpr = (qlncExprCommon*)(_aExpr);                               \
        QLNC_INIT_LIST( &((_aBooleanFilter)->mRefNodeList) );                               \
                                                                                            \
        (_aBooleanFilter)->mPossibleJoinCond = (_aPossibleJoinCond);                        \
        (_aBooleanFilter)->mIsPhysicalFilter = STL_FALSE;                                   \
        (_aBooleanFilter)->mSubQueryExprList = NULL;                                        \
                                                                                            \
        sConvertExprParamInfo.mParamInfo = *aParamInfo;                                     \
        sConvertExprParamInfo.mTableMapArr = NULL;                                          \
        sConvertExprParamInfo.mOuterTableMapArr = NULL;                                     \
        sConvertExprParamInfo.mInitExpr = NULL;                                             \
        sConvertExprParamInfo.mCandExpr = NULL;                                             \
        sConvertExprParamInfo.mExprPhraseFlag = 0;                                          \
        sConvertExprParamInfo.mSubQueryExprList = NULL;                                     \
        sConvertExprParamInfo.mExprSubQuery = NULL;                                         \
                                                                                            \
        if( (_aPossibleJoinCond) == STL_TRUE )                                              \
        {                                                                                   \
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),                         \
                                        STL_SIZEOF( qlncRefNodeList ),                      \
                                        (void**) &((_aBooleanFilter)->mLeftRefNodeList),    \
                                        KNL_ENV(aEnv) )                                     \
                     == STL_SUCCESS );                                                      \
            QLNC_INIT_LIST( (_aBooleanFilter)->mLeftRefNodeList );                          \
                                                                                            \
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),                         \
                                        STL_SIZEOF( qlncRefNodeList ),                      \
                                        (void**) &((_aBooleanFilter)->mRightRefNodeList),   \
                                        KNL_ENV(aEnv) )                                     \
                     == STL_SUCCESS );                                                      \
            QLNC_INIT_LIST( (_aBooleanFilter)->mRightRefNodeList );                         \
                                                                                            \
            sConvertExprParamInfo.mRefNodeList = (_aBooleanFilter)->mLeftRefNodeList;       \
            STL_TRY( qlncFindAddRefColumnToRefNodeList( &sConvertExprParamInfo,             \
                                                        (_aLeftExpr),                       \
                                                        aEnv )                              \
                     == STL_SUCCESS );                                                      \
                                                                                            \
            sConvertExprParamInfo.mRefNodeList = (_aBooleanFilter)->mRightRefNodeList;      \
            STL_TRY( qlncFindAddRefColumnToRefNodeList( &sConvertExprParamInfo,             \
                                                        (_aRightExpr),                      \
                                                        aEnv )                              \
                     == STL_SUCCESS );                                                      \
                                                                                            \
            sConvertExprParamInfo.mRefNodeList = &((_aBooleanFilter)->mRefNodeList);        \
                                                                                            \
            /* Append Left Reference Node List */                                           \
            sRefNodeItem = (_aBooleanFilter)->mLeftRefNodeList->mHead;                      \
            while( sRefNodeItem != NULL )                                                   \
            {                                                                               \
                sRefColumnItem = sRefNodeItem->mRefColumnList.mHead;                        \
                while( sRefColumnItem != NULL )                                             \
                {                                                                           \
                    STL_TRY( qlncAddRefColumnToRefNodeList( &sConvertExprParamInfo,         \
                                                            sRefColumnItem->mExpr,          \
                                                            aEnv )                          \
                             == STL_SUCCESS );                                              \
                                                                                            \
                    sRefColumnItem = sRefColumnItem->mNext;                                 \
                }                                                                           \
                                                                                            \
                sRefNodeItem = sRefNodeItem->mNext;                                         \
            }                                                                               \
                                                                                            \
            /* Append Right Reference Node List */                                          \
            sRefNodeItem = (_aBooleanFilter)->mRightRefNodeList->mHead;                     \
            while( sRefNodeItem != NULL )                                                   \
            {                                                                               \
                sRefColumnItem = sRefNodeItem->mRefColumnList.mHead;                        \
                while( sRefColumnItem != NULL )                                             \
                {                                                                           \
                    STL_TRY( qlncAddRefColumnToRefNodeList( &sConvertExprParamInfo,         \
                                                            sRefColumnItem->mExpr,          \
                                                            aEnv )                          \
                             == STL_SUCCESS );                                              \
                                                                                            \
                    sRefColumnItem = sRefColumnItem->mNext;                                 \
                }                                                                           \
                                                                                            \
                sRefNodeItem = sRefNodeItem->mNext;                                         \
            }                                                                               \
        }                                                                                   \
        else                                                                                \
        {                                                                                   \
            (_aBooleanFilter)->mLeftRefNodeList = NULL;                                     \
            (_aBooleanFilter)->mRightRefNodeList = NULL;                                    \
                                                                                            \
            /* Reference Node List 설정 */                                                  \
            sConvertExprParamInfo.mRefNodeList = &((_aBooleanFilter)->mRefNodeList);        \
            STL_TRY( qlncFindAddRefColumnToRefNodeList( &sConvertExprParamInfo,             \
                                                        (qlncExprCommon*)(_aExpr),          \
                                                        aEnv )                              \
                     == STL_SUCCESS );                                                      \
        }                                                                                   \
    }


    /* Semi Jin And Filter 생성 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncAndFilter ),
                                (void**) &sSemiJoinAndFilter,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_INIT_AND_FILTER( sSemiJoinAndFilter );

    /* Left Expression과 Right Target으로 Filter 만들기 */
    if( aRightTargetCount == 1 )
    {
        STL_DASSERT( aLeftExpr->mType != QLNC_EXPR_TYPE_ROW_EXPR );

        /* Function 만들기  */
        _QLNC_MAKE_FUNCTION( aLeftExpr, aRightTargetArr[0].mExpr, aSetFuncId, sFunction );

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
                                 aRightTargetArr[i].mExpr,
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

        for( i = 0; i < aRightTargetCount; i++ )
        {
            sRowExpr->mArgs[i] = aRightTargetArr[i].mExpr; 
        }

        /* List Function의 List Function Type 재설정 */
        STL_DASSERT( (aSetFuncId == KNL_BUILTIN_FUNC_IS_LESS_THAN_ROW) ||
                     (aSetFuncId == KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL_ROW) ||
                     (aSetFuncId == KNL_BUILTIN_FUNC_IS_GREATER_THAN_ROW) ||
                     (aSetFuncId == KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL_ROW) );

        aListFunc->mFuncId = aSetFuncId;

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

#endif 

/** @} qlnc */
