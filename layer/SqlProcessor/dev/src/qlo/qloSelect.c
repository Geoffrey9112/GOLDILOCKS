/*******************************************************************************
 * qloSelect.c
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
 * @file qloSelect.c
 * @brief SQL Processor Layer SELECT statement optimization
 */

#include <qll.h>
#include <qlDef.h>



/*******************************************************
 * CODE AREA
 *******************************************************/

/**
 * @addtogroup qlocSelect
 * @{
 */

/**
 * @brief SELECT 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement 객체
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qloCodeOptimizeSelect( smlTransId     aTransID,
                                 qllDBCStmt   * aDBCStmt,
                                 qllStatement * aSQLStmt,
                                 qllEnv       * aEnv )
{
    qlvInitSelect         * sInitPlan          = NULL;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aSQLStmt->mStmtType == QLL_STMT_SELECT_TYPE ) ||
                        ( aSQLStmt->mStmtType == QLL_STMT_SELECT_FOR_UPDATE_TYPE ) ||
                        ( aSQLStmt->mStmtType == QLL_STMT_SELECT_INTO_TYPE ) ||
                        ( aSQLStmt->mStmtType == QLL_STMT_SELECT_INTO_FOR_UPDATE_TYPE ),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mInitPlan != NULL, QLL_ERROR_STACK(aEnv) );

    
    /****************************************************************
     * 정보 획득
     ****************************************************************/
    
    /**
     * 기본 정보 획득
     */

    /* Break Point */
    KNL_BREAKPOINT( KNL_BREAKPOINT_QLL_OPTIMIZE_CODE_SQL_BEGIN, KNL_ENV(aEnv) );
    
    STL_TRY( qlxSelectGetValidPlan( aTransID,
                                    aDBCStmt,
                                    aSQLStmt,
                                    QLL_PHASE_CODE_OPTIMIZE,
                                    & sInitPlan,
                                    NULL,
                                    NULL,
                                    aEnv )
             == STL_SUCCESS );
    
    /**
     * Plan Cache로부터 Code Plan 획득한 경우는 Code Opt를 생략한다.
     */
    
    if( aSQLStmt->mSqlHandle != NULL )
    {
        STL_DASSERT( aSQLStmt->mInitPlan == NULL );
        STL_DASSERT( aSQLStmt->mCodePlan != NULL );

        STL_THROW( RAMP_FINISH );
    }
    else
    {
        aSQLStmt->mInitPlan = sInitPlan;
    }

    /****************************************************************
     * Code Optimization
     ****************************************************************/

    /**
     * Code Optimization 을 수행한다.
     */

    STL_TRY( qloCodeOptimizeSelectInternal( aTransID,
                                            aDBCStmt,
                                            aSQLStmt,
                                            aEnv )
             == STL_SUCCESS );
    
    STL_RAMP( RAMP_FINISH );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief SELECT 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement 객체
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qloCodeOptimizeSelectInternal( smlTransId     aTransID,
                                         qllDBCStmt   * aDBCStmt,
                                         qllStatement * aSQLStmt,
                                         qllEnv       * aEnv )
{
    qlvInitSelect         * sInitPlan          = NULL;
    qllOptimizationNode   * sOptNode           = NULL;
    qllOptimizationNode   * sOptQueryNode      = NULL;
    qlnoSelectStmt        * sOptSelectStmt     = NULL;

    qloExprInfo           * sTotalExprInfo     = NULL;
    knlExprStack          * sConstExprStack    = NULL;

    qllOptNodeList        * sOptNodeList       = NULL;
    qllEstimateCost       * sOptCost           = NULL;
    
    qloInitStmtExprList   * sStmtExprList      = NULL;
    qloInitExprList         sQueryExprList;
    
    qlvRefExprItem        * sExprItem          = NULL;
    stlUInt64               sExprCnt           = 0;

    stlInt32                sTargetCount       = 0;
    qlvInitTarget         * sInitTargets       = NULL;
    qlvRefExprList        * sTargetColList     = NULL;
    qlvInitTarget         * sTargets           = NULL;
    stlInt32                sLoop              = 0;
    
    qlvInitExpression     * sExpr              = NULL;
    knlExprEntry          * sExprEntry         = NULL;
    qloDBType             * sDBType            = NULL;

    qlnoQuerySpec         * sOptQuerySpec      = NULL;
    qlnoQuerySet          * sOptQuerySet       = NULL;

    stlInt64                sSkipCnt           = 0;
    stlInt64                sFetchCnt          = QLL_FETCH_COUNT_MAX;
    qlvInitExpression     * sResultSkip        = NULL;
    qlvInitExpression     * sResultLimit       = NULL;

    qloCodeOptParamInfo     sCodeOptParamInfo;

    qllBindParamInfo      * sBindParamInfo  = NULL;

    qlncSelectStmtParamInfo sSelectPlan;
    stlInt32                sInternalBindIdx = 0;

    QLL_OPT_DECLARE( stlTime sBeginTime );
    QLL_OPT_DECLARE( stlTime sEndTime );

    /****************************************************************
     * Optimization Node 생성
     ****************************************************************/

    /* Break Point */
    KNL_BREAKPOINT( KNL_BREAKPOINT_QLL_OPTIMIZE_CODE_SQL_INTERNAL_BEGIN, KNL_ENV(aEnv) );
    
    sInitPlan = (qlvInitSelect *) aSQLStmt->mInitPlan;
    
    /**
     * Optimization Node List 생성
     */

    STL_TRY( qlnoCreateOptNodeList( & sOptNodeList,
                                    QLL_CODE_PLAN( aDBCStmt ),
                                    aEnv )
             == STL_SUCCESS );


    /****************************************************************
     * Common Optimization Node 정보 설정
     ****************************************************************/

    /**
     * SELECT STATEMENT Optimization Node 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlnoSelectStmt ),
                                (void **) & sOptSelectStmt,
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
     * Bind Parameter Info 생성
     */

    STL_TRY( qlnoCreateBindParamInfo( aSQLStmt,
                                      & sBindParamInfo,
                                      QLL_CODE_PLAN( aDBCStmt ),
                                      aEnv )
             == STL_SUCCESS );


    /**
     * Common Optimization Node 생성
     */

    STL_TRY( qlnoCreateOptNode( sOptNodeList,
                                QLL_PLAN_NODE_STMT_SELECT_TYPE,
                                sOptSelectStmt,
                                NULL,  /* Query Node */
                                sOptCost,
                                sBindParamInfo,
                                & sOptNode,
                                QLL_CODE_PLAN( aDBCStmt ),
                                aEnv )
             == STL_SUCCESS );


    /****************************************************************
     * Expression List 구성
     ****************************************************************/

    /**
     * Statement 단위 Expression List 생성
     */
    
    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( qloInitStmtExprList ),
                                (void **) & sStmtExprList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( & sStmtExprList->mAllExprList,
                                   QLL_CODE_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( & sStmtExprList->mColumnExprList,
                                   QLL_CODE_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( & sStmtExprList->mRowIdColumnExprList,
                                   QLL_CODE_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    sStmtExprList->mConstExprStack = NULL;
    sStmtExprList->mTotalExprInfo  = NULL;

    
    /**
     * Init Statement Expression List 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlvInitStmtExprList ),
                                (void**) &(sStmtExprList->mInitExprList),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( &sStmtExprList->mInitExprList->mPseudoColExprList,
                                   QLL_CODE_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( &sStmtExprList->mInitExprList->mConstExprList,
                                   QLL_CODE_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    
    /**
     * 임시 Query 단위 Expression List 생성
     */

    sQueryExprList.mInitExprList = NULL;
    sQueryExprList.mStmtExprList = sStmtExprList;


    /****************************************************************
     * Trace Optimizer 설정 확인
     ****************************************************************/

    if( QLL_OPT_TRACE_COST_PLAN_ON( aEnv ) == STL_TRUE )
    {
        if( QLL_IS_INIT_TRACE_LOGGER(aEnv) )
        {
            /* Trace Logger 종료 */
            STL_TRY( qllTraceDestroyLogger( aEnv ) == STL_SUCCESS );
        }

        STL_TRY( qllTraceCreateLogger( aEnv ) == STL_SUCCESS );

        QLL_SET_ENABLE_TRACE_LOGGER(aEnv);
    }
    else
    {
        QLL_SET_DISABLE_TRACE_LOGGER(aEnv);
    }


    /****************************************************************
     * Query Optimization
     ****************************************************************/

    /**
     * Query Optimization
     */

    sSelectPlan.mStmtNode     = (qlvInitNode *) sInitPlan;
    sSelectPlan.mQueryNode    = sInitPlan->mQueryNode;
    sSelectPlan.mOutPlanQuery = NULL;
    sSelectPlan.mOutStmtNode  = NULL;

    sInternalBindIdx = sInitPlan->mInitPlan.mInternalBindCount;
        
    QLL_OPT_CHECK_TIME( sBeginTime );

    STL_TRY( qlncSelectOptimizer( aTransID,
                                  aDBCStmt,
                                  aSQLStmt,
                                  & sQueryExprList,
                                  & sInternalBindIdx,
                                  & sSelectPlan,
                                  aEnv )
             == STL_SUCCESS );

    QLL_OPT_CHECK_TIME( sEndTime );
    QLL_OPT_ADD_ELAPSED_TIME( aSQLStmt->mOptInfo.mOptimizerTime, sBeginTime, sEndTime );
    QLL_OPT_ADD_COUNT( aSQLStmt->mOptInfo.mOptimizerCallCount, 1 );


    /**
     * Init Plan 연결 재설정
     */

    sInitPlan = (qlvInitSelect *) sSelectPlan.mOutStmtNode;


    /**
     * Valid Table Statistic List
     */

    sOptSelectStmt->mValidTblStatList = sSelectPlan.mOutValidTblStatList;


    /**
     * Code Optimization
     */

    sCodeOptParamInfo.mDBCStmt = aDBCStmt;
    sCodeOptParamInfo.mSQLStmt = aSQLStmt;
    sCodeOptParamInfo.mQueryExprList = &sQueryExprList;
    sCodeOptParamInfo.mOptNodeList = sOptNodeList;
    sCodeOptParamInfo.mOptCost = sOptCost;
    sCodeOptParamInfo.mOptQueryNode = NULL;
    sCodeOptParamInfo.mCandPlan = sSelectPlan.mOutPlanQuery;
    sCodeOptParamInfo.mOptNode = NULL;
    sCodeOptParamInfo.mRootQuerySetIdx = 0;
    sCodeOptParamInfo.mRootQuerySet = NULL;

    QLNO_CODE_OPTIMIZE( &sCodeOptParamInfo, aEnv );

    sOptQueryNode = sCodeOptParamInfo.mOptNode;


    /****************************************************************
     * Trace Optimizer 종료
     ****************************************************************/

    QLL_SET_DISABLE_TRACE_LOGGER(aEnv);


    /****************************************************************
     * Target Expression Optimization
     ****************************************************************/

    /**
     * Target Expression List 얻어오기
     */

    switch( sOptQueryNode->mType )
    {
        case QLL_PLAN_NODE_QUERY_SPEC_TYPE :
            {
                sOptQuerySpec  = (qlnoQuerySpec *) sOptQueryNode->mOptNode;
                sTargetColList = sOptQuerySpec->mTargetColList;
                sTargetCount   = sTargetColList->mCount;
                sInitTargets   = ( (qlvInitQuerySpecNode *) sInitPlan->mQueryNode )->mTargets;
                break;
            }
        case QLL_PLAN_NODE_QUERY_SET_TYPE :
            {
                sOptQuerySet   = (qlnoQuerySet *) sOptQueryNode->mOptNode;
                sTargetColList = sOptQuerySet->mTargetList;
                sTargetCount   = sTargetColList->mCount;
                sInitTargets   = ( (qlvInitQuerySetNode *) sInitPlan->mQueryNode )->mSetTargets;
                break;
            }
        default :
            {
                STL_DASSERT( 0 );
                break;
            }
    }

    /* target 정보 공간 할당 */
    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlvInitTarget ) * sTargetCount,
                                (void **) & sTargets,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* target 정보를 validation 정보로 초기화 */
    stlMemcpy( sTargets,
               sInitTargets,
               STL_SIZEOF( qlvInitTarget ) * sTargetCount );

    /* Target 정보의 Target Expression 갱신 */
    sExprItem = sTargetColList->mHead;
    for( sLoop = 0 ; sLoop < sTargetCount ; sLoop++ )
    {
        sTargets[ sLoop ].mExpr = sExprItem->mExprPtr;
        sExprItem = sExprItem->mNext;
    }

    sOptSelectStmt->mQueryNode = sOptQueryNode;


    /****************************************************************
     * Constant Expression 공간 설정
     ****************************************************************/

    /**
     * Constant Expression 내부의 Expression 개수 세기
     */
    
    sExprCnt = 0;
    sExprItem = sStmtExprList->mInitExprList->mConstExprList->mHead;
    while( sExprItem != NULL )
    {
        /* constant expression 은 제외한 개수 세기 */
        sExprCnt += sExprItem->mExprPtr->mIncludeExprCnt;
        sExprItem = sExprItem->mNext;
    }
    sExprCnt++;
    

    /**
     * Constant Expression Stack 공간 할당
     */

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( knlExprStack ),
                                (void **) & sConstExprStack,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    
    /**
     * Constant Expression Stack 초기화
     */
        
    STL_TRY( knlExprInit( sConstExprStack,
                          sExprCnt,
                          QLL_CODE_PLAN( aDBCStmt ),
                          KNL_ENV( aEnv ) )
             == STL_SUCCESS );


    /**
     * Statement Expression List의 Constant Expression Stack 설정
     */

    sStmtExprList->mConstExprStack = sConstExprStack;

    
    /****************************************************************
     * SELECT STATEMENT Optimization Node 정보 설정
     ****************************************************************/

    /**
     * 전체 Optimization Node List
     */
    
    sOptSelectStmt->mOptNodeList = sOptNodeList;


    /**
     * Init Plan Node
     */
    
    sOptSelectStmt->mCachedInitNode = (void *) sSelectPlan.mOutStmtNode;


    /**
     * Query Optimization Node Idx
     */
    
    /* sOptSelectStmt->mQueryNodeIdx = sOptQueryNode->mIdx; */

    
    /**
     * Target Column Count
     */

    sOptSelectStmt->mTargetCnt = sTargetCount;


    /**
     * Target Column List
     */

    sOptSelectStmt->mTargetColList = sTargetColList;


    /**
     * Target Info
     */

    sOptSelectStmt->mTargets = sTargets;

    
    /**
     * INTO clause target array
     */

    sOptSelectStmt->mIntoTargetArray = NULL;

    
    /**
     * Validation 과정에서 생성된 Value Expression List
     */

    sOptSelectStmt->mStmtExprList = sStmtExprList;

    
    /**
     * Result Set 생성을 위한 Cursor 정보
     */

    sOptSelectStmt->mScanTableList = sInitPlan->mScanTableList;
    sOptSelectStmt->mLockTableList = sInitPlan->mLockTableList;

    
    /****************************************************************
     * Add Expression 수행
     ****************************************************************/

    /**
     * 각 노드의 Expr 정보들을 추가함
     */

    QLNO_ADD_EXPR( aDBCStmt,
                   aSQLStmt,
                   sOptNodeList,
                   sOptNode,
                   & sQueryExprList,
                   aEnv );


    /****************************************************************
     * Expression 을 위한 DB type 정보 공간 설정
     ****************************************************************/

    /**
     * DB type 정보 관리를 위한 공간 할당
     */

    STL_TRY( qloCreateExprInfo( sStmtExprList->mAllExprList->mCount,
                                & sTotalExprInfo,
                                QLL_CODE_PLAN( aDBCStmt ),
                                aEnv )
             == STL_SUCCESS );

    sStmtExprList->mTotalExprInfo = sTotalExprInfo;

    
    /****************************************************************
     * Constant Expression Stack 구성
     ****************************************************************/

    /**
     * 각 Constant Expression을 하나의 Stack으로 구성
     */

    sExprItem = sStmtExprList->mInitExprList->mConstExprList->mHead;
    while( sExprItem != NULL )
    {
        sExpr = sExprItem->mExprPtr->mExpr.mConstExpr->mOrgExpr;

        STL_TRY( qloAddExprStackEntry( aSQLStmt->mRetrySQL,
                                       sExpr,
                                       aSQLStmt->mBindContext,
                                       sConstExprStack,
                                       sConstExprStack,
                                       sConstExprStack->mEntryCount + 1,
                                       KNL_BUILTIN_FUNC_INVALID,
                                       sTotalExprInfo,
                                       & sExprEntry,
                                       QLL_CODE_PLAN( aDBCStmt ),
                                       aEnv )
                 == STL_SUCCESS );

        sDBType = & sTotalExprInfo->mExprDBType[ sExpr->mOffset ];
        
        STL_TRY( qloSetExprDBType( sExprItem->mExprPtr,
                                   sTotalExprInfo,
                                   sDBType->mDBType,
                                   sDBType->mArgNum1,
                                   sDBType->mArgNum2,
                                   sDBType->mStringLengthUnit,
                                   sDBType->mIntervalIndicator,
                                   aEnv )
                 == STL_SUCCESS );

        sExprItem = sExprItem->mNext;
    }

    if( sStmtExprList->mInitExprList->mConstExprList->mCount > 1 )
    {
        sConstExprStack->mExprComposition = KNL_EXPR_COMPOSITION_COMPLEX;
    }
    else
    {
        STL_TRY( knlAnalyzeExprStack( sConstExprStack, KNL_ENV(aEnv) ) == STL_SUCCESS );
    }
    
    
    /****************************************************************
     * Code Plan 설정
     ****************************************************************/

    /**
     * Code Plan 연결 
     */

    aSQLStmt->mCodePlan = sOptNode;

    
    /****************************************************************
     * Complete Building Optimization Node
     * => Expression List 및 Node List을 한 후,
     *    Optimization Node의 추가적인 정보 구성
     ****************************************************************/

    QLNO_COMPLETE( aDBCStmt,
                   aSQLStmt,
                   sOptNodeList,
                   sOptNode,
                   & sQueryExprList,
                   aEnv );


    /****************************************************************
     * OFFSET ..  LIMIT 설정
     ****************************************************************/

    /**
     * OFFSET .. LIMIT Expression 얻어오기
     */
    
    switch( sOptQueryNode->mType )
    {
        case QLL_PLAN_NODE_QUERY_SPEC_TYPE :
            {
                sOptQuerySpec = (qlnoQuerySpec *) sOptQueryNode->mOptNode;

                sSkipCnt      = sOptQuerySpec->mSkipCnt;
                sFetchCnt     = sOptQuerySpec->mFetchCnt;
                sResultSkip   = sOptQuerySpec->mResultSkip;
                sResultLimit  = sOptQuerySpec->mResultLimit;
                
                sOptQuerySpec->mSkipCnt     = 0;
                sOptQuerySpec->mFetchCnt    = QLL_FETCH_COUNT_MAX;
                sOptQuerySpec->mResultSkip  = NULL;
                sOptQuerySpec->mResultLimit = NULL;

                break;
            }
        case QLL_PLAN_NODE_QUERY_SET_TYPE :
            {
                sOptQuerySet = (qlnoQuerySet *) sOptQueryNode->mOptNode;

                sSkipCnt      = sOptQuerySet->mSkipCnt;
                sFetchCnt     = sOptQuerySet->mFetchCnt;
                sResultSkip   = sOptQuerySet->mResultSkip;
                sResultLimit  = sOptQuerySet->mResultLimit;
                
                sOptQuerySet->mSkipCnt     = 0;
                sOptQuerySet->mFetchCnt    = QLL_FETCH_COUNT_MAX;
                sOptQuerySet->mResultSkip  = NULL;
                sOptQuerySet->mResultLimit = NULL;
                break;
            }
        default :
            {
                STL_DASSERT( 0 );
                break;
            }
    }


    /**
     * OFFSET .. LIMIT 절 관련
     */

    sOptSelectStmt->mSkipCnt     = sSkipCnt;
    sOptSelectStmt->mFetchCnt    = sFetchCnt;
    sOptSelectStmt->mResultSkip  = sResultSkip;
    sOptSelectStmt->mResultLimit = sResultLimit;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SELECT 구문의 Optimization Node 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoCompleteSelectStmt( qllDBCStmt            * aDBCStmt,
                                  qllStatement          * aSQLStmt,
                                  qllOptNodeList        * aOptNodeList,
                                  qllOptimizationNode   * aOptNode,
                                  qloInitExprList       * aQueryExprList,
                                  qllEnv                * aEnv )
{
    qlnoSelectStmt       * sOptSelectStmt      = NULL;

    
    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_STMT_SELECT_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );    


    /**********************************************************
     * 기본 정보 획득
     **********************************************************/

    /**
     * SELECT STATEMENT Node 획득
     */

    sOptSelectStmt  = (qlnoSelectStmt *) aOptNode->mOptNode;


    /****************************************************************
     * 하위 node에 대한 Complete Node 수행
     ****************************************************************/

    QLNO_COMPLETE( aDBCStmt,
                   aSQLStmt,
                   aOptNodeList,
                   sOptSelectStmt->mQueryNode,
                   aQueryExprList,
                   aEnv );

    
    /****************************************************************
     * Target Info 설정
     ****************************************************************/

    if( ( aSQLStmt->mStmtType == QLL_STMT_SELECT_TYPE ) ||
        ( aSQLStmt->mStmtType == QLL_STMT_SELECT_FOR_UPDATE_TYPE ) )
    {
        STL_TRY( qlfMakeQueryTarget( aSQLStmt,
                                     QLL_CODE_PLAN( aDBCStmt ),
                                     & sOptSelectStmt->mTargetInfo,
                                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sOptSelectStmt->mTargetInfo = NULL;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SELECT 구문의 All Expr 리스트 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoAddExprSelectStmt( qllDBCStmt            * aDBCStmt,
                                 qllStatement          * aSQLStmt,
                                 qllOptNodeList        * aOptNodeList,
                                 qllOptimizationNode   * aOptNode,
                                 qloInitExprList       * aQueryExprList,
                                 qllEnv                * aEnv )
{
    qlnoSelectStmt       * sOptSelectStmt = NULL;
    qlvRefExprItem       * sExprItem      = NULL;
    qlvInitExpression    * sExpr          = NULL;


    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_STMT_SELECT_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );    


    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/

    sOptSelectStmt = (qlnoSelectStmt*)aOptNode->mOptNode;


    /****************************************************************
     * Add Expression Optimization Node
     * => All Expression List에 InsertStmt 노드의 Expr 정보를 추가
     ****************************************************************/

    /**
     * Statement 단위 Constant Expression 들을 전체 Expr List에 추가함
     */

    sExprItem = aQueryExprList->mStmtExprList->mInitExprList->mConstExprList->mHead;
    while( sExprItem != NULL )
    {
        STL_DASSERT( sExprItem->mExprPtr->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT );

        sExpr = sExprItem->mExprPtr->mExpr.mConstExpr->mOrgExpr;
        STL_TRY( qloAddExpr( sExpr,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN( aDBCStmt ),
                             aEnv )
                 == STL_SUCCESS );

        STL_TRY( qloAddExpr( sExprItem->mExprPtr,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN( aDBCStmt ),
                             aEnv )
                 == STL_SUCCESS );

        sExprItem = sExprItem->mNext;
    }


    /****************************************************************
     * Child Node의 Add Expression 수행
     ****************************************************************/

    QLNO_ADD_EXPR( aDBCStmt,
                   aSQLStmt,
                   aOptNodeList,
                   sOptSelectStmt->mQueryNode,
                   aQueryExprList,
                   aEnv );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlocSelect */


/*******************************************************
 * DATA AREA
 *******************************************************/



/**
 * @addtogroup qlodSelect
 * @{
 */


/**
 * @brief SELECT 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement 객체
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qloDataOptimizeSelect( smlTransId     aTransID,
                                 qllDBCStmt   * aDBCStmt,
                                 qllStatement * aSQLStmt,
                                 qllEnv       * aEnv )
{
    qlvInitSelect        * sInitPlan      = NULL;
    qllOptimizationNode  * sOptNode       = NULL;
    qlnoSelectStmt       * sOptSelectStmt = NULL;
    qllDataArea          * sDataArea      = NULL;

    qllOptimizationNode  * sOptQueryNode  = NULL;
    qllOptimizationNode  * sNode          = NULL;

    qlnoQuerySpec        * sOptQuerySpec  = NULL;
    qlnoQuerySet         * sOptQuerySet   = NULL;

    qloDataOptExprInfo     sDataOptInfo;
    
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aSQLStmt->mStmtType == QLL_STMT_SELECT_TYPE ) ||
                        ( aSQLStmt->mStmtType == QLL_STMT_SELECT_FOR_UPDATE_TYPE ) ||
                        ( aSQLStmt->mStmtType == QLL_STMT_SELECT_INTO_TYPE ) ||
                        ( aSQLStmt->mStmtType == QLL_STMT_SELECT_INTO_FOR_UPDATE_TYPE ),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mCodePlan != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**
     * 기본 정보 획득
     */

    STL_TRY( qlxSelectGetValidPlan( aTransID,
                                    aDBCStmt,
                                    aSQLStmt,
                                    QLL_PHASE_DATA_OPTIMIZE,
                                    & sInitPlan,
                                    & sOptNode,
                                    NULL,
                                    aEnv )
             == STL_SUCCESS );

    
    /**
     * SELECT STATEMENT Optimization Node 획득
     */

    sOptSelectStmt = (qlnoSelectStmt *) sOptNode->mOptNode;


    /****************************************************************
     * Data Area 구성
     ****************************************************************/

    /**
     * Data Area 공간 할당
     */

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( qllDataArea ),
                                (void **) & sDataArea,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sDataArea, 0x00, STL_SIZEOF( qllDataArea ) );


    /**
     * Block Allocation Count 설정 => Block Alloc Count로 설정
     */

    sDataArea->mBlockAllocCnt = knlGetPropertyBigIntValueByID( KNL_PROPERTY_BLOCK_READ_COUNT,
                                                               KNL_ENV(aEnv) );
    
    /**
     * Execution 수행시 첫번째 Execution 인지 여부
     */

    sDataArea->mIsFirstExecution = STL_TRUE;
    
        
    /**
     * Execution Evaluate Cost 평가 여부 설정
     * @todo 향후 Session Property로 부터 정보 획득하도록 구현 필요
     */
    
    sDataArea->mIsEvaluateCost = STL_FALSE;


    /**
     * Execution Node List 할당
     */
    
    sDataArea->mExecNodeCnt = sOptSelectStmt->mOptNodeList->mCount;

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( qllExecutionNode ) * sDataArea->mExecNodeCnt,
                                (void **) & sDataArea->mExecNodeList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sDataArea->mExecNodeList,
               0x00,
               STL_SIZEOF( qllExecutionNode ) * sDataArea->mExecNodeCnt );


    /**
     * Expression Context Data 공간 할당
     */

    STL_DASSERT( sOptSelectStmt->mStmtExprList->mTotalExprInfo != NULL );
    STL_TRY( knlCreateContextInfo( sOptSelectStmt->mStmtExprList->mTotalExprInfo->mExprCnt,
                                   & sDataArea->mExprDataContext,
                                   & QLL_DATA_PLAN( aDBCStmt ),
                                   KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Data Optimization Info 설정
     */

    sDataOptInfo.mStmtExprList      = sOptSelectStmt->mStmtExprList;
    sDataOptInfo.mDataArea          = sDataArea;
    sDataOptInfo.mSQLStmt           = aSQLStmt; 
    sDataOptInfo.mWriteRelationNode = NULL;
    sDataOptInfo.mBindContext       = aSQLStmt->mBindContext;
    sDataOptInfo.mRegionMem         = & QLL_DATA_PLAN( aDBCStmt );


    /**
     * 전체 Pseudo Column 공간 확보 : Data Optimization 전에 수행 
     */

    STL_TRY( qlndAllocPseudoBlockList( aSQLStmt,
                                       sOptSelectStmt->mStmtExprList->mInitExprList->mPseudoColExprList,
                                       sDataArea->mBlockAllocCnt,
                                       & sDataArea->mPseudoColBlock,
                                       & QLL_DATA_PLAN( aDBCStmt ),
                                       aEnv )
             == STL_SUCCESS );


    /**
     * ROWID Pseudo Column Block 할당
     */

    STL_TRY( qlndCreateColumnBlockList( & sDataArea->mRowIdColumnBlock,
                                        & QLL_DATA_PLAN( aDBCStmt ),
                                        aEnv )
             == STL_SUCCESS );

    STL_TRY( qlndAllocRowIdColumnBlockList( aSQLStmt,
                                            sDataArea->mRowIdColumnBlock,
                                            sOptSelectStmt->mStmtExprList->mRowIdColumnExprList,
                                            sDataArea->mBlockAllocCnt,
                                            & QLL_DATA_PLAN( aDBCStmt ),
                                            aEnv )
             == STL_SUCCESS );
    

    /**
     * Read Column Block 할당
     */

    STL_TRY( qlndCreateColumnBlockList( & sDataArea->mReadColumnBlock,
                                        & QLL_DATA_PLAN( aDBCStmt ),
                                        aEnv )
             == STL_SUCCESS );

    STL_TRY( qlndAddColumnListToColumnBlockList(
                 aSQLStmt,
                 & sDataOptInfo,
                 sDataArea->mReadColumnBlock,
                 sOptSelectStmt->mStmtExprList->mColumnExprList,
                 sDataArea->mBlockAllocCnt,
                 aEnv )
             == STL_SUCCESS );


    /**
     * Write Column Block 할당
     */

    sDataArea->mWriteColumnBlock = NULL;
    

    /**
     * Predicate Result Column 공간 확보
     */

    STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                & sDataArea->mPredResultBlock,
                                sDataArea->mBlockAllocCnt,
                                STL_TRUE,
                                STL_LAYER_SQL_PROCESSOR,
                                KNL_ANONYMOUS_TABLE_ID, /* Table ID 지정이 의미가 없음 */
                                0,
                                DTL_TYPE_BOOLEAN,
                                gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mArgNum1,
                                gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mArgNum2,
                                gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mStringLengthUnit,
                                gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mIntervalIndicator,
                                & QLL_DATA_PLAN( aDBCStmt ),
                                aEnv )
             == STL_SUCCESS );

    
    /**
     * Expression 을 위한 Data Context 생성 
     * - Expression Data Context 는
     *   Expression 의 Code Stack 이 참조할 수 있도록
     *   Value Block 과 Context 의 관계를 형성한다.
     */

    STL_TRY( qloDataOptimizeExpressionAll( & sDataOptInfo,
                                           aEnv )
             == STL_SUCCESS );
    

    /**
     * Write Row Count 초기화
     * @remark SELECT STATEMENT는 사용하지 않는다.
     */
    
    sDataArea->mWriteRowCnt = 0;


    /**
     * Fetch Row Count 초기화
     * @remark SELECT STATEMENT는 사용하지 않는다.
     */

    sDataArea->mFetchRowCnt = 0;


    /******************************************************
     * Target Block List 구성
     * (DataArea 구축에서 가장 마지막에 설정)
     ******************************************************/

    /**
     * Fetch 결과가 저장될 Target Column Block 할당한다.
     * @remark 최상위 Query Node의 Target Expression List에 해당하는 Value Block List를
     *    <BR> DataArea의 Target Block으로 설정한다.
     *    <BR> 
     *    <BR> 반드시, qloDataOptimizeExpression() 수행 후에 설정한다.
     */
    
    /* DataArea의 Target List 설정 */
    sOptQueryNode = sOptSelectStmt->mQueryNode;
    if( sOptQueryNode->mType == QLL_PLAN_NODE_QUERY_SPEC_TYPE )
    {
        sOptQuerySpec = (qlnoQuerySpec *) sOptQueryNode->mOptNode;
        
        STL_TRY( qlndBuildRefBlockList( aSQLStmt,
                                        sDataArea,
                                        NULL,  /* statement expression list */
                                        sOptQuerySpec->mTargetColList,
                                        & sDataArea->mTargetBlock,
                                        & QLL_DATA_PLAN( aDBCStmt ),
                                        aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /* Query Node는 Query Spec Node 또는 Query Set 노드만 올 수 있다. */
        STL_DASSERT( sOptQueryNode->mType == QLL_PLAN_NODE_QUERY_SET_TYPE );

        sOptQuerySet = (qlnoQuerySet *) sOptQueryNode->mOptNode;
        
        STL_TRY( qlndBuildRefBlockList( aSQLStmt,
                                        sDataArea,
                                        NULL,  /* statement expression list */
                                        sOptQuerySet->mTargetList,
                                        & sDataArea->mTargetBlock,
                                        & QLL_DATA_PLAN( aDBCStmt ),
                                        aEnv )
                 == STL_SUCCESS );
    }


    /****************************************************************
     * Data Optimization For Execution Nodes
     ****************************************************************/

    /**
     * Execution Node에 대한 Data Optimization 수행
     */

    sNode = sOptSelectStmt->mOptNodeList->mHead->mNode;
    STL_TRY( qlndDataOptimizeSelectStmt( aTransID,
                                         aDBCStmt,
                                         aSQLStmt,
                                         sNode,
                                         sDataArea,
                                         aEnv )
             == STL_SUCCESS );

    
    /****************************************************************
     * Data Plan 설정
     ****************************************************************/

    /**
     * Data Area 연결 
     */

    aSQLStmt->mDataPlan = sDataArea;

    /****************************************************************
     * Result Set 의 Cursor 정보 설정
     ****************************************************************/

    /**
     * Result Set 의 Target 정보 설정
     */
    
    STL_TRY( qlcrDataOptInitResultSetDesc( aSQLStmt,
                                           sDataArea->mTargetBlock,
                                           & QLL_DATA_PLAN( aDBCStmt ),
                                           aEnv )
             == STL_SUCCESS );
    STL_DASSERT( aSQLStmt->mResultSetDesc != NULL );
    
    /**
     * For Sensitivity
     */

    if( aSQLStmt->mResultSetDesc->mCursorProperty.mSensitivity == ELL_CURSOR_SENSITIVITY_SENSITIVE )
    {
        /**
         * SENSITIVE Result Set 정보 설정
         */
        
        STL_TRY( qlcrDataOptSetSensitiveResultSet( aDBCStmt,
                                                   aSQLStmt,
                                                   aSQLStmt->mResultSetDesc,
                                                   aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /**
         * INSENSITIVE Result Set 정보 설정 
         */

        qlcrDataOptSetInsensitiveResultSet( aSQLStmt->mResultSetDesc );
    }
    
    /**
     * For Updatabliity
     */
    
    if ( aSQLStmt->mResultSetDesc->mCursorProperty.mUpdatability == ELL_CURSOR_UPDATABILITY_FOR_UPDATE ) 
    {
        /**
         * Lock Item 정보 구축해야 함.
         */

        STL_TRY( qlcrDataOptSetUpdatableResultSet( aDBCStmt,
                                                   aSQLStmt,
                                                   aSQLStmt->mResultSetDesc,
                                                   aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        qlcrDataOptSetReadOnlyResultSet( aSQLStmt->mResultSetDesc );
    }

    /****************************************************************
     * Result Set Materialization 정보 설정 
     ****************************************************************/
    
    if ( (aSQLStmt->mResultSetDesc->mCursorProperty.mUpdatability == ELL_CURSOR_UPDATABILITY_FOR_UPDATE) ||
         (aSQLStmt->mResultSetDesc->mCursorProperty.mScrollability == ELL_CURSOR_SCROLLABILITY_SCROLL) )
    {
        /**
         * Materialized Result Set 정보를 설정
         */

        STL_TRY( qlcrDataOptSetMaterializedResultSet( aDBCStmt,
                                                      aSQLStmt,
                                                      aSQLStmt->mResultSetDesc,
                                                      aEnv )
                 == STL_SUCCESS );
                                               
    }
    else
    {
        /**
         * Non-Materialized Result Set 정보를 설정
         */
        
        qlcrDataOptSetNonMaterializedResultSet( aSQLStmt->mResultSetDesc );
    }
         
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/** @} qlodSelect */


