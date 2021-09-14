/*******************************************************************************
 * qltAlterSpaceRename.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qltAlterSpaceRename.c 5803 2012-09-27 06:40:12Z jhkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qltAlterSpaceRename.c
 * @brief ALTER TABLESPACE .. RENAME TO 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qltAlterSpaceRename ALTER TABLESPACE .. RENAME TO
 * @ingroup qltAlterSpace
 * @{
 */


/**
 * @brief ALTER TABLESPACE .. RENAME TO 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qltValidateAlterSpaceRename( smlTransId      aTransID,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       stlChar       * aSQLString,
                                       qllNode       * aParseTree,
                                       qllEnv        * aEnv )
{
    qltInitAlterSpaceRename * sInitPlan = NULL;
    qlpAlterSpaceRename     * sParseTree = NULL;
    stlBool                   sObjectExist = STL_FALSE;
    ellDictHandle             sSpaceHandle;
    stlInt64                  sSpaceID;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLESPACE_RENAME_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_TABLESPACE_RENAME_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */

    sParseTree = (qlpAlterSpaceRename *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qltInitAlterSpaceRename),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qltInitAlterSpaceRename) );

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

    STL_TRY( ellGetTablespaceDictHandle( aTransID,
                                         aSQLStmt->mViewSCN,
                                         QLP_GET_PTR_VALUE( sParseTree->mNewName ),
                                         & sSpaceHandle,
                                         & sObjectExist,
                                         ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sObjectExist == STL_FALSE, RAMP_ERR_USED_TABLESPACE_NAME );

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                stlStrlen( QLP_GET_PTR_VALUE(sParseTree->mNewName) ) + 1,
                                (void **) & sInitPlan->mNewSpaceName,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    stlStrcpy( sInitPlan->mNewSpaceName, QLP_GET_PTR_VALUE(sParseTree->mNewName) );
    
    /**
     * ALTER TABLESPACE ON DATABASE 권한 검사
     */

    STL_TRY( qlaCheckDatabasePriv( aTransID,
                                   aDBCStmt,
                                   aSQLStmt,
                                   ELL_DB_PRIV_ACTION_ALTER_TABLESPACE,
                                   aEnv )
             == STL_SUCCESS );

    /*************************************************
     * TABLESPACE 검증 
     *************************************************/

    /**
     * Built-in tablespace 검사
     */

    STL_TRY_THROW( ellIsBuiltInTablespace( &sInitPlan->mSpaceHandle ) == STL_FALSE,
                   RAMP_ERR_RENAME_BUILTIN_TABLESPACE );
    
    /**
     * Offline tablespace 검사
     */

    sSpaceID = ellGetTablespaceID( &sInitPlan->mSpaceHandle );
                                    
    STL_TRY_THROW( smlIsOnlineTablespace( sSpaceID ) == STL_TRUE,
                   RAMP_ERR_RENAME_OFFLINE_TABLESPACE );

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
    
    /**
     * Init Plan 연결 
     */

    aSQLStmt->mInitPlan = (void *) sInitPlan;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_USED_TABLESPACE_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_USED_TABLESPACE_NAME,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mNewName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      QLP_GET_PTR_VALUE( sParseTree->mNewName ) );
    }

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

    STL_CATCH( RAMP_ERR_RENAME_BUILTIN_TABLESPACE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_RENAME_BUILTIN_TABLESPACE,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_RENAME_OFFLINE_TABLESPACE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_RENAME_OFFLINE_TABLESPACE,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}



/**
 * @brief ALTER TABLESPACE .. RENAME TO 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qltOptCodeAlterSpaceRename( smlTransId      aTransID,
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
 * @brief ALTER TABLESPACE .. RENAME TO 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qltOptDataAlterSpaceRename( smlTransId      aTransID,
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
 * @brief ALTER TABLESPACE .. RENAME TO 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qltExecuteAlterSpaceRename( smlTransId      aTransID,
                                      smlStatement  * aStmt,
                                      qllDBCStmt    * aDBCStmt,
                                      qllStatement  * aSQLStmt,
                                      qllEnv        * aEnv )
{
    qltInitAlterSpaceRename * sInitPlan = NULL;
    stlTime                   sTime = 0;
    stlInt64                  sSpaceID;

    stlBool   sLocked = STL_TRUE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLESPACE_RENAME_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mInitPlan != NULL, QLL_ERROR_STACK(aEnv) );

    /***************************************************************
     * DDL 준비 
     ***************************************************************/
    
    /**
     * DDL 수행 시간 설정
     */

    sTime = stlNow();
    STL_TRY( ellBeginStmtDDL( aStmt, sTime, ELL_ENV(aEnv) ) == STL_SUCCESS );

    /***************************************************************
     *  Run-Time Validation
     ***************************************************************/

    STL_RAMP( RAMP_RETRY );
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
    
    sInitPlan = (qltInitAlterSpaceRename *) aSQLStmt->mInitPlan;

    /**
     * DDL Lock 획득 
     */

    STL_TRY( ellLock4AlterTablespace( aTransID,
                                      aStmt,
                                      & sInitPlan->mSpaceHandle,
                                      & sLocked,
                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    /**
     * Valid Plan 을 다시 획득
     */

    STL_TRY( qlgGetValidPlan4DDL( aTransID,
                                  aDBCStmt,
                                  aSQLStmt,
                                  QLL_PHASE_EXECUTE,
                                  aEnv )
             == STL_SUCCESS );

    sInitPlan = (qltInitAlterSpaceRename *) aSQLStmt->mInitPlan;

    /***************************************************************
     * RENAME TABLESPACE 
     ***************************************************************/
    
    sSpaceID = ellGetTablespaceID( &sInitPlan->mSpaceHandle );
                                    
    STL_TRY_THROW( smlIsOnlineTablespace( sSpaceID ) == STL_TRUE,
                   RAMP_ERR_RENAME_OFFLINE_TABLESPACE );
    
    KNL_BREAKPOINT( KNL_BREAKPOINT_QLTEXECUTEALTERSPACERENAME_AFTER_LOCK,
                    KNL_ENV(aEnv) );
                                   
    /**
     * Rename Tablespace
     */
    
    STL_TRY( smlRenameTablespace( aStmt,
                                  ellGetTablespaceID( & sInitPlan->mSpaceHandle ),
                                  sInitPlan->mNewSpaceName,
                                  SML_ENV(aEnv) )
             == STL_SUCCESS );

    /***************************************************************
     * Dictionary Record 변경 
     ***************************************************************/

    STL_TRY( ellModifySpaceName( aTransID,
                                 aStmt,
                                 ellGetTablespaceID( & sInitPlan->mSpaceHandle ),
                                 sInitPlan->mNewSpaceName,
                                 ELL_ENV(aEnv) )
             == STL_SUCCESS );
                                 
    /***************************************************************
     * Dictionary Cache 변경 
     ***************************************************************/

    STL_TRY( ellRefineCache4RenameSpace( aTransID,
                                         aStmt,
                                         & sInitPlan->mSpaceHandle,
                                         ELL_ENV(aEnv) )
             == STL_SUCCESS );
                                        
    KNL_BREAKPOINT( KNL_BREAKPOINT_QLTEXECUTEALTERSPACERENAME_AFTER_REFINECACHE,
                    KNL_ENV(aEnv) );
                                   
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_RENAME_OFFLINE_TABLESPACE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_RENAME_OFFLINE_TABLESPACE,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}



/** @} qltAlterSpaceRename */
