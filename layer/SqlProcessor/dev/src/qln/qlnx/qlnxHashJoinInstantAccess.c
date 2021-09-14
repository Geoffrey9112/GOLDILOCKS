/*******************************************************************************
 * qlnxHashJoinInstantAccess.c
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
 * @file qlnxHashJoinInstantAccess.c
 * @brief SQL Processor Layer Execution Node - HASH JOIN INSTANT TABLE
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnx
 * @{
 */

/**
 * @brief HASH JOIN INSTANT TABLE node를 초기화한다.
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
stlStatus qlnxInitializeHashJoinInstantAccess( smlTransId              aTransID,
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
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_HASH_JOIN_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * HASH JOIN INSTANT TABLE Optimization Node 획득
     */

    sOptHashInstant = (qlnoInstant *) aOptNode->mOptNode;


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * HASH JOIN INSTANT TABLE Execution Node 할당
     */

    sExeHashInstant = (qlnxInstant *) sExecNode->mExecNode;

    STL_DASSERT( sExeHashInstant->mKeyColBlock != NULL );
    STL_DASSERT( sExeHashInstant->mInsertColBlock != NULL );


    /**
     * HASH JOIN INSTANT TABLE 생성
     */

    stlMemset( & sExeHashInstant->mInstantTable, 0x00, STL_SIZEOF( qlnInstantTable ) );

    if( sExeHashInstant->mNeedMaterialize == STL_TRUE )
    {
        /**
         * @todo SM - Hash Semi Join 반영시 주석 풀기
         */
        
        if( sOptHashInstant->mHashTableAttr.mUniquenessFlag == STL_TRUE )
        {
            STL_TRY( qlnxCreateGroupHashInstantTable( aTransID,
                                                      aStmt,
                                                      & sExeHashInstant->mInstantTable,
                                                      sOptHashInstant->mHashTableAttr.mSkipNullFlag,
                                                      sOptHashInstant->mInstantNode->mKeyColCnt,
                                                      sExeHashInstant->mInsertColBlock,
                                                      aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            STL_TRY( qlnxCreateFlatInstantTable( aTransID,
                                                 aStmt,
                                                 & sExeHashInstant->mInstantTable,
                                                 STL_FALSE,  /* aIsScrollableInstant */
                                                 aEnv )
                     == STL_SUCCESS );
        
            STL_TRY( qlnxCreateHashJoinInstantIndex( aTransID,
                                                     aStmt,
                                                     & sExeHashInstant->mInstantTable,
                                                     sOptHashInstant->mInstantNode->mKeyColCnt,
                                                     sExeHashInstant->mInsertColBlock,
                                                     & sOptHashInstant->mHashTableAttr,
                                                     aEnv )
                     == STL_SUCCESS );
        }
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
    

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief HASH JOIN INSTANT TABLE node를 수행한다.
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
stlStatus qlnxExecuteHashJoinInstantAccess( smlTransId              aTransID,
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
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_HASH_JOIN_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * HASH JOIN INSTANT TABLE Optimization Node 획득
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
     * HASH JOIN INSTANT ACCESS Execution Node 획득
     */

    sExeHashInstant = (qlnxInstant *) sExecNode->mExecNode;

    /*****************************************
     * Hash Join Instant Table Iterator 초기화
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
            
            /**
             * @todo Index를 위한 Instant에 대한 truncate 함수 지원시 주석 제거
             */
            
            /* STL_TRY( smlTruncateIndex( aStmt, */
            /*                            sExeHashInstant->mInstantIndex.mIndexHandle, */
            /*                            NULL, */
            /*                            SML_DROP_STORAGE_REUSE, */
            /*                            & sExeHashInstant->mInstantIndex.mIndexHandle, */
            /*                            & sNewRelationId, */
            /*                            SML_ENV(aEnv) ) */
            /*          == STL_SUCCESS ); */
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
 * @brief HASH JOIN INSTANT TABLE node를 Fetch한다.
 * @param[in]      aFetchNodeInfo  Fetch Node Info
 * @param[out]     aUsedBlockCnt   Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF            End Of Fetch 여부
 * @param[in]      aEnv            Environment
 *
 * @remark Fetch 수행시 호출된다.
 */
stlStatus qlnxFetchHashJoinInstantAccess( qlnxFetchNodeInfo     * aFetchNodeInfo,
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

    qlnxFetchNodeInfo       sLocalFetchInfo;

    QLL_OPT_DECLARE( stlTime sBeginBuildTime );
    QLL_OPT_DECLARE( stlTime sEndBuildTime );
    QLL_OPT_DECLARE( stlTime sBuildTime );

    QLL_OPT_DECLARE( stlTime sBeginExceptTime );
    QLL_OPT_DECLARE( stlTime sEndExceptTime );
    QLL_OPT_DECLARE( stlTime sExceptTime );

    QLL_OPT_SET_VALUE( sBuildTime, 0 );
    QLL_OPT_SET_VALUE( sExceptTime, 0 );

    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aFetchNodeInfo->mOptNode->mType == QLL_PLAN_NODE_HASH_JOIN_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * HASH JOIN INSTANT TABLE Optimization Node 획득
     */

    sOptHashInstant = (qlnoInstant *) aFetchNodeInfo->mOptNode->mOptNode;

    
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aFetchNodeInfo->mDataArea,
                                        QLL_GET_OPT_NODE_IDX( aFetchNodeInfo->mOptNode ) );


    /**
     * HASH JOIN INSTANT TABLE Execution Node 획득
     */

    sExeHashInstant = sExecNode->mExecNode;


    /**
     * Local Fetch Info 초기화
     */
    
    QLNX_SET_LOCAL_FETCH_INFO( & sLocalFetchInfo, aFetchNodeInfo );


    /*****************************************
     * Node 수행 여부 확인
     ****************************************/

    if( sExeHashInstant->mHasFalseFilter == STL_TRUE )
    {
        /* Coverage : hash join 노드에서 push 된 false filter는 하위 노드에 전달된다. */
        KNL_SET_ALL_BLOCK_USED_CNT( sExeHashInstant->mReadColBlock, 0 );
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

        sEvalInfo.mContext  = sLocalFetchInfo.mDataArea->mExprDataContext;
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
            QLNX_FETCH_NODE( & sLocalFetchInfo,
                             sOptHashInstant->mChildNode->mIdx,
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
                
                if( sOptHashInstant->mNeedEvalInsertCol == STL_TRUE )
                {
                    sEvalInfo.mBlockCount = sReadCnt;

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

                STL_TRY( smlInsertRecord( sLocalFetchInfo.mStmt,
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
         * Outer Join 을 위해 저장 Row 의 존재 여부에 관계없이 iterator 를 생성해야 함.
         */

        SML_SET_ITERATOR_PROP( sInstantTable->mIteratorProperty,
                               0,
                               STL_UINT64_MAX,
                               NULL,
                               STL_FALSE,
                               sOptHashInstant->mIsSkipHitRecord );
        
        if( sInstantTable->mIterator != NULL )
        {
            STL_TRY_THROW( smlFreeIterator( sInstantTable->mIterator,
                                            SML_ENV( aEnv ) )
                           == STL_SUCCESS, ERR_FREE_ITERATOR );
            
            sInstantTable->mIterator = NULL;
        }
        
        if( sOptHashInstant->mHashTableAttr.mUniquenessFlag == STL_TRUE )
        {
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
        }
        else
        {
            STL_TRY( smlBuildIndex( sLocalFetchInfo.mStmt,
                                    sInstantTable->mIndexHandle,
                                    sInstantTable->mTableHandle,
                                    0,    /* meaningless */
                                    NULL, /* meaningless */
                                    NULL, /* meaningless */
                                    0,    /* meaningless */
                                    SML_ENV(aEnv) )
                     == STL_SUCCESS );
            
            STL_TRY( smlAllocIterator( sLocalFetchInfo.mStmt,
                                       sInstantTable->mIndexHandle,
                                       SML_TRM_SNAPSHOT,
                                       SML_SRM_SNAPSHOT,
                                       & sInstantTable->mIteratorProperty,
                                       SML_SCAN_FORWARD,
                                       & sInstantTable->mIterator,
                                       SML_ENV( aEnv ) )
                     == STL_SUCCESS );
        }

        QLL_OPT_CHECK_TIME( sEndBuildTime );
        QLL_OPT_ADD_ELAPSED_TIME( sBuildTime, sBeginBuildTime, sEndBuildTime );
        QLL_OPT_SET_VALUE( sExeHashInstant->mCommonInfo.mBuildTime, sBuildTime - sExceptTime );

        QLL_OPT_SET_VALUE( sExceptTime, 0 );

        if( sInstantTable->mRecCnt == 0 )
        {
            KNL_SET_ALL_BLOCK_USED_CNT( sExeHashInstant->mReadColBlock, 0 );
        }
        else
        {
            /* Do Nothing */
        }
    }
    else
    {
        /* nothing to do */
    }

    STL_RAMP( RAMP_EXIT );
    

    /**
     * OUTPUT 설정
     */

    *aEOF = STL_TRUE;
    *aUsedBlockCnt = 0;

    /* Parent의 Fetch Node Info에 대한 변경이 없음 */

    
    /*****************************************
     * Fetch
     ****************************************/

    /**
     * Blocked Hash Join 이 수행되므로, 별도의 Fetch 를 수행하지 않는다.
     * Fetch 는 상위 Hash Join 에 의해 제어됨.
     */

    QLL_OPT_ADD_COUNT( sExeHashInstant->mCommonInfo.mFetchCallCount, 1 );

    sExeHashInstant->mCommonInfo.mFetchRowCnt = 0;
    
    return STL_SUCCESS;

    STL_CATCH( ERR_FREE_ITERATOR )
    {
        sInstantTable->mIterator = NULL;
    }
    
    STL_FINISH;

    QLL_OPT_ADD_COUNT( sExeHashInstant->mCommonInfo.mFetchCallCount, 1 );

    return STL_FAILURE;
}


/**
 * @brief HASH JOIN INSTANT TABLE node 수행을 종료한다.
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 *
 * @remark Execution을 종료하고자 할 때 호출한다.
 */
stlStatus qlnxFinalizeHashJoinInstantAccess( qllOptimizationNode   * aOptNode,
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
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_HASH_JOIN_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    
    /**
     * HASH JOIN INSTANT TABLE Optimization Node 획득
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
     * HASH JOIN INSTANT ACCESS Execution Node 획득
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


/** @} qlnx */
