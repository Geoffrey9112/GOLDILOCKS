/*******************************************************************************
 * qltxLockTable.h
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

#ifndef _QLTX_LOCK_TABLE_H_
#define _QLTX_LOCK_TABLE_H_ 1

/**
 * @file qltxLockTable.h
 * @brief Transcation Control Language
 */


/**
 * @brief LOCK TABLE Init Plan
 */
typedef struct qltxInitLockTable
{
    ellObjectList * mObjectList; /**< table, view object handle list */
    qlpLockMode     mMode;       /**< lock mode */
    stlInt64        mWaitTime;   /**< wait second */
} qltxInitLockTable;



/*********************************************************************
 * LOCK TABLE
 *********************************************************************/

stlStatus qltxValidateLockTable( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 stlChar       * aSQLString,
                                 qllNode       * aParseTree,
                                 qllEnv        * aEnv );

stlStatus qltxOptCodeLockTable( smlTransId      aTransID,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv );

stlStatus qltxOptDataLockTable( smlTransId      aTransID,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv );

stlStatus qltxExecuteLockTable( smlTransId      aTransID,
                                smlStatement  * aStmt,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv );

#endif /* _QLTX_LOCK_TABLE_H_ */

