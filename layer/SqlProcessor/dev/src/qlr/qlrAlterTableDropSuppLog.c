/*******************************************************************************
 * qlrAlterTableDropSuppLog.c
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
 * @file qlrAlterTableDropSuppLog.c
 * @brief ALTER TABLE .. DROP SUPPLEMENTAL LOG 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qlrAlterTableDropSuppLog ALTER TABLE .. DROP SUPPLEMENTAL LOG
 * @ingroup qlrAlterTable
 * @{
 */


/**
 * @brief DROP SUPPLEMENTAL LOG 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrValidateDropSuppLog( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  stlChar       * aSQLString,
                                  qllNode       * aParseTree,
                                  qllEnv        * aEnv )
{
    ellDictHandle * sAuthHandle = NULL;
    
    qlrInitDropSuppLog   * sInitPlan = NULL;
    qlpAlterTableSuppLog * sParseTree = NULL;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLE_DROP_SUPPLOG_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_TABLE_DROP_SUPPLOG_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */

    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    sParseTree = (qlpAlterTableSuppLog *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlrInitDropSuppLog),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlrInitDropSuppLog) );

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
                                     sParseTree->mTableName,
                                     & sInitPlan->mSchemaHandle,
                                     & sInitPlan->mTableHandle,
                                     aEnv )
             == STL_SUCCESS );

    /**********************************************************
     * Supplemental Log Data 설정 여부 확인 
     **********************************************************/

    STL_TRY_THROW( ellIsTableSuppLogPK( & sInitPlan->mTableHandle ) == STL_TRUE,
                   RAMP_ERR_NOT_SET_SUPPLOG_PK );

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

    STL_CATCH( RAMP_ERR_NOT_SET_SUPPLOG_PK )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CANNOT_DROP_NONEXISTENT_PK_SUPPLEMENTAL_LOGGING,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief DROP SUPPLEMENTAL LOG 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrOptCodeDropSuppLog( smlTransId      aTransID,
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
 * @brief DROP SUPPLEMENTAL LOG 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrOptDataDropSuppLog( smlTransId      aTransID,
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
 * @brief DROP SUPPLEMENTAL LOG 구문을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qlrExecuteDropSuppLog( smlTransId      aTransID,
                                 smlStatement  * aStmt,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv )
{
    qlrInitDropSuppLog * sInitPlan = NULL;
    
    stlTime sTime = 0;
    stlBool sLocked = STL_TRUE;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLE_DROP_SUPPLOG_TYPE,
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
    
    sInitPlan = (qlrInitDropSuppLog *) aSQLStmt->mInitPlan;
    
    /**
     * DDL Lock 획득 
     */

    STL_TRY( ellLock4TableSuppLog( aTransID,
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

    sInitPlan = (qlrInitDropSuppLog *) aSQLStmt->mInitPlan;

    /***************************************************************
     * Table 을 SUPPLEMENTAL LOG 를 ON
     ***************************************************************/

    /**
     * Table Dictionary 갱신
     */

    STL_TRY( ellModifyTableSuppLog( aTransID,
                                    aStmt,
                                    ellGetTableID( & sInitPlan->mTableHandle ),
                                    STL_FALSE,    /* OFF */
                                    ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Table 관련 Cache 재구성 
     */

    STL_TRY( ellRefineCache4SuppLogTable( aTransID,
                                          aStmt,
                                          & sInitPlan->mTableHandle,
                                          STL_FALSE,  /* DROP */
                                          ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


                               
                               
/** @} qlrAlterTableDropSuppLog */
