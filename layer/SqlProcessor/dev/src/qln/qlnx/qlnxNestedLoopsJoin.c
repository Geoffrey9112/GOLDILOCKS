/*******************************************************************************
 * qlnxNestedLoopsJoin.c
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
 * @file qlnxNestedLoopsJoin.c
 * @brief SQL Processor Layer Execution Node - NESTED LOOPS JOIN
 */

#include <qll.h>
#include <qlDef.h>

/**
 * @addtogroup qlnx
 * @{
 */


const qlnxNestedJoinFetchFuncPtr qlnxNestedJoinFetchFuncList[ QLV_JOIN_TYPE_MAX ] =
{
    NULL,                                       /**< QLV_JOIN_TYPE_NONE */
    NULL,                                       /**< QLV_JOIN_TYPE_CROSS */
    qlnxFetchNestedLoopsJoin_Inner,             /**< QLV_JOIN_TYPE_INNER */

    qlnxFetchNestedLoopsJoin_Outer,             /**< QLV_JOIN_TYPE_LEFT_OUTER */
    qlnxFetchNestedLoopsJoin_Outer,             /**< QLV_JOIN_TYPE_RIGHT_OUTER */
    qlnxFetchNestedLoopsJoin_Outer,             /**< QLV_JOIN_TYPE_FULL_OUTER */

    qlnxFetchNestedLoopsJoin_Semi,              /**< QLV_JOIN_TYPE_LEFT_SEMI */
    qlnxFetchNestedLoopsJoin_Semi,              /**< QLV_JOIN_TYPE_RIGHT_SEMI */

    qlnxFetchNestedLoopsJoin_AntiSemi,          /**< QLV_JOIN_TYPE_LEFT_ANTI_SEMI */
    qlnxFetchNestedLoopsJoin_AntiSemi,          /**< QLV_JOIN_TYPE_RIGHT_ANTI_SEMI */

    NULL,                                       /**< QLV_JOIN_TYPE_LEFT_ANTI_SEMI_NA */
    NULL,                                       /**< QLV_JOIN_TYPE_RIGHT_ANTI_SEMI_NA */

    qlnxFetchNestedLoopsJoin_Inner,             /**< QLV_JOIN_TYPE_INVERTED_LEFT_SEMI */
    qlnxFetchNestedLoopsJoin_Inner,             /**< QLV_JOIN_TYPE_INVERTED_RIGHT_SEMI */
};


/**
 * @brief NESTED LOOPS JOIN node를 초기화한다.
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
stlStatus qlnxInitializeNestedLoopsJoin( smlTransId              aTransID,
                                         smlStatement          * aStmt,
                                         qllDBCStmt            * aDBCStmt,
                                         qllStatement          * aSQLStmt,
                                         qllOptimizationNode   * aOptNode,
                                         qllDataArea           * aDataArea,
                                         qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode           = NULL;
    /* qlnoNestedLoopsJoin   * sOptNestedLoopsJoin = NULL; */
    qlnxNestedLoopsJoin   * sExeNestedLoopsJoin = NULL;


    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_NESTED_LOOPS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * NESTED LOOPS JOIN Optimization Node 획득
     */

    /* sOptNestedLoopsJoin = (qlnoNestedLoopsJoin *) aOptNode->mOptNode; */


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea,
                                        QLL_GET_OPT_NODE_IDX( aOptNode ) );

    
    /**
     * NESTED LOOPS JOIN Execution Node 할당
     */

    sExeNestedLoopsJoin = (qlnxNestedLoopsJoin *) sExecNode->mExecNode;

    sExeNestedLoopsJoin->mHasFalseFilter = STL_FALSE;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief NESTED LOOPS JOIN node를 수행한다.
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
stlStatus qlnxExecuteNestedLoopsJoin( smlTransId              aTransID,
                                      smlStatement          * aStmt,
                                      qllDBCStmt            * aDBCStmt,
                                      qllStatement          * aSQLStmt,
                                      qllOptimizationNode   * aOptNode,
                                      qllDataArea           * aDataArea,
                                      qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode               = NULL;
    qlnoNestedLoopsJoin   * sOptNestedLoopsJoin     = NULL;
    qlnxNestedLoopsJoin   * sExeNestedLoopsJoin     = NULL;
    qlnxCacheBlockInfo    * sCacheBlockInfo         = NULL;

    qlnxOuterColumnBlockItem    * sOuterColumnBlockItem = NULL;


    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_NESTED_LOOPS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    

    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
   
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * NESTED LOOPS JOIN Optimization Node 획득
     */

    sOptNestedLoopsJoin = (qlnoNestedLoopsJoin *) aOptNode->mOptNode;


    /**
     * NESTED LOOPS JOIN Execution Node 획득
     */

    sExeNestedLoopsJoin = sExecNode->mExecNode;

    
    /**
     * @todo 불필요한 요소를 제거하여 Filter 재구성 (확장 필요)
     */
    
    sExeNestedLoopsJoin->mHasFalseFilter =
        qlfHasFalseFilter( sOptNestedLoopsJoin->mLogicalFilterExpr,
                           aDataArea );

    if( sExeNestedLoopsJoin->mHasFalseFilter == STL_FALSE )
    {
        switch( sExeNestedLoopsJoin->mJoinType )
        {
            case QLV_JOIN_TYPE_LEFT_OUTER :
            case QLV_JOIN_TYPE_RIGHT_OUTER :
            case QLV_JOIN_TYPE_FULL_OUTER :
            case QLV_JOIN_TYPE_LEFT_ANTI_SEMI :
            case QLV_JOIN_TYPE_RIGHT_ANTI_SEMI :
            case QLV_JOIN_TYPE_LEFT_ANTI_SEMI_NA :
            case QLV_JOIN_TYPE_RIGHT_ANTI_SEMI_NA :
                /* Do Nothing */
                break;
            default :
                sExeNestedLoopsJoin->mHasFalseFilter =
                    qlfHasFalseFilter( sOptNestedLoopsJoin->mJoinFilterExpr,
                                       aDataArea );
                break;
        }
    }
    

    /**
     * Left Outer Column 및 Right Outer Column에 대한 Data Value 설정 변경
     */

    QLNX_SET_OUTER_COLUMN_DATA_VALUE_INFO( &(sExeNestedLoopsJoin->mLeftOuterColumnBlockList) );
    QLNX_SET_OUTER_COLUMN_DATA_VALUE_INFO( &(sExeNestedLoopsJoin->mRightOuterColumnBlockList) );


    /**
     * Outer Table 초기화
     */

    sCacheBlockInfo = &(sExeNestedLoopsJoin->mOuterCacheBlockInfo);
    QLNX_EXECUTE_NODE( aTransID,
                       aStmt,
                       aDBCStmt,
                       aSQLStmt,
                       aDataArea->mExecNodeList[sCacheBlockInfo->mNodeIdx].mOptNode,
                       aDataArea,
                       aEnv );
    QLNX_RESET_CACHE_BLOCK_INFO( sCacheBlockInfo );


    /**
     * Inner Table 초기화
     * ----------------------
     *  Inner Table은 Fetch를 새로 해야 하는 것으로 셋팅해야 한다.
     *  실제 Fetch에서 Execute를 호출하므로 여기서 Execute를
     *  호출할 필요가 없다.
     */

    sCacheBlockInfo = &(sExeNestedLoopsJoin->mInnerCacheBlockInfo);
    QLNX_EXECUTE_NODE( aTransID,
                       aStmt,
                       aDBCStmt,
                       aSQLStmt,
                       aDataArea->mExecNodeList[sCacheBlockInfo->mNodeIdx].mOptNode,
                       aDataArea,
                       aEnv );
    QLNX_SET_END_OF_TABLE_FETCH( sCacheBlockInfo );


    /**
     * Reset을 위한 기타 정보들 초기화
     */

    /* Result Block Index */
    sExeNestedLoopsJoin->mCurrBlockIdx = 0;

    /* First Fetch 여부 */
    sExeNestedLoopsJoin->mIsFirstFetch = STL_TRUE;

    /* EOF 여부 */
    sExeNestedLoopsJoin->mIsEOF = STL_FALSE;

    /* Inner와 Outer의 Switching 여부 */
    sExeNestedLoopsJoin->mIsSwitched = STL_FALSE;

    /* Outer Column의 DstDataValue 정보 복구 */
    sOuterColumnBlockItem = sExeNestedLoopsJoin->mLeftOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );
    sOuterColumnBlockItem = sExeNestedLoopsJoin->mRightOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );
    sOuterColumnBlockItem = sExeNestedLoopsJoin->mWhereOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );


    return STL_SUCCESS;

    STL_FINISH;

    /* Outer Column의 DstDataValue 정보 복구 */
    sOuterColumnBlockItem = sExeNestedLoopsJoin->mLeftOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );
    sOuterColumnBlockItem = sExeNestedLoopsJoin->mRightOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );
    sOuterColumnBlockItem = sExeNestedLoopsJoin->mWhereOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );

    return STL_FAILURE;
}


/**
 * @brief Inner Join에 대한 NESTED LOOPS JOIN node를 Fetch한다.
 * @param[in]      aFetchNodeInfo  Fetch Node Info
 * @param[out]     aUsedBlockCnt   Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF            End Of Fetch 여부
 * @param[in]      aEnv            Environment
 *
 * @remark Fetch 수행시 호출된다.
 */
