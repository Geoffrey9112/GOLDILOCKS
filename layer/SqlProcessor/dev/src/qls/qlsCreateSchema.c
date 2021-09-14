/*******************************************************************************
 * qlsCreateSchema.c
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
 * @file qlsCreateSchema.c
 * @brief CREATE SCHEMA 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qlsCreateSchema CREATE SCHEMA
 * @ingroup qls
 * @{
 */


/**
 * @brief CREATE SCHEMA 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlsValidateCreateSchema( smlTransId      aTransID,
                                   qllDBCStmt    * aDBCStmt,
                                   qllStatement  * aSQLStmt,
                                   stlChar       * aSQLString,
                                   qllNode       * aParseTree,
                                   qllEnv        * aEnv )
{
    ellDictHandle * sAuthHandle = NULL;
    
    qlpSchemaDef        * sParseTree = NULL;
    qlsInitCreateSchema * sInitPlan = NULL;
    
    stlChar         * sSchemaName = NULL;
    stlInt32          sSchemaPos = 0;
    
    ellDictHandle     sObjectHandle;
    stlBool           sObjectExist = STL_FALSE;

    qlpListElement * sListElement = NULL;
    qllNode        * sSchemaElement = NULL;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_CREATE_SCHEMA_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_CREATE_SCHEMA_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */
    
    sParseTree = (qlpSchemaDef *) aParseTree;
    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlsInitCreateSchema),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlsInitCreateSchema) );

    STL_TRY( qlvSetInitPlan( aDBCStmt,
                             aSQLStmt,
                             & sInitPlan->mCommonInit,
                             aEnv )
             == STL_SUCCESS );
    
    /**********************************************************
     * Schema Validation
     **********************************************************/

    /**
     * Schema Name 정보 획득
     */

    if ( sParseTree->mSchemaName == NULL )
    {
        /**
         * CREATE SCHEMA AUTHORIZATION user 인 경우
         */

        sSchemaName = QLP_GET_PTR_VALUE(sParseTree->mOwnerName);
        sSchemaPos = sParseTree->mOwnerName->mNodePos;
            
    }
    else
    {
        /**
         * CREATE SCHEMA schema_name 인 경우
         */
        
        sSchemaName = QLP_GET_PTR_VALUE(sParseTree->mSchemaName);
        sSchemaPos = sParseTree->mSchemaName->mNodePos;
    }
    
    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                stlStrlen( sSchemaName ) + 1,
                                (void **) & sInitPlan->mSchemaName,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlStrcpy( sInitPlan->mSchemaName, sSchemaName );
    
    /**
     * Schema 존재 여부 확인
     */
    
    STL_TRY( ellGetSchemaDictHandle( aTransID,
                                     aSQLStmt->mViewSCN,
                                     sInitPlan->mSchemaName,
                                     & sObjectHandle,
                                     & sObjectExist,
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sObjectExist == STL_FALSE, RAMP_ERR_SAME_NAME_SCHEMA_EXISTS );

    /**
     * CREATE SCHEMA ON DATABASE 권한 검사
     */

    STL_TRY( qlaCheckDatabasePriv( aTransID,
                                   aDBCStmt,
                                   aSQLStmt,
                                   ELL_DB_PRIV_ACTION_CREATE_SCHEMA,
                                   aEnv )
             == STL_SUCCESS );
    
    /**********************************************************
     * Owner Validation
     **********************************************************/

    if ( sParseTree->mOwnerName == NULL )
    {
        /**
         * CREATE SCHEMA schema_name
         */
        
        stlMemcpy( & sInitPlan->mOwnerHandle, sAuthHandle, STL_SIZEOF(ellDictHandle) );
    }
    else
    {
        /**
         * CREATE SCHEMA .. AUTHORIZATION user_name
         */

        STL_TRY( ellGetAuthDictHandle( aTransID,
                                       aSQLStmt->mViewSCN,
                                       QLP_GET_PTR_VALUE(sParseTree->mOwnerName),
                                       & sInitPlan->mOwnerHandle,
                                       & sObjectExist,
                                       ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_USER_NOT_EXISTS );

        /**
         * USER 인지 검사, role ..
         */
        
        STL_TRY_THROW( ellGetAuthType( & sInitPlan->mOwnerHandle ) == ELL_AUTHORIZATION_TYPE_USER,
                       RAMP_ERR_USER_NOT_EXISTS );
    }

    /**********************************************************
     * Schema Element(DDL) Schema Name Matching 
     **********************************************************/

    sInitPlan->mSchemaElemList = sParseTree->mSchemaElemList;
    
    if ( sInitPlan->mSchemaElemList == NULL )
    {
        /**
         * Schema Element 가 없음
         */

        aSQLStmt->mKeepParseTree = STL_FALSE;
    }
    else
    {
        /**
         * schema element(DDL) 가 존재함.
         */

        aSQLStmt->mKeepParseTree = STL_TRUE;
        
        /**
         * Parse Tree 에서 구성된 Schema Object 의 Name Validation
         */

        STL_TRY( qlsMatchSchemaName( sInitPlan->mSchemaName,
                                     aSQLString,
                                     sParseTree->mRefSchemaObjectList,
                                     aEnv )
                 == STL_SUCCESS );
        
        /**
         * DDL 유형별 match Schema Name
         * - COMMENT, GRANT 구문등은 Column Name 에 대한 검사가 필요함.
         */
        
        QLP_LIST_FOR_EACH( sParseTree->mSchemaElemList, sListElement )
        {
            sSchemaElement = (qllNode *) QLP_LIST_GET_POINTER_VALUE( sListElement );

            switch ( sSchemaElement->mType )
            {
                case QLL_STMT_CREATE_TABLE_TYPE:
                case QLL_STMT_CREATE_VIEW_TYPE:
                case QLL_STMT_CREATE_INDEX_TYPE:
                case QLL_STMT_CREATE_SEQUENCE_TYPE:
                    {
                        /* Schema Object Name 에 대한 검사가 이미 이루어짐 */
                        break;
                    }
                case QLL_STMT_GRANT_DATABASE_TYPE:
                case QLL_STMT_GRANT_TABLESPACE_TYPE:
                case QLL_STMT_GRANT_SCHEMA_TYPE:
                case QLL_STMT_GRANT_TABLE_TYPE:
                case QLL_STMT_GRANT_USAGE_TYPE:
                    {
                        STL_TRY( qlsMatchSchemaNameGrantPriv( sInitPlan->mSchemaName,
                                                              aSQLString,
                                                              (qlpGrantPriv *) sSchemaElement,
                                                              aEnv )
                                 == STL_SUCCESS );
                        break;
                    }
                case QLL_STMT_COMMENT_ON_TYPE:
                    {
                        STL_TRY( qlsMatchSchemaNameCommentOn( sInitPlan->mSchemaName,
                                                              aSQLString,
                                                              (qlpCommentOnStmt *) sSchemaElement,
                                                              aEnv )
                                 == STL_SUCCESS );
                        break;
                    }
                default:
                    {
                        STL_ASSERT(0);
                        break;
                    }
            }
        }
    }

    /********************************************************
     * Validation Object 추가 
     ********************************************************/

    STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                  & sInitPlan->mOwnerHandle,
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

    STL_CATCH( RAMP_ERR_SAME_NAME_SCHEMA_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_USED_SCHEMA_NAME,
                      qlgMakeErrPosString( aSQLString,
                                           sSchemaPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sSchemaName );
    }

    STL_CATCH( RAMP_ERR_USER_NOT_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_USER_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mOwnerName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      QLP_GET_PTR_VALUE(sParseTree->mOwnerName) );
    }
    
    /* STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED ) */
    /* { */
    /*     stlPushError( STL_ERROR_LEVEL_ABORT, */
    /*                   QLL_ERRCODE_NOT_IMPLEMENTED, */
    /*                   NULL, */
    /*                   QLL_ERROR_STACK(aEnv), */
    /*                   "qlsValidateCreateSchema()" ); */
    /* } */
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief CREATE SCHEMA 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlsOptCodeCreateSchema( smlTransId      aTransID,
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
 * @brief CREATE SCHEMA 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlsOptDataCreateSchema( smlTransId      aTransID,
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
 * @brief CREATE SCHEMA 구문을 Execute 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aStmt      Statement
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlsExecuteCreateSchema( smlTransId      aTransID,
                                  smlStatement  * aStmt,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  qllEnv        * aEnv )
{
    stlInt32 sState = 0;
    
    qlsInitCreateSchema * sInitPlan = NULL;

    stlInt64              sSchemaID = ELL_DICT_OBJECT_ID_NA;
    
    stlTime               sTime = 0;

    stlBool   sLocked = STL_TRUE;
    
    qlpListElement * sListElement = NULL;
    qllNode        * sSchemaElement = NULL;

    /**
     * GRANT 의 중복 수행, COMMENT 등은 dictionary row 의 update 를 수반한다.
     * 따라서, schema element 실패시 Rollback 할 수 있도록 element statement 를 별도로 생성한다.
     * 최상위 Statement 는 어떠한 변경도 발생시키지 않아야 한다.
     */
    
    smlStatement  * sElementStmt = NULL;
    stlInt32        sElementStmtAttr = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_CREATE_SCHEMA_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mInitPlan != NULL, QLL_ERROR_STACK(aEnv) );

    /***************************************************************
     * 기본 정보 설정
     ***************************************************************/

    /**
     * schema DDL 을 위한 statement 생성
     */
    
    sElementStmtAttr = SML_STMT_ATTR_UPDATABLE | SML_STMT_ATTR_LOCKABLE; 
    
    STL_TRY( smlAllocStatement( aTransID,
                                NULL, /* aUpdateRelHandle */
                                sElementStmtAttr,
                                SML_LOCK_TIMEOUT_PROPERTY,
                                STL_FALSE, /* aNeedSnapshotIterator */
                                & sElementStmt,
                                SML_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 1;

    aSQLStmt->mViewSCN = SML_MAXIMUM_STABLE_SCN;
    
    /**
     * DDL 수행 시간 설정
     */

    sTime = stlNow();
    STL_TRY( ellBeginStmtDDL( sElementStmt, sTime, ELL_ENV(aEnv) ) == STL_SUCCESS );

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
    
    sInitPlan = (qlsInitCreateSchema *) aSQLStmt->mInitPlan;
    
    /**
     * CREATE SCHEMA 구문을 위한 DDL Lock 획득
     */
    
    STL_TRY( ellLock4CreateSchema( aTransID,
                                   sElementStmt,
                                   & sInitPlan->mOwnerHandle,
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

    sInitPlan = (qlsInitCreateSchema *) aSQLStmt->mInitPlan;

    /***************************************************************
     * Dictionary Record 추가 
     ***************************************************************/

    /**
     * Schema Descriptor 추가
     */

    STL_TRY( ellInsertSchemaDesc( aTransID,
                                  sElementStmt,
                                  ellGetAuthID( & sInitPlan->mOwnerHandle ),
                                  & sSchemaID,
                                  sInitPlan->mSchemaName,
                                  NULL,   /* aSchemaComment */
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**********************************************************
     * SQL-Schema Cache 구축 
     **********************************************************/
    
    /**
     * SQL-Schema Cache 추가
     */
    
    STL_TRY( ellRefineCache4AddSchema( aTransID,
                                       sElementStmt,
                                       ellGetAuthID( & sInitPlan->mOwnerHandle ),
                                       sSchemaID,
                                       ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * 생성한 객체에 대해 Lock 을 획득
     * 
     * commit -> pending -> trans end (unlock) 과정 중에
     * 해당 객체에 대한 DROP, ALTER 가 수행될 경우를 방지하기 위해
     * 생성한 객체에 대한 lock 을 획득해야 함.
     */

    STL_TRY( ellLockRowAfterCreateNonBaseTableObject( aTransID,
                                                      sElementStmt,
                                                      ELL_OBJECT_SCHEMA,
                                                      sSchemaID,
                                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    
    /***************************************************************
     * 전용 Statement 종료 
     ***************************************************************/
    
    /**
     * Element Statement 종료
     */

    sState = 0;
    STL_TRY( smlFreeStatement( sElementStmt, SML_ENV(aEnv) ) == STL_SUCCESS );
    sElementStmt = NULL;
    
    /**********************************************************
     * Schema Element 수행 
     **********************************************************/

    if ( sInitPlan->mSchemaElemList == NULL )
    {
        /* nothing to do */
    }
    else
    {
        /**
         * schema element (DDL) 을 수행
         */
        
        QLP_LIST_FOR_EACH( sInitPlan->mSchemaElemList, sListElement )
        {
            /**
             * schema element 별 statement 생성
             */

            sElementStmtAttr = SML_STMT_ATTR_UPDATABLE | SML_STMT_ATTR_LOCKABLE; 

            STL_TRY( smlAllocStatement( aTransID,
                                        NULL, /* aUpdateRelHandle */
                                        sElementStmtAttr,
                                        SML_LOCK_TIMEOUT_PROPERTY,
                                        STL_FALSE, /* aNeedSnapshotIterator */
                                        & sElementStmt,
                                        SML_ENV(aEnv) )
                     == STL_SUCCESS );
            sState = 1;
            
            aSQLStmt->mViewSCN = SML_MAXIMUM_STABLE_SCN;
            
            
            sSchemaElement = (qllNode *) QLP_LIST_GET_POINTER_VALUE( sListElement );

            aSQLStmt->mStmtType = sSchemaElement->mType;

            /**
             * Granted Privilege List 초기화
             */
            
            STL_TRY( ellInitPrivList( & aSQLStmt->mGrantedPrivList,
                                      STL_FALSE,  /* aForRevoke */
                                      & aDBCStmt->mShareMemStmt,
                                      ELL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            /**
             * Cycle Detector 초기화
             */
            
            STL_TRY( ellInitObjectList( & aSQLStmt->mViewCycleList,
                                        & aDBCStmt->mShareMemStmt,
                                        ELL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            /**
             * DDL 유형별 구문 수행
             */

            switch ( sSchemaElement->mType )
            {
                case QLL_STMT_CREATE_TABLE_TYPE:
                    {
                        /**
                         * validate CREATE TABLE
                         */

                        qlgSetQueryPhaseMemMgr( aDBCStmt, QLL_PHASE_VALIDATION, aEnv );
                        
                        STL_TRY( qlrValidateCreateTable( aTransID,
                                                         aDBCStmt,
                                                         aSQLStmt,
                                                         aSQLStmt->mRetrySQL,
                                                         sSchemaElement,
                                                         aEnv )
                                 == STL_SUCCESS );

                        /**
                         * code optimize CREATE TABLE
                         */

                        qlgSetQueryPhaseMemMgr( aDBCStmt, QLL_PHASE_CODE_OPTIMIZE, aEnv );
                        
                        STL_TRY( qlrOptCodeCreateTable( aTransID,
                                                        aDBCStmt,
                                                        aSQLStmt,
                                                        aEnv )
                                 == STL_SUCCESS );

                        /**
                         * data optimize CREATE TABLE
                         */

                        qlgSetQueryPhaseMemMgr( aDBCStmt, QLL_PHASE_DATA_OPTIMIZE, aEnv );
                        
                        STL_TRY( qlrOptDataCreateTable( aTransID,
                                                        aDBCStmt,
                                                        aSQLStmt,
                                                        aEnv )
                                 == STL_SUCCESS );

                        /**********************************************
                         * execute CREATE TABLE
                         **********************************************/

                        /**
                         * owner 를 current user 로 설정
                         */
                        
                        ellSetCurrentUser( & sInitPlan->mOwnerHandle, ELL_ENV(aEnv) );

                        /**
                         * execute CREATE TABLE
                         */
                        
                        qlgSetQueryPhaseMemMgr( aDBCStmt, QLL_PHASE_EXECUTE, aEnv );
                        
                        STL_TRY( qlrExecuteCreateTable( aTransID,
                                                        sElementStmt,
                                                        aDBCStmt,
                                                        aSQLStmt,
                                                        aEnv )
                                 == STL_SUCCESS );

                        /**
                         * Current User 를 Session User 로 원복 
                         */
                        
                        ellRollbackCurrentUser( ELL_ENV(aEnv) );
                        
                        break;
                    }
                case QLL_STMT_CREATE_VIEW_TYPE:
                    {
                        /**
                         * validate CREATE VIEW
                         */

                        qlgSetQueryPhaseMemMgr( aDBCStmt, QLL_PHASE_VALIDATION, aEnv );

                        STL_TRY( qlrValidateCreateView( aTransID,
                                                        aDBCStmt,
                                                        aSQLStmt,
                                                        aSQLStmt->mRetrySQL,
                                                        sSchemaElement,
                                                        aEnv )
                                 == STL_SUCCESS );

                        /**
                         * code optimize CREATE VIEW
                         */

                        qlgSetQueryPhaseMemMgr( aDBCStmt, QLL_PHASE_CODE_OPTIMIZE, aEnv );

                        STL_TRY( qlrOptCodeCreateView( aTransID,
                                                       aDBCStmt,
                                                       aSQLStmt,
                                                       aEnv )
                                 == STL_SUCCESS );

                        /**
                         * data optimize CREATE VIEW
                         */

                        qlgSetQueryPhaseMemMgr( aDBCStmt, QLL_PHASE_DATA_OPTIMIZE, aEnv );

                        STL_TRY( qlrOptDataCreateView( aTransID,
                                                       aDBCStmt,
                                                       aSQLStmt,
                                                       aEnv )
                                 == STL_SUCCESS );

                        /**********************************************
                         * execute CREATE VIEW
                         **********************************************/

                        /**
                         * owner 를 current user 로 설정
                         */
                        
                        ellSetCurrentUser( & sInitPlan->mOwnerHandle, ELL_ENV(aEnv) );

                        /**
                         * execute CREATE VIEW
                         */

                        qlgSetQueryPhaseMemMgr( aDBCStmt, QLL_PHASE_EXECUTE, aEnv );
                        
                        STL_TRY( qlrExecuteCreateView( aTransID,
                                                       sElementStmt,
                                                       aDBCStmt,
                                                       aSQLStmt,
                                                       aEnv )
                                 == STL_SUCCESS );

                        /**
                         * Current User 를 Session User 로 원복 
                         */
        
                        ellRollbackCurrentUser( ELL_ENV(aEnv) );
                        
                        break;
                    }
                case QLL_STMT_CREATE_INDEX_TYPE:
                    {
                        /**
                         * validate CREATE INDEX
                         */

                        qlgSetQueryPhaseMemMgr( aDBCStmt, QLL_PHASE_VALIDATION, aEnv );
                        
                        STL_TRY( qliValidateCreateIndex( aTransID,
                                                         aDBCStmt,
                                                         aSQLStmt,
                                                         aSQLStmt->mRetrySQL,
                                                         sSchemaElement,
                                                         aEnv )
                                 == STL_SUCCESS );

                        /**
                         * code optimize CREATE INDEX
                         */

                        qlgSetQueryPhaseMemMgr( aDBCStmt, QLL_PHASE_CODE_OPTIMIZE, aEnv );

                        STL_TRY( qliOptCodeCreateIndex( aTransID,
                                                        aDBCStmt,
                                                        aSQLStmt,
                                                        aEnv )
                                 == STL_SUCCESS );

                        /**
                         * data optimize CREATE INDEX
                         */

                        qlgSetQueryPhaseMemMgr( aDBCStmt, QLL_PHASE_DATA_OPTIMIZE, aEnv );

                        STL_TRY( qliOptDataCreateIndex( aTransID,
                                                        aDBCStmt,
                                                        aSQLStmt,
                                                        aEnv )
                                 == STL_SUCCESS );

                        
                        /**********************************************
                         * execute CREATE INDEX
                         **********************************************/

                        /**
                         * owner 를 current user 로 설정
                         */
                        
                        ellSetCurrentUser( & sInitPlan->mOwnerHandle, ELL_ENV(aEnv) );

                        /**
                         * execute CREATE INDEX
                         */

                        qlgSetQueryPhaseMemMgr( aDBCStmt, QLL_PHASE_EXECUTE, aEnv );
                        
                        STL_TRY( qliExecuteCreateIndex( aTransID,
                                                        sElementStmt,
                                                        aDBCStmt,
                                                        aSQLStmt,
                                                        aEnv )
                                 == STL_SUCCESS );

                        /**
                         * Current User 를 Session User 로 원복 
                         */
        
                        ellRollbackCurrentUser( ELL_ENV(aEnv) );
                        
                        break;
                    }
                case QLL_STMT_CREATE_SEQUENCE_TYPE:
                    {
                        /**
                         * validate CREATE SEQUENCE
                         */

                        qlgSetQueryPhaseMemMgr( aDBCStmt, QLL_PHASE_VALIDATION, aEnv );

                        STL_TRY( qlqValidateCreateSeq( aTransID,
                                                       aDBCStmt,
                                                       aSQLStmt,
                                                       aSQLStmt->mRetrySQL,
                                                       sSchemaElement,
                                                       aEnv )
                                 == STL_SUCCESS );

                        /**
                         * code optimize CREATE SEQEUNCE
                         */

                        qlgSetQueryPhaseMemMgr( aDBCStmt, QLL_PHASE_CODE_OPTIMIZE, aEnv );

                        STL_TRY( qlqOptCodeCreateSeq( aTransID,
                                                      aDBCStmt,
                                                      aSQLStmt,
                                                      aEnv )
                                 == STL_SUCCESS );

                        /**
                         * data optimize CREATE SEQUENCE
                         */

                        qlgSetQueryPhaseMemMgr( aDBCStmt, QLL_PHASE_DATA_OPTIMIZE, aEnv );

                        STL_TRY( qlqOptDataCreateSeq( aTransID,
                                                      aDBCStmt,
                                                      aSQLStmt,
                                                      aEnv )
                                 == STL_SUCCESS );

                        /**********************************************
                         * execute CREATE SEQEUNCE
                         **********************************************/

                        /**
                         * owner 를 current user 로 설정
                         */
                        
                        ellSetCurrentUser( & sInitPlan->mOwnerHandle, ELL_ENV(aEnv) );

                        /**
                         * execute CREATE SEQUENCE
                         */

                        qlgSetQueryPhaseMemMgr( aDBCStmt, QLL_PHASE_EXECUTE, aEnv );
                        
                        STL_TRY( qlqExecuteCreateSeq( aTransID,
                                                      sElementStmt,
                                                      aDBCStmt,
                                                      aSQLStmt,
                                                      aEnv )
                                 == STL_SUCCESS );

                        /**
                         * Current User 를 Session User 로 원복 
                         */
        
                        ellRollbackCurrentUser( ELL_ENV(aEnv) );
                        
                        break;
                    }
                case QLL_STMT_GRANT_DATABASE_TYPE:
                case QLL_STMT_GRANT_TABLESPACE_TYPE:
                case QLL_STMT_GRANT_SCHEMA_TYPE:
                case QLL_STMT_GRANT_TABLE_TYPE:
                case QLL_STMT_GRANT_USAGE_TYPE:
                    {
                        /**
                         * validate GRANT privilege
                         */

                        qlgSetQueryPhaseMemMgr( aDBCStmt, QLL_PHASE_VALIDATION, aEnv );

                        STL_TRY( qlaValidateGrantPriv( aTransID,
                                                       aDBCStmt,
                                                       aSQLStmt,
                                                       aSQLStmt->mRetrySQL,
                                                       sSchemaElement,
                                                       aEnv )
                                 == STL_SUCCESS );

                        /**
                         * code optimize GRANT privilege
                         */

                        qlgSetQueryPhaseMemMgr( aDBCStmt, QLL_PHASE_CODE_OPTIMIZE, aEnv );

                        STL_TRY( qlaOptCodeGrantPriv( aTransID,
                                                      aDBCStmt,
                                                      aSQLStmt,
                                                      aEnv )
                                 == STL_SUCCESS );

                        /**
                         * data optimize GRANT privilege
                         */

                        qlgSetQueryPhaseMemMgr( aDBCStmt, QLL_PHASE_DATA_OPTIMIZE, aEnv );

                        STL_TRY( qlaOptDataGrantPriv( aTransID,
                                                      aDBCStmt,
                                                      aSQLStmt,
                                                      aEnv )
                                 == STL_SUCCESS );

                        /**********************************************
                         * execute GRANT privilege
                         **********************************************/

                        /**
                         * owner 를 current user 로 설정
                         */
                        
                        ellSetCurrentUser( & sInitPlan->mOwnerHandle, ELL_ENV(aEnv) );

                        /**
                         * execute GRANT privilege
                         */

                        qlgSetQueryPhaseMemMgr( aDBCStmt, QLL_PHASE_EXECUTE, aEnv );
                        
                        STL_TRY( qlaExecuteGrantPriv( aTransID,
                                                      sElementStmt,
                                                      aDBCStmt,
                                                      aSQLStmt,
                                                      aEnv )
                                 == STL_SUCCESS );

                        /**
                         * Current User 를 Session User 로 원복 
                         */

                        ellRollbackCurrentUser( ELL_ENV(aEnv) );
                        
                        break;
                    }
                case QLL_STMT_COMMENT_ON_TYPE:
                    {
                        /**
                         * validate COMMENT ON
                         */

                        qlgSetQueryPhaseMemMgr( aDBCStmt, QLL_PHASE_VALIDATION, aEnv );

                        STL_TRY( qldValidateCommentStmt( aTransID,
                                                         aDBCStmt,
                                                         aSQLStmt,
                                                         aSQLStmt->mRetrySQL,
                                                         sSchemaElement,
                                                         aEnv )
                                 == STL_SUCCESS );

                        /**
                         * code optimize COMMENT ON
                         */

                        qlgSetQueryPhaseMemMgr( aDBCStmt, QLL_PHASE_CODE_OPTIMIZE, aEnv );

                        STL_TRY( qldOptCodeCommentStmt( aTransID,
                                                        aDBCStmt,
                                                        aSQLStmt,
                                                        aEnv )
                                 == STL_SUCCESS );

                        /**
                         * data optimize COMMENT ON
                         */

                        qlgSetQueryPhaseMemMgr( aDBCStmt, QLL_PHASE_DATA_OPTIMIZE, aEnv );

                        STL_TRY( qldOptDataCommentStmt( aTransID,
                                                        aDBCStmt,
                                                        aSQLStmt,
                                                        aEnv )
                                 == STL_SUCCESS );

                        /**********************************************
                         * execute COMMENT ON
                         **********************************************/

                        /**
                         * owner 를 current user 로 설정
                         */
                        
                        ellSetCurrentUser( & sInitPlan->mOwnerHandle, ELL_ENV(aEnv) );

                        /**
                         * execute COMMENT ON
                         */

                        qlgSetQueryPhaseMemMgr( aDBCStmt, QLL_PHASE_EXECUTE, aEnv );
                        
                        STL_TRY( qldExecuteCommentStmt( aTransID,
                                                        sElementStmt,
                                                        aDBCStmt,
                                                        aSQLStmt,
                                                        aEnv )
                                 == STL_SUCCESS );

                        /**
                         * Current User 를 Session User 로 원복 
                         */
        
                        ellRollbackCurrentUser( ELL_ENV(aEnv) );
                        
                        break;
                    }
                default:
                    {
                        STL_ASSERT(0);
                        break;
                    }
            }

            /**
             * Element Statement 종료
             */
            
            sState = 0;
            STL_TRY( smlFreeStatement( sElementStmt, SML_ENV(aEnv) ) == STL_SUCCESS );
            sElementStmt = NULL;
        }

        /**
         * SQL Type 원복
         */
        
        aSQLStmt->mStmtType = QLL_STMT_CREATE_SCHEMA_TYPE;
        aSQLStmt->mInitPlan = sInitPlan;

    }

    return STL_SUCCESS;
    
    STL_FINISH;

    /**
     * Current User 를 Session User 로 원복 
     */
    
    ellRollbackCurrentUser( ELL_ENV(aEnv) );

    /**
     * Element Statement Rollback
     */

    switch (sState)
    {
        case 1:
            (void) smlRollbackStatement( sElementStmt, SML_ENV( aEnv ) );
            (void) smlFreeStatement( sElementStmt, SML_ENV( aEnv ) );
        default:
            break;
    }
    
    return STL_FAILURE;
}


/**
 * @brief Schema Object 들에 대한 schema name matching
 * @param[in] aSchemaName        Schema Name
 * @param[in] aSQLString         SQL String
 * @param[in] aSchemaObjectList  Schema Object List (nullable)
 * @param[in] aEnv               Environment
 * @remarks
 */
stlStatus qlsMatchSchemaName( stlChar      * aSchemaName,
                              stlChar      * aSQLString,
                              qlpList      * aSchemaObjectList,
                              qllEnv       * aEnv )
{
    qlpListElement * sListElement = NULL;
    qlpObjectName  * sObjectName = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSchemaName != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Schema Object List 의 Schema Name Validation 
     */

    if ( aSchemaObjectList == NULL )
    {
        /**
         * Schema Object 가 없는 경우
         * - ex) CREATE SCHEMA s1 GRANT CREATE SESSION ON DATABASE TO u1;
         */
    }
    else
    {
        QLP_LIST_FOR_EACH( aSchemaObjectList, sListElement )
        {
            sObjectName = (qlpObjectName *) QLP_LIST_GET_POINTER_VALUE( sListElement );
        
            if ( sObjectName->mSchema == NULL )
            {
                /**
                 * Schema Name 이 명시되지 않은 경우
                 */
            
                sObjectName->mSchema    = aSchemaName;
                sObjectName->mSchemaPos = sObjectName->mObjectPos;
            }
            else
            {
                /**
                 * Schema Name 이 동일한지 검사
                 */
            
                STL_TRY_THROW( stlStrcmp( sObjectName->mSchema, aSchemaName ) == 0,
                               RAMP_MISMATCH_SCHEMA_NAME );
            }
        }
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_MISMATCH_SCHEMA_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_ELEMENT_NOT_MATCH_SCHEMA_IDENTIFIER,
                      qlgMakeErrPosString( aSQLString,
                                           sObjectName->mSchemaPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}




/**
 * @brief GRANT Privilege 에 대한 schema name matching
 * @param[in] aSchemaName   Schema Name
 * @param[in] aSQLString    SQL String
 * @param[in] aParseTree    CREATE SEQUENCE parse tree
 * @param[in] aEnv          Environment
 * @remarks
 */
stlStatus qlsMatchSchemaNameGrantPriv( stlChar      * aSchemaName,
                                       stlChar      * aSQLString,
                                       qlpGrantPriv * aParseTree,
                                       qllEnv       * aEnv )
{
    qlpPrivObject  * sPrivObject = NULL;
    stlChar        * sSchemaName = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSchemaName != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    /********************************************************
     * Schema of Privilege Object
     ********************************************************/

    sPrivObject = aParseTree->mPrivilege->mPrivObject;

    switch (sPrivObject->mPrivObjType)
    {
        case ELL_PRIV_DATABASE:
            {
                STL_THROW( RAMP_NOT_SCHEMA_OBJECT_PRIV );
                break;
            }
        case ELL_PRIV_SPACE:
            {
                STL_THROW( RAMP_MISMATCH_SCHEMA_NAME );
                break;
            }
        case ELL_PRIV_SCHEMA:
            {
                sSchemaName = QLP_GET_PTR_VALUE( sPrivObject->mNonSchemaObjectName );
                
                /**
                 * Schema Name 이 동일한지 검사
                 */
                
                STL_TRY_THROW( stlStrcmp( sSchemaName, aSchemaName ) == 0, RAMP_MISMATCH_SCHEMA_NAME );
                break;
            }
        case ELL_PRIV_TABLE:
        case ELL_PRIV_USAGE:
        case ELL_PRIV_COLUMN:
            {
                sSchemaName = sPrivObject->mSchemaObjectName->mSchema;

                if ( sSchemaName == NULL )
                {
                    /**
                     * Schema Name 이 명시되지 않은 경우
                     */
                    
                    sPrivObject->mSchemaObjectName->mSchema    = aSchemaName;
                    sPrivObject->mSchemaObjectName->mSchemaPos = sPrivObject->mSchemaObjectName->mObjectPos;
                }
                else
                {
                    STL_TRY_THROW( stlStrcmp( sSchemaName, aSchemaName ) == 0, RAMP_MISMATCH_QUANTIFIER_NAME );
                }
                
                break;
            }
        default:
            {
                STL_ASSERT(0);
                break;
            }
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_NOT_SCHEMA_OBJECT_PRIV )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_ELEMENT_NOT_MATCH_SCHEMA_IDENTIFIER,
                      qlgMakeErrPosString( aSQLString,
                                           sPrivObject->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_CATCH( RAMP_MISMATCH_SCHEMA_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_ELEMENT_NOT_MATCH_SCHEMA_IDENTIFIER,
                      qlgMakeErrPosString( aSQLString,
                                           sPrivObject->mNonSchemaObjectName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_MISMATCH_QUANTIFIER_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_ELEMENT_NOT_MATCH_SCHEMA_IDENTIFIER,
                      qlgMakeErrPosString( aSQLString,
                                           sPrivObject->mSchemaObjectName->mSchemaPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}




/**
 * @brief COMMENT ON 에 대한 schema name matching
 * @param[in] aSchemaName   Schema Name
 * @param[in] aSQLString    SQL String
 * @param[in] aParseTree    COMMENT ON parse tree
 * @param[in] aEnv          Environment
 * @remarks
 */
stlStatus qlsMatchSchemaNameCommentOn( stlChar          * aSchemaName,
                                       stlChar          * aSQLString,
                                       qlpCommentOnStmt * aParseTree,
                                       qllEnv           * aEnv )
{
    qlpCommentObject * sCommentObject = NULL;
    stlChar          * sSchemaName = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSchemaName != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    /********************************************************
     * Schema of Comment Object
     ********************************************************/

    sCommentObject = aParseTree->mCommentObject;

    switch (sCommentObject->mObjectType)
    {
        case ELL_OBJECT_CATALOG:
        case ELL_OBJECT_AUTHORIZATION:
        case ELL_OBJECT_TABLESPACE:
        
            {
                STL_THROW( RAMP_NOT_SCHEMA_OBJECT );
                break;
            }
        case ELL_OBJECT_SCHEMA:
            {
                sSchemaName = QLP_GET_PTR_VALUE( sCommentObject->mNonSchemaObjectName );
                
                /**
                 * Schema Name 이 동일한지 검사
                 */
                
                STL_TRY_THROW( stlStrcmp( sSchemaName, aSchemaName ) == 0, RAMP_MISMATCH_SCHEMA_NAME );
                break;
            }
        case ELL_OBJECT_TABLE:
        case ELL_OBJECT_CONSTRAINT:
        case ELL_OBJECT_INDEX:
        case ELL_OBJECT_SEQUENCE:
            {
                sSchemaName = sCommentObject->mSchemaObjectName->mSchema;

                if ( sSchemaName == NULL )
                {
                    /**
                     * Schema Name 이 명시되지 않은 경우
                     */
                    
                    sCommentObject->mSchemaObjectName->mSchema    = aSchemaName;
                    sCommentObject->mSchemaObjectName->mSchemaPos = sCommentObject->mSchemaObjectName->mObjectPos;
                }
                else
                {
                    STL_TRY_THROW( stlStrcmp( sSchemaName, aSchemaName ) == 0, RAMP_MISMATCH_QUANTIFIER_NAME );
                }
                
                break;
            }
        case ELL_OBJECT_COLUMN:
            {
                sSchemaName = sCommentObject->mColumnObjectName->mSchema;

                if ( sSchemaName == NULL )
                {
                    /**
                     * Schema Name 이 명시되지 않은 경우
                     */
                    
                    sCommentObject->mColumnObjectName->mSchema    = aSchemaName;
                    sCommentObject->mColumnObjectName->mSchemaPos = sCommentObject->mColumnObjectName->mColumnPos;
                }
                else
                {
                    STL_TRY_THROW( stlStrcmp( sSchemaName, aSchemaName ) == 0, RAMP_MISMATCH_COLUMN_SCHEMA_NAME );
                }
                
                break;
            }
        default:
            {
                STL_ASSERT(0);
                break;
            }
    }

    return STL_SUCCESS;
    
    STL_CATCH( RAMP_NOT_SCHEMA_OBJECT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_ELEMENT_NOT_MATCH_SCHEMA_IDENTIFIER,
                      qlgMakeErrPosString( aSQLString,
                                           sCommentObject->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_CATCH( RAMP_MISMATCH_SCHEMA_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_ELEMENT_NOT_MATCH_SCHEMA_IDENTIFIER,
                      qlgMakeErrPosString( aSQLString,
                                           sCommentObject->mNonSchemaObjectName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_MISMATCH_QUANTIFIER_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_ELEMENT_NOT_MATCH_SCHEMA_IDENTIFIER,
                      qlgMakeErrPosString( aSQLString,
                                           sCommentObject->mSchemaObjectName->mSchemaPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_MISMATCH_COLUMN_SCHEMA_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_ELEMENT_NOT_MATCH_SCHEMA_IDENTIFIER,
                      qlgMakeErrPosString( aSQLString,
                                           sCommentObject->mColumnObjectName->mSchemaPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}





/** @} qlsCreateSchema */
