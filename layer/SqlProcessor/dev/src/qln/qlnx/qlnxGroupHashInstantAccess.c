/*******************************************************************************
 * qlnxGroupHashInstantAccess.c
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
 * @file qlnxGroupHashInstantAccess.c
 * @brief SQL Processor Layer Execution Node - GROUP HASH INSTANT TABLE
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnx
 * @{
 */


/**
 * @brief GROUP INSTANT TABLE node를 초기화한다.
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
stlStatus qlnxInitializeGroupHashInstantAccess( smlTransId              aTransID,
                                                smlStatement          * aStmt,
                                                qllDBCStmt            * aDBCStmt,
                                                qllStatement          * aSQLStmt,
                                                qllOptimizationNode   * aOptNode,
                                                qllDataArea           * aDataArea,
                                                qllEnv                * aEnv )
{
    qllExecutionNode  * sExecNode           = NULL;
    qlnoInstant       * sOptGroupInstant    = NULL;
    qlnxInstant       * sExeGroupInstant    = NULL;

    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_GROUP_HASH_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**
     * GROUP INSTANT TABLE Optimization Node 획득
     */

    sOptGroupInstant = (qlnoInstant *) aOptNode->mOptNode;


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * 해당 노드를 포함하는 Query Expression Optimization Node 획득
     */

    /* sOptQueryNode = aOptNode->mOptCurQueryNode; */


    /**
     * GROUP INSTANT ACCESS Execution Node 획득
     */

    sExeGroupInstant = (qlnxInstant *) sExecNode->mExecNode;

    STL_DASSERT( sExeGroupInstant->mKeyColBlock != NULL );
    STL_DASSERT( sExeGroupInstant->mInsertColBlock != NULL );

    
    /**
     * GROUP INSTANT TABLE 생성
     */

    STL_DASSERT( sExeGroupInstant->mNeedMaterialize == STL_TRUE );

    stlMemset( & sExeGroupInstant->mInstantTable, 0x00, STL_SIZEOF( qlnInstantTable ) );
        
    STL_TRY( qlnxCreateGroupHashInstantTable( aTransID,
                                              aStmt,
                                              & sExeGroupInstant->mInstantTable,
                                              STL_FALSE,
                                              sOptGroupInstant->mInstantNode->mKeyColCnt,
                                              sExeGroupInstant->mInsertColBlock,
                                              aEnv )
             == STL_SUCCESS );
    

    /**
     * Aggregation Distinct를 위한 Instant Table 생성
     */

    if( sOptGroupInstant->mAggrDistFuncCnt > 0 )
    {
        STL_TRY( qlnxInitializeAggrConflictCheck( aTransID,
                                                  aStmt,
                                                  sOptGroupInstant->mAggrDistFuncCnt,
                                                  sExeGroupInstant->mConflictCheckInfo,
                                                  aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /* Do Nothing */
    }


    /**
     * Nested Aggregation Distinct를 위한 Instant Table 생성
     */
    
    if( sOptGroupInstant->mNestedAggrDistFuncCnt > 0 )
    {
        STL_TRY( qlnxInitializeAggrDist( aTransID,
                                         aStmt,
                                         sOptGroupInstant->mNestedAggrDistFuncCnt,
                                         sExeGroupInstant->mNestedAggrDistExecInfo,
                                         aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /* Do Nothing */
    }

    sExeGroupInstant->mHasFalseFilter = STL_FALSE;
    sExeGroupInstant->mIsFirstExecute = STL_TRUE;
    

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief GROUP INSTANT TABLE node를 수행한다.
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
stlStatus qlnxExecuteGroupHashInstantAccess( smlTransId              aTransID,
                                             smlStatement          * aStmt,
                                             qllDBCStmt            * aDBCStmt,
                                             qllStatement          * aSQLStmt,
                                             qllOptimizationNode   * aOptNode,
                                             qllDataArea           * aDataArea,
                                             qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode        = NULL;
    qlnoInstant           * sOptGroupInstant = NULL;
    qlnxInstant           * sExeGroupInstant = NULL;
    qlnInstantTable       * sInstantTable    = NULL;

    knlPhysicalFilter     * sPhysicalFilter  = NULL;
    smlFetchInfo          * sFetchInfo       = NULL;

    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_GROUP_HASH_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * GROUP INSTANT TABLE Optimization Node 획득
     */

    sOptGroupInstant = (qlnoInstant *) aOptNode->mOptNode;


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * 해당 노드를 포함하는 Query Expression Optimization Node 획득
     */

    /* sOptQueryNode = aOptNode->mOptCurQueryNode; */


    /**
     * GROUP INSTANT ACCESS Execution Node 획득
     */

    sExeGroupInstant = (qlnxInstant *) sExecNode->mExecNode;


    /*****************************************
     * Group Instant Table Iterator 초기화
     ****************************************/

    /**
     * Cursor를 First Record 위치로 이동
     */
    
    sInstantTable = & sExeGroupInstant->mInstantTable;
    
    sInstantTable->mStmt   = aStmt;
    sInstantTable->mCurIdx = 0;

    if( sInstantTable->mIsNeedBuild == STL_FALSE )
    {
        if( sExeGroupInstant->mNeedRebuild == STL_TRUE )
        {
            /* Aggregation Distinct를 위한 Instant Table 해제 */
            if( sOptGroupInstant->mAggrDistFuncCnt > 0 )
            {
                STL_TRY( qlnxFinalizeAggrConflictCheck( sOptGroupInstant->mAggrDistFuncCnt,
                                                        sExeGroupInstant->mConflictCheckInfo,
                                                        aEnv )
                         == STL_SUCCESS );
            }

            /* Nested Aggregation Distinct를 위한 Instant Table 해제 */
            if( sOptGroupInstant->mNestedAggrDistFuncCnt > 0 )
            {
                STL_TRY( qlnxFinalizeAggrDist( sOptGroupInstant->mNestedAggrDistFuncCnt,
                                               sExeGroupInstant->mNestedAggrDistExecInfo,
                                               aEnv )
                         == STL_SUCCESS );
            }

            STL_TRY( qlnxTruncateInstantTable( aStmt,
                                               & sExeGroupInstant->mInstantTable,
                                               aEnv )
                     == STL_SUCCESS );
                
            /* Aggregation Distinct를 위한 Instant Table 생성 */
            if( sOptGroupInstant->mAggrDistFuncCnt > 0 )
            {
                STL_TRY( qlnxInitializeAggrConflictCheck( aTransID,
                                                          aStmt,
                                                          sOptGroupInstant->mAggrDistFuncCnt,
                                                          sExeGroupInstant->mConflictCheckInfo,
                                                          aEnv )
                         == STL_SUCCESS );
            }

            /* Nested Aggregation Distinct를 위한 Instant Table 생성 */
            if( sOptGroupInstant->mNestedAggrDistFuncCnt > 0 )
            {
                STL_TRY( qlnxInitializeAggrDist( aTransID,
                                                 aStmt,
                                                 sOptGroupInstant->mNestedAggrDistFuncCnt,
                                                 sExeGroupInstant->mNestedAggrDistExecInfo,
                                                 aEnv )
                         == STL_SUCCESS );
            }

            sInstantTable->mIsNeedBuild = STL_TRUE;
        }
        else
        {
            if( sInstantTable->mIterator != NULL )
            {
                STL_TRY( smlResetIterator( sInstantTable->mIterator,
                                           SML_ENV( aEnv ) )
                         == STL_SUCCESS );
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
    
    if( sExeGroupInstant->mIsFirstExecute == STL_FALSE )
    {
        /**
         * Prepare Range 수행
         */

        if( sExeGroupInstant->mPreRangeEvalInfo != NULL )
        {
            /* Coverage : range scan이 지원되어야 적용되는 코드임 */
            STL_TRY( knlEvaluateOneExpression( sExeGroupInstant->mPreRangeEvalInfo,
                                               KNL_ENV( aEnv ) )
                     == STL_SUCCESS );
        }

        
        /**
         * Prepare Key Filter 수행
         */

        if( sExeGroupInstant->mPreKeyFilterEvalInfo != NULL )
        {
            /* Coverage : range scan이 지원되어야 적용되는 코드임 */
            STL_TRY( knlEvaluateOneExpression( sExeGroupInstant->mPreKeyFilterEvalInfo,
                                               KNL_ENV( aEnv ) )
                     == STL_SUCCESS );
        }

        
        /**
         * Prepare Table Filter 수행
         */

        if( sExeGroupInstant->mPreFilterEvalInfo != NULL )
        {
            STL_TRY( knlEvaluateOneExpression( sExeGroupInstant->mPreFilterEvalInfo,
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

    sFetchInfo = & sExeGroupInstant->mFetchInfo;
    
    sFetchInfo->mIsEndOfScan = STL_FALSE;
    sFetchInfo->mSkipCnt     = 0;
    sFetchInfo->mFetchCnt    = QLL_FETCH_COUNT_MAX;


    /**
     * Physical Filter의 constant value 정보 갱신
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
     * @todo 불필요한 요소를 제거하여 Filter 재구성 (확장 필요)
     */
    
    sExeGroupInstant->mHasFalseFilter =
        qlfHasFalseFilter( sOptGroupInstant->mTableLogicalFilterExpr,
                           aDataArea );

    sExeGroupInstant->mIsFirstExecute = STL_FALSE;

    
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
                       aDataArea->mExecNodeList[sOptGroupInstant->mChildNode->mIdx].mOptNode,
                       aDataArea,
                       aEnv );
    
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief GROUP INSTANT TABLE node를 Fetch한다.
 * @param[in]      aFetchNodeInfo  Fetch Node Info
 * @param[out]     aUsedBlockCnt   Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF            End Of Fetch 여부
 * @param[in]      aEnv            Environment
 *
 * @remark Fetch 수행시 호출된다.
 */
stlStatus qlnxFetchGroupHashInstantAccess( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                           stlInt64              * aUsedBlockCnt,
                                           stlBool               * aEOF,
                                           qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode         = NULL;
    qlnoInstant           * sOptGroupInstant  = NULL;
    qlnxInstant           * sExeGroupInstant  = NULL;
    qlnInstantTable       * sInstantTable     = NULL;

    stlInt64                sReadCnt          = 0;
    stlBool                 sEOF              = STL_FALSE;
    
    knlValueBlockList     * sValueBlock       = NULL;
    knlExprStack         ** sExprStack        = NULL;
    smlFetchInfo          * sFetchInfo        = NULL;
    smlAggrFuncInfo       * sAggrExecInfo     = NULL;
    qlnfAggrExecInfo      * sQlnfAggrExecInfo = NULL;
    stlInt32                sInsertColCnt     = 0;
    stlInt32                sLoop             = 0;
    knlExprEvalInfo         sEvalInfo;

    stlInt64                sFetchedCnt       = 0;
    stlInt64                sSkippedCnt       = 0;
    qlnxFetchNodeInfo       sLocalFetchInfo;

    
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

    STL_PARAM_VALIDATE( aFetchNodeInfo->mOptNode->mType == QLL_PLAN_NODE_GROUP_HASH_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    QLL_OPT_CHECK_TIME( sBeginFetchTime );

    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * GROUP INSTANT TABLE Optimization Node 획득
     */

    sOptGroupInstant = (qlnoInstant *) aFetchNodeInfo->mOptNode->mOptNode;

    
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aFetchNodeInfo->mDataArea,
                                        QLL_GET_OPT_NODE_IDX( aFetchNodeInfo->mOptNode ) );


    /**
     * GROUP INSTANT TABLE Execution Node 획득
     */

    sExeGroupInstant = sExecNode->mExecNode;


    /**
     * Used Block Count 초기화
     */
    
    *aUsedBlockCnt = 0;

    
    /**
     * Node 수행 여부 확인 
     */

    STL_DASSERT( *aEOF == STL_FALSE );
    STL_DASSERT( sExeGroupInstant->mFetchInfo.mIsEndOfScan == STL_FALSE );
    STL_DASSERT( aFetchNodeInfo->mFetchCnt > 0);


    /**
     * Local Fetch Info 초기화
     */
    
    QLNX_SET_LOCAL_FETCH_INFO( & sLocalFetchInfo, aFetchNodeInfo );

    
    /*****************************************
     * Node 수행 여부 확인
     ****************************************/

    if( ( sOptGroupInstant->mAggrFuncCnt > 0 ) ||
        ( sOptGroupInstant->mAggrDistFuncCnt > 0 ) )
    {
        if( aFetchNodeInfo->mSkipCnt > 0 )
        {
            sFetchedCnt = 0;
            sSkippedCnt = 1;
        }
        else
        {
            sFetchedCnt = 1;
            sSkippedCnt = 0;
        }
    }
    else
    {
        /* Do Nothing */
    }
    
    if( sExeGroupInstant->mHasFalseFilter == STL_TRUE )
    {
        if( sOptGroupInstant->mAggrFuncCnt > 0 )
        {
            sAggrExecInfo = sExeGroupInstant->mMergeRecord->mAggrFuncInfoList;

            STL_TRY( qlnxInitAggrFunc( sAggrExecInfo,
                                       aEnv )
                     == STL_SUCCESS );

            for( sLoop = 0 ; sLoop < sOptGroupInstant->mAggrFuncCnt ; sLoop++ )
            {
                KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sAggrExecInfo->mAggrValueList, 0, 1 );
                sAggrExecInfo++;
            }
        }
        else if( sOptGroupInstant->mAggrDistFuncCnt > 0 )
        {
            sAggrExecInfo = sExeGroupInstant->mMergeRecord->mAggrFuncInfoList;

            STL_TRY( qlnxInitAggrFunc( sAggrExecInfo,
                                       aEnv )
                     == STL_SUCCESS );

            for( sLoop = 0 ; sLoop < sOptGroupInstant->mAggrDistFuncCnt ; sLoop++ )
            {
                KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sAggrExecInfo->mAggrValueList, 0, 1 );
                sAggrExecInfo++;
            }
        }

        if( sOptGroupInstant->mNestedAggrFuncCnt > 0 )
        {
            if( sOptGroupInstant->mNestedAggrDistFuncCnt > 0 )
            {
                /* fetch & aggr */
                sQlnfAggrExecInfo = sExeGroupInstant->mNestedAggrExecInfo;
            
                for( sLoop = 0 ; sLoop < sOptGroupInstant->mNestedAggrFuncCnt ; sLoop++ )
                {
                    STL_TRY( gQlnfAggrFuncInfoArr[sQlnfAggrExecInfo->mAggrOptInfo->mAggrFuncId].mInitFuncPtr(
                                 sQlnfAggrExecInfo,
                                 0, /* result block idx */
                                 aEnv )
                             == STL_SUCCESS );
                    sQlnfAggrExecInfo++;
                }
            }
            else
            {
                /* aggr fetch */
                sAggrExecInfo = sExeGroupInstant->mFetchInfo.mAggrFetchInfo;
            
                STL_TRY( qlnxInitAggrFunc( sAggrExecInfo,
                                           aEnv )
                         == STL_SUCCESS );

                for( sLoop = 0 ; sLoop < sOptGroupInstant->mNestedAggrFuncCnt ; sLoop++ )
                {
                    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sAggrExecInfo->mAggrValueList, 0, 1 );
                    sAggrExecInfo++;
                }
            }
        }
        else if( sOptGroupInstant->mNestedAggrDistFuncCnt > 0 )
        {
            for( sLoop = 0 ; sLoop < sOptGroupInstant->mNestedAggrDistFuncCnt ; sLoop++ )
            {
                sQlnfAggrExecInfo = (qlnfAggrExecInfo *)(& sExeGroupInstant->mNestedAggrDistExecInfo[ sLoop ]);
            
                STL_TRY( gQlnfAggrFuncInfoArr[sQlnfAggrExecInfo->mAggrOptInfo->mAggrFuncId].mInitFuncPtr(
                             sQlnfAggrExecInfo,
                             0, /* result block idx */
                             aEnv )
                         == STL_SUCCESS );
                sQlnfAggrExecInfo++;
            }
        }
        else
        {
            sFetchedCnt = 0;
        }

        KNL_SET_ALL_BLOCK_USED_CNT( sExeGroupInstant->mReadColBlock, sFetchedCnt );
        *aEOF = STL_TRUE;
        STL_THROW( RAMP_EXIT );
    }
    else
    {
        /* Do Nothing */
    }

    
    /*****************************************
     * Aggregation Fetch 수행
     ****************************************/

    /**
     * Aggregation 포함된 fetch 수행
     */

    if( sOptGroupInstant->mAggrDistFuncCnt > 0 )
    {
        return qlnxFetchAggrDistGroupHashInstantAccess( aFetchNodeInfo,
                                                        aUsedBlockCnt,
                                                        aEOF,
                                                        aEnv );
    }
    else
    {
        /* Do Nothing */
    }

    
    /*****************************************
     * Fetch 수행하면서 Instant Table에 레코드 삽입
     ****************************************/

    sInstantTable = & sExeGroupInstant->mInstantTable;
    sFetchInfo    = & sExeGroupInstant->mFetchInfo;

    if( sInstantTable->mIsNeedBuild == STL_TRUE )
    {
        QLL_OPT_CHECK_TIME( sBeginBuildTime );

        sEvalInfo.mContext  = sLocalFetchInfo.mDataArea->mExprDataContext;
        sEvalInfo.mBlockIdx = 0;

        sExprStack = sOptGroupInstant->mColExprStack;
        
        sInsertColCnt = ( sOptGroupInstant->mInstantNode->mKeyColCnt +
                          sOptGroupInstant->mInstantNode->mRecColCnt );
        
        if( sExeGroupInstant->mMergeRecord != NULL )
        {
            while( sEOF == STL_FALSE )
            {
                /**
                 * Block Read
                 */

                QLL_OPT_CHECK_TIME( sBeginExceptTime );

                /* fetch records*/
                sReadCnt = 0;
                QLNX_FETCH_NODE( & sLocalFetchInfo,
                                 sOptGroupInstant->mChildNode->mIdx,
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
                
                    if( sOptGroupInstant->mNeedEvalKeyCol == STL_TRUE )
                    {
                        sEvalInfo.mBlockCount = sReadCnt;
                        
                        sValueBlock = sExeGroupInstant->mInsertColBlock;
                        for( sLoop = 0 ; sLoop < sOptGroupInstant->mInstantNode->mKeyColCnt ; sLoop++ )
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

                    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sExeGroupInstant->mInsertColBlock, 0, sReadCnt );

                    STL_TRY( smlMergeRecord( sLocalFetchInfo.mStmt,
                                             sInstantTable->mTableHandle,
                                             sExeGroupInstant->mInsertColBlock,
                                             sExeGroupInstant->mMergeRecord,
                                             SML_ENV( aEnv ) )
                             == STL_SUCCESS );
                }
                else
                {
                    STL_DASSERT( sEOF == STL_TRUE );
                    break;
                }
            }
        }
        else
        {
            while( sEOF == STL_FALSE )
            {
                /**
                 * Block Read
                 */

                QLL_OPT_CHECK_TIME( sBeginExceptTime );

                /* fetch records*/
                sReadCnt = 0;
                QLNX_FETCH_NODE( & sLocalFetchInfo,
                                 sOptGroupInstant->mChildNode->mIdx,
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

                    if( sOptGroupInstant->mNeedEvalInsertCol == STL_TRUE )
                    {
                        sEvalInfo.mBlockCount = sReadCnt;
                        
                        sValueBlock = sExeGroupInstant->mInsertColBlock;
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

                    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sExeGroupInstant->mInsertColBlock, 0, sReadCnt );

                    STL_TRY( smlInsertRecord( sLocalFetchInfo.mStmt,
                                              sInstantTable->mTableHandle,
                                              sExeGroupInstant->mInsertColBlock,
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
        }
        
        sInstantTable->mIsNeedBuild = STL_FALSE;


        /**
         * Iterator Allocation
         */
        
        if( sInstantTable->mIterator != NULL )
        {
            STL_TRY_THROW( smlFreeIterator( sInstantTable->mIterator,
                                            SML_ENV( aEnv ) )
                           == STL_SUCCESS, ERR_FREE_ITERATOR );
            
            sInstantTable->mIterator = NULL;
        }

        SML_INIT_ITERATOR_PROP( sInstantTable->mIteratorProperty );
            
        STL_TRY( smlAllocIterator( sLocalFetchInfo.mStmt,
                                   sInstantTable->mTableHandle,
                                   SML_TRM_SNAPSHOT,
                                   SML_SRM_SNAPSHOT,
                                   & sInstantTable->mIteratorProperty,
                                   SML_SCAN_FORWARD,
                                   & sInstantTable->mIterator,
                                   SML_ENV( aEnv ) )
                 == STL_SUCCESS );
        
        if( sInstantTable->mRecCnt == 0 )
        {
            if( sOptGroupInstant->mNestedAggrFuncCnt > 0 )
            {
                if( sOptGroupInstant->mNestedAggrDistFuncCnt > 0 )
                {
                    /* fetch & aggr */
                    sQlnfAggrExecInfo = sExeGroupInstant->mNestedAggrExecInfo;
            
                    for( sLoop = 0 ; sLoop < sOptGroupInstant->mNestedAggrFuncCnt ; sLoop++ )
                    {
                        STL_TRY( gQlnfAggrFuncInfoArr[sQlnfAggrExecInfo->mAggrOptInfo->mAggrFuncId].mInitFuncPtr(
                                     sQlnfAggrExecInfo,
                                     0, /* result block idx */
                                     aEnv )
                                 == STL_SUCCESS );
                        sQlnfAggrExecInfo++;
                    }
                }
                else
                {
                    /* aggr fetch */
                    sAggrExecInfo = sExeGroupInstant->mFetchInfo.mAggrFetchInfo;
            
                    STL_TRY( qlnxInitAggrFunc( sAggrExecInfo,
                                               aEnv )
                             == STL_SUCCESS );

                    for( sLoop = 0 ; sLoop < sOptGroupInstant->mNestedAggrFuncCnt ; sLoop++ )
                    {
                        KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sAggrExecInfo->mAggrValueList, 0, sFetchedCnt );
                        sAggrExecInfo++;
                    }
                }

                KNL_SET_ALL_BLOCK_USED_CNT( sExeGroupInstant->mReadColBlock, sFetchedCnt );
                *aEOF = STL_TRUE;
            }
            else if( sOptGroupInstant->mNestedAggrDistFuncCnt > 0 )
            {
                for( sLoop = 0 ; sLoop < sOptGroupInstant->mNestedAggrDistFuncCnt ; sLoop++ )
                {
                    sQlnfAggrExecInfo = (qlnfAggrExecInfo *)(& sExeGroupInstant->mNestedAggrDistExecInfo[ sLoop ]);
            
                    STL_TRY( gQlnfAggrFuncInfoArr[sQlnfAggrExecInfo->mAggrOptInfo->mAggrFuncId].mInitFuncPtr(
                                 sQlnfAggrExecInfo,
                                 0, /* result block idx */
                                 aEnv )
                             == STL_SUCCESS );
                    sQlnfAggrExecInfo++;
                }

                KNL_SET_ALL_BLOCK_USED_CNT( sExeGroupInstant->mReadColBlock, sFetchedCnt );
                *aEOF = STL_TRUE;
            }
            else
            {
                sFetchedCnt = 0;
                
                KNL_SET_ALL_BLOCK_USED_CNT( sExeGroupInstant->mReadColBlock, sFetchedCnt );
                *aEOF = STL_TRUE;
            }
            STL_THROW( RAMP_EXIT );
        }

        QLL_OPT_CHECK_TIME( sEndBuildTime );
        QLL_OPT_ADD_ELAPSED_TIME( sBuildTime, sBeginBuildTime, sEndBuildTime );
        QLL_OPT_SET_VALUE( sExeGroupInstant->mCommonInfo.mBuildTime, sBuildTime - sExceptTime );

        QLL_OPT_SET_VALUE( sExceptTime, 0 );
    }
    else
    {
        /* Do Nothing */
    }


    /*****************************************
     * FETCH
     ****************************************/

    /**
     * Distinct 조건이 Aggregation 포함된 fetch 수행
     */
    
    if( sOptGroupInstant->mNestedAggrDistFuncCnt > 0 )
    {
        sFetchInfo->mSkipCnt  = aFetchNodeInfo->mSkipCnt;
        sFetchInfo->mFetchCnt = 1;
            
        STL_TRY( qlnxFetchAggrDist( sLocalFetchInfo.mStmt,
                                    sInstantTable->mIterator,
                                    sFetchInfo,
                                    sOptGroupInstant->mNestedAggrFuncCnt,
                                    sOptGroupInstant->mNestedAggrDistFuncCnt,
                                    sExeGroupInstant->mNestedAggrExecInfo,
                                    sExeGroupInstant->mNestedAggrDistExecInfo,
                                    & sFetchedCnt,
                                    aEOF,
                                    aEnv )
                 == STL_SUCCESS );
        
        STL_DASSERT( *aEOF == STL_TRUE );
        STL_THROW( RAMP_EXIT );
    }
    else
    {
        /**
         * Aggregation이 없는 fetch 수행
         */
        
        if( sOptGroupInstant->mNestedAggrFuncCnt > 0 )
        {
            sFetchInfo->mSkipCnt  = 0;
            sFetchInfo->mFetchCnt = 1;
                
            STL_TRY( qlnxInitAggrFunc( sFetchInfo->mAggrFetchInfo,
                                       aEnv )
                     == STL_SUCCESS );

            STL_TRY( smlFetchAggr( sInstantTable->mIterator,
                                   sFetchInfo,
                                   SML_ENV(aEnv) )
                     == STL_SUCCESS );

            sAggrExecInfo = sFetchInfo->mAggrFetchInfo;
    
            for( sLoop = 0 ; sLoop < sOptGroupInstant->mNestedAggrFuncCnt ; sLoop++ )
            {
                KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sAggrExecInfo->mAggrValueList, 0, sFetchedCnt );
                sAggrExecInfo = sAggrExecInfo->mNext;
            }
            
            *aEOF = STL_TRUE;
        }
        else
        {
            /**
             * Skip Count 와 Fetch Count 조정
             */

            STL_DASSERT( sInstantTable->mIterator != NULL );

            sFetchInfo->mSkipCnt  = aFetchNodeInfo->mSkipCnt;
            sFetchInfo->mFetchCnt = aFetchNodeInfo->mFetchCnt;
        
            /* fetch records */
            STL_TRY( smlFetchNext( sInstantTable->mIterator,
                                   sFetchInfo,
                                   SML_ENV(aEnv) )
                     == STL_SUCCESS );
        
            sFetchedCnt = SML_USED_BLOCK_SIZE( sFetchInfo->mRowBlock );
            sSkippedCnt = aFetchNodeInfo->mSkipCnt - sFetchInfo->mSkipCnt;
            
            *aEOF = sFetchInfo->mIsEndOfScan;

            sInstantTable->mCurIdx += sFetchedCnt;
        }
    }

    
    STL_RAMP( RAMP_EXIT );

    /**
     * OUTPUT 설정
     */

    *aUsedBlockCnt = sFetchedCnt;
    
    /* Parent의 Fetch Node Info 설정 */
    aFetchNodeInfo->mFetchCnt -= sFetchedCnt;
    aFetchNodeInfo->mSkipCnt  -= sSkippedCnt;

    
    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeGroupInstant->mCommonInfo.mFetchTime, sFetchTime -sBuildTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeGroupInstant->mCommonInfo.mFetchCallCount, 1 );
    QLL_OPT_ADD_RECORD_STAT_INFO( sExeGroupInstant->mCommonInfo.mFetchRecordStat,
                                  sExeGroupInstant->mCommonInfo.mResultColBlock,
                                  *aUsedBlockCnt,
                                  aEnv );

    sExeGroupInstant->mCommonInfo.mFetchRowCnt += *aUsedBlockCnt;

    return STL_SUCCESS;

    STL_CATCH( ERR_FREE_ITERATOR )
    {
        sInstantTable->mIterator = NULL;
    }
    
    STL_FINISH;

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeGroupInstant->mCommonInfo.mFetchTime, sFetchTime -sBuildTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeGroupInstant->mCommonInfo.mFetchCallCount, 1 );

    return STL_FAILURE;
}


/**
 * @brief GROUP INSTANT TABLE node 수행을 종료한다.
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 *
 * @remark Execution을 종료하고자 할 때 호출한다.
 */
stlStatus qlnxFinalizeGroupHashInstantAccess( qllOptimizationNode   * aOptNode,
                                              qllDataArea           * aDataArea,
                                              qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode        = NULL;
    qlnoInstant           * sOptGroupInstant = NULL;
    qlnxInstant           * sExeGroupInstant = NULL;

    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_GROUP_HASH_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    
    /**
     * GROUP INSTANT TABLE Optimization Node 획득
     */

    sOptGroupInstant = (qlnoInstant *) aOptNode->mOptNode;


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * 해당 노드를 포함하는 Query Expression Optimization Node 획득
     */

    /* sOptQueryNode = aOptNode->mOptCurQueryNode; */


    /**
     * GROUP INSTANT ACCESS Execution Node 획득
     */

    sExeGroupInstant = (qlnxInstant *) sExecNode->mExecNode;

    
    /**
     * Instant Table 해제
     */

    STL_TRY( qlnxDropInstantTable( & sExeGroupInstant->mInstantTable,
                                   aEnv )
             == STL_SUCCESS );


    /**
     * Aggregation Distinct를 위한 Instant Table 해제
     */

    if( sOptGroupInstant->mAggrDistFuncCnt > 0 )
    {
        STL_TRY( qlnxFinalizeAggrConflictCheck( sOptGroupInstant->mAggrDistFuncCnt,
                                                sExeGroupInstant->mConflictCheckInfo,
                                                aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /* Do Nothing */
    }


    /**
     * Nested Aggregation Distinct를 위한 Instant Table 해제
     */

    if( sOptGroupInstant->mNestedAggrDistFuncCnt > 0 )
    {
        STL_TRY( qlnxFinalizeAggrDist( sOptGroupInstant->mNestedAggrDistFuncCnt,
                                       sExeGroupInstant->mNestedAggrDistExecInfo,
                                       aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /* Do Nothing */
    }

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief GROUP INSTANT TABLE node의 Aggregation을 Fetch한다.
 * @param[in]      aFetchNodeInfo  Fetch Node Info
 * @param[out]     aUsedBlockCnt   Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF            End Of Fetch 여부
 * @param[in]      aEnv            Environment
 *
 * @remark Fetch 수행시 호출된다.
 */
stlStatus qlnxFetchAggrDistGroupHashInstantAccess( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                                   stlInt64              * aUsedBlockCnt,
                                                   stlBool               * aEOF,
                                                   qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode         = NULL;
    qlnoInstant           * sOptGroupInstant  = NULL;
    qlnxInstant           * sExeGroupInstant  = NULL;
    qlnInstantTable       * sInstantTable     = NULL;

    stlInt64                sReadCnt          = 0;
    stlBool                 sEOF              = STL_FALSE;

    knlValueBlockList     * sValueBlock       = NULL;
    knlExprStack         ** sExprStack        = NULL;
    smlFetchInfo          * sFetchInfo        = NULL;
    smlAggrFuncInfo       * sAggrExecInfo     = NULL;
    qlnfAggrExecInfo      * sQlnfAggrExecInfo = NULL;
    /* stlInt32                sInsertColCnt     = 0; */
    stlInt32                sLoop             = 0;
    stlInt32                sBlockIdx         = 0;
    knlExprEvalInfo         sEvalInfo;

    qlnxConflictCheckInfo * sConflictCheck    = NULL;
    dtlDataValue          * sResultValue      = NULL;
    dtlDataValue          * sDataValue        = NULL;
    stlInt64                sFetchedCnt       = 0;
    stlInt64                sSkippedCnt       = 0;
    qlnxFetchNodeInfo       sLocalFetchInfo;

    
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

    STL_PARAM_VALIDATE( aFetchNodeInfo->mOptNode->mType == QLL_PLAN_NODE_GROUP_HASH_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    QLL_OPT_CHECK_TIME( sBeginFetchTime );

    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * GROUP INSTANT TABLE Optimization Node 획득
     */

    sOptGroupInstant = (qlnoInstant *) aFetchNodeInfo->mOptNode->mOptNode;

    
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aFetchNodeInfo->mDataArea, QLL_GET_OPT_NODE_IDX( aFetchNodeInfo->mOptNode ) );


    /**
     * GROUP INSTANT TABLE Execution Node 획득
     */

    sExeGroupInstant = sExecNode->mExecNode;


    /**
     * Used Block Count 초기화
     */
    
    *aUsedBlockCnt = 0;


    /**
     * Node 수행 여부 확인 
     */

    STL_DASSERT( *aEOF == STL_FALSE );
    STL_DASSERT( sExeGroupInstant->mFetchInfo.mIsEndOfScan == STL_FALSE );
    STL_DASSERT( aFetchNodeInfo->mFetchCnt > 0);

    
    /**
     * Local Fetch Info 초기화
     */
    
    QLNX_SET_LOCAL_FETCH_INFO( & sLocalFetchInfo, aFetchNodeInfo );


    /*****************************************
     * Fetch 수행하면서 Instant Table에 레코드 삽입
     ****************************************/

    /**
     * Internal Fetch Info 설정
     */
        
    sInstantTable = & sExeGroupInstant->mInstantTable;
    sFetchInfo    = & sExeGroupInstant->mFetchInfo;

    if( ( sOptGroupInstant->mNestedAggrFuncCnt > 0 ) ||
        ( sOptGroupInstant->mNestedAggrDistFuncCnt > 0 ) )
    {
        if( aFetchNodeInfo->mSkipCnt > 0 )
        {
            sFetchedCnt = 0;
            sSkippedCnt = 1;
        }
        else
        {
            sFetchedCnt = 1;
            sSkippedCnt = 0;
        }
    }
    else
    {
        /* Do Nothing */
    }
    
    if( sInstantTable->mIsNeedBuild == STL_TRUE )
    {
        QLL_OPT_CHECK_TIME( sBeginBuildTime );

        sEvalInfo.mContext  = sLocalFetchInfo.mDataArea->mExprDataContext;
        sEvalInfo.mBlockIdx = 0;

        sExprStack = sOptGroupInstant->mColExprStack;
        
        /* sInsertColCnt = ( sOptGroupInstant->mInstantNode->mKeyColCnt + */
        /*                   sOptGroupInstant->mInstantNode->mRecColCnt ); */
        
        STL_DASSERT( sExeGroupInstant->mMergeRecord != NULL );

        while( sEOF == STL_FALSE )
        {
            /**
             * Block Read
             */

            QLL_OPT_CHECK_TIME( sBeginExceptTime );

            /* fetch records*/
            sReadCnt = 0;
            QLNX_FETCH_NODE( & sLocalFetchInfo,
                             sOptGroupInstant->mChildNode->mIdx,
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

                sValueBlock = sExeGroupInstant->mInsertColBlock;
                for( sLoop = 0 ; sLoop < sOptGroupInstant->mInstantNode->mKeyColCnt ; sLoop++ )
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

                
                /**
                 * Aggregation Distinct의 Conflict 체크
                 */

                KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sExeGroupInstant->mInsertColBlock, 0, sReadCnt );

                sConflictCheck = sExeGroupInstant->mConflictCheckInfo;

                for( sLoop = 0 ; sLoop < sOptGroupInstant->mAggrDistFuncCnt ; sLoop++ )
                {
                    /* aggregation의 argument는 하나임 */
                    sEvalInfo.mExprStack = sOptGroupInstant->mAggrDistOptInfo[ sLoop ].mArgCodeStack[ 0 ];

                    STL_DASSERT( sEvalInfo.mExprStack != NULL );

                    sEvalInfo.mResultBlock = sConflictCheck->mConflictColList;
                    STL_TRY( knlEvaluateBlockExpression( & sEvalInfo,
                                                         KNL_ENV( aEnv ) )
                             == STL_SUCCESS );
                    
                    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sConflictCheck->mInsertColList, 0, sReadCnt );
                    
                    STL_TRY( smlInsertRecord( sLocalFetchInfo.mStmt,
                                              sConflictCheck->mInstantTable.mTableHandle,
                                              sConflictCheck->mInsertColList,
                                              sConflictCheck->mConflictResult, /* unique violation */
                                              sFetchInfo->mRowBlock,
                                              SML_ENV( aEnv ) )
                             == STL_SUCCESS );

                    STL_DASSERT( DTL_GET_BLOCK_IS_SEP_BUFF( sConflictCheck->mConflictColList ) == STL_TRUE );
                    STL_DASSERT( sConflictCheck->mConflictColList->mNext == NULL );
                    
                    sResultValue = DTL_GET_BLOCK_FIRST_DATA_VALUE( sConflictCheck->mConflictResult );
                    sDataValue   = DTL_GET_BLOCK_FIRST_DATA_VALUE( sConflictCheck->mConflictColList );
                    
                    for( sBlockIdx = 0 ; sBlockIdx < sReadCnt ; sBlockIdx++ )
                    {
                        if( DTL_BOOLEAN_IS_TRUE( sResultValue ) )
                        {
                            DTL_SET_NULL( sDataValue );
                        }
                        sResultValue++;
                        sDataValue++;
                    }
                    
                    sConflictCheck++;
                }
                

                /**
                 * Instant Table에 Record 삽입
                 */

                STL_TRY( smlMergeRecord( sLocalFetchInfo.mStmt,
                                         sInstantTable->mTableHandle,
                                         sExeGroupInstant->mInsertColBlock,
                                         sExeGroupInstant->mMergeRecord,
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
            STL_TRY_THROW( smlFreeIterator( sInstantTable->mIterator,
                                            SML_ENV( aEnv ) )
                           == STL_SUCCESS, ERR_FREE_ITERATOR );
            
            sInstantTable->mIterator = NULL;
        }

        SML_INIT_ITERATOR_PROP( sInstantTable->mIteratorProperty );
            
        STL_TRY( smlAllocIterator( sLocalFetchInfo.mStmt,
                                   sInstantTable->mTableHandle,
                                   SML_TRM_SNAPSHOT,
                                   SML_SRM_SNAPSHOT,
                                   & sInstantTable->mIteratorProperty,
                                   SML_SCAN_FORWARD,
                                   & sInstantTable->mIterator,
                                   SML_ENV( aEnv ) )
                 == STL_SUCCESS );
        
        if( sInstantTable->mRecCnt == 0 )
        {
            if( sOptGroupInstant->mNestedAggrFuncCnt > 0 )
            {
                if( sOptGroupInstant->mNestedAggrDistFuncCnt > 0 )
                {
                    /* fetch & aggr */
                    sQlnfAggrExecInfo = sExeGroupInstant->mNestedAggrExecInfo;
            
                    for( sLoop = 0 ; sLoop < sOptGroupInstant->mNestedAggrFuncCnt ; sLoop++ )
                    {
                        STL_TRY( gQlnfAggrFuncInfoArr[sQlnfAggrExecInfo->mAggrOptInfo->mAggrFuncId].mInitFuncPtr(
                                     sQlnfAggrExecInfo,
                                     0, /* result block idx */
                                     aEnv )
                                 == STL_SUCCESS );
                        sQlnfAggrExecInfo++;
                    }
                }
                else
                {
                    /* aggr fetch */
                    sAggrExecInfo = sExeGroupInstant->mFetchInfo.mAggrFetchInfo;
            
                    STL_TRY( qlnxInitAggrFunc( sAggrExecInfo,
                                               aEnv )
                             == STL_SUCCESS );

                    for( sLoop = 0 ; sLoop < sOptGroupInstant->mNestedAggrFuncCnt ; sLoop++ )
                    {
                        KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sAggrExecInfo->mAggrValueList, 0, sFetchedCnt );
                        sAggrExecInfo++;
                    }
                }

                KNL_SET_ALL_BLOCK_USED_CNT( sExeGroupInstant->mReadColBlock, sFetchedCnt );
                *aEOF = STL_TRUE;
            }
            else if( sOptGroupInstant->mNestedAggrDistFuncCnt > 0 )
            {
                for( sLoop = 0 ; sLoop < sOptGroupInstant->mNestedAggrDistFuncCnt ; sLoop++ )
                {
                    sQlnfAggrExecInfo = (qlnfAggrExecInfo *)(& sExeGroupInstant->mNestedAggrDistExecInfo[ sLoop ]);
            
                    STL_TRY( gQlnfAggrFuncInfoArr[sQlnfAggrExecInfo->mAggrOptInfo->mAggrFuncId].mInitFuncPtr(
                                 sQlnfAggrExecInfo,
                                 0, /* result block idx */
                                 aEnv )
                             == STL_SUCCESS );
                    sQlnfAggrExecInfo++;
                }

                KNL_SET_ALL_BLOCK_USED_CNT( sExeGroupInstant->mReadColBlock, sFetchedCnt );
                *aEOF = STL_TRUE;
            }
            else
            {
                sFetchedCnt = 0;
                
                KNL_SET_ALL_BLOCK_USED_CNT( sExeGroupInstant->mReadColBlock, sFetchedCnt );
                *aEOF = STL_TRUE;
            }

            STL_THROW( RAMP_EXIT );
        }

        QLL_OPT_CHECK_TIME( sEndBuildTime );
        QLL_OPT_ADD_ELAPSED_TIME( sBuildTime, sBeginBuildTime, sEndBuildTime );
        QLL_OPT_SET_VALUE( sExeGroupInstant->mCommonInfo.mBuildTime, sBuildTime - sExceptTime );

        QLL_OPT_SET_VALUE( sExceptTime, 0 );
    }
    else
    {
        /* Do Nothing */
    }

        
    /*****************************************
     * Fetch
     ****************************************/

    /**
     * Distinct 조건이 Aggregation 포함된 fetch 수행
     */
    
    if( sOptGroupInstant->mNestedAggrDistFuncCnt > 0 )
    {
        sFetchInfo->mSkipCnt  = aFetchNodeInfo->mSkipCnt;
        sFetchInfo->mFetchCnt = 1;
            
        STL_TRY( qlnxFetchAggrDist( sLocalFetchInfo.mStmt,
                                    sInstantTable->mIterator,
                                    sFetchInfo,
                                    sOptGroupInstant->mNestedAggrFuncCnt,
                                    sOptGroupInstant->mNestedAggrDistFuncCnt,
                                    sExeGroupInstant->mNestedAggrExecInfo,
                                    sExeGroupInstant->mNestedAggrDistExecInfo,
                                    & sFetchedCnt,
                                    aEOF,
                                    aEnv )
                 == STL_SUCCESS );
        
        STL_DASSERT( *aEOF == STL_TRUE );
        STL_THROW( RAMP_EXIT );
    }
    else
    {
        /**
         * Distinct Aggregation이 없는 fetch 수행
         */
        
        if( sOptGroupInstant->mNestedAggrFuncCnt > 0 )
        {
            sFetchInfo->mSkipCnt  = 0;
            sFetchInfo->mFetchCnt = 1;
                
            STL_TRY( qlnxInitAggrFunc( sFetchInfo->mAggrFetchInfo,
                                       aEnv )
                     == STL_SUCCESS );

            STL_TRY( smlFetchAggr( sInstantTable->mIterator,
                                   sFetchInfo,
                                   SML_ENV(aEnv) )
                     == STL_SUCCESS );

            sAggrExecInfo = sFetchInfo->mAggrFetchInfo;
    
            for( sLoop = 0 ; sLoop < sOptGroupInstant->mNestedAggrFuncCnt ; sLoop++ )
            {
                KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sAggrExecInfo->mAggrValueList, 0, sFetchedCnt );
                sAggrExecInfo = sAggrExecInfo->mNext;
            }
            
            *aEOF = STL_TRUE;
        }
        else
        {
            /**
             * Skip Count 와 Fetch Count 조정
             */
        
            sFetchInfo->mSkipCnt  = aFetchNodeInfo->mSkipCnt;
            sFetchInfo->mFetchCnt = aFetchNodeInfo->mFetchCnt;
        
            /* fetch records */
            STL_TRY( smlFetchNext( sInstantTable->mIterator,
                                   sFetchInfo,
                                   SML_ENV(aEnv) )
                     == STL_SUCCESS );
        
            sFetchedCnt = SML_USED_BLOCK_SIZE( sFetchInfo->mRowBlock );
            sSkippedCnt = aFetchNodeInfo->mSkipCnt - sFetchInfo->mSkipCnt;
            
            *aEOF = sFetchInfo->mIsEndOfScan;

            sInstantTable->mCurIdx += sFetchedCnt;
        }
    }

    
    STL_RAMP( RAMP_EXIT );


    /**
     * OUTPUT 설정
     */

    *aUsedBlockCnt = sFetchedCnt;
    
    /* Parent의 Fetch Node Info 설정 */
    aFetchNodeInfo->mFetchCnt -= sFetchedCnt;
    aFetchNodeInfo->mSkipCnt  -= sSkippedCnt;


    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeGroupInstant->mCommonInfo.mFetchTime, sFetchTime - sBuildTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeGroupInstant->mCommonInfo.mFetchCallCount, 1 );
    QLL_OPT_ADD_RECORD_STAT_INFO( sExeGroupInstant->mCommonInfo.mFetchRecordStat,
                                  sExeGroupInstant->mCommonInfo.mResultColBlock,
                                  *aUsedBlockCnt,
                                  aEnv );


    return STL_SUCCESS;

    STL_CATCH( ERR_FREE_ITERATOR )
    {
        sInstantTable->mIterator = NULL;
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlnx */
