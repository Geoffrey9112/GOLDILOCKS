/*******************************************************************************
 * qlnoQuerySpec.c
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
 * @file qlnoQuerySpec.c
 * @brief SQL Processor Layer Code Optimization Node - QUERY SPEC
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

stlStatus qlnoCodeOptimizeQuerySpec( qloCodeOptParamInfo    * aCodeOptParamInfo,
                                     qllEnv                 * aEnv )
{
    qlncPlanQuerySpec   * sPlanQuerySpec    = NULL;
    qlnoQuerySpec       * sOptQuerySpec     = NULL;
    qllEstimateCost     * sOptCost          = NULL;
    qllOptimizationNode * sOptNode          = NULL;
    qloInitExprList     * sQueryExprList    = NULL;

    qloInitExprList     * sBackupQueryExprList  = NULL;

    qllOptimizationNode * sOptFromNode      = NULL;


    qllOptimizationNode   * sOptChildNode   = NULL; 

    qlvInitExpression     * sIniOffsetExpr  = NULL;
    qlvInitExpression     * sIniLimitExpr   = NULL;
    
    stlInt64                sSkipCnt        = 0;
    stlInt64                sFetchCnt       = 0;

    qllOptimizationNode   * sOptAggrNode    = NULL;
       
    qlvAggregation        * sIniAggrNode    = NULL;


    /*
     * Paramter Validation
     */

    QLO_CODE_OPT_DEFAULT_PARAM_VALIDATE( aCodeOptParamInfo, aEnv );
    STL_PARAM_VALIDATE( aCodeOptParamInfo->mCandPlan->mPlanType == QLNC_PLAN_TYPE_QUERY_SPEC,
                        QLL_ERROR_STACK(aEnv) );

    
    /****************************************************************
     * Cand Node 정보 획득
     ****************************************************************/

    /**
     * 기본 정보 획득
     */

    sPlanQuerySpec = (qlncPlanQuerySpec *) aCodeOptParamInfo->mCandPlan;
 
    /* Backup Code Optimize Parameter Info */
    sBackupQueryExprList = aCodeOptParamInfo->mQueryExprList;

    
    /****************************************************************
     * Optimization Node 생성
     ****************************************************************/

    /**
     * QUERY SPEC Optimization Node 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                STL_SIZEOF( qlnoQuerySpec ),
                                (void **) & sOptQuerySpec,
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
                                QLL_PLAN_NODE_QUERY_SPEC_TYPE,
                                sOptQuerySpec,
                                NULL,  /* Query Node */
                                sOptCost,
                                NULL,
                                & sOptNode,
                                QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                aEnv )
             == STL_SUCCESS );


    /****************************************************************
     * Expression List 구성
     ****************************************************************/

    /**
     * Query 단위 Expression List 설정
     */
    
    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                STL_SIZEOF( qloInitExprList ),
                                (void **) & sQueryExprList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sQueryExprList->mInitExprList = sPlanQuerySpec->mInitExprList;
    sQueryExprList->mStmtExprList = aCodeOptParamInfo->mQueryExprList->mStmtExprList;


    /****************************************************************
     * Child Node Optimization
     ****************************************************************/

    aCodeOptParamInfo->mQueryExprList = sQueryExprList;
    aCodeOptParamInfo->mOptQueryNode = sOptNode;
    aCodeOptParamInfo->mCandPlan = sPlanQuerySpec->mChildPlanNode;

    QLNO_CODE_OPTIMIZE( aCodeOptParamInfo, aEnv );

    sOptFromNode = aCodeOptParamInfo->mOptNode;


    /****************************************************************
     * Rewrite Expression
     ****************************************************************/

    /**
     * Target Column List에 Expression 추가
     */

    sOptQuerySpec->mTargetCount   = sPlanQuerySpec->mTargetCount;
    sOptQuerySpec->mTargetColList = sPlanQuerySpec->mTargetColList;
    

    /**
     * @todo WINDOW 절 rewrite
     */


    /**
     * @todo WITH 절 rewrite
     */

    
    /**
     * OFFSET 절 rewrite
     */

    if( sPlanQuerySpec->mOffset == NULL )
    {
        sSkipCnt = 0;
    }
    else
    {
        sIniOffsetExpr = sPlanQuerySpec->mOffset;

        if( sIniOffsetExpr->mType == QLV_EXPR_TYPE_VALUE )
        {
            /* validation 과정에서 이미 평가됨 */
            STL_ASSERT( dtlGetIntegerFromString(
                            sIniOffsetExpr->mExpr.mValue->mData.mString,
                            stlStrlen( sIniOffsetExpr->mExpr.mValue->mData.mString ),
                            & sSkipCnt,
                            QLL_ERROR_STACK( aEnv ) )
                        == STL_SUCCESS );
        }
        else
        {
            STL_DASSERT( sIniOffsetExpr->mType == QLV_EXPR_TYPE_BIND_PARAM );
            sSkipCnt = 0;
        }
    }
        
    
    /**
     * LIMIT 절 rewrite
     */

    if( sPlanQuerySpec->mLimit == NULL )
    {
        sFetchCnt = QLL_FETCH_COUNT_MAX;
    }
    else
    {
        sIniLimitExpr = sPlanQuerySpec->mLimit;

        if( sIniLimitExpr->mType == QLV_EXPR_TYPE_VALUE )
        {
            /* validation 과정에서 이미 평가됨 */
            STL_ASSERT( dtlGetIntegerFromString(
                            sIniLimitExpr->mExpr.mValue->mData.mString,
                            stlStrlen( sIniLimitExpr->mExpr.mValue->mData.mString ),
                            & sFetchCnt,
                            QLL_ERROR_STACK( aEnv ) )
                        == STL_SUCCESS );
        }
        else
        {
            STL_DASSERT( sIniLimitExpr->mType == QLV_EXPR_TYPE_BIND_PARAM );
            sFetchCnt = QLL_FETCH_COUNT_MAX;
        }
    }


    /****************************************************************
     * Query Optimization
     ****************************************************************/

    /**
     * Child Node 설정
     */
    
    sOptChildNode = sOptFromNode;
    

    /****************************************************************
     * Optimization Node 구성
     ****************************************************************/

    /**
     * Aggregation Node 구성
     */

    if( sPlanQuerySpec->mCommon.mAggrExprList.mCount > 0 )
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

        sIniAggrNode->mAggrExprList->mCount = sPlanQuerySpec->mCommon.mAggrExprList.mCount;
        sIniAggrNode->mAggrExprList->mHead = sPlanQuerySpec->mCommon.mAggrExprList.mHead;
        sIniAggrNode->mAggrExprList->mTail = sPlanQuerySpec->mCommon.mAggrExprList.mTail;

        STL_TRY( qloCodeOptimizeHashAggregation( aCodeOptParamInfo->mDBCStmt,
                                                 aCodeOptParamInfo->mSQLStmt,
                                                 sOptNode,
                                                 sOptChildNode,
                                                 sIniAggrNode,
                                                 sQueryExprList,
                                                 aCodeOptParamInfo->mOptNodeList,
                                                 & sOptAggrNode,
                                                 sOptCost,
                                                 aEnv )
                 == STL_SUCCESS );            


        sOptChildNode = sOptAggrNode;
    }

    
    /****************************************************************
     * QUERY SPEC Optimization Node 정보 설정
     ****************************************************************/

    /**
     * Child Optimization Node Idx
     */
    
    sOptQuerySpec->mChildNode = sOptChildNode;


    /**
     * Row Block이 필요한지 여부
     */

    sOptQuerySpec->mNeedRowBlock = sPlanQuerySpec->mCommon.mNeedRowBlock;

    
    /**
     * Target Count
     */
    
    /* sOptQuerySpec->mTargetCount = sIniQuerySpec->mTargetCount; */

    
    /**
     * Target Column List
     */

    /* sOptQuerySpec->mTargetColList = sTargetColList; */


    /**
     * Target Column List
     */

    sOptQuerySpec->mTargets = ((qlvInitQuerySpecNode *)sPlanQuerySpec->mCommon.mInitNode )->mTargets;


    /**
     * Query 단위 Expression 정보
     */

    sOptQuerySpec->mQueryExprList = sQueryExprList;


    /**
     * WITH Clause
     * @todo 스펙 정의되지 않음
     */

    sOptQuerySpec->mWith = NULL;

    
    /**
     * Skip(Offset) Count
     */

    sOptQuerySpec->mSkipCnt = sSkipCnt;

    
    /**
     * Fetch Count
     */

    sOptQuerySpec->mFetchCnt = sFetchCnt;


    /**
     * Skip(Offset) Expression
     */

    sOptQuerySpec->mResultSkip = sPlanQuerySpec->mOffset;

    
    /**
     * Limit(Fetch) Expression
     */

    sOptQuerySpec->mResultLimit = sPlanQuerySpec->mLimit;


    /* Backup된 Code Optimize Parameter Info 원복 */
    aCodeOptParamInfo->mQueryExprList = sBackupQueryExprList;


    /****************************************************************
     * OUTPUT 설정
     ****************************************************************/

    aCodeOptParamInfo->mOptNode = sOptNode;
    

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/****************************************************************************
 * Complete Building Optimization Node Functions
 ***************************************************************************/

/**
 * @brief QUERY SPEC의 Optimization Node 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoCompleteQuerySpec( qllDBCStmt            * aDBCStmt,
                                 qllStatement          * aSQLStmt,
                                 qllOptNodeList        * aOptNodeList,
                                 qllOptimizationNode   * aOptNode,
                                 qloInitExprList       * aQueryExprList,
                                 qllEnv                * aEnv )
{
    qlnoQuerySpec        * sOptQuerySpec    = NULL;

    qlvInitExpression    * sTargetExpr      = NULL;
    knlExprStack         * sTargetExprStack = NULL;
    qlvRefExprItem       * sTargetItem      = NULL;
    stlInt32               sTargetCount     = 0;
        
    knlExprEntry         * sExprEntry       = NULL;
    qloDBType            * sExprDBType      = NULL;
    
    stlInt32               sLoop            = 0;

    qloDBType            * sDBTypes         = NULL;
    qlvInitTarget        * sTargets         = NULL;
    

    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_QUERY_SPEC_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );    


    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/
    
    sOptQuerySpec = (qlnoQuerySpec *) aOptNode->mOptNode;

    sDBTypes = aQueryExprList->mStmtExprList->mTotalExprInfo->mExprDBType;
    
    
    /****************************************************************
     * 하위 node에 대한 Complete Node 수행
     ****************************************************************/

    QLNO_COMPLETE( aDBCStmt,
                   aSQLStmt,
                   aOptNodeList,
                   sOptQuerySpec->mChildNode,
                   aQueryExprList,
                   aEnv );


    /****************************************************************
     * Target에 대한 Expression Stack 구성
     ****************************************************************/

    /**
     * Target절 Expression Code Stack 공간 할당
     */

    STL_DASSERT( sOptQuerySpec->mTargetCount > 0 );
    
    sTargetCount = sOptQuerySpec->mTargetCount;
    
    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( knlExprStack ) * sTargetCount,
                                (void **) & sTargetExprStack,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    
    /**
     * Target절 Expression Code Stack 생성
     */

    STL_DASSERT( sOptQuerySpec->mTargetCount == sOptQuerySpec->mTargetColList->mCount );

    sTargetItem = sOptQuerySpec->mTargetColList->mHead;
    sTargets    = sOptQuerySpec->mTargets;
    
    for( sLoop = 0 ; sLoop < sTargetCount ; sLoop++)
    {
        sTargetExpr = sTargetItem->mExprPtr;

        STL_DASSERT( sTargetExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN );

        sTargetExpr = sTargetExpr->mExpr.mInnerColumn->mOrgExpr;

        
        /**
         * Target절 Expression Stack 생성
         */

        STL_TRY( knlExprInit( & sTargetExprStack[ sLoop ],
                              sTargetExpr->mIncludeExprCnt,
                              QLL_CODE_PLAN( aDBCStmt ),
                              KNL_ENV( aEnv ) )
                 == STL_SUCCESS );

        
        /**
         * Target절 Expression Stack 구성
         */
            
        STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                            sTargetExpr,
                                            aSQLStmt->mBindContext,
                                            & sTargetExprStack[ sLoop ],
                                            aQueryExprList->mStmtExprList->mConstExprStack,
                                            aQueryExprList->mStmtExprList->mTotalExprInfo,
                                            & sExprEntry,
                                            QLL_CODE_PLAN( aDBCStmt ),
                                            aEnv )
                 == STL_SUCCESS );
        
        sExprDBType = & sDBTypes[ sTargetExpr->mOffset ];
        STL_TRY( qloSetExprDBType( sTargetItem->mExprPtr,
                                   aQueryExprList->mStmtExprList->mTotalExprInfo,
                                   sExprDBType->mDBType,
                                   sExprDBType->mArgNum1,
                                   sExprDBType->mArgNum2,
                                   sExprDBType->mStringLengthUnit,
                                   sExprDBType->mIntervalIndicator,
                                   aEnv )
                 == STL_SUCCESS );

        STL_TRY( qloCastExprDBType( sTargetItem->mExprPtr,
                                    aQueryExprList->mStmtExprList->mTotalExprInfo,
                                    sTargets->mDataTypeInfo.mDataType,
                                    sTargets->mDataTypeInfo.mArgNum1,
                                    sTargets->mDataTypeInfo.mArgNum2,
                                    sTargets->mDataTypeInfo.mStringLengthUnit,
                                    sTargets->mDataTypeInfo.mIntervalIndicator,
                                    STL_FALSE,
                                    STL_FALSE,
                                    aEnv )
                 == STL_SUCCESS );                                    

        sTargets++;
        sTargetItem = sTargetItem->mNext;
    }

    
    /****************************************************************
     * Node 정보 설정
     ****************************************************************/

    /**
     * Target 절 Expression Stack Array
     */
    
    sOptQuerySpec->mTargetsCodeStack = sTargetExprStack;

    
    return STL_SUCCESS;
 
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief QUERY SPEC의 All Expr리스트 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoAddExprQuerySpec( qllDBCStmt            * aDBCStmt,
                                qllStatement          * aSQLStmt,
                                qllOptNodeList        * aOptNodeList,
                                qllOptimizationNode   * aOptNode,
                                qloInitExprList       * aQueryExprList,
                                qllEnv                * aEnv )
{
    qlnoQuerySpec        * sOptQuerySpec  = NULL;
    qlvRefExprItem       * sRefColumnItem = NULL;

        
    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_QUERY_SPEC_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );    


    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/
    
    sOptQuerySpec = (qlnoQuerySpec *) aOptNode->mOptNode;
    

    /****************************************************************
     * Child Node의 Add Expression 수행
     ****************************************************************/

    QLNO_ADD_EXPR( aDBCStmt,
                   aSQLStmt,
                   aOptNodeList,
                   sOptQuerySpec->mChildNode,
                   aQueryExprList,
                   aEnv );


    /****************************************************************
     * Add Expr
     ****************************************************************/

    /**
     * Add Expr Target clause Expression
     * ------------------------
     *  Target의 경우 Table Node에서 읽은 Column을
     *  Inner Column으로 한번더 감싸고 있기때문에
     *  OrgExpr도 All Expression List에 넣어야 한다.
     */

    sRefColumnItem = sOptQuerySpec->mTargetColList->mHead;
    while( sRefColumnItem != NULL )
    {
        /**
         * Orginal Expression을 All Expression List에 추가
         */

        STL_TRY( qloAddExpr( sRefColumnItem->mExprPtr->mExpr.mInnerColumn->mOrgExpr,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN( aDBCStmt ),
                             aEnv )
                 == STL_SUCCESS );

        
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
    

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}
   

/** @} qlno */





