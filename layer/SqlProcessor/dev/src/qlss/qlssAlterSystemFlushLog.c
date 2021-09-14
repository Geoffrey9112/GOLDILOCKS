/*******************************************************************************
 * qlssAlterSystemFlushLog.c
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
 * @file qlssAlterSystemFlushLog.c
 * @brief ALTER SYSTEM FLUSH LOG 처리 루틴 
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @defgroup qlssSystemAlterSystemFlushLog ALTER SYSTEM FLUSH LOG
 * @ingroup qlssSystem
 * @{
 */

/**
 * @brief ALTER SYSTEM FLUSH LOG 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssValidateSystemFlushLog( smlTransId      aTransID,
                                      qllDBCStmt    * aDBCStmt,
                                      qllStatement  * aSQLStmt,
                                      stlChar       * aSQLString,
                                      qllNode       * aParseTree,
                                      qllEnv        * aEnv )
{
    qlssInitSystemFlushLog * sInitPlan  = NULL;
    qlpAlterSystemFlushLog * sParseTree = NULL;
    stlInt64                 sDatabaseTestOption = 0;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_SYSTEM_FLUSH_LOG_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_SYSTEM_FLUSH_LOG_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sParseTree = (qlpAlterSystemFlushLog *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlssInitSystemFlushLog),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlssInitSystemFlushLog) );
    sInitPlan->mBehavior = sParseTree->mBehavior;

    /**
     * Property 유효성 검사
     */

    if( (sInitPlan->mBehavior == QLP_START_FLUSH_LOGS) ||
        (sInitPlan->mBehavior == QLP_STOP_FLUSH_LOGS) )
    {
        sDatabaseTestOption = knlGetPropertyBigIntValueByID( KNL_PROPERTY_DATABASE_TEST_OPTION,
                                                             KNL_ENV(aEnv) );

        STL_TRY_THROW( sDatabaseTestOption < 1, RAMP_ERR_DISALLOW_STATEMENT );
    }
    
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
 * @brief ALTER SYSTEM FLUSH LOG 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssOptCodeSystemFlushLog( smlTransId      aTransID,
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
 * @brief ALTER SYSTEM FLUSH LOG 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssOptDataSystemFlushLog( smlTransId      aTransID,
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
 * @brief ALTER SYSTEM FLUSH LOG 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qlssExecuteSystemFlushLog( smlTransId      aTransID,
                                     smlStatement  * aStmt,
                                     qllDBCStmt    * aDBCStmt,
                                     qllStatement  * aSQLStmt,
                                     qllEnv        * aEnv )
{
    qlssInitSystemFlushLog  * sInitPlan = NULL;

    /**
     * Init Plan 획득 
     */

    sInitPlan = (qlssInitSystemFlushLog *) aSQLStmt->mInitPlan;

    /**
     * Execution
     */
    switch( sInitPlan->mBehavior )
    {
        case QLP_START_FLUSH_LOGS:
            STL_TRY( smlActivateLogFlushing( SML_ENV(aEnv) ) == STL_SUCCESS );
            break;
        case QLP_STOP_FLUSH_LOGS:
            STL_TRY( smlDeactivateLogFlushing( SML_ENV(aEnv) ) == STL_SUCCESS );
            break;
        case QLP_FLUSH_LOGS:
            STL_TRY( smlFlushAllLogs( SML_ENV(aEnv) ) == STL_SUCCESS );
            break;
        default:
            STL_ASSERT( 0 );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} qlssSystemAlterSystemFlushLog */


