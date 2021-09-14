/*******************************************************************************
 * qlnxIntersect.c
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
 * @file qlnxIntersect.c
 * @brief SQL Processor Layer Execution Node - INTERSECT ( ALL / DISTINCT )
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnx
 * @{
 */

/**
 * @brief INTERSECT ( ALL / DISTINCT ) node 를 초기화한다.
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aStmt         Statement
 * @param[in]  aDBCStmt      DBC Statement
 * @param[in]  aSQLStmt      SQL Statement
 * @param[in]  aOptNode      Optimization Node
 * @param[in]  aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]  aEnv          Environment
 * @remarks 
 */
stlStatus qlnxInitializeIntersect( smlTransId              aTransID,
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
    STL_PARAM_VALIDATE( (aOptNode->mType == QLL_PLAN_NODE_INTERSECT_ALL_TYPE) ||
                        (aOptNode->mType == QLL_PLAN_NODE_INTERSECT_DISTINCT_TYPE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );

    /*****************************************
     * 기본 정보 획득
     ****************************************/

    sOptSetOP = (qlnoSetOP *) aOptNode->mOptNode;
    
    sExecNode    = QLL_GET_EXECUTION_NODE( aDataArea, aOptNode->mIdx );
    sExeSetOP = (qlnxSetOP *) sExecNode->mExecNode;

    /*****************************************
     * Instant Table A 생성
     ****************************************/

    /**
     * Instant Table 정보 초기화 
     */

    sExeSetOP->mInstantA.mIsNeedBuild = STL_TRUE;
    sExeSetOP->mInstantA.mRecCnt = 0;
    sExeSetOP->mInstantA.mCurIdx = 0;

    sExeSetOP->mInstantA.mStmt = aStmt;
    sExeSetOP->mInstantA.mTableHandle = NULL;
    sExeSetOP->mInstantA.mIndexHandle = NULL;
    sExeSetOP->mInstantA.mIterator = NULL;
    SML_INIT_ITERATOR_PROP( sExeSetOP->mInstantA.mIteratorProperty );


    sExeSetOP->mInstantB.mIsNeedBuild = STL_TRUE;
    sExeSetOP->mInstantB.mRecCnt = 0;
    sExeSetOP->mInstantB.mCurIdx = 0;

    sExeSetOP->mInstantB.mStmt = aStmt;
    sExeSetOP->mInstantB.mTableHandle = NULL;
    sExeSetOP->mInstantB.mIndexHandle = NULL;
    sExeSetOP->mInstantB.mIterator = NULL;
    SML_INIT_ITERATOR_PROP( sExeSetOP->mInstantB.mIteratorProperty );
    
    /**
     * Index Attribute 설정
     */
    
    stlMemset( & sIndexAttr, 0x00, STL_SIZEOF(smlIndexAttr) );

    sIndexAttr.mValidFlags |= SML_INDEX_LOGGING_YES;
    sIndexAttr.mLoggingFlag = STL_FALSE;

    if ( aOptNode->mType == QLL_PLAN_NODE_INTERSECT_DISTINCT_TYPE )
    {
        sIndexAttr.mValidFlags |= SML_INDEX_UNIQUENESS_YES;
        sIndexAttr.mUniquenessFlag = STL_TRUE;
    }
    else
    {
        sIndexAttr.mValidFlags |= SML_INDEX_UNIQUENESS_YES;
        sIndexAttr.mUniquenessFlag = STL_FALSE;
    }

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
                               & sExeSetOP->mInstantA.mTableHandle,
                               SML_ENV( aEnv ) )
             == STL_SUCCESS );

    STL_TRY( smlCreateIoTable( aStmt,
                               ellGetTablespaceID( & sSpaceHandle ),
                               SML_MEMORY_INSTANT_HASH_TABLE,
                               NULL,  /* aTableAttr */
                               & sIndexAttr,
                               sExeSetOP->mColumnBlock,
                               sOptSetOP->mRootQuerySet->mTargetCount,
                               NULL,  /* aKeyColFlags */
                               & sTablePhyID,
                               & sExeSetOP->mInstantB.mTableHandle,
                               SML_ENV( aEnv ) )
             == STL_SUCCESS );

    /**
     * Source / Destination 초기화
     */

    sExeSetOP->mDstInstant = & sExeSetOP->mInstantA;
    sExeSetOP->mSrcInstant = & sExeSetOP->mInstantB;
    
    return STL_SUCCESS;
   
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief INTERSECT ( ALL / DISTINCT ) node 를 수행한다.
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aStmt         Statement
 * @param[in]  aDBCStmt      DBC Statement
 * @param[in]  aSQLStmt      SQL Statement
 * @param[in]  aOptNode      Optimization Node
 * @param[in]  aDataArea     Data Area
 * @param[in]  aEnv          Environment
 * @remarks 
 */
stlStatus qlnxExecuteIntersect( smlTransId              aTransID,
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
    STL_PARAM_VALIDATE( (aOptNode->mType == QLL_PLAN_NODE_INTERSECT_ALL_TYPE) ||
                        (aOptNode->mType == QLL_PLAN_NODE_INTERSECT_DISTINCT_TYPE),
                        QLL_ERROR_STACK(aEnv) );
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
         * Instant Table 을 Truncate 한다.
         */

        STL_TRY( qlnxTruncateInstantTable( aStmt,
                                           & sExeSetOP->mInstantA,
                                           aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlnxTruncateInstantTable( aStmt,
                                           & sExeSetOP->mInstantB,
                                           aEnv )
                 == STL_SUCCESS );
    }

    /**
     * 공통 초기화
     */
    
    sExeSetOP->mInstantA.mCurIdx = 0;
    sExeSetOP->mInstantB.mCurIdx = 0;
    
    if( sExeSetOP->mInstantA.mIterator != NULL )
    {
        STL_TRY( smlResetIterator( sExeSetOP->mInstantA.mIterator,
                                   SML_ENV( aEnv ) )
                 == STL_SUCCESS );
    }

    if( sExeSetOP->mInstantB.mIterator != NULL )
    {
        STL_TRY( smlResetIterator( sExeSetOP->mInstantB.mIterator,
                                   SML_ENV( aEnv ) )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
   
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief INTERSECT ( ALL / DISTINCT ) node 를 Fetch 한다.
 * @param[in]      aFetchNodeInfo  Fetch Node Info
 * @param[out]     aUsedBlockCnt   Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF            End Of Fetch 여부
 * @param[in]      aEnv            Environment
 * @remark 
 */
stlStatus qlnxFetchIntersect( qlnxFetchNodeInfo     * aFetchNodeInfo,
                              stlInt64              * aUsedBlockCnt,
                              stlBool               * aEOF,
                              qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode      = NULL;
    qlnoSetOP             * sOptSetOP      = NULL;
    qlnxSetOP             * sExeSetOP      = NULL;

    stlInt64                sSkipCnt       = 0;
    stlInt64                sFetchCnt      = 0;

    stlBool                 sEOF           = STL_FALSE;
    stlBool                 sChildEOF      = STL_FALSE;

    knlValueBlockList     * sMyList        = NULL;
    knlValueBlockList     * sChildList     = NULL;
    qllExecutionNode      * sChildNode     = NULL;
    stlInt32                sChildNodeIdx  = 0;

    stlInt64                sUsedBlockCnt  = 0;

    stlBool                 sVersionConflict = STL_FALSE;
    stlBool                 sSkipped       = STL_FALSE;
    
    stlInt32                i              = 0;
    stlInt32                sBlockIdx      = 0;

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

    STL_PARAM_VALIDATE( (aFetchNodeInfo->mOptNode->mType == QLL_PLAN_NODE_INTERSECT_ALL_TYPE) ||
                        (aFetchNodeInfo->mOptNode->mType == QLL_PLAN_NODE_INTERSECT_DISTINCT_TYPE),
                        QLL_ERROR_STACK(aEnv) );


    QLL_OPT_CHECK_TIME( sBeginFetchTime );

    /*****************************************
     * 기본 정보 획득
     ****************************************/

    sOptSetOP = (qlnoSetOP *) aFetchNodeInfo->mOptNode->mOptNode;
    
    sExecNode    = QLL_GET_EXECUTION_NODE( aFetchNodeInfo->mDataArea,
                                           aFetchNodeInfo->mOptNode->mIdx );
    sExeSetOP = (qlnxSetOP *) sExecNode->mExecNode;

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

    if ( sExeSetOP->mDstInstant->mIsNeedBuild == STL_TRUE )
    {
        /*******************************************************
         * Left-most Child 를 저장
         *******************************************************/

        /**
         * Left-most Child Fetch 를 위한 Value Block 대체
         */
        
        sChildNodeIdx = sOptSetOP->mChildNodeArray[0]->mIdx;
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

        sExeSetOP->mDstInstant = & sExeSetOP->mInstantA;
        
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
                             &sUsedBlockCnt,
                             &sChildEOF,
                             aEnv );
            
            QLL_OPT_CHECK_TIME( sEndExceptTime );
            QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );
            
            /**
             * Store Data
             */
            
            if ( sUsedBlockCnt > 0 )
            {
                KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sExeSetOP->mColumnBlock, 0, sUsedBlockCnt );
                
                STL_TRY( smlInsertRecord( sLocalFetchInfo.mStmt,
                                          sExeSetOP->mDstInstant->mTableHandle,
                                          sExeSetOP->mColumnBlock,
                                          NULL, /* unique violation */
                                          & sExeSetOP->mHashRowBlock,
                                          SML_ENV( aEnv ) )
                         == STL_SUCCESS );
            }
            else
            {
                STL_DASSERT( sChildEOF == STL_TRUE );
            }
        }

        /**
         * Destine Instant 에 Skip Count 보다 많이 저장되어 있는지 검사 
         */
        
        STL_TRY( smlGetTotalRecordCount( sExeSetOP->mDstInstant->mTableHandle,
                                         & sExeSetOP->mDstInstant->mRecCnt,
                                         SML_ENV(aEnv) )
                 == STL_SUCCESS );
        
        STL_TRY_THROW ( sExeSetOP->mDstInstant->mRecCnt > sSkipCnt, RAMP_EXIT );
        
        /*******************************************************
         * Other Child 를 반복 수행하며 Source 에서 Destine Instant Table 로 이동
         *******************************************************/
        
        for( i = 1; i < sOptSetOP->mChildCount; i++ )
        {
            /*******************************************************
             * Source Instant 와 Destine Instant 조정
             *******************************************************/
            
            /**
             * Source 와 Desitin 선택
             */

            if ( (i % 2) == 0 )
            {
                sExeSetOP->mDstInstant = & sExeSetOP->mInstantA;
                sExeSetOP->mSrcInstant = & sExeSetOP->mInstantB;
            }
            else
            {
                sExeSetOP->mSrcInstant = & sExeSetOP->mInstantA;
                sExeSetOP->mDstInstant = & sExeSetOP->mInstantB;
            }

            /**
             * Destine Instant 를 Truncate 
             */

            if ( i > 1 )
            {
                STL_TRY( qlnxTruncateInstantTable( sLocalFetchInfo.mStmt,
                                                   sExeSetOP->mDstInstant,
                                                   aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                /* nothing to do */
            }

            /*******************************************************
             * Child 에 대한 INTERSECT 처리
             *******************************************************/
            
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
             * Child Data 가 존재할 때까지 Fetch 
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
                                 &sUsedBlockCnt,
                                 &sChildEOF,
                                 aEnv );
        
                QLL_OPT_CHECK_TIME( sEndExceptTime );
                QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

                /**
                 * Child Data 를 Fetch 하여 Source Instant 에서 Desine Instant 로 이동
                 */

                if ( sUsedBlockCnt > 0 )
                {
                    /**
                     * Delete Child Record
                     */

                    for ( sBlockIdx = 0; sBlockIdx < sUsedBlockCnt; sBlockIdx++ )
                    {
                        STL_TRY( smlDeleteRecord( sLocalFetchInfo.mStmt,
                                                  sExeSetOP->mSrcInstant->mTableHandle,
                                                  & sExeSetOP->mHashRowBlock.mRidBlock[ sBlockIdx ],
                                                  sExeSetOP->mHashRowBlock.mScnBlock[ sBlockIdx ],
                                                  sBlockIdx,
                                                  sExeSetOP->mColumnBlock,
                                                  & sVersionConflict,
                                                  & sSkipped,
                                                  SML_ENV(aEnv) )
                                 == STL_SUCCESS );

                        if ( sSkipped == STL_TRUE )
                        {
                            /**
                             * 존재하지 않는 Data 임
                             */
                        }
                        else
                        {
                            /**
                             * INTERSECT 한 Data 임 
                             */

                            KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sExeSetOP->mColumnBlock, sBlockIdx, sBlockIdx + 1);
                
                            STL_TRY( smlInsertRecord( sLocalFetchInfo.mStmt,
                                                      sExeSetOP->mDstInstant->mTableHandle,
                                                      sExeSetOP->mColumnBlock,
                                                      NULL, /* unique violation */
                                                      & sExeSetOP->mHashRowBlock,
                                                      SML_ENV( aEnv ) )
                                     == STL_SUCCESS );
                        }
                    }

                    /**
                     * Source Instant 가 비었다면 Child 를 더이상 수행할 필요가 없다.
                     */
                    
                    STL_TRY( smlGetTotalRecordCount( sExeSetOP->mSrcInstant->mTableHandle,
                                                     & sExeSetOP->mSrcInstant->mRecCnt,
                                                     SML_ENV(aEnv) )
                             == STL_SUCCESS );

                    if ( sExeSetOP->mSrcInstant->mRecCnt > 0 )
                    {
                        /* go go */
                    }
                    else
                    {
                        sChildEOF = STL_TRUE;
                    }
                }
                else
                {
                    STL_DASSERT( sChildEOF == STL_TRUE );
                }
            }

            /**
             * Destine Instant 에 Skip Count 보다 많이 저장되어 있는지 검사 
             */
            
            STL_TRY( smlGetTotalRecordCount( sExeSetOP->mDstInstant->mTableHandle,
                                             & sExeSetOP->mDstInstant->mRecCnt,
                                             SML_ENV(aEnv) )
                     == STL_SUCCESS );
            
            STL_TRY_THROW ( sExeSetOP->mDstInstant->mRecCnt > sSkipCnt, RAMP_EXIT );
            
        }

        /****************************************
         * Fetch Phase 를 위한 준비
         ****************************************/

        /**
         * Set Column 을 원복
         */
        
        STL_TRY( qlnxRevertQuerySetTarget( sLocalFetchInfo.mDataArea, sExecNode->mOptNode, aEnv )
                 == STL_SUCCESS );

        /**
         * Iterator 생성
         */

        if( sExeSetOP->mDstInstant->mIterator == NULL )
        {
            SML_INIT_ITERATOR_PROP( sExeSetOP->mDstInstant->mIteratorProperty );
            
            STL_TRY( smlAllocIterator( sLocalFetchInfo.mStmt,
                                       sExeSetOP->mDstInstant->mTableHandle,
                                       SML_TRM_SNAPSHOT,
                                       SML_SRM_SNAPSHOT,
                                       & sExeSetOP->mDstInstant->mIteratorProperty,
                                       SML_SCAN_FORWARD,
                                       & sExeSetOP->mDstInstant->mIterator,
                                       SML_ENV( aEnv ) )
                     == STL_SUCCESS );
        }
        else
        {
            STL_TRY( smlResetIterator( sExeSetOP->mDstInstant->mIterator,
                                       SML_ENV( aEnv ) )
                     == STL_SUCCESS );
        }

        /**
         * Build Phase 종료
         */
        
        sExeSetOP->mDstInstant->mIsNeedBuild = STL_FALSE;
        sExeSetOP->mSrcInstant->mIsNeedBuild = STL_FALSE;
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

    STL_DASSERT( sExeSetOP->mDstInstant->mIterator != NULL );

    STL_RAMP( RAMP_EXIT );
    
    /**
     * fetch 수행
     */

    if( sExeSetOP->mDstInstant->mRecCnt <= sExeSetOP->mDstInstant->mCurIdx + sSkipCnt )
    {
        KNL_SET_ALL_BLOCK_USED_CNT( sExeSetOP->mColumnBlock, 0 );

        sFetchedCnt = 0;
        sSkippedCnt = sExeSetOP->mDstInstant->mRecCnt - sExeSetOP->mDstInstant->mCurIdx;

        sEOF = STL_TRUE;
    }
    else
    {
        /**
         * Skip Count 와 Fetch Count 설정
         */
        
        sExeSetOP->mFetchInfo.mSkipCnt  = sSkipCnt;
        sExeSetOP->mFetchInfo.mFetchCnt = sFetchCnt;
        
        STL_TRY( smlFetchNext( sExeSetOP->mDstInstant->mIterator,
                               & sExeSetOP->mFetchInfo,
                               SML_ENV(aEnv) )
                 == STL_SUCCESS );
        
        sFetchedCnt = SML_USED_BLOCK_SIZE( sExeSetOP->mFetchInfo.mRowBlock );
        sSkippedCnt = aFetchNodeInfo->mSkipCnt - sExeSetOP->mFetchInfo.mSkipCnt;

        sEOF = sExeSetOP->mFetchInfo.mIsEndOfScan;

        sExeSetOP->mDstInstant->mCurIdx += (sFetchedCnt + sSkipCnt);
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
 * @brief INTERSECT ( ALL / DISTINCT ) node 수행을 종료한다.
 * @param[in] aOptNode      Optimization Node
 * @param[in] aDataArea     Data Area 
 * @param[in] aEnv          Environment
 * @remarks
 */
stlStatus qlnxFinalizeIntersect( qllOptimizationNode   * aOptNode,
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
    STL_PARAM_VALIDATE( (aOptNode->mType == QLL_PLAN_NODE_INTERSECT_ALL_TYPE) ||
                        (aOptNode->mType == QLL_PLAN_NODE_INTERSECT_DISTINCT_TYPE),
                        QLL_ERROR_STACK(aEnv) );
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

    STL_TRY( qlnxDropInstantTable( & sExeSetOP->mInstantA, aEnv ) == STL_SUCCESS );
    STL_TRY( qlnxDropInstantTable( & sExeSetOP->mInstantB, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
        
    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlnx */

