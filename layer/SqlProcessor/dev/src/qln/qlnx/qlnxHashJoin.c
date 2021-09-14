/*******************************************************************************
 * qlnxHashJoin.c
 *
 * Copyright (c) 2013, SUNJESOFT Inc.
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
 * @file qlnxHashJoin.c
 * @brief SQL Processor Layer Execution Node - HASH JOIN
 */

#include <qll.h>
#include <qlDef.h>

/**
 * @addtogroup qlnx
 * @{
 */


/**
 * @brief HASH JOIN node를 초기화한다.
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
stlStatus qlnxInitializeHashJoin( smlTransId              aTransID,
                                  smlStatement          * aStmt,
                                  qllDBCStmt            * aDBCStmt,
                                  qllStatement          * aSQLStmt,
                                  qllOptimizationNode   * aOptNode,
                                  qllDataArea           * aDataArea,
                                  qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode           = NULL;
    qlnxHashJoin          * sExeHashJoin        = NULL;


    /*****************************************
     * Parameter Validation
     ****************************************/

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_HASH_JOIN_TYPE,
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
     * HASH JOIN Execution Node 할당
     */

    sExeHashJoin = (qlnxHashJoin *) sExecNode->mExecNode;

    sExeHashJoin->mHasFalseFilter = STL_FALSE;
    
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief HASH JOIN node를 수행한다.
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
stlStatus qlnxExecuteHashJoin( smlTransId              aTransID,
                               smlStatement          * aStmt,
                               qllDBCStmt            * aDBCStmt,
                               qllStatement          * aSQLStmt,
                               qllOptimizationNode   * aOptNode,
                               qllDataArea           * aDataArea,
                               qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode               = NULL;
    qlnoHashJoin          * sOptHashJoin            = NULL;
    qlnxHashJoin          * sExeHashJoin            = NULL;
    qlnxCacheBlockInfo    * sCacheBlockInfo         = NULL;

    qllExecutionNode      * sInstantExecNode  = NULL;
    qlnxInstant           * sExeHashInstant   = NULL;
    smlBlockJoinFetchInfo * sJoinFetchInfo    = NULL;

    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_HASH_JOIN_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /*****************************************
     * 기본 정보 획득
     ****************************************/

    /**
     * HASH JOIN Optimization Node 획득
     */

    sOptHashJoin = (qlnoHashJoin *) aOptNode->mOptNode;


    /**
     * Common Execution Node 획득
     */

    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * HASH JOIN Execution Node 획득
     */

    sExeHashJoin = (qlnxHashJoin*)sExecNode->mExecNode;


    /*****************************************
     * Outer Table 및 Inner Table 초기화
     ****************************************/

    /**
     * @todo 불필요한 요소를 제거하여 Filter 재구성 (확장 필요)
     */
    
    sExeHashJoin->mHasFalseFilter =
        qlfHasFalseFilter( sOptHashJoin->mWhereFilterExpr,
                           aDataArea );


    /**
     * Outer Table 초기화
     */

    sCacheBlockInfo = &(sExeHashJoin->mOuterCacheBlockInfo);
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

    sCacheBlockInfo = &(sExeHashJoin->mInnerCacheBlockInfo);
    QLNX_EXECUTE_NODE( aTransID,
                       aStmt,
                       aDBCStmt,
                       aSQLStmt,
                       aDataArea->mExecNodeList[sCacheBlockInfo->mNodeIdx].mOptNode,
                       aDataArea,
                       aEnv );
    QLNX_SET_END_OF_TABLE_FETCH( sCacheBlockInfo );


    /*****************************************
     * Reset을 위한 기타 정보들 초기화
     ****************************************/

    sInstantExecNode  = QLL_GET_EXECUTION_NODE( aDataArea, sCacheBlockInfo->mNodeIdx );
    sExeHashInstant   = sInstantExecNode->mExecNode;
    sJoinFetchInfo    = sExeHashInstant->mFetchInfo.mBlockJoinFetchInfo;

    /* Result Block Index */
    sExeHashJoin->mCurrBlockIdx = 0;

    /* First Fetch 여부 */
    sExeHashJoin->mIsFirstFetch = STL_TRUE;

    /* EOF 여부 */
    sExeHashJoin->mIsEOF = STL_FALSE;

    /* Inner와 Outer의 Switching 여부 */
    sExeHashJoin->mIsSwitched = STL_FALSE;

    /* subquery 를 위한 초기화 */
    if ( sExeHashJoin->mJoinType == QLV_JOIN_TYPE_FULL_OUTER )
    {
        sJoinFetchInfo->mBlockJoinType = KNL_BLOCK_JOIN_LEFT_OUTER_JOIN;
    }
    else
    {
        /* nothing to do */
    }

    /* End Of Join 초기화 */
    sJoinFetchInfo->mIsEndOfJoin = STL_FALSE;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}





/**
 * @brief HASH JOIN node를 Fetch한다.
 * @param[in]      aFetchNodeInfo  Fetch Node Info
 * @param[out]     aUsedBlockCnt   Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF            End Of Fetch 여부
 * @param[in]      aEnv            Environment
 *
 * @remark Fetch 수행시 호출된다.
 */
stlStatus qlnxFetchHashJoin( qlnxFetchNodeInfo     * aFetchNodeInfo,
                             stlInt64              * aUsedBlockCnt,
                             stlBool               * aEOF,
                             qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode               = NULL;
    qlnxHashJoin          * sExeHashJoin            = NULL;

    stlBool                 sEOF                    = STL_FALSE;
    stlBool                 sRightEOF               = STL_FALSE;

    stlInt64                sPrevReadCnt            = 0;
    stlInt64                sUsedBlockCnt           = 0;
    stlInt64                sRemainSkipCnt          = aFetchNodeInfo->mSkipCnt;
    stlInt64                sRemainFetchCnt         = aFetchNodeInfo->mFetchCnt;

    qlnxCacheBlockInfo    * sOuterCacheBlockInfo    = NULL;
    qlnxCacheBlockInfo    * sInnerCacheBlockInfo    = NULL;

    qlnxRowBlockItem      * sRowBlockItem           = NULL;
    stlInt64                sTempBlockIdx           = 0;

    qllExecutionNode      * sInstantExecNode = NULL;
    qlnxInstant           * sExeHashInstant  = NULL;
    qlnInstantTable       * sInstantTable    = NULL;
    
    smlFetchInfo          * sFetchInfo       = NULL;
    smlBlockJoinFetchInfo * sJoinFetchInfo   = NULL;

    qlnxOuterColumnBlockItem    * sOuterColumnBlockItem = NULL;

    stlInt32  i = 0;

    qlnxFetchNodeInfo       sLocalFetchInfo;

    QLL_OPT_DECLARE( stlTime sBeginFetchTime );
    QLL_OPT_DECLARE( stlTime sEndFetchTime );
    QLL_OPT_DECLARE( stlTime sFetchTime );

    QLL_OPT_DECLARE( stlTime sBeginInstantFetchTime );
    QLL_OPT_DECLARE( stlTime sEndInstantFetchTime );
    QLL_OPT_DECLARE( stlTime sInstantFetchTime );

    QLL_OPT_DECLARE( stlTime sBeginExceptTime );
    QLL_OPT_DECLARE( stlTime sEndExceptTime );
    QLL_OPT_DECLARE( stlTime sExceptTime );

    QLL_OPT_SET_VALUE( sFetchTime, 0 );
    QLL_OPT_SET_VALUE( sInstantFetchTime, 0 );
    QLL_OPT_SET_VALUE( sExceptTime, 0 );


    /*****************************************
     * Parameter Validation
     ****************************************/

    STL_PARAM_VALIDATE( aFetchNodeInfo->mOptNode->mType == QLL_PLAN_NODE_HASH_JOIN_TYPE,
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
     * Node 수행 여부 확인 
     */

    STL_DASSERT( *aEOF == STL_FALSE );
    STL_DASSERT( aFetchNodeInfo->mFetchCnt > 0);


    /**
     * HASH JOIN Execution Node 획득
     */

    sExeHashJoin = (qlnxHashJoin*)sExecNode->mExecNode;

    sOuterCacheBlockInfo = &(sExeHashJoin->mOuterCacheBlockInfo);
    sInnerCacheBlockInfo = &(sExeHashJoin->mInnerCacheBlockInfo);
    
    sInstantExecNode = QLL_GET_EXECUTION_NODE( aFetchNodeInfo->mDataArea,
                                               sInnerCacheBlockInfo->mNodeIdx );
    sExeHashInstant  = sInstantExecNode->mExecNode;
    sInstantTable    = & sExeHashInstant->mInstantTable;
    sFetchInfo       = & sExeHashInstant->mFetchInfo;
    sJoinFetchInfo   = sFetchInfo->mBlockJoinFetchInfo;
    
    sJoinFetchInfo->mResultBlockIdx = 0;


    /**
     * Local Fetch Info 초기화
     */
    
    QLNX_SET_LOCAL_FETCH_INFO( & sLocalFetchInfo, aFetchNodeInfo );


    /*****************************************
     * Node 수행 여부 확인
     ****************************************/
    
    if( ( sExeHashJoin->mIsEOF == STL_TRUE )|| 
        ( sExeHashJoin->mHasFalseFilter == STL_TRUE ) )
    {
        sEOF = STL_TRUE;
        STL_THROW( RAMP_NO_DATA );
    }
    else
    {
        /* Do Nothing */
    }


    /**
     * FULL OUTER JOIN 의 ANTI OUTER JOIN phase 인 경우
     */ 

    if( (sExeHashJoin->mJoinType == QLV_JOIN_TYPE_FULL_OUTER) &&
        (sExeHashJoin->mIsSwitched == STL_TRUE) )
    {
        /* full outer join 의 anti outer join phase 를 수행 */
        STL_THROW( RAMP_ANTI_OUTER_PHASE );
    }
    else
    {
        /* inner join (left outer join) phase 를 수행 */
    }

    /*****************************************
     * Fetch 수행
     ****************************************/

    sPrevReadCnt = 0;
    while( (sJoinFetchInfo->mResultBlockIdx < sLocalFetchInfo.mDataArea->mBlockAllocCnt) &&
           (sRemainFetchCnt > 0) )
    {
        /****************************************************************
         * read left plan
         ****************************************************************/
        
        sEOF = STL_FALSE;
        if ( sOuterCacheBlockInfo->mNeedScan == STL_TRUE )
        {
            QLL_OPT_CHECK_TIME( sBeginExceptTime );

            QLNX_FETCH_NODE( & sLocalFetchInfo,
                             sOuterCacheBlockInfo->mNodeIdx,
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
                if ( sExeHashJoin->mJoinType == QLV_JOIN_TYPE_FULL_OUTER )
                {
                    /**
                     * FULL OUTER JOIN 인 경우
                     */

                    if ( sInstantTable->mIsNeedBuild == STL_TRUE )
                    {
                        /**
                         * 최초 hash 를 build 하는 경우 
                         */
                        
                        QLL_OPT_CHECK_TIME( sBeginExceptTime );
                        
                        QLNX_FETCH_NODE( & sLocalFetchInfo,
                                         sInnerCacheBlockInfo->mNodeIdx,
                                         0, /* Start Idx */
                                         0, /* Skip Count */
                                         QLL_FETCH_COUNT_MAX, /* Fetch Count */
                                         & sUsedBlockCnt,
                                         & sRightEOF,
                                         aEnv );
                        
                        QLL_OPT_CHECK_TIME( sEndExceptTime );
                        QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );
                    }
                    
                    sExeHashJoin->mIsSwitched = STL_TRUE;

                    sJoinFetchInfo->mBlockJoinType = KNL_BLOCK_JOIN_ANTI_OUTER_JOIN;
        
                    STL_TRY( smlResetIterator( sInstantTable->mIterator,
                                               SML_ENV(aEnv) )
                             == STL_SUCCESS);
                    
                    STL_THROW( RAMP_ANTI_OUTER_PHASE );
                }
                else
                {
                    /* left 에 data 가 없음 */
                    sEOF = STL_TRUE;
                    STL_THROW( RAMP_NO_DATA );
                }
            }
            else
            {
                KNL_SET_ALL_BLOCK_USED_CNT( sJoinFetchInfo->mKeyBlockList,
                                            sUsedBlockCnt );
                    
                if( sEOF == STL_TRUE )
                {
                    /* left 에 data 가 있으나 다음엔 없음 */
                    sOuterCacheBlockInfo->mNeedScan = STL_FALSE;
                    sOuterCacheBlockInfo->mEOF      = STL_TRUE;
                    
                    sEOF = STL_FALSE;
                }
                else
                {
                    /* left 에 data 가 여전히 남아있음 */
                    sOuterCacheBlockInfo->mNeedScan = STL_TRUE;
                    sOuterCacheBlockInfo->mEOF      = STL_FALSE;
                }
            }
        }
        else
        {
            /* left 에 아직 data 가 남아 있음 */
        }

        /****************************************************************
         * read right plan
         ****************************************************************/

        sEOF = STL_FALSE;
        if ( sInstantTable->mIsNeedBuild == STL_TRUE )
        {
            /**
             * 최초 hash 를 build 하는 경우 
             */

            QLL_OPT_CHECK_TIME( sBeginExceptTime );

            QLNX_FETCH_NODE( & sLocalFetchInfo,
                             sInnerCacheBlockInfo->mNodeIdx,
                             0, /* Start Idx */
                             0, /* Skip Count */
                             QLL_FETCH_COUNT_MAX, /* Fetch Count */
                             & sUsedBlockCnt,
                             & sRightEOF,
                             aEnv );

            QLL_OPT_CHECK_TIME( sEndExceptTime );
            QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

            if( sInstantTable->mRecCnt == 0 )
            {
                if( (sExeHashJoin->mJoinType == QLV_JOIN_TYPE_LEFT_OUTER) ||
                    (sExeHashJoin->mJoinType == QLV_JOIN_TYPE_RIGHT_OUTER) ||
                    (sExeHashJoin->mJoinType == QLV_JOIN_TYPE_FULL_OUTER) ||
                    (sExeHashJoin->mJoinType == QLV_JOIN_TYPE_LEFT_ANTI_SEMI) ||
                    (sExeHashJoin->mJoinType == QLV_JOIN_TYPE_LEFT_ANTI_SEMI_NA) ||
                    (sExeHashJoin->mJoinType == QLV_JOIN_TYPE_RIGHT_ANTI_SEMI) ||
                    (sExeHashJoin->mJoinType == QLV_JOIN_TYPE_RIGHT_ANTI_SEMI_NA) )
                {
                    /**
                     * outer join 인 경우
                     * - right 결과가 없더라도 left row 에 대한 결과를 null padding 결과를 추출해야 함.
                     */
                }
                else
                {
                    /**
                     * inner join 인 경우
                     */
                    
                    sEOF = STL_TRUE;
                    STL_THROW( RAMP_NO_DATA );
                }
            }
        }
        else
        {
            /* 이미 hash instance 가 build 되어 있음 */
        }

        /**
         * Fetch 수행
         */
        
        sFetchInfo->mSkipCnt = sRemainSkipCnt;
        sFetchInfo->mFetchCnt = sRemainFetchCnt;

        QLL_OPT_CHECK_TIME( sBeginInstantFetchTime );

        STL_TRY( smlFetchNext( sInstantTable->mIterator,
                               sFetchInfo,
                               SML_ENV(aEnv) )
                 == STL_SUCCESS );

        QLL_OPT_CHECK_TIME( sEndInstantFetchTime );
        QLL_OPT_ADD_ELAPSED_TIME( sInstantFetchTime, sBeginInstantFetchTime, sEndInstantFetchTime );

        sRightEOF = sFetchInfo->mIsEndOfScan;

        sRemainSkipCnt = sFetchInfo->mSkipCnt;
        sRemainFetchCnt = sRemainFetchCnt - SML_USED_BLOCK_SIZE( sFetchInfo->mRowBlock );


        /**
         * Row Block들을 Result Block에 복사
         * - FOR UPDATE or SENSITIVE cursor 인 경우에 해당함.
         */

        sRowBlockItem = sExeHashJoin->mRowBlockList.mHead;
        while( sRowBlockItem != NULL )
        {
            for ( i = sPrevReadCnt; i < sJoinFetchInfo->mResultBlockIdx; i++ )
            {
                /**
                 * 원본 블럭의 idx 가져오기
                 */

                sTempBlockIdx = (sRowBlockItem->mIsLeftTableRowBlock == STL_TRUE) ?
                    sJoinFetchInfo->mResultRefLeftBlockIdx[i] : i;

                /**
                 * Scn Value 복사
                 */

                SML_SET_SCN_VALUE( sRowBlockItem->mRowBlock,
                                   i,
                                   SML_GET_SCN_VALUE( sRowBlockItem->mOrgRowBlock, sTempBlockIdx ) );


                /**
                 * Rid Value 복사
                 */

                SML_SET_RID_VALUE( sRowBlockItem->mRowBlock,
                                   i,
                                   SML_GET_RID_VALUE( sRowBlockItem->mOrgRowBlock, sTempBlockIdx ) );
            }

            sRowBlockItem = sRowBlockItem->mNext;
        }
        sPrevReadCnt = sJoinFetchInfo->mResultBlockIdx;

        if( sJoinFetchInfo->mIsEndOfJoin == STL_TRUE )
        {
            sOuterCacheBlockInfo->mNeedScan = STL_FALSE;
            sOuterCacheBlockInfo->mEOF      = STL_TRUE;

            sEOF = STL_TRUE;
            break;
        }
        
        if ( sRightEOF == STL_TRUE )
        {
            /**
             * left block 에 해당하는 result 가 모두 생성됨
             */
            
            if ( sOuterCacheBlockInfo->mEOF == STL_TRUE )
            {
                /**
                 * left 를 다시 읽을 필요가 없음
                 */
                sOuterCacheBlockInfo->mNeedScan = STL_FALSE;
                break;
            }
            else
            {
                /**
                 * left 를 다시 읽어야 함
                 */
                sOuterCacheBlockInfo->mNeedScan = STL_TRUE;
            }
        }
        else
        {
            /**
             * left block 에 대한 result 가 아직 남아 있음
             */
            
            sOuterCacheBlockInfo->mNeedScan = STL_FALSE;
        }
    }


    /**
     * 더 이상 Fetch 할 Data 가 없는 경우인지 확인
     */

    if ( (sOuterCacheBlockInfo->mEOF == STL_TRUE) && (sRightEOF == STL_TRUE) )
    {
        if ( sExeHashJoin->mJoinType == QLV_JOIN_TYPE_FULL_OUTER )
        {
            /**
             * FULL OUTER JOIN 의 LEFT OUTER JOIN 만 완료된 경우
             */

            sJoinFetchInfo->mBlockJoinType = KNL_BLOCK_JOIN_ANTI_OUTER_JOIN;

            STL_TRY( smlResetIterator( sInstantTable->mIterator,
                                       SML_ENV(aEnv) )
                     == STL_SUCCESS);

            sExeHashJoin->mIsSwitched = STL_TRUE;
            sEOF = STL_FALSE;
        }
        else
        {
            sEOF = STL_TRUE;
        }
    }
    else
    {
        sEOF = STL_FALSE;
    }

    /**
     * FULL OUTER JOIN 의 ANTI OUTER JOIN phase 수행
     */
    
    STL_RAMP( RAMP_ANTI_OUTER_PHASE );

    if ( (sExeHashJoin->mIsSwitched == STL_TRUE) && (sRemainFetchCnt > 0) )
    {
        /**
         * Anti Outer Join 을 수행하는 경우
         */

        sFetchInfo->mSkipCnt = sRemainSkipCnt;
        sFetchInfo->mFetchCnt = sRemainFetchCnt;

        QLL_OPT_CHECK_TIME( sBeginInstantFetchTime );

        STL_TRY( smlFetchNext( sInstantTable->mIterator,
                               sFetchInfo,
                               SML_ENV(aEnv) )
                 == STL_SUCCESS );

        QLL_OPT_CHECK_TIME( sEndInstantFetchTime );
        QLL_OPT_ADD_ELAPSED_TIME( sInstantFetchTime, sBeginInstantFetchTime, sEndInstantFetchTime );

        sRemainSkipCnt = sFetchInfo->mSkipCnt;
        sRemainFetchCnt = sRemainFetchCnt - SML_USED_BLOCK_SIZE( sFetchInfo->mRowBlock );

        sEOF = sFetchInfo->mIsEndOfScan;
    }
    
    /**
     * Node 종료 정보 설정
     */

    STL_RAMP( RAMP_NO_DATA );
    
    if ( sEOF == STL_TRUE )
    {
        QLNX_SET_END_OF_TABLE_FETCH( sOuterCacheBlockInfo );
        QLNX_SET_END_OF_TABLE_FETCH( sInnerCacheBlockInfo );
        sExeHashJoin->mIsFirstFetch = STL_FALSE;
        sExeHashJoin->mIsEOF = STL_TRUE;
    }
    else
    {
        /* nothing to do */
    }

    
    /**
     * OUTPUT 설정
     */

    *aEOF = sEOF;
    *aUsedBlockCnt = sJoinFetchInfo->mResultBlockIdx;
    
    /* Parent의 Fetch Node Info 설정 */
    aFetchNodeInfo->mFetchCnt = sRemainFetchCnt;
    aFetchNodeInfo->mSkipCnt  = sRemainSkipCnt;

    if ( sJoinFetchInfo->mResultBlockList != NULL )
    {
        KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sJoinFetchInfo->mResultBlockList,
                                             0,
                                             sJoinFetchInfo->mResultBlockIdx );
    }

    sRowBlockItem = sExeHashJoin->mRowBlockList.mHead;
    while( sRowBlockItem != NULL )
    {
        SML_SET_USED_BLOCK_SIZE(
            sRowBlockItem->mRowBlock,
            sJoinFetchInfo->mResultBlockIdx );

        sRowBlockItem = sRowBlockItem->mNext;
    }

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeHashJoin->mCommonInfo.mFetchTime, sFetchTime - sInstantFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeHashJoin->mCommonInfo.mFetchCallCount, 1 );
    QLL_OPT_ADD_RECORD_STAT_INFO( sExeHashJoin->mCommonInfo.mFetchRecordStat,
                                  sExeHashJoin->mCommonInfo.mResultColBlock,
                                  *aUsedBlockCnt,
                                  aEnv );

    QLL_OPT_ADD_TIME( sExeHashInstant->mCommonInfo.mFetchTime, sInstantFetchTime );
    QLL_OPT_ADD_COUNT( sExeHashInstant->mCommonInfo.mFetchCallCount, 1 );
    QLL_OPT_ADD_RECORD_STAT_INFO( sExeHashInstant->mCommonInfo.mFetchRecordStat,
                                  sExeHashInstant->mCommonInfo.mResultColBlock,
                                  *aUsedBlockCnt,
                                  aEnv );

    sExeHashJoin->mCommonInfo.mFetchRowCnt += *aUsedBlockCnt;
    ((qlnxCommonInfo*)(sLocalFetchInfo.mDataArea->mExecNodeList[sInnerCacheBlockInfo->mNodeIdx].mExecNode))->mFetchRowCnt
        += *aUsedBlockCnt;

    /* Outer Column의 DstDataValue 정보 복구 */
    sOuterColumnBlockItem = sExeHashJoin->mLeftOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );
    sOuterColumnBlockItem = sExeHashJoin->mRightOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );


    return STL_SUCCESS;

    STL_FINISH;

    /* Outer Column의 DstDataValue 정보 복구 */
    sOuterColumnBlockItem = sExeHashJoin->mLeftOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );
    sOuterColumnBlockItem = sExeHashJoin->mRightOuterColumnBlockList.mHead;
    QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( sOuterColumnBlockItem );

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeHashJoin->mCommonInfo.mFetchTime, sFetchTime - sInstantFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeHashJoin->mCommonInfo.mFetchCallCount, 1 );

    QLL_OPT_ADD_TIME( sExeHashInstant->mCommonInfo.mFetchTime, sInstantFetchTime );
    QLL_OPT_ADD_COUNT( sExeHashInstant->mCommonInfo.mFetchCallCount, 1 );

    return STL_FAILURE;
}



/**
 * @brief HASH JOIN node 수행을 종료한다.
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 *
 * @remark Execution을 종료하고자 할 때 호출한다.
 */
stlStatus qlnxFinalizeHashJoin( qllOptimizationNode   * aOptNode,
                                qllDataArea           * aDataArea,
                                qllEnv                * aEnv )
{
    /* qllExecutionNode      * sExecNode               = NULL; */
    /* qlnxHashJoin          * sExeHashJoin            = NULL; */


    /*****************************************
     * Parameter Validation
     ****************************************/

    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_HASH_JOIN_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    /*****************************************
     * 기본 정보 획득
     ****************************************/

    /**
     * Common Execution Node 획득
     */

    /* sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) ); */


    /**
     * HASH JOIN Execution Node 획득
     */

    /* sExeHashJoin = (qlnxHashJoin*)sExecNode->mExecNode; */


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlnx */
