/*******************************************************************************
 * qlneSubQueryFunc.c
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
 * @file qlneSubQueryFunc.c
 * @brief SQL Processor Layer Explain Node - SUB QUERY FUNCTION
 */

#include <qll.h>
#include <qlDef.h>

/**
 * @addtogroup qlne
 * @{
 */

/**
 * @brief Sub Query Func node에 대한 수행 정보를 출력한다.
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area
 * @param[in]      aRegionMem    Region Memory Manager
 * @param[in]      aDepth        Node Depth
 * @param[in,out]  aPlanText     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 */
stlStatus qlneExplainSubQueryFunc( qllOptimizationNode   * aOptNode,
                                   qllDataArea           * aDataArea,
                                   knlRegionMem          * aRegionMem,
                                   stlUInt32               aDepth,
                                   qllExplainNodeText   ** aPlanText,
                                   qllEnv                * aEnv )
{
    qlnoSubQueryFunc    * sOptSubQueryFunc  = NULL;
    qllExecutionNode    * sExecNode         = NULL;
    qllExplainNodeText  * sCurPlanText      = NULL;
    qllExplainPredText  * sCurPredText      = NULL;

    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_FUNCTION_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDepth >= 0, QLL_ERROR_STACK(aEnv) );
    
    
    /**********************************************************
     * Sub Query Func Node 정보 출력
     *********************************************************/

    sOptSubQueryFunc = (qlnoSubQueryFunc *) aOptNode->mOptNode;
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * Sub Query Func Node 정보 출력
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
    if( ((qlnxSubQueryFunc *) sExecNode->mExecNode)->mNeedMaterialize == STL_FALSE )
    {
        STL_TRY( qleSetStringToExplainText( aRegionMem,
                                            &(sCurPlanText->mDescBuffer),
                                            aEnv,
                                            "SUB QUERY FUNCTION" )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( qleSetStringToExplainText( aRegionMem,
                                            &(sCurPlanText->mDescBuffer),
                                            aEnv,
                                            "SUB QUERY FUNCTION (MATERIALIZED)" )
                 == STL_SUCCESS );
    }

    sCurPlanText->mFetchRowCnt =
        ((qlnxSubQueryFunc *) sExecNode->mExecNode)->mCommonInfo.mFetchRowCnt;

    sCurPlanText->mBuildTime =
        ((qlnxSubQueryFunc *) sExecNode->mExecNode)->mCommonInfo.mBuildTime;
    sCurPlanText->mFetchTime =
        ((qlnxSubQueryFunc *) sExecNode->mExecNode)->mCommonInfo.mFetchTime;
    sCurPlanText->mFetchRecordAvgSize =
        ( ((qlnxSubQueryFunc *) sExecNode->mExecNode)->mCommonInfo.mFetchRecordStat.mTotalCount > 0
          ? ( ((qlnxSubQueryFunc *) sExecNode->mExecNode)->mCommonInfo.mFetchRecordStat.mTotalSize /
              ((qlnxSubQueryFunc *) sExecNode->mExecNode)->mCommonInfo.mFetchRecordStat.mTotalCount )
          : 0 );
    sCurPlanText->mFetchCallCount =
        ((qlnxSubQueryFunc *) sExecNode->mExecNode)->mCommonInfo.mFetchCallCount;

    /* Explain Predicate Text 생성 */
    STL_TRY( qleCreateExplainPredText( aRegionMem,
                                       0,
                                       *aPlanText,
                                       &sCurPredText,
                                       aEnv )
             == STL_SUCCESS );

    STL_TRY( qleAppendStringToExplainText( aRegionMem,
                                           &(sCurPredText->mLineBuffer),
                                           aEnv,
                                           "FUNCTION : " )
             == STL_SUCCESS );

    STL_TRY( qleSetExplainExpr( aRegionMem,
                                &(sCurPredText->mLineBuffer),
                                sOptSubQueryFunc->mFuncExpr,
                                STL_FALSE,
                                STL_FALSE,
                                aEnv )
             == STL_SUCCESS );

    STL_TRY( qleAppendStringToExplainText( aRegionMem,
                                           &(sCurPredText->mLineBuffer),
                                           aEnv,
                                           "\n" )
             == STL_SUCCESS );

    
    /**********************************************************
     * Node 정보 출력
     *********************************************************/

    /**
     * Sub Query Func Node 정보 출력
     */

    STL_TRY( qlneExplainNodeList[ sOptSubQueryFunc->mRightOperandNode->mType ] (
                 sOptSubQueryFunc->mRightOperandNode,
                 aDataArea,
                 aRegionMem,
                 aDepth + 1,
                 aPlanText,
                 aEnv )
             == STL_SUCCESS );


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
