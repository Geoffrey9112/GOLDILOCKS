/*******************************************************************************
 * qlrAlterTableRenameColumn.h
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

#ifndef _QLR_ALTER_TABLE_RENAME_COLUMN_H_
#define _QLR_ALTER_TABLE_RENAME_COLUMN_H_ 1

/**
 * @file qlrAlterTableRenameColumn.h
 * @brief ALTER TABLE .. RENAME COLUMN
 */

/**
 * @brief RENAME COLUMN 을 위한 Code Plan
 */
typedef struct qlrInitRenameColumn
{
    qlvInitPlan      mCommonInit;       /**< Common Init Plan */
    
    ellDictHandle      mSchemaHandle;     /**< Schema Handle */
    ellDictHandle      mTableHandle;      /**< Table Handle */
    ellDictHandle      mColumnHandle;     /**< Column Handle */

    stlChar          * mNewColumnName;    /**< New Column Name */
} qlrInitRenameColumn;




stlStatus qlrValidateRenameColumn( smlTransId      aTransID,
                                   qllDBCStmt    * aDBCStmt,
                                   qllStatement  * aSQLStmt,
                                   stlChar       * aSQLString,
                                   qllNode       * aParseTree,
                                   qllEnv        * aEnv );

stlStatus qlrOptCodeRenameColumn( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  qllEnv        * aEnv );

stlStatus qlrOptDataRenameColumn( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  qllEnv        * aEnv );

stlStatus qlrExecuteRenameColumn( smlTransId      aTransID,
                                  smlStatement  * aStmt,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  qllEnv        * aEnv );

#endif /* _QLR_ALTER_TABLE_RENAME_COLUMN_H_ */

