/*******************************************************************************
 * qlrAlterTableAddSuppLog.h
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

#ifndef _QLR_ALTER_TABLE_ADD_SUPPLEMENTAL_LOG_H_
#define _QLR_ALTER_TABLE_ADD_SUPPLEMENTAL_LOG_H_ 1

/**
 * @file qlrAlterTableAddSuppLog.h
 * @brief ALTER TABLE .. ADD SUPPLEMENTAL LOG
 */

/**
 * @brief ADD SUPPLEMENTAL LOG 을 위한 Code Plan
 */
typedef struct qlrInitAddSuppLog
{
    qlvInitPlan      mCommonInit;       /**< Common Init Plan */
    
    ellDictHandle      mSchemaHandle;     /**< Schema Handle */
    ellDictHandle      mTableHandle;      /**< Table Handle */
} qlrInitAddSuppLog;




stlStatus qlrValidateAddSuppLog( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 stlChar       * aSQLString,
                                 qllNode       * aParseTree,
                                 qllEnv        * aEnv );

stlStatus qlrOptCodeAddSuppLog( smlTransId      aTransID,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv );

stlStatus qlrOptDataAddSuppLog( smlTransId      aTransID,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv );

stlStatus qlrExecuteAddSuppLog( smlTransId      aTransID,
                                smlStatement  * aStmt,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv );


#endif /* _QLR_ALTER_TABLE_ADD_SUPPLEMENTAL_LOG_H_ */

