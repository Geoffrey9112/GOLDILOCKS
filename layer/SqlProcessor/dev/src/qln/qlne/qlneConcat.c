/*******************************************************************************
 * qlneConcat.c
 *
 * Copyright (c) 2013, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlneConcat.c 7302 2013-02-18 04:04:06Z bsyou $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlneConcat.c
 * @brief SQL Processor Layer Explain Node - CONCAT
 */

#include <qll.h>
#include <qlDef.h>

/**
 * @addtogroup qlne
 * @{
 */


/**
 * @brief node에 대한 수행 정보를 출력한다.
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area
 * @param[in]      aRegionMem    Region Memory Manager
 * @param[in]      aDepth        Node Depth
 * @param[in,out]  aPlanText     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 */
stlStatus qlneExplainConcat( qllOptimizationNode   * aOptNode,
                             qllDataArea           * aDataArea,
                             knlRegionMem          * aRegionMem,
                             stlUInt32               aDepth,
                             qllExplainNodeText   ** aPlanText,
                             qllEnv                * aEnv )
{
    qlnoConcat          * sOptConcat    = NULL;
    qllExecutionNode    * sExecNode     = NULL;
    
    stlInt32              i = 0;
    qllExplainNodeText  * sCurPlanText  = NULL;
    

    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_CONCAT_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDepth >= 0, QLL_ERROR_STACK(aEnv) );

    
    /**********************************************************
     * HASH JOIN Node 정보 출력
     *********************************************************/

    sOptConcat = (qlnoConcat *) aOptNode->mOptNode;
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


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
                                        "CONCAT" )
             == STL_SUCCESS );

    sCurPlanText->mFetchRowCnt =
        ((qlnxConcat *) sExecNode->mExecNode)->mCommonInfo.mFetchRowCnt;

    sCurPlanText->mBuildTime =
        ((qlnxConcat *) sExecNode->mExecNode)->mCommonInfo.mBuildTime;
    sCurPlanText->mFetchTime =
        ((qlnxConcat *) sExecNode->mExecNode)->mCommonInfo.mFetchTime;
    sCurPlanText->mFetchRecordAvgSize =
        ( ((qlnxConcat *) sExecNode->mExecNode)->mCommonInfo.mFetchRecordStat.mTotalCount > 0
          ? ( ((qlnxConcat *) sExecNode->mExecNode)->mCommonInfo.mFetchRecordStat.mTotalSize /
              ((qlnxConcat *) sExecNode->mExecNode)->mCommonInfo.mFetchRecordStat.mTotalCount )
          : 0 );
    sCurPlanText->mFetchCallCount =
        ((qlnxConcat *) sExecNode->mExecNode)->mCommonInfo.mFetchCallCount;

    
    /**********************************************************
     * Child Node 정보 출력
     *********************************************************/

    /**
     * Child Node 정보 출력
     */

    for( i = 0; i < sOptConcat->mChildCount; i++ )
    {
        STL_TRY( qlneExplainNodeList[ sOptConcat->mChildNodeArr[i]->mType ] (
                     sOptConcat->mChildNodeArr[i],
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
