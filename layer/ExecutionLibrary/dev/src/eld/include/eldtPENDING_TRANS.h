/*******************************************************************************
 * eldtPENDING_TRANS.h
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


#ifndef _ELDT_PENDING_TRANS_H_
#define _ELDT_PENDING_TRANS_H_ 1

/**
 * @file eldtPENDING_TRANS.h
 * @brief PENDING_TRANS dictionary base table
 */

/**
 * @defgroup eldtPENDING_TRANS PENDING_TRANS table
 * @ingroup eldtNonStandard
 * @{
 */

/**
 * @brief PENDING_TRANS 테이블의 컬럼 순서
 */
typedef enum eldtColumnOrderPENDING_TRANS
{
    /*
     * 식별자 영역
     */

    ELDT_PendingTrans_ColumnOrder_GLOBAL_TRANS_ID = 0,
    ELDT_PendingTrans_ColumnOrder_LOCAL_TRANS_ID,
    ELDT_PendingTrans_ColumnOrder_HEURISTIC_DECISION,
    ELDT_PendingTrans_ColumnOrder_FAIL_TIME,
    ELDT_PendingTrans_ColumnOrder_COMMENTS,
    
    ELDT_PendingTrans_ColumnOrder_MAX
    
} eldtColumnOrderPENDING_TRANS;
    

extern eldtDefinitionTableDesc   gEldtTableDescPENDING_TRANS;
extern eldtDefinitionColumnDesc  gEldtColumnDescPENDING_TRANS[ELDT_PendingTrans_ColumnOrder_MAX];


/**
 * @brief PENDING_TRANS 테이블의 KEY 제약 조건
 * - Key 제약 조건이 없어야 함.
 */

/**
 * @brief PENDING_TRANS 테이블의 부가적 INDEX
 * - Index 가 없어야 함.
 */



/** @} eldtPENDING_TRANS */



#endif /* _ELDT_PENDING_TRANS_H_ */
