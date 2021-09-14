/*******************************************************************************
 * qlneSubQueryFilter.c
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
 * @file qlneSubQueryFilter.c
 * @brief SQL Processor Layer Explain Node - SUB QUERY FILTER
 */

#include <qll.h>
#include <qlDef.h>

/**
 * @addtogroup qlne
 * @{
 */

/**
 * @brief Sub Query Filter node에 대한 수행 정보를 출력한다.
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area
 * @param[in]      aRegionMem    Region Memory Manager
 * @param[in]      aDepth        Node Depth
 * @param[in,out]  aPlanText     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 */
stlStatus qlneExplainSubQueryFilter( qllOptimizationNode   * aOptNode,
                                     qllDataArea           * aDataArea,
                                     knlRegionMem          * aRegionMem,
                                     stlUInt32               aDepth,
                                     qllExplainNodeText   ** aPlanText,
                                     qllEnv                * aEnv )
{
    qlnoSubQueryFilter  * sOptSubQueryFilter    = NULL;
    qllExecutionNode    * sExecNode             = NULL;
    qllExplainNodeText  * sCurPlanText          = NULL;
    qllExplainPredText  * sCurPredText          = NULL;
    stlInt32              sLoop                 = 0;
    
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_FILTER_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDepth >= 0, QLL_ERROR_STACK(aEnv) );
    
    
    /**********************************************************
     * Sub Query Node 정보 출력
     *********************************************************/

    sOptSubQueryFilter = (qlnoSubQueryFilter *) aOptNode->mOptNode;
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**********************************************************
     * Node 정보 출력
     *********************************************************/

    /**
     * SUB QUERY Node 출력
     */

    /* Explain Node Text 생성 */
    STL_TRY( qleCreateExplainNodeText( aRegionMem,
                                       aDepth,
                                       *aPlanText,
                                       &sCurPlanText,
                                       aEnv )
             == STL_SUCCESS );

    /* 상위 Node Text 포인터 변경 */
    *aPlanText = sCurPlanText;

    /* Node 정보 저장 */
    STL_TRY( qleSetStringToExplainText( aRegionMem,
                                        &(sCurPlanText->mDescBuffer),
                                        aEnv,
                                        "SUB QUERY FILTER" )
             == STL_SUCCESS );

    sCurPlanText->mFetchRowCnt =
        ((qlnxSubQueryFilter *) sExecNode->mExecNode)->mCommonInfo.mFetchRowCnt;

    sCurPlanText->mBuildTime =
        ((qlnxSubQueryFilter *) sExecNode->mExecNode)->mCommonInfo.mBuildTime;
    sCurPlanText->mFetchTime =
        ((qlnxSubQueryFilter *) sExecNode->mExecNode)->mCommonInfo.mFetchTime;
    sCurPlanText->mFetchRecordAvgSize =
        ( ((qlnxSubQueryFilter *) sExecNode->mExecNode)->mCommonInfo.mFetchRecordStat.mTotalCount > 0
          ? ( ((qlnxSubQueryFilter *) sExecNode->mExecNode)->mCommonInfo.mFetchRecordStat.mTotalSize /
              ((qlnxSubQueryFilter *) sExecNode->mExecNode)->mCommonInfo.mFetchRecordStat.mTotalCount )
          : 0 );
    sCurPlanText->mFetchCallCount =
        ((qlnxSubQueryFilter *) sExecNode->mExecNode)->mCommonInfo.mFetchCallCount;


    /**
     * Filter 출력
     */

    if( sOptSubQueryFilter->mFilterExprArr == NULL )
    {
        /* Do Nothing */
    }
    else
    {
        for( sLoop = 0 ; sLoop < sOptSubQueryFilter->mAndFilterCnt ; sLoop++ )
        {
            /* Explain Predicate Text 생성 */
            STL_TRY( qleCreateExplainPredText( aRegionMem,
                                               0,
                                               *aPlanText,
                                               &sCurPredText,
                                               aEnv )
                     == STL_SUCCESS );

            /* Row Header */
            STL_TRY( qleAppendStringToExplainText( aRegionMem,
                                                   &(sCurPredText->mLineBuffer),
                                                   aEnv,
                                                   "FILTER : " )
                     == STL_SUCCESS );

            STL_TRY( qleSetExplainExpr( aRegionMem,
                                        &(sCurPredText->mLineBuffer),
                                        sOptSubQueryFilter->mFilterExprArr[ sLoop ],
                                        STL_FALSE,
                                        STL_TRUE,
                                        aEnv )
                     == STL_SUCCESS );

            STL_TRY( qleAppendStringToExplainText( aRegionMem,
                                                   &(sCurPredText->mLineBuffer),
                                                   aEnv,
                                                   "\n" )
                     == STL_SUCCESS );
        }
        
        for( sLoop = 0 ; sLoop < sOptSubQueryFilter->mAndFilterCnt ; sLoop++ )
        {
            /**
             * Child Node 정보 출력
             */

            STL_TRY( qlneExplainNodeList[ sOptSubQueryFilter->mAndFilterNodeArr[ sLoop ]->mType ] (
                         sOptSubQueryFilter->mAndFilterNodeArr[ sLoop ],
                         aDataArea,
                         aRegionMem,
                         aDepth + 1,
                         aPlanText,
                         aEnv )
                     == STL_SUCCESS );
        }
    }

    
    /**********************************************************
     * Cost 정보 출력
     *********************************************************/

    /**
     * @todo Cost 정보 출력
     */
    

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} qlne */
