/*******************************************************************************
 * qlndSortMergeJoin.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlndSortMergeJoin.c 10973 2014-01-16 07:17:19Z bsyou $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlndSortMergeJoin.c
 * @brief SQL Processor Layer Data Optimization Node - SORT MERGE JOIN
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnd
 * @{
 */


/**
 * @brief SORT MERGE JOIN Execution node에 대한 Data 정보를 구축한다.
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
 */
stlStatus qlndDataOptimizeSortMergeJoin( smlTransId              aTransID,
                                         qllDBCStmt            * aDBCStmt,
                                         qllStatement          * aSQLStmt,
                                         qllOptimizationNode   * aOptNode,
                                         qllDataArea           * aDataArea,
                                         qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode               = NULL;
    qllExecutionNode      * sSortInstantExecNode    = NULL;
    
    qllOptimizationNode   * sInnerNode              = NULL;
    qllOptimizationNode   * sOuterNode              = NULL;
    qllOptimizationNode   * sChildNode              = NULL;

    qlnoInstant           * sOptInnerSortInstant    = NULL;
    qlnoInstant           * sOptOuterSortInstant    = NULL;

    qlnxInstant           * sExeInnerSortInstant    = NULL;
    qlnxInstant           * sExeOuterSortInstant    = NULL;

    qlnoSortMergeJoin     * sOptSortMergeJoin       = NULL;
    qlnxSortMergeJoin     * sExeSortMergeJoin       = NULL;

    qlnxCacheBlockInfo    * sCacheBlockInfo         = NULL;

    stlInt64              * sLeftCacheBlockIdxPtr   = NULL;
    stlInt64              * sRightCacheBlockIdxPtr  = NULL;

    stlInt32                sIdx;

    qlvRefExprItem        * sJoinColItem            = NULL;

    qlvRefExprItem        * sLeftSortKeyItem        = NULL;
    qlvRefExprItem        * sRightSortKeyItem       = NULL;
    qloDBType            ** sLeftSortKeyDBTypePtrArr    = NULL;
    qloDBType            ** sRightSortKeyDBTypePtrArr   = NULL;

    knlValueBlockList     * sSrcValueBlockList      = NULL;
    knlValueBlockList     * sDstValueBlockList      = NULL;
    qlnxValueBlockList    * sValueBlockList         = NULL;
    qlnxValueBlockItem    * sValueBlockItem         = NULL;

//    qllOptimizationNode   * sOptQueryNode           = NULL;

    knlExprContext            * sContexts               = NULL;
    qlvRefExprItem            * sOuterColumnItem        = NULL;
    qlvInitExpression         * sOrgExpr                = NULL;
    qlnxOuterColumnBlockList  * sOuterColumnBlockList   = NULL;
    qlnxOuterColumnBlockItem  * sOuterColumnBlockItem   = NULL;

    dtlDataType                 sOuterType;
    dtlDataType                 sInnerType;
    qlnxSortKeyCompareItem    * sSortKeyCompareItem     = NULL;


#define ALLOC_AND_INIT_ROW_BLOCK_ITEM( aRowBlockItem,                               \
                                       aLeafOptNode,                                \
                                       aOrgRowBlock,                                \
                                       aIsLeftTableRowBlock )                       \
    {                                                                               \
        /* Row Block Item 공간 할당 */                                              \
        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),                    \
                                    STL_SIZEOF( qlnxRowBlockItem ),                 \
                                    (void **) & (aRowBlockItem),                    \
                                    KNL_ENV(aEnv) )                                 \
                 == STL_SUCCESS );                                                  \
                                                                                    \
        /* Row Block 공간 할당 */                                                   \
        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),                    \
                                    STL_SIZEOF( smlRowBlock ),                      \
                                    (void **) & (aRowBlockItem)->mRowBlock,         \
                                    KNL_ENV(aEnv) )                                 \
                 == STL_SUCCESS );                                                  \
                                                                                    \
        /* Row Block  초기화 */                                                     \
        STL_TRY( smlInitRowBlock( (aRowBlockItem)->mRowBlock,                       \
                                  aDataArea->mBlockAllocCnt,                        \
                                  & QLL_DATA_PLAN( aDBCStmt ),                      \
                                  SML_ENV(aEnv) )                                   \
                 == STL_SUCCESS );                                                  \
                                                                                    \
        /* Optimization Node 및 Original Row Block, Left Table Row Block 설정 */    \
        (aRowBlockItem)->mLeafOptNode = (aLeafOptNode);                             \
        (aRowBlockItem)->mOrgRowBlock = (aOrgRowBlock);                             \
        (aRowBlockItem)->mIsLeftTableRowBlock = (aIsLeftTableRowBlock);             \
        (aRowBlockItem)->mNext = NULL;                                              \
    }

#define ADD_ROW_BLOCK_ITEM_TO_ROW_BLOCK_LIST( aRowBlockList, aRowBlockItem )        \
    {                                                                               \
        if( (aRowBlockList)->mCount == 0 )                                          \
        {                                                                           \
            (aRowBlockList)->mHead = (aRowBlockItem);                               \
            (aRowBlockList)->mTail = (aRowBlockItem);                               \
        }                                                                           \
        else                                                                        \
        {                                                                           \
            (aRowBlockList)->mTail->mNext = (aRowBlockItem);                        \
            (aRowBlockList)->mTail = (aRowBlockItem);                               \
        }                                                                           \
        (aRowBlockList)->mCount += 1;                                               \
    }


    /****************************************************************
     * Paramter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_MERGE_JOIN_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * 준비작업
     ****************************************************************/

    /**
     * SORT MERGE JOIN Optimization Node 획득
     */

    sOptSortMergeJoin = (qlnoSortMergeJoin *) aOptNode->mOptNode;


    /**
     * 하위 node에 대한 Data Optimize 수행
     */

    QLND_DATA_OPTIMIZE( aTransID,
                        aDBCStmt,
                        aSQLStmt,
                        sOptSortMergeJoin->mLeftChildNode,
                        aDataArea,
                        aEnv );

    QLND_DATA_OPTIMIZE( aTransID,
                        aDBCStmt,
                        aSQLStmt,
                        sOptSortMergeJoin->mRightChildNode,
                        aDataArea,
                        aEnv );


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea,
                                        QLL_GET_OPT_NODE_IDX( aOptNode ) );

 
    /**
     * 해당 노드를 포함하는 Query Expression Optimization Node 획득
     */

