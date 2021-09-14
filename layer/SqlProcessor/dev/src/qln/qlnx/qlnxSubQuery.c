/*******************************************************************************
 * qlnxSubQuery.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlnxSubQuery.c 8502 2013-05-20 02:42:55Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlnxSubQuery.c
 * @brief SQL Processor Layer Execution Node - SUB QUERY 
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnx
 * @{
 */

/**
 * @brief SUB QUERY node를 초기화한다.
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
stlStatus qlnxInitializeSubQuery( smlTransId              aTransID,
                                  smlStatement          * aStmt,
                                  qllDBCStmt            * aDBCStmt,
                                  qllStatement          * aSQLStmt,
                                  qllOptimizationNode   * aOptNode,
                                  qllDataArea           * aDataArea,
                                  qllEnv                * aEnv )
{
    qllExecutionNode  * sExecNode      = NULL;
    /* qlnoSubQuery      * sOptSubQuery   = NULL; */
    qlnxSubQuery      * sExeSubQuery   = NULL;

    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * SUB-QUERY Optimization Node 획득
     */

    /* sOptSubQuery = (qlnoSubQuery *) aOptNode->mOptNode; */


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea,
                                        QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * SUB-QUERY Execution Node 할당
     */

    sExeSubQuery = (qlnxSubQuery *) sExecNode->mExecNode;

    sExeSubQuery->mHasFalseFilter = STL_FALSE;

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SUB QUERY node를 수행한다.
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
stlStatus qlnxExecuteSubQuery( smlTransId              aTransID,
                               smlStatement          * aStmt,
                               qllDBCStmt            * aDBCStmt,
                               qllStatement          * aSQLStmt,
                               qllOptimizationNode   * aOptNode,
                               qllDataArea           * aDataArea,
                               qllEnv                * aEnv )
{
    qllExecutionNode  * sExecNode    = NULL;
    qlnoSubQuery      * sOptSubQuery = NULL;
    qlnxSubQuery      * sExeSubQuery = NULL;
    

    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /*****************************************
     * 기본 정보 획득
     ****************************************/

    /**
     * SUB-QUERY Optimization Node 획득
     */

    sOptSubQuery = (qlnoSubQuery *) aOptNode->mOptNode;


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea,
                                        QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * SUB-QUERY Execution Node 획득
     */

    sExeSubQuery = sExecNode->mExecNode;

    
    /**
     * @todo 불필요한 요소를 제거하여 Filter 재구성 (확장 필요)
     */
    
    sExeSubQuery->mHasFalseFilter =
        qlfHasFalseFilter( sOptSubQuery->mFilterExpr,
                           aDataArea );


    /**
     * 하위 노드 Execute
     */

    QLNX_EXECUTE_NODE( aTransID,
                       aStmt,
                       aDBCStmt,
                       aSQLStmt,
                       aDataArea->mExecNodeList[ sOptSubQuery->mChildNode->mIdx ].mOptNode,
                       aDataArea,
                       aEnv );

    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SUB QUERY node를 Fetch한다.
 * @param[in]      aFetchNodeInfo  Fetch Node Info
 * @param[out]     aUsedBlockCnt   Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF            End Of Fetch 여부
 * @param[in]      aEnv            Environment
 *
 * @remark Fetch 수행시 또는 반복되는 Execution 수행시 호출된다.
 */
stlStatus qlnxFetchSubQuery( qlnxFetchNodeInfo     * aFetchNodeInfo,
                             stlInt64              * aUsedBlockCnt,
                             stlBool               * aEOF,
                             qllEnv                * aEnv )
{
    qllExecutionNode  * sExecNode     = NULL;
    qlnoSubQuery      * sOptSubQuery  = NULL;
    qlnxSubQuery      * sExeSubQuery  = NULL;

    stlInt64            sSkipCnt      = 0;
    stlInt64            sFetchCnt     = 0;

    stlInt32            i;
    stlInt32            sCurIdx;
    knlValueBlockList * sValueBlockList   = NULL;
    /* knlValueBlockList * sOuterColBlock    = NULL; */
    /* knlValueBlockList * sOuterOrgColBlock = NULL; */
    
    stlInt64            sFetchedCnt   = 0;
    stlInt64            sSkippedCnt   = 0;
    qlnxFetchNodeInfo   sLocalFetchInfo;
    
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

    STL_PARAM_VALIDATE( aFetchNodeInfo->mOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    QLL_OPT_CHECK_TIME( sBeginFetchTime );

    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * SUB-QUERY Optimization Node 획득
     */

    sOptSubQuery = (qlnoSubQuery *) aFetchNodeInfo->mOptNode->mOptNode;

    
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aFetchNodeInfo->mDataArea,
                                        QLL_GET_OPT_NODE_IDX( aFetchNodeInfo->mOptNode ) );


    /**
     * SUB-QUERY Execution Node 획득
     */

    sExeSubQuery = sExecNode->mExecNode;


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

    
    if( sExeSubQuery->mHasFalseFilter == STL_TRUE )
    {
        sFetchedCnt = 0;
        *aEOF = STL_TRUE;

        STL_THROW( RAMP_EXIT );
    }
    else
    {
        /* Do Nothing */
    }

    /*****************************************
     * FETCH
     ****************************************/

    /**
     * OUTER COLUMN 설정
     */

    /* sOuterColBlock    = sExeSubQuery->mOuterColBlock; */
    /* sOuterOrgColBlock = sExeSubQuery->mOuterOrgColBlock; */
    
    /* while( sOuterColBlock != NULL ) */
    /* { */
    /*     STL_TRY( knlCopyDataValue( KNL_GET_BLOCK_FIRST_DATA_VALUE( sOuterOrgColBlock ), */
    /*                                KNL_GET_BLOCK_FIRST_DATA_VALUE( sOuterColBlock ), */
    /*                                KNL_ENV( aEnv ) ) */
    /*              == STL_SUCCESS ); */

    /*     sOuterColBlock    = sOuterColBlock->mNext; */
    /*     sOuterOrgColBlock = sOuterOrgColBlock->mNext; */
    /* } */


    /**
     * Fetch
     */
    
    if( sExeSubQuery->mFilterExprEvalInfo == NULL )
    {
        QLL_OPT_CHECK_TIME( sBeginExceptTime );

        /* Fetch Next */
        QLNX_FETCH_NODE( & sLocalFetchInfo,
                         sOptSubQuery->mChildNode->mIdx,
                         0, /* Start Idx */
                         sSkipCnt,
                         sFetchCnt,
                         & sFetchedCnt,
                         aEOF,
                         aEnv );

        sSkippedCnt = sSkipCnt - sLocalFetchInfo.mSkipCnt;

        QLL_OPT_CHECK_TIME( sEndExceptTime );
        QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );
    }
    else
    {
        STL_RAMP( RAMP_RETRY_FETCH );

        QLL_OPT_CHECK_TIME( sBeginExceptTime );

        /* Fetch Next */
        QLNX_FETCH_NODE( & sLocalFetchInfo,
                         sOptSubQuery->mChildNode->mIdx,
                         0, /* Start Idx */
                         0,
                         ( sSkipCnt + sFetchCnt >= sFetchCnt ? sSkipCnt + sFetchCnt : QLL_FETCH_COUNT_MAX ),
                         & sFetchedCnt,
                         aEOF,
                         aEnv );

        QLL_OPT_CHECK_TIME( sEndExceptTime );
        QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

        sCurIdx = 0;

        STL_DASSERT( sFetchedCnt == 0 ? *aEOF == STL_TRUE : STL_TRUE );
        
        for( i = 0; i < sFetchedCnt; i++ )
        {
            sExeSubQuery->mFilterExprEvalInfo->mBlockIdx = i;
            sExeSubQuery->mFilterExprEvalInfo->mBlockCount = 1;
            STL_TRY( knlEvaluateOneExpression( sExeSubQuery->mFilterExprEvalInfo,
                                               KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            if( DTL_BOOLEAN_IS_TRUE( KNL_GET_BLOCK_DATA_VALUE(
                                         sExeSubQuery->mFilterExprEvalInfo->mResultBlock,
                                         i ) ) )
            {
                if( sSkipCnt > 0 )
                {
                    sSkipCnt--;
                }
                else
                {
                    if( sCurIdx != i )
                    {
                        /* Value Block List의 i번째 데이터들을 sCurIdx번째로 복사 */
                        sValueBlockList = sExeSubQuery->mTargetBlock;
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
                    }

                    sCurIdx++;
                    sFetchCnt--;

                    if( sFetchCnt == 0 )
                    {
                        break;
                    }
                }
            }
        }

        if( (*aEOF == STL_FALSE) &&
            ((sSkipCnt > 0) || (sCurIdx == 0)) )
        {
            STL_THROW( RAMP_RETRY_FETCH );
        }

        sFetchedCnt = sCurIdx;
        sSkippedCnt = aFetchNodeInfo->mSkipCnt - sSkipCnt;
    }

    STL_RAMP( RAMP_EXIT );

    
    /**
     * OUTPUT 설정
     */

    *aUsedBlockCnt = sFetchedCnt;
    
    /* Parent의 Fetch Node Info 설정 */
    aFetchNodeInfo->mFetchCnt -= sFetchedCnt;
    aFetchNodeInfo->mSkipCnt  -= sSkippedCnt;

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sExeSubQuery->mTargetBlock,
                                         0,
                                         sFetchedCnt );                                         

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeSubQuery->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeSubQuery->mCommonInfo.mFetchCallCount, 1 );
    QLL_OPT_ADD_RECORD_STAT_INFO( sExeSubQuery->mCommonInfo.mFetchRecordStat,
                                  sExeSubQuery->mCommonInfo.mResultColBlock,
                                  *aUsedBlockCnt,
                                  aEnv );

    sExeSubQuery->mCommonInfo.mFetchRowCnt += *aUsedBlockCnt;

    
    return STL_SUCCESS;

    STL_FINISH;

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeSubQuery->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeSubQuery->mCommonInfo.mFetchCallCount, 1 );

    return STL_FAILURE;
}


/**
 * @brief SUB QUERY node 수행을 종료한다.
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 *
 * @remark Execution을 종료하고자 할 때 호출한다.
 */
stlStatus qlnxFinalizeSubQuery( qllOptimizationNode   * aOptNode,
                                qllDataArea           * aDataArea,
                                qllEnv                * aEnv )
{
    /* qllExecutionNode  * sExecNode    = NULL; */
    /* qlnoSubQuery      * sOptSubQuery = NULL; */
    /* qlnxSubQuery      * sExeSubQuery = NULL; */

    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    /**
     * SUB-QUERY Optimization Node 획득
     */

    /* sOptSubQuery = (qlnoSubQuery *) aOptNode->mOptNode; */

    
    /**
     * Common Execution Node 획득
     */
        
    /* sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) ); */


    /**
     * SUB-QUERY Execution Node 획득
     */

    /* sExeSubQuery = sExecNode->mExecNode; */


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} qlnx */
