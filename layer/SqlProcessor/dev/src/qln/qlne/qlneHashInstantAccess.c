/*******************************************************************************
 * qlneHashInstantAccess.c
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
 * @file qlneHashInstantAccess.c
 * @brief SQL Processor Layer Explain Node - HASH INSTANT ACCESS
 */

#include <qll.h>
#include <qlDef.h>

/**
 * @addtogroup qlne
 * @{
 */


#if 0
/**
 * @brief node에 대한 수행 정보를 출력한다.
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area
 * @param[in]      aRegionMem    Region Memory Manager
 * @param[in]      aDepth        Node Depth
 * @param[in,out]  aPlanText     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 */
stlStatus qlneExplainHashInstantAccess( qllOptimizationNode   * aOptNode,
                                        qllDataArea           * aDataArea,
                                        knlRegionMem          * aRegionMem,
                                        stlUInt32               aDepth,
                                        qllExplainNodeText   ** aPlanText,
                                        qllEnv                * aEnv )
{
    qlnoInstant         * sOptInstant   = NULL;
    qllExecutionNode    * sExecNode     = NULL;
    qlvInitInstantNode  * sInstantNode  = NULL;
    qllExplainNodeText  * sCurPlanText  = NULL;
    qllExplainPredText  * sCurPredText  = NULL;


    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_HASH_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDepth >= 0, QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * HASH INSTANT ACCESS Node 정보 출력
     *********************************************************/

    sOptInstant = (qlnoInstant *) aOptNode->mOptNode;
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );
    sInstantNode = sOptInstant->mInstantNode;

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
                                        "HASH INSTANT ACCESS" )
             == STL_SUCCESS );

    sCurPlanText->mFetchRowCnt =
        ((qlnxInstant *) sExecNode->mExecNode)->mCommonInfo.mFetchRowCnt;

    sCurPlanText->mBuildTime =
        ((qlnxInstant *) sExecNode->mExecNode)->mCommonInfo.mBuildTime;
    sCurPlanText->mFetchTime =
        ((qlnxInstant *) sExecNode->mExecNode)->mCommonInfo.mFetchTime;
    sCurPlanText->mFetchRecordAvgSize =
        ( ((qlnxInstant *) sExecNode->mExecNode)->mCommonInfo.mFetchRecordStat.mTotalCount > 0
          ? ( ((qlnxInstant *) sExecNode->mExecNode)->mCommonInfo.mFetchRecordStat.mTotalSize /
              ((qlnxInstant *) sExecNode->mExecNode)->mCommonInfo.mFetchRecordStat.mTotalCount )
          : 0 );
    sCurPlanText->mFetchCallCount =
        ((qlnxInstant *) sExecNode->mExecNode)->mCommonInfo.mFetchCallCount;


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

    if( sInstantNode->mKeyColCnt > 0 )
    {
        STL_TRY( qleAppendStringToExplainText( aRegionMem,
                                               &(sCurPredText->mLineBuffer),
                                               aEnv,
                                               "INDEX COLUMNS : " )
                 == STL_SUCCESS );

        STL_TRY( qleSetExplainExprList( aRegionMem,
                                        &(sCurPredText->mLineBuffer),
                                        sInstantNode->mKeyColList,
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
                                               "INDEX COLUMNS : NOTHING" )
                 == STL_SUCCESS );
    }

    STL_TRY( qleAppendStringToExplainText( aRegionMem,
                                           &(sCurPredText->mLineBuffer),
                                           aEnv,
                                           "\n" )
             == STL_SUCCESS );


    /**
     * Record Column List 출력
     */

    /* Explain Predicate Text 생성 */
    STL_TRY( qleCreateExplainPredText( aRegionMem,
                                       0,
                                       *aPlanText,
                                       &sCurPredText,
                                       aEnv )
             == STL_SUCCESS );

    if( sInstantNode->mRecColCnt > 0 )
    {
        STL_TRY( qleAppendStringToExplainText( aRegionMem,
                                               &(sCurPredText->mLineBuffer),
                                               aEnv,
                                               "TABLE COLUMNS : " )
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
     * Hash Filter 출력
     */

    if( sOptInstant->mHashScanInfo->mHashFilterExpr != NULL )
    {
        if( sOptInstant->mHashScanInfo->mHashFilterExpr[0]->mCount > 0 )
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
                                                   "HASH FILTER : " )
                     == STL_SUCCESS );

            STL_TRY( qleSetExplainRange( aRegionMem,
                                         &(sCurPredText->mLineBuffer),
                                         sOptInstant->mHashScanInfo->mHashFilterExpr,
                                         sOptInstant->mHashScanInfo->mHashColCount,
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

    if( sOptInstant->mHashScanInfo->mPhysicalTableFilterExprList != NULL )
    {
        if( sOptInstant->mHashScanInfo->mPhysicalTableFilterExprList->mCount > 0 )
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
                                                   "PHYSICAL TABLE FILTER : " )
                     == STL_SUCCESS );

            STL_TRY( qleSetExplainExprList( aRegionMem,
                                            &(sCurPredText->mLineBuffer),
                                            sOptInstant->mHashScanInfo->mPhysicalTableFilterExprList,
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

    if( sOptInstant->mHashScanInfo->mLogicalTableFilterExprList != NULL )
    {
        if( sOptInstant->mHashScanInfo->mLogicalTableFilterExprList->mCount > 0 )
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
                                                   "LOGICAL TABLE FILTER : " )
                     == STL_SUCCESS );

            STL_TRY( qleSetExplainExprList( aRegionMem,
                                            &(sCurPredText->mLineBuffer),
                                            sOptInstant->mHashScanInfo->mLogicalTableFilterExprList,
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


    /**********************************************************
     * Child Node 정보 출력
     *********************************************************/

    /**
     * Child Node 정보 출력
     */

    STL_TRY( qlneExplainNodeList[ sOptInstant->mChildNode->mType ] ( sOptInstant->mChildNode,
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
#endif

/** @} qlne */
