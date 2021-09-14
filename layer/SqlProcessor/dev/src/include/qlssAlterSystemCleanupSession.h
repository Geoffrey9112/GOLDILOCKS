/*******************************************************************************
 * qlssAlterSystemCleanupSession.h
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

#ifndef _QLSS_ALTER_SYSTEM_CLEANUP_SESSION_H_
#define _QLSS_ALTER_SYSTEM_CLEANUP_SESSION_H_ 1

/**
 * @file qlssAlterSystemCleanupSession.h
 * @brief System Control Language
 */

/**
 * @brief ALTER SYSTEM CLEANUP SESSION Init Plan
 */
typedef struct qlssInitSystemCleanupSession
{
    stlInt64   mDummy;      /**< prepare/execution 의 유효성 검사를 위한 dummy plan */
} qlssInitSystemCleanupSession;

stlStatus qlssValidateSystemCleanupSession( smlTransId      aTransID,
                                            qllDBCStmt    * aDBCStmt,
                                            qllStatement  * aSQLStmt,
                                            stlChar       * aSQLString,
                                            qllNode       * aParseTree,
                                            qllEnv        * aEnv );

stlStatus qlssOptCodeSystemCleanupSession( smlTransId      aTransID,
                                           qllDBCStmt    * aDBCStmt,
                                           qllStatement  * aSQLStmt,
                                           qllEnv        * aEnv );

stlStatus qlssOptDataSystemCleanupSession( smlTransId      aTransID,
                                           qllDBCStmt    * aDBCStmt,
                                           qllStatement  * aSQLStmt,
                                           qllEnv        * aEnv );

stlStatus qlssExecuteSystemCleanupSession( smlTransId      aTransID,
                                           smlStatement  * aStmt,
                                           qllDBCStmt    * aDBCStmt,
                                           qllStatement  * aSQLStmt,
                                           qllEnv        * aEnv );



#endif /* _QLSS_ALTER_SYSTEM_CLEANUP_SESSION_H_ */

