/*******************************************************************************
 * ellDictKeyColumnUsage.c
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
 * @file ellDictKeyColumnUsage.c
 * @brief Key Column Usage Dictionary 관리 모듈
 */

#include <ell.h>
#include <elDef.h>

#include <eldt.h>
#include <eldc.h>
#include <eldDictionary.h>

#include <eldKeyColumnUsage.h>


/**
 * @addtogroup ellObjectConstraint
 * @{
 */

/**
 * @brief Key Column Usage Descriptor 를 Dictionary 에 추가한다.
 * @param[in]  aTransID                 Transaction ID
 * @param[in]  aStmt                    Statement
 * @param[in]  aKeyOwnerID              Key Constraint 의 Owner ID
 * @param[in]  aKeySchemaID             Key Constraint 의 Schema ID
 * @param[in]  aKeyConstID              Key Constraint 의 ID
 * @param[in]  aRefOwnerID              Key 제약 조건에서 참조하는 Column 의 Owner ID
 * @param[in]  aRefSchemaID             Key 제약 조건에서 참조하는 Column 의 Schema ID
 * @param[in]  aRefTableID              Key 제약 조건에서 참조하는 Column 의 Table ID
 * @param[in]  aRefColumnID             Key 제약 조건에서 참조하는 Column ID
 * @param[in]  aKeyOrdinalPosition      Key 내에서의 Column 의 순서
 *                                 <BR> (0부터 시작, DB에는 1부터 저장됨)
 * @param[in]  aPositionInUniqueConst   참조하는 Unique Constraint에서의 컬럼의 순서
 *                                 <BR> (0부터 시작, DB에는 1부터 저장됨)
 *                                 <BR> Foreign Key 가 아니라면,
 *                                 <BR> - ELL_DICT_POSITION_NOT_AVAILABLE
 * @param[in]  aEnv                     Execution Library Environment
 * @remarks
 */

stlStatus ellInsertKeyColumnUsageDesc( smlTransId       aTransID,
                                       smlStatement   * aStmt,
                                       stlInt64         aKeyOwnerID,
                                       stlInt64         aKeySchemaID,
                                       stlInt64         aKeyConstID,
                                       stlInt64         aRefOwnerID,
                                       stlInt64         aRefSchemaID,
                                       stlInt64         aRefTableID,
                                       stlInt64         aRefColumnID,
                                       stlInt32         aKeyOrdinalPosition,
                                       stlInt32         aPositionInUniqueConst,
                                       ellEnv         * aEnv )
{
    STL_TRY( eldInsertKeyColumnUsageDesc( aTransID,
                                          aStmt,
                                          aKeyOwnerID,
                                          aKeySchemaID,
                                          aKeyConstID,
                                          aRefOwnerID,
                                          aRefSchemaID,
                                          aRefTableID,
                                          aRefColumnID,
                                          aKeyOrdinalPosition,
                                          aPositionInUniqueConst,
                                          aEnv )
             == STL_SUCCESS );

    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/** @} ellObjectConstraint */
