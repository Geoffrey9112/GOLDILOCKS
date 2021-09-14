/*******************************************************************************
 * eldtPENDING_TRANS.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id:$
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file eldtPENDING_TRANS.c
 * @brief PENDING_TRANS dictionary table 
 */

#include <ell.h>

#include <eldt.h>

/**
 * @addtogroup eldtPENDING_TRANS
 * @internal
 * @{
 */

/**
 * @brief DEFINITION_SCHEMA.PENDING_TRANS 의 컬럼 정의
 */
eldtDefinitionColumnDesc  gEldtColumnDescPENDING_TRANS[ELDT_PendingTrans_ColumnOrder_MAX] =
{
    {
        ELDT_TABLE_ID_PENDING_TRANS,               /**< Table ID */
        "GLOBAL_TRANS_ID",                         /**< 컬럼의 이름  */
        ELDT_PendingTrans_ColumnOrder_GLOBAL_TRANS_ID, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_LONG_DESC,                     /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                         /**< 컬럼의 Nullable 여부 */
        "global transaction identifier ( xid )"
    },
    {
        ELDT_TABLE_ID_PENDING_TRANS,               /**< Table ID */
        "LOCAL_TRANS_ID",                          /**< 컬럼의 이름  */
        ELDT_PendingTrans_ColumnOrder_LOCAL_TRANS_ID, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                    /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                         /**< 컬럼의 Nullable 여부 */
        "local transaction identifier"
    },
    {
        ELDT_TABLE_ID_PENDING_TRANS,               /**< Table ID */
        "HEURISTIC_DECISION",                      /**< 컬럼의 이름  */
        ELDT_PendingTrans_ColumnOrder_HEURISTIC_DECISION,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                     /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                         /**< 컬럼의 Nullable 여부 */
        "heuristic decision: rolled back(false) or committed(true)"
    },
    {
        ELDT_TABLE_ID_PENDING_TRANS,               /**< Table ID */
        "FAIL_TIME",                               /**< 컬럼의 이름  */
        ELDT_PendingTrans_ColumnOrder_FAIL_TIME,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_TIME_STAMP,                    /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                         /**< 컬럼의 Nullable 여부 */
        "time when the row was inserted"
    },
    {
        ELDT_TABLE_ID_PENDING_TRANS,               /**< Table ID */
        "COMMENTS",                                /**< 컬럼의 이름  */
        ELDT_PendingTrans_ColumnOrder_COMMENTS,    /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_LONG_DESC,                     /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                         /**< 컬럼의 Nullable 여부 */
        "comments"
    }
    
};


/**
 * @brief DEFINITION_SCHEMA.PENDING_TRANS 의 테이블 정의
 */
eldtDefinitionTableDesc gEldtTableDescPENDING_TRANS =
{
    ELDT_TABLE_ID_PENDING_TRANS, /**< Table ID */
    "PENDING_TRANS",             /**< 테이블의 이름  */
    "pending transactions"
};

/** @} eldtPENDING_TRANS */
