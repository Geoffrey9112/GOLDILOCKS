/*******************************************************************************
 * qlnxSortMergeJoin.c
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
 * @file qlnxSortMergeJoin.c
 * @brief SQL Processor Layer Execution Node - SORT MERGE JOIN
 */

#include <qll.h>
#include <qlDef.h>

/**
 * @addtogroup qlnx
 * @{
 */


/**
 * @brief SORT MERGE JOIN node를 초기화한다.
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
stlStatus qlnxInitializeSortMergeJoin( smlTransId              aTransID,
                                       smlStatement          * aStmt,
                                       qllDBCStmt            * aDBCStmt,
                                       qllStatement          * aSQLStmt,
                                       qllOptimizationNode   * aOptNode,
                                       qllDataArea           * aDataArea,
                                       qllEnv                * aEnv )
{
    qllExecutionNode    * sExecNode           = NULL;
    /* qlnoSortMergeJoin   * sOptSortMergeJoin   = NULL; */
    qlnxSortMergeJoin   * sExeSortMergeJoin   = NULL;


    /*****************************************
     * Parameter Validation
     ****************************************/

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_MERGE_JOIN_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /*****************************************
     * 기본 정보 획득
     ****************************************/

    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea,
                                        QLL_GET_OPT_NODE_IDX( aOptNode ) );

    
    /**
     * SORT MERGE JOIN Optimization Node 획득
     */

    /* sOptSortMergeJoin = (qlnoSortMergeJoin *) aOptNode->mOptNode; */


    /**
     * SORT MERGE JOIN Execution Node 할당
     */

    sExeSortMergeJoin = (qlnxSortMergeJoin *) sExecNode->mExecNode;

    sExeSortMergeJoin->mHasFalseFilter = STL_FALSE;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SORT MERGE JOIN node를 수행한다.
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
stlStatus qlnxExecuteSortMergeJoin( smlTransId              aTransID,
                                    smlStatement          * aStmt,
                                    qllDBCStmt            * aDBCStmt,
                                    qllStatement          * aSQLStmt,
                                    qllOptimizationNode   * aOptNode,
                                    qllDataArea           * aDataArea,
                                    qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode               = NULL;
    qlnoSortMergeJoin     * sOptSortMergeJoin       = NULL;
    qlnxSortMergeJoin     * sExeSortMergeJoin       = NULL;
    qlnxCacheBlockInfo    * sCacheBlockInfo         = NULL;

    /* qlnInstantTable       * sInstantTable           = NULL; */
    /* smlFetchInfo          * sInstantFetchInfo       = NULL; */

    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_MERGE_JOIN_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /*****************************************
     * 기본 정보 획득
     ****************************************/

    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea,
                                        QLL_GET_OPT_NODE_IDX( aOptNode ) );

    
    /**
     * SORT MERGE JOIN Optimization Node 획득
     */

    sOptSortMergeJoin = (qlnoSortMergeJoin *) aOptNode->mOptNode;


    /**
     * SORT MERGE JOIN Execution Node 할당
     */

    sExeSortMergeJoin = (qlnxSortMergeJoin *) sExecNode->mExecNode;


    /*****************************************
     * Outer Table 및 Inner Table 초기화
     ****************************************/

    /**
     * @todo 불필요한 요소를 제거하여 Filter 재구성 (확장 필요)
     */
    
    sExeSortMergeJoin->mHasFalseFilter =
        qlfHasFalseFilter( sOptSortMergeJoin->mWhereFilterExpr,
                           aDataArea );

    if( sExeSortMergeJoin->mHasFalseFilter == STL_FALSE )
    {
        switch( sExeSortMergeJoin->mJoinType )
        {
            case QLV_JOIN_TYPE_LEFT_OUTER :
            case QLV_JOIN_TYPE_RIGHT_OUTER :
            case QLV_JOIN_TYPE_FULL_OUTER :
            case QLV_JOIN_TYPE_LEFT_ANTI_SEMI :
            case QLV_JOIN_TYPE_RIGHT_ANTI_SEMI :
            case QLV_JOIN_TYPE_LEFT_ANTI_SEMI_NA :
            case QLV_JOIN_TYPE_RIGHT_ANTI_SEMI_NA :
                /* Do Nothing */
                break;
            default :
                sExeSortMergeJoin->mHasFalseFilter =
                    qlfHasFalseFilter( sOptSortMergeJoin->mJoinFilterExpr,
                                       aDataArea );
                break;
        }
    }

    /**
     * Outer Table 초기화
     */

    sCacheBlockInfo = &(sExeSortMergeJoin->mOuterCacheBlockInfo);
    QLNX_EXECUTE_NODE( aTransID,
                       aStmt,
                       aDBCStmt,
                       aSQLStmt,
                       aDataArea->mExecNodeList[sCacheBlockInfo->mNodeIdx].mOptNode,
                       aDataArea,
                       aEnv );
    QLNX_RESET_CACHE_BLOCK_INFO( sCacheBlockInfo );


    /**
     * Inner Table 초기화
     * ----------------------
     *  Inner Table은 Equal Join이 아닌 경우 Fetch를 새로 해야 하는 것으로 셋팅해야 한다.
     *  실제 Fetch에서 Execute를 호출하므로 여기서 Execute를
     *  호출할 필요가 없다.
     */

    sCacheBlockInfo = &(sExeSortMergeJoin->mInnerCacheBlockInfo);
    QLNX_EXECUTE_NODE( aTransID,
                       aStmt,
                       aDBCStmt,
                       aSQLStmt,
                       aDataArea->mExecNodeList[sCacheBlockInfo->mNodeIdx].mOptNode,
                       aDataArea,
                       aEnv );
    if( aDataArea->mExecNodeList[sCacheBlockInfo->mNodeIdx].mOptNode->mType == QLL_PLAN_NODE_SORT_JOIN_INSTANT_ACCESS_TYPE )
    {
        QLNX_SET_END_OF_TABLE_FETCH( sCacheBlockInfo );
    }
    else
    {
        QLNX_RESET_CACHE_BLOCK_INFO( sCacheBlockInfo );
    }


    /*****************************************
     * Fetch Function Pointer 설정
     ****************************************/

    switch( sExeSortMergeJoin->mJoinType )
    {
        case QLV_JOIN_TYPE_INNER:
            sExeSortMergeJoin->mFetchFunc =
                ( sOptSortMergeJoin->mRightChildNode->mType ==
                  QLL_PLAN_NODE_SORT_JOIN_INSTANT_ACCESS_TYPE
                  ? (void*)qlnxFetchSortMergeJoin_Instant_Inner
                  : (void*)qlnxFetchSortMergeJoin_Index_Inner );
            break;
        case QLV_JOIN_TYPE_LEFT_OUTER:
        case QLV_JOIN_TYPE_RIGHT_OUTER:
        case QLV_JOIN_TYPE_FULL_OUTER:
            sExeSortMergeJoin->mFetchFunc =
                ( sOptSortMergeJoin->mRightChildNode->mType ==
                  QLL_PLAN_NODE_SORT_JOIN_INSTANT_ACCESS_TYPE
                  ? (void*)qlnxFetchSortMergeJoin_Instant_Outer
                  : (void*)qlnxFetchSortMergeJoin_Index_Outer );
            break;
        case QLV_JOIN_TYPE_LEFT_SEMI:
        case QLV_JOIN_TYPE_RIGHT_SEMI:
            sExeSortMergeJoin->mFetchFunc =
                ( sOptSortMergeJoin->mRightChildNode->mType ==
                  QLL_PLAN_NODE_SORT_JOIN_INSTANT_ACCESS_TYPE
                  ? (void*)qlnxFetchSortMergeJoin_Instant_Semi
                  : (void*)qlnxFetchSortMergeJoin_Index_Semi );
            break;
        case QLV_JOIN_TYPE_LEFT_ANTI_SEMI:
        case QLV_JOIN_TYPE_RIGHT_ANTI_SEMI:
            sExeSortMergeJoin->mFetchFunc =
                ( sOptSortMergeJoin->mRightChildNode->mType ==
                  QLL_PLAN_NODE_SORT_JOIN_INSTANT_ACCESS_TYPE
                  ? (void*)qlnxFetchSortMergeJoin_Instant_AntiSemi
                  : (void*)qlnxFetchSortMergeJoin_Index_AntiSemi );
            break;

        default:
            STL_DASSERT( 0 );
            sExeSortMergeJoin->mFetchFunc = NULL;
            break;
    }

    
    /*****************************************
     * Reset을 위한 기타 정보들 초기화
     ****************************************/

    /* Result Block Index */
    sExeSortMergeJoin->mCurrBlockIdx = 0;

    /* First Fetch 여부 */
    sExeSortMergeJoin->mIsFirstFetch = STL_TRUE;

    /* EOF 여부 */
    sExeSortMergeJoin->mIsEOF = STL_FALSE;
    sExeSortMergeJoin->mIsGroupEOF = STL_FALSE;
    sExeSortMergeJoin->mIsSetAntiOuter = STL_FALSE;

    /* Left 및 Right Record의 Match 여부 */
    sExeSortMergeJoin->mIsLeftMatched = STL_TRUE;
    sExeSortMergeJoin->mIsRightMatched = STL_TRUE;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Instant를 이용한 Inner Join에 대한 SORT MERGE JOIN node를 Fetch한다.
 * @param[in]      aFetchNodeInfo  Fetch Node Info
 * @param[out]     aUsedBlockCnt   Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF            End Of Fetch 여부
 * @param[in]      aEnv            Environment
 *
 * @remark Fetch 수행시 호출된다.
 */
stlStatus qlnxFetchSortMergeJoin_Instant_Inner( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                                stlInt64              * aUsedBlockCnt,
                                                stlBool               * aEOF,
                                                qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode               = NULL;
    qlnxSortMergeJoin     * sExeSortMergeJoin       = NULL;
    qllOptimizationNode   * sInnerOptNode           = NULL;

    stlBool                 sEOF                    = STL_FALSE;

    stlInt64                sUsedBlockCnt           = 0;
    stlInt64                sRemainSkipCnt          = 0;
    stlInt64                sRemainFetchCnt         = 0;

    qlnxCacheBlockInfo    * sInnerCacheBlockInfo    = NULL;
    qlnxCacheBlockInfo    * sOuterCacheBlockInfo    = NULL;

    knlExprEvalInfo       * sWhereExprEvalInfo      = NULL;
    knlExprEvalInfo       * sJoinExprEvalInfo       = NULL;

    qlnxOuterColumnBlockItem  * sOuterColumnBlockItem   = NULL;

    dtlCompareResult            sCompareResult = DTL_COMPARISON_EQUAL;
    stlBool                     sHasNullValue;

    stlBool                 sIsComparedEqual            = STL_FALSE;
    stlBool                 sIsFirstFetch               = STL_FALSE;

    qlnxFetchNodeInfo       sLocalFetchInfo;

    QLL_OPT_DECLARE( stlTime sBeginFetchTime );
    QLL_OPT_DECLARE( stlTime sEndFetchTime );
    QLL_OPT_DECLARE( stlTime sFetchTime );

    QLL_OPT_DECLARE( stlTime sBeginExceptTime );
    QLL_OPT_DECLARE( stlTime sEndExceptTime );
    QLL_OPT_DECLARE( stlTime sExceptTime );

    QLL_OPT_SET_VALUE( sFetchTime, 0 );
    QLL_OPT_SET_VALUE( sExceptTime, 0 );


    /*****************************************
     * Parameter Validation
     ****************************************/

    STL_PARAM_VALIDATE( aFetchNodeInfo->mOptNode->mType == QLL_PLAN_NODE_MERGE_JOIN_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    QLL_OPT_CHECK_TIME( sBeginFetchTime );

    /*****************************************
     * 기본 정보 획득
     ****************************************/

    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aFetchNodeInfo->mDataArea,
                                        QLL_GET_OPT_NODE_IDX( aFetchNodeInfo->mOptNode ) );


    /**
     * SORT MERGE JOIN Execution Node 획득
     */

    sExeSortMergeJoin = (qlnxSortMergeJoin*) sExecNode->mExecNode;


    /**
     * Node 수행 여부 확인 
     */

    STL_DASSERT( *aEOF == STL_FALSE );
    STL_DASSERT( aFetchNodeInfo->mFetchCnt > 0);


    /**
     * Used Block Count 초기화
     */

    sUsedBlockCnt = 0;


    /**
     * Cache Block Info 정보 설정
     */

    sInnerCacheBlockInfo = &(sExeSortMergeJoin->mInnerCacheBlockInfo);
    sOuterCacheBlockInfo = &(sExeSortMergeJoin->mOuterCacheBlockInfo);


    /**
     * Evaluate Info 관련 정보 설정
     */

    sWhereExprEvalInfo  = &sExeSortMergeJoin->mWhereExprEvalInfo;
    sJoinExprEvalInfo   = &sExeSortMergeJoin->mJoinExprEvalInfo;


    /**
     * Local Fetch Info 초기화
     */
    
    QLNX_SET_LOCAL_FETCH_INFO( & sLocalFetchInfo, aFetchNodeInfo );


    /*****************************************
     * Node 수행 여부 확인
     ****************************************/

    STL_DASSERT( sExeSortMergeJoin->mIsEOF == STL_FALSE );
    if( sExeSortMergeJoin->mHasFalseFilter == STL_TRUE )
    {
        QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
        QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
        sExeSortMergeJoin->mIsFirstFetch = STL_FALSE;
        sExeSortMergeJoin->mIsEOF = STL_TRUE;

        STL_THROW( RAMP_NO_DATA );
    }
    else
    {
        /* Do Nothing */
    }


    /**
     * Fetch에 필요한 정보 설정
     */

    sRemainSkipCnt = aFetchNodeInfo->mSkipCnt;
    sRemainFetchCnt = ( aFetchNodeInfo->mFetchCnt < aFetchNodeInfo->mDataArea->mBlockAllocCnt
                        ? aFetchNodeInfo->mFetchCnt
                        : aFetchNodeInfo->mDataArea->mBlockAllocCnt );


    /**
     * Inner Opt Node 설정
     */

    sInnerOptNode =
        aFetchNodeInfo->mDataArea->mExecNodeList[sExeSortMergeJoin->mInnerCacheBlockInfo.mNodeIdx].mOptNode;


    /*****************************************
     * Fetch 수행
     ****************************************/

    STL_RAMP( RAMP_READ_NEXT_ONE_ROW );

    sIsComparedEqual = STL_FALSE;
    if( QLNX_NEED_NEXT_FETCH( sInnerCacheBlockInfo ) &&
        ( sInnerCacheBlockInfo->mEOF == STL_TRUE) )
    {
        /* Inner Table을 Fetch가 종료된 경우 */

        /* Outer Table을 다음 레코드로 이동 */
        if( QLNX_NEED_NEXT_FETCH( sOuterCacheBlockInfo ) )
        {
            /* EOF 체크 */
            if( sOuterCacheBlockInfo->mEOF == STL_TRUE )
            {
                /* Outer Table의 Scan 종료 */
                QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                STL_THROW( RAMP_NO_DATA );
            }

            /* Outer Cache Block에 새로운 데이터를 Fetch */
            QLL_OPT_CHECK_TIME( sBeginExceptTime );

            sEOF = STL_FALSE;
            QLNX_FETCH_NODE( & sLocalFetchInfo,
                             sExeSortMergeJoin->mOuterCacheBlockInfo.mNodeIdx,
                             0, /* Start Idx */
                             0, /* Skip Count */
                             QLL_FETCH_COUNT_MAX, /* Fetch Count */
                             & sUsedBlockCnt,
                             & sEOF,
                             aEnv );

            QLL_OPT_CHECK_TIME( sEndExceptTime );
            QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

            if( sUsedBlockCnt == 0 )
            {
                QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                STL_THROW( RAMP_NO_DATA );
            }
            else if( sEOF == STL_TRUE )
            {
                QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
            }
            else
            {
                /* Do Nothing */
            }

            sOuterCacheBlockInfo->mCurrBlockIdx = 0;
            sOuterCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;

            /* 처음 Fetch시에만 Group Reset을 수행한다. */
            if( sExeSortMergeJoin->mIsFirstFetch == STL_TRUE )
            {
                /* Outer Table의 Block이 바뀌었으므로 Inner Table의 Group을 Reset */
                QLL_OPT_CHECK_TIME( sBeginExceptTime );

                sEOF = STL_FALSE;
                STL_TRY( qlnxGroupInitSortJoinInstantAccess( sLocalFetchInfo.mTransID,
                                                             sLocalFetchInfo.mStmt,
                                                             sLocalFetchInfo.mDBCStmt,
                                                             sLocalFetchInfo.mSQLStmt,
                                                             sInnerOptNode,
                                                             sLocalFetchInfo.mDataArea,
                                                             &sEOF,
                                                             aEnv )
                         == STL_SUCCESS );

                QLL_OPT_CHECK_TIME( sEndExceptTime );
                QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                sExeSortMergeJoin->mIsFirstFetch = STL_FALSE;
                if( sEOF == STL_TRUE )
                {
                    /* 더 이상의 Group이 없음. 즉, Join할 레코드가 없음 */
                    sExeSortMergeJoin->mIsGroupEOF = STL_TRUE;
                    QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                    QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                    STL_THROW( RAMP_NO_DATA );
                }
                else
                {
                    QLNX_RESET_CACHE_BLOCK_INFO( sInnerCacheBlockInfo );
                }

                /* Group Init의 경우 반드시 Fetch를 수행해야 한다. */
                sInnerCacheBlockInfo->mNeedScan = STL_TRUE;
                sInnerCacheBlockInfo->mCurrBlockIdx = 0;
            }
            else
            {
                QLL_OPT_CHECK_TIME( sBeginExceptTime );

                sEOF = STL_FALSE;
                STL_TRY( qlnxGroupResetSortJoinInstantAccess( sLocalFetchInfo.mTransID,
                                                              sLocalFetchInfo.mStmt,
                                                              sLocalFetchInfo.mDBCStmt,
                                                              sLocalFetchInfo.mSQLStmt,
                                                              sInnerOptNode,
                                                              sLocalFetchInfo.mDataArea,
                                                              &sEOF,
                                                              aEnv )
                         == STL_SUCCESS );

                QLL_OPT_CHECK_TIME( sEndExceptTime );
                QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                STL_DASSERT( sEOF == STL_FALSE );
                QLNX_RESET_CACHE_BLOCK_INFO( sInnerCacheBlockInfo );

                /* 새로운 Outer 데이터에 대해 수행하므로 반드시 Fetch를 수행해야 한다. */
                sInnerCacheBlockInfo->mNeedScan = STL_TRUE;
                sInnerCacheBlockInfo->mCurrBlockIdx = 0;
            }
        }
        else
        {
            /* Cache에서 읽을 Block Idx를 설정 */
            sOuterCacheBlockInfo->mCurrBlockIdx++;

            /* Outer Table의 현재 key가 이전 key랑 동일한지 파악 */
            QLNX_COMPARE_ONLY_OUTER_SORT_KEY( &(sExeSortMergeJoin->mSortKeyCompareList),
                                              sOuterCacheBlockInfo->mCurrBlockIdx,
                                              sHasNullValue,
                                              sCompareResult );

            STL_TRY_THROW( sHasNullValue == STL_FALSE, RAMP_READ_NEXT_ONE_ROW );

            if( sCompareResult == DTL_COMPARISON_EQUAL )
            {
                /* 이전과 같은 key이므로 Inner Table의 Group을 Reset */
                if( sInnerCacheBlockInfo->mNeedScan == STL_TRUE )
                {
                    QLL_OPT_CHECK_TIME( sBeginExceptTime );

                    sEOF = STL_FALSE;
                    STL_TRY( qlnxGroupResetSortJoinInstantAccess( sLocalFetchInfo.mTransID,
                                                                  sLocalFetchInfo.mStmt,
                                                                  sLocalFetchInfo.mDBCStmt,
                                                                  sLocalFetchInfo.mSQLStmt,
                                                                  sInnerOptNode,
                                                                  sLocalFetchInfo.mDataArea,
                                                                  &sEOF,
                                                                  aEnv )
                             == STL_SUCCESS );

                    QLL_OPT_CHECK_TIME( sEndExceptTime );
                    QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                    STL_DASSERT( sEOF == STL_FALSE );
                    QLNX_RESET_CACHE_BLOCK_INFO( sInnerCacheBlockInfo );
                }

                sIsComparedEqual = STL_TRUE;
            }
            else
            {
                /* 이전과 다른 key이므로 Inner Table의 Group을 Next로 이동 */
                QLL_OPT_CHECK_TIME( sBeginExceptTime );

                sEOF = STL_FALSE;
                STL_TRY( qlnxGroupNextSortJoinInstantAccess( sLocalFetchInfo.mTransID,
                                                             sLocalFetchInfo.mStmt,
                                                             sLocalFetchInfo.mDBCStmt,
                                                             sLocalFetchInfo.mSQLStmt,
                                                             sInnerOptNode,
                                                             sLocalFetchInfo.mDataArea,
                                                             &sEOF,
                                                             aEnv )
                         == STL_SUCCESS );

                QLL_OPT_CHECK_TIME( sEndExceptTime );
                QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                if( sEOF == STL_TRUE )
                {
                    /* 더이상의 Group이 없음. 즉, Inner Table의 Scan이 종료 */
                    sExeSortMergeJoin->mIsGroupEOF = STL_TRUE;
                    QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                    QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                    STL_THROW( RAMP_NO_DATA );
                }
                else
                {
                    QLNX_RESET_CACHE_BLOCK_INFO( sInnerCacheBlockInfo );
                }

                /* Group Next의 경우 반드시 Fetch를 수행해야 한다. */
                sInnerCacheBlockInfo->mNeedScan = STL_TRUE;
                sInnerCacheBlockInfo->mCurrBlockIdx = 0;
            }
        }

        sIsFirstFetch = STL_TRUE;
    }
    else
    {
        /* Inner Table을 Fetch하고 있던 중인 경우 */
        sIsFirstFetch = STL_FALSE;
    }


    /**
     * Outer Table과 Inner Table에 대해 하나씩 레코드 읽어가며 비교
     */

    while( 1 )
    {
        /* Outer Table의 현재 Key와 비교하여 동일한 그룹인지 파악 */
        if( sIsComparedEqual == STL_FALSE )
        {
            QLNX_COMPARE_SORT_KEY( &(sExeSortMergeJoin->mSortKeyCompareList),
                                   sOuterCacheBlockInfo->mCurrBlockIdx,
                                   sInnerCacheBlockInfo->mCurrBlockIdx,
                                   sCompareResult,
                                   aEnv );
        }

        if( sCompareResult == DTL_COMPARISON_EQUAL )
        {
            /* Fetch 수행 */
            if( QLNX_NEED_NEXT_FETCH( sInnerCacheBlockInfo ) &&
                sInnerCacheBlockInfo->mNeedScan == STL_FALSE )
            {
                /* Fetch를 다시할 필요가 없는 경우 Offset을 초기화한다. */
                sInnerCacheBlockInfo->mCurrBlockIdx = -1;
            }

            while( 1 )
            {
                /* Inner Table을 Fetch함 */
                if( QLNX_NEED_NEXT_FETCH( sInnerCacheBlockInfo ) )
                {
                    /* EOF 체크 */
                    STL_TRY_THROW( sInnerCacheBlockInfo->mEOF == STL_FALSE,
                                   RAMP_READ_NEXT_ONE_ROW );

                    /* Inner Cache Block에 새로운 데이터를 Fetch */
                    QLL_OPT_CHECK_TIME( sBeginExceptTime );

                    sEOF = STL_FALSE;
                    QLNX_FETCH_NODE( & sLocalFetchInfo,
                                     sExeSortMergeJoin->mInnerCacheBlockInfo.mNodeIdx,
                                     0, /* Start Idx */
                                     0, /* Skip Count */
                                     QLL_FETCH_COUNT_MAX, /* Fetch Count */
                                     & sUsedBlockCnt,
                                     & sEOF,
                                     aEnv );

                    QLL_OPT_CHECK_TIME( sEndExceptTime );
                    QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                    if( sUsedBlockCnt == 0 )
                    {
                        QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                        STL_THROW( RAMP_READ_NEXT_ONE_ROW );
                    }
                    else if( sEOF == STL_TRUE )
                    {
                        QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                        if( sIsFirstFetch == STL_TRUE )
                        {
                            sInnerCacheBlockInfo->mNeedScan = STL_FALSE;
                        }
                        else
                        {
                            sInnerCacheBlockInfo->mNeedScan = STL_TRUE;
                        }
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                    sIsFirstFetch = STL_FALSE;

                    sInnerCacheBlockInfo->mCurrBlockIdx = 0;
                    sInnerCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;
                }
                else
                {
                    /* Cache에서 읽을 Block Idx를 설정 */
                    sInnerCacheBlockInfo->mCurrBlockIdx++;
                }

                /* Join Condition Logical Filter 평가 */
                if( sExeSortMergeJoin->mHasJoinCondition == STL_TRUE )
                {
                    /* Left Outer Column에 대한 Data Value 재설정 */
                    sOuterColumnBlockItem = sExeSortMergeJoin->mLeftOuterColumnBlockList.mHead;
                    QLNX_SET_OUTER_COLUMN_BLOCK_LIST( sOuterColumnBlockItem, aEnv );

                    /* Right Outer Column에 대한 Data Value 재설정 */
                    sOuterColumnBlockItem = sExeSortMergeJoin->mRightOuterColumnBlockList.mHead;
                    QLNX_SET_OUTER_COLUMN_BLOCK_LIST( sOuterColumnBlockItem, aEnv );

                    /* Join Condition 평가 */
                    sJoinExprEvalInfo->mBlockIdx = sExeSortMergeJoin->mCurrBlockIdx;
                    STL_TRY( knlEvaluateOneExpression( sJoinExprEvalInfo,
                                                       KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    if( !DTL_BOOLEAN_IS_TRUE( KNL_GET_BLOCK_DATA_VALUE(
                                                  sJoinExprEvalInfo->mResultBlock,
                                                  sExeSortMergeJoin->mCurrBlockIdx ) ) )
                    {
                        continue;
                    }
                }
                
                /* Outer Table의 데이터와 Inner Table의 데이터에 대하여 Sort Merge Join 수행 */
                if( sExeSortMergeJoin->mJoinedColBlock != NULL )
                {
                    /* Result Block에 데이터 복사 */
                    QLNX_SET_VALUE_BLOCK( &(sExeSortMergeJoin->mValueBlockList), aEnv );
                }

                /* Where Condition Logical Filter 평가 */
                if( sExeSortMergeJoin->mHasWhereCondition == STL_TRUE )
                {
                    sWhereExprEvalInfo->mBlockIdx = sExeSortMergeJoin->mCurrBlockIdx;
                    STL_TRY( knlEvaluateOneExpression( sWhereExprEvalInfo,
                                                       KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    if( !DTL_BOOLEAN_IS_TRUE(
                            KNL_GET_BLOCK_DATA_VALUE(
                                sWhereExprEvalInfo->mResultBlock,
                                sExeSortMergeJoin->mCurrBlockIdx ) ) )
                    {
                        continue;
                    }
                }

                /* Skip Count가 남았으면 Skip 수행 */
                if( sRemainSkipCnt > 0 )
                {
                    sRemainSkipCnt--;
                    continue;
                }

                /* Row Block들을 Result Block에 복사 */
                QLNX_SET_ROW_BLOCK( &(sExeSortMergeJoin->mRowBlockList),
                                    sExeSortMergeJoin->mCurrBlockIdx,
                                    sOuterCacheBlockInfo->mCurrBlockIdx,
                                    sInnerCacheBlockInfo->mCurrBlockIdx );

                /* Fetch Count 체크 */
                sExeSortMergeJoin->mCurrBlockIdx++;
                sRemainFetchCnt --;

                if( sRemainFetchCnt == 0 )
                {
                    break;
                }
                else
                {
                    /**
                     * Right Table의 Cache Block을 다 읽었지만
                     * 아직 Fetch Count가 남은 경우
                     */

                    /* Do Nothing */
                }
            }

            if( sRemainFetchCnt == 0 )
            {
                break;
            }
        }
        else if( sCompareResult == DTL_COMPARISON_LESS )
        {
            /* Outer Table을 다음 레코드로 이동 */
            if( QLNX_NEED_NEXT_FETCH( sOuterCacheBlockInfo ) )
            {
                /* EOF 체크 */
                if( sOuterCacheBlockInfo->mEOF == STL_TRUE )
                {
                    /* Outer Table의 Scan 종료 */
                    QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                    QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                    STL_THROW( RAMP_NO_DATA );
                }

                /* Outer Cache Block에 새로운 데이터를 Fetch */
                QLL_OPT_CHECK_TIME( sBeginExceptTime );

                sEOF = STL_FALSE;
                QLNX_FETCH_NODE( & sLocalFetchInfo,
                                 sExeSortMergeJoin->mOuterCacheBlockInfo.mNodeIdx,
                                 0, /* Start Idx */
                                 0, /* Skip Count */
                                 QLL_FETCH_COUNT_MAX, /* Fetch Count */
                                 & sUsedBlockCnt,
                                 & sEOF,
                                 aEnv );

                QLL_OPT_CHECK_TIME( sEndExceptTime );
                QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                if( sUsedBlockCnt == 0 )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                    QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                    STL_THROW( RAMP_NO_DATA );
                }
                else if( sEOF == STL_TRUE )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                }
                else
                {
                    /* Do Nothing */
                }

                sOuterCacheBlockInfo->mCurrBlockIdx = 0;
                sOuterCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;
            }
            else
            {
                /* Cache에서 읽을 Block Idx를 설정 */
                sOuterCacheBlockInfo->mCurrBlockIdx++;
            }
        }
        else
        {
            /* Inner Table을 다음 그룹으로 이동 */
            QLL_OPT_CHECK_TIME( sBeginExceptTime );

            sEOF = STL_FALSE;
            STL_TRY( qlnxGroupNextSortJoinInstantAccess( sLocalFetchInfo.mTransID,
                                                         sLocalFetchInfo.mStmt,
                                                         sLocalFetchInfo.mDBCStmt,
                                                         sLocalFetchInfo.mSQLStmt,
                                                         sInnerOptNode,
                                                         sLocalFetchInfo.mDataArea,
                                                         &sEOF,
                                                         aEnv )
                     == STL_SUCCESS );

            QLL_OPT_CHECK_TIME( sEndExceptTime );
            QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

            if( sEOF == STL_TRUE )
            {
                /* 더이상의 Group이 없음. 즉, Inner Table의 Scan이 종료 */
                sExeSortMergeJoin->mIsGroupEOF = STL_TRUE;
                QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                STL_THROW( RAMP_NO_DATA );
            }
            else
            {
                QLNX_RESET_CACHE_BLOCK_INFO( sInnerCacheBlockInfo );
            }

            /* Group Next의 경우 반드시 Fetch를 수행해야 한다. */
            sInnerCacheBlockInfo->mNeedScan = STL_TRUE;
            sInnerCacheBlockInfo->mCurrBlockIdx = 0;

            sIsFirstFetch = STL_TRUE;
        }
    }

    STL_RAMP( RAMP_NO_DATA );

    if( QLNX_IS_END_OF_TABLE_FETCH( sInnerCacheBlockInfo ) &&
        QLNX_IS_END_OF_TABLE_FETCH( sOuterCacheBlockInfo ) )
    {
        sEOF = STL_TRUE;
        sExeSortMergeJoin->mIsEOF = STL_TRUE;
    }
    else
    {
        sEOF = STL_FALSE;
        sExeSortMergeJoin->mIsEOF = STL_FALSE;
    }


    /**
     * OUTPUT 설정
     */

    *aEOF = sEOF;
    *aUsedBlockCnt = sExeSortMergeJoin->mCurrBlockIdx;
    
    /* Parent의 Fetch Node Info 설정 */
    aFetchNodeInfo->mFetchCnt = sRemainFetchCnt;
    aFetchNodeInfo->mSkipCnt  = sRemainSkipCnt;

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT(
        sExeSortMergeJoin->mJoinedColBlock,
        0,
        sExeSortMergeJoin->mCurrBlockIdx );

    QLNX_SET_USED_BLOCK_SIZE_ON_ROW_BLOCK( &(sExeSortMergeJoin->mRowBlockList),
                                           sExeSortMergeJoin->mCurrBlockIdx );

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeSortMergeJoin->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeSortMergeJoin->mCommonInfo.mFetchCallCount, 1 );
    QLL_OPT_ADD_RECORD_STAT_INFO( sExeSortMergeJoin->mCommonInfo.mFetchRecordStat,
                                  sExeSortMergeJoin->mCommonInfo.mResultColBlock,
                                  *aUsedBlockCnt,
                                  aEnv );

    sExeSortMergeJoin->mCommonInfo.mFetchRowCnt += *aUsedBlockCnt;
 
    /* Outer Column의 DstDataValue 정보 복구 */
    sOuterColumnBlockItem = sExeSortMergeJoin->mLeftOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );
    sOuterColumnBlockItem = sExeSortMergeJoin->mRightOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );


    return STL_SUCCESS;

    STL_FINISH;

    /* Outer Column의 DstDataValue 정보 복구 */
    sOuterColumnBlockItem = sExeSortMergeJoin->mLeftOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );
    sOuterColumnBlockItem = sExeSortMergeJoin->mRightOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeSortMergeJoin->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeSortMergeJoin->mCommonInfo.mFetchCallCount, 1 );

    return STL_FAILURE;
}


/**
 * @brief Instant를 이용한 Outer Join에 대한 SORT MERGE JOIN node를 Fetch한다.
 * @param[in]      aFetchNodeInfo  Fetch Node Info
 * @param[out]     aUsedBlockCnt   Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF            End Of Fetch 여부
 * @param[in]      aEnv            Environment
 *
 * @remark Fetch 수행시 호출된다.
 */
stlStatus qlnxFetchSortMergeJoin_Instant_Outer( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                                stlInt64              * aUsedBlockCnt,
                                                stlBool               * aEOF,
                                                qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode               = NULL;
//    qlnoSortMergeJoin     * sOptSortMergeJoin       = NULL;
    qlnxSortMergeJoin     * sExeSortMergeJoin       = NULL;
    qllOptimizationNode   * sInnerOptNode           = NULL;

    stlBool                 sEOF                    = STL_FALSE;

    stlInt64                sUsedBlockCnt           = 0;
    stlInt64                sRemainSkipCnt          = 0;
    stlInt64                sRemainFetchCnt         = 0;

    qlnxCacheBlockInfo    * sInnerCacheBlockInfo    = NULL;
    qlnxCacheBlockInfo    * sOuterCacheBlockInfo    = NULL;

    knlExprEvalInfo       * sWhereExprEvalInfo      = NULL;
    knlExprEvalInfo       * sJoinExprEvalInfo       = NULL;

    qlnxOuterColumnBlockItem  * sOuterColumnBlockItem   = NULL;

    dtlCompareResult        sCompareResult = DTL_COMPARISON_EQUAL;

    stlBool                 sIsComparedEqual            = STL_FALSE;
    stlBool                 sHasNullValue;

    stlBool                 sIsFirstFetch               = STL_FALSE;
    stlBool                 sIsResultTrue;

    qlnxFetchNodeInfo       sLocalFetchInfo;

    QLL_OPT_DECLARE( stlTime sBeginFetchTime );
    QLL_OPT_DECLARE( stlTime sEndFetchTime );
    QLL_OPT_DECLARE( stlTime sFetchTime );

    QLL_OPT_DECLARE( stlTime sBeginExceptTime );
    QLL_OPT_DECLARE( stlTime sEndExceptTime );
    QLL_OPT_DECLARE( stlTime sExceptTime );

    QLL_OPT_SET_VALUE( sFetchTime, 0 );
    QLL_OPT_SET_VALUE( sExceptTime, 0 );


    /*****************************************
     * Parameter Validation
     ****************************************/

    STL_PARAM_VALIDATE( aFetchNodeInfo->mOptNode->mType == QLL_PLAN_NODE_MERGE_JOIN_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    QLL_OPT_CHECK_TIME( sBeginFetchTime );

    /*****************************************
     * 기본 정보 획득
     ****************************************/

    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aFetchNodeInfo->mDataArea,
                                        QLL_GET_OPT_NODE_IDX( aFetchNodeInfo->mOptNode ) );


    /**
     * SORT MERGE JOIN Execution Node 획득
     */

    sExeSortMergeJoin = (qlnxSortMergeJoin*) sExecNode->mExecNode;


    /**
     * SORT MERGE JOIN Optimization Node 획득
     */

//    sOptSortMergeJoin = (qlnoSortMergeJoin *) aOptNode->mOptNode;


    /**
     * Node 수행 여부 확인 
     */

    STL_DASSERT( *aEOF == STL_FALSE );
    STL_DASSERT( aFetchNodeInfo->mFetchCnt > 0);


    /**
     * Used Block Count 초기화
     */

    sUsedBlockCnt = 0;


    /**
     * Cache Block Info 정보 설정
     */

    sInnerCacheBlockInfo = &(sExeSortMergeJoin->mInnerCacheBlockInfo);
    sOuterCacheBlockInfo = &(sExeSortMergeJoin->mOuterCacheBlockInfo);


    /**
     * Evaluate Info 관련 정보 설정
     */

    sWhereExprEvalInfo  = &sExeSortMergeJoin->mWhereExprEvalInfo;
    sJoinExprEvalInfo   = &sExeSortMergeJoin->mJoinExprEvalInfo;


    /**
     * Local Fetch Info 초기화
     */
    
    QLNX_SET_LOCAL_FETCH_INFO( & sLocalFetchInfo, aFetchNodeInfo );


    /*****************************************
     * Node 수행 여부 확인
     ****************************************/

    STL_DASSERT( sExeSortMergeJoin->mIsEOF == STL_FALSE );
    if( sExeSortMergeJoin->mHasFalseFilter == STL_TRUE )
    {
        QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
        QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
        sEOF = STL_TRUE;
        sExeSortMergeJoin->mIsFirstFetch = STL_FALSE;
        sExeSortMergeJoin->mIsEOF = STL_TRUE;

        STL_THROW( RAMP_FINISH );
    }
    else
    {
        /* Do Nothing */
    }


    /**
     * Fetch에 필요한 정보 설정
     */

    sRemainSkipCnt = aFetchNodeInfo->mSkipCnt;
    sRemainFetchCnt = ( aFetchNodeInfo->mFetchCnt < aFetchNodeInfo->mDataArea->mBlockAllocCnt
                        ? aFetchNodeInfo->mFetchCnt
                        : aFetchNodeInfo->mDataArea->mBlockAllocCnt );


    /**
     * Inner Opt Node 설정
     */

    sInnerOptNode =
        aFetchNodeInfo->mDataArea->mExecNodeList[sExeSortMergeJoin->mInnerCacheBlockInfo.mNodeIdx].mOptNode;


    /*****************************************
     * Fetch 수행
     ****************************************/

    /* Right Node를 모두 패치했는지 체크 */
    if( QLNX_NEED_NEXT_FETCH( sInnerCacheBlockInfo ) &&
        (sExeSortMergeJoin->mIsGroupEOF == STL_TRUE) )
    {
        STL_THROW( RAMP_RIGHT_NODE_EOF );
    }


    STL_RAMP( RAMP_READ_NEXT_ONE_ROW );


    sIsComparedEqual = STL_FALSE;
    if( QLNX_NEED_NEXT_FETCH( sInnerCacheBlockInfo ) &&
        ( sInnerCacheBlockInfo->mEOF == STL_TRUE) )
    {
        /* Inner Table을 Fetch가 종료된 경우 */

        /* Right Node의 Flag를 저장 */
        if( (sExeSortMergeJoin->mJoinType == QLV_JOIN_TYPE_FULL_OUTER) &&
            (sExeSortMergeJoin->mInnerCacheBlockInfo.mCacheBlockCnt > 0) )
        {
            KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT(
                sExeSortMergeJoin->mSetRightFlagBlock,
                0,
                sExeSortMergeJoin->mInnerCacheBlockInfo.mCacheBlockCnt );

            STL_TRY( qlnxSetFlagSortJoinInstantAccess(
                         sLocalFetchInfo.mStmt,
                         sLocalFetchInfo.mDataArea->mExecNodeList[sExeSortMergeJoin->mInnerCacheBlockInfo.mNodeIdx].mOptNode,
                         sLocalFetchInfo.mDataArea,
                         sExeSortMergeJoin->mSetRightFlagBlock,
                         aEnv )
                     == STL_SUCCESS );
        }

        if( (sExeSortMergeJoin->mIsFirstFetch == STL_FALSE) &&
            (sExeSortMergeJoin->mIsLeftMatched == STL_FALSE) )
        {
            /* Result Block에 데이터 복사 */
            QLNX_SET_OUTER_VALUE_BLOCK( &(sExeSortMergeJoin->mValueBlockList),
                                        &(sExeSortMergeJoin->mOuterCacheBlockInfo.mCurrBlockIdx),
                                        aEnv );

            sExeSortMergeJoin->mIsLeftMatched = STL_TRUE;

            /* Where Condition Logical Filter 평가 */
            sIsResultTrue = STL_TRUE;
            if( sExeSortMergeJoin->mHasWhereCondition == STL_TRUE )
            {
                sWhereExprEvalInfo->mBlockIdx = sExeSortMergeJoin->mCurrBlockIdx;
                STL_TRY( knlEvaluateOneExpression( sWhereExprEvalInfo,
                                                   KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                if( !DTL_BOOLEAN_IS_TRUE(
                        KNL_GET_BLOCK_DATA_VALUE(
                            sWhereExprEvalInfo->mResultBlock,
                            sExeSortMergeJoin->mCurrBlockIdx ) ) )
                {
                    sIsResultTrue = STL_FALSE;
                }
            }

            if( sIsResultTrue == STL_TRUE )
            {
                /* Skip Count가 남았으면 Skip 수행 */
                if( sRemainSkipCnt > 0 )
                {
                    sRemainSkipCnt--;
                }
                else
                {
                    /* Fetch Count 체크 */
                    sExeSortMergeJoin->mCurrBlockIdx++;
                    sRemainFetchCnt --;

                    if( sRemainFetchCnt == 0 )
                    {
                        STL_THROW( RAMP_FULL_DATA );
                    }
                    else
                    {
                        /**
                         * Right Table의 Cache Block을 다 읽었지만
                         * 아직 Fetch Count가 남은 경우
                         */

                        /* Do Nothing */
                    }
                }
            }
        }

        /* Outer Table을 다음 레코드로 이동 */
        if( QLNX_NEED_NEXT_FETCH( sOuterCacheBlockInfo ) )
        {
            /* EOF 체크 */
            if( sOuterCacheBlockInfo->mEOF == STL_TRUE )
            {
                /* Outer Table의 Scan 종료 */
                QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                STL_THROW( RAMP_LEFT_NODE_EOF );
            }

            /* Outer Cache Block에 새로운 데이터를 Fetch */
            QLL_OPT_CHECK_TIME( sBeginExceptTime );

            sEOF = STL_FALSE;
            QLNX_FETCH_NODE( & sLocalFetchInfo,
                             sExeSortMergeJoin->mOuterCacheBlockInfo.mNodeIdx,
                             0, /* Start Idx */
                             0, /* Skip Count */
                             QLL_FETCH_COUNT_MAX, /* Fetch Count */
                             & sUsedBlockCnt,
                             & sEOF,
                             aEnv );

            QLL_OPT_CHECK_TIME( sEndExceptTime );
            QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

            if( sUsedBlockCnt == 0 )
            {
                QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                STL_THROW( RAMP_LEFT_NODE_EOF );
            }
            else if( sEOF == STL_TRUE )
            {
                QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
            }
            else
            {
                /* Do Nothing */
            }

            sOuterCacheBlockInfo->mCurrBlockIdx = 0;
            sOuterCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;
            sExeSortMergeJoin->mIsLeftMatched = STL_FALSE;

            /* 처음 Fetch시에만 Group Reset을 수행한다. */
            if( sExeSortMergeJoin->mIsFirstFetch == STL_TRUE )
            {
                /* Outer Table의 Block이 바뀌었으므로 Inner Table의 Group을 Reset */
                QLL_OPT_CHECK_TIME( sBeginExceptTime );

                sEOF = STL_FALSE;
                STL_TRY( qlnxGroupInitSortJoinInstantAccess( sLocalFetchInfo.mTransID,
                                                             sLocalFetchInfo.mStmt,
                                                             sLocalFetchInfo.mDBCStmt,
                                                             sLocalFetchInfo.mSQLStmt,
                                                             sInnerOptNode,
                                                             sLocalFetchInfo.mDataArea,
                                                             &sEOF,
                                                             aEnv )
                         == STL_SUCCESS );

                QLL_OPT_CHECK_TIME( sEndExceptTime );
                QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                sExeSortMergeJoin->mIsFirstFetch = STL_FALSE;
                if( sEOF == STL_TRUE )
                {
                    /* 더 이상의 Group이 없음. 즉, Join할 레코드가 없음 */
                    sExeSortMergeJoin->mIsGroupEOF = STL_TRUE;
                    QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                    STL_THROW( RAMP_RIGHT_NODE_EOF );
                }
                else
                {
                    QLNX_RESET_CACHE_BLOCK_INFO( sInnerCacheBlockInfo );
                }

                /* Group Init의 경우 반드시 Fetch를 수행해야 한다. */
                sInnerCacheBlockInfo->mNeedScan = STL_TRUE;
                sInnerCacheBlockInfo->mCurrBlockIdx = 0;
                sExeSortMergeJoin->mIsRightMatched = STL_FALSE;
            }
            else
            {
                /* EOF 체크 */
                STL_DASSERT( (sInnerCacheBlockInfo->mEOF == STL_FALSE) ||
                             (sExeSortMergeJoin->mIsGroupEOF == STL_FALSE));

                QLL_OPT_CHECK_TIME( sBeginExceptTime );

                sEOF = STL_FALSE;
                STL_TRY( qlnxGroupResetSortJoinInstantAccess( sLocalFetchInfo.mTransID,
                                                              sLocalFetchInfo.mStmt,
                                                              sLocalFetchInfo.mDBCStmt,
                                                              sLocalFetchInfo.mSQLStmt,
                                                              sInnerOptNode,
                                                              sLocalFetchInfo.mDataArea,
                                                              &sEOF,
                                                              aEnv )
                         == STL_SUCCESS );

                QLL_OPT_CHECK_TIME( sEndExceptTime );
                QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                STL_DASSERT( sEOF == STL_FALSE );
                QLNX_RESET_CACHE_BLOCK_INFO( sInnerCacheBlockInfo );

                /* 새로운 Outer 데이터에 대해 수행하므로 반드시 Fetch를 수행해야 한다. */
                sInnerCacheBlockInfo->mNeedScan = STL_TRUE;
                sInnerCacheBlockInfo->mCurrBlockIdx = 0;
            }
        }
        else
        {
            /* Cache에서 읽을 Block Idx를 설정 */
            sOuterCacheBlockInfo->mCurrBlockIdx++;
            sExeSortMergeJoin->mIsLeftMatched = STL_FALSE;

            /* Outer Table의 현재 key가 이전 key랑 동일한지 파악 */
            QLNX_COMPARE_ONLY_OUTER_SORT_KEY( &(sExeSortMergeJoin->mSortKeyCompareList),
                                              sOuterCacheBlockInfo->mCurrBlockIdx,
                                              sHasNullValue,
                                              sCompareResult );

            STL_TRY_THROW( sHasNullValue == STL_FALSE, RAMP_READ_NEXT_ONE_ROW );

            if( sCompareResult == DTL_COMPARISON_EQUAL )
            {
                /* 이전과 같은 key이므로 Inner Table의 Group을 Reset */
                if( sInnerCacheBlockInfo->mNeedScan == STL_TRUE )
                {
                    QLL_OPT_CHECK_TIME( sBeginExceptTime );

                    sEOF = STL_FALSE;
                    STL_TRY( qlnxGroupResetSortJoinInstantAccess( sLocalFetchInfo.mTransID,
                                                                  sLocalFetchInfo.mStmt,
                                                                  sLocalFetchInfo.mDBCStmt,
                                                                  sLocalFetchInfo.mSQLStmt,
                                                                  sInnerOptNode,
                                                                  sLocalFetchInfo.mDataArea,
                                                                  &sEOF,
                                                                  aEnv )
                             == STL_SUCCESS );

                    QLL_OPT_CHECK_TIME( sEndExceptTime );
                    QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                    STL_DASSERT( sEOF == STL_FALSE );
                    QLNX_RESET_CACHE_BLOCK_INFO( sInnerCacheBlockInfo );
                }

                sIsComparedEqual = STL_TRUE;
            }
            else
            {
                /* 이전과 다른 key이므로 Inner Table의 Group을 Next로 이동 */
                QLL_OPT_CHECK_TIME( sBeginExceptTime );

                sEOF = STL_FALSE;
                STL_TRY( qlnxGroupNextSortJoinInstantAccess( sLocalFetchInfo.mTransID,
                                                             sLocalFetchInfo.mStmt,
                                                             sLocalFetchInfo.mDBCStmt,
                                                             sLocalFetchInfo.mSQLStmt,
                                                             sInnerOptNode,
                                                             sLocalFetchInfo.mDataArea,
                                                             &sEOF,
                                                             aEnv )
                         == STL_SUCCESS );

                QLL_OPT_CHECK_TIME( sEndExceptTime );
                QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                if( sEOF == STL_TRUE )
                {
                    /* 더이상의 Group이 없음. 즉, Inner Table의 Scan이 종료 */
                    sExeSortMergeJoin->mIsGroupEOF = STL_TRUE;
                    QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                    STL_THROW( RAMP_RIGHT_NODE_EOF );
                }
                else
                {
                    QLNX_RESET_CACHE_BLOCK_INFO( sInnerCacheBlockInfo );
                }

                /* Group Next의 경우 반드시 Fetch를 수행해야 한다. */
                sInnerCacheBlockInfo->mNeedScan = STL_TRUE;
                sInnerCacheBlockInfo->mCurrBlockIdx = 0;
                sExeSortMergeJoin->mIsRightMatched = STL_FALSE;
            }
        }

        sIsFirstFetch = STL_TRUE;
    }
    else
    {
        /* Inner Table을 Fetch하고 있던 중인 경우 */
        sIsFirstFetch = STL_FALSE;
    }


    /**
     * Outer Table과 Inner Table에 대해 하나씩 레코드 읽어가며 비교
     */

    while( 1 )
    {
        /* Outer Table의 현재 Key와 비교하여 동일한 그룹인지 파악 */
        if( sIsComparedEqual == STL_FALSE )
        {
            QLNX_COMPARE_SORT_KEY( &(sExeSortMergeJoin->mSortKeyCompareList),
                                   sOuterCacheBlockInfo->mCurrBlockIdx,
                                   sInnerCacheBlockInfo->mCurrBlockIdx,
                                   sCompareResult,
                                   aEnv );
        }

        if( sCompareResult == DTL_COMPARISON_EQUAL )
        {
            /* Fetch 수행 */
            if( QLNX_NEED_NEXT_FETCH( sInnerCacheBlockInfo ) &&
                sInnerCacheBlockInfo->mNeedScan == STL_FALSE )
            {
                /* Fetch를 다시할 필요가 없는 경우 Offset을 초기화한다. */
                sInnerCacheBlockInfo->mCurrBlockIdx = -1;
            }

            while( 1 )
            {
                /* Inner Table을 Fetch함 */
                if( QLNX_NEED_NEXT_FETCH( sInnerCacheBlockInfo ) )
                {
                    /* Right Node의 Flag를 저장 */
                    if( (sExeSortMergeJoin->mJoinType == QLV_JOIN_TYPE_FULL_OUTER) &&
                        (sExeSortMergeJoin->mInnerCacheBlockInfo.mCacheBlockCnt > 0) )
                    {
                        KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT(
                            sExeSortMergeJoin->mSetRightFlagBlock,
                            0,
                            sExeSortMergeJoin->mInnerCacheBlockInfo.mCacheBlockCnt );

                        STL_TRY( qlnxSetFlagSortJoinInstantAccess(
                                     sLocalFetchInfo.mStmt,
                                     sLocalFetchInfo.mDataArea->mExecNodeList[sExeSortMergeJoin->mInnerCacheBlockInfo.mNodeIdx].mOptNode,
                                     sLocalFetchInfo.mDataArea,
                                     sExeSortMergeJoin->mSetRightFlagBlock,
                                     aEnv )
                                 == STL_SUCCESS );
                    }

                    /* EOF 체크 */
                    STL_TRY_THROW( sInnerCacheBlockInfo->mEOF == STL_FALSE,
                                   RAMP_READ_NEXT_ONE_ROW );

                    /* Outer Cache Block에 새로운 데이터를 Fetch */
                    QLL_OPT_CHECK_TIME( sBeginExceptTime );

                    sEOF = STL_FALSE;
                    QLNX_FETCH_NODE( & sLocalFetchInfo,
                                     sExeSortMergeJoin->mInnerCacheBlockInfo.mNodeIdx,
                                     0, /* Start Idx */
                                     0, /* Skip Count */
                                     QLL_FETCH_COUNT_MAX, /* Fetch Count */
                                     & sUsedBlockCnt,
                                     & sEOF,
                                     aEnv );

                    QLL_OPT_CHECK_TIME( sEndExceptTime );
                    QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                    if( sUsedBlockCnt == 0 )
                    {
                        QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                        STL_THROW( RAMP_READ_NEXT_ONE_ROW );
                    }
                    else if( sEOF == STL_TRUE )
                    {
                        QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                        if( sIsFirstFetch == STL_TRUE )
                        {
                            sInnerCacheBlockInfo->mNeedScan = STL_FALSE;
                        }
                        else
                        {
                            sInnerCacheBlockInfo->mNeedScan = STL_TRUE;
                        }
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                    sIsFirstFetch = STL_FALSE;

                    sInnerCacheBlockInfo->mCurrBlockIdx = 0;
                    sInnerCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;
                    sExeSortMergeJoin->mIsRightMatched = STL_FALSE;
                }
                else
                {
                    /* Cache에서 읽을 Block Idx를 설정 */
                    sInnerCacheBlockInfo->mCurrBlockIdx++;
                    sExeSortMergeJoin->mIsRightMatched = STL_FALSE;
                }

                /* Join Condition Logical Filter 평가 */
                if( sExeSortMergeJoin->mHasJoinCondition == STL_TRUE )
                {
                    /* Left Outer Column에 대한 Data Value 재설정 */
                    sOuterColumnBlockItem = sExeSortMergeJoin->mLeftOuterColumnBlockList.mHead;
                    QLNX_SET_OUTER_COLUMN_BLOCK_LIST( sOuterColumnBlockItem, aEnv );

                    /* Right Outer Column에 대한 Data Value 재설정 */
                    sOuterColumnBlockItem = sExeSortMergeJoin->mRightOuterColumnBlockList.mHead;
                    QLNX_SET_OUTER_COLUMN_BLOCK_LIST( sOuterColumnBlockItem, aEnv );

                    /* Join Condition 평가 */
                    sJoinExprEvalInfo->mBlockIdx = sExeSortMergeJoin->mCurrBlockIdx;
                    STL_TRY( knlEvaluateOneExpression( sJoinExprEvalInfo,
                                                       KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    if( !DTL_BOOLEAN_IS_TRUE(
                            KNL_GET_BLOCK_DATA_VALUE(
                                sJoinExprEvalInfo->mResultBlock,
                                sExeSortMergeJoin->mCurrBlockIdx ) ) )
                    {
                        /* Right Node에 Flag Setting */
                        if( sExeSortMergeJoin->mJoinType == QLV_JOIN_TYPE_FULL_OUTER )
                        {
                            dtlDataValue    * sDataValue    = NULL;

                            sDataValue =
                                KNL_GET_VALUE_BLOCK_DATA_VALUE(
                                    sExeSortMergeJoin->mSetRightFlagBlock->mValueBlock,
                                    sInnerCacheBlockInfo->mCurrBlockIdx );

                            STL_DASSERT( sDataValue->mBufferSize >= 1 );

                            sDataValue->mLength = 1;
                            ((stlInt8*)(sDataValue->mValue))[0] =
                                SML_INSTANT_TABLE_ROW_FLAG_DONTCARE;
                        }

                        sExeSortMergeJoin->mIsRightMatched = STL_TRUE;

                        continue;
                    }
                }

                /* Outer Table의 데이터와 Inner Table의 데이터에 대하여 Sort Merge Join 수행 */
                if( sExeSortMergeJoin->mJoinedColBlock != NULL )
                {
                    /* Result Block에 데이터 복사 */
                    QLNX_SET_VALUE_BLOCK( &(sExeSortMergeJoin->mValueBlockList), aEnv );
                }

                sExeSortMergeJoin->mIsLeftMatched = STL_TRUE;
                sExeSortMergeJoin->mIsRightMatched = STL_TRUE;

                /* Right Node에 Flag Setting */
                if( sExeSortMergeJoin->mJoinType == QLV_JOIN_TYPE_FULL_OUTER )
                {
                    dtlDataValue    * sDataValue    = NULL;

                    sDataValue =
                        KNL_GET_VALUE_BLOCK_DATA_VALUE(
                            sExeSortMergeJoin->mSetRightFlagBlock->mValueBlock,
                            sInnerCacheBlockInfo->mCurrBlockIdx );

                    STL_DASSERT( sDataValue->mBufferSize >= 1 );

                    sDataValue->mLength = 1;
                    ((stlInt8*)(sDataValue->mValue))[0] = SML_INSTANT_TABLE_ROW_FLAG_SET;
                }

                /* Where Condition Logical Filter 평가 */
                if( sExeSortMergeJoin->mHasWhereCondition == STL_TRUE )
                {
                    sWhereExprEvalInfo->mBlockIdx = sExeSortMergeJoin->mCurrBlockIdx;
                    STL_TRY( knlEvaluateOneExpression( sWhereExprEvalInfo,
                                                       KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    if( !DTL_BOOLEAN_IS_TRUE( KNL_GET_BLOCK_DATA_VALUE(
                            sWhereExprEvalInfo->mResultBlock,
                            sExeSortMergeJoin->mCurrBlockIdx ) ) )
                    {
                        continue;
                    }
                }

                /* Skip Count가 남았으면 Skip 수행 */
                if( sRemainSkipCnt > 0 )
                {
                    sRemainSkipCnt--;
                    continue;
                }

                /* Fetch Count 체크 */
                sExeSortMergeJoin->mCurrBlockIdx++;
                sRemainFetchCnt --;

                if( sRemainFetchCnt == 0 )
                {
                    STL_THROW( RAMP_FULL_DATA );
                }
                else
                {
                    /**
                     * Right Table의 Cache Block을 다 읽었지만
                     * 아직 Fetch Count가 남은 경우
                     */

                    /* Do Nothing */
                }
            }

            if( sRemainFetchCnt == 0 )
            {
                STL_THROW( RAMP_FULL_DATA );
            }
        }
        else if( sCompareResult == DTL_COMPARISON_LESS )
        {
            if( sExeSortMergeJoin->mIsLeftMatched == STL_FALSE )
            {
                /* Result Block에 데이터 복사 */
                QLNX_SET_OUTER_VALUE_BLOCK(
                    &(sExeSortMergeJoin->mValueBlockList),
                    &(sExeSortMergeJoin->mOuterCacheBlockInfo.mCurrBlockIdx),
                    aEnv );

                sExeSortMergeJoin->mIsLeftMatched = STL_TRUE;

                /* Where Condition Logical Filter 평가 */
                sIsResultTrue = STL_TRUE;
                if( sExeSortMergeJoin->mHasWhereCondition == STL_TRUE )
                {
                    sWhereExprEvalInfo->mBlockIdx = sExeSortMergeJoin->mCurrBlockIdx;
                    STL_TRY( knlEvaluateOneExpression( sWhereExprEvalInfo,
                                                       KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    if( !DTL_BOOLEAN_IS_TRUE( KNL_GET_BLOCK_DATA_VALUE(
                             sWhereExprEvalInfo->mResultBlock,
                             sExeSortMergeJoin->mCurrBlockIdx ) ) )
                    {
                        sIsResultTrue = STL_FALSE;
                    }
                }

                if( sIsResultTrue == STL_TRUE )
                {
                    /* Skip Count가 남았으면 Skip 수행 */
                    if( sRemainSkipCnt > 0 )
                    {
                        sRemainSkipCnt--;
                    }
                    else
                    {
                        /* Fetch Count 체크 */
                        sExeSortMergeJoin->mCurrBlockIdx++;
                        sRemainFetchCnt --;

                        if( sRemainFetchCnt == 0 )
                        {
                            STL_THROW( RAMP_FULL_DATA );
                        }
                        else
                        {
                            /**
                             * Right Table의 Cache Block을 다 읽었지만
                             * 아직 Fetch Count가 남은 경우
                             */

                            /* Do Nothing */
                        }
                    }
                }
            }

            /* Outer Table을 다음 레코드로 이동 */
            if( QLNX_NEED_NEXT_FETCH( sOuterCacheBlockInfo ) )
            {
                /* EOF 체크 */
                if( sOuterCacheBlockInfo->mEOF == STL_TRUE )
                {
                    /* Outer Table의 Scan 종료 */
                    QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                    STL_THROW( RAMP_LEFT_NODE_EOF );
                }

                /* Outer Cache Block에 새로운 데이터를 Fetch */
                QLL_OPT_CHECK_TIME( sBeginExceptTime );

                sEOF = STL_FALSE;
                QLNX_FETCH_NODE( & sLocalFetchInfo,
                                 sExeSortMergeJoin->mOuterCacheBlockInfo.mNodeIdx,
                                 0, /* Start Idx */
                                 0, /* Skip Count */
                                 QLL_FETCH_COUNT_MAX, /* Fetch Count */
                                 & sUsedBlockCnt,
                                 & sEOF,
                                 aEnv );

                QLL_OPT_CHECK_TIME( sEndExceptTime );
                QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                if( sUsedBlockCnt == 0 )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                    STL_THROW( RAMP_LEFT_NODE_EOF );
                }
                else if( sEOF == STL_TRUE )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                }
                else
                {
                    /* Do Nothing */
                }

                sOuterCacheBlockInfo->mCurrBlockIdx = 0;
                sOuterCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;
                sExeSortMergeJoin->mIsLeftMatched = STL_FALSE;
            }
            else
            {
                /* Cache에서 읽을 Block Idx를 설정 */
                sOuterCacheBlockInfo->mCurrBlockIdx++;
                sExeSortMergeJoin->mIsLeftMatched = STL_FALSE;
            }
        }
        else
        {
            /* Inner Table을 다음 그룹으로 이동 */
            QLL_OPT_CHECK_TIME( sBeginExceptTime );

            sEOF = STL_FALSE;
            STL_TRY( qlnxGroupNextSortJoinInstantAccess( sLocalFetchInfo.mTransID,
                                                         sLocalFetchInfo.mStmt,
                                                         sLocalFetchInfo.mDBCStmt,
                                                         sLocalFetchInfo.mSQLStmt,
                                                         sInnerOptNode,
                                                         sLocalFetchInfo.mDataArea,
                                                         &sEOF,
                                                         aEnv )
                     == STL_SUCCESS );

            QLL_OPT_CHECK_TIME( sEndExceptTime );
            QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

            if( sEOF == STL_TRUE )
            {
                /* 더이상의 Group이 없음. 즉, Inner Table의 Scan이 종료 */
                sExeSortMergeJoin->mIsGroupEOF = STL_TRUE;
                QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                STL_THROW( RAMP_RIGHT_NODE_EOF );
            }
            else
            {
                QLNX_RESET_CACHE_BLOCK_INFO( sInnerCacheBlockInfo );
            }

            /* Group Next의 경우 반드시 Fetch를 수행해야 한다. */
            sInnerCacheBlockInfo->mNeedScan = STL_TRUE;
            sInnerCacheBlockInfo->mCurrBlockIdx = 0;

            sExeSortMergeJoin->mIsRightMatched = STL_FALSE;
        }
    }


    STL_THROW( RAMP_FULL_DATA );


    /**
     * Left Node가 EOF인 경우
     */

    STL_RAMP( RAMP_LEFT_NODE_EOF );

    if( sExeSortMergeJoin->mJoinType == QLV_JOIN_TYPE_FULL_OUTER )
    {
        sExeSortMergeJoin->mFetchFunc =
            (void*)qlnxFetchSortMergeJoin_Instant_AntiOuter;

        sEOF = STL_FALSE;

        sLocalFetchInfo.mOptNode =
            sLocalFetchInfo.mDataArea->mExecNodeList[ sExecNode->mOptNode->mIdx ].mOptNode;
        sLocalFetchInfo.mStartIdx = 0;
        sLocalFetchInfo.mSkipCnt = sRemainSkipCnt;
        sLocalFetchInfo.mFetchCnt = sRemainFetchCnt;

        STL_TRY( qlnxFetchSortMergeJoin_Instant_AntiOuter( & sLocalFetchInfo,
                                                           &sUsedBlockCnt,
                                                           &sEOF,
                                                           aEnv )
                 == STL_SUCCESS );

        sRemainSkipCnt = sLocalFetchInfo.mSkipCnt;
        sRemainFetchCnt = sLocalFetchInfo.mFetchCnt;

        sExeSortMergeJoin->mCommonInfo.mFetchRowCnt -= sUsedBlockCnt;

        STL_DASSERT( sUsedBlockCnt == 0 ? sEOF == STL_TRUE : STL_TRUE );

        STL_THROW( RAMP_FINISH );
    }
    else
    {
        /* Left Outer인 경우 Inner쪽도 종료로 셋팅한다. */
        QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
    }

    STL_THROW( RAMP_FULL_DATA );


    /**
     * Right Node가 EOF인 경우
     */

    STL_RAMP( RAMP_RIGHT_NODE_EOF );

    /* Left Node에 남은 데이터를 모두 내보낸다. */
    while( STL_TRUE )
    {
        if( sExeSortMergeJoin->mIsLeftMatched == STL_TRUE )
        {
            /* Outer Table을 다음 레코드로 이동 */
            if( QLNX_NEED_NEXT_FETCH( sOuterCacheBlockInfo ) )
            {
                /* EOF 체크 */
                if( sOuterCacheBlockInfo->mEOF == STL_TRUE )
                {
                    /* Outer Table의 Scan 종료 */
                    QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                    STL_THROW( RAMP_LEFT_NODE_EOF );
                }

                /* Outer Cache Block에 새로운 데이터를 Fetch */
                QLL_OPT_CHECK_TIME( sBeginExceptTime );

                sEOF = STL_FALSE;
                QLNX_FETCH_NODE( & sLocalFetchInfo,
                                 sExeSortMergeJoin->mOuterCacheBlockInfo.mNodeIdx,
                                 0, /* Start Idx */
                                 0, /* Skip Count */
                                 QLL_FETCH_COUNT_MAX, /* Fetch Count */
                                 & sUsedBlockCnt,
                                 & sEOF,
                                 aEnv );

                QLL_OPT_CHECK_TIME( sEndExceptTime );
                QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                if( sUsedBlockCnt == 0 )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                    STL_THROW( RAMP_LEFT_NODE_EOF );
                }
                else if( sEOF == STL_TRUE )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                }
                else
                {
                    /* Do Nothing */
                }

                sOuterCacheBlockInfo->mCurrBlockIdx = 0;
                sOuterCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;
                sExeSortMergeJoin->mIsLeftMatched = STL_FALSE;
            }
            else
            {
                /* Cache에서 읽을 Block Idx를 설정 */
                sOuterCacheBlockInfo->mCurrBlockIdx++;
                sExeSortMergeJoin->mIsLeftMatched = STL_FALSE;
            }
        }

        /* Result Block에 데이터 복사 */
        QLNX_SET_OUTER_VALUE_BLOCK(
            &(sExeSortMergeJoin->mValueBlockList),
            &(sExeSortMergeJoin->mOuterCacheBlockInfo.mCurrBlockIdx),
            aEnv );

        sExeSortMergeJoin->mIsLeftMatched = STL_TRUE;

        /* Where Condition Logical Filter 평가 */
        if( sExeSortMergeJoin->mHasWhereCondition == STL_TRUE )
        {
            sWhereExprEvalInfo->mBlockIdx = sExeSortMergeJoin->mCurrBlockIdx;
            STL_TRY( knlEvaluateOneExpression( sWhereExprEvalInfo,
                                               KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            if( !DTL_BOOLEAN_IS_TRUE( KNL_GET_BLOCK_DATA_VALUE(
                     sWhereExprEvalInfo->mResultBlock,
                     sExeSortMergeJoin->mCurrBlockIdx ) ) )
            {
                continue;
            }
        }

        sExeSortMergeJoin->mIsLeftMatched = STL_TRUE;
        sExeSortMergeJoin->mIsRightMatched = STL_TRUE;

        /* Skip Count가 남았으면 Skip 수행 */
        if( sRemainSkipCnt > 0 )
        {
            sRemainSkipCnt--;
            continue;
        }

        /* Fetch Count 체크 */
        sExeSortMergeJoin->mCurrBlockIdx++;
        sRemainFetchCnt --;

        if( sRemainFetchCnt == 0 )
        {
            STL_THROW( RAMP_FULL_DATA );
        }
        else
        {
            /**
             * Right Table의 Cache Block을 다 읽었지만
             * 아직 Fetch Count가 남은 경우
             */

            /* Do Nothing */
        }
    }


    STL_RAMP( RAMP_FULL_DATA );

    if( QLNX_IS_END_OF_TABLE_FETCH( sInnerCacheBlockInfo ) &&
        QLNX_IS_END_OF_TABLE_FETCH( sOuterCacheBlockInfo ) )
    {
        if( sExeSortMergeJoin->mJoinType == QLV_JOIN_TYPE_FULL_OUTER )
        {
            /* Right Node의 Flag를 저장 */
            if( sExeSortMergeJoin->mInnerCacheBlockInfo.mCacheBlockCnt > 0 )
            {
                KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT(
                    sExeSortMergeJoin->mSetRightFlagBlock,
                    0,
                    sExeSortMergeJoin->mInnerCacheBlockInfo.mCacheBlockCnt );

                STL_TRY( qlnxSetFlagSortJoinInstantAccess(
                             sLocalFetchInfo.mStmt,
                             sLocalFetchInfo.mDataArea->mExecNodeList[sExeSortMergeJoin->mInnerCacheBlockInfo.mNodeIdx].mOptNode,
                             sLocalFetchInfo.mDataArea,
                             sExeSortMergeJoin->mSetRightFlagBlock,
                             aEnv )
                         == STL_SUCCESS );
            }

            sExeSortMergeJoin->mFetchFunc =
                (void*)qlnxFetchSortMergeJoin_Instant_AntiOuter;

            if( sRemainFetchCnt == 0 )
            {
                sEOF = STL_FALSE;
                sExeSortMergeJoin->mIsEOF = STL_FALSE;
            }
            else
            {
                sEOF = STL_FALSE;

                sLocalFetchInfo.mOptNode =
                    sLocalFetchInfo.mDataArea->mExecNodeList[ sExecNode->mOptNode->mIdx ].mOptNode;
                sLocalFetchInfo.mStartIdx = 0;
                sLocalFetchInfo.mSkipCnt = sRemainSkipCnt;
                sLocalFetchInfo.mFetchCnt = sRemainFetchCnt;

                STL_TRY( qlnxFetchSortMergeJoin_Instant_AntiOuter( & sLocalFetchInfo,
                                                                   &sUsedBlockCnt,
                                                                   &sEOF,
                                                                   aEnv )
                         == STL_SUCCESS );

                sRemainSkipCnt = sLocalFetchInfo.mSkipCnt;
                sRemainFetchCnt = sLocalFetchInfo.mFetchCnt;

                sExeSortMergeJoin->mCommonInfo.mFetchRowCnt -= sUsedBlockCnt;

                STL_DASSERT( sUsedBlockCnt == 0 ? sEOF == STL_TRUE : STL_TRUE );
            }
        }
        else
        {
            sEOF = STL_TRUE;
            sExeSortMergeJoin->mIsEOF = STL_TRUE;
        }
    }
    else
    {
        sEOF = STL_FALSE;
        sExeSortMergeJoin->mIsEOF = STL_FALSE;
    }


    /**
     * OUTPUT 설정
     */

    STL_RAMP( RAMP_FINISH );

    *aEOF = sEOF;
    *aUsedBlockCnt = sExeSortMergeJoin->mCurrBlockIdx;

    /* Parent의 Fetch Node Info 설정 */
    aFetchNodeInfo->mFetchCnt = sRemainFetchCnt;
    aFetchNodeInfo->mSkipCnt  = sRemainSkipCnt;

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT(
        sExeSortMergeJoin->mJoinedColBlock,
        0,
        sExeSortMergeJoin->mCurrBlockIdx );

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeSortMergeJoin->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeSortMergeJoin->mCommonInfo.mFetchCallCount, 1 );
    QLL_OPT_ADD_RECORD_STAT_INFO( sExeSortMergeJoin->mCommonInfo.mFetchRecordStat,
                                  sExeSortMergeJoin->mCommonInfo.mResultColBlock,
                                  *aUsedBlockCnt,
                                  aEnv );

    sExeSortMergeJoin->mCommonInfo.mFetchRowCnt += *aUsedBlockCnt;
 
    /* Outer Column의 DstDataValue 정보 복구 */
    sOuterColumnBlockItem = sExeSortMergeJoin->mLeftOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );
    sOuterColumnBlockItem = sExeSortMergeJoin->mRightOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );


    return STL_SUCCESS;

    STL_FINISH;

    /* Outer Column의 DstDataValue 정보 복구 */
    sOuterColumnBlockItem = sExeSortMergeJoin->mLeftOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );
    sOuterColumnBlockItem = sExeSortMergeJoin->mRightOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeSortMergeJoin->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeSortMergeJoin->mCommonInfo.mFetchCallCount, 1 );

    return STL_FAILURE;
}


/**
 * @brief Instant를 이용한 Anti Outer Join에 대한 SORT MERGE JOIN node를 Fetch한다.
 * @param[in]      aFetchNodeInfo  Fetch Node Info
 * @param[out]     aUsedBlockCnt   Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF            End Of Fetch 여부
 * @param[in]      aEnv            Environment
 *
 * @remark Fetch 수행시 호출된다.
 */
stlStatus qlnxFetchSortMergeJoin_Instant_AntiOuter( qlnxFetchNodeInfo * aFetchNodeInfo,
                                                    stlInt64          * aUsedBlockCnt,
                                                    stlBool           * aEOF,
                                                    qllEnv            * aEnv )
{
    qllExecutionNode      * sExecNode               = NULL;
//    qlnoSortMergeJoin     * sOptSortMergeJoin       = NULL;
    qlnxSortMergeJoin     * sExeSortMergeJoin       = NULL;
    qllOptimizationNode   * sInnerOptNode           = NULL;

    stlBool                 sEOF                    = STL_FALSE;

    stlInt64                sUsedBlockCnt           = 0;
    stlInt64                sRemainSkipCnt          = 0;
    stlInt64                sRemainFetchCnt         = 0;

    qlnxCacheBlockInfo    * sInnerCacheBlockInfo    = NULL;
    qlnxCacheBlockInfo    * sOuterCacheBlockInfo    = NULL;

    knlExprEvalInfo       * sWhereExprEvalInfo      = NULL;

    qlnxOuterColumnBlockItem  * sOuterColumnBlockItem   = NULL;

    stlBool                 sIsFirstFetch               = STL_FALSE;

    qlnxFetchNodeInfo       sLocalFetchInfo;

    QLL_OPT_DECLARE( stlTime sBeginFetchTime );
    QLL_OPT_DECLARE( stlTime sEndFetchTime );
    QLL_OPT_DECLARE( stlTime sFetchTime );

    QLL_OPT_DECLARE( stlTime sBeginExceptTime );
    QLL_OPT_DECLARE( stlTime sEndExceptTime );
    QLL_OPT_DECLARE( stlTime sExceptTime );

    QLL_OPT_SET_VALUE( sFetchTime, 0 );
    QLL_OPT_SET_VALUE( sExceptTime, 0 );


    /*****************************************
     * Parameter Validation
     ****************************************/

    STL_PARAM_VALIDATE( aFetchNodeInfo->mOptNode->mType == QLL_PLAN_NODE_MERGE_JOIN_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    QLL_OPT_CHECK_TIME( sBeginFetchTime );

    /*****************************************
     * 기본 정보 획득
     ****************************************/

    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aFetchNodeInfo->mDataArea,
                                        QLL_GET_OPT_NODE_IDX( aFetchNodeInfo->mOptNode ) );


    /**
     * SORT MERGE JOIN Execution Node 획득
     */

    sExeSortMergeJoin = (qlnxSortMergeJoin*) sExecNode->mExecNode;


    /**
     * SORT MERGE JOIN Optimization Node 획득
     */

//    sOptSortMergeJoin = (qlnoSortMergeJoin *) aOptNode->mOptNode;


    /**
     * Node 수행 여부 확인 
     */

    STL_DASSERT( *aEOF == STL_FALSE );
    STL_DASSERT( aFetchNodeInfo->mFetchCnt > 0);


    /**
     * Used Block Count 초기화
     */

    sUsedBlockCnt = 0;


    /**
     * Cache Block Info 정보 설정
     */

    sInnerCacheBlockInfo = &(sExeSortMergeJoin->mInnerCacheBlockInfo);
    sOuterCacheBlockInfo = &(sExeSortMergeJoin->mOuterCacheBlockInfo);


    /**
     * Evaluate Info 관련 정보 설정
     */

    sWhereExprEvalInfo  = &sExeSortMergeJoin->mWhereExprEvalInfo;


    /**
     * Local Fetch Info 초기화
     */
    
    QLNX_SET_LOCAL_FETCH_INFO( & sLocalFetchInfo, aFetchNodeInfo );


    /*****************************************
     * Node 수행 여부 확인
     ****************************************/

    STL_DASSERT( sExeSortMergeJoin->mIsEOF == STL_FALSE );
    if( sExeSortMergeJoin->mHasFalseFilter == STL_TRUE )
    {
        QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
        QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
        sExeSortMergeJoin->mIsFirstFetch = STL_FALSE;
        sExeSortMergeJoin->mIsEOF = STL_TRUE;

        STL_THROW( RAMP_NO_DATA );
    }
    else
    {
        /* Do Nothing */
    }


    /**
     * Fetch에 필요한 정보 설정
     */

    sRemainSkipCnt = aFetchNodeInfo->mSkipCnt;
    sRemainFetchCnt = ( aFetchNodeInfo->mFetchCnt < aFetchNodeInfo->mDataArea->mBlockAllocCnt
                        ? aFetchNodeInfo->mFetchCnt
                        : aFetchNodeInfo->mDataArea->mBlockAllocCnt );


    /**
     * Inner Opt Node 설정
     */

    sInnerOptNode =
        aFetchNodeInfo->mDataArea->mExecNodeList[sExeSortMergeJoin->mInnerCacheBlockInfo.mNodeIdx].mOptNode;


    /*****************************************
     * Fetch 수행
     ****************************************/

    /* Full Outer인 경우 Right Node에 남은 데이터를 모두 내보낸다. */
    /* 선행처리로 Right Outer인 경우는 없다. */

    if( sExeSortMergeJoin->mIsSetAntiOuter == STL_FALSE )
    {
        QLNX_EXECUTE_NODE( sLocalFetchInfo.mTransID,
                           sLocalFetchInfo.mStmt,
                           sLocalFetchInfo.mDBCStmt,
                           sLocalFetchInfo.mSQLStmt,
                           sLocalFetchInfo.mDataArea->mExecNodeList[sInnerCacheBlockInfo->mNodeIdx].mOptNode,
                           sLocalFetchInfo.mDataArea,
                           aEnv );

        STL_TRY( qlnxSetFetchRowFlagSortJoinInstantAccess(
                     sLocalFetchInfo.mStmt,
                     sLocalFetchInfo.mDataArea->mExecNodeList[sInnerCacheBlockInfo->mNodeIdx].mOptNode,
                     sLocalFetchInfo.mDataArea,
                     SML_INSTANT_TABLE_ROW_FLAG_UNSET,
                     aEnv )
                 == STL_SUCCESS );

        sExeSortMergeJoin->mIsSetAntiOuter = STL_TRUE;
        sExeSortMergeJoin->mIsGroupEOF = STL_FALSE;

        QLL_OPT_CHECK_TIME( sBeginExceptTime );

        sEOF = STL_FALSE;
        STL_TRY( qlnxGroupInitSortJoinInstantAccess( sLocalFetchInfo.mTransID,
                                                     sLocalFetchInfo.mStmt,
                                                     sLocalFetchInfo.mDBCStmt,
                                                     sLocalFetchInfo.mSQLStmt,
                                                     sInnerOptNode,
                                                     sLocalFetchInfo.mDataArea,
                                                     &sEOF,
                                                     aEnv )
                 == STL_SUCCESS );

        QLL_OPT_CHECK_TIME( sEndExceptTime );
        QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

        if( sEOF == STL_TRUE )
        {
            /* 더 이상의 Group이 없음. 즉, Join할 레코드가 없음 */
            sExeSortMergeJoin->mIsGroupEOF = STL_TRUE;
            QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
            STL_THROW( RAMP_NO_DATA );
        }
        else
        {
            QLNX_RESET_CACHE_BLOCK_INFO( sInnerCacheBlockInfo );
        }

        /* Group Init의 경우 반드시 Fetch를 수행해야 한다. */
        sInnerCacheBlockInfo->mNeedScan = STL_TRUE;
        sInnerCacheBlockInfo->mCurrBlockIdx = 0;

        sExeSortMergeJoin->mIsRightMatched = STL_FALSE;
    }

    while( STL_TRUE )
    {
        sIsFirstFetch = STL_TRUE;
        while( STL_TRUE )
        {
            /* Inner Table을 Fetch함 */
            if( QLNX_NEED_NEXT_FETCH( sInnerCacheBlockInfo ) )
            {
                /* EOF 체크 */
                if( sInnerCacheBlockInfo->mEOF == STL_TRUE )
                {
                    break;
                }

                /* Inner Cache Block에 새로운 데이터를 Fetch */
                QLL_OPT_CHECK_TIME( sBeginExceptTime );

                sEOF = STL_FALSE;
                QLNX_FETCH_NODE( & sLocalFetchInfo,
                                 sExeSortMergeJoin->mInnerCacheBlockInfo.mNodeIdx,
                                 0, /* Start Idx */
                                 0, /* Skip Count */
                                 QLL_FETCH_COUNT_MAX, /* Fetch Count */
                                 & sUsedBlockCnt,
                                 & sEOF,
                                 aEnv );

                QLL_OPT_CHECK_TIME( sEndExceptTime );
                QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                if( sUsedBlockCnt == 0 )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                    break;
                }
                else if( sEOF == STL_TRUE )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                    if( sIsFirstFetch == STL_TRUE )
                    {
                        sInnerCacheBlockInfo->mNeedScan = STL_FALSE;
                    }
                    else
                    {
                        sInnerCacheBlockInfo->mNeedScan = STL_TRUE;
                    }
                }
                else
                {
                    /* Do Nothing */
                }
                sIsFirstFetch = STL_FALSE;

                sInnerCacheBlockInfo->mCurrBlockIdx = 0;
                sInnerCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;
                sExeSortMergeJoin->mIsRightMatched = STL_FALSE;
            }
            else
            {
                /* Cache에서 읽을 Block Idx를 설정 */
                sInnerCacheBlockInfo->mCurrBlockIdx++;
                sExeSortMergeJoin->mIsRightMatched = STL_FALSE;
            }

            /* Outer Table의 데이터와 Inner Table의 데이터에 대하여 Sort Merge Join 수행 */
            if( sExeSortMergeJoin->mJoinedColBlock != NULL )
            {
                /* Result Block에 데이터 복사 */
                QLNX_SET_OUTER_VALUE_BLOCK(
                    &(sExeSortMergeJoin->mValueBlockList),
                    &(sExeSortMergeJoin->mInnerCacheBlockInfo.mCurrBlockIdx),
                    aEnv );

                sExeSortMergeJoin->mIsRightMatched = STL_TRUE;
            }

            /* Where Condition Logical Filter 평가 */
            if( sExeSortMergeJoin->mHasWhereCondition == STL_TRUE )
            {
                sWhereExprEvalInfo->mBlockIdx = sExeSortMergeJoin->mCurrBlockIdx;
                STL_TRY( knlEvaluateOneExpression( sWhereExprEvalInfo,
                                                   KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                if( !DTL_BOOLEAN_IS_TRUE(
                        KNL_GET_BLOCK_DATA_VALUE(
                            sWhereExprEvalInfo->mResultBlock,
                            sExeSortMergeJoin->mCurrBlockIdx ) ) )
                {
                    continue;
                }
            }

            sExeSortMergeJoin->mIsLeftMatched = STL_TRUE;
            sExeSortMergeJoin->mIsRightMatched = STL_TRUE;

            /* Skip Count가 남았으면 Skip 수행 */
            if( sRemainSkipCnt > 0 )
            {
                sRemainSkipCnt--;
                continue;
            }

            /* Fetch Count 체크 */
            sExeSortMergeJoin->mCurrBlockIdx++;
            sRemainFetchCnt --;

            if( sRemainFetchCnt == 0 )
            {
                STL_THROW( RAMP_FULL_DATA );
            }
            else
            {
                /**
                 * Right Table의 Cache Block을 다 읽었지만
                 * 아직 Fetch Count가 남은 경우
                 */

                /* Do Nothing */
            }
        }

        /* Inner Table을 다음 그룹으로 이동 */
        QLL_OPT_CHECK_TIME( sBeginExceptTime );

        sEOF = STL_FALSE;
        STL_TRY( qlnxGroupNextSortJoinInstantAccess( sLocalFetchInfo.mTransID,
                                                     sLocalFetchInfo.mStmt,
                                                     sLocalFetchInfo.mDBCStmt,
                                                     sLocalFetchInfo.mSQLStmt,
                                                     sInnerOptNode,
                                                     sLocalFetchInfo.mDataArea,
                                                     &sEOF,
                                                     aEnv )
                 == STL_SUCCESS );

        QLL_OPT_CHECK_TIME( sEndExceptTime );
        QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

        if( sEOF == STL_TRUE )
        {
            /* 더이상의 Group이 없음. 즉, Inner Table의 Scan이 종료 */
            sExeSortMergeJoin->mIsGroupEOF = STL_TRUE;
            QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
            STL_THROW( RAMP_NO_DATA );
        }
        else
        {
            QLNX_RESET_CACHE_BLOCK_INFO( sInnerCacheBlockInfo );
        }

        /* Group Next의 경우 반드시 Fetch를 수행해야 한다. */
        sInnerCacheBlockInfo->mNeedScan = STL_TRUE;
        sInnerCacheBlockInfo->mCurrBlockIdx = 0;

        sExeSortMergeJoin->mIsRightMatched = STL_TRUE;
    }


    STL_RAMP( RAMP_FULL_DATA );
    STL_RAMP( RAMP_NO_DATA );

    if( QLNX_IS_END_OF_TABLE_FETCH( sInnerCacheBlockInfo ) &&
        (sExeSortMergeJoin->mIsGroupEOF == STL_TRUE) &&
        QLNX_IS_END_OF_TABLE_FETCH( sOuterCacheBlockInfo ) )
    {
        sEOF = STL_TRUE;
        sExeSortMergeJoin->mIsEOF = STL_TRUE;
    }
    else
    {
        sEOF = STL_FALSE;
        sExeSortMergeJoin->mIsEOF = STL_FALSE;
    }


    /**
     * OUTPUT 설정
     */

    *aEOF = sEOF;
    *aUsedBlockCnt = sExeSortMergeJoin->mCurrBlockIdx;

    /* Parent의 Fetch Node Info 설정 */
    aFetchNodeInfo->mFetchCnt = sRemainFetchCnt;
    aFetchNodeInfo->mSkipCnt  = sRemainSkipCnt;

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT(
        sExeSortMergeJoin->mJoinedColBlock,
        0,
        sExeSortMergeJoin->mCurrBlockIdx );

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeSortMergeJoin->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeSortMergeJoin->mCommonInfo.mFetchCallCount, 1 );
    QLL_OPT_ADD_RECORD_STAT_INFO( sExeSortMergeJoin->mCommonInfo.mFetchRecordStat,
                                  sExeSortMergeJoin->mCommonInfo.mResultColBlock,
                                  *aUsedBlockCnt,
                                  aEnv );

    sExeSortMergeJoin->mCommonInfo.mFetchRowCnt += *aUsedBlockCnt;
 
    /* Outer Column의 DstDataValue 정보 복구 */
    sOuterColumnBlockItem = sExeSortMergeJoin->mLeftOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );
    sOuterColumnBlockItem = sExeSortMergeJoin->mRightOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );


    return STL_SUCCESS;

    STL_FINISH;

    /* Outer Column의 DstDataValue 정보 복구 */
    sOuterColumnBlockItem = sExeSortMergeJoin->mLeftOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );
    sOuterColumnBlockItem = sExeSortMergeJoin->mRightOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeSortMergeJoin->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeSortMergeJoin->mCommonInfo.mFetchCallCount, 1 );

    return STL_FAILURE;
}


/**
 * @brief Instant를 이용한 Semi Join에 대한 SORT MERGE JOIN node를 Fetch한다.
 * @param[in]      aFetchNodeInfo  Fetch Node Info
 * @param[out]     aUsedBlockCnt   Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF            End Of Fetch 여부
 * @param[in]      aEnv            Environment
 *
 * @remark Fetch 수행시 호출된다.
 */
stlStatus qlnxFetchSortMergeJoin_Instant_Semi( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                               stlInt64              * aUsedBlockCnt,
                                               stlBool               * aEOF,
                                               qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode               = NULL;
    qlnxSortMergeJoin     * sExeSortMergeJoin       = NULL;
    qllOptimizationNode   * sInnerOptNode           = NULL;

    stlBool                 sEOF                    = STL_FALSE;

    stlInt64                sUsedBlockCnt           = 0;
    stlInt64                sRemainSkipCnt          = 0;
    stlInt64                sRemainFetchCnt         = 0;

    qlnxCacheBlockInfo    * sInnerCacheBlockInfo    = NULL;
    qlnxCacheBlockInfo    * sOuterCacheBlockInfo    = NULL;

    knlExprEvalInfo       * sWhereExprEvalInfo      = NULL;
    knlExprEvalInfo       * sJoinExprEvalInfo       = NULL;

    qlnxOuterColumnBlockItem  * sOuterColumnBlockItem   = NULL;

    dtlCompareResult            sCompareResult = DTL_COMPARISON_EQUAL;
    stlBool                     sHasNullValue;

    stlBool                 sIsComparedEqual            = STL_FALSE;
    stlBool                 sIsFirstFetch               = STL_FALSE;

    qlnxFetchNodeInfo       sLocalFetchInfo;

    QLL_OPT_DECLARE( stlTime sBeginFetchTime );
    QLL_OPT_DECLARE( stlTime sEndFetchTime );
    QLL_OPT_DECLARE( stlTime sFetchTime );

    QLL_OPT_DECLARE( stlTime sBeginExceptTime );
    QLL_OPT_DECLARE( stlTime sEndExceptTime );
    QLL_OPT_DECLARE( stlTime sExceptTime );

    QLL_OPT_SET_VALUE( sFetchTime, 0 );
    QLL_OPT_SET_VALUE( sExceptTime, 0 );


    /*****************************************
     * Parameter Validation
     ****************************************/

    STL_PARAM_VALIDATE( aFetchNodeInfo->mOptNode->mType == QLL_PLAN_NODE_MERGE_JOIN_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    QLL_OPT_CHECK_TIME( sBeginFetchTime );

    /*****************************************
     * 기본 정보 획득
     ****************************************/

    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aFetchNodeInfo->mDataArea,
                                        QLL_GET_OPT_NODE_IDX( aFetchNodeInfo->mOptNode ) );


    /**
     * SORT MERGE JOIN Execution Node 획득
     */

    sExeSortMergeJoin = (qlnxSortMergeJoin*) sExecNode->mExecNode;


    /**
     * Node 수행 여부 확인 
     */

    STL_DASSERT( *aEOF == STL_FALSE );
    STL_DASSERT( aFetchNodeInfo->mFetchCnt > 0);


    /**
     * Used Block Count 초기화
     */

    sUsedBlockCnt = 0;


    /**
     * Cache Block Info 정보 설정
     */

    sInnerCacheBlockInfo = &(sExeSortMergeJoin->mInnerCacheBlockInfo);
    sOuterCacheBlockInfo = &(sExeSortMergeJoin->mOuterCacheBlockInfo);


    /**
     * Evaluate Info 관련 정보 설정
     */

    sWhereExprEvalInfo  = &sExeSortMergeJoin->mWhereExprEvalInfo;
    sJoinExprEvalInfo   = &sExeSortMergeJoin->mJoinExprEvalInfo;


    /**
     * Local Fetch Info 초기화
     */
    
    QLNX_SET_LOCAL_FETCH_INFO( & sLocalFetchInfo, aFetchNodeInfo );


    /*****************************************
     * Node 수행 여부 확인
     ****************************************/
    STL_DASSERT( sExeSortMergeJoin->mIsEOF == STL_FALSE );
    if( sExeSortMergeJoin->mHasFalseFilter == STL_TRUE )
    {
        QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
        QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
        sExeSortMergeJoin->mIsFirstFetch = STL_FALSE;
        sExeSortMergeJoin->mIsEOF = STL_TRUE;

        STL_THROW( RAMP_NO_DATA );
    }
    else
    {
        /* Do Nothing */
    }


    /**
     * Fetch에 필요한 정보 설정
     */

    sRemainSkipCnt = aFetchNodeInfo->mSkipCnt;
    sRemainFetchCnt = ( aFetchNodeInfo->mFetchCnt < aFetchNodeInfo->mDataArea->mBlockAllocCnt
                        ? aFetchNodeInfo->mFetchCnt
                        : aFetchNodeInfo->mDataArea->mBlockAllocCnt );


    /**
     * Inner Opt Node 설정
     */

    sInnerOptNode =
        aFetchNodeInfo->mDataArea->mExecNodeList[sExeSortMergeJoin->mInnerCacheBlockInfo.mNodeIdx].mOptNode;


    /*****************************************
     * Fetch 수행
     ****************************************/

    STL_DASSERT( (QLNX_NEED_NEXT_FETCH( sOuterCacheBlockInfo ) == STL_FALSE) ||
                 (sOuterCacheBlockInfo->mEOF == STL_FALSE) );

    STL_RAMP( RAMP_READ_NEXT_ONE_ROW );

    sIsComparedEqual = STL_FALSE;

    /* Outer Table을 다음 레코드로 이동 */
    if( QLNX_NEED_NEXT_FETCH( sOuterCacheBlockInfo ) )
    {
        /* EOF 체크 */
        if( sOuterCacheBlockInfo->mEOF == STL_TRUE )
        {
            /* Outer Table의 Scan 종료 */
            QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
            QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
            STL_THROW( RAMP_NO_DATA );
        }

        /* Outer Cache Block에 새로운 데이터를 Fetch */
        QLL_OPT_CHECK_TIME( sBeginExceptTime );

        sEOF = STL_FALSE;
        QLNX_FETCH_NODE( & sLocalFetchInfo,
                         sExeSortMergeJoin->mOuterCacheBlockInfo.mNodeIdx,
                         0, /* Start Idx */
                         0, /* Skip Count */
                         QLL_FETCH_COUNT_MAX, /* Fetch Count */
                         & sUsedBlockCnt,
                         & sEOF,
                         aEnv );

        QLL_OPT_CHECK_TIME( sEndExceptTime );
        QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

        if( sUsedBlockCnt == 0 )
        {
            QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
            QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
            STL_THROW( RAMP_NO_DATA );
        }
        else if( sEOF == STL_TRUE )
        {
            QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
        }
        else
        {
            /* Do Nothing */
        }

        sOuterCacheBlockInfo->mCurrBlockIdx = 0;
        sOuterCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;

        /* 처음 Fetch시에만 Group Reset을 수행한다. */
        if( sExeSortMergeJoin->mIsFirstFetch == STL_TRUE )
        {
            /* Outer Table의 Block이 바뀌었으므로 Inner Table의 Group을 Reset */
            QLL_OPT_CHECK_TIME( sBeginExceptTime );

            sEOF = STL_FALSE;
            STL_TRY( qlnxGroupInitSortJoinInstantAccess( sLocalFetchInfo.mTransID,
                                                         sLocalFetchInfo.mStmt,
                                                         sLocalFetchInfo.mDBCStmt,
                                                         sLocalFetchInfo.mSQLStmt,
                                                         sInnerOptNode,
                                                         sLocalFetchInfo.mDataArea,
                                                         &sEOF,
                                                         aEnv )
                     == STL_SUCCESS );

            QLL_OPT_CHECK_TIME( sEndExceptTime );
            QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

            sExeSortMergeJoin->mIsFirstFetch = STL_FALSE;
            if( sEOF == STL_TRUE )
            {
                /* 더 이상의 Group이 없음. 즉, Join할 레코드가 없음 */
                sExeSortMergeJoin->mIsGroupEOF = STL_TRUE;
                QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                STL_THROW( RAMP_NO_DATA );
            }
            else
            {
                QLNX_RESET_CACHE_BLOCK_INFO( sInnerCacheBlockInfo );
            }

            /* Group Init의 경우 반드시 Fetch를 수행해야 한다. */
            sInnerCacheBlockInfo->mNeedScan = STL_TRUE;
            sInnerCacheBlockInfo->mCurrBlockIdx = 0;
        }
        else
        {
            QLL_OPT_CHECK_TIME( sBeginExceptTime );

            sEOF = STL_FALSE;
            STL_TRY( qlnxGroupResetSortJoinInstantAccess( sLocalFetchInfo.mTransID,
                                                          sLocalFetchInfo.mStmt,
                                                          sLocalFetchInfo.mDBCStmt,
                                                          sLocalFetchInfo.mSQLStmt,
                                                          sInnerOptNode,
                                                          sLocalFetchInfo.mDataArea,
                                                          &sEOF,
                                                          aEnv )
                     == STL_SUCCESS );

            QLL_OPT_CHECK_TIME( sEndExceptTime );
            QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

            STL_DASSERT( sEOF == STL_FALSE );
            QLNX_RESET_CACHE_BLOCK_INFO( sInnerCacheBlockInfo );
            
            /* 새로운 Outer 데이터에 대해 수행하므로 반드시 Fetch를 수행해야 한다. */
            sInnerCacheBlockInfo->mNeedScan = STL_TRUE;
            sInnerCacheBlockInfo->mCurrBlockIdx = 0;
        }
    }
    else
    {
        /* Cache에서 읽을 Block Idx를 설정 */
        sOuterCacheBlockInfo->mCurrBlockIdx++;

        /* Outer Table의 현재 key가 이전 key랑 동일한지 파악 */
        QLNX_COMPARE_ONLY_OUTER_SORT_KEY( &(sExeSortMergeJoin->mSortKeyCompareList),
                                          sOuterCacheBlockInfo->mCurrBlockIdx,
                                          sHasNullValue,
                                          sCompareResult );

        STL_TRY_THROW( sHasNullValue == STL_FALSE, RAMP_READ_NEXT_ONE_ROW );

        if( sCompareResult == DTL_COMPARISON_EQUAL )
        {
            /* 이전과 같은 key이므로 Inner Table의 Group을 Reset */
            if( sInnerCacheBlockInfo->mNeedScan == STL_TRUE )
            {
                QLL_OPT_CHECK_TIME( sBeginExceptTime );

                sEOF = STL_FALSE;
                STL_TRY( qlnxGroupResetSortJoinInstantAccess( sLocalFetchInfo.mTransID,
                                                              sLocalFetchInfo.mStmt,
                                                              sLocalFetchInfo.mDBCStmt,
                                                              sLocalFetchInfo.mSQLStmt,
                                                              sInnerOptNode,
                                                              sLocalFetchInfo.mDataArea,
                                                              &sEOF,
                                                              aEnv )
                         == STL_SUCCESS );

                QLL_OPT_CHECK_TIME( sEndExceptTime );
                QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                STL_DASSERT( sEOF == STL_FALSE );
                QLNX_RESET_CACHE_BLOCK_INFO( sInnerCacheBlockInfo );
            }

            sIsComparedEqual = STL_TRUE;
        }
        else
        {
            /* 이전과 다른 key이므로 Inner Table의 Group을 Next로 이동 */
            QLL_OPT_CHECK_TIME( sBeginExceptTime );

            sEOF = STL_FALSE;
            STL_TRY( qlnxGroupNextSortJoinInstantAccess( sLocalFetchInfo.mTransID,
                                                         sLocalFetchInfo.mStmt,
                                                         sLocalFetchInfo.mDBCStmt,
                                                         sLocalFetchInfo.mSQLStmt,
                                                         sInnerOptNode,
                                                         sLocalFetchInfo.mDataArea,
                                                         &sEOF,
                                                         aEnv )
                     == STL_SUCCESS );

            QLL_OPT_CHECK_TIME( sEndExceptTime );
            QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

            if( sEOF == STL_TRUE )
            {
                /* 더이상의 Group이 없음. 즉, Inner Table의 Scan이 종료 */
                sExeSortMergeJoin->mIsGroupEOF = STL_TRUE;
                QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                STL_THROW( RAMP_NO_DATA );
            }
            else
            {
                QLNX_RESET_CACHE_BLOCK_INFO( sInnerCacheBlockInfo );
            }

            /* Group Next의 경우 반드시 Fetch를 수행해야 한다. */
            sInnerCacheBlockInfo->mNeedScan = STL_TRUE;
            sInnerCacheBlockInfo->mCurrBlockIdx = 0;
        }
    }

    sIsFirstFetch = STL_TRUE;


    /**
     * Outer Table과 Inner Table에 대해 하나씩 레코드 읽어가며 비교
     */

    while( 1 )
    {
        /* Outer Table의 현재 Key와 비교하여 동일한 그룹인지 파악 */
        if( sIsComparedEqual == STL_FALSE )
        {
            QLNX_COMPARE_SORT_KEY( &(sExeSortMergeJoin->mSortKeyCompareList),
                                   sOuterCacheBlockInfo->mCurrBlockIdx,
                                   sInnerCacheBlockInfo->mCurrBlockIdx,
                                   sCompareResult,
                                   aEnv );
        }

        if( sCompareResult == DTL_COMPARISON_EQUAL )
        {
            /* Fetch 수행 */
            if( QLNX_NEED_NEXT_FETCH( sInnerCacheBlockInfo ) &&
                sInnerCacheBlockInfo->mNeedScan == STL_FALSE )
            {
                /* Fetch를 다시할 필요가 없는 경우 Offset을 초기화한다. */
                sInnerCacheBlockInfo->mCurrBlockIdx = -1;
            }

            while( 1 )
            {
                /* Inner Table을 Fetch함 */
                if( QLNX_NEED_NEXT_FETCH( sInnerCacheBlockInfo ) )
                {
                    /* EOF 체크 */
                    STL_TRY_THROW( sInnerCacheBlockInfo->mEOF == STL_FALSE,
                                   RAMP_READ_NEXT_ONE_ROW );

                    /* Inner Cache Block에 새로운 데이터를 Fetch */
                    QLL_OPT_CHECK_TIME( sBeginExceptTime );

                    sEOF = STL_FALSE;
                    QLNX_FETCH_NODE( & sLocalFetchInfo,
                                     sExeSortMergeJoin->mInnerCacheBlockInfo.mNodeIdx,
                                     0, /* Start Idx */
                                     0, /* Skip Count */
                                     QLL_FETCH_COUNT_MAX, /* Fetch Count */
                                     & sUsedBlockCnt,
                                     & sEOF,
                                     aEnv );

                    QLL_OPT_CHECK_TIME( sEndExceptTime );
                    QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                    if( sUsedBlockCnt == 0 )
                    {
                        QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                        STL_THROW( RAMP_READ_NEXT_ONE_ROW );
                    }
                    else if( sEOF == STL_TRUE )
                    {
                        QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                        if( sIsFirstFetch == STL_TRUE )
                        {
                            sInnerCacheBlockInfo->mNeedScan = STL_FALSE;
                        }
                        else
                        {
                            /* Coverage : equi 조건만을 판단하는 경우 해당 코드는 들어올 수 없다.
                             *            하지만 anti-semi join에서 equi & non-equi 가
                             *            동시에 들어오는 버그 존재하기 때문에 ASSERT로 제어하지는 않는다. */
                            sInnerCacheBlockInfo->mNeedScan = STL_TRUE;
                        }
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                    sIsFirstFetch = STL_FALSE;

                    sInnerCacheBlockInfo->mCurrBlockIdx = 0;
                    sInnerCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;
                }
                else
                {
                    /* Cache에서 읽을 Block Idx를 설정 */
                    sInnerCacheBlockInfo->mCurrBlockIdx++;
                }

                /* Join Condition Logical Filter 평가 */
                if( sExeSortMergeJoin->mHasJoinCondition == STL_TRUE )
                {
                    /* Left Outer Column에 대한 Data Value 재설정 */
                    sOuterColumnBlockItem = sExeSortMergeJoin->mLeftOuterColumnBlockList.mHead;
                    QLNX_SET_OUTER_COLUMN_BLOCK_LIST( sOuterColumnBlockItem, aEnv );

                    /* Right Outer Column에 대한 Data Value 재설정 */
                    sOuterColumnBlockItem = sExeSortMergeJoin->mRightOuterColumnBlockList.mHead;
                    QLNX_SET_OUTER_COLUMN_BLOCK_LIST( sOuterColumnBlockItem, aEnv );

                    /* Join Condition 평가 */
                    sJoinExprEvalInfo->mBlockIdx = sExeSortMergeJoin->mCurrBlockIdx;
                    STL_TRY( knlEvaluateOneExpression( sJoinExprEvalInfo,
                                                       KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    if( !DTL_BOOLEAN_IS_TRUE( KNL_GET_BLOCK_DATA_VALUE(
                                                  sJoinExprEvalInfo->mResultBlock,
                                                  sExeSortMergeJoin->mCurrBlockIdx ) ) )
                    {
                        continue;
                    }
                }
                
                /* Outer Table의 데이터와 Inner Table의 데이터에 대하여 Sort Merge Join 수행 */
                if( sExeSortMergeJoin->mJoinedColBlock != NULL )
                {
                    /* Result Block에 데이터 복사 */
                    QLNX_SET_OUTER_VALUE_BLOCK( &(sExeSortMergeJoin->mValueBlockList),
                                                &(sExeSortMergeJoin->mOuterCacheBlockInfo.mCurrBlockIdx),
                                                aEnv );
                }

                /* Where Condition Logical Filter 평가 */
                if( sExeSortMergeJoin->mHasWhereCondition == STL_TRUE )
                {
                    sWhereExprEvalInfo->mBlockIdx = sExeSortMergeJoin->mCurrBlockIdx;
                    STL_TRY( knlEvaluateOneExpression( sWhereExprEvalInfo,
                                                       KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    if( !DTL_BOOLEAN_IS_TRUE(
                            KNL_GET_BLOCK_DATA_VALUE(
                                sWhereExprEvalInfo->mResultBlock,
                                sExeSortMergeJoin->mCurrBlockIdx ) ) )
                    {
                        continue;
                    }
                }


                /* Skip Count가 남았으면 Skip 수행 */
                if( sRemainSkipCnt > 0 )
                {
                    sRemainSkipCnt--;
                    break;
                }

                /* Row Block들을 Result Block에 복사 */
                QLNX_SET_ROW_BLOCK( &(sExeSortMergeJoin->mRowBlockList),
                                    sExeSortMergeJoin->mCurrBlockIdx,
                                    sOuterCacheBlockInfo->mCurrBlockIdx,
                                    sInnerCacheBlockInfo->mCurrBlockIdx );

                /* Fetch Count 체크 */
                sExeSortMergeJoin->mCurrBlockIdx++;
                sRemainFetchCnt --;

                QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );

                if( sRemainFetchCnt == 0 )
                {
                    sEOF = STL_FALSE;
                    sExeSortMergeJoin->mIsEOF = STL_FALSE;
                    STL_THROW( RAMP_FULL_DATA );
                }
                else
                {
                    break;
                }
            }

            QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );

            STL_THROW( RAMP_READ_NEXT_ONE_ROW );
        }
        else if( sCompareResult == DTL_COMPARISON_LESS )
        {
            /* Outer Table을 다음 레코드로 이동 */
            if( QLNX_NEED_NEXT_FETCH( sOuterCacheBlockInfo ) )
            {
                /* EOF 체크 */
                if( sOuterCacheBlockInfo->mEOF == STL_TRUE )
                {
                    /* Outer Table의 Scan 종료 */
                    QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                    QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                    STL_THROW( RAMP_NO_DATA );
                }

                /* Outer Cache Block에 새로운 데이터를 Fetch */
                QLL_OPT_CHECK_TIME( sBeginExceptTime );

                sEOF = STL_FALSE;
                QLNX_FETCH_NODE( & sLocalFetchInfo,
                                 sExeSortMergeJoin->mOuterCacheBlockInfo.mNodeIdx,
                                 0, /* Start Idx */
                                 0, /* Skip Count */
                                 QLL_FETCH_COUNT_MAX, /* Fetch Count */
                                 & sUsedBlockCnt,
                                 & sEOF,
                                 aEnv );

                QLL_OPT_CHECK_TIME( sEndExceptTime );
                QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                if( sUsedBlockCnt == 0 )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                    QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                    STL_THROW( RAMP_NO_DATA );
                }
                else if( sEOF == STL_TRUE )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                }
                else
                {
                    /* Do Nothing */
                }

                sOuterCacheBlockInfo->mCurrBlockIdx = 0;
                sOuterCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;
            }
            else
            {
                /* Cache에서 읽을 Block Idx를 설정 */
                sOuterCacheBlockInfo->mCurrBlockIdx++;
            }
        }
        else
        {
            /* Inner Table을 다음 그룹으로 이동 */
            QLL_OPT_CHECK_TIME( sBeginExceptTime );

            sEOF = STL_FALSE;
            STL_TRY( qlnxGroupNextSortJoinInstantAccess( sLocalFetchInfo.mTransID,
                                                         sLocalFetchInfo.mStmt,
                                                         sLocalFetchInfo.mDBCStmt,
                                                         sLocalFetchInfo.mSQLStmt,
                                                         sInnerOptNode,
                                                         sLocalFetchInfo.mDataArea,
                                                         &sEOF,
                                                         aEnv )
                     == STL_SUCCESS );

            QLL_OPT_CHECK_TIME( sEndExceptTime );
            QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

            if( sEOF == STL_TRUE )
            {
                /* 더이상의 Group이 없음. 즉, Inner Table의 Scan이 종료 */
                sExeSortMergeJoin->mIsGroupEOF = STL_TRUE;
                QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                STL_THROW( RAMP_NO_DATA );
            }
            else
            {
                QLNX_RESET_CACHE_BLOCK_INFO( sInnerCacheBlockInfo );
            }

            /* Group Next의 경우 반드시 Fetch를 수행해야 한다. */
            sInnerCacheBlockInfo->mNeedScan = STL_TRUE;
            sInnerCacheBlockInfo->mCurrBlockIdx = 0;
        }
    }

    STL_RAMP( RAMP_NO_DATA );
    STL_RAMP( RAMP_FULL_DATA );

    if( QLNX_IS_END_OF_TABLE_FETCH( sInnerCacheBlockInfo ) &&
        QLNX_IS_END_OF_TABLE_FETCH( sOuterCacheBlockInfo ) )
    {
        sEOF = STL_TRUE;
        sExeSortMergeJoin->mIsEOF = STL_TRUE;
    }
    else
    {
        sEOF = STL_FALSE;
        sExeSortMergeJoin->mIsEOF = STL_FALSE;
    }


    /**
     * OUTPUT 설정
     */

    *aEOF = sEOF;
    *aUsedBlockCnt = sExeSortMergeJoin->mCurrBlockIdx;

    /* Parent의 Fetch Node Info 설정 */
    aFetchNodeInfo->mFetchCnt = sRemainFetchCnt;
    aFetchNodeInfo->mSkipCnt  = sRemainSkipCnt;

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT(
        sExeSortMergeJoin->mJoinedColBlock,
        0,
        sExeSortMergeJoin->mCurrBlockIdx );

    QLNX_SET_USED_BLOCK_SIZE_ON_ROW_BLOCK( &(sExeSortMergeJoin->mRowBlockList),
                                           sExeSortMergeJoin->mCurrBlockIdx );

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeSortMergeJoin->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeSortMergeJoin->mCommonInfo.mFetchCallCount, 1 );
    QLL_OPT_ADD_RECORD_STAT_INFO( sExeSortMergeJoin->mCommonInfo.mFetchRecordStat,
                                  sExeSortMergeJoin->mCommonInfo.mResultColBlock,
                                  *aUsedBlockCnt,
                                  aEnv );

    sExeSortMergeJoin->mCommonInfo.mFetchRowCnt += *aUsedBlockCnt;
 
    /* Outer Column의 DstDataValue 정보 복구 */
    sOuterColumnBlockItem = sExeSortMergeJoin->mLeftOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );
    sOuterColumnBlockItem = sExeSortMergeJoin->mRightOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );


    return STL_SUCCESS;

    STL_FINISH;

    /* Outer Column의 DstDataValue 정보 복구 */
    sOuterColumnBlockItem = sExeSortMergeJoin->mLeftOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );
    sOuterColumnBlockItem = sExeSortMergeJoin->mRightOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeSortMergeJoin->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeSortMergeJoin->mCommonInfo.mFetchCallCount, 1 );

    return STL_FAILURE;
}


/**
 * @brief Instant를 이용한 Anti Semi Join에 대한 SORT MERGE JOIN node를 Fetch한다.
 * @param[in]      aFetchNodeInfo  Fetch Node Info
 * @param[out]     aUsedBlockCnt   Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF            End Of Fetch 여부
 * @param[in]      aEnv            Environment
 *
 * @remark Fetch 수행시 호출된다.
 */
stlStatus qlnxFetchSortMergeJoin_Instant_AntiSemi( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                                   stlInt64              * aUsedBlockCnt,
                                                   stlBool               * aEOF,
                                                   qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode               = NULL;
    qlnxSortMergeJoin     * sExeSortMergeJoin       = NULL;
    qllOptimizationNode   * sInnerOptNode           = NULL;

    stlBool                 sEOF                    = STL_FALSE;

    stlInt64                sUsedBlockCnt           = 0;
    stlInt64                sRemainSkipCnt          = 0;
    stlInt64                sRemainFetchCnt         = 0;

    qlnxCacheBlockInfo    * sInnerCacheBlockInfo    = NULL;
    qlnxCacheBlockInfo    * sOuterCacheBlockInfo    = NULL;

    knlExprEvalInfo       * sWhereExprEvalInfo      = NULL;
    knlExprEvalInfo       * sJoinExprEvalInfo       = NULL;

    qlnxOuterColumnBlockItem  * sOuterColumnBlockItem   = NULL;

    dtlCompareResult            sCompareResult = DTL_COMPARISON_EQUAL;
    stlBool                     sHasNullValue;

    stlBool                 sIsComparedEqual            = STL_FALSE;
    stlBool                 sIsFirstFetch               = STL_FALSE;
    stlBool                 sIsMatched                  = STL_FALSE;

    qlnxFetchNodeInfo       sLocalFetchInfo;

    QLL_OPT_DECLARE( stlTime sBeginFetchTime );
    QLL_OPT_DECLARE( stlTime sEndFetchTime );
    QLL_OPT_DECLARE( stlTime sFetchTime );

    QLL_OPT_DECLARE( stlTime sBeginExceptTime );
    QLL_OPT_DECLARE( stlTime sEndExceptTime );
    QLL_OPT_DECLARE( stlTime sExceptTime );

    QLL_OPT_SET_VALUE( sFetchTime, 0 );
    QLL_OPT_SET_VALUE( sExceptTime, 0 );


    /*****************************************
     * Parameter Validation
     ****************************************/

    STL_PARAM_VALIDATE( aFetchNodeInfo->mOptNode->mType == QLL_PLAN_NODE_MERGE_JOIN_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    QLL_OPT_CHECK_TIME( sBeginFetchTime );

    /*****************************************
     * 기본 정보 획득
     ****************************************/

    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aFetchNodeInfo->mDataArea,
                                        QLL_GET_OPT_NODE_IDX( aFetchNodeInfo->mOptNode ) );


    /**
     * SORT MERGE JOIN Execution Node 획득
     */

    sExeSortMergeJoin = (qlnxSortMergeJoin*) sExecNode->mExecNode;


    /**
     * Node 수행 여부 확인 
     */

    STL_DASSERT( *aEOF == STL_FALSE );
    STL_DASSERT( aFetchNodeInfo->mFetchCnt > 0);


    /**
     * Used Block Count 초기화
     */

    sUsedBlockCnt = 0;


    /**
     * Cache Block Info 정보 설정
     */

    sInnerCacheBlockInfo = &(sExeSortMergeJoin->mInnerCacheBlockInfo);
    sOuterCacheBlockInfo = &(sExeSortMergeJoin->mOuterCacheBlockInfo);


    /**
     * Evaluate Info 관련 정보 설정
     */

    sWhereExprEvalInfo  = &sExeSortMergeJoin->mWhereExprEvalInfo;
    sJoinExprEvalInfo   = &sExeSortMergeJoin->mJoinExprEvalInfo;


    /**
     * Local Fetch Info 초기화
     */
    
    QLNX_SET_LOCAL_FETCH_INFO( & sLocalFetchInfo, aFetchNodeInfo );

    
    /*****************************************
     * Node 수행 여부 확인
     ****************************************/
    
    STL_DASSERT( sExeSortMergeJoin->mIsEOF == STL_FALSE );
    if( sExeSortMergeJoin->mHasFalseFilter == STL_TRUE )
    {
        sEOF = STL_TRUE;

        QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
        QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
        sExeSortMergeJoin->mIsFirstFetch = STL_FALSE;
        sExeSortMergeJoin->mIsEOF = STL_TRUE;

        STL_THROW( RAMP_NO_DATA );
    }
    else
    {
        /* Do Nothing */
    }


    /**
     * Fetch에 필요한 정보 설정
     */

    sRemainSkipCnt = aFetchNodeInfo->mSkipCnt;
    sRemainFetchCnt = ( aFetchNodeInfo->mFetchCnt < aFetchNodeInfo->mDataArea->mBlockAllocCnt
                        ? aFetchNodeInfo->mFetchCnt
                        : aFetchNodeInfo->mDataArea->mBlockAllocCnt );


    /**
     * Inner Opt Node 설정
     */

    sInnerOptNode =
        aFetchNodeInfo->mDataArea->mExecNodeList[sExeSortMergeJoin->mInnerCacheBlockInfo.mNodeIdx].mOptNode;


    /*****************************************
     * Fetch 수행
     ****************************************/

    /* Left Node가 EOF인지 판별 */
    if( QLNX_NEED_NEXT_FETCH( sOuterCacheBlockInfo ) &&
        (sOuterCacheBlockInfo->mEOF == STL_TRUE) )
    {
        /* Outer Table의 Scan 종료 */
        QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
        QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
        STL_THROW( RAMP_NO_DATA );
    }

    /* Right Node가 EOF인지 판별 */
    if( (sExeSortMergeJoin->mIsGroupEOF == STL_TRUE) &&
        QLNX_NEED_NEXT_FETCH( sInnerCacheBlockInfo ) &&
        (sInnerCacheBlockInfo->mEOF == STL_TRUE) )
    {
        /* Inner Table의 Scan 종료 */
        STL_THROW( RAMP_END_OF_RIGHT );
    }

    STL_RAMP( RAMP_READ_NEXT_ONE_ROW );

    sIsComparedEqual = STL_FALSE;

    /* Outer Table을 다음 레코드로 이동 */
    if( QLNX_NEED_NEXT_FETCH( sOuterCacheBlockInfo ) )
    {
        /* EOF 체크 */
        if( sOuterCacheBlockInfo->mEOF == STL_TRUE )
        {
            /* Outer Table의 Scan 종료 */
            QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
            QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
            STL_THROW( RAMP_NO_DATA );
        }

        /* Outer Cache Block에 새로운 데이터를 Fetch */
        QLL_OPT_CHECK_TIME( sBeginExceptTime );

        sEOF = STL_FALSE;
        QLNX_FETCH_NODE( & sLocalFetchInfo,
                         sExeSortMergeJoin->mOuterCacheBlockInfo.mNodeIdx,
                         0, /* Start Idx */
                         0, /* Skip Count */
                         QLL_FETCH_COUNT_MAX, /* Fetch Count */
                         & sUsedBlockCnt,
                         & sEOF,
                         aEnv );

        QLL_OPT_CHECK_TIME( sEndExceptTime );
        QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

        if( sUsedBlockCnt == 0 )
        {
            QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
            QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
            STL_THROW( RAMP_NO_DATA );
        }
        else if( sEOF == STL_TRUE )
        {
            QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
        }
        else
        {
            /* Do Nothing */
        }

        sOuterCacheBlockInfo->mCurrBlockIdx = 0;
        sOuterCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;

        /* 처음 Fetch시에만 Group Reset을 수행한다. */
        if( sExeSortMergeJoin->mIsFirstFetch == STL_TRUE )
        {
            /* Outer Table의 Block이 바뀌었으므로 Inner Table의 Group을 Reset */
            QLL_OPT_CHECK_TIME( sBeginExceptTime );

            sEOF = STL_FALSE;
            STL_TRY( qlnxGroupInitSortJoinInstantAccess( sLocalFetchInfo.mTransID,
                                                         sLocalFetchInfo.mStmt,
                                                         sLocalFetchInfo.mDBCStmt,
                                                         sLocalFetchInfo.mSQLStmt,
                                                         sInnerOptNode,
                                                         sLocalFetchInfo.mDataArea,
                                                         &sEOF,
                                                         aEnv )
                     == STL_SUCCESS );

            QLL_OPT_CHECK_TIME( sEndExceptTime );
            QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

            sExeSortMergeJoin->mIsFirstFetch = STL_FALSE;
            if( sEOF == STL_TRUE )
            {
                /* 더이상 Group Fetch할 것이 없다. */
                sExeSortMergeJoin->mIsGroupEOF = STL_TRUE;
                sOuterCacheBlockInfo->mCurrBlockIdx--;
                QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                STL_THROW( RAMP_END_OF_RIGHT );
            }
            else
            {
                QLNX_RESET_CACHE_BLOCK_INFO( sInnerCacheBlockInfo );
            }

            /* Group Init의 경우 반드시 Fetch를 수행해야 한다. */
            sInnerCacheBlockInfo->mNeedScan = STL_TRUE;
            sInnerCacheBlockInfo->mCurrBlockIdx = 0;
        }
        else
        {
            QLL_OPT_CHECK_TIME( sBeginExceptTime );

            sEOF = STL_FALSE;
            STL_TRY( qlnxGroupResetSortJoinInstantAccess( sLocalFetchInfo.mTransID,
                                                          sLocalFetchInfo.mStmt,
                                                          sLocalFetchInfo.mDBCStmt,
                                                          sLocalFetchInfo.mSQLStmt,
                                                          sInnerOptNode,
                                                          sLocalFetchInfo.mDataArea,
                                                          &sEOF,
                                                          aEnv )
                     == STL_SUCCESS );

            QLL_OPT_CHECK_TIME( sEndExceptTime );
            QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

            STL_DASSERT( sEOF == STL_FALSE );
            QLNX_RESET_CACHE_BLOCK_INFO( sInnerCacheBlockInfo );

            /* 새로운 Outer 데이터에 대해 수행하므로 반드시 Fetch를 수행해야 한다. */
            sInnerCacheBlockInfo->mNeedScan = STL_TRUE;
            sInnerCacheBlockInfo->mCurrBlockIdx = 0;
        }
    }
    else
    {
        /* Cache에서 읽을 Block Idx를 설정 */
        sOuterCacheBlockInfo->mCurrBlockIdx++;

        /* Outer Table의 현재 key가 이전 key랑 동일한지 파악 */
        QLNX_COMPARE_ONLY_OUTER_SORT_KEY( &(sExeSortMergeJoin->mSortKeyCompareList),
                                          sOuterCacheBlockInfo->mCurrBlockIdx,
                                          sHasNullValue,
                                          sCompareResult );

        STL_TRY_THROW( sHasNullValue == STL_FALSE, RAMP_READ_NEXT_ONE_ROW );

        if( sCompareResult == DTL_COMPARISON_EQUAL )
        {
            /* 이전과 같은 key이므로 Inner Table의 Group을 Reset */
            if( sInnerCacheBlockInfo->mNeedScan == STL_TRUE )
            {
                QLL_OPT_CHECK_TIME( sBeginExceptTime );

                sEOF = STL_FALSE;
                STL_TRY( qlnxGroupResetSortJoinInstantAccess( sLocalFetchInfo.mTransID,
                                                              sLocalFetchInfo.mStmt,
                                                              sLocalFetchInfo.mDBCStmt,
                                                              sLocalFetchInfo.mSQLStmt,
                                                              sInnerOptNode,
                                                              sLocalFetchInfo.mDataArea,
                                                              &sEOF,
                                                              aEnv )
                         == STL_SUCCESS );

                QLL_OPT_CHECK_TIME( sEndExceptTime );
                QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                STL_DASSERT( sEOF == STL_FALSE );
                if( sInnerCacheBlockInfo->mNeedScan == STL_TRUE )
                {
                    QLNX_RESET_CACHE_BLOCK_INFO( sInnerCacheBlockInfo );
                }
            }

            sIsComparedEqual = STL_TRUE;
        }
        else
        {
            /* 이전과 다른 key이므로 Inner Table의 Group을 Next로 이동 */
            QLL_OPT_CHECK_TIME( sBeginExceptTime );

            sEOF = STL_FALSE;
            STL_TRY( qlnxGroupNextSortJoinInstantAccess( sLocalFetchInfo.mTransID,
                                                         sLocalFetchInfo.mStmt,
                                                         sLocalFetchInfo.mDBCStmt,
                                                         sLocalFetchInfo.mSQLStmt,
                                                         sInnerOptNode,
                                                         sLocalFetchInfo.mDataArea,
                                                         &sEOF,
                                                         aEnv )
                     == STL_SUCCESS );

            QLL_OPT_CHECK_TIME( sEndExceptTime );
            QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

            if( sEOF == STL_TRUE )
            {
                sExeSortMergeJoin->mIsGroupEOF = STL_TRUE;
                sOuterCacheBlockInfo->mCurrBlockIdx--;
                QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                STL_THROW( RAMP_END_OF_RIGHT );
            }
            else
            {
                QLNX_RESET_CACHE_BLOCK_INFO( sInnerCacheBlockInfo );
            }

            /* Group Next의 경우 반드시 Fetch를 수행해야 한다. */
            sInnerCacheBlockInfo->mNeedScan = STL_TRUE;
            sInnerCacheBlockInfo->mCurrBlockIdx = 0;
        }
    }

    sIsFirstFetch = STL_TRUE;


    /**
     * Outer Table과 Inner Table에 대해 하나씩 레코드 읽어가며 비교
     */

    while( 1 )
    {
        /* Outer Table의 현재 Key와 비교하여 동일한 그룹인지 파악 */
        if( sIsComparedEqual == STL_FALSE )
        {
            QLNX_COMPARE_SORT_KEY( &(sExeSortMergeJoin->mSortKeyCompareList),
                                   sOuterCacheBlockInfo->mCurrBlockIdx,
                                   sInnerCacheBlockInfo->mCurrBlockIdx,
                                   sCompareResult,
                                   aEnv );
        }

        if( sCompareResult == DTL_COMPARISON_EQUAL )
        {
            /* Fetch 수행 */
            if( QLNX_NEED_NEXT_FETCH( sInnerCacheBlockInfo ) &&
                sInnerCacheBlockInfo->mNeedScan == STL_FALSE )
            {
                /* Fetch를 다시할 필요가 없는 경우 Offset을 초기화한다. */
                sInnerCacheBlockInfo->mCurrBlockIdx = -1;
            }

            sIsMatched = STL_FALSE;
            while( 1 )
            {
                /* Inner Table을 Fetch함 */
                if( QLNX_NEED_NEXT_FETCH( sInnerCacheBlockInfo ) )
                {
                    /* EOF 체크 */
                    if( sInnerCacheBlockInfo->mEOF == STL_TRUE )
                    {
                        break;
                    }

                    /* Inner Cache Block에 새로운 데이터를 Fetch */
                    QLL_OPT_CHECK_TIME( sBeginExceptTime );

                    sEOF = STL_FALSE;
                    QLNX_FETCH_NODE( & sLocalFetchInfo,
                                     sExeSortMergeJoin->mInnerCacheBlockInfo.mNodeIdx,
                                     0, /* Start Idx */
                                     0, /* Skip Count */
                                     QLL_FETCH_COUNT_MAX, /* Fetch Count */
                                     & sUsedBlockCnt,
                                     & sEOF,
                                     aEnv );

                    QLL_OPT_CHECK_TIME( sEndExceptTime );
                    QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                    if( sUsedBlockCnt == 0 )
                    {
                        QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                        break;
                    }
                    else if( sEOF == STL_TRUE )
                    {
                        QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                        if( sIsFirstFetch == STL_TRUE )
                        {
                            sInnerCacheBlockInfo->mNeedScan = STL_FALSE;
                        }
                        else
                        {
                            /* Coverage : equi 조건만을 판단하는 경우 해당 코드는 들어올 수 없다.
                             *            하지만 anti-semi join에서 equi & non-equi 가
                             *            동시에 들어오는 버그 존재하기 때문에 ASSERT로 제어하지는 않는다. */
                            sInnerCacheBlockInfo->mNeedScan = STL_TRUE;
                        }
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                    sIsFirstFetch = STL_FALSE;

                    sInnerCacheBlockInfo->mCurrBlockIdx = 0;
                    sInnerCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;
                }
                else
                {
                    /* Cache에서 읽을 Block Idx를 설정 */
                    sInnerCacheBlockInfo->mCurrBlockIdx++;
                }

                /* Join Condition Logical Filter 평가 */
                if( sExeSortMergeJoin->mHasJoinCondition == STL_TRUE )
                {
                    /* Left Outer Column에 대한 Data Value 재설정 */
                    sOuterColumnBlockItem = sExeSortMergeJoin->mLeftOuterColumnBlockList.mHead;
                    QLNX_SET_OUTER_COLUMN_BLOCK_LIST( sOuterColumnBlockItem, aEnv );

                    /* Right Outer Column에 대한 Data Value 재설정 */
                    sOuterColumnBlockItem = sExeSortMergeJoin->mRightOuterColumnBlockList.mHead;
                    QLNX_SET_OUTER_COLUMN_BLOCK_LIST( sOuterColumnBlockItem, aEnv );

                    /* Join Condition 평가 */
                    sJoinExprEvalInfo->mBlockIdx = sExeSortMergeJoin->mCurrBlockIdx;
                    STL_TRY( knlEvaluateOneExpression( sJoinExprEvalInfo,
                                                       KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    if( !DTL_BOOLEAN_IS_TRUE( KNL_GET_BLOCK_DATA_VALUE(
                                                  sJoinExprEvalInfo->mResultBlock,
                                                  sExeSortMergeJoin->mCurrBlockIdx ) ) )
                    {
                        continue;
                    }
                }
                
                if( sExeSortMergeJoin->mJoinedColBlock != NULL )
                {
                    /* Result Block에 데이터 복사 */
                    QLNX_SET_OUTER_VALUE_BLOCK( &(sExeSortMergeJoin->mValueBlockList),
                                                &(sExeSortMergeJoin->mOuterCacheBlockInfo.mCurrBlockIdx),
                                                aEnv );
                }

                /* Where Condition Logical Filter 평가 */
                if( sExeSortMergeJoin->mHasWhereCondition == STL_TRUE )
                {
                    sWhereExprEvalInfo->mBlockIdx = sExeSortMergeJoin->mCurrBlockIdx;
                    STL_TRY( knlEvaluateOneExpression( sWhereExprEvalInfo,
                                                       KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    if( !DTL_BOOLEAN_IS_TRUE(
                            KNL_GET_BLOCK_DATA_VALUE(
                                sWhereExprEvalInfo->mResultBlock,
                                sExeSortMergeJoin->mCurrBlockIdx ) ) )
                    {
                        continue;
                    }
                }

                sIsMatched = STL_TRUE;

                QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );

                break;
            }

            if( sIsMatched == STL_FALSE )
            {
                /* Skip Count가 남았으면 Skip 수행 */
                if( sRemainSkipCnt > 0 )
                {
                    sRemainSkipCnt--;
                }
                else
                {
                    /* Row Block들을 Result Block에 복사 */
                    QLNX_SET_ROW_BLOCK( &(sExeSortMergeJoin->mRowBlockList),
                                        sExeSortMergeJoin->mCurrBlockIdx,
                                        sOuterCacheBlockInfo->mCurrBlockIdx,
                                        sInnerCacheBlockInfo->mCurrBlockIdx );

                    /* Fetch Count 체크 */
                    sExeSortMergeJoin->mCurrBlockIdx++;
                    sRemainFetchCnt --;

                    if( sRemainFetchCnt == 0 )
                    {
                        sEOF = STL_FALSE;
                        sExeSortMergeJoin->mIsEOF = STL_FALSE;
                        STL_THROW( RAMP_FULL_DATA );
                    }
                }
            }

            STL_THROW( RAMP_READ_NEXT_ONE_ROW );
        }
        else if( sCompareResult == DTL_COMPARISON_LESS )
        {
            /* 현재 Outer Table의 데이터는 결과로 보낼 데이터임 */
            if( sExeSortMergeJoin->mJoinedColBlock != NULL )
            {
                /* Result Block에 데이터 복사 */
                QLNX_SET_OUTER_VALUE_BLOCK( &(sExeSortMergeJoin->mValueBlockList),
                                            &(sExeSortMergeJoin->mOuterCacheBlockInfo.mCurrBlockIdx),
                                            aEnv );
            }

            /* Where Condition Logical Filter 평가 */
            if( sExeSortMergeJoin->mHasWhereCondition == STL_TRUE )
            {
                sWhereExprEvalInfo->mBlockIdx = sExeSortMergeJoin->mCurrBlockIdx;
                STL_TRY( knlEvaluateOneExpression( sWhereExprEvalInfo,
                                                   KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                if( !DTL_BOOLEAN_IS_TRUE(
                        KNL_GET_BLOCK_DATA_VALUE(
                            sWhereExprEvalInfo->mResultBlock,
                            sExeSortMergeJoin->mCurrBlockIdx ) ) )
                {
                    STL_THROW( RAMP_SKIP_OUTER_RESULT );
                }
            }

            /* Skip Count가 남았으면 Skip 수행 */
            if( sRemainSkipCnt > 0 )
            {
                sRemainSkipCnt--;
            }
            else
            {
                /* Row Block들을 Result Block에 복사 */
                QLNX_SET_ROW_BLOCK( &(sExeSortMergeJoin->mRowBlockList),
                                    sExeSortMergeJoin->mCurrBlockIdx,
                                    sOuterCacheBlockInfo->mCurrBlockIdx,
                                    sInnerCacheBlockInfo->mCurrBlockIdx );

                /* Fetch Count 체크 */
                sExeSortMergeJoin->mCurrBlockIdx++;
                sRemainFetchCnt --;

                if( sRemainFetchCnt == 0 )
                {
                    sEOF = STL_FALSE;
                    sExeSortMergeJoin->mIsEOF = STL_FALSE;
                    STL_THROW( RAMP_FULL_DATA );
                }
            }

            STL_RAMP( RAMP_SKIP_OUTER_RESULT );

            /* Outer Table을 다음 레코드로 이동 */
            if( QLNX_NEED_NEXT_FETCH( sOuterCacheBlockInfo ) )
            {
                /* EOF 체크 */
                if( sOuterCacheBlockInfo->mEOF == STL_TRUE )
                {
                    /* Outer Table의 Scan 종료 */
                    QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                    QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                    STL_THROW( RAMP_NO_DATA );
                }

                /* Outer Cache Block에 새로운 데이터를 Fetch */
                QLL_OPT_CHECK_TIME( sBeginExceptTime );

                sEOF = STL_FALSE;
                QLNX_FETCH_NODE( & sLocalFetchInfo,
                                 sExeSortMergeJoin->mOuterCacheBlockInfo.mNodeIdx,
                                 0, /* Start Idx */
                                 0, /* Skip Count */
                                 QLL_FETCH_COUNT_MAX, /* Fetch Count */
                                 & sUsedBlockCnt,
                                 & sEOF,
                                 aEnv );

                QLL_OPT_CHECK_TIME( sEndExceptTime );
                QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                if( sUsedBlockCnt == 0 )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                    QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                    STL_THROW( RAMP_NO_DATA );
                }
                else if( sEOF == STL_TRUE )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                }
                else
                {
                    /* Do Nothing */
                }

                sOuterCacheBlockInfo->mCurrBlockIdx = 0;
                sOuterCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;
            }
            else
            {
                /* Cache에서 읽을 Block Idx를 설정 */
                sOuterCacheBlockInfo->mCurrBlockIdx++;
            }
        }
        else
        {
            /* Inner Table을 다음 그룹으로 이동 */
            QLL_OPT_CHECK_TIME( sBeginExceptTime );

            sEOF = STL_FALSE;
            STL_TRY( qlnxGroupNextSortJoinInstantAccess( sLocalFetchInfo.mTransID,
                                                         sLocalFetchInfo.mStmt,
                                                         sLocalFetchInfo.mDBCStmt,
                                                         sLocalFetchInfo.mSQLStmt,
                                                         sInnerOptNode,
                                                         sLocalFetchInfo.mDataArea,
                                                         &sEOF,
                                                         aEnv )
                     == STL_SUCCESS );

            QLL_OPT_CHECK_TIME( sEndExceptTime );
            QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

            if( sEOF == STL_TRUE )
            {
                sExeSortMergeJoin->mIsGroupEOF = STL_TRUE;
                sOuterCacheBlockInfo->mCurrBlockIdx--;
                QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                STL_THROW( RAMP_END_OF_RIGHT );
            }
            else
            {
                QLNX_RESET_CACHE_BLOCK_INFO( sInnerCacheBlockInfo );
            }

            /* Group Next의 경우 반드시 Fetch를 수행해야 한다. */
            sInnerCacheBlockInfo->mNeedScan = STL_TRUE;
            sInnerCacheBlockInfo->mCurrBlockIdx = 0;
        }
    }
    
    STL_RAMP( RAMP_END_OF_RIGHT );


    while( 1 )
    {
        /**
         * Outer Table의 Cache Block에 아직 읽을 데이터가 남았는지 체크
         */

        if( QLNX_NEED_NEXT_FETCH( sOuterCacheBlockInfo ) )
        {
            /* EOF 체크 */
            STL_TRY_THROW( sOuterCacheBlockInfo->mEOF == STL_FALSE,
                           RAMP_NO_DATA );


            /* Outer Cache Block에 새로운 데이터를 Fetch */
            QLL_OPT_CHECK_TIME( sBeginExceptTime );

            sEOF = STL_FALSE;
            QLNX_FETCH_NODE( & sLocalFetchInfo,
                             sExeSortMergeJoin->mOuterCacheBlockInfo.mNodeIdx,
                             0, /* Start Idx */
                             0, /* Skip Count */
                             QLL_FETCH_COUNT_MAX, /* Fetch Count */
                             & sUsedBlockCnt,
                             & sEOF,
                             aEnv );

            QLL_OPT_CHECK_TIME( sEndExceptTime );
            QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

            if( sUsedBlockCnt == 0 )
            {
                QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                STL_THROW( RAMP_NO_DATA );
            }
            else if( sEOF == STL_TRUE )
            {
                QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
            }
            else
            {
                /* Do Nothing */
            }

            sOuterCacheBlockInfo->mCurrBlockIdx = 0;
            sOuterCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;
        }
        else
        {
            /**
             * Cache에서 읽을 Block Idx를 설정
             */

            sOuterCacheBlockInfo->mCurrBlockIdx++;
        }

        if( sExeSortMergeJoin->mJoinedColBlock != NULL )
        {
            /* Result Block에 데이터 복사 */
            QLNX_SET_OUTER_VALUE_BLOCK( &(sExeSortMergeJoin->mValueBlockList),
                                        &(sExeSortMergeJoin->mOuterCacheBlockInfo.mCurrBlockIdx),
                                        aEnv );
        }

        /* Where Condition Logical Filter 평가 */
        if( sExeSortMergeJoin->mHasWhereCondition == STL_TRUE )
        {
            sWhereExprEvalInfo->mBlockIdx = sExeSortMergeJoin->mCurrBlockIdx;
            STL_TRY( knlEvaluateOneExpression( sWhereExprEvalInfo,
                                               KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            if( !DTL_BOOLEAN_IS_TRUE(
                    KNL_GET_BLOCK_DATA_VALUE(
                        sWhereExprEvalInfo->mResultBlock,
                        sExeSortMergeJoin->mCurrBlockIdx ) ) )
            {
                continue;
            }
        }

        /* Skip Count가 남았으면 Skip 수행 */
        if( sRemainSkipCnt > 0 )
        {
            sRemainSkipCnt--;
        }
        else
        {
            /* Row Block들을 Result Block에 복사 */
            QLNX_SET_ROW_BLOCK( &(sExeSortMergeJoin->mRowBlockList),
                                sExeSortMergeJoin->mCurrBlockIdx,
                                sOuterCacheBlockInfo->mCurrBlockIdx,
                                sInnerCacheBlockInfo->mCurrBlockIdx );

            /* Fetch Count 체크 */
            sExeSortMergeJoin->mCurrBlockIdx++;
            sRemainFetchCnt --;

            if( sRemainFetchCnt == 0 )
            {
                sEOF = STL_FALSE;
                sExeSortMergeJoin->mIsEOF = STL_FALSE;
                STL_THROW( RAMP_FULL_DATA );
            }
        }
    }


    STL_RAMP( RAMP_NO_DATA );
    STL_RAMP( RAMP_FULL_DATA );

    if( QLNX_IS_END_OF_TABLE_FETCH( sInnerCacheBlockInfo ) &&
        QLNX_IS_END_OF_TABLE_FETCH( sOuterCacheBlockInfo ) )
    {
        sEOF = STL_TRUE;
        sExeSortMergeJoin->mIsEOF = STL_TRUE;
    }
    else
    {
        sEOF = STL_FALSE;
        sExeSortMergeJoin->mIsEOF = STL_FALSE;
    }


    /**
     * OUTPUT 설정
     */

    *aEOF = sEOF;
    *aUsedBlockCnt = sExeSortMergeJoin->mCurrBlockIdx;

    /* Parent의 Fetch Node Info 설정 */
    aFetchNodeInfo->mFetchCnt = sRemainFetchCnt;
    aFetchNodeInfo->mSkipCnt  = sRemainSkipCnt;

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT(
        sExeSortMergeJoin->mJoinedColBlock,
        0,
        sExeSortMergeJoin->mCurrBlockIdx );

    QLNX_SET_USED_BLOCK_SIZE_ON_ROW_BLOCK( &(sExeSortMergeJoin->mRowBlockList),
                                           sExeSortMergeJoin->mCurrBlockIdx );

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeSortMergeJoin->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeSortMergeJoin->mCommonInfo.mFetchCallCount, 1 );
    QLL_OPT_ADD_RECORD_STAT_INFO( sExeSortMergeJoin->mCommonInfo.mFetchRecordStat,
                                  sExeSortMergeJoin->mCommonInfo.mResultColBlock,
                                  *aUsedBlockCnt,
                                  aEnv );

    sExeSortMergeJoin->mCommonInfo.mFetchRowCnt += *aUsedBlockCnt;
 
    /* Outer Column의 DstDataValue 정보 복구 */
    sOuterColumnBlockItem = sExeSortMergeJoin->mLeftOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );
    sOuterColumnBlockItem = sExeSortMergeJoin->mRightOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );


    return STL_SUCCESS;

    STL_FINISH;

    /* Outer Column의 DstDataValue 정보 복구 */
    sOuterColumnBlockItem = sExeSortMergeJoin->mLeftOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );
    sOuterColumnBlockItem = sExeSortMergeJoin->mRightOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeSortMergeJoin->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeSortMergeJoin->mCommonInfo.mFetchCallCount, 1 );

    return STL_FAILURE;
}


/**
 * @brief Index를 이용한 Inner Join에 대한 SORT MERGE JOIN node를 Fetch한다.
 * @param[in]      aFetchNodeInfo  Fetch Node Info
 * @param[out]     aUsedBlockCnt   Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF            End Of Fetch 여부
 * @param[in]      aEnv            Environment
 *
 * @remark Fetch 수행시 호출된다.
 */
stlStatus qlnxFetchSortMergeJoin_Index_Inner( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                              stlInt64              * aUsedBlockCnt,
                                              stlBool               * aEOF,
                                              qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode               = NULL;
//    qlnoSortMergeJoin     * sOptSortMergeJoin       = NULL;
    qlnxSortMergeJoin     * sExeSortMergeJoin       = NULL;

    stlBool                 sEOF                    = STL_FALSE;

    stlInt64                sUsedBlockCnt           = 0;
    stlInt64                sRemainSkipCnt          = 0;
    stlInt64                sRemainFetchCnt         = 0;

    qlnxCacheBlockInfo    * sInnerCacheBlockInfo    = NULL;
    qlnxCacheBlockInfo    * sOuterCacheBlockInfo    = NULL;

    knlExprEvalInfo       * sWhereExprEvalInfo      = NULL;
    knlExprEvalInfo       * sJoinExprEvalInfo       = NULL;

    qlnxOuterColumnBlockItem  * sOuterColumnBlockItem   = NULL;

    dtlCompareResult            sCompareResult          = DTL_COMPARISON_EQUAL;

    qlnxFetchNodeInfo       sLocalFetchInfo;

    QLL_OPT_DECLARE( stlTime sBeginFetchTime );
    QLL_OPT_DECLARE( stlTime sEndFetchTime );
    QLL_OPT_DECLARE( stlTime sFetchTime );

    QLL_OPT_DECLARE( stlTime sBeginExceptTime );
    QLL_OPT_DECLARE( stlTime sEndExceptTime );
    QLL_OPT_DECLARE( stlTime sExceptTime );

    QLL_OPT_SET_VALUE( sFetchTime, 0 );
    QLL_OPT_SET_VALUE( sExceptTime, 0 );


    /*****************************************
     * Parameter Validation
     ****************************************/

    STL_PARAM_VALIDATE( aFetchNodeInfo->mOptNode->mType == QLL_PLAN_NODE_MERGE_JOIN_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    QLL_OPT_CHECK_TIME( sBeginFetchTime );

    /*****************************************
     * 기본 정보 획득
     ****************************************/

    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aFetchNodeInfo->mDataArea,
                                        QLL_GET_OPT_NODE_IDX( aFetchNodeInfo->mOptNode ) );


    /**
     * SORT MERGE JOIN Execution Node 획득
     */

    sExeSortMergeJoin = (qlnxSortMergeJoin*) sExecNode->mExecNode;


    /**
     * SORT MERGE JOIN Optimization Node 획득
     */

//    sOptSortMergeJoin = (qlnoSortMergeJoin *) aOptNode->mOptNode;


    /**
     * Node 수행 여부 확인 
     */

    STL_DASSERT( *aEOF == STL_FALSE );
    STL_DASSERT( aFetchNodeInfo->mFetchCnt > 0);


    /**
     * Used Block Count 초기화
     */

    sUsedBlockCnt = 0;


    /**
     * Cache Block Info 정보 설정
     */

    sInnerCacheBlockInfo = &(sExeSortMergeJoin->mInnerCacheBlockInfo);
    sOuterCacheBlockInfo = &(sExeSortMergeJoin->mOuterCacheBlockInfo);


    /**
     * Evaluate Info 관련 정보 설정
     */

    sWhereExprEvalInfo  = &sExeSortMergeJoin->mWhereExprEvalInfo;
    sJoinExprEvalInfo   = &sExeSortMergeJoin->mJoinExprEvalInfo;


    /**
     * Local Fetch Info 초기화
     */
    
    QLNX_SET_LOCAL_FETCH_INFO( & sLocalFetchInfo, aFetchNodeInfo );


    /*****************************************
     * Node 수행 여부 확인
     ****************************************/

    STL_DASSERT( sExeSortMergeJoin->mIsEOF == STL_FALSE );
    if( sExeSortMergeJoin->mHasFalseFilter == STL_TRUE )
    {
        QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
        QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
        sExeSortMergeJoin->mIsFirstFetch = STL_FALSE;
        sExeSortMergeJoin->mIsEOF = STL_TRUE;

        STL_THROW( RAMP_NO_DATA );
    }
    else
    {
        /* Do Nothing */
    }


    /**
     * Fetch에 필요한 정보 설정
     */

    sRemainSkipCnt = aFetchNodeInfo->mSkipCnt;
    sRemainFetchCnt = ( aFetchNodeInfo->mFetchCnt < aFetchNodeInfo->mDataArea->mBlockAllocCnt
                        ? aFetchNodeInfo->mFetchCnt
                        : aFetchNodeInfo->mDataArea->mBlockAllocCnt );


    /*****************************************
     * Fetch 수행
     ****************************************/

    /**
     * Outer Table의 Cache Block에 아직 읽을 데이터가 남았는지 체크
     */

    if( QLNX_NEED_NEXT_FETCH( sOuterCacheBlockInfo ) )
    {
        /**
         * EOF 체크
         */

        STL_TRY_THROW( sOuterCacheBlockInfo->mEOF == STL_FALSE,
                       RAMP_NO_DATA );

        /**
         * Outer Table의 Fetch를 수행
         */

        QLL_OPT_CHECK_TIME( sBeginExceptTime );

        sEOF = STL_FALSE;
        QLNX_FETCH_NODE( & sLocalFetchInfo,
                         sExeSortMergeJoin->mOuterCacheBlockInfo.mNodeIdx,
                         0, /* Start Idx */
                         0, /* Skip Count */
                         QLL_FETCH_COUNT_MAX, /* Fetch Count */
                         & sUsedBlockCnt,
                         & sEOF,
                         aEnv );

        QLL_OPT_CHECK_TIME( sEndExceptTime );
        QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

        if( sUsedBlockCnt == 0 )
        {
            QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
            QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
            STL_THROW( RAMP_NO_DATA );            
        }
        else if( sEOF == STL_TRUE )
        {
            sExeSortMergeJoin->mIsEOF = STL_TRUE;
            QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
        }
        else
        {
            /* Do Nothing */
        }
        
        sOuterCacheBlockInfo->mCurrBlockIdx = 0;
        sOuterCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;
    }
    else
    {
        /**
         * Cache에서 읽을 Block Idx를 설정
         */

        sOuterCacheBlockInfo->mCurrBlockIdx++;
    }

    if( sExeSortMergeJoin->mIsFirstFetch == STL_TRUE )
    {
        /**
         * Inner Table의 Fetch를 수행
         */

        STL_DASSERT( QLNX_NEED_NEXT_FETCH( sInnerCacheBlockInfo ) );

        QLL_OPT_CHECK_TIME( sBeginExceptTime );

        sEOF = STL_FALSE;
        QLNX_FETCH_NODE( & sLocalFetchInfo,
                         sExeSortMergeJoin->mInnerCacheBlockInfo.mNodeIdx,
                         0, /* Start Idx */
                         0, /* Skip Count */
                         QLL_FETCH_COUNT_MAX, /* Fetch Count */
                         & sUsedBlockCnt,
                         & sEOF,
                         aEnv );

        QLL_OPT_CHECK_TIME( sEndExceptTime );
        QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

        if( sUsedBlockCnt == 0 )
        {
            QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
            QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
            STL_THROW( RAMP_NO_DATA );
        }
        else if( sEOF == STL_TRUE )
        {
            sExeSortMergeJoin->mIsEOF = STL_TRUE;
            QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
        }
        else
        {
            /* Do Nothing */
        }

        sInnerCacheBlockInfo->mCurrBlockIdx = 0;
        sInnerCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;

        sExeSortMergeJoin->mIsFirstFetch = STL_FALSE;
    }

    while( 1 )
    {
        /**
         * Block Read
         */

        STL_RAMP( RAMP_RETRY_COMPARE );


        /**
         * Sort Key Compare 수행
         */

        QLNX_COMPARE_SORT_KEY( &(sExeSortMergeJoin->mSortKeyCompareList),
                               sOuterCacheBlockInfo->mCurrBlockIdx,
                               sInnerCacheBlockInfo->mCurrBlockIdx,
                               sCompareResult,
                               aEnv );

        if( sCompareResult == DTL_COMPARISON_EQUAL )
        {
            /**
             * Join Condition Logical Filter 평가
             */

            if( sExeSortMergeJoin->mHasJoinCondition == STL_TRUE )
            {
                /* Left Outer Column에 대한 Data Value 재설정 */
                sOuterColumnBlockItem = sExeSortMergeJoin->mLeftOuterColumnBlockList.mHead;
                QLNX_SET_OUTER_COLUMN_BLOCK_LIST( sOuterColumnBlockItem, aEnv );

                /* Right Outer Column에 대한 Data Value 재설정 */
                sOuterColumnBlockItem = sExeSortMergeJoin->mRightOuterColumnBlockList.mHead;
                QLNX_SET_OUTER_COLUMN_BLOCK_LIST( sOuterColumnBlockItem, aEnv );

                /* Join Condition 평가 */
                sJoinExprEvalInfo->mBlockIdx = sExeSortMergeJoin->mCurrBlockIdx;
                STL_TRY( knlEvaluateOneExpression( sJoinExprEvalInfo,
                                                   KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                if( !DTL_BOOLEAN_IS_TRUE( KNL_GET_BLOCK_DATA_VALUE(
                                              sJoinExprEvalInfo->mResultBlock,
                                              sExeSortMergeJoin->mCurrBlockIdx ) ) )
                {
                    STL_THROW( RAMP_READ_NEXT_ONE_ROW );
                }
            }


            /**
             * Outer Table의 데이터와 Inner Table의 데이터에 대하여
             * Sort Merge Join 수행
             */

            if( sExeSortMergeJoin->mJoinedColBlock != NULL )
            {
                /* Result Block에 데이터 복사 */
                QLNX_SET_VALUE_BLOCK( &(sExeSortMergeJoin->mValueBlockList), aEnv );
            }

            /* Where Condition Logical Filter 평가 */
            if( sExeSortMergeJoin->mHasWhereCondition == STL_TRUE )
            {
                sWhereExprEvalInfo->mBlockIdx = sExeSortMergeJoin->mCurrBlockIdx;
                STL_TRY( knlEvaluateOneExpression( sWhereExprEvalInfo,
                                                   KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                if( !DTL_BOOLEAN_IS_TRUE(
                        KNL_GET_BLOCK_DATA_VALUE(
                            sWhereExprEvalInfo->mResultBlock,
                            sExeSortMergeJoin->mCurrBlockIdx ) ) )
                {
                    STL_THROW( RAMP_READ_NEXT_ONE_ROW );
                }
            }

            /* Skip Count가 남았으면 Skip 수행 */
            if( sRemainSkipCnt > 0 )
            {
                sRemainSkipCnt--;

                STL_THROW( RAMP_READ_NEXT_ONE_ROW );
            }

            /* Row Block들을 Result Block에 복사 */
            QLNX_SET_ROW_BLOCK( &(sExeSortMergeJoin->mRowBlockList),
                                sExeSortMergeJoin->mCurrBlockIdx,
                                sOuterCacheBlockInfo->mCurrBlockIdx,
                                sInnerCacheBlockInfo->mCurrBlockIdx );

            /* Fetch Count 체크 */
            sExeSortMergeJoin->mCurrBlockIdx++;
            sRemainFetchCnt --;

            if( sRemainFetchCnt == 0 )
            {
                sEOF = STL_FALSE;
                sExeSortMergeJoin->mIsEOF = STL_FALSE;
                STL_THROW( RAMP_FULL_DATA );
            }
            else
            {
                /**
                 * Right Table의 Cache Block을 다 읽었지만
                 * 아직 Fetch Count가 남은 경우
                 */

                /* Do Nothing */
            }


            STL_RAMP( RAMP_READ_NEXT_ONE_ROW );


            /**
             * Outer Table의 Cache Block에 아직 읽을 데이터가 남았는지 체크
             */

            if( QLNX_NEED_NEXT_FETCH( sOuterCacheBlockInfo ) )
            {
                /**
                 * EOF 체크
                 */

                STL_TRY_THROW( sOuterCacheBlockInfo->mEOF == STL_FALSE,
                               RAMP_NO_DATA );


                /**
                 * Outer Cache Block에 새로운 데이터를 Fetch
                 */

                QLL_OPT_CHECK_TIME( sBeginExceptTime );

                sEOF = STL_FALSE;
                QLNX_FETCH_NODE( & sLocalFetchInfo,
                                 sExeSortMergeJoin->mOuterCacheBlockInfo.mNodeIdx,
                                 0, /* Start Idx */
                                 0, /* Skip Count */
                                 QLL_FETCH_COUNT_MAX, /* Fetch Count */
                                 & sUsedBlockCnt,
                                 & sEOF,
                                 aEnv );

                QLL_OPT_CHECK_TIME( sEndExceptTime );
                QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                if( sUsedBlockCnt == 0 )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                    QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                    STL_THROW( RAMP_NO_DATA );
                }
                else if( sEOF == STL_TRUE )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                }
                else
                {
                    /* Do Nothing */
                }

                sOuterCacheBlockInfo->mCurrBlockIdx = 0;
                sOuterCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;
            }
            else
            {
                /**
                 * Cache에서 읽을 Block Idx를 설정
                 */

                sOuterCacheBlockInfo->mCurrBlockIdx++;
            }

            STL_THROW( RAMP_RETRY_COMPARE );
        }
        else if( sCompareResult == DTL_COMPARISON_LESS )
        {
            /**
             * Outer Table의 Cache Block에 아직 읽을 데이터가 남았는지 체크
             */

            if( QLNX_NEED_NEXT_FETCH( sOuterCacheBlockInfo ) )
            {
                /**
                 * EOF 체크
                 */

                STL_TRY_THROW( sOuterCacheBlockInfo->mEOF == STL_FALSE,
                               RAMP_NO_DATA );


                /**
                 * Outer Cache Block에 새로운 데이터를 Fetch
                 */

                QLL_OPT_CHECK_TIME( sBeginExceptTime );

                sEOF = STL_FALSE;
                QLNX_FETCH_NODE( & sLocalFetchInfo,
                                 sExeSortMergeJoin->mOuterCacheBlockInfo.mNodeIdx,
                                 0, /* Start Idx */
                                 0, /* Skip Count */
                                 QLL_FETCH_COUNT_MAX, /* Fetch Count */
                                 & sUsedBlockCnt,
                                 & sEOF,
                                 aEnv );

                QLL_OPT_CHECK_TIME( sEndExceptTime );
                QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                if( sUsedBlockCnt == 0 )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                    QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                    STL_THROW( RAMP_NO_DATA );
                }
                else if( sEOF == STL_TRUE )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                }
                else
                {
                    /* Do Nothing */
                }

                sOuterCacheBlockInfo->mCurrBlockIdx = 0;
                sOuterCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;
            }
            else
            {
                /**
                 * Cache에서 읽을 Block Idx를 설정
                 */

                sOuterCacheBlockInfo->mCurrBlockIdx++;
            }

            STL_THROW( RAMP_RETRY_COMPARE );
        }
        else
        {
            /**
             * Inner Table의 Cache Block에 아직 읽을 데이터가 남았는지 체크
             */

            if( QLNX_NEED_NEXT_FETCH( sInnerCacheBlockInfo ) )
            {
                /**
                 * EOF 체크
                 */

                STL_TRY_THROW( sInnerCacheBlockInfo->mEOF == STL_FALSE,
                               RAMP_NO_DATA );

                /**
                 * Cache에 새로운 데이터를 Fetch
                 */

                QLL_OPT_CHECK_TIME( sBeginExceptTime );

                sEOF = STL_FALSE;
                QLNX_FETCH_NODE( & sLocalFetchInfo,
                                 sExeSortMergeJoin->mInnerCacheBlockInfo.mNodeIdx,
                                 0, /* Start Idx */
                                 0, /* Skip Count */
                                 QLL_FETCH_COUNT_MAX, /* Fetch Count */
                                 & sUsedBlockCnt,
                                 & sEOF,
                                 aEnv );

                QLL_OPT_CHECK_TIME( sEndExceptTime );
                QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                if( sUsedBlockCnt == 0 )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                    STL_THROW( RAMP_NO_DATA );
                }
                else if( sEOF == STL_TRUE )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                }
                else
                {
                    /* Do Nothing */
                }
        
                sInnerCacheBlockInfo->mCurrBlockIdx = 0;
                sInnerCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;
            }
            else
            {
                /**
                 * Cache에서 읽을 Block Idx를 설정
                 */

                sInnerCacheBlockInfo->mCurrBlockIdx++;
            }

            STL_THROW( RAMP_RETRY_COMPARE );
        }
    }

    STL_RAMP( RAMP_NO_DATA );

    STL_DASSERT( QLNX_IS_END_OF_TABLE_FETCH( sInnerCacheBlockInfo ) ||
                 QLNX_IS_END_OF_TABLE_FETCH( sOuterCacheBlockInfo ) );

    sEOF = STL_TRUE;
    sExeSortMergeJoin->mIsEOF = STL_TRUE;

    STL_RAMP( RAMP_FULL_DATA );


    /**
     * OUTPUT 설정
     */

    *aEOF = sEOF;
    *aUsedBlockCnt = sExeSortMergeJoin->mCurrBlockIdx;

    /* Parent의 Fetch Node Info 설정 */
    aFetchNodeInfo->mFetchCnt = sRemainFetchCnt;
    aFetchNodeInfo->mSkipCnt  = sRemainSkipCnt;

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT(
        sExeSortMergeJoin->mJoinedColBlock,
        0,
        sExeSortMergeJoin->mCurrBlockIdx );

    QLNX_SET_USED_BLOCK_SIZE_ON_ROW_BLOCK( &(sExeSortMergeJoin->mRowBlockList),
                                           sExeSortMergeJoin->mCurrBlockIdx );

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeSortMergeJoin->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeSortMergeJoin->mCommonInfo.mFetchCallCount, 1 );
    QLL_OPT_ADD_RECORD_STAT_INFO( sExeSortMergeJoin->mCommonInfo.mFetchRecordStat,
                                  sExeSortMergeJoin->mCommonInfo.mResultColBlock,
                                  *aUsedBlockCnt,
                                  aEnv );

    sExeSortMergeJoin->mCommonInfo.mFetchRowCnt += *aUsedBlockCnt;
 
    /* Outer Column의 DstDataValue 정보 복구 */
    sOuterColumnBlockItem = sExeSortMergeJoin->mLeftOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );
    sOuterColumnBlockItem = sExeSortMergeJoin->mRightOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );


    return STL_SUCCESS;

    STL_FINISH;

    /* Outer Column의 DstDataValue 정보 복구 */
    sOuterColumnBlockItem = sExeSortMergeJoin->mLeftOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );
    sOuterColumnBlockItem = sExeSortMergeJoin->mRightOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeSortMergeJoin->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeSortMergeJoin->mCommonInfo.mFetchCallCount, 1 );

    return STL_FAILURE;
}


/**
 * @brief Index를 이용한 Outer Join에 대한 SORT MERGE JOIN node를 Fetch한다.
 * @param[in]      aFetchNodeInfo  Fetch Node Info
 * @param[out]     aUsedBlockCnt   Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF            End Of Fetch 여부
 * @param[in]      aEnv            Environment
 *
 * @remark Fetch 수행시 호출된다.
 */
stlStatus qlnxFetchSortMergeJoin_Index_Outer( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                              stlInt64              * aUsedBlockCnt,
                                              stlBool               * aEOF,
                                              qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode               = NULL;
//    qlnoSortMergeJoin     * sOptSortMergeJoin       = NULL;
    qlnxSortMergeJoin     * sExeSortMergeJoin       = NULL;

    stlBool                 sEOF                    = STL_FALSE;

    stlInt64                sUsedBlockCnt           = 0;
    stlInt64                sRemainSkipCnt          = 0;
    stlInt64                sRemainFetchCnt         = 0;

    qlnxCacheBlockInfo    * sInnerCacheBlockInfo    = NULL;
    qlnxCacheBlockInfo    * sOuterCacheBlockInfo    = NULL;

    knlExprEvalInfo       * sWhereExprEvalInfo      = NULL;
    knlExprEvalInfo       * sJoinExprEvalInfo       = NULL;

    qlnxOuterColumnBlockItem  * sOuterColumnBlockItem   = NULL;

    dtlCompareResult        sCompareResult          = DTL_COMPARISON_EQUAL;

    stlBool                 sIsResultTrue;

    qlnxFetchNodeInfo       sLocalFetchInfo;

    QLL_OPT_DECLARE( stlTime sBeginFetchTime );
    QLL_OPT_DECLARE( stlTime sEndFetchTime );
    QLL_OPT_DECLARE( stlTime sFetchTime );

    QLL_OPT_DECLARE( stlTime sBeginExceptTime );
    QLL_OPT_DECLARE( stlTime sEndExceptTime );
    QLL_OPT_DECLARE( stlTime sExceptTime );

    QLL_OPT_SET_VALUE( sFetchTime, 0 );
    QLL_OPT_SET_VALUE( sExceptTime, 0 );


    /*****************************************
     * Parameter Validation
     ****************************************/

    STL_PARAM_VALIDATE( aFetchNodeInfo->mOptNode->mType == QLL_PLAN_NODE_MERGE_JOIN_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    QLL_OPT_CHECK_TIME( sBeginFetchTime );

    /*****************************************
     * 기본 정보 획득
     ****************************************/

    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aFetchNodeInfo->mDataArea,
                                        QLL_GET_OPT_NODE_IDX( aFetchNodeInfo->mOptNode ) );


    /**
     * SORT MERGE JOIN Execution Node 획득
     */

    sExeSortMergeJoin = (qlnxSortMergeJoin*) sExecNode->mExecNode;


    /**
     * SORT MERGE JOIN Optimization Node 획득
     */

//    sOptSortMergeJoin = (qlnoSortMergeJoin *) aOptNode->mOptNode;


    /**
     * Node 수행 여부 확인 
     */

    STL_DASSERT( *aEOF == STL_FALSE );
    STL_DASSERT( aFetchNodeInfo->mFetchCnt > 0);


    /**
     * Used Block Count 초기화
     */

    sUsedBlockCnt = 0;


    /**
     * Cache Block Info 정보 설정
     */

    sInnerCacheBlockInfo = &(sExeSortMergeJoin->mInnerCacheBlockInfo);
    sOuterCacheBlockInfo = &(sExeSortMergeJoin->mOuterCacheBlockInfo);


    /**
     * Evaluate Info 관련 정보 설정
     */

    sWhereExprEvalInfo  = &sExeSortMergeJoin->mWhereExprEvalInfo;
    sJoinExprEvalInfo   = &sExeSortMergeJoin->mJoinExprEvalInfo;


    /**
     * Local Fetch Info 초기화
     */
    
    QLNX_SET_LOCAL_FETCH_INFO( & sLocalFetchInfo, aFetchNodeInfo );


    /*****************************************
     * Node 수행 여부 확인
     ****************************************/

    STL_DASSERT( sExeSortMergeJoin->mIsEOF == STL_FALSE );
    if( sExeSortMergeJoin->mHasFalseFilter == STL_TRUE )
    {
        QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
        QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
        sExeSortMergeJoin->mIsFirstFetch = STL_FALSE;
        sExeSortMergeJoin->mIsEOF = STL_TRUE;

        STL_THROW( RAMP_NO_DATA );
    }
    else
    {
        /* Do Nothing */
    }


    /**
     * Fetch에 필요한 정보 설정
     */

    sRemainSkipCnt = aFetchNodeInfo->mSkipCnt;
    sRemainFetchCnt = ( aFetchNodeInfo->mFetchCnt < aFetchNodeInfo->mDataArea->mBlockAllocCnt
                        ? aFetchNodeInfo->mFetchCnt
                        : aFetchNodeInfo->mDataArea->mBlockAllocCnt );


    /*****************************************
     * Fetch 수행
     ****************************************/

    /* Right Node가 EOF인지 판별 */
    if( QLNX_NEED_NEXT_FETCH( sInnerCacheBlockInfo ) &&
        (sInnerCacheBlockInfo->mCacheBlockCnt == 0) &&
        (sInnerCacheBlockInfo->mEOF == STL_TRUE) )
    {
        STL_THROW( RAMP_RIGHT_NODE_EOF );
    }

    /**
     * Outer Table의 Cache Block에 아직 읽을 데이터가 남았는지 체크
     */

    if( (sExeSortMergeJoin->mIsFirstFetch == STL_TRUE) ||
        (sExeSortMergeJoin->mIsLeftMatched == STL_TRUE) )
    {
        if( QLNX_NEED_NEXT_FETCH( sOuterCacheBlockInfo ) )
        {
            /**
             * EOF 체크
             */

            STL_TRY_THROW( sOuterCacheBlockInfo->mEOF == STL_FALSE,
                           RAMP_LEFT_NODE_EOF );


            /**
             * Outer Table의 Fetch를 수행
             */

            QLL_OPT_CHECK_TIME( sBeginExceptTime );

            sEOF = STL_FALSE;
            QLNX_FETCH_NODE( & sLocalFetchInfo,
                             sExeSortMergeJoin->mOuterCacheBlockInfo.mNodeIdx,
                             0, /* Start Idx */
                             0, /* Skip Count */
                             QLL_FETCH_COUNT_MAX, /* Fetch Count */
                             & sUsedBlockCnt,
                             & sEOF,
                             aEnv );

            QLL_OPT_CHECK_TIME( sEndExceptTime );
            QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

            if( sUsedBlockCnt == 0 )
            {
                QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                STL_THROW( RAMP_LEFT_NODE_EOF );            
            }
            else if( sEOF == STL_TRUE )
            {
                sExeSortMergeJoin->mIsEOF = STL_TRUE;
                QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
            }
            else
            {
                /* Do Nothing */
            }

            sOuterCacheBlockInfo->mCurrBlockIdx = 0;
            sOuterCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;
            sExeSortMergeJoin->mIsLeftMatched = STL_FALSE;
        }
        else
        {
            /**
             * Cache에서 읽을 Block Idx를 설정
             */

            sOuterCacheBlockInfo->mCurrBlockIdx++;
            sExeSortMergeJoin->mIsLeftMatched = STL_FALSE;
        }
    }

    if( sExeSortMergeJoin->mIsFirstFetch == STL_TRUE )
    {
        /**
         * Inner Table의 Fetch를 수행
         */

        STL_DASSERT( QLNX_NEED_NEXT_FETCH( sInnerCacheBlockInfo ) );

        QLL_OPT_CHECK_TIME( sBeginExceptTime );

        sEOF = STL_FALSE;
        QLNX_FETCH_NODE( & sLocalFetchInfo,
                         sExeSortMergeJoin->mInnerCacheBlockInfo.mNodeIdx,
                         0, /* Start Idx */
                         0, /* Skip Count */
                         QLL_FETCH_COUNT_MAX, /* Fetch Count */
                         & sUsedBlockCnt,
                         & sEOF,
                         aEnv );

        QLL_OPT_CHECK_TIME( sEndExceptTime );
        QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

        if( sUsedBlockCnt == 0 )
        {
            QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
            STL_THROW( RAMP_RIGHT_NODE_EOF );
        }
        else if( sEOF == STL_TRUE )
        {
            sExeSortMergeJoin->mIsEOF = STL_TRUE;
            QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
        }
        else
        {
            /* Do Nothing */
        }

        sInnerCacheBlockInfo->mCurrBlockIdx = 0;
        sInnerCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;
        sExeSortMergeJoin->mIsRightMatched = STL_FALSE;

        sExeSortMergeJoin->mIsFirstFetch = STL_FALSE;
    }

    while( 1 )
    {
        /**
         * Block Read
         */

        STL_RAMP( RAMP_RETRY_COMPARE );


        /**
         * Sort Key Compare 수행
         */

        QLNX_COMPARE_SORT_KEY( &(sExeSortMergeJoin->mSortKeyCompareList),
                               sOuterCacheBlockInfo->mCurrBlockIdx,
                               sInnerCacheBlockInfo->mCurrBlockIdx,
                               sCompareResult,
                               aEnv );

        if( sCompareResult == DTL_COMPARISON_EQUAL )
        {
            /**
             * Outer Table의 데이터와 Inner Table의 데이터에 대하여
             * Sort Merge Join 수행
             */


            /* Join Condition Logical Filter 평가 */
            if( sExeSortMergeJoin->mHasJoinCondition == STL_TRUE )
            {
                /* Left Outer Column에 대한 Data Value 재설정 */
                sOuterColumnBlockItem = sExeSortMergeJoin->mLeftOuterColumnBlockList.mHead;
                QLNX_SET_OUTER_COLUMN_BLOCK_LIST( sOuterColumnBlockItem, aEnv );

                /* Right Outer Column에 대한 Data Value 재설정 */
                sOuterColumnBlockItem = sExeSortMergeJoin->mRightOuterColumnBlockList.mHead;
                QLNX_SET_OUTER_COLUMN_BLOCK_LIST( sOuterColumnBlockItem, aEnv );

                /* Join Condition 평가 */
                sJoinExprEvalInfo->mBlockIdx = sExeSortMergeJoin->mCurrBlockIdx;
                STL_TRY( knlEvaluateOneExpression( sJoinExprEvalInfo,
                                                   KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                if( !DTL_BOOLEAN_IS_TRUE(
                        KNL_GET_BLOCK_DATA_VALUE(
                            sJoinExprEvalInfo->mResultBlock,
                            sExeSortMergeJoin->mCurrBlockIdx ) ) )
                {
                    if( sExeSortMergeJoin->mJoinType == QLV_JOIN_TYPE_FULL_OUTER )
                    {
                        /* Result Block에 데이터 복사 */
                        QLNX_SET_OUTER_VALUE_BLOCK(
                            &(sExeSortMergeJoin->mValueBlockList),
                            &(sExeSortMergeJoin->mInnerCacheBlockInfo.mCurrBlockIdx),
                            aEnv );

                        sExeSortMergeJoin->mIsRightMatched = STL_TRUE;

                        /* Where Condition Logical Filter 평가 */
                        sIsResultTrue = STL_TRUE;
                        if( sExeSortMergeJoin->mHasWhereCondition == STL_TRUE )
                        {
                            sWhereExprEvalInfo->mBlockIdx = sExeSortMergeJoin->mCurrBlockIdx;
                            STL_TRY( knlEvaluateOneExpression( sWhereExprEvalInfo,
                                                               KNL_ENV(aEnv) )
                                     == STL_SUCCESS );

                            if( !DTL_BOOLEAN_IS_TRUE(
                                    KNL_GET_BLOCK_DATA_VALUE(
                                        sWhereExprEvalInfo->mResultBlock,
                                        sExeSortMergeJoin->mCurrBlockIdx ) ) )
                            {
                                sIsResultTrue = STL_FALSE;
                            }
                        }

                        if( sIsResultTrue == STL_TRUE )
                        {
                            /* Skip Count가 남았으면 Skip 수행 */
                            if( sRemainSkipCnt > 0 )
                            {
                                sRemainSkipCnt--;
                            }
                            else
                            {
                                /* Fetch Count 체크 */
                                sExeSortMergeJoin->mCurrBlockIdx++;
                                sRemainFetchCnt --;

                                if( sRemainFetchCnt == 0 )
                                {
                                    STL_THROW( RAMP_FULL_DATA );
                                }
                                else
                                {
                                    /**
                                     * Right Table의 Cache Block을 다 읽었지만
                                     * 아직 Fetch Count가 남은 경우
                                     */

                                    /* Do Nothing */
                                }
                            }
                        }
                    }

                    sExeSortMergeJoin->mIsRightMatched = STL_TRUE;

                    STL_THROW( RAMP_READ_NEXT_ONE_ROW );
                }
            }

            if( sExeSortMergeJoin->mJoinedColBlock != NULL )
            {
                /* Result Block에 데이터 복사 */

                QLNX_SET_VALUE_BLOCK( &(sExeSortMergeJoin->mValueBlockList), aEnv );
            }

            sExeSortMergeJoin->mIsLeftMatched = STL_TRUE;
            sExeSortMergeJoin->mIsRightMatched = STL_TRUE;


            /**
             * Where Condition Logical Filter 평가
             */

            if( sExeSortMergeJoin->mHasWhereCondition == STL_TRUE )
            {
                sWhereExprEvalInfo->mBlockIdx = sExeSortMergeJoin->mCurrBlockIdx;
                STL_TRY( knlEvaluateOneExpression( sWhereExprEvalInfo,
                                                   KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                if( !DTL_BOOLEAN_IS_TRUE(
                        KNL_GET_BLOCK_DATA_VALUE(
                            sWhereExprEvalInfo->mResultBlock,
                            sExeSortMergeJoin->mCurrBlockIdx ) ) )
                {
                    STL_THROW( RAMP_READ_NEXT_ONE_ROW );
                }
            }


            /**
             * Skip Count가 남았으면 Skip 수행
             */

            if( sRemainSkipCnt > 0 )
            {
                sRemainSkipCnt--;

                STL_THROW( RAMP_READ_NEXT_ONE_ROW );
            }


            /**
             * Fetch Count 체크
             */

            sExeSortMergeJoin->mCurrBlockIdx++;
            sRemainFetchCnt --;

            if( sRemainFetchCnt == 0 )
            {
                STL_THROW( RAMP_FULL_DATA );
            }
            else
            {
                /**
                 * Right Table의 Cache Block을 다 읽었지만
                 * 아직 Fetch Count가 남은 경우
                 */

                /* Do Nothing */
            }


            STL_RAMP( RAMP_READ_NEXT_ONE_ROW );


            if( sExeSortMergeJoin->mIsLeftMatched == STL_FALSE )
            {
                /* Result Block에 데이터 복사 */
                QLNX_SET_OUTER_VALUE_BLOCK( &(sExeSortMergeJoin->mValueBlockList),
                                            &(sExeSortMergeJoin->mOuterCacheBlockInfo.mCurrBlockIdx),
                                            aEnv );

                sExeSortMergeJoin->mIsLeftMatched = STL_TRUE;

                /* Where Condition Logical Filter 평가 */
                sIsResultTrue = STL_TRUE;
                if( sExeSortMergeJoin->mHasWhereCondition == STL_TRUE )
                {
                    sWhereExprEvalInfo->mBlockIdx = sExeSortMergeJoin->mCurrBlockIdx;
                    STL_TRY( knlEvaluateOneExpression( sWhereExprEvalInfo,
                                                       KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    if( !DTL_BOOLEAN_IS_TRUE( KNL_GET_BLOCK_DATA_VALUE(
                             sWhereExprEvalInfo->mResultBlock,
                             sExeSortMergeJoin->mCurrBlockIdx ) ) )
                    {
                        sIsResultTrue = STL_FALSE;
                    }
                }
                if( sIsResultTrue == STL_TRUE )
                {
                    /* Skip Count가 남았으면 Skip 수행 */
                    if( sRemainSkipCnt > 0 )
                    {
                        sRemainSkipCnt--;
                    }
                    else
                    {
                        /* Fetch Count 체크 */
                        sExeSortMergeJoin->mCurrBlockIdx++;
                        sRemainFetchCnt --;

                        if( sRemainFetchCnt == 0 )
                        {
                            STL_THROW( RAMP_FULL_DATA );
                        }
                        else
                        {
                            /**
                             * Right Table의 Cache Block을 다 읽었지만
                             * 아직 Fetch Count가 남은 경우
                             */

                            /* Do Nothing */
                        }
                    }
                }
            }


            /**
             * Outer Table의 Cache Block에 아직 읽을 데이터가 남았는지 체크
             */

            if( QLNX_NEED_NEXT_FETCH( sOuterCacheBlockInfo ) )
            {
                /**
                 * EOF 체크
                 */

                STL_TRY_THROW( sOuterCacheBlockInfo->mEOF == STL_FALSE,
                               RAMP_LEFT_NODE_EOF );


                /**
                 * Outer Cache Block에 새로운 데이터를 Fetch
                 */

                QLL_OPT_CHECK_TIME( sBeginExceptTime );

                sEOF = STL_FALSE;
                QLNX_FETCH_NODE( & sLocalFetchInfo,
                                 sExeSortMergeJoin->mOuterCacheBlockInfo.mNodeIdx,
                                 0, /* Start Idx */
                                 0, /* Skip Count */
                                 QLL_FETCH_COUNT_MAX, /* Fetch Count */
                                 & sUsedBlockCnt,
                                 & sEOF,
                                 aEnv );

                QLL_OPT_CHECK_TIME( sEndExceptTime );
                QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                if( sUsedBlockCnt == 0 )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                    STL_THROW( RAMP_LEFT_NODE_EOF );
                }
                else if( sEOF == STL_TRUE )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                }
                else
                {
                    /* Do Nothing */
                }

                sOuterCacheBlockInfo->mCurrBlockIdx = 0;
                sOuterCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;
                sExeSortMergeJoin->mIsLeftMatched = STL_FALSE;
            }
            else
            {
                /**
                 * Cache에서 읽을 Block Idx를 설정
                 */

                sOuterCacheBlockInfo->mCurrBlockIdx++;
                sExeSortMergeJoin->mIsLeftMatched = STL_FALSE;
            }

            STL_THROW( RAMP_RETRY_COMPARE );
        }
        else if( sCompareResult == DTL_COMPARISON_LESS )
        {
            if( sExeSortMergeJoin->mIsLeftMatched == STL_FALSE )
            {
                /* Result Block에 데이터 복사 */
                QLNX_SET_OUTER_VALUE_BLOCK( &(sExeSortMergeJoin->mValueBlockList),
                                            &(sExeSortMergeJoin->mOuterCacheBlockInfo.mCurrBlockIdx),
                                            aEnv );

                sExeSortMergeJoin->mIsLeftMatched = STL_TRUE;

                /* Where Condition Logical Filter 평가 */
                sIsResultTrue = STL_TRUE;
                if( sExeSortMergeJoin->mHasWhereCondition == STL_TRUE )
                {
                    sWhereExprEvalInfo->mBlockIdx = sExeSortMergeJoin->mCurrBlockIdx;
                    STL_TRY( knlEvaluateOneExpression( sWhereExprEvalInfo,
                                                       KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    if( !DTL_BOOLEAN_IS_TRUE( KNL_GET_BLOCK_DATA_VALUE(
                             sWhereExprEvalInfo->mResultBlock,
                             sExeSortMergeJoin->mCurrBlockIdx ) ) )
                    {
                        sIsResultTrue = STL_FALSE;
                    }
                }

                if( sIsResultTrue == STL_TRUE )
                {
                    /* Skip Count가 남았으면 Skip 수행 */
                    if( sRemainSkipCnt > 0 )
                    {
                        sRemainSkipCnt--;
                    }
                    else
                    {
                        /* Fetch Count 체크 */
                        sExeSortMergeJoin->mCurrBlockIdx++;
                        sRemainFetchCnt --;

                        if( sRemainFetchCnt == 0 )
                        {
                            STL_THROW( RAMP_FULL_DATA );
                        }
                        else
                        {
                            /**
                             * Right Table의 Cache Block을 다 읽었지만
                             * 아직 Fetch Count가 남은 경우
                             */

                            /* Do Nothing */
                        }
                    }
                }
            }

            /**
             * Outer Table의 Cache Block에 아직 읽을 데이터가 남았는지 체크
             */

            if( QLNX_NEED_NEXT_FETCH( sOuterCacheBlockInfo ) )
            {
                /**
                 * EOF 체크
                 */

                STL_TRY_THROW( sOuterCacheBlockInfo->mEOF == STL_FALSE,
                               RAMP_LEFT_NODE_EOF );


                /**
                 * Outer Cache Block에 새로운 데이터를 Fetch
                 */

                QLL_OPT_CHECK_TIME( sBeginExceptTime );

                sEOF = STL_FALSE;
                QLNX_FETCH_NODE( & sLocalFetchInfo,
                                 sExeSortMergeJoin->mOuterCacheBlockInfo.mNodeIdx,
                                 0, /* Start Idx */
                                 0, /* Skip Count */
                                 QLL_FETCH_COUNT_MAX, /* Fetch Count */
                                 & sUsedBlockCnt,
                                 & sEOF,
                                 aEnv );

                QLL_OPT_CHECK_TIME( sEndExceptTime );
                QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                if( sUsedBlockCnt == 0 )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                    STL_THROW( RAMP_LEFT_NODE_EOF );
                }
                else if( sEOF == STL_TRUE )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                }
                else
                {
                    /* Do Nothing */
                }

                sOuterCacheBlockInfo->mCurrBlockIdx = 0;
                sOuterCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;
                sExeSortMergeJoin->mIsLeftMatched = STL_FALSE;
            }
            else
            {
                /**
                 * Cache에서 읽을 Block Idx를 설정
                 */

                sOuterCacheBlockInfo->mCurrBlockIdx++;
                sExeSortMergeJoin->mIsLeftMatched = STL_FALSE;
            }

            STL_THROW( RAMP_RETRY_COMPARE );
        }
        else
        {
            if( (sExeSortMergeJoin->mIsRightMatched == STL_FALSE) &&
                (sExeSortMergeJoin->mJoinType == QLV_JOIN_TYPE_FULL_OUTER) )
            {
                /* Result Block에 데이터 복사 */
                QLNX_SET_OUTER_VALUE_BLOCK( &(sExeSortMergeJoin->mValueBlockList),
                                            &(sExeSortMergeJoin->mInnerCacheBlockInfo.mCurrBlockIdx),
                                            aEnv );

                sExeSortMergeJoin->mIsRightMatched = STL_TRUE;

                /* Where Condition Logical Filter 평가 */
                sIsResultTrue = STL_TRUE;
                if( sExeSortMergeJoin->mHasWhereCondition == STL_TRUE )
                {
                    sWhereExprEvalInfo->mBlockIdx = sExeSortMergeJoin->mCurrBlockIdx;
                    STL_TRY( knlEvaluateOneExpression( sWhereExprEvalInfo,
                                                       KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    if( !DTL_BOOLEAN_IS_TRUE( KNL_GET_BLOCK_DATA_VALUE(
                             sWhereExprEvalInfo->mResultBlock,
                             sExeSortMergeJoin->mCurrBlockIdx ) ) )
                    {
                        sIsResultTrue = STL_FALSE;
                    }
                }

                if( sIsResultTrue == STL_TRUE )
                {
                    /* Skip Count가 남았으면 Skip 수행 */
                    if( sRemainSkipCnt > 0 )
                    {
                        sRemainSkipCnt--;
                    }
                    else
                    {
                        /* Fetch Count 체크 */
                        sExeSortMergeJoin->mCurrBlockIdx++;
                        sRemainFetchCnt --;

                        if( sRemainFetchCnt == 0 )
                        {
                            STL_THROW( RAMP_FULL_DATA );
                        }
                        else
                        {
                            /**
                             * Right Table의 Cache Block을 다 읽었지만
                             * 아직 Fetch Count가 남은 경우
                             */

                            /* Do Nothing */
                        }
                    }
                }
            }

            /**
             * Inner Table의 Cache Block에 아직 읽을 데이터가 남았는지 체크
             */

            if( QLNX_NEED_NEXT_FETCH( sInnerCacheBlockInfo ) )
            {
                /**
                 * EOF 체크
                 */

                STL_TRY_THROW( sInnerCacheBlockInfo->mEOF == STL_FALSE,
                               RAMP_RIGHT_NODE_EOF );

                /**
                 * Cache에 새로운 데이터를 Fetch
                 */

                QLL_OPT_CHECK_TIME( sBeginExceptTime );

                sEOF = STL_FALSE;
                QLNX_FETCH_NODE( & sLocalFetchInfo,
                                 sExeSortMergeJoin->mInnerCacheBlockInfo.mNodeIdx,
                                 0, /* Start Idx */
                                 0, /* Skip Count */
                                 QLL_FETCH_COUNT_MAX, /* Fetch Count */
                                 & sUsedBlockCnt,
                                 & sEOF,
                                 aEnv );

                QLL_OPT_CHECK_TIME( sEndExceptTime );
                QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                if( sUsedBlockCnt == 0 )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                    STL_THROW( RAMP_RIGHT_NODE_EOF );
                }
                else if( sEOF == STL_TRUE )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                }
                else
                {
                    /* Do Nothing */
                }
        
                sInnerCacheBlockInfo->mCurrBlockIdx = 0;
                sInnerCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;
                sExeSortMergeJoin->mIsRightMatched = STL_FALSE;
            }
            else
            {
                /**
                 * Cache에서 읽을 Block Idx를 설정
                 */

                sInnerCacheBlockInfo->mCurrBlockIdx++;
                sExeSortMergeJoin->mIsRightMatched = STL_FALSE;
            }

            STL_THROW( RAMP_RETRY_COMPARE );
        }
    }


    STL_THROW( RAMP_FULL_DATA );


    /**
     * Left Node가 EOF인 경우
     */

    STL_RAMP( RAMP_LEFT_NODE_EOF );

    if( sExeSortMergeJoin->mJoinType == QLV_JOIN_TYPE_FULL_OUTER )
    {
        while( STL_TRUE )
        {
            if( sExeSortMergeJoin->mIsRightMatched == STL_TRUE )
            {
                /* Inner Table을 다음 레코드로 이동 */
                if( QLNX_NEED_NEXT_FETCH( sInnerCacheBlockInfo ) )
                {
                    /* EOF 체크 */
                    if( sInnerCacheBlockInfo->mEOF == STL_TRUE )
                    {
                        /* Outer Table의 Scan 종료 */
                        QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                        STL_THROW( RAMP_NO_DATA );
                    }

                    /* Inner Cache Block에 새로운 데이터를 Fetch */
                    QLL_OPT_CHECK_TIME( sBeginExceptTime );

                    sEOF = STL_FALSE;
                    QLNX_FETCH_NODE( & sLocalFetchInfo,
                                     sExeSortMergeJoin->mInnerCacheBlockInfo.mNodeIdx,
                                     0, /* Start Idx */
                                     0, /* Skip Count */
                                     QLL_FETCH_COUNT_MAX, /* Fetch Count */
                                     & sUsedBlockCnt,
                                     & sEOF,
                                     aEnv );

                    QLL_OPT_CHECK_TIME( sEndExceptTime );
                    QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                    if( sUsedBlockCnt == 0 )
                    {
                        QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                        STL_THROW( RAMP_NO_DATA );
                    }
                    else if( sEOF == STL_TRUE )
                    {
                        QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                    }
                    else
                    {
                        /* Do Nothing */
                    }

                    sInnerCacheBlockInfo->mCurrBlockIdx = 0;
                    sInnerCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;
                    sExeSortMergeJoin->mIsRightMatched = STL_FALSE;
                }
                else
                {
                    /* Cache에서 읽을 Block Idx를 설정 */
                    sInnerCacheBlockInfo->mCurrBlockIdx++;
                    sExeSortMergeJoin->mIsRightMatched = STL_FALSE;
                }
            }

            /* Result Block에 데이터 복사 */
            QLNX_SET_OUTER_VALUE_BLOCK( &(sExeSortMergeJoin->mValueBlockList),
                                        &(sExeSortMergeJoin->mInnerCacheBlockInfo.mCurrBlockIdx),
                                        aEnv );

            sExeSortMergeJoin->mIsRightMatched = STL_TRUE;

            /* Where Condition Logical Filter 평가 */
            if( sExeSortMergeJoin->mHasWhereCondition == STL_TRUE )
            {
                sWhereExprEvalInfo->mBlockIdx = sExeSortMergeJoin->mCurrBlockIdx;
                STL_TRY( knlEvaluateOneExpression( sWhereExprEvalInfo,
                                                   KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                if( !DTL_BOOLEAN_IS_TRUE( KNL_GET_BLOCK_DATA_VALUE(
                         sWhereExprEvalInfo->mResultBlock,
                         sExeSortMergeJoin->mCurrBlockIdx ) ) )
                {
                    continue;
                }
            }

            sExeSortMergeJoin->mIsLeftMatched = STL_TRUE;
            sExeSortMergeJoin->mIsRightMatched = STL_TRUE;

            /* Skip Count가 남았으면 Skip 수행 */
            if( sRemainSkipCnt > 0 )
            {
                sRemainSkipCnt--;
                continue;
            }

            /* Fetch Count 체크 */
            sExeSortMergeJoin->mCurrBlockIdx++;
            sRemainFetchCnt --;

            if( sRemainFetchCnt == 0 )
            {
                STL_THROW( RAMP_FULL_DATA );
            }
            else
            {
                /**
                 * Right Table의 Cache Block을 다 읽었지만
                 * 아직 Fetch Count가 남은 경우
                 */

                /* Do Nothing */
            }
        }
    }
    else
    {
        /* Left Outer인 경우 Inner쪽도 종료로 셋팅한다. */
        QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
    }

    STL_THROW( RAMP_FULL_DATA );


    /**
     * Right Node가 EOF인 경우
     */

    STL_RAMP( RAMP_RIGHT_NODE_EOF );

    /* Left Node에 남은 데이터를 모두 내보낸다. */
    while( STL_TRUE )
    {
        if( sExeSortMergeJoin->mIsLeftMatched == STL_TRUE )
        {
            /* Outer Table을 다음 레코드로 이동 */
            if( QLNX_NEED_NEXT_FETCH( sOuterCacheBlockInfo ) )
            {
                /* EOF 체크 */
                if( sOuterCacheBlockInfo->mEOF == STL_TRUE )
                {
                    /* Outer Table의 Scan 종료 */
                    QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                    STL_THROW( RAMP_NO_DATA );
                }

                /* Outer Cache Block에 새로운 데이터를 Fetch */
                QLL_OPT_CHECK_TIME( sBeginExceptTime );

                sEOF = STL_FALSE;
                QLNX_FETCH_NODE( & sLocalFetchInfo,
                                 sExeSortMergeJoin->mOuterCacheBlockInfo.mNodeIdx,
                                 0, /* Start Idx */
                                 0, /* Skip Count */
                                 QLL_FETCH_COUNT_MAX, /* Fetch Count */
                                 & sUsedBlockCnt,
                                 & sEOF,
                                 aEnv );

                QLL_OPT_CHECK_TIME( sEndExceptTime );
                QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                if( sUsedBlockCnt == 0 )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                    STL_THROW( RAMP_NO_DATA );
                }
                else if( sEOF == STL_TRUE )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                }
                else
                {
                    /* Do Nothing */
                }

                sOuterCacheBlockInfo->mCurrBlockIdx = 0;
                sOuterCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;
                sExeSortMergeJoin->mIsLeftMatched = STL_FALSE;
            }
            else
            {
                /* Cache에서 읽을 Block Idx를 설정 */
                sOuterCacheBlockInfo->mCurrBlockIdx++;
                sExeSortMergeJoin->mIsLeftMatched = STL_FALSE;
            }
        }

        /* Result Block에 데이터 복사 */
        QLNX_SET_OUTER_VALUE_BLOCK( &(sExeSortMergeJoin->mValueBlockList),
                                    &(sExeSortMergeJoin->mOuterCacheBlockInfo.mCurrBlockIdx),
                                    aEnv );

        sExeSortMergeJoin->mIsLeftMatched = STL_TRUE;

        /* Where Condition Logical Filter 평가 */
        if( sExeSortMergeJoin->mHasWhereCondition == STL_TRUE )
        {
            sWhereExprEvalInfo->mBlockIdx = sExeSortMergeJoin->mCurrBlockIdx;
            STL_TRY( knlEvaluateOneExpression( sWhereExprEvalInfo,
                                               KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            if( !DTL_BOOLEAN_IS_TRUE( KNL_GET_BLOCK_DATA_VALUE(
                     sWhereExprEvalInfo->mResultBlock,
                     sExeSortMergeJoin->mCurrBlockIdx ) ) )
            {
                continue;
            }
        }

        sExeSortMergeJoin->mIsLeftMatched = STL_TRUE;
        sExeSortMergeJoin->mIsRightMatched = STL_TRUE;

        /* Skip Count가 남았으면 Skip 수행 */
        if( sRemainSkipCnt > 0 )
        {
            sRemainSkipCnt--;
            continue;
        }

        /* Fetch Count 체크 */
        sExeSortMergeJoin->mCurrBlockIdx++;
        sRemainFetchCnt --;

        if( sRemainFetchCnt == 0 )
        {
            STL_THROW( RAMP_FULL_DATA );
        }
        else
        {
            /**
             * Right Table의 Cache Block을 다 읽었지만
             * 아직 Fetch Count가 남은 경우
             */

            /* Do Nothing */
        }
    }


    STL_RAMP( RAMP_NO_DATA );
    STL_RAMP( RAMP_FULL_DATA );

    if( QLNX_IS_END_OF_TABLE_FETCH( sInnerCacheBlockInfo ) &&
        QLNX_IS_END_OF_TABLE_FETCH( sOuterCacheBlockInfo ) )
    {
        sEOF = STL_TRUE;
        sExeSortMergeJoin->mIsEOF = STL_TRUE;
    }
    else
    {
        sEOF = STL_FALSE;
        sExeSortMergeJoin->mIsEOF = STL_FALSE;
    }


    /**
     * OUTPUT 설정
     */

    *aEOF = sEOF;
    *aUsedBlockCnt = sExeSortMergeJoin->mCurrBlockIdx;

    /* Parent의 Fetch Node Info 설정 */
    aFetchNodeInfo->mFetchCnt = sRemainFetchCnt;
    aFetchNodeInfo->mSkipCnt  = sRemainSkipCnt;

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT(
        sExeSortMergeJoin->mJoinedColBlock,
        0,
        sExeSortMergeJoin->mCurrBlockIdx );

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeSortMergeJoin->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeSortMergeJoin->mCommonInfo.mFetchCallCount, 1 );
    QLL_OPT_ADD_RECORD_STAT_INFO( sExeSortMergeJoin->mCommonInfo.mFetchRecordStat,
                                  sExeSortMergeJoin->mCommonInfo.mResultColBlock,
                                  *aUsedBlockCnt,
                                  aEnv );

    sExeSortMergeJoin->mCommonInfo.mFetchRowCnt += *aUsedBlockCnt;
 
    /* Outer Column의 DstDataValue 정보 복구 */
    sOuterColumnBlockItem = sExeSortMergeJoin->mLeftOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );
    sOuterColumnBlockItem = sExeSortMergeJoin->mRightOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );


    return STL_SUCCESS;

    STL_FINISH;

    /* Outer Column의 DstDataValue 정보 복구 */
    sOuterColumnBlockItem = sExeSortMergeJoin->mLeftOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );
    sOuterColumnBlockItem = sExeSortMergeJoin->mRightOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeSortMergeJoin->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeSortMergeJoin->mCommonInfo.mFetchCallCount, 1 );

    return STL_FAILURE;
}


/**
 * @brief Index를 이용한 Semi Join에 대한 SORT MERGE JOIN node를 Fetch한다.
 * @param[in]      aFetchNodeInfo  Fetch Node Info
 * @param[out]     aUsedBlockCnt   Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF            End Of Fetch 여부
 * @param[in]      aEnv            Environment
 *
 * @remark Fetch 수행시 호출된다.
 */
stlStatus qlnxFetchSortMergeJoin_Index_Semi( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                             stlInt64              * aUsedBlockCnt,
                                             stlBool               * aEOF,
                                             qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode               = NULL;
//    qlnoSortMergeJoin     * sOptSortMergeJoin       = NULL;
    qlnxSortMergeJoin     * sExeSortMergeJoin       = NULL;

    stlBool                 sEOF                    = STL_FALSE;

    stlInt64                sUsedBlockCnt           = 0;
    stlInt64                sRemainSkipCnt          = 0;
    stlInt64                sRemainFetchCnt         = 0;

    qlnxCacheBlockInfo    * sInnerCacheBlockInfo    = NULL;
    qlnxCacheBlockInfo    * sOuterCacheBlockInfo    = NULL;

    knlExprEvalInfo       * sJoinExprEvalInfo       = NULL;
    knlExprEvalInfo       * sWhereExprEvalInfo      = NULL;

    qlnxOuterColumnBlockItem  * sOuterColumnBlockItem   = NULL;

    dtlCompareResult            sCompareResult          = DTL_COMPARISON_EQUAL;

    qlnxFetchNodeInfo       sLocalFetchInfo;

    QLL_OPT_DECLARE( stlTime sBeginFetchTime );
    QLL_OPT_DECLARE( stlTime sEndFetchTime );
    QLL_OPT_DECLARE( stlTime sFetchTime );

    QLL_OPT_DECLARE( stlTime sBeginExceptTime );
    QLL_OPT_DECLARE( stlTime sEndExceptTime );
    QLL_OPT_DECLARE( stlTime sExceptTime );

    QLL_OPT_SET_VALUE( sFetchTime, 0 );
    QLL_OPT_SET_VALUE( sExceptTime, 0 );


    /*****************************************
     * Parameter Validation
     ****************************************/

    STL_PARAM_VALIDATE( aFetchNodeInfo->mOptNode->mType == QLL_PLAN_NODE_MERGE_JOIN_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    QLL_OPT_CHECK_TIME( sBeginFetchTime );

    /*****************************************
     * 기본 정보 획득
     ****************************************/

    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aFetchNodeInfo->mDataArea,
                                        QLL_GET_OPT_NODE_IDX( aFetchNodeInfo->mOptNode ) );


    /**
     * SORT MERGE JOIN Execution Node 획득
     */

    sExeSortMergeJoin = (qlnxSortMergeJoin*) sExecNode->mExecNode;


    /**
     * SORT MERGE JOIN Optimization Node 획득
     */

//    sOptSortMergeJoin = (qlnoSortMergeJoin *) aOptNode->mOptNode;


    /**
     * Node 수행 여부 확인 
     */

    STL_DASSERT( *aEOF == STL_FALSE );
    STL_DASSERT( aFetchNodeInfo->mFetchCnt > 0);


    /**
     * Used Block Count 초기화
     */

    sUsedBlockCnt = 0;


    /**
     * Cache Block Info 정보 설정
     */

    sInnerCacheBlockInfo = &(sExeSortMergeJoin->mInnerCacheBlockInfo);
    sOuterCacheBlockInfo = &(sExeSortMergeJoin->mOuterCacheBlockInfo);


    /**
     * Evaluate Info 관련 정보 설정
     */

    sJoinExprEvalInfo   = &sExeSortMergeJoin->mJoinExprEvalInfo;
    sWhereExprEvalInfo  = &sExeSortMergeJoin->mWhereExprEvalInfo;


    /**
     * Local Fetch Info 초기화
     */
    
    QLNX_SET_LOCAL_FETCH_INFO( & sLocalFetchInfo, aFetchNodeInfo );


    /*****************************************
     * Node 수행 여부 확인
     ****************************************/

    STL_DASSERT( sExeSortMergeJoin->mIsEOF == STL_FALSE );
    if( sExeSortMergeJoin->mHasFalseFilter == STL_TRUE )
    {
        QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
        QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
        sExeSortMergeJoin->mIsFirstFetch = STL_FALSE;
        sExeSortMergeJoin->mIsEOF = STL_TRUE;

        STL_THROW( RAMP_NO_DATA );
    }
    else
    {
        /* Do Nothing */
    }


    /**
     * Fetch에 필요한 정보 설정
     */

    sRemainSkipCnt = aFetchNodeInfo->mSkipCnt;
    sRemainFetchCnt = ( aFetchNodeInfo->mFetchCnt < aFetchNodeInfo->mDataArea->mBlockAllocCnt
                        ? aFetchNodeInfo->mFetchCnt
                        : aFetchNodeInfo->mDataArea->mBlockAllocCnt );


    /*****************************************
     * Fetch 수행
     ****************************************/

    /**
     * Outer Table의 Cache Block에 아직 읽을 데이터가 남았는지 체크
     */

    if( QLNX_NEED_NEXT_FETCH( sOuterCacheBlockInfo ) )
    {
        /* EOF 체크 */
        STL_TRY_THROW( sOuterCacheBlockInfo->mEOF == STL_FALSE,
                       RAMP_NO_DATA );

        /* Outer Table의 Fetch를 수행 */
        QLL_OPT_CHECK_TIME( sBeginExceptTime );

        sEOF = STL_FALSE;
        QLNX_FETCH_NODE( & sLocalFetchInfo,
                         sExeSortMergeJoin->mOuterCacheBlockInfo.mNodeIdx,
                         0, /* Start Idx */
                         0, /* Skip Count */
                         QLL_FETCH_COUNT_MAX, /* Fetch Count */
                         & sUsedBlockCnt,
                         & sEOF,
                         aEnv );

        QLL_OPT_CHECK_TIME( sEndExceptTime );
        QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

        if( sUsedBlockCnt == 0 )
        {
            QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
            QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
            STL_THROW( RAMP_NO_DATA );            
        }
        else if( sEOF == STL_TRUE )
        {
            sExeSortMergeJoin->mIsEOF = STL_TRUE;
            QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
        }
        else
        {
            /* Do Nothing */
        }
        
        sOuterCacheBlockInfo->mCurrBlockIdx = 0;
        sOuterCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;
    }
    else
    {
        /**
         * Cache에서 읽을 Block Idx를 설정
         */

        sOuterCacheBlockInfo->mCurrBlockIdx++;
    }

    if( sExeSortMergeJoin->mIsFirstFetch == STL_TRUE )
    {
        /* Inner Table의 Fetch를 수행 */
        STL_DASSERT( QLNX_NEED_NEXT_FETCH( sInnerCacheBlockInfo ) );

        QLL_OPT_CHECK_TIME( sBeginExceptTime );

        sEOF = STL_FALSE;
        QLNX_FETCH_NODE( & sLocalFetchInfo,
                         sExeSortMergeJoin->mInnerCacheBlockInfo.mNodeIdx,
                         0, /* Start Idx */
                         0, /* Skip Count */
                         QLL_FETCH_COUNT_MAX, /* Fetch Count */
                         & sUsedBlockCnt,
                         & sEOF,
                         aEnv );

        QLL_OPT_CHECK_TIME( sEndExceptTime );
        QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

        if( sUsedBlockCnt == 0 )
        {
            QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
            QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
            STL_THROW( RAMP_NO_DATA );
        }
        else if( sEOF == STL_TRUE )
        {
            sExeSortMergeJoin->mIsEOF = STL_TRUE;
            QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
        }
        else
        {
            /* Do Nothing */
        }

        sInnerCacheBlockInfo->mCurrBlockIdx = 0;
        sInnerCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;

        sExeSortMergeJoin->mIsFirstFetch = STL_FALSE;
    }

    while( 1 )
    {
        /**
         * Block Read
         */

        STL_RAMP( RAMP_RETRY_COMPARE );


        /* Sort Key Compare 수행 */
        QLNX_COMPARE_SORT_KEY( &(sExeSortMergeJoin->mSortKeyCompareList),
                               sOuterCacheBlockInfo->mCurrBlockIdx,
                               sInnerCacheBlockInfo->mCurrBlockIdx,
                               sCompareResult,
                               aEnv );

        if( sCompareResult == DTL_COMPARISON_EQUAL )
        {
            /* Join Condition Logical Filter 평가 */
            if( sExeSortMergeJoin->mHasJoinCondition == STL_TRUE )
            {
                /* Left Outer Column에 대한 Data Value 재설정 */
                sOuterColumnBlockItem = sExeSortMergeJoin->mLeftOuterColumnBlockList.mHead;
                QLNX_SET_OUTER_COLUMN_BLOCK_LIST( sOuterColumnBlockItem, aEnv );

                /* Right Outer Column에 대한 Data Value 재설정 */
                sOuterColumnBlockItem = sExeSortMergeJoin->mRightOuterColumnBlockList.mHead;
                QLNX_SET_OUTER_COLUMN_BLOCK_LIST( sOuterColumnBlockItem, aEnv );

                /* Join Condition 평가 */
                sJoinExprEvalInfo->mBlockIdx = sExeSortMergeJoin->mCurrBlockIdx;
                STL_TRY( knlEvaluateOneExpression( sJoinExprEvalInfo,
                                                   KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                if( !DTL_BOOLEAN_IS_TRUE( KNL_GET_BLOCK_DATA_VALUE(
                                              sJoinExprEvalInfo->mResultBlock,
                                              sExeSortMergeJoin->mCurrBlockIdx ) ) )
                {
                    STL_THROW( RAMP_READ_NEXT_ONE_ROW );
                }
            }


            /**
             * Outer Table의 데이터와 Inner Table의 데이터에 대하여
             * Sort Merge Join 수행
             */

            if( sExeSortMergeJoin->mJoinedColBlock != NULL )
            {
                /* Result Block에 데이터 복사 */
                QLNX_SET_VALUE_BLOCK( &(sExeSortMergeJoin->mValueBlockList), aEnv );
            }

            /* Where Condition Logical Filter 평가 */
            if( sExeSortMergeJoin->mHasWhereCondition == STL_TRUE )
            {
                sWhereExprEvalInfo->mBlockIdx = sExeSortMergeJoin->mCurrBlockIdx;
                STL_TRY( knlEvaluateOneExpression( sWhereExprEvalInfo,
                                                   KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                if( !DTL_BOOLEAN_IS_TRUE(
                        KNL_GET_BLOCK_DATA_VALUE(
                            sWhereExprEvalInfo->mResultBlock,
                            sExeSortMergeJoin->mCurrBlockIdx ) ) )
                {
                    STL_THROW( RAMP_READ_NEXT_ONE_ROW );
                }
            }

            /* Skip Count가 남았으면 Skip 수행 */
            if( sRemainSkipCnt > 0 )
            {
                sRemainSkipCnt--;

                STL_THROW( RAMP_READ_NEXT_ONE_ROW );
            }

            /* Row Block들을 Result Block에 복사 */
            QLNX_SET_ROW_BLOCK( &(sExeSortMergeJoin->mRowBlockList),
                                sExeSortMergeJoin->mCurrBlockIdx,
                                sOuterCacheBlockInfo->mCurrBlockIdx,
                                sInnerCacheBlockInfo->mCurrBlockIdx );

            /* Fetch Count 체크 */
            sExeSortMergeJoin->mCurrBlockIdx++;
            sRemainFetchCnt --;

            if( sRemainFetchCnt == 0 )
            {
                sEOF = STL_FALSE;
                sExeSortMergeJoin->mIsEOF = STL_FALSE;
                STL_THROW( RAMP_FULL_DATA );
            }
            else
            {
                /**
                 * Right Table의 Cache Block을 다 읽었지만
                 * 아직 Fetch Count가 남은 경우
                 */

                /* Do Nothing */
            }


            STL_RAMP( RAMP_READ_NEXT_ONE_ROW );


            /**
             * Outer Table의 Cache Block에 아직 읽을 데이터가 남았는지 체크
             */

            if( QLNX_NEED_NEXT_FETCH( sOuterCacheBlockInfo ) )
            {
                /* EOF 체크 */
                STL_TRY_THROW( sOuterCacheBlockInfo->mEOF == STL_FALSE,
                               RAMP_NO_DATA );


                /* Outer Cache Block에 새로운 데이터를 Fetch */
                QLL_OPT_CHECK_TIME( sBeginExceptTime );

                sEOF = STL_FALSE;
                QLNX_FETCH_NODE( & sLocalFetchInfo,
                                 sExeSortMergeJoin->mOuterCacheBlockInfo.mNodeIdx,
                                 0, /* Start Idx */
                                 0, /* Skip Count */
                                 QLL_FETCH_COUNT_MAX, /* Fetch Count */
                                 & sUsedBlockCnt,
                                 & sEOF,
                                 aEnv );

                QLL_OPT_CHECK_TIME( sEndExceptTime );
                QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                if( sUsedBlockCnt == 0 )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                    QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                    STL_THROW( RAMP_NO_DATA );
                }
                else if( sEOF == STL_TRUE )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                }
                else
                {
                    /* Do Nothing */
                }

                sOuterCacheBlockInfo->mCurrBlockIdx = 0;
                sOuterCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;
            }
            else
            {
                /**
                 * Cache에서 읽을 Block Idx를 설정
                 */

                sOuterCacheBlockInfo->mCurrBlockIdx++;
            }

            STL_THROW( RAMP_RETRY_COMPARE );
        }
        else if( sCompareResult == DTL_COMPARISON_LESS )
        {
            /**
             * Outer Table의 Cache Block에 아직 읽을 데이터가 남았는지 체크
             */

            if( QLNX_NEED_NEXT_FETCH( sOuterCacheBlockInfo ) )
            {
                /**
                 * EOF 체크
                 */

                STL_TRY_THROW( sOuterCacheBlockInfo->mEOF == STL_FALSE,
                               RAMP_NO_DATA );


                /**
                 * Outer Cache Block에 새로운 데이터를 Fetch
                 */

                QLL_OPT_CHECK_TIME( sBeginExceptTime );

                sEOF = STL_FALSE;
                QLNX_FETCH_NODE( & sLocalFetchInfo,
                                 sExeSortMergeJoin->mOuterCacheBlockInfo.mNodeIdx,
                                 0, /* Start Idx */
                                 0, /* Skip Count */
                                 QLL_FETCH_COUNT_MAX, /* Fetch Count */
                                 & sUsedBlockCnt,
                                 & sEOF,
                                 aEnv );

                QLL_OPT_CHECK_TIME( sEndExceptTime );
                QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                if( sUsedBlockCnt == 0 )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                    QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                    STL_THROW( RAMP_NO_DATA );
                }
                else if( sEOF == STL_TRUE )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                }
                else
                {
                    /* Do Nothing */
                }

                sOuterCacheBlockInfo->mCurrBlockIdx = 0;
                sOuterCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;
            }
            else
            {
                /**
                 * Cache에서 읽을 Block Idx를 설정
                 */

                sOuterCacheBlockInfo->mCurrBlockIdx++;
            }

            STL_THROW( RAMP_RETRY_COMPARE );
        }
        else
        {
            /**
             * Inner Table의 Cache Block에 아직 읽을 데이터가 남았는지 체크
             */

            if( QLNX_NEED_NEXT_FETCH( sInnerCacheBlockInfo ) )
            {
                /**
                 * EOF 체크
                 */

                STL_TRY_THROW( sInnerCacheBlockInfo->mEOF == STL_FALSE,
                               RAMP_NO_DATA );

                /**
                 * Cache에 새로운 데이터를 Fetch
                 */

                QLL_OPT_CHECK_TIME( sBeginExceptTime );

                sEOF = STL_FALSE;
                QLNX_FETCH_NODE( & sLocalFetchInfo,
                                 sExeSortMergeJoin->mInnerCacheBlockInfo.mNodeIdx,
                                 0, /* Start Idx */
                                 0, /* Skip Count */
                                 QLL_FETCH_COUNT_MAX, /* Fetch Count */
                                 & sUsedBlockCnt,
                                 & sEOF,
                                 aEnv );

                QLL_OPT_CHECK_TIME( sEndExceptTime );
                QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                if( sUsedBlockCnt == 0 )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                    STL_THROW( RAMP_NO_DATA );
                }
                else if( sEOF == STL_TRUE )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                }
                else
                {
                    /* Do Nothing */
                }
        
                sInnerCacheBlockInfo->mCurrBlockIdx = 0;
                sInnerCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;
            }
            else
            {
                /**
                 * Cache에서 읽을 Block Idx를 설정
                 */

                sInnerCacheBlockInfo->mCurrBlockIdx++;
            }

            STL_THROW( RAMP_RETRY_COMPARE );
        }
    }

    STL_RAMP( RAMP_NO_DATA );

    STL_DASSERT( QLNX_IS_END_OF_TABLE_FETCH( sInnerCacheBlockInfo ) ||
                 QLNX_IS_END_OF_TABLE_FETCH( sOuterCacheBlockInfo ) );

    sEOF = STL_TRUE;
    sExeSortMergeJoin->mIsEOF = STL_TRUE;

    STL_RAMP( RAMP_FULL_DATA );


    /**
     * OUTPUT 설정
     */

    *aEOF = sEOF;
    *aUsedBlockCnt = sExeSortMergeJoin->mCurrBlockIdx;

    /* Parent의 Fetch Node Info 설정 */
    aFetchNodeInfo->mFetchCnt = sRemainFetchCnt;
    aFetchNodeInfo->mSkipCnt  = sRemainSkipCnt;

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT(
        sExeSortMergeJoin->mJoinedColBlock,
        0,
        sExeSortMergeJoin->mCurrBlockIdx );

    QLNX_SET_USED_BLOCK_SIZE_ON_ROW_BLOCK( &(sExeSortMergeJoin->mRowBlockList),
                                           sExeSortMergeJoin->mCurrBlockIdx );

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeSortMergeJoin->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeSortMergeJoin->mCommonInfo.mFetchCallCount, 1 );
    QLL_OPT_ADD_RECORD_STAT_INFO( sExeSortMergeJoin->mCommonInfo.mFetchRecordStat,
                                  sExeSortMergeJoin->mCommonInfo.mResultColBlock,
                                  *aUsedBlockCnt,
                                  aEnv );

    sExeSortMergeJoin->mCommonInfo.mFetchRowCnt += *aUsedBlockCnt;
 
    /* Outer Column의 DstDataValue 정보 복구 */
    sOuterColumnBlockItem = sExeSortMergeJoin->mLeftOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );
    sOuterColumnBlockItem = sExeSortMergeJoin->mRightOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );


    return STL_SUCCESS;

    STL_FINISH;

    /* Outer Column의 DstDataValue 정보 복구 */
    sOuterColumnBlockItem = sExeSortMergeJoin->mLeftOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );
    sOuterColumnBlockItem = sExeSortMergeJoin->mRightOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeSortMergeJoin->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeSortMergeJoin->mCommonInfo.mFetchCallCount, 1 );

    return STL_FAILURE;
}


/**
 * @brief Index를 이용한 Anti Semi Join에 대한 SORT MERGE JOIN node를 Fetch한다.
 * @param[in]      aFetchNodeInfo  Fetch Node Info
 * @param[out]     aUsedBlockCnt   Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF            End Of Fetch 여부
 * @param[in]      aEnv            Environment
 *
 * @remark Fetch 수행시 호출된다.
 */
stlStatus qlnxFetchSortMergeJoin_Index_AntiSemi( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                                 stlInt64              * aUsedBlockCnt,
                                                 stlBool               * aEOF,
                                                 qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode               = NULL;
//    qlnoSortMergeJoin     * sOptSortMergeJoin       = NULL;
    qlnxSortMergeJoin     * sExeSortMergeJoin       = NULL;

    stlBool                 sEOF                    = STL_FALSE;
    stlBool                 sResult                 = STL_FALSE;

    stlInt64                sUsedBlockCnt           = 0;
    stlInt64                sRemainSkipCnt          = 0;
    stlInt64                sRemainFetchCnt         = 0;

    qlnxCacheBlockInfo    * sInnerCacheBlockInfo    = NULL;
    qlnxCacheBlockInfo    * sOuterCacheBlockInfo    = NULL;

    knlExprEvalInfo       * sJoinExprEvalInfo       = NULL;
    knlExprEvalInfo       * sWhereExprEvalInfo      = NULL;

    qlnxOuterColumnBlockItem  * sOuterColumnBlockItem   = NULL;

    dtlCompareResult            sCompareResult          = DTL_COMPARISON_EQUAL;
    stlBool                     sIsMatchedJoinCond;

    qlnxFetchNodeInfo       sLocalFetchInfo;

    QLL_OPT_DECLARE( stlTime sBeginFetchTime );
    QLL_OPT_DECLARE( stlTime sEndFetchTime );
    QLL_OPT_DECLARE( stlTime sFetchTime );

    QLL_OPT_DECLARE( stlTime sBeginExceptTime );
    QLL_OPT_DECLARE( stlTime sEndExceptTime );
    QLL_OPT_DECLARE( stlTime sExceptTime );

    QLL_OPT_SET_VALUE( sFetchTime, 0 );
    QLL_OPT_SET_VALUE( sExceptTime, 0 );


    /*****************************************
     * Parameter Validation
     ****************************************/

    STL_PARAM_VALIDATE( aFetchNodeInfo->mOptNode->mType == QLL_PLAN_NODE_MERGE_JOIN_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    QLL_OPT_CHECK_TIME( sBeginFetchTime );

    /*****************************************
     * 기본 정보 획득
     ****************************************/

    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aFetchNodeInfo->mDataArea,
                                        QLL_GET_OPT_NODE_IDX( aFetchNodeInfo->mOptNode ) );


    /**
     * SORT MERGE JOIN Execution Node 획득
     */

    sExeSortMergeJoin = (qlnxSortMergeJoin*) sExecNode->mExecNode;


    /**
     * SORT MERGE JOIN Optimization Node 획득
     */

//    sOptSortMergeJoin = (qlnoSortMergeJoin *) aOptNode->mOptNode;


    /**
     * Node 수행 여부 확인 
     */

    STL_DASSERT( *aEOF == STL_FALSE );
    STL_DASSERT( aFetchNodeInfo->mFetchCnt > 0);


    /**
     * Used Block Count 초기화
     */

    sUsedBlockCnt = 0;


    /**
     * Cache Block Info 정보 설정
     */

    sInnerCacheBlockInfo = &(sExeSortMergeJoin->mInnerCacheBlockInfo);
    sOuterCacheBlockInfo = &(sExeSortMergeJoin->mOuterCacheBlockInfo);


    /**
     * Evaluate Info 관련 정보 설정
     */

    sJoinExprEvalInfo   = &sExeSortMergeJoin->mJoinExprEvalInfo;
    sWhereExprEvalInfo  = &sExeSortMergeJoin->mWhereExprEvalInfo;


    /**
     * Local Fetch Info 초기화
     */
    
    QLNX_SET_LOCAL_FETCH_INFO( & sLocalFetchInfo, aFetchNodeInfo );


    /*****************************************
     * Node 수행 여부 확인
     ****************************************/

    STL_DASSERT( sExeSortMergeJoin->mIsEOF == STL_FALSE );
    if( sExeSortMergeJoin->mHasFalseFilter == STL_TRUE )
    {
        QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
        QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
        sExeSortMergeJoin->mIsFirstFetch = STL_FALSE;
        sExeSortMergeJoin->mIsEOF = STL_TRUE;

        STL_THROW( RAMP_NO_DATA );
    }
    else
    {
        /* Do Nothing */
    }


    /**
     * Fetch에 필요한 정보 설정
     */

    sRemainSkipCnt = aFetchNodeInfo->mSkipCnt;
    sRemainFetchCnt = ( aFetchNodeInfo->mFetchCnt < aFetchNodeInfo->mDataArea->mBlockAllocCnt
                        ? aFetchNodeInfo->mFetchCnt
                        : aFetchNodeInfo->mDataArea->mBlockAllocCnt );


    /*****************************************
     * Fetch 수행
     ****************************************/

    /**
     * Outer Table의 Cache Block에 아직 읽을 데이터가 남았는지 체크
     */

    if( QLNX_NEED_NEXT_FETCH( sOuterCacheBlockInfo ) )
    {
        /* EOF 체크 */
        STL_TRY_THROW( sOuterCacheBlockInfo->mEOF == STL_FALSE,
                       RAMP_NO_DATA );

        /* Outer Table의 Fetch를 수행 */
        QLL_OPT_CHECK_TIME( sBeginExceptTime );

        sEOF = STL_FALSE;
        QLNX_FETCH_NODE( & sLocalFetchInfo,
                         sExeSortMergeJoin->mOuterCacheBlockInfo.mNodeIdx,
                         0, /* Start Idx */
                         0, /* Skip Count */
                         QLL_FETCH_COUNT_MAX, /* Fetch Count */
                         & sUsedBlockCnt,
                         & sEOF,
                         aEnv );

        QLL_OPT_CHECK_TIME( sEndExceptTime );
        QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

        if( sUsedBlockCnt == 0 )
        {
            QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
            QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
            STL_THROW( RAMP_NO_DATA );            
        }
        else if( sEOF == STL_TRUE )
        {
            sExeSortMergeJoin->mIsEOF = STL_TRUE;
            QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
        }
        else
        {
            /* Do Nothing */
        }
        
        sOuterCacheBlockInfo->mCurrBlockIdx = 0;
        sOuterCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;
    }
    else
    {
        /**
         * Cache에서 읽을 Block Idx를 설정
         */

        sOuterCacheBlockInfo->mCurrBlockIdx++;
    }

    if( sExeSortMergeJoin->mIsFirstFetch == STL_TRUE )
    {
        /* Inner Table의 Fetch를 수행 */
        STL_DASSERT( QLNX_NEED_NEXT_FETCH( sInnerCacheBlockInfo ) );

        QLL_OPT_CHECK_TIME( sBeginExceptTime );

        sEOF = STL_FALSE;
        QLNX_FETCH_NODE( & sLocalFetchInfo,
                         sExeSortMergeJoin->mInnerCacheBlockInfo.mNodeIdx,
                         0, /* Start Idx */
                         0, /* Skip Count */
                         QLL_FETCH_COUNT_MAX, /* Fetch Count */
                         & sUsedBlockCnt,
                         & sEOF,
                         aEnv );

        QLL_OPT_CHECK_TIME( sEndExceptTime );
        QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

        sExeSortMergeJoin->mIsFirstFetch = STL_FALSE;

        if( sUsedBlockCnt == 0 )
        {
            QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
            STL_THROW( RAMP_END_OF_RIGHT );
        }
        else if( sEOF == STL_TRUE )
        {
            sExeSortMergeJoin->mIsEOF = STL_TRUE;
            QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
        }
        else
        {
            /* Do Nothing */
        }

        sInnerCacheBlockInfo->mCurrBlockIdx = 0;
        sInnerCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;
    }
    else
    {
        if( QLNX_IS_END_OF_TABLE_FETCH( sInnerCacheBlockInfo ) == STL_TRUE )
        {
            STL_THROW( RAMP_END_OF_RIGHT );
        }
    }
    
    
    while( 1 )
    {
        /**
         * Block Read
         */

        STL_RAMP( RAMP_RETRY_COMPARE );


        /* Sort Key Compare 수행 */
        QLNX_COMPARE_SORT_KEY( &(sExeSortMergeJoin->mSortKeyCompareList),
                               sOuterCacheBlockInfo->mCurrBlockIdx,
                               sInnerCacheBlockInfo->mCurrBlockIdx,
                               sCompareResult,
                               aEnv );

        if( sCompareResult == DTL_COMPARISON_LESS )
        {
            /* Join Condition Logical Filter 평가 */
            if( sExeSortMergeJoin->mHasJoinCondition == STL_TRUE )
            {
                /* Left Outer Column에 대한 Data Value 재설정 */
                sOuterColumnBlockItem = sExeSortMergeJoin->mLeftOuterColumnBlockList.mHead;
                QLNX_SET_OUTER_COLUMN_BLOCK_LIST( sOuterColumnBlockItem, aEnv );

                /* Right Outer Column에 대한 Data Value 재설정 */
                sOuterColumnBlockItem = sExeSortMergeJoin->mRightOuterColumnBlockList.mHead;
                QLNX_SET_OUTER_COLUMN_BLOCK_LIST( sOuterColumnBlockItem, aEnv );

                /* Join Condition 평가 */
                sJoinExprEvalInfo->mBlockIdx = sExeSortMergeJoin->mCurrBlockIdx;
                STL_TRY( knlEvaluateOneExpression( sJoinExprEvalInfo,
                                                   KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                if( DTL_BOOLEAN_IS_TRUE( KNL_GET_BLOCK_DATA_VALUE(
                                             sJoinExprEvalInfo->mResultBlock,
                                             sExeSortMergeJoin->mCurrBlockIdx ) ) )
                {
                    sIsMatchedJoinCond = STL_TRUE;
                }
                else
                {
                    sIsMatchedJoinCond = STL_FALSE;
                }
            }
            else
            {
                sIsMatchedJoinCond = STL_TRUE;
            }

            if( sIsMatchedJoinCond == STL_FALSE )
            {
                STL_THROW( RAMP_READ_NEXT_ONE_ROW );
            }

            /**
             * Outer Table의 데이터와 Inner Table의 데이터에 대하여
             * Sort Merge Join 수행
             */

            if( sExeSortMergeJoin->mJoinedColBlock != NULL )
            {
                /* Result Block에 데이터 복사 */
                QLNX_SET_VALUE_BLOCK( &(sExeSortMergeJoin->mValueBlockList), aEnv );
            }

            /* Where Condition Logical Filter 평가 */
            if( sExeSortMergeJoin->mHasWhereCondition == STL_TRUE )
            {
                sWhereExprEvalInfo->mBlockIdx = sExeSortMergeJoin->mCurrBlockIdx;
                STL_TRY( knlEvaluateOneExpression( sWhereExprEvalInfo,
                                                   KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                if( !DTL_BOOLEAN_IS_TRUE(
                        KNL_GET_BLOCK_DATA_VALUE(
                            sWhereExprEvalInfo->mResultBlock,
                            sExeSortMergeJoin->mCurrBlockIdx ) ) )
                {
                    STL_THROW( RAMP_READ_NEXT_ONE_ROW );
                }
            }

            /* Skip Count가 남았으면 Skip 수행 */
            if( sRemainSkipCnt > 0 )
            {
                sRemainSkipCnt--;

                STL_THROW( RAMP_READ_NEXT_ONE_ROW );
            }

            /* Row Block들을 Result Block에 복사 */
            QLNX_SET_ROW_BLOCK( &(sExeSortMergeJoin->mRowBlockList),
                                sExeSortMergeJoin->mCurrBlockIdx,
                                sOuterCacheBlockInfo->mCurrBlockIdx,
                                sInnerCacheBlockInfo->mCurrBlockIdx );

            /* Fetch Count 체크 */
            sExeSortMergeJoin->mCurrBlockIdx++;
            sRemainFetchCnt --;

            if( sRemainFetchCnt == 0 )
            {
                sEOF = STL_FALSE;
                sExeSortMergeJoin->mIsEOF = STL_FALSE;
                STL_THROW( RAMP_FULL_DATA );
            }
            else
            {
                /**
                 * Right Table의 Cache Block을 다 읽었지만
                 * 아직 Fetch Count가 남은 경우
                 */

                /* Do Nothing */
            }


            STL_RAMP( RAMP_READ_NEXT_ONE_ROW );


            /**
             * Outer Table의 Cache Block에 아직 읽을 데이터가 남았는지 체크
             */

            if( QLNX_NEED_NEXT_FETCH( sOuterCacheBlockInfo ) )
            {
                /* EOF 체크 */
                STL_TRY_THROW( sOuterCacheBlockInfo->mEOF == STL_FALSE,
                               RAMP_NO_DATA );


                /* Outer Cache Block에 새로운 데이터를 Fetch */
                QLL_OPT_CHECK_TIME( sBeginExceptTime );

                sEOF = STL_FALSE;
                QLNX_FETCH_NODE( & sLocalFetchInfo,
                                 sExeSortMergeJoin->mOuterCacheBlockInfo.mNodeIdx,
                                 0, /* Start Idx */
                                 0, /* Skip Count */
                                 QLL_FETCH_COUNT_MAX, /* Fetch Count */
                                 & sUsedBlockCnt,
                                 & sEOF,
                                 aEnv );

                QLL_OPT_CHECK_TIME( sEndExceptTime );
                QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                if( sUsedBlockCnt == 0 )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                    QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                    STL_THROW( RAMP_NO_DATA );
                }
                else if( sEOF == STL_TRUE )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                }
                else
                {
                    /* Do Nothing */
                }

                sOuterCacheBlockInfo->mCurrBlockIdx = 0;
                sOuterCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;
            }
            else
            {
                /**
                 * Cache에서 읽을 Block Idx를 설정
                 */

                sOuterCacheBlockInfo->mCurrBlockIdx++;
            }

            STL_THROW( RAMP_RETRY_COMPARE );
        }
        else if( sCompareResult == DTL_COMPARISON_EQUAL )
        {
            /**
             * Outer Table의 Cache Block에 아직 읽을 데이터가 남았는지 체크
             */

            if( QLNX_NEED_NEXT_FETCH( sOuterCacheBlockInfo ) )
            {
                /* EOF 체크 */
                STL_TRY_THROW( sOuterCacheBlockInfo->mEOF == STL_FALSE,
                               RAMP_NO_DATA );


                /* Outer Cache Block에 새로운 데이터를 Fetch */
                QLL_OPT_CHECK_TIME( sBeginExceptTime );

                sEOF = STL_FALSE;
                QLNX_FETCH_NODE( & sLocalFetchInfo,
                                 sExeSortMergeJoin->mOuterCacheBlockInfo.mNodeIdx,
                                 0, /* Start Idx */
                                 0, /* Skip Count */
                                 QLL_FETCH_COUNT_MAX, /* Fetch Count */
                                 & sUsedBlockCnt,
                                 & sEOF,
                                 aEnv );

                QLL_OPT_CHECK_TIME( sEndExceptTime );
                QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                if( sUsedBlockCnt == 0 )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                    QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                    STL_THROW( RAMP_NO_DATA );
                }
                else if( sEOF == STL_TRUE )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                }
                else
                {
                    /* Do Nothing */
                }

                sOuterCacheBlockInfo->mCurrBlockIdx = 0;
                sOuterCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;
            }
            else
            {
                /**
                 * Cache에서 읽을 Block Idx를 설정
                 */

                sOuterCacheBlockInfo->mCurrBlockIdx++;
            }
            
            STL_THROW( RAMP_RETRY_COMPARE );
        }
        else
        {
            /**
             * Inner Table의 Cache Block에 아직 읽을 데이터가 남았는지 체크
             */

            if( QLNX_NEED_NEXT_FETCH( sInnerCacheBlockInfo ) )
            {
                /**
                 * EOF 체크
                 */

                STL_TRY_THROW( sInnerCacheBlockInfo->mEOF == STL_FALSE,
                               RAMP_END_OF_RIGHT );

                /**
                 * Cache에 새로운 데이터를 Fetch
                 */

                QLL_OPT_CHECK_TIME( sBeginExceptTime );

                sEOF = STL_FALSE;
                QLNX_FETCH_NODE( & sLocalFetchInfo,
                                 sExeSortMergeJoin->mInnerCacheBlockInfo.mNodeIdx,
                                 0, /* Start Idx */
                                 0, /* Skip Count */
                                 QLL_FETCH_COUNT_MAX, /* Fetch Count */
                                 & sUsedBlockCnt,
                                 & sEOF,
                                 aEnv );

                QLL_OPT_CHECK_TIME( sEndExceptTime );
                QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                if( sUsedBlockCnt == 0 )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                    STL_THROW( RAMP_END_OF_RIGHT );
                }
                else if( sEOF == STL_TRUE )
                {
                    QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                }
                else
                {
                    /* Do Nothing */
                }
        
                sInnerCacheBlockInfo->mCurrBlockIdx = 0;
                sInnerCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;
            }
            else
            {
                /**
                 * Cache에서 읽을 Block Idx를 설정
                 */

                sInnerCacheBlockInfo->mCurrBlockIdx++;
            }

            STL_THROW( RAMP_RETRY_COMPARE );
        }
    }


    STL_RAMP( RAMP_END_OF_RIGHT );


    while( 1 )
    {
        if( sExeSortMergeJoin->mJoinedColBlock != NULL )
        {
            /* Result Block에 데이터 복사 */
            QLNX_SET_VALUE_BLOCK( &(sExeSortMergeJoin->mValueBlockList), aEnv );
        }

        /* Where Condition Logical Filter 평가 */
        if( sExeSortMergeJoin->mHasWhereCondition == STL_TRUE )
        {
            sWhereExprEvalInfo->mBlockIdx = sExeSortMergeJoin->mCurrBlockIdx;
            STL_TRY( knlEvaluateOneExpression( sWhereExprEvalInfo,
                                               KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            if( DTL_BOOLEAN_IS_TRUE(
                   KNL_GET_BLOCK_DATA_VALUE(
                       sWhereExprEvalInfo->mResultBlock,
                       sExeSortMergeJoin->mCurrBlockIdx ) ) )
            {
                sResult = STL_TRUE;
            }
            else
            {
                sResult = STL_FALSE;
            }
        }
        else
        {
            sResult = STL_TRUE;
        }

        if( sResult == STL_TRUE )
        {
            /* Skip Count가 남았으면 Skip 수행 */
            if( sRemainSkipCnt > 0 )
            {
                sRemainSkipCnt--;
            }
            else
            {
                /* Row Block들을 Result Block에 복사 */
                QLNX_SET_ROW_BLOCK( &(sExeSortMergeJoin->mRowBlockList),
                                    sExeSortMergeJoin->mCurrBlockIdx,
                                    sOuterCacheBlockInfo->mCurrBlockIdx,
                                    sInnerCacheBlockInfo->mCurrBlockIdx );

                /* Fetch Count 체크 */
                sExeSortMergeJoin->mCurrBlockIdx++;
                sRemainFetchCnt --;

                if( sRemainFetchCnt == 0 )
                {
                    sEOF = STL_FALSE;
                    sExeSortMergeJoin->mIsEOF = STL_FALSE;
                    STL_THROW( RAMP_FULL_DATA );
                }
                else
                {
                    /**
                     * Right Table의 Cache Block을 다 읽었지만
                     * 아직 Fetch Count가 남은 경우
                     */

                    /* Do Nothing */
                }
            }
        }


        /**
         * Outer Table의 Cache Block에 아직 읽을 데이터가 남았는지 체크
         */

        if( QLNX_NEED_NEXT_FETCH( sOuterCacheBlockInfo ) )
        {
            /* EOF 체크 */
            STL_TRY_THROW( sOuterCacheBlockInfo->mEOF == STL_FALSE,
                           RAMP_NO_DATA );


            /* Outer Cache Block에 새로운 데이터를 Fetch */
            QLL_OPT_CHECK_TIME( sBeginExceptTime );

            sEOF = STL_FALSE;
            QLNX_FETCH_NODE( & sLocalFetchInfo,
                             sExeSortMergeJoin->mOuterCacheBlockInfo.mNodeIdx,
                             0, /* Start Idx */
                             0, /* Skip Count */
                             QLL_FETCH_COUNT_MAX, /* Fetch Count */
                             & sUsedBlockCnt,
                             & sEOF,
                             aEnv );

            QLL_OPT_CHECK_TIME( sEndExceptTime );
            QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

            if( sUsedBlockCnt == 0 )
            {
                QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
                QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
                STL_THROW( RAMP_NO_DATA );
            }
            else if( sEOF == STL_TRUE )
            {
                QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
            }
            else
            {
                /* Do Nothing */
            }

            sOuterCacheBlockInfo->mCurrBlockIdx = 0;
            sOuterCacheBlockInfo->mCacheBlockCnt = sUsedBlockCnt;
        }
        else
        {
            /**
             * Cache에서 읽을 Block Idx를 설정
             */

            sOuterCacheBlockInfo->mCurrBlockIdx++;
        }
    }


    STL_RAMP( RAMP_NO_DATA );

    if( QLNX_IS_END_OF_TABLE_FETCH( sInnerCacheBlockInfo ) ||
        QLNX_IS_END_OF_TABLE_FETCH( sOuterCacheBlockInfo ) )
    {
        sEOF = STL_TRUE;
        sExeSortMergeJoin->mIsEOF = STL_TRUE;
    }
    else
    {
        sEOF = STL_FALSE;
        sExeSortMergeJoin->mIsEOF = STL_FALSE;
    }


    STL_RAMP( RAMP_FULL_DATA );


    /**
     * OUTPUT 설정
     */

    *aEOF = sEOF;
    *aUsedBlockCnt = sExeSortMergeJoin->mCurrBlockIdx;

    /* Parent의 Fetch Node Info 설정 */
    aFetchNodeInfo->mFetchCnt = sRemainFetchCnt;
    aFetchNodeInfo->mSkipCnt  = sRemainSkipCnt;

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT(
        sExeSortMergeJoin->mJoinedColBlock,
        0,
        sExeSortMergeJoin->mCurrBlockIdx );

    QLNX_SET_USED_BLOCK_SIZE_ON_ROW_BLOCK( &(sExeSortMergeJoin->mRowBlockList),
                                           sExeSortMergeJoin->mCurrBlockIdx );

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeSortMergeJoin->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeSortMergeJoin->mCommonInfo.mFetchCallCount, 1 );
    QLL_OPT_ADD_RECORD_STAT_INFO( sExeSortMergeJoin->mCommonInfo.mFetchRecordStat,
                                  sExeSortMergeJoin->mCommonInfo.mResultColBlock,
                                  *aUsedBlockCnt,
                                  aEnv );

    sExeSortMergeJoin->mCommonInfo.mFetchRowCnt += *aUsedBlockCnt;
 
    /* Outer Column의 DstDataValue 정보 복구 */
    sOuterColumnBlockItem = sExeSortMergeJoin->mLeftOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );
    sOuterColumnBlockItem = sExeSortMergeJoin->mRightOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );


    return STL_SUCCESS;

    STL_FINISH;

    /* Outer Column의 DstDataValue 정보 복구 */
    sOuterColumnBlockItem = sExeSortMergeJoin->mLeftOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );
    sOuterColumnBlockItem = sExeSortMergeJoin->mRightOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeSortMergeJoin->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeSortMergeJoin->mCommonInfo.mFetchCallCount, 1 );

    return STL_FAILURE;
}


/**
 * @brief SORT MERGE JOIN node를 Fetch한다.
 * @param[in]      aFetchNodeInfo  Fetch Node Info
 * @param[out]     aUsedBlockCnt   Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF            End Of Fetch 여부
 * @param[in]      aEnv            Environment
 *
 * @remark Fetch 수행시 호출된다.
 */
stlStatus qlnxFetchSortMergeJoin( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                  stlInt64              * aUsedBlockCnt,
                                  stlBool               * aEOF,
                                  qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode               = NULL;
    qlnxSortMergeJoin     * sExeSortMergeJoin       = NULL;


    /*****************************************
     * Parameter Validation
     ****************************************/

    STL_PARAM_VALIDATE( aFetchNodeInfo->mOptNode->mType == QLL_PLAN_NODE_MERGE_JOIN_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    /*****************************************
     * 기본 정보 획득
     ****************************************/

    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aFetchNodeInfo->mDataArea,
                                        QLL_GET_OPT_NODE_IDX( aFetchNodeInfo->mOptNode ) );


    /**
     * SORT MERGE JOIN Execution Node 획득
     */

    sExeSortMergeJoin = (qlnxSortMergeJoin*) sExecNode->mExecNode;


    /**
     * Node 수행 여부 확인 
     */

    STL_DASSERT( *aEOF == STL_FALSE );
    STL_DASSERT( aFetchNodeInfo->mFetchCnt > 0);


    /**
     * Current Block Idx 초기화
     */

    sExeSortMergeJoin->mCurrBlockIdx = 0;


    /**
     * Join Type 및 Index 여부에 따라 다른 함수로 처리한다.
     */

    *aUsedBlockCnt = 0;
    STL_TRY( ((qlnxMergeJoinFetchFuncPtr)(sExeSortMergeJoin->mFetchFunc))(
                   aFetchNodeInfo,
                   aUsedBlockCnt,
                   aEOF,
                   aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SORT MERGE JOIN node 수행을 종료한다.
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 *
 * @remark Execution을 종료하고자 할 때 호출한다.
 */
stlStatus qlnxFinalizeSortMergeJoin( qllOptimizationNode   * aOptNode,
                                     qllDataArea           * aDataArea,
                                     qllEnv                * aEnv )
{
    /* qllExecutionNode      * sExecNode               = NULL; */
    /* qlnxSortMergeJoin     * sExeSortMergeJoin       = NULL; */


    /*****************************************
     * Parameter Validation
     ****************************************/

    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_MERGE_JOIN_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    /*****************************************
     * 기본 정보 획득
     ****************************************/

    /**
     * Common Execution Node 획득
     */

    /* sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) ); */


    /**
     * SORT MERGE JOIN Execution Node 획득
     */

    /* sExeSortMergeJoin = (qlnxSortMergeJoin*)sExecNode->mExecNode; */


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}




/** @} qlnx */
