/*******************************************************************************
 * qlnxUnionDistinct.c
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
 * @file qlnxUnionDistinct.c
 * @brief SQL Processor Layer Execution Node - UNION DISTINCT
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnx
 * @{
 */

/**
 * @brief UNION DISTINCT node 를 초기화한다.
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aStmt         Statement
 * @param[in]  aDBCStmt      DBC Statement
 * @param[in]  aSQLStmt      SQL Statement
 * @param[in]  aOptNode      Optimization Node
 * @param[in]  aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]  aEnv          Environment
 * @remarks 
 */
stlStatus qlnxInitializeUnionDistinct( smlTransId              aTransID,
                                       smlStatement          * aStmt,
                                       qllDBCStmt            * aDBCStmt,
                                       qllStatement          * aSQLStmt,
                                       qllOptimizationNode   * aOptNode,
                                       qllDataArea           * aDataArea,
                                       qllEnv                * aEnv )
{
    qllExecutionNode   * sExecNode  = NULL;
    qlnoSetOP          * sOptSetOP  = NULL;
    qlnxSetOP          * sExeSetOP  = NULL;

    smlIndexAttr       sIndexAttr;
    stlInt64           sTablePhyID = 0;
    ellDictHandle      sSpaceHandle;
    
    /**
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_UNION_DISTINCT_TYPE, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );

    /*****************************************
     * 기본 정보 획득
     ****************************************/

    sOptSetOP = (qlnoSetOP *) aOptNode->mOptNode;
    
    sExecNode    = QLL_GET_EXECUTION_NODE( aDataArea, aOptNode->mIdx );
    sExeSetOP = (qlnxSetOP *) sExecNode->mExecNode;

    /*****************************************
     * Instant Table 생성
     ****************************************/

    /**
     * Instant Table 정보 초기화 
     */

    sExeSetOP->mInstantTable.mIsNeedBuild = STL_TRUE;
    sExeSetOP->mInstantTable.mRecCnt = 0;
    sExeSetOP->mInstantTable.mCurIdx = 0;

    sExeSetOP->mInstantTable.mStmt = aStmt;
    sExeSetOP->mInstantTable.mTableHandle = NULL;
    sExeSetOP->mInstantTable.mIndexHandle = NULL;
    sExeSetOP->mInstantTable.mIterator = NULL;
    SML_INIT_ITERATOR_PROP( sExeSetOP->mInstantTable.mIteratorProperty );
    
    /**
     * Index Attribute 설정
     */
    
    stlMemset( & sIndexAttr, 0x00, STL_SIZEOF(smlIndexAttr) );

    sIndexAttr.mValidFlags |= SML_INDEX_LOGGING_YES;
    sIndexAttr.mLoggingFlag = STL_FALSE;
    
    sIndexAttr.mValidFlags |= SML_INDEX_UNIQUENESS_YES;
    sIndexAttr.mUniquenessFlag = STL_TRUE;

    /**
     * Current User 의 Default Temporary Space Handle 획득
     */
    
    STL_TRY( ellGetCurrentTempSpaceHandle( aTransID,
                                           aStmt->mScn,
                                           & sSpaceHandle,
                                           ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Instant Table 생성
     */

    STL_TRY( smlCreateIoTable( aStmt,
                               ellGetTablespaceID( & sSpaceHandle ),
                               SML_MEMORY_INSTANT_HASH_TABLE,
                               NULL,  /* aTableAttr */
                               & sIndexAttr,
                               sExeSetOP->mColumnBlock,
                               sOptSetOP->mRootQuerySet->mTargetCount,
                               NULL,  /* aKeyColFlags */
                               & sTablePhyID,
                               & sExeSetOP->mInstantTable.mTableHandle,
                               SML_ENV( aEnv ) )
             == STL_SUCCESS );

    /**
     * 수행할 Child 초기화
     */
    
    sExeSetOP->mFetchChildIdx = 0;
    
    return STL_SUCCESS;
   
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief UNION DISTINCT node 를 수행한다.
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aStmt         Statement
 * @param[in]  aDBCStmt      DBC Statement
 * @param[in]  aSQLStmt      SQL Statement
 * @param[in]  aOptNode      Optimization Node
 * @param[in]  aDataArea     Data Area
 * @param[in]  aEnv          Environment
 * @remarks 
 */
stlStatus qlnxExecuteUnionDistinct( smlTransId              aTransID,
                                    smlStatement          * aStmt,
                                    qllDBCStmt            * aDBCStmt,
                                    qllStatement          * aSQLStmt,
                                    qllOptimizationNode   * aOptNode,
                                    qllDataArea           * aDataArea,
                                    qllEnv                * aEnv )
{
    qllExecutionNode   * sExecNode  = NULL;
    qlnoSetOP          * sOptSetOP  = NULL;
    qlnxSetOP          * sExeSetOP  = NULL;

    stlInt32  i = 0;
    
    /**
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_UNION_DISTINCT_TYPE, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );

    /*****************************************
     * 기본 정보 획득
     ****************************************/

    sOptSetOP = (qlnoSetOP *) aOptNode->mOptNode;
    
    sExecNode    = QLL_GET_EXECUTION_NODE( aDataArea, aOptNode->mIdx );
    sExeSetOP = (qlnxSetOP *) sExecNode->mExecNode;

    /*****************************************
     * 하위 노드 Execute
     ****************************************/

    for ( i = 0; i < sOptSetOP->mChildCount; i++ )
    {
        QLNX_EXECUTE_NODE( aTransID,
                           aStmt,
                           aDBCStmt,
                           aSQLStmt,
                           aDataArea->mExecNodeList[sOptSetOP->mChildNodeArray[i]->mIdx].mOptNode,
                           aDataArea,
                           aEnv );
    }
    
    /*****************************************
     * Subquery Type 유형에 따른 초기화 
     ****************************************/

    if ( sOptSetOP->mNeedRebuild == STL_FALSE )
    {
        /**
         * Type N 형인 경우
         * - outer column reference 가 없는 경우 
         */

        /**
         * Instant Table 의 저장 정보를 그대로 둔다
         */
    }
    else
    {
        /**
         * Type J 형인 경우
         * - outer column reference 가 있는 경우
         */

        /**
         * @todo Instant Table 을 Truncate 한다.
         */

        /**
         * Instant Table 의 저장 정보를 초기화한다.
         */
        
        sExeSetOP->mInstantTable.mIsNeedBuild = STL_TRUE;
        sExeSetOP->mInstantTable.mRecCnt = 0;
    }

    /**
     * 공통 초기화
     */
    
    sExeSetOP->mInstantTable.mCurIdx = 0;
    
    if( sExeSetOP->mInstantTable.mIterator != NULL )
    {
        STL_TRY( smlResetIterator( sExeSetOP->mInstantTable.mIterator,
                                   SML_ENV( aEnv ) )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
   
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief UNION DISTINCT node 를 Fetch 한다.
 * @param[in]      aFetchNodeInfo  Fetch Node Info
 * @param[out]     aUsedBlockCnt   Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF            End Of Fetch 여부
 * @param[in]      aEnv            Environment
 * @remark 
 */
stlStatus qlnxFetchUnionDistinct( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                  stlInt64              * aUsedBlockCnt,
                                  stlBool               * aEOF,
                                  qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode      = NULL;
    qlnoSetOP             * sOptSetOP      = NULL;
    qlnxSetOP             * sExeSetOP      = NULL;

    qlnInstantTable       * sInstantTable  = NULL;
    
    stlInt64                sSkipCnt       = 0;
    stlInt64                sFetchCnt      = 0;
    stlInt64                sBuildRowCnt   = 0;

    stlBool                 sEOF           = STL_FALSE;
    stlBool                 sChildEOF      = STL_FALSE;

    knlValueBlockList     * sMyList        = NULL;
    knlValueBlockList     * sChildList     = NULL;
    qllExecutionNode      * sChildNode     = NULL;
    stlInt32                sChildNodeIdx  = 0;

    stlInt32                i              = 0;

    stlInt64                sFetchedCnt    = 0;
    stlInt64                sSkippedCnt    = 0;
    qlnxFetchNodeInfo       sLocalFetchInfo;
    
    QLL_OPT_DECLARE( stlTime sBeginFetchTime );
    QLL_OPT_DECLARE( stlTime sEndFetchTime );
    QLL_OPT_DECLARE( stlTime sFetchTime );

    QLL_OPT_DECLARE( stlTime sBeginExceptTime );
    QLL_OPT_DECLARE( stlTime sEndExceptTime );
    QLL_OPT_DECLARE( stlTime sExceptTime );

    QLL_OPT_SET_VALUE( sFetchTime, 0 );
    QLL_OPT_SET_VALUE( sExceptTime, 0 );


    /**
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aFetchNodeInfo->mOptNode->mType == QLL_PLAN_NODE_UNION_DISTINCT_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    QLL_OPT_CHECK_TIME( sBeginFetchTime );

    /*****************************************
     * 기본 정보 획득
     ****************************************/

    sOptSetOP = (qlnoSetOP *) aFetchNodeInfo->mOptNode->mOptNode;
    
    sExecNode    = QLL_GET_EXECUTION_NODE( aFetchNodeInfo->mDataArea,
                                           aFetchNodeInfo->mOptNode->mIdx );
    sExeSetOP = (qlnxSetOP *) sExecNode->mExecNode;
    sInstantTable = & sExeSetOP->mInstantTable;

    /**
     * Node 수행 여부 확인 
     */

    STL_DASSERT( *aEOF == STL_FALSE );
    STL_DASSERT( aFetchNodeInfo->mFetchCnt > 0);
    

    /**
     * Local Fetch Info 초기화
     */
    
    QLNX_SET_LOCAL_FETCH_INFO( & sLocalFetchInfo, aFetchNodeInfo );


    /**
     * OFFSET .. FETCH 값 조정
     */

    sSkipCnt  = aFetchNodeInfo->mSkipCnt;
    sFetchCnt = aFetchNodeInfo->mFetchCnt;

    /*****************************************
     * Build Phase
     ****************************************/

    if ( sInstantTable->mIsNeedBuild == STL_TRUE )
    {
        /**
         * 저장할 건수 계산
         */
        
        sBuildRowCnt = sSkipCnt + sFetchCnt;

        if ( (sSkipCnt >= 0) && (sFetchCnt >= 0) && (sBuildRowCnt < 0) )
        {
            /* overflow 가 발생한 경우 */
            sBuildRowCnt = QLL_FETCH_COUNT_MAX;
        }
        
        /**
         * Child 개수만큼 반복 수행하면서 저장
         */

        for( i = 0; i < sOptSetOP->mChildCount; i++ )
        {
            /**
             * Child Fetch 를 위한 Value Block 대체
             */

            sChildNodeIdx = sOptSetOP->mChildNodeArray[i]->mIdx;
            sChildNode = QLL_GET_EXECUTION_NODE( sLocalFetchInfo.mDataArea, sChildNodeIdx );

            sMyList = sExeSetOP->mColumnBlock;
            sChildList = ((qlnxCommonInfo *)sChildNode->mExecNode)->mResultColBlock;
            
            while (sChildList != NULL)
            {
                if ( sMyList->mValueBlock != sChildList->mValueBlock )
                {
                    stlMemcpy( sMyList->mValueBlock, sChildList->mValueBlock, STL_SIZEOF(knlValueBlock) );
                }
                else
                {
                    /* nothing to do */
                }
                
                sChildList = sChildList->mNext;
                sMyList = sMyList->mNext;
            }
            
            STL_DASSERT( sChildList == NULL );
            STL_DASSERT( sMyList == NULL );

            /**
             * Child Data 를 Hash Instance 에 저장
             */
            
            sChildEOF = STL_FALSE;
            while ( sChildEOF == STL_FALSE )
            {
                QLL_OPT_CHECK_TIME( sBeginExceptTime );
                
                /**
                 * Fetch Child
                 */
                
                QLNX_FETCH_NODE( & sLocalFetchInfo,
                                 sChildNodeIdx,
                                 0, /* Start Idx */
                                 0,   /* aSkipCnt */
                                 QLL_FETCH_COUNT_MAX,   
                                 & sFetchedCnt,
                                 &sChildEOF,
                                 aEnv );
        
                QLL_OPT_CHECK_TIME( sEndExceptTime );
                QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                /**
                 * Store Data
                 */

                if ( sFetchedCnt > 0 )
                {
                    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sExeSetOP->mColumnBlock, 0, sFetchedCnt );
                    
                    STL_TRY( smlInsertRecord( sLocalFetchInfo.mStmt,
                                              sInstantTable->mTableHandle,
                                              sExeSetOP->mColumnBlock,
                                              NULL, /* unique violation */
                                              & sExeSetOP->mHashRowBlock,
                                              SML_ENV( aEnv ) )
                             == STL_SUCCESS );
                    
                    /**
                     * Build Row Count 보다 많이 저장되어 있는지 검사 
                     */
                    
                    STL_TRY( smlGetTotalRecordCount( sInstantTable->mTableHandle,
                                                     & sInstantTable->mRecCnt,
                                                     SML_ENV(aEnv) )
                             == STL_SUCCESS );

                    STL_TRY_THROW( sBuildRowCnt > sInstantTable->mRecCnt, RAMP_BUILD_FINISH );
                }
                else
                {
                    STL_DASSERT( sChildEOF == STL_TRUE );
                }
            }
        }

        /****************************************
         * Fetch Phase 를 위한 준비
         ****************************************/

        STL_RAMP( RAMP_BUILD_FINISH );
        
        /**
         * Set Column 을 원복
         */
        
        STL_TRY( qlnxRevertQuerySetTarget( sLocalFetchInfo.mDataArea, sExecNode->mOptNode, aEnv )
                 == STL_SUCCESS );

        /**
         * Iterator 생성
         */

        if( sInstantTable->mIterator == NULL )
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
            STL_TRY( smlResetIterator( sInstantTable->mIterator,
                                       SML_ENV( aEnv ) )
                     == STL_SUCCESS );
        }

        /**
         * Build Phase 종료
         */
        
        sInstantTable->mIsNeedBuild = STL_FALSE;
    }
    else
    {
        /**
         * nothing to do
         */
    }
    
    /*****************************************
     * Fetch Phase
     ****************************************/

    STL_DASSERT( sInstantTable->mIterator != NULL );

    /**
     * fetch 수행
     */

    if( sInstantTable->mRecCnt <= sInstantTable->mCurIdx + sSkipCnt )
    {
        KNL_SET_ALL_BLOCK_USED_CNT( sExeSetOP->mColumnBlock, 0 );

        sFetchedCnt = 0;
        sSkippedCnt = sInstantTable->mRecCnt - sInstantTable->mCurIdx;

        sEOF = STL_TRUE;
    }
    else
    {
        /**
         * Skip Count 와 Fetch Count 설정
         */
        
        sExeSetOP->mFetchInfo.mSkipCnt  = sSkipCnt;
        sExeSetOP->mFetchInfo.mFetchCnt = sFetchCnt;
        
        STL_TRY( smlFetchNext( sInstantTable->mIterator,
                               & sExeSetOP->mFetchInfo,
                               SML_ENV(aEnv) )
                 == STL_SUCCESS );
        
        sFetchedCnt = SML_USED_BLOCK_SIZE( sExeSetOP->mFetchInfo.mRowBlock );
        sSkippedCnt = aFetchNodeInfo->mSkipCnt - sExeSetOP->mFetchInfo.mSkipCnt;

        sEOF = sExeSetOP->mFetchInfo.mIsEndOfScan;

        sInstantTable->mCurIdx += (sFetchedCnt + sSkipCnt);
    }


    /**
     * OUTPUT 설정
     */

    *aUsedBlockCnt = sFetchedCnt;
    
    /* Parent의 Fetch Node Info 설정 */
    aFetchNodeInfo->mFetchCnt -= sFetchedCnt;
    aFetchNodeInfo->mSkipCnt  -= sSkippedCnt;

    *aEOF = sEOF;

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sExeSetOP->mColumnBlock, 0, sFetchedCnt );

    
    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeSetOP->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeSetOP->mCommonInfo.mFetchCallCount, 1 );
    QLL_OPT_ADD_RECORD_STAT_INFO( sExeSetOP->mCommonInfo.mFetchRecordStat,
                                  sExeSetOP->mCommonInfo.mResultColBlock,
                                  *aUsedBlockCnt,
                                  aEnv );

    sExeSetOP->mCommonInfo.mFetchRowCnt += *aUsedBlockCnt;
    
    return STL_SUCCESS;
   
    STL_FINISH;

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeSetOP->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeSetOP->mCommonInfo.mFetchCallCount, 1 );

    return STL_FAILURE;
}


/**
 * @brief UNION DISTINCT node 수행을 종료한다.
 * @param[in] aOptNode      Optimization Node
 * @param[in] aDataArea     Data Area 
 * @param[in] aEnv          Environment
 * @remarks
 */
stlStatus qlnxFinalizeUnionDistinct( qllOptimizationNode   * aOptNode,
                                     qllDataArea           * aDataArea,
                                     qllEnv                * aEnv )
{
    qllExecutionNode   * sExecNode  = NULL;
    /* qlnoSetOP          * sOptSetOP  = NULL; */
    qlnxSetOP          * sExeSetOP  = NULL;

    /**
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_UNION_DISTINCT_TYPE, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );

    /*****************************************
     * 기본 정보 획득
     ****************************************/

    /* sOptSetOP = (qlnoSetOP *) aOptNode->mOptNode; */
    
    sExecNode    = QLL_GET_EXECUTION_NODE( aDataArea, aOptNode->mIdx );
    sExeSetOP = (qlnxSetOP *) sExecNode->mExecNode;

    /*****************************************
     * Instant Table 제거
     ****************************************/

    STL_TRY( qlnxDropInstantTable( & sExeSetOP->mInstantTable,
                                   aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
        
    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlnx */

