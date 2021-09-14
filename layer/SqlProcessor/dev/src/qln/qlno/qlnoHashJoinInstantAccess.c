/*******************************************************************************
 * qlnoHashJoinInstantAccess.c
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
 * @file qlnoHashJoinInstantAccess.c
 * @brief SQL Processor Layer Code Optimization Node - HASH JOIN INSTANT ACCESS
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlno
 * @{
 */


/**
 * @brief Hash Join Instant에 대한 Code Optimize를 수행한다.
 * @param[in]   aCodeOptParamInfo   Code Optimize Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlnoCodeOptimizeHashJoinInstant( qloCodeOptParamInfo  * aCodeOptParamInfo,
                                           qllEnv               * aEnv )
{
    qlncPlanHashInstant     * sPlanHashInstant  = NULL;
    qllOptimizationNode     * sOptNode          = NULL;
    qlnoInstant             * sOptHashInstant   = NULL;
    qllOptimizationNode     * sOptChildNode     = NULL;
    
    qllEstimateCost         * sOptCost          = NULL;

    qloHashScanInfo         * sHashScanInfo     = NULL;

    qllOptNodeList          * sSrcRelationList  = NULL;

    
    /*
     * Paramter Validation
     */

    QLO_CODE_OPT_DEFAULT_PARAM_VALIDATE( aCodeOptParamInfo, aEnv );
    STL_PARAM_VALIDATE( aCodeOptParamInfo->mCandPlan->mPlanType == QLNC_PLAN_TYPE_HASH_JOIN_INSTANT,
                        QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Hash Instant Plan 정보 획득
     ****************************************************************/

    sPlanHashInstant = (qlncPlanHashInstant*)(aCodeOptParamInfo->mCandPlan);


    /****************************************************************
     * Optimization Node 생성
     ****************************************************************/

    /**
     * HASH INSTANT ACCESS Optimization Node 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                STL_SIZEOF( qlnoInstant ),
                                (void **) & sOptHashInstant,
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
                                QLL_PLAN_NODE_HASH_JOIN_INSTANT_ACCESS_TYPE,
                                sOptHashInstant,
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

    aCodeOptParamInfo->mCandPlan = sPlanHashInstant->mChildPlanNode;
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
     * Statement Expression List에 Hash Key Column 추가
     */

    STL_TRY( qlvAddExprListToRefExprList(
                 aCodeOptParamInfo->mQueryExprList->mStmtExprList->mColumnExprList,
                 sPlanHashInstant->mKeyColList,
                 STL_FALSE,
                 QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                 aEnv )
             == STL_SUCCESS );


    /**
     * Statement Expression List에 Record Column 추가
     */

    STL_TRY( qlvAddExprListToRefExprList(
                 aCodeOptParamInfo->mQueryExprList->mStmtExprList->mColumnExprList,
                 sPlanHashInstant->mRecColList,
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
     * Validate Hash Instant 정보 설정
     ****************************************************************/

    /* Key Column List */
    STL_DASSERT( sPlanHashInstant->mKeyColList != NULL );
    STL_DASSERT( sPlanHashInstant->mKeyColList->mCount > 0 );
    ((qlvInitInstantNode*)(sPlanHashInstant->mCommon.mInitNode))->mKeyColList =
        sPlanHashInstant->mKeyColList;
    ((qlvInitInstantNode*)(sPlanHashInstant->mCommon.mInitNode))->mKeyColCnt =
        sPlanHashInstant->mKeyColList->mCount;

    /* Record Column List */
    STL_DASSERT( sPlanHashInstant->mRecColList != NULL );
    ((qlvInitInstantNode*)(sPlanHashInstant->mCommon.mInitNode))->mRecColList =
        sPlanHashInstant->mRecColList;
    ((qlvInitInstantNode*)(sPlanHashInstant->mCommon.mInitNode))->mRecColCnt =
        sPlanHashInstant->mRecColList->mCount;

    /* Read Column List */
    STL_DASSERT( sPlanHashInstant->mReadColList != NULL );
    ((qlvInitInstantNode*)(sPlanHashInstant->mCommon.mInitNode))->mReadColList =
        sPlanHashInstant->mReadColList;
    ((qlvInitInstantNode*)(sPlanHashInstant->mCommon.mInitNode))->mReadColCnt =
        sPlanHashInstant->mReadColList->mCount;


    /****************************************************************
     * Hash Filter 정보 구성
     ****************************************************************/

    /**
     * Hash 요소별 Expression 분류 
     */

    STL_TRY( qloGetKeyHash( aCodeOptParamInfo->mSQLStmt->mRetrySQL,
                            NULL,
                            (qlvInitNode*)(sPlanHashInstant->mCommon.mInitNode),
                            aCodeOptParamInfo->mSQLStmt->mBindContext,
                            sPlanHashInstant->mHashKeyFilterExpr,
                            sPlanHashInstant->mNonHashKeyFilterExpr,
                            & sHashScanInfo,
                            QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                            aEnv )
             == STL_SUCCESS );


    /**
     * Hash Table 정보 설정
     */
    
    sOptHashInstant->mHashTableAttr = sPlanHashInstant->mHashTableAttr;

    if( sOptHashInstant->mHashTableAttr.mUniquenessFlag == STL_TRUE )
    {
        STL_DASSERT( sHashScanInfo->mPhysicalTableFilterExprList != NULL );
        STL_DASSERT( ((qlvInitInstantNode*)(sPlanHashInstant->mCommon.mInitNode))->mRecColCnt == 0 );

        if( ( sHashScanInfo->mPhysicalTableFilterExprList->mCount == 0 ) &&
            ( sHashScanInfo->mLogicalTableFilter == NULL ) )
        {
            /* not exists read column */
            ((qlvInitInstantNode*)(sPlanHashInstant->mCommon.mInitNode))->mReadColCnt = 0;
        }
    }            


    /****************************************************************
     * HASH INSTANT ACCESS Optimization Node 정보 설정
     ****************************************************************/

    /**
     * Node Type
     */
    
    sOptHashInstant->mType = QLV_NODE_TYPE_HASH_INSTANT;


    /**
     * Row Block이 필요한지 여부
     */

    sOptHashInstant->mNeedRowBlock = sPlanHashInstant->mCommon.mNeedRowBlock;


    /**
     * Materialization
     */

    sOptHashInstant->mNeedRebuild = sPlanHashInstant->mNeedRebuild;


    /**
     * Scrollable
     */
    
    sOptHashInstant->mScrollable = sPlanHashInstant->mScrollable;


    /**
     * Child Node
     */
    
    sOptHashInstant->mChildNode = sOptChildNode;
    

    /**
     * Instant Table Node
     */
    
    sOptHashInstant->mInstantNode = (qlvInitInstantNode*)(sPlanHashInstant->mCommon.mInitNode);


    /**
     * Base Relation Node List
     */

    sOptHashInstant->mSrcRelationList = sSrcRelationList;


    /**
     * Skip Hit Record : 상위 노드에서 설정
     */

    sOptHashInstant->mIsSkipHitRecord = STL_FALSE;


    /**
     * Outer Column 관련
     */

    sOptHashInstant->mOuterColumnList = sPlanHashInstant->mOuterColumnList;


    /**
     * Total Filter Expression
     */

    sOptHashInstant->mTotalFilterExpr = sPlanHashInstant->mNonHashKeyFilterExpr;


    /**
     * Index Scan 관련
     */
    
    /* 사용하지 않음 */
    sOptHashInstant->mSortTableAttr.mTopDown      = STL_FALSE;
    sOptHashInstant->mSortTableAttr.mVolatile     = STL_FALSE;
    sOptHashInstant->mSortTableAttr.mLeafOnly     = STL_FALSE;
    sOptHashInstant->mSortTableAttr.mDistinct     = STL_FALSE;
    sOptHashInstant->mSortTableAttr.mNullExcluded = STL_FALSE;
    
    sOptHashInstant->mIndexScanInfo = NULL;


    /**
     * Hash Scan 관련
     */
    
    /* sOptHashInstant->mHashTableAttr = sPlanHashInstant->mHashTableAttr; */
    sOptHashInstant->mHashScanInfo = sHashScanInfo;


    /**
     * Aggregation 관련 : PUSH 할 수 없음
     */
    
    /* 사용하지 않음 */
    sOptHashInstant->mAggrFuncCnt       = 0;
    sOptHashInstant->mAggrDistFuncCnt   = 0;

    sOptHashInstant->mTotalAggrFuncList = NULL;
    sOptHashInstant->mAggrFuncList      = NULL;
    sOptHashInstant->mAggrDistFuncList  = NULL;

    
    /**
     * Nested Aggregation 관련 : PUSH 할 수 없음
     */
    
    /* 사용하지 않음 */
    sOptHashInstant->mNestedAggrFuncCnt       = 0;
    sOptHashInstant->mNestedAggrDistFuncCnt   = 0;

    sOptHashInstant->mNestedTotalAggrFuncList = NULL;
    sOptHashInstant->mNestedAggrFuncList      = NULL;
    sOptHashInstant->mNestedAggrDistFuncList  = NULL;
    

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
 * @brief HASH JOIN INSTANT ACCESS의 Optimization Node 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoCompleteHashJoinInstantAccess( qllDBCStmt            * aDBCStmt,
                                             qllStatement          * aSQLStmt,
                                             qllOptNodeList        * aOptNodeList,
                                             qllOptimizationNode   * aOptNode,
                                             qloInitExprList       * aQueryExprList,
                                             qllEnv                * aEnv )
{
    qlnoInstant        * sOptHashInstant     = NULL;

    knlExprStack       * sTableLogicalStack  = NULL;

    qlvRefExprItem     * sRefColumnItem      = NULL;
    qloDBType          * sExprDBType         = NULL;
    qlvInstantColDesc  * sColumnDesc         = NULL;
    qlvInstantColDesc  * sCurColumnDesc      = NULL;
    
    qlvInitExpression  * sCodeExpr           = NULL;
    knlExprEntry       * sExprEntry          = NULL;

    qloHashScanInfo    * sScanInfo           = NULL;
    qlvInitExpression  * sAndExpr            = NULL;

    knlExprStack      ** sColExprStackList   = NULL;
    knlExprStack       * sColExprStack       = NULL;
    knlExprStack         sTempColExprStack;

    stlInt32             sLoop               = 0;
    stlInt32             sIdx                = 0;
    stlInt32             sTotalColCnt        = 0;


    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_HASH_JOIN_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );    
    

    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/

    sOptHashInstant = (qlnoInstant *) aOptNode->mOptNode;

    STL_DASSERT( sOptHashInstant->mHashScanInfo != NULL );

    sScanInfo = sOptHashInstant->mHashScanInfo;

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
                   sOptHashInstant->mChildNode,
                   aQueryExprList,
                   aEnv );

    
    /****************************************************************
     * Column Expression Stack 설정
     ****************************************************************/

    /**
     * Column Expression Code Stack 공간 할당
     */

    sTotalColCnt = ( sOptHashInstant->mInstantNode->mKeyColCnt +
                     sOptHashInstant->mInstantNode->mRecColCnt );
    
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
     * Hash Key Column Expression Stack 설정 (for Insert Columns)
     */
    
    sIdx = 0;

    sOptHashInstant->mNeedEvalKeyCol = STL_FALSE;
    sRefColumnItem = sOptHashInstant->mInstantNode->mKeyColList->mHead;
    for( sLoop = 0 ; sLoop < sOptHashInstant->mInstantNode->mKeyColCnt ; sLoop++, sIdx++ )
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
            sOptHashInstant->mNeedEvalKeyCol = STL_TRUE;
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

    sOptHashInstant->mNeedEvalRecCol = STL_FALSE;
    sRefColumnItem = sOptHashInstant->mInstantNode->mRecColList->mHead;
    for( sLoop = 0 ; sLoop < sOptHashInstant->mInstantNode->mRecColCnt ; sLoop++, sIdx++ )
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

        /* Hash Join을 위한 Instant 구성시 Record Column에 대한 Expression은
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

    sOptHashInstant->mNeedEvalInsertCol =
        ( sOptHashInstant->mNeedEvalKeyCol || sOptHashInstant->mNeedEvalRecCol );

    
    /****************************************************************
     * Physical & Logical Filter 구분
     * Hash Filter 관련 설정
     ****************************************************************/

    /**
     * Physical Key Filter를 구성하는 Expression에 대한 DB Type 정보 설정
     *  <BR> Physical Key Filter를 위한 임시 Predicate 생성
     */

    STL_DASSERT( sScanInfo->mHashFilterExpr != NULL );


    /**
     * Prepare Hash Key Expression Stack 생성
     */

    STL_TRY( qloMakeRangeStack( aDBCStmt,
                                aSQLStmt,
                                aQueryExprList,
                                sScanInfo->mHashColCount,
                                STL_TRUE,
                                sScanInfo->mHashFilterExpr,
                                NULL,
                                & sOptHashInstant->mPrepareRangeStack,
                                aEnv )
             == STL_SUCCESS );


    /****************************************************************
     * Table Physical Filter 관련 설정
     ****************************************************************/
    
    /**
     * Prepare Table Filter Expression Stack 생성
     */
        
    STL_DASSERT( sScanInfo->mPhysicalTableFilterExprList != NULL );

    if( sScanInfo->mPhysicalTableFilterExprList->mCount == 0 )
    {
        sOptHashInstant->mPrepareFilterStack = NULL;
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
                                             & sOptHashInstant->mPrepareFilterStack,
                                             aEnv )
                 == STL_SUCCESS );
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
     * Hash Key Column Descriptor 설정
     */

    sIdx = 0;

    sRefColumnItem = sOptHashInstant->mInstantNode->mKeyColList->mHead;
    for( sLoop = 0 ; sLoop < sOptHashInstant->mInstantNode->mKeyColCnt ; sLoop++, sIdx++ )
    {
        STL_DASSERT( sRefColumnItem != NULL );
        STL_DASSERT( sIdx == *sRefColumnItem->mExprPtr->mExpr.mInnerColumn->mIdx );
        
        sCurColumnDesc = & sColumnDesc[ sIdx ];
        
        sCurColumnDesc->mIdx         = sIdx;
        
        sCurColumnDesc->mIsSortKey   = STL_TRUE;
        sCurColumnDesc->mKeyColFlags = DTL_KEYCOLUMN_EMPTY;   /* no use */

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

    sRefColumnItem = sOptHashInstant->mInstantNode->mRecColList->mHead;
    for( sLoop = 0 ; sLoop < sOptHashInstant->mInstantNode->mRecColCnt ; sLoop++, sIdx++ )
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
    
    sOptHashInstant->mColumnDesc   = sColumnDesc;
    sOptHashInstant->mColExprStack = sColExprStackList;

    
    /**
     * Predicate 관련
     */

    sOptHashInstant->mPredicate = NULL;

    
    /**
     * Table Filter 관련
     */

    sOptHashInstant->mTablePhysicalFilterExpr = NULL;
    sOptHashInstant->mTableLogicalFilterExpr  = sScanInfo->mLogicalTableFilter;
    sOptHashInstant->mTableLogicalStack       = sTableLogicalStack;
    sOptHashInstant->mPrepareKeyFilterStack   = NULL;


    /**
     * Index Key Filter 관련
     */
    
    sOptHashInstant->mIndexKeyLogicalStack = NULL;


    /**
     * Aggregation 관련
     */

    /* sOptSortInstant->mAggrOptInfoForFilter = NULL; */


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
 * @brief HASH JOIN INSTANT ACCESS의 All Expr 리스트 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoAddExprHashJoinInstantAccess( qllDBCStmt            * aDBCStmt,
                                            qllStatement          * aSQLStmt,
                                            qllOptNodeList        * aOptNodeList,
                                            qllOptimizationNode   * aOptNode,
                                            qloInitExprList       * aQueryExprList,
                                            qllEnv                * aEnv )
{
    stlInt32                  sLoop;
    qlnoInstant             * sOptHashInstant   = NULL;
    qlvRefExprItem          * sRefColumnItem    = NULL;
    qlvInitInstantNode      * sInstantNode      = NULL;
    qloHashScanInfo         * sScanInfo         = NULL;

    
    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_HASH_JOIN_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );    

    
    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/

    sOptHashInstant = (qlnoInstant *) aOptNode->mOptNode;
    sScanInfo = sOptHashInstant->mHashScanInfo;

    
    /****************************************************************
     * Child Node의 Add Expression 수행
     ****************************************************************/

    QLNO_ADD_EXPR( aDBCStmt,
                   aSQLStmt,
                   aOptNodeList,
                   sOptHashInstant->mChildNode,
                   aQueryExprList,
                   aEnv );
    

    /****************************************************************
     * Add Expr
     ****************************************************************/

    /**
     * Add Expr Hash Instant Table : Hash Key Column
     */

    sInstantNode = sOptHashInstant->mInstantNode;

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
     * Add Expr Hash Instant Table : Record Column
     */

    sInstantNode = sOptHashInstant->mInstantNode;

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
     * Add Expr Hash Instant Table Column
     * @remark Read Column은 Hash Key Column과 Record Column의 부분 집합이기 때문에
     *         Read Column의 Original Expresssion을 전체 Expression List에 추가할 필요 없다.
     */

    sInstantNode = sOptHashInstant->mInstantNode;

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
     * Add Expr Physical Hash Filter Expression
     */

    if( sScanInfo->mHashFilterExpr != NULL )
    {
        for( sLoop = 0; sLoop < sScanInfo->mHashColCount; sLoop++ )
        {
            sRefColumnItem = sScanInfo->mHashFilterExpr[sLoop]->mHead;
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


    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlno */
