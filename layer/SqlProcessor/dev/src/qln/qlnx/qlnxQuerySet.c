/*******************************************************************************
 * qlnxQuerySet.c
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
 * @file qlnxQuerySet.c
 * @brief SQL Processor Layer Execution Node - QUERY SET
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnx
 * @{
 */

/**
 * @brief QUERY SET node 를 초기화한다.
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aStmt         Statement
 * @param[in]  aDBCStmt      DBC Statement
 * @param[in]  aSQLStmt      SQL Statement
 * @param[in]  aOptNode      Optimization Node
 * @param[in]  aDataArea     Data Area
 * @param[in]  aEnv          Environment
 * @remarks 
 */
stlStatus qlnxInitializeQuerySet( smlTransId              aTransID,
                                   smlStatement          * aStmt,
                                   qllDBCStmt            * aDBCStmt,
                                   qllStatement          * aSQLStmt,
                                   qllOptimizationNode   * aOptNode,
                                   qllDataArea           * aDataArea,
                                   qllEnv                * aEnv )
{
    /* nothing to do */
    
    return STL_SUCCESS;
}


/**
 * @brief QUERY SET node 를 수행한다.
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aStmt         Statement
 * @param[in]  aDBCStmt      DBC Statement
 * @param[in]  aSQLStmt      SQL Statement
 * @param[in]  aOptNode      Optimization Node
 * @param[in]  aDataArea     Data Area
 * @param[in]  aEnv          Environment
 * @remarks 
 */
stlStatus qlnxExecuteQuerySet( smlTransId              aTransID,
                               smlStatement          * aStmt,
                               qllDBCStmt            * aDBCStmt,
                               qllStatement          * aSQLStmt,
                               qllOptimizationNode   * aOptNode,
                               qllDataArea           * aDataArea,
                               qllEnv                * aEnv )
{
    qllExecutionNode   * sExecNode     = NULL;
    qlnoQuerySet       * sOptQuerySet  = NULL;
    qlnxQuerySet       * sExeQuerySet  = NULL;

    stlInt64             sSkipCnt      = 0;
    stlInt64             sFetchCnt     = 0;

    /**
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_QUERY_SET_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );

    /*****************************************
     * 기본 정보 획득
     ****************************************/

    sOptQuerySet = (qlnoQuerySet *) aOptNode->mOptNode;
    
    sExecNode    = QLL_GET_EXECUTION_NODE( aDataArea, aOptNode->mIdx );
    sExeQuerySet = (qlnxQuerySet *)sExecNode->mExecNode;

    /*****************************************
     * OFFSET .. LIMIT 절 관련
     ****************************************/

    /**
     * Skip Count 설정
     */

    if( sOptQuerySet->mResultSkip == NULL )
    {
        sSkipCnt = sOptQuerySet->mSkipCnt;
    }
    else if( sOptQuerySet->mResultSkip->mType == QLV_EXPR_TYPE_VALUE )
    {
        sSkipCnt = sOptQuerySet->mSkipCnt;
    }
    else
    {
        STL_TRY( qlnxGetBindedValue4FETCH( aDBCStmt,
                                           aSQLStmt,
                                           STL_FALSE, /* aIsFetch */
                                           sOptQuerySet->mResultSkip,
                                           & sSkipCnt,
                                           aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Fetch Count 설정
     */
    
    if( sOptQuerySet->mResultLimit == NULL )
    {
        sFetchCnt = sOptQuerySet->mFetchCnt;
    }
    else if( sOptQuerySet->mResultLimit->mType == QLV_EXPR_TYPE_VALUE )
    {
        sFetchCnt = sOptQuerySet->mFetchCnt;
    }
    else
    {
        STL_TRY( qlnxGetBindedValue4FETCH( aDBCStmt,
                                           aSQLStmt,
                                           STL_TRUE, /* aIsFetch */
                                           sOptQuerySet->mResultLimit,
                                           & sFetchCnt,
                                           aEnv )
                 == STL_SUCCESS );
    }

    sExeQuerySet->mRemainSkipCnt  = sSkipCnt;
    sExeQuerySet->mRemainFetchCnt = sFetchCnt;

    /**
     * 하위 노드 Execute
     */

    QLNX_EXECUTE_NODE( aTransID,
                       aStmt,
                       aDBCStmt,
                       aSQLStmt,
                       aDataArea->mExecNodeList[sOptQuerySet->mChildNode->mIdx].mOptNode,
                       aDataArea,
                       aEnv );

    
    
    return STL_SUCCESS;
   
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief QUERY SET node 를 Fetch 한다.
 * @param[in]      aFetchNodeInfo  Fetch Node Info
 * @param[out]     aUsedBlockCnt   Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF            End Of Fetch 여부
 * @param[in]      aEnv            Environment
 * @remark 
 */
stlStatus qlnxFetchQuerySet( qlnxFetchNodeInfo     * aFetchNodeInfo,
                             stlInt64              * aUsedBlockCnt,
                             stlBool               * aEOF,
                             qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode    = NULL;
    qlnoQuerySet          * sOptQuerySet = NULL;
    qlnxQuerySet          * sExeQuerySet = NULL;

    stlInt64                sSkipCnt      = 0;
    stlInt64                sFetchCnt     = 0;

    stlInt64                sFetchedCnt   = 0;
    stlInt64                sSkippedCnt   = 0;
    qlnxFetchNodeInfo       sLocalFetchInfo;

    QLL_OPT_DECLARE( stlTime sBeginFetchTime );
    QLL_OPT_DECLARE( stlTime sEndFetchTime );
    QLL_OPT_DECLARE( stlTime sFetchTime );

    QLL_OPT_DECLARE( stlTime sBeginExceptTime );
    QLL_OPT_DECLARE( stlTime sEndExceptTime );
    QLL_OPT_DECLARE( stlTime sExceptTime );

    QLL_OPT_SET_VALUE( sFetchTime, 0 );
    QLL_OPT_SET_VALUE( sExceptTime, 0 );

    
    /**
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aFetchNodeInfo->mOptNode->mType == QLL_PLAN_NODE_QUERY_SET_TYPE,  QLL_ERROR_STACK(aEnv) );


    QLL_OPT_CHECK_TIME( sBeginFetchTime );

    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    sOptQuerySet = (qlnoQuerySet *) aFetchNodeInfo->mOptNode->mOptNode;
    
    sExecNode = QLL_GET_EXECUTION_NODE( aFetchNodeInfo->mDataArea,
                                        aFetchNodeInfo->mOptNode->mIdx );
    sExeQuerySet = (qlnxQuerySet *) sExecNode->mExecNode;


    /**
     * Node 수행 여부 확인 
     */

    STL_DASSERT( *aEOF == STL_FALSE );
    STL_DASSERT( aFetchNodeInfo->mFetchCnt > 0);
    

    /**
     * Local Fetch Info 초기화
     */
    
    QLNX_SET_LOCAL_FETCH_INFO( & sLocalFetchInfo, aFetchNodeInfo );


    /*****************************************
     * OFFSET .. FETCH 값 조정
     ****************************************/

    sSkipCnt  = aFetchNodeInfo->mSkipCnt + sExeQuerySet->mRemainSkipCnt;
    if ( sExeQuerySet->mRemainFetchCnt > aFetchNodeInfo->mFetchCnt )
    {
        sFetchCnt = aFetchNodeInfo->mFetchCnt;
    }
    else
    {
        sFetchCnt = sExeQuerySet->mRemainFetchCnt;
    }

    sExeQuerySet->mRemainSkipCnt = 0;

    if( sFetchCnt == 0 )
    {
        *aEOF = ( sExeQuerySet->mRemainFetchCnt == 0 );
        STL_THROW( RAMP_EXIT );
    }

    /*****************************************
     * FETCH
     ****************************************/

    QLL_OPT_CHECK_TIME( sBeginExceptTime );

    /* Fetch Next */
    QLNX_FETCH_NODE( & sLocalFetchInfo,
                     sOptQuerySet->mChildNode->mIdx,
                     0, /* Start Idx */
                     sSkipCnt,
                     sFetchCnt,
                     & sFetchedCnt,
                     aEOF,
                     aEnv );

    if( sSkipCnt > 0 )
    {
        sSkippedCnt = sSkipCnt - sLocalFetchInfo.mSkipCnt;
        if( sSkippedCnt >= sExeQuerySet->mRemainSkipCnt )
        {
            sExeQuerySet->mRemainSkipCnt = 0;
            sSkippedCnt -= sExeQuerySet->mRemainSkipCnt;
        }
        else
        {
            sExeQuerySet->mRemainSkipCnt -= sSkippedCnt;
            sSkippedCnt = 0;
        }
    }
    else
    {
        /* Do Nothing */
    }

    QLL_OPT_CHECK_TIME( sEndExceptTime );
    QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );
    
    /* Remain Fetch Count 수정 */
    sExeQuerySet->mRemainFetchCnt -= sFetchedCnt;
    

    STL_RAMP( RAMP_EXIT );


    /**
     * OUTPUT 설정
     */

    *aUsedBlockCnt = sFetchedCnt;
    
    /* Parent의 Fetch Node Info 설정 */
    aFetchNodeInfo->mFetchCnt -= sFetchedCnt;
    aFetchNodeInfo->mSkipCnt  -= sSkippedCnt;

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sExeQuerySet->mTargetColBlock,
                                         0,
                                         sFetchedCnt );

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeQuerySet->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeQuerySet->mCommonInfo.mFetchCallCount, 1 );
    QLL_OPT_ADD_RECORD_STAT_INFO( sExeQuerySet->mCommonInfo.mFetchRecordStat,
                                  sExeQuerySet->mCommonInfo.mResultColBlock,
                                  *aUsedBlockCnt,
                                  aEnv );

    sExeQuerySet->mCommonInfo.mFetchRowCnt += *aUsedBlockCnt;
    
    return STL_SUCCESS;

    STL_FINISH;

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeQuerySet->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeQuerySet->mCommonInfo.mFetchCallCount, 1 );

    return STL_FAILURE;
}


/**
 * @brief QUERY SET node 수행을 종료한다.
 * @param[in] aOptNode      Optimization Node
 * @param[in] aDataArea     Data Area 
 * @param[in] aEnv          Environment
 * @remarks
 */
stlStatus qlnxFinalizeQuerySet( qllOptimizationNode   * aOptNode,
                                qllDataArea           * aDataArea,
                                qllEnv                * aEnv )
{
    /* nothing to do */
    return STL_SUCCESS;
}






stlStatus qlnxRevertQuerySetTarget( qllDataArea         * aDataArea,
                                    qllOptimizationNode * aOptNode,
                                    qllEnv              * aEnv )
{
    qlnoSetOP          * sOptSetOP  = NULL;
    
    qllExecutionNode   * sExecNode     = NULL;
    qlnxQuerySet       * sExeQuerySet  = NULL;

    knlValueBlockList  * sOriginBlock = NULL;
    knlValueBlockList  * sColumnBlock = NULL;
    
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( (aOptNode->mType == QLL_PLAN_NODE_UNION_ALL_TYPE) ||
                        (aOptNode->mType == QLL_PLAN_NODE_UNION_DISTINCT_TYPE) ||
                        (aOptNode->mType == QLL_PLAN_NODE_EXCEPT_ALL_TYPE) ||
                        (aOptNode->mType == QLL_PLAN_NODE_EXCEPT_DISTINCT_TYPE) ||
                        (aOptNode->mType == QLL_PLAN_NODE_INTERSECT_ALL_TYPE) ||
                        (aOptNode->mType == QLL_PLAN_NODE_INTERSECT_DISTINCT_TYPE),
                        QLL_ERROR_STACK(aEnv) );
    
    /**
     * Query Set Node 획득
     */

    sOptSetOP = (qlnoSetOP *) aOptNode->mOptNode;

    
    sExecNode    = QLL_GET_EXECUTION_NODE( aDataArea, sOptSetOP->mRootQuerySetIdx );
    sExeQuerySet = (qlnxQuerySet *)sExecNode->mExecNode;

    /**
     * Set Column Block List 를 Origin Block List 로 원복
     */
    
    sColumnBlock = sExeQuerySet->mSetColumnBlock;
    sOriginBlock = sExeQuerySet->mOriginColumnBlock;
            
    while (sColumnBlock != NULL)
    {
        if ( sColumnBlock->mValueBlock != sOriginBlock->mValueBlock )
        {
            stlMemcpy( sColumnBlock->mValueBlock, sOriginBlock->mValueBlock, STL_SIZEOF(knlValueBlock) );
        }
        else
        {
            /* nothing to do */
        }
        
        sColumnBlock = sColumnBlock->mNext;
        sOriginBlock = sOriginBlock->mNext;
    }
    
    STL_DASSERT( sColumnBlock == NULL );
    STL_DASSERT( sOriginBlock == NULL );
     
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlnx */

