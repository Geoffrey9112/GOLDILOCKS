/*******************************************************************************
 * qlncCommon.c
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
 * @file qlncCommon.c
 * @brief SQL Processor Layer SELECT statement Common
 */

#include <qll.h>
#include <qlDef.h>



/**
 * @addtogroup qlnc
 * @{
 */


/**
 * @brief Table Node에서 처리 가능한 Or Filter인지 판별한다.
 * @param[in]   aTableNode  Candidate Table Node
 * @param[in]   aOrFilter   Or Filter
 */
stlBool qlncIsUsableOrFilterOnTableNode( qlncNodeCommon * aTableNode,
                                         qlncOrFilter   * aOrFilter )
{
    stlBool               sResult;
    stlInt32              i;


    STL_DASSERT( aTableNode != NULL );
    STL_DASSERT( aOrFilter != NULL );
    STL_DASSERT( aOrFilter->mFilterCount > 0 );


    sResult = STL_TRUE;

    for( i = 0; i < aOrFilter->mFilterCount; i++ )
    {
        sResult = qlncIsUsableFilterOnTableNode( aTableNode,
                                                 aOrFilter->mFilters[i] );
        if( sResult == STL_FALSE )
        {
            break;
        }
    }


    return sResult;
}


/**
 * @brief Table Node에서 처리 가능한 Boolean Filter인지 판별한다.
 * @param[in]   aTableNode      Candidate Table Node
 * @param[in]   aFilter         Filter
 */
stlBool qlncIsUsableFilterOnTableNode( qlncNodeCommon    * aTableNode,
                                       qlncExprCommon    * aFilter )
{
    stlBool               sResult;
    qlncRefNodeItem     * sRefNodeItem  = NULL;


    STL_DASSERT( aTableNode != NULL );
    STL_DASSERT( aFilter != NULL );

    if( aFilter->mType == QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER )
    {
        sResult = STL_TRUE;
    }
    else
    {
        STL_DASSERT( aFilter->mType == QLNC_EXPR_TYPE_BOOLEAN_FILTER );
        STL_DASSERT( ((qlncBooleanFilter*)aFilter)->mRefNodeList.mCount > 0 );
        sRefNodeItem = ((qlncBooleanFilter*)aFilter)->mRefNodeList.mHead;

        if( (aTableNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE) ||
            (aTableNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE) )
        {
            if( (sRefNodeItem->mNext == NULL) &&
                (sRefNodeItem->mNode->mNodeID == aTableNode->mNodeID) )
            {
                sResult = STL_TRUE;
            }
            else
            {
                sResult = STL_FALSE;
            }

            STL_THROW( RAMP_FINISH );
        }

        STL_DASSERT( aTableNode->mNodeType == QLNC_NODE_TYPE_JOIN_TABLE );

        sResult = STL_TRUE;
        while( sRefNodeItem != NULL )
        {
            sResult = qlncIsExistNode( aTableNode,
                                       sRefNodeItem->mNode );
            if( sResult == STL_FALSE )
            {
                STL_THROW( RAMP_FINISH );
            }

            sRefNodeItem = sRefNodeItem->mNext;
        }
    }

    STL_RAMP( RAMP_FINISH );


    return sResult;
}


/**
 * @brief 현재의 Candidate Node Tree에 Node가 존재하는지 판별한다.
 * @param[in]   aSrcNode    Source Candidate Node
 * @param[in]   aNode       Candidate Node
 */
stlBool qlncIsExistNode( qlncNodeCommon     * aSrcNode,
                         qlncNodeCommon     * aNode )
{
    stlBool               sResult;
    stlInt32              i;
    qlncNodeArray       * sNodeArr      = NULL;


    STL_DASSERT( aSrcNode != NULL );
    STL_DASSERT( aNode != NULL );


    sResult = STL_FALSE;

    if( (aSrcNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE) ||
        (aSrcNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE) )
    {
        if( aSrcNode->mNodeID == aNode->mNodeID )
        {
            sResult = STL_TRUE;
        }
        else
        {
            sResult = STL_FALSE;
        }
    }
    else if( aSrcNode->mNodeType == QLNC_NODE_TYPE_JOIN_TABLE )
    {
        sNodeArr = &(((qlncJoinTableNode*)aSrcNode)->mNodeArray);

        for( i = 0; i < sNodeArr->mCurCnt; i++ )
        {
            if( sNodeArr->mNodeArr[i]->mNodeID == aNode->mNodeID )
            {
                sResult = STL_TRUE;
                break;
            }
            else
            {
                sResult = qlncIsExistNode( sNodeArr->mNodeArr[i],
                                           aNode );
                if( sResult == STL_TRUE )
                {
                    break;
                }
            }
        }
    }
    else if( aSrcNode->mNodeType == QLNC_NODE_TYPE_INNER_JOIN_TABLE )
    {
        sResult = qlncIsExistNode( ((qlncInnerJoinTableNode*)aSrcNode)->mLeftNode,
                                   aNode );

        if( sResult == STL_FALSE )
        {
            sResult = qlncIsExistNode( ((qlncInnerJoinTableNode*)aSrcNode)->mRightNode,
                                       aNode );
        }
    }
    else if( aSrcNode->mNodeType == QLNC_NODE_TYPE_OUTER_JOIN_TABLE )
    {
        sResult = qlncIsExistNode( ((qlncOuterJoinTableNode*)aSrcNode)->mLeftNode,
                                   aNode );

        if( sResult == STL_FALSE )
        {
            sResult = qlncIsExistNode( ((qlncOuterJoinTableNode*)aSrcNode)->mRightNode,
                                       aNode );
        }
    }
    else if( aSrcNode->mNodeType == QLNC_NODE_TYPE_SEMI_JOIN_TABLE )
    {
        sResult = qlncIsExistNode( ((qlncSemiJoinTableNode*)aSrcNode)->mLeftNode,
                                   aNode );

        if( sResult == STL_FALSE )
        {
            /* @todo 여기에 들어오는 경우가 현재 없는 것으로 판단됨 */

            sResult = qlncIsExistNode( ((qlncSemiJoinTableNode*)aSrcNode)->mRightNode,
                                       aNode );
        }
    }
    else if( aSrcNode->mNodeType == QLNC_NODE_TYPE_ANTI_SEMI_JOIN_TABLE )
    {
        sResult = qlncIsExistNode( ((qlncAntiSemiJoinTableNode*)aSrcNode)->mLeftNode,
                                   aNode );

        if( sResult == STL_FALSE )
        {
            sResult = qlncIsExistNode( ((qlncAntiSemiJoinTableNode*)aSrcNode)->mRightNode,
                                       aNode );
        }
    }
    else if( aSrcNode->mNodeType == QLNC_NODE_TYPE_SORT_INSTANT )
    {
        if( aSrcNode->mNodeID == aNode->mNodeID )
        {
            sResult = STL_TRUE;
        }
        else
        {
            sResult = qlncIsExistNode( ((qlncSortInstantNode*)aSrcNode)->mChildNode,
                                       aNode );
        }
    }
    else if( aSrcNode->mNodeType == QLNC_NODE_TYPE_HASH_INSTANT )
    {
        if( aSrcNode->mNodeID == aNode->mNodeID )
        {
            sResult = STL_TRUE;
        }
        else
        {
            sResult = qlncIsExistNode( ((qlncHashInstantNode*)aSrcNode)->mChildNode,
                                       aNode );
        }
    }
    else if( aSrcNode->mNodeType == QLNC_NODE_TYPE_QUERY_SET )
    {
        /* @todo 여기에 들어오는 경우가 현재 없는 것으로 판단됨 */

        if( aSrcNode->mNodeID == aNode->mNodeID )
        {
            sResult = STL_TRUE;
        }
        else
        {
            sResult = qlncIsExistNode( ((qlncQuerySet*)aSrcNode)->mChildNode,
                                       aNode );
        }
    }
    else if( aSrcNode->mNodeType == QLNC_NODE_TYPE_SET_OP )
    {
        /* @todo 여기에 들어오는 경우가 현재 없는 것으로 판단됨 */

        if( aSrcNode->mNodeID == aNode->mNodeID )
        {
            sResult = STL_TRUE;
        }
        else
        {
            for( i = 0; i < ((qlncSetOP*)aSrcNode)->mChildCount; i++ )
            {
                sResult = qlncIsExistNode( ((qlncSetOP*)aSrcNode)->mChildNodeArr[i],
                                           aNode );

                if( sResult == STL_TRUE )
                {
                    break;
                }
            }
        }
    }
    else if( aSrcNode->mNodeType == QLNC_NODE_TYPE_QUERY_SPEC )
    {
        if( aSrcNode->mNodeID == aNode->mNodeID )
        {
            /* @todo 여기에 들어오는 경우가 현재 없는 것으로 판단됨 */

            sResult = STL_TRUE;
        }
        else
        {
            sResult = qlncIsExistNode( ((qlncQuerySpec*)aSrcNode)->mChildNode,
                                       aNode );
        }
    }
    else
    {
        STL_DASSERT( 0 );
    }


    return sResult;
}


/**
 * @brief 현재의 Candidate Plan Node Tree에 Node가 존재하는지 판별한다.
 * @param[in]   aPlanNode   Candidate Plan Node
 * @param[in]   aCandNode   Candidate Node
 */
stlBool qlncIsExistCandNodeOnPlanNode( qlncPlanCommon   * aPlanNode,
                                       qlncNodeCommon   * aCandNode )
{
    stlBool       sIsExist;
    stlInt32      i;


    STL_DASSERT( aPlanNode != NULL );

    /* 현재노드와 일치여부 검사 */
    if( (aPlanNode->mPlanType != QLNC_PLAN_TYPE_SUB_QUERY_FILTER) &&
        (aPlanNode->mPlanType != QLNC_PLAN_TYPE_SUB_QUERY_LIST) )
    {
        if( aPlanNode->mCandNode->mNodeID == aCandNode->mNodeID )
        {
            return STL_TRUE;
        }
    }

    sIsExist = STL_FALSE;
    switch( aPlanNode->mPlanType )
    {
        case QLNC_PLAN_TYPE_TABLE_SCAN:
        case QLNC_PLAN_TYPE_INDEX_SCAN:
        case QLNC_PLAN_TYPE_INDEX_COMBINE:
        case QLNC_PLAN_TYPE_ROWID_SCAN:
        case QLNC_PLAN_TYPE_SUB_TABLE:
            sIsExist = STL_FALSE;

            break;

        case QLNC_PLAN_TYPE_NESTED_JOIN:
            sIsExist = qlncIsExistCandNodeOnPlanNode(
                           ((qlncPlanNestedJoin*)aPlanNode)->mLeftChildPlanNode,
                           aCandNode );

            if( sIsExist == STL_FALSE )
            {
                sIsExist = qlncIsExistCandNodeOnPlanNode(
                               ((qlncPlanNestedJoin*)aPlanNode)->mRightChildPlanNode,
                               aCandNode );
            }

            break;

        case QLNC_PLAN_TYPE_MERGE_JOIN:
            sIsExist = qlncIsExistCandNodeOnPlanNode(
                           ((qlncPlanMergeJoin*)aPlanNode)->mLeftChildPlanNode,
                           aCandNode );

            if( sIsExist == STL_FALSE )
            {
                sIsExist = qlncIsExistCandNodeOnPlanNode(
                               ((qlncPlanMergeJoin*)aPlanNode)->mRightChildPlanNode,
                               aCandNode );
            }

            break;

        case QLNC_PLAN_TYPE_HASH_JOIN:
            sIsExist = qlncIsExistCandNodeOnPlanNode(
                           ((qlncPlanHashJoin*)aPlanNode)->mLeftChildPlanNode,
                           aCandNode );

            if( sIsExist == STL_FALSE )
            {
                sIsExist = qlncIsExistCandNodeOnPlanNode(
                               ((qlncPlanHashJoin*)aPlanNode)->mRightChildPlanNode,
                               aCandNode );
            }

            break;

        case QLNC_PLAN_TYPE_JOIN_COMBINE:
            for( i = 0; i < ((qlncPlanJoinCombine*)aPlanNode)->mChildCount; i++ )
            {
                sIsExist = qlncIsExistCandNodeOnPlanNode(
                               ((qlncPlanJoinCombine*)aPlanNode)->mChildPlanArr[i],
                               aCandNode );

                if( sIsExist == STL_TRUE )
                {
                    break;
                }
            }

            break;

        case QLNC_PLAN_TYPE_SORT_INSTANT:
        case QLNC_PLAN_TYPE_SORT_GROUP_INSTANT:
        case QLNC_PLAN_TYPE_SORT_JOIN_INSTANT:
            sIsExist = qlncIsExistCandNodeOnPlanNode(
                           ((qlncPlanSortInstant*)aPlanNode)->mChildPlanNode,
                           aCandNode );

            break;

        case QLNC_PLAN_TYPE_HASH_INSTANT:
        case QLNC_PLAN_TYPE_HASH_GROUP_INSTANT:
        case QLNC_PLAN_TYPE_HASH_JOIN_INSTANT:
        case QLNC_PLAN_TYPE_GROUP:
            sIsExist = qlncIsExistCandNodeOnPlanNode(
                           ((qlncPlanHashInstant*)aPlanNode)->mChildPlanNode,
                           aCandNode );

            break;

        case QLNC_PLAN_TYPE_SUB_QUERY_LIST:
            sIsExist = qlncIsExistCandNodeOnPlanNode(
                           ((qlncPlanSubQueryList*)aPlanNode)->mChildPlanNode,
                           aCandNode );

            break;

        case QLNC_PLAN_TYPE_SUB_QUERY_FILTER:
            sIsExist = qlncIsExistCandNodeOnPlanNode(
                           ((qlncPlanSubQueryFilter*)aPlanNode)->mChildPlanNodes[ 0 ],
                           aCandNode );

            break;

        /* case QLNC_PLAN_TYPE_QUERY_SET: */
        /* case QLNC_PLAN_TYPE_SET_OP: */
        /* case QLNC_PLAN_TYPE_QUERY_SPEC: */
        default:
            STL_DASSERT( 0 );
            break;
    }

    return sIsExist;
}


/**
 * @brief And Filter에서 Outer Column이 사용되는지 체크한다.
 * @param[in]   aExpr       Expression
 * @param[in]   aNodeArray  Candidate Node Array
 */
