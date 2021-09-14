/*******************************************************************************
 * ellDictCheckTableUsage.c
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
 * @file ellDictCheckTableUsage.c
 * @brief Check Table Usage Dictionary 관리 모듈
 */

#include <ell.h>
#include <elDef.h>

#include <eldt.h>
#include <eldc.h>
#include <eldDictionary.h>

#include <eldCheckTableUsage.h>


/**
 * @addtogroup ellObjectConstraint
 * @{
 */

/**
 * @brief Check Table Usage Descriptor 를 Dictionary 에 추가한다.
 * @param[in]  aTransID                 Transaction ID
 * @param[in]  aStmt                    Statement
 * @param[in]  aCheckOwnerID            Check Constraint 의 Owner ID
 * @param[in]  aCheckSchemaID           Check Constraint 가 속한 Schema ID
 * @param[in]  aCheckConstID            Check Constraint 의 ID
 * @param[in]  aRefOwnerID              Check 제약 조건에서 참조하는 Column 의 Owner ID
 * @param[in]  aRefSchemaID             Check 제약 조건에서 참조하는 Column 의 Schema ID
 * @param[in]  aRefTableID              Check 제약 조건에서 참조하는 Column 의 Table ID
 * @param[in]  aEnv                     Execution Library Environment
 * @remarks
 */

stlStatus ellInsertCheckTableUsageDesc( smlTransId       aTransID,
                                        smlStatement   * aStmt,
                                        stlInt64         aCheckOwnerID,
                                        stlInt64         aCheckSchemaID,
                                        stlInt64         aCheckConstID,
                                        stlInt64         aRefOwnerID,
                                        stlInt64         aRefSchemaID,
                                        stlInt64         aRefTableID,
                                        ellEnv         * aEnv )
{
    STL_TRY( eldInsertCheckTableUsageDesc( aTransID,
                                           aStmt,
                                           aCheckOwnerID,
                                           aCheckSchemaID,
                                           aCheckConstID,
                                           aRefOwnerID,
                                           aRefSchemaID,
                                           aRefTableID,
                                           aEnv )
             == STL_SUCCESS );

    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/** @} ellObjectConstraint */
