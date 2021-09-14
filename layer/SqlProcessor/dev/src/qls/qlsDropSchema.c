/*******************************************************************************
 * qlsDropSchema.c
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
 * @file qlsDropSchema.c
 * @brief DROP SCHEMA 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qlsDropSchema DROP SCHEMA
 * @ingroup qls
 * @{
 */


/**
 * @brief DROP SCHEMA 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlsValidateDropSchema( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 stlChar       * aSQLString,
                                 qllNode       * aParseTree,
                                 qllEnv        * aEnv )
{
    stlInt32  sState = 0;
    
    qlsInitDropSchema  * sInitPlan = NULL;
    qlpDropSchema      * sParseTree = NULL;
    
    stlBool             sObjectExist = STL_FALSE;

    ellObjectList * sInnerSeqList = NULL;
    ellObjectList * sInnerSynonymList = NULL;
    ellObjectList * sInnerTableList = NULL;
    ellObjectList * sOuterTableInnerPrimaryKeyList = NULL;
    ellObjectList * sOuterTableInnerUniqueKeyList = NULL;
    ellObjectList * sOuterTableInnerIndexList = NULL;
    ellObjectList * sOuterTableInnerNotNullList = NULL;
    ellObjectList * sOuterTableInnerCheckClauseList = NULL;
    ellObjectList * sInnerForeignKeyList = NULL;
    ellObjectList * sOuterForeignKeyList = NULL;

    smlStatement  * sStmt = NULL;
    stlInt32        sStmtAttr = 0;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_DROP_SCHEMA_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_DROP_SCHEMA_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */

    sParseTree = (qlpDropSchema *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlsInitDropSchema),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlsInitDropSchema) );

    STL_TRY( qlvSetInitPlan( aDBCStmt,
                             aSQLStmt,
                             & sInitPlan->mCommonInit,
                             aEnv )
             == STL_SUCCESS );
    
    /**
     * Statement 할당
     */
    
    sStmtAttr = SML_STMT_ATTR_READONLY;
    
    STL_TRY( smlAllocStatement( aTransID,
                                NULL, /* aUpdateRelHandle */
                                sStmtAttr,
                                SML_LOCK_TIMEOUT_INVALID,
                                STL_TRUE, /* aNeedSnapshotIterator */
                                & sStmt,
                                SML_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 1;

    aSQLStmt->mViewSCN = sStmt->mScn;
    
    /*************************************************
     * SCHEMA name 검증 
     *************************************************/

    /**
     * mIfExists
     */

    sInitPlan->mIfExists = sParseTree->mIfExists;
    
    /**
     * Schema 존재 여부
     */

    STL_TRY( ellGetSchemaDictHandle( aTransID,
                                     aSQLStmt->mViewSCN,
                                     QLP_GET_PTR_VALUE( sParseTree->mName ),
                                     & sInitPlan->mSchemaHandle,
                                     & sObjectExist,
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );

    if ( sObjectExist == STL_TRUE )
    {
        /**
         * Schema 가 존재함
         */
        
        sInitPlan->mSchemaExist = STL_TRUE;
    }
    else
    {
        /**
         * Schema 가 없음
         */
        
        sInitPlan->mSchemaExist = STL_FALSE;
            
        /**
         * If Exists 옵션이 있다면 무조건 SUCCESS 한다.
         */
        
        if ( sInitPlan->mIfExists == STL_TRUE )
        {
            STL_THROW( RAMP_IF_EXISTS_NO_SCHEMA );
        }
        else
        {
            STL_THROW( RAMP_ERR_SCHEMA_NOT_EXIST );
        }
    }

    /*************************************************
     * SCHEMA 검증 
     *************************************************/

    /**
     * System schema 검사
     */

    STL_TRY_THROW( ellIsBuiltInSchema( &sInitPlan->mSchemaHandle ) == STL_FALSE,
                   RAMP_ERR_DROP_BUILTIN_SCHEMA );

        
    /**
     * DROP SCHEMA 권한 검사
     */

    STL_TRY( qlaCheckPrivDropSchema( aTransID,
                                     aDBCStmt,
                                     aSQLStmt,
                                     & sInitPlan->mSchemaHandle,
                                     aEnv )
             == STL_SUCCESS );
             
    /*************************************************
     * Option Validation
     *************************************************/

    sInitPlan->mIsCascade = sParseTree->mIsCascade;
    
    /**
     * Drop Schema 관련 객체 정보 획득
     */

    STL_TRY( ellGetObjectList4DropSchema( aTransID,
                                          sStmt,
                                          & sInitPlan->mSchemaHandle,
                                          QLL_INIT_PLAN(aDBCStmt),
                                          & sInnerSeqList,
                                          & sInnerSynonymList,
                                          & sInnerTableList,
                                          & sOuterTableInnerPrimaryKeyList,
                                          & sOuterTableInnerUniqueKeyList,
                                          & sOuterTableInnerIndexList,
                                          & sOuterTableInnerNotNullList,
                                          & sOuterTableInnerCheckClauseList,
                                          & sInnerForeignKeyList,
                                          & sOuterForeignKeyList,
                                          ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * CASCADE 검증
     */
    
    if ( sInitPlan->mIsCascade == STL_TRUE )
    {
        /**
         * nothing to do
         */
    }
    else
    {
        /**
         * SCHEMA 내에 객체가 존재하지 않아야 함
         */

        STL_TRY_THROW( ellHasObjectItem( sInnerSeqList ) == STL_FALSE,
                       RAMP_ERR_SCHEMA_NOT_EMPTY );

        STL_TRY_THROW( ellHasObjectItem( sInnerSynonymList ) == STL_FALSE,
                       RAMP_ERR_SCHEMA_NOT_EMPTY );
        
        STL_TRY_THROW( ellHasObjectItem( sInnerTableList ) == STL_FALSE,
                       RAMP_ERR_SCHEMA_NOT_EMPTY );

        STL_TRY_THROW( ellHasObjectItem( sOuterTableInnerPrimaryKeyList ) == STL_FALSE,
                       RAMP_ERR_SCHEMA_NOT_EMPTY );

        STL_TRY_THROW( ellHasObjectItem( sOuterTableInnerUniqueKeyList ) == STL_FALSE,
                       RAMP_ERR_SCHEMA_NOT_EMPTY );

        STL_TRY_THROW( ellHasObjectItem( sOuterTableInnerIndexList ) == STL_FALSE,
                       RAMP_ERR_SCHEMA_NOT_EMPTY );

        STL_TRY_THROW( ellHasObjectItem( sOuterTableInnerNotNullList ) == STL_FALSE,
                       RAMP_ERR_SCHEMA_NOT_EMPTY );

        STL_TRY_THROW( ellHasObjectItem( sOuterTableInnerCheckClauseList ) == STL_FALSE,
                       RAMP_ERR_SCHEMA_NOT_EMPTY );
        
        STL_TRY_THROW( ellHasObjectItem( sInnerForeignKeyList ) == STL_FALSE,
                       RAMP_ERR_SCHEMA_NOT_EMPTY );
    }

    /********************************************************
     * Validation Object 추가 
     ********************************************************/

    STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                  & sInitPlan->mSchemaHandle,
                                  QLL_INIT_PLAN(aDBCStmt),
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /*************************************************
     * Plan 연결 
     *************************************************/
    
    /**
     * IF EXISTS 구문이 있을 경우, Schema 가 존재하지 않는다면,
     * 수행없이 SUCCESS 하게 되고, 권한 검사 및 Writable 은 검사하지 않는다.
     */
    
    STL_RAMP( RAMP_IF_EXISTS_NO_SCHEMA );

    /**
     * Statement 해제
     */
    
    sState = 0;
    STL_TRY( smlFreeStatement( sStmt, SML_ENV(aEnv) ) == STL_SUCCESS );
    sStmt = NULL;
    
    /**
     * Init Plan 연결 
     */

    aSQLStmt->mInitPlan = (void *) sInitPlan;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SCHEMA_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      QLP_GET_PTR_VALUE( sParseTree->mName ) );
    }

    STL_CATCH( RAMP_ERR_DROP_BUILTIN_SCHEMA )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_BUILT_IN_SCHEMA_IS_NOT_MODIFIABLE,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      QLP_GET_PTR_VALUE( sParseTree->mName ) );
    }

    STL_CATCH( RAMP_ERR_SCHEMA_NOT_EMPTY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_NOT_EMPTY,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    /* STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED ) */
    /* { */
    /*     stlPushError( STL_ERROR_LEVEL_ABORT, */
    /*                   QLL_ERRCODE_NOT_IMPLEMENTED, */
    /*                   NULL, */
    /*                   QLL_ERROR_STACK(aEnv), */
    /*                   "qlsValidateDropSchema()" ); */
    /* } */
    
    STL_FINISH;

    switch (sState)
    {
        case 1:
            (void) smlFreeStatement( sStmt, SML_ENV(aEnv) );
        default:
            break;
    }
    
    return STL_FAILURE;
}

/**
 * @brief DROP SCHEMA 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlsOptCodeDropSchema( smlTransId      aTransID,
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
 * @brief DROP SCHEMA 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlsOptDataDropSchema( smlTransId      aTransID,
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
 * @brief DROP SCHEMA 구문을 Execute 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aStmt      Statement
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlsExecuteDropSchema( smlTransId      aTransID,
                                smlStatement  * aStmt,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv )
{
    qlsInitDropSchema  * sInitPlan = NULL;
    
    ellObjectList * sInnerSeqList = NULL;
    ellObjectList * sInnerSynonymList = NULL;
    ellObjectList * sInnerTableList = NULL;
    ellObjectList * sOuterTableInnerPrimaryKeyList = NULL;
    ellObjectList * sOuterTableInnerUniqueKeyList = NULL;
    ellObjectList * sOuterTableInnerIndexList = NULL;
    ellObjectList * sOuterTableInnerNotNullList = NULL;
    ellObjectList * sOuterTableInnerCheckClauseList = NULL;
    ellObjectList * sInnerForeignKeyList = NULL;
    ellObjectList * sOuterForeignKeyList = NULL;

    ellObjectList * sViewColumnList = NULL;
    ellObjectList * sAffectedViewList = NULL;
    
    ellObjectList * sOuterTableInnerPrimaryKeyTableList = NULL;
    ellObjectList * sOuterTableInnerPrimaryKeyIndexList = NULL;

    ellObjectList * sOuterTableInnerUniqueKeyTableList = NULL;
    ellObjectList * sOuterTableInnerUniqueKeyIndexList = NULL;

    ellObjectList * sOuterTableInnerIndexTableList = NULL;
    ellObjectList * sOuterTableInnerNotNullTableList = NULL;
    ellObjectList * sOuterTableInnerCheckClauseTableList = NULL;
    
    ellObjectList * sInnerForeignKeyParentTableList = NULL;
    ellObjectList * sInnerForeignKeyChildTableList = NULL;
    ellObjectList * sInnerForeignKeyIndexList = NULL;

    ellObjectList * sOuterForeignKeyParentTableList = NULL;
    ellObjectList * sOuterForeignKeyChildTableList = NULL;
    ellObjectList * sOuterForeignKeyIndexList = NULL;

    ellObjectList * sRemainNotNullList = NULL;
    ellObjectList * sRemainNotNullTableList = NULL;

    ellObjectItem   * sNotNullItem   = NULL;
    ellDictHandle   * sNotNullHandle = NULL;

    ellObjectItem   * sTableItem   = NULL;
    ellDictHandle   * sTableHandle = NULL;
    
    stlTime  sTime = 0;

    stlBool   sLocked = STL_TRUE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_DROP_SCHEMA_TYPE,
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
    
    sInitPlan = (qlsInitDropSchema *) aSQLStmt->mInitPlan;

    /**
     * IF EXISTS 옵션 처리
     */

    if ( (sInitPlan->mIfExists == STL_TRUE) &&
         (sInitPlan->mSchemaExist != STL_TRUE) )
    {
        /**
         * IF EXISTS 옵션을 사용하고, Schema 가 존재하지 않는 경우
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

    STL_TRY( ellLock4DropSchema( aTransID,
                                 aStmt,
                                 & sInitPlan->mSchemaHandle,
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

    sInitPlan = (qlsInitDropSchema *) aSQLStmt->mInitPlan;

    /**********************************************************
     * 관련 객체 정보 획득 및 Lock 획득
     **********************************************************/
    
    /**
     * Drop Schema 관련 객체 정보 획득
     */

    STL_TRY( ellGetObjectList4DropSchema( aTransID,
                                          aStmt,
                                          & sInitPlan->mSchemaHandle,
                                          & QLL_EXEC_DDL_MEM(aEnv),
                                          & sInnerSeqList,
                                          & sInnerSynonymList,
                                          & sInnerTableList,
                                          & sOuterTableInnerPrimaryKeyList,
                                          & sOuterTableInnerUniqueKeyList,
                                          & sOuterTableInnerIndexList,
                                          & sOuterTableInnerNotNullList,
                                          & sOuterTableInnerCheckClauseList,
                                          & sInnerForeignKeyList,
                                          & sOuterForeignKeyList,
                                          ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * CASCADE 검증
     */
    
    if ( sInitPlan->mIsCascade == STL_TRUE )
    {
        /**
         * nothing to do
         */
    }
    else
    {
        /**
         * SCHEMA 내에 객체가 존재하지 않아야 함
         */

        STL_TRY_THROW( ellHasObjectItem( sInnerSeqList ) == STL_FALSE,
                       RAMP_ERR_SCHEMA_NOT_EMPTY );

        STL_TRY_THROW( ellHasObjectItem( sInnerSynonymList ) == STL_FALSE,
                       RAMP_ERR_SCHEMA_NOT_EMPTY );
        
        STL_TRY_THROW( ellHasObjectItem( sInnerTableList ) == STL_FALSE,
                       RAMP_ERR_SCHEMA_NOT_EMPTY );

        STL_TRY_THROW( ellHasObjectItem( sOuterTableInnerPrimaryKeyList ) == STL_FALSE,
                       RAMP_ERR_SCHEMA_NOT_EMPTY );

        STL_TRY_THROW( ellHasObjectItem( sOuterTableInnerUniqueKeyList ) == STL_FALSE,
                       RAMP_ERR_SCHEMA_NOT_EMPTY );

        STL_TRY_THROW( ellHasObjectItem( sOuterTableInnerIndexList ) == STL_FALSE,
                       RAMP_ERR_SCHEMA_NOT_EMPTY );

        STL_TRY_THROW( ellHasObjectItem( sOuterTableInnerNotNullList ) == STL_FALSE,
                       RAMP_ERR_SCHEMA_NOT_EMPTY );

        STL_TRY_THROW( ellHasObjectItem( sOuterTableInnerCheckClauseList ) == STL_FALSE,
                       RAMP_ERR_SCHEMA_NOT_EMPTY );
        
        STL_TRY_THROW( ellHasObjectItem( sInnerForeignKeyList ) == STL_FALSE,
                       RAMP_ERR_SCHEMA_NOT_EMPTY );
    }

    /**
     * Inner Sequence List 에 X lock 획득
     */

    STL_TRY( ellLockRelatedSeqList4DDL( aTransID,
                                        aStmt,
                                        sInnerSeqList,
                                        & sLocked,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );

     /**
     * Inner Synonym List 에 X lock 획득
     */

    STL_TRY( ellLockRelatedSynonymList4DDL( aTransID,
                                            aStmt,
                                            sInnerSynonymList,
                                            & sLocked,
                                            ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    /**
     * Inner Table List 에 X lock 획득
     */
    
    STL_TRY( ellLockRelatedTableList4DDL( aTransID,
                                          aStmt,
                                          SML_LOCK_X,
                                          sInnerTableList,
                                          & sLocked,
                                          ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    /**
     * Outer Table Inner Primary Key 의 Table List 에 X lock 획득 
     */

    STL_TRY( ellLock4ConstraintList( aTransID,
                                     aStmt,
                                     sOuterTableInnerPrimaryKeyList,
                                     & sLocked,
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    /**
     * Outer Table Inner Unique Key 의 Table List 에 S lock 획득 
     */

    STL_TRY( ellLock4ConstraintList( aTransID,
                                     aStmt,
                                     sOuterTableInnerUniqueKeyList,
                                     & sLocked,
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    /**
     * Outer Table Inner Index 의 Table List 에 S lock 획득 
     */

    STL_TRY( ellLock4IndexList( aTransID,
                                aStmt,
                                sOuterTableInnerIndexList,
                                & sLocked,
                                ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    /**
     * Outer Table Inner Not Null 의 Table List 에 X lock 획득 
     */

    STL_TRY( ellLock4ConstraintList( aTransID,
                                     aStmt,
                                     sOuterTableInnerNotNullList,
                                     & sLocked,
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    /**
     * Outer Table Inner Check Clause 의 Table List 에 S lock 획득 
     */

    STL_TRY( ellLock4ConstraintList( aTransID,
                                     aStmt,
                                     sOuterTableInnerCheckClauseList,
                                     & sLocked,
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );

    /**
     * Inner Foreign Key List 의 Table List 에 S lock 획득 
     */
    
    STL_TRY( ellLock4ConstraintList( aTransID,
                                     aStmt,
                                     sInnerForeignKeyList,
                                     & sLocked,
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    /**
     * Outer Foreign Key List 의 Table List 에 S lock 획득 
     */
    
    STL_TRY( ellLock4ConstraintList( aTransID,
                                     aStmt,
                                     sOuterForeignKeyList,
                                     & sLocked,
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    /**
     * Lock 을 모두 획득 후 다시 Drop Schema 관련 객체 정보 획득
     */

    STL_TRY( ellGetObjectList4DropSchema( aTransID,
                                          aStmt,
                                          & sInitPlan->mSchemaHandle,
                                          & QLL_EXEC_DDL_MEM(aEnv),
                                          & sInnerSeqList,
                                          & sInnerSynonymList,
                                          & sInnerTableList,
                                          & sOuterTableInnerPrimaryKeyList,
                                          & sOuterTableInnerUniqueKeyList,
                                          & sOuterTableInnerIndexList,
                                          & sOuterTableInnerNotNullList,
                                          & sOuterTableInnerCheckClauseList,
                                          & sInnerForeignKeyList,
                                          & sOuterForeignKeyList,
                                          ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /***************************************************************
     * 관련 객체로부터 영향을 받는 정보 회득 및 Lock 획득 
     ***************************************************************/

    /**
     * Inner Relation 의 View Column List 획득
     */

    STL_TRY( ellGetViewColumnListByRelationList( aTransID,
                                                 aStmt,
                                                 sInnerTableList,
                                                 & QLL_EXEC_DDL_MEM(aEnv),
                                                 & sViewColumnList,
                                                 ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Inner Relation 의 영향을 받는 Outer View List 획득 및 X Lock 획득 
     */

    STL_TRY( ellGetAffectedOuterViewList4DropNonSchema( aTransID,
                                                        aStmt,
                                                        sInnerTableList,
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
    
    STL_TRY( ellGetAffectedOuterViewList4DropNonSchema( aTransID,
                                                        aStmt,
                                                        sInnerTableList,
                                                        & QLL_EXEC_DDL_MEM(aEnv),
                                                        & sAffectedViewList,
                                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    
    /**
     * Outer Table Inner Primary Key 와 관련된 정보 획득 및 Table List 에 X lock 획득 
     */

    STL_TRY( ellGetTableList4ConstList( aTransID,
                                        ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                        sOuterTableInnerPrimaryKeyList,
                                        & QLL_EXEC_DDL_MEM(aEnv),
                                        & sOuterTableInnerPrimaryKeyTableList,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellLockRelatedTableList4DDL( aTransID,
                                          aStmt,
                                          SML_LOCK_X,
                                          sOuterTableInnerPrimaryKeyTableList,
                                          & sLocked,
                                          ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    STL_TRY( ellGetTableList4ConstList( aTransID,
                                        ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                        sOuterTableInnerPrimaryKeyList,
                                        & QLL_EXEC_DDL_MEM(aEnv),
                                        & sOuterTableInnerPrimaryKeyTableList,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    
    STL_TRY( ellGetIndexList4KeyList( aTransID,
                                      sOuterTableInnerPrimaryKeyList,
                                      & QLL_EXEC_DDL_MEM(aEnv),
                                      & sOuterTableInnerPrimaryKeyIndexList,
                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Outer Table Inner Unique Key 와 관련된 정보 획득 및 Table List 에 S lock 획득 
     */

    STL_TRY( ellGetTableList4ConstList( aTransID,
                                        ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                        sOuterTableInnerUniqueKeyList,
                                        & QLL_EXEC_DDL_MEM(aEnv),
                                        & sOuterTableInnerUniqueKeyTableList,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellLockRelatedTableList4DDL( aTransID,
                                          aStmt,
                                          SML_LOCK_S,
                                          sOuterTableInnerUniqueKeyTableList,
                                          & sLocked,
                                          ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    STL_TRY( ellGetTableList4ConstList( aTransID,
                                        ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                        sOuterTableInnerUniqueKeyList,
                                        & QLL_EXEC_DDL_MEM(aEnv),
                                        & sOuterTableInnerUniqueKeyTableList,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY( ellGetIndexList4KeyList( aTransID,
                                      sOuterTableInnerUniqueKeyList,
                                      & QLL_EXEC_DDL_MEM(aEnv),
                                      & sOuterTableInnerUniqueKeyIndexList,
                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Outer Table Inner Index 와 관련된 정보 획득 및 Table List 에 S lock 획득 
     */

    STL_TRY( ellGetTableList4IndexList( aTransID,
                                        ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                        sOuterTableInnerIndexList,
                                        & QLL_EXEC_DDL_MEM(aEnv),
                                        & sOuterTableInnerIndexTableList,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellLockRelatedTableList4DDL( aTransID,
                                          aStmt,
                                          SML_LOCK_S,
                                          sOuterTableInnerIndexTableList,
                                          & sLocked,
                                          ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    STL_TRY( ellGetTableList4IndexList( aTransID,
                                        ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                        sOuterTableInnerIndexList,
                                        & QLL_EXEC_DDL_MEM(aEnv),
                                        & sOuterTableInnerIndexTableList,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );
    

    /**
     * Outer Table Inner Not Null 와 관련된 정보 획득 및 Table List 에 X lock 획득 
     */

    STL_TRY( ellGetTableList4ConstList( aTransID,
                                        ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                        sOuterTableInnerNotNullList,
                                        & QLL_EXEC_DDL_MEM(aEnv),
                                        & sOuterTableInnerNotNullTableList,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellLockRelatedTableList4DDL( aTransID,
                                          aStmt,
                                          SML_LOCK_X,
                                          sOuterTableInnerNotNullTableList,
                                          & sLocked,
                                          ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    STL_TRY( ellGetTableList4ConstList( aTransID,
                                        ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                        sOuterTableInnerNotNullList,
                                        & QLL_EXEC_DDL_MEM(aEnv),
                                        & sOuterTableInnerNotNullTableList,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Outer Table Inner Check Clause 와 관련된 정보 획득 및 Table List 에 S lock 획득 
     */

    STL_TRY( ellGetTableList4ConstList( aTransID,
                                        ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                        sOuterTableInnerCheckClauseList,
                                        & QLL_EXEC_DDL_MEM(aEnv),
                                        & sOuterTableInnerCheckClauseTableList,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellLockRelatedTableList4DDL( aTransID,
                                          aStmt,
                                          SML_LOCK_S,
                                          sOuterTableInnerCheckClauseTableList,
                                          & sLocked,
                                          ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    STL_TRY( ellGetTableList4ConstList( aTransID,
                                        ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                        sOuterTableInnerCheckClauseList,
                                        & QLL_EXEC_DDL_MEM(aEnv),
                                        & sOuterTableInnerCheckClauseTableList,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Inner Foreign Key 와 관련된 정보 획득 및 Parent Table 에 S lock 획득 
     */

    STL_TRY( ellGetParentTableList4ForeignKeyList( aTransID,
                                                   ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                                   sInnerForeignKeyList,
                                                   & QLL_EXEC_DDL_MEM(aEnv),
                                                   & sInnerForeignKeyParentTableList,
                                                   ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellLockRelatedTableList4DDL( aTransID,
                                          aStmt,
                                          SML_LOCK_S,
                                          sInnerForeignKeyParentTableList,
                                          & sLocked,
                                          ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    STL_TRY( ellGetParentTableList4ForeignKeyList( aTransID,
                                                   ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                                   sInnerForeignKeyList,
                                                   & QLL_EXEC_DDL_MEM(aEnv),
                                                   & sInnerForeignKeyParentTableList,
                                                   ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Inner Foreign Key 와 관련된 정보 획득 및 Child Table 에 S lock 획득 
     */
    
    STL_TRY( ellGetTableList4ConstList( aTransID,
                                        ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                        sInnerForeignKeyList,
                                        & QLL_EXEC_DDL_MEM(aEnv),
                                        & sInnerForeignKeyChildTableList,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellLockRelatedTableList4DDL( aTransID,
                                          aStmt,
                                          SML_LOCK_S,
                                          sInnerForeignKeyChildTableList,
                                          & sLocked,
                                          ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    STL_TRY( ellGetTableList4ConstList( aTransID,
                                        ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                        sInnerForeignKeyList,
                                        & QLL_EXEC_DDL_MEM(aEnv),
                                        & sInnerForeignKeyChildTableList,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellGetIndexList4KeyList( aTransID,
                                      sInnerForeignKeyList,
                                      & QLL_EXEC_DDL_MEM(aEnv),
                                      & sInnerForeignKeyIndexList,
                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Outer Foreign Key 와 관련된 정보 획득 및 Parent Table 에 S lock 획득 
     */

    STL_TRY( ellGetParentTableList4ForeignKeyList( aTransID,
                                                   ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                                   sOuterForeignKeyList,
                                                   & QLL_EXEC_DDL_MEM(aEnv),
                                                   & sOuterForeignKeyParentTableList,
                                                   ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellLockRelatedTableList4DDL( aTransID,
                                          aStmt,
                                          SML_LOCK_S,
                                          sOuterForeignKeyParentTableList,
                                          & sLocked,
                                          ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    STL_TRY( ellGetParentTableList4ForeignKeyList( aTransID,
                                                   ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                                   sOuterForeignKeyList,
                                                   & QLL_EXEC_DDL_MEM(aEnv),
                                                   & sOuterForeignKeyParentTableList,
                                                   ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Outer Foreign Key 와 관련된 정보 획득 및 Child Table 에 S lock 획득 
     */
    
    STL_TRY( ellGetTableList4ConstList( aTransID,
                                        ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                        sOuterForeignKeyList,
                                        & QLL_EXEC_DDL_MEM(aEnv),
                                        & sOuterForeignKeyChildTableList,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellLockRelatedTableList4DDL( aTransID,
                                          aStmt,
                                          SML_LOCK_S,
                                          sOuterForeignKeyChildTableList,
                                          & sLocked,
                                          ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    STL_TRY( ellGetTableList4ConstList( aTransID,
                                        ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                        sOuterForeignKeyList,
                                        & QLL_EXEC_DDL_MEM(aEnv),
                                        & sOuterForeignKeyChildTableList,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY( ellGetIndexList4KeyList( aTransID,
                                      sOuterForeignKeyList,
                                      & QLL_EXEC_DDL_MEM(aEnv),
                                      & sOuterForeignKeyIndexList,
                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /***************************************************************
     * 물리적 객체 제거 
     ***************************************************************/

    /**
     * Inner Sequence List 의 Sequence 객체 제거
     */

    STL_TRY( qlqDropSequence4SeqList( aStmt,
                                      sInnerSeqList,
                                      aEnv )
             == STL_SUCCESS );
    
    /**
     * Inner Foreign Key List 의 Index 제거
     */
    
    STL_TRY( qliDropIndexes4IndexList( aStmt,
                                       sInnerForeignKeyIndexList,
                                       aEnv )
             == STL_SUCCESS );

    /**
     * Outer Foreign Key List 의 Index 제거
     */
    
    STL_TRY( qliDropIndexes4IndexList( aStmt,
                                       sOuterForeignKeyIndexList,
                                       aEnv )
             == STL_SUCCESS );

    /**
     * Outer Table Inner Primary Key List 의 Index 제거
     */
    
    STL_TRY( qliDropIndexes4IndexList( aStmt,
                                       sOuterTableInnerPrimaryKeyIndexList,
                                       aEnv )
             == STL_SUCCESS );

    /**
     * Outer Table Inner Unique Key List 의 Index 제거
     */
    
    STL_TRY( qliDropIndexes4IndexList( aStmt,
                                       sOuterTableInnerUniqueKeyIndexList,
                                       aEnv )
             == STL_SUCCESS );

    /**
     * Outer Table Inner Index List 의 Index 제거
     */
    
    STL_TRY( qliDropIndexes4IndexList( aStmt,
                                       sOuterTableInnerIndexList,
                                       aEnv )
             == STL_SUCCESS );

    /**
     * Inner Table List 의 Table Prime Element 객체 제거 
     */
    
    STL_TRY( qlrDropPhyTablePrimeElement4TableList( aTransID,
                                                    aStmt,
                                                    aDBCStmt,
                                                    sInnerTableList,
                                                    aEnv )
             == STL_SUCCESS );

    /***************************************************************
     * Dictionary Record 변경
     ***************************************************************/

    /**
     * Affected View List 에 대해 IS_AFFECTED 를 TRUE 로 변경
     */

    STL_TRY( ellSetAffectedViewList( aTransID,
                                     aStmt,
                                     sAffectedViewList,
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Inner Sequence List 의 Dictionary Record 삭제
     */

    STL_TRY( ellRemoveDictSeqByObjectList( aTransID,
                                           aStmt,
                                           sInnerSeqList,
                                           ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Inner Synonym List 의 Dictionary Record 삭제
     */

    STL_TRY( ellRemoveDictSynonymByObjectList( aTransID,
                                               aStmt,
                                               sInnerSynonymList,
                                               ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Inner Foreign Key List 의 Dictionary Record 삭제 
     */
    
    STL_TRY( ellDeleteDictConstraintByObjectList( aTransID,
                                                  aStmt,
                                                  sInnerForeignKeyList,
                                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellDeleteDictIndexByObjectList( aTransID,
                                             aStmt,
                                             sInnerForeignKeyIndexList,
                                             ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Outer Foreign Key List 의 Dictionary Record 삭제 
     */
    
    STL_TRY( ellDeleteDictConstraintByObjectList( aTransID,
                                                  aStmt,
                                                  sOuterForeignKeyList,
                                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellDeleteDictIndexByObjectList( aTransID,
                                             aStmt,
                                             sOuterForeignKeyIndexList,
                                             ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Outer Table Inner Primary Key List 의 Dictionary Record 삭제
     */
    
    STL_TRY( ellDeleteDictConstraintByObjectList( aTransID,
                                                  aStmt,
                                                  sOuterTableInnerPrimaryKeyList,
                                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellDeleteDictIndexByObjectList( aTransID,
                                             aStmt,
                                             sOuterTableInnerPrimaryKeyIndexList,
                                             ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Outer Table Inner Unique Key List 의 Dictionary Record 삭제 
     */
    
    STL_TRY( ellDeleteDictConstraintByObjectList( aTransID,
                                                  aStmt,
                                                  sOuterTableInnerUniqueKeyList,
                                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellDeleteDictIndexByObjectList( aTransID,
                                             aStmt,
                                             sOuterTableInnerUniqueKeyIndexList,
                                             ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Outer Table Inner Index List 의 Dictionary Record 삭제 
     */
    
    STL_TRY( ellDeleteDictIndexByObjectList( aTransID,
                                             aStmt,
                                             sOuterTableInnerIndexList,
                                             ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Outer Table Inner Not Null List 의 Dictionary Record 삭제 
     */
    
    STL_TRY( ellDeleteDictConstraintByObjectList( aTransID,
                                                  aStmt,
                                                  sOuterTableInnerNotNullList,
                                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Outer Table Inner Check Clause List 의 Dictionary Record 삭제 
     */
    
    STL_TRY( ellDeleteDictConstraintByObjectList( aTransID,
                                                  aStmt,
                                                  sOuterTableInnerCheckClauseList,
                                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * PRIMARY KEY 정보와 NOT NULL 정보를 조합하여 Outer Table 의 Column Nullable 정보 설정
     */

    STL_TRY( qlsSetOuterTableColumnNullable4DropNonSchemaObject( aTransID,
                                                                 aStmt,
                                                                 & QLL_EXEC_DDL_MEM(aEnv),
                                                                 ELL_OBJECT_SCHEMA,
                                                                 & sInitPlan->mSchemaHandle,
                                                                 sOuterTableInnerPrimaryKeyTableList,
                                                                 sOuterTableInnerNotNullTableList,
                                                                 aEnv )
             == STL_SUCCESS );
                                                        
    /**
     * Inner Table List 의 Dictionary Record 삭제 
     */
    
    STL_TRY( ellRemoveDictTablePrimeElement4TableList( aTransID,
                                                       aStmt,
                                                       sInnerTableList,
                                                       ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Schema 의 Dictionary Record 삭제 
     */

    STL_TRY( ellRemoveDict4DropSchema( aTransID,
                                       aStmt,
                                       & sInitPlan->mSchemaHandle,
                                       ELL_ENV(aEnv) )
             == STL_SUCCESS );
                                       
    /***************************************************************
     * Dictionary Cache 재구성 
     ***************************************************************/

    /**
     * View Column List 의 Cache 제거
     * - View 의 Table Cache 와 Column Cache 간에는 연결 관계가 없음
     */

    STL_TRY( ellRefineCache4RemoveViewColumns( aTransID,
                                               aStmt,
                                               sViewColumnList,
                                               ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Inner Sequence List 에 대한 Refine Cache
     */

    STL_TRY( ellRefineCacheDropSeqByObjectList( aTransID,
                                                aStmt,
                                                sInnerSeqList,
                                                ELL_ENV(aEnv) )
             == STL_SUCCESS );

     /**
     * Inner Synonym List 에 대한 Refine Cache
     */

    STL_TRY( ellRefineCacheDropSynonymByObjectList( aTransID,
                                                    aStmt,
                                                    sInnerSynonymList,
                                                    ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Inner Foreign Key List 에 대한 Refine Cache
     */

    STL_TRY( qlrRefineParentList4DropForeignKeyList( aTransID,
                                                     aStmt,
                                                     sInnerForeignKeyList,
                                                     sInnerForeignKeyParentTableList,
                                                     aEnv )
             == STL_SUCCESS );
    
    STL_TRY( qlrRefineCache4DropKeyList( aTransID,
                                         aStmt,
                                         sInnerForeignKeyList,
                                         STL_FALSE,  /* aKeepIndex */
                                         sInnerForeignKeyIndexList,
                                         sInnerForeignKeyChildTableList,
                                         aEnv )
             == STL_SUCCESS );

    /**
     * Outer Foreign Key List 에 대한 Refine Cache
     */

    STL_TRY( qlrRefineParentList4DropForeignKeyList( aTransID,
                                                     aStmt,
                                                     sOuterForeignKeyList,
                                                     sOuterForeignKeyParentTableList,
                                                     aEnv )
             == STL_SUCCESS );
    
    STL_TRY( qlrRefineCache4DropKeyList( aTransID,
                                         aStmt,
                                         sOuterForeignKeyList,
                                         STL_FALSE,  /* aKeepIndex */
                                         sOuterForeignKeyIndexList,
                                         sOuterForeignKeyChildTableList,
                                         aEnv )
             == STL_SUCCESS );
    
    /**
     * Outer Table Inner Unique Key List 에 대한 Refine Cache
     */

    STL_TRY( qlrRefineCache4DropKeyList( aTransID,
                                         aStmt,
                                         sOuterTableInnerUniqueKeyList,
                                         STL_FALSE,  /* aKeepIndex */
                                         sOuterTableInnerUniqueKeyIndexList,
                                         sOuterTableInnerUniqueKeyTableList,
                                         aEnv )
             == STL_SUCCESS );
    
    /**
     * Outer Table Inner Index List 에 대한 Refine Cache
     */

    STL_TRY( qliRefineCache4DropIndexList( aTransID,
                                           aStmt,
                                           sOuterTableInnerIndexList,
                                           sOuterTableInnerIndexTableList,
                                           aEnv )
             == STL_SUCCESS );

    /**
     * Outer Table Inner Check Clause List 에 대한 Refine Cache
     */

    STL_TRY( qlrRefineCache4DropCheckClauseList( aTransID,
                                                 aStmt,
                                                 sOuterTableInnerCheckClauseList,
                                                 sOuterTableInnerCheckClauseTableList,
                                                 aEnv )
             == STL_SUCCESS );

    /**
     * Outer Table Inner Primary Key List 에 대한 Refine Cache
     * - Table 에 종속된 모든 Cache 가 Rebuild 됨.
     */

    STL_TRY( qlrRefineCache4DropKeyList( aTransID,
                                         aStmt,
                                         sOuterTableInnerPrimaryKeyList,
                                         STL_FALSE,  /* aKeepIndex */
                                         sOuterTableInnerPrimaryKeyIndexList,
                                         sOuterTableInnerPrimaryKeyTableList,
                                         aEnv )
             == STL_SUCCESS );
    
    /**
     * Outer Table Inner Not Null List 에 대한 Refine Cache
     * - Table 에 종속된 모든 Cache 가 Rebuild 됨.
     * - Outer Table Inner Primary Key List 에 포함되지 않은 Table List 만 Rebuild 해야 함.
     */

    /*
     * Outer Not Null List 중 Outer Primary Key List 에 포함된 내용을 제거 
     */

    STL_TRY( ellInitObjectList( & sRemainNotNullList,
                                & QLL_EXEC_DDL_MEM(aEnv),
                                ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellInitObjectList( & sRemainNotNullTableList,
                                & QLL_EXEC_DDL_MEM(aEnv),
                                ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    sTableItem = STL_RING_GET_FIRST_DATA( & sOuterTableInnerNotNullTableList->mHeadList );
    
    STL_RING_FOREACH_ENTRY( & sOuterTableInnerNotNullList->mHeadList, sNotNullItem )
    {
        sNotNullHandle = & sNotNullItem->mObjectHandle;

        sTableHandle = & sTableItem->mObjectHandle;

        if ( ellObjectExistInObjectList( ELL_OBJECT_TABLE,
                                         ellGetTableID( sTableHandle ),
                                         sOuterTableInnerPrimaryKeyTableList )
             == STL_TRUE )
        {
            /**
             * 이미 Rebuild 된 Table Cache 임.
             */
        }
        else
        {
            /**
             * Rebuild 되지 않은 Table Cache 임.
             */

            STL_TRY( ellAppendObjectItem( sRemainNotNullList,
                                          sNotNullHandle,
                                          & QLL_EXEC_DDL_MEM(aEnv),
                                          ELL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            STL_TRY( ellAppendObjectItem( sRemainNotNullTableList,
                                          sTableHandle,
                                          & QLL_EXEC_DDL_MEM(aEnv),
                                          ELL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
        
        sTableItem = STL_RING_GET_NEXT_DATA( & sOuterTableInnerNotNullTableList->mHeadList, sTableItem );
    }

    /*
     * Remain Not Null Table List 에 대한 Refine Cache
     */
    
    STL_TRY( qlrRefineCache4DropNotNullList( aTransID,
                                             aStmt,
                                             sRemainNotNullList,
                                             sRemainNotNullTableList,
                                             aEnv )
             == STL_SUCCESS );
    
    
    /**
     * Inner Table List 에 대한 Refine Cache
     */

    STL_TRY( ellRefineCacheDropTablePrime4TableList( aTransID,
                                                     aStmt,
                                                     sInnerTableList,
                                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Schema 에 대한 Refine Cache
     */

    STL_TRY( ellRefineCache4DropSchema( aTransID,
                                        & sInitPlan->mSchemaHandle,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * @todo Authorization List 에 대한 Refine Cache
     */
    
    /**
     * If Exists 옵션을 사용하고, Schema 가 존재하지 않는 경우
     */
    
    STL_RAMP( RAMP_IF_EXISTS_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SCHEMA_NOT_EMPTY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_NOT_EMPTY,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }
    
    /* STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED ) */
    /* { */
    /*     stlPushError( STL_ERROR_LEVEL_ABORT, */
    /*                   QLL_ERRCODE_NOT_IMPLEMENTED, */
    /*                   NULL, */
    /*                   QLL_ERROR_STACK(aEnv), */
    /*                   "qlsExecuteDropSchema()" ); */
    /* } */
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief DROP SCHEMA/USER 를 위해 Outer Table List 의 Column 들에 대한 Nullable 정보를 설정한다.
 * @param[in] aTransID                   Transaction ID
 * @param[in] aStmt                      Statement
 * @param[in] aRegionMem                 Region Memory
 * @param[in] aObjectType                SCHEMA or AUTHORIZATION
 * @param[in] aObjectHandle              Object Handle
 * @param[in] aPrimaryKeyOuterTableList  Inner Primary Key 에 대응하는 Outer Table List
 * @param[in] aNotNullOuterTableList     Inner Not Null 에 대응하는 Outer Table List
 * @param[in] aEnv                       Environment
 * @remarks
 */
stlStatus qlsSetOuterTableColumnNullable4DropNonSchemaObject( smlTransId      aTransID,
                                                              smlStatement  * aStmt,
                                                              knlRegionMem  * aRegionMem,
                                                              ellObjectType   aObjectType,
                                                              ellDictHandle * aObjectHandle,
                                                              ellObjectList * aPrimaryKeyOuterTableList,
                                                              ellObjectList * aNotNullOuterTableList,
                                                              qllEnv        * aEnv )
{
    stlInt64  sObjectID = ELL_DICT_OBJECT_ID_NA;

    ellObjectList * sOuterTableList = NULL;

    ellObjectItem   * sTableItem = NULL;
    ellDictHandle   * sTableHandle = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aObjectType == ELL_OBJECT_SCHEMA) || (aObjectType == ELL_OBJECT_AUTHORIZATION)
                        , QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aObjectHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPrimaryKeyOuterTableList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNotNullOuterTableList != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    if ( aObjectType == ELL_OBJECT_SCHEMA )
    {
        sObjectID = ellGetSchemaID( aObjectHandle );
    }
    else
    {
        sObjectID = ellGetAuthID( aObjectHandle );
    }
    
    /**
     * Outer Table List 초기화 
     */

    STL_TRY( ellInitObjectList( & sOuterTableList, aRegionMem, ELL_ENV(aEnv) ) == STL_SUCCESS );

    /**
     * aPrimaryKeyOuterTableList 를 순회하며, Outer Table List 에 추가 
     */
    
    STL_RING_FOREACH_ENTRY( & aPrimaryKeyOuterTableList->mHeadList, sTableItem )
    {
        sTableHandle = & sTableItem->mObjectHandle;

        if ( aObjectType == ELL_OBJECT_SCHEMA )
        {
            STL_ASSERT( ellGetTableSchemaID(sTableHandle) != sObjectID );
        }
        else
        {
            STL_ASSERT( ellGetTableOwnerID(sTableHandle) != sObjectID );
        }
        
        /**
         * 중복되지 않도록 Outer Table List 에 추가 
         */
        
        STL_TRY( ellAddNewObjectItem( sOuterTableList,
                                      sTableHandle,
                                      NULL,
                                      aRegionMem,
                                      ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    /**
     * aNotNullOuterTableList 를 순회하며, Outer Table List 에 추가 
     */
    
    STL_RING_FOREACH_ENTRY( & aNotNullOuterTableList->mHeadList, sTableItem )
    {
        sTableHandle = & sTableItem->mObjectHandle;

        if ( aObjectType == ELL_OBJECT_SCHEMA )
        {
            STL_ASSERT( ellGetTableSchemaID(sTableHandle) != sObjectID );
        }
        else
        {
            STL_ASSERT( ellGetTableOwnerID(sTableHandle) != sObjectID );
        }
        
        /**
         * 중복되지 않도록 Outer Table List 에 추가 
         */
        
        STL_TRY( ellAddNewObjectItem( sOuterTableList,
                                      sTableHandle,
                                      NULL,
                                      aRegionMem,
                                      ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    /**
     * Outer Table List 를 순회하며, Column Nullable 정보를 설정
     */
    
    STL_RING_FOREACH_ENTRY( & sOuterTableList->mHeadList, sTableItem )
    {
        sTableHandle = & sTableItem->mObjectHandle;

        STL_TRY( qlsSetTableColumnNullable4DropNonSchemaObject( aTransID,
                                                                aStmt,
                                                                aRegionMem,
                                                                aObjectType,
                                                                aObjectHandle,
                                                                sTableHandle,
                                                                aEnv )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief DROP SCHEMA/USER 를 위해 하나의 Outer Table 의 Column 들에 대한 Nullable 정보를 설정한다.
 * @param[in] aTransID                   Transaction ID
 * @param[in] aStmt                      Statement
 * @param[in] aRegionMem                 Region Memory
 * @param[in] aObjectType                SCHEMA or AUTHORIZATION
 * @param[in] aObjectHandle              Object Handle
 * @param[in] aOuterTableHandle          Outer Table Handle
 * @param[in] aEnv                       Environment
 * @remarks
 */
stlStatus qlsSetTableColumnNullable4DropNonSchemaObject( smlTransId      aTransID,
                                                         smlStatement  * aStmt,
                                                         knlRegionMem  * aRegionMem,
                                                         ellObjectType   aObjectType,
                                                         ellDictHandle * aObjectHandle,
                                                         ellDictHandle * aOuterTableHandle,
                                                         qllEnv        * aEnv )
{
    stlInt64  sObjectID = ELL_DICT_OBJECT_ID_NA;
    stlInt64  sConstParentID = ELL_DICT_OBJECT_ID_NA;

    stlInt32        sColumnCnt = 0;
    ellDictHandle * sColumnHandle = NULL;
    stlInt64        sColumnID = ELL_DICT_OBJECT_ID_NA;

    stlInt32        sPKCnt = 0;
    ellDictHandle * sPKHandle = NULL;

    stlInt32        sPKColumnCnt = 0;
    ellDictHandle * sPKColumnHandle = NULL;
    
    stlInt32        sNotNullCnt = 0;
    ellDictHandle * sNotNullHandle = NULL;
    ellDictHandle * sNotNullColumnHandle = NULL;

    stlBool   sFound = STL_FALSE;
    
    stlInt32  i = 0;
    stlInt32  j = 0;

    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aObjectType == ELL_OBJECT_SCHEMA) || (aObjectType == ELL_OBJECT_AUTHORIZATION)
                        , QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aObjectHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOuterTableHandle != NULL, QLL_ERROR_STACK(aEnv) );

    /**************************************************************
     * 관련 정보 획득 
     **************************************************************/
    
    /**
     * 기본 정보 획득
     */

    if ( aObjectType == ELL_OBJECT_SCHEMA )
    {
        sObjectID = ellGetSchemaID( aObjectHandle );
    }
    else
    {
        sObjectID = ellGetAuthID( aObjectHandle );
    }

    /**
     * 컬럼 정보 획득
     */
    
    sColumnCnt    = ellGetTableColumnCnt( aOuterTableHandle );
    sColumnHandle = ellGetTableColumnDictHandle( aOuterTableHandle );

    /**
     * Primary Key 정보 획득
     */
    
    STL_TRY( ellGetTablePrimaryKeyDictHandle( aTransID,
                                              ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                              aRegionMem,
                                              aOuterTableHandle,
                                              & sPKCnt,
                                              & sPKHandle,
                                              ELL_ENV(aEnv) )
             == STL_SUCCESS );

    if ( sPKCnt > 0 )
    {
        sPKColumnCnt = ellGetPrimaryKeyColumnCount( sPKHandle );
        sPKColumnHandle = ellGetPrimaryKeyColumnHandle( sPKHandle );
    }
    else
    {
        sPKColumnCnt = 0;
        sPKColumnHandle = NULL;
    }

    /**
     * Not Null 정보 획득
     */
    
    STL_TRY( ellGetTableCheckNotNullDictHandle( aTransID,
                                                ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                                aRegionMem,
                                                aOuterTableHandle,
                                                & sNotNullCnt,
                                                & sNotNullHandle,
                                                ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**************************************************************
     * 컬럼의 Nullable 정보 획득 
     **************************************************************/
    
    /**
     * 컬럼을 순회하며 Not Null => Nullable 로 변경
     */
    
    for ( i = 0; i < sColumnCnt; i++ )
    {
        sColumnID = ellGetColumnID( & sColumnHandle[i] );
        
        /**
         * Column 이 Not Null 인지 확인
         */

        if ( ellGetColumnNullable( & sColumnHandle[i] ) == STL_TRUE )
        {
            /**
             * Nullable 컬럼으로 변경되지 않음
             */
            
            continue;
        }

        /**
         * Column 이 Identity Column 인지 확인
         */

        if ( ellGetColumnIdentityHandle( & sColumnHandle[i] ) != NULL )
        {
            /**
             * Not Null 상태가 유지됨
             */
            continue;
        }
        
        /**
         * 삭제되지 않는 Primary Key 에 포함되는 지 확인 
         */

        if ( sPKCnt > 0 )
        {
            if ( aObjectType == ELL_OBJECT_SCHEMA )
            {
                sConstParentID = ellGetConstraintSchemaID( sPKHandle );
            }
            else
            {
                sConstParentID = ellGetConstraintOwnerID( sPKHandle );
            }
            
            if ( sConstParentID == sObjectID )
            {
                /**
                 * 삭제될 Primary Key 임
                 */
            }
            else
            {
                sFound = STL_FALSE;
                
                for ( j = 0; j < sPKColumnCnt; j++ )
                {
                    if ( sColumnID == ellGetColumnID( & sPKColumnHandle[j] ) )
                    {
                        sFound = STL_TRUE;
                        break;
                    }
                    else
                    {
                        continue;
                    }
                }

                if ( sFound == STL_TRUE )
                {
                    /**
                     * 삭제되지 않을 Primary Key 에 포함된 컬럼임
                     */
                    continue;
                }
                else
                {
                    /* go go */
                }
            }
        }
        else
        {
            /**
             * Primary Key 가 없음
             */
        }
        
        /**
         * 삭제되지 않는 Not Null Constraint 에 포함되는 컬럼인지 확인
         */

        sFound = STL_FALSE;
        
        for ( j = 0; j < sNotNullCnt; j++ )
        {
            sNotNullColumnHandle = ellGetCheckNotNullColumnHandle( & sNotNullHandle[j] );
            
            if ( sColumnID == ellGetColumnID( sNotNullColumnHandle ) )
            {
                /**
                 * Not Null Constraint 에 포함됨
                 */
                
                if ( aObjectType == ELL_OBJECT_SCHEMA )
                {
                    sConstParentID = ellGetConstraintSchemaID( & sNotNullHandle[j] );
                }
                else
                {
                    sConstParentID = ellGetConstraintOwnerID( & sNotNullHandle[j] );
                }
                
                if ( sConstParentID == sObjectID )
                {
                    /**
                     * 삭제될 Not Null Constraint 임
                     */
                    
                }
                else
                {
                    /**
                     * 삭제되지 않을 Not Null Constraint 임
                     * - Not Null 이 유지됨 
                     */

                    sFound = STL_TRUE;
                }
                
                break;;
            }
            else
            {
                continue;
            }
        }

        /**
         * Column 의 Nullable 속성을 변경: NOT NULL => Nullable
         */

        if ( sFound == STL_TRUE )
        {
            /**
             * 삭제되지 않는 Not Null Constraint 에 포함됨
             */
        }
        else
        {
            STL_TRY( ellModifyColumnNullable( aTransID,
                                              aStmt,
                                              sColumnID,
                                              STL_TRUE,  /* aIsNullable */
                                              ELL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} qlsDropSchema */
