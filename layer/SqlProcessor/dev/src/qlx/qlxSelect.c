/*******************************************************************************
 * qlxSelect.c
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
 * @file qlxSelect.c
 * @brief SQL Processor Layer SELECT statement execution
 */

#include <qll.h>
#include <qlDef.h>
#include <qlnxNode.h>


/**
 * @addtogroup qlxSelect
 * @{
 */


/**
 * @brief SELECT 구문을 실행한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aStmt      Statement
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement 객체
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlxExecuteSelect( smlTransId      aTransID,
                            smlStatement  * aStmt,
                            qllDBCStmt    * aDBCStmt,
                            qllStatement  * aSQLStmt,
                            qllEnv        * aEnv )
{
    qlvInitSelect           * sInitPlan      = NULL;
    qllOptimizationNode     * sOptNode       = NULL;
    qllDataArea             * sDataArea      = NULL;
    qllResultSetDesc        * sResultSetDesc = NULL;


    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aSQLStmt->mStmtType == QLL_STMT_SELECT_TYPE ) ||
                        ( aSQLStmt->mStmtType == QLL_STMT_SELECT_FOR_UPDATE_TYPE ),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mDataPlan != NULL, QLL_ERROR_STACK(aEnv) );


    /******************************************************
     * 정보 획득 
     ******************************************************/
    
    /**
     * 검증된 Plan 정보 획득 
     */

    STL_TRY( qlxSelectGetValidPlan( aTransID,
                                    aDBCStmt,
                                    aSQLStmt,
                                    QLL_PHASE_EXECUTE,
                                    & sInitPlan, 
                                    & sOptNode,
                                    & sDataArea,
                                    aEnv )
             == STL_SUCCESS );

    /**
     * transaction id 설정
     */
    
    sDataArea->mTransID = aTransID;    

    /******************************************************
     * Initialize & Execute Execution Nodes
     ******************************************************/
    
    if( sDataArea->mIsFirstExecution == STL_TRUE )
    {
        /* Execution Node 초기화 */
        QLNX_INITIALIZE_NODE( aTransID,
                              aStmt,
                              aDBCStmt,
                              aSQLStmt,
                              sOptNode,
                              sDataArea,
                              aEnv );

        /* Execution Node 수행 */
        QLNX_EXECUTE_NODE( aTransID,
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


    /*****************************************************
     * Result Set 정보 설정 
     *****************************************************/

    sResultSetDesc = aSQLStmt->mResultSetDesc;

    /**
     * Result Set 시작을 설정
     */
    
    sResultSetDesc->mIsOpen = STL_TRUE;
    sResultSetDesc->mStmtTimestamp = smlGetStatementTimestamp( aStmt );

    /**
     * Result Set 의 Target 정보 설정
     */

    if ( sResultSetDesc->mMaterialized == STL_TRUE )
    {
        /**
         * Materialized Result Set 을 구축함.
         */

        if ( sResultSetDesc->mCursorProperty.mUpdatability == ELL_CURSOR_UPDATABILITY_FOR_UPDATE )
        {
            /**
             * UPDATABLE cursor 인 경우 Execution 시점에 모두 구축 
             */

            STL_TRY( qlxBuildMaterialResultSet4ForUpdate( aTransID,
                                                          aStmt,
                                                          aDBCStmt,
                                                          aSQLStmt,
                                                          sInitPlan->mSerialAction,
                                                          sResultSetDesc,
                                                          aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /**
             * SCROLL & READ ONLY cursor 인 경우
             * - Execution 시점에 준비하고 Fetch 시점에 구축하게 됨.
             */

            STL_TRY( qlxBuildMaterialResultSet4ForReadOnly( aTransID,
                                                            aStmt,
                                                            aDBCStmt,
                                                            aSQLStmt,
                                                            sResultSetDesc,
                                                            aEnv )
                     == STL_SUCCESS );
        }
            
    }
    else
    {
        /**
         * nothing to do
         */
    }
       

    return STL_SUCCESS;

    STL_FINISH;

    /* Execution Node 종료 */
    QLNX_FINALIZE_NODE_IGNORE_FAILURE( sOptNode,
                                       sDataArea,
                                       aEnv );

    return STL_FAILURE;
}


/**
 * @brief SELECT 수행을 위한 Lock 을 잡고 유효한 Plan 을 얻는다.
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aDBCStmt      DBC Statement
 * @param[in]  aSQLStmt      SQL Statement
 * @param[in]  aQueryPhase   Query Processing Phase
 * @param[out] aInitPlan     SELECT Init Plan
 * @param[out] aCodePlan     SELECT Code Plan    
 * @param[out] aDataPlan     SELECT Data Plan
 * @param[in]  aEnv          Environment
 */
stlStatus qlxSelectGetValidPlan( smlTransId              aTransID,
                                 qllDBCStmt            * aDBCStmt,
                                 qllStatement          * aSQLStmt,
                                 qllQueryPhase           aQueryPhase,
                                 qlvInitSelect        ** aInitPlan,
                                 qllOptimizationNode  ** aCodePlan,
                                 qllDataArea          ** aDataPlan,
                                 qllEnv                * aEnv )
{
    qlvInitSelect        * sInitPlan = NULL;
    qllOptimizationNode  * sCodePlan = NULL;
    qllDataArea          * sDataPlan = NULL;
    
    stlBool  sIsValid       = STL_FALSE;
    stlBool  sSegmentExist  = STL_FALSE;
    stlBool  sNeedRecompile = STL_FALSE;

    qlvRefTableItem      * sLockTableItem = NULL;
    qlvInitBaseTableNode * sBaseTableNode = NULL;
    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_SELECT_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_SELECT_FOR_UPDATE_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_SELECT_INTO_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_SELECT_INTO_FOR_UPDATE_TYPE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aQueryPhase > QLL_PHASE_VALIDATION) &&
                        (aQueryPhase <= QLL_PHASE_EXECUTE),
                        QLL_ERROR_STACK(aEnv) );


    /**
     * 기본 정보 설정
     */
    
    sInitPlan = (qlvInitSelect *) qllGetInitPlan( aSQLStmt );
    sCodePlan = (qllOptimizationNode *) aSQLStmt->mCodePlan;
    sDataPlan = (qllDataArea *) aSQLStmt->mDataPlan;

    /**
     * Handle Validation
     */

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
            
            sInitPlan = (qlvInitSelect *) qllGetInitPlan( aSQLStmt );
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
         * SELECT FOR UPDATE 일 경우 Lock Item 에 대해 IX LOCK 획득
         */
        
        if ( (sInitPlan->mLockTableList != NULL) &&
             (aQueryPhase == QLL_PHASE_EXECUTE) )
        {
            /**
             * Lock 대상 Table 들에 IX LOCK 획득
             */
            
            for ( sLockTableItem = sInitPlan->mLockTableList->mHead;
                  sLockTableItem != NULL;
                  sLockTableItem = sLockTableItem->mNext )
            {
                sBaseTableNode = (qlvInitBaseTableNode *) sLockTableItem->mTableNode;
                
                STL_TRY( smlLockTable( aTransID,
                                       SML_LOCK_IX,
                                       sBaseTableNode->mTablePhyHandle,
                                       SML_LOCK_TIMEOUT_INFINITE,  
                                       & sSegmentExist,
                                       SML_ENV(aEnv) )
                         == STL_SUCCESS );

                if ( sSegmentExist == STL_TRUE )
                {
                    /* segment 가 유효함 */
                }
                else
                {
                    /* segment 가 존재하지 않는 경우 */
                    break;;
                }
            }

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
             * Lock 획득 후 Handle Validation
             */
            
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
            
            if( sIsValid == STL_TRUE )
            {
                /**
                 * Table 의 구조가 변하지 않음
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
             * READ ONLY Select 임
             * - nothing to do 
             */
        }

        break;
    }
    
    /**
     * Output 설정
     */

    if( aInitPlan != NULL )
    {
        *aInitPlan = sInitPlan;
    }
            
    if( aCodePlan != NULL )
    {
        *aCodePlan = sCodePlan;
    }

    if( aDataPlan != NULL )
    {
        *aDataPlan = sDataPlan;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief FOR UPDATE 를 위한 Materialized Result Set 을 구축한다.
 * @param[in]  aTransID       Transaction ID
 * @param[in]  aStmt          Statement
 * @param[in]  aDBCStmt       DBC Statement
 * @param[in]  aSQLStmt       SQL Statement
 * @param[in]  aSerialAction  Version Conflict Resolution 을 위한 Serializability Action
 * @param[in]  aResultSetDesc Result Set Desc
 * @param[in]  aEnv           Environment
 * @remarks
 */
stlStatus qlxBuildMaterialResultSet4ForUpdate( smlTransId                aTransID,
                                               smlStatement            * aStmt,
                                               qllDBCStmt              * aDBCStmt,
                                               qllStatement            * aSQLStmt,
                                               qlvSerialAction           aSerialAction,
                                               qllResultSetDesc        * aResultSetDesc,
                                               qllEnv                  * aEnv )
{
    qllOptimizationNode  * sOptNode  = NULL;
    qllDataArea          * sDataArea = NULL;

    qllExecutionNode     * sExecNode = NULL;
    qlnxSelectStmt       * sExeSelectStmt = NULL;
    
    qlnInstantTable      * sInstantTable = NULL;
    smlFetchInfo         * sFetchInfo = NULL;
    
    stlBool                sEOF = STL_FALSE;
    stlInt64               sReadCnt = 0;
    
    stlInt64               sBlockIdx = 0;
    stlInt64               sSkipCnt = 0;
    stlInt64               sUsedCnt = 0;
    
    qllCursorLockItem    * sLockItem = NULL;

    stlBool                sConflictExist = STL_FALSE;
    stlBool                sVersionConflict = STL_FALSE;

    stlBool                sIsMatched  = STL_FALSE;
    stlBool                sRowDeleted = STL_FALSE;

    knlExprEvalInfo        sTargetEvalInfo;
    knlValueBlockList    * sTargetBlock = NULL;
    stlInt32               sTargetIdx = 0;

    stlInt64               sOrgSkipCnt = 0;
    stlInt64               sOrgFetchCnt = 0;

    stlInt64               sRemainSkipCnt = 0;
    stlInt64               sRemainFetchCnt = 0;

    qlnxFetchNodeInfo      sFetchNodeInfo;
    
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aSerialAction == QLV_SERIAL_NO_RETRY) ||
                        (aSerialAction == QLV_SERIAL_RETRY_ONLY),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aResultSetDesc != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aResultSetDesc->mMaterialized == STL_TRUE, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aResultSetDesc->mStoreBlockList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aResultSetDesc->mInstantTable != NULL, QLL_ERROR_STACK(aEnv) );
    
    /**************************************************
     * Instant Table 생성 
     **************************************************/

    sInstantTable = (qlnInstantTable *) aResultSetDesc->mInstantTable;

    stlMemset( sInstantTable, 0x00, STL_SIZEOF( qlnInstantTable ) );

    STL_TRY( qlnxCreateFlatInstantTable( aTransID,
                                         aStmt,
                                         sInstantTable,
                                         STL_TRUE, /* aIsScrollableInstant */
                                         aEnv )
             == STL_SUCCESS );

    sFetchInfo = & aResultSetDesc->mInstantFetchInfo;

    
    /*****************************************
     * EOF 만날때까지 Fetch 수행하면서 Instant Table에 레코드 삽입
     ****************************************/

    /**
     * Plan 정보 획득
     */
    
    sOptNode  = (qllOptimizationNode *) aSQLStmt->mCodePlan;
    sDataArea = (qllDataArea *) aSQLStmt->mDataPlan;

    /**
     * 최상위 노드 (SELECT Statement) 의
     * OFFSET FETCH 정보와 통제권을 가져온다.
     */

    sExecNode = QLL_GET_EXECUTION_NODE( sDataArea, QLL_GET_OPT_NODE_IDX( sOptNode ) );
    sExeSelectStmt = sExecNode->mExecNode;

    sOrgSkipCnt  = sExeSelectStmt->mRemainSkipCnt;
    sOrgFetchCnt = sExeSelectStmt->mRemainFetchCnt;

    /**
     * Fetch 시작
     */
    
    STL_RAMP( VERSION_CONFLICT_RESOLUTION_RETRY );
        
    sEOF = STL_FALSE;

    sExeSelectStmt->mRemainSkipCnt  = 0;
    sExeSelectStmt->mRemainFetchCnt = QLL_FETCH_COUNT_MAX;
    
    sRemainSkipCnt  = sOrgSkipCnt;
    sRemainFetchCnt = sOrgFetchCnt;

    QLNX_INIT_FETCH_NODE_INFO( & sFetchNodeInfo,
                               aTransID,
                               sInstantTable->mStmt,
                               aDBCStmt,
                               aSQLStmt,
                               sDataArea );

    while ( sEOF == STL_FALSE )
    {
        /**
         * Fetch 수행
         */

        if ( sRemainFetchCnt <= 0 )
        {
            break;
        }
        
        sReadCnt = 0;
        QLNX_FETCH_NODE( & sFetchNodeInfo,
                         QLL_GET_OPT_NODE_IDX( sOptNode ),
                         0, /* Start Idx */
                         sRemainSkipCnt, 
                         sRemainFetchCnt,
                         & sReadCnt,
                         & sEOF,
                         aEnv );

        sRemainSkipCnt = 0;
        
        /**
         * Fetch 종료 여부
         * - Fetch API 가 아닌 NODE Fetch 를 수행할 경우
         * - EOF 여부로 검사하지 않고 ReadCnt 로 검사한다.
         */
        
        if ( sReadCnt == 0 )
        {
            break;
        }
        
        /**
         * Store Block 정보 설정
         */

        STL_DASSERT( aResultSetDesc->mStoreBlockList != NULL );
        
        sSkipCnt = KNL_GET_BLOCK_SKIP_CNT( aResultSetDesc->mTargetBlock );
        sUsedCnt = KNL_GET_BLOCK_USED_CNT( aResultSetDesc->mTargetBlock );

        /**********************************************************
         * 각 Row 에 대해 Lock 을 획득함.
         **********************************************************/

        STL_DASSERT( aResultSetDesc->mLockItemList != NULL );

        if ( aSerialAction == QLV_SERIAL_NO_RETRY )
        {
            /**********************************************************
             * NO RETRY 인 경우 (WHERE 절이 없는 경우)
             **********************************************************/

            /**
             * Fetch Record Information 초기화
             */
            
            for ( sBlockIdx = sSkipCnt; sBlockIdx < sUsedCnt; sBlockIdx++ )
            {
                /**
                 * JOIN 등이 올 수 없음
                 */
                
                sLockItem = aResultSetDesc->mLockItemList;
                STL_DASSERT( sLockItem->mNext == NULL );
                
                /**
                 * 다음 상황이 될때까지 Lock 획득을 시도 
                 * - Lock 을 잡거나,
                 * - Lock 을 못잡거나 (error)
                 * - 삭제된 Row 일때
                 */
                
                sConflictExist = STL_FALSE;
                sVersionConflict = STL_TRUE;
                
                while( sVersionConflict == STL_TRUE )
                {
                    /**
                     * Lock 획득을 시도
                     */
                    
                    sVersionConflict = STL_FALSE;
                    STL_TRY( smlLockRecordForUpdate(
                                 sInstantTable->mStmt,
                                 sLockItem->mTablePhyHandle,
                                 sLockItem->mLockRowBlock->mRidBlock[sBlockIdx],
                                 sLockItem->mLockRowBlock->mScnBlock[sBlockIdx],
                                 aResultSetDesc->mLockTimeInterval,
                                 & sVersionConflict,
                                 SML_ENV(aEnv) )
                             == STL_SUCCESS );
                    
                    if ( sVersionConflict == STL_TRUE )
                    {
                        /**
                         * Version Conflict 가 발생함.
                         */

                        sConflictExist = STL_TRUE;
                        
                        /**
                         * RID 에 해당하는 Row 를 새로 읽음 
                         */
                        
                        STL_TRY( smlFetchRecord(
                                     sInstantTable->mStmt,
                                     sLockItem->mFetchRecordInfo,
                                     & sLockItem->mLockRowBlock->mRidBlock[sBlockIdx],
                                     sBlockIdx,
                                     & sIsMatched,
                                     & sRowDeleted,
                                     NULL, /* aUpdated */
                                     SML_ENV(aEnv) )
                                 == STL_SUCCESS );

                        if ( sRowDeleted == STL_TRUE )
                        {
                            break;
                        }
                    }
                    else
                    {
                        /**
                         * 해당 Row 에 대해 Lock 을 획득함.
                         */
                            
                        sRowDeleted = STL_FALSE;
                    }

                } /* while loop of lock row */

                if ( sRowDeleted == STL_TRUE )
                {
                    aResultSetDesc->mRowLocked[sBlockIdx] = STL_FALSE;
                }
                else
                {
                    if ( sConflictExist == STL_TRUE )
                    {
                        /**
                         * Conflict 가 발생한 적이 있다면, Target 을 재수행한다.
                         */

                        for ( sTargetBlock = aResultSetDesc->mTargetBlock,
                                  sTargetIdx = 0;
                              sTargetBlock != NULL;
                              sTargetBlock = sTargetBlock->mNext,
                                  sTargetIdx++ )
                        {
                            /**
                             * Evaluation Information 구성
                             */

                            if( aResultSetDesc->mTargetCodeStack[sTargetIdx].mEntryCount > 0 )
                            {
                                sTargetEvalInfo.mExprStack =
                                    & aResultSetDesc->mTargetCodeStack[sTargetIdx];
                                sTargetEvalInfo.mContext     = aResultSetDesc->mTargetDataContext;
                                sTargetEvalInfo.mResultBlock = sTargetBlock;
                                sTargetEvalInfo.mBlockIdx    = sBlockIdx;
                                sTargetEvalInfo.mBlockCount  = 1;
                            
                                /**
                                 * Targeting Expression 수행
                                 */
                            
                                STL_TRY( knlEvaluateOneExpression( & sTargetEvalInfo,
                                                                   KNL_ENV(aEnv) )
                                         == STL_SUCCESS );
                            }
                        }
                    }
                    else
                    {
                        /**
                         * nothing to do
                         */
                    }

                    sRemainFetchCnt--;
                    aResultSetDesc->mRowLocked[sBlockIdx] = STL_TRUE;
                }
                    
            } /* for loop of block read */
        }
        else
        {
            /**********************************************************
             * RETRY_ONLY 인 경우 (WHERE 절 등이 포함된 일반적인 SELECT FOR UPDATE 경우)
             **********************************************************/

            for ( sBlockIdx = sSkipCnt; sBlockIdx < sUsedCnt; sBlockIdx++ )
            {
                for ( sLockItem = aResultSetDesc->mLockItemList;
                      sLockItem != NULL;
                      sLockItem = sLockItem->mNext )
                {
                    sVersionConflict = STL_FALSE;
                    STL_TRY( smlLockRecordForUpdate(
                                 sInstantTable->mStmt,
                                 sLockItem->mTablePhyHandle,
                                 sLockItem->mLockRowBlock->mRidBlock[sBlockIdx],
                                 sLockItem->mLockRowBlock->mScnBlock[sBlockIdx],
                                 aResultSetDesc->mLockTimeInterval,
                                 & sVersionConflict,
                                 SML_ENV(aEnv) )
                             == STL_SUCCESS );

                    if ( sVersionConflict == STL_TRUE )
                    {
                        /**
                         * Instant Table 을 비운다.
                         */

                        STL_TRY( qlnxTruncateInstantTable( aStmt,
                                                           sInstantTable,
                                                           aEnv )
                                 == STL_SUCCESS );
                        
                        /**
                         * Query 를 종료한다.
                         */
                        
                        QLNX_FINALIZE_NODE( sOptNode,
                                            sDataArea,
                                            aEnv );
                        
                        /**
                         * SM Statement RESET
                         */

                        STL_TRY( smlResetStatement( sInstantTable->mStmt,
                                                    STL_FALSE, /* aDoRollback */
                                                    SML_ENV(aEnv) )
                                 == STL_SUCCESS );
                        
                        /**
                         * Query 를 재수행하여 Before Fetch 상태로 돌린다.
                         */
                        
                        /* Execution Node 초기화 */
                        QLNX_INITIALIZE_NODE( aTransID,
                                              aStmt,
                                              aDBCStmt,
                                              aSQLStmt,
                                              sOptNode,
                                              sDataArea,
                                              aEnv );
                        
                        /* Execution Node 수행 */
                        QLNX_EXECUTE_NODE( aTransID,
                                           aStmt,
                                           aDBCStmt,
                                           aSQLStmt,
                                           sOptNode,
                                           sDataArea,
                                           aEnv );
                        
                        /**
                         * Query 를 재수행한다.
                         */
                        
                        STL_THROW( VERSION_CONFLICT_RESOLUTION_RETRY );

                    }
                    else
                    {
                        sRemainFetchCnt--;
                        aResultSetDesc->mRowLocked[sBlockIdx] = STL_TRUE;
                    }
                }
            }
        }

        /********************************************************
         * Locked Row 구간만 저장
         ********************************************************/

        STL_TRY( qlnxAddMeetBlockIntoInstantTable( sInstantTable,
                                                   sFetchInfo->mRowBlock,
                                                   aResultSetDesc->mStoreBlockList,
                                                   aResultSetDesc->mRowLocked,
                                                   sSkipCnt,
                                                   sUsedCnt,
                                                   aEnv )
                 == STL_SUCCESS );

        if ( sEOF == STL_TRUE )
        {
            break;
        }
    }

    /*****************************************
     * Instant Table 에 대한 Iterator 할당
     ****************************************/

    /**
     * Finalize Nodes
     */
    
    QLNX_FINALIZE_NODE( sOptNode,
                        sDataArea,
                        aEnv );
    
    /**
     * Instant Table 에 대한 Iterator 할당
     */

    STL_TRY( smlAllocIterator( sInstantTable->mStmt,
                               sInstantTable->mTableHandle,
                               SML_TRM_SNAPSHOT,
                               SML_SRM_SNAPSHOT,
                               & sInstantTable->mIteratorProperty,
                               SML_SCAN_FORWARD,
                               & sInstantTable->mIterator,
                               SML_ENV( aEnv ) )
             == STL_SUCCESS );

    
    /**
     * Instant Table 에 대한 Iterator Fetch 정보 초기화
     */

    sFetchInfo->mSkipCnt     = 0;
    sFetchInfo->mFetchCnt    = QLL_FETCH_COUNT_MAX;
    sFetchInfo->mIsEndOfScan = STL_FALSE;


    /********************************************
     * Result Set Fetch 정보 설정
     ********************************************/

    /**
     * Result Set 시작을 설정
     */
    
    aResultSetDesc->mIsOpen = STL_TRUE;
    
    /**
     * Result Set 이 모두 Materialzed 됨
     */

    aResultSetDesc->mKnownLastPosition = STL_TRUE;
    aResultSetDesc->mLastPositionIdx = sInstantTable->mRecCnt;

    /**
     * Result Set 에 대한 Fetch 가 수행되기 전임 
     */
    
    aResultSetDesc->mBlockStartIdx = -1;
    aResultSetDesc->mBlockEOFIdx   = 0;
        
    aResultSetDesc->mBlockStartPositionIdx = 0;
    aResultSetDesc->mBlockEOFPositionIdx   = 1;
    
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
 * @brief Read Only 를 위한 Materialized Result Set 을 구축한다.
 * @param[in]  aTransID       Transaction ID
 * @param[in]  aStmt          Statement
 * @param[in]  aDBCStmt       DBC Statement
 * @param[in]  aSQLStmt       SQL Statement
 * @param[in]  aResultSetDesc Result Set Descriptor 
 * @param[in]  aEnv           Environment
 * @remarks
 */
stlStatus qlxBuildMaterialResultSet4ForReadOnly( smlTransId                aTransID,
                                                 smlStatement            * aStmt,
                                                 qllDBCStmt              * aDBCStmt,
                                                 qllStatement            * aSQLStmt,
                                                 qllResultSetDesc        * aResultSetDesc,
                                                 qllEnv                  * aEnv )
{
    qlnInstantTable        * sInstantTable = NULL;
    smlFetchInfo           * sFetchInfo = NULL;

    stlBool             sIsScrollableInstant = STL_FALSE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aResultSetDesc != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aResultSetDesc->mMaterialized == STL_TRUE, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aResultSetDesc->mStoreBlockList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aResultSetDesc->mInstantTable != NULL, QLL_ERROR_STACK(aEnv) );

    /**************************************************
     * Instant Table 생성 
     **************************************************/

    sInstantTable = aResultSetDesc->mInstantTable;

    if ( aResultSetDesc->mCursorProperty.mScrollability == ELL_CURSOR_SCROLLABILITY_SCROLL )
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
    

    /**************************************************
     * Fetch 를 위한 Iterator 할당 
     **************************************************/
    
    /**
     * Instant Table 에 대한 Iterator 할당
     */

    STL_TRY( smlAllocIterator( sInstantTable->mStmt,
                               sInstantTable->mTableHandle,
                               SML_TRM_SNAPSHOT,
                               SML_SRM_SNAPSHOT,
                               & sInstantTable->mIteratorProperty,
                               SML_SCAN_FORWARD,
                               & sInstantTable->mIterator,
                               SML_ENV( aEnv ) )
             == STL_SUCCESS );
    

    /**
     * Instant Table 에 대한 Iterator Fetch 정보 초기화
     */

    sFetchInfo = & aResultSetDesc->mInstantFetchInfo;

    sFetchInfo->mSkipCnt     = 0;
    sFetchInfo->mFetchCnt    = QLL_FETCH_COUNT_MAX;
    sFetchInfo->mIsEndOfScan = STL_FALSE;


    /********************************************
     * Result Set Fetch 정보 설정
     ********************************************/

    /**
     * Result Set 시작을 설정
     */
    
    aResultSetDesc->mIsOpen = STL_TRUE;
    
    /**
     * Result Set 에 저장된 Row 가 없음 
     */

    aResultSetDesc->mKnownLastPosition = STL_FALSE;
    aResultSetDesc->mLastPositionIdx = 0;

    /**
     * Result Set 에 대한 Fetch 가 수행되기 전임 
     */
    
    aResultSetDesc->mBlockStartIdx = -1;
    aResultSetDesc->mBlockEOFIdx   = 0;
        
    aResultSetDesc->mBlockStartPositionIdx = 0;
    aResultSetDesc->mBlockEOFPositionIdx   = 1;
    
    return STL_SUCCESS;

    STL_FINISH;

    if ( sInstantTable != NULL )
    {
        (void) qlnxDropInstantTable( sInstantTable,
                                     aEnv );
    }
    
    return STL_FAILURE;
    
}    



/** @} qlxSelect */


