/*******************************************************************************
 * qlnxDeleteStmt.c
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
 * @file qlnxDeleteStmt.c
 * @brief SQL Processor Layer Execution - DELETE STATEMENT Node
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnx
 * @{
 */


/**
 * @brief DELETE STATEMENT node를 초기화한다.
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
stlStatus qlnxInitializeDeleteStmt( smlTransId              aTransID,
                                    smlStatement          * aStmt,
                                    qllDBCStmt            * aDBCStmt,
                                    qllStatement          * aSQLStmt,
                                    qllOptimizationNode   * aOptNode,
                                    qllDataArea           * aDataArea,
                                    qllEnv                * aEnv )
{
    /* qllExecutionNode  * sExecNode = NULL; */
    qlnoDeleteStmt    * sOptDeleteStmt = NULL;
    /* qlnxDeleteStmt    * sExeDeleteStmt = NULL; */
    
    qllOptNodeItem    * sOptNodeItem = NULL;
    stlInt16            sNodeCnt = 0;

 
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_STMT_DELETE_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * DELETE STATEMENT Optimization Node 획득
     */

    sOptDeleteStmt = (qlnoDeleteStmt *) aOptNode->mOptNode;


    /**
     * Common Execution Node 획득
     */
        
    /* sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, */
    /*                                     QLL_GET_OPT_NODE_IDX( aOptNode ) ); */


    /**
     * DELETE STATEMENT Execution Node 할당
     */

    /* sExeDeleteStmt = (qlnxDeleteStmt *) sExecNode->mExecNode; */


    /******************************************************
     * 하위 Node들 Initialize 수행
     ******************************************************/

    sOptNodeItem = sOptDeleteStmt->mOptNodeList->mTail;
    
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
 * @brief DELETE STATEMENT node를 수행한다.
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
stlStatus qlnxExecuteDeleteStmt( smlTransId              aTransID,
                                 smlStatement          * aStmt,
                                 qllDBCStmt            * aDBCStmt,
                                 qllStatement          * aSQLStmt,
                                 qllOptimizationNode   * aOptNode,
                                 qllDataArea           * aDataArea,
                                 qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode = NULL;
    qlnoDeleteStmt        * sOptDeleteStmt = NULL;
    qlnxDeleteStmt        * sExeDeleteStmt = NULL;
    
    stlInt64                sSkipCnt = 0;
    stlInt64                sFetchCnt = 0;
    stlInt64                sReadCnt = 0;
    stlBool                 sEOF = STL_FALSE;

    qlnInstantTable       * sInstantTable    = NULL;

    /* DELETE 시 key 삭제를 하지 않은 것으로 보이는 현상을 위한 Dubugging 을 위한 정보 추가 */
#ifdef STL_DEBUG
    stlInt32   sKeyDeleteCnt = 0;
    stlInt32   i = 0;
#endif
    
    /*
     * for positioned DELETE
     */

    qlvInitDelete         * sInitPlan  = NULL;

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


    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_STMT_DELETE_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );

    
    QLL_OPT_CHECK_TIME( sBeginFetchTime );

    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * DELETE STATEMENT Optimization Node 획득
     */

    sInitPlan      = (qlvInitDelete *) qllGetInitPlan( aSQLStmt );
    sOptDeleteStmt = (qlnoDeleteStmt *) aOptNode->mOptNode;

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
     * DELETE STATEMENT Execution Node 획득
     */

    sExeDeleteStmt = sExecNode->mExecNode;

    
    /**
     * 기본 정보 설정
     */

#ifdef STL_DEBUG
    sKeyDeleteCnt = 0;
#endif
    aDataArea->mWriteRowCnt = 0;

    /**
     * Constraint Check Time 획득,
     */
    
    STL_TRY( qlxDeleteGetConstraintCheckTime( & sInitPlan->mRelObject,
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

    /* if( sOptDeleteStmt->mStmtExprList->mInitExprList->mPseudoColExprList->mCount > 0 ) */
    /* { */
    /*     /\* STATEMENT 단위 Pseudo Column 수행 *\/ */
    /*     STL_TRY( qlxExecutePseudoColumnBlock( */
    /*                  aTransID, */
    /*                  aStmt, */
    /*                  DTL_ITERATION_TIME_FOR_EACH_STATEMENT, */
    /*                  sOptDeleteStmt->mTableHandle, */
    /*                  sExeDeleteStmt->mRowBlock, */
    /*                  sOptDeleteStmt->mStmtExprList->mInitExprList->mPseudoColExprList, */
    /*                  sExeDeleteStmt->mTotalPseudoColBlock, */
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
    
    STL_TRY( qlxEvaluateConstExpr( sOptDeleteStmt->mStmtExprList->mConstExprStack,
                                   aDataArea,
                                   KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    
    
    sEOF = STL_FALSE;
    sReadCnt = 0;


    /******************************************************
     * DELETE 시작 
     ******************************************************/

    STL_RAMP( VERSION_CONFLICT_RESOLUTION_RETRY );

    if ( sExeDeleteStmt->mVersionConflict == STL_TRUE )
    {
        STL_ASSERT( (sInitPlan->mSerialAction == QLV_SERIAL_RETRY_ONLY) ||
                    (sInitPlan->mSerialAction == QLV_SERIAL_ROLLBACK_RETRY) );
        
        /**
         * Instant Table 을 비운다.
         */
        
        if ( (sInstantTable != NULL) && (sInitPlan->mSerialAction == QLV_SERIAL_ROLLBACK_RETRY) )
        {
            /**********************************************************
             * DELETE RETURNING QUERY 이고,
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
             * 삭제한 Row가  Rollback 되지 않는다.
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
             * 삭제한 Row가  Rollback 됨.
             */

#ifdef STL_DEBUG
            sKeyDeleteCnt = 0;
#endif
            aDataArea->mWriteRowCnt = 0;

            STL_TRY( qlxDeleteResetConstraintCollision( & sInitPlan->mRelObject,
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
        
        
    sExeDeleteStmt->mVersionConflict = STL_FALSE;
    sEOF = STL_FALSE;
    
    /******************************************************
     * 하위 Node Execution
     ******************************************************/
    
    if ( aSQLStmt->mStmtType == QLL_STMT_DELETE_WHERE_CURRENT_OF_TYPE )
    {
        /**
         * positioned DELETE 인 경우
         * - RID 를 이용한 Fetch 를 수행하므로 하위 Node 를 수행하지 않는다.
         */

    }
    else
    {
        /**
         * searched DELETE 인 경우
         */
        
        /******************************************************
         * 하위 Node들 Execute 수행
         ******************************************************/
        
        QLNX_EXECUTE_NODE( aTransID,
                           aStmt,
                           aDBCStmt,
                           aSQLStmt,
                           aDataArea->mExecNodeList[ sOptDeleteStmt->mScanNode->mIdx ].mOptNode,
                           aDataArea,
                           aEnv );
    }

    
    /******************************************************
     * DELETE 수행
     ******************************************************/

    /**
     * Skip Count 설정
     */

    if( sOptDeleteStmt->mResultSkip == NULL )
    {
        sSkipCnt = sOptDeleteStmt->mSkipCnt;
    }
    else if( sOptDeleteStmt->mResultSkip->mType == QLV_EXPR_TYPE_VALUE )
    {
        sSkipCnt = sOptDeleteStmt->mSkipCnt;
    }
    else
    {
        STL_TRY( qlnxGetBindedValue4FETCH( aDBCStmt,
                                           aSQLStmt,
                                           STL_FALSE, /* aIsFetch */
                                           sOptDeleteStmt->mResultSkip,
                                           & sSkipCnt,
                                           aEnv )
                 == STL_SUCCESS );
    }

    /**
     * Fetch Count 설정
     */
    
    if( sOptDeleteStmt->mResultLimit == NULL )
    {
        sFetchCnt = sOptDeleteStmt->mFetchCnt;
    }
    else if( sOptDeleteStmt->mResultLimit->mType == QLV_EXPR_TYPE_VALUE )
    {
        sFetchCnt = sOptDeleteStmt->mFetchCnt;
    }
    else
    {
        STL_TRY( qlnxGetBindedValue4FETCH( aDBCStmt,
                                           aSQLStmt,
                                           STL_TRUE, /* aIsFetch */
                                           sOptDeleteStmt->mResultLimit,
                                           & sFetchCnt,
                                           aEnv )
                 == STL_SUCCESS );
    }

    
    /**
     * 조건에 맞는 데이터가 없을 때까지 반복
     */

    while( ( sFetchCnt > 0 ) && ( sEOF == STL_FALSE ) )
    {
        /**
         * Fetch 수행
         */

        if ( aSQLStmt->mStmtType == QLL_STMT_DELETE_WHERE_CURRENT_OF_TYPE )
        {
            /**
             * positioned DELETE 인 경우
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
                 * DELETE table 과 동일한 Result Set Desc 의 Lock Item 이 존재하는 지 검사
                 */
                
                if ( sOptDeleteStmt->mPhysicalHandle == sLockTableItem->mTablePhyHandle )
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
            sFetchRecordInfo = & sExeDeleteStmt->mFetchRecordInfo;
            
            sFetchRecordInfo->mRelationHandle  = sOptDeleteStmt->mPhysicalHandle;
            sFetchRecordInfo->mColList         = sExeDeleteStmt->mReadColBlock;
            sFetchRecordInfo->mRowIdColList    = NULL;
            sFetchRecordInfo->mPhysicalFilter  = NULL;
            sFetchRecordInfo->mTransReadMode   = SML_TRM_COMMITTED;  /* 다른 Tx 도 참조 */
            sFetchRecordInfo->mStmtReadMode    = SML_SRM_RECENT;     /* 다른 Stmt 도 참조 */
            sFetchRecordInfo->mScnBlock        = sExeDeleteStmt->mRowBlock->mScnBlock;
            sFetchRecordInfo->mLogicalFilter   = NULL;
            sFetchRecordInfo->mLogicalFilterEvalInfo = NULL;

            /**
             * Positioned Row 의 RID 와 SCN 정보를 Fetch Record Info 정보에 복사
             */

            stlMemcpy( & sExeDeleteStmt->mRowBlock->mRidBlock[0],
                       & sLockTableItem->mLockRowBlock->mRidBlock[sResultSetDesc->mBlockRowIdx],
                       STL_SIZEOF(smlRid) );

            stlMemcpy( & sExeDeleteStmt->mRowBlock->mScnBlock[0],
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
                         & sExeDeleteStmt->mRowBlock->mRidBlock[0],
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
            
            SML_SET_USED_BLOCK_SIZE( sExeDeleteStmt->mRowBlock, 1 );
            KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sExeDeleteStmt->mReadColBlock, 0, 1 );

            /* Coverage : DELETE CURRENT OF 구문에는 Rowid Column이 WHERE절이나 RETUEN절에 올 수 없다. */
            STL_DASSERT( sExeDeleteStmt->mRowIdColBlock == NULL );
            
            sReadCnt = 1;
            sEOF = STL_TRUE;
        }
        else
        {
            /**
             * searched DELETE 인 경우
             */
        
            /* fetch records*/
            QLNX_FETCH_NODE( & sFetchNodeInfo,
                             sOptDeleteStmt->mScanNode->mIdx,
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
         * DELETE Row 수행
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

            /* Coverage : Sequence는 search condition이나 return 절에 사용할 수 없으므로
             *            delete 구문에서는 참조가 불가하다. */
            STL_DASSERT( sOptDeleteStmt->mQueryExprList->mInitExprList->mPseudoColExprList->mCount == 0 );
            
            /* if( sOptDeleteStmt->mQueryExprList->mInitExprList->mPseudoColExprList->mCount > 0 ) */
            /* { */
            /*     /\* RESULT Pseudo Column 수행 *\/ */
            /*     STL_TRY( qlxExecutePseudoColumnBlock( */
            /*                  aTransID, */
            /*                  aStmt, */
            /*                  DTL_ITERATION_TIME_FOR_EACH_EXPR, */
            /*                  sOptDeleteStmt->mTableHandle, */
            /*                  sExeDeleteStmt->mRowBlock, */
            /*                  sOptDeleteStmt->mQueryExprList->mInitExprList->mPseudoColExprList, */
            /*                  sExeDeleteStmt->mQueryPseudoColBlock, */
            /*                  0, */
            /*                  sReadCnt, */
            /*                  aEnv ) */
            /*              == STL_SUCCESS ); */
            /* } */
            /* else */
            /* { */
            /*     /\* nothing to do *\/ */
            /* } */


            /**
             * Block Read 한 Row 들 중 유효한 Row 만 갱신
             */
        
            STL_TRY( qlxDeleteValidBlockRow( aTransID,
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
            
            /**
             * 삭제한 Row 와 관련된 NOT NULL, CHECK, identity column 제약 조건들을 검사
             */
            
            if ( sOptDeleteStmt->mHasCheck == STL_TRUE )
            {
                STL_TRY( qlxDeleteIntegrityCheckConstraint( aTransID,
                                                            aStmt,
                                                            aDataArea,
                                                            sExecNode,
                                                            aEnv )
                         == STL_SUCCESS );
                
            }
            else
            {
                /* deferrable check constraint 가 없음 */
            }

            /******************************************************
             * Key 무결성 검사 및 Index Key 삭제
             ******************************************************/
            
            /**
             * 삭제한 Row 와 관련된 Index 들의 Key 를 갱신
             */

            if( sOptDeleteStmt->mHasIndex == STL_TRUE )
            {
                
#ifdef STL_DEBUG
                /***********************************************************************************
                 * DELETE 시 key 삭제를 하지 않은 것으로 보이는 현상을 위한 Dubugging 을 위한 정보 추가
                 ***********************************************************************************/

                for ( i = 0; i < sExeDeleteStmt->mRowBlock->mUsedBlockSize; i++ )
                {
                    if ( sExeDeleteStmt->mIsDeleted[i] == STL_TRUE )
                    {
                        sKeyDeleteCnt++;
                    }
                }
                /* delete record 개수와 delete key 개수가 동일해야 함. */
                STL_DASSERT( aDataArea->mWriteRowCnt == sKeyDeleteCnt );
#endif
                
                
                STL_TRY( qlxDeleteIntegrityKeyConstIndexKey( aTransID,
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
            
            STL_TRY_THROW( sExeDeleteStmt->mVersionConflict == STL_FALSE, VERSION_CONFLICT_RESOLUTION_RETRY );
            
        }
    }

    /**********************************************
     * Deferred Constraint 에 대한 정보 등록
     **********************************************/

    STL_TRY( qlxDeleteCheckStmtCollision( aTransID,
                                          aStmt,
                                          aDataArea,
                                          sExecNode,
                                          aEnv )
             == STL_SUCCESS );

    
    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sExeDeleteStmt->mCommonInfo.mFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_COUNT( sExeDeleteStmt->mCommonInfo.mFetchCallCount, 1 );

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
    QLL_OPT_ADD_ELAPSED_TIME( sExeDeleteStmt->mCommonInfo.mFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_COUNT( sExeDeleteStmt->mCommonInfo.mFetchCallCount, 1 );

    return STL_FAILURE;
}


#if 0
/**
 * @brief DELETE STATEMENT node를 Fetch한다.
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
stlStatus qlnxFetchDeleteStmt( smlTransId              aTransID,
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
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_STMT_DELETE_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    /* Do Nothing */
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
#endif


/**
 * @brief DELETE STATEMENT node 수행을 종료한다.
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 *
 * @remark Execution을 종료하고자 할 때 호출한다.
 */
stlStatus qlnxFinalizeDeleteStmt( qllOptimizationNode   * aOptNode,
                                  qllDataArea           * aDataArea,
                                  qllEnv                * aEnv )
{
//    qllExecutionNode      * sExecNode = NULL;
//    qlnxDeleteStmt        * sExeDeleteStmt = NULL;

    stlInt16                sNodeCnt = 0;
    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_STMT_DELETE_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    /**
     * Common Execution Node 획득
     */
        
//    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * DELETE STATEMENT Execution Node 획득
     */

//    sExeDeleteStmt = sExecNode->mExecNode;


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


