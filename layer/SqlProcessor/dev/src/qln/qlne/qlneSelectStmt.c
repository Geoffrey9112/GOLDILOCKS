/*******************************************************************************
 * qlneSelectStmt.c
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
 * @file qlneSelectStmt.c
 * @brief SQL Processor Layer Explain Node - SELECT STATEMENT
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
stlStatus qlneExplainSelectStmt( qllOptimizationNode   * aOptNode,
                                 qllDataArea           * aDataArea,
                                 knlRegionMem          * aRegionMem,
                                 stlUInt32               aDepth,
                                 qllExplainNodeText   ** aPlanText,
                                 qllEnv                * aEnv )
{
    qlnoSelectStmt      * sOptSelectStmt    = NULL;
    qllExplainNodeText  * sCurPlanText      = NULL;
    qllExecutionNode    * sExecNode         = NULL;

    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_STMT_SELECT_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDepth >= 0, QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * SELECT STMT Node 정보 출력
     *********************************************************/

    sOptSelectStmt = (qlnoSelectStmt *) aOptNode->mOptNode;

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
                                        "SELECT STATEMENT" )
             == STL_SUCCESS );

    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );

    sCurPlanText->mBuildTime =
        ((qlnxSelectStmt *) sExecNode->mExecNode)->mCommonInfo.mBuildTime;
    sCurPlanText->mFetchTime =
        ((qlnxSelectStmt *) sExecNode->mExecNode)->mCommonInfo.mFetchTime;
    sCurPlanText->mFetchCallCount =
        ((qlnxSelectStmt *) sExecNode->mExecNode)->mCommonInfo.mFetchCallCount;

    
    /**********************************************************
     * Child Node 정보 출력
     *********************************************************/

    /**
     * Child Node 정보 출력
     */

    STL_TRY( qlneExplainNodeList[ sOptSelectStmt->mQueryNode->mType ] (
                 sOptSelectStmt->mQueryNode,
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
