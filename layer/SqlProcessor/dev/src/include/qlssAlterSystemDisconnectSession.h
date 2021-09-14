/*******************************************************************************
 * qlssAlterSystemDisconnectSession.h
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

#ifndef _QLSS_ALTER_SYSTEM_DISCONNECT_SESSION_H_
#define _QLSS_ALTER_SYSTEM_DISCONNECT_SESSION_H_ 1

/**
 * @file qlssAlterSystemDisconnectSession.h
 * @brief System Control Language
 */

/**
 * @brief [KILL/DISCONNECT] SESSION 의 옵션
 */
typedef enum
{
    QLSS_END_SESSION_OPTION_IMMEDIATE = 1, /**< IMMEDIATE */
    QLSS_END_SESSION_OPTION_MAX
} qlssEndSessionOption;

/**
 * @brief ALTER SYSTEM DISCONNECT SESSION Init Plan
 */
typedef struct qlssInitSystemDisconnectSession
{
    stlUInt32            mSessionID;            /**< SESSION ID */
    stlUInt64            mSerialNum;            /**< SERIAL# */
    stlBool              mIsPostTransaction;    /**< POST_TRANSACTION */
    qlssEndSessionOption mOption;               /**< OPTION */
} qlssInitSystemDisconnectSession;



stlStatus qlssValidateSystemDisconnectSession( smlTransId      aTransID,
                                               qllDBCStmt    * aDBCStmt,
                                               qllStatement  * aSQLStmt,
                                               stlChar       * aSQLString,
                                               qllNode       * aParseTree,
                                               qllEnv        * aEnv );

stlStatus qlssOptCodeSystemDisconnectSession( smlTransId      aTransID,
                                              qllDBCStmt    * aDBCStmt,
                                              qllStatement  * aSQLStmt,
                                              qllEnv        * aEnv );

stlStatus qlssOptDataSystemDisconnectSession( smlTransId      aTransID,
                                              qllDBCStmt    * aDBCStmt,
                                              qllStatement  * aSQLStmt,
                                              qllEnv        * aEnv );

stlStatus qlssExecuteSystemDisconnectSession( smlTransId      aTransID,
                                              smlStatement  * aStmt,
                                              qllDBCStmt    * aDBCStmt,
                                              qllStatement  * aSQLStmt,
                                              qllEnv        * aEnv );


#endif /* _QLSS_ALTER_SYSTEM_DISCONNECT_SESSION_H_ */

