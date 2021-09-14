/*******************************************************************************
 * qldAlterDatabaseClearPasswordHistory.c
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
 * @file qldAlterDatabaseClearPasswordHistory.c
 * @brief ALTER DATABASE CLEAR PASSWORD HISTORY 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>

/**
 * @defgroup qldAlterDatabaseClearPasswordHistory ALTER DATABASE CLEAR PASSWORD HISTORY
 * @ingroup qldAlterDatabase
 * @{
 */


/**
 * @brief ALTER DATABASE CLEAR PASSWORD HISTORY 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldValidateAlterDatabaseClearPasswordHistory( smlTransId      aTransID,
                                                        qllDBCStmt    * aDBCStmt,
                                                        qllStatement  * aSQLStmt,
                                                        stlChar       * aSQLString,
                                                        qllNode       * aParseTree,
                                                        qllEnv        * aEnv )
{
//    qlpAlterDatabaseClearPassHistory     * sParseTree;
    qldInitAlterDatabaseClearPassHistory * sInitPlan = NULL;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_DATABASE_CLEAR_PASSWORD_HISTORY_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_DATABASE_CLEAR_PASSWORD_HISTORY_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

//    sParseTree = (qlpAlterDatabaseClearPassHistory *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qldInitAlterDatabaseClearPassHistory),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qldInitAlterDatabaseClearPassHistory) );

    STL_TRY( qlvSetInitPlan( aDBCStmt,
                             aSQLStmt,
                             & sInitPlan->mCommonInit,
                             aEnv )
             == STL_SUCCESS );
    
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

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief ALTER DATABASE CLEAR PASSWORD HISTORY 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldOptCodeAlterDatabaseClearPasswordHistory( smlTransId      aTransID,
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
 * @brief ALTER DATABASE CLEAR PASSWORD HISTORY 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldOptDataAlterDatabaseClearPasswordHistory( smlTransId      aTransID,
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
 * @brief ALTER DATABASE CLEAR PASSWORD HISTORY 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qldExecuteAlterDatabaseClearPasswordHistory( smlTransId      aTransID,
                                                       smlStatement  * aStmt,
                                                       qllDBCStmt    * aDBCStmt,
                                                       qllStatement  * aSQLStmt,
                                                       qllEnv        * aEnv )
{
//    qldInitAlterDatabaseClearPassHistory * sInitPlan = NULL;

    stlTime               sTime = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_DATABASE_CLEAR_PASSWORD_HISTORY_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /***************************************************************
     * 기본 정보 설정 
     ***************************************************************/
    
    /**
     * DDL 수행 시간 설정
     */

    sTime = stlNow();
    STL_TRY( ellBeginStmtDDL( aStmt, sTime, ELL_ENV(aEnv) ) == STL_SUCCESS );

    /***************************************************************
     *  Run-Time Validation
     ***************************************************************/

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

//    sInitPlan = (qldInitAlterDatabaseClearPassHistory *) aSQLStmt->mInitPlan;
    
    /***************************************************************
     * History 제거 
     ***************************************************************/

    STL_TRY( ellClearUserPasswordHistory( aTransID,
                                          aStmt,
                                          ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/** @} qldAlterDatabaseClearPasswordHistory */
