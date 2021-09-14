/*******************************************************************************
 * qlssAlterSystemShutdownDatabase.c
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
 * @file qlssAlterSystemShutdownDatabase.c
 * @brief ALTER SYSTEM CLOSE DATABASE 처리 루틴 
 */

#include <qll.h>
#include <qlDef.h>



/**
 * @defgroup qlssAlterSystemShutdownDatabase ALTER SYSTEM CLOSE DATABASE
 * @ingroup qlssSystem
 * @{
 */


/**
 * @brief ALTER SYSTEM CLOSE DATABASE 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssValidateSystemShutdownDatabase( smlTransId      aTransID,
                                              qllDBCStmt    * aDBCStmt,
                                              qllStatement  * aSQLStmt,
                                              stlChar       * aSQLString,
                                              qllNode       * aParseTree,
                                              qllEnv        * aEnv )
{
    qlssInitSystemShutdownDatabase  * sInitPlan = NULL;
    qlpAlterSystemShutdownDatabase  * sParseTree = NULL;
    stlInt32                          sDataStoreMode;
    stlInt64                          sDatabaseTestOption = 0;
    knlSessionEnv                   * sSessEnv;

    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * shared mode에서는 shutdown 못하도록 함.
     */
    sSessEnv = KNL_SESS_ENV(aEnv);
    STL_TRY_THROW( sSessEnv->mSessionType != KNL_SESSION_TYPE_SHARED, RAMP_ERR_SHUTDOWN_MUST_DEDICATED );

    sDatabaseTestOption = knlGetPropertyBigIntValueByID( KNL_PROPERTY_DATABASE_TEST_OPTION,
                                                         KNL_ENV(aEnv) );

    STL_TRY_THROW( sDatabaseTestOption < 1, RAMP_ERR_DISALLOW_STATEMENT );
    
    /**
     * 기본 정보 획득 
     */
    
    sParseTree = (qlpAlterSystemShutdownDatabase *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlssInitSystemShutdownDatabase),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlssInitSystemShutdownDatabase) );

    sInitPlan->mShutdownPhase = sParseTree->mShutdownPhase;
    sInitPlan->mCloseOption   = KNL_SHUTDOWN_MODE_NORMAL;

    if( sParseTree->mCloseOption != NULL )
    {
        sInitPlan->mCloseOption = QLP_GET_INT_VALUE( sParseTree->mCloseOption );
    }

    /**
     * 유효성 검사
     */

    sDataStoreMode = smlGetDataStoreMode();

    /**
     * CDS MODE는 Shutdown Mode가 ABORT/OPERATIONAL 이면 안된다.
     */
    if( sDataStoreMode == SML_DATA_STORE_MODE_CDS )
    {
        STL_TRY_THROW( sInitPlan->mCloseOption != KNL_SHUTDOWN_MODE_ABORT,
                       RAMP_ERR_CDS_ABORT_CLOSE_OPTION );
        STL_TRY_THROW( sInitPlan->mCloseOption != KNL_SHUTDOWN_MODE_OPERATIONAL,
                       RAMP_ERR_CDS_OPERATIONAL_CLOSE_OPTION );
    }

    /**
     * ADMINISTRATION ON DATABASE 권한 검사
     */

    STL_TRY( qlaCheckDatabasePriv( aTransID,
                                   aDBCStmt,
                                   aSQLStmt,
                                   ELL_DB_PRIV_ACTION_ADMINISTRATION,
                                   aEnv )
             == STL_SUCCESS );
    
    
    /**
     * Init Plan 연결 
     */

    aSQLStmt->mInitPlan = (void *) sInitPlan;
        
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CDS_ABORT_CLOSE_OPTION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CDS_ABORT_CLOSE_OPTION,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mCloseOption->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_CATCH( RAMP_ERR_CDS_OPERATIONAL_CLOSE_OPTION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CDS_OPERATIONAL_CLOSE_OPTION,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mCloseOption->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_CATCH( RAMP_ERR_DISALLOW_STATEMENT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_DISALLOW_STATEMENT,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_SHUTDOWN_MUST_DEDICATED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SHUTDOWN_MUST_DEDICATED,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief ALTER SYSTEM CLOSE DATABASE 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssOptCodeSystemShutdownDatabase( smlTransId      aTransID,
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
 * @brief ALTER SYSTEM CLOSE DATABASE 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssOptDataSystemShutdownDatabase( smlTransId      aTransID,
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
 * @brief ALTER SYSTEM CLOSE DATABASE 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qlssExecuteSystemShutdownDatabase( smlTransId      aTransID,
                                             smlStatement  * aStmt,
                                             qllDBCStmt    * aDBCStmt,
                                             qllStatement  * aSQLStmt,
                                             qllEnv        * aEnv )
{
    stlInt64                         sKey = 0;
    stlChar                          sSemName[STL_MAX_NAMED_SEM_NAME+1];
    stlNamedSemaphore                sSem;
    stlInt32                         sState = 0;
    qlssInitSystemShutdownDatabase * sInitPlan = NULL;
    stlBool                          sPreventBackup = STL_FALSE;
    knlStartupPhase                  sCurStartupPhase;
    smlBackupMessage                 sBackupMsg;
    knlShutdownEventArg              sShutdownEventArg;

    /**
     * Init Plan 획득 
     */
    
    sInitPlan = (qlssInitSystemShutdownDatabase *) aSQLStmt->mInitPlan;

    KNL_INIT_SHUTDOWN_EVENT_ARG( &sShutdownEventArg );
    sShutdownEventArg.mShutdownMode = sInitPlan->mCloseOption;
    
    /**
     * 새로운 Session및 Warmup의 접근을 막는다.
     */
    STL_TRY( knlUnsetSar( KNL_SAR_WARMUP | KNL_SAR_SESSION,
                          KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 1;

    /**
     * SHUTDOWN ABORT을 제외한 option에서는 자신의 Transaction이 Active가 아니여야 한다.
     */

    if( sShutdownEventArg.mShutdownMode > KNL_SHUTDOWN_MODE_ABORT )
    {
        STL_TRY_THROW( aTransID == SML_INVALID_TRANSID, RAMP_ERR_INPROGRESS_TRANSACTION );
    }

    /**
     * MOUNT Phase에서 Recovery가 진행될 수 없도록 한다.
     */
    sCurStartupPhase = knlGetCurrStartupPhase();

    if( sCurStartupPhase == KNL_STARTUP_PHASE_MOUNT )
    {
        STL_TRY( smlSetRecoveryPhaseAtMount( SML_RECOVERY_AT_MOUNT_RECOVERY_PREVENT,
                                             SML_ENV( aEnv ) )
                 == STL_SUCCESS );
    }

    /**
     * Backup이 진행 중이면 shutdown을 수행할 수 없다.
     */
    if( sShutdownEventArg.mShutdownMode > KNL_SHUTDOWN_MODE_ABORT )
    {
        /**
         * Backup이 실행될 수 있는 경우 backup이 진행 중이면 shutdown이 실패하고,
         * backup이 진행중이지 않으면 backup 불가능 상태로 변경한다.
         */
        if( (smlIsArchivelogMode() == STL_TRUE) &&
            (sCurStartupPhase >= KNL_STARTUP_PHASE_OPEN) )
        {
            stlMemset( &sBackupMsg, 0x00, STL_SIZEOF( smlBackupMessage ) );

            sBackupMsg.mBackupObject = SML_BACKUP_OBJECT_TYPE_NONE;
            sBackupMsg.mCommand      = SML_BACKUP_COMMAND_PREVENT;

            STL_TRY( smlBackup( &sBackupMsg, SML_ENV( aEnv ) ) == STL_SUCCESS );

            sPreventBackup = STL_TRUE;
        }
    }

    KNL_BREAKPOINT( KNL_BREAKPOINT_SHUTDOWNDATABASE_AFTER_PREVENTBACKUP,
                    KNL_ENV(aEnv) );

    if( sShutdownEventArg.mShutdownMode >= KNL_SHUTDOWN_MODE_NORMAL )
    {
        /**
         * Active session의 종료를 기다림
         */
        STL_TRY( qlssWaitAllUserSession( aEnv ) == STL_SUCCESS );
    }
        
    /**
     * 새로운 Transaction의 접근을 막는다.
     */
    STL_TRY( knlUnsetSar( KNL_SAR_TRANSACTION, KNL_ENV(aEnv) ) == STL_SUCCESS );
    sState = 2;

    if( sShutdownEventArg.mShutdownMode >= KNL_SHUTDOWN_MODE_TRANSACTIONAL )
    {
        /**
         * Active transaction의 종료를 기다림
         */
        STL_TRY( smlWaitAllTransactionEnd( SML_ENV(aEnv) ) == STL_SUCCESS );
    }

    /**
     * 모든 Session을 강제로 종료 시킴.
     */
    STL_TRY( qlssCloseAllUserSession( aEnv ) == STL_SUCCESS );
        
    if( sShutdownEventArg.mShutdownMode >= KNL_SHUTDOWN_MODE_OPERATIONAL )
    {
        /**
         * 모든 Active Statement를 기다린다.
         */
        STL_TRY( qlssWaitAllActiveStatement( aEnv ) == STL_SUCCESS );
        
        /**
         * Cleanup Thread가 Dead session의 active transaction들을 rollback할때까지 대기한다.
         */
        
        STL_TRY( smlWaitAllTransactionEnd( SML_ENV(aEnv) ) == STL_SUCCESS );
    }
        
    if( sShutdownEventArg.mShutdownMode >= KNL_SHUTDOWN_MODE_IMMEDIATE )
    {
        /**
         * 실패 불가능한 critical section 시간을 줄이기 위해서 더티된 페이지를
         * 디스크로 기록한다.
         */

        STL_TRY( smlFlushPages4Shutdown( SML_ENV(aEnv) ) == STL_SUCCESS );
    }

    STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_SHARED_MEMORY_KEY,
                                      &sKey,
                                      KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlSnprintf( sSemName, STL_MAX_NAMED_SEM_NAME+1, "%d", (stlInt32)sKey );

    STL_TRY( stlCreateNamedSemaphore( &sSem,
                                      sSemName,
                                      0,
                                      KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    sState = 3;

    /**
     * Shutdown database
     * - 이후는 실패 불가능한 Critical Section 이다.
     */

    sShutdownEventArg.mShutdownPhase = KNL_SHUTDOWN_PHASE_INIT;
    
    STL_TRY( knlAddEnvEvent( KNL_EVENT_SHUTDOWN,
                             NULL,                             /* aEventMem */
                             (void *)&sShutdownEventArg,       /* aData */
                             STL_SIZEOF(knlShutdownEventArg),  /* aDataSize */
                             0,                                /* aTargetEnvId */
                             KNL_EVENT_WAIT_NO,
                             STL_FALSE, /* aForceSuccess */
                             KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    STL_TRY( stlAcquireNamedSemaphore( &sSem,
                                       KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    sState = 2;
    STL_TRY( stlDestroyNamedSemaphore( &sSem,
                                       KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    /**
     * 상태 전이를 위한 Context 정보 설정
     */
    aSQLStmt->mDCLContext.mType = QLL_DCL_TYPE_CLOSE_DATABASE;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INPROGRESS_TRANSACTION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INPROGRESS_TRANSACTION,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    if( sPreventBackup == STL_TRUE )
    {
        stlMemset( &sBackupMsg, 0x00, STL_SIZEOF( smlBackupMessage ) );

        sBackupMsg.mBackupObject = SML_BACKUP_OBJECT_TYPE_NONE;
        sBackupMsg.mCommand      = SML_BACKUP_COMMAND_PREVENT_ROLLBACK;

        (void)smlBackup( &sBackupMsg, SML_ENV( aEnv ) );
    }

    switch( sState )
    {
        case 3:
            (void)stlDestroyNamedSemaphore( &sSem, KNL_ERROR_STACK(aEnv) );
        case 2:
            (void) knlAddSar( KNL_SAR_TRANSACTION, KNL_ENV(aEnv) );
        case 1:
            (void) knlAddSar( KNL_SAR_WARMUP | KNL_SAR_SESSION, KNL_ENV(aEnv) );
        default:
            break;
    }

    return STL_FAILURE;
}


/**
 * @brief 모든 사용자 세션들의 종료를 기다림.
 * @param[in] aEnv Environment Pointer
 */
stlStatus qlssWaitAllUserSession( qllEnv * aEnv )
{
    knlSessionEnv * sSessEnv;
    qllSessionEnv * sMySessEnv;
    stlBool         sExistActiveSession;

    sMySessEnv = QLL_SESS_ENV( aEnv );

    STL_DASSERT( sMySessEnv != NULL );

    while( 1 )
    {
        sSessEnv = knlGetFirstSessionEnv();

        sExistActiveSession = STL_FALSE;
        
        while( sSessEnv != NULL )
        {
            if( (sMySessEnv != (qllSessionEnv*)sSessEnv) &&
                (knlIsUsedSessionEnv(sSessEnv) == STL_TRUE) &&
                (KNL_IS_USER_LAYER(knlGetSessionTopLayer(sSessEnv)) == STL_TRUE) )
            {
                sExistActiveSession = STL_TRUE;
                break;
            }

            sSessEnv = knlGetNextSessionEnv( sSessEnv );
        }

        if( sExistActiveSession == STL_FALSE )
        {
            break;
        }

        STL_TRY( knlCheckEndSession( sMySessEnv, KNL_ENV(aEnv) ) == STL_SUCCESS );
        STL_TRY( knlCheckEndStatement( KNL_ENV(aEnv) ) == STL_SUCCESS );
        
        stlSleep( 10000 );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 모든 사용자 세션을 닫는다.
 * @param[in] aEnv Environment Pointer
 */
stlStatus qlssCloseAllUserSession( qllEnv * aEnv )
{
    knlSessionEnv * sSessEnv;
    qllSessionEnv * sMySessEnv;

    sMySessEnv = QLL_SESS_ENV( aEnv );

    sSessEnv = knlGetFirstSessionEnv();

    while( sSessEnv != NULL )
    {
        if( (sMySessEnv != (qllSessionEnv*)sSessEnv) &&
            (knlIsUsedSessionEnv(sSessEnv) == STL_TRUE) &&
            (KNL_IS_USER_LAYER(knlGetSessionTopLayer(sSessEnv)) == STL_TRUE) )
        {
            STL_TRY( knlEndSession( sSessEnv,
                                    KNL_END_SESSION_OPTION_KILL,
                                    KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            knlKillSession( sSessEnv );
        }

        sSessEnv = knlGetNextSessionEnv( sSessEnv );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 모든 Active Statement의 종료를 기다림.
 * @param[in] aEnv Environment Pointer
 */
stlStatus qlssWaitAllActiveStatement( qllEnv * aEnv )
{
    knlSessionEnv * sSessEnv;
    qllSessionEnv * sMySessEnv;
    stlBool         sExistActiveSession;

    sMySessEnv = QLL_SESS_ENV( aEnv );

    STL_DASSERT( sMySessEnv != NULL );

    while( 1 )
    {
        sSessEnv = knlGetFirstSessionEnv();

        sExistActiveSession = STL_FALSE;
        
        while( sSessEnv != NULL )
        {
            if( (sMySessEnv != (qllSessionEnv*)sSessEnv) &&
                (knlIsUsedSessionEnv(sSessEnv) == STL_TRUE) &&
                (KNL_IS_USER_LAYER(knlGetSessionTopLayer(sSessEnv)) == STL_TRUE) &&
                (KNL_GET_SESSION_ACCESS_MASK(sSessEnv) == KNL_SESSION_ACCESS_ENTER) )
            {
                sExistActiveSession = STL_TRUE;
                break;
            }

            sSessEnv = knlGetNextSessionEnv( sSessEnv );
        }

        if( sExistActiveSession == STL_FALSE )
        {
            break;
        }

        STL_TRY( knlCheckEndSession( sMySessEnv, KNL_ENV(aEnv) ) == STL_SUCCESS );
        STL_TRY( knlCheckEndStatement( KNL_ENV(aEnv) ) == STL_SUCCESS );
        
        stlSleep( 10000 );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlssAlterSystemShutdownDatabase */


