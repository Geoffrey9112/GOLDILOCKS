/*******************************************************************************
 * ellDictReferentialConstraint.h
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


#ifndef _ELL_DICT_REFERENTIAL_CONSTRAINT_H_
#define _ELL_DICT_REFERENTIAL_CONSTRAINT_H_ 1

/**
 * @file ellDictReferentialConstraint.h
 * @brief Constraint Dictionary Management
 */

/**
 * @addtogroup ellObjectConstraint
 * @{
 */

stlStatus ellInsertRefConstraintDesc( smlTransId                aTransID,
                                      smlStatement            * aStmt,
                                      stlInt64                  aRefConstOwnerID,
                                      stlInt64                  aRefConstSchemaID,
                                      stlInt64                  aRefConstID,
                                      stlInt64                  aUniqueConstOwnerID,
                                      stlInt64                  aUniqueConstSchemaID,
                                      stlInt64                  aUniqueConstID,
                                      ellReferentialMatchOption aMatchOption,
                                      ellReferentialActionRule  aUpdateRule,
                                      ellReferentialActionRule  aDeleteRule,
                                      ellEnv                  * aEnv );


/** @} ellObjectConstraint */



#endif /* _ELL_DICT_REFERENTIAL_CONSTRAINT_H_ */
