/*******************************************************************************
 * qlaCreateProfile.c
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
 * @file qlaCreateProfile.c
 * @brief CREATE PROFILE 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qlaCreateProfile CREATE PROFILE
 * @ingroup qla
 * @{
 */

/**
 * @brief CREATE PROFILE 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlaValidateCreateProfile( smlTransId      aTransID,
                                    qllDBCStmt    * aDBCStmt,
                                    qllStatement  * aSQLStmt,
                                    stlChar       * aSQLString,
                                    qllNode       * aParseTree,
                                    qllEnv        * aEnv )
{
    qlpProfileDef            * sParseTree;
    qlaInitCreateProfile     * sInitPlan = NULL;
    stlChar                  * sProfileName = NULL;
    ellDictHandle              sProfileHandle;
    stlBool                    sProfileExist = STL_FALSE;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_CREATE_PROFILE_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_CREATE_PROFILE_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */

    sParseTree = (qlpProfileDef *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlaInitCreateProfile),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlaInitCreateProfile) );

    STL_TRY( qlvSetInitPlan( aDBCStmt,
                             aSQLStmt,
                             & sInitPlan->mCommonInit,
                             aEnv )
             == STL_SUCCESS );

    /**********************************************************
     * Profile Validation
     **********************************************************/

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
     * Profile 존재 여부 확인
     */

    STL_TRY( ellGetProfileDictHandleByName( aTransID,
                                            aSQLStmt->mViewSCN,
                                            sInitPlan->mProfileName,
                                            & sProfileHandle,
                                            & sProfileExist,
                                            ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sProfileExist == STL_FALSE, RAMP_ERR_SAME_NAME_PROFILE_EXISTS );

    /**
     * CREATE PROFILE ON DATABASE 권한 검사
     */

    STL_TRY( qlaCheckDatabasePriv( aTransID,
                                   aDBCStmt,
                                   aSQLStmt,
                                   ELL_DB_PRIV_ACTION_CREATE_PROFILE,
                                   aEnv )
             == STL_SUCCESS );

    /**********************************************************
     * Password Parameter Validation
     **********************************************************/

    /**
     * 해당 타입의 Password Paramter 값을 설정하여 mPasswordParameterList에 추가
     */ 
    
    STL_TRY( qlaAddPasswordParameter( aDBCStmt,
                                      aSQLStmt,
                                      aSQLString,
                                      sParseTree->mPasswordParameters,
                                      ELL_PROFILE_PARAM_FAILED_LOGIN_ATTEMPTS, 
                                      & sInitPlan->mPasswordParameterList, 
                                      aEnv )
             == STL_SUCCESS );

    STL_TRY( qlaAddPasswordParameter( aDBCStmt,
                                      aSQLStmt,
                                      aSQLString,
                                      sParseTree->mPasswordParameters,
                                      ELL_PROFILE_PARAM_PASSWORD_LOCK_TIME, 
                                      & sInitPlan->mPasswordParameterList, 
                                      aEnv )
             == STL_SUCCESS );

    STL_TRY( qlaAddPasswordParameter( aDBCStmt,
                                      aSQLStmt,
                                      aSQLString,
                                      sParseTree->mPasswordParameters,
                                      ELL_PROFILE_PARAM_PASSWORD_LIFE_TIME, 
                                      & sInitPlan->mPasswordParameterList, 
                                      aEnv )
             == STL_SUCCESS );

    STL_TRY( qlaAddPasswordParameter( aDBCStmt,
                                      aSQLStmt,
                                      aSQLString,
                                      sParseTree->mPasswordParameters,
                                      ELL_PROFILE_PARAM_PASSWORD_GRACE_TIME, 
                                      & sInitPlan->mPasswordParameterList, 
                                      aEnv )
             == STL_SUCCESS );

    STL_TRY( qlaAddPasswordParameter( aDBCStmt,
                                      aSQLStmt,
                                      aSQLString,
                                      sParseTree->mPasswordParameters,
                                      ELL_PROFILE_PARAM_PASSWORD_REUSE_TIME, 
                                      & sInitPlan->mPasswordParameterList, 
                                      aEnv )
             == STL_SUCCESS );

    STL_TRY( qlaAddPasswordParameter( aDBCStmt,
                                      aSQLStmt,
                                      aSQLString,
                                      sParseTree->mPasswordParameters,
                                      ELL_PROFILE_PARAM_PASSWORD_REUSE_MAX, 
                                      & sInitPlan->mPasswordParameterList, 
                                      aEnv )
             == STL_SUCCESS );

    STL_TRY( qlaAddPasswordParameter( aDBCStmt,
                                      aSQLStmt,
                                      aSQLString,
                                      sParseTree->mPasswordParameters,
                                      ELL_PROFILE_PARAM_PASSWORD_VERIFY_FUNCTION, 
                                      & sInitPlan->mPasswordParameterList, 
                                      aEnv )
             == STL_SUCCESS );

    /**********************************************************
     * Output 설정 
     **********************************************************/

    /**
     * Init Plan 연결 
     */

    aSQLStmt->mInitPlan = (void *) sInitPlan;
    

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SAME_NAME_PROFILE_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_USED_PROFILE_NAME,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mProfileName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sProfileName );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief CREATE PROFILE 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlaOptCodeCreateProfile( smlTransId      aTransID,
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
 * @brief CREATE PROFILE 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlaOptDataCreateProfile( smlTransId      aTransID,
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
stlStatus qlaExecuteCreateProfile( smlTransId      aTransID,
                                   smlStatement  * aStmt,
                                   qllDBCStmt    * aDBCStmt,
                                   qllStatement  * aSQLStmt,
                                   qllEnv        * aEnv )
{
    qlaInitCreateProfile     * sInitPlan = NULL;
    stlInt64                   sProfileID = ELL_DICT_OBJECT_ID_NA;
    qlaInitPasswordParameter * sCurParam = NULL;
    stlTime                    sTime = 0;

    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_CREATE_PROFILE_TYPE,
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

    sInitPlan = (qlaInitCreateProfile *) aSQLStmt->mInitPlan;

    /***************************************************************
     * Dictionary Record 추가 
     ***************************************************************/

    STL_TRY( ellInsertProfileDesc( aTransID,
                                   aStmt,
                                   & sProfileID,
                                   sInitPlan->mProfileName,
                                   NULL, /* aProfileComment*/
                                   ELL_ENV(aEnv) )
             == STL_SUCCESS );

    for( sCurParam =  sInitPlan->mPasswordParameterList;
         sCurParam != NULL;
         sCurParam = sCurParam->mNext )
    {
        switch( sCurParam->mParameterType )
        {
            case ELL_PROFILE_PARAM_FAILED_LOGIN_ATTEMPTS:
            case ELL_PROFILE_PARAM_PASSWORD_REUSE_MAX:
                
                STL_TRY( ellInsertProfileParamInteger( aTransID,
                                                       aStmt,
                                                       sProfileID,
                                                       sCurParam->mParameterType,
                                                       sCurParam->mIsDefault,
                                                       sCurParam->mIntegerValue,
                                                       sCurParam->mValueString,
                                                       ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            case ELL_PROFILE_PARAM_PASSWORD_LOCK_TIME:
            case ELL_PROFILE_PARAM_PASSWORD_LIFE_TIME:
            case ELL_PROFILE_PARAM_PASSWORD_GRACE_TIME:
            case ELL_PROFILE_PARAM_PASSWORD_REUSE_TIME:

                STL_TRY( ellInsertProfileParamIntervalNumber( aTransID,
                                                              aStmt,
                                                              sProfileID,
                                                              sCurParam->mParameterType,
                                                              sCurParam->mIsDefault,
                                                              sCurParam->mIntervalValue,
                                                              sCurParam->mValueString,
                                                              ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            case ELL_PROFILE_PARAM_PASSWORD_VERIFY_FUNCTION:
                
                STL_TRY( ellInsertProfileParamVerifyFunction( aTransID,
                                                              aStmt,
                                                              sProfileID,
                                                              sCurParam->mIsDefault,
                                                              sCurParam->mIntegerValue,
                                                              sCurParam->mValueString,
                                                              ELL_ENV(aEnv) )
                         == STL_SUCCESS );

                 break;
            default:
                STL_ASSERT(0);
                break;
        }
    }

    /***************************************************************
     * Dictionary Cache 재구성 
     ***************************************************************/

    STL_TRY( ellRefineCache4CreateProfile( aTransID,
                                           aStmt,
                                           sProfileID,
                                           ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * 생성한 객체에 대해 Lock 을 획득
     * 
     * commit -> pending -> trans end (unlock) 과정 중에
     * 해당 객체에 대한 DROP, ALTER 가 수행될 경우를 방지하기 위해
     * 생성한 객체에 대한 lock 을 획득해야 함.
     */
    
    STL_TRY( ellLockRowAfterCreateNonBaseTableObject( aTransID,
                                                      aStmt,
                                                      ELL_OBJECT_PROFILE,
                                                      sProfileID,
                                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;

}

/**
 * @brief 해당 타입의 Password Paramter 값을 설정하여 List에 추가 
 * @param[in]  aDBCStmt                       DBC Statement
 * @param[in]  aSQLStmt                       SQL Statement
 * @param[in]  aSQLString                     SQL String
 * @param[in]  aUserDefPasswordParameterList  User Defined Password Parameter List 
 * @param[in]  aParameterType                 추가해야 할 Password Parameter 타입
 * @param[out] aPasswordParameterList         Password Parameter List 
 * @param[in]  aEnv                           Environment
 * @remarks
 */
stlStatus qlaAddPasswordParameter( qllDBCStmt                * aDBCStmt,
                                   qllStatement              * aSQLStmt,
                                   stlChar                   * aSQLString,
                                   qlpList                   * aUserDefPasswordParameterList,
                                   ellProfileParam             aParameterType, 
                                   qlaInitPasswordParameter ** aPasswordParameterList, 
                                   qllEnv                    * aEnv )
{
    qlaInitPasswordParameter * sNewParam = NULL;
    qlpListElement           * sListElem = NULL;
    qlpPasswordParameter     * sCurUserDefParam;
    stlBool                    sFindUserDefParam = STL_FALSE;
    
    qlaInitPasswordParameter * sCurParam = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aUserDefPasswordParameterList != NULL, QLL_ERROR_STACK(aEnv) );
    

    /**
     * Password Parameter 노드 생성 및 타입 설정 
     */
    
    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlaInitPasswordParameter),
                                (void **) & sNewParam,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sNewParam, 0x00, STL_SIZEOF(qlaInitPasswordParameter) );

    sNewParam->mParameterType = aParameterType;

    /**
     * 사용자 정의 Parameter 찾음    
     */
   
    QLP_LIST_FOR_EACH( aUserDefPasswordParameterList, sListElem )
    {
        sCurUserDefParam = (qlpPasswordParameter *) QLP_LIST_GET_POINTER_VALUE( sListElem );

        if ( sCurUserDefParam->mParameterType == sNewParam->mParameterType )
        {
            sFindUserDefParam = STL_TRUE;
            break;
        }
    }

    /**
     * Parameter Validation
     */
    
    if ( sFindUserDefParam == STL_TRUE )
    {
        STL_TRY( qlaValidateUserParam( aDBCStmt,
                                       aSQLStmt,
                                       aSQLString,
                                       sCurUserDefParam,
                                       STL_FALSE, /* aUndefParam */
                                       sNewParam,
                                       aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( qlaValidateUserParam( aDBCStmt,
                                       aSQLStmt,
                                       aSQLString,
                                       NULL,      /* aParseParam */
                                       STL_TRUE, /* aUndefParam */
                                       sNewParam,
                                       aEnv )
                 == STL_SUCCESS );
    }
    
    /**
     * Password Parameter List 연결 
     */

    if ( *aPasswordParameterList != NULL )
    {
        for ( sCurParam = *aPasswordParameterList;
              sCurParam->mNext != NULL;
              sCurParam = sCurParam->mNext )
        {
        }
        sCurParam->mNext = sNewParam;
    }
    else
    {
        *aPasswordParameterList = sNewParam;
    }
    
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;

}


/**
 * @brief 사용자 정의 Password Parameter 를 Validation 한다
 * @param[in]  aDBCStmt    DBC Statement
 * @param[in]  aSQLStmt    SQL Statement
 * @param[in]  aSQLString  SQL String
 * @param[in]  aParseParam Parse Parameter
 * @param[in]  aUndefParam Undefined parameter
 * @param[out] aInitParam  Init Parameter
 * @param[in]  aEnv        Environment
 * @remarks
 */
stlStatus qlaValidateUserParam( qllDBCStmt               * aDBCStmt,
                                qllStatement             * aSQLStmt,
                                stlChar                  * aSQLString,
                                qlpPasswordParameter     * aParseParam,
                                stlBool                    aUndefParam,
                                qlaInitPasswordParameter * aInitParam,
                                qllEnv                   * aEnv )
{
    stlInt32    sValueLen = 0;
    stlChar   * sValueString = NULL;

    qlvInitTypeDef   sNumberTypeDef;
    stlChar        * sVerifyPolicy = NULL;

    qlpValue  * sValue1 = NULL;
    qlpValue  * sValue2 = NULL;
    stlInt64    sIntegerValue1 = 0;
    stlInt64    sIntegerValue2 = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    if ( aUndefParam == STL_TRUE )
    {
        STL_PARAM_VALIDATE( aParseParam == NULL, QLL_ERROR_STACK(aEnv) );
    }
    else
    {
        STL_PARAM_VALIDATE( aParseParam != NULL, QLL_ERROR_STACK(aEnv) );
    }
    STL_PARAM_VALIDATE( aInitParam != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Password Parameter Validation
     */
    
    if ( aUndefParam == STL_TRUE )
    {
        /**
         * Undefined Parameter 인 경우
         */

        /* 이미 설정되어 있어야 함 */
        STL_DASSERT( (aInitParam->mParameterType > ELL_PROFILE_PARAM_NA) &&
                     (aInitParam->mParameterType < ELL_PROFILE_PARAM_MAX) );
        
        aInitParam->mIsDefault = STL_TRUE;

        aInitParam->mIntegerValue = ELL_PROFILE_LIMIT_UNLIMITED;
        aInitParam->mIntervalValue = ellGetNullNumber();
        
        sValueLen = stlStrlen( ELL_PROFILE_LIMIT_STRING_DEFAULT );
        
        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                    sValueLen + 1,
                                    (void **) & sValueString,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );        

        stlMemcpy( sValueString, ELL_PROFILE_LIMIT_STRING_DEFAULT, sValueLen );
        sValueString[sValueLen] = '\0';

        aInitParam->mValueString = sValueString;

        STL_THROW( RAMP_FINISH );
    }

    /**
     * Number Type Definition 설정 
     */

    sNumberTypeDef.mDBType            = DTL_TYPE_NUMBER;
    sNumberTypeDef.mUserTypeName      = NULL;
    sNumberTypeDef.mArgNum1           = DTL_NUMERIC_MAX_PRECISION;
    sNumberTypeDef.mArgNum2           = DTL_SCALE_NA;
    sNumberTypeDef.mStringLengthUnit  = DTL_STRING_LENGTH_UNIT_NA;;
    sNumberTypeDef.mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
    sNumberTypeDef.mNumericRadix      = DTL_NUMERIC_PREC_RADIX_DECIMAL;

    /**
     * Parameter 정보 설정
     */
    
    aInitParam->mParameterType = aParseParam->mParameterType;
    aInitParam->mIsDefault     = STL_FALSE;
    aInitParam->mIntegerValue  = ELL_PROFILE_LIMIT_UNLIMITED;
    aInitParam->mIntervalValue = ellGetNullNumber();
    aInitParam->mValueString   = NULL;

    if ( aParseParam->mIsDEFAULT == STL_TRUE )
    {
        /**
         * parameter DEFAULT 인 경우
         */

        aInitParam->mIsDefault     = STL_TRUE;
        
        sValueLen = stlStrlen( ELL_PROFILE_LIMIT_STRING_DEFAULT );
            
        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                    sValueLen + 1,
                                    (void **) & sValueString,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );        
            
        stlMemcpy( sValueString, ELL_PROFILE_LIMIT_STRING_DEFAULT, sValueLen );
        sValueString[sValueLen] = '\0';
            
        aInitParam->mValueString = sValueString;
            
    }
    else
    {
        aInitParam->mIsDefault     = STL_FALSE;
        
        switch( aParseParam->mParameterType )
        {
            case ELL_PROFILE_PARAM_FAILED_LOGIN_ATTEMPTS:
            case ELL_PROFILE_PARAM_PASSWORD_REUSE_MAX:
                {
                    /**
                     * Integer Value Parameter
                     */
                    
                    if ( aParseParam->mIsUNLIMITED == STL_TRUE )
                    {
                        aInitParam->mIntegerValue = ELL_PROFILE_LIMIT_UNLIMITED;

                        sValueLen = stlStrlen( ELL_PROFILE_LIMIT_STRING_UNLIMITED );
                        
                        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                                    sValueLen + 1,
                                                    (void **) & sValueString,
                                                    KNL_ENV(aEnv) )
                                 == STL_SUCCESS );
                        
                        stlMemcpy( sValueString, ELL_PROFILE_LIMIT_STRING_UNLIMITED, sValueLen );
                        sValueString[sValueLen] = '\0';

                        aInitParam->mValueString = sValueString;
                    }
                    else
                    {
                        STL_TRY( dtlGetIntegerFromString( QLP_GET_PTR_VALUE(aParseParam->mValue),
                                                          stlStrlen( QLP_GET_PTR_VALUE(aParseParam->mValue) ),
                                                          & aInitParam->mIntegerValue,
                                                          QLL_ERROR_STACK(aEnv) )
                                 == STL_SUCCESS );

                        /* 0 보다 큰 양의 정수 */
                        STL_TRY_THROW( aInitParam->mIntegerValue > 0, RAMP_ERR_INVALID_PROFILE_PARAMETER );
                        
                        sValueLen = aParseParam->mValueStrLen;
                        
                        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                                    sValueLen + 1,
                                                    (void **) & sValueString,
                                                    KNL_ENV(aEnv) )
                                 == STL_SUCCESS );
                        
                        stlMemcpy( sValueString, QLP_GET_PTR_VALUE( aParseParam->mValue ), sValueLen );
                        sValueString[sValueLen] = '\0';
                        stlTrimString( sValueString );

                        aInitParam->mValueString = sValueString;
                    }
                    
                    break;
                }
            case ELL_PROFILE_PARAM_PASSWORD_LOCK_TIME:
            case ELL_PROFILE_PARAM_PASSWORD_LIFE_TIME:
            case ELL_PROFILE_PARAM_PASSWORD_GRACE_TIME:
            case ELL_PROFILE_PARAM_PASSWORD_REUSE_TIME:
                {
                    /**
                     * Interval Value Parameter
                     */
                    
                    if ( aParseParam->mIsUNLIMITED == STL_TRUE )
                    {
                        aInitParam->mIntervalValue = ellGetNullNumber();

                        sValueLen = stlStrlen( ELL_PROFILE_LIMIT_STRING_UNLIMITED );
                        
                        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                                    sValueLen + 1,
                                                    (void **) & sValueString,
                                                    KNL_ENV(aEnv) )
                                 == STL_SUCCESS );
                        
                        stlMemcpy( sValueString, ELL_PROFILE_LIMIT_STRING_UNLIMITED, sValueLen );
                        sValueString[sValueLen] = '\0';
                        stlTrimString( sValueString );

                        aInitParam->mValueString = sValueString;
                    }
                    else
                    {
                        /**
                         * Data Value 공간 할당
                         */
                        
                        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                                    STL_SIZEOF( dtlDataValue ),
                                                    (void **) & aInitParam->mIntervalValue,
                                                    KNL_ENV(aEnv) )
                                 == STL_SUCCESS );
                        
                        STL_TRY( qlvGetConstDataValueFromParseNode( aSQLStmt,
                                                                    aSQLString,
                                                                    (qllNode*) aParseParam->mValue,
                                                                    & sNumberTypeDef,
                                                                    & aInitParam->mIntervalValue,
                                                                    QLL_INIT_PLAN(aDBCStmt),
                                                                    aEnv )
                                 == STL_SUCCESS );

                        /**
                         *  Interval Value 범위 체크 (1/86400) ~ 100000
                         */

                        if( QLL_NODE_TYPE( aParseParam->mValue ) == QLL_FUNCTION_TYPE )
                        {   
                            sValue1 = (qlpValue*) QLP_LIST_GET_POINTER_VALUE(
                                ((qlpFunction*)aParseParam->mValue)->mArgs->mHead);

                            sValue2 = (qlpValue*)QLP_LIST_GET_POINTER_VALUE(
                                ((qlpFunction*)aParseParam->mValue)->mArgs->mTail);

                            STL_TRY( dtlGetIntegerFromString(
                                         QLP_GET_PTR_VALUE(sValue1),
                                         stlStrlen( QLP_GET_PTR_VALUE(sValue1) ),
                                         & sIntegerValue1,
                                         QLL_ERROR_STACK(aEnv) )
                                     == STL_SUCCESS );

                            STL_TRY( dtlGetIntegerFromString(
                                         QLP_GET_PTR_VALUE(sValue2),
                                         stlStrlen( QLP_GET_PTR_VALUE(sValue2) ),
                                         & sIntegerValue2,
                                         QLL_ERROR_STACK(aEnv) )
                                     == STL_SUCCESS );
                            
                            STL_TRY_THROW(
                                ( (stlFloat64)sIntegerValue1/(stlFloat64)sIntegerValue2 >=
                                  (stlFloat64)1/(stlFloat64)86400 ) &&
                                ( (stlFloat64)sIntegerValue1/(stlFloat64)sIntegerValue2 <=
                                  (stlFloat64)100000 ),
                                RAMP_ERR_INVALID_PROFILE_PARAMETER );

                            /* function type 인 경우, value length는 function node로부터 얻어옴 */
                            sValueLen = ((qlpFunction*)aParseParam->mValue)->mNodeLen;
                            
                        }
                        else
                        {   
                            STL_TRY( dtlGetIntegerFromString(
                                         QLP_GET_PTR_VALUE(aParseParam->mValue),
                                         stlStrlen( QLP_GET_PTR_VALUE(aParseParam->mValue) ),
                                         & sIntegerValue1,
                                         QLL_ERROR_STACK(aEnv) )
                                     == STL_SUCCESS );

                            
                            STL_TRY_THROW( ( sIntegerValue1 > 0 ) &&
                                           ( sIntegerValue1 <= 100000 ),
                                           RAMP_ERR_INVALID_PROFILE_PARAMETER );

                            sValueLen = aParseParam->mValueStrLen;
                        }
                        
                        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                                    sValueLen + 1,
                                                    (void **) & sValueString,
                                                    KNL_ENV(aEnv) )
                                 == STL_SUCCESS );

                        stlMemcpy( sValueString,
                                   aSQLString + aParseParam->mValue->mNodePos,
                                   sValueLen );
                        sValueString[sValueLen] = '\0';
                        stlTrimString( sValueString );

                        aInitParam->mValueString = sValueString;
                    }
                    
                    break;
                }   
            case ELL_PROFILE_PARAM_PASSWORD_VERIFY_FUNCTION:
                {
                    /**
                     * Verify Function Parameter
                     */

                    if ( aParseParam->mValue == NULL )
                    {
                        sValueLen = stlStrlen( ELL_PROFILE_LIMIT_STRING_NULL );
                        
                        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                                    sValueLen + 1,
                                                    (void **) & sValueString,
                                                    KNL_ENV(aEnv) )
                                 == STL_SUCCESS );
                        
                        stlMemcpy( sValueString, ELL_PROFILE_LIMIT_STRING_NULL, sValueLen );
                        sValueString[sValueLen] = '\0';

                        aInitParam->mValueString = sValueString;

                        aInitParam->mIntegerValue = ELL_DICT_OBJECT_ID_NA;
                    }
                    else
                    {
                        sValueLen = aParseParam->mValueStrLen;
                        
                        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                                    sValueLen + 1,
                                                    (void **) & sValueString,
                                                    KNL_ENV(aEnv) )
                                 == STL_SUCCESS );
                        
                        stlMemcpy( sValueString, QLP_GET_PTR_VALUE( aParseParam->mValue ), sValueLen );
                        sValueString[sValueLen] = '\0';

                        aInitParam->mValueString = sValueString;
                    
                        sVerifyPolicy = QLP_GET_PTR_VALUE(aParseParam->mValue);
                        
                        if ( strcmp( ELL_PROFILE_VERIFY_KISA_STRING, sVerifyPolicy) == 0 )
                        {
                            aInitParam->mIntegerValue = ELL_PROFILE_VERIFY_KISA;
                        }
                        else if ( strcmp( ELL_PROFILE_VERIFY_ORA12_STRING, sVerifyPolicy) == 0 )
                        {
                            aInitParam->mIntegerValue = ELL_PROFILE_VERIFY_ORA12;
                        }
                        else if ( strcmp( ELL_PROFILE_VERIFY_ORA12_STRONG_STRING, sVerifyPolicy) == 0 )
                        {
                            aInitParam->mIntegerValue = ELL_PROFILE_VERIFY_ORA12_STRONG;
                        }
                        else if ( strcmp( ELL_PROFILE_VERIFY_ORA11_STRING, sVerifyPolicy) == 0 )
                        {
                            aInitParam->mIntegerValue = ELL_PROFILE_VERIFY_ORA11;
                        }
                        else if ( strcmp( ELL_PROFILE_VERIFY_ORACLE_STRING, sVerifyPolicy) == 0 )
                        {
                            aInitParam->mIntegerValue = ELL_PROFILE_VERIFY_ORACLE;
                        }
                        else
                        {
                            STL_THROW( RAMP_ERR_PASSWORD_VERIFY_FUNCTION_NOT_FOUND );
                        }
                    }
                    
                    break;
                }
            default:
                {
                    STL_ASSERT(0);
                    break;
                }
        }
    }

    STL_RAMP( RAMP_FINISH );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_PASSWORD_VERIFY_FUNCTION_NOT_FOUND )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_PASSWORD_VERIFY_FUNCTION_NOT_FOUND,
                      qlgMakeErrPosString( aSQLString,
                                           aParseParam->mValue->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sVerifyPolicy );
    }

    STL_CATCH( RAMP_ERR_INVALID_PROFILE_PARAMETER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_PROFILE_PARAMETER,
                      qlgMakeErrPosString( aSQLString,
                                           aParseParam->mValue->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sVerifyPolicy );
    }
    
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Password 를 검증한다.
 * @param[in] aUserName           UserName
 * @param[in] aPlainNewPassword   Plain Text New Password
 * @param[in] aPlainOrgPassword   Plain Text Org Password (nullable)
 * @param[in] aVerifyFuncID       Verify Function ID
 * @param[in] aEnv                Environment
 * @remarks
 */
stlStatus qlaVerifyPassword( stlChar  * aUserName,
                             stlChar  * aPlainNewPassword,
                             stlChar  * aPlainOrgPassword,
                             stlInt64   aVerifyFuncID,
                             qllEnv   * aEnv )
{
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aUserName != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPlainNewPassword != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Verifiy Function 을 이용하여 Password 검증
     */
    
    if ( aVerifyFuncID == ELL_DICT_OBJECT_ID_NA )
    {
        /* nothing to do: verify function 이 없음 */
    }
    else
    {
        if ( aVerifyFuncID > ELL_DICT_OBJECT_ID_NA  )
        {
            /**
             * @todo stored function 이 구현되어야 함
             */
            STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
        }
        else
        {
            STL_DASSERT( aVerifyFuncID >  ELL_PROFILE_VERIFY_MIN );
            
            switch ( aVerifyFuncID )
            {
                case ELL_PROFILE_VERIFY_KISA:
                    {
                        STL_TRY( qlaVerifyPasswordKisa( aUserName,
                                                        aPlainNewPassword,
                                                        aPlainOrgPassword,
                                                        aEnv )
                                 == STL_SUCCESS );
                        break;
                    }
                case ELL_PROFILE_VERIFY_ORA12:
                    {
                        STL_TRY( qlaVerifyPasswordOra12( aUserName,
                                                         aPlainNewPassword,
                                                         aPlainOrgPassword,
                                                         aEnv )
                                 == STL_SUCCESS );
                        
                        break;
                    }
                case ELL_PROFILE_VERIFY_ORA12_STRONG:
                    {
                        STL_TRY( qlaVerifyPasswordOra12Strong( aUserName,
                                                               aPlainNewPassword,
                                                               aPlainOrgPassword,
                                                               aEnv )
                                 == STL_SUCCESS );
                        
                        break;
                    }
                case ELL_PROFILE_VERIFY_ORA11:
                    {
                        STL_TRY( qlaVerifyPasswordOra11( aUserName,
                                                         aPlainNewPassword,
                                                         aPlainOrgPassword,
                                                         aEnv )
                                 == STL_SUCCESS );
                        break;
                    }
                case ELL_PROFILE_VERIFY_ORACLE:
                    {
                        STL_TRY( qlaVerifyPasswordOracle( aUserName,
                                                          aPlainNewPassword,
                                                          aPlainOrgPassword,
                                                          aEnv )
                                 == STL_SUCCESS );
                        break;
                    }
                default:
                    {
                        STL_DASSERT( 0 );
                        break;
                    }
            }
        }
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ELL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      "qlaVerifyPassword()" );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Complexity Check
 * @param[in] aPassword        Password
 * @param[in] aPasswordLen     Password Length
 * @param[in] aLetterCnt       Letter Count 
 * @param[in] aUpperCharCnt    Upper Character Count
 * @param[in] aLowerCharCnt    Lower Character Count
 * @param[in] aDigitCnt        Digit Count
 * @param[in] aSpecialCharCnt  Special Character Count
 * @param[in] aEnv             Environment
 * @remarks
 */
stlStatus qlaPasswordComplexityCheck( stlChar  * aPassword,
                                      stlInt64   aPasswordLen,
                                      stlInt64   aLetterCnt,
                                      stlInt64   aUpperCharCnt,
                                      stlInt64   aLowerCharCnt,
                                      stlInt64   aDigitCnt,
                                      stlInt64   aSpecialCharCnt,
                                      qllEnv   * aEnv )
{
    stlInt64 sPasswordLen = 0;
    stlInt64 sIndex = 0;
    stlInt64 sLetterCnt = 0;
    stlInt64 sUpperCharCnt = 0;
    stlInt64 sLowerCharCnt = 0;
    stlInt64 sDigitCnt = 0;
    stlInt64 sSpecialCharCnt = 0;
    
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aPassword != NULL, QLL_ERROR_STACK(aEnv) );

    /* Password Length Check */
    sPasswordLen = stlStrlen( aPassword );
    
    STL_TRY_THROW( sPasswordLen >= aPasswordLen, RAMP_ERROR_PASSWORD_LENGTH );
    
    for( sIndex = 0; sIndex < sPasswordLen; sIndex++ )
    {
        if ( stlIsalpha( aPassword[sIndex] ) == STL_TRUE )
        {
            sLetterCnt++;
            if ( stlIsupper( aPassword[sIndex] ) == STL_TRUE )
            {
                sUpperCharCnt++;
            }
            else
            {
                sLowerCharCnt++;
            }
        }
        else if ( stlIsdigit( aPassword[sIndex] ) == STL_TRUE )
        {
            sDigitCnt++;
        }
        else
        {
            sSpecialCharCnt++;
        }
    }

    if ( aLetterCnt > 0 )
    {
        STL_TRY_THROW( sLetterCnt >= aLetterCnt, RAMP_ERR_PASSWORD_LETTER_COUNT );
    }
    else
    {
        /* nothing to do */
    }

    if ( aUpperCharCnt > 0 )
    {
        STL_TRY_THROW( sUpperCharCnt >= aUpperCharCnt, RAMP_ERR_PASSWORD_UPPER_CHARACTER_COUNT );
    }
    else
    {
        /* nothing to do */
    }

    if ( aLowerCharCnt > 0 )
    {
        STL_TRY_THROW( sLowerCharCnt >= aLowerCharCnt, RAMP_ERR_PASSWORD_LOWER_CHARACTER_COUNT );
    }
    else
    {
        /* nothing to do */
    }

    if ( aDigitCnt > 0 )
    {
        STL_TRY_THROW( sDigitCnt >= aDigitCnt, RAMP_ERR_PASSWORD_DIGIT_COUNT );
    }
    else
    {
        /* nothing to do */
    }

    if ( aSpecialCharCnt > 0 )
    {
        STL_TRY_THROW( sSpecialCharCnt >= aSpecialCharCnt, RAMP_ERR_PASSWORD_SPECIAL_COUNT );
    }
    else
    {
        /* nothing to do */
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERROR_PASSWORD_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_PASSWORD_LENGTH,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      aPasswordLen );
    }

    STL_CATCH( RAMP_ERR_PASSWORD_LETTER_COUNT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_PASSWORD_LETTER_COUNT,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      aLetterCnt );
    }

    STL_CATCH( RAMP_ERR_PASSWORD_UPPER_CHARACTER_COUNT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_PASSWORD_UPPER_CHARACTER_COUNT,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      aUpperCharCnt );
    }

    STL_CATCH( RAMP_ERR_PASSWORD_LOWER_CHARACTER_COUNT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_PASSWORD_LOWER_CHARACTER_COUNT,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      aLowerCharCnt );
    }

    STL_CATCH( RAMP_ERR_PASSWORD_DIGIT_COUNT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_PASSWORD_DIGIT_COUNT,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      aDigitCnt );
    }

    STL_CATCH( RAMP_ERR_PASSWORD_SPECIAL_COUNT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_PASSWORD_SPECIAL_COUNT,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      aSpecialCharCnt );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Get Reversed Word
 * @param[in]  aWord             Word
 * @param[out] aReversedWord     Reversed Word
 * @remarks
 */
