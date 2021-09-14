/*******************************************************************************
 * qllSession.c
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
 * @file qllSession.c
 * @brief SQL Session API
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qllSession
 * @{
 */

/**
 * @brief SQL 이 아닌 직접 호출하는 연결 API
 * @param[in] aUserName         User Name
 * @param[in] aEncryptedPasswd  Encrypted Password
 * @param[in] aRoleName         Role Name
 * @param[in] aPlainOrgPassword Plain Text Org Password
 * @param[in] aPlainNewPassword Plain Text New Password
 * @param[in] aEnv              Environment
 * @remarks
 * 내부적으로 별도의 Transaction 과 Statement 를 생성한다.
 */
stlStatus qllLogin( stlChar * aUserName,
                    stlChar * aEncryptedPasswd,
                    stlChar * aRoleName,
                    stlChar * aPlainOrgPassword,
                    stlChar * aPlainNewPassword,
                    qllEnv  * aEnv )
{
    stlInt32  sState = 0;
    
    smlTransId      sTransID = SML_INVALID_TRANSID;
    smlStatement  * sStmt = NULL;

    stlInt64        sAuthIdSYS = ELL_DICT_OBJECT_ID_NA;
    
    ellDictHandle   sLoginHandle;
    ellDictHandle   sRoleHandle;
    stlBool         sValidObject = STL_FALSE;

    ellDictHandle   sPrivHandle;
    stlBool         sPrivExist = STL_FALSE;

    knlStartupPhase sCurrPhase = KNL_STARTUP_PHASE_NONE;
    stlBool         sVersionConflict = STL_FALSE;
    stlBool         sHasModifyError = STL_FALSE;
    
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aUserName != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRoleName != NULL, QLL_ERROR_STACK(aEnv) );

    /******************************************************************
     * 기본 정보 획득
     ******************************************************************/
    
    /**
     * 현재 Phase 에서 수행가능하지 검사
     */

    sCurrPhase = knlGetCurrStartupPhase();    
    
    STL_TRY_THROW ( (sCurrPhase >= KNL_STARTUP_PHASE_NO_MOUNT )&&
                    (sCurrPhase <= KNL_STARTUP_PHASE_OPEN),
                    RAMP_ERR_INVALID_EXECUTABLE_PHASE );

    /**
     * Begin Transaction
     */
    
    if ( sCurrPhase == KNL_STARTUP_PHASE_OPEN )
    {
        STL_TRY( smlBeginTrans( SML_TIL_READ_COMMITTED,
                                STL_FALSE,  /* aIsGlobalTrans */
                                &sTransID,
                                SML_ENV( aEnv ) )
                 == STL_SUCCESS );
    }
    else
    {
        sTransID = SML_INVALID_TRANSID;
    }
    sState = 1;
    
    /**
     * Statement 생성
     */

    if ( sCurrPhase == KNL_STARTUP_PHASE_OPEN )
    {
        STL_TRY( smlAllocStatement( sTransID,
                                    NULL, /* aUpdateRelHandle */
                                    SML_STMT_ATTR_UPDATABLE | SML_STMT_ATTR_LOCKABLE,
                                    SML_LOCK_TIMEOUT_PROPERTY,
                                    STL_TRUE, /* aNeedSnapshotIterator */
                                    & sStmt,
                                    SML_ENV( aEnv ) )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( smlAllocStatement( sTransID,
                                    NULL, /* aUpdateRelHandle */
                                    SML_STMT_ATTR_READONLY,
                                    SML_LOCK_TIMEOUT_INVALID,
                                    STL_TRUE, /* aNeedSnapshotIterator */
                                    & sStmt,
                                    SML_ENV( aEnv ) )
                 == STL_SUCCESS );
    }
    
    sState = 2;

    /******************************************************************
     * Session NLS 설정
     * user password 변경 시간등을 기록하므로
     * authentication 검사 이전에 수행해야 함.
     ******************************************************************/
    
    STL_TRY( qllInitSessNLS( aEnv ) == STL_SUCCESS );

    
    sVersionConflict = STL_FALSE;

    STL_RAMP( RAMP_RETRY );

    if ( sVersionConflict == STL_TRUE )
    {
        STL_TRY( smlResetStatement( sStmt,
                                    STL_TRUE, /* aDoRollback */
                                    SML_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    
    /******************************************************************
     * Login User 정보 획득
     ******************************************************************/

    sAuthIdSYS = ellGetAuthIdSYS();
    
    if ( ( knlGetSessionConnectionType( KNL_SESS_ENV(aEnv) ) == KNL_CONNECTION_TYPE_DA ) &&
          (aRoleName[0] != '\0') )
    {
        /**
         * SYS 계정의 Authorization Handle 을 얻는다
         * ex) gsql --as sysdba 등과 같이 user/password 없이 role 이름만 부여한 경우
         * ex) 계정을 검증할 수 있는 OPEN 단계가 아닌 경우 
         */

        /**
         * SYS 계정의 Authorization Handle 을 얻는다
         */

        STL_TRY( ellGetAuthDictHandleByID( sTransID,
                                           sStmt->mScn,
                                           sAuthIdSYS,
                                           & sLoginHandle,
                                           & sValidObject,
                                           ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        STL_DASSERT( sValidObject == STL_TRUE );
    }
    else
    {
        /**
         * User/Password 검사가 필요한 경우
         */

        ellInitDictHandle( & sLoginHandle );
        
        /**
         * Authorization Handle 획득
         */

        STL_TRY( ellGetAuthDictHandle( sTransID,
                                       sStmt->mScn,
                                       aUserName,
                                       & sLoginHandle,
                                       & sValidObject,
                                       ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        STL_TRY_THROW( sValidObject == STL_TRUE, RAMP_ERR_INVALID_LOGIN );

        STL_TRY_THROW( ellGetAuthType( & sLoginHandle ) == ELL_AUTHORIZATION_TYPE_USER,
                       RAMP_ERR_INVALID_LOGIN );
        
        /**
         * Password 검증
         */

        if ( sCurrPhase == KNL_STARTUP_PHASE_OPEN )
        {
            if ( aPlainNewPassword == NULL )
            {
                STL_TRY( qllCheckLoginAuthentication( sTransID,
                                                      sStmt,
                                                      & sLoginHandle,
                                                      aEncryptedPasswd,
                                                      NULL,
                                                      NULL,
                                                      & sVersionConflict,
                                                      & sHasModifyError,
                                                      aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                STL_TRY( qllCheckLoginAuthentication( sTransID,
                                                      sStmt,
                                                      & sLoginHandle,
                                                      NULL,
                                                      aPlainOrgPassword,
                                                      aPlainNewPassword,
                                                      & sVersionConflict,
                                                      & sHasModifyError,
                                                      aEnv )
                         == STL_SUCCESS );
            }

            if ( sVersionConflict == STL_TRUE )
            {
                STL_THROW( RAMP_RETRY );
            }
            else
            {
                /* ok */
            }
        }
        else
        {
            /**
             * DB 가 구동되지 않은 상태(NO MOUNT, MOUNT) 로 Password 검증할 수 없음
             */
        }
    }

    /**
     * SQL Session 에 Login Handle 정보 설정
     */
    
    ellSetLoginUser( & sLoginHandle, ELL_ENV(aEnv) );
    
    /******************************************************************
     * Role 정보 획득
     ******************************************************************/
    
    if ( aRoleName[0] == '\0' )
    {
        /**
         * nothing to do
         */
    }
    else
    {
        /**
         * SYS user 로 login 했는지 확인
         */

        STL_TRY_THROW( sAuthIdSYS == ellGetAuthID( & sLoginHandle ), RAMP_ERR_INVALID_LOGIN );

        /**
         * Role 정보를 획득
         */

        STL_TRY( ellGetAuthDictHandle( sTransID,
                                       sStmt->mScn,
                                       aRoleName,
                                       & sRoleHandle,
                                       & sValidObject,
                                       ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        STL_TRY_THROW( sValidObject == STL_TRUE, RAMP_ERR_INVALID_LOGIN );

        STL_TRY_THROW( ellGetAuthType( & sRoleHandle ) == ELL_AUTHORIZATION_TYPE_ROLE,
                       RAMP_ERR_INVALID_LOGIN );
        
        /**
         * SQL Session 에 Session Role 을 설정
         */

        ellSetSessionRole( & sRoleHandle, ELL_ENV(aEnv) );
    }

    /******************************************************************
     * 권한 검사 
     ******************************************************************/

    /**
     * CREATE SESSION ON DATABASE 권한이 있는지 검사
     */

    STL_TRY( ellGetGrantedPrivHandle( sTransID,
                                      sStmt->mScn,
                                      ELL_PRIV_DATABASE,
                                      ellGetDbCatalogID(),
                                      ELL_DB_PRIV_ACTION_CREATE_SESSION,
                                      STL_FALSE,  /* aWithGrant */
                                      & sPrivHandle,
                                      & sPrivExist,
                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sPrivExist == STL_TRUE, RAMP_ERR_PRIVILEGE_CREATE_SESSION );

    /**
     * Statement 종료 
     */

    sState = 1;
    STL_TRY( smlFreeStatement( sStmt, SML_ENV( aEnv ) ) == STL_SUCCESS );
    sStmt = NULL;

    /**
     * Transaction 종료
     */

    sState = 0;
    
    if ( sCurrPhase == KNL_STARTUP_PHASE_OPEN )
    {
        STL_ASSERT( smlCommit( sTransID,
                               NULL, /* aComment */
                               SML_TRANSACTION_CWM_NO_WAIT,
                               SML_ENV( aEnv ) )
                    == STL_SUCCESS );

        STL_ASSERT( smlEndTrans( sTransID,
                                 SML_PEND_ACTION_COMMIT,
                                 SML_TRANSACTION_CWM_NO_WAIT,
                                 SML_ENV( aEnv ) )
                    == STL_SUCCESS );
    }
    else
    {
        /* nohting to do */
    }
    
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_EXECUTABLE_PHASE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_EXECUTABLE_PHASE,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      gPhaseString[sCurrPhase],
                      "LOGIN",
                      gPhaseString[KNL_STARTUP_PHASE_NO_MOUNT],
                      gPhaseString[KNL_STARTUP_PHASE_OPEN] );
    }
    
    STL_CATCH( RAMP_ERR_INVALID_LOGIN )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_LOGIN,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_PRIVILEGE_CREATE_SESSION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_USER_LACKS_CREATE_SESSION_PRIVILEGE,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      ellGetAuthName( & sLoginHandle ) );
    }
    
    STL_FINISH;

    switch (sState)
    {
        case 2:
            (void) smlFreeStatement( sStmt, SML_ENV( aEnv ) );
        case 1:
            if ( sCurrPhase == KNL_STARTUP_PHASE_OPEN )
            {
                if ( sHasModifyError == STL_TRUE )
                {
                    /*
                     * authentication 정보 변경 중 에러가 발생한 경우
                     */
                    
                    (void) smlRollback( sTransID,
                                        SML_TRANSACTION_CWM_NO_WAIT,
                                        SML_ENV( aEnv ) );
                    (void) smlEndTrans( sTransID,
                                        SML_PEND_ACTION_ROLLBACK,
                                        SML_TRANSACTION_CWM_NO_WAIT,
                                        SML_ENV( aEnv ) );
                }
                else
                {
                    /*
                     * login 이 실패하더라도 User Authentication 정보 변경을 COMMIT 한다.
                     */
                    
                    (void) smlCommit( sTransID,
                                      NULL, /* aComment */
                                      SML_TRANSACTION_CWM_NO_WAIT,
                                      SML_ENV( aEnv ) );
                    (void) smlEndTrans( sTransID,
                                        SML_PEND_ACTION_COMMIT,
                                        SML_TRANSACTION_CWM_NO_WAIT,
                                        SML_ENV( aEnv ) );
                }
            }
        default:
            break;
    }
    
    return STL_FAILURE;
}




/**
 * @brief Transaction COMMIT 을 수행함.
 * @param[in] aTransID   Transaction ID
 * @param[in] aComment   Transaction Comment
 * @param[in] aWriteMode Transaction Commit Write Mode
 * @param[in] aXaContext XA Context ( if null, local transaction )
 * @param[in] aEnv       Environemnt
 * @see @a aWriteMode : smlTransactionWriteMode
 * @remarks
 */
stlStatus qllCommit( smlTransId     aTransID,
                     stlChar      * aComment,
                     stlInt64       aWriteMode,
                     knlXaContext * aXaContext,
                     qllEnv       * aEnv )
 {
    stlInt32        sState = 0;
    stlInt64        sWriteMode = aWriteMode;

    stlUInt32  sSessionID = 0;
    
    /**
     * 위배한 Deferred Constraint 가 존재하는 지 검사 
     */

    sState = 0;
    if ( aXaContext == NULL )
    {
        STL_TRY_THROW( qlrcHasViolatedDeferredConstraint( aEnv ) != STL_TRUE,
                       RAMP_ERR_INTEGRITY_CONSTRAINT_VIOLATION );
    }
    else
    {
        STL_TRY_THROW( qlrcHasXaViolatedConstraint( aXaContext ) != STL_TRUE,
                       RAMP_ERR_XA_INTEGRITY_CONSTRAINT_VIOLATION );
    }

    /**
     * Deferred Constraint 정보 Reset
     */
    
    if ( aXaContext == NULL )
    {
        qlrcTransEndDeferConstraint( aEnv );
    }
    else
    {
        qllClearXaDeferContext( aXaContext, aEnv );
    }
    
    /**
     * DDL Trace Message
     */
    
    sState = 1;
    if ( knlGetPropertyBoolValueByID( KNL_PROPERTY_TRACE_DDL, KNL_ENV(aEnv) ) == STL_TRUE )
    {
        STL_TRY( knlGetSessionEnvId( (void*) ELL_SESS_ENV(aEnv),
                                     & sSessionID,
                                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        if ( ELL_SESS_HAS_DDL(aEnv) == STL_TRUE )
        {
            knlLogMsg( NULL,
                       KNL_ENV( aEnv ),
                       KNL_LOG_LEVEL_INFO,
                       "[SESSION:%d][COMMIT with DDL] \n",
                       sSessionID );
        }
        else
        {
            /* nothing to do */
        }
    }
    else
    {
        /* nothing to do */
    }
    
    /**
     * OPEN 된 WITHOUT HOLD 옵션을 가진 Named Cursor 를 닫음.
     * 
     * COMMIT/ROLLBACK 시 Storage Manager 에서
     * DML/SELECT FOR UPDATE/WITHOUT HOLD CURSOR 를 위해 Open 된 Iterator를 모두 닫으나,
     * 다음과 같은 이유로 Named Cursor 에 대해 명시적으로 닫음.
     * - UPDATE WHERE CURRENT OF cursor 구문의 error 를 발생시켜야 함.
     * - 자원 해제 
     */

    sState = 2;
    STL_TRY( qlcrCloseAllOpenNotHoldableCursor4TransEnd( aEnv ) == STL_SUCCESS );

    STL_TRY( ellPreActionCommitDDL( aTransID, ELL_ENV(aEnv) ) == STL_SUCCESS );
    
    /**
     * 트랙잭션 COMMIT
     */

    if( aWriteMode == SML_TRANSACTION_CWM_NONE )
    {
        sWriteMode = knlGetPropertyBigIntValueByID( KNL_PROPERTY_TRANSACTION_COMMIT_WRITE_MODE,
                                                    KNL_ENV(aEnv) );
    }
    
    sState = 3;
    STL_ASSERT( smlCommit( aTransID,
                           aComment,
                           sWriteMode,
                           SML_ENV( aEnv ) )
                == STL_SUCCESS );

    KNL_BREAKPOINT( KNL_BREAKPOINT_QLLCOMMIT_BEFORE_DICTIONARY_PENDING, KNL_ENV(aEnv) );
    
    sState = 4;
    STL_TRY( ellCommitTransDDL( aTransID, ELL_ENV( aEnv ) ) == STL_SUCCESS );

    sState = 5;
    STL_ASSERT( smlEndTrans( aTransID,
                             SML_PEND_ACTION_COMMIT,
                             sWriteMode,
                             SML_ENV( aEnv ) ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTEGRITY_CONSTRAINT_VIOLATION )
    {
        stlChar sErrMsg[STL_MAX_ERROR_MESSAGE] = {0,};

        (void) qlrcSetViolatedHandleMessage( aTransID,
                                             sErrMsg,
                                             STL_MAX_ERROR_MESSAGE,
                                             NULL, /* aConstHandle */
                                             aEnv );
        
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TRANSACTION_ROLLBACK_INTEGRITY_CONSTRAINT_VIOLATION,
                      sErrMsg,
                      QLL_ERROR_STACK( aEnv ) );
    }

    STL_CATCH( RAMP_ERR_XA_INTEGRITY_CONSTRAINT_VIOLATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TRANSACTION_ROLLBACK_INTEGRITY_CONSTRAINT_VIOLATION,
                      NULL,
                      QLL_ERROR_STACK( aEnv ) );
    }
    
    STL_FINISH;

    if ( sState > 1 )
    {
        /**
         * COMMIT 이 시작되면 실패가 발생하면 안됨. 
         */
        
        STL_ASSERT(0);
    }
    else
    {
        if ( aXaContext == NULL )
        {
            qlrcTransEndDeferConstraint( aEnv );
        }
        else
        {
            qllClearXaDeferContext( aXaContext, aEnv );
        }
    }

    return STL_FAILURE;
}

/**
 * @brief Transaction ROLLBACK 을 수행함.
 * @param[in] aTransID   Transaction ID
 * @param[in] aXaContext XA Context ( if null, local transaction )
 * @param[in] aEnv       Environemnt
 * @remarks
 */
stlStatus qllRollback( smlTransId     aTransID,
                       knlXaContext * aXaContext,
                       qllEnv       * aEnv )
{
    stlUInt32  sSessionID = 0;

    /**
     * DDL Trace Message
     */

    if ( knlGetPropertyBoolValueByID( KNL_PROPERTY_TRACE_DDL, KNL_ENV(aEnv) ) == STL_TRUE )
    {
        if ( ELL_SESS_HAS_DDL(aEnv) == STL_TRUE )
        {
            STL_TRY( knlGetSessionEnvId( (void*) ELL_SESS_ENV(aEnv),
                                         & sSessionID,
                                         KNL_ENV(aEnv) )
                     == STL_SUCCESS );
        
            knlLogMsg( NULL,
                       KNL_ENV( aEnv ),
                       KNL_LOG_LEVEL_INFO,
                       "[SESSION:%d][ROLLBACK with DDL] \n",
                       sSessionID );
        }
        else
        {
            /* nothing to do */
        }
    }
    else
    {
        /* nothing to do */
    }
    
    /**
     * OPEN 된 WITHOUT HOLD 옵션을 가진 Named Cursor 를 닫음.
     * 
     * COMMIT/ROLLBACK 시 Storage Manager 에서
     * DML/SELECT FOR UPDATE/WITHOUT HOLD CURSOR 를 위해 Open 된 Iterator를 모두 닫으나,
     * 다음과 같은 이유로 Named Cursor 에 대해 명시적으로 닫음.
     * - UPDATE WHERE CURRENT OF cursor 구문의 error 를 발생시켜야 함.
     * - 자원 해제 
     */

    STL_TRY( qlcrCloseAllOpenNotHoldableCursor4TransEnd( aEnv ) == STL_SUCCESS );

    STL_TRY( ellPreActionRollbackDDL( aTransID, ELL_ENV(aEnv) ) == STL_SUCCESS );
    
    /**
     * 트랙잭션 ROLLBACK
     */
    
    STL_ASSERT( smlRollback( aTransID,
                             SML_TRANSACTION_CWM_NO_WAIT,
                             SML_ENV( aEnv ) ) == STL_SUCCESS );

    KNL_BREAKPOINT( KNL_BREAKPOINT_QLLROLLBACK_BEFORE_DICTIONARY_PENDING, KNL_ENV(aEnv) );
    
    STL_TRY( ellRollbackTransDDL( aTransID, ELL_ENV( aEnv ) ) == STL_SUCCESS );

    STL_ASSERT( smlEndTrans( aTransID,
                             SML_PEND_ACTION_ROLLBACK,
                             SML_TRANSACTION_CWM_NO_WAIT,
                             SML_ENV( aEnv ) ) == STL_SUCCESS );

    /**
     * Deferred Constraint 정보 Reset
     */
    
    if ( aXaContext == NULL )
    {
        qlrcTransEndDeferConstraint( aEnv );
    }
    else
    {
        qllClearXaDeferContext( aXaContext, aEnv );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    /**
     * ROLLBACK 이 시작되면 실패가 발생하면 안됨. 
     */
        
    STL_ASSERT(0);
    
    return STL_FAILURE;
}


/**
 * @brief 현재 Session 에 XA Context 정보를 Associate 한다.
 * @param[in] aContext   XA Context
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qllXaAssociate( knlXaContext * aContext,
                          qllEnv       * aEnv )
{
    stlInt32   sState = 0;
    
    qllSessionEnv * sSessEnv = QLL_SESS_ENV( aEnv );
    qllDeferrableConstraintInfo * sXaDeferContext = (qllDeferrableConstraintInfo *) aContext->mDeferContext;
    qllDeferrableConstraintInfo * sSessDeferInfo = & sSessEnv->mDeferConstInfo;

    qllSetConstraintModeItem * sSetMode      = NULL;
    qllSetConstraintModeItem * sCurrSetMode  = NULL;
    qllSetConstraintModeItem * sPrevSetMode  = NULL;
    
    qllCollisionItem         * sCollItem     = NULL;
    qllCollisionItem         * sCurrCollItem = NULL;
    qllCollisionItem         * sPrevCollItem = NULL;
    
    /**
     * Parameter Validation
     */

    /* 현재 Session 에 deferred constraint 정보가 존재하지 않아야 함 */
    STL_DASSERT( sSessDeferInfo->mSetModeHistory        == NULL );
    STL_DASSERT( sSessDeferInfo->mTransCollisionHistory == NULL );
    STL_DASSERT( sSessDeferInfo->mTransCollisionSummary == NULL );
    STL_DASSERT( sSessDeferInfo->mStmtCollisionList     == NULL );
    STL_DASSERT( sSessDeferInfo->mNewCollisionSummary   == NULL );

    /********************************************************************
     * Associate : XA Context => Session
     ********************************************************************/
    
    if ( sXaDeferContext == NULL )
    {
        /* XA Context 에 deferrable constraint 정보가 없음 */
    }
    else
    {
        /********************************************************************
         * XA Context 의 Deferred Constraint 정보를 복사 
         ********************************************************************/

        sState = 1;
        
        /**
         * SET CONSTRAINT history
         */
        
        sPrevSetMode = NULL;
        for ( sSetMode = sXaDeferContext->mSetModeHistory;
              sSetMode != NULL;
              sSetMode = sSetMode->mNext )
        {
            STL_TRY( knlAllocDynamicMem( & QLL_COLLISION_MEM(aEnv),
                                         STL_SIZEOF(qllSetConstraintModeItem),
                                         (void **) & sCurrSetMode,
                                         KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            stlMemcpy( sCurrSetMode, sSetMode, STL_SIZEOF(qllSetConstraintModeItem) );
            sCurrSetMode->mNext = NULL;

            if ( sPrevSetMode == NULL )
            {
                sSessDeferInfo->mSetModeHistory = sCurrSetMode;
            }
            else
            {
                sPrevSetMode->mNext = sCurrSetMode;
            }

            sPrevSetMode = sCurrSetMode;
        }

        /**
         * Collision history
         */

        sPrevCollItem = NULL;
        for ( sCollItem = sXaDeferContext->mTransCollisionHistory;
              sCollItem != NULL;
              sCollItem = sCollItem->mNext )
        {
            STL_TRY( knlAllocDynamicMem( & QLL_COLLISION_MEM(aEnv),
                                         STL_SIZEOF(qllCollisionItem),
                                         (void **) & sCurrCollItem,
                                         KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            stlMemcpy( sCurrCollItem, sCollItem, STL_SIZEOF(qllCollisionItem) );
            sCurrCollItem->mNext = NULL;

            if ( sPrevCollItem == NULL )
            {
                sSessDeferInfo->mTransCollisionHistory = sCurrCollItem;
            }
            else
            {
                sPrevCollItem->mNext = sCurrCollItem;
            }

            sPrevCollItem = sCurrCollItem;
        }

        /**
         * Collision summary
         */

        sPrevCollItem = NULL;
        for ( sCollItem = sXaDeferContext->mTransCollisionSummary;
              sCollItem != NULL;
              sCollItem = sCollItem->mNext )
        {
            STL_TRY( knlAllocDynamicMem( & QLL_COLLISION_MEM(aEnv),
                                         STL_SIZEOF(qllCollisionItem),
                                         (void **) & sCurrCollItem,
                                         KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            stlMemcpy( sCurrCollItem, sCollItem, STL_SIZEOF(qllCollisionItem) );
            sCurrCollItem->mNext = NULL;

            if ( sPrevCollItem == NULL )
            {
                sSessDeferInfo->mTransCollisionSummary = sCurrCollItem;
            }
            else
            {
                sPrevCollItem->mNext = sCurrCollItem;
            }

            sPrevCollItem = sCurrCollItem;
        }
        
        /********************************************************************
         * XA Context 의 Deferred Constraint 정보를 제거
         ********************************************************************/

        sState = 2;

        qllClearXaDeferContext( aContext, aEnv );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    switch ( sState )
    {
        case 2:
            break;
        case 1:
            qlrcResetDeferConstraint( aEnv );
            break;
        default:
            break;
    }
    
    return STL_FAILURE;
}



/**
 * @brief 현재 Session 의 Constraint 정보를 XA Context 에 설정한다.
 * @param[in] aContext   XA Context
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qllXaDissociate( knlXaContext * aContext,
                           qllEnv       * aEnv )
{
    stlInt32   sState = 0;

    qllSessionEnv * sSessEnv = QLL_SESS_ENV( aEnv );
    
    qllDeferrableConstraintInfo * sXaDeferContext = NULL;
    qllDeferrableConstraintInfo * sSessDeferInfo = & sSessEnv->mDeferConstInfo;

    qllSetConstraintModeItem * sSetMode      = NULL;
    qllSetConstraintModeItem * sCurrSetMode  = NULL;
    qllSetConstraintModeItem * sPrevSetMode  = NULL;
    
    qllCollisionItem         * sCollItem     = NULL;
    qllCollisionItem         * sCurrCollItem = NULL;
    qllCollisionItem         * sPrevCollItem = NULL;
    
    /**
     * Parameter Validation
     */

    STL_DASSERT( aContext->mDeferContext == NULL );
    
    /* 현재 Session 에 statement constraint 정보가 존재하지 않아야 함 */
    STL_DASSERT( sSessDeferInfo->mStmtCollisionList     == NULL );
    STL_DASSERT( sSessDeferInfo->mNewCollisionSummary   == NULL );

    /********************************************************************
     * Dissociate : Session => XA Context 
     ********************************************************************/
    
    if ( ( sSessDeferInfo->mSetModeHistory        == NULL ) &&
         ( sSessDeferInfo->mTransCollisionHistory == NULL ) &&
         ( sSessDeferInfo->mTransCollisionSummary == NULL ) )
    {
        /* Session 에 deferred constraint 정보가 없는 경우 */

        aContext->mDeferContext = NULL;
    }
    else
    {
        /********************************************************************
         * Session 의 Deferred Constraint 정보를 XA Context 에 복사 
         ********************************************************************/

        /**
         * XA Collision Context 공간 할당
         */
        
        STL_TRY( knlAllocDynamicMem( & gQllSharedEntry->mMemContextXA,
                                     STL_SIZEOF(qllDeferrableConstraintInfo),
                                     (void **) & sXaDeferContext,
                                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        sState = 1;
        
        stlMemset( sXaDeferContext, 0x00, STL_SIZEOF(qllDeferrableConstraintInfo) );

        /**
         * SET CONSTRAINT history
         */
        
        sPrevSetMode = NULL;
        for ( sSetMode = sSessDeferInfo->mSetModeHistory;
              sSetMode != NULL;
              sSetMode = sSetMode->mNext )
        {
            STL_TRY( knlAllocDynamicMem( & gQllSharedEntry->mMemContextXA,
                                         STL_SIZEOF(qllSetConstraintModeItem),
                                         (void **) & sCurrSetMode,
                                         KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            stlMemcpy( sCurrSetMode, sSetMode, STL_SIZEOF(qllSetConstraintModeItem) );
            sCurrSetMode->mNext = NULL;

            if ( sPrevSetMode == NULL )
            {
                sXaDeferContext->mSetModeHistory = sCurrSetMode;
            }
            else
            {
                sPrevSetMode->mNext = sCurrSetMode;
            }

            sPrevSetMode = sCurrSetMode;
        }

        /**
         * Collision history
         */

        sPrevCollItem = NULL;
        for ( sCollItem = sSessDeferInfo->mTransCollisionHistory;
              sCollItem != NULL;
              sCollItem = sCollItem->mNext )
        {
            STL_TRY( knlAllocDynamicMem( & gQllSharedEntry->mMemContextXA,
                                         STL_SIZEOF(qllCollisionItem),
                                         (void **) & sCurrCollItem,
                                         KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            stlMemcpy( sCurrCollItem, sCollItem, STL_SIZEOF(qllCollisionItem) );
            sCurrCollItem->mNext = NULL;

            if ( sPrevCollItem == NULL )
            {
                sXaDeferContext->mTransCollisionHistory = sCurrCollItem;
            }
            else
            {
                sPrevCollItem->mNext = sCurrCollItem;
            }

            sPrevCollItem = sCurrCollItem;
        }

        /**
         * Collision summary
         */

        sPrevCollItem = NULL;
        for ( sCollItem = sSessDeferInfo->mTransCollisionSummary;
              sCollItem != NULL;
              sCollItem = sCollItem->mNext )
        {
            STL_TRY( knlAllocDynamicMem( & gQllSharedEntry->mMemContextXA,
                                         STL_SIZEOF(qllCollisionItem),
                                         (void **) & sCurrCollItem,
                                         KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            stlMemcpy( sCurrCollItem, sCollItem, STL_SIZEOF(qllCollisionItem) );
            sCurrCollItem->mNext = NULL;

            if ( sPrevCollItem == NULL )
            {
                sXaDeferContext->mTransCollisionSummary = sCurrCollItem;
            }
            else
            {
                sPrevCollItem->mNext = sCurrCollItem;
            }

            sPrevCollItem = sCurrCollItem;
        }

        aContext->mDeferContext = (void *) sXaDeferContext;
    }

    /**
     * Session 의 Deferrable Constraint 정보를 제거
     */

    sState = 2;
    qlrcResetDeferConstraint( aEnv );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch ( sState )
    {
        case 2:
        case 1:
            qllClearXaDeferContext( aContext, aEnv );
            break;
        default:
            break;
    }
    
    return STL_FAILURE;
}


/**
 * @brief XA context 의 Deferrable Constraint 메모리를 해제
 * @param[in] aContext   XA Context
 * @param[in] aEnv       Environment
 * @remarks
 */
void qllClearXaDeferContext( knlXaContext * aContext,
                             qllEnv       * aEnv )
{
    qllDeferrableConstraintInfo * sXaDeferContext = (qllDeferrableConstraintInfo *) aContext->mDeferContext;

    qllSetConstraintModeItem * sSetMode      = NULL;
    qllSetConstraintModeItem * sNextSetMode  = NULL;
    
    qllCollisionItem         * sCollItem     = NULL;
    qllCollisionItem         * sNextCollItem = NULL;
    
    STL_TRY_THROW( sXaDeferContext != NULL, RAMP_END );

    /**
     * SET CONSTRAINT history
     */
    
    sSetMode = sXaDeferContext->mSetModeHistory;
    
    while( sSetMode != NULL )
    {
        sNextSetMode = sSetMode->mNext;
        
        if ( knlFreeDynamicMem( & gQllSharedEntry->mMemContextXA, sSetMode, KNL_ENV(aEnv) ) == STL_SUCCESS )
        {
            /* ok */
        }
        else
        {
            STL_DASSERT(0);
        }
                                    
        sSetMode = sNextSetMode;
    }

    /**
     * Collision history
     */

    sCollItem = sXaDeferContext->mTransCollisionHistory;        

    while( sCollItem != NULL )
    {
        sNextCollItem = sCollItem->mNext;

        if ( knlFreeDynamicMem( & gQllSharedEntry->mMemContextXA, sCollItem, KNL_ENV(aEnv) ) == STL_SUCCESS )
        {
            /* ok */
        }
        else
        {
            STL_DASSERT(0);
        }
                                    
        sCollItem = sNextCollItem;
    }

    /**
     * Collision Summary
     */

    sCollItem = sXaDeferContext->mTransCollisionSummary;        

    while( sCollItem != NULL )
    {
        sNextCollItem = sCollItem->mNext;

        if ( knlFreeDynamicMem( & gQllSharedEntry->mMemContextXA, sCollItem, KNL_ENV(aEnv) ) == STL_SUCCESS )
        {
            /* ok */
        }
        else
        {
            STL_DASSERT(0);
        }
                                    
        sCollItem = sNextCollItem;
    }

    /**
     * Collision Context
     */

    if ( knlFreeDynamicMem( & gQllSharedEntry->mMemContextXA, sXaDeferContext, KNL_ENV(aEnv) ) == STL_SUCCESS )
    {
        /* ok */
    }
    else
    {
        STL_DASSERT(0);
    }

    aContext->mDeferContext = NULL;

    STL_RAMP( RAMP_END );
}

/**
 * @brief Global Transaction을 PREPARE한다.
 * @param[in]     aXid        Prepare할 글로벌 트랜잭션 아이디
 * @param[in]     aTransID    Prepare할 로컬 트랜잭션 아이디
 * @param[in]     aWriteMode  Transaction Commit Write Mode
 * @param[in]     aXaContext  XA Context
 * @param[in,out] aEnv        Environment 포인터 
 * @remarks
 */
stlStatus qllPrepare( stlXid       * aXid,
                      smlTransId     aTransID,
                      stlInt64       aWriteMode,
                      knlXaContext * aXaContext,
                      qllEnv       * aEnv )
{
    stlInt64 sWriteMode = aWriteMode;
    
    /**
     * 트랙잭션 PREPARE
     */

    STL_DASSERT( aXaContext != NULL );
    
    STL_TRY_THROW( qlrcHasXaViolatedConstraint( aXaContext ) != STL_TRUE,
                   RAMP_ERR_XA_INTEGRITY_CONSTRAINT_VIOLATION );

    if( aWriteMode == SML_TRANSACTION_CWM_NONE )
    {
        sWriteMode = knlGetPropertyBigIntValueByID( KNL_PROPERTY_TRANSACTION_COMMIT_WRITE_MODE,
                                                    KNL_ENV(aEnv) );
    }

    STL_TRY( smlPrepare( aXid,
                         aTransID,
                         sWriteMode,
                         SML_ENV( aEnv ) ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_XA_INTEGRITY_CONSTRAINT_VIOLATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TRANSACTION_ROLLBACK_INTEGRITY_CONSTRAINT_VIOLATION,
                      NULL,
                      QLL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

inline void qllSetTransAccessMode( qllAccessMode   aAccessMode,
                                   qllEnv        * aEnv )
{
    qllSessionEnv * sSessionEnv;

    sSessionEnv = QLL_SESS_ENV(aEnv);
    
    sSessionEnv->mAccessMode = aAccessMode;
}

inline qllAccessMode qllGetTransAccessMode( qllEnv * aEnv )
{
    return QLL_SESS_ENV(aEnv)->mAccessMode;
}

inline void qllSetTransUniqueMode( qllUniqueMode   aUniqueMode,
                                   qllEnv        * aEnv )
{
    qllSessionEnv * sSessionEnv;

    sSessionEnv = QLL_SESS_ENV(aEnv);
    
    sSessionEnv->mUniqueMode = aUniqueMode;
}

inline qllUniqueMode qllGetTransUniqueMode( qllEnv * aEnv )
{
    return QLL_SESS_ENV(aEnv)->mUniqueMode;
}

/**
 * @brief Session 종료 시 SQL-Session Object 들을 정리한다.
 * @param[in] aEnv    Environment
 * @remarks
 * SQL-Session Object
 * - SQL-Cursor
 * - Local Temporary Table
 * - Global Temporary Table
 */
stlStatus qllFiniSQLSessionObject( qllEnv  * aEnv )
{
    /**
     * SQL-Cursor 를 정리
     */

    STL_TRY( qlcrFiniSQLCursor( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief user handle 의 login authentication 을 검사한다.
 * @param[in] aTransID           Transaction ID
 * @param[in] aStmt              Statement
 * @param[in] aUserHandle        User Handle
 * @param[in] aEncryptPassword   Encrypted Password (nullable)
 * @param[in] aPlainOrgPassword  Plain Text Original Password (nullable)
 * @param[in] aPlainNewPassword  Plain Text New Password (nullable)
 * @param[out] aVersionConflict  Version Conflict 여부
 * @param[out] aHasModifyError   Authentication 정보 변경중 error 발생 여부 
 * @param[in] aEnv               Environemnt
 * @remarks
 */
stlStatus qllCheckLoginAuthentication( smlTransId      aTransID,
                                       smlStatement  * aStmt,
                                       ellDictHandle * aUserHandle,
                                       stlChar       * aEncryptPassword,
                                       stlChar       * aPlainOrgPassword,
                                       stlChar       * aPlainNewPassword,
                                       stlBool       * aVersionConflict,
                                       stlBool       * aHasModifyError,
                                       qllEnv        * aEnv )
{
    stlBool sConflict = STL_FALSE;
    stlBool sHasError = STL_FALSE;
    stlBool sNeedModify = STL_FALSE;
    stlBool sValidPassword = STL_FALSE;
    
    stlTime   sLoginTime = 0;
    stlTime   sCheckTime = 0;

    stlInt64       sTimeInterval = 0;
    
    stlBool   sHasProfile = STL_FALSE;
    stlInt64  sProfileID = ELL_DICT_OBJECT_ID_NA;
    
    ellDictHandle sProfileHandle;
    stlBool       sObjectExist = STL_FALSE;

    stlChar       sEncryptOrgPassword[STL_MAX_SQL_IDENTIFIER_LENGTH] = {0,};

    ellUserProfileParam sProfileParam;
    ellAuthenInfo       sUserAuthenInfo;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aTransID != SML_INVALID_TRANSID, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aUserHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aVersionConflict != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aHasModifyError != NULL, QLL_ERROR_STACK(aEnv) );
    if ( aEncryptPassword == NULL )
    {
        STL_PARAM_VALIDATE( aPlainOrgPassword != NULL, QLL_ERROR_STACK(aEnv) );
        STL_PARAM_VALIDATE( aPlainNewPassword != NULL, QLL_ERROR_STACK(aEnv) );
    }
    else
    {
        STL_PARAM_VALIDATE( aPlainOrgPassword == NULL, QLL_ERROR_STACK(aEnv) );
        STL_PARAM_VALIDATE( aPlainNewPassword == NULL, QLL_ERROR_STACK(aEnv) );
    }
    
    *aHasModifyError = STL_FALSE;
    
    /**************************************************************************
     * 기본 정보 획득
     **************************************************************************/

    /**
     * Login Time 획득
     */

    sLoginTime = stlNow();
    
    /**
     * User 의 Authentication Information 획득
     */

    STL_TRY( ellGetUserAuthenticationInfo( aTransID,
                                           aStmt,
                                           aUserHandle,
                                           & sUserAuthenInfo,
                                           ELL_ENV(aEnv) )
             == STL_SUCCESS );
                                           
    /**
     * User 와 관계된 Profile Parameter 획득
     */
    
    sProfileID = ellGetUserProfileID( aUserHandle );

    if ( sProfileID == ELL_DICT_OBJECT_ID_NA )
    {
        sHasProfile = STL_FALSE;
        
        ellInitDictHandle( & sProfileHandle );

        STL_TRY( ellGetUserProfileParam( aTransID,
                                         aStmt,
                                         NULL,
                                         & sProfileParam,
                                         ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        sHasProfile = STL_TRUE;
        
        STL_TRY( ellGetProfileDictHandleByID( aTransID,
                                              aStmt->mScn,
                                              sProfileID,
                                              & sProfileHandle,
                                              & sObjectExist,
                                              ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        STL_DASSERT( sObjectExist == STL_TRUE );

        STL_TRY( ellGetUserProfileParam( aTransID,
                                         aStmt,
                                         & sProfileHandle,
                                         & sProfileParam,
                                         ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    
    /**************************************************************************
     * Locked Status 검증
     **************************************************************************/

    sHasError = STL_FALSE;
    sNeedModify = STL_FALSE;

    /**
     * Lock Status 검증
     */
    
    switch( sUserAuthenInfo.mLockedStatus )
    {
        case ELL_USER_LOCKED_STATUS_OPEN:
            {
                break;
            }
        case ELL_USER_LOCKED_STATUS_LOCKED:
            {
                sHasError = STL_TRUE;

                stlPushError( STL_ERROR_LEVEL_ABORT,
                              QLL_ERRCODE_ACCOUNT_LOCKED,
                              NULL,
                              QLL_ERROR_STACK(aEnv) );
                
                STL_THROW( RAMP_FINISH );
                break;
            }
        case ELL_USER_LOCKED_STATUS_LOCKED_TIMED:
            {
                /**
                 * Lock Time 경과 검사
                 */

                if ( DTL_IS_NULL( sProfileParam.mLockTime ) == STL_TRUE )
                {
                    sHasError = STL_TRUE;

                    stlPushError( STL_ERROR_LEVEL_ABORT,
                                  QLL_ERRCODE_ACCOUNT_LOCKED,
                                  NULL,
                                  QLL_ERROR_STACK(aEnv) );

                    STL_THROW( RAMP_FINISH );
                }
                else
                {
                    STL_TRY( dtlFuncStlTimeAddNumber( sUserAuthenInfo.mLockedTime,
                                                      sProfileParam.mLockTime,
                                                      & sCheckTime,
                                                      KNL_DT_VECTOR(aEnv),
                                                      aEnv,
                                                      QLL_ERROR_STACK(aEnv) )
                             == STL_SUCCESS );

                    if ( sLoginTime > sCheckTime )
                    {
                        sNeedModify = STL_TRUE;
                        
                        sUserAuthenInfo.mLockedStatus = ELL_USER_LOCKED_STATUS_OPEN;
                        sUserAuthenInfo.mLockedTime   = ELL_DICT_STLTIME_NA;
                        sUserAuthenInfo.mFailedLoginAttempt = 0;
                    }
                    else
                    {
                        sHasError = STL_TRUE;

                        stlPushError( STL_ERROR_LEVEL_ABORT,
                                      QLL_ERRCODE_ACCOUNT_LOCKED,
                                      NULL,
                                      QLL_ERROR_STACK(aEnv) );
                        
                        STL_THROW( RAMP_FINISH );
                    }
                }
                
                break;
            }
        default:
            {
                STL_DASSERT(0);
                break;
            }
    }

    STL_DASSERT( sUserAuthenInfo.mLockedStatus == ELL_USER_LOCKED_STATUS_OPEN );
    
    /**************************************************************************
     * 기존 Password 검증
     **************************************************************************/

    /**
     * 기존 Password 검증
     */
    
    sValidPassword = STL_FALSE;
    
    if ( aEncryptPassword != NULL )
    {
        if ( stlStrcmp( aEncryptPassword, sUserAuthenInfo.mEncryptPassword ) == 0 )
        {
            sValidPassword = STL_TRUE;
        }
        else
        {
            sValidPassword = STL_FALSE;
        }
    }
    else
    {
        STL_TRY( stlMakeEncryptedPassword( aPlainOrgPassword,
                                           stlStrlen( aPlainOrgPassword ),
                                           sEncryptOrgPassword,
                                           STL_MAX_SQL_IDENTIFIER_LENGTH,
                                           QLL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );

        
        if ( stlStrcmp( sEncryptOrgPassword, sUserAuthenInfo.mEncryptPassword ) == 0 )
        {
            sValidPassword = STL_TRUE;
        }
        else
        {
            sValidPassword = STL_FALSE;
        }
    }

    /**
     * Locked Status 변경
     */
    
    if ( sValidPassword == STL_TRUE )
    {
        if ( sUserAuthenInfo.mFailedLoginAttempt == 0 )
        {
            /* ok */
        }
        else
        {
            sNeedModify = STL_TRUE;
            sUserAuthenInfo.mFailedLoginAttempt = 0;
        }
    }
    else
    {
        /**
         * password 가 잘못된 경우
         */
        
        sNeedModify = STL_TRUE;
        sUserAuthenInfo.mFailedLoginAttempt++;

        if ( sHasProfile == STL_TRUE )
        {
            if ( sProfileParam.mFailedLogin == ELL_PROFILE_LIMIT_UNLIMITED )
            {
                /* OPEN 상태를 유지 */
            }
            else
            {
                if ( sProfileParam.mFailedLogin > sUserAuthenInfo.mFailedLoginAttempt )
                {
                    /* OPEN 상태를 유지 */
                }
                else
                {
                    /* LOCKED(TIMED) 상태로 변경 */
                    sUserAuthenInfo.mLockedStatus = ELL_USER_LOCKED_STATUS_LOCKED_TIMED;
                    sUserAuthenInfo.mLockedTime   = sLoginTime;
                }
            }
        }
        else
        {
            /* OPEN 상태를 유지 */
        }
        
        sHasError = STL_TRUE;

        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_LOGIN,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
        
        STL_THROW( RAMP_FINISH );
    }

    /**************************************************************************
     * New Password 검증
     **************************************************************************/

    /**
     * New Password 검증
     */

    if ( aPlainNewPassword == NULL )
    {
        /* nothing to do */
    }
    else
    {
        if ( sHasProfile == STL_TRUE )
        {
            /**
             * Password Verify Function 검증
             */

            STL_TRY( qlaVerifyPassword( ellGetAuthName( aUserHandle ),
                                        aPlainNewPassword,
                                        aPlainOrgPassword,
                                        sProfileParam.mVerifyFuncID,
                                        aEnv )
                     == STL_SUCCESS );

            /**
             * Password Reuse 검증
             */

            if ( ( sProfileParam.mReuseMax == ELL_PROFILE_LIMIT_UNLIMITED ) &&
                 ( DTL_IS_NULL(sProfileParam.mReuseTime) == STL_TRUE ) )
            {
                /**
                 * PASSWORD_REUSE_MAX UNLIMITED
                 * PASSWORD_REUSE_TIME UNLIMITED
                 */

                /* password reuse 를 검사할 필요가 없음 */
            }
            else
            {
                STL_TRY( qlaCheckPasswordReuse( aTransID,
                                                aStmt,
                                                aUserHandle,
                                                & sUserAuthenInfo,
                                                & sProfileParam,
                                                aPlainNewPassword,
                                                sLoginTime,
                                                aEnv )
                         == STL_SUCCESS );
            }
        }
        else
        {
            /**
             * Profile 이 없는 경우
             */
        }
        
        /**
         * PASSWORD EXPIRY 관련 정보 변경
         */

        sNeedModify = STL_TRUE;
        
        sUserAuthenInfo.mExpiryStatus = ELL_USER_EXPIRY_STATUS_OPEN;
        
        if ( sHasProfile == STL_TRUE )
        {
            if ( DTL_IS_NULL( sProfileParam.mLifeTime ) == STL_TRUE )
            {
                sUserAuthenInfo.mExpiryTime = ELL_DICT_STLTIME_NA;
            }
            else
            {
                STL_TRY( dtlFuncStlTimeAddNumber( sLoginTime,
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
            sUserAuthenInfo.mExpiryTime = ELL_DICT_STLTIME_NA;
        }

        STL_TRY( stlMakeEncryptedPassword( aPlainNewPassword,
                                           stlStrlen( aPlainNewPassword ),
                                           sUserAuthenInfo.mEncryptPassword,
                                           STL_MAX_SQL_IDENTIFIER_LENGTH,
                                           QLL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );
        sUserAuthenInfo.mPassChangeCount++;
    }

    /**************************************************************************
     * Expiry Status 검증
     **************************************************************************/

    switch( sUserAuthenInfo.mExpiryStatus )
    {
        case ELL_USER_EXPIRY_STATUS_OPEN:
            {
                /**
                 * Expiry Time 검사
                 */
                
                if ( sUserAuthenInfo.mExpiryTime == ELL_DICT_STLTIME_NA )
                {
                    /* ok */
                }
                else
                {
                    if ( sLoginTime > sUserAuthenInfo.mExpiryTime )
                    {
                        sNeedModify = STL_TRUE;

                        sUserAuthenInfo.mExpiryStatus = ELL_USER_EXPIRY_STATUS_EXPIRED_GRACE;

                        /**
                         * Expiry Time 조정
                         */

                        if ( DTL_IS_NULL( sProfileParam.mGraceTime ) == STL_TRUE )
                        {
                            sUserAuthenInfo.mExpiryTime = ELL_DICT_STLTIME_NA;
                            
                            /* error 가 아니라 warning 임 */
                            stlPushError( STL_ERROR_LEVEL_WARNING,
                                          QLL_ERRCODE_PASSWORD_WILL_EXPIRE_SOON,
                                          NULL,
                                          QLL_ERROR_STACK(aEnv) );
                        }
                        else
                        {
                            STL_TRY( dtlFuncStlTimeAddNumber( sLoginTime,
                                                              sProfileParam.mGraceTime,
                                                              & sUserAuthenInfo.mExpiryTime,
                                                              KNL_DT_VECTOR(aEnv),
                                                              aEnv,
                                                              QLL_ERROR_STACK(aEnv) )
                                     == STL_SUCCESS );

                            /* stlTime 간격을 day 간격으로 변경 */
                            sTimeInterval = sUserAuthenInfo.mExpiryTime - sLoginTime;
                            sTimeInterval = (stlInt64) (sTimeInterval / DTL_USECS_PER_DAY);

                            if ( sTimeInterval == 0 )
                            {
                                /* error 가 아니라 warning 임 */
                                stlPushError( STL_ERROR_LEVEL_WARNING,
                                              QLL_ERRCODE_PASSWORD_WILL_EXPIRE_SOON,
                                              NULL,
                                              QLL_ERROR_STACK(aEnv) );
                            }
                            else
                            {
                                /* error 가 아니라 warning 임 */
                                stlPushError( STL_ERROR_LEVEL_WARNING,
                                              QLL_ERRCODE_PASSWORD_WILL_EXPIRE_N_DAYS,
                                              NULL,
                                              QLL_ERROR_STACK(aEnv),
                                              sTimeInterval );
                            }
                            
                        }
                    }
                    else
                    {
                        /* ok, sLoginTime <= sUserAuthenInfo.mExpiryTime */
                    }
                }
                
                break;
            }
        case ELL_USER_EXPIRY_STATUS_EXPIRED:
            {
                /* shuffle test 에서는 password expire 를 발생시키지 않는다 */
                STL_TRY_THROW( knlGetPropertyBigIntValueByID( KNL_PROPERTY_DATABASE_TEST_OPTION,
                                                              KNL_ENV(aEnv) )
                               < 1, RAMP_ERR_DISALLOW_STATEMENT );
                
                sHasError = STL_TRUE;

                stlPushError( STL_ERROR_LEVEL_ABORT,
                              QLL_ERRCODE_PASSWORD_EXPIRED,
                              NULL,
                              QLL_ERROR_STACK(aEnv) );
                STL_THROW( RAMP_FINISH );
                
                break;
            }
        case ELL_USER_EXPIRY_STATUS_EXPIRED_GRACE:
            {
                /**
                 * Expiry Time 검사
                 */
                
                if ( sUserAuthenInfo.mExpiryTime == ELL_DICT_STLTIME_NA )
                {
                    /* error 가 아니라 warning 임 */
                    stlPushError( STL_ERROR_LEVEL_WARNING,
                                  QLL_ERRCODE_PASSWORD_WILL_EXPIRE_SOON,
                                  NULL,
                                  QLL_ERROR_STACK(aEnv) );
                }
                else
                {
                    if ( sLoginTime > sUserAuthenInfo.mExpiryTime )
                    {
                        /* shuffle test 에서는 password expire 를 발생시키지 않는다 */
                        STL_TRY_THROW( knlGetPropertyBigIntValueByID( KNL_PROPERTY_DATABASE_TEST_OPTION,
                                                                      KNL_ENV(aEnv) )
                                       < 1, RAMP_ERR_DISALLOW_STATEMENT );
                                       
                        sNeedModify = STL_TRUE;
                        sUserAuthenInfo.mExpiryStatus = ELL_USER_EXPIRY_STATUS_EXPIRED;
                        sUserAuthenInfo.mExpiryTime   = sLoginTime;

                        sHasError = STL_TRUE;
                        stlPushError( STL_ERROR_LEVEL_ABORT,
                                      QLL_ERRCODE_PASSWORD_EXPIRED,
                                      NULL,
                                      QLL_ERROR_STACK(aEnv) );

                        STL_THROW( RAMP_FINISH );
                    }
                    else
                    {
                        /* error 가 아니라 warning 임 */
                        stlPushError( STL_ERROR_LEVEL_WARNING,
                                      QLL_ERRCODE_PASSWORD_WILL_EXPIRE_SOON,
                                      NULL,
                                      QLL_ERROR_STACK(aEnv) );
                        
                    }
                }
                
                break;
            }
        default:
            {
                STL_DASSERT(0);
                break;
            }
    }

    /**************************************************************************
     * 마무리 
     **************************************************************************/
    
    STL_RAMP( RAMP_FINISH );

    sConflict = STL_FALSE;
    if ( sNeedModify == STL_TRUE )
    {
        if( ellModifyUserAuthentication4Login( aTransID,
                                               aStmt,
                                               aUserHandle,
                                               & sUserAuthenInfo,
                                               & sConflict,
                                               ELL_ENV(aEnv) )
                 == STL_SUCCESS )
        {
            *aHasModifyError = STL_FALSE;
        }
        else
        {
            *aHasModifyError = STL_TRUE;
            STL_TRY(0);
        }
    }

    if ( sConflict == STL_TRUE )
    {
        /* 다시 수행해야 함 */
        sHasError = STL_FALSE;
        
        STL_INIT_ERROR_STACK( QLL_ERROR_STACK(aEnv) );
    }
    

    if ( sHasError == STL_TRUE )
    {
        STL_TRY(0);
    }
    
    /**
     * Output 설정
     */

    *aVersionConflict = sConflict;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_DISALLOW_STATEMENT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_DISALLOW_STATEMENT,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/** @} qllSession */

