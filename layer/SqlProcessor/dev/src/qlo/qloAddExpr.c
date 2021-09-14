/*******************************************************************************
 * qloAddExpr.c
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
 * @file qloAddExpr.c
 * @brief SQL Processor Layer Add Expression
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlo
 * @{
 */


/**************************************************************************
 * Add Expression Functions For Expression
 **************************************************************************/

/**
 * @brief Add Expression Function List
 */
static const qloAddExprFuncPtr qloAddExprFuncList[ QLV_EXPR_TYPE_MAX ] = 
{
    qloAddExprValue,         /**< QLV_EXPR_TYPE_VALUE */
    qloAddExprBindParam,     /**< QLV_EXPR_TYPE_BIND_PARAM */
    qloAddExprColumn,        /**< QLV_EXPR_TYPE_COLUMN */
    qloAddExprFunction,      /**< QLV_EXPR_TYPE_FUNCTION */
    qloAddExprCast,          /**< QLV_EXPR_TYPE_CAST */
    qloAddExprPseudoColumn,  /**< QLV_EXPR_TYPE_PSEUDO_COLUMN */
    qloAddExprPseudoArgs,    /**< QLV_EXPR_TYPE_PSEUDO_ARGUMENT */
    qloAddExprConstExpr,     /**< QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT */
    qloAddExprReference,     /**< QLV_EXPR_TYPE_REFERENCE */
    qloAddExprSubQuery,      /**< QLV_EXPR_TYPE_SUB_QUERY */
    qloAddExprInnerColumn,   /**< QLV_EXPR_TYPE_INNER_COLUMN */
    qloAddExprOuterColumn,   /**< QLV_EXPR_TYPE_OUTER_COLUMN */
    qloAddExprRowIdColumn,   /**< QLV_EXPR_TYPE_ROWID_COLUMN */
    qloAddExprAggregation,   /**< QLV_EXPR_TYPE_AGGREGATION */
    qloAddExprCaseExpr,      /**< QLV_EXPR_TYPE_CASE_EXPR */
    qloAddExprListFunction,  /**< QLV_EXPR_TYPE_LIST_FUNCTION */
    qloAddExprListColumn,    /**< QLV_EXPR_TYPE_LIST_COLUMN */
    qloAddExprRowExpr        /**< QLV_EXPR_TYPE_ROW_EXPR */
};


