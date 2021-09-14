/*******************************************************************************
 * qlaDropUser.c
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
 * @file qlaDropUser.c
 * @brief DROP USER 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qlaDropUser DROP USER
 * @ingroup qla
 * @{
 */


/**
 * @brief DROP USER 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlaValidateDropUser( smlTransId      aTransID,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               stlChar       * aSQLString,
                               qllNode       * aParseTree,
                               qllEnv        * aEnv )
{
    stlInt32  sState = 0;
    
    ellDictHandle    * sAuthHandle = NULL;
    
    qlaInitDropUser  * sInitPlan = NULL;
    qlpDropUser      * sParseTree = NULL;
    
    stlBool             sObjectExist = STL_FALSE;

    ellObjectList * sSchemaList = NULL;
    
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
    ellObjectList * sPrivObjectList = NULL;

    smlStatement  * sStmt = NULL;
    stlInt32        sStmtAttr = 0;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_DROP_USER_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_DROP_USER_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */

    sParseTree = (qlpDropUser *) aParseTree;
    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlaInitDropUser),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlaInitDropUser) );

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
     * User name 검증 
     *************************************************/

    /**
     * mIfExists
     */

    sInitPlan->mIfExists = sParseTree->mIfExists;
    
    /**
     * User 존재 여부
     */

    STL_TRY( ellGetAuthDictHandle( aTransID,
                                   aSQLStmt->mViewSCN,
                                   QLP_GET_PTR_VALUE( sParseTree->mName ),
                                   & sInitPlan->mUserHandle,
                                   & sObjectExist,
                                   ELL_ENV(aEnv) )
             == STL_SUCCESS );

    if ( sObjectExist == STL_TRUE )
    {
        /**
         * User 가 존재함
         */
        
        sInitPlan->mUserExist = STL_TRUE;
    }
    else
    {
        /**
         * Schema 가 없음
         */
        
        sInitPlan->mUserExist = STL_FALSE;
            
        /**
         * If Exists 옵션이 있다면 무조건 SUCCESS 한다.
         */
        
        if ( sInitPlan->mIfExists == STL_TRUE )
        {
            STL_THROW( RAMP_IF_EXISTS_NO_USER );
        }
        else
        {
            STL_THROW( RAMP_ERR_USER_NOT_EXIST );
        }
    }

    /*************************************************
     * USER 검증 
     *************************************************/

    /**
     * USER 인지 검사, role ..
     */

    STL_TRY_THROW( ellGetAuthType( & sInitPlan->mUserHandle ) == ELL_AUTHORIZATION_TYPE_USER,
                   RAMP_ERR_USER_NOT_EXIST );
    
    /**
     * System User 검사
     */

    STL_TRY_THROW( ellIsBuiltInAuth( & sInitPlan->mUserHandle ) == STL_FALSE,
                   RAMP_ERR_DROP_BUILTIN_AUTH );

    /**
     *  자기 자신인지 검사 
     */

    STL_TRY_THROW( ellGetAuthID( & sInitPlan->mUserHandle ) != ellGetAuthID(sAuthHandle),
                   RAMP_ERR_DROP_SELF_AUTH );
    
    /**
     * DROP USER 권한 검사
     */

    STL_TRY( qlaCheckPrivDropUser( aTransID,
                                   aDBCStmt,
                                   aSQLStmt,
                                   aEnv )
             == STL_SUCCESS );
    
    /*************************************************
     * Own Schema 존재 여부 검사 
     *************************************************/

    /**
     * User 가 소유한 Schema 가 존재하는 지 검사
     */

    STL_TRY( ellGetUserOwnedSchemaList( aTransID,
                                        sStmt,
                                        & sInitPlan->mUserHandle,
                                        QLL_INIT_PLAN(aDBCStmt),
                                        & sSchemaList,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( ellHasObjectItem( sSchemaList ) == STL_FALSE,
                   RAMP_ERR_USER_HAS_OWN_SCHEMA );
                                        
    /*************************************************
     * Option Validation
     *************************************************/

    sInitPlan->mIsCascade = sParseTree->mIsCascade;
    
    /**
     * Drop User 관련 객체 정보 획득
     */

    STL_TRY( ellGetObjectList4DropUser( aTransID,
                                        sStmt,
                                        & sInitPlan->mUserHandle,
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
                                        & sPrivObjectList,
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
         * User 가 소유한 객체가 존재하지 않아야 함
         */

        STL_TRY_THROW( ellHasObjectItem( sInnerSeqList ) == STL_FALSE,
                       RAMP_ERR_USER_NOT_EMPTY );

        STL_TRY_THROW( ellHasObjectItem( sInnerSynonymList ) == STL_FALSE,
                       RAMP_ERR_USER_NOT_EMPTY );
        
        STL_TRY_THROW( ellHasObjectItem( sInnerTableList ) == STL_FALSE,
                       RAMP_ERR_USER_NOT_EMPTY );

        STL_TRY_THROW( ellHasObjectItem( sOuterTableInnerPrimaryKeyList ) == STL_FALSE,
                       RAMP_ERR_USER_NOT_EMPTY );

        STL_TRY_THROW( ellHasObjectItem( sOuterTableInnerUniqueKeyList ) == STL_FALSE,
                       RAMP_ERR_USER_NOT_EMPTY );

        STL_TRY_THROW( ellHasObjectItem( sOuterTableInnerIndexList ) == STL_FALSE,
                       RAMP_ERR_USER_NOT_EMPTY );

        STL_TRY_THROW( ellHasObjectItem( sOuterTableInnerNotNullList ) == STL_FALSE,
                       RAMP_ERR_USER_NOT_EMPTY );

        STL_TRY_THROW( ellHasObjectItem( sOuterTableInnerCheckClauseList ) == STL_FALSE,
                       RAMP_ERR_USER_NOT_EMPTY );
        
        STL_TRY_THROW( ellHasObjectItem( sInnerForeignKeyList ) == STL_FALSE,
                       RAMP_ERR_USER_NOT_EMPTY );
    }

    /********************************************************
     * Validation Object 추가 
     ********************************************************/

    STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                  & sInitPlan->mUserHandle,
                                  QLL_INIT_PLAN(aDBCStmt),
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

    
    /*************************************************
     * Plan 연결 
     *************************************************/

    /**
     * IF EXISTS 구문이 있을 경우, User 가 존재하지 않는다면,
     * 수행없이 SUCCESS 하게 되고, 권한 검사 및 Writable 은 검사하지 않는다.
     */
    
    STL_RAMP( RAMP_IF_EXISTS_NO_USER );

    /**
     * Statement 해제
     */

    sState = 0;
    STL_TRY( smlFreeStatement( sStmt, SML_ENV(aEnv) ) == STL_SUCCESS );
    
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
                                           sParseTree->mName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      QLP_GET_PTR_VALUE( sParseTree->mName ) );
    }

    STL_CATCH( RAMP_ERR_DROP_BUILTIN_AUTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_BUILT_IN_AUTHORIZATION_IS_NOT_MODIFIABLE,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      QLP_GET_PTR_VALUE( sParseTree->mName ) );
    }

    STL_CATCH( RAMP_ERR_DROP_SELF_AUTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CANNOT_DROP_USER_CURRENTLY_CONNECTED,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_CATCH( RAMP_ERR_USER_HAS_OWN_SCHEMA )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_USER_HAS_OWN_SCHEMA,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_USER_NOT_EMPTY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CASCADE_MUST_BE_SPECIFIED_TO_DROP_USER,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      QLP_GET_PTR_VALUE( sParseTree->mName ) );
    }
    
    /* STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED ) */
    /* { */
    /*     stlPushError( STL_ERROR_LEVEL_ABORT, */
    /*                   QLL_ERRCODE_NOT_IMPLEMENTED, */
    /*                   NULL, */
    /*                   QLL_ERROR_STACK(aEnv), */
    /*                   "qlaValidateDropUser()" ); */
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
 * @brief DROP USER 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlaOptCodeDropUser( smlTransId      aTransID,
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
 * @brief DROP USER 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlaOptDataDropUser( smlTransId      aTransID,
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
 * @brief DROP USER 구문을 Execute 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aStmt      Statement
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlaExecuteDropUser( smlTransId      aTransID,
                              smlStatement  * aStmt,
                              qllDBCStmt    * aDBCStmt,
                              qllStatement  * aSQLStmt,
                              qllEnv        * aEnv )
{
    qlaInitDropUser  * sInitPlan = NULL;
    stlInt64           sUserID = ELL_DICT_OBJECT_ID_NA;

    ellObjectList * sSchemaList = NULL;
    
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

    ellObjectList * sPrivObjectList = NULL;
    
    ellObjectList * sRemainNotNullList = NULL;
    ellObjectList * sRemainNotNullTableList = NULL;

    ellObjectItem   * sNotNullItem   = NULL;
    ellDictHandle   * sNotNullHandle = NULL;

    ellObjectItem   * sTableItem   = NULL;
    ellDictHandle   * sTableHandle = NULL;


    ellPrivList  * sUserPrivList = NULL;
    
    ellPrivItem  * sPrivItem = NULL;

    
    stlTime  sTime = 0;

    stlBool   sLocked = STL_TRUE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_DROP_USER_TYPE,
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

    sInitPlan = (qlaInitDropUser *) aSQLStmt->mInitPlan;

    /**
     * IF EXISTS 옵션 처리
     */

    if ( (sInitPlan->mIfExists == STL_TRUE) &&
         (sInitPlan->mUserExist != STL_TRUE) )
    {
        /**
         * IF EXISTS 옵션을 사용하고, User 가 존재하지 않는 경우
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

    STL_TRY( ellLock4DropUser( aTransID,
                               aStmt,
                               & sInitPlan->mUserHandle,
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

    sInitPlan = (qlaInitDropUser *) aSQLStmt->mInitPlan;

    /***************************************************************
     * 관련 정보 획득 및 Lock 획득
     ***************************************************************/
    
    /**
     * User 가 소유한 Schema 가 존재하는 지 검사
     */

    sUserID = ellGetAuthID( & sInitPlan->mUserHandle );
    
    STL_TRY( ellGetUserOwnedSchemaList( aTransID,
                                        aStmt,
                                        & sInitPlan->mUserHandle,
                                        & QLL_EXEC_DDL_MEM(aEnv),
                                        & sSchemaList,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( ellHasObjectItem( sSchemaList ) == STL_FALSE,
                   RAMP_ERR_USER_HAS_OWN_SCHEMA );
                                        
    /**
     * Drop User 관련 객체 정보 획득
     */

    STL_TRY( ellGetObjectList4DropUser( aTransID,
                                        aStmt,
                                        & sInitPlan->mUserHandle,
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
                                        & sPrivObjectList,
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
         * User 가 소유한 객체가 존재하지 않아야 함
         */

        STL_TRY_THROW( ellHasObjectItem( sInnerSeqList ) == STL_FALSE,
                       RAMP_ERR_USER_NOT_EMPTY );

        STL_TRY_THROW( ellHasObjectItem( sInnerSynonymList ) == STL_FALSE,
                       RAMP_ERR_USER_NOT_EMPTY );
        
        STL_TRY_THROW( ellHasObjectItem( sInnerTableList ) == STL_FALSE,
                       RAMP_ERR_USER_NOT_EMPTY );

        STL_TRY_THROW( ellHasObjectItem( sOuterTableInnerPrimaryKeyList ) == STL_FALSE,
                       RAMP_ERR_USER_NOT_EMPTY );

        STL_TRY_THROW( ellHasObjectItem( sOuterTableInnerUniqueKeyList ) == STL_FALSE,
                       RAMP_ERR_USER_NOT_EMPTY );

        STL_TRY_THROW( ellHasObjectItem( sOuterTableInnerIndexList ) == STL_FALSE,
                       RAMP_ERR_USER_NOT_EMPTY );

        STL_TRY_THROW( ellHasObjectItem( sOuterTableInnerNotNullList ) == STL_FALSE,
                       RAMP_ERR_USER_NOT_EMPTY );

        STL_TRY_THROW( ellHasObjectItem( sOuterTableInnerCheckClauseList ) == STL_FALSE,
                       RAMP_ERR_USER_NOT_EMPTY );
        
        STL_TRY_THROW( ellHasObjectItem( sInnerForeignKeyList ) == STL_FALSE,
                       RAMP_ERR_USER_NOT_EMPTY );
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
     * Outer Table Inner Unique Key 의 Table List 에 X lock 획득 
     */
    
    STL_TRY( ellLock4ConstraintList( aTransID,
                                     aStmt,
                                     sOuterTableInnerUniqueKeyList,
                                     & sLocked,
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );

    /**
     * Outer Table Inner Index 의 Table List 에 X lock 획득 
     */
    
    STL_TRY( ellLock4IndexList( aTransID,
                                aStmt,
                                sOuterTableInnerIndexList,
                                & sLocked,
                                ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    /**
     * Outer Table Inner Not Null List 의 Table List 에 X lock 획득 
     */

    STL_TRY( ellLock4ConstraintList( aTransID,
                                     aStmt,
                                     sOuterTableInnerNotNullList,
                                     & sLocked,
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    /**
     * Outer Table Inner Check List 의 Table List 에 X lock 획득 
     */
    
    STL_TRY( ellLock4ConstraintList( aTransID,
                                     aStmt,
                                     sOuterTableInnerCheckClauseList,
                                     & sLocked,
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    /**
     * Inner Foreign Key List 의 Table List 에 X lock 획득 
     */
    
    STL_TRY( ellLock4ConstraintList( aTransID,
                                     aStmt,
                                     sInnerForeignKeyList,
                                     & sLocked,
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    /**
     * Outer Foreign Key List 의 Table List 에 X lock 획득 
     */
    
    STL_TRY( ellLock4ConstraintList( aTransID,
                                     aStmt,
                                     sOuterForeignKeyList,
                                     & sLocked,
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    /**
     * User 가 Grantor/Grantee 인 Object 에 대해 S 또는 IS lock 을 획득
     */

    STL_TRY( ellLock4PrivObjectList( aTransID,
                                     aStmt,
                                     sPrivObjectList,
                                     & sLocked,
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    /**
     * Lock 을 모두 획득 후 Drop User 관련 객체 정보를 다시 획득
     */

    STL_TRY( ellGetObjectList4DropUser( aTransID,
                                        aStmt,
                                        & sInitPlan->mUserHandle,
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
                                        & sPrivObjectList,
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
     * Inner Relation 의 영향을 받는 Outer View List 획득 및 X lock 획득
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
     * Outer Table Inner Primary Key 에 대한 Lock 획득 
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

    /* primary key 의 index 획득 */
    STL_TRY( ellGetIndexList4KeyList( aTransID,
                                      sOuterTableInnerPrimaryKeyList,
                                      & QLL_EXEC_DDL_MEM(aEnv),
                                      & sOuterTableInnerPrimaryKeyIndexList,
                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Outer Table Inner Unique Key 의 Table List 에 S lock 획득 
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
    
    /* unique key 의 index 획득 */
    STL_TRY( ellGetIndexList4KeyList( aTransID,
                                      sOuterTableInnerUniqueKeyList,
                                      & QLL_EXEC_DDL_MEM(aEnv),
                                      & sOuterTableInnerUniqueKeyIndexList,
                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Outer Table Inner Index 와 관련된 정보 획득 및 S lock 획득
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
     * Outer Table Inner Not Null 와 관련된 정보 획득 및 X lock 획득 
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
     * Outer Table Inner Check Clause 와 관련된 정보 획득 및 S lock 획득 
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
     * Inner Foreign Key 와 관련된 Parent Table 정보 획득 및 S lock 획득 
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
     * Inner Foreign Key 와 관련된 Child Table 정보 획득 및 S lock 획득 
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
    

    /* foreign key 의 index 획득 */
    STL_TRY( ellGetIndexList4KeyList( aTransID,
                                      sInnerForeignKeyList,
                                      & QLL_EXEC_DDL_MEM(aEnv),
                                      & sInnerForeignKeyIndexList,
                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Outer Foreign Key 와 관련된 Parent Table 정보 획득 및 S lock 획득 
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
     * Outer Foreign Key 와 관련된 Child Table 정보 획득 및 S lock 획득 
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

    /* foreign key 의 index 획득 */
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
                                                                 ELL_OBJECT_AUTHORIZATION,
                                                                 & sInitPlan->mUserHandle,
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
     * User 가 Grantee 인 Privilege 제거 
     */

    STL_TRY( ellRevokeAuthGranteeAllPrivilege( aTransID,
                                               aStmt,
                                               ellGetAuthID( & sInitPlan->mUserHandle ),
                                               ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * User 의 Dictionary Record 삭제 
     */

    STL_TRY( ellRemoveDict4DropUser( aTransID,
                                     aStmt,
                                     & sInitPlan->mUserHandle,
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
     * User 에 대한 Refine Cache
     */

    STL_TRY( ellRefineCache4DropUser( aTransID,
                                      & sInitPlan->mUserHandle,
                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /***************************************************************
     * User 가 Grantor 인 남아 있는 Privilege 제거
     ***************************************************************/
    
    /**
     * User 가 부여한 Privilege 목록 획득
     */

    STL_TRY( ellGetAuthGrantorPrivList( aTransID,
                                        aStmt,
                                        sUserID,
                                        & sUserPrivList,
                                        & QLL_EXEC_DDL_MEM(aEnv),
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );
                                        
                                        
    /**
     * User 가 부여한 Privilege 를 순회하며 Privilege 와 Dependent Privilege Descriptor 를 제거한다.
     */
    
    STL_RING_FOREACH_ENTRY( & sUserPrivList->mHeadList, sPrivItem )
    {
        /**
         * Depedent Privilege Descriptor 를 제거한다.
         */

        STL_TRY( qlaRevokeDependentPriv( aTransID,
                                         aStmt,
                                         QLP_REVOKE_CASCADE_CONSTRAINTS,
                                         STL_FALSE,  /* aGrantOption */
                                         & sPrivItem->mPrivDesc,
                                         & QLL_EXEC_DDL_MEM(aEnv),
                                         aEnv )
                 == STL_SUCCESS );
    }
    

    /**
     * If Exists 옵션을 사용하고, User 가 존재하지 않는 경우
     */
    
    
    STL_RAMP( RAMP_IF_EXISTS_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_USER_HAS_OWN_SCHEMA )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_USER_HAS_OWN_SCHEMA,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_USER_NOT_EMPTY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CASCADE_MUST_BE_SPECIFIED_TO_DROP_USER,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      ellGetAuthName( & sInitPlan->mUserHandle ) );
    }
    
    /* STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED ) */
    /* { */
    /*     stlPushError( STL_ERROR_LEVEL_ABORT, */
    /*                   QLL_ERRCODE_NOT_IMPLEMENTED, */
    /*                   NULL, */
    /*                   QLL_ERROR_STACK(aEnv), */
    /*                   "qlaExecuteDropUser()" ); */
    /* } */
    
    STL_FINISH;

    return STL_FAILURE;
}




/** @} qlaDropUser */
