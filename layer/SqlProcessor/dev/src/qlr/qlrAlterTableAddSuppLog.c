/*******************************************************************************
 * qlrAlterTableAddSuppLog.c
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
 * @file qlrAlterTableAddSuppLog.c
 * @brief ALTER TABLE .. ADD SUPPLEMENTAL LOG 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qlrAlterTableAddSuppLog ALTER TABLE .. ADD SUPPLEMENTAL LOG
 * @ingroup qlrAlterTable
 * @{
 */


/**
 * @brief ADD SUPPLEMENTAL LOG 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrValidateAddSuppLog( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 stlChar       * aSQLString,
                                 qllNode       * aParseTree,
                                 qllEnv        * aEnv )
{
    ellDictHandle * sAuthHandle = NULL;
    
    qlrInitAddSuppLog    * sInitPlan = NULL;
    qlpAlterTableSuppLog * sParseTree = NULL;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLE_ADD_SUPPLOG_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_TABLE_ADD_SUPPLOG_TYPE,
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
                                STL_SIZEOF(qlrInitAddSuppLog),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlrInitAddSuppLog) );

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

    STL_TRY_THROW( ellIsTableSuppLogPK( & sInitPlan->mTableHandle ) == STL_FALSE,
                   RAMP_ERR_MULTIPLE_SET_SUPPLOG_PK );

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

    STL_CATCH( RAMP_ERR_MULTIPLE_SET_SUPPLOG_PK )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SUPPLEMENTAL_LOGGING_PK_EXISTS,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief ADD SUPPLEMENTAL LOG 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrOptCodeAddSuppLog( smlTransId      aTransID,
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
 * @brief ADD SUPPLEMENTAL LOG 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrOptDataAddSuppLog( smlTransId      aTransID,
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
 * @brief ADD SUPPLEMENTAL LOG 구문을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qlrExecuteAddSuppLog( smlTransId      aTransID,
                                smlStatement  * aStmt,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv )
{
    qlrInitAddSuppLog * sInitPlan = NULL;
    
    stlTime sTime = 0;
    stlBool sLocked = STL_TRUE;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLE_ADD_SUPPLOG_TYPE,
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
    
    sInitPlan = (qlrInitAddSuppLog *) aSQLStmt->mInitPlan;
    
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

    sInitPlan = (qlrInitAddSuppLog *) aSQLStmt->mInitPlan;

    /***************************************************************
     * Table 을 SUPPLEMENTAL LOG 를 ON
     ***************************************************************/

    /**
     * Table Dictionary 갱신
     */

    STL_TRY( ellModifyTableSuppLog( aTransID,
                                    aStmt,
                                    ellGetTableID( & sInitPlan->mTableHandle ),
                                    STL_TRUE,    /* ON */
                                    ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Table 관련 Cache 재구성 
     */

    STL_TRY( ellRefineCache4SuppLogTable( aTransID,
                                          aStmt,
                                          & sInitPlan->mTableHandle,
                                          STL_TRUE,  /* ADD */
                                          ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


                               

/** @} qlrAlterTableAddSuppLog */
