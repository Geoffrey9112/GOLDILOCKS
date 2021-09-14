/*******************************************************************************
 * qlssSetSessionAuth.c
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
 * @file qlssSetSessionAuth.c
 * @brief SET SESSION AUTH AUTHORIZATION 처리 루틴 
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @defgroup qlssSessionSetSessionAuth SET SESSION AUTHORIZATION
 * @ingroup qlssSession
 * @{
 */


/**
 * @brief SET SESSION AUTHORIZATION 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssValidateSetSessionAuth( smlTransId      aTransID,
                                      qllDBCStmt    * aDBCStmt,
                                      qllStatement  * aSQLStmt,
                                      stlChar       * aSQLString,
                                      qllNode       * aParseTree,
                                      qllEnv        * aEnv )
{
    qlssInitSetAuth   * sInitPlan = NULL;
    qlpSetSessionAuth * sParseTree = NULL;

    ellDictHandle     * sAuthHandle = NULL;

    stlBool             sGetSessUser = STL_FALSE;
    ellDictHandle       sSessionUser;

    stlBool    sObjectExist = STL_FALSE;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_SET_SESSION_AUTHORIZATION_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_SET_SESSION_AUTHORIZATION_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */

    sParseTree = (qlpSetSessionAuth *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlssInitSetAuth),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlssInitSetAuth) );

    STL_TRY( qlvSetInitPlan( aDBCStmt,
                             aSQLStmt,
                             & sInitPlan->mCommonInit,
                             aEnv )
             == STL_SUCCESS );
    
    /*************************************************
     * User name 검증 
     *************************************************/

    /**
     * User 존재 여부
     */

    STL_TRY( ellGetAuthDictHandle( aTransID,
                                   aSQLStmt->mViewSCN,
                                   QLP_GET_PTR_VALUE( sParseTree->mUserName ),
                                   & sInitPlan->mUserHandle,
                                   & sObjectExist,
                                   ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_USER_NOT_EXIST );

    /**
     * User 인지 검증
     */

    STL_TRY_THROW( ellGetAuthType( & sInitPlan->mUserHandle ) == ELL_AUTHORIZATION_TYPE_USER,
                   RAMP_ERR_USER_NOT_EXIST );

    /*************************************************
     * Login User 의 권한 검증 
     *************************************************/

    sGetSessUser = STL_TRUE;
    stlMemcpy( & sSessionUser,
               ellGetSessionUserHandle( ELL_ENV(aEnv) ),
               STL_SIZEOF( ellDictHandle ) );
    
    /**
     * Login User 를 Session User 로 설정
     */

    sAuthHandle = ellGetLoginUserHandle( ELL_ENV(aEnv) );
    ellSetSessionUser( sAuthHandle, ELL_ENV(aEnv) );

    /**
     * ACCESS CONTROL ON DATABASE 권한 검사
     */

    STL_TRY( qlaCheckDatabasePriv( aTransID,
                                   aDBCStmt,
                                   aSQLStmt,
                                   ELL_DB_PRIV_ACTION_ACCESS_CONTROL,
                                   aEnv )
             == STL_SUCCESS );

    /**
     * Session User 를 원복 
     */
    
    ellSetSessionUser( & sSessionUser, ELL_ENV(aEnv) );
    sGetSessUser = STL_FALSE;

    /********************************************************
     * Validation Object 추가 
     ********************************************************/

    STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                  & sInitPlan->mUserHandle,
                                  QLL_INIT_PLAN(aDBCStmt),
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /***********************************************************
     * Init Plan 연결 
     ***********************************************************/
    
    /**
     * Init Plan 연결 
     */

    aSQLStmt->mInitPlan = (void *) sInitPlan;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_USER_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_USER_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mUserName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      QLP_GET_PTR_VALUE( sParseTree->mUserName ) );
    }
    
    /* STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED ) */
    /* { */
    /*     stlPushError( STL_ERROR_LEVEL_ABORT, */
    /*                   QLL_ERRCODE_NOT_IMPLEMENTED, */
    /*                   NULL, */
    /*                   QLL_ERROR_STACK(aEnv), */
    /*                   "qlssValidateSetSessionAuth()" ); */
    /* } */
    
    STL_FINISH;

    /**
     * Session User 를 원복 
     */

    if ( sGetSessUser == STL_TRUE )
    {
        ellSetSessionUser( & sSessionUser, ELL_ENV(aEnv) );
    }
    
    return STL_FAILURE;
}

/**
 * @brief SET SESSION AUTHORIZATION 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssOptCodeSetSessionAuth( smlTransId      aTransID,
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
 * @brief SET SESSION AUTHORIZATION 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssOptDataSetSessionAuth( smlTransId      aTransID,
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
 * @brief SET SESSION AUTH 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qlssExecuteSetSessionAuth( smlTransId      aTransID,
                                     smlStatement  * aStmt,
                                     qllDBCStmt    * aDBCStmt,
                                     qllStatement  * aSQLStmt,
                                     qllEnv        * aEnv )
{
    qlssInitSetAuth  * sInitPlan = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_SET_SESSION_AUTHORIZATION_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mInitPlan != NULL, QLL_ERROR_STACK(aEnv) );

    /**********************************************************
     *  Run-Time Validation
     **********************************************************/

    /**
     * Valid Plan 획득
     */

    STL_TRY( qlgGetValidPlan4DDL( aTransID,
                                  aDBCStmt,
                                  aSQLStmt,
                                  QLL_PHASE_EXECUTE,
                                  aEnv )
             == STL_SUCCESS );

    sInitPlan = (qlssInitSetAuth *) aSQLStmt->mInitPlan;

    /***************************************************************
     *  Execution
     ***************************************************************/

    /**
     * Session User 를 설정 
     */
    
    ellSetSessionUser( & sInitPlan->mUserHandle, ELL_ENV(aEnv) );
    
    return STL_SUCCESS;

    /* STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED ) */
    /* { */
    /*     stlPushError( STL_ERROR_LEVEL_ABORT, */
    /*                   QLL_ERRCODE_NOT_IMPLEMENTED, */
    /*                   NULL, */
    /*                   QLL_ERROR_STACK(aEnv), */
    /*                   "qlssExecuteSetSessionAuth()" ); */
    /* } */
    
    STL_FINISH;

    return STL_FAILURE;
}




/** @} qlssSession */
