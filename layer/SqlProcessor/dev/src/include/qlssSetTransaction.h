/*******************************************************************************
 * qlssSetTransaction.h
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

#ifndef _QLSS_SET_TRANSACTION_H_
#define _QLSS_SET_TRANSACTION_H_ 1

/**
 * @file qlssSetTransaction.h
 * @brief Session Control Language
 */

/**
 * @brief SET [ TRANSACTION | SESSION CHARACTERISTICS ] Init Plan
 */
typedef struct qlssInitSetTransactionMode
{
    qlpTransactionAttr mMode;            /**< transacton mode */

    stlInt32           mAccessMode;      /**< access mode */
    smlIsolationLevel  mIsolationLevel;  /**< isolation level */
    qllUniqueMode      mUniqueMode;      /**< unique integrity check time */
    
} qlssInitSetTransactionMode;



stlStatus qlssValidateSetTransaction( smlTransId      aTransID,
                                      qllDBCStmt    * aDBCStmt,
                                      qllStatement  * aSQLStmt,
                                      stlChar       * aSQLString,
                                      qllNode       * aParseTree,
                                      qllEnv        * aEnv );

stlStatus qlssOptCodeSetTransaction( smlTransId      aTransID,
                                     qllDBCStmt    * aDBCStmt,
                                     qllStatement  * aSQLStmt,
                                     qllEnv        * aEnv );

stlStatus qlssOptDataSetTransaction( smlTransId      aTransID,
                                     qllDBCStmt    * aDBCStmt,
                                     qllStatement  * aSQLStmt,
                                     qllEnv        * aEnv );

stlStatus qlssExecuteSetTransaction( smlTransId      aTransID,
                                     smlStatement  * aStmt,
                                     qllDBCStmt    * aDBCStmt,
                                     qllStatement  * aSQLStmt,
                                     qllEnv        * aEnv );

#endif /* _QLSS_SET_TRANSACTION_H_ */

