/*******************************************************************************
 * ellDictSqlConformance.c
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
 * @file ellDictSqlConformance.c
 * @brief SQL CONFORMANCE Dictionary 관리 모듈
 */

#include <ell.h>
#include <elDef.h>

#include <eldt.h>
#include <eldc.h>
#include <eldDictionary.h>

#include <eldSqlConformance.h>


/**
 * @addtogroup ellSqlConformance
 * @{
 */





/**
 * @brief SQL-implementation information item 을 Dictionary 에 추가한다.
 * @param[in]  aTransID            Transaction ID
 * @param[in]  aStmt               Statement
 * @param[in]  aType               conformance element 의 Type
 * @param[in]  aID                 conformance element 의 ID
 * @param[in]  aSubID              conformance element 의 subfeature ID
 * @param[in]  aName               conformance element 의 Name
 * @param[in]  aSubName            conformance element 의 subfeature Name
 * @param[in]  aIsSupported        conformance element 의 지원여부
 * @param[in]  aIsMandatory        SQL conformance core feature 여부 
 * @param[in]  aIsVerifiedBy       conformance element 의 testing 여부
 * @param[in]  aComments           SQL-implementation information item 의 comment
 * @param[in]  aEnv                Execution Library Environment
 * @remarks
 */
stlStatus ellInsertSqlConformanceDesc( smlTransId     aTransID,
                                       smlStatement * aStmt,
                                       stlChar      * aType,
                                       stlChar      * aID,
                                       stlChar      * aSubID,
                                       stlChar      * aName,
                                       stlChar      * aSubName,
                                       stlBool        aIsSupported,
                                       stlBool        aIsMandatory,
                                       stlChar      * aIsVerifiedBy,
                                       stlChar      * aComments,
                                       ellEnv       * aEnv )
{
    STL_TRY( eldInsertSqlConformanceDesc( aTransID,
                                          aStmt,
                                          aType,
                                          aID,
                                          aSubID,
                                          aName,
                                          aSubName,
                                          aIsSupported,
                                          aIsMandatory,
                                          aIsVerifiedBy,
                                          aComments,
                                          aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/** @} ellSqlConformance */
