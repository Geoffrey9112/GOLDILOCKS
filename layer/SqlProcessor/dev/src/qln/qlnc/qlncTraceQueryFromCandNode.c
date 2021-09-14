/*******************************************************************************
 * qlncTraceQueryFromCandNode.c
 *
 * Copyright (c) 2013, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlncTraceQueryFromCandNode.c 9742 2013-10-01 01:45:14Z bsyou $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file qlncTraceQueryFromCandNode.c
 * @brief SQL Processor Layer Optimizer Trace Query From Candidate Node
 */

#include <qll.h>
#include <qlDef.h>



/**
 * @addtogroup qlnc
 * @{
 */


/**
 * @brief Trace Query String From Candidate Node Function List
 */
const qlncTraceQueryFromCandNodeFuncPtr qlncTraceQueryFromCandNodeFuncList[ QLNC_NODE_TYPE_MAX ] =
{
    qlncTraceQueryFromCandNodeQuerySet,         /**< QLNC_NODE_TYPE_QUERY_SET */
    qlncTraceQueryFromCandNodeSetOP,            /**< QLNC_NODE_TYPE_SET_OP */
    qlncTraceQueryFromCandNodeQuerySpec,        /**< QLNC_NODE_TYPE_QUERY_SPEC */
    qlncTraceQueryFromCandNodeBaseTable,        /**< QLNC_NODE_TYPE_BASE_TABLE */
    qlncTraceQueryFromCandNodeSubTable,         /**< QLNC_NODE_TYPE_SUB_TABLE */
    qlncTraceQueryFromCandNodeJoinTable,        /**< QLNC_NODE_TYPE_JOIN_TABLE */
    qlncTraceQueryFromCandNodeFlatInstant,      /**< QLNC_NODE_TYPE_FLAT_INSTANT */
    qlncTraceQueryFromCandNodeSortInstant,      /**< QLNC_NODE_TYPE_SORT_INSTANT */
    qlncTraceQueryFromCandNodeHashInstant,      /**< QLNC_NODE_TYPE_HASH_INSTANT */
    NULL,                                       /**< QLNC_NODE_TYPE_CREATE_NODE_MAX */
    qlncTraceQueryFromCandNodeInnerJoinTable,   /**< QLNC_NODE_TYPE_INNER_JOIN_TABLE */
    qlncTraceQueryFromCandNodeOuterJoinTable,   /**< QLNC_NODE_TYPE_OUTER_JOIN_TABLE */
    qlncTraceQueryFromCandNodeSemiJoinTable,    /**< QLNC_NODE_TYPE_SEMI_JOIN_TABLE */
    qlncTraceQueryFromCandNodeAntiSemiJoinTable /**< QLNC_NODE_TYPE_ANTI_SEMI_JOIN_TABLE */
};


/**
 * @brief Query Set Candidate Node로부터 Query 구문을 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCandNode       Candidate Node
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceQueryFromCandNodeQuerySet( qlncStringBlock   * aStringBlock,
                                              qlncNodeCommon    * aCandNode,
                                              qllEnv            * aEnv )
{
    stlInt32              i;
    qlncQuerySet        * sCandQuerySet     = NULL;
    qlncSetOP           * sCandSetOP        = NULL;
    qlncSortInstantNode * sCandOrderBy      = NULL;
    qlncRefExprItem     * sRefExprItem      = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode->mNodeType == QLNC_NODE_TYPE_QUERY_SET, QLL_ERROR_STACK(aEnv) );


    /* Query Set Candidate Node 설정 */
    sCandQuerySet = (qlncQuerySet*)aCandNode;

    /* Find Child Set OP And Child Order By */
    if( sCandQuerySet->mChildNode->mNodeType == QLNC_NODE_TYPE_SET_OP )
    {
        sCandSetOP = (qlncSetOP*)(sCandQuerySet->mChildNode);
        sCandOrderBy = NULL;
    }
    else
    {
        STL_DASSERT( sCandQuerySet->mChildNode->mNodeType == QLNC_NODE_TYPE_SORT_INSTANT );
        STL_DASSERT( ((qlncSortInstantNode*)(sCandQuerySet->mChildNode))->mChildNode->mNodeType == QLNC_NODE_TYPE_SET_OP );
        sCandSetOP = (qlncSetOP*)(((qlncSortInstantNode*)(sCandQuerySet->mChildNode))->mChildNode);
        sCandOrderBy = (qlncSortInstantNode*)(sCandQuerySet->mChildNode);
    }

    /* Set OP 출력 */
    STL_TRY( qlncTraceQueryFromCandNodeFuncList[ sCandSetOP->mNodeCommon.mNodeType ](
                 aStringBlock,
                 (qlncNodeCommon*)sCandSetOP,
                 aEnv )
             == STL_SUCCESS );

    /* Order By 출력 */
    if( sCandOrderBy != NULL )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     " ORDER BY " )
                 == STL_SUCCESS );

        i = 0;
        sRefExprItem = sCandOrderBy->mKeyColList->mHead;
        while( sRefExprItem != NULL )
        {
            /* Expression 출력 */
            STL_TRY( qlncTraceQueryFromCandExprFuncList[ sRefExprItem->mExpr->mType ](
                         aStringBlock,
                         sRefExprItem->mExpr,
                         aEnv )
                     == STL_SUCCESS );

            STL_TRY( qlncTraceAppendStringToStringBlock(
                         aStringBlock,
                         aEnv,
                         " %s %s",
                         ( DTL_GET_KEY_FLAG_SORT_ORDER( sCandOrderBy->mKeyFlags[i] )
                           == DTL_KEYCOLUMN_INDEX_ORDER_ASC
                           ? "ASC" : "DESC" ),
                         ( DTL_GET_KEY_FLAG_NULL_ORDER( sCandOrderBy->mKeyFlags[i] )
                           == DTL_KEYCOLUMN_INDEX_NULL_ORDER_FIRST
                           ? "NULLS FIRST" : "NULLS LAST" ))
                     == STL_SUCCESS );

            if( sRefExprItem->mNext != NULL )
            {
                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             ", " )
                         == STL_SUCCESS );
            }

            i++;
            sRefExprItem = sRefExprItem->mNext;
        }

    }

    /* offset */
    if( sCandQuerySet->mOffset != NULL )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     " OFFSET " )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceQueryFromCandExprFuncList[ sCandQuerySet->mOffset->mType ](
                     aStringBlock,
                     sCandQuerySet->mOffset,
                     aEnv )
                 == STL_SUCCESS );
    }

    /* limit */
    if( sCandQuerySet->mLimit != NULL )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     " LIMIT " )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceQueryFromCandExprFuncList[ sCandQuerySet->mLimit->mType ](
                     aStringBlock,
                     sCandQuerySet->mLimit,
                     aEnv )
                 == STL_SUCCESS );
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Set OP Candidate Node로부터 Query 구문을 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCandNode       Candidate Node
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceQueryFromCandNodeSetOP( qlncStringBlock  * aStringBlock,
                                           qlncNodeCommon   * aCandNode,
                                           qllEnv           * aEnv )
{
    stlInt32      i;
    qlncSetOP   * sCandSetOP    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode->mNodeType == QLNC_NODE_TYPE_SET_OP, QLL_ERROR_STACK(aEnv) );


    /* Set OP Candidate Node 설정 */
    sCandSetOP = (qlncSetOP*)aCandNode;

    for( i = 0; i < sCandSetOP->mChildCount; i++ )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "(" )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceQueryFromCandNodeFuncList[ sCandSetOP->mChildNodeArr[i]->mNodeType ](
                     aStringBlock,
                     sCandSetOP->mChildNodeArr[i],
                     aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     ")" )
                 == STL_SUCCESS );

        if( (i + 1) < sCandSetOP->mChildCount )
        {
            if( sCandSetOP->mSetType == QLNC_SET_TYPE_UNION )
            {
                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             " UNION " )
                         == STL_SUCCESS );
            }
            else if( sCandSetOP->mSetType == QLNC_SET_TYPE_EXCEPT )
            {
                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             " EXCEPT " )
                         == STL_SUCCESS );
            }
            else
            {
                STL_DASSERT( sCandSetOP->mSetType == QLNC_SET_TYPE_INTERSECT );
                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             " INTERSECT " )
                         == STL_SUCCESS );
            }

            if( sCandSetOP->mSetOption == QLNC_SET_OPTION_ALL )
            {
                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             "ALL " )
                         == STL_SUCCESS );
            }
            else
            {
                STL_DASSERT( sCandSetOP->mSetOption == QLNC_SET_OPTION_DISTINCT );
                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             "DISTINCT " )
                         == STL_SUCCESS );
            }
        }
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Query Spec Candidate Node로부터 Query 구문을 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCandNode       Candidate Node
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceQueryFromCandNodeQuerySpec( qlncStringBlock  * aStringBlock,
                                               qlncNodeCommon   * aCandNode,
                                               qllEnv           * aEnv )
{
    stlBool               sIsDistinct;
    stlInt32              i;
    qlncQuerySpec       * sCandQuerySpec    = NULL;
    qlncNodeCommon      * sChildNode        = NULL;
    qlncHashInstantNode * sCandGroupBy      = NULL;
    qlncSortInstantNode * sCandOrderBy      = NULL;
    qlncRefExprItem     * sRefExprItem      = NULL;
    qlncAndFilter       * sAndFilter        = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode->mNodeType == QLNC_NODE_TYPE_QUERY_SPEC, QLL_ERROR_STACK(aEnv) );


    /* Query Spec Candidate Node 설정 */
    sCandQuerySpec = (qlncQuerySpec*)aCandNode;

    /* Table, Group By, Order By, Distinct 구분 */
    sIsDistinct = STL_FALSE;
    sChildNode = sCandQuerySpec->mChildNode;

    if( sChildNode->mNodeType == QLNC_NODE_TYPE_SORT_INSTANT )
    {
        if( ((qlncSortInstantNode*)sChildNode)->mSortTableAttr.mDistinct == STL_TRUE )
        {
            /* Distinct & Order By */
            sIsDistinct = STL_TRUE;
            sCandOrderBy = (qlncSortInstantNode*)sChildNode;
            sChildNode = sCandOrderBy->mChildNode;
        }
        else
        {
            /* Order By */
            sIsDistinct = STL_FALSE;
            sCandOrderBy = (qlncSortInstantNode*)sChildNode;
            sChildNode = sCandOrderBy->mChildNode;
        }
    }
    else
    {
        sCandOrderBy = NULL;
    }

    if( sChildNode->mNodeType == QLNC_NODE_TYPE_HASH_INSTANT )
    {
        if( ((qlncHashInstantNode*)sChildNode)->mInstantType == QLNC_INSTANT_TYPE_GROUP )
        {
            /* Group By */
            sCandGroupBy = (qlncHashInstantNode*)sChildNode;
            sChildNode = sCandGroupBy->mChildNode;
        }
        else
        {
            /* Distinct */
            STL_DASSERT( ((qlncHashInstantNode*)sChildNode)->mInstantType == QLNC_INSTANT_TYPE_DISTINCT );
            sIsDistinct = STL_TRUE;
            sChildNode = ((qlncHashInstantNode*)sChildNode)->mChildNode;

            if( sChildNode->mNodeType == QLNC_NODE_TYPE_HASH_INSTANT )
            {
                STL_DASSERT( ((qlncHashInstantNode*)sChildNode)->mInstantType
                             == QLNC_INSTANT_TYPE_GROUP );

                sCandGroupBy = (qlncHashInstantNode*)sChildNode;
                sChildNode = sCandGroupBy->mChildNode;
            }
            else
            {
                sCandGroupBy = NULL;
            }
        }
    }
    else
    {
        sCandGroupBy = NULL;
    }

    /* select */
    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "SELECT " )
             == STL_SUCCESS );

    if( sIsDistinct == STL_TRUE )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "DISTINCT " )
                 == STL_SUCCESS );
    }

    /* Target 출력 */
    for( i = 0; i < sCandQuerySpec->mTargetCount; i++ )
    {
        /* Expression 출력 */
        STL_TRY( qlncTraceQueryFromCandExprFuncList[ sCandQuerySpec->mTargetArr[i].mExpr->mType ](
                     aStringBlock,
                     sCandQuerySpec->mTargetArr[i].mExpr,
                     aEnv )
                 == STL_SUCCESS );

        /* Target의 Alias Name 출력 */
        if( sCandQuerySpec->mTargetArr[i].mDisplayName != NULL )
        {
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         " AS " )
                     == STL_SUCCESS );

            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "\"%s\"",
                                                         sCandQuerySpec->mTargetArr[i].mDisplayName )
                     == STL_SUCCESS );
        }

        if( (i + 1) < sCandQuerySpec->mTargetCount )
        {
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         ", " )
                     == STL_SUCCESS );
        }
    }

    /* from */
    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 " FROM " )
             == STL_SUCCESS );

    STL_TRY( qlncTraceQueryFromCandNodeFuncList[ sChildNode->mNodeType ](
                 aStringBlock,
                 sChildNode,
                 aEnv )
             == STL_SUCCESS );

    /* where */
    if( sChildNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE )
    {
        sAndFilter = ((qlncBaseTableNode*)sChildNode)->mFilter;
    }
    else if( sChildNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE )
    {
        sAndFilter = ((qlncSubTableNode*)sChildNode)->mFilter;
    }
    else
    {
        STL_DASSERT( sChildNode->mNodeType == QLNC_NODE_TYPE_JOIN_TABLE );
        sAndFilter = ((qlncJoinTableNode*)sChildNode)->mFilter;
    }

    if( sAndFilter != NULL )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     " WHERE " )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceQueryFromCandExprAndFilter(
                     aStringBlock,
                     (qlncExprCommon*)sAndFilter,
                     aEnv )
                 == STL_SUCCESS );
    }


    /* group by */
    if( sCandGroupBy != NULL )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     " GROUP BY " )
                 == STL_SUCCESS );

        sRefExprItem = sCandGroupBy->mKeyColList->mHead;
        while( sRefExprItem != NULL )
        {
            /* Expression 출력 */
            STL_TRY( qlncTraceQueryFromCandExprFuncList[ sRefExprItem->mExpr->mType ](
                         aStringBlock,
                         sRefExprItem->mExpr,
                         aEnv )
                     == STL_SUCCESS );

            if( sRefExprItem->mNext != NULL )
            {
                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             ", " )
                         == STL_SUCCESS );
            }

            sRefExprItem = sRefExprItem->mNext;
        }

        if( sCandGroupBy->mFilter != NULL )
        {
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         " HAVING " )
                     == STL_SUCCESS );

            STL_TRY( qlncTraceQueryFromCandExprAndFilter(
                         aStringBlock,
                         (qlncExprCommon*)(sCandGroupBy->mFilter),
                         aEnv )
                     == STL_SUCCESS );
        }
    }

    /* order by */
    if( sCandOrderBy != NULL )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     " ORDER BY " )
                 == STL_SUCCESS );

        i = 0;
        sRefExprItem = sCandOrderBy->mKeyColList->mHead;
        while( sRefExprItem != NULL )
        {
            /* Expression 출력 */
            STL_TRY( qlncTraceQueryFromCandExprFuncList[ sRefExprItem->mExpr->mType ](
                         aStringBlock,
                         sRefExprItem->mExpr,
                         aEnv )
                     == STL_SUCCESS );

            STL_TRY( qlncTraceAppendStringToStringBlock(
                         aStringBlock,
                         aEnv,
                         " %s %s",
                         ( DTL_GET_KEY_FLAG_SORT_ORDER( sCandOrderBy->mKeyFlags[i] )
                           == DTL_KEYCOLUMN_INDEX_ORDER_ASC
                           ? "ASC" : "DESC" ),
                         ( DTL_GET_KEY_FLAG_NULL_ORDER( sCandOrderBy->mKeyFlags[i] )
                           == DTL_KEYCOLUMN_INDEX_NULL_ORDER_FIRST
                           ? "NULLS FIRST" : "NULLS LAST" ))
                     == STL_SUCCESS );

            if( sRefExprItem->mNext != NULL )
            {
                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             ", " )
                         == STL_SUCCESS );
            }

            i++;
            sRefExprItem = sRefExprItem->mNext;
        }

    }

    /* offset */
    if( sCandQuerySpec->mOffset != NULL )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     " OFFSET " )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceQueryFromCandExprFuncList[ sCandQuerySpec->mOffset->mType ](
                     aStringBlock,
                     sCandQuerySpec->mOffset,
                     aEnv )
                 == STL_SUCCESS );
    }

    /* limit */
    if( sCandQuerySpec->mLimit != NULL )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     " LIMIT " )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceQueryFromCandExprFuncList[ sCandQuerySpec->mLimit->mType ](
                     aStringBlock,
                     sCandQuerySpec->mLimit,
                     aEnv )
                 == STL_SUCCESS );
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Base Table Candidate Node로부터 Query 구문을 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCandNode       Candidate Node
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceQueryFromCandNodeBaseTable( qlncStringBlock  * aStringBlock,
                                               qlncNodeCommon   * aCandNode,
                                               qllEnv           * aEnv )
{
    qlncBaseTableNode   * sCandBaseTable    = NULL;
    qlncRelationName    * sRelationName     = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE, QLL_ERROR_STACK(aEnv) );


    /* Candidate Base Table Node 설정 */
    sCandBaseTable = (qlncBaseTableNode*)aCandNode;

    /* Relation Name 설정 */
    sRelationName = &(sCandBaseTable->mTableInfo->mRelationName);

    if( stlStrcmp( ellGetTableName( sCandBaseTable->mTableInfo->mTableHandle ),
                   sRelationName->mTable )
        == 0 )
    {
        /* Catalog Name 출력 */
        if( sRelationName->mCatalog != NULL )
        {
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "\"%s\".",
                                                         sRelationName->mCatalog )
                     == STL_SUCCESS );
        }

        /* Schema Name 출력 */
        if( sRelationName->mSchema != NULL )
        {
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "\"%s\".",
                                                         sRelationName->mSchema )
                     == STL_SUCCESS );
        }

        /* Table Name 출력 */
        if( sRelationName->mTable != NULL )
        {
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "\"%s\"",
                                                         sRelationName->mTable )
                     == STL_SUCCESS );
        }
    }
    else
    {
        /* Alias Name이 있는 경우 */
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "\"%s\" \"%s\"",
                                                     ellGetTableName( sCandBaseTable->mTableInfo->mTableHandle ),
                                                     sRelationName->mTable )
                 == STL_SUCCESS );
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Sub Table Candidate Node로부터 Query 구문을 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCandNode       Candidate Node
 * @param[in]   aEnv            Environment
 */

stlStatus qlncTraceQueryFromCandNodeSubTable( qlncStringBlock   * aStringBlock,
                                              qlncNodeCommon    * aCandNode,
                                              qllEnv            * aEnv )
{
    qlncSubTableNode    * sCandSubTable = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE, QLL_ERROR_STACK(aEnv) );


    /* Candidate Sub Table Node 설정 */
    sCandSubTable = (qlncSubTableNode*)aCandNode;

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "( " )
             == STL_SUCCESS );

    STL_TRY( qlncTraceQueryFromCandNodeFuncList[ sCandSubTable->mQueryNode->mNodeType ](
                 aStringBlock,
                 sCandSubTable->mQueryNode,
                 aEnv )
             == STL_SUCCESS );

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 " )" )
             == STL_SUCCESS );

    if( sCandSubTable->mRelationName != NULL )
    {
        if( sCandSubTable->mRelationName->mTable != NULL )
        {
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                         aEnv,
                         " \"%s\"",
                         sCandSubTable->mRelationName->mTable )
                     == STL_SUCCESS );
        }
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Join Table Candidate Node로부터 Query 구문을 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCandNode       Candidate Node
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceQueryFromCandNodeJoinTable( qlncStringBlock  * aStringBlock,
                                               qlncNodeCommon   * aCandNode,
                                               qllEnv           * aEnv )
{
    stlInt32              i;
    qlncJoinTableNode   * sCandJoinTable    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode->mNodeType == QLNC_NODE_TYPE_JOIN_TABLE, QLL_ERROR_STACK(aEnv) );


