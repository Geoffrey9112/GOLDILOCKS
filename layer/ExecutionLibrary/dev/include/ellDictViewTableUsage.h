/*******************************************************************************
 * ellDictViewTableUsage.h
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


#ifndef _ELL_DICT_VIEW_TABLE_USAGE_H_
#define _ELL_DICT_VIEW_TABLE_USAGE_H_ 1

/**
 * @file ellDictViewTableUsage.h
 * @brief View Table Usage Dictionary Management
 */

/**
 * @addtogroup ellObjectView
 * @{
 */

stlStatus ellInsertViewTableUsageDesc( smlTransId       aTransID,
                                       smlStatement   * aStmt,
                                       stlInt64         aViewOwnerID,
                                       stlInt64         aViewSchemaID,
                                       stlInt64         aViewID,
                                       stlInt64         aTableOwnerID,
                                       stlInt64         aTableSchemaID,
                                       stlInt64         aTableID,
                                       ellEnv         * aEnv );


/** @} ellObjectView */



#endif /* _ELL_DICT_VIEW_TABLE_USAGE_H_ */
