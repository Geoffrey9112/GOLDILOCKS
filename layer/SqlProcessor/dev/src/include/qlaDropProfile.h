/*******************************************************************************
 * qlaDropProfile.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlaDropProfile.h 5803 2012-09-27 06:40:12Z jhkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/

#ifndef _QLA_DROP_PROFILE_H_
#define _QLA_DROP_PROFILE_H_ 1

/**
 * @file qlaDropProfile.h
 * @brief DROP PROFILE
 */

/**
 * @brief DROP PROFILE 를 위한 Init Plan
 */
typedef struct qlaInitDropProfile
{
    qlvInitPlan      mCommonInit;       /**< Common Init Plan */

    stlChar        * mProfileName;      /**< Profile Name */
    
    stlBool          mIfExists;         /**< IF EXISTS 옵션 사용 여부 */
    stlBool          mProfileExist;     /**< Profile 존재 여부 */
    
    ellDictHandle    mProfileHandle;    /**< Profile Handle */
    
    stlBool          mIsCascade;        /**< CASCADE 여부  */
    
} qlaInitDropProfile;


stlStatus qlaValidateDropProfile( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  stlChar       * aSQLString,
                                  qllNode       * aParseTree,
                                  qllEnv        * aEnv );

stlStatus qlaOptCodeDropProfile( smlTransId      aTransID,
                                   qllDBCStmt    * aDBCStmt,
                                   qllStatement  * aSQLStmt,
                                   qllEnv        * aEnv );

stlStatus qlaOptDataDropProfile( smlTransId      aTransID,
                                   qllDBCStmt    * aDBCStmt,
                                   qllStatement  * aSQLStmt,
                                   qllEnv        * aEnv );

stlStatus qlaExecuteDropProfile( smlTransId      aTransID,
                                   smlStatement  * aStmt,
                                   qllDBCStmt    * aDBCStmt,
                                   qllStatement  * aSQLStmt,
                                   qllEnv        * aEnv );

/*
 * Execution 보조 함수
 */


#endif /* _QLA_DROP_PROFILE_H_ */
