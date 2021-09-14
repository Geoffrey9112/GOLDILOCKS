/*******************************************************************************
 * qlneGroup.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlneGroup.c 13531 2014-09-04 02:10:50Z bsyou $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlneGroup.c
 * @brief SQL Processor Layer Explain Node - GROUP
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
stlStatus qlneExplainGroup( qllOptimizationNode   * aOptNode,
                            qllDataArea           * aDataArea,
                            knlRegionMem          * aRegionMem,
                            stlUInt32               aDepth,
                            qllExplainNodeText   ** aPlanText,
                            qllEnv                * aEnv )
{
    qlnoGroup           * sOptGroup     = NULL;
    qllExecutionNode    * sExecNode     = NULL;
    qlvInitInstantNode  * sInstantNode  = NULL;
    qllExplainNodeText  * sCurPlanText  = NULL;
    qllExplainPredText  * sCurPredText  = NULL;


    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_GROUP_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDepth >= 0, QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * GROUP INSTANT ACCESS Node 정보 출력
     *********************************************************/

    sOptGroup = (qlnoGroup *) aOptNode->mOptNode;
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );
    sInstantNode = sOptGroup->mInstantNode;

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
                                        "GROUP" )
             == STL_SUCCESS );

    sCurPlanText->mFetchRowCnt =
        ((qlnxGroup *) sExecNode->mExecNode)->mCommonInfo.mFetchRowCnt;

    sCurPlanText->mBuildTime =
        ((qlnxGroup *) sExecNode->mExecNode)->mCommonInfo.mBuildTime;
    sCurPlanText->mFetchTime =
        ((qlnxGroup *) sExecNode->mExecNode)->mCommonInfo.mFetchTime;
    sCurPlanText->mFetchRecordAvgSize =
        ( ((qlnxGroup *) sExecNode->mExecNode)->mCommonInfo.mFetchRecordStat.mTotalCount > 0
          ? ( ((qlnxGroup *) sExecNode->mExecNode)->mCommonInfo.mFetchRecordStat.mTotalSize /
              ((qlnxGroup *) sExecNode->mExecNode)->mCommonInfo.mFetchRecordStat.mTotalCount )
          : 0 );
    sCurPlanText->mFetchCallCount =
        ((qlnxGroup *) sExecNode->mExecNode)->mCommonInfo.mFetchCallCount;


    /**
     * Aggregation Functions
     */


    if( sOptGroup->mTotalAggrFuncList != NULL )
    {
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
                                               "AGGREGATIONS : " )
                 == STL_SUCCESS );

        STL_TRY( qleSetExplainExprList( aRegionMem,
                                        &(sCurPredText->mLineBuffer),
                                        sOptGroup->mTotalAggrFuncList,
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


    /**
     * Key Column List 출력
     */

    /* Explain Predicate Text 생성 */
    STL_TRY( qleCreateExplainPredText( aRegionMem,
                                       0,
                                       *aPlanText,
                                       &sCurPredText,
                                       aEnv )
             == STL_SUCCESS );

    STL_DASSERT( sInstantNode->mKeyColCnt > 0 );

    STL_TRY( qleAppendStringToExplainText( aRegionMem,
                                           &(sCurPredText->mLineBuffer),
                                           aEnv,
                                           "GROUPING COLUMNS : " )
             == STL_SUCCESS );

    STL_TRY( qleSetExplainExprList( aRegionMem,
                                    &(sCurPredText->mLineBuffer),
                                    sInstantNode->mKeyColList,
                                    ", ",
                                    STL_FALSE,
                                    aEnv )
             == STL_SUCCESS );

    STL_TRY( qleAppendStringToExplainText( aRegionMem,
                                           &(sCurPredText->mLineBuffer),
                                           aEnv,
                                           "\n" )
             == STL_SUCCESS );


    /**
     * Record Column List 출력
     */

    if( sInstantNode->mRecColCnt > 0 )
    {
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
                                               "RECORD COLUMNS : " )
                 == STL_SUCCESS );

        STL_TRY( qleSetExplainExprList( aRegionMem,
                                        &(sCurPredText->mLineBuffer),
                                        sInstantNode->mRecColList,
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


    /**
     * Read Column List 출력
     */

    if( sInstantNode->mReadColCnt > 0 )
    {
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
                                               "READ COLUMNS : " )
                 == STL_SUCCESS );

        STL_TRY( qleSetExplainExprList( aRegionMem,
                                        &(sCurPredText->mLineBuffer),
                                        sInstantNode->mReadColList,
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


    /**
     * Logical Filter 출력
     */

    if( sOptGroup->mTotalFilterExpr == NULL )
    {
        /* Do Nothing */
    }
    else
    {
        /* Explain Predicate Text 생성 */
        STL_TRY( qleCreateExplainPredText( aRegionMem,
                                           1,
                                           *aPlanText,
                                           &sCurPredText,
                                           aEnv )
                 == STL_SUCCESS );

        STL_TRY( qleAppendStringToExplainText( aRegionMem,
                                               &(sCurPredText->mLineBuffer),
                                               aEnv,
                                               "LOGICAL FILTER : " )
                 == STL_SUCCESS );

        STL_TRY( qleSetExplainExpr( aRegionMem,
                                    &(sCurPredText->mLineBuffer),
                                    sOptGroup->mTotalFilterExpr,
                                    STL_FALSE,
                                    STL_FALSE,
                                    aEnv )
                 == STL_SUCCESS );

        STL_TRY( qleAppendStringToExplainText( aRegionMem,
                                               &(sCurPredText->mLineBuffer),
                                               aEnv,
                                               "\n" )
                 == STL_SUCCESS );
    }


    /**********************************************************
     * Child Node 정보 출력
     *********************************************************/

    /**
     * Child Node 정보 출력
     */

    STL_TRY( qlneExplainNodeList[ sOptGroup->mChildNode->mType ] ( sOptGroup->mChildNode,
                                                                   aDataArea,
                                                                   aRegionMem,
                                                                   aDepth + 1,
                                                                   aPlanText,
                                                                   aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} qlne */
