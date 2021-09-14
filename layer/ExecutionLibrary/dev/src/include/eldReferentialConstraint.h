/*******************************************************************************
 * eldReferentialConstraint.h
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


#ifndef _ELD_REFERENTIAL_CONSTRAINT_H_
#define _ELD_REFERENTIAL_CONSTRAINT_H_ 1

/**
 * @file eldReferentialConstraint.h
 * @brief Referential Constraint Dictionary 관리 루틴 
 */


/**
 * @defgroup eldReferentialConstraint Referential Constraint Descriptor 관리 루틴 
 * @internal 
 * @ingroup eldDesc
 * @{
 */

stlStatus eldInsertRefConstraintDesc( smlTransId                aTransID,
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



#endif /* _ELD_REFERENTIAL_CONSTRAINT_H_ */
