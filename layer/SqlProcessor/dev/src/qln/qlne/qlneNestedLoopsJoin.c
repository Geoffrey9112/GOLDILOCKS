/*******************************************************************************
 * qlneNestedLoopsJoin.c
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
 * @file qlneNestedLoopsJoin.c
 * @brief SQL Processor Layer Explain Node - NESTED LOOPS JOIN
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
stlStatus qlneExplainNestedLoopsJoin( qllOptimizationNode   * aOptNode,
                                      qllDataArea           * aDataArea,
                                      knlRegionMem          * aRegionMem,
                                      stlUInt32               aDepth,
                                      qllExplainNodeText   ** aPlanText,
                                      qllEnv                * aEnv )
{
    stlInt32              i;
    qlnoNestedLoopsJoin * sOptNestedLoopsJoin   = NULL;
    qllExecutionNode    * sExecNode             = NULL;
    qllExplainNodeText  * sCurPlanText          = NULL;
    qllExplainPredText  * sCurPredText          = NULL;


    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_NESTED_LOOPS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDepth >= 0, QLL_ERROR_STACK(aEnv) );

    
    /**********************************************************
     * NESTED LOOPS JOIN Node 정보 출력
     *********************************************************/

    sOptNestedLoopsJoin = (qlnoNestedLoopsJoin *) aOptNode->mOptNode;
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
                                        "NESTED LOOP JOIN (%s)",
                                        qlneJoinTypeName[ sOptNestedLoopsJoin->mJoinType ] )
             == STL_SUCCESS );

    sCurPlanText->mFetchRowCnt =
        ((qlnxNestedLoopsJoin *) sExecNode->mExecNode)->mCommonInfo.mFetchRowCnt;

    sCurPlanText->mBuildTime =
        ((qlnxNestedLoopsJoin *) sExecNode->mExecNode)->mCommonInfo.mBuildTime;
    sCurPlanText->mFetchTime =
        ((qlnxNestedLoopsJoin *) sExecNode->mExecNode)->mCommonInfo.mFetchTime;
    sCurPlanText->mFetchRecordAvgSize =
        ( ((qlnxNestedLoopsJoin *) sExecNode->mExecNode)->mCommonInfo.mFetchRecordStat.mTotalCount > 0
          ? ( ((qlnxNestedLoopsJoin *) sExecNode->mExecNode)->mCommonInfo.mFetchRecordStat.mTotalSize /
              ((qlnxNestedLoopsJoin *) sExecNode->mExecNode)->mCommonInfo.mFetchRecordStat.mTotalCount )
          : 0 );
    sCurPlanText->mFetchCallCount =
        ((qlnxNestedLoopsJoin *) sExecNode->mExecNode)->mCommonInfo.mFetchCallCount;

    
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

    if( sOptNestedLoopsJoin->mJoinColList->mCount > 0 )
    {
        STL_TRY( qleAppendStringToExplainText( aRegionMem,
                                               &(sCurPredText->mLineBuffer),
                                               aEnv,
                                               "JOINED COLUMNS : " )
                 == STL_SUCCESS );

        STL_TRY( qleSetExplainExprList( aRegionMem,
                                        &(sCurPredText->mLineBuffer),
                                        sOptNestedLoopsJoin->mJoinColList,
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
     * Join Filter 출력
     */

    if( sOptNestedLoopsJoin->mJoinFilterExpr == NULL )
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
                                    sOptNestedLoopsJoin->mJoinFilterExpr,
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
     * SubQuery Filter 출력
     */

    if( sOptNestedLoopsJoin->mSubQueryAndFilterCnt > 0 )
    {
        for( i = 0; i < sOptNestedLoopsJoin->mSubQueryAndFilterCnt; i++ )
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
                                                   "SUBQUERY FILTER : " )
                     == STL_SUCCESS );

            STL_TRY( qleSetExplainExpr( aRegionMem,
                                        &(sCurPredText->mLineBuffer),
                                        sOptNestedLoopsJoin->mSubQueryFilterExprArr[i],
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
    }

 
    /**
     * Where Filter 출력
     */

    if( sOptNestedLoopsJoin->mLogicalFilterExpr == NULL )
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
                                    sOptNestedLoopsJoin->mLogicalFilterExpr,
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

    STL_TRY( qlneExplainNodeList[ sOptNestedLoopsJoin->mLeftChildNode->mType ] (
                 sOptNestedLoopsJoin->mLeftChildNode,
                 aDataArea,
                 aRegionMem,
                 aDepth + 1,
                 aPlanText,
                 aEnv )
             == STL_SUCCESS );

    STL_TRY( qlneExplainNodeList[ sOptNestedLoopsJoin->mRightChildNode->mType ] (
                 sOptNestedLoopsJoin->mRightChildNode,
                 aDataArea,
                 aRegionMem,
                 aDepth + 1,
                 aPlanText,
                 aEnv )
             == STL_SUCCESS );


    /**********************************************************
     * SubQuery Filter관련 Node 정보 출력
     *********************************************************/

    /* Statement단위 처리의 SubQuery Node 정보 출력 */
    if( sOptNestedLoopsJoin->mStmtSubQueryNode != NULL )
    {
        STL_TRY( qlneExplainNodeList[ sOptNestedLoopsJoin->mStmtSubQueryNode->mType ] (
                     sOptNestedLoopsJoin->mStmtSubQueryNode,
                     aDataArea,
                     aRegionMem,
                     aDepth + 1,
                     aPlanText,
                     aEnv )
                 == STL_SUCCESS );
    }

    /* SubQuery Filter 관련 Node들의 정보 출력 */
    if( sOptNestedLoopsJoin->mSubQueryAndFilterCnt > 0 )
    {
        for( i = 0; i < sOptNestedLoopsJoin->mSubQueryAndFilterCnt; i++ )
        {
            STL_TRY( qlneExplainNodeList[ sOptNestedLoopsJoin->mSubQueryAndFilterNodeArr[i]->mType ] (
                         sOptNestedLoopsJoin->mSubQueryAndFilterNodeArr[i],
                         aDataArea,
                         aRegionMem,
                         aDepth + 1,
                         aPlanText,
                         aEnv )
                     == STL_SUCCESS );
        }
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