stlStatus  qlaGetReversedWord( stlChar   * aWord,
                               stlChar   * aReversedWord )
{
    stlInt64 sWordLen = 0;
    stlInt64 sIndex = 0;

    sWordLen = stlStrlen( aWord );

    for ( sIndex = 0; sIndex < sWordLen; sIndex++ )
    {
        aReversedWord[sWordLen - sIndex -1] = aWord[sIndex];
    }

    aReversedWord[sWordLen] = '\0';

    return STL_SUCCESS;
}



/**
 * @brief DifferenceCount 갯수만큼 String1 과 String2 가 달라야 한다. 
 * @param[in]  aWord1      Word1
 * @param[in]  aLen1       Word1의 Length
 * @param[in]  aWord2      Word2
 * @param[in]  aLen2       Word2의 Length
 * @remarks
 */
stlInt64  qlaLevenShteinDistance( stlChar  * aWord1,
                                  stlInt64   aLen1,
                                  stlChar  * aWord2,
                                  stlInt64   aLen2 )
{
    stlInt64 sMatrix[aLen1 + 1][aLen2 + 1];
    stlInt64 i = 0;
    stlInt64 j = 0;
    stlChar  sChar1;
    stlChar  sChar2;
    stlInt64 sDelete = 0;
    stlInt64 sInsert = 0;
    stlInt64 sSubstitute = 0;
    stlInt64 sMinimum = 0;
    

    for( i = 0; i <= aLen1; i++ )
    {
        sMatrix[i][0] = i;
    }

    for( i = 0; i <= aLen2; i++ )
    {
        sMatrix[0][i] = i;
    }


    for( i = 1; i <= aLen1; i++ )
    {
        sChar1 = aWord1[i-1];

        for( j = 1; j <= aLen2; j++ )
        {
            sChar2 = aWord2[j-1];

            if ( sChar1 == sChar2 )
            {
                sMatrix[i][j] = sMatrix[i-1][j-1];
            }
            else
            {
                sDelete     = sMatrix[i-1][j] + 1;
                sInsert     = sMatrix[i][j-1] + 1;
                sSubstitute = sMatrix[i-1][j-1] + 1;
                sMinimum    = sDelete;
                
                if ( sInsert < sMinimum )
                {
                    sMinimum = sInsert;
                }
                
                if ( sSubstitute < sMinimum )
                {
                    sMinimum = sSubstitute;
                }
                
                sMatrix[i][j] = sMinimum;
            }
        }
    }
    
    return sMatrix[aLen1][aLen2];
}



/**
 * @brief KISA_VERIFY_FUNCTION 으로 password 를 검증한다.
 * @param[in] aUserName      UserName
 * @param[in] aNewPassword   New Password
 * @param[in] aOrgPassword   Org Password (nullable)
 * @param[in] aEnv           Environment
 * @remarks
 */
stlStatus qlaVerifyPasswordKisa( stlChar * aUserName,
                                 stlChar * aNewPassword,
                                 stlChar * aOrgPassword,
                                 qllEnv  * aEnv )
{
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aUserName != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNewPassword != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * 8 글자 이상
     * 1 개 이상의 문자
     * 1 개 이상의 숫자
     * 1 개 이상의 특수문자
     */

    STL_TRY( qlaPasswordComplexityCheck( aNewPassword,
                                         8, // aPasswordLen,
                                         1, // aLetterCnt,
                                         0, // aUpperCharCnt,
                                         0, // aLowerCharCnt,
                                         1, // aDigitCnt,
                                         1, // aSpecialCharCnt,
                                         aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}



/**
 * @brief ORA12C_VERIFY_FUNCTION 으로 password 를 검증한다.
 * @param[in] aUserName      UserName
 * @param[in] aNewPassword   New Password
 * @param[in] aOrgPassword   Org Password (nullable)
 * @param[in] aEnv           Environment
 * @remarks
 */
stlStatus qlaVerifyPasswordOra12( stlChar * aUserName,
                                  stlChar * aNewPassword,
                                  stlChar * aOrgPassword,
                                  qllEnv  * aEnv )
{
    stlInt64   sDifferenceCnt = 0;
    stlChar    sLowerUserName[STL_MAX_SQL_IDENTIFIER_LENGTH] = {0,};
    stlChar    sReversedUserName[STL_MAX_SQL_IDENTIFIER_LENGTH] = {0,};
    stlChar    sLowerNewPassword[STL_MAX_SQL_IDENTIFIER_LENGTH] = {0,};
    stlChar    sDatabaseName[STL_MAX_SQL_IDENTIFIER_LENGTH] = {0,};
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aUserName != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNewPassword != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * 8 글자 이상
     * 1 개 이상의 문자
     * 1 개 이상의 숫자
     * 1 개 이상의 특수문자
     */

    STL_TRY( qlaPasswordComplexityCheck( aNewPassword,
                                         8, // aPasswordLen,
                                         1, // aLetterCnt,
                                         0, // aUpperCharCnt,
                                         0, // aLowerCharCnt,
                                         1, // aDigitCnt,
                                         0, // aSpecialCharCnt,
                                         aEnv )
             == STL_SUCCESS );

    /**
     * 사용자 이름을 포함하면 안됨 
     */
    
    stlTolowerString( aUserName, sLowerUserName );
    stlTolowerString( aNewPassword, sLowerNewPassword );
    

    STL_TRY_THROW( stlStrstr( sLowerNewPassword, sLowerUserName ) == NULL,
                   RAMP_ERR_PASSWORD_SAME_OR_SIMILAR_TO_USER_NAME);

    /**
     * 거꾸로된 사용자 이름을 포함하면 안됨 
     */

    STL_TRY( qlaGetReversedWord( sLowerUserName, sReversedUserName ) == STL_SUCCESS );

    STL_TRY_THROW( stlStrstr( sLowerNewPassword, sReversedUserName ) == NULL,
                   RAMP_ERR_PASSWORD_CONTAIN_USER_NAME_RESERVED);
    

    /**
     * 데이터베이스 이름을 포함하면 안됨 
     */

    stlTolowerString( ellGetDbCatalogName(), sDatabaseName );

    STL_TRY_THROW( stlStrstr( sLowerNewPassword, sDatabaseName ) == NULL,
                   RAMP_ERR_PASSWORD_SAME_OR_SIMILAR_TO_DATABASE_NAME);

    /**
     * 'goldilocks'를 포함하면 안됨 
     */

    STL_TRY_THROW( stlStrstr( sLowerNewPassword, "goldilocks" ) == NULL,
                   RAMP_ERR_PASSWORD_TOO_SIMPLE );
    

    /**
     * 'oracle'을 포함하면 안됨 
     */

    STL_TRY_THROW( stlStrstr( sLowerNewPassword, "oracle" ) == NULL,
                   RAMP_ERR_PASSWORD_TOO_SIMPLE );

    /**
     * 아래와 같은 단순 패스워드는 사용할 수 없음
     * welcome1, database1, account1, user1234, password1, oracle123, computer1, abcdefg1
     */

    if( ( stlStrcmp( sLowerNewPassword, "welcome1" ) == 0 )  ||
        ( stlStrcmp( sLowerNewPassword, "database1" ) == 0 ) ||
        ( stlStrcmp( sLowerNewPassword, "account1" ) == 0 )  ||
        ( stlStrcmp( sLowerNewPassword, "user1234" ) == 0 )  ||
        ( stlStrcmp( sLowerNewPassword, "password1" ) == 0 ) ||
        ( stlStrcmp( sLowerNewPassword, "oracle123" ) == 0 ) ||
        ( stlStrcmp( sLowerNewPassword, "computer1" ) == 0 ) ||
        ( stlStrcmp( sLowerNewPassword, "abcdefg1" ) == 0 ) )
    {
        STL_THROW( RAMP_ERR_PASSWORD_TOO_SIMPLE );
    }

    /**
     * 이전 Password 와 적어도 3글자는 달라야 함
     */

    if ( aOrgPassword != NULL )
    {
        sDifferenceCnt = qlaLevenShteinDistance( aNewPassword,
                                                 stlStrlen( aNewPassword ),
                                                 aOrgPassword,
                                                 stlStrlen( aOrgPassword ) );
        
        STL_TRY_THROW( sDifferenceCnt >= 3, RAMP_ERR_PASSWORD_DIFFER_FROM_OLD_PASSWORD );
    }
    else
    {
        /* nothing to do */
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_PASSWORD_SAME_OR_SIMILAR_TO_USER_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_PASSWORD_SAME_OR_SIMILAR_TO_USER_NAME,
                      NULL,
                      QLL_ERROR_STACK(aEnv));
    }

    STL_CATCH( RAMP_ERR_PASSWORD_CONTAIN_USER_NAME_RESERVED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_PASSWORD_CONTAIN_USER_NAME_REVERSED,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_PASSWORD_SAME_OR_SIMILAR_TO_DATABASE_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_PASSWORD_SAME_OR_SIMILAR_TO_DATABASE_NAME,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_PASSWORD_TOO_SIMPLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_PASSWORD_TOO_SIMPLE,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_PASSWORD_DIFFER_FROM_OLD_PASSWORD )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_PASSWORD_DIFFER_FROM_OLD_PASSWORD,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      3 );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief ORA12C_STRONG_VERIFY_FUNCTION 으로 password 를 검증한다.
 * @param[in] aUserName      UserName
 * @param[in] aNewPassword   New Password
 * @param[in] aOrgPassword   Org Password (nullable)
 * @param[in] aEnv           Environment
 * @remarks
 */
stlStatus qlaVerifyPasswordOra12Strong( stlChar * aUserName,
                                        stlChar * aNewPassword,
                                        stlChar * aOrgPassword,
                                        qllEnv  * aEnv )
{
    stlInt64 sDifferenceCnt = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aUserName != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNewPassword != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * 9 글자 이상
     * 2 개 이상의 대문자
     * 2 개 이상의 소문자
     * 2 개 이상의 숫자
     * 2 개 이상의 특수문자
     */

    STL_TRY( qlaPasswordComplexityCheck( aNewPassword,
                                         9, // aPasswordLen,
                                         0, // aLetterCnt,
                                         2, // aUpperCharCnt,
                                         2, // aLowerCharCnt,
                                         2, // aDigitCnt,
                                         2, // aSpecialCharCnt,
                                         aEnv )
             == STL_SUCCESS );

    /**
     * 이전 Password 와 적어도 4글자는 달라야 함
     */

    if ( aOrgPassword != NULL )
    {
        sDifferenceCnt = qlaLevenShteinDistance( aNewPassword,
                                                 stlStrlen( aNewPassword ),
                                                 aOrgPassword,
                                                 stlStrlen( aOrgPassword ) );
        
        STL_TRY_THROW( sDifferenceCnt >= 4, RAMP_ERR_PASSWORD_DIFFER_FROM_OLD_PASSWORD );
    }
    else
    {
        /* nothing to do */
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_PASSWORD_DIFFER_FROM_OLD_PASSWORD )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_PASSWORD_DIFFER_FROM_OLD_PASSWORD,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      4 );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief VERIFY_FUNCTION_11G 으로 password 를 검증한다.
 * @param[in] aUserName      UserName
 * @param[in] aNewPassword   New Password
 * @param[in] aOrgPassword   Org Password (nullable)
 * @param[in] aEnv           Environment
 * @remarks
 */
stlStatus qlaVerifyPasswordOra11( stlChar * aUserName,
                                  stlChar * aNewPassword,
                                  stlChar * aOrgPassword,
                                  qllEnv  * aEnv )
{
    stlInt64   sDifferenceCnt = 0;
    stlChar    sLowerUserName[STL_MAX_SQL_IDENTIFIER_LENGTH] = {0,};
    stlChar    sReversedUserName[STL_MAX_SQL_IDENTIFIER_LENGTH] = {0,};
    stlChar    sLowerNewPassword[STL_MAX_SQL_IDENTIFIER_LENGTH] = {0,};
    stlChar    sDatabaseName[STL_MAX_SQL_IDENTIFIER_LENGTH] = {0,};
    stlInt64   sUserNameLen = 0;
    stlInt64   sDatabaseNameLen = 0;
    stlInt64   i;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aUserName != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNewPassword != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * 8 글자 이상
     * 1 개 이상의 문자
     * 1 개 이상의 숫자
     */

    STL_TRY( qlaPasswordComplexityCheck( aNewPassword,
                                         8, // aPasswordLen,
                                         1, // aLetterCnt,
                                         0, // aUpperCharCnt,
                                         0, // aLowerCharCnt,
                                         1, // aDigitCnt,
                                         0, // aSpecialCharCnt,
                                         aEnv )
             == STL_SUCCESS );

    /**
     * 사용자 이름과 같으면 안됨 
     */

    stlTolowerString( aUserName, sLowerUserName );
    stlTolowerString( aNewPassword, sLowerNewPassword );

    STL_TRY_THROW( stlStrcmp( sLowerNewPassword, sLowerUserName ) != 0,
                   RAMP_ERR_PASSWORD_SAME_OR_SIMILAR_TO_USER_NAME);

    /**
     * 거꾸로된 사용자 이름과 같으면 안됨 
     */

    STL_TRY( qlaGetReversedWord( sLowerUserName, sReversedUserName ) == STL_SUCCESS );

    STL_TRY_THROW( stlStrcmp( sLowerNewPassword, sReversedUserName ) != 0,
                   RAMP_ERR_PASSWORD_CONTAIN_USER_NAME_RESERVED);
    

    /**
     * 사용자 이름 + (1~100) 한 값과 같으면 안됨 
     */

    sUserNameLen = stlStrlen( sLowerUserName );

    for ( i = 1; i <= 100; i++ )
    {
        stlSnprintf( sLowerUserName + sUserNameLen,
                     STL_MAX_SQL_IDENTIFIER_LENGTH,
                     "%d",
                     i );

        STL_TRY_THROW( stlStrcmp( sLowerNewPassword, sLowerUserName ) != 0,
                       RAMP_ERR_PASSWORD_SAME_OR_SIMILAR_TO_USER_NAME);
    }

    /**
     * 서버 이름과 같으면 안됨 
     */

    stlTolowerString( ellGetDbCatalogName(), sDatabaseName );

    STL_TRY_THROW( stlStrcmp( sLowerNewPassword, sDatabaseName ) != 0,
                   RAMP_ERR_PASSWORD_SAME_OR_SIMILAR_TO_DATABASE_NAME);

    /**
     * 서버 이름 + (1~100) 한 값과 같으면 안됨 
     */

    sDatabaseNameLen = stlStrlen( sDatabaseName );

    for ( i = 1; i <= 100; i++ )
    {
        stlSnprintf( sDatabaseName + sDatabaseNameLen,
                     STL_MAX_SQL_IDENTIFIER_LENGTH,
                     "%d",
                     i );

        STL_TRY_THROW( stlStrcmp( sLowerNewPassword, sDatabaseName ) != 0,
                       RAMP_ERR_PASSWORD_SAME_OR_SIMILAR_TO_DATABASE_NAME );
    }

     /**
     * 아래와 같은 단순 패스워드는 사용할 수 없음
     * welcome1, database1, account1, user1234, password1, oracle123, computer1, abcdefg1
     */

    if( ( stlStrcmp( sLowerNewPassword, "welcome1" ) == 0 )  ||
        ( stlStrcmp( sLowerNewPassword, "database1" ) == 0 ) ||
        ( stlStrcmp( sLowerNewPassword, "account1" ) == 0 )  ||
        ( stlStrcmp( sLowerNewPassword, "user1234" ) == 0 )  ||
        ( stlStrcmp( sLowerNewPassword, "password1" ) == 0 ) ||
        ( stlStrcmp( sLowerNewPassword, "oracle123" ) == 0 ) ||
        ( stlStrcmp( sLowerNewPassword, "computer1" ) == 0 ) ||
        ( stlStrcmp( sLowerNewPassword, "abcdefg1" ) == 0 ) )
    {
        STL_THROW( RAMP_ERR_PASSWORD_TOO_SIMPLE );
    }

    /**
     * 이전 Password 와 적어도 3글자는 달라야 함
     */

    if ( aOrgPassword != NULL )
    {
        sDifferenceCnt = qlaLevenShteinDistance( aNewPassword,
                                                 stlStrlen( aNewPassword ),
                                                 aOrgPassword,
                                                 stlStrlen( aOrgPassword ) );
        
        STL_TRY_THROW( sDifferenceCnt >= 3, RAMP_ERR_PASSWORD_DIFFER_FROM_OLD_PASSWORD );
    }
    else
    {
        /* nothing to do */
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_PASSWORD_SAME_OR_SIMILAR_TO_USER_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_PASSWORD_SAME_OR_SIMILAR_TO_USER_NAME,
                      NULL,
                      QLL_ERROR_STACK(aEnv));
    }

    STL_CATCH( RAMP_ERR_PASSWORD_CONTAIN_USER_NAME_RESERVED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_PASSWORD_CONTAIN_USER_NAME_REVERSED,
                      NULL,
                      QLL_ERROR_STACK(aEnv));
    }

    STL_CATCH( RAMP_ERR_PASSWORD_SAME_OR_SIMILAR_TO_DATABASE_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_PASSWORD_SAME_OR_SIMILAR_TO_DATABASE_NAME,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_PASSWORD_TOO_SIMPLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_PASSWORD_TOO_SIMPLE,
                      NULL,
                      QLL_ERROR_STACK(aEnv));
    }

    STL_CATCH( RAMP_ERR_PASSWORD_DIFFER_FROM_OLD_PASSWORD )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_PASSWORD_DIFFER_FROM_OLD_PASSWORD,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      3 );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief VERIFY_FUNCTION 으로 password 를 검증한다.
 * @param[in] aUserName      UserName
 * @param[in] aNewPassword   New Password
 * @param[in] aOrgPassword   Org Password (nullable)
 * @param[in] aEnv           Environment
 * @remarks
 */
stlStatus qlaVerifyPasswordOracle( stlChar * aUserName,
                                   stlChar * aNewPassword,
                                   stlChar * aOrgPassword,
                                   qllEnv  * aEnv )
{
    stlInt64   sDifferenceCnt = 0;
    stlChar    sLowerUserName[STL_MAX_SQL_IDENTIFIER_LENGTH] = {0,};
    stlChar    sLowerNewPassword[STL_MAX_SQL_IDENTIFIER_LENGTH] = {0,};
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aUserName != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNewPassword != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * 4 글자 이상
     * 1 개 이상의 문자
     * 1 개 이상의 숫자
     * 1 개 이상의 특수문자
     */

    STL_TRY( qlaPasswordComplexityCheck( aNewPassword,
                                         4, // aPasswordLen,
                                         1, // aLetterCnt,
                                         0, // aUpperCharCnt,
                                         0, // aLowerCharCnt,
                                         1, // aDigitCnt,
                                         1, // aSpecialCharCnt,
                                         aEnv )
             == STL_SUCCESS );

    /**
     * 사용자 이름과 같으면 안됨 
     */

    stlTolowerString( aUserName, sLowerUserName );
    stlTolowerString( aNewPassword, sLowerNewPassword );

    STL_TRY_THROW( stlStrcmp( sLowerNewPassword, sLowerUserName ) != 0,
                   RAMP_ERR_PASSWORD_SAME_OR_SIMILAR_TO_USER_NAME);

    /**
     * 이전 Password 와 적어도 3글자는 달라야 함
     */

    if ( aOrgPassword != NULL )
    {
        sDifferenceCnt = qlaLevenShteinDistance( aNewPassword,
                                                 stlStrlen( aNewPassword ),
                                                 aOrgPassword,
                                                 stlStrlen( aOrgPassword ) );
        
        STL_TRY_THROW( sDifferenceCnt >= 3, RAMP_ERR_PASSWORD_DIFFER_FROM_OLD_PASSWORD );
    }
    else
    {
        /* nothing to do */
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_PASSWORD_SAME_OR_SIMILAR_TO_USER_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_PASSWORD_SAME_OR_SIMILAR_TO_USER_NAME,
                      NULL,
                      QLL_ERROR_STACK(aEnv));
    }

    STL_CATCH( RAMP_ERR_PASSWORD_DIFFER_FROM_OLD_PASSWORD )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_PASSWORD_DIFFER_FROM_OLD_PASSWORD,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      3 );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Password 의 재사용 가능 여부를 검사한다.
 * @param[in] aTransID         Transaction ID
 * @param[in] aStmt            Statement
 * @param[in] aUserHandle      User Handle
 * @param[in] aUserAuthenInfo  User Authentication 정보
 * @param[in] aProfileParam    User Profile Param
 * @param[in] aPlainPassword   Plain Text Password
 * @param[in] aChangeTime      Password Change Time
 * @param[in] aEnv             Environment
 * @remarks
 */
stlStatus qlaCheckPasswordReuse( smlTransId            aTransID,
                                 smlStatement        * aStmt,
                                 ellDictHandle       * aUserHandle,
                                 ellAuthenInfo       * aUserAuthenInfo,
                                 ellUserProfileParam * aProfileParam,
                                 stlChar             * aPlainPassword,
                                 stlTime               aChangeTime,
                                 qllEnv              * aEnv )
{
    stlChar  sEncryptPassword[STL_MAX_SQL_IDENTIFIER_LENGTH] = {0,};
    stlBool  sExist = STL_FALSE;

    stlInt64  sMinChangeNO = 0;
    stlTime   sMinChangeTime = 0;
    stlTime   sDiffTime = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aUserHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aUserAuthenInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aProfileParam != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPlainPassword != NULL, QLL_ERROR_STACK(aEnv) );

    /* REUSE 검사가 필요한 경우에만 호출되어야 함. */

    if ( ( aProfileParam->mReuseMax == ELL_PROFILE_LIMIT_UNLIMITED ) &&
         ( DTL_IS_NULL(aProfileParam->mReuseTime) == STL_TRUE ) )
    {
        /**
         * PASSWORD_REUSE_MAX UNLIMITED
         * PASSWORD_REUSE_TIME UNLIMITED
         */
        
        STL_DASSERT(0);
    }

    /**
     * REUSE 못하는 최소값 계산
     */
    
    if ( aProfileParam->mReuseMax == ELL_PROFILE_LIMIT_UNLIMITED )
    {
        sMinChangeNO = 0;
    }
    else
    {
        sMinChangeNO = aUserAuthenInfo->mPassChangeCount - aProfileParam->mReuseMax + 1;
        if ( sMinChangeNO < 0 )
        {
            sMinChangeNO = 0;
        }
    }

    if ( DTL_IS_NULL(aProfileParam->mReuseTime) == STL_TRUE )
    {
        sMinChangeTime = 0;
    }
    else
    {
        STL_TRY( dtlFuncStlTimeAddNumber( aChangeTime,
                                          aProfileParam->mReuseTime,
                                          & sDiffTime,
                                          KNL_DT_VECTOR(aEnv),
                                          aEnv,
                                          QLL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );

        sMinChangeTime = aChangeTime - ( sDiffTime - aChangeTime );
        if ( sMinChangeTime < 0 )
        {
            sMinChangeTime = 0;
        }
    }
    
    /**
     * Encrypt Password 생성
     */

    STL_TRY( stlMakeEncryptedPassword( aPlainPassword,
                                       stlStrlen( aPlainPassword ),
                                       sEncryptPassword,
                                       STL_MAX_SQL_IDENTIFIER_LENGTH,
                                       QLL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    /**
     * REUSE 조건을 위반하는 Password 가 존재하는 지 검사 
     */

    STL_TRY( ellFindDisableReusePassword( aTransID,
                                          aStmt,
                                          aUserHandle,
                                          sEncryptPassword,
                                          sMinChangeNO,
                                          sMinChangeTime,
                                          & sExist,
                                          ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sExist == STL_FALSE, RAMP_ERR_REUSE_PASSWORD );

    /**
     * Password History 에 추가
     */

    STL_TRY( ellInsertPasswordHistoryDesc( aTransID,
                                           aStmt,
                                           ellGetAuthID( aUserHandle ),
                                           sEncryptPassword,
                                           aUserAuthenInfo->mPassChangeCount + 1,
                                           aChangeTime,
                                           ELL_ENV(aEnv) )
             == STL_SUCCESS );
                                           
    /**
     * Old Password History 를 제거한다.
     */

    if ( ( aProfileParam->mReuseMax != ELL_PROFILE_LIMIT_UNLIMITED) &&
         ( DTL_IS_NULL(aProfileParam->mReuseTime) == STL_FALSE) )
    {
        /**
         * PARAM_REUSE_MAX value
         * PARAM_REUSE_TIME value
         */

        STL_TRY( ellRemoveOldPassword( aTransID,
                                       aStmt,
                                       aUserHandle,
                                       sMinChangeNO,
                                       sMinChangeTime,
                                       ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        /* Old Password History 를 제거할 수 없다. */
    }
                                
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_REUSE_PASSWORD )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_PASSWORD_CANNOT_REUSED,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}



/** @} qlaCreateProfile */
