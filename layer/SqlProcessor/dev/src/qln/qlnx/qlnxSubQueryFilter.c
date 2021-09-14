/*******************************************************************************
 * qlnxSubQueryFilter.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlnxSubQueryFilter.c 8502 2013-05-20 02:42:55Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlnxSubQueryFilter.c
 * @brief SQL Processor Layer Execution Node - SUB QUERY FILTER
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnx
 * @{
 */

/**
 * @brief SUB QUERY FILTER node를 초기화한다.
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
stlStatus qlnxInitializeSubQueryFilter( smlTransId              aTransID,
                                        smlStatement          * aStmt,
                                        qllDBCStmt            * aDBCStmt,
                                        qllStatement          * aSQLStmt,
                                        qllOptimizationNode   * aOptNode,
                                        qllDataArea           * aDataArea,
                                        qllEnv                * aEnv )
{
    stlInt32              i;
    stlInt32              j;
    qllExecutionNode    * sExecNode             = NULL;
    qlnoSubQueryFilter  * sOptSubQueryFilter    = NULL;
    /* qlnxSubQueryFilter  * sExeSubQueryFilter    = NULL; */
    qlnoSubQueryList    * sOptSubQueryList      = NULL;
    qlnxSubQueryList    * sExeSubQueryList      = NULL;
    qlvRefExprItem      * sExprItem             = NULL;
    knlValueBlockList   * sRefColBlock          = NULL;

    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_FILTER_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * SUB-QUERY FILTER Optimization Node 획득
     */

    sOptSubQueryFilter = (qlnoSubQueryFilter *) aOptNode->mOptNode;


    /**
     * Common Execution Node 획득
     */
        
    /* sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, */
    /*                                     QLL_GET_OPT_NODE_IDX( aOptNode ) );*/


    /**
     * SUB-QUERY FILTER Execution Node 할당
     */

    /* sExeSubQueryFilter = (qlnxSubQueryFilter *) sExecNode->mExecNode; */


    /**
     * 하위 노드의 SubQuery List 중에서 첫번째를 제외한 모든 SubQuery List에 대하여
     * Iteration Time이 Each Query인 Reference Column Block의 IsSep를 모두 FALSE로 변경한다.
     */

    for( i = 1 ; i < sOptSubQueryFilter->mAndFilterCnt ; i++ )
    {
        /* SUB-QUERY LIST Optimization Node 획득 */
        sOptSubQueryList = (qlnoSubQueryList*)(sOptSubQueryFilter->mAndFilterNodeArr[i]->mOptNode);

        /* Common Execution Node 획득 */
        sExecNode = QLL_GET_EXECUTION_NODE( aDataArea,
                                            sOptSubQueryFilter->mAndFilterNodeArr[i]->mIdx );

        /* SUB-QUERY LIST Execution Node 할당 */
        sExeSubQueryList = (qlnxSubQueryList*)(sExecNode->mExecNode);

        /* SubQuery List의 Reference Column Block을 모두 IsSep FALSE로 변경 */
        sExprItem = sOptSubQueryList->mSubQueryExprList->mHead;
        for( j = 0; j < sOptSubQueryList->mSubQueryNodeCnt; j++ )
        {
            if( sExprItem->mExprPtr->mIterationTime == DTL_ITERATION_TIME_FOR_EACH_QUERY )
            {
                sRefColBlock = sExeSubQueryList->mRefColBlockList[j];
                while( sRefColBlock != NULL )
                {
                    sRefColBlock->mValueBlock->mIsSepBuff = STL_FALSE;

                    sRefColBlock = sRefColBlock->mNext;
                }
            }

            sExprItem = sExprItem->mNext;
        }
    }

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SUB QUERY FILTER node를 수행한다.
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
stlStatus qlnxExecuteSubQueryFilter( smlTransId              aTransID,
                                     smlStatement          * aStmt,
                                     qllDBCStmt            * aDBCStmt,
                                     qllStatement          * aSQLStmt,
                                     qllOptimizationNode   * aOptNode,
                                     qllDataArea           * aDataArea,
                                     qllEnv                * aEnv )
{
    /* qllExecutionNode    * sExecNode          = NULL; */
    qlnoSubQueryFilter  * sOptSubQueryFilter = NULL;
    /* qlnxSubQueryFilter  * sExeSubQueryFilter = NULL; */
    stlInt32              sLoop              = 0;
    

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_FILTER_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /*****************************************
     * 기본 정보 획득
     ****************************************/

    /**
     * SUB-QUERY FILTER Optimization Node 획득
     */

    sOptSubQueryFilter = (qlnoSubQueryFilter *) aOptNode->mOptNode;


    /**
     * Common Execution Node 획득
     */
        
    /* sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, */
    /*                                     QLL_GET_OPT_NODE_IDX( aOptNode ) ); */


    /**
     * SUB-QUERY FILTER Execution Node 획득
     */

    /* sExeSubQueryFilter = sExecNode->mExecNode; */

    
    /**
     * 하위 노드 Execute
     */

    for( sLoop = 0 ; sLoop < sOptSubQueryFilter->mAndFilterCnt ; sLoop++ )
    {
        QLNX_EXECUTE_NODE( aTransID,
                           aStmt,
                           aDBCStmt,
                           aSQLStmt,
                           aDataArea->mExecNodeList[ sOptSubQueryFilter->mAndFilterNodeArr[ sLoop ]->mIdx ].mOptNode,
                           aDataArea,
                           aEnv );
    }


    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SUB QUERY FILTER node를 Fetch한다.
 * @param[in]      aFetchNodeInfo  Fetch Node Info
 * @param[out]     aUsedBlockCnt   Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF            End Of Fetch 여부
 * @param[in]      aEnv            Environment
 *
 * @remark Fetch 수행시 또는 반복되는 Execution 수행시 호출된다.
 */
stlStatus qlnxFetchSubQueryFilter( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                   stlInt64              * aUsedBlockCnt,
                                   stlBool               * aEOF,
                                   qllEnv                * aEnv )
{
    qllExecutionNode    * sExecNode          = NULL;
    qlnoSubQueryFilter  * sOptSubQueryFilter = NULL;
    qlnxSubQueryFilter  * sExeSubQueryFilter = NULL;
    qlnoSubQueryList    * sOptSubQueryList   = NULL;

    stlInt64              sSkipCnt          = 0;
    stlInt64              sFetchCnt         = 0;
    stlInt64              sUsedBlockCnt     = 0;
    stlInt64              sTempUsedBlockCnt = 0;

    stlInt32              i;
    stlInt32              sLoop;
    stlInt32              sCurIdx;
    knlValueBlockList   * sValueBlockList = NULL;
    knlValueBlockList   * sReadColList    = NULL;
    qlnxRowBlockItem    * sRowBlockItem   = NULL;
    smlRowBlock         * sRowBlock       = NULL;
    stlBool               sEOF            = STL_FALSE;

    stlInt64              sFetchedCnt     = 0;
    stlInt64              sSkippedCnt     = 0;
    qlnxFetchNodeInfo     sLocalFetchInfo;

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

    STL_PARAM_VALIDATE( aFetchNodeInfo->mOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_FILTER_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    QLL_OPT_CHECK_TIME( sBeginFetchTime );

    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * SUB-QUERY FILTER Optimization Node 획득
     */

    sOptSubQueryFilter = (qlnoSubQueryFilter *) aFetchNodeInfo->mOptNode->mOptNode;

    
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aFetchNodeInfo->mDataArea,
                                        QLL_GET_OPT_NODE_IDX( aFetchNodeInfo->mOptNode ) );


    /**
     * SUB-QUERY FILTER Execution Node 획득
     */

    sExeSubQueryFilter = sExecNode->mExecNode;


    /**
     * Node 수행 여부 확인
     */

    STL_DASSERT( *aEOF == STL_FALSE );
    STL_DASSERT( aFetchNodeInfo->mFetchCnt > 0);

    sSkipCnt = aFetchNodeInfo->mSkipCnt;
    sFetchCnt = aFetchNodeInfo->mFetchCnt;

    
    /**
     * Local Fetch Info 초기화
     */
    
    QLNX_SET_LOCAL_FETCH_INFO( & sLocalFetchInfo, aFetchNodeInfo );

    
    /*****************************************
     * Child Node Fetch 수행
     ****************************************/

    STL_TRY_THROW( sFetchCnt > 0, RAMP_EXIT );


    /*****************************************
     * FETCH
     ****************************************/

    sReadColList = sExeSubQueryFilter->mCommonInfo.mResultColBlock;

    STL_DASSERT( sExeSubQueryFilter->mFilterExprEvalInfo != NULL );

    STL_DASSERT( sOptSubQueryFilter->mChildNode->mType == QLL_PLAN_NODE_SUB_QUERY_LIST_TYPE );
    
    sOptSubQueryList = (qlnoSubQueryList *) sOptSubQueryFilter->mChildNode->mOptNode;


    STL_RAMP( RAMP_RETRY_FETCH );


    QLL_OPT_CHECK_TIME( sBeginExceptTime );

    /* Fetch Next */
    QLNX_FETCH_NODE( & sLocalFetchInfo,
                     sOptSubQueryFilter->mChildNode->mIdx,
                     0, /* Start Idx */
                     0,
                     sFetchCnt,
                     &sUsedBlockCnt,
                     aEOF,
                     aEnv );

    QLL_OPT_CHECK_TIME( sEndExceptTime );
    QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

    sCurIdx = 0;

    for( i = 0; i < sUsedBlockCnt; i++ )
    {
        /**
         * Evaluate SubQuery Expression
         */
        
        sExeSubQueryFilter->mFilterExprEvalInfo[ 0 ]->mBlockIdx = i;
        sExeSubQueryFilter->mFilterExprEvalInfo[ 0 ]->mBlockCount = 1;
        STL_TRY( knlEvaluateOneExpression( sExeSubQueryFilter->mFilterExprEvalInfo[ 0 ],
                                           KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        if( DTL_BOOLEAN_IS_TRUE( KNL_GET_BLOCK_DATA_VALUE(
                                     sExeSubQueryFilter->mFilterExprEvalInfo[ 0 ]->mResultBlock,
                                     i ) ) )
        {
            if( sOptSubQueryList->mChildNode != NULL )
            {
                STL_TRY( qlndSetOuterColumnValue( sLocalFetchInfo.mDataArea,
                                                  sOptSubQueryList->mChildNode->mIdx,
                                                  i,
                                                  aEnv )
                         == STL_SUCCESS );
            }
            
            for( sLoop = 1 ; sLoop < sOptSubQueryFilter->mAndFilterCnt ; sLoop++ )
            {
                QLL_OPT_CHECK_TIME( sBeginExceptTime );

                /* Fetch Next */
                sEOF = STL_FALSE;
                
                QLNX_FETCH_NODE( & sLocalFetchInfo,
                                 sOptSubQueryFilter->mAndFilterNodeArr[ sLoop ]->mIdx,
                                 i, /* Start Idx */                                 
                                 0,
                                 1,
                                 &sTempUsedBlockCnt,
                                 &sEOF,
                                 aEnv );

                QLL_OPT_CHECK_TIME( sEndExceptTime );
                QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                sExeSubQueryFilter->mFilterExprEvalInfo[ sLoop ]->mBlockIdx = i;
                sExeSubQueryFilter->mFilterExprEvalInfo[ sLoop ]->mBlockCount = 1;
                STL_TRY( knlEvaluateOneExpression( sExeSubQueryFilter->mFilterExprEvalInfo[ sLoop ],
                                                   KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                STL_TRY_THROW( DTL_BOOLEAN_IS_TRUE(
                                   KNL_GET_BLOCK_DATA_VALUE(
                                       sExeSubQueryFilter->mFilterExprEvalInfo[ sLoop ]->mResultBlock,
                                       i ) ),
                               RAMP_NEXT );
            }
            
            if( sSkipCnt > 0 )
            {
                sSkipCnt--;
            }
            else
            {
                if( sCurIdx != i )
                {
                    /* Value Block List의 i번째 데이터들을 sCurIdx번째로 복사 */
                    sValueBlockList = sReadColList;
                    while( sValueBlockList != NULL )
                    {
                        STL_TRY( knlCopyDataValue(
                                     KNL_GET_VALUE_BLOCK_DATA_VALUE(
                                         sValueBlockList->mValueBlock,
                                         i ),
                                     KNL_GET_VALUE_BLOCK_DATA_VALUE(
                                         sValueBlockList->mValueBlock,
                                         sCurIdx ),
                                     KNL_ENV(aEnv) )
                                 == STL_SUCCESS );

                        sValueBlockList = sValueBlockList->mNext;
                    }

                    /* Value Block List의 i번째 데이터들을 sCurIdx번째로 복사 */
                    if( sExeSubQueryFilter->mCommonInfo.mRowBlockList != NULL )
                    {
                        sRowBlockItem = sExeSubQueryFilter->mCommonInfo.mRowBlockList->mHead;
                        while( sRowBlockItem != NULL )
                        {
                            sRowBlock    = sRowBlockItem->mRowBlock;
                            
                            sRowBlock->mScnBlock[ sCurIdx ] = sRowBlock->mScnBlock[ i ];
                            sRowBlock->mRidBlock[ sCurIdx ] = sRowBlock->mRidBlock[ i ];
                            
                            sRowBlockItem = sRowBlockItem->mNext;
                        }
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }

                sCurIdx++;
                sFetchCnt--;

                if( sFetchCnt == 0 )
                {
                    break;
                }
            }
        }

        STL_RAMP( RAMP_NEXT );
    }

    if( (*aEOF == STL_FALSE) &&
        ((sSkipCnt > 0) || (sCurIdx == 0)) )
    {
        STL_THROW( RAMP_RETRY_FETCH );
    }

    sFetchedCnt = sCurIdx;
    sSkippedCnt = aFetchNodeInfo->mSkipCnt - sSkipCnt;


    STL_RAMP( RAMP_EXIT );


    /**
     * OUTPUT 설정
     */

    *aUsedBlockCnt = sFetchedCnt;
    
    /* Parent의 Fetch Node Info 설정 */
    aFetchNodeInfo->mFetchCnt -= sFetchedCnt;
    aFetchNodeInfo->mSkipCnt  -= sSkippedCnt;
    
    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sValueBlockList,
                                         0,
                                         sFetchedCnt );

    /* Block Used Count 설정 */
    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sExeSubQueryFilter->mCommonInfo.mResultColBlock,
                                         0,
                                         sFetchedCnt );

    if( sExeSubQueryFilter->mCommonInfo.mRowBlockList != NULL )
    {
        sRowBlockItem = sExeSubQueryFilter->mCommonInfo.mRowBlockList->mHead;
        while( sRowBlockItem != NULL )
        {
            SML_SET_USED_BLOCK_SIZE( sRowBlockItem->mRowBlock,
                                     sFetchedCnt );

            sRowBlockItem = sRowBlockItem->mNext;
        }
    }


    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeSubQueryFilter->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeSubQueryFilter->mCommonInfo.mFetchCallCount, 1 );
    QLL_OPT_ADD_RECORD_STAT_INFO( sExeSubQueryFilter->mCommonInfo.mFetchRecordStat,
                                  sExeSubQueryFilter->mCommonInfo.mResultColBlock,
                                  *aUsedBlockCnt,
                                  aEnv );

    sExeSubQueryFilter->mCommonInfo.mFetchRowCnt += *aUsedBlockCnt;

    
    return STL_SUCCESS;

    STL_FINISH;

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeSubQueryFilter->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeSubQueryFilter->mCommonInfo.mFetchCallCount, 1 );

    return STL_FAILURE;
}


/**
 * @brief SUB QUERY FILTER node 수행을 종료한다.
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 *
 * @remark Execution을 종료하고자 할 때 호출한다.
 */
stlStatus qlnxFinalizeSubQueryFilter( qllOptimizationNode   * aOptNode,
                                      qllDataArea           * aDataArea,
                                      qllEnv                * aEnv )
{
    stlInt32              i;
    stlInt32              j;
    qllExecutionNode    * sExecNode             = NULL;
    qlnoSubQueryFilter  * sOptSubQueryFilter    = NULL;
    /* qlnxSubQueryFilter  * sExeSubQueryFilter    = NULL; */
    qlnoSubQueryList    * sOptSubQueryList      = NULL;
    qlnxSubQueryList    * sExeSubQueryList      = NULL;
    qlvRefExprItem      * sExprItem             = NULL;
    knlValueBlockList   * sRefColBlock          = NULL;

    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_FILTER_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    /**
     * SUB-QUERY FILTER Optimization Node 획득
     */

    sOptSubQueryFilter = (qlnoSubQueryFilter *) aOptNode->mOptNode;

    
    /**
     * Common Execution Node 획득
     */
        
    /* sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) ); */


    /**
     * SUB-QUERY FILTER Execution Node 획득
     */

    /* sExeSubQueryFilter = sExecNode->mExecNode; */


    /**
     * 하위 노드의 SubQuery List 중에서 첫번째를 제외한 모든 SubQuery List에 대하여
     * Iteration Time이 Each Query인 Reference Column Block의 IsSep를 모두 TRUE로 변경한다.
     * (Init에서 FALSE로 변경한 것을 다시 원상복구하는 것임)
     */

    for( i = 1 ; i < sOptSubQueryFilter->mAndFilterCnt ; i++ )
    {
        /* SUB-QUERY LIST Optimization Node 획득 */
        sOptSubQueryList = (qlnoSubQueryList*)(sOptSubQueryFilter->mAndFilterNodeArr[i]->mOptNode);

        /* Common Execution Node 획득 */
        sExecNode = QLL_GET_EXECUTION_NODE( aDataArea,
                                            sOptSubQueryFilter->mAndFilterNodeArr[i]->mIdx );

        /* SUB-QUERY LIST Execution Node 할당 */
        sExeSubQueryList = (qlnxSubQueryList*)(sExecNode->mExecNode);

        /* SubQuery List의 Reference Column Block을 모두 IsSep FALSE로 변경 */
        sExprItem = sOptSubQueryList->mSubQueryExprList->mHead;
        for( j = 0; j < sOptSubQueryList->mSubQueryNodeCnt; j++ )
        {
            if( sExprItem->mExprPtr->mIterationTime == DTL_ITERATION_TIME_FOR_EACH_QUERY )
            {
                sRefColBlock = sExeSubQueryList->mRefColBlockList[j];
                while( sRefColBlock != NULL )
                {
                    sRefColBlock->mValueBlock->mIsSepBuff = STL_FALSE;

                    sRefColBlock = sRefColBlock->mNext;
                }
            }
        }
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} qlnx */
