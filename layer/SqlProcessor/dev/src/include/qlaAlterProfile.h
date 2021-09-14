/*******************************************************************************
 * qlaAlterProfile.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlaAlterProfile.h 5803 2012-09-27 06:40:12Z jhkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/

#ifndef _QLA_ALTER_PROFILE_H_
#define _QLA_ALTER_PROFILE_H_ 1

/**
 * @file qlaAlterProfile.h
 * @brief ALTER PROFILE
 */

/**
 * @brief ALTER PROFILE 를 위한 Init Plan
 */
typedef struct qlaInitAlterProfile
{
    qlvInitPlan      mCommonInit;       /**< Common Init Plan */

    ellDictHandle    mProfileHandle;    /**< Profile Handle */

    stlBool          mHasLifeTime;      /**< PASSWORD_LIFE_TIME 포함 여부 */
    qlaInitPasswordParameter * mPasswordParameterList; /**< Password ParameterList */
    
} qlaInitAlterProfile;

stlStatus qlaValidateAlterProfile( smlTransId      aTransID,
                                   qllDBCStmt    * aDBCStmt,
                                   qllStatement  * aSQLStmt,
                                   stlChar       * aSQLString,
                                   qllNode       * aParseTree,
                                   qllEnv        * aEnv );

stlStatus qlaOptCodeAlterProfile( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  qllEnv        * aEnv );

stlStatus qlaOptDataAlterProfile( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  qllEnv        * aEnv );

stlStatus qlaExecuteAlterProfile( smlTransId      aTransID,
                                  smlStatement  * aStmt,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  qllEnv        * aEnv );

/*
 * Execution 보조 함수
 */


#endif /* _QLA_ALTER_PROFILE_H_ */
