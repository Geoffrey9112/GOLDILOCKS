/*******************************************************************************
 * qlnxAggregation.c
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
 * @file qlnxAggregation.c
 * @brief SQL Processor Layer Execution Node - HASH AGGREGATION
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnx
 * @{
 */


/**
 * @brief HASH AGGREGATION node를 초기화한다.
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
stlStatus qlnxInitializeHashAggregation( smlTransId              aTransID,
                                         smlStatement          * aStmt,
                                         qllDBCStmt            * aDBCStmt,
                                         qllStatement          * aSQLStmt,
                                         qllOptimizationNode   * aOptNode,
                                         qllDataArea           * aDataArea,
                                         qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode       = NULL;
    qlnoAggregation       * sOptAggregation = NULL;
    qlnxAggregation       * sExeAggregation = NULL;
    
    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_HASH_AGGREGATION_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * AGGREGATION Optimization Node 획득
     */

    sOptAggregation = (qlnoAggregation *) aOptNode->mOptNode;


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea,
                                        QLL_GET_OPT_NODE_IDX( aOptNode ) );

    
    /**
     * AGGREGATION Execution Node 획득
     */

    sExeAggregation = (qlnxAggregation *) sExecNode->mExecNode;


    /**
     * AGGREGATION Execution Node 정보 초기화
     */

    sExeAggregation->mIsNeedBuild = STL_TRUE;
    
    
    /**
     * Aggregation Distinct를 위한 Instant Table 생성
     */

    if( sOptAggregation->mAggrDistFuncCnt > 0 )
    {
        STL_TRY( qlnxInitializeAggrDist( aTransID,
                                         aStmt,
                                         sOptAggregation->mAggrDistFuncCnt,
                                         sExeAggregation->mAggrDistExecInfo,
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
 * @brief HASH AGGREGATION node를 수행한다.
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
stlStatus qlnxExecuteHashAggregation( smlTransId              aTransID,
                                      smlStatement          * aStmt,
                                      qllDBCStmt            * aDBCStmt,
                                      qllStatement          * aSQLStmt,
                                      qllOptimizationNode   * aOptNode,
                                      qllDataArea           * aDataArea,
                                      qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode       = NULL;
    qlnoAggregation       * sOptAggregation = NULL;
    qlnxAggregation       * sExeAggregation = NULL;


    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_HASH_AGGREGATION_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * AGGREGATION Optimization Node 획득
     */

    sOptAggregation = (qlnoAggregation *) aOptNode->mOptNode;


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea,
                                        QLL_GET_OPT_NODE_IDX( aOptNode ) );

    
    /**
     * AGGREGATION Execution Node 획득
     */

    sExeAggregation = (qlnxAggregation *) sExecNode->mExecNode;


    /**
     * AGGREGATION Execution Node 정보 초기화
     */

    sExeAggregation->mIsNeedBuild = STL_TRUE;


    /*****************************************
     * Child Node Execution
     ****************************************/

    /**
     * 하위 노드 Execute
     */

    QLNX_EXECUTE_NODE( aTransID,
                       aStmt,
                       aDBCStmt,
                       aSQLStmt,
                       aDataArea->mExecNodeList[sOptAggregation->mChildNode->mIdx].mOptNode,
                       aDataArea,
                       aEnv );

    /* Do Nothing */
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief HASH AGGREGATION node를 Fetch한다.
 * @param[in]      aFetchNodeInfo  Fetch Node Info
 * @param[out]     aUsedBlockCnt   Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF            End Of Fetch 여부
 * @param[in]      aEnv            Environment
 *
 * @remark Fetch 수행시 호출된다.
 */
stlStatus qlnxFetchHashAggregation( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                    stlInt64              * aUsedBlockCnt,
                                    stlBool               * aEOF,
                                    qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode       = NULL;
    qlnxAggregation       * sExeAggregation = NULL;
    qlnoAggregation       * sOptAggregation = NULL;

    qlnfAggrExecInfo      * sAggrExecInfo     = NULL;
    qlnfAggrDistExecInfo  * sAggrDistExecInfo = NULL;
    smlFetchInfo          * sFetchInfo        = NULL;
    
    stlInt64                sReadCnt          = 0;
    stlInt32                sArgIdx           = 0;
    stlInt32                sLoop             = 0;
    stlBool                 sEOF              = STL_FALSE;
    stlInt64                sFetchedCnt       = 0;
    stlInt64                sSkippedCnt       = 0;
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

    STL_PARAM_VALIDATE( aFetchNodeInfo->mOptNode->mType == QLL_PLAN_NODE_HASH_AGGREGATION_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    QLL_OPT_CHECK_TIME( sBeginFetchTime );

    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * AGGREGATION Optimization Node 획득
     */

    sOptAggregation = (qlnoAggregation *) aFetchNodeInfo->mOptNode->mOptNode;
    
    
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aFetchNodeInfo->mDataArea,
                                        QLL_GET_OPT_NODE_IDX( aFetchNodeInfo->mOptNode ) );


    /**
     * AGGREGATION Execution Node 획득
     */

    sExeAggregation = sExecNode->mExecNode;

    
    /**
     * 해당 노드를 포함하는 Query Expression Optimization Node 획득
     */
    
    /* Query Node는 없을 수도 있다. */

    
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
     * Aggregation Fetch 수행
     ****************************************/

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

    STL_TRY_THROW( sExeAggregation->mIsNeedBuild == STL_TRUE, RAMP_EXIT );

    sAggrExecInfo = sExeAggregation->mAggrExecInfo;
    for( sLoop = 0 ; sLoop < sOptAggregation->mAggrFuncCnt ; sLoop++ )
    {
        STL_TRY( gQlnfAggrFuncInfoArr[sAggrExecInfo->mAggrOptInfo->mAggrFuncId].mInitFuncPtr(
                     sAggrExecInfo,
                     0, /* result block idx */
                     aEnv )
                 == STL_SUCCESS );
        sAggrExecInfo++;
    }

    while( sEOF == STL_FALSE )
    {
        QLL_OPT_CHECK_TIME( sBeginExceptTime );

        /* fetch records*/
        sReadCnt = 0;
        QLNX_FETCH_NODE( & sLocalFetchInfo,
                         sOptAggregation->mChildNode->mIdx,
                         0, /* Start Idx */
                         0, /* Skip Count */
                         QLL_FETCH_COUNT_MAX, /* Fetch Count */
                         & sReadCnt,
                         & sEOF,
                         aEnv );

        QLL_OPT_CHECK_TIME( sEndExceptTime );
        QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

        if( sReadCnt > 0 )
        {
            /*****************************************
             * Aggregation 수행 (No Distinct)
             ****************************************/

            if( sExeAggregation->mAggrExecInfo != NULL )
            {
                sAggrExecInfo = sExeAggregation->mAggrExecInfo;

                for( sLoop = 0 ; sLoop < sOptAggregation->mAggrFuncCnt ; sLoop++ )
                {
                    STL_TRY( gQlnfAggrFuncInfoArr[sAggrExecInfo->mAggrOptInfo->mAggrFuncId].mBuildFuncPtr(
                                 sAggrExecInfo,
                                 0, /* start block idx */
                                 sReadCnt,
                                 0, /* result block idx */
                                 aEnv )
                             == STL_SUCCESS );
                    sAggrExecInfo++;
                }
            }
            else
            {
                /* Do Nothing */
            }

            
            /*****************************************
             * Aggregation Distinct 수행
             ****************************************/

            /**
             * Aggregation Distinct를 위해 Instant Table에 Record 삽입
             */

            sAggrDistExecInfo = sExeAggregation->mAggrDistExecInfo;

        
            /**
             * Evaluate Column Expression Stack
             */

            for( sLoop = 0 ; sLoop < sOptAggregation->mAggrDistFuncCnt ; sLoop++ )
            {
                /**
                 * Evaluate Column Expression Stack
                 */

                for( sArgIdx = 0 ; sArgIdx < sAggrDistExecInfo->mInputArgCnt ; sArgIdx++ )
                {
                    if( sAggrDistExecInfo->mEvalInfo[ sArgIdx ] != NULL )
                    {
                        sAggrDistExecInfo->mEvalInfo[ sArgIdx ]->mBlockCount = sReadCnt;
                        STL_TRY( knlEvaluateBlockExpression( sAggrDistExecInfo->mEvalInfo[ sArgIdx ],
                                                             KNL_ENV( aEnv ) )
                                 == STL_SUCCESS );
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sAggrDistExecInfo->mInsertColList, 0, sReadCnt );
                }

            
                /**
                 * Instant Table에 Record 삽입
                 */

                STL_TRY( smlInsertRecord( sLocalFetchInfo.mStmt,
                                          sAggrDistExecInfo->mInstantTable.mTableHandle,
                                          sAggrDistExecInfo->mInsertColList,
                                          NULL, /* unique violation */
                                          sAggrDistExecInfo->mFetchInfo.mRowBlock,
                                          SML_ENV( aEnv ) )
                         == STL_SUCCESS );

                sAggrDistExecInfo++;
            }
        }
    }
    

    /**
     * Iterator Allocation
     */
        
    sAggrDistExecInfo = sExeAggregation->mAggrDistExecInfo;
            
    for( sLoop = 0 ; sLoop < sOptAggregation->mAggrDistFuncCnt ; sLoop++ )
    {
        SML_INIT_ITERATOR_PROP( sAggrDistExecInfo->mInstantTable.mIteratorProperty );
            
        if( sAggrDistExecInfo->mInstantTable.mIterator != NULL )
        {
            STL_TRY_THROW( smlFreeIterator( sAggrDistExecInfo->mInstantTable.mIterator,
                                            SML_ENV( aEnv ) )
                           == STL_SUCCESS, ERR_FREE_ITERATOR );
            
            sAggrDistExecInfo->mInstantTable.mIterator = NULL;
        }
        
        STL_TRY( smlAllocIterator( sLocalFetchInfo.mStmt,
                                   sAggrDistExecInfo->mInstantTable.mTableHandle,
                                   SML_TRM_SNAPSHOT,
                                   SML_SRM_SNAPSHOT,
                                   & sAggrDistExecInfo->mInstantTable.mIteratorProperty,
                                   SML_SCAN_FORWARD,
                                   & sAggrDistExecInfo->mInstantTable.mIterator,
                                   SML_ENV( aEnv ) )
                 == STL_SUCCESS );

        sAggrDistExecInfo->mInstantTable.mIsNeedBuild = STL_FALSE;
        sAggrDistExecInfo++;
    }


    /*****************************************
     * Aggregation Fetch 수행
     ****************************************/

    sAggrDistExecInfo = sExeAggregation->mAggrDistExecInfo;
    for( sLoop = 0 ; sLoop < sOptAggregation->mAggrDistFuncCnt ; sLoop++ )
    {
        sFetchInfo = & sAggrDistExecInfo->mFetchInfo;
        sFetchInfo->mSkipCnt  = 0;
        sFetchInfo->mFetchCnt = 1;
        
        STL_TRY( qlnxInitAggrFunc( sFetchInfo->mAggrFetchInfo,
                                   aEnv )
                 == STL_SUCCESS );

        STL_TRY( smlFetchAggr( sAggrDistExecInfo->mInstantTable.mIterator,
                               sFetchInfo,
                               SML_ENV(aEnv) )
                 == STL_SUCCESS );

        KNL_SET_ALL_BLOCK_USED_CNT( sAggrDistExecInfo->mAggrValueList, sFetchedCnt );
    
        sAggrDistExecInfo++;
    }

    sExeAggregation->mIsNeedBuild = STL_FALSE;

    STL_RAMP( RAMP_EXIT );
    

    /**
     * OUTPUT 설정
     */
    
    *aUsedBlockCnt = sFetchedCnt;
    
    /* Parent의 Fetch Node Info 설정 */
    aFetchNodeInfo->mFetchCnt -= sFetchedCnt;
    aFetchNodeInfo->mSkipCnt  -= sSkippedCnt;

    *aEOF = STL_TRUE;


    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeAggregation->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeAggregation->mCommonInfo.mFetchCallCount, 1 );
    QLL_OPT_ADD_RECORD_STAT_INFO( sExeAggregation->mCommonInfo.mFetchRecordStat,
                                  sExeAggregation->mCommonInfo.mResultColBlock,
                                  *aUsedBlockCnt,
                                  aEnv );

    sExeAggregation->mCommonInfo.mFetchRowCnt += *aUsedBlockCnt;

    return STL_SUCCESS;

    STL_CATCH( ERR_FREE_ITERATOR )
    {
        sAggrDistExecInfo->mInstantTable.mIterator = NULL;
    }
    
    STL_FINISH;

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeAggregation->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeAggregation->mCommonInfo.mFetchCallCount, 1 );

    return STL_FAILURE;
}


/**
 * @brief HASH AGGREGATION node 수행을 종료한다.
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 *
 * @remark Execution을 종료하고자 할 때 호출한다.
 */
stlStatus qlnxFinalizeHashAggregation( qllOptimizationNode   * aOptNode,
                                       qllDataArea           * aDataArea,
                                       qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode         = NULL;
    qlnoAggregation       * sOptAggregation   = NULL;
    qlnxAggregation       * sExeAggregation   = NULL;
    

    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_HASH_AGGREGATION_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * TABLE ACCESS Optimization Node 획득
     */

    sOptAggregation = (qlnoAggregation * ) aOptNode->mOptNode;
        

    /**
     * TABLE ACCESS Execution Node 획득
     */

    sExeAggregation = sExecNode->mExecNode;


    /**
     * Aggregation Distinct를 위한 Instant Table 해제
     */

    if( sOptAggregation->mAggrDistFuncCnt > 0 )
    {
        STL_TRY( qlnxFinalizeAggrDist( sOptAggregation->mAggrDistFuncCnt,
                                       sExeAggregation->mAggrDistExecInfo,
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
 * @brief node의 Aggregation Distinct를 Init한다.
 * @param[in]      aTransID      Transaction ID
 * @param[in]      aStmt         Statement
 * @param[in]      aAggrDistCnt  Aggregation Distinct Count
 * @param[in]      aAggrDistInfo Aggregation Distinct Info
 * @param[in]      aEnv          Environment
 */
stlStatus qlnxInitializeAggrDist( smlTransId              aTransID,
                                  smlStatement          * aStmt,
                                  stlInt32                aAggrDistCnt,
                                  qlnfAggrDistExecInfo  * aAggrDistInfo,
                                  qllEnv                * aEnv )
{
    qlnfAggrDistExecInfo  * sAggrDistExecInfo = NULL;
    stlInt32                sLoop             = 0;
    smlIndexAttr            sHashTableAttr;


    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAggrDistInfo != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**
     * Hash Attr 셋팅
     */
    
    stlMemset( &sHashTableAttr, 0x00, STL_SIZEOF( smlIndexAttr ) );

    /* Logging 하지 않음*/
    sHashTableAttr.mValidFlags |= SML_INDEX_LOGGING_YES;
    sHashTableAttr.mLoggingFlag = STL_FALSE;

    /* Uniqueness 여부 */
    sHashTableAttr.mValidFlags |= SML_INDEX_UNIQUENESS_YES;
    sHashTableAttr.mUniquenessFlag = STL_TRUE;
    
    /* Primary Key 여부 */
    sHashTableAttr.mPrimaryFlag = STL_FALSE;

  
    /**
     * Hash Instant로 구성
     */

    sAggrDistExecInfo = aAggrDistInfo;

    for( sLoop = 0 ; sLoop < aAggrDistCnt ; sLoop++ )
    {
        stlMemset( & sAggrDistExecInfo->mInstantTable, 0x00, STL_SIZEOF( qlnInstantTable ) );

        STL_TRY( qlnxCreateHashInstantTable( aTransID,
                                             aStmt,
                                             & sAggrDistExecInfo->mInstantTable,
                                             NULL,
                                             sAggrDistExecInfo->mInputArgCnt,
                                             sAggrDistExecInfo->mInputArgList,
                                             & sHashTableAttr,
                                             aEnv )
                 == STL_SUCCESS );

        sAggrDistExecInfo++;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief node의 Aggregation Distinct를 Fetch한다.
 * @param[in]      aStmt         Statement
 * @param[in]      aIterator     Access Node Iterator
 * @param[in]      aFetchInfo    Access Node Fetch Info
 * @param[in]      aAggrCnt      Aggregation Count
 * @param[in]      aAggrDistCnt  Aggregation Distinct Count
 * @param[in]      aAggrInfo     Aggregation Info (No Distinct)
 * @param[in]      aAggrDistInfo Aggregation Distinct Info
 * @param[out]     aUsedBlockCnt Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF          End Of Fetch 여부
 * @param[in]      aEnv          Environment
 */
stlStatus qlnxFetchAggrDist( smlStatement          * aStmt,
                             void                  * aIterator,
                             smlFetchInfo          * aFetchInfo,
                             stlInt32                aAggrCnt,
                             stlInt32                aAggrDistCnt,
                             qlnfAggrExecInfo      * aAggrInfo,
                             qlnfAggrDistExecInfo  * aAggrDistInfo,
                             stlInt64              * aUsedBlockCnt,
                             stlBool               * aEOF,
                             qllEnv                * aEnv )
{
    qlnfAggrExecInfo      * sAggrExecInfo     = NULL;
    qlnfAggrDistExecInfo  * sAggrDistExecInfo = NULL;
    smlFetchInfo          * sFetchInfo        = NULL;

    stlInt64                sReadCnt          = 0;
    stlInt32                sArgIdx           = 0;
    stlInt32                sLoop             = 0;
    stlInt64                sAggrResCnt       = 0;
    

    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIterator != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aFetchInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aUsedBlockCnt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aEOF != NULL, QLL_ERROR_STACK(aEnv) );


    STL_DASSERT( *aEOF == STL_FALSE );
    
    /*****************************************
     * OUTPUT 정보 초기화
     ****************************************/
    
    *aUsedBlockCnt = 0;

    STL_TRY_THROW( aFetchInfo->mIsEndOfScan == STL_FALSE, RAMP_EXIT );
    

    /*****************************************
     * Aggregation 수행
     ****************************************/

    sAggrResCnt = ( aFetchInfo->mSkipCnt > 0 ? 0 : 1 );
    
    aFetchInfo->mSkipCnt  = 0;
    aFetchInfo->mFetchCnt = QLL_FETCH_COUNT_MAX;

    sAggrExecInfo = aAggrInfo;
    for( sLoop = 0 ; sLoop < aAggrCnt ; sLoop++ )
    {
        STL_TRY( gQlnfAggrFuncInfoArr[sAggrExecInfo->mAggrOptInfo->mAggrFuncId].mInitFuncPtr(
                     sAggrExecInfo,
                     0, /* result block idx */
                     aEnv )
                 == STL_SUCCESS );
        sAggrExecInfo++;
    }


    while( aFetchInfo->mIsEndOfScan == STL_FALSE )
    {
        /*****************************************
         * FETCH
         ****************************************/

        /* Fetch Next */
        STL_TRY( smlFetchNext( aIterator,
                               aFetchInfo,
                               SML_ENV(aEnv) )
                 == STL_SUCCESS );

        sReadCnt = aFetchInfo->mRowBlock->mUsedBlockSize;

        if( sReadCnt == 0 )
        {
            STL_DASSERT( aFetchInfo->mIsEndOfScan == STL_TRUE );
            break;
        }
        else
        {
            /* Do Nothing */
        }


        /*****************************************
         * Aggregation 수행 (No Distinct)
         ****************************************/

        if( aAggrInfo != NULL )
        {
            sAggrExecInfo = aAggrInfo;

            for( sLoop = 0 ; sLoop < aAggrCnt ; sLoop++ )
            {
                STL_TRY( gQlnfAggrFuncInfoArr[sAggrExecInfo->mAggrOptInfo->mAggrFuncId].mBuildFuncPtr(
                             sAggrExecInfo,
                             0, /* start block idx */
                             sReadCnt,
                             0, /* result block idx */
                             aEnv )
                         == STL_SUCCESS );
                sAggrExecInfo++;
            }
        }
        else
        {
            /* Do Nothing */
        }
        

        /*****************************************
         * Aggregation Distinct 수행
         ****************************************/

        /**
         * Aggregation Distinct를 위해 Instant Table에 Record 삽입
         */

        sAggrDistExecInfo = aAggrDistInfo;

        
        /**
         * Evaluate Column Expression Stack
         */

        for( sLoop = 0 ; sLoop < aAggrDistCnt ; sLoop++ )
        {
            /**
             * Evaluate Column Expression Stack
             */

            for( sArgIdx = 0 ; sArgIdx < sAggrDistExecInfo->mInputArgCnt ; sArgIdx++ )
            {
                if( sAggrDistExecInfo->mEvalInfo[ sArgIdx ] != NULL )
                {
                    sAggrDistExecInfo->mEvalInfo[ sArgIdx ]->mBlockCount = sReadCnt;
                    STL_TRY( knlEvaluateBlockExpression( sAggrDistExecInfo->mEvalInfo[ sArgIdx ],
                                                         KNL_ENV( aEnv ) )
                             == STL_SUCCESS );
                }
                else
                {
                    /* Do Nothing */
                }
                KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sAggrDistExecInfo->mInsertColList, 0, sReadCnt );
            }

            
            /**
             * Instant Table에 Record 삽입
             */

            STL_TRY( smlInsertRecord( aStmt,
                                      sAggrDistExecInfo->mInstantTable.mTableHandle,
                                      sAggrDistExecInfo->mInsertColList,
                                      NULL, /* unique violation */
                                      aFetchInfo->mRowBlock,
                                      SML_ENV( aEnv ) )
                     == STL_SUCCESS );

            sAggrDistExecInfo++;
        }
    }
        
        
    /**
     * Iterator Allocation
     */
        
    sAggrDistExecInfo = aAggrDistInfo;
            
    for( sLoop = 0 ; sLoop < aAggrDistCnt ; sLoop++ )
    {
        SML_INIT_ITERATOR_PROP( sAggrDistExecInfo->mInstantTable.mIteratorProperty );
            
        if( sAggrDistExecInfo->mInstantTable.mIterator != NULL )
        {
            STL_TRY_THROW( smlFreeIterator( sAggrDistExecInfo->mInstantTable.mIterator,
                                      SML_ENV( aEnv ) )
                           == STL_SUCCESS, ERR_FREE_ITERATOR );

            sAggrDistExecInfo->mInstantTable.mIterator = NULL;
        }

        STL_TRY( smlAllocIterator( aStmt,
                                   sAggrDistExecInfo->mInstantTable.mTableHandle,
                                   SML_TRM_SNAPSHOT,
                                   SML_SRM_SNAPSHOT,
                                   & sAggrDistExecInfo->mInstantTable.mIteratorProperty,
                                   SML_SCAN_FORWARD,
                                   & sAggrDistExecInfo->mInstantTable.mIterator,
                                   SML_ENV( aEnv ) )
                 == STL_SUCCESS );

        sAggrDistExecInfo->mInstantTable.mIsNeedBuild = STL_FALSE;
        sAggrDistExecInfo++;
    }
    

    /*****************************************
     * Aggregation Fetch 수행
     ****************************************/

    sAggrDistExecInfo = aAggrDistInfo;
    for( sLoop = 0 ; sLoop < aAggrDistCnt ; sLoop++ )
    {
        sFetchInfo = & sAggrDistExecInfo->mFetchInfo;
        sFetchInfo->mSkipCnt  = 0;
        sFetchInfo->mFetchCnt = 1;
        
        STL_TRY( qlnxInitAggrFunc( sFetchInfo->mAggrFetchInfo,
                                   aEnv )
                 == STL_SUCCESS );

        STL_TRY( smlFetchAggr( sAggrDistExecInfo->mInstantTable.mIterator,
                               sFetchInfo,
                               SML_ENV(aEnv) )
                 == STL_SUCCESS );

        KNL_SET_ALL_BLOCK_USED_CNT( sAggrDistExecInfo->mAggrValueList, sAggrResCnt );
    
        sAggrDistExecInfo++;
    }

    STL_RAMP( RAMP_EXIT );

    *aUsedBlockCnt = sAggrResCnt;
    *aEOF = STL_TRUE;
    
    return STL_SUCCESS;

    STL_CATCH( ERR_FREE_ITERATOR )
    {
        sAggrDistExecInfo->mInstantTable.mIterator = NULL;
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief node의 Aggregation Distinct를 Finalize한다.
 * @param[in]      aAggrDistCnt  Aggregation Distinct Count
 * @param[in]      aAggrDistInfo Aggregation Distinct Info
 * @param[in]      aEnv          Environment
 */
stlStatus qlnxFinalizeAggrDist( stlInt32                aAggrDistCnt,
                                qlnfAggrDistExecInfo  * aAggrDistInfo,
                                qllEnv                * aEnv )
{
    qlnfAggrDistExecInfo  * sAggrDistExecInfo = NULL;
    stlInt32                sLoop             = 0;


    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aAggrDistInfo != NULL, QLL_ERROR_STACK(aEnv) );


    sAggrDistExecInfo = aAggrDistInfo;
    
    for( sLoop = 0 ; sLoop < aAggrDistCnt ; sLoop++ )
    {
        STL_TRY( qlnxDropInstantTable( & sAggrDistExecInfo->mInstantTable,
                                       aEnv )
                 == STL_SUCCESS );

        sAggrDistExecInfo++;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief node의 Aggregation Distinct를 위한 Conflict Check를 Init한다.
 * @param[in]      aTransID            Transaction ID
 * @param[in]      aStmt               Statement
 * @param[in]      aAggrDistCnt        Aggregation Distinct Count
 * @param[in]      aConflictCheckInfo  Conflict Check Info
 * @param[in]      aEnv                Environment
 */
stlStatus qlnxInitializeAggrConflictCheck( smlTransId              aTransID,
                                           smlStatement          * aStmt,
                                           stlInt32                aAggrDistCnt,
                                           qlnxConflictCheckInfo * aConflictCheckInfo,
                                           qllEnv                * aEnv )
{
    qlnxConflictCheckInfo * sConflictCheckInfo = NULL;
    stlInt32                sLoop              = 0;
    smlIndexAttr            sHashTableAttr;


    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConflictCheckInfo != NULL, QLL_ERROR_STACK(aEnv) );
    
    
    /**
     * Hash Attr 셋팅
     */
    
    stlMemset( &sHashTableAttr, 0x00, STL_SIZEOF( smlIndexAttr ) );

    /* Logging 하지 않음*/
    sHashTableAttr.mValidFlags |= SML_INDEX_LOGGING_YES;
    sHashTableAttr.mLoggingFlag = STL_FALSE;

    /* Uniqueness 여부 */
    sHashTableAttr.mValidFlags |= SML_INDEX_UNIQUENESS_YES;
    sHashTableAttr.mUniquenessFlag = STL_TRUE;
    
    /* Primary Key 여부 */
    sHashTableAttr.mPrimaryFlag = STL_FALSE;


    /**
     * Hash Instant로 구성
     */

    sConflictCheckInfo = aConflictCheckInfo;

    for( sLoop = 0 ; sLoop < aAggrDistCnt ; sLoop++ )
    {
        stlMemset( & sConflictCheckInfo->mInstantTable, 0x00, STL_SIZEOF( qlnInstantTable ) );
        
        STL_TRY( qlnxCreateHashInstantTable( aTransID,
                                             aStmt,
                                             & sConflictCheckInfo->mInstantTable,
                                             NULL,
                                             sConflictCheckInfo->mInsertColCnt,
                                             sConflictCheckInfo->mInsertColList,
                                             & sHashTableAttr,
                                             aEnv )
                 == STL_SUCCESS );

        sConflictCheckInfo++;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief node의 Aggregation Distinct를 위한 Conflict Check를 Finalize한다.
 * @param[in]      aAggrDistCnt        Aggregation Distinct Count
 * @param[in]      aConflictCheckInfo  Conflict Check Info
 * @param[in]      aEnv                Environment
 */
stlStatus qlnxFinalizeAggrConflictCheck( stlInt32                aAggrDistCnt,
                                         qlnxConflictCheckInfo * aConflictCheckInfo,
                                         qllEnv                * aEnv )
{
    qlnxConflictCheckInfo * sConflictCheckInfo = NULL;
    stlInt32                sLoop              = 0;


    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aConflictCheckInfo != NULL, QLL_ERROR_STACK(aEnv) );


    sConflictCheckInfo = aConflictCheckInfo;
    
    for( sLoop = 0 ; sLoop < aAggrDistCnt ; sLoop++ )
    {
        STL_TRY( qlnxDropInstantTable( & sConflictCheckInfo->mInstantTable,
                                       aEnv )
                 == STL_SUCCESS );

        sConflictCheckInfo++;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Aggregation Function Info의 Aggregation 값을 Initialize한다.
 * @param[in,out]  aAggrExecInfo       Aggregation Execution Info for SM
 * @param[in]      aEnv                Environment
 */
stlStatus qlnxInitAggrFunc( smlAggrFuncInfo   * aAggrExecInfo,
                            qllEnv            * aEnv )
{
    smlAggrFuncInfo   * sAggrExecInfo    = aAggrExecInfo;
    qlnfAggrExecInfo    sTmpAggrExecInfo = { 0, NULL, NULL, NULL, NULL };
    
    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**
     * Aggregation Function들에 대한 Initialize
     */
    
    while( sAggrExecInfo != NULL )
    {
        sTmpAggrExecInfo.mInputArgList  = sAggrExecInfo->mInputArgList;
        sTmpAggrExecInfo.mAggrValueList = sAggrExecInfo->mAggrValueList;

        STL_TRY( gQlnfAggrFuncInfoArr[ sAggrExecInfo->mAggrFuncModule->mFuncID ].mInitFuncPtr( & sTmpAggrExecInfo,
                                                                                               0, /* result block idx */
                                                                                               aEnv )
                 == STL_SUCCESS );

        sAggrExecInfo = sAggrExecInfo->mNext;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} qlnx */

