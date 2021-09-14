/*******************************************************************************
 * qlneQuerySet.c
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
 * @file qlneQuerySet.c
 * @brief SQL Processor Layer Explain Node - QUERY SET
 */

#include <qll.h>
#include <qlDef.h>

/**
 * @addtogroup qlne
 * @{
 */


/**
 * @brief Query Set node에 대한 수행 정보를 출력한다.
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area
 * @param[in]      aRegionMem    Region Memory Manager
 * @param[in]      aDepth        Node Depth
 * @param[in,out]  aPlanText     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 */
stlStatus qlneExplainQuerySet( qllOptimizationNode   * aOptNode,
                               qllDataArea           * aDataArea,
                               knlRegionMem          * aRegionMem,
                               stlUInt32               aDepth,
                               qllExplainNodeText   ** aPlanText,
                               qllEnv                * aEnv )
{
    qlnoQuerySet        * sOptQuerySet  = NULL;

    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_QUERY_SET_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDepth >= 0, QLL_ERROR_STACK(aEnv) );
    
    
    /**********************************************************
     * Query Set Node 정보 출력
     *********************************************************/

    sOptQuerySet = (qlnoQuerySet *) aOptNode->mOptNode;

    
    /**********************************************************
     * Child Node 정보 출력
     *********************************************************/

    /**
     * Child Node 정보 출력
     */

    STL_TRY( qlneExplainNodeList[ sOptQuerySet->mChildNode->mType ] (
                 sOptQuerySet->mChildNode,
                 aDataArea,
                 aRegionMem,
                 aDepth,
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



/**
 * @brief Set OP node에 대한 수행 정보를 출력한다.
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area
 * @param[in]      aRegionMem    Region Memory Manager
 * @param[in]      aDepth        Node Depth
 * @param[in,out]  aPlanText     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 */
stlStatus qlneExplainSetOP( qllOptimizationNode   * aOptNode,
                            qllDataArea           * aDataArea,
                            knlRegionMem          * aRegionMem,
                            stlUInt32               aDepth,
                            qllExplainNodeText   ** aPlanText,
                            qllEnv                * aEnv )
{
    stlInt32              i;
    qlnoSetOP           * sOptSetOP     = NULL;
    qllExecutionNode    * sExecNode     = NULL;
    qllExplainNodeText  * sCurPlanText  = NULL;


    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aOptNode->mType == QLL_PLAN_NODE_UNION_ALL_TYPE) ||
                        (aOptNode->mType == QLL_PLAN_NODE_UNION_DISTINCT_TYPE) ||
                        (aOptNode->mType == QLL_PLAN_NODE_EXCEPT_ALL_TYPE) ||
                        (aOptNode->mType == QLL_PLAN_NODE_EXCEPT_DISTINCT_TYPE) ||
                        (aOptNode->mType == QLL_PLAN_NODE_INTERSECT_ALL_TYPE) ||
                        (aOptNode->mType == QLL_PLAN_NODE_INTERSECT_DISTINCT_TYPE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDepth >= 0, QLL_ERROR_STACK(aEnv) );
    
    
    /**********************************************************
     * SET OP Node 정보 출력
     *********************************************************/

    sOptSetOP = (qlnoSetOP *) aOptNode->mOptNode;
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
    switch ( aOptNode->mType )
    {
        case QLL_PLAN_NODE_UNION_ALL_TYPE:
            STL_TRY( qleSetStringToExplainText( aRegionMem,
                                                &(sCurPlanText->mDescBuffer),
                                                aEnv,
                                                "UNION-ALL" )
                     == STL_SUCCESS );
            break;
        case QLL_PLAN_NODE_UNION_DISTINCT_TYPE:
            STL_TRY( qleSetStringToExplainText( aRegionMem,
                                                &(sCurPlanText->mDescBuffer),
                                                aEnv,
                                                "UNION-DISTINCT" )
                     == STL_SUCCESS );
            break;
        case QLL_PLAN_NODE_EXCEPT_ALL_TYPE:
            STL_TRY( qleSetStringToExplainText( aRegionMem,
                                                &(sCurPlanText->mDescBuffer),
                                                aEnv,
                                                "EXCEPT-ALL" )
                     == STL_SUCCESS );
            break;
        case QLL_PLAN_NODE_EXCEPT_DISTINCT_TYPE:
            STL_TRY( qleSetStringToExplainText( aRegionMem,
                                                &(sCurPlanText->mDescBuffer),
                                                aEnv,
                                                "EXCEPT-DISTINCT" )
                     == STL_SUCCESS );
            break;
        case QLL_PLAN_NODE_INTERSECT_ALL_TYPE:
            STL_TRY( qleSetStringToExplainText( aRegionMem,
                                                &(sCurPlanText->mDescBuffer),
                                                aEnv,
                                                "INTERSECT-ALL" )
                     == STL_SUCCESS );
            break;
        case QLL_PLAN_NODE_INTERSECT_DISTINCT_TYPE:
            STL_TRY( qleSetStringToExplainText( aRegionMem,
                                                &(sCurPlanText->mDescBuffer),
                                                aEnv,
                                                "INTERSECT-DISTINCT" )
                     == STL_SUCCESS );
            break;
        default:
            STL_DASSERT(0);
            break;
    }

    sCurPlanText->mFetchRowCnt =
        ((qlnxSetOP *) sExecNode->mExecNode)->mCommonInfo.mFetchRowCnt;

    sCurPlanText->mBuildTime =
        ((qlnxSetOP *) sExecNode->mExecNode)->mCommonInfo.mBuildTime;
    sCurPlanText->mFetchTime =
        ((qlnxSetOP *) sExecNode->mExecNode)->mCommonInfo.mFetchTime;
    sCurPlanText->mFetchRecordAvgSize =
        ( ((qlnxSetOP *) sExecNode->mExecNode)->mCommonInfo.mFetchRecordStat.mTotalCount > 0
          ? ( ((qlnxSetOP *) sExecNode->mExecNode)->mCommonInfo.mFetchRecordStat.mTotalSize /
              ((qlnxSetOP *) sExecNode->mExecNode)->mCommonInfo.mFetchRecordStat.mTotalCount )
          : 0 );
    sCurPlanText->mFetchCallCount =
        ((qlnxSetOP *) sExecNode->mExecNode)->mCommonInfo.mFetchCallCount;


    /**********************************************************
     * Child Node 정보 출력
     *********************************************************/

    /**
     * Child Node 정보 출력
     */

    for( i = 0; i < sOptSetOP->mChildCount; i++ )
    {
        STL_TRY( qlneExplainNodeList[ sOptSetOP->mChildNodeArray[i]->mType ] (
                     sOptSetOP->mChildNodeArray[i],
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
