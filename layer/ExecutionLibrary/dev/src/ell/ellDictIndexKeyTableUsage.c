/*******************************************************************************
 * ellDictIndexKeyTableUsage.c
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
 * @file ellDictIndexKeyTableUsage.c
 * @brief Index Key Table Usage Dictionary 관리 모듈
 */

#include <ell.h>
#include <elDef.h>

#include <eldt.h>
#include <eldc.h>
#include <eldDictionary.h>

#include <eldIndexKeyTableUsage.h>


/**
 * @addtogroup ellObjectIndex
 * @{
 */

/**
 * @brief Index Key Column Usage Descriptor 를 Dictionary 에 추가한다.
 * @param[in]  aTransID                 Transaction ID
 * @param[in]  aStmt                    Statement
 * @param[in]  aIndexOwnerID            Index 의 Owner ID
 * @param[in]  aIndexSchemaID           Index 의 Schema ID
 * @param[in]  aIndexID                 Index 의 ID
 * @param[in]  aRefOwnerID              Index 에서 참조하는 Column 의 Owner ID
 * @param[in]  aRefSchemaID             Index 에서 참조하는 Column 의 Schema ID
 * @param[in]  aRefTableID              Index 에서 참조하는 Column 의 Table ID
 * @param[in]  aEnv                     Execution Library Environment
 * @remarks
 */

stlStatus ellInsertIndexKeyTableUsageDesc( smlTransId       aTransID,
                                           smlStatement   * aStmt,
                                           stlInt64         aIndexOwnerID,
                                           stlInt64         aIndexSchemaID,
                                           stlInt64         aIndexID,
                                           stlInt64         aRefOwnerID,
                                           stlInt64         aRefSchemaID,
                                           stlInt64         aRefTableID,
                                           ellEnv         * aEnv )
{
    STL_TRY( eldInsertIndexKeyTableUsageDesc( aTransID,
                                              aStmt,
                                              aIndexOwnerID,
                                              aIndexSchemaID,
                                              aIndexID,
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


/** @} ellObjectIndex */
