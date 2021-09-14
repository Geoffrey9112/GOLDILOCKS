/*******************************************************************************
 * qldAlterDatabaseDropLogfileGroup.h
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

#ifndef _QLD_ALTER_DATABASE_DROP_LOGFILE_GROUP_STMT_H_
#define _QLD_ALTER_DATABASE_DROP_LOGFILE_GROUP_STMT_H_ 1

/**
 * @file qldAlterDatabaseDropLogfileGroup.h
 * @brief ALTER DATABASE DROP LOGFILE GROUP Statement
 */

/**
 * @brief Init Plan of Alter Database DROP LOGFILE GROUP Statement
 */
/**
 * @brief ALTER TABLESPACE DROP LOGFILE GROUP ¸¦ À§ÇÑ Init Plan
 */
typedef struct qldInitDropLogfileGroup
{
    smlLogStreamAttr   mLogStreamAttr;      /**< Log Stream Attribute */
} qldInitDropLogfileGroup;

stlStatus qldValidateAlterDatabaseDropLogfileGroup( smlTransId      aTransID,
                                                    qllDBCStmt    * aDBCStmt,
                                                    qllStatement  * aSQLStmt,
                                                    stlChar       * aSQLString,
                                                    qllNode       * aParseTree,
                                                    qllEnv        * aEnv );

stlStatus qldOptCodeAlterDatabaseDropLogfileGroup( smlTransId      aTransID,
                                                   qllDBCStmt    * aDBCStmt,
                                                   qllStatement  * aSQLStmt,
                                                   qllEnv        * aEnv );

stlStatus qldOptDataAlterDatabaseDropLogfileGroup( smlTransId      aTransID,
                                                   qllDBCStmt    * aDBCStmt,
                                                   qllStatement  * aSQLStmt,
                                                   qllEnv        * aEnv );

stlStatus qldExecuteAlterDatabaseDropLogfileGroup( smlTransId      aTransID,
                                                   smlStatement  * aStmt,
                                                   qllDBCStmt    * aDBCStmt,
                                                   qllStatement  * aSQLStmt,
                                                   qllEnv        * aEnv );

#endif /* _QLD_ALTER_DATABASE_DROP_LOGFILE_GROUP_STMT_H_ */