stlStatus qlnxFetchNestedLoopsJoin_Inner( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                          stlInt64              * aUsedBlockCnt,
                                          stlBool               * aEOF,
                                          qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode               = NULL;
    qlnxNestedLoopsJoin   * sExeNestedLoopsJoin     = NULL;

    stlBool                 sEOF                    = STL_FALSE;

    stlInt64                sUsedBlockCnt           = 0;
    stlInt64                sRemainSkipCnt          = 0;
    stlInt64                sRemainFetchCnt         = 0;

    qlnxCacheBlockInfo    * sInnerCacheBlockInfo    = NULL;
    qlnxCacheBlockInfo    * sOuterCacheBlockInfo    = NULL;

    knlExprEvalInfo       * sWhereExprEvalInfo      = NULL;
    knlExprEvalInfo       * sJoinExprEvalInfo       = NULL;

    qlnxOuterColumnBlockItem    * sOuterColumnBlockItem = NULL;

    qlnxRowBlockItem      * sRowBlockItem           = NULL;
    stlInt64                sTempBlockIdx           = 0;

    qlnxFetchNodeInfo       sLocalFetchInfo;

    QLL_OPT_DECLARE( stlTime sBeginFetchTime );
    QLL_OPT_DECLARE( stlTime sEndFetchTime );
    QLL_OPT_DECLARE( stlTime sFetchTime );

    QLL_OPT_DECLARE( stlTime sBeginExceptTime );
    QLL_OPT_DECLARE( stlTime sEndExceptTime );
    QLL_OPT_DECLARE( stlTime sExceptTime );

    QLL_OPT_SET_VALUE( sFetchTime, 0 );
    QLL_OPT_SET_VALUE( sExceptTime, 0 );


    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aFetchNodeInfo->mOptNode->mType == QLL_PLAN_NODE_NESTED_LOOPS_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    QLL_OPT_CHECK_TIME( sBeginFetchTime );

    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aFetchNodeInfo->mDataArea,
                                        QLL_GET_OPT_NODE_IDX( aFetchNodeInfo->mOptNode ) );


    /**
     * NESTED LOOPS JOIN Execution Node 획득
     */

    sExeNestedLoopsJoin = sExecNode->mExecNode;


    /**
     * Node 수행 여부 확인 
     */

    STL_DASSERT( *aEOF == STL_FALSE );
    STL_DASSERT( aFetchNodeInfo->mFetchCnt > 0);


    /**
     * Used Block Count 초기화
     */

    sUsedBlockCnt = 0;


    /**
     * Current Block Idx 초기화
     */

    sExeNestedLoopsJoin->mCurrBlockIdx = 0;


    /**
     * Cache Block Info 정보 설정
     */

    sInnerCacheBlockInfo = &(sExeNestedLoopsJoin->mInnerCacheBlockInfo);
    sOuterCacheBlockInfo = &(sExeNestedLoopsJoin->mOuterCacheBlockInfo);


    /**
     * Evaluate Info 관련 정보 설정
     */

    sWhereExprEvalInfo  = &sExeNestedLoopsJoin->mWhereExprEvalInfo;
    sJoinExprEvalInfo   = &sExeNestedLoopsJoin->mJoinExprEvalInfo;

    sWhereExprEvalInfo->mBlockCount = 1;
    sJoinExprEvalInfo->mBlockCount = 1;


    /**
     * Local Fetch Info 초기화
     */
    
    QLNX_SET_LOCAL_FETCH_INFO( & sLocalFetchInfo, aFetchNodeInfo );


    /*****************************************
     * Node 수행 여부 확인
     ****************************************/

    /* 이전 연산에 의헤 sExeNestedLoopsJoin->mIsEOF가 true로 설정된 경우, 이 함수를 다시 들어올 수 없다.
       sExeNestedLoopsJoin->mIsEOF가 true로 설정된 경우 반환되는 EOF 정보도 true로 설정되어 있다. */
    STL_DASSERT( sExeNestedLoopsJoin->mIsEOF == STL_FALSE );

    /* Inner Join 은 filter가 없기 때문에 false filter를 체크하지 못한다. */
    STL_DASSERT( sExeNestedLoopsJoin->mHasFalseFilter == STL_FALSE );


    /**
     * Fetch에 필요한 정보 설정
     */

    sRemainSkipCnt = aFetchNodeInfo->mSkipCnt;
    sRemainFetchCnt = ( aFetchNodeInfo->mFetchCnt < aFetchNodeInfo->mDataArea->mBlockAllocCnt
                        ? aFetchNodeInfo->mFetchCnt
                        : aFetchNodeInfo->mDataArea->mBlockAllocCnt );


    /*****************************************
     * Fetch 수행
     ****************************************/

    while( 1 )
    {
        /**
         * Block Read
         */

        STL_RAMP( RAMP_READ_NEXT_ONE_ROW );

        if( QLNX_IS_END_OF_TABLE_FETCH( sInnerCacheBlockInfo ) )
        {
            /**
             * Inner Table을 처음부터 읽어야 하는 경우
             */

            /**
             * Outer Table에서 하나의 레코드를 읽고
             * Inner Table은 Node 초기화를 해서 최종 개수만큼
             * Read하여 Nested Loops Join 연산을 수행한다.
             */

            /**
             * Outer Table의 Cache Block에 아직 읽을 데이터가 남았는지 체크
             */

            if( QLNX_NEED_NEXT_FETCH( sOuterCacheBlockInfo ) )
            {
                /**
                 * EOF 체크
                 */

                STL_TRY_THROW( sOuterCacheBlockInfo->mEOF == STL_FALSE,
                               RAMP_NO_DATA );

                /**
                 * Outer Cache Block에 새로운 데이터를 Fetch
                 */

                QLL_OPT_CHECK_TIME( sBeginExceptTime );

                sEOF = STL_FALSE;
                QLNX_FETCH_NODE( aFetchNodeInfo,
                                 sOuterCacheBlockInfo->mNodeIdx,
                                 0, /* Start Idx */
                                 0,
                                 QLL_FETCH_COUNT_MAX,
                                 &sUsedBlockCnt,
                                 &sEOF,
                                 aEnv );
                
                QLL_OPT_CHECK_TIME( sEndExceptTime );
                QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                if( sUsedBlockCnt == 0 )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                    QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                    STL_THROW( RAMP_NO_DATA );
                }
                else if( sEOF == STL_TRUE )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                }
                else
                {
                    /* Do Nothing */
                }

                sOuterCacheBlockInfo->mCurrBlockIdx = 0;
                sOuterCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;
            }
            else
            {
                /**
                 * Cache에서 읽을 Block Idx를 설정
                 */

                sOuterCacheBlockInfo->mCurrBlockIdx++;
            }


            /**
             * Inner Table 읽기
             */

            if( sInnerCacheBlockInfo->mNeedScan == STL_FALSE )
            {
                sInnerCacheBlockInfo->mCurrBlockIdx = 0;
            }
            else
            {
                /**
                 * Left Outer Column에 대한 Data Value 재설정
                 */

                sOuterColumnBlockItem = sExeNestedLoopsJoin->mLeftOuterColumnBlockList.mHead;
                QLNX_SET_OUTER_COLUMN_BLOCK_LIST( sOuterColumnBlockItem, aEnv );

                /**
                 * Inner Table을 다시 초기화
                 */

                QLNX_EXECUTE_NODE( sLocalFetchInfo.mTransID,
                                   sLocalFetchInfo.mStmt,
                                   sLocalFetchInfo.mDBCStmt,
                                   sLocalFetchInfo.mSQLStmt,
                                   sLocalFetchInfo.mDataArea->mExecNodeList[sInnerCacheBlockInfo->mNodeIdx].mOptNode,
                                   sLocalFetchInfo.mDataArea,
                                   aEnv );

                QLNX_RESET_CACHE_BLOCK_INFO( sInnerCacheBlockInfo );


                /**
                 * Cache에 새로운 데이터를 Fetch
                 */

                QLL_OPT_CHECK_TIME( sBeginExceptTime );

                sEOF = STL_FALSE;

                QLNX_FETCH_NODE( aFetchNodeInfo,
                                 sInnerCacheBlockInfo->mNodeIdx,
                                 0, /* Start Idx */
                                 0,
                                 QLL_FETCH_COUNT_MAX,
                                 &sUsedBlockCnt,
                                 &sEOF,
                                 aEnv );
                
                QLL_OPT_CHECK_TIME( sEndExceptTime );
                QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                if( sUsedBlockCnt == 0 )
                {
                    /**
                     * Inner Table를 새로 Fetch하였는데 결과 개수가 0개인 경우
                     * ------------------------------
                     *  Join 결과가 없다.
                     */
                    QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                    if( sExeNestedLoopsJoin->mLeftOuterColumnBlockList.mCount == 0 )
                    {
                        sInnerCacheBlockInfo->mNeedScan = STL_FALSE;
                        QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                        STL_THROW( RAMP_NO_DATA );
                    }
                    else
                    {
                        /**
                         * Outer Column이 존재하는 경우 다시 scan해야 한다.
                         */
                        sInnerCacheBlockInfo->mNeedScan = STL_TRUE;
                        STL_THROW( RAMP_READ_NEXT_ONE_ROW );
                    }
                }
                else if( sEOF == STL_TRUE )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                    if( sExeNestedLoopsJoin->mLeftOuterColumnBlockList.mCount == 0 )
                    {
                        sInnerCacheBlockInfo->mNeedScan = STL_FALSE;
                    }
                    else
                    {
                        /**
                         * Outer Column이 존재하는 경우 다시 scan해야 한다.
                         */
                        sInnerCacheBlockInfo->mNeedScan = STL_TRUE;
                    }
                }
                else
                {
                    /* Do Nothing */
                }

                sInnerCacheBlockInfo->mCurrBlockIdx = 0;
                sInnerCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;
            }
        }
        else
        {
            /**
             * Inner Table을 아직 다 읽지 않은 경우
             *
             * Outer Table에서 현재 읽은 Block에 대해서
             * Inner Table에서의 나머지 Block들과 Nested Loops Join
             */

            /**
             * Inner Table의 Cache Block에 아직 읽을 데이터가 남았는지 체크
             */

            if( QLNX_NEED_NEXT_FETCH( sInnerCacheBlockInfo ) )
            {
                /**
                 * EOF 체크
                 */

                STL_TRY_THROW( sInnerCacheBlockInfo->mEOF == STL_FALSE,
                               RAMP_READ_NEXT_ONE_ROW );

                /**
                 * Left Outer Column에 대한 Data Value 재설정
                 */

                sOuterColumnBlockItem = sExeNestedLoopsJoin->mLeftOuterColumnBlockList.mHead;
                QLNX_SET_OUTER_COLUMN_BLOCK_LIST( sOuterColumnBlockItem, aEnv );

                /**
                 * Cache에 새로운 데이터를 Fetch
                 */

                QLL_OPT_CHECK_TIME( sBeginExceptTime );

                sEOF = STL_FALSE;
                QLNX_FETCH_NODE( aFetchNodeInfo,
                                 sInnerCacheBlockInfo->mNodeIdx,
                                 0, /* Start Idx */
                                 0,
                                 QLL_FETCH_COUNT_MAX,
                                 &sUsedBlockCnt,
                                 &sEOF,
                                 aEnv );

                QLL_OPT_CHECK_TIME( sEndExceptTime );
                QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                if( sUsedBlockCnt == 0 )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                    STL_THROW( RAMP_READ_NEXT_ONE_ROW );
                }
                else if( sEOF == STL_TRUE )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                }
                else
                {
                    /* Do Nothing */
                }
                
                sInnerCacheBlockInfo->mCurrBlockIdx = 0;
                sInnerCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;
            }
            else
            {
                /**
                 * Cache에서 읽을 Block Idx를 설정
                 */

                sInnerCacheBlockInfo->mCurrBlockIdx++;
            }

        }


        /**
         * Left Outer Column에 대한 Data Value 재설정
         */

        sOuterColumnBlockItem = sExeNestedLoopsJoin->mLeftOuterColumnBlockList.mHead;
        QLNX_SET_OUTER_COLUMN_BLOCK_LIST( sOuterColumnBlockItem, aEnv );

        /**
         * Right Outer Column에 대한 Data Value 재설정
         */

        sOuterColumnBlockItem = sExeNestedLoopsJoin->mRightOuterColumnBlockList.mHead;
        QLNX_SET_OUTER_COLUMN_BLOCK_LIST( sOuterColumnBlockItem, aEnv );


        /**
         * Join Condition Logical Filter 평가
         */

        if( sExeNestedLoopsJoin->mHasJoinCondition == STL_TRUE )
        {
            sJoinExprEvalInfo->mBlockIdx = sExeNestedLoopsJoin->mCurrBlockIdx;
            STL_TRY( knlEvaluateOneExpression( sJoinExprEvalInfo,
                                               KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            if( !DTL_BOOLEAN_IS_TRUE( KNL_GET_BLOCK_DATA_VALUE(
                                          sJoinExprEvalInfo->mResultBlock,
                                          sExeNestedLoopsJoin->mCurrBlockIdx ) ) )
            {
                STL_THROW( RAMP_READ_NEXT_ONE_ROW );
            }
        }

        
        /**
         * Outer Table의 데이터와 Inner Table의 데이터에 대하여
         * Nested Loops Join 수행
         */

        if( sExeNestedLoopsJoin->mJoinedColBlock == NULL )
        {
            /**
             * Join된 결과로 읽어야할 column이 존재하지 않는 경우
             * ex) select 1 from t1, t2;
             */

            /* Do Nothing */
        }
        else
        {
            /**
             * Result Block에 데이터 복사
             */

            QLNX_SET_VALUE_BLOCK( &(sExeNestedLoopsJoin->mValueBlockList),
                                  aEnv );


            /**
             * Where Filter Outer Column에 대한 Data Value 재설정
             */

            sOuterColumnBlockItem = sExeNestedLoopsJoin->mWhereOuterColumnBlockList.mHead;
            QLNX_SET_OUTER_COLUMN_BLOCK_LIST( sOuterColumnBlockItem, aEnv );


            /**
             * Where Condition Logical Filter 평가
             */

            if( sExeNestedLoopsJoin->mHasWhereCondition == STL_TRUE )
            {
                sWhereExprEvalInfo->mBlockIdx = sExeNestedLoopsJoin->mCurrBlockIdx;
                STL_TRY( knlEvaluateOneExpression( sWhereExprEvalInfo,
                                                   KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                if( !DTL_BOOLEAN_IS_TRUE( KNL_GET_BLOCK_DATA_VALUE(
                                              sWhereExprEvalInfo->mResultBlock,
                                              sExeNestedLoopsJoin->mCurrBlockIdx ) ) )
                {
                    STL_THROW( RAMP_READ_NEXT_ONE_ROW );
                }
            }
        }


        /**
         * Row Block들을 Result Block에 복사
         */

        sRowBlockItem = sExeNestedLoopsJoin->mRowBlockList.mHead;
        while( sRowBlockItem != NULL )
        {
            /* 원본 블럭의 idx 가져오기 */
            sTempBlockIdx =
                (sRowBlockItem->mIsLeftTableRowBlock == STL_TRUE
                 ? sOuterCacheBlockInfo->mCurrBlockIdx
                 : sInnerCacheBlockInfo->mCurrBlockIdx);

            /* Scn Value 복사 */
            SML_SET_SCN_VALUE(
                sRowBlockItem->mRowBlock,
                sExeNestedLoopsJoin->mCurrBlockIdx,
                SML_GET_SCN_VALUE(
                    sRowBlockItem->mOrgRowBlock,
                    sTempBlockIdx ) );

            /* Rid Value 복사 */
            SML_SET_RID_VALUE(
                sRowBlockItem->mRowBlock,
                sExeNestedLoopsJoin->mCurrBlockIdx,
                SML_GET_RID_VALUE(
                    sRowBlockItem->mOrgRowBlock,
                    sTempBlockIdx ) );

            sRowBlockItem = sRowBlockItem->mNext;
        }


        /**
         * Skip Count가 남았으면 Skip 수행
         */

        if( sRemainSkipCnt > 0 )
        {
            sRemainSkipCnt--;

            STL_THROW( RAMP_READ_NEXT_ONE_ROW );
        }


        /**
         * Fetch Count 체크
         */

        sExeNestedLoopsJoin->mCurrBlockIdx++;
        sRemainFetchCnt --;

        if( sRemainFetchCnt == 0 )
        {
            break;
        }
        else
        {
            /**
             * Right Table의 Cache Block을 다 읽었지만
             * 아직 Fetch Count가 남은 경우
             */

            /* Do Nothing */
        }
    }

    STL_RAMP( RAMP_NO_DATA );

    if( QLNX_IS_END_OF_TABLE_FETCH( sInnerCacheBlockInfo ) &&
        QLNX_IS_END_OF_TABLE_FETCH( sOuterCacheBlockInfo ) )
    {
        sEOF = STL_TRUE;
        sExeNestedLoopsJoin->mIsEOF = STL_TRUE;
    }
    else
    {
        sEOF = STL_FALSE;
        sExeNestedLoopsJoin->mIsEOF = STL_FALSE;
    }


    /**
     * OUTPUT 설정
     */

    *aEOF = sEOF;
    *aUsedBlockCnt = sExeNestedLoopsJoin->mCurrBlockIdx;

    /* Parent의 Fetch Node Info 설정 */
    aFetchNodeInfo->mFetchCnt = sRemainFetchCnt;
    aFetchNodeInfo->mSkipCnt  = sRemainSkipCnt;

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT(
        sExeNestedLoopsJoin->mJoinedColBlock,
        0,
        sExeNestedLoopsJoin->mCurrBlockIdx );

    sRowBlockItem = sExeNestedLoopsJoin->mRowBlockList.mHead;
    while( sRowBlockItem != NULL )
    {
        SML_SET_USED_BLOCK_SIZE(
            sRowBlockItem->mRowBlock,
            sExeNestedLoopsJoin->mCurrBlockIdx );

        sRowBlockItem = sRowBlockItem->mNext;
    }

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeNestedLoopsJoin->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeNestedLoopsJoin->mCommonInfo.mFetchCallCount, 1 );
    QLL_OPT_ADD_RECORD_STAT_INFO( sExeNestedLoopsJoin->mCommonInfo.mFetchRecordStat,
                                  sExeNestedLoopsJoin->mCommonInfo.mResultColBlock,
                                  *aUsedBlockCnt,
                                  aEnv );

    sExeNestedLoopsJoin->mCommonInfo.mFetchRowCnt += *aUsedBlockCnt;
 
    /* Outer Column의 DstDataValue 정보 복구 */
    sOuterColumnBlockItem = sExeNestedLoopsJoin->mLeftOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );
    sOuterColumnBlockItem = sExeNestedLoopsJoin->mRightOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );
    sOuterColumnBlockItem = sExeNestedLoopsJoin->mWhereOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );


    return STL_SUCCESS;

    STL_FINISH;

    /* Outer Column의 DstDataValue 정보 복구 */
    sOuterColumnBlockItem = sExeNestedLoopsJoin->mLeftOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );
    sOuterColumnBlockItem = sExeNestedLoopsJoin->mRightOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );
    sOuterColumnBlockItem = sExeNestedLoopsJoin->mWhereOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeNestedLoopsJoin->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeNestedLoopsJoin->mCommonInfo.mFetchCallCount, 1 );

    return STL_FAILURE;
}


