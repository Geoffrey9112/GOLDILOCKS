/*******************************************************************************
 * qlssSetSessionAuth.h
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

#ifndef _QLSS_SET_SESSION_AUTH_H_
#define _QLSS_SET_SESSION_AUTH_H_ 1

/**
 * @file qlssSetSessionAuth.h
 * @brief Session Control Language
 */

/**
 * @brief SET SESSION AUTHORIZATION 를 위한 Init Plan
 */
typedef struct qlssInitSetAuth
{
    qlvInitPlan      mCommonInit;       /**< Common Init Plan */
    
    ellDictHandle       mUserHandle;       /**< User Handle */
} qlssInitSetAuth;


stlStatus qlssValidateSetSessionAuth( smlTransId      aTransID,
                                      qllDBCStmt    * aDBCStmt,
                                      qllStatement  * aSQLStmt,
                                      stlChar       * aSQLString,
                                      qllNode       * aParseTree,
                                      qllEnv        * aEnv );

stlStatus qlssOptCodeSetSessionAuth( smlTransId      aTransID,
                                     qllDBCStmt    * aDBCStmt,
                                     qllStatement  * aSQLStmt,
                                     qllEnv        * aEnv );

stlStatus qlssOptDataSetSessionAuth( smlTransId      aTransID,
                                     qllDBCStmt    * aDBCStmt,
                                     qllStatement  * aSQLStmt,
                                     qllEnv        * aEnv );

stlStatus qlssExecuteSetSessionAuth( smlTransId      aTransID,
                                     smlStatement  * aStmt,
                                     qllDBCStmt    * aDBCStmt,
                                     qllStatement  * aSQLStmt,
                                     qllEnv        * aEnv );


#endif /* _QLSS_SET_SESSION_AUTH_H_ */

