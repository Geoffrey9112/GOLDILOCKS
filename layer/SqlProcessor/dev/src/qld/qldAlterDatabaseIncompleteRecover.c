/*******************************************************************************
 * qldAlterDatabaseIncompleteRecover.c
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
 * @file qldAlterDatabaseIncompleteRecover.c
 * @brief ALTER DATABASE INCOMPLETE RECOVER 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @addtogroup qldAlterDatabaseIncompleteRecover
 * @{
 */


/**
 * @brief ALTER DATABASE INCOMPLETE RECOVER 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldValidateAlterDatabaseIncompleteRecover( smlTransId      aTransID,
                                                     qllDBCStmt    * aDBCStmt,
                                                     qllStatement  * aSQLStmt,
                                                     stlChar       * aSQLString,
                                                     qllNode       * aParseTree,
                                                     qllEnv        * aEnv )
{
    qldInitAlterDatabaseIncompleteRecover * sInitPlan = NULL;
    qlpImrOption                          * sImrOption;
    qlpAlterDatabaseRecover               * sParseTree = NULL;
    knlStartupPhase                         sCurrStartupPhase;
    qlpInteractiveImrCondition            * sImrCondition;
    stlBool                                 sFileExist;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_DATABASE_INCOMPLETE_RECOVERY_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_DATABASE_INCOMPLETE_RECOVERY_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sParseTree = (qlpAlterDatabaseRecover *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qldInitAlterDatabaseIncompleteRecover),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qldInitAlterDatabaseIncompleteRecover) );

    /**
     * 불완전 복구 option validation
     */
    sImrOption = (qlpImrOption *)sParseTree->mRecoveryOption;

    /* 불완전 복구 option( INTERACTIVE, BATCH ) */
    sInitPlan->mRecoveryType = SML_RECOVERY_TYPE_INCOMPLETE_MEDIA_RECOVERY;
    sInitPlan->mImrOption    = sImrOption->mImrOption;
    sInitPlan->mImrCondition = sImrOption->mImrCondition;
    sInitPlan->mUntilValue   = -1;

    switch( sInitPlan->mImrOption )
    {
        case SML_IMR_OPTION_BATCH:
            switch( sInitPlan->mImrCondition )
            {
                case SML_IMR_CONDITION_TIME:
                    /* TIME 기반 불완전 복구 */
                    STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                    /* STL_TRY( dtlGetIntegerFromString( */
                    /*              QLP_GET_PTR_VALUE(sImrOption->mImrOption), */
                    /*              stlStrlen( QLP_GET_PTR_VALUE( */
                    /*                             sImrOption->mImrOption) ), */
                    /*              & sInitPlan->mUntilValue, */
                    /*              QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS ); */
                    break;

                case SML_IMR_CONDITION_CHANGE:
                    /* CHANGE 기반 불완전 복구
                     * : CHANGE 기반 불완전 복구를 위한 UNTIL VALUE 획득 */
                    STL_TRY( dtlGetIntegerFromString(
                                 QLP_GET_PTR_VALUE(sImrOption->mConditionValue),
                                 stlStrlen( QLP_GET_PTR_VALUE(
                                                sImrOption->mConditionValue) ),
                                 & sInitPlan->mUntilValue,
                                 QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS );
                    break;
                default:
                    break;
            }
            break;
        case SML_IMR_OPTION_INTERACTIVE:
            /* Interactive Incomplete Media Recovery
             * : Interactive 불완전 복구를 위한 Condition
             *   (BEGIN, SUGGESTION, FILENAME, AUTO, END) */
            sImrCondition = (qlpInteractiveImrCondition *)sImrOption->mConditionValue;
            sInitPlan->mImrCondition = sImrCondition->mImrCondition;

            if( (sInitPlan->mImrOption == SML_IMR_OPTION_INTERACTIVE) &&
                (sInitPlan->mImrCondition == SML_IMR_CONDITION_MANUAL) )
            {
                sInitPlan->mLogfileName = QLP_GET_PTR_VALUE(sImrCondition->mLogfileName);

                /* 특정 Logfile을 이용하여 redo를 하는 경우 Logfile이 존재하는지 체크한다. */
                STL_TRY( smlExistLogfile( sInitPlan->mLogfileName,
                                          &sFileExist,
                                          SML_ENV(aEnv) ) == STL_SUCCESS );

                STL_TRY_THROW( sFileExist == STL_TRUE, RAMP_ERR_NOT_EXIST_FILE );
            }
            break;
        case SML_IMR_OPTION_NONE:
        default:
            STL_ASSERT( 0 );
    }

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

    aSQLStmt->mInitPlan = (void *) sInitPlan;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_MOUNTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_DATABASE_NOT_MOUNTED,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_NOT_EXIST_FILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_FILE_NOT_EXIST,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      "DATABASE RECOVER UNTIL TIME" );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief ALTER DATABASE INCOMPLETE RECOVER 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldOptCodeAlterDatabaseIncompleteRecover( smlTransId      aTransID,
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
 * @brief ALTER DATABASE INCOMPLETE RECOVER 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldOptDataAlterDatabaseIncompleteRecover( smlTransId      aTransID,
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
 * @brief ALTER DATABASE INCOMPLETE RECOVER 를 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qldExecuteAlterDatabaseIncompleteRecover( smlTransId      aTransID,
                                                    smlStatement  * aStmt,
                                                    qllDBCStmt    * aDBCStmt,
                                                    qllStatement  * aSQLStmt,
                                                    qllEnv        * aEnv )
{
    qldInitAlterDatabaseIncompleteRecover * sInitPlan = NULL;
    stlInt32                                sOffset = 0;
    smlMediaRecoveryInfo                    sMediaRecoveryInfo;
    stlInt32                                sMsgLength;
    stlChar                               * sRecoverMsg;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_DATABASE_INCOMPLETE_RECOVERY_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * Init Plan 획득 
     */
    sInitPlan = (qldInitAlterDatabaseIncompleteRecover *) aSQLStmt->mInitPlan;

    /**
     * Media recovery를 수행한다.
     */
    sMediaRecoveryInfo.mRecoveryType = sInitPlan->mRecoveryType;
    sMediaRecoveryInfo.mObjectType   = SML_RECOVERY_OBJECT_TYPE_DATABASE;
    sMediaRecoveryInfo.mTbsId        = SML_INVALID_TBS_ID;
    sMediaRecoveryInfo.mImrOption    = sInitPlan->mImrOption;
    sMediaRecoveryInfo.mUntilValue   = sInitPlan->mUntilValue;
    sMediaRecoveryInfo.mImrCondition = sInitPlan->mImrCondition;

    sMsgLength = STL_SIZEOF( smlMediaRecoveryInfo );

    if( (sInitPlan->mImrOption == SML_IMR_OPTION_INTERACTIVE) &&
        (sInitPlan->mImrCondition == SML_IMR_CONDITION_MANUAL) )
    {
        sMsgLength += (stlStrlen(sInitPlan->mLogfileName) + 1);
    }

    STL_TRY( knlAllocRegionMem( & QLL_EXEC_DDL_MEM(aEnv),
                                sMsgLength,
                                (void **) & sRecoverMsg,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sRecoverMsg, 0x00, sMsgLength );

    sOffset = 0;
    STL_WRITE_MOVE_BYTES( sRecoverMsg,
                          &sMediaRecoveryInfo,
                          STL_SIZEOF(smlMediaRecoveryInfo),
                          sOffset );

    /**
     * Logfile 단위 Incomplete Media Recovery type에서 사용자가
     * Recovery를 위한 Logfile을 입력하였을 경우 Event Context에
     * 입력된 Logfile이름을 추가하여 Event를 수행한다.
     */
    if( (sInitPlan->mImrOption == SML_IMR_OPTION_INTERACTIVE) &&
        (sInitPlan->mImrCondition == SML_IMR_CONDITION_MANUAL) )
    {
        STL_WRITE_MOVE_BYTES( sRecoverMsg,
                              sInitPlan->mLogfileName,
                              stlStrlen(sInitPlan->mLogfileName),
                              sOffset );
    }

    STL_TRY( knlAddEnvEvent( SML_EVENT_MEDIA_RECOVER,
                             NULL,                               /* aEventMem */
                             sRecoverMsg,                        /* aData */
                             sOffset,                            /* aDataSize */
                             0,                                  /* aTargetEnvId */
                             KNL_EVENT_WAIT_BLOCKED,
                             STL_FALSE, /* aForceSuccess */
                             KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} qldAlterDatabaseIncompleteRecover */
