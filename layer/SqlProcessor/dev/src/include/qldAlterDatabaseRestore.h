/*******************************************************************************
 * qldAlterDatabaseRestore.h
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

#ifndef _QLD_ALTER_DATABASE_RESTORE_STMT_H_
#define _QLD_ALTER_DATABASE_RESTORE_STMT_H_ 1

/**
 * @file qldAlterDatabaseRestore.h
 * @brief ALTER DATABASE .. RESTORE Statement
 */

/**
 * @brief Init Plan of Alter Database RESTORE Statement
 */
/**
 * @brief ALTER TABLESPACE RESTORE ¸¦ À§ÇÑ Init Plan
 */
typedef struct qldInitAlterDatabaseRestore
{
    stlChar   mUntilType;
    stlInt64  mUntilValue;
} qldInitAlterDatabaseRestore;

stlStatus qldValidateAlterDatabaseRestore( smlTransId      aTransID,
                                           qllDBCStmt    * aDBCStmt,
                                           qllStatement  * aSQLStmt,
                                           stlChar       * aSQLString,
                                           qllNode       * aParseTree,
                                           qllEnv        * aEnv );

stlStatus qldOptCodeAlterDatabaseRestore( smlTransId      aTransID,
                                          qllDBCStmt    * aDBCStmt,
                                          qllStatement  * aSQLStmt,
                                          qllEnv        * aEnv );

stlStatus qldOptDataAlterDatabaseRestore( smlTransId      aTransID,
                                          qllDBCStmt    * aDBCStmt,
                                          qllStatement  * aSQLStmt,
                                          qllEnv        * aEnv );

stlStatus qldExecuteAlterDatabaseRestore( smlTransId      aTransID,
                                          smlStatement  * aStmt,
                                          qllDBCStmt    * aDBCStmt,
                                          qllStatement  * aSQLStmt,
                                          qllEnv        * aEnv );

#endif /* _QLD_ALTER_DATABASE_RESTORE_STMT_H_ */


