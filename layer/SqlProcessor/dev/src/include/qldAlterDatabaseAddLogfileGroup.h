/*******************************************************************************
 * qldAlterDatabaseAddLogfileGroup.h
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

#ifndef _QLD_ALTER_DATABASE_ADD_LOGFILE_GROUP_STMT_H_
#define _QLD_ALTER_DATABASE_ADD_LOGFILE_GROUP_STMT_H_ 1

/**
 * @file qldAlterDatabaseAddLogfileGroup.h
 * @brief ALTER DATABASE ADD LOGFILE GROUP Statement
 */

/**
 * @brief Init Plan of Alter Database ADD LOGFILE GROUP Statement
 */
/**
 * @brief ALTER TABLESPACE ADD LOGFILE GROUP ¸¦ À§ÇÑ Init Plan
 */
typedef struct qldInitAddLogFileGroup
{
    smlLogStreamAttr * mLogStreamAttr;      /**< Log Stream Attribute List */
} qldInitAddLogFileGroup;


stlStatus qldValidateAlterDatabaseAddLogfileGroup( smlTransId      aTransID,
                                                   qllDBCStmt    * aDBCStmt,
                                                   qllStatement  * aSQLStmt,
                                                   stlChar       * aSQLString,
                                                   qllNode       * aParseTree,
                                                   qllEnv        * aEnv );

stlStatus qldOptCodeAlterDatabaseAddLogfileGroup( smlTransId      aTransID,
                                                  qllDBCStmt    * aDBCStmt,
                                                  qllStatement  * aSQLStmt,
                                                  qllEnv        * aEnv );

stlStatus qldOptDataAlterDatabaseAddLogfileGroup( smlTransId      aTransID,
                                                  qllDBCStmt    * aDBCStmt,
                                                  qllStatement  * aSQLStmt,
                                                  qllEnv        * aEnv );

stlStatus qldExecuteAlterDatabaseAddLogfileGroup( smlTransId      aTransID,
                                                  smlStatement  * aStmt,
                                                  qllDBCStmt    * aDBCStmt,
                                                  qllStatement  * aSQLStmt,
                                                  qllEnv        * aEnv );

#endif /* _QLD_ALTER_DATABASE_ADD_LOGFILE_GROUP_STMT_H_ */


