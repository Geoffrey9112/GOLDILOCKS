/*******************************************************************************
 * ellDictIndexKeyTableUsage.h
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


#ifndef _ELL_DICT_INDEX_KEY_TABLE_USAGE_H_
#define _ELL_DICT_INDEX_KEY_TABLE_USAGE_H_ 1

/**
 * @file ellDictIndexKeyTableUsage.h
 * @brief Index Key Table Usage Dictionary Management
 */

/**
 * @addtogroup ellObjectIndex
 * @{
 */

stlStatus ellInsertIndexKeyTableUsageDesc( smlTransId       aTransID,
                                           smlStatement   * aStmt,
                                           stlInt64         aIndexOwnerID,
                                           stlInt64         aIndexSchemaID,
                                           stlInt64         aIndexID,
                                           stlInt64         aRefOwnerID,
                                           stlInt64         aRefSchemaID,
                                           stlInt64         aRefTableID,
                                           ellEnv         * aEnv );


/** @} ellObjectIndex */



#endif /* _ELL_DICT_INDEX_KEY_TABLE_USAGE_H_ */
