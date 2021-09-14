/*******************************************************************************
 * qlnoQuerySet.c
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
 * @file qlnoQuerySet.c
 * @brief SQL Processor Layer Code Optimization Node - QUERY SET
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
 * @brief Query Set에 대한 Code Optimize를 수행한다.
 * @param[in]   aCodeOptParamInfo   Code Optimize Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlnoCodeOptimizeQuerySet( qloCodeOptParamInfo * aCodeOptParamInfo,
                                    qllEnv              * aEnv )
{
    qlncPlanQuerySet    * sPlanQuerySet     = NULL;
    qlnoQuerySet        * sCodeQuerySet     = NULL;

    qllOptimizationNode * sQuerySetNode     = NULL;
    qllEstimateCost     * sOptCost          = NULL;
 
    qlvInitExpression   * sInitOffsetExpr   = NULL;
    qlvInitExpression   * sInitLimitExpr    = NULL;
    
    stlInt64              sSkipCnt          = 0;
    stlInt64              sFetchCnt         = 0;

    
    /*
     * Paramter Validation
     */

    QLO_CODE_OPT_DEFAULT_PARAM_VALIDATE( aCodeOptParamInfo, aEnv );


    /****************************************************************
     * Query Set Plan 정보 획득
     ****************************************************************/

    sPlanQuerySet = (qlncPlanQuerySet*)(aCodeOptParamInfo->mCandPlan);


    /****************************************************************
     * Optimization Node 생성
     ****************************************************************/

    /**
     * QUERY SET Optimization Node 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                STL_SIZEOF( qlnoQuerySet ),
                                (void **) & sCodeQuerySet,
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
                                QLL_PLAN_NODE_QUERY_SET_TYPE,
                                sCodeQuerySet,
                                NULL,  /* Query Node */
                                sOptCost,
                                NULL,
                                & sQuerySetNode,
                                QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                aEnv )
             == STL_SUCCESS );


    /****************************************************************
     * Target List 및 Column List 구성
     ****************************************************************/

    sCodeQuerySet->mIsRootSet = sPlanQuerySet->mIsRootSet;

    sCodeQuerySet->mTargetCount = sPlanQuerySet->mTargetCount;
    sCodeQuerySet->mTargetList = sPlanQuerySet->mTargetList;
    sCodeQuerySet->mSetTargets = sPlanQuerySet->mSetTargets;
    sCodeQuerySet->mColumnList = sPlanQuerySet->mColumnList;


    /**
     * Statement Expression List에 Record Column 추가
     */
    
    STL_TRY( qlvAddExprListToRefExprList( aCodeOptParamInfo->mQueryExprList->mStmtExprList->mColumnExprList,
                                          sCodeQuerySet->mColumnList,
                                          STL_FALSE,
                                          QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                          aEnv )
             == STL_SUCCESS );


    /****************************************************************
     * Child Node Optimization
     ****************************************************************/

    if( sCodeQuerySet->mIsRootSet == STL_TRUE )
    {
        aCodeOptParamInfo->mRootQuerySetIdx = sQuerySetNode->mIdx;
        aCodeOptParamInfo->mRootQuerySet = (void*)sCodeQuerySet;
    }

    aCodeOptParamInfo->mCandPlan = sPlanQuerySet->mChildPlanNode;

    QLNO_CODE_OPTIMIZE( aCodeOptParamInfo, aEnv );

    sCodeQuerySet->mChildNode = aCodeOptParamInfo->mOptNode;

    
    /****************************************************************
     * Offset/Limit 의 Code Optimization
     ****************************************************************/
    
    /**
     * OFFSET 절 rewrite
     */

    if( sPlanQuerySet->mOffset == NULL )
    {
        sSkipCnt = 0;
    }
    else
    {
        sInitOffsetExpr = sPlanQuerySet->mOffset;

        if( sInitOffsetExpr->mType == QLV_EXPR_TYPE_VALUE )
        {
            /* validation 과정에서 이미 평가됨 */
            STL_ASSERT( dtlGetIntegerFromString(
                            sInitOffsetExpr->mExpr.mValue->mData.mString,
                            stlStrlen( sInitOffsetExpr->mExpr.mValue->mData.mString ),
                            & sSkipCnt,
                            QLL_ERROR_STACK( aEnv ) )
                        == STL_SUCCESS );
        }
        else
        {
            STL_DASSERT( sInitOffsetExpr->mType == QLV_EXPR_TYPE_BIND_PARAM );
            sSkipCnt = 0;
        }
    }
        
    /**
     * LIMIT 절 rewrite
     */

    if( sPlanQuerySet->mLimit == NULL )
    {
        sFetchCnt = QLL_FETCH_COUNT_MAX;
    }
    else
    {
        sInitLimitExpr = sPlanQuerySet->mLimit;

        if( sInitLimitExpr->mType == QLV_EXPR_TYPE_VALUE )
        {
            /* validation 과정에서 이미 평가됨 */
            STL_ASSERT( dtlGetIntegerFromString(
                            sInitLimitExpr->mExpr.mValue->mData.mString,
                            stlStrlen( sInitLimitExpr->mExpr.mValue->mData.mString ),
                            & sFetchCnt,
                            QLL_ERROR_STACK( aEnv ) )
                        == STL_SUCCESS );
        }
        else
        {
            STL_DASSERT( sInitLimitExpr->mType == QLV_EXPR_TYPE_BIND_PARAM );
            sFetchCnt = QLL_FETCH_COUNT_MAX;
        }
    }

    sCodeQuerySet->mSkipCnt     = sSkipCnt;
    sCodeQuerySet->mFetchCnt    = sFetchCnt;
    sCodeQuerySet->mResultSkip  = sPlanQuerySet->mOffset;
    sCodeQuerySet->mResultLimit = sPlanQuerySet->mLimit;
    sCodeQuerySet->mNeedRebuild = sPlanQuerySet->mNeedRebuild;


    /****************************************************************
     * Row Block이 필요한지 여부
     ****************************************************************/

    sCodeQuerySet->mNeedRowBlock = sPlanQuerySet->mCommon.mNeedRowBlock;


    /****************************************************************
     * OUTPUT 설정
     ****************************************************************/

    aCodeOptParamInfo->mOptNode = sQuerySetNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Set OP에 대한 Code Optimize를 수행한다.
 * @param[in]   aCodeOptParamInfo   Code Optimize Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlnoCodeOptimizeSetOP( qloCodeOptParamInfo    * aCodeOptParamInfo,
                                 qllEnv                 * aEnv )
{
    qlncPlanSetOP       * sPlanSetOP    = NULL;
    qlnoSetOP           * sCodeSetOP    = NULL;
    qllPlanNodeType       sPlanType     = QLL_PLAN_NODE_TYPE_MAX;    
    
    qllOptimizationNode * sSetOPNode    = NULL;
    qllEstimateCost     * sOptCost      = NULL;
    stlInt32              i;
    

    /*
     * Paramter Validation
     */

    QLO_CODE_OPT_DEFAULT_PARAM_VALIDATE( aCodeOptParamInfo, aEnv );


    /****************************************************************
     * Set OP Plan 정보 획득
     ****************************************************************/

    sPlanSetOP = (qlncPlanSetOP*)(aCodeOptParamInfo->mCandPlan);


    /****************************************************************
     * Optimization Node 생성
     ****************************************************************/

    switch ( sPlanSetOP->mSetType )
    {
        case QLV_SET_TYPE_UNION:
            {
                if ( sPlanSetOP->mSetOption == QLV_SET_OPTION_ALL )
                {
                    sPlanType = QLL_PLAN_NODE_UNION_ALL_TYPE;
                }
                else
                {
                    sPlanType = QLL_PLAN_NODE_UNION_DISTINCT_TYPE;
                }
                break;
            }
        case QLV_SET_TYPE_EXCEPT:
            {
                if ( sPlanSetOP->mSetOption == QLV_SET_OPTION_ALL )
                {
                    sPlanType = QLL_PLAN_NODE_EXCEPT_ALL_TYPE;
                }
                else
                {
                    sPlanType = QLL_PLAN_NODE_EXCEPT_DISTINCT_TYPE;
                }
                break;
            }
        case QLV_SET_TYPE_INTERSECT:
            {
                if ( sPlanSetOP->mSetOption == QLV_SET_OPTION_ALL )
                {
                    sPlanType = QLL_PLAN_NODE_INTERSECT_ALL_TYPE;
                }
                else
                {
                    sPlanType = QLL_PLAN_NODE_INTERSECT_DISTINCT_TYPE;
                }
                break;
            }
        default:
            STL_DASSERT(0);
            break;
    }


    /**
     * Set Optimization Node 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                STL_SIZEOF( qlnoSetOP ),
                                (void **) & sCodeSetOP,
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
                                sPlanType,
                                sCodeSetOP,
                                NULL,  /* Query Node */
                                sOptCost,
                                NULL,
                                & sSetOPNode,
                                QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                aEnv )
             == STL_SUCCESS );


    /****************************************************************
     * Set Operator 설정
     ****************************************************************/

    sCodeSetOP->mSetType   = sPlanSetOP->mSetType;
    sCodeSetOP->mSetOption = sPlanSetOP->mSetOption;

    sCodeSetOP->mRootQuerySetIdx = aCodeOptParamInfo->mRootQuerySetIdx;
    sCodeSetOP->mRootQuerySet = (qlnoQuerySet*)(aCodeOptParamInfo->mRootQuerySet);


    /****************************************************************
     * Child Code Plan Array 할당 
     ****************************************************************/

    sCodeSetOP->mChildCount = sPlanSetOP->mChildCount;

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                STL_SIZEOF(qllOptimizationNode *) * sCodeSetOP->mChildCount,
                                (void **) & sCodeSetOP->mChildNodeArray,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sCodeSetOP->mChildNodeArray,
               0x00,
               STL_SIZEOF(qllOptimizationNode *) * sCodeSetOP->mChildCount );


    /****************************************************************
     * Child Code Optimize
     ****************************************************************/

    for( i = 0; i < sPlanSetOP->mChildCount; i++ )
    {
        aCodeOptParamInfo->mCandPlan = sPlanSetOP->mChildPlanNodeArr[i];

        QLNO_CODE_OPTIMIZE( aCodeOptParamInfo, aEnv );

        sCodeSetOP->mChildNodeArray[i] = aCodeOptParamInfo->mOptNode;
    }

    sCodeSetOP->mCurChildIdx = sCodeSetOP->mChildCount;


    /****************************************************************
     * Row Block이 필요한지 여부
     ****************************************************************/

    sCodeSetOP->mNeedRowBlock = sPlanSetOP->mCommon.mNeedRowBlock;


    /****************************************************************
     * Rebuild가 필요한지 여부
     ****************************************************************/

    sCodeSetOP->mNeedRebuild = sPlanSetOP->mNeedRebuild;

    
    /****************************************************************
     * OUTPUT 설정
     ****************************************************************/

    aCodeOptParamInfo->mOptNode = sSetOPNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief QUERY SET 의 All Expr리스트 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoAddExprQuerySet( qllDBCStmt            * aDBCStmt,
                               qllStatement          * aSQLStmt,
                               qllOptNodeList        * aOptNodeList,
                               qllOptimizationNode   * aOptNode,
                               qloInitExprList       * aQueryExprList,
                               qllEnv                * aEnv )
{
    qlnoQuerySet         * sOptQuerySet  = NULL;
    qlvRefExprItem       * sRefColumnItem = NULL;
        
    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_QUERY_SET_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );    


    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/
    
    sOptQuerySet = (qlnoQuerySet *) aOptNode->mOptNode;

    
    /****************************************************************
     * Child Node의 Add Expression 수행
     ****************************************************************/

    QLNO_ADD_EXPR( aDBCStmt,
                   aSQLStmt,
                   aOptNodeList,
                   sOptQuerySet->mChildNode,
                   aQueryExprList,
                   aEnv );
    
    /****************************************************************
     * Add Query Set Column Expression
     ****************************************************************/

    if ( sOptQuerySet->mIsRootSet == STL_TRUE )
    {
        /**
         * Root Query Set 인 경우
         */

        sRefColumnItem = sOptQuerySet->mColumnList->mHead;
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
    }
    else
    {
        /**
         * Inner Query Set 인 경우
         */
    }
    
    /****************************************************************
     * Add Target Expression
     ****************************************************************/

    /**
     * Add Expr Target clause Expression
     * ------------------------
     *  Target의 경우 Table Node에서 읽은 Column을
     *  Inner Column으로 한번더 감싸고 있기때문에
     *  OrgExpr도 All Expression List에 넣어야 한다.
     */

    sRefColumnItem = sOptQuerySet->mTargetList->mHead;
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



/**
 * @brief SET Operation 의 All Expr리스트 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoAddExprSetOP( qllDBCStmt            * aDBCStmt,
                            qllStatement          * aSQLStmt,
                            qllOptNodeList        * aOptNodeList,
                            qllOptimizationNode   * aOptNode,
                            qloInitExprList       * aQueryExprList,
                            qllEnv                * aEnv )
{
    qlnoSetOP        * sOptSetOP  = NULL;
    /* qlvRefExprItem   * sRefColumnItem = NULL; */

    stlInt32  i = 0;
    
    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aOptNode->mType == QLL_PLAN_NODE_UNION_ALL_TYPE) ||
                        (aOptNode->mType == QLL_PLAN_NODE_UNION_DISTINCT_TYPE) ||
                        (aOptNode->mType == QLL_PLAN_NODE_EXCEPT_ALL_TYPE) ||
                        (aOptNode->mType == QLL_PLAN_NODE_EXCEPT_DISTINCT_TYPE) ||
                        (aOptNode->mType == QLL_PLAN_NODE_INTERSECT_ALL_TYPE) ||
                        (aOptNode->mType == QLL_PLAN_NODE_INTERSECT_DISTINCT_TYPE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );    


    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/
    
    sOptSetOP = (qlnoSetOP *) aOptNode->mOptNode;

    
    /****************************************************************
     * Child Node의 Add Expression 수행
     ****************************************************************/

    for ( i = 0; i < sOptSetOP->mChildCount; i++ )
    {
        QLNO_ADD_EXPR( aDBCStmt,
                       aSQLStmt,
                       aOptNodeList,
                       sOptSetOP->mChildNodeArray[i],
                       aQueryExprList,
                       aEnv );
    }
    
    /****************************************************************
     * Add Set Column Expression
     ****************************************************************/

    /**
     * 하위 노드의 Set Column Expression을 참조하고 있기 때문에 Add Expression 생략
     */

    /* Do Nothing */

    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief QUERY SET 의 Optimization Node 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoCompleteQuerySet( qllDBCStmt            * aDBCStmt,
                                qllStatement          * aSQLStmt,
                                qllOptNodeList        * aOptNodeList,
                                qllOptimizationNode   * aOptNode,
                                qloInitExprList       * aQueryExprList,
                                qllEnv                * aEnv )
{
    qlnoQuerySet    * sOptQuerySet = NULL;
    qlvInitTarget   * sSetTargets  = NULL;
    qlvRefExprItem  * sRefExprItem = NULL;


    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_QUERY_SET_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );    


    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/
    
    sOptQuerySet = (qlnoQuerySet *) aOptNode->mOptNode;
    

    /****************************************************************
     * 하위 node에 대한 Complete Node 수행
     ****************************************************************/

    QLNO_COMPLETE( aDBCStmt,
                   aSQLStmt,
                   aOptNodeList,
                   sOptQuerySet->mChildNode,
                   aQueryExprList,
                   aEnv );

    /****************************************************************
     * Query Set 의 최종 DB Type 결정
     ****************************************************************/

    /**
     * Column List DB Type 설정
     */

    sSetTargets  = sOptQuerySet->mSetTargets;
    sRefExprItem = sOptQuerySet->mColumnList->mHead;
    
    while( sRefExprItem != NULL )
    {
        STL_TRY( qloCastExprDBType( sRefExprItem->mExprPtr->mExpr.mInnerColumn->mOrgExpr,
                                    aQueryExprList->mStmtExprList->mTotalExprInfo,
                                    sSetTargets->mDataTypeInfo.mDataType,
                                    sSetTargets->mDataTypeInfo.mArgNum1,
                                    sSetTargets->mDataTypeInfo.mArgNum2,
                                    sSetTargets->mDataTypeInfo.mStringLengthUnit,
                                    sSetTargets->mDataTypeInfo.mIntervalIndicator,
                                    STL_FALSE, /* aIsInFunction */
                                    STL_FALSE,
                                    aEnv )
                 == STL_SUCCESS );

        STL_TRY( qloSetExprDBType( sRefExprItem->mExprPtr,
                                   aQueryExprList->mStmtExprList->mTotalExprInfo,
                                   sSetTargets->mDataTypeInfo.mDataType,
                                   sSetTargets->mDataTypeInfo.mArgNum1,
                                   sSetTargets->mDataTypeInfo.mArgNum2,
                                   sSetTargets->mDataTypeInfo.mStringLengthUnit,
                                   sSetTargets->mDataTypeInfo.mIntervalIndicator,
                                   aEnv )
                 == STL_SUCCESS );

        sSetTargets++;
        sRefExprItem = sRefExprItem->mNext;
    }
    
    
    /**
     * Target List DB Type 설정
     */

    sSetTargets  = sOptQuerySet->mSetTargets;
    sRefExprItem = sOptQuerySet->mTargetList->mHead;
    
    while( sRefExprItem != NULL )
    {
        STL_TRY( qloSetExprDBType( sRefExprItem->mExprPtr->mExpr.mInnerColumn->mOrgExpr,
                                   aQueryExprList->mStmtExprList->mTotalExprInfo,
                                   sSetTargets->mDataTypeInfo.mDataType,
                                   sSetTargets->mDataTypeInfo.mArgNum1,
                                   sSetTargets->mDataTypeInfo.mArgNum2,
                                   sSetTargets->mDataTypeInfo.mStringLengthUnit,
                                   sSetTargets->mDataTypeInfo.mIntervalIndicator,
                                   aEnv )
                 == STL_SUCCESS );
        
        STL_TRY( qloSetExprDBType( sRefExprItem->mExprPtr, 
                                   aQueryExprList->mStmtExprList->mTotalExprInfo,
                                   sSetTargets->mDataTypeInfo.mDataType,
                                   sSetTargets->mDataTypeInfo.mArgNum1,
                                   sSetTargets->mDataTypeInfo.mArgNum2,
                                   sSetTargets->mDataTypeInfo.mStringLengthUnit,
                                   sSetTargets->mDataTypeInfo.mIntervalIndicator,
                                   aEnv )
                 == STL_SUCCESS );

        sSetTargets++;
        sRefExprItem = sRefExprItem->mNext;
    }


    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}




/**
 * @brief Set Operation 의 Optimization Node 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoCompleteSetOP( qllDBCStmt            * aDBCStmt,
                             qllStatement          * aSQLStmt,
                             qllOptNodeList        * aOptNodeList,
                             qllOptimizationNode   * aOptNode,
                             qloInitExprList       * aQueryExprList,
                             qllEnv                * aEnv )
{
    qlnoSetOP       * sOptSetOP    = NULL;
    qlvInitTarget   * sSetTargets  = NULL;
    qlvRefExprItem  * sRefExprItem = NULL;
    stlInt32          i            = 0;
    
    
    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aOptNode->mType == QLL_PLAN_NODE_UNION_ALL_TYPE) ||
                        (aOptNode->mType == QLL_PLAN_NODE_UNION_DISTINCT_TYPE) ||
                        (aOptNode->mType == QLL_PLAN_NODE_EXCEPT_ALL_TYPE) ||
                        (aOptNode->mType == QLL_PLAN_NODE_EXCEPT_DISTINCT_TYPE) ||
                        (aOptNode->mType == QLL_PLAN_NODE_INTERSECT_ALL_TYPE) ||
                        (aOptNode->mType == QLL_PLAN_NODE_INTERSECT_DISTINCT_TYPE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );    


    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/
    
    sOptSetOP = (qlnoSetOP *) aOptNode->mOptNode;

    STL_DASSERT( sOptSetOP->mRootQuerySet != NULL );
    STL_DASSERT( sOptSetOP->mRootQuerySet->mSetTargets != NULL );
    
    
    /****************************************************************
     * 하위 node에 대한 Complete Node 수행
     ****************************************************************/

    for ( i = 0; i < sOptSetOP->mChildCount; i++ )
    {
        QLNO_COMPLETE( aDBCStmt,
                       aSQLStmt,
                       aOptNodeList,
                       sOptSetOP->mChildNodeArray[i],
                       aQueryExprList,
                       aEnv );

        /* Type Cast */
        if( sOptSetOP->mChildNodeArray[i]->mType == QLL_PLAN_NODE_QUERY_SPEC_TYPE )
        {
            STL_DASSERT( sOptSetOP->mRootQuerySet->mTargetCount ==
                         ((qlnoQuerySpec *)(sOptSetOP->mChildNodeArray[i]->mOptNode))->mTargetColList->mCount );

            sRefExprItem =
                ((qlnoQuerySpec *)(sOptSetOP->mChildNodeArray[i]->mOptNode))->mTargetColList->mHead;

            sSetTargets = sOptSetOP->mRootQuerySet->mSetTargets;
            while( sRefExprItem != NULL )
            {
                if( aOptNode->mType != QLL_PLAN_NODE_UNION_ALL_TYPE )
                {
                    STL_TRY_THROW( dtlCheckKeyCompareType( sSetTargets->mDataTypeInfo.mDataType ) == STL_TRUE,
                                   RAMP_ERR_INVALID_KEY_COMP_TYPE );
                }

                STL_TRY( qloCastExprDBType( sRefExprItem->mExprPtr->mExpr.mInnerColumn->mOrgExpr,
                                            aQueryExprList->mStmtExprList->mTotalExprInfo,
                                            sSetTargets->mDataTypeInfo.mDataType,
                                            sSetTargets->mDataTypeInfo.mArgNum1,
                                            sSetTargets->mDataTypeInfo.mArgNum2,
                                            sSetTargets->mDataTypeInfo.mStringLengthUnit,
                                            sSetTargets->mDataTypeInfo.mIntervalIndicator,
                                            STL_FALSE, /* aIsInFunction */
                                            STL_FALSE,
                                            aEnv )
                         == STL_SUCCESS );
                
                STL_TRY( qloSetExprDBType( sRefExprItem->mExprPtr,
                                           aQueryExprList->mStmtExprList->mTotalExprInfo,
                                           sSetTargets->mDataTypeInfo.mDataType,
                                           sSetTargets->mDataTypeInfo.mArgNum1,
                                           sSetTargets->mDataTypeInfo.mArgNum2,
                                           sSetTargets->mDataTypeInfo.mStringLengthUnit,
                                           sSetTargets->mDataTypeInfo.mIntervalIndicator,
                                           aEnv )
                         == STL_SUCCESS );

                sRefExprItem = sRefExprItem->mNext;
                sSetTargets++;
            }
        }
        else if( sOptSetOP->mChildNodeArray[i]->mType == QLL_PLAN_NODE_QUERY_SET_TYPE )
        {
            STL_DASSERT( sOptSetOP->mRootQuerySet->mTargetCount ==
                         ((qlnoQuerySet *)(sOptSetOP->mChildNodeArray[i]->mOptNode))->mTargetList->mCount );

            sRefExprItem =
                ((qlnoQuerySet *)(sOptSetOP->mChildNodeArray[i]->mOptNode))->mTargetList->mHead;

            sSetTargets = sOptSetOP->mRootQuerySet->mSetTargets;
            while( sRefExprItem != NULL )
            {
                if( aOptNode->mType != QLL_PLAN_NODE_UNION_ALL_TYPE )
                {
                    STL_TRY_THROW( dtlCheckKeyCompareType( sSetTargets->mDataTypeInfo.mDataType ) == STL_TRUE,
                                   RAMP_ERR_INVALID_KEY_COMP_TYPE );
                }

                STL_TRY( qloCastExprDBType( sRefExprItem->mExprPtr->mExpr.mInnerColumn->mOrgExpr,
                                            aQueryExprList->mStmtExprList->mTotalExprInfo,
                                            sSetTargets->mDataTypeInfo.mDataType,
                                            sSetTargets->mDataTypeInfo.mArgNum1,
                                            sSetTargets->mDataTypeInfo.mArgNum2,
                                            sSetTargets->mDataTypeInfo.mStringLengthUnit,
                                            sSetTargets->mDataTypeInfo.mIntervalIndicator,
                                            STL_FALSE, /* aIsInFunction */
                                            STL_FALSE,
                                            aEnv )
                         == STL_SUCCESS );
                
                STL_TRY( qloSetExprDBType( sRefExprItem->mExprPtr,
                                           aQueryExprList->mStmtExprList->mTotalExprInfo,
                                           sSetTargets->mDataTypeInfo.mDataType,
                                           sSetTargets->mDataTypeInfo.mArgNum1,
                                           sSetTargets->mDataTypeInfo.mArgNum2,
                                           sSetTargets->mDataTypeInfo.mStringLengthUnit,
                                           sSetTargets->mDataTypeInfo.mIntervalIndicator,
                                           aEnv )
                         == STL_SUCCESS );

                sRefExprItem = sRefExprItem->mNext;
                sSetTargets++;
            }
        }
    }
    

    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_KEY_COMP_TYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_ILLEGAL_USE_OF_DATA_TYPE,
                      qlgMakeErrPosString( aSQLStmt->mRetrySQL,
                                           sSetTargets->mExpr->mPosition,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      dtlDataTypeName[ sSetTargets->mDataTypeInfo.mDataType ] );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlno */
