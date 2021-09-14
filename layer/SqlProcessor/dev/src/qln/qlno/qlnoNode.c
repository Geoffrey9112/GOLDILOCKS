/*******************************************************************************
 * qlnoNode.c
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

const qlnoCodeOptNodeFuncPtr qlnoCodeOptNodeFuncList[ QLNC_PLAN_TYPE_MAX ] =
{
    qlnoCodeOptimizeQuerySet,           /**< QLNC_PLAN_TYPE_QUERY_SET */
    qlnoCodeOptimizeSetOP,              /**< QLNC_PLAN_TYPE_SET_OP */
    qlnoCodeOptimizeQuerySpec,          /**< QLNC_PLAN_TYPE_QUERY_SPEC */

    qlnoCodeOptimizeTableAccess,        /**< QLNC_PLAN_TYPE_TABLE_SCAN */
    qlnoCodeOptimizeIndexAccess,        /**< QLNC_PLAN_TYPE_INDEX_SCAN */
    qlnoCodeOptimizeIndexCombine,       /**< QLNC_PLAN_TYPE_INDEX_COMBINE */
    qlnoCodeOptimizeRowidAccess,        /**< QLNC_PLAN_TYPE_ROWID_SCAN */

    qlnoCodeOptimizeSubTable,           /**< QLNC_PLAN_TYPE_SUB_TABLE */

    qlnoCodeOptimizeNestedLoops,        /**< QLNC_PLAN_TYPE_NESTED_JOIN */
    qlnoCodeOptimizeMerge,              /**< QLNC_PLAN_TYPE_MERGE_JOIN */
    qlnoCodeOptimizeHash,               /**< QLNC_PLAN_TYPE_HASH_JOIN */
    qlnoCodeOptimizeJoinCombine,        /**< QLNC_PLAN_TYPE_JOIN_COMBINE */

    qlnoCodeOptimizeSortInstant,        /**< QLNC_PLAN_TYPE_SORT_INSTANT */
    NULL,   /**< QLNC_PLAN_TYPE_SORT_GROUP_INSTANT */
    qlnoCodeOptimizeSortJoinInstant,    /**< QLNC_PLAN_TYPE_SORT_JOIN_INSTANT */

    NULL,        /**< QLNC_PLAN_TYPE_HASH_INSTANT */
    qlnoCodeOptimizeGroupHashInstant,   /**< QLNC_PLAN_TYPE_HASH_GROUP_INSTANT */
    qlnoCodeOptimizeHashJoinInstant,    /**< QLNC_PLAN_TYPE_HASH_JOIN_INSTANT */

    qlnoCodeOptimizeSubQueryList,       /**< QLNC_PLAN_TYPE_SUB_QUERY_LIST */
    qlnoCodeOptimizeSubQueryFunc,       /**< QLNC_PLAN_TYPE_SUB_QUERY_FUNCTION */
    qlnoCodeOptimizeSubQueryFilter,     /**< QLNC_PLAN_TYPE_SUB_QUERY_FILTER */

    qlnoCodeOptimizeGroup               /**< QLNC_PLAN_TYPE_GROUP */
};

