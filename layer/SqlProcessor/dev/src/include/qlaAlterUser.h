/*******************************************************************************
 * qlaAlterUser.h
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

#ifndef _QLA_ALTER_USER_H_
#define _QLA_ALTER_USER_H_ 1

/**
 * @file qlaAlterUser.h
 * @brief ALTER USER
 */

/**
 * @brief ALTER USER 를 위한 Init Plan
 */
typedef struct qlaInitAlterUser
{
    qlvInitPlan      mCommonInit;       /**< Common Init Plan */
    
    ellDictHandle       mUserHandle;       /**< User Handle */

    qlpAlterUserAction  mAlterAction;      /**< Alter Action */

    /*
     * ALTER PASSWORD
     */

    stlBool             mHasOrgPassword;   /**< Original Password 존재 여부 */
    stlInt32            mPosOrgPassword;   /**< Original Password Position */
    
    stlChar             mPlainOrgPassword[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar             mPlainNewPassword[STL_MAX_SQL_IDENTIFIER_LENGTH];
    
    stlChar             mEncryptPassword[STL_MAX_SQL_IDENTIFIER_LENGTH];  /**< Encrypted Password */

    /*
     * TABLESPACE
     */
    
    ellDictHandle       mSpaceHandle;       /**< Tablespace Handle */

    /*
     * SCHEMA PATH
     */
    
    stlInt32            mSchemaMaxCnt;       /**< Schema Max Path Count */
    stlInt32            mSchemaCnt;          /**< Schema Path Count */
    ellDictHandle     * mSchemaHandleArray;  /**< Schema Handle Array */

    /*
     * ALTER PROFILE
     */

    stlBool        mHasProfile;              /**< PROFILE name or PROFILE null */
    ellDictHandle  mProfileHandle;           /**< PROFILE handle */
    
} qlaInitAlterUser;



stlStatus qlaValidateAlterUser( smlTransId      aTransID,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                stlChar       * aSQLString,
                                qllNode       * aParseTree,
                                qllEnv        * aEnv );

stlStatus qlaOptCodeAlterUser( smlTransId      aTransID,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv );

stlStatus qlaOptDataAlterUser( smlTransId      aTransID,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv );

stlStatus qlaExecuteAlterUser( smlTransId      aTransID,
                               smlStatement  * aStmt,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv );

/*
 * Validation 보조 함수
 */

stlStatus qlaValidateAlterPassword( smlTransId         aTransID,
                                    qllDBCStmt       * aDBCStmt,
                                    qllStatement     * aSQLStmt,
                                    stlChar          * aSQLString,
                                    qlpAlterUser     * aParseTree,
                                    qlaInitAlterUser * aInitPlan,
                                    qllEnv           * aEnv );

stlStatus qlaValidateAlterUserProfile( smlTransId         aTransID,
                                       qllStatement     * aSQLStmt,
                                       stlChar          * aSQLString,
                                       qlpAlterUser     * aParseTree,
                                       qlaInitAlterUser * aInitPlan,
                                       qllEnv           * aEnv );

stlStatus qlaValidateAlterSpace( smlTransId         aTransID,
                                 qllDBCStmt       * aDBCStmt,
                                 qllStatement     * aSQLStmt,
                                 stlChar          * aSQLString,
                                 qlpAlterUser     * aParseTree,
                                 qlaInitAlterUser * aInitPlan,
                                 qllEnv           * aEnv );

stlStatus qlaValidateAlterSchemaPath( smlTransId         aTransID,
                                      qllDBCStmt       * aDBCStmt,
                                      qllStatement     * aSQLStmt,
                                      stlChar          * aSQLString,
                                      qlpAlterUser     * aParseTree,
                                      qlaInitAlterUser * aInitPlan,
                                      qllEnv           * aEnv );

/*
 * Execution 보조 함수
 */

stlStatus qlaExecuteAlterPassword( smlTransId         aTransID,
                                   smlStatement     * aStmt,
                                   qllStatement     * aSQLStmt,
                                   qlaInitAlterUser * aInitPlan,
                                   stlTime            aModifyTime,
                                   qllEnv           * aEnv );

stlStatus qlaExecuteAlterUserProfile( smlTransId         aTransID,
                                      smlStatement     * aStmt,
                                      qllStatement     * aSQLStmt,
                                      qlaInitAlterUser * aInitPlan,
                                      stlTime            aModifyTime,
                                      qllEnv           * aEnv );

stlStatus qlaExecuteAlterPasswordExpire( smlTransId         aTransID,
                                         smlStatement     * aStmt,
                                         qlaInitAlterUser * aInitPlan,
                                         stlTime            aExpiryTime,
                                         qllEnv           * aEnv );

stlStatus qlaExecuteAlterAccountLocked( smlTransId         aTransID,
                                        smlStatement     * aStmt,
                                        qlaInitAlterUser * aInitPlan,
                                        stlTime            aLockedTime,
                                        qllEnv           * aEnv );

stlStatus qlaExecuteAlterSpace( smlTransId         aTransID,
                                smlStatement     * aStmt,
                                qllDBCStmt       * aDBCStmt,
                                qllStatement     * aSQLStmt,
                                qlaInitAlterUser * aInitPlan,
                                qllEnv           * aEnv );

stlStatus qlaExecuteAlterSchemaPath( smlTransId         aTransID,
                                     smlStatement     * aStmt,
                                     qllDBCStmt       * aDBCStmt,
                                     qllStatement     * aSQLStmt,
                                     qlaInitAlterUser * aInitPlan,
                                     qllEnv           * aEnv );


#endif /* _QLA_ALTER_USER_H_ */
