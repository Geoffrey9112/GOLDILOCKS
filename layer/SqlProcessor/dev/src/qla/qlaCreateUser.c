/*******************************************************************************
 * qlaCreateUser.c
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
 * @file qlaCreateUser.c
 * @brief CREATE USER 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qlaCreateUser CREATE USER
 * @ingroup qla
 * @{
 */


/**
 * @brief CREATE USER 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlaValidateCreateUser( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 stlChar       * aSQLString,
                                 qllNode       * aParseTree,
                                 qllEnv        * aEnv )
{
    stlInt32   sState = 0;
    
    qlpUserDef        * sParseTree = NULL;
    qlaInitCreateUser * sInitPlan = NULL;
    
    stlChar         * sUserName = NULL;
    stlChar         * sPassword = NULL;

    stlChar         * sDataSpaceName = NULL;
    stlChar         * sTempSpaceName = NULL;
    
    stlChar         * sSchemaName = NULL;
    stlChar         * sProfileName = NULL;
    
    ellDictHandle     sObjectHandle;
    stlBool           sObjectExist = STL_FALSE;

    smlStatement  * sStmt = NULL;
    stlInt32        sStmtAttr = 0;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_CREATE_USER_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_CREATE_USER_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */
    
    sParseTree = (qlpUserDef *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlaInitCreateUser),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlaInitCreateUser) );

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
    
    /**********************************************************
     * User Validation
     **********************************************************/

    /**
     * User Name 정보 획득
     */

    sUserName = QLP_GET_PTR_VALUE(sParseTree->mUserName);
    
    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                stlStrlen( sUserName ) + 1,
                                (void **) & sInitPlan->mUserName,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlStrcpy( sInitPlan->mUserName, sUserName );
    
    /**
     * User 존재 여부 확인
     */
    
    STL_TRY( ellGetAuthDictHandle( aTransID,
                                   aSQLStmt->mViewSCN,
                                   sInitPlan->mUserName,
                                   & sObjectHandle,
                                   & sObjectExist,
                                   ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sObjectExist == STL_FALSE, RAMP_ERR_SAME_NAME_USER_EXISTS );

    /**
     * CREATE USER ON DATABASE 권한 검사
     */

    STL_TRY( qlaCheckDatabasePriv( aTransID,
                                   aDBCStmt,
                                   aSQLStmt,
                                   ELL_DB_PRIV_ACTION_CREATE_USER,
                                   aEnv )
             == STL_SUCCESS );

    /**********************************************************
     * Authentication 정보 Validation
     **********************************************************/
    
    /**
     * Password Encryption
     */

    sPassword = QLP_GET_PTR_VALUE(sParseTree->mPassword);

    /* password verify 를 위한 설정 */
    stlStrcpy( sInitPlan->mPlainPassword, sPassword );
    
    stlMemset( sInitPlan->mEncryptPassword, 0x00, STL_MAX_SQL_IDENTIFIER_LENGTH );
    STL_TRY( stlMakeEncryptedPassword( sPassword,
                                       stlStrlen( sPassword ),
                                       sInitPlan->mEncryptPassword,
                                       STL_MAX_SQL_IDENTIFIER_LENGTH,
                                       QLL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    if ( sParseTree->mProfileName == NULL )
    {
        sInitPlan->mHasProfile = STL_FALSE;
        ellInitDictHandle( & sInitPlan->mProfileHandle );
    }
    else
    {
        sInitPlan->mHasProfile = STL_TRUE;
        sProfileName = QLP_GET_PTR_VALUE(sParseTree->mProfileName);

        STL_TRY( ellGetProfileDictHandleByName( aTransID,
                                                aSQLStmt->mViewSCN,
                                                sProfileName,
                                                & sInitPlan->mProfileHandle,
                                                & sObjectExist,
                                                ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_PROFILE_NOT_EXIST );
    }
    
    /**
     * PASSWORD_EXPIRE
     */

    sInitPlan->mPasswordExpire = sParseTree->mPasswordExpire;

    /**
     * ACCOUNT LOCK | UNLOCK
     */

    sInitPlan->mAccountLock = sParseTree->mAccountLock;
    
    /**********************************************************
     * Default Data TableSpace Validation
     **********************************************************/

    /**
     * Default Data Tablespace
     */
    
    if ( sParseTree->mDefaultSpace == NULL )
    {
        /**
         * Database Default Data Space 정보 획득
         */
        
        STL_TRY( ellGetDbDataSpaceHandle( aTransID,
                                          sStmt,
                                          & sInitPlan->mDataSpaceHandle,
                                          QLL_INIT_PLAN(aDBCStmt),
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        sDataSpaceName = QLP_GET_PTR_VALUE(sParseTree->mDefaultSpace);

        STL_TRY( ellGetTablespaceDictHandle( aTransID,
                                             aSQLStmt->mViewSCN,
                                             sDataSpaceName,
                                             & sInitPlan->mDataSpaceHandle,
                                             & sObjectExist,
                                             ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_DATA_SPACE_NAME_NOT_EXISTS );
    }

    /**
     * DATA Tablespace 인지 검사
     */

    STL_TRY_THROW( ellGetTablespaceUsageType( & sInitPlan->mDataSpaceHandle ) == ELL_SPACE_USAGE_TYPE_DATA,
                   RAMP_ERR_TABLESPACE_NOT_DATA_TABLESPACE );
    
    /**********************************************************
     * Default Temporary TableSpace Validation
     **********************************************************/
    
    /**
     * Default Temporary Tablespace
     */

    if ( sParseTree->mTempSpace == NULL )
    {
        /**
         * Database Default Temp Space 정보 획득
         */

        STL_TRY( ellGetDbTempSpaceHandle( aTransID,
                                          sStmt,
                                          & sInitPlan->mTempSpaceHandle,
                                          QLL_INIT_PLAN(aDBCStmt),
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        sTempSpaceName = QLP_GET_PTR_VALUE(sParseTree->mTempSpace);

        STL_TRY( ellGetTablespaceDictHandle( aTransID,
                                             aSQLStmt->mViewSCN,
                                             sTempSpaceName,
                                             & sInitPlan->mTempSpaceHandle,
                                             & sObjectExist,
                                             ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_TEMP_SPACE_NAME_NOT_EXISTS );
    }
        
    /**
     * TEMP Tablespace 인지 검사
     */

    STL_TRY_THROW( ellGetTablespaceUsageType( & sInitPlan->mTempSpaceHandle ) == ELL_SPACE_USAGE_TYPE_TEMPORARY,
                   RAMP_ERR_TABLESPACE_NOT_TEMP_TABLESPACE );
    
    /**********************************************************
     * Schema Validation
     **********************************************************/

    if ( sParseTree->mSchemaName == NULL )
    {
        /**
         * WITHOUT SCHEMA 임
         */
        
        sInitPlan->mSchemaName = NULL;
    }
    else
    {
        sSchemaName = QLP_GET_PTR_VALUE(sParseTree->mSchemaName);

        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                    stlStrlen( sSchemaName ) + 1,
                                    (void **) & sInitPlan->mSchemaName,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
        stlStrcpy( sInitPlan->mSchemaName, sSchemaName );
    
        /**
         * Schema 존재 여부 확인
         */
        
        STL_TRY( ellGetSchemaDictHandle( aTransID,
                                         aSQLStmt->mViewSCN,
                                         sInitPlan->mSchemaName,
                                         & sObjectHandle,
                                         & sObjectExist,
                                         ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        STL_TRY_THROW( sObjectExist == STL_FALSE, RAMP_ERR_SAME_NAME_SCHEMA_EXISTS );
    }

    /**
     * Statement 해제
     */
    
    sState = 0;
    STL_TRY( smlFreeStatement( sStmt, SML_ENV(aEnv) ) == STL_SUCCESS );
    sStmt = NULL;

    /********************************************************
     * Validation Object 추가 
     ********************************************************/

    if ( sInitPlan->mHasProfile == STL_TRUE )
    {
        STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                      & sInitPlan->mProfileHandle,
                                      QLL_INIT_PLAN(aDBCStmt),
                                      ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        /* nothing to do */
    }
    
    STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                  & sInitPlan->mDataSpaceHandle,
                                  QLL_INIT_PLAN(aDBCStmt),
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                  & sInitPlan->mTempSpaceHandle,
                                  QLL_INIT_PLAN(aDBCStmt),
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**********************************************************
     * Output 설정 
     **********************************************************/

    /**
     * Init Plan 연결 
     */

    aSQLStmt->mInitPlan = (void *) sInitPlan;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SAME_NAME_USER_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_USER_NAME_CONFLICT_WITH_ANOTHER_USER_OR_ROLE_NAME,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mUserName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sUserName );
    }

    STL_CATCH( RAMP_ERR_PROFILE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_PROFILE_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mProfileName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sProfileName );
    }
    
    STL_CATCH( RAMP_ERR_DATA_SPACE_NAME_NOT_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLESPACE_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mDefaultSpace->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sDataSpaceName );
    }

    STL_CATCH( RAMP_ERR_TABLESPACE_NOT_DATA_TABLESPACE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLESPACE_NOT_DATA_TABLESPACE,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      ellGetTablespaceName( & sInitPlan->mDataSpaceHandle ) );
    }
    
    STL_CATCH( RAMP_ERR_TEMP_SPACE_NAME_NOT_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLESPACE_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mTempSpace->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sTempSpaceName );
    }

    STL_CATCH( RAMP_ERR_TABLESPACE_NOT_TEMP_TABLESPACE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLESPACE_NOT_TEMPORARY_TABLESPACE,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      ellGetTablespaceName( & sInitPlan->mTempSpaceHandle ) );
    }

    STL_CATCH( RAMP_ERR_SAME_NAME_SCHEMA_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_USED_SCHEMA_NAME,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mSchemaName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sSchemaName );
    }
    
    /* STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED ) */
    /* { */
    /*     stlPushError( STL_ERROR_LEVEL_ABORT, */
    /*                   QLL_ERRCODE_NOT_IMPLEMENTED, */
    /*                   NULL, */
    /*                   QLL_ERROR_STACK(aEnv), */
    /*                   "qlaValidateCreateUser()" ); */
    /* } */
    
    STL_FINISH;

    switch (sState)
    {
        case 1:
            (void) smlFreeStatement( sStmt, SML_ENV(aEnv) );
            break;
        default:
            break;
    }
    
    return STL_FAILURE;
}

/**
 * @brief CREATE USER 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlaOptCodeCreateUser( smlTransId      aTransID,
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
 * @brief CREATE USER 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlaOptDataCreateUser( smlTransId      aTransID,
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
 * @brief CREATE USER 구문을 Execute 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aStmt      Statement
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlaExecuteCreateUser( smlTransId      aTransID,
                                smlStatement  * aStmt,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv )
{
    qlaInitCreateUser * sInitPlan = NULL;

    stlInt64            sUserID = ELL_DICT_OBJECT_ID_NA;
    stlInt64            sSchemaID = ELL_DICT_OBJECT_ID_NA;

    stlInt64            sSchemaPath[QLA_USER_SCHEMA_PATH_CNT] = {ELL_DICT_OBJECT_ID_NA,};
    stlInt32            sSchemaPathCnt = 0;
    
    ellDictHandle     * sAuthHandle = NULL;
    
    stlTime             sTime = 0;
    stlBool             sLocked = STL_TRUE;

    ellUserProfileParam sProfileParam;
    ellAuthenInfo       sUserAuthenInfo;
    
    stlInt32 i = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_CREATE_USER_TYPE,
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

    sInitPlan = (qlaInitCreateUser *) aSQLStmt->mInitPlan;
    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    
    /**
     * CREATE USER 구문을 위한 DDL Lock 획득
     */

    if ( sInitPlan->mHasProfile == STL_TRUE )
    {
        STL_TRY( ellLock4CreateUser( aTransID,
                                     aStmt,
                                     sAuthHandle,
                                     & sInitPlan->mDataSpaceHandle,
                                     & sInitPlan->mTempSpaceHandle,
                                     & sInitPlan->mProfileHandle,
                                     & sLocked,
                                     ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( ellLock4CreateUser( aTransID,
                                     aStmt,
                                     sAuthHandle,
                                     & sInitPlan->mDataSpaceHandle,
                                     & sInitPlan->mTempSpaceHandle,
                                     NULL,
                                     & sLocked,
                                     ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
        

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    /**
     * Valid Plan 을 다시 획득
     */

    STL_TRY( qlgGetValidPlan4DDL( aTransID,
                                  aDBCStmt,
                                  aSQLStmt,
                                  QLL_PHASE_EXECUTE,
                                  aEnv )
             == STL_SUCCESS );

    sInitPlan = (qlaInitCreateUser *) aSQLStmt->mInitPlan;
    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );

    /***************************************************************
     * User Authentication 정보 설정
     ***************************************************************/

    /**
     * PROFILE 정보 획득
     */
    
    if ( sInitPlan->mHasProfile == STL_TRUE )
    {
        STL_TRY( ellGetUserProfileParam( aTransID,
                                         aStmt,
                                         & sInitPlan->mProfileHandle,
                                         & sProfileParam,
                                         ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( ellGetUserProfileParam( aTransID,
                                         aStmt,
                                         NULL,
                                         & sProfileParam,
                                         ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    /**
     * User Authentication 정보 초기화
     */

    sUserAuthenInfo.mLockedStatus = ELL_USER_LOCKED_STATUS_OPEN;
    sUserAuthenInfo.mLockedTime   = ELL_DICT_STLTIME_NA;
    sUserAuthenInfo.mFailedLoginAttempt = 0;
    
    sUserAuthenInfo.mExpiryStatus = ELL_USER_EXPIRY_STATUS_OPEN;
    sUserAuthenInfo.mExpiryTime   = ELL_DICT_STLTIME_NA;
    stlStrcpy( sUserAuthenInfo.mEncryptPassword, sInitPlan->mEncryptPassword );
    sUserAuthenInfo.mPassChangeCount = 0;

    /**
     * Password 검증
     */

    if ( sInitPlan->mHasProfile == STL_TRUE )
    {
        /**
         * Password Verify Function 검증
         */
        
        STL_TRY( qlaVerifyPassword( sInitPlan->mUserName,
                                    sInitPlan->mPlainPassword,
                                    NULL, /* aPlainOrgPassword */
                                    sProfileParam.mVerifyFuncID,
                                    aEnv )
                 == STL_SUCCESS );

        /**
         * Expiry Time 계산
         */

        if ( DTL_IS_NULL( sProfileParam.mLifeTime ) == STL_TRUE )
        {
            sUserAuthenInfo.mExpiryTime   = ELL_DICT_STLTIME_NA;
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

    /**
     * PASSWORD EXPIRE 인 경우
     */
    
    if ( sInitPlan->mPasswordExpire == STL_TRUE )
    {
        sUserAuthenInfo.mExpiryTime   = sTime;
        sUserAuthenInfo.mExpiryStatus = ELL_USER_EXPIRY_STATUS_EXPIRED;
    }
    else
    {
        /* nothing to do */
    }
    
    /**
     * ACCOUNT LOCK 인 경우
     */

    if ( sInitPlan->mAccountLock == STL_TRUE )
    {
        sUserAuthenInfo.mLockedTime   = sTime;
        sUserAuthenInfo.mLockedStatus = ELL_USER_LOCKED_STATUS_LOCKED;
    }
    else
    {
        /* nothing to do */
    }
    
    /***************************************************************
     * Dictionary Record 추가 
     ***************************************************************/

    /**
     * Authorization Descriptor 추가
     */

    STL_TRY( ellInsertAuthDesc( aTransID,
                                aStmt,
                                & sUserID,
                                sInitPlan->mUserName,
                                ELL_AUTHORIZATION_TYPE_USER,
                                NULL, /* aAuthComment */
                                ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * User Descriptor 추가
     */

    if ( sInitPlan->mHasProfile == STL_TRUE )
    {
        STL_TRY( ellInsertUserDesc( aTransID,
                                    aStmt,
                                    sUserID,
                                    ellGetProfileID( & sInitPlan->mProfileHandle ),
                                    sInitPlan->mEncryptPassword,
                                    ellGetTablespaceID( & sInitPlan->mDataSpaceHandle ),
                                    ellGetTablespaceID( & sInitPlan->mTempSpaceHandle ),
                                    sUserAuthenInfo.mLockedStatus,
                                    sUserAuthenInfo.mLockedTime, 
                                    sUserAuthenInfo.mExpiryStatus,
                                    sUserAuthenInfo.mExpiryTime, 
                                    ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        /**
         * Password Reuse 정보 추가
         */

        if ( ( sProfileParam.mReuseMax == ELL_PROFILE_LIMIT_UNLIMITED ) &&
             ( DTL_IS_NULL(sProfileParam.mReuseTime) == STL_TRUE ) )
        {
            /**
             * PASSWORD_REUSE_MAX UNLIMITED
             * PASSWORD_REUSE_TIME UNLIMITED
             */
            
            /* password reuse 를 정보를 추가할 필요가 없음 */
        }
        else
        {
            STL_TRY( ellInsertPasswordHistoryDesc( aTransID,
                                                   aStmt,
                                                   sUserID,
                                                   sInitPlan->mEncryptPassword,
                                                   0,     /* aPassChangeNO */
                                                   sTime, /* aPassChangeTime */
                                                   ELL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
    }
    else
    {
        STL_TRY( ellInsertUserDesc( aTransID,
                                    aStmt,
                                    sUserID,
                                    ELL_DICT_OBJECT_ID_NA, /* aProfileID */
                                    sInitPlan->mEncryptPassword,
                                    ellGetTablespaceID( & sInitPlan->mDataSpaceHandle ),
                                    ellGetTablespaceID( & sInitPlan->mTempSpaceHandle ),
                                    sUserAuthenInfo.mLockedStatus,
                                    sUserAuthenInfo.mLockedTime, 
                                    sUserAuthenInfo.mExpiryStatus,
                                    sUserAuthenInfo.mExpiryTime, 
                                    ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    
    /**
     * Schema Descriptor 추가
     */

    if ( sInitPlan->mSchemaName == NULL )
    {
        /**
         * nothing to do
         */
    }
    else
    {
        STL_TRY( ellInsertSchemaDesc( aTransID,
                                      aStmt,
                                      sUserID,
                                      & sSchemaID,
                                      sInitPlan->mSchemaName,
                                      NULL,   /* aSchemaComment */
                                      ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    
    /**
     * User Schema Path 추가
     */

    if ( sInitPlan->mSchemaName == NULL )
    {
        sSchemaPathCnt = 1;

        sSchemaPath[0] = ellGetSchemaIdPUBLIC();
    }
    else
    {
        sSchemaPathCnt = 2;

        sSchemaPath[0] = sSchemaID;
        sSchemaPath[1] = ellGetSchemaIdPUBLIC();
    }

    for ( i = 0; i < sSchemaPathCnt; i++ )
    {
        STL_TRY( ellInsertUserSchemaPathDesc( aTransID,
                                              aStmt,
                                              sUserID, 
                                              sSchemaPath[i],
                                              i,
                                              ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    /**********************************************************
     * Dictionary Cache 구축 
     **********************************************************/
    
    /**
     * SQL-Schema Cache 추가
     */
    
    if ( sInitPlan->mSchemaName == NULL )
    {
        /**
         * nothing to do
         */
    }
    else
    {
        STL_TRY( ellRefineCache4AddSchema( aTransID,
                                           aStmt,
                                           sUserID,
                                           sSchemaID,
                                           ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    /**
     * SQL-Authorization Cache 추가
     */

    STL_TRY( ellRefineCache4AddUser( aTransID,
                                     aStmt,
                                     sUserID,
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
                                                      ELL_OBJECT_AUTHORIZATION,
                                                      sUserID,
                                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    /* STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED ) */
    /* { */
    /*     stlPushError( STL_ERROR_LEVEL_ABORT, */
    /*                   QLL_ERRCODE_NOT_IMPLEMENTED, */
    /*                   NULL, */
    /*                   QLL_ERROR_STACK(aEnv), */
    /*                   "qlaExecuteCreateUser()" ); */
    /* } */
    
    STL_FINISH;

    return STL_FAILURE;
}






/** @} qlaCreateUser */
