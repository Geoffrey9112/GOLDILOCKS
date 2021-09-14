/*******************************************************************************
 * ellDictCheckConstraint.h
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


#ifndef _ELL_DICT_CHECK_CONSTRAINT_H_
#define _ELL_DICT_CHECK_CONSTRAINT_H_ 1

/**
 * @file ellDictCheckConstraint.h
 * @brief Constraint Dictionary Management
 */

/**
 * @addtogroup ellObjectConstraint
 * @{
 */

stlStatus ellInsertCheckConstraintDesc( smlTransId       aTransID,
                                        smlStatement   * aStmt,
                                        stlInt64         aCheckOwnerID,
                                        stlInt64         aCheckSchemaID,
                                        stlInt64         aCheckConstID,
                                        stlChar        * aCheckClause,
                                        ellEnv         * aEnv );


/** @} ellObjectConstraint */



#endif /* _ELL_DICT_CHECK_CONSTRAINT_H_ */
