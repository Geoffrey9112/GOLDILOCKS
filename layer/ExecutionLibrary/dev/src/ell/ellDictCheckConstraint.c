/*******************************************************************************
 * ellDictCheckConstraint.c
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
 * @file ellDictCheckConstraint.c
 * @brief Check Constraint Dictionary 관리 모듈
 */

#include <ell.h>
#include <elDef.h>

#include <eldt.h>
#include <eldc.h>
#include <eldDictionary.h>

#include <eldCheckConstraint.h>


/**
 * @addtogroup ellObjectConstraint
 * @{
 */

/**
 * @brief Check Constraint Descriptor 를 Dictionary 에 추가한다.
 * @param[in]  aTransID             Transaction ID
 * @param[in]  aStmt                Statement
 * @param[in]  aCheckOwnerID        Check Constraint 의 Owner ID
 * @param[in]  aCheckSchemaID       Check Constraint 가 속한 Schema ID
 * @param[in]  aCheckConstID        Check Constraint 의 ID
 * @param[in]  aCheckClause         Check Constraint 의 검사 조건 구문
 *                             <BR> - NOT NULL 제약 조건이라면, null 을 설정 
 * @param[in]  aEnv                 Execution Library Environment
 * @remarks
 */

stlStatus ellInsertCheckConstraintDesc( smlTransId       aTransID,
                                        smlStatement   * aStmt,
                                        stlInt64         aCheckOwnerID,
                                        stlInt64         aCheckSchemaID,
                                        stlInt64         aCheckConstID,
                                        stlChar        * aCheckClause,
                                        ellEnv         * aEnv )
{
    STL_TRY( eldInsertCheckConstraintDesc( aTransID,
                                           aStmt,
                                           aCheckOwnerID,
                                           aCheckSchemaID,
                                           aCheckConstID,
                                           aCheckClause,
                                           aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/** @} ellObjectConstraint */
