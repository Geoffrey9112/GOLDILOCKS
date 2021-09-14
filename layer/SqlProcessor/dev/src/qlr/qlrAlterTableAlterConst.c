/*******************************************************************************
 * qlrAlterTableAlterConst.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlrAlterTableAlterConst.c 5812 2012-09-27 07:56:09Z bsyou $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlrAlterTableAlterConst.c
 * @brief ALTER TABLE .. ALTER CONSTRAINT .. 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qlrAlterTableAlterConst ALTER TABLE .. DROP CONSTRAINT
 * @ingroup qlrAlterTable
 * @{
 */


/**
 * @brief ALTER CONSTRAINT 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrValidateAlterConst( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 stlChar       * aSQLString,
                                 qllNode       * aParseTree,
                                 qllEnv        * aEnv )
{
    ellDictHandle * sAuthHandle = NULL;
    
    qlrInitAlterConst            * sInitPlan = NULL;
    qlpAlterTableAlterConstraint * sParseTree = NULL;
    qlpConstraintAttr            * sConstAttr = NULL;
    stlInt64  sBigintValue = 0;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLE_ALTER_CONSTRAINT_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_TABLE_ALTER_CONSTRAINT_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */

    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    sParseTree = (qlpAlterTableAlterConstraint *) aParseTree;
    sConstAttr = sParseTree->mConstAttr;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlrInitAlterConst),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlrInitAlterConst) );

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
     * Constraint Object Validation
     **********************************************************/

    STL_TRY( qlrGetDropConstHandle( aTransID,
                                    aDBCStmt,
                                    aSQLStmt,
                                    aSQLString,
                                    & sInitPlan->mTableHandle,
                                    sParseTree->mConstObject,
                                    & sInitPlan->mConstHandle,
                                    aEnv )
             == STL_SUCCESS );

    /**********************************************************
     * Constraint Characteristics 정보 획득
     **********************************************************/

    /**
     * INITIALLY DEFERRED / INITIALLY IMMEDIATE
     */
    
    if ( sConstAttr->mInitDeferred == NULL )
    {
        sInitPlan->mSetInitDeferred = STL_FALSE;
        sInitPlan->mInitDeferred = ELL_TABLE_CONSTRAINT_INITIALLY_DEFERRED_DEFAULT;
        
    }
    else
    {
        sInitPlan->mSetInitDeferred = STL_TRUE;
        
        sBigintValue = QLP_GET_INT_VALUE( sConstAttr->mInitDeferred );
        sInitPlan->mInitDeferred = (stlBool) sBigintValue;
    }
    
    /**
     * DEFERRABLE / NOT DEFERRABLE
     */

    if ( sConstAttr->mDeferrable == NULL )
    {
        sInitPlan->mSetDeferrable = STL_FALSE;
        sInitPlan->mDeferrable      = ELL_TABLE_CONSTRAINT_DEFERRABLE_DEFAULT;
        
        
    }
    else
    {
        sInitPlan->mSetDeferrable = STL_TRUE;
        
        sBigintValue = QLP_GET_INT_VALUE( sConstAttr->mDeferrable );
        sInitPlan->mDeferrable = (stlBool) sBigintValue;

        if ( (sInitPlan->mDeferrable == STL_FALSE) &&
             (sInitPlan->mSetInitDeferred == STL_TRUE) &&
             (sInitPlan->mInitDeferred == STL_TRUE) )
        {
            /**
             * NOT DEFERRABLE 과 INITIALLY DEFERRED 는 함께 기술할 수 없다.
             */

            STL_THROW( RAMP_ERR_CANNOT_DEFER_NOT_DEFERRABLE );
        }
        else
        {
            /* ok */
        }
    }
    
    /**
     * @todo 향후 constraint enforcement 구현해야 함
     */
    
    STL_TRY_THROW ( sConstAttr->mEnforce == NULL, RAMP_ERR_NOT_IMPLEMENTED );
    
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

    STL_CATCH( RAMP_ERR_CANNOT_DEFER_NOT_DEFERRABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CANNOT_DEFER_CONSTRAINT_THAT_IS_NOT_DEFERRABLE,
                      qlgMakeErrPosString( aSQLString,
                                           sConstAttr->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      "qlrValidateAlterConst()" );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief ALTER CONSTRAINT 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrOptCodeAlterConst( smlTransId      aTransID,
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
 * @brief ALTER CONSTRAINT 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrOptDataAlterConst( smlTransId      aTransID,
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
 * @brief ALTER CONSTRAINT 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qlrExecuteAlterConst( smlTransId      aTransID,
                                smlStatement  * aStmt,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv )
{
    qlrInitAlterConst * sInitPlan = NULL;

    stlTime sTime = 0;
    stlBool sLocked = STL_TRUE;

    stlBool sIsDeferrable   = STL_FALSE;
    stlBool sIsInitDeferred = STL_FALSE;

    ellTableConstraintType sConstType = ELL_TABLE_CONSTRAINT_TYPE_NA;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLE_ALTER_CONSTRAINT_TYPE,
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
     *  Run-Time Validation : Plan
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
    
    sInitPlan = (qlrInitAlterConst *) aSQLStmt->mInitPlan;
    
    /**
     * DDL Lock 획득 
     */

    STL_TRY( ellLock4AlterConst( aTransID,
                                 aStmt,
                                 & sInitPlan->mTableHandle,
                                 & sInitPlan->mConstHandle,
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

    sInitPlan = (qlrInitAlterConst *) aSQLStmt->mInitPlan;

    /***************************************************************
     *  Run-Time Validation : Constraint Characteristics
     ***************************************************************/
    
    /**
     * Constraint Characteristics 정보 획득
     */

    if ( sInitPlan->mSetDeferrable == STL_TRUE )
    {
        sIsDeferrable = sInitPlan->mDeferrable;
    }
    else
    {
        sIsDeferrable = ellGetConstraintAttrIsDeferrable( & sInitPlan->mConstHandle );
    }

    if ( sInitPlan->mSetInitDeferred == STL_TRUE )
    {
        sIsInitDeferred = sInitPlan->mInitDeferred;
    }
    else
    {
        sIsInitDeferred = ellGetConstraintAttrIsInitDeferred( & sInitPlan->mConstHandle );
    }

    /**
     * NOT DEFERRABLE 은 INIT DEFERRED 로 설정할 수 없음
     */
    
    if ( (sIsDeferrable == STL_FALSE) && (sIsInitDeferred == STL_TRUE ) )
    {
        STL_THROW( RAMP_ERR_CANNOT_DEFER_NOT_DEFERRABLE );
    }
    else
    {
        /* ok */
    }

    /***************************************************************
     * Dictionary Record 변경
     ***************************************************************/

    
    STL_TRY( ellModifyDict4AlterConstFeature( aTransID,
                                              aStmt,
                                              & sInitPlan->mConstHandle,
                                              sIsDeferrable,
                                              sIsInitDeferred,
                                              STL_TRUE, /* aEnforced */
                                              ELL_ENV(aEnv) )
             == STL_SUCCESS );
    

    /**
     * Table 구조가 변경된 시간을 설정
     * - Table Constraint 의 변경을 Table 변경 시간에 적용할 경우,
     * - 동시 수행시 table descriptor 를 위한 dictionary record 변경이 함께 발생하므로,
     * - 충분히 동시에 수행할 수 있는 ADD/DROP UNIQUE KEY 등을 동시에 수행할 수 없게 된다.
     */

    sConstType = ellGetConstraintType( & sInitPlan->mConstHandle );
    
    switch( sConstType )
    {
        case ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY:
            STL_TRY( ellSetTime4ModifyTableElement( aTransID,
                                                    aStmt,
                                                    & sInitPlan->mTableHandle,
                                                    ELL_ENV(aEnv) )
                     == STL_SUCCESS );
            break;
        case ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY:
            /**
             * 동시에 수행할 수 있다.
             */
            break;
        case ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY:
            /**
             * 동시에 수행할 수 있다.
             */
            break;
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL:
            STL_TRY( ellSetTime4ModifyTableElement( aTransID,
                                                    aStmt,
                                                    & sInitPlan->mTableHandle,
                                                    ELL_ENV(aEnv) )
                     == STL_SUCCESS );
            break;
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_CLAUSE:
            /**
             * 동시에 수행할 수 있다.
             */
            break;
        default:
            STL_ASSERT(0);
            break;
    }

    /***************************************************************
     * Cache Refinement
     ***************************************************************/

    /**
     * ALTER Constraint 에 대한 Refine Cache
     */
    
    STL_TRY( ellRefineCache4AlterConstraint( aTransID,
                                             aStmt,
                                             & sInitPlan->mTableHandle,
                                             & sInitPlan->mConstHandle,
                                             ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CANNOT_DEFER_NOT_DEFERRABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CANNOT_DEFER_CONSTRAINT_THAT_IS_NOT_DEFERRABLE,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}




/** @} qlrAlterTableAlterConst */
