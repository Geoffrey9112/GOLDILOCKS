/*******************************************************************************
 * qlnxFlatInstantAccess.c
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
 * @file qlnxFlatInstantAccess.c
 * @brief SQL Processor Layer Execution Node - FLAT INSTANT TABLE
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnx
 * @{
 */

#if 0
/**
 * @brief FLAT INSTANT TABLE node를 초기화한다.
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
stlStatus qlnxInitializeFlatInstantAccess( smlTransId              aTransID,
                                           smlStatement          * aStmt,
                                           qllDBCStmt            * aDBCStmt,
                                           qllStatement          * aSQLStmt,
                                           qllOptimizationNode   * aOptNode,
                                           qllDataArea           * aDataArea,
                                           qllEnv                * aEnv )
{
    qllExecutionNode  * sExecNode       = NULL;
    qlnoInstant       * sOptFlatInstant = NULL;
    qlnxInstant       * sExeFlatInstant = NULL;

    stlBool             sIsScrollableInstant = STL_FALSE;
    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_FLAT_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * FLAT INSTANT TABLE Optimization Node 획득
     */

    sOptFlatInstant = (qlnoInstant *) aOptNode->mOptNode;


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * 해당 노드를 포함하는 Query Expression Optimization Node 획득
     */

    /* sOptQueryNode = aOptNode->mOptCurQueryNode; */


    /**
     * FLAT INSTANT ACCESS Execution Node 획득
     */

    sExeFlatInstant = (qlnxInstant *) sExecNode->mExecNode;

    
    /**
     * FLAT INSTANT TABLE 생성
     */

    STL_DASSERT( sExeFlatInstant->mNeedMaterialize == STL_TRUE );

    if ( sOptFlatInstant->mScrollable == ELL_CURSOR_SCROLLABILITY_SCROLL )
    {
        sIsScrollableInstant = STL_TRUE;
    }
    else
    {
        sIsScrollableInstant = STL_FALSE;
    }

    stlMemset( & sExeFlatInstant->mInstantTable, 0x00, STL_SIZEOF( qlnInstantTable ) );

    STL_TRY( qlnxCreateFlatInstantTable( aTransID,
                                         aStmt,
                                         & sExeFlatInstant->mInstantTable,
                                         sIsScrollableInstant,
                                         aEnv )
             == STL_SUCCESS );

    sExeFlatInstant->mHasFalseFilter = STL_FALSE;
        
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief FLAT INSTANT TABLE node를 수행한다.
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
stlStatus qlnxExecuteFlatInstantAccess( smlTransId              aTransID,
                                        smlStatement          * aStmt,
                                        qllDBCStmt            * aDBCStmt,
                                        qllStatement          * aSQLStmt,
                                        qllOptimizationNode   * aOptNode,
                                        qllDataArea           * aDataArea,
                                        qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode       = NULL;
    qlnoInstant           * sOptFlatInstant = NULL;
    qlnxInstant           * sExeFlatInstant = NULL;
    qlnInstantTable       * sInstantTable   = NULL;

    knlPhysicalFilter     * sPhysicalFilter = NULL;
    smlFetchInfo          * sFetchInfo      = NULL;


    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_FLAT_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * FLAT INSTANT TABLE Optimization Node 획득
     */

    sOptFlatInstant = (qlnoInstant *) aOptNode->mOptNode;


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * 해당 노드를 포함하는 Query Expression Optimization Node 획득
     */

    /* sOptQueryNode = aOptNode->mOptCurQueryNode; */


    /**
     * FLAT INSTANT ACCESS Execution Node 획득
     */

    sExeFlatInstant = (qlnxInstant *) sExecNode->mExecNode;


    /*****************************************
     * Flat Instant Table Iterator 초기화
     ****************************************/

    /**
     * Cursor를 First Record 위치로 이동
     */

    sInstantTable = & sExeFlatInstant->mInstantTable;
    
    sInstantTable->mStmt   = aStmt;
    sInstantTable->mCurIdx = 0;

    if( sInstantTable->mIsNeedBuild == STL_FALSE )
    {
        if( sInstantTable->mIterator != NULL )
        {
            STL_TRY( smlResetIterator( sInstantTable->mIterator,
                                       SML_ENV( aEnv ) )
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

    sFetchInfo = & sExeFlatInstant->mFetchInfo;

    sFetchInfo->mIsEndOfScan = STL_FALSE;
    sFetchInfo->mSkipCnt     = 0;
    sFetchInfo->mFetchCnt    = QLL_FETCH_COUNT_MAX;


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


    /**
     * @todo 불필요한 요소를 제거하여 Filter 재구성 (확장 필요)
     */
    
    sExeFlatInstant->mHasFalseFilter =
        qlfHasFalseFilter( sOptFlatInstant->mTableLogicalFilterExpr,
                           aDataArea );
    
        
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
                       aDataArea->mExecNodeList[sOptFlatInstant->mChildNode->mIdx].mOptNode,
                       aDataArea,
                       aEnv );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief FLAT INSTANT TABLE node를 Fetch한다.
 * @param[in]      aTransID      Transaction ID
 * @param[in]      aStmt         Statement
 * @param[in]      aDBCStmt      DBC Statement
 * @param[in,out]  aSQLStmt      SQL Statement
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aStartIdx     Start Index
 * @param[in]      aSkipCnt      Skip Record Count
 * @param[in]      aFetchCnt     Fetch Record Count
 * @param[out]     aUsedBlockCnt Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF          End Of Fetch 여부
 * @param[in]      aEnv          Environment
 *
 * @remark Fetch 수행시 호출된다.
 */
stlStatus qlnxFetchFlatInstantAccess( smlTransId              aTransID,
                                      smlStatement          * aStmt,
                                      qllDBCStmt            * aDBCStmt,
                                      qllStatement          * aSQLStmt,
                                      qllOptimizationNode   * aOptNode,
                                      qllDataArea           * aDataArea,
                                      stlInt32                aStartIdx,
                                      stlInt64                aSkipCnt,
                                      stlInt64                aFetchCnt,
                                      stlInt64              * aUsedBlockCnt,
                                      stlBool               * aEOF,
                                      qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode       = NULL;
    qlnoInstant           * sOptFlatInstant = NULL;
    qlnxInstant           * sExeFlatInstant = NULL;
    qlnInstantTable       * sInstantTable   = NULL;

    stlInt64                sSkipCnt        = 0;
    stlInt64                sFetchCnt       = 0;
    stlInt64                sReadCnt        = 0;
    stlBool                 sEOF            = STL_FALSE;
    
    knlValueBlockList     * sValueBlock     = NULL;
    knlExprStack         ** sExprStack      = NULL;
    smlFetchInfo          * sFetchInfo      = NULL;
    stlInt32                sInsertColCnt   = 0;
    stlInt32                sLoop           = 0;
    knlExprEvalInfo         sEvalInfo;

 
    QLL_OPT_DECLARE( stlTime sBeginBuildTime );
    QLL_OPT_DECLARE( stlTime sEndBuildTime );

    QLL_OPT_DECLARE( stlTime sBeginFetchTime );
    QLL_OPT_DECLARE( stlTime sEndFetchTime );

    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_FLAT_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    
    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * FLAT INSTANT TABLE Optimization Node 획득
     */

    sOptFlatInstant = (qlnoInstant *) aOptNode->mOptNode;

    
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * FLAT INSTANT TABLE Execution Node 획득
     */

    sExeFlatInstant = sExecNode->mExecNode;


    /**
     * Used Block Count 초기화
     */
    
    *aUsedBlockCnt = 0;


    /**
     * Node 수행 여부 확인 
     */

    STL_DASSERT( *aEOF == STL_FALSE );
    STL_DASSERT( sExeFlatInstant->mFetchInfo.mIsEndOfScan == STL_FALSE );
    STL_DASSERT( aFetchCnt > 0);


    /*****************************************
     * Node 수행 여부 확인
     ****************************************/

    if( sExeFlatInstant->mHasFalseFilter == STL_TRUE )
    {
        KNL_SET_ALL_BLOCK_USED_CNT( sExeFlatInstant->mReadColBlock, 0 );
        *aEOF = STL_TRUE;
        *aUsedBlockCnt = 0;
        STL_THROW( RAMP_EXIT );
    }
    else
    {
        /* Do Nothing */
    }

    
    /*****************************************
     * Fetch 수행하면서 Instant Table에 레코드 삽입
     ****************************************/

    sInstantTable = & sExeFlatInstant->mInstantTable;
    sFetchInfo    = & sExeFlatInstant->mFetchInfo;

    if( sInstantTable->mIsNeedBuild == STL_TRUE )
    {
        sEvalInfo.mContext  = aDataArea->mExprDataContext;
        sEvalInfo.mBlockIdx = 0;

        sExprStack    = sOptFlatInstant->mColExprStack;
        sInsertColCnt = sOptFlatInstant->mInstantNode->mRecColCnt;
        sFetchCnt     = aFetchCnt;

        if( sOptFlatInstant->mScrollable == ELL_CURSOR_SCROLLABILITY_SCROLL )
        {
            sSkipCnt = 0;
        }
        else
        {
            sSkipCnt = aSkipCnt;
            aSkipCnt = 0;
        }
        
        while( ( sEOF == STL_FALSE ) && ( sFetchCnt > 0 ) )
        {
            /**
             * Block Read
             */

            /* fetch records*/
            sReadCnt = 0;
            QLNX_FETCH_NODE( aTransID,
                             aStmt,
                             aDBCStmt,
                             aSQLStmt,
                             sOptFlatInstant->mChildNode->mIdx,
                             aDataArea,
                             0, /* Start Idx */
                             sSkipCnt,  /* Skip Count */
                             sFetchCnt, /* Fetch Count */
                             & sReadCnt,
                             & sEOF,
                             aEnv );

            if( sReadCnt > 0 )
            {
                QLL_OPT_CHECK_TIME( sBeginBuildTime );

                /**
                 * Evaluate Column Expression Stack
                 */
            
                sInstantTable->mRecCnt += sReadCnt;

                if( sOptFlatInstant->mNeedEvalInsertCol == STL_TRUE )
                {
                    sEvalInfo.mBlockCount = sReadCnt;

                    sValueBlock = sExeFlatInstant->mInsertColBlock;
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

                KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sExeFlatInstant->mInsertColBlock, 0, sReadCnt );

                STL_TRY( smlInsertRecord( aStmt,
                                          sInstantTable->mTableHandle,
                                          sExeFlatInstant->mInsertColBlock,
                                          NULL, /* unique violation */
                                          sFetchInfo->mRowBlock,
                                          SML_ENV( aEnv ) )
                         == STL_SUCCESS );
                    
                sSkipCnt   = 0;
                sFetchCnt -= sReadCnt;

                QLL_OPT_CHECK_TIME( sEndBuildTime );
                QLL_OPT_ADD_ELAPSED_TIME( sExeFlatInstant->mCommonInfo.mBuildTime, sBeginBuildTime, sEndBuildTime );
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
            
            STL_TRY( smlAllocIterator( aStmt,
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

            KNL_SET_ALL_BLOCK_USED_CNT( sExeFlatInstant->mReadColBlock, 0 );
            *aEOF = STL_TRUE;
            *aUsedBlockCnt = 0;
            STL_THROW( RAMP_EXIT );
        }
    }
    else
    {
        STL_PARAM_VALIDATE( sInstantTable->mIterator != NULL,
                            QLL_ERROR_STACK(aEnv) );
    }


    QLL_OPT_CHECK_TIME( sBeginFetchTime );

    /*****************************************
     * Fetch
     ****************************************/

    /**
     * Instant Table에 대한 Read Records
     */

    if( sInstantTable->mRecCnt <= sInstantTable->mCurIdx + aSkipCnt )
    {
        KNL_SET_ALL_BLOCK_USED_CNT( sExeFlatInstant->mReadColBlock, 0 );
        *aEOF = STL_TRUE;
        *aUsedBlockCnt = 0;
    }
    else
    {
        /**
         * Skip Count 와 Fetch Count 조정
         */
        
        sFetchInfo->mSkipCnt  = aSkipCnt;
        sFetchInfo->mFetchCnt = aFetchCnt;
        
        /* fetch records */
        STL_TRY( smlFetchNext( sInstantTable->mIterator,
                               sFetchInfo,
                               SML_ENV(aEnv) )
                 == STL_SUCCESS );
        
        *aEOF = sFetchInfo->mIsEndOfScan;

        *aUsedBlockCnt = sFetchInfo->mRowBlock->mUsedBlockSize;

        sInstantTable->mCurIdx += *aUsedBlockCnt;
    }

    
    STL_RAMP( RAMP_EXIT );

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sExeFlatInstant->mCommonInfo.mFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_COUNT( sExeFlatInstant->mCommonInfo.mFetchCallCount, 1 );
    QLL_OPT_ADD_RECORD_STAT_INFO( sExeFlatInstant->mCommonInfo.mFetchRecordStat,
                                  sExeFlatInstant->mCommonInfo.mResultColBlock,
                                  *aUsedBlockCnt,
                                  aEnv );

    sExeFlatInstant->mCommonInfo.mFetchRowCnt += *aUsedBlockCnt;

    return STL_SUCCESS;

    STL_CATCH( ERR_FREE_ITERATOR )
    {
        sInstantTable->mIterator = NULL;
    }
    
    STL_FINISH;

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sExeFlatInstant->mCommonInfo.mFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_COUNT( sExeFlatInstant->mCommonInfo.mFetchCallCount, 1 );

    return STL_FAILURE;
}


/**
 * @brief FLAT INSTANT TABLE node 수행을 종료한다.
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 *
 * @remark Execution을 종료하고자 할 때 호출한다.
 */
stlStatus qlnxFinalizeFlatInstantAccess( qllOptimizationNode   * aOptNode,
                                         qllDataArea           * aDataArea,
                                         qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode       = NULL;
    /* qlnoInstant           * sOptFlatInstant = NULL; */
    qlnxInstant           * sExeFlatInstant = NULL;

    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_FLAT_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    
    /**
     * FLAT INSTANT TABLE Optimization Node 획득
     */

    /* sOptFlatInstant = (qlnoInstant *) aOptNode->mOptNode; */


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * 해당 노드를 포함하는 Query Expression Optimization Node 획득
     */

    /* sOptQueryNode = aOptNode->mOptCurQueryNode; */


    /**
     * FLAT INSTANT ACCESS Execution Node 획득
     */

    sExeFlatInstant = (qlnxInstant *) sExecNode->mExecNode;

    
    /**
     * Instant Table 해제
     */

    STL_TRY( qlnxDropInstantTable( & sExeFlatInstant->mInstantTable,
                                   aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
#endif


/**
 * @brief INSTANT TABLE 을 DROP 한다.
 * @param[in]      aInstantTable  Instant Table (flat/sort/hash/group 공통)
 * @param[in]      aEnv           Environment
 */
stlStatus qlnxDropInstantTable( qlnInstantTable  * aInstantTable,
                                qllEnv           * aEnv )
{
    stlInt32 sState = 3;
    
    /*
     * Parameter Validation
     */

    STL_TRY_THROW( aInstantTable != NULL, RAMP_EXIT );
    

    /**
     * Drop Instant Table
     */
    
    sState = 2;
    if( aInstantTable->mIndexHandle != NULL )
    {
        STL_DASSERT( aInstantTable->mStmt != NULL );
        
        STL_TRY( smlDropIndex( aInstantTable->mStmt,
                               aInstantTable->mIndexHandle,
                               SML_ENV( aEnv ) )
                 == STL_SUCCESS );

        aInstantTable->mIndexHandle = NULL;
    }
            
    sState = 1;
    if( aInstantTable->mTableHandle != NULL )
    {
        STL_DASSERT( aInstantTable->mStmt != NULL );
        
        STL_TRY( smlDropTable( aInstantTable->mStmt,
                               aInstantTable->mTableHandle,
                               SML_ENV( aEnv ) )
                 == STL_SUCCESS );

        aInstantTable->mTableHandle = NULL;
    }

    sState = 0;
    if( aInstantTable->mIterator != NULL )
    {
        (void) smlFreeIterator( aInstantTable->mIterator, SML_ENV(aEnv) );
        aInstantTable->mIterator = NULL;
    }

    aInstantTable->mStmt = NULL;

    STL_RAMP( RAMP_EXIT );
    
    return STL_SUCCESS;

    STL_FINISH;

    /* Instant Table의 share memory 관련 자원해제는 실패하지 않아야 한다. */
    STL_DASSERT( 0 );

    switch (sState)
    {
        case 3:
            if( aInstantTable->mIndexHandle != NULL )
            {
                (void) smlDropIndex( aInstantTable->mStmt,
                                     aInstantTable->mIndexHandle,
                                     SML_ENV( aEnv ) );
            }
        case 2:
            aInstantTable->mIndexHandle = NULL;
            
            if( aInstantTable->mTableHandle != NULL )
            {
                (void) smlDropTable( aInstantTable->mStmt,
                                     aInstantTable->mTableHandle,
                                     SML_ENV( aEnv ) );
            }
        case 1:
            aInstantTable->mTableHandle = NULL;
            
            if( aInstantTable->mIterator != NULL )
            {
                (void) smlFreeIterator( aInstantTable->mIterator, SML_ENV(aEnv) );
            }
        default:
            aInstantTable->mIterator = NULL;
            break;
    }

    aInstantTable->mStmt = NULL;
    
    return STL_FAILURE;
}


/**
 * @brief Instant Table 을 Truncate 한다.
 * @param[in] aStmt              Statement
 * @param[in] aInstantTable      Instant Table
 * @param[in] aEnv               Environment
 * @remarks
 */
stlStatus qlnxTruncateInstantTable( smlStatement    * aStmt,
                                    qlnInstantTable * aInstantTable,
                                    qllEnv          * aEnv )
{
    stlInt64   sNewRelationId = 0;
    
    /**
     * Parameter Validation 
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInstantTable != NULL, QLL_ERROR_STACK(aEnv) );
    
    /**
     * Instant Table 을 Truncate
     */
    
    STL_TRY( smlTruncateTable( aStmt,
                               aInstantTable->mTableHandle,
                               SML_DROP_STORAGE_REUSE,
                               & aInstantTable->mTableHandle,
                               & sNewRelationId,
                               SML_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Instant Table의 Index 를 Truncate
     */

    if( aInstantTable->mIndexHandle != NULL )
    {
        STL_TRY( smlTruncateIndex( aStmt,
                                   aInstantTable->mIndexHandle,
                                   aInstantTable->mTableHandle,
                                   SML_DROP_STORAGE_REUSE,
                                   & aInstantTable->mIndexHandle,
                                   & sNewRelationId,
                                   SML_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    /**
     * Instant Table 정보 초기화 
     */
    
    aInstantTable->mIsNeedBuild = STL_TRUE;
    aInstantTable->mRecCnt      = 0;
    aInstantTable->mCurIdx      = 0;
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( 0 );

    return STL_FAILURE;
}



/** @} qlnx */


