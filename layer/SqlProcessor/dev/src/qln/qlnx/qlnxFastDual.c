/*******************************************************************************
 * qlnxFastDual.c
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
 * @file qlnxFastDual.c
 * @brief SQL Processor Layer Execution Node - FAST DUAL
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnx
 * @{
 */

#if 0
/**
 * @brief FAST DUAL node를 초기화한다.
 * @param[in]      aTransID      Transaction ID
 * @param[in]      aStmt         Statement
 * @param[in]      aDBCStmt      DBC Statement
 * @param[in,out]  aSQLStmt      SQL Statement
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 *
 * @remark Data Optimization 단계에서 수행한다.
 *    <BR> Execution Node를 생성하고, Execution을 위한 정보를 구축한다.
 *    <BR> 공간 할당시 Data Plan 메모리 관리자를 이용한다.
 */
stlStatus qlnxInitializeFastDualAccess( smlTransId              aTransID,
                                        smlStatement          * aStmt,
                                        qllDBCStmt            * aDBCStmt,
                                        qllStatement          * aSQLStmt,
                                        qllOptimizationNode   * aOptNode,
                                        qllDataArea           * aDataArea,
                                        qllEnv                * aEnv )
{
    qllExecutionNode    * sExecNode = NULL;
    /* qlnoFastDualAccess  * sOptFastDual = NULL; */
    qlnxFastDualAccess  * sExeFastDual = NULL;
    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_FAST_DUAL_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * FAST DUAL Optimization Node 획득
     */

    /* sOptFastDual = (qlnoFastDual *) aOptNode->mOptNode; */


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea,
                                        QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * FAST DUAL Execution Node 할당
     */

    /* STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ), */
    /*                             STL_SIZEOF( qlnxFastDualAccess ), */
    /*                             (void **) & sExeFastDual, */
    /*                             KNL_ENV(aEnv) ) */
    /*          == STL_SUCCESS ); */
    /* stlMemset( sExeFastDual, 0x00, STL_SIZEOF( qlnxFastDualAccess ) ); */


    /**
     * FAST DUAL Execution Node 정보 초기화
     */
    
    /* Do Nothing */
    
    
    /**
     * Common Execution Node 정보 설정
     */

    sExecNode->mNodeType  = QLL_PLAN_NODE_FAST_DUAL_ACCESS_TYPE;

    sExecNode->mOptNode   = aOptNode;
    sExecNode->mExecNode  = sExeFastDual;

    if( aDataArea->mIsEvaluateCost == STL_TRUE )
    {
        /**
         * @todo Data Opt 단계에서 할당 받기
         */
        
        STL_TRY( knlAllocRegionMem( & QLL_EXE_DML_MEM( aDBCStmt ),
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


/**
 * @brief FAST DUAL node를 수행한다.
 * @param[in]      aTransID      Transaction ID
 * @param[in]      aStmt         Statement
 * @param[in]      aDBCStmt      DBC Statement
 * @param[in,out]  aSQLStmt      SQL Statement
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 *
 * @remark Execution 첫번째 수행시 호출한다.
 */
stlStatus qlnxExecuteFastDualAccess( smlTransId              aTransID,
                                     smlStatement          * aStmt,
                                     qllDBCStmt            * aDBCStmt,
                                     qllStatement          * aSQLStmt,
                                     qllOptimizationNode   * aOptNode,
                                     qllDataArea           * aDataArea,
                                     qllEnv                * aEnv )
{
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_FAST_DUAL_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /* Do Nothing */
    
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief FAST DUAL node를 Fetch한다.
 * @param[in]      aTransID      Transaction ID
 * @param[in]      aStmt         Statement
 * @param[in]      aDBCStmt      DBC Statement
 * @param[in,out]  aSQLStmt      SQL Statement
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aStartIdx     Start Index
 * @param[in]      aSkipCnt      Skip Record Count
 * @param[in]      aFetchCnt     Fetch Record Count
 * @param[out]     aUsedBlockCnt Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF          End Of Fetch 여부
 * @param[in]      aEnv          Environment
 *
 * @remark Fetch 수행시 또는 반복되는 Execution 수행시 호출된다.
 */
stlStatus qlnxFetchFastDualAccess( smlTransId              aTransID,
                                   smlStatement          * aStmt,
                                   qllDBCStmt            * aDBCStmt,
                                   qllStatement          * aSQLStmt,
                                   qllOptimizationNode   * aOptNode,
                                   qllDataArea           * aDataArea,
                                   stlInt32                aStartIdx,
                                   stlInt64                aSkipCnt,
                                   stlInt64                aFetchCnt,
                                   stlInt64              * aUsedBlockCnt,
                                   stlBool               * aEOF,
                                   qllEnv                * aEnv )
{
    /* qllExecutionNode      * sExecNode = NULL; */
    /* qlnoFastDualAccess    * sOptFastDual = NULL; */
    /* qlnxFastDualAccess    * sExeFastDual = NULL; */
    /* qlnoQuerySpec         * sOptQuerySpec = NULL; */
    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_FAST_DUAL_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    
    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * FAST DUAL Optimization Node 획득
     */

    /* sOptFastDual = (qlnoFastDual *) aOptNode->mOptNode; */

    
    /**
     * Common Execution Node 획득
     */
        
    /* sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) ); */


    /**
     * FAST DUAL Execution Node 획득
     */

    /* sExeFastDual = sExecNode->mExecNode; */


    /**
     * 해당 노드를 포함하는 Query Expression Optimization Node 획득
     */
    
    /* STL_PARAM_VALIDATE( aOptNode->mOptCurQueryNode != NULL, */
    /*                     QLL_ERROR_STACK(aEnv) ); */
            
    /* sOptQuerySpec = (qlnoQuerySpec *) aOptNode->mOptCurQueryNode->mOptNode; */

    
    /*****************************************
     * Fetch
     ****************************************/

    /**
     * OUTPUT 설정
     */
    
    /* Used Block Count 설정 */
    if( ( aSkipCnt > 0 ) || ( aFetchCnt <= 0 ) )
    {
        *aUsedBlockCnt = 0;
    }
    else
    {
        *aUsedBlockCnt = 1;
    }
    
    *aEOF = STL_TRUE;
    
    /* sExeFastDual->mCommonInfo.mFetchRowCnt += *aUsedBlockCnt; */
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief FAST DUAL node 수행을 종료한다.
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 *
 * @remark Execution을 종료하고자 할 때 호출한다.
 */
stlStatus qlnxFinalizeFastDualAccess( qllOptimizationNode   * aOptNode,
                                      qllDataArea           * aDataArea,
                                      qllEnv                * aEnv )
{
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_FAST_DUAL_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    /* Do Nothing */

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
#endif

/** @} qlnx */
