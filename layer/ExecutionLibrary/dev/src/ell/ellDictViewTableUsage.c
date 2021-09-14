/*******************************************************************************
 * ellDictViewTableUsage.c
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
 * @file ellDictViewTableUsage.c
 * @brief View Column Usage Dictionary 관리 모듈
 */

#include <ell.h>
#include <elDef.h>

#include <eldt.h>
#include <eldc.h>
#include <eldDictionary.h>

#include <eldViewTableUsage.h>


/**
 * @addtogroup ellObjectView
 * @{
 */

/**
 * @brief View Table Usage Descriptor 를 Dictionary 에 추가한다.
 * @param[in]  aTransID                 Transaction ID
 * @param[in]  aStmt                    Statement
 * @param[in]  aViewOwnerID             View 의 Owner ID
 * @param[in]  aViewSchemaID            View 의 Schema ID
 * @param[in]  aViewID                  View ID
 * @param[in]  aTableOwnerID            View가 참조하는 Table 의 Owner ID
 * @param[in]  aTableSchemaID           View가 참조하는 Table 의 Schema ID
 * @param[in]  aTableID                 View가 참조하는 Table 의 Table ID
 * @param[in]  aEnv                     Execution Library Environment
 * @remarks
 */

stlStatus ellInsertViewTableUsageDesc( smlTransId       aTransID,
                                       smlStatement   * aStmt,
                                       stlInt64         aViewOwnerID,
                                       stlInt64         aViewSchemaID,
                                       stlInt64         aViewID,
                                       stlInt64         aTableOwnerID,
                                       stlInt64         aTableSchemaID,
                                       stlInt64         aTableID,
                                       ellEnv         * aEnv )
{
    STL_TRY( eldInsertViewTableUsageDesc( aTransID,
                                          aStmt,
                                          aViewOwnerID,
                                          aViewSchemaID,
                                          aViewID,
                                          aTableOwnerID,
                                          aTableSchemaID,
                                          aTableID,
                                          aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/** @} ellObjectView */
