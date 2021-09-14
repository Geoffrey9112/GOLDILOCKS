/*******************************************************************************
 * qldAlterDatabaseRestoreTablespace.h
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

#ifndef _QLD_ALTER_DATABASE_RESTORE_TABLESPACE_STMT_H_
#define _QLD_ALTER_DATABASE_RESTORE_TABLESPACE_STMT_H_ 1

/**
 * @file qldAlterDatabaseRestoreTablespace.h
 * @brief ALTER DATABASE RESTORE TABLESPACE Statement
 */

/**
 * @brief ALTER DATABASE REOCVER TABLESPACE ¸¦ À§ÇÑ Init Plan
 */
typedef struct qldInitAlterDatabaseRestoreTablespace
{
    qlvInitPlan      mCommonInit;       /**< Common Init Plan */
    
    ellDictHandle   mSpaceHandle;      /**< Tablespace Handle */
} qldInitAlterDatabaseRestoreTablespace;

stlStatus qldValidateAlterDatabaseRestoreTablespace( smlTransId      aTransID,
                                                     qllDBCStmt    * aDBCStmt,
                                                     qllStatement  * aSQLStmt,
                                                     stlChar       * aSQLString,
                                                     qllNode       * aParseTree,
                                                     qllEnv        * aEnv );

stlStatus qldOptCodeAlterDatabaseRestoreTablespace( smlTransId      aTransID,
                                                    qllDBCStmt    * aDBCStmt,
                                                    qllStatement  * aSQLStmt,
                                                    qllEnv        * aEnv );

stlStatus qldOptDataAlterDatabaseRestoreTablespace( smlTransId      aTransID,
                                                    qllDBCStmt    * aDBCStmt,
                                                    qllStatement  * aSQLStmt,
                                                    qllEnv        * aEnv );

stlStatus qldExecuteAlterDatabaseRestoreTablespace( smlTransId      aTransID,
                                                    smlStatement  * aStmt,
                                                    qllDBCStmt    * aDBCStmt,
                                                    qllStatement  * aSQLStmt,
                                                    qllEnv        * aEnv );


#endif /* _QLD_ALTER_DATABASE_RESTORE_TABLESPACE_STMT_H_ */