stlBool qlncIsExistOuterColumnOnExpr( qlncExprCommon    * aExpr,
                                      qlncNodeArray     * aNodeArray )
{
    stlBool       sResult = STL_FALSE;
    stlInt32      i;


    if( aExpr == NULL )
    {
        return STL_FALSE;
    }

    switch( aExpr->mType )
    {
        case QLNC_EXPR_TYPE_VALUE:
        case QLNC_EXPR_TYPE_BIND_PARAM:
        case QLNC_EXPR_TYPE_PSEUDO_COLUMN:
        case QLNC_EXPR_TYPE_PSEUDO_ARGUMENT:
            {
                sResult = STL_FALSE;
                break;
            }
        case QLNC_EXPR_TYPE_COLUMN:
            {
                for( i = 0; i < aNodeArray->mCurCnt; i++ )
                {
                    if( aNodeArray->mNodeArr[i]->mNodeID == ((qlncExprColumn*)aExpr)->mNode->mNodeID )
                    {
                        break;
                    }
                }

                if( i < aNodeArray->mCurCnt )
                {
                    sResult = STL_FALSE;
                }
                else
                {
                    sResult = STL_TRUE;
                }

                break;
            }
        case QLNC_EXPR_TYPE_ROWID_COLUMN:
            {
                for( i = 0; i < aNodeArray->mCurCnt; i++ )
                {
                    if( aNodeArray->mNodeArr[i]->mNodeID == ((qlncExprRowidColumn*)aExpr)->mNode->mNodeID )
                    {
                        break;
                    }
                }

                if( i < aNodeArray->mCurCnt )
                {
                    sResult = STL_FALSE;
                }
                else
                {
                    sResult = STL_TRUE;
                }

                break;
            }
        case QLNC_EXPR_TYPE_FUNCTION:
            {
                qlncExprFunction    * sFunction = NULL;

                sFunction = (qlncExprFunction*)aExpr;
                for( i = 0; i < sFunction->mArgCount; i++ )
                {
                    sResult = qlncIsExistOuterColumnOnExpr( sFunction->mArgs[i],
                                                            aNodeArray );
                    if( sResult == STL_TRUE )
                    {
                        break;
                    }
                }

                break;
            }
        case QLNC_EXPR_TYPE_CAST:
            {
                qlncExprTypeCast    * sTypeCast = NULL;

                sTypeCast = (qlncExprTypeCast*)aExpr;
                for( i = 0; i < sTypeCast->mArgCount; i++ )
                {
                    sResult = qlncIsExistOuterColumnOnExpr( sTypeCast->mArgs[i],
                                                            aNodeArray );
                    if( sResult == STL_TRUE )
                    {
                        break;
                    }
                }

                break;
            }
        case QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT:
            {
                sResult = qlncIsExistOuterColumnOnExpr( ((qlncExprConstExpr*)aExpr)->mOrgExpr,
                                                        aNodeArray );
                break;
            }
        case QLNC_EXPR_TYPE_SUB_QUERY:
            {
                sResult = qlncIsExistOuterColumnOnCandNode( ((qlncExprSubQuery*)aExpr)->mNode,
                                                            aNodeArray );
                break;
            }
        case QLNC_EXPR_TYPE_REFERENCE:
            {
                /* @todo 현재 여기로 들어올 수 없음 */

                sResult = qlncIsExistOuterColumnOnExpr( ((qlncExprReference*)aExpr)->mOrgExpr,
                                                        aNodeArray );
                break;
            }
        case QLNC_EXPR_TYPE_INNER_COLUMN:
            {
                for( i = 0; i < aNodeArray->mCurCnt; i++ )
                {
                    if( aNodeArray->mNodeArr[i]->mNodeID == ((qlncExprInnerColumn*)aExpr)->mNode->mNodeID )
                    {
                        break;
                    }
                }

                if( i < aNodeArray->mCurCnt )
                {
                    sResult = qlncIsExistOuterColumnOnExpr( ((qlncExprInnerColumn*)aExpr)->mOrgExpr,
                                                            aNodeArray );
                }
                else
                {
                    sResult = STL_TRUE;
                }

                break;
            }
        case QLNC_EXPR_TYPE_OUTER_COLUMN:
            {
                /* @todo 현재 여기로 들어올 수 없음 */

                sResult = STL_TRUE;
                break;
            }
        case QLNC_EXPR_TYPE_AGGREGATION:
            {
                qlncExprAggregation * sAggregation  = NULL;

                sAggregation = (qlncExprAggregation*)aExpr;
                for( i = 0; i < sAggregation->mArgCount; i++ )
                {
                    sResult = qlncIsExistOuterColumnOnExpr( sAggregation->mArgs[i],
                                                            aNodeArray );
                    if( sResult == STL_TRUE )
                    {
                        break;
                    }
                }

                if( sResult == STL_FALSE )
                {
                    if( sAggregation->mFilter != NULL )
                    {
                        /* @todo Aggregation Filter는 향후 구현한다. */
                        sResult = qlncIsExistOuterColumnOnExpr( sAggregation->mFilter,
                                                                aNodeArray );
                    }
                }

                break;
            }
        case QLNC_EXPR_TYPE_CASE_EXPR:
            {
                qlncExprCaseExpr    * sCaseExpr = NULL;

                sCaseExpr = (qlncExprCaseExpr*)aExpr;
                for( i = 0; i < sCaseExpr->mCount; i++ )
                {
                    sResult = qlncIsExistOuterColumnOnExpr( sCaseExpr->mWhenArr[i],
                                                            aNodeArray );
                    if( sResult == STL_TRUE )
                    {
                        break;
                    }

                    sResult = qlncIsExistOuterColumnOnExpr( sCaseExpr->mThenArr[i],
                                                            aNodeArray );
                    if( sResult == STL_TRUE )
                    {
                        break;
                    }
                }

                if( sResult == STL_FALSE )
                {
                    sResult = qlncIsExistOuterColumnOnExpr( sCaseExpr->mDefResult,
                                                            aNodeArray );
                }

                break;
            }
        case QLNC_EXPR_TYPE_LIST_FUNCTION:
            {
                qlncExprListFunc    * sListFunc = NULL;

                sListFunc = (qlncExprListFunc*)aExpr;
                for( i = 0; i < sListFunc->mArgCount; i++ )
                {
                    sResult = qlncIsExistOuterColumnOnExpr( sListFunc->mArgs[i],
                                                            aNodeArray );
                    if( sResult == STL_TRUE )
                    {
                        break;
                    }
                }

                break;
            }
        case QLNC_EXPR_TYPE_LIST_COLUMN:
            {
                qlncExprListCol * sListCol  = NULL;

                sListCol = (qlncExprListCol*)aExpr;
                for( i = 0; i < sListCol->mArgCount; i++ )
                {
                    sResult = qlncIsExistOuterColumnOnExpr( sListCol->mArgs[i],
                                                            aNodeArray );
                    if( sResult == STL_TRUE )
                    {
                        break;
                    }
                }

                break;
            }
        case QLNC_EXPR_TYPE_ROW_EXPR:
            {
                qlncExprRowExpr * sRowExpr  = NULL;

                sRowExpr = (qlncExprRowExpr*)aExpr;
                for( i = 0; i < sRowExpr->mArgCount; i++ )
                {
                    sResult = qlncIsExistOuterColumnOnExpr( sRowExpr->mArgs[i],
                                                            aNodeArray );
                    if( sResult == STL_TRUE )
                    {
                        break;
                    }
                }

                break;
            }
        case QLNC_EXPR_TYPE_AND_FILTER:
            {
                qlncAndFilter   * sAndFilter    = NULL;

                sAndFilter = (qlncAndFilter*)aExpr;
                for( i = 0; i < sAndFilter->mOrCount; i++ )
                {
                    sResult = qlncIsExistOuterColumnOnExpr( (qlncExprCommon*)(sAndFilter->mOrFilters[i]),
                                                            aNodeArray );
                    if( sResult == STL_TRUE )
                    {
                        break;
                    }
                }

                break;
            }
        case QLNC_EXPR_TYPE_OR_FILTER:
            {
                qlncOrFilter    * sOrFilter = NULL;

                sOrFilter = (qlncOrFilter*)aExpr;
                for( i = 0; i < sOrFilter->mFilterCount; i++ )
                {
                    sResult = qlncIsExistOuterColumnOnExpr( sOrFilter->mFilters[i],
                                                            aNodeArray );
                    if( sResult == STL_TRUE )
                    {
                        break;
                    }
                }

                break;
            }
        case QLNC_EXPR_TYPE_BOOLEAN_FILTER:
            {
                sResult = qlncIsExistOuterColumnOnExpr( ((qlncBooleanFilter*)aExpr)->mExpr,
                                                        aNodeArray );
                break;
            }
        case QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER:
            {
                sResult = qlncIsExistOuterColumnOnExpr( ((qlncConstBooleanFilter*)aExpr)->mExpr,
                                                        aNodeArray );
                break;
            }
        default:
            {
                STL_DASSERT( 0 );
                sResult = STL_FALSE;
                break;
            }
    }

    return sResult;
}


/**
 * @brief Candidate Node에서 Outer Column이 사용되는지 체크한다.
 * @param[in]   aNode       Candidate Node
 * @param[in]   aNodeArray  Candidate Node Array
 */
