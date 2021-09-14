/*******************************************************************************
 * qlrAlterTableRenameColumn.c
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
 * @file qlrAlterTableRenameColumn.c
 * @brief ALTER TABLE .. RENAME COLUMN .. 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qlrAlterTableRenameColumn ALTER TABLE .. RENAME COLUMN
 * @ingroup qlrAlterTable
 * @{
 */

/**
 * @brief RENAME COLUMN 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrValidateRenameColumn( smlTransId      aTransID,
                                   qllDBCStmt    * aDBCStmt,
                                   qllStatement  * aSQLStmt,
                                   stlChar       * aSQLString,
                                   qllNode       * aParseTree,
                                   qllEnv        * aEnv )
{
    stlInt32 sState = 0;
    
    ellDictHandle * sAuthHandle = NULL;
    
    qlrInitRenameColumn       * sInitPlan = NULL;
    qlpAlterTableRenameColumn * sParseTree = NULL;

    stlBool         sObjectExist = STL_FALSE;
    
    ellObjectList * sCheckObjectList  = NULL;

    ellDictHandle   sNewColHandle;

    smlStatement  * sStmt = NULL;
    stlInt32        sStmtAttr = 0;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLE_RENAME_COLUMN_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_TABLE_RENAME_COLUMN_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */

    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    sParseTree = (qlpAlterTableRenameColumn *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlrInitRenameColumn),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlrInitRenameColumn) );

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
    
    STL_TRY( qlrGetAlterTableHandle( aTransID,
                                     aDBCStmt,
                                     aSQLStmt,
                                     aSQLString,
                                     sAuthHandle,
                                     sParseTree->mTableName,
                                     & sInitPlan->mSchemaHandle,
                                     & sInitPlan->mTableHandle,
                                     aEnv )
             == STL_SUCCESS );
    
    /**********************************************************
     * Original Column Name Validation
     **********************************************************/

    STL_TRY( ellGetColumnDictHandle( aTransID,
                                     aSQLStmt->mViewSCN,
                                     & sInitPlan->mTableHandle,
                                     QLP_GET_PTR_VALUE( sParseTree->mOrgColName ),
                                     & sInitPlan->mColumnHandle,
                                     & sObjectExist,
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_COLUMN_NOT_EXIST );

    /**********************************************************
     * Column Related Object 획득 
     **********************************************************/

    /**
     * Column 과 관련된 Object 목록 획득
     * - Prepare 후 다른 Session 에 의해 관련 객체가 변경될 수 있으므로
     * - Validation 단계에서 관련 정보를 Init Plan 에 저장하면 안됨.
     */

    STL_TRY( ellGetObjectList4RenameColumn( aTransID,
                                            sStmt,
                                            & sInitPlan->mColumnHandle,
                                            QLL_INIT_PLAN(aDBCStmt),
                                            & sCheckObjectList,
                                            ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * CHECK CONSTRAINTS 에 포함된 컬럼인지 검사
     */
    
    STL_TRY_THROW( ellHasObjectItem( sCheckObjectList ) != STL_TRUE,
                   RAMP_ERR_CAUSED_BY_CHECK_CONSTRAINT );
    
    /**********************************************************
     * New Column Name Validation
     **********************************************************/

    STL_TRY( ellGetColumnDictHandle( aTransID,
                                     aSQLStmt->mViewSCN,
                                     & sInitPlan->mTableHandle,
                                     QLP_GET_PTR_VALUE( sParseTree->mNewColName ),
                                     & sNewColHandle,
                                     & sObjectExist,
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sObjectExist == STL_FALSE, RAMP_ERR_COLUMN_DUPLICATE );

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                stlStrlen( QLP_GET_PTR_VALUE(sParseTree->mNewColName) ) + 1,
                                (void **) & sInitPlan->mNewColumnName,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlStrcpy( sInitPlan->mNewColumnName, QLP_GET_PTR_VALUE(sParseTree->mNewColName) );

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

    STL_CATCH( RAMP_ERR_COLUMN_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_IDENTIFIER,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mOrgColName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      QLP_GET_PTR_VALUE(sParseTree->mOrgColName) );
    }

    STL_CATCH( RAMP_ERR_CAUSED_BY_CHECK_CONSTRAINT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_COLUMN_HAS_CHECK_CONSTRAINT_DEFINED,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mOrgColName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_CATCH( RAMP_ERR_COLUMN_DUPLICATE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_DUPLICATE_COLUMN_NAME,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mNewColName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

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
 * @brief RENAME COLUMN 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrOptCodeRenameColumn( smlTransId      aTransID,
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
 * @brief RENAME COLUMN 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrOptDataRenameColumn( smlTransId      aTransID,
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
 * @brief RENAME COLUMN 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qlrExecuteRenameColumn( smlTransId      aTransID,
                                  smlStatement  * aStmt,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  qllEnv        * aEnv )
{
    qlrInitRenameColumn * sInitPlan = NULL;

    stlTime sTime = 0;
    stlBool sLocked = STL_TRUE;

    ellObjectList * sCheckObjectList   = NULL;
    ellObjectList * sAffectedViewList = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLE_RENAME_COLUMN_TYPE,
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
    
    sInitPlan = (qlrInitRenameColumn *) aSQLStmt->mInitPlan;
    
    /**
     * DDL Lock 획득 
     */

    STL_TRY( ellLock4RenameColumn( aTransID,
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
    
    sInitPlan = (qlrInitRenameColumn *) aSQLStmt->mInitPlan;

    /**********************************************************
     * 관련 객체 정보 획득 및 Lock 획득
     **********************************************************/
    
    /**
     * Column 과 관련된 Object 목록 획득
     * - Prepare 후 다른 Session 에 의해 관련 객체가 변경될 수 있으므로 새로이 정보를 획득해야 함.
     */

    STL_TRY( ellGetObjectList4RenameColumn( aTransID,
                                            aStmt,
                                            & sInitPlan->mColumnHandle,
                                            & QLL_EXEC_DDL_MEM(aEnv),
                                            & sCheckObjectList,
                                            ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * CHECK CONSTRAINTS 에 포함된 컬럼인지 검사
     */
    
    STL_TRY_THROW( ellHasObjectItem( sCheckObjectList ) != STL_TRUE,
                   RAMP_ERR_CAUSED_BY_CHECK_CONSTRAINT );

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
     * Column Dictionary 갱신
     */

    STL_TRY( ellModifyColumnName( aTransID,
                                  aStmt,
                                  ellGetColumnID( & sInitPlan->mColumnHandle ),
                                  sInitPlan->mNewColumnName,
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
    
    /**
     * Table 구조가 변경된 시간을 설정 
     */

    STL_TRY( ellSetTime4ModifyTableElement( aTransID,
                                            aStmt,
                                            & sInitPlan->mTableHandle,
                                            ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /***************************************************************
     * Dictionary Cache 재구성 
     ***************************************************************/

    STL_TRY( ellRefineCache4RenameColumn( aTransID,
                                          aStmt,
                                          & sInitPlan->mTableHandle,
                                          ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CAUSED_BY_CHECK_CONSTRAINT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_COLUMN_HAS_CHECK_CONSTRAINT_DEFINED,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


                               
                               

/** @} qlrAlterTableRenameColumn */
