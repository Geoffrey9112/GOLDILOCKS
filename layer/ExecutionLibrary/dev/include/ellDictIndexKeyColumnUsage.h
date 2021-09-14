/*******************************************************************************
 * ellDictIndexKeyColumnUsage.h
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


#ifndef _ELL_DICT_INDEX_KEY_COLUMN_USAGE_H_
#define _ELL_DICT_INDEX_KEY_COLUMN_USAGE_H_ 1

/**
 * @file ellDictIndexKeyColumnUsage.h
 * @brief Index Key Column Usage Dictionary Management
 */

/**
 * @addtogroup ellObjectIndex
 * @{
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
                                            ellEnv         * aEnv );


/** @} ellObjectIndex */



#endif /* _ELL_DICT_INDEX_KEY_COLUMN_USAGE_H_ */
