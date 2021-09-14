/*******************************************************************************
 * qlaGrantPriv.c
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
 * @file qlaGrantPriv.c
 * @brief GRANT PRIVILEGE 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qlaGrantPriv GRANT PRIVILEGE
 * @ingroup qla
 * @{
 */


/**
 * @brief GRANT PRIVILEGE 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlaValidateGrantPriv( smlTransId      aTransID,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                stlChar       * aSQLString,
                                qllNode       * aParseTree,
                                qllEnv        * aEnv )
{
    stlInt32  sState = 0;
    
    qlpGrantPriv        * sParseTree = NULL;
    qlaInitGrantPriv    * sInitPlan = NULL;

    smlStatement  * sStmt = NULL;
    stlInt32        sStmtAttr = 0;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_GRANT_DATABASE_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_GRANT_TABLESPACE_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_GRANT_SCHEMA_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_GRANT_TABLE_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_GRANT_USAGE_TYPE) ,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aParseTree->mType == QLL_STMT_GRANT_DATABASE_TYPE) ||
                        (aParseTree->mType == QLL_STMT_GRANT_TABLESPACE_TYPE) ||
                        (aParseTree->mType == QLL_STMT_GRANT_SCHEMA_TYPE) ||
                        (aParseTree->mType == QLL_STMT_GRANT_TABLE_TYPE) ||
                        (aParseTree->mType == QLL_STMT_GRANT_USAGE_TYPE) ,
                        QLL_ERROR_STACK(aEnv) );
    
    /**
     * 기본 정보 획득 
     */
    
    sParseTree = (qlpGrantPriv *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlaInitGrantPriv),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlaInitGrantPriv) );

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
                               sParseTree->mGranteeList,
                               & sInitPlan->mObjectHandle,
                               aEnv )
             == STL_SUCCESS );

    /**********************************************************
     * Grantor 의 ACCESS CONTROL 권한 존재 여부
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
     * Grantee List 정보 설정
     **********************************************************/

    STL_TRY( qlaGetGranteeList( aTransID,
                                aDBCStmt,
                                aSQLStmt,
                                aSQLString,
                                sParseTree->mGranteeList,
                                & sInitPlan->mGranteeList,
                                aEnv )
             == STL_SUCCESS );

    /**********************************************************
     * Privilege Action List 정보 설정
     **********************************************************/

    /**
     * WITH GRANT OPTION 정보 설정
     */
    
    sInitPlan->mWithGrant = sParseTree->mWithGrant;

    /**
     * Grantable Privilege Action List 정보 설정
     */

    STL_TRY( qlaGetGrantablePrivActionList( aTransID,
                                            sStmt,
                                            aDBCStmt,
                                            aSQLString,
                                            sInitPlan->mPrivObject,
                                            & sInitPlan->mObjectHandle,
                                            sInitPlan->mHasControl,
                                            sParseTree->mPrivilege->mPrivActionList,
                                            & sInitPlan->mGrantablePrivList,
                                            aEnv )
             == STL_SUCCESS );
    
    /**
     * Total Privilege Action List 정보 설정
     */

    STL_TRY( qlaGetTotalPrivList( aTransID,
                                  aDBCStmt,
                                  sInitPlan->mWithGrant,
                                  sInitPlan->mGrantablePrivList,
                                  sInitPlan->mGranteeList,
                                  & sInitPlan->mTotalPrivList,
                                  aEnv )
             == STL_SUCCESS );

    /**
     * Statement 해제
     */

    sState = 0;
    STL_TRY( smlFreeStatement( sStmt, SML_ENV(aEnv) ) == STL_SUCCESS );
    
    /********************************************************
     * Validation Object 추가 
     ********************************************************/

    STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                  & sInitPlan->mObjectHandle,
                                  QLL_INIT_PLAN(aDBCStmt),
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellAppendObjectList( sInitPlan->mCommonInit.mValidationObjList,
                                  sInitPlan->mGranteeList,
                                  QLL_INIT_PLAN(aDBCStmt),
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );
                                  
    /**********************************************************
     * Output 설정 
     **********************************************************/

    /**
     * Init Plan 연결 
     */

    aSQLStmt->mInitPlan = (void *) sInitPlan;
    
    return STL_SUCCESS;
    
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
 * @brief GRANT PRIVILEGE 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlaOptCodeGrantPriv( smlTransId      aTransID,
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
 * @brief GRANT PRIVILEGE 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlaOptDataGrantPriv( smlTransId      aTransID,
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
 * @brief GRANT PRIVILEGE 구문을 Execute 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aStmt      Statement
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlaExecuteGrantPriv( smlTransId      aTransID,
                               smlStatement  * aStmt,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv )
{
    qlaInitGrantPriv * sInitPlan = NULL;

    ellDictHandle     * sAuthHandle = NULL;
    
    stlTime             sTime = 0;

    ellPrivItem  * sPrivItem = NULL;

    stlBool   sLocked = STL_TRUE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_GRANT_DATABASE_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_GRANT_TABLESPACE_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_GRANT_SCHEMA_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_GRANT_TABLE_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_GRANT_USAGE_TYPE) ,
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

    sInitPlan = (qlaInitGrantPriv *) aSQLStmt->mInitPlan;
    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    
    /**
     * GRANT PRIV 구문을 위한 DDL Lock 획득
     */
    
    STL_TRY( ellLock4GrantPriv( aTransID,
                                aStmt,
                                sAuthHandle,
                                sInitPlan->mGranteeList,
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

    sInitPlan = (qlaInitGrantPriv *) aSQLStmt->mInitPlan;
    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );

    /***************************************************************
     * Privilege Dictionary & Cache 추가 
     ***************************************************************/

    /**
     * Privilege Action List 를 순회하며 Privilege Descriptor 를 추가한다.
     */
    
    STL_RING_FOREACH_ENTRY( & sInitPlan->mTotalPrivList->mHeadList, sPrivItem )
    {
        if ( sPrivItem->mPrivDesc.mGrantorID == sPrivItem->mPrivDesc.mGranteeID )
        {
            /**
             * ACCESS CONTROL 로 생성한 privilege action 이
             * Grantor 와 Grantee 가 동일한 경우라면(Schema-Object Onwer 에게 권한을 부여하는 경우)
             * 권한을 추가하지 않는다.
             */
        }
        else
        {
            STL_TRY( ellGrantPrivDescAndCache( aTransID,
                                               aStmt,
                                               & sPrivItem->mPrivDesc,
                                               ELL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
    }
    
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Privilege Object 정보를 획득한다.
 * @param[in]  aTransID       Transaction ID
 * @param[in]  aDBCStmt       DBC Statement
 * @param[in]  aSQLStmt       SQL Statement
 * @param[in]  aSQLString     SQL String 
 * @param[in]  aPrivObject    Privilege Parse Tree
 * @param[in]  aGranteeList   Grantee List for ( [ON SCHEMA ..] 가 생략된 경우 필요 )
 * @param[out] aObjectHandle  Privilege Object Handle
 * @param[in]  aEnv           Environment
 * @remarks
 */
stlStatus qlaGetPrivObject( smlTransId      aTransID,
                            qllDBCStmt    * aDBCStmt,
                            qllStatement  * aSQLStmt,
                            stlChar       * aSQLString,
                            qlpPrivObject * aPrivObject,
                            qlpList       * aGranteeList,
                            ellDictHandle * aObjectHandle,
                            qllEnv        * aEnv )
{
    stlChar * sObjectName = NULL;
    stlInt32  sObjectPos = 0;
    stlBool   sObjectExist = STL_FALSE;

    ellDictHandle * sAuthHandle = NULL;
    ellDictHandle   sSchemaHandle;

    qlpListElement * sGranteeElement = NULL;
    qlpValue       * sGranteeValue = NULL;
    ellDictHandle    sGranteeHandle;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPrivObject != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aGranteeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aObjectHandle != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    ellInitDictHandle( aObjectHandle );
    
    /**
     * Privilege Object 유형에 따른 Object Handle 획득
     */

    
    switch ( aPrivObject->mPrivObjType )
    {
        case ELL_PRIV_DATABASE:
            {
                STL_TRY( ellGetCatalogDictHandle( aTransID,
                                                  aObjectHandle,
                                                  ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case ELL_PRIV_SPACE:
            {
                sObjectName = QLP_GET_PTR_VALUE( aPrivObject->mNonSchemaObjectName );
                sObjectPos = aPrivObject->mNonSchemaObjectName->mNodePos;
                
                STL_TRY( ellGetTablespaceDictHandle( aTransID,
                                                     aSQLStmt->mViewSCN,
                                                     sObjectName,
                                                     aObjectHandle,
                                                     & sObjectExist,
                                                     ELL_ENV(aEnv) )
                         == STL_SUCCESS );

                STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_SPACE_NOT_EXIST );

                break;
            }
        case ELL_PRIV_SCHEMA:
            {
                if ( aPrivObject->mNonSchemaObjectName != NULL )
                {
                    /**
                     * ON SCHEMA schema_name 으로 명시된 경우
                     */
                    sObjectName = QLP_GET_PTR_VALUE( aPrivObject->mNonSchemaObjectName );
                    sObjectPos = aPrivObject->mNonSchemaObjectName->mNodePos;
                    
                    STL_TRY( ellGetSchemaDictHandle( aTransID,
                                                     aSQLStmt->mViewSCN,
                                                     sObjectName,
                                                     aObjectHandle,
                                                     & sObjectExist,
                                                     ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                    
                    STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_SCHEMA_NOT_EXIST );
                }
                else
                {
                    /**
                     * [ON SCHEMA schema_name] TO one_grantee 로 된 경우
                     * - 각 grantee 의 schema handle 을 이용해야 함.
                     */

                    /* parser 에서 걸러짐 */
                    STL_ASSERT( QLP_LIST_GET_COUNT(aGranteeList) == 1 );

                    sGranteeElement = QLP_LIST_GET_FIRST(aGranteeList);
                    sGranteeValue = QLP_LIST_GET_POINTER_VALUE(sGranteeElement);

                    sObjectName = QLP_GET_PTR_VALUE( sGranteeValue );
                    sObjectPos  = sGranteeValue->mNodePos;
                    
                    STL_TRY( ellGetAuthDictHandle( aTransID,
                                                   aSQLStmt->mViewSCN,
                                                   sObjectName,
                                                   & sGranteeHandle,
                                                   & sObjectExist,
                                                   ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                    STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_USER_NOT_EXIST );

                    STL_TRY_THROW( ellGetAuthType( & sGranteeHandle ) == ELL_AUTHORIZATION_TYPE_USER,
                                   RAMP_ERR_USER_NOT_EXIST );
                    
                    STL_TRY( ellGetAuthFirstSchemaDictHandle( aTransID,
                                                              aSQLStmt->mViewSCN,
                                                              & sGranteeHandle,
                                                              aObjectHandle,
                                                              ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                }
                break;
            }
        case ELL_PRIV_TABLE:
            {
                if ( aPrivObject->mSchemaObjectName->mSchema == NULL )
                {
                    /**
                     * Schema Name 이 명시되지 않은 경우
                     */
                    
                    sObjectPos = aPrivObject->mSchemaObjectName->mObjectPos;

                    STL_TRY( qlvGetTableDictHandleByAuthHandleNTblName(
                                 aTransID,
                                 aDBCStmt,
                                 aSQLStmt,
                                 aSQLString,
                                 sAuthHandle,
                                 aPrivObject->mSchemaObjectName->mObject,
                                 aPrivObject->mSchemaObjectName->mObjectPos,
                                 aObjectHandle,
                                 & sObjectExist,
                                 aEnv )
                             == STL_SUCCESS );
                    
                    STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_TABLE_NOT_EXIST );
                }
                else
                {
                    /**
                     * Schema 가 명시된 경우
                     */

                    sObjectPos = aPrivObject->mSchemaObjectName->mSchemaPos;
                    
                    STL_TRY( ellGetSchemaDictHandle( aTransID,
                                                     aSQLStmt->mViewSCN,
                                                     aPrivObject->mSchemaObjectName->mSchema,
                                                     & sSchemaHandle,
                                                     & sObjectExist,
                                                     ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                    
                    STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_TABLE_NOT_EXIST );

                     STL_TRY( qlvGetTableDictHandleBySchHandleNTblName(
                                  aTransID,
                                  aDBCStmt,
                                  aSQLStmt,
                                  aSQLString,
                                  & sSchemaHandle,
                                  aPrivObject->mSchemaObjectName->mObject,
                                  aPrivObject->mSchemaObjectName->mObjectPos,
                                  aObjectHandle,
                                  & sObjectExist,
                                  aEnv )
                              == STL_SUCCESS );                    

                    STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_TABLE_NOT_EXIST );
                }

                STL_TRY_THROW( (ellGetTableType( aObjectHandle ) != ELL_TABLE_TYPE_FIXED_TABLE) &&
                               (ellGetTableType( aObjectHandle ) != ELL_TABLE_TYPE_DUMP_TABLE),
                               RAMP_ERR_FIXED_TABLE_NOT_GRANTABLE );
                break;
            }
        case ELL_PRIV_USAGE:
            {
                /**
                 * @todo USAGE privilege 에 대해 SEQUENCE 만을 고려하고 있음.
                 * 향후, schema 객체로 DOMAIN, COLLATION, CHARACTER SET, TRANSLATION 등을
                 * 지원하게 된다면, 구조 변경이 필요함.
                 */

                if ( aPrivObject->mSchemaObjectName->mSchema == NULL )
                {
                    /**
                     * Schema Name 이 명시되지 않은 경우
                     */

                    sObjectPos = aPrivObject->mSchemaObjectName->mObjectPos;

                    STL_TRY( qlvGetSequenceDictHandleByAuthHandleNSeqName(
                                 aTransID,
                                 aDBCStmt,
                                 aSQLStmt,
                                 aSQLString,
                                 sAuthHandle,
                                 aPrivObject->mSchemaObjectName->mObject,
                                 aPrivObject->mSchemaObjectName->mObjectPos,
                                 aObjectHandle,
                                 & sObjectExist,
                                 aEnv )
                             == STL_SUCCESS );
                    
                    STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_SEQ_NOT_EXIST );
                }
                else
                {
                    /**
                     * Schema 가 명시된 경우
                     */

                    sObjectPos = aPrivObject->mSchemaObjectName->mSchemaPos;
                    
                    STL_TRY( ellGetSchemaDictHandle( aTransID,
                                                     aSQLStmt->mViewSCN,
                                                     aPrivObject->mSchemaObjectName->mSchema,
                                                     & sSchemaHandle,
                                                     & sObjectExist,
                                                     ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                    
                    STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_SEQ_NOT_EXIST );

                    STL_TRY( qlvGetSequenceDictHandleBySchHandleNSeqName(
                                 aTransID,
                                 aDBCStmt,
                                 aSQLStmt,
                                 aSQLString,
                                 & sSchemaHandle,
                                 aPrivObject->mSchemaObjectName->mObject,
                                 aPrivObject->mSchemaObjectName->mObjectPos,
                                 aObjectHandle,
                                 & sObjectExist,
                                 aEnv )
                             == STL_SUCCESS );

                    STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_SEQ_NOT_EXIST );
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

    STL_CATCH( RAMP_ERR_SPACE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLESPACE_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           sObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sObjectName );
    }
    
    STL_CATCH( RAMP_ERR_SCHEMA_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           sObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sObjectName );
    }

    STL_CATCH( RAMP_ERR_USER_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_USER_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           sObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sObjectName );
    }
    
    STL_CATCH( RAMP_ERR_TABLE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLE_OR_VIEW_NOT_EXIST,
                      qlgMakeErrPosString( aSQLString,
                                           sObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_FIXED_TABLE_NOT_GRANTABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_FIXED_TABLE_NOT_GRANT_REVOKE,
                      qlgMakeErrPosString( aSQLString,
                                           sObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_CATCH( RAMP_ERR_SEQ_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SEQUENCE_NOT_EXIST,
                      qlgMakeErrPosString( aSQLString,
                                           sObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Grantee List 정보를 구축한다.
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aDBCStmt      DBC Statement
 * @param[in]  aSQLStmt      SQL Statement
 * @param[in]  aSQLString    SQL String
 * @param[in]  aNameList     Name List Parse Tree
 * @param[out] aGranteeList  Grantee Object List 
 * @param[in]  aEnv          Environment
 * @remarks
 */
stlStatus qlaGetGranteeList( smlTransId       aTransID,
                             qllDBCStmt     * aDBCStmt,
                             qllStatement   * aSQLStmt,
                             stlChar        * aSQLString,
                             qlpList        * aNameList,
                             ellObjectList ** aGranteeList,
                             qllEnv         * aEnv )
{
    ellDictHandle * sAuthHandle = NULL;

    ellObjectList * sGranteeList = NULL;
    
    qlpListElement * sListElement = NULL;
    qlpValue       * sGrantee = NULL;
    stlChar        * sGranteeName = NULL;
    ellDictHandle    sGranteeHandle;

    stlBool          sObjectExist = STL_FALSE;
    stlBool          sDuplicate = STL_FALSE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNameList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aGranteeList != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    ellInitObjectList( & sGranteeList, QLL_INIT_PLAN(aDBCStmt), ELL_ENV(aEnv) );

    /**
     * Name List 를 순회하며 Grantee 의 Dictionary Handle 정보를 획득
     */
    
    QLP_LIST_FOR_EACH( aNameList, sListElement )
    {
        sGrantee = (qlpValue *) QLP_LIST_GET_POINTER_VALUE( sListElement );
        sGranteeName = QLP_GET_PTR_VALUE( sGrantee );

        /**
         * Authorization 이 존재하는 지 검사
         */
        
        STL_TRY( ellGetAuthDictHandle( aTransID,
                                       aSQLStmt->mViewSCN,
                                       sGranteeName,
                                       & sGranteeHandle,
                                       & sObjectExist,
                                       ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_AUTH_NOT_EXIST );
        
        /**
         * 자신에게 부여하는 지 검사
         */

        STL_TRY_THROW( ellGetAuthID(sAuthHandle) != ellGetAuthID( & sGranteeHandle ),
                       RAMP_ERR_SELF_AUTH );

        /**
         * Built-In Authorization 인지 검사 
         */

        STL_TRY_THROW( ellIsUpdatablePrivAuth( & sGranteeHandle ) == STL_TRUE,
                       RAMP_ERR_MODIFY_BUILT_IN_AUTH );
        
        /**
         * Grantee List 에 추가
         */

        STL_TRY( ellAddNewObjectItem( sGranteeList,
                                      & sGranteeHandle,
                                      & sDuplicate,
                                      QLL_INIT_PLAN(aDBCStmt),
                                      ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY_THROW( sDuplicate == STL_FALSE, RAMP_ERR_DUP_GRANTEE );
    }

    /**
     * Output 설정
     */

    *aGranteeList = sGranteeList;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_AUTH_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_AUTHORIZATION_NOT_EXIST,
                      qlgMakeErrPosString( aSQLString,
                                           sGrantee->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sGranteeName );
    }

    STL_CATCH( RAMP_ERR_SELF_AUTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_GRANT_REVOKE_PRIVILEGES_YOURSELF,
                      qlgMakeErrPosString( aSQLString,
                                           sGrantee->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_MODIFY_BUILT_IN_AUTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_GRANT_REVOKE_PRIVILEGES_BUILT_IN_AUTHORIZATION,
                      qlgMakeErrPosString( aSQLString,
                                           sGrantee->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_CATCH( RAMP_ERR_DUP_GRANTEE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_DUPLICATE_AUTH_NAME_IN_GRATEE_REVOKEE_LIST,
                      qlgMakeErrPosString( aSQLString,
                                           sGrantee->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}




/**
 * @brief Parse Tree 로부터 Grantable Privilege Action List 정보를 구축한다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aStmt             Statement
 * @param[in]  aDBCStmt          DBC Statement
 * @param[in]  aSQLString        SQL String
 * @param[in]  aPrivObject       Privilege Object 유형
 * @param[in]  aObjectHandle     Object Handle
 * @param[in]  aAccessControl    ACCESS CONTROL 권한 소유 여부 
 * @param[in]  aParseActionList  Action Parse Tree (nullable, ALL PRIVILEGES)
 * @param[out] aPrivActionList   Priviliege Action List 
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus qlaGetGrantablePrivActionList( smlTransId       aTransID,
                                         smlStatement   * aStmt,
                                         qllDBCStmt     * aDBCStmt,
                                         stlChar        * aSQLString,
                                         ellPrivObject    aPrivObject,
                                         ellDictHandle  * aObjectHandle,
                                         stlBool          aAccessControl, 
                                         qlpList        * aParseActionList,
                                         ellPrivList   ** aPrivActionList,
                                         qllEnv         * aEnv )
{
    ellDictHandle * sAuthHandle = NULL;
    stlInt64        sGrantorID  = ELL_DICT_OBJECT_ID_NA;

    ellPrivList * sPrivList = NULL;

    qlpListElement * sListElement = NULL;
    qlpPrivAction  * sPrivAction = NULL;
    ellDictHandle    sPrivHandle;
    stlBool          sPrivExist = STL_FALSE;
    stlBool          sDuplicate = STL_FALSE;

    stlInt64         sObjectID = ELL_DICT_OBJECT_ID_NA;

    ellPrivDesc      sAccessDesc;
    
    qlpListElement * sColActElement = NULL;
    qlpValue       * sColActValue = NULL;
    stlChar        * sColActName  = NULL;
    ellDictHandle    sColActHandle;
    stlBool          sColActExist = STL_FALSE;

    stlInt32        i = 0;
    stlInt32        sColCount = 0;
    ellDictHandle * sColHandle = NULL;
    
    stlBool             sNeedColPriv = STL_FALSE;
    ellPrivDesc         sColPrivDesc;
    ellColumnPrivAction sColPrivAction = ELL_COLUMN_PRIV_ACTION_NA;

    ellPrivItem  * sPrivItem = NULL;
    
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
                    STL_TRY( ellGetGrantableAllDatabasePriv( aTransID,
                                                             aStmt,
                                                             & sPrivList,
                                                             QLL_INIT_PLAN(aDBCStmt),
                                                             ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                    break;
                }
            case ELL_PRIV_SPACE:
                {
                    STL_TRY( ellGetGrantableAllSpacePriv( aTransID,
                                                          aStmt,
                                                          aObjectHandle,
                                                          & sPrivList,
                                                          QLL_INIT_PLAN(aDBCStmt),
                                                          ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                    break;
                }
            case ELL_PRIV_SCHEMA:
                {
                    STL_TRY( ellGetGrantableAllSchemaPriv( aTransID,
                                                           aStmt,
                                                           aObjectHandle,
                                                           & sPrivList,
                                                           QLL_INIT_PLAN(aDBCStmt),
                                                           ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                    break;
                }
            case ELL_PRIV_TABLE:
                {
                    STL_TRY( ellGetGrantableAllTablePriv( aTransID,
                                                          aStmt,
                                                          aObjectHandle,
                                                          & sPrivList,
                                                          QLL_INIT_PLAN(aDBCStmt),
                                                          ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                    break;
                }
            case ELL_PRIV_USAGE:
                {
                    STL_TRY( ellGetGrantableAllSeqPriv( aTransID,
                                                        aStmt,
                                                        aObjectHandle,
                                                        & sPrivList,
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

        STL_TRY_THROW( ellHasPrivItem( sPrivList ) == STL_TRUE,
                       RAMP_ERR_EMPTY_ALL_PRIVILEGE );

        /**
         * Privilege Descriptor 재구성
         */

        STL_RING_FOREACH_ENTRY( & sPrivList->mHeadList, sPrivItem )
        {
            /**
             * Grantor 변경 
             */
            
            sPrivItem->mPrivDesc.mGrantorID = sGrantorID;
            sPrivItem->mPrivDesc.mGranteeID = ELL_DICT_OBJECT_ID_NA;;
        }
    }
    else
    {
        /**
         * privilege action list ON privilege object
         */

        STL_TRY( ellInitPrivList( & sPrivList,
                                  STL_FALSE,  /* aForRevoke */
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
                                                             ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                                             aObjectHandle,
                                                             sColActName,
                                                             & sColActHandle,
                                                             & sColActExist,
                                                             ELL_ENV(aEnv) )
                                     == STL_SUCCESS );
                            STL_TRY_THROW( sColActExist == STL_TRUE, RAMP_ERR_COLUMN_NOT_EXISTS );

                            /**
                             * Grantable Privilege 가 존재하는지 검사 
                             */
                            
                            STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                                              ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                                              sPrivAction->mPrivObject,
                                                              ellGetColumnID( & sColActHandle ),
                                                              sPrivAction->mPrivAction,
                                                              STL_TRUE, /* aWithGrant */
                                                              & sPrivHandle,
                                                              & sPrivExist,
                                                              ELL_ENV(aEnv) )
                                     == STL_SUCCESS );

                            if ( sPrivExist == STL_TRUE )
                            {
                                /**
                                 * Auth 가 해당 Grantable Privilege 가 있는 경우
                                 */
                                
                                stlMemcpy( & sAccessDesc,
                                           ellGetPrivDesc( & sPrivHandle ),
                                           STL_SIZEOF(ellPrivDesc) );
                                
                                sAccessDesc.mGrantorID = sGrantorID;
                                sAccessDesc.mGranteeID = ELL_DICT_OBJECT_ID_NA;
                            }
                            else
                            {
                                /**
                                 * Auth 가 해당 Grantable Privilege 가 없는 경우
                                 */

                                STL_TRY_THROW( aAccessControl == STL_TRUE,
                                               RAMP_ERR_INSUFFICIENT_PRIVILEGE_TO_GRANT );
                                
                                /**
                                 * ACCESS CONTROL 이 있는 경우로 Grantable Privilege 없이 부여할 수 있다.
                                 * Grantor 는 Object Owner 가 된다.
                                 */
                                
                                sAccessDesc.mGrantorID  = ellGetColumnOwnerID( & sColActHandle );
                                sAccessDesc.mGranteeID  = ELL_DICT_OBJECT_ID_NA;
                                
                                sAccessDesc.mPrivObject = sPrivAction->mPrivObject;
                                sAccessDesc.mObjectID   = ellGetColumnID( & sColActHandle );
                                sAccessDesc.mWithGrant  = STL_TRUE; /* N/A */
                                sAccessDesc.mPrivAction.mAction = sPrivAction->mPrivAction;
                            }

                            /**
                             * Privilege Action 목록에 추가
                             */
                            
                            STL_TRY( ellAddNewPrivItem( sPrivList,
                                                        sPrivAction->mPrivObject,
                                                        NULL,  /* aPrivHandle */
                                                        & sAccessDesc,
                                                        & sDuplicate,
                                                        QLL_INIT_PLAN(aDBCStmt),
                                                        ELL_ENV(aEnv) )
                                     == STL_SUCCESS );
                                
                            STL_TRY_THROW( sDuplicate == STL_FALSE, RAMP_ERR_DUPLICATE_PRIVILEGE_LIST );
                            
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
                
            /**
             * Grantable Privilege 가 존재하는지 검사 
             */
            
            STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                              ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                              sPrivAction->mPrivObject,
                                              sObjectID,
                                              sPrivAction->mPrivAction,
                                              STL_TRUE, /* aWithGrant */
                                              & sPrivHandle,
                                              & sPrivExist,
                                              ELL_ENV(aEnv) )
                     == STL_SUCCESS );

            if ( sPrivExist == STL_TRUE )
            {
                stlMemcpy( & sAccessDesc,
                           ellGetPrivDesc( & sPrivHandle ),
                           STL_SIZEOF(ellPrivDesc) );
                
                sAccessDesc.mGrantorID = sGrantorID;
                sAccessDesc.mGranteeID = ELL_DICT_OBJECT_ID_NA;
            }
            else
            {
                /**
                 * Auth 가 해당 Grantable Privilege 가 없는 경우
                 */
                
                STL_TRY_THROW( aAccessControl == STL_TRUE, RAMP_ERR_INSUFFICIENT_PRIVILEGE_TO_GRANT );
                
                /**
                 * ACCESS CONTROL 이 있는 경우로 Grantable Privilege 없이 부여할 수 있다.
                 */

                /**
                 * Grantor 는 Object Owner 가 된다.
                 */
                switch ( sPrivAction->mPrivObject )
                {
                    case ELL_PRIV_DATABASE:
                        sAccessDesc.mGrantorID = ellGetAuthIdSYSTEM();
                        break;
                    case ELL_PRIV_SPACE:
                        sAccessDesc.mGrantorID = ellGetAuthIdSYSTEM();
                        break;
                    case ELL_PRIV_SCHEMA:
                        sAccessDesc.mGrantorID = ellGetAuthIdSYSTEM();
                        break;
                    case ELL_PRIV_TABLE:
                        sAccessDesc.mGrantorID = ellGetTableOwnerID(aObjectHandle);
                        break;
                    case ELL_PRIV_USAGE:
                        sAccessDesc.mGrantorID = ellGetSequenceOwnerID(aObjectHandle);
                        break;
                    default:
                        STL_DASSERT(0);
                        break;
                }
                
                sAccessDesc.mGranteeID  = ELL_DICT_OBJECT_ID_NA;
                sAccessDesc.mPrivObject = sPrivAction->mPrivObject;
                sAccessDesc.mObjectID   = sObjectID;
                sAccessDesc.mWithGrant  = STL_TRUE;  /* N/A */
                sAccessDesc.mPrivAction.mAction = sPrivAction->mPrivAction;
            }

            /**
             * Privilege Action 목록에 추가
             */
            
            
            STL_TRY( ellAddNewPrivItem( sPrivList,
                                        sPrivAction->mPrivObject,
                                        NULL,  /* aPrivHandle */
                                        & sAccessDesc,
                                        & sDuplicate,
                                        QLL_INIT_PLAN(aDBCStmt),
                                        ELL_ENV(aEnv) )
                     == STL_SUCCESS );

            STL_TRY_THROW( sDuplicate == STL_FALSE, RAMP_ERR_DUPLICATE_PRIVILEGE_LIST );

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

                        sColPrivDesc.mGrantorID  = sAccessDesc.mGrantorID;
                        sColPrivDesc.mGranteeID  = ELL_DICT_OBJECT_ID_NA;
                        sColPrivDesc.mPrivObject = ELL_PRIV_COLUMN;
                        sColPrivDesc.mObjectID   = ellGetColumnID( & sColHandle[i] );
                        sColPrivDesc.mWithGrant  = STL_TRUE;  /* N/A */
                        sColPrivDesc.mPrivAction.mColumnAction = sColPrivAction;

                        STL_TRY( ellAddNewPrivItem( sPrivList,
                                                    ELL_PRIV_COLUMN,
                                                    NULL,             /* aPrivHandle */
                                                    & sColPrivDesc,   /* aPrivDesc */
                                                    & sDuplicate,
                                                    QLL_INIT_PLAN(aDBCStmt),
                                                    ELL_ENV(aEnv) )
                                 == STL_SUCCESS );

                        /**
                         * 중복이 없어야 함
                         */
                        
                        STL_TRY_THROW( sDuplicate == STL_FALSE, RAMP_ERR_DUPLICATE_PRIVILEGE_LIST );
                    }
                }
                else
                {
                    /**
                     * nothing to ddo
                     */
                }
            }
        }
    }

    /**
     * Output 설정
     */

    *aPrivActionList = sPrivList;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_EMPTY_ALL_PRIVILEGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NO_PRIVILEGES_TO_GRANT_ALL_PRIVILEGES,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

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
    
    STL_CATCH( RAMP_ERR_INSUFFICIENT_PRIVILEGE_TO_GRANT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INSUFFICIENT_PRIVILEGE_TO_GRANT,
                      qlgMakeErrPosString( aSQLString,
                                           sPrivAction->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
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
 * @brief Grantable Privilege List 와 Grantee List 를 조합하여 전체 Privilege List 를 구성한다.
 * @param[in] aTransID           Transaction ID
 * @param[in] aDBCStmt           DBC Statement
 * @param[in] aWithGrantOption   WITH GRANT OPTION
 * @param[in] aGrantableList     Grantable Privilege List
 * @param[in] aGranteeList       Grantee List
 * @param[in] aTotalPrivList     Total Privilege List
 * @param[in] aEnv               Environment
 * @remarks
 */
stlStatus qlaGetTotalPrivList( smlTransId       aTransID,
                               qllDBCStmt     * aDBCStmt,
                               stlBool          aWithGrantOption,
                               ellPrivList    * aGrantableList,
                               ellObjectList  * aGranteeList,
                               ellPrivList   ** aTotalPrivList,
                               qllEnv         * aEnv )
{
    ellPrivList * sTotalList = NULL;

    ellPrivDesc   sPrivDesc;

    ellObjectItem * sGranteeItem = NULL;
    stlInt64        sGranteeID = ELL_DICT_OBJECT_ID_NA;
    
    ellPrivItem  * sPrivItem = NULL;

    stlBool        sDuplicate = STL_FALSE;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aGrantableList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aGranteeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTotalPrivList != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    STL_TRY( ellInitPrivList( & sTotalList,
                              STL_TRUE,  /* aForRevoke, 동일한 것이 전혀 없음을 보장하기 위함 */
                              QLL_INIT_PLAN(aDBCStmt),
                              ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Grantee List 를 순회하며 Privilege Descriptor 를 추가한다.
     */

    STL_RING_FOREACH_ENTRY( & aGranteeList->mHeadList, sGranteeItem )
    {
        sGranteeID = ellGetAuthID( & sGranteeItem->mObjectHandle );

        /**
         * Privilege Action List 를 순회하며 Privilege Descriptor 를 추가한다.
         */

        STL_RING_FOREACH_ENTRY( & aGrantableList->mHeadList, sPrivItem )
        {
            /**
             * Privilege Descriptor 재구성
             */
            
            stlMemcpy( & sPrivDesc, & sPrivItem->mPrivDesc, STL_SIZEOF(ellPrivDesc) );

            sPrivDesc.mGranteeID = sGranteeID;
            sPrivDesc.mWithGrant = aWithGrantOption;

            /**
             * Privilege Descriptor 추가
             */
            
            STL_TRY( ellAddNewPrivItem( sTotalList,
                                        sPrivDesc.mPrivObject,
                                        NULL,  /* aPrivHandle */
                                        & sPrivDesc,
                                        & sDuplicate,
                                        QLL_INIT_PLAN(aDBCStmt),
                                        ELL_ENV(aEnv) )
                     == STL_SUCCESS );

            STL_ASSERT( sDuplicate == STL_FALSE );
        }
    }
    
    /**
     * Output 설정
     */

    *aTotalPrivList = sTotalList;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} qlaGrantPriv */
