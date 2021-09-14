/*******************************************************************************
 * qlrAlterView.c
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
 * @file qlrAlterView.c
 * @brief ALTER VIEW .. 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>



/**
 * @defgroup qlrAlterView ALTER VIEW
 * @ingroup qlr
 * @{
 */


/**
 * @brief ALTER VIEW 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrValidateAlterView( smlTransId      aTransID,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                stlChar       * aSQLString,
                                qllNode       * aParseTree,
                                qllEnv        * aEnv )
{
    stlStatus sStatus;
    
    qlrInitAlterView * sInitPlan = NULL;
    qlpAlterView     * sParseTree = NULL;

    stlChar  * sViewColumnString = NULL;
    stlChar  * sViewQueryString = NULL;

    stlInt32   sStrLen = 0;

    qllNode * sViewTree = NULL;
    stlInt32      sBindCount = 0;

    qlpPhraseViewedTable * sPhraseViewTable = NULL;
    qlpSelect            * sPhraseViewQuery = NULL;

    ellDictHandle sCurrentUser;

    stlInt64      sViewOwnerID = ELL_DICT_OBJECT_ID_NA;
    ellDictHandle sViewOwner;

    qlvInitNode        * sInitQuery  = NULL;
    stlInt32             sTargetCount = 0;
    qlvInitTarget      * sTargetList = NULL;
    
    stlBool   sObjectExist = STL_FALSE;
    
    stlBool   sHasCycle = STL_FALSE;
    stlBool   sDeleted = STL_FALSE;

    qlvInitStmtExprList  * sStmtExprList  = NULL;
    qlvInitExprList      * sQueryExprList = NULL;
    stlInt32               sQBIndex;
    qlvStmtInfo            sStmtInfo;

    stlBool                sDummy = STL_FALSE;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_VIEW_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_VIEW_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */

    /**
     * Current User 정보 획득
     */

    stlMemcpy( & sCurrentUser, ellGetCurrentUserHandle( ELL_ENV(aEnv) ), STL_SIZEOF( ellDictHandle ) );
    
    sParseTree = (qlpAlterView *) aParseTree;
    
    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlrInitAlterView),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlrInitAlterView) );

    STL_TRY( qlvSetInitPlan( aDBCStmt,
                             aSQLStmt,
                             & sInitPlan->mCommonInit,
                             aEnv )
             == STL_SUCCESS );
    
    /**********************************************************
     * View Name Validation
     **********************************************************/

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
                                                & sCurrentUser,
                                                sParseTree->mViewName->mObject,
                                                & sInitPlan->mViewHandle,
                                                & sObjectExist,
                                                ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_VIEW_NOT_EXIST );

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

        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_SCHEMA_NOT_EXIST );
        
        STL_TRY( ellGetTableDictHandleWithSchema( aTransID,
                                                  aSQLStmt->mViewSCN,
                                                  & sInitPlan->mSchemaHandle,
                                                  sParseTree->mViewName->mObject,
                                                  & sInitPlan->mViewHandle,
                                                  & sObjectExist,
                                                  ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_VIEW_NOT_EXIST );
    }

    /**
     * Built-In View 인지 검사
     */

    STL_TRY_THROW( ellIsBuiltInTable( & sInitPlan->mViewHandle ) == STL_FALSE,
                   RAMP_ERR_BUILT_IN_VIEW );


    /**
     * ALTER ON TABLE 권한 검사
     */

    STL_TRY( qlaCheckTablePriv( aTransID,
                                aDBCStmt,
                                aSQLStmt,
                                ELL_TABLE_PRIV_ACTION_ALTER,
                                & sInitPlan->mViewHandle,
                                NULL, /* aColumnList */
                                aEnv )
             == STL_SUCCESS );

    /**********************************************************
     * Alter View Action
     **********************************************************/

    sInitPlan->mAlterAction = sParseTree->mAlterAction;

    STL_DASSERT( (sInitPlan->mAlterAction > QLP_ALTER_VIEW_ACTION_NA) &&
                 (sInitPlan->mAlterAction < QLP_ALTER_VIEW_ACTION_MAX) );

    /****************************************************************
     * View 정보 획득
     ****************************************************************/

    /**
     * View Phrase 획득
     */
    
    sViewColumnString = ellGetViewColumnString( & sInitPlan->mViewHandle );
    sViewQueryString  = ellGetViewQueryString( & sInitPlan->mViewHandle );

    STL_DASSERT( sViewColumnString != NULL );
    STL_DASSERT( sViewQueryString != NULL );

    /**
     * View Column String 저장
     */

    sStrLen = stlStrlen(sViewColumnString);

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                sStrLen + 1,
                                (void **) & sInitPlan->mViewColumnString,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlStrcpy( sInitPlan->mViewColumnString, sViewColumnString );

    /**
     * View 의 origin query expression 을 저장
     */

    sStrLen = stlStrlen(sViewQueryString);

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                sStrLen + 1,
                                (void **) & sInitPlan->mViewQueryString,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlStrcpy( sInitPlan->mViewQueryString, sViewQueryString);

    /****************************************************************
     * View Phase Parsing
     ****************************************************************/

    /**
     * View Column Phrase Parsing
     */
    
    qlgSetQueryPhaseMemMgr( aDBCStmt, QLL_PHASE_PARSING, aEnv );
    
    sStatus = qlgParseInternal( aDBCStmt,
                                sViewColumnString,
                                & sViewTree,
                                & sBindCount,
                                aEnv );

    qlgSetQueryPhaseMemMgr( aDBCStmt, QLL_PHASE_VALIDATION, aEnv );
    
    STL_TRY_THROW( sStatus == STL_SUCCESS, RAMP_ERR_VIEW_HAS_ERRORS );
    
    STL_DASSERT( sBindCount == 0 );
    STL_DASSERT( sViewTree->mType == QLL_PHRASE_VIEWED_TABLE_TYPE );

    sPhraseViewTable = (qlpPhraseViewedTable *) sViewTree;

    /**
     * View Query Parsing
     */

    qlgSetQueryPhaseMemMgr( aDBCStmt, QLL_PHASE_PARSING, aEnv );
    
    sStatus = qlgParseInternal( aDBCStmt,
                                sViewQueryString,
                                & sViewTree,
                                & sBindCount,
                                aEnv );

    qlgSetQueryPhaseMemMgr( aDBCStmt, QLL_PHASE_VALIDATION, aEnv );
    
    STL_TRY_THROW( sStatus == STL_SUCCESS, RAMP_ERR_VIEW_HAS_ERRORS );
    
    STL_DASSERT( sBindCount == 0 );
    STL_DASSERT( sViewTree->mType == QLL_STMT_SELECT_TYPE );
    
    sPhraseViewQuery = (qlpSelect *) sViewTree;

    STL_DASSERT( sPhraseViewQuery->mQueryNode != NULL );
    STL_DASSERT( sPhraseViewQuery->mInto == NULL );
    STL_DASSERT( sPhraseViewQuery->mUpdatability == NULL );

    /****************************************************************
     * View Query Validation 
     ****************************************************************/

    /**
     * View Owner 로 Authorization Stack 변경
     */

    sViewOwnerID = ellGetTableOwnerID( & sInitPlan->mViewHandle );

    STL_TRY( ellGetAuthDictHandleByID( aTransID,
                                       aSQLStmt->mViewSCN,
                                       sViewOwnerID,
                                       & sViewOwner,
                                       & sObjectExist,
                                       ELL_ENV(aEnv) )
             == STL_SUCCESS );
    STL_DASSERT( sObjectExist == STL_TRUE );

    ellSetCurrentUser( & sViewOwner, ELL_ENV(aEnv) );

    /**
     * Cycle Detection 을 위한 View Handle 등록
     * ex-1) CREATE FORCE VIEW v1 AS SELECT * FROM t1, v1;
     * ex-2) CREATE TABLE t1 ( c1 INTEGER );
     * ex-3) ALTER VIEW v1 COMPILE;
     */
    
    STL_TRY( ellAddNewObjectItem( aSQLStmt->mViewCycleList,
                                  & sInitPlan->mViewHandle,
                                  & sHasCycle,
                                  & aDBCStmt->mShareMemStmt,
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_DASSERT( sHasCycle == STL_FALSE );
    
    /**
     * Statement 단위 Expression List 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlvInitStmtExprList ),
                                (void **) & sStmtExprList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( & sStmtExprList->mPseudoColExprList,
                                   QLL_INIT_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( & sStmtExprList->mConstExprList,
                                   QLL_INIT_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    sStmtExprList->mHasSubQuery = STL_FALSE;

    /**
     * Query 단위 Expression List 설정
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlvInitExprList ),
                                (void **) & sQueryExprList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( & sQueryExprList->mPseudoColExprList,
                                   QLL_INIT_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( & sQueryExprList->mAggrExprList,
                                   QLL_INIT_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( & sQueryExprList->mNestedAggrExprList,
                                   QLL_INIT_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    sQueryExprList->mStmtExprList = sStmtExprList;

    /**
     * statement information 설정
     */

    sQBIndex = 0;

    sStmtInfo.mTransID       = aTransID;
    sStmtInfo.mHasHintError  = STL_FALSE;
    sStmtInfo.mQBIndex       = &sQBIndex;
    stlStrcpy( sStmtInfo.mQBPrefix, QLV_QUERY_BLOCK_NAME_PREFIX_INS );
    sStmtInfo.mDBCStmt       = aDBCStmt;
    sStmtInfo.mSQLStmt       = aSQLStmt;
    sStmtInfo.mSQLString     = sInitPlan->mViewQueryString;
    sStmtInfo.mQueryExprList = sQueryExprList;
    
    /**
     * View Query Validation
     */

    sStatus = qlvQueryNodeValidation( & sStmtInfo,
                                      STL_FALSE,
                                      sInitPlan->mCommonInit.mValidationObjList,
                                      NULL,
                                      (qllNode *) sPhraseViewQuery->mQueryNode,
                                      & sInitPlan->mInitQueryNode,
                                      aEnv );

    /**
     * Current User 로 Authorization Stack 원복
     */

    ellSetCurrentUser( & sCurrentUser, ELL_ENV(aEnv) );

    /**
     * View Query Plan 설정
     */
    
    STL_TRY_THROW( sStatus == STL_SUCCESS, RAMP_ERR_VIEW_HAS_ERRORS );
    
    /**
     * View Cycle List 에서 제거
     */

    ellDeleteObjectItem( aSQLStmt->mViewCycleList,
                         & sInitPlan->mViewHandle,
                         & sDeleted );
    
    STL_DASSERT( sDeleted == STL_TRUE );

    /****************************************************************
     * View Target Validation 
     ****************************************************************/

    /**
     * Target 갯수 획득
     */

    sInitQuery = sInitPlan->mInitQueryNode;
    switch( sInitQuery->mType )
    {
        case QLV_NODE_TYPE_QUERY_SET :
            {
                sTargetCount = ((qlvInitQuerySetNode*) sInitQuery)->mSetTargetCount;
                sTargetList  = ((qlvInitQuerySetNode*) sInitQuery)->mSetTargets;
                break;
            }
        case QLV_NODE_TYPE_QUERY_SPEC :
            {
                sTargetCount = ((qlvInitQuerySpecNode*) sInitQuery)->mTargetCount;
                sTargetList  = ((qlvInitQuerySpecNode*) sInitQuery)->mTargets;
                break;
            }
        default :
            {
                STL_DASSERT( 0 );
                break;
            }
    }

    /**
     * Column Name List 구축 
     */
    
    sInitPlan->mColumnCount = sTargetCount;
    
    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(stlChar *) * sTargetCount,
                                (void **) & sInitPlan->mColumnName,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan->mColumnName, 0x00, STL_SIZEOF(stlChar *) * sTargetCount );

    /**
     * Validation Column Name List
     */

    STL_TRY_THROW( qlrValidateUserColList( aDBCStmt,
                                           sInitPlan->mViewQueryString,
                                           sInitPlan->mViewColumnString,
                                           sPhraseViewTable->mColumnList,
                                           sTargetCount,
                                           sTargetList,
                                           sInitPlan->mColumnName,
                                           STL_FALSE, /* IsForce, For CREATE VIEW AS SELECT */
                                           & sDummy,  /* Not Use, For CREATE VIEW AS SELECT */
                                           STL_TRUE,  /* Check Sequence */ 
                                           aEnv )
                   == STL_SUCCESS, RAMP_ERR_VIEW_HAS_ERRORS );

    /**********************************************************
     * View 참조 객체 정보 획득
     **********************************************************/

    /**
     * Underlying Relation List 획득
     */
    
    STL_TRY( ellGetObjectList( sInitPlan->mCommonInit.mValidationObjList,
                               ELL_OBJECT_TABLE,
                               & sInitPlan->mUnderlyingRelationList,
                               QLL_INIT_PLAN(aDBCStmt),
                               ELL_ENV(aEnv) )
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

    STL_CATCH( RAMP_ERR_VIEW_HAS_ERRORS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_VIEW_HAS_ERRORS,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mViewName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      ellGetSchemaName( & sInitPlan->mSchemaHandle ),
                      ellGetTableName( & sInitPlan->mViewHandle ) );
    }

    
    /* STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED ) */
    /* { */
    /*     stlPushError( STL_ERROR_LEVEL_ABORT, */
    /*                   QLL_ERRCODE_NOT_IMPLEMENTED, */
    /*                   NULL, */
    /*                   QLL_ERROR_STACK(aEnv), */
    /*                   "qlrValidateAlterView()" ); */
    /* } */
    
    STL_FINISH;

    /**
     * Current User 를 Session User 로 원복 
     */

    ellRollbackCurrentUser( ELL_ENV(aEnv) );
    
    return STL_FAILURE;
}

/**
 * @brief ALTER VIEW 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrOptCodeAlterView( smlTransId      aTransID,
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
 * @brief ALTER VIEW 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrOptDataAlterView( smlTransId      aTransID,
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
 * @brief ALTER VIEW 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qlrExecuteAlterView( smlTransId      aTransID,
                               smlStatement  * aStmt,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv )
{
    qlrInitAlterView * sInitPlan = NULL;

    stlTime sTime = 0;
    stlBool   sLocked = STL_TRUE;

    stlInt64   sViewOwnerID  = ELL_DICT_OBJECT_ID_NA;
    stlInt64   sViewSchemaID = ELL_DICT_OBJECT_ID_NA;
    stlInt64   sViewID       = ELL_DICT_OBJECT_ID_NA;
    stlInt64   sColumnID     = ELL_DICT_OBJECT_ID_NA;

    qlrInitColumn    * sCodeColumn = NULL;
    ellAddColumnDesc * sColumnDesc = NULL;

    qllTarget * sTargetList = NULL;
    qllTarget * sTargetItem = NULL;

    ellObjectItem   * sObjectItem = NULL;
    ellDictHandle   * sObjectHandle = NULL;

    ellObjectList * sViewColumnList = NULL;
    
    stlInt32  i = 0;

    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_VIEW_TYPE,
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

    /******************************************************************
     * Run-Time Validation
     ******************************************************************/

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

    sInitPlan = (qlrInitAlterView *) aSQLStmt->mInitPlan;
    
    /**
     * ALTER VIEW 구문을 위한 DDL 획득
     */
    
    STL_TRY( ellLock4AlterView( aTransID,
                                aStmt,
                                & sInitPlan->mViewHandle,
                                & sLocked,
                                ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    /**
     * Underlying Relation 에 대해 IS lock 획득
     */

    STL_TRY( ellLockUnderlyingTableList4ViewDDL( aTransID,
                                                 aStmt,
                                                 SML_LOCK_IS,
                                                 sInitPlan->mUnderlyingRelationList,
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

    sInitPlan = (qlrInitAlterView *) aSQLStmt->mInitPlan;

    /**********************************************************
     * 관련 객체 정보 획득
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
     * View 의 기본 정보 획득
     * - 기존 View 의 정보를 유지한다.
     */

    sViewOwnerID  = ellGetTableOwnerID( & sInitPlan->mViewHandle );
    sViewSchemaID = ellGetTableSchemaID( & sInitPlan->mViewHandle );
    sViewID       = ellGetTableID( & sInitPlan->mViewHandle );

    /***************************************************************
     * Dictionary Record 변경
     ***************************************************************/

    /**
     * View 의 기존 Dictionary 정보 제거
     */

    STL_TRY( ellRemoveDict4AlterView( aTransID,
                                      aStmt,
                                      & sInitPlan->mViewHandle,
                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * COLUMNS descriptor 공간 할당 
     */

    STL_TRY( knlAllocRegionMem( & QLL_EXEC_DDL_MEM(aEnv),
                                STL_SIZEOF(qlrInitColumn) * sInitPlan->mColumnCount,
                                (void **) & sCodeColumn,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sCodeColumn, 0x00, STL_SIZEOF(qlrInitColumn) * sInitPlan->mColumnCount );

    STL_TRY( knlAllocRegionMem( & QLL_EXEC_DDL_MEM(aEnv),
                                STL_SIZEOF(ellAddColumnDesc) * sInitPlan->mColumnCount,
                                (void **) & sColumnDesc,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sColumnDesc, 0x00, STL_SIZEOF(ellAddColumnDesc) * sInitPlan->mColumnCount );
    
    /**
     * View Query 의 Target 정보 획득
     */

    STL_TRY( qlfMakeQueryTarget( aSQLStmt,
                                 & QLL_EXEC_DDL_MEM(aEnv),
                                 & sTargetList, 
                                 aEnv )
             == STL_SUCCESS );

    /**
     * COLUMNS descriptor 추가 
     */
    
    for ( i = 0, sTargetItem = sTargetList;
          i < sInitPlan->mColumnCount;
          i++, sTargetItem = sTargetItem->mNext )
    {
        /**
         * Code Column 정보 구성
         */
        
        qlrSetColumnInfoFromTarget( & sCodeColumn[i],
                                    i,
                                    sInitPlan->mColumnName[i],
                                    sTargetItem );
        
        STL_TRY( qlrExecuteAddColumnDesc( aTransID,
                                          aStmt,
                                          sViewOwnerID,
                                          sViewSchemaID,
                                          sViewID,
                                          ELL_TABLE_TYPE_VIEW,
                                          & sCodeColumn[i],
                                          & sColumnDesc[i],
                                          & sColumnID,
                                          aEnv )
                 == STL_SUCCESS );
    }
    
    /**
     * VIEWS descriptor 추가
     */

    STL_TRY( ellInsertViewDesc( aTransID,
                                aStmt,
                                sViewOwnerID,
                                sViewSchemaID,
                                sViewID,
                                sInitPlan->mViewColumnString,
                                sInitPlan->mViewQueryString,
                                STL_TRUE,   /* aIsCompiled */
                                STL_FALSE,  /* aIsAffected */
                                ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * VIEW_TABLE_USAGE descriptor 추가
     */
    
    STL_RING_FOREACH_ENTRY( & sInitPlan->mUnderlyingRelationList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;
        
        STL_TRY( ellInsertViewTableUsageDesc( aTransID,
                                              aStmt,
                                              sViewOwnerID,
                                              sViewSchemaID,
                                              sViewID,
                                              ellGetTableOwnerID( sObjectHandle ),
                                              ellGetTableSchemaID( sObjectHandle ),
                                              ellGetTableID( sObjectHandle ),
                                              ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    
    /******************************************************************
     * Dictionary Cache 재구성 
     ******************************************************************/

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
     * ALTER VIEW COMPILE 을 위한 Refine Cache
     */

    STL_TRY( ellRefineCache4AlterViewCompile( aTransID,
                                              aStmt,
                                              & sInitPlan->mViewHandle,
                                              ELL_ENV(aEnv) )
             == STL_SUCCESS );
                                              
    
    return STL_SUCCESS;

    /* STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED ) */
    /* { */
    /*     stlPushError( STL_ERROR_LEVEL_ABORT, */
    /*                   QLL_ERRCODE_NOT_IMPLEMENTED, */
    /*                   NULL, */
    /*                   QLL_ERROR_STACK(aEnv), */
    /*                   "qlrExecuteAlterView()" ); */
    /* } */
    
    STL_FINISH;

    return STL_FAILURE;
}



/** @} qlrAlterTableAlterView */
