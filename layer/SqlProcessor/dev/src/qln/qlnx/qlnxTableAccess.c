/*******************************************************************************
 * qlnxTableAccess.c
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
 * @file qlnxTableAccess.c
 * @brief SQL Processor Layer Execution Node - TABLE ACCESS
 */

#include <qll.h>
#include <qlDef.h>

/**
 * @addtogroup qlnx
 * @{
 */


/**
 * @brief TABLE ACCESS node를 초기화한다.
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
stlStatus qlnxInitializeTableAccess( smlTransId              aTransID,
                                     smlStatement          * aStmt,
                                     qllDBCStmt            * aDBCStmt,
                                     qllStatement          * aSQLStmt,
                                     qllOptimizationNode   * aOptNode,
                                     qllDataArea           * aDataArea,
                                     qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode       = NULL;
    qlnoTableAccess       * sOptTableAccess = NULL;
    qlnxTableAccess       * sExeTableAccess = NULL;
    
    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_TABLE_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * TABLE ACCESS Optimization Node 획득
     */

    sOptTableAccess = (qlnoTableAccess *) aOptNode->mOptNode;


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea,
                                        QLL_GET_OPT_NODE_IDX( aOptNode ) );

    
    /**
     * 해당 노드를 포함하는 Query Expression Optimization Node 획득
     */

    /* Query Node는 없을 수도 있다. */


    /**
     * TABLE ACCESS Execution Node 획득
     */

    sExeTableAccess = (qlnxTableAccess *) sExecNode->mExecNode;


    /**
     * TABLE ACCESS Execution Node 정보 초기화
     */
    
    sExeTableAccess->mIsFirstExecute = STL_TRUE;
    sExeTableAccess->mHasFalseFilter = STL_FALSE;

    SML_INIT_ITERATOR_PROP( sExeTableAccess->mIteratorProperty );
    sExeTableAccess->mIteratorProperty.mDumpOption  = sOptTableAccess->mDumpOption;

    
    /**
     * Iterator 생성
     */

    sExeTableAccess->mIterator = NULL;
    
    STL_TRY( smlAllocIterator( aStmt,
                               sExeTableAccess->mPhysicalHandle,
                               sExeTableAccess->mTransReadMode,
                               sExeTableAccess->mStmtReadMode,
                               & sExeTableAccess->mIteratorProperty,
                               SML_SCAN_FORWARD,
                               & sExeTableAccess->mIterator,
                               SML_ENV( aEnv ) )
             == STL_SUCCESS );


    /**
     * Aggregation Distinct를 위한 Instant Table 생성
     */

    if( sOptTableAccess->mAggrDistFuncCnt > 0 )
    {
        STL_TRY( qlnxInitializeAggrDist( aTransID,
                                         aStmt,
                                         sOptTableAccess->mAggrDistFuncCnt,
                                         sExeTableAccess->mAggrDistExecInfo,
                                         aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /* Do Nothing */
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief TABLE ACCESS node를 수행한다.
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
stlStatus qlnxExecuteTableAccess( smlTransId              aTransID,
                                  smlStatement          * aStmt,
                                  qllDBCStmt            * aDBCStmt,
                                  qllStatement          * aSQLStmt,
                                  qllOptimizationNode   * aOptNode,
                                  qllDataArea           * aDataArea,
                                  qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode       = NULL;
    qlnoTableAccess       * sOptTableAccess = NULL;
    qlnxTableAccess       * sExeTableAccess = NULL;

    knlPhysicalFilter     * sPhysicalFilter = NULL;
    
    smlFetchInfo          * sFetchInfo      = NULL;

    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_TABLE_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    

    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * TABLE ACCESS Optimization Node 획득
     */

    sOptTableAccess = (qlnoTableAccess *) aOptNode->mOptNode;

    
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * TABLE ACCESS Execution Node 획득
     */

    sExeTableAccess = sExecNode->mExecNode;
    
    /**
     * 해당 노드를 포함하는 Query Expression Optimization Node 획득
     */
    
    /* Query Node는 없을 수도 있다. */


    /*****************************************
     * Node 수행 준비
     ****************************************/

    /**
     * Table Iterator 초기화
     */
    
    sFetchInfo = & sExeTableAccess->mFetchInfo;

    sFetchInfo->mIsEndOfScan = STL_FALSE;
    sFetchInfo->mSkipCnt     = 0;
    sFetchInfo->mFetchCnt    = QLL_FETCH_COUNT_MAX;

    if( sExeTableAccess->mIsFirstExecute == STL_FALSE )
    {
        STL_TRY( smlResetIterator( sExeTableAccess->mIterator,
                                   SML_ENV( aEnv ) )
                 == STL_SUCCESS );

        /**
         * Prepare Filter 수행
         */

        if( sExeTableAccess->mPreFilterEvalInfo != NULL )
        {
            STL_TRY( knlEvaluateOneExpression( sExeTableAccess->mPreFilterEvalInfo,
                                               KNL_ENV( aEnv ) )
                     == STL_SUCCESS );
        }
    }
    else
    {
        /* Do Nothing */
    }

    
    /**
     * Physical Filter의 constant value 정보 갱신
     */

    sPhysicalFilter = sFetchInfo->mPhysicalFilter;
    
    while( sPhysicalFilter != NULL )
    {
        if( sPhysicalFilter->mConstVal != NULL )
        {
            if( sPhysicalFilter->mColIdx2 == KNL_PHYSICAL_LIST_VALUE_CONSTANT )
            {
                /* Do Nothing */
            }
            else
            {
                sPhysicalFilter->mColVal2->mValue  = sPhysicalFilter->mConstVal->mValue;
                sPhysicalFilter->mColVal2->mLength = sPhysicalFilter->mConstVal->mLength;
            }
        }
        sPhysicalFilter = sPhysicalFilter->mNext;
    }


    sExeTableAccess->mIsFirstExecute = STL_FALSE;
    sExeTableAccess->mHasFalseFilter =
        qlfHasFalseFilter( sOptTableAccess->mLogicalFilterExpr,
                           aDataArea );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief TABLE ACCESS node를 Fetch한다.
 * @param[in]      aFetchNodeInfo  Fetch Node Info
 * @param[out]     aUsedBlockCnt   Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF            End Of Fetch 여부
 * @param[in]      aEnv            Environment
 *
 * @remark Fetch 수행시 호출된다.
 */
stlStatus qlnxFetchTableAccess( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                stlInt64              * aUsedBlockCnt,
                                stlBool               * aEOF,
                                qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode         = NULL;
    qlnxTableAccess       * sExeTableAccess   = NULL;
    qlnoTableAccess       * sOptTableAccess   = NULL;
    smlFetchInfo          * sFetchInfo        = NULL;
    smlAggrFuncInfo       * sAggrExecInfo     = NULL;    
    qlnfAggrDistExecInfo  * sAggrDistExecInfo = NULL;
    stlInt32                sLoop             = 0;
    stlInt64                sFetchedCnt       = 0;
    stlInt64                sSkippedCnt       = 0;
    qlnxFetchNodeInfo       sLocalFetchInfo;

    QLL_OPT_DECLARE( stlTime sBeginFetchTime );
    QLL_OPT_DECLARE( stlTime sEndFetchTime );

    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aFetchNodeInfo->mOptNode->mType == QLL_PLAN_NODE_TABLE_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    QLL_OPT_CHECK_TIME( sBeginFetchTime );

    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * TABLE ACCESS Optimization Node 획득
     */

    sOptTableAccess = (qlnoTableAccess *) aFetchNodeInfo->mOptNode->mOptNode;
    
    
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aFetchNodeInfo->mDataArea,
                                        QLL_GET_OPT_NODE_IDX( aFetchNodeInfo->mOptNode ) );


    /**
     * TABLE ACCESS Execution Node 획득
     */

    sExeTableAccess = sExecNode->mExecNode;

    
    /**
     * 해당 노드를 포함하는 Query Expression Optimization Node 획득
     */
    
    /* Query Node는 없을 수도 있다. */


    /**
     * Used Block Count 초기화
     */
    
    *aUsedBlockCnt = 0;
    
    
    /**
     * Node 수행 여부 확인 
     */

    STL_DASSERT( *aEOF == STL_FALSE );
    STL_DASSERT( sExeTableAccess->mFetchInfo.mIsEndOfScan == STL_FALSE );
    STL_DASSERT( aFetchNodeInfo->mFetchCnt > 0);
    STL_DASSERT( sExeTableAccess->mIterator != NULL );


    /**
     * Local Fetch Info 초기화
     */
    
    QLNX_SET_LOCAL_FETCH_INFO( & sLocalFetchInfo, aFetchNodeInfo );

    
    /*****************************************
     * Node 수행 여부 확인
     ****************************************/

    sFetchInfo = & sExeTableAccess->mFetchInfo;

    if( ( sOptTableAccess->mAggrFuncCnt > 0 ) ||
        ( sOptTableAccess->mAggrDistFuncCnt > 0 ) )
    {
        if( aFetchNodeInfo->mSkipCnt > 0 )
        {
            sFetchedCnt = 0;
            sSkippedCnt = 1;
        }
        else
        {
            sFetchedCnt = 1;
            sSkippedCnt = 0;
        }
    }
    else
    {
        /* Do Nothing */
    }

    if( sExeTableAccess->mHasFalseFilter == STL_TRUE )
    {
        if( sOptTableAccess->mAggrFuncCnt > 0 )
        {
            sAggrExecInfo = sFetchInfo->mAggrFetchInfo;
            
            STL_TRY( qlnxInitAggrFunc( sAggrExecInfo,
                                       aEnv )
                     == STL_SUCCESS );
            
            for( sLoop = 0 ; sLoop < sOptTableAccess->mAggrFuncCnt ; sLoop++ )
            {
                KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sAggrExecInfo->mAggrValueList, 0, sFetchedCnt );
                sAggrExecInfo++;
            }
        }
        else if( sOptTableAccess->mAggrDistFuncCnt > 0 )
        {
            sAggrDistExecInfo = sExeTableAccess->mAggrDistExecInfo;
            for( sLoop = 0 ; sLoop < sOptTableAccess->mAggrDistFuncCnt ; sLoop++ )
            {
                STL_TRY( qlnxInitAggrFunc( sAggrDistExecInfo->mFetchInfo.mAggrFetchInfo,
                                           aEnv )
                         == STL_SUCCESS );

                KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sAggrDistExecInfo->mAggrValueList, 0, sFetchedCnt );
                sAggrDistExecInfo++;
            }
        }

        sExeTableAccess->mFetchInfo.mIsEndOfScan = STL_TRUE;
        *aEOF = STL_TRUE;
        STL_THROW( RAMP_EXIT );
    }
    else
    {
        /* Do Nothing */
    }


    /*****************************************
     * Aggregation Fetch 수행
     ****************************************/

    /**
     * Distinct 조건이 포함된 Aggregation fetch 수행
     */
    
    if( sOptTableAccess->mAggrDistFuncCnt > 0 )
    {
        sExeTableAccess->mFetchInfo.mSkipCnt  = aFetchNodeInfo->mSkipCnt;
        sExeTableAccess->mFetchInfo.mFetchCnt = 1;
        
        STL_TRY( qlnxFetchAggrDist( sLocalFetchInfo.mStmt,
                                    sExeTableAccess->mIterator,
                                    & sExeTableAccess->mFetchInfo,
                                    sOptTableAccess->mAggrFuncCnt,
                                    sOptTableAccess->mAggrDistFuncCnt,
                                    sExeTableAccess->mAggrExecInfo,
                                    sExeTableAccess->mAggrDistExecInfo,
                                    & sFetchedCnt,
                                    aEOF,
                                    aEnv )
                 == STL_SUCCESS );
        
        STL_DASSERT( *aEOF == STL_TRUE );
        STL_THROW( RAMP_EXIT );
    }
    else
    {
        /* Do Nothing */
    }

    
    /*****************************************
     * Fetch 수행
     ****************************************/

    /**
     * Block Read
     */

    sFetchInfo = & sExeTableAccess->mFetchInfo;
    
    if( sOptTableAccess->mAggrFuncCnt > 0 )
    {
        sFetchInfo->mSkipCnt  = 0;
        sFetchInfo->mFetchCnt = 1;

        STL_TRY( qlnxInitAggrFunc( sFetchInfo->mAggrFetchInfo,
                                   aEnv )
                 == STL_SUCCESS );

        STL_TRY( smlFetchAggr( sExeTableAccess->mIterator,
                               sFetchInfo,
                               SML_ENV(aEnv) )
                 == STL_SUCCESS );

        sAggrExecInfo = sFetchInfo->mAggrFetchInfo;
    
        for( sLoop = 0 ; sLoop < sOptTableAccess->mAggrFuncCnt ; sLoop++ )
        {
            KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sAggrExecInfo->mAggrValueList, 0, sFetchedCnt );
            sAggrExecInfo = sAggrExecInfo->mNext;
        }

        *aEOF = STL_TRUE;
    }
    else
    {
        sFetchInfo->mSkipCnt  = aFetchNodeInfo->mSkipCnt;
        sFetchInfo->mFetchCnt = aFetchNodeInfo->mFetchCnt;
        
        STL_TRY( smlFetchNext( sExeTableAccess->mIterator,
                               sFetchInfo,
                               SML_ENV(aEnv) )
                 == STL_SUCCESS );
        
        sFetchedCnt = SML_USED_BLOCK_SIZE( sFetchInfo->mRowBlock );
        sSkippedCnt = aFetchNodeInfo->mSkipCnt - sFetchInfo->mSkipCnt;

        *aEOF = sFetchInfo->mIsEndOfScan;
    }
    
    
    STL_RAMP( RAMP_EXIT );

    
    /**
     * OUTPUT 설정
     */

    *aUsedBlockCnt = sFetchedCnt;
    
    /* Parent의 Fetch Node Info 설정 */
    aFetchNodeInfo->mFetchCnt -= sFetchedCnt;
    aFetchNodeInfo->mSkipCnt  -= sSkippedCnt;
    

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sExeTableAccess->mCommonInfo.mFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_COUNT( sExeTableAccess->mCommonInfo.mFetchCallCount, 1 );
    QLL_OPT_ADD_RECORD_STAT_INFO( sExeTableAccess->mCommonInfo.mFetchRecordStat,
                                  sExeTableAccess->mCommonInfo.mResultColBlock,
                                  *aUsedBlockCnt,
                                  aEnv );

    sExeTableAccess->mCommonInfo.mFetchRowCnt += *aUsedBlockCnt;
    
    return STL_SUCCESS;

    STL_FINISH;

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sExeTableAccess->mCommonInfo.mFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_COUNT( sExeTableAccess->mCommonInfo.mFetchCallCount, 1 );

    return STL_FAILURE;
}


/**
 * @brief TABLE ACCESS node 수행을 종료한다.
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 *
 * @remark Execution을 종료하고자 할 때 호출한다.
 */
stlStatus qlnxFinalizeTableAccess( qllOptimizationNode   * aOptNode,
                                   qllDataArea           * aDataArea,
                                   qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode         = NULL;
    qlnoTableAccess       * sOptTableAccess   = NULL;
    qlnxTableAccess       * sExeTableAccess   = NULL;
    

    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_TABLE_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * TABLE ACCESS Optimization Node 획득
     */

    sOptTableAccess = (qlnoTableAccess * ) aOptNode->mOptNode;
        

    /**
     * TABLE ACCESS Execution Node 획득
     */

    sExeTableAccess = sExecNode->mExecNode;
    

    /**
     * Iterator 해제
     */

    if( sExeTableAccess->mIterator != NULL )
    {
        (void) smlFreeIterator( sExeTableAccess->mIterator, SML_ENV(aEnv) );
        sExeTableAccess->mIterator = NULL;
    }
    else
    {
        /* Do Nothing */
    }

    
    /**
     * Aggregation Distinct를 위한 Instant Table 해제
     */

    if( sOptTableAccess->mAggrDistFuncCnt > 0 )
    {
        STL_TRY( qlnxFinalizeAggrDist( sOptTableAccess->mAggrDistFuncCnt,
                                       sExeTableAccess->mAggrDistExecInfo,
                                       aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /* Do Nothing */
    }
    

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlnx */
