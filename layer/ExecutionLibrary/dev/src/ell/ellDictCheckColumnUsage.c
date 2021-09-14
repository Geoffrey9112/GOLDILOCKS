/*******************************************************************************
 * ellDictCheckColumnUsage.c
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
 * @file ellDictCheckColumnUsage.c
 * @brief Check Column Usage Dictionary 관리 모듈
 */

#include <ell.h>
#include <elDef.h>

#include <eldt.h>
#include <eldc.h>
#include <eldDictionary.h>

#include <eldCheckColumnUsage.h>


/**
 * @addtogroup ellObjectConstraint
 * @{
 */

/**
 * @brief Check Column Usage Descriptor 를 Dictionary 에 추가한다.
 * @param[in]  aTransID                 Transaction ID
 * @param[in]  aStmt                    Statement
 * @param[in]  aCheckOwnerID            Check Constraint 의 Owner ID
 * @param[in]  aCheckSchemaID           Check Constraint 가 속한 Schema ID
 * @param[in]  aCheckConstID            Check Constraint 의 ID
 * @param[in]  aRefOwnerID              Check 제약 조건에서 참조하는 Column 의 Owner ID
 * @param[in]  aRefSchemaID             Check 제약 조건에서 참조하는 Column 의 Schema ID
 * @param[in]  aRefTableID              Check 제약 조건에서 참조하는 Column 의 Table ID
 * @param[in]  aRefColumnID             Check 제약 조건에서 참조하는 Column ID
 * @param[in]  aEnv                     Execution Library Environment
 * @remarks
 */

stlStatus ellInsertCheckColumnUsageDesc( smlTransId       aTransID,
                                         smlStatement   * aStmt,
                                         stlInt64         aCheckOwnerID,
                                         stlInt64         aCheckSchemaID,
                                         stlInt64         aCheckConstID,
                                         stlInt64         aRefOwnerID,
                                         stlInt64         aRefSchemaID,
                                         stlInt64         aRefTableID,
                                         stlInt64         aRefColumnID,
                                         ellEnv         * aEnv )
{
    STL_TRY( eldInsertCheckColumnUsageDesc( aTransID,
                                            aStmt,
                                            aCheckOwnerID,
                                            aCheckSchemaID,
                                            aCheckConstID,
                                            aRefOwnerID,
                                            aRefSchemaID,
                                            aRefTableID,
                                            aRefColumnID,
                                            aEnv )
             == STL_SUCCESS );

    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/** @} ellObjectConstraint */
