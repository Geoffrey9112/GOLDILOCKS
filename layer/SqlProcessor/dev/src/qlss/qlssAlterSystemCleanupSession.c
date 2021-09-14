/*******************************************************************************
 * qlssAlterSystemCleanupSession.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlssAlterSystemCleanupSession.c 9975 2013-10-21 06:28:35Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlssAlterSystemCleanupSession.c
 * @brief ALTER SYSTEM CLEANUP SESSION 처리 루틴 
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @defgroup qlssSystemAlterSystemCleanupSession ALTER SYSTEM CLEANUP SESSION
 * @ingroup qlssSystem
 * @{
 */

/**
 * @brief ALTER SYSTEM CLEANUP SESSION 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssValidateSystemCleanupSession( smlTransId      aTransID,
                                            qllDBCStmt    * aDBCStmt,
                                            qllStatement  * aSQLStmt,
                                            stlChar       * aSQLString,
                                            qllNode       * aParseTree,
                                            qllEnv        * aEnv )
{
    qlssInitSystemCleanupSession * sInitPlan   = NULL;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_SYSTEM_CLEANUP_SESSION_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_SYSTEM_CLEANUP_SESSION_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     * nothing to do
     */
    
    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlssInitSystemCleanupSession),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlssInitSystemCleanupSession) );
    
    /**
     * Property 유효성 검사
     */

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

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief ALTER SYSTEM CLEANUP SESSION 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssOptCodeSystemCleanupSession( smlTransId      aTransID,
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
 * @brief ALTER SYSTEM CLEANUP SESSION 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssOptDataSystemCleanupSession( smlTransId      aTransID,
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
 * @brief ALTER SYSTEM CLEANUP SESSION 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qlssExecuteSystemCleanupSession( smlTransId      aTransID,
                                           smlStatement  * aStmt,
                                           qllDBCStmt    * aDBCStmt,
                                           qllStatement  * aSQLStmt,
                                           qllEnv        * aEnv )
{
    STL_TRY( knlAddEnvEvent( KNL_EVENT_CLEANUP,
                             NULL,     /* aEventMem */
                             NULL,     /* aData */
                             0,        /* aDataSize */
                             SML_CLEANUP_ENV_ID,
                             KNL_EVENT_WAIT_POLLING,
                             STL_FALSE, /* aForceSuccess */
                             KNL_ENV( aEnv ) ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} qlssSystemAlterSystemCleanupSession */


