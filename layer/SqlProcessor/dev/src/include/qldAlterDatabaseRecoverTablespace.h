/*******************************************************************************
 * qldAlterDatabaseRecoverTablespace.h
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

#ifndef _QLD_ALTER_DATABASE_RECOVER_TABLESPACE_STMT_H_
#define _QLD_ALTER_DATABASE_RECOVER_TABLESPACE_STMT_H_ 1

/**
 * @file qldAlterDatabaseRecoverTablespace.h
 * @brief ALTER DATABASE RECOVER TABLESPACE Statement
 */

/**
 * @brief ALTER DATABASE REOCVER TABLESPACE ¸¦ À§ÇÑ Init Plan
 */
typedef struct qldInitAlterDatabaseRecoverTablespace
{
    qlvInitPlan      mCommonInit;       /**< Common Init Plan */
    
    ellDictHandle   mSpaceHandle;      /**< Tablespace Handle */
} qldInitAlterDatabaseRecoverTablespace;

stlStatus qldValidateAlterDatabaseRecoverTablespace( smlTransId      aTransID,
                                                     qllDBCStmt    * aDBCStmt,
                                                     qllStatement  * aSQLStmt,
                                                     stlChar       * aSQLString,
                                                     qllNode       * aParseTree,
                                                     qllEnv        * aEnv );

stlStatus qldOptCodeAlterDatabaseRecoverTablespace( smlTransId      aTransID,
                                                    qllDBCStmt    * aDBCStmt,
                                                    qllStatement  * aSQLStmt,
                                                    qllEnv        * aEnv );

stlStatus qldOptDataAlterDatabaseRecoverTablespace( smlTransId      aTransID,
                                                    qllDBCStmt    * aDBCStmt,
                                                    qllStatement  * aSQLStmt,
                                                    qllEnv        * aEnv );

stlStatus qldExecuteAlterDatabaseRecoverTablespace( smlTransId      aTransID,
                                                    smlStatement  * aStmt,
                                                    qllDBCStmt    * aDBCStmt,
                                                    qllStatement  * aSQLStmt,
                                                    qllEnv        * aEnv );


#endif /* _QLD_ALTER_DATABASE_RECOVER_TABLESPACE_STMT_H_ */


