/*******************************************************************************
 * qldAlterDatabaseRecoverTablespace.c
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
 * @file qldAlterDatabaseRecoverTablespace.c
 * @brief ALTER DATABASE RECOVER TABLESPACE 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @addtogroup qldAlterDatabaseRecoverTablespace
 * @{
 */


/**
 * @brief ALTER DATABASE RECOVER TABLESPACE 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldValidateAlterDatabaseRecoverTablespace( smlTransId      aTransID,
                                                     qllDBCStmt    * aDBCStmt,
                                                     qllStatement  * aSQLStmt,
                                                     stlChar       * aSQLString,
                                                     qllNode       * aParseTree,
                                                     qllEnv        * aEnv )
{
    qldInitAlterDatabaseRecoverTablespace * sInitPlan = NULL;
    qlpAlterDatabaseRecoverTablespace     * sParseTree = NULL;
    stlBool                                 sObjectExist = STL_FALSE;
    stlInt64                                sSpaceID;
    knlStartupPhase                         sStartupPhase = KNL_STARTUP_PHASE_NONE;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_DATABASE_RECOVER_TABLESPACE_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_DATABASE_RECOVER_TABLESPACE_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sParseTree = (qlpAlterDatabaseRecoverTablespace *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qldInitAlterDatabaseRecoverTablespace),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qldInitAlterDatabaseRecoverTablespace) );

    STL_TRY( qlvSetInitPlan( aDBCStmt,
                             aSQLStmt,
                             & sInitPlan->mCommonInit,
                             aEnv )
             == STL_SUCCESS );
    
    /*************************************************
     * TABLESPACE name 검증 
     *************************************************/
    
    /**
     * Tablespace 존재 여부
     */

    STL_TRY( ellGetTablespaceDictHandle( aTransID,
                                         aSQLStmt->mViewSCN,
                                         QLP_GET_PTR_VALUE( sParseTree->mName ),
                                         & sInitPlan->mSpaceHandle,
                                         & sObjectExist,
                                         ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_SPACE_NOT_EXIST );

    /*************************************************
     * TABLESPACE 검증 
     *************************************************/

    /**
     * Tablespce Online 검증
     */
    
    sSpaceID = ellGetTablespaceID( &sInitPlan->mSpaceHandle );

    sStartupPhase = knlGetCurrStartupPhase();

    /**
     * OPEN phase에서는 offline tablespace만 RECOVER가 가능하다.
     */
    if( sStartupPhase >= KNL_STARTUP_PHASE_OPEN )
    {
        STL_TRY_THROW( smlIsOnlineTablespace( sSpaceID ) == STL_FALSE,
                       RAMP_ERR_CANNOT_RECOVER_TABLESPACE );
    }
    else
    {
        /**
         * ALTER DATABASE ON DATABASE 권한 검사
         */

        STL_TRY( qlaCheckDatabasePriv( aTransID,
                                       aDBCStmt,
                                       aSQLStmt,
                                       ELL_DB_PRIV_ACTION_ALTER_DATABASE,
                                       aEnv )
                 == STL_SUCCESS );
    }

    STL_TRY( smlValidateMediaRecovery( sSpaceID,
                                       SML_RECOVERY_OBJECT_TYPE_TABLESPACE,
                                       SML_ENV(aEnv) ) == STL_SUCCESS );
    
    /********************************************************
     * Validation Object 추가 
     ********************************************************/

    STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                  & sInitPlan->mSpaceHandle,
                                  QLL_INIT_PLAN(aDBCStmt),
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /***********************************************************
     * Init Plan 연결 
     ***********************************************************/
    
    /**
     * Init Plan 연결 
     */

    aSQLStmt->mInitPlan = (void *) sInitPlan;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SPACE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLESPACE_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      QLP_GET_PTR_VALUE( sParseTree->mName ) );
    }

    STL_CATCH( RAMP_ERR_CANNOT_RECOVER_TABLESPACE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CANNOT_RECOVER_TABLESPACE,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief ALTER DATABASE RECOVER TABLESPACE 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldOptCodeAlterDatabaseRecoverTablespace( smlTransId      aTransID,
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
 * @brief ALTER DATABASE RECOVER TABLESPACE 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldOptDataAlterDatabaseRecoverTablespace( smlTransId      aTransID,
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
 * @brief ALTER DATABASE RECOVER TABLESPACE 를 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qldExecuteAlterDatabaseRecoverTablespace( smlTransId      aTransID,
                                                    smlStatement  * aStmt,
                                                    qllDBCStmt    * aDBCStmt,
                                                    qllStatement  * aSQLStmt,
                                                    qllEnv        * aEnv )
{
    qldInitAlterDatabaseRecoverTablespace * sInitPlan = NULL;
    smlMediaRecoveryInfo                    sMediaRecoveryInfo;
    stlInt64                                sSpaceID;
    stlTime                                 sTime = 0;
    knlStartupPhase                         sStartupPhase = KNL_STARTUP_PHASE_NONE;
    stlBool                                 sLocked = STL_TRUE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_DATABASE_RECOVER_TABLESPACE_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mInitPlan != NULL, QLL_ERROR_STACK(aEnv) );

    /***************************************************************
     * DDL 준비 
     ***************************************************************/

    sStartupPhase = knlGetCurrStartupPhase();
    
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

    sInitPlan = (qldInitAlterDatabaseRecoverTablespace *) aSQLStmt->mInitPlan;
    
    /**
     * DDL Lock 획득 
     */

    if( sStartupPhase >= KNL_STARTUP_PHASE_OPEN )
    {
        STL_TRY( qltLock4AlterSpaceOnOffline( aTransID,
                                              aStmt,
                                              aDBCStmt,
                                              & sInitPlan->mSpaceHandle,
                                              & sLocked,
                                              aEnv )
                 == STL_SUCCESS );

        STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
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

    sInitPlan = (qldInitAlterDatabaseRecoverTablespace *) aSQLStmt->mInitPlan;

    /**
     * OPEN phase에서는 offline tablespace만 RECOVER가 가능하다.
     */
    
    sSpaceID = ellGetTablespaceID( & sInitPlan->mSpaceHandle );

    if( sStartupPhase >= KNL_STARTUP_PHASE_OPEN )
    {
        STL_TRY_THROW( smlIsOnlineTablespace( sSpaceID ) == STL_FALSE,
                       RAMP_ERR_NOT_OFFLINE_TABLESPACE );
    }

    /***************************************************************
     * execute RECOVER TABLESPACE
     ***************************************************************/
    
    if( sStartupPhase == KNL_STARTUP_PHASE_OPEN )
    {
        STL_TRY( smlMediaRecoverTablespace( sSpaceID,
                                            SML_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        /**
         * Media recovery를 수행한다.
         */
        sMediaRecoveryInfo.mRecoveryType = SML_RECOVERY_TYPE_COMPLETE_MEDIA_RECOVERY;
        sMediaRecoveryInfo.mObjectType   = SML_RECOVERY_OBJECT_TYPE_TABLESPACE;
        sMediaRecoveryInfo.mTbsId        = sSpaceID;
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
    }
                                   
    /***************************************************************
     * ALTER 수행시간을 기록 
     ***************************************************************/

    if( sStartupPhase >= KNL_STARTUP_PHASE_OPEN )
    {
        STL_TRY( ellSetTimeAlterTablespace( aTransID,
                                            aStmt,
                                            sSpaceID,
                                            ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_OFFLINE_TABLESPACE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_OFFLINE_TABLESPACE,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qldAlterDatabaseRecoverTablespace */
