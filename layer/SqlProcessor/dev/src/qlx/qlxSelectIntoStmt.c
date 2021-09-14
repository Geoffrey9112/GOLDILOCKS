/*******************************************************************************
 * qlxSelectIntoStmt.c
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
 * @file qlxSelectIntoStmt.c
 * @brief SQL Processor Layer : Execution of SELECT INTO statement
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlxSelectInto
 * @{
 */


/**
 * @brief SELECT INTO 구문을 실행한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aStmt      Statement
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement 객체
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlxExecuteSelectInto( smlTransId      aTransID,
                                smlStatement  * aStmt,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv )
{
    qllOptimizationNode  * sOptNode     = NULL;
    qllDataArea          * sDataArea    = NULL;

    knlValueBlockList    * sTargetBlock = NULL;
    dtlDataValue         * sTargetValue = NULL;
    dtlDataValue         * sIntoValue = NULL;

    knlValueBlockList    * sIntoBlock   = NULL;

    stlInt64               sFetchCnt    = 0;
    stlInt64               sReadCnt     = 0;

    stlBool                sEOF         = STL_FALSE;
    stlInt32               sLoop        = 0;

    qllResultSetDesc     * sResultSetDesc = NULL;
    qllCursorLockItem    * sLockItem = NULL;

    stlBool                sVersionConflict = STL_FALSE;

    qlnxFetchNodeInfo      sFetchNodeInfo;
    
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_SELECT_INTO_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_SELECT_INTO_FOR_UPDATE_TYPE),
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
                                    NULL, 
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
    
    /**
     * Result Set 시작을 설정
     */
    
    sResultSetDesc = aSQLStmt->mResultSetDesc;
    sResultSetDesc->mIsOpen = STL_TRUE;

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
    
    
    /******************************************************
     * Fetch 수행
     ******************************************************/

    QLNX_INIT_FETCH_NODE_INFO( & sFetchNodeInfo,
                               aTransID,
                               aStmt,
                               aDBCStmt,
                               aSQLStmt,
                               sDataArea );

    STL_RAMP( VERSION_CONFLICT_RESOLUTION_RETRY );

    
    /**
     * Fetch 수행
     */

    sFetchCnt = 0;

    while( 1 )
    {
        sEOF = STL_FALSE;
        
        /* fetch records*/
        QLNX_FETCH_NODE( & sFetchNodeInfo,
                         QLL_GET_OPT_NODE_IDX( sOptNode ),
                         0, /* Start Idx */
                         0, /* Skip Count */
                         2, /* Fetch Count */
                         & sReadCnt,
                         & sEOF,
                         aEnv );

        STL_TRY_THROW( sFetchCnt + sReadCnt <= 1, RAMP_ERR_SINGLE_ROW_EXPRESSION );

        /* EOF 평가 */
        if( sEOF == STL_TRUE )
        {
            if( sReadCnt == 0 )
            {
                /* Fetch 된 Data가 없음 */
                STL_THROW( RAMP_FINISH );
            }
            else
            {
                /* INTO 절 수행 */
                sFetchCnt = sReadCnt;
            }
            break;
        }
        else
        {
            if( sReadCnt == 0 )
            {
                /* Fetch 반복 수행 */
            }
            else
            {
                /* INTO 절 수행 */
                sFetchCnt = sReadCnt;
                break;
            }
        }
    }

    /******************************************************
     * FOR UPDATE 절 처리 
     ******************************************************/

    /**
     * SELECT INTO .. FOR UPDATE 는 Cursor 를 통해 제어하지 않으므로, 
     * SELECT FOR UPDATE 와 달리 Instant Table 을 생성하지 않는다.
     */

    if ( ( sResultSetDesc->mCursorProperty.mUpdatability == ELL_CURSOR_UPDATABILITY_FOR_UPDATE) &&
         ( sFetchCnt == 1 ) )
    {
        for ( sLockItem = sResultSetDesc->mLockItemList;
              sLockItem != NULL;
              sLockItem = sLockItem->mNext )
        {
            sVersionConflict = STL_FALSE;
            STL_TRY( smlLockRecordForUpdate( aStmt,
                                             sLockItem->mTablePhyHandle,
                                             sLockItem->mLockRowBlock->mRidBlock[0],
                                             sLockItem->mLockRowBlock->mScnBlock[0],
                                             sResultSetDesc->mLockTimeInterval,
                                             & sVersionConflict,
                                             SML_ENV(aEnv) )
                     == STL_SUCCESS );

            if ( sVersionConflict == STL_TRUE )
            {
                /**
                 * Query 를 종료한다.
                 */
                
                QLNX_FINALIZE_NODE( sOptNode,
                                    sDataArea,
                                    aEnv );
                
                /**
                 * SM Statement RESET
                 */
                
                STL_TRY( smlResetStatement( aStmt,
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
                /**
                 * 1 Table 에 대해 lock 을 획득함
                 */
            }
        }
        
    }
    else
    {
        /**
         * FOR UPDATE 구문이 아님
         */
    }
    
    /******************************************************
     * INTO 절 처리 
     ******************************************************/

    if( sFetchCnt == 0 )
    {
        /**
         * Fetch 된 Row 가 없음
         */

        /* Do Nothing */
    }
    else
    {
        sLoop = 0;
        for ( sTargetBlock = sDataArea->mTargetBlock, sIntoBlock = sDataArea->mIntoBlock;
              sTargetBlock != NULL ;
              sTargetBlock = sTargetBlock->mNext, sIntoBlock = sIntoBlock->mNext )
        {
            /**
             * Target Value 획득
             */

            sTargetValue = DTL_GET_BLOCK_FIRST_DATA_VALUE( sTargetBlock );

            
            /**
             * INTO assignment
             */

            if( DTL_IS_NULL( sTargetValue ) == STL_TRUE )
            {
                DTL_SET_NULL( DTL_GET_BLOCK_FIRST_DATA_VALUE( sIntoBlock ) );
            }
            else
            {
                sIntoValue = DTL_GET_BLOCK_FIRST_DATA_VALUE( sIntoBlock );

                STL_TRY( knlCopyDataValue( sTargetValue,
                                           sIntoValue,
                                           KNL_ENV(aEnv) )
                         == STL_SUCCESS );
            }

            sLoop++;
        }
        KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sDataArea->mIntoBlock, 0, 1 );
    }


    /**
     * 한건만 존재하는지 확인
     */
    
    while( sEOF == STL_FALSE )
    {
        /* fetch records*/
        sReadCnt = 0;
        QLNX_FETCH_NODE( & sFetchNodeInfo,
                         QLL_GET_OPT_NODE_IDX( sOptNode ),
                         0, /* Start Idx */
                         0, /* Skip Count */
                         1, /* Fetch Count */
                         & sReadCnt,
                         & sEOF,
                         aEnv );

        STL_TRY_THROW( sReadCnt == 0, RAMP_ERR_SINGLE_ROW_EXPRESSION );
    }


    STL_RAMP( RAMP_FINISH );


    /**
     * Cursor Result Set
     * - Long Varying Data 도 해제된다.
     */

    STL_DASSERT( sEOF == STL_TRUE );
    
    STL_TRY( qlcrCloseResultSet( sResultSetDesc->mQueryStmt, aEnv ) == STL_SUCCESS );

    sResultSetDesc = NULL;


    /******************************************************
     * Statement 수행 결과 정보 설정
     ******************************************************/

    /**
     * Fetch Row Count 설정
     */
    
    sDataArea->mFetchRowCnt = sFetchCnt;


    /******************************************************
     * Finalize Execution Node
     ******************************************************/

    QLNX_FINALIZE_NODE( sOptNode,
                        sDataArea,
                        aEnv );


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SINGLE_ROW_EXPRESSION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INTO_CLAUSE_MULTIPLE_ROWS,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    if( sResultSetDesc != NULL )
    {
        (void) qlcrCloseResultSet( sResultSetDesc->mQueryStmt, aEnv );
    }

    /* Execution Node 종료 */
    QLNX_FINALIZE_NODE_IGNORE_FAILURE( sOptNode,
                                       sDataArea,
                                       aEnv );
    
    return STL_FAILURE;
}



/** @} qlxSelectInto */


