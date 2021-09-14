/*******************************************************************************
 * qlnoSortJoinInstantAccess.c
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
 * @file qlnoSortJoinInstantAccess.c
 * @brief SQL Processor Layer Code Optimization Node - SORT JOIN INSTANT ACCESS
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlno
 * @{
 */

/**
 * @brief Sort Join Instant에 대한 Code Optimize를 수행한다.
 * @param[in]   aCodeOptParamInfo   Code Optimize Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlnoCodeOptimizeSortJoinInstant( qloCodeOptParamInfo  * aCodeOptParamInfo,
                                           qllEnv               * aEnv )
{
    qlncPlanSortInstant     * sPlanSortInstant  = NULL;
    qllOptimizationNode     * sOptNode          = NULL;
    qlnoInstant             * sOptSortInstant   = NULL;

    qllEstimateCost         * sOptCost          = NULL;

    qllOptimizationNode     * sOptChildNode     = NULL;

    qloIndexScanInfo        * sIndexScanInfo    = NULL;

    qllOptNodeList          * sSrcRelationList  = NULL;

    
    /*
     * Paramter Validation
     */

    QLO_CODE_OPT_DEFAULT_PARAM_VALIDATE( aCodeOptParamInfo, aEnv );
    STL_PARAM_VALIDATE( aCodeOptParamInfo->mCandPlan->mPlanType == QLNC_PLAN_TYPE_SORT_JOIN_INSTANT,
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
                                QLL_PLAN_NODE_SORT_JOIN_INSTANT_ACCESS_TYPE,
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
    
    sOptSortInstant->mType = QLV_NODE_TYPE_SORT_JOIN_INSTANT;


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
     * Aggregation 관련 : PUSH 할 수 없음
     */
    
    /* 사용하지 않음 */
    sOptSortInstant->mAggrFuncCnt       = 0;
    sOptSortInstant->mAggrDistFuncCnt   = 0;

    sOptSortInstant->mTotalAggrFuncList = NULL;
    sOptSortInstant->mAggrFuncList      = NULL;
    sOptSortInstant->mAggrDistFuncList  = NULL;

    
    /**
     * Nested Aggregation 관련 : PUSH 할 수 없음
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
 * @brief SORT JOIN INSTANT ACCESS의 Optimization Node 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoCompleteSortJoinInstantAccess( qllDBCStmt            * aDBCStmt,
                                             qllStatement          * aSQLStmt,
                                             qllOptNodeList        * aOptNodeList,
                                             qllOptimizationNode   * aOptNode,
                                             qloInitExprList       * aQueryExprList,
                                             qllEnv                * aEnv )
{
    qlnoInstant        * sOptSortInstant     = NULL;

    knlExprStack       * sKeyLogicalStack    = NULL;

    qlvRefExprItem     * sRefColumnItem      = NULL;
    qloDBType          * sExprDBType         = NULL;
    qlvInstantColDesc  * sColumnDesc         = NULL;
    qlvInstantColDesc  * sCurColumnDesc      = NULL;
    
    qlvInitExpression  * sCodeExpr           = NULL;
    knlExprEntry       * sExprEntry          = NULL;

    qloIndexScanInfo   * sScanInfo           = NULL;
    
    knlExprStack      ** sColExprStackList   = NULL;
    knlExprStack       * sColExprStack       = NULL;
    knlExprStack         sTempColExprStack;

    stlInt32             sLoop               = 0;
    stlInt32             sIdx                = 0;
    stlInt32             sTotalColCnt        = 0;

    qlvInitInstantNode * sIniSortInstant     = NULL;
    

    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SORT_JOIN_INSTANT_ACCESS_TYPE,
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

        /* Sort Merge Join을 위한 Instant 구성시 Record Column에 대한 Expression은
         * 변경 없이 원본 그대로 저장하기 때문에 
         * Record Column 은 항상 KNL_EXPR_TYPE_VALUE 이며, internal cast 가 존재하지 않는다. */
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
     * => Sort Merge Join을 위한 Physical Key Filter는 없다.
     */

    STL_DASSERT( sScanInfo->mPhysicalKeyFilterExprList == NULL );

    sOptSortInstant->mPrepareKeyFilterStack = NULL;
    
    
    /****************************************************************
     * Table Physical Filter 관련 설정
     ****************************************************************/
    
    /**
     * Prepare Table Filter Expression Stack 생성
     * => Sort Merge Join을 위한 Physical Table Filter는 없다.
     */
        
    STL_DASSERT( sScanInfo->mPhysicalTableFilterExprList == NULL );

    sOptSortInstant->mPrepareFilterStack = NULL;


    /****************************************************************
     * Key Logical Filter 관련 설정
     ****************************************************************/

    /**
     * Key Logical Filter를 위한 Expression Stack 구성
     * => Sort Merge Join을 위한 Logical Key Filter는 없다.
     */
        
    STL_DASSERT( sScanInfo->mLogicalKeyFilter == NULL );

    sKeyLogicalStack = NULL;
        
    
    /****************************************************************
     * Table Logical Filter 관련 설정
     ****************************************************************/

    /**
     * Table Logical Filter를 위한 Expression Stack 구성
     * => Sort Merge Join을 위한 Logical Table Filter는 없다.
     */
        
    STL_DASSERT( sScanInfo->mLogicalTableFilter == NULL );


    /****************************************************************
     * Index Scan을 위해 추가된 Expression 관련 설정
     ****************************************************************/
    
    /**
     * Add FALSE Expression
     */

    STL_DASSERT( sScanInfo->mNullValueExpr == NULL );

    
    /**
     * Add FALSE Expression
     */

    STL_DASSERT( sScanInfo->mFalseValueExpr == NULL );


    /**
     * Range Predicate 구성
     */

    STL_DASSERT( sKeyLogicalStack == NULL );


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

        STL_DASSERT( dtlCheckKeyCompareType( sExprDBType->mDBType ) == STL_TRUE );
            
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

    sOptSortInstant->mPredicate = NULL;

    
    /**
     * Table Filter 관련
     */

    sOptSortInstant->mTablePhysicalFilterExpr = NULL;
    sOptSortInstant->mTableLogicalFilterExpr  = NULL;
    sOptSortInstant->mTableLogicalStack       = NULL;


    /**
     * Index Key Filter 관련
     */
    
    sOptSortInstant->mIndexKeyLogicalStack = sKeyLogicalStack;


    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SORT JOIN INSTANT ACCESS의 All Expr 리스트 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoAddExprSortJoinInstantAccess( qllDBCStmt            * aDBCStmt,
                                            qllStatement          * aSQLStmt,
                                            qllOptNodeList        * aOptNodeList,
                                            qllOptimizationNode   * aOptNode,
                                            qloInitExprList       * aQueryExprList,
                                            qllEnv                * aEnv )
{
    qlnoInstant             * sOptSortInstant   = NULL;
    qlvRefExprItem          * sRefColumnItem    = NULL;
    qlvInitInstantNode      * sInstantNode      = NULL;

    
    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SORT_JOIN_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );    

    
    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/

    sOptSortInstant = (qlnoInstant *) aOptNode->mOptNode;

    
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
     * Add Expr Sort Instant Table Column
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
     * Range 와 Filter 가 존재하지 않기 때문에 Scan Info를 구성하지 않는다.
     */

    STL_DASSERT( sOptSortInstant->mIndexScanInfo->mReadKeyCnt == 0 );

    /* Do Nothing */


    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlno */
