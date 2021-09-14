/*******************************************************************************
 * qlaCreateUser.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlaCreateUser.h 5803 2012-09-27 06:40:12Z jhkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/

#ifndef _QLA_CREATE_USER_H_
#define _QLA_CREATE_USER_H_ 1

/**
 * @file qlaCreateUser.h
 * @brief CREATE USER
 */

#define QLA_USER_SCHEMA_PATH_CNT (4)

/**
 * @brief CREATE USER 를 위한 Init Plan
 */
typedef struct qlaInitCreateUser
{
    qlvInitPlan      mCommonInit;       /**< Common Init Plan */
    
    stlChar                * mUserName;                                        /**< User Name */
    stlChar                  mPlainPassword[STL_MAX_SQL_IDENTIFIER_LENGTH];    /**< Plain Text Password */
    stlChar                  mEncryptPassword[STL_MAX_SQL_IDENTIFIER_LENGTH];  /**< Encrypted Password */
    
    stlChar                * mSchemaName;       /**< Schema Name */

    ellDictHandle           mDataSpaceHandle;   /**< Default Data Tablespace */
    ellDictHandle           mTempSpaceHandle;   /**< Default Temporary Tablespace */

    stlBool                 mHasProfile;        /**< PROFILE 존재 여부 */
    ellDictHandle           mProfileHandle;     /**< Profile handle */
    stlBool                 mPasswordExpire;    /**< PASSWORD EXPIRE */
    stlBool                 mAccountLock;       /**< ACCOUNT LOCK|UNLOCK */
} qlaInitCreateUser;


stlStatus qlaValidateCreateUser( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 stlChar       * aSQLString,
                                 qllNode       * aParseTree,
                                 qllEnv        * aEnv );

stlStatus qlaOptCodeCreateUser( smlTransId      aTransID,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv );

stlStatus qlaOptDataCreateUser( smlTransId      aTransID,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv );

stlStatus qlaExecuteCreateUser( smlTransId      aTransID,
                                smlStatement  * aStmt,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv );

/*
 * Execution 보조 함수
 */


#endif /* _QLA_CREATE_USER_H_ */
