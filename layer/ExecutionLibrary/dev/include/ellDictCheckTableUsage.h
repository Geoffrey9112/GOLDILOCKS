/*******************************************************************************
 * ellDictCheckTableUsage.h
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


#ifndef _ELL_DICT_CHECK_TABLE_USAGE_H_
#define _ELL_DICT_CHECK_TABLE_USAGE_H_ 1

/**
 * @file ellDictCheckTableUsage.h
 * @brief Constraint Dictionary Management
 */

/**
 * @addtogroup ellObjectConstraint
 * @{
 */

stlStatus ellInsertCheckTableUsageDesc( smlTransId       aTransID,
                                        smlStatement   * aStmt,
                                        stlInt64         aCheckOwnerID,
                                        stlInt64         aCheckSchemaID,
                                        stlInt64         aCheckConstID,
                                        stlInt64         aRefOwnerID,
                                        stlInt64         aRefSchemaID,
                                        stlInt64         aRefTableID,
                                        ellEnv         * aEnv );


/** @} ellObjectConstraint */



#endif /* _ELL_DICT_CHECK_TABLE_USAGE_H_ */
