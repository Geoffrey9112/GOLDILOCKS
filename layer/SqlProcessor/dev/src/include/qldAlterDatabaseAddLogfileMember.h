/*******************************************************************************
 * qldAlterDatabaseAddLogfileMember.h
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

#ifndef _QLD_ALTER_DATABASE_ADD_LOGFILE_MEMBER_STMT_H_
#define _QLD_ALTER_DATABASE_ADD_LOGFILE_MEMBER_STMT_H_ 1

/**
 * @file qldAlterDatabaseAddLogfileMember.h
 * @brief ALTER DATABASE ADD LOGFILE MEMBER Statement
 */

/**
 * @brief Init Plan of Alter Database ADD LOGFILE MEMBER Statement
 */
/**
 * @brief ALTER DATABASE ADD LOGFILE MEMBER ¸¦ À§ÇÑ Init Plan
 */
typedef struct qldInitAddLogfileMember
{
    stlInt32          mMemberCount;
    smlLogGroupAttr * mLogGroupAttr;
} qldInitAddLogfileMember;

stlStatus qldValidateAlterDatabaseAddLogfileMember( smlTransId      aTransID,
                                                    qllDBCStmt    * aDBCStmt,
                                                    qllStatement  * aSQLStmt,
                                                    stlChar       * aSQLString,
                                                    qllNode       * aParseTree,
                                                    qllEnv        * aEnv );

stlStatus qldOptCodeAlterDatabaseAddLogfileMember( smlTransId      aTransID,
                                                   qllDBCStmt    * aDBCStmt,
                                                   qllStatement  * aSQLStmt,
                                                   qllEnv        * aEnv );

stlStatus qldOptDataAlterDatabaseAddLogfileMember( smlTransId      aTransID,
                                                   qllDBCStmt    * aDBCStmt,
                                                   qllStatement  * aSQLStmt,
                                                   qllEnv        * aEnv );

stlStatus qldExecuteAlterDatabaseAddLogfileMember( smlTransId      aTransID,
                                                   smlStatement  * aStmt,
                                                   qllDBCStmt    * aDBCStmt,
                                                   qllStatement  * aSQLStmt,
                                                   qllEnv        * aEnv );

#endif /* _QLD_ALTER_DATABASE_ADD_LOGFILE_MEMBER_STMT_H_ */


