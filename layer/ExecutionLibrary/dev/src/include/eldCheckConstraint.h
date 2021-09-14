/*******************************************************************************
 * eldCheckConstraint.h
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


#ifndef _ELD_CHECK_CONSTRAINT_H_
#define _ELD_CHECK_CONSTRAINT_H_ 1

/**
 * @file eldCheckConstraint.h
 * @brief Check Constraint Dictionary 관리 루틴 
 */


/**
 * @defgroup eldCheckConstraint Check Constraint Descriptor 관리 루틴 
 * @internal 
 * @ingroup eldDesc
 * @{
 */

stlStatus eldInsertCheckConstraintDesc( smlTransId       aTransID,
                                        smlStatement   * aStmt,
                                        stlInt64         aCheckOwnerID,
                                        stlInt64         aCheckSchemaID,
                                        stlInt64         aCheckConstID,
                                        stlChar        * aCheckClause,
                                        ellEnv         * aEnv );


/** @} eldCheckConstraint */



#endif /* _ELD_CHECK_CONSTRAINT_H_ */
