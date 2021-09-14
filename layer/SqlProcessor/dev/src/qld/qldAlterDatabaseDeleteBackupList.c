/*******************************************************************************
 * qldAlterDatabaseDeleteBackupList.c
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
 * @file qldAlterDatabaseDeleteBackupList.c
 * @brief ALTER DATABASE DELETE BACKUP LIST 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>

/**
 * @defgroup qldAlterDatabaseDeleteBackupList ALTER DATABASE DELETE BACKUP LIST
 * @ingroup qldAlterDatabase
 * @{
 */


/**
 * @brief ALTER DATABASE DELETE BACKUP LIST 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldValidateAlterDatabaseDeleteBackupList( smlTransId      aTransID,
                                                    qllDBCStmt    * aDBCStmt,
                                                    qllStatement  * aSQLStmt,
                                                    stlChar       * aSQLString,
                                                    qllNode       * aParseTree,
                                                    qllEnv        * aEnv )
{
    qlpAlterDatabaseDeleteBackupList     * sParseTree;
    qldInitAlterDatabaseDeleteBackupList * sInitPlan = NULL;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_DATABASE_DELETE_BACKUP_LIST_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_DATABASE_DELETE_BACKUP_LIST_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sParseTree = (qlpAlterDatabaseDeleteBackupList *) aParseTree;

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
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qldInitAlterDatabaseDeleteBackupList),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sInitPlan->mDeleteFile = sParseTree->mDeleteFile;

    if ( stlStrcmp( QLP_GET_PTR_VALUE(sParseTree->mTarget), "OBSOLETE" ) == 0 )
    {
        sInitPlan->mTarget = SML_DELETE_OBSOLETE_BACKUP_LIST;
    }
    else
    {
        sInitPlan->mTarget = SML_DELETE_ALL_BACKUP_LIST;
    }

    /**
     * Init Plan 연결 
     */

    aSQLStmt->mInitPlan = (void *) sInitPlan;
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief ALTER DATABASE DELETE BACKUP LIST 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldOptCodeAlterDatabaseDeleteBackupList( smlTransId      aTransID,
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
 * @brief ALTER DATABASE DELETE BACKUP LIST 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldOptDataAlterDatabaseDeleteBackupList( smlTransId      aTransID,
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
 * @brief ALTER DATABASE DELETE BACKUP LIST 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qldExecuteAlterDatabaseDeleteBackupList( smlTransId      aTransID,
                                                   smlStatement  * aStmt,
                                                   qllDBCStmt    * aDBCStmt,
                                                   qllStatement  * aSQLStmt,
                                                   qllEnv        * aEnv )
{
    qldInitAlterDatabaseDeleteBackupList * sInitPlan = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_DATABASE_DELETE_BACKUP_LIST_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * Init Plan 획득 
     */

    sInitPlan = (qldInitAlterDatabaseDeleteBackupList *) aSQLStmt->mInitPlan;

    STL_TRY( smlDeleteBackupList( sInitPlan->mTarget,
                                  sInitPlan->mDeleteFile,
                                  SML_ENV( aEnv ) ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/** @} qldAlterDatabaseDeleteBackupList */
