/*******************************************************************************
 * qltxCommit.h
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

#ifndef _QLTX_COMMIT_H_
#define _QLTX_COMMIT_H_ 1

/**
 * @file qltxCommit.h
 * @brief Transcation Control Language
 */


/**
 * @brief COMMIT Init Plan
 */
typedef struct qltxInitCommit
{
    stlChar  * mXidString;
    stlChar  * mComment;
    stlInt32   mWriteMode;
} qltxInitCommit;


stlStatus qltxValidateCommit( smlTransId      aTransID,
                              qllDBCStmt    * aDBCStmt,
                              qllStatement  * aSQLStmt,
                              stlChar       * aSQLString,
                              qllNode       * aParseTree,
                              qllEnv        * aEnv );

stlStatus qltxOptCodeCommit( smlTransId      aTransID,
                             qllDBCStmt    * aDBCStmt,
                             qllStatement  * aSQLStmt,
                             qllEnv        * aEnv );

stlStatus qltxOptDataCommit( smlTransId      aTransID,
                             qllDBCStmt    * aDBCStmt,
                             qllStatement  * aSQLStmt,
                             qllEnv        * aEnv );

stlStatus qltxExecuteCommit( smlTransId      aTransID,
                             smlStatement  * aStmt,
                             qllDBCStmt    * aDBCStmt,
                             qllStatement  * aSQLStmt,
                             qllEnv        * aEnv );

#endif /* _QLTX_COMMIT_H_ */

