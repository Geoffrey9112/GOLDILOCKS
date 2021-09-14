/*******************************************************************************
 * qlndSubQueryFilter.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlndSubQueryFilter.c 8545 2013-05-23 01:55:08Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlndSubQueryFilter.c
 * @brief SQL Processor Layer Data Optimization Node - Sub Query Filter
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnd
 * @{
 */


/**
 * @brief Sub Query Filter 을 Data Optimize 한다.
 * @param[in]  aTransID   Transcation ID
 * @param[in]  aDBCStmt   DBC Statement
 * @param[in]  aSQLStmt   SQL Statement
 * @param[in]  aOptNode   Code Optimization Node
 * @param[in]  aDataArea  Data Area
 * @param[in]  aEnv       Environment
 * @remarks
 */
stlStatus qlndDataOptimizeSubQueryFilter( smlTransId              aTransID,
                                          qllDBCStmt            * aDBCStmt,
                                          qllStatement          * aSQLStmt,
                                          qllOptimizationNode   * aOptNode,
                                          qllDataArea           * aDataArea,
                                          qllEnv                * aEnv )
{
    qllExecutionNode    * sExecNode          = NULL;
    qllExecutionNode    * sChildExecNode     = NULL; 
    qlnoSubQueryFilter  * sOptSubQueryFilter = NULL;
    qlnxSubQueryFilter  * sExeSubQueryFilter = NULL;
    knlExprEvalInfo     * sExprEvalInfo      = NULL;
    stlInt32              sLoop              = 0;

    
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_FILTER_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * QUERY EXPR Optimization Node 획득
     */

    sOptSubQueryFilter = (qlnoSubQueryFilter *) aOptNode->mOptNode;


    /**
     * 하위 node에 대한 Data Optimize 수행
     */

    for( sLoop = 0 ; sLoop < sOptSubQueryFilter->mAndFilterCnt ; sLoop++ )
    {
        QLND_DATA_OPTIMIZE( aTransID,
                            aDBCStmt,
                            aSQLStmt,
                            sOptSubQueryFilter->mAndFilterNodeArr[ sLoop ],
                            aDataArea,
                            aEnv );
    }


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea,
                                        QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * QUERY EXPR Execution Node 할당
     */

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlnxSubQueryFilter ),
                                (void **) & sExeSubQueryFilter,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sExeSubQueryFilter, 0x00, STL_SIZEOF( qlnxSubQueryFilter ) );


    /**
     * Filter Evaluate Info 및 관련 value block list 설정
     */

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( knlExprEvalInfo* ) * sOptSubQueryFilter->mAndFilterCnt,
                                (void **) & sExeSubQueryFilter->mFilterExprEvalInfo,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sExeSubQueryFilter->mFilterExprEvalInfo,
               0x00,
               STL_SIZEOF( knlExprEvalInfo* ) * sOptSubQueryFilter->mAndFilterCnt );

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( knlExprEvalInfo ) * sOptSubQueryFilter->mAndFilterCnt,
                                (void **) & sExprEvalInfo,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    for( sLoop = 0 ; sLoop < sOptSubQueryFilter->mAndFilterCnt ; sLoop++ )
    {
        STL_DASSERT( sOptSubQueryFilter->mFilterPredArr[ sLoop ] != NULL );

        sExprEvalInfo[ sLoop ].mExprStack      =
            KNL_PRED_GET_FILTER( sOptSubQueryFilter->mFilterPredArr[ sLoop ] );
        sExprEvalInfo[ sLoop ].mContext        =
            aDataArea->mExprDataContext;
        sExprEvalInfo[ sLoop ].mResultBlock    =
            aDataArea->mPredResultBlock;

        sExprEvalInfo[ sLoop ].mBlockIdx   = 0;
        sExprEvalInfo[ sLoop ].mBlockCount = 0;

        sExeSubQueryFilter->mFilterExprEvalInfo[ sLoop ] = & sExprEvalInfo[ sLoop ];
    }
    

    /**
     * Common Info 설정
     */

    sChildExecNode = & aDataArea->mExecNodeList[ sOptSubQueryFilter->mChildNode->mIdx ];

    sExeSubQueryFilter->mCommonInfo.mResultColBlock =
        ((qlnxCommonInfo*)sChildExecNode->mExecNode)->mResultColBlock;
    sExeSubQueryFilter->mCommonInfo.mRowBlockList =
        ((qlnxCommonInfo*)sChildExecNode->mExecNode)->mRowBlockList;
    
    sExeSubQueryFilter->mCommonInfo.mOuterColBlock    = NULL;
    sExeSubQueryFilter->mCommonInfo.mOuterOrgColBlock = NULL;
    

    /**
     * Common Execution Node 정보 설정
     */

    sExecNode->mNodeType  = QLL_PLAN_NODE_SUB_QUERY_FILTER_TYPE;

    sExecNode->mOptNode   = aOptNode;
    sExecNode->mExecNode  = sExeSubQueryFilter;

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
