/*******************************************************************************
 * qltxSavepoint.h
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

#ifndef _QLTX_SAVEPOINT_H_
#define _QLTX_SAVEPOINT_H_ 1

/**
 * @file qltxSavepoint.h
 * @brief Transcation Control Language
 */

/**
 * @brief SAVEPOINT Init Plan
 */
typedef struct qltxInitSavepoint
{
    stlChar * mSavepoint;     /**< Savepoint Name */
} qltxInitSavepoint;



stlStatus qltxValidateSavepoint( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 stlChar       * aSQLString,
                                 qllNode       * aParseTree,
                                 qllEnv        * aEnv );

stlStatus qltxOptCodeSavepoint( smlTransId      aTransID,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv );

stlStatus qltxOptDataSavepoint( smlTransId      aTransID,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv );

stlStatus qltxExecuteSavepoint( smlTransId      aTransID,
                                smlStatement  * aStmt,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv );

#endif /* _QLTX_SAVEPOINT_H_ */