/**
 * @brief Outer Join에 대한 NESTED LOOPS JOIN node를 Fetch한다.
 * @param[in]      aFetchNodeInfo  Fetch Node Info
 * @param[out]     aUsedBlockCnt   Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF            End Of Fetch 여부
 * @param[in]      aEnv            Environment
 *
 * @remark Fetch 수행시 호출된다.
 */
stlStatus qlnxFetchNestedLoopsJoin_Outer( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                          stlInt64              * aUsedBlockCnt,
                                          stlBool               * aEOF,
                                          qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode               = NULL;
    qlnoNestedLoopsJoin   * sOptNestedLoopsJoin     = NULL;
    qlnxNestedLoopsJoin   * sExeNestedLoopsJoin     = NULL;

    stlBool                 sEOF                    = STL_FALSE;
    stlBool                 sTmpEOF                 = STL_FALSE;

    stlInt64                sUsedBlockCnt           = 0;
    stlInt64                sTmpUsedBlockCnt        = 0;
    stlInt64                sRemainSkipCnt          = 0;
    stlInt64                sRemainFetchCnt         = 0;

    qlnxCacheBlockInfo    * sInnerCacheBlockInfo    = NULL;
    qlnxCacheBlockInfo    * sOuterCacheBlockInfo    = NULL;

    knlExprEvalInfo       * sWhereExprEvalInfo      = NULL;
    knlExprEvalInfo       * sJoinExprEvalInfo       = NULL;

    qlnxValueBlockItem          * sValueBlockItem       = NULL;
    qlnxOuterColumnBlockItem    * sOuterColumnBlockItem = NULL;
    qlnxOuterColumnBlockList      sTmpOuterColumnBlockList;

    stlBool                 sHasMatchedRecord       = STL_FALSE;

    stlInt16                sTmpNodeIdx             = 0;
    stlInt32                i;

    qlnxFetchNodeInfo       sLocalFetchInfo;

    QLL_OPT_DECLARE( stlTime sBeginFetchTime );
    QLL_OPT_DECLARE( stlTime sEndFetchTime );
    QLL_OPT_DECLARE( stlTime sFetchTime );

    QLL_OPT_DECLARE( stlTime sBeginExceptTime );
    QLL_OPT_DECLARE( stlTime sEndExceptTime );
    QLL_OPT_DECLARE( stlTime sExceptTime );

    QLL_OPT_SET_VALUE( sFetchTime, 0 );
    QLL_OPT_SET_VALUE( sExceptTime, 0 );


    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aFetchNodeInfo->mOptNode->mType == QLL_PLAN_NODE_NESTED_LOOPS_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    QLL_OPT_CHECK_TIME( sBeginFetchTime );

    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aFetchNodeInfo->mDataArea,
                                        QLL_GET_OPT_NODE_IDX( aFetchNodeInfo->mOptNode ) );


    /**
     * NESTED LOOPS JOIN Optimization Node 획득
     */

    sOptNestedLoopsJoin = (qlnoNestedLoopsJoin *) aFetchNodeInfo->mOptNode->mOptNode;


    /**
     * NESTED LOOPS JOIN Execution Node 획득
     */

    sExeNestedLoopsJoin = sExecNode->mExecNode;

    
    /**
     * Node 수행 여부 확인 
     */

    STL_DASSERT( *aEOF == STL_FALSE );
    STL_DASSERT( aFetchNodeInfo->mFetchCnt > 0);


    /**
     * Used Block Count 초기화
     */
    
    sUsedBlockCnt = 0;


    /**
     * Current Block Idx 초기화
     */

    sExeNestedLoopsJoin->mCurrBlockIdx = 0;


    /**
     * Cache Block Info 정보 설정
     */

    sInnerCacheBlockInfo = &(sExeNestedLoopsJoin->mInnerCacheBlockInfo);
    sOuterCacheBlockInfo = &(sExeNestedLoopsJoin->mOuterCacheBlockInfo);


    /**
     * Evaluate Info 관련 정보 설정
     */

    sWhereExprEvalInfo  = &sExeNestedLoopsJoin->mWhereExprEvalInfo;
    sJoinExprEvalInfo   = &sExeNestedLoopsJoin->mJoinExprEvalInfo;

    sWhereExprEvalInfo->mBlockCount = 1;
    sJoinExprEvalInfo->mBlockCount = 1;
    

    /**
     * Local Fetch Info 초기화
     */
    
    QLNX_SET_LOCAL_FETCH_INFO( & sLocalFetchInfo, aFetchNodeInfo );


    /*****************************************
     * Node 수행 여부 확인
     ****************************************/
    
    if( ( sExeNestedLoopsJoin->mIsEOF == STL_TRUE )|| 
        ( sExeNestedLoopsJoin->mHasFalseFilter == STL_TRUE ) )
    {
        QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
        QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
        sExeNestedLoopsJoin->mIsFirstFetch = STL_FALSE;
        sExeNestedLoopsJoin->mIsEOF = STL_TRUE;
        sExeNestedLoopsJoin->mIsSwitched = STL_TRUE;

        STL_THROW( RAMP_NO_DATA );
    }
    else
    {
        /* Do Nothing */
    }


    /**
     * Fetch에 필요한 정보 설정
     */

    sRemainSkipCnt = aFetchNodeInfo->mSkipCnt;
    sRemainFetchCnt = ( aFetchNodeInfo->mFetchCnt < aFetchNodeInfo->mDataArea->mBlockAllocCnt
                        ? aFetchNodeInfo->mFetchCnt
                        : aFetchNodeInfo->mDataArea->mBlockAllocCnt );


    /*****************************************
     * Fetch 수행
     ****************************************/

    STL_RAMP( RETRY_FETCH_BY_SWITCHING_TABLE_NODE );

    sHasMatchedRecord = STL_TRUE;
    while( 1 )
    {
        /**
         * Block Read
         */

        STL_RAMP( RAMP_READ_NEXT_ONE_ROW );

        if( QLNX_IS_END_OF_TABLE_FETCH( sInnerCacheBlockInfo ) )
        {
            /**
             * Inner Table을 처음부터 읽어야 하는 경우
             */

            /**
             * Outer Table에서 하나의 레코드를 읽고
             * Inner Table은 Node 초기화를 해서 최종 개수만큼
             * Read하여 Nested Loops Join 연산을 수행한다.
             */

            /**
             * Outer Join에 대한 처리
             */

            if( sHasMatchedRecord == STL_FALSE )
            {
                /**
                 * Inner Table의 Record를 모두 NULL로 변경
                 */

                sValueBlockItem = sExeNestedLoopsJoin->mValueBlockList.mHead;
                while( sValueBlockItem != NULL )
                {
                    if( sValueBlockItem->mSrcBlockIdx ==
                        &(sExeNestedLoopsJoin->mInnerCacheBlockInfo.mCurrBlockIdx) )
                    {
                        DTL_SET_NULL(
                            KNL_GET_VALUE_BLOCK_DATA_VALUE(
                                sValueBlockItem->mDstValueBlock,
                                *(sValueBlockItem->mDstBlockIdx) ) );
                    }

                    sValueBlockItem = sValueBlockItem->mNext;
                }


                /**
                 * Row Block들을 Result Block에 복사
                 * -----------------------
                 *  Inner Table의 Row Block들은 NULL로 셋팅
                 */

                sHasMatchedRecord = STL_TRUE;

                STL_THROW( RAMP_MADE_OUTER_JOIN_RECORD );
            }

            STL_RAMP( RAMP_READ_NEXT_ONE_ROW_IN_FULL_OUTER );


            /**
             * Outer Table의 Cache Block에 아직 읽을 데이터가 남았는지 체크
             */

            if( QLNX_NEED_NEXT_FETCH( sOuterCacheBlockInfo ) )
            {
                /**
                 * EOF 체크
                 */

                STL_TRY_THROW( sOuterCacheBlockInfo->mEOF == STL_FALSE,
                               RAMP_NO_DATA );

                /**
                 * Outer Cache Block에 새로운 데이터를 Fetch
                 */

                QLL_OPT_CHECK_TIME( sBeginExceptTime );

                sEOF = STL_FALSE;
                QLNX_FETCH_NODE( aFetchNodeInfo,
                                 sOuterCacheBlockInfo->mNodeIdx,
                                 0, /* Start Idx */
                                 0,
                                 QLL_FETCH_COUNT_MAX,
                                 &sUsedBlockCnt,
                                 &sEOF,
                                 aEnv );

                QLL_OPT_CHECK_TIME( sEndExceptTime );
                QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                if( sUsedBlockCnt == 0 )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                    QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                    STL_THROW( RAMP_NO_DATA );
                }
                else if( sEOF == STL_TRUE )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                }
                else
                {
                    /* Do Nothing */
                }

                sOuterCacheBlockInfo->mCurrBlockIdx = 0;
                sOuterCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;
            }
            else
            {
                /**
                 * Cache에서 읽을 Block Idx를 설정
                 */

                sOuterCacheBlockInfo->mCurrBlockIdx++;
            }

            sHasMatchedRecord = STL_FALSE;


            /**
             * Inner Table 읽기
             */

            if( sInnerCacheBlockInfo->mNeedScan == STL_FALSE )
            {
                if( sInnerCacheBlockInfo->mCacheBlockCnt == 0 )
                {
                    /**
                     * Result Block에 데이터 복사
                     * ------------------------------
                     *  Outer Table의 데이터는 복사하고
                     *  Right Table의 데이터는 NULL로 설정
                     */

                    QLNX_SET_OUTER_VALUE_BLOCK( &(sExeNestedLoopsJoin->mValueBlockList),
                                                &(sExeNestedLoopsJoin->mOuterCacheBlockInfo.mCurrBlockIdx),
                                                aEnv );


                    /**
                     * Row Block들을 Result Block에 복사
                     * -----------------------
                     *  Inner Table의 Row Block들은 NULL로 셋팅
                     */

                    sHasMatchedRecord = STL_TRUE;

                    STL_THROW( RAMP_MADE_OUTER_JOIN_RECORD );
                }

                sInnerCacheBlockInfo->mCurrBlockIdx = 0;
            }
            else
            {
                /**
                 * Left Outer Column에 대한 Data Value 재설정
                 */

                sOuterColumnBlockItem = sExeNestedLoopsJoin->mLeftOuterColumnBlockList.mHead;
                QLNX_SET_OUTER_COLUMN_BLOCK_LIST( sOuterColumnBlockItem, aEnv );


                /**
                 * Inner Table을 다시 초기화
                 */

                QLNX_EXECUTE_NODE( sLocalFetchInfo.mTransID,
                                   sLocalFetchInfo.mStmt,
                                   sLocalFetchInfo.mDBCStmt,
                                   sLocalFetchInfo.mSQLStmt,
                                   sLocalFetchInfo.mDataArea->mExecNodeList[sInnerCacheBlockInfo->mNodeIdx].mOptNode,
                                   sLocalFetchInfo.mDataArea,
                                   aEnv );

                QLNX_RESET_CACHE_BLOCK_INFO( sInnerCacheBlockInfo );


                /**
                 * Cache에 새로운 데이터를 Fetch
                 */

                QLL_OPT_CHECK_TIME( sBeginExceptTime );

                sEOF = STL_FALSE;
                QLNX_FETCH_NODE( aFetchNodeInfo,
                                 sInnerCacheBlockInfo->mNodeIdx,
                                 0, /* Start Idx */
                                 0,
                                 QLL_FETCH_COUNT_MAX,
                                 &sUsedBlockCnt,
                                 &sEOF,
                                 aEnv );

                QLL_OPT_CHECK_TIME( sEndExceptTime );
                QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                if( sUsedBlockCnt == 0 )
                {
                    /**
                     * Outer Join은 Inner Table의 Record가 하나도 없으면
                     * Outer Table의 각 Record에 대하여 Inner Table Record가
                     * NULL로 조합되어 결과가 만들어져야 한다.
                     */

                    QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                    if( sExeNestedLoopsJoin->mLeftOuterColumnBlockList.mCount == 0 )
                    {
                        sInnerCacheBlockInfo->mNeedScan = STL_FALSE;
                    }
                    else
                    {
                        /**
                         * Outer Column이 존재하는 경우 다시 scan해야 한다.
                         */
                        sInnerCacheBlockInfo->mNeedScan = STL_TRUE;
                    }

                    /**
                     * Result Block에 데이터 복사
                     * ------------------------------
                     *  Outer Table의 데이터는 복사하고
                     *  Right Table의 데이터는 NULL로 설정
                     */

                    QLNX_SET_OUTER_VALUE_BLOCK( &(sExeNestedLoopsJoin->mValueBlockList),
                                                &(sExeNestedLoopsJoin->mOuterCacheBlockInfo.mCurrBlockIdx),
                                                aEnv );


                    /**
                     * Row Block들을 Result Block에 복사
                     * -----------------------
                     *  Inner Table의 Row Block들은 NULL로 셋팅
                     */

                    sHasMatchedRecord = STL_TRUE;

                    STL_THROW( RAMP_MADE_OUTER_JOIN_RECORD );
                }
                else 
                {
                    if( sEOF == STL_TRUE )
                    {
                        /**
                         * Outer Join은 Inner Table의 Record가 하나도 없으면
                         * Outer Table의 각 Record에 대하여 Inner Table Record가
                         * NULL로 조합되어 결과가 만들어져야 한다.
                         */

                        QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                        if( sExeNestedLoopsJoin->mLeftOuterColumnBlockList.mCount == 0 )
                        {
                            sInnerCacheBlockInfo->mNeedScan = STL_FALSE;
                        }
                        else
                        {
                            /**
                             * Outer Column이 존재하는 경우 다시 scan해야 한다.
                             */
                            sInnerCacheBlockInfo->mNeedScan = STL_TRUE;
                        }
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }
                
                sInnerCacheBlockInfo->mCurrBlockIdx = 0;
                sInnerCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;
            }
        }
        else
        {
            /**
             * Inner Table을 아직 다 읽지 않은 경우
             *
             * Outer Table에서 현재 읽은 Block에 대해서
             * Inner Table에서의 나머지 Block들과 Nested Loops Join
             */

            /**
             * Inner Table의 Cache Block에 아직 읽을 데이터가 남았는지 체크
             */

            if( QLNX_NEED_NEXT_FETCH( sInnerCacheBlockInfo ) )
            {
                /**
                 * EOF 체크
                 */

                STL_TRY_THROW( sInnerCacheBlockInfo->mEOF == STL_FALSE,
                               RAMP_READ_NEXT_ONE_ROW );

                /**
                 * Left Outer Column에 대한 Data Value 재설정
                 */

                sOuterColumnBlockItem = sExeNestedLoopsJoin->mLeftOuterColumnBlockList.mHead;
                QLNX_SET_OUTER_COLUMN_BLOCK_LIST( sOuterColumnBlockItem, aEnv );

                /**
                 * Cache에 새로운 데이터를 Fetch
                 */

                QLL_OPT_CHECK_TIME( sBeginExceptTime );

                sEOF = STL_FALSE;
                QLNX_FETCH_NODE( aFetchNodeInfo,
                                 sInnerCacheBlockInfo->mNodeIdx,
                                 0, /* Start Idx */
                                 0,
                                 1,
                                 &sUsedBlockCnt,
                                 &sEOF,
                                 aEnv );

                QLL_OPT_CHECK_TIME( sEndExceptTime );
                QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                if( sUsedBlockCnt == 0 )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                    STL_THROW( RAMP_READ_NEXT_ONE_ROW );
                }
                else if( sEOF == STL_TRUE )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                }
                else
                {
                    /* Do Nothing */
                }
                
                sInnerCacheBlockInfo->mCurrBlockIdx = 0;
                sInnerCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;
            }
            else
            {
                /**
                 * Cache에서 읽을 Block Idx를 설정
                 */

                sInnerCacheBlockInfo->mCurrBlockIdx++;
            }

        }


        /**
         * Left Outer Column에 대한 Data Value 재설정
         */

        sOuterColumnBlockItem = sExeNestedLoopsJoin->mLeftOuterColumnBlockList.mHead;
        QLNX_SET_OUTER_COLUMN_BLOCK_LIST( sOuterColumnBlockItem, aEnv );


        /**
         * Right Outer Column에 대한 Data Value 재설정
         */

        sOuterColumnBlockItem = sExeNestedLoopsJoin->mRightOuterColumnBlockList.mHead;
        QLNX_SET_OUTER_COLUMN_BLOCK_LIST( sOuterColumnBlockItem, aEnv );


        /**
         * Outer Table의 데이터와 Inner Table의 데이터에 대하여
         * Nested Loops Join 수행
         */

        if( sExeNestedLoopsJoin->mJoinedColBlock == NULL )
        {
            /**
             * Join Condition Logical Filter 평가
             */

            if( sExeNestedLoopsJoin->mHasJoinCondition == STL_TRUE )
            {
                sJoinExprEvalInfo->mBlockIdx = sExeNestedLoopsJoin->mCurrBlockIdx;
                STL_TRY( knlEvaluateOneExpression( sJoinExprEvalInfo,
                                                   KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                if( !DTL_BOOLEAN_IS_TRUE( KNL_GET_BLOCK_DATA_VALUE(
                                              sJoinExprEvalInfo->mResultBlock,
                                              sExeNestedLoopsJoin->mCurrBlockIdx ) ) )
                {
                    STL_THROW( RAMP_READ_NEXT_ONE_ROW );
                }
            }

            /**
             * Join된 결과로 읽어야할 column이 존재하지 않는 경우
             * ex) select 1 from t1, t2;
             */

            sHasMatchedRecord = STL_TRUE;


            /**
             * Full Outer Join에서 Inner Table과 Outer Table이 Switching된 경우
             * 조건을 만족하는 레코드가 있으면 해당 레코드는 버리고,
             * Inner Table에 대하여 더이상 scan 하지 않는다.
             */

            if( (sExeNestedLoopsJoin->mJoinType == QLV_JOIN_TYPE_FULL_OUTER) &&
                (sExeNestedLoopsJoin->mIsSwitched == STL_TRUE) )
            {
                QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                STL_THROW( RAMP_READ_NEXT_ONE_ROW_IN_FULL_OUTER );
            }
        }
        else
        {
            /**
             * Result Block에 데이터 복사
             */

            QLNX_SET_VALUE_BLOCK( &(sExeNestedLoopsJoin->mValueBlockList),
                                  aEnv );


            /**
             * Join Condition Logical Filter 평가
             */

            if( sExeNestedLoopsJoin->mHasJoinCondition == STL_TRUE )
            {
                sJoinExprEvalInfo->mBlockIdx = sExeNestedLoopsJoin->mCurrBlockIdx;
                STL_TRY( knlEvaluateOneExpression( sJoinExprEvalInfo,
                                                   KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                if( !DTL_BOOLEAN_IS_TRUE( KNL_GET_BLOCK_DATA_VALUE(
                                              sJoinExprEvalInfo->mResultBlock,
                                              sExeNestedLoopsJoin->mCurrBlockIdx ) ) )
                {
                    STL_THROW( RAMP_READ_NEXT_ONE_ROW );
                }
            }


            /**
             * Where Filter Outer Column에 대한 Data Value 재설정
             */

            sOuterColumnBlockItem = sExeNestedLoopsJoin->mWhereOuterColumnBlockList.mHead;
            QLNX_SET_OUTER_COLUMN_BLOCK_LIST( sOuterColumnBlockItem, aEnv );


            /**
             * Join Condition에 존재하는 SubQuery Filter 관련 처리
             */

            if( sOptNestedLoopsJoin->mStmtSubQueryNode != NULL )
            {
                QLNX_EXECUTE_NODE( sLocalFetchInfo.mTransID,
                                   sLocalFetchInfo.mStmt,
                                   sLocalFetchInfo.mDBCStmt,
                                   sLocalFetchInfo.mSQLStmt,
                                   sOptNestedLoopsJoin->mStmtSubQueryNode,
                                   sLocalFetchInfo.mDataArea,
                                   aEnv );

                sTmpEOF = STL_FALSE;
                QLNX_FETCH_NODE( aFetchNodeInfo,
                                 sOptNestedLoopsJoin->mStmtSubQueryNode->mIdx,
                                 0, /* Start Idx */
                                 0,
                                 QLL_FETCH_COUNT_MAX,
                                 &sTmpUsedBlockCnt,
                                 &sTmpEOF,
                                 aEnv );
            }

            if( sOptNestedLoopsJoin->mSubQueryAndFilterCnt > 0 )
            {
                for( i = 0; i < sOptNestedLoopsJoin->mSubQueryAndFilterCnt; i++ )
                {
                    QLNX_EXECUTE_NODE( sLocalFetchInfo.mTransID,
                                       sLocalFetchInfo.mStmt,
                                       sLocalFetchInfo.mDBCStmt,
                                       sLocalFetchInfo.mSQLStmt,
                                       sOptNestedLoopsJoin->mSubQueryAndFilterNodeArr[i],
                                       sLocalFetchInfo.mDataArea,
                                       aEnv );

                    sTmpEOF = STL_FALSE;
                    QLNX_FETCH_NODE( aFetchNodeInfo,
                                     sOptNestedLoopsJoin->mSubQueryAndFilterNodeArr[i]->mIdx,
                                     0, /* Start Idx */
                                     0,
                                     1,
                                     &sTmpUsedBlockCnt,
                                     &sTmpEOF,
                                     aEnv );

                    STL_DASSERT( sTmpUsedBlockCnt == 1 );

                    sExeNestedLoopsJoin->mSubQueryFilterExprEvalInfo[i]->mBlockIdx = 0;
                    sExeNestedLoopsJoin->mSubQueryFilterExprEvalInfo[i]->mBlockCount = 1;
                    STL_TRY( knlEvaluateOneExpression(
                                 sExeNestedLoopsJoin->mSubQueryFilterExprEvalInfo[i],
                                 KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    STL_TRY_THROW(
                        DTL_BOOLEAN_IS_TRUE(
                            KNL_GET_BLOCK_DATA_VALUE(
                                sExeNestedLoopsJoin->mSubQueryFilterExprEvalInfo[i]->mResultBlock,
                                0 ) ),
                        RAMP_READ_NEXT_ONE_ROW );
                }
            }

            sHasMatchedRecord = STL_TRUE;


            /**
             * Full Outer Join에서 Inner Table과 Outer Table이 Switching된 경우
             * 조건을 만족하는 레코드가 있으면 해당 레코드는 버리고,
             * Inner Table에 대하여 더이상 scan 하지 않는다.
             */

            if( (sExeNestedLoopsJoin->mJoinType == QLV_JOIN_TYPE_FULL_OUTER) &&
                (sExeNestedLoopsJoin->mIsSwitched == STL_TRUE) )
            {
                QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                STL_THROW( RAMP_READ_NEXT_ONE_ROW_IN_FULL_OUTER );
            }
        }


        /**
         * Row Block은 없다.
         */

        STL_DASSERT( sExeNestedLoopsJoin->mRowBlockList.mHead == NULL );

        STL_RAMP( RAMP_MADE_OUTER_JOIN_RECORD );


        /**
         * Where Filter Outer Column에 대한 Data Value 재설정
         */

        sOuterColumnBlockItem = sExeNestedLoopsJoin->mWhereOuterColumnBlockList.mHead;
        QLNX_SET_OUTER_COLUMN_BLOCK_LIST( sOuterColumnBlockItem, aEnv );


        /**
         * Where Condition Logical Filter 평가
         */

        if( sExeNestedLoopsJoin->mHasWhereCondition == STL_TRUE )
        {
            sWhereExprEvalInfo->mBlockIdx = sExeNestedLoopsJoin->mCurrBlockIdx;
            STL_TRY( knlEvaluateOneExpression( sWhereExprEvalInfo,
                                               KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            if( !DTL_BOOLEAN_IS_TRUE( KNL_GET_BLOCK_DATA_VALUE(
                                          sWhereExprEvalInfo->mResultBlock,
                                          sExeNestedLoopsJoin->mCurrBlockIdx ) ) )
            {
                sHasMatchedRecord = STL_TRUE;
                STL_THROW( RAMP_READ_NEXT_ONE_ROW );
            }
        }


        /**
         * Skip Count가 남았으면 Skip 수행
         */

        if( sRemainSkipCnt > 0 )
        {
            sRemainSkipCnt--;

            STL_THROW( RAMP_READ_NEXT_ONE_ROW );
        }


        /**
         * Fetch Count 체크
         */

        sExeNestedLoopsJoin->mCurrBlockIdx++;
        sRemainFetchCnt --;

        if( sRemainFetchCnt == 0 )
        {
            break;
        }
        else
        {
            /**
             * Right Table의 Cache Block을 다 읽었지만
             * 아직 Fetch Count가 남은 경우
             */

            /* Do Nothing */
        }
    }

    STL_RAMP( RAMP_NO_DATA );

    if( QLNX_IS_END_OF_TABLE_FETCH( sInnerCacheBlockInfo ) &&
        QLNX_IS_END_OF_TABLE_FETCH( sOuterCacheBlockInfo ) )
    {
        if( (sExeNestedLoopsJoin->mJoinType == QLV_JOIN_TYPE_FULL_OUTER) &&
            (sExeNestedLoopsJoin->mIsSwitched == STL_FALSE) )
        {
            sEOF = STL_FALSE;
            sExeNestedLoopsJoin->mIsEOF = STL_FALSE;


            /**
             * Inner와 Outer를 Switching
             * --------------------------
             *  Outer Table은 여기서 초기화 해둔다.
             */

            QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
            QLNX_RESET_CACHE_BLOCK_INFO( sOuterCacheBlockInfo );
            sTmpNodeIdx = sInnerCacheBlockInfo->mNodeIdx;
            sInnerCacheBlockInfo->mNodeIdx = sOuterCacheBlockInfo->mNodeIdx;
            sOuterCacheBlockInfo->mNodeIdx = sTmpNodeIdx;

            QLNX_EXECUTE_NODE( sLocalFetchInfo.mTransID,
                               sLocalFetchInfo.mStmt,
                               sLocalFetchInfo.mDBCStmt,
                               sLocalFetchInfo.mSQLStmt,
                               sLocalFetchInfo.mDataArea->mExecNodeList[sOuterCacheBlockInfo->mNodeIdx].mOptNode,
                               sLocalFetchInfo.mDataArea,
                               aEnv );

            /* 복사할 block idx를 바꿔준다. */
            sValueBlockItem = sExeNestedLoopsJoin->mValueBlockList.mHead;
            while( sValueBlockItem != NULL )
            {
                if( sValueBlockItem->mSrcBlockIdx ==
                    &(sOuterCacheBlockInfo->mCurrBlockIdx) )
                {
                    sValueBlockItem->mSrcBlockIdx =
                        &(sInnerCacheBlockInfo->mCurrBlockIdx);
                }
                else
                {
                    sValueBlockItem->mSrcBlockIdx =
                        &(sOuterCacheBlockInfo->mCurrBlockIdx);
                }
                sValueBlockItem = sValueBlockItem->mNext;
            }

            /* Left Outer Column과 Right Outer Column을 변경 */
            sTmpOuterColumnBlockList = sExeNestedLoopsJoin->mLeftOuterColumnBlockList;
            sExeNestedLoopsJoin->mLeftOuterColumnBlockList =
                sExeNestedLoopsJoin->mRightOuterColumnBlockList;
            sExeNestedLoopsJoin->mRightOuterColumnBlockList = sTmpOuterColumnBlockList;

            sOuterColumnBlockItem = sExeNestedLoopsJoin->mLeftOuterColumnBlockList.mHead;
            while( sOuterColumnBlockItem != NULL )
            {
                sOuterColumnBlockItem->mSrcBlockIdx = &(sOuterCacheBlockInfo->mCurrBlockIdx);
                sOuterColumnBlockItem = sOuterColumnBlockItem->mNext;
            }

            sOuterColumnBlockItem = sExeNestedLoopsJoin->mRightOuterColumnBlockList.mHead;
            while( sOuterColumnBlockItem != NULL )
            {
                sOuterColumnBlockItem->mSrcBlockIdx = &(sInnerCacheBlockInfo->mCurrBlockIdx);
                sOuterColumnBlockItem = sOuterColumnBlockItem->mNext;
            }

            sExeNestedLoopsJoin->mIsSwitched = STL_TRUE;

            if( sRemainFetchCnt > 0 )
            {
                STL_THROW( RETRY_FETCH_BY_SWITCHING_TABLE_NODE );
            }
        }
        else
        {
            sEOF = STL_TRUE;
            sExeNestedLoopsJoin->mIsEOF = STL_TRUE;
        }
    }
    else
    {
        sEOF = STL_FALSE;
        sExeNestedLoopsJoin->mIsEOF = STL_FALSE;
    }


    /**
     * OUTPUT 설정
     */

    *aEOF = sEOF;
    *aUsedBlockCnt = sExeNestedLoopsJoin->mCurrBlockIdx;

    /* Parent의 Fetch Node Info 설정 */
    aFetchNodeInfo->mFetchCnt = sRemainFetchCnt;
    aFetchNodeInfo->mSkipCnt  = sRemainSkipCnt;

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT(
        sExeNestedLoopsJoin->mJoinedColBlock,
        0,
        sExeNestedLoopsJoin->mCurrBlockIdx );

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeNestedLoopsJoin->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeNestedLoopsJoin->mCommonInfo.mFetchCallCount, 1 );
    QLL_OPT_ADD_RECORD_STAT_INFO( sExeNestedLoopsJoin->mCommonInfo.mFetchRecordStat,
                                  sExeNestedLoopsJoin->mCommonInfo.mResultColBlock,
                                  *aUsedBlockCnt,
                                  aEnv );

    sExeNestedLoopsJoin->mCommonInfo.mFetchRowCnt += *aUsedBlockCnt;

    /* Outer Column의 DstDataValue 정보 복구 */
    sOuterColumnBlockItem = sExeNestedLoopsJoin->mLeftOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );
    sOuterColumnBlockItem = sExeNestedLoopsJoin->mRightOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );
    sOuterColumnBlockItem = sExeNestedLoopsJoin->mWhereOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );


    return STL_SUCCESS;

    STL_FINISH;

    /* Outer Column의 DstDataValue 정보 복구 */
    sOuterColumnBlockItem = sExeNestedLoopsJoin->mLeftOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );
    sOuterColumnBlockItem = sExeNestedLoopsJoin->mRightOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );
    sOuterColumnBlockItem = sExeNestedLoopsJoin->mWhereOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeNestedLoopsJoin->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeNestedLoopsJoin->mCommonInfo.mFetchCallCount, 1 );

    return STL_FAILURE;
}


/**
 * @brief Semi Join에 대한 NESTED LOOPS JOIN node를 Fetch한다.
 * @param[in]      aFetchNodeInfo  Fetch Node Info
 * @param[out]     aUsedBlockCnt   Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF            End Of Fetch 여부
 * @param[in]      aEnv            Environment
 *
 * @remark Fetch 수행시 호출된다.
 */
stlStatus qlnxFetchNestedLoopsJoin_Semi( qlnxFetchNodeInfo  * aFetchNodeInfo,
                                         stlInt64           * aUsedBlockCnt,
                                         stlBool            * aEOF,
                                         qllEnv             * aEnv )
{
    qllExecutionNode      * sExecNode               = NULL;
    qlnxNestedLoopsJoin   * sExeNestedLoopsJoin     = NULL;

    stlBool                 sEOF                    = STL_FALSE;

    stlInt64                sUsedBlockCnt           = 0;
    stlInt64                sRemainSkipCnt          = 0;
    stlInt64                sRemainFetchCnt         = 0;

    qlnxCacheBlockInfo    * sInnerCacheBlockInfo    = NULL;
    qlnxCacheBlockInfo    * sOuterCacheBlockInfo    = NULL;

    knlExprEvalInfo       * sWhereExprEvalInfo      = NULL;
    knlExprEvalInfo       * sJoinExprEvalInfo       = NULL;

    qlnxValueBlockItem          * sValueBlockItem       = NULL;
    qlnxOuterColumnBlockItem    * sOuterColumnBlockItem = NULL;

    qlnxRowBlockItem      * sRowBlockItem           = NULL;
    stlInt64                sTempBlockIdx           = 0;

    qlnxFetchNodeInfo       sLocalFetchInfo;

    QLL_OPT_DECLARE( stlTime sBeginFetchTime );
    QLL_OPT_DECLARE( stlTime sEndFetchTime );
    QLL_OPT_DECLARE( stlTime sFetchTime );

    QLL_OPT_DECLARE( stlTime sBeginExceptTime );
    QLL_OPT_DECLARE( stlTime sEndExceptTime );
    QLL_OPT_DECLARE( stlTime sExceptTime );

    QLL_OPT_SET_VALUE( sFetchTime, 0 );
    QLL_OPT_SET_VALUE( sExceptTime, 0 );


    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aFetchNodeInfo->mOptNode->mType == QLL_PLAN_NODE_NESTED_LOOPS_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    QLL_OPT_CHECK_TIME( sBeginFetchTime );

    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aFetchNodeInfo->mDataArea,
                                        QLL_GET_OPT_NODE_IDX( aFetchNodeInfo->mOptNode ) );


    /**
     * NESTED LOOPS JOIN Execution Node 획득
     */

    sExeNestedLoopsJoin = sExecNode->mExecNode;


    /**
     * Node 수행 여부 확인 
     */

    STL_DASSERT( *aEOF == STL_FALSE );
    STL_DASSERT( aFetchNodeInfo->mFetchCnt > 0);


    /**
     * Used Block Count 초기화
     */

    sUsedBlockCnt = 0;


    /**
     * Current Block Idx 초기화
     */

    sExeNestedLoopsJoin->mCurrBlockIdx = 0;


    /**
     * Cache Block Info 정보 설정
     */

    sInnerCacheBlockInfo = &(sExeNestedLoopsJoin->mInnerCacheBlockInfo);
    sOuterCacheBlockInfo = &(sExeNestedLoopsJoin->mOuterCacheBlockInfo);


    /**
     * Evaluate Info 관련 정보 설정
     */

    sWhereExprEvalInfo  = &sExeNestedLoopsJoin->mWhereExprEvalInfo;
    sJoinExprEvalInfo   = &sExeNestedLoopsJoin->mJoinExprEvalInfo;

    sWhereExprEvalInfo->mBlockCount = 1;
    sJoinExprEvalInfo->mBlockCount = 1;


    /**
     * Local Fetch Info 초기화
     */
    
    QLNX_SET_LOCAL_FETCH_INFO( & sLocalFetchInfo, aFetchNodeInfo );


    /*****************************************
     * Node 수행 여부 확인
     ****************************************/
    
    /* 이전 연산에 의헤 sExeNestedLoopsJoin->mIsEOF가 true로 설정된 경우, 이 함수를 다시 들어올 수 없다.
       sExeNestedLoopsJoin->mIsEOF가 true로 설정된 경우 반환되는 EOF 정보도 true로 설정되어 있다. */
    STL_DASSERT( sExeNestedLoopsJoin->mIsEOF == STL_FALSE );

    /* Semi Join 은 constant filter가 없기 때문에 false filter를 체크하지 못한다. */
    STL_DASSERT( sExeNestedLoopsJoin->mHasFalseFilter == STL_FALSE );


    /**
     * Fetch에 필요한 정보 설정
     */

    sRemainSkipCnt = aFetchNodeInfo->mSkipCnt;
    sRemainFetchCnt = ( aFetchNodeInfo->mFetchCnt < aFetchNodeInfo->mDataArea->mBlockAllocCnt
                        ? aFetchNodeInfo->mFetchCnt
                        : aFetchNodeInfo->mDataArea->mBlockAllocCnt );


    /*****************************************
     * Fetch 수행
     ****************************************/

    while( 1 )
    {
        STL_RAMP( RAMP_READ_NEXT_ONE_ROW );


        /**
         * Outer Table의 Cache Block에 아직 읽을 데이터가 남았는지 체크
         */

        if( QLNX_NEED_NEXT_FETCH( sOuterCacheBlockInfo ) )
        {
            /* EOF 체크 */
            STL_TRY_THROW( sOuterCacheBlockInfo->mEOF == STL_FALSE,
                           RAMP_NO_DATA );

            /* Outer Cache Block에 새로운 데이터를 Fetch */
            QLL_OPT_CHECK_TIME( sBeginExceptTime );

            sEOF = STL_FALSE;
            QLNX_FETCH_NODE( aFetchNodeInfo,
                             sOuterCacheBlockInfo->mNodeIdx,
                             0, /* Start Idx */
                             0,
                             QLL_FETCH_COUNT_MAX,
                             &sUsedBlockCnt,
                             &sEOF,
                             aEnv );

            QLL_OPT_CHECK_TIME( sEndExceptTime );
            QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

            if( sUsedBlockCnt == 0 )
            {
                QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                STL_THROW( RAMP_NO_DATA );
            }
            else if( sEOF == STL_TRUE )
            {
                QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
            }
            else
            {
                /* Do Nothing */
            }

            sOuterCacheBlockInfo->mCurrBlockIdx = 0;
            sOuterCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;
        }
        else
        {
            /* Cache에서 읽을 Block Idx를 설정 */
            sOuterCacheBlockInfo->mCurrBlockIdx++;
        }


        /**
         * Left Outer Column에 대한 Data Value 재설정
         */

        sOuterColumnBlockItem = sExeNestedLoopsJoin->mLeftOuterColumnBlockList.mHead;
        QLNX_SET_OUTER_COLUMN_BLOCK_LIST( sOuterColumnBlockItem, aEnv );


        /* Inner Table을 다시 초기화 */
        QLNX_EXECUTE_NODE( sLocalFetchInfo.mTransID,
                           sLocalFetchInfo.mStmt,
                           sLocalFetchInfo.mDBCStmt,
                           sLocalFetchInfo.mSQLStmt,
                           sLocalFetchInfo.mDataArea->mExecNodeList[sInnerCacheBlockInfo->mNodeIdx].mOptNode,
                           sLocalFetchInfo.mDataArea,
                           aEnv );

        QLNX_RESET_CACHE_BLOCK_INFO( sInnerCacheBlockInfo );


        /* Inner Cache에 새로운 데이터를 Fetch */
        QLL_OPT_CHECK_TIME( sBeginExceptTime );

        sEOF = STL_FALSE;
        QLNX_FETCH_NODE( aFetchNodeInfo,
                         sInnerCacheBlockInfo->mNodeIdx,
                         0, /* Start Idx */
                         0,
                         1,
                         &sUsedBlockCnt,
                         &sEOF,
                         aEnv );

        QLL_OPT_CHECK_TIME( sEndExceptTime );
        QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );


        if( sUsedBlockCnt == 0 )
        {
            /**
             * Inner Table를 새로 Fetch하였는데 결과 개수가 0개인 경우
             * ------------------------------
             *  Join 결과가 없다.
             */
            QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
            if( sExeNestedLoopsJoin->mLeftOuterColumnBlockList.mCount == 0 )
            {
                sInnerCacheBlockInfo->mNeedScan = STL_FALSE;
                QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                STL_THROW( RAMP_NO_DATA );
            }
            else
            {
                /**
                 * Outer Column이 존재하는 경우 다시 scan해야 한다.
                 */
                sInnerCacheBlockInfo->mNeedScan = STL_TRUE;
                STL_THROW( RAMP_READ_NEXT_ONE_ROW );
            }
        }
        else
        {
            /* Do Nothing */
        }

        sInnerCacheBlockInfo->mCurrBlockIdx = 0;
        sInnerCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;


        /**
         * Left Outer Column에 대한 Data Value 재설정
         */

        sOuterColumnBlockItem = sExeNestedLoopsJoin->mLeftOuterColumnBlockList.mHead;
        QLNX_SET_OUTER_COLUMN_BLOCK_LIST( sOuterColumnBlockItem, aEnv );


        /**
         * Right Outer Column에 대한 Data Value 재설정
         */

        sOuterColumnBlockItem = sExeNestedLoopsJoin->mRightOuterColumnBlockList.mHead;
        QLNX_SET_OUTER_COLUMN_BLOCK_LIST( sOuterColumnBlockItem, aEnv );


        /**
         * Join Condition Logical Filter 평가
         */

        if( sExeNestedLoopsJoin->mHasJoinCondition == STL_TRUE )
        {
            /* Coverage : 재현하기 어려움 */
            sJoinExprEvalInfo->mBlockIdx = sExeNestedLoopsJoin->mCurrBlockIdx;
            STL_TRY( knlEvaluateOneExpression( sJoinExprEvalInfo,
                                               KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            if( !DTL_BOOLEAN_IS_TRUE( KNL_GET_BLOCK_DATA_VALUE(
                                          sJoinExprEvalInfo->mResultBlock,
                                          sExeNestedLoopsJoin->mCurrBlockIdx ) ) )
            {
                STL_THROW( RAMP_READ_NEXT_ONE_ROW );
            }
        }

      
        /**
         * Outer Table의 데이터와 Inner Table의 데이터에 대하여
         * Nested Loops Join 수행
         */

        if( sExeNestedLoopsJoin->mJoinedColBlock == NULL )
        {
            /**
             * Join된 결과로 읽어야할 column이 존재하지 않는 경우
             * ex) select 1 from t1, t2;
             */

            /* Do Nothing */
        }
        else
        {
            /**
             * Result Block에 데이터 복사
             */

            sValueBlockItem = sExeNestedLoopsJoin->mValueBlockList.mHead;
            while( sValueBlockItem != NULL )
            {
                STL_TRY( knlCopyDataValue(
                             KNL_GET_VALUE_BLOCK_DATA_VALUE(
                                 sValueBlockItem->mSrcValueBlock,
                                 *(sValueBlockItem->mSrcBlockIdx) ),
                             KNL_GET_VALUE_BLOCK_DATA_VALUE(
                                 sValueBlockItem->mDstValueBlock,
                                 *(sValueBlockItem->mDstBlockIdx) ),
                             KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                sValueBlockItem = sValueBlockItem->mNext;
            }
        }


        /**
         * Where Filter Outer Column에 대한 Data Value 재설정
         */

        sOuterColumnBlockItem = sExeNestedLoopsJoin->mWhereOuterColumnBlockList.mHead;
        QLNX_SET_OUTER_COLUMN_BLOCK_LIST( sOuterColumnBlockItem, aEnv );


        /**
         * Where Condition Logical Filter 평가
         */

        if( sExeNestedLoopsJoin->mHasWhereCondition == STL_TRUE )
        {
            sWhereExprEvalInfo->mBlockIdx = sExeNestedLoopsJoin->mCurrBlockIdx;
            STL_TRY( knlEvaluateOneExpression( sWhereExprEvalInfo,
                                               KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            if( !DTL_BOOLEAN_IS_TRUE( KNL_GET_BLOCK_DATA_VALUE(
                                          sWhereExprEvalInfo->mResultBlock,
                                          sExeNestedLoopsJoin->mCurrBlockIdx ) ) )
            {
                STL_THROW( RAMP_READ_NEXT_ONE_ROW );
            }
        }


        /**
         * Row Block들을 Result Block에 복사
         */

        sRowBlockItem = sExeNestedLoopsJoin->mRowBlockList.mHead;
        while( sRowBlockItem != NULL )
        {
            /* 원본 블럭의 idx 가져오기 */
            sTempBlockIdx =
                (sRowBlockItem->mIsLeftTableRowBlock == STL_TRUE
                 ? sOuterCacheBlockInfo->mCurrBlockIdx
                 : sInnerCacheBlockInfo->mCurrBlockIdx);

            /* Scn Value 복사 */
            SML_SET_SCN_VALUE(
                sRowBlockItem->mRowBlock,
                sExeNestedLoopsJoin->mCurrBlockIdx,
                SML_GET_SCN_VALUE(
                    sRowBlockItem->mOrgRowBlock,
                    sTempBlockIdx ) );

            /* Rid Value 복사 */
            SML_SET_RID_VALUE(
                sRowBlockItem->mRowBlock,
                sExeNestedLoopsJoin->mCurrBlockIdx,
                SML_GET_RID_VALUE(
                    sRowBlockItem->mOrgRowBlock,
                    sTempBlockIdx ) );

            sRowBlockItem = sRowBlockItem->mNext;
        }


        /**
         * Skip Count가 남았으면 Skip 수행
         */

        if( sRemainSkipCnt > 0 )
        {
            sRemainSkipCnt--;

            STL_THROW( RAMP_READ_NEXT_ONE_ROW );
        }


        /**
         * Fetch Count 체크
         */

        sExeNestedLoopsJoin->mCurrBlockIdx++;
        sRemainFetchCnt --;

        if( sRemainFetchCnt == 0 )
        {
            break;
        }
        else
        {
            /**
             * Right Table의 Cache Block을 다 읽었지만
             * 아직 Fetch Count가 남은 경우
             */

            /* Do Nothing */
        }
    }

    STL_RAMP( RAMP_NO_DATA );

    if( QLNX_IS_END_OF_TABLE_FETCH( sOuterCacheBlockInfo ) )
    {
        sEOF = STL_TRUE;
        sExeNestedLoopsJoin->mIsEOF = STL_TRUE;
    }
    else
    {
        sEOF = STL_FALSE;
        sExeNestedLoopsJoin->mIsEOF = STL_FALSE;
    }


    /**
     * OUTPUT 설정
     */

    *aEOF = sEOF;
    *aUsedBlockCnt = sExeNestedLoopsJoin->mCurrBlockIdx;

    /* Parent의 Fetch Node Info 설정 */
    aFetchNodeInfo->mFetchCnt = sRemainFetchCnt;
    aFetchNodeInfo->mSkipCnt  = sRemainSkipCnt;

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT(
        sExeNestedLoopsJoin->mJoinedColBlock,
        0,
        sExeNestedLoopsJoin->mCurrBlockIdx );

    sRowBlockItem = sExeNestedLoopsJoin->mRowBlockList.mHead;
    while( sRowBlockItem != NULL )
    {
        SML_SET_USED_BLOCK_SIZE(
            sRowBlockItem->mRowBlock,
            sExeNestedLoopsJoin->mCurrBlockIdx );

        sRowBlockItem = sRowBlockItem->mNext;
    }

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeNestedLoopsJoin->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeNestedLoopsJoin->mCommonInfo.mFetchCallCount, 1 );
    QLL_OPT_ADD_RECORD_STAT_INFO( sExeNestedLoopsJoin->mCommonInfo.mFetchRecordStat,
                                  sExeNestedLoopsJoin->mCommonInfo.mResultColBlock,
                                  *aUsedBlockCnt,
                                  aEnv );

    sExeNestedLoopsJoin->mCommonInfo.mFetchRowCnt += *aUsedBlockCnt;
 
    /* Outer Column의 DstDataValue 정보 복구 */
    sOuterColumnBlockItem = sExeNestedLoopsJoin->mLeftOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );
    sOuterColumnBlockItem = sExeNestedLoopsJoin->mRightOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );
    sOuterColumnBlockItem = sExeNestedLoopsJoin->mWhereOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );


    return STL_SUCCESS;

    STL_FINISH;

    /* Outer Column의 DstDataValue 정보 복구 */
    sOuterColumnBlockItem = sExeNestedLoopsJoin->mLeftOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );
    sOuterColumnBlockItem = sExeNestedLoopsJoin->mRightOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );
    sOuterColumnBlockItem = sExeNestedLoopsJoin->mWhereOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeNestedLoopsJoin->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeNestedLoopsJoin->mCommonInfo.mFetchCallCount, 1 );

    return STL_FAILURE;
}


