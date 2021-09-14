/*******************************************************************************
 * qlaAlterProfile.c
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
 * @file qlaAlterProfile.c
 * @brief ALTER PROFILE 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qlaAlterProfile ALTER PROFILE
 * @ingroup qla
 * @{
 */

/**
 * @brief ALTER PROFILE 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlaValidateAlterProfile( smlTransId      aTransID,
                                   qllDBCStmt    * aDBCStmt,
                                   qllStatement  * aSQLStmt,
                                   stlChar       * aSQLString,
                                   qllNode       * aParseTree,
                                   qllEnv        * aEnv )
{
    qlpAlterProfile     * sParseTree = NULL;
    qlaInitAlterProfile * sInitPlan = NULL;

    stlBool              sObjectExist = STL_FALSE;

    qlpListElement           * sListElem = NULL;
    qlpPasswordParameter     * sParseParam = NULL;
    qlaInitPasswordParameter * sInitParam = NULL;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_PROFILE_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_PROFILE_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */

    sParseTree = (qlpAlterProfile *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlaInitAlterProfile),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlaInitAlterProfile) );

    STL_TRY( qlvSetInitPlan( aDBCStmt,
                             aSQLStmt,
                             & sInitPlan->mCommonInit,
                             aEnv )
             == STL_SUCCESS );

    /*************************************************
     * Profile name 검증 
     *************************************************/

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

    STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_PROFILE_NOT_EXIST );
    
    /**
     * ALTER PROFILE ON DATABASE 권한 검사 
     */    
    
    STL_TRY( qlaCheckDatabasePriv( aTransID,
                                   aDBCStmt,
                                   aSQLStmt,
                                   ELL_DB_PRIV_ACTION_ALTER_PROFILE,
                                   aEnv )
             == STL_SUCCESS );

    /********************************************************
     * Profile Parameter Validation
     ********************************************************/

    sInitPlan->mHasLifeTime = STL_FALSE;
    sInitPlan->mPasswordParameterList = NULL;
    
    QLP_LIST_FOR_EACH( sParseTree->mPasswordParameters, sListElem )
    {
        sParseParam = (qlpPasswordParameter *) QLP_LIST_GET_POINTER_VALUE( sListElem );

        /**
         * Parameter Validation
         */
        
        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                    STL_SIZEOF(qlaInitPasswordParameter),
                                    (void **) & sInitParam,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlMemset( sInitParam, 0x00, STL_SIZEOF(qlaInitPasswordParameter) );

        STL_TRY( qlaValidateUserParam( aDBCStmt,
                                       aSQLStmt,
                                       aSQLString,
                                       sParseParam,
                                       STL_FALSE, /* aUndefParam */
                                       sInitParam,
                                       aEnv )
                 == STL_SUCCESS );

        /**
         * DEFAULT profile 은 DEFAULT 값을 사용할 수 없다.
         */
        
        if ( sInitParam->mIsDefault == STL_TRUE )
        {
            STL_TRY_THROW( ellGetProfileID( & sInitPlan->mProfileHandle )
                           != ellGetDefaultProfileID(),
                           RAMP_ERR_DEFAULT_PROFILE_DEFAULT );
        }

        /**
         * Parameter 연결
         */
        
        if ( sInitPlan->mPasswordParameterList == NULL )
        {
            sInitPlan->mPasswordParameterList = sInitParam;
        }
        else
        {
            sInitParam->mNext = sInitPlan->mPasswordParameterList;
            sInitPlan->mPasswordParameterList = sInitParam;
        }

        if ( sInitParam->mParameterType == ELL_PROFILE_PARAM_PASSWORD_LIFE_TIME )
        {
            sInitPlan->mHasLifeTime = STL_TRUE;
        }
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

    STL_CATCH( RAMP_ERR_DEFAULT_PROFILE_DEFAULT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CANNOT_ASSIGN_DEFAULT_VALUE_TO_DEFAULT_PROFILE,
                      qlgMakeErrPosString( aSQLString,
                                           sParseParam->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief ALTER PROFILE 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlaOptCodeAlterProfile( smlTransId      aTransID,
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
 * @brief ALTER PROFILE 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlaOptDataAlterProfile( smlTransId      aTransID,
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
stlStatus qlaExecuteAlterProfile( smlTransId      aTransID,
                                  smlStatement  * aStmt,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                   qllEnv        * aEnv )
{
    qlaInitAlterProfile  * sInitPlan = NULL;
    
    stlTime               sTime = 0;
    stlBool               sLocked = STL_TRUE;

    ellObjectList       * sUserList = NULL;
    ellObjectItem       * sUserItem = NULL;
    ellDictHandle       * sUserHandle = NULL;

    qlaInitPasswordParameter * sInitParam = NULL;
    qlaInitPasswordParameter * sLifeTimeParam = NULL;

    ellDictHandle  sDefaultHandle;
    stlBool        sObjectExist = STL_FALSE;
    
    dtlDataValue * sLifeTimeValue = NULL;
    
    ellAuthenInfo       sUserAuthenInfo;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_PROFILE_TYPE,
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

    sInitPlan = (qlaInitAlterProfile *) aSQLStmt->mInitPlan;


    /**
     * DDL Lock 획득 
     */

    STL_TRY( ellLock4AlterProfile( aTransID,
                                   aStmt,
                                   & sInitPlan->mProfileHandle,
                                   & sLocked,
                                   ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    /**
     * PASSWORD_LIFE_TIME 의 변경은 User Authentication 정보 변경을 필요로 함
     */

    if ( sInitPlan->mHasLifeTime == STL_TRUE )
    {
        /**
         * Profile 사용하는 User 정보 획득
         */
        
        STL_TRY( ellGetUserList4AlterProfile( aTransID,
                                              aStmt,
                                              & sInitPlan->mProfileHandle,
                                              & QLL_EXEC_DDL_MEM(aEnv),
                                              & sUserList,
                                              ELL_ENV(aEnv) )
                 == STL_SUCCESS );

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
        
        STL_TRY( ellGetUserList4AlterProfile( aTransID,
                                              aStmt,
                                              & sInitPlan->mProfileHandle,
                                              & QLL_EXEC_DDL_MEM(aEnv),
                                              & sUserList,
                                              ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    /**
     * Valid Plan 을 다시 획득
     */

    STL_TRY( qlgGetValidPlan4DDL( aTransID,
                                  aDBCStmt,
                                  aSQLStmt,
                                  QLL_PHASE_EXECUTE,
                                  aEnv )
             == STL_SUCCESS );

    sInitPlan = (qlaInitAlterProfile *) aSQLStmt->mInitPlan;

    /***************************************************************
     * Parameter 정보 변경
     ***************************************************************/

    sLifeTimeParam = NULL;
    
    for( sInitParam =  sInitPlan->mPasswordParameterList;
         sInitParam != NULL;
         sInitParam = sInitParam->mNext )
    {
        switch( sInitParam->mParameterType )
        {
            case ELL_PROFILE_PARAM_FAILED_LOGIN_ATTEMPTS:
            case ELL_PROFILE_PARAM_PASSWORD_REUSE_MAX:
                {
                    STL_TRY( ellModifyProfileParamInteger( aTransID,
                                                           aStmt,
                                                           & sInitPlan->mProfileHandle,
                                                           sInitParam->mParameterType,
                                                           sInitParam->mIsDefault,
                                                           sInitParam->mIntegerValue,
                                                           sInitParam->mValueString,
                                                           ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                    break;
                }
            case ELL_PROFILE_PARAM_PASSWORD_LOCK_TIME:
            case ELL_PROFILE_PARAM_PASSWORD_LIFE_TIME:
            case ELL_PROFILE_PARAM_PASSWORD_GRACE_TIME:
            case ELL_PROFILE_PARAM_PASSWORD_REUSE_TIME:
                {
                    STL_TRY( ellModifyProfileParamIntervalNumber( aTransID,
                                                                  aStmt,
                                                                  & sInitPlan->mProfileHandle,
                                                                  sInitParam->mParameterType,
                                                                  sInitParam->mIsDefault,
                                                                  sInitParam->mIntervalValue,
                                                                  sInitParam->mValueString,
                                                                  ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                    
                    if ( sInitParam->mParameterType == ELL_PROFILE_PARAM_PASSWORD_LIFE_TIME )
                    {
                        sLifeTimeParam = sInitParam;
                    }
                    
                    break;
                }
            case ELL_PROFILE_PARAM_PASSWORD_VERIFY_FUNCTION:
                {
                    STL_TRY( ellModifyProfileParamVerifyFunction( aTransID,
                                                                  aStmt,
                                                                  & sInitPlan->mProfileHandle,
                                                                  sInitParam->mIsDefault,
                                                                  sInitParam->mIntegerValue,
                                                                  sInitParam->mValueString,
                                                                  ELL_ENV(aEnv) )
                             == STL_SUCCESS );

                    break;
                }
            default:
                {
                    STL_DASSERT(0);
                    break;
                }
        }
    }

    /***************************************************************
     * PASSWORD_LIFE_TIME 의 영향을 받는 User Authentication 정보 변경
     ***************************************************************/

    
    if ( sInitPlan->mHasLifeTime == STL_TRUE )
    {
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
                /**
                 * PASSWORD_LIFE_TIME 에 해당하는 Number Interval 값 획득
                 */
                
                if ( sLifeTimeParam->mIsDefault == STL_TRUE )
                {
                    STL_TRY( ellGetProfileDictHandleByID( aTransID,
                                                          aStmt->mScn,
                                                          ellGetDefaultProfileID(),
                                                          & sDefaultHandle,
                                                          & sObjectExist,
                                                          ELL_ENV(aEnv) )
                             == STL_SUCCESS );

                    sLifeTimeValue = 
                        ellGetProfileParamNumberInterval( & sDefaultHandle,
                                                          ELL_PROFILE_PARAM_PASSWORD_LIFE_TIME);
                }
                else
                {
                    sLifeTimeValue = sLifeTimeParam->mIntervalValue;
                }

                /**
                 * Expiry Time 계산
                 */
                
                if ( DTL_IS_NULL( sLifeTimeValue ) == STL_TRUE )
                {
                    sUserAuthenInfo.mExpiryTime = ELL_DICT_STLTIME_NA;
                }
                else
                {
                    STL_TRY( dtlFuncStlTimeAddNumber( sTime,
                                                      sLifeTimeValue,
                                                      & sUserAuthenInfo.mExpiryTime,
                                                      KNL_DT_VECTOR(aEnv),
                                                      aEnv,
                                                      QLL_ERROR_STACK(aEnv) )
                             == STL_SUCCESS );
                }

                STL_TRY( ellModifyUserAuthentication4DDL( aTransID,
                                                          aStmt,
                                                          sUserHandle,
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
                 * Authentication 변경은 User Handle 재구성이 필요없음
                 */
            }
            else
            {
                /**
                 * EXPIRED 상태 유지
                 */
            }
        }
    }

    /***************************************************************
     * Dictionary Cache 재구성 
     ***************************************************************/

    STL_TRY( ellRebuildProfileCache( aTransID,
                                     aStmt,
                                     & sInitPlan->mProfileHandle,
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/** @} qlaAlterProfile */