stlBool qlncIsExistOuterColumnOnCandNode( qlncNodeCommon    * aNode,
                                          qlncNodeArray     * aNodeArray )
{
    stlBool       sResult = STL_FALSE;
    stlInt32      i;


#define _QLNC_CHECK_REFERENCE_COLUMN_LIST( _aRefColList, _aResult )     \
    {                                                                   \
        qlncRefExprItem * _sRefExprItem = NULL;                         \
                                                                        \
        if( (_aRefColList) != NULL )                                    \
        {                                                               \
            _sRefExprItem = (_aRefColList)->mHead;                      \
            while( _sRefExprItem != NULL )                              \
            {                                                           \
                (_aResult) = qlncIsExistOuterColumnOnExpr(              \
                                 _sRefExprItem->mExpr,                  \
                                 aNodeArray );                          \
                                                                        \
                STL_TRY_THROW( (_aResult) == STL_FALSE, RAMP_FINISH );  \
                                                                        \
                _sRefExprItem = _sRefExprItem->mNext;                   \
            }                                                           \
        }                                                               \
    }

#define _QLNC_CHECK_AND_FILTER( _aAndFilter, _aResult )             \
    {                                                               \
        if( (_aAndFilter) != NULL )                                 \
        {                                                           \
            (_aResult) = qlncIsExistOuterColumnOnExpr(              \
                             (qlncExprCommon*)(_aAndFilter),        \
                             aNodeArray );                          \
                                                                    \
            STL_TRY_THROW( (_aResult) == STL_FALSE, RAMP_FINISH );  \
        }                                                           \
    }


    switch( aNode->mNodeType )
    {
        case QLNC_NODE_TYPE_QUERY_SET:
            {
                sResult = qlncIsExistOuterColumnOnCandNode(
                              ((qlncQuerySet*)aNode)->mChildNode,
                              aNodeArray );
                break;
            }
        case QLNC_NODE_TYPE_SET_OP:
            {
                qlncSetOP   * sSetOP    = NULL;

                sSetOP = (qlncSetOP*)aNode;
                for( i = 0; i < sSetOP->mChildCount; i++ )
                {
                    sResult = qlncIsExistOuterColumnOnCandNode(
                                  sSetOP->mChildNodeArr[i],
                                  aNodeArray );

                    STL_TRY_THROW( sResult == STL_FALSE, RAMP_FINISH );
                }

                break;
            }
        case QLNC_NODE_TYPE_QUERY_SPEC:
            {
                qlncQuerySpec   * sQuerySpec    = NULL;

                sResult = STL_FALSE;
                sQuerySpec = (qlncQuerySpec*)aNode;
                for( i = 0; i < sQuerySpec->mTargetCount; i++ )
                {
                    sResult = qlncIsExistOuterColumnOnExpr(
                                  sQuerySpec->mTargetArr[i].mExpr,
                                  aNodeArray );

                    STL_TRY_THROW( sResult == STL_FALSE, RAMP_FINISH );
                }

                sResult = qlncIsExistOuterColumnOnCandNode(
                              sQuerySpec->mChildNode,
                              aNodeArray );

                break;
            }
        case QLNC_NODE_TYPE_BASE_TABLE:
            {
                if( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_TABLE_SCAN )
                {
                    _QLNC_CHECK_AND_FILTER( ((qlncTableScanCost*)(aNode->mBestCost))->mTableAndFilter, sResult );
                }
                else if( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_INDEX_SCAN )
                {
                    _QLNC_CHECK_AND_FILTER( ((qlncIndexScanCost*)(aNode->mBestCost))->mIndexRangeAndFilter, sResult );
                    _QLNC_CHECK_AND_FILTER( ((qlncIndexScanCost*)(aNode->mBestCost))->mIndexKeyAndFilter, sResult );
                    _QLNC_CHECK_AND_FILTER( ((qlncIndexScanCost*)(aNode->mBestCost))->mTableAndFilter, sResult );
                }
                else if( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_ROWID_SCAN )
                {
                    _QLNC_CHECK_AND_FILTER( ((qlncRowidScanCost*)(aNode->mBestCost))->mRowidAndFilter, sResult );
                    _QLNC_CHECK_AND_FILTER( ((qlncRowidScanCost*)(aNode->mBestCost))->mTableAndFilter, sResult );
                }
                else
                {
                    STL_DASSERT( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_INDEX_COMBINE );
                    for( i = 0; i < ((qlncIndexCombineCost*)(aNode->mBestCost))->mDnfAndFilterCount; i++ )
                    {
                        _QLNC_CHECK_AND_FILTER( ((qlncIndexCombineCost*)(aNode->mBestCost))->mIndexScanCostArr[i].mIndexRangeAndFilter, sResult );
                        _QLNC_CHECK_AND_FILTER( ((qlncIndexCombineCost*)(aNode->mBestCost))->mIndexScanCostArr[i].mIndexKeyAndFilter, sResult );
                        _QLNC_CHECK_AND_FILTER( ((qlncIndexCombineCost*)(aNode->mBestCost))->mIndexScanCostArr[i].mTableAndFilter, sResult );

                        STL_TRY_THROW( sResult == STL_FALSE, RAMP_FINISH );
                    }
                }

                break;
            }
        case QLNC_NODE_TYPE_SUB_TABLE:
            {
                if( ((qlncFilterCost*)(aNode->mBestCost))->mMaterializedNode == NULL )
                {
                    _QLNC_CHECK_AND_FILTER( ((qlncFilterCost*)(aNode->mBestCost))->mFilter, sResult );

                    sResult = qlncIsExistOuterColumnOnCandNode(
                                  ((qlncSubTableNode*)aNode)->mQueryNode,
                                  aNodeArray );
                }
                else
                {
                    /* @todo 여기에 들어오기 위한 테스트 케이스를 만들기 어려움 */
                    sResult = qlncIsExistOuterColumnOnCandNode(
                                  ((qlncFilterCost*)(aNode->mBestCost))->mMaterializedNode,
                                  aNodeArray );
                }

                break;
            }
        case QLNC_NODE_TYPE_JOIN_TABLE:
            {
                if( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_INNER_JOIN )
                {
                    sResult = qlncIsExistOuterColumnOnCandNode(
                                  (qlncNodeCommon*)(((qlncJoinTableNode*)aNode)->mBestInnerJoinTable),
                                  aNodeArray );
                }
                else if( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_OUTER_JOIN )
                {
                    sResult = qlncIsExistOuterColumnOnCandNode(
                                  (qlncNodeCommon*)(((qlncJoinTableNode*)aNode)->mBestOuterJoinTable),
                                  aNodeArray );
                }
                else if( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_SEMI_JOIN )
                {
                    sResult = qlncIsExistOuterColumnOnCandNode(
                                  (qlncNodeCommon*)(((qlncJoinTableNode*)aNode)->mBestSemiJoinTable),
                                  aNodeArray );
                }
                else
                {

                    STL_DASSERT( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_ANTI_SEMI_JOIN );
                    sResult = qlncIsExistOuterColumnOnCandNode(
                                  (qlncNodeCommon*)(((qlncJoinTableNode*)aNode)->mBestAntiSemiJoinTable),
                                  aNodeArray );
                }

                break;
            }
        case QLNC_NODE_TYPE_INNER_JOIN_TABLE:
        case QLNC_NODE_TYPE_OUTER_JOIN_TABLE:
        case QLNC_NODE_TYPE_SEMI_JOIN_TABLE:
        case QLNC_NODE_TYPE_ANTI_SEMI_JOIN_TABLE:
            {
                if( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_NESTED_LOOPS )
                {
                    _QLNC_CHECK_AND_FILTER( ((qlncNestedJoinCost*)(aNode->mBestCost))->mJoinCondition, sResult );
                    _QLNC_CHECK_AND_FILTER( ((qlncNestedJoinCost*)(aNode->mBestCost))->mJoinFilter, sResult );
                    sResult = qlncIsExistOuterColumnOnCandNode(
                                  ((qlncNestedJoinCost*)(aNode->mBestCost))->mLeftNode,
                                  aNodeArray );
                    STL_TRY_THROW( sResult == STL_FALSE, RAMP_FINISH );

                    sResult = qlncIsExistOuterColumnOnCandNode(
                                  ((qlncNestedJoinCost*)(aNode->mBestCost))->mRightNode,
                                  aNodeArray );
                }
                else if( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_MERGE )
                {
                    _QLNC_CHECK_AND_FILTER( ((qlncMergeJoinCost*)(aNode->mBestCost))->mEquiJoinCondition, sResult );
                    _QLNC_CHECK_AND_FILTER( ((qlncMergeJoinCost*)(aNode->mBestCost))->mNonEquiJoinCondition, sResult );
                    _QLNC_CHECK_AND_FILTER( ((qlncMergeJoinCost*)(aNode->mBestCost))->mJoinFilter, sResult );
                    sResult = qlncIsExistOuterColumnOnCandNode(
                                  ((qlncMergeJoinCost*)(aNode->mBestCost))->mLeftNode,
                                  aNodeArray );
                    STL_TRY_THROW( sResult == STL_FALSE, RAMP_FINISH );

                    sResult = qlncIsExistOuterColumnOnCandNode(
                                  ((qlncMergeJoinCost*)(aNode->mBestCost))->mRightNode,
                                  aNodeArray );
                }
                else if( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_HASH )
                {
                    _QLNC_CHECK_AND_FILTER( ((qlncHashJoinCost*)(aNode->mBestCost))->mEquiJoinCondition, sResult );
                    _QLNC_CHECK_AND_FILTER( ((qlncHashJoinCost*)(aNode->mBestCost))->mNonEquiJoinCondition, sResult );
                    _QLNC_CHECK_AND_FILTER( ((qlncHashJoinCost*)(aNode->mBestCost))->mJoinFilter, sResult );
                    sResult = qlncIsExistOuterColumnOnCandNode(
                                  ((qlncHashJoinCost*)(aNode->mBestCost))->mLeftNode,
                                  aNodeArray );
                    STL_TRY_THROW( sResult == STL_FALSE, RAMP_FINISH );

                    sResult = qlncIsExistOuterColumnOnCandNode(
                                  ((qlncHashJoinCost*)(aNode->mBestCost))->mRightNode,
                                  aNodeArray );
                }
                else
                {
                    STL_DASSERT( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_JOIN_COMBINE );
                    for( i = 0; i < ((qlncJoinCombineCost*)(aNode->mBestCost))->mDnfAndFilterCount; i++ )
                    {
                        sResult = qlncIsExistOuterColumnOnCandNode(
                                      ((qlncJoinCombineCost*)(aNode->mBestCost))->mJoinNodePtrArr[i],
                                      aNodeArray );
                        STL_TRY_THROW( sResult == STL_FALSE, RAMP_FINISH );
                    }
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
                _QLNC_CHECK_REFERENCE_COLUMN_LIST( ((qlncSortInstantNode*)aNode)->mKeyColList, sResult );
                _QLNC_CHECK_REFERENCE_COLUMN_LIST( ((qlncSortInstantNode*)aNode)->mRecColList, sResult );

                _QLNC_CHECK_AND_FILTER( ((qlncSortInstantNode*)aNode)->mFilter, sResult );

                sResult = qlncIsExistOuterColumnOnCandNode(
                              ((qlncSortInstantNode*)aNode)->mChildNode,
                              aNodeArray );

                break;
            }
        case QLNC_NODE_TYPE_HASH_INSTANT:
            {
                _QLNC_CHECK_REFERENCE_COLUMN_LIST( ((qlncHashInstantNode*)aNode)->mKeyColList, sResult );
                _QLNC_CHECK_REFERENCE_COLUMN_LIST( ((qlncHashInstantNode*)aNode)->mRecColList, sResult );

                _QLNC_CHECK_AND_FILTER( ((qlncHashInstantNode*)aNode)->mFilter, sResult );

                sResult = qlncIsExistOuterColumnOnCandNode(
                              ((qlncHashInstantNode*)aNode)->mChildNode,
                              aNodeArray );

                break;
            }
        default:
            STL_DASSERT( 0 );
            sResult = STL_FALSE;
            break;
    }


    STL_RAMP( RAMP_FINISH );


    return sResult;
}


/**
 * @brief And Filter에서 Reference Node에 대한 Outer Column이 사용되는지 체크한다.
 * @param[in]   aExpr           Expression
 * @param[in]   aRefNodeArray   Reference Node Array
 */
stlBool qlncIsExistOuterColumnByRefNodeOnExpr( qlncExprCommon   * aExpr,
                                               qlncNodeArray    * aRefNodeArray )
{
    stlBool       sResult = STL_FALSE;
    stlInt32      i;


    STL_RAMP( RAMP_RETRY );

    if( aExpr == NULL )
    {
        return STL_FALSE;
    }

    switch( aExpr->mType )
    {
        case QLNC_EXPR_TYPE_VALUE:
        case QLNC_EXPR_TYPE_BIND_PARAM:
        case QLNC_EXPR_TYPE_PSEUDO_COLUMN:
        case QLNC_EXPR_TYPE_PSEUDO_ARGUMENT:
            {
                sResult = STL_FALSE;
                break;
            }
        case QLNC_EXPR_TYPE_COLUMN:
            {
                for( i = 0; i < aRefNodeArray->mCurCnt; i++ )
                {
                    if( aRefNodeArray->mNodeArr[i]->mNodeID ==
                        ((qlncExprColumn*)aExpr)->mNode->mNodeID )
                    {
                        break;
                    }
                }

                if( i < aRefNodeArray->mCurCnt )
                {
                    sResult = STL_TRUE;
                }
                else
                {
                    sResult = STL_FALSE;
                }

                break;
            }
        case QLNC_EXPR_TYPE_ROWID_COLUMN:
            {
                for( i = 0; i < aRefNodeArray->mCurCnt; i++ )
                {
                    if( aRefNodeArray->mNodeArr[i]->mNodeID ==
                        ((qlncExprRowidColumn*)aExpr)->mNode->mNodeID )
                    {
                        break;
                    }
                }

                if( i < aRefNodeArray->mCurCnt )
                {
                    sResult = STL_TRUE;
                }
                else
                {
                    sResult = STL_FALSE;
                }

                break;
            }
        case QLNC_EXPR_TYPE_FUNCTION:
            {
                qlncExprFunction    * sFunction = NULL;

                sFunction = (qlncExprFunction*)aExpr;
                for( i = 0; i < sFunction->mArgCount; i++ )
                {
                    sResult = qlncIsExistOuterColumnByRefNodeOnExpr(
                                  sFunction->mArgs[i],
                                  aRefNodeArray );
                    if( sResult == STL_TRUE )
                    {
                        break;
                    }
                }

                break;
            }
        case QLNC_EXPR_TYPE_CAST:
            {
                qlncExprTypeCast    * sTypeCast = NULL;

                sTypeCast = (qlncExprTypeCast*)aExpr;
                for( i = 0; i < sTypeCast->mArgCount; i++ )
                {
                    sResult = qlncIsExistOuterColumnByRefNodeOnExpr(
                                  sTypeCast->mArgs[i],
                                  aRefNodeArray );
                    if( sResult == STL_TRUE )
                    {
                        break;
                    }
                }

                break;
            }
        case QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT:
            {
                sResult = qlncIsExistOuterColumnByRefNodeOnExpr(
                              ((qlncExprConstExpr*)aExpr)->mOrgExpr,
                              aRefNodeArray );
                break;
            }
        case QLNC_EXPR_TYPE_SUB_QUERY:
            {
                sResult = qlncIsExistOuterColumnByRefNodeOnCandNode(
                              ((qlncExprSubQuery*)aExpr)->mNode,
                              aRefNodeArray );
                break;
            }
        case QLNC_EXPR_TYPE_REFERENCE:
            {
                /* @todo 현재 여기로 들어올 수 없음 */

                sResult = qlncIsExistOuterColumnByRefNodeOnExpr(
                              ((qlncExprReference*)aExpr)->mOrgExpr,
                              aRefNodeArray );
                break;
            }
        case QLNC_EXPR_TYPE_INNER_COLUMN:
            {
                if( ((qlncExprInnerColumn*)aExpr)->mOrgExpr != NULL )
                {
                    aExpr = ((qlncExprInnerColumn*)aExpr)->mOrgExpr;
                    STL_THROW( RAMP_RETRY );
                }

                for( i = 0; i < aRefNodeArray->mCurCnt; i++ )
                {
                    if( aRefNodeArray->mNodeArr[i]->mNodeID ==
                        ((qlncExprInnerColumn*)aExpr)->mNode->mNodeID )
                    {
                        break;
                    }
                }

                if( i < aRefNodeArray->mCurCnt )
                {
                    sResult = STL_TRUE;
                }
                else
                {
                    sResult = STL_FALSE;
                }

                break;
            }
        case QLNC_EXPR_TYPE_OUTER_COLUMN:
            {
                /* @todo 현재 여기로 들어올 수 없음 */

                sResult = STL_TRUE;
                break;
            }
        case QLNC_EXPR_TYPE_AGGREGATION:
            {
                qlncExprAggregation * sAggregation  = NULL;

                sAggregation = (qlncExprAggregation*)aExpr;
                for( i = 0; i < sAggregation->mArgCount; i++ )
                {
                    sResult = qlncIsExistOuterColumnByRefNodeOnExpr(
                                  sAggregation->mArgs[i],
                                  aRefNodeArray );
                    if( sResult == STL_TRUE )
                    {
                        break;
                    }
                }

                if( sResult == STL_FALSE )
                {
                    if( sAggregation->mFilter != NULL )
                    {
                        /* @todo Aggregation Filter는 향후 구현한다. */
                        sResult = qlncIsExistOuterColumnByRefNodeOnExpr(
                                      sAggregation->mFilter,
                                      aRefNodeArray );
                    }
                }

                break;
            }
        case QLNC_EXPR_TYPE_CASE_EXPR:
            {
                qlncExprCaseExpr    * sCaseExpr = NULL;

                sCaseExpr = (qlncExprCaseExpr*)aExpr;
                for( i = 0; i < sCaseExpr->mCount; i++ )
                {
                    sResult = qlncIsExistOuterColumnByRefNodeOnExpr(
                                  sCaseExpr->mWhenArr[i],
                                  aRefNodeArray );
                    if( sResult == STL_TRUE )
                    {
                        break;
                    }

                    sResult = qlncIsExistOuterColumnByRefNodeOnExpr(
                                  sCaseExpr->mThenArr[i],
                                  aRefNodeArray );
                    if( sResult == STL_TRUE )
                    {
                        break;
                    }
                }

                if( sResult == STL_FALSE )
                {
                    sResult = qlncIsExistOuterColumnByRefNodeOnExpr(
                                  sCaseExpr->mDefResult,
                                  aRefNodeArray );
                }

                break;
            }
        case QLNC_EXPR_TYPE_LIST_FUNCTION:
            {
                qlncExprListFunc    * sListFunc = NULL;

                sListFunc = (qlncExprListFunc*)aExpr;
                for( i = 0; i < sListFunc->mArgCount; i++ )
                {
                    sResult = qlncIsExistOuterColumnByRefNodeOnExpr(
                                  sListFunc->mArgs[i],
                                  aRefNodeArray );
                    if( sResult == STL_TRUE )
                    {
                        break;
                    }
                }

                break;
            }
        case QLNC_EXPR_TYPE_LIST_COLUMN:
            {
                qlncExprListCol * sListCol  = NULL;

                sListCol = (qlncExprListCol*)aExpr;
                for( i = 0; i < sListCol->mArgCount; i++ )
                {
                    sResult = qlncIsExistOuterColumnByRefNodeOnExpr(
                                  sListCol->mArgs[i],
                                  aRefNodeArray );
                    if( sResult == STL_TRUE )
                    {
                        break;
                    }
                }

                break;
            }
        case QLNC_EXPR_TYPE_ROW_EXPR:
            {
                qlncExprRowExpr * sRowExpr  = NULL;

                sRowExpr = (qlncExprRowExpr*)aExpr;
                for( i = 0; i < sRowExpr->mArgCount; i++ )
                {
                    sResult = qlncIsExistOuterColumnByRefNodeOnExpr(
                                  sRowExpr->mArgs[i],
                                  aRefNodeArray );
                    if( sResult == STL_TRUE )
                    {
                        break;
                    }
                }

                break;
            }
        case QLNC_EXPR_TYPE_AND_FILTER:
            {
                qlncAndFilter   * sAndFilter    = NULL;

                sAndFilter = (qlncAndFilter*)aExpr;
                for( i = 0; i < sAndFilter->mOrCount; i++ )
                {
                    sResult = qlncIsExistOuterColumnByRefNodeOnExpr(
                                  (qlncExprCommon*)(sAndFilter->mOrFilters[i]),
                                  aRefNodeArray );
                    if( sResult == STL_TRUE )
                    {
                        break;
                    }
                }

                break;
            }
        case QLNC_EXPR_TYPE_OR_FILTER:
            {
                qlncOrFilter    * sOrFilter = NULL;

                sOrFilter = (qlncOrFilter*)aExpr;
                for( i = 0; i < sOrFilter->mFilterCount; i++ )
                {
                    sResult = qlncIsExistOuterColumnByRefNodeOnExpr(
                                  sOrFilter->mFilters[i],
                                  aRefNodeArray );
                    if( sResult == STL_TRUE )
                    {
                        break;
                    }
                }

                break;
            }
        case QLNC_EXPR_TYPE_BOOLEAN_FILTER:
            {
                sResult = qlncIsExistOuterColumnByRefNodeOnExpr(
                              ((qlncBooleanFilter*)aExpr)->mExpr,
                              aRefNodeArray );
                break;
            }
        case QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER:
            {
                sResult = qlncIsExistOuterColumnByRefNodeOnExpr(
                              ((qlncConstBooleanFilter*)aExpr)->mExpr,
                              aRefNodeArray );
                break;
            }
        default:
            {
                STL_DASSERT( 0 );
                sResult = STL_FALSE;
                break;
            }
    }

    return sResult;
}


/**
 * @brief Candidate Node에서 Reference Node에 대한 Outer Column이 사용되는지 체크한다.
 * @param[in]   aNode           Candidate Node
 * @param[in]   aRefNodeArray   Reference Node Array
 */
stlBool qlncIsExistOuterColumnByRefNodeOnCandNode( qlncNodeCommon   * aNode,
                                                   qlncNodeArray    * aRefNodeArray )
{
    stlBool       sResult = STL_FALSE;
    stlInt32      i;


#define _QLNC_CHECK_REFERENCE_COLUMN_LIST_BY_REF_NODE( _aRefColList, _aResult ) \
    {                                                                           \
        qlncRefExprItem * _sRefExprItem = NULL;                                 \
                                                                                \
        if( (_aRefColList) != NULL )                                            \
        {                                                                       \
            _sRefExprItem = (_aRefColList)->mHead;                              \
            while( _sRefExprItem != NULL )                                      \
            {                                                                   \
                (_aResult) = qlncIsExistOuterColumnByRefNodeOnExpr(             \
                                 _sRefExprItem->mExpr,                          \
                                 aRefNodeArray );                               \
                                                                                \
                STL_TRY_THROW( (_aResult) == STL_FALSE, RAMP_FINISH );          \
                                                                                \
                _sRefExprItem = _sRefExprItem->mNext;                           \
            }                                                                   \
        }                                                                       \
    }

#define _QLNC_CHECK_AND_FILTER_BY_REF_NODE( _aAndFilter, _aResult ) \
    {                                                               \
        if( (_aAndFilter) != NULL )                                 \
        {                                                           \
            (_aResult) = qlncIsExistOuterColumnByRefNodeOnExpr(     \
                             (qlncExprCommon*)(_aAndFilter),        \
                             aRefNodeArray );                       \
                                                                    \
            STL_TRY_THROW( (_aResult) == STL_FALSE, RAMP_FINISH );  \
        }                                                           \
    }


    switch( aNode->mNodeType )
    {
        case QLNC_NODE_TYPE_QUERY_SET:
            {
                sResult = qlncIsExistOuterColumnByRefNodeOnCandNode(
                              ((qlncQuerySet*)aNode)->mChildNode,
                              aRefNodeArray );
                break;
            }
        case QLNC_NODE_TYPE_SET_OP:
            {
                qlncSetOP   * sSetOP    = NULL;

                sSetOP = (qlncSetOP*)aNode;
                for( i = 0; i < sSetOP->mChildCount; i++ )
                {
                    sResult = qlncIsExistOuterColumnByRefNodeOnCandNode(
                                  sSetOP->mChildNodeArr[i],
                                  aRefNodeArray );

                    STL_TRY_THROW( sResult == STL_FALSE, RAMP_FINISH );
                }

                break;
            }
        case QLNC_NODE_TYPE_QUERY_SPEC:
            {
                qlncQuerySpec   * sQuerySpec    = NULL;

                sResult = STL_FALSE;
                sQuerySpec = (qlncQuerySpec*)aNode;
                for( i = 0; i < sQuerySpec->mTargetCount; i++ )
                {
                    sResult = qlncIsExistOuterColumnByRefNodeOnExpr(
                                  sQuerySpec->mTargetArr[i].mExpr,
                                  aRefNodeArray );

                    STL_TRY_THROW( sResult == STL_FALSE, RAMP_FINISH );
                }

                sResult = qlncIsExistOuterColumnByRefNodeOnCandNode(
                              sQuerySpec->mChildNode,
                              aRefNodeArray );

                break;
            }
        case QLNC_NODE_TYPE_BASE_TABLE:
            {
                if( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_TABLE_SCAN )
                {
                    _QLNC_CHECK_AND_FILTER_BY_REF_NODE( ((qlncTableScanCost*)(aNode->mBestCost))->mTableAndFilter, sResult );
                }
                else if( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_INDEX_SCAN )
                {
                    _QLNC_CHECK_AND_FILTER_BY_REF_NODE( ((qlncIndexScanCost*)(aNode->mBestCost))->mIndexRangeAndFilter, sResult );
                    _QLNC_CHECK_AND_FILTER_BY_REF_NODE( ((qlncIndexScanCost*)(aNode->mBestCost))->mIndexKeyAndFilter, sResult );
                    _QLNC_CHECK_AND_FILTER_BY_REF_NODE( ((qlncIndexScanCost*)(aNode->mBestCost))->mTableAndFilter, sResult );
                }
                else if( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_ROWID_SCAN )
                {
                    _QLNC_CHECK_AND_FILTER_BY_REF_NODE( ((qlncRowidScanCost*)(aNode->mBestCost))->mRowidAndFilter, sResult );
                    _QLNC_CHECK_AND_FILTER_BY_REF_NODE( ((qlncRowidScanCost*)(aNode->mBestCost))->mTableAndFilter, sResult );
                }
                else
                {
                    STL_DASSERT( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_INDEX_COMBINE );
                    for( i = 0; i < ((qlncIndexCombineCost*)(aNode->mBestCost))->mDnfAndFilterCount; i++ )
                    {
                        _QLNC_CHECK_AND_FILTER_BY_REF_NODE( ((qlncIndexCombineCost*)(aNode->mBestCost))->mIndexScanCostArr[i].mIndexRangeAndFilter, sResult );
                        _QLNC_CHECK_AND_FILTER_BY_REF_NODE( ((qlncIndexCombineCost*)(aNode->mBestCost))->mIndexScanCostArr[i].mIndexKeyAndFilter, sResult );
                        _QLNC_CHECK_AND_FILTER_BY_REF_NODE( ((qlncIndexCombineCost*)(aNode->mBestCost))->mIndexScanCostArr[i].mTableAndFilter, sResult );

                        STL_TRY_THROW( sResult == STL_FALSE, RAMP_FINISH );
                    }
                }

                break;
            }
        case QLNC_NODE_TYPE_SUB_TABLE:
            {
                if( ((qlncFilterCost*)(aNode->mBestCost))->mMaterializedNode == NULL )
                {
                    _QLNC_CHECK_AND_FILTER_BY_REF_NODE( ((qlncFilterCost*)(aNode->mBestCost))->mFilter, sResult );

                    sResult = qlncIsExistOuterColumnByRefNodeOnCandNode(
                                  ((qlncSubTableNode*)aNode)->mQueryNode,
                                  aRefNodeArray );
                }
                else
                {
                    /* @todo 여기에 들어오기 위한 테스트 케이스를 만들기 어려움 */
                    sResult = qlncIsExistOuterColumnByRefNodeOnCandNode(
                                  ((qlncFilterCost*)(aNode->mBestCost))->mMaterializedNode,
                                  aRefNodeArray );
                }

                break;
            }
        case QLNC_NODE_TYPE_JOIN_TABLE:
            {
                if( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_INNER_JOIN )
                {
                    sResult = qlncIsExistOuterColumnByRefNodeOnCandNode(
                                  (qlncNodeCommon*)(((qlncJoinTableNode*)aNode)->mBestInnerJoinTable),
                                  aRefNodeArray );
                }
                else if( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_OUTER_JOIN )
                {
                    sResult = qlncIsExistOuterColumnByRefNodeOnCandNode(
                                  (qlncNodeCommon*)(((qlncJoinTableNode*)aNode)->mBestOuterJoinTable),
                                  aRefNodeArray );
                }
                else if( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_SEMI_JOIN )
                {
                    sResult = qlncIsExistOuterColumnByRefNodeOnCandNode(
                                  (qlncNodeCommon*)(((qlncJoinTableNode*)aNode)->mBestSemiJoinTable),
                                  aRefNodeArray );
                }
                else
                {

                    STL_DASSERT( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_ANTI_SEMI_JOIN );
                    sResult = qlncIsExistOuterColumnByRefNodeOnCandNode(
                                  (qlncNodeCommon*)(((qlncJoinTableNode*)aNode)->mBestAntiSemiJoinTable),
                                  aRefNodeArray );
                }

                break;
            }
        case QLNC_NODE_TYPE_INNER_JOIN_TABLE:
        case QLNC_NODE_TYPE_OUTER_JOIN_TABLE:
        case QLNC_NODE_TYPE_SEMI_JOIN_TABLE:
        case QLNC_NODE_TYPE_ANTI_SEMI_JOIN_TABLE:
            {
                if( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_NESTED_LOOPS )
                {
                    _QLNC_CHECK_AND_FILTER_BY_REF_NODE( ((qlncNestedJoinCost*)(aNode->mBestCost))->mJoinCondition, sResult );
                    _QLNC_CHECK_AND_FILTER_BY_REF_NODE( ((qlncNestedJoinCost*)(aNode->mBestCost))->mJoinFilter, sResult );
                    sResult = qlncIsExistOuterColumnByRefNodeOnCandNode(
                                  ((qlncNestedJoinCost*)(aNode->mBestCost))->mLeftNode,
                                  aRefNodeArray );
                    STL_TRY_THROW( sResult == STL_FALSE, RAMP_FINISH );

                    sResult = qlncIsExistOuterColumnByRefNodeOnCandNode(
                                  ((qlncNestedJoinCost*)(aNode->mBestCost))->mRightNode,
                                  aRefNodeArray );
                }
                else if( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_MERGE )
                {
                    _QLNC_CHECK_AND_FILTER_BY_REF_NODE( ((qlncMergeJoinCost*)(aNode->mBestCost))->mEquiJoinCondition, sResult );
                    _QLNC_CHECK_AND_FILTER_BY_REF_NODE( ((qlncMergeJoinCost*)(aNode->mBestCost))->mNonEquiJoinCondition, sResult );
                    _QLNC_CHECK_AND_FILTER_BY_REF_NODE( ((qlncMergeJoinCost*)(aNode->mBestCost))->mJoinFilter, sResult );
                    sResult = qlncIsExistOuterColumnByRefNodeOnCandNode(
                                  ((qlncMergeJoinCost*)(aNode->mBestCost))->mLeftNode,
                                  aRefNodeArray );
                    STL_TRY_THROW( sResult == STL_FALSE, RAMP_FINISH );

                    sResult = qlncIsExistOuterColumnByRefNodeOnCandNode(
                                  ((qlncMergeJoinCost*)(aNode->mBestCost))->mRightNode,
                                  aRefNodeArray );
                }
                else if( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_HASH )
                {
                    _QLNC_CHECK_AND_FILTER_BY_REF_NODE( ((qlncHashJoinCost*)(aNode->mBestCost))->mEquiJoinCondition, sResult );
                    _QLNC_CHECK_AND_FILTER_BY_REF_NODE( ((qlncHashJoinCost*)(aNode->mBestCost))->mNonEquiJoinCondition, sResult );
                    _QLNC_CHECK_AND_FILTER_BY_REF_NODE( ((qlncHashJoinCost*)(aNode->mBestCost))->mJoinFilter, sResult );
                    sResult = qlncIsExistOuterColumnByRefNodeOnCandNode(
                                  ((qlncHashJoinCost*)(aNode->mBestCost))->mLeftNode,
                                  aRefNodeArray );
                    STL_TRY_THROW( sResult == STL_FALSE, RAMP_FINISH );

                    sResult = qlncIsExistOuterColumnByRefNodeOnCandNode(
                                  ((qlncHashJoinCost*)(aNode->mBestCost))->mRightNode,
                                  aRefNodeArray );
                }
                else
                {
                    STL_DASSERT( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_JOIN_COMBINE );
                    for( i = 0; i < ((qlncJoinCombineCost*)(aNode->mBestCost))->mDnfAndFilterCount; i++ )
                    {
                        sResult = qlncIsExistOuterColumnByRefNodeOnCandNode(
                                      ((qlncJoinCombineCost*)(aNode->mBestCost))->mJoinNodePtrArr[i],
                                      aRefNodeArray );
                        STL_TRY_THROW( sResult == STL_FALSE, RAMP_FINISH );
                    }
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
                _QLNC_CHECK_REFERENCE_COLUMN_LIST_BY_REF_NODE( ((qlncSortInstantNode*)aNode)->mKeyColList, sResult );
                _QLNC_CHECK_REFERENCE_COLUMN_LIST_BY_REF_NODE( ((qlncSortInstantNode*)aNode)->mRecColList, sResult );

                _QLNC_CHECK_AND_FILTER_BY_REF_NODE( ((qlncSortInstantNode*)aNode)->mFilter, sResult );

                sResult = qlncIsExistOuterColumnByRefNodeOnCandNode(
                              ((qlncSortInstantNode*)aNode)->mChildNode,
                              aRefNodeArray );

                break;
            }
        case QLNC_NODE_TYPE_HASH_INSTANT:
            {
                _QLNC_CHECK_REFERENCE_COLUMN_LIST_BY_REF_NODE( ((qlncHashInstantNode*)aNode)->mKeyColList, sResult );
                _QLNC_CHECK_REFERENCE_COLUMN_LIST_BY_REF_NODE( ((qlncHashInstantNode*)aNode)->mRecColList, sResult );

                _QLNC_CHECK_AND_FILTER_BY_REF_NODE( ((qlncHashInstantNode*)aNode)->mFilter, sResult );

                sResult = qlncIsExistOuterColumnByRefNodeOnCandNode(
                              ((qlncHashInstantNode*)aNode)->mChildNode,
                              aRefNodeArray );

                break;
            }
        default:
            STL_DASSERT( 0 );
            sResult = STL_FALSE;
            break;
    }


    STL_RAMP( RAMP_FINISH );


    return sResult;
}


/**
 * @brief 현재노드부터 Child Node들 모두에 대한 Node Array를 만든다.
 * @param[in,out]   aNodeArray  Candidate Node Array
 * @param[in]       aExpr       Expression
 * @param[in]       aEnv        Environment
 */
stlStatus qlncMakeNodeArrayOnExpr( qlncNodeArray    * aNodeArray,
                                   qlncExprCommon   * aExpr,
                                   qllEnv           * aEnv )
{
    stlInt32      i;


    STL_PARAM_VALIDATE( aNodeArray != NULL, QLL_ERROR_STACK(aEnv) );


    if( aExpr == NULL )
    {
        return STL_SUCCESS;
    }

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
                    STL_TRY( qlncMakeNodeArrayOnExpr( aNodeArray,
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
                    STL_TRY( qlncMakeNodeArrayOnExpr( aNodeArray,
                                                      sTypeCast->mArgs[i],
                                                      aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT:
            {
                STL_TRY( qlncMakeNodeArrayOnExpr( aNodeArray,
                                                  ((qlncExprConstExpr*)aExpr)->mOrgExpr,
                                                  aEnv )
                         == STL_SUCCESS );
                break;
            }
        case QLNC_EXPR_TYPE_SUB_QUERY:
            {
                STL_TRY( qlncMakeNodeArrayOnCurrNode( aNodeArray,
                                                      ((qlncExprSubQuery*)aExpr)->mNode,
                                                      aEnv )
                         == STL_SUCCESS );
                break;
            }
        case QLNC_EXPR_TYPE_REFERENCE:
            {
                /* @todo 현재 여기에 들어올 수 없음 */

                STL_TRY( qlncMakeNodeArrayOnExpr( aNodeArray,
                                                  ((qlncExprReference*)aExpr)->mOrgExpr,
                                                  aEnv )
                         == STL_SUCCESS );
                break;
            }
        case QLNC_EXPR_TYPE_INNER_COLUMN:
            {
                STL_TRY( qlncMakeNodeArrayOnExpr( aNodeArray,
                                                  ((qlncExprInnerColumn*)aExpr)->mOrgExpr,
                                                  aEnv )
                         == STL_SUCCESS );
                break;
            }
        case QLNC_EXPR_TYPE_OUTER_COLUMN:
            {
                /* @todo 현재 여기에 들어올 수 없음 */

                STL_TRY( qlncMakeNodeArrayOnExpr( aNodeArray,
                                                  ((qlncExprOuterColumn*)aExpr)->mOrgExpr,
                                                  aEnv )
                         == STL_SUCCESS );
                break;
            }
        case QLNC_EXPR_TYPE_AGGREGATION:
            {
                qlncExprAggregation * sAggregation  = NULL;

                sAggregation = (qlncExprAggregation*)aExpr;
                for( i = 0; i < sAggregation->mArgCount; i++ )
                {
                    STL_TRY( qlncMakeNodeArrayOnExpr( aNodeArray,
                                                      sAggregation->mArgs[i],
                                                      aEnv )
                             == STL_SUCCESS );
                }

                if( sAggregation->mFilter != NULL )
                {
                    /* @todo Aggregation Filter는 향후 구현한다.  */
                    STL_TRY( qlncMakeNodeArrayOnExpr( aNodeArray,
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
                    STL_TRY( qlncMakeNodeArrayOnExpr( aNodeArray,
                                                      sCaseExpr->mWhenArr[i],
                                                      aEnv )
                             == STL_SUCCESS );

                    STL_TRY( qlncMakeNodeArrayOnExpr( aNodeArray,
                                                      sCaseExpr->mThenArr[i],
                                                      aEnv )
                             == STL_SUCCESS );
                }

                STL_TRY( qlncMakeNodeArrayOnExpr( aNodeArray,
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
                    STL_TRY( qlncMakeNodeArrayOnExpr( aNodeArray,
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
                    STL_TRY( qlncMakeNodeArrayOnExpr( aNodeArray,
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
                    STL_TRY( qlncMakeNodeArrayOnExpr( aNodeArray,
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
                    STL_TRY( qlncMakeNodeArrayOnExpr( aNodeArray,
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
                    STL_TRY( qlncMakeNodeArrayOnExpr( aNodeArray,
                                                      sOrFilter->mFilters[i],
                                                      aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_EXPR_TYPE_BOOLEAN_FILTER:
            {
                STL_TRY( qlncMakeNodeArrayOnExpr( aNodeArray,
                                                  ((qlncBooleanFilter*)aExpr)->mExpr,
                                                  aEnv )
                         == STL_SUCCESS );
                break;
            }
        case QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER:
            {
                STL_TRY( qlncMakeNodeArrayOnExpr( aNodeArray,
                                                  ((qlncConstBooleanFilter*)aExpr)->mExpr,
                                                  aEnv )
                         == STL_SUCCESS );
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


/**
 * @brief 현재노드부터 Child Node들 모두에 대한 Node Array를 만든다.
 * @param[in,out]   aNodeArray  Candidate Node Array
 * @param[in]       aNode       Candidate Node
 * @param[in]       aEnv        Environment
 */
stlStatus qlncMakeNodeArrayOnCurrNode( qlncNodeArray    * aNodeArray,
                                       qlncNodeCommon   * aNode,
                                       qllEnv           * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aNodeArray != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode != NULL, QLL_ERROR_STACK(aEnv) );


    /* Node 추가 */
    STL_TRY( qlncAddNodeToNodeArray( aNodeArray,
                                     aNode,
                                     aEnv )
             == STL_SUCCESS );

    /* Optimizer가 되었으면 Optimize된 정보를 이용하여 처리 */
    if( aNode->mBestCost != NULL )
    {
        switch( aNode->mBestOptType )
        {
            case QLNC_BEST_OPT_TYPE_INNER_JOIN:
                {
                    STL_TRY( qlncMakeNodeArrayOnCurrNode(
                                 aNodeArray,
                                 (qlncNodeCommon*)(((qlncJoinTableNode*)aNode)->mBestInnerJoinTable),
                                 aEnv )
                             == STL_SUCCESS );

                    STL_THROW( RAMP_FINISH );
                }
            case QLNC_BEST_OPT_TYPE_OUTER_JOIN:
                {
                    STL_TRY( qlncMakeNodeArrayOnCurrNode(
                                 aNodeArray,
                                 (qlncNodeCommon*)(((qlncJoinTableNode*)aNode)->mBestOuterJoinTable),
                                 aEnv )
                             == STL_SUCCESS );

                    STL_THROW( RAMP_FINISH );
                }
            case QLNC_BEST_OPT_TYPE_SEMI_JOIN:
                {
                    STL_TRY( qlncMakeNodeArrayOnCurrNode(
                                 aNodeArray,
                                 (qlncNodeCommon*)(((qlncJoinTableNode*)aNode)->mBestSemiJoinTable),
                                 aEnv )
                             == STL_SUCCESS );

                    STL_THROW( RAMP_FINISH );
                }
            case QLNC_BEST_OPT_TYPE_ANTI_SEMI_JOIN:
                {
                    STL_TRY( qlncMakeNodeArrayOnCurrNode(
                                 aNodeArray,
                                 (qlncNodeCommon*)(((qlncJoinTableNode*)aNode)->mBestAntiSemiJoinTable),
                                 aEnv )
                             == STL_SUCCESS );

                    STL_THROW( RAMP_FINISH );
                }
            case QLNC_BEST_OPT_TYPE_FILTER:
                {
                    qlncFilterCost  * sFilterCost   = NULL;

                    sFilterCost = (qlncFilterCost*)(aNode->mBestCost);

                    if( sFilterCost->mMaterializedNode != NULL )
                    {
                        /* @todo 여기에 들어오기 위한 테스트 케이스를 만들기 어려움 */
                        STL_TRY( qlncMakeNodeArrayOnCurrNode(
                                     aNodeArray,
                                     sFilterCost->mMaterializedNode,
                                     aEnv )
                                 == STL_SUCCESS );
                    }
                    else
                    {
                        STL_TRY( qlncMakeNodeArrayOnCurrNode(
                                     aNodeArray,
                                     ((qlncSubTableNode*)aNode)->mQueryNode,
                                     aEnv )
                                 == STL_SUCCESS );
                    }

                    STL_THROW( RAMP_FINISH );
                }
            case QLNC_BEST_OPT_TYPE_NESTED_LOOPS:
                {
                    qlncNestedJoinCost  * sNestedJoinCost   = NULL;

                    sNestedJoinCost = (qlncNestedJoinCost*)(aNode->mBestCost);

                    STL_TRY( qlncMakeNodeArrayOnCurrNode(
                                 aNodeArray,
                                 sNestedJoinCost->mLeftNode,
                                 aEnv )
                             == STL_SUCCESS );

                    STL_TRY( qlncMakeNodeArrayOnCurrNode(
                                 aNodeArray,
                                 sNestedJoinCost->mRightNode,
                                 aEnv )
                             == STL_SUCCESS );

                    STL_THROW( RAMP_FINISH );
                }
            case QLNC_BEST_OPT_TYPE_MERGE:
                {
                    qlncMergeJoinCost   *  sMergeJoinCost   = NULL;

                    sMergeJoinCost = (qlncMergeJoinCost*)(aNode->mBestCost);

                    STL_TRY( qlncMakeNodeArrayOnCurrNode(
                                 aNodeArray,
                                 sMergeJoinCost->mLeftNode,
                                 aEnv )
                             == STL_SUCCESS );

                    STL_TRY( qlncMakeNodeArrayOnCurrNode(
                                 aNodeArray,
                                 sMergeJoinCost->mRightNode,
                                 aEnv )
                             == STL_SUCCESS );

                    STL_THROW( RAMP_FINISH );
                }
            case QLNC_BEST_OPT_TYPE_HASH:
                {
                    qlncHashJoinCost    *  sHashJoinCost    = NULL;

                    sHashJoinCost = (qlncHashJoinCost*)(aNode->mBestCost);

                    STL_TRY( qlncMakeNodeArrayOnCurrNode(
                                 aNodeArray,
                                 sHashJoinCost->mLeftNode,
                                 aEnv )
                             == STL_SUCCESS );

                    STL_TRY( qlncMakeNodeArrayOnCurrNode(
                                 aNodeArray,
                                 sHashJoinCost->mRightNode,
                                 aEnv )
                             == STL_SUCCESS );

                    STL_THROW( RAMP_FINISH );
                }
            case QLNC_BEST_OPT_TYPE_JOIN_COMBINE:
                {
                    stlInt32              i;
                    qlncJoinCombineCost * sJoinCombineCost  = NULL;

                    sJoinCombineCost = ((qlncInnerJoinTableNode*)aNode)->mJoinCombineCost;
                    for( i = 0; i < sJoinCombineCost->mDnfAndFilterCount; i++ )
                    {
                        STL_TRY( qlncMakeNodeArrayOnCurrNode(
                                     aNodeArray,
                                     sJoinCombineCost->mJoinNodePtrArr[i],
                                     aEnv )
                                 == STL_SUCCESS );
                    }

                    STL_THROW( RAMP_FINISH );
                }

            default:
                break;
        }
    }

    /* Node Type에 따른 처리 */
    switch( aNode->mNodeType )
    {
        case QLNC_NODE_TYPE_QUERY_SET:
            {
                STL_TRY( qlncMakeNodeArrayOnCurrNode( aNodeArray,
                                                      ((qlncQuerySet*)aNode)->mChildNode,
                                                      aEnv )
                         == STL_SUCCESS );
                break;
            }
        case QLNC_NODE_TYPE_SET_OP:
            {
                stlInt32      i;
                qlncSetOP   * sSetOP    = NULL;

                sSetOP = (qlncSetOP*)aNode;
                for( i = 0; i < sSetOP->mChildCount; i++ )
                {
                    STL_TRY( qlncMakeNodeArrayOnCurrNode( aNodeArray,
                                                          sSetOP->mChildNodeArr[i],
                                                          aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_NODE_TYPE_QUERY_SPEC:
            {
                stlInt32      i;

                STL_TRY( qlncMakeNodeArrayOnCurrNode( aNodeArray,
                                                      ((qlncQuerySpec*)aNode)->mChildNode,
                                                      aEnv )
                         == STL_SUCCESS );

                for( i = 0; i < ((qlncQuerySpec*)aNode)->mTargetCount; i++ )
                {
                    STL_TRY( qlncMakeNodeArrayOnExpr( aNodeArray,
                                                      ((qlncQuerySpec*)aNode)->mTargetArr[i].mExpr,
                                                      aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_NODE_TYPE_BASE_TABLE:
            {
                if( ((qlncBaseTableNode*)aNode)->mFilter != NULL )
                {
                    STL_TRY( qlncMakeNodeArrayOnExpr( aNodeArray,
                                                      (qlncExprCommon*)(((qlncBaseTableNode*)aNode)->mFilter),
                                                      aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_NODE_TYPE_SUB_TABLE:
            {
                STL_TRY( qlncMakeNodeArrayOnCurrNode( aNodeArray,
                                                      ((qlncSubTableNode*)aNode)->mQueryNode,
                                                      aEnv )
                         == STL_SUCCESS );

                if( ((qlncSubTableNode*)aNode)->mFilter != NULL )
                {
                    STL_TRY( qlncMakeNodeArrayOnExpr( aNodeArray,
                                                      (qlncExprCommon*)(((qlncSubTableNode*)aNode)->mFilter),
                                                      aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_NODE_TYPE_JOIN_TABLE:
            {
                stlInt32      i;
                qlncNodeArray   * sNodeArray    = NULL;

                /* @todo 여기로 들어오는 테스트 케이스를 만들기 어려움 */
                sNodeArray = &(((qlncJoinTableNode*)aNode)->mNodeArray);
                for( i = 0; i < sNodeArray->mCurCnt; i++ )
                {
                    STL_TRY( qlncMakeNodeArrayOnCurrNode( aNodeArray,
                                                          sNodeArray->mNodeArr[i],
                                                          aEnv )
                             == STL_SUCCESS );
                }

                if( ((qlncJoinTableNode*)aNode)->mJoinCond != NULL )
                {
                    STL_TRY( qlncMakeNodeArrayOnExpr( aNodeArray,
                                                      (qlncExprCommon*)(((qlncJoinTableNode*)aNode)->mJoinCond),
                                                      aEnv )
                             == STL_SUCCESS );
                }

                if( ((qlncJoinTableNode*)aNode)->mFilter != NULL )
                {
                    STL_TRY( qlncMakeNodeArrayOnExpr( aNodeArray,
                                                      (qlncExprCommon*)(((qlncJoinTableNode*)aNode)->mFilter),
                                                      aEnv )
                             == STL_SUCCESS );
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
                STL_TRY( qlncMakeNodeArrayOnCurrNode( aNodeArray,
                                                      ((qlncSortInstantNode*)aNode)->mChildNode,
                                                      aEnv )
                         == STL_SUCCESS );

                if( ((qlncSortInstantNode*)aNode)->mFilter != NULL )
                {
                    STL_TRY( qlncMakeNodeArrayOnExpr( aNodeArray,
                                                      (qlncExprCommon*)(((qlncSortInstantNode*)aNode)->mFilter),
                                                      aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_NODE_TYPE_HASH_INSTANT:
            {
                STL_TRY( qlncMakeNodeArrayOnCurrNode( aNodeArray,
                                                      ((qlncHashInstantNode*)aNode)->mChildNode,
                                                      aEnv )
                         == STL_SUCCESS );

                if( ((qlncHashInstantNode*)aNode)->mFilter != NULL )
                {
                    STL_TRY( qlncMakeNodeArrayOnExpr( aNodeArray,
                                                      (qlncExprCommon*)(((qlncHashInstantNode*)aNode)->mFilter),
                                                      aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        /* @todo 아래 Join들은 이미 Best Cost가 존재하므로 들어올 수 없음 */
        /* case QLNC_NODE_TYPE_INNER_JOIN_TABLE: */
        /* case QLNC_NODE_TYPE_OUTER_JOIN_TABLE: */
        /* case QLNC_NODE_TYPE_SEMI_JOIN_TABLE: */
        /* case QLNC_NODE_TYPE_ANTI_SEMI_JOIN_TABLE: */
        default:
            STL_DASSERT( 0 );
            break;
    }


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


#if 0

/**
 * @brief Expression을 탐색하여 Column이 나올때 ExceptNodeArray에 속하지 않는 Node를 참조하는 경우
 *        OrgNodeArray로부터 해당 Node를 찾아 제거하고 SemiJoinNodeArray에 추가한다.
 * @param[in]       aExpr                   Expression
 * @param[in]       aOrgNodeArray           Original Node Array
 * @param[in]       aExceptNodeArray        Except Node Array
 * @param[in,out]   aSemiJoinNodeArray      Semi-Join에 사용될 Node Array
 * @param[in]       aEnv                    Environment
 */
stlStatus qlncExtractNodeForSemiJoinFromExpr( qlncExprCommon    * aExpr,
                                              qlncNodeArray     * aOrgNodeArray,
                                              qlncNodeArray     * aExceptNodeArray,
                                              qlncNodeArray     * aSemiJoinNodeArray,
                                              qllEnv            * aEnv )
{
    stlInt32      i;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOrgNodeArray != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExceptNodeArray != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSemiJoinNodeArray != NULL, QLL_ERROR_STACK(aEnv) );


#define _QLNC_ADJUST_NODE_ARRAY( _aNode )                                       \
    {                                                                           \
        for( i = 0; i < aExceptNodeArray->mCurCnt; i++ )                        \
        {                                                                       \
            if( (_aNode)->mNodeID ==                                            \
                aExceptNodeArray->mNodeArr[i]->mNodeID )                        \
            {                                                                   \
                break;                                                          \
            }                                                                   \
        }                                                                       \
                                                                                \
        /* Except Node Array에 존재하지 않는 경우 */                            \
        if( i >= aExceptNodeArray->mCurCnt )                                    \
        {                                                                       \
            /* Orginal Node Array에서 제거 */                                   \
            for( i = 0; i < aOrgNodeArray->mCurCnt; i++ )                       \
            {                                                                   \
                if( (_aNode)->mNodeID ==                                        \
                    aOrgNodeArray->mNodeArr[i]->mNodeID )                       \
                {                                                               \
                    break;                                                      \
                }                                                               \
            }                                                                   \
                                                                                \
            if( i < aOrgNodeArray->mCurCnt )                                    \
            {                                                                   \
                /* Orginal Node Array를 Compact */                              \
                for( ; (i + 1) < aOrgNodeArray->mCurCnt; i++ )                  \
                {                                                               \
                    aOrgNodeArray->mNodeArr[i] = aOrgNodeArray->mNodeArr[i+1];  \
                }                                                               \
                aOrgNodeArray->mCurCnt--;                                       \
                                                                                \
                /* Semi-Join Node Array에 현재 Node 추가 */                     \
                STL_TRY( qlncAddNodeToNodeArray( aSemiJoinNodeArray,            \
                                                 (_aNode),                      \
                                                 aEnv )                         \
                         == STL_SUCCESS );                                      \
            }                                                                   \
        }                                                                       \
    }

    switch( aExpr->mType )
    {
        case QLNC_EXPR_TYPE_VALUE:
        case QLNC_EXPR_TYPE_BIND_PARAM:
        case QLNC_EXPR_TYPE_PSEUDO_COLUMN:
        case QLNC_EXPR_TYPE_PSEUDO_ARGUMENT:
            {
                break;
            }
        case QLNC_EXPR_TYPE_COLUMN:
            {
                _QLNC_ADJUST_NODE_ARRAY( ((qlncExprColumn*)aExpr)->mNode );
                break;
            }
        case QLNC_EXPR_TYPE_ROWID_COLUMN:
            {
                _QLNC_ADJUST_NODE_ARRAY( ((qlncExprRowidColumn*)aExpr)->mNode );
                break;
            }
        case QLNC_EXPR_TYPE_FUNCTION:
            {
                qlncExprFunction    * sFunction = NULL;

                sFunction = (qlncExprFunction*)aExpr;
                for( i = 0; i < sFunction->mArgCount; i++ )
                {
                    STL_TRY( qlncExtractNodeForSemiJoinFromExpr( sFunction->mArgs[i],
                                                                 aOrgNodeArray,
                                                                 aExceptNodeArray,
                                                                 aSemiJoinNodeArray,
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
                    STL_TRY( qlncExtractNodeForSemiJoinFromExpr( sTypeCast->mArgs[i],
                                                                 aOrgNodeArray,
                                                                 aExceptNodeArray,
                                                                 aSemiJoinNodeArray,
                                                                 aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT:
            {
                STL_TRY( qlncExtractNodeForSemiJoinFromExpr( ((qlncExprConstExpr*)aExpr)->mOrgExpr,
                                                             aOrgNodeArray,
                                                             aExceptNodeArray,
                                                             aSemiJoinNodeArray,
                                                             aEnv )
                         == STL_SUCCESS );
                break;
            }
        case QLNC_EXPR_TYPE_SUB_QUERY:
            {
                STL_DASSERT( 0 );
                break;
            }
        case QLNC_EXPR_TYPE_REFERENCE:
            {
                STL_TRY( qlncExtractNodeForSemiJoinFromExpr( ((qlncExprReference*)aExpr)->mOrgExpr,
                                                             aOrgNodeArray,
                                                             aExceptNodeArray,
                                                             aSemiJoinNodeArray,
                                                             aEnv )
                         == STL_SUCCESS );
                break;
            }
        case QLNC_EXPR_TYPE_INNER_COLUMN:
            {
                if( (((qlncExprInnerColumn*)aExpr)->mNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE) ||
                    (((qlncExprInnerColumn*)aExpr)->mNode->mNodeType == QLNC_NODE_TYPE_SORT_INSTANT) ||
                    (((qlncExprInnerColumn*)aExpr)->mNode->mNodeType == QLNC_NODE_TYPE_HASH_INSTANT) )
                {
                    _QLNC_ADJUST_NODE_ARRAY( ((qlncExprInnerColumn*)aExpr)->mNode );
                }
                else
                {
                    STL_DASSERT( ((qlncExprInnerColumn*)aExpr)->mOrgExpr != NULL );
                    STL_TRY( qlncExtractNodeForSemiJoinFromExpr( ((qlncExprInnerColumn*)aExpr)->mOrgExpr,
                                                                 aOrgNodeArray,
                                                                 aExceptNodeArray,
                                                                 aSemiJoinNodeArray,
                                                                 aEnv )
                             == STL_SUCCESS );
                }
                break;
            }
        case QLNC_EXPR_TYPE_OUTER_COLUMN:
            {
                STL_TRY( qlncExtractNodeForSemiJoinFromExpr( ((qlncExprOuterColumn*)aExpr)->mOrgExpr,
                                                             aOrgNodeArray,
                                                             aExceptNodeArray,
                                                             aSemiJoinNodeArray,
                                                             aEnv )
                         == STL_SUCCESS );
                break;
            }
        case QLNC_EXPR_TYPE_AGGREGATION:
            {
                qlncExprAggregation * sAggregation  = NULL;

                sAggregation = (qlncExprAggregation*)aExpr;
                for( i = 0; i < sAggregation->mArgCount; i++ )
                {
                    STL_TRY( qlncExtractNodeForSemiJoinFromExpr( sAggregation->mArgs[i],
                                                                 aOrgNodeArray,
                                                                 aExceptNodeArray,
                                                                 aSemiJoinNodeArray,
                                                                 aEnv )
                             == STL_SUCCESS );
                }

                if( sAggregation->mFilter != NULL )
                {
                    STL_TRY( qlncExtractNodeForSemiJoinFromExpr( sAggregation->mFilter,
                                                                 aOrgNodeArray,
                                                                 aExceptNodeArray,
                                                                 aSemiJoinNodeArray,
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
                    STL_TRY( qlncExtractNodeForSemiJoinFromExpr( sCaseExpr->mWhenArr[i],
                                                                 aOrgNodeArray,
                                                                 aExceptNodeArray,
                                                                 aSemiJoinNodeArray,
                                                                 aEnv )
                             == STL_SUCCESS );

                    STL_TRY( qlncExtractNodeForSemiJoinFromExpr( sCaseExpr->mThenArr[i],
                                                                 aOrgNodeArray,
                                                                 aExceptNodeArray,
                                                                 aSemiJoinNodeArray,
                                                                 aEnv )
                             == STL_SUCCESS );
                }

                STL_TRY( qlncExtractNodeForSemiJoinFromExpr( sCaseExpr->mDefResult,
                                                             aOrgNodeArray,
                                                             aExceptNodeArray,
                                                             aSemiJoinNodeArray,
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
                    STL_TRY( qlncExtractNodeForSemiJoinFromExpr( sListFunc->mArgs[i],
                                                                 aOrgNodeArray,
                                                                 aExceptNodeArray,
                                                                 aSemiJoinNodeArray,
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
                    STL_TRY( qlncExtractNodeForSemiJoinFromExpr( sListCol->mArgs[i],
                                                                 aOrgNodeArray,
                                                                 aExceptNodeArray,
                                                                 aSemiJoinNodeArray,
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
                    STL_TRY( qlncExtractNodeForSemiJoinFromExpr( sRowExpr->mArgs[i],
                                                                 aOrgNodeArray,
                                                                 aExceptNodeArray,
                                                                 aSemiJoinNodeArray,
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
                    STL_TRY( qlncExtractNodeForSemiJoinFromExpr( (qlncExprCommon*)(sAndFilter->mOrFilters[i]),
                                                                 aOrgNodeArray,
                                                                 aExceptNodeArray,
                                                                 aSemiJoinNodeArray,
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
                    STL_TRY( qlncExtractNodeForSemiJoinFromExpr( sOrFilter->mFilters[i],
                                                                 aOrgNodeArray,
                                                                 aExceptNodeArray,
                                                                 aSemiJoinNodeArray,
                                                                 aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_EXPR_TYPE_BOOLEAN_FILTER:
            {
                STL_TRY( qlncExtractNodeForSemiJoinFromExpr( ((qlncBooleanFilter*)aExpr)->mExpr,
                                                             aOrgNodeArray,
                                                             aExceptNodeArray,
                                                             aSemiJoinNodeArray,
                                                             aEnv )
                         == STL_SUCCESS );
                break;
            }
        case QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER:
            {
                STL_TRY( qlncExtractNodeForSemiJoinFromExpr( ((qlncConstBooleanFilter*)aExpr)->mExpr,
                                                             aOrgNodeArray,
                                                             aExceptNodeArray,
                                                             aSemiJoinNodeArray,
                                                             aEnv )
                         == STL_SUCCESS );
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
 * @brief Candidate Node에서 Set 연산자를 갖는 Node가 존재하는지 체크한다.
 * @param[in]   aNode       Candidate Node
 */
stlBool qlncIsUpdatableNode( qlncNodeCommon    * aNode )
{
    stlBool           sResult       = STL_FALSE;
    stlInt32          i;
    qlncRefExprItem * sRefExprItem  = NULL;


    switch( aNode->mNodeType )
    {
        case QLNC_NODE_TYPE_QUERY_SET:
            {
                sResult = STL_FALSE;
                break;
            }
        case QLNC_NODE_TYPE_SET_OP:
            {
                STL_DASSERT( 0 );
                break;
            }
        case QLNC_NODE_TYPE_QUERY_SPEC:
            {
                sResult = qlncIsUpdatableNode( ((qlncQuerySpec*)aNode)->mChildNode );

                if( sResult == STL_TRUE )
                {
                    /* Target에 Aggregation이 존재하는지 파악한다. */
                    for( i = 0; i < ((qlncQuerySpec*)aNode)->mTargetCount; i++ )
                    {
                        if( qlncHasAggregationOnExpr( ((qlncQuerySpec*)aNode)->mTargetArr[i].mExpr,
                                                      aNode->mNodeID,
                                                      STL_FALSE )
                            == STL_TRUE )
                        {
                            sResult = STL_FALSE;
                            break;
                        }
                    }
                }

                break;
            }
        case QLNC_NODE_TYPE_BASE_TABLE:
            {
                if( ellGetTableType( ((qlncBaseTableNode*)aNode)->mTableInfo->mTableHandle )
                    == ELL_TABLE_TYPE_BASE_TABLE )
                {
                    sResult = STL_TRUE;
                }
                else
                {
                    sResult = STL_FALSE;
                }

                break;
            }
        case QLNC_NODE_TYPE_SUB_TABLE:
            {
                sResult = qlncIsUpdatableNode( ((qlncSubTableNode*)aNode)->mQueryNode );
                break;
            }
        case QLNC_NODE_TYPE_JOIN_TABLE:
            {
                qlncNodeArray   * sNodeArray    = NULL;

                if( ((qlncJoinTableNode*)aNode)->mJoinType == QLNC_JOIN_TYPE_INNER )
                {
                    sResult = STL_TRUE;
                    sNodeArray = &(((qlncJoinTableNode*)aNode)->mNodeArray);
                    for( i = 0; i < sNodeArray->mCurCnt; i++ )
                    {
                        sResult = qlncIsUpdatableNode( sNodeArray->mNodeArr[i] );
                        if( sResult == STL_FALSE )
                        {
                            break;
                        }
                    }
                }
                else
                {
                    sResult = STL_FALSE;
                }

                break;
            }
        case QLNC_NODE_TYPE_FLAT_INSTANT:
            {
                STL_DASSERT( 0 );
                sResult = STL_FALSE;
                break;
            }
        case QLNC_NODE_TYPE_SORT_INSTANT:
            {
                if( (((qlncSortInstantNode*)aNode)->mInstantType == QLNC_INSTANT_TYPE_GROUP) ||
                    (((qlncSortInstantNode*)aNode)->mInstantType == QLNC_INSTANT_TYPE_DISTINCT) )
                {
                    /* @todo Sort Instant는 현재 Group, Distinct가 될 수 없다. */

                    sResult = STL_FALSE;
                }
                else
                {
                    sResult = qlncIsUpdatableNode( ((qlncSortInstantNode*)aNode)->mChildNode );

                    if( sResult == STL_TRUE )
                    {
                        /* Target에 Aggregation이 존재하는지 파악한다. */
                        sRefExprItem = ((qlncSortInstantNode*)aNode)->mKeyColList->mHead;
                        while( sRefExprItem != NULL )
                        {
                            if( qlncHasAggregationOnExpr( sRefExprItem->mExpr,
                                                          aNode->mNodeID,
                                                          STL_FALSE )
                                == STL_TRUE )
                            {
                                sResult = STL_FALSE;
                                break;
                            }

                            sRefExprItem = sRefExprItem->mNext;
                        }
                    }
                }
                break;
            }
        case QLNC_NODE_TYPE_HASH_INSTANT:
            {
                if( (((qlncHashInstantNode*)aNode)->mInstantType == QLNC_INSTANT_TYPE_GROUP) ||
                    (((qlncHashInstantNode*)aNode)->mInstantType == QLNC_INSTANT_TYPE_DISTINCT) )
                {
                    sResult = STL_FALSE;
                }
                else
                {
                    /* @todo Hash Join Instant인 경우 여기에 들어올 수 없음 */
                    sResult = qlncIsUpdatableNode( ((qlncHashInstantNode*)aNode)->mChildNode );
                }
                break;
            }
        case QLNC_NODE_TYPE_INNER_JOIN_TABLE:
            {
                sResult = qlncIsUpdatableNode( ((qlncInnerJoinTableNode*)aNode)->mLeftNode );

                if( sResult == STL_TRUE )
                {
                    sResult = qlncIsUpdatableNode( ((qlncInnerJoinTableNode*)aNode)->mRightNode );
                }

                break;
            }
        /* @todo Outer Join이나 Semi Join, Anti Semi Join은
         * Join Table Node에서 이미 걸러지므로 들어올 수 없다. */
        /* case QLNC_NODE_TYPE_OUTER_JOIN_TABLE: */
        /* case QLNC_NODE_TYPE_SEMI_JOIN_TABLE: */
        /* case QLNC_NODE_TYPE_ANTI_SEMI_JOIN_TABLE: */
        default:
            STL_DASSERT( 0 );
            sResult = STL_FALSE;
            break;
    }

    return sResult;
}


/**
 * @brief Expression에 Aggregation이 존재하는지 판단한다.
 * @param[in]   aExpr               Expression
 * @param[in]   aSkipSubTableNodeID Skip Node ID of Inner Column
 * @param[in]   aCheckNested        Nested Aggregation인지를 판단해야 하는지 여부
 */
stlBool qlncHasAggregationOnExpr( qlncExprCommon    * aExpr,
                                  stlInt32            aSkipSubTableNodeID,
                                  stlBool             aCheckNested )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_DASSERT( aExpr != NULL );


    switch( aExpr->mType )
    {
        case QLNC_EXPR_TYPE_VALUE:
        case QLNC_EXPR_TYPE_BIND_PARAM:
        case QLNC_EXPR_TYPE_COLUMN:
            {
                return STL_FALSE;
            }
        case QLNC_EXPR_TYPE_FUNCTION:
            {
                qlncExprFunction    * sFunction = NULL;
                stlInt32              i;

                sFunction = (qlncExprFunction*)aExpr;
                for( i = 0; i < sFunction->mArgCount; i++ )
                {
                    if( qlncHasAggregationOnExpr( sFunction->mArgs[i],
                                                  aSkipSubTableNodeID,
                                                  aCheckNested )
                        == STL_TRUE )
                    {
                        return STL_TRUE;
                    }
                }

                return STL_FALSE;
            }
        case QLNC_EXPR_TYPE_CAST:
            {
                qlncExprTypeCast    * sTypeCast = NULL;
                stlInt32              i;

                sTypeCast = (qlncExprTypeCast*)aExpr;
                for( i = 0; i < sTypeCast->mArgCount; i++ )
                {
                    if( qlncHasAggregationOnExpr( sTypeCast->mArgs[i],
                                                  aSkipSubTableNodeID,
                                                  aCheckNested )
                        == STL_TRUE )
                    {
                        return STL_TRUE;
                    }
                }

                return STL_FALSE;
            }
        case QLNC_EXPR_TYPE_PSEUDO_COLUMN:
        case QLNC_EXPR_TYPE_PSEUDO_ARGUMENT:
        case QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT:
        case QLNC_EXPR_TYPE_SUB_QUERY:
        case QLNC_EXPR_TYPE_REFERENCE:
            {
                return STL_FALSE;
            }
        case QLNC_EXPR_TYPE_INNER_COLUMN:
            {
                qlncExprInnerColumn * sInnerColumn  = NULL;

                sInnerColumn = (qlncExprInnerColumn*)aExpr;
                if( (sInnerColumn->mNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE) &&
                    (sInnerColumn->mNode->mNodeID == aSkipSubTableNodeID) )
                {

                    if( ((qlncSubTableNode*)(sInnerColumn->mNode))->mQueryNode->mNodeType ==
                        QLNC_NODE_TYPE_QUERY_SPEC )
                    {
                        return qlncHasAggregationOnExpr(
                                   ((qlncQuerySpec*)(((qlncSubTableNode*)(sInnerColumn->mNode))->mQueryNode))->mTargetArr[sInnerColumn->mIdx].mExpr,
                                   aSkipSubTableNodeID,
                                   aCheckNested );
                    }
                    else
                    {
                        STL_DASSERT( ((qlncSubTableNode*)(sInnerColumn->mNode))->mQueryNode->mNodeType ==
                                     QLNC_NODE_TYPE_QUERY_SET );

                        return qlncHasAggregationOnExpr(
                                   ((qlncQuerySet*)(((qlncSubTableNode*)(sInnerColumn->mNode))->mQueryNode))->mSetTargetArr[sInnerColumn->mIdx].mExpr,
                                   aSkipSubTableNodeID,
                                   aCheckNested );
                    }
                }
                else
                {
                    if( sInnerColumn->mOrgExpr != NULL )
                    {
                        return qlncHasAggregationOnExpr( sInnerColumn->mOrgExpr,
                                                         aSkipSubTableNodeID,
                                                         aCheckNested );
                    }
                }

                return STL_FALSE;
            }
        case QLNC_EXPR_TYPE_OUTER_COLUMN:
            {
                STL_DASSERT( 0 );
                return STL_FALSE;
            }
        case QLNC_EXPR_TYPE_ROWID_COLUMN:
            {
                return STL_FALSE;
            }
        case QLNC_EXPR_TYPE_AGGREGATION:
            {
                if( aCheckNested == STL_TRUE )
                {
                    if( ((qlncExprAggregation*)aExpr)->mHasNestedAggr == STL_TRUE )
                    {
                        return STL_TRUE;
                    }
                    else
                    {
                        return STL_FALSE;
                    }
                }
                else
                {
                    return STL_TRUE;
                }
            }
        case QLNC_EXPR_TYPE_CASE_EXPR:
            {
                qlncExprCaseExpr    * sCaseExpr = NULL;
                stlInt32              i;

                sCaseExpr = (qlncExprCaseExpr*)aExpr;
                for( i = 0; i < sCaseExpr->mCount; i++ )
                {
                    if( qlncHasAggregationOnExpr( sCaseExpr->mWhenArr[i],
                                                  aSkipSubTableNodeID,
                                                  aCheckNested )
                        == STL_TRUE )
                    {
                        return STL_TRUE;
                    }

                    if( qlncHasAggregationOnExpr( sCaseExpr->mThenArr[i],
                                                  aSkipSubTableNodeID,
                                                  aCheckNested )
                        == STL_TRUE )
                    {
                        return STL_TRUE;
                    }
                }

                if( qlncHasAggregationOnExpr( sCaseExpr->mDefResult,
                                              aSkipSubTableNodeID,
                                              aCheckNested )
                    == STL_TRUE )
                {
                    return STL_TRUE;
                }

                return STL_FALSE;
            }
        case QLNC_EXPR_TYPE_LIST_FUNCTION:
            {
                qlncExprListFunc    * sListFunc = NULL;
                stlInt32              i;

                sListFunc = (qlncExprListFunc*)aExpr;
                for( i = 0; i < sListFunc->mArgCount; i++ )
                {
                    if( qlncHasAggregationOnExpr( sListFunc->mArgs[i],
                                                  aSkipSubTableNodeID,
                                                  aCheckNested )
                        == STL_TRUE )
                    {
                        return STL_TRUE;
                    }
                }

                return STL_FALSE;
            }
        case QLNC_EXPR_TYPE_LIST_COLUMN:
            {
                qlncExprListCol     * sListCol  = NULL;
                stlInt32              i;

                sListCol = (qlncExprListCol*)aExpr;
                for( i = 0; i < sListCol->mArgCount; i++ )
                {
                    if( qlncHasAggregationOnExpr( sListCol->mArgs[i],
                                                  aSkipSubTableNodeID,
                                                  aCheckNested )
                        == STL_TRUE )
                    {
                        return STL_TRUE;
                    }
                }

                return STL_FALSE;
            }
        case QLNC_EXPR_TYPE_ROW_EXPR:
            {
                qlncExprRowExpr     * sRowExpr  = NULL;
                stlInt32              i;

                sRowExpr = (qlncExprRowExpr*)aExpr;
                for( i = 0; i < sRowExpr->mArgCount; i++ )
                {
                    if( qlncHasAggregationOnExpr( sRowExpr->mArgs[i],
                                                  aSkipSubTableNodeID,
                                                  aCheckNested )
                        == STL_TRUE )
                    {
                        return STL_TRUE;
                    }
                }

                return STL_FALSE;
            }
        case QLNC_EXPR_TYPE_AND_FILTER:
            {
                qlncAndFilter   * sAndFilter    = NULL;
                stlInt32          i;

                sAndFilter = (qlncAndFilter*)aExpr;
                for( i = 0; i < sAndFilter->mOrCount; i++ )
                {
                    if( qlncHasAggregationOnExpr( (qlncExprCommon*)(sAndFilter->mOrFilters[i]),
                                                  aSkipSubTableNodeID,
                                                  aCheckNested )
                        == STL_TRUE )
                    {
                        return STL_TRUE;
                    }
                }

                return STL_FALSE;
            }
        case QLNC_EXPR_TYPE_OR_FILTER:
            {
                qlncOrFilter    * sOrFilter = NULL;
                stlInt32          i;

                sOrFilter = (qlncOrFilter*)aExpr;
                for( i = 0; i < sOrFilter->mFilterCount; i++ )
                {
                    if( qlncHasAggregationOnExpr( sOrFilter->mFilters[i],
                                                  aSkipSubTableNodeID,
                                                  aCheckNested )
                        == STL_TRUE )
                    {
                        return STL_TRUE;
                    }
                }

                return STL_FALSE;
            }
        case QLNC_EXPR_TYPE_BOOLEAN_FILTER:
            {
                qlncBooleanFilter   * sBooleanFilter    = NULL;

                sBooleanFilter = (qlncBooleanFilter*)aExpr;
                if( qlncHasAggregationOnExpr( sBooleanFilter->mExpr,
                                              aSkipSubTableNodeID,
                                              aCheckNested )
                    == STL_TRUE )
                {
                    return STL_TRUE;
                }

                return STL_FALSE;
            }
        case QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER:
            {
                return STL_FALSE;
            }
        default:
            {
                STL_DASSERT( 0 );
                return STL_FALSE;
            }
    }


    return STL_FALSE;
}


/**
 * @brief Boolean Filter에서 Inner Node에 포함되지 않는 노드 참조 정보를 모두 제거한다.
 * @param[in]   aParamInfo  Parameter Info
 * @param[in]   aExpr       Expression
 * @param[in]   aInnerNode  Candidate Inner Node
 * @param[in]   aOutExpr    Output Expression
 * @param[in]   aEnv        Environment
 */
stlStatus qlncRemoveOuterTableNodeFromRefNodeList( qlncParamInfo    * aParamInfo,
                                                   qlncExprCommon   * aExpr,
                                                   qlncNodeCommon   * aInnerNode,
                                                   qlncExprCommon  ** aOutExpr,
                                                   qllEnv           * aEnv )
{
    qlncExprCommon          * sOutExpr              = NULL;
    qlncConstBooleanFilter  * sConstBooleanFilter   = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInnerNode != NULL, QLL_ERROR_STACK(aEnv) );


#define _QLNC_REMOVE_OUTER_TABLE_NODE_FROM_NODE_LIST( _aRefNodeList )   \
    {                                                                   \
        qlncRefNodeItem     * _sPrevRefNodeItem = NULL;                 \
        qlncRefNodeItem     * _sRefNodeItem     = NULL;                 \
                                                                        \
        _sPrevRefNodeItem = NULL;                                       \
        _sRefNodeItem = (_aRefNodeList)->mHead;                         \
        while( _sRefNodeItem != NULL )                                  \
        {                                                               \
            if( qlncIsExistNode( aInnerNode, _sRefNodeItem->mNode )     \
                == STL_FALSE )                                          \
            {                                                           \
                if( _sPrevRefNodeItem == NULL )                         \
                {                                                       \
                    (_aRefNodeList)->mHead = _sRefNodeItem->mNext;      \
                }                                                       \
                else                                                    \
                {                                                       \
                    _sPrevRefNodeItem->mNext = _sRefNodeItem->mNext;    \
                }                                                       \
                (_aRefNodeList)->mCount--;                              \
            }                                                           \
            else                                                        \
            {                                                           \
                _sPrevRefNodeItem = _sRefNodeItem;                      \
            }                                                           \
                                                                        \
            _sRefNodeItem = _sRefNodeItem->mNext;                       \
        }                                                               \
                                                                        \
        if( (_sPrevRefNodeItem) != NULL )                               \
        {                                                               \
            (_sPrevRefNodeItem)->mNext = NULL;                          \
        }                                                               \
    }


    sOutExpr = aExpr;


    switch( aExpr->mType )
    {
        case QLNC_EXPR_TYPE_VALUE:
        case QLNC_EXPR_TYPE_BIND_PARAM:
        case QLNC_EXPR_TYPE_COLUMN:
            {
                break;
            }
        case QLNC_EXPR_TYPE_FUNCTION:
            {
                qlncExprFunction    * sFunction = NULL;
                stlInt32              i;

                sFunction = (qlncExprFunction*)aExpr;
                for( i = 0; i < sFunction->mArgCount; i++ )
                {
                    STL_TRY( qlncRemoveOuterTableNodeFromRefNodeList(
                                 aParamInfo,
                                 sFunction->mArgs[i],
                                 aInnerNode,
                                 &(sFunction->mArgs[i]),
                                 aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_EXPR_TYPE_CAST:
            {
                qlncExprTypeCast    * sTypeCast = NULL;
                stlInt32              i;

                sTypeCast = (qlncExprTypeCast*)aExpr;
                for( i = 0; i < sTypeCast->mArgCount; i++ )
                {
                    STL_TRY( qlncRemoveOuterTableNodeFromRefNodeList(
                                 aParamInfo,
                                 sTypeCast->mArgs[i],
                                 aInnerNode,
                                 &(sTypeCast->mArgs[i]),
                                 aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_EXPR_TYPE_PSEUDO_COLUMN:
        case QLNC_EXPR_TYPE_PSEUDO_ARGUMENT:
        case QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT:
        case QLNC_EXPR_TYPE_SUB_QUERY:
        case QLNC_EXPR_TYPE_REFERENCE:
        case QLNC_EXPR_TYPE_INNER_COLUMN:
        case QLNC_EXPR_TYPE_OUTER_COLUMN:
        case QLNC_EXPR_TYPE_ROWID_COLUMN:
            {
                break;
            }
        case QLNC_EXPR_TYPE_AGGREGATION:
            {
                qlncExprAggregation * sAggregation  = NULL;
                stlInt32              i;

                sAggregation = (qlncExprAggregation*)aExpr;
                for( i = 0; i < sAggregation->mArgCount; i++ )
                {
                    STL_TRY( qlncRemoveOuterTableNodeFromRefNodeList(
                                 aParamInfo,
                                 sAggregation->mArgs[i],
                                 aInnerNode,
                                 &(sAggregation->mArgs[i]),
                                 aEnv )
                             == STL_SUCCESS );
                }

                if( sAggregation->mFilter != NULL )
                {
                    /* @todo 현재 Aggregation은 Filter가 존재하지 않는다. */

                    STL_TRY( qlncRemoveOuterTableNodeFromRefNodeList(
                                 aParamInfo,
                                 sAggregation->mFilter,
                                 aInnerNode,
                                 &(sAggregation->mFilter),
                                 aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_EXPR_TYPE_CASE_EXPR:
            {
                qlncExprCaseExpr    * sCaseExpr = NULL;
                stlInt32              i;

                sCaseExpr = (qlncExprCaseExpr*)aExpr;
                for( i = 0; i < sCaseExpr->mCount; i++ )
                {
                    STL_TRY( qlncRemoveOuterTableNodeFromRefNodeList(
                                 aParamInfo,
                                 sCaseExpr->mWhenArr[i],
                                 aInnerNode,
                                 &(sCaseExpr->mWhenArr[i]),
                                 aEnv )
                             == STL_SUCCESS );

                    STL_TRY( qlncRemoveOuterTableNodeFromRefNodeList(
                                 aParamInfo,
                                 sCaseExpr->mThenArr[i],
                                 aInnerNode,
                                 &(sCaseExpr->mThenArr[i]),
                                 aEnv )
                             == STL_SUCCESS );
                }

                STL_TRY( qlncRemoveOuterTableNodeFromRefNodeList(
                             aParamInfo,
                             sCaseExpr->mDefResult,
                             aInnerNode,
                             &(sCaseExpr->mDefResult),
                             aEnv )
                         == STL_SUCCESS );

                break;
            }
        case QLNC_EXPR_TYPE_LIST_FUNCTION:
            {
                qlncExprListFunc    * sListFunc = NULL;
                stlInt32              i;

                sListFunc = (qlncExprListFunc*)aExpr;
                for( i = 0; i < sListFunc->mArgCount; i++ )
                {
                    STL_TRY( qlncRemoveOuterTableNodeFromRefNodeList(
                                 aParamInfo,
                                 sListFunc->mArgs[i],
                                 aInnerNode,
                                 &(sListFunc->mArgs[i]),
                                 aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_EXPR_TYPE_LIST_COLUMN:
            {
                qlncExprListCol     * sListCol  = NULL;
                stlInt32              i;

                sListCol = (qlncExprListCol*)aExpr;
                for( i = 0; i < sListCol->mArgCount; i++ )
                {
                    STL_TRY( qlncRemoveOuterTableNodeFromRefNodeList(
                                 aParamInfo,
                                 sListCol->mArgs[i],
                                 aInnerNode,
                                 &(sListCol->mArgs[i]),
                                 aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_EXPR_TYPE_ROW_EXPR:
            {
                qlncExprRowExpr     * sRowExpr  = NULL;
                stlInt32              i;

                sRowExpr = (qlncExprRowExpr*)aExpr;
                for( i = 0; i < sRowExpr->mArgCount; i++ )
                {
                    STL_TRY( qlncRemoveOuterTableNodeFromRefNodeList(
                                 aParamInfo,
                                 sRowExpr->mArgs[i],
                                 aInnerNode,
                                 &(sRowExpr->mArgs[i]),
                                 aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_EXPR_TYPE_AND_FILTER:
            {
                qlncAndFilter   * sAndFilter    = NULL;
                stlInt32          i;

                sAndFilter = (qlncAndFilter*)aExpr;
                for( i = 0; i < sAndFilter->mOrCount; i++ )
                {
                    STL_TRY( qlncRemoveOuterTableNodeFromRefNodeList(
                                 aParamInfo,
                                 (qlncExprCommon*)(sAndFilter->mOrFilters[i]),
                                 aInnerNode,
                                 (qlncExprCommon**)&(sAndFilter->mOrFilters[i]),
                                 aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_EXPR_TYPE_OR_FILTER:
            {
                qlncOrFilter    * sOrFilter = NULL;
                stlInt32          i;

                sOrFilter = (qlncOrFilter*)aExpr;
                for( i = 0; i < sOrFilter->mFilterCount; i++ )
                {
                    STL_TRY( qlncRemoveOuterTableNodeFromRefNodeList(
                                 aParamInfo,
                                 sOrFilter->mFilters[i],
                                 aInnerNode,
                                 &(sOrFilter->mFilters[i]),
                                 aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_EXPR_TYPE_BOOLEAN_FILTER:
            {
                qlncBooleanFilter   * sBooleanFilter    = NULL;

                sBooleanFilter = (qlncBooleanFilter*)aExpr;
                STL_TRY( qlncRemoveOuterTableNodeFromRefNodeList(
                             aParamInfo,
                             sBooleanFilter->mExpr,
                             aInnerNode,
                             &(sBooleanFilter->mExpr),
                             aEnv )
                         == STL_SUCCESS );

                if( sBooleanFilter->mLeftRefNodeList != NULL )
                {
                    _QLNC_REMOVE_OUTER_TABLE_NODE_FROM_NODE_LIST( sBooleanFilter->mLeftRefNodeList );
                    if( sBooleanFilter->mLeftRefNodeList->mCount == 0 )
                    {
                        sBooleanFilter->mLeftRefNodeList = NULL;
                    }
                }

                if( sBooleanFilter->mRightRefNodeList != NULL )
                {
                    _QLNC_REMOVE_OUTER_TABLE_NODE_FROM_NODE_LIST( sBooleanFilter->mRightRefNodeList );
                    if( sBooleanFilter->mRightRefNodeList->mCount == 0 )
                    {
                        sBooleanFilter->mRightRefNodeList = NULL;
                    }
                }

                if( (sBooleanFilter->mLeftRefNodeList == NULL) ||
                    (sBooleanFilter->mRightRefNodeList == NULL) )
                {
                    sBooleanFilter->mLeftRefNodeList = NULL;
                    sBooleanFilter->mRightRefNodeList = NULL;
                    sBooleanFilter->mPossibleJoinCond = STL_FALSE;
                }

                _QLNC_REMOVE_OUTER_TABLE_NODE_FROM_NODE_LIST( &(sBooleanFilter->mRefNodeList) );

                if( sBooleanFilter->mRefNodeList.mCount == 0 )
                {
                    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                STL_SIZEOF( qlncConstBooleanFilter ),
                                                (void**)&sConstBooleanFilter,
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    QLNC_SET_EXPR_COMMON( &sConstBooleanFilter->mCommon,
                                          &QLL_CANDIDATE_MEM( aEnv ),
                                          aEnv,
                                          QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER,
                                          sBooleanFilter->mCommon.mExprPhraseFlag,
                                          sBooleanFilter->mCommon.mIterationTime,
                                          sBooleanFilter->mCommon.mPosition,
                                          sBooleanFilter->mCommon.mColumnName,
                                          DTL_TYPE_BOOLEAN );

                    sConstBooleanFilter->mExpr = sBooleanFilter->mExpr;
                    sConstBooleanFilter->mSubQueryExprList = sBooleanFilter->mSubQueryExprList;

                    sOutExpr = (qlncExprCommon*)sConstBooleanFilter;
                }

                break;
            }
        case QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER:
            {
                break;
            }
        default:
            {
                STL_DASSERT( 0 );
                break;
            }
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
 * @brief Expression으로부터 SubQuery를 찾아 SubQueryExprList에 달아준다.
 * @param[in]       aParamInfo          Parameter Info
 * @param[in]       aExpr               Candidate Expression
 * @param[in,out]   aSubQueryExprList   SubQuery Expression List
 * @param[in]       aEnv                Environment
 */
stlStatus qlncFindAddSubQueryExprToSubQueryExprList( qlncParamInfo      * aParamInfo,
                                                     qlncExprCommon     * aExpr,
                                                     qlncRefExprList    * aSubQueryExprList,
                                                     qllEnv             * aEnv )
{
    stlInt32      i;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSubQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );


    switch( aExpr->mType )
    {
        case QLNC_EXPR_TYPE_VALUE:
        case QLNC_EXPR_TYPE_BIND_PARAM:
        case QLNC_EXPR_TYPE_COLUMN:
            {
                break;
            }
        case QLNC_EXPR_TYPE_FUNCTION:
            {
                qlncExprFunction    * sFunction = NULL;

                sFunction = (qlncExprFunction*)aExpr;
                for( i = 0; i < sFunction->mArgCount; i++ )
                {
                    STL_TRY( qlncFindAddSubQueryExprToSubQueryExprList( aParamInfo,
                                                                        sFunction->mArgs[i],
                                                                        aSubQueryExprList,
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
                    STL_TRY( qlncFindAddSubQueryExprToSubQueryExprList( aParamInfo,
                                                                        sTypeCast->mArgs[i],
                                                                        aSubQueryExprList,
                                                                        aEnv )
                             == STL_SUCCESS );
                }
                break;
            }
        case QLNC_EXPR_TYPE_PSEUDO_COLUMN:
        case QLNC_EXPR_TYPE_PSEUDO_ARGUMENT:
        case QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT:
            {
                break;
            }
        case QLNC_EXPR_TYPE_SUB_QUERY:
            {
                qlncRefExprItem * sRefExprItem  = NULL;

                STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                            STL_SIZEOF( qlncRefExprItem ),
                                            (void**)&sRefExprItem,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );
                sRefExprItem->mExpr = aExpr;
                sRefExprItem->mNext = NULL;

                QLNC_APPEND_ITEM_TO_LIST( aSubQueryExprList, sRefExprItem );
                break;
            }
        case QLNC_EXPR_TYPE_REFERENCE:
            {
                break;
            }
        case QLNC_EXPR_TYPE_INNER_COLUMN:
            {
                if( ((qlncExprInnerColumn*)aExpr)->mOrgExpr != NULL )
                {
                    STL_TRY( qlncFindAddSubQueryExprToSubQueryExprList( aParamInfo,
                                                                        ((qlncExprInnerColumn*)aExpr)->mOrgExpr,
                                                                        aSubQueryExprList,
                                                                        aEnv )
                             == STL_SUCCESS );
                }
                break;
            }
        case QLNC_EXPR_TYPE_OUTER_COLUMN:
        case QLNC_EXPR_TYPE_ROWID_COLUMN:
            {
                break;
            }
        case QLNC_EXPR_TYPE_AGGREGATION:
            {
                qlncExprAggregation * sAggregation  = NULL;

                sAggregation = (qlncExprAggregation*)aExpr;
                for( i = 0; i < sAggregation->mArgCount; i++ )
                {
                    STL_TRY( qlncFindAddSubQueryExprToSubQueryExprList( aParamInfo,
                                                                        sAggregation->mArgs[i],
                                                                        aSubQueryExprList,
                                                                        aEnv )
                             == STL_SUCCESS );
                }

                if( sAggregation->mFilter != NULL )
                {
                    /* @todo Aggregation Filter는 향후 구현한다. */
                    STL_TRY( qlncFindAddSubQueryExprToSubQueryExprList( aParamInfo,
                                                                        sAggregation->mFilter,
                                                                        aSubQueryExprList,
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
                    STL_TRY( qlncFindAddSubQueryExprToSubQueryExprList( aParamInfo,
                                                                        sCaseExpr->mWhenArr[i],
                                                                        aSubQueryExprList,
                                                                        aEnv )
                             == STL_SUCCESS );

                    STL_TRY( qlncFindAddSubQueryExprToSubQueryExprList( aParamInfo,
                                                                        sCaseExpr->mThenArr[i],
                                                                        aSubQueryExprList,
                                                                        aEnv )
                             == STL_SUCCESS );
                }

                STL_TRY( qlncFindAddSubQueryExprToSubQueryExprList( aParamInfo,
                                                                    sCaseExpr->mDefResult,
                                                                    aSubQueryExprList,
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
                    STL_TRY( qlncFindAddSubQueryExprToSubQueryExprList( aParamInfo,
                                                                        sListFunc->mArgs[i],
                                                                        aSubQueryExprList,
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
                    STL_TRY( qlncFindAddSubQueryExprToSubQueryExprList( aParamInfo,
                                                                        sListCol->mArgs[i],
                                                                        aSubQueryExprList,
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
                    STL_TRY( qlncFindAddSubQueryExprToSubQueryExprList( aParamInfo,
                                                                        sRowExpr->mArgs[i],
                                                                        aSubQueryExprList,
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
                    STL_TRY( qlncFindAddSubQueryExprToSubQueryExprList( aParamInfo,
                                                                        (qlncExprCommon*)(sAndFilter->mOrFilters[i]),
                                                                        aSubQueryExprList,
                                                                        aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_EXPR_TYPE_OR_FILTER:
            {
                qlncRefExprItem * sRefExprItem      = NULL;
                qlncRefExprItem * sNewRefExprItem   = NULL;

                if( ((qlncOrFilter*)aExpr)->mSubQueryExprList != NULL )
                {
                    sRefExprItem = ((qlncOrFilter*)aExpr)->mSubQueryExprList->mHead;
                    while( sRefExprItem != NULL )
                    {
                        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                    STL_SIZEOF( qlncRefExprItem ),
                                                    (void**)&sNewRefExprItem,
                                                    KNL_ENV(aEnv) )
                                 == STL_SUCCESS );

                        sNewRefExprItem->mExpr = sRefExprItem->mExpr;
                        sNewRefExprItem->mNext = NULL;

                        QLNC_APPEND_ITEM_TO_LIST( aSubQueryExprList, sNewRefExprItem );

                        sRefExprItem = sRefExprItem->mNext;
                    }
                }

                break;
            }
        case QLNC_EXPR_TYPE_BOOLEAN_FILTER:
        case QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER:
            {
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


/** @} qlnc */
