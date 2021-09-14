/*******************************************************************************
 * qlaGrantPriv.h
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

#ifndef _QLA_GRANT_PRIV_H_
#define _QLA_GRANT_PRIV_H_ 1

/**
 * @file qlaGrantPriv.h
 * @brief GRANT PRIVILEGE
 */

/**
 * @brief GRANT PRIVILEGE 를 위한 Init Plan
 */
typedef struct qlaInitGrantPriv
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
     * Grant Privilege 정보 
     */

    ellObjectList     * mGranteeList;        /**< Grantee List */
    
    stlBool             mWithGrant;          /**< WITH GRANT OPTION */
    ellPrivList       * mGrantablePrivList;  /**< Grantable Privilege List */
    ellPrivList       * mTotalPrivList;      /**< Privilege List for all grantee */
    
} qlaInitGrantPriv;


stlStatus qlaValidateGrantPriv( smlTransId      aTransID,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                stlChar       * aSQLString,
                                qllNode       * aParseTree,
                                qllEnv        * aEnv );

stlStatus qlaOptCodeGrantPriv( smlTransId      aTransID,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv );

stlStatus qlaOptDataGrantPriv( smlTransId      aTransID,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv );

stlStatus qlaExecuteGrantPriv( smlTransId      aTransID,
                               smlStatement  * aStmt,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv );

/*
 * Validation 보조 함수
 */


stlStatus qlaGetPrivObject( smlTransId      aTransID,
                            qllDBCStmt    * aDBCStmt,
                            qllStatement  * aSQLStmt,
                            stlChar       * aSQLString,
                            qlpPrivObject * aPrivObject,
                            qlpList       * aGranteeList,
                            ellDictHandle * aObjectHandle,
                            qllEnv        * aEnv );

stlStatus qlaGetGranteeList( smlTransId       aTransID,
                             qllDBCStmt     * aDBCStmt,
                             qllStatement   * aSQLStmt,
                             stlChar        * aSQLString,
                             qlpList        * aNameList,
                             ellObjectList ** aGranteeList,
                             qllEnv         * aEnv );
                             
stlStatus qlaGetGrantablePrivActionList( smlTransId       aTransID,
                                         smlStatement   * aStmt,
                                         qllDBCStmt     * aDBCStmt,
                                         stlChar        * aSQLString,
                                         ellPrivObject    aPrivObject,
                                         ellDictHandle  * aObjectHandle,
                                         stlBool          aAccessControl,
                                         qlpList        * aParseActionList,
                                         ellPrivList   ** aPrivActionList,
                                         qllEnv         * aEnv );
                             
stlStatus qlaGetTotalPrivList( smlTransId       aTransID,
                               qllDBCStmt     * aDBCStmt,
                               stlBool          aWithGrantOption,
                               ellPrivList    * aGrantableList,
                               ellObjectList  * aGranteeList,
                               ellPrivList   ** aTotalPrivList,
                               qllEnv         * aEnv );


/*
 * Execution 보조 함수
 */


#endif /* _QLA_GRANT_PRIV_H_ */