#define QLNC_TRACE_SET_NODE_STRING( _aNode )                                    \
    {                                                                           \
        if( (_aNode)->mNodeType == QLNC_NODE_TYPE_JOIN_TABLE )                  \
        {                                                                       \
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,          \
                                                         aEnv,                  \
                                                         "( " )                 \
                     == STL_SUCCESS );                                          \
                                                                                \
            STL_TRY( qlncTraceQueryFromCandNodeFuncList[ (_aNode)->mNodeType ]( \
                         aStringBlock,                                          \
                         (_aNode),                                              \
                         aEnv )                                                 \
                     == STL_SUCCESS );                                          \
                                                                                \
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,          \
                                                         aEnv,                  \
                                                         " )" )                 \
                     == STL_SUCCESS );                                          \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            STL_TRY( qlncTraceQueryFromCandNodeFuncList[ (_aNode)->mNodeType ]( \
                         aStringBlock,                                          \
                         (_aNode),                                              \
                         aEnv )                                                 \
                     == STL_SUCCESS );                                          \
        }                                                                       \
    }


    /* Join Table Node 설정 */
    sCandJoinTable = (qlncJoinTableNode*)aCandNode;

    switch( sCandJoinTable->mJoinType )
    {
        case QLNC_JOIN_TYPE_INNER:
            /* Inner Join */
            if( sCandJoinTable->mNodeArray.mCurCnt == 2 )
            {
                QLNC_TRACE_SET_NODE_STRING( sCandJoinTable->mNodeArray.mNodeArr[0] );

                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             " INNER JOIN " )
                         == STL_SUCCESS );

                QLNC_TRACE_SET_NODE_STRING( sCandJoinTable->mNodeArray.mNodeArr[1] );
            }
            else
            {
                for( i = 1; i < sCandJoinTable->mNodeArray.mCurCnt; i++ )
                {
                    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                                 aEnv,
                                                                 "( " )
                             == STL_SUCCESS );
                }

                QLNC_TRACE_SET_NODE_STRING( sCandJoinTable->mNodeArray.mNodeArr[0] );

                for( i = 1; i < sCandJoinTable->mNodeArray.mCurCnt; i++ )
                {
                    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                                 aEnv,
                                                                 " INNER JOIN " )
                             == STL_SUCCESS );

                    QLNC_TRACE_SET_NODE_STRING( sCandJoinTable->mNodeArray.mNodeArr[i] );

                    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                                 aEnv,
                                                                 " )" )
                             == STL_SUCCESS );
                }
            }

            if( sCandJoinTable->mJoinCond != NULL )
            {
                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             " ON " )
                         == STL_SUCCESS );

                STL_TRY( qlncTraceQueryFromCandExprAndFilter( aStringBlock,
                                                              (qlncExprCommon*)(sCandJoinTable->mJoinCond),
                                                              aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_JOIN_TYPE_OUTER:
            /* Outer Join */
            STL_DASSERT( sCandJoinTable->mNodeArray.mCurCnt == 2 );

            QLNC_TRACE_SET_NODE_STRING( sCandJoinTable->mNodeArray.mNodeArr[0] );

            if( sCandJoinTable->mJoinDirect == QLNC_JOIN_DIRECT_LEFT )
            {
                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             " LEFT OUTER JOIN " )
                         == STL_SUCCESS );
            }
            else if( sCandJoinTable->mJoinDirect == QLNC_JOIN_DIRECT_RIGHT )
            {
                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             " RIGHT OUTER JOIN " )
                         == STL_SUCCESS );
            }
            else
            {
                STL_DASSERT( sCandJoinTable->mJoinDirect == QLNC_JOIN_DIRECT_FULL );
                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             " FULL OUTER JOIN " )
                         == STL_SUCCESS );
            }

            QLNC_TRACE_SET_NODE_STRING( sCandJoinTable->mNodeArray.mNodeArr[1] );

            if( sCandJoinTable->mJoinCond != NULL )
            {
                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             " ON " )
                         == STL_SUCCESS );

                STL_TRY( qlncTraceQueryFromCandExprAndFilter( aStringBlock,
                                                              (qlncExprCommon*)(sCandJoinTable->mJoinCond),
                                                              aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_JOIN_TYPE_SEMI:
            /* Semi Join */
            STL_DASSERT( sCandJoinTable->mNodeArray.mCurCnt == 2 );

            QLNC_TRACE_SET_NODE_STRING( sCandJoinTable->mNodeArray.mNodeArr[0] );

            if( sCandJoinTable->mJoinDirect == QLNC_JOIN_DIRECT_LEFT )
            {
                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             " LEFT SEMI JOIN " )
                         == STL_SUCCESS );
            }
            else
            {
                STL_DASSERT( sCandJoinTable->mJoinDirect == QLNC_JOIN_DIRECT_RIGHT );
                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             " RIGHT SEMI JOIN " )
                         == STL_SUCCESS );
            }

            QLNC_TRACE_SET_NODE_STRING( sCandJoinTable->mNodeArray.mNodeArr[1] );

            if( sCandJoinTable->mJoinCond != NULL )
            {
                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             " ON " )
                         == STL_SUCCESS );

                STL_TRY( qlncTraceQueryFromCandExprAndFilter( aStringBlock,
                                                              (qlncExprCommon*)(sCandJoinTable->mJoinCond),
                                                              aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_JOIN_TYPE_ANTI_SEMI:
            /* Anti-Semi Join */
            STL_DASSERT( sCandJoinTable->mNodeArray.mCurCnt == 2 );

            QLNC_TRACE_SET_NODE_STRING( sCandJoinTable->mNodeArray.mNodeArr[0] );

            if( sCandJoinTable->mJoinDirect == QLNC_JOIN_DIRECT_LEFT )
            {
                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             " LEFT ANTI SEMI JOIN " )
                         == STL_SUCCESS );
            }
            else
            {
                STL_DASSERT( sCandJoinTable->mJoinDirect == QLNC_JOIN_DIRECT_RIGHT );
                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             " RIGHT ANTI SEMI JOIN " )
                         == STL_SUCCESS );
            }

            QLNC_TRACE_SET_NODE_STRING( sCandJoinTable->mNodeArray.mNodeArr[1] );

            if( sCandJoinTable->mJoinCond != NULL )
            {
                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             " ON " )
                         == STL_SUCCESS );

                STL_TRY( qlncTraceQueryFromCandExprAndFilter( aStringBlock,
                                                              (qlncExprCommon*)(sCandJoinTable->mJoinCond),
                                                              aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_JOIN_TYPE_ANTI_SEMI_NA:
            /* Null Aware Anti-Semi Join */
            STL_DASSERT( sCandJoinTable->mNodeArray.mCurCnt == 2 );

            QLNC_TRACE_SET_NODE_STRING( sCandJoinTable->mNodeArray.mNodeArr[0] );

            if( sCandJoinTable->mJoinDirect == QLNC_JOIN_DIRECT_LEFT )
            {
                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             " LEFT ANTI SEMI JOIN NA " )
                         == STL_SUCCESS );
            }
            else
            {
                STL_DASSERT( sCandJoinTable->mJoinDirect == QLNC_JOIN_DIRECT_RIGHT );
                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             " RIGHT ANTI SEMI JOIN NA " )
                         == STL_SUCCESS );
            }

            QLNC_TRACE_SET_NODE_STRING( sCandJoinTable->mNodeArray.mNodeArr[1] );

            if( sCandJoinTable->mJoinCond != NULL )
            {
                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             " ON " )
                         == STL_SUCCESS );

                STL_TRY( qlncTraceQueryFromCandExprAndFilter( aStringBlock,
                                                              (qlncExprCommon*)(sCandJoinTable->mJoinCond),
                                                              aEnv )
                         == STL_SUCCESS );
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
 * @brief Flat Instant Candidate Node로부터 Query 구문을 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCandNode       Candidate Node
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceQueryFromCandNodeFlatInstant( qlncStringBlock    * aStringBlock,
                                                 qlncNodeCommon     * aCandNode,
                                                 qllEnv             * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode->mNodeType == QLNC_NODE_TYPE_FLAT_INSTANT, QLL_ERROR_STACK(aEnv) );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Sort Instant Candidate Node로부터 Query 구문을 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCandNode       Candidate Node
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceQueryFromCandNodeSortInstant( qlncStringBlock    * aStringBlock,
                                                 qlncNodeCommon     * aCandNode,
                                                 qllEnv             * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode->mNodeType == QLNC_NODE_TYPE_SORT_INSTANT, QLL_ERROR_STACK(aEnv) );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Hash Instant Candidate Node로부터 Query 구문을 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCandNode       Candidate Node
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceQueryFromCandNodeHashInstant( qlncStringBlock    * aStringBlock,
                                                 qlncNodeCommon     * aCandNode,
                                                 qllEnv             * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode->mNodeType == QLNC_NODE_TYPE_HASH_INSTANT, QLL_ERROR_STACK(aEnv) );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Inner Join Table Candidate Node로부터 Query 구문을 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCandNode       Candidate Node
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceQueryFromCandNodeInnerJoinTable( qlncStringBlock * aStringBlock,
                                                    qlncNodeCommon  * aCandNode,
                                                    qllEnv          * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode->mNodeType == QLNC_NODE_TYPE_INNER_JOIN_TABLE, QLL_ERROR_STACK(aEnv) );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Outer Join Table Candidate Node로부터 Query 구문을 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCandNode       Candidate Node
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceQueryFromCandNodeOuterJoinTable( qlncStringBlock * aStringBlock,
                                                    qlncNodeCommon  * aCandNode,
                                                    qllEnv          * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode->mNodeType == QLNC_NODE_TYPE_OUTER_JOIN_TABLE,
                        QLL_ERROR_STACK(aEnv) );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Semi Join Table Candidate Node로부터 Query 구문을 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCandNode       Candidate Node
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceQueryFromCandNodeSemiJoinTable( qlncStringBlock  * aStringBlock,
                                                   qlncNodeCommon   * aCandNode,
                                                   qllEnv           * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode->mNodeType == QLNC_NODE_TYPE_SEMI_JOIN_TABLE,
                        QLL_ERROR_STACK(aEnv) );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Anti Semi Join Table Candidate Node로부터 Query 구문을 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCandNode       Candidate Node
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceQueryFromCandNodeAntiSemiJoinTable( qlncStringBlock  * aStringBlock,
                                                       qlncNodeCommon   * aCandNode,
                                                       qllEnv           * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode->mNodeType == QLNC_NODE_TYPE_ANTI_SEMI_JOIN_TABLE,
                        QLL_ERROR_STACK(aEnv) );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlnc */
