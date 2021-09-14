/*******************************************************************************
 * qlnxSubQueryList.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlnxSubQuery.c 8502 2013-05-20 02:42:55Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlnxSubQueryList.c
 * @brief SQL Processor Layer Execution Node - SUB QUERY LIST
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnx
 * @{
 */

/**
 * @brief SUB QUERY LIST node를 초기화한다.
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
stlStatus qlnxInitializeSubQueryList( smlTransId              aTransID,
                                      smlStatement          * aStmt,
                                      qllDBCStmt            * aDBCStmt,
                                      qllStatement          * aSQLStmt,
                                      qllOptimizationNode   * aOptNode,
                                      qllDataArea           * aDataArea,
                                      qllEnv                * aEnv )
{
    qllExecutionNode  * sExecNode        = NULL;
    /* qlnoSubQueryList  * sOptSubQueryList = NULL; */
    qlnxSubQueryList  * sExeSubQueryList = NULL;

    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_LIST_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * SUB-QUERY LIST Optimization Node 획득
     */

    /* sOptSubQueryList = (qlnoSubQueryList *) aOptNode->mOptNode; */


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea,
                                        QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * SUB-QUERY Execution Node 할당
     */

    sExeSubQueryList = (qlnxSubQueryList *) sExecNode->mExecNode;

    sExeSubQueryList->mIsFirstExec = STL_TRUE;

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SUB QUERY LIST node를 수행한다.
 * @param[in]      aTransID      Transaction ID
 * @param[in]      aStmt         Statement
 * @param[in]      aDBCStmt      DBC Statement
 * @param[in,out]  aSQLStmt      SQL Statement
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
_ * @param[in]      aEnv          Environment
 *
 * @remark Execution 첫번째 수행시 호출한다.
 */
stlStatus qlnxExecuteSubQueryList( smlTransId              aTransID,
                                   smlStatement          * aStmt,
                                   qllDBCStmt            * aDBCStmt,
                                   qllStatement          * aSQLStmt,
                                   qllOptimizationNode   * aOptNode,
                                   qllDataArea           * aDataArea,
                                   qllEnv                * aEnv )
{
    qllExecutionNode  * sExecNode        = NULL;
    qlnoSubQueryList  * sOptSubQueryList = NULL;
    qlnxSubQueryList  * sExeSubQueryList = NULL;
    
    knlValueBlockList * sTargetBlock     = NULL;
    knlValueBlockList * sRefColBlock     = NULL;
    qlvRefExprItem    * sExprItem        = NULL;
    
    stlInt32            sLoop            = 0;
    stlInt64            sReadCnt         = 0;
    stlInt16            sNodeIdx         = 0;
    stlBool             sEOF             = STL_FALSE;

    qlnxFetchNodeInfo   sFetchNodeInfo;
    
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_LIST_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /*****************************************
     * 기본 정보 획득
     ****************************************/

    /**
     * SUB-QUERY LIST Optimization Node 획득
     */

    sOptSubQueryList = (qlnoSubQueryList *) aOptNode->mOptNode;


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea,
                                        QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * SUB-QUERY LIST Execution Node 획득
     */

    sExeSubQueryList = sExecNode->mExecNode;


    /**
     * Sub Query 중 Iteration Time이 For Each Statement인 것만 수행한다.
     */

    if( sExeSubQueryList->mIsFirstExec == STL_TRUE )
    {
        /**
         * Fetch Node Info 설정
         */
    
        QLNX_INIT_FETCH_NODE_INFO( & sFetchNodeInfo,
                                   aTransID,
                                   aStmt,
                                   aDBCStmt,
                                   aSQLStmt,
                                   aDataArea );


        /**
         * Sub Query Node Fetch 수행 : Sub Query를 포함한 List Function은 제외
         */
        
        sExprItem = sOptSubQueryList->mSubQueryExprList->mHead;
        
        for( sLoop = 0 ; sLoop < sOptSubQueryList->mSubQueryNodeCnt ; sLoop++ )
        {
            if( sExprItem->mExprPtr->mType != QLV_EXPR_TYPE_SUB_QUERY )
            {
                sExprItem = sExprItem->mNext;
                continue;
            }

            if( sExprItem->mExprPtr->mIterationTime >= DTL_ITERATION_TIME_FOR_EACH_STATEMENT )
            {
                /* execution */
                if( sOptSubQueryList->mChildNode != NULL )
                {
                    QLNX_EXECUTE_NODE( aTransID,
                                       aStmt,
                                       aDBCStmt,
                                       aSQLStmt,
                                       sOptSubQueryList->mChildNode,
                                       aDataArea,
                                       aEnv );
                }
                
                /* Sub Query의 Execute */
                QLNX_EXECUTE_NODE( aTransID,
                                   aStmt,
                                   aDBCStmt,
                                   aSQLStmt,
                                   sOptSubQueryList->mSubQueryNodes[ sLoop ],
                                   aDataArea,
                                   aEnv );
        
                /* Sub Query의 Fetch */
                sEOF     = STL_FALSE;
                sNodeIdx = sOptSubQueryList->mSubQueryNodes[ sLoop ]->mIdx;
                
                if( aDataArea->mBlockAllocCnt == 1  )
                {
                    /* fetch records*/
                    QLNX_FETCH_NODE( & sFetchNodeInfo,
                                     sNodeIdx,
                                     0, /* Start Idx */
                                     0, /* Skip Count */
                                     1, /* Fetch Count */
                                     & sReadCnt,
                                     & sEOF,
                                     aEnv );

                    STL_TRY_THROW( ( ( sReadCnt == 0 ) && ( sEOF == STL_TRUE ) ) ||
                                   ( sReadCnt == 1 ),
                                   RAMP_ERR_SINGLE_ROW_SUBQUERY_RETURNS_MULTIPLE_ROWS );

                    sTargetBlock = sExeSubQueryList->mTargetBlockList[ sLoop ];
                    sRefColBlock = sExeSubQueryList->mRefColBlockList[ sLoop ];

                    if( sReadCnt == 0 )
                    {
                        while( sRefColBlock != NULL )
                        {
                            STL_DASSERT( KNL_GET_BLOCK_IS_SEP_BUFF( sRefColBlock ) == STL_FALSE );
                    
                            DTL_SET_NULL( KNL_GET_BLOCK_FIRST_DATA_VALUE( sRefColBlock ) );
                    
                            sRefColBlock = sRefColBlock->mNext;
                        }
                    }
                    else
                    {
                        while( sRefColBlock != NULL )
                        {
                            STL_DASSERT( KNL_GET_BLOCK_IS_SEP_BUFF( sRefColBlock ) == STL_FALSE );
                        
                            STL_TRY( knlCopyDataValue(
                                         KNL_GET_BLOCK_FIRST_DATA_VALUE( sTargetBlock ),
                                         KNL_GET_BLOCK_FIRST_DATA_VALUE( sRefColBlock ),
                                         KNL_ENV( aEnv ) )
                                     == STL_SUCCESS );

                            sTargetBlock = sTargetBlock->mNext;
                            sRefColBlock = sRefColBlock->mNext;
                        }
                    }

                    if( sEOF == STL_FALSE )
                    {            
                        /* fetch records*/
                        QLNX_FETCH_NODE( & sFetchNodeInfo,
                                         sNodeIdx,
                                         0, /* Start Idx */
                                         0, /* Skip Count */
                                         1, /* Fetch Count */
                                         & sReadCnt,
                                         & sEOF,
                                         aEnv );
                        
                        STL_TRY_THROW( sReadCnt == 0,
                                       RAMP_ERR_SINGLE_ROW_SUBQUERY_RETURNS_MULTIPLE_ROWS );
                    }
                }
                else
                {
                    STL_DASSERT( aDataArea->mBlockAllocCnt > 1 );

                    /* fetch records*/
                    QLNX_FETCH_NODE( & sFetchNodeInfo,
                                     sNodeIdx,
                                     0, /* Start Idx */
                                     0, /* Skip Count */
                                     2, /* Fetch Count */
                                     & sReadCnt,
                                     & sEOF,
                                     aEnv );

                    STL_TRY_THROW( sReadCnt <= 1,
                                   RAMP_ERR_SINGLE_ROW_SUBQUERY_RETURNS_MULTIPLE_ROWS );

                    sTargetBlock = sExeSubQueryList->mTargetBlockList[ sLoop ];
                    sRefColBlock = sExeSubQueryList->mRefColBlockList[ sLoop ];

                    if( sReadCnt == 0 )
                    {
                        while( sRefColBlock != NULL )
                        {
                            STL_DASSERT( KNL_GET_BLOCK_IS_SEP_BUFF( sRefColBlock ) == STL_FALSE );
                    
                            DTL_SET_NULL( KNL_GET_BLOCK_FIRST_DATA_VALUE( sRefColBlock ) );
                    
                            sRefColBlock = sRefColBlock->mNext;
                        }
                    }
                    else
                    {
                        while( sRefColBlock != NULL )
                        {
                            STL_DASSERT( KNL_GET_BLOCK_IS_SEP_BUFF( sRefColBlock ) == STL_FALSE );
                        
                            STL_TRY( knlCopyDataValue(
                                         KNL_GET_BLOCK_FIRST_DATA_VALUE( sTargetBlock ),
                                         KNL_GET_BLOCK_FIRST_DATA_VALUE( sRefColBlock ),
                                         KNL_ENV( aEnv ) )
                                     == STL_SUCCESS );

                            sTargetBlock = sTargetBlock->mNext;
                            sRefColBlock = sRefColBlock->mNext;
                        }
                    }

                    if( ( sReadCnt == 1 ) && ( sEOF == STL_FALSE ) )
                    {
                        /* fetch records*/
                        QLNX_FETCH_NODE( & sFetchNodeInfo,
                                         sNodeIdx,
                                         0, /* Start Idx */
                                         0, /* Skip Count */
                                         1, /* Fetch Count */
                                         & sReadCnt,
                                         & sEOF,
                                         aEnv );

                        STL_TRY_THROW( ( sReadCnt == 0 ) && ( sEOF == STL_TRUE ),
                                       RAMP_ERR_SINGLE_ROW_SUBQUERY_RETURNS_MULTIPLE_ROWS );
                    }
                }
            }

            sExprItem = sExprItem->mNext;
        }


        /**
         * Sub Query를 포함한 List Function 수행 : Block Fetch
         */
        
        sExprItem = sOptSubQueryList->mSubQueryExprList->mHead;
        
        for( sLoop = 0 ; sLoop < sOptSubQueryList->mSubQueryNodeCnt ; sLoop++ )
        {
            if( ( sExprItem->mExprPtr->mType != QLV_EXPR_TYPE_SUB_QUERY ) &&
                ( sExprItem->mExprPtr->mIterationTime >= DTL_ITERATION_TIME_FOR_EACH_STATEMENT ) )
            {
                /* execution */
                if( sOptSubQueryList->mChildNode != NULL )
                {
                    QLNX_EXECUTE_NODE( aTransID,
                                       aStmt,
                                       aDBCStmt,
                                       aSQLStmt,
                                       sOptSubQueryList->mChildNode,
                                       aDataArea,
                                       aEnv );
                }

                /* Sub Query의 Execute */
                QLNX_EXECUTE_NODE( aTransID,
                                   aStmt,
                                   aDBCStmt,
                                   aSQLStmt,
                                   sOptSubQueryList->mSubQueryNodes[ sLoop ],
                                   aDataArea,
                                   aEnv );
        
                /* Sub Query의 Fetch */
                sEOF     = STL_FALSE;
                sNodeIdx = sOptSubQueryList->mSubQueryNodes[ sLoop ]->mIdx;

                /* fetch records*/
                QLNX_FETCH_NODE( & sFetchNodeInfo,
                                 sNodeIdx,
                                 0, /* Start Idx */
                                 0, /* Skip Count */
                                 1, /* Fetch Count */
                                 & sReadCnt,
                                 & sEOF,
                                 aEnv );

                /* list function의 결과는 하위 노드에서 설정됨 */

                STL_DASSERT( sReadCnt == 1 );
            }
            else
            {
                /* Do Nothing */
            }

            sExprItem = sExprItem->mNext;
        }
    }

    
    /**
     * 하위 노드 Execute
     */

    if( sOptSubQueryList->mChildNode != NULL )
    {
        QLNX_EXECUTE_NODE( aTransID,
                           aStmt,
                           aDBCStmt,
                           aSQLStmt,
                           sOptSubQueryList->mChildNode,
                           aDataArea,
                           aEnv );
    }

    
    /**
     * Sub Query 중 Iteration Time이 For Each Statement이 아닌 것만 수행한다.
     */

    sExprItem = sOptSubQueryList->mSubQueryExprList->mHead;
    for( sLoop = 0 ; sLoop < sOptSubQueryList->mSubQueryNodeCnt ; sLoop++ )
    {
        if( sExprItem->mExprPtr->mIterationTime < DTL_ITERATION_TIME_FOR_EACH_STATEMENT )
        {
            /* Sub Query의 Execute */
            QLNX_EXECUTE_NODE( aTransID,
                               aStmt,
                               aDBCStmt,
                               aSQLStmt,
                               sOptSubQueryList->mSubQueryNodes[ sLoop ],
                               aDataArea,
                               aEnv );
        }

        sExprItem = sExprItem->mNext;
    }
    
    sExeSubQueryList->mIsFirstExec = STL_FALSE;
    
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_SINGLE_ROW_SUBQUERY_RETURNS_MULTIPLE_ROWS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SINGLE_ROW_SUBQUERY_RETURNS_MORE_THAN_ONE_ROW,
                      qlgMakeErrPosString( aSQLStmt->mRetrySQL,
                                           sExprItem->mExprPtr->mPosition,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SUB QUERY LIST node를 Fetch한다.
 * @param[in]      aFetchNodeInfo  Fetch Node Info
 * @param[out]     aUsedBlockCnt   Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF            End Of Fetch 여부
 * @param[in]      aEnv            Environment
 *
 * @remark Fetch 수행시 또는 반복되는 Execution 수행시 호출된다.
 */
stlStatus qlnxFetchSubQueryList( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                 stlInt64              * aUsedBlockCnt,
                                 stlBool               * aEOF,
                                 qllEnv                * aEnv )
{
    qllExecutionNode  * sExecNode        = NULL;
    qlnoSubQueryList  * sOptSubQueryList = NULL;
    qlnxSubQueryList  * sExeSubQueryList = NULL;

    knlValueBlockList * sTargetBlock     = NULL;
    knlValueBlockList * sRefColBlock     = NULL;
    qlvRefExprItem    * sExprItem        = NULL;
    
    stlInt32            sCurIdx          = 0;
    stlInt32            sStartIdx        = 0;
    stlInt32            sLoop            = 0;

    stlInt64            sReadCnt         = 0;
    stlInt16            sNodeIdx         = 0;
    stlBool             sIsSetOuterCol   = STL_FALSE;
    stlBool             sEOF             = STL_FALSE;

    stlInt64            sFetchedCnt       = 0;
    stlInt64            sSkippedCnt       = 0;
    qlnxFetchNodeInfo   sLocalFetchInfo;

    
    QLL_OPT_DECLARE( stlTime sBeginFetchTime );
    QLL_OPT_DECLARE( stlTime sEndFetchTime );
    QLL_OPT_DECLARE( stlTime sFetchTime );

    QLL_OPT_DECLARE( stlTime sBeginExceptTime );
    QLL_OPT_DECLARE( stlTime sEndExceptTime );
    QLL_OPT_DECLARE( stlTime sExceptTime );

    QLL_OPT_SET_VALUE( sFetchTime, 0 );
    QLL_OPT_SET_VALUE( sExceptTime, 0 );

    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aFetchNodeInfo->mOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_LIST_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    QLL_OPT_CHECK_TIME( sBeginFetchTime );
    
    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * SUB-QUERY LIST Optimization Node 획득
     */

    sOptSubQueryList = (qlnoSubQueryList *) aFetchNodeInfo->mOptNode->mOptNode;

    
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aFetchNodeInfo->mDataArea,
                                        QLL_GET_OPT_NODE_IDX( aFetchNodeInfo->mOptNode ) );


    /**
     * SUB-QUERY Execution Node 획득
     */

    sExeSubQueryList = sExecNode->mExecNode;


    /**
     * Node 수행 여부 확인
     */

    STL_DASSERT( *aEOF == STL_FALSE );
    STL_DASSERT( aFetchNodeInfo->mFetchCnt > 0);

    sStartIdx = aFetchNodeInfo->mStartIdx;


    /**
     * Local Fetch Info 초기화
     */
    
    QLNX_SET_LOCAL_FETCH_INFO( & sLocalFetchInfo, aFetchNodeInfo );


    /*****************************************
     * FETCH
     ****************************************/

    /**
     * Child Node Fetch 수행
     */
    
    QLL_OPT_CHECK_TIME( sBeginExceptTime );

    /* Fetch Next */
    if( sOptSubQueryList->mChildNode != NULL )
    {
        QLNX_FETCH_NODE( & sLocalFetchInfo,
                         sOptSubQueryList->mChildNode->mIdx,
                         sLocalFetchInfo.mStartIdx, /* Start Idx */
                         aFetchNodeInfo->mSkipCnt,
                         aFetchNodeInfo->mFetchCnt,
                         & sFetchedCnt,
                         aEOF,
                         aEnv );

        sSkippedCnt = aFetchNodeInfo->mSkipCnt - sLocalFetchInfo.mSkipCnt;
    }
    else
    {
        *aEOF = STL_TRUE;
        sFetchedCnt = 1;
    }

    STL_TRY_THROW( sFetchedCnt > 0, RAMP_EXIT );


    /**
     * Sub Query Node Fetch 수행 : Sub Query를 포함한 List Function은 제외
     */

    for( sCurIdx = 0 ; sCurIdx < sFetchedCnt ; sCurIdx++ )
    {
        sIsSetOuterCol = STL_FALSE;
        
        /**
         * Sub Query Node Fetch
         */

        sExprItem = sOptSubQueryList->mSubQueryExprList->mHead;
        
        for( sLoop = 0 ; sLoop < sOptSubQueryList->mSubQueryNodeCnt ; sLoop++ )
        {
            if( sExprItem->mExprPtr->mType != QLV_EXPR_TYPE_SUB_QUERY )
            {
                sExprItem = sExprItem->mNext;
                continue;
            }
            
            /* Set Outer Column Value */
            if( ( sIsSetOuterCol == STL_FALSE ) &&
                ( sOptSubQueryList->mChildNode != NULL ) )
            {
                STL_TRY( qlndSetOuterColumnValue( sLocalFetchInfo.mDataArea,
                                                  sOptSubQueryList->mChildNode->mIdx,
                                                  sCurIdx,
                                                  aEnv )
                         == STL_SUCCESS );
                
                sIsSetOuterCol = STL_TRUE;
            }
            
            if( sExprItem->mExprPtr->mIterationTime ==
                DTL_ITERATION_TIME_FOR_EACH_QUERY )
            {
                sEOF     = STL_FALSE;
                sNodeIdx = sOptSubQueryList->mSubQueryNodes[ sLoop ]->mIdx;

                if( sLocalFetchInfo.mDataArea->mBlockAllocCnt == 1  )
                {
                    /* execution */
                    QLNX_EXECUTE_NODE( sLocalFetchInfo.mTransID,
                                       sLocalFetchInfo.mStmt,
                                       sLocalFetchInfo.mDBCStmt,
                                       sLocalFetchInfo.mSQLStmt,
                                       sOptSubQueryList->mSubQueryNodes[ sLoop ],
                                       sLocalFetchInfo.mDataArea,
                                       aEnv );

                    /* fetch records*/
                    QLNX_FETCH_NODE( & sLocalFetchInfo,
                                     sNodeIdx,
                                     sStartIdx, /* Start Idx */
                                     0, /* Skip Count */
                                     1, /* Fetch Count */
                                     & sReadCnt,
                                     & sEOF,
                                     aEnv );

                    sTargetBlock = sExeSubQueryList->mTargetBlockList[ sLoop ];
                    sRefColBlock = sExeSubQueryList->mRefColBlockList[ sLoop ];
            
                    if( sReadCnt == 0 )
                    {
                        while( sRefColBlock != NULL )
                        {
                            DTL_SET_NULL( KNL_GET_BLOCK_SEP_DATA_VALUE( sRefColBlock, sCurIdx ) );
                    
                            sRefColBlock = sRefColBlock->mNext;
                        }
                    }
                    else
                    {
                        while( sRefColBlock != NULL )
                        {
                            STL_TRY( knlCopyDataValue(
                                         KNL_GET_BLOCK_FIRST_DATA_VALUE( sTargetBlock ),
                                         KNL_GET_BLOCK_DATA_VALUE( sRefColBlock, sCurIdx ),
                                         KNL_ENV(aEnv) )
                                     == STL_SUCCESS );

                            sTargetBlock = sTargetBlock->mNext;
                            sRefColBlock = sRefColBlock->mNext;
                        }
                    }
                    
                    if( ( sReadCnt > 0 ) && ( sEOF == STL_FALSE ) )
                    {
                        QLNX_FETCH_NODE( & sLocalFetchInfo,
                                         sNodeIdx,
                                         sStartIdx, /* Start Idx */
                                         0, /* Skip Count */
                                         1, /* Fetch Count */
                                         & sReadCnt,
                                         & sEOF,
                                         aEnv );

                        STL_TRY_THROW( sReadCnt == 0,
                                       RAMP_ERR_SINGLE_ROW_SUBQUERY_RETURNS_MULTIPLE_ROWS );
                    }
                }
                else
                {
                    STL_DASSERT( sLocalFetchInfo.mDataArea->mBlockAllocCnt > 1 );
            
                    /* execution */
                    QLNX_EXECUTE_NODE( sLocalFetchInfo.mTransID,
                                       sLocalFetchInfo.mStmt,
                                       sLocalFetchInfo.mDBCStmt,
                                       sLocalFetchInfo.mSQLStmt,
                                       sOptSubQueryList->mSubQueryNodes[ sLoop ],
                                       sLocalFetchInfo.mDataArea,
                                       aEnv );

                    /* fetch records*/
                    QLNX_FETCH_NODE( & sLocalFetchInfo,
                                     sNodeIdx,
                                     sStartIdx, /* Start Idx */
                                     0, /* Skip Count */
                                     2, /* Fetch Count */
                                     & sReadCnt,
                                     & sEOF,
                                     aEnv );

                    STL_TRY_THROW( sReadCnt <= 1,
                                   RAMP_ERR_SINGLE_ROW_SUBQUERY_RETURNS_MULTIPLE_ROWS );

                    sTargetBlock = sExeSubQueryList->mTargetBlockList[ sLoop ];
                    sRefColBlock = sExeSubQueryList->mRefColBlockList[ sLoop ];
            
                    if( sReadCnt == 0 )
                    {
                        while( sRefColBlock != NULL )
                        {
                            DTL_SET_NULL( KNL_GET_BLOCK_SEP_DATA_VALUE( sRefColBlock, sCurIdx ) );
                    
                            sRefColBlock = sRefColBlock->mNext;
                        }
                    }
                    else
                    {
                        while( sRefColBlock != NULL )
                        {
                            STL_TRY( knlCopyDataValue(
                                         KNL_GET_BLOCK_FIRST_DATA_VALUE( sTargetBlock ),
                                         KNL_GET_BLOCK_DATA_VALUE( sRefColBlock, sCurIdx ),
                                         KNL_ENV(aEnv) )
                                     == STL_SUCCESS );

                            sTargetBlock = sTargetBlock->mNext;
                            sRefColBlock = sRefColBlock->mNext;
                        }
                    }
                    
                    if( ( sReadCnt == 1 ) && ( sEOF == STL_FALSE ) )
                    {
                        QLNX_FETCH_NODE( & sLocalFetchInfo,
                                         sNodeIdx,
                                         sStartIdx, /* Start Idx */
                                         0, /* Skip Count */
                                         1, /* Fetch Count */
                                         & sReadCnt,
                                         & sEOF,
                                         aEnv );

                        STL_TRY_THROW( sReadCnt == 0,
                                       RAMP_ERR_SINGLE_ROW_SUBQUERY_RETURNS_MULTIPLE_ROWS );
                    }
                }
            }
            else
            {
                /* Do Nothing */
            }
            
            sExprItem = sExprItem->mNext;
        }
    }

    
    /**
     * Sub Query를 포함한 List Function 수행 : Block Fetch
     */

    sExprItem = sOptSubQueryList->mSubQueryExprList->mHead;
        
    for( sLoop = 0 ; sLoop < sOptSubQueryList->mSubQueryNodeCnt ; sLoop++ )
    {
        if( ( sExprItem->mExprPtr->mType != QLV_EXPR_TYPE_SUB_QUERY ) &&
            ( sExprItem->mExprPtr->mIterationTime == DTL_ITERATION_TIME_FOR_EACH_QUERY ) )
        {
            sEOF     = STL_FALSE;
            sNodeIdx = sOptSubQueryList->mSubQueryNodes[ sLoop ]->mIdx;

            /* execution */
            QLNX_EXECUTE_NODE( sLocalFetchInfo.mTransID,
                               sLocalFetchInfo.mStmt,
                               sLocalFetchInfo.mDBCStmt,
                               sLocalFetchInfo.mSQLStmt,
                               sOptSubQueryList->mSubQueryNodes[ sLoop ],
                               sLocalFetchInfo.mDataArea,
                               aEnv );

            /* fetch records */
            QLNX_FETCH_NODE( & sLocalFetchInfo,
                             sNodeIdx,
                             sStartIdx, /* Start Idx */
                             0, /* Skip Count */
                             sFetchedCnt, /* Fetch Count */
                             & sReadCnt,
                             & sEOF,
                             aEnv );

            /* list function의 결과는 하위 노드에서 설정됨 */

            STL_DASSERT( sReadCnt == sFetchedCnt );
        }
        else
        {
            /* Do Nothing */
        }
            
        sExprItem = sExprItem->mNext;
    }


    STL_RAMP( RAMP_EXIT );

    QLL_OPT_CHECK_TIME( sEndExceptTime );
    QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

    
    /**
     * OUTPUT 설정
     */

    *aUsedBlockCnt = sFetchedCnt;
    
    /* Parent의 Fetch Node Info 설정 */
    aFetchNodeInfo->mFetchCnt -= sFetchedCnt;
    aFetchNodeInfo->mSkipCnt  -= sSkippedCnt;
    
    for( sLoop = 0 ; sLoop < sOptSubQueryList->mSubQueryNodeCnt ; sLoop++ )
    {
        sTargetBlock = sExeSubQueryList->mTargetBlockList[ sLoop ];
        
        KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sTargetBlock,
                                             0,
                                             sFetchedCnt );

        sRefColBlock = sExeSubQueryList->mRefColBlockList[ sLoop ];

        KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sRefColBlock,
                                             0,
                                             sFetchedCnt );
    }

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeSubQueryList->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeSubQueryList->mCommonInfo.mFetchCallCount, 1 );
    QLL_OPT_ADD_RECORD_STAT_INFO( sExeSubQueryList->mCommonInfo.mFetchRecordStat,
                                  sExeSubQueryList->mCommonInfo.mResultColBlock,
                                  *aUsedBlockCnt,
                                  aEnv );

    sExeSubQueryList->mCommonInfo.mFetchRowCnt += *aUsedBlockCnt;

    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SINGLE_ROW_SUBQUERY_RETURNS_MULTIPLE_ROWS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SINGLE_ROW_SUBQUERY_RETURNS_MORE_THAN_ONE_ROW,
                      qlgMakeErrPosString( sLocalFetchInfo.mSQLStmt->mRetrySQL,
                                           sExprItem->mExprPtr->mPosition,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeSubQueryList->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeSubQueryList->mCommonInfo.mFetchCallCount, 1 );

    return STL_FAILURE;
}


/**
 * @brief SUB QUERY LIST node 수행을 종료한다.
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 *
 * @remark Execution을 종료하고자 할 때 호출한다.
 */
stlStatus qlnxFinalizeSubQueryList( qllOptimizationNode   * aOptNode,
                                    qllDataArea           * aDataArea,
                                    qllEnv                * aEnv )
{
    /* qllExecutionNode  * sExecNode        = NULL; */
    /* qlnoSubQueryList  * sOptSubQueryList = NULL; */
    /* qlnxSubQueryList  * sExeSubQueryList = NULL; */

    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_LIST_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    /**
     * SUB-QUERY LIST Optimization Node 획득
     */

    /* sOptSubQueryList = (qlnoSubQueryList *) aOptNode->mOptNode; */

    
    /**
     * Common Execution Node 획득
     */
        
    /* sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) ); */


    /**
     * SUB-QUERY Execution Node 획득
     */

    /* sExeSubQueryList = sExecNode->mExecNode; */


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} qlnx */