/**
 * @brief Anti Semi Join에 대한 NESTED LOOPS JOIN node를 Fetch한다.
 * @param[in]      aFetchNodeInfo  Fetch Node Info
 * @param[out]     aUsedBlockCnt   Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF            End Of Fetch 여부
 * @param[in]      aEnv            Environment
 *
 * @remark Fetch 수행시 호출된다.
 */
stlStatus qlnxFetchNestedLoopsJoin_AntiSemi( qlnxFetchNodeInfo  * aFetchNodeInfo,
                                             stlInt64           * aUsedBlockCnt,
                                             stlBool            * aEOF,
                                             qllEnv             * aEnv )
{
    qllExecutionNode      * sExecNode               = NULL;
    qlnxNestedLoopsJoin   * sExeNestedLoopsJoin     = NULL;

    stlBool                 sEOF                    = STL_FALSE;

    stlInt64                sUsedBlockCnt           = 0;
    stlInt64                sRemainSkipCnt          = 0;
    stlInt64                sRemainFetchCnt         = 0;

    qlnxCacheBlockInfo    * sInnerCacheBlockInfo    = NULL;
    qlnxCacheBlockInfo    * sOuterCacheBlockInfo    = NULL;

    knlExprEvalInfo       * sWhereExprEvalInfo      = NULL;
    knlExprEvalInfo       * sJoinExprEvalInfo       = NULL;

    qlnxValueBlockItem          * sValueBlockItem       = NULL;
    qlnxOuterColumnBlockItem    * sOuterColumnBlockItem = NULL;

    qlnxRowBlockItem      * sRowBlockItem           = NULL;
    stlInt64                sTempBlockIdx           = 0;

    qlnxFetchNodeInfo       sLocalFetchInfo;

    QLL_OPT_DECLARE( stlTime sBeginFetchTime );
    QLL_OPT_DECLARE( stlTime sEndFetchTime );
    QLL_OPT_DECLARE( stlTime sFetchTime );

    QLL_OPT_DECLARE( stlTime sBeginExceptTime );
    QLL_OPT_DECLARE( stlTime sEndExceptTime );
    QLL_OPT_DECLARE( stlTime sExceptTime );

    QLL_OPT_SET_VALUE( sFetchTime, 0 );
    QLL_OPT_SET_VALUE( sExceptTime, 0 );


    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aFetchNodeInfo->mOptNode->mType == QLL_PLAN_NODE_NESTED_LOOPS_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    QLL_OPT_CHECK_TIME( sBeginFetchTime );

    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aFetchNodeInfo->mDataArea,
                                        QLL_GET_OPT_NODE_IDX( aFetchNodeInfo->mOptNode ) );


    /**
     * NESTED LOOPS JOIN Execution Node 획득
     */

    sExeNestedLoopsJoin = sExecNode->mExecNode;


    /**
     * Node 수행 여부 확인 
     */

    STL_DASSERT( *aEOF == STL_FALSE );
    STL_DASSERT( aFetchNodeInfo->mFetchCnt > 0);


    /**
     * Used Block Count 초기화
     */

    sUsedBlockCnt = 0;


    /**
     * Current Block Idx 초기화
     */

    sExeNestedLoopsJoin->mCurrBlockIdx = 0;


    /**
     * Cache Block Info 정보 설정
     */

    sInnerCacheBlockInfo = &(sExeNestedLoopsJoin->mInnerCacheBlockInfo);
    sOuterCacheBlockInfo = &(sExeNestedLoopsJoin->mOuterCacheBlockInfo);


    /**
     * Evaluate Info 관련 정보 설정
     */

    sWhereExprEvalInfo  = &sExeNestedLoopsJoin->mWhereExprEvalInfo;
    sJoinExprEvalInfo   = &sExeNestedLoopsJoin->mJoinExprEvalInfo;

    sWhereExprEvalInfo->mBlockCount = 1;
    sJoinExprEvalInfo->mBlockCount = 1;


    /**
     * Local Fetch Info 초기화
     */
    
    QLNX_SET_LOCAL_FETCH_INFO( & sLocalFetchInfo, aFetchNodeInfo );


    /*****************************************
     * Node 수행 여부 확인
     ****************************************/

    /* 이전 연산에 의헤 sExeNestedLoopsJoin->mIsEOF가 true로 설정된 경우, 이 함수를 다시 들어올 수 없다.
       sExeNestedLoopsJoin->mIsEOF가 true로 설정된 경우 반환되는 EOF 정보도 true로 설정되어 있다. */
    STL_DASSERT( sExeNestedLoopsJoin->mIsEOF == STL_FALSE );

    /* Anti Semi Join 은 constant filter가 없기 때문에 false filter를 체크하지 못한다. */
    STL_DASSERT( sExeNestedLoopsJoin->mHasFalseFilter == STL_FALSE );


    /**
     * Fetch에 필요한 정보 설정
     */

    sRemainSkipCnt = aFetchNodeInfo->mSkipCnt;
    sRemainFetchCnt = ( aFetchNodeInfo->mFetchCnt < aFetchNodeInfo->mDataArea->mBlockAllocCnt
                        ? aFetchNodeInfo->mFetchCnt
                        : aFetchNodeInfo->mDataArea->mBlockAllocCnt );


    /*****************************************
     * Fetch 수행
     ****************************************/

    while( 1 )
    {
        STL_RAMP( RAMP_READ_NEXT_ONE_ROW );


        /**
         * Outer Table의 Cache Block에 아직 읽을 데이터가 남았는지 체크
         */

        if( QLNX_NEED_NEXT_FETCH( sOuterCacheBlockInfo ) )
        {
            /* EOF 체크 */
            STL_TRY_THROW( sOuterCacheBlockInfo->mEOF == STL_FALSE,
                           RAMP_NO_DATA );

            /* Outer Cache Block에 새로운 데이터를 Fetch */
            QLL_OPT_CHECK_TIME( sBeginExceptTime );

            sEOF = STL_FALSE;
            QLNX_FETCH_NODE( aFetchNodeInfo,
                             sOuterCacheBlockInfo->mNodeIdx,
                             0, /* Start Idx */
                             0,
                             QLL_FETCH_COUNT_MAX,
                             &sUsedBlockCnt,
                             &sEOF,
                             aEnv );

            QLL_OPT_CHECK_TIME( sEndExceptTime );
            QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

            if( sUsedBlockCnt == 0 )
            {
                QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                STL_THROW( RAMP_NO_DATA );
            }
            else if( sEOF == STL_TRUE )
            {
                QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
            }
            else
            {
                /* Do Nothing */
            }

            sOuterCacheBlockInfo->mCurrBlockIdx = 0;
            sOuterCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;
        }
        else
        {
            /* Cache에서 읽을 Block Idx를 설정 */
            sOuterCacheBlockInfo->mCurrBlockIdx++;
        }


        /**
         * Left Outer Column에 대한 Data Value 재설정
         */

        sOuterColumnBlockItem = sExeNestedLoopsJoin->mLeftOuterColumnBlockList.mHead;
        QLNX_SET_OUTER_COLUMN_BLOCK_LIST( sOuterColumnBlockItem, aEnv );


        /* Inner Table을 다시 초기화 */
        QLNX_EXECUTE_NODE( sLocalFetchInfo.mTransID,
                           sLocalFetchInfo.mStmt,
                           sLocalFetchInfo.mDBCStmt,
                           sLocalFetchInfo.mSQLStmt,
                           sLocalFetchInfo.mDataArea->mExecNodeList[sInnerCacheBlockInfo->mNodeIdx].mOptNode,
                           sLocalFetchInfo.mDataArea,
                           aEnv );

        QLNX_RESET_CACHE_BLOCK_INFO( sInnerCacheBlockInfo );


        /* Inner Cache에 새로운 데이터를 Fetch */
        QLL_OPT_CHECK_TIME( sBeginExceptTime );

        sEOF = STL_FALSE;
        QLNX_FETCH_NODE( aFetchNodeInfo,
                         sInnerCacheBlockInfo->mNodeIdx,
                         0, /* Start Idx */
                         0,
                         1,
                         &sUsedBlockCnt,
                         &sEOF,
                         aEnv );

        QLL_OPT_CHECK_TIME( sEndExceptTime );
        QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );


        if( sUsedBlockCnt == 0 )
        {
            /**
             * Inner Table를 새로 Fetch하였는데 결과 개수가 0개인 경우
             * ------------------------------
             *  Join 결과가 없다.
             */
            QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
            if( sExeNestedLoopsJoin->mLeftOuterColumnBlockList.mCount == 0 )
            {
                sInnerCacheBlockInfo->mNeedScan = STL_FALSE;
                QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                STL_THROW( RAMP_NO_DATA );
            }
            else
            {
                /**
                 * Outer Column이 존재하는 경우 Anti Semi Join에 부합되는 경우이다.
                 */
                sInnerCacheBlockInfo->mNeedScan = STL_TRUE;
            }
        }
        else
        {
            STL_THROW( RAMP_READ_NEXT_ONE_ROW );
        }

        sInnerCacheBlockInfo->mCurrBlockIdx = 0;
        sInnerCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;


        /**
         * Left Outer Column에 대한 Data Value 재설정
         */

        sOuterColumnBlockItem = sExeNestedLoopsJoin->mLeftOuterColumnBlockList.mHead;
        QLNX_SET_OUTER_COLUMN_BLOCK_LIST( sOuterColumnBlockItem, aEnv );


        /**
         * Right Outer Column에 대한 Data Value 재설정
         */

        sOuterColumnBlockItem = sExeNestedLoopsJoin->mRightOuterColumnBlockList.mHead;
        QLNX_SET_OUTER_COLUMN_BLOCK_LIST( sOuterColumnBlockItem, aEnv );


        /**
         * Join Condition Logical Filter 평가
         */

        if( sExeNestedLoopsJoin->mHasJoinCondition == STL_TRUE )
        {
            /* Coverage : 재현하기 어려움 */
            sJoinExprEvalInfo->mBlockIdx = sExeNestedLoopsJoin->mCurrBlockIdx;
            STL_TRY( knlEvaluateOneExpression( sJoinExprEvalInfo,
                                               KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            if( !DTL_BOOLEAN_IS_TRUE( KNL_GET_BLOCK_DATA_VALUE(
                                          sJoinExprEvalInfo->mResultBlock,
                                          sExeNestedLoopsJoin->mCurrBlockIdx ) ) )
            {
                STL_THROW( RAMP_READ_NEXT_ONE_ROW );
            }
        }

      
        /**
         * Outer Table의 데이터와 Inner Table의 데이터에 대하여
         * Nested Loops Join 수행
         */

        if( sExeNestedLoopsJoin->mJoinedColBlock == NULL )
        {
            /**
             * Join된 결과로 읽어야할 column이 존재하지 않는 경우
             * ex) select 1 from t1, t2;
             */

            /* Do Nothing */
        }
        else
        {
            /**
             * Result Block에 데이터 복사
             */

            sValueBlockItem = sExeNestedLoopsJoin->mValueBlockList.mHead;
            while( sValueBlockItem != NULL )
            {
                STL_TRY( knlCopyDataValue(
                             KNL_GET_VALUE_BLOCK_DATA_VALUE(
                                 sValueBlockItem->mSrcValueBlock,
                                 *(sValueBlockItem->mSrcBlockIdx) ),
                             KNL_GET_VALUE_BLOCK_DATA_VALUE(
                                 sValueBlockItem->mDstValueBlock,
                                 *(sValueBlockItem->mDstBlockIdx) ),
                             KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                sValueBlockItem = sValueBlockItem->mNext;
            }
        }


        /**
         * Where Filter Outer Column에 대한 Data Value 재설정
         */

        sOuterColumnBlockItem = sExeNestedLoopsJoin->mWhereOuterColumnBlockList.mHead;
        QLNX_SET_OUTER_COLUMN_BLOCK_LIST( sOuterColumnBlockItem, aEnv );
        
        
        /**
         * Where Condition Logical Filter 평가
         */

        if( sExeNestedLoopsJoin->mHasWhereCondition == STL_TRUE )
        {
            sWhereExprEvalInfo->mBlockIdx = sExeNestedLoopsJoin->mCurrBlockIdx;
            STL_TRY( knlEvaluateOneExpression( sWhereExprEvalInfo,
                                               KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            if( !DTL_BOOLEAN_IS_TRUE( KNL_GET_BLOCK_DATA_VALUE(
                                          sWhereExprEvalInfo->mResultBlock,
                                          sExeNestedLoopsJoin->mCurrBlockIdx ) ) )
            {
                STL_THROW( RAMP_READ_NEXT_ONE_ROW );
            }
        }


        /**
         * Row Block들을 Result Block에 복사
         */

        sRowBlockItem = sExeNestedLoopsJoin->mRowBlockList.mHead;
        while( sRowBlockItem != NULL )
        {
            /* 원본 블럭의 idx 가져오기 */
            sTempBlockIdx =
                (sRowBlockItem->mIsLeftTableRowBlock == STL_TRUE
                 ? sOuterCacheBlockInfo->mCurrBlockIdx
                 : sInnerCacheBlockInfo->mCurrBlockIdx);

            /* Scn Value 복사 */
            SML_SET_SCN_VALUE(
                sRowBlockItem->mRowBlock,
                sExeNestedLoopsJoin->mCurrBlockIdx,
                SML_GET_SCN_VALUE(
                    sRowBlockItem->mOrgRowBlock,
                    sTempBlockIdx ) );

            /* Rid Value 복사 */
            SML_SET_RID_VALUE(
                sRowBlockItem->mRowBlock,
                sExeNestedLoopsJoin->mCurrBlockIdx,
                SML_GET_RID_VALUE(
                    sRowBlockItem->mOrgRowBlock,
                    sTempBlockIdx ) );

            sRowBlockItem = sRowBlockItem->mNext;
        }


        /**
         * Skip Count가 남았으면 Skip 수행
         */

        if( sRemainSkipCnt > 0 )
        {
            sRemainSkipCnt--;

            STL_THROW( RAMP_READ_NEXT_ONE_ROW );
        }


        /**
         * Fetch Count 체크
         */

        sExeNestedLoopsJoin->mCurrBlockIdx++;
        sRemainFetchCnt --;

        if( sRemainFetchCnt == 0 )
        {
            break;
        }
        else
        {
            /**
             * Right Table의 Cache Block을 다 읽었지만
             * 아직 Fetch Count가 남은 경우
             */

            /* Do Nothing */
        }
    }

    STL_RAMP( RAMP_NO_DATA );

    if( QLNX_IS_END_OF_TABLE_FETCH( sOuterCacheBlockInfo ) )
    {
        sEOF = STL_TRUE;
        sExeNestedLoopsJoin->mIsEOF = STL_TRUE;
    }
    else
    {
        sEOF = STL_FALSE;
        sExeNestedLoopsJoin->mIsEOF = STL_FALSE;
    }


    /**
     * OUTPUT 설정
     */

    *aEOF = sEOF;
    *aUsedBlockCnt = sExeNestedLoopsJoin->mCurrBlockIdx;

    /* Parent의 Fetch Node Info 설정 */
    aFetchNodeInfo->mFetchCnt = sRemainFetchCnt;
    aFetchNodeInfo->mSkipCnt  = sRemainSkipCnt;

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT(
        sExeNestedLoopsJoin->mJoinedColBlock,
        0,
        sExeNestedLoopsJoin->mCurrBlockIdx );

    sRowBlockItem = sExeNestedLoopsJoin->mRowBlockList.mHead;
    while( sRowBlockItem != NULL )
    {
        SML_SET_USED_BLOCK_SIZE(
            sRowBlockItem->mRowBlock,
            sExeNestedLoopsJoin->mCurrBlockIdx );

        sRowBlockItem = sRowBlockItem->mNext;
    }

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeNestedLoopsJoin->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeNestedLoopsJoin->mCommonInfo.mFetchCallCount, 1 );
    QLL_OPT_ADD_RECORD_STAT_INFO( sExeNestedLoopsJoin->mCommonInfo.mFetchRecordStat,
                                  sExeNestedLoopsJoin->mCommonInfo.mResultColBlock,
                                  *aUsedBlockCnt,
                                  aEnv );

    sExeNestedLoopsJoin->mCommonInfo.mFetchRowCnt += *aUsedBlockCnt;
 
    /* Outer Column의 DstDataValue 정보 복구 */
    sOuterColumnBlockItem = sExeNestedLoopsJoin->mLeftOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );
    sOuterColumnBlockItem = sExeNestedLoopsJoin->mRightOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );
    sOuterColumnBlockItem = sExeNestedLoopsJoin->mWhereOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );


    return STL_SUCCESS;

    STL_FINISH;

    /* Outer Column의 DstDataValue 정보 복구 */
    sOuterColumnBlockItem = sExeNestedLoopsJoin->mLeftOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );
    sOuterColumnBlockItem = sExeNestedLoopsJoin->mRightOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );
    sOuterColumnBlockItem = sExeNestedLoopsJoin->mWhereOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeNestedLoopsJoin->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeNestedLoopsJoin->mCommonInfo.mFetchCallCount, 1 );

    return STL_FAILURE;
}


/**
 * @brief NESTED LOOPS JOIN node를 Fetch한다.
 * @param[in]      aFetchNodeInfo  Fetch Node Info
 * @param[out]     aUsedBlockCnt   Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF            End Of Fetch 여부
 * @param[in]      aEnv            Environment
 *
 * @remark Fetch 수행시 호출된다.
 */
stlStatus qlnxFetchNestedLoopsJoin( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                    stlInt64              * aUsedBlockCnt,
                                    stlBool               * aEOF,
                                    qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode               = NULL;
    qlnxNestedLoopsJoin   * sExeNestedLoopsJoin     = NULL;


    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aFetchNodeInfo->mOptNode->mType == QLL_PLAN_NODE_NESTED_LOOPS_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aFetchNodeInfo->mDataArea,
                                        QLL_GET_OPT_NODE_IDX( aFetchNodeInfo->mOptNode ) );


    /**
     * NESTED LOOPS JOIN Execution Node 획득
     */

    sExeNestedLoopsJoin = sExecNode->mExecNode;


    /**
     * Join Type에 따라 다른 함수로 처리한다.
     */

    *aUsedBlockCnt = 0;
    STL_TRY( qlnxNestedJoinFetchFuncList[ sExeNestedLoopsJoin->mJoinType ] (
                 aFetchNodeInfo,
                 aUsedBlockCnt,
                 aEOF,
                 aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief NESTED LOOPS JOIN node 수행을 종료한다.
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 *
 * @remark Execution을 종료하고자 할 때 호출한다.
 */
stlStatus qlnxFinalizeNestedLoopsJoin( qllOptimizationNode   * aOptNode,
                                       qllDataArea           * aDataArea,
                                       qllEnv                * aEnv )
{
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_NESTED_LOOPS_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    /* Do Nothing */

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}




/** @} qlnx */
