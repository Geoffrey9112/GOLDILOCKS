/*******************************************************************************
 * qldAlterDatabaseRecover.c
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
 * @file qldAlterDatabaseRecover.c
 * @brief ALTER DATABASE RECOVER 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @addtogroup qldAlterDatabaseRecover
 * @{
 */


/**
 * @brief ALTER DATABASE RECOVER 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldValidateAlterDatabaseRecover( smlTransId      aTransID,
                                           qllDBCStmt    * aDBCStmt,
                                           qllStatement  * aSQLStmt,
                                           stlChar       * aSQLString,
                                           qllNode       * aParseTree,
                                           qllEnv        * aEnv )
{
    qlpAlterDatabaseRecover     * sParseTree = NULL;
    knlStartupPhase               sCurrStartupPhase;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_DATABASE_RECOVER_DATABASE_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_DATABASE_RECOVER_DATABASE_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sParseTree = (qlpAlterDatabaseRecover *) aParseTree;

    /**
     * Init Plan 생성
     */

    /**
     * 불완전 복구 option validation
     */
    STL_ASSERT( sParseTree->mRecoveryOption == NULL );
    
    /**
     * 유효성 검사
     */

    sCurrStartupPhase = knlGetCurrStartupPhase();

    STL_TRY_THROW( sCurrStartupPhase == KNL_STARTUP_PHASE_MOUNT,
                   RAMP_ERR_NOT_MOUNTED );

    STL_TRY( smlValidateMediaRecovery( SML_INVALID_TBS_ID,
                                       SML_RECOVERY_OBJECT_TYPE_DATABASE,
                                       SML_ENV( aEnv ) ) == STL_SUCCESS );

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

    aSQLStmt->mInitPlan = NULL;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_MOUNTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_DATABASE_NOT_MOUNTED,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief ALTER DATABASE RECOVER 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldOptCodeAlterDatabaseRecover( smlTransId      aTransID,
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
 * @brief ALTER DATABASE RECOVER 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldOptDataAlterDatabaseRecover( smlTransId      aTransID,
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
 * @brief ALTER DATABASE RECOVER 를 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qldExecuteAlterDatabaseRecover( smlTransId      aTransID,
                                          smlStatement  * aStmt,
                                          qllDBCStmt    * aDBCStmt,
                                          qllStatement  * aSQLStmt,
                                          qllEnv        * aEnv )
{
    smlMediaRecoveryInfo          sMediaRecoveryInfo;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_DATABASE_RECOVER_DATABASE_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * Init Plan 획득 
     */

    /**
     * Media recovery를 수행한다.
     */
    sMediaRecoveryInfo.mRecoveryType = SML_RECOVERY_TYPE_COMPLETE_MEDIA_RECOVERY;
    sMediaRecoveryInfo.mObjectType   = SML_RECOVERY_OBJECT_TYPE_DATABASE;
    sMediaRecoveryInfo.mTbsId        = SML_INVALID_TBS_ID;
    sMediaRecoveryInfo.mImrOption    = SML_IMR_OPTION_NONE;
    sMediaRecoveryInfo.mUntilValue   = -1;
    sMediaRecoveryInfo.mImrCondition = SML_IMR_CONDITION_NONE;

    STL_TRY( knlAddEnvEvent( SML_EVENT_MEDIA_RECOVER,
                             NULL,                               /* aEventMem */
                             &sMediaRecoveryInfo,                /* aData */
                             STL_SIZEOF(smlMediaRecoveryInfo),   /* aDataSize */
                             0,                                  /* aTargetEnvId */
                             KNL_EVENT_WAIT_POLLING,
                             STL_FALSE, /* aForceSuccess */
                             KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/** @} qldAlterDatabaseRecover */
