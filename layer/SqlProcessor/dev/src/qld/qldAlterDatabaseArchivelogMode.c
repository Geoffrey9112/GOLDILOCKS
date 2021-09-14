/*******************************************************************************
 * qldAlterDatabaseArchivelogMode.c
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
 * @file qldAlterDatabaseArchivelogMode.c
 * @brief ALTER DATABASE ARCHIVELOG | NOARCHIVELOG 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qldAlterDatabaseArchivelogMode ALTER DATABASE ARCHIVELOG | NOARCHIVELOG
 * @ingroup qldAlterDatabase
 * @{
 */



/**
 * @brief ALTER DATABASE ARCHIVELOG | NOARCHIVELOG 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldValidateAlterDatabaseArchivelogMode( smlTransId      aTransID,
                                                  qllDBCStmt    * aDBCStmt,
                                                  qllStatement  * aSQLStmt,
                                                  stlChar       * aSQLString,
                                                  qllNode       * aParseTree,
                                                  qllEnv        * aEnv )
{
    qlpAlterDatabaseArchivelogMode * sParseTree;
    stlInt32                       * sInitPlan = NULL;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_ALTER_DATABASE_ARCHIVELOG_MODE_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_ALTER_DATABASE_NOARCHIVELOG_MODE_TYPE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aParseTree->mType == QLL_STMT_ALTER_DATABASE_ARCHIVELOG_MODE_TYPE) ||
                        (aParseTree->mType == QLL_STMT_ALTER_DATABASE_NOARCHIVELOG_MODE_TYPE),
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */

    sParseTree = (qlpAlterDatabaseArchivelogMode *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(stlInt32),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    *sInitPlan = sParseTree->mMode;

    /**
     * Validation
     */

    STL_TRY( smlValidateAlterArchivelog( *sInitPlan,
                                         SML_ENV( aEnv ) )
             == STL_SUCCESS );

    /**
     * Init Plan 연결
     */

    aSQLStmt->mInitPlan = (void *) sInitPlan;

    /**
     * ALTER DATABASE ON DATABASE 권한 검사
     */

    STL_TRY( qlaCheckDatabasePriv( aTransID,
                                   aDBCStmt,
                                   aSQLStmt,
                                   ELL_DB_PRIV_ACTION_ALTER_DATABASE,
                                   aEnv )
             == STL_SUCCESS );
    
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief ALTER DATABASE ARCHIVELOG | NOARCHIVELOG 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldOptCodeAlterDatabaseArchivelogMode( smlTransId      aTransID,
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
 * @brief ALTER DATABASE ARCHIVELOG | NOARCHIVELOG 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldOptDataAlterDatabaseArchivelogMode( smlTransId      aTransID,
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
 * @brief ALTER DATABASE ARCHIVELOG | NOARCHIVELOG 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qldExecuteAlterDatabaseArchivelogMode( smlTransId      aTransID,
                                                 smlStatement  * aStmt,
                                                 qllDBCStmt    * aDBCStmt,
                                                 qllStatement  * aSQLStmt,
                                                 qllEnv        * aEnv )
{
    stlInt32 * sInitPlan = NULL;
    stlInt32   sMode;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_ALTER_DATABASE_ARCHIVELOG_MODE_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_ALTER_DATABASE_NOARCHIVELOG_MODE_TYPE),
                        QLL_ERROR_STACK(aEnv) );

    /**
     * Init Plan 획득
     */

    sInitPlan = (stlInt32 *) aSQLStmt->mInitPlan;

    sMode = *sInitPlan;

    STL_TRY( knlAddEnvEvent( SML_EVENT_ALTER_ARCHIVELOG,
                             NULL,                   /* aEventMem */
                             &sMode,                 /* aData */
                             STL_SIZEOF(stlInt32),   /* aDataSize */
                             0,                      /* startup env id */
                             KNL_EVENT_WAIT_POLLING,
                             STL_FALSE, /* aForceSuccess */
                             KNL_ENV( aEnv ) ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/** @} qldAlterDatabaseArchivelogMode */
