/*******************************************************************************
 * qlxInsertSelect.c
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
 * @file qlxInsertSelect.c
 * @brief SQL Processor Layer INSERT .. SELECT statement execution
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @addtogroup qlxInsertSelect
 * @{
 */


/**
 * @brief INSERT .. SELECT 구문을 실행한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aStmt      Statement
 * @param[in] aDBCStmt   DBC Statement 
 * @param[in] aSQLStmt   SQL Statement 객체
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlxExecuteInsertSelect( smlTransId      aTransID,
                                  smlStatement  * aStmt,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  qllEnv        * aEnv )
{
    qlvInitInsertSelect  * sInitPlan       = NULL;
    qllOptimizationNode  * sOptNode       = NULL;
    qllDataArea          * sDataArea      = NULL;

    qllResultSetDesc       * sResultSetDesc = NULL;
    qlnInstantTable        * sInstantTable = NULL;

    stlBool             sIsScrollableInstant = STL_FALSE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_INSERT_SELECT_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_INTO_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_QUERY_TYPE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mDataPlan != NULL, QLL_ERROR_STACK(aEnv) );


    /******************************************************
     * 정보 획득 
     ******************************************************/
    
    /**
     * 검증된 Plan 정보 획득 
     */

    STL_TRY( qlxInsertSelectGetValidPlan( aTransID,
                                          aDBCStmt,
                                          aSQLStmt,
                                          QLL_PHASE_EXECUTE,
                                          & sInitPlan,
                                          & sOptNode,
                                          & sDataArea,
                                          aEnv )
             == STL_SUCCESS );

    /**
     * Update Relation Handle 설정
     */

    STL_TRY( smlUpdateRelHandle( aStmt,
                                 sInitPlan->mTablePhyHandle,
                                 SML_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**************************************************
     * Instant Table 생성 
     **************************************************/

    if ( aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_QUERY_TYPE )
    {
        sResultSetDesc = aSQLStmt->mResultSetDesc;
        sInstantTable  = sResultSetDesc->mInstantTable;

        if ( sResultSetDesc->mCursorProperty.mScrollability == ELL_CURSOR_SCROLLABILITY_SCROLL )
        {
            sIsScrollableInstant = STL_TRUE;
        }
        else
        {
            sIsScrollableInstant = STL_FALSE;
        }

        stlMemset( sInstantTable, 0x00, STL_SIZEOF( qlnInstantTable ) );
        
        STL_TRY( qlnxCreateFlatInstantTable( aTransID,
                                             aStmt,
                                             sInstantTable,
                                             sIsScrollableInstant,
                                             aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /**
         * nothing to do
         */
    }

    /**
     * transaction id 설정
     */
    
    sDataArea->mTransID = aTransID;    

    /******************************************************
     * Initialize Execution Node 
     ******************************************************/

    if( sDataArea->mIsFirstExecution == STL_TRUE )
    {
        QLNX_INITIALIZE_NODE( aTransID,
                              aStmt,
                              aDBCStmt,
                              aSQLStmt,
                              sOptNode,
                              sDataArea,
                              aEnv );

        /* 초기화 Flag 설정 */
        sDataArea->mIsFirstExecution = STL_FALSE;
    }
    else
    {
        /* Do Nothing */
    }

    
    /******************************************************
     * Execute Execution Node 
     ******************************************************/

    QLNX_EXECUTE_NODE( aTransID,
                       aStmt,
                       aDBCStmt,
                       aSQLStmt,
                       sOptNode,
                       sDataArea,
                       aEnv );


    /******************************************************
     * RETURN / INTO 절 처리
     ******************************************************/

    /**
     * 상위 RETURN 절 처리에서 INTO 블럭에서 처리했음
     */


    /******************************************************
     * Finalize Execution Node
     ******************************************************/

    QLNX_FINALIZE_NODE( sOptNode,
                        sDataArea,
                        aEnv );

    
    return STL_SUCCESS;

    STL_FINISH;

    if ( sInstantTable != NULL )
    {
        (void) qlnxDropInstantTable( sInstantTable,
                                     aEnv );
    }
    
    /* Execution Node 종료 */
    QLNX_FINALIZE_NODE_IGNORE_FAILURE( sOptNode,
                                       sDataArea,
                                       aEnv );

    return STL_FAILURE;
}


/**
 * @brief INSERT .. SELECT 수행을 위한 Lock 을 잡고 유효한 Plan 을 얻는다.
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aDBCStmt      DBC Statement
 * @param[in]  aSQLStmt      SQL Statement
 * @param[in]  aQueryPhase   Query Processing Phase
 * @param[out] aInitPlan  INSERT VALUES Init Plan
 * @param[out] aCodePlan  INSERT VALUES Code Plan    
 * @param[out] aDataPlan  INSERT VALUES Data Plan
 * @param[in]  aEnv          Environment
 */
stlStatus qlxInsertSelectGetValidPlan( smlTransId             aTransID,
                                       qllDBCStmt           * aDBCStmt,
                                       qllStatement         * aSQLStmt,
                                       qllQueryPhase          aQueryPhase,
                                       qlvInitInsertSelect ** aInitPlan,
                                       qllOptimizationNode ** aCodePlan,
                                       qllDataArea         ** aDataPlan,
                                       qllEnv               * aEnv )
{
    qlvInitInsertSelect * sInitPlan = NULL;
    qllOptimizationNode * sCodePlan = NULL;
    qllDataArea         * sDataPlan = NULL;

    stlBool  sIsValid       = STL_FALSE;
    stlBool  sSegmentExist  = STL_FALSE;
    stlBool  sNeedRecompile = STL_FALSE;

    /**
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_INSERT_SELECT_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_INTO_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_QUERY_TYPE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aQueryPhase > QLL_PHASE_VALIDATION) &&
                        (aQueryPhase <= QLL_PHASE_EXECUTE),
                        QLL_ERROR_STACK(aEnv) );
    

    /**
     * 기본 정보 설정
     */
    
    if( (aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_INTO_TYPE) ||
        (aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_QUERY_TYPE) )
    {
        sInitPlan = 
            ((qlvInitInsertReturnInto *) qllGetInitPlan( aSQLStmt ))->mInitInsertSelect;
    }
    else
    {
        sInitPlan = (qlvInitInsertSelect *) qllGetInitPlan( aSQLStmt );
    }
    sCodePlan = (qllOptimizationNode *) aSQLStmt->mCodePlan;
    sDataPlan = (qllDataArea *) aSQLStmt->mDataPlan;


    /**
     * Handle Validation
     */

    STL_TRY( ellIsRecentDictHandle( aTransID,
                                    & sInitPlan->mTableHandle,
                                    & sIsValid,
                                    ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    if ( sIsValid == STL_TRUE )
    {
        if ( sInitPlan->mInitPlan.mValidationObjList == NULL )
        {
            STL_DASSERT( qloGetValidTblStatList( aSQLStmt ) == NULL );
            sIsValid = STL_TRUE;
        }
        else
        {
            STL_TRY( qllIsRecentAccessObject( aTransID,
                                              sInitPlan->mInitPlan.mValidationObjList,
                                              ( aQueryPhase == QLL_PHASE_EXECUTE
                                                ? (void*)qloGetValidTblStatList( aSQLStmt )
                                                : NULL ),
                                              & sIsValid,
                                              aEnv )
                     == STL_SUCCESS );
        }
    }
    else
    {
        sIsValid = STL_FALSE;
    }

    if( sIsValid == STL_TRUE )
    {
        /**
         * Table 의 구조가 변하지 않음
         */

        sNeedRecompile = STL_FALSE;
    }
    else
    {
        /**
         * Table 의 구조가 변함
         */
        
        sNeedRecompile = STL_TRUE;
    }

    /**
     * 유효한 Plan 을 얻을 때까지 Recompile 을 반복
     */
    
    while ( 1 )
    {
        STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );
        
        if ( sNeedRecompile == STL_TRUE )
        {
            if( aSQLStmt->mSqlHandle != NULL )
            {
                /**
                 * Plan을 삭제한다.
                 */
                
                STL_TRY( knlDiscardSql( aSQLStmt->mSqlHandle,
                                        KNL_ENV(aEnv) )
                         == STL_SUCCESS );
                
                STL_TRY( knlUnfixSql( aSQLStmt->mSqlHandle,
                                      KNL_ENV(aEnv) )
                         == STL_SUCCESS );
                
                aSQLStmt->mSqlHandle = NULL;
            }
            
            STL_TRY( qlgRecompileSQL( aTransID,
                                      aDBCStmt,
                                      aSQLStmt,
                                      STL_TRUE,  /* aSearchPlanCache */
                                      aQueryPhase,
                                      aEnv ) == STL_SUCCESS );

            /**
             * 기본 정보를 다시 획득
             */
        
            if( (aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_INTO_TYPE) ||
                (aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_QUERY_TYPE) )
            {
                sInitPlan = 
                    ((qlvInitInsertReturnInto *) qllGetInitPlan( aSQLStmt ))->mInitInsertSelect;
            }
            else
            {
                sInitPlan = (qlvInitInsertSelect *) qllGetInitPlan( aSQLStmt );
            }
            sCodePlan = (qllOptimizationNode *) aSQLStmt->mCodePlan;
            sDataPlan = (qllDataArea *) aSQLStmt->mDataPlan;
        }
        else
        {
            /**
             * while 문이 반복될 경우 Recompile 을 해야 함.
             */
            sNeedRecompile = STL_TRUE;
        }

        /**
         * Query 수행 단계에 따라 Lock 획득
         */
        
        if ( aQueryPhase == QLL_PHASE_EXECUTE )
        {
    
            /**
             * IX LOCK 획득
             * - Statement 가 시작되었으므로 
             * - 구조가 변경되었다 하더라도 Physical Handle 은 Aging 되지 않아 Lock 설정이 가능하다.
             */
    
            STL_TRY( smlLockTable( aTransID,
                                   SML_LOCK_IX,
                                   sInitPlan->mTablePhyHandle,
                                   SML_LOCK_TIMEOUT_INFINITE,  
                                   &sSegmentExist,
                                   SML_ENV(aEnv) )
                     == STL_SUCCESS );

            if ( sSegmentExist == STL_TRUE )
            {
                /* segment 가 유효함 */
            }
            else
            {
                /* segment 가 존재하지 않는 경우 */
                continue;
            }
            
            /**
             * Lock 이후의 Table Handle 의 유효성 검증
             */

            STL_TRY( ellIsRecentDictHandle( aTransID,
                                           & sInitPlan->mTableHandle,
                                           & sIsValid,
                                           ELL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            if ( sIsValid == STL_TRUE )
            {
                if ( sInitPlan->mInitPlan.mValidationObjList == NULL )
                {
                    STL_DASSERT( qloGetValidTblStatList( aSQLStmt ) == NULL );
                    sIsValid = STL_TRUE;
                }
                else
                {
                    STL_TRY( qllIsRecentAccessObject( aTransID,
                                                      sInitPlan->mInitPlan.mValidationObjList,
                                                      ( aQueryPhase == QLL_PHASE_EXECUTE
                                                        ? (void*)qloGetValidTblStatList( aSQLStmt )
                                                        : NULL ),
                                                      & sIsValid,
                                                      aEnv )
                             == STL_SUCCESS );
                }
            }
            else
            {
                sIsValid = STL_FALSE;
            }

            if ( sIsValid == STL_TRUE )
            {
                /**
                 * - Handle 검사와 IX lock 중간에 Table 구조가 변경되었는지 검사
                 * - IX lock 이후에는 Table 구조 및 관련 Object(Index, Constraint 등)이 변경되지 않는다.
                 */
            }
            else
            {
                /**
                 * 다시 Recompile 해야 함
                 */

                continue;
            }
        }
        else
        {
            /**
             * Execute 이전에는 Lock 을 잡지 않음
             */
        }
        
        break;
    }


    /**
     * Output 설정
     */

    if ( aInitPlan != NULL )
    {
        *aInitPlan = sInitPlan;
    }
            
    if ( aCodePlan != NULL )
    {
        *aCodePlan = sCodePlan;
    }

    if ( aDataPlan != NULL )
    {
        *aDataPlan = sDataPlan;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} qlxInsertSelect */
