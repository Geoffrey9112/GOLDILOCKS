/*******************************************************************************
 * qlnoGroup.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlnoGroup.c 13496 2014-08-29 05:38:43Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlnoGroup.c
 * @brief SQL Processor Layer Code Optimization Node - GROUP
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlno
 * @{
 */


/**
 * @brief GROUP에 대한 Code Optimize를 수행한다.
 * @param[in]   aCodeOptParamInfo   Code Optimize Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlnoCodeOptimizeGroup( qloCodeOptParamInfo  * aCodeOptParamInfo,
                                 qllEnv               * aEnv )
{
    qlncPlanGroup           * sPlanGroup               = NULL;
    qllOptimizationNode     * sOptNode                 = NULL;
    qlnoGroup               * sOptGroup                = NULL;
    qllOptimizationNode     * sOptChildNode            = NULL;
    
    qllEstimateCost         * sOptCost                 = NULL;
    qllOptNodeList          * sSrcRelationList         = NULL;

    stlInt32                  sAggrFuncCnt             = 0;
    stlInt32                  sAggrDistFuncCnt         = 0;
    qlvRefExprList          * sTotalAggrFuncList       = NULL;
    qlvRefExprList          * sAggrFuncList            = NULL;
    qlvRefExprList          * sAggrDistFuncList        = NULL;
    qlvRefExprItem          * sRefExprItem             = NULL;

    qllOptimizationNode     * sOptAggrNode             = NULL;
    qlvAggregation          * sIniAggrNode             = NULL;


    /*
     * Paramter Validation
     */

    QLO_CODE_OPT_DEFAULT_PARAM_VALIDATE( aCodeOptParamInfo, aEnv );
    STL_PARAM_VALIDATE( aCodeOptParamInfo->mCandPlan->mPlanType == QLNC_PLAN_TYPE_GROUP,
                        QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Group Plan 정보 획득
     ****************************************************************/

    sPlanGroup = (qlncPlanGroup*)(aCodeOptParamInfo->mCandPlan);


    /****************************************************************
     * Optimization Node 생성
     ****************************************************************/

    /**
     * GROUP Optimization Node 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                STL_SIZEOF( qlnoGroup ),
                                (void **) & sOptGroup,
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
                                QLL_PLAN_NODE_GROUP_TYPE,
                                sOptGroup,
                                aCodeOptParamInfo->mOptQueryNode,  /* Query Node */
                                sOptCost,
                                NULL,
                                & sOptNode,
                                QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                aEnv )
             == STL_SUCCESS );


    /****************************************************************
     * Child Node 구성
     ****************************************************************/

    aCodeOptParamInfo->mCandPlan = sPlanGroup->mChildPlanNode;
    QLNO_CODE_OPTIMIZE( aCodeOptParamInfo, aEnv );
    sOptChildNode = aCodeOptParamInfo->mOptNode;


    /****************************************************************
     * 연관된 Base Table 정보 구축 (QUERY SPEC의 FROM 절 분석)
     ****************************************************************/

    /**
     * Optimization Node List 생성
     */

    STL_TRY( qlnoCreateOptNodeList( & sSrcRelationList,
                                    QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                    aEnv )
             == STL_SUCCESS );


    /**
     * 연관된 Base Table List 구축
     */

    STL_TRY( qlnoGetRefBaseTableNodeList( aCodeOptParamInfo->mOptNodeList,
                                          sOptChildNode,
                                          sSrcRelationList,
                                          QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                          aEnv )
             == STL_SUCCESS );


    /****************************************************************
     * Read Column List 구성 : PUSH PROJECTION에 의해 변경 가능
     ****************************************************************/

    /**
     * Statement Expression List에 Group Key Column 추가
     */

    STL_TRY( qlvAddExprListToRefExprList(
                 aCodeOptParamInfo->mQueryExprList->mStmtExprList->mColumnExprList,
                 sPlanGroup->mKeyColList,
                 STL_FALSE,
                 QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                 aEnv )
             == STL_SUCCESS );


    /**
     * Statement Expression List에 Record Column 추가
     */

    STL_TRY( qlvAddExprListToRefExprList(
                 aCodeOptParamInfo->mQueryExprList->mStmtExprList->mColumnExprList,
                 sPlanGroup->mRecColList,
                 STL_FALSE,
                 QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                 aEnv )
             == STL_SUCCESS );


    /**
     * Statement Expression List에 Read Column 추가
     * @remark Read Column은 Group Key Column과 Record Column의 부분 집합이기 때문에
     *         Column Expression List에 추가할 필요 없다.
     */


    /****************************************************************
     * Validate Hash Instant 정보 설정
     ****************************************************************/

    /* Key Column List */
    STL_DASSERT( sPlanGroup->mKeyColList != NULL );
    STL_DASSERT( sPlanGroup->mKeyColList->mCount > 0 );
    ((qlvInitInstantNode*)(sPlanGroup->mCommon.mInitNode))->mKeyColList =
        sPlanGroup->mKeyColList;
    ((qlvInitInstantNode*)(sPlanGroup->mCommon.mInitNode))->mKeyColCnt =
        sPlanGroup->mKeyColList->mCount;

    /* Record Column List */
    STL_DASSERT( sPlanGroup->mRecColList != NULL );
    ((qlvInitInstantNode*)(sPlanGroup->mCommon.mInitNode))->mRecColList =
        sPlanGroup->mRecColList;
    ((qlvInitInstantNode*)(sPlanGroup->mCommon.mInitNode))->mRecColCnt =
        sPlanGroup->mRecColList->mCount;

    /* Read Column List */
    STL_DASSERT( sPlanGroup->mReadColList != NULL );
    ((qlvInitInstantNode*)(sPlanGroup->mCommon.mInitNode))->mReadColList =
        sPlanGroup->mReadColList;
    ((qlvInitInstantNode*)(sPlanGroup->mCommon.mInitNode))->mReadColCnt =
        sPlanGroup->mReadColList->mCount;


    /****************************************************************
     * Distinct 조건을 포함 여부로 Aggregation 구분
     ****************************************************************/

    if( sPlanGroup->mRecColList->mHead != NULL )
    {
        /* total aggregation list 생성 */
        STL_TRY( qlvCreateRefExprList( & sTotalAggrFuncList,
                                       QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                       aEnv )
                 == STL_SUCCESS );

        sRefExprItem = sPlanGroup->mRecColList->mHead;
        while( sRefExprItem != NULL )
        {
            STL_DASSERT( sRefExprItem->mExprPtr->mType == QLV_EXPR_TYPE_INNER_COLUMN );
            STL_DASSERT( sRefExprItem->mExprPtr->mExpr.mInnerColumn->mOrgExpr->mType ==
                         QLV_EXPR_TYPE_AGGREGATION );

            STL_TRY( qlvAddExprToRefExprList( sTotalAggrFuncList,
                                              sRefExprItem->mExprPtr->mExpr.mInnerColumn->mOrgExpr,
                                              STL_FALSE,
                                              QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                              aEnv )
                     == STL_SUCCESS );

            sRefExprItem = sRefExprItem->mNext;
        }

        STL_DASSERT( sTotalAggrFuncList->mCount > 0 );

        /* aggregation list 구분 */
        STL_TRY( qlnfClassifyAggrFunc( sTotalAggrFuncList,
                                       & sAggrFuncList,
                                       & sAggrDistFuncList,
                                       QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                       aEnv )
                 == STL_SUCCESS );

        STL_DASSERT( sAggrFuncList != NULL );
        STL_DASSERT( sAggrDistFuncList != NULL );
        
        sAggrFuncCnt = sAggrFuncList->mCount;
        sAggrDistFuncCnt = sAggrDistFuncList->mCount;
    }
    else
    {
        sTotalAggrFuncList = NULL;
        sAggrFuncList      = NULL;
        sAggrDistFuncList  = NULL;

        sAggrFuncCnt       = 0;
        sAggrDistFuncCnt   = 0;
    }


    /****************************************************************
     * Child Node 구성
     ****************************************************************/

    if( sPlanGroup->mCommon.mNestedAggrExprList.mCount > 0 )
    {
        /**
         * Aggregation Node Optimization
         */

        STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                    STL_SIZEOF( qlvAggregation ),
                                    (void **) & sIniAggrNode,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY( qlvCreateRefExprList( &(sIniAggrNode->mAggrExprList),
                                       QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                       aEnv )
                 == STL_SUCCESS );

        sIniAggrNode->mAggrExprList->mCount = sPlanGroup->mCommon.mNestedAggrExprList.mCount;
        sIniAggrNode->mAggrExprList->mHead = sPlanGroup->mCommon.mNestedAggrExprList.mHead;
        sIniAggrNode->mAggrExprList->mTail = sPlanGroup->mCommon.mNestedAggrExprList.mTail;

        STL_TRY( qloCodeOptimizeHashAggregation( aCodeOptParamInfo->mDBCStmt,
                                                 aCodeOptParamInfo->mSQLStmt,
                                                 aCodeOptParamInfo->mOptQueryNode,
                                                 sOptNode,
                                                 sIniAggrNode,
                                                 aCodeOptParamInfo->mQueryExprList,
                                                 aCodeOptParamInfo->mOptNodeList,
                                                 & sOptAggrNode,
                                                 sOptCost,
                                                 aEnv )
                 == STL_SUCCESS );

        sOptNode = sOptAggrNode;
    }


    /****************************************************************
     * GROUP Optimization Node 정보 설정
     ****************************************************************/

    /**
     * Row Block이 필요한지 여부
     */

    sOptGroup->mNeedRowBlock = sPlanGroup->mCommon.mNeedRowBlock;


    /**
     * Child Node
     */
    
    sOptGroup->mChildNode = sOptChildNode;
    

    /**
     * Instant Table Node
     */
    
    sOptGroup->mInstantNode = (qlvInitInstantNode*)(sPlanGroup->mCommon.mInitNode);


    /**
     * Base Relation Node List
     */

    sOptGroup->mSrcRelationList = sSrcRelationList;


    /**
     * Outer Column 관련
     */

    sOptGroup->mOuterColumnList = sPlanGroup->mOuterColumnList;


    /**
     * Total Filter 관련
     */

    STL_DASSERT( sPlanGroup->mGroupKeyFilterExpr == NULL );
    sOptGroup->mTotalFilterExpr = sPlanGroup->mNonGroupKeyFilterExpr;

    
    /**
     * Aggregation 관련
     */

    sOptGroup->mAggrFuncCnt         = sAggrFuncCnt;
    sOptGroup->mAggrFuncList        = sAggrFuncList;

    sOptGroup->mTotalAggrFuncList   = sTotalAggrFuncList;
    sOptGroup->mAggrDistFuncCnt     = sAggrDistFuncCnt;
    sOptGroup->mAggrDistFuncList    = sAggrDistFuncList;


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
 * @brief GROUP의 Optimization Node 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoCompleteGroup( qllDBCStmt            * aDBCStmt,
                             qllStatement          * aSQLStmt,
                             qllOptNodeList        * aOptNodeList,
                             qllOptimizationNode   * aOptNode,
                             qloInitExprList       * aQueryExprList,
                             qllEnv                * aEnv )
{
    qlnoGroup          * sOptGroup              = NULL;

    qlvRefExprItem     * sRefColumnItem         = NULL;
    
    qlvInitExpression  * sLogicalFilterExpr     = NULL;
    knlExprStack       * sLogicalFilterStack    = NULL;
    knlPredicateList   * sLogicalFilterPred     = NULL;

    qlvInitExpression  * sCodeExpr              = NULL;
    knlExprEntry       * sExprEntry             = NULL;

    knlExprStack       * sTempConstExprStack    = NULL;
    
    knlExprStack      ** sColExprStackList      = NULL;
    knlExprStack       * sColExprStack          = NULL;
    knlExprStack         sTempColExprStack;

    stlInt32             sLoop                  = 0;
    stlInt32             sIdx                   = 0;
    stlInt32             sTotalColCnt           = 0;


    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_GROUP_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );    


    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/

    sOptGroup = (qlnoGroup *) aOptNode->mOptNode;

    /* 임시 Column Expression Stack 초기화 */
    STL_TRY( knlExprInit( & sTempColExprStack,
                          1,
                          & QLL_CANDIDATE_MEM( aEnv ),
                          KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    
    /****************************************************************
     * 하위 node에 대한 Complete Node 수행
     ****************************************************************/

    QLNO_COMPLETE( aDBCStmt,
                   aSQLStmt,
                   aOptNodeList,
                   sOptGroup->mChildNode,
                   aQueryExprList,
                   aEnv );

    
    /****************************************************************
     * Column Expression Stack 설정
     ****************************************************************/

    /**
     * Column Expression Code Stack 공간 할당
     */

    sTotalColCnt = ( sOptGroup->mInstantNode->mKeyColCnt +
                     sOptGroup->mInstantNode->mRecColCnt );
    
    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( knlExprStack * ) * sTotalColCnt,
                                (void **) & sColExprStackList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( knlExprStack ) * sTotalColCnt,
                                (void **) & sColExprStack,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Group Key Column Expression Stack 설정 (for Insert Columns)
     */
    
    sIdx = 0;

    sOptGroup->mNeedEvalKeyCol = STL_FALSE;
    sRefColumnItem = sOptGroup->mInstantNode->mKeyColList->mHead;    
    for( sLoop = 0 ; sLoop < sOptGroup->mInstantNode->mKeyColCnt ; sLoop++, sIdx++ )
    {
        /**
         * Logical Expression Stack 생성
         */

        sCodeExpr = sRefColumnItem->mExprPtr->mExpr.mInnerColumn->mOrgExpr;

        STL_TRY( knlExprInit( & sColExprStack[ sIdx ],
                              sCodeExpr->mIncludeExprCnt,
                              QLL_CODE_PLAN( aDBCStmt ),
                              KNL_ENV( aEnv ) )
                 == STL_SUCCESS );

        
        /**
         * Column Expression Stack 구성
         */

        STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                            sCodeExpr,
                                            aSQLStmt->mBindContext,
                                            & sColExprStack[ sIdx ],
                                            aQueryExprList->mStmtExprList->mConstExprStack,
                                            aQueryExprList->mStmtExprList->mTotalExprInfo,
                                            & sExprEntry,
                                            QLL_CODE_PLAN( aDBCStmt ),
                                            aEnv )
                 == STL_SUCCESS );

        if( ( sColExprStack[ sIdx ].mEntryCount == 1 ) &&
            ( sColExprStack[ sIdx ].mEntries[ 1 ].mExprType == KNL_EXPR_TYPE_VALUE ) )
        {
            sColExprStackList[ sIdx ] = NULL;
        }
        else
        {
            sColExprStackList[ sIdx ] = & sColExprStack[ sIdx ];
            sOptGroup->mNeedEvalKeyCol = STL_TRUE;
        }

        
        /**
         * Inner Column Expression Stack 구성
         */

        sCodeExpr = sRefColumnItem->mExprPtr;
        sTempColExprStack.mEntryCount = 0;

        STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                            sCodeExpr,
                                            aSQLStmt->mBindContext,
                                            & sTempColExprStack,
                                            aQueryExprList->mStmtExprList->mConstExprStack,
                                            aQueryExprList->mStmtExprList->mTotalExprInfo,
                                            & sExprEntry,
                                            & QLL_CANDIDATE_MEM( aEnv ),
                                            aEnv )
                 == STL_SUCCESS );

        sRefColumnItem = sRefColumnItem->mNext;
    }


    /**
     * Record Column Expression Stack 설정 (for Insert Columns)
     */

    /* group hash instant 에서는 mNeedEvalRecCol 이 사용되지 않는다. */
    sRefColumnItem = sOptGroup->mInstantNode->mRecColList->mHead;
    for( sLoop = 0 ; sLoop < sOptGroup->mInstantNode->mRecColCnt ; sLoop++, sIdx++ )
    {
        /**
         * Logical Expression Stack 생성
         */

        sCodeExpr = sRefColumnItem->mExprPtr->mExpr.mInnerColumn->mOrgExpr;

        STL_TRY( knlExprInit( & sColExprStack[ sIdx ],
                              sCodeExpr->mIncludeExprCnt,
                              QLL_CODE_PLAN( aDBCStmt ),
                              KNL_ENV( aEnv ) )
                 == STL_SUCCESS );

        
        /**
         * Column Expression Stack 구성
         */

        STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                            sCodeExpr,
                                            aSQLStmt->mBindContext,
                                            & sColExprStack[ sIdx ],
                                            aQueryExprList->mStmtExprList->mConstExprStack,
                                            aQueryExprList->mStmtExprList->mTotalExprInfo,
                                            & sExprEntry,
                                            QLL_CODE_PLAN( aDBCStmt ),
                                            aEnv )
                 == STL_SUCCESS );

        /* Group By 구문의 Record Column 이 될 수 있는 대상은 Aggregation으로 한정된다.
         * Aggregation 내부의 expression은 Aggregation에서 처리하므로
         * Record Column 은 항상 KNL_EXPR_TYPE_VALUE 이며, internal cast 가 존재하지 않는다.  */
        
        STL_DASSERT( ( sColExprStack[ sIdx ].mEntryCount == 1 ) &&
                     ( sColExprStack[ sIdx ].mEntries[ 1 ].mExprType == KNL_EXPR_TYPE_VALUE ) );

        sColExprStackList[ sIdx ] = NULL;


        /**
         * Inner Column Expression Stack 구성
         */

        sCodeExpr = sRefColumnItem->mExprPtr;
        sTempColExprStack.mEntryCount = 0;

        STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                            sCodeExpr,
                                            aSQLStmt->mBindContext,
                                            & sTempColExprStack,
                                            aQueryExprList->mStmtExprList->mConstExprStack,
                                            aQueryExprList->mStmtExprList->mTotalExprInfo,
                                            & sExprEntry,
                                            & QLL_CANDIDATE_MEM( aEnv ),
                                            aEnv )
                 == STL_SUCCESS );

        sRefColumnItem = sRefColumnItem->mNext;
    }


    /****************************************************************
     * Aggregation 에 대한 Expression Stack 구성
     ****************************************************************/

    /**
     * Aggregation Function 정보 구축
     */

    if( sOptGroup->mAggrFuncCnt > 0 )
    {
        STL_TRY( qlnfGetAggrOptInfo( aSQLStmt,
                                     aQueryExprList,
                                     sOptGroup->mAggrFuncCnt,
                                     sOptGroup->mAggrFuncList,
                                     & sOptGroup->mAggrOptInfo,
                                     QLL_CODE_PLAN( aDBCStmt ),
                                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sOptGroup->mAggrOptInfo = NULL;
    }

    
    /****************************************************************
     * Aggregation Distinct에 대한 Expression Stack 구성
     ****************************************************************/

    /**
     * Aggregation Function 정보 구축
     */

    if( sOptGroup->mAggrDistFuncCnt > 0 )
    {
        STL_TRY( qlnfGetAggrOptInfo( aSQLStmt,
                                     aQueryExprList,
                                     sOptGroup->mAggrDistFuncCnt,
                                     sOptGroup->mAggrDistFuncList,
                                     & sOptGroup->mAggrDistOptInfo,
                                     QLL_CODE_PLAN( aDBCStmt ),
                                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sOptGroup->mAggrDistOptInfo = NULL;
    }


    /****************************************************************
     * Logical Filter 처리
     ****************************************************************/
    
    if( sOptGroup->mTotalFilterExpr == NULL )
    {
        sLogicalFilterExpr  = NULL;
        sLogicalFilterPred  = NULL;
    }
    else 
    {
        sLogicalFilterExpr = sOptGroup->mTotalFilterExpr;

        /**
         * Logical Filter를 위한 임시 Constant Expression Stack 공간 할당
         * @remark Candidate memory manager 사용
         */

        STL_TRY( knlAllocRegionMem( & QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( knlExprStack ),
                                    (void **) & sTempConstExprStack,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );


        /**
         * Logical Filter를 위한 임시 Constant Expression Stack 구성
         * @remark Candidate memory manager 사용
         */

        STL_TRY( knlExprInit( sTempConstExprStack,
                              sLogicalFilterExpr->mIncludeExprCnt,
                              & QLL_CANDIDATE_MEM( aEnv ),
                              KNL_ENV( aEnv ) )
                 == STL_SUCCESS );


        /**
         * Logical Filter Predicate 생성 : Logical Expression Code Stack 공간 할당
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
                                            sTempConstExprStack,
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


    /****************************************************************
     * Node 정보 설정
     ****************************************************************/

    /**
     * Column 관련
     */
    
    sOptGroup->mColExprStack = sColExprStackList;

    
    /**
     * Predicate 관련
     */

    sOptGroup->mPredicate = sLogicalFilterPred;

    
    /**
     * Filter 관련
     */

    sOptGroup->mLogicalStack = sLogicalFilterStack;


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
 * @brief GROUP의 All Expr 리스트 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoAddExprGroup( qllDBCStmt            * aDBCStmt,
                            qllStatement          * aSQLStmt,
                            qllOptNodeList        * aOptNodeList,
                            qllOptimizationNode   * aOptNode,
                            qloInitExprList       * aQueryExprList,
                            qllEnv                * aEnv )
{
    qlnoGroup               * sOptGroup     = NULL;
    qlvRefExprItem          * sRefExprItem  = NULL;
    qlvInitInstantNode      * sInstantNode  = NULL;


    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_GROUP_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );    


    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/

    sOptGroup = (qlnoGroup *) aOptNode->mOptNode;

    
    /****************************************************************
     * Child Node의 Add Expression 수행
     ****************************************************************/

    QLNO_ADD_EXPR( aDBCStmt,
                   aSQLStmt,
                   aOptNodeList,
                   sOptGroup->mChildNode,
                   aQueryExprList,
                   aEnv );
    

    /****************************************************************
     * Add Expr
     ****************************************************************/

    /**
     * Add Expr Group Instant Table : Group Key Column
     */

    sInstantNode = sOptGroup->mInstantNode;

    sRefExprItem = sInstantNode->mKeyColList->mHead;
    while( sRefExprItem != NULL )
    {
        STL_DASSERT( sRefExprItem->mExprPtr->mType == QLV_EXPR_TYPE_INNER_COLUMN );

        STL_TRY( qloAddExpr( sRefExprItem->mExprPtr->mExpr.mInnerColumn->mOrgExpr,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN( aDBCStmt ),
                             aEnv )
                 == STL_SUCCESS );

        STL_TRY( qloAddExpr( sRefExprItem->mExprPtr,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN( aDBCStmt ),
                             aEnv )
                 == STL_SUCCESS );

        sRefExprItem = sRefExprItem->mNext;
    }


    /**
     * Add Expr Group Instant Table : Record Column
     */

    sRefExprItem = sInstantNode->mRecColList->mHead;
    while( sRefExprItem != NULL )
    {
        STL_DASSERT( sRefExprItem->mExprPtr->mType == QLV_EXPR_TYPE_INNER_COLUMN );

        STL_TRY( qloAddExpr( sRefExprItem->mExprPtr->mExpr.mInnerColumn->mOrgExpr,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN( aDBCStmt ),
                             aEnv )
                 == STL_SUCCESS );

        STL_TRY( qloAddExpr( sRefExprItem->mExprPtr,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN( aDBCStmt ),
                             aEnv )
                 == STL_SUCCESS );

        sRefExprItem = sRefExprItem->mNext;
    }


    /**
     * Add Expr Group Instant Table Column
     * @remark Read Column은 Group Key Column과 Record Column의 부분 집합이기 때문에
     *         Read Column의 Original Expresssion을 전체 Expression List에 추가할 필요 없다.
     */

    sRefExprItem = sInstantNode->mReadColList->mHead;
    while( sRefExprItem != NULL )
    {
        STL_DASSERT( sRefExprItem->mExprPtr->mType == QLV_EXPR_TYPE_INNER_COLUMN );

        STL_TRY( qloAddExpr( sRefExprItem->mExprPtr,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN( aDBCStmt ),
                             aEnv )
                 == STL_SUCCESS );

        sRefExprItem = sRefExprItem->mNext;
    }

    
    /**
     * Add Expr HAVING Clause Expression
     */
        
    if( sOptGroup->mTotalFilterExpr != NULL )
    {
        STL_TRY( qloAddExpr( sOptGroup->mTotalFilterExpr,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN( aDBCStmt ),
                             aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Add Expr Aggregation Distinct Functions
     * @remark Instant Table의 Column으로 구성되어 있기 때문에 Add하지 않는다.
     */

    /* Do Nothing */
    

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlno */
