/*******************************************************************************
 * ellDictReferentialConstraint.c
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
 * @file ellDictReferentialConstraint.c
 * @brief Referential Constraint Dictionary 관리 모듈
 */

#include <ell.h>
#include <elDef.h>

#include <eldt.h>
#include <eldc.h>
#include <eldDictionary.h>

#include <eldReferentialConstraint.h>



/**
 * @addtogroup ellObjectConstraint
 * @{
 */

/**
 * @brief Referential Match Option 의 String 상수 
 */
const stlChar * const gEllReferentialMatchOptionString[ELL_REFERENTIAL_MATCH_OPTION_MAX] =
{
    "N/A",      /**< ELL_REFERENTIAL_MATCH_OPTION_NOT_AVAILABLE */
    
    "SIMPLE",   /**< ELL_REFERENTIAL_MATCH_OPTION_SIMPLE */
    "PARTIAL",  /**< ELL_REFERENTIAL_MATCH_OPTION_PARTIAL */
    "FULL"      /**< ELL_REFERENTIAL_MATCH_OPTION_FULL */
};

/**
 * @brief Referential Action Rule 의 String 상수 
 */
const stlChar * const gEllReferentialActionRuleString[ELL_REFERENTIAL_ACTION_RULE_MAX] =
{
    "N/A",          /**< ELL_REFERENTIAL_ACTION_RULE_NOT_AVAILABLE */
    
    "CASCADE",      /**< ELL_REFERENTIAL_ACTION_RULE_CASCADE */
    "SET NULL",     /**< ELL_REFERENTIAL_ACTION_RULE_SET_NULL */
    "SET DEFAULT",  /**< ELL_REFERENTIAL_ACTION_RULE_SET_DEFAULT */
    "RESTRICT",     /**< ELL_REFERENTIAL_ACTION_RULE_RESTRICT */
    "NO ACTION"     /**< ELL_REFERENTIAL_ACTION_RULE_NO_ACTION */
}; 

/**
 * @brief Referential Constraint Descriptor 를 Dictionary 에 추가한다.
 * @param[in]  aTransID             Transaction ID
 * @param[in]  aStmt                Statement
 * @param[in]  aRefConstOwnerID     참조 제약조건의 Schema ID
 * @param[in]  aRefConstSchemaID    참조 제약조건의 Schema ID
 * @param[in]  aRefConstID          참조 제약조건의 ID
 * @param[in]  aUniqueConstOwnerID  참조 제약조건이 참조하는 Unique 의 Owner ID
 * @param[in]  aUniqueConstSchemaID 참조 제약조건이 참조하는 Unique 의 Schema ID
 * @param[in]  aUniqueConstID       참조 제약조건이 참조하는 Unique Constraint ID
 * @param[in]  aMatchOption         참조 제약조건의 Match Option
 *                 <BR> - 사용자 입력값이 없다면, ELL_REFERENTIAL_MATCH_OPTION_NOT_AVAILABLE
 * @param[in]  aUpdateRule          참조 제약조건의 Update Action Rule
 *                 <BR> - 사용자 입력값이 없다면, ELL_REFERENTIAL_ACTION_RULE_NOT_AVAILABLE
 * @param[in]  aDeleteRule          참조 제약조건의 Delete Action Rule
 *                 <BR> - 사용자 입력값이 없다면, ELL_REFERENTIAL_ACTION_RULE_NOT_AVAILABLE
 * @param[in]  aEnv                 Execution Library Environment
 * @remarks
 */

stlStatus ellInsertRefConstraintDesc( smlTransId                aTransID,
                                      smlStatement            * aStmt,
                                      stlInt64                  aRefConstOwnerID,
                                      stlInt64                  aRefConstSchemaID,
                                      stlInt64                  aRefConstID,
                                      stlInt64                  aUniqueConstOwnerID,
                                      stlInt64                  aUniqueConstSchemaID,
                                      stlInt64                  aUniqueConstID,
                                      ellReferentialMatchOption aMatchOption,
                                      ellReferentialActionRule  aUpdateRule,
                                      ellReferentialActionRule  aDeleteRule,
                                      ellEnv                  * aEnv )
{
    STL_TRY( eldInsertRefConstraintDesc( aTransID,
                                         aStmt,
                                         aRefConstOwnerID,
                                         aRefConstSchemaID,
                                         aRefConstID,
                                         aUniqueConstOwnerID,
                                         aUniqueConstSchemaID,
                                         aUniqueConstID,
                                         aMatchOption,
                                         aUpdateRule,
                                         aDeleteRule,
                                         aEnv )
             == STL_SUCCESS );

    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/** @} ellObjectConstraint */