/**
 * @brief Common Add Expression Function
 * @param[in]      aInitExpr        Init Expression Source
 * @param[in,out]  aRefExprList     Reference Expression List
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qloAddExpr( qlvInitExpression   * aInitExpr,
                      qlvRefExprList      * aRefExprList,
                      knlRegionMem        * aRegionMem,
                      qllEnv              * aEnv )
{
    if( aInitExpr->mIsSetOffset == STL_TRUE )
    {
        return STL_SUCCESS;
    }
    else
    {
        return qloAddExprFuncList[ aInitExpr->mType ] ( aInitExpr,
                                                        aRefExprList,
                                                        aRegionMem,
                                                        aEnv );
    }
}


/**
 * @brief Add Expression Function for Value (literal)
 * @param[in]      aInitExpr        Init Expression Source
 * @param[in,out]  aRefExprList     Reference Expression List
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qloAddExprValue( qlvInitExpression   * aInitExpr,
                           qlvRefExprList      * aRefExprList,
                           knlRegionMem        * aRegionMem,
                           qllEnv              * aEnv )
{
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Expression List에 Expression 추가
     */

    STL_TRY( qlvAddExprToRefExprList( aRefExprList,
                                      aInitExpr,
                                      STL_TRUE,
                                      aRegionMem,
                                      aEnv )
             == STL_SUCCESS );
 

    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Add Expression Function for Bind Parameter
 * @param[in]      aInitExpr        Init Expression Source
 * @param[in,out]  aRefExprList     Reference Expression List
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qloAddExprBindParam( qlvInitExpression   * aInitExpr,
                               qlvRefExprList      * aRefExprList,
                               knlRegionMem        * aRegionMem,
                               qllEnv              * aEnv )
{
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Expression List에 Expression 추가
     */

    STL_TRY( qlvAddExprToRefExprList( aRefExprList,
                                      aInitExpr,
                                      STL_TRUE,
                                      aRegionMem,
                                      aEnv )
             == STL_SUCCESS );

    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Add Expression Function for Column
 * @param[in]      aInitExpr        Init Expression Source
 * @param[in,out]  aRefExprList     Reference Expression List
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qloAddExprColumn( qlvInitExpression   * aInitExpr,
                            qlvRefExprList      * aRefExprList,
                            knlRegionMem        * aRegionMem,
                            qllEnv              * aEnv )
{
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**
     * Expression List에 Expression 추가
     */

    STL_TRY( qlvAddExprToRefExprList( aRefExprList,
                                      aInitExpr,
                                      STL_TRUE,
                                      aRegionMem,
                                      aEnv )
             == STL_SUCCESS );

    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Add Expression Function for Built-In Function
 * @param[in]      aInitExpr        Init Expression Source
 * @param[in,out]  aRefExprList     Reference Expression List
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qloAddExprFunction( qlvInitExpression   * aInitExpr,
                              qlvRefExprList      * aRefExprList,
                              knlRegionMem        * aRegionMem,
                              qllEnv              * aEnv )
{
    qlvInitExpression  * sArgExpr  = NULL;
    stlInt32             sArgCount = 0;
 

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Function의 Argument Expression들에 대한 Add Expression 수행
     */

    for( sArgCount = 0 ;
         sArgCount < aInitExpr->mExpr.mFunction->mArgCount ;
         sArgCount++ )
    {
        sArgExpr = aInitExpr->mExpr.mFunction->mArgs[ sArgCount ];
        STL_TRY( qloAddExpr( sArgExpr,
                             aRefExprList,
                             aRegionMem,
                             aEnv )
                 == STL_SUCCESS );
    }


    /**
     * 현재 Expression을 Expression List에 추가
     */

    STL_TRY( qlvAddExprToRefExprList( aRefExprList,
                                      aInitExpr,
                                      STL_TRUE,
                                      aRegionMem,
                                      aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Add Expression Function for Cast Function
 * @param[in]      aInitExpr        Init Expression Source
 * @param[in,out]  aRefExprList     Reference Expression List
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qloAddExprCast( qlvInitExpression   * aInitExpr,
                          qlvRefExprList      * aRefExprList,
                          knlRegionMem        * aRegionMem,
                          qllEnv              * aEnv )
{
    qlvInitExpression  * sArgExpr  = NULL;
    stlInt32             sArgCount = 0;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Cast의 Argument Expression들에 대한 Add Expression 수행
     */

    for( sArgCount = 0 ;
         sArgCount < DTL_CAST_INPUT_ARG_CNT ;
         sArgCount++ )
    {
        sArgExpr = aInitExpr->mExpr.mTypeCast->mArgs[ sArgCount ];
        STL_TRY( qloAddExpr( sArgExpr,
                             aRefExprList,
                             aRegionMem,
                             aEnv )
                 == STL_SUCCESS );
    }


    /**
     * 현재 Expression을 Expression List에 추가
     */

    STL_TRY( qlvAddExprToRefExprList( aRefExprList,
                                      aInitExpr,
                                      STL_TRUE,
                                      aRegionMem,
                                      aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Add Expression Function for Pseudo Column
 * @param[in]      aInitExpr        Init Expression Source
 * @param[in,out]  aRefExprList     Reference Expression List
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qloAddExprPseudoColumn( qlvInitExpression   * aInitExpr,
                                  qlvRefExprList      * aRefExprList,
                                  knlRegionMem        * aRegionMem,
                                  qllEnv              * aEnv )
{
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Expression List에 Expression 추가
     */

    STL_TRY( qlvAddExprToRefExprList( aRefExprList,
                                      aInitExpr,
                                      STL_TRUE,
                                      aRegionMem,
                                      aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Add Expression Function for Pseudo Arguments
 * @param[in]      aInitExpr        Init Expression Source
 * @param[in,out]  aRefExprList     Reference Expression List
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qloAddExprPseudoArgs( qlvInitExpression   * aInitExpr,
                                qlvRefExprList      * aRefExprList,
                                knlRegionMem        * aRegionMem,
                                qllEnv              * aEnv )
{
    STL_DASSERT( 0 );
    
    return STL_FAILURE;
}


/**
 * @brief Add Expression Function for Constant Expression
 * @param[in]      aInitExpr        Init Expression Source
 * @param[in,out]  aRefExprList     Reference Expression List
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qloAddExprConstExpr( qlvInitExpression   * aInitExpr,
                               qlvRefExprList      * aRefExprList,
                               knlRegionMem        * aRegionMem,
                               qllEnv              * aEnv )
{
    /* qlvInitExpression   * sOrgExpr  = NULL; */


    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * 현재 Expression을 Expression List에 추가
     */

    STL_TRY( qlvAddExprToRefExprList( aRefExprList,
                                      aInitExpr,
                                      STL_TRUE,
                                      aRegionMem,
                                      aEnv )
             == STL_SUCCESS );
 

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Add Expression Function for Reference Expression
 * @param[in]      aInitExpr        Init Expression Source
 * @param[in,out]  aRefExprList     Reference Expression List
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qloAddExprReference( qlvInitExpression   * aInitExpr,
                               qlvRefExprList      * aRefExprList,
                               knlRegionMem        * aRegionMem,
                               qllEnv              * aEnv )
{
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Org Expression은 Expression List에 추가되어 있어야 한다.
     */

    STL_DASSERT( aInitExpr->mExpr.mReference->mOrgExpr->mIsSetOffset == STL_TRUE );


    /**
     * 현재 Expression을 Expression List에 추가
     */

    STL_TRY( qlvAddExprToRefExprList( aRefExprList,
                                      aInitExpr,
                                      STL_TRUE,
                                      aRegionMem,
                                      aEnv )
             == STL_SUCCESS );
 

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Add Expression Function for Sub-Query
 * @param[in]      aInitExpr        Init Expression Source
 * @param[in,out]  aRefExprList     Reference Expression List
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qloAddExprSubQuery( qlvInitExpression   * aInitExpr,
                              qlvRefExprList      * aRefExprList,
                              knlRegionMem        * aRegionMem,
                              qllEnv              * aEnv )
{
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /* Do Nothing */

    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Add Expression Function for Inner Column
 * @param[in]      aInitExpr        Init Expression Source
 * @param[in,out]  aRefExprList     Reference Expression List
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qloAddExprInnerColumn( qlvInitExpression   * aInitExpr,
                                 qlvRefExprList      * aRefExprList,
                                 knlRegionMem        * aRegionMem,
                                 qllEnv              * aEnv )
{
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**
     * Expression List에 Expression 추가
     */

    STL_TRY( qlvAddExprToRefExprList( aRefExprList,
                                      aInitExpr,
                                      STL_TRUE,
                                      aRegionMem,
                                      aEnv )
             == STL_SUCCESS );

    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Add Expression Function for Outer Column
 * @param[in]      aInitExpr        Init Expression Source
 * @param[in,out]  aRefExprList     Reference Expression List
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qloAddExprOuterColumn( qlvInitExpression   * aInitExpr,
                                 qlvRefExprList      * aRefExprList,
                                 knlRegionMem        * aRegionMem,
                                 qllEnv              * aEnv )
{
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Outer Column은 Org Expr을 먼저 Add한다.
     */

    STL_TRY( qloAddExpr( aInitExpr->mExpr.mOuterColumn->mOrgExpr,
                         aRefExprList,
                         aRegionMem,
                         aEnv )
             == STL_SUCCESS );


    /**
     * Expression List에 Expression 추가
     */

    STL_TRY( qlvAddExprToRefExprList( aRefExprList,
                                      aInitExpr,
                                      STL_TRUE,
                                      aRegionMem,
                                      aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Add Expression Function for RowId Column
 * @param[in]      aInitExpr        Init Expression Source
 * @param[in,out]  aRefExprList     Reference Expression List
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qloAddExprRowIdColumn( qlvInitExpression   * aInitExpr,
                                 qlvRefExprList      * aRefExprList,
                                 knlRegionMem        * aRegionMem,
                                 qllEnv              * aEnv )
{
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**
     * Expression List에 Expression 추가
     */

    STL_TRY( qlvAddExprToRefExprList( aRefExprList,
                                      aInitExpr,
                                      STL_TRUE,
                                      aRegionMem,
                                      aEnv )
             == STL_SUCCESS );

    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Add Expression for Built-In Aggregation
 * @param[in]      aInitExpr        Init Expression Source
 * @param[in,out]  aRefExprList     Reference Expression List
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qloAddExprAggregation( qlvInitExpression   * aInitExpr,
                                 qlvRefExprList      * aRefExprList,
                                 knlRegionMem        * aRegionMem,
                                 qllEnv              * aEnv )
{
    qlvInitExpression  * sArgExpr  = NULL;
    stlInt32             sArgCount = 0;
 

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Aggregation의 Argument Expression들에 대한 Add Expression 수행
     */

    for( sArgCount = 0 ;
         sArgCount < aInitExpr->mExpr.mAggregation->mArgCount ;
         sArgCount++ )
    {
        sArgExpr = aInitExpr->mExpr.mAggregation->mArgs[ sArgCount ];

        STL_TRY( qloAddExpr( sArgExpr,
                             aRefExprList,
                             aRegionMem,
                             aEnv )
                 == STL_SUCCESS );
    }


    /**
     * @todo (LONG TODO)
     * Aggregation의 Filter Expression에 대한 Add Expression 수행
     */

    /* if( aInitExpr->mExpr.mAggregation->mFilter != NULL ) */
    /* { */
    /*     sArgExpr = aInitExpr->mExpr.mAggregation->mFilter; */
    /*     STL_TRY( qloAddExpr( sArgExpr, */
    /*                          aRefExprList, */
    /*                          aRegionMem, */
    /*                          aEnv ) */
    /*              == STL_SUCCESS ); */
    /* } */
    /* else */
    /* { */
    /*     /\* Do Nothing *\/ */
    /* } */
    

    /**
     * 현재 Expression을 Expression List에 추가
     */

    STL_TRY( qlvAddExprToRefExprList( aRefExprList,
                                      aInitExpr,
                                      STL_TRUE,
                                      aRegionMem,
                                      aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief Add Expression for Case Expression
 * @param[in]      aInitExpr        Init Expression Source
 * @param[in,out]  aRefExprList     Reference Expression List
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qloAddExprCaseExpr( qlvInitExpression   * aInitExpr,
                              qlvRefExprList      * aRefExprList,
                              knlRegionMem        * aRegionMem,
                              qllEnv              * aEnv )
{
    qlvInitExpression  * sExpr     = NULL;
    qlvInitCaseExpr    * sCaseExpr = NULL;
    stlInt32             sCount    = 0;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );

    sCaseExpr = aInitExpr->mExpr.mCaseExpr;

    /**
     * Case의 When ... Then ... Expression들에 대한 Add Expression 수행
     */

    for( sCount = 0 ;
         sCount < sCaseExpr->mCount ;
         sCount++ )
    {
        /*
         * When 
         */
        
        sExpr = sCaseExpr->mWhenArr[ sCount ];
        STL_TRY( qloAddExpr( sExpr,
                             aRefExprList,
                             aRegionMem,
                             aEnv )
                 == STL_SUCCESS );

        /*
         * Then
         */
        
        sExpr = sCaseExpr->mThenArr[ sCount ];
        STL_TRY( qloAddExpr( sExpr,
                             aRefExprList,
                             aRegionMem,
                             aEnv )
                 == STL_SUCCESS );
    }

    /**
     * Case의 Default Result Expression에 대한 Add Expression 수행
     */

    sExpr = sCaseExpr->mDefResult;
    STL_TRY( qloAddExpr( sExpr,
                         aRefExprList,
                         aRegionMem,
                         aEnv )
             == STL_SUCCESS );

    /**
     * 현재 Expression을 Expression List에 추가
     */

    STL_TRY( qlvAddExprToRefExprList( aRefExprList,
                                      aInitExpr,
                                      STL_TRUE,
                                      aRegionMem,
                                      aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief Add Expression Function for List Function
 * @param[in]      aInitExpr        Init Expression Source
 * @param[in,out]  aRefExprList     Reference Expression List
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qloAddExprListFunction( qlvInitExpression   * aInitExpr,
                                  qlvRefExprList      * aRefExprList,
                                  knlRegionMem        * aRegionMem,
                                  qllEnv              * aEnv )
{
    qlvInitExpression  * sArgExpr   = NULL;
    stlInt32             sArgCount  = 0;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
      
    /**
     * List Func의 Argument Expression들에 대한 Add Expression 수행
     */

    for( sArgCount = 0 ;
         sArgCount < aInitExpr->mExpr.mListFunc->mArgCount ;
         sArgCount ++ )
    {
        sArgExpr = aInitExpr->mExpr.mListFunc->mArgs[ sArgCount ];
        
        STL_TRY( qloAddExpr( sArgExpr,
                             aRefExprList,
                             aRegionMem,
                             aEnv )
                 == STL_SUCCESS );
    }

    /**
     * Expression List에 Expression 추가
     */

    STL_TRY( qlvAddExprToRefExprList( aRefExprList,
                                      aInitExpr,
                                      STL_TRUE,
                                      aRegionMem,
                                      aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief Add Expression Function for List Column
 * @param[in]      aInitExpr        Init Expression Source
 * @param[in,out]  aRefExprList     Reference Expression List
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qloAddExprListColumn( qlvInitExpression   * aInitExpr,
                                qlvRefExprList      * aRefExprList,
                                knlRegionMem        * aRegionMem,
                                qllEnv              * aEnv )
{
    qlvInitExpression  * sArgExpr   = NULL;
    stlInt32             sArgCount  = 0;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
      
    /**
     * List Func의 Argument Expression들에 대한 Add Expression 수행
     */

    for( sArgCount = 0 ;
         sArgCount < aInitExpr->mExpr.mListCol->mArgCount ;
         sArgCount ++ )
    {
        sArgExpr = aInitExpr->mExpr.mListCol->mArgs[ sArgCount ];
        
        STL_TRY( qloAddExpr( sArgExpr,
                             aRefExprList,
                             aRegionMem,
                             aEnv )
                 == STL_SUCCESS );
    }

    /**
     * Expression List에 Expression 추가
     */

    STL_TRY( qlvAddExprToRefExprList( aRefExprList,
                                      aInitExpr,
                                      STL_TRUE,
                                      aRegionMem,
                                      aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief Add Expression Function for Row Column
 * @param[in]      aInitExpr        Init Expression Source
 * @param[in,out]  aRefExprList     Reference Expression List
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qloAddExprRowExpr( qlvInitExpression   * aInitExpr,
                             qlvRefExprList      * aRefExprList,
                             knlRegionMem        * aRegionMem,
                             qllEnv              * aEnv )
{
    qlvInitExpression  * sArgExpr   = NULL;
    stlInt32             sArgCount  = 0;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
      
    /**
     * List Func의 Argument Expression들에 대한 Add Expression 수행
     */

    for( sArgCount = 0 ;
         sArgCount < aInitExpr->mExpr.mRowExpr->mArgCount ;
         sArgCount ++ )
    {
        sArgExpr = aInitExpr->mExpr.mRowExpr->mArgs[ sArgCount ];
        
        STL_TRY( qloAddExpr( sArgExpr,
                             aRefExprList,
                             aRegionMem,
                             aEnv )
                 == STL_SUCCESS );
    }

    /**
     * Expression List에 Expression 추가
     */

    STL_TRY( qlvAddExprToRefExprList( aRefExprList,
                                      aInitExpr,
                                      STL_TRUE,
                                      aRegionMem,
                                      aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/** @} qlo */
