/*******************************************************************************
 * qlndNestedLoopsJoin.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlndNestedLoopsJoin.c 11114 2014-02-05 02:47:56Z bsyou $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlndNestedLoopsJoin.c
 * @brief SQL Processor Layer Data Optimization Node - NESTED LOOPS JOIN
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnd
 * @{
 */


/**
 * @brief NESTED LOOPS JOIN Execution node에 대한 Data 정보를 구축한다.
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
stlStatus qlndDataOptimizeNestedLoopsJoin( smlTransId              aTransID,
                                           qllDBCStmt            * aDBCStmt,
                                           qllStatement          * aSQLStmt,
                                           qllOptimizationNode   * aOptNode,
                                           qllDataArea           * aDataArea,
                                           qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode               = NULL;

    qllOptimizationNode   * sInnerNode              = 0;
    qllOptimizationNode   * sOuterNode              = 0;

    qlnoNestedLoopsJoin   * sOptNestedLoopsJoin     = NULL;
    qlnxNestedLoopsJoin   * sExeNestedLoopsJoin     = NULL;

    qlnxCacheBlockInfo    * sCacheBlockInfo         = NULL;

    stlInt64              * sLeftCacheBlockIdxPtr   = NULL;
    stlInt64              * sRightCacheBlockIdxPtr  = NULL;

    qlvRefExprItem        * sJoinColItem            = NULL;

    knlValueBlockList     * sDstValueBlockList      = NULL;
    qlnxValueBlockList    * sValueBlockList         = NULL;
    qlnxValueBlockItem    * sValueBlockItem         = NULL;

    knlExprContext              * sContexts             = NULL;
    qlvRefExprItem              * sOuterColumnItem      = NULL;
    qlvInitExpression           * sOrgExpr              = NULL;
    qlnxOuterColumnBlockList    * sOuterColumnBlockList = NULL;
    qlnxOuterColumnBlockItem    * sOuterColumnBlockItem = NULL;

    stlInt32                      i;
    knlExprEvalInfo             * sExprEvalInfo         = NULL;


    /****************************************************************
     * Paramter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_NESTED_LOOPS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * 준비작업
     ****************************************************************/

    /**
     * NESTED LOOPS JOIN Optimization Node 획득
     */

    sOptNestedLoopsJoin = (qlnoNestedLoopsJoin *) aOptNode->mOptNode;


    /**
     * 하위 node에 대한 Data Optimize 수행
     */

    QLND_DATA_OPTIMIZE( aTransID,
                        aDBCStmt,
                        aSQLStmt,
                        sOptNestedLoopsJoin->mLeftChildNode,
                        aDataArea,
                        aEnv );

    QLND_DATA_OPTIMIZE( aTransID,
                        aDBCStmt,
                        aSQLStmt,
                        sOptNestedLoopsJoin->mRightChildNode,
                        aDataArea,
                        aEnv );


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea,
                                        QLL_GET_OPT_NODE_IDX( aOptNode ) );

    
    /**
     * NESTED LOOPS JOIN Execution Node 할당
     */

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlnxNestedLoopsJoin ),
                                (void **) & sExeNestedLoopsJoin,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sExeNestedLoopsJoin, 0x00, STL_SIZEOF( qlnxNestedLoopsJoin ) );


    /**
     * Iterator를 위한 Read Value Block List 구성
     */

    STL_DASSERT( sOptNestedLoopsJoin->mJoinColList != NULL );
    if( sOptNestedLoopsJoin->mJoinColList->mCount == 0 )
    {
        sExeNestedLoopsJoin->mJoinedColBlock = NULL;
    }
    else
    {
        STL_TRY( qlndBuildTableColBlockList( aDataArea,
                                             sOptNestedLoopsJoin->mJoinColList,
                                             STL_TRUE,
                                             & sExeNestedLoopsJoin->mJoinedColBlock,
                                             & QLL_DATA_PLAN( aDBCStmt ),
                                             aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Inner Table Node와 Outer Table Node의 Idx 설정
     */

    switch( sOptNestedLoopsJoin->mJoinType )
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
            sInnerNode = sOptNestedLoopsJoin->mRightChildNode;
            sOuterNode = sOptNestedLoopsJoin->mLeftChildNode;
            sLeftCacheBlockIdxPtr =
                &(sExeNestedLoopsJoin->mOuterCacheBlockInfo.mCurrBlockIdx);
            sRightCacheBlockIdxPtr =
                &(sExeNestedLoopsJoin->mInnerCacheBlockInfo.mCurrBlockIdx);
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


    /****************************************************************
     * Cache Block Info 정보 설정
     ****************************************************************/

    /**
     * Outer(Left) Cache Block Info 정보 구성
     */

    sCacheBlockInfo = &(sExeNestedLoopsJoin->mOuterCacheBlockInfo);
    QLNX_RESET_CACHE_BLOCK_INFO( sCacheBlockInfo );
    sCacheBlockInfo->mNodeIdx = sOuterNode->mIdx;


    /**
     * Inner(Right) Cache Block Info 정보 구성
     */

    sCacheBlockInfo = &(sExeNestedLoopsJoin->mInnerCacheBlockInfo);
    QLNX_RESET_CACHE_BLOCK_INFO( sCacheBlockInfo );
    sCacheBlockInfo->mNodeIdx = sInnerNode->mIdx;


    /****************************************************************
     * Row Block 정보 설정
     ****************************************************************/

    /**
     * Row Block List 초기화
     */

    sExeNestedLoopsJoin->mRowBlockList.mCount = 0;
    sExeNestedLoopsJoin->mRowBlockList.mHead = NULL;
    sExeNestedLoopsJoin->mRowBlockList.mTail = NULL;

    if( sOptNestedLoopsJoin->mNeedRowBlock == STL_TRUE )
    {
        STL_TRY( qlndSetRowBlockOnJoinNode(
                     &QLL_DATA_PLAN( aDBCStmt ),
                     aDataArea->mBlockAllocCnt,
                     ((qlnxCommonInfo*)(aDataArea->mExecNodeList[sOuterNode->mIdx].mExecNode))->mRowBlockList,
                     ((qlnxCommonInfo*)(aDataArea->mExecNodeList[sInnerNode->mIdx].mExecNode))->mRowBlockList,
                     &(sExeNestedLoopsJoin->mRowBlockList),
                     aEnv )
                 == STL_SUCCESS );
    }


    /****************************************************************
     * Value Block List 정보 설정
     ****************************************************************/

    /**
     * Value Block List 초기화
     */

    sValueBlockList = &(sExeNestedLoopsJoin->mValueBlockList);
    sValueBlockList->mCount = 0;
    sValueBlockList->mHead = NULL;
    sValueBlockList->mTail = NULL;

    if( sOptNestedLoopsJoin->mJoinColList->mCount > 0 )
    {
        sJoinColItem = sOptNestedLoopsJoin->mJoinColList->mHead;
        sDstValueBlockList = sExeNestedLoopsJoin->mJoinedColBlock;
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
            sValueBlockItem->mDstBlockIdx = &(sExeNestedLoopsJoin->mCurrBlockIdx);

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
     * Outer Column 정보 설정
     ****************************************************************/

    /**
     * Left Outer Column
     */

    sOuterColumnBlockList = &(sExeNestedLoopsJoin->mLeftOuterColumnBlockList);
    sOuterColumnBlockList->mCount = 0;
    sOuterColumnBlockList->mHead = NULL;
    sOuterColumnBlockList->mTail = NULL;

    sOuterColumnItem = sOptNestedLoopsJoin->mLeftOuterColumnList->mHead;
    sContexts = aDataArea->mExprDataContext->mContexts;
    while( sOuterColumnItem != NULL )
    {
        /* Outer Column Value Block Item 생성 */
        STL_TRY( knlAllocRegionMem( &QLL_DATA_PLAN( aDBCStmt ),
                                    STL_SIZEOF( qlnxOuterColumnBlockItem ),
                                    (void **) & sOuterColumnBlockItem,
                                    KNL_ENV(aEnv) )
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
        sOuterColumnBlockItem->mCast = NULL;

        /* Source Column의 Value Block 및 Block Idx 연결 */
        sOrgExpr = sOuterColumnItem->mExprPtr->mExpr.mOuterColumn->mOrgExpr;
        if( (sOrgExpr->mType == QLV_EXPR_TYPE_COLUMN) ||
            (sOrgExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN) ||
            (sOrgExpr->mType == QLV_EXPR_TYPE_ROWID_COLUMN) )
        {
            sOuterColumnBlockItem->mSrcValueBlock =
                sContexts[sOrgExpr->mOffset].mInfo.mValueInfo;
            sOuterColumnBlockItem->mSrcBlockIdx = sLeftCacheBlockIdxPtr;
        }
        else
        {
            STL_DASSERT( 0 );
        }


        /**
         * Outer Column Value Block List에 등록
         */

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

    sOuterColumnBlockList = &(sExeNestedLoopsJoin->mRightOuterColumnBlockList);
    sOuterColumnBlockList->mCount = 0;
    sOuterColumnBlockList->mHead = NULL;
    sOuterColumnBlockList->mTail = NULL;

    sOuterColumnItem = sOptNestedLoopsJoin->mRightOuterColumnList->mHead;
    sContexts = aDataArea->mExprDataContext->mContexts;
    while( sOuterColumnItem != NULL )
    {
        /* Outer Column Value Block Item 생성 */
        STL_TRY( knlAllocRegionMem( &QLL_DATA_PLAN( aDBCStmt ),
                                    STL_SIZEOF( qlnxOuterColumnBlockItem ),
                                    (void **) & sOuterColumnBlockItem,
                                    KNL_ENV(aEnv) )
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
        sOuterColumnBlockItem->mCast = NULL;

        /* Source Column의 Value Block 및 Block Idx 연결 */
        sOrgExpr = sOuterColumnItem->mExprPtr->mExpr.mOuterColumn->mOrgExpr;
        if( (sOrgExpr->mType == QLV_EXPR_TYPE_COLUMN) ||
            (sOrgExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN) ||
            (sOrgExpr->mType == QLV_EXPR_TYPE_ROWID_COLUMN) )
        {
            sOuterColumnBlockItem->mSrcValueBlock =
                sContexts[sOrgExpr->mOffset].mInfo.mValueInfo;
            sOuterColumnBlockItem->mSrcBlockIdx = sRightCacheBlockIdxPtr;
        }
        else
        {
            STL_DASSERT( 0 );
        }


        /**
         * Outer Column Value Block List에 등록
         */

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
     * Where Filter Outer Column
     */

    sOuterColumnBlockList = &(sExeNestedLoopsJoin->mWhereOuterColumnBlockList);
    sOuterColumnBlockList->mCount = 0;
    sOuterColumnBlockList->mHead = NULL;
    sOuterColumnBlockList->mTail = NULL;

    sOuterColumnItem = sOptNestedLoopsJoin->mWhereOuterColumnList->mHead;
    sContexts = aDataArea->mExprDataContext->mContexts;
    while( sOuterColumnItem != NULL )
    {
        /* Outer Column Value Block Item 생성 */
        STL_TRY( knlAllocRegionMem( &QLL_DATA_PLAN( aDBCStmt ),
                                    STL_SIZEOF( qlnxOuterColumnBlockItem ),
                                    (void **) & sOuterColumnBlockItem,
                                    KNL_ENV(aEnv) )
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
        sOuterColumnBlockItem->mCast = NULL;

        /* Source Column의 Value Block 및 Block Idx 연결 */
        sOrgExpr = sOuterColumnItem->mExprPtr->mExpr.mOuterColumn->mOrgExpr;
        if( (sOrgExpr->mType == QLV_EXPR_TYPE_COLUMN) ||
            (sOrgExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN) ||
            (sOrgExpr->mType == QLV_EXPR_TYPE_ROWID_COLUMN) )
        {
            sOuterColumnBlockItem->mSrcValueBlock =
                sContexts[sOrgExpr->mOffset].mInfo.mValueInfo;
            sOuterColumnBlockItem->mSrcBlockIdx = &(sExeNestedLoopsJoin->mCurrBlockIdx);
        }
        else
        {
            STL_DASSERT( 0 );
        }


        /**
         * Outer Column Value Block List에 등록
         */

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
     * SubQuery 관련 설정
     ****************************************************************/

    /* coverage : test 생성 어려움 */
    /* Statement단위 처리의 SubQuery Node에 대한 Data Optimize 수행 */
    if( sOptNestedLoopsJoin->mStmtSubQueryNode != NULL )
    {
        QLND_DATA_OPTIMIZE( aTransID,
                            aDBCStmt,
                            aSQLStmt,
                            sOptNestedLoopsJoin->mStmtSubQueryNode,
                            aDataArea,
                            aEnv );
    }

    /* SubQuery Filter에 대한 Data Optimize 수행 */
    if( sOptNestedLoopsJoin->mSubQueryAndFilterCnt > 0 )
    {
        /* SubQuery Filter Node에 대한 Data Optimize 수행 */
        for( i = 0; i < sOptNestedLoopsJoin->mSubQueryAndFilterCnt; i++ )
        {
            QLND_DATA_OPTIMIZE( aTransID,
                                aDBCStmt,
                                aSQLStmt,
                                sOptNestedLoopsJoin->mSubQueryAndFilterNodeArr[i],
                                aDataArea,
                                aEnv );
        }

        /* SubQuery Filter Evaluate Info 및 관련 value block list 설정 */
        STL_TRY( knlAllocRegionMem(
                     & QLL_DATA_PLAN( aDBCStmt ),
                     STL_SIZEOF( knlExprEvalInfo* ) * sOptNestedLoopsJoin->mSubQueryAndFilterCnt,
                     (void **) & sExeNestedLoopsJoin->mSubQueryFilterExprEvalInfo,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemset( sExeNestedLoopsJoin->mSubQueryFilterExprEvalInfo,
                   0x00,
                   STL_SIZEOF( knlExprEvalInfo* ) * sOptNestedLoopsJoin->mSubQueryAndFilterCnt );

        STL_TRY( knlAllocRegionMem(
                     & QLL_DATA_PLAN( aDBCStmt ),
                     STL_SIZEOF( knlExprEvalInfo ) * sOptNestedLoopsJoin->mSubQueryAndFilterCnt,
                     (void **) & sExprEvalInfo,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        for( i = 0; i < sOptNestedLoopsJoin->mSubQueryAndFilterCnt; i++ )
        {
            STL_DASSERT( sOptNestedLoopsJoin->mSubQueryFilterPredArr[i] != NULL );

            sExprEvalInfo[i].mExprStack      =
                KNL_PRED_GET_FILTER( sOptNestedLoopsJoin->mSubQueryFilterPredArr[i] );
            sExprEvalInfo[i].mContext        =
                aDataArea->mExprDataContext;
            sExprEvalInfo[i].mResultBlock    =
                aDataArea->mPredResultBlock;

            sExprEvalInfo[i].mBlockIdx   = 0;
            sExprEvalInfo[i].mBlockCount = 0;

            sExeNestedLoopsJoin->mSubQueryFilterExprEvalInfo[i] = &sExprEvalInfo[i];
        }
    }
    else
    {
        sExeNestedLoopsJoin->mSubQueryFilterExprEvalInfo = NULL;
    }


    /****************************************************************
     * 기타 정보 설정
     ****************************************************************/

    /**
     * Where Condition Evaluate Info 설정
     */

    if( sOptNestedLoopsJoin->mLogicalFilterPred != NULL )
    {
        sExeNestedLoopsJoin->mWhereExprEvalInfo.mExprStack      =
            KNL_PRED_GET_FILTER( sOptNestedLoopsJoin->mLogicalFilterPred );
        sExeNestedLoopsJoin->mWhereExprEvalInfo.mContext        =
            aDataArea->mExprDataContext;
        sExeNestedLoopsJoin->mWhereExprEvalInfo.mResultBlock    =
            aDataArea->mPredResultBlock;

        sExeNestedLoopsJoin->mWhereExprEvalInfo.mBlockIdx   = 0;
        sExeNestedLoopsJoin->mWhereExprEvalInfo.mBlockCount = 0;

        sExeNestedLoopsJoin->mHasWhereCondition = STL_TRUE;
    }
    else
    {
        sExeNestedLoopsJoin->mHasWhereCondition = STL_FALSE;
    }


    /**
     * Join Condition Evaluate Info 설정
     */

    if( sOptNestedLoopsJoin->mJoinFilterPred != NULL )
    {
        sExeNestedLoopsJoin->mJoinExprEvalInfo.mExprStack       =
            KNL_PRED_GET_FILTER( sOptNestedLoopsJoin->mJoinFilterPred );
        sExeNestedLoopsJoin->mJoinExprEvalInfo.mContext         =
            aDataArea->mExprDataContext;
        sExeNestedLoopsJoin->mJoinExprEvalInfo.mResultBlock     =
            aDataArea->mPredResultBlock;
        
        sExeNestedLoopsJoin->mJoinExprEvalInfo.mBlockIdx   = 0;
        sExeNestedLoopsJoin->mJoinExprEvalInfo.mBlockCount = 0;

        sExeNestedLoopsJoin->mHasJoinCondition = STL_TRUE;
    }
    else
    {
        sExeNestedLoopsJoin->mHasJoinCondition = STL_FALSE;
    }


    /* First Fetch 여부 */
    sExeNestedLoopsJoin->mIsFirstFetch = STL_TRUE;


    /* EOF (더이상 Fetch할 것이 없는지) 여부 */
    sExeNestedLoopsJoin->mIsEOF = STL_FALSE;

    /* Join Type Flag 셋팅 */
    sExeNestedLoopsJoin->mJoinType = sOptNestedLoopsJoin->mJoinType;


    /**
     * Common Info 설정
     */

    sExeNestedLoopsJoin->mCommonInfo.mResultColBlock   = sExeNestedLoopsJoin->mJoinedColBlock;

    if( sOptNestedLoopsJoin->mNeedRowBlock == STL_TRUE )
    {
        sExeNestedLoopsJoin->mCommonInfo.mRowBlockList = &(sExeNestedLoopsJoin->mRowBlockList);
    }
    else
    {
        sExeNestedLoopsJoin->mCommonInfo.mRowBlockList = NULL;
    }


    /**
     * Outer Column Value Block List 설정
     */

    STL_TRY( qlndBuildOuterColBlockList( aDataArea,
                                         sOptNestedLoopsJoin->mOuterColumnList,
                                         & sExeNestedLoopsJoin->mCommonInfo.mOuterColBlock,
                                         & sExeNestedLoopsJoin->mCommonInfo.mOuterOrgColBlock,
                                         & QLL_DATA_PLAN( aDBCStmt ),
                                         aEnv )
             == STL_SUCCESS );


    /**
     * Common Execution Node 정보 설정
     */

    sExecNode->mNodeType  = QLL_PLAN_NODE_NESTED_LOOPS_TYPE;
    sExecNode->mOptNode   = aOptNode;
    sExecNode->mExecNode  = sExeNestedLoopsJoin;

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





