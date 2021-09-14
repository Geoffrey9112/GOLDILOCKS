/*******************************************************************************
 * qlndAggregation.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlndAggregation.c 9998 2013-10-23 06:48:36Z bsyou $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlndAggregation.c
 * @brief SQL Processor Layer Data Optimization Node - HASH AGGREGATION
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnd
 * @{
 */

/**
 * @brief HASH AGGREGATION Execution node에 대한 Data 정보를 구축한다.
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
 *    <BR> 상위 Query Node를 가지지 않는다.
 */
stlStatus qlndDataOptimizeHashAggregation( smlTransId              aTransID,
                                           qllDBCStmt            * aDBCStmt,
                                           qllStatement          * aSQLStmt,
                                           qllOptimizationNode   * aOptNode,
                                           qllDataArea           * aDataArea,
                                           qllEnv                * aEnv )
{
    qllExecutionNode    * sExecNode       = NULL;
    qlnoAggregation     * sOptAggregation = NULL;
    qlnxAggregation     * sExeAggregation = NULL;
    /* qlnoQuerySpec       * sOptQuerySpec   = NULL; */
    

    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_HASH_AGGREGATION_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    

    /**
     * AGGREGATION Optimization Node 획득
     */

    sOptAggregation = (qlnoAggregation *) aOptNode->mOptNode;


    /**
     * 하위 node에 대한 Data Optimize 수행
     */

    QLND_DATA_OPTIMIZE( aTransID,
                        aDBCStmt,
                        aSQLStmt,
                        sOptAggregation->mChildNode,
                        aDataArea,
                        aEnv );


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea,
                                        QLL_GET_OPT_NODE_IDX( aOptNode ) );

    
    /**
     * AGGREGATION Execution Node 할당
     */

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlnxAggregation ),
                                (void **) & sExeAggregation,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sExeAggregation, 0x00, STL_SIZEOF( qlnxAggregation ) );


    /**
     * Read Row Block 구성
     */

    /* Grouping이 포함된 경우 RowId를 구성할 필요가 없다. */
    STL_DASSERT( ((qlnxCommonInfo*)(aDataArea->mExecNodeList[ sOptAggregation->mChildNode->mIdx ].mExecNode))->mRowBlockList ==
                 NULL );


    /**
     * Common Info 설정
     */

    sExeAggregation->mCommonInfo.mResultColBlock = NULL;
    sExeAggregation->mCommonInfo.mRowBlockList = NULL;
    sExeAggregation->mCommonInfo.mOuterColBlock = NULL;
    sExeAggregation->mCommonInfo.mOuterOrgColBlock = NULL;


    /**
     * Aggregation Functions Execution 정보 설정
     */

    /* sOptQuerySpec = (qlnoQuerySpec *) aOptNode->mOptCurQueryNode->mOptNode; */

    if( sOptAggregation->mAggrFuncCnt > 0 )
    {
        STL_TRY( qlnfGetAggrFuncExecInfo( aSQLStmt,
                                          sOptAggregation->mAggrFuncCnt,
                                          sOptAggregation->mAggrOptInfo,
                                          aDataArea,
                                          & sExeAggregation->mAggrExecInfo,
                                          & QLL_DATA_PLAN( aDBCStmt ),
                                          aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sExeAggregation->mAggrExecInfo = NULL;
    }


    /**
     * Aggregation Distinct Functions Execution 정보 설정
     */
    
    if( sOptAggregation->mAggrDistFuncCnt > 0 )
    {
        STL_TRY( qlnfGetAggrDistFuncExecInfo( aSQLStmt,
                                              sOptAggregation->mAggrDistFuncCnt,
                                              sOptAggregation->mAggrDistOptInfo,
                                              aDataArea,
                                              & sExeAggregation->mAggrDistExecInfo,
                                              & QLL_DATA_PLAN( aDBCStmt ),
                                              aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sExeAggregation->mAggrDistExecInfo = NULL;
    }


    /**
     * Common Execution Node 정보 설정
     */

    sExecNode->mNodeType  = QLL_PLAN_NODE_HASH_AGGREGATION_TYPE;
    sExecNode->mOptNode   = aOptNode;
    sExecNode->mExecNode  = sExeAggregation;

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
