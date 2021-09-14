/*******************************************************************************
 * qlnxHashInstantAccess.c
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
 * @file qlnxHashInstantAccess.c
 * @brief SQL Processor Layer Execution Node - HASH INSTANT TABLE
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnx
 * @{
 */

#if 0
/**
 * @brief HASH INSTANT TABLE node를 초기화한다.
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
stlStatus qlnxInitializeHashInstantAccess( smlTransId              aTransID,
                                           smlStatement          * aStmt,
                                           qllDBCStmt            * aDBCStmt,
                                           qllStatement          * aSQLStmt,
                                           qllOptimizationNode   * aOptNode,
                                           qllDataArea           * aDataArea,
                                           qllEnv                * aEnv )
{
    qllExecutionNode    * sExecNode = NULL;
    qlnoInstant         * sOptHashInstant = NULL;
    qlnxInstant         * sExeHashInstant = NULL;


    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_HASH_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * HASH INSTANT TABLE Optimization Node 획득
     */

    sOptHashInstant = (qlnoInstant *) aOptNode->mOptNode;


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * HASH INSTANT TABLE Execution Node 할당
     */

    sExeHashInstant = (qlnxInstant *) sExecNode->mExecNode;

    STL_DASSERT( sExeHashInstant->mKeyColBlock != NULL );
    STL_DASSERT( sExeHashInstant->mInsertColBlock != NULL );


    /**
     * HASH INSTANT TABLE 생성
     */

    if( sExeHashInstant->mNeedMaterialize == STL_TRUE )
    {
        stlMemset( & sExeHashInstant->mInstantTable, 0x00, STL_SIZEOF( qlnInstantTable ) );
        
        STL_TRY( qlnxCreateHashInstantTable( aTransID,
                                             aStmt,
                                             & sExeHashInstant->mInstantTable,
                                             sExeHashInstant->mInstantTable.mTableHandle,
                                             sOptHashInstant->mInstantNode->mKeyColCnt,
                                             sExeHashInstant->mInsertColBlock,
                                             & sOptHashInstant->mHashTableAttr,
                                             aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_DASSERT( sOptHashInstant->mInstantNode->mRecColCnt == 0 );

        /**
         * @todo conflict 체크하기 위한 hash 생성
         */
        
        STL_DASSERT( 0 );
    }

    sExeHashInstant->mHasFalseFilter = STL_FALSE;
    sExeHashInstant->mIsFirstExecute = STL_TRUE;

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief HASH INSTANT TABLE node를 수행한다.
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
stlStatus qlnxExecuteHashInstantAccess( smlTransId              aTransID,
                                        smlStatement          * aStmt,
                                        qllDBCStmt            * aDBCStmt,
                                        qllStatement          * aSQLStmt,
                                        qllOptimizationNode   * aOptNode,
                                        qllDataArea           * aDataArea,
                                        qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode       = NULL;
    qlnoInstant           * sOptHashInstant = NULL;
    qlnxInstant           * sExeHashInstant = NULL;
    qlnInstantTable       * sInstantTable   = NULL;

    knlPhysicalFilter     * sPhysicalFilter = NULL;
    smlFetchInfo          * sFetchInfo      = NULL;
    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_HASH_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * HASH INSTANT TABLE Optimization Node 획득
     */

    sOptHashInstant = (qlnoInstant *) aOptNode->mOptNode;


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * 해당 노드를 포함하는 Query Expression Optimization Node 획득
     */

    /* sOptQueryNode = aOptNode->mOptCurQueryNode; */


    /**
     * HASH INSTANT ACCESS Execution Node 획득
     */

    sExeHashInstant = (qlnxInstant *) sExecNode->mExecNode;

    /*****************************************
     * Hash Instant Table Iterator 초기화
     ****************************************/

    /**
     * Cursor를 First Record 위치로 이동
     */

    sInstantTable = & sExeHashInstant->mInstantTable;
    
    sInstantTable->mStmt   = aStmt;
    sInstantTable->mCurIdx = 0;

    if( sInstantTable->mIsNeedBuild == STL_FALSE )
    {
        if( sExeHashInstant->mNeedRebuild == STL_TRUE )
        {
            STL_TRY( qlnxTruncateInstantTable( aStmt,
                                               & sExeHashInstant->mInstantTable,
                                               aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            if( sInstantTable->mIterator != NULL )
            {
                STL_TRY( smlResetIterator( sInstantTable->mIterator,
                                           SML_ENV( aEnv ) )
                         == STL_SUCCESS );
            }
            else
            {
                sInstantTable->mIsNeedBuild = STL_TRUE;
            }
        }
    }
    else
    {
        /* Do Nothing */
    }


    /**
     * Prepare Expression Stack 수행
     */
    
    if( sExeHashInstant->mIsFirstExecute == STL_FALSE )
    {
        /**
         * Prepare Range 수행
         */

        if( sExeHashInstant->mPreRangeEvalInfo != NULL )
        {
            STL_TRY( knlEvaluateOneExpression( sExeHashInstant->mPreRangeEvalInfo,
                                               KNL_ENV( aEnv ) )
                     == STL_SUCCESS );
        }

        
        /**
         * Prepare Key Filter 수행
         */

        if( sExeHashInstant->mPreKeyFilterEvalInfo != NULL )
        {
            STL_TRY( knlEvaluateOneExpression( sExeHashInstant->mPreKeyFilterEvalInfo,
                                               KNL_ENV( aEnv ) )
                     == STL_SUCCESS );
        }

        
        /**
         * Prepare Table Filter 수행
         */

        if( sExeHashInstant->mPreFilterEvalInfo != NULL )
        {
            STL_TRY( knlEvaluateOneExpression( sExeHashInstant->mPreFilterEvalInfo,
                                               KNL_ENV( aEnv ) )
                     == STL_SUCCESS );
        }
    }
    else
    {
        /* Do Nothing */
    }


    /**
     * Instant Table Iterator 초기화
     */

    sFetchInfo = & sExeHashInstant->mFetchInfo;

    sFetchInfo->mIsEndOfScan = STL_FALSE;
    sFetchInfo->mSkipCnt     = 0;
    sFetchInfo->mFetchCnt    = QLL_FETCH_COUNT_MAX;


    /**
     * Hash Physical Filter의 constant value 정보 갱신
     */

    sPhysicalFilter = sFetchInfo->mPhysicalFilter;
    while( sPhysicalFilter != NULL )
    {
        if( sPhysicalFilter->mConstVal != NULL )
        {
            if( sPhysicalFilter->mColIdx2 == KNL_PHYSICAL_LIST_VALUE_CONSTANT )
            {
                /* Do Nothing */
            }
            else
            {
                sPhysicalFilter->mColVal2->mValue  = sPhysicalFilter->mConstVal->mValue;
                sPhysicalFilter->mColVal2->mLength = sPhysicalFilter->mConstVal->mLength;
            }
        }
        sPhysicalFilter = sPhysicalFilter->mNext;
    }


    /**
     * Fetch Record Info는 존재하면 안된다.
     */

    STL_DASSERT( sFetchInfo->mFetchRecordInfo == NULL );


    /**
     * @todo 불필요한 요소를 제거하여 Filter 재구성 (확장 필요)
     */
    
    sExeHashInstant->mHasFalseFilter =
        qlfHasFalseFilter( sOptHashInstant->mTableLogicalFilterExpr,
                           aDataArea );

    sExeHashInstant->mIsFirstExecute = STL_FALSE;


    /*****************************************
     * Child Node Execution
     ****************************************/

    /**
     * 하위 노드 Execute
     */

    QLNX_EXECUTE_NODE( aTransID,
                       aStmt,
                       aDBCStmt,
                       aSQLStmt,
                       aDataArea->mExecNodeList[sOptHashInstant->mChildNode->mIdx].mOptNode,
                       aDataArea,
                       aEnv );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief HASH INSTANT TABLE node를 Fetch한다.
 * @param[in]      aTransID      Transaction ID
 * @param[in]      aStmt         Statement
 * @param[in]      aDBCStmt      DBC Statement
 * @param[in,out]  aSQLStmt      SQL Statement
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aStartIdx     Start Index
 * @param[in]      aSkipCnt      Skip Record Count
 * @param[in]      aFetchCnt     Fetch Record Count
 * @param[out]     aUsedBlockCnt Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF          End Of Fetch 여부
 * @param[in]      aEnv          Environment
 *
 * @remark Fetch 수행시 호출된다.
 */
stlStatus qlnxFetchHashInstantAccess( smlTransId              aTransID,
                                      smlStatement          * aStmt,
                                      qllDBCStmt            * aDBCStmt,
                                      qllStatement          * aSQLStmt,
                                      qllOptimizationNode   * aOptNode,
                                      qllDataArea           * aDataArea,
                                      stlInt32                aStartIdx,
                                      stlInt64                aSkipCnt,
                                      stlInt64                aFetchCnt,
                                      stlInt64              * aUsedBlockCnt,
                                      stlBool               * aEOF,
                                      qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode       = NULL;
    qlnoInstant           * sOptHashInstant = NULL;
    qlnxInstant           * sExeHashInstant = NULL;
    qlnInstantTable       * sInstantTable   = NULL;

    stlInt64                sReadCnt        = 0;
    stlBool                 sEOF            = STL_FALSE;
    
    knlValueBlockList     * sValueBlock     = NULL;
    knlExprStack         ** sExprStack      = NULL;
    smlFetchInfo          * sFetchInfo      = NULL;
    stlInt32                sInsertColCnt   = 0;
    stlInt32                sLoop           = 0;
    knlExprEvalInfo         sEvalInfo;

    QLL_OPT_DECLARE( stlTime sBeginBuildTime );
    QLL_OPT_DECLARE( stlTime sEndBuildTime );
    QLL_OPT_DECLARE( stlTime sBuildTime );

    QLL_OPT_DECLARE( stlTime sBeginFetchTime );
    QLL_OPT_DECLARE( stlTime sEndFetchTime );
    QLL_OPT_DECLARE( stlTime sFetchTime );

    QLL_OPT_DECLARE( stlTime sBeginExceptTime );
    QLL_OPT_DECLARE( stlTime sEndExceptTime );
    QLL_OPT_DECLARE( stlTime sExceptTime );

    QLL_OPT_SET_VALUE( sBuildTime, 0 );
    QLL_OPT_SET_VALUE( sFetchTime, 0 );
    QLL_OPT_SET_VALUE( sExceptTime, 0 );

    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_HASH_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    QLL_OPT_CHECK_TIME( sBeginFetchTime );

    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * HASH INSTANT TABLE Optimization Node 획득
     */

    sOptHashInstant = (qlnoInstant *) aOptNode->mOptNode;

    
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * HASH INSTANT TABLE Execution Node 획득
     */

    sExeHashInstant = sExecNode->mExecNode;


    /**
     * Used Block Count 초기화
     */
    
    *aUsedBlockCnt = 0;


    /**
     * Node 수행 여부 확인 
     */

    STL_DASSERT( *aEOF == STL_FALSE );
    /* blocked join 에서는 assert 를 판단할 대상이 아님 */
    /* STL_DASSERT( sExeHashInstant->mFetchInfo.mIsEndOfScan == STL_FALSE ); */
    STL_DASSERT( aFetchCnt > 0);

        
    /**
     * @todo 불필요한 요소를 제거하여 Range 재구성 (확장 필요)
     */

    if( sExeHashInstant->mHasFalseFilter == STL_TRUE )
    {
        sExeHashInstant->mFetchInfo.mIsEndOfScan = STL_TRUE;
        *aEOF = STL_TRUE;
        STL_THROW( RAMP_EXIT );
    }
    else
    {
        /* Do Nothing */
    }


    /*****************************************
     * Fetch 수행하면서 Instant Table에 레코드 삽입
     ****************************************/

    sInstantTable = & sExeHashInstant->mInstantTable;
    sFetchInfo    = & sExeHashInstant->mFetchInfo;

    if( sInstantTable->mIsNeedBuild == STL_TRUE )
    {
        QLL_OPT_CHECK_TIME( sBeginBuildTime );

        sEvalInfo.mContext  = aDataArea->mExprDataContext;
        sEvalInfo.mBlockIdx = 0;

        sExprStack  = sOptHashInstant->mColExprStack;
        sInsertColCnt = ( sOptHashInstant->mInstantNode->mKeyColCnt +
                          sOptHashInstant->mInstantNode->mRecColCnt );

        while( sEOF == STL_FALSE )
        {
            /**
             * Block Read
             */

            QLL_OPT_CHECK_TIME( sBeginExceptTime );

            /* fetch records*/
            sReadCnt = 0;
            QLNX_FETCH_NODE( aTransID,
                             aStmt,
                             aDBCStmt,
                             aSQLStmt,
                             sOptHashInstant->mChildNode->mIdx,
                             aDataArea,
                             0, /* Start Idx */
                             0, /* Skip Count */
                             QLL_FETCH_COUNT_MAX, /* Fetch Count */
                             & sReadCnt,
                             & sEOF,
                             aEnv );

            QLL_OPT_CHECK_TIME( sEndExceptTime );
            QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

            if( sReadCnt > 0 )
            {
                /**
                 * Evaluate Column Expression Stack
                 */
            
                sInstantTable->mRecCnt += sReadCnt;
                
                sEvalInfo.mBlockCount = sReadCnt;

                if( sOptHashInstant->mNeedEvalInsertCol == STL_TRUE )
                {
                    sValueBlock = sExeHashInstant->mInsertColBlock;
                    for( sLoop = 0 ; sLoop < sInsertColCnt ; sLoop++ )
                    {
                        if( sExprStack[ sLoop ] != NULL )
                        {
                            sEvalInfo.mExprStack   = sExprStack[ sLoop ];
                            sEvalInfo.mResultBlock = sValueBlock;

                            STL_TRY( knlEvaluateBlockExpression( & sEvalInfo,
                                                                 KNL_ENV( aEnv ) )
                                     == STL_SUCCESS );
                        }
                        sValueBlock = sValueBlock->mNext;
                    }
                }
                else
                {
                    /* Do Nothing */
                }
                

                /**
                 * Instant Table에 Record 삽입
                 */

                KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sExeHashInstant->mInsertColBlock, 0, sReadCnt );

                STL_TRY( smlInsertRecord( aStmt,
                                          sInstantTable->mTableHandle,
                                          sExeHashInstant->mInsertColBlock,
                                          NULL, /* unique violation */
                                          sFetchInfo->mRowBlock,
                                          SML_ENV( aEnv ) )
                         == STL_SUCCESS );
            }
            else
            {
                STL_DASSERT( sEOF == STL_TRUE );
                break;
            }
        }

        sInstantTable->mIsNeedBuild = STL_FALSE;


        /**
         * Iterator Allocation
         */
        
        if( sInstantTable->mIterator != NULL )
        {
            STL_TRY( smlFreeIterator( sInstantTable->mIterator,
                                      SML_ENV( aEnv ) )
                     == STL_SUCCESS, ERR_FREE_ITERATOR );
            
            sInstantTable->mIterator = NULL;
        }

        if( sInstantTable->mRecCnt > 0 )
        {
            SML_INIT_ITERATOR_PROP( sInstantTable->mIteratorProperty );
            
            STL_TRY( smlAllocIterator( aStmt,
                                       sInstantTable->mTableHandle,
                                       SML_TRM_SNAPSHOT,
                                       SML_SRM_SNAPSHOT,
                                       & sInstantTable->mIteratorProperty,
                                       SML_SCAN_FORWARD,
                                       & sInstantTable->mIterator,
                                       SML_ENV( aEnv ) )
                     == STL_SUCCESS );
        }
        else
        {
            sInstantTable->mIterator = NULL;
            KNL_SET_ALL_BLOCK_USED_CNT( sExeHashInstant->mReadColBlock, 0 );
            *aEOF = STL_TRUE;
            *aUsedBlockCnt = 0;
            STL_THROW( RAMP_EXIT );
        }

        QLL_OPT_CHECK_TIME( sEndBuildTime );
        QLL_OPT_ADD_ELAPSED_TIME( sBuildTime, sBeginBuildTime, sEndBuildTime );
        QLL_OPT_SET_VALUE( sExeHashInstant->mCommonInfo.mBuildTime, sBuildTime - sExceptTime );

        QLL_OPT_SET_VALUE( sExceptTime, 0 );
    }
    else
    {
        STL_PARAM_VALIDATE( sInstantTable->mIterator != NULL,
                            QLL_ERROR_STACK(aEnv) );
    }


    /*****************************************
     * Fetch
     ****************************************/

    /**
     * fetch 수행
     */

    if( (sInstantTable->mRecCnt <= sInstantTable->mCurIdx + aSkipCnt) &&
        (sFetchInfo->mBlockJoinFetchInfo == NULL) )
    {
        /**
         * Join 에서는 검사 대상이 아님
         */
        
        KNL_SET_ALL_BLOCK_USED_CNT( sExeHashInstant->mReadColBlock, 0 );
        *aEOF = STL_TRUE;
        *aUsedBlockCnt = 0;
    }
    else
    {
        /**
         * Skip Count 와 Fetch Count 조정
         */
        
        sFetchInfo->mSkipCnt  = aSkipCnt;
        sFetchInfo->mFetchCnt = aFetchCnt;
        
        /* fetch records */
        STL_TRY( smlFetchNext( sInstantTable->mIterator,
                               sFetchInfo,
                               SML_ENV(aEnv) )
                 == STL_SUCCESS );
        
        *aEOF = sFetchInfo->mIsEndOfScan;

        *aUsedBlockCnt = sFetchInfo->mRowBlock->mUsedBlockSize;

        sInstantTable->mCurIdx += *aUsedBlockCnt;
    }
    
    
    STL_RAMP( RAMP_EXIT );

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeHashInstant->mCommonInfo.mFetchTime, sFetchTime - sBuildTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeHashInstant->mCommonInfo.mFetchCallCount, 1 );
    QLL_OPT_ADD_RECORD_STAT_INFO( sExeHashInstant->mCommonInfo.mFetchRecordStat,
                                  sExeHashInstant->mCommonInfo.mResultColBlock,
                                  *aUsedBlockCnt,
                                  aEnv );

    sExeHashInstant->mCommonInfo.mFetchRowCnt += *aUsedBlockCnt;

    return STL_SUCCESS;

    STL_CATCH( ERR_FREE_ITERATOR )
    {
        sInstantTable->mIterator = NULL;
    }
    
    STL_FINISH;

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeHashInstant->mCommonInfo.mFetchTime, sFetchTime - sBuildTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeHashInstant->mCommonInfo.mFetchCallCount, 1 );

    return STL_FAILURE;
}


/**
 * @brief HASH INSTANT TABLE node 수행을 종료한다.
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 *
 * @remark Execution을 종료하고자 할 때 호출한다.
 */
stlStatus qlnxFinalizeHashInstantAccess( qllOptimizationNode   * aOptNode,
                                         qllDataArea           * aDataArea,
                                         qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode       = NULL;
//    qlnoInstant           * sOptHashInstant = NULL;
    qlnxInstant           * sExeHashInstant = NULL;

    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_HASH_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    
    /**
     * HASH INSTANT TABLE Optimization Node 획득
     */

//    sOptHashInstant = (qlnoInstant *) aOptNode->mOptNode;


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * 해당 노드를 포함하는 Query Expression Optimization Node 획득
     */

    /* sOptQueryNode = aOptNode->mOptCurQueryNode; */


    /**
     * HASH INSTANT ACCESS Execution Node 획득
     */

    sExeHashInstant = (qlnxInstant *) sExecNode->mExecNode;

    
    /**
     * Instant Table 해제
     */

    STL_TRY( qlnxDropInstantTable( & sExeHashInstant->mInstantTable,
                                   aEnv )
             == STL_SUCCESS );

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
#endif


/** @} qlnx */
