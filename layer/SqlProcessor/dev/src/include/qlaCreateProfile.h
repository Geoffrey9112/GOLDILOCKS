/*******************************************************************************
 * qlaCreateProfile.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlaCreateProfile.h 5803 2012-09-27 06:40:12Z jhkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/

#ifndef _QLA_CREATE_PROFILE_H_
#define _QLA_CREATE_PROFILE_H_ 1

/**
 * @file qlaCreateProfile.h
 * @brief CREATE PROFILE
 */

/****************************************************
 * CREATE PROFILE Nodes
 ****************************************************/

typedef struct qlaInitPasswordParameter     qlaInitPasswordParameter;

/**
 * @brief Init Plan: Password Parameter List 
 */
struct qlaInitPasswordParameter
{
    ellProfileParam      mParameterType;     /**< Password Parameter Type */

    stlBool              mIsDefault;         /**< Is Default */

    stlInt64             mIntegerValue;      /**< Integer Value */
    
    dtlDataValue       * mIntervalValue;     /**< Inteval Value */
    
    stlChar            * mValueString;       /**< ValueString */

    struct qlaInitPasswordParameter  * mNext;
};


/**
 * @brief CREATE PROFILE 를 위한 Init Plan
 */
typedef struct qlaInitCreateProfile
{
    qlvInitPlan                mCommonInit;            /**< Common Init Plan */

    stlChar                  * mProfileName;           /**< Profile Name */
    
    qlaInitPasswordParameter * mPasswordParameterList; /**< Password ParameterList */
    
} qlaInitCreateProfile;

stlStatus qlaValidateCreateProfile( smlTransId      aTransID,
                                    qllDBCStmt    * aDBCStmt,
                                    qllStatement  * aSQLStmt,
                                    stlChar       * aSQLString,
                                    qllNode       * aParseTree,
                                    qllEnv        * aEnv );

stlStatus qlaOptCodeCreateProfile( smlTransId      aTransID,
                                   qllDBCStmt    * aDBCStmt,
                                   qllStatement  * aSQLStmt,
                                   qllEnv        * aEnv );

stlStatus qlaOptDataCreateProfile( smlTransId      aTransID,
                                   qllDBCStmt    * aDBCStmt,
                                   qllStatement  * aSQLStmt,
                                   qllEnv        * aEnv );

stlStatus qlaExecuteCreateProfile( smlTransId      aTransID,
                                   smlStatement  * aStmt,
                                   qllDBCStmt    * aDBCStmt,
                                   qllStatement  * aSQLStmt,
                                   qllEnv        * aEnv );


/*
 * Validation 보조 함수
 */

stlStatus qlaAddPasswordParameter( qllDBCStmt                * aDBCStmt,
                                   qllStatement              * aSQLStmt,
                                   stlChar                   * aSQLString,
                                   qlpList                   * aUserDefPasswordParameterList,
                                   ellProfileParam             aParameterType, 
                                   qlaInitPasswordParameter ** aPasswordParameterList, 
                                   qllEnv                    * aEnv );

stlStatus qlaValidateUserParam( qllDBCStmt               * aDBCStmt,
                                qllStatement             * aSQLStmt,
                                stlChar                  * aSQLString,
                                qlpPasswordParameter     * aParseParam,
                                stlBool                    aUndefParam,
                                qlaInitPasswordParameter * aInitParam,
                                qllEnv                   * aEnv );



/*
 * Password Verify 함수
 */
stlStatus qlaPasswordComplexityCheck( stlChar  * aPassword,
                                      stlInt64   aPasswordLen,
                                      stlInt64   aLetterCnt,
                                      stlInt64   aUpperCharCnt,
                                      stlInt64   aLowerCharCnt,
                                      stlInt64   aDigitCnt,
                                      stlInt64   aSpecialCharCnt,
                                      qllEnv   * aEnv );

stlStatus  qlaGetReversedWord( stlChar   * aWord,
                               stlChar   * aReversedWord );

stlInt64  qlaLevenShteinDistance( stlChar  * aWord1,
                                  stlInt64   aLen1,
                                  stlChar  * aWord2,
                                  stlInt64   aLen2 );

stlStatus qlaVerifyPassword( stlChar  * aUserName,
                             stlChar  * aPlainNewPassword,
                             stlChar  * aPlainOrgPassword,
                             stlInt64   aVerifyFuncID,
                             qllEnv   * aEnv );

stlStatus qlaVerifyPasswordKisa( stlChar * aUserName,
                                 stlChar * aNewPassword,
                                 stlChar * aOrgPassword,
                                 qllEnv  * aEnv );

stlStatus qlaVerifyPasswordOra12( stlChar * aUserName,
                                  stlChar * aNewPassword,
                                  stlChar * aOrgPassword,
                                  qllEnv  * aEnv );

stlStatus qlaVerifyPasswordOra12Strong( stlChar * aUserName,
                                        stlChar * aNewPassword,
                                        stlChar * aOrgPassword,
                                        qllEnv  * aEnv );

stlStatus qlaVerifyPasswordOra11( stlChar * aUserName,
                                  stlChar * aNewPassword,
                                  stlChar * aOrgPassword,
                                  qllEnv  * aEnv );

stlStatus qlaVerifyPasswordOracle( stlChar * aUserName,
                                   stlChar * aNewPassword,
                                   stlChar * aOrgPassword,
                                   qllEnv  * aEnv );

stlStatus qlaCheckPasswordReuse( smlTransId            aTransID,
                                 smlStatement        * aStmt,
                                 ellDictHandle       * aUserHandle,
                                 ellAuthenInfo       * aUserAuthenInfo,
                                 ellUserProfileParam * aProfileParam,
                                 stlChar             * aPlainPassword,
                                 stlTime               aChangeTime,
                                 qllEnv              * aEnv );

#endif /* _QLA_CREATE_PROFILE_H_ */
