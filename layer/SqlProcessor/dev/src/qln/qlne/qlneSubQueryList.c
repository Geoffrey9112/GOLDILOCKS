/*******************************************************************************
 * qlneSubQueryList.c
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
 * @file qlneSubQueryList.c
 * @brief SQL Processor Layer Explain Node - SUB QUERY LIST
 */

#include <qll.h>
#include <qlDef.h>

/**
 * @addtogroup qlne
 * @{
 */

/**
 * @brief Sub Query List node에 대한 수행 정보를 출력한다.
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area
 * @param[in]      aRegionMem    Region Memory Manager
 * @param[in]      aDepth        Node Depth
 * @param[in,out]  aPlanText     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 */
stlStatus qlneExplainSubQueryList( qllOptimizationNode   * aOptNode,
                                   qllDataArea           * aDataArea,
                                   knlRegionMem          * aRegionMem,
                                   stlUInt32               aDepth,
                                   qllExplainNodeText   ** aPlanText,
                                   qllEnv                * aEnv )
{
    qlnoSubQueryList    * sOptSubQueryList  = NULL;
    qllExecutionNode    * sExecNode         = NULL;
    stlInt32              sLoop             = 0;
    qllExplainNodeText  * sCurPlanText      = NULL;
    qllExplainPredText  * sCurPredText      = NULL;

    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_LIST_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDepth >= 0, QLL_ERROR_STACK(aEnv) );
    
    
    /**********************************************************
     * Sub Query List Node 정보 출력
     *********************************************************/

    sOptSubQueryList = (qlnoSubQueryList *) aOptNode->mOptNode;
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * Child Node 정보 출력
     */

    if( sOptSubQueryList->mChildNode != NULL )
    {
        STL_TRY( qlneExplainNodeList[ sOptSubQueryList->mChildNode->mType ] (
                     sOptSubQueryList->mChildNode,
                     aDataArea,
                     aRegionMem,
                     aDepth,
                     aPlanText,
                     aEnv )
                 == STL_SUCCESS );
    }

    
    /**
     * Sub Query List Node 정보 출력
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
                                        "SUB QUERY LIST" )
             == STL_SUCCESS );

    sCurPlanText->mFetchRowCnt =
        ((qlnxSubQueryList *) sExecNode->mExecNode)->mCommonInfo.mFetchRowCnt;

    sCurPlanText->mBuildTime =
        ((qlnxSubQueryList *) sExecNode->mExecNode)->mCommonInfo.mBuildTime;
    sCurPlanText->mFetchTime =
        ((qlnxSubQueryList *) sExecNode->mExecNode)->mCommonInfo.mFetchTime;
    sCurPlanText->mFetchRecordAvgSize =
        ( ((qlnxSubQueryList *) sExecNode->mExecNode)->mCommonInfo.mFetchRecordStat.mTotalCount > 0
          ? ( ((qlnxSubQueryList *) sExecNode->mExecNode)->mCommonInfo.mFetchRecordStat.mTotalSize /
              ((qlnxSubQueryList *) sExecNode->mExecNode)->mCommonInfo.mFetchRecordStat.mTotalCount )
          : 0 );
    sCurPlanText->mFetchCallCount =
        ((qlnxSubQueryList *) sExecNode->mExecNode)->mCommonInfo.mFetchCallCount;

    if( sOptSubQueryList->mOuterColumnList != NULL )
    {
        if( sOptSubQueryList->mOuterColumnList->mCount > 0 )
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
                                                   "READ COLUMNS : " )
                     == STL_SUCCESS );

            STL_TRY( qleSetExplainExprList( aRegionMem,
                                            &(sCurPredText->mLineBuffer),
                                            sOptSubQueryList->mOuterColumnList,
                                            ", ",
                                            STL_FALSE,
                                            aEnv )
                     == STL_SUCCESS );

            STL_TRY( qleAppendStringToExplainText( aRegionMem,
                                                   &(sCurPredText->mLineBuffer),
                                                   aEnv,
                                                   "\n" )
                     == STL_SUCCESS );
        }
        else
        {
            /* Do Nothing */
        }
    }
    else
    {
        /* Do Nothing */
    }


    /**********************************************************
     * Node 정보 출력
     *********************************************************/

    /**
     * Sub Query List Node 정보 출력
     */

    for( sLoop = 0 ; sLoop < sOptSubQueryList->mSubQueryNodeCnt ; sLoop++ )
    {
        STL_TRY( qlneExplainNodeList[ sOptSubQueryList->mSubQueryNodes[ sLoop ]->mType ] (
                     sOptSubQueryList->mSubQueryNodes[ sLoop ],
                     aDataArea,
                     aRegionMem,
                     aDepth + 1,
                     aPlanText,
                     aEnv )
                 == STL_SUCCESS );
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
