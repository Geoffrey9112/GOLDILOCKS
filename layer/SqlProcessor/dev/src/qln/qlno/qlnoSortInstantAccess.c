/*******************************************************************************
 * qlnoSortInstantAccess.c
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
 * @file qlnoSortInstantAccess.c
 * @brief SQL Processor Layer Code Optimization Node - SORT INSTANT ACCESS
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlno
 * @{
 */


/**
 * @brief Sort Instant에 대한 Code Optimize를 수행한다.
 * @param[in]   aCodeOptParamInfo   Code Optimize Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlnoCodeOptimizeSortInstant( qloCodeOptParamInfo  * aCodeOptParamInfo,
                                       qllEnv               * aEnv )
{
    qlncPlanSortInstant     * sPlanSortInstant  = NULL;
    qllOptimizationNode     * sOptNode          = NULL;
    qlnoInstant             * sOptSortInstant   = NULL;
    
    qllEstimateCost         * sOptCost          = NULL;
    qllOptimizationNode     * sOptChildNode     = NULL;

    qloIndexScanInfo        * sIndexScanInfo    = NULL;

    qllOptNodeList          * sSrcRelationList  = NULL;

    qllOptimizationNode     * sOptAggrNode      = NULL;
    qlvAggregation          * sIniAggrNode      = NULL;


    /*
     * Paramter Validation
     */

    QLO_CODE_OPT_DEFAULT_PARAM_VALIDATE( aCodeOptParamInfo, aEnv );
    STL_PARAM_VALIDATE( aCodeOptParamInfo->mCandPlan->mPlanType == QLNC_PLAN_TYPE_SORT_INSTANT,
                        QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Sort Instant Plan 정보 획득
     ****************************************************************/

    sPlanSortInstant = (qlncPlanSortInstant*)(aCodeOptParamInfo->mCandPlan);


    /****************************************************************
     * Optimization Node 생성
     ****************************************************************/

    /**
     * SORT INSTANT ACCESS Optimization Node 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                STL_SIZEOF( qlnoInstant ),
                                (void **) & sOptSortInstant,
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
                                QLL_PLAN_NODE_SORT_INSTANT_ACCESS_TYPE,
                                sOptSortInstant,
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

    aCodeOptParamInfo->mCandPlan = sPlanSortInstant->mChildPlanNode;
    QLNO_CODE_OPTIMIZE( aCodeOptParamInfo, aEnv );
    sOptChildNode = aCodeOptParamInfo->mOptNode;

    
    /**
     * Aggregation Node 구성
     */

    if( sPlanSortInstant->mCommon.mAggrExprList.mCount > 0 )
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

        sIniAggrNode->mAggrExprList->mCount = sPlanSortInstant->mCommon.mAggrExprList.mCount;
        sIniAggrNode->mAggrExprList->mHead = sPlanSortInstant->mCommon.mAggrExprList.mHead;
        sIniAggrNode->mAggrExprList->mTail = sPlanSortInstant->mCommon.mAggrExprList.mTail;

        STL_TRY( qloCodeOptimizeHashAggregation( aCodeOptParamInfo->mDBCStmt,
                                                 aCodeOptParamInfo->mSQLStmt,
                                                 sOptNode,
                                                 sOptChildNode,
                                                 sIniAggrNode,
                                                 aCodeOptParamInfo->mQueryExprList,
                                                 aCodeOptParamInfo->mOptNodeList,
                                                 & sOptAggrNode,
                                                 sOptCost,
                                                 aEnv )
                 == STL_SUCCESS );

        sOptChildNode = sOptAggrNode;
    }

    
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
     * Statement Expression List에 Sort Key Column 추가
     */

    STL_TRY( qlvAddExprListToRefExprList(
                 aCodeOptParamInfo->mQueryExprList->mStmtExprList->mColumnExprList,
                 sPlanSortInstant->mKeyColList,
                 STL_FALSE,
                 QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                 aEnv )
             == STL_SUCCESS );


    /**
     * Statement Expression List에 Record Column 추가
     */

    STL_TRY( qlvAddExprListToRefExprList(
                 aCodeOptParamInfo->mQueryExprList->mStmtExprList->mColumnExprList,
                 sPlanSortInstant->mRecColList,
                 STL_FALSE,
                 QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                 aEnv )
             == STL_SUCCESS );


    /**
     * Statement Expression List에 Read Column 추가
     * @remark Read Column은 Sort Key Column과 Record Column의 부분 집합이기 때문에
     *         Column Expression List에 추가할 필요 없다.
     */


    /****************************************************************
     * Validate Sort Instant 정보 설정
     ****************************************************************/

    /* Key Column List */
    STL_DASSERT( sPlanSortInstant->mKeyColList != NULL );
    STL_DASSERT( sPlanSortInstant->mKeyColList->mCount > 0 );
    ((qlvInitInstantNode*)(sPlanSortInstant->mCommon.mInitNode))->mKeyColList =
        sPlanSortInstant->mKeyColList;
    ((qlvInitInstantNode*)(sPlanSortInstant->mCommon.mInitNode))->mKeyColCnt =
        sPlanSortInstant->mKeyColList->mCount;

    /* Record Column List */
    STL_DASSERT( sPlanSortInstant->mRecColList != NULL );
    ((qlvInitInstantNode*)(sPlanSortInstant->mCommon.mInitNode))->mRecColList =
        sPlanSortInstant->mRecColList;
    ((qlvInitInstantNode*)(sPlanSortInstant->mCommon.mInitNode))->mRecColCnt =
        sPlanSortInstant->mRecColList->mCount;

    /* Read Column List */
    STL_DASSERT( sPlanSortInstant->mReadColList != NULL );
    ((qlvInitInstantNode*)(sPlanSortInstant->mCommon.mInitNode))->mReadColList =
        sPlanSortInstant->mReadColList;
    ((qlvInitInstantNode*)(sPlanSortInstant->mCommon.mInitNode))->mReadColCnt =
        sPlanSortInstant->mReadColList->mCount;

    /* Key Flags */
    stlMemcpy( ((qlvInitInstantNode*)(sPlanSortInstant->mCommon.mInitNode))->mKeyFlags,
               sPlanSortInstant->mKeyFlags,
               STL_SIZEOF( stlUInt8 ) * sPlanSortInstant->mKeyColList->mCount );


    /****************************************************************
     * Range 정보 구성
     ****************************************************************/

    /**
     * Range 요소별 Expression 분류 
     */

    STL_TRY( qloGetKeyRange( aCodeOptParamInfo->mSQLStmt->mRetrySQL,
                             NULL,
                             (qlvInitNode*)(sPlanSortInstant->mCommon.mInitNode),
                             aCodeOptParamInfo->mSQLStmt->mBindContext,
                             sPlanSortInstant->mFilterExpr,
                             & sIndexScanInfo,
                             QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                             aEnv )
             == STL_SUCCESS );


    /****************************************************************
     * SORT INSTANT ACCESS Optimization Node 정보 설정
     ****************************************************************/

    /**
     * Node Type
     */
    
    sOptSortInstant->mType = QLV_NODE_TYPE_SORT_INSTANT;


    /**
     * Row Block이 필요한지 여부
     */

    sOptSortInstant->mNeedRowBlock = sPlanSortInstant->mCommon.mNeedRowBlock;


    /**
     * Materialization
     */

    sOptSortInstant->mNeedRebuild = sPlanSortInstant->mNeedRebuild;


    /**
     * Scrollable
     */

    sOptSortInstant->mScrollable = sPlanSortInstant->mScrollable;


    /**
     * Child Node
     */
    
    sOptSortInstant->mChildNode = sOptChildNode;
    

    /**
     * Instant Table Node
     */

    sOptSortInstant->mInstantNode = (qlvInitInstantNode*)(sPlanSortInstant->mCommon.mInitNode);


    /**
     * Base Relation Node List
     */

    sOptSortInstant->mSrcRelationList = sSrcRelationList;


    /**
     * Skip Hit Record
     */

    sOptSortInstant->mIsSkipHitRecord = STL_FALSE;


    /**
     * Outer Column List
     */

    sOptSortInstant->mOuterColumnList = sPlanSortInstant->mOuterColumnList;


    /**
     * Total Filter 관련
     */

    sOptSortInstant->mTotalFilterExpr = sPlanSortInstant->mFilterExpr;

    
    /**
     * Index Scan 관련
     */
    
    sOptSortInstant->mSortTableAttr = sPlanSortInstant->mSortTableAttr;
    sOptSortInstant->mIndexScanInfo = sIndexScanInfo;


    /**
     * Hash Scan 관련
     */

    /* 사용하지 않음 */
    stlMemset( &sOptSortInstant->mHashTableAttr, 0x00, STL_SIZEOF( smlIndexAttr ) );
    sOptSortInstant->mHashScanInfo = NULL;

    
    /**
     * Aggregation 관련
     */
    
    /* 사용하지 않음 */
    sOptSortInstant->mAggrFuncCnt       = 0;
    sOptSortInstant->mAggrDistFuncCnt   = 0;

    sOptSortInstant->mTotalAggrFuncList = NULL;
    sOptSortInstant->mAggrFuncList      = NULL;
    sOptSortInstant->mAggrDistFuncList  = NULL;

    
    /**
     * Nested Aggregation 관련
     */
    
    /* 사용하지 않음 */
    sOptSortInstant->mNestedAggrFuncCnt       = 0;
    sOptSortInstant->mNestedAggrDistFuncCnt   = 0;

    sOptSortInstant->mNestedTotalAggrFuncList = NULL;
    sOptSortInstant->mNestedAggrFuncList      = NULL;
    sOptSortInstant->mNestedAggrDistFuncList  = NULL;
    

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
 * @brief SORT INSTANT ACCESS의 Optimization Node 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoCompleteSortInstantAccess( qllDBCStmt            * aDBCStmt,
                                         qllStatement          * aSQLStmt,
                                         qllOptNodeList        * aOptNodeList,
                                         qllOptimizationNode   * aOptNode,
                                         qloInitExprList       * aQueryExprList,
                                         qllEnv                * aEnv )
{
    qlnoInstant        * sOptSortInstant     = NULL;

    knlExprStack       * sKeyLogicalStack    = NULL;
    knlExprStack       * sTableLogicalStack  = NULL;

    qlvRefExprItem     * sRefColumnItem      = NULL;
    qloDBType          * sExprDBType         = NULL;
    qlvInstantColDesc  * sColumnDesc         = NULL;
    qlvInstantColDesc  * sCurColumnDesc      = NULL;
    
    qlvInitExpression  * sCodeExpr           = NULL;
    knlExprEntry       * sExprEntry          = NULL;
    knlExprEntry         sTempExprEntry;

    qloIndexScanInfo   * sScanInfo           = NULL;

    qlvInitExpression  * sAndExpr            = NULL;

    knlExprStack      ** sColExprStackList   = NULL;
    knlExprStack       * sColExprStack       = NULL;
    knlExprStack         sTempColExprStack;

    stlInt32             sLoop               = 0;
    stlInt32             sIdx                = 0;
    stlInt32             sTotalColCnt        = 0;

    qlvInitInstantNode * sIniSortInstant     = NULL;
    knlPredicateList   * sRangePred          = NULL;
    

    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SORT_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );    
    

    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/

    sOptSortInstant = (qlnoInstant *) aOptNode->mOptNode;

    STL_DASSERT( sOptSortInstant->mIndexScanInfo != NULL );

    sScanInfo = sOptSortInstant->mIndexScanInfo;

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
                   sOptSortInstant->mChildNode,
                   aQueryExprList,
                   aEnv );


    /****************************************************************
     * Column Expression Stack 설정
     ****************************************************************/

    /**
     * Column Expression Code Stack 공간 할당
     */

    sTotalColCnt = ( sOptSortInstant->mInstantNode->mKeyColCnt +
                     sOptSortInstant->mInstantNode->mRecColCnt );
    
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
     * Sort Key Column Expression Stack 설정 (for Insert Columns)
     */
    
    sIdx = 0;

    sOptSortInstant->mNeedEvalKeyCol = STL_FALSE;
    sRefColumnItem = sOptSortInstant->mInstantNode->mKeyColList->mHead;
    for( sLoop = 0 ; sLoop < sOptSortInstant->mInstantNode->mKeyColCnt ; sLoop++, sIdx++ )
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
            sOptSortInstant->mNeedEvalKeyCol = STL_TRUE;
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

    sOptSortInstant->mNeedEvalRecCol = STL_FALSE;
    sRefColumnItem = sOptSortInstant->mInstantNode->mRecColList->mHead;
    for( sLoop = 0 ; sLoop < sOptSortInstant->mInstantNode->mRecColCnt ; sLoop++, sIdx++ )
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
            sOptSortInstant->mNeedEvalRecCol = STL_TRUE;
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

    sOptSortInstant->mNeedEvalInsertCol =
        ( sOptSortInstant->mNeedEvalKeyCol || sOptSortInstant->mNeedEvalRecCol );

    
    /****************************************************************
     * Range 관련 설정
     ****************************************************************/

    /**
     * Prepare Range Expression Stack 생성
     */

    STL_TRY( qloMakeRangeStack( aDBCStmt,
                                aSQLStmt,
                                aQueryExprList,
                                sScanInfo->mReadKeyCnt,
                                STL_FALSE,
                                sScanInfo->mMinRangeExpr,
                                sScanInfo->mMaxRangeExpr,
                                & sOptSortInstant->mPrepareRangeStack,
                                aEnv )
             == STL_SUCCESS );

    
    /****************************************************************
     * Key-Filter 관련 설정
     ****************************************************************/

    /**
     * Prepare Key Filter  Expression Stack 생성
     */

    if( sScanInfo->mPhysicalKeyFilterExprList == NULL )
    {
        sOptSortInstant->mPrepareKeyFilterStack = NULL;
    }
    else
    {
        if( sScanInfo->mPhysicalKeyFilterExprList->mCount == 0 )
        {
            sOptSortInstant->mPrepareKeyFilterStack = NULL;
        }
        else
        {
            STL_TRY( qlvMakeFuncAndFromExprList( & QLL_CANDIDATE_MEM( aEnv ),
                                                 0,
                                                 sScanInfo->mPhysicalKeyFilterExprList,
                                                 & sAndExpr,
                                                 aEnv )
                     == STL_SUCCESS );

            STL_TRY( qloMakePhysicalFilterStack( aDBCStmt,
                                                 aSQLStmt,
                                                 aQueryExprList,
                                                 sAndExpr,
                                                 & sOptSortInstant->mPrepareKeyFilterStack,
                                                 aEnv )
                     == STL_SUCCESS );
        }
    }
    
    
    /****************************************************************
     * Table Physical Filter 관련 설정
     ****************************************************************/
    
    /**
     * Prepare Table Filter Expression Stack 생성
     */
        
    if( sScanInfo->mPhysicalTableFilterExprList == NULL )
    {
        sOptSortInstant->mPrepareFilterStack = NULL;
    }
    else
    {
        if( sScanInfo->mPhysicalTableFilterExprList->mCount == 0 )
        {
            sOptSortInstant->mPrepareFilterStack = NULL;
        }
        else
        {
            STL_TRY( qlvMakeFuncAndFromExprList( & QLL_CANDIDATE_MEM( aEnv ),
                                                 0,
                                                 sScanInfo->mPhysicalTableFilterExprList,
                                                 & sAndExpr,
                                                 aEnv )
                     == STL_SUCCESS );

            STL_TRY( qloMakePhysicalFilterStack( aDBCStmt,
                                                 aSQLStmt,
                                                 aQueryExprList,
                                                 sAndExpr,
                                                 & sOptSortInstant->mPrepareFilterStack,
                                                 aEnv )
                     == STL_SUCCESS );
        }
    }


    /****************************************************************
     * Key Logical Filter 관련 설정
     ****************************************************************/

    /**
     * Key Logical Filter를 위한 Expression Stack 구성
     */
        
    if( sScanInfo->mLogicalKeyFilter == NULL )
    {
        sKeyLogicalStack = NULL;
    }
    else
    {
        /**
         * Logical Expression Code Stack 공간 할당
         */

        STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                    STL_SIZEOF( knlExprStack ),
                                    (void **) & sKeyLogicalStack,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );


        /**
         * Logical Expression Stack 생성
         */

        sCodeExpr = sScanInfo->mLogicalKeyFilter;

        STL_TRY( knlExprInit( sKeyLogicalStack,
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
                                            sKeyLogicalStack,
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
    }
        
    
    /****************************************************************
     * Table Logical Filter 관련 설정
     ****************************************************************/

    /**
     * Table Logical Filter를 위한 Expression Stack 구성
     */
        
    if( sScanInfo->mLogicalTableFilter == NULL )
    {
        sTableLogicalStack = NULL;
    }
    else
    {
        /**
         * Logical Expression Code Stack 공간 할당
         */

        STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                    STL_SIZEOF( knlExprStack ),
                                    (void **) & sTableLogicalStack,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );


        /**
         * Logical Expression Stack 생성
         */

        sCodeExpr = sScanInfo->mLogicalTableFilter;

        STL_TRY( knlExprInit( sTableLogicalStack,
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
                                            sTableLogicalStack,
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
    }


    /****************************************************************
     * Index Scan을 위해 추가된 Expression 관련 설정
     ****************************************************************/
    
    /**
     * Add FALSE Expression
     */

    if( sScanInfo->mNullValueExpr != NULL )
    {
        stlMemset( & sTempExprEntry, 0x00, STL_SIZEOF( knlExprEntry ) );
        STL_TRY( qloSetExprDBType( sScanInfo->mNullValueExpr,
                                   aQueryExprList->mStmtExprList->mTotalExprInfo,
                                   DTL_TYPE_BOOLEAN,
                                   gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mArgNum1,
                                   gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mArgNum2,
                                   gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mStringLengthUnit,
                                   gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mIntervalIndicator,
                                   aEnv )
                 == STL_SUCCESS );
    }

    
    /**
     * Add FALSE Expression
     */

    if( sScanInfo->mFalseValueExpr != NULL )
    {
        stlMemset( & sTempExprEntry, 0x00, STL_SIZEOF( knlExprEntry ) );
        STL_TRY( qloSetExprDBType( sScanInfo->mFalseValueExpr,
                                   aQueryExprList->mStmtExprList->mTotalExprInfo,
                                   DTL_TYPE_BOOLEAN,
                                   gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mArgNum1,
                                   gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mArgNum2,
                                   gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mStringLengthUnit,
                                   gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mIntervalIndicator,
                                   aEnv )
                 == STL_SUCCESS );
    }    


    /**
     * Range Predicate 구성
     */

    if( sKeyLogicalStack == NULL )
    {
        sRangePred = NULL;
    }
    else
    {
        STL_TRY( knlMakeRange( NULL, /* 설정할 필요 없음 */
                               NULL, /* 설정할 필요 없음 */
                               sKeyLogicalStack,
                               & sRangePred,
                               QLL_CODE_PLAN(aDBCStmt),
                               KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
    }


    /****************************************************************
     * Column Description 정보 설정
     ****************************************************************/

    /**
     * Column Description 공간 할당
     */

    sIniSortInstant = sOptSortInstant->mInstantNode;

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlvInstantColDesc ) * sTotalColCnt,
                                (void **) & sColumnDesc,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Sort Key Column Descriptor 설정
     */

    sIdx = 0;

    sRefColumnItem = sOptSortInstant->mInstantNode->mKeyColList->mHead;
    for( sLoop = 0 ; sLoop < sOptSortInstant->mInstantNode->mKeyColCnt ; sLoop++, sIdx++ )
    {
        STL_DASSERT( sRefColumnItem != NULL );
        STL_DASSERT( sIdx == *sRefColumnItem->mExprPtr->mExpr.mInnerColumn->mIdx );
        
        sCurColumnDesc = & sColumnDesc[ sIdx ];
        
        sCurColumnDesc->mIdx         = sIdx;
        
        sCurColumnDesc->mIsSortKey   = STL_TRUE;
        sCurColumnDesc->mKeyColFlags = sIniSortInstant->mKeyFlags[ sIdx ];

        sExprDBType = & aQueryExprList->mStmtExprList->mTotalExprInfo->mExprDBType[ sRefColumnItem->mExprPtr->mOffset ];

        STL_TRY_THROW( dtlCheckKeyCompareType( sExprDBType->mDBType ) == STL_TRUE,
                       RAMP_ERR_INVALID_KEY_COMP_TYPE );
            
        sCurColumnDesc->mType              = sExprDBType->mDBType;
        sCurColumnDesc->mArgNum1           = sExprDBType->mArgNum1;
        sCurColumnDesc->mArgNum2           = sExprDBType->mArgNum2;
        sCurColumnDesc->mStringLengthUnit  = sExprDBType->mStringLengthUnit;
        sCurColumnDesc->mIntervalIndicator = sExprDBType->mIntervalIndicator;

        sCurColumnDesc->mExpr = sRefColumnItem->mExprPtr;

        /* DBType 설정 */
        STL_TRY( qloSetExprDBType( sRefColumnItem->mExprPtr,
                                   aQueryExprList->mStmtExprList->mTotalExprInfo,
                                   sExprDBType->mDBType,
                                   sExprDBType->mArgNum1,
                                   sExprDBType->mArgNum2,
                                   sExprDBType->mStringLengthUnit,
                                   sExprDBType->mIntervalIndicator,
                                   aEnv )
                 == STL_SUCCESS );

        sRefColumnItem = sRefColumnItem->mNext;
    }
    

    /**
     * Record Column Descriptor 설정
     */

    sRefColumnItem = sOptSortInstant->mInstantNode->mRecColList->mHead;
    for( sLoop = 0 ; sLoop < sOptSortInstant->mInstantNode->mRecColCnt ; sLoop++, sIdx++ )
    {
        STL_DASSERT( sRefColumnItem != NULL );
        STL_DASSERT( sIdx == *sRefColumnItem->mExprPtr->mExpr.mInnerColumn->mIdx );
        
        sCurColumnDesc = & sColumnDesc[ sIdx ];
        
        sCurColumnDesc->mIdx         = sIdx;
        
        sCurColumnDesc->mIsSortKey   = STL_FALSE;
        sCurColumnDesc->mKeyColFlags = sIniSortInstant->mKeyFlags[ sIdx ];

        sExprDBType = & aQueryExprList->mStmtExprList->mTotalExprInfo->mExprDBType[ sRefColumnItem->mExprPtr->mOffset ];
            
        sCurColumnDesc->mType              = sExprDBType->mDBType;
        sCurColumnDesc->mArgNum1           = sExprDBType->mArgNum1;
        sCurColumnDesc->mArgNum2           = sExprDBType->mArgNum2;
        sCurColumnDesc->mStringLengthUnit  = sExprDBType->mStringLengthUnit;
        sCurColumnDesc->mIntervalIndicator = sExprDBType->mIntervalIndicator;

        sCurColumnDesc->mExpr = sRefColumnItem->mExprPtr;

        /* DBType 설정 */
        STL_TRY( qloSetExprDBType( sRefColumnItem->mExprPtr,
                                   aQueryExprList->mStmtExprList->mTotalExprInfo,
                                   sExprDBType->mDBType,
                                   sExprDBType->mArgNum1,
                                   sExprDBType->mArgNum2,
                                   sExprDBType->mStringLengthUnit,
                                   sExprDBType->mIntervalIndicator,
                                   aEnv )
                 == STL_SUCCESS );

        sRefColumnItem = sRefColumnItem->mNext;
    }


    /****************************************************************
     * Node 정보 설정
     ****************************************************************/

    /**
     * Column 관련
     */
    
    sOptSortInstant->mColumnDesc   = sColumnDesc;
    sOptSortInstant->mColExprStack = sColExprStackList;

    
    /**
     * Predicate 관련
     */

    sOptSortInstant->mPredicate = sRangePred;

    
    /**
     * Table Filter 관련
     */

    sOptSortInstant->mTablePhysicalFilterExpr = NULL;
    sOptSortInstant->mTableLogicalFilterExpr  = sScanInfo->mLogicalTableFilter;
    sOptSortInstant->mTableLogicalStack       = sTableLogicalStack;


    /**
     * Index Key Filter 관련
     */
    
    sOptSortInstant->mIndexKeyLogicalStack = sKeyLogicalStack;


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
 * @brief SORT INSTANT ACCESS의 All Expr 리스트 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoAddExprSortInstantAccess( qllDBCStmt            * aDBCStmt,
                                        qllStatement          * aSQLStmt,
                                        qllOptNodeList        * aOptNodeList,
                                        qllOptimizationNode   * aOptNode,
                                        qloInitExprList       * aQueryExprList,
                                        qllEnv                * aEnv )
{
    qlnoInstant             * sOptSortInstant   = NULL;
    qlvRefExprItem          * sRefColumnItem    = NULL;
    qlvInitInstantNode      * sInstantNode      = NULL;
    qlvRefExprItem          * sMinRefColumnItem = NULL;
    qlvRefExprItem          * sMaxRefColumnItem = NULL;
    qloIndexScanInfo        * sScanInfo         = NULL;
    stlInt32                  sLoop             = 0;

    
    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SORT_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );    

    
    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/

    sOptSortInstant = (qlnoInstant *) aOptNode->mOptNode;
    sScanInfo = sOptSortInstant->mIndexScanInfo;

    
    /****************************************************************
     * Child Node의 Add Expression 수행
     ****************************************************************/

    QLNO_ADD_EXPR( aDBCStmt,
                   aSQLStmt,
                   aOptNodeList,
                   sOptSortInstant->mChildNode,
                   aQueryExprList,
                   aEnv );
    

    /****************************************************************
     * Add Expr
     ****************************************************************/

    /**
     * Add Expr Sort Instant Table : Sort Key Column
     */

    sInstantNode = sOptSortInstant->mInstantNode;

    sRefColumnItem = sInstantNode->mKeyColList->mHead;
    while( sRefColumnItem != NULL )
    {
        STL_DASSERT( sRefColumnItem->mExprPtr->mType == QLV_EXPR_TYPE_INNER_COLUMN );

        STL_TRY( qloAddExpr( sRefColumnItem->mExprPtr->mExpr.mInnerColumn->mOrgExpr,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN( aDBCStmt ),
                             aEnv )
                 == STL_SUCCESS );

        STL_TRY( qloAddExpr( sRefColumnItem->mExprPtr,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN( aDBCStmt ),
                             aEnv )
                 == STL_SUCCESS );

        sRefColumnItem = sRefColumnItem->mNext;
    }


    /**
     * Add Expr Sort Instant Table : Record Column
     */

    sInstantNode = sOptSortInstant->mInstantNode;

    sRefColumnItem = sInstantNode->mRecColList->mHead;
    while( sRefColumnItem != NULL )
    {
        STL_DASSERT( sRefColumnItem->mExprPtr->mType == QLV_EXPR_TYPE_INNER_COLUMN );

        STL_TRY( qloAddExpr( sRefColumnItem->mExprPtr->mExpr.mInnerColumn->mOrgExpr,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN( aDBCStmt ),
                             aEnv )
                 == STL_SUCCESS );

        STL_TRY( qloAddExpr( sRefColumnItem->mExprPtr,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN( aDBCStmt ),
                             aEnv )
                 == STL_SUCCESS );

        sRefColumnItem = sRefColumnItem->mNext;
    }


    /**
     * Add Expr Sort Instant Table Column : Read Column
     * @remark Read Column은 Sort Key Column과 Record Column의 부분 집합이기 때문에
     *         Read Column의 Original Expresssion을 전체 Expression List에 추가할 필요 없다.
     *         동일한 Pointer를 공유하므로 추가할 필요 없음
     */

    sInstantNode = sOptSortInstant->mInstantNode;

    sRefColumnItem = sInstantNode->mReadColList->mHead;
    while( sRefColumnItem != NULL )
    {
        STL_DASSERT( sRefColumnItem->mExprPtr->mType == QLV_EXPR_TYPE_INNER_COLUMN );

        STL_TRY( qloAddExpr( sRefColumnItem->mExprPtr,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN( aDBCStmt ),
                             aEnv )
                 == STL_SUCCESS );

        sRefColumnItem = sRefColumnItem->mNext;
    }

    
    /**
     * Add Expr Min-Range Expression
     */

    for( sLoop = 0 ; sLoop < sScanInfo->mReadKeyCnt ; sLoop++ )
    {
        sMinRefColumnItem = sScanInfo->mMinRangeExpr[ sLoop ]->mHead;
        
        while( sMinRefColumnItem != NULL )
        {
            STL_TRY( qloAddExpr( sMinRefColumnItem->mExprPtr,
                                 aQueryExprList->mStmtExprList->mAllExprList,
                                 QLL_CODE_PLAN( aDBCStmt ),
                                 aEnv )
                     == STL_SUCCESS );

            sMinRefColumnItem = sMinRefColumnItem->mNext;
        }
    }
    

    /**
     * Add Expr Max-Range Expression
     */

    for( sLoop = 0 ; sLoop < sScanInfo->mReadKeyCnt ; sLoop++ )
    {
        sMaxRefColumnItem = sScanInfo->mMaxRangeExpr[ sLoop ]->mHead;
        
        while( sMaxRefColumnItem != NULL )
        {
            sMinRefColumnItem = sScanInfo->mMinRangeExpr[ sLoop ]->mHead;
            while( sMinRefColumnItem != NULL )
            {
                if( sMaxRefColumnItem->mExprPtr == sMinRefColumnItem->mExprPtr )
                {
                    break;
                }

                sMinRefColumnItem = sMinRefColumnItem->mNext;
            }

            if( sMinRefColumnItem == NULL )
            {
                STL_TRY( qloAddExpr( sMaxRefColumnItem->mExprPtr,
                                     aQueryExprList->mStmtExprList->mAllExprList,
                                     QLL_CODE_PLAN( aDBCStmt ),
                                     aEnv )
                         == STL_SUCCESS );
            }

            sMaxRefColumnItem = sMaxRefColumnItem->mNext;
        }
    }


    /**
     * Add Expr Physical Key Filter Expression
     */

    if( sScanInfo->mPhysicalKeyFilterExprList != NULL )
    {
        sRefColumnItem = sScanInfo->mPhysicalKeyFilterExprList->mHead;
        while( sRefColumnItem != NULL )
        {
            STL_TRY( qloAddExpr( sRefColumnItem->mExprPtr,
                                 aQueryExprList->mStmtExprList->mAllExprList,
                                 QLL_CODE_PLAN( aDBCStmt ),
                                 aEnv )
                     == STL_SUCCESS );

            sRefColumnItem = sRefColumnItem->mNext;
        }
    }

    
    /**
     * Add Expr Physical Table Filter Expression
     */

    if( sScanInfo->mPhysicalTableFilterExprList != NULL )
    {
        sRefColumnItem = sScanInfo->mPhysicalTableFilterExprList->mHead;
        while( sRefColumnItem != NULL )
        {
            STL_TRY( qloAddExpr( sRefColumnItem->mExprPtr,
                                 aQueryExprList->mStmtExprList->mAllExprList,
                                 QLL_CODE_PLAN( aDBCStmt ),
                                 aEnv )
                     == STL_SUCCESS );

            sRefColumnItem = sRefColumnItem->mNext;
        }
    }

    
    /**
     * Add Expr Logical Key Filter Expression
     */

    if( sScanInfo->mLogicalKeyFilter != NULL )
    {
        STL_TRY( qloAddExpr( sScanInfo->mLogicalKeyFilter,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN( aDBCStmt ),
                             aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Add Expr Logical Table Filter Expression
     */

    if( sScanInfo->mLogicalTableFilter != NULL )
    {
        STL_TRY( qloAddExpr( sScanInfo->mLogicalTableFilter,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN( aDBCStmt ),
                             aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Add NULL Expression
     */

    if( sScanInfo->mNullValueExpr != NULL )
    {
        STL_TRY( qloAddExpr( sScanInfo->mNullValueExpr,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN( aDBCStmt ),
                             aEnv )
                 == STL_SUCCESS );
    }

    
    /**
     * Add FALSE Expression
     */

    if( sScanInfo->mFalseValueExpr != NULL )
    {
        STL_TRY( qloAddExpr( sScanInfo->mFalseValueExpr,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN( aDBCStmt ),
                             aEnv )
                 == STL_SUCCESS );
    }


    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlno */
