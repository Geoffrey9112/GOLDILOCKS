/*******************************************************************************
 * qlnoNestedLoopsJoin.c
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
 * @file qlnoNestedLoopsJoin.c
 * @brief SQL Processor Layer Code Optimization Node - NESTED LOOPS JOIN
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
 * @brief Nested Loops Join에 대한 Code Optimize를 수행한다.
 * @param[in]   aCodeOptParamInfo   Code Optimize Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlnoCodeOptimizeNestedLoops( qloCodeOptParamInfo  * aCodeOptParamInfo,
                                       qllEnv               * aEnv )
{
    qlncPlanNestedJoin      * sPlanNestedJoin       = NULL;

    qllOptimizationNode     * sJoinOptNode          = NULL;
//    qllOptimizationNode * sOptSubQueryFilter    = NULL;
    qlnoNestedLoopsJoin     * sOptNestedLoopsJoin   = NULL;
    qllOptimizationNode    ** sOptAndFilterNode     = NULL;
    qllEstimateCost         * sOptCost              = NULL;

    stlInt32                  sLoop;
    stlInt32                  sIdx;
    qlnoSubQueryList        * sOptSubQueryList      = NULL;
//    qlvInitExpression   * sSubQueryFilterExpr   = NULL;

    qlvRefExprList          * sRefColumnList        = NULL;
//    qlvRefExprList      * sSubQueryExprList     = NULL;


    /*
     * Paramter Validation
     */

    QLO_CODE_OPT_DEFAULT_PARAM_VALIDATE( aCodeOptParamInfo, aEnv );


    /****************************************************************
     * Nested Join Plan 정보 획득
     ****************************************************************/

    sPlanNestedJoin = (qlncPlanNestedJoin*)(aCodeOptParamInfo->mCandPlan);


    /****************************************************************
     * Optimization Node 생성
     ****************************************************************/

    /**
     * NESTED LOOPS JOIN Optimization Node 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                STL_SIZEOF( qlnoNestedLoopsJoin ),
                                (void **) &sOptNestedLoopsJoin,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


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
                                QLL_PLAN_NODE_NESTED_LOOPS_TYPE,
                                sOptNestedLoopsJoin,
                                aCodeOptParamInfo->mOptQueryNode,  /* Query Node */
                                sOptCost,
                                NULL,
                                &sJoinOptNode,
                                QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                aEnv )
             == STL_SUCCESS );


    /****************************************************************
     * Optimization Node 생성 : SCAN method 결정된 이후에 설정
     ****************************************************************/

   
    /**
     * Left Table Node에 대한 Optimization Node 생성
     */

    aCodeOptParamInfo->mCandPlan = sPlanNestedJoin->mLeftChildPlanNode;
    QLNO_CODE_OPTIMIZE( aCodeOptParamInfo, aEnv );
    sOptNestedLoopsJoin->mLeftChildNode = aCodeOptParamInfo->mOptNode;


    /**
     * Right Table Node에 대한 Optimization Node 생성
     */

    aCodeOptParamInfo->mCandPlan = sPlanNestedJoin->mRightChildPlanNode;
    QLNO_CODE_OPTIMIZE( aCodeOptParamInfo, aEnv );
    sOptNestedLoopsJoin->mRightChildNode = aCodeOptParamInfo->mOptNode;


    /****************************************************************
     * Read Column List 구성
     ****************************************************************/

    /**
     * Read Column List 구성
     */

    sRefColumnList = sPlanNestedJoin->mReadColList;


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
     * Statement 레벨에서 처리되는 SubQuery Node 설정
     ****************************************************************/

    if( sPlanNestedJoin->mStmtSubQueryPlanNode != NULL )
    {
        aCodeOptParamInfo->mCandPlan = sPlanNestedJoin->mStmtSubQueryPlanNode;
        QLNO_CODE_OPTIMIZE( aCodeOptParamInfo, aEnv );
        sOptNestedLoopsJoin->mStmtSubQueryNode = aCodeOptParamInfo->mOptNode;
    }
    else
    {
        sOptNestedLoopsJoin->mStmtSubQueryNode = NULL;
    }


    /****************************************************************
     * SubQuery Filter 관련 설정
     ****************************************************************/

    if( sPlanNestedJoin->mSubQueryAndFilter != NULL )
    {
        /**
         * SubQuery And Filter Node 공간 할당
         */

        STL_TRY( knlAllocRegionMem(
                     QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                     STL_SIZEOF( qllOptimizationNode* ) * sPlanNestedJoin->mSubQueryAndFilter->mAndFilterCnt,
                     (void **) & sOptAndFilterNode,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemset( sOptAndFilterNode,
                   0x00,
                   STL_SIZEOF( qllOptimizationNode* ) * sPlanNestedJoin->mSubQueryAndFilter->mAndFilterCnt );

        /**
         * And Filter Node Optimization
         */

        for( sLoop = 0 ; sLoop < sPlanNestedJoin->mSubQueryAndFilter->mAndFilterCnt ; sLoop++ )
        {
            aCodeOptParamInfo->mCandPlan = sPlanNestedJoin->mSubQueryPlanNodes[ sLoop ];
            QLNO_CODE_OPTIMIZE( aCodeOptParamInfo, aEnv );
            sOptAndFilterNode[ sLoop ] = aCodeOptParamInfo->mOptNode;

            STL_DASSERT( sOptAndFilterNode[ sLoop ]->mType == QLL_PLAN_NODE_SUB_QUERY_LIST_TYPE );

            sOptSubQueryList = (qlnoSubQueryList *) sOptAndFilterNode[ sLoop ]->mOptNode;

            for( sIdx = 0 ; sIdx < sOptSubQueryList->mSubQueryNodeCnt ; sIdx++ )
            {
                if( sOptSubQueryList->mSubQueryNodes[ sIdx ]->mType == QLL_PLAN_NODE_SUB_QUERY_FUNCTION_TYPE )
                {
                    ((qlnoSubQueryFunc*)(sOptSubQueryList->mSubQueryNodes[ sIdx ]->mOptNode))->mChildNode = NULL;
                }
            }
        }

        sOptNestedLoopsJoin->mSubQueryAndFilterNodeArr = sOptAndFilterNode;
        sOptNestedLoopsJoin->mSubQueryFilterExprArr =
            sPlanNestedJoin->mSubQueryAndFilter->mInitSubQueryFilter;
        sOptNestedLoopsJoin->mSubQueryAndFilterCnt =
            sPlanNestedJoin->mSubQueryAndFilter->mAndFilterCnt;
    }
    else
    {
        sOptNestedLoopsJoin->mSubQueryAndFilterNodeArr = NULL;
        sOptNestedLoopsJoin->mSubQueryFilterExprArr = NULL;
        sOptNestedLoopsJoin->mSubQueryAndFilterCnt = 0;
    }

    /****************************************************************
     * Optimize 정보 연결
     ****************************************************************/


    /**
     * Row Block이 필요한지 여부
     */

    sOptNestedLoopsJoin->mNeedRowBlock = sPlanNestedJoin->mCommon.mNeedRowBlock;


    /**
     * Join Type
     */

    sOptNestedLoopsJoin->mJoinType = sPlanNestedJoin->mJoinType;


    /**
     * Joined Column List
     */

    sOptNestedLoopsJoin->mJoinColList = sRefColumnList;


    /**
     * Join Table Node
     */

    sOptNestedLoopsJoin->mJoinTableNode = (qlvInitJoinTableNode*)(sPlanNestedJoin->mCommon.mInitNode);


    /**
     * Join Left Outer Column List
     */

    sOptNestedLoopsJoin->mLeftOuterColumnList = sPlanNestedJoin->mJoinLeftOuterColumnList;


    /**
     * Join Right Outer Column List
     */

    sOptNestedLoopsJoin->mRightOuterColumnList = sPlanNestedJoin->mJoinRightOuterColumnList;


    /**
     * Where Filter Outer Column List
     */

    sOptNestedLoopsJoin->mWhereOuterColumnList = sPlanNestedJoin->mWhereFilterOuterColumnList;


    /**
     * Outer Column 관련
     */

    sOptNestedLoopsJoin->mOuterColumnList = sPlanNestedJoin->mOuterColumnList;


    /**
     * Where Clause Total Filter 설정
     */

    sOptNestedLoopsJoin->mTotalFilterExpr = sPlanNestedJoin->mWhereFilterExpr;


    /**
     * Where Clause Physical Filter 설정
     */

    sOptNestedLoopsJoin->mPhysicalFilterExpr = NULL;


    /**
     * Where Clause Logical Filter 설정
     */

    sOptNestedLoopsJoin->mLogicalFilterExpr = NULL;


    /**
     * Where Clause Logical Filter Predicate 설정
     */

    sOptNestedLoopsJoin->mLogicalFilterPred = NULL;


    /**
     * Join Filiter Expression 설정
     */

    sOptNestedLoopsJoin->mJoinFilterExpr = sPlanNestedJoin->mJoinFilterExpr;


    /**
     * Join Filter Predicate 설정
     */

    sOptNestedLoopsJoin->mJoinFilterPred = NULL;


    /**
     * SubQuery Filter Predicate 설정
     */

    sOptNestedLoopsJoin->mSubQueryFilterPredArr = NULL;


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
 * @brief NESTED LOOPS JOIN의 Optimization Node 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoCompleteNestedLoopsJoin( qllDBCStmt            * aDBCStmt,
                                       qllStatement          * aSQLStmt,
                                       qllOptNodeList        * aOptNodeList,
                                       qllOptimizationNode   * aOptNode,
                                       qloInitExprList       * aQueryExprList,
                                       qllEnv                * aEnv )
{
    qlnoNestedLoopsJoin * sOptNestedLoopsJoin   = NULL;

    qlvInitExpression   * sPhysicalFilterExpr   = NULL;
    qlvInitExpression   * sLogicalFilterExpr    = NULL;
    
    knlExprStack        * sLogicalFilterStack   = NULL;
    knlPredicateList    * sLogicalFilterPred    = NULL;

    knlExprStack        * sJoinFilterStack      = NULL;
    knlPredicateList    * sJoinFilterPred       = NULL;

    knlExprStack        * sSubQueryFilterStack      = NULL;
    knlPredicateList   ** sSubQueryFilterPredArr    = NULL;

    qlvInitExpression   * sCodeExpr             = NULL;
    knlExprEntry        * sExprEntry            = NULL;

    /* knlExprStack        * sTempConstExprStack   = NULL; */
    
    qlvInitDataTypeInfo   sDataTypeInfo;
    qlvRefExprItem      * sExprItem             = NULL;
    qlvInitExpression   * sOrgExpr              = NULL;
    qloDBType           * sDBType               = NULL;
    ellDictHandle       * sColumnHandle         = NULL;

    stlInt32              sLoop;
    
    
    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_NESTED_LOOPS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );    


    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/

    sOptNestedLoopsJoin = (qlnoNestedLoopsJoin *) aOptNode->mOptNode;


    /****************************************************************
     * 하위 node에 대한 Complete Node 수행
     ****************************************************************/

    QLNO_COMPLETE( aDBCStmt,
                   aSQLStmt,
                   aOptNodeList,
                   sOptNestedLoopsJoin->mLeftChildNode,
                   aQueryExprList,
                   aEnv );

    QLNO_COMPLETE( aDBCStmt,
                   aSQLStmt,
                   aOptNodeList,
                   sOptNestedLoopsJoin->mRightChildNode,
                   aQueryExprList,
                   aEnv );

    
    /****************************************************************
     * Join Column에 대한 DB Type 설정
     ****************************************************************/

    sExprItem = sOptNestedLoopsJoin->mJoinColList->mHead;
    
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

    if( sOptNestedLoopsJoin->mJoinFilterExpr == NULL )
    {
        sJoinFilterPred  = NULL;
    }
    else
    {
        /**
         * Join Filter를 위한 임시 Constant Expression Stack 공간 할당
         * @remark Candidate memory manager 사용
         */

        /* STL_TRY( knlAllocRegionMem( & QLL_CANDIDATE_MEM( aDBCStmt ), */
        /*                             STL_SIZEOF( knlExprStack ), */
        /*                             (void **) & sTempConstExprStack, */
        /*                             KNL_ENV(aEnv) ) */
        /*          == STL_SUCCESS ); */

        /* stlMemset( sTempConstExprStack, */
        /*            0x00, */
        /*            STL_SIZEOF( knlExprStack ) ); */


        /**
         * Join Filter를 위한 임시 Constant Expression Stack 구성
         * @remark Candidate memory manager 사용
         */

        /* STL_TRY( knlExprInit( sTempConstExprStack, */
        /*                       sOptNestedLoopsJoin->mJoinFilterExpr->mIncludeExprCnt, */
        /*                       & QLL_CANDIDATE_MEM( aDBCStmt ), */
        /*                       KNL_ENV( aEnv ) ) */
        /*          == STL_SUCCESS ); */


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

        sCodeExpr = sOptNestedLoopsJoin->mJoinFilterExpr;

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
     * SubQuery관련 하위 node에 대한 Complete Node 수행
     ****************************************************************/

    /* Statement단위 처리의 SubQuery Node */
    if( sOptNestedLoopsJoin->mStmtSubQueryNode != NULL )
    {
        QLNO_COMPLETE( aDBCStmt,
                       aSQLStmt,
                       aOptNodeList,
                       sOptNestedLoopsJoin->mStmtSubQueryNode,
                       aQueryExprList,
                       aEnv );
    }

    if( sOptNestedLoopsJoin->mSubQueryAndFilterCnt > 0 )
    {
        for( sLoop = 0 ; sLoop < sOptNestedLoopsJoin->mSubQueryAndFilterCnt ; sLoop++ )
        {
            QLNO_COMPLETE( aDBCStmt,
                           aSQLStmt,
                           aOptNodeList,
                           sOptNestedLoopsJoin->mSubQueryAndFilterNodeArr[ sLoop ],
                           aQueryExprList,
                           aEnv );
        }
    }
    

    /****************************************************************
     * SubQuery Filter Predicate 생성
     ****************************************************************/

    if( sOptNestedLoopsJoin->mSubQueryAndFilterCnt > 0 )
    {
        STL_TRY( knlAllocRegionMem(
                     QLL_CODE_PLAN( aDBCStmt ),
                     STL_SIZEOF( knlPredicateList* ) * sOptNestedLoopsJoin->mSubQueryAndFilterCnt,
                     (void **) & sSubQueryFilterPredArr,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        for( sLoop = 0 ; sLoop < sOptNestedLoopsJoin->mSubQueryAndFilterCnt ; sLoop++ )
        {
            STL_DASSERT( sOptNestedLoopsJoin->mSubQueryFilterExprArr[ sLoop ] != NULL );

            /* Filter Expression Code Stack 공간 할당 */
            STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                        STL_SIZEOF( knlExprStack ),
                                        (void **) & sSubQueryFilterStack,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            /* Filter Expression Stack 생성 */
            sCodeExpr = sOptNestedLoopsJoin->mSubQueryFilterExprArr[ sLoop ];

            STL_TRY( knlExprInit( sSubQueryFilterStack,
                                  sCodeExpr->mIncludeExprCnt,
                                  QLL_CODE_PLAN( aDBCStmt ),
                                  KNL_ENV( aEnv ) )
                     == STL_SUCCESS );

            /* Logical Expression Stack 구성 */
            STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                                sCodeExpr,
                                                aSQLStmt->mBindContext,
                                                sSubQueryFilterStack,
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


            /* Logical Expression에 대한 Filter Predicate 구성 */
            STL_TRY( knlMakeFilter( sSubQueryFilterStack,
                                    & sSubQueryFilterPredArr[ sLoop ],
                                    QLL_CODE_PLAN( aDBCStmt ),
                                    KNL_ENV( aEnv ) )
                     == STL_SUCCESS );
        }
    }
    else
    {
        sSubQueryFilterPredArr = NULL;
    }


    /****************************************************************
     * Physical & Logical Filter 구분
     ****************************************************************/
    
    /**
     * Logical Filter 와 Physical Filter 분리
     */

    if( sOptNestedLoopsJoin->mTotalFilterExpr == NULL )
    {
        sLogicalFilterExpr  = NULL;
        sPhysicalFilterExpr = NULL;
        sLogicalFilterPred  = NULL;
    }
    else
    {
        /**
         * @todo Logical Filter 와 Physical Filter Expression 분리
         */

        /* STL_TRY( qloClassifyFilterExpr( sOptNestedLoopsJoin->mTotalFilterExpr, */
        /*                                 QLL_CODE_PLAN( aDBCStmt ), */
        /*                                 aQueryExprList->mStmtExprList->mTotalExprInfo, */
        /*                                 & sLogicalFilterExpr, */
        /*                                 & sPhysicalFilterExpr, */
        /*                                 aEnv ) */
        /*          == STL_SUCCESS ); */
        
        sLogicalFilterExpr = sOptNestedLoopsJoin->mTotalFilterExpr;
        
        if( sLogicalFilterExpr == NULL )
        {
            /* Do Nothing */
        }
        else
        {
            /**
             * Logical Filter를 위한 임시 Constant Expression Stack 공간 할당
             * @remark Candidate memory manager 사용
             */

            /* STL_TRY( knlAllocRegionMem( & QLL_CANDIDATE_MEM( aDBCStmt ), */
            /*                             STL_SIZEOF( knlExprStack ), */
            /*                             (void **) & sTempConstExprStack, */
            /*                             KNL_ENV(aEnv) ) */
            /*          == STL_SUCCESS ); */
    
            /* stlMemset( sTempConstExprStack, */
            /*            0x00, */
            /*            STL_SIZEOF( knlExprStack ) ); */

    
            /**
             * Logical Filter를 위한 임시 Constant Expression Stack 구성
             * @remark Candidate memory manager 사용
             */

            /* STL_TRY( knlExprInit( sTempConstExprStack, */
            /*                       sLogicalFilterExpr->mIncludeExprCnt, */
            /*                       & QLL_CANDIDATE_MEM( aDBCStmt ), */
            /*                       KNL_ENV( aEnv ) ) */
            /*          == STL_SUCCESS ); */


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

            sCodeExpr = sLogicalFilterExpr;

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
     * Physical Filter Expression
     */
    
    sOptNestedLoopsJoin->mPhysicalFilterExpr = sPhysicalFilterExpr;

    
    /**
     * Logical Filter Expression
     */
    
    sOptNestedLoopsJoin->mLogicalFilterExpr = sLogicalFilterExpr;


    /**
     * Logical Filter Predicate
     */
    
    sOptNestedLoopsJoin->mLogicalFilterPred = sLogicalFilterPred;


    /**
     * Join Filter Predicate
     */

    sOptNestedLoopsJoin->mJoinFilterPred = sJoinFilterPred;


    /**
     * SubQuery Filter Predicate
     */

    sOptNestedLoopsJoin->mSubQueryFilterPredArr = sSubQueryFilterPredArr;


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
 * @brief NESTED LOOPS JOIN의 All Expr 리스트 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoAddExprNestedLoopsJoin( qllDBCStmt            * aDBCStmt,
                                      qllStatement          * aSQLStmt,
                                      qllOptNodeList        * aOptNodeList,
                                      qllOptimizationNode   * aOptNode,
                                      qloInitExprList       * aQueryExprList,
                                      qllEnv                * aEnv )
{
    stlInt32                i;
    qlnoNestedLoopsJoin   * sOptNestedLoopsJoin = NULL;
    qlvRefExprItem        * sRefColumnItem      = NULL;
    

    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_NESTED_LOOPS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );    


    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/
    
    sOptNestedLoopsJoin = (qlnoNestedLoopsJoin *) aOptNode->mOptNode;


    /****************************************************************
     * Child Node의 Add Expression 수행
     ****************************************************************/

    QLNO_ADD_EXPR( aDBCStmt,
                   aSQLStmt,
                   aOptNodeList,
                   sOptNestedLoopsJoin->mLeftChildNode,
                   aQueryExprList,
                   aEnv );

    QLNO_ADD_EXPR( aDBCStmt,
                   aSQLStmt,
                   aOptNodeList,
                   sOptNestedLoopsJoin->mRightChildNode,
                   aQueryExprList,
                   aEnv );


    /****************************************************************
     * Add Expr
     ****************************************************************/

    /**
     * Add Expr Join Read Column
     */

    sRefColumnItem = sOptNestedLoopsJoin->mJoinColList->mHead;
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

    if( sOptNestedLoopsJoin->mJoinFilterExpr != NULL )
    {
        STL_TRY( qloAddExpr( sOptNestedLoopsJoin->mJoinFilterExpr,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN(aDBCStmt),
                             aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Statement단위 처리의 SubQuery Node
     */

    if( sOptNestedLoopsJoin->mStmtSubQueryNode != NULL )
    {
        QLNO_ADD_EXPR( aDBCStmt,
                       aSQLStmt,
                       aOptNodeList,
                       sOptNestedLoopsJoin->mStmtSubQueryNode,
                       aQueryExprList,
                       aEnv );
    }


    /**
     * SubQuery Filter 관련 Add Expr
     */

    if( sOptNestedLoopsJoin->mSubQueryAndFilterCnt > 0 )
    {
        /* SubQuery Node */
        for( i = 0; i < sOptNestedLoopsJoin->mSubQueryAndFilterCnt; i++ )
        {
            QLNO_ADD_EXPR( aDBCStmt,
                           aSQLStmt,
                           aOptNodeList,
                           sOptNestedLoopsJoin->mSubQueryAndFilterNodeArr[i],
                           aQueryExprList,
                           aEnv );
        }

        /* SubQuery Filter */
        for( i = 0; i < sOptNestedLoopsJoin->mSubQueryAndFilterCnt; i++ )
        {
            if( sOptNestedLoopsJoin->mSubQueryFilterExprArr[i] != NULL )
            {
                STL_TRY( qloAddExpr( sOptNestedLoopsJoin->mSubQueryFilterExprArr[i],
                                     aQueryExprList->mStmtExprList->mAllExprList,
                                     QLL_CODE_PLAN(aDBCStmt),
                                     aEnv )
                         == STL_SUCCESS );
            }
        }
    }


    /**
     * Add Expr Where Clause Expression
     */

    if( sOptNestedLoopsJoin->mTotalFilterExpr != NULL )
    {
        STL_TRY( qloAddExpr( sOptNestedLoopsJoin->mTotalFilterExpr,
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





