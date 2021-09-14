/*******************************************************************************
 * qlvRewrite.c
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
 * @file qlvRewrite.c
 * @brief SQL Processor Layer Rewrite Expression
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlv
 * @{
 */


/**************************************************************************
 * Rewritting Functions For Expression
 **************************************************************************/

/**
 * @brief Rewritting Expression Function List
 * @remark DB Type이 정해지지 않은 Expression에 대한 Rewrite를 수행한다.
 */
static const qlvRewriteFuncPtr qlvRewriteFuncList[ QLV_EXPR_TYPE_MAX ] = 
{
    qlvRewriteValue,         /**< QLV_EXPR_TYPE_VALUE */
    qlvRewriteBindParam,     /**< QLV_EXPR_TYPE_BIND_PARAM */
    qlvRewriteColumn,        /**< QLV_EXPR_TYPE_COLUMN */
    qlvRewriteFunction,      /**< QLV_EXPR_TYPE_FUNCTION */
    qlvRewriteCast,          /**< QLV_EXPR_TYPE_CAST */
    qlvRewritePseudoColumn,  /**< QLV_EXPR_TYPE_PSEUDO_COLUMN */
    qlvRewritePseudoArgs,    /**< QLV_EXPR_TYPE_PSEUDO_ARGUMENT */
    qlvRewriteConstExpr,     /**< QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT */
    qlvRewriteReference,     /**< QLV_EXPR_TYPE_REFERENCE */
    qlvRewriteSubQuery,      /**< QLV_EXPR_TYPE_SUB_QUERY */
    qlvRewriteInnerColumn,   /**< QLV_EXPR_TYPE_INNER_COLUMN */
    qlvRewriteOuterColumn,   /**< QLV_EXPR_TYPE_OUTER_COLUMN */
    qlvRewriteRowIdColumn,   /**< QLV_EXPR_TYPE_ROWID_COLUMN */
    qlvRewriteAggregation,   /**< QLV_EXPR_TYPE_AGGREGATION */
    qlvRewriteCaseExpr,      /**< QLV_EXPR_TYPE_CASE_EXPR */
    qlvRewriteListFunction,  /**< QLV_EXPR_TYPE_LIST_FUNCTION */
    qlvRewriteListColumn,    /**< QLV_EXPR_TYPE_LIST_COLUMN */
    qlvRewriteRowExpr        /**< QLV_EXPR_TYPE_ROW_EXPR */
    
};


/**
 * @brief Common Rewritting Expression Function
 * @param[in]      aInitExpr        Init Expression Source
 * @param[in]      aParentIterTime  Parent's Iteration Time
 * @param[in,out]  aInitExprList    Init Expression List
 * @param[out]     aInitNewExpr     New Init Expression Source
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qlvRewriteExpr( qlvInitExpression   * aInitExpr,
                          dtlIterationTime      aParentIterTime,
                          qlvInitExprList     * aInitExprList,
                          qlvInitExpression  ** aInitNewExpr,
                          knlRegionMem        * aRegionMem,
                          qllEnv              * aEnv )
{
    return qlvRewriteFuncList[ aInitExpr->mType ] ( aInitExpr,
                                                    aParentIterTime, /* 초기값 : DTL_ITERATION_TIME_FOR_EACH_EXPR */
                                                    aInitExprList,
                                                    aInitNewExpr,
                                                    aRegionMem,
                                                    aEnv );
}


/**
 * @brief Rewritting Expression Function for Value (literal)
 * @param[in]      aInitExpr        Init Expression Source
 * @param[in]      aParentIterTime  Parent's Iteration Time
 * @param[in,out]  aInitExprList    Init Expression List
 * @param[out]     aInitNewExpr     New Init Expression Source
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qlvRewriteValue( qlvInitExpression   * aInitExpr,
                           dtlIterationTime      aParentIterTime,
                           qlvInitExprList     * aInitExprList,
                           qlvInitExpression  ** aInitNewExpr,
                           knlRegionMem        * aRegionMem,
                           qllEnv              * aEnv )
{
    qlvInitExpression  * sInitExpr = NULL;
    
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitNewExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**
     * Constant Expression List에 Expression 추가
     */
    
    STL_TRY( qlvAddConstExpr( aInitExpr,
                              aParentIterTime,
                              aInitExprList,
                              & sInitExpr,
                              aRegionMem,
                              aEnv )
             == STL_SUCCESS );

    
    /**
     * OUTPUT 설정
     */

    *aInitNewExpr = sInitExpr;

    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Rewritting Expression Function for Bind Parameter
 * @param[in]      aInitExpr        Init Expression Source
 * @param[in]      aParentIterTime  Parent's Iteration Time
 * @param[in,out]  aInitExprList    Init Expression List
 * @param[out]     aInitNewExpr     New Init Expression Source
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qlvRewriteBindParam( qlvInitExpression   * aInitExpr,
                               dtlIterationTime      aParentIterTime,
                               qlvInitExprList     * aInitExprList,
                               qlvInitExpression  ** aInitNewExpr,
                               knlRegionMem        * aRegionMem,
                               qllEnv              * aEnv )
{
    qlvInitExpression  * sInitExpr = NULL;
    
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitNewExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**
     * Constant Expression List에 Expression 추가
     */
    
    STL_TRY( qlvAddConstExpr( aInitExpr,
                              aParentIterTime,
                              aInitExprList,
                              & sInitExpr,
                              aRegionMem,
                              aEnv )
             == STL_SUCCESS );

    
    /**
     * OUTPUT 설정
     */

    *aInitNewExpr = sInitExpr;
    
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Rewritting Expression Function for Column
 * @param[in]      aInitExpr        Init Expression Source
 * @param[in]      aParentIterTime  Parent's Iteration Time
 * @param[in,out]  aInitExprList    Init Expression List
 * @param[out]     aInitNewExpr     New Init Expression Source
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qlvRewriteColumn( qlvInitExpression   * aInitExpr,
                            dtlIterationTime      aParentIterTime,
                            qlvInitExprList     * aInitExprList,
                            qlvInitExpression  ** aInitNewExpr,
                            knlRegionMem        * aRegionMem,
                            qllEnv              * aEnv )
{
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitNewExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /* Do Nothing */

    
    /**
     * OUTPUT 설정
     */

    *aInitNewExpr = aInitExpr;

    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Rewritting Expression Function for Built-In Function
 * @param[in]      aInitExpr        Init Expression Source
 * @param[in]      aParentIterTime  Parent's Iteration Time
 * @param[in,out]  aInitExprList    Init Expression List
 * @param[out]     aInitNewExpr     New Init Expression Source
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qlvRewriteFunction( qlvInitExpression   * aInitExpr,
                              dtlIterationTime      aParentIterTime,
                              qlvInitExprList     * aInitExprList,
                              qlvInitExpression  ** aInitNewExpr,
                              knlRegionMem        * aRegionMem,
                              qllEnv              * aEnv )
{
    qlvInitExpression  * sOrgExpr  = NULL;
    qlvInitExpression  * sInitExpr = NULL;
    qlvInitExpression  * sArgExpr  = NULL;
    stlInt32             sArgCount = 0;
    
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitNewExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    

    /**
     * Constant Expression List에 Expression 추가
     */

    STL_TRY( qlvAddConstExpr( aInitExpr,
                              aParentIterTime,
                              aInitExprList,
                              & sInitExpr,
                              aRegionMem,
                              aEnv )
             == STL_SUCCESS );

    if( sInitExpr->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT )
    {
        sOrgExpr = sInitExpr->mExpr.mConstExpr->mOrgExpr;
    }
    else
    {
        sOrgExpr = sInitExpr;
    }


    /**
     * Expression List에 Expression 추가
     */

    /* Function의 Argument Expression들에 대한 Rewrite 수행 */
    for( sArgCount = 0 ;
         sArgCount < sOrgExpr->mExpr.mFunction->mArgCount ;
         sArgCount++ )
    {
        sArgExpr = sOrgExpr->mExpr.mFunction->mArgs[ sArgCount ];

        STL_TRY( qlvRewriteFuncList[ sArgExpr->mType ] ( sArgExpr,
                                                         sOrgExpr->mIterationTime,
                                                         aInitExprList,
                                                         & sArgExpr,
                                                         aRegionMem,
                                                         aEnv )
                 == STL_SUCCESS );
            
        sOrgExpr->mExpr.mFunction->mArgs[ sArgCount ] = sArgExpr;
    }


    /**
     * OUTPUT 설정
     */

    if( sInitExpr->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT )
    {
        sInitExpr->mExpr.mConstExpr->mOrgExpr = sOrgExpr;
    }
    else
    {
        sInitExpr = sOrgExpr;
    }

    *aInitNewExpr = sInitExpr;
    
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Rewritting Expression Function for Cast Function
 * @param[in]      aInitExpr        Init Expression Source
 * @param[in]      aParentIterTime  Parent's Iteration Time
 * @param[in,out]  aInitExprList    Init Expression List
 * @param[out]     aInitNewExpr     New Init Expression Source
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qlvRewriteCast( qlvInitExpression   * aInitExpr,
                          dtlIterationTime      aParentIterTime,
                          qlvInitExprList     * aInitExprList,
                          qlvInitExpression  ** aInitNewExpr,
                          knlRegionMem        * aRegionMem,
                          qllEnv              * aEnv )
{
    qlvInitExpression  * sOrgExpr  = NULL;
    qlvInitExpression  * sInitExpr = NULL;
    qlvInitExpression  * sArgExpr  = NULL;
    stlInt32             sArgCount = 0;
   
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitNewExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Constant Expression List에 Expression 추가
     */

    STL_TRY( qlvAddConstExpr( aInitExpr,
                              aParentIterTime,
                              aInitExprList,
                              & sInitExpr,
                              aRegionMem,
                              aEnv )
             == STL_SUCCESS );

    if( sInitExpr->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT )
    {
        sOrgExpr = sInitExpr->mExpr.mConstExpr->mOrgExpr;
    }
    else
    {
        sOrgExpr = sInitExpr;
    }

    
    /**
     * Expression List에 Expression 추가
     */

    /* Cast의 Argument Expression들에 대한 Rewrite 수행 */
    for( sArgCount = 0 ;
         sArgCount < DTL_CAST_INPUT_ARG_CNT ;
         sArgCount++ )
    {
        sArgExpr = sOrgExpr->mExpr.mTypeCast->mArgs[ sArgCount ];

        STL_TRY( qlvRewriteFuncList[ sArgExpr->mType ] ( sArgExpr,
                                                         sOrgExpr->mIterationTime,
                                                         aInitExprList,
                                                         & sArgExpr,
                                                         aRegionMem,
                                                         aEnv )
                 == STL_SUCCESS );

        sOrgExpr->mExpr.mTypeCast->mArgs[ sArgCount ] = sArgExpr;
    }

    
    /**
     * OUTPUT 설정
     */

    if( sInitExpr->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT )
    {
        sInitExpr->mExpr.mConstExpr->mOrgExpr = sOrgExpr;
    }
    else
    {
        sInitExpr = sOrgExpr;
    }

    *aInitNewExpr = sInitExpr;

    
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Rewritting Expression Function for Pseudo Column
 * @param[in]      aInitExpr        Init Expression Source
 * @param[in]      aParentIterTime  Parent's Iteration Time
 * @param[in,out]  aInitExprList    Init Expression List
 * @param[out]     aInitNewExpr     New Init Expression Source
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qlvRewritePseudoColumn( qlvInitExpression   * aInitExpr,
                                  dtlIterationTime      aParentIterTime,
                                  qlvInitExprList     * aInitExprList,
                                  qlvInitExpression  ** aInitNewExpr,
                                  knlRegionMem        * aRegionMem,
                                  qllEnv              * aEnv )
{
    qlvInitExpression  * sInitExpr = NULL;
    qlvInitExpression  * sOrgExpr  = NULL;
    
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitNewExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**
     * Constant Expression List에 Expression 추가
     */

    STL_TRY( qlvAddConstExpr( aInitExpr,
                              aParentIterTime,
                              aInitExprList,
                              & sInitExpr,
                              aRegionMem,
                              aEnv )
             == STL_SUCCESS );
    
    if( sInitExpr->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT )
    {
        /* 현재 pseudo column 이 상수처럼 처리되는 경우는 없다. */
        sOrgExpr = sInitExpr->mExpr.mConstExpr->mOrgExpr;
    }
    else
    {
        sOrgExpr = sInitExpr;
    }


    /**
     * Pseudo Column List에 Expression 추가
     */

    switch( sOrgExpr->mIterationTime )
    {
        case DTL_ITERATION_TIME_FOR_EACH_EXPR:
            {
                /**
                 * 현재 레벨의 Expression List내에 존재하는
                 * Pseudo Column Expression List를 가져옴
                 */

                /* Query Unit */
                STL_TRY( qlvAddPseudoColExprList( aInitExprList->mPseudoColExprList,
                                                  sOrgExpr,
                                                  aRegionMem,
                                                  aEnv )
                         == STL_SUCCESS );

                /* Statement Unit */
                STL_TRY( qlvAddPseudoColExprList( aInitExprList->mStmtExprList->mPseudoColExprList,
                                                  sOrgExpr,
                                                  aRegionMem,
                                                  aEnv )
                         == STL_SUCCESS );

                break;
            }
        case DTL_ITERATION_TIME_FOR_EACH_STATEMENT:
        case DTL_ITERATION_TIME_NONE:
            {
                /**
                 * Root 레벨(STMT 레벨)의 Expression List내에 존재하는
                 * Pseudo Column Expression List를 가져옴
                 */

                /* Statement Unit */
                STL_TRY( qlvAddPseudoColExprList( aInitExprList->mStmtExprList->mPseudoColExprList,
                                                  sOrgExpr,
                                                  aRegionMem,
                                                  aEnv )
                         == STL_SUCCESS );
                break;
            }
        default:
            {
                STL_DASSERT( 0 );
                break;
            }
    }


    /**
     * OUTPUT 설정
     */

    *aInitNewExpr = sInitExpr;

    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Rewritting Expression Function for Pseudo Arguments
 * @param[in]      aInitExpr        Init Expression Source
 * @param[in]      aParentIterTime  Parent's Iteration Time
 * @param[in,out]  aInitExprList    Init Expression List
 * @param[out]     aInitNewExpr     New Init Expression Source
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qlvRewritePseudoArgs( qlvInitExpression   * aInitExpr,
                                dtlIterationTime      aParentIterTime,
                                qlvInitExprList     * aInitExprList,
                                qlvInitExpression  ** aInitNewExpr,
                                knlRegionMem        * aRegionMem,
                                qllEnv              * aEnv )
{
    STL_DASSERT( 0 );
    
    return STL_FAILURE;
}


/**
 * @brief Rewritting Expression Function for Constant Expression
 * @param[in]      aInitExpr        Init Expression Source
 * @param[in]      aParentIterTime  Parent's Iteration Time
 * @param[in,out]  aInitExprList    Init Expression List
 * @param[out]     aInitNewExpr     New Init Expression Source
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qlvRewriteConstExpr( qlvInitExpression   * aInitExpr,
                               dtlIterationTime      aParentIterTime,
                               qlvInitExprList     * aInitExprList,
                               qlvInitExpression  ** aInitNewExpr,
                               knlRegionMem        * aRegionMem,
                               qllEnv              * aEnv )
{
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitNewExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /* Do Nothing */


    /**
     * OUTPUT 설정
     */

    *aInitNewExpr = aInitExpr;
    
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Rewritting Expression Function for Reference Expression
 * @param[in]      aInitExpr        Init Expression Source
 * @param[in]      aParentIterTime  Parent's Iteration Time
 * @param[in,out]  aInitExprList    Init Expression List
 * @param[out]     aInitNewExpr     New Init Expression Source
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qlvRewriteReference( qlvInitExpression   * aInitExpr,
                               dtlIterationTime      aParentIterTime,
                               qlvInitExprList     * aInitExprList,
                               qlvInitExpression  ** aInitNewExpr,
                               knlRegionMem        * aRegionMem,
                               qllEnv              * aEnv )
{
    STL_DASSERT( 0 );    
    
    return STL_FAILURE;
}


/**
 * @brief Rewritting Expression Function for Sub-Query
 * @param[in]      aInitExpr        Init Expression Source
 * @param[in]      aParentIterTime  Parent's Iteration Time
 * @param[in,out]  aInitExprList    Init Expression List
 * @param[out]     aInitNewExpr     New Init Expression Source
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qlvRewriteSubQuery( qlvInitExpression   * aInitExpr,
                              dtlIterationTime      aParentIterTime,
                              qlvInitExprList     * aInitExprList,
                              qlvInitExpression  ** aInitNewExpr,
                              knlRegionMem        * aRegionMem,
                              qllEnv              * aEnv )
{
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitNewExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Sub Query Expression List 구성
     */

    aInitExprList->mStmtExprList->mHasSubQuery = STL_TRUE;


    /**
     * OUTPUT 설정
     */

    *aInitNewExpr = aInitExpr;
    

    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Rewritting Expression Function for Inner Column
 * @param[in]      aInitExpr        Init Expression Source
 * @param[in]      aParentIterTime  Parent's Iteration Time
 * @param[in,out]  aInitExprList    Init Expression List
 * @param[out]     aInitNewExpr     New Init Expression Source
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qlvRewriteInnerColumn( qlvInitExpression   * aInitExpr,
                                 dtlIterationTime      aParentIterTime,
                                 qlvInitExprList     * aInitExprList,
                                 qlvInitExpression  ** aInitNewExpr,
                                 knlRegionMem        * aRegionMem,
                                 qllEnv              * aEnv )
{
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitNewExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * OUTPUT 설정
     */

    *aInitNewExpr = aInitExpr;

    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Rewritting Expression Function for Outer Column
 * @param[in]      aInitExpr        Init Expression Source
 * @param[in]      aParentIterTime  Parent's Iteration Time
 * @param[in,out]  aInitExprList    Init Expression List
 * @param[out]     aInitNewExpr     New Init Expression Source
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qlvRewriteOuterColumn( qlvInitExpression   * aInitExpr,
                                 dtlIterationTime      aParentIterTime,
                                 qlvInitExprList     * aInitExprList,
                                 qlvInitExpression  ** aInitNewExpr,
                                 knlRegionMem        * aRegionMem,
                                 qllEnv              * aEnv )
{
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitNewExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**
     * OUTPUT 설정
     */

    *aInitNewExpr = aInitExpr;

    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Rewritting Expression Function for RowId Column
 * @param[in]      aInitExpr        Init Expression Source
 * @param[in]      aParentIterTime  Parent's Iteration Time
 * @param[in,out]  aInitExprList    Init Expression List
 * @param[out]     aInitNewExpr     New Init Expression Source
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qlvRewriteRowIdColumn( qlvInitExpression   * aInitExpr,
                                 dtlIterationTime      aParentIterTime,
                                 qlvInitExprList     * aInitExprList,
                                 qlvInitExpression  ** aInitNewExpr,
                                 knlRegionMem        * aRegionMem,
                                 qllEnv              * aEnv )
{
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitNewExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /* Do Nothing */

    
    /**
     * OUTPUT 설정
     */

    *aInitNewExpr = aInitExpr;

    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Rewritting Expression for Aggregation
 * @param[in]      aInitExpr        Init Expression Source
 * @param[in]      aParentIterTime  Parent's Iteration Time
 * @param[in,out]  aInitExprList    Init Expression List
 * @param[out]     aInitNewExpr     New Init Expression Source
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qlvRewriteAggregation( qlvInitExpression   * aInitExpr,
                                 dtlIterationTime      aParentIterTime,
                                 qlvInitExprList     * aInitExprList,
                                 qlvInitExpression  ** aInitNewExpr,
                                 knlRegionMem        * aRegionMem,
                                 qllEnv              * aEnv )
{
    qlvInitExpression  * sInitExpr = NULL;
    qlvInitExpression  * sArgExpr  = NULL;
    stlInt32             sArgCount = 0;


    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitNewExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    sInitExpr = aInitExpr;


    /**
     * Expression List에 Expression 추가
     */

    /* Aggregation의 Argument Expression들에 대한 Rewrite 수행 */
    for( sArgCount = 0 ;
         sArgCount < sInitExpr->mExpr.mAggregation->mArgCount ;
         sArgCount++ )
    {
        sArgExpr = sInitExpr->mExpr.mAggregation->mArgs[ sArgCount ];
        
        STL_TRY( qlvRewriteFuncList[ sArgExpr->mType ] ( sArgExpr,
                                                         sInitExpr->mIterationTime,
                                                         aInitExprList,
                                                         & sArgExpr,
                                                         aRegionMem,
                                                         aEnv )
                 == STL_SUCCESS );
            
        sInitExpr->mExpr.mAggregation->mArgs[ sArgCount ] = sArgExpr;
    }

    
    /**
     * Aggregation List에 Expression 추가
     */

    /* Query Unit */
    if( sInitExpr->mExpr.mAggregation->mHasNestedAggr == STL_TRUE )
    {
        STL_TRY( qlvAddExprToRefExprList( aInitExprList->mNestedAggrExprList,
                                          sInitExpr,
                                          STL_FALSE,
                                          aRegionMem,
                                          aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( qlvAddExprToRefExprList( aInitExprList->mAggrExprList,
                                          sInitExpr,
                                          STL_FALSE,
                                          aRegionMem,
                                          aEnv )
                 == STL_SUCCESS );
    }

    
    /**
     * OUTPUT 설정
     */

    *aInitNewExpr = sInitExpr;

    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Rewritting Expression Function for Case expr
 * @param[in]      aInitExpr        Init Expression Source
 * @param[in]      aParentIterTime  Parent's Iteration Time
 * @param[in,out]  aInitExprList    Init Expression List
 * @param[out]     aInitNewExpr     New Init Expression Source
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qlvRewriteCaseExpr( qlvInitExpression   * aInitExpr,
                              dtlIterationTime      aParentIterTime,
                              qlvInitExprList     * aInitExprList,
                              qlvInitExpression  ** aInitNewExpr,
                              knlRegionMem        * aRegionMem,
                              qllEnv              * aEnv )
{
    qlvInitExpression  * sOrgExpr  = NULL;
    qlvInitExpression  * sInitExpr = NULL;
    qlvInitExpression  * sExpr     = NULL;
    stlInt32             sCount    = 0;
    
    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitNewExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Constant Expression List에 Expression 추가
     */

    STL_TRY( qlvAddConstExpr( aInitExpr,
                              aParentIterTime,
                              aInitExprList,
                              & sInitExpr,
                              aRegionMem,
                              aEnv )
             == STL_SUCCESS );

    if( sInitExpr->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT )
    {
        sOrgExpr = sInitExpr->mExpr.mConstExpr->mOrgExpr;
    }
    else
    {
        sOrgExpr = sInitExpr;
    }

    /**
     * Expression List에 Expression 추가
     */
    
    for( sCount = 0 ;
         sCount < sOrgExpr->mExpr.mCaseExpr->mCount;
         sCount++ )
    {
        /*
         * Case의 When Expression들에 대한 Rewrite 수행
         */
        
        sExpr = sOrgExpr->mExpr.mCaseExpr->mWhenArr[ sCount ];
        
        if( sExpr->mType == QLV_EXPR_TYPE_SUB_QUERY )
        {
            sOrgExpr->mExpr.mCaseExpr->mWhenArr[ sCount ] =
                sExpr->mExpr.mSubQuery->mTargets[0].mExpr;

            STL_TRY( qlvRewriteFuncList[ sExpr->mType ] ( sExpr,
                                                          sOrgExpr->mIterationTime,
                                                          aInitExprList,
                                                          & sExpr,
                                                          aRegionMem,
                                                          aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            STL_TRY( qlvRewriteFuncList[ sExpr->mType ] ( sExpr,
                                                          sOrgExpr->mIterationTime,
                                                          aInitExprList,
                                                          & sExpr,
                                                          aRegionMem,
                                                          aEnv )
                     == STL_SUCCESS );
            
            sOrgExpr->mExpr.mCaseExpr->mWhenArr[ sCount ] = sExpr;
        }
        

        /*
         * Case의 Then Expression들에 대한 Rewrite 수행
         */
        
        sExpr = sOrgExpr->mExpr.mCaseExpr->mThenArr[ sCount ];

        STL_TRY( qlvRewriteFuncList[ sExpr->mType ] ( sExpr,
                                                      sOrgExpr->mIterationTime,
                                                      aInitExprList,
                                                      & sExpr,
                                                      aRegionMem,
                                                      aEnv )
                 == STL_SUCCESS );
            
        sOrgExpr->mExpr.mCaseExpr->mThenArr[ sCount ] = sExpr;
    }

    /*
     * Case의 Default Result Expression들에 대한 Rewrite 수행
     */
    
    sExpr = sOrgExpr->mExpr.mCaseExpr->mDefResult;

    STL_TRY( qlvRewriteFuncList[ sExpr->mType ] ( sExpr,
                                                  sOrgExpr->mIterationTime,
                                                  aInitExprList,
                                                  & sExpr,
                                                  aRegionMem,
                                                  aEnv )
             == STL_SUCCESS );
            
    sOrgExpr->mExpr.mCaseExpr->mDefResult = sExpr;

    
    /**
     * OUTPUT 설정
     */

    if( sInitExpr->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT )
    {
        sInitExpr->mExpr.mConstExpr->mOrgExpr = sOrgExpr;
    }
    else
    {
        sInitExpr = sOrgExpr;
    }

    *aInitNewExpr = sInitExpr;

    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;    
}


/**
 * @brief Rewritting Expression Function for List func
 * @param[in]      aInitExpr        Init Expression Source
 * @param[in]      aParentIterTime  Parent's Iteration Time
 * @param[in,out]  aInitExprList    Init Expression List
 * @param[out]     aInitNewExpr     New Init Expression Source
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qlvRewriteListFunction( qlvInitExpression   * aInitExpr,
                                  dtlIterationTime      aParentIterTime,
                                  qlvInitExprList     * aInitExprList,
                                  qlvInitExpression  ** aInitNewExpr,
                                  knlRegionMem        * aRegionMem,
                                  qllEnv              * aEnv )
{
    qlvInitExpression  * sOrgExpr  = NULL;
    qlvInitExpression  * sInitExpr = NULL;
    qlvInitExpression  * sArgExpr  = NULL;
    stlInt32             sArgCount = 0;


    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitNewExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * List Function Expression List 구성
     */

    STL_DASSERT( aInitExpr->mExpr.mListFunc->mArgCount > 0 );

    STL_DASSERT( aInitExpr->mExpr.mListFunc->mArgs[ 0 ]->mType ==
                 QLV_EXPR_TYPE_LIST_COLUMN );


    /**
     * Constant Expression List에 Expression 추가
     */

    STL_TRY( qlvAddConstExpr( aInitExpr,
                              aParentIterTime,
                              aInitExprList,
                              & sInitExpr,
                              aRegionMem,
                              aEnv )
             == STL_SUCCESS );

    if( sInitExpr->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT )
    {
        sOrgExpr = sInitExpr->mExpr.mConstExpr->mOrgExpr;
    }
    else
    {
        sOrgExpr = sInitExpr;
    }


    /**
     * Expression List에 Expression 추가
     */

    /* Function의 Argument Expression들에 대한 Rewrite 수행 */
    for( sArgCount = 0 ;
         sArgCount < sOrgExpr->mExpr.mListFunc->mArgCount ;
         sArgCount++ )
    {
        sArgExpr = sOrgExpr->mExpr.mListFunc->mArgs[ sArgCount ];
        STL_TRY( qlvRewriteFuncList[ sArgExpr->mType ] ( sArgExpr,
                                                         sOrgExpr->mIterationTime,
                                                         aInitExprList,
                                                         & sArgExpr,
                                                         aRegionMem,
                                                         aEnv )
                 == STL_SUCCESS );

        sOrgExpr->mExpr.mListFunc->mArgs[ sArgCount ] = sArgExpr;
    }


    /**
     * OUTPUT 설정
     */

    if( sInitExpr->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT )
    {
        sInitExpr->mExpr.mConstExpr->mOrgExpr = sOrgExpr;
    }
    else
    {
        sInitExpr = sOrgExpr;
    }


    *aInitNewExpr = sInitExpr;
    
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief Rewritting Expression Function for List column
 * @param[in]      aInitExpr        Init Expression Source
 * @param[in]      aParentIterTime  Parent's Iteration Time
 * @param[in,out]  aInitExprList    Init Expression List
 * @param[out]     aInitNewExpr     New Init Expression Source
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qlvRewriteListColumn( qlvInitExpression   * aInitExpr,
                                dtlIterationTime      aParentIterTime,
                                qlvInitExprList     * aInitExprList,
                                qlvInitExpression  ** aInitNewExpr,
                                knlRegionMem        * aRegionMem,
                                qllEnv              * aEnv )
{
    qlvInitExpression  * sOrgExpr  = NULL;
    qlvInitExpression  * sArgExpr  = NULL;
    stlInt32             sArgCount = 0;

    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitNewExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );

    
     /**
     * Rewrite 수행
     * IterTime은 Each Expr로 평가되므로
     * List Column 또는 Row Expr는 Constant가 될 수 없다.
     * 그러므로 List Column, Row Expr Parent에 대한 IterTime을 갖게 된다. 
     */

    sOrgExpr = aInitExpr;

    
    /**
     * Expression List에 Expression 추가
     */

    /* Function의 Argument Expression들에 대한 Rewrite 수행 */
    for( sArgCount = 0 ;
         sArgCount < sOrgExpr->mExpr.mListCol->mArgCount ;
         sArgCount++ )
    {
        sArgExpr = sOrgExpr->mExpr.mListCol->mArgs[ sArgCount ];
        STL_TRY( qlvRewriteFuncList[ sArgExpr->mType ] ( sArgExpr,
                                                         aParentIterTime,
                                                         aInitExprList,
                                                         & sArgExpr,
                                                         aRegionMem,
                                                         aEnv )
                 == STL_SUCCESS );

        sOrgExpr->mExpr.mListCol->mArgs[ sArgCount ] = sArgExpr;
    }


    /**
     * OUTPUT 설정
     */

    *aInitNewExpr = sOrgExpr;
    
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Rewritting Expression Row Expr
 * @param[in]      aInitExpr        Init Expression Source
 * @param[in]      aParentIterTime  Parent's Iteration Time
 * @param[in,out]  aInitExprList    Init Expression List
 * @param[out]     aInitNewExpr     New Init Expression Source
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qlvRewriteRowExpr( qlvInitExpression   * aInitExpr,
                             dtlIterationTime      aParentIterTime,
                             qlvInitExprList     * aInitExprList,
                             qlvInitExpression  ** aInitNewExpr,
                             knlRegionMem        * aRegionMem,
                             qllEnv              * aEnv )
{
    qlvInitExpression  * sOrgExpr  = NULL;
    qlvInitExpression  * sArgExpr  = NULL;
    qlvInitRowExpr     * sRowExpr  = NULL;
    stlInt32             sArgCount = 0;

    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitNewExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**
     * Rewrite 수행
     */

    sOrgExpr = aInitExpr;
    sRowExpr = sOrgExpr->mExpr.mRowExpr;
    

    /**
     * Expression List에 Expression 추가
     */

    /* Function의 Argument Expression들에 대한 Rewrite 수행 */
    for( sArgCount = 0 ;
         sArgCount < sRowExpr->mArgCount ;
         sArgCount++ )
    {
        sArgExpr = sRowExpr->mArgs[ sArgCount ];
        STL_TRY( qlvRewriteFuncList[ sArgExpr->mType ] ( sArgExpr,
                                                         aParentIterTime,
                                                         aInitExprList,
                                                         & sArgExpr,
                                                         aRegionMem,
                                                         aEnv )
                 == STL_SUCCESS );

        sRowExpr->mArgs[ sArgCount ] = sArgExpr;
    }

    
    /**
     * OUTPUT 설정
     */

    *aInitNewExpr = sOrgExpr;
    
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**************************************************************************
 * Common Functions for Rewritting
 **************************************************************************/

/**
 * @brief Expresion을 평가하여 Constant Expression List 구성
 * @param[in]      aInitExpr        Init Expression Source
 * @param[in]      aParentIterTime  Parent's Iteration Time
 * @param[in,out]  aInitExprList    Init Expression List
 * @param[out]     aOrgExpr         Constant Expression List
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qlvAddConstExpr( qlvInitExpression   * aInitExpr,
                           dtlIterationTime      aParentIterTime,
                           qlvInitExprList     * aInitExprList,
                           qlvInitExpression  ** aOrgExpr,
                           knlRegionMem        * aRegionMem,
                           qllEnv              * aEnv )
{
    qlvInitExpression  * sNewExpr   = NULL;
    qlvInitExpression  * sConstExpr = NULL;
    qlvInitConstExpr   * sConstant  = NULL;
    
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aParentIterTime >= DTL_ITERATION_TIME_FOR_EACH_AGGREGATION ) &&
                        ( aParentIterTime < DTL_ITERATION_TIME_MAX ),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Parent Iteration Time과 현재 Expression의 Iteration Time 평가
     */

    if( ( aParentIterTime < DTL_ITERATION_TIME_FOR_EACH_STATEMENT ) &&
        ( aInitExpr->mIterationTime >= DTL_ITERATION_TIME_FOR_EACH_STATEMENT ) )
    {
        /* 현재 Expression 복사하여 새로운 Expression 생성 */
        STL_TRY( qlvCopyExpr( aInitExpr,
                              & sNewExpr,
                              aRegionMem,
                              aEnv )
                 == STL_SUCCESS );

        /* qlvInitConstExpr 공간 할당 */
        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( qlvInitConstExpr ),
                                    (void **) & sConstant,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* qlvInitConstExpr 설정 */
        sConstant->mOrgExpr = aInitExpr;
        
        /* 현재 Expression을 Constant Expression으로 변경 */
        sNewExpr->mType = QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT;
        sNewExpr->mExpr.mConstExpr = sConstant;

        QLV_VALIDATE_INCLUDE_EXPR_CNT( sNewExpr->mIncludeExprCnt + 1, aEnv );

        sNewExpr->mIncludeExprCnt++;

        /* Constant Expression을 Constant Expression List에 추가 */
        STL_TRY( qlvAddExprToRefExprList( aInitExprList->mStmtExprList->mConstExprList,
                                          sNewExpr,
                                          STL_FALSE,
                                          aRegionMem,
                                          aEnv )
                 == STL_SUCCESS );
        
        sConstExpr = sNewExpr;
    }
    else
    {
        sConstExpr = aInitExpr;
    }

    
    /**
     * OUTPUT 설정
     */

    *aOrgExpr = sConstExpr;
    
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Pseudo Column을 Expression 리스트에 추가
 * @param[in,out]  aRefExprList  Pseudo Expression List
 * @param[in]      aExpr         Pseudo Expression
 * @param[in]      aRegionMem    Region Memory
 * @param[in]      aEnv          Environment
 */
stlStatus qlvAddPseudoColExprList( qlvRefExprList     * aRefExprList,
                                   qlvInitExpression  * aExpr,
                                   knlRegionMem       * aRegionMem,
                                   qllEnv             * aEnv )
{
    qlvRefExprList      * sRefExprList     = NULL;
    qlvRefExprItem      * sRefExprItem     = NULL;
    qlvRefExprItem      * sRefExprItemPrev = NULL;
    qlvInitPseudoCol    * sSourcePseudo    = NULL;
    qlvInitPseudoCol    * sTargetPseudo    = NULL;

    stlBool               sExist = STL_FALSE;
    stlInt32              i = 0;

    STL_PARAM_VALIDATE( aRefExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr->mType == QLV_EXPR_TYPE_PSEUDO_COLUMN,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * Pseudo Column Expr List 처리
     **********************************************************/

    /**
     * 동일한 Pseudo Column 이 존재하는 지 검사
     */
    sRefExprList        = aRefExprList;
    sTargetPseudo       = aExpr->mExpr.mPseudoCol;
    sRefExprItem        = sRefExprList->mHead;
    sRefExprItemPrev    = NULL;
    sExist              = STL_FALSE;

    while( sRefExprItem != NULL )
    {
        sSourcePseudo = sRefExprItem->mExprPtr->mExpr.mPseudoCol;

        if( (sSourcePseudo->mPseudoId == sTargetPseudo->mPseudoId) ||
            ( (sSourcePseudo->mPseudoId == KNL_PSEUDO_COL_CURRVAL) &&
              (sTargetPseudo->mPseudoId == KNL_PSEUDO_COL_NEXTVAL) ) ||
            ( (sSourcePseudo->mPseudoId == KNL_PSEUDO_COL_NEXTVAL) &&
              (sTargetPseudo->mPseudoId == KNL_PSEUDO_COL_CURRVAL) ) )
        {
            /* 동일한 Pseudo Function ID 인 경우 */

            /*
             * 동일한 Sequence 에 대해 SEQ.NEXTVAL, SEQ.CURRVAL 이 함께 사용되면
             * SEQ.CURRVAL, SEQ.NEXTVAL 과 동일한 결과가 생성되어야 함.
             */

            if( sSourcePseudo->mArgCount == sTargetPseudo->mArgCount )
            {
                /* Argument 개수가 동일한 경우 */

                if ( sSourcePseudo->mArgCount == 0 )
                {
                    /* 현재 구현된 pseudo column은 모두 argument 1개를 가진다. */

                    /**
                     * @todo (LONG TODO) level 과 같은 pseudo column 추가
                     */
                    
                    sExist = STL_TRUE;
                    break;
                }
                else
                {
                    /* 모든 Argument 가 동일한 지 검사 */

                    sExist = STL_TRUE;
                    for( i = 0; i < sSourcePseudo->mArgCount; i++ )
                    {
                        /**
                         * @note
                         * Argument 가 Handle 등이 아니라면 pointer 비교는 위험한 접근이긴 하다.
                         */

                        if ( sSourcePseudo->mArgs[i].mPseudoArg
                             == sTargetPseudo->mArgs[i].mPseudoArg )
                        {
                            continue;
                        }
                        else
                        {
                            sExist = STL_FALSE;
                            break;
                        }
                    }

                    /* 모든 Argument 가 동일한 경우 */

                    if( sExist == STL_TRUE )
                    {
                        break;
                    }
                }
            }
            else
            {
                /* Argument 개수가 다른 경우 */
                /* 현재 구현된 pseudo column은 모두 argument 1개를 가진다. */
                sExist = STL_FALSE;
            }
        }
        else
        {
            /* 다른 Pseudo Function ID 인 경우 */
            /* 현재 sequence 관련 pseudo colum 만 구현되어 있다. */
            sExist = STL_FALSE;
        }

        sRefExprItemPrev    = sRefExprItem;
        sRefExprItem        = sRefExprItem->mNext;
    }

    /**
     * 동일한 Pseudo Column 정보가 없는 경우 추가
     */
    if( sExist == STL_FALSE )
    {
        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( qlvRefExprItem ),
                                    (void **) & sRefExprItem,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sTargetPseudo->mPseudoOffset = sRefExprList->mCount;
        sRefExprItem->mExprPtr       = aExpr;
        sRefExprItem->mNext          = NULL;

        if( sRefExprItemPrev == NULL )
        {
            /* 제일 앞에 추가되는 경우 */
            if( sRefExprList->mCount == 0 )
            {
                /* 처음 추가되는 경우 */
                sRefExprList->mHead = sRefExprItem;
                sRefExprList->mTail = sRefExprItem;
            }
            else
            {
                /* 추가된 Reference Expression Item이 있는 경우 */
                sRefExprItem->mNext = sRefExprList->mHead;
                sRefExprList->mHead = sRefExprItem;
            }
        }
        else
        {
            /* 중간에 추가되는 경우 */
            if( sRefExprItemPrev == sRefExprList->mTail )
            {
                /* 제일 마지막에 추가되는 경우 */
                sRefExprList->mTail->mNext  = sRefExprItem;
                sRefExprList->mTail         = sRefExprItem;
            }
            else
            {
                /* 중간에 추가되는 경우 */
                sRefExprItem->mNext     = sRefExprItemPrev->mNext;
                sRefExprItemPrev->mNext = sRefExprItem;
            }
        }

        sRefExprList->mCount++;
    }
    else
    {
        /**
         * 동일한 Pseudo Column 이 존재하는 경우
         */
        
        sTargetPseudo->mPseudoOffset = sRefExprItem->mExprPtr->mExpr.mPseudoCol->mPseudoOffset;
        
        /**
         * Target 이 NEXTVAL 이면,
         * 이전에 CURRVAL 또는 NEXTVAL 여부에 관계없이 NEXTVAL 을 설정함.
         */
        if ( sTargetPseudo->mPseudoId == KNL_PSEUDO_COL_NEXTVAL )
        {
            sRefExprItem->mExprPtr->mExpr.mPseudoCol->mPseudoId = sTargetPseudo->mPseudoId;
        }
        else
        {
            /**
             * Source Pseudo 를 그대로 사용함
             * - nothing to do
             */
        }
    }


    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlv */

