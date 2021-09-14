/*******************************************************************************
 * qldAlterDatabaseRestore.c
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
 * @file qldAlterDatabaseRestore.c
 * @brief ALTER DATABASE RESTORE 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @addtogroup qldAlterDatabaseRestore
 * @{
 */


/**
 * @brief ALTER DATABASE RESTORE 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldValidateAlterDatabaseRestore( smlTransId      aTransID,
                                           qllDBCStmt    * aDBCStmt,
                                           qllStatement  * aSQLStmt,
                                           stlChar       * aSQLString,
                                           qllNode       * aParseTree,
                                           qllEnv        * aEnv )
{
    qldInitAlterDatabaseRestore * sInitPlan = NULL;
    qlpRestoreOption            * sRestoreOption;
    qlpAlterDatabaseRestore     * sParseTree = NULL;
    knlStartupPhase               sCurrStartupPhase;
    stlBool                       sBackupExist = STL_FALSE;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_DATABASE_RESTORE_DATABASE_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_DATABASE_RESTORE_DATABASE_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sParseTree = (qlpAlterDatabaseRestore *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qldInitAlterDatabaseRestore),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qldInitAlterDatabaseRestore) );

    sRestoreOption = (qlpRestoreOption *)sParseTree->mRestoreOption;
    sInitPlan->mUntilType = sRestoreOption->mUntilType;
    sInitPlan->mUntilValue = -1;

    switch( sInitPlan->mUntilType )
    {
        case SML_IMR_CONDITION_TIME:
            STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
            /* STL_TRY( dtlGetIntegerFromString( */
            /*              QLP_GET_PTR_VALUE(sRestoreOption->mUntilValue), */
            /*              stlStrlen( QLP_GET_PTR_VALUE( */
            /*                             sRestoreOption->mUntilValue) ), */
            /*              & sInitPlan->mUntilValue, */
            /*              QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS ); */
            break;
        case SML_IMR_CONDITION_CHANGE:
            STL_TRY( dtlGetIntegerFromString(
                         QLP_GET_PTR_VALUE(sRestoreOption->mUntilValue),
                         stlStrlen( QLP_GET_PTR_VALUE(
                                        sRestoreOption->mUntilValue) ),
                         & sInitPlan->mUntilValue,
                         QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS );
            break;
        case SML_IMR_CONDITION_NONE:
            break;
        default:
            STL_ASSERT( 0 );
    }

    /**
     * 유효성 검사
     */

    sCurrStartupPhase = knlGetCurrStartupPhase();

    STL_TRY_THROW( sCurrStartupPhase == KNL_STARTUP_PHASE_MOUNT,
                   RAMP_ERR_NOT_MOUNTED );

    STL_TRY( smlExistValidIncBackup( SML_INVALID_TBS_ID,
                                     &sBackupExist,
                                     SML_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sBackupExist == STL_TRUE, RAMP_ERR_NOT_EXIST_VALID_BACKUP );

    /**
     * ALTER DATABASE ON DATABASE 권한 검사
     */

    STL_TRY( qlaCheckDatabasePriv( aTransID,
                                   aDBCStmt,
                                   aSQLStmt,
                                   ELL_DB_PRIV_ACTION_ALTER_DATABASE,
                                   aEnv )
             == STL_SUCCESS );

    /**
     * Init Plan 연결 
     */

    aSQLStmt->mInitPlan = (void *) sInitPlan;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_MOUNTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_DATABASE_NOT_MOUNTED,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      "DATABASE RESTORE UNTIL TIME" );
    }

    STL_CATCH( RAMP_ERR_NOT_EXIST_VALID_BACKUP )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_EXIST_VALID_INCREMENTAL_BACKUP,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief ALTER DATABASE RESTORE 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldOptCodeAlterDatabaseRestore( smlTransId      aTransID,
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
 * @brief ALTER DATABASE RESTORE 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldOptDataAlterDatabaseRestore( smlTransId      aTransID,
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
 * @brief ALTER DATABASE RESTORE 를 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qldExecuteAlterDatabaseRestore( smlTransId      aTransID,
                                          smlStatement  * aStmt,
                                          qllDBCStmt    * aDBCStmt,
                                          qllStatement  * aSQLStmt,
                                          qllEnv        * aEnv )
{
    qldInitAlterDatabaseRestore * sInitPlan = NULL;
    smlRestoreInfo                sRestoreInfo;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_DATABASE_RESTORE_DATABASE_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * Init Plan 획득 
     */

    sInitPlan = (qldInitAlterDatabaseRestore *) aSQLStmt->mInitPlan;

    /**
     * Database Restore를 수행한다.
     */

    sRestoreInfo.mTbsId      = SML_INVALID_TBS_ID;
    sRestoreInfo.mUntilType  = sInitPlan->mUntilType;
    sRestoreInfo.mUntilValue = sInitPlan->mUntilValue;

    STL_TRY( knlAddEnvEvent( SML_EVENT_RESTORE,
                             NULL,                         /* aEventMem */
                             &sRestoreInfo,                /* aData */
                             STL_SIZEOF(smlRestoreInfo),   /* aDataSize */
                             0,                            /* aTargetEnvId */
                             KNL_EVENT_WAIT_POLLING,
                             STL_FALSE, /* aForceSuccess */
                             KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} qldAlterDatabaseRestore */
