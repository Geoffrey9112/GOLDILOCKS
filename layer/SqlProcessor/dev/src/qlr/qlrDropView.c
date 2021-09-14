/*******************************************************************************
 * qlrDropView.c
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
 * @file qlrDropView.c
 * @brief DROP VIEW 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qlrDropView DROP VIEW
 * @ingroup qlr
 * @{
 */


/**
 * @brief DROP VIEW 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrValidateDropView( smlTransId      aTransID,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               stlChar       * aSQLString,
                               qllNode       * aParseTree,
                               qllEnv        * aEnv )
{
    ellDictHandle * sAuthHandle = NULL;
    
    qlrInitDropView * sInitPlan = NULL;
    qlpDropView     * sParseTree = NULL;

    stlBool   sObjectExist = STL_FALSE;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_DROP_VIEW_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_DROP_VIEW_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */

    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    sParseTree = (qlpDropView *) aParseTree;
    
    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlrInitDropView),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlrInitDropView) );

    STL_TRY( qlvSetInitPlan( aDBCStmt,
                             aSQLStmt,
                             & sInitPlan->mCommonInit,
                             aEnv )
             == STL_SUCCESS );
    
    /**********************************************************
     * View Name Validation
     **********************************************************/

    /**
     * mIfExists
     */

    sInitPlan->mIfExists = sParseTree->mIfExists;

    /**
     * View Name 존재 여부
     */
    
    if ( sParseTree->mViewName->mSchema == NULL )
    {
        /**
         * Schema 가 명시되지 않은 경우
         */

        STL_TRY( ellGetTableDictHandleWithAuth( aTransID,
                                                aSQLStmt->mViewSCN,
                                                sAuthHandle,
                                                sParseTree->mViewName->mObject,
                                                & sInitPlan->mViewHandle,
                                                & sObjectExist,
                                                ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sObjectExist == STL_TRUE )
        {
            /**
             * View 가 존재함
             */
            
            sInitPlan->mViewExist = STL_TRUE;
        }
        else
        {
            /**
             * View 가 없음
             */
            
            sInitPlan->mViewExist = STL_FALSE;
            
            /**
             * If Exists 옵션이 있다면 무조건 SUCCESS 한다.
             */
            
            if ( sInitPlan->mIfExists == STL_TRUE )
            {
                STL_THROW( RAMP_IF_EXISTS_NO_VIEW );
            }
            else
            {
                STL_THROW( RAMP_ERR_VIEW_NOT_EXIST );
            }
        }

        /**
         * View 의 Schema Handle 획득
         */
        
        STL_TRY( ellGetSchemaDictHandleByID(
                     aTransID,
                     aSQLStmt->mViewSCN,
                     ellGetTableSchemaID( & sInitPlan->mViewHandle ),
                     & sInitPlan->mSchemaHandle,
                     & sObjectExist,
                     ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_VIEW_NOT_EXIST );
    }
    else
    {
        /**
         * Schema 가 명시된 경우
         */

        STL_TRY( ellGetSchemaDictHandle( aTransID,
                                         aSQLStmt->mViewSCN,
                                         sParseTree->mViewName->mSchema,
                                         & sInitPlan->mSchemaHandle,
                                         & sObjectExist,
                                         ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sObjectExist == STL_TRUE )
        {
            /* Schema 가 존재함 */
        }
        else
        {
            /**
             * Schema 가 존재하지 않을 경우,
             * If Exists 옵션이 있다면 무조건 SUCCESS 한다.
             */

            sInitPlan->mViewExist = STL_FALSE;
            
            if ( sInitPlan->mIfExists == STL_TRUE )
            {
                STL_THROW( RAMP_IF_EXISTS_NO_VIEW );
            }
            else
            {
                STL_THROW( RAMP_ERR_SCHEMA_NOT_EXIST );
            }
        }
        
        STL_TRY( ellGetTableDictHandleWithSchema( aTransID,
                                                  aSQLStmt->mViewSCN,
                                                  & sInitPlan->mSchemaHandle,
                                                  sParseTree->mViewName->mObject,
                                                  & sInitPlan->mViewHandle,
                                                  & sObjectExist,
                                                  ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sObjectExist == STL_TRUE )
        {
            /**
             * View 가 존재함
             */
            
            sInitPlan->mViewExist = STL_TRUE;
        }
        else
        {
            /**
             * View 가 없음
             */
            
            sInitPlan->mViewExist = STL_FALSE;
            
            /**
             * If Exists 옵션이 있다면 무조건 SUCCESS 한다.
             */
            
            if ( sInitPlan->mIfExists == STL_TRUE )
            {
                STL_THROW( RAMP_IF_EXISTS_NO_VIEW );
            }
            else
            {
                STL_THROW( RAMP_ERR_VIEW_NOT_EXIST );
            }
        }
    }

    /**
     * Built-In View 인지 검사
     */

    STL_TRY_THROW( ellIsBuiltInTable( & sInitPlan->mViewHandle ) == STL_FALSE,
                   RAMP_ERR_BUILT_IN_VIEW );

    /**
     * View 인지 검사
     */
    
    STL_TRY_THROW( ELL_TABLE_TYPE_VIEW == ellGetTableType( & sInitPlan->mViewHandle ),
                   RAMP_ERR_NOT_VIEW );

    /**
     * DROP VIEW 권한 검사
     */

    STL_TRY( qlaCheckPrivDropView( aTransID,
                                   aDBCStmt,
                                   aSQLStmt,
                                   & sInitPlan->mViewHandle,
                                   aEnv )
             == STL_SUCCESS );

    /********************************************************
     * Validation Object 추가 
     ********************************************************/

    STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                  & sInitPlan->mViewHandle,
                                  QLL_INIT_PLAN(aDBCStmt),
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /***********************************************************
     * Init Plan 연결 
     ***********************************************************/
    
    /**
     * Init Plan 연결 
     */

    STL_RAMP( RAMP_IF_EXISTS_NO_VIEW );
    
    aSQLStmt->mInitPlan = (void *) sInitPlan;
    
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_VIEW_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLE_OR_VIEW_NOT_EXIST,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mViewName->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_SCHEMA_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mViewName->mSchemaPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sParseTree->mViewName->mSchema );
    }

    STL_CATCH( RAMP_ERR_BUILT_IN_VIEW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_BUILT_IN_TABLE_IS_NOT_MODIFIABLE,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mViewName->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sParseTree->mViewName->mObject );
    }

    STL_CATCH( RAMP_ERR_NOT_VIEW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_IDENTIFIER,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mViewName->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sParseTree->mViewName->mObject );
    }
    
    /* STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED ) */
    /* { */
    /*     stlPushError( STL_ERROR_LEVEL_ABORT, */
    /*                   QLL_ERRCODE_NOT_IMPLEMENTED, */
    /*                   NULL, */
    /*                   QLL_ERROR_STACK(aEnv), */
    /*                   "qlrValidateCreateView()" ); */
    /* } */
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief DROP VIEW 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrOptCodeDropView( smlTransId      aTransID,
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
 * @brief DROP VIEW 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrOptDataDropView( smlTransId      aTransID,
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
 * @brief DROP VIEW 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qlrExecuteDropView( smlTransId      aTransID,
                               smlStatement  * aStmt,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv )
{
    qlrInitDropView * sInitPlan = NULL;

    ellObjectList * sViewColumnList = NULL;
    ellObjectList * sAffectedViewList = NULL;
    
    stlTime sTime = 0;
    stlBool sLocked = STL_TRUE;
    

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_DROP_VIEW_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mInitPlan != NULL, QLL_ERROR_STACK(aEnv) );

    /***************************************************************
     * 기본 정보 설정 
     ***************************************************************/
    
    /**
     * DDL 수행 시간 설정
     */

    sTime = stlNow();
    STL_TRY( ellBeginStmtDDL( aStmt, sTime, ELL_ENV(aEnv) ) == STL_SUCCESS );

    /**********************************************************
     *  Run-Time Validation
     **********************************************************/

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

    sInitPlan = (qlrInitDropView *) aSQLStmt->mInitPlan;

    /**
     * IF EXISTS 옵션 처리
     */

    if ( (sInitPlan->mIfExists == STL_TRUE) &&
         (sInitPlan->mViewExist != STL_TRUE) )
    {
        /**
         * IF EXISTS 옵션을 사용하고, Table 이 존재하지 않는 경우
         */
        
        STL_THROW( RAMP_IF_EXISTS_SUCCESS );
    }
    else
    {
        /* go go */
    }
    
    /**
     * DDL Lock 획득 
     */

    STL_TRY( ellLock4DropView( aTransID,
                               aStmt,
                               & sInitPlan->mViewHandle,
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

    sInitPlan = (qlrInitDropView *) aSQLStmt->mInitPlan;
    
    /**********************************************************
     * 관련 객체 정보 획득 및 Lock 획득
     **********************************************************/
    
    /**
     * View Column List 획득
     */

    STL_TRY( ellGetViewColumnList( aTransID,
                                   aStmt,
                                   & sInitPlan->mViewHandle,
                                   & QLL_EXEC_DDL_MEM(aEnv),
                                   & sViewColumnList,
                                   ELL_ENV(aEnv) )
             == STL_SUCCESS );
                                   
    /**
     * Affected View List 에 대해 X Lock 획득
     */
    
    STL_TRY( ellGetAffectedViewListByRelation( aTransID,
                                               aStmt,
                                               & sInitPlan->mViewHandle,
                                               & QLL_EXEC_DDL_MEM(aEnv),
                                               & sAffectedViewList,
                                               ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellLockRelatedTableList4DDL( aTransID,
                                          aStmt,
                                          SML_LOCK_X,
                                          sAffectedViewList,
                                          & sLocked,
                                          ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    STL_TRY( ellGetAffectedViewListByRelation( aTransID,
                                               aStmt,
                                               & sInitPlan->mViewHandle,
                                               & QLL_EXEC_DDL_MEM(aEnv),
                                               & sAffectedViewList,
                                               ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /***************************************************************
     * Dictionary Record 제거 
     ***************************************************************/

    /**
     * View 의 Dictionary 정보 제거
     */

    STL_TRY( ellRemoveDict4DropView( aTransID,
                                     aStmt,
                                     & sInitPlan->mViewHandle,
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Affected View List 에 대해 IS_AFFECTED 를 TRUE 로 변경
     */

    STL_TRY( ellSetAffectedViewList( aTransID,
                                     aStmt,
                                     sAffectedViewList,
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /***************************************************************
     * Dictionary Cache 재구성 
     ***************************************************************/

    /**
     * View 의 Column Cache 제거
     * - View 의 Table Cache 와 Column Cache 간에는 연결 관계가 없음
     */

    STL_TRY( ellRefineCache4RemoveViewColumns( aTransID,
                                               aStmt,
                                               sViewColumnList,
                                               ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * View 의 Cache 제거
     */

    STL_TRY( ellRefineCache4DropView( aTransID,
                                      & sInitPlan->mViewHandle,
                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * If Exists 옵션을 사용하고, Table 이 존재하지 않는 경우
     */
    
    STL_RAMP( RAMP_IF_EXISTS_SUCCESS );
    
    return STL_SUCCESS;

    /* STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED ) */
    /* { */
    /*     stlPushError( STL_ERROR_LEVEL_ABORT, */
    /*                   QLL_ERRCODE_NOT_IMPLEMENTED, */
    /*                   NULL, */
    /*                   QLL_ERROR_STACK(aEnv), */
    /*                   "qlrExecuteDropView()" ); */
    /* } */
    
    STL_FINISH;

    return STL_FAILURE;
}



/** @} qlrDropView */
