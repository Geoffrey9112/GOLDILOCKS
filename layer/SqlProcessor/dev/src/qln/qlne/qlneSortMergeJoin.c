/*******************************************************************************
 * qlneSortMergeJoin.c
 *
 * Copyright (c) 2013, SUNJESOFT Inc.
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
 * @file qlneSortMergeJoin.c
 * @brief SQL Processor Layer Explain Node - SORT MERGE JOIN
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
stlStatus qlneExplainSortMergeJoin( qllOptimizationNode   * aOptNode,
                                    qllDataArea           * aDataArea,
                                    knlRegionMem          * aRegionMem,
                                    stlUInt32               aDepth,
                                    qllExplainNodeText   ** aPlanText,
                                    qllEnv                * aEnv )
{
    qlnoSortMergeJoin   * sOptSortMergeJoin = NULL;
    qllExecutionNode    * sExecNode         = NULL;
    qlvRefExprItem      * sLeftSortKeyItem  = NULL;
    qlvRefExprItem      * sRightSortKeyItem = NULL;
    qllExplainNodeText  * sCurPlanText      = NULL;
    qllExplainPredText  * sCurPredText      = NULL;


    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_MERGE_JOIN_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDepth >= 0, QLL_ERROR_STACK(aEnv) );

    
    /**********************************************************
     * SORT MERGE JOIN Node 정보 출력
     *********************************************************/

    sOptSortMergeJoin = (qlnoSortMergeJoin *) aOptNode->mOptNode;
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
                                        "SORT MERGE JOIN (%s) : EQUAL",
                                        qlneJoinTypeName[ sOptSortMergeJoin->mJoinType ] )
             == STL_SUCCESS );

    sCurPlanText->mFetchRowCnt =
        ((qlnxSortMergeJoin *) sExecNode->mExecNode)->mCommonInfo.mFetchRowCnt;

    sCurPlanText->mBuildTime =
        ((qlnxSortMergeJoin *) sExecNode->mExecNode)->mCommonInfo.mBuildTime;
    sCurPlanText->mFetchTime =
        ((qlnxSortMergeJoin *) sExecNode->mExecNode)->mCommonInfo.mFetchTime;
    sCurPlanText->mFetchRecordAvgSize =
        ( ((qlnxSortMergeJoin *) sExecNode->mExecNode)->mCommonInfo.mFetchRecordStat.mTotalCount > 0
          ? ( ((qlnxSortMergeJoin *) sExecNode->mExecNode)->mCommonInfo.mFetchRecordStat.mTotalSize /
              ((qlnxSortMergeJoin *) sExecNode->mExecNode)->mCommonInfo.mFetchRecordStat.mTotalCount )
          : 0 );
    sCurPlanText->mFetchCallCount =
        ((qlnxSortMergeJoin *) sExecNode->mExecNode)->mCommonInfo.mFetchCallCount;

    
    /**
     * Joined Column List 출력
     */

    /* Explain Predicate Text 생성 */
    STL_TRY( qleCreateExplainPredText( aRegionMem,
                                       0,
                                       *aPlanText,
                                       &sCurPredText,
                                       aEnv )
             == STL_SUCCESS );

    if( sOptSortMergeJoin->mJoinColList->mCount > 0 )
    {
        STL_TRY( qleAppendStringToExplainText( aRegionMem,
                                               &(sCurPredText->mLineBuffer),
                                               aEnv,
                                               "JOINED COLUMNS : " )
                 == STL_SUCCESS );

        STL_TRY( qleSetExplainExprList( aRegionMem,
                                        &(sCurPredText->mLineBuffer),
                                        sOptSortMergeJoin->mJoinColList,
                                        ", ",
                                        STL_TRUE,
                                        aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( qleAppendStringToExplainText( aRegionMem,
                                               &(sCurPredText->mLineBuffer),
                                               aEnv,
                                               "JOINED COLUMNS : NOTHING" )
                 == STL_SUCCESS );
    }

    STL_TRY( qleAppendStringToExplainText( aRegionMem,
                                           &(sCurPredText->mLineBuffer),
                                           aEnv,
                                           "\n" )
             == STL_SUCCESS );


    /**
     * Sort Merge Join의 Key들 조건 출력
     */

    if( sOptSortMergeJoin->mLeftSortKeyList == NULL )
    {
        /* Do Nothing */
    }
    else
    {
        if( sOptSortMergeJoin->mLeftSortKeyList->mCount > 0 )
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
                                                   "MERGE FILTER : " )
                     == STL_SUCCESS );

            sLeftSortKeyItem = sOptSortMergeJoin->mLeftSortKeyList->mHead;
            sRightSortKeyItem = sOptSortMergeJoin->mRightSortKeyList->mHead;

            while( sLeftSortKeyItem != NULL )
            {
                STL_TRY( qleSetExplainExpr( aRegionMem,
                                            &(sCurPredText->mLineBuffer),
                                            sLeftSortKeyItem->mExprPtr,
                                            STL_FALSE,
                                            STL_TRUE,
                                            aEnv )
                         == STL_SUCCESS );

                STL_TRY( qleAppendStringToExplainText( aRegionMem,
                                                       &(sCurPredText->mLineBuffer),
                                                       aEnv,
                                                       " = " )
                         == STL_SUCCESS );

                STL_TRY( qleSetExplainExpr( aRegionMem,
                                            &(sCurPredText->mLineBuffer),
                                            sRightSortKeyItem->mExprPtr,
                                            STL_FALSE,
                                            STL_TRUE,
                                            aEnv )
                         == STL_SUCCESS );

                if( sLeftSortKeyItem->mNext != NULL )
                {
                    STL_TRY( qleAppendStringToExplainText( aRegionMem,
                                                           &(sCurPredText->mLineBuffer),
                                                           aEnv,
                                                           " AND " )
                             == STL_SUCCESS );
                }

                sLeftSortKeyItem = sLeftSortKeyItem->mNext;
                sRightSortKeyItem = sRightSortKeyItem->mNext;
            }

            STL_TRY( qleAppendStringToExplainText( aRegionMem,
                                                   &(sCurPredText->mLineBuffer),
                                                   aEnv,
                                                   "\n" )
                     == STL_SUCCESS );
        }
    }


    /**
     * Join Filter 출력
     */

    if( sOptSortMergeJoin->mJoinFilterExpr == NULL )
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
                                               "JOIN FILTER : " )
                 == STL_SUCCESS );

        STL_TRY( qleSetExplainExpr( aRegionMem,
                                    &(sCurPredText->mLineBuffer),
                                    sOptSortMergeJoin->mJoinFilterExpr,
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

 
    /**
     * Where Filter 출력
     */

    if( sOptSortMergeJoin->mWhereFilterExpr == NULL )
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
                                               "WHERE FILTER : " )
                 == STL_SUCCESS );

        STL_TRY( qleSetExplainExpr( aRegionMem,
                                    &(sCurPredText->mLineBuffer),
                                    sOptSortMergeJoin->mWhereFilterExpr,
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
     * Child Node 정보 출력
     *********************************************************/

    /**
     * Child Node 정보 출력
     */

    STL_TRY( qlneExplainNodeList[ sOptSortMergeJoin->mLeftChildNode->mType ] (
                 sOptSortMergeJoin->mLeftChildNode,
                 aDataArea,
                 aRegionMem,
                 aDepth + 1,
                 aPlanText,
                 aEnv )
             == STL_SUCCESS );

    STL_TRY( qlneExplainNodeList[ sOptSortMergeJoin->mRightChildNode->mType ] (
                 sOptSortMergeJoin->mRightChildNode,
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
