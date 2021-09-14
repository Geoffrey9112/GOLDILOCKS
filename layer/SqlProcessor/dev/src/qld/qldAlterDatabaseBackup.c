/*******************************************************************************
 * qldAlterDatabaseBackup.c
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
 * @file qldAlterDatabaseBackup.c
 * @brief ALTER DATABASE ... BACKUP 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>

/**
 * @defgroup qldAlterDatabaseBackup ALTER DATABASE ... BACKUP
 * @ingroup qldAlterDatabase
 * @{
 */


/**
 * @brief ALTER DATABASE ... BACKUP 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldValidateAlterDatabaseBackup( smlTransId      aTransID,
                                          qllDBCStmt    * aDBCStmt,
                                          qllStatement  * aSQLStmt,
                                          stlChar       * aSQLString,
                                          qllNode       * aParseTree,
                                          qllEnv        * aEnv )
{
    qlpAlterDatabaseBackup     * sParseTree;
    qldInitAlterDatabaseBackup * sInitPlan = NULL;
    qlpIncrementalBackupOption * sOption;
    stlInt64                     sLevel;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_ALTER_DATABASE_BACKUP_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_ALTER_DATABASE_BACKUP_INCREMENTAL_TYPE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aParseTree->mType == QLL_STMT_ALTER_DATABASE_BACKUP_TYPE) ||
                        (aParseTree->mType == QLL_STMT_ALTER_DATABASE_BACKUP_INCREMENTAL_TYPE),
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sParseTree = (qlpAlterDatabaseBackup *) aParseTree;

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
                                STL_SIZEOF(qldInitAlterDatabaseBackup),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qldInitAlterDatabaseBackup) );

    sInitPlan->mBackupType = sParseTree->mBackupType;

    if ( sInitPlan->mBackupType == SML_BACKUP_TYPE_FULL )
    {
        STL_ASSERT( sParseTree->mCommand != NULL );

        if ( stlStrcmp( QLP_GET_PTR_VALUE(sParseTree->mCommand), "BEGIN" ) == 0 )
        {
            sInitPlan->mCommand = SML_BACKUP_COMMAND_BEGIN;
        }
        else
        {
            sInitPlan->mCommand = SML_BACKUP_COMMAND_END;
        }
    }
    else
    {
        STL_ASSERT( sParseTree->mOption != NULL );

        sOption = (qlpIncrementalBackupOption *)sParseTree->mOption;

        STL_ASSERT( sOption->mLevel != NULL );

        STL_TRY( dtlGetIntegerFromString(
                     QLP_GET_PTR_VALUE(sOption->mLevel),
                     stlStrlen( QLP_GET_PTR_VALUE(sOption->mLevel) ),
                     & sLevel,
                     QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS );

        STL_TRY_THROW( (sLevel >= 0 ) && (sLevel < SML_INCREMENTAL_BACKUP_LEVEL_MAX),
                       RAMP_ERR_NOT_SUPPORT_INC_BACKUP_LEVEL );

        sInitPlan->mLevel = (stlInt16)sLevel;

        if ( sOption->mOption == NULL )
        {
            /**
             * Incremental Backup의 default option은 DIFFERENTIAL 이다.
             */
            sInitPlan->mOption = SML_INCREMENTAL_BACKUP_OPTION_DIFFERENTIAL;
        }
        else
        {
            /**
             * Level 0 Backup은 CUMULATIVE, DIFFERENTIAL option을 쓸 수 없다.
             */
            STL_TRY_THROW( sLevel != 0, RAMP_ERR_NOT_SUPPORT_INC_BACKUP_OPTION );

            if ( stlStrcmp( QLP_GET_PTR_VALUE(sOption->mOption), "DIFFERENTIAL" ) == 0 )
            {
                sInitPlan->mOption = SML_INCREMENTAL_BACKUP_OPTION_DIFFERENTIAL;
            }
            else
            {
                sInitPlan->mOption = SML_INCREMENTAL_BACKUP_OPTION_CUMULATIVE;
            }
        }
    }

    /**
     * Init Plan 연결 
     */

    aSQLStmt->mInitPlan = (void *) sInitPlan;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_SUPPORT_INC_BACKUP_LEVEL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CANNOT_SUPPORT_INCREMENTAL_BACKUP_LEVEL,
                      NULL,
                      QLL_ERROR_STACK( aEnv ),
                      sLevel );
    }

    STL_CATCH( RAMP_ERR_NOT_SUPPORT_INC_BACKUP_OPTION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CANNOT_SUPPORT_INCREMENTAL_BACKUP_OPTION,
                      NULL,
                      QLL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief ALTER DATABASE ... BACKUP 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldOptCodeAlterDatabaseBackup( smlTransId      aTransID,
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
 * @brief ALTER DATABASE ... BACKUP 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldOptDataAlterDatabaseBackup( smlTransId      aTransID,
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
 * @brief ALTER DATABASE ... BACKUP 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qldExecuteAlterDatabaseBackup( smlTransId      aTransID,
                                         smlStatement  * aStmt,
                                         qllDBCStmt    * aDBCStmt,
                                         qllStatement  * aSQLStmt,
                                         qllEnv        * aEnv )
{
    qldInitAlterDatabaseBackup * sInitPlan = NULL;
    smlBackupMessage             sBackupMsg;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_ALTER_DATABASE_BACKUP_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_ALTER_DATABASE_BACKUP_INCREMENTAL_TYPE),
                        QLL_ERROR_STACK(aEnv) );

    STL_TRY_THROW( smlIsArchivelogMode() == STL_TRUE,
                   RAMP_ERR_CANNOT_BACKUP_NOARCHIVELOG_MODE );

    /**
     * Init Plan 획득 
     */

    sInitPlan = (qldInitAlterDatabaseBackup *) aSQLStmt->mInitPlan;

    stlMemset( &sBackupMsg, 0x00, STL_SIZEOF( smlBackupMessage ) );

    sBackupMsg.mBackupObject            = SML_BACKUP_OBJECT_TYPE_DATABASE;
    sBackupMsg.mBackupType              = sInitPlan->mBackupType;
    sBackupMsg.mCommand                 = sInitPlan->mCommand;
    sBackupMsg.mIncrementalBackupOption = sInitPlan->mOption;
    sBackupMsg.mIncrementalBackupLevel  = sInitPlan->mLevel;
    sBackupMsg.mTbsId                   = SML_INVALID_TBS_ID;

    stlMemset( sBackupMsg.mTarget, 0x00, STL_SIZEOF(sBackupMsg.mTarget) );

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

/** @} qldAlterDatabaseBackup */
