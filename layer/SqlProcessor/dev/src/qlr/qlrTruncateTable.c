/*******************************************************************************
 * qlrTruncateTable.c
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
 * @file qlrTruncateTable.c
 * @brief TRUNCATE TABLE 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qlrTruncateTable TRUNCATE TABLE
 * @ingroup qlr
 * @{
 */


/**
 * @brief TRUNCATE TABLE 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrValidateTruncateTable( smlTransId      aTransID,
                                    qllDBCStmt    * aDBCStmt,
                                    qllStatement  * aSQLStmt,
                                    stlChar       * aSQLString,
                                    qllNode       * aParseTree,
                                    qllEnv        * aEnv )
{
    stlInt32 sState = 0;
    
    ellDictHandle * sAuthHandle = NULL;
    
    qlrInitTruncateTable * sInitPlan = NULL;
    qlpTruncateTable     * sParseTree = NULL;

    stlBool   sObjectExist = STL_FALSE;

    ellObjectList * sEnableOuterChildForeignKeyList   = NULL;

    smlStatement  * sStmt = NULL;
    stlInt32        sStmtAttr = 0;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_TRUNCATE_TABLE_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_TRUNCATE_TABLE_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */

    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    sParseTree = (qlpTruncateTable *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlrInitTruncateTable),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlrInitTruncateTable) );

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
    
    /**********************************************************
     * Table Name Validation
     **********************************************************/
    
    if ( sParseTree->mName->mSchema == NULL )
    {
        /**
         * Schema 가 명시되지 않은 경우
         */

        STL_TRY( ellGetTableDictHandleWithAuth( aTransID,
                                                aSQLStmt->mViewSCN,
                                                sAuthHandle,
                                                sParseTree->mName->mObject,
                                                & sInitPlan->mTableHandle,
                                                & sObjectExist,
                                                ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_TABLE_NOT_EXIST );

        /**
         * Table 의 Schema Handle 획득
         */
        
        STL_TRY( ellGetSchemaDictHandleByID(
                     aTransID,
                     aSQLStmt->mViewSCN,
                     ellGetTableSchemaID( & sInitPlan->mTableHandle ),
                     & sInitPlan->mSchemaHandle,
                     & sObjectExist,
                     ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_TABLE_NOT_EXIST );
    }
    else
    {
        /**
         * Schema 가 명시된 경우
         */

        STL_TRY( ellGetSchemaDictHandle( aTransID,
                                         aSQLStmt->mViewSCN,
                                         sParseTree->mName->mSchema,
                                         & sInitPlan->mSchemaHandle,
                                         & sObjectExist,
                                         ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_SCHEMA_NOT_EXIST );
        
        STL_TRY( ellGetTableDictHandleWithSchema( aTransID,
                                                  aSQLStmt->mViewSCN,
                                                  & sInitPlan->mSchemaHandle,
                                                  sParseTree->mName->mObject,
                                                  & sInitPlan->mTableHandle,
                                                  & sObjectExist,
                                                  ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY_THROW ( sObjectExist == STL_TRUE, RAMP_ERR_TABLE_NOT_EXIST );
    }

    /**
     * Built-In Table 인지 검사
     */

    STL_TRY_THROW( ellIsBuiltInTable( & sInitPlan->mTableHandle ) == STL_FALSE,
                   RAMP_ERR_BUILT_IN_TABLE );

    /**
     * Base Table 인지 검사
     */
    
    STL_TRY_THROW( ELL_TABLE_TYPE_BASE_TABLE == ellGetTableType( & sInitPlan->mTableHandle ),
                   RAMP_ERR_NOT_BASE_TABLE );
    
    /**
     * DROP TABLE 권한 검사
     */

    STL_TRY( qlaCheckPrivDropTable( aTransID,
                                    aDBCStmt,
                                    aSQLStmt,
                                    & sInitPlan->mTableHandle,
                                    aEnv )
             == STL_SUCCESS );
    

    /**********************************************************
     * Table 관련 객체 획득 
     **********************************************************/

    STL_TRY( ellGetObjectList4TruncateTable( aTransID,
                                             sStmt,
                                             & sInitPlan->mTableHandle,
                                             QLL_INIT_PLAN(aDBCStmt),
                                             & sEnableOuterChildForeignKeyList,
                                             ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Enabled Outer Child Foreign Key 가 존재하면 안됨
     * @todo CASCADE 옵션과 함께 다시 고민해야 함 (Disable 은 ??)
     */
    
    STL_TRY_THROW( ellHasObjectItem( sEnableOuterChildForeignKeyList ) == STL_FALSE,
                   RAMP_ERR_CAUSED_BY_FOREIGN_KEY );
    
    /**********************************************************
     * Identity Restart Option
     **********************************************************/

    if ( sParseTree->mIdentityRestart == NULL )
    {
        sInitPlan->mIdentityRestart = QLP_IDENTITY_RESTART_OPTION_CONTINUE;
    }
    else
    {
        sInitPlan->mIdentityRestart = QLP_GET_INT_VALUE( sParseTree->mIdentityRestart );
    }

    switch ( sInitPlan->mIdentityRestart )
    {
        case QLP_IDENTITY_RESTART_OPTION_CONTINUE:
            break;
        case QLP_IDENTITY_RESTART_OPTION_RESTART:
            break;
        default:
            STL_ASSERT(0);
            break;
    }

    /**********************************************************
     * Drop Storage Option
     **********************************************************/

    if ( sParseTree->mDropStorage == NULL )
    {
        sInitPlan->mDropStorage = QLP_DROP_STORAGE_DROP;
    }
    else
    {
        sInitPlan->mDropStorage = QLP_GET_INT_VALUE( sParseTree->mDropStorage );
    }

    switch ( sInitPlan->mDropStorage )
    {
        case QLP_DROP_STORAGE_DROP:
            break;
        case QLP_DROP_STORAGE_DROP_ALL:
            break;
        case QLP_DROP_STORAGE_REUSE:
            /**
             * @todo REUSE STORAGE option 을 구현해야 함.
             */
            STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
            break;
        default:
            STL_ASSERT(0);
            break;
    }

    /**
     * Statement 해제
     */

    sState = 0;
    STL_TRY( smlFreeStatement( sStmt, SML_ENV(aEnv) ) == STL_SUCCESS );
    sStmt = NULL;

    /********************************************************
     * Validation Object 추가 
     ********************************************************/

    STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                  & sInitPlan->mTableHandle,
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

    STL_CATCH( RAMP_ERR_TABLE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLE_OR_VIEW_NOT_EXIST,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_SCHEMA_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mSchemaPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sParseTree->mName->mSchema );
    }

    STL_CATCH( RAMP_ERR_BUILT_IN_TABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_BUILT_IN_TABLE_IS_NOT_MODIFIABLE,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sParseTree->mName->mObject );
    }

    STL_CATCH( RAMP_ERR_NOT_BASE_TABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_IDENTIFIER,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sParseTree->mName->mObject );
    }
    
    STL_CATCH( RAMP_ERR_CAUSED_BY_FOREIGN_KEY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLE_KEY_REFERENCED_BY_FOREIGN_KEY,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      "qlrValidateTruncateTable()" );
    }
    
    STL_FINISH;

    /**
     * Statement 해제
     */

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
 * @brief TRUNCATE TABLE 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrOptCodeTruncateTable( smlTransId      aTransID,
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
 * @brief TRUNCATE TABLE 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrOptDataTruncateTable( smlTransId      aTransID,
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
 * @brief TRUNCATE TABLE 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qlrExecuteTruncateTable( smlTransId      aTransID,
                                   smlStatement  * aStmt,
                                   qllDBCStmt    * aDBCStmt,
                                   qllStatement  * aSQLStmt,
                                   qllEnv        * aEnv )
{
    qlrInitTruncateTable * sInitPlan = NULL;

    stlTime sTime = 0;
    stlBool sLocked = STL_TRUE;

    smlDropStorageOption  sPhyBehavior = SML_DROP_STORAGE_NONE;

    ellObjectList * sEnableOuterChildForeignKeyList   = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_TRUNCATE_TABLE_TYPE,
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

    sInitPlan = (qlrInitTruncateTable *) aSQLStmt->mInitPlan;
    
    /**
     * DDL Lock 획득 
     */

    STL_TRY( ellLock4TruncateTable( aTransID,
                                    aStmt,
                                    & sInitPlan->mTableHandle,
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

    sInitPlan = (qlrInitTruncateTable *) aSQLStmt->mInitPlan;
    
    
    STL_TRY( ellGetObjectList4TruncateTable( aTransID,
                                             aStmt,
                                             & sInitPlan->mTableHandle,
                                             & QLL_EXEC_DDL_MEM(aEnv),
                                             & sEnableOuterChildForeignKeyList,
                                             ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Enabled Outer Child Foreign Key 가 존재하면 안됨
     */
    
    STL_TRY_THROW( ellHasObjectItem( sEnableOuterChildForeignKeyList ) == STL_FALSE,
                   RAMP_ERR_CAUSED_BY_FOREIGN_KEY );
    
    /***************************************************************
     *  Truncate Behavior 정보 설정 
     ***************************************************************/
    
    switch ( sInitPlan->mDropStorage )
    {
        case QLP_DROP_STORAGE_DROP:
            sPhyBehavior = SML_DROP_STORAGE_DROP;
            break;
        case QLP_DROP_STORAGE_DROP_ALL:
            sPhyBehavior = SML_DROP_STORAGE_DROP_ALL;
            break;
        case QLP_DROP_STORAGE_REUSE:
            sPhyBehavior = SML_DROP_STORAGE_REUSE;
            break;
        default:
            STL_ASSERT(0);
            break;
    }

    /***************************************************************
     *  현재 Table 을 Truncate
     ***************************************************************/

    /**
     * 현재 Table 을 Truncate
     */
    
    STL_TRY( qlrTruncateOneTable( aTransID,
                                  aStmt,
                                  aDBCStmt,
                                  & sInitPlan->mTableHandle,
                                  sPhyBehavior,
                                  sInitPlan->mIdentityRestart,
                                  aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CAUSED_BY_FOREIGN_KEY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLE_KEY_REFERENCED_BY_FOREIGN_KEY,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}




/**
 * @brief TRUNCATE 과 관련된 하나의 Table 을 Truncate 한다.
 * @param[in] aTransID         Transaction ID
 * @param[in] aStmt            Statement
 * @param[in] aDBCStmt         DBC Statement
 * @param[in] aTableHandle     Table Handle
 * @param[in] aPhyBehavior     Truncate Behavior
 * @param[in] aIdentityRestart Identity Restart Option
 * @param[in] aEnv             Environment
 * @remarks
 */
stlStatus qlrTruncateOneTable( smlTransId             aTransID,
                               smlStatement         * aStmt,
                               qllDBCStmt           * aDBCStmt,
                               ellDictHandle        * aTableHandle,
                               smlDropStorageOption   aPhyBehavior,
                               qlpIdentityRestart     aIdentityRestart,
                               qllEnv               * aEnv )
{
    stlInt32        sColumnCnt = 0;
    ellDictHandle * sColumnHandle = NULL;
    void          * sIdentityHandle = NULL;

    ellObjectList * sIndexObjectList = NULL;
    ellDictHandle * sIndexHandle = NULL;

    stlInt64        sNewIndexPhyID = 0;
    void          * sNewIndexPhyHandle = NULL;

    stlInt64        sNewTablePhyID = 0;
    void          * sNewTablePhyHandle = NULL;

    stlInt64        sNewIdentityPhyID = 0;
    void          * sNewIdentityHandle = NULL;
    smlSequenceAttr sSeqAttr;    
    stlInt32        i = 0;
        
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableHandle != NULL, QLL_ERROR_STACK(aEnv) );

    /***************************************************************
     *  시쿼스 옵션 변경 
     ***************************************************************/
    
    /**
     * Identity Column 이 존재할 경우 Identity Truncate Option 을 적용해야 함.
     */
    
    sColumnCnt = ellGetTableColumnCnt( aTableHandle );
    sColumnHandle = ellGetTableColumnDictHandle( aTableHandle );

    for ( i = 0; i < sColumnCnt; i++ )
    {
        sIdentityHandle = ellGetColumnIdentityHandle( & sColumnHandle[i] );
        
        if( sIdentityHandle != NULL )
        {
            switch ( aIdentityRestart )
            {
                case QLP_IDENTITY_RESTART_OPTION_CONTINUE:
                    {
                        /**
                         * 그냥 둔다
                         */
                        break;
                    }
                case QLP_IDENTITY_RESTART_OPTION_RESTART:
                    {
                        /**
                         * Identity 를 재시작하도록 설정한다.
                         */

                        stlMemset( & sSeqAttr, 0x00, STL_SIZEOF(smlSequenceAttr) );
                        sSeqAttr.mValidFlags |= SML_SEQ_RESTART_DEFAULT;
                        
                        STL_TRY( smlAlterSequence( aStmt,
                                                   sIdentityHandle,
                                                   & sSeqAttr,           
                                                   & sNewIdentityPhyID,  
                                                   & sNewIdentityHandle, 
                                                   SML_ENV(aEnv) )
                                 == STL_SUCCESS );

                        /**
                         * Identity Column Dictionary 갱신
                         */
                        
                        STL_TRY( ellModifyColumnAlterIdentity( aTransID,
                                                               aStmt,
                                                               ellGetColumnID( & sColumnHandle[i] ),
                                                               NULL, /* aIdentityGenOption */
                                                               NULL, /* aIdentityStart */
                                                               NULL, /* aIdentityIncrement */
                                                               NULL, /* aIdentityMaximum */
                                                               NULL, /* aIdentityMinimum */
                                                               NULL, /* aIdentityCycle */
                                                               & sNewIdentityPhyID, 
                                                               NULL, /* aIdentityCacheSize */
                                                               ELL_ENV(aEnv) )
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
        else
        {
            /* Normal Column 임 */
        }
    }
    
    /***************************************************************
     *  테이블 재구성 
     ***************************************************************/

    /**
     * Table 재구성 
     */
    STL_TRY( smlTruncateTable( aStmt,
                               ellGetTableHandle( aTableHandle ),
                               aPhyBehavior,
                               & sNewTablePhyHandle,
                               & sNewTablePhyID,
                               SML_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Table Dictionary 갱신
     */
    
    STL_TRY( ellModifyTablePhysicalID( aTransID,
                                       aStmt,
                                       ellGetTableID( aTableHandle ),
                                       sNewTablePhyID,
                                       ELL_ENV(aEnv) )
             == STL_SUCCESS );
                                       

    /***************************************************************
     *  인덱스 재구성  
     ***************************************************************/

    /**
     * Table 과 관련된 Index 목록 획득
     */

    STL_TRY( ellGetIndexList4TruncateTable( aTransID,
                                            aStmt,
                                            aTableHandle,
                                            & QLL_EXEC_DDL_MEM(aEnv),
                                            & sIndexObjectList,
                                            ELL_ENV(aEnv) )
             == STL_SUCCESS );

    i = 0;
    while ( 1 )
    {
        sIndexHandle = ellGetNthObjectHandle( sIndexObjectList, i );

        if ( sIndexHandle == NULL )
        {
            break;
        }
        else
        {
            /**
             * Index 재구성
             */
            STL_TRY( smlTruncateIndex( aStmt,
                                       ellGetIndexHandle(sIndexHandle),
                                       sNewTablePhyHandle,
                                       aPhyBehavior,
                                       & sNewIndexPhyHandle,
                                       & sNewIndexPhyID,
                                       SML_ENV(aEnv) )
                     == STL_SUCCESS );

            /**
             * Index Dictionary 갱신
             */
            
            STL_TRY( ellModifyIndexPhysicalID( aTransID,
                                               aStmt,
                                               ellGetIndexID(sIndexHandle),
                                               sNewIndexPhyID,
                                               ELL_ENV(aEnv) )
                     == STL_SUCCESS );
        }

        i++;
    }
    
    /***************************************************************
     *  Table 관련 Cache 재구성 
     ***************************************************************/

    STL_TRY( ellRefineCache4TruncateTable( aTransID,
                                           aStmt,
                                           sNewTablePhyID,
                                           aTableHandle,
                                           ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
                               
                               

/** @} qlrTruncateTable */
