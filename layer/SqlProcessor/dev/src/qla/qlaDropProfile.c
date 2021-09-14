/*******************************************************************************
 * qlaDropProfile.c
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

/**
 * @file qlaDropProfile.c
 * @brief DROP PROFILE 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qlaDropProfile DROP PROFILE
 * @ingroup qla
 * @{
 */

/**
 * @brief DROP PROFILE 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlaValidateDropProfile( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  stlChar       * aSQLString,
                                  qllNode       * aParseTree,
                                  qllEnv        * aEnv )
{
    qlpDropProfile     * sParseTree = NULL;
    qlaInitDropProfile * sInitPlan = NULL;

    stlInt32             sState = 0;
    smlStatement       * sStmt = NULL;
    stlInt32             sStmtAttr = 0;

    stlChar            * sProfileName = NULL;

    stlBool              sObjectExist = STL_FALSE;

    ellObjectList      * sUserList = NULL;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_DROP_PROFILE_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_DROP_PROFILE_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */

    sParseTree = (qlpDropProfile *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlaInitDropProfile),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlaInitDropProfile) );

    STL_TRY( qlvSetInitPlan( aDBCStmt,
                             aSQLStmt,
                             & sInitPlan->mCommonInit,
                             aEnv )
             == STL_SUCCESS );

    /**
     * Statement 할당
     */
    
    sStmtAttr = SML_STMT_ATTR_READONLY;
    
    STL_TRY( smlAllocStatement( aTransID,
                                NULL, /* aUpdateRelHandle */
                                sStmtAttr,
                                SML_LOCK_TIMEOUT_INVALID,
                                STL_TRUE, /* aNeedSnapshotIterator */
                                & sStmt,
                                SML_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 1;

    aSQLStmt->mViewSCN = sStmt->mScn;

    /*************************************************
     * Profile name 검증 
     *************************************************/

    /**
     * Profile Name 정보 획득
     */

    sProfileName = QLP_GET_PTR_VALUE(sParseTree->mProfileName);
    
    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                stlStrlen( sProfileName ) + 1,
                                (void **) & sInitPlan->mProfileName,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlStrcpy( sInitPlan->mProfileName, sProfileName );

    /**
     * mIfExists
     */

    sInitPlan->mIfExists = sParseTree->mIfExists;

    /**
     * Profile 존재 여부
     */

    STL_TRY( ellGetProfileDictHandleByName( aTransID,
                                            aSQLStmt->mViewSCN,
                                            QLP_GET_PTR_VALUE( sParseTree->mProfileName ),
                                            & sInitPlan->mProfileHandle,
                                            & sObjectExist,
                                            ELL_ENV(aEnv) )
             == STL_SUCCESS );

    if ( sObjectExist == STL_TRUE )
    {
        /**
         * Profile 가 존재함
         */
        
        sInitPlan->mProfileExist = STL_TRUE;
    }
    else
    {
        sInitPlan->mProfileExist = STL_FALSE;

        /**
         * If Exists 옵션이 있다면 무조건 SUCCESS 한다.
         */
        
        if ( sInitPlan->mIfExists == STL_TRUE )
        {
            STL_THROW( RAMP_IF_EXISTS_NO_PROFILE );
        }
        else
        {
            STL_THROW( RAMP_ERR_PROFILE_NOT_EXIST );
        }
    }

    /**
     * DROP PROFILE ON DATABASE 권한 검사 
     */    
    
    STL_TRY( qlaCheckDatabasePriv( aTransID,
                                   aDBCStmt,
                                   aSQLStmt,
                                   ELL_DB_PRIV_ACTION_DROP_PROFILE,
                                   aEnv )
             == STL_SUCCESS );

    /*************************************************
     * 해당 Profile을 사용하는 User가 존재하는 경우 
     *************************************************/

    sInitPlan->mIsCascade = sParseTree->mIsCascade;

    /**
     * Profile 사용하는 User 정보 획득
     */

    STL_TRY( ellGetUserList4DropProfile( aTransID,
                                         sStmt,
                                         & sInitPlan->mProfileHandle,
                                         QLL_INIT_PLAN(aDBCStmt),
                                         & sUserList,
                                         ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * CASCADE 검증
     */

    if( sInitPlan->mIsCascade == STL_TRUE )
    {
        /**
         * nothing to do
         */
    }
    else
    {
        /**
         * Profile을 사용하는 User 가 존재하지 않아야 함
         */

        STL_TRY_THROW( ellHasObjectItem( sUserList ) == STL_FALSE,
                       RAMP_ERR_CANNOT_DROP_PROFILE_WITHOUT_CASCADE );
    }

    /********************************************************
     * Validation Object 추가 
     ********************************************************/

    STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                  & sInitPlan->mProfileHandle,
                                  QLL_INIT_PLAN(aDBCStmt),
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    
    /*************************************************
     * Plan 연결 
     *************************************************/

    /**
     * IF EXISTS 구문이 있을 경우, Profile 이 존재하지 않는다면,
     * 수행없이 SUCCESS 하게 되고, 권한 검사 및 Writable 은 검사하지 않는다.
     */
    
    STL_RAMP( RAMP_IF_EXISTS_NO_PROFILE );

    /**
     * Statement 해제
     */

    sState = 0;
    STL_TRY( smlFreeStatement( sStmt, SML_ENV(aEnv) ) == STL_SUCCESS );
    
    /**
     * Init Plan 연결 
     */

    aSQLStmt->mInitPlan = (void *) sInitPlan;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_PROFILE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_PROFILE_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mProfileName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      QLP_GET_PTR_VALUE( sParseTree->mProfileName ) );
    }

    STL_CATCH( RAMP_ERR_CANNOT_DROP_PROFILE_WITHOUT_CASCADE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CANNOT_DROP_PROFILE_WITHOUT_CASCADE,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mProfileName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      QLP_GET_PTR_VALUE( sParseTree->mProfileName ) );
    }
    
    STL_FINISH;

    switch (sState)
    {
        case 1:
            (void) smlFreeStatement( sStmt, SML_ENV(aEnv) );
        default:
            break;
    }
    
    
    return STL_FAILURE;
}

