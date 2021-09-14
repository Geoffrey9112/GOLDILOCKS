/*******************************************************************************
 * qlnxSelectStmt.c
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
 * @file qlnxSelectStmt.c
 * @brief SQL Processor Layer Execution - SELECT STATEMENT Node
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnx
 * @{
 */


/**
 * @brief SELECT STATEMENT node를 초기화한다.
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
stlStatus qlnxInitializeSelectStmt( smlTransId              aTransID,
                                    smlStatement          * aStmt,
                                    qllDBCStmt            * aDBCStmt,
                                    qllStatement          * aSQLStmt,
                                    qllOptimizationNode   * aOptNode,
                                    qllDataArea           * aDataArea,
                                    qllEnv                * aEnv )
{
    /* qllExecutionNode  * sExecNode = NULL; */
    qlnoSelectStmt    * sOptSelectStmt = NULL;
    /* qlnxSelectStmt    * sExeSelectStmt = NULL; */

    qllOptNodeItem    * sOptNodeItem = NULL;
    stlInt16            sNodeCnt = 0;

    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_STMT_SELECT_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * SELECT STATEMENT Optimization Node 획득
     */

    sOptSelectStmt = (qlnoSelectStmt *) aOptNode->mOptNode;


    /**
     * Common Execution Node 획득
     */
        
    /* sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, */
    /*                                     QLL_GET_OPT_NODE_IDX( aOptNode ) ); */


    /**
     * SELECT STATEMENT Execution Node 획득
     */

    /* sExeSelectStmt = (sExeSelectStmt *) sExecNode->mExecNode; */
    

    /******************************************************
     * 하위 Node들 Initialize 수행
     ******************************************************/

    sOptNodeItem = sOptSelectStmt->mOptNodeList->mTail;
    
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
 * @brief SELECT STATEMENT node를 수행한다.
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
stlStatus qlnxExecuteSelectStmt( smlTransId              aTransID,
                                 smlStatement          * aStmt,
                                 qllDBCStmt            * aDBCStmt,
                                 qllStatement          * aSQLStmt,
                                 qllOptimizationNode   * aOptNode,
                                 qllDataArea           * aDataArea,
                                 qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode      = NULL;
    qlnoSelectStmt        * sOptSelectStmt = NULL;
    qlnxSelectStmt        * sExeSelectStmt = NULL;

    stlInt64                sSkipCnt       = 0;
    stlInt64                sFetchCnt      = 0;


    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_STMT_SELECT_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * SELECT STATEMENT Optimization Node 획득
     */

    sOptSelectStmt = (qlnoSelectStmt *) aOptNode->mOptNode;

    
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * SELECT STATEMENT Execution Node 획득
     */

    sExeSelectStmt = (qlnxSelectStmt *) sExecNode->mExecNode;

    
    /******************************************************
     * Statement 단위 Pseudo Column 평가
     ******************************************************/
    
    /* /\** */
    /*  * STATEMENT 단위 Pseudo Column 수행 */
    /*  *\/ */

    /* if( sOptSelectStmt->mStmtExprList->mInitExprList->mPseudoColExprList->mCount > 0 ) */
    /* { */
    /*     STL_TRY( qlxExecutePseudoColumnBlock( */
    /*                  aTransID, */
    /*                  aStmt, */
    /*                  DTL_ITERATION_TIME_FOR_EACH_STATEMENT, */
    /*                  NULL, */
    /*                  NULL, */
    /*                  sOptSelectStmt->mStmtExprList->mInitExprList->mPseudoColExprList, */
    /*                  sExeSelectStmt->mStmtPseudoColBlock, */
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

    STL_TRY( qlxEvaluateConstExpr( sOptSelectStmt->mStmtExprList->mConstExprStack,
                                   aDataArea,
                                   KNL_ENV( aEnv ) )
             == STL_SUCCESS );


    /*****************************************
     * OFFSET .. LIMIT 절 관련
     ****************************************/

    /**
     * Skip Count 설정
     */

    if( sOptSelectStmt->mResultSkip == NULL )
    {
        sSkipCnt = sOptSelectStmt->mSkipCnt;
    }
    else if( sOptSelectStmt->mResultSkip->mType == QLV_EXPR_TYPE_VALUE )
    {
        sSkipCnt = sOptSelectStmt->mSkipCnt;
    }
    else
    {
        STL_TRY( qlnxGetBindedValue4FETCH( aDBCStmt,
                                           aSQLStmt,
                                           STL_FALSE, /* aIsFetch */
                                           sOptSelectStmt->mResultSkip,
                                           & sSkipCnt,
                                           aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Fetch Count 설정
     */
    
    if( sOptSelectStmt->mResultLimit == NULL )
    {
        sFetchCnt = sOptSelectStmt->mFetchCnt;
    }
    else if( sOptSelectStmt->mResultLimit->mType == QLV_EXPR_TYPE_VALUE )
    {
        sFetchCnt = sOptSelectStmt->mFetchCnt;
    }
    else
    {
        STL_TRY( qlnxGetBindedValue4FETCH( aDBCStmt,
                                           aSQLStmt,
                                           STL_TRUE, /* aIsFetch */
                                           sOptSelectStmt->mResultLimit,
                                           & sFetchCnt,
                                           aEnv )
                 == STL_SUCCESS );
    }

    sExeSelectStmt->mRemainSkipCnt  = sSkipCnt;
    sExeSelectStmt->mRemainFetchCnt = sFetchCnt;

    
    /******************************************************
     * 하위 Node들 Execute 수행
     ******************************************************/

    QLNX_EXECUTE_NODE( aTransID,
                       aStmt,
                       aDBCStmt,
                       aSQLStmt,
                       aDataArea->mExecNodeList[ sOptSelectStmt->mQueryNode->mIdx ].mOptNode,
                       aDataArea,
                       aEnv );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SELECT STATEMENT node를 Fetch한다.
 * @param[in]      aFetchNodeInfo  Fetch Node Info
 * @param[out]     aUsedBlockCnt   Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF            End Of Fetch 여부
 * @param[in]      aEnv            Environment
 *
 * @remark Fetch 수행시 호출된다.
 */
stlStatus qlnxFetchSelectStmt( qlnxFetchNodeInfo     * aFetchNodeInfo,
                               stlInt64              * aUsedBlockCnt,
                               stlBool               * aEOF,
                               qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode      = NULL;
    qlnoSelectStmt        * sOptSelectStmt = NULL;
    qlnxSelectStmt        * sExeSelectStmt = NULL;

    stlInt64                sSkipCnt       = 0;
    stlInt64                sFetchCnt      = 0;

    stlInt64                sFetchedCnt    = 0;
    stlInt64                sSkippedCnt    = 0;
    qlnxFetchNodeInfo       sLocalFetchInfo;

    QLL_OPT_DECLARE( stlTime sBeginFetchTime );
    QLL_OPT_DECLARE( stlTime sEndFetchTime );


    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aFetchNodeInfo->mOptNode->mType == QLL_PLAN_NODE_STMT_SELECT_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    QLL_OPT_CHECK_TIME( sBeginFetchTime );

    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * SELECT STATEMENT Optimization Node 획득
     */

    sOptSelectStmt = (qlnoSelectStmt *) aFetchNodeInfo->mOptNode->mOptNode;

    
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aFetchNodeInfo->mDataArea,
                                        QLL_GET_OPT_NODE_IDX( aFetchNodeInfo->mOptNode ) );


    /**
     * SELECT STATEMENT Execution Node 획득
     */

    sExeSelectStmt = sExecNode->mExecNode;

    
    /**
     * Node 수행 여부 확인 
     */

    STL_DASSERT( *aEOF == STL_FALSE );


    /**
     * Local Fetch Info 초기화
     */
    
    QLNX_SET_LOCAL_FETCH_INFO( & sLocalFetchInfo, aFetchNodeInfo );


    /*****************************************
     * Child Node Fetch 수행
     ****************************************/

    sSkipCnt = aFetchNodeInfo->mSkipCnt + sExeSelectStmt->mRemainSkipCnt;
    if ( sExeSelectStmt->mRemainFetchCnt > aFetchNodeInfo->mFetchCnt )
    {
        sFetchCnt = aFetchNodeInfo->mFetchCnt;
    }
    else
    {
        sFetchCnt = sExeSelectStmt->mRemainFetchCnt;
    }

    sExeSelectStmt->mRemainSkipCnt = 0;

    STL_DASSERT( sFetchCnt > 0 );

    
    /**
     * FETCH
     */
    
    QLNX_FETCH_NODE( & sLocalFetchInfo,
                     sOptSelectStmt->mQueryNode->mIdx,
                     0, /* Start Idx */
                     sSkipCnt,
                     sFetchCnt,
                     & sFetchedCnt,
                     aEOF,
                     aEnv );

    if( sSkipCnt > 0 )
    {
        sSkippedCnt = sSkipCnt - sLocalFetchInfo.mSkipCnt;
        if( sSkippedCnt >= sExeSelectStmt->mRemainSkipCnt )
        {
            sExeSelectStmt->mRemainSkipCnt = 0;
            sSkippedCnt -= sExeSelectStmt->mRemainSkipCnt;
        }
        else
        {
            sExeSelectStmt->mRemainSkipCnt -= sSkippedCnt;
            sSkippedCnt = 0;
        }
    }
    else
    {
        /* Do Nothing */
    }


    /* Remain Fetch Count 수정 */
    sExeSelectStmt->mRemainFetchCnt -= sFetchedCnt;
    

    /**
     * OUTPUT 설정
     */

    *aUsedBlockCnt = sFetchedCnt;
    
    /* Parent의 Fetch Node Info 설정 */
    aFetchNodeInfo->mFetchCnt -= sFetchedCnt;
    aFetchNodeInfo->mSkipCnt  -= sSkippedCnt;

    if ( sExeSelectStmt->mRemainFetchCnt <= 0 )
    {
        *aEOF = STL_TRUE;
    }

    
    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sExeSelectStmt->mCommonInfo.mFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_COUNT( sExeSelectStmt->mCommonInfo.mFetchCallCount, 1 );
    QLL_OPT_ADD_RECORD_STAT_INFO( sExeSelectStmt->mCommonInfo.mFetchRecordStat,
                                  sExeSelectStmt->mCommonInfo.mResultColBlock,
                                  *aUsedBlockCnt,
                                  aEnv );

    return STL_SUCCESS;

    STL_FINISH;

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sExeSelectStmt->mCommonInfo.mFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_COUNT( sExeSelectStmt->mCommonInfo.mFetchCallCount, 1 );

    return STL_FAILURE;
}




/**
 * @brief SELECT STATEMENT node 수행을 종료한다.
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 *
 * @remark Execution을 종료하고자 할 때 호출한다.
 */
stlStatus qlnxFinalizeSelectStmt( qllOptimizationNode   * aOptNode,
                                  qllDataArea           * aDataArea,
                                  qllEnv                * aEnv )
{
//    qllExecutionNode      * sExecNode = NULL;
//    qlnxSelectStmt        * sExeSelectStmt = NULL;

    stlInt16                sNodeCnt = 0;    

    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_STMT_SELECT_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    /**
     * Common Execution Node 획득
     */
        
//    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * SELECT STATEMENT Execution Node 획득
     */

//    sExeSelectStmt = sExecNode->mExecNode;


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
