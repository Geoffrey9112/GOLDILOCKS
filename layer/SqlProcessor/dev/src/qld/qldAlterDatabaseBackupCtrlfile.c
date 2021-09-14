/*******************************************************************************
 * qldAlterDatabaseBackupCtrlfile.c
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
 * @file qldAlterDatabaseBackupCtrlfile.c
 * @brief ALTER DATABASE BACKUP CONTROLFILE 처리 루틴 
 */

#include <qll.h>
#include <qlDef.h>

/**
 * @defgroup qldAlterDatabaseBackupCtrlfile ALTER DATABASE BACKUP CONTROLFILE
 * @ingroup qldAlterDatabase
 * @{
 */


/**
 * @brief ALTER DATABASE BACKUP CONTROLFILE 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldValidateAlterDatabaseBackupCtrlfile( smlTransId      aTransID,
                                                  qllDBCStmt    * aDBCStmt,
                                                  qllStatement  * aSQLStmt,
                                                  stlChar       * aSQLString,
                                                  qllNode       * aParseTree,
                                                  qllEnv        * aEnv )
{
    qlpBackupCtrlfile     * sParseTree;
    qldInitBackupCtrlfile * sInitPlan = NULL;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_DATABASE_BACKUP_CTRLFILE_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_DATABASE_BACKUP_CTRLFILE_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sParseTree = (qlpBackupCtrlfile *) aParseTree;

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
                                STL_SIZEOF(qldInitBackupCtrlfile),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sInitPlan->mTarget, 0x00, STL_SIZEOF(qldInitBackupCtrlfile) );
    stlStrcpy( sInitPlan->mTarget, QLP_GET_PTR_VALUE(sParseTree->mTarget) );

    /**
     * Init Plan 연결 
     */

    aSQLStmt->mInitPlan = (void *) sInitPlan;
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief ALTER DATABASE BACKUP CONTROLFILE 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldOptCodeAlterDatabaseBackupCtrlfile( smlTransId      aTransID,
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
 * @brief ALTER DATABASE BACKUP CONTROLFILE 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldOptDataAlterDatabaseBackupCtrlfile( smlTransId      aTransID,
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
 * @brief ALTER DATABASE BACKUP CONTROLFILE 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qldExecuteAlterDatabaseBackupCtrlfile( smlTransId      aTransID,
                                                 smlStatement  * aStmt,
                                                 qllDBCStmt    * aDBCStmt,
                                                 qllStatement  * aSQLStmt,
                                                 qllEnv        * aEnv )
{
    qldInitBackupCtrlfile * sInitPlan = NULL;
    smlBackupMessage        sBackupMsg;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_DATABASE_BACKUP_CTRLFILE_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    STL_TRY_THROW( smlIsArchivelogMode() == STL_TRUE,
                   RAMP_ERR_CANNOT_BACKUP_NOARCHIVELOG_MODE );

    /**
     * Init Plan 획득 
     */

    sInitPlan = (qldInitBackupCtrlfile *) aSQLStmt->mInitPlan;

    stlMemset( &sBackupMsg, 0x00, STL_SIZEOF( smlBackupMessage ) );

    sBackupMsg.mBackupObject = SML_BACKUP_OBJECT_TYPE_CONTROLFILE;

    stlMemcpy( sBackupMsg.mTarget, sInitPlan->mTarget, STL_SIZEOF(sBackupMsg.mTarget) );

    STL_TRY( smlBackup( &sBackupMsg, SML_ENV( aEnv ) ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CANNOT_BACKUP_NOARCHIVELOG_MODE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CANNOT_BACKUP_NOARCHIVELOG_MODE,
                      NULL,
                      QLL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}

/** @} qldAlterDatabaseBackupCtrlfile */
