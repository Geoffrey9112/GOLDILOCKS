/*******************************************************************************
 * qlssAlterSystemStartupDatabase.c
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
 * @file qlssAlterSystemStartupDatabase.c
 * @brief ALTER SYSTEM {MOUNT|OPEN} DATABASE 처리 루틴 
 */

#include <qll.h>
#include <qlDef.h>



/**
 * @defgroup qlssAlterSystemStartupDatabase ALTER SYSTEM MOUNT/OPEN DATABASE
 * @ingroup qlssSystem
 * @{
 */


/**
 * @brief ALTER SYSTEM MOUNT/OPEN DATABASE 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssValidateSystemStartupDatabase( smlTransId      aTransID,
                                             qllDBCStmt    * aDBCStmt,
                                             qllStatement  * aSQLStmt,
                                             stlChar       * aSQLString,
                                             qllNode       * aParseTree,
                                             qllEnv        * aEnv )
{
    qlssInitSystemStartupDatabase  * sInitPlan = NULL;
    qlpAlterSystemStartupDatabase  * sParseTree = NULL;
    knlStartupPhase                  sCurrStartupPhase;
    stlInt64                         sDatabaseTestOption = 0;

    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    sDatabaseTestOption = knlGetPropertyBigIntValueByID( KNL_PROPERTY_DATABASE_TEST_OPTION,
                                                         KNL_ENV(aEnv) );

    STL_TRY_THROW( sDatabaseTestOption < 1, RAMP_ERR_DISALLOW_STATEMENT );
    
    /**
     * 기본 정보 획득 
     */
    
    sParseTree = (qlpAlterSystemStartupDatabase *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlssInitSystemStartupDatabase),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlssInitSystemStartupDatabase) );

    sInitPlan->mStartupPhase = sParseTree->mStartupPhase;
    sInitPlan->mOpenOption = SML_DATA_ACCESS_MODE_NONE;
    sInitPlan->mLogOption = SML_STARTUP_LOG_OPTION_NORESETLOGS;

    if( sInitPlan->mStartupPhase == KNL_STARTUP_PHASE_OPEN )
    {
        if( sParseTree->mOpenOption != NULL )
        {
            sInitPlan->mOpenOption = QLP_GET_INT_VALUE( sParseTree->mOpenOption );
        }

        sInitPlan->mLogOption = (stlChar)QLP_GET_INT_VALUE( sParseTree->mLogOption );
    }

    /**
     * 유효성 검사
     */

    sCurrStartupPhase = knlGetCurrStartupPhase();

    STL_TRY_THROW( sCurrStartupPhase < sInitPlan->mStartupPhase,
                   RAMP_ERR_INVALID_STARTUP_PHASE );

    if( sInitPlan->mStartupPhase == KNL_STARTUP_PHASE_MOUNT )
    {
        STL_TRY( smlValidateMountDatabase( SML_ENV(aEnv) ) == STL_SUCCESS );
    }

    if( sInitPlan->mStartupPhase == KNL_STARTUP_PHASE_OPEN )
    {
        STL_TRY_THROW( sCurrStartupPhase == KNL_STARTUP_PHASE_MOUNT,
                       RAMP_ERR_NOT_MOUNTED );
        
        STL_TRY( smlValidateOpenDatabase( sInitPlan->mOpenOption,
                                          sInitPlan->mLogOption,
                                          SML_ENV(aEnv) )
                 == STL_SUCCESS );
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

    STL_CATCH( RAMP_ERR_INVALID_STARTUP_PHASE )
    {
        if( sCurrStartupPhase == KNL_STARTUP_PHASE_MOUNT )
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          QLL_ERRCODE_INVALID_MOUNTABLE_PHASE,
                          NULL,
                          KNL_ERROR_STACK(aEnv) );
        }
        else
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          QLL_ERRCODE_INVALID_OPENABLE_PHASE,
                          NULL,
                          KNL_ERROR_STACK(aEnv) );
        }
    }

    STL_CATCH( RAMP_ERR_NOT_MOUNTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_DATABASE_NOT_MOUNTED,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }

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

/**
 * @brief ALTER SYSTEM MOUNT/OPEN DATABASE 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssOptCodeSystemStartupDatabase( smlTransId      aTransID,
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
 * @brief ALTER SYSTEM MOUNT/OPEN DATABASE 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssOptDataSystemStartupDatabase( smlTransId      aTransID,
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
 * @brief ALTER SYSTEM MOUNT/OPEN DATABASE 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qlssExecuteSystemStartupDatabase( smlTransId      aTransID,
                                            smlStatement  * aStmt,
                                            qllDBCStmt    * aDBCStmt,
                                            qllStatement  * aSQLStmt,
                                            qllEnv        * aEnv )
{
    qlssInitSystemStartupDatabase  * sInitPlan = NULL;
    knlStartupInfo                   sStartupInfo;
    knlStartupPhase                  sCurStartupPhase;

    ellDictHandle sLoginHandle;
    ellDictHandle sRoleHandle;

    stlBool       sObjectExist = STL_FALSE;
    
    ellDictHandle * sCurrentRole;
    stlInt64        sRoleID = ELL_DICT_OBJECT_ID_NA;

    void          * sSpaceIter = NULL;
    stlChar         sSpaceName[STL_MAX_SQL_IDENTIFIER_LENGTH + 1];
    stlUInt32       sSpaceAttr = 0;
    smlTbsId        sSpaceID   = 0;
    ellDictHandle   sSpaceHandle;
    stlBool         sTbsExist = STL_FALSE;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    
    /**
     * Init Plan 획득 
     */

    sInitPlan = (qlssInitSystemStartupDatabase *) aSQLStmt->mInitPlan;

    KNL_INIT_STARTUP_ARG( &sStartupInfo );
    sStartupInfo.mStartupPhase    = sInitPlan->mStartupPhase;
    sStartupInfo.mDataAccessMode  = sInitPlan->mOpenOption;
    sStartupInfo.mLogOption       = sInitPlan->mLogOption;

    sCurStartupPhase = knlGetCurrStartupPhase();

    KNL_BREAKPOINT( KNL_BREAKPOINT_STARTUPDATABASE_BEFORE_ADDENVEVENT,
                    KNL_ENV(aEnv) );

    STL_TRY( knlAddEnvEvent( KNL_EVENT_STARTUP,
                             NULL,                         /* aEventMem */
                             &sStartupInfo,                /* aData */
                             STL_SIZEOF(knlStartupInfo),   /* aDataSize */
                             0,                            /* aTargetEnvId */
                             KNL_EVENT_WAIT_POLLING,
                             STL_FALSE, /* aForceSuccess */
                             KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    KNL_BREAKPOINT( KNL_BREAKPOINT_STARTUPDATABASE_AFTER_ADDENVEVENT,
                    KNL_ENV(aEnv) );

    sCurStartupPhase = knlGetCurrStartupPhase();

    /**
     * Start-Up 과정에 부합하는 Dump Handle 을 재설정함
     */
    
    STL_TRY( ellSetDictionaryDumpHandle( ELL_ENV(aEnv) ) == STL_SUCCESS );

    /**
     * Startup 과정중 System Scn이 상승될수 있고, Dictionary는
     * 최신 View을 봐야하기 때문에 View SCN 을 최신으로 갱신한다.
     */
    
    aSQLStmt->mViewSCN = smlGetSystemScn();
    
    /**
     * SYS --AS role(SYSDBA, ADMIN) authorization descriptor 재설정 
     */
    
    STL_TRY( ellGetAuthDictHandleByID( SML_INVALID_TRANSID,
                                       aSQLStmt->mViewSCN,
                                       ellGetAuthIdSYS(),
                                       & sLoginHandle,
                                       & sObjectExist,
                                       ELL_ENV(aEnv) )
             == STL_SUCCESS );
    STL_DASSERT( sObjectExist == STL_TRUE );

    sCurrentRole = ellGetSessionRoleHandle( ELL_ENV(aEnv) );
    sRoleID = ellGetAuthID( sCurrentRole );

    STL_TRY( ellGetAuthDictHandleByID( SML_INVALID_TRANSID,
                                       aSQLStmt->mViewSCN,
                                       sRoleID,
                                       & sRoleHandle,
                                       & sObjectExist,
                                       ELL_ENV(aEnv) )
             == STL_SUCCESS );
    STL_ASSERT( sObjectExist == STL_TRUE );

    /**
     * OPEN DATABASE RESET 성공 후 Dictionary에 없지만, Controlfile에 존재하는 Tbs 정리한다.
     */ 
    if( sInitPlan->mLogOption == SML_STARTUP_LOG_OPTION_RESETLOGS )
    {
        STL_TRY( smlFetchFirstTbsInfo( &sSpaceIter,
                                       &sSpaceID,
                                       sSpaceName,
                                       &sSpaceAttr,
                                       SML_ENV( aEnv ) ) == STL_SUCCESS );

        while( sSpaceIter != NULL )
        {
            STL_TRY( ellGetTablespaceDictHandleByID( aTransID,
                                                     aSQLStmt->mViewSCN,
                                                     sSpaceID,
                                                     &sSpaceHandle,
                                                     &sTbsExist,
                                                     ELL_ENV( aEnv) ) == STL_SUCCESS );
            if( sTbsExist == STL_FALSE )
            {
                STL_TRY( smlDropUnregisteredTablespace( sSpaceID,
                                                        SML_ENV( aEnv ) ) == STL_SUCCESS );
            }
            
            STL_TRY( smlFetchNextTbsInfo( &sSpaceIter,
                                          &sSpaceID,
                                          sSpaceName,
                                          &sSpaceAttr,
                                          SML_ENV( aEnv ) ) == STL_SUCCESS );
        }
    }

    /**
     * Session 정보 설정
     */
    
    ellSetLoginUser( & sLoginHandle, ELL_ENV(aEnv) );
    ellSetSessionRole( & sRoleHandle, ELL_ENV(aEnv) );

    if( sCurStartupPhase == KNL_STARTUP_PHASE_OPEN )
    {
        aSQLStmt->mDCLContext.mType = QLL_DCL_TYPE_OPEN_DATABASE;
    }

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV(aEnv),
                        KNL_LOG_LEVEL_INFO,
                        "[STARTUP] %s PHASE\n",
                        gPhaseString[sCurStartupPhase] )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} qlssAlterSystemStartupDatabase */


