/*******************************************************************************
 * qlrAlterTableDropSuppLog.h
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

#ifndef _QLR_ALTER_TABLE_DROP_SUPPLEMENTAL_LOG_H_
#define _QLR_ALTER_TABLE_DROP_SUPPLEMENTAL_LOG_H_ 1

/**
 * @file qlrAlterTableDropSuppLog.h
 * @brief ALTER TABLE .. ADD SUPPLEMENTAL LOG
 */

/**
 * @brief ADD SUPPLEMENTAL LOG 을 위한 Code Plan
 */
typedef struct qlrInitDropSuppLog
{
    qlvInitPlan      mCommonInit;       /**< Common Init Plan */
    
    ellDictHandle      mSchemaHandle;     /**< Schema Handle */
    ellDictHandle      mTableHandle;      /**< Table Handle */
} qlrInitDropSuppLog;




stlStatus qlrValidateDropSuppLog( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  stlChar       * aSQLString,
                                  qllNode       * aParseTree,
                                  qllEnv        * aEnv );

stlStatus qlrOptCodeDropSuppLog( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv );

stlStatus qlrOptDataDropSuppLog( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv );

stlStatus qlrExecuteDropSuppLog( smlTransId      aTransID,
                                 smlStatement  * aStmt,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv );

#endif /* _QLR_ALTER_TABLE_DROP_SUPPLEMENTAL_LOG_H_ */