const qlnoCompleteNodeFuncPtr qlnoCompleteNodeFuncList[ QLL_PLAN_NODE_TYPE_MAX ] =
{
    qlnoCompleteSelectStmt,     /**< QLL_PLAN_NODE_STMT_SELECT_TYPE */
    qlnoCompleteInsertStmt,     /**< QLL_PLAN_NODE_STMT_INSERT_TYPE */
    qlnoCompleteUpdateStmt,     /**< QLL_PLAN_NODE_STMT_UPDATE_TYPE */
    qlnoCompleteDeleteStmt,     /**< QLL_PLAN_NODE_STMT_DELETE_TYPE */
    NULL,     /**< QLL_PLAN_NODE_STMT_MERGE_TYPE */

    qlnoCompleteQuerySpec,     /**< QLL_PLAN_NODE_QUERY_SPEC_TYPE */
    NULL,     /**< QLL_PLAN_NODE_INDEX_BUILD_TYPE */
    NULL,     /**< QLL_PLAN_NODE_FOR_UPDATE_TYPE */
    NULL,     /**< QLL_PLAN_NODE_MERGE_TYPE */
    NULL,     /**< QLL_PLAN_NODE_VIEW_TYPE */
    NULL,     /**< QLL_PLAN_NODE_VIEW_PUSHED_PRED_TYPE */
    NULL,     /**< QLL_PLAN_NODE_FILTER_TYPE */
    NULL,     /**< QLL_PLAN_NODE_INLIST_TYPE */

    qlnoCompleteQuerySet,  /**< QLL_PLAN_NODE_QUERY_SET_TYPE */
    qlnoCompleteSetOP,     /**< QLL_PLAN_NODE_UNION_ALL_TYPE */
    qlnoCompleteSetOP,     /**< QLL_PLAN_NODE_UNION_DISTINCT_TYPE */
    qlnoCompleteSetOP,     /**< QLL_PLAN_NODE_EXCEPT_ALL_TYPE */
    qlnoCompleteSetOP,     /**< QLL_PLAN_NODE_EXCEPT_DISTINCT_TYPE */
    qlnoCompleteSetOP,     /**< QLL_PLAN_NODE_INTERSECT_ALL_TYPE */
    qlnoCompleteSetOP,     /**< QLL_PLAN_NODE_INTERSECT_DISTINCT_TYPE */

    qlnoCompleteSubQuery,            /**< QLL_PLAN_NODE_SUB_QUERY_TYPE */
    qlnoCompleteSubQueryList,        /**< QLL_PLAN_NODE_SUB_QUERY_LIST_TYPE */
    qlnoCompleteSubQueryFunc,        /**< QLL_PLAN_NODE_SUB_QUERY_FUNCTION_TYPE */
    qlnoCompleteSubQueryFilter,      /**< QLL_PLAN_NODE_SUB_QUERY_FILTER_TYPE */

    qlnoCompleteConcat,              /**< QLL_PLAN_NODE_CONCATENATION_TYPE */

    qlnoCompleteNestedLoopsJoin,     /**< QLL_PLAN_NODE_NESTED_LOOPS_TYPE */
    qlnoCompleteSortMergeJoin,       /**< QLL_PLAN_NODE_MERGE_JOIN_TYPE */
    qlnoCompleteHashJoin,            /**< QLL_PLAN_NODE_HASH_JOIN_TYPE */
    NULL,     /**< QLL_PLAN_NODE_INDEX_JOIN_TYPE */
    
    NULL,     /**< QLL_PLAN_NODE_CONNECT_BY_TYPE */
    NULL,     /**< QLL_PLAN_NODE_PSEUDO_COLUMN_TYPE */
    NULL,     /**< QLL_PLAN_NODE_COUNT_TYPE */
    NULL,     /**< QLL_PLAN_NODE_WINDOW_TYPE */
    NULL,     /**< QLL_PLAN_NODE_HASH_TYPE */
    qlnoCompleteHashAggregation,     /**< QLL_PLAN_NODE_HASH_AGGREGATION_TYPE */
    qlnoCompleteGroup,               /**< QLL_PLAN_NODE_GROUP_TYPE */

    qlnoCompleteTableAccess,     /**< QLL_PLAN_NODE_TABLE_ACCESS_TYPE */
    qlnoCompleteIndexAccess,     /**< QLL_PLAN_NODE_INDEX_ACCESS_TYPE */
    qlnoCompleteRowIdAccess,     /**< QLL_PLAN_NODE_ROWID_ACCESS_TYPE */
    NULL,     /**< QLL_PLAN_NODE_FAST_DUAL_ACCESS_TYPE */
    NULL,      /**< QLL_PLAN_NODE_FLAT_INSTANT_ACCESS_TYPE */
    qlnoCompleteSortInstantAccess,      /**< QLL_PLAN_NODE_SORT_INSTANT_ACCESS_TYPE */
    NULL,      /**< QLL_PLAN_NODE_HASH_INSTANT_ACCESS_TYPE */
    qlnoCompleteGroupHashInstantAccess, /**< QLL_PLAN_NODE_GROUP_HASH_INSTANT_ACCESS_TYPE */
    NULL, /**< QLL_PLAN_NODE_GROUP_SORT_INSTANT_ACCESS_TYPE */
    qlnoCompleteSortJoinInstantAccess,  /**< QLL_PLAN_NODE_SORT_JOIN_INSTANT_ACCESS_TYPE */
    qlnoCompleteHashJoinInstantAccess   /**< QLL_PLAN_NODE_HASH_JOIN_INSTANT_ACCESS_TYPE */
};


const qlnoAddExprNodeFuncPtr qlnoAddExprNodeFuncList[ QLL_PLAN_NODE_TYPE_MAX ] =
{
    qlnoAddExprSelectStmt,     /**< QLL_PLAN_NODE_STMT_SELECT_TYPE */
    qlnoAddExprInsertStmt,     /**< QLL_PLAN_NODE_STMT_INSERT_TYPE */
    qlnoAddExprUpdateStmt,     /**< QLL_PLAN_NODE_STMT_UPDATE_TYPE */
    qlnoAddExprDeleteStmt,     /**< QLL_PLAN_NODE_STMT_DELETE_TYPE */
    NULL,     /**< QLL_PLAN_NODE_STMT_MERGE_TYPE */

    qlnoAddExprQuerySpec,     /**< QLL_PLAN_NODE_QUERY_SPEC_TYPE */
    NULL,     /**< QLL_PLAN_NODE_INDEX_BUILD_TYPE */
    NULL,     /**< QLL_PLAN_NODE_FOR_UPDATE_TYPE */
    NULL,     /**< QLL_PLAN_NODE_MERGE_TYPE */
    NULL,     /**< QLL_PLAN_NODE_VIEW_TYPE */
    NULL,     /**< QLL_PLAN_NODE_VIEW_PUSHED_PRED_TYPE */
    NULL,     /**< QLL_PLAN_NODE_FILTER_TYPE */
    NULL,     /**< QLL_PLAN_NODE_INLIST_TYPE */

    qlnoAddExprQuerySet,  /**< QLL_PLAN_NODE_QUERY_SET_TYPE */
    qlnoAddExprSetOP,     /**< QLL_PLAN_NODE_UNION_ALL_TYPE */
    qlnoAddExprSetOP,     /**< QLL_PLAN_NODE_UNION_DISTINCT_TYPE */
    qlnoAddExprSetOP,     /**< QLL_PLAN_NODE_EXCEPT_ALL_TYPE */
    qlnoAddExprSetOP,     /**< QLL_PLAN_NODE_EXCEPT_DISTINCT_TYPE */
    qlnoAddExprSetOP,     /**< QLL_PLAN_NODE_INTERSECT_ALL_TYPE */
    qlnoAddExprSetOP,     /**< QLL_PLAN_NODE_INTERSECT_DISTINCT_TYPE */

    qlnoAddExprSubQuery,            /**< QLL_PLAN_NODE_SUB_QUERY_TYPE */
    qlnoAddExprSubQueryList,        /**< QLL_PLAN_NODE_SUB_QUERY_LIST_TYPE */
    qlnoAddExprSubQueryFunc,        /**< QLL_PLAN_NODE_SUB_QUERY_FUNCTION_TYPE */
    qlnoAddExprSubQueryFilter,      /**< QLL_PLAN_NODE_SUB_QUERY_FILTER_TYPE */

    qlnoAddExprConcat,              /**< QLL_PLAN_NODE_CONCATENATION_TYPE */

    qlnoAddExprNestedLoopsJoin,     /**< QLL_PLAN_NODE_NESTED_LOOPS_TYPE */
    qlnoAddExprSortMergeJoin,       /**< QLL_PLAN_NODE_MERGE_JOIN_TYPE */
    qlnoAddExprHashJoin,            /**< QLL_PLAN_NODE_HASH_JOIN_TYPE */
    NULL,     /**< QLL_PLAN_NODE_INDEX_JOIN_TYPE */
    
    NULL,     /**< QLL_PLAN_NODE_CONNECT_BY_TYPE */
    NULL,     /**< QLL_PLAN_NODE_PSEUDO_COLUMN_TYPE */
    NULL,     /**< QLL_PLAN_NODE_COUNT_TYPE */
    NULL,     /**< QLL_PLAN_NODE_WINDOW_TYPE */
    NULL,     /**< QLL_PLAN_NODE_HASH_TYPE */
    qlnoAddExprHashAggregation,     /**< QLL_PLAN_NODE_HASH_AGGREGATION_TYPE */
    qlnoAddExprGroup,               /**< QLL_PLAN_NODE_GROUP_TYPE */

    qlnoAddExprTableAccess,     /**< QLL_PLAN_NODE_TABLE_ACCESS_TYPE */
    qlnoAddExprIndexAccess,     /**< QLL_PLAN_NODE_INDEX_ACCESS_TYPE */
    qlnoAddExprRowIdAccess,     /**< QLL_PLAN_NODE_ROWID_ACCESS_TYPE */
    NULL,     /**< QLL_PLAN_NODE_FAST_DUAL_ACCESS_TYPE */
    NULL,      /**< QLL_PLAN_NODE_FLAT_INSTANT_ACCESS_TYPE */
    qlnoAddExprSortInstantAccess,      /**< QLL_PLAN_NODE_SORT_INSTANT_ACCESS_TYPE */
    NULL,      /**< QLL_PLAN_NODE_HASH_INSTANT_ACCESS_TYPE */
    qlnoAddExprGroupHashInstantAccess, /**< QLL_PLAN_NODE_GROUP_HASH_INSTANT_ACCESS_TYPE */
    NULL, /**< QLL_PLAN_NODE_GROUP_SORT_INSTANT_ACCESS_TYPE */
    qlnoAddExprSortJoinInstantAccess,  /**< QLL_PLAN_NODE_SORT_JOIN_INSTANT_ACCESS_TYPE */
    qlnoAddExprHashJoinInstantAccess   /**< QLL_PLAN_NODE_HASH_JOIN_INSTANT_ACCESS_TYPE */
};


/** @} qlno */
