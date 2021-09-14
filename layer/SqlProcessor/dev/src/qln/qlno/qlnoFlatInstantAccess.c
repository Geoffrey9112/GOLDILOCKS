/*******************************************************************************
 * qlnoFlatInstantAccess.c
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
 * @file qlnoFlatInstantAccess.c
 * @brief SQL Processor Layer Code Optimization Node - FLAT INSTANT ACCESS
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlno
 * @{
 */

#if 0
stlStatus qloCodeOptimizeFlatInstantAccess( qllDBCStmt              * aDBCStmt,
                                            qllStatement            * aSQLStmt,
                                            qllOptimizationNode     * aOptQueryNode,
                                            qllOptimizationNode     * aOptChildNode,
                                            qlvInitNode             * aInitNode,
                                            qlvInitExpression       * aWhereExpr,
                                            ellCursorScrollability    aScrollable,
                                            qloInitExprList         * aQueryExprList,
                                            qllOptNodeList          * aOptNodeList,
                                            qllOptimizationNode    ** aOptNode,
                                            qllEstimateCost         * aOptCost,
                                            qllEnv                  * aEnv )
{
    qlvInitInstantNode      * sIniFlatInstant  = NULL;
    qllOptimizationNode     * sOptNode         = NULL;
    qlnoInstant             * sOptFlatInstant  = NULL;
    
    qllEstimateCost         * sOptCost         = NULL;

    qllOptNodeList          * sSrcRelationList = NULL;
    

    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptQueryNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptChildNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitNode->mType == QLV_NODE_TYPE_FLAT_INSTANT,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptCost != NULL, QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Init Node 정보 획득
     ****************************************************************/

    sIniFlatInstant = (qlvInitInstantNode *) aInitNode;


    /****************************************************************
     * Optimization Node 생성
     ****************************************************************/

    /**
     * FLAT INSTANT ACCESS Optimization Node 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlnoInstant ),
                                (void **) & sOptFlatInstant,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Optimization Cost 공간 할당
     */

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( qllEstimateCost ),
                                (void **) & sOptCost,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sOptCost, 0x00, STL_SIZEOF( qllEstimateCost ) );


    /**
     * Common Optimization Node 생성
     */

    STL_TRY( qlnoCreateOptNode( aOptNodeList,
                                QLL_PLAN_NODE_FLAT_INSTANT_ACCESS_TYPE,
                                sOptFlatInstant,
                                aOptQueryNode,  /* Query Node */
                                sOptCost,
                                NULL,
                                & sOptNode,
                                QLL_CODE_PLAN( aDBCStmt ),
                                aEnv )
             == STL_SUCCESS );


    /**
     * Inner Column의 Idx 설정
     */

    STL_TRY( qlnoSetIdxInstant( sIniFlatInstant,
                                aEnv )
             == STL_SUCCESS );


    /****************************************************************
     * 연관된 Base Table 정보 구축 (QUERY SPEC의 FROM 절 분석)
     ****************************************************************/

    /**
     * Optimization Node List 생성
     */

    STL_TRY( qlnoCreateOptNodeList( & sSrcRelationList,
                                    QLL_CODE_PLAN( aDBCStmt ),
                                    aEnv )
             == STL_SUCCESS );


    /**
     * 연관된 Base Table List 구축
     */

    STL_TRY( qlnoGetRefBaseTableNodeList( aOptNodeList,
                                          aOptChildNode,
                                          sSrcRelationList,
                                          QLL_CODE_PLAN( aDBCStmt ),
                                          aEnv )
             == STL_SUCCESS );


    /****************************************************************
     * Read Column List 구성 : PUSH PROJECTION에 의해 변경 가능
     ****************************************************************/

    /**
     * Statement Expression List에 Record Column 추가
     */

    if( sIniFlatInstant->mReadColList->mCount > 0 )
    {
        STL_TRY( qlvAddExprListToRefExprList(
                     aQueryExprList->mStmtExprList->mColumnExprList,
                     sIniFlatInstant->mReadColList,
                     STL_FALSE,
                     QLL_CODE_PLAN( aDBCStmt ),
                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /* Do Nothing */
    }


    /****************************************************************
     * FLAT INSTANT ACCESS Optimization Node 정보 설정
     ****************************************************************/

    /**
     * Node Type
     */
    
    sOptFlatInstant->mType = QLV_NODE_TYPE_FLAT_INSTANT;


    /**
     * Scrollable
     */
    
    sOptFlatInstant->mScrollable = aScrollable;


    /**
     * Child Node
     */
    
    sOptFlatInstant->mChildNode = aOptChildNode;


    /**
     * Instant Table Node
     */
    
    sOptFlatInstant->mInstantNode = sIniFlatInstant;


    /**
     * Base Relation Node List
     */

    sOptFlatInstant->mSrcRelationList = sSrcRelationList;


    /**
     * Skip Hit Record
     */

    sOptFlatInstant->mIsSkipHitRecord = STL_FALSE;


    /**
     * Outer Column 관련
     */

    sOptFlatInstant->mOuterColumnList = sIniFlatInstant->mOuterColumnList;

    
    /**
     * Total Filter 관련
     */

    sOptFlatInstant->mTotalFilterExpr = aWhereExpr;


    /**
     * Index Scan 관련
     */

    /* 사용하지 않음 */
    sOptFlatInstant->mSortTableAttr.mTopDown      = STL_FALSE;
    sOptFlatInstant->mSortTableAttr.mVolatile     = STL_FALSE;
    sOptFlatInstant->mSortTableAttr.mLeafOnly     = STL_FALSE;
    sOptFlatInstant->mSortTableAttr.mDistinct     = STL_FALSE;
    sOptFlatInstant->mSortTableAttr.mNullExcluded = STL_FALSE;
    sOptFlatInstant->mIndexScanInfo = NULL;


    /**
     * Hash 관련
     */
    
    /* 사용하지 않음 */
    stlMemset( &sOptFlatInstant->mHashTableAttr, 0x00, STL_SIZEOF( smlIndexAttr ) );
    sOptFlatInstant->mHashScanInfo = NULL;


    /**
     * Aggregation 관련 : PUSH 할 수 없음
     */
    
    /* 사용하지 않음 */
    sOptFlatInstant->mAggrFuncCnt       = 0;
    sOptFlatInstant->mAggrDistFuncCnt   = 0;

    sOptFlatInstant->mTotalAggrFuncList = NULL;
    sOptFlatInstant->mAggrFuncList      = NULL;
    sOptFlatInstant->mAggrDistFuncList  = NULL;

    
    /**
     * Nested Aggregation 관련 : PUSH 할 수 없음
     */
    
    /* 사용하지 않음 */
    sOptFlatInstant->mNestedAggrFuncCnt       = 0;
    sOptFlatInstant->mNestedAggrDistFuncCnt   = 0;

    sOptFlatInstant->mNestedTotalAggrFuncList = NULL;
    sOptFlatInstant->mNestedAggrFuncList      = NULL;
    sOptFlatInstant->mNestedAggrDistFuncList  = NULL;


    /****************************************************************
     * OUTPUT 설정
     ****************************************************************/

    /**
     * 결과 Common Optimization Node 설정
     */

    *aOptNode = sOptNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
#endif



#if 0
/**
 * @brief FLAT INSTANT ACCESS의 Optimization Node 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoCompleteFlatInstantAccess( qllDBCStmt            * aDBCStmt,
                                         qllStatement          * aSQLStmt,
                                         qllOptNodeList        * aOptNodeList,
                                         qllOptimizationNode   * aOptNode,
                                         qloInitExprList       * aQueryExprList,
                                         qllEnv                * aEnv )
{
    qlnoInstant        * sOptFlatInstant     = NULL;

    qlvRefExprItem     * sRefColumnItem      = NULL;
    qloDBType          * sExprDBType         = NULL;
    qlvInstantColDesc  * sColumnDesc         = NULL;
    qlvInstantColDesc  * sCurColumnDesc      = NULL;
    
    qlvInitExpression  * sPhysicalFilterExpr = NULL;
    qlvInitExpression  * sLogicalFilterExpr  = NULL;

    knlExprStack       * sPhysicalFilterStack = NULL;    
    knlExprStack       * sLogicalFilterStack  = NULL;
    knlPredicateList   * sLogicalFilterPred   = NULL;
    
    qlvInitExpression  * sCodeExpr           = NULL;
    knlExprEntry       * sExprEntry          = NULL;
    
    knlExprStack       * sTempConstExprStack = NULL;
    
    knlExprStack      ** sColExprStackList   = NULL;
    knlExprStack       * sColExprStack       = NULL;
    knlExprStack         sTempColExprStack;

    stlInt32             sLoop               = 0;


    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_FLAT_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );    
    

    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/

    sOptFlatInstant = (qlnoInstant *) aOptNode->mOptNode;

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
                   sOptFlatInstant->mChildNode,
                   aQueryExprList,
                   aEnv );


    /****************************************************************
     * Column Expression Stack 설정
     ****************************************************************/

    /**
     * Column Expression Code Stack 공간 할당
     */

    STL_DASSERT( sOptFlatInstant->mInstantNode->mReadColCnt > 0 );

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( knlExprStack * ) * sOptFlatInstant->mInstantNode->mReadColCnt,
                                (void **) & sColExprStackList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( knlExprStack ) * sOptFlatInstant->mInstantNode->mReadColCnt,
                                (void **) & sColExprStack,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    
    sRefColumnItem = sOptFlatInstant->mInstantNode->mReadColList->mHead;

    sOptFlatInstant->mNeedEvalKeyCol = STL_FALSE;
    sOptFlatInstant->mNeedEvalRecCol = STL_FALSE;
    for( sLoop = 0 ; sLoop < sOptFlatInstant->mInstantNode->mReadColCnt ; sLoop++ )
    {
        /**
         * Logical Expression Stack 생성
         */

        sCodeExpr = sRefColumnItem->mExprPtr->mExpr.mInnerColumn->mOrgExpr;

        STL_TRY( knlExprInit( & sColExprStack[ sLoop ],
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
                                            & sColExprStack[ sLoop ],
                                            aQueryExprList->mStmtExprList->mConstExprStack,
                                            aQueryExprList->mStmtExprList->mTotalExprInfo,
                                            & sExprEntry,
                                            QLL_CODE_PLAN( aDBCStmt ),
                                            aEnv )
                 == STL_SUCCESS );


        if( ( sColExprStack[ sLoop ].mEntryCount == 1 ) &&
            ( sColExprStack[ sLoop ].mEntries[ 1 ].mExprType == KNL_EXPR_TYPE_VALUE ) )
        {
            sColExprStackList[ sLoop ] = NULL;
        }
        else
        {
            sColExprStackList[ sLoop ] = & sColExprStack[ sLoop ];
            sOptFlatInstant->mNeedEvalRecCol = STL_TRUE;
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

    sOptFlatInstant->mNeedEvalInsertCol = sOptFlatInstant->mNeedEvalRecCol;


    /****************************************************************
     * Physical & Logical Filter 구분
     ****************************************************************/
    
    /**
     * Logical Filter 와 Physical Filter 분리
     */

    if( sOptFlatInstant->mTotalFilterExpr == NULL )
    {
        sLogicalFilterExpr  = NULL;
        sPhysicalFilterExpr = NULL;
        sLogicalFilterPred  = NULL;
    }
    else 
    {   
        /**
         * Total Filter를 위한 임시 Constant Expression Stack 공간 할당
         * @remark Candidate memory manager 사용
         */

        STL_TRY( knlAllocRegionMem( & QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( knlExprStack ),
                                    (void **) & sTempConstExprStack,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    
    
        /**
         * Total Filter를 위한 임시 Constant Expression Stack 구성
         * @remark Candidate memory manager 사용
         */

        STL_TRY( knlExprInit( sTempConstExprStack,
                              sOptFlatInstant->mTotalFilterExpr->mIncludeExprCnt,
                              & QLL_CANDIDATE_MEM( aEnv ),
                              KNL_ENV( aEnv ) )
                 == STL_SUCCESS );

        /*******************************************
         * Physical & Logical Filter 구분
         *******************************************/
        
        /**
         * Logical Filter 와 Physical Filter Expression 분리
         */

        STL_TRY( qloClassifyFilterExpr( aSQLStmt->mRetrySQL,
                                        sOptFlatInstant->mTotalFilterExpr,
                                        (qlvInitNode *) sOptFlatInstant->mInstantNode,
                                        QLL_CODE_PLAN( aDBCStmt ),
                                        aSQLStmt->mBindContext,
                                        & sLogicalFilterExpr,
                                        & sPhysicalFilterExpr,
                                        aEnv )
                 == STL_SUCCESS );

        /*************************
         * Table Physical Filter 
         *************************/

        if( sPhysicalFilterExpr == NULL )
        {
            /* Do Nothing */
        }
        else
        {
            /**
             * Physical Filter를 위한 임시 Constant Expression Stack 공간 할당
             * @remark Candidate memory manager 사용
             */

            STL_TRY( knlAllocRegionMem( & QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( knlExprStack ),
                                        (void **) & sTempConstExprStack,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

    
            /**
             * Physical Filter를 위한 임시 Constant Expression Stack 구성
             * @remark Candidate memory manager 사용
             */

            STL_TRY( knlExprInit( sTempConstExprStack,
                                  sPhysicalFilterExpr->mIncludeExprCnt,
                                  & QLL_CANDIDATE_MEM( aEnv ),
                                  KNL_ENV( aEnv ) )
                     == STL_SUCCESS );


            /**
             * Physical Filter Predicate 생성
             */

            /**
             * Physical Expression Code Stack 공간 할당
             */

            STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                        STL_SIZEOF( knlExprStack ),
                                        (void **) & sPhysicalFilterStack,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );


            /**
             * Physical Expression Stack 생성
             */

            sCodeExpr = sPhysicalFilterExpr;

            STL_TRY( knlExprInit( sPhysicalFilterStack,
                                  sCodeExpr->mIncludeExprCnt,
                                  QLL_CODE_PLAN( aDBCStmt ),
                                  KNL_ENV( aEnv ) )
                     == STL_SUCCESS );


            /**
             * Physical Expression Stack 구성
             */

            STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                                sCodeExpr,
                                                aSQLStmt->mBindContext,
                                                sPhysicalFilterStack,
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
        }
        
        /*************************
         * Table Logical Filter 
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

    if( sOptFlatInstant->mInstantNode->mReadColCnt > 0 )
    {
        STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                    STL_SIZEOF( qlvInstantColDesc ) * sOptFlatInstant->mInstantNode->mReadColCnt,
                                    (void **) & sColumnDesc,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        sColumnDesc = NULL;
    }
    
    sRefColumnItem = sOptFlatInstant->mInstantNode->mReadColList->mHead;

    for( sLoop = 0 ; sLoop < sOptFlatInstant->mInstantNode->mReadColCnt ; sLoop++ )
    {
        STL_DASSERT( sRefColumnItem != NULL );
        STL_DASSERT( sLoop == *sRefColumnItem->mExprPtr->mExpr.mInnerColumn->mIdx );
        
        sCurColumnDesc = & sColumnDesc[ sLoop ];
        
        sCurColumnDesc->mIdx         = sLoop;
        
        sCurColumnDesc->mIsSortKey   = STL_FALSE;
        sCurColumnDesc->mKeyColFlags = DTL_KEYCOLUMN_EMPTY;

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
    
    sOptFlatInstant->mColumnDesc   = sColumnDesc;
    sOptFlatInstant->mColExprStack = sColExprStackList;

    
    /**
     * Predicate 관련
     */

    sOptFlatInstant->mPredicate = sLogicalFilterPred;


    /**
     * Table Filter 관련
     */

    sOptFlatInstant->mTablePhysicalFilterExpr = sPhysicalFilterExpr;
    sOptFlatInstant->mTableLogicalFilterExpr  = sLogicalFilterExpr;
    sOptFlatInstant->mTableLogicalStack       = sLogicalFilterStack;


    /**
     * Index Key Filter 관련
     */
    
    sOptFlatInstant->mIndexKeyLogicalStack = NULL;


    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_WHERE_CLAUSE )
    {
        (void)stlPopError( QLL_ERROR_STACK(aEnv) );
        
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_WHERE_CLAUSE,
                      qlgMakeErrPosString( aSQLStmt->mRetrySQL,
                                           sCodeExpr->mPosition,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}
#endif


#if 0
/**
 * @brief FLAT INSTANT ACCESS의 All Expr 리스트 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoAddExprFlatInstantAccess( qllDBCStmt            * aDBCStmt,
                                        qllStatement          * aSQLStmt,
                                        qllOptNodeList        * aOptNodeList,
                                        qllOptimizationNode   * aOptNode,
                                        qloInitExprList       * aQueryExprList,
                                        qllEnv                * aEnv )
{
    qlnoInstant         * sOptFlatInstant = NULL;
    qlvRefExprItem      * sRefColumnItem  = NULL;
    qlvInitInstantNode  * sInstantNode    = NULL;
    

    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_FLAT_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );    
    

    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/

    sOptFlatInstant = (qlnoInstant *) aOptNode->mOptNode;

    
    /****************************************************************
     * Child Node의 Add Expression 수행
     ****************************************************************/

    QLNO_ADD_EXPR( aDBCStmt,
                   aSQLStmt,
                   aOptNodeList,
                   sOptFlatInstant->mChildNode,
                   aQueryExprList,
                   aEnv );


    /****************************************************************
     * Add Expr
     ****************************************************************/

    /**
     * Add Expr Flat Instant Table Column : Record Column
     */

    sInstantNode = sOptFlatInstant->mInstantNode;

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
     * Add Expr Flat Instant Table Column : Read Column
     */

    sInstantNode = sOptFlatInstant->mInstantNode;

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
     * Add Expr Where Clause Expression
     */
        
    if( sOptFlatInstant->mTotalFilterExpr != NULL )
    {
        STL_TRY( qloAddExpr( sOptFlatInstant->mTotalFilterExpr,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN( aDBCStmt ),
                             aEnv )
                 == STL_SUCCESS );
    }


    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}
#endif


/** @} qlno */
