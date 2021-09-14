/*******************************************************************************
 * qlnxUpdateStmt.c
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
 * @file qlnxUpdateStmt.c
 * @brief SQL Processor Layer Execution - UPDATE STATEMENT Node
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnx
 * @{
 */


/**
 * @brief UPDATE STATEMENT node를 초기화한다.
 * @param[in]      aTransID      Transaction ID
 * @param[in]      aStmt         Statement
 * @param[in]      aDBCStmt      DBC Statement
 * @param[in,out]  aSQLStmt      SQL Statement
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 *
 * @remark 첫번째 Execution에서 수행한다.
 *    <BR> Execution을 위한 정보를 보강한다.
 */
stlStatus qlnxInitializeUpdateStmt( smlTransId              aTransID,
                                    smlStatement          * aStmt,
                                    qllDBCStmt            * aDBCStmt,
                                    qllStatement          * aSQLStmt,
                                    qllOptimizationNode   * aOptNode,
                                    qllDataArea           * aDataArea,
                                    qllEnv                * aEnv )
{
    /* qllExecutionNode  * sExecNode = NULL; */
    qlnoUpdateStmt    * sOptUpdateStmt = NULL;
    /* qlnxUpdateStmt    * sExeUpdateStmt = NULL; */
    
    qllOptNodeItem    * sOptNodeItem = NULL;
    stlInt16            sNodeCnt = 0;

    
    /**
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_STMT_UPDATE_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * UPDATE STATEMENT Optimization Node 획득
     */

    sOptUpdateStmt = (qlnoUpdateStmt *) aOptNode->mOptNode;


    /**
     * Common Execution Node 획득
     */
        
    /* sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, */
    /*                                     QLL_GET_OPT_NODE_IDX( aOptNode ) ); */


    /**
     * UPDATE STATEMENT Execution Node 할당
     */

    /* sExeUpdateStmt = (qlnxUpdateStmt *) sExecNode->mExecNode; */


    /******************************************************
     * 하위 Node들 Initialize 수행
     ******************************************************/

    sOptNodeItem = sOptUpdateStmt->mOptNodeList->mTail;
    
    for( sNodeCnt = 1 ; /* Statement 제외한 노드들에 대해 Execute 수행 */
         sNodeCnt < aDataArea->mExecNodeCnt ;
         sNodeCnt++ )
    {
        QLNX_INITIALIZE_NODE( aTransID,
                              aStmt,
                              aDBCStmt,
                              aSQLStmt,
                              sOptNodeItem->mNode,
                              aDataArea,
                              aEnv );
        sOptNodeItem = sOptNodeItem->mPrev;
    }
    
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief UPDATE STATEMENT node를 수행한다.
 * @param[in]      aTransID      Transaction ID
 * @param[in]      aStmt         Statement
 * @param[in]      aDBCStmt      DBC Statement
 * @param[in,out]  aSQLStmt      SQL Statement
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 *
 * @remark Execution 첫번째 수행시 호출한다.
 */
stlStatus qlnxExecuteUpdateStmt( smlTransId              aTransID,
                                 smlStatement          * aStmt,
                                 qllDBCStmt            * aDBCStmt,
                                 qllStatement          * aSQLStmt,
                                 qllOptimizationNode   * aOptNode,
                                 qllDataArea           * aDataArea,
                                 qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode = NULL;
    qlnoUpdateStmt        * sOptUpdateStmt = NULL;
    qlnxUpdateStmt        * sExeUpdateStmt = NULL;
    
    stlInt64                sSkipCnt = 0;
    stlInt64                sFetchCnt = 0;
    stlInt64                sReadCnt = 0;
    stlBool                 sEOF = STL_FALSE;

    qlnInstantTable       * sInstantTable = NULL;
    
    /* 
     * for positioned UPDATE
     */
    
    qlvInitUpdate         * sInitPlan  = NULL; 

    ellCursorInstDesc     * sCursorInstDesc = NULL;
    qllStatement          * sCursorSQLStmt = NULL;
    qllResultSetDesc      * sResultSetDesc = NULL;
    
    qllCursorLockItem     * sLockTableItem = NULL;
    
    smlFetchRecordInfo    * sFetchRecordInfo = NULL;

    stlBool                 sDeleted   = STL_FALSE; 
    stlBool                 sIsMatched = STL_FALSE;

    qlnxFetchNodeInfo       sFetchNodeInfo;

    QLL_OPT_DECLARE( stlTime sBeginFetchTime );
    QLL_OPT_DECLARE( stlTime sEndFetchTime );


    /**
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_STMT_UPDATE_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );

    
    QLL_OPT_CHECK_TIME( sBeginFetchTime );

    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * UPDATE STATEMENT Optimization Node 획득
     */

    sInitPlan      = (qlvInitUpdate *) qllGetInitPlan( aSQLStmt );
    sOptUpdateStmt = (qlnoUpdateStmt *) aOptNode->mOptNode;

    if ( aSQLStmt->mResultSetDesc != NULL )
    {
        sInstantTable = (qlnInstantTable *) aSQLStmt->mResultSetDesc->mInstantTable;
    }
    else
    {
        sInstantTable = NULL;
    }
    
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * UPDATE STATEMENT Execution Node 획득
     */

    sExeUpdateStmt = sExecNode->mExecNode;

    
    /**
     * 기본 정보 설정
     */

    aDataArea->mWriteRowCnt = 0;

    /**
     * Constraint Check Time 획득,
     */
    
    STL_TRY( qlxUpdateGetConstraintCheckTime( & sInitPlan->mRelObject,
                                              aOptNode,
                                              aDataArea,
                                              aEnv )
             == STL_SUCCESS );
    
    
    /**
     * Fetch Node Info 설정
     */
    
    QLNX_INIT_FETCH_NODE_INFO( & sFetchNodeInfo,
                               aTransID,
                               aStmt,
                               aDBCStmt,
                               aSQLStmt,
                               aDataArea );


    /******************************************************
     * Statement 단위 Pseudo Column 평가
     ******************************************************/
    
    /* /\** */
    /*  * Pseudo Column 수행 */
    /*  *\/ */

    /* if( sOptUpdateStmt->mStmtExprList->mInitExprList->mPseudoColExprList->mCount > 0 ) */
    /* { */
    /*     /\* STATEMENT 단위 Pseudo Column 수행 *\/ */
    /*     STL_TRY( qlxExecutePseudoColumnBlock( */
    /*                  aTransID, */
    /*                  aStmt, */
    /*                  DTL_ITERATION_TIME_FOR_EACH_STATEMENT, */
    /*                  sOptUpdateStmt->mTableHandle, */
    /*                  sExeUpdateStmt->mRowBlock, */
    /*                  sOptUpdateStmt->mStmtExprList->mInitExprList->mPseudoColExprList, */
    /*                  sExeUpdateStmt->mTotalPseudoColBlock, */
    /*                  0, */
    /*                  aDataArea->mBlockAllocCnt, */
    /*                  aEnv ) */
    /*              == STL_SUCCESS ); */
    /* } */
    /* else */
    /* { */
    /*     /\* nothing to do *\/ */
    /* } */


    /******************************************************
     * CONSTANT EXPRESSION STACK 평가 
     ******************************************************/

    /**
     * FOR_EACH_STMT인 iteration time을 가지는 Pseudo Column 수행 후 &
     * statement 내의 어떠한 expression stack을 평가하기 전에 수행
     */

    STL_DASSERT( aDataArea->mTransID == aTransID );

    STL_TRY( qlxEvaluateConstExpr( sOptUpdateStmt->mStmtExprList->mConstExprStack,
                                   aDataArea,
                                   KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    sEOF = STL_FALSE;
    sReadCnt = 0;


    /******************************************************
     * UPDATE 시작 
     ******************************************************/

    STL_RAMP( VERSION_CONFLICT_RESOLUTION_RETRY );

    if ( sExeUpdateStmt->mVersionConflict == STL_TRUE )
    {
        STL_ASSERT( (sInitPlan->mSerialAction == QLV_SERIAL_RETRY_ONLY) ||
                    (sInitPlan->mSerialAction == QLV_SERIAL_ROLLBACK_RETRY) );
        
        /**
         * Instant Table 을 비운다.
         */
        
        if ( (sInstantTable != NULL) && (sInitPlan->mSerialAction == QLV_SERIAL_ROLLBACK_RETRY) )
        {
            /**********************************************************
             * UPDATE RETURNING QUERY 이고,
             * ROLLBACK & RETRY 인 경우 (Subquery 가 있는 경우)
             **********************************************************/

            STL_TRY( qlnxTruncateInstantTable( aStmt,
                                               sInstantTable,
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
         * Query 를 종료한다.
         * (context를 old context로 원복한 후 finalize를 호출한다.)
         */

        QLNX_FINALIZE_NODE( sExecNode->mOptNode,
                            aDataArea,
                            aEnv );
        
        /**
         * SM Statement RESET
         */
        
        if ( sInitPlan->mSerialAction == QLV_SERIAL_RETRY_ONLY )
        {
            /**
             * RETRY 인 경우 (WHERE 절이 있는 경우)
             */
            
            STL_TRY( smlResetStatement( aStmt,
                                        STL_FALSE, /* aDoRollback */
                                        SML_ENV(aEnv) )
                     == STL_SUCCESS );
            
            /**
             * 갱신한 Row가  Rollback 되지 않는다.
             */
            
            /* aDataArea->mWriteRowCnt = 0; */
        }
        else
        {
            /**
             * ROLLBACK & RETRY 인 경우 (Subquery 가 있는 경우)
             */
            
            STL_TRY( smlResetStatement( aStmt,
                                        STL_TRUE, /* aDoRollback */
                                        SML_ENV(aEnv) )
                     == STL_SUCCESS );
            
            /**
             * 갱신한 Row가  Rollback 됨.
             */
            
            aDataArea->mWriteRowCnt = 0;

            STL_TRY( qlxUpdateResetConstraintCollision( & sInitPlan->mRelObject,
                                                        aOptNode,
                                                        aDataArea,
                                                        aEnv )
                     == STL_SUCCESS );
        }
        
        /**
         * Query 를 재수행하여 Execution 이전 상태로 돌린다.
         */
        
        /* Execution Node 초기화 */
        QLNX_INITIALIZE_NODE( aTransID,
                              aStmt,
                              aDBCStmt,
                              aSQLStmt,
                              sExecNode->mOptNode,
                              aDataArea,
                              aEnv );
    }
    else
    {
        /**
         * nothing to do
         */
    }
        
        
    sExeUpdateStmt->mVersionConflict = STL_FALSE;
    sEOF = STL_FALSE;
    
    /******************************************************
     * 하위 Node Execution
     ******************************************************/

    if ( aSQLStmt->mStmtType == QLL_STMT_UPDATE_WHERE_CURRENT_OF_TYPE )
    {
        /** 
         * positioned UPDATE 인 경우 
         * - RID 를 이용한 Fetch 를 수행하므로 하위 Node 를 수행하지 않는다.
         */ 
    }
    else
    {
        /******************************************************
         * 하위 Node들 Execute 수행
         ******************************************************/
        
        QLNX_EXECUTE_NODE( aTransID,
                           aStmt,
                           aDBCStmt,
                           aSQLStmt,
                           aDataArea->mExecNodeList[ sOptUpdateStmt->mScanNode->mIdx ].mOptNode,
                           aDataArea,
                           aEnv );
    }


    /******************************************************
     * UPDATE 수행 
     ******************************************************/
    
    /**
     * Skip Count 설정
     */

    if( sOptUpdateStmt->mResultSkip == NULL )
    {
        sSkipCnt = sOptUpdateStmt->mSkipCnt;
    }
    else if( sOptUpdateStmt->mResultSkip->mType == QLV_EXPR_TYPE_VALUE )
    {
        sSkipCnt = sOptUpdateStmt->mSkipCnt;
    }
    else
    {
        STL_TRY( qlnxGetBindedValue4FETCH( aDBCStmt,
                                           aSQLStmt,
                                           STL_FALSE, /* aIsFetch */
                                           sOptUpdateStmt->mResultSkip,
                                           & sSkipCnt,
                                           aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Fetch Count 설정
     */
    
    if( sOptUpdateStmt->mResultLimit == NULL )
    {
        sFetchCnt = sOptUpdateStmt->mFetchCnt;
    }
    else if( sOptUpdateStmt->mResultLimit->mType == QLV_EXPR_TYPE_VALUE )
    {
        sFetchCnt = sOptUpdateStmt->mFetchCnt;
    }
    else
    {
        STL_TRY( qlnxGetBindedValue4FETCH( aDBCStmt,
                                           aSQLStmt,
                                           STL_TRUE, /* aIsFetch */
                                           sOptUpdateStmt->mResultLimit,
                                           & sFetchCnt,
                                           aEnv )
                 == STL_SUCCESS );
    }


    /**
     * 조건에 맞는 데이터가 없을 때까지 반복 
     */

    sEOF = STL_FALSE;
    while( ( sFetchCnt > 0 ) && ( sEOF == STL_FALSE ) )
    {
        /**
         * Fetch 수행
         */

        if ( aSQLStmt->mStmtType == QLL_STMT_UPDATE_WHERE_CURRENT_OF_TYPE )
        {
            /**
             * positioned UPDATE 인 경우
             */

            /*****************************************************************
             * Run-Time Validation : Cursor Name
             * - PSM, Embedded SQL 과 달리 ODBC 는 Cursor 를 재선언할 수 있다.
             * - 따라서, Run-Time Validation 이 필요하다.
             *****************************************************************/
            
            /**
             * Cursor 가 선언되어 있어야 함.
             */
            
            STL_TRY( ellFindCursorInstDesc( sInitPlan->mCursorName,
                                            sInitPlan->mCursorProcID, 
                                            & sCursorInstDesc,
                                            ELL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            STL_TRY_THROW( sCursorInstDesc != NULL, RAMP_ERR_CURSOR_NOT_EXIST );
            
            /**
             * Cursor 가 Open 되어 있어야 함.
             */
            
            STL_TRY_THROW( ellCursorIsOpen( sCursorInstDesc ) == STL_TRUE,
                           RAMP_ERR_CURSOR_IS_NOT_OPEN );
            
            /**
             * Result Set Desciptor 가 Updatable Cursor 여야 함.
             */

            sCursorSQLStmt = (qllStatement *)     sCursorInstDesc->mCursorSQLStmt;
            sResultSetDesc = (qllResultSetDesc *) sCursorSQLStmt->mResultSetDesc;
            
            STL_TRY_THROW( sResultSetDesc->mCursorProperty.mUpdatability
                           == ELL_CURSOR_UPDATABILITY_FOR_UPDATE,
                           RAMP_ERR_CURSOR_IS_NOT_UPDATABLE );
            
            /**
             * 갱신할 Table 이 FOR UPDATE 목록에 있어야 함.
             */
            
            for ( sLockTableItem = sResultSetDesc->mLockItemList;
                  sLockTableItem != NULL;
                  sLockTableItem = sLockTableItem->mNext )
            {
                /**
                 * UPDATE table 과 동일한 Result Set Desc 의 Lock Item 이 존재하는 지 검사
                 */
                
                if ( sOptUpdateStmt->mPhysicalHandle == sLockTableItem->mTablePhyHandle )
                {
                    /**
                     * 동일한 Table 을 찾음
                     */
                    break;
                }
            }
            
            STL_TRY_THROW( sLockTableItem != NULL, RAMP_ERR_CURSOR_NOT_IDENTIFY_TABLE );
            
            /*****************************************************************
             * Run-Time Validation : Cursor Position
             *****************************************************************/
            
            /**
             * Cursor 가 특정 Row 에 위치하고 있어야 함.
             */
            
            STL_TRY_THROW( sResultSetDesc->mPositionType == QLL_CURSOR_POSITION_ON_A_CERTAIN_ROW,
                           RAMP_ERR_NOT_FETCHED_POSITION );
            
            /******************************************************
             * Row Block 설정
             ******************************************************/
            
            /**
             * fetch record 정보 설정
             * - RID 를 이용해 가장 최신 정보를 읽는다.
             */
            sFetchRecordInfo = & sExeUpdateStmt->mFetchRecordInfo;
            
            sFetchRecordInfo->mRelationHandle  = sOptUpdateStmt->mPhysicalHandle;
            sFetchRecordInfo->mColList         = sExeUpdateStmt->mReadColBlock;
            sFetchRecordInfo->mRowIdColList    = NULL;
            sFetchRecordInfo->mPhysicalFilter  = NULL;
            sFetchRecordInfo->mTransReadMode   = SML_TRM_COMMITTED;  /* 다른 Tx 도 참조 */
            sFetchRecordInfo->mStmtReadMode    = SML_SRM_RECENT;     /* 다른 Stmt 도 참조 */
            sFetchRecordInfo->mScnBlock        = sExeUpdateStmt->mRowBlock->mScnBlock;
            sFetchRecordInfo->mLogicalFilter   = NULL;
            sFetchRecordInfo->mLogicalFilterEvalInfo = NULL;
            
            /**
             * Positioned Row 의 RID 와 SCN 정보를 Fetch Record Info 정보에 복사
             */

            stlMemcpy( & sExeUpdateStmt->mRowBlock->mRidBlock[0],
                       & sLockTableItem->mLockRowBlock->mRidBlock[sResultSetDesc->mBlockRowIdx],
                       STL_SIZEOF(smlRid) );

            stlMemcpy( & sExeUpdateStmt->mRowBlock->mScnBlock[0],
                       & sLockTableItem->mLockRowBlock->mScnBlock[sResultSetDesc->mBlockRowIdx],
                       STL_SIZEOF(smlScn) );
            
            /**
             * RID 를 이용한 Fetch Record
             */

            sDeleted = STL_FALSE;
            sIsMatched = STL_TRUE;

            STL_TRY( smlFetchRecord(
                         aStmt,
                         sFetchRecordInfo,
                         & sExeUpdateStmt->mRowBlock->mRidBlock[0],
                         0,
                         & sIsMatched,
                         & sDeleted,
                         NULL, /* aUpdated */
                         SML_ENV(aEnv) )
                     == STL_SUCCESS );

            /**
             * Positioned Row 가 이미 삭제되었는지 검사
             */
            
            STL_TRY_THROW( sDeleted == STL_FALSE, RAMP_ERR_POSITIONED_ROW_DELETED );
            STL_DASSERT( sIsMatched == STL_TRUE );

            /**
             * 한건을 모두 읽었음을 설정
             */
            
            SML_SET_USED_BLOCK_SIZE( sExeUpdateStmt->mRowBlock, 1 );
            KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sExeUpdateStmt->mReadColBlock, 0, 1 );

            /* Coverage : UPDATE CURRENT OF 구문에는 Write Column에 해당하는 Rowid Column이
             *            WHERE절이나 RETURN절에 올 수 없다. */
            STL_DASSERT( sExeUpdateStmt->mRowIdColBlock == NULL );
            
            sReadCnt = 1;
            sEOF = STL_TRUE;
        }
        else
        {
            /**
             * searched UPDATE 인 경우
             */
            
            /* fetch records*/
            QLNX_FETCH_NODE( & sFetchNodeInfo,
                             sOptUpdateStmt->mScanNode->mIdx,
                             0, /* Start Idx */
                             sSkipCnt, /* Skip Count */
                             sFetchCnt, /* Fetch Count */
                             & sReadCnt,
                             & sEOF,
                             aEnv );
        }

        sSkipCnt = 0;
        sFetchCnt -= sReadCnt;
        

        /**
         * UPDATE Row 수행
         */
        
        if( sReadCnt == 0 )
        {
            break;
        }
        else
        {
            /**
             * RESULT Pseudo Column 수행
             */

            if( sOptUpdateStmt->mQueryExprList->mInitExprList->mPseudoColExprList->mCount > 0 )
            {
                /* RESULT Pseudo Column 수행 */
                STL_TRY( qlxExecutePseudoColumnBlock(
                             aTransID,
                             aStmt,
                             DTL_ITERATION_TIME_FOR_EACH_EXPR,
                             sOptUpdateStmt->mTableHandle,
                             sExeUpdateStmt->mRowBlock,
                             sOptUpdateStmt->mQueryExprList->mInitExprList->mPseudoColExprList,
                             sExeUpdateStmt->mQueryPseudoColBlock,
                             0,
                             sReadCnt,
                             aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                /* nothing to do */
            }

            /**
             * Block Read 한 Row 들 중 유효한 Row 만 갱신
             */
        
            STL_TRY( qlxUpdateValidBlockRow( aTransID,
                                             aStmt,
                                             aDBCStmt,
                                             aSQLStmt,
                                             aDataArea,
                                             sExecNode,
                                             aEnv )
                     == STL_SUCCESS );

            /******************************************************
             * Check 무결성 검사 
             ******************************************************/
            
            if( sOptUpdateStmt->mHasCheck == STL_TRUE )
            {
                STL_TRY( qlxUpdateIntegrityCheckConst( aTransID,
                                                       aSQLStmt,
                                                       aDataArea,
                                                       sExecNode,
                                                       aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                /* check constraint가 없음 */
            }

            /******************************************************
             * Key 무결성 검사 및 Index Key 추가 
             ******************************************************/
            
            /**
             * 갱신한 Row 와 관련된 Index 들의 Key 를 갱신 
             */

            if( sOptUpdateStmt->mHasIndex == STL_TRUE )
            {
                STL_TRY( qlxUpdateIntegrityKeyConstIndexKey( aTransID,
                                                             aStmt,
                                                             aDataArea,
                                                             sExecNode,
                                                             aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                /* index 가 없음 */
            }

            /**
             * Version Conflict 가 발생한 경우 재수행한다.
             * - Block 내에 삭제 성공한 일부 data 가 있을 경우,
             * - RETRY 시 해당 data 를 skip 하게 되므로 index key 등에 대한 작업이 완료된 후 Retry 한다.
             */
            
            STL_TRY_THROW( sExeUpdateStmt->mVersionConflict == STL_FALSE, VERSION_CONFLICT_RESOLUTION_RETRY );
        }
    }

    
    /**********************************************
     * Immediate Key Constraint 를 위배했는지를 확인
     **********************************************/

    STL_TRY( qlxUpdateCheckStmtCollision( aTransID,
                                          aStmt,
                                          aSQLStmt,
                                          aDataArea,
                                          sExecNode,
                                          aEnv )
             == STL_SUCCESS );

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sExeUpdateStmt->mCommonInfo.mFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_COUNT( sExeUpdateStmt->mCommonInfo.mFetchCallCount, 1 );

    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CURSOR_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CURSOR_NOT_EXIST,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      sInitPlan->mCursorName );
    }

    STL_CATCH( RAMP_ERR_CURSOR_IS_NOT_OPEN )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CURSOR_IS_NOT_OPEN,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      sInitPlan->mCursorName );
    }

    /* Coverage : 향후 적용될 수 있는 코드임 */
    STL_CATCH( RAMP_ERR_CURSOR_IS_NOT_UPDATABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CURSOR_IS_NOT_UPDATABLE,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      sInitPlan->mCursorName );
    }

    /* Coverage : 향후 적용될 수 있는 코드임 */
    STL_CATCH( RAMP_ERR_CURSOR_NOT_IDENTIFY_TABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CURSOR_CANNOT_IDENTIFY_UNDERLYING_TABLE,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_NOT_FETCHED_POSITION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CURSOR_WAS_NOT_POSITIONED_ON_A_CERTAIN_ROW,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_POSITIONED_ROW_DELETED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CURSOR_WAS_POSITIONED_ON_A_ROW_THAT_HAD_BEEN_DELETED,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    if ( sInstantTable != NULL )
    {
        (void) qlnxDropInstantTable( sInstantTable,
                                     aEnv );
    }
    
    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sExeUpdateStmt->mCommonInfo.mFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_COUNT( sExeUpdateStmt->mCommonInfo.mFetchCallCount, 1 );

    return STL_FAILURE;
}


#if 0
/**
 * @brief UPDATE STATEMENT node를 Fetch한다.
 * @param[in]      aTransID      Transaction ID
 * @param[in]      aStmt         Statement
 * @param[in]      aDBCStmt      DBC Statement
 * @param[in,out]  aSQLStmt      SQL Statement
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aStartIdx     Start Index
 * @param[in]      aSkipCnt      Skip Record Count
 * @param[in]      aFetchCnt     Fetch Record Count
 * @param[out]     aUsedBlockCnt Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF          End Of Fetch 여부
 * @param[in]      aEnv          Environment
 *
 * @remark INTO 절이 없는 RETURN 절에 대한 Fetch 수행시 호출된다.
 */
stlStatus qlnxFetchUpdateStmt( smlTransId              aTransID,
                               smlStatement          * aStmt,
                               qllDBCStmt            * aDBCStmt,
                               qllStatement          * aSQLStmt,
                               qllOptimizationNode   * aOptNode,
                               qllDataArea           * aDataArea,
                               stlInt32                aStartIdx,
                               stlInt64                aSkipCnt,
                               stlInt64                aFetchCnt,
                               stlInt64              * aUsedBlockCnt,
                               stlBool               * aEOF,
                               qllEnv                * aEnv )
{
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_STMT_UPDATE_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    /* Do Nothing */
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
#endif


/**
 * @brief UPDATE STATEMENT node 수행을 종료한다.
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 *
 * @remark Execution을 종료하고자 할 때 호출한다.
 */
stlStatus qlnxFinalizeUpdateStmt( qllOptimizationNode   * aOptNode,
                                  qllDataArea           * aDataArea,
                                  qllEnv                * aEnv )
{
//    qllExecutionNode      * sExecNode = NULL;
//    qlnxUpdateStmt        * sExeUpdateStmt = NULL;

    stlInt16                sNodeCnt = 0;
    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_STMT_UPDATE_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    /**
     * Common Execution Node 획득
     */
        
//    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * UPDATE STATEMENT Execution Node 획득
     */

//    sExeUpdateStmt = sExecNode->mExecNode;


    /******************************************************
     * 하위 Node들에 대한 Finalize 수행
     ******************************************************/

    for( sNodeCnt = 1 ; /* Statement 제외한 노드들에 대해 Finalize 수행 */
         sNodeCnt < aDataArea->mExecNodeCnt ;
         sNodeCnt++ )
    {
        QLNX_FINALIZE_NODE( aDataArea->mExecNodeList[ sNodeCnt ].mOptNode,
                            aDataArea,
                            aEnv );
    }

    aDataArea->mIsFirstExecution = STL_TRUE;
    
    
    return STL_SUCCESS;

    STL_FINISH;

    for( sNodeCnt = 1 ; /* Statement 제외한 노드들에 대해 Finalize 수행 */
         sNodeCnt < aDataArea->mExecNodeCnt ;
         sNodeCnt++ )
    {
        QLNX_FINALIZE_NODE_IGNORE_FAILURE( aDataArea->mExecNodeList[ sNodeCnt ].mOptNode,
                                           aDataArea,
                                           aEnv );
    }

    return STL_FAILURE;
}


/** @} qlnx */


