/*******************************************************************************
 * qlnoNode.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlnoNode.c 10976 2014-01-16 10:14:03Z jhkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlnoNode.c
 * @brief SQL Processor Layer Optimization Node Common Functions
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlno
 * @{
 */


const qlndDataOptimizeNodeFuncPtr qlndDataOptNodeFuncList[ QLL_PLAN_NODE_TYPE_MAX ] =
{
    qlndDataOptimizeSelectStmt,     /**< QLL_PLAN_NODE_STMT_SELECT_TYPE */
    qlndDataOptimizeInsertStmt,     /**< QLL_PLAN_NODE_STMT_INSERT_TYPE */
    qlndDataOptimizeUpdateStmt,     /**< QLL_PLAN_NODE_STMT_UPDATE_TYPE */
    qlndDataOptimizeDeleteStmt,     /**< QLL_PLAN_NODE_STMT_DELETE_TYPE */
    NULL,     /**< QLL_PLAN_NODE_STMT_MERGE_TYPE */

    qlndDataOptimizeQuerySpec,     /**< QLL_PLAN_NODE_QUERY_SPEC_TYPE */
    NULL,     /**< QLL_PLAN_NODE_INDEX_BUILD_TYPE */
    NULL,     /**< QLL_PLAN_NODE_FOR_UPDATE_TYPE */
    NULL,     /**< QLL_PLAN_NODE_MERGE_TYPE */
    NULL,     /**< QLL_PLAN_NODE_VIEW_TYPE */
    NULL,     /**< QLL_PLAN_NODE_VIEW_PUSHED_PRED_TYPE */
    NULL,     /**< QLL_PLAN_NODE_FILTER_TYPE */
    NULL,     /**< QLL_PLAN_NODE_INLIST_TYPE */

    qlndDataOptimizeQuerySet,   /**< QLL_PLAN_NODE_QUERY_SET_TYPE */
    qlndDataOptimizeSetOP,      /**< QLL_PLAN_NODE_UNION_ALL_TYPE */
    qlndDataOptimizeSetOP,      /**< QLL_PLAN_NODE_UNION_DISTINCT_TYPE */
    qlndDataOptimizeSetOP,      /**< QLL_PLAN_NODE_EXCEPT_ALL_TYPE */
    qlndDataOptimizeSetOP,      /**< QLL_PLAN_NODE_EXCEPT_DISTINCT_TYPE */
    qlndDataOptimizeSetOP,      /**< QLL_PLAN_NODE_INTERSECT_ALL_TYPE */
    qlndDataOptimizeSetOP,      /**< QLL_PLAN_NODE_INTERSECT_DISTINCT_TYPE */

    qlndDataOptimizeSubQuery,            /**< QLL_PLAN_NODE_SUB_QUERY_TYPE */
    qlndDataOptimizeSubQueryList,        /**< QLL_PLAN_NODE_SUB_QUERY_LIST_TYPE */
    qlndDataOptimizeSubQueryFunc,        /**< QLL_PLAN_NODE_SUB_QUERY_FUNCTION_TYPE */
    qlndDataOptimizeSubQueryFilter,      /**< QLL_PLAN_NODE_SUB_QUERY_FILTER_TYPE */

    qlndDataOptimizeConcat,              /**< QLL_PLAN_NODE_CONCATENATION_TYPE */

    qlndDataOptimizeNestedLoopsJoin,     /**< QLL_PLAN_NODE_NESTED_LOOPS_TYPE */
    qlndDataOptimizeSortMergeJoin,       /**< QLL_PLAN_NODE_MERGE_JOIN_TYPE */
    qlndDataOptimizeHashJoin,            /**< QLL_PLAN_NODE_HASH_JOIN_TYPE */
    NULL,     /**< QLL_PLAN_NODE_INDEX_JOIN_TYPE */
    
    NULL,     /**< QLL_PLAN_NODE_CONNECT_BY_TYPE */
    NULL,     /**< QLL_PLAN_NODE_PSEUDO_COLUMN_TYPE */
    NULL,     /**< QLL_PLAN_NODE_COUNT_TYPE */
    NULL,     /**< QLL_PLAN_NODE_WINDOW_TYPE */
    NULL,     /**< QLL_PLAN_NODE_HASH_TYPE */
    qlndDataOptimizeHashAggregation,     /**< QLL_PLAN_NODE_HASH_AGGREGATION_TYPE */
    qlndDataOptimizeGroup,               /**< QLL_PLAN_NODE_GROUP_TYPE */

    qlndDataOptimizeTableAccess,     /**< QLL_PLAN_NODE_TABLE_ACCESS_TYPE */
    qlndDataOptimizeIndexAccess,     /**< QLL_PLAN_NODE_INDEX_ACCESS_TYPE */
    qlndDataOptimizeRowIdAccess,     /**< QLL_PLAN_NODE_ROWID_ACCESS_TYPE */
    NULL,     /**< QLL_PLAN_NODE_FAST_DUAL_ACCESS_TYPE */
    NULL,      /**< QLL_PLAN_NODE_FLAT_INSTANT_ACCESS_TYPE */
    qlndDataOptimizeSortInstantAccess,      /**< QLL_PLAN_NODE_SORT_INSTANT_ACCESS_TYPE */
    NULL,      /**< QLL_PLAN_NODE_HASH_INSTANT_ACCESS_TYPE */
    qlndDataOptimizeGroupHashInstantAccess, /**< QLL_PLAN_NODE_GROUP_HASH_INSTANT_ACCESS_TYPE */
    NULL, /**< QLL_PLAN_NODE_GROUP_SORT_INSTANT_ACCESS_TYPE */
    qlndDataOptimizeSortJoinInstantAccess,  /**< QLL_PLAN_NODE_SORT_JOIN_INSTANT_ACCESS_TYPE */
    qlndDataOptimizeHashJoinInstantAccess   /**< QLL_PLAN_NODE_HASH_JOIN_INSTANT_ACCESS_TYPE */
};


/** @} qlno */
