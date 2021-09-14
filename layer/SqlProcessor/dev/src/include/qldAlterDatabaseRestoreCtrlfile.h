/*******************************************************************************
 * qldAlterDatabaseRestoreCtrlfile.h
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

#ifndef _QLD_ALTER_DATABASE_RESTORE_CTRLFILE_STMT_H_
#define _QLD_ALTER_DATABASE_RESTORE_CTRLFILE_STMT_H_ 1

/**
 * @file qldAlterDatabaseRestoreCtrlfile.h
 * @brief ALTER DATABASE .. RESTORE Statement
 */

/**
 * @brief Init Plan of Alter Database RESTORE CONTROLFILE Statement
 */

typedef struct qldInitRestoreCtrlfile
{
    stlChar  mTarget[STL_MAX_FILE_PATH_LENGTH + STL_MAX_FILE_NAME_LENGTH];
} qldInitRestoreCtrlfile;

stlStatus qldValidateAlterDatabaseRestoreCtrlfile( smlTransId      aTransID,
                                                   qllDBCStmt    * aDBCStmt,
                                                   qllStatement  * aSQLStmt,
                                                   stlChar       * aSQLString,
                                                   qllNode       * aParseTree,
                                                   qllEnv        * aEnv );

stlStatus qldOptCodeAlterDatabaseRestoreCtrlfile( smlTransId      aTransID,
                                                  qllDBCStmt    * aDBCStmt,
                                                  qllStatement  * aSQLStmt,
                                                  qllEnv        * aEnv );

stlStatus qldOptDataAlterDatabaseRestoreCtrlfile( smlTransId      aTransID,
                                                  qllDBCStmt    * aDBCStmt,
                                                  qllStatement  * aSQLStmt,
                                                  qllEnv        * aEnv );

stlStatus qldExecuteAlterDatabaseRestoreCtrlfile( smlTransId      aTransID,
                                                  smlStatement  * aStmt,
                                                  qllDBCStmt    * aDBCStmt,
                                                  qllStatement  * aSQLStmt,
                                                  qllEnv        * aEnv );

#endif /* _QLD_ALTER_DATABASE_RESTORE_CTRLFILE_STMT_H_ */


