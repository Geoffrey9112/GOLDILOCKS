/*******************************************************************************
 * qlnxUnionAll.c
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
 * @file qlnxUnionAll.c
 * @brief SQL Processor Layer Execution Node - UNION ALL
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnx
 * @{
 */

/**
 * @brief UNION ALL node 를 초기화한다.
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aStmt         Statement
 * @param[in]  aDBCStmt      DBC Statement
 * @param[in]  aSQLStmt      SQL Statement
 * @param[in]  aOptNode      Optimization Node
 * @param[in]  aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]  aEnv          Environment
 * @remarks 
 */
stlStatus qlnxInitializeUnionAll( smlTransId              aTransID,
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
 * @brief UNION ALL node 를 수행한다.
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aStmt         Statement
 * @param[in]  aDBCStmt      DBC Statement
 * @param[in]  aSQLStmt      SQL Statement
 * @param[in]  aOptNode      Optimization Node
 * @param[in]  aDataArea     Data Area
 * @param[in]  aEnv          Environment
 * @remarks 
 */
stlStatus qlnxExecuteUnionAll( smlTransId              aTransID,
                               smlStatement          * aStmt,
                               qllDBCStmt            * aDBCStmt,
                               qllStatement          * aSQLStmt,
                               qllOptimizationNode   * aOptNode,
                               qllDataArea           * aDataArea,
                               qllEnv                * aEnv )
{
    qllExecutionNode   * sExecNode  = NULL;
    qlnoSetOP          * sOptSetOP  = NULL;
    qlnxSetOP          * sExeSetOP  = NULL;

    stlInt32   i = 0;
    
    /**
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_UNION_ALL_TYPE, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );

    /*****************************************
     * 기본 정보 획득
     ****************************************/

    sOptSetOP = (qlnoSetOP *) aOptNode->mOptNode;
    
    sExecNode    = QLL_GET_EXECUTION_NODE( aDataArea, aOptNode->mIdx );
    sExeSetOP = (qlnxSetOP *) sExecNode->mExecNode;

    /*****************************************
     * 하위 노드 Execute
     ****************************************/

    for ( i = 0; i < sOptSetOP->mChildCount; i++ )
    {
        QLNX_EXECUTE_NODE( aTransID,
                           aStmt,
                           aDBCStmt,
                           aSQLStmt,
                           aDataArea->mExecNodeList[sOptSetOP->mChildNodeArray[i]->mIdx].mOptNode,
                           aDataArea,
                           aEnv );
    }

    /*****************************************
     * 수행할 Child 초기화 
     ****************************************/

    sExeSetOP->mFetchChildIdx = 0;
    
    return STL_SUCCESS;
   
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief UNION ALL node 를 Fetch 한다.
 * @param[in]      aFetchNodeInfo  Fetch Node Info
 * @param[out]     aUsedBlockCnt   Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF            End Of Fetch 여부
 * @param[in]      aEnv            Environment
 * @remark 
 */
stlStatus qlnxFetchUnionAll( qlnxFetchNodeInfo     * aFetchNodeInfo,
                             stlInt64              * aUsedBlockCnt,
                             stlBool               * aEOF,
                             qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode      = NULL;
    qlnoSetOP             * sOptSetOP      = NULL;
    qlnxSetOP             * sExeSetOP      = NULL;

    stlInt64                sSkipCnt       = 0;
    stlInt64                sFetchCnt      = 0;
    stlInt64                sChildFetchCnt = 0;

    stlBool                 sChildEOF      = STL_FALSE;

    knlValueBlockList     * sMyList        = NULL;
    knlValueBlockList     * sChildList     = NULL;
    qllExecutionNode      * sChildNode     = NULL;
    stlInt32                sChildNodeIdx  = 0;
    
    stlInt32                i              = 0;
    stlInt32                j              = 0;

    knlValueBlockList     * sResultList    = NULL;
    dtlDataValue          * sDstValue      = NULL;
    dtlDataValue          * sSrcValue      = NULL;

    stlInt64                sFetchedCnt    = 0;
    stlInt64                sSkippedCnt    = 0;
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

    STL_PARAM_VALIDATE( aFetchNodeInfo->mOptNode->mType == QLL_PLAN_NODE_UNION_ALL_TYPE,  QLL_ERROR_STACK(aEnv) );


    QLL_OPT_CHECK_TIME( sBeginFetchTime );

    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    sOptSetOP = (qlnoSetOP *) aFetchNodeInfo->mOptNode->mOptNode;
    
    sExecNode = QLL_GET_EXECUTION_NODE( aFetchNodeInfo->mDataArea,
                                        QLL_GET_OPT_NODE_IDX( aFetchNodeInfo->mOptNode ) );
    sExeSetOP = (qlnxSetOP *) sExecNode->mExecNode;

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

    sSkipCnt  = aFetchNodeInfo->mSkipCnt;
    sFetchCnt = aFetchNodeInfo->mFetchCnt;

    
    /*****************************************
     * FETCH
     ****************************************/

    while( sExeSetOP->mFetchChildIdx < sOptSetOP->mChildCount )
    {
        /**
         * Fetch Count 설정
         * - Skip Count 를 child 로 내리지 않는다.
         * - Skip 된 건수를 정확히 알아야 하므로 skip cnt 를 0 으로 넘긴다.
         */
        
        sChildEOF = STL_FALSE;

        sChildFetchCnt = sSkipCnt + sFetchCnt;

        if ( (sSkipCnt >= 0) && (sFetchCnt >= 0) && (sChildFetchCnt < 0) )
        {
            /* overflow 가 발생한 경우 */
            sChildFetchCnt = QLL_FETCH_COUNT_MAX;
        }

        QLL_OPT_CHECK_TIME( sBeginExceptTime );

        /**
         * Fetch Next
         */
        sChildNodeIdx = sOptSetOP->mChildNodeArray[sExeSetOP->mFetchChildIdx]->mIdx;
        QLNX_FETCH_NODE( & sLocalFetchInfo,
                         sChildNodeIdx,
                         0, /* Start Idx */
                         0,   /* aSkipCnt */
                         sChildFetchCnt,   
                         & sFetchedCnt,
                         &sChildEOF,
                         aEnv );
        
        QLL_OPT_CHECK_TIME( sEndExceptTime );
        QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

        
        /**
         * OFFSET .. FETCH 처리
         */
        
        if ( sFetchedCnt <= sSkipCnt )
        {
            /**
             * 모두 skip 할 data 임
             */
            
            sSkipCnt -= sFetchedCnt;
            sFetchedCnt = 0;
        }
        else
        {
            /**
             * 새로운 Child Fetch 시 Value Block 대체
             */

            sChildNode = QLL_GET_EXECUTION_NODE( sLocalFetchInfo.mDataArea, sChildNodeIdx );

            sMyList = sExeSetOP->mColumnBlock;
            sChildList = ((qlnxCommonInfo *)sChildNode->mExecNode)->mResultColBlock;

            if( sMyList->mValueBlock->mDataValue != sChildList->mValueBlock->mDataValue )
            {
                while (sChildList != NULL)
                {
                    stlMemcpy( sMyList->mValueBlock, sChildList->mValueBlock, STL_SIZEOF(knlValueBlock) );
                    
                    sChildList = sChildList->mNext;
                    sMyList = sMyList->mNext;
                }
            
                STL_DASSERT( sChildList == NULL );
                STL_DASSERT( sMyList == NULL );
            }

            
            /**
             * Return 할 결과가 포함됨
             */
            
            sFetchCnt = sFetchCnt - (sFetchedCnt - sSkipCnt);

            /**
             * Compact Result Block
             */
            
            if ( sSkipCnt > 0 )
            {
                sResultList = sExeSetOP->mColumnBlock;

                while ( sResultList != NULL )
                {
                    for ( i = 0, j = sSkipCnt; j < sFetchedCnt; i++, j++ )
                    {
                        sDstValue = DTL_GET_BLOCK_DATA_VALUE( sResultList, i );
                        sSrcValue = DTL_GET_BLOCK_DATA_VALUE( sResultList, j );

                        STL_TRY( knlCopyDataValue( sSrcValue,
                                                   sDstValue,
                                                   KNL_ENV(aEnv) )
                                 == STL_SUCCESS );
                    }

                    sResultList = sResultList->mNext;
                }

                sFetchedCnt -= sSkipCnt;
            }
            else
            {
                /* nothing to do */
            }
            
            sSkipCnt  = 0;
        }

        /**
         * 다음 Child 결정
         */
        
        if ( sChildEOF == STL_TRUE )
        {
            /**
             * 다음 Child 를 준비
             */
            sExeSetOP->mFetchChildIdx++;
        }
        else
        {
            /**
             * 현재 Child 를 유지
             */

            /* Do Nothing */
        }

        /**
         * Result 존재 여부 
         */
        
        if ( sFetchedCnt > 0 )
        {
            STL_THROW( RAMP_EXIT );
        }
        else
        {
            continue;
        }
    }

    *aEOF = STL_TRUE;
    
    STL_TRY( qlnxRevertQuerySetTarget( sLocalFetchInfo.mDataArea, sExecNode->mOptNode, aEnv )
             == STL_SUCCESS );


    STL_RAMP( RAMP_EXIT );

    
    /**
     * OUTPUT 설정
     */

    sSkippedCnt = aFetchNodeInfo->mSkipCnt - sSkipCnt;
    
    *aUsedBlockCnt = sFetchedCnt;
    
    /* Parent의 Fetch Node Info 설정 */
    aFetchNodeInfo->mFetchCnt -= sFetchedCnt;
    aFetchNodeInfo->mSkipCnt  -= sSkippedCnt;

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sExeSetOP->mColumnBlock,
                                         0,
                                         sFetchedCnt );

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeSetOP->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeSetOP->mCommonInfo.mFetchCallCount, 1 );
    QLL_OPT_ADD_RECORD_STAT_INFO( sExeSetOP->mCommonInfo.mFetchRecordStat,
                                  sExeSetOP->mCommonInfo.mResultColBlock,
                                  *aUsedBlockCnt,
                                  aEnv );

    sExeSetOP->mCommonInfo.mFetchRowCnt += *aUsedBlockCnt;
    
    return STL_SUCCESS;

    STL_FINISH;

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeSetOP->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeSetOP->mCommonInfo.mFetchCallCount, 1 );

    return STL_FAILURE;
}


/**
 * @brief UNION ALL node 수행을 종료한다.
 * @param[in] aOptNode      Optimization Node
 * @param[in] aDataArea     Data Area 
 * @param[in] aEnv          Environment
 * @remarks
 */
stlStatus qlnxFinalizeUnionAll( qllOptimizationNode   * aOptNode,
                                qllDataArea           * aDataArea,
                                qllEnv                * aEnv )
{
    /* nothing to do */
    
    return STL_SUCCESS;
}


/** @} qlnx */

