/*******************************************************************************
 * qlssAlterSystemKillSession.c
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
 * @file qlssAlterSystemKillSession.c
 * @brief ALTER SYSTEM KILL SESSION 처리 루틴 
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @defgroup qlssSystemAlterSystemKillSession ALTER SYSTEM KILL SESSION
 * @ingroup qlssSystem
 * @{
 */


/**
 * @brief ALTER SYSTEM KILL SESSION 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssValidateSystemKillSession( smlTransId      aTransID,
                                         qllDBCStmt    * aDBCStmt,
                                         qllStatement  * aSQLStmt,
                                         stlChar       * aSQLString,
                                         qllNode       * aParseTree,
                                         qllEnv        * aEnv )
{
    qlssInitSystemKillSession * sInitPlan  = NULL;
    qlpAlterSystemKillSession * sParseTree    = NULL;
    stlInt64                    sSessionID;
    stlInt64                    sSerialNum;
    stlChar                   * sSessionIDPtr = NULL;
    stlChar                   * sSerialNumPtr = NULL;
    stlChar                   * sEndPtr;
    stlBool                     sIsUserSession = STL_FALSE;

    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_SYSTEM_KILL_SESSION_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_SYSTEM_KILL_SESSION_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sParseTree = (qlpAlterSystemKillSession *) aParseTree;
    
    STL_PARAM_VALIDATE( sParseTree->mSessionID != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlssInitSystemKillSession),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlssInitSystemKillSession) );

    /**
     * Init Plan 구성
     */

    sSessionIDPtr = QLP_GET_PTR_VALUE( sParseTree->mSessionID );
    sSerialNumPtr = QLP_GET_PTR_VALUE( sParseTree->mSerialNum );

    STL_TRY( stlStrToInt64( sSessionIDPtr,
                            STL_NTS,
                            &sEndPtr,
                            10,
                            &sSessionID,
                            KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    STL_TRY( stlStrToInt64( sSerialNumPtr,
                            STL_NTS,
                            &sEndPtr,
                            10,
                            &sSerialNum,
                            KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    sInitPlan->mSessionID = (stlUInt32)sSessionID;
    sInitPlan->mSerialNum = sSerialNum;

    /**
     * System 내부적으로 사용하는 session은 disconnect될수 없다.
     */
    
    STL_TRY( knlIsUserSession( sInitPlan->mSessionID,
                               &sIsUserSession,
                               KNL_ENV(aEnv) )
             == STL_SUCCESS);

    STL_TRY_THROW( sIsUserSession == STL_TRUE, RAMP_ERR_SYSTEM_SESSION );

    /**
     * ALTER SYSTEM ON DATABASE 권한 검사
     */

    STL_TRY( qlaCheckDatabasePriv( aTransID,
                                   aDBCStmt,
                                   aSQLStmt,
                                   ELL_DB_PRIV_ACTION_ALTER_SYSTEM,
                                   aEnv )
             == STL_SUCCESS );
    
    /**
     * Init Plan 연결 
     */
        
    aSQLStmt->mInitPlan = (void *) sInitPlan;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SYSTEM_SESSION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SYSTEM_SESSION_CANNOT_BE_KILLED,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }
                   
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief ALTER SYSTEM KILL SESSION 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssOptCodeSystemKillSession( smlTransId      aTransID,
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
 * @brief ALTER SYSTEM KILL SESSION 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssOptDataSystemKillSession( smlTransId      aTransID,
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
 * @brief ALTER SYSTEM KILL SESSION 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qlssExecuteSystemKillSession( smlTransId      aTransID,
                                        smlStatement  * aStmt,
                                        qllDBCStmt    * aDBCStmt,
                                        qllStatement  * aSQLStmt,
                                        qllEnv        * aEnv )
{
    qlssInitSystemKillSession * sInitPlan = NULL;

    /**
     * Init Plan 획득 
     */

    sInitPlan = (qlssInitSystemKillSession *)aSQLStmt->mInitPlan;

    STL_TRY( knlCleanupKilledSession( sInitPlan->mSessionID,
                                      sInitPlan->mSerialNum,
                                      KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} qlssSystemAlterSystemKillSession */


