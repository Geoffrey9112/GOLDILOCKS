/*******************************************************************************
 * qlndQuerySpec.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlndQuerySpec.c 10973 2014-01-16 07:17:19Z bsyou $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlndQuerySpec.c
 * @brief SQL Processor Layer Data Optimization Node - QUERY SPEC
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnd
 * @{
 */
   

/**
 * @brief QUERY SPEC Execution node에 대한 Data 정보를 구축한다.
 * @param[in]      aTransID      Transaction ID
 * @param[in]      aDBCStmt      DBC Statement
 * @param[in,out]  aSQLStmt      SQL Statement
 * @param[in]      aOptNode      Optimization Node
 * @param[in,out]  aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 *
 * @remark Data Optimization 단계에서 수행한다.
 *    <BR> Execution Node를 생성하고, Execution을 위한 정보를 구축한다.
 *    <BR> 공간 할당시 Data Plan 메모리 관리자를 이용한다.
 */
stlStatus qlndDataOptimizeQuerySpec( smlTransId              aTransID,
                                     qllDBCStmt            * aDBCStmt,
                                     qllStatement          * aSQLStmt,
                                     qllOptimizationNode   * aOptNode,
                                     qllDataArea           * aDataArea,
                                     qllEnv                * aEnv )
{
    qllExecutionNode  * sExecNode     = NULL;
    qlnoQuerySpec     * sOptQuerySpec = NULL;
    qlnxQuerySpec     * sExeQuerySpec = NULL;

    
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_QUERY_SPEC_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * QUERY EXPR Optimization Node 획득
     */

    sOptQuerySpec = (qlnoQuerySpec *) aOptNode->mOptNode;


    /**
     * 하위 node에 대한 Data Optimize 수행
     */

    QLND_DATA_OPTIMIZE( aTransID,
                        aDBCStmt,
                        aSQLStmt,
                        sOptQuerySpec->mChildNode,
                        aDataArea,
                        aEnv );


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea,
                                        QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * QUERY EXPR Execution Node 할당
     */

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlnxQuerySpec ),
                                (void **) & sExeQuerySpec,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sExeQuerySpec, 0x00, STL_SIZEOF( qlnxQuerySpec ) );


    /**
     * QUERY EXPR Execution Node 정보 초기화
     */
    
    /* Target Column List 구성 (share) */
    STL_DASSERT( sOptQuerySpec->mTargetColList->mCount > 0 );

    STL_TRY( qlndBuildRefBlockList( aSQLStmt,
                                    aDataArea,
                                    NULL,  /* statement expression list */
                                    sOptQuerySpec->mTargetColList,
                                    & sExeQuerySpec->mTargetColBlock,
                                    & QLL_DATA_PLAN( aDBCStmt ),
                                    aEnv )
             == STL_SUCCESS );

    
    /**
     * Pseudo Column List 구성 (share)
     */

    if( sOptQuerySpec->mQueryExprList->mInitExprList->mPseudoColExprList->mCount > 0 )
    {
        STL_TRY( qlndBuildPseudoColBlockList( aDataArea,
                                              sOptQuerySpec->mQueryExprList->mInitExprList->mPseudoColExprList,
                                              & sExeQuerySpec->mPseudoColBlock,
                                              & QLL_DATA_PLAN( aDBCStmt ),
                                              aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sExeQuerySpec->mPseudoColBlock = NULL;
    }


    /**
     * Common Info 설정
     */

    sExeQuerySpec->mCommonInfo.mResultColBlock = sExeQuerySpec->mTargetColBlock;
    sExeQuerySpec->mCommonInfo.mRowBlockList =
        ((qlnxCommonInfo*)(aDataArea->mExecNodeList[sOptQuerySpec->mChildNode->mIdx].mExecNode))->mRowBlockList;
    sExeQuerySpec->mCommonInfo.mOuterColBlock = NULL;
    sExeQuerySpec->mCommonInfo.mOuterOrgColBlock = NULL;


    /**
     * Common Execution Node 정보 설정
     */

    sExecNode->mNodeType  = QLL_PLAN_NODE_QUERY_SPEC_TYPE;

    sExecNode->mOptNode   = aOptNode;
    sExecNode->mExecNode  = sExeQuerySpec;

    if( aDataArea->mIsEvaluateCost == STL_TRUE )
    {
        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                    STL_SIZEOF( qllExecutionCost ),
                                    (void **) & sExecNode->mExecutionCost,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemset( sExecNode->mExecutionCost, 0x00, STL_SIZEOF( qllExecutionCost ) );
    }
    else
    {
        sExecNode->mExecutionCost  = NULL;
    }


    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/** @} qlnd */
