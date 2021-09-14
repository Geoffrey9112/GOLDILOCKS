/*******************************************************************************
 * qlssAlterSystemKillSession.h
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

#ifndef _QLSS_ALTER_SYSTEM_KILL_SESSION_H_
#define _QLSS_ALTER_SYSTEM_KILL_SESSION_H_ 1

/**
 * @file qlssAlterSystemKillSession.h
 * @brief System Control Language
 */

/**
 * @brief ALTER SYSTEM KILL SESSION Init Plan
 */
typedef struct qlssInitSystemKillSession
{
    stlUInt32    mSessionID;            /**< SESSION ID */
    stlUInt64    mSerialNum;            /**< SERIAL# */
} qlssInitSystemKillSession;




stlStatus qlssValidateSystemKillSession( smlTransId      aTransID,
                                         qllDBCStmt    * aDBCStmt,
                                         qllStatement  * aSQLStmt,
                                         stlChar       * aSQLString,
                                         qllNode       * aParseTree,
                                         qllEnv        * aEnv );

stlStatus qlssOptCodeSystemKillSession( smlTransId      aTransID,
                                        qllDBCStmt    * aDBCStmt,
                                        qllStatement  * aSQLStmt,
                                        qllEnv        * aEnv );

stlStatus qlssOptDataSystemKillSession( smlTransId      aTransID,
                                        qllDBCStmt    * aDBCStmt,
                                        qllStatement  * aSQLStmt,
                                        qllEnv        * aEnv );

stlStatus qlssExecuteSystemKillSession( smlTransId      aTransID,
                                        smlStatement  * aStmt,
                                        qllDBCStmt    * aDBCStmt,
                                        qllStatement  * aSQLStmt,
                                        qllEnv        * aEnv );

#endif /* _QLSS_ALTER_SYSTEM_KILL_SESSION_H_ */

