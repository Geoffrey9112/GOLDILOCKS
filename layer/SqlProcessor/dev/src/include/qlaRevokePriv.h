/*******************************************************************************
 * qlaRevokePriv.h
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

#ifndef _QLA_REVOKE_PRIV_H_
#define _QLA_REVOKE_PRIV_H_ 1

/**
 * @file qlaRevokePriv.h
 * @brief REVOKE PRIVILEGE
 */

/**
 * @brief REVOKE PRIVILEGE 를 위한 Init Plan
 */
typedef struct qlaInitRevokePriv
{
    qlvInitPlan      mCommonInit;       /**< Common Init Plan */
    
    /*
     * Privilege Object 정보
     */
    
    ellPrivObject       mPrivObject;     /**< Privilege Object 유형 */
    ellDictHandle       mObjectHandle;   /**< Privilege Object Handle */

    /*
     * ACCESS CONTROL ON DATABASE 권한 정보 
     */

    stlBool             mHasControl;     /**< ACCESS CONTROL 권한 존재 여부 */
    ellDictHandle       mControlHandle;  /**< ACCESS CONTROL handle */
    
    /*
     * Revoke Privilege 정보 
     */

    ellObjectList     * mRevokeeList;        /**< Revokee List */
    ellPrivList       * mRevokePrivList;     /**< Revoke Privilege List */

    /*
     * Revoke Option
     */
    
    stlBool             mGrantOption;        /**< GRANT OPTION FOR */
    qlpRevokeBehavior   mRevokeBehavior;     /**< revoke behavior */
    
} qlaInitRevokePriv;


stlStatus qlaValidateRevokePriv( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 stlChar       * aSQLString,
                                 qllNode       * aParseTree,
                                 qllEnv        * aEnv );

stlStatus qlaOptCodeRevokePriv( smlTransId      aTransID,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv );

stlStatus qlaOptDataRevokePriv( smlTransId      aTransID,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv );

stlStatus qlaExecuteRevokePriv( smlTransId      aTransID,
                                smlStatement  * aStmt,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv );

/*
 * Validation 보조 함수
 */

stlStatus qlaGetRevokablePrivActionList( smlTransId       aTransID,
                                         smlStatement   * aStmt,
                                         qllDBCStmt     * aDBCStmt,
                                         stlChar        * aSQLString,
                                         ellPrivObject    aPrivObject,
                                         ellDictHandle  * aObjectHandle,
                                         stlBool          aAccessControl,
                                         stlBool          aGrantOption,
                                         ellObjectList  * aRevokeeList,
                                         qlpList        * aParseActionList,
                                         ellPrivList   ** aPrivActionList,
                                         qllEnv         * aEnv );


/*
 * Execution 보조 함수
 */

stlStatus qlaRevokeDependentPriv( smlTransId          aTransID,
                                  smlStatement      * aStmt,
                                  qlpRevokeBehavior   aRevokeBehavior,
                                  stlBool             aGrantOption,
                                  ellPrivDesc       * aRevokePriv,
                                  knlRegionMem      * aRegionMem,
                                  qllEnv            * aEnv );

                                  
#endif /* _QLA_REVOKE_PRIV_H_ */
