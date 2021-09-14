/*******************************************************************************
 * qldAlterDatabaseRecover.h
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

#ifndef _QLD_ALTER_DATABASE_RECOVER_STMT_H_
#define _QLD_ALTER_DATABASE_RECOVER_STMT_H_ 1

/**
 * @file qldAlterDatabaseRecover.h
 * @brief ALTER DATABASE .. RECOVER Statement
 */

/**
 * @brief Init Plan of Alter Database RECOVER Statement
 */

stlStatus qldValidateAlterDatabaseRecover( smlTransId      aTransID,
                                           qllDBCStmt    * aDBCStmt,
                                           qllStatement  * aSQLStmt,
                                           stlChar       * aSQLString,
                                           qllNode       * aParseTree,
                                           qllEnv        * aEnv );

stlStatus qldOptCodeAlterDatabaseRecover( smlTransId      aTransID,
                                          qllDBCStmt    * aDBCStmt,
                                          qllStatement  * aSQLStmt,
                                          qllEnv        * aEnv );

stlStatus qldOptDataAlterDatabaseRecover( smlTransId      aTransID,
                                          qllDBCStmt    * aDBCStmt,
                                          qllStatement  * aSQLStmt,
                                          qllEnv        * aEnv );

stlStatus qldExecuteAlterDatabaseRecover( smlTransId      aTransID,
                                          smlStatement  * aStmt,
                                          qllDBCStmt    * aDBCStmt,
                                          qllStatement  * aSQLStmt,
                                          qllEnv        * aEnv );

#endif /* _QLD_ALTER_DATABASE_RECOVER_STMT_H_ */


