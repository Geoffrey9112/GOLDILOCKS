/*******************************************************************************
 * qldAlterDatabaseRecoverDatafile.c
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

#ifndef _QLD_ALTER_DATABASE_RECOVER_DATAFILE_STMT_H_
#define _QLD_ALTER_DATABASE_RECOVER_DATAFILE_STMT_H_ 1

/**
 * @file qldAlterDatabaseRecoverDatafile.h
 * @brief ALTER DATABASE RECOVER DATAFILE .. Statement
 */

/**
 * @brief ALTER DATABASE REOCVER DATAFILE .. 를 위한 Init Plan
 */
typedef struct qldInitAlterDatabaseRecoverDatafile
{
    smlDatafileRecoveryInfo mRecoveryInfo;
} qldInitAlterDatabaseRecoverDatafile;

stlStatus qldValidateAlterDatabaseRecoverDatafile( smlTransId      aTransID,
                                                   qllDBCStmt    * aDBCStmt,
                                                   qllStatement  * aSQLStmt,
                                                   stlChar       * aSQLString,
                                                   qllNode       * aParseTree,
                                                   qllEnv        * aEnv );

stlStatus qldOptCodeAlterDatabaseRecoverDatafile( smlTransId      aTransID,
                                                  qllDBCStmt    * aDBCStmt,
                                                  qllStatement  * aSQLStmt,
                                                  qllEnv        * aEnv );

stlStatus qldOptDataAlterDatabaseRecoverDatafile( smlTransId      aTransID,
                                                  qllDBCStmt    * aDBCStmt,
                                                  qllStatement  * aSQLStmt,
                                                  qllEnv        * aEnv );

stlStatus qldExecuteAlterDatabaseRecoverDatafile( smlTransId      aTransID,
                                                  smlStatement  * aStmt,
                                                  qllDBCStmt    * aDBCStmt,
                                                  qllStatement  * aSQLStmt,
                                                  qllEnv        * aEnv );

#endif /* _QLD_ALTER_DATABASE_RECOVER_PAGE_STMT_H_ */
