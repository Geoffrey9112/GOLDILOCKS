/*******************************************************************************
 * qlnxSortInstantAccess.c
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
 * @file qlnxSortInstantAccess.c
 * @brief SQL Processor Layer Execution Node - SORT INSTANT TABLE
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnx
 * @{
 */

/**
 * @brief SORT INSTANT TABLE node를 초기화한다.
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
stlStatus qlnxInitializeSortInstantAccess( smlTransId              aTransID,
                                           smlStatement          * aStmt,
                                           qllDBCStmt            * aDBCStmt,
                                           qllStatement          * aSQLStmt,
                                           qllOptimizationNode   * aOptNode,
                                           qllDataArea           * aDataArea,
                                           qllEnv                * aEnv )
{
    qllExecutionNode  * sExecNode       = NULL;
    qlnoInstant       * sOptSortInstant = NULL;
    qlnxInstant       * sExeSortInstant = NULL;

    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SORT_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * SORT INSTANT TABLE Optimization Node 획득
     */

    sOptSortInstant = (qlnoInstant *) aOptNode->mOptNode;


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * 해당 노드를 포함하는 Query Expression Optimization Node 획득
     */

    /* sOptQueryNode = aOptNode->mOptCurQueryNode; */


    /**
     * SORT INSTANT ACCESS Execution Node 획득
     */

    sExeSortInstant = (qlnxInstant *) sExecNode->mExecNode;

    STL_DASSERT( sExeSortInstant->mKeyColBlock != NULL );
    STL_DASSERT( sExeSortInstant->mInsertColBlock != NULL );

    
    /**
     * SORT INSTANT TABLE 생성
     */

    if( sExeSortInstant->mNeedMaterialize == STL_TRUE )
    {
        stlMemset( & sExeSortInstant->mInstantTable, 0x00, STL_SIZEOF( qlnInstantTable ) );
        
        STL_TRY( qlnxCreateSortInstantTable( aTransID,
                                             aStmt,
                                             & sExeSortInstant->mInstantTable,
                                             sExeSortInstant->mInstantTable.mTableHandle,
                                             sOptSortInstant->mInstantNode->mKeyColCnt,
                                             sExeSortInstant->mKeyColBlock,
                                             sOptSortInstant->mInstantNode->mKeyFlags,
                                             sOptSortInstant->mSortTableAttr.mTopDown,
                                             sOptSortInstant->mSortTableAttr.mVolatile,
                                             sOptSortInstant->mSortTableAttr.mLeafOnly,
                                             sOptSortInstant->mSortTableAttr.mDistinct,
                                             sOptSortInstant->mSortTableAttr.mNullExcluded,
                                             aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_DASSERT( sOptSortInstant->mInstantNode->mRecColCnt == 0 );

        STL_DASSERT( 0 );
    }

    sExeSortInstant->mHasFalseFilter = STL_FALSE;
    sExeSortInstant->mIsFirstExecute = STL_TRUE;
        
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SORT INSTANT TABLE node를 수행한다.
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
stlStatus qlnxExecuteSortInstantAccess( smlTransId              aTransID,
                                        smlStatement          * aStmt,
                                        qllDBCStmt            * aDBCStmt,
                                        qllStatement          * aSQLStmt,
                                        qllOptimizationNode   * aOptNode,
                                        qllDataArea           * aDataArea,
                                        qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode       = NULL;
    qlnoInstant           * sOptSortInstant = NULL;
    qlnxInstant           * sExeSortInstant = NULL;
    qlnInstantTable       * sInstantTable   = NULL;

    knlCompareList        * sCompList       = NULL;
    knlPhysicalFilter     * sPhysicalFilter = NULL;
    smlFetchInfo          * sFetchInfo      = NULL;

    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SORT_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * SORT INSTANT TABLE Optimization Node 획득
     */

    sOptSortInstant = (qlnoInstant *) aOptNode->mOptNode;


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * 해당 노드를 포함하는 Query Expression Optimization Node 획득
     */

    /* sOptQueryNode = aOptNode->mOptCurQueryNode; */


    /**
     * SORT INSTANT ACCESS Execution Node 획득
     */

    sExeSortInstant = (qlnxInstant *) sExecNode->mExecNode;

    /*****************************************
     * Sort Instant Table Iterator 초기화
     ****************************************/

    /**
     * Cursor를 First Record 위치로 이동
     */

    sInstantTable = & sExeSortInstant->mInstantTable;
    
    sInstantTable->mStmt   = aStmt;
    sInstantTable->mCurIdx = 0;

    if( sInstantTable->mIsNeedBuild == STL_FALSE )
    {
        if( sExeSortInstant->mNeedRebuild == STL_TRUE )
        {
            STL_TRY( qlnxTruncateInstantTable( aStmt,
                                               & sExeSortInstant->mInstantTable,
                                               aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            if( sInstantTable->mIterator != NULL )
            {
                STL_TRY( smlResetIterator( sInstantTable->mIterator,
                                           SML_ENV( aEnv ) )
                         == STL_SUCCESS );
            }
        }
    }
    else
    {
        /* Do Nothing */
    }


    /**
     * Prepare Expression Stack 수행
     */
    
    if( sExeSortInstant->mIsFirstExecute == STL_FALSE )
    {
        /**
         * Prepare Range 수행
         */

        if( sExeSortInstant->mPreRangeEvalInfo != NULL )
        {
            STL_TRY( knlEvaluateOneExpression( sExeSortInstant->mPreRangeEvalInfo,
                                               KNL_ENV( aEnv ) )
                     == STL_SUCCESS );
        }

        
        /**
         * Prepare Key Filter 수행
         */

        if( sExeSortInstant->mPreKeyFilterEvalInfo != NULL )
        {
            STL_TRY( knlEvaluateOneExpression( sExeSortInstant->mPreKeyFilterEvalInfo,
                                               KNL_ENV( aEnv ) )
                     == STL_SUCCESS );
        }

        
        /**
         * Prepare Table Filter 수행
         */

        if( sExeSortInstant->mPreFilterEvalInfo != NULL )
        {
            STL_TRY( knlEvaluateOneExpression( sExeSortInstant->mPreFilterEvalInfo,
                                               KNL_ENV( aEnv ) )
                     == STL_SUCCESS );
        }
    }
    else
    {
        /* Do Nothing */
    }


    /**
     * Instant Table Iterator 초기화
     */

    sFetchInfo = & sExeSortInstant->mFetchInfo;

    sFetchInfo->mIsEndOfScan = STL_FALSE;
    sFetchInfo->mSkipCnt     = 0;
    sFetchInfo->mFetchCnt    = QLL_FETCH_COUNT_MAX;


    /**
     * Range Compare List의 constant value 정보 갱신
     */

    sCompList = sFetchInfo->mRange->mMinRange;
    while( sCompList != NULL )
    {
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
            
        sCompList = sCompList->mNext;
    }

    sCompList = sFetchInfo->mRange->mMaxRange;
    while( sCompList != NULL )
    {
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
            
        sCompList = sCompList->mNext;
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
     * @todo 불필요한 요소를 제거하여 Filter 재구성 (확장 필요)
     */
    
    sExeSortInstant->mHasFalseFilter =
        qlfIsAlwaysFalseRange( sExeSortInstant->mFetchInfo.mRange->mMinRange,
                               sExeSortInstant->mFetchInfo.mRange->mMaxRange );

    if( sExeSortInstant->mHasFalseFilter == STL_FALSE )
    {
        sExeSortInstant->mHasFalseFilter =
            qlfHasFalseFilter( sOptSortInstant->mIndexScanInfo->mLogicalKeyFilter,
                               aDataArea );

        if( sExeSortInstant->mHasFalseFilter == STL_FALSE )
        {
            sExeSortInstant->mHasFalseFilter =
                qlfHasFalseFilter( sOptSortInstant->mIndexScanInfo->mLogicalTableFilter,
                                   aDataArea );
        }
    }
    else
    {
        /* Do Nothing */
    }

    sExeSortInstant->mIsFirstExecute = STL_FALSE;

    
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
                       aDataArea->mExecNodeList[sOptSortInstant->mChildNode->mIdx].mOptNode,
                       aDataArea,
                       aEnv );
    
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SORT INSTANT TABLE node를 Fetch한다.
 * @param[in]      aFetchNodeInfo  Fetch Node Info
 * @param[out]     aUsedBlockCnt   Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF            End Of Fetch 여부
 * @param[in]      aEnv            Environment
 *
 * @remark Fetch 수행시 호출된다.
 */
stlStatus qlnxFetchSortInstantAccess( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                      stlInt64              * aUsedBlockCnt,
                                      stlBool               * aEOF,
                                      qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode       = NULL;
    qlnoInstant           * sOptSortInstant = NULL;
    qlnxInstant           * sExeSortInstant = NULL;
    qlnInstantTable       * sInstantTable   = NULL;

    stlInt64                sReadCnt        = 0;
    stlBool                 sEOF            = STL_FALSE;
    
    knlValueBlockList     * sValueBlock     = NULL;
    knlExprStack         ** sExprStack      = NULL;
    smlFetchInfo          * sFetchInfo      = NULL;
    stlInt32                sInsertColCnt   = 0;
    stlInt32                sLoop           = 0;
    knlExprEvalInfo         sEvalInfo;
    stlInt64                sFetchedCnt     = 0;
    stlInt64                sSkippedCnt     = 0;
    qlnxFetchNodeInfo       sLocalFetchInfo;

    QLL_OPT_DECLARE( stlTime sBeginBuildTime );
    QLL_OPT_DECLARE( stlTime sEndBuildTime );
    QLL_OPT_DECLARE( stlTime sBuildTime );

    QLL_OPT_DECLARE( stlTime sBeginFetchTime );
    QLL_OPT_DECLARE( stlTime sEndFetchTime );
    QLL_OPT_DECLARE( stlTime sFetchTime );

    QLL_OPT_DECLARE( stlTime sBeginExceptTime );
    QLL_OPT_DECLARE( stlTime sEndExceptTime );
    QLL_OPT_DECLARE( stlTime sExceptTime );

    QLL_OPT_SET_VALUE( sBuildTime, 0 );
    QLL_OPT_SET_VALUE( sFetchTime, 0 );
    QLL_OPT_SET_VALUE( sExceptTime, 0 );

    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aFetchNodeInfo->mOptNode->mType == QLL_PLAN_NODE_SORT_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    
    QLL_OPT_CHECK_TIME( sBeginFetchTime );

    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * SORT INSTANT TABLE Optimization Node 획득
     */

    sOptSortInstant = (qlnoInstant *) aFetchNodeInfo->mOptNode->mOptNode;

    
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aFetchNodeInfo->mDataArea,
                                        QLL_GET_OPT_NODE_IDX( aFetchNodeInfo->mOptNode ) );


    /**
     * SORT INSTANT TABLE Execution Node 획득
     */

    sExeSortInstant = sExecNode->mExecNode;


    /**
     * Used Block Count 초기화
     */
    
    *aUsedBlockCnt = 0;


    /**
     * Node 수행 여부 확인 
     */

    STL_DASSERT( *aEOF == STL_FALSE );
    STL_DASSERT( sExeSortInstant->mFetchInfo.mIsEndOfScan == STL_FALSE );
    STL_DASSERT( aFetchNodeInfo->mFetchCnt > 0);


    /**
     * Local Fetch Info 초기화
     */
    
    QLNX_SET_LOCAL_FETCH_INFO( & sLocalFetchInfo, aFetchNodeInfo );

    
    /**
     * @todo 불필요한 요소를 제거하여 Range 재구성 (확장 필요)
     */

    if( ( ( sOptSortInstant->mIndexScanInfo->mIsExist == STL_FALSE ) &&
          ( sOptSortInstant->mIndexScanInfo->mEmptyValue == STL_FALSE ) ) ||
        (  sExeSortInstant->mHasFalseFilter == STL_TRUE ) )
    {
        KNL_SET_ALL_BLOCK_USED_CNT( sExeSortInstant->mReadColBlock, sFetchedCnt );
        sExeSortInstant->mFetchInfo.mIsEndOfScan = STL_TRUE;
        *aEOF = STL_TRUE;
        STL_THROW( RAMP_EXIT );
    }
    else
    {
        /* Do Nothing */
    }

    
    /*****************************************
     * Fetch 수행하면서 Instant Table에 레코드 삽입
     ****************************************/

    sInstantTable = & sExeSortInstant->mInstantTable;
    sFetchInfo    = & sExeSortInstant->mFetchInfo;

    if( sInstantTable->mIsNeedBuild == STL_TRUE )
    {
        QLL_OPT_CHECK_TIME( sBeginBuildTime );

        sEvalInfo.mContext  = sLocalFetchInfo.mDataArea->mExprDataContext;
        sEvalInfo.mBlockIdx = 0;

        sExprStack  = sOptSortInstant->mColExprStack;
        sInsertColCnt = ( sOptSortInstant->mInstantNode->mKeyColCnt +
                          sOptSortInstant->mInstantNode->mRecColCnt );

        while( sEOF == STL_FALSE )
        {
            /**
             * Block Read
             */

            QLL_OPT_CHECK_TIME( sBeginExceptTime );

            /* fetch records*/
            sReadCnt = 0;
            QLNX_FETCH_NODE( & sLocalFetchInfo,
                             sOptSortInstant->mChildNode->mIdx,
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
                /**
                 * Evaluate Column Expression Stack
                 */
            
                sInstantTable->mRecCnt += sReadCnt;
                
                if( sOptSortInstant->mNeedEvalInsertCol == STL_TRUE )
                {
                    sEvalInfo.mBlockCount = sReadCnt;

                    sValueBlock = sExeSortInstant->mInsertColBlock;
                    for( sLoop = 0 ; sLoop < sInsertColCnt ; sLoop++ )
                    {
                        if( sExprStack[ sLoop ] != NULL )
                        {
                            sEvalInfo.mExprStack   = sExprStack[ sLoop ];
                            sEvalInfo.mResultBlock = sValueBlock;

                            STL_TRY( knlEvaluateBlockExpression( & sEvalInfo,
                                                                 KNL_ENV( aEnv ) )
                                     == STL_SUCCESS );
                        }                
                        sValueBlock = sValueBlock->mNext;
                    }
                }
                else
                {
                    /* Do Nothing */
                }

                
                /**
                 * Instant Table에 Record 삽입
                 */

                KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sExeSortInstant->mInsertColBlock, 0, sReadCnt );

                STL_TRY( smlInsertRecord( sLocalFetchInfo.mStmt,
                                          sInstantTable->mTableHandle,
                                          sExeSortInstant->mInsertColBlock,
                                          NULL, /* unique violation */
                                          sFetchInfo->mRowBlock,
                                          SML_ENV( aEnv ) )
                         == STL_SUCCESS );
            }
            else
            {
                STL_DASSERT( sEOF == STL_TRUE );
                break;
            }
        }
        
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

        if( sInstantTable->mRecCnt > 0 )
        {
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
        }
        else
        {
            sInstantTable->mIterator = NULL;
            KNL_SET_ALL_BLOCK_USED_CNT( sExeSortInstant->mReadColBlock, sFetchedCnt );
            *aEOF = STL_TRUE;
            STL_THROW( RAMP_EXIT );
        }

        QLL_OPT_CHECK_TIME( sEndBuildTime );
        QLL_OPT_ADD_ELAPSED_TIME( sBuildTime, sBeginBuildTime, sEndBuildTime );
        QLL_OPT_SET_VALUE( sExeSortInstant->mCommonInfo.mBuildTime, sBuildTime - sExceptTime );

        QLL_OPT_SET_VALUE( sExceptTime, 0 );
    }
    else
    {
        if( sInstantTable->mIterator == NULL )
        {
            KNL_SET_ALL_BLOCK_USED_CNT( sExeSortInstant->mReadColBlock, sFetchedCnt );
            *aEOF = STL_TRUE;
            STL_THROW( RAMP_EXIT );
        }
    }


    /*****************************************
     * Fetch
     ****************************************/

    /**
     * fetch 수행
     */
        
    sFetchInfo->mSkipCnt  = aFetchNodeInfo->mSkipCnt;
    sFetchInfo->mFetchCnt = aFetchNodeInfo->mFetchCnt;
        
    /* fetch records */
    STL_TRY( smlFetchNext( sInstantTable->mIterator,
                           sFetchInfo,
                           SML_ENV(aEnv) )
             == STL_SUCCESS );
        
    sFetchedCnt = SML_USED_BLOCK_SIZE( sFetchInfo->mRowBlock );
    sSkippedCnt = aFetchNodeInfo->mSkipCnt - sFetchInfo->mSkipCnt;
    
    *aEOF = sFetchInfo->mIsEndOfScan;

    sInstantTable->mCurIdx += sFetchedCnt;
    
    
    STL_RAMP( RAMP_EXIT );


    /**
     * OUTPUT 설정
     */

    *aUsedBlockCnt = sFetchedCnt;
    
    /* Parent의 Fetch Node Info 설정 */
    aFetchNodeInfo->mFetchCnt -= sFetchedCnt;
    aFetchNodeInfo->mSkipCnt  -= sSkippedCnt;
    

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeSortInstant->mCommonInfo.mFetchTime, sFetchTime -sBuildTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeSortInstant->mCommonInfo.mFetchCallCount, 1 );
    QLL_OPT_ADD_RECORD_STAT_INFO( sExeSortInstant->mCommonInfo.mFetchRecordStat,
                                  sExeSortInstant->mCommonInfo.mResultColBlock,
                                  *aUsedBlockCnt,
                                  aEnv );

    sExeSortInstant->mCommonInfo.mFetchRowCnt += *aUsedBlockCnt;

    return STL_SUCCESS;

    STL_CATCH( ERR_FREE_ITERATOR )
    {
        sInstantTable->mIterator = NULL;
    }
    
    STL_FINISH;

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeSortInstant->mCommonInfo.mFetchTime, sFetchTime -sBuildTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeSortInstant->mCommonInfo.mFetchCallCount, 1 );

    return STL_FAILURE;
}


/**
 * @brief SORT INSTANT TABLE node 수행을 종료한다.
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 *
 * @remark Execution을 종료하고자 할 때 호출한다.
 */
stlStatus qlnxFinalizeSortInstantAccess( qllOptimizationNode   * aOptNode,
                                         qllDataArea           * aDataArea,
                                         qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode       = NULL;
//    qlnoInstant           * sOptSortInstant = NULL;
    qlnxInstant           * sExeSortInstant = NULL;

    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SORT_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    
    /**
     * SORT INSTANT TABLE Optimization Node 획득
     */

//    sOptSortInstant = (qlnoInstant *) aOptNode->mOptNode;


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * 해당 노드를 포함하는 Query Expression Optimization Node 획득
     */

    /* sOptQueryNode = aOptNode->mOptCurQueryNode; */


    /**
     * SORT INSTANT ACCESS Execution Node 획득
     */

    sExeSortInstant = (qlnxInstant *) sExecNode->mExecNode;

    
    /**
     * Instant Table 해제
     */

    STL_TRY( qlnxDropInstantTable( & sExeSortInstant->mInstantTable,
                                   aEnv )
             == STL_SUCCESS );

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} qlnx */
