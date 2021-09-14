/*******************************************************************************
 * ellDictKeyColumnUsage.h
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


#ifndef _ELL_DICT_KEY_COLUMN_USAGE_H_
#define _ELL_DICT_KEY_COLUMN_USAGE_H_ 1

/**
 * @file ellDictKeyColumnUsage.h
 * @brief Key Column Usage Dictionary Management
 */

/**
 * @addtogroup ellObjectConstraint
 * @{
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
                                       ellEnv         * aEnv );


/** @} ellObjectConstraint */



#endif /* _ELL_DICT_KEY_COLUMN_USAGE_H_ */
