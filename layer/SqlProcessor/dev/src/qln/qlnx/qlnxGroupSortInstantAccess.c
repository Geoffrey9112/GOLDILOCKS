/*******************************************************************************
 * qlnxGroupSortInstantAccess.c
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
 * @file qlnxGroupSortInstantAccess.c
 * @brief SQL Processor Layer Execution Node - GROUP SORT INSTANT TABLE
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnx
 * @{
 */

#if 0
stlStatus qlnxFetchGroupSortInstantAccessPipeline( smlTransId              aTransID,
                                                   smlStatement          * aStmt,
                                                   qllDBCStmt            * aDBCStmt,
                                                   qllStatement          * aSQLStmt,
                                                   qllOptimizationNode   * aOptNode,
                                                   qllDataArea           * aDataArea,
                                                   stlInt64                aSkipCnt,
                                                   stlInt64                aFetchCnt,
                                                   stlInt64              * aUsedBlockCnt,
                                                   stlBool               * aEOF,
                                                   qllEnv                * aEnv );
#endif


#if 0
/**
 * @brief GROUP INSTANT TABLE node를 초기화한다.
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
stlStatus qlnxInitializeGroupSortInstantAccess( smlTransId              aTransID,
                                                smlStatement          * aStmt,
                                                qllDBCStmt            * aDBCStmt,
                                                qllStatement          * aSQLStmt,
                                                qllOptimizationNode   * aOptNode,
                                                qllDataArea           * aDataArea,
                                                qllEnv                * aEnv )
{
    qllExecutionNode  * sExecNode        = NULL;
    qlnoInstant       * sOptGroupInstant = NULL;
    qlnxInstant       * sExeGroupInstant = NULL;
    qlnfAggrExecInfo  * sAggrExecInfo    = NULL;
    stlInt32            sLoop            = 0;

    stlBool             sIsScrollableInstant = STL_FALSE;
    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_GROUP_SORT_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**
     * GROUP INSTANT TABLE Optimization Node 획득
     */

    sOptGroupInstant = (qlnoInstant *) aOptNode->mOptNode;


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * 해당 노드를 포함하는 Query Expression Optimization Node 획득
     */

    /* sOptQueryNode = aOptNode->mOptCurQueryNode; */


    /**
     * GROUP INSTANT ACCESS Execution Node 획득
     */

    sExeGroupInstant = (qlnxInstant *) sExecNode->mExecNode;

    STL_DASSERT( sExeGroupInstant->mKeyColBlock != NULL );
    STL_DASSERT( sExeGroupInstant->mInsertColBlock != NULL );

    
    /**
     * GROUP INSTANT TABLE 생성
     */

    stlMemset( sExeGroupInstant->mInstantTable, 0x00, STL_SIZEOF( qlnInstantTable ) );
    
    if( sExeGroupInstant->mNeedMaterialize == STL_TRUE )
    {
        STL_TRY( qlnxCreateGroupSortInstantTable( aTransID,
                                                  aStmt,
                                                  & sExeGroupInstant->mInstantTable,
                                                  NULL,
                                                  sOptGroupInstant->mInstantNode->mKeyColCnt,
                                                  sExeGroupInstant->mKeyColBlock,
                                                  sOptGroupInstant->mInstantNode->mKeyFlags,
                                                  sOptGroupInstant->mSortTableAttr.mTopDown,
                                                  sOptGroupInstant->mSortTableAttr.mVolatile,
                                                  sOptGroupInstant->mSortTableAttr.mLeafOnly,
                                                  sOptGroupInstant->mSortTableAttr.mUnique,
                                                  sOptGroupInstant->mSortTableAttr.mNullExcluded,
                                                  aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_DASSERT( sExeGroupInstant->mHasAggr == STL_FALSE );
        STL_DASSERT( sOptGroupInstant->mInstantNode->mRecColCnt == 0 );
        
        STL_TRY( qlnxCreateSortInstantTable( aTransID,
                                             aStmt,
                                             & sExeGroupInstant->mInstantTable,
                                             NULL,
                                             sOptGroupInstant->mInstantNode->mKeyColCnt,
                                             sExeGroupInstant->mKeyColBlock,
                                             sOptGroupInstant->mInstantNode->mKeyFlags,
                                             STL_TRUE,
                                             STL_FALSE,
                                             STL_TRUE,
                                             STL_TRUE,
                                             STL_FALSE,
                                             aEnv )
                 == STL_SUCCESS );
    }
    

    /**
     * GROUP-AGGREGATION INSTANT TABLE 생성
     */

    if( sExeGroupInstant->mHasAggr == STL_TRUE )
    {
        /**
         * Aggregation 결과를 저장할 Sort Instant Table 생성
         */

        stlMemset( sExeGroupInstant->mAggrDistInstantTable, 0x00, STL_SIZEOF( qlnInstantTable ) );
    
        if( ( sOptGroupInstant->mIndexScanInfo->mIsExist == STL_FALSE ) &&
            ( sOptGroupInstant->mIndexScanInfo->mEmptyValue == STL_TRUE ) )
        {
            if ( sOptGroupInstant->mScrollable == ELL_CURSOR_SCROLLABILITY_SCROLL )
            {
                sIsScrollableInstant = STL_TRUE;
            }
            else
            {
                sIsScrollableInstant = STL_FALSE;
            }
            
            STL_TRY( qlnxCreateFlatInstantTable( aTransID,
                                                 aStmt,
                                                 sExeGroupInstant->mAggrDistInstantTable,
                                                 sIsScrollableInstant,
                                                 aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            STL_TRY( qlnxCreateSortInstantTable( aTransID,
                                                 aStmt,
                                                 sExeGroupInstant->mAggrDistInstantTable,
                                                 NULL,
                                                 sOptGroupInstant->mInstantNode->mKeyColCnt,
                                                 sExeGroupInstant->mAggrDistKeyColBlock,
                                                 sOptGroupInstant->mInstantNode->mKeyFlags,
                                                 sOptGroupInstant->mSortTableAttr.mTopDown,
                                                 sOptGroupInstant->mSortTableAttr.mVolatile,
                                                 sOptGroupInstant->mSortTableAttr.mLeafOnly,
                                                 sOptGroupInstant->mSortTableAttr.mUnique,
                                                 sOptGroupInstant->mSortTableAttr.mNullExcluded,
                                                 aEnv )
                     == STL_SUCCESS );
        }
        
        
        /**
         * Aggregation Distinct를 위한 Instant Table 생성
         */

        sAggrExecInfo = sExeGroupInstant->mAggrDistExecInfo;

        for( sLoop = 0 ; sLoop < sOptGroupInstant->mAggrFuncCnt ; sLoop++ )
        {
            stlMemset( sAggrExecInfo->mInstantTable, 0x00, STL_SIZEOF( qlnInstantTable ) );
        
            if( sAggrExecInfo->mInstantTable != NULL )
            {
                STL_TRY( qlnxCreateSortInstantTable( aTransID,
                                                     aStmt,
                                                     sAggrExecInfo->mInstantTable,
                                                     NULL,
                                                     sAggrExecInfo->mInputArgCnt,
                                                     sAggrExecInfo->mInputArgList,
                                                     sAggrExecInfo->mInputArgKeyColFlag,
                                                     STL_TRUE,
                                                     STL_FALSE,
                                                     STL_FALSE,
                                                     STL_TRUE,
                                                     STL_FALSE,
                                                     aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                /* Do Nothing */
            }
            sAggrExecInfo++;
        }
    }
    else
    {
        sExeGroupInstant->mAggrDistInstantTable = NULL;
    }

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief GROUP INSTANT TABLE node를 수행한다.
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
stlStatus qlnxExecuteGroupSortInstantAccess( smlTransId              aTransID,
                                             smlStatement          * aStmt,
                                             qllDBCStmt            * aDBCStmt,
                                             qllStatement          * aSQLStmt,
                                             qllOptimizationNode   * aOptNode,
                                             qllDataArea           * aDataArea,
                                             qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode        = NULL;
    qlnoInstant           * sOptGroupInstant = NULL;
    qlnxInstant           * sExeGroupInstant = NULL;
    qlnInstantTable       * sInstantTable    = NULL;

    knlCompareList        * sCompList        = NULL;
    knlPhysicalFilter     * sPhysicalFilter  = NULL;
    smlFetchInfo          * sFetchInfo       = NULL;


    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_GROUP_SORT_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * GROUP INSTANT TABLE Optimization Node 획득
     */

    sOptGroupInstant = (qlnoInstant *) aOptNode->mOptNode;


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * 해당 노드를 포함하는 Query Expression Optimization Node 획득
     */

    /* sOptQueryNode = aOptNode->mOptCurQueryNode; */


    /**
     * GROUP INSTANT ACCESS Execution Node 획득
     */

    sExeGroupInstant = (qlnxInstant *) sExecNode->mExecNode;

    /*****************************************
     * Group Instant Table Iterator 초기화
     ****************************************/

    /**
     * Cursor를 First Record 위치로 이동
     */
    
    sInstantTable = & sExeGroupInstant->mInstantTable;
    
    sInstantTable->mStmt   = aStmt;
    sInstantTable->mCurIdx = 0;

    if( sExeGroupInstant->mNeedMaterialize == STL_TRUE )
    {
        if( sInstantTable->mIsNeedBuild == STL_FALSE )
        {
            STL_TRY_THROW( sExeGroupInstant->mAggrDistInstantTable == NULL,
                           RAMP_AGGREGATION_INSTANT );
        
            STL_TRY( smlResetIterator( sInstantTable->mIterator,
                                       SML_ENV( aEnv ) )
                     == STL_SUCCESS );
        }
        else
        {
            /* Do Nothing */
        }
    }
    else
    {
        if( sExeGroupInstant->mInstantTable.mTableHandle != NULL )
        {
            STL_DASSERT( sExeGroupInstant->mAggrDistInstantTable == NULL );
        
            STL_TRY( qlnxDropInstantTable( & sExeGroupInstant->mInstantTable,
                                           aEnv )
                     == STL_SUCCESS );

            stlMemset( & sExeGroupInstant->mInstantTable, 0x00, STL_SIZEOF( qlnInstantTable ) );

            STL_TRY( qlnxCreateSortInstantTable( aTransID,
                                                 aStmt,
                                                 & sExeGroupInstant->mInstantTable,
                                                 NULL,
                                                 sOptGroupInstant->mInstantNode->mKeyColCnt,
                                                 sExeGroupInstant->mKeyColBlock,
                                                 sOptGroupInstant->mInstantNode->mKeyFlags,
                                                 STL_TRUE,
                                                 STL_FALSE,
                                                 STL_FALSE,
                                                 STL_TRUE,
                                                 STL_FALSE,
                                                 aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /* Do Nothing */
        }

        STL_THROW( RAMP_SKIP_AGGRGATION );
    }

    
    /**
     * Instant Table Iterator 초기화
     */

    sFetchInfo = & sExeGroupInstant->mFetchInfo;
    
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
            sCompList->mRangeVal = sCompList->mConstVal->mValue;
            sCompList->mRangeLen = sCompList->mConstVal->mLength;
        }
            
        sCompList = sCompList->mNext;
    }

    sCompList = sFetchInfo->mRange->mMaxRange;
    while( sCompList != NULL )
    {
        if( sCompList->mConstVal != NULL )
        {
            sCompList->mRangeVal = sCompList->mConstVal->mValue;
            sCompList->mRangeLen = sCompList->mConstVal->mLength;
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
    
    sExeGroupInstant->mHasFalseFilter =
        qlfIsAlwaysFalseRange( sExeGroupInstant->mFetchInfo.mRange->mMinRange,
                               sExeGroupInstant->mFetchInfo.mRange->mMaxRange );

    
    /*****************************************
     * Group Aggregation Instant Table Iterator 초기화
     ****************************************/

    STL_RAMP( RAMP_AGGREGATION_INSTANT );
    STL_TRY_THROW( sExeGroupInstant->mHasAggr == STL_TRUE, RAMP_SKIP_AGGRGATION );
    
    
    /**
     * Cursor를 First Record 위치로 이동
     */
    
    sInstantTable = sExeGroupInstant->mAggrDistInstantTable;

    sInstantTable->mStmt   = aStmt;
    sInstantTable->mCurIdx = 0;

    if( sInstantTable->mIsNeedBuild == STL_FALSE )
    {
        STL_TRY( smlResetIterator( sInstantTable->mIterator,
                                   SML_ENV( aEnv ) )
                 == STL_SUCCESS );
    }
    else
    {
        /* Do Nothing */
    }
    

    /**
     * Instant Table Iterator 초기화
     */

    sFetchInfo = sExeGroupInstant->mAggrDistFetchInfo;
    
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
            sCompList->mRangeVal = sCompList->mConstVal->mValue;
            sCompList->mRangeLen = sCompList->mConstVal->mLength;
        }
            
        sCompList = sCompList->mNext;
    }

    sCompList = sFetchInfo->mRange->mMaxRange;
    while( sCompList != NULL )
    {
        if( sCompList->mConstVal != NULL )
        {
            sCompList->mRangeVal = sCompList->mConstVal->mValue;
            sCompList->mRangeLen = sCompList->mConstVal->mLength;
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
    
    sExeGroupInstant->mHasFalseFilter =
        qlfIsAlwaysFalseRange( sExeGroupInstant->mAggrDistFetchInfo->mRange->mMinRange,
                               sExeGroupInstant->mAggrDistFetchInfo->mRange->mMaxRange );


    STL_RAMP( RAMP_SKIP_AGGRGATION );


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
                       aDataArea->mExecNodeList[sOptGroupInstant->mChildNode->mIdx].mOptNode,
                       aDataArea,
                       aEnv );
    
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief GROUP INSTANT TABLE node를 Fetch한다.
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
stlStatus qlnxFetchGroupSortInstantAccess( smlTransId              aTransID,
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
    qllExecutionNode      * sExecNode        = NULL;
    qlnoInstant           * sOptGroupInstant = NULL;
    qlnxInstant           * sExeGroupInstant = NULL;
    qlnInstantTable       * sInstantTable    = NULL;

    stlInt64                sReadCnt         = 0;
    stlBool                 sEOF             = STL_FALSE;
    
    knlValueBlockList     * sValueBlock      = NULL;
    knlExprStack          * sExprStack       = NULL;
    smlFetchInfo          * sFetchInfo       = NULL;
    stlInt32                sInsertColCnt    = 0;
    stlInt32                sLoop            = 0;
    knlExprEvalInfo         sEvalInfo;

    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_GROUP_SORT_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    
    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * GROUP INSTANT TABLE Optimization Node 획득
     */

    sOptGroupInstant = (qlnoInstant *) aOptNode->mOptNode;

    
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * GROUP INSTANT TABLE Execution Node 획득
     */

    sExeGroupInstant = sExecNode->mExecNode;


    /**
     * Used Block Count 초기화
     */
    
    *aUsedBlockCnt = 0;


    /*****************************************
     * Aggregation Fetch 수행
     ****************************************/

    /**
     * Aggregation 포함된 fetch 수행
     */

    if( sExeGroupInstant->mAggrDistExecInfo != NULL )
    {
        return qlnxFetchAggrGroupSortInstantAccess( aTransID,
                                                    aStmt,
                                                    aDBCStmt,
                                                    aSQLStmt,
                                                    aOptNode,
                                                    aDataArea,
                                                    aSkipCnt,
                                                    aFetchCnt,
                                                    aUsedBlockCnt,
                                                    aEOF,
                                                    aEnv );
    }
    /* else if( sExeGroupInstant->mNeedMaterialize == STL_FALSE ) */
    /* { */
    /*     return qlnxFetchGroupSortInstantAccessPipeline( aTransID, */
    /*                                                 aStmt, */
    /*                                                 aDBCStmt, */
    /*                                                 aSQLStmt, */
    /*                                                 aOptNode, */
    /*                                                 aDataArea, */
    /*                                                 aSkipCnt, */
    /*                                                 aFetchCnt, */
    /*                                                 aUsedBlockCnt, */
    /*                                                 aEOF, */
    /*                                                 aEnv ); */
    /* } */
    else
    {
        /**
         * Node 수행 여부 확인 
         */

        STL_DASSERT( *aEOF == STL_FALSE );
        STL_DASSERT( sExeGroupInstant->mFetchInfo.mIsEndOfScan == STL_FALSE );
        STL_DASSERT( aFetchCnt > 0);

        
        /**
         * @todo 불필요한 요소를 제거하여 Range 재구성 (확장 필요)
         */

        if( ( ( sOptGroupInstant->mIndexScanInfo->mIsExist == STL_FALSE ) &&
              ( sOptGroupInstant->mIndexScanInfo->mEmptyValue == STL_FALSE ) ) ||
            (  sExeGroupInstant->mHasFalseFilter == STL_TRUE ) )
        {
            sExeGroupInstant->mFetchInfo.mIsEndOfScan = STL_TRUE;
            *aEOF = STL_TRUE;
            STL_THROW( RAMP_EXIT );
        }
        else
        {
            /* Do Nothing */
        }
    }

    
    /*****************************************
     * Fetch 수행하면서 Instant Table에 레코드 삽입
     ****************************************/

    sInstantTable = & sExeGroupInstant->mInstantTable;
    sFetchInfo    = & sExeGroupInstant->mFetchInfo;

    if( sInstantTable->mIsNeedBuild == STL_TRUE )
    {
        sEvalInfo.mContext  = aDataArea->mExprDataContext;
        sEvalInfo.mBlockIdx = 0;

        sExprStack    = sOptGroupInstant->mColExprStack;
        sInsertColCnt = ( sOptGroupInstant->mInstantNode->mKeyColCnt +
                          sOptGroupInstant->mInstantNode->mRecColCnt );

        while( sEOF == STL_FALSE )
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
                             sOptGroupInstant->mChildNode->mIdx,
                             aDataArea,
                             0, /* Start Idx */
                             0, /* Skip Count */
                             QLL_FETCH_COUNT_MAX, /* Fetch Count */
                             & sReadCnt,
                             & sEOF,
                             aEnv );

            if( sReadCnt > 0 )
            {
                /**
                 * Evaluate Column Expression Stack
                 */
            
                sInstantTable->mRecCnt += sReadCnt;
                
                sEvalInfo.mBlockCount = sReadCnt;

                sValueBlock = sExeGroupInstant->mInsertColBlock;
                for( sLoop = 0 ; sLoop < sInsertColCnt ; sLoop++ )
                {
                    sEvalInfo.mExprStack   = & sExprStack[ sLoop ];
                    sEvalInfo.mResultBlock = sValueBlock;

                    STL_TRY( knlEvaluateBlockExpression( & sEvalInfo,
                                                         KNL_ENV( aEnv ) )
                             == STL_SUCCESS );
                
                    sValueBlock = sValueBlock->mNext;
                }


                /**
                 * Instant Table에 Record 삽입
                 */

                KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sExeGroupInstant->mInsertColBlock, 0, sReadCnt );

                STL_TRY( smlInsertRecord( aStmt,
                                          sInstantTable->mTableHandle,
                                          sExeGroupInstant->mInsertColBlock,
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
            SML_SET_ITERATOR_PROP( sInstantTable->mIteratorProperty,
                                   0,
                                   STL_UINT64_MAX,
                                   NULL,
                                   STL_TRUE,     /* aGroupFetch */
                                   STL_FALSE );  /* aSkipHitRecord */
            
            STL_TRY( smlAllocIterator( aStmt,
                                       sInstantTable->mTableHandle,
                                       SML_TRM_SNAPSHOT,
                                       SML_SRM_SNAPSHOT,
                                       & sInstantTable->mIteratorProperty,
                                       SML_SCAN_FORWARD,
                                       & sInstantTable->mIterator,
                                       SML_ENV( aEnv ) )
                     == STL_SUCCESS );

            /* move to first group */
            STL_TRY( smlMoveToNextKey( sInstantTable->mIterator,
                                       sFetchInfo,
                                       SML_ENV(aEnv) )
                     == STL_SUCCESS );

            if( sFetchInfo->mIsEndOfScan == STL_TRUE )
            {
                KNL_SET_ALL_BLOCK_USED_CNT( sExeGroupInstant->mReadColBlock, 0 );
                *aEOF = STL_TRUE;
                *aUsedBlockCnt = 0;
                STL_THROW( RAMP_EXIT );
            }
            else
            {
                /* Do Nothing */
            }
        }
        else
        {
            sInstantTable->mIterator = NULL;
            KNL_SET_ALL_BLOCK_USED_CNT( sExeGroupInstant->mReadColBlock, 0 );
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


    /*****************************************
     * FETCH
     ****************************************/
    
    /**
     * Aggregation이 없는 fetch 수행
     */
        
    sFetchInfo->mSkipCnt  = aSkipCnt;
    sFetchInfo->mFetchCnt = 1;
        
    while( 1 )
    {
        /* fetch records */
        STL_TRY( smlFetchNext( sInstantTable->mIterator,
                               sFetchInfo,
                               SML_ENV(aEnv) )
                 == STL_SUCCESS );

        *aUsedBlockCnt = sFetchInfo->mRowBlock->mUsedBlockSize;
            
        if( *aUsedBlockCnt > 0 )
        {
            /* move to next group */
            sFetchInfo->mIsEndOfScan = STL_FALSE;
            STL_TRY( smlMoveToNextKey( sInstantTable->mIterator,
                                       sFetchInfo,
                                       SML_ENV(aEnv) )
                     == STL_SUCCESS );
            *aEOF = sFetchInfo->mIsEndOfScan;

            break;
        }
        else
        {
            STL_DASSERT( sFetchInfo->mIsEndOfScan == STL_TRUE );

            /* move to next group */
            sFetchInfo->mIsEndOfScan = STL_FALSE;
            STL_TRY( smlMoveToNextKey( sInstantTable->mIterator,
                                       sFetchInfo,
                                       SML_ENV(aEnv) )
                     == STL_SUCCESS );

            if( sFetchInfo->mIsEndOfScan == STL_TRUE )
            {
                KNL_SET_ALL_BLOCK_USED_CNT( sExeGroupInstant->mReadColBlock, 0 );
                *aEOF = STL_TRUE;
                *aUsedBlockCnt = 0;
                STL_THROW( RAMP_EXIT );
            }
            else
            {
                /* Do Nothing */
            }
        }
    }
    
    
    STL_RAMP( RAMP_EXIT );

    sExeGroupInstant->mCommonInfo.mFetchRowCnt += *aUsedBlockCnt;

    return STL_SUCCESS;

    STL_CATCH( ERR_FREE_ITERATOR )
    {
        sInstantTable->mIterator = NULL;
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief GROUP INSTANT TABLE node 수행을 종료한다.
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 *
 * @remark Execution을 종료하고자 할 때 호출한다.
 */
stlStatus qlnxFinalizeGroupSortInstantAccess( qllOptimizationNode   * aOptNode,
                                              qllDataArea           * aDataArea,
                                              qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode        = NULL;
    qlnoInstant           * sOptGroupInstant = NULL;
    qlnxInstant           * sExeGroupInstant = NULL;
    qlnfAggrExecInfo      * sAggrExecInfo    = NULL;
    stlInt32                sLoop            = 0;

    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_GROUP_SORT_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    
    /**
     * GROUP INSTANT TABLE Optimization Node 획득
     */

    sOptGroupInstant = (qlnoInstant *) aOptNode->mOptNode;


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * 해당 노드를 포함하는 Query Expression Optimization Node 획득
     */

    /* sOptQueryNode = aOptNode->mOptCurQueryNode; */


    /**
     * GROUP INSTANT ACCESS Execution Node 획득
     */

    sExeGroupInstant = (qlnxInstant *) sExecNode->mExecNode;

    
    /**
     * Instant Table 해제
     */

    STL_TRY( qlnxDropInstantTable( & sExeGroupInstant->mInstantTable,
                                   aEnv )
             == STL_SUCCESS );


    /**
     * Instant Table For Aggregation 해제
     */

    if( sExeGroupInstant->mAggrDistInstantTable != NULL )
    {
        STL_TRY( qlnxDropInstantTable( sExeGroupInstant->mAggrDistInstantTable,
                                       aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /* Do Nothing */
    }

    
    /**
     * Aggregation Instant Table 해제
     */

    sAggrExecInfo = sExeGroupInstant->mAggrDistExecInfo;
    
    for( sLoop = 0 ; sLoop < sOptGroupInstant->mAggrFuncCnt ; sLoop++ )
    {
        if( sAggrExecInfo->mInstantTable != NULL )
        {
            STL_TRY( qlnxDropInstantTable( sAggrExecInfo->mInstantTable,
                                           aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /* Do Nothing */
        }

        sAggrExecInfo++;
    }

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief GROUP INSTANT TABLE node의 Aggregation을 Fetch한다.
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
stlStatus qlnxFetchAggrGroupSortInstantAccess( smlTransId              aTransID,
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
    qllExecutionNode      * sExecNode         = NULL;
    qlnoInstant           * sOptGroupInstant  = NULL;
    qlnxInstant           * sExeGroupInstant  = NULL;
    qlnInstantTable       * sInstantTable     = NULL;
    qlnInstantTable       * sAggrInstantTable = NULL;

    stlInt64                sReadCnt          = 0;
    stlBool                 sEOF              = STL_FALSE;
    stlBool                 sIsExistRec       = STL_FALSE;

    knlValueBlockList     * sValueBlock       = NULL;
    knlExprStack          * sExprStack        = NULL;
    smlFetchInfo          * sFetchInfo        = NULL;
    smlFetchInfo          * sAggrFetchInfo    = NULL;
    qlnfAggrOptInfo       * sAggrOptInfo      = NULL;
    qlnfAggrExecInfo      * sAggrExecInfo     = NULL;
    stlInt32                sAggrFuncCnt      = 0;
    stlInt32                sInsertColCnt     = 0;
    stlInt32                sLoop             = 0;
    knlExprEvalInfo         sEvalInfo;

    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_GROUP_SORT_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    
    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * GROUP INSTANT TABLE Optimization Node 획득
     */

    sOptGroupInstant = (qlnoInstant *) aOptNode->mOptNode;

    
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * GROUP INSTANT TABLE Execution Node 획득
     */

    sExeGroupInstant = sExecNode->mExecNode;


    /**
     * Used Block Count 초기화
     */
    
    *aUsedBlockCnt = 0;


    /**
     * Node 수행 여부 확인 
     */

    STL_DASSERT( *aEOF == STL_FALSE );
    STL_DASSERT( sExeGroupInstant->mAggrDistFetchInfo->mIsEndOfScan == STL_FALSE );
    STL_DASSERT( aFetchCnt > 0);


    /**
     * @todo 불필요한 요소를 제거하여 Range 재구성 (확장 필요)
     */

    if( ( ( sOptGroupInstant->mIndexScanInfo->mIsExist == STL_FALSE ) &&
          ( sOptGroupInstant->mIndexScanInfo->mEmptyValue == STL_FALSE ) ) ||
        (  sExeGroupInstant->mHasFalseFilter == STL_TRUE ) )
    {
        sExeGroupInstant->mAggrDistFetchInfo->mIsEndOfScan = STL_TRUE;
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

    /**
     * Internal Fetch Info 설정
     */
        
    sInstantTable = & sExeGroupInstant->mInstantTable;
    sFetchInfo    = & sExeGroupInstant->mFetchInfo;
    
    if( sExeGroupInstant->mAggrDistInstantTable->mIsNeedBuild == STL_TRUE )
    {
        /**
         * Eval Info 설정
         */
        
        sEvalInfo.mContext  = aDataArea->mExprDataContext;
        sEvalInfo.mBlockIdx = 0;

        sExprStack = sOptGroupInstant->mColExprStack;
        sInsertColCnt = ( sOptGroupInstant->mInstantNode->mKeyColCnt +
                          sOptGroupInstant->mInstantNode->mRecColCnt );

        while( sEOF == STL_FALSE )
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
                             sOptGroupInstant->mChildNode->mIdx,
                             aDataArea,
                             0, /* Start Idx */
                             0, /* Skip Count */
                             QLL_FETCH_COUNT_MAX, /* Fetch Count */
                             & sReadCnt,
                             & sEOF,
                             aEnv );

            if( sReadCnt > 0 )
            {
                /**
                 * Evaluate Column Expression Stack
                 */
            
                sIsExistRec = STL_TRUE;
                sEvalInfo.mBlockCount = sReadCnt;

                sValueBlock = sExeGroupInstant->mInsertColBlock;
                for( sLoop = 0 ; sLoop < sInsertColCnt ; sLoop++ )
                {
                    sEvalInfo.mExprStack   = & sExprStack[ sLoop ];
                    sEvalInfo.mResultBlock = sValueBlock;

                    STL_TRY( knlEvaluateBlockExpression( & sEvalInfo,
                                                         KNL_ENV( aEnv ) )
                             == STL_SUCCESS );
                
                    sValueBlock = sValueBlock->mNext;
                }


                /**
                 * Instant Table에 Record 삽입
                 */

                KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sExeGroupInstant->mInsertColBlock, 0, sReadCnt );

                STL_TRY( smlInsertRecord( aStmt,
                                          sInstantTable->mTableHandle,
                                          sExeGroupInstant->mInsertColBlock,
                                          NULL, /* unique violation */
                                          sFetchInfo->mRowBlock,
                                          SML_ENV( aEnv ) )
                         == STL_SUCCESS );

                sInstantTable->mRecCnt += sReadCnt;
            }
            else
            {
                if( sIsExistRec == STL_FALSE )
                {
                    KNL_SET_ALL_BLOCK_USED_CNT( sExeGroupInstant->mAggrDistReadColBlock, 0 );
                    *aEOF = STL_TRUE;
                    *aUsedBlockCnt = 0;
                    STL_THROW( RAMP_EXIT );
                }
                else
                {
                    STL_DASSERT( sEOF == STL_TRUE );
                    break;
                }
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
                           == STL_SUCCESS, ERR_FREE_INST_ITERATOR );
            
            sInstantTable->mIterator = NULL;
        }

        if( sInstantTable->mRecCnt > 0 )
        {
            SML_SET_ITERATOR_PROP( sInstantTable->mIteratorProperty,
                                   0,
                                   STL_UINT64_MAX,
                                   NULL,
                                   STL_TRUE,     /* aGroupFetch */
                                   STL_FALSE );  /* aSkipHitRecord */
            
            STL_TRY( smlAllocIterator( aStmt,
                                       sInstantTable->mTableHandle,
                                       SML_TRM_SNAPSHOT,
                                       SML_SRM_SNAPSHOT,
                                       & sInstantTable->mIteratorProperty,
                                       SML_SCAN_FORWARD,
                                       & sInstantTable->mIterator,
                                       SML_ENV( aEnv ) )
                     == STL_SUCCESS );

            /* move to first group */
            STL_TRY( smlMoveToNextKey( sInstantTable->mIterator,
                                       sFetchInfo,
                                       SML_ENV(aEnv) )
                     == STL_SUCCESS );

            if( sFetchInfo->mIsEndOfScan == STL_TRUE )
            {
                KNL_SET_ALL_BLOCK_USED_CNT( sExeGroupInstant->mAggrDistReadColBlock, 0 );
                *aEOF = STL_TRUE;
                *aUsedBlockCnt = 0;
                STL_THROW( RAMP_EXIT );
            }
            else
            {
                /* Do Nothing */
            }
        }
        else
        {
            sInstantTable->mIterator = NULL;
            KNL_SET_ALL_BLOCK_USED_CNT( sExeGroupInstant->mAggrDistReadColBlock, 0 );
            *aEOF = STL_TRUE;
            *aUsedBlockCnt = 0;
            STL_THROW( RAMP_EXIT );
        }
    }
    else
    {
        if( sInstantTable->mRecCnt == 0 )
        {
            KNL_SET_ALL_BLOCK_USED_CNT( sExeGroupInstant->mAggrDistReadColBlock, 0 );
            *aEOF = STL_TRUE;
            *aUsedBlockCnt = 0;
            STL_THROW( RAMP_EXIT );
        }
        else
        {
            STL_PARAM_VALIDATE( sInstantTable->mIterator != NULL,
                                QLL_ERROR_STACK(aEnv) );
            STL_THROW( RAMP_FETCH );
        }
    }


    /*****************************************
     * Aggregation 수행
     ****************************************/

    sInstantTable     = & sExeGroupInstant->mInstantTable;
    sFetchInfo        = & sExeGroupInstant->mFetchInfo;

    sAggrInstantTable = sExeGroupInstant->mAggrDistInstantTable;
    sAggrFetchInfo    = sExeGroupInstant->mAggrDistFetchInfo;


    /**
     * Fetch Count 만큼 Group 단위 Aggregation 수행 
     */

    while( aFetchCnt > 0 )
    {
        /**
         * Build Aggregation
         */

        sFetchInfo->mIsEndOfScan = STL_FALSE;
        sAggrFuncCnt = sOptGroupInstant->mAggrFuncCnt;

        
        /**
         * Aggregation Init 수행
         */
        
        sAggrOptInfo  = sOptGroupInstant->mAggrOptInfo;
        sAggrExecInfo = sExeGroupInstant->mAggrDistExecInfo;
            
        for( sLoop = 0 ; sLoop < sAggrFuncCnt ; sLoop++ )
        {
            STL_TRY( gQlnfAggrFuncInfoArr[sAggrOptInfo->mAggrFuncId].mInitFuncPtr(
                         sAggrExecInfo,
                         0, /* result block idx */
                         aEnv )
                     == STL_SUCCESS );
            sAggrOptInfo++;
            sAggrExecInfo++;
        }

        while( sFetchInfo->mIsEndOfScan == STL_FALSE )
        {
            /* fetch records */
            STL_TRY( smlFetchNext( sInstantTable->mIterator,
                                   sFetchInfo,
                                   SML_ENV(aEnv) )
                     == STL_SUCCESS );

            *aUsedBlockCnt = sFetchInfo->mRowBlock->mUsedBlockSize;
            
            if( *aUsedBlockCnt > 0 )
            {
                sAggrOptInfo  = sOptGroupInstant->mAggrOptInfo;
                sAggrExecInfo = sExeGroupInstant->mAggrDistExecInfo;
                
                for( sLoop = 0 ; sLoop < sAggrFuncCnt ; sLoop++ )
                {
                    if( sAggrExecInfo->mIsDistinct == STL_TRUE )
                    {
                        if( sAggrExecInfo->mIsConstDistinct == STL_TRUE )
                        {
                            if( sAggrExecInfo->mIsFirstInsert == STL_TRUE )
                            {
                                STL_TRY( gQlnfAggrFuncInfoArr[sAggrOptInfo->mAggrFuncId].mBuildFuncPtr(
                                             sAggrExecInfo,
                                             0, /* start block idx */
                                             1,
                                             0, /* result block idx */
                                             aEnv )
                                         == STL_SUCCESS );
                        
                                sAggrExecInfo->mIsFirstInsert = STL_FALSE;
                                
                            }
                            else
                            {
                                /* Do Nothing */
                            }
                        }
                        else
                        {
                            STL_TRY( gQlnfAggrFuncInfoArr[sAggrOptInfo->mAggrFuncId].mBuildUniqueFuncPtr(
                                         sAggrExecInfo,
                                         *aUsedBlockCnt,
                                         aEnv )
                                     == STL_SUCCESS );
                        }
                    }
                    else
                    {
                        STL_TRY( gQlnfAggrFuncInfoArr[sAggrOptInfo->mAggrFuncId].mBuildFuncPtr(
                                     sAggrExecInfo,
                                     0, /* start block idx */
                                     *aUsedBlockCnt,
                                     0, /* result block idx */
                                     aEnv )
                                 == STL_SUCCESS );
                    }
            
                    sAggrOptInfo++;
                    sAggrExecInfo++;
                }
            }
            else
            {
                break;
            }
        }

        
        /**
         * Aggregation Fetch 수행
         */

        sAggrOptInfo  = sOptGroupInstant->mAggrOptInfo;
        sAggrExecInfo = sExeGroupInstant->mAggrDistExecInfo;

        for( sLoop = 0 ; sLoop < sAggrFuncCnt ; sLoop++ )
        {
            STL_TRY( gQlnfAggrFuncInfoArr[sAggrOptInfo->mAggrFuncId].mFetchFuncPtr(
                         sAggrExecInfo,
                         aEnv )
                     == STL_SUCCESS );
        
            STL_DASSERT( KNL_GET_BLOCK_USED_CNT( sAggrExecInfo->mResultValue ) == 1 );
        
            sAggrOptInfo++;
            sAggrExecInfo++;
        }

        
        /**
         * Aggregation Fetch 수행 결과를 Instant Table에 삽입
         */

        KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sExeGroupInstant->mAggrDistInsertColBlock, 0, 1 );
        
        STL_TRY( smlInsertRecord( aStmt,
                                  sAggrInstantTable->mTableHandle,
                                  sExeGroupInstant->mAggrDistInsertColBlock,
                                  NULL, /* unique violation */
                                  sAggrFetchInfo->mRowBlock,
                                  SML_ENV( aEnv ) )
                 == STL_SUCCESS );

        sAggrInstantTable->mRecCnt++;

        /* move to first group */
        sFetchInfo->mIsEndOfScan = STL_FALSE;
        STL_TRY( smlMoveToNextKey( sInstantTable->mIterator,
                                   sFetchInfo,
                                   SML_ENV(aEnv) )
                 == STL_SUCCESS );

        if( sFetchInfo->mIsEndOfScan == STL_TRUE )
        {
            break;
        }
        else
        {
            /* Do Nothing */
        }
    }

    STL_DASSERT( sAggrInstantTable->mRecCnt > 0 );

    
    /**
     * Iterator Allocation
     */

    SML_INIT_ITERATOR_PROP( sAggrInstantTable->mIteratorProperty );
            
    if( sAggrInstantTable->mIterator != NULL )
    {
        STL_TRY_THROW( smlFreeIterator( sAggrInstantTable.mIterator,
                                        SML_ENV( aEnv ) )
                       == STL_SUCCESS, ERR_FREE_AGGR_ITERATOR );
            
        sAggrInstantTable.mIterator = NULL;
    }
    
    STL_TRY( smlAllocIterator( aStmt,
                               sAggrInstantTable->mTableHandle,
                               SML_TRM_SNAPSHOT,
                               SML_SRM_SNAPSHOT,
                               & sAggrInstantTable->mIteratorProperty,
                               SML_SCAN_FORWARD,
                               & sAggrInstantTable->mIterator,
                               SML_ENV( aEnv ) )
             == STL_SUCCESS );


    /**
     * DROP GROUP INSTANT TABLE
     */
    
    sAggrInstantTable->mIsNeedBuild = STL_FALSE;

        
    /*****************************************
     * Fetch
     ****************************************/

    STL_RAMP( RAMP_FETCH );
    
    /**
     * Instant Table에 대한 Read Records
     */

    sAggrInstantTable = sExeGroupInstant->mAggrDistInstantTable;
    sAggrFetchInfo    = sExeGroupInstant->mAggrDistFetchInfo;
    
    if( sAggrInstantTable->mRecCnt <= sAggrInstantTable->mCurIdx + aSkipCnt )
    {
        KNL_SET_ALL_BLOCK_USED_CNT( sExeGroupInstant->mAggrDistReadColBlock, 0 );
        *aEOF = STL_TRUE;
        *aUsedBlockCnt = 0;
    }
    else
    {
        /**
         * Skip Count 와 Fetch Count 조정
         */
        
        sAggrFetchInfo->mSkipCnt  = aSkipCnt;
        sAggrFetchInfo->mFetchCnt = aFetchCnt;
        
        /* fetch records */
        STL_TRY( smlFetchNext( sAggrInstantTable->mIterator,
                               sAggrFetchInfo,
                               SML_ENV(aEnv) )
                 == STL_SUCCESS );
        
        *aEOF = sAggrFetchInfo->mIsEndOfScan;

        *aUsedBlockCnt = sAggrFetchInfo->mRowBlock->mUsedBlockSize;

        sAggrInstantTable->mCurIdx += *aUsedBlockCnt;
    }

    
    STL_RAMP( RAMP_EXIT );


    return STL_SUCCESS;

    STL_CATCH( ERR_FREE_INST_ITERATOR )
    {
        sInstantTable->mIterator = NULL;
    }

    STL_CATCH( ERR_FREE_AGGR_ITERATOR )
    {
        sAggrInstantTable.mIterator = NULL;
    }
    
    STL_FINISH;

    return STL_FAILURE;
}
#endif



/* /\** */
/*  * @brief GROUP INSTANT TABLE node를 Fetch한다. (pipeline) */
/*  * @param[in]      aTransID      Transaction ID */
/*  * @param[in]      aStmt         Statement */
/*  * @param[in]      aDBCStmt      DBC Statement */
/*  * @param[in,out]  aSQLStmt      SQL Statement */
/*  * @param[in]      aOptNode      Optimization Node */
/*  * @param[in]      aDataArea     Data Area (Data Optimization 결과물) */
/*  * @param[in]      aSkipCnt      Skip Record Count */
/*  * @param[in]      aFetchCnt     Fetch Record Count */
/*  * @param[out]     aUsedBlockCnt Read Value Block의 Data가 저장된 공간의 개수 */
/*  * @param[out]     aEOF          End Of Fetch 여부 */
/*  * @param[in]      aEnv          Environment */
/*  * */
/*  * @remark Fetch 수행시 호출된다. */
/*  *\/ */
/* stlStatus qlnxFetchGroupSortInstantAccessPipeline( smlTransId              aTransID, */
/*                                                    smlStatement          * aStmt, */
/*                                                    qllDBCStmt            * aDBCStmt, */
/*                                                    qllStatement          * aSQLStmt, */
/*                                                    qllOptimizationNode   * aOptNode, */
/*                                                    qllDataArea           * aDataArea, */
/*                                                    stlInt64                aSkipCnt, */
/*                                                    stlInt64                aFetchCnt, */
/*                                                    stlInt64              * aUsedBlockCnt, */
/*                                                    stlBool               * aEOF, */
/*                                                    qllEnv                * aEnv ) */
/* { */
/*     qllExecutionNode      * sExecNode        = NULL; */
/*     qlnoInstant           * sOptGroupInstant = NULL; */
/*     qlnxInstant           * sExeGroupInstant = NULL; */
/*     qlnInstantTable       * sInstantTable    = NULL; */

/*     stlInt64                sReadCnt         = 0; */
/*     stlBool                 sEOF             = STL_FALSE; */
    
/*     knlValueBlockList     * sValueBlock      = NULL; */
/*     knlExprStack          * sExprStack       = NULL; */
/*     smlFetchInfo          * sFetchInfo       = NULL; */
/*     stlInt32                sInsertColCnt    = 0; */
/*     stlInt32                sLoop            = 0; */
/*     stlInt64                sUsedBlockCnt    = 0; */
/*     knlExprEvalInfo         sEvalInfo; */

/*     dtlDataValue          * sConflictValue   = NULL; */
/*     dtlDataValue          * sDataValue       = NULL; */

/*     /\* */
/*      * Parameter Validation */
/*      *\/  */

/*     STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) ); */
/*     STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) ); */
/*     STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) ); */
/*     STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) ); */
/*     STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_GROUP_SORT_INSTANT_ACCESS_TYPE, */
/*                         QLL_ERROR_STACK(aEnv) ); */

    
/*     /\***************************************** */
/*      * 기본 정보 획득 */
/*      ****************************************\/ */
    
/*     /\** */
/*      * GROUP INSTANT TABLE Optimization Node 획득 */
/*      *\/ */

/*     sOptGroupInstant = (qlnoInstant *) aOptNode->mOptNode; */

    
/*     /\** */
/*      * Common Execution Node 획득 */
/*      *\/ */
        
/*     sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) ); */


/*     /\** */
/*      * GROUP INSTANT TABLE Execution Node 획득 */
/*      *\/ */

/*     sExeGroupInstant = sExecNode->mExecNode; */


/*     /\** */
/*      * Used Block Count 초기화 */
/*      *\/ */
    
/*     *aUsedBlockCnt = 0; */


/*     /\** */
/*      * Node 수행 여부 확인  */
/*      *\/ */

/*     STL_DASSERT( *aEOF == STL_FALSE ); */
/*     STL_DASSERT( sExeGroupInstant->mFetchInfo.mIsEndOfScan == STL_FALSE ); */
/*     STL_DASSERT( aFetchCnt > 0); */

        
/*     /\** */
/*      * @todo 불필요한 요소를 제거하여 Range 재구성 (확장 필요) */
/*      *\/ */

/*     if( ( ( sOptGroupInstant->mIndexScanInfo->mIsExist == STL_FALSE ) && */
/*           ( sOptGroupInstant->mIndexScanInfo->mEmptyValue == STL_FALSE ) ) || */
/*         (  sExeGroupInstant->mHasFalseFilter == STL_TRUE ) ) */
/*     { */
/*         sExeGroupInstant->mFetchInfo.mIsEndOfScan = STL_TRUE; */
/*         *aEOF = STL_TRUE; */
/*         STL_THROW( RAMP_EXIT ); */
/*     } */
/*     else */
/*     { */
/*         /\* Do Nothing *\/ */
/*     } */

    
/*     /\***************************************** */
/*      * Fetch 수행하면서 Instant Table에 레코드 삽입 */
/*      ****************************************\/ */

/*     sInstantTable = & sExeGroupInstant->mInstantTable; */
/*     sFetchInfo    = & sExeGroupInstant->mFetchInfo; */

/*     sEvalInfo.mContext  = aDataArea->mExprDataContext; */
/*     sEvalInfo.mBlockIdx = 0; */

/*     sExprStack    = sOptGroupInstant->mColExprStack; */
/*     sInsertColCnt = sOptGroupInstant->mInstantNode->mKeyColCnt; */

/*     while( sEOF == STL_FALSE ) */
/*     { */
/*         /\** */
/*          * Block Read */
/*          *\/ */

/*         /\* fetch records*\/ */
/*         sReadCnt = 0; */
/*         QLNX_FETCH_NODE( aTransID, */
/*                          aStmt, */
/*                          aDBCStmt, */
/*                          aSQLStmt, */
/*                          sOptGroupInstant->mChildNode->mIdx, */
/*                          aDataArea, */
/*                          0, /\* Skip Count *\/ */
/*                          QLL_FETCH_COUNT_MAX, /\* Fetch Count *\/ */
/*                          & sReadCnt, */
/*                          & sEOF, */
/*                          aEnv ); */

/*         if( sReadCnt > 0 ) */
/*         { */
/*             /\** */
/*              * Evaluate Column Expression Stack */
/*              *\/ */
            
/*             sInstantTable->mRecCnt += sReadCnt; */
                
/*             sEvalInfo.mBlockCount = sReadCnt; */

/*             sValueBlock = sExeGroupInstant->mKeyColBlock; */
/*             for( sLoop = 0 ; sLoop < sInsertColCnt ; sLoop++ ) */
/*             { */
/*                 sEvalInfo.mExprStack   = & sExprStack[ sLoop ]; */
/*                 sEvalInfo.mResultBlock = sValueBlock; */

/*                 STL_TRY( knlEvaluateBlockExpression( & sEvalInfo, */
/*                                                      KNL_ENV( aEnv ) ) */
/*                          == STL_SUCCESS ); */
                
/*                 sValueBlock = sValueBlock->mNext; */
/*             } */
/*         } */
/*         else */
/*         { */
/*             KNL_SET_ALL_BLOCK_USED_CNT( sExeGroupInstant->mReadColBlock, 0 ); */
/*             *aEOF = STL_TRUE; */
/*             *aUsedBlockCnt = 0; */
/*             STL_THROW( RAMP_EXIT ); */
/*         } */

        
/*         /\***************************************** */
/*          * FETCH */
/*          ****************************************\/ */
    
/*         /\** */
/*          * Uniqueness 체크 */
/*          *\/ */

/*         KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sExeGroupInstant->mKeyColBlock, 0, sReadCnt ); */

/*         STL_TRY( smlInsertRecord( aStmt, */
/*                                   sInstantTable->mTableHandle, */
/*                                   sExeGroupInstant->mKeyColBlock, */
/*                                   aDataArea->mPredResultBlock, /\* unique violation *\/ */
/*                                   sFetchInfo->mRowBlock, */
/*                                   SML_ENV( aEnv ) ) */
/*                  == STL_SUCCESS ); */

            
/*         /\** */
/*          * Value Block 재정렬 */
/*          *\/ */

/*         sUsedBlockCnt  = 0; */
/*         sConflictValue = aDataArea->mPredResultBlock->mValueBlock->mDataValue; */

/*         for( sLoop = 0 ; ( sLoop < sReadCnt ) && ( aFetchCnt > 0 ) ; sLoop++ ) */
/*         { */
/*             STL_DASSERT( sConflictValue[ sLoop ].mLength == DTL_BOOLEAN_SIZE ); */
            
/*             if( DTL_BOOLEAN_IS_FALSE( & sConflictValue[ sLoop ] ) ) */
/*             { */
/*                 if( aSkipCnt > 0 ) */
/*                 { */
/*                     aSkipCnt--; */
/*                     continue; */
/*                 } */
/*                 else */
/*                 { */
/*                     /\* Do Nothing *\/ */
/*                 } */
                    
/*                 if( sLoop == sUsedBlockCnt ) */
/*                 { */
/*                     /\* Do Nothing *\/ */
/*                 } */
/*                 else */
/*                 { */
/*                     sValueBlock = sExeGroupInstant->mReadColBlock; */
                    
/*                     while( sValueBlock != NULL ) */
/*                     { */
/*                         if( DTL_GET_BLOCK_IS_SEP_BUFF( sValueBlock ) == STL_TRUE ) */
/*                         { */
/*                             sDataValue = KNL_GET_BLOCK_SEP_DATA_VALUE( sValueBlock, sLoop ); */
                        
/*                             DTL_COPY_DATA_VALUE( sDataValue->mLength, */
/*                                                  sDataValue, */
/*                                                  KNL_GET_BLOCK_SEP_DATA_VALUE( sValueBlock, sUsedBlockCnt ) ); */
/*                         } */
/*                         else */
/*                         { */
/*                             /\* Do Nothing *\/ */
/*                         } */
                        
/*                         sValueBlock = sValueBlock->mNext; */
/*                     } */
/*                 } */

/*                 aFetchCnt--; */
/*                 sUsedBlockCnt++; */
/*             } */
/*             else */
/*             { */
/*                 /\* Do Nothing *\/ */
/*             } */
/*         } */


/*         if( sUsedBlockCnt > 0 ) */
/*         { */
/*             KNL_SET_ALL_BLOCK_USED_CNT( sExeGroupInstant->mReadColBlock, sUsedBlockCnt ); */
/*             *aEOF = sEOF; */
/*             *aUsedBlockCnt = sUsedBlockCnt; */
/*             break; */
/*         } */
/*         else */
/*         { */
/*             /\* Do Nothing *\/ */
/*         } */
/*     } */
    

/*     STL_RAMP( RAMP_EXIT ); */


/*     return STL_SUCCESS; */

/*     STL_FINISH; */

/*     return STL_FAILURE; */
/* } */


/** @} qlnx */
