/*******************************************************************************
 * qlnxIndexAccess.c
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
 * @file qlnxIndexAccess.c
 * @brief SQL Processor Layer Execution Node - INDEX ACCESS
 */

#include <qll.h>
#include <qlDef.h>

/**
 * @addtogroup qlnx
 * @{
 */


/**
 * @brief INDEX ACCESS node를 초기화한다.
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
stlStatus qlnxInitializeIndexAccess( smlTransId              aTransID,
                                     smlStatement          * aStmt,
                                     qllDBCStmt            * aDBCStmt,
                                     qllStatement          * aSQLStmt,
                                     qllOptimizationNode   * aOptNode,
                                     qllDataArea           * aDataArea,
                                     qllEnv                * aEnv )
{
    qllExecutionNode   * sExecNode       = NULL;
    qlnoIndexAccess    * sOptIndexAccess = NULL;
    qlnxIndexAccess    * sExeIndexAccess = NULL;
    
    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_INDEX_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * INDEX ACCESS Optimization Node 획득
     */

    sOptIndexAccess = (qlnoIndexAccess *) aOptNode->mOptNode;


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
     * INDEX ACCESS Execution Node 획득
     */

    sExeIndexAccess = (qlnxIndexAccess *) sExecNode->mExecNode;


    /**
     * INDEX ACCESS Execution Node 정보 초기화
     */

    sExeIndexAccess->mIsFirstExecute = STL_TRUE;
    sExeIndexAccess->mHasFalseFilter = STL_FALSE;

    SML_INIT_ITERATOR_PROP( sExeIndexAccess->mIteratorProperty );

    
    /**
     * Iterator 생성
     */

    sExeIndexAccess->mIterator = NULL;

    STL_TRY( smlAllocIterator( aStmt,
                               sOptIndexAccess->mIndexPhysicalHandle,
                               sExeIndexAccess->mTransReadMode,
                               sExeIndexAccess->mStmtReadMode,
                               & sExeIndexAccess->mIteratorProperty,
                               sOptIndexAccess->mScanDirection,
                               & sExeIndexAccess->mIterator,
                               SML_ENV( aEnv ) )
             == STL_SUCCESS );
    
    
    /**
     * Aggregation Distinct를 위한 Instant Table 생성
     */

    if( sOptIndexAccess->mAggrDistFuncCnt > 0 )
    {
        STL_TRY( qlnxInitializeAggrDist( aTransID,
                                         aStmt,
                                         sOptIndexAccess->mAggrDistFuncCnt,
                                         sExeIndexAccess->mAggrDistExecInfo,
                                         aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /* Do Nothing */
    }
    
    
    /**
     * SORT INSTANT TABLE 생성
     */

    if( sExeIndexAccess->mInKeyRange != NULL )
    {
        stlMemset( & sExeIndexAccess->mInKeyRange->mInstantTable, 0x00, STL_SIZEOF( qlnInstantTable ) );
        
        STL_TRY( qlnxCreateSortInstantTable(
                     aTransID,
                     aStmt,
                     & sExeIndexAccess->mInKeyRange->mInstantTable,
                     NULL,
                     sOptIndexAccess->mListColMapCount,
                     sExeIndexAccess->mInKeyRange->mInsertColBlock[ 0 ],
                     sExeIndexAccess->mInKeyRange->mSortKeyFlags,
                     STL_TRUE,   /* top-down */
                     STL_FALSE,  /* volatile */
                     STL_TRUE,   /* leaf only */
                     STL_TRUE,   /* unique */
                     STL_TRUE,   /* null exlcuded */
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
 * @brief INDEX ACCESS node를 수행한다.
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
stlStatus qlnxExecuteIndexAccess( smlTransId              aTransID,
                                  smlStatement          * aStmt,
                                  qllDBCStmt            * aDBCStmt,
                                  qllStatement          * aSQLStmt,
                                  qllOptimizationNode   * aOptNode,
                                  qllDataArea           * aDataArea,
                                  qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode        = NULL;
    qlnoIndexAccess       * sOptIndexAccess  = NULL;
    qlnxIndexAccess       * sExeIndexAccess  = NULL;

    knlCompareList        * sCompList        = NULL;
    knlPhysicalFilter     * sPhysicalFilter  = NULL;

    smlFetchInfo          * sFetchInfo       = NULL;
    stlInt32                sLoop            = 0;
    
    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_INDEX_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    

    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * INDEX ACCESS Optimization Node 획득
     */

    sOptIndexAccess = (qlnoIndexAccess *) aOptNode->mOptNode;

    
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * INDEX ACCESS Execution Node 획득
     */

    sExeIndexAccess = sExecNode->mExecNode;
    
    /**
     * 해당 노드를 포함하는 Query Expression Optimization Node 획득
     */
    
    /* Query Node는 없을 수도 있다. */
   

    /*****************************************
     * Node 수행 준비
     ****************************************/

    /**
     * Index Iterator 초기화
     */

    if( sExeIndexAccess->mInKeyRange != NULL )
    {
        sFetchInfo = & sExeIndexAccess->mInKeyRange->mFetchInfo;

        sFetchInfo->mIsEndOfScan = STL_FALSE;
        sFetchInfo->mSkipCnt     = 0;
        sFetchInfo->mFetchCnt    = QLL_FETCH_COUNT_MAX;

        sExeIndexAccess->mInKeyRange->mFetchInfo.mIsEndOfScan = STL_FALSE;
        sExeIndexAccess->mInKeyRange->mNeedFetchKeyRec = STL_TRUE;
        sExeIndexAccess->mInKeyRange->mInstantTable.mIsNeedBuild = STL_TRUE;
    }

    sFetchInfo = & sExeIndexAccess->mFetchInfo;

    sFetchInfo->mIsEndOfScan = STL_FALSE;
    sFetchInfo->mSkipCnt     = 0;
    sFetchInfo->mFetchCnt    = QLL_FETCH_COUNT_MAX;

    if( sExeIndexAccess->mIsFirstExecute == STL_FALSE )
    {
        STL_TRY( smlResetIterator( sExeIndexAccess->mIterator,
                                   SML_ENV( aEnv ) )
                 == STL_SUCCESS );

        /**
         * Truncate Instant Table For IN Key Range
         */
        
        if( sExeIndexAccess->mInKeyRange != NULL )
        {
            STL_TRY( qlnxTruncateInstantTable( aStmt,
                                               & sExeIndexAccess->mInKeyRange->mInstantTable,
                                               aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /**
             * Prepare Range 수행
             */

            if( sExeIndexAccess->mPreRangeEvalInfo != NULL )
            {
                STL_TRY( knlEvaluateOneExpression( sExeIndexAccess->mPreRangeEvalInfo,
                                                   KNL_ENV( aEnv ) )
                         == STL_SUCCESS );
            }
        }
        

        /**
         * Prepare Key Filter 수행
         */

        if( sExeIndexAccess->mPreKeyFilterEvalInfo != NULL )
        {
            STL_TRY( knlEvaluateOneExpression( sExeIndexAccess->mPreKeyFilterEvalInfo,
                                               KNL_ENV( aEnv ) )
                     == STL_SUCCESS );
        }

        
        /**
         * Prepare Table Filter 수행
         */

        if( sExeIndexAccess->mPreFilterEvalInfo != NULL )
        {
            STL_TRY( knlEvaluateOneExpression( sExeIndexAccess->mPreFilterEvalInfo,
                                               KNL_ENV( aEnv ) )
                     == STL_SUCCESS );
        }
    }
    else
    {
        /* Do Nothing */
    }

    
    /**
     * Range Compare List의 constant value 정보 갱신
     */

    for( sLoop = 0 ; sLoop < sExeIndexAccess->mMinRangeTotalCnt ; sLoop++ )
    {
        sCompList = & sExeIndexAccess->mMinRangeCompList[ sLoop ];
        if( sCompList->mConstVal != NULL )
        {
            if( sCompList->mIsLikeFunc == STL_TRUE )
            {
                qlnxGetRangePatternValue( sCompList->mConstVal,
                                          sCompList );
            }
            else
            {
                sCompList->mRangeVal = sCompList->mConstVal->mValue;
                sCompList->mRangeLen = sCompList->mConstVal->mLength;
            }
        }
    }

    for( sLoop = 0 ; sLoop < sExeIndexAccess->mMaxRangeTotalCnt ; sLoop++ )
    {
        sCompList = & sExeIndexAccess->mMaxRangeCompList[ sLoop ];
        if( sCompList->mConstVal != NULL )
        {
            if( sCompList->mIsLikeFunc == STL_TRUE )
            {
                qlnxGetRangePatternValue( sCompList->mConstVal,
                                          sCompList );
            }
            else
            {
                sCompList->mRangeVal = sCompList->mConstVal->mValue;
                sCompList->mRangeLen = sCompList->mConstVal->mLength;
            }
        }
    }


    /**
     * Index Key Physical Filter의 constant value 정보 갱신
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

    
    /**
     * Table Physical Filter의 constant value 정보 갱신
     */

    if( sFetchInfo->mFetchRecordInfo != NULL )
    {
        sPhysicalFilter = sFetchInfo->mFetchRecordInfo->mPhysicalFilter;
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
    }
    else
    {
        /* Do Nothing */
    }


    /**
     * @todo Table Logical Filter가 하나의 Constant Expression으로 이루어져 있다면,
     *  <BR> 미리 수행하여 Index Scan 여부를 결정짓는다.
     */

    
    /**
     * 불필요한 요소를 제거하여 Range 재구성 (확장 필요)
     */
    
    if( ( sOptIndexAccess->mIndexScanInfo->mIsExist == STL_FALSE ) &&
        ( sOptIndexAccess->mIndexScanInfo->mEmptyValue == STL_FALSE ) )
    {
        sExeIndexAccess->mHasFalseFilter = STL_TRUE;
    }
    else
    {
        /**
         * IN Key Range를 포함하면 Refine Range를 수행하지 않는다.
         */

        if( sExeIndexAccess->mInKeyRange != NULL )
        {
            sExeIndexAccess->mHasFalseFilter =
                qlfHasFalseFilter( sOptIndexAccess->mIndexScanInfo->mLogicalKeyFilter,
                                   aDataArea );

            if( sExeIndexAccess->mHasFalseFilter == STL_FALSE )
            {
                sExeIndexAccess->mHasFalseFilter =
                    qlfHasFalseFilter( sOptIndexAccess->mIndexScanInfo->mLogicalTableFilter,
                                       aDataArea );
            }
        }
        else
        {
            if( sExeIndexAccess->mIsFirstExecute == STL_FALSE )
            {
                /* False Filter 체크도 해준다. */
                STL_TRY( qlnxRefineIndexFetchInfo( sOptIndexAccess->mIndexHandle,
                                                   sOptIndexAccess->mIndexScanInfo,
                                                   sFetchInfo,
                                                   sExeIndexAccess,
                                                   aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                sExeIndexAccess->mHasFalseFilter =
                    qlfIsAlwaysFalseRange( sFetchInfo->mRange->mMinRange,
                                           sFetchInfo->mRange->mMaxRange );
            }


            if( sExeIndexAccess->mHasFalseFilter == STL_FALSE )
            {
                sExeIndexAccess->mHasFalseFilter =
                    qlfHasFalseFilter( sOptIndexAccess->mIndexScanInfo->mLogicalKeyFilter,
                                       aDataArea );
            }
            
            if( sExeIndexAccess->mHasFalseFilter == STL_FALSE )
            {
                sExeIndexAccess->mHasFalseFilter =
                    qlfHasFalseFilter( sOptIndexAccess->mIndexScanInfo->mLogicalTableFilter,
                                       aDataArea );
            }
        }
    }


    sExeIndexAccess->mIsFirstExecute = STL_FALSE;
    
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief INDEX ACCESS node를 Fetch한다.
 * @param[in]      aFetchNodeInfo  Fetch Node Info
 * @param[out]     aUsedBlockCnt   Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF            End Of Fetch 여부
 * @param[in]      aEnv            Environment
 *
 * @remark Fetch 수행시 호출된다.
 */
stlStatus qlnxFetchIndexAccess( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                stlInt64              * aUsedBlockCnt,
                                stlBool               * aEOF,
                                qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode         = NULL;
    qlnoIndexAccess       * sOptIndexAccess   = NULL;
    qlnxIndexAccess       * sExeIndexAccess   = NULL;
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

    STL_PARAM_VALIDATE( aFetchNodeInfo->mOptNode->mType == QLL_PLAN_NODE_INDEX_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    
    QLL_OPT_CHECK_TIME( sBeginFetchTime );

    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * INDEX ACCESS Optimization Node 획득
     */

    sOptIndexAccess = (qlnoIndexAccess *) aFetchNodeInfo->mOptNode->mOptNode;

    
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aFetchNodeInfo->mDataArea,
                                        QLL_GET_OPT_NODE_IDX( aFetchNodeInfo->mOptNode ) );


    /**
     * INDEX ACCESS Execution Node 획득
     */

    sExeIndexAccess = sExecNode->mExecNode;

    
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
    STL_DASSERT( sExeIndexAccess->mFetchInfo.mIsEndOfScan == STL_FALSE );
    STL_DASSERT( aFetchNodeInfo->mFetchCnt > 0);
    STL_DASSERT( sExeIndexAccess->mIterator != NULL );


    /**
     * Local Fetch Info 초기화
     */
    
    QLNX_SET_LOCAL_FETCH_INFO( & sLocalFetchInfo, aFetchNodeInfo );


    /*****************************************
     * Node 수행 여부 확인
     ****************************************/

    sFetchInfo = & sExeIndexAccess->mFetchInfo;

    if( ( sOptIndexAccess->mAggrFuncCnt > 0 ) ||
        ( sOptIndexAccess->mAggrDistFuncCnt > 0 ) )
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
    
    if( sExeIndexAccess->mHasFalseFilter == STL_TRUE )
    {
        if( sOptIndexAccess->mAggrFuncCnt > 0 )
        {
            sAggrExecInfo = sFetchInfo->mAggrFetchInfo;

            STL_TRY( qlnxInitAggrFunc( sAggrExecInfo,
                                       aEnv )
                     == STL_SUCCESS );

            for( sLoop = 0 ; sLoop < sOptIndexAccess->mAggrFuncCnt ; sLoop++ )
            {
                KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sAggrExecInfo->mAggrValueList, 0, sFetchedCnt );
                sAggrExecInfo++;
            }
        }
        else if( sOptIndexAccess->mAggrDistFuncCnt > 0 )
        {
            sAggrDistExecInfo = sExeIndexAccess->mAggrDistExecInfo;
            for( sLoop = 0 ; sLoop < sOptIndexAccess->mAggrDistFuncCnt ; sLoop++ )
            {
                STL_TRY( qlnxInitAggrFunc( sAggrDistExecInfo->mFetchInfo.mAggrFetchInfo,
                                           aEnv )
                         == STL_SUCCESS );

                KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sAggrDistExecInfo->mAggrValueList, 0, sFetchedCnt );
                sAggrDistExecInfo++;
            }
        }

        sExeIndexAccess->mFetchInfo.mIsEndOfScan = STL_TRUE;
        *aEOF = STL_TRUE;
        STL_THROW( RAMP_EXIT );
    }
    else
    {
        /* Do Nothing */
    }


    /*****************************************
     * IN Key Range 수행
     ****************************************/

    if( sExeIndexAccess->mInKeyRange != NULL )
    {
        sLocalFetchInfo.mSkipCnt  = aFetchNodeInfo->mSkipCnt;
        sLocalFetchInfo.mFetchCnt = aFetchNodeInfo->mFetchCnt;
        
        if( sOptIndexAccess->mAggrDistFuncCnt == 0 )
        {
            STL_TRY( qlnxFetchIndexAccessInKeyRange( & sLocalFetchInfo,
                                                     & sFetchedCnt,
                                                     aEOF,
                                                     aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            STL_TRY( qlnxFetchIndexAccessInKeyRangeAggrDist( & sLocalFetchInfo,
                                                             & sFetchedCnt,
                                                             aEOF,
                                                             aEnv )
                     == STL_SUCCESS );
            
            STL_DASSERT( *aEOF == STL_TRUE );
        }
        
        sFetchedCnt = aFetchNodeInfo->mFetchCnt - sLocalFetchInfo.mFetchCnt;
        sSkippedCnt = aFetchNodeInfo->mSkipCnt - sLocalFetchInfo.mSkipCnt;
        
        STL_THROW( RAMP_EXIT );
    }
    

    /*****************************************
     * Aggregation Fetch 수행
     ****************************************/

    /**
     * Distinct 조건이 Aggregation 포함된 fetch 수행
     */
    
    if( sOptIndexAccess->mAggrDistFuncCnt > 0 )
    {
        sExeIndexAccess->mFetchInfo.mSkipCnt  = aFetchNodeInfo->mSkipCnt;
        sExeIndexAccess->mFetchInfo.mFetchCnt = 1;

        STL_TRY( qlnxFetchAggrDist( sLocalFetchInfo.mStmt,
                                    sExeIndexAccess->mIterator,
                                    & sExeIndexAccess->mFetchInfo,
                                    sOptIndexAccess->mAggrFuncCnt,
                                    sOptIndexAccess->mAggrDistFuncCnt,
                                    sExeIndexAccess->mAggrExecInfo,
                                    sExeIndexAccess->mAggrDistExecInfo,
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

    if( sOptIndexAccess->mAggrFuncCnt > 0 )
    {
        sFetchInfo->mSkipCnt  = 0;
        sFetchInfo->mFetchCnt = 1;
        
        STL_TRY( qlnxInitAggrFunc( sFetchInfo->mAggrFetchInfo,
                                   aEnv )
                 == STL_SUCCESS );

        STL_TRY( smlFetchAggr( sExeIndexAccess->mIterator,
                               sFetchInfo,
                               SML_ENV(aEnv) )
                 == STL_SUCCESS );

        sAggrExecInfo = sFetchInfo->mAggrFetchInfo;
    
        for( sLoop = 0 ; sLoop < sOptIndexAccess->mAggrFuncCnt ; sLoop++ )
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
        
        STL_TRY( smlFetchNext( sExeIndexAccess->mIterator,
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
    QLL_OPT_ADD_ELAPSED_TIME( sExeIndexAccess->mCommonInfo.mFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_COUNT( sExeIndexAccess->mCommonInfo.mFetchCallCount, 1 );
    QLL_OPT_ADD_RECORD_STAT_INFO( sExeIndexAccess->mCommonInfo.mFetchRecordStat,
                                  sExeIndexAccess->mCommonInfo.mResultColBlock,
                                  *aUsedBlockCnt, aEnv );

    sExeIndexAccess->mCommonInfo.mFetchRowCnt += *aUsedBlockCnt;
    
    return STL_SUCCESS;

    STL_FINISH;

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sExeIndexAccess->mCommonInfo.mFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_COUNT( sExeIndexAccess->mCommonInfo.mFetchCallCount, 1 );

    return STL_FAILURE;
}


/**
 * @brief INDEX ACCESS node 수행을 종료한다.
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 *
 * @remark Execution을 종료하고자 할 때 호출한다.
 */
stlStatus qlnxFinalizeIndexAccess( qllOptimizationNode   * aOptNode,
                                   qllDataArea           * aDataArea,
                                   qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode       = NULL;
    qlnoIndexAccess       * sOptIndexAccess = NULL;
    qlnxIndexAccess       * sExeIndexAccess = NULL;
    

    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_INDEX_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * INDEX ACCESS Optimization Node 획득
     */

    sOptIndexAccess = (qlnoIndexAccess * ) aOptNode->mOptNode;


    /**
     * INDEX ACCESS Execution Node 획득
     */

    sExeIndexAccess = sExecNode->mExecNode;
    

    /**
     * Iterator 해제
     */

    if( sExeIndexAccess->mIterator != NULL )
    {
        (void) smlFreeIterator( sExeIndexAccess->mIterator, SML_ENV(aEnv) );
        sExeIndexAccess->mIterator = NULL;
    }
    else
    {
        /* Do Nothing */
    }

    
    /**
     * Aggregation Distinct를 위한 Instant Table 해제
     */

    if( sOptIndexAccess->mAggrDistFuncCnt > 0 )
    {
        STL_TRY( qlnxFinalizeAggrDist( sOptIndexAccess->mAggrDistFuncCnt,
                                       sExeIndexAccess->mAggrDistExecInfo,
                                       aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /* Do Nothing */
    }


    /**
     * In Key Range를 위한 Instant Table 해제
     */

    if( sExeIndexAccess->mInKeyRange != NULL )
    {
        STL_TRY( qlnxDropInstantTable( & sExeIndexAccess->mInKeyRange->mInstantTable,
                                       aEnv )
                 == STL_SUCCESS );
    }

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Range의 constant 값을 평가하여 Range 재구성
 * @param[in]     aIndexHandle     Index Handle
 * @param[in]     aIndexScanInfo   Index Scan Info
 * @param[in]     aFetchInfo       Fetch Info
 * @param[in,out] aExeIndexAccess  Index Access Execution Node
 * @param[in]     aEnv             Environment
 */
stlStatus qlnxRefineIndexFetchInfo( ellDictHandle     * aIndexHandle,
                                    qloIndexScanInfo  * aIndexScanInfo,
                                    smlFetchInfo      * aFetchInfo,
                                    qlnxIndexAccess   * aExeIndexAccess,
                                    qllEnv            * aEnv )
    
{
    knlCompareList        * sMinRangeCompList = NULL;
    knlCompareList        * sMaxRangeCompList = NULL;
    knlCompareList        * sCurCompList      = NULL;
    knlCompareList        * sLastMinComp      = NULL;
    knlCompareList        * sLastMaxComp      = NULL;
    knlCompareList        * sHeadMinComp      = NULL;
    knlCompareList        * sHeadMaxComp      = NULL;
    ellIndexKeyDesc       * sIndexKeyDesc     = NULL;

    stlInt32                sMinRangeCnt      = 0;
    stlInt32                sMaxRangeCnt      = 0;

    stlBool                 sIsNullable       = STL_FALSE;
    stlBool                 sIsNullsFirst     = STL_FALSE;
    stlBool                 sIsAsc            = STL_FALSE;
    dtlCompareResult        sCompResult       = DTL_COMPARISON_EQUAL;

    stlInt32                sKeyColIdx        = 0;
    stlInt32                sLoop             = 0;

    
#define QLNX_ADD_RANGE_COMP_LIST()                              \
    {                                                           \
        if( sLastMinComp != NULL )                              \
        {                                                       \
            if( aFetchInfo->mRange->mMinRange == NULL )         \
            {                                                   \
                sLastMinComp->mNext = NULL;                     \
                sHeadMinComp = sLastMinComp;                    \
                aFetchInfo->mRange->mMinRange = sHeadMinComp;   \
            }                                                   \
            else                                                \
            {                                                   \
                sHeadMinComp->mNext = sLastMinComp;             \
                sLastMinComp->mNext = NULL;                     \
                sHeadMinComp = sLastMinComp;                    \
            }                                                   \
        }                                                       \
        if( sLastMaxComp != NULL )                              \
        {                                                       \
            if( aFetchInfo->mRange->mMaxRange == NULL )         \
            {                                                   \
                sLastMaxComp->mNext = NULL;                     \
                sHeadMaxComp = sLastMaxComp;                    \
                aFetchInfo->mRange->mMaxRange = sHeadMaxComp;   \
            }                                                   \
            else                                                \
            {                                                   \
                sHeadMaxComp->mNext = sLastMaxComp;             \
                sLastMaxComp->mNext = NULL;                     \
                sHeadMaxComp = sLastMaxComp;                    \
            }                                                   \
        }                                                       \
    }

    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aIndexHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexScanInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aFetchInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExeIndexAccess != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Min & Max Range 평가
     */

    aFetchInfo->mRange->mMinRange = NULL;
    aFetchInfo->mRange->mMaxRange = NULL;
    aFetchInfo->mRange->mNext     = NULL;
    aExeIndexAccess->mHasFalseFilter = STL_FALSE;

    if( ( aExeIndexAccess->mMinRangeCompList == NULL ) &&
        ( aExeIndexAccess->mMaxRangeCompList == NULL ) )
    {
        STL_THROW( RAMP_FINISH );
    }
    
    sMinRangeCompList = aExeIndexAccess->mMinRangeCompList;
    sMaxRangeCompList = aExeIndexAccess->mMaxRangeCompList;
    sIndexKeyDesc     = ellGetIndexKeyDesc( aIndexHandle );

    for( sKeyColIdx = 0 ; sKeyColIdx < aIndexScanInfo->mKeyColCount ; sKeyColIdx++ )
    {
        /**
         * 각 range 마다 가장 큰 Min Range 결정
         */

        sMinRangeCnt  = aIndexScanInfo->mMinRangeExpr[ sKeyColIdx ]->mCount;
        sMaxRangeCnt  = aIndexScanInfo->mMaxRangeExpr[ sKeyColIdx ]->mCount;
        sLastMinComp  = NULL;
        sLastMaxComp  = NULL;
        sIsNullable   =
            ellGetColumnNullable( & sIndexKeyDesc[ sKeyColIdx ].mKeyColumnHandle );
        sIsNullsFirst =
            ( sIndexKeyDesc[ sKeyColIdx ].mKeyNullOrdering == ELL_INDEX_COLUMN_NULLS_FIRST );
        sIsAsc        =
            ( sIndexKeyDesc[ sKeyColIdx ].mKeyOrdering == ELL_INDEX_COLUMN_ASCENDING );
        
        /* IS NULL, IS NOT NULL 정리 */ 
        for( sLoop = 0 ; sLoop < sMinRangeCnt ; sLoop++ )
        {
            sCurCompList = & sMinRangeCompList[ sLoop ];

            if( sCurCompList->mCompFunc == dtlLikeAllPhysicalCompareChar )
            {
                STL_THROW( RAMP_NEXT_MIN_RANGE );
            }
            
            if( sCurCompList->mConstVal->mLength == 0 )
            {
                if( sCurCompList->mCompNullStop == STL_TRUE )
                {
                    STL_THROW( RAMP_FALSE_FILTER );
                }
                
                if( sCurCompList->mIsIncludeEqual == STL_TRUE )
                {
                    /* IS NULL */
                    if( sIsNullable == STL_FALSE )
                    {
                        STL_THROW( RAMP_FALSE_FILTER );
                    }
                    else
                    {
                        /* Coverage : Optimizer에 의해 null check를 중복으로 하는 경우는 제거됨 */

                        /* if( sLastMinComp != NULL ) */
                        /* { */
                        /*     if( ( sLastMinComp->mConstVal->mLength == 0 ) && */
                        /*         ( sLastMinComp->mIsIncludeEqual == STL_TRUE ) ) */
                        /*     { */
                        /*         /\* case : is null *\/ */
                        /*         STL_THROW( RAMP_NEXT_MIN_RANGE ); */
                        /*     } */
                        /*     else */
                        /*     { */
                        /*         /\* case : is not null, equal, not equal *\/ */
                        /*         STL_THROW( RAMP_FALSE_FILTER ); */
                        /*     } */
                        /* } */
                    }
                }
                else
                {
                    /* IS NOT NULL */
                    /* Coverage : range value가 있는 경우 Optimizer에 의해 null check가 제거됨 */

                    /* if( sLastMinComp != NULL ) */
                    /* { */
                    /*     if( ( sLastMinComp->mConstVal->mLength == 0 ) && */
                    /*         ( sLastMinComp->mIsIncludeEqual == STL_TRUE ) ) */
                    /*     { */
                    /*         /\* case : is null *\/ */
                    /*         STL_THROW( RAMP_FALSE_FILTER ); */
                    /*     } */
                    /*     else */
                    /*     { */
                    /*         /\* case : is not null, equal, not equal *\/ */
                    /*         STL_THROW( RAMP_NEXT_MIN_RANGE ); */
                    /*     } */
                    /* } */
                }

                STL_DASSERT( sLastMinComp == NULL );
                
                sLastMinComp = sCurCompList;
            }
            else
            {
                if( sLastMinComp == NULL )
                {
                    /* Last Compare List로 설정*/
                    sLastMinComp = sCurCompList;
                    STL_THROW( RAMP_NEXT_MIN_RANGE );
                }

                if( sLastMinComp->mConstVal->mLength == 0 )
                {
                    if( sLastMinComp->mIsIncludeEqual == STL_TRUE )
                    {
                        /* last case : null */
                        STL_THROW( RAMP_FALSE_FILTER );
                    }
                    else
                    {
                        /* last case : not null */
                        /* remove not null */
                        sLastMinComp = sCurCompList;
                        STL_THROW( RAMP_NEXT_MIN_RANGE );
                    }
                }

                if( sLastMinComp->mIsLikeFunc == STL_TRUE )
                {
                    if( sCurCompList->mIsLikeFunc == STL_TRUE )
                    {
                        sCompResult = dtlPhysicalCompareFuncList
                            [ DTL_TYPE_VARCHAR ]
                            [ DTL_TYPE_VARCHAR ]
                            ( sLastMinComp->mRangeVal,
                              sLastMinComp->mRangeLen,
                              sCurCompList->mRangeVal,
                              sCurCompList->mRangeLen );
                    }
                    else
                    {
                        sCompResult = dtlPhysicalCompareFuncList
                            [ DTL_TYPE_VARCHAR ]
                            [ sCurCompList->mConstVal->mType ]
                            ( sLastMinComp->mRangeVal,
                              sLastMinComp->mRangeLen,
                              sCurCompList->mConstVal->mValue,
                              sCurCompList->mConstVal->mLength );
                    }
                }
                else
                {
                    if( sCurCompList->mIsLikeFunc == STL_TRUE )
                    {
                        sCompResult = dtlPhysicalCompareFuncList
                            [ sLastMinComp->mConstVal->mType ]
                            [ DTL_TYPE_VARCHAR ]
                            ( sLastMinComp->mConstVal->mValue,
                              sLastMinComp->mConstVal->mLength,
                              sCurCompList->mRangeVal,
                              sCurCompList->mRangeLen );
                    }
                    else
                    {
                        sCompResult = dtlPhysicalCompareFuncList
                            [ sLastMinComp->mConstVal->mType ]
                            [ sCurCompList->mConstVal->mType ]
                            ( sLastMinComp->mConstVal->mValue,
                              sLastMinComp->mConstVal->mLength,
                              sCurCompList->mConstVal->mValue,
                              sCurCompList->mConstVal->mLength );
                    }
                }

                if( sIsAsc == STL_TRUE )
                {
                    /* ASC */
                    switch( sCompResult )
                    {
                        case DTL_COMPARISON_LESS : 
                            {
                                sLastMinComp = sCurCompList;
                                break;
                            }
                        case DTL_COMPARISON_EQUAL :
                            {
                                if( sLastMinComp->mIsIncludeEqual == STL_TRUE )
                                {
                                    sLastMinComp = sCurCompList;
                                }
                                break;
                            }
                        default : /* DTL_COMPARISON_GREATER */
                            {
                                /* Do Nothing */
                                break;
                            }
                    }
                }
                else
                {
                    /* DESC */
                    switch( sCompResult )
                    {
                        case DTL_COMPARISON_LESS : 
                            {
                                /* Do Nothing */
                                break;
                            }
                        case DTL_COMPARISON_EQUAL :
                            {
                                if( sLastMinComp->mIsIncludeEqual == STL_FALSE )
                                {
                                    sLastMinComp = sCurCompList;
                                }
                                break;
                            }
                        default : /* DTL_COMPARISON_GREATER */
                            {
                                sLastMinComp = sCurCompList;
                                break;
                            }
                    }
                }
            }

            STL_RAMP( RAMP_NEXT_MIN_RANGE );
        }
        
        sMinRangeCompList += sMinRangeCnt;


        /**
         * 각 range 마다 가장 큰 Max Range 결정
         */

        /* IS NULL, IS NOT NULL 정리 */ 
        for( sLoop = 0 ; sLoop < sMaxRangeCnt ; sLoop++ )
        {
            sCurCompList = & sMaxRangeCompList[ sLoop ];
            
            if( sCurCompList->mCompFunc == dtlLikeAllPhysicalCompareChar )
            {
                STL_THROW( RAMP_NEXT_MAX_RANGE );
            }

            if( sCurCompList->mConstVal->mLength == 0 )
            {
                if( sCurCompList->mCompNullStop == STL_TRUE )
                {
                    STL_THROW( RAMP_FALSE_FILTER );
                }
                
                if( sCurCompList->mIsIncludeEqual == STL_TRUE )
                {
                    /* IS NULL */
                    if( sIsNullable == STL_FALSE )
                    {
                        /* Coverage : Min ragne 에 의해 먼저 걸러짐 */
                        STL_THROW( RAMP_FALSE_FILTER );
                    }
                    else
                    {
                        /* Coverage : Optimizer에 의해 null check를 중복으로 하는 경우는 제거됨 */

                        /* if( sLastMaxComp != NULL ) */
                        /* { */
                        /*     if( ( sLastMaxComp->mConstVal->mLength == 0 ) && */
                        /*         ( sLastMaxComp->mIsIncludeEqual == STL_TRUE ) ) */
                        /*     { */
                        /*         /\* case : is null *\/ */
                        /*         STL_THROW( RAMP_NEXT_MAX_RANGE ); */
                        /*     } */
                        /*     else */
                        /*     { */
                        /*         /\* case : is not null, equal, not equal *\/ */
                        /*         STL_THROW( RAMP_FALSE_FILTER ); */
                        /*     } */
                        /* } */
                    }
                }
                else
                {
                    /* IS NOT NULL */
                    /* Coverage : range value가 있는 경우 Optimizer에 의해 null check가 제거됨 */

                    /* if( sLastMaxComp != NULL ) */
                    /* { */
                    /*     if( ( sLastMaxComp->mConstVal->mLength == 0 ) && */
                    /*         ( sLastMaxComp->mIsIncludeEqual == STL_TRUE ) ) */
                    /*     { */
                    /*         /\* case : is null *\/ */
                    /*         STL_THROW( RAMP_FALSE_FILTER ); */
                    /*     } */
                    /*     else */
                    /*     { */
                    /*         /\* case : is not null, equal, not equal *\/ */
                    /*         STL_THROW( RAMP_NEXT_MAX_RANGE ); */
                    /*     } */
                    /* } */
                }

                STL_DASSERT( sLastMaxComp == NULL );
                
                sLastMaxComp = sCurCompList;
            }
            else
            {
                if( sLastMaxComp == NULL )
                {
                    /* Last Compare List로 설정*/
                    sLastMaxComp = sCurCompList;
                    STL_THROW( RAMP_NEXT_MAX_RANGE );
                }

                if( sLastMaxComp->mConstVal->mLength == 0 )
                {
                    if( sLastMaxComp->mIsIncludeEqual == STL_TRUE )
                    {
                        /* last case : null */
                        STL_THROW( RAMP_FALSE_FILTER );
                    }
                    else
                    {
                        /* last case : not null */
                        /* remove not null */
                        sLastMaxComp = sCurCompList;
                        STL_THROW( RAMP_NEXT_MAX_RANGE );
                    }
                }

                if( sLastMaxComp->mIsLikeFunc == STL_TRUE )
                {
                    if( sCurCompList->mIsLikeFunc == STL_TRUE )
                    {
                        sCompResult = dtlPhysicalCompareFuncList
                            [ DTL_TYPE_VARCHAR ]
                            [ DTL_TYPE_VARCHAR ]
                            ( sLastMaxComp->mRangeVal,
                              sLastMaxComp->mRangeLen,
                              sCurCompList->mRangeVal,
                              sCurCompList->mRangeLen );
                    }
                    else
                    {
                        sCompResult = dtlPhysicalCompareFuncList
                            [ DTL_TYPE_VARCHAR ]
                            [ sCurCompList->mConstVal->mType ]
                            ( sLastMaxComp->mRangeVal,
                              sLastMaxComp->mRangeLen,
                              sCurCompList->mConstVal->mValue,
                              sCurCompList->mConstVal->mLength );
                    }
                }
                else
                {
                    if( sCurCompList->mIsLikeFunc == STL_TRUE )
                    {
                        sCompResult = dtlPhysicalCompareFuncList
                            [ sLastMaxComp->mConstVal->mType ]
                            [ DTL_TYPE_VARCHAR ]
                            ( sLastMaxComp->mConstVal->mValue,
                              sLastMaxComp->mConstVal->mLength,
                              sCurCompList->mRangeVal,
                              sCurCompList->mRangeLen );
                    }
                    else
                    {
                        sCompResult = dtlPhysicalCompareFuncList
                            [ sLastMaxComp->mConstVal->mType ]
                            [ sCurCompList->mConstVal->mType ]
                            ( sLastMaxComp->mConstVal->mValue,
                              sLastMaxComp->mConstVal->mLength,
                              sCurCompList->mConstVal->mValue,
                              sCurCompList->mConstVal->mLength );
                    }
                }

                if( sIsAsc == STL_TRUE )
                {
                    /* ASC */
                    switch( sCompResult )
                    {
                        case DTL_COMPARISON_LESS : 
                            {
                                /* Do Nothing */
                                break;
                            }
                        case DTL_COMPARISON_EQUAL :
                            {
                                if( sLastMaxComp->mIsIncludeEqual == STL_TRUE )
                                {
                                    sLastMaxComp = sCurCompList;
                                }
                                break;
                            }
                        default : /* DTL_COMPARISON_GREATER */
                            {
                                sLastMaxComp = sCurCompList;
                                break;
                            }
                    }
                }
                else
                {
                    /* DESC */
                    switch( sCompResult )
                    {
                        case DTL_COMPARISON_LESS : 
                            {
                                sLastMaxComp = sCurCompList;
                                break;
                            }
                        case DTL_COMPARISON_EQUAL :
                            {
                                if( sLastMaxComp->mIsIncludeEqual == STL_FALSE )
                                {
                                    sLastMaxComp = sCurCompList;
                                }
                                break;
                            }
                        default : /* DTL_COMPARISON_GREATER */
                            {
                                /* Do Nothing */
                                break;
                            }
                    }
                }
            }

            STL_RAMP( RAMP_NEXT_MAX_RANGE );
        }
        
        sMaxRangeCompList += sMaxRangeCnt;


        /**
         * Always FALSE filter 인지 검사
         *
         * # index order 
         * NF : Nullable / Nulls First
         * NL : Nullable / Nulls Last
         * NA : Nullable false
         *
         * # range value 
         * EM : Empty
         * NU : Null
         * NN : Not Null
         * EQ : Equal
         * NE : Not Equal
         */

        if( sLastMinComp == NULL )
        {
            if( sLastMaxComp == NULL )
            {
                /* Coverage : range 가 없는 경우는 없다 */
                STL_THROW( RAMP_FINISH );
            }
            else if( ( sLastMaxComp->mConstVal->mLength == 0 ) &&
                     ( sLastMaxComp->mIsIncludeEqual == STL_TRUE ) )
            {
                /* Coverage : null에 의한 false filter 인 경우 상위에서 걸러졌다 */
                STL_DASSERT( 0 );

                /* if( sIsNullable == STL_FALSE ) */
                /* { */
                /*     /\* EM-NU(NA) *\/ */
                /*     STL_THROW( RAMP_FALSE_FILTER ); */
                /* } */
                    
                /* if( sIsNullsFirst == STL_TRUE ) */
                /* { */
                /*     /\* EM-NU(NF) *\/ */
                /*     QLNX_ADD_RANGE_COMP_LIST(); */
                /* } */
                /* else */
                /* { */
                /*     /\* EM-NU(NL) *\/ */
                /*     STL_THROW( RAMP_FALSE_FILTER ); */
                /* } */
            }
            else
            {
                if( sIsNullsFirst == STL_TRUE )
                {
                    /* Coverage : null에 의한 false filter 인 경우 상위에서 걸러졌다 */
                    STL_DASSERT( 0 );

                    /* /\* EM-NN,NE,EQ(NF) *\/ */
                    /* STL_THROW( RAMP_FALSE_FILTER ); */
                }
                else
                {
                    if( ( sLastMaxComp->mConstVal->mLength == 0 ) &&
                        ( sIsNullable == STL_FALSE ) )
                    {
                        /* EM-NN(NA) */
                        STL_THROW( RAMP_FINISH );
                    }
                    else
                    {
                        /* EM-NN(NL) */
                        /* EM-EQ(NL,NA) */
                        /* EM-NE(NL,NA) */
                        QLNX_ADD_RANGE_COMP_LIST();
                        STL_THROW( RAMP_FINISH );
                    }
                }
            }
        }
        else if( sLastMinComp->mConstVal->mLength == 0 )
        {
            if( sLastMinComp->mIsIncludeEqual == STL_TRUE )
            {
                if( sLastMaxComp == NULL )
                {
                    if( ( sIsNullable == STL_TRUE ) &&
                        ( sIsNullsFirst == STL_FALSE ) )
                    {
                        /* Coverage : max range 가 이미 설정되어 있다 */
                        STL_DASSERT( 0 );

                        /* /\* NU-EM(NL) *\/ */
                        /* QLNX_ADD_RANGE_COMP_LIST(); */
                    }
                    else
                    {
                        /* Coverage : null에 의한 false filter 인 경우 상위에서 걸러졌다 */
                        STL_DASSERT( 0 );

                        /* /\* NU-EM(NF,NA) *\/ */
                        /* STL_THROW( RAMP_FALSE_FILTER ); */
                    }
                }
                else
                {
                    if( ( sLastMaxComp->mConstVal->mLength == 0 ) &&
                        ( sLastMaxComp->mIsIncludeEqual == STL_TRUE ) &&
                        ( sIsNullable == STL_TRUE ) )
                    {
                        if( sIsNullsFirst == STL_TRUE )
                        {
                            /* NU-NU(NF) */
                            sLastMinComp = NULL;
                            QLNX_ADD_RANGE_COMP_LIST();
                        }
                        else
                        {
                            /* NU-NU(NL) */
                            sLastMaxComp = NULL;
                            QLNX_ADD_RANGE_COMP_LIST();
                        }
                    }
                    else
                    {
                        /* Coverage : null에 의한 false filter 인 경우 상위에서 걸러졌다 */
                        STL_DASSERT( 0 );
                        
                        /* /\* NU-NU(NA) *\/ */
                        /* /\* NU-NN,EQ,NE(NF,NL,NA) *\/ */
                        /* STL_THROW( RAMP_FALSE_FILTER ); */
                    }
                }
            }
            else
            {
                if( sLastMaxComp == NULL )
                {
                    if( sIsNullable == STL_FALSE )
                    {
                        /* NN-EM(NA) */
                        STL_THROW( RAMP_FINISH );
                    }
                    
                    if( sIsNullsFirst == STL_TRUE )
                    {
                        /* NN-EM(NF) */
                        QLNX_ADD_RANGE_COMP_LIST();
                        STL_THROW( RAMP_FINISH );
                    }
                    else
                    {
                        /* Coverage : max range 가 이미 설정되어 있다 */
                        STL_DASSERT( 0 );

                        /* /\* NN-EM(NL) *\/ */
                        /* STL_THROW( RAMP_FALSE_FILTER ); */
                    }
                }
                else
                {
                    if( sLastMaxComp->mConstVal->mLength == 0 )
                    {
                        /* Coverage : null에 의한 false filter 인 경우 상위에서 걸러졌다 */
                        STL_DASSERT( 0 );
                        
                        /* if( sLastMaxComp->mIsIncludeEqual == STL_TRUE ) */
                        /* { */
                        /*     /\* NN-NU(NF,NL,NA) *\/ */
                        /*     STL_THROW( RAMP_FALSE_FILTER ); */
                        /* } */
                        /* else */
                        /* { */
                        /*     if( sIsNullable == STL_FALSE ) */
                        /*     { */
                        /*         /\* NN-NN(NA) *\/ */
                        /*         STL_THROW( RAMP_FINISH ); */
                        /*     } */

                        /*     if( sIsNullsFirst == STL_TRUE ) */
                        /*     { */
                        /*         /\* NN-NN(NF) *\/ */
                        /*         sLastMaxComp = NULL; */
                        /*         QLNX_ADD_RANGE_COMP_LIST(); */
                        /*         STL_THROW( RAMP_FINISH ); */
                        /*     } */
                        /*     else */
                        /*     { */
                        /*         /\* NN-NN(NL) *\/ */
                        /*         sLastMinComp = NULL; */
                        /*         QLNX_ADD_RANGE_COMP_LIST(); */
                        /*         STL_THROW( RAMP_FINISH ); */
                        /*     } */
                        /* } */
                    }
                    else
                    {
                        if( ( sIsNullable == STL_TRUE ) &&
                            ( sIsNullsFirst == STL_TRUE ) )
                        {
                            /* NN-EQ,NE(NF) */
                            QLNX_ADD_RANGE_COMP_LIST();
                            STL_THROW( RAMP_FINISH );
                        }
                        else
                        {
                            /* NN-EQ,NE(NL,NA) */
                            sLastMinComp = NULL;
                            QLNX_ADD_RANGE_COMP_LIST();
                            STL_THROW( RAMP_FINISH );
                        }
                    }
                }
            }
        }
        else
        {
            if( sLastMaxComp == NULL )
            {
                if( ( sIsNullable == STL_TRUE ) &&
                    ( sIsNullsFirst == STL_FALSE ) )
                {
                    /* Coverage : false filter 인 경우 상위에서 걸러졌다 */
                    STL_DASSERT( 0 );
                    
                    /* /\* EQ-EM(NL) *\/ */
                    /* /\* NE-EM(NL) *\/ */
                    /* STL_THROW( RAMP_FALSE_FILTER ); */
                }
                else
                {
                    /* EQ-EM(NF,NA) */
                    /* NE-EM(NF,NA) */
                    QLNX_ADD_RANGE_COMP_LIST();
                    STL_THROW( RAMP_FINISH );
                }
            }

            if( sLastMaxComp->mConstVal->mLength == 0 )
            {
                if( sLastMaxComp->mIsIncludeEqual == STL_TRUE )
                {
                    /* Coverage : null에 의한 false filter 인 경우 상위에서 걸러졌다 */
                    STL_DASSERT( 0 );

                    /* /\* EQ-NU(NF,NL,NA) *\/ */
                    /* /\* NE-NU(NF,NL,NA) *\/ */
                    /* STL_THROW( RAMP_FALSE_FILTER ); */
                }
                else
                {
                    if( ( sIsNullable == STL_TRUE ) &&
                        ( sIsNullsFirst == STL_FALSE ) )
                    {
                        /* EQ-NN(NL) */
                        /* NE-NN(NL) */
                        QLNX_ADD_RANGE_COMP_LIST();
                        STL_THROW( RAMP_FINISH );
                    }
                    else
                    {
                        /* EQ-NN(NF,NA) */
                        /* NE-NN(NF,NA) */
                        sLastMaxComp = NULL;
                        QLNX_ADD_RANGE_COMP_LIST();
                        STL_THROW( RAMP_FINISH );
                    }
                }
            }
            else
            {
                /* EQ-EQ,NE(NF,NL,NA) */
                /* NE-EQ,NE(NF,NL,NA) */

                if( sLastMinComp->mIsLikeFunc == STL_TRUE )
                {
                    if( sLastMaxComp->mIsLikeFunc == STL_TRUE )
                    {
                        sCompResult = dtlPhysicalCompareFuncList
                            [ DTL_TYPE_VARCHAR ]
                            [ DTL_TYPE_VARCHAR ]
                            ( sLastMinComp->mRangeVal,
                              sLastMinComp->mRangeLen,
                              sLastMaxComp->mRangeVal,
                              sLastMaxComp->mRangeLen );
                    }
                    else
                    {
                        sCompResult = dtlPhysicalCompareFuncList
                            [ DTL_TYPE_VARCHAR ]
                            [ sLastMaxComp->mConstVal->mType ]
                            ( sLastMinComp->mRangeVal,
                              sLastMinComp->mRangeLen,
                              sLastMaxComp->mConstVal->mValue,
                              sLastMaxComp->mConstVal->mLength );
                    }
                }
                else
                {
                    if( sLastMaxComp->mIsLikeFunc == STL_TRUE )
                    {
                        sCompResult = dtlPhysicalCompareFuncList
                            [ sLastMinComp->mConstVal->mType ]
                            [ DTL_TYPE_VARCHAR ]
                            ( sLastMinComp->mConstVal->mValue,
                              sLastMinComp->mConstVal->mLength,
                              sLastMaxComp->mRangeVal,
                              sLastMaxComp->mRangeLen );
                    }
                    else
                    {
                        sCompResult = dtlPhysicalCompareFuncList
                            [ sLastMinComp->mConstVal->mType ]
                            [ sLastMaxComp->mConstVal->mType ]
                            ( sLastMinComp->mConstVal->mValue,
                              sLastMinComp->mConstVal->mLength,
                              sLastMaxComp->mConstVal->mValue,
                              sLastMaxComp->mConstVal->mLength );
                    }
                }
                
                switch( sCompResult )
                {
                    case DTL_COMPARISON_LESS : 
                        {
                            if( sIsAsc == STL_TRUE )
                            {
                                QLNX_ADD_RANGE_COMP_LIST();
                                STL_THROW( RAMP_FINISH );
                            }
                            else
                            {
                                STL_THROW( RAMP_FALSE_FILTER );
                            }
                            break;
                        }
                    case DTL_COMPARISON_EQUAL :
                        {
                            if( ( sLastMinComp->mIsIncludeEqual == STL_TRUE ) &&
                                ( sLastMaxComp->mIsIncludeEqual == STL_TRUE ) )
                            {
                                QLNX_ADD_RANGE_COMP_LIST();
                            }
                            else
                            {
                                STL_THROW( RAMP_FALSE_FILTER );
                            }
                            break;
                        }
                    default : /* DTL_COMPARISON_GREATER */
                        {
                            if( sIsAsc == STL_TRUE )
                            {
                                STL_THROW( RAMP_FALSE_FILTER );
                            }
                            else
                            {
                                QLNX_ADD_RANGE_COMP_LIST();
                                STL_THROW( RAMP_FINISH );
                            }
                            break;
                        }
                }
            }
        }
    }


    /**
     * @todo Key Filter 평가
     */



    /**
     * @todo Table Filter 평가
     */



    /**
     * FALSE FILTER 설정
     */

    STL_THROW( RAMP_FINISH );

    STL_RAMP( RAMP_FALSE_FILTER );

    aExeIndexAccess->mHasFalseFilter = STL_TRUE;
    aFetchInfo->mRange->mMinRange = NULL;
    aFetchInfo->mRange->mMaxRange = NULL;
    aFetchInfo->mRange->mNext     = NULL;
    

    /**
     * OUTPUT 설정
     */
    
    STL_RAMP( RAMP_FINISH );
    

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
                               

/**
 * @brief Index Range 로의 LIKE Function에 대한 Constant Value를 얻어온다.
 * @param[in]      aConstValue   Constant Value (Like Pattern)
 * @param[out]     aCompList     Comp List
 *
 * @remark 첫번째 Atom-Pattern 문자열을 얻어온다.
 */
void qlnxGetRangePatternValue( dtlDataValue    * aConstValue,
                               knlCompareList  * aCompList )
{
    dtlPatternInfo  * sPatternInfo;
    
    if( aConstValue->mLength == 0 )
    {
        aCompList->mRangeVal = NULL;
        aCompList->mRangeLen = 0;
    }
    else
    {
        sPatternInfo = (dtlPatternInfo*)aConstValue->mValue;

        STL_DASSERT( sPatternInfo->mPatternString != NULL );

        if( ( sPatternInfo->mPatternString[0] == '%' ) ||
            ( sPatternInfo->mPatternString[0] == '_' ) )
        {
            aCompList->mCompFunc = dtlLikeAllPhysicalCompareChar;
            aCompList->mRangeVal = sPatternInfo->mPatternString;
            aCompList->mRangeLen = 1;
        }
        else
        {
            aCompList->mCompFunc = dtlLikePhysicalCompareChar;
            aCompList->mRangeVal = sPatternInfo->mLeadingPattern->mAtomPatterns[0];
            aCompList->mRangeLen = sPatternInfo->mLeadingPattern->mAtomPatternLen[0];
        }
    }
}


/**
 * @brief INDEX ACCESS node를 Fetch한다.
 * @param[in]      aFetchNodeInfo  Fetch Node Info
 * @param[out]     aUsedBlockCnt   Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF            End Of Fetch 여부
 * @param[in]      aEnv            Environment
 *
 * @remark Fetch 수행시 호출된다.
 */
stlStatus qlnxFetchIndexAccessInKeyRange( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                          stlInt64              * aUsedBlockCnt,
                                          stlBool               * aEOF,
                                          qllEnv                * aEnv )
{
    qllExecutionNode         * sExecNode            = NULL;
    qlnoIndexAccess          * sOptIndexAccess      = NULL;
    qlnxIndexAccess          * sExeIndexAccess      = NULL;
    qlnxInKeyRangeFetchInfo  * sInKeyRangeFetchInfo = NULL;
    qlnInstantTable          * sInstantTable        = NULL;
    knlValueBlockList        * sValueBlockList      = NULL;
    knlValueBlockList        * sListColBlockList    = NULL;
    smlFetchInfo             * sInstantFetchInfo    = NULL;
    smlFetchInfo             * sFetchInfo           = NULL;
    smlAggrFuncInfo          * sAggrExecInfo        = NULL;
    knlCompareList           * sCompList            = NULL;
    knlPhysicalFilter        * sPhysicalFilter      = NULL;
    dtlDataValue             * sDataValue           = NULL;
    
    stlInt32                   sLoop                = 0;
    stlInt64                   sUsedBlockCnt        = 0;
    stlBool                    sIsFirstRange        = STL_TRUE;

    stlInt64                   sFetchedCnt          = 0;
    stlInt64                   sSkippedCnt          = 0;
    qlnxFetchNodeInfo          sLocalFetchInfo;

    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aFetchNodeInfo->mOptNode->mType == QLL_PLAN_NODE_INDEX_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    
    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * INDEX ACCESS Optimization Node 획득
     */

    sOptIndexAccess = (qlnoIndexAccess *) aFetchNodeInfo->mOptNode->mOptNode;

    
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aFetchNodeInfo->mDataArea,
                                        QLL_GET_OPT_NODE_IDX( aFetchNodeInfo->mOptNode ) );


    /**
     * INDEX ACCESS Execution Node 획득
     */

    sExeIndexAccess = sExecNode->mExecNode;

    
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
    STL_DASSERT( sExeIndexAccess->mFetchInfo.mIsEndOfScan == STL_FALSE );
    STL_DASSERT( aFetchNodeInfo->mFetchCnt > 0);
    STL_DASSERT( sExeIndexAccess->mIterator != NULL );


    /**
     * Local Fetch Info 초기화
     */
    
    QLNX_SET_LOCAL_FETCH_INFO( & sLocalFetchInfo, aFetchNodeInfo );

    
    /*****************************************
     * Node 수행 여부 확인
     ****************************************/

    STL_DASSERT( sExeIndexAccess->mHasFalseFilter == STL_FALSE );
    STL_DASSERT( sOptIndexAccess->mAggrDistFuncCnt == 0 );
    STL_DASSERT( sExeIndexAccess->mInKeyRange != NULL );
    STL_DASSERT( sOptIndexAccess->mListColMapCount > 0 );


    if( sOptIndexAccess->mAggrFuncCnt > 0 )
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
    
                
    /**
     * IN Key Range Record 구축
     */
    
    sInKeyRangeFetchInfo = sExeIndexAccess->mInKeyRange;
    sInstantTable = & sInKeyRangeFetchInfo->mInstantTable;

    sInstantFetchInfo = & sInKeyRangeFetchInfo->mFetchInfo;
    sFetchInfo        = & sExeIndexAccess->mFetchInfo;

    if( sInstantTable->mIsNeedBuild == STL_TRUE )
    {
        /**
         * Prepare IN Key Range 수행
         */

        if( sExeIndexAccess->mPreInKeyRangeEvalInfo != NULL )
        {
            STL_TRY( knlEvaluateOneExpression( sExeIndexAccess->mPreInKeyRangeEvalInfo,
                                               KNL_ENV( aEnv ) )
                     == STL_SUCCESS );
        }


        /**
         * Insert IN Key Ranges
         */

        STL_DASSERT( sInKeyRangeFetchInfo->mRecCnt > 0 );

        for( sLoop = 0 ; sLoop < sInKeyRangeFetchInfo->mRecCnt ;  sLoop++ )
        {
            /**
             * Instant Table에 Record 삽입
             */

            KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sInKeyRangeFetchInfo->mInsertColBlock[ sLoop ], 0, 1 );
            
            STL_TRY( smlInsertRecord( sLocalFetchInfo.mStmt,
                                      sInstantTable->mTableHandle,
                                      sInKeyRangeFetchInfo->mInsertColBlock[ sLoop ],
                                      NULL, /* unique violation */
                                      & sInKeyRangeFetchInfo->mRowBlock,
                                      SML_ENV( aEnv ) )
                     == STL_SUCCESS );
        }
        sInstantTable->mRecCnt = sInKeyRangeFetchInfo->mRecCnt;
        sInstantTable->mIsNeedBuild = STL_FALSE;
            
            
        /**
         * Iterator Allocation
         */
            
        if( sInstantTable->mIterator != NULL )
        {
            STL_TRY_THROW( smlFreeIterator( sInstantTable->mIterator,
                                      SML_ENV( aEnv ) )
                           == STL_SUCCESS, ERR_FREE_ITERATOR );
            
            sInstantTable->mIterator = NULL;
        }

        SML_INIT_ITERATOR_PROP( sInstantTable->mIteratorProperty );
            
        STL_TRY( smlAllocIterator( sLocalFetchInfo.mStmt,
                                   sInstantTable->mTableHandle,
                                   SML_TRM_SNAPSHOT,
                                   SML_SRM_SNAPSHOT,
                                   & sInstantTable->mIteratorProperty,
                                   SML_SCAN_FORWARD,
                                   & sInstantTable->mIterator,
                                   SML_ENV( aEnv ) )
                 == STL_SUCCESS );

        sFetchInfo->mIsEndOfScan = STL_FALSE;
        sFetchInfo->mSkipCnt     = 0;
        sFetchInfo->mFetchCnt    = QLL_FETCH_COUNT_MAX;

        sInKeyRangeFetchInfo->mFetchInfo.mIsEndOfScan = STL_FALSE;
        sInKeyRangeFetchInfo->mNeedFetchKeyRec = STL_TRUE;
    }
    else
    {
        /* Do Nothing */
    }
    

    /*****************************************
     * Fetch 수행
     ****************************************/

    /**
     * Fetch 수행
     */

    sFetchInfo->mSkipCnt  = aFetchNodeInfo->mSkipCnt;
    sFetchInfo->mFetchCnt = aFetchNodeInfo->mFetchCnt;

    if( sInKeyRangeFetchInfo->mNeedFetchKeyRec == STL_FALSE )
    {
        /**
         * Block Read
         */

        STL_DASSERT( sOptIndexAccess->mAggrFuncCnt == 0 );

        STL_TRY( smlFetchNext( sExeIndexAccess->mIterator,
                               sFetchInfo,
                               SML_ENV(aEnv) )
                 == STL_SUCCESS );

        if( SML_USED_BLOCK_SIZE( sFetchInfo->mRowBlock ) > 0 )
        {
            sInKeyRangeFetchInfo->mNeedFetchKeyRec = ( sFetchInfo->mIsEndOfScan == STL_TRUE );
                
            sFetchedCnt = SML_USED_BLOCK_SIZE( sFetchInfo->mRowBlock );
            sSkippedCnt = aFetchNodeInfo->mSkipCnt - sFetchInfo->mSkipCnt;

            STL_THROW( RAMP_FINISH );
        }
        else
        {
            STL_TRY( smlResetIterator( sExeIndexAccess->mIterator,
                                       SML_ENV( aEnv ) )
                     == STL_SUCCESS );

            sInKeyRangeFetchInfo->mNeedFetchKeyRec = STL_TRUE;
        }
    }
    else
    {
        STL_TRY( smlResetIterator( sExeIndexAccess->mIterator,
                                   SML_ENV( aEnv ) )
                 == STL_SUCCESS );
    }
    
    sFetchInfo->mIsEndOfScan = STL_FALSE;
    
    while( sInstantFetchInfo->mIsEndOfScan == STL_FALSE )
    {
        STL_DASSERT( sInKeyRangeFetchInfo->mNeedFetchKeyRec == STL_TRUE );
        
        /**
         * Fetch : Range Record
         */

        sInstantFetchInfo->mSkipCnt  = 0;
        sInstantFetchInfo->mFetchCnt = 1;
        
        STL_TRY( smlFetchNext( sInstantTable->mIterator,
                               sInstantFetchInfo,
                               SML_ENV(aEnv) )
                 == STL_SUCCESS );

        sUsedBlockCnt = sInstantFetchInfo->mRowBlock->mUsedBlockSize;

        if( sUsedBlockCnt == 0 )
        {
            /**
             * @todo BUGBUG Instant Table 에서 EOF 넘겨야 함
             */
            
            /* STL_DASSERT( sInstantFetchInfo->mIsEndOfScan == STL_TRUE ); */

            sInstantFetchInfo->mIsEndOfScan = STL_TRUE;
            
            sInKeyRangeFetchInfo->mNeedFetchKeyRec = STL_TRUE;
            STL_THROW( RAMP_FINISH );
        }
        else
        {
            /* sInstantFetchInfo->mIsEndOfScan = STL_FALSE; */
        }
        
        sInstantTable->mCurIdx += sUsedBlockCnt;


        /**
         * IN Key Range 값 설정
         */

        sValueBlockList = sInKeyRangeFetchInfo->mValueBlock;
        sListColBlockList = sInKeyRangeFetchInfo->mListColBlock;
        while( sValueBlockList != NULL )
        {
            STL_DASSERT( sListColBlockList != NULL );
            
            sDataValue = KNL_GET_BLOCK_FIRST_DATA_VALUE( sListColBlockList );
            *sValueBlockList->mValueBlock->mDataValue = *sDataValue;

            sValueBlockList = sValueBlockList->mNext;
            sListColBlockList = sListColBlockList->mNext;
        }
            

        /**
         * Prepare Range 수행
         */

        if( sExeIndexAccess->mPreRangeEvalInfo != NULL )
        {
            STL_TRY( knlEvaluateOneExpression( sExeIndexAccess->mPreRangeEvalInfo,
                                               KNL_ENV( aEnv ) )
                     == STL_SUCCESS );
        }

            
        /**
         * Range Compare List의 constant value 정보 갱신
         */

        for( sLoop = 0 ; sLoop < sExeIndexAccess->mMinRangeTotalCnt ; sLoop++ )
        {
            sCompList = & sExeIndexAccess->mMinRangeCompList[ sLoop ];
            if( sCompList->mConstVal != NULL )
            {
                if( sCompList->mIsLikeFunc == STL_TRUE )
                {
                    qlnxGetRangePatternValue( sCompList->mConstVal,
                                              sCompList );
                }
                else
                {
                    sCompList->mRangeVal = sCompList->mConstVal->mValue;
                    sCompList->mRangeLen = sCompList->mConstVal->mLength;
                }
            }
        }

        for( sLoop = 0 ; sLoop < sExeIndexAccess->mMaxRangeTotalCnt ; sLoop++ )
        {
            sCompList = & sExeIndexAccess->mMaxRangeCompList[ sLoop ];
            if( sCompList->mConstVal != NULL )
            {
                if( sCompList->mIsLikeFunc == STL_TRUE )
                {
                    qlnxGetRangePatternValue( sCompList->mConstVal,
                                              sCompList );
                }
                else
                {
                    sCompList->mRangeVal = sCompList->mConstVal->mValue;
                    sCompList->mRangeLen = sCompList->mConstVal->mLength;
                }
            }
        }


        /**
         * Index Key Physical Filter의 constant value 정보 갱신
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

    
        /**
         * Table Physical Filter의 constant value 정보 갱신
         */

        if( sFetchInfo->mFetchRecordInfo != NULL )
        {
            sPhysicalFilter = sFetchInfo->mFetchRecordInfo->mPhysicalFilter;
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
        }
        else
        {
            /* Do Nothing */
        }


        /**
         * 불필요한 요소를 제거하여 Range 재구성 (확장 필요)
         */

        /* False Filter 체크도 해준다. */
        STL_TRY( qlnxRefineIndexFetchInfo( sOptIndexAccess->mIndexHandle,
                                           sOptIndexAccess->mIndexScanInfo,
                                           sFetchInfo,
                                           sExeIndexAccess,
                                           aEnv )
                 == STL_SUCCESS );

        if( sExeIndexAccess->mHasFalseFilter == STL_TRUE )
        {
            sExeIndexAccess->mHasFalseFilter = STL_FALSE;
            continue;
        }
            
            
        /**
         * Block Read
         */

        if( sOptIndexAccess->mAggrFuncCnt > 0 )
        {
            sFetchInfo->mSkipCnt  = 0;
            sFetchInfo->mFetchCnt = 1;

            if( sIsFirstRange == STL_TRUE )
            {
                STL_TRY( qlnxInitAggrFunc( sFetchInfo->mAggrFetchInfo,
                                           aEnv )
                         == STL_SUCCESS );
            
                sIsFirstRange = STL_FALSE;
            }

            STL_TRY( smlFetchAggr( sExeIndexAccess->mIterator,
                                   sFetchInfo,
                                   SML_ENV(aEnv) )
                     == STL_SUCCESS );

            STL_TRY( smlResetIterator( sExeIndexAccess->mIterator,
                                       SML_ENV( aEnv ) )
                     == STL_SUCCESS );

            sInKeyRangeFetchInfo->mNeedFetchKeyRec = STL_TRUE;
            sFetchInfo->mIsEndOfScan = STL_FALSE;
        }
        else
        {
            STL_TRY( smlFetchNext( sExeIndexAccess->mIterator,
                                   sFetchInfo,
                                   SML_ENV(aEnv) )
                     == STL_SUCCESS );

            if( SML_USED_BLOCK_SIZE( sFetchInfo->mRowBlock ) > 0 )
            {
                sInKeyRangeFetchInfo->mNeedFetchKeyRec = ( sFetchInfo->mIsEndOfScan == STL_TRUE );

                sFetchedCnt = SML_USED_BLOCK_SIZE( sFetchInfo->mRowBlock );
                sSkippedCnt = aFetchNodeInfo->mSkipCnt - sFetchInfo->mSkipCnt;

                STL_THROW( RAMP_FINISH );
            }
            else
            {
                STL_TRY( smlResetIterator( sExeIndexAccess->mIterator,
                                           SML_ENV( aEnv ) )
                         == STL_SUCCESS );

                sInKeyRangeFetchInfo->mNeedFetchKeyRec = STL_TRUE;
                sFetchInfo->mIsEndOfScan = STL_FALSE;
            }
        }
    }   
    
    STL_RAMP( RAMP_FINISH );

    
    /**
     * OUTPUT 설정
     */

    *aUsedBlockCnt = sFetchedCnt;
    
    /* Parent의 Fetch Node Info 설정 */
    aFetchNodeInfo->mFetchCnt -= sFetchedCnt;
    aFetchNodeInfo->mSkipCnt  -= sSkippedCnt;

    sFetchInfo->mIsEndOfScan = STL_FALSE;

    if( ( sInstantFetchInfo->mIsEndOfScan == STL_TRUE ) &&
        ( sInKeyRangeFetchInfo->mNeedFetchKeyRec == STL_TRUE ) )
    {
        sAggrExecInfo = sFetchInfo->mAggrFetchInfo;
    
        for( sLoop = 0 ; sLoop < sOptIndexAccess->mAggrFuncCnt ; sLoop++ )
        {
            KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sAggrExecInfo->mAggrValueList, 0, *aUsedBlockCnt );
            sAggrExecInfo = sAggrExecInfo->mNext;
        }
            
        *aEOF = STL_TRUE;
    }

    
    return STL_SUCCESS;

    STL_CATCH( ERR_FREE_ITERATOR )
    {
        sInstantTable->mIterator = NULL;
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief INDEX ACCESS node를 In Key Range Scan과 동시에 Aggregation Distinct를 수행한다.
 * @param[in]      aFetchNodeInfo  Fetch Node Info
 * @param[out]     aUsedBlockCnt   Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF            End Of Fetch 여부
 * @param[in]      aEnv            Environment
 *
 * @remark Fetch 수행시 호출된다.
 */
stlStatus qlnxFetchIndexAccessInKeyRangeAggrDist( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                                  stlInt64              * aUsedBlockCnt,
                                                  stlBool               * aEOF,
                                                  qllEnv                * aEnv )
{
    qllExecutionNode         * sExecNode            = NULL;
    qlnoIndexAccess          * sOptIndexAccess      = NULL;
    qlnxIndexAccess          * sExeIndexAccess      = NULL;
    qlnxInKeyRangeFetchInfo  * sInKeyRangeFetchInfo = NULL;
    qlnInstantTable          * sInstantTable        = NULL;
    knlValueBlockList        * sValueBlockList      = NULL;
    knlValueBlockList        * sListColBlockList    = NULL;
    smlFetchInfo             * sInstantFetchInfo    = NULL;
    smlFetchInfo             * sFetchInfo           = NULL;
    knlCompareList           * sCompList            = NULL;
    knlPhysicalFilter        * sPhysicalFilter      = NULL;
    dtlDataValue             * sDataValue           = NULL;
    qlnfAggrExecInfo         * sAggrExecInfo        = NULL;        
    qlnfAggrDistExecInfo     * sAggrDistExecInfo    = NULL;
    
    stlInt32                   sLoop                = 0;
    stlInt64                   sUsedBlockCnt        = 0;
    stlInt64                   sReadCnt             = 0;
    stlInt32                   sArgIdx              = 0;

    stlInt64                   sFetchedCnt          = 0;
    stlInt64                   sSkippedCnt          = 0;
    qlnxFetchNodeInfo          sLocalFetchInfo;

    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aFetchNodeInfo->mOptNode->mType == QLL_PLAN_NODE_INDEX_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    
    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * INDEX ACCESS Optimization Node 획득
     */

    sOptIndexAccess = (qlnoIndexAccess *) aFetchNodeInfo->mOptNode->mOptNode;

    
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aFetchNodeInfo->mDataArea,
                                        QLL_GET_OPT_NODE_IDX( aFetchNodeInfo->mOptNode ) );


    /**
     * INDEX ACCESS Execution Node 획득
     */

    sExeIndexAccess = sExecNode->mExecNode;

    
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
    STL_DASSERT( sExeIndexAccess->mFetchInfo.mIsEndOfScan == STL_FALSE );
    STL_DASSERT( aFetchNodeInfo->mFetchCnt > 0);
    STL_DASSERT( sExeIndexAccess->mIterator != NULL );


    /**
     * Local Fetch Info 초기화
     */
    
    QLNX_SET_LOCAL_FETCH_INFO( & sLocalFetchInfo, aFetchNodeInfo );


    /*****************************************
     * Node 수행 여부 확인
     ****************************************/

    STL_DASSERT( sExeIndexAccess->mHasFalseFilter == STL_FALSE );
    STL_DASSERT( sOptIndexAccess->mAggrDistFuncCnt > 0 );
    STL_DASSERT( sExeIndexAccess->mInKeyRange != NULL );
    STL_DASSERT( sOptIndexAccess->mListColMapCount > 0 );


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


    /**
     * IN Key Range Record 구축
     */
    
    sInKeyRangeFetchInfo = sExeIndexAccess->mInKeyRange;
    sInstantTable = & sInKeyRangeFetchInfo->mInstantTable;

    sInstantFetchInfo = & sInKeyRangeFetchInfo->mFetchInfo;
    sFetchInfo        = & sExeIndexAccess->mFetchInfo;

    STL_DASSERT( sInstantTable->mIsNeedBuild == STL_TRUE );

    /**
     * Prepare IN Key Range 수행
     */

    if( sExeIndexAccess->mPreInKeyRangeEvalInfo != NULL )
    {
        STL_TRY( knlEvaluateOneExpression( sExeIndexAccess->mPreInKeyRangeEvalInfo,
                                           KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
    }


    /**
     * Insert IN Key Ranges
     */

    STL_DASSERT( sInKeyRangeFetchInfo->mRecCnt > 0 );

    for( sLoop = 0 ; sLoop < sInKeyRangeFetchInfo->mRecCnt ;  sLoop++ )
    {
        /**
         * Instant Table에 Record 삽입
         */

        KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sInKeyRangeFetchInfo->mInsertColBlock[ sLoop ], 0, 1 );
            
        STL_TRY( smlInsertRecord( sLocalFetchInfo.mStmt,
                                  sInstantTable->mTableHandle,
                                  sInKeyRangeFetchInfo->mInsertColBlock[ sLoop ],
                                  NULL, /* unique violation */
                                  & sInKeyRangeFetchInfo->mRowBlock,
                                  SML_ENV( aEnv ) )
                 == STL_SUCCESS );
    }
    sInstantTable->mRecCnt = sInKeyRangeFetchInfo->mRecCnt;
    sInstantTable->mIsNeedBuild = STL_FALSE;
            
            
    /**
     * Iterator Allocation
     */
            
    if( sInstantTable->mIterator != NULL )
    {
        STL_TRY_THROW( smlFreeIterator( sInstantTable->mIterator,
                                        SML_ENV( aEnv ) )
                       == STL_SUCCESS, ERR_FREE_ITERATOR );
            
        sInstantTable->mIterator = NULL;
    }

    SML_INIT_ITERATOR_PROP( sInstantTable->mIteratorProperty );
            
    STL_TRY( smlAllocIterator( sLocalFetchInfo.mStmt,
                               sInstantTable->mTableHandle,
                               SML_TRM_SNAPSHOT,
                               SML_SRM_SNAPSHOT,
                               & sInstantTable->mIteratorProperty,
                               SML_SCAN_FORWARD,
                               & sInstantTable->mIterator,
                               SML_ENV( aEnv ) )
             == STL_SUCCESS );

    sFetchInfo->mIsEndOfScan = STL_FALSE;
    sFetchInfo->mSkipCnt     = 0;
    sFetchInfo->mFetchCnt    = QLL_FETCH_COUNT_MAX;

    sInKeyRangeFetchInfo->mFetchInfo.mIsEndOfScan = STL_FALSE;
    sInKeyRangeFetchInfo->mNeedFetchKeyRec = STL_TRUE;
    

    /*****************************************
     * Aggregation 수행
     ****************************************/

    sAggrExecInfo = sExeIndexAccess->mAggrExecInfo;
    for( sLoop = 0 ; sLoop < sOptIndexAccess->mAggrFuncCnt ; sLoop++ )
    {
        STL_TRY( gQlnfAggrFuncInfoArr[sAggrExecInfo->mAggrOptInfo->mAggrFuncId].mInitFuncPtr(
                     sAggrExecInfo,
                     0, /* result block idx */
                     aEnv )
                 == STL_SUCCESS );
        sAggrExecInfo++;
    }


    /*****************************************
     * Fetch 수행
     ****************************************/

    /**
     * Fetch 수행
     */

    while( sInstantFetchInfo->mIsEndOfScan == STL_FALSE )
    {
        /**
         * Fetch : Range Record
         */

        sInstantFetchInfo->mSkipCnt  = 0;
        sInstantFetchInfo->mFetchCnt = 1;
        
        STL_TRY( smlFetchNext( sInstantTable->mIterator,
                               sInstantFetchInfo,
                               SML_ENV(aEnv) )
                 == STL_SUCCESS );

        sUsedBlockCnt = sInstantFetchInfo->mRowBlock->mUsedBlockSize;

        if( sUsedBlockCnt == 0 )
        {
            /**
             * @todo BUGBUG Instant Table 에서 EOF 넘겨야 함
             */
            
            /* STL_DASSERT( sInstantFetchInfo->mIsEndOfScan == STL_TRUE ); */

            sInstantFetchInfo->mIsEndOfScan = STL_TRUE;
            break;
        }
        
        sInstantTable->mCurIdx += sUsedBlockCnt;


        /**
         * IN Key Range 값 설정
         */

        sValueBlockList = sInKeyRangeFetchInfo->mValueBlock;
        sListColBlockList = sInKeyRangeFetchInfo->mListColBlock;
        while( sValueBlockList != NULL )
        {
            STL_DASSERT( sListColBlockList != NULL );
            
            sDataValue = KNL_GET_BLOCK_FIRST_DATA_VALUE( sListColBlockList );
            *sValueBlockList->mValueBlock->mDataValue = *sDataValue;

            sValueBlockList = sValueBlockList->mNext;
            sListColBlockList = sListColBlockList->mNext;
        }
            

        /**
         * Prepare Range 수행
         */

        if( sExeIndexAccess->mPreRangeEvalInfo != NULL )
        {
            STL_TRY( knlEvaluateOneExpression( sExeIndexAccess->mPreRangeEvalInfo,
                                               KNL_ENV( aEnv ) )
                     == STL_SUCCESS );
        }

            
        /**
         * Range Compare List의 constant value 정보 갱신
         */

        for( sLoop = 0 ; sLoop < sExeIndexAccess->mMinRangeTotalCnt ; sLoop++ )
        {
            sCompList = & sExeIndexAccess->mMinRangeCompList[ sLoop ];
            if( sCompList->mConstVal != NULL )
            {
                if( sCompList->mIsLikeFunc == STL_TRUE )
                {
                    qlnxGetRangePatternValue( sCompList->mConstVal,
                                              sCompList );
                }
                else
                {
                    sCompList->mRangeVal = sCompList->mConstVal->mValue;
                    sCompList->mRangeLen = sCompList->mConstVal->mLength;
                }
            }
        }

        for( sLoop = 0 ; sLoop < sExeIndexAccess->mMaxRangeTotalCnt ; sLoop++ )
        {
            sCompList = & sExeIndexAccess->mMaxRangeCompList[ sLoop ];
            if( sCompList->mConstVal != NULL )
            {
                if( sCompList->mIsLikeFunc == STL_TRUE )
                {
                    qlnxGetRangePatternValue( sCompList->mConstVal,
                                              sCompList );
                }
                else
                {
                    sCompList->mRangeVal = sCompList->mConstVal->mValue;
                    sCompList->mRangeLen = sCompList->mConstVal->mLength;
                }
            }
        }


        /**
         * Index Key Physical Filter의 constant value 정보 갱신
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

    
        /**
         * Table Physical Filter의 constant value 정보 갱신
         */

        if( sFetchInfo->mFetchRecordInfo != NULL )
        {
            sPhysicalFilter = sFetchInfo->mFetchRecordInfo->mPhysicalFilter;
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
        }
        else
        {
            /* Do Nothing */
        }


        /**
         * 불필요한 요소를 제거하여 Range 재구성 (확장 필요)
         */

        /* False Filter 체크도 해준다. */
        STL_TRY( qlnxRefineIndexFetchInfo( sOptIndexAccess->mIndexHandle,
                                           sOptIndexAccess->mIndexScanInfo,
                                           sFetchInfo,
                                           sExeIndexAccess,
                                           aEnv )
                 == STL_SUCCESS );

        if( sExeIndexAccess->mHasFalseFilter == STL_TRUE )
        {
            sExeIndexAccess->mHasFalseFilter = STL_FALSE;
            continue;
        }
            
            
        /**
         * Block Read
         */

        STL_TRY( smlFetchNext( sExeIndexAccess->mIterator,
                               sFetchInfo,
                               SML_ENV(aEnv) )
                 == STL_SUCCESS );

        if( SML_USED_BLOCK_SIZE( sFetchInfo->mRowBlock ) > 0 )
        {
            sReadCnt = SML_USED_BLOCK_SIZE( sFetchInfo->mRowBlock );
            
            /**
             * Aggregation 수행 (No Distinct)
             */

            if( sExeIndexAccess->mAggrExecInfo != NULL )
            {
                sAggrExecInfo = sExeIndexAccess->mAggrExecInfo;

                for( sLoop = 0 ; sLoop < sOptIndexAccess->mAggrFuncCnt ; sLoop++ )
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

            /**
             * Aggregation Distinct를 위해 Instant Table에 Record 삽입
             */

            sAggrDistExecInfo = sExeIndexAccess->mAggrDistExecInfo;

            for( sLoop = 0 ; sLoop < sOptIndexAccess->mAggrDistFuncCnt ; sLoop++ )
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
                                          sExeIndexAccess->mFetchInfo.mRowBlock,
                                          SML_ENV( aEnv ) )
                         == STL_SUCCESS );

                sAggrDistExecInfo++;
            }
        }

        STL_TRY( smlResetIterator( sExeIndexAccess->mIterator,
                                   SML_ENV( aEnv ) )
                 == STL_SUCCESS );

        sFetchInfo->mIsEndOfScan = STL_FALSE;
    }


    /**
     * Iterator Allocation
     */
        
    sAggrDistExecInfo = sExeIndexAccess->mAggrDistExecInfo;
            
    for( sLoop = 0 ; sLoop < sOptIndexAccess->mAggrDistFuncCnt ; sLoop++ )
    {
        SML_INIT_ITERATOR_PROP( sAggrDistExecInfo->mInstantTable.mIteratorProperty );
            
        if( sAggrDistExecInfo->mInstantTable.mIterator != NULL )
        {
            STL_TRY_THROW( smlFreeIterator( sAggrDistExecInfo->mInstantTable.mIterator,
                                            SML_ENV( aEnv ) )
                           == STL_SUCCESS, ERR_FREE_AGGR_ITERATOR );

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

    sAggrDistExecInfo = sExeIndexAccess->mAggrDistExecInfo;
    for( sLoop = 0 ; sLoop < sOptIndexAccess->mAggrDistFuncCnt ; sLoop++ )
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


    STL_DASSERT( sInstantFetchInfo->mIsEndOfScan == STL_TRUE );
    
    
    /**
     * OUTPUT 설정
     */

    *aUsedBlockCnt = sFetchedCnt;
    
    /* Parent의 Fetch Node Info 설정 */
    aFetchNodeInfo->mFetchCnt -= sFetchedCnt;
    aFetchNodeInfo->mSkipCnt  -= sSkippedCnt;

    *aEOF = STL_TRUE;

    
    return STL_SUCCESS;

    STL_CATCH( ERR_FREE_ITERATOR )
    {
        sInstantTable->mIterator = NULL;
    }
    
    STL_CATCH( ERR_FREE_AGGR_ITERATOR )
    {
        sAggrDistExecInfo->mInstantTable.mIterator = NULL;
    }

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlnx */
