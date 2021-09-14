/*******************************************************************************
 * qlneIndexAccess.c
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
 * @file qlneIndexAccess.c
 * @brief SQL Processor Layer Explain Node - INDEX ACCESS
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
stlStatus qlneExplainIndexAccess( qllOptimizationNode   * aOptNode,
                                  qllDataArea           * aDataArea,
                                  knlRegionMem          * aRegionMem,
                                  stlUInt32               aDepth,
                                  qllExplainNodeText   ** aPlanText,
                                  qllEnv                * aEnv )
{
    qlnoIndexAccess     * sOptIndexAccess   = NULL;
    qllExecutionNode    * sExecNode         = NULL;
    qllExplainNodeText  * sCurPlanText      = NULL;
    qllExplainPredText  * sCurPredText      = NULL;


    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_INDEX_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDepth >= 0, QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * INDEX ACCESS Node 정보 출력
     *********************************************************/

    sOptIndexAccess = (qlnoIndexAccess *) aOptNode->mOptNode;
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
    if( stlStrcmp( ((qlvInitBaseTableNode *)sOptIndexAccess->mRelationNode)->mRelationName->mTable,
                   sOptIndexAccess->mTableName ) == 0 )
    {
        STL_TRY( qleSetStringToExplainText( aRegionMem,
                                            &(sCurPlanText->mDescBuffer),
                                            aEnv,
                                            "INDEX ACCESS (\"%s, %s\")",
                                            sOptIndexAccess->mTableName,
                                            sOptIndexAccess->mIndexName )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( qleSetStringToExplainText( aRegionMem,
                                            &(sCurPlanText->mDescBuffer),
                                            aEnv,
                                            "INDEX ACCESS (\"%s AS %s, %s \")",
                                            sOptIndexAccess->mTableName,
                                            ((qlvInitBaseTableNode *)sOptIndexAccess->mRelationNode)->mRelationName->mTable,
                                            sOptIndexAccess->mIndexName )
                 == STL_SUCCESS );
    }

    sCurPlanText->mFetchRowCnt =
        ((qlnxIndexAccess *) sExecNode->mExecNode)->mCommonInfo.mFetchRowCnt;

    sCurPlanText->mBuildTime =
        ((qlnxIndexAccess *) sExecNode->mExecNode)->mCommonInfo.mBuildTime;
    sCurPlanText->mFetchTime =
        ((qlnxIndexAccess *) sExecNode->mExecNode)->mCommonInfo.mFetchTime;
    sCurPlanText->mFetchRecordAvgSize =
        ( ((qlnxIndexAccess *) sExecNode->mExecNode)->mCommonInfo.mFetchRecordStat.mTotalCount > 0
          ? ( ((qlnxIndexAccess *) sExecNode->mExecNode)->mCommonInfo.mFetchRecordStat.mTotalSize /
              ((qlnxIndexAccess *) sExecNode->mExecNode)->mCommonInfo.mFetchRecordStat.mTotalCount )
          : 0 );
    sCurPlanText->mFetchCallCount =
        ((qlnxIndexAccess *) sExecNode->mExecNode)->mCommonInfo.mFetchCallCount;


    /**
     * Index Read Column List 출력
     */

    /* Explain Predicate Text 생성 */
    STL_TRY( qleCreateExplainPredText( aRegionMem,
                                       0,
                                       *aPlanText,
                                       &sCurPredText,
                                       aEnv )
             == STL_SUCCESS );

    if( sOptIndexAccess->mIndexReadColList->mCount > 0 )
    {
        STL_TRY( qleAppendStringToExplainText( aRegionMem,
                                               &(sCurPredText->mLineBuffer),
                                               aEnv,
                                               "READ INDEX COLUMNS : " )
                 == STL_SUCCESS );

        STL_TRY( qleSetExplainExprList( aRegionMem,
                                        &(sCurPredText->mLineBuffer),
                                        sOptIndexAccess->mIndexReadColList,
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
                                               "READ INDEX COLUMNS : NOTHING" )
                 == STL_SUCCESS );
    }

    STL_TRY( qleAppendStringToExplainText( aRegionMem,
                                           &(sCurPredText->mLineBuffer),
                                           aEnv,
                                           "\n" )
             == STL_SUCCESS );
    

    /**
     * Table Read Column List 출력
     */

    if( sOptIndexAccess->mTableReadColList->mCount > 0 )
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
                                               "READ TABLE COLUMNS : " )
                 == STL_SUCCESS );

        STL_TRY( qleSetExplainExprList( aRegionMem,
                                        &(sCurPredText->mLineBuffer),
                                        sOptIndexAccess->mTableReadColList,
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
     * Aggregation Functions
     */

    if( sOptIndexAccess->mTotalAggrFuncList != NULL )
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
                                        sOptIndexAccess->mTotalAggrFuncList,
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
     * Min-Range 출력
     */

    if( sOptIndexAccess->mIndexScanInfo->mMinRangeExpr != NULL )
    {
        if( sOptIndexAccess->mIndexScanInfo->mMinRangeExpr[0]->mCount > 0 )
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
                                                   "MIN RANGE : " )
                     == STL_SUCCESS );

            STL_TRY( qleSetExplainRange( aRegionMem,
                                         &(sCurPredText->mLineBuffer),
                                         sOptIndexAccess->mIndexScanInfo->mMinRangeExpr,
                                         sOptIndexAccess->mIndexScanInfo->mReadKeyCnt,
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
     * Max-Range 출력
     */

    if( sOptIndexAccess->mIndexScanInfo->mMaxRangeExpr != NULL )
    {
        if( sOptIndexAccess->mIndexScanInfo->mMaxRangeExpr[0]->mCount > 0 )
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
                                                   "MAX RANGE : " )
                     == STL_SUCCESS );

            STL_TRY( qleSetExplainRange( aRegionMem,
                                         &(sCurPredText->mLineBuffer),
                                         sOptIndexAccess->mIndexScanInfo->mMaxRangeExpr,
                                         sOptIndexAccess->mIndexScanInfo->mReadKeyCnt,
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
     * Physical Key Filter 출력
     */

    if( sOptIndexAccess->mIndexScanInfo->mPhysicalKeyFilterExprList != NULL )
    {
        if( sOptIndexAccess->mIndexScanInfo->mPhysicalKeyFilterExprList->mCount > 0 )
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
                                                   "PHYSICAL KEY FILTER : " )
                     == STL_SUCCESS );

            STL_TRY( qleSetExplainExprList( aRegionMem,
                                            &(sCurPredText->mLineBuffer),
                                            sOptIndexAccess->mIndexScanInfo->mPhysicalKeyFilterExprList,
                                            " AND ",
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
     * Logical Key Filter 출력
     */

    if( sOptIndexAccess->mIndexScanInfo->mLogicalKeyFilterExprList != NULL )
    {
        if( sOptIndexAccess->mIndexScanInfo->mLogicalKeyFilterExprList->mCount > 0 )
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
                                                   "LOGICAL KEY FILTER : " )
                     == STL_SUCCESS );

            STL_TRY( qleSetExplainExprList( aRegionMem,
                                            &(sCurPredText->mLineBuffer),
                                            sOptIndexAccess->mIndexScanInfo->mLogicalKeyFilterExprList,
                                            " AND ",
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
     * Physical Table Filter 출력
     */

    if( sOptIndexAccess->mIndexScanInfo->mPhysicalTableFilterExprList != NULL )
    {
        if( sOptIndexAccess->mIndexScanInfo->mPhysicalTableFilterExprList->mCount > 0 )
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
                                                   "PHYSICAL TABLE FILTER : " )
                     == STL_SUCCESS );

            STL_TRY( qleSetExplainExprList( aRegionMem,
                                            &(sCurPredText->mLineBuffer),
                                            sOptIndexAccess->mIndexScanInfo->mPhysicalTableFilterExprList,
                                            " AND ",
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
     * Logical Table Filter 출력
     */

    if( sOptIndexAccess->mIndexScanInfo->mLogicalTableFilterExprList != NULL )
    {
        if( sOptIndexAccess->mIndexScanInfo->mLogicalTableFilterExprList->mCount > 0 )
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
                                                   "LOGICAL TABLE FILTER : " )
                     == STL_SUCCESS );

            STL_TRY( qleSetExplainExprList( aRegionMem,
                                            &(sCurPredText->mLineBuffer),
                                            sOptIndexAccess->mIndexScanInfo->mLogicalTableFilterExprList,
                                            " AND ",
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


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} qlne */
