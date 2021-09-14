/*******************************************************************************
 * qlnoIndexCombine.c
 *
 * Copyright (c) 2013, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlnoIndexCombine.c 7616 2013-03-18 02:57:54Z xcom73 $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlnoIndexCombine.c
 * @brief SQL Processor Layer Code Optimization HINT - INDEX_COMBINE
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
 * @brief Index Combine에 대한 Code Optimize를 수행한다.
 * @param[in]   aCodeOptParamInfo   Code Optimize Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlnoCodeOptimizeIndexCombine( qloCodeOptParamInfo * aCodeOptParamInfo,
                                        qllEnv              * aEnv )
{
    stlInt32                  i;
    qlncPlanIndexCombine    * sPlanIndexCombine     = NULL;
    qllOptimizationNode     * sConcatOptNode        = NULL;
    qlnoConcat              * sOptConcat            = NULL;
    qllEstimateCost         * sOptCost              = NULL;

    qlvRefExprList          * sReadColumnList       = NULL;
    

    /*
     * Paramter Validation
     */

    QLO_CODE_OPT_DEFAULT_PARAM_VALIDATE( aCodeOptParamInfo, aEnv );
    STL_PARAM_VALIDATE( aCodeOptParamInfo->mCandPlan->mPlanType == QLNC_PLAN_TYPE_INDEX_COMBINE,
                        QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Index Combine Plan 정보 획득
     ****************************************************************/

    sPlanIndexCombine = (qlncPlanIndexCombine*)(aCodeOptParamInfo->mCandPlan);


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


    STL_DASSERT( sPlanIndexCombine->mChildCount > 0 );

    sOptConcat->mChildCount = sPlanIndexCombine->mChildCount;
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
        aCodeOptParamInfo->mCandPlan = sPlanIndexCombine->mChildPlanArr[i];

        QLNO_CODE_OPTIMIZE( aCodeOptParamInfo, aEnv );

        sOptConcat->mChildNodeArr[i] = aCodeOptParamInfo->mOptNode;
    }


    /**
     * Index Combine의 Read Column List 구성
     */

    sReadColumnList = sPlanIndexCombine->mReadColList;


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

    
    /**
     * Statement Expression List에 Read RowId Column 추가
     */

    if( sPlanIndexCombine->mRowidColumn != NULL )
    {
        STL_TRY( qlvAddExprToRefExprList(
                     aCodeOptParamInfo->mQueryExprList->mStmtExprList->mRowIdColumnExprList,
                     sPlanIndexCombine->mRowidColumn,
                     STL_FALSE,
                     QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /* Do Nothing */
    }

    
    /****************************************************************
     * Optimize 정보 연결
     ****************************************************************/

    /**
     * Row Block이 필요한지 여부
     */

    sOptConcat->mNeedRowBlock = sPlanIndexCombine->mCommon.mNeedRowBlock;


    /**
     * Base Table Handle
     */

    sOptConcat->mTableHandle = sPlanIndexCombine->mTableHandle;


    /**
     * Column List
     */

    sOptConcat->mConcatColList = sReadColumnList;

    
    /**
     * Outer Column 관련
     */

    sOptConcat->mOuterColumnList = sPlanIndexCombine->mOuterColumnList;


    /**
     * RowId Expression 관련
     */
    
    sOptConcat->mRowIdColExpr = sPlanIndexCombine->mRowidColumn;


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

