/*******************************************************************************
 * ellDictSqlSizing.c
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
 * @file ellDictSqlSizing.c
 * @brief SQL SIZING Dictionary 관리 모듈
 */

#include <ell.h>
#include <elDef.h>

#include <eldt.h>
#include <eldc.h>
#include <eldDictionary.h>

#include <eldSqlSizing.h>


/**
 * @addtogroup ellSqlSizing
 * @{
 */





/**
 * @brief sizing item 을 Dictionary 에 추가한다.
 * @param[in]  aTransID            Transaction ID
 * @param[in]  aStmt               Statement
 * @param[in]  aID                 sizing item 의 ID
 * @param[in]  aName               sizing item 의 이름
 * @param[in]  aValue              sizing item 의 값
 * @param[in]  aComments           sizing item 의 comment
 * @param[in]  aEnv                Execution Library Environment
 * @remarks
 */
stlStatus ellInsertSqlSizingDesc( smlTransId     aTransID,
                                  smlStatement * aStmt,
                                  stlInt32       aID,
                                  stlChar      * aName,
                                  stlInt32       aValue,
                                  stlChar      * aComments,
                                  ellEnv       * aEnv )
{
    STL_TRY( eldInsertSqlSizingDesc( aTransID,
                                     aStmt,
                                     aID,
                                     aName,
                                     aValue,
                                     aComments,
                                     aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/** @} ellSqlSizing */
