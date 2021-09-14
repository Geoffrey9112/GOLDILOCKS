/*******************************************************************************
 * ellDictIndexKeyColumnUsage.c
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
 * @file ellDictIndexKeyColumnUsage.c
 * @brief Index Key Column Usage Dictionary 관리 모듈
 */

#include <ell.h>
#include <elDef.h>

#include <eldt.h>
#include <eldc.h>
#include <eldDictionary.h>

#include <eldIndexKeyColumnUsage.h>


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
 * @param[in]  aRefColumnID             Index 에서 참조하는 Column ID
 * @param[in]  aKeyOrdinalPosition      Key 내에서의 Column 의 순서
 *                                 <BR> (0부터 시작, DB에는 1부터 저장됨)
 * @param[in]  aColumnOrdering          Key Column 의 Ascending/Descending 여부
 *                                 <BR> 기본값은, ELL_INDEX_COLUMN_ORDERING_DEFAULT
 * @param[in]  aNullsOrdering           Key Column 의 Nulls First / Nulls Last 여부
 *                                 <BR> 기본값은, ELL_INDEX_COLUMN_NULLS_ORDERING_DEFAULT
 * @param[in]  aEnv                     Execution Library Environment
 * @remarks
 */

stlStatus ellInsertIndexKeyColumnUsageDesc( smlTransId       aTransID,
                                            smlStatement   * aStmt,
                                            stlInt64         aIndexOwnerID,
                                            stlInt64         aIndexSchemaID,
                                            stlInt64         aIndexID,
                                            stlInt64         aRefOwnerID,
                                            stlInt64         aRefSchemaID,
                                            stlInt64         aRefTableID,
                                            stlInt64         aRefColumnID,
                                            stlInt32         aKeyOrdinalPosition,
                                            ellIndexColumnOrdering      aColumnOrdering,
                                            ellIndexColumnNullsOrdering aNullsOrdering,
                                            ellEnv         * aEnv )
{
    STL_TRY( eldInsertIndexKeyColumnUsageDesc( aTransID,
                                               aStmt,
                                               aIndexOwnerID,
                                               aIndexSchemaID,
                                               aIndexID,
                                               aRefOwnerID,
                                               aRefSchemaID,
                                               aRefTableID,
                                               aRefColumnID,
                                               aKeyOrdinalPosition,
                                               aColumnOrdering,
                                               aNullsOrdering,
                                               aEnv )
             == STL_SUCCESS );

    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/** @} ellObjectIndex */
