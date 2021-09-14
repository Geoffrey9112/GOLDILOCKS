/*******************************************************************************
 * qlnoSubQueryFunc.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlnoSubQueryFunc.c 8545 2013-05-23 01:55:08Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlnoSubQueryFunc.c
 * @brief SQL Processor Layer Code Optimization Node - Sub Query Func
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
 * @brief Sub Query Function에 대한 Code Optimize를 수행한다.
 * @param[in]   aCodeOptParamInfo   Code Optimize Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlnoCodeOptimizeSubQueryFunc( qloCodeOptParamInfo     * aCodeOptParamInfo,
                                        qllEnv                  * aEnv )
{
    qllOptimizationNode   * sRightOperandNode = NULL;
    qllOptimizationNode   * sOptNode          = NULL;
    qlnoSubQueryFunc      * sOptSubQueryFunc  = NULL;
    
    qllEstimateCost       * sOptCost          = NULL;
    qlncPlanSubQueryFunc  * sPlanSubQueryFunc = NULL;
    
    
    /*
     * Paramter Validation
     */

    QLO_CODE_OPT_DEFAULT_PARAM_VALIDATE( aCodeOptParamInfo, aEnv );
    STL_PARAM_VALIDATE( aCodeOptParamInfo->mCandPlan->mPlanType == QLNC_PLAN_TYPE_SUB_QUERY_FUNCTION,
                        QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Plan 정보 획득
     ****************************************************************/

    sPlanSubQueryFunc = (qlncPlanSubQueryFunc*)(aCodeOptParamInfo->mCandPlan);
    
    
    /****************************************************************
     * Optimization Node 생성
     ****************************************************************/

    /**
     * SUB-QUERY FUNC Optimization Node 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                STL_SIZEOF( qlnoSubQueryFunc ),
                                (void **) & sOptSubQueryFunc,
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
                                QLL_PLAN_NODE_SUB_QUERY_FUNCTION_TYPE,
                                sOptSubQueryFunc,
                                aCodeOptParamInfo->mOptQueryNode,
                                sOptCost,
                                NULL,
                                & sOptNode,
                                QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                aEnv )
             == STL_SUCCESS );

    
    /****************************************************************
     * Operand Node 구성
     ****************************************************************/

    /**
     * Right Operand Node Optimization
     */
    
    STL_DASSERT( sPlanSubQueryFunc->mRightOperandPlanNode != NULL );
    
    aCodeOptParamInfo->mCandPlan = sPlanSubQueryFunc->mRightOperandPlanNode;
    QLNO_CODE_OPTIMIZE( aCodeOptParamInfo, aEnv );
    sRightOperandNode = aCodeOptParamInfo->mOptNode;

    
    /****************************************************************
     * SUB-QUERY Optimization Node 정보 설정
     ****************************************************************/

    /**
     * Child Optimization Node
     */
    
    sOptSubQueryFunc->mChildNode = NULL;

    
    /**
     * Operand Node 관련
     */

    sOptSubQueryFunc->mRightOperandNode = sRightOperandNode;
    

    /**
     * Function Expression
     */
    
    sOptSubQueryFunc->mFuncExpr   = sPlanSubQueryFunc->mFuncExpr;
    sOptSubQueryFunc->mResultExpr = sPlanSubQueryFunc->mResultExpr;

    
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
 * @brief Sub Query Function의 Optimization Node 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoCompleteSubQueryFunc( qllDBCStmt            * aDBCStmt,
                                    qllStatement          * aSQLStmt,
                                    qllOptNodeList        * aOptNodeList,
                                    qllOptimizationNode   * aOptNode,
                                    qloInitExprList       * aQueryExprList,
                                    qllEnv                * aEnv )
{
    qlnoSubQueryFunc    * sOptSubQueryFunc   = NULL;
    knlExprStack        * sFuncExprStack     = NULL;
    knlExprEntry        * sExprEntry         = NULL;
    knlExprStack       ** sLeftOperCodeStack = NULL;
    qlvInitListFunc     * sListFunc          = NULL;
    qlvInitRowExpr      * sLeftRowExpr       = NULL;
    qlvInitRowExpr      * sRightRowExpr      = NULL;

    qloDBType           * sDBTypes           = NULL;
    stlInt32              sLoop              = 0;

    const dtlCompareType    * sCompType;
        

    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_FUNCTION_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/
    
    sOptSubQueryFunc = (qlnoSubQueryFunc *) aOptNode->mOptNode;
    
    
    /****************************************************************
     * 하위 node에 대한 Complete Node 수행
     ****************************************************************/

    if( sOptSubQueryFunc->mRightOperandNode != NULL )
    {
        QLNO_COMPLETE( aDBCStmt,
                       aSQLStmt,
                       aOptNodeList,
                       sOptSubQueryFunc->mRightOperandNode,
                       aQueryExprList,
                       aEnv );
    }
    

    /****************************************************************
     * Function Expression Complete
     ****************************************************************/
    
    /**
     * Sub Query Function을 위한 임시 Expression Stack 공간 할당
     * @remark Candidate memory manager 사용
     */

    STL_TRY( knlAllocRegionMem( & QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( knlExprStack ),
                                (void **) & sFuncExprStack,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

        
    /**
     * Sub Query Function을 위한 임시 Expression Stack 구성
     * @remark Candidate memory manager 사용
     */

    STL_TRY( knlExprInit( sFuncExprStack,
                          sOptSubQueryFunc->mFuncExpr->mIncludeExprCnt,
                          & QLL_CANDIDATE_MEM( aEnv ),
                          KNL_ENV( aEnv ) )
             == STL_SUCCESS );

        
    /**
     * Sub Query Function에 대한 Data Type 설정
     */

    STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                        sOptSubQueryFunc->mFuncExpr,
                                        aSQLStmt->mBindContext,
                                        sFuncExprStack,
                                        aQueryExprList->mStmtExprList->mConstExprStack,
                                        aQueryExprList->mStmtExprList->mTotalExprInfo,
                                        & sExprEntry,
                                        QLL_CODE_PLAN( aDBCStmt ),
                                        aEnv )
             == STL_SUCCESS );


    /**
     * Result Expression Data Type 설정
     */
    
    STL_TRY( qloSetExprDBType( sOptSubQueryFunc->mResultExpr,
                               aQueryExprList->mStmtExprList->mTotalExprInfo,
                               DTL_TYPE_BOOLEAN,
                               gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mArgNum1,
                               gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mArgNum2,
                               gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mStringLengthUnit,
                               gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mIntervalIndicator,
                               aEnv )
             == STL_SUCCESS );


    /**
     * Left Operand에 대한 Expression Stack 구성
     */

    STL_DASSERT( sOptSubQueryFunc->mFuncExpr != NULL );
    STL_DASSERT( sOptSubQueryFunc->mFuncExpr->mType == QLV_EXPR_TYPE_LIST_FUNCTION );

    sListFunc = sOptSubQueryFunc->mFuncExpr->mExpr.mListFunc;
    sDBTypes  = aQueryExprList->mStmtExprList->mTotalExprInfo->mExprDBType;
    
    if( sListFunc->mArgCount == 1 )
    {
        sLeftOperCodeStack = NULL;
    }
    else
    {
        STL_DASSERT( sListFunc->mArgCount == 2 );
        STL_DASSERT( sListFunc->mArgs[ 1 ]->mExpr.mListCol->mArgCount == 1 );

        sRightRowExpr = sListFunc->mArgs[ 0 ]->mExpr.mListCol->mArgs[ 0 ]->mExpr.mRowExpr;
        sLeftRowExpr  = sListFunc->mArgs[ 1 ]->mExpr.mListCol->mArgs[ 0 ]->mExpr.mRowExpr;


        /**
         * Expression Stack Array 공간 할당
         */

        STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                    STL_SIZEOF( knlExprStack * ) * sRightRowExpr->mArgCount,
                                    (void **) & sLeftOperCodeStack,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        
        /**
         * Expression Stack Array 정보 구성
         */
        
        for( sLoop = 0 ; sLoop < sRightRowExpr->mArgCount ; sLoop++ )
        {
            /* 각 Expression에 대한 임시 Expression Stack 구성 */
            sFuncExprStack->mEntryCount = 0;
            
            STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                                sLeftRowExpr->mArgs[ sLoop ],
                                                aSQLStmt->mBindContext,
                                                sFuncExprStack,
                                                aQueryExprList->mStmtExprList->mConstExprStack,
                                                aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                & sExprEntry,
                                                QLL_CODE_PLAN( aDBCStmt ),
                                                aEnv )
                     == STL_SUCCESS );
            
            /* Physical Compare 가 가능하도록 양쪽 Expression에 Internal Cast 추가 */
            /* Right Operand도 Instant Table을 구성하기 전에 Cast를 수행하므로
             * Constant Value로 취급하여 Scan에 유리한 Type으로 미리 만들어 놓는다. */
            STL_TRY( qlnoGetCompareType( aSQLStmt->mRetrySQL,
                                         sDBTypes[ sLeftRowExpr->mArgs[ sLoop ]->mOffset ].mDBType,
                                         sDBTypes[ sRightRowExpr->mArgs[ sLoop ]->mOffset ].mDBType,
                                         STL_FALSE,
                                         STL_FALSE,
                                         sOptSubQueryFunc->mFuncExpr->mPosition,
                                         &sCompType,
                                         aEnv )
                     == STL_SUCCESS );

            STL_DASSERT( dtlPhysicalCompareFuncList
                         [ sCompType->mLeftType ]
                         [ sCompType->mRightType ] != NULL );

            /* 최종 DataType 정보로 Cast */
            STL_DASSERT( sCompType->mLeftType != DTL_TYPE_NA );
            if( sDBTypes[ sLeftRowExpr->mArgs[ sLoop ]->mOffset ].mDBType != sCompType->mLeftType )
            {
                STL_TRY( qloCastExprDBType(
                             sLeftRowExpr->mArgs[ sLoop ],
                             aQueryExprList->mStmtExprList->mTotalExprInfo,
                             sCompType->mLeftType,
                             gResultDataTypeDef[ sCompType->mLeftType ].mArgNum1,
                             gResultDataTypeDef[ sCompType->mLeftType ].mArgNum2,
                             gResultDataTypeDef[ sCompType->mLeftType ].mStringLengthUnit,
                             gResultDataTypeDef[ sCompType->mLeftType ].mIntervalIndicator,
                             STL_FALSE,
                             STL_FALSE,
                             aEnv )
                         == STL_SUCCESS );
            }
            
            /* Right Operand도 Instant Table을 구성하기 전에 Cast를 수행하므로
             * 별도의 internal cast가 올 수 없다. */
            STL_DASSERT( sCompType->mRightType != DTL_TYPE_NA );
            STL_DASSERT( sDBTypes[ sRightRowExpr->mArgs[ sLoop ]->mOffset ].mDBType == sCompType->mRightType );
            
            /* Expression Stack 구성 */
            if( ( sFuncExprStack->mEntries[1].mExprType == KNL_EXPR_TYPE_VALUE ) &&
                ( sFuncExprStack->mEntryCount == 1 ) &&
                ( sDBTypes[ sLeftRowExpr->mArgs[ sLoop ]->mOffset ].mIsNeedCast == STL_FALSE ) )
            {
                /* 불필요한 Stack 제거 */
                sLeftOperCodeStack[ sLoop ] = NULL;
            }
            else
            {
                /* 임시 Expression Stack 복사하여 실제 Expression Stack 구성 */
                STL_TRY( knlExprCopy( sFuncExprStack,
                                      & sLeftOperCodeStack[ sLoop ],
                                      QLL_CODE_PLAN( aDBCStmt ),
                                      KNL_ENV( aEnv ) )
                         == STL_SUCCESS );
            }
        }
    }
    

    /****************************************************************
     * Node 정보 설정
     ****************************************************************/

    sOptSubQueryFunc->mLeftOperCodeStack = sLeftOperCodeStack;
    

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Sub Query Function의 All Expr리스트 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoAddExprSubQueryFunc( qllDBCStmt            * aDBCStmt,
                                   qllStatement          * aSQLStmt,
                                   qllOptNodeList        * aOptNodeList,
                                   qllOptimizationNode   * aOptNode,
                                   qloInitExprList       * aQueryExprList,
                                   qllEnv                * aEnv )
{
    qlnoSubQueryFunc  * sOptSubQueryFunc = NULL;
    
        
    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_FUNCTION_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/
    
    sOptSubQueryFunc = (qlnoSubQueryFunc *) aOptNode->mOptNode;

    
    /****************************************************************
     * Operand Node의 Add Expression 수행
     ****************************************************************/

    if( sOptSubQueryFunc->mRightOperandNode != NULL )
    {
        QLNO_ADD_EXPR( aDBCStmt,
                       aSQLStmt,
                       aOptNodeList,
                       sOptSubQueryFunc->mRightOperandNode,
                       aQueryExprList,
                       aEnv );
    }


    /****************************************************************
     * Add Expr
     ****************************************************************/

    /**
     * Add Expr Function Expression
     */

    STL_TRY( qloAddExpr( sOptSubQueryFunc->mFuncExpr,
                         aQueryExprList->mStmtExprList->mAllExprList,
                         QLL_CODE_PLAN(aDBCStmt),
                         aEnv )
             == STL_SUCCESS );

    
    /**
     * Add Expr Function Expression
     */

    STL_TRY( qloAddExpr( sOptSubQueryFunc->mResultExpr,
                         aQueryExprList->mStmtExprList->mAllExprList,
                         QLL_CODE_PLAN(aDBCStmt),
                         aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlno */
