/*******************************************************************************
 * qlneTableAccess.c
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
 * @file qlneTableAccess.c
 * @brief SQL Processor Layer Explain Node - TABLE ACCESS
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
stlStatus qlneExplainTableAccess( qllOptimizationNode   * aOptNode,
                                  qllDataArea           * aDataArea,
                                  knlRegionMem          * aRegionMem,
                                  stlUInt32               aDepth,
                                  qllExplainNodeText   ** aPlanText,
                                  qllEnv                * aEnv )
{
    qlnoTableAccess     * sOptTableAccess   = NULL;
    qllExecutionNode    * sExecNode         = NULL;
    qllExplainNodeText  * sCurPlanText      = NULL;
    qllExplainPredText  * sCurPredText      = NULL;

    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_TABLE_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDepth >= 0, QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * TABLE ACCESS Node 정보 출력
     *********************************************************/

    sOptTableAccess = (qlnoTableAccess *) aOptNode->mOptNode;
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
    if( stlStrcmp( ((qlvInitBaseTableNode *)sOptTableAccess->mRelationNode)->mRelationName->mTable,
                   sOptTableAccess->mTableName ) == 0 )
    {
        STL_TRY( qleSetStringToExplainText( aRegionMem,
                                            &(sCurPlanText->mDescBuffer),
                                            aEnv,
                                            "TABLE ACCESS (\"%s\")",
                                            sOptTableAccess->mTableName )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( qleSetStringToExplainText( aRegionMem,
                                            &(sCurPlanText->mDescBuffer),
                                            aEnv,
                                            "TABLE ACCESS (\"%s AS %s\")",
                                            sOptTableAccess->mTableName,
                                            ((qlvInitBaseTableNode *)sOptTableAccess->mRelationNode)->mRelationName->mTable )
                 == STL_SUCCESS );
    }

    sCurPlanText->mFetchRowCnt =
        ((qlnxTableAccess *) sExecNode->mExecNode)->mCommonInfo.mFetchRowCnt;

    sCurPlanText->mBuildTime =
        ((qlnxTableAccess *) sExecNode->mExecNode)->mCommonInfo.mBuildTime;
    sCurPlanText->mFetchTime =
        ((qlnxTableAccess *) sExecNode->mExecNode)->mCommonInfo.mFetchTime;
    sCurPlanText->mFetchRecordAvgSize =
        ( ((qlnxTableAccess *) sExecNode->mExecNode)->mCommonInfo.mFetchRecordStat.mTotalCount > 0
          ? ( ((qlnxTableAccess *) sExecNode->mExecNode)->mCommonInfo.mFetchRecordStat.mTotalSize /
              ((qlnxTableAccess *) sExecNode->mExecNode)->mCommonInfo.mFetchRecordStat.mTotalCount )
          : 0 );
    sCurPlanText->mFetchCallCount =
        ((qlnxTableAccess *) sExecNode->mExecNode)->mCommonInfo.mFetchCallCount;


    /**
     * Read Column List 출력
     */

    /* Explain Predicate Text 생성 */
    STL_TRY( qleCreateExplainPredText( aRegionMem,
                                       0,
                                       *aPlanText,
                                       &sCurPredText,
                                       aEnv )
             == STL_SUCCESS );

    if( sOptTableAccess->mReadColList->mCount > 0 )
    {
        STL_TRY( qleAppendStringToExplainText( aRegionMem,
                                               &(sCurPredText->mLineBuffer),
                                               aEnv,
                                               "READ COLUMNS : " )
                 == STL_SUCCESS );

        STL_TRY( qleSetExplainExprList( aRegionMem,
                                        &(sCurPredText->mLineBuffer),
                                        sOptTableAccess->mReadColList,
                                        ", ",
                                        STL_FALSE,
                                        aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( qleAppendStringToExplainText( aRegionMem,
                                               &(sCurPredText->mLineBuffer),
                                               aEnv,
                                               "READ COLUMNS : NOTHING" )
                 == STL_SUCCESS );
    }

    STL_TRY( qleAppendStringToExplainText( aRegionMem,
                                           &(sCurPredText->mLineBuffer),
                                           aEnv,
                                           "\n" )
             == STL_SUCCESS );


    /**
     * Aggregation Functions
     */

    if( sOptTableAccess->mTotalAggrFuncList != NULL )
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
                                        sOptTableAccess->mTotalAggrFuncList,
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
     * Physical Filter 출력
     */

    if( sOptTableAccess->mPhysicalFilterExpr == NULL )
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

        /* Row Header */
        STL_TRY( qleAppendStringToExplainText( aRegionMem,
                                               &(sCurPredText->mLineBuffer),
                                               aEnv,
                                               "PHYSICAL FILTER : " )
                 == STL_SUCCESS );

        STL_TRY( qleSetExplainExpr( aRegionMem,
                                    &(sCurPredText->mLineBuffer),
                                    sOptTableAccess->mPhysicalFilterExpr,
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


    /**
     * Logical Filter 출력
     */

    if( sOptTableAccess->mLogicalFilterExpr == NULL )
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

        /* Row Header */
        STL_TRY( qleAppendStringToExplainText( aRegionMem,
                                               &(sCurPredText->mLineBuffer),
                                               aEnv,
                                               "LOGICAL FILTER : " )
                 == STL_SUCCESS );

        STL_TRY( qleSetExplainExpr( aRegionMem,
                                    &(sCurPredText->mLineBuffer),
                                    sOptTableAccess->mLogicalFilterExpr,
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


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} qlne */
