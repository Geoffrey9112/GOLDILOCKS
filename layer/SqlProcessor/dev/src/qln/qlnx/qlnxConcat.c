/*******************************************************************************
 * qlnxConcat.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlnxConcat.c 7583 2013-03-13 02:11:36Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlnxConcat.c
 * @brief SQL Processor Layer Execution Node - Concat
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnx
 * @{
 */

/**
 * @brief CONCAT node를 초기화한다.
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
stlStatus qlnxInitializeConcat( smlTransId              aTransID,
                                smlStatement          * aStmt,
                                qllDBCStmt            * aDBCStmt,
                                qllStatement          * aSQLStmt,
                                qllOptimizationNode   * aOptNode,
                                qllDataArea           * aDataArea,
                                qllEnv                * aEnv )
{
    qllExecutionNode  * sExecNode       = NULL;
    qlnoConcat        * sOptConcat      = NULL;
    qlnxConcat        * sExeConcat      = NULL;
    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_CONCAT_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * FLAT INSTANT TABLE Optimization Node 획득
     */

    sOptConcat = (qlnoConcat *) aOptNode->mOptNode;


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * 해당 노드를 포함하는 Query Expression Optimization Node 획득
     */

    /* sOptQueryNode = aOptNode->mOptCurQueryNode; */


    /**
     * CONCAT Execution Node 획득
     */

    sExeConcat = (qlnxConcat *) sExecNode->mExecNode;


    /**
     * HASH INSTANT TABLE 생성
     */

    /* 첫번째 Child는 Unique 체크를 할 필요가 없다 */
    sExeConcat->mHashTableAttr.mValidFlags = SML_INDEX_UNIQUENESS_YES;
    sExeConcat->mHashTableAttr.mUniquenessFlag = STL_FALSE;

    stlMemset( & sExeConcat->mInstantTable, 0x00, STL_SIZEOF( qlnInstantTable ) );
    
    STL_TRY( qlnxCreateHashInstantTable( aTransID,
                                         aStmt,
                                         & sExeConcat->mInstantTable,
                                         sOptConcat->mTableHandle,
                                         sExeConcat->mKeyColCount,
                                         sExeConcat->mKeyValueBlock,
                                         &sExeConcat->mHashTableAttr,
                                         aEnv )
             == STL_SUCCESS );

    sExeConcat->mIsFirstFetch = STL_TRUE;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;

}


/**
 * @brief CONCAT TABLE node를 수행한다.
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
stlStatus qlnxExecuteConcat( smlTransId              aTransID,
                             smlStatement          * aStmt,
                             qllDBCStmt            * aDBCStmt,
                             qllStatement          * aSQLStmt,
                             qllOptimizationNode   * aOptNode,
                             qllDataArea           * aDataArea,
                             qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode       = NULL;
    qlnoConcat            * sOptConcat      = NULL;
    qlnxConcat            * sExeConcat      = NULL;
    qlnxCommonInfo        * sCurChildNode   = NULL;
    stlInt32                sChildIdx;
    stlInt32                i;


    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_CONCAT_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * CONCAT Optimization Node 획득
     */

    sOptConcat = (qlnoConcat *) aOptNode->mOptNode;


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * 해당 노드를 포함하는 Query Expression Optimization Node 획득
     */

    /* sOptQueryNode = aOptNode->mOptCurQueryNode; */


    /**
     * CONCAT Execution Node 획득
     */

    sExeConcat = (qlnxConcat *) sExecNode->mExecNode;


    /*****************************************
     * Concat Iterator 초기화
     ****************************************/

    /**
     * Cursor를 First Record 위치로 이동
     */

    sExeConcat->mCurChildSeq       = 0;
    sChildIdx     = (sOptConcat->mChildNodeArr[0])->mIdx;
    sCurChildNode = (qlnxCommonInfo*)(aDataArea->mExecNodeList[sChildIdx].mExecNode);
    sExeConcat->mCurChildColBlock = sCurChildNode->mResultColBlock;
    sExeConcat->mCurChildRowBlock = sCurChildNode->mRowBlockList->mHead->mRowBlock;
    sExeConcat->mCurInputBlockIdx = 0;
    sExeConcat->mCurInputBlockCnt = 0;
    sExeConcat->mIsEOF = STL_FALSE;
    sExeConcat->mIsCurChildEOF = STL_FALSE;

    
    /*****************************************
     * Child Node Execution
     ****************************************/

    /**
     * 하위 노드 Execute
     */

    for( i = 0; i < sExeConcat->mChildCount; i++ )
    {
        QLNX_EXECUTE_NODE( aTransID,
                           aStmt,
                           aDBCStmt,
                           aSQLStmt,
                           aDataArea->mExecNodeList[(sOptConcat->mChildNodeArr[i])->mIdx].mOptNode,
                           aDataArea,
                           aEnv );
    }


    if( sExeConcat->mIsFirstFetch != STL_TRUE )
    {
        STL_TRY( qlnxTruncateInstantTable( aStmt,
                                           & sExeConcat->mInstantTable,
                                           aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sExeConcat->mIsFirstFetch = STL_FALSE;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief CONCAT node를 Fetch한다.
 * @param[in]      aFetchNodeInfo  Fetch Node Info
 * @param[out]     aUsedBlockCnt   Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF            End Of Fetch 여부
 * @param[in]      aEnv            Environment
 *
 * @remark Fetch 수행시 호출된다.
 */
stlStatus qlnxFetchConcat( qlnxFetchNodeInfo     * aFetchNodeInfo,
                           stlInt64              * aUsedBlockCnt,
                           stlBool               * aEOF,
                           qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode       = NULL;
    qlnoConcat            * sOptConcat      = NULL;
    qlnxConcat            * sExeConcat      = NULL;
    qlnxCommonInfo        * sCurChildNode   = NULL;

    stlInt64                sFetchCnt       = 0;
    stlInt64                sSkipCnt        = 0;

    stlInt32                sDstIdx;
    stlInt32                sChildIdx;
    dtlDataValue          * sResultDataValue;

    stlInt64                sFetchedCnt       = 0;
    stlInt64                sSkippedCnt       = 0;
    qlnxFetchNodeInfo       sLocalFetchInfo;

    QLL_OPT_DECLARE( stlTime sBeginFetchTime );
    QLL_OPT_DECLARE( stlTime sEndFetchTime );
    QLL_OPT_DECLARE( stlTime sFetchTime );

    QLL_OPT_DECLARE( stlTime sBeginExceptTime );
    QLL_OPT_DECLARE( stlTime sEndExceptTime );
    QLL_OPT_DECLARE( stlTime sExceptTime );

    QLL_OPT_SET_VALUE( sFetchTime, 0 );
    QLL_OPT_SET_VALUE( sExceptTime, 0 );

#define COPY_RESULT_COLS( aSrcCol, aSrcIdx, aDstCol, aDstIdx )                          \
    {                                                                                   \
        knlValueBlockList * tmpSrcCol = (aSrcCol);                                      \
        knlValueBlockList * tmpDstCol = (aDstCol);                                      \
        while( tmpDstCol != NULL )                                                      \
        {                                                                               \
            while( tmpSrcCol != NULL )                                                  \
            {                                                                           \
                if( DTL_GET_BLOCK_COLUMN_ORDER( tmpSrcCol ) ==                          \
                    DTL_GET_BLOCK_COLUMN_ORDER( tmpDstCol ) )                           \
                {                                                                       \
                    STL_TRY( knlCopyDataValue(                                          \
                                 & tmpSrcCol->mValueBlock->mDataValue[(aSrcIdx)],       \
                                 & tmpDstCol->mValueBlock->mDataValue[(aDstIdx)],       \
                                 KNL_ENV(aEnv) )                                        \
                        == STL_SUCCESS );                                               \
                    break;                                                              \
                }                                                                       \
                tmpSrcCol = tmpSrcCol->mNext;                                           \
            }                                                                           \
            STL_DASSERT( tmpSrcCol != NULL );                                           \
                                                                                        \
            tmpDstCol = tmpDstCol->mNext;                                               \
        }                                                                               \
    }

#define COPY_ROW_BLOCKS( aRowBlockList, aSrcIdx, aDstIdx )                      \
    {                                                                           \
        qlnxRowBlockItem    * tmpRowBlockItem = (aRowBlockList)->mHead;         \
        while( tmpRowBlockItem != NULL )                                        \
        {                                                                       \
            tmpRowBlockItem->mRowBlock->mRidBlock[(aDstIdx)] =                  \
                tmpRowBlockItem->mOrgRowBlock->mRidBlock[(aSrcIdx)];            \
            tmpRowBlockItem->mRowBlock->mScnBlock[(aDstIdx)] =                  \
                tmpRowBlockItem->mOrgRowBlock->mScnBlock[(aSrcIdx)];            \
                                                                                \
            tmpRowBlockItem = tmpRowBlockItem->mNext;                           \
        }                                                                       \
    }

#define SWAP_DATA_VALUE( aValueBlockList1, aValueBlockList2 )                                \
    {                                                                                        \
        knlValueBlockList * curBlockList1 = (aValueBlockList1);                              \
        knlValueBlockList * curBlockList2 = (aValueBlockList2);                              \
        dtlDataValue      * tmpDataValue;                                                    \
        while( curBlockList1 != NULL )                                                       \
        {                                                                                    \
            tmpDataValue = curBlockList1->mValueBlock->mDataValue;                           \
            curBlockList1->mValueBlock->mDataValue = curBlockList2->mValueBlock->mDataValue; \
            curBlockList2->mValueBlock->mDataValue = tmpDataValue;                           \
            curBlockList1 = curBlockList1->mNext;                                            \
            curBlockList2 = curBlockList2->mNext;                                            \
        }                                                                                    \
    }



    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aFetchNodeInfo->mOptNode->mType == QLL_PLAN_NODE_CONCAT_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    QLL_OPT_CHECK_TIME( sBeginFetchTime );

    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * CONCAT Optimization Node 획득
     */

    sOptConcat = (qlnoConcat *) aFetchNodeInfo->mOptNode->mOptNode;

    
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aFetchNodeInfo->mDataArea,
                                        QLL_GET_OPT_NODE_IDX( aFetchNodeInfo->mOptNode ) );


    /**
     *  Execution Node 획득
     */

    sExeConcat = sExecNode->mExecNode;


    /**
     * Used Block Count 초기화
     */
    
    *aUsedBlockCnt = 0;


    /**
     * Node 수행 여부 확인 
     */

    STL_DASSERT( *aEOF == STL_FALSE );
    STL_DASSERT( aFetchNodeInfo->mFetchCnt > 0);

    sFetchCnt = aFetchNodeInfo->mFetchCnt;
    sSkipCnt  = aFetchNodeInfo->mSkipCnt;

    STL_DASSERT( sExeConcat->mIsEOF == STL_FALSE );
    

    /**
     * Local Fetch Info 초기화
     */
    
    QLNX_SET_LOCAL_FETCH_INFO( & sLocalFetchInfo, aFetchNodeInfo );


    /*****************************************
     * Fetch 수행하면서 결과 셋에 복사
     ****************************************/

    STL_DASSERT( sExeConcat->mCurChildSeq < sExeConcat->mChildCount );
    sChildIdx = (sOptConcat->mChildNodeArr[sExeConcat->mCurChildSeq])->mIdx;
    sDstIdx = 0;

    while( sExeConcat->mCurChildSeq < sExeConcat->mChildCount )
    {
        if( (sExeConcat->mCurInputBlockIdx >= sExeConcat->mCurInputBlockCnt) &&
            (sExeConcat->mIsCurChildEOF == STL_FALSE) )
        {
            /* fetch records from Child */

            QLL_OPT_CHECK_TIME( sBeginExceptTime );

            sExeConcat->mCurInputBlockIdx = 0;
            QLNX_FETCH_NODE( & sLocalFetchInfo,
                             sChildIdx,
                             0, /* Start Idx */
                             0,  /* Skip Count */
                             QLL_FETCH_COUNT_MAX, /* Fetch Count */
                             & sExeConcat->mCurInputBlockCnt,
                             & sExeConcat->mIsCurChildEOF,
                             aEnv );

            QLL_OPT_CHECK_TIME( sEndExceptTime );
            QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

            if( sExeConcat->mCurInputBlockCnt > 0 )
            {
                /**
                 * Instant Table에 Record 삽입
                 */

                KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sExeConcat->mKeyValueBlock,
                                                     0,
                                                     sExeConcat->mCurInputBlockCnt );
                sExeConcat->mHashRowBlock.mUsedBlockSize = sExeConcat->mCurInputBlockCnt;

                STL_TRY( smlInsertRecord( sLocalFetchInfo.mStmt,
                                          sExeConcat->mInstantTable.mTableHandle,
                                          sExeConcat->mKeyValueBlock,
                                          sExeConcat->mDupFlagBlockList,
                                          &sExeConcat->mHashRowBlock,
                                          SML_ENV( aEnv ) )
                         == STL_SUCCESS );
            }
        }

        /*  첫 Child는 unqiue가 있을 수가 없음 */
        if( sExeConcat->mCurChildSeq == 0 )
        {
            while( sExeConcat->mCurInputBlockIdx < sExeConcat->mCurInputBlockCnt )
            {
                if( sSkipCnt > 0 )
                {
                    sExeConcat->mCurInputBlockIdx++;
                    sSkipCnt--;
                }
                else
                {
                    /* copy result */
                    COPY_RESULT_COLS( sExeConcat->mCurChildColBlock,
                                      sExeConcat->mCurInputBlockIdx,
                                      sExeConcat->mCommonInfo.mResultColBlock,
                                      sDstIdx );

                    COPY_ROW_BLOCKS( &(sExeConcat->mRowBlockList),
                                     sExeConcat->mCurInputBlockIdx,
                                     sDstIdx );

                    sExeConcat->mCurInputBlockIdx++;
                    sDstIdx++;

                    if( sDstIdx == sFetchCnt )
                    {
                        sExeConcat->mCurChildSeq = sExeConcat->mChildCount;
                        sExeConcat->mCurInputBlockIdx = 0;
                        sExeConcat->mCurInputBlockCnt = 0;
                        sExeConcat->mIsEOF = STL_FALSE;

                        STL_THROW( RAMP_FINISH );
                    }
                    if( sDstIdx == sLocalFetchInfo.mDataArea->mBlockAllocCnt )
                    {
                        STL_THROW( RAMP_FINISH );
                    }
                }
            }
        }
        else
        {
            /* UNIQUE 처리 */
            sResultDataValue = sExeConcat->mDupFlagBlockList->mValueBlock->mDataValue;

            while( sExeConcat->mCurInputBlockIdx < sExeConcat->mCurInputBlockCnt )
            {
                if( *(stlBool*)(sResultDataValue[sExeConcat->mCurInputBlockIdx].mValue) == STL_FALSE )
                {
                    if( sSkipCnt > 0 )
                    {
                        sExeConcat->mCurInputBlockIdx++;
                        sSkipCnt--;

                        continue;
                    }

                    /* copy result */
                    COPY_RESULT_COLS( sExeConcat->mCurChildColBlock,
                                      sExeConcat->mCurInputBlockIdx,
                                      sExeConcat->mCommonInfo.mResultColBlock,
                                      sDstIdx );

                    COPY_ROW_BLOCKS( &(sExeConcat->mRowBlockList),
                                     sExeConcat->mCurInputBlockIdx,
                                     sDstIdx );

                    sExeConcat->mCurInputBlockIdx++;
                    sDstIdx++;

                    if( sDstIdx == sFetchCnt )
                    {
                        sExeConcat->mCurChildSeq = sExeConcat->mChildCount;
                        sExeConcat->mCurInputBlockIdx = 0;
                        sExeConcat->mCurInputBlockCnt = 0;
                        sExeConcat->mIsEOF = STL_FALSE;

                        STL_THROW( RAMP_FINISH );
                    }
                    if( sDstIdx == sLocalFetchInfo.mDataArea->mBlockAllocCnt )
                    {
                        STL_THROW( RAMP_FINISH );
                    }
                }
                else
                {
                    sExeConcat->mCurInputBlockIdx++;
                }
            }
        }

        if( sExeConcat->mIsCurChildEOF == STL_TRUE )
        {
            if( sExeConcat->mCurChildSeq + 1 < sExeConcat->mChildCount )
            {
                /**
                 * 다음 child로 이동
                 */

                sExeConcat->mCurChildSeq++;
                STL_DASSERT( sExeConcat->mCurChildSeq < sExeConcat->mChildCount );

                sExeConcat->mIsCurChildEOF = STL_FALSE;

                sChildIdx = (sOptConcat->mChildNodeArr[sExeConcat->mCurChildSeq])->mIdx;
                sCurChildNode = (qlnxCommonInfo*)(sLocalFetchInfo.mDataArea->mExecNodeList[sChildIdx].mExecNode);
                sExeConcat->mCurChildColBlock = sCurChildNode->mResultColBlock;
                sExeConcat->mCurChildRowBlock = sCurChildNode->mRowBlockList->mHead->mRowBlock;

                if( sExeConcat->mCurChildSeq == 1 )
                {
                    /* 두번째 Child 부터는 Uniqueness 체크를 한다 */
                    sExeConcat->mHashTableAttr.mUniquenessFlag = STL_TRUE;
                    STL_TRY( smlAlterTableAttr( sLocalFetchInfo.mStmt,
                                                sExeConcat->mInstantTable.mTableHandle,
                                                NULL,
                                                &sExeConcat->mHashTableAttr,
                                                SML_ENV( aEnv ) )
                             == STL_SUCCESS );
                }
            }
            else
            {
                /* Fetch 종료 */
                sExeConcat->mCurChildSeq = sExeConcat->mChildCount;
                sExeConcat->mCurInputBlockIdx = 0;
                sExeConcat->mCurInputBlockCnt = 0;
                sExeConcat->mIsEOF = STL_TRUE;

                STL_THROW( RAMP_FINISH );
            }
        }
    }
    
    STL_RAMP( RAMP_FINISH );

    
    /**
     * OUTPUT 설정
     */

    sFetchedCnt = sDstIdx;
    sSkippedCnt = aFetchNodeInfo->mSkipCnt - sSkipCnt;
        
    sExeConcat->mRowBlockList.mHead->mRowBlock->mUsedBlockSize = sFetchedCnt;

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sExeConcat->mCommonInfo.mResultColBlock,
                                         0,
                                         sFetchedCnt );

    *aUsedBlockCnt = sFetchedCnt;
    
    /* Parent의 Fetch Node Info 설정 */
    aFetchNodeInfo->mFetchCnt -= sFetchedCnt;
    aFetchNodeInfo->mSkipCnt  -= sSkippedCnt;

    *aEOF = sExeConcat->mIsEOF;

    
    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeConcat->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeConcat->mCommonInfo.mFetchCallCount, 1 );
    QLL_OPT_ADD_RECORD_STAT_INFO( sExeConcat->mCommonInfo.mFetchRecordStat,
                                  sExeConcat->mCommonInfo.mResultColBlock,
                                  *aUsedBlockCnt,
                                  aEnv );

    sExeConcat->mCommonInfo.mFetchRowCnt += *aUsedBlockCnt;


    return STL_SUCCESS;

    STL_FINISH;

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeConcat->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeConcat->mCommonInfo.mFetchCallCount, 1 );

    return STL_FAILURE;
}


/**
 * @brief CONCAT node 수행을 종료한다.
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 *
 * @remark Execution을 종료하고자 할 때 호출한다.
 */
stlStatus qlnxFinalizeConcat( qllOptimizationNode   * aOptNode,
                              qllDataArea           * aDataArea,
                              qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode       = NULL;
/*    qlnoConcat            * sOptConcat = NULL; */
    qlnxConcat            * sExeConcat = NULL;

    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_CONCAT_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );

    /**
     * CONCAT Execution Node 획득
     */

    sExeConcat = sExecNode->mExecNode;

    /* Drop Hash Instant Table */
    STL_TRY( qlnxDropInstantTable( & sExeConcat->mInstantTable,
                                   aEnv )
             == STL_SUCCESS );

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlnx */
