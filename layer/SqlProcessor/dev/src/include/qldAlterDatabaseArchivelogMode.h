/*******************************************************************************
 * qldAlterDatabaseArchivelogMode.h
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

#ifndef _QLD_ALTER_DATABASE_ARCHIVELOG_MODE_STMT_H_
#define _QLD_ALTER_DATABASE_ARCHIVELOG_MODE_STMT_H_ 1

/**
 * @file qldAlterDatabaseArchivelogMode.h
 * @brief ALTER DATABASE ARCHIVELOG Statement
 */

/**
 * @brief Init Plan of Alter Database ARCHIVELOG | NOARCHIVELOG Statement
 */
/**
 * @brief ALTER TABLESPACE ARCHIVELOG | NOARCHIVELOG ¸¦ À§ÇÑ Init Plan
 */

stlStatus qldValidateAlterDatabaseArchivelogMode( smlTransId      aTransID,
                                                  qllDBCStmt    * aDBCStmt,
                                                  qllStatement  * aSQLStmt,
                                                  stlChar       * aSQLString,
                                                  qllNode       * aParseTree,
                                                  qllEnv        * aEnv );

stlStatus qldOptCodeAlterDatabaseArchivelogMode( smlTransId      aTransID,
                                                 qllDBCStmt    * aDBCStmt,
                                                 qllStatement  * aSQLStmt,
                                                 qllEnv        * aEnv );

stlStatus qldOptDataAlterDatabaseArchivelogMode( smlTransId      aTransID,
                                                 qllDBCStmt    * aDBCStmt,
                                                 qllStatement  * aSQLStmt,
                                                 qllEnv        * aEnv );

stlStatus qldExecuteAlterDatabaseArchivelogMode( smlTransId      aTransID,
                                                 smlStatement  * aStmt,
                                                 qllDBCStmt    * aDBCStmt,
                                                 qllStatement  * aSQLStmt,
                                                 qllEnv        * aEnv );

#endif /* _QLD_ALTER_DATABASE_ARCHIVELOG_MODE_STMT_H_ */


