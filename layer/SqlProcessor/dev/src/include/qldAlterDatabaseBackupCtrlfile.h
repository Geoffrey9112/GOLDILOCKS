/*******************************************************************************
 * qldAlterDatabaseBackupCtrlfile.h
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

#ifndef _QLD_ALTER_DATABASE_BACKUP_CTRLFILE_STMT_H_
#define _QLD_ALTER_DATABASE_BACKUP_CTRLFILE_STMT_H_ 1

/**
 * @file qldAlterDatabaseBackupCtrlfile.h
 * @brief ALTER DATABASE .. BACKUP Statement
 */

/**
 * @brief Init Plan of Alter Database BACKUP CONTROLFILE Statement
 */

typedef struct qldInitBackupCtrlfile
{
    stlChar  mTarget[STL_MAX_FILE_PATH_LENGTH];
} qldInitBackupCtrlfile;

stlStatus qldValidateAlterDatabaseBackupCtrlfile( smlTransId      aTransID,
                                                  qllDBCStmt    * aDBCStmt,
                                                  qllStatement  * aSQLStmt,
                                                  stlChar       * aSQLString,
                                                  qllNode       * aParseTree,
                                                  qllEnv        * aEnv );

stlStatus qldOptCodeAlterDatabaseBackupCtrlfile( smlTransId      aTransID,
                                                 qllDBCStmt    * aDBCStmt,
                                                 qllStatement  * aSQLStmt,
                                                 qllEnv        * aEnv );

stlStatus qldOptDataAlterDatabaseBackupCtrlfile( smlTransId      aTransID,
                                                 qllDBCStmt    * aDBCStmt,
                                                 qllStatement  * aSQLStmt,
                                                 qllEnv        * aEnv );

stlStatus qldExecuteAlterDatabaseBackupCtrlfile( smlTransId      aTransID,
                                                 smlStatement  * aStmt,
                                                 qllDBCStmt    * aDBCStmt,
                                                 qllStatement  * aSQLStmt,
                                                 qllEnv        * aEnv );

#endif /* _QLD_ALTER_DATABASE_BACKUP_CTRLFILE_STMT_H_ */


