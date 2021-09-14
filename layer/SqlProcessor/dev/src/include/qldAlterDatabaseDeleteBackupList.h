/*******************************************************************************
 * qldAlterDatabaseDeleteBackupList.h
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

#ifndef _QLD_ALTER_DATABASE_DELETE_BACKUP_LIST_STMT_H_
#define _QLD_ALTER_DATABASE_DELETE_BACKUP_LIST_STMT_H_ 1

/**
 * @file qldAlterDatabaseDeleteBackupList.h
 * @brief ALTER DATABASE DELETE BACKUP LIST Statement
 */

/**
 * @brief Init Plan of Alter Database Delete Backup List Statement
 */
/**
 * @brief ALTER DATABASE DELETE BACKUP LIST ¸¦ À§ÇÑ Init Plan
 */

typedef struct qldInitAlterDatabaseDeleteBackupList
{
    stlChar   mTarget;
    stlBool   mDeleteFile;
} qldInitAlterDatabaseDeleteBackupList;

stlStatus qldValidateAlterDatabaseDeleteBackupList( smlTransId      aTransID,
                                                    qllDBCStmt    * aDBCStmt,
                                                    qllStatement  * aSQLStmt,
                                                    stlChar       * aSQLString,
                                                    qllNode       * aParseTree,
                                                    qllEnv        * aEnv );

stlStatus qldOptCodeAlterDatabaseDeleteBackupList( smlTransId      aTransID,
                                                   qllDBCStmt    * aDBCStmt,
                                                   qllStatement  * aSQLStmt,
                                                   qllEnv        * aEnv );

stlStatus qldOptDataAlterDatabaseDeleteBackupList( smlTransId      aTransID,
                                                   qllDBCStmt    * aDBCStmt,
                                                   qllStatement  * aSQLStmt,
                                                   qllEnv        * aEnv );

stlStatus qldExecuteAlterDatabaseDeleteBackupList( smlTransId      aTransID,
                                                   smlStatement  * aStmt,
                                                   qllDBCStmt    * aDBCStmt,
                                                   qllStatement  * aSQLStmt,
                                                   qllEnv        * aEnv );

#endif /* _QLD_ALTER_DATABASE_DELETE_BACKUP_LIST_STMT_H_ */


