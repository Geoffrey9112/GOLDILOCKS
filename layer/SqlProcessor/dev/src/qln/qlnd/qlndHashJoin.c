/*******************************************************************************
 * qlndHashJoin.c
 *
 * Copyright (c) 2013, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlndHashJoin.c 11114 2014-02-05 02:47:56Z bsyou $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlndHashJoin.c
 * @brief SQL Processor Layer Data Optimization Node - HASH JOIN
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnd
 * @{
 */


/****************************************************************************
 * Node Optimization Function
 ***************************************************************************/


/**
 * @brief HASH JOIN Execution node에 대한 Data 정보를 구축한다.
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
stlStatus qlndDataOptimizeHashJoin( smlTransId              aTransID,
                                    qllDBCStmt            * aDBCStmt,
                                    qllStatement          * aSQLStmt,
                                    qllOptimizationNode   * aOptNode,
                                    qllDataArea           * aDataArea,
                                    qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode               = NULL;
    qllExecutionNode      * sHashInstantExecNode    = NULL;
    
    qllOptimizationNode   * sInnerNode              = 0;
    qllOptimizationNode   * sOuterNode              = 0;

    qlnxInstant           * sExeInnerHashInstant    = NULL;

    qlnoHashJoin          * sOptHashJoin            = NULL;
    qlnxHashJoin          * sExeHashJoin            = NULL;

    qlnoInstant           * sOptHashInstant         = NULL;

    qlnxCacheBlockInfo    * sCacheBlockInfo         = NULL;

    stlInt64              * sLeftCacheBlockIdxPtr   = NULL;
    stlInt64              * sRightCacheBlockIdxPtr  = NULL;

    qlvRefExprItem        * sJoinColItem            = NULL;

    knlValueBlockList     * sDstValueBlockList      = NULL;
    qlnxValueBlockList    * sValueBlockList         = NULL;
    qlnxValueBlockItem    * sValueBlockItem         = NULL;

//    qllOptimizationNode   * sOptQueryNode           = NULL;

    knlExprContext            * sContexts               = NULL;
    qlvRefExprItem            * sOuterColumnItem        = NULL;
    qlvInitExpression         * sOrgExpr                = NULL;
    qlnxOuterColumnBlockList  * sOuterColumnBlockList   = NULL;
    qlnxOuterColumnBlockItem  * sOuterColumnBlockItem   = NULL;

    
    smlBlockJoinFetchInfo * sJoinFetchInfo = NULL;
    
    knlBlockRelationList  * sRelationBlockList = NULL;
    knlBlockRelationList  * sRelationBlockLast = NULL;
    knlBlockRelationList  * sRelationBlockItem = NULL;

    knlValueBlockList     * sLeftFilterBlockList = NULL;
    knlValueBlockList     * sLeftFilterBlockLast = NULL;
    knlValueBlockList     * sLeftFilterBlockItem = NULL;

    knlValueBlockList     * sRightFilterBlockList = NULL;
    knlValueBlockList     * sRightFilterBlockLast = NULL;
    knlValueBlockList     * sRightFilterBlockItem = NULL;

    knlDataValueList      * sLeftFilterConstList = NULL;
    knlDataValueList      * sLeftFilterConstLast = NULL;
    knlDataValueList      * sLeftFilterConstItem = NULL;

    knlDataValueList      * sRightFilterConstList = NULL;
    knlDataValueList      * sRightFilterConstLast = NULL;
    knlDataValueList      * sRightFilterConstItem = NULL;

   
    /****************************************************************
     * Paramter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_HASH_JOIN_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * 준비작업
     ****************************************************************/

    /**
     * HASH JOIN Optimization Node 획득
     */

    sOptHashJoin = (qlnoHashJoin *) aOptNode->mOptNode;


    /**
     * 하위 node에 대한 Data Optimize 수행
     */

    QLND_DATA_OPTIMIZE( aTransID,
                        aDBCStmt,
                        aSQLStmt,
                        sOptHashJoin->mLeftChildNode,
                        aDataArea,
                        aEnv );

    QLND_DATA_OPTIMIZE( aTransID,
                        aDBCStmt,
                        aSQLStmt,
                        sOptHashJoin->mRightChildNode,
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
     * HASH JOIN Execution Node 할당
     */

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlnxHashJoin ),
                                (void **) & sExeHashJoin,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sExeHashJoin, 0x00, STL_SIZEOF( qlnxHashJoin ) );


    /**
     * Iterator를 위한 Read Value Block List 구성
     */

    STL_DASSERT( sOptHashJoin->mJoinColList != NULL );
    if( sOptHashJoin->mJoinColList->mCount == 0 )
    {
        sExeHashJoin->mJoinedColBlock = NULL;
    }
    else
    {
        STL_TRY( qlndBuildTableColBlockList( aDataArea,
                                             sOptHashJoin->mJoinColList,
                                             STL_TRUE,
                                             & sExeHashJoin->mJoinedColBlock,
                                             & QLL_DATA_PLAN( aDBCStmt ),
                                             aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Inner Table Node와 Outer Table Node의 Idx 설정
     */

    switch( sOptHashJoin->mJoinType )
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
            sInnerNode = sOptHashJoin->mRightChildNode;
            sOuterNode = sOptHashJoin->mLeftChildNode;

            sLeftCacheBlockIdxPtr =
                &(sExeHashJoin->mOuterCacheBlockInfo.mCurrBlockIdx);
            sRightCacheBlockIdxPtr =
                &(sExeHashJoin->mInnerCacheBlockInfo.mCurrBlockIdx);

            sHashInstantExecNode = QLL_GET_EXECUTION_NODE( aDataArea,
                                                           QLL_GET_OPT_NODE_IDX( sInnerNode ) );
            sExeInnerHashInstant = (qlnxInstant *) sHashInstantExecNode->mExecNode;

            break;
        case QLV_JOIN_TYPE_RIGHT_OUTER:
        case QLV_JOIN_TYPE_RIGHT_SEMI:
        case QLV_JOIN_TYPE_RIGHT_ANTI_SEMI:
        case QLV_JOIN_TYPE_RIGHT_ANTI_SEMI_NA:
        case QLV_JOIN_TYPE_INVERTED_RIGHT_SEMI:
        default:
            STL_DASSERT( 0 );
            break;
    }


    /****************************************************************
     * Cache Block Info 정보 설정
     ****************************************************************/

    /**
     * Outer(Left) Cache Block Info 정보 구성
     */

    sCacheBlockInfo = &(sExeHashJoin->mOuterCacheBlockInfo);
    QLNX_RESET_CACHE_BLOCK_INFO( sCacheBlockInfo );
    sCacheBlockInfo->mNodeIdx = sOuterNode->mIdx;


    /**
     * Inner(Right) Cache Block Info 정보 구성
     */

    sCacheBlockInfo = &(sExeHashJoin->mInnerCacheBlockInfo);
    QLNX_RESET_CACHE_BLOCK_INFO( sCacheBlockInfo );
    sCacheBlockInfo->mNodeIdx = sInnerNode->mIdx;


    /****************************************************************
     * Row Block 정보 설정
     ****************************************************************/

    /**
     * Row Block List 초기화
     */

    sExeHashJoin->mRowBlockList.mCount = 0;
    sExeHashJoin->mRowBlockList.mHead = NULL;
    sExeHashJoin->mRowBlockList.mTail = NULL;

    if( sOptHashJoin->mNeedRowBlock == STL_TRUE )
    {
        STL_TRY( qlndSetRowBlockOnJoinNode(
                     &QLL_DATA_PLAN( aDBCStmt ),
                     aDataArea->mBlockAllocCnt,
                     ((qlnxCommonInfo*)(aDataArea->mExecNodeList[sOuterNode->mIdx].mExecNode))->mRowBlockList,
                     ((qlnxCommonInfo*)(aDataArea->mExecNodeList[sInnerNode->mIdx].mExecNode))->mRowBlockList,
                     &(sExeHashJoin->mRowBlockList),
                     aEnv )
                 == STL_SUCCESS );
    }


    /****************************************************************
     * Value Block List 정보 설정
     ****************************************************************/

    /**
     * Value Block List 초기화
     */

    sValueBlockList = &(sExeHashJoin->mValueBlockList);
    sValueBlockList->mCount = 0;
    sValueBlockList->mHead = NULL;
    sValueBlockList->mTail = NULL;

    if( sOptHashJoin->mJoinColList->mCount > 0 )
    {
        sJoinColItem = sOptHashJoin->mJoinColList->mHead;
        sDstValueBlockList = sExeHashJoin->mJoinedColBlock;
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
            sValueBlockItem->mDstBlockIdx = &(sExeHashJoin->mCurrBlockIdx);

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

            /************************************************************
             * Blocked Join 을 위한 Source/Destination Relationship 구성
             ************************************************************/
            
            STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                        STL_SIZEOF( knlBlockRelationList ),
                                        (void **) & sRelationBlockItem,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            sRelationBlockItem->mIsLeftSrc      = sJoinColItem->mIsLeft;
            sRelationBlockItem->mSrcBlock       = sContexts[sOrgExpr->mOffset].mInfo.mValueInfo;
            sRelationBlockItem->mDstBlock       = sDstValueBlockList->mValueBlock;
            sRelationBlockItem->mCastFuncInfo   = sValueBlockItem->mCastFuncInfo;
            sRelationBlockItem->mNext           = NULL;

            if ( sRelationBlockLast == NULL )
            {
                sRelationBlockList = sRelationBlockItem;
            }
            else
            {
                sRelationBlockLast->mNext = sRelationBlockItem;
            }
            sRelationBlockLast = sRelationBlockItem;

            
            sJoinColItem = sJoinColItem->mNext;
            sDstValueBlockList = sDstValueBlockList->mNext;
        }
    }


    /****************************************************************
     * Outer Column 정보 설정
     ****************************************************************/

    /**
     * Left Outer Column
     */

    sOuterColumnBlockList = &(sExeHashJoin->mLeftOuterColumnBlockList);
    sOuterColumnBlockList->mCount = 0;
    sOuterColumnBlockList->mHead = NULL;
    sOuterColumnBlockList->mTail = NULL;

    sOuterColumnItem = sOptHashJoin->mLeftOuterColumnList->mHead;
    sContexts = aDataArea->mExprDataContext->mContexts;
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

        /* Source Column의 Data Value 및 Block Idx 연결 */
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

        /************************************************************
         * Blocked Join 을 위한 Join Filter 를 위한 Left Block List 를 구성
         ************************************************************/
        
        STL_TRY( knlInitShareBlockFromBlock( & sLeftFilterBlockItem,
                                             sContexts[sOrgExpr->mOffset].mInfo.mValueInfo,
                                             &QLL_DATA_PLAN( aDBCStmt ),
                                             KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY( knlAllocRegionMem( &QLL_DATA_PLAN( aDBCStmt ),
                                    STL_SIZEOF( knlDataValueList ),
                                    (void**) & sLeftFilterConstItem,
                                    KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
        
        sLeftFilterConstItem->mConstVal = sOuterColumnBlockItem->mDstDataValue;
        sLeftFilterConstItem->mNext = NULL;
        
        if ( sLeftFilterBlockLast == NULL )
        {
            sLeftFilterBlockList = sLeftFilterBlockItem;
            sLeftFilterConstList = sLeftFilterConstItem;
        }
        else
        {
            sLeftFilterBlockLast->mNext = sLeftFilterBlockItem;
            sLeftFilterConstLast->mNext = sLeftFilterConstItem;
        }
        sLeftFilterBlockLast = sLeftFilterBlockItem;
        sLeftFilterConstLast = sLeftFilterConstItem;
            
        sOuterColumnItem = sOuterColumnItem->mNext;
    }


    /**
     * Right Outer Column
     */

    sOuterColumnBlockList = &(sExeHashJoin->mRightOuterColumnBlockList);
    sOuterColumnBlockList->mCount = 0;
    sOuterColumnBlockList->mHead = NULL;
    sOuterColumnBlockList->mTail = NULL;

    sOuterColumnItem = sOptHashJoin->mRightOuterColumnList->mHead;
    sContexts = aDataArea->mExprDataContext->mContexts;
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

        /* Source Column의 Data Value 및 Block Idx 연결 */
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

        /************************************************************
         * Blocked Join 을 위한 Join Filter 를 위한 Right Block List 를 구성
         ************************************************************/
        
        STL_TRY( knlInitShareBlockFromBlock( & sRightFilterBlockItem,
                                             sContexts[sOrgExpr->mOffset].mInfo.mValueInfo,
                                             &QLL_DATA_PLAN( aDBCStmt ),
                                             KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY( knlAllocRegionMem( &QLL_DATA_PLAN( aDBCStmt ),
                                    STL_SIZEOF( knlDataValueList ),
                                    (void**) & sRightFilterConstItem,
                                    KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
        
        sRightFilterConstItem->mConstVal = sOuterColumnBlockItem->mDstDataValue;
        sRightFilterConstItem->mNext = NULL;
        
        if ( sRightFilterBlockLast == NULL )
        {
            sRightFilterBlockList = sRightFilterBlockItem;
            sRightFilterConstList = sRightFilterConstItem;
        }
        else
        {
            sRightFilterBlockLast->mNext = sRightFilterBlockItem;
            sRightFilterConstLast->mNext = sRightFilterConstItem;
        }
        sRightFilterBlockLast = sRightFilterBlockItem;
        sRightFilterConstLast = sRightFilterConstItem;
            
        sOuterColumnItem = sOuterColumnItem->mNext;
    }


    /****************************************************************
     * 기타 정보 설정
     ****************************************************************/

    /**
     * Where Condition Evaluate Info 설정
     */

    if( sOptHashJoin->mWhereFilterPred != NULL )
    {
        sExeHashJoin->mWhereExprEvalInfo.mExprStack      =
            KNL_PRED_GET_FILTER( sOptHashJoin->mWhereFilterPred );
        sExeHashJoin->mWhereExprEvalInfo.mContext        =
            aDataArea->mExprDataContext;
        sExeHashJoin->mWhereExprEvalInfo.mResultBlock    =
            aDataArea->mPredResultBlock;
        
        sExeHashJoin->mWhereExprEvalInfo.mBlockIdx   = 0;
        sExeHashJoin->mWhereExprEvalInfo.mBlockCount = 0;

        sExeHashJoin->mHasWhereCondition = STL_TRUE;
    }
    else
    {
        sExeHashJoin->mHasWhereCondition = STL_FALSE;
    }


    /**
     * Join Condition Evaluate Info 설정
     */

    if( sOptHashJoin->mJoinFilterPred != NULL )
    {
        sExeHashJoin->mJoinExprEvalInfo.mExprStack       =
            KNL_PRED_GET_FILTER( sOptHashJoin->mJoinFilterPred );
        sExeHashJoin->mJoinExprEvalInfo.mContext         =
            aDataArea->mExprDataContext;
        sExeHashJoin->mJoinExprEvalInfo.mResultBlock    =
            aDataArea->mPredResultBlock;
        
        sExeHashJoin->mJoinExprEvalInfo.mBlockIdx   = 0;
        sExeHashJoin->mJoinExprEvalInfo.mBlockCount = 0;

        sExeHashJoin->mHasJoinCondition = STL_TRUE;
    }
    else
    {
        sExeHashJoin->mHasJoinCondition = STL_FALSE;
    }

    /* First Fetch 여부 */
    sExeHashJoin->mIsFirstFetch = STL_TRUE;

    /* EOF (더이상 Fetch할 것이 없는지) 여부 */
    sExeHashJoin->mIsEOF = STL_FALSE;

    /* Join Type 셋팅 */
    sExeHashJoin->mJoinType = sOptHashJoin->mJoinType;


    /**
     * Common Info 설정
     */

    sExeHashJoin->mCommonInfo.mResultColBlock   = sExeHashJoin->mJoinedColBlock;

    if( sOptHashJoin->mNeedRowBlock == STL_TRUE )
    {
        sExeHashJoin->mCommonInfo.mRowBlockList = &(sExeHashJoin->mRowBlockList);
    }
    else
    {
        sExeHashJoin->mCommonInfo.mRowBlockList = NULL;
    }


    /**
     * Outer Column Value Block List 설정
     */

    STL_TRY( qlndBuildOuterColBlockList( aDataArea,
                                         sOptHashJoin->mOuterColumnList,
                                         & sExeHashJoin->mCommonInfo.mOuterColBlock,
                                         & sExeHashJoin->mCommonInfo.mOuterOrgColBlock,
                                         & QLL_DATA_PLAN( aDBCStmt ),
                                         aEnv )
             == STL_SUCCESS );


    /**
     * Common Execution Node 정보 설정
     */

    sExecNode->mNodeType  = QLL_PLAN_NODE_HASH_JOIN_TYPE;
    sExecNode->mOptNode   = aOptNode;
    sExecNode->mExecNode  = sExeHashJoin;

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

    /*********************************************************************************
     * Blocked Join 을 위한 Join Fetch Information 구축
     *********************************************************************************/

    stlMemset( & sExeHashJoin->mBlockJoinFetchInfo, 0x00, STL_SIZEOF(smlBlockJoinFetchInfo) );
    sJoinFetchInfo = & sExeHashJoin->mBlockJoinFetchInfo;

    sJoinFetchInfo->mNullAwareComparison = STL_FALSE;
    
    /**
     * Blocked Join Type 설정
     */
    
    switch( sOptHashJoin->mJoinType )
    {
        case QLV_JOIN_TYPE_NONE:
        case QLV_JOIN_TYPE_CROSS:
        case QLV_JOIN_TYPE_INNER:
            sJoinFetchInfo->mBlockJoinType = KNL_BLOCK_JOIN_INNER_JOIN;
            break;
        case QLV_JOIN_TYPE_LEFT_OUTER:
        case QLV_JOIN_TYPE_RIGHT_OUTER:
            sJoinFetchInfo->mBlockJoinType = KNL_BLOCK_JOIN_LEFT_OUTER_JOIN;
            break;
        case QLV_JOIN_TYPE_FULL_OUTER:
            /*
             * 두 단계로 진행됨
             * - 1 phase: LEFT OUTER JOIN
             * - 2 phase: ANTI OUTER JOIN
             */
            sJoinFetchInfo->mBlockJoinType = KNL_BLOCK_JOIN_LEFT_OUTER_JOIN;
            break;
        case QLV_JOIN_TYPE_LEFT_SEMI:
        case QLV_JOIN_TYPE_RIGHT_SEMI:
            sJoinFetchInfo->mBlockJoinType = KNL_BLOCK_JOIN_SEMI_JOIN;
            break;
        case QLV_JOIN_TYPE_LEFT_ANTI_SEMI:
        case QLV_JOIN_TYPE_RIGHT_ANTI_SEMI:
            sJoinFetchInfo->mBlockJoinType = KNL_BLOCK_JOIN_ANTI_SEMI_JOIN;
            break;
        case QLV_JOIN_TYPE_LEFT_ANTI_SEMI_NA:
        case QLV_JOIN_TYPE_RIGHT_ANTI_SEMI_NA:
            sJoinFetchInfo->mBlockJoinType = KNL_BLOCK_JOIN_ANTI_SEMI_JOIN;
            sJoinFetchInfo->mNullAwareComparison = STL_TRUE;
            break;
        case QLV_JOIN_TYPE_INVERTED_LEFT_SEMI:
        case QLV_JOIN_TYPE_INVERTED_RIGHT_SEMI:
            sJoinFetchInfo->mBlockJoinType = KNL_BLOCK_JOIN_INNER_JOIN;
            break;
        default:
            STL_DASSERT( 0 );
            break;
    }

    /**
     * Key 대상 정보 설정를 Right Child 에서 설정
     */

    sJoinFetchInfo->mKeyBlockList = sExeInnerHashInstant->mLeftConstBlockList;
    sJoinFetchInfo->mKeyConstList = sExeInnerHashInstant->mLeftConstValueList;
    
    /**
     * Join Iteration 정보 설정
     */

    sJoinFetchInfo->mResultRelationList = sRelationBlockList;
    sJoinFetchInfo->mResultBlockList    = sExeHashJoin->mJoinedColBlock;

    sJoinFetchInfo->mResultBlockIdx = 0;
    sJoinFetchInfo->mLeftBlockIdx   = 0;

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF(stlInt32) * aDataArea->mBlockAllocCnt,
                                (void **) & sJoinFetchInfo->mResultRefLeftBlockIdx,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sJoinFetchInfo->mResultRefLeftBlockIdx,
               0x00,
               STL_SIZEOF(stlInt32) * aDataArea->mBlockAllocCnt );
    
    /**
     * Join Filter 정보 설정
     */

    sJoinFetchInfo->mReady4JoinFilter = STL_FALSE;

    sJoinFetchInfo->mLeftBlock4JoinFilter = sLeftFilterBlockList;
    sJoinFetchInfo->mLeftConst4JoinFilter = sLeftFilterConstList;

    sJoinFetchInfo->mRightBlock4JoinFilter = sRightFilterBlockList;
    sJoinFetchInfo->mRightConst4JoinFilter = sRightFilterConstList;

    if ( sExeHashJoin->mHasJoinCondition == STL_TRUE )
    {
        sJoinFetchInfo->mOuterJoinFilter = & sExeHashJoin->mJoinExprEvalInfo;
    }
    else
    {
        sJoinFetchInfo->mOuterJoinFilter = NULL;
    }

    if ( sExeHashJoin->mHasWhereCondition == STL_TRUE )
    {
        sJoinFetchInfo->mResultFilter = & sExeHashJoin->mWhereExprEvalInfo;
    }
    else
    {
        sJoinFetchInfo->mResultFilter = NULL;
    }

    
    /**
     * Prepare Hash Key Eval Info 설정
     */

    STL_DASSERT( sInnerNode->mType ==
                 QLL_PLAN_NODE_HASH_JOIN_INSTANT_ACCESS_TYPE );
    
    sOptHashInstant = (qlnoInstant *) sInnerNode->mOptNode;

    STL_TRY( qlndMakePrepareStackEvalInfo( aDataArea->mExprDataContext,
                                           sOptHashInstant->mPrepareRangeStack,
                                           & sJoinFetchInfo->mPreHashEvalInfo,
                                           & QLL_DATA_PLAN( aDBCStmt ),
                                           aEnv )
             == STL_SUCCESS );


    /**
     * Prepare Join Predicate Eval Info 설정
     */

    STL_TRY( qlndMakePrepareStackEvalInfo( aDataArea->mExprDataContext,
                                           sOptHashInstant->mPrepareFilterStack,
                                           & sJoinFetchInfo->mPreJoinEvalInfo,
                                           & QLL_DATA_PLAN( aDBCStmt ),
                                           aEnv )
             == STL_SUCCESS );

    
    /**
     * End Of Join 초기화
     */

    sJoinFetchInfo->mIsEndOfJoin = STL_FALSE;


    /**
     * Right Child 의 Fetch Information 에 Blocked Join Fetch Information 설정
     */

    sExeInnerHashInstant->mFetchInfo.mBlockJoinFetchInfo = sJoinFetchInfo;

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlnd */

