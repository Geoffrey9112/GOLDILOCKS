/*******************************************************************************
 * qltxLockTable.c
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
 * @file qltxLockTable.c
 * @brief LOCK TABLE 처리 루틴 
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @defgroup qltxLockTable  LOCK TABLE
 * @ingroup qltx
 * @{
 */

    
/**
 * @brief LOCK TABLE 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qltxValidateLockTable( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 stlChar       * aSQLString,
                                 qllNode       * aParseTree,
                                 qllEnv        * aEnv )
{
    ellDictHandle     * sAuthHandle = NULL;
    qltxInitLockTable * sInitPlan   = NULL;
    qlpLockTable      * sParseTree  = NULL;
    qlpListElement    * sListElem   = NULL;
    qlpObjectName     * sObjectName = NULL;
    ellDictHandle       sTableHandle;
    ellDictHandle       sSchemaHandle;
    stlChar           * sEndPtr;
    stlInt64            sWaitTime;
    stlBool             sObjectExist = STL_FALSE;

    ellInitDictHandle( &sTableHandle );
    ellInitDictHandle( &sSchemaHandle );

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_LOCK_TABLE_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_LOCK_TABLE_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */
    
    sParseTree = (qlpLockTable *) aParseTree;

    STL_PARAM_VALIDATE( sParseTree->mRelations != NULL,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( sParseTree->mLockMode == QLP_LOCK_MODE_INVALID ) ||
                        ( sParseTree->mLockMode == QLP_LOCK_MODE_ROW_SHARE ) ||
                        ( sParseTree->mLockMode == QLP_LOCK_MODE_ROW_EXCLUSIVE ) ||
                        ( sParseTree->mLockMode == QLP_LOCK_MODE_SHARE ) ||
                        ( sParseTree->mLockMode == QLP_LOCK_MODE_EXCLUSIVE ) ||
                        ( sParseTree->mLockMode == QLP_LOCK_MODE_SHARE_ROW_EXCLUSIVE ),
                        QLL_ERROR_STACK(aEnv) );
    
    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qltxInitLockTable),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qltxInitLockTable) );

    STL_TRY( ellInitObjectList( & sInitPlan->mObjectList,
                                QLL_INIT_PLAN(aDBCStmt),
                                ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Init Plan 구성
     */

    sInitPlan->mMode = sParseTree->mLockMode;

    if( sParseTree->mWaitTime == NULL )
    {
        sInitPlan->mWaitTime = SML_LOCK_TIMEOUT_INFINITE;
    }
    else
    {
        STL_TRY( stlStrToInt64( QLP_GET_PTR_VALUE( sParseTree->mWaitTime ),
                                STL_NTS,
                                &sEndPtr,
                                10,
                                &sWaitTime,
                                KNL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );

        if( sWaitTime == 0 )
        {
            sInitPlan->mWaitTime = SML_LOCK_TIMEOUT_NOWAIT;
        }
        else
        {
            sInitPlan->mWaitTime = sWaitTime;
        }

        /**
         * WAIT interval validation
         */
        
        STL_TRY_THROW( (sInitPlan->mWaitTime >= QLV_MIN_WAIT_INTERVAL) &&
                       (sInitPlan->mWaitTime <= QLV_MAX_WAIT_INTERVAL),
                       RAMP_ERR_INVALID_WAIT_INTERVAL );
        
    }

    /**********************************************************
     * Relation Validation
     **********************************************************/
    
    QLP_LIST_FOR_EACH( sParseTree->mRelations, sListElem )
    {
        sObjectName = (qlpObjectName*)sListElem->mData.mPtrValue;

        if ( sObjectName->mSchema == NULL )
        {
            /**
             * Schema Name 이 명시되지 않은 경우
             */

            STL_TRY( qlvGetTableDictHandleByAuthHandleNTblName( aTransID,
                                                                aDBCStmt,
                                                                aSQLStmt,
                                                                aSQLString,
                                                                sAuthHandle,
                                                                sObjectName->mObject,
                                                                sObjectName->mObjectPos,
                                                                & sTableHandle,
                                                                & sObjectExist,
                                                                aEnv )
                 == STL_SUCCESS );
                   
            STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_TABLE_NOT_EXISTS );
        }
        else
        {
            /**
             * Schema Name 이 명시된 경우
             */

            STL_TRY( ellGetSchemaDictHandle( aTransID,
                                             aSQLStmt->mViewSCN,
                                             sObjectName->mSchema,
                                             & sSchemaHandle,
                                             & sObjectExist,
                                             ELL_ENV(aEnv) )
                     == STL_SUCCESS );

            STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_SCHEMA_NOT_EXISTS );

            /**
             * Table 존재 여부 확인
             */

            STL_TRY( qlvGetTableDictHandleBySchHandleNTblName( aTransID,
                                                               aDBCStmt,
                                                               aSQLStmt,
                                                               aSQLString,
                                                               & sSchemaHandle,
                                                               sObjectName->mObject,
                                                               sObjectName->mObjectPos,
                                                               & sTableHandle,
                                                               & sObjectExist,
                                                               aEnv )
                     == STL_SUCCESS );
            
            STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_TABLE_NOT_EXISTS );
        }

        /**
         * Built-In Table 인지 검사
         */

        switch( ellGetTableType( &sTableHandle ) )
        {
            case ELL_TABLE_TYPE_BASE_TABLE:
                break;
            case ELL_TABLE_TYPE_VIEW:
                /**
                 * @todo View의 Base Table의 Handle을 얻어와야 한다.
                 */
                STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                break;
            default :
                STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                break;
        }

        STL_TRY_THROW( ellIsBuiltInTable( &sTableHandle ) == STL_FALSE,
                       RAMP_ERR_BUILT_IN_TABLE );
        
        /**
         * LOCK ON TABLE 권한 검사 
         */

        STL_TRY( qlaCheckTablePriv( aTransID,
                                    aDBCStmt,
                                    aSQLStmt,
                                    ELL_TABLE_PRIV_ACTION_LOCK,
                                    & sTableHandle,
                                    NULL, /* aColumnList */
                                    aEnv )
                 == STL_SUCCESS );

        /**
         * Object List 에 등록
         */

        STL_TRY( ellAddNewObjectItem( sInitPlan->mObjectList,
                                      & sTableHandle,
                                      NULL,
                                      QLL_INIT_PLAN(aDBCStmt),
                                      ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }


    /**
     * Init Plan 연결 
     */

    aSQLStmt->mInitPlan = (void *) sInitPlan;
        
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

    STL_CATCH( RAMP_ERR_TABLE_NOT_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLE_OR_VIEW_NOT_EXIST,
                      qlgMakeErrPosString( aSQLString,
                                           sObjectName->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sObjectName->mObject );
    }

    STL_CATCH( RAMP_ERR_BUILT_IN_TABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_BUILT_IN_TABLE_IS_NOT_MODIFIABLE,
                      qlgMakeErrPosString( aSQLString,
                                           sObjectName->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sObjectName->mObject );
    }

    STL_CATCH( RAMP_ERR_INVALID_WAIT_INTERVAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_WAIT_INTERVAL,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mWaitTime->mNodePos,
                                           aEnv ),
                      ELL_ERROR_STACK(aEnv) );
    }
    
    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      qlgMakeErrPosString( aSQLString,
                                           sObjectName->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      "LOCK TABLE view" );
    }
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief LOCK TABLE 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qltxOptCodeLockTable( smlTransId      aTransID,
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
 * @brief LOCK TABLE 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qltxOptDataLockTable( smlTransId      aTransID,
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
 * @brief LOCK TABLE 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qltxExecuteLockTable( smlTransId      aTransID,
                                smlStatement  * aStmt,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv )
{
    qltxInitLockTable * sInitPlan = NULL;
    ellObjectItem     * sObjectItem;
    ellDictHandle     * sTableHandle;
    stlBool             sTableExist = STL_TRUE;

    stlBool    sIsValid = STL_FALSE;
    
    /**
     * Init Plan 획득 
     */

    sInitPlan = (qltxInitLockTable *) aSQLStmt->mInitPlan;

    STL_RING_FOREACH_ENTRY( &sInitPlan->mObjectList->mHeadList, sObjectItem )
    {
        sTableHandle = &sObjectItem->mObjectHandle;
        
        smlSetScnToDisableAging( aTransID, SML_SESS_ENV(aEnv) );

        STL_TRY( ellIsRecentDictHandle( aTransID,
                                        sTableHandle,
                                        & sIsValid,
                                        ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        STL_TRY_THROW( sIsValid == STL_TRUE, RAMP_OBJECT_MODIFIED );

        STL_TRY( smlLockTable( aTransID,
                               sInitPlan->mMode,
                               ellGetTableHandle( sTableHandle ),
                               sInitPlan->mWaitTime,
                               &sTableExist,
                               SML_ENV(aEnv) )
                 == STL_SUCCESS );

        /**
         * Dictionary Cache에서 테이블이 유효하다고 판단했더라도 삭제된 테이블일수 있다.
         * - 테이블 삭제후 SM commit까지 수행한 시점에서는 객체는 삭제되어있고 Dictionary Cache는
         *   유효하다고 판단할수 있다.
         * 반드시 삭제되었는지 확인해야 한다.
         */

        STL_TRY_THROW( sTableExist == STL_TRUE, RAMP_OBJECT_MODIFIED );
        
        STL_TRY( ellIsRecentDictHandle( aTransID,
                                        sTableHandle,
                                        & sIsValid,
                                        ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        STL_TRY_THROW( sIsValid == STL_TRUE, RAMP_OBJECT_MODIFIED );
             
        smlUnsetScnToEnableAging( SML_SESS_ENV(aEnv) );
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_OBJECT_MODIFIED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_OBJECT_BY_CONCURRENT_DDL,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    smlUnsetScnToEnableAging( SML_SESS_ENV(aEnv) );
    
    return STL_FAILURE;
}


/** @} qltxTransaction */
