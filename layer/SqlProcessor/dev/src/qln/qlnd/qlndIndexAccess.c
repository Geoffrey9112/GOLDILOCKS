/*******************************************************************************
 * qlndIndexAccess.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlndIndexAccess.c 10633 2013-12-10 04:08:24Z jhkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlndIndexAccess.c
 * @brief SQL Processor Layer Data Optimization Node - INDEX ACCESS
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnd
 * @{
 */


/**
 * @brief INDEX ACCESS Execution node에 대한 Data 정보를 구축한다.
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
stlStatus qlndDataOptimizeIndexAccess( smlTransId              aTransID,
                                       qllDBCStmt            * aDBCStmt,
                                       qllStatement          * aSQLStmt,
                                       qllOptimizationNode   * aOptNode,
                                       qllDataArea           * aDataArea,
                                       qllEnv                * aEnv )
{
    qllExecutionNode  * sExecNode       = NULL;
    qlnoIndexAccess   * sOptIndexAccess = NULL;
    qlnxIndexAccess   * sExeIndexAccess = NULL;
    smlFetchInfo      * sFetchInfo      = NULL;
    knlCompareList    * sTempCompList   = NULL;
    stlInt32            sCompListCnt    = 0;
    
    
    /*
     * Paramter Validation
     */

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
     * INDEX ACCESS Execution Node 할당
     */

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlnxIndexAccess ),
                                (void **) & sExeIndexAccess,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sExeIndexAccess, 0x00, STL_SIZEOF( qlnxIndexAccess ) );


    /**
     * Index Iterator 에 대한 Fetch Info 설정
     */

    sFetchInfo = & sExeIndexAccess->mFetchInfo;


    /**
     * Iterator를 위한 Index Key Column Block List 구성
     * : Index Column Order로 구성
     */

    STL_DASSERT( sOptIndexAccess->mIndexReadColList != NULL );

    if( sOptIndexAccess->mIndexReadColList->mCount > 0 )
    {
        STL_TRY( qlndBuildIndexColBlockList( aDataArea,
                                             sOptIndexAccess->mIndexReadColList,
                                             sOptIndexAccess->mIndexHandle,
                                             & sExeIndexAccess->mIndexReadColBlock,
                                             & QLL_DATA_PLAN( aDBCStmt ),
                                             aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sExeIndexAccess->mIndexReadColBlock = NULL;
    }
    sFetchInfo->mColList = sExeIndexAccess->mIndexReadColBlock;


    /**
     * Iterator를 위한 Table Column Block List 구성
     * : Table Column Order로 구성
     */
    
    STL_DASSERT( sOptIndexAccess->mTableReadColList != NULL );

    if( sOptIndexAccess->mTableReadColList->mCount > 0 )
    {
        STL_TRY( qlndBuildTableColBlockList( aDataArea,
                                             sOptIndexAccess->mTableReadColList,
                                             STL_TRUE,
                                             & sExeIndexAccess->mTableReadColBlock,
                                             & QLL_DATA_PLAN( aDBCStmt ),
                                             aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sExeIndexAccess->mTableReadColBlock = NULL;
    }


    /**
     * Iterator를 위한 Total Table Column Block List 구성
     * : Table Column Order로 구성
     */

    STL_DASSERT( sOptIndexAccess->mTotalReadColList != NULL );

    if( sOptIndexAccess->mTotalReadColList->mCount > 0 )
    {
        STL_TRY( qlndBuildTableColBlockList( aDataArea,
                                             sOptIndexAccess->mTotalReadColList,
                                             STL_TRUE,
                                             & sExeIndexAccess->mTotalReadColBlock,
                                             & QLL_DATA_PLAN( aDBCStmt ),
                                             aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sExeIndexAccess->mTotalReadColBlock = NULL;
    }


    /**
     * RowId 를 위한 Value Block List 구성
     */

    if( sOptIndexAccess->mRowIdColumn != NULL )
    {
        STL_TRY( qlndBuildTableRowIdColBlockList( aDataArea,
                                                  sOptIndexAccess->mRowIdColumn,
                                                  & sExeIndexAccess->mRowIdColBlock,
                                                  & QLL_DATA_PLAN( aDBCStmt ),
                                                  aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sExeIndexAccess->mRowIdColBlock = NULL;
    }
    sFetchInfo->mRowIdColList = sExeIndexAccess->mRowIdColBlock;

    /**
     * Table Logical ID : RowId Column 구성시 사용됨.
     */
    
    sFetchInfo->mTableLogicalId = ellGetTableID( sOptIndexAccess->mTableHandle );
    
    
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
    sFetchInfo->mBlockJoinFetchInfo   = NULL;

    
    /**
     * Iterator 의 Read Mode 는 Dictionary Iterator, Foreign Key Iterator 를 제외하고
     * 모두 SNAPSHOT 의 Transaction Read Mode 와 Statement Read Mode 를 갖는다.
     */

    sExeIndexAccess->mTransReadMode = SML_TRM_SNAPSHOT;
    sExeIndexAccess->mStmtReadMode  = SML_SRM_SNAPSHOT;


    /**
     * Index 관련 정보 & Fetch Record 설정
     */

    STL_TRY( qlndGetRangePredInfo( aDBCStmt,
                                   aSQLStmt,
                                   aDataArea,
                                   sOptIndexAccess,
                                   sExeIndexAccess,
                                   aEnv )
             == STL_SUCCESS );


    /**
     * Aggregation Functions Execution 정보 설정
     */

    if( sOptIndexAccess->mAggrFuncCnt > 0 )
    {
        if( sOptIndexAccess->mAggrDistFuncCnt > 0 )
        {
            /* fetch & aggr */
            STL_TRY( qlnfGetAggrFuncExecInfo( aSQLStmt,
                                              sOptIndexAccess->mAggrFuncCnt,
                                              sOptIndexAccess->mAggrOptInfo,
                                              aDataArea,
                                              & sExeIndexAccess->mAggrExecInfo,
                                              & QLL_DATA_PLAN( aDBCStmt ),
                                              aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /* aggr fetch */
            STL_TRY( qlnfGetAggrFuncAggrFetchInfo( aSQLStmt,
                                                   sOptIndexAccess->mAggrFuncCnt,
                                                   sOptIndexAccess->mAggrOptInfo,
                                                   aDataArea,
                                                   & sFetchInfo->mAggrFetchInfo,
                                                   & QLL_DATA_PLAN( aDBCStmt ),
                                                   aEnv )
                     == STL_SUCCESS );
        }
    }
    else
    {
        sFetchInfo->mAggrFetchInfo     = NULL;
        sExeIndexAccess->mAggrExecInfo = NULL;
    }


    /**
     * Aggregation Distinct Functions Execution 정보 설정
     */
    
    if( sOptIndexAccess->mAggrDistFuncCnt > 0 )
    {
        /* aggr distinct */
        STL_TRY( qlnfGetAggrDistFuncExecInfo( aSQLStmt,
                                              sOptIndexAccess->mAggrDistFuncCnt,
                                              sOptIndexAccess->mAggrDistOptInfo,
                                              aDataArea,
                                              & sExeIndexAccess->mAggrDistExecInfo,
                                              & QLL_DATA_PLAN( aDBCStmt ),
                                              aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sExeIndexAccess->mAggrDistExecInfo = NULL;
    }


    /**
     * Prepare Range 정보 설정
     */

    STL_TRY( qlndMakePrepareStackEvalInfo( aDataArea->mExprDataContext,
                                           sOptIndexAccess->mPrepareRangeStack,
                                           & sExeIndexAccess->mPreRangeEvalInfo,
                                           & QLL_DATA_PLAN( aDBCStmt ),
                                           aEnv )
             == STL_SUCCESS );


    /**
     * Prepare IN Key Range 정보 설정
     */

    STL_TRY( qlndMakePrepareStackEvalInfo( aDataArea->mExprDataContext,
                                           sOptIndexAccess->mPrepareInKeyRangeStack,
                                           & sExeIndexAccess->mPreInKeyRangeEvalInfo,
                                           & QLL_DATA_PLAN( aDBCStmt ),
                                           aEnv )
             == STL_SUCCESS );


    /**
     * Prepare Key Filter 정보 설정
     */

    STL_TRY( qlndMakePrepareStackEvalInfo( aDataArea->mExprDataContext,
                                           sOptIndexAccess->mPrepareKeyFilterStack,
                                           & sExeIndexAccess->mPreKeyFilterEvalInfo,
                                           & QLL_DATA_PLAN( aDBCStmt ),
                                           aEnv )
             == STL_SUCCESS );


    /**
     * Prepare Filter 정보 설정
     */

    STL_TRY( qlndMakePrepareStackEvalInfo( aDataArea->mExprDataContext,
                                           sOptIndexAccess->mPrepareFilterStack,
                                           & sExeIndexAccess->mPreFilterEvalInfo,
                                           & QLL_DATA_PLAN( aDBCStmt ),
                                           aEnv )
             == STL_SUCCESS );


    /**
     * Range 관련 정보 설정
     */

    sExeIndexAccess->mMinRangeCompList = sFetchInfo->mRange->mMinRange;
    sExeIndexAccess->mMaxRangeCompList = sFetchInfo->mRange->mMaxRange;

    sCompListCnt = 0;
    sTempCompList = sFetchInfo->mRange->mMinRange;
    while( sTempCompList != NULL )
    {
        sCompListCnt++;
        sTempCompList = sTempCompList->mNext;
    }
    sExeIndexAccess->mMinRangeTotalCnt = sCompListCnt;
    
    sCompListCnt = 0;
    sTempCompList = sFetchInfo->mRange->mMaxRange;
    while( sTempCompList != NULL )
    {
        sCompListCnt++;
        sTempCompList = sTempCompList->mNext;
    }
    sExeIndexAccess->mMaxRangeTotalCnt = sCompListCnt;


    /**
     * Common Info 설정
     */

    if( sExeIndexAccess->mRowIdColBlock != NULL )
    {
        sExeIndexAccess->mRowIdColBlock->mNext = sExeIndexAccess->mTotalReadColBlock;
        sExeIndexAccess->mCommonInfo.mResultColBlock = sExeIndexAccess->mRowIdColBlock;
    }
    else
    {
        sExeIndexAccess->mCommonInfo.mResultColBlock = sExeIndexAccess->mTotalReadColBlock;
    }


    /**
     * Outer Column Value Block List 설정
     */

    STL_TRY( qlndBuildOuterColBlockList( aDataArea,
                                         sOptIndexAccess->mOuterColumnList,
                                         & sExeIndexAccess->mCommonInfo.mOuterColBlock,
                                         & sExeIndexAccess->mCommonInfo.mOuterOrgColBlock,
                                         & QLL_DATA_PLAN( aDBCStmt ),
                                         aEnv )
             == STL_SUCCESS );

    
    if( sOptIndexAccess->mNeedRowBlock == STL_TRUE )
    {
        qlnxRowBlockItem    * sRowBlockItem;

        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                    STL_SIZEOF( qlnxRowBlockList ),
                                    (void **) & sExeIndexAccess->mCommonInfo.mRowBlockList,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                    STL_SIZEOF( qlnxRowBlockItem ),
                                    (void **) & sRowBlockItem,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sRowBlockItem->mLeafOptNode = aOptNode;
        sRowBlockItem->mRowBlock = sExeIndexAccess->mFetchInfo.mRowBlock;
        sRowBlockItem->mOrgRowBlock = NULL;
        sRowBlockItem->mIsLeftTableRowBlock = STL_TRUE;
        sRowBlockItem->mNext = NULL;

        sExeIndexAccess->mCommonInfo.mRowBlockList->mCount = 1;
        sExeIndexAccess->mCommonInfo.mRowBlockList->mHead = sRowBlockItem;
        sExeIndexAccess->mCommonInfo.mRowBlockList->mTail = sRowBlockItem;
    }
    else
    {
        sExeIndexAccess->mCommonInfo.mRowBlockList = NULL;
    }


    /**
     * IN Key Range 정보 구성
     */

    if( sOptIndexAccess->mListColMapCount > 0 )
    {
        STL_TRY( qlndMakeInKeyRangeInfo( aSQLStmt,
                                         aDataArea,
                                         sOptIndexAccess,
                                         & sExeIndexAccess->mInKeyRange,
                                         & QLL_DATA_PLAN( aDBCStmt ),
                                         aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sExeIndexAccess->mInKeyRange = NULL;
    }


    /**
     * Common Execution Node 정보 설정
     */

    sExecNode->mNodeType  = QLL_PLAN_NODE_INDEX_ACCESS_TYPE;
    sExecNode->mOptNode   = aOptNode;
    sExecNode->mExecNode  = sExeIndexAccess;

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
        sExecNode->mExecutionCost  = NULL;
    }

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

    
/**
 * @brief INDEX ACCESS Optimization Node로 부터 Index Scan에 필요한 range & filter 정보를 구성한다.
 * @param[in]      aDBCStmt           DBC Statement
 * @param[in,out]  aSQLStmt           SQL Statement
 * @param[in]      aDataArea          Data Area (Data Optimization 결과물)
 * @param[in]      aOptIndexAccess    Index Access Optimization Node
 * @param[in,out]  aExeIndexAccess    Index Access Execution Node
 * @param[in]      aEnv               Environment
 */
stlStatus qlndGetRangePredInfo( qllDBCStmt       * aDBCStmt,
                                qllStatement     * aSQLStmt,
                                qllDataArea      * aDataArea,
                                qlnoIndexAccess  * aOptIndexAccess,
                                qlnxIndexAccess  * aExeIndexAccess,
                                qllEnv           * aEnv )
{
    qlnoIndexAccess     * sOptIndexAccess      = NULL;
    qlnxIndexAccess     * sExeIndexAccess      = NULL;

    qloIndexScanInfo    * sIndexScanInfo       = NULL;

    smlFetchInfo        * sFetchInfo           = NULL;
    smlFetchRecordInfo  * sFetchRecordInfo     = NULL;

    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptIndexAccess != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExeIndexAccess != NULL, QLL_ERROR_STACK(aEnv) );
    

    /**
     * Node 획득
     */

    sOptIndexAccess = aOptIndexAccess;
    sExeIndexAccess = aExeIndexAccess;
    sIndexScanInfo  = sOptIndexAccess->mIndexScanInfo;


    /**********************************************************
     * Rewrite Filter 
     **********************************************************/

    /**
     * @todo Predicate 생성 여부 판단
     */


    /**
     * @todo Range & Filter 재구성
     * @remark Min/Max Range 및 Filter 의 Constant Value 를 고려하여,
     *    <BR> 불필요한 조건들을 제거한다.
     *    <BR> 남은 조건들만을 가지고 Physical / Logical Filter 구성
     *    <BR> 
     *    <BR> Bind Parameter에 대한 value가 설정되어 있기 때문에
     *    <BR> 정확한 Range 및 Filter를 구성할 수 있다.
     *    <BR> Scan 여부를 판가름 할 수도 있는 튜닝 요소임.
     */

    
    

    /**
     * @todo MIN Range를 분석하여 불필요한 조건들 제거
     */

    
    /**
     * @todo MAX Range를 분석하여 불필요한 조건들 제거
     */


    /**
     * @todo Physical Key Filter를 분석하여 불필요한 조건들 제거
     */

    
    /**
     * @todo Logical Key Filter를 분석하여 불필요한 조건들 제거
     */


    /**
     * @todo Physical Table Filter를 분석하여 불필요한 조건들 제거
     */


    /**
     * @todo Logical Table Filter를 분석하여 불필요한 조건들 제거
     */
    

    /**
     * 항상 같은 값을 반환하는 경우 Ragne Predicate 생성하지 않음 (Optimization 정보)
     */

    sFetchInfo = & sExeIndexAccess->mFetchInfo;


    /**********************************************************
     * Key Compare List 설정
     **********************************************************/

    /**
     * Key Compare List 구성
     */

    STL_TRY( ellGetKeyCompareList( aOptIndexAccess->mIndexHandle,
                                   & QLL_DATA_PLAN( aDBCStmt ),
                                   & sFetchInfo->mKeyCompList,
                                   ELL_ENV(aEnv) )
             == STL_SUCCESS );

    
    /**********************************************************
     * Range 정보 설정
     **********************************************************/

    if( sIndexScanInfo->mIsExist == STL_FALSE )
    {
        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                    STL_SIZEOF( knlRange ),
                                    (void **) & sFetchInfo->mRange,
                                    KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
        
        sFetchInfo->mRange->mMinRange = NULL;
        sFetchInfo->mRange->mMaxRange = NULL;
        sFetchInfo->mRange->mNext     = NULL;
        sFetchInfo->mPhysicalFilter   = NULL;
        sFetchInfo->mLogicalFilter    = NULL;

        sFetchInfo->mFilterEvalInfo   = NULL;

        STL_THROW( RAMP_SET_FETCH_RECORD_INFO );
    }
    else
    {
        /* Do Nothing */
    }


    /**********************************************************
     * Index Scan을 위한 Fetch Info 구성
     **********************************************************/
    
    /**
     * Range 관련 Compare List 구성
     */

    STL_TRY( qlndSetFetchInfoFromRangeExpr( aDataArea,
                                            sOptIndexAccess,
                                            sExeIndexAccess,
                                            & QLL_DATA_PLAN( aDBCStmt ),
                                            aEnv )
             == STL_SUCCESS );


    /**********************************************************
     * Index Scan을 위한 Fetch Record Info 구성
     **********************************************************/

    STL_RAMP( RAMP_SET_FETCH_RECORD_INFO );


    /**
     * Fetch Record 필요 여부 검사
     */

    if( sExeIndexAccess->mTableReadColBlock == NULL )
    {
        sFetchRecordInfo = NULL;
        STL_THROW( RAMP_FINISH );
    }
    else
    {
        /* Do Nothing */
    }

    
    /**
     * Fetch Record 정보를 위한 공간 할당
     */
        
    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( smlFetchRecordInfo ),
                                (void **) & sFetchRecordInfo,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    
    /**
     * Fetch Record 정보 설정 : Table & Read Column 관련
     */
    
    sFetchRecordInfo->mRelationHandle  = sOptIndexAccess->mTablePhysicalHandle;
    sFetchRecordInfo->mColList         = sExeIndexAccess->mTableReadColBlock;
    sFetchRecordInfo->mRowIdColList    = NULL;
    sFetchRecordInfo->mTableLogicalId  = sFetchInfo->mTableLogicalId;
    
    
    /**
     * Physical Table Filter 구성
     */

    if( sIndexScanInfo->mPhysicalTableFilterExprList != NULL )
    {
        if( sIndexScanInfo->mPhysicalTableFilterExprList->mCount > 0 )
        {
            STL_TRY( qlndMakePhysicalFilterFromExprList(
                         sIndexScanInfo->mPhysicalTableFilterExprList,
                         sExeIndexAccess->mTableReadColBlock,
                         sExeIndexAccess->mIndexReadColBlock,
                         aDataArea->mExprDataContext,
                         STL_FALSE,
                         sOptIndexAccess->mIndexHandle,
                         & QLL_DATA_PLAN( aDBCStmt ),
                         & sFetchRecordInfo->mPhysicalFilter,
                         aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            sFetchRecordInfo->mPhysicalFilter = NULL;
        }
    }
    else
    {
        sFetchRecordInfo->mPhysicalFilter = NULL;
    }

    
    /**
     * Iterator 의 Read Mode 는 Dictionary Iterator, Foreign Key Iterator 를 제외하고
     * 모두 SNAPSHOT 의 Transaction Read Mode 와 Statement Read Mode 를 갖는다.
     */

    sFetchRecordInfo->mTransReadMode = SML_TRM_SNAPSHOT;
    sFetchRecordInfo->mStmtReadMode  = SML_SRM_SNAPSHOT;


    /**
     * Scn Block List 설정
     */
    
    sFetchRecordInfo->mScnBlock = sFetchInfo->mRowBlock->mScnBlock;
    

    /**
     * Logical Table Filter 구성
     */

    if( sOptIndexAccess->mTableLogicalStack == NULL )
    {
        sFetchRecordInfo->mLogicalFilter = NULL;
        sFetchRecordInfo->mLogicalFilterEvalInfo = NULL;
    }
    else
    {
        sFetchRecordInfo->mLogicalFilter = sOptIndexAccess->mTableLogicalStack;
        
        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                    STL_SIZEOF( knlExprEvalInfo ),
                                    (void **) & sFetchRecordInfo->mLogicalFilterEvalInfo,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sFetchRecordInfo->mLogicalFilterEvalInfo->mExprStack =
            sOptIndexAccess->mTableLogicalStack;
        sFetchRecordInfo->mLogicalFilterEvalInfo->mContext =
            aDataArea->mExprDataContext;
        sFetchRecordInfo->mLogicalFilterEvalInfo->mResultBlock =
            aDataArea->mPredResultBlock;
        sFetchRecordInfo->mLogicalFilterEvalInfo->mBlockIdx = 0;
        sFetchRecordInfo->mLogicalFilterEvalInfo->mBlockCount = 0;
    }


    /**
     * View Scn & Tcn 초기화 : storage layer 에서 설정하는 부분
     */
    
    sFetchRecordInfo->mViewScn = 0;
    sFetchRecordInfo->mViewTcn = 0;
    
            
    STL_RAMP( RAMP_FINISH );

    sFetchInfo->mFetchRecordInfo = sFetchRecordInfo;

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Range Expression List로부터 Fetch를 위한 Range Predicate 정보 구성하기
 * @param[in]      aDataArea          Data Area (Data Optimization 결과물)
 * @param[in]      aOptIndexAccess    Index Access Optimization Node
 * @param[in,out]  aExeIndexAccess    Index Access Execution Node
 * @param[in]      aRegionMem         영역 기반 메모리 할당자 (knlRegionMem)
 * @param[in,out]  aEnv               커널 Environment
 */
stlStatus qlndSetFetchInfoFromRangeExpr( qllDataArea      * aDataArea,
                                         qlnoIndexAccess  * aOptIndexAccess,
                                         qlnxIndexAccess  * aExeIndexAccess,
                                         knlRegionMem     * aRegionMem,
                                         qllEnv           * aEnv )
{
    qloIndexScanInfo   * sIndexScanInfo     = NULL;
    knlExprContext     * sExprContexts      = NULL;
    ellIndexKeyDesc    * sIndexKeyDesc      = NULL;
    qlvRefExprItem     * sListItem          = NULL;
    qlvInitExpression  * sExpr              = NULL;
    
    smlFetchInfo       * sFetchInfo         = NULL;

    knlCompareList     * sMinRangeCompList  = NULL;
    knlCompareList     * sMaxRangeCompList  = NULL;
    knlCompareList     * sCurComp           = NULL;
    dtlDataValue       * sRangeValue        = NULL;
    dtlDataType          sColDataType       = DTL_TYPE_NA;
    knlPredicateList   * sRangePred         = NULL;
 
    stlInt32             sLoop              = 0;
    stlInt32             sRangeExprCnt      = 0;
    stlUInt32            sKeyColOffset      = 0;
    stlUInt32            sRangeValOffset    = 0;
    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptIndexAccess != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExeIndexAccess != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Range Expression 존재 여부 검사
     */

    sIndexScanInfo = aOptIndexAccess->mIndexScanInfo;
    sFetchInfo     = & aExeIndexAccess->mFetchInfo;

    STL_TRY_THROW( aOptIndexAccess->mIndexScanInfo->mReadKeyCnt > 0,
                   RAMP_MAKE_KEY_PHYSICAL_FILTER );

    sExprContexts  = aDataArea->mExprDataContext->mContexts;
    sIndexKeyDesc  = ellGetIndexKeyDesc( aOptIndexAccess->mIndexHandle );
    

    /*************************************************
     * MIN Range Compare List 구성
     *************************************************/

    /**
     * Compare List 공간 할당
     */

    sRangeExprCnt = 0;
    for( sLoop = 0 ;
         ( ( sLoop < sIndexScanInfo->mReadKeyCnt ) &&
           ( sIndexScanInfo->mMinRangeExpr[ sLoop ]->mCount > 0 ) );
         sLoop++ )
    {
        sRangeExprCnt += sIndexScanInfo->mMinRangeExpr[ sLoop ]->mCount;
    }

    if( sRangeExprCnt > 0 )
    {
        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( knlCompareList ) * sRangeExprCnt,
                                    (void**) & sMinRangeCompList,
                                    KNL_ENV( aEnv ) )
                 == STL_SUCCESS );

        stlMemset( sMinRangeCompList,
                   0x00,
                   STL_SIZEOF( knlCompareList ) * sRangeExprCnt );


        /**
         * Compare List 설정
         */

        sCurComp = sMinRangeCompList;
        for( sLoop = 0 ; sLoop < sIndexScanInfo->mReadKeyCnt ; sLoop++ )
        {
            sListItem = sIndexScanInfo->mMinRangeExpr[ sLoop ]->mHead;
        
            while( sListItem != NULL )
            {
                /**
                 * Expression으로부터 Range 구성 정보 얻기
                 */
            
                sExpr = sListItem->mExprPtr;
                STL_TRY( qloGetRangeFromExpr( sExpr,
                                              sIndexScanInfo->mNullValueExpr->mOffset,
                                              sIndexScanInfo->mFalseValueExpr->mOffset,
                                              & sKeyColOffset,
                                              & sRangeValOffset,
                                              & sCurComp->mIsIncludeEqual,
                                              & sCurComp->mCompNullStop,
                                              & sCurComp->mIsLikeFunc,
                                              aEnv )
                         == STL_SUCCESS );

            
                /**
                 * Comapre 정보 구성
                 */
            
                sColDataType = sExprContexts[ sKeyColOffset ].mInfo.mValueInfo->mDataValue[0].mType;

                if( sExpr->mExpr.mFunction->mFuncId == KNL_BUILTIN_FUNC_LIKE )
                {
                    STL_DASSERT( sExpr->mExpr.mFunction->mArgs[1]->mIterationTime !=
                                 DTL_ITERATION_TIME_FOR_EACH_EXPR );

                    STL_DASSERT( sExprContexts[ sRangeValOffset ].mCast == NULL );

                    switch( sExpr->mExpr.mFunction->mArgs[1]->mType )
                    {
                        case QLV_EXPR_TYPE_FUNCTION :
                        case QLV_EXPR_TYPE_CAST :
                        case QLV_EXPR_TYPE_CASE_EXPR :
                            sRangeValue = & sExprContexts[ sRangeValOffset ].mInfo.mFuncData->mResultValue;
                            break;
                        case QLV_EXPR_TYPE_LIST_FUNCTION :
                            sRangeValue = & sExprContexts[ sRangeValOffset ].mInfo.mListFunc->mResultValue;
                            break;
                        default :
                            sRangeValue = & sExprContexts[ sRangeValOffset ].mInfo.mValueInfo->mDataValue[0];
                            break;
                    }
                }
                else
                {
                    STL_DASSERT( sExpr->mExpr.mFunction->mArgs[0]->mType == QLV_EXPR_TYPE_COLUMN );
                    
                    if( sExprContexts[ sRangeValOffset ].mCast == NULL )
                    {
                        if( sExpr->mExpr.mFunction->mArgCount == 1 )
                        {
                            sRangeValue = & sExprContexts[ sRangeValOffset ].mInfo.mValueInfo->mDataValue[0];
                        }
                        else
                        {
                            switch( sExpr->mExpr.mFunction->mArgs[1]->mType )
                            {
                                case QLV_EXPR_TYPE_FUNCTION :
                                case QLV_EXPR_TYPE_CAST :
                                case QLV_EXPR_TYPE_CASE_EXPR :
                                    sRangeValue = & sExprContexts[ sRangeValOffset ].mInfo.mFuncData->mResultValue;
                                    break;
                                case QLV_EXPR_TYPE_LIST_FUNCTION :
                                    sRangeValue = & sExprContexts[ sRangeValOffset ].mInfo.mListFunc->mResultValue;
                                    break;
                                default :
                                    sRangeValue = & sExprContexts[ sRangeValOffset ].mInfo.mValueInfo->mDataValue[0];
                                    break;
                            }
                        }
                    }
                    else
                    {
                        sRangeValue = & sExprContexts[ sRangeValOffset ].mCast->mCastResultBuf;
                    }
                }
                
                sCurComp->mColOrder    = sLoop;
                sCurComp->mRangeVal    = sRangeValue->mValue;
                sCurComp->mRangeLen    = sRangeValue->mLength;
                sCurComp->mConstVal    = sRangeValue;

                if( sCurComp->mIsLikeFunc == STL_TRUE )
                {
                    sCurComp->mCompFunc = dtlLikePhysicalCompareChar;
                }
                else
                {
                    sCurComp->mCompFunc =
                        dtlPhysicalCompareFuncList[ sColDataType ][ sRangeValue->mType ];
                }

                sCurComp->mIsAsc       = ( sIndexKeyDesc[ sLoop ].mKeyOrdering ==
                                           ELL_INDEX_COLUMN_ASCENDING );
                sCurComp->mIsNullFirst = ( sIndexKeyDesc[ sLoop ].mKeyNullOrdering ==
                                           ELL_INDEX_COLUMN_NULLS_FIRST );

                if( sIndexScanInfo->mNullValueExpr->mOffset == sRangeValOffset )
                {
                    sCurComp->mIsDiffTypeCmp = STL_FALSE;
                }
                else
                {
                    sCurComp->mIsDiffTypeCmp = ( sColDataType != sRangeValue->mType );
                }
                
                sCurComp->mNext = sCurComp + 1;
                sCurComp++;

                sListItem = sListItem->mNext;
            }
        }

        sMinRangeCompList[ sRangeExprCnt - 1 ].mNext = NULL;
    }
    else
    {
        sMinRangeCompList = NULL;
    }

    
    /*************************************************
     * MAX Range Compare List 구성
     *************************************************/

    /**
     * Compare List 공간 할당
     */

    sRangeExprCnt = 0;
    for( sLoop = 0 ;
         ( ( sLoop < sIndexScanInfo->mReadKeyCnt ) &&
           ( sIndexScanInfo->mMaxRangeExpr[ sLoop ]->mCount > 0 ) );
         sLoop++ )
    {
        sRangeExprCnt += sIndexScanInfo->mMaxRangeExpr[ sLoop ]->mCount;
    }
    
    if( sRangeExprCnt > 0 )
    {
        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( knlCompareList ) * sRangeExprCnt,
                                    (void**) & sMaxRangeCompList,
                                    KNL_ENV( aEnv ) )
                 == STL_SUCCESS );

        stlMemset( sMaxRangeCompList,
                   0x00,
                   STL_SIZEOF( knlCompareList ) * sRangeExprCnt );


        /**
         * Compare List 설정
         */

        sCurComp = sMaxRangeCompList;
        for( sLoop = 0 ; sLoop < sIndexScanInfo->mReadKeyCnt ; sLoop++ )
        {
            sListItem = sIndexScanInfo->mMaxRangeExpr[ sLoop ]->mHead;
        
            while( sListItem != NULL )
            {
                /**
                 * Expression으로부터 Range 구성 정보 얻기
                 */
            
                sExpr = sListItem->mExprPtr;
                STL_TRY( qloGetRangeFromExpr( sExpr,
                                              sIndexScanInfo->mNullValueExpr->mOffset,
                                              sIndexScanInfo->mFalseValueExpr->mOffset,
                                              & sKeyColOffset,
                                              & sRangeValOffset,
                                              & sCurComp->mIsIncludeEqual,
                                              & sCurComp->mCompNullStop,
                                              & sCurComp->mIsLikeFunc,
                                              aEnv )
                         == STL_SUCCESS );

            
                /**
                 * Comapre 정보 구성
                 */
            
                sColDataType = sExprContexts[ sKeyColOffset ].mInfo.mValueInfo->mDataValue[0].mType;

                if( sExpr->mExpr.mFunction->mFuncId == KNL_BUILTIN_FUNC_LIKE )
                {
                    STL_DASSERT( sExpr->mExpr.mFunction->mArgs[1]->mIterationTime !=
                                 DTL_ITERATION_TIME_FOR_EACH_EXPR );

                    STL_DASSERT( sExprContexts[ sRangeValOffset ].mCast == NULL );
                    
                    switch( sExpr->mExpr.mFunction->mArgs[1]->mType )
                    {
                        case QLV_EXPR_TYPE_FUNCTION :
                        case QLV_EXPR_TYPE_CAST :
                        case QLV_EXPR_TYPE_CASE_EXPR :
                            sRangeValue = & sExprContexts[ sRangeValOffset ].mInfo.mFuncData->mResultValue;
                            break;
                        case QLV_EXPR_TYPE_LIST_FUNCTION :
                            sRangeValue = & sExprContexts[ sRangeValOffset ].mInfo.mListFunc->mResultValue;
                            break;
                        default :
                            sRangeValue = & sExprContexts[ sRangeValOffset ].mInfo.mValueInfo->mDataValue[0];
                            break;
                    }
                }
                else
                {
                    if( sExprContexts[ sRangeValOffset ].mCast == NULL )
                    {
                        STL_DASSERT( sExpr->mExpr.mFunction->mArgs[0]->mType == QLV_EXPR_TYPE_COLUMN );

                        if( sExpr->mExpr.mFunction->mArgCount == 1 )
                        {
                            sRangeValue = & sExprContexts[ sRangeValOffset ].mInfo.mValueInfo->mDataValue[0];
                        }
                        else
                        {
                            switch( sExpr->mExpr.mFunction->mArgs[1]->mType )
                            {
                                case QLV_EXPR_TYPE_FUNCTION :
                                case QLV_EXPR_TYPE_CAST :
                                case QLV_EXPR_TYPE_CASE_EXPR :
                                    sRangeValue = & sExprContexts[ sRangeValOffset ].mInfo.mFuncData->mResultValue;
                                    break;
                                case QLV_EXPR_TYPE_LIST_FUNCTION :
                                    sRangeValue = & sExprContexts[ sRangeValOffset ].mInfo.mListFunc->mResultValue;
                                    break;
                                default :
                                    sRangeValue = & sExprContexts[ sRangeValOffset ].mInfo.mValueInfo->mDataValue[0];
                                    break;
                            }
                        }
                    }
                    else
                    {
                        sRangeValue = & sExprContexts[ sRangeValOffset ].mCast->mCastResultBuf;
                    }
                }
                
                sCurComp->mColOrder    = sLoop;
                sCurComp->mRangeVal    = sRangeValue->mValue;
                sCurComp->mRangeLen    = sRangeValue->mLength;
                sCurComp->mConstVal    = sRangeValue;

                if( sCurComp->mIsLikeFunc == STL_TRUE )
                {
                    sCurComp->mCompFunc = dtlLikePhysicalCompareChar;
                }
                else
                {
                    sCurComp->mCompFunc =
                        dtlPhysicalCompareFuncList[ sColDataType ][ sRangeValue->mType ];
                }

                sCurComp->mIsAsc       = ( sIndexKeyDesc[ sLoop ].mKeyOrdering ==
                                           ELL_INDEX_COLUMN_ASCENDING );
                sCurComp->mIsNullFirst = ( sIndexKeyDesc[ sLoop ].mKeyNullOrdering ==
                                           ELL_INDEX_COLUMN_NULLS_FIRST );

                if( sIndexScanInfo->mNullValueExpr->mOffset == sRangeValOffset )
                {
                    sCurComp->mIsDiffTypeCmp = STL_FALSE;
                }
                else
                {
                    sCurComp->mIsDiffTypeCmp = ( sColDataType != sRangeValue->mType );
                }
                
                sCurComp->mNext = sCurComp + 1;
                sCurComp++;

                sListItem = sListItem->mNext;
            }
        }

        sMaxRangeCompList[ sRangeExprCnt - 1 ].mNext = NULL;
    }
    else
    {
        sMaxRangeCompList = NULL;
    }


    /*************************************************
     * Key Physical Filter 구성
     *************************************************/

    STL_RAMP( RAMP_MAKE_KEY_PHYSICAL_FILTER );

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( knlRange ),
                                (void **) & sFetchInfo->mRange,
                                KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    
    sFetchInfo->mRange->mMinRange = sMinRangeCompList;
    sFetchInfo->mRange->mMaxRange = sMaxRangeCompList;
    sFetchInfo->mRange->mNext     = NULL;


    /**
     * Physical Key Filter 구성 : Fetch Info의 Physical Filter
     */

    STL_DASSERT( sIndexScanInfo->mPhysicalKeyFilterExprList != NULL );
    
    if( sIndexScanInfo->mPhysicalKeyFilterExprList->mCount > 0 )
    {
        STL_TRY( qlndMakePhysicalFilterFromExprList(
                     sIndexScanInfo->mPhysicalKeyFilterExprList,
                     aExeIndexAccess->mTableReadColBlock,
                     aExeIndexAccess->mIndexReadColBlock,
                     aDataArea->mExprDataContext,
                     STL_TRUE,
                     aOptIndexAccess->mIndexHandle,
                     aRegionMem,
                     & sFetchInfo->mPhysicalFilter,
                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sFetchInfo->mPhysicalFilter = NULL;
    }

    
    /*************************************************
     * Range Predicate 구성 : Logical Stack 
     *************************************************/

    /**
     * Range Predicate 구성
     */

    if( aOptIndexAccess->mKeyLogicalStack == NULL )
    {
        aExeIndexAccess->mFetchInfo.mLogicalFilter = NULL;
        aExeIndexAccess->mFetchInfo.mFilterEvalInfo = NULL;
        sRangePred = NULL;
    }
    else
    {
        aExeIndexAccess->mFetchInfo.mLogicalFilter = aOptIndexAccess->mKeyLogicalStack;
        
        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( knlExprEvalInfo ),
                                    (void **) & sFetchInfo->mFilterEvalInfo,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        sFetchInfo->mFilterEvalInfo->mExprStack   = aOptIndexAccess->mKeyLogicalStack;
        sFetchInfo->mFilterEvalInfo->mContext     = aDataArea->mExprDataContext;
        sFetchInfo->mFilterEvalInfo->mResultBlock = aDataArea->mPredResultBlock;
        sFetchInfo->mFilterEvalInfo->mBlockIdx    = 0;
        sFetchInfo->mFilterEvalInfo->mBlockCount  = 0;

        STL_TRY( knlMakeRange( NULL, /* 설정할 필요 없음 */
                               NULL, /* 설정할 필요 없음 */
                               aOptIndexAccess->mKeyLogicalStack,
                               & sRangePred,
                               aRegionMem,
                               KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
    }

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Execution List를 위한 Physical Filter 정보 구축 
 * @param[in]     aExprList          Expression List
 * @param[in]     aTableValueBlocks  Read Value Block List
 * @param[in]     aIndexValueBlocks  Read Value Block List
 * @param[in]     aExprDataContext   Context Info
 * @param[in]     aIsKeyFilter       Key Filter 인지 여부
 * @param[in]     aIndexHandle       Index Handle
 * @param[in]     aRegionMem         Region Memory
 * @param[out]    aPhysicalFilter    Physical Filter
 * @param[in]     aEnv               Environment
 */
stlStatus qlndMakePhysicalFilterFromExprList( qlvRefExprList       * aExprList,
                                              knlValueBlockList    * aTableValueBlocks,
                                              knlValueBlockList    * aIndexValueBlocks,
                                              knlExprContextInfo   * aExprDataContext,
                                              stlBool                aIsKeyFilter,
                                              ellDictHandle        * aIndexHandle,
                                              knlRegionMem         * aRegionMem,
                                              knlPhysicalFilter   ** aPhysicalFilter,
                                              qllEnv               * aEnv )
{
    qlvInitExpression     * sFilterExpr;
    knlPhysicalFilter     * sPhysicalFilter = NULL;
    knlPhysicalFilter     * sCurFilter      = NULL;
    knlValueBlockList     * sValueBlock;
    qlvInitFunction       * sFuncCode;
    qlvInitListFunc       * sListFunc       = NULL;
    qlvInitListCol        * sListColLeft    = NULL;
    qlvInitRowExpr        * sRowExpr1       = NULL;
    qlvInitListCol        * sListColRight   = NULL;
//    qlvInitRowExpr        * sRowExpr2       = NULL;
    knlColumnInReadRow    * sColumnInReadRow;
    dtlDataValue          * sDataValue;
    stlInt32                sLoop1 = 0;
    stlInt32                sLoop2 = 0;

    knlPhysicalFilterPtr  * sSortList = NULL;
    knlPhysicalFilter     * sSortTemp = NULL;

    stlInt32                sExprListCnt = 0;
    qlvRefExprItem        * sListItem = NULL;
    
    ellDictHandle         * sColumnHandle = NULL;
    ellIndexKeyDesc       * sIndexKeyDesc = NULL;
    
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprDataContext != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPhysicalFilter != NULL, QLL_ERROR_STACK(aEnv) );

    
#define GET_INDEX_VALUE_BLOCK_WITH_COLUMN_ORDER( block, order )                                     \
    {                                                                                               \
        if( aIsKeyFilter == STL_TRUE )                                                              \
        {                                                                                           \
            (block) = aIndexValueBlocks;                                                            \
            while( (block) != NULL )                                                                \
            {                                                                                       \
                STL_DASSERT( KNL_GET_BLOCK_COLUMN_ORDER( (block) ) <                                \
                             ellGetIndexKeyCount( aIndexHandle ) );                                 \
                sColumnHandle =                                                                     \
                    & sIndexKeyDesc[ KNL_GET_BLOCK_COLUMN_ORDER( (block) ) ].mKeyColumnHandle;      \
                if( ellGetColumnIdx( sColumnHandle ) == (order) )                                   \
                {                                                                                   \
                    break;                                                                          \
                }                                                                                   \
                (block) = (block)->mNext;                                                           \
            }                                                                                       \
        }                                                                                           \
        else                                                                                        \
        {                                                                                           \
            (block) = aTableValueBlocks;                                                            \
            while( (block) != NULL )                                                                \
            {                                                                                       \
                if( KNL_GET_BLOCK_COLUMN_ORDER( (block) ) == (order) )                              \
                {                                                                                   \
                    break;                                                                          \
                }                                                                                   \
                (block) = (block)->mNext;                                                           \
            }                                                                                       \
                                                                                                    \
            if( (block) == NULL )                                                                   \
            {                                                                                       \
                (block) = aIndexValueBlocks;                                                        \
                while( (block) != NULL )                                                            \
                {                                                                                   \
                    STL_DASSERT( KNL_GET_BLOCK_COLUMN_ORDER( (block) ) <                            \
                                 ellGetIndexKeyCount( aIndexHandle ) );                             \
                    sColumnHandle =                                                                 \
                        & sIndexKeyDesc[ KNL_GET_BLOCK_COLUMN_ORDER( (block) ) ].mKeyColumnHandle;  \
                    if( ellGetColumnIdx( sColumnHandle ) == (order) )                               \
                    {                                                                               \
                        break;                                                                      \
                    }                                                                               \
                    (block) = (block)->mNext;                                                       \
                }                                                                                   \
            }                                                                                       \
        }                                                                                           \
        STL_DASSERT( (block) != NULL );                                                             \
    }


    /**
     * Index 정보 설정
     */

    if( aIndexHandle != NULL )
    {
        sIndexKeyDesc = ellGetIndexKeyDesc( aIndexHandle );
    }
    else
    {
        /* Do Nothing */
    }

    
    /**
     * Filter 공간 할당
     */

    sExprListCnt = aExprList->mCount;

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( knlPhysicalFilter ) * sExprListCnt,
                                (void **) & sPhysicalFilter,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    

    /**
     * Physical Filter List 생성
     */
    
    sCurFilter = sPhysicalFilter;
    sListItem  = aExprList->mHead;
    
    while( sListItem != NULL )
    {
        /**
         * Filter 구성
         */
        sFilterExpr = sListItem->mExprPtr;
        
        /* Column Order 기준으로 Filter entry 정렬 */
        if( sFilterExpr->mType == QLV_EXPR_TYPE_FUNCTION )
        {
            /* function 으로 구성된 filter */
            sFuncCode = sFilterExpr->mExpr.mFunction;
            
            if( sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_COLUMN )
            {
                if( ( sFuncCode->mArgCount == 2 ) &&
                    ( sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_COLUMN ) )
                {
                    /**
                     * 두 개의 column으로 구성된 filter
                     */
                    if( ellGetColumnIdx( sFuncCode->mArgs[0]->mExpr.mColumn->mColumnHandle ) >=
                        ellGetColumnIdx( sFuncCode->mArgs[1]->mExpr.mColumn->mColumnHandle ) )
                    {
                        /**
                         * column vs column ( idx1 >= idx2 )
                         */
                        
                        /* column value 설정 */
                        GET_INDEX_VALUE_BLOCK_WITH_COLUMN_ORDER(
                            sValueBlock,
                            ellGetColumnIdx( sFuncCode->mArgs[0]->mExpr.mColumn->mColumnHandle ) );

                        sCurFilter->mColIdx1 = ellGetColumnIdx( sFuncCode->mArgs[0]->mExpr.mColumn->mColumnHandle );
                        sCurFilter->mColVal1 = & sValueBlock->mColumnInRow;

                        /* column value 설정 */
                        GET_INDEX_VALUE_BLOCK_WITH_COLUMN_ORDER(
                            sValueBlock,
                            ellGetColumnIdx( sFuncCode->mArgs[1]->mExpr.mColumn->mColumnHandle ) );

                        sCurFilter->mColIdx2  = ellGetColumnIdx( sFuncCode->mArgs[1]->mExpr.mColumn->mColumnHandle );
                        sCurFilter->mColVal2  = & sValueBlock->mColumnInRow;
                        sCurFilter->mConstVal = NULL;

                        /* function pointer 설정 */
                        qloGetPhysicalFuncPtr( sFuncCode->mFuncId,
                                               ellGetColumnDBType( sFuncCode->mArgs[0]->mExpr.mColumn->mColumnHandle ),
                                               ellGetColumnDBType( sFuncCode->mArgs[1]->mExpr.mColumn->mColumnHandle ),
                                               & sCurFilter->mFunc,
                                               STL_FALSE );
                    }
                    else
                    {
                        /**
                         * column vs column ( idx1 < idx2 ) : Operand의 위치 바꾸기 => reverse function 적용
                         */

                        /* column value 설정 */
                        GET_INDEX_VALUE_BLOCK_WITH_COLUMN_ORDER(
                            sValueBlock,
                            ellGetColumnIdx( sFuncCode->mArgs[1]->mExpr.mColumn->mColumnHandle ) );
                        
                        sCurFilter->mColIdx1 = ellGetColumnIdx( sFuncCode->mArgs[1]->mExpr.mColumn->mColumnHandle );
                        sCurFilter->mColVal1 = & sValueBlock->mColumnInRow;

                        /* column value 설정 */
                        GET_INDEX_VALUE_BLOCK_WITH_COLUMN_ORDER(
                            sValueBlock,
                            ellGetColumnIdx( sFuncCode->mArgs[0]->mExpr.mColumn->mColumnHandle ) );

                        sCurFilter->mColIdx2  = ellGetColumnIdx( sFuncCode->mArgs[0]->mExpr.mColumn->mColumnHandle );
                        sCurFilter->mColVal2  = & sValueBlock->mColumnInRow;
                        sCurFilter->mConstVal = NULL;
                        
                        /* function pointer 설정 */
                        qloGetPhysicalFuncPtr( sFuncCode->mFuncId,
                                               ellGetColumnDBType( sFuncCode->mArgs[0]->mExpr.mColumn->mColumnHandle ),
                                               ellGetColumnDBType( sFuncCode->mArgs[1]->mExpr.mColumn->mColumnHandle ),
                                               & sCurFilter->mFunc,
                                               STL_TRUE );
                    }
                }
                else
                {
                    if( sFuncCode->mArgCount == 1 )
                    {
                        /**
                         * column 하나만으로 구성된 filter
                         */ 
            
                        /* column value 설정 */
                        GET_INDEX_VALUE_BLOCK_WITH_COLUMN_ORDER(
                            sValueBlock,
                            ellGetColumnIdx( sFuncCode->mArgs[0]->mExpr.mColumn->mColumnHandle ) );
            
                        sCurFilter->mColIdx1 = ellGetColumnIdx( sFuncCode->mArgs[0]->mExpr.mColumn->mColumnHandle );
                        sCurFilter->mColVal1 = & sValueBlock->mColumnInRow;

                        /* emtpy value 설정 */
                        sCurFilter->mColIdx2  = KNL_PHYSICAL_VALUE_CONSTANT;
                        sCurFilter->mColVal2  = knlEmtpyColumnValue;
                        sCurFilter->mConstVal = NULL;
                        
                        /* function pointer 설정 */
                        qloGetPhysicalFuncPtr( sFuncCode->mFuncId,
                                               ellGetColumnDBType( sFuncCode->mArgs[0]->mExpr.mColumn->mColumnHandle ),
                                               0,
                                               & sCurFilter->mFunc,
                                               STL_FALSE );
                    }
                    else
                    {
                        /**
                         * column vs constant value
                         */
                        
                        /* column value 설정 */
                        GET_INDEX_VALUE_BLOCK_WITH_COLUMN_ORDER(
                            sValueBlock,
                            ellGetColumnIdx( sFuncCode->mArgs[0]->mExpr.mColumn->mColumnHandle ) );

                        sCurFilter->mColIdx1 = ellGetColumnIdx( sFuncCode->mArgs[0]->mExpr.mColumn->mColumnHandle );
                        sCurFilter->mColVal1 = & sValueBlock->mColumnInRow;

                        /* constant value 설정 */
                        if( aExprDataContext->mContexts[ sFuncCode->mArgs[1]->mOffset ].mCast != NULL )
                        {
                            sDataValue = & aExprDataContext->mContexts[ sFuncCode->mArgs[1]->mOffset ].mCast->mCastResultBuf;
                        }
                        else
                        {
                            if( (sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT) ||
                                (sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_VALUE) ||
                                (sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_REFERENCE) ||
                                (sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_OUTER_COLUMN) )
                            {
                                sDataValue = aExprDataContext->mContexts[ sFuncCode->mArgs[1]->mOffset ].
                                    mInfo.mValueInfo->mDataValue;
                            }
                            else
                            {
                                if( sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_LIST_FUNCTION )
                                {
                                    sDataValue = & aExprDataContext->mContexts[ sFuncCode->mArgs[1]->mOffset ].
                                        mInfo.mListFunc->mResultValue;
                                }
                                else
                                {
                                    sDataValue = & aExprDataContext->mContexts[ sFuncCode->mArgs[1]->mOffset ].
                                        mInfo.mFuncData->mResultValue;
                                }
                            }
                        }

                        STL_TRY( knlAllocRegionMem( aRegionMem,
                                                    STL_SIZEOF( knlColumnInReadRow ),
                                                    (void **) & sColumnInReadRow,
                                                    KNL_ENV(aEnv) )
                                 == STL_SUCCESS );

                        sColumnInReadRow->mValue  = (void *) sDataValue->mValue;
                        sColumnInReadRow->mLength = sDataValue->mLength;
                                
                        sCurFilter->mColIdx2  = KNL_PHYSICAL_VALUE_CONSTANT;
                        sCurFilter->mColVal2  = sColumnInReadRow;
                        sCurFilter->mConstVal = sDataValue;
                        
                        /* function pointer 설정 */
                        qloGetPhysicalFuncPtr( sFuncCode->mFuncId,
                                               ellGetColumnDBType( sFuncCode->mArgs[0]->mExpr.mColumn->mColumnHandle ),
                                               sDataValue->mType,
                                               & sCurFilter->mFunc,
                                               STL_FALSE );
                    }
                }
            }
            else
            {
                /**
                 * column vs constant value : reverse function
                 */
                
                STL_DASSERT( sFuncCode->mArgCount == 2 );         /* constant value만으로 구성된 physical filter는 없다. */
                STL_DASSERT( sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_COLUMN );
                
                /* column value 설정 */
                GET_INDEX_VALUE_BLOCK_WITH_COLUMN_ORDER(
                    sValueBlock,
                    ellGetColumnIdx( sFuncCode->mArgs[1]->mExpr.mColumn->mColumnHandle ) );

                sCurFilter->mColIdx1 = ellGetColumnIdx( sFuncCode->mArgs[1]->mExpr.mColumn->mColumnHandle );
                sCurFilter->mColVal1 = & sValueBlock->mColumnInRow;

                /* constant value 설정 */
                if( aExprDataContext->mContexts[ sFuncCode->mArgs[0]->mOffset ].mCast != NULL )
                {
                    sDataValue = & aExprDataContext->mContexts[ sFuncCode->mArgs[0]->mOffset ].mCast->mCastResultBuf;
                }
                else
                {
                    if( (sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT) ||
                        (sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_VALUE) ||
                        (sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_REFERENCE) ||
                        (sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_OUTER_COLUMN) )
                    {
                        sDataValue = aExprDataContext->mContexts[ sFuncCode->mArgs[0]->mOffset ].
                            mInfo.mValueInfo->mDataValue;
                    }
                    else
                    {
                        if( sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_LIST_FUNCTION )
                        {
                            sDataValue = & aExprDataContext->mContexts[ sFuncCode->mArgs[0]->mOffset ].
                                mInfo.mFuncData->mResultValue;
                        }
                        else
                        {
                            sDataValue = & aExprDataContext->mContexts[ sFuncCode->mArgs[0]->mOffset ].
                                mInfo.mFuncData->mResultValue;
                        }
                    }
                }

                STL_TRY( knlAllocRegionMem( aRegionMem,
                                            STL_SIZEOF( knlColumnInReadRow ),
                                            (void **) & sColumnInReadRow,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                sColumnInReadRow->mValue  = (void *) sDataValue->mValue;
                sColumnInReadRow->mLength = sDataValue->mLength;
                                
                sCurFilter->mColIdx2  = KNL_PHYSICAL_VALUE_CONSTANT;
                sCurFilter->mColVal2  = sColumnInReadRow;
                sCurFilter->mConstVal = sDataValue;

                /* function pointer 설정 */
                qloGetPhysicalFuncPtr( sFuncCode->mFuncId,
                                       sDataValue->mType,
                                       ellGetColumnDBType( sFuncCode->mArgs[1]->mExpr.mColumn->mColumnHandle ),
                                       & sCurFilter->mFunc,
                                       STL_TRUE );
           }
        }
        else if( sFilterExpr->mType == QLV_EXPR_TYPE_LIST_FUNCTION )
        {
            /* List Function으로 구성된 filter */
            sListFunc = sFilterExpr->mExpr.mListFunc;
            STL_DASSERT( sListFunc->mArgs[1]->mType == QLV_EXPR_TYPE_LIST_COLUMN );

            sListColLeft = sListFunc->mArgs[1]->mExpr.mListCol;
            STL_DASSERT( sListColLeft->mArgs[0]->mType == QLV_EXPR_TYPE_ROW_EXPR );

            sRowExpr1 = sListColLeft->mArgs[0]->mExpr.mRowExpr;
           
            STL_DASSERT( sRowExpr1->mArgs[0]->mType == QLV_EXPR_TYPE_COLUMN );
            
            /* column value 설정 */
            GET_INDEX_VALUE_BLOCK_WITH_COLUMN_ORDER(
                sValueBlock,
                ellGetColumnIdx( sRowExpr1->mArgs[0]->mExpr.mColumn->mColumnHandle ) );

            sCurFilter->mColIdx1 = ellGetColumnIdx( sRowExpr1->mArgs[0]->mExpr.mColumn->mColumnHandle );
            sCurFilter->mColVal1 = & sValueBlock->mColumnInRow;

            STL_DASSERT( sListFunc->mArgs[0]->mType == QLV_EXPR_TYPE_LIST_COLUMN );

            sListColRight = sListFunc->mArgs[0]->mExpr.mListCol;
            STL_DASSERT( sListColRight->mArgs[0]->mType == QLV_EXPR_TYPE_ROW_EXPR );

//            sRowExpr2 = sListColRight->mArgs[0]->mExpr.mRowExpr;

            /* Constant value 설정 */

            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF( knlColumnInReadRow ),
                                        (void **) & sColumnInReadRow,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF( dtlDataValue ) * ( sListColRight->mArgCount + 1 ),
                                        (void **) & sDataValue,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            
            /**
             * @todo IN Physical Function Pointer
             * aIsInPhysical = STL_TRUE인 경우, 기존의 Physical Function 과 달리
             * Left와 Right의 void형 포인트로 dtlDataValue 가 list 로 오게된다.
             * Function pointer를 통해 dtlDataValue를 physical function안에서 하나씩 처리하도록 함.
             */

            /* RightVal의 마지막 DataValue는 Left의 Type만 갖고 있다 */
            sDataValue[sListColRight->mArgCount].mType
                = ellGetColumnDBType( sRowExpr1->mArgs[0]->mExpr.mColumn->mColumnHandle );
            
            for( sLoop2 = 0 ; sLoop2 < sListColRight->mArgCount ; sLoop2 ++ )
            {
                if( aExprDataContext->mContexts
                    [ sListColRight->mArgs[sLoop2]->mExpr.mRowExpr->mArgs[0]->mOffset ].mCast != NULL )
                {
                    sDataValue[sLoop2].mValue = & aExprDataContext->mContexts
                        [ sListColRight->mArgs[sLoop2]->mExpr.mRowExpr->mArgs[0]->mOffset ].mCast->mCastResultBuf;
                }
                else
                {
                    if( (sListColRight->mArgs[sLoop2]->mExpr.mRowExpr->mArgs[0]->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT) ||
                        (sListColRight->mArgs[sLoop2]->mExpr.mRowExpr->mArgs[0]->mType == QLV_EXPR_TYPE_VALUE) ||
                        (sListColRight->mArgs[sLoop2]->mExpr.mRowExpr->mArgs[0]->mType == QLV_EXPR_TYPE_REFERENCE) ||
                        (sListColRight->mArgs[sLoop2]->mExpr.mRowExpr->mArgs[0]->mType == QLV_EXPR_TYPE_OUTER_COLUMN) )
                    {
                        sDataValue[sLoop2].mValue = aExprDataContext->mContexts
                            [ sListColRight->mArgs[sLoop2]->mExpr.mRowExpr->mArgs[0]->mOffset ].mInfo.mValueInfo->mDataValue;
                    }
                    else
                    {
                        if( sListColRight->mArgs[sLoop2]->mExpr.mRowExpr->mArgs[0]->mType ==
                            QLV_EXPR_TYPE_LIST_FUNCTION )
                        {
                            sDataValue[sLoop2].mValue = & aExprDataContext->mContexts
                                [ sListColRight->mArgs[sLoop2]->mExpr.mRowExpr->mArgs[0]->mOffset ].mInfo.mListFunc->mResultValue;
                        }
                        else
                        {
                            sDataValue[sLoop2].mValue = & aExprDataContext->mContexts
                                [ sListColRight->mArgs[sLoop2]->mExpr.mRowExpr->mArgs[0]->mOffset ].mInfo.mFuncData->mResultValue;
                        }
                    }
                }
            }
            sColumnInReadRow->mValue  = sDataValue;
            sColumnInReadRow->mLength = sListColRight->mArgCount;
                    
            sCurFilter->mColIdx2  = KNL_PHYSICAL_LIST_VALUE_CONSTANT;
            sCurFilter->mColVal2  = sColumnInReadRow;
            sCurFilter->mConstVal = sDataValue;
            
            sCurFilter->mFunc = dtlInPhysicalFuncPointer[KNL_IN_PHYSICAL_FUNC_ID_PTR( sListFunc->mFuncId )];
        }
        else
        {
            /* Coverage : code opt에서 value expression에 대한 range 를 구성하지 않음 */
            STL_DASSERT( 0 );

            /* column 하나만으로 구성된 filter */ 
            /* STL_DASSERT( sFilterExpr->mType == QLV_EXPR_TYPE_COLUMN ); */

            /* /\* function pointer 설정 *\/ */
            /* sCurFilter->mFunc = dtlPhysicalFuncIsTrue; */
            
            /* /\* column value 설정 *\/ */
            /* GET_INDEX_VALUE_BLOCK_WITH_COLUMN_ORDER( */
            /*     sValueBlock, */
            /*     ellGetColumnIdx( sFilterExpr->mExpr.mColumn->mColumnHandle ) ); */
            
            /* sCurFilter->mColIdx1 = ellGetColumnIdx( sFilterExpr->mExpr.mColumn->mColumnHandle ); */
            /* sCurFilter->mColVal1 = & sValueBlock->mColumnInRow; */

            /* /\* emtpy value 설정 *\/ */
            /* sCurFilter->mColIdx2  = KNL_PHYSICAL_VALUE_CONSTANT; */
            /* sCurFilter->mColVal2  = knlEmtpyColumnValue; */
            /* sCurFilter->mConstVal = NULL; */
        }

        STL_DASSERT( sCurFilter->mFunc != NULL );

        sCurFilter++;
        sListItem = sListItem->mNext;
    }

    
    /**
     * Column1 Order 기준으로 Filter 정렬
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( knlPhysicalFilterPtr ) * sExprListCnt,
                                (void **) & sSortList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* insertion sort */
    for( sLoop1 = 0 ; sLoop1 < sExprListCnt ; sLoop1++ )
    {
        sSortList[ sLoop1 ] = & sPhysicalFilter[ sLoop1 ];
    }

    for( sLoop1 = 1 ; sLoop1 < sExprListCnt ; sLoop1++ )
    {
        if( sSortList[ sLoop1 - 1 ]->mColIdx1 <= sSortList[ sLoop1 ]->mColIdx1 )
        {
            continue;
        }

        sSortTemp = sSortList[ sLoop1 ];
        sSortList[ sLoop1 ] = sSortList[ sLoop1 - 1 ];
            
        for( sLoop2 = sLoop1 - 1 ; sLoop2 > 0 ; sLoop2-- )
        {
            if( sSortList[ sLoop2 - 1 ]->mColIdx1 <= sSortTemp->mColIdx1 )
            {
                break;
            }
            sSortList[ sLoop2 ] = sSortList[ sLoop2 - 1 ];
        }
        sSortList[ sLoop2 ] = sSortTemp;
    }

    
    /**
     * Physical Filter List의 Link 구성
     */
    
    for( sLoop1 = 0 ; sLoop1 < sExprListCnt - 1 ; sLoop1++ )
    {
        sSortList[ sLoop1 ]->mNext = sSortList[ sLoop1 + 1 ];
    }
    sSortList[ sLoop1 ]->mNext = NULL;


    /**
     * OUTPUT 설정
     */
    
    *aPhysicalFilter = sSortList[0];

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief In Key Range 위한 Fetch Info 정보 구축 
 * @param[in]     aSQLStmt                SQL Statement
 * @param[in]     aDataArea               Data Area (Data Optimization 결과물)
 * @param[in]     aOptIndexAccess         Index Access Opt Node
 * @param[out]    aInKeyRangeFetchInfo    In Key Range Fetch Info
 * @param[in]     aRegionMem              영역 기반 메모리 할당자 (knlRegionMem)
 * @param[in]     aEnv                    Environment
 */
stlStatus qlndMakeInKeyRangeInfo( qllStatement              * aSQLStmt,
                                  qllDataArea               * aDataArea,
                                  qlnoIndexAccess           * aOptIndexAccess,
                                  qlnxInKeyRangeFetchInfo  ** aInKeyRangeFetchInfo,
                                  knlRegionMem              * aRegionMem,
                                  qllEnv                    * aEnv )
{
    qlnxInKeyRangeFetchInfo  * sInKeyRange  = NULL;

    knlExprContext           * sContexts    = NULL;
    knlExprContext           * sCurContext  = NULL;
    knlValueBlockList        * sLastBlock   = NULL;
    knlValueBlockList        * sNewBlock    = NULL;
    knlValueBlockList        * sCurBlock    = NULL;

    qlvInitListCol           * sListCol     = NULL;
    qlvInitRowExpr           * sRowExpr     = NULL;

    dtlDataValue             * sDataValue   = NULL;

    smlFetchInfo             * sFetchInfo   = NULL;

    stlInt32                   sRowIdx      = 0;
    stlInt32                   sColIdx      = 0;
    stlInt32                   sLoop        = 0;
    stlInt32                   sRowCnt      = 0;
    stlUInt8                   sSortKeyFlag = DTL_KEYCOLUMN_EMPTY;

    
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptIndexAccess != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInKeyRangeFetchInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    

    /**
     * IN Key Range 정보 공간 할당
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlnxInKeyRangeFetchInfo ),
                                (void **) & sInKeyRange,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sInKeyRange, 0x00, STL_SIZEOF( qlnxInKeyRangeFetchInfo ) );
        
    /**
     * Value Block 설정
     */

    sContexts = aDataArea->mExprDataContext->mContexts;

    sLastBlock = NULL;
    sColIdx    = 0;

    for( sLoop = 0 ; sLoop < aOptIndexAccess->mListColMapCount ; sLoop++ )
    {
        STL_DASSERT( aOptIndexAccess->mValueExpr[ sLoop ]->mType == 
                     QLV_EXPR_TYPE_VALUE );

        sCurContext = & sContexts[ aOptIndexAccess->mValueExpr[ sLoop ]->mOffset ];

        STL_DASSERT( sCurContext->mCast == NULL );

        sDataValue = sCurContext->mInfo.mValueInfo->mDataValue;

        STL_TRY( knlInitBlockWithDataValue(
                     & sNewBlock,
                     sDataValue,
                     sDataValue->mType,
                     STL_LAYER_SQL_PROCESSOR,
                     gResultDataTypeDef[ sDataValue->mType ].mArgNum1,
                     gResultDataTypeDef[ sDataValue->mType ].mArgNum2,
                     gResultDataTypeDef[ sDataValue->mType ].mStringLengthUnit,
                     gResultDataTypeDef[ sDataValue->mType ].mIntervalIndicator,
                     aRegionMem,
                     KNL_ENV( aEnv ) )
                 == STL_SUCCESS );

        KNL_SET_BLOCK_COLUMN_ORDER( sNewBlock, sColIdx );
        sColIdx++;
        
        if( sLastBlock == NULL )
        {
            sInKeyRange->mValueBlock = sNewBlock;
        }
        else
        {
            KNL_LINK_BLOCK_LIST( sLastBlock, sNewBlock );
        }
        sLastBlock = sNewBlock;
    }


    /**
     * List Column Block 설정
     */
        
    sCurBlock  = sInKeyRange->mValueBlock;
    sLastBlock = NULL;
    sColIdx    = 0;
        
    while( sCurBlock != NULL )
    {
        STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                    & sNewBlock,
                                    aDataArea->mBlockAllocCnt,
                                    STL_TRUE,
                                    STL_LAYER_SQL_PROCESSOR,
                                    0,
                                    sColIdx,
                                    KNL_GET_BLOCK_DB_TYPE( sCurBlock ),
                                    KNL_GET_BLOCK_ARG_NUM1( sCurBlock ),
                                    KNL_GET_BLOCK_ARG_NUM2( sCurBlock ),
                                    KNL_GET_BLOCK_STRING_LENGTH_UNIT( sCurBlock ),
                                    KNL_GET_BLOCK_INTERVAL_INDICATOR( sCurBlock ),
                                    aRegionMem,
                                    aEnv )
                 == STL_SUCCESS );
        sColIdx++;

        if( sLastBlock == NULL )
        {
            sInKeyRange->mListColBlock = sNewBlock;
        }
        else
        {
            KNL_LINK_BLOCK_LIST( sLastBlock, sNewBlock );
        }
        sLastBlock = sNewBlock;

        sCurBlock = sCurBlock->mNext;
    }

        
    /**
     * Insert Column Block 설정
     */

    sRowCnt = aOptIndexAccess->mListColExpr[ 0 ]->mExpr.mListCol->mArgCount;
    sInKeyRange->mRecCnt = sRowCnt;

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( knlValueBlockList * ) * sRowCnt,
                                (void **) & sInKeyRange->mInsertColBlock,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    for( sRowIdx = 0 ; sRowIdx < sRowCnt ; sRowIdx++ )
    {
        sLastBlock = NULL;

        for( sColIdx = 0 ; sColIdx < aOptIndexAccess->mListColMapCount ; sColIdx++ )
        {
            STL_DASSERT( aOptIndexAccess->mListColExpr[ sColIdx ] != NULL );
            STL_DASSERT( aOptIndexAccess->mListColExpr[ sColIdx ]->mType == QLV_EXPR_TYPE_LIST_COLUMN );
            sListCol = aOptIndexAccess->mListColExpr[ sColIdx ]->mExpr.mListCol;

            STL_DASSERT( sListCol->mArgs[ sRowIdx ]->mType == QLV_EXPR_TYPE_ROW_EXPR );
            sRowExpr = sListCol->mArgs[ sRowIdx ]->mExpr.mRowExpr;
            
            STL_DASSERT( sRowExpr->mArgCount == 1 );
            sCurContext = & sContexts[ sRowExpr->mArgs[ 0 ]->mOffset ];

            if( sCurContext->mCast == NULL )
            {
                switch( sRowExpr->mArgs[ 0 ]->mType )
                {
                    case QLV_EXPR_TYPE_VALUE :
                    case QLV_EXPR_TYPE_BIND_PARAM :
                    case QLV_EXPR_TYPE_PSEUDO_COLUMN :
                    case QLV_EXPR_TYPE_COLUMN :
                    case QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT :
                    case QLV_EXPR_TYPE_REFERENCE :
                    case QLV_EXPR_TYPE_INNER_COLUMN :
                    case QLV_EXPR_TYPE_OUTER_COLUMN :
                    case QLV_EXPR_TYPE_ROWID_COLUMN :
                    case QLV_EXPR_TYPE_AGGREGATION :
                    case QLV_EXPR_TYPE_ROW_EXPR :
                        {
                            sDataValue = sCurContext->mInfo.mValueInfo->mDataValue;
                            break;
                        }
                    case QLV_EXPR_TYPE_FUNCTION :
                    case QLV_EXPR_TYPE_CAST :
                    case QLV_EXPR_TYPE_CASE_EXPR :
                        {
                            sDataValue = & sCurContext->mInfo.mFuncData->mResultValue;
                            break;
                        }
                    case QLV_EXPR_TYPE_LIST_FUNCTION :
                        {
                            sDataValue = & sCurContext->mInfo.mListFunc->mResultValue;
                            break;
                        }
                        /* case QLV_EXPR_TYPE_LIST_COLUMN : */
                        /* case QLV_EXPR_TYPE_SUB_QUERY : */
                        /* case QLV_EXPR_TYPE_PSEUDO_ARGUMENT : */
                    default :
                        {
                            STL_DASSERT( 0 );
                            break;
                        }
                }
            }
            else
            {
                sDataValue = & sCurContext->mCast->mCastResultBuf;
            }

            STL_TRY( knlInitBlockWithDataValue(
                         & sNewBlock,
                         sDataValue,
                         sDataValue->mType,
                         STL_LAYER_SQL_PROCESSOR,
                         gResultDataTypeDef[ sDataValue->mType ].mArgNum1,
                         gResultDataTypeDef[ sDataValue->mType ].mArgNum2,
                         gResultDataTypeDef[ sDataValue->mType ].mStringLengthUnit,
                         gResultDataTypeDef[ sDataValue->mType ].mIntervalIndicator,
                         aRegionMem,
                         KNL_ENV( aEnv ) )
                     == STL_SUCCESS );

            KNL_SET_BLOCK_COLUMN_ORDER( sNewBlock, sColIdx );
            
            if( sLastBlock == NULL )
            {
                sInKeyRange->mInsertColBlock[ sRowIdx ] = sNewBlock;
            }
            else
            {
                KNL_LINK_BLOCK_LIST( sLastBlock, sNewBlock );
            }
            sLastBlock = sNewBlock;
        }        
    }


    /**
     * Sort Instant Key 공간 할당
     */
    
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( stlUInt8 ) * aOptIndexAccess->mListColMapCount,
                                (void **) & sInKeyRange->mSortKeyFlags,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    DTL_SET_INDEX_COLUMN_FLAG( sSortKeyFlag,
                               DTL_KEYCOLUMN_INDEX_ORDER_ASC,
                               DTL_KEYCOLUMN_INDEX_NULLABLE_FALSE,
                               DTL_KEYCOLUMN_INDEX_NULL_ORDER_LAST );
                    
    stlMemset( sInKeyRange->mSortKeyFlags,
               sSortKeyFlag,
               STL_SIZEOF( stlUInt8 ) * aOptIndexAccess->mListColMapCount );

        
    /**
     * Row Block 공간 할당
     */

    STL_TRY( smlInitRowBlock( & sInKeyRange->mRowBlock,
                              aDataArea->mBlockAllocCnt,
                              aRegionMem,
                              SML_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Fetch Info 설정
     */
        
    sFetchInfo = & sInKeyRange->mFetchInfo;

    stlMemset( sFetchInfo, 0x00, STL_SIZEOF( smlFetchInfo ) );

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( knlRange ),
                                (void **) & sFetchInfo->mRange,
                                KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    stlMemset( sFetchInfo->mRange, 0x00, STL_SIZEOF( knlRange ) );
    
    sFetchInfo->mColList = sInKeyRange->mListColBlock;
    sFetchInfo->mRowBlock = & sInKeyRange->mRowBlock;


    /**
     * OUTPUT 설정
     */

    *aInKeyRangeFetchInfo = sInKeyRange;
    

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/** @} qlno */
