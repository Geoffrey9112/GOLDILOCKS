/*******************************************************************************
 * ellDictCheckColumnUsage.h
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


#ifndef _ELL_DICT_CHECK_COLUMN_USAGE_H_
#define _ELL_DICT_CHECK_COLUMN_USAGE_H_ 1

/**
 * @file ellDictCheckColumnUsage.h
 * @brief Constraint Dictionary Management
 */

/**
 * @addtogroup ellObjectConstraint
 * @{
 */

stlStatus ellInsertCheckColumnUsageDesc( smlTransId       aTransID,
                                         smlStatement   * aStmt,
                                         stlInt64         aCheckOwnerID,
                                         stlInt64         aCheckSchemaID,
                                         stlInt64         aCheckConstID,
                                         stlInt64         aRefOwnerID,
                                         stlInt64         aRefSchemaID,
                                         stlInt64         aRefTableID,
                                         stlInt64         aRefColumnID,
                                         ellEnv         * aEnv );


/** @} ellObjectConstraint */



#endif /* _ELL_DICT_CHECK_COLUMN_USAGE_H_ */
