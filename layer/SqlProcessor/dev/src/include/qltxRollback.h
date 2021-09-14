/*******************************************************************************
 * qltxRollback.h
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

#ifndef _QLTX_ROLLBACK_H_
#define _QLTX_ROLLBACK_H_ 1

/**
 * @file qltxRollback.h
 * @brief Transcation Control Language
 */


/**
 * @brief ROLLBACK Init Plan
 */
typedef struct qltxInitRollback
{
    stlChar  * mXidString;
    stlChar  * mComment;
    stlChar  * mSavepoint;             /**< Savepoint Name */
    stlInt64   mSavepointTimestamp;    /**< Savepoint Timestamp */
} qltxInitRollback;



stlStatus qltxValidateRollback( smlTransId      aTransID,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                stlChar       * aSQLString,
                                qllNode       * aParseTree,
                                qllEnv        * aEnv );

stlStatus qltxOptCodeRollback( smlTransId      aTransID,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv );

stlStatus qltxOptDataRollback( smlTransId      aTransID,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv );

stlStatus qltxExecuteRollback( smlTransId      aTransID,
                               smlStatement  * aStmt,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv );

#endif /* _QLTX_ROLLBACK_H_ */

