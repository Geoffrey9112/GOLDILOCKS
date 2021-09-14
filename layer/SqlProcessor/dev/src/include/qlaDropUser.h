/*******************************************************************************
 * qlaDropUser.h
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

#ifndef _QLA_DROP_USER_H_
#define _QLA_DROP_USER_H_ 1

/**
 * @file qlaDropUser.h
 * @brief DROP USER
 */

/**
 * @brief DROP USER 를 위한 Init Plan
 */
typedef struct qlaInitDropUser
{
    qlvInitPlan      mCommonInit;       /**< Common Init Plan */
    
    stlBool          mIfExists;         /**< IF EXISTS 옵션 사용 여부 */
    stlBool          mUserExist;        /**< User 존재 여부 */
    
    ellDictHandle    mUserHandle;       /**< User Handle */
    
    stlBool          mIsCascade;        /**< CASCADE 여부  */
    
} qlaInitDropUser;



stlStatus qlaValidateDropUser( smlTransId      aTransID,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               stlChar       * aSQLString,
                               qllNode       * aParseTree,
                               qllEnv        * aEnv );

stlStatus qlaOptCodeDropUser( smlTransId      aTransID,
                              qllDBCStmt    * aDBCStmt,
                              qllStatement  * aSQLStmt,
                              qllEnv        * aEnv );

stlStatus qlaOptDataDropUser( smlTransId      aTransID,
                              qllDBCStmt    * aDBCStmt,
                              qllStatement  * aSQLStmt,
                              qllEnv        * aEnv );

stlStatus qlaExecuteDropUser( smlTransId      aTransID,
                              smlStatement  * aStmt,
                              qllDBCStmt    * aDBCStmt,
                              qllStatement  * aSQLStmt,
                              qllEnv        * aEnv );

/*
 * Execution 보조 함수
 */


#endif /* _QLA_DROP_USER_H_ */