/**
 * @brief DROP PROFILE 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlaOptCodeDropProfile( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  qllEnv        * aEnv )
{
    /**
     * nothing to do
     */
    
    return STL_SUCCESS;
}

/**
 * @brief DROP PROFILE 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlaOptDataDropProfile( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  qllEnv        * aEnv )
{
    /**
     * nothing to do
     */
    
    return STL_SUCCESS;
}

/**
 * @brief CREATE Profile 구문을 Execute 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aStmt      Statement
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlaExecuteDropProfile( smlTransId      aTransID,
                                 smlStatement  * aStmt,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv )
{
    qlaInitDropProfile  * sInitPlan = NULL;
    stlTime               sTime = 0;
    stlBool               sLocked = STL_TRUE;

    ellObjectList       * sUserList = NULL;
    ellObjectItem       * sUserItem = NULL;
    ellDictHandle       * sUserHandle = NULL;

    stlInt64      sDefaultProfID = ELL_DICT_OBJECT_ID_NA;
    ellDictHandle sDefaultProfHandle;
    stlBool       sObjectExist = STL_FALSE;

    ellUserProfileParam sProfileParam;
    ellAuthenInfo       sUserAuthenInfo;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_DROP_PROFILE_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mInitPlan != NULL, QLL_ERROR_STACK(aEnv) );

    /***************************************************************
     * 기본 정보 설정 
     ***************************************************************/
    
    /**
     * DDL 수행 시간 설정
     */

    sTime = stlNow();
    STL_TRY( ellBeginStmtDDL( aStmt, sTime, ELL_ENV(aEnv) ) == STL_SUCCESS );

    /***************************************************************
     *  Run-Time Validation
     ***************************************************************/

    STL_RAMP( RAMP_RETRY );
    STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );

    /**
     * Valid Plan 획득
     */

    STL_TRY( qlgGetValidPlan4DDL( aTransID,
                                  aDBCStmt,
                                  aSQLStmt,
                                  QLL_PHASE_EXECUTE,
                                  aEnv )
             == STL_SUCCESS );

    sInitPlan = (qlaInitDropProfile *) aSQLStmt->mInitPlan;

    /**
     * IF EXISTS 옵션 처리
     */

    if ( (sInitPlan->mIfExists == STL_TRUE) &&
         (sInitPlan->mProfileExist != STL_TRUE) )
    {
        /**
         * IF EXISTS 옵션을 사용하고, Profile 이 존재하지 않는 경우
         */
        
        STL_THROW( RAMP_IF_EXISTS_SUCCESS );
    }
    else
    {
        /* go go */
    }

    /**
     * DDL Lock 획득 
     */

    STL_TRY( ellLock4DropProfile( aTransID,
                                  aStmt,
                                  & sInitPlan->mProfileHandle,
                                  & sLocked,
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );

    /*************************************************
     * 해당 Profile을 사용하는 User 정보 획득 및 Lock 획득 
     *************************************************/

    /**
     * Profile 사용하는 User 정보 획득
     */

    STL_TRY( ellGetUserList4DropProfile( aTransID,
                                         aStmt,
                                         & sInitPlan->mProfileHandle,
                                         & QLL_EXEC_DDL_MEM(aEnv),
                                         & sUserList,
                                         ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * CASCADE 검증
     */

    if( sInitPlan->mIsCascade == STL_TRUE )
    {
        /**
         * DEFAULT profile 정보 획득
         */
        
        sDefaultProfID = ellGetDefaultProfileID();

        STL_TRY( ellGetProfileDictHandleByID( aTransID,
                                              aStmt->mScn,
                                              sDefaultProfID,
                                              & sDefaultProfHandle,
                                              & sObjectExist,
                                              ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        STL_DASSERT( sObjectExist == STL_TRUE );

        /**
         * DEFAULT profile 에 S lock 획득
         */
        
        STL_TRY( ellLogicalObjectLock( aTransID,
                                       aStmt,
                                       & sDefaultProfHandle,
                                       SML_LOCK_S,
                                       & sLocked,
                                       ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    }
    else
    {
        /**
         * Profile을 사용하는 User 가 존재하지 않아야 함
         */

        STL_TRY_THROW( ellHasObjectItem( sUserList ) == STL_FALSE,
                       RAMP_ERR_CANNOT_DROP_PROFILE_WITHOUT_CASCADE );
    }

    /**
     * User List 에 X lock 획득
     */

    STL_RING_FOREACH_ENTRY( & sUserList->mHeadList, sUserItem )
    {
        sUserHandle = & sUserItem->mObjectHandle;

        STL_TRY( ellLock4AlterUser( aTransID,
                                    aStmt,
                                    sUserHandle,
                                    & sLocked,
                                    ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    }

    /**
     * Lock 획득 후 User 정보를 다시 획득
     */

    STL_TRY( ellGetUserList4DropProfile( aTransID,
                                         aStmt,
                                         & sInitPlan->mProfileHandle,
                                         & QLL_EXEC_DDL_MEM(aEnv),
                                         & sUserList,
                                         ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Valid Plan 을 다시 획득
     */

    STL_TRY( qlgGetValidPlan4DDL( aTransID,
                                  aDBCStmt,
                                  aSQLStmt,
                                  QLL_PHASE_EXECUTE,
                                  aEnv )
             == STL_SUCCESS );

    sInitPlan = (qlaInitDropProfile *) aSQLStmt->mInitPlan;


    /***************************************************************
     * User에서 Profile 정보 삭제 및 Default Profile 설정 
     ***************************************************************/

    if ( ellHasObjectItem( sUserList ) == STL_TRUE )
    {
        /**
         * DEFAULT 를 기준으로 User Profile Param 획득
         */
        
        STL_TRY( ellGetUserProfileParam( aTransID,
                                         aStmt,
                                         & sDefaultProfHandle,
                                         & sProfileParam,
                                         ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        /**
         * User List 의 Profile 을 DEFAULT profile 로 변경
         */
        
        STL_RING_FOREACH_ENTRY( & sUserList->mHeadList, sUserItem )
        {
            sUserHandle = & sUserItem->mObjectHandle;

            /**
             * User Authentication 정보 획득
             */
            
            STL_TRY( ellGetUserAuthenticationInfo( aTransID,
                                                   aStmt,
                                                   sUserHandle,
                                                   & sUserAuthenInfo,
                                                   ELL_ENV(aEnv) )
                     == STL_SUCCESS );

            /**
             * User 의 EXPIRY_DATE 변경
             */
            
            if ( sUserAuthenInfo.mExpiryStatus == ELL_USER_EXPIRY_STATUS_OPEN )
            {
                if ( DTL_IS_NULL( sProfileParam.mLifeTime ) == STL_TRUE )
                {
                    sUserAuthenInfo.mExpiryTime = ELL_DICT_STLTIME_NA;
                }
                else
                {
                    STL_TRY( dtlFuncStlTimeAddNumber( sTime,
                                                      sProfileParam.mLifeTime,
                                                      & sUserAuthenInfo.mExpiryTime,
                                                      KNL_DT_VECTOR(aEnv),
                                                      aEnv,
                                                      QLL_ERROR_STACK(aEnv) )
                             == STL_SUCCESS );
                }
            }
            else
            {
                /**
                 * EXPIRED 상태 유지
                 */
            }

            /**
             * User Profile Dictionary 변경
             */
            
            STL_TRY( ellModifyUserProfile( aTransID,
                                           aStmt,
                                           sUserHandle,
                                           sDefaultProfID,
                                           & sUserAuthenInfo,
                                           ELL_ENV(aEnv) )
                     == STL_SUCCESS );

            /**
             * Authorization 변경 시간 설정
             */
            
            STL_TRY( ellSetTimeAlterAuth( aTransID,
                                          aStmt,
                                          ellGetAuthID( sUserHandle ),
                                          ELL_ENV(aEnv) )
                     == STL_SUCCESS );
    
            /**
             * User Dictionary Cache 재구성 
             */

            STL_TRY( ellRefineCache4AlterUser( aTransID,
                                               aStmt,
                                               sUserHandle,
                                               ELL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
    }
    else
    {
        /* 관련 user list 가 없음 */
    }
    
    /***************************************************************
     * Profile과 관련된 Dictionary 제거 
     ***************************************************************/

    STL_TRY( ellRemoveDict4DropProfile( aTransID,
                                        aStmt,
                                        & sInitPlan->mProfileHandle,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /***************************************************************
     * Dictionary Cache 재구성 
     ***************************************************************/

    STL_TRY( ellRefineCache4DropProfile( aTransID,
                                         & sInitPlan->mProfileHandle,
                                         ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * If Exists 옵션을 사용하고, Profile 이 존재하지 않는 경우
     */

    STL_RAMP( RAMP_IF_EXISTS_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CANNOT_DROP_PROFILE_WITHOUT_CASCADE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CANNOT_DROP_PROFILE_WITHOUT_CASCADE,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      sInitPlan->mProfileName );
    }
    
    STL_FINISH;

    return STL_FAILURE;

}

/** @} qlaDropProfile */
