/*******************************************************************************
 * ellDictSqlImplementationInfo.c
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
 * @file ellDictSqlImplementationInfo.c
 * @brief SQL IMPLEMENTATION INFO Dictionary 관리 모듈
 */

#include <ell.h>
#include <elDef.h>

#include <eldt.h>
#include <eldc.h>
#include <eldDictionary.h>

#include <eldSqlImplementationInfo.h>


/**
 * @addtogroup ellSqlImplementationInfo
 * @{
 */





/**
 * @brief SQL-implementation information item 을 Dictionary 에 추가한다.
 * @param[in]  aTransID            Transaction ID
 * @param[in]  aStmt               Statement
 * @param[in]  aID                 SQL-implementation information item 의 ID
 * @param[in]  aName               SQL-implementation information item 의 이름
 * @param[in]  aIntegerValue       SQL-implementation information item 의 값
 * @param[in]  aCharacterValue     SQL-implementation information item 의 값
 * @param[in]  aComments           SQL-implementation information item 의 comment
 * @param[in]  aEnv                Execution Library Environment
 * @remarks
 */
stlStatus ellInsertSqlImplementationInfoDesc( smlTransId     aTransID,
                                              smlStatement * aStmt,
                                              stlChar      * aID,
                                              stlChar      * aName,
                                              stlInt32       aIntegerValue,
                                              stlChar      * aCharacterValue,
                                              stlChar      * aComments,
                                              ellEnv       * aEnv )
{
    STL_TRY( eldInsertSqlImplementationInfoDesc( aTransID,
                                                 aStmt,
                                                 aID,
                                                 aName,
                                                 aIntegerValue,
                                                 aCharacterValue,
                                                 aComments,
                                                 aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/** @} ellSqlImplementationInfo */
