/*******************************************************************************
 * qlrAlterTableRenameTable.c
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
 * @file qlrAlterTableRenameTable.c
 * @brief ALTER TABLE .. RENAME TO .. 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>



/**
 * @defgroup qlrAlterTableRenameTable ALTER TABLE .. RENAME TO ..
 * @ingroup qlrAlterTable
 * @{
 */


/**
 * @brief RENAME TABLE 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrValidateRenameTable( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  stlChar       * aSQLString,
                                  qllNode       * aParseTree,
                                  qllEnv        * aEnv )
{
    ellDictHandle * sAuthHandle = NULL;
    
    qlrInitRenameTable       * sInitPlan = NULL;
    qlpAlterTableRenameTable * sParseTree = NULL;

    stlBool   sObjectExist = STL_FALSE;

    ellDictHandle   sNewTableHandle;
    
    ellInitDictHandle( & sNewTableHandle );
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLE_RENAME_TABLE_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_TABLE_RENAME_TABLE_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */

    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    sParseTree = (qlpAlterTableRenameTable *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlrInitRenameTable),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlrInitRenameTable) );

    STL_TRY( qlvSetInitPlan( aDBCStmt,
                             aSQLStmt,
                             & sInitPlan->mCommonInit,
                             aEnv )
             == STL_SUCCESS );
    
    /**********************************************************
     * Table Name Validation
     **********************************************************/

    STL_TRY( qlrGetAlterTableHandle( aTransID,
                                     aDBCStmt,
                                     aSQLStmt,
                                     aSQLString,
                                     sAuthHandle,
                                     sParseTree->mOrgName,
                                     & sInitPlan->mSchemaHandle,
                                     & sInitPlan->mTableHandle,
                                     aEnv )
             == STL_SUCCESS );
    
    /**********************************************************
     * New Table 에 대한 Validation
     **********************************************************/

    /**
     * Table 존재 여부 확인
     */

    STL_TRY( ellGetTableDictHandleWithSchema( aTransID,
                                              aSQLStmt->mViewSCN,
                                              & sInitPlan->mSchemaHandle,
                                              QLP_GET_PTR_VALUE(sParseTree->mNewName),
                                              & sNewTableHandle,
                                              & sObjectExist,
                                              ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sObjectExist != STL_TRUE, RAMP_ERR_SAME_NAME_TABLE_EXISTS );

    /**
     * 같은 이름을 가진 Sequence 가 있는지 확인
     * - Oracle 호환성을 위해 Name Space 를 Table 과 Sequence 를 같이 사용한다.
     * - 호환성을 위해 Dictionary 체계를 바꾸는 것보다 QP 단에서 검사한다.
     */

    STL_TRY( ellGetSequenceDictHandleWithSchema( aTransID,
                                                 aSQLStmt->mViewSCN,
                                                 & sInitPlan->mSchemaHandle,
                                                 QLP_GET_PTR_VALUE(sParseTree->mNewName),
                                                 & sNewTableHandle,
                                                 & sObjectExist,
                                                 ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sObjectExist != STL_TRUE, RAMP_ERR_SAME_NAME_SEQUENCE_EXISTS );
    
    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                stlStrlen( QLP_GET_PTR_VALUE(sParseTree->mNewName) ) + 1,
                                (void **) & sInitPlan->mNewTableName,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlStrcpy( sInitPlan->mNewTableName, QLP_GET_PTR_VALUE(sParseTree->mNewName) );

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

    STL_CATCH( RAMP_ERR_SAME_NAME_TABLE_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_USED_OBJECT_NAME,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mNewName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      ellGetSchemaName( & sInitPlan->mSchemaHandle ),
                      QLP_GET_PTR_VALUE(sParseTree->mNewName) );
    }

    
    STL_CATCH( RAMP_ERR_SAME_NAME_SEQUENCE_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_USED_OBJECT_NAME,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mNewName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      ellGetSchemaName( & sInitPlan->mSchemaHandle ),
                      QLP_GET_PTR_VALUE(sParseTree->mNewName) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief RENAME TABLE 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrOptCodeRenameTable( smlTransId      aTransID,
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
 * @brief RENAME TABLE 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrOptDataRenameTable( smlTransId      aTransID,
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
 * @brief RENAME TABLE 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qlrExecuteRenameTable( smlTransId      aTransID,
                                 smlStatement  * aStmt,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv )
{
    qlrInitRenameTable * sInitPlan = NULL;

    ellObjectList * sAffectedViewList = NULL;
    
    stlTime sTime = 0;
    stlBool sLocked = STL_TRUE;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLE_RENAME_TABLE_TYPE,
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
    
    sInitPlan = (qlrInitRenameTable *) aSQLStmt->mInitPlan;

    
    /**
     * DDL Lock 획득 
     */

    STL_TRY( ellLock4RenameTable( aTransID,
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
    
    sInitPlan = (qlrInitRenameTable *) aSQLStmt->mInitPlan;

    /**********************************************************
     * 관련 객체 정보 획득 및 Lock 획득
     **********************************************************/
    
    /**
     * Affected View List 획득 및 X Lock 획득
     */
    
    STL_TRY( ellGetAffectedViewListByRelation( aTransID,
                                               aStmt,
                                               & sInitPlan->mTableHandle,
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
                                               & sInitPlan->mTableHandle,
                                               & QLL_EXEC_DDL_MEM(aEnv),
                                               & sAffectedViewList,
                                               ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /***************************************************************
     * Dictionary Record 갱신
     ***************************************************************/

    /**
     * Table Dictionary 갱신
     */

    STL_TRY( ellModifyTableName( aTransID,
                                 aStmt,
                                 ellGetTableID( & sInitPlan->mTableHandle ),
                                 sInitPlan->mNewTableName,
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
     * Table 관련 Cache 재구성 
     */

    STL_TRY( ellRefineCache4RenameTable( aTransID,
                                         aStmt,
                                         & sInitPlan->mTableHandle,
                                         ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief ALTER TABLE table_name 에 대한 Handle 을 획득한다.
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aDBCStmt      DBC Statement
 * @param[in]  aSQLStmt      SQL Statement
 * @param[in]  aSQLString    SQL String
 * @param[in]  aAuthHandle   Authorization Handle
 * @param[in]  aTableName    ParseTree for Table Name
 * @param[out] aSchemaHandle Schema Handle
 * @param[out] aTableHandle  Table Handle
 * @param[in]  aEnv          Environment
 * @remarks
 */
stlStatus qlrGetAlterTableHandle( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  stlChar       * aSQLString,
                                  ellDictHandle * aAuthHandle,
                                  qlpObjectName * aTableName,
                                  ellDictHandle * aSchemaHandle,
                                  ellDictHandle * aTableHandle,
                                  qllEnv        * aEnv )
{
    stlBool  sObjectExist = STL_FALSE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAuthHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableName  != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableHandle  != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Quantified Table Name 이 존재하는 지 검사
     */
    
    if ( aTableName->mSchema == NULL )
    {
        /**
         * Schema 가 명시되지 않은 경우
         */

        STL_TRY( ellGetTableDictHandleWithAuth( aTransID,
                                                aSQLStmt->mViewSCN,
                                                aAuthHandle,
                                                aTableName->mObject,
                                                aTableHandle,
                                                & sObjectExist,
                                                ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_TABLE_NOT_EXIST );

        /**
         * Table 의 Schema Handle 획득
         */
        
        STL_TRY( ellGetSchemaDictHandleByID( aTransID,
                                             aSQLStmt->mViewSCN,
                                             ellGetTableSchemaID( aTableHandle ),
                                             aSchemaHandle,
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
                                         aTableName->mSchema,
                                         aSchemaHandle,
                                         & sObjectExist,
                                         ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_SCHEMA_NOT_EXIST );
        
        STL_TRY( ellGetTableDictHandleWithSchema( aTransID,
                                                  aSQLStmt->mViewSCN,
                                                  aSchemaHandle,
                                                  aTableName->mObject,
                                                  aTableHandle,
                                                  & sObjectExist,
                                                  ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY_THROW ( sObjectExist == STL_TRUE, RAMP_ERR_TABLE_NOT_EXIST );
    }

    /**
     * Built-In Table 인지 검사
     */

    STL_TRY_THROW( ellIsBuiltInTable( aTableHandle ) == STL_FALSE,
                   RAMP_ERR_BUILT_IN_TABLE );

    /**
     * Base Table 인지 검사
     */
    
    STL_TRY_THROW( ELL_TABLE_TYPE_BASE_TABLE == ellGetTableType( aTableHandle ),
                   RAMP_ERR_NOT_BASE_TABLE );
    
    
    /**
     * ALTER ON TABLE 권한 검사
     */

    STL_TRY( qlaCheckTablePriv( aTransID,
                                aDBCStmt,
                                aSQLStmt,
                                ELL_TABLE_PRIV_ACTION_ALTER,
                                aTableHandle,
                                NULL, /* aColumnList */
                                aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_TABLE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLE_OR_VIEW_NOT_EXIST,
                      qlgMakeErrPosString( aSQLString,
                                           aTableName->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_SCHEMA_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           aTableName->mSchemaPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      aTableName->mSchema );
    }

    STL_CATCH( RAMP_ERR_BUILT_IN_TABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_BUILT_IN_TABLE_IS_NOT_MODIFIABLE,
                      qlgMakeErrPosString( aSQLString,
                                           aTableName->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      aTableName->mObject );
    }

    STL_CATCH( RAMP_ERR_NOT_BASE_TABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_IDENTIFIER,
                      qlgMakeErrPosString( aSQLString,
                                           aTableName->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      aTableName->mObject );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}
                               
                               

/** @} qlrAlterTableRenameTable */
