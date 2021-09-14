/*******************************************************************************
 * qlnoJoinCombine.c
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
 * @file qlnoJoinCombine.c
 * @brief SQL Processor Code Layer Optimization - JOIN_COMBINE
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlno
 * @{
 */


/****************************************************************************
 * Node Optimization Function
 ***************************************************************************/

/**
 * @brief Join Combine에 대한 Code Optimize를 수행한다.
 * @param[in]   aCodeOptParamInfo   Code Optimize Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlnoCodeOptimizeJoinCombine( qloCodeOptParamInfo  * aCodeOptParamInfo,
                                       qllEnv               * aEnv )
{
    stlInt32              i;
    qlncPlanJoinCombine * sPlanJoinCombine  = NULL;
    qllOptimizationNode * sConcatOptNode    = NULL;
    qlnoConcat          * sOptConcat        = NULL;
    qllEstimateCost     * sOptCost          = NULL;

    qlvRefExprList      * sReadColumnList   = NULL;
    

    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLO_CODE_OPT_DEFAULT_PARAM_VALIDATE( aCodeOptParamInfo, aEnv );
    STL_PARAM_VALIDATE( aCodeOptParamInfo->mCandPlan->mPlanType == QLNC_PLAN_TYPE_JOIN_COMBINE,
                        QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Join Combine Plan 정보 획득
     ****************************************************************/

    sPlanJoinCombine = (qlncPlanJoinCombine*)(aCodeOptParamInfo->mCandPlan);


    /****************************************************************
     * Optimization Node 생성
     ****************************************************************/

    /**
     * CONCAT Optimization Node 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                STL_SIZEOF( qlnoConcat ),
                                (void **) &sOptConcat,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sOptConcat, 0x00, STL_SIZEOF( qlnoConcat ) );


    /**
     * Optimization Cost 공간 할당
     */

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                STL_SIZEOF( qllEstimateCost ),
                                (void **) &sOptCost,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sOptCost, 0x00, STL_SIZEOF( qllEstimateCost ) );


    /**
     * Common Optimization Node 생성
     */

    STL_TRY( qlnoCreateOptNode( aCodeOptParamInfo->mOptNodeList,
                                QLL_PLAN_NODE_CONCAT_TYPE,
                                sOptConcat,
                                aCodeOptParamInfo->mOptQueryNode,  /* Query Node */
                                sOptCost,
                                NULL,
                                &sConcatOptNode,
                                QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                aEnv )
             == STL_SUCCESS );


    STL_DASSERT( sPlanJoinCombine->mChildCount > 0 );

    sOptConcat->mChildCount = sPlanJoinCombine->mChildCount;
    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                STL_SIZEOF( qllOptimizationNode *) * sOptConcat->mChildCount,
                                (void **) &sOptConcat->mChildNodeArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /****************************************************************
     * Optimization Node 생성
     ****************************************************************/

    /**
     * Child Node들에 대한 Optimization Node 생성
     */

    for( i = 0; i < sOptConcat->mChildCount; i++ )
    {
        qllOptimizationNode * sOptNode  = NULL;

        aCodeOptParamInfo->mCandPlan = sPlanJoinCombine->mChildPlanArr[i];

        QLNO_CODE_OPTIMIZE( aCodeOptParamInfo, aEnv );

        sOptConcat->mChildNodeArr[i] = aCodeOptParamInfo->mOptNode;

        sOptNode = sOptConcat->mChildNodeArr[i];
        while( STL_TRUE )
        {
            /* coverage : cost 평가에 의해 node 구조가 변경되기 때문에 test case 구축이 어려움.
             *            해당 코드는 수행될 수 있다. */
            if( sOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_LIST_TYPE )
            {
                sOptNode = ((qlnoSubQueryList*)sOptNode->mOptNode)->mChildNode;
            }
            else if( sOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_FILTER_TYPE )
            {
                sOptNode = ((qlnoSubQueryFilter*)sOptNode->mOptNode)->mChildNode;
                STL_DASSERT( sOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_LIST_TYPE );
                sOptNode = ((qlnoSubQueryList*)sOptNode->mOptNode)->mChildNode;
            }
            else
            {
                break;
            }
        }

        /* Child Node가 Join 노드일 경우 Read Column List를 재정렬 */
        if( sOptNode->mType == QLL_PLAN_NODE_NESTED_LOOPS_TYPE )
        {
            if( ((qlnoNestedLoopsJoin*)(sOptNode->mOptNode))->mJoinColList != NULL )
            {
                STL_TRY( qlnoAdjustJoinReadColumnOrder(
                              &QLL_CANDIDATE_MEM( aEnv ),
                              ((qlnoNestedLoopsJoin*)(sOptNode->mOptNode))->mJoinColList,
                              aEnv )
                         == STL_SUCCESS );
            }
        }
        else if( sOptNode->mType == QLL_PLAN_NODE_MERGE_JOIN_TYPE )
        {
            if( ((qlnoSortMergeJoin*)(sOptNode->mOptNode))->mJoinColList != NULL )
            {
                STL_TRY( qlnoAdjustJoinReadColumnOrder(
                              &QLL_CANDIDATE_MEM( aEnv ),
                              ((qlnoSortMergeJoin*)(sOptNode->mOptNode))->mJoinColList,
                              aEnv )
                         == STL_SUCCESS );
            }
        }
        else if( sOptNode->mType == QLL_PLAN_NODE_HASH_JOIN_TYPE )
        {
            if( ((qlnoHashJoin*)(sOptNode->mOptNode))->mJoinColList != NULL )
            {
                STL_TRY( qlnoAdjustJoinReadColumnOrder(
                              &QLL_CANDIDATE_MEM( aEnv ),
                              ((qlnoHashJoin*)(sOptNode->mOptNode))->mJoinColList,
                              aEnv )
                         == STL_SUCCESS );
            }
        }
        else
        {
            STL_DASSERT( 0 );
        }
    }


    /**
     * Join Combine의 Read Column List 구성
     */

    sReadColumnList = sPlanJoinCombine->mReadColList;

    /* Read Column List를 재정렬 */
    if( sReadColumnList != NULL )
    {
        STL_TRY( qlnoAdjustJoinReadColumnOrder( &QLL_CANDIDATE_MEM( aEnv ),
                                                sReadColumnList,
                                                aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Statement Expression List에 Read Column 추가
     */

    if( sReadColumnList != NULL )
    {
        STL_TRY( qlvAddExprListToRefExprList(
                     aCodeOptParamInfo->mQueryExprList->mStmtExprList->mColumnExprList,
                     sReadColumnList,
                     STL_FALSE,
                     QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( qlvCreateRefExprList( & sReadColumnList,
                                       QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                       aEnv )
                 == STL_SUCCESS );
    }

    
    /****************************************************************
     * Optimize 정보 연결
     ****************************************************************/

    /**
     * Row Block이 필요한지 여부
     */

    sOptConcat->mNeedRowBlock = sPlanJoinCombine->mCommon.mNeedRowBlock;


    /**
     * Base Table Handle
     */

    sOptConcat->mTableHandle = NULL;


    /**
     * Column List
     */

    sOptConcat->mConcatColList = sReadColumnList;

    
    /**
     * Outer Column 관련
     */

    sOptConcat->mOuterColumnList = sPlanJoinCombine->mOuterColumnList;

    
    /**
     * RowId Expression 관련 : 하위 RowId 를 참조할 수 없다.
     */
    
    sOptConcat->mRowIdColExpr = NULL;


    /****************************************************************
     * 마무리
     ****************************************************************/

    /**
     * Output 설정
     */

    aCodeOptParamInfo->mOptNode = sConcatOptNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlno */

