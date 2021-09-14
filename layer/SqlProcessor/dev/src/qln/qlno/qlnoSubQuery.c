/*******************************************************************************
 * qlnoSubQuery.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlnoSubQuery.c 8545 2013-05-23 01:55:08Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlnoSubQuery.c
 * @brief SQL Processor Layer Code Optimization Node - Sub Query
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
 * @brief Sub Table에 대한 Code Optimize를 수행한다.
 * @param[in]   aCodeOptParamInfo   Code Optimize Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlnoCodeOptimizeSubTable( qloCodeOptParamInfo     * aCodeOptParamInfo,
                                    qllEnv                  * aEnv )
{
    qlncPlanSubTable        * sPlanSubTable         = NULL;
    qlnoSubQuery            * sOptSubQuery          = NULL;
    qllOptimizationNode     * sOptNode              = NULL;
    qllEstimateCost         * sOptCost              = NULL;
    qllEstimateCost         * sBackupOptCost        = NULL;

    qllOptimizationNode     * sOptChildNode         = NULL;

    
    /*
     * Paramter Validation
     */

    QLO_CODE_OPT_DEFAULT_PARAM_VALIDATE( aCodeOptParamInfo, aEnv );
    STL_PARAM_VALIDATE( aCodeOptParamInfo->mCandPlan->mPlanType == QLNC_PLAN_TYPE_SUB_TABLE,
                        QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Sub Table Plan 정보 획득
     ****************************************************************/

    sPlanSubTable = (qlncPlanSubTable*)(aCodeOptParamInfo->mCandPlan);
    
    
    /****************************************************************
     * Optimization Node 생성
     ****************************************************************/

    /**
     * SUB-QUERY Optimization Node 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                STL_SIZEOF( qlnoSubQuery ),
                                (void **) & sOptSubQuery,
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
                                QLL_PLAN_NODE_SUB_QUERY_TYPE,
                                sOptSubQuery,
                                aCodeOptParamInfo->mOptQueryNode,
                                sOptCost,
                                NULL,
                                & sOptNode,
                                QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                aEnv )
             == STL_SUCCESS );

    
    /****************************************************************
     * Optimization Node 구성
     ****************************************************************/

    aCodeOptParamInfo->mCandPlan = sPlanSubTable->mChildPlanNode;
    sBackupOptCost = aCodeOptParamInfo->mOptCost;
    aCodeOptParamInfo->mOptCost = sOptCost;

    QLNO_CODE_OPTIMIZE( aCodeOptParamInfo, aEnv );

    aCodeOptParamInfo->mOptCost = sBackupOptCost;
    sOptChildNode = aCodeOptParamInfo->mOptNode;

    
    /****************************************************************
     * Rewrite Expression
     ****************************************************************/

    /**
     * Target Column List에 Expression 추가
     */

    STL_DASSERT( sPlanSubTable->mReadColList != NULL );
    sOptSubQuery->mTargetCount = sPlanSubTable->mReadColList->mCount;
    sOptSubQuery->mTargetList = sPlanSubTable->mReadColList;


    /**
     * Statement Expression List에 Read Column 추가
     */

    STL_DASSERT( sOptSubQuery->mTargetList != NULL );

    STL_TRY( qlvAddExprListToRefExprList(
                 aCodeOptParamInfo->mQueryExprList->mStmtExprList->mColumnExprList,
                 sOptSubQuery->mTargetList,
                 STL_FALSE,
                 QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                 aEnv )
             == STL_SUCCESS );

    
    /****************************************************************
     * SUB-QUERY Optimization Node 정보 설정
     ****************************************************************/

    /**
     * Child Optimization Node
     */
    
    sOptSubQuery->mChildNode = sOptChildNode;


    /**
     * Row Block이 필요한지 여부
     */

    sOptSubQuery->mNeedRowBlock = sPlanSubTable->mCommon.mNeedRowBlock;


    /**
     * Validation Node
     */
    
    sOptSubQuery->mInitNode = (qlvInitSubTableNode*)(sPlanSubTable->mCommon.mInitNode);


    /**
     * Outer Column List
     */

    sOptSubQuery->mOuterColumnList = sPlanSubTable->mOuterColumnList;


    /**
     * Filter Expression
     */
    
    sOptSubQuery->mFilterExpr = sPlanSubTable->mFilterExpr;

    
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
 * @brief Sub Query 의 All Expr리스트 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoAddExprSubQuery( qllDBCStmt            * aDBCStmt,
                               qllStatement          * aSQLStmt,
                               qllOptNodeList        * aOptNodeList,
                               qllOptimizationNode   * aOptNode,
                               qloInitExprList       * aQueryExprList,
                               qllEnv                * aEnv )
{
    qlnoSubQuery    * sOptSubQuery   = NULL;
    qlvRefExprItem  * sRefColumnItem = NULL;
    
        
    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );    


    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/
    
    sOptSubQuery = (qlnoSubQuery *) aOptNode->mOptNode;

    
    /****************************************************************
     * Child Node의 Add Expression 수행
     ****************************************************************/

    QLNO_ADD_EXPR( aDBCStmt,
                   aSQLStmt,
                   aOptNodeList,
                   sOptSubQuery->mChildNode,
                   aQueryExprList,
                   aEnv );


    /****************************************************************
     * Add Expr
     ****************************************************************/

    /**
     * Add Expr Target clause Expression
     */

    sRefColumnItem = sOptSubQuery->mTargetList->mHead;
    while( sRefColumnItem != NULL )
    {
        /**
         * Target Expression을 All Expression List에 추가
         */

        STL_TRY( qloAddExpr( sRefColumnItem->mExprPtr,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN( aDBCStmt ),
                             aEnv )
                 == STL_SUCCESS );

        sRefColumnItem = sRefColumnItem->mNext;
    }

    
    /**
     * Add Expr Filter Expression
     */

    if( sOptSubQuery->mFilterExpr != NULL )
    {
        STL_TRY( qloAddExpr( sOptSubQuery->mFilterExpr,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN(aDBCStmt),
                             aEnv )
                 == STL_SUCCESS );
    }


    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Sub Query 의 Optimization Node 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoCompleteSubQuery( qllDBCStmt            * aDBCStmt,
                                qllStatement          * aSQLStmt,
                                qllOptNodeList        * aOptNodeList,
                                qllOptimizationNode   * aOptNode,
                                qloInitExprList       * aQueryExprList,
                                qllEnv                * aEnv )
{
    qlnoSubQuery        * sOptSubQuery  = NULL;
    qlvRefExprItem      * sTargetItem   = NULL;
    qloDBType           * sExprDBType   = NULL;

    knlExprEntry        * sExprEntry    = NULL;
    knlExprStack        * sFilterStack  = NULL;
    knlPredicateList    * sFilterPred   = NULL;
    qlvInitExpression   * sCodeExpr     = NULL;


    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );    


    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/
    
    sOptSubQuery = (qlnoSubQuery *) aOptNode->mOptNode;
    
    
    /****************************************************************
     * 하위 node에 대한 Complete Node 수행
     ****************************************************************/

    QLNO_COMPLETE( aDBCStmt,
                   aSQLStmt,
                   aOptNodeList,
                   sOptSubQuery->mChildNode,
                   aQueryExprList,
                   aEnv );


    /****************************************************************
     * Filter Predicate 생성
     ****************************************************************/

    if( sOptSubQuery->mFilterExpr == NULL )
    {
        sFilterPred = NULL;
    }
    else
    {
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

        sCodeExpr = sOptSubQuery->mFilterExpr;

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
                                & sFilterPred,
                                QLL_CODE_PLAN( aDBCStmt ),
                                KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
    }


    /**
     * Target Column에 대한 DB Type 설정
     */

    sTargetItem = sOptSubQuery->mTargetList->mHead;
    while( sTargetItem != NULL )
    {
        STL_DASSERT( sTargetItem->mExprPtr->mType == QLV_EXPR_TYPE_INNER_COLUMN );

        sExprDBType = & aQueryExprList->mStmtExprList->mTotalExprInfo->mExprDBType
            [ sTargetItem->mExprPtr->mExpr.mInnerColumn->mOrgExpr->mOffset ];
        
        STL_TRY( qloSetExprDBType( sTargetItem->mExprPtr,
                                   aQueryExprList->mStmtExprList->mTotalExprInfo,
                                   sExprDBType->mDBType,
                                   sExprDBType->mArgNum1,
                                   sExprDBType->mArgNum2,
                                   sExprDBType->mStringLengthUnit,
                                   sExprDBType->mIntervalIndicator,
                                   aEnv )
                 == STL_SUCCESS );

        sTargetItem = sTargetItem->mNext;
    }


    /****************************************************************
     * Node 정보 설정
     ****************************************************************/

    /**
     * Filter Predicate
     */
    
    sOptSubQuery->mFilterPred = sFilterPred;


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


/** @} qlno */
