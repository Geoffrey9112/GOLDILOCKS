/*******************************************************************************
 * qlnoAggregation.c
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
 * @file qlnoAggregation.c
 * @brief SQL Processor Layer Code Optimization Node - HASH AGGREGATION
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlno
 * @{
 */

stlStatus qloCodeOptimizeHashAggregation( qllDBCStmt              * aDBCStmt,
                                          qllStatement            * aSQLStmt,
                                          qllOptimizationNode     * aOptQueryNode,
                                          qllOptimizationNode     * aOptChildNode,
                                          qlvAggregation          * aInitNode,
                                          qloInitExprList         * aQueryExprList,
                                          qllOptNodeList          * aOptNodeList,
                                          qllOptimizationNode    ** aOptNode,
                                          qllEstimateCost         * aOptCost,
                                          qllEnv                  * aEnv )
{
    qllOptimizationNode  * sOptNode     = NULL;
    qlnoAggregation      * sOptAggrNode = NULL;
    qllEstimateCost      * sOptCost     = NULL;

    
    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptQueryNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptChildNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptCost != NULL, QLL_ERROR_STACK(aEnv) );

    
    /****************************************************************
     * Optimization Node 생성
     ****************************************************************/

    /**
     * QUERY SPEC Optimization Node 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlnoAggregation ),
                                (void **) & sOptAggrNode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Optimization Cost 공간 할당
     */

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( qllEstimateCost ),
                                (void **) & sOptCost,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sOptCost, 0x00, STL_SIZEOF( qllEstimateCost ) );


    /**
     * Common Optimization Node 생성
     */

    STL_TRY( qlnoCreateOptNode( aOptNodeList,
                                QLL_PLAN_NODE_HASH_AGGREGATION_TYPE,
                                sOptAggrNode,
                                aOptQueryNode,
                                sOptCost,
                                NULL,
                                & sOptNode,
                                QLL_CODE_PLAN( aDBCStmt ),
                                aEnv )
             == STL_SUCCESS );

    
    /****************************************************************
     * Distinct 조건을 포함 여부로 Aggregation 구분
     ****************************************************************/

    STL_DASSERT( aInitNode->mAggrExprList->mCount > 0 );
    
    STL_TRY( qlnfClassifyAggrFunc( aInitNode->mAggrExprList,
                                   & sOptAggrNode->mAggrFuncList,
                                   & sOptAggrNode->mAggrDistFuncList,
                                   QLL_CODE_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    STL_DASSERT( sOptAggrNode->mAggrFuncList != NULL );
    STL_DASSERT( sOptAggrNode->mAggrDistFuncList != NULL );
    
    sOptAggrNode->mAggrFuncCnt     = sOptAggrNode->mAggrFuncList->mCount;
    sOptAggrNode->mAggrDistFuncCnt = sOptAggrNode->mAggrDistFuncList->mCount;


    /****************************************************************
     * AGGREGATION Optimization Node 정보 설정
     ****************************************************************/

    /* Child Node */
    sOptAggrNode->mChildNode = aOptChildNode;

    /* Aggregation 관련 */
    sOptAggrNode->mAggregation = aInitNode;
    

    /****************************************************************
     * OUTPUT 설정
     ****************************************************************/

    /**
     * 결과 Common Optimization Node 설정
     */

    *aOptNode = sOptNode;

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief HASH AGGREGATION의 Optimization Node 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoCompleteHashAggregation( qllDBCStmt            * aDBCStmt,
                                       qllStatement          * aSQLStmt,
                                       qllOptNodeList        * aOptNodeList,
                                       qllOptimizationNode   * aOptNode,
                                       qloInitExprList       * aQueryExprList,
                                       qllEnv                * aEnv )
{
    qlnoAggregation  * sOptAggrNode = NULL;
    
    
    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_HASH_AGGREGATION_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );    


    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/

    sOptAggrNode = (qlnoAggregation *) aOptNode->mOptNode;
    

    /****************************************************************
     * 하위 node에 대한 Complete Node 수행
     ****************************************************************/

    QLNO_COMPLETE( aDBCStmt,
                   aSQLStmt,
                   aOptNodeList,
                   sOptAggrNode->mChildNode,
                   aQueryExprList,
                   aEnv );

    
    /****************************************************************
     * Aggregation 에 대한 Expression Stack 구성
     ****************************************************************/

    /**
     * Aggregation Function 정보 구축
     */

    if( sOptAggrNode->mAggrFuncCnt > 0 )
    {
        STL_TRY( qlnfGetAggrOptInfo( aSQLStmt,
                                     aQueryExprList,
                                     sOptAggrNode->mAggrFuncCnt,
                                     sOptAggrNode->mAggrFuncList,
                                     & sOptAggrNode->mAggrOptInfo,
                                     QLL_CODE_PLAN( aDBCStmt ),
                                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sOptAggrNode->mAggrOptInfo = NULL;
    }


    /****************************************************************
     * Aggregation Distinct에 대한 Expression Stack 구성
     ****************************************************************/

    /**
     * Aggregation Function 정보 구축
     */

    if( sOptAggrNode->mAggrDistFuncCnt > 0 )
    {
        STL_TRY( qlnfGetAggrOptInfo( aSQLStmt,
                                     aQueryExprList,
                                     sOptAggrNode->mAggrDistFuncCnt,
                                     sOptAggrNode->mAggrDistFuncList,
                                     & sOptAggrNode->mAggrDistOptInfo,
                                     QLL_CODE_PLAN( aDBCStmt ),
                                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sOptAggrNode->mAggrDistOptInfo = NULL;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief HASH AGGREGATION의 All Expr 리스트 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoAddExprHashAggregation( qllDBCStmt            * aDBCStmt,
                                      qllStatement          * aSQLStmt,
                                      qllOptNodeList        * aOptNodeList,
                                      qllOptimizationNode   * aOptNode,
                                      qloInitExprList       * aQueryExprList,
                                      qllEnv                * aEnv )
{
    qlnoAggregation  * sOptAggrNode = NULL;
    qlvRefExprItem   * sRefExprItem = NULL;

    
    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_HASH_AGGREGATION_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );    


    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/
    
    sOptAggrNode = (qlnoAggregation *) aOptNode->mOptNode;


    /****************************************************************
     * Child Node의 Add Expression 수행
     ****************************************************************/

    QLNO_ADD_EXPR( aDBCStmt,
                   aSQLStmt,
                   aOptNodeList,
                   sOptAggrNode->mChildNode,
                   aQueryExprList,
                   aEnv );


    /****************************************************************
     * Add Expr
     ****************************************************************/

    /**
     * Add Expr Aggregation Functions
     */

    if( sOptAggrNode->mAggrFuncCnt > 0 )
    {
        sRefExprItem = sOptAggrNode->mAggrFuncList->mHead;

        while( sRefExprItem != NULL )
        {
            STL_TRY( qloAddExpr( sRefExprItem->mExprPtr,
                                 aQueryExprList->mStmtExprList->mAllExprList,
                                 QLL_CODE_PLAN( aDBCStmt ),
                                 aEnv )
                     == STL_SUCCESS );
            
            sRefExprItem = sRefExprItem->mNext;
        }
    }
    else
    {
        /* Do Nothing */
    }

    
    /**
     * Add Expr Aggregation Distinct Functions
     */

    if( sOptAggrNode->mAggrDistFuncCnt > 0 )
    {
        sRefExprItem = sOptAggrNode->mAggrDistFuncList->mHead;

        while( sRefExprItem != NULL )
        {
            STL_TRY( qloAddExpr( sRefExprItem->mExprPtr,
                                 aQueryExprList->mStmtExprList->mAllExprList,
                                 QLL_CODE_PLAN( aDBCStmt ),
                                 aEnv )
                     == STL_SUCCESS );

            sRefExprItem = sRefExprItem->mNext;
        }
    }
    else
    {
        /* Do Nothing */
    }
    

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlno */
