/*******************************************************************************
 * qldAlterDatabaseClearPasswordHistory.h
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

#ifndef _QLD_ALTER_DATABASE_CLEAR_PASSWORD_HISTORY_H_
#define _QLD_ALTER_DATABASE_CLEAR_PASSWORD_HISTORY_H_ 1

/**
 * @file qldAlterDatabaseClearPasswordHistory.h
 * @brief ALTER DATABASE CLEAR PASSWORD HISTORY Statement
 */

/**
 * @brief ALTER DATABASE CLEAR PASSWORD HISTORY ¸¦ À§ÇÑ Init Plan
 */

typedef struct qldInitAlterDatabaseClearPassHistory
{
    qlvInitPlan      mCommonInit;       /**< Common Init Plan */
} qldInitAlterDatabaseClearPassHistory;

stlStatus qldValidateAlterDatabaseClearPasswordHistory( smlTransId      aTransID,
                                                        qllDBCStmt    * aDBCStmt,
                                                        qllStatement  * aSQLStmt,
                                                        stlChar       * aSQLString,
                                                        qllNode       * aParseTree,
                                                        qllEnv        * aEnv );

stlStatus qldOptCodeAlterDatabaseClearPasswordHistory( smlTransId      aTransID,
                                                       qllDBCStmt    * aDBCStmt,
                                                       qllStatement  * aSQLStmt,
                                                       qllEnv        * aEnv );

stlStatus qldOptDataAlterDatabaseClearPasswordHistory( smlTransId      aTransID,
                                                       qllDBCStmt    * aDBCStmt,
                                                       qllStatement  * aSQLStmt,
                                                       qllEnv        * aEnv );

stlStatus qldExecuteAlterDatabaseClearPasswordHistory( smlTransId      aTransID,
                                                       smlStatement  * aStmt,
                                                       qllDBCStmt    * aDBCStmt,
                                                       qllStatement  * aSQLStmt,
                                                       qllEnv        * aEnv );

#endif /* _QLD_ALTER_DATABASE_CLEAR_PASSWORD_HISTORY_H_ */


