/*******************************************************************************
 * qlnoSubQueryList.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlnoSubQueryList.c 8545 2013-05-23 01:55:08Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlnoSubQueryList.c
 * @brief SQL Processor Layer Code Optimization Node - Sub Query List
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
 * @brief Sub Query List에 대한 Code Optimize를 수행한다.
 * @param[in]   aCodeOptParamInfo   Code Optimize Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlnoCodeOptimizeSubQueryList( qloCodeOptParamInfo     * aCodeOptParamInfo,
                                        qllEnv                  * aEnv )
{
    qllOptimizationNode     * sOptChildNode         = NULL;
    qlnoSubQueryList        * sOptSubQueryList      = NULL;
    qllOptimizationNode     * sOptNode              = NULL;

    qllEstimateCost         * sOptCost              = NULL;
    qllEstimateCost         * sBackupOptCost        = NULL;

    qllOptimizationNode    ** sSubQueryNodes        = NULL;
    qlvRefExprList          * sRefColList           = NULL;
    qlncPlanSubQueryList    * sPlanSubQueryList     = NULL;
    qlvRefExprItem          * sExprItem             = NULL;
    stlInt32                  sLoop                 = 0;

    
    /*
     * Paramter Validation
     */

    QLO_CODE_OPT_DEFAULT_PARAM_VALIDATE( aCodeOptParamInfo, aEnv );
    STL_PARAM_VALIDATE( aCodeOptParamInfo->mCandPlan->mPlanType == QLNC_PLAN_TYPE_SUB_QUERY_LIST,
                        QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Plan 정보 획득
     ****************************************************************/

    sPlanSubQueryList = (qlncPlanSubQueryList*)(aCodeOptParamInfo->mCandPlan);
    
    
    /****************************************************************
     * Optimization Node 생성
     ****************************************************************/

    /**
     * SUB-QUERY LIST Optimization Node 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                STL_SIZEOF( qlnoSubQueryList ),
                                (void **) & sOptSubQueryList,
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
                                QLL_PLAN_NODE_SUB_QUERY_LIST_TYPE,
                                sOptSubQueryList,
                                aCodeOptParamInfo->mOptQueryNode,
                                sOptCost,
                                NULL,
                                & sOptNode,
                                QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                aEnv )
             == STL_SUCCESS );

    
    /****************************************************************
     * Child Node 구성
     ****************************************************************/

    if( sPlanSubQueryList->mChildPlanNode != NULL )
    {
        aCodeOptParamInfo->mCandPlan = sPlanSubQueryList->mChildPlanNode;
        QLNO_CODE_OPTIMIZE( aCodeOptParamInfo, aEnv );
        sOptChildNode = aCodeOptParamInfo->mOptNode;
    }
    else
    {
        sOptChildNode = NULL;
    }

    
    /****************************************************************
     * Optimization Node 구성
     ****************************************************************/

    /**
     * Cost 백업
     */

    sBackupOptCost = aCodeOptParamInfo->mOptCost;


    /**
     * Optimization Cost 공간 할당
     */

    STL_TRY( knlAllocRegionMem(
                 QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                 STL_SIZEOF( qllEstimateCost ) * sPlanSubQueryList->mSubQueryCnt,
                 (void **) & sOptCost,
                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    stlMemset( sOptCost,
               0x00,
               STL_SIZEOF( qllEstimateCost ) * sPlanSubQueryList->mSubQueryCnt );


    /**
     * Sub Query List에 대한 Optimization Node 공간 할당
     */

    STL_TRY( knlAllocRegionMem(
                 QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                 STL_SIZEOF( qllOptimizationNode * ) * sPlanSubQueryList->mSubQueryCnt,
                 (void **) & sSubQueryNodes,
                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    stlMemset( sSubQueryNodes,
               0x00,
               STL_SIZEOF( qllOptimizationNode * ) * sPlanSubQueryList->mSubQueryCnt );

    
    /**
     * Code Optimize Sub Query
     */

    sExprItem = sPlanSubQueryList->mSubQueryExprList->mHead;
  
    STL_TRY( qlvCreateRefExprList( & sRefColList,
                                   QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    for( sLoop = 0 ; sLoop < sPlanSubQueryList->mSubQueryCnt ; sLoop++ )
    {
        aCodeOptParamInfo->mCandPlan =
            (qlncPlanCommon *) sPlanSubQueryList->mSubQueryPlanNodes[ sLoop ];
        aCodeOptParamInfo->mOptCost = & sOptCost[ sLoop ];

        QLNO_CODE_OPTIMIZE( aCodeOptParamInfo, aEnv );

        sSubQueryNodes[ sLoop ] = aCodeOptParamInfo->mOptNode;

        if( sPlanSubQueryList->mSubQueryPlanNodes[ sLoop ]->mPlanType !=
            QLNC_PLAN_TYPE_SUB_QUERY_FUNCTION )
        {
            STL_TRY( qlvAddExprListToRefExprList( sRefColList,
                                                  sExprItem->mExprPtr->mExpr.mSubQuery->mRefColList,
                                                  STL_FALSE,
                                                  QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                                  aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            ((qlnoSubQueryFunc*)(sSubQueryNodes[ sLoop ]->mOptNode))->mChildNode = sOptChildNode;
        }
        
        sExprItem = sExprItem->mNext;
    }

    
    /**
     * Cost 원복
     */
    
    aCodeOptParamInfo->mOptCost = sBackupOptCost;

    
    /****************************************************************
     * SUB-QUERY Optimization Node 정보 설정
     ****************************************************************/

    /**
     * Child Optimization Node
     */
    
    sOptSubQueryList->mChildNode = sOptChildNode;


    /**
     * Row Block이 필요한지 여부
     */

    sOptSubQueryList->mNeedRowBlock = sPlanSubQueryList->mCommon.mNeedRowBlock;


    /**
     * Sub Query Expression List
     */
    
    sOptSubQueryList->mSubQueryExprList = sPlanSubQueryList->mSubQueryExprList;


    /**
     * Outer Column Expression List
     */
    
    sOptSubQueryList->mOuterColumnList = sRefColList;


    /**
     * Sub Query Optimization Nodes
     */

    sOptSubQueryList->mSubQueryNodes   = sSubQueryNodes;
    sOptSubQueryList->mSubQueryNodeCnt = sPlanSubQueryList->mSubQueryCnt;

    
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
 * @brief Sub Query List 의 All Expr리스트 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoAddExprSubQueryList( qllDBCStmt            * aDBCStmt,
                                   qllStatement          * aSQLStmt,
                                   qllOptNodeList        * aOptNodeList,
                                   qllOptimizationNode   * aOptNode,
                                   qloInitExprList       * aQueryExprList,
                                   qllEnv                * aEnv )
{
    qlnoSubQueryList  * sOptSubQueryList  = NULL;
    qlvRefExprItem    * sSubQueryExprItem = NULL;
    qlvRefExprItem    * sTargetExprItem   = NULL;
    qlvRefExprItem    * sRefColExprItem   = NULL;
    stlInt32            sLoop             = 0;
    
        
    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_LIST_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/
    
    sOptSubQueryList = (qlnoSubQueryList *) aOptNode->mOptNode;


    /****************************************************************
     * Statement단위로 처리 가능한 Sub Query의 Add Expression 수행
     ****************************************************************/

    sSubQueryExprItem = sOptSubQueryList->mSubQueryExprList->mHead;
    for( sLoop = 0 ; sLoop < sOptSubQueryList->mSubQueryNodeCnt ; sLoop++ )
    {
        if( sSubQueryExprItem->mExprPtr->mIterationTime >= DTL_ITERATION_TIME_FOR_EACH_STATEMENT )
        {
            QLNO_ADD_EXPR( aDBCStmt,
                           aSQLStmt,
                           aOptNodeList,
                           sOptSubQueryList->mSubQueryNodes[ sLoop ],
                           aQueryExprList,
                           aEnv );

            if( sSubQueryExprItem->mExprPtr->mType == QLV_EXPR_TYPE_SUB_QUERY )
            {
                sTargetExprItem = sSubQueryExprItem->mExprPtr->mExpr.mSubQuery->mTargetList->mHead;
                sRefColExprItem = sSubQueryExprItem->mExprPtr->mExpr.mSubQuery->mRefColList->mHead;

                while( sTargetExprItem != NULL )
                {
                    STL_DASSERT( sTargetExprItem->mExprPtr->mType == QLV_EXPR_TYPE_INNER_COLUMN );

                    STL_TRY( qloAddExpr( sTargetExprItem->mExprPtr,
                                         aQueryExprList->mStmtExprList->mAllExprList,
                                         QLL_CODE_PLAN( aDBCStmt ),
                                         aEnv )
                             == STL_SUCCESS );

                    STL_DASSERT( sRefColExprItem->mExprPtr->mType == QLV_EXPR_TYPE_REFERENCE );

                    STL_TRY( qloAddExpr( sRefColExprItem->mExprPtr,
                                         aQueryExprList->mStmtExprList->mAllExprList,
                                         QLL_CODE_PLAN( aDBCStmt ),
                                         aEnv )
                             == STL_SUCCESS );

                    sTargetExprItem = sTargetExprItem->mNext;
                    sRefColExprItem = sRefColExprItem->mNext;
                }
            }
        }

        sSubQueryExprItem = sSubQueryExprItem->mNext;
    }


    /****************************************************************
     * Child Node의 Add Expression 수행
     ****************************************************************/

    if( sOptSubQueryList->mChildNode != NULL )
    {
        QLNO_ADD_EXPR( aDBCStmt,
                       aSQLStmt,
                       aOptNodeList,
                       sOptSubQueryList->mChildNode,
                       aQueryExprList,
                       aEnv );
    }


    /****************************************************************
     * Statement단위로 처리 불가능한 Sub Query의 Add Expression 수행
     ****************************************************************/

    sSubQueryExprItem = sOptSubQueryList->mSubQueryExprList->mHead;
    for( sLoop = 0 ; sLoop < sOptSubQueryList->mSubQueryNodeCnt ; sLoop++ )
    {
        if( sSubQueryExprItem->mExprPtr->mIterationTime == DTL_ITERATION_TIME_FOR_EACH_QUERY )
        {
            QLNO_ADD_EXPR( aDBCStmt,
                           aSQLStmt,
                           aOptNodeList,
                           sOptSubQueryList->mSubQueryNodes[ sLoop ],
                           aQueryExprList,
                           aEnv );

            if( sSubQueryExprItem->mExprPtr->mType == QLV_EXPR_TYPE_SUB_QUERY )
            {
                sTargetExprItem = sSubQueryExprItem->mExprPtr->mExpr.mSubQuery->mTargetList->mHead;
                sRefColExprItem = sSubQueryExprItem->mExprPtr->mExpr.mSubQuery->mRefColList->mHead;

                while( sTargetExprItem != NULL )
                {
                    STL_DASSERT( sTargetExprItem->mExprPtr->mType == QLV_EXPR_TYPE_INNER_COLUMN );

                    STL_TRY( qloAddExpr( sTargetExprItem->mExprPtr,
                                         aQueryExprList->mStmtExprList->mAllExprList,
                                         QLL_CODE_PLAN( aDBCStmt ),
                                         aEnv )
                             == STL_SUCCESS );

                    STL_DASSERT( sRefColExprItem->mExprPtr->mType == QLV_EXPR_TYPE_REFERENCE );

                    STL_TRY( qloAddExpr( sRefColExprItem->mExprPtr,
                                         aQueryExprList->mStmtExprList->mAllExprList,
                                         QLL_CODE_PLAN( aDBCStmt ),
                                         aEnv )
                             == STL_SUCCESS );

                    sTargetExprItem = sTargetExprItem->mNext;
                    sRefColExprItem = sRefColExprItem->mNext;
                }
            }
        }

        sSubQueryExprItem = sSubQueryExprItem->mNext;
    }


    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Sub Query List 의 Optimization Node 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoCompleteSubQueryList( qllDBCStmt            * aDBCStmt,
                                    qllStatement          * aSQLStmt,
                                    qllOptNodeList        * aOptNodeList,
                                    qllOptimizationNode   * aOptNode,
                                    qloInitExprList       * aQueryExprList,
                                    qllEnv                * aEnv )
{
    qlnoSubQueryList  * sOptSubQueryList  = NULL;
    qlvRefExprItem    * sSubQueryExprItem = NULL;
    qlvRefExprItem    * sTargetExprItem   = NULL;
    qlvRefExprItem    * sRefColExprItem   = NULL;
    knlExprStack      * sTempCodeStack    = NULL;
    knlExprEntry      * sExprEntry        = NULL;
    stlInt32            sLoop             = 0;


    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_LIST_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/
    
    sOptSubQueryList = (qlnoSubQueryList *) aOptNode->mOptNode;


    /****************************************************************
     * Statement단위로 처리 가능한 Sub Query의 Complete Node 수행
     ****************************************************************/

    /**
     * 임시 Expression Code Stack 공간 할당
     */

    STL_TRY( knlAllocRegionMem( & QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( knlExprStack ),
                                (void **) & sTempCodeStack,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * 임시 Expression Code Stack 생성
     */

    STL_TRY( knlExprInit( sTempCodeStack,
                          3,
                          & QLL_CANDIDATE_MEM( aEnv ),
                          KNL_ENV( aEnv ) )
             == STL_SUCCESS );


    /**
     * Sub Query Complete
     */
    
    sSubQueryExprItem = sOptSubQueryList->mSubQueryExprList->mHead;
    for( sLoop = 0 ; sLoop < sOptSubQueryList->mSubQueryNodeCnt ; sLoop++ )
    {
        if( sSubQueryExprItem->mExprPtr->mIterationTime >= DTL_ITERATION_TIME_FOR_EACH_STATEMENT )
        {
            QLNO_COMPLETE( aDBCStmt,
                           aSQLStmt,
                           aOptNodeList,
                           sOptSubQueryList->mSubQueryNodes[ sLoop ],
                           aQueryExprList,
                           aEnv );

            if( sSubQueryExprItem->mExprPtr->mType == QLV_EXPR_TYPE_SUB_QUERY )
            {
                sTargetExprItem = sSubQueryExprItem->mExprPtr->mExpr.mSubQuery->mTargetList->mHead;
                sRefColExprItem = sSubQueryExprItem->mExprPtr->mExpr.mSubQuery->mRefColList->mHead;

                while( sTargetExprItem != NULL )
                {
                    sTempCodeStack->mEntryCount = 0;
                    STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                                        sTargetExprItem->mExprPtr,
                                                        aSQLStmt->mBindContext,
                                                        sTempCodeStack,
                                                        aQueryExprList->mStmtExprList->mConstExprStack,
                                                        aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                        & sExprEntry,
                                                        & QLL_CANDIDATE_MEM( aEnv ),
                                                        aEnv )
                             == STL_SUCCESS );

                    sTempCodeStack->mEntryCount = 0;
                    STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                                        sRefColExprItem->mExprPtr,
                                                        aSQLStmt->mBindContext,
                                                        sTempCodeStack,
                                                        aQueryExprList->mStmtExprList->mConstExprStack,
                                                        aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                        & sExprEntry,
                                                        & QLL_CANDIDATE_MEM( aEnv ),
                                                        aEnv )
                             == STL_SUCCESS );

                    sTargetExprItem = sTargetExprItem->mNext;
                    sRefColExprItem = sRefColExprItem->mNext;
                }
            }
        }

        sSubQueryExprItem = sSubQueryExprItem->mNext;
    }


    /****************************************************************
     * 하위 node에 대한 Complete Node 수행
     ****************************************************************/

    if( sOptSubQueryList->mChildNode != NULL )
    {
        QLNO_COMPLETE( aDBCStmt,
                       aSQLStmt,
                       aOptNodeList,
                       sOptSubQueryList->mChildNode,
                       aQueryExprList,
                       aEnv );
    }


    /****************************************************************
     * Statement단위로 처리 불가능한 Sub Query의 Complete Node 수행
     ****************************************************************/

    /**
     * Sub Query Complete
     */
    
    sSubQueryExprItem = sOptSubQueryList->mSubQueryExprList->mHead;
    for( sLoop = 0 ; sLoop < sOptSubQueryList->mSubQueryNodeCnt ; sLoop++ )
    {
        if( sSubQueryExprItem->mExprPtr->mIterationTime < DTL_ITERATION_TIME_FOR_EACH_STATEMENT )
        {
            QLNO_COMPLETE( aDBCStmt,
                           aSQLStmt,
                           aOptNodeList,
                           sOptSubQueryList->mSubQueryNodes[ sLoop ],
                           aQueryExprList,
                           aEnv );

            if( sSubQueryExprItem->mExprPtr->mType == QLV_EXPR_TYPE_SUB_QUERY )
            {
                sTargetExprItem = sSubQueryExprItem->mExprPtr->mExpr.mSubQuery->mTargetList->mHead;
                sRefColExprItem = sSubQueryExprItem->mExprPtr->mExpr.mSubQuery->mRefColList->mHead;

                while( sTargetExprItem != NULL )
                {
                    sTempCodeStack->mEntryCount = 0;
                    STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                                        sTargetExprItem->mExprPtr,
                                                        aSQLStmt->mBindContext,
                                                        sTempCodeStack,
                                                        aQueryExprList->mStmtExprList->mConstExprStack,
                                                        aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                        & sExprEntry,
                                                        & QLL_CANDIDATE_MEM( aEnv ),
                                                        aEnv )
                             == STL_SUCCESS );

                    sTempCodeStack->mEntryCount = 0;
                    STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                                        sRefColExprItem->mExprPtr,
                                                        aSQLStmt->mBindContext,
                                                        sTempCodeStack,
                                                        aQueryExprList->mStmtExprList->mConstExprStack,
                                                        aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                        & sExprEntry,
                                                        & QLL_CANDIDATE_MEM( aEnv ),
                                                        aEnv )
                             == STL_SUCCESS );

                    sTargetExprItem = sTargetExprItem->mNext;
                    sRefColExprItem = sRefColExprItem->mNext;
                }
            }
        }

        sSubQueryExprItem = sSubQueryExprItem->mNext;
    }


    /****************************************************************
     * Node 정보 설정
     ****************************************************************/

    /* Do Nothing */


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlno */
