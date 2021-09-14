/*******************************************************************************
 * qlndGroupHashInstantAccess.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlndGroupHashInstantAccess.c 10973 2014-01-16 07:17:19Z bsyou $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlndGroupHashInstantAccess.c
 * @brief SQL Processor Layer Data Optimization Node - GROUP INSTANT ACCESS
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnd
 * @{
 */


/**
 * @brief GROUP INSTANT ACCESS Execution node에 대한 Data 정보를 구축한다.
 * @param[in]      aTransID      Transaction ID
 * @param[in]      aDBCStmt      DBC Statement
 * @param[in,out]  aSQLStmt      SQL Statement
 * @param[in]      aOptNode      Optimization Node
 * @param[in,out]  aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 *
 * @remark Data Optimization 단계에서 수행한다.
 *    <BR> Execution Node를 생성하고, Execution을 위한 정보를 구축한다.
 *    <BR> 공간 할당시 Data Plan 메모리 관리자를 이용한다.
 *    <BR> 상위 Query Node를 가지지 않는다.
 */
stlStatus qlndDataOptimizeGroupHashInstantAccess( smlTransId              aTransID,
                                                  qllDBCStmt            * aDBCStmt,
                                                  qllStatement          * aSQLStmt,
                                                  qllOptimizationNode   * aOptNode,
                                                  qllDataArea           * aDataArea,
                                                  qllEnv                * aEnv )
{
    qllExecutionNode  * sExecNode        = NULL;
    qlnoInstant       * sOptGroupInstant = NULL;
    qlnxInstant       * sExeGroupInstant = NULL;
    smlFetchInfo      * sFetchInfo       = NULL;

    knlValueBlockList * sNewBlock        = NULL;
    knlValueBlockList * sCurBlock        = NULL;
    knlValueBlockList * sLastTableBlock  = NULL;
    qlvRefExprItem    * sRefColumnItem   = NULL;
    stlInt32            sLoop            = 0;

    qlnfAggrOptInfo   * sTempAggrOptInfo = NULL;
    qlnfAggrOptInfo   * sAggrOptInfo     = NULL;
    qlnfAggrOptInfo   * sAggrDistOptInfo = NULL;
    
    
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_GROUP_HASH_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * GROUP INSTANT ACCESS Optimization Node 획득
     */

    sOptGroupInstant = (qlnoInstant *) aOptNode->mOptNode;


    /**
     * 하위 node에 대한 Data Optimize 수행
     */

    QLND_DATA_OPTIMIZE( aTransID,
                        aDBCStmt,
                        aSQLStmt,
                        sOptGroupInstant->mChildNode,
                        aDataArea,
                        aEnv );


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea,
                                        QLL_GET_OPT_NODE_IDX( aOptNode ) );

    
    /**
     * GROUP INSTANT ACCESS Execution Node 할당
     */

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlnxInstant ),
                                (void **) & sExeGroupInstant,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sExeGroupInstant, 0x00, STL_SIZEOF( qlnxInstant ) );


    /**
     * GROUP INSTANT TABLE Iterator 에 대한 Fetch Info 설정
     */

    sFetchInfo = & sExeGroupInstant->mFetchInfo;


    /**
     * Read Row Block 구성
     */

    /* Grouping이 포함된 경우 RowId를 구성할 필요가 없다. */
    /* STL_DASSERT( ((qlnxCommonInfo*)(aDataArea->mExecNodeList[ sOptGroupInstant->mChildNode->mIdx ].mExecNode))->mRowBlockList == */
    /*              NULL ); */


    /**
     * Iterator를 위한 Group Key Value Block List 구성
     */

    STL_DASSERT( sOptGroupInstant->mInstantNode->mKeyColList != NULL );
    
    STL_DASSERT(  sOptGroupInstant->mInstantNode->mKeyColList->mCount > 0 );

    STL_TRY( qlndBuildTableColBlockList( aDataArea,
                                         sOptGroupInstant->mInstantNode->mKeyColList,
                                         STL_TRUE,
                                         & sExeGroupInstant->mKeyColBlock,
                                         & QLL_DATA_PLAN( aDBCStmt ),
                                         aEnv )
             == STL_SUCCESS );


    /**
     * Iterator를 위한 Record Value Block List 구성
     */

    STL_DASSERT( sOptGroupInstant->mInstantNode->mRecColList != NULL );
    
    if( sOptGroupInstant->mInstantNode->mRecColList->mCount > 0 )
    {
        STL_TRY( qlndBuildTableColBlockList( aDataArea,
                                             sOptGroupInstant->mInstantNode->mRecColList,
                                             STL_TRUE,
                                             & sExeGroupInstant->mRecColBlock,
                                             & QLL_DATA_PLAN( aDBCStmt ),
                                             aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sExeGroupInstant->mRecColBlock = NULL;
    }


    /**
     * Iterator를 위한 Read Value Block List 구성
     */

    STL_DASSERT( sOptGroupInstant->mInstantNode->mReadColList != NULL );
    
    if( sOptGroupInstant->mInstantNode->mReadColList->mCount > 0 )
    {
        STL_TRY( qlndBuildTableColBlockList( aDataArea,
                                             sOptGroupInstant->mInstantNode->mReadColList,
                                             STL_TRUE,
                                             & sExeGroupInstant->mReadColBlock,
                                             & QLL_DATA_PLAN( aDBCStmt ),
                                             aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sExeGroupInstant->mReadColBlock = NULL;
    }
    sFetchInfo->mColList = sExeGroupInstant->mReadColBlock;

    
    /**
     * Index Read Column Block 설정
     */

    sExeGroupInstant->mIndexReadColBlock = NULL;
    

    /**
     * Table Read Column Block 설정
     */

    sExeGroupInstant->mTableReadColBlock = sExeGroupInstant->mReadColBlock;

    
    /**
     * Insert Value Block List 설정
     */

    sLastTableBlock = NULL;

    sCurBlock = sExeGroupInstant->mKeyColBlock;
    while( sCurBlock != NULL )
    {
        STL_TRY( knlInitShareBlock( & sNewBlock,
                                    sCurBlock,
                                    & QLL_DATA_PLAN( aDBCStmt ),
                                    KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
        
        if( sLastTableBlock == NULL )
        {
            sExeGroupInstant->mInsertColBlock = sNewBlock;
        }
        else
        {
            KNL_LINK_BLOCK_LIST( sLastTableBlock, sNewBlock );
        }
        sLastTableBlock = sNewBlock;

        sCurBlock = sCurBlock->mNext;
    }

    STL_DASSERT( sLastTableBlock != NULL );

    sCurBlock = sExeGroupInstant->mRecColBlock;
    while( sCurBlock != NULL )
    {
        STL_TRY( knlInitShareBlock( & sNewBlock,
                                    sCurBlock,
                                    & QLL_DATA_PLAN( aDBCStmt ),
                                    KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
        
        KNL_LINK_BLOCK_LIST( sLastTableBlock, sNewBlock );
        sLastTableBlock = sNewBlock;

        sCurBlock = sCurBlock->mNext;
    }


    /**
     * Physical Filter 구성
     */
    
    if( sOptGroupInstant->mTablePhysicalFilterExpr == NULL )
    {
        sFetchInfo->mPhysicalFilter = NULL;
    }
    else
    {
        STL_TRY( qlndMakePhysicalFilter( (qlvInitNode *) sOptGroupInstant->mInstantNode,
                                         sOptGroupInstant->mTablePhysicalFilterExpr,
                                         sFetchInfo->mColList,
                                         aDataArea->mExprDataContext,
                                         & QLL_DATA_PLAN( aDBCStmt ),
                                         & sFetchInfo->mPhysicalFilter,
                                         aEnv )
                 == STL_SUCCESS );
    }
    

    /**
     * Logical Filter 구성
     */

    if( sOptGroupInstant->mPredicate == NULL )
    {
        sFetchInfo->mLogicalFilter  = NULL;
        sFetchInfo->mFilterEvalInfo = NULL;
    }
    else
    {
        sFetchInfo->mLogicalFilter   = sOptGroupInstant->mTableLogicalStack;

        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                    STL_SIZEOF( knlExprEvalInfo ),
                                    (void **) & sFetchInfo->mFilterEvalInfo,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        sFetchInfo->mFilterEvalInfo->mExprStack   = sFetchInfo->mLogicalFilter;
        sFetchInfo->mFilterEvalInfo->mContext     = aDataArea->mExprDataContext;
        sFetchInfo->mFilterEvalInfo->mResultBlock = aDataArea->mPredResultBlock;
        sFetchInfo->mFilterEvalInfo->mBlockIdx    = 0;
        sFetchInfo->mFilterEvalInfo->mBlockCount  = 0;
    }


    /**
     * Index 관련 정보 초기화
     */

    sFetchInfo->mRange           = NULL;
    sFetchInfo->mKeyCompList     = NULL;
    sFetchInfo->mFetchRecordInfo = NULL;
    sFetchInfo->mAggrFetchInfo   = NULL;

    
    /**
     * Row Block 공간 할당
     */
    
    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( smlRowBlock ),
                                (void **) & sFetchInfo->mRowBlock,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY( smlInitRowBlock( sFetchInfo->mRowBlock,
                              aDataArea->mBlockAllocCnt,
                              & QLL_DATA_PLAN( aDBCStmt ),
                              SML_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Fetch 정보 초기화
     */

    sFetchInfo->mSkipCnt         = 0;
    sFetchInfo->mFetchCnt        = QLL_FETCH_COUNT_MAX;
    sFetchInfo->mIsEndOfScan     = STL_FALSE;


    /**
     * Rebuild 정보 설정
     */

    sExeGroupInstant->mNeedRebuild = sOptGroupInstant->mNeedRebuild;


    /**
     * Prepare Group Key 정보 설정
     */

    STL_TRY( qlndMakePrepareStackEvalInfo( aDataArea->mExprDataContext,
                                           sOptGroupInstant->mPrepareRangeStack,
                                           & sExeGroupInstant->mPreRangeEvalInfo,
                                           & QLL_DATA_PLAN( aDBCStmt ),
                                           aEnv )
             == STL_SUCCESS );


    /**
     * Prepare Key Filter 정보 설정
     */

    STL_TRY( qlndMakePrepareStackEvalInfo( aDataArea->mExprDataContext,
                                           sOptGroupInstant->mPrepareKeyFilterStack,
                                           & sExeGroupInstant->mPreKeyFilterEvalInfo,
                                           & QLL_DATA_PLAN( aDBCStmt ),
                                           aEnv )
             == STL_SUCCESS );


    /**
     * Prepare Filter 정보 설정
     */

    STL_TRY( qlndMakePrepareStackEvalInfo( aDataArea->mExprDataContext,
                                           sOptGroupInstant->mPrepareFilterStack,
                                           & sExeGroupInstant->mPreFilterEvalInfo,
                                           & QLL_DATA_PLAN( aDBCStmt ),
                                           aEnv )
             == STL_SUCCESS );


    /**
     * Common Info 설정
     */

    sExeGroupInstant->mCommonInfo.mResultColBlock = sExeGroupInstant->mReadColBlock;
    sExeGroupInstant->mCommonInfo.mRowBlockList   = NULL;


    /**
     * Outer Column Value Block List 설정
     */

    STL_TRY( qlndBuildOuterColBlockList( aDataArea,
                                         sOptGroupInstant->mOuterColumnList,
                                         & sExeGroupInstant->mCommonInfo.mOuterColBlock,
                                         & sExeGroupInstant->mCommonInfo.mOuterOrgColBlock,
                                         & QLL_DATA_PLAN( aDBCStmt ),
                                         aEnv )
             == STL_SUCCESS );


    /***************************************************************************
     * Aggregation 정보 구성
     **************************************************************************/

    sExeGroupInstant->mHasAggr        = ( sOptGroupInstant->mAggrFuncCnt > 0 );
    sExeGroupInstant->mHasNestedAggr  = ( sOptGroupInstant->mNestedAggrFuncCnt > 0 );
    sExeGroupInstant->mHasFalseFilter = STL_FALSE;

    if( ( sOptGroupInstant->mAggrFuncCnt == 0 ) &&
        ( sOptGroupInstant->mScrollable == ELL_CURSOR_SCROLLABILITY_NO_SCROLL ) && 
        ( sOptGroupInstant->mTotalFilterExpr == NULL ) )
    {
        /**
         * @todo Top-Down 방식 확장
         */
        
        /* sExeGroupInstant->mNeedMaterialize = STL_FALSE; */
        sExeGroupInstant->mNeedMaterialize = STL_TRUE;
    }
    else
    {
        sExeGroupInstant->mNeedMaterialize = STL_TRUE;
    }
    

    /**
     * Aggregation Functions Execution 정보 설정 : Merge Record Info
     */
    
    if( sOptGroupInstant->mAggrFuncCnt > 0 )
    {
        if( sOptGroupInstant->mAggrDistFuncCnt > 0 )
        {
            /**
             * @todo 통합 Aggregation Optimization Info 구성
             */

            /* sOptGroupInstant->mAggrOptInfo */
            STL_DASSERT( sOptGroupInstant->mTotalAggrFuncList->mCount ==
                         ( sOptGroupInstant->mAggrFuncList->mCount +
                           sOptGroupInstant->mAggrDistFuncList->mCount ) );
            
            STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                        STL_SIZEOF( qlnfAggrOptInfo ) *
                                        sOptGroupInstant->mTotalAggrFuncList->mCount,
                                        (void **) & sTempAggrOptInfo,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            sRefColumnItem   = sOptGroupInstant->mTotalAggrFuncList->mHead;
            sAggrOptInfo     = sOptGroupInstant->mAggrOptInfo;
            sAggrDistOptInfo = sOptGroupInstant->mAggrDistOptInfo;

            for( sLoop = 0 ; sLoop < sOptGroupInstant->mTotalAggrFuncList->mCount ; sLoop++ )
            {
                STL_DASSERT( sRefColumnItem->mExprPtr->mType == QLV_EXPR_TYPE_AGGREGATION );
                
                if( sRefColumnItem->mExprPtr->mExpr.mAggregation->mIsDistinct == STL_FALSE )
                {
                    STL_DASSERT( sRefColumnItem->mExprPtr == sAggrOptInfo->mAggrExpr );
                    STL_DASSERT( sAggrOptInfo <
                                 & sOptGroupInstant->mAggrOptInfo[ sOptGroupInstant->mAggrFuncList->mCount ] );
                    
                    sTempAggrOptInfo[ sLoop ] = *sAggrOptInfo;
                    sAggrOptInfo++;
                }
                else
                {
                    STL_DASSERT( sRefColumnItem->mExprPtr == sAggrDistOptInfo->mAggrExpr );
                    STL_DASSERT( sAggrDistOptInfo <
                                 & sOptGroupInstant->mAggrDistOptInfo[ sOptGroupInstant->mAggrDistFuncList->mCount ] );

                    sTempAggrOptInfo[ sLoop ] = *sAggrDistOptInfo;
                    sAggrDistOptInfo++;
                }

                sRefColumnItem = sRefColumnItem->mNext;
            }
            STL_DASSERT( sRefColumnItem == NULL );
            
            /* non-distinct + distinct aggregation */
            STL_TRY( qlndSetMergeRecordInfo( aSQLStmt,
                                             sTempAggrOptInfo,
                                             sOptGroupInstant->mTotalAggrFuncList,
                                             sOptGroupInstant->mInstantNode,
                                             & sExeGroupInstant->mMergeRecord,
                                             aDataArea,
                                             & QLL_DATA_PLAN( aDBCStmt ),
                                             aEnv )
                     == STL_SUCCESS );

            /* conflict check info */
            STL_TRY( qlndSetConflictCheckInfo( aSQLStmt,
                                               sOptGroupInstant->mAggrDistFuncCnt,
                                               sOptGroupInstant->mTotalAggrFuncList->mCount,
                                               sTempAggrOptInfo,
                                               sOptGroupInstant->mInstantNode->mKeyColCnt,
                                               sExeGroupInstant->mKeyColBlock,
                                               sExeGroupInstant->mMergeRecord,
                                               aDataArea,
                                               & sExeGroupInstant->mConflictCheckInfo,
                                               & QLL_DATA_PLAN( aDBCStmt ),
                                               aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /* non-distinct */
            STL_TRY( qlndSetMergeRecordInfo( aSQLStmt,
                                             sOptGroupInstant->mAggrOptInfo,
                                             sOptGroupInstant->mAggrFuncList,
                                             sOptGroupInstant->mInstantNode,
                                             & sExeGroupInstant->mMergeRecord,
                                             aDataArea,
                                             & QLL_DATA_PLAN( aDBCStmt ),
                                             aEnv )
                     == STL_SUCCESS );

            sExeGroupInstant->mConflictCheckInfo = NULL;
        }
    }
    else
    {
        if( sOptGroupInstant->mAggrDistFuncCnt > 0 )
        {
            /* distinct */
            STL_TRY( qlndSetMergeRecordInfo( aSQLStmt,
                                             sOptGroupInstant->mAggrDistOptInfo,
                                             sOptGroupInstant->mAggrDistFuncList,
                                             sOptGroupInstant->mInstantNode,
                                             & sExeGroupInstant->mMergeRecord,
                                             aDataArea,
                                             & QLL_DATA_PLAN( aDBCStmt ),
                                             aEnv )
                     == STL_SUCCESS );

            /* conflict check info */
            STL_TRY( qlndSetConflictCheckInfo( aSQLStmt,
                                               sOptGroupInstant->mAggrDistFuncCnt,
                                               sOptGroupInstant->mAggrDistFuncCnt,
                                               sOptGroupInstant->mAggrDistOptInfo,
                                               sOptGroupInstant->mInstantNode->mKeyColCnt,
                                               sExeGroupInstant->mKeyColBlock,
                                               sExeGroupInstant->mMergeRecord,
                                               aDataArea,
                                               & sExeGroupInstant->mConflictCheckInfo,
                                               & QLL_DATA_PLAN( aDBCStmt ),
                                               aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            sExeGroupInstant->mMergeRecord = NULL;
            sExeGroupInstant->mConflictCheckInfo = NULL;
        }
    }
    

    /**
     * Nested Aggregation Functions Execution 정보 설정
     */
    
    if( sOptGroupInstant->mNestedAggrFuncCnt > 0 )
    {
        if( sOptGroupInstant->mNestedAggrDistFuncCnt > 0 )
        {
            /* fetch & aggr */
            STL_TRY( qlnfGetAggrFuncExecInfo( aSQLStmt,
                                              sOptGroupInstant->mNestedAggrFuncCnt,
                                              sOptGroupInstant->mNestedAggrOptInfo,
                                              aDataArea,
                                              & sExeGroupInstant->mNestedAggrExecInfo,
                                              & QLL_DATA_PLAN( aDBCStmt ),
                                              aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /* aggr fetch */
            STL_TRY( qlnfGetAggrFuncAggrFetchInfo( aSQLStmt,
                                                   sOptGroupInstant->mNestedAggrFuncCnt,
                                                   sOptGroupInstant->mNestedAggrOptInfo,
                                                   aDataArea,
                                                   & sFetchInfo->mAggrFetchInfo,
                                                   & QLL_DATA_PLAN( aDBCStmt ),
                                                   aEnv )
                     == STL_SUCCESS );
        }
    }
    else
    {
        /* Do Nothing */
    }


    /**
     * Nested Aggregation Distinct Functions Execution 정보 설정
     */

    if( sOptGroupInstant->mNestedAggrDistFuncCnt > 0 )
    {
        /* fetch & aggr */
        STL_TRY( qlnfGetAggrDistFuncExecInfo( aSQLStmt,
                                              sOptGroupInstant->mNestedAggrDistFuncCnt,
                                              sOptGroupInstant->mNestedAggrDistOptInfo,
                                              aDataArea,
                                              & sExeGroupInstant->mNestedAggrDistExecInfo,
                                              & QLL_DATA_PLAN( aDBCStmt ),
                                              aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /* Do Nothing */
    }

        
    /***************************************************************************
     * Execution Node 설정
     **************************************************************************/

    /**
     * Common Execution Node 정보 설정
     */

    sExecNode->mNodeType  = QLL_PLAN_NODE_GROUP_HASH_INSTANT_ACCESS_TYPE;
    sExecNode->mOptNode   = aOptNode;
    sExecNode->mExecNode  = sExeGroupInstant;

    if( aDataArea->mIsEvaluateCost == STL_TRUE )
    {
        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                    STL_SIZEOF( qllExecutionCost ),
                                    (void **) & sExecNode->mExecutionCost,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemset( sExecNode->mExecutionCost, 0x00, STL_SIZEOF( qllExecutionCost ) );
    }
    else
    {
        sExecNode->mExecutionCost = NULL;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlnd */
