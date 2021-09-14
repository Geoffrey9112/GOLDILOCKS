/*******************************************************************************
 * qltxReleaseSavepoint.h
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

#ifndef _QLTX_RELEASE_SAVEPOINT_H_
#define _QLTX_RELEASE_SAVEPOINT_H_ 1

/**
 * @file qltxReleaseSavepoint.h
 * @brief Transcation Control Language
 */

/**
 * @brief RELEASE SAVEPOINT Init Plan
 */
typedef struct qltxInitReleaseSavepoint
{
    stlChar * mSavepoint;     /**< Savepoint Name */
} qltxInitReleaseSavepoint;



stlStatus qltxValidateReleaseSavepoint( smlTransId      aTransID,
                                        qllDBCStmt    * aDBCStmt,
                                        qllStatement  * aSQLStmt,
                                        stlChar       * aSQLString,
                                        qllNode       * aParseTree,
                                        qllEnv        * aEnv );

stlStatus qltxOptCodeReleaseSavepoint( smlTransId      aTransID,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       qllEnv        * aEnv );

stlStatus qltxOptDataReleaseSavepoint( smlTransId      aTransID,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       qllEnv        * aEnv );

stlStatus qltxExecuteReleaseSavepoint( smlTransId      aTransID,
                                       smlStatement  * aStmt,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       qllEnv        * aEnv );

#endif /* _QLTX_RELEASE_SAVEPOINT_H_ */

