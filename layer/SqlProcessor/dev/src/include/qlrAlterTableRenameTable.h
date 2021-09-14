/*******************************************************************************
 * qlrAlterTableRenameTable.h
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

#ifndef _QLR_ALTER_TABLE_RENAME_TABLE_H_
#define _QLR_ALTER_TABLE_RENAME_TABLE_H_ 1

/**
 * @file qlrAlterTableRenameTable.h
 * @brief ALTER TABLE .. RENAME TO ..
 */

/**
 * @brief RENAME TABLE 을 위한 Code Plan
 */
typedef struct qlrInitRenameTable
{
    qlvInitPlan      mCommonInit;       /**< Common Init Plan */
    
    ellDictHandle      mSchemaHandle;     /**< Schema Handle */
    ellDictHandle      mTableHandle;      /**< Table Handle */

    stlChar          * mNewTableName;     /**< New Table Name */
} qlrInitRenameTable;




stlStatus qlrValidateRenameTable( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  stlChar       * aSQLString,
                                  qllNode       * aParseTree,
                                  qllEnv        * aEnv );

stlStatus qlrOptCodeRenameTable( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv );

stlStatus qlrOptDataRenameTable( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv );

stlStatus qlrExecuteRenameTable( smlTransId      aTransID,
                                 smlStatement  * aStmt,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv );

stlStatus qlrGetAlterTableHandle( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  stlChar       * aSQLString,
                                  ellDictHandle * aAuthHandle,
                                  qlpObjectName * aTableName,
                                  ellDictHandle * aSchemaHandle,
                                  ellDictHandle * aTableHandle,
                                  qllEnv        * aEnv );

#endif /* _QLR_ALTER_TABLE_RENAME_TABLE_H_ */

