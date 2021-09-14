/*******************************************************************************
 * qlndSelectStmt.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlndSelectStmt.c 9998 2013-10-23 06:48:36Z bsyou $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlndSelectStmt.c
 * @brief SQL Processor Layer Data Optimization Node - SELECT STATEMENT Node
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnd
 * @{
 */


/**
 * @brief SELECT STATEMENT Execution node에 대한 Data 정보를 구축한다.
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
stlStatus qlndDataOptimizeSelectStmt( smlTransId              aTransID,
                                      qllDBCStmt            * aDBCStmt,
                                      qllStatement          * aSQLStmt,
                                      qllOptimizationNode   * aOptNode,
                                      qllDataArea           * aDataArea,
                                      qllEnv                * aEnv )
{
    qllExecutionNode  * sExecNode      = NULL;
    qlnoSelectStmt    * sOptSelectStmt = NULL;
    qlnxSelectStmt    * sExeSelectStmt = NULL;
    qlvInitExpression * sExpr          = NULL;
    
    
    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_STMT_SELECT_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * SELECT STATEMENT Optimization Node 획득
     */

    sOptSelectStmt = (qlnoSelectStmt *) aOptNode->mOptNode;


    /**
     * 하위 node에 대한 Data Optimize 수행
     */

    QLND_DATA_OPTIMIZE( aTransID,
                        aDBCStmt,
                        aSQLStmt,
                        sOptSelectStmt->mQueryNode,
                        aDataArea,
                        aEnv );


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea,
                                        QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * SELECT STATEMENT Execution Node 할당
     */

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlnxSelectStmt ),
                                (void **) & sExeSelectStmt,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sExeSelectStmt, 0x00, STL_SIZEOF( qlnxSelectStmt ) );


    /**
     * Pseudo Column List 구성
     */

    if( sOptSelectStmt->mStmtExprList->mInitExprList->mPseudoColExprList->mCount > 0 )
    {
        STL_TRY( qlndBuildPseudoColBlockList( aDataArea,
                                              sOptSelectStmt->mStmtExprList->mInitExprList->mPseudoColExprList,
                                              & sExeSelectStmt->mStmtPseudoColBlock,
                                              & QLL_DATA_PLAN( aDBCStmt ),
                                              aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sExeSelectStmt->mStmtPseudoColBlock = NULL;
    }
    

    /**
     * Constant Expression Result Column 공간 확보
     */
    
    if( sOptSelectStmt->mStmtExprList->mInitExprList->mConstExprList->mCount > 0 )
    {
        sExpr = sOptSelectStmt->mStmtExprList->mInitExprList->mConstExprList->mHead->mExprPtr;
        
        STL_TRY( knlInitShareBlockFromBlock(
                     & aDataArea->mConstResultBlock,
                     aDataArea->mExprDataContext->mContexts[ sExpr->mOffset ].mInfo.mValueInfo,
                     & QLL_DATA_PLAN( aDBCStmt ),
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        aDataArea->mConstResultBlock = NULL;
    }
    


    /**
     * Common Execution Node 정보 설정
     */

    STL_DASSERT( aOptNode->mIdx == 0 );
    
    sExecNode->mNodeType  = QLL_PLAN_NODE_STMT_SELECT_TYPE;
    sExecNode->mOptNode   = aOptNode;
    sExecNode->mExecNode  = sExeSelectStmt;

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
