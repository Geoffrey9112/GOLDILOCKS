/*******************************************************************************
 * qlndSubQueryList.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlndSubQueryList.c 8545 2013-05-23 01:55:08Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlndSubQueryList.c
 * @brief SQL Processor Layer Data Optimization Node - Sub Query List
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnd
 * @{
 */


/**
 * @brief Sub Query List 을 Data Optimize 한다.
 * @param[in]  aTransID   Transcation ID
 * @param[in]  aDBCStmt   DBC Statement
 * @param[in]  aSQLStmt   SQL Statement
 * @param[in]  aOptNode   Code Optimization Node
 * @param[in]  aDataArea  Data Area
 * @param[in]  aEnv       Environment
 * @remarks
 */
stlStatus qlndDataOptimizeSubQueryList( smlTransId              aTransID,
                                        qllDBCStmt            * aDBCStmt,
                                        qllStatement          * aSQLStmt,
                                        qllOptimizationNode   * aOptNode,
                                        qllDataArea           * aDataArea,
                                        qllEnv                * aEnv )
{
    qllExecutionNode    * sExecNode          = NULL;
    qlnoSubQueryList    * sOptSubQueryList   = NULL;
    qlnxSubQueryList    * sExeSubQueryList   = NULL;

    qlvRefExprItem      * sSubQueryExprItem  = NULL;
    qlvRefExprItem      * sTargetExprItem    = NULL;
    qlvRefExprItem      * sRefColExprItem    = NULL;
    knlExprContext      * sContext           = NULL;
    
    knlValueBlockList  ** sTargetBlockList   = NULL;
    knlValueBlockList  ** sRefColBlockList   = NULL;
    knlValueBlockList   * sTargetBlock       = NULL;
    knlValueBlockList   * sRefColBlock       = NULL;
    knlValueBlockList   * sPrevBlock         = NULL;
    stlInt32              sLoop              = 0;
    
    
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_LIST_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * QUERY EXPR Optimization Node 획득
     */

    sOptSubQueryList = (qlnoSubQueryList *) aOptNode->mOptNode;


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea,
                                        QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * QUERY EXPR Execution Node 할당
     */

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlnxSubQueryList ),
                                (void **) & sExeSubQueryList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sExeSubQueryList, 0x00, STL_SIZEOF( qlnxSubQueryList ) );


    /**
     * Statement단위로 처리 가능한 Sub Query node에 대한 Data Optimize 수행
     */

    sSubQueryExprItem = sOptSubQueryList->mSubQueryExprList->mHead;
    for( sLoop = 0 ; sLoop < sOptSubQueryList->mSubQueryNodeCnt ; sLoop++ )
    {
        if( sSubQueryExprItem->mExprPtr->mIterationTime >= DTL_ITERATION_TIME_FOR_EACH_STATEMENT )
        {
            QLND_DATA_OPTIMIZE( aTransID,
                                aDBCStmt,
                                aSQLStmt,
                                sOptSubQueryList->mSubQueryNodes[ sLoop ],
                                aDataArea,
                                aEnv );
        }

        sSubQueryExprItem = sSubQueryExprItem->mNext;
    }


    /**
     * 하위 node에 대한 Data Optimize 수행
     */

    if( sOptSubQueryList->mChildNode )
    {
        QLND_DATA_OPTIMIZE( aTransID,
                            aDBCStmt,
                            aSQLStmt,
                            sOptSubQueryList->mChildNode,
                            aDataArea,
                            aEnv );
    }
    

    /**
     * Statement단위로 처리 불가능한 Sub Query node에 대한 Data Optimize 수행
     */

    sSubQueryExprItem = sOptSubQueryList->mSubQueryExprList->mHead;
    for( sLoop = 0 ; sLoop < sOptSubQueryList->mSubQueryNodeCnt ; sLoop++ )
    {
        if( sSubQueryExprItem->mExprPtr->mIterationTime < DTL_ITERATION_TIME_FOR_EACH_STATEMENT )
        {
            QLND_DATA_OPTIMIZE( aTransID,
                                aDBCStmt,
                                aSQLStmt,
                                sOptSubQueryList->mSubQueryNodes[ sLoop ],
                                aDataArea,
                                aEnv );
        }

        sSubQueryExprItem = sSubQueryExprItem->mNext;
    }

    
    /**
     * Sub Query node에 대한 Target Value Block List Array 공간 할당
     */

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( knlValueBlockList * ) * sOptSubQueryList->mSubQueryNodeCnt,
                                (void **) & sTargetBlockList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    
    /**
     * Sub Query node에 대한 Target Value Block List 구성
     */

    sSubQueryExprItem = sOptSubQueryList->mSubQueryExprList->mHead;
    for( sLoop = 0 ; sLoop < sOptSubQueryList->mSubQueryNodeCnt ; sLoop++ )
    {
        STL_DASSERT( sSubQueryExprItem != NULL );

        if( sSubQueryExprItem->mExprPtr->mType == QLV_EXPR_TYPE_SUB_QUERY )
        {
            sPrevBlock = NULL;
            sTargetExprItem = sSubQueryExprItem->mExprPtr->mExpr.mSubQuery->mTargetList->mHead;
            while( sTargetExprItem != NULL )
            {
                sContext = & aDataArea->mExprDataContext->mContexts[ sTargetExprItem->mExprPtr->mOffset ];

                STL_DASSERT( sContext->mCast == NULL );

                STL_TRY( knlInitShareBlockFromBlock( & sTargetBlock,
                                                     sContext->mInfo.mValueInfo,
                                                     & QLL_DATA_PLAN( aDBCStmt ),
                                                     KNL_ENV(aEnv) )
                         == STL_SUCCESS );
            
                if( sPrevBlock == NULL )
                {
                    sTargetBlockList[ sLoop ] = sTargetBlock;
                }
                else
                {
                    KNL_LINK_BLOCK_LIST( sPrevBlock, sTargetBlock );
                }
                sPrevBlock = sTargetBlock;
            
                sTargetExprItem = sTargetExprItem->mNext;
            }
        }
        else
        {
            sTargetBlockList[ sLoop ] = NULL;
        }

        sSubQueryExprItem = sSubQueryExprItem->mNext;
    }
    

    /**
     * Sub Query node에 대한 Reference Column Value Block List Array 공간 할당
     */

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( knlValueBlockList * ) * sOptSubQueryList->mSubQueryNodeCnt,
                                (void **) & sRefColBlockList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Sub Query node에 대한 Reference Column Value Block List 구성
     */

    sSubQueryExprItem = sOptSubQueryList->mSubQueryExprList->mHead;
    for( sLoop = 0 ; sLoop < sOptSubQueryList->mSubQueryNodeCnt ; sLoop++ )
    {
        STL_DASSERT( sSubQueryExprItem != NULL );

        if( sSubQueryExprItem->mExprPtr->mType == QLV_EXPR_TYPE_SUB_QUERY )
        {
            sPrevBlock = NULL;
            sRefColExprItem = sSubQueryExprItem->mExprPtr->mExpr.mSubQuery->mRefColList->mHead;
            while( sRefColExprItem != NULL )
            {
                sContext = & aDataArea->mExprDataContext->mContexts[ sRefColExprItem->mExprPtr->mOffset ];

                STL_TRY( knlInitShareBlockFromBlock( & sRefColBlock,
                                                     sContext->mInfo.mValueInfo,
                                                     & QLL_DATA_PLAN( aDBCStmt ),
                                                     KNL_ENV(aEnv) )
                         == STL_SUCCESS );
            
                if( sPrevBlock == NULL )
                {
                    sRefColBlockList[ sLoop ] = sRefColBlock;
                }
                else
                {
                    KNL_LINK_BLOCK_LIST( sPrevBlock, sRefColBlock );
                }
                sPrevBlock = sRefColBlock;
            
                sRefColExprItem = sRefColExprItem->mNext;
            }
        }
        else
        {
            sRefColBlockList[ sLoop ] = NULL;
        }
        sSubQueryExprItem = sSubQueryExprItem->mNext;
    }

    
    /**
     * 하위 node에 대한 Data Optimize 수행
     */

    sExeSubQueryList->mTargetBlockList = sTargetBlockList;
    sExeSubQueryList->mRefColBlockList = sRefColBlockList;
    sExeSubQueryList->mSubQueryCnt     = sOptSubQueryList->mSubQueryNodeCnt;
    sExeSubQueryList->mIsFirstExec     = STL_TRUE;
    
    
    /**
     * Common Info 설정
     */

    if( sOptSubQueryList->mChildNode != NULL )
    {
        sExeSubQueryList->mCommonInfo.mResultColBlock =
            ((qlnxCommonInfo*)(aDataArea->mExecNodeList[sOptSubQueryList->mChildNode->mIdx].mExecNode))->mResultColBlock;
        sExeSubQueryList->mCommonInfo.mRowBlockList =
            ((qlnxCommonInfo*)(aDataArea->mExecNodeList[sOptSubQueryList->mChildNode->mIdx].mExecNode))->mRowBlockList;
        sExeSubQueryList->mCommonInfo.mOuterColBlock = NULL;
        sExeSubQueryList->mCommonInfo.mOuterOrgColBlock = NULL;
    }
    else
    {
        sExeSubQueryList->mCommonInfo.mResultColBlock = NULL;
        sExeSubQueryList->mCommonInfo.mRowBlockList = NULL;
        sExeSubQueryList->mCommonInfo.mOuterColBlock = NULL;
        sExeSubQueryList->mCommonInfo.mOuterOrgColBlock = NULL;
    }

    
    /**
     * Common Execution Node 정보 설정
     */

    sExecNode->mNodeType  = QLL_PLAN_NODE_SUB_QUERY_LIST_TYPE;

    sExecNode->mOptNode   = aOptNode;
    sExecNode->mExecNode  = sExeSubQueryList;

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
