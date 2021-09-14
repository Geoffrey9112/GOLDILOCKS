/*******************************************************************************
 * qlncTraceTreeFromCodePlan.c
 *
 * Copyright (c) 2013, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlncTraceTreeFromCodePlan.c 9658 2013-09-24 07:32:57Z bsyou $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file qlncTraceTreeFromCodePlan.c
 * @brief SQL Processor Layer Optimizer Trace Tree From Code Plan
 */

#include <qll.h>
#include <qlDef.h>



/**
 * @addtogroup qlnc
 * @{
 */


/**
 * @brief Trace Tree From Code Plan Function List
 */
const qlncTraceTreeFromCodePlanFuncPtr qlncTraceTreeFromCodePlanFuncList[ QLL_PLAN_NODE_TYPE_MAX ] =
{
    qlncTraceTreeFromCodePlan_SelectStmt,               /**< QLL_PLAN_NODE_STMT_SELECT_TYPE */
    qlncTraceTreeFromCodePlan_InsertStmt,               /**< QLL_PLAN_NODE_STMT_INSERT_TYPE */
    qlncTraceTreeFromCodePlan_UpdateStmt,               /**< QLL_PLAN_NODE_STMT_UPDATE_TYPE */
    qlncTraceTreeFromCodePlan_DeleteStmt,               /**< QLL_PLAN_NODE_STMT_DELETE_TYPE */
    NULL,                                               /**< QLL_PLAN_NODE_STMT_MERGE_TYPE */

    /* query */
    qlncTraceTreeFromCodePlan_QuerySpec,                /**< QLL_PLAN_NODE_QUERY_SPEC_TYPE */
    NULL,                                               /**< QLL_PLAN_NODE_INDEX_BUILD_TYPE */
    NULL,                                               /**< QLL_PLAN_NODE_FOR_UPDATE_TYPE */
    NULL,                                               /**< QLL_PLAN_NODE_MERGE_TYPE */
    NULL,                                               /**< QLL_PLAN_NODE_VIEW_TYPE */
    NULL,                                               /**< QLL_PLAN_NODE_VIEW_PUSHED_PRED_TYPE */
    NULL,                                               /**< QLL_PLAN_NODE_FILTER_TYPE */
    NULL,                                               /**< QLL_PLAN_NODE_INLIST_TYPE */

    /* set */
    qlncTraceTreeFromCodePlan_QuerySet,                 /**< QLL_PLAN_NODE_QUERY_SET_TYPE */
    qlncTraceTreeFromCodePlan_UnionAll,                 /**< QLL_PLAN_NODE_UNION_ALL_TYPE */
    qlncTraceTreeFromCodePlan_UnionDistinct,            /**< QLL_PLAN_NODE_UNION_DISTINCT_TYPE */
    qlncTraceTreeFromCodePlan_ExceptAll,                /**< QLL_PLAN_NODE_EXCEPT_ALL_TYPE */
    qlncTraceTreeFromCodePlan_ExceptDistinct,           /**< QLL_PLAN_NODE_EXCEPT_DISTINCT_TYPE */
    qlncTraceTreeFromCodePlan_IntersectAll,             /**< QLL_PLAN_NODE_INTERSECT_ALL_TYPE */
    qlncTraceTreeFromCodePlan_IntersectDistinct,        /**< QLL_PLAN_NODE_INTERSECT_DISTINCT_TYPE */

    /* sub query */
    qlncTraceTreeFromCodePlan_SubQuery,                 /**< QLL_PLAN_NODE_SUB_QUERY_TYPE */
    qlncTraceTreeFromCodePlan_SubQueryList,             /**< QLL_PLAN_NODE_SUB_QUERY_LIST_TYPE */
    qlncTraceTreeFromCodePlan_SubQueryFunc,             /**< QLL_PLAN_NODE_SUB_QUERY_FUNCTION_TYPE */
    qlncTraceTreeFromCodePlan_SubQueryFilter,           /**< QLL_PLAN_NODE_SUB_QUERY_FILTER_TYPE */

    /* or */
    qlncTraceTreeFromCodePlan_Concat,                   /**< QLL_PLAN_NODE_CONCAT_TYPE */

    /* join */
    qlncTraceTreeFromCodePlan_NestedLoops,              /**< QLL_PLAN_NODE_NESTED_LOOPS_TYPE */
    qlncTraceTreeFromCodePlan_MergeJoin,                /**< QLL_PLAN_NODE_MERGE_JOIN_TYPE */
    qlncTraceTreeFromCodePlan_HashJoin,                 /**< QLL_PLAN_NODE_HASH_JOIN_TYPE */
    NULL,                                               /**< QLL_PLAN_NODE_INDEX_JOIN_TYPE */
    
    /* order by, group by, windows and etc. */
    NULL,                                               /**< QLL_PLAN_NODE_CONNECT_BY_TYPE */
    NULL,                                               /**< QLL_PLAN_NODE_PSEUDO_COLUMN_TYPE */
    NULL,                                               /**< QLL_PLAN_NODE_COUNT_TYPE */
    NULL,                                               /**< QLL_PLAN_NODE_WINDOW_TYPE */
    NULL,                                               /**< QLL_PLAN_NODE_HASH_TYPE */
    qlncTraceTreeFromCodePlan_HashAggregation,          /**< QLL_PLAN_NODE_HASH_AGGREGATION_TYPE */
    qlncTraceTreeFromCodePlan_Group,                    /**< QLL_PLAN_NODE_GROUP_TYPE */

    /* scan */
    qlncTraceTreeFromCodePlan_TableAccess,              /**< QLL_PLAN_NODE_TABLE_ACCESS_TYPE */
    qlncTraceTreeFromCodePlan_IndexAccess,              /**< QLL_PLAN_NODE_INDEX_ACCESS_TYPE */
    qlncTraceTreeFromCodePlan_RowidAccess,              /**< QLL_PLAN_NODE_ROWID_ACCESS_TYPE */
    qlncTraceTreeFromCodePlan_FastDualAccess,           /**< QLL_PLAN_NODE_FAST_DUAL_ACCESS_TYPE */
    qlncTraceTreeFromCodePlan_FlatInstantAccess,        /**< QLL_PLAN_NODE_FLAT_INSTANT_ACCESS_TYPE */
    qlncTraceTreeFromCodePlan_SortInstantAccess,        /**< QLL_PLAN_NODE_SORT_INSTANT_ACCESS_TYPE */
    qlncTraceTreeFromCodePlan_HashInstantAccess,        /**< QLL_PLAN_NODE_HASH_INSTANT_ACCESS_TYPE */
    qlncTraceTreeFromCodePlan_GroupHashInstantAccess,   /**< QLL_PLAN_NODE_GROUP_HASH_INSTANT_ACCESS_TYPE */
    qlncTraceTreeFromCodePlan_GroupSortInstantAccess,   /**< QLL_PLAN_NODE_GROUP_SORT_INSTANT_ACCESS_TYPE */
    qlncTraceTreeFromCodePlan_SortJoinInstantAccess,    /**< QLL_PLAN_NODE_SORT_JOIN_INSTANT_ACCESS_TYPE */
    qlncTraceTreeFromCodePlan_HashJoinInstantAccess     /**< QLL_PLAN_NODE_HASH_JOIN_INSTANT_ACCESS_TYPE */
};


