/*******************************************************************************
 * qltAlterSpaceBackup.c
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
 * @file qltAlterSpaceBackup.c
 * @brief ALTER TABLESPACE .. BACKUP 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qltAlterSpaceBackup ALTER TABLESPACE .. BACKUP
 * @ingroup qltAlterSpace
 * @{
 */


/**
 * @brief ALTER TABLESPACE .. BACKUP 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qltValidateAlterSpaceBackup( smlTransId      aTransID,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       stlChar       * aSQLString,
                                       qllNode       * aParseTree,
                                       qllEnv        * aEnv )
{
    qltInitAlterSpaceBackup    * sInitPlan = NULL;
    qlpAlterTablespaceBackup   * sParseTree = NULL;
    qlpIncrementalBackupOption * sOption;
    stlInt64                     sLevel;
    stlBool                      sObjectExist = STL_FALSE;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLESPACE_BACKUP_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLESPACE_BACKUP_INCREMENTAL_TYPE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aParseTree->mType == QLL_STMT_ALTER_TABLESPACE_BACKUP_TYPE) ||
                        (aParseTree->mType == QLL_STMT_ALTER_TABLESPACE_BACKUP_INCREMENTAL_TYPE),
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */

    sParseTree = (qlpAlterTablespaceBackup *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qltInitAlterSpaceBackup),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qltInitAlterSpaceBackup) );

    STL_TRY( qlvSetInitPlan( aDBCStmt,
                             aSQLStmt,
                             & sInitPlan->mCommonInit,
                             aEnv )
             == STL_SUCCESS );
    
    /*************************************************
     * TABLESPACE name 검증 
     *************************************************/
    
    /**
     * Tablespace 존재 여부
     */

    STL_TRY( ellGetTablespaceDictHandle( aTransID,
                                         aSQLStmt->mViewSCN,
                                         QLP_GET_PTR_VALUE( sParseTree->mName ),
                                         & sInitPlan->mSpaceHandle,
                                         & sObjectExist,
                                         ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_SPACE_NOT_EXIST );

    /**
     * ALTER TABLESPACE ON DATABASE 권한 검사
     */

    STL_TRY( qlaCheckDatabasePriv( aTransID,
                                   aDBCStmt,
                                   aSQLStmt,
                                   ELL_DB_PRIV_ACTION_ALTER_TABLESPACE,
                                   aEnv )
             == STL_SUCCESS );

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

    /********************************************************
     * Validation Object 추가 
     ********************************************************/

    STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                  & sInitPlan->mSpaceHandle,
                                  QLL_INIT_PLAN(aDBCStmt),
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /*************************************************
     * Plan 연결 
     *************************************************/
    
    aSQLStmt->mInitPlan = (void *) sInitPlan;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SPACE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLESPACE_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      QLP_GET_PTR_VALUE( sParseTree->mName ) );
    }

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
 * @brief ALTER TABLESPACE .. BACKUP 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qltOptCodeAlterSpaceBackup( smlTransId      aTransID,
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
 * @brief ALTER TABLESPACE .. BACKUP 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qltOptDataAlterSpaceBackup( smlTransId      aTransID,
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
 * @brief ALTER TABLESPACE .. BACKUP 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qltExecuteAlterSpaceBackup( smlTransId      aTransID,
                                      smlStatement  * aStmt,
                                      qllDBCStmt    * aDBCStmt,
                                      qllStatement  * aSQLStmt,
                                      qllEnv        * aEnv )
{
    qltInitAlterSpaceBackup * sInitPlan = NULL;
    smlBackupMessage          sBackupMsg;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLESPACE_BACKUP_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLESPACE_BACKUP_INCREMENTAL_TYPE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mInitPlan != NULL, QLL_ERROR_STACK(aEnv) );

    STL_TRY_THROW( smlIsArchivelogMode() == STL_TRUE,
                   RAMP_ERR_CANNOT_BACKUP_NOARCHIVELOG_MODE );

    /***************************************************************
     *  Run-Time Validation
     ***************************************************************/

    /**
     * Valid Plan 획득
     */

    STL_TRY( qlgGetValidPlan4DDL( aTransID,
                                  aDBCStmt,
                                  aSQLStmt,
                                  QLL_PHASE_EXECUTE,
                                  aEnv )
             == STL_SUCCESS );

    sInitPlan = (qltInitAlterSpaceBackup *) aSQLStmt->mInitPlan;

    /***************************************************************
     * BACKUP
     ***************************************************************/

    stlMemset( &sBackupMsg, 0x00, STL_SIZEOF( smlBackupMessage ) );

    sBackupMsg.mBackupObject            = SML_BACKUP_OBJECT_TYPE_TABLESPACE;
    sBackupMsg.mBackupType              = sInitPlan->mBackupType;
    sBackupMsg.mCommand                 = sInitPlan->mCommand;
    sBackupMsg.mIncrementalBackupOption = sInitPlan->mOption;
    sBackupMsg.mIncrementalBackupLevel  = sInitPlan->mLevel;
    sBackupMsg.mTbsId                   = ellGetTablespaceID( & sInitPlan->mSpaceHandle );

    stlMemset( sBackupMsg.mTarget, 0x00, STL_SIZEOF(sBackupMsg.mTarget) );

    STL_TRY( smlBackup( &sBackupMsg,
                        SML_ENV( aEnv ) ) == STL_SUCCESS );

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


/** @} qltAlterSpaceBackup */
