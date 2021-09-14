/*******************************************************************************
 * qldAlterDatabaseBackup.h
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

#ifndef _QLD_ALTER_DATABASE_BACKUP_STMT_H_
#define _QLD_ALTER_DATABASE_BACKUP_STMT_H_ 1

/**
 * @file qldAlterDatabaseBackup.h
 * @brief ALTER DATABASE .. BACKUP Statement
 */

/**
 * @brief Init Plan of Alter Database .. BACKUP Statement
 */
/**
 * @brief ALTER DATABASE .. BACKUP ¸¦ À§ÇÑ Init Plan
 */

typedef struct qldInitAlterDatabaseBackup
{
    stlChar   mBackupType;
    stlChar   mCommand;
    stlChar   mOption;
    stlInt16  mLevel;
} qldInitAlterDatabaseBackup;

stlStatus qldValidateAlterDatabaseBackup( smlTransId      aTransID,
                                          qllDBCStmt    * aDBCStmt,
                                          qllStatement  * aSQLStmt,
                                          stlChar       * aSQLString,
                                          qllNode       * aParseTree,
                                          qllEnv        * aEnv );

stlStatus qldOptCodeAlterDatabaseBackup( smlTransId      aTransID,
                                         qllDBCStmt    * aDBCStmt,
                                         qllStatement  * aSQLStmt,
                                         qllEnv        * aEnv );

stlStatus qldOptDataAlterDatabaseBackup( smlTransId      aTransID,
                                         qllDBCStmt    * aDBCStmt,
                                         qllStatement  * aSQLStmt,
                                         qllEnv        * aEnv );

stlStatus qldExecuteAlterDatabaseBackup( smlTransId      aTransID,
                                         smlStatement  * aStmt,
                                         qllDBCStmt    * aDBCStmt,
                                         qllStatement  * aSQLStmt,
                                         qllEnv        * aEnv );

#endif /* _QLD_ALTER_DATABASE_BACKUP_STMT_H_ */