//    sOptQueryNode = aOptNode->mOptCurQueryNode;


    /**
     * SORT MERGE JOIN Execution Node 할당
     */

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlnxSortMergeJoin ),
                                (void **) & sExeSortMergeJoin,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sExeSortMergeJoin, 0x00, STL_SIZEOF( qlnxSortMergeJoin ) );


    /**
     * Iterator를 위한 Read Value Block List 구성
     */

    STL_DASSERT( sOptSortMergeJoin->mJoinColList != NULL );
    if( sOptSortMergeJoin->mJoinColList->mCount == 0 )
    {
        sExeSortMergeJoin->mJoinedColBlock = NULL;
    }
    else
    {
        STL_TRY( qlndBuildTableColBlockList( aDataArea,
                                             sOptSortMergeJoin->mJoinColList,
                                             STL_TRUE,
                                             & sExeSortMergeJoin->mJoinedColBlock,
                                             & QLL_DATA_PLAN( aDBCStmt ),
                                             aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Inner Table Node와 Outer Table Node의 Idx 설정
     */

    switch( sOptSortMergeJoin->mJoinType )
    {
        case QLV_JOIN_TYPE_NONE:
        case QLV_JOIN_TYPE_CROSS:
        case QLV_JOIN_TYPE_INNER:
        case QLV_JOIN_TYPE_LEFT_OUTER:
        case QLV_JOIN_TYPE_FULL_OUTER:
        case QLV_JOIN_TYPE_LEFT_SEMI:
        case QLV_JOIN_TYPE_LEFT_ANTI_SEMI:
        case QLV_JOIN_TYPE_LEFT_ANTI_SEMI_NA:
        case QLV_JOIN_TYPE_INVERTED_LEFT_SEMI:
            sInnerNode = sOptSortMergeJoin->mRightChildNode;
            sOuterNode = sOptSortMergeJoin->mLeftChildNode;

            sLeftCacheBlockIdxPtr =
                &(sExeSortMergeJoin->mOuterCacheBlockInfo.mCurrBlockIdx);
            sRightCacheBlockIdxPtr =
                &(sExeSortMergeJoin->mInnerCacheBlockInfo.mCurrBlockIdx);
            break;
        /* case QLV_JOIN_TYPE_RIGHT_OUTER: */
        /* case QLV_JOIN_TYPE_RIGHT_SEMI: */
        /* case QLV_JOIN_TYPE_RIGHT_ANTI_SEMI: */
        /* case QLV_JOIN_TYPE_RIGHT_ANTI_SEMI_NA: */
        /* case QLV_JOIN_TYPE_INVERTED_RIGHT_SEMI: */
        default:
            STL_DASSERT( 0 );
            break;
    }

    sChildNode = sInnerNode;
    while( STL_TRUE )
    {
        if( sChildNode->mType == QLL_PLAN_NODE_SUB_QUERY_LIST_TYPE )
        {
            sChildNode = ((qlnoSubQueryList *) sChildNode->mOptNode)->mChildNode;
        }
        else if( sChildNode->mType == QLL_PLAN_NODE_SUB_QUERY_FILTER_TYPE ) 	 	 
        { 	 	 
            sChildNode = ((qlnoSubQueryFilter *) sChildNode->mOptNode)->mChildNode; 	 	 
            STL_DASSERT( sChildNode->mType == QLL_PLAN_NODE_SUB_QUERY_LIST_TYPE );
        }
        else
        {
            break;
        }
    }

    if( sChildNode->mType == QLL_PLAN_NODE_INDEX_ACCESS_TYPE )
    {
        sExeInnerSortInstant = NULL;
        sOptInnerSortInstant = NULL;
    }
    else
    {
        sSortInstantExecNode = QLL_GET_EXECUTION_NODE( aDataArea,
                                                       QLL_GET_OPT_NODE_IDX( sChildNode ) );
        sExeInnerSortInstant = (qlnxInstant *) sSortInstantExecNode->mExecNode;
        sOptInnerSortInstant = (qlnoInstant *) sSortInstantExecNode->mOptNode->mOptNode;
    }


    sChildNode = sOuterNode;
    while( STL_TRUE )
    {
        if( sChildNode->mType == QLL_PLAN_NODE_SUB_QUERY_LIST_TYPE )
        {
            sChildNode = ((qlnoSubQueryList *) sChildNode->mOptNode)->mChildNode;
        }
        else if( sChildNode->mType == QLL_PLAN_NODE_SUB_QUERY_FILTER_TYPE ) 	 	 
        { 	 	 
            sChildNode = ((qlnoSubQueryFilter *) sChildNode->mOptNode)->mChildNode; 	 	 
            STL_DASSERT( sChildNode->mType == QLL_PLAN_NODE_SUB_QUERY_LIST_TYPE );
        }
        else
        {
            break;
        }
    }

    if( sChildNode->mType == QLL_PLAN_NODE_INDEX_ACCESS_TYPE )
    {
        sExeOuterSortInstant = NULL;
        sOptOuterSortInstant = NULL;
    }
    else
    {
        sSortInstantExecNode = QLL_GET_EXECUTION_NODE( aDataArea,
                                                       QLL_GET_OPT_NODE_IDX( sChildNode ) );
        sExeOuterSortInstant = (qlnxInstant *) sSortInstantExecNode->mExecNode;
        sOptOuterSortInstant = (qlnoInstant *) sSortInstantExecNode->mOptNode->mOptNode;
    }


    /****************************************************************
     * Cache Block Info 정보 설정
     ****************************************************************/

    /**
     * Outer(Left) Cache Block Info 정보 구성
     */

    sCacheBlockInfo = &(sExeSortMergeJoin->mOuterCacheBlockInfo);
    QLNX_RESET_CACHE_BLOCK_INFO( sCacheBlockInfo );
    sCacheBlockInfo->mNodeIdx = sOuterNode->mIdx;


    /**
     * Inner(Right) Cache Block Info 정보 구성
     */

    sCacheBlockInfo = &(sExeSortMergeJoin->mInnerCacheBlockInfo);
    QLNX_RESET_CACHE_BLOCK_INFO( sCacheBlockInfo );
    sCacheBlockInfo->mNodeIdx = sInnerNode->mIdx;


    /****************************************************************
     * Row Block 정보 설정
     ****************************************************************/

    /**
     * Row Block List 초기화
     */

    sExeSortMergeJoin->mRowBlockList.mCount = 0;
    sExeSortMergeJoin->mRowBlockList.mHead = NULL;
    sExeSortMergeJoin->mRowBlockList.mTail = NULL;


    if( sOptSortMergeJoin->mNeedRowBlock == STL_TRUE )
    {
        STL_TRY( qlndSetRowBlockOnJoinNode(
                     &QLL_DATA_PLAN( aDBCStmt ),
                     aDataArea->mBlockAllocCnt,
                     ((qlnxCommonInfo*)(aDataArea->mExecNodeList[sOuterNode->mIdx].mExecNode))->mRowBlockList,
                     ((qlnxCommonInfo*)(aDataArea->mExecNodeList[sInnerNode->mIdx].mExecNode))->mRowBlockList,
                     &(sExeSortMergeJoin->mRowBlockList),
                     aEnv )
                 == STL_SUCCESS );
    }


    /****************************************************************
     * Value Block List 정보 설정
     ****************************************************************/

    /**
     * Value Block List 초기화
     */

    sValueBlockList = &(sExeSortMergeJoin->mValueBlockList);
    sValueBlockList->mCount = 0;
    sValueBlockList->mHead = NULL;
    sValueBlockList->mTail = NULL;

    if( sOptSortMergeJoin->mJoinColList->mCount > 0 )
    {
        sJoinColItem = sOptSortMergeJoin->mJoinColList->mHead;
        sDstValueBlockList = sExeSortMergeJoin->mJoinedColBlock;
        sContexts = aDataArea->mExprDataContext->mContexts;

        while( sJoinColItem != NULL )
        {
            /* Value Block Item 공간 할당 */
            STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                        STL_SIZEOF( qlnxValueBlockItem ),
                                        (void **) & sValueBlockItem,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            /* Join Column의 Value Block 및 Block Idx 연결 */
            sValueBlockItem->mDstValueBlock = 
                sContexts[sJoinColItem->mExprPtr->mOffset].mInfo.mValueInfo;
            sValueBlockItem->mDstBlockIdx = &(sExeSortMergeJoin->mCurrBlockIdx);

            /* Source Column의 Data Value 및 Block Idx 연결 */
            sOrgExpr = sJoinColItem->mExprPtr->mExpr.mInnerColumn->mOrgExpr;
            switch( sOrgExpr->mType )
            {
                case QLV_EXPR_TYPE_COLUMN:
                case QLV_EXPR_TYPE_INNER_COLUMN:
                case QLV_EXPR_TYPE_ROWID_COLUMN:
                    sValueBlockItem->mSrcValueBlock =
                        sContexts[sOrgExpr->mOffset].mInfo.mValueInfo;
                    if( sJoinColItem->mIsLeft == STL_TRUE )
                    {
                        sValueBlockItem->mSrcBlockIdx = sLeftCacheBlockIdxPtr;
                    }
                    else
                    {
                        sValueBlockItem->mSrcBlockIdx = sRightCacheBlockIdxPtr;
                    }

                    STL_TRY( qloDataSetInternalCastInfoBetweenValueBlock(
                                 aSQLStmt,
                                 sValueBlockItem->mSrcValueBlock,
                                 sValueBlockItem->mDstValueBlock,
                                 &(sValueBlockItem->mCastFuncInfo),
                                 & QLL_DATA_PLAN( aDBCStmt ),
                                 aEnv )
                             == STL_SUCCESS );

                    break;
                default:
                    STL_DASSERT( 0 );
                    break;
            }

            sValueBlockItem->mDataValueSize = KNL_GET_BLOCK_DATA_BUFFER_SIZE( sDstValueBlockList );
            sValueBlockItem->mNext = NULL;

            /* Value Block Item을 Value Block List에 등록 */
            if( sValueBlockList->mCount == 0 )
            {
                sValueBlockList->mHead = sValueBlockItem;
                sValueBlockList->mTail = sValueBlockItem;
            }
            else
            {
                sValueBlockList->mTail->mNext = sValueBlockItem;
                sValueBlockList->mTail = sValueBlockItem;
            }
            sValueBlockList->mCount += 1;

            sJoinColItem = sJoinColItem->mNext;
            sDstValueBlockList = sDstValueBlockList->mNext;
        }
    }


    /****************************************************************
     * Equal Join인 경우 Sort Key에 대한 physical compare list 만들기
     ****************************************************************/

    sExeSortMergeJoin->mSortKeyCompareList.mCount = 0;
    sExeSortMergeJoin->mSortKeyCompareList.mHead = NULL;
    sExeSortMergeJoin->mSortKeyCompareList.mTail = NULL;

    sLeftSortKeyItem = sOptSortMergeJoin->mLeftSortKeyList->mHead;
    sRightSortKeyItem = sOptSortMergeJoin->mRightSortKeyList->mHead;
    STL_DASSERT( (sLeftSortKeyItem != NULL) && (sRightSortKeyItem != NULL) );

    sLeftSortKeyDBTypePtrArr = sOptSortMergeJoin->mLeftSortKeyDBTypePtrArr;
    sRightSortKeyDBTypePtrArr = sOptSortMergeJoin->mRightSortKeyDBTypePtrArr;
    sIdx = 0;

    while( sLeftSortKeyItem != NULL )
    {
        STL_DASSERT( sRightSortKeyItem != NULL );

        /* Sort Key Compare Item 할당 */
        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                    STL_SIZEOF( qlnxSortKeyCompareItem ),
                                    (void **) & (sSortKeyCompareItem),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Outer Data Value 셋팅 */
        STL_TRY( qlndFindColumnBlockList( aDataArea,
                                          aDataArea->mReadColumnBlock,
                                          sLeftSortKeyItem->mExprPtr,
                                          & QLL_DATA_PLAN( aDBCStmt ),
                                          & sSrcValueBlockList,
                                          aEnv )
                 == STL_SUCCESS );

        sSortKeyCompareItem->mOuterValueBlock = sSrcValueBlockList->mValueBlock;

        /* Outer Sort Key의 Cast 여부 설정*/
        if( sLeftSortKeyDBTypePtrArr[sIdx]->mIsNeedCast == STL_TRUE )
        {
            sSortKeyCompareItem->mNeedOuterCast = STL_TRUE;
            STL_TRY( qloDataSetInternalCastInfo( aSQLStmt,
                                                 &QLL_DATA_PLAN( aDBCStmt ),
                                                 sLeftSortKeyDBTypePtrArr[sIdx],
                                                 &(sSortKeyCompareItem->mOuterCastFuncInfo),
                                                 STL_TRUE,  /* aIsAllocCastFuncInfo */
                                                 STL_TRUE,  /* aIsAllocCastResultBuf */
                                                 aEnv )
                     == STL_SUCCESS );
            sOuterType = sLeftSortKeyDBTypePtrArr[sIdx]->mDBType;
        }
            
        else
        {
            sSortKeyCompareItem->mNeedOuterCast = STL_FALSE;
            sSortKeyCompareItem->mOuterCastFuncInfo = NULL;
            sOuterType = sSortKeyCompareItem->mOuterValueBlock->mDataValue[0].mType;
        }

        /* Inner Data Value 셋팅 */
        STL_TRY( qlndFindColumnBlockList( aDataArea,
                                          aDataArea->mReadColumnBlock,
                                          sRightSortKeyItem->mExprPtr,
                                          & QLL_DATA_PLAN( aDBCStmt ),
                                          & sSrcValueBlockList,
                                          aEnv )
                 == STL_SUCCESS );

        sSortKeyCompareItem->mInnerValueBlock = sSrcValueBlockList->mValueBlock;

        /* Inner Sort Key의 Cast 여부 설정*/
        if( sRightSortKeyDBTypePtrArr[sIdx]->mIsNeedCast == STL_TRUE )
        {
            sSortKeyCompareItem->mNeedInnerCast = STL_TRUE;
            STL_TRY( qloDataSetInternalCastInfo( aSQLStmt,
                                                 &QLL_DATA_PLAN( aDBCStmt ),
                                                 sRightSortKeyDBTypePtrArr[sIdx],
                                                 &(sSortKeyCompareItem->mInnerCastFuncInfo),
                                                 STL_TRUE,  /* aIsAllocCastFuncInfo */
                                                 STL_TRUE,  /* aIsAllocCastResultBuf */
                                                 aEnv )
                     == STL_SUCCESS );
            sInnerType = sRightSortKeyDBTypePtrArr[sIdx]->mDBType;
        }
        else
        {
            sSortKeyCompareItem->mNeedInnerCast = STL_FALSE;
            sSortKeyCompareItem->mInnerCastFuncInfo = NULL;
            sInnerType = sSortKeyCompareItem->mInnerValueBlock->mDataValue[0].mType;
        }

        /* Outer Data Value와 Inner Data Value의 비교 함수 찾기 */
        sSortKeyCompareItem->mCompFunc =
            dtlPhysicalCompareFuncList[sOuterType][sInnerType];
        STL_DASSERT( sSortKeyCompareItem->mCompFunc != NULL );

        /* Outer Data Value끼리의 비교 함수 찾기 */
        sSortKeyCompareItem->mOuterValueCompFunc =
            dtlPhysicalCompareFuncList[sOuterType][sOuterType];
        STL_DASSERT( sSortKeyCompareItem->mOuterValueCompFunc != NULL );

        sSortKeyCompareItem->mNext = NULL;

        /* Sort Key Compare Item을 List에 등록 */
        if( sExeSortMergeJoin->mSortKeyCompareList.mCount == 0 )
        {
            sExeSortMergeJoin->mSortKeyCompareList.mHead = sSortKeyCompareItem;
            sExeSortMergeJoin->mSortKeyCompareList.mTail = sSortKeyCompareItem;
        }
        else
        {
            sExeSortMergeJoin->mSortKeyCompareList.mTail->mNext = sSortKeyCompareItem;
            sExeSortMergeJoin->mSortKeyCompareList.mTail = sSortKeyCompareItem;
        }
        sExeSortMergeJoin->mSortKeyCompareList.mCount++;

        sIdx++;
        sLeftSortKeyItem = sLeftSortKeyItem->mNext;
        sRightSortKeyItem = sRightSortKeyItem->mNext;
    }


    /****************************************************************
     * Outer Column 정보 설정
     ****************************************************************/

    /**
     * Left Outer Column
     */

    sOuterColumnBlockList = &(sExeSortMergeJoin->mLeftOuterColumnBlockList);
    sOuterColumnBlockList->mCount = 0;
    sOuterColumnBlockList->mHead = NULL;
    sOuterColumnBlockList->mTail = NULL;

    sOuterColumnItem = sOptSortMergeJoin->mLeftOuterColumnList->mHead;
    sContexts = aDataArea->mExprDataContext->mContexts;
    if( sOptOuterSortInstant != NULL )
    {
        sSrcValueBlockList = sExeOuterSortInstant->mKeyColBlock;
    }
    else
    {
        sSrcValueBlockList = NULL;
    }
    while( sOuterColumnItem != NULL )
    {
        /* Outer Column Value Block Item 생성 */
        STL_TRY( knlAllocRegionMem( &QLL_DATA_PLAN( aDBCStmt ),
                                    STL_SIZEOF( qlnxOuterColumnBlockItem ),
                                    (void**)&sOuterColumnBlockItem,
                                    KNL_ENV( aEnv ) )
                 == STL_SUCCESS );

        sOuterColumnBlockItem->mNext = NULL;

        /* Outer Column의 Data Value 연결 */
        sOuterColumnBlockItem->mDstDataValue = 
            KNL_GET_VALUE_BLOCK_DATA_VALUE(
                sContexts[sOuterColumnItem->mExprPtr->mOffset].mInfo.mValueInfo,
                0 );

        /* Outer Column의 Original Information 저장 */
        QLNX_BACKUP_OUTER_COLUMN_DATA_VALUE_INFO( sOuterColumnBlockItem );

        /* Outer Column의 Cast 정보 셋팅 */
        sOuterColumnBlockItem->mCast =
            sContexts[sOuterColumnItem->mExprPtr->mOffset].mCast;
        sContexts[sOuterColumnItem->mExprPtr->mOffset].mCast = NULL;

        /* Source Column의 Data Value 및 Block Idx 연결 */
        /* Outer Column은 Original Expr로 Column 계열을 가지고 있다. */
        sOrgExpr = sOuterColumnItem->mExprPtr->mExpr.mOuterColumn->mOrgExpr;
        switch( sOrgExpr->mType )
        {
            case QLV_EXPR_TYPE_COLUMN:
            case QLV_EXPR_TYPE_INNER_COLUMN:
            case QLV_EXPR_TYPE_ROWID_COLUMN:
                sOuterColumnBlockItem->mSrcValueBlock =
                    sContexts[sOrgExpr->mOffset].mInfo.mValueInfo;
                sOuterColumnBlockItem->mSrcBlockIdx = sLeftCacheBlockIdxPtr;
                break;
            default:
                STL_DASSERT( 0 );
                break;
        }

        /* Outer Column Value Block List에 등록 */
        if( sOuterColumnBlockList->mCount == 0 )
        {
            sOuterColumnBlockList->mHead = sOuterColumnBlockItem;
            sOuterColumnBlockList->mTail = sOuterColumnBlockItem;
        }
        else
        {
            sOuterColumnBlockList->mTail->mNext = sOuterColumnBlockItem;
            sOuterColumnBlockList->mTail = sOuterColumnBlockItem;
        }
        sOuterColumnBlockList->mCount++;

        sOuterColumnItem = sOuterColumnItem->mNext;
    }


    /**
     * Right Outer Column
     */

    sOuterColumnBlockList = &(sExeSortMergeJoin->mRightOuterColumnBlockList);
    sOuterColumnBlockList->mCount = 0;
    sOuterColumnBlockList->mHead = NULL;
    sOuterColumnBlockList->mTail = NULL;

    sOuterColumnItem = sOptSortMergeJoin->mRightOuterColumnList->mHead;
    sContexts = aDataArea->mExprDataContext->mContexts;
    if( sOptInnerSortInstant != NULL )
    {
        sSrcValueBlockList = sExeInnerSortInstant->mKeyColBlock;
    }
    else
    {
        sSrcValueBlockList = NULL;
    }
    while( sOuterColumnItem != NULL )
    {
        /* Outer Column Value Block Item 생성 */
        STL_TRY( knlAllocRegionMem( &QLL_DATA_PLAN( aDBCStmt ),
                                    STL_SIZEOF( qlnxOuterColumnBlockItem ),
                                    (void**)&sOuterColumnBlockItem,
                                    KNL_ENV( aEnv ) )
                 == STL_SUCCESS );

        sOuterColumnBlockItem->mNext = NULL;

        /* Outer Column의 Data Value 연결 */
        sOuterColumnBlockItem->mDstDataValue = 
            KNL_GET_VALUE_BLOCK_DATA_VALUE(
                sContexts[sOuterColumnItem->mExprPtr->mOffset].mInfo.mValueInfo,
                0 );

        /* Outer Column의 Original Information 저장 */
        QLNX_BACKUP_OUTER_COLUMN_DATA_VALUE_INFO( sOuterColumnBlockItem );

        /* Outer Column의 Cast 정보 셋팅 */
        sOuterColumnBlockItem->mCast =
            sContexts[sOuterColumnItem->mExprPtr->mOffset].mCast;
        sContexts[sOuterColumnItem->mExprPtr->mOffset].mCast= NULL;

        /* Source Column의 Data Value 및 Block Idx 연결 */
        /* Outer Column은 Original Expr로 Column 계열을 가지고 있다. */
        sOrgExpr = sOuterColumnItem->mExprPtr->mExpr.mOuterColumn->mOrgExpr;
        switch( sOrgExpr->mType )
        {
            case QLV_EXPR_TYPE_COLUMN:
            case QLV_EXPR_TYPE_INNER_COLUMN:
            case QLV_EXPR_TYPE_ROWID_COLUMN:
                sOuterColumnBlockItem->mSrcValueBlock =
                    sContexts[sOrgExpr->mOffset].mInfo.mValueInfo;
                sOuterColumnBlockItem->mSrcBlockIdx = sRightCacheBlockIdxPtr;
                break;
            default:
                STL_DASSERT( 0 );
                break;
        }

        /* Outer Column Value Block List에 등록 */
        if( sOuterColumnBlockList->mCount == 0 )
        {
            sOuterColumnBlockList->mHead = sOuterColumnBlockItem;
            sOuterColumnBlockList->mTail = sOuterColumnBlockItem;
        }
        else
        {
            sOuterColumnBlockList->mTail->mNext = sOuterColumnBlockItem;
            sOuterColumnBlockList->mTail = sOuterColumnBlockItem;
        }
        sOuterColumnBlockList->mCount++;

        sOuterColumnItem = sOuterColumnItem->mNext;
    }


    /****************************************************************
     * 기타 정보 설정
     ****************************************************************/

    /**
     * Where Condition Evaluate Info 설정
     */

    if( sOptSortMergeJoin->mWhereFilterPred != NULL )
    {
        sExeSortMergeJoin->mWhereExprEvalInfo.mExprStack      =
            KNL_PRED_GET_FILTER( sOptSortMergeJoin->mWhereFilterPred );
        sExeSortMergeJoin->mWhereExprEvalInfo.mContext        =
            aDataArea->mExprDataContext;
        sExeSortMergeJoin->mWhereExprEvalInfo.mResultBlock    =
            aDataArea->mPredResultBlock;
        
        sExeSortMergeJoin->mWhereExprEvalInfo.mBlockIdx   = 0;
        sExeSortMergeJoin->mWhereExprEvalInfo.mBlockCount = 0;

        sExeSortMergeJoin->mHasWhereCondition = STL_TRUE;
    }
    else
    {
        sExeSortMergeJoin->mHasWhereCondition = STL_FALSE;
    }


    /**
     * Join Condition Evaluate Info 설정
     */

    if( sOptSortMergeJoin->mJoinFilterPred != NULL )
    {
        sExeSortMergeJoin->mJoinExprEvalInfo.mExprStack       =
            KNL_PRED_GET_FILTER( sOptSortMergeJoin->mJoinFilterPred );
        sExeSortMergeJoin->mJoinExprEvalInfo.mContext         =
            aDataArea->mExprDataContext;
        sExeSortMergeJoin->mJoinExprEvalInfo.mResultBlock    =
            aDataArea->mPredResultBlock;
        
        sExeSortMergeJoin->mJoinExprEvalInfo.mBlockIdx   = 0;
        sExeSortMergeJoin->mJoinExprEvalInfo.mBlockCount = 0;

        sExeSortMergeJoin->mHasJoinCondition = STL_TRUE;
    }
    else
    {
        sExeSortMergeJoin->mHasJoinCondition = STL_FALSE;
    }


    /* First Fetch 여부 */
    sExeSortMergeJoin->mIsFirstFetch = STL_TRUE;

    /* EOF (더이상 Fetch할 것이 없는지) 여부 */
    sExeSortMergeJoin->mIsEOF = STL_FALSE;

    /* Group EOF (더이상 Group Fetch할 것이 없는지) 여부 */
    sExeSortMergeJoin->mIsGroupEOF = STL_FALSE;

    /* Join Type 셋팅 */
    sExeSortMergeJoin->mJoinType = sOptSortMergeJoin->mJoinType;


    /**
     * Right Node의 Flag를 Setting하기 위한 Value Block 생성
     * (Outer Join이고 Right Node가 Instant Node인 경우에 한함)
     */

    if( ( (sOptSortMergeJoin->mJoinType == QLV_JOIN_TYPE_LEFT_OUTER) ||
          (sOptSortMergeJoin->mJoinType == QLV_JOIN_TYPE_RIGHT_OUTER) ||
          (sOptSortMergeJoin->mJoinType == QLV_JOIN_TYPE_FULL_OUTER) ) &&
        ( sOptInnerSortInstant != NULL ) )
    {
        STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                    &(sExeSortMergeJoin->mSetRightFlagBlock),
                                    aDataArea->mBlockAllocCnt,
                                    STL_TRUE, /* IsSepBuff */
                                    STL_LAYER_SQL_PROCESSOR,
                                    0, /* TableID */
                                    0,
                                    DTL_TYPE_BINARY,
                                    STL_SIZEOF( stlInt8 ),
                                    DTL_SCALE_NA,
                                    DTL_STRING_LENGTH_UNIT_NA,
                                    DTL_INTERVAL_INDICATOR_NA,
                                    & QLL_DATA_PLAN( aDBCStmt ),
                                    aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Common Info 설정
     */

    sExeSortMergeJoin->mCommonInfo.mResultColBlock   = sExeSortMergeJoin->mJoinedColBlock;

    if( sOptSortMergeJoin->mNeedRowBlock == STL_TRUE )
    {
        sExeSortMergeJoin->mCommonInfo.mRowBlockList = &(sExeSortMergeJoin->mRowBlockList);
    }
    else
    {
        sExeSortMergeJoin->mCommonInfo.mRowBlockList = NULL;
    }


    /**
     * Outer Column Value Block List 설정
     */

    STL_TRY( qlndBuildOuterColBlockList( aDataArea,
                                         sOptSortMergeJoin->mOuterColumnList,
                                         & sExeSortMergeJoin->mCommonInfo.mOuterColBlock,
                                         & sExeSortMergeJoin->mCommonInfo.mOuterOrgColBlock,
                                         & QLL_DATA_PLAN( aDBCStmt ),
                                         aEnv )
             == STL_SUCCESS );


    /**
     * Common Execution Node 정보 설정
     */

    sExecNode->mNodeType  = QLL_PLAN_NODE_MERGE_JOIN_TYPE;
    sExecNode->mOptNode   = aOptNode;
    sExecNode->mExecNode  = sExeSortMergeJoin;

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

