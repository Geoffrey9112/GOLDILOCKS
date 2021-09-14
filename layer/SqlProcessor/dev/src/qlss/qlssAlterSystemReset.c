/*******************************************************************************
 * qlssAlterSystemReset.c
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
 * @file qlssAlterSystemReset.c
 * @brief ALTER SYSTEM RESET 처리 루틴 
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @defgroup qlssSystemAlterSystemReset ALTER SYSTEM RESET property
 * @ingroup qlssSystem
 * @{
 */

/**
 * @brief ALTER SYSTEM RESET 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssValidateSystemReset( smlTransId      aTransID,
                                   qllDBCStmt    * aDBCStmt,
                                   qllStatement  * aSQLStmt,
                                   stlChar       * aSQLString,
                                   qllNode       * aParseTree,
                                   qllEnv        * aEnv )
{
    qlssInitSystemSet   * sInitPlan = NULL;
    qlpAlterSystem      * sParseTree = NULL;
    knlPropertyID         sPropertyId = KNL_PROPERTY_ID_MAX;
    qlpValue            * sNameValue;
    stlChar             * sPropertyName = NULL;
    stlBool               sExist = STL_FALSE;

    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_SYSTEM_RESET_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_SYSTEM_RESET_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */
    
    sParseTree = (qlpAlterSystem *) aParseTree;

    STL_PARAM_VALIDATE( sParseTree->mPropertyName != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sParseTree->mSetValue == NULL, QLL_ERROR_STACK(aEnv) );
    
    
    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlssInitSystemSet),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlssInitSystemSet) );

    /**
     * Property 유효성 검사
     */

    sNameValue    = sParseTree->mPropertyName;
    sPropertyName = QLP_GET_PTR_VALUE( sNameValue );

    STL_TRY( knlExistPropertyByName( sPropertyName,
                                     &sExist,
                                     KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    STL_TRY_THROW( sExist == STL_TRUE, RAMP_ERR_NAME );

    /**
     * Init Plan 구성
     */

    STL_TRY( knlGetPropertyIDByName( sPropertyName,
                                     &sPropertyId,
                                     KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    sInitPlan->mPropertyId   = sPropertyId;
    sInitPlan->mIsDeferred   = sParseTree->mIsDeferred;
    sInitPlan->mScope        = sParseTree->mScope;
    sInitPlan->mDefaultValue = STL_TRUE;

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

    STL_CATCH( RAMP_ERR_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_NOT_EXIST,
                      qlgMakeErrPosString( aSQLString,
                                           sNameValue->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK( aEnv ),
                      sPropertyName );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief ALTER SYSTEM RESET 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssOptCodeSystemReset( smlTransId      aTransID,
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
 * @brief ALTER SYSTEM RESET 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssOptDataSystemReset( smlTransId      aTransID,
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
 * @brief ALTER SYSTEM RESET 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qlssExecuteSystemReset( smlTransId      aTransID,
                                  smlStatement  * aStmt,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  qllEnv        * aEnv )
{
    qlssInitSystemSet   * sInitPlan = NULL;

    /**
     * Init Plan 획득 
     */

    sInitPlan = (qlssInitSystemSet *) aSQLStmt->mInitPlan;

    STL_ASSERT( sInitPlan->mPropertyId != KNL_PROPERTY_ID_MAX );

    STL_TRY( knlRemovePropertyValueByID( sInitPlan->mPropertyId,
                                         KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlssSystemAlterSystemReset */


