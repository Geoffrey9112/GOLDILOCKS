/*******************************************************************************
 * qlndRowIdAccess.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlndRowIdAccess.c 10602 2013-12-05 06:56:59Z jhkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlndRowIdAccess.c
 * @brief SQL Processor Layer Data Optimization Node - ROWID ACCESS
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnd
 * @{
 */


/**
 * @brief ROWID ACCESS Execution node에 대한 Data 정보를 구축한다.
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
stlStatus qlndDataOptimizeRowIdAccess( smlTransId              aTransID,
                                       qllDBCStmt            * aDBCStmt,
                                       qllStatement          * aSQLStmt,
                                       qllOptimizationNode   * aOptNode,
                                       qllDataArea           * aDataArea,
                                       qllEnv                * aEnv )
{
    qllExecutionNode    * sExecNode        = NULL;
    qlnoRowIdAccess     * sOptRowIdAccess  = NULL;
    qlnxRowIdAccess     * sExeRowIdAccess  = NULL;

    smlFetchInfo        * sFetchInfo       = NULL;
    smlFetchRecordInfo  * sFetchRecordInfo = NULL;

    
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_ROWID_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * ROWID ACCESS Optimization Node 획득
     */

    sOptRowIdAccess = (qlnoRowIdAccess *) aOptNode->mOptNode;


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
     * ROWID ACCESS Execution Node 할당
     */

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlnxRowIdAccess ),
                                (void **) & sExeRowIdAccess,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sExeRowIdAccess, 0x00, STL_SIZEOF( qlnxRowIdAccess ) );


    /**
     * RowID Iterator 에 대한 Fetch Info 설정
     */

    sFetchInfo = & sExeRowIdAccess->mFetchInfo;


    /**
     * Iterator를 위한 Read Value Block List 구성
     */

    STL_DASSERT( sOptRowIdAccess->mReadColList != NULL );
    
    if( sOptRowIdAccess->mReadColList->mCount > 0 )
    {
        STL_TRY( qlndBuildTableColBlockList( aDataArea,
                                             sOptRowIdAccess->mReadColList,
                                             STL_TRUE,
                                             & sExeRowIdAccess->mReadColBlock,
                                             & QLL_DATA_PLAN( aDBCStmt ),
                                             aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sExeRowIdAccess->mReadColBlock = NULL;
    }
    sFetchInfo->mColList = sExeRowIdAccess->mReadColBlock;


    /**
     * RowId 를 위한 Value Block List 구성
     */

    STL_DASSERT( sOptRowIdAccess->mRowIdColumn != NULL );

    STL_TRY( qlndBuildTableRowIdColBlockList( aDataArea,
                                              sOptRowIdAccess->mRowIdColumn,
                                              & sExeRowIdAccess->mRowIdColBlock,
                                              & QLL_DATA_PLAN( aDBCStmt ),
                                              aEnv )
             == STL_SUCCESS );

    sFetchInfo->mRowIdColList = sExeRowIdAccess->mRowIdColBlock;

    
    /**
     * Table Logical ID : RowId Column 구성시 사용됨.
     */
    
    sFetchInfo->mTableLogicalId = ellGetTableID( sOptRowIdAccess->mTableHandle );
    
    
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

    sFetchInfo->mRange           = NULL;
    sFetchInfo->mPhysicalFilter  = NULL;
    sFetchInfo->mKeyCompList     = NULL;
    sFetchInfo->mFetchRecordInfo = NULL;

    sFetchInfo->mSkipCnt         = 0;
    sFetchInfo->mFetchCnt        = 0;
    sFetchInfo->mIsEndOfScan     = STL_FALSE;

    sFetchInfo->mFilterEvalInfo  = NULL;


    /**
     * Fetch Record 공간 할당
     */

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( smlFetchRecordInfo ),
                                (void **) & sFetchRecordInfo,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    
    /**
     * Fetch Record 정보 설정 : Table & Read Column 관련
     */
    
    sFetchRecordInfo->mRelationHandle  = sOptRowIdAccess->mPhysicalHandle;
    sFetchRecordInfo->mColList         = sExeRowIdAccess->mReadColBlock;
    sFetchRecordInfo->mRowIdColList    = sExeRowIdAccess->mRowIdColBlock;
    sFetchRecordInfo->mTableLogicalId  = sFetchInfo->mTableLogicalId;
    
    
    /**
     * Physical Filter 구성
     */
    
    if( sOptRowIdAccess->mPhysicalFilterExpr == NULL )
    {
        sFetchRecordInfo->mPhysicalFilter = NULL;
    }
    else
    {
        STL_TRY( qlndMakePhysicalFilter( sOptRowIdAccess->mRelationNode,
                                         sOptRowIdAccess->mPhysicalFilterExpr,
                                         sExeRowIdAccess->mReadColBlock,
                                         aDataArea->mExprDataContext,
                                         & QLL_DATA_PLAN( aDBCStmt ),
                                         & sFetchRecordInfo->mPhysicalFilter,
                                         aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Iterator 의 Read Mode 는 Dictionary Iterator, Foreign Key Iterator 를 제외하고
     * 모두 SNAPSHOT 의 Transaction Read Mode 와 Statement Read Mode 를 갖는다.
     */

    sExeRowIdAccess->mTransReadMode = SML_TRM_SNAPSHOT;
    sExeRowIdAccess->mStmtReadMode  = SML_SRM_SNAPSHOT;

    sFetchRecordInfo->mTransReadMode = sExeRowIdAccess->mTransReadMode;
    sFetchRecordInfo->mStmtReadMode  = sExeRowIdAccess->mStmtReadMode;

    
    /**
     * Scn Block List 설정
     */

    sFetchRecordInfo->mScnBlock = sFetchInfo->mRowBlock->mScnBlock;
    

    /**
     * View Scn & Tcn 초기화 : storage layer 에서 설정하는 부분
     */
    
    sFetchRecordInfo->mViewScn = 0;
    sFetchRecordInfo->mViewTcn = 0;


    /**
     * Logical Table Filter 구성
     */

    if( sOptRowIdAccess->mLogicalStack == NULL )
    {
        sFetchRecordInfo->mLogicalFilter = NULL;
        sFetchRecordInfo->mLogicalFilterEvalInfo = NULL;
    }
    else
    {
        sFetchRecordInfo->mLogicalFilter = sOptRowIdAccess->mLogicalStack;
        
        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                    STL_SIZEOF( knlExprEvalInfo ),
                                    (void **) & sFetchRecordInfo->mLogicalFilterEvalInfo,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sFetchRecordInfo->mLogicalFilterEvalInfo->mExprStack =
            sOptRowIdAccess->mLogicalStack;
        sFetchRecordInfo->mLogicalFilterEvalInfo->mContext =
            aDataArea->mExprDataContext;
        sFetchRecordInfo->mLogicalFilterEvalInfo->mResultBlock =
            aDataArea->mPredResultBlock;
        sFetchRecordInfo->mLogicalFilterEvalInfo->mBlockIdx = 0;
        sFetchRecordInfo->mLogicalFilterEvalInfo->mBlockCount = 0;
    }

    sFetchInfo->mFetchRecordInfo = sFetchRecordInfo;


    /**
     * RowId Access 관련 정보 구성
     */
    
    STL_DASSERT( sOptRowIdAccess->mRowIdScanExpr != NULL );
    
    if( sOptRowIdAccess->mRowIdScanExpr->mExpr.mFunction->mArgs[0]->mType
        == QLV_EXPR_TYPE_ROWID_COLUMN ) 
    {
        sExeRowIdAccess->mRowIdScanValueExpr =
            sOptRowIdAccess->mRowIdScanExpr->mExpr.mFunction->mArgs[1];
    }
    else
    {
        if( sOptRowIdAccess->mRowIdScanExpr->mExpr.mFunction->mArgs[1]->mType
            == QLV_EXPR_TYPE_ROWID_COLUMN )
        {
            sExeRowIdAccess->mRowIdScanValueExpr =
                sOptRowIdAccess->mRowIdScanExpr->mExpr.mFunction->mArgs[0];
        }
    }

    sExeRowIdAccess->mRidForRowIdScan = & SML_GET_RID_VALUE( sFetchInfo->mRowBlock, 0 );
    

    /**
     * Aggregation Functions Execution 정보 설정
     */

    if( sOptRowIdAccess->mAggrFuncCnt > 0 )
    {
        /* fetch & aggr */
        STL_TRY( qlnfGetAggrFuncExecInfo( aSQLStmt,
                                          sOptRowIdAccess->mAggrFuncCnt,
                                          sOptRowIdAccess->mAggrOptInfo,
                                          aDataArea,
                                          & sExeRowIdAccess->mAggrExecInfo,
                                          & QLL_DATA_PLAN( aDBCStmt ),
                                          aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sFetchInfo->mAggrFetchInfo     = NULL;
        sExeRowIdAccess->mAggrExecInfo = NULL;
    }

    sFetchInfo->mBlockJoinFetchInfo   = NULL;

    
    /**
     * Prepare Filter 정보 설정
     */

    STL_TRY( qlndMakePrepareStackEvalInfo( aDataArea->mExprDataContext,
                                           sOptRowIdAccess->mPrepareFilterStack,
                                           & sExeRowIdAccess->mPreFilterEvalInfo,
                                           & QLL_DATA_PLAN( aDBCStmt ),
                                           aEnv )
             == STL_SUCCESS );

    
    /**
     * Common Info 설정
     */

    STL_DASSERT( sExeRowIdAccess->mRowIdColBlock != NULL );

    sExeRowIdAccess->mRowIdColBlock->mNext = sExeRowIdAccess->mReadColBlock;
    sExeRowIdAccess->mCommonInfo.mResultColBlock = sExeRowIdAccess->mRowIdColBlock;


    /**
     * Outer Column Value Block List 설정
     */

    STL_TRY( qlndBuildOuterColBlockList( aDataArea,
                                         sOptRowIdAccess->mOuterColumnList,
                                         & sExeRowIdAccess->mCommonInfo.mOuterColBlock,
                                         & sExeRowIdAccess->mCommonInfo.mOuterOrgColBlock,
                                         & QLL_DATA_PLAN( aDBCStmt ),
                                         aEnv )
             == STL_SUCCESS );

    
    if( sOptRowIdAccess->mNeedRowBlock == STL_TRUE )
    {
        qlnxRowBlockItem    * sRowBlockItem;

        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                    STL_SIZEOF( qlnxRowBlockList ),
                                    (void **) & sExeRowIdAccess->mCommonInfo.mRowBlockList,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                    STL_SIZEOF( qlnxRowBlockItem ),
                                    (void **) & sRowBlockItem,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sRowBlockItem->mLeafOptNode = aOptNode;
        sRowBlockItem->mRowBlock = sExeRowIdAccess->mFetchInfo.mRowBlock;
        sRowBlockItem->mOrgRowBlock = NULL;
        sRowBlockItem->mIsLeftTableRowBlock = STL_TRUE;
        sRowBlockItem->mNext = NULL;

        sExeRowIdAccess->mCommonInfo.mRowBlockList->mCount = 1;
        sExeRowIdAccess->mCommonInfo.mRowBlockList->mHead = sRowBlockItem;
        sExeRowIdAccess->mCommonInfo.mRowBlockList->mTail = sRowBlockItem;
    }
    else
    {
        sExeRowIdAccess->mCommonInfo.mRowBlockList = NULL;
    }

    
    /**
     * Common Execution Node 정보 설정
     */

    sExecNode->mNodeType  = QLL_PLAN_NODE_ROWID_ACCESS_TYPE;
    sExecNode->mOptNode   = aOptNode;
    sExecNode->mExecNode  = sExeRowIdAccess;

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
