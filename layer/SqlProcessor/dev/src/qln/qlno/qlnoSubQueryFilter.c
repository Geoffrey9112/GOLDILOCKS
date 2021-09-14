/*******************************************************************************
 * qlnoSubQueryFilter.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlnoSubQueryFilter.c 8545 2013-05-23 01:55:08Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlnoSubQueryFilter.c
 * @brief SQL Processor Layer Code Optimization Node - Sub Query Filter
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlno
 * @{
 */

/***********************************************************************************
 * Code Optimization
 ***********************************************************************************/

/**
 * @brief Sub Query Filter에 대한 Code Optimize를 수행한다.
 * @param[in]   aCodeOptParamInfo   Code Optimize Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlnoCodeOptimizeSubQueryFilter( qloCodeOptParamInfo     * aCodeOptParamInfo,
                                          qllEnv                  * aEnv )
{
    qllOptimizationNode     * sOptChildNode         = NULL;
    qllOptimizationNode    ** sOptAndFilterNode     = NULL;
    qlnoSubQueryFilter      * sOptSubQueryFilter    = NULL;
    qllOptimizationNode     * sOptNode              = NULL;
    
    qllEstimateCost         * sOptCost              = NULL;
    qlncPlanSubQueryFilter  * sPlanSubQueryFilter   = NULL;
    qlnoSubQueryList        * sOptSubQueryList      = NULL;

    stlInt32                  sLoop;
    stlInt32                  sIdx;

    
    /*
     * Paramter Validation
     */

    QLO_CODE_OPT_DEFAULT_PARAM_VALIDATE( aCodeOptParamInfo, aEnv );
    STL_PARAM_VALIDATE( aCodeOptParamInfo->mCandPlan->mPlanType == QLNC_PLAN_TYPE_SUB_QUERY_FILTER,
                        QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Plan 정보 획득
     ****************************************************************/

    sPlanSubQueryFilter = (qlncPlanSubQueryFilter*)(aCodeOptParamInfo->mCandPlan);
    
    
    /****************************************************************
     * Optimization Node 생성
     ****************************************************************/

    /**
     * SUB-QUERY FILTER Optimization Node 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                STL_SIZEOF( qlnoSubQueryFilter ),
                                (void **) & sOptSubQueryFilter,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Optimization Cost 공간 할당
     */

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                STL_SIZEOF( qllEstimateCost ),
                                (void **) & sOptCost,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sOptCost, 0x00, STL_SIZEOF( qllEstimateCost ) );


    /**
     * Common Optimization Node 생성
     */

    STL_TRY( qlnoCreateOptNode( aCodeOptParamInfo->mOptNodeList,
                                QLL_PLAN_NODE_SUB_QUERY_FILTER_TYPE,
                                sOptSubQueryFilter,
                                aCodeOptParamInfo->mOptQueryNode,
                                sOptCost,
                                NULL,
                                & sOptNode,
                                QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                aEnv )
             == STL_SUCCESS );

    
    /****************************************************************
     * Child Node 구성
     ****************************************************************/

    STL_DASSERT( sPlanSubQueryFilter->mChildPlanNodes[0]->mPlanType == QLNC_PLAN_TYPE_SUB_QUERY_LIST );


    /**
     * Child Node Optimization
     */
    
    aCodeOptParamInfo->mCandPlan = sPlanSubQueryFilter->mChildPlanNodes[0];
    QLNO_CODE_OPTIMIZE( aCodeOptParamInfo, aEnv );
    sOptChildNode = aCodeOptParamInfo->mOptNode;

    
    /****************************************************************
     * And Filter Node 구성
     ****************************************************************/

    /**
     * And Filter Node 공간 할당
     */

    STL_TRY( knlAllocRegionMem(
                 QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                 STL_SIZEOF( qllOptimizationNode* ) * sPlanSubQueryFilter->mAndFilter->mAndFilterCnt,
                 (void **) & sOptAndFilterNode,
                 KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sOptAndFilterNode,
               0x00,
               STL_SIZEOF( qllOptimizationNode* ) * sPlanSubQueryFilter->mAndFilter->mAndFilterCnt );
    

    /**
     * And Filter Node Optimization
     */

    sOptAndFilterNode[ 0 ] = sOptChildNode;

    for( sLoop = 1 ; sLoop < sPlanSubQueryFilter->mAndFilter->mAndFilterCnt ; sLoop++ )
    {
        aCodeOptParamInfo->mCandPlan = sPlanSubQueryFilter->mChildPlanNodes[ sLoop ];
        QLNO_CODE_OPTIMIZE( aCodeOptParamInfo, aEnv );
        sOptAndFilterNode[ sLoop ] = aCodeOptParamInfo->mOptNode;

        STL_DASSERT( sOptAndFilterNode[ sLoop ]->mType == QLL_PLAN_NODE_SUB_QUERY_LIST_TYPE );
        
        sOptSubQueryList = (qlnoSubQueryList *) sOptAndFilterNode[ sLoop ]->mOptNode;
            
        for( sIdx = 0 ; sIdx < sOptSubQueryList->mSubQueryNodeCnt ; sIdx++ )
        {
            if( sOptSubQueryList->mSubQueryNodes[ sIdx ]->mType == QLL_PLAN_NODE_SUB_QUERY_FUNCTION_TYPE )
            {
                ((qlnoSubQueryFunc*)(sOptSubQueryList->mSubQueryNodes[ sIdx ]->mOptNode))->mChildNode =
                    ((qlnoSubQueryList *) sOptChildNode->mOptNode)->mChildNode;
            }
        }
    }
    

    /****************************************************************
     * SUB-QUERY Optimization Node 정보 설정
     ****************************************************************/

    /**
     * Child Optimization Node
     */
    
    sOptSubQueryFilter->mChildNode = sOptChildNode;

    
    /**
     * And Filter Node 관련
     */

    sOptSubQueryFilter->mAndFilterNodeArr = sOptAndFilterNode;
    

    /**
     * Row Block이 필요한지 여부
     */

    sOptSubQueryFilter->mNeedRowBlock = sPlanSubQueryFilter->mCommon.mNeedRowBlock;


    /**
     * Filter Expression
     */
    
    sOptSubQueryFilter->mFilterExprArr = sPlanSubQueryFilter->mAndFilter->mInitSubQueryFilter;
    sOptSubQueryFilter->mAndFilterCnt  = sPlanSubQueryFilter->mAndFilter->mAndFilterCnt;

    
    /****************************************************************
     * OUTPUT 설정
     ****************************************************************/

    /**
     * 결과 Common Optimization Node 설정
     */

    aCodeOptParamInfo->mOptNode = sOptNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Sub Query Filter의 Optimization Node 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoCompleteSubQueryFilter( qllDBCStmt            * aDBCStmt,
                                      qllStatement          * aSQLStmt,
                                      qllOptNodeList        * aOptNodeList,
                                      qllOptimizationNode   * aOptNode,
                                      qloInitExprList       * aQueryExprList,
                                      qllEnv                * aEnv )
{
    qlnoSubQueryFilter  * sOptSubQueryFilter = NULL;
    
    knlExprEntry        * sExprEntry         = NULL;
    knlExprStack        * sFilterStack       = NULL;
    knlPredicateList   ** sFilterPredArr     = NULL;
    qlvInitExpression   * sCodeExpr          = NULL;
    stlInt32              sLoop              = 0;
    

    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_FILTER_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/
    
    sOptSubQueryFilter = (qlnoSubQueryFilter *) aOptNode->mOptNode;
    
    
    /****************************************************************
     * 하위 node에 대한 Complete Node 수행
     ****************************************************************/

    for( sLoop = 0 ; sLoop < sOptSubQueryFilter->mAndFilterCnt ; sLoop++ )
    {
        QLNO_COMPLETE( aDBCStmt,
                       aSQLStmt,
                       aOptNodeList,
                       sOptSubQueryFilter->mAndFilterNodeArr[ sLoop ],
                       aQueryExprList,
                       aEnv );
    }
    

    /****************************************************************
     * Filter Predicate 생성
     ****************************************************************/

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( knlPredicateList* ) * sOptSubQueryFilter->mAndFilterCnt,
                                (void **) & sFilterPredArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    for( sLoop = 0 ; sLoop < sOptSubQueryFilter->mAndFilterCnt ; sLoop++ )
    {
        STL_DASSERT( sOptSubQueryFilter->mFilterExprArr[ sLoop ] != NULL );

        /**
         * Filter Expression Code Stack 공간 할당
         */

        STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                    STL_SIZEOF( knlExprStack ),
                                    (void **) & sFilterStack,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );


        /**
         * Filter Expression Stack 생성
         */

        sCodeExpr = sOptSubQueryFilter->mFilterExprArr[ sLoop ];

        STL_TRY( knlExprInit( sFilterStack,
                              sCodeExpr->mIncludeExprCnt,
                              QLL_CODE_PLAN( aDBCStmt ),
                              KNL_ENV( aEnv ) )
                 == STL_SUCCESS );


        /**
         * Logical Expression Stack 구성
         */

        STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                            sCodeExpr,
                                            aSQLStmt->mBindContext,
                                            sFilterStack,
                                            aQueryExprList->mStmtExprList->mConstExprStack,
                                            aQueryExprList->mStmtExprList->mTotalExprInfo,
                                            & sExprEntry,
                                            QLL_CODE_PLAN(aDBCStmt),
                                            aEnv )
                 == STL_SUCCESS );

        STL_TRY_THROW( qloCastExprDBType(
                           sCodeExpr,
                           aQueryExprList->mStmtExprList->mTotalExprInfo,
                           DTL_TYPE_BOOLEAN,
                           gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mArgNum1,
                           gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mArgNum2,
                           gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mStringLengthUnit,
                           gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mIntervalIndicator,
                           STL_FALSE,
                           STL_FALSE,
                           aEnv )
                       == STL_SUCCESS,
                       RAMP_ERR_INVALID_WHERE_CLAUSE );


        /**
         * Logical Expression에 대한 Filter Predicate 구성
         */

        STL_TRY( knlMakeFilter( sFilterStack,
                                & sFilterPredArr[ sLoop ],
                                QLL_CODE_PLAN( aDBCStmt ),
                                KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
    }
    

    /****************************************************************
     * Node 정보 설정
     ****************************************************************/

    /**
     * Filter Predicate
     */
    
    sOptSubQueryFilter->mFilterPredArr = sFilterPredArr;


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_WHERE_CLAUSE )
    {
        (void)stlPopError( QLL_ERROR_STACK(aEnv) );

        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_BOOLEAN_EXPRESSION_EXPECTED,
                      qlgMakeErrPosString( aSQLStmt->mRetrySQL,
                                           sCodeExpr->mPosition,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Sub Query Filter의 All Expr리스트 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoAddExprSubQueryFilter( qllDBCStmt            * aDBCStmt,
                                     qllStatement          * aSQLStmt,
                                     qllOptNodeList        * aOptNodeList,
                                     qllOptimizationNode   * aOptNode,
                                     qloInitExprList       * aQueryExprList,
                                     qllEnv                * aEnv )
{
    qlnoSubQueryFilter  * sOptSubQueryFilter = NULL;
    stlInt32              sLoop              = 0;
    
        
    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_FILTER_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/
    
    sOptSubQueryFilter = (qlnoSubQueryFilter *) aOptNode->mOptNode;

    
    /****************************************************************
     * Child Node의 Add Expression 수행
     ****************************************************************/

    for( sLoop = 0 ; sLoop < sOptSubQueryFilter->mAndFilterCnt ; sLoop++ )
    {
        QLNO_ADD_EXPR( aDBCStmt,
                       aSQLStmt,
                       aOptNodeList,
                       sOptSubQueryFilter->mAndFilterNodeArr[ sLoop ],
                       aQueryExprList,
                       aEnv );
    }


    /****************************************************************
     * Add Expr
     ****************************************************************/

    /**
     * Add Expr Filter Expression
     */

    for( sLoop = 0 ; sLoop < sOptSubQueryFilter->mAndFilterCnt ; sLoop++ )
    {
        if( sOptSubQueryFilter->mFilterExprArr[ sLoop ] != NULL )
        {
            STL_TRY( qloAddExpr( sOptSubQueryFilter->mFilterExprArr[ sLoop ],
                                 aQueryExprList->mStmtExprList->mAllExprList,
                                 QLL_CODE_PLAN(aDBCStmt),
                                 aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /* Do Nothing */
        }
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlno */
