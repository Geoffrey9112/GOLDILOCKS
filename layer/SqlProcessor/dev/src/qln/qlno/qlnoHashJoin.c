/*******************************************************************************
 * qlnoHashJoin.c
 *
 * Copyright (c) 2013, SUNJESOFT Inc.
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
 * @file qlnoHashJoin.c
 * @brief SQL Processor Layer Code Optimization Node - HASH JOIN
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlno
 * @{
 */


/****************************************************************************
 * Node Optimization Function
 ***************************************************************************/


/**
 * @brief Hash Join에 대한 Code Optimize를 수행한다.
 * @param[in]   aCodeOptParamInfo   Code Optimize Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlnoCodeOptimizeHash( qloCodeOptParamInfo * aCodeOptParamInfo,
                                qllEnv              * aEnv )
{
    qlncPlanHashJoin    * sPlanHashJoin     = NULL;
    qllOptimizationNode * sJoinOptNode      = NULL;
    qlnoHashJoin        * sOptHashJoin      = NULL;
    qllEstimateCost     * sOptCost          = NULL;

    qlvRefExprList      * sRefColumnList    = NULL;


    /*
     * Paramter Validation
     */

    QLO_CODE_OPT_DEFAULT_PARAM_VALIDATE( aCodeOptParamInfo, aEnv );
    STL_PARAM_VALIDATE( aCodeOptParamInfo->mCandPlan->mPlanType == QLNC_PLAN_TYPE_HASH_JOIN,
                        QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Hash Join Plan 정보 획득
     ****************************************************************/

    sPlanHashJoin = (qlncPlanHashJoin*)(aCodeOptParamInfo->mCandPlan);


    /****************************************************************
     * Optimization Node 생성
     ****************************************************************/

    /**
     * HASH JOIN Optimization Node 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                STL_SIZEOF( qlnoHashJoin ),
                                (void **) &sOptHashJoin,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sOptHashJoin, 0x00, STL_SIZEOF( qlnoHashJoin ) );


    /**
     * Optimization Cost 공간 할당
     */

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                STL_SIZEOF( qllEstimateCost ),
                                (void **) &sOptCost,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sOptCost, 0x00, STL_SIZEOF( qllEstimateCost ) );


    /**
     * Common Optimization Node 생성
     */

    STL_TRY( qlnoCreateOptNode( aCodeOptParamInfo->mOptNodeList,
                                QLL_PLAN_NODE_HASH_JOIN_TYPE,
                                sOptHashJoin,
                                aCodeOptParamInfo->mOptQueryNode,  /* Query Node */
                                sOptCost,
                                NULL,
                                &sJoinOptNode,
                                QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                aEnv )
             == STL_SUCCESS );


    /**
     * Left Child Node에 대한 Optimization Node 생성
     */

    aCodeOptParamInfo->mCandPlan = sPlanHashJoin->mLeftChildPlanNode;
    QLNO_CODE_OPTIMIZE( aCodeOptParamInfo, aEnv );
    sOptHashJoin->mLeftChildNode = aCodeOptParamInfo->mOptNode;


    /**
     * Right Child Node에 대한 Optimization Node 생성
     */

    aCodeOptParamInfo->mCandPlan = sPlanHashJoin->mRightChildPlanNode;
    QLNO_CODE_OPTIMIZE( aCodeOptParamInfo, aEnv );
    sOptHashJoin->mRightChildNode = aCodeOptParamInfo->mOptNode;

    STL_DASSERT( sOptHashJoin->mRightChildNode->mType == QLL_PLAN_NODE_HASH_JOIN_INSTANT_ACCESS_TYPE );
    
    /* NULL Skip 여부 */
    switch( sPlanHashJoin->mJoinType )
    {
        case QLV_JOIN_TYPE_INNER :
        case QLV_JOIN_TYPE_LEFT_OUTER :
        case QLV_JOIN_TYPE_LEFT_SEMI :
        case QLV_JOIN_TYPE_LEFT_ANTI_SEMI :
            /* Do Nothing */
            break;
        case QLV_JOIN_TYPE_CROSS :
        case QLV_JOIN_TYPE_RIGHT_OUTER :
        case QLV_JOIN_TYPE_RIGHT_SEMI :
        case QLV_JOIN_TYPE_RIGHT_ANTI_SEMI :
        case QLV_JOIN_TYPE_RIGHT_ANTI_SEMI_NA :
        case QLV_JOIN_TYPE_INVERTED_RIGHT_SEMI :
            STL_DASSERT( 0 );
            break;
        default :
            ((qlnoInstant*)(sOptHashJoin->mRightChildNode->mOptNode))->mHashTableAttr.mValidFlags |= SML_INDEX_SKIP_NULL_NO;
            ((qlnoInstant*)(sOptHashJoin->mRightChildNode->mOptNode))->mHashTableAttr.mSkipNullFlag = STL_FALSE;
            break;
    }

    /* Skip Hit 여부 */
    switch( sPlanHashJoin->mJoinType )
    {
        case QLV_JOIN_TYPE_INVERTED_LEFT_SEMI :
        case QLV_JOIN_TYPE_INVERTED_RIGHT_SEMI :
            ((qlnoInstant*)(sOptHashJoin->mRightChildNode->mOptNode))->mIsSkipHitRecord = STL_TRUE;
            break;
        default :
            ((qlnoInstant*)(sOptHashJoin->mRightChildNode->mOptNode))->mIsSkipHitRecord = STL_FALSE;
            break;
    }


    /****************************************************************
     * Read Column List 구성
     ****************************************************************/

    /**
     * Read Column List 구성
     */

    sRefColumnList = sPlanHashJoin->mReadColList;


    /**
     * Statement Expression List에 Join Read Column 추가
     */

    STL_TRY( qlvAddExprListToRefExprList(
                 aCodeOptParamInfo->mQueryExprList->mStmtExprList->mColumnExprList,
                 sRefColumnList,
                 STL_FALSE,
                 QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                 aEnv )
             == STL_SUCCESS );


    /****************************************************************
     * Optimize 정보 연결
     ****************************************************************/

    /**
     * Row Block이 필요한지 여부
     */

    sOptHashJoin->mNeedRowBlock = sPlanHashJoin->mCommon.mNeedRowBlock;


    /**
     * Join Type
     */

    sOptHashJoin->mJoinType = sPlanHashJoin->mJoinType;


    /**
     * Joined Column List
     */

    sOptHashJoin->mJoinColList = sRefColumnList;


    /**
     * Join Table Node
     */

    sOptHashJoin->mJoinTableNode = (qlvInitJoinTableNode*)(sPlanHashJoin->mCommon.mInitNode);


    /**
     * Join Left Outer Column List
     */

    sOptHashJoin->mLeftOuterColumnList = sPlanHashJoin->mJoinLeftOuterColumnList;


    /**
     * Join Right Outer Column List
     */

    sOptHashJoin->mRightOuterColumnList = sPlanHashJoin->mJoinRightOuterColumnList;

 
    /**
     * Outer Column 관련
     */

    sOptHashJoin->mOuterColumnList = sPlanHashJoin->mOuterColumnList;


    /**
     * Where Clause Logical Filter 설정
     */

    sOptHashJoin->mWhereFilterExpr = sPlanHashJoin->mWhereFilterExpr;


    /**
     * Where Clause Logical Filter Predicate 설정
     */

    sOptHashJoin->mWhereFilterPred = NULL;


    /**
     * Join Filiter Expression 설정
     */

    sOptHashJoin->mJoinFilterExpr = sPlanHashJoin->mJoinFilterExpr;


    /**
     * Join Filter Predicate 설정
     */

    sOptHashJoin->mJoinFilterPred = NULL;


    /****************************************************************
     * 마무리
     ****************************************************************/

    /**
     * Output 설정
     */

    aCodeOptParamInfo->mOptNode = sJoinOptNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/****************************************************************************
 * Complete Building Optimization Node Functions
 ***************************************************************************/

/**
 * @brief HASH JOIN의 Optimization Node 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoCompleteHashJoin( qllDBCStmt            * aDBCStmt,
                                qllStatement          * aSQLStmt,
                                qllOptNodeList        * aOptNodeList,
                                qllOptimizationNode   * aOptNode,
                                qloInitExprList       * aQueryExprList,
                                qllEnv                * aEnv )
{
    qlnoHashJoin        * sOptHashJoin     = NULL;

    knlExprStack        * sLogicalFilterStack   = NULL;
    knlPredicateList    * sLogicalFilterPred    = NULL;

    knlExprStack        * sJoinFilterStack      = NULL;
    knlPredicateList    * sJoinFilterPred       = NULL;
    
    qlvInitExpression   * sCodeExpr             = NULL;
    knlExprEntry        * sExprEntry            = NULL;

    qlvInitDataTypeInfo   sDataTypeInfo;
    qlvRefExprItem      * sExprItem             = NULL;
    qlvInitExpression   * sOrgExpr              = NULL;
    qloDBType           * sDBType               = NULL;
    ellDictHandle       * sColumnHandle         = NULL;

    knlExprStack          sTmpSortKeyExprStack;
    
    
    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_HASH_JOIN_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );    


    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/

    sOptHashJoin = (qlnoHashJoin *) aOptNode->mOptNode;

    /* Temp Sort Key Expression Stack 초기화 */
    STL_TRY( knlExprInit( &sTmpSortKeyExprStack,
                          1,
                          QLL_CODE_PLAN( aDBCStmt ),
                          KNL_ENV( aEnv ) )
             == STL_SUCCESS );


    /****************************************************************
     * 하위 node에 대한 Complete Node 수행
     ****************************************************************/

    QLNO_COMPLETE( aDBCStmt,
                   aSQLStmt,
                   aOptNodeList,
                   sOptHashJoin->mLeftChildNode,
                   aQueryExprList,
                   aEnv );

    QLNO_COMPLETE( aDBCStmt,
                   aSQLStmt,
                   aOptNodeList,
                   sOptHashJoin->mRightChildNode,
                   aQueryExprList,
                   aEnv );

    
    /****************************************************************
     * Join Column에 대한 DB Type 설정
     ****************************************************************/

    sExprItem = sOptHashJoin->mJoinColList->mHead;
    
    while( sExprItem != NULL )
    {
        STL_DASSERT( sExprItem->mExprPtr->mType == QLV_EXPR_TYPE_INNER_COLUMN );
        
        sOrgExpr = sExprItem->mExprPtr->mExpr.mInnerColumn->mOrgExpr;

        if( sOrgExpr->mType == QLV_EXPR_TYPE_COLUMN )
        {
            sColumnHandle = sOrgExpr->mExpr.mColumn->mColumnHandle;
            
            /* sOrgExpr 의 COLUMN 에 대한 DBType 설정 */
            STL_TRY( qloSetExprDBType( sOrgExpr,
                                       aQueryExprList->mStmtExprList->mTotalExprInfo,
                                       ellGetColumnDBType( sColumnHandle ),
                                       ellGetColumnPrecision( sColumnHandle ),
                                       ellGetColumnScale( sColumnHandle ),
                                       ellGetColumnStringLengthUnit( sColumnHandle ),
                                       ellGetColumnIntervalIndicator( sColumnHandle ),
                                       aEnv )
                     == STL_SUCCESS );
        }
        else if( sOrgExpr->mType == QLV_EXPR_TYPE_ROWID_COLUMN )
        {
            /* sOrgExpr 의 ROWID COLUMN 에 대한 DBType 설정 */
            STL_TRY( qloSetExprDBType( sOrgExpr,
                                       aQueryExprList->mStmtExprList->mTotalExprInfo,
                                       DTL_TYPE_ROWID,
                                       gDefaultDataTypeDef[ DTL_TYPE_ROWID ].mArgNum1,
                                       gDefaultDataTypeDef[ DTL_TYPE_ROWID ].mArgNum2,
                                       gDefaultDataTypeDef[ DTL_TYPE_ROWID ].mStringLengthUnit,
                                       gDefaultDataTypeDef[ DTL_TYPE_ROWID ].mIntervalIndicator,
                                       aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            STL_DASSERT( sOrgExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN );
 
            /* sOrgExpr 에 대한 DBType 설정 */
            sDBType  = & aQueryExprList->mStmtExprList->mTotalExprInfo->mExprDBType[ sOrgExpr->mOffset ];
            STL_TRY( qloSetExprDBType( sOrgExpr,
                                       aQueryExprList->mStmtExprList->mTotalExprInfo,
                                       sDBType->mDBType,
                                       sDBType->mArgNum1,
                                       sDBType->mArgNum2,
                                       sDBType->mStringLengthUnit,
                                       sDBType->mIntervalIndicator,
                                       aEnv )
                     == STL_SUCCESS );
        }

        STL_TRY( qlvGetExprDataTypeInfo( aSQLStmt->mRetrySQL,
                                         QLL_CODE_PLAN( aDBCStmt ),
                                         sExprItem->mExprPtr,
                                         aSQLStmt->mBindContext,
                                         &sDataTypeInfo,
                                         aEnv )
                 == STL_SUCCESS );
        
        /* INNER COLUMN에 대한 DBType 설정 */
        STL_TRY( qloSetExprDBType( sExprItem->mExprPtr,
                                   aQueryExprList->mStmtExprList->mTotalExprInfo,
                                   sDataTypeInfo.mDataType,
                                   sDataTypeInfo.mArgNum1,
                                   sDataTypeInfo.mArgNum2,
                                   sDataTypeInfo.mStringLengthUnit,
                                   sDataTypeInfo.mIntervalIndicator,
                                   aEnv )
                 == STL_SUCCESS );

        sExprItem = sExprItem->mNext;
    }


    /****************************************************************
     * Join Condition Filter
     ****************************************************************/
    
    /**
     * Join Condition Filter를 Logical Filter 형태로 생성
     */

    if( sOptHashJoin->mJoinFilterExpr == NULL )
    {
        sJoinFilterPred  = NULL;
    }
    else
    {
        /**
         * Join Filter Predicate 생성
         */

        /**
         * Join Expression Code Stack 공간 할당
         */

        STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                    STL_SIZEOF( knlExprStack ),
                                    (void **) & sJoinFilterStack,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );


        /**
         * Join Expression Stack 생성
         */

        sCodeExpr = sOptHashJoin->mJoinFilterExpr;

        STL_TRY( knlExprInit( sJoinFilterStack,
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
                                            sJoinFilterStack,
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

        STL_TRY( knlMakeFilter( sJoinFilterStack,
                                & sJoinFilterPred,
                                QLL_CODE_PLAN( aDBCStmt ),
                                KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
    }


    /****************************************************************
     * Where Filter 처리
     ****************************************************************/
    
    if( sOptHashJoin->mWhereFilterExpr == NULL )
    {
        sLogicalFilterPred  = NULL;
    }
    else
    {
        if( sOptHashJoin->mWhereFilterExpr == NULL )
        {
            /* Do Nothing */
        }
        else
        {
            /**
             * Logical Filter Predicate 생성
             */

            /**
             * Logical Expression Code Stack 공간 할당
             */

            STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                        STL_SIZEOF( knlExprStack ),
                                        (void **) & sLogicalFilterStack,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );


            /**
             * Logical Expression Stack 생성
             */

            sCodeExpr = sOptHashJoin->mWhereFilterExpr;

            STL_TRY( knlExprInit( sLogicalFilterStack,
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
                                                sLogicalFilterStack,
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

            STL_TRY( knlMakeFilter( sLogicalFilterStack,
                                    & sLogicalFilterPred,
                                    QLL_CODE_PLAN( aDBCStmt ),
                                    KNL_ENV( aEnv ) )
                     == STL_SUCCESS );
        }
    }
        
    
    /****************************************************************
     * Node 정보 설정
     ****************************************************************/

    /**
     * Logical Filter Predicate
     */
    
    sOptHashJoin->mWhereFilterPred = sLogicalFilterPred;


    /**
     * Join Filter Predicate
     */

    sOptHashJoin->mJoinFilterPred = sJoinFilterPred;


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
 * @brief HASH JOIN의 All Expr 리스트 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoAddExprHashJoin( qllDBCStmt            * aDBCStmt,
                               qllStatement          * aSQLStmt,
                               qllOptNodeList        * aOptNodeList,
                               qllOptimizationNode   * aOptNode,
                               qloInitExprList       * aQueryExprList,
                               qllEnv                * aEnv )
{
    qlnoHashJoin          * sOptHashJoin        = NULL;
    qlvRefExprItem        * sRefColumnItem      = NULL;
    

    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_HASH_JOIN_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );    


    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/
    
    sOptHashJoin = (qlnoHashJoin *) aOptNode->mOptNode;


    /****************************************************************
     * Child Node의 Add Expression 수행
     ****************************************************************/

    QLNO_ADD_EXPR( aDBCStmt,
                   aSQLStmt,
                   aOptNodeList,
                   sOptHashJoin->mLeftChildNode,
                   aQueryExprList,
                   aEnv );

    QLNO_ADD_EXPR( aDBCStmt,
                   aSQLStmt,
                   aOptNodeList,
                   sOptHashJoin->mRightChildNode,
                   aQueryExprList,
                   aEnv );


    /**
     * Add Expr Join Read Column
     */

    sRefColumnItem = sOptHashJoin->mJoinColList->mHead;
    while( sRefColumnItem != NULL )
    {
        STL_DASSERT( sRefColumnItem->mExprPtr->mType == QLV_EXPR_TYPE_INNER_COLUMN );

        STL_TRY( qloAddExpr( sRefColumnItem->mExprPtr->mExpr.mInnerColumn->mOrgExpr,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN(aDBCStmt),
                             aEnv )
                 == STL_SUCCESS );
        
        STL_TRY( qloAddExpr( sRefColumnItem->mExprPtr,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN(aDBCStmt),
                             aEnv )
                 == STL_SUCCESS );

        sRefColumnItem = sRefColumnItem->mNext;
    }


    /**
     * Add Expr Join Condition Expression
     */

    if( sOptHashJoin->mJoinFilterExpr != NULL )
    {
        STL_TRY( qloAddExpr( sOptHashJoin->mJoinFilterExpr,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN(aDBCStmt),
                             aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Add Expr Where Clause Expression
     */

    if( sOptHashJoin->mWhereFilterExpr != NULL )
    {
        STL_TRY( qloAddExpr( sOptHashJoin->mWhereFilterExpr,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN(aDBCStmt),
                             aEnv )
                 == STL_SUCCESS );
    }


    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlno */

