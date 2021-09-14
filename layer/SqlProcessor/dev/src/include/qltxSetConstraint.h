/*******************************************************************************
 * qltxSetConstraint.h
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

#ifndef _QLTX_SET_CONSTRAINT_H_
#define _QLTX_SET_CONSTRAINT_H_ 1

/**
 * @file qltxSetConstraint.h
 * @brief Transcation Control Language
 */

/**
 * @brief SAVEPOINT Init Plan
 */
typedef struct qltxInitSetConstraint
{
    qlvInitPlan            mCommonInit;          /**< Common Init Plan */

    qllSetConstraintMode   mSetMode;             /**< SET CONSTRAINT mode */
    ellObjectList        * mConstraintList;      /**< Constraint List */
    
} qltxInitSetConstraint;



stlStatus qltxValidateSetConstraint( smlTransId      aTransID,
                                     qllDBCStmt    * aDBCStmt,
                                     qllStatement  * aSQLStmt,
                                     stlChar       * aSQLString,
                                     qllNode       * aParseTree,
                                     qllEnv        * aEnv );

stlStatus qltxOptCodeSetConstraint( smlTransId      aTransID,
                                    qllDBCStmt    * aDBCStmt,
                                    qllStatement  * aSQLStmt,
                                    qllEnv        * aEnv );

stlStatus qltxOptDataSetConstraint( smlTransId      aTransID,
                                    qllDBCStmt    * aDBCStmt,
                                    qllStatement  * aSQLStmt,
                                    qllEnv        * aEnv );

stlStatus qltxExecuteSetConstraint( smlTransId      aTransID,
                                    smlStatement  * aStmt,
                                    qllDBCStmt    * aDBCStmt,
                                    qllStatement  * aSQLStmt,
                                    qllEnv        * aEnv );

#endif /* _QLTX_SET_CONSTRAINT_H_ */

