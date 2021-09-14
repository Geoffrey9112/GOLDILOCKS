/*******************************************************************************
 * qlndSubQuery.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlndSubQuery.c 8545 2013-05-23 01:55:08Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlndSubQuery.c
 * @brief SQL Processor Layer Data Optimization Node - Sub Query
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnd
 * @{
 */


/**
 * @brief Sub Query 을 Data Optimize 한다.
 * @param[in]  aTransID   Transcation ID
 * @param[in]  aDBCStmt   DBC Statement
 * @param[in]  aSQLStmt   SQL Statement
 * @param[in]  aOptNode   Code Optimization Node
 * @param[in]  aDataArea  Data Area
 * @param[in]  aEnv       Environment
 * @remarks
 */
stlStatus qlndDataOptimizeSubQuery( smlTransId              aTransID,
                                    qllDBCStmt            * aDBCStmt,
                                    qllStatement          * aSQLStmt,
                                    qllOptimizationNode   * aOptNode,
                                    qllDataArea           * aDataArea,
                                    qllEnv                * aEnv )
{
    qllExecutionNode    * sExecNode         = NULL;
    qlnoSubQuery        * sOptSubQuery      = NULL;
    qlnxSubQuery        * sExeSubQuery      = NULL;
    /* knlValueBlockList   * sOuterColBlock    = NULL; */
    /* knlValueBlockList   * sOuterOrgColBlock = NULL; */
    /* qlvRefExprItem      * sExprItem         = NULL; */

    /* qloDataExprContext  * sExprContext        = NULL; */
    /* knlExprContext      * sOuterColContext    = NULL; */
    /* knlExprContext      * sOuterOrgColContext = NULL; */
    
    
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * QUERY EXPR Optimization Node 획득
     */

    sOptSubQuery = (qlnoSubQuery *) aOptNode->mOptNode;


    /**
     * 하위 node에 대한 Data Optimize 수행
     */

    QLND_DATA_OPTIMIZE( aTransID,
                        aDBCStmt,
                        aSQLStmt,
                        sOptSubQuery->mChildNode,
                        aDataArea,
                        aEnv );


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea,
                                        QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * QUERY EXPR Execution Node 할당
     */

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlnxSubQuery ),
                                (void **) & sExeSubQuery,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sExeSubQuery, 0x00, STL_SIZEOF( qlnxSubQuery ) );


    /**
     * QUERY EXPR Execution Node 정보 초기화
     */
    
    /* Target Column List 구성 (share) */
    STL_DASSERT( sOptSubQuery->mTargetList->mCount > 0 );

    STL_TRY( qlndBuildRefBlockList( aSQLStmt,
                                    aDataArea,
                                    NULL,  /* statement expression list */
                                    sOptSubQuery->mTargetList,
                                    & sExeSubQuery->mTargetBlock,
                                    & QLL_DATA_PLAN( aDBCStmt ),
                                    aEnv )
             == STL_SUCCESS );


    /**
     * Filter Evaluate Info 및 관련 value block list 설정
     */

    if( sOptSubQuery->mFilterPred != NULL )
    {
        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                    STL_SIZEOF( knlExprEvalInfo ),
                                    (void **) & sExeSubQuery->mFilterExprEvalInfo,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlMemset( sExeSubQuery->mFilterExprEvalInfo, 0x00, STL_SIZEOF( knlExprEvalInfo ) );

        sExeSubQuery->mFilterExprEvalInfo->mExprStack      =
            KNL_PRED_GET_FILTER( sOptSubQuery->mFilterPred );
        sExeSubQuery->mFilterExprEvalInfo->mContext        =
            aDataArea->mExprDataContext;
        sExeSubQuery->mFilterExprEvalInfo->mResultBlock    =
            aDataArea->mPredResultBlock;

        sExeSubQuery->mFilterExprEvalInfo->mBlockIdx   = 0;
        sExeSubQuery->mFilterExprEvalInfo->mBlockCount = 0;
    }
    else
    {
        sExeSubQuery->mFilterExprEvalInfo = NULL;
    }


    /**
     * Outer Column Value Block List 설정
     */

    /* sExprContext = aDataArea->mExprDataContext; */
    
    /* if( sOptSubQuery->mOuterColList != NULL ) */
    /* { */
    /*     sExprItem = sOptSubQuery->mOuterColList->mHead; */
        
    /*     while( sExprItem != NULL  ) */
    /*     { */
    /*         STL_DASSERT( sExprItem->mExprPtr->mType == QLV_EXPR_TYPE_OUTER_COLUMN ); */
    /*         STL_DASSERT( ( sExprItem->mExprPtr->mExpr.mOuterColumn->mOrgExpr->mType == */
    /*                        QLV_EXPR_TYPE_COLUMN ) || */
    /*                      ( sExprItem->mExprPtr->mExpr.mOuterColumn->mOrgExpr->mType == */
    /*                        QLV_EXPR_TYPE_INNER_COLUMN ) || */
    /*                      ( sExprItem->mExprPtr->mExpr.mOuterColumn->mOrgExpr->mType == */
    /*                        QLV_EXPR_TYPE_ROWID_COLUMN ) ); */
                    
    /*         sOuterColContext    = & sExprContext->mContexts[ sExprItem->mExprPtr->mOffset ]; */
    /*         sOuterOrgColContext = & sExprContext->mContexts */
    /*             [ sExprItem->mExprPtr->mExpr.mOuterColumn->mOrgExpr->mOffset ]; */
            
    /*         STL_TRY( knlInitShareBlockFromBlock( & sOuterColBlock, */
    /*                                              sOuterColContext->mInfo.mValueInfo, */
    /*                                              & QLL_DATA_PLAN( aDBCStmt ), */
    /*                                              KNL_ENV(aEnv) ) */
    /*                  == STL_SUCCESS ); */

    /*         STL_TRY( knlInitShareBlockFromBlock( & sOuterOrgColBlock, */
    /*                                              sOuterOrgColContext->mInfo.mValueInfo, */
    /*                                              & QLL_DATA_PLAN( aDBCStmt ), */
    /*                                              KNL_ENV(aEnv) ) */
    /*                  == STL_SUCCESS ); */

    /*         if( sExeSubQuery->mOuterColBlock == NULL ) */
    /*         { */
    /*             /\* Do Nothing *\/ */
    /*         } */
    /*         else */
    /*         { */
    /*             KNL_LINK_BLOCK_LIST( sOuterColBlock, sExeSubQuery->mOuterColBlock ); */
    /*             KNL_LINK_BLOCK_LIST( sOuterOrgColBlock, sExeSubQuery->mOuterOrgColBlock ); */
    /*         } */
            
    /*         sExeSubQuery->mOuterColBlock    = sOuterColBlock; */
    /*         sExeSubQuery->mOuterOrgColBlock = sOuterOrgColBlock; */

    /*         sExprItem = sExprItem->mNext; */
    /*     } */
    /* } */
    /* else */
    /* { */
    /*     sExeSubQuery->mOuterColBlock    = NULL; */
    /*     sExeSubQuery->mOuterOrgColBlock = NULL; */
    /* } */
    

    /**
     * Common Info 설정
     */

    sExeSubQuery->mCommonInfo.mResultColBlock = sExeSubQuery->mTargetBlock;
    sExeSubQuery->mCommonInfo.mRowBlockList =
        ((qlnxCommonInfo*)(aDataArea->mExecNodeList[sOptSubQuery->mChildNode->mIdx].mExecNode))->mRowBlockList;


    /**
     * Outer Column Value Block List 설정
     */

    STL_TRY( qlndBuildOuterColBlockList( aDataArea,
                                         sOptSubQuery->mOuterColumnList,
                                         & sExeSubQuery->mCommonInfo.mOuterColBlock,
                                         & sExeSubQuery->mCommonInfo.mOuterOrgColBlock,
                                         & QLL_DATA_PLAN( aDBCStmt ),
                                         aEnv )
             == STL_SUCCESS );


    /**
     * Common Execution Node 정보 설정
     */

    sExecNode->mNodeType  = QLL_PLAN_NODE_SUB_QUERY_TYPE;

    sExecNode->mOptNode   = aOptNode;
    sExecNode->mExecNode  = sExeSubQuery;

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
