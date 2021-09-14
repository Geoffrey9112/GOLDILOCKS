/*******************************************************************************
 * qlrCreateView.c
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
 * @file qlrCreateView.c
 * @brief CREATE VIEW 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qlrCreateView CREATE VIEW
 * @ingroup qlr
 * @{
 */

/**
 * @brief CREATE VIEW 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrValidateCreateView( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 stlChar       * aSQLString,
                                 qllNode       * aParseTree,
                                 qllEnv        * aEnv )
{
    ellDictHandle       * sAuthHandle = NULL;
    
    qlpViewDef          * sParseTree = NULL;
    qlrInitCreateView   * sInitPlan = NULL;

    qlpObjectName       * sObjectName = NULL;
    stlBool               sObjectExist = STL_FALSE;

    stlInt32              sStrLen = 0;

    stlBool               sHasCycle = STL_FALSE;
    stlBool               sDeleted = STL_FALSE;
    
    ellDictHandle          sTableHandle;
    
    stlStatus              sStatus;
    stlInt32               sTargetCount = 0;
    qlvInitTarget        * sTargetList = NULL;
    qlvInitNode          * sInitQuery  = NULL;
    qlvInitStmtExprList  * sStmtExprList  = NULL;
    qlvInitExprList      * sQueryExprList = NULL;
    stlInt32               sQBIndex;
    qlvStmtInfo            sStmtInfo;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_CREATE_VIEW_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_CREATE_VIEW_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */
    
    sParseTree = (qlpViewDef *) aParseTree;
    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    
    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlrInitCreateView),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlrInitCreateView) );

    STL_TRY( qlvSetInitPlan( aDBCStmt,
                             aSQLStmt,
                             & sInitPlan->mCommonInit,
                             aEnv )
             == STL_SUCCESS );
    
    STL_DASSERT( aSQLStmt->mBindCount == 0 );
    
    /**********************************************************
     * Schema Validation
     **********************************************************/

    /**
     * Schema 존재 여부 확인
     */
    
    sObjectName = sParseTree->mViewName;

    if ( sObjectName->mSchema == NULL )
    {
        /**
         * Schema Name 이 명시되지 않은 경우
         */

        STL_TRY( ellGetAuthFirstSchemaDictHandle( aTransID,
                                                  aSQLStmt->mViewSCN,
                                                  sAuthHandle,
                                                  & sInitPlan->mSchemaHandle,
                                                  ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        /**
         * Schema Name 이 명시된 경우
         */

        STL_TRY( ellGetSchemaDictHandle( aTransID,
                                         aSQLStmt->mViewSCN,
                                         sObjectName->mSchema,
                                         & sInitPlan->mSchemaHandle,
                                         & sObjectExist,
                                         ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_SCHEMA_NOT_EXISTS );
    }

    /**
     * Writable Schema 인지 검사
     */
    
    if ( ellGetAuthID( sAuthHandle ) == ellGetAuthIdSYSTEM() )
    {
        /**
         * _SYSTEM 계정으로 CREATE VIEW 를 하고 있는 경우
         */
    }
    else
    {
        STL_TRY_THROW( ellGetSchemaWritable( & sInitPlan->mSchemaHandle ) == STL_TRUE,
                       RAMP_ERR_SCHEMA_NOT_WRITABLE );
    }

    /**
     * CREATE VIEW ON SCHEMA 권한 검사
     */

    STL_TRY( qlaCheckSchemaPriv( aTransID,
                                 aDBCStmt,
                                 aSQLStmt,
                                 ELL_SCHEMA_PRIV_ACTION_CREATE_VIEW,
                                 & sInitPlan->mSchemaHandle,
                                 aEnv )
             == STL_SUCCESS );

    /**********************************************************
     * Create Option 정보 획득
     **********************************************************/
    
    sInitPlan->mOrReplace = sParseTree->mOrReplace;
    sInitPlan->mIsForce   = sParseTree->mIsForce;
    
    /**********************************************************
     * View Name Validation
     **********************************************************/
    
    /**
     * Table (base table or viewed table) 존재 여부 확인
     */

    STL_TRY( ellGetTableDictHandleWithSchema( aTransID,
                                              aSQLStmt->mViewSCN,
                                              & sInitPlan->mSchemaHandle,
                                              sObjectName->mObject,
                                              & sTableHandle,
                                              & sObjectExist,
                                              ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * OR REPLACE 여부에 따른 Handle Validation
     */

    if ( sObjectExist == STL_TRUE )
    {
        STL_TRY_THROW( sInitPlan->mOrReplace == STL_TRUE, RAMP_ERR_SAME_NAME_TABLE_EXISTS );
        
        /**
         * OR REPLACE option 인 경우
         */
        
        STL_TRY_THROW( ellGetTableType( & sTableHandle ) == ELL_TABLE_TYPE_VIEW,
                       RAMP_ERR_SAME_NAME_TABLE_EXISTS );
        
        /**
         * 동일한 View 가 존재함
         */
        
        sInitPlan->mOldViewExist = STL_TRUE;
        stlMemcpy( & sInitPlan->mOldViewHandle, & sTableHandle, STL_SIZEOF( ellDictHandle ) );

        /**
         * DROP VIEW 권한 검사
         */

        STL_TRY( qlaCheckPrivDropView( aTransID,
                                       aDBCStmt,
                                       aSQLStmt,
                                       & sInitPlan->mOldViewHandle,
                                       aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sInitPlan->mOldViewExist = STL_FALSE;
    }

    /**
     * 같은 이름을 가진 Sequence 가 있는지 확인
     * - Oracle 호환성을 위해 Name Space 를 Table(view, base table) 과 Sequence 를 같이 사용한다.
     * - 호환성을 위해 Dictionary 체계를 바꾸는 것보다 QP 단에서 검사한다.
     */

    STL_TRY( ellGetSequenceDictHandleWithSchema( aTransID,
                                                 aSQLStmt->mViewSCN,
                                                 & sInitPlan->mSchemaHandle,
                                                 sObjectName->mObject,
                                                 & sTableHandle,
                                                 & sObjectExist,
                                                 ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sObjectExist != STL_TRUE, RAMP_ERR_SAME_NAME_SEQUENCE_EXISTS );
    
    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                stlStrlen( sObjectName->mObject ) + 1,
                                (void **) & sInitPlan->mViewName,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlStrcpy( sInitPlan->mViewName, sObjectName->mObject );

    /**********************************************************
     * View Column List Validation
     **********************************************************/

    /**
     * View Column String 저장
     */

    sStrLen = stlStrlen(sParseTree->mViewColumnString);

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                sStrLen + 1,
                                (void **) & sInitPlan->mViewColumnString,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlStrcpy( sInitPlan->mViewColumnString, sParseTree->mViewColumnString);
    stlTrimString( sInitPlan->mViewColumnString );

   
    /**********************************************************
     * View Query Validation
     **********************************************************/

    /**
     * View 의 origin query expression 을 저장
     */

    sStrLen = stlStrlen(sParseTree->mViewQueryString);

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                sStrLen + 1,
                                (void **) & sInitPlan->mViewQueryString,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlStrcpy( sInitPlan->mViewQueryString, sParseTree->mViewQueryString);
    stlTrimString( sInitPlan->mViewQueryString );

    /**
     * Cycle Detection 을 위한 View Handle 등록
     */

    if ( sInitPlan->mOldViewExist == STL_TRUE )
    {
        STL_TRY( ellAddNewObjectItem( aSQLStmt->mViewCycleList,
                                      & sInitPlan->mOldViewHandle,
                                      & sHasCycle,
                                      & aDBCStmt->mShareMemStmt,
                                      ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        STL_DASSERT( sHasCycle == STL_FALSE );
    }
    else
    {
        /**
         * nothing to do
         */
    }
    
    /**
     * Origin Query Validation
     */

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
    sStmtInfo.mSQLString     = aSQLString;
    sStmtInfo.mQueryExprList = sQueryExprList;

    /**
     * Origin Query Validation
     */

    sStatus = qlvQueryNodeValidation( & sStmtInfo,
                                      STL_FALSE,
                                      sInitPlan->mCommonInit.mValidationObjList,
                                      NULL,
                                      (qllNode *) sParseTree->mViewQuery,
                                      & sInitPlan->mInitQueryNode,
                                      aEnv );
    
    if ( sStatus == STL_SUCCESS )
    {
        /**
         * compile 성공
         */

        sInitPlan->mInitCompiled           = STL_TRUE;

    }
    else
    {
        /**
         * compile 실패 
         */

        sInitPlan->mInitCompiled           = STL_FALSE;
        sInitPlan->mInitQueryNode          = NULL;
        sInitPlan->mUnderlyingRelationList = NULL;

        /**
         * FORCE 옵션이 있다면 그대로 종료
         */
        
        STL_TRY( sInitPlan->mIsForce == STL_TRUE );
        STL_THROW( RAMP_FINISH );
    }

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
                                STL_SIZEOF(stlChar *) * sInitPlan->mColumnCount,
                                (void **) & sInitPlan->mColumnName,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan->mColumnName, 0x00, STL_SIZEOF(stlChar *) * sInitPlan->mColumnCount );
    
    STL_TRY( qlrValidateUserColList( aDBCStmt,
                                     aSQLString,
                                     NULL, 
                                     sParseTree->mColumnList,
                                     sTargetCount,
                                     sTargetList,
                                     sInitPlan->mColumnName,
                                     sInitPlan->mIsForce,
                                     & sInitPlan->mInitCompiled,
                                     STL_TRUE, /* Check Sequence */ 
                                     aEnv )
             == STL_SUCCESS );

    sInitPlan->mUnderlyingRelationList = NULL;

    if ( sInitPlan->mInitCompiled == STL_FALSE )
    {
        sInitPlan->mInitQueryNode = NULL;
        STL_THROW( RAMP_FINISH );
    }
    else
    {
        /**
         * nothing to do
         */
    }
    
    /**
     * View Cycle List 에서 제거
     */

    if ( sInitPlan->mOldViewExist == STL_TRUE )
    {
        ellDeleteObjectItem( aSQLStmt->mViewCycleList,
                             & sInitPlan->mOldViewHandle,
                             & sDeleted );
        
        STL_DASSERT( sDeleted == STL_TRUE );
    }
    else
    {
        /**
         * nothing to do
         */
    }

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

    STL_RAMP( RAMP_FINISH );
    
    STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                  & sInitPlan->mSchemaHandle,
                                  QLL_INIT_PLAN(aDBCStmt),
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

    if ( sInitPlan->mOldViewExist == STL_TRUE )
    {
        STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                      & sInitPlan->mOldViewHandle,
                                      QLL_INIT_PLAN(aDBCStmt),
                                      ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        /* nothing to do */
    }
    
    /***********************************************************
     * Init Plan 연결 
     ***********************************************************/
    
    /**
     * Init Plan 연결 
     */

    aSQLStmt->mInitPlan = (void *) sInitPlan;

    if ( sInitPlan->mInitCompiled == STL_FALSE )
    {
        STL_DASSERT( sInitPlan->mIsForce == STL_TRUE );
        
        stlPushError( STL_ERROR_LEVEL_WARNING,
                      QLL_ERRCODE_WARNING_VIEW_DEFINITION_HAS_COMPILATION_ERRORS,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SCHEMA_NOT_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           sObjectName->mSchemaPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sObjectName->mSchema );
    }

    STL_CATCH( RAMP_ERR_SCHEMA_NOT_WRITABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_NOT_WRITABLE,
                      qlgMakeErrPosString( aSQLString,
                                           sObjectName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      ellGetSchemaName( & sInitPlan->mSchemaHandle ) );
    }

    STL_CATCH( RAMP_ERR_SAME_NAME_TABLE_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_USED_OBJECT_NAME,
                      qlgMakeErrPosString( aSQLString,
                                           sObjectName->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      ellGetSchemaName( & sInitPlan->mSchemaHandle ),
                      sObjectName->mObject );
    }
    
    STL_CATCH( RAMP_ERR_SAME_NAME_SEQUENCE_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_USED_OBJECT_NAME,
                      qlgMakeErrPosString( aSQLString,
                                           sObjectName->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      ellGetSchemaName( & sInitPlan->mSchemaHandle ),
                      sObjectName->mObject );
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
 * @brief CREATE VIEW 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrOptCodeCreateView( smlTransId      aTransID,
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
 * @brief CREATE VIEW 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrOptDataCreateView( smlTransId      aTransID,
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
 * @brief CREATE VIEW 구문을 Execute 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aStmt      Statement
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrExecuteCreateView( smlTransId      aTransID,
                                smlStatement  * aStmt,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv )
{
    ellDictHandle * sAuthHandle = NULL;
    
    qlrInitCreateView * sInitPlan = NULL;

    stlTime sTime = 0;
    stlBool   sLocked = STL_TRUE;

    ellObjectList * sViewColumnList = NULL;
    ellObjectList * sAffectedViewList = NULL;

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
    
    stlInt32  i = 0;

    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_CREATE_VIEW_TYPE,
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

    sInitPlan = (qlrInitCreateView *) aSQLStmt->mInitPlan;
    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );

    /**
     * CREATE VIEW 구문을 위한 DDL Lock 획득
     */

    if ( sInitPlan->mOldViewExist == STL_TRUE )
    {
        /**
         * OR REPLACE 옵션이 존재하고, 해당 VIEW 가 존재하는 경우
         */

        STL_TRY( ellLock4DropView( aTransID,
                                   aStmt,
                                   & sInitPlan->mOldViewHandle,
                                   & sLocked,
                                   ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    }
    else
    {
        /**
         * 해당 VIEW 가 존재하지 않는 경우
         */
        
        STL_TRY( ellLock4CreateView( aTransID,
                                     aStmt,
                                     sAuthHandle,
                                     & sInitPlan->mSchemaHandle,
                                     & sLocked,
                                     ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    }

    /**
     * Underlying Relation 에 대해 IS lock 획득
     */

    if ( sInitPlan->mInitCompiled == STL_TRUE )
    {
        STL_DASSERT( sInitPlan->mUnderlyingRelationList != NULL );

        STL_TRY( ellLockUnderlyingTableList4ViewDDL( aTransID,
                                                     aStmt,
                                                     SML_LOCK_IS,
                                                     sInitPlan->mUnderlyingRelationList,
                                                     & sLocked,
                                                     ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    }
    else
    {
        /**
         * Compile Error 인 VIEW 인 경우
         * - nothing to do 
         */
    }

    /**
     * Valid Plan 을 다시 획득
     */
    
    STL_TRY( qlgGetValidPlan4DDL( aTransID,
                                  aDBCStmt,
                                  aSQLStmt,
                                  QLL_PHASE_EXECUTE,
                                  aEnv )
             == STL_SUCCESS );

    sInitPlan = (qlrInitCreateView *) aSQLStmt->mInitPlan;
    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );

    /******************************************************************
     * 관련 객체에 대한 정보 및 Lock 획득 
     ******************************************************************/
    
    if ( sInitPlan->mOldViewExist == STL_TRUE )
    {
        /**
         * OR REPLACE 옵션이 존재하고, 해당 VIEW 가 존재하는 경우
         */
        
        /**
         * View Column List 획득
         */
        
        STL_TRY( ellGetViewColumnList( aTransID,
                                       aStmt,
                                       & sInitPlan->mOldViewHandle,
                                       & QLL_EXEC_DDL_MEM(aEnv),
                                       & sViewColumnList,
                                       ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        /**
         * Affetcted View List 획득
         */

        STL_TRY( ellGetAffectedViewListByRelation( aTransID,
                                                   aStmt,
                                                   & sInitPlan->mOldViewHandle,
                                                   & QLL_EXEC_DDL_MEM(aEnv),
                                                   & sAffectedViewList,
                                                   ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        /**
         * Affected View List 에 대해 X lock 획득
         */
        
        if ( sAffectedViewList != NULL )
        {
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
                                                       & sInitPlan->mOldViewHandle,
                                                       & QLL_EXEC_DDL_MEM(aEnv),
                                                       & sAffectedViewList,
                                                       ELL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
        else
        {
            /* nothing to do */
        }
    }
    else
    {
        /**
         * 해당 VIEW 가 존재하지 않는 경우
         */

        sViewColumnList   = NULL;     
        sAffectedViewList = NULL;
    }

    /******************************************************************
     * Dictionary Record 정보 설정
     ******************************************************************/

    /**
     * View 상위 객체 정보 획득
     */

    sViewOwnerID  = ellGetAuthID( sAuthHandle );
    sViewSchemaID = ellGetSchemaID( & sInitPlan->mSchemaHandle );

    /**
     * View 의 TABLES Descriptor 추가
     */

    if ( sInitPlan->mOldViewExist == STL_TRUE )
    {
        /**
         * 이미 View 가 존재하는 경우
         */

        sViewID = ellGetTableID( & sInitPlan->mOldViewHandle );
    }
    else
    {
        /**
         * 새로운 View 를 생성하는 경우
         */

        STL_TRY( ellInsertTableDesc( aTransID,
                                     aStmt,
                                     sViewOwnerID,
                                     sViewSchemaID,
                                     & sViewID,
                                     ELL_DICT_TABLESPACE_ID_NA,   /* aTablespaceID */
                                     ELL_DICT_OBJECT_ID_NA,       /* aPhysicalID */
                                     sInitPlan->mViewName,
                                     ELL_TABLE_TYPE_VIEW,
                                     NULL,                    /* aTableComment */
                                     ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    /**
     * 기존 View 관련 정보 제거
     */

    if ( sInitPlan->mOldViewExist == STL_TRUE )
    {
        /**
         * 기존 View 가 존재하는 경우
         */

        STL_TRY( ellRemoveDict4ReplaceView( aTransID,
                                            aStmt,
                                            & sInitPlan->mOldViewHandle,
                                            ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        /**
         * 새로운 View 를 생성하는 경우
         * nothing to do
         */
    }

    /**
     * COLUMNS descriptor 추가
     */

    if ( sInitPlan->mInitCompiled == STL_TRUE )
    {
        /**
         * 컬럼 Descriptor 정보를 추가
         */

        /**
         * 공간 할당
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
    }
    else
    {
        /**
         * COMPILE 이 실패한 경우
         * - nothing to do 
         */
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
                                sInitPlan->mInitCompiled,
                                (sInitPlan->mInitCompiled == STL_TRUE) ? STL_FALSE : STL_TRUE, /* aIsAffected */
                                ELL_ENV(aEnv) )
             == STL_SUCCESS );

    if ( sInitPlan->mInitCompiled == STL_TRUE )
    {
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
    }
    else
    {
        /**
         * FORCE 옵션이 사용되고 Compile 이 실패한 경우
         * - nothing to do
         */
    }

    /**
     * Affected View 가 영향을 받았음을 설정
     */

    if ( sAffectedViewList != NULL )
    {
        STL_TRY( ellSetAffectedViewList( aTransID,
                                         aStmt,
                                         sAffectedViewList,
                                         ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        /**
         * nothing to do
         */
    }
    
    /******************************************************************
     * Dictionary Cache 재구성 
     ******************************************************************/

    if ( sInitPlan->mOldViewExist == STL_TRUE )
    {
        /**
         * 기존 View 의 Column Cache 제거
         * - View 의 Table Cache 와 Column Cache 간에는 연결 관계가 없음
         */
        
        STL_TRY( ellRefineCache4RemoveViewColumns( aTransID,
                                                   aStmt,
                                                   sViewColumnList,
                                                   ELL_ENV(aEnv) )
             == STL_SUCCESS );
        
        /**
         * OR REPLACE 를 위한 Cache Refine
         */

        STL_TRY( ellRefineCache4ReplaceView( aTransID,
                                             aStmt,
                                             & sInitPlan->mOldViewHandle,
                                             ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        /**
         * CREATE VIEW 를 위한 Cache Refine
         */
        
        STL_TRY( ellRefineCache4CreateView( aTransID,
                                            aStmt,
                                            sViewID,
                                            ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    /**
     * Compile 을 실패했으나, FORCE 옵션을 통해 생성한 경우
     * - Warning 정보를 설정한다.
     */
    
    if ( sInitPlan->mInitCompiled == STL_FALSE )
    {
        STL_DASSERT( sInitPlan->mIsForce == STL_TRUE );
    }
    else
    {
        /* nothing to do */
    }

    /**
     * 생성한 객체에 대해 Lock 을 획득
     * 
     * commit -> pending -> trans end (unlock) 과정 중에
     * 해당 객체에 대한 DROP, ALTER 가 수행될 경우를 방지하기 위해
     * 생성한 객체에 대한 lock 을 획득해야 함.
     */

    STL_TRY( ellLockRowAfterCreateNonBaseTableObject( aTransID,
                                                      aStmt,
                                                      ELL_OBJECT_TABLE,
                                                      sViewID,
                                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    /* STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED ) */
    /* { */
    /*     stlPushError( STL_ERROR_LEVEL_ABORT, */
    /*                   QLL_ERRCODE_NOT_IMPLEMENTED, */
    /*                   NULL, */
    /*                   QLL_ERROR_STACK(aEnv), */
    /*                   "qlrExecuteCreateView()" ); */
    /* } */
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Non-Service 단계에서의 Performance View 의 CREATE VIEW 구문을 Execute 한다.
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 * OPEN 단계가 아니므로 Dictionary Table 에 추가하지 않고 Dictionary Cache 에만 추가한다.
 */
stlStatus qlrExecuteCreateNonServicePerfView( qllDBCStmt    * aDBCStmt,
                                              qllStatement  * aSQLStmt,
                                              qllEnv        * aEnv )
{
    ellDictHandle * sAuthHandle = NULL;
    
    qlrInitCreateView * sInitPlan = NULL;

    stlInt64   sViewOwnerID  = ELL_DICT_OBJECT_ID_NA;
    stlInt64   sViewSchemaID = ELL_DICT_OBJECT_ID_NA;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_CREATE_VIEW_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mInitPlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( knlGetCurrStartupPhase() != KNL_STARTUP_PHASE_OPEN, QLL_ERROR_STACK(aEnv) );
    
    /**
     * Valid Plan 획득
     */

    sInitPlan = (qlrInitCreateView *) aSQLStmt->mInitPlan;
    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );

    /**
     * Performance View 의 Dictionary Cache 정보 구축
     */

    sViewOwnerID  = ellGetAuthID( sAuthHandle );
    sViewSchemaID = ellGetSchemaID( & sInitPlan->mSchemaHandle );

    STL_TRY( ellAddNonServicePerfViewCache( sViewOwnerID,
                                            sViewSchemaID,
                                            sInitPlan->mViewName,
                                            sInitPlan->mViewColumnString,
                                            sInitPlan->mViewQueryString,
                                            ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Validate User-Specified Column List
 * @param[in]  aDBCStmt         DBC Statement
 * @param[in]  aSQLString       SQL String
 * @param[in]  aUserColString   User-Specified Column List String
 * @param[in]  aUserColList     User-Specified Column List
 * @param[in]  aTargetCount     Subquery Target Count 
 * @param[in]  aTargetList      Subquery Target List
 * @param[in,out]  aColumnName  Column Name Pointer Array
 * @param[in]  aIsForce         Is Force ( For Only Create View )
 * @param[in]  aInitCompiled    Init Compiled ( For Only Create View )
 * @param[in]  aCheckSequence   Check Sequence 
 * @param[in]  aEnv             Environment
 * @remarks CREATE VIEW  viewName (colname, ... ) AS SELECT target list,
 *          CREATE TABLE viewName (colname, ... ) AS SELECT target list
 *          위 두 구문의 column name list와 target list를 validation
 *          column name list가 명시되지 않은 경우,
 *          target list로 column name list를 생성한다. 
 */
stlStatus qlrValidateUserColList( qllDBCStmt        * aDBCStmt,
                                  stlChar           * aSQLString,
                                  stlChar           * aUserColString,
                                  qlpList           * aUserColList,
                                  stlInt32            aTargetCount,
                                  qlvInitTarget     * aTargetList,
                                  stlChar          ** aColumnName,
                                  stlBool             aIsForce,
                                  stlBool           * aInitCompiled,
                                  stlBool             aCheckSequence, 
                                  qllEnv            * aEnv )
{
    qlpListElement  * sListElement = NULL;
    qlpValue        * sColNode = NULL;
    stlChar         * sColName = NULL;
    qlpListElement  * sListElementCmp = NULL;
    qlpValue        * sColNodeCmp = NULL;
    stlChar         * sColNameCmp = NULL;
    stlInt32          sColNameLen = 0;
    stlInt32          sUserColNameCount = 0;
    qlvInitTarget   * sTargetItem = NULL;
    stlStatus         sStatus;
    stlInt32          sErrNodePos = 0;

    stlInt32  i = 0;
    stlInt32  j = 0;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );

    /****************************************************************
     * User Defined Column List에 동일한 Column Name 이 존재하는지 검사
     ****************************************************************/

    if ( aUserColList != NULL )
    {
        for ( sListElement = QLP_LIST_GET_FIRST( aUserColList );
              sListElement != NULL;
              sListElement = QLP_LIST_GET_NEXT_ELEM( sListElement ) )
        {
            sColNode    = (qlpValue *) QLP_LIST_GET_POINTER_VALUE( sListElement );
            sColName    = QLP_GET_PTR_VALUE( sColNode );
            sErrNodePos = sColNode->mNodePos;
            
            for ( sListElementCmp = QLP_LIST_GET_FIRST( aUserColList );
                  sListElementCmp != sListElement;
                  sListElementCmp = QLP_LIST_GET_NEXT_ELEM( sListElementCmp ) )
            {
                sColNodeCmp = (qlpValue *) QLP_LIST_GET_POINTER_VALUE( sListElementCmp );
                sColNameCmp = QLP_GET_PTR_VALUE( sColNodeCmp );
                
                STL_TRY_THROW( stlStrcmp( sColName, sColNameCmp ) != 0,
                               RAMP_ERR_SAME_COLUMN_EXIST );
            }
        }
    }
    else
    {
        /**
         * User Specied Column Name List가 없음
         */ 
    }

    /****************************************************************
     * Target 에 View 에서 사용할 수 없는 Expression (SEQUENCE) 이 존재하는지 확인
     * 단, Create Table As Select에는 sequence 존재 가능
     ****************************************************************/

    if ( aCheckSequence == STL_TRUE )
    {
        for ( i = 0; i < aTargetCount; i++ )
        {
            sTargetItem = & aTargetList[i];
            
            sStatus = qlvValidateSequenceInTarget( aSQLString,
                                                   sTargetItem->mExpr,
                                                   aEnv );
            
            if ( sStatus == STL_SUCCESS )
            {
                /* 존재하지 않음 */
            }
            else
            {
                /**
                 * compile 실패 
                 */
                
                *aInitCompiled = STL_FALSE;
                
                /**
                 * FORCE 옵션이 있다면 그대로 종료
                 */
                
                STL_TRY( aIsForce == STL_TRUE );
                STL_THROW( RAMP_FINISH );
            }
        }
    }
    else
    {
        /**
         * CREATE TABLE AS SELECT의 target에는 sequence 존재 가능 
         */ 
    }

    /****************************************************************
     * Column Name List 구축 
     ****************************************************************/
    
    if ( aUserColList != NULL )
    {
        /**
         * User Defined Column List가 명시된 경우,
         * Subquery의 Target 개수와 동일해야 함
         */

        sUserColNameCount = QLP_LIST_GET_COUNT( aUserColList );
        
        if ( aTargetCount == sUserColNameCount )
        {
            /**
             * 유효한 Query 임
             */

             /**
             * Column Name 저장
             */

            i = 0;
            QLP_LIST_FOR_EACH( aUserColList, sListElement )
            {
                sColNode = (qlpValue *) QLP_LIST_GET_POINTER_VALUE( sListElement );
                sColName    = QLP_GET_PTR_VALUE( sColNode );
                sColNameLen = stlStrlen( sColName );
                sErrNodePos = sColNode->mNodePos;
                
                STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                            sColNameLen + 1,
                                            (void **) & aColumnName[i],
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );
                stlStrcpy( aColumnName[i], sColName );

                i++;
            }
        }
        else
        {
            /**
             * 유효하지 않은 Query 임
             */

            /**
             * compile 실패 
             */
            
            *aInitCompiled = STL_FALSE;
            
            /**
             * FORCE 옵션이 있다면 그대로 종료
             */

            sColNode = QLP_LIST_GET_POINTER_VALUE( QLP_LIST_GET_FIRST( aUserColList ) );
            sErrNodePos = sColNode->mNodePos;
            
            STL_TRY_THROW( aIsForce == STL_TRUE, RAMP_ERR_MISMATCH_DEGREE );
            STL_THROW( RAMP_FINISH );
        }
    }
    else
    {
        /**
         * User Defined Column List가 명시되지 않은 경우, 
         * Target은 이름이 식별 가능하고, 중복되지 않아야 함
         */
        
        for( i = 0; i < aTargetCount; i++ )
        {
            sTargetItem = & aTargetList[i];

            /**
             * 명시적인 Alias Name 이거나, Column Name 이어야 함
             */

            if ( sTargetItem->mAliasName != NULL )
            {
                /**
                 * 명시적인 Alias Name 
                 */
                sColName = sTargetItem->mAliasName;
            }
            else
            {
                if ( sTargetItem->mExpr->mColumnName != NULL )
                {
                    /**
                     * Column Name 
                     */

                    sColName = sTargetItem->mExpr->mColumnName;
                }
                else
                {
                    /**
                     * FORCE 옵션이 존재한다면 그대로 종료
                     */
                    
                   *aInitCompiled = STL_FALSE;
                   sErrNodePos    = sTargetItem->mDisplayPos;
                    
                    STL_TRY_THROW( aIsForce == STL_TRUE, RAMP_ERR_NEED_ALIAS_NAME );
                    STL_THROW( RAMP_FINISH );
                }
            }
            
            sColNameLen = stlStrlen( sColName );

            /**
             * 동일한 Column Name 이 존재하는지 검사
             */

            for ( j = 0; j < i; j++ )
            {
                if ( stlStrcmp( sColName, aColumnName[j] ) == 0 )
                {
                    /**
                     * FORCE 옵션이 존재한다면 그대로 종료
                     */
                    
                    *aInitCompiled = STL_FALSE;
                    sErrNodePos    = sTargetItem->mDisplayPos;
                    
                    STL_TRY_THROW( aIsForce == STL_TRUE, RAMP_ERR_SAME_COLUMN_EXIST );
                    STL_THROW( RAMP_FINISH );
                }
                else
                {
                    /* 서로 다른 Column Name 임 */
                }
            }

            /**
             * Column Name 저장
             */
            
            STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                        sColNameLen + 1,
                                        (void **) & aColumnName[i],
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            stlStrcpy( aColumnName[i], sColName );
        }
    }
   
    
    STL_RAMP( RAMP_FINISH );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_MISMATCH_DEGREE )
    {
        if ( aUserColString == NULL )
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          QLL_ERRCODE_INVALID_NUMBER_OF_COLUMN_NAMES_SPECIFIED,
                          qlgMakeErrPosString( aSQLString,
                                               sErrNodePos,
                                               aEnv ),
                          QLL_ERROR_STACK(aEnv) );
        }
        else
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          QLL_ERRCODE_INVALID_NUMBER_OF_COLUMN_NAMES_SPECIFIED,
                          qlgMakeErrPosString( aUserColString,
                                               sErrNodePos,
                                               aEnv ),
                          QLL_ERROR_STACK(aEnv) );
        }
    }

    STL_CATCH( RAMP_ERR_NEED_ALIAS_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_MUST_NAME_THIS_EXPRESSION_WTIH_COLUMN_ALIAS,
                      qlgMakeErrPosString( aSQLString,
                                           sErrNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_SAME_COLUMN_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_DUPLICATE_COLUMN_NAME,
                      qlgMakeErrPosString( aSQLString,
                                           sErrNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Target 정보로부터 Column 정보를 구성한다.
 * @param[in,out] aColumnInfo        Column Information
 * @param[in]     aOrdinalPosition   Column's Ordinal Position
 * @param[in]     aColumnName        Column Name
 * @param[in]     aTargetItem        a Target Information
 */
void qlrSetColumnInfoFromTarget( qlrInitColumn * aColumnInfo,
                                 stlInt32        aOrdinalPosition,
                                 stlChar       * aColumnName,
                                 qllTarget     * aTargetItem )
{
    STL_DASSERT( aColumnInfo != NULL );
    STL_DASSERT( aColumnName != NULL );
    STL_DASSERT( aTargetItem != NULL );

    /**
     * Code Column 정보 구성
     */
    
    aColumnInfo->mColumnName      = aColumnName;
    aColumnInfo->mColumnDefault   = NULL;
    aColumnInfo->mNullDefault     = STL_FALSE;
    aColumnInfo->mPhysicalOrdinalPosition = aOrdinalPosition;
    aColumnInfo->mLogicalOrdinalPosition  = aOrdinalPosition;
    aColumnInfo->mIsNullable      = aTargetItem->mIsNullable;
    aColumnInfo->mIsIdentity      = STL_FALSE;
    aColumnInfo->mIdentityOption  = ELL_IDENTITY_GENERATION_NA;
    /* aColumnInfo->mIdentityAttr; */
    
    aColumnInfo->mTypeDef.mDBType       = aTargetItem->mDBType;
    aColumnInfo->mTypeDef.mUserTypeName = aTargetItem->mTypeName;

    switch ( aTargetItem->mDBType )
    {
        case DTL_TYPE_BOOLEAN:
            {
                aColumnInfo->mTypeDef.mArgNum1           = DTL_PRECISION_NA;
                aColumnInfo->mTypeDef.mArgNum2           = DTL_SCALE_NA;
                aColumnInfo->mTypeDef.mStringLengthUnit  = DTL_STRING_LENGTH_UNIT_NA;
                aColumnInfo->mTypeDef.mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                aColumnInfo->mTypeDef.mNumericRadix      = DTL_NUMERIC_PREC_RADIX_NA;
                break;
            }
        case DTL_TYPE_NATIVE_SMALLINT:
        case DTL_TYPE_NATIVE_INTEGER:
        case DTL_TYPE_NATIVE_BIGINT:
        case DTL_TYPE_NATIVE_REAL:
        case DTL_TYPE_NATIVE_DOUBLE:
        case DTL_TYPE_FLOAT:
        case DTL_TYPE_NUMBER:
        case DTL_TYPE_DECIMAL:
            {
                aColumnInfo->mTypeDef.mArgNum1           = aTargetItem->mPrecision;
                aColumnInfo->mTypeDef.mArgNum2           = aTargetItem->mScale;
                aColumnInfo->mTypeDef.mStringLengthUnit  = DTL_STRING_LENGTH_UNIT_NA;
                aColumnInfo->mTypeDef.mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                aColumnInfo->mTypeDef.mNumericRadix      = aTargetItem->mNumPrecRadix;
                break;
            }
        case DTL_TYPE_CHAR:
        case DTL_TYPE_VARCHAR:
        case DTL_TYPE_LONGVARCHAR:
        case DTL_TYPE_CLOB:
            {
                switch (aTargetItem->mStringLengthUnit)
                {
                    case DTL_STRING_LENGTH_UNIT_CHARACTERS:
                        aColumnInfo->mTypeDef.mArgNum1 = aTargetItem->mCharacterLength;
                        break;
                    case DTL_STRING_LENGTH_UNIT_OCTETS:
                        aColumnInfo->mTypeDef.mArgNum1 = aTargetItem->mOctetLength;
                        break;
                    default:
                        STL_DASSERT(0);
                        break;
                }
                
                aColumnInfo->mTypeDef.mArgNum2           = DTL_SCALE_NA;
                aColumnInfo->mTypeDef.mStringLengthUnit  = aTargetItem->mStringLengthUnit;
                aColumnInfo->mTypeDef.mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                aColumnInfo->mTypeDef.mNumericRadix      = DTL_NUMERIC_PREC_RADIX_NA;
                break;
            }
        case DTL_TYPE_BINARY:
        case DTL_TYPE_VARBINARY:
        case DTL_TYPE_LONGVARBINARY:
        case DTL_TYPE_BLOB:
            {
                aColumnInfo->mTypeDef.mArgNum1           = aTargetItem->mOctetLength;
                aColumnInfo->mTypeDef.mArgNum2           = DTL_SCALE_NA;
                aColumnInfo->mTypeDef.mStringLengthUnit  = DTL_STRING_LENGTH_UNIT_OCTETS;
                aColumnInfo->mTypeDef.mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                aColumnInfo->mTypeDef.mNumericRadix      = DTL_NUMERIC_PREC_RADIX_NA;
                break;
            }
        case DTL_TYPE_DATE:
            {
                aColumnInfo->mTypeDef.mArgNum1           = DTL_PRECISION_NA;
                aColumnInfo->mTypeDef.mArgNum2           = DTL_SCALE_NA;
                aColumnInfo->mTypeDef.mStringLengthUnit  = DTL_STRING_LENGTH_UNIT_NA;
                aColumnInfo->mTypeDef.mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                aColumnInfo->mTypeDef.mNumericRadix      = DTL_NUMERIC_PREC_RADIX_NA;
                break;
            }
        case DTL_TYPE_TIME:
        case DTL_TYPE_TIMESTAMP:
        case DTL_TYPE_TIME_WITH_TIME_ZONE:
        case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:
            {
                aColumnInfo->mTypeDef.mArgNum1           = aTargetItem->mPrecision;
                aColumnInfo->mTypeDef.mArgNum2           = DTL_SCALE_NA;
                aColumnInfo->mTypeDef.mStringLengthUnit  = DTL_STRING_LENGTH_UNIT_NA;
                aColumnInfo->mTypeDef.mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                aColumnInfo->mTypeDef.mNumericRadix      = DTL_NUMERIC_PREC_RADIX_NA;
                break;
            }
                    
        case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:
        case DTL_TYPE_INTERVAL_DAY_TO_SECOND:
            {
                aColumnInfo->mTypeDef.mArgNum1           = aTargetItem->mDateTimeIntervalPrec;
                aColumnInfo->mTypeDef.mArgNum2           = aTargetItem->mPrecision;
                aColumnInfo->mTypeDef.mStringLengthUnit  = DTL_STRING_LENGTH_UNIT_NA;
                aColumnInfo->mTypeDef.mIntervalIndicator = aTargetItem->mIntervalCode;
                aColumnInfo->mTypeDef.mNumericRadix      = DTL_NUMERIC_PREC_RADIX_NA;
                break;
            }
        case DTL_TYPE_ROWID:
            {
                aColumnInfo->mTypeDef.mArgNum1           = DTL_PRECISION_NA;
                aColumnInfo->mTypeDef.mArgNum2           = DTL_SCALE_NA;
                aColumnInfo->mTypeDef.mStringLengthUnit  = DTL_STRING_LENGTH_UNIT_NA;
                aColumnInfo->mTypeDef.mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                aColumnInfo->mTypeDef.mNumericRadix      = DTL_NUMERIC_PREC_RADIX_NA;
                break;
            }
        default:
            STL_DASSERT(0);
            break;
    }
}


/** @} qlrCreateView */
