/*******************************************************************************
 * qldAlterDatabaseDropLogfileMember.h
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

#ifndef _QLD_ALTER_DATABASE_DROP_LOGFILE_MEMBER_STMT_H_
#define _QLD_ALTER_DATABASE_DROP_LOGFILE_MEMBER_STMT_H_ 1

/**
 * @file qldAlterDatabaseDropLogfileMember.h
 * @brief ALTER DATABASE DROP LOGFILE MEMBER Statement
 */

/**
 * @brief Init Plan of Alter Database DROP LOGFILE MEMBER Statement
 */
/**
 * @brief ALTER TABLESPACE DROP LOGFILE MEMBER ¸¦ À§ÇÑ Init Plan
 */
typedef struct qldInitDropLogfileMember
{
    stlInt32           mGroupCount;
    stlInt32           mMemberCount;
    smlLogStreamAttr * mStreamAttr;      /**< Log Stream Attribute */
} qldInitDropLogfileMember;

stlStatus qldValidateAlterDatabaseDropLogfileMember( smlTransId      aTransID,
                                                     qllDBCStmt    * aDBCStmt,
                                                     qllStatement  * aSQLStmt,
                                                     stlChar       * aSQLString,
                                                     qllNode       * aParseTree,
                                                     qllEnv        * aEnv );

stlStatus qldOptCodeAlterDatabaseDropLogfileMember( smlTransId      aTransID,
                                                    qllDBCStmt    * aDBCStmt,
                                                    qllStatement  * aSQLStmt,
                                                    qllEnv        * aEnv );

stlStatus qldOptDataAlterDatabaseDropLogfileMember( smlTransId      aTransID,
                                                    qllDBCStmt    * aDBCStmt,
                                                    qllStatement  * aSQLStmt,
                                                    qllEnv        * aEnv );

stlStatus qldExecuteAlterDatabaseDropLogfileMember( smlTransId      aTransID,
                                                    smlStatement  * aStmt,
                                                    qllDBCStmt    * aDBCStmt,
                                                    qllStatement  * aSQLStmt,
                                                    qllEnv        * aEnv );

#endif /* _QLD_ALTER_DATABASE_DROP_LOGFILE_MEMBER_STMT_H_ */