/**
 * @brief Select Statement Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_SelectStmt( qlncStringBlock     * aStringBlock,
                                                qllOptimizationNode * aCodePlan,
                                                stlInt32              aLevel,
                                                qllEnv              * aEnv )
{
    qlnoSelectStmt  * sOptSelectStmt    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_STMT_SELECT_TYPE, QLL_ERROR_STACK(aEnv) );


    /* Select Statement Code Plan 설정 */
    sOptSelectStmt = (qlnoSelectStmt*)(aCodePlan->mOptNode);

    /* Code Plan Name */
    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "%s[SELECT STMT] IDX: %d PTR: 0x%p\n",
                 qlncTraceTreePrefix[aLevel],
                 aCodePlan->mIdx,
                 sOptSelectStmt )
             == STL_SUCCESS );

    /* Into 절 */

    /* Offset */
    if( sOptSelectStmt->mResultSkip != NULL )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "%s<OFFSET>\n",
                                                     qlncTraceTreePrefix[aLevel+1] )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceTreeFromCodeExprFuncList[ sOptSelectStmt->mResultSkip->mType ](
                     aStringBlock,
                     sOptSelectStmt->mResultSkip,
                     aLevel+2,
                     aEnv )
                 == STL_SUCCESS );
    }

    /* Limit */
    if( sOptSelectStmt->mResultLimit != NULL )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "%s<LIMIT>\n",
                                                     qlncTraceTreePrefix[aLevel+1] )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceTreeFromCodeExprFuncList[ sOptSelectStmt->mResultLimit->mType ](
                     aStringBlock,
                     sOptSelectStmt->mResultLimit,
                     aLevel+2,
                     aEnv )
                 == STL_SUCCESS );
    }

    /* Child Node 수행 */
    STL_TRY( qlncTraceTreeFromCodePlanFuncList[ sOptSelectStmt->mQueryNode->mType ](
                 aStringBlock,
                 sOptSelectStmt->mQueryNode,
                 aLevel+1,
                 aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Insert Statement Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_InsertStmt( qlncStringBlock     * aStringBlock,
                                                qllOptimizationNode * aCodePlan,
                                                stlInt32              aLevel,
                                                qllEnv              * aEnv )
{
    stlInt32          i;
    stlInt32          j;
    qlnoInsertStmt  * sOptInsertStmt    = NULL;
    stlChar           sTmpStr[16];


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_STMT_INSERT_TYPE, QLL_ERROR_STACK(aEnv) );


    /* Insert Statement Code Plan 설정 */
    sOptInsertStmt = (qlnoInsertStmt*)(aCodePlan->mOptNode);

    /* Code Plan Name */
    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "%s[INSERT%s STMT] IDX: %d PTR: 0x%p\n",
                 qlncTraceTreePrefix[aLevel],
                 (sOptInsertStmt->mHasQuery == STL_TRUE
                  ? " SELECT" : ""),
                 aCodePlan->mIdx,
                 sOptInsertStmt )
             == STL_SUCCESS );

    /* Insert할 Column */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "INSERT COLUMN",
                                                    sOptInsertStmt->mInsertColExprList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Insert할 Row */
    for( i = 0; i < sOptInsertStmt->mInsertRowCount; i++ )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     "%s<ROWS-%3d>\n",
                     qlncTraceTreePrefix[aLevel+1],
                     i )
                 == STL_SUCCESS );

        /* Insert할 Column Value */
        for( j = 0; j < sOptInsertStmt->mInsertColumnCnt; j++ )
        {
            stlSnprintf( sTmpStr, 15, "COLUMN-%3d", j );
            STL_TRY( qlncTraceTreeFromCodePlan_Expr( aStringBlock,
                                                     sTmpStr,
                                                     sOptInsertStmt->mInsertValueExpr[i * sOptInsertStmt->mInsertColumnCnt + j],
                                                     aLevel + 2,
                                                     aEnv )
                     == STL_SUCCESS );
        }
    }

    /* Child Node 수행 */
    if( sOptInsertStmt->mQueryNode != NULL )
    {
        STL_TRY( qlncTraceTreeFromCodePlanFuncList[ sOptInsertStmt->mQueryNode->mType ](
                     aStringBlock,
                     sOptInsertStmt->mQueryNode,
                     aLevel+1,
                     aEnv )
                 == STL_SUCCESS );
    }

    /* Return Into 절 관련 */


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Update Statement Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_UpdateStmt( qlncStringBlock     * aStringBlock,
                                                qllOptimizationNode * aCodePlan,
                                                stlInt32              aLevel,
                                                qllEnv              * aEnv )
{
    stlInt32          i;
    qlnoUpdateStmt  * sOptUpdateStmt    = NULL;
    qlvRefExprItem  * sRefExprItem      = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_STMT_UPDATE_TYPE, QLL_ERROR_STACK(aEnv) );


    /* Update Statement Code Plan 설정 */
    sOptUpdateStmt = (qlnoUpdateStmt*)(aCodePlan->mOptNode);

    /* Code Plan Name */
    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "%s[UPDATE STMT] IDX: %d PTR: 0x%p\n",
                 qlncTraceTreePrefix[aLevel],
                 aCodePlan->mIdx,
                 sOptUpdateStmt )
             == STL_SUCCESS );

    /* Set 관련 */
    sRefExprItem = sOptUpdateStmt->mSetColExprList->mHead;
    for( i = 0; i < sOptUpdateStmt->mSetColumnCnt; i++ )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     "%s<SET-%3d>\n",
                     qlncTraceTreePrefix[aLevel+1],
                     i )
                 == STL_SUCCESS );

        /* Column */
        STL_TRY( qlncTraceTreeFromCodePlan_Expr( aStringBlock,
                                                 "COLUMN",
                                                 sRefExprItem->mExprPtr,
                                                 aLevel + 2,
                                                 aEnv )
                 == STL_SUCCESS );

        /* Value */
        STL_TRY( qlncTraceTreeFromCodePlan_Expr( aStringBlock,
                                                 "VALUE",
                                                 sOptUpdateStmt->mSetExprArray[i],
                                                 aLevel + 2,
                                                 aEnv )
                 == STL_SUCCESS );

        sRefExprItem = sRefExprItem->mNext;
    }

    /* Set Child Node 수행 */
    if( sOptUpdateStmt->mSetChildNode != NULL )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     "%s<SET CHILD NODE>\n",
                     qlncTraceTreePrefix[aLevel+1] )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceTreeFromCodePlanFuncList[ sOptUpdateStmt->mSetChildNode->mType ](
                     aStringBlock,
                     sOptUpdateStmt->mSetChildNode,
                     aLevel+2,
                     aEnv )
                 == STL_SUCCESS );
    }
    
    /* Scan Node 수행 */
    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "%s<SCAN NODE>\n",
                 qlncTraceTreePrefix[aLevel+1] )
             == STL_SUCCESS );

    STL_TRY( qlncTraceTreeFromCodePlanFuncList[ sOptUpdateStmt->mScanNode->mType ](
                 aStringBlock,
                 sOptUpdateStmt->mScanNode,
                 aLevel+2,
                 aEnv )
             == STL_SUCCESS );

    /* Return Into 절 관련 */


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Delete Statement Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_DeleteStmt( qlncStringBlock     * aStringBlock,
                                                qllOptimizationNode * aCodePlan,
                                                stlInt32              aLevel,
                                                qllEnv              * aEnv )
{
    qlnoDeleteStmt  * sOptDeleteStmt    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_STMT_DELETE_TYPE, QLL_ERROR_STACK(aEnv) );


    /* Delete Statement Code Plan 설정 */
    sOptDeleteStmt = (qlnoDeleteStmt*)(aCodePlan->mOptNode);

    /* Code Plan Name */
    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "%s[DELETE STMT] IDX: %d PTR: 0x%p\n",
                 qlncTraceTreePrefix[aLevel],
                 aCodePlan->mIdx,
                 sOptDeleteStmt )
             == STL_SUCCESS );

    /* Offset */
    if( sOptDeleteStmt->mResultSkip != NULL )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "%s<OFFSET>\n",
                                                     qlncTraceTreePrefix[aLevel+1] )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceTreeFromCodeExprFuncList[ sOptDeleteStmt->mResultSkip->mType ](
                     aStringBlock,
                     sOptDeleteStmt->mResultSkip,
                     aLevel+2,
                     aEnv )
                 == STL_SUCCESS );
    }

    /* Limit */
    if( sOptDeleteStmt->mResultLimit != NULL )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "%s<LIMIT>\n",
                                                     qlncTraceTreePrefix[aLevel+1] )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceTreeFromCodeExprFuncList[ sOptDeleteStmt->mResultLimit->mType ](
                     aStringBlock,
                     sOptDeleteStmt->mResultLimit,
                     aLevel+2,
                     aEnv )
                 == STL_SUCCESS );
    }

    /* Child Node 수행 */
    STL_TRY( qlncTraceTreeFromCodePlanFuncList[ sOptDeleteStmt->mScanNode->mType ](
                 aStringBlock,
                 sOptDeleteStmt->mScanNode,
                 aLevel+1,
                 aEnv )
             == STL_SUCCESS );

    /* Return Into 절 관련 */


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Merge Statement Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_MergeStmt( qlncStringBlock      * aStringBlock,
                                               qllOptimizationNode  * aCodePlan,
                                               stlInt32               aLevel,
                                               qllEnv               * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_STMT_MERGE_TYPE, QLL_ERROR_STACK(aEnv) );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Query Spec Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_QuerySpec( qlncStringBlock      * aStringBlock,
                                               qllOptimizationNode  * aCodePlan,
                                               stlInt32               aLevel,
                                               qllEnv               * aEnv )
{
    qlnoQuerySpec   * sOptQuerySpec = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_QUERY_SPEC_TYPE, QLL_ERROR_STACK(aEnv) );


    /* Query Spec Code Plan 설정 */
    sOptQuerySpec = (qlnoQuerySpec*)(aCodePlan->mOptNode);

    /* Code Plan Name */
    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "%s[QUERY SPEC] IDX: %d PTR: 0x%p\n",
                 qlncTraceTreePrefix[aLevel],
                 aCodePlan->mIdx,
                 sOptQuerySpec )
             == STL_SUCCESS );

    /* Target */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "TARGET",
                                                    sOptQuerySpec->mTargetColList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Filter */
    /* STL_TRY( qlncTraceTreeFromCodePlan_Expr( aStringBlock, */
    /*                                          "FILTER", */
    /*                                          sOptQuerySpec->mWhereExpr, */
    /*                                          aLevel + 1, */
    /*                                          aEnv ) */
    /*          == STL_SUCCESS ); */

    /* Offset */
    STL_TRY( qlncTraceTreeFromCodePlan_Expr( aStringBlock,
                                             "OFFSET",
                                             sOptQuerySpec->mResultSkip,
                                             aLevel + 1,
                                             aEnv )
             == STL_SUCCESS );

    /* Limit */
    STL_TRY( qlncTraceTreeFromCodePlan_Expr( aStringBlock,
                                             "LIMIT",
                                             sOptQuerySpec->mResultLimit,
                                             aLevel + 1,
                                             aEnv )
             == STL_SUCCESS );

    /* Child Node 수행 */
    STL_TRY( qlncTraceTreeFromCodePlanFuncList[ sOptQuerySpec->mChildNode->mType ](
                 aStringBlock,
                 sOptQuerySpec->mChildNode,
                 aLevel+1,
                 aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Index Build Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_IndexBuild( qlncStringBlock     * aStringBlock,
                                                qllOptimizationNode * aCodePlan,
                                                stlInt32              aLevel,
                                                qllEnv              * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_INDEX_BUILD_TYPE, QLL_ERROR_STACK(aEnv) );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief For Update Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_ForUpdate( qlncStringBlock      * aStringBlock,
                                               qllOptimizationNode  * aCodePlan,
                                               stlInt32               aLevel,
                                               qllEnv               * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_FOR_UPDATE_TYPE, QLL_ERROR_STACK(aEnv) );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Merge Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_Merge( qlncStringBlock      * aStringBlock,
                                           qllOptimizationNode  * aCodePlan,
                                           stlInt32               aLevel,
                                           qllEnv               * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_MERGE_TYPE, QLL_ERROR_STACK(aEnv) );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief View Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_View( qlncStringBlock       * aStringBlock,
                                          qllOptimizationNode   * aCodePlan,
                                          stlInt32                aLevel,
                                          qllEnv                * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_VIEW_TYPE, QLL_ERROR_STACK(aEnv) );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief View Push Pred Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_ViewPushedPred( qlncStringBlock     * aStringBlock,
                                                    qllOptimizationNode * aCodePlan,
                                                    stlInt32              aLevel,
                                                    qllEnv              * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_VIEW_PUSHED_PRED_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Filter Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_Filter( qlncStringBlock     * aStringBlock,
                                            qllOptimizationNode * aCodePlan,
                                            stlInt32              aLevel,
                                            qllEnv              * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_FILTER_TYPE, QLL_ERROR_STACK(aEnv) );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief InList Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_InList( qlncStringBlock     * aStringBlock,
                                            qllOptimizationNode * aCodePlan,
                                            stlInt32              aLevel,
                                            qllEnv              * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_INLIST_TYPE, QLL_ERROR_STACK(aEnv) );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Query Set Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_QuerySet( qlncStringBlock       * aStringBlock,
                                              qllOptimizationNode   * aCodePlan,
                                              stlInt32                aLevel,
                                              qllEnv                * aEnv )
{
    qlnoQuerySet    * sOptQuerySet  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_QUERY_SET_TYPE, QLL_ERROR_STACK(aEnv) );


    /* Query Set Code Plan 설정 */
    sOptQuerySet = (qlnoQuerySet*)(aCodePlan->mOptNode);

    /* Code Plan Name */
    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "%s[QUERY SET] IDX: %d ROOT: %c OPTPTR: 0x%p\n",
                 qlncTraceTreePrefix[aLevel],
                 aCodePlan->mIdx,
                 (sOptQuerySet->mIsRootSet == STL_TRUE
                  ? 'Y' : 'N'),
                 sOptQuerySet )
             == STL_SUCCESS );

    /* Target */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "TARGET",
                                                    sOptQuerySet->mTargetList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Offset */
    STL_TRY( qlncTraceTreeFromCodePlan_Expr( aStringBlock,
                                             "OFFSET",
                                             sOptQuerySet->mResultSkip,
                                             aLevel + 1,
                                             aEnv )
             == STL_SUCCESS );

    /* Limit */
    STL_TRY( qlncTraceTreeFromCodePlan_Expr( aStringBlock,
                                             "LIMIT",
                                             sOptQuerySet->mResultLimit,
                                             aLevel + 1,
                                             aEnv )
             == STL_SUCCESS );

    /* Child Node 수행 */
    STL_TRY( qlncTraceTreeFromCodePlanFuncList[ sOptQuerySet->mChildNode->mType ](
                 aStringBlock,
                 sOptQuerySet->mChildNode,
                 aLevel+1,
                 aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Union All Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_UnionAll( qlncStringBlock       * aStringBlock,
                                              qllOptimizationNode   * aCodePlan,
                                              stlInt32                aLevel,
                                              qllEnv                * aEnv )
{
    stlInt32      i;
    qlnoSetOP   * sOptSetOP = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_UNION_ALL_TYPE, QLL_ERROR_STACK(aEnv) );


    /* Set OP Code Plan 설정 */
    sOptSetOP = (qlnoSetOP*)(aCodePlan->mOptNode);

    /* Code Plan Name */
    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "%s[SET OP] IDX: %d TYP: UNION ALL ROOTIDX: %d CHDCNT: %d OPTPTR: 0x%p ROOTPTR: 0x%p\n",
                 qlncTraceTreePrefix[aLevel],
                 aCodePlan->mIdx,
                 sOptSetOP->mRootQuerySetIdx,
                 sOptSetOP->mChildCount,
                 sOptSetOP,
                 sOptSetOP->mRootQuerySet )
             == STL_SUCCESS );

    /* Child Node 수행 */
    for( i = 0; i < sOptSetOP->mChildCount; i++ )
    {
        STL_TRY( qlncTraceTreeFromCodePlanFuncList[ sOptSetOP->mChildNodeArray[i]->mType ](
                     aStringBlock,
                     sOptSetOP->mChildNodeArray[i],
                     aLevel+1,
                     aEnv )
                 == STL_SUCCESS );
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Union Distinct Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_UnionDistinct( qlncStringBlock      * aStringBlock,
                                                   qllOptimizationNode  * aCodePlan,
                                                   stlInt32               aLevel,
                                                   qllEnv               * aEnv )
{
    stlInt32      i;
    qlnoSetOP   * sOptSetOP = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_UNION_DISTINCT_TYPE, QLL_ERROR_STACK(aEnv) );


    /* Set OP Code Plan 설정 */
    sOptSetOP = (qlnoSetOP*)(aCodePlan->mOptNode);

    /* Code Plan Name */
    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "%s[SET OP] IDX: %d TYP: UNION DISTINCT ROOTIDX: %d CHDCNT: %d OPTPTR: 0x%p ROOTPTR: 0x%p\n",
                 qlncTraceTreePrefix[aLevel],
                 aCodePlan->mIdx,
                 sOptSetOP->mRootQuerySetIdx,
                 sOptSetOP->mChildCount,
                 sOptSetOP,
                 sOptSetOP->mRootQuerySet )
             == STL_SUCCESS );

    /* Child Node 수행 */
    for( i = 0; i < sOptSetOP->mChildCount; i++ )
    {
        STL_TRY( qlncTraceTreeFromCodePlanFuncList[ sOptSetOP->mChildNodeArray[i]->mType ](
                     aStringBlock,
                     sOptSetOP->mChildNodeArray[i],
                     aLevel+1,
                     aEnv )
                 == STL_SUCCESS );
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Except All Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_ExceptAll( qlncStringBlock      * aStringBlock,
                                               qllOptimizationNode  * aCodePlan,
                                               stlInt32               aLevel,
                                               qllEnv               * aEnv )
{
    stlInt32      i;
    qlnoSetOP   * sOptSetOP = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_EXCEPT_ALL_TYPE, QLL_ERROR_STACK(aEnv) );


    /* Set OP Code Plan 설정 */
    sOptSetOP = (qlnoSetOP*)(aCodePlan->mOptNode);

    /* Code Plan Name */
    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "%s[SET OP] IDX: %d TYP: EXCEPT ALL ROOTIDX: %d CHDCNT: %d OPTPTR: 0x%p ROOTPTR: 0x%p\n",
                 qlncTraceTreePrefix[aLevel],
                 aCodePlan->mIdx,
                 sOptSetOP->mRootQuerySetIdx,
                 sOptSetOP->mChildCount,
                 sOptSetOP,
                 sOptSetOP->mRootQuerySet )
             == STL_SUCCESS );

    /* Child Node 수행 */
    for( i = 0; i < sOptSetOP->mChildCount; i++ )
    {
        STL_TRY( qlncTraceTreeFromCodePlanFuncList[ sOptSetOP->mChildNodeArray[i]->mType ](
                     aStringBlock,
                     sOptSetOP->mChildNodeArray[i],
                     aLevel+1,
                     aEnv )
                 == STL_SUCCESS );
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Except Distinct Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_ExceptDistinct( qlncStringBlock     * aStringBlock,
                                                    qllOptimizationNode * aCodePlan,
                                                    stlInt32              aLevel,
                                                    qllEnv              * aEnv )
{
    stlInt32      i;
    qlnoSetOP   * sOptSetOP = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_EXCEPT_DISTINCT_TYPE, QLL_ERROR_STACK(aEnv) );


    /* Set OP Code Plan 설정 */
    sOptSetOP = (qlnoSetOP*)(aCodePlan->mOptNode);

    /* Code Plan Name */
    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "%s[SET OP] IDX: %d TYP: EXCEPT DISTINCT ROOTIDX: %d CHDCNT: %d OPTPTR: 0x%p ROOTPTR: 0x%p\n",
                 qlncTraceTreePrefix[aLevel],
                 aCodePlan->mIdx,
                 sOptSetOP->mRootQuerySetIdx,
                 sOptSetOP->mChildCount,
                 sOptSetOP,
                 sOptSetOP->mRootQuerySet )
             == STL_SUCCESS );

    /* Child Node 수행 */
    for( i = 0; i < sOptSetOP->mChildCount; i++ )
    {
        STL_TRY( qlncTraceTreeFromCodePlanFuncList[ sOptSetOP->mChildNodeArray[i]->mType ](
                     aStringBlock,
                     sOptSetOP->mChildNodeArray[i],
                     aLevel+1,
                     aEnv )
                 == STL_SUCCESS );
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Intersect All Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_IntersectAll( qlncStringBlock       * aStringBlock,
                                                  qllOptimizationNode   * aCodePlan,
                                                  stlInt32                aLevel,
                                                  qllEnv                * aEnv )
{
    stlInt32      i;
    qlnoSetOP   * sOptSetOP = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_INTERSECT_ALL_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    /* Set OP Code Plan 설정 */
    sOptSetOP = (qlnoSetOP*)(aCodePlan->mOptNode);

    /* Code Plan Name */
    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "%s[SET OP] IDX: %d TYP: INTERSECT ALL ROOTIDX: %d CHDCNT: %d OPTPTR: 0x%p ROOTPTR: 0x%p\n",
                 qlncTraceTreePrefix[aLevel],
                 aCodePlan->mIdx,
                 sOptSetOP->mRootQuerySetIdx,
                 sOptSetOP->mChildCount,
                 sOptSetOP,
                 sOptSetOP->mRootQuerySet )
             == STL_SUCCESS );

    /* Child Node 수행 */
    for( i = 0; i < sOptSetOP->mChildCount; i++ )
    {
        STL_TRY( qlncTraceTreeFromCodePlanFuncList[ sOptSetOP->mChildNodeArray[i]->mType ](
                     aStringBlock,
                     sOptSetOP->mChildNodeArray[i],
                     aLevel+1,
                     aEnv )
                 == STL_SUCCESS );
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Intersect Distinct Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_IntersectDistinct( qlncStringBlock      * aStringBlock,
                                                       qllOptimizationNode  * aCodePlan,
                                                       stlInt32               aLevel,
                                                       qllEnv               * aEnv )
{
    stlInt32      i;
    qlnoSetOP   * sOptSetOP = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_INTERSECT_DISTINCT_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    /* Set OP Code Plan 설정 */
    sOptSetOP = (qlnoSetOP*)(aCodePlan->mOptNode);

    /* Code Plan Name */
    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "%s[SET OP] IDX: %d TYP: INTERSECT DISTINCT ROOTIDX: %d CHDCNT: %d OPTPTR: 0x%p ROOTPTR: 0x%p\n",
                 qlncTraceTreePrefix[aLevel],
                 aCodePlan->mIdx,
                 sOptSetOP->mRootQuerySetIdx,
                 sOptSetOP->mChildCount,
                 sOptSetOP,
                 sOptSetOP->mRootQuerySet )
             == STL_SUCCESS );

    /* Child Node 수행 */
    for( i = 0; i < sOptSetOP->mChildCount; i++ )
    {
        STL_TRY( qlncTraceTreeFromCodePlanFuncList[ sOptSetOP->mChildNodeArray[i]->mType ](
                     aStringBlock,
                     sOptSetOP->mChildNodeArray[i],
                     aLevel+1,
                     aEnv )
                 == STL_SUCCESS );
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Sub Query Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_SubQuery( qlncStringBlock       * aStringBlock,
                                              qllOptimizationNode   * aCodePlan,
                                              stlInt32                aLevel,
                                              qllEnv                * aEnv )
{
    qlnoSubQuery    * sOptSubQuery  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_SUB_QUERY_TYPE, QLL_ERROR_STACK(aEnv) );


    /* SubQuery Code Plan 설정 */
    sOptSubQuery = (qlnoSubQuery*)(aCodePlan->mOptNode);

    /* Code Plan Name */
    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "%s[SUB QUERY] IDX: %d OPTPTR: 0x%p INITPTR: 0x%p\n",
                 qlncTraceTreePrefix[aLevel],
                 aCodePlan->mIdx,
                 sOptSubQuery,
                 sOptSubQuery->mInitNode )
             == STL_SUCCESS );

    /* Target */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "TARGET",
                                                    sOptSubQuery->mTargetList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Outer Column */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "OUTER COLUMN",
                                                    sOptSubQuery->mOuterColumnList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Filter */
    STL_TRY( qlncTraceTreeFromCodePlan_Expr( aStringBlock,
                                             "FILTER",
                                             sOptSubQuery->mFilterExpr,
                                             aLevel + 1,
                                             aEnv )
             == STL_SUCCESS );

    /* Child Node 수행 */
    STL_TRY( qlncTraceTreeFromCodePlanFuncList[ sOptSubQuery->mChildNode->mType ](
                 aStringBlock,
                 sOptSubQuery->mChildNode,
                 aLevel+1,
                 aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Sub Query List Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_SubQueryList( qlncStringBlock       * aStringBlock,
                                                  qllOptimizationNode   * aCodePlan,
                                                  stlInt32                aLevel,
                                                  qllEnv                * aEnv )
{
    stlInt32              i;
    qlnoSubQueryList    * sOptSubQueryList  = NULL;
    qlvRefExprItem      * sRefExprItem      = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_SUB_QUERY_LIST_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    /* SubQuery List Code Plan 설정 */
    sOptSubQueryList = (qlnoSubQueryList*)(aCodePlan->mOptNode);

    /* Code Plan Name */
    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "%s[SUB QUERY LIST] IDX: %d SUBCNT: %d OPTPTR: 0x%p\n",
                 qlncTraceTreePrefix[aLevel],
                 aCodePlan->mIdx,
                 sOptSubQueryList->mSubQueryNodeCnt,
                 sOptSubQueryList )
             == STL_SUCCESS );

    /* SubQuery Expr List */
    if( sOptSubQueryList->mSubQueryExprList != NULL )
    {
        i = 0;
        sRefExprItem = sOptSubQueryList->mSubQueryExprList->mHead;
        while( sRefExprItem != NULL )
        {
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "%s<SUB QUERY-%03d>\n",
                                                         qlncTraceTreePrefix[aLevel+1],
                                                         i )
                     == STL_SUCCESS );

            STL_TRY( qlncTraceTreeFromCodeExprFuncList[ sRefExprItem->mExprPtr->mType ](
                         aStringBlock,
                         sRefExprItem->mExprPtr,
                         aLevel+2,
                         aEnv )
                     == STL_SUCCESS );

            /* SubQuery Node 수행 */
            STL_TRY( qlncTraceTreeFromCodePlanFuncList[ sOptSubQueryList->mSubQueryNodes[i]->mType ](
                         aStringBlock,
                         sOptSubQueryList->mSubQueryNodes[i],
                         aLevel+3,
                         aEnv )
                     == STL_SUCCESS );

            i++;
            sRefExprItem = sRefExprItem->mNext;
        }
    }

    /* Outer Column */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "OUTER COLUMN",
                                                    sOptSubQueryList->mOuterColumnList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Child Node 수행 */
    if( sOptSubQueryList->mChildNode != NULL )
    {
        STL_TRY( qlncTraceTreeFromCodePlanFuncList[ sOptSubQueryList->mChildNode->mType ](
                     aStringBlock,
                     sOptSubQueryList->mChildNode,
                     aLevel+1,
                     aEnv )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Sub Query Function Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_SubQueryFunc( qlncStringBlock       * aStringBlock,
                                                  qllOptimizationNode   * aCodePlan,
                                                  stlInt32                aLevel,
                                                  qllEnv                * aEnv )
{
    qlnoSubQueryFunc    * sOptSubQueryFunc  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_SUB_QUERY_FUNCTION_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    /* SubQuery Func Code Plan 설정 */
    sOptSubQueryFunc = (qlnoSubQueryFunc*)(aCodePlan->mOptNode);

    /* Code Plan Name */
    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "%s[SUB QUERY FUNCTION] IDX: %d OPTPTR: 0x%p\n",
                 qlncTraceTreePrefix[aLevel],
                 aCodePlan->mIdx,
                 sOptSubQueryFunc )
             == STL_SUCCESS );

    /* Sub Query Function */
    STL_TRY( qlncTraceTreeFromCodePlan_Expr( aStringBlock,
                                             "FUNCTION",
                                             sOptSubQueryFunc->mFuncExpr,
                                             aLevel + 1,
                                             aEnv )
             == STL_SUCCESS );

    /* Right Operand Node 수행 */
    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "%s<SUB QUERY>\n",
                 qlncTraceTreePrefix[aLevel+1] )
             == STL_SUCCESS );


    STL_TRY( qlncTraceTreeFromCodePlanFuncList[ sOptSubQueryFunc->mRightOperandNode->mType ](
                 aStringBlock,
                 sOptSubQueryFunc->mRightOperandNode,
                 aLevel+2,
                 aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Sub Query Filter Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_SubQueryFilter( qlncStringBlock     * aStringBlock,
                                                    qllOptimizationNode * aCodePlan,
                                                    stlInt32              aLevel,
                                                    qllEnv              * aEnv )
{
    qlnoSubQueryFilter  * sOptSubQueryFilter    = NULL;
    stlInt32              sLoop;
    stlChar               sTmpStr[16];


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_SUB_QUERY_FILTER_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    /* SubQuery Filter Code Plan 설정 */
    sOptSubQueryFilter = (qlnoSubQueryFilter*)(aCodePlan->mOptNode);

    /* Code Plan Name */
    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "%s[SUB QUERY FILTER] IDX: %d OPTPTR: 0x%p\n",
                 qlncTraceTreePrefix[aLevel],
                 aCodePlan->mIdx,
                 sOptSubQueryFilter )
             == STL_SUCCESS );

    /* Filter */
    for( sLoop = 0 ; sLoop < sOptSubQueryFilter->mAndFilterCnt ; sLoop++ )
    {
        stlSnprintf( sTmpStr, 15, "FILTER-%3d", sLoop );
        STL_TRY( qlncTraceTreeFromCodePlan_Expr( aStringBlock,
                                                 sTmpStr,
                                                 sOptSubQueryFilter->mFilterExprArr[ sLoop ],
                                                 aLevel + 1,
                                                 aEnv )
                 == STL_SUCCESS );
    }

    /* Child Node 수행 */
    STL_TRY( qlncTraceTreeFromCodePlanFuncList[ sOptSubQueryFilter->mChildNode->mType ](
                 aStringBlock,
                 sOptSubQueryFilter->mChildNode,
                 aLevel+1,
                 aEnv )
             == STL_SUCCESS );

    /* SubQuery List */
    for( sLoop = 0; sLoop < sOptSubQueryFilter->mAndFilterCnt; sLoop++ )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     "%s<SUB QUERY LIST-%3d>\n",
                     qlncTraceTreePrefix[aLevel+1],
                     sLoop )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceTreeFromCodePlanFuncList[ sOptSubQueryFilter->mAndFilterNodeArr[sLoop]->mType ](
                     aStringBlock,
                     sOptSubQueryFilter->mAndFilterNodeArr[sLoop],
                     aLevel+2,
                     aEnv )
                 == STL_SUCCESS );
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Concat Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_Concat( qlncStringBlock     * aStringBlock,
                                            qllOptimizationNode * aCodePlan,
                                            stlInt32              aLevel,
                                            qllEnv              * aEnv )
{
    stlInt32          i;
    qlnoConcat      * sOptConcat    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_CONCAT_TYPE, QLL_ERROR_STACK(aEnv) );


    /* Concat Code Plan 설정 */
    sOptConcat = (qlnoConcat*)(aCodePlan->mOptNode);

    /* Code Plan Name */
    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "%s[CONCAT] IDX: %d CHDCNT: %d TBLNAM: %s TBLHND: 0x%p OPTPTR: 0x%p\n",
                 qlncTraceTreePrefix[aLevel],
                 aCodePlan->mIdx,
                 sOptConcat->mChildCount,
                 ( sOptConcat->mTableHandle == NULL
                   ? "NULL"
                   : ellGetTableName( sOptConcat->mTableHandle ) ),
                 sOptConcat->mTableHandle,
                 sOptConcat )
             == STL_SUCCESS );

    /* Concat Column List */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "CONCAT COL",
                                                    sOptConcat->mConcatColList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Outer Column */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "OUTER COLUMN",
                                                    sOptConcat->mOuterColumnList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Child Node 수행 */
    for( i = 0; i < sOptConcat->mChildCount; i++ )
    {
        STL_TRY( qlncTraceTreeFromCodePlanFuncList[ sOptConcat->mChildNodeArr[i]->mType ](
                     aStringBlock,
                     sOptConcat->mChildNodeArr[i],
                     aLevel+1,
                     aEnv )
                 == STL_SUCCESS );
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Nested Loops Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_NestedLoops( qlncStringBlock        * aStringBlock,
                                                 qllOptimizationNode    * aCodePlan,
                                                 stlInt32                 aLevel,
                                                 qllEnv                 * aEnv )
{
    qlnoNestedLoopsJoin * sOptNestedJoin    = NULL;
    stlChar               sTmpStr[32];
    stlInt32              i;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_NESTED_LOOPS_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    /* Nested Loops Join Code Plan 설정 */
    sOptNestedJoin = (qlnoNestedLoopsJoin*)(aCodePlan->mOptNode);

    /* Code Plan Name */
    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "%s[NESTED JOIN] IDX: %d JOINTYP: %s OPTPTR: 0x%p INITPTR: 0x%p\n",
                 qlncTraceTreePrefix[aLevel],
                 aCodePlan->mIdx,
                 qlncJoinTypeName[sOptNestedJoin->mJoinType],
                 sOptNestedJoin,
                 sOptNestedJoin->mJoinTableNode )
             == STL_SUCCESS );

    /* Join Column List */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "JOIN COL",
                                                    sOptNestedJoin->mJoinColList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Left Outer Column */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "JOIN LEFT OUTER COLUMN",
                                                    sOptNestedJoin->mLeftOuterColumnList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Right Outer Column */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "JOIN RIGHT OUTER COLUMN",
                                                    sOptNestedJoin->mRightOuterColumnList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Outer Column */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "OUTER COLUMN",
                                                    sOptNestedJoin->mOuterColumnList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Join Filter */
    STL_TRY( qlncTraceTreeFromCodePlan_Expr( aStringBlock,
                                             "JOIN FILTER",
                                             sOptNestedJoin->mJoinFilterExpr,
                                             aLevel + 1,
                                             aEnv )
             == STL_SUCCESS );

    /* SubQuery Filter */
    for( i = 0; i < sOptNestedJoin->mSubQueryAndFilterCnt; i++ )
    {
        stlSnprintf( sTmpStr, 32, "SUBQUERY FILTER %03d", i );
        STL_TRY( qlncTraceTreeFromCodePlan_Expr( aStringBlock,
                                                 sTmpStr,
                                                 sOptNestedJoin->mSubQueryFilterExprArr[i],
                                                 aLevel + 1,
                                                 aEnv )
                 == STL_SUCCESS );
    }

    /* Where Filter */
    STL_TRY( qlncTraceTreeFromCodePlan_Expr( aStringBlock,
                                             "WHERE FILTER",
                                             sOptNestedJoin->mTotalFilterExpr,
                                             aLevel + 1,
                                             aEnv )
             == STL_SUCCESS );

    /* Left Child Node 수행 */
    STL_TRY( qlncTraceTreeFromCodePlanFuncList[ sOptNestedJoin->mLeftChildNode->mType ](
                 aStringBlock,
                 sOptNestedJoin->mLeftChildNode,
                 aLevel+1,
                 aEnv )
             == STL_SUCCESS );

    /* Right Child Node 수행 */
    STL_TRY( qlncTraceTreeFromCodePlanFuncList[ sOptNestedJoin->mRightChildNode->mType ](
                 aStringBlock,
                 sOptNestedJoin->mRightChildNode,
                 aLevel+1,
                 aEnv )
             == STL_SUCCESS );

    /* Statement Level단위 처리의 SubQuery Node 수행 */
    if( sOptNestedJoin->mStmtSubQueryNode != NULL )
    {
        STL_TRY( qlncTraceTreeFromCodePlanFuncList[ sOptNestedJoin->mStmtSubQueryNode->mType ](
                     aStringBlock,
                     sOptNestedJoin->mStmtSubQueryNode,
                     aLevel+1,
                     aEnv )
                 == STL_SUCCESS );
    }

    /* SubQuery Filter에 대한 Node Array 수행 */
    for( i = 0; i < sOptNestedJoin->mSubQueryAndFilterCnt; i++ )
    {
        STL_TRY( qlncTraceTreeFromCodePlanFuncList[ sOptNestedJoin->mSubQueryAndFilterNodeArr[i]->mType ](
                     aStringBlock,
                     sOptNestedJoin->mSubQueryAndFilterNodeArr[i],
                     aLevel+1,
                     aEnv )
                 == STL_SUCCESS );
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Merge Join Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_MergeJoin( qlncStringBlock      * aStringBlock,
                                               qllOptimizationNode  * aCodePlan,
                                               stlInt32               aLevel,
                                               qllEnv               * aEnv )
{
    qlnoSortMergeJoin   * sOptMergeJoin = NULL;

    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_MERGE_JOIN_TYPE, QLL_ERROR_STACK(aEnv) );


    /* Sort Merge Join Code Plan 설정 */
    sOptMergeJoin = (qlnoSortMergeJoin*)(aCodePlan->mOptNode);

    /* Code Plan Name */
    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "%s[MERGE JOIN] IDX: %d JOINTYP: %s OPTPTR: 0x%p INITPTR: 0x%p\n",
                 qlncTraceTreePrefix[aLevel],
                 aCodePlan->mIdx,
                 qlncJoinTypeName[sOptMergeJoin->mJoinType],
                 sOptMergeJoin,
                 sOptMergeJoin->mJoinTableNode )
             == STL_SUCCESS );

    /* Join Column List */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "JOIN COL",
                                                    sOptMergeJoin->mJoinColList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Left Sort Key List */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "LEFT SORT KEY",
                                                    sOptMergeJoin->mLeftSortKeyList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Right Sort Key List */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "RIGHT SORT KEY",
                                                    sOptMergeJoin->mRightSortKeyList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Left Outer Column */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "JOIN LEFT OUTER COLUMN",
                                                    sOptMergeJoin->mLeftOuterColumnList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Right Outer Column */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "JOIN RIGHT OUTER COLUMN",
                                                    sOptMergeJoin->mRightOuterColumnList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Outer Column */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "OUTER COLUMN",
                                                    sOptMergeJoin->mOuterColumnList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Join Filter */
    STL_TRY( qlncTraceTreeFromCodePlan_Expr( aStringBlock,
                                             "JOIN FILTER",
                                             sOptMergeJoin->mJoinFilterExpr,
                                             aLevel + 1,
                                             aEnv )
             == STL_SUCCESS );

    /* Where Filter */
    STL_TRY( qlncTraceTreeFromCodePlan_Expr( aStringBlock,
                                             "WHERE FILTER",
                                             sOptMergeJoin->mWhereFilterExpr,
                                             aLevel + 1,
                                             aEnv )
             == STL_SUCCESS );

    /* Left Child Node 수행 */
    STL_TRY( qlncTraceTreeFromCodePlanFuncList[ sOptMergeJoin->mLeftChildNode->mType ](
                 aStringBlock,
                 sOptMergeJoin->mLeftChildNode,
                 aLevel+1,
                 aEnv )
             == STL_SUCCESS );

    /* Right Child Node 수행 */
    STL_TRY( qlncTraceTreeFromCodePlanFuncList[ sOptMergeJoin->mRightChildNode->mType ](
                 aStringBlock,
                 sOptMergeJoin->mRightChildNode,
                 aLevel+1,
                 aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Hash Join Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_HashJoin( qlncStringBlock       * aStringBlock,
                                              qllOptimizationNode   * aCodePlan,
                                              stlInt32                aLevel,
                                              qllEnv                * aEnv )
{
    qlnoHashJoin    * sOptHashJoin  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_HASH_JOIN_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    /* Hash Join Code Plan 설정 */
    sOptHashJoin = (qlnoHashJoin*)(aCodePlan->mOptNode);

    /* Code Plan Name */
    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "%s[HASH JOIN] IDX: %d JOINTYP: %s OPTPTR: 0x%p INITPTR: 0x%p\n",
                 qlncTraceTreePrefix[aLevel],
                 aCodePlan->mIdx,
                 qlncJoinTypeName[sOptHashJoin->mJoinType],
                 sOptHashJoin,
                 sOptHashJoin->mJoinTableNode )
             == STL_SUCCESS );

    /* Join Column List */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "JOIN COL",
                                                    sOptHashJoin->mJoinColList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Left Outer Column */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "JOIN LEFT OUTER COLUMN",
                                                    sOptHashJoin->mLeftOuterColumnList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Right Outer Column */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "JOIN RIGHT OUTER COLUMN",
                                                    sOptHashJoin->mRightOuterColumnList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Outer Column */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "OUTER COLUMN",
                                                    sOptHashJoin->mOuterColumnList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Join Filter */
    STL_TRY( qlncTraceTreeFromCodePlan_Expr( aStringBlock,
                                             "JOIN FILTER",
                                             sOptHashJoin->mJoinFilterExpr,
                                             aLevel + 1,
                                             aEnv )
             == STL_SUCCESS );

    /* Where Filter */
    STL_TRY( qlncTraceTreeFromCodePlan_Expr( aStringBlock,
                                             "WHERE FILTER",
                                             sOptHashJoin->mWhereFilterExpr,
                                             aLevel + 1,
                                             aEnv )
             == STL_SUCCESS );

    /* Left Child Node 수행 */
    STL_TRY( qlncTraceTreeFromCodePlanFuncList[ sOptHashJoin->mLeftChildNode->mType ](
                 aStringBlock,
                 sOptHashJoin->mLeftChildNode,
                 aLevel+1,
                 aEnv )
             == STL_SUCCESS );

    /* Right Child Node 수행 */
    STL_TRY( qlncTraceTreeFromCodePlanFuncList[ sOptHashJoin->mRightChildNode->mType ](
                 aStringBlock,
                 sOptHashJoin->mRightChildNode,
                 aLevel+1,
                 aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Index Join Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_IndexJoin( qlncStringBlock      * aStringBlock,
                                               qllOptimizationNode  * aCodePlan,
                                               stlInt32               aLevel,
                                               qllEnv               * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_INDEX_JOIN_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Connect By Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_ConnectBy( qlncStringBlock      * aStringBlock,
                                               qllOptimizationNode  * aCodePlan,
                                               stlInt32               aLevel,
                                               qllEnv               * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_CONNECT_BY_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Pseudo Column Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_PseudoColumn( qlncStringBlock       * aStringBlock,
                                                  qllOptimizationNode   * aCodePlan,
                                                  stlInt32                aLevel,
                                                  qllEnv                * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_PSEUDO_COLUMN_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Count Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_Count( qlncStringBlock      * aStringBlock,
                                           qllOptimizationNode  * aCodePlan,
                                           stlInt32               aLevel,
                                           qllEnv               * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_COUNT_TYPE, QLL_ERROR_STACK(aEnv) );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Window Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_Window( qlncStringBlock     * aStringBlock,
                                            qllOptimizationNode * aCodePlan,
                                            stlInt32              aLevel,
                                            qllEnv              * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_WINDOW_TYPE, QLL_ERROR_STACK(aEnv) );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Hash Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_Hash( qlncStringBlock       * aStringBlock,
                                          qllOptimizationNode   * aCodePlan,
                                          stlInt32                aLevel,
                                          qllEnv                * aEnv )
{
    qlnoHash    * sOptHash  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_HASH_TYPE, QLL_ERROR_STACK(aEnv) );


    /* Hash Code Plan 설정 */
    sOptHash = (qlnoHash*)(aCodePlan->mOptNode);

    /* Code Plan Name */
    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "%s[HASH] IDX: %d OPTPTR: 0x%p\n",
                 qlncTraceTreePrefix[aLevel],
                 aCodePlan->mIdx,
                 sOptHash )
             == STL_SUCCESS );

    /* Key Column */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "KEY COL",
                                                    sOptHash->mKeyColExprList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Value Column */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "VAL COL",
                                                    sOptHash->mValueColExprList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Child Node 수행 */
    STL_TRY( qlncTraceTreeFromCodePlanFuncList[ sOptHash->mChildNode->mType ](
                 aStringBlock,
                 sOptHash->mChildNode,
                 aLevel+1,
                 aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Hash Aggregation Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_HashAggregation( qlncStringBlock        * aStringBlock,
                                                     qllOptimizationNode    * aCodePlan,
                                                     stlInt32                 aLevel,
                                                     qllEnv                 * aEnv )
{
    qlnoAggregation     * sOptAggregation   = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_HASH_AGGREGATION_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    /* Aggregation Code Plan 설정 */
    sOptAggregation = (qlnoAggregation*)(aCodePlan->mOptNode);

    /* Code Plan Name */
    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "%s[AGGREGATION] IDX: %d AGGRCNT: %d AGGRDISTCNT: %d OPTPTR: 0x%p\n",
                 qlncTraceTreePrefix[aLevel],
                 aCodePlan->mIdx,
                 sOptAggregation->mAggrFuncCnt,
                 sOptAggregation->mAggrDistFuncCnt,
                 sOptAggregation )
             == STL_SUCCESS );

    /* Aggregation Function */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "AGGR FUNC",
                                                    sOptAggregation->mAggrFuncList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Aggregation Distinct Function */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "AGGR DIST FUNC",
                                                    sOptAggregation->mAggrDistFuncList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Child Node 수행 */
    STL_TRY( qlncTraceTreeFromCodePlanFuncList[ sOptAggregation->mChildNode->mType ](
                 aStringBlock,
                 sOptAggregation->mChildNode,
                 aLevel+1,
                 aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Group Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_Group( qlncStringBlock      * aStringBlock,
                                           qllOptimizationNode  * aCodePlan,
                                           stlInt32               aLevel,
                                           qllEnv               * aEnv )
{
    qlnoGroup   * sOptGroup = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_GROUP_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    /* Group Code Plan 설정 */
    sOptGroup = (qlnoGroup*)(aCodePlan->mOptNode);

    /* Code Plan Name */
    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "%s[GROUP] IDX: %d AGGRCNT: %d AGGRDISTCNT: %d OPTPTR: 0x%p INITPTR: 0x%p\n",
                 qlncTraceTreePrefix[aLevel],
                 aCodePlan->mIdx,
                 sOptGroup->mAggrFuncCnt,
                 sOptGroup->mAggrDistFuncCnt,
                 sOptGroup,
                 sOptGroup->mInstantNode )
             == STL_SUCCESS );

    /* Key Column List */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "KEY COL",
                                                    sOptGroup->mInstantNode->mKeyColList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Record Column List */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "RECORD COL",
                                                    sOptGroup->mInstantNode->mRecColList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Read Column List */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "READ COL",
                                                    sOptGroup->mInstantNode->mReadColList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Outer Column */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "OUTER COLUMN",
                                                    sOptGroup->mOuterColumnList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Filter */
    STL_TRY( qlncTraceTreeFromCodePlan_Expr( aStringBlock,
                                             "FILTER",
                                             sOptGroup->mTotalFilterExpr,
                                             aLevel + 1,
                                             aEnv )
             == STL_SUCCESS );

    /* Aggregation Function */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "AGGR FUNC",
                                                    sOptGroup->mAggrFuncList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Aggregation Distinct Function */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "AGGR DIST FUNC",
                                                    sOptGroup->mAggrDistFuncList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Child Node 수행 */
    STL_TRY( qlncTraceTreeFromCodePlanFuncList[ sOptGroup->mChildNode->mType ](
                 aStringBlock,
                 sOptGroup->mChildNode,
                 aLevel+1,
                 aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Table Access Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_TableAccess( qlncStringBlock        * aStringBlock,
                                                 qllOptimizationNode    * aCodePlan,
                                                 stlInt32                 aLevel,
                                                 qllEnv                 * aEnv )
{
    qlnoTableAccess * sOptTableAccess   = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_TABLE_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    /* Table Access Code Plan 설정 */
    sOptTableAccess = (qlnoTableAccess*)(aCodePlan->mOptNode);

    /* Code Plan Name */
    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "%s[TABLE ACCESS] IDX: %d TBLNAM: %s TBLHND: 0x%p, PHYHND: 0x%p AGGRCNT: %d AGGRDISTCNT: %d OPTPTR: 0x%p INITPTR: 0x%p\n",
                 qlncTraceTreePrefix[aLevel],
                 aCodePlan->mIdx,
                 ellGetTableName( sOptTableAccess->mTableHandle ),
                 sOptTableAccess->mTableHandle,
                 ellGetTableHandle( sOptTableAccess->mTableHandle ),
                 sOptTableAccess->mAggrFuncCnt,
                 sOptTableAccess->mAggrDistFuncCnt,
                 sOptTableAccess,
                 sOptTableAccess->mRelationNode )
             == STL_SUCCESS );

    /* Dump Option */
    if( sOptTableAccess->mDumpOption != NULL )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     "%s<DUMP OPTION> %S\n",
                     qlncTraceTreePrefix[aLevel+1],
                     sOptTableAccess->mDumpOption )
                 == STL_SUCCESS );
    }

    /* Read Column List */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "READ COL",
                                                    sOptTableAccess->mReadColList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Rowid Column */
    STL_TRY( qlncTraceTreeFromCodePlan_Expr( aStringBlock,
                                             "ROWID COL",
                                             sOptTableAccess->mRowIdColumn,
                                             aLevel + 1,
                                             aEnv )
             == STL_SUCCESS );

    /* Outer Column */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "OUTER COLUMN",
                                                    sOptTableAccess->mOuterColumnList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Physical Filter */
    STL_TRY( qlncTraceTreeFromCodePlan_Expr( aStringBlock,
                                             "PHYSICAL FILTER",
                                             sOptTableAccess->mPhysicalFilterExpr,
                                             aLevel + 1,
                                             aEnv )
             == STL_SUCCESS );

    /* Logical Filter */
    STL_TRY( qlncTraceTreeFromCodePlan_Expr( aStringBlock,
                                             "LOGICAL FILTER",
                                             sOptTableAccess->mLogicalFilterExpr,
                                             aLevel + 1,
                                             aEnv )
             == STL_SUCCESS );

    /* Aggregation Function */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "AGGR FUNC",
                                                    sOptTableAccess->mAggrFuncList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Aggregation Distinct Function */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "AGGR DIST FUNC",
                                                    sOptTableAccess->mAggrDistFuncList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Index Access Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_IndexAccess( qlncStringBlock        * aStringBlock,
                                                 qllOptimizationNode    * aCodePlan,
                                                 stlInt32                 aLevel,
                                                 qllEnv                 * aEnv )
{
    qlnoIndexAccess * sOptIndexAccess   = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_INDEX_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    /* Index Access Code Plan 설정 */
    sOptIndexAccess = (qlnoIndexAccess*)(aCodePlan->mOptNode);

    /* Code Plan Name */
    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "%s[INDEX ACCESS] IDX: %d TBLNAM: %s TBLHND: 0x%p, TBLPHYHND: 0x%p IDXNAM: %s IDXHND: 0x%p IDXPHYHND: 0x%p SCAN: %s AGGRCNT: %d AGGRDISTCNT: %d OPTPTR: 0x%p INITPTR: 0x%p\n",
                 qlncTraceTreePrefix[aLevel],
                 aCodePlan->mIdx,
                 ellGetTableName( sOptIndexAccess->mTableHandle ),
                 sOptIndexAccess->mTableHandle,
                 sOptIndexAccess->mTablePhysicalHandle,
                 ellGetIndexName( sOptIndexAccess->mIndexHandle ),
                 sOptIndexAccess->mIndexHandle,
                 sOptIndexAccess->mIndexPhysicalHandle,
                 (sOptIndexAccess->mScanDirection == SML_SCAN_FORWARD ? "FORWARD" : "BACKWARD"),
                 sOptIndexAccess->mAggrFuncCnt,
                 sOptIndexAccess->mAggrDistFuncCnt,
                 sOptIndexAccess,
                 sOptIndexAccess->mRelationNode )
             == STL_SUCCESS );

    /* Index Read Column List */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "INDEX READ COL",
                                                    sOptIndexAccess->mIndexReadColList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Table Read Column List */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "TABLE READ COL",
                                                    sOptIndexAccess->mTableReadColList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Rowid Column */
    STL_TRY( qlncTraceTreeFromCodePlan_Expr( aStringBlock,
                                             "ROWID COL",
                                             sOptIndexAccess->mRowIdColumn,
                                             aLevel + 1,
                                             aEnv )
             == STL_SUCCESS );

    /* Outer Column */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "OUTER COLUMN",
                                                    sOptIndexAccess->mOuterColumnList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Index Scan Info */
    STL_TRY( qlncTraceTreeFromCodePlan_IndexScanInfo( aStringBlock,
                                                      sOptIndexAccess->mIndexScanInfo,
                                                      aLevel + 1,
                                                      aEnv )
             == STL_SUCCESS );

    /* Aggregation Function */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "AGGR FUNC",
                                                    sOptIndexAccess->mAggrFuncList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Aggregation Distinct Function */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "AGGR DIST FUNC",
                                                    sOptIndexAccess->mAggrDistFuncList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Rowid Access Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_RowidAccess( qlncStringBlock        * aStringBlock,
                                                 qllOptimizationNode    * aCodePlan,
                                                 stlInt32                 aLevel,
                                                 qllEnv                 * aEnv )
{
    qlnoRowIdAccess * sOptRowidAccess   = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_ROWID_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    /* Rowid Access Code Plan 설정 */
    sOptRowidAccess = (qlnoRowIdAccess*)(aCodePlan->mOptNode);

    /* Code Plan Name */
    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "%s[ROWID ACCESS] IDX: %d TBLNAM: %s TBLHND: 0x%p, PHYHND: 0x%p AGGRCNT: %d OPTPTR: 0x%p INITPTR: 0x%p\n",
                 qlncTraceTreePrefix[aLevel],
                 aCodePlan->mIdx,
                 ellGetTableName( sOptRowidAccess->mTableHandle ),
                 sOptRowidAccess->mTableHandle,
                 sOptRowidAccess->mPhysicalHandle,
                 sOptRowidAccess->mAggrFuncCnt,
                 sOptRowidAccess,
                 sOptRowidAccess->mRelationNode )
             == STL_SUCCESS );

    /* Read Column List */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "READ COL",
                                                    sOptRowidAccess->mReadColList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Rowid Column */
    STL_TRY( qlncTraceTreeFromCodePlan_Expr( aStringBlock,
                                             "ROWID COL",
                                             sOptRowidAccess->mRowIdColumn,
                                             aLevel + 1,
                                             aEnv )
             == STL_SUCCESS );

    /* Outer Column */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "OUTER COLUMN",
                                                    sOptRowidAccess->mOuterColumnList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Rowid Scan Expr */
    STL_TRY( qlncTraceTreeFromCodePlan_Expr( aStringBlock,
                                             "ROWID FILTER",
                                             sOptRowidAccess->mRowIdScanExpr,
                                             aLevel + 1,
                                             aEnv )
             == STL_SUCCESS );

    /* Physical Filter */
    STL_TRY( qlncTraceTreeFromCodePlan_Expr( aStringBlock,
                                             "PHYSICAL FILTER",
                                             sOptRowidAccess->mPhysicalFilterExpr,
                                             aLevel + 1,
                                             aEnv )
             == STL_SUCCESS );

    /* Logical Filter */
    STL_TRY( qlncTraceTreeFromCodePlan_Expr( aStringBlock,
                                             "LOGICAL FILTER",
                                             sOptRowidAccess->mLogicalFilterExpr,
                                             aLevel + 1,
                                             aEnv )
             == STL_SUCCESS );

    /* Aggregation Function */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "AGGR FUNC",
                                                    sOptRowidAccess->mAggrFuncList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Fast Dual Access Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_FastDualAccess( qlncStringBlock     * aStringBlock,
                                                    qllOptimizationNode * aCodePlan,
                                                    stlInt32              aLevel,
                                                    qllEnv              * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_FAST_DUAL_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Flat Instant Access Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_FlatInstantAccess( qlncStringBlock      * aStringBlock,
                                                       qllOptimizationNode  * aCodePlan,
                                                       stlInt32               aLevel,
                                                       qllEnv               * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_FLAT_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    STL_TRY( qlncTraceTreeFromCodePlan_Instant( aStringBlock,
                                                aCodePlan,
                                                aLevel,
                                                aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Sort Instant Access Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_SortInstantAccess( qlncStringBlock      * aStringBlock,
                                                       qllOptimizationNode  * aCodePlan,
                                                       stlInt32               aLevel,
                                                       qllEnv               * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_SORT_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    STL_TRY( qlncTraceTreeFromCodePlan_Instant( aStringBlock,
                                                aCodePlan,
                                                aLevel,
                                                aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Hash Instant Access Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_HashInstantAccess( qlncStringBlock      * aStringBlock,
                                                       qllOptimizationNode  * aCodePlan,
                                                       stlInt32               aLevel,
                                                       qllEnv               * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_HASH_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    STL_TRY( qlncTraceTreeFromCodePlan_Instant( aStringBlock,
                                                aCodePlan,
                                                aLevel,
                                                aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Group Hash Instant Access Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_GroupHashInstantAccess( qlncStringBlock     * aStringBlock,
                                                            qllOptimizationNode * aCodePlan,
                                                            stlInt32              aLevel,
                                                            qllEnv              * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_GROUP_HASH_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    STL_TRY( qlncTraceTreeFromCodePlan_Instant( aStringBlock,
                                                aCodePlan,
                                                aLevel,
                                                aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Group Sort Instant Access Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_GroupSortInstantAccess( qlncStringBlock     * aStringBlock,
                                                            qllOptimizationNode * aCodePlan,
                                                            stlInt32              aLevel,
                                                            qllEnv              * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_GROUP_SORT_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    STL_TRY( qlncTraceTreeFromCodePlan_Instant( aStringBlock,
                                                aCodePlan,
                                                aLevel,
                                                aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Sort Join Instant Access Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_SortJoinInstantAccess( qlncStringBlock      * aStringBlock,
                                                           qllOptimizationNode  * aCodePlan,
                                                           stlInt32               aLevel,
                                                           qllEnv               * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_SORT_JOIN_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    STL_TRY( qlncTraceTreeFromCodePlan_Instant( aStringBlock,
                                                aCodePlan,
                                                aLevel,
                                                aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Hash Join Instant Access Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_HashJoinInstantAccess( qlncStringBlock      * aStringBlock,
                                                           qllOptimizationNode  * aCodePlan,
                                                           stlInt32               aLevel,
                                                           qllEnv               * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan->mType == QLL_PLAN_NODE_HASH_JOIN_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    STL_TRY( qlncTraceTreeFromCodePlan_Instant( aStringBlock,
                                                aCodePlan,
                                                aLevel,
                                                aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Expression에 대한 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aTitle          Title
 * @param[in]   aExpr           Expression
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_Expr( qlncStringBlock   * aStringBlock,
                                          stlChar           * aTitle,
                                          qlvInitExpression * aExpr,
                                          stlInt32            aLevel,
                                          qllEnv            * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTitle != NULL, QLL_ERROR_STACK(aEnv) );


    if( aExpr != NULL )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "%s<%s>\n",
                                                     qlncTraceTreePrefix[aLevel],
                                                     aTitle )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceTreeFromCodeExprFuncList[ aExpr->mType ](
                     aStringBlock,
                     aExpr,
                     aLevel + 1,
                     aEnv )
                 == STL_SUCCESS );
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Reference Expression List에 대한 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aTitle          Title
 * @param[in]   aRefExprList    Reference Expression List
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_RefExprList( qlncStringBlock    * aStringBlock,
                                                 stlChar            * aTitle,
                                                 qlvRefExprList     * aRefExprList,
                                                 stlInt32             aLevel,
                                                 qllEnv             * aEnv )
{
    stlInt32          i;
    qlvRefExprItem  * sRefExprItem  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTitle != NULL, QLL_ERROR_STACK(aEnv) );


    if( aRefExprList != NULL )
    {
        i = 0;
        sRefExprItem = aRefExprList->mHead;
        while( sRefExprItem != NULL )
        {
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "%s<%s-%03d>\n",
                                                         qlncTraceTreePrefix[aLevel],
                                                         aTitle,
                                                         i )
                     == STL_SUCCESS );

            STL_TRY( qlncTraceTreeFromCodeExprFuncList[ sRefExprItem->mExprPtr->mType ](
                         aStringBlock,
                         sRefExprItem->mExprPtr,
                         aLevel + 1,
                         aEnv )
                     == STL_SUCCESS );

            i++;
            sRefExprItem = sRefExprItem->mNext;
        }
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Index Scan Info로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aIndexScanInfo  Index Scan Info
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_IndexScanInfo( qlncStringBlock  * aStringBlock,
                                                   qloIndexScanInfo * aIndexScanInfo,
                                                   stlInt32           aLevel,
                                                   qllEnv           * aEnv )
{
    stlInt32          i;
    stlInt32          sIdx;
    qlvRefExprItem  * sRefExprItem  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexScanInfo != NULL, QLL_ERROR_STACK(aEnv) );


    /* Min Range Expression */
    if( aIndexScanInfo->mMinRangeExpr != NULL )
    {
        sIdx = 0;
        for( i = 0; i < aIndexScanInfo->mKeyColCount; i++ )
        {
            if( aIndexScanInfo->mMinRangeExpr[i] != NULL )
            {
                sRefExprItem = aIndexScanInfo->mMinRangeExpr[i]->mHead;
                while( sRefExprItem != NULL )
                {
                    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                                 aEnv,
                                                                 "%s<MIN RANGE-%03d>\n",
                                                                 qlncTraceTreePrefix[aLevel],
                                                                 sIdx )
                             == STL_SUCCESS );

                    STL_TRY( qlncTraceTreeFromCodeExprFuncList[ sRefExprItem->mExprPtr->mType ](
                                 aStringBlock,
                                 sRefExprItem->mExprPtr,
                                 aLevel + 1,
                                 aEnv )
                             == STL_SUCCESS );

                    sIdx++;
                    sRefExprItem = sRefExprItem->mNext;
                }
            }
        }
    }

    /* Max Range Expression */
    if( aIndexScanInfo->mMaxRangeExpr != NULL )
    {
        sIdx = 0;
        for( i = 0; i < aIndexScanInfo->mKeyColCount; i++ )
        {
            if( aIndexScanInfo->mMaxRangeExpr[i] != NULL )
            {
                sRefExprItem = aIndexScanInfo->mMaxRangeExpr[i]->mHead;
                while( sRefExprItem != NULL )
                {
                    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                                 aEnv,
                                                                 "%s<MAX RANGE-%03d>\n",
                                                                 qlncTraceTreePrefix[aLevel],
                                                                 sIdx )
                             == STL_SUCCESS );

                    STL_TRY( qlncTraceTreeFromCodeExprFuncList[ sRefExprItem->mExprPtr->mType ](
                                 aStringBlock,
                                 sRefExprItem->mExprPtr,
                                 aLevel + 1,
                                 aEnv )
                             == STL_SUCCESS );

                    sIdx++;
                    sRefExprItem = sRefExprItem->mNext;
                }
            }
        }
    }

    /* Physical Key Filter */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList(
                 aStringBlock,
                 "PHYSICAL KEY FILTER",
                 aIndexScanInfo->mPhysicalKeyFilterExprList,
                 aLevel,
                 aEnv )
             == STL_SUCCESS );

    /* Logical Key Filter */
    STL_TRY( qlncTraceTreeFromCodePlan_Expr( aStringBlock,
                                             "LOGICAL KEY FILTER",
                                             aIndexScanInfo->mLogicalKeyFilter,
                                             aLevel,
                                             aEnv )
             == STL_SUCCESS );

    /* Physical Table Filter */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList(
                 aStringBlock,
                 "PHYSICAL TABLE FILTER",
                 aIndexScanInfo->mPhysicalTableFilterExprList,
                 aLevel,
                 aEnv )
             == STL_SUCCESS );

    /* Logical Table Filter */
    STL_TRY( qlncTraceTreeFromCodePlan_Expr( aStringBlock,
                                             "LOGICAL TABLE FILTER",
                                             aIndexScanInfo->mLogicalTableFilter,
                                             aLevel,
                                             aEnv )
             == STL_SUCCESS );

    /* Null Value Filter */
    STL_TRY( qlncTraceTreeFromCodePlan_Expr( aStringBlock,
                                             "NULL VALUE FILTER",
                                             aIndexScanInfo->mNullValueExpr,
                                             aLevel,
                                             aEnv )
             == STL_SUCCESS );

    /* False Value Filter */
    STL_TRY( qlncTraceTreeFromCodePlan_Expr( aStringBlock,
                                             "FALSE VALUE FILTER",
                                             aIndexScanInfo->mFalseValueExpr,
                                             aLevel,
                                             aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Hash Scan Info로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aHashScanInfo   Hash Scan Info
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_HashScanInfo( qlncStringBlock   * aStringBlock,
                                                  qloHashScanInfo   * aHashScanInfo,
                                                  stlInt32            aLevel,
                                                  qllEnv            * aEnv )
{
    stlInt32          i;
    stlInt32          sIdx;
    qlvRefExprItem  * sRefExprItem  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );


    if( aHashScanInfo == NULL )
    {
        STL_THROW( RAMP_FINISH );
    }

    /* Hash Filter Expression */
    if( aHashScanInfo->mHashFilterExpr != NULL )
    {
        sIdx = 0;
        for( i = 0; i < aHashScanInfo->mHashColCount; i++ )
        {
            if( aHashScanInfo->mHashFilterExpr[i] != NULL )
            {
                sRefExprItem = aHashScanInfo->mHashFilterExpr[i]->mHead;
                while( sRefExprItem != NULL )
                {
                    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                                 aEnv,
                                                                 "%s<HASH FILTER-%03d>\n",
                                                                 qlncTraceTreePrefix[aLevel],
                                                                 sIdx )
                             == STL_SUCCESS );

                    STL_TRY( qlncTraceTreeFromCodeExprFuncList[ sRefExprItem->mExprPtr->mType ](
                                 aStringBlock,
                                 sRefExprItem->mExprPtr,
                                 aLevel + 1,
                                 aEnv )
                             == STL_SUCCESS );

                    sIdx++;
                    sRefExprItem = sRefExprItem->mNext;
                }
            }
        }
    }

    /* Physical Table Filter */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList(
                 aStringBlock,
                 "PHYSICAL TABLE FILTER",
                 aHashScanInfo->mPhysicalTableFilterExprList,
                 aLevel,
                 aEnv )
             == STL_SUCCESS );

    /* Logical Table Filter */
    STL_TRY( qlncTraceTreeFromCodePlan_Expr( aStringBlock,
                                             "LOGICAL TABLE FILTER",
                                             aHashScanInfo->mLogicalTableFilter,
                                             aLevel,
                                             aEnv )
             == STL_SUCCESS );


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Instant Code Plan으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aCodePlan       Code Plan
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodePlan_Instant( qlncStringBlock      * aStringBlock,
                                             qllOptimizationNode  * aCodePlan,
                                             stlInt32               aLevel,
                                             qllEnv               * aEnv )
{
    stlInt32          i;
    qlnoInstant     * sOptInstant   = NULL;
    qlvRefExprItem  * sRefExprItem  = NULL;
    stlChar         * sInstName     = NULL;
    stlChar         * sScrollName   = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );


    /* Instant Code Plan 설정 */
    sOptInstant = (qlnoInstant*)(aCodePlan->mOptNode);

    switch( sOptInstant->mType )
    {
        case QLV_NODE_TYPE_FLAT_INSTANT:
            sInstName = "FLAT INSTANT";
            break;

        case QLV_NODE_TYPE_SORT_INSTANT:
            sInstName = "SORT INSTANT";
            break;

        case QLV_NODE_TYPE_HASH_INSTANT:
            sInstName = "HASH INSTANT";
            break;

        case QLV_NODE_TYPE_GROUP_HASH_INSTANT:
            sInstName = "GROUP HASH INSTANT";
            break;

        case QLV_NODE_TYPE_GROUP_SORT_INSTANT:
            sInstName = "GROUP SORT INSTANT";
            break;

        case QLV_NODE_TYPE_SORT_JOIN_INSTANT:
            sInstName = "SORT JOIN INSTANT";
            break;

        case QLV_NODE_TYPE_HASH_JOIN_INSTANT:
            sInstName = "HASH JOIN INSTANT";
            break;

        default:
            STL_DASSERT( 0 );
            break;
    }

    switch( sOptInstant->mScrollable )
    {
        case ELL_CURSOR_SCROLLABILITY_NA:
            sScrollName = "N/A";
            break;

        case ELL_CURSOR_SCROLLABILITY_NO_SCROLL:
            sScrollName = "NO";
            break;

        case ELL_CURSOR_SCROLLABILITY_SCROLL:
            sScrollName = "YES";
            break;

        default:
            STL_DASSERT( 0 );
            break;
    }

    /* Code Plan Name */
    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "%s[%s] IDX: %d SCROLL: %s AGGRCNT: %d AGGRDISTCNT: %d NESTAGGRCNT: %d NESTAGGRDISTCNT: %d OPTPTR: 0x%p INITPTR: 0x%p\n",
                 qlncTraceTreePrefix[aLevel],
                 sInstName,
                 aCodePlan->mIdx,
                 sScrollName,
                 sOptInstant->mAggrFuncCnt,
                 sOptInstant->mAggrDistFuncCnt,
                 sOptInstant->mNestedAggrFuncCnt,
                 sOptInstant->mNestedAggrDistFuncCnt,
                 sOptInstant,
                 sOptInstant->mInstantNode )
             == STL_SUCCESS );

    /* Attr */
    switch( sOptInstant->mType )
    {
        case QLV_NODE_TYPE_FLAT_INSTANT:
            break;

        case QLV_NODE_TYPE_SORT_INSTANT:
        case QLV_NODE_TYPE_GROUP_SORT_INSTANT:
        case QLV_NODE_TYPE_SORT_JOIN_INSTANT:
            STL_TRY( qlncTraceAppendStringToStringBlock(
                         aStringBlock,
                         aEnv,
                         "%s<SORT ATTR> TopDown: %c Volatile: %c LeafOnly: %c Unique: %c\n",
                         qlncTraceTreePrefix[aLevel+1],
                         (sOptInstant->mSortTableAttr.mTopDown == STL_TRUE
                          ? 'T' : 'F'),
                         (sOptInstant->mSortTableAttr.mVolatile == STL_TRUE
                          ? 'T' : 'F'),
                         (sOptInstant->mSortTableAttr.mLeafOnly == STL_TRUE
                          ? 'T' : 'F'),
                         (sOptInstant->mSortTableAttr.mDistinct == STL_TRUE
                          ? 'T' : 'F'))
                     == STL_SUCCESS );

            break;

        case QLV_NODE_TYPE_HASH_INSTANT:
        case QLV_NODE_TYPE_GROUP_HASH_INSTANT:
        case QLV_NODE_TYPE_HASH_JOIN_INSTANT:
            STL_TRY( qlncTraceAppendStringToStringBlock(
                         aStringBlock,
                         aEnv,
                         "%s<HASH ATTR> Logging: %c SkipNull: %c Unique: %c Primary: %c\n",
                         qlncTraceTreePrefix[aLevel+1],
                         ((sOptInstant->mHashTableAttr.mLoggingFlag | SML_INDEX_LOGGING_MASK)
                          == SML_INDEX_LOGGING_YES
                          ? 'T' : 'F'),
                         ((sOptInstant->mHashTableAttr.mSkipNullFlag | SML_INDEX_SKIP_NULL_MASK)
                          == SML_INDEX_SKIP_NULL_YES
                          ? 'T' : 'F'),
                         ((sOptInstant->mHashTableAttr.mUniquenessFlag | SML_INDEX_UNIQUENESS_MASK)
                          == SML_INDEX_UNIQUENESS_YES
                          ? 'T' : 'F'),
                         ((sOptInstant->mHashTableAttr.mPrimaryFlag | SML_INDEX_PRIMARY_MASK)
                          == SML_INDEX_PRIMARY_YES
                          ? 'T' : 'F'))
                     == STL_SUCCESS );

            break;

        default:
            STL_DASSERT( 0 );
            break;
    }

    /* Key Column List */
    switch( sOptInstant->mType )
    {
        case QLV_NODE_TYPE_FLAT_INSTANT:
        case QLV_NODE_TYPE_HASH_INSTANT:
        case QLV_NODE_TYPE_GROUP_HASH_INSTANT:
        case QLV_NODE_TYPE_HASH_JOIN_INSTANT:
            STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                            "KEY COL",
                                                            sOptInstant->mInstantNode->mKeyColList,
                                                            aLevel + 1,
                                                            aEnv )
                     == STL_SUCCESS );
            break;

        case QLV_NODE_TYPE_SORT_INSTANT:
        case QLV_NODE_TYPE_GROUP_SORT_INSTANT:
        case QLV_NODE_TYPE_SORT_JOIN_INSTANT:
            if( sOptInstant->mInstantNode->mKeyColList != NULL )
            {
                i = 0;
                sRefExprItem = sOptInstant->mInstantNode->mKeyColList->mHead;
                while( sRefExprItem != NULL )
                {
                    STL_TRY( qlncTraceAppendStringToStringBlock(
                                 aStringBlock,
                                 aEnv,
                                 "%s<KEY COL-%03d> %s %s\n",
                                 qlncTraceTreePrefix[aLevel+1],
                                 i,
                                 (DTL_GET_KEY_FLAG_SORT_ORDER( sOptInstant->mInstantNode->mKeyFlags[i] )
                                  == DTL_KEYCOLUMN_INDEX_ORDER_ASC
                                  ? "ASC" : "DESC"),
                                 ( DTL_GET_KEY_FLAG_NULL_ORDER( sOptInstant->mInstantNode->mKeyFlags[i] )
                                   == DTL_KEYCOLUMN_INDEX_NULL_ORDER_FIRST
                                   ? "NULLS FIRST" : "NULLS LAST") )
                             == STL_SUCCESS );

                    STL_TRY( qlncTraceTreeFromCodeExprFuncList[ sRefExprItem->mExprPtr->mType ](
                                 aStringBlock,
                                 sRefExprItem->mExprPtr,
                                 aLevel + 2,
                                 aEnv )
                             == STL_SUCCESS );

                    i++;
                    sRefExprItem = sRefExprItem->mNext;
                }
            }
            break;

        default:
            STL_DASSERT( 0 );
            break;
    }

    /* Record Column List */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "RECORD COL",
                                                    sOptInstant->mInstantNode->mRecColList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Read Column List */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "READ COL",
                                                    sOptInstant->mInstantNode->mReadColList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Outer Column */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "OUTER COLUMN",
                                                    sOptInstant->mOuterColumnList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Scan Info */
    switch( sOptInstant->mType )
    {
        case QLV_NODE_TYPE_FLAT_INSTANT:
            break;

        case QLV_NODE_TYPE_SORT_INSTANT:
        case QLV_NODE_TYPE_GROUP_SORT_INSTANT:
        case QLV_NODE_TYPE_SORT_JOIN_INSTANT:
            STL_TRY( qlncTraceTreeFromCodePlan_IndexScanInfo( aStringBlock,
                                                              sOptInstant->mIndexScanInfo,
                                                              aLevel + 1,
                                                              aEnv )
                     == STL_SUCCESS );

            break;

        case QLV_NODE_TYPE_HASH_INSTANT:
        case QLV_NODE_TYPE_GROUP_HASH_INSTANT:
        case QLV_NODE_TYPE_HASH_JOIN_INSTANT:
            STL_TRY( qlncTraceTreeFromCodePlan_HashScanInfo( aStringBlock,
                                                             sOptInstant->mHashScanInfo,
                                                             aLevel + 1,
                                                             aEnv )
                     == STL_SUCCESS );

            break;

        default:
            STL_DASSERT( 0 );
            break;
    }

    /* Filter */
    STL_TRY( qlncTraceTreeFromCodePlan_Expr( aStringBlock,
                                             "FILTER",
                                             sOptInstant->mTotalFilterExpr,
                                             aLevel + 1,
                                             aEnv )
             == STL_SUCCESS );

    /* Aggregation Function */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "AGGR FUNC",
                                                    sOptInstant->mAggrFuncList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Aggregation Distinct Function */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "AGGR DIST FUNC",
                                                    sOptInstant->mAggrDistFuncList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Nested Aggregation Function */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "NESTED AGGR FUNC",
                                                    sOptInstant->mNestedAggrFuncList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Nested Aggregation Distinct Function */
    STL_TRY( qlncTraceTreeFromCodePlan_RefExprList( aStringBlock,
                                                    "NESTED AGGR DIST FUNC",
                                                    sOptInstant->mNestedAggrDistFuncList,
                                                    aLevel + 1,
                                                    aEnv )
             == STL_SUCCESS );

    /* Child Node 수행 */
    STL_TRY( qlncTraceTreeFromCodePlanFuncList[ sOptInstant->mChildNode->mType ](
                 aStringBlock,
                 sOptInstant->mChildNode,
                 aLevel+1,
                 aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlnc */
