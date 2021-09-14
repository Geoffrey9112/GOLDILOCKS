/*******************************************************************************
 * qlnoConcat.c
 *
 * Copyright (c) 2013, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlnoConcat.c 7616 2013-03-18 02:57:54Z xcom73 $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlnoConcat.c
 * @brief SQL Processor Layer Code Optimization NODE - CONCAT
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlno
 * @{
 */


/****************************************************************************
 * Complete Building Optimization Node Functions
 ***************************************************************************/

/**
 * @brief CONCAT 의 Optimization Node 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoCompleteConcat( qllDBCStmt            * aDBCStmt,
                              qllStatement          * aSQLStmt,
                              qllOptNodeList        * aOptNodeList,
                              qllOptimizationNode   * aOptNode,
                              qloInitExprList       * aQueryExprList,
                              qllEnv                * aEnv )
{
    qlnoConcat          * sOptConcat            = NULL;

    stlInt32              i;
    
    
    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_CONCAT_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );    


    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/

    sOptConcat = (qlnoConcat *) aOptNode->mOptNode;


    /****************************************************************
     * 하위 node에 대한 Complete Node 수행
     ****************************************************************/

    for( i = 0; i < sOptConcat->mChildCount; i++ )
    {
        QLNO_COMPLETE( aDBCStmt,
                       aSQLStmt,
                       aOptNodeList,
                       sOptConcat->mChildNodeArr[i],
                       aQueryExprList,
                       aEnv );
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief CONCAT의 All Expr 리스트 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoAddExprConcat( qllDBCStmt            * aDBCStmt,
                             qllStatement          * aSQLStmt,
                             qllOptNodeList        * aOptNodeList,
                             qllOptimizationNode   * aOptNode,
                             qloInitExprList       * aQueryExprList,
                             qllEnv                * aEnv )
{
    qlvRefExprItem        * sRefExprItem        = NULL;
    qlnoConcat            * sOptConcat          = NULL;
    stlInt32                i;
    

    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_CONCAT_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );    


    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/
    
    sOptConcat = (qlnoConcat *) aOptNode->mOptNode;


    /****************************************************************
     * Child Node의 Add Expression 수행
     ****************************************************************/

    for( i = 0; i < sOptConcat->mChildCount; i++ )
    {
        QLNO_ADD_EXPR( aDBCStmt,
                       aSQLStmt,
                       aOptNodeList,
                       sOptConcat->mChildNodeArr[i],
                       aQueryExprList,
                       aEnv );
    }


    /**
     * Add Expr Concat Column
     */

    if( sOptConcat->mConcatColList != NULL )
    {
        sRefExprItem = sOptConcat->mConcatColList->mHead;
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


    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlno */

