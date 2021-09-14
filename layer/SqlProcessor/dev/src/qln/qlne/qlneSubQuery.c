/*******************************************************************************
 * qlneSubQuery.c
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
 * @file qlneSubQuery.c
 * @brief SQL Processor Layer Explain Node - SUB QUERY
 */

#include <qll.h>
#include <qlDef.h>

/**
 * @addtogroup qlne
 * @{
 */

/**
 * @brief Sub Query node에 대한 수행 정보를 출력한다.
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area
 * @param[in]      aRegionMem    Region Memory Manager
 * @param[in]      aDepth        Node Depth
 * @param[in,out]  aPlanText     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 */
stlStatus qlneExplainSubQuery( qllOptimizationNode   * aOptNode,
                               qllDataArea           * aDataArea,
                               knlRegionMem          * aRegionMem,
                               stlUInt32               aDepth,
                               qllExplainNodeText   ** aPlanText,
                               qllEnv                * aEnv )
{
    qlnoSubQuery        * sOptSubQuery  = NULL;
    qllExecutionNode    * sExecNode     = NULL;
    qlvInitSubTableNode * sInitSubQuery = NULL;
    qllExplainNodeText  * sCurPlanText  = NULL;
    qllExplainPredText  * sCurPredText  = NULL;

    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDepth >= 0, QLL_ERROR_STACK(aEnv) );
    
    
    /**********************************************************
     * Sub Query Node 정보 출력
     *********************************************************/

    sOptSubQuery = (qlnoSubQuery *) aOptNode->mOptNode;
    sInitSubQuery = sOptSubQuery->mInitNode;
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
    STL_DASSERT( sInitSubQuery->mRelationName != NULL );

    if( sInitSubQuery->mRelationName->mTable != NULL )
    {
        STL_TRY( qleSetStringToExplainText( aRegionMem,
                                            &(sCurPlanText->mDescBuffer),
                                            aEnv,
                                            "VIEW (\"%s\")",
                                            sInitSubQuery->mRelationName->mTable )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( qleSetStringToExplainText( aRegionMem,
                                            &(sCurPlanText->mDescBuffer),
                                            aEnv,
                                            "VIEW" )
                 == STL_SUCCESS );
    }

    sCurPlanText->mFetchRowCnt =
        ((qlnxSubQuery *) sExecNode->mExecNode)->mCommonInfo.mFetchRowCnt;

    sCurPlanText->mBuildTime =
        ((qlnxSubQuery *) sExecNode->mExecNode)->mCommonInfo.mBuildTime;
    sCurPlanText->mFetchTime =
        ((qlnxSubQuery *) sExecNode->mExecNode)->mCommonInfo.mFetchTime;
    sCurPlanText->mFetchRecordAvgSize =
        ( ((qlnxSubQuery *) sExecNode->mExecNode)->mCommonInfo.mFetchRecordStat.mTotalCount > 0
          ? ( ((qlnxSubQuery *) sExecNode->mExecNode)->mCommonInfo.mFetchRecordStat.mTotalSize /
              ((qlnxSubQuery *) sExecNode->mExecNode)->mCommonInfo.mFetchRecordStat.mTotalCount )
          : 0 );
    sCurPlanText->mFetchCallCount =
        ((qlnxSubQuery *) sExecNode->mExecNode)->mCommonInfo.mFetchCallCount;


    /**
     * Target Column List 출력
     */

    STL_DASSERT( sOptSubQuery->mTargetCount > 0 );

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
                                           "COLUMNS : " )
             == STL_SUCCESS );

    STL_TRY( qleSetExplainExprList( aRegionMem,
                                    &(sCurPredText->mLineBuffer),
                                    sOptSubQuery->mTargetList,
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
     * Outer Column List 출력
     */

    if( sOptSubQuery->mOuterColumnList != NULL )
    {
        if( sOptSubQuery->mOuterColumnList->mCount > 0 )
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
                                            sOptSubQuery->mOuterColumnList,
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
    

    /**
     * Filter 출력
     */

    if( sOptSubQuery->mFilterExpr == NULL )
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
                                               "FILTER : " )
                 == STL_SUCCESS );

        STL_TRY( qleSetExplainExpr( aRegionMem,
                                    &(sCurPredText->mLineBuffer),
                                    sOptSubQuery->mFilterExpr,
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


    /**********************************************************
     * Node 정보 출력
     *********************************************************/

    /**
     * Child Node 정보 출력
     */

    STL_TRY( qlneExplainNodeList[ sOptSubQuery->mChildNode->mType ] (
                 sOptSubQuery->mChildNode,
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
