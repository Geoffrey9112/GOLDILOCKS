/*******************************************************************************
 * qldAlterDatabaseRestoreCtrlfile.c
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
 * @file qldAlterDatabaseRestoreCtrlfile.c
 * @brief ALTER DATABASE RESTORE CONTROLFILE 처리 루틴 
 */

#include <qll.h>
#include <qlDef.h>

/**
 * @defgroup qldAlterDatabaseRestoreCtrlfile ALTER DATABASE RESTORE CONTROLFILE
 * @ingroup qldAlterDatabase
 * @{
 */


/**
 * @brief ALTER DATABASE RESTORE CONTROLFILE FROM 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldValidateAlterDatabaseRestoreCtrlfile( smlTransId      aTransID,
                                                   qllDBCStmt    * aDBCStmt,
                                                   qllStatement  * aSQLStmt,
                                                   stlChar       * aSQLString,
                                                   qllNode       * aParseTree,
                                                   qllEnv        * aEnv )
{
    qlpRestoreCtrlfile     * sParseTree;
    qldInitRestoreCtrlfile * sInitPlan = NULL;
    stlBool                  sExist = STL_FALSE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_DATABASE_RESTORE_CTRLFILE_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_DATABASE_RESTORE_CTRLFILE_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sParseTree = (qlpRestoreCtrlfile *) aParseTree;

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
                                STL_SIZEOF(qldInitRestoreCtrlfile),
                                (void **) &sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sInitPlan->mTarget, 0x00, STL_SIZEOF(qldInitRestoreCtrlfile) );
    stlStrcpy( sInitPlan->mTarget, QLP_GET_PTR_VALUE(sParseTree->mTarget) );

    STL_TRY( smlConvAbsLogfilePath( sInitPlan->mTarget,
                                    SML_ENV(aEnv) ) == STL_SUCCESS );

    STL_TRY( stlExistFile( sInitPlan->mTarget,
                           &sExist,
                           QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS );

    STL_TRY_THROW( sExist == STL_TRUE, RAMP_ERR_NOT_EXIST_FILE );
    
    /**
     * Init Plan 연결 
     */

    aSQLStmt->mInitPlan = (void *) sInitPlan;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_EXIST_FILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_FILE_NOT_EXIST,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      sInitPlan->mTarget );
    }

    STL_FINISH;

    return STL_FAILURE;
    
}

/**
 * @brief ALTER DATABASE RESTORE CONTROLFILE FROM 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldOptCodeAlterDatabaseRestoreCtrlfile( smlTransId      aTransID,
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
 * @brief ALTER DATABASE RESTORE CONTROLFILE FROM 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldOptDataAlterDatabaseRestoreCtrlfile( smlTransId      aTransID,
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
 * @brief ALTER DATABASE RESTORE CONTROLFILE FROM 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qldExecuteAlterDatabaseRestoreCtrlfile( smlTransId      aTransID,
                                                  smlStatement  * aStmt,
                                                  qllDBCStmt    * aDBCStmt,
                                                  qllStatement  * aSQLStmt,
                                                  qllEnv        * aEnv )
{
    qldInitRestoreCtrlfile * sInitPlan = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_DATABASE_RESTORE_CTRLFILE_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * Init Plan 획득 
     */

    sInitPlan = (qldInitRestoreCtrlfile *) aSQLStmt->mInitPlan;

    STL_TRY( smlRestoreCtrlFile( sInitPlan->mTarget, SML_ENV( aEnv ) ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/** @} qldAlterDatabaseRestoreCtrlfile */
