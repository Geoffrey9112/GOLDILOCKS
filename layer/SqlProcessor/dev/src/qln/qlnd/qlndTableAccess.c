/*******************************************************************************
 * qlndTableAccess.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlndTableAccess.c 10602 2013-12-05 06:56:59Z jhkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlndTableAccess.c
 * @brief SQL Processor Layer Data Optimization Node - TABLE ACCESS
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnd
 * @{
 */


/**
 * @brief TABLE ACCESS Execution node에 대한 Data 정보를 구축한다.
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
stlStatus qlndDataOptimizeTableAccess( smlTransId              aTransID,
                                       qllDBCStmt            * aDBCStmt,
                                       qllStatement          * aSQLStmt,
                                       qllOptimizationNode   * aOptNode,
                                       qllDataArea           * aDataArea,
                                       qllEnv                * aEnv )
{
    qllExecutionNode    * sExecNode       = NULL;
    qlnoTableAccess     * sOptTableAccess = NULL;
    qlnxTableAccess     * sExeTableAccess = NULL;
    smlFetchInfo        * sFetchInfo      = NULL;
    
    
    /*
     * Paramter Validation
     */

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
     * TABLE ACCESS Execution Node 할당
     */

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlnxTableAccess ),
                                (void **) & sExeTableAccess,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sExeTableAccess, 0x00, STL_SIZEOF( qlnxTableAccess ) );


    /**
     * Table Iterator 에 대한 Fetch Info 설정
     */

    sFetchInfo = & sExeTableAccess->mFetchInfo;


    /**
     * Iterator를 위한 Read Value Block List 구성
     */

    STL_DASSERT( sOptTableAccess->mReadColList != NULL );
    
    if( sOptTableAccess->mReadColList->mCount > 0 )
    {
        STL_TRY( qlndBuildTableColBlockList( aDataArea,
                                             sOptTableAccess->mReadColList,
                                             STL_TRUE,
                                             & sExeTableAccess->mReadColBlock,
                                             & QLL_DATA_PLAN( aDBCStmt ),
                                             aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sExeTableAccess->mReadColBlock = NULL;
    }
    sFetchInfo->mColList = sExeTableAccess->mReadColBlock;

    
    /**
     * RowId 를 위한 Value Block List 구성
     */

    if( sOptTableAccess->mRowIdColumn != NULL )
    {
        STL_TRY( qlndBuildTableRowIdColBlockList( aDataArea,
                                                  sOptTableAccess->mRowIdColumn,
                                                  & sExeTableAccess->mRowIdColBlock,
                                                  & QLL_DATA_PLAN( aDBCStmt ),
                                                  aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sExeTableAccess->mRowIdColBlock = NULL;
    }
    sFetchInfo->mRowIdColList = sExeTableAccess->mRowIdColBlock;

    /**
     * Table Logical ID : RowId Column 구성시 사용됨.
     */
    
    sFetchInfo->mTableLogicalId = ellGetTableID( sOptTableAccess->mTableHandle );

    
    /**
     * Physical Filter 구성
     */
    
    if( sOptTableAccess->mPhysicalFilterExpr == NULL )
    {
        sFetchInfo->mPhysicalFilter = NULL;
    }
    else
    {
        STL_TRY( qlndMakePhysicalFilter( sOptTableAccess->mRelationNode,
                                         sOptTableAccess->mPhysicalFilterExpr,
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

    if( sOptTableAccess->mLogicalFilterPred == NULL )
    {
        sFetchInfo->mLogicalFilter  = NULL;
        sFetchInfo->mFilterEvalInfo = NULL;
    }
    else
    {
        sFetchInfo->mLogicalFilter   = KNL_PRED_GET_FILTER( sOptTableAccess->mLogicalFilterPred );

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
    sFetchInfo->mAggrFetchInfo   = NULL;
    sFetchInfo->mBlockJoinFetchInfo   = NULL;
    

    /**
     * Iterator 의 Read Mode 는 Dictionary Iterator, Foreign Key Iterator 를 제외하고
     * 모두 SNAPSHOT 의 Transaction Read Mode 와 Statement Read Mode 를 갖는다.
     */

    sExeTableAccess->mTransReadMode = SML_TRM_SNAPSHOT;
    sExeTableAccess->mStmtReadMode  = SML_SRM_SNAPSHOT;

    
    /**
     * Aggregation Functions Execution 정보 설정
     */

    if( sOptTableAccess->mAggrFuncCnt > 0 )
    {
        if( sOptTableAccess->mAggrDistFuncCnt > 0 )
        {
            /* fetch & aggr */
            STL_TRY( qlnfGetAggrFuncExecInfo( aSQLStmt,
                                              sOptTableAccess->mAggrFuncCnt,
                                              sOptTableAccess->mAggrOptInfo,
                                              aDataArea,
                                              & sExeTableAccess->mAggrExecInfo,
                                              & QLL_DATA_PLAN( aDBCStmt ),
                                              aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /* aggr fetch */
            STL_TRY( qlnfGetAggrFuncAggrFetchInfo( aSQLStmt,
                                                   sOptTableAccess->mAggrFuncCnt,
                                                   sOptTableAccess->mAggrOptInfo,
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
        sExeTableAccess->mAggrExecInfo = NULL;
    }

    sFetchInfo->mBlockJoinFetchInfo   = NULL;

    /**
     * Aggregation Distinct Functions Execution 정보 설정
     */
    
    if( sOptTableAccess->mAggrDistFuncCnt > 0 )
    {
        /* aggr distinct */
        STL_TRY( qlnfGetAggrDistFuncExecInfo( aSQLStmt,
                                              sOptTableAccess->mAggrDistFuncCnt,
                                              sOptTableAccess->mAggrDistOptInfo,
                                              aDataArea,
                                              & sExeTableAccess->mAggrDistExecInfo,
                                              & QLL_DATA_PLAN( aDBCStmt ),
                                              aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sExeTableAccess->mAggrDistExecInfo = NULL;
    }


    /**
     * Outer Column Value Block List 설정
     */

    STL_TRY( qlndBuildOuterColBlockList( aDataArea,
                                         sOptTableAccess->mOuterColumnList,
                                         & sExeTableAccess->mCommonInfo.mOuterColBlock,
                                         & sExeTableAccess->mCommonInfo.mOuterOrgColBlock,
                                         & QLL_DATA_PLAN( aDBCStmt ),
                                         aEnv )
             == STL_SUCCESS );


    /**
     * Prepare Filter 정보 설정
     */

    STL_TRY( qlndMakePrepareStackEvalInfo( aDataArea->mExprDataContext,
                                           sOptTableAccess->mPrepareFilterStack,
                                           & sExeTableAccess->mPreFilterEvalInfo,
                                           & QLL_DATA_PLAN( aDBCStmt ),
                                           aEnv )
             == STL_SUCCESS );


    /**
     * Common Info 설정
     */

    if( sExeTableAccess->mRowIdColBlock != NULL )
    {
        sExeTableAccess->mRowIdColBlock->mNext = sExeTableAccess->mReadColBlock;
        sExeTableAccess->mCommonInfo.mResultColBlock = sExeTableAccess->mRowIdColBlock;
    }
    else
    {
        sExeTableAccess->mCommonInfo.mResultColBlock = sExeTableAccess->mReadColBlock;
    }

    if( sOptTableAccess->mNeedRowBlock == STL_TRUE )
    {
        qlnxRowBlockItem    * sRowBlockItem;

        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                    STL_SIZEOF( qlnxRowBlockList ),
                                    (void **) & sExeTableAccess->mCommonInfo.mRowBlockList,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                    STL_SIZEOF( qlnxRowBlockItem ),
                                    (void **) & sRowBlockItem,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sRowBlockItem->mLeafOptNode = aOptNode;
        sRowBlockItem->mRowBlock = sExeTableAccess->mFetchInfo.mRowBlock;
        sRowBlockItem->mOrgRowBlock = NULL;
        sRowBlockItem->mIsLeftTableRowBlock = STL_TRUE;
        sRowBlockItem->mNext = NULL;

        sExeTableAccess->mCommonInfo.mRowBlockList->mCount = 1;
        sExeTableAccess->mCommonInfo.mRowBlockList->mHead = sRowBlockItem;
        sExeTableAccess->mCommonInfo.mRowBlockList->mTail = sRowBlockItem;
    }
    else
    {
        sExeTableAccess->mCommonInfo.mRowBlockList = NULL;
    }

    /**
     * Table Physical Handle
     */

    sExeTableAccess->mPhysicalHandle = ellGetTableHandle( sOptTableAccess->mTableHandle );


    /**
     * Common Execution Node 정보 설정
     */

    sExecNode->mNodeType  = QLL_PLAN_NODE_TABLE_ACCESS_TYPE;
    sExecNode->mOptNode   = aOptNode;
    sExecNode->mExecNode  = sExeTableAccess;

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

    
/** @} qlnd */
