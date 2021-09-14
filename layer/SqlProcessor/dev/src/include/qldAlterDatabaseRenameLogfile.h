/*******************************************************************************
 * qldAlterDatabaseRenameLogfile.h
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

#ifndef _QLD_ALTER_DATABASE_RENAME_LOGFILE_STMT_H_
#define _QLD_ALTER_DATABASE_RENAME_LOGFILE_STMT_H_ 1

/**
 * @file qldAlterDatabaseRenameLogfile.h
 * @brief ALTER DATABASE RENAME LOG FILE Statement
 */

/**
 * @brief Init Plan of Alter Database RENAME LOG FILE Statement
 */
/**
 * @brief ALTER TABLESPACE RENAME FILE ¸¦ À§ÇÑ Init Plan
 */
typedef struct qldInitRenameLogFile
{
    stlInt32           mAttrCount;
    smlLogMemberAttr * mFromFileAttr;    /**< From log member List */
    smlLogMemberAttr * mToFileAttr;      /**< To log member List */
} qldInitRenameLogFile;

stlStatus qldValidateAlterDatabaseRenameLogfile( smlTransId      aTransID,
                                              qllDBCStmt    * aDBCStmt,
                                              qllStatement  * aSQLStmt,
                                              stlChar       * aSQLString,
                                              qllNode       * aParseTree,
                                              qllEnv        * aEnv );

stlStatus qldOptCodeAlterDatabaseRenameLogfile( smlTransId      aTransID,
                                             qllDBCStmt    * aDBCStmt,
                                             qllStatement  * aSQLStmt,
                                             qllEnv        * aEnv );

stlStatus qldOptDataAlterDatabaseRenameLogfile( smlTransId      aTransID,
                                             qllDBCStmt    * aDBCStmt,
                                             qllStatement  * aSQLStmt,
                                             qllEnv        * aEnv );

stlStatus qldExecuteAlterDatabaseRenameLogfile( smlTransId      aTransID,
                                             smlStatement  * aStmt,
                                             qllDBCStmt    * aDBCStmt,
                                             qllStatement  * aSQLStmt,
                                             qllEnv        * aEnv );

#endif /* _QLD_ALTER_DATABASE_RENAME_LOGFILE_STMT_H_ */


