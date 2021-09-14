/*******************************************************************************
 * qlnoGroupHashInstantAccess.c
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
 * @file qlnoGroupHashInstantAccess.c
 * @brief SQL Processor Layer Code Optimization Node - GROUP INSTANT ACCESS
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlno
 * @{
 */


/**
 * @brief Group Hash Instant에 대한 Code Optimize를 수행한다.
 * @param[in]   aCodeOptParamInfo   Code Optimize Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlnoCodeOptimizeGroupHashInstant( qloCodeOptParamInfo * aCodeOptParamInfo,
                                            qllEnv              * aEnv )
{
    qlncPlanHashInstant     * sPlanHashGroupInstant = NULL;
    qllOptimizationNode     * sOptNode              = NULL;
    qlnoInstant             * sOptHashGroupInstant  = NULL;
    qllOptimizationNode     * sOptChildNode         = NULL;
    
    qllEstimateCost         * sOptCost              = NULL;

    qllOptNodeList          * sSrcRelationList      = NULL;

    stlInt32                  sAggrFuncCnt       = 0;
    stlInt32                  sAggrDistFuncCnt   = 0;
    qlvRefExprList          * sTotalAggrFuncList = NULL;
    qlvRefExprList          * sAggrFuncList      = NULL;
    qlvRefExprList          * sAggrDistFuncList  = NULL;

    stlInt32                  sNestedAggrFuncCnt       = 0;
    stlInt32                  sNestedAggrDistFuncCnt   = 0;
    qlvRefExprList          * sNestedTotalAggrFuncList = NULL;
    qlvRefExprList          * sNestedAggrFuncList      = NULL;
    qlvRefExprList          * sNestedAggrDistFuncList  = NULL;

    qlvRefExprItem          * sRefExprItem             = NULL;


    /*
     * Paramter Validation
     */

    QLO_CODE_OPT_DEFAULT_PARAM_VALIDATE( aCodeOptParamInfo, aEnv );
    STL_PARAM_VALIDATE( aCodeOptParamInfo->mCandPlan->mPlanType == QLNC_PLAN_TYPE_HASH_GROUP_INSTANT,
                        QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Hash Group Instant Plan 정보 획득
     ****************************************************************/

    sPlanHashGroupInstant = (qlncPlanHashInstant*)(aCodeOptParamInfo->mCandPlan);


    /****************************************************************
     * Optimization Node 생성
     ****************************************************************/

    /**
     * GROUP INSTANT ACCESS Optimization Node 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                STL_SIZEOF( qlnoInstant ),
                                (void **) & sOptHashGroupInstant,
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
                                QLL_PLAN_NODE_GROUP_HASH_INSTANT_ACCESS_TYPE,
                                sOptHashGroupInstant,
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

    aCodeOptParamInfo->mCandPlan = sPlanHashGroupInstant->mChildPlanNode;
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
                 sPlanHashGroupInstant->mKeyColList,
                 STL_FALSE,
                 QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                 aEnv )
             == STL_SUCCESS );


    /**
     * Statement Expression List에 Record Column 추가
     */

    STL_TRY( qlvAddExprListToRefExprList(
                 aCodeOptParamInfo->mQueryExprList->mStmtExprList->mColumnExprList,
                 sPlanHashGroupInstant->mRecColList,
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
    STL_DASSERT( sPlanHashGroupInstant->mKeyColList != NULL );
    STL_DASSERT( sPlanHashGroupInstant->mKeyColList->mCount > 0 );
    ((qlvInitInstantNode*)(sPlanHashGroupInstant->mCommon.mInitNode))->mKeyColList =
        sPlanHashGroupInstant->mKeyColList;
    ((qlvInitInstantNode*)(sPlanHashGroupInstant->mCommon.mInitNode))->mKeyColCnt =
        sPlanHashGroupInstant->mKeyColList->mCount;

    /* Record Column List */
    STL_DASSERT( sPlanHashGroupInstant->mRecColList != NULL );
    ((qlvInitInstantNode*)(sPlanHashGroupInstant->mCommon.mInitNode))->mRecColList =
        sPlanHashGroupInstant->mRecColList;
    ((qlvInitInstantNode*)(sPlanHashGroupInstant->mCommon.mInitNode))->mRecColCnt =
        sPlanHashGroupInstant->mRecColList->mCount;

    /* Read Column List */
    STL_DASSERT( sPlanHashGroupInstant->mReadColList != NULL );
    ((qlvInitInstantNode*)(sPlanHashGroupInstant->mCommon.mInitNode))->mReadColList =
        sPlanHashGroupInstant->mReadColList;
    ((qlvInitInstantNode*)(sPlanHashGroupInstant->mCommon.mInitNode))->mReadColCnt =
        sPlanHashGroupInstant->mReadColList->mCount;


    /****************************************************************
     * Distinct 조건을 포함 여부로 Aggregation 구분
     ****************************************************************/

    if( sPlanHashGroupInstant->mRecColList->mHead != NULL )
    {
        /* total aggregation list 생성 */
        STL_TRY( qlvCreateRefExprList( & sTotalAggrFuncList,
                                       QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                       aEnv )
                 == STL_SUCCESS );

        sRefExprItem = sPlanHashGroupInstant->mRecColList->mHead;
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
     * Distinct 조건을 포함 여부로 Nested Aggregation 구분
     ****************************************************************/

    STL_TRY( qlnfClassifyAggrFunc( &(sPlanHashGroupInstant->mCommon.mNestedAggrExprList),
                                   & sNestedAggrFuncList,
                                   & sNestedAggrDistFuncList,
                                   QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    if( sNestedAggrFuncList == NULL )
    {
        sNestedAggrFuncCnt = 0;
    }
    else
    {
        sNestedAggrFuncCnt = sNestedAggrFuncList->mCount;
    }

    if( sNestedAggrDistFuncList == NULL )
    {
        sNestedAggrDistFuncCnt = 0;
    }
    else
    {
        sNestedAggrDistFuncCnt = sNestedAggrDistFuncList->mCount;
    }

    if( sPlanHashGroupInstant->mCommon.mNestedAggrExprList.mCount > 0 )
    {
        STL_TRY( qlvCreateRefExprList( &sNestedTotalAggrFuncList,
                                       QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                       aEnv )
                 == STL_SUCCESS );

        sNestedTotalAggrFuncList->mCount = sPlanHashGroupInstant->mCommon.mNestedAggrExprList.mCount;
        sNestedTotalAggrFuncList->mHead = sPlanHashGroupInstant->mCommon.mNestedAggrExprList.mHead;
        sNestedTotalAggrFuncList->mTail = sPlanHashGroupInstant->mCommon.mNestedAggrExprList.mTail;
    }
    else
    {
        sNestedTotalAggrFuncList = NULL;
    }


    /****************************************************************
     * GROUP INSTANT ACCESS Optimization Node 정보 설정
     ****************************************************************/

    /**
     * Node Type
     */
    
    sOptHashGroupInstant->mType = QLV_NODE_TYPE_GROUP_HASH_INSTANT;


    /**
     * Row Block이 필요한지 여부
     */

    sOptHashGroupInstant->mNeedRowBlock = sPlanHashGroupInstant->mCommon.mNeedRowBlock;


    /**
     * Materialization
     */

    sOptHashGroupInstant->mNeedRebuild = sPlanHashGroupInstant->mNeedRebuild;


    /**
     * Scrollable
     */
    
    sOptHashGroupInstant->mScrollable = sPlanHashGroupInstant->mScrollable;


    /**
     * Child Node
     */
    
    sOptHashGroupInstant->mChildNode = sOptChildNode;
    

    /**
     * Instant Table Node
     */
    
    sOptHashGroupInstant->mInstantNode = (qlvInitInstantNode*)(sPlanHashGroupInstant->mCommon.mInitNode);


    /**
     * Base Relation Node List
     */

    sOptHashGroupInstant->mSrcRelationList = sSrcRelationList;


    /**
     * Skip Hit Record
     */

    sOptHashGroupInstant->mIsSkipHitRecord = STL_FALSE;


    /**
     * Outer Column 관련
     */

    sOptHashGroupInstant->mOuterColumnList = sPlanHashGroupInstant->mOuterColumnList;


    /**
     * Total Filter 관련
     */

    STL_DASSERT( sPlanHashGroupInstant->mHashKeyFilterExpr == NULL );
    sOptHashGroupInstant->mTotalFilterExpr = sPlanHashGroupInstant->mNonHashKeyFilterExpr;

    
    /**
     * Index Scan 관련
     */
    
    /* 사용하지 않음 */
    sOptHashGroupInstant->mSortTableAttr.mTopDown      = STL_FALSE;
    sOptHashGroupInstant->mSortTableAttr.mVolatile     = STL_FALSE;
    sOptHashGroupInstant->mSortTableAttr.mLeafOnly     = STL_FALSE;
    sOptHashGroupInstant->mSortTableAttr.mDistinct     = STL_FALSE;
    sOptHashGroupInstant->mSortTableAttr.mNullExcluded = STL_FALSE;
    sOptHashGroupInstant->mIndexScanInfo = NULL;


    /**
     * Hash 관련
     */
    
    stlMemset( &sOptHashGroupInstant->mHashTableAttr, 0x00, STL_SIZEOF( smlIndexAttr ) );
    sOptHashGroupInstant->mHashScanInfo = NULL;

    
    /**
     * Aggregation 관련
     */

    sOptHashGroupInstant->mAggrFuncCnt       = sAggrFuncCnt;
    sOptHashGroupInstant->mAggrFuncList      = sAggrFuncList;

    sOptHashGroupInstant->mTotalAggrFuncList = sTotalAggrFuncList;
    sOptHashGroupInstant->mAggrDistFuncCnt   = sAggrDistFuncCnt;
    sOptHashGroupInstant->mAggrDistFuncList  = sAggrDistFuncList;


    /**
     * NestedAggregation 관련
     */

    sOptHashGroupInstant->mNestedAggrFuncCnt       = sNestedAggrFuncCnt;
    sOptHashGroupInstant->mNestedAggrFuncList      = sNestedAggrFuncList;

    sOptHashGroupInstant->mNestedTotalAggrFuncList = sNestedTotalAggrFuncList;
    sOptHashGroupInstant->mNestedAggrDistFuncCnt   = sNestedAggrDistFuncCnt;
    sOptHashGroupInstant->mNestedAggrDistFuncList  = sNestedAggrDistFuncList;


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
 * @brief GROUP INSTANT ACCESS의 Optimization Node 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoCompleteGroupHashInstantAccess( qllDBCStmt            * aDBCStmt,
                                              qllStatement          * aSQLStmt,
                                              qllOptNodeList        * aOptNodeList,
                                              qllOptimizationNode   * aOptNode,
                                              qloInitExprList       * aQueryExprList,
                                              qllEnv                * aEnv )
{
    qlnoInstant        * sOptGroupInstant       = NULL;

    qlvRefExprItem     * sRefColumnItem         = NULL;
    qloDBType          * sExprDBType            = NULL;
    qlvInstantColDesc  * sColumnDesc            = NULL;
    qlvInstantColDesc  * sCurColumnDesc         = NULL;
    
    qlvInitExpression  * sPhysicalFilterExpr    = NULL;
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
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_GROUP_HASH_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );    
    

    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/

    sOptGroupInstant = (qlnoInstant *) aOptNode->mOptNode;

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
                   sOptGroupInstant->mChildNode,
                   aQueryExprList,
                   aEnv );

    
    /****************************************************************
     * Column Expression Stack 설정
     ****************************************************************/

    /**
     * Column Expression Code Stack 공간 할당
     */

    sTotalColCnt = ( sOptGroupInstant->mInstantNode->mKeyColCnt +
                     sOptGroupInstant->mInstantNode->mRecColCnt );
    
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

    sOptGroupInstant->mNeedEvalKeyCol = STL_FALSE;
    sRefColumnItem = sOptGroupInstant->mInstantNode->mKeyColList->mHead;    
    for( sLoop = 0 ; sLoop < sOptGroupInstant->mInstantNode->mKeyColCnt ; sLoop++, sIdx++ )
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
            sOptGroupInstant->mNeedEvalKeyCol = STL_TRUE;
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
    sOptGroupInstant->mNeedEvalRecCol = STL_FALSE;
    sRefColumnItem = sOptGroupInstant->mInstantNode->mRecColList->mHead;
    for( sLoop = 0 ; sLoop < sOptGroupInstant->mInstantNode->mRecColCnt ; sLoop++, sIdx++ )
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

    sOptGroupInstant->mNeedEvalInsertCol =
        ( sOptGroupInstant->mNeedEvalKeyCol || sOptGroupInstant->mNeedEvalRecCol );
    

    /****************************************************************
     * Aggregation 에 대한 Expression Stack 구성
     ****************************************************************/

    /**
     * Aggregation Function 정보 구축
     */

    if( sOptGroupInstant->mAggrFuncCnt > 0 )
    {
        STL_TRY( qlnfGetAggrOptInfo( aSQLStmt,
                                     aQueryExprList,
                                     sOptGroupInstant->mAggrFuncCnt,
                                     sOptGroupInstant->mAggrFuncList,
                                     & sOptGroupInstant->mAggrOptInfo,
                                     QLL_CODE_PLAN( aDBCStmt ),
                                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sOptGroupInstant->mAggrOptInfo = NULL;
    }

    
    /****************************************************************
     * Aggregation Distinct에 대한 Expression Stack 구성
     ****************************************************************/

    /**
     * Aggregation Function 정보 구축
     */

    if( sOptGroupInstant->mAggrDistFuncCnt > 0 )
    {
        STL_TRY( qlnfGetAggrOptInfo( aSQLStmt,
                                     aQueryExprList,
                                     sOptGroupInstant->mAggrDistFuncCnt,
                                     sOptGroupInstant->mAggrDistFuncList,
                                     & sOptGroupInstant->mAggrDistOptInfo,
                                     QLL_CODE_PLAN( aDBCStmt ),
                                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sOptGroupInstant->mAggrDistOptInfo = NULL;
    }


    /****************************************************************
     * Nested Aggregation에 대한 Expression Stack 구성
     ****************************************************************/

    /**
     * Nested Aggregation Function 정보 구축
     */

    if( sOptGroupInstant->mNestedAggrFuncCnt > 0 )
    {
        STL_TRY( qlnfGetAggrOptInfo( aSQLStmt,
                                     aQueryExprList,
                                     sOptGroupInstant->mNestedAggrFuncCnt,
                                     sOptGroupInstant->mNestedAggrFuncList,
                                     & sOptGroupInstant->mNestedAggrOptInfo,
                                     QLL_CODE_PLAN( aDBCStmt ),
                                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sOptGroupInstant->mNestedAggrOptInfo = NULL;
    }


    /****************************************************************
     * Nested Aggregation Distinct에 대한 Expression Stack 구성
     ****************************************************************/

    /**
     * Nested Aggregation Function 정보 구축
     */

    if( sOptGroupInstant->mNestedAggrDistFuncCnt > 0 )
    {
        STL_TRY( qlnfGetAggrOptInfo( aSQLStmt,
                                     aQueryExprList,
                                     sOptGroupInstant->mNestedAggrDistFuncCnt,
                                     sOptGroupInstant->mNestedAggrDistFuncList,
                                     & sOptGroupInstant->mNestedAggrDistOptInfo,
                                     QLL_CODE_PLAN( aDBCStmt ),
                                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sOptGroupInstant->mNestedAggrDistOptInfo = NULL;
    }


    /****************************************************************
     * Physical & Logical Filter 구분
     ****************************************************************/
    
    /**
     * Logical Filter 와 Physical Filter 분리
     */

    if( sOptGroupInstant->mTotalFilterExpr == NULL )
    {
        sLogicalFilterExpr  = NULL;
        sPhysicalFilterExpr = NULL;
        sLogicalFilterPred  = NULL;
        sOptGroupInstant->mPrepareFilterStack = NULL;
    }
    else 
    {   
        /*******************************************
         * Physical & Logical Filter 구분
         *******************************************/
        
        /**
         * Logical Filter 와 Physical Filter Expression 분리
         */

        STL_TRY( qloClassifyFilterExpr( aSQLStmt->mRetrySQL,
                                        sOptGroupInstant->mTotalFilterExpr,
                                        (qlvInitNode *) sOptGroupInstant->mInstantNode,
                                        QLL_CODE_PLAN( aDBCStmt ),
                                        aSQLStmt->mBindContext,
                                        & sLogicalFilterExpr,
                                        & sPhysicalFilterExpr,
                                        aEnv )
                 == STL_SUCCESS );

        
        /*************************
         * Physical Filter 
         *************************/

        /**
         * Prepare Table Filter Expression Stack 생성
         */
        
        if( sPhysicalFilterExpr == NULL )
        {
            sOptGroupInstant->mPrepareFilterStack = NULL;
        }
        else
        {
            STL_TRY( qloMakePhysicalFilterStack( aDBCStmt,
                                                 aSQLStmt,
                                                 aQueryExprList,
                                                 sPhysicalFilterExpr,
                                                 & sOptGroupInstant->mPrepareFilterStack,
                                                 aEnv )
                     == STL_SUCCESS );
        }                           


        /*************************
         * Logical Filter 
         *************************/

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
    }


    /****************************************************************
     * Column Description 정보 설정
     ****************************************************************/

    /**
     * Column Description 공간 할당
     */

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlvInstantColDesc ) * sTotalColCnt,
                                (void **) & sColumnDesc,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Group Key Column Descriptor 설정
     */

    sIdx = 0;

    sRefColumnItem = sOptGroupInstant->mInstantNode->mKeyColList->mHead;
    for( sLoop = 0 ; sLoop < sOptGroupInstant->mInstantNode->mKeyColCnt ; sLoop++, sIdx++ )
    {
        STL_DASSERT( sRefColumnItem != NULL );
        STL_DASSERT( sIdx == *sRefColumnItem->mExprPtr->mExpr.mInnerColumn->mIdx );
        
        sCurColumnDesc = & sColumnDesc[ sIdx ];
        
        sCurColumnDesc->mIdx         = sIdx;
        
        sCurColumnDesc->mIsSortKey   = STL_TRUE;
        sCurColumnDesc->mKeyColFlags = DTL_KEYCOLUMN_EMPTY;   /* no use */

        sExprDBType = & aQueryExprList->mStmtExprList->mTotalExprInfo->mExprDBType[ sRefColumnItem->mExprPtr->mOffset ];

        STL_TRY_THROW( dtlCheckKeyCompareType( sExprDBType->mDBType ) == STL_TRUE,
                       RAMP_ERR_INVALID_KEY_COMP_TYPE );
            
        sCurColumnDesc->mType              = sExprDBType->mDBType;
        sCurColumnDesc->mArgNum1           = sExprDBType->mArgNum1;
        sCurColumnDesc->mArgNum2           = sExprDBType->mArgNum2;
        sCurColumnDesc->mStringLengthUnit  = sExprDBType->mStringLengthUnit;
        sCurColumnDesc->mIntervalIndicator = sExprDBType->mIntervalIndicator;

        sCurColumnDesc->mExpr = sRefColumnItem->mExprPtr;

        sRefColumnItem = sRefColumnItem->mNext;
    }
    

    /**
     * Record Column Descriptor 설정
     */

    sRefColumnItem = sOptGroupInstant->mInstantNode->mRecColList->mHead;
    for( sLoop = 0 ; sLoop < sOptGroupInstant->mInstantNode->mRecColCnt ; sLoop++, sIdx++ )
    {
        STL_DASSERT( sRefColumnItem != NULL );
        STL_DASSERT( sIdx == *sRefColumnItem->mExprPtr->mExpr.mInnerColumn->mIdx );
        
        sCurColumnDesc = & sColumnDesc[ sIdx ];
        
        sCurColumnDesc->mIdx         = sIdx;
        
        sCurColumnDesc->mIsSortKey   = STL_FALSE;
        sCurColumnDesc->mKeyColFlags = DTL_KEYCOLUMN_EMPTY;   /* no use */

        sExprDBType = & aQueryExprList->mStmtExprList->mTotalExprInfo->mExprDBType[ sRefColumnItem->mExprPtr->mOffset ];
            
        sCurColumnDesc->mType              = sExprDBType->mDBType;
        sCurColumnDesc->mArgNum1           = sExprDBType->mArgNum1;
        sCurColumnDesc->mArgNum2           = sExprDBType->mArgNum2;
        sCurColumnDesc->mStringLengthUnit  = sExprDBType->mStringLengthUnit;
        sCurColumnDesc->mIntervalIndicator = sExprDBType->mIntervalIndicator;

        sCurColumnDesc->mExpr = sRefColumnItem->mExprPtr;

        sRefColumnItem = sRefColumnItem->mNext;
    }


    /****************************************************************
     * Node 정보 설정
     ****************************************************************/

    /**
     * Column 관련
     */
    
    sOptGroupInstant->mColumnDesc        = sColumnDesc;
    sOptGroupInstant->mColExprStack      = sColExprStackList;

    
    /**
     * Predicate 관련
     */

    sOptGroupInstant->mPredicate = sLogicalFilterPred;

    
    /**
     * Table Filter 관련
     */

    sOptGroupInstant->mTablePhysicalFilterExpr = sPhysicalFilterExpr;
    sOptGroupInstant->mTableLogicalFilterExpr  = sLogicalFilterExpr;
    sOptGroupInstant->mTableLogicalStack       = sLogicalFilterStack;
    sOptGroupInstant->mPrepareRangeStack       = NULL;
    sOptGroupInstant->mPrepareKeyFilterStack   = NULL;


    /**
     * Index Key Filter 관련
     */
    
    sOptGroupInstant->mIndexKeyLogicalStack = NULL;


    /**
     * Aggregation 관련
     */

    /* sOptGroupInstant->mAggrOptInfoForFilter = NULL; */


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

    STL_CATCH( RAMP_ERR_INVALID_KEY_COMP_TYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_ILLEGAL_USE_OF_DATA_TYPE,
                      qlgMakeErrPosString( aSQLStmt->mRetrySQL,
                                           sRefColumnItem->mExprPtr->mPosition,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      dtlDataTypeName[ sExprDBType->mDBType ] );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief GROUP INSTANT ACCESS의 All Expr 리스트 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoAddExprGroupHashInstantAccess( qllDBCStmt            * aDBCStmt,
                                             qllStatement          * aSQLStmt,
                                             qllOptNodeList        * aOptNodeList,
                                             qllOptimizationNode   * aOptNode,
                                             qloInitExprList       * aQueryExprList,
                                             qllEnv                * aEnv )
{
    qlnoInstant             * sOptGroupInstant = NULL;
    qlvRefExprItem          * sRefExprItem     = NULL;
    qlvInitInstantNode      * sInstantNode     = NULL;

    
    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_GROUP_HASH_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );    

    
    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/

    sOptGroupInstant = (qlnoInstant *) aOptNode->mOptNode;

    
    /****************************************************************
     * Child Node의 Add Expression 수행
     ****************************************************************/

    QLNO_ADD_EXPR( aDBCStmt,
                   aSQLStmt,
                   aOptNodeList,
                   sOptGroupInstant->mChildNode,
                   aQueryExprList,
                   aEnv );
    

    /****************************************************************
     * Add Expr
     ****************************************************************/

    /**
     * Add Expr Group Instant Table : Group Key Column
     */

    sInstantNode = sOptGroupInstant->mInstantNode;

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
     * Add Expr Where Clause Expression
     */
        
    if( sOptGroupInstant->mTotalFilterExpr != NULL )
    {
        STL_TRY( qloAddExpr( sOptGroupInstant->mTotalFilterExpr,
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
    

    /**
     * Add Expr Nested Aggregation Functions
     */

    if( sOptGroupInstant->mNestedAggrFuncCnt > 0 )
    {
        sRefExprItem = sOptGroupInstant->mNestedAggrFuncList->mHead;

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
     * Add Expr Nested Aggregation Distinct Functions
     */

    if( sOptGroupInstant->mNestedAggrDistFuncCnt > 0 )
    {
        sRefExprItem = sOptGroupInstant->mNestedAggrDistFuncList->mHead;

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
