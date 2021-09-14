/*******************************************************************************
 * qlaRevokePriv.c
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
 * @file qlaRevokePriv.c
 * @brief REVOKE PRIVILEGE 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qlaRevokePriv REVOKE PRIVILEGE
 * @ingroup qla
 * @{
 */


/**
 * @brief REVOKE PRIVILEGE 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlaValidateRevokePriv( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 stlChar       * aSQLString,
                                 qllNode       * aParseTree,
                                 qllEnv        * aEnv )
{
    stlInt32  sState = 0;
    
    qlpRevokePriv        * sParseTree = NULL;
    qlaInitRevokePriv    * sInitPlan = NULL;

    smlStatement  * sStmt = NULL;
    stlInt32        sStmtAttr = 0;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_REVOKE_DATABASE_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_REVOKE_TABLESPACE_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_REVOKE_SCHEMA_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_REVOKE_TABLE_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_REVOKE_USAGE_TYPE) ,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aParseTree->mType == QLL_STMT_REVOKE_DATABASE_TYPE) ||
                        (aParseTree->mType == QLL_STMT_REVOKE_TABLESPACE_TYPE) ||
                        (aParseTree->mType == QLL_STMT_REVOKE_SCHEMA_TYPE) ||
                        (aParseTree->mType == QLL_STMT_REVOKE_TABLE_TYPE) ||
                        (aParseTree->mType == QLL_STMT_REVOKE_USAGE_TYPE) ,
                        QLL_ERROR_STACK(aEnv) );
    
    /**
     * 기본 정보 획득 
     */
    
    sParseTree = (qlpRevokePriv *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlaInitRevokePriv),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlaInitRevokePriv) );

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
    
    /**
     * 별도의 구문 권한 검사 없음
     */

    /**********************************************************
     * Privilege Object 정보 설정
     **********************************************************/

    sInitPlan->mPrivObject = sParseTree->mPrivilege->mPrivObject->mPrivObjType;

    STL_TRY( qlaGetPrivObject( aTransID,
                               aDBCStmt,
                               aSQLStmt,
                               aSQLString,
                               sParseTree->mPrivilege->mPrivObject,
                               sParseTree->mRevokeeList,
                               & sInitPlan->mObjectHandle,
                               aEnv )
             == STL_SUCCESS );
    
    /**********************************************************
     * Revoker 의 ACCESS CONTROL 권한 존재 여부
     **********************************************************/

    STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                      aSQLStmt->mViewSCN,
                                      ELL_PRIV_DATABASE,
                                      ellGetDbCatalogID(),
                                      ELL_DB_PRIV_ACTION_ACCESS_CONTROL,
                                      STL_FALSE, /* aWithGrant */
                                      & sInitPlan->mControlHandle,
                                      & sInitPlan->mHasControl,
                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**********************************************************
     * Revokee List 정보 설정
     **********************************************************/

    STL_TRY( qlaGetGranteeList( aTransID,
                                aDBCStmt,
                                aSQLStmt,
                                aSQLString,
                                sParseTree->mRevokeeList,
                                & sInitPlan->mRevokeeList,
                                aEnv )
             == STL_SUCCESS );

    /**********************************************************
     * Privilege Action List 정보 설정
     **********************************************************/

    /**
     * GRANT OPTION FOR 정보 설정
     */

    sInitPlan->mGrantOption = sParseTree->mGrantOption;
    
    /**
     * Revoke Behavior 정보 설정 
     */

    sInitPlan->mRevokeBehavior = sParseTree->mRevokeBehavior;
    STL_ASSERT( (sInitPlan->mRevokeBehavior > QLP_REVOKE_NA) && (sInitPlan->mRevokeBehavior < QLP_REVOKE_MAX) );

    /**
     * Revokable Privilege Action List 설정
     */

    STL_TRY( qlaGetRevokablePrivActionList( aTransID,
                                            sStmt,
                                            aDBCStmt,
                                            aSQLString,
                                            sInitPlan->mPrivObject,
                                            & sInitPlan->mObjectHandle,
                                            sInitPlan->mHasControl,
                                            sInitPlan->mGrantOption,
                                            sInitPlan->mRevokeeList,
                                            sParseTree->mPrivilege->mPrivActionList,
                                            & sInitPlan->mRevokePrivList,
                                            aEnv )
             == STL_SUCCESS );

    /********************************************************
     * Validation Object 추가 
     ********************************************************/

    STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                  & sInitPlan->mObjectHandle,
                                  QLL_INIT_PLAN(aDBCStmt),
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellAppendObjectList( sInitPlan->mCommonInit.mValidationObjList,
                                  sInitPlan->mRevokeeList,
                                  QLL_INIT_PLAN(aDBCStmt),
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**********************************************************
     * Output 설정 
     **********************************************************/

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

    /* STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED ) */
    /* { */
    /*     stlPushError( STL_ERROR_LEVEL_ABORT, */
    /*                   QLL_ERRCODE_NOT_IMPLEMENTED, */
    /*                   NULL, */
    /*                   QLL_ERROR_STACK(aEnv), */
    /*                   "qlaValidateRevokePriv()" ); */
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
 * @brief REVOKE PRIVILEGE 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlaOptCodeRevokePriv( smlTransId      aTransID,
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
 * @brief REVOKE PRIVILEGE 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlaOptDataRevokePriv( smlTransId      aTransID,
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
 * @brief REVOKE PRIVILEGE 구문을 Execute 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aStmt      Statement
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlaExecuteRevokePriv( smlTransId      aTransID,
                                smlStatement  * aStmt,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv )
{
    qlaInitRevokePriv * sInitPlan = NULL;

    ellDictHandle     * sAuthHandle = NULL;
    
    stlTime             sTime = 0;

    ellPrivItem  * sPrivItem = NULL;

    stlBool   sLocked = STL_TRUE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_REVOKE_DATABASE_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_REVOKE_TABLESPACE_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_REVOKE_SCHEMA_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_REVOKE_TABLE_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_REVOKE_USAGE_TYPE) ,
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

    sInitPlan = (qlaInitRevokePriv *) aSQLStmt->mInitPlan;
    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    
    /**
     * REVOKE PRIV 구문을 위한 DDL Lock 획득
     */
    
    STL_TRY( ellLock4RevokePriv( aTransID,
                                 aStmt,
                                 sAuthHandle,
                                 sInitPlan->mRevokeeList,
                                 sInitPlan->mPrivObject,
                                 & sInitPlan->mObjectHandle,
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

    sInitPlan = (qlaInitRevokePriv *) aSQLStmt->mInitPlan;
    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    
    /***************************************************************
     * Privilege Dictionary & Cache 제거 
     ***************************************************************/

    /**
     * Revoke Privilege List 를 순회하며 Privilege 와 Dependent Privilege Descriptor 를 제거한다.
     */
    
    STL_RING_FOREACH_ENTRY( & sInitPlan->mRevokePrivList->mHeadList, sPrivItem )
    {
        /**
         * Depedent Privilege Descriptor 를 제거한다.
         */

        STL_TRY( qlaRevokeDependentPriv( aTransID,
                                         aStmt,
                                         sInitPlan->mRevokeBehavior,
                                         sInitPlan->mGrantOption,
                                         & sPrivItem->mPrivDesc,
                                         & QLL_EXEC_DDL_MEM(aEnv),
                                         aEnv )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    /* STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED ) */
    /* { */
    /*     stlPushError( STL_ERROR_LEVEL_ABORT, */
    /*                   QLL_ERRCODE_NOT_IMPLEMENTED, */
    /*                   NULL, */
    /*                   QLL_ERROR_STACK(aEnv), */
    /*                   "qlaExecuteRevokePriv()" ); */
    /* } */
    
    STL_FINISH;
    
    return STL_FAILURE;
}



/**
 * @brief Parse Tree 로부터 Revokable Privilege Action List 정보를 구축한다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aStmt             Statement
 * @param[in]  aDBCStmt          DBC Statement
 * @param[in]  aSQLString        SQL String
 * @param[in]  aPrivObject       Privilege Object 유형
 * @param[in]  aObjectHandle     Object Handle
 * @param[in]  aAccessControl    ACCESS CONTROL 권한 소유 여부
 * @param[in]  aGrantOption      GRANT OPTION FOR
 * @param[in]  aRevokeeList      Revokee List
 * @param[in]  aParseActionList  Action Parse Tree (nullable, ALL PRIVILEGES)
 * @param[out] aPrivActionList   Priviliege Action List 
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus qlaGetRevokablePrivActionList( smlTransId       aTransID,
                                         smlStatement   * aStmt,
                                         qllDBCStmt     * aDBCStmt,
                                         stlChar        * aSQLString,
                                         ellPrivObject    aPrivObject,
                                         ellDictHandle  * aObjectHandle,
                                         stlBool          aAccessControl,
                                         stlBool          aGrantOption,
                                         ellObjectList  * aRevokeeList,
                                         qlpList        * aParseActionList,
                                         ellPrivList   ** aPrivActionList,
                                         qllEnv         * aEnv )
{
    ellDictHandle * sAuthHandle = NULL;
    stlInt64        sGrantorID  = ELL_DICT_OBJECT_ID_NA;

    ellPrivList  * sPrivList = NULL;
    ellPrivList  * sTotalPrivList = NULL;
    ellPrivList  * sRevokeePrivList = NULL;
    
    ellPrivItem   * sPrivItem = NULL;

    ellPrivDesc      sPrivDesc;

    qlpListElement * sListElement = NULL;
    qlpPrivAction  * sPrivAction = NULL;
    ellDictHandle    sPrivHandle;
    stlBool          sPrivExist = STL_FALSE;
    stlBool          sDuplicate = STL_FALSE;

    stlInt64         sObjectID = ELL_DICT_OBJECT_ID_NA;

    qlpListElement * sColActElement = NULL;
    qlpValue       * sColActValue = NULL;
    stlChar        * sColActName  = NULL;
    ellDictHandle    sColActHandle;
    stlBool          sColActExist = STL_FALSE;

    stlInt32        i = 0;
    stlInt32        sColCount = 0;
    ellDictHandle * sColHandle = NULL;
    
    stlBool             sNeedColPriv = STL_FALSE;
    ellColumnPrivAction sColPrivAction = ELL_COLUMN_PRIV_ACTION_NA;

    ellObjectItem * sRevokeeItem = NULL;
    stlInt64        sRevokeeID = ELL_DICT_OBJECT_ID_NA;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aObjectHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPrivActionList != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    sGrantorID = ellGetAuthID( sAuthHandle );

    /**
     * Revokee List 를 순회하며 Privilege Descriptor 를 추가한다.
     */

    STL_TRY( ellInitPrivList( & sTotalPrivList,
                              STL_TRUE,  /* aForRevoke */
                              QLL_INIT_PLAN(aDBCStmt),
                              ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_RING_FOREACH_ENTRY( & aRevokeeList->mHeadList, sRevokeeItem )
    {
        sRevokeeID = ellGetAuthID( & sRevokeeItem->mObjectHandle );

        /**
         * Privilege Object 유형별 Privilege Action List 정보 획득
         */

        if ( aParseActionList == NULL )
        {
            /**
             * ALL PRIVILEGES ON privilege object
             */
                    
            switch ( aPrivObject )
            {
                case ELL_PRIV_DATABASE:
                    {
                        STL_TRY( ellGetRevokableAllDatabasePriv( aTransID,
                                                                 aStmt,
                                                                 sRevokeeID,
                                                                 aAccessControl,
                                                                 aGrantOption,
                                                                 & sRevokeePrivList,
                                                                 QLL_INIT_PLAN(aDBCStmt),
                                                                 ELL_ENV(aEnv) )
                                 == STL_SUCCESS );
                        break;
                    }
                case ELL_PRIV_SPACE:
                    {
                        STL_TRY( ellGetRevokableAllSpacePriv( aTransID,
                                                              aStmt,
                                                              aObjectHandle,
                                                              sRevokeeID,
                                                              aAccessControl,
                                                              aGrantOption,
                                                              & sRevokeePrivList,
                                                              QLL_INIT_PLAN(aDBCStmt),
                                                              ELL_ENV(aEnv) )
                                 == STL_SUCCESS );
                        break;
                    }
                case ELL_PRIV_SCHEMA:
                    {
                        STL_TRY( ellGetRevokableAllSchemaPriv( aTransID,
                                                               aStmt,
                                                               aObjectHandle,
                                                               sRevokeeID,
                                                               aAccessControl,
                                                               aGrantOption,
                                                               & sRevokeePrivList,
                                                               QLL_INIT_PLAN(aDBCStmt),
                                                               ELL_ENV(aEnv) )
                                 == STL_SUCCESS );
                        break;
                    }
                case ELL_PRIV_TABLE:
                    {
                        STL_TRY( ellGetRevokableAllTablePriv( aTransID,
                                                              aStmt,
                                                              aObjectHandle,
                                                              sRevokeeID,
                                                              aAccessControl,
                                                              aGrantOption,
                                                              & sRevokeePrivList,
                                                              QLL_INIT_PLAN(aDBCStmt),
                                                              ELL_ENV(aEnv) )
                                 == STL_SUCCESS );
                        break;
                    }
                case ELL_PRIV_USAGE:
                    {
                        STL_TRY( ellGetRevokableAllSeqPriv( aTransID,
                                                            aStmt,
                                                            aObjectHandle,
                                                            sRevokeeID,
                                                            aAccessControl,
                                                            aGrantOption,
                                                            & sRevokeePrivList,
                                                            QLL_INIT_PLAN(aDBCStmt),
                                                            ELL_ENV(aEnv) )
                                 == STL_SUCCESS );
                        break;
                    }
                case ELL_PRIV_COLUMN:
                    {
                        STL_ASSERT(0);
                        break;
                    }
                default:
                    {
                        STL_ASSERT(0);
                        break;
                    }
            }

            /**
             * REVOKE 할 Privilege 가 존재하지 않아도 무방함.
             */
        }
        else
        {
            /**
             * privilege action list ON privilege object
             */
            
            STL_TRY( ellInitPrivList( & sRevokeePrivList,
                                      STL_TRUE,  /* aForRevoke */
                                      QLL_INIT_PLAN(aDBCStmt),
                                      ELL_ENV(aEnv) )
                     == STL_SUCCESS );
                
            QLP_LIST_FOR_EACH( aParseActionList, sListElement )
            {
                sPrivAction = (qlpPrivAction *) QLP_LIST_GET_POINTER_VALUE( sListElement );

                switch ( sPrivAction->mPrivObject )
                {
                    case ELL_PRIV_DATABASE:
                        sObjectID = ellGetDbCatalogID();
                        break;
                    case ELL_PRIV_SPACE:
                        sObjectID = ellGetTablespaceID(aObjectHandle);
                        break;
                    case ELL_PRIV_SCHEMA:
                        sObjectID = ellGetSchemaID(aObjectHandle);
                        break;
                    case ELL_PRIV_TABLE:
                        sObjectID = ellGetTableID(aObjectHandle);
                        
                        if ( ellGetTableType( aObjectHandle ) == ELL_TABLE_TYPE_BASE_TABLE )
                        {
                            /**
                             * Base Table 에만 Column Privilge 를 추가한다.
                             */
                            sColCount = ellGetTableColumnCnt( aObjectHandle );
                            sColHandle = ellGetTableColumnDictHandle( aObjectHandle );
                        }
                        else
                        {
                            /**
                             * Base Table 이 아닌 경우
                             */
                            sColCount  = 0;
                            sColHandle = NULL;
                        }
                        
                        break;
                    case ELL_PRIV_USAGE:
                        sObjectID = ellGetSequenceID(aObjectHandle);
                        break;
                    case ELL_PRIV_COLUMN:
                        {
                            /**
                             * Base Table 인지 검사
                             */
                            
                            if( ellGetTableType( aObjectHandle ) == ELL_TABLE_TYPE_BASE_TABLE )
                            {
                                /**
                                 * Base Table 에만 Column Privilege 를 부여할 수 있다.
                                 */
                            }
                            else
                            {
                                sColActElement = QLP_LIST_GET_FIRST( sPrivAction->mColumnList );
                                sColActValue = (qlpValue *) QLP_LIST_GET_POINTER_VALUE( sColActElement );
                                STL_THROW( RAMP_ERR_NOT_BASE_TABLE_COLUMN );
                            }
                            
                            /**
                             * column list 에 대해 Column Privilege 를 구성 
                             */
                        
                            QLP_LIST_FOR_EACH( sPrivAction->mColumnList, sColActElement )
                            {
                                sColActValue = (qlpValue *) QLP_LIST_GET_POINTER_VALUE( sColActElement );
                                sColActName = QLP_GET_PTR_VALUE(sColActValue);

                                STL_TRY( ellGetColumnDictHandle( aTransID,
                                                                 aStmt->mScn,
                                                                 aObjectHandle,
                                                                 sColActName,
                                                                 & sColActHandle,
                                                                 & sColActExist,
                                                                 ELL_ENV(aEnv) )
                                         == STL_SUCCESS );
                                STL_TRY_THROW( sColActExist == STL_TRUE, RAMP_ERR_COLUMN_NOT_EXISTS );

                                if( aAccessControl == STL_TRUE )
                                {
                                    /**
                                     * ACCESS CONTROL 이 있는 경우
                                     */
                                    
                                    STL_TRY( ellGetRevokableColumnPrivByAccessControl( aTransID,
                                                                                       aStmt,
                                                                                       & sColActHandle,
                                                                                       sRevokeeID,
                                                                                       sPrivAction->mPrivAction,
                                                                                       & sPrivExist,
                                                                                       & sDuplicate,
                                                                                       sRevokeePrivList,
                                                                                       QLL_INIT_PLAN(aDBCStmt),
                                                                                       ELL_ENV(aEnv) )
                                             == STL_SUCCESS );
                                    
                                    STL_TRY_THROW( sPrivExist == STL_TRUE,
                                                   RAMP_ERR_INSUFFICIENT_PRIVILEGE_TO_REVOKE );
                                    
                                    STL_TRY_THROW( sDuplicate == STL_FALSE,
                                                   RAMP_ERR_DUPLICATE_PRIVILEGE_LIST );
                                }
                                else
                                {
                                    /**
                                     * Privilege Descriptor 구성
                                     */
                                    
                                    sPrivDesc.mGrantorID  = sGrantorID;
                                    sPrivDesc.mGranteeID  = sRevokeeID;
                                    sPrivDesc.mObjectID   = ellGetColumnID( & sColActHandle );
                                    sPrivDesc.mPrivObject = ELL_PRIV_COLUMN;
                                    sPrivDesc.mWithGrant  = aGrantOption;
                                    sPrivDesc.mPrivAction.mColumnAction = sPrivAction->mPrivAction;
                                    
                                    /**
                                     * Privilege Descriptor 와 동일한 Privilege Handle 이 존재하는지 검사 
                                     */
                                    
                                    STL_TRY( ellGetPrivDescHandle( aTransID,
                                                                   aStmt->mScn,
                                                                   & sPrivDesc,
                                                                   & sPrivHandle,
                                                                   & sPrivExist,
                                                                   ELL_ENV( aEnv ) )
                                             == STL_SUCCESS );

                                    STL_TRY_THROW( sPrivExist == STL_TRUE, 
                                                   RAMP_ERR_INSUFFICIENT_PRIVILEGE_TO_REVOKE );
                                    
                                    /**
                                     * Privilege Action 목록에 추가
                                     */
                                    
                                    STL_TRY( ellAddNewPrivItem( sRevokeePrivList,
                                                                sPrivAction->mPrivObject,
                                                                NULL,  /* aPrivHandle */
                                                                & sPrivDesc,
                                                                & sDuplicate,
                                                                QLL_INIT_PLAN(aDBCStmt),
                                                                ELL_ENV(aEnv) )
                                             == STL_SUCCESS );
                                    
                                    STL_TRY_THROW( sDuplicate == STL_FALSE,
                                                   RAMP_ERR_DUPLICATE_PRIVILEGE_LIST );
                                }
                            }
                            
                            /**
                             * COLUMN PRIVILEGE 를 모두 추가함
                             */
                            continue;
                        }
                    default:
                        {
                            STL_ASSERT(0);
                            break;
                        }
                }

                if( aAccessControl == STL_TRUE )
                {
                    /**
                     * ACCESS CONTROL 이 있는 경우
                     */

                    STL_TRY( ellGetRevokablePrivByAccessControl( aTransID,
                                                                 aStmt,
                                                                 sPrivAction->mPrivObject,
                                                                 aObjectHandle,
                                                                 sRevokeeID,
                                                                 sPrivAction->mPrivAction,
                                                                 & sPrivExist,
                                                                 & sDuplicate,
                                                                 sRevokeePrivList,
                                                                 QLL_INIT_PLAN(aDBCStmt),
                                                                 ELL_ENV(aEnv) )
                             == STL_SUCCESS );

                    STL_TRY_THROW( sPrivExist == STL_TRUE,
                                   RAMP_ERR_INSUFFICIENT_PRIVILEGE_TO_REVOKE );

                    STL_TRY_THROW( sDuplicate == STL_FALSE,
                                   RAMP_ERR_DUPLICATE_PRIVILEGE_LIST );
                }
                else
                {
                    /**
                     * Privilege Descriptor 구성
                     */
                    
                    sPrivDesc.mGrantorID  = sGrantorID;
                    sPrivDesc.mGranteeID  = sRevokeeID;
                    sPrivDesc.mObjectID   = sObjectID;
                    sPrivDesc.mPrivObject = sPrivAction->mPrivObject;
                    sPrivDesc.mWithGrant  = aGrantOption;
                    sPrivDesc.mPrivAction.mAction = sPrivAction->mPrivAction;
                    
                    /**
                     * Privilege Descriptor 와 동일한 Privilege Handle 이 존재하는지 검사 
                     */
                    
                    STL_TRY( ellGetPrivDescHandle( aTransID,
                                                   aStmt->mScn,
                                                   & sPrivDesc,
                                                   & sPrivHandle,
                                                   & sPrivExist,
                                                   ELL_ENV( aEnv ) )
                             == STL_SUCCESS );
                    
                    STL_TRY_THROW( sPrivExist == STL_TRUE,
                                   RAMP_ERR_INSUFFICIENT_PRIVILEGE_TO_REVOKE );
                    
                    /**
                     * Privilege Action 목록에 추가
                     */
                    
                    STL_TRY( ellAddNewPrivItem( sRevokeePrivList,
                                                sPrivAction->mPrivObject,
                                                NULL,  /* aPrivHandle */
                                                & sPrivDesc,
                                                & sDuplicate,
                                                QLL_INIT_PLAN(aDBCStmt),
                                                ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                    
                    STL_TRY_THROW( sDuplicate == STL_FALSE,
                                   RAMP_ERR_DUPLICATE_PRIVILEGE_LIST );
                }
                
                /**
                 * TABLE PRIVILEGE 인 경우, ACTION 에 따라 COLUMN PRIVILEGE 들을 추가한다.
                 */
            
                if ( sPrivAction->mPrivObject == ELL_PRIV_TABLE )
                {
                    switch ( sPrivAction->mPrivAction )
                    {
                        case ELL_TABLE_PRIV_ACTION_SELECT:
                            sNeedColPriv = STL_TRUE;
                            sColPrivAction = ELL_COLUMN_PRIV_ACTION_SELECT;
                            break;
                        case ELL_TABLE_PRIV_ACTION_INSERT:
                            sNeedColPriv = STL_TRUE;
                            sColPrivAction = ELL_COLUMN_PRIV_ACTION_INSERT;
                            break;
                        case ELL_TABLE_PRIV_ACTION_UPDATE:
                            sNeedColPriv = STL_TRUE;
                            sColPrivAction = ELL_COLUMN_PRIV_ACTION_UPDATE;
                            break;
                        case ELL_TABLE_PRIV_ACTION_REFERENCES:
                            sNeedColPriv = STL_TRUE;
                            sColPrivAction = ELL_COLUMN_PRIV_ACTION_REFERENCES;
                            break;
                        default:
                            /**
                             * nothing to do
                             */
                            sNeedColPriv = STL_FALSE;
                            sColPrivAction = ELL_COLUMN_PRIV_ACTION_NA;
                            break;
                    }

                    if ( sNeedColPriv == STL_TRUE )
                    {
                        /**
                         * COLUMN PRIVILEGE ACTION 을 추가해야 함.
                         */

                        for ( i = 0; i < sColCount; i++ )
                        {
                            if ( ellGetColumnUnused( & sColHandle[i] ) == STL_TRUE )
                            {
                                /**
                                 * Unused Column 임
                                 */
                                continue;
                            }

                            if ( aAccessControl == STL_TRUE )
                            {
                                /**
                                 * ACCESS CONTROL 이 있는 경우
                                 */
                                
                                STL_TRY( ellGetRevokableColumnPrivByAccessControl( aTransID,
                                                                                   aStmt,
                                                                                   & sColHandle[i],
                                                                                   sRevokeeID,
                                                                                   sColPrivAction,
                                                                                   & sPrivExist,
                                                                                   & sDuplicate,
                                                                                   sRevokeePrivList,
                                                                                   QLL_INIT_PLAN(aDBCStmt),
                                                                                   ELL_ENV(aEnv) )
                                         == STL_SUCCESS );
                                
                                STL_TRY_THROW( sDuplicate == STL_FALSE,
                                               RAMP_ERR_DUPLICATE_PRIVILEGE_LIST );
                            }
                            else
                            {
                                /**
                                 * Privilege Descriptor 구성
                                 */
                            
                                sPrivDesc.mGrantorID  = sGrantorID;
                                sPrivDesc.mGranteeID  = sRevokeeID;
                                sPrivDesc.mObjectID   = ellGetColumnID( & sColHandle[i] );
                                sPrivDesc.mPrivObject = ELL_PRIV_COLUMN;
                                sPrivDesc.mWithGrant  = aGrantOption;
                                sPrivDesc.mPrivAction.mColumnAction = sColPrivAction;
                            
                                /**
                                 * Privilege Descriptor 와 동일한 Privilege Handle 이 존재하는지 검사 
                                 */
                            
                                STL_TRY( ellGetPrivDescHandle( aTransID,
                                                               aStmt->mScn,
                                                               & sPrivDesc,
                                                               & sPrivHandle,
                                                               & sPrivExist,
                                                               ELL_ENV( aEnv ) )
                                         == STL_SUCCESS );
                            
                                if ( sPrivExist == STL_FALSE )
                                {
                                    /**
                                     * 파생된 Columm Privilege 가 없는 경우로 에러가 아님
                                     */
                                    continue;
                                }
                                else
                                {
                                    /**
                                     * Privilege Action 목록에 추가
                                     */
                                
                                    STL_TRY( ellAddNewPrivItem( sRevokeePrivList,
                                                                sPrivAction->mPrivObject,
                                                                NULL,  /* aPrivHandle */
                                                                & sPrivDesc,
                                                                & sDuplicate,
                                                                QLL_INIT_PLAN(aDBCStmt),
                                                                ELL_ENV(aEnv) )
                                             == STL_SUCCESS );
                                    
                                    STL_TRY_THROW( sDuplicate == STL_FALSE,
                                                   RAMP_ERR_DUPLICATE_PRIVILEGE_LIST );
                                }
                            }
                        }
                    }
                    else
                    {
                        /**
                         * nothing to do
                         * - Column Privilege 가 없는 Table Privilege Action 임 
                         */
                    }
                }
                else
                {
                    
                    /**
                     * nothing to do
                     * - TABLE Privilege 가 아님 
                     */
                }
            }
        }

        /**
         * Total Privilege List 에 추가
         */

        STL_RING_FOREACH_ENTRY( & sRevokeePrivList->mHeadList, sPrivItem )
        {
            /**
             * Privilege Action 목록에 추가
             */
            
            STL_TRY( ellAddNewPrivItem( sTotalPrivList,
                                        sPrivItem->mPrivDesc.mPrivObject,
                                        NULL,  /* aPrivHandle */
                                        & sPrivItem->mPrivDesc,
                                        & sDuplicate,
                                        QLL_INIT_PLAN(aDBCStmt),
                                        ELL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            STL_DASSERT( sDuplicate == STL_FALSE );
        }
    }
    
    /**************************************************************
     * GRANT OPTION FOR 검사 
     **************************************************************/
    
    if ( aGrantOption == STL_TRUE )
    {
        /**
         * GRANT OPTION FOR 가 있는 경우 
         */
        
        STL_TRY( ellInitPrivList( & sPrivList,
                                  STL_TRUE,  /* aForRevoke */
                                  QLL_INIT_PLAN(aDBCStmt),
                                  ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_RING_FOREACH_ENTRY( & sTotalPrivList->mHeadList, sPrivItem )
        {
            if ( sPrivItem->mPrivDesc.mWithGrant == STL_TRUE )
            {
                STL_TRY( ellAddNewPrivItem( sPrivList,
                                            sPrivItem->mPrivDesc.mPrivObject,
                                            NULL,   /* aPrivHandle */
                                            & sPrivItem->mPrivDesc,
                                            & sDuplicate,
                                            QLL_INIT_PLAN(aDBCStmt),
                                            ELL_ENV(aEnv) )
                         == STL_SUCCESS );

                STL_ASSERT( sDuplicate == STL_FALSE );
            }
            else
            {
                /**
                 * WITH GRANT OPTION 이 없음
                 */
            }
        }
        
    }
    else
    {
        /**
         * GRANT OPTION FOR 가 없는 경우
         */

        sPrivList = sTotalPrivList;
    }
    
    /**
     * Output 설정
     */

    *aPrivActionList = sPrivList;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_BASE_TABLE_COLUMN )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_ONLY_BASE_TABLE_COLUMNS_ARE_ALLOWED_TO_GRANT_REVOKE,
                      qlgMakeErrPosString( aSQLString,
                                           sColActValue->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sColActName );
    }
    
    STL_CATCH( RAMP_ERR_COLUMN_NOT_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_IDENTIFIER,
                      qlgMakeErrPosString( aSQLString,
                                           sColActValue->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sColActName );
    }
    
    STL_CATCH( RAMP_ERR_INSUFFICIENT_PRIVILEGE_TO_REVOKE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_REVOKE_PRIVILEGES_YOU_DID_NOT_GRANT,
                      qlgMakeErrPosString( aSQLString,
                                           sPrivAction->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      ellGetAuthName( & sRevokeeItem->mObjectHandle ) );
    }

    STL_CATCH( RAMP_ERR_DUPLICATE_PRIVILEGE_LIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_DUPLICATE_PRIVILEGE_LISTED,
                      qlgMakeErrPosString( aSQLString,
                                           sPrivAction->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Revoked Privilege 와 관련된 dependent privilege 를 제거한다.
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aStmt           Statement
 * @param[in]  aRevokeBehavior Revoke Behavior
 * @param[in]  aGrantOption    GRANT OPTION FOR
 * @param[in]  aRevokePriv     Revoked Privilege
 * @param[in]  aRegionMem      Region Memory
 * @param[in]  aEnv            Environemnt
 * @remarks
 */
stlStatus qlaRevokeDependentPriv( smlTransId          aTransID,
                                  smlStatement      * aStmt,
                                  qlpRevokeBehavior   aRevokeBehavior,
                                  stlBool             aGrantOption,
                                  ellPrivDesc       * aRevokePriv,
                                  knlRegionMem      * aRegionMem,
                                  qllEnv            * aEnv )
{
    ellPrivList * sDependentList = NULL;

    ellPrivItem   * sPrivItem   = NULL;
    ellPrivDesc   * sDependPriv = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aRevokeBehavior > QLP_REVOKE_NA) && (aRevokeBehavior < QLP_REVOKE_MAX),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRevokePriv != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Revoked Privilege 와 관련된 Dependent Privilege List 를 획득
     */

    STL_TRY( ellGetRevokeDependPrivList( aTransID,
                                         aStmt,
                                         aGrantOption,
                                         aRevokePriv,
                                         & sDependentList,
                                         aRegionMem,
                                         ELL_ENV(aEnv) )
             == STL_SUCCESS );

    if ( sDependentList == NULL )
    {
        /**
         * Dependent Privilege 가 없는 경우
         */
    }
    else
    {
        /**
         * Dependent Privilege 가 존재하는 경우
         */
        
        STL_TRY_THROW( aRevokeBehavior != QLP_REVOKE_RESTRICT, RAMP_DEPENDENT_PRIV_EXIST );


        /**
         * Dependent Privilege List 의 제거
         * - 모든 후손 Dependent Privilege 가 존재함.
         */
        
        STL_RING_FOREACH_ENTRY( & sDependentList->mHeadList, sPrivItem )
        {
            sDependPriv = & sPrivItem->mPrivDesc;
            
            /**
             * Revoke Privilege 가 Reference Privilege 인 경우
             */
            if ( ( (aRevokePriv->mPrivObject == ELL_PRIV_TABLE) &&
                   (aRevokePriv->mPrivAction.mTableAction == ELL_TABLE_PRIV_ACTION_REFERENCES) ) ||
                 ( (aRevokePriv->mPrivObject == ELL_PRIV_COLUMN) &&
                   (aRevokePriv->mPrivAction.mColumnAction == ELL_COLUMN_PRIV_ACTION_REFERENCES) ) )
            {
                switch ( aRevokeBehavior )
                {
                    case QLP_REVOKE_CASCADE:
                        {
                            /**
                             * @todo 관련된 FOREIGN KEY 가 존재하는 지 검사하고 존재한다면 에러
                             */
                            
                            break;
                        }
                    case QLP_REVOKE_CASCADE_CONSTRAINTS:
                        {
                            /**
                             * @todo 관련된 FOREIGN KEY 가 존재하는 지 검사하고 존재하면 제거 
                             */
                            break;
                        }
                    default:
                        STL_ASSERT(0);
                        break;
                }
            }
            else
            {
                /**
                 * nothing to do
                 */
            }
            
            /**
             * Depedent Privilege Descriptor 에 대한 Dictionary Row 와 Dictionary Cache 제거 
             */
            
            STL_TRY( ellRevokePrivDescAndCache( aTransID,
                                                aStmt,
                                                aGrantOption,
                                                sDependPriv,
                                                ELL_ENV(aEnv) )
                     == STL_SUCCESS );

        }
        
    }

    /**
     * Privilege Descriptor 에 대한 Dictionary Row 와 Dictionary Cache 제거 
     */
    
    STL_TRY( ellRevokePrivDescAndCache( aTransID,
                                        aStmt,
                                        aGrantOption,
                                        aRevokePriv,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_DEPENDENT_PRIV_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_DEPENDENT_PRIVILEGE_DESCRIPTORS_STILL_EXIST,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
    
}





/** @} qlaRevokePriv */
