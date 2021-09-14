/*******************************************************************************
 * qlssAlterSystemDisconnectSession.c
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
 * @file qlssAlterSystemDisconnectSession.c
 * @brief ALTER SYSTEM DISCONNECT SESSION 처리 루틴 
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @defgroup qlssSystemAlterSystemDisconnectSession ALTER SYSTEM DISCONNECT SESSION
 * @ingroup qlssSystem
 * @{
 */



/**
 * @brief ALTER SYSTEM DISCONNECT SESSION 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssValidateSystemDisconnectSession( smlTransId      aTransID,
                                               qllDBCStmt    * aDBCStmt,
                                               qllStatement  * aSQLStmt,
                                               stlChar       * aSQLString,
                                               qllNode       * aParseTree,
                                               qllEnv        * aEnv )
{
    qlssInitSystemDisconnectSession  * sInitPlan  = NULL;
    qlpAlterSystemDisconnSession     * sParseTree    = NULL;
    stlInt64                           sSessionID;
    stlInt64                           sSerialNum;
    stlChar                          * sSessionIDPtr = NULL;
    stlChar                          * sSerialNumPtr = NULL;
    stlChar                          * sEndPtr;
    stlBool                            sIsUserSession = STL_FALSE;

    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_SYSTEM_DISCONNECT_SESSION_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_SYSTEM_DISCONNECT_SESSION_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sParseTree = (qlpAlterSystemDisconnSession *) aParseTree;
    
    STL_PARAM_VALIDATE( sParseTree->mSessionID != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlssInitSystemDisconnectSession),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlssInitSystemDisconnectSession) );

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

    sInitPlan->mSessionID         = (stlUInt32)sSessionID;
    sInitPlan->mSerialNum         = sSerialNum;
    sInitPlan->mIsPostTransaction = sParseTree->mIsPostTransaction;

    switch( sParseTree->mOption->mType )
    {
        case QLL_BNF_STRING_CONSTANT_TYPE :
        case QLL_BNF_NULL_CONSTANT_TYPE :
            sInitPlan->mOption = QLSS_END_SESSION_OPTION_IMMEDIATE;
        default :
            break;
    }

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
 * @brief ALTER SYSTEM DISCONNECT SESSION 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssOptCodeSystemDisconnectSession( smlTransId      aTransID,
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
 * @brief ALTER SYSTEM DISCONNECT SESSION 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssOptDataSystemDisconnectSession( smlTransId      aTransID,
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
 * @brief ALTER SYSTEM DISCONNECT SESSION 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qlssExecuteSystemDisconnectSession( smlTransId      aTransID,
                                              smlStatement  * aStmt,
                                              qllDBCStmt    * aDBCStmt,
                                              qllStatement  * aSQLStmt,
                                              qllEnv        * aEnv )
{
    qlssInitSystemDisconnectSession * sInitPlan = NULL;

    /**
     * Init Plan 획득 
     */

    sInitPlan = (qlssInitSystemDisconnectSession *) aSQLStmt->mInitPlan;

    if( sInitPlan->mIsPostTransaction == STL_TRUE )
    {
        STL_TRY( knlEndSessionByID( sInitPlan->mSessionID,
                                    sInitPlan->mSerialNum,
                                    KNL_END_SESSION_OPTION_POST_TRANSACTION,
                                    KNL_ENV( aEnv ) ) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( knlEndSessionByID( sInitPlan->mSessionID,
                                    sInitPlan->mSerialNum,
                                    KNL_END_SESSION_OPTION_IMMEDIATE,
                                    KNL_ENV( aEnv ) ) == STL_SUCCESS );
        
        STL_TRY( knlKillSessionByID( sInitPlan->mSessionID,
                                     sInitPlan->mSerialNum,
                                     KNL_ENV(aEnv) ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlssSystemAlterSystemDisconnectSession */


