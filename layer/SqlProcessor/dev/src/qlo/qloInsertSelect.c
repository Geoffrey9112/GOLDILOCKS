/*******************************************************************************
 * qloInsertSelect.c
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
 * @file qloInsertSelect.c
 * @brief SQL Processor Layer INSERT .. SELECT statement optimization
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @addtogroup qlocInsertSelect
 * @{
 */


/**
 * @brief INSERT .. SELECT 구문의 Code Area 를 최적화한다.
 * @param[in]  aTransID     Transaction ID
 * @param[in]  aDBCStmt     DBC Statement
 * @param[in]  aSQLStmt     SQL Statement 객체
 * @param[in]  aEnv         Environment
 * @remarks
 */
stlStatus qloCodeOptimizeInsertSelect( smlTransId       aTransID,
                                       qllDBCStmt     * aDBCStmt,
                                       qllStatement   * aSQLStmt,
                                       qllEnv         * aEnv )
{
    qlvInitInsertSelect   * sInitPlan       = NULL;
    qllOptimizationNode   * sOptNode        = NULL;
    qlnoInsertStmt        * sOptInsertStmt  = NULL;
    qllOptimizationNode   * sOptQueryNode   = NULL;
    
    qloExprInfo           * sTotalExprInfo  = NULL;
    knlExprStack          * sConstExprStack = NULL;

    qlvInitExpression    ** sValueExpr      = NULL;
    stlInt64                sValueCount     = 0;
    stlInt64                sColIdx         = 0;

    knlExprStack          * sExprStack      = NULL;
    stlInt32                sValuesStackCnt = 0;
    
    qllOptNodeList        * sOptNodeList    = NULL;
    knlExprStack         ** sValueStackList = NULL;
    qllEstimateCost       * sOptCost        = NULL;

    qloInitStmtExprList   * sStmtExprList   = NULL;
    qloInitExprList       * sQueryExprList  = NULL;
    qlvInitExprList       * sInitExprList   = NULL;
    
    qlvRefExprItem        * sExprItem       = NULL;
    stlUInt64               sExprCnt        = 0;

    qlvRefExprList        * sTargetColList = NULL;
    
    stlInt32                sTargetCnt      = 0;
    stlInt32                sLoop           = 0;

    qlvRefExprList        * sReturnTargetColList    = NULL;

    stlInt32                sReturnTargetCount      = 0;
    knlExprStack          * sReturnTargetExprStack  = NULL;

    qlvInitExpression     * sExpr           = NULL;
    knlExprEntry          * sExprEntry      = NULL;
    qloDBType             * sDBType         = NULL;

    qlvInitExpression     * sRowIdColumn    = NULL;
    
    qlncPlanCommon        * sCandQueryPlan     = NULL;
    qloCodeOptParamInfo     sCodeOptParamInfo;
    qlncInsertStmtParamInfo sInsertStmtParamInfo;

    qllBindParamInfo      * sBindParamInfo  = NULL;
    stlInt32                sInternalBindIdx = 0;

    qloValidTblStatList   * sTableStatList  = NULL;
        
    QLL_OPT_DECLARE( stlTime sBeginTime );
    QLL_OPT_DECLARE( stlTime sEndTime );


    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_INSERT_SELECT_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_INTO_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_QUERY_TYPE),
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

    STL_TRY( qlxInsertSelectGetValidPlan( aTransID,
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
        /* Do Nothing */
    }

    
    /****************************************************************
     * Optimization Node 생성
     ****************************************************************/

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
     * INSERT STATEMENT Optimization Node 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlnoInsertStmt ),
                                (void **) & sOptInsertStmt,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sOptInsertStmt, 0x00, STL_SIZEOF( qlnoInsertStmt ) );

    
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
                                QLL_PLAN_NODE_STMT_INSERT_TYPE,
                                sOptInsertStmt,
                                NULL,
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
     * Query 단위 Expression List 생성
     */
    
    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( qloInitExprList ),
                                (void **) & sQueryExprList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlvInitExprList ),
                                (void**) &sInitExprList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( &sInitExprList->mPseudoColExprList,
                                   QLL_CODE_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( &sInitExprList->mAggrExprList,
                                   QLL_CODE_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( &sInitExprList->mNestedAggrExprList,
                                   QLL_CODE_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    sInitExprList->mStmtExprList = sStmtExprList->mInitExprList;

    sQueryExprList->mInitExprList = sInitExprList;
    sQueryExprList->mStmtExprList = sStmtExprList;


    /**
     * Valid Table Statistic List 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( qloValidTblStatList ),
                                (void **) & sTableStatList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sTableStatList->mCount = 0;
    sTableStatList->mHead = NULL;
    sTableStatList->mTail = NULL;


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
     * Select Query Optimization
     */

    sInternalBindIdx = sInitPlan->mInitPlan.mInternalBindCount;
    
    QLL_OPT_CHECK_TIME( sBeginTime );

    STL_TRY( qlncQueryOptimizer( aTransID,
                                 aDBCStmt,
                                 aSQLStmt,
                                 sInitPlan->mSubQueryNode,
                                 sStmtExprList->mInitExprList,
                                 sTableStatList,
                                 &sInternalBindIdx,
                                 &sCandQueryPlan,
                                 aEnv )
             == STL_SUCCESS );

    QLL_OPT_CHECK_TIME( sEndTime );
    QLL_OPT_ADD_ELAPSED_TIME( aSQLStmt->mOptInfo.mOptimizerTime, sBeginTime, sEndTime );


    /**
     * Code Optimization
     */

    sCodeOptParamInfo.mDBCStmt = aDBCStmt;
    sCodeOptParamInfo.mSQLStmt = aSQLStmt;
    sCodeOptParamInfo.mQueryExprList = sQueryExprList;
    sCodeOptParamInfo.mOptNodeList = sOptNodeList;
    sCodeOptParamInfo.mOptCost = sOptCost;
    sCodeOptParamInfo.mOptQueryNode = NULL;
    sCodeOptParamInfo.mCandPlan = sCandQueryPlan;
    sCodeOptParamInfo.mOptNode = NULL;
    sCodeOptParamInfo.mRootQuerySetIdx = 0;
    sCodeOptParamInfo.mRootQuerySet = NULL;

    QLNO_CODE_OPTIMIZE( &sCodeOptParamInfo, aEnv );

    sOptQueryNode = sCodeOptParamInfo.mOptNode;


    /**
     * Insert Stmt Optimization
     */

    sInsertStmtParamInfo.mStmtNode = (qlvInitNode *) sInitPlan;
    sInsertStmtParamInfo.mWriteTableNode = sInitPlan->mWriteTableNode;
    sInsertStmtParamInfo.mMultiValueCount = 1;
    sInsertStmtParamInfo.mTotalColumnCount = sInitPlan->mTotalColumnCnt;
    sInsertStmtParamInfo.mInsertColExprList = sInitPlan->mInsertColExprList;
    sInsertStmtParamInfo.mInsertTarget = &(sInitPlan->mInsertTarget);
    sInsertStmtParamInfo.mRowList = NULL;
    sInsertStmtParamInfo.mReturnTargetCount = sInitPlan->mReturnTargetCnt;
    sInsertStmtParamInfo.mReturnTargetArray = sInitPlan->mReturnTargetArray;
    sInsertStmtParamInfo.mOutReturnTargetArray = NULL;
    sInsertStmtParamInfo.mOutReturnOuterColumnList = NULL;
    sInsertStmtParamInfo.mOutRowList = NULL;
    sInsertStmtParamInfo.mOutInsertColExprList = NULL;
    sInsertStmtParamInfo.mOutPlanWriteTable = NULL;
    sInsertStmtParamInfo.mOutPlanRowSubQueryList = NULL;
    sInsertStmtParamInfo.mOutPlanReturnSubQueryList = NULL;
    sInsertStmtParamInfo.mOutStmtNode = NULL;

    QLL_OPT_CHECK_TIME( sBeginTime );

    STL_TRY( qlncInsertSelectOptimizer( aTransID,
                                        aDBCStmt,
                                        aSQLStmt,
                                        sQueryExprList,
                                        sTableStatList,
                                        &sInternalBindIdx,
                                        &sInsertStmtParamInfo,
                                        aEnv )
             == STL_SUCCESS );

    QLL_OPT_CHECK_TIME( sEndTime );
    QLL_OPT_ADD_ELAPSED_TIME( aSQLStmt->mOptInfo.mOptimizerTime, sBeginTime, sEndTime );
    QLL_OPT_ADD_COUNT( aSQLStmt->mOptInfo.mOptimizerCallCount, 1 );


    /**
     * Init Plan 연결 재설정
     */

    if( (aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_INTO_TYPE) ||
        (aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_QUERY_TYPE) )
    {
        sInitPlan = ((qlvInitInsertReturnInto *) sInsertStmtParamInfo.mOutStmtNode)->mInitInsertSelect;
    }
    else
    {
        sInitPlan = (qlvInitInsertSelect *) sInsertStmtParamInfo.mOutStmtNode;
    }


    /**
     * Valid Table Statistic List
     */

    sOptInsertStmt->mValidTblStatList = sInsertStmtParamInfo.mOutValidTblStatList;


    /**
     * Return 공간 확보
     */

    if ( sInitPlan->mIntoTargetArray == NULL )
    {
        sOptInsertStmt->mReturnTarget = sInsertStmtParamInfo.mOutReturnTargetArray;
    }
    else
    {
        sOptInsertStmt->mReturnTarget = NULL;
    }


    /****************************************************************
     * Trace Optimizer 종료
     ****************************************************************/

    QLL_SET_DISABLE_TRACE_LOGGER(aEnv);


    sValueCount = sInitPlan->mTotalColumnCnt;
    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlvInitExpression * ) * sValueCount,
                                (void **) & sValueExpr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    for( sValueCount = 0 ; sValueCount < sInitPlan->mTotalColumnCnt ; sValueCount++ )
    {
        if( sInsertStmtParamInfo.mOutInsertTarget->mIsRowDefault[ sValueCount ] == STL_TRUE )
        {
            sValueExpr[ sValueCount ] =
                sInsertStmtParamInfo.mOutInsertTarget->mRowDefaultExpr[ sValueCount ];
        }
        else
        {
            /* Do Nothing */
        }
    }

    if( sOptQueryNode->mType == QLL_PLAN_NODE_QUERY_SPEC_TYPE )
    {
        sTargetColList = ((qlnoQuerySpec *) sOptQueryNode->mOptNode)->mTargetColList;
    }
    else
    {
        STL_DASSERT( sOptQueryNode->mType == QLL_PLAN_NODE_QUERY_SET_TYPE );
        sTargetColList = ((qlnoQuerySet *) sOptQueryNode->mOptNode)->mTargetList;
    }
    
    sTargetCnt = sTargetColList->mCount;
    sExprItem  = sTargetColList->mHead;

    for( sLoop = 0 ; sLoop < sTargetCnt ; sLoop++ )
    {
        sColIdx = sInsertStmtParamInfo.mOutInsertTarget->mTargetColumnIdx[ sLoop ];
        
        if( sInsertStmtParamInfo.mOutInsertTarget->mIsRowDefault[ sColIdx ] == STL_TRUE )
        {
            /* Do Nothing */
        }
        else
        {
            sValueExpr[ sColIdx ] = sExprItem->mExprPtr;
        }

        if ( sInsertStmtParamInfo.mOutInsertTarget->mIsDeclareColumn[ sColIdx ] == STL_TRUE )
        {
            sExprItem = sExprItem->mNext;
        }
        else
        {
            /* Do Nothing */
        }
    }

    sOptInsertStmt->mInsertValueCount = sInitPlan->mTotalColumnCnt;
    sOptInsertStmt->mInsertValueExpr  = sValueExpr;

    /* SubQuery Expression List가 존재하는 경우 처리 */
    STL_DASSERT( sInsertStmtParamInfo.mOutPlanRowSubQueryList == NULL );


    /**
     * Return Target Column List에 Expression 추가
     */

    if( sInitPlan->mReturnTargetCnt > 0 )
    {
        STL_TRY( qlvCreateRefExprList( &sReturnTargetColList,
                                       QLL_CODE_PLAN( aDBCStmt ),
                                       aEnv )
                 == STL_SUCCESS );

        for( sLoop = 0; sLoop < sInitPlan->mReturnTargetCnt; sLoop++ )
        {
            STL_TRY( qlvAddExprToRefExprList(
                         sReturnTargetColList,
                         sInsertStmtParamInfo.mOutReturnTargetArray[ sLoop ].mExpr,
                         STL_FALSE,
                         QLL_CODE_PLAN( aDBCStmt ),
                         aEnv )
                     == STL_SUCCESS );
        }

        /* SubQuery Expression List가 존재하는 경우 처리 */
        if( sInsertStmtParamInfo.mOutPlanReturnSubQueryList != NULL )
        {
            sCodeOptParamInfo.mDBCStmt = aDBCStmt;
            sCodeOptParamInfo.mSQLStmt = aSQLStmt;
            sCodeOptParamInfo.mQueryExprList = sQueryExprList;
            sCodeOptParamInfo.mOptNodeList = sOptNodeList;
            sCodeOptParamInfo.mOptCost = sOptCost;
            sCodeOptParamInfo.mOptQueryNode = NULL;
            sCodeOptParamInfo.mCandPlan = sInsertStmtParamInfo.mOutPlanReturnSubQueryList;
            sCodeOptParamInfo.mOptNode = NULL;
            sCodeOptParamInfo.mRootQuerySetIdx = 0;
            sCodeOptParamInfo.mRootQuerySet = NULL;

            QLNO_CODE_OPTIMIZE( &sCodeOptParamInfo, aEnv );

            sOptInsertStmt->mReturnChildNode = sCodeOptParamInfo.mOptNode;

            /* optimizer에서 이미 할당되었음을 보장해야 한다. */
            STL_DASSERT( sInsertStmtParamInfo.mOutReturnOuterColumnList != NULL );
            sOptInsertStmt->mReturnOuterColumnList =
                sInsertStmtParamInfo.mOutReturnOuterColumnList;
        }
        else
        {
            sOptInsertStmt->mReturnChildNode = NULL;
            sOptInsertStmt->mReturnOuterColumnList = NULL;
        }
    }
    else
    {
        sReturnTargetColList = NULL;
        sOptInsertStmt->mReturnChildNode = NULL;
        sOptInsertStmt->mReturnOuterColumnList = NULL;
    }

    sOptInsertStmt->mReturnExprCnt   = sInitPlan->mReturnTargetCnt;
    sOptInsertStmt->mReturnExprList  = sReturnTargetColList;
    sOptInsertStmt->mInsertColumnCnt = sInitPlan->mTotalColumnCnt;
    sOptInsertStmt->mInsertRowCount  = 1;

    sOptInsertStmt->mHasQuery     = STL_TRUE;
    sOptInsertStmt->mQueryNode    = sOptQueryNode;


    /****************************************************************
     * Add Expression 수행
     ****************************************************************/

    /**
     * Returning절에 올 수 있는 RowId Column을 처리하기 위해
     * Statement Expression List에 Read RowId Column 추가
     */

    sRowIdColumn = ((qlvInitBaseTableNode*)(sInsertStmtParamInfo.mOutPlanWriteTable->mInitNode))->mRefRowIdColumn;

    if( sRowIdColumn != NULL )
    {
        STL_TRY( qlvAddExprToRefExprList( sStmtExprList->mRowIdColumnExprList,
                                          sRowIdColumn,
                                          STL_FALSE,
                                          QLL_CODE_PLAN( aDBCStmt ),
                                          aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /* Do Nothing */
    }

    /**
     * Node List를 뒤에서부터 순차적으로 순회하면서
     * 각 노드의 Expr 정보들을 추가함
     */

    QLNO_ADD_EXPR( aDBCStmt,
                   aSQLStmt,
                   sOptNodeList,
                   sOptNode,
                   sQueryExprList,
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
     * Expression 을 위한 Code Stack 생성
     ****************************************************************/
    
    /**
     * Expression Code Stack 공간 할당
     */

    sValuesStackCnt = sInitPlan->mTotalColumnCnt;
    
    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( knlExprStack ) * sValuesStackCnt,
                                (void **) & sExprStack,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    stlMemset( sExprStack, 0x00, STL_SIZEOF( knlExprStack ) * sValuesStackCnt );

    
    /****************************************************************
     * Code Plan 설정
     ****************************************************************/

    STL_TRY( knlAllocRegionMem(
                 QLL_CODE_PLAN( aDBCStmt ),
                 STL_SIZEOF( knlExprStack * ),
                 (void **) & sValueStackList,
                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    stlMemset( sValueStackList,
               0x00,
               STL_SIZEOF( knlExprStack * ) );

    sValueStackList[ 0 ] = & sExprStack[ 0 ];


    /****************************************************************
     * RETURN Target Expression 을 위한 Code Stack 생성
     ****************************************************************/

    if( sInitPlan->mReturnTargetCnt > 0 )
    {
        /**
         * RETURN Target Expression Code Stack 공간 할당
         */

        sReturnTargetCount = sInitPlan->mReturnTargetCnt;

        STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                    STL_SIZEOF( knlExprStack ) * sReturnTargetCount,
                                    (void**) &sReturnTargetExprStack,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemset( sReturnTargetExprStack,
                   0x00,
                   STL_SIZEOF( knlExprStack ) * sReturnTargetCount );
    }
    else
    {
        sReturnTargetCount      = 0;
        sReturnTargetExprStack  = NULL;
    }


    /****************************************************************
     * INSERT STATEMENT Optimization Node 정보 설정
     ****************************************************************/

    /**
     * Total Optimization Node List
     * => Statement에 사용되는 모든 Optimization Node List
     */

    sOptInsertStmt->mOptNodeList = sOptNodeList;

    
    /**
     * Init Plan Node
     */
    
    sOptInsertStmt->mCachedInitNode = (void *) sInsertStmtParamInfo.mOutStmtNode;


    /**
     * Query Optimization Node Idx
     */

    /* sOptInsertStmt->mHasQuery     = STL_TRUE; */
    /* sOptInsertStmt->mQueryNodeIdx = sOptQueryNode->mIdx; */


    /**
     * Insert Values 관련 정보 설정
     */
    
    /* sOptInsertStmt->mInsertColumnCnt      = sInitPlan->mColumnCnt; */
    /* sOptInsertStmt->mInsertRowCount       = 1; */
    sOptInsertStmt->mInsertColExprList    = sInsertStmtParamInfo.mOutInsertColExprList;
    sOptInsertStmt->mInsertValueStackList = sValueStackList;
    /* sOptInsertStmt->mInsertValueExpr      = sValueExpr;  */
    /* sOptInsertStmt->mInsertValueCount     = sInitPlan->mColumnCnt; */


    /**
     * Table 관련
     */
    
    STL_TRY( qlnoAllocAndCopyString( ellGetTableName( & sInitPlan->mTableHandle ),
                                     & sOptInsertStmt->mTableName,
                                     QLL_CODE_PLAN( aDBCStmt ),
                                     aEnv )
             == STL_SUCCESS );

    sOptInsertStmt->mTableHandle    = & sInitPlan->mTableHandle;
    sOptInsertStmt->mColumnHandle   = sInitPlan->mColumnHandle;
    sOptInsertStmt->mPhysicalHandle = sInitPlan->mTablePhyHandle;

    
    /**
     * Value Block 관련
     */

    sOptInsertStmt->mStmtExprList  = sStmtExprList;
    sOptInsertStmt->mQueryExprList = sQueryExprList;
    sOptInsertStmt->mRowIdColumn   = sRowIdColumn;    
    

    /**
     * Related Object 관련
     */
    
    sOptInsertStmt->mHasCheck  = sInitPlan->mHasCheck;
    sOptInsertStmt->mHasIndex  = sInitPlan->mHasIndex;
    sOptInsertStmt->mRelObject = & sInitPlan->mRelObject;


    /**
     * RETURN INTO 절 관련
     */
    
    /* sOptInsertStmt->mReturnExprCnt   = sReturnTargetCount; */
    /* sOptInsertStmt->mReturnExprList  = sReturnTargetColList; */
    sOptInsertStmt->mIntoTargetArray = sInitPlan->mIntoTargetArray;

    
    /**
     * RETURN clause Expression Stack
     */
    
    sOptInsertStmt->mReturnExprStack = sReturnTargetExprStack;


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
                   sQueryExprList,
                   aEnv );
    

    STL_RAMP( RAMP_FINISH );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlocInsert */



/**
 * @addtogroup qlodInsertSelect
 * @{
 */



/**
 * @brief INSERT .. SELECT 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement 객체
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qloDataOptimizeInsertSelect( smlTransId     aTransID,
                                       qllDBCStmt   * aDBCStmt,
                                       qllStatement * aSQLStmt,
                                       qllEnv       * aEnv )
{
    qlvInitInsertSelect  * sInitPlan      = NULL;
    qllOptimizationNode  * sOptNode       = NULL;
    qllOptimizationNode  * sNode          = NULL;
    qlnoInsertStmt       * sOptInsertStmt = NULL;
    qllDataArea          * sDataArea      = NULL;

    knlValueBlockList    * sTargetBlock   = NULL;
    stlInt32               sTargetIdx = 0;

    qloDataOptExprInfo     sDataOptInfo;

    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_INSERT_SELECT_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_INTO_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_QUERY_TYPE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mCodePlan != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**
     * 기본 정보 획득
     */

    STL_TRY( qlxInsertSelectGetValidPlan( aTransID,
                                          aDBCStmt,
                                          aSQLStmt,
                                          QLL_PHASE_DATA_OPTIMIZE,
                                          & sInitPlan,
                                          & sOptNode,
                                          NULL,
                                          aEnv )
             == STL_SUCCESS );

    
    /**
     * INSERT STATEMENT Optimization Node 획득
     */

    sOptInsertStmt = (qlnoInsertStmt *) sOptNode->mOptNode;

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
    
    sDataArea->mExecNodeCnt = sOptInsertStmt->mOptNodeList->mCount;

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

    STL_DASSERT( sOptInsertStmt->mStmtExprList->mTotalExprInfo != NULL );
    STL_TRY( knlCreateContextInfo( sOptInsertStmt->mStmtExprList->mTotalExprInfo->mExprCnt,
                                   & sDataArea->mExprDataContext,
                                   & QLL_DATA_PLAN( aDBCStmt ),
                                   KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Data Optimization Info 설정
     */

    sDataOptInfo.mStmtExprList      = sOptInsertStmt->mStmtExprList;
    sDataOptInfo.mDataArea          = sDataArea;
    sDataOptInfo.mSQLStmt           = aSQLStmt;
    sDataOptInfo.mWriteRelationNode = NULL;
    sDataOptInfo.mBindContext       = aSQLStmt->mBindContext;
    sDataOptInfo.mRegionMem         = & QLL_DATA_PLAN( aDBCStmt );


    /**
     * 전제 Pseudo Column 공간 확보 : Data Optimization 전에 수행 
     */

    STL_TRY( qlndAllocPseudoBlockList( aSQLStmt,
                                       sOptInsertStmt->mStmtExprList->mInitExprList->mPseudoColExprList,
                                       sDataArea->mBlockAllocCnt,
                                       & sDataArea->mPseudoColBlock,
                                       & QLL_DATA_PLAN( aDBCStmt ),
                                       aEnv )
             == STL_SUCCESS );


    /**
     * RowId Column 공간 확보
     */

    STL_TRY( qlndCreateColumnBlockList( & sDataArea->mRowIdColumnBlock,
                                        & QLL_DATA_PLAN( aDBCStmt ),
                                        aEnv )
             == STL_SUCCESS );

    STL_TRY( qlndAllocRowIdColumnBlockList( aSQLStmt,
                                            sDataArea->mRowIdColumnBlock,
                                            sOptInsertStmt->mStmtExprList->mRowIdColumnExprList,
                                            sDataArea->mBlockAllocCnt,
                                            & QLL_DATA_PLAN( aDBCStmt ),
                                            aEnv )
             == STL_SUCCESS );


    /**
     * Read Column Block 할당
     * @remark Sub-Query가 Value Expression으로 오는 경우,
     *    <BR> Read Column Block을 구성해야 한다.
     */
    
    STL_TRY( qlndCreateColumnBlockList( & sDataArea->mReadColumnBlock,
                                        & QLL_DATA_PLAN( aDBCStmt ),
                                        aEnv )
             == STL_SUCCESS );

    STL_TRY( qlndAddColumnListToColumnBlockList(
                 aSQLStmt,
                 & sDataOptInfo,
                 sDataArea->mReadColumnBlock,
                 sOptInsertStmt->mStmtExprList->mColumnExprList,
                 sDataArea->mBlockAllocCnt,
                 aEnv )
             == STL_SUCCESS );


    /**
     * Write Column Block 할당
     * @remark INSERT STATEMENT는 Write Column Block으로
     *    <BR> Insert Values 대상 Column을 설정한다.
     */

    STL_TRY( qlndCreateColumnBlockList( & sDataArea->mWriteColumnBlock,
                                        & QLL_DATA_PLAN( aDBCStmt ),
                                        aEnv )
             == STL_SUCCESS );

    STL_TRY( qlndAddColumnListToColumnBlockList(
                 aSQLStmt,
                 & sDataOptInfo,
                 sDataArea->mWriteColumnBlock,
                 sOptInsertStmt->mInsertColExprList,
                 sDataArea->mBlockAllocCnt,
                 aEnv )
             == STL_SUCCESS );

    STL_DASSERT( sDataArea->mWriteColumnBlock->mHead != NULL );
    STL_DASSERT( sDataArea->mWriteColumnBlock->mHead->mNextRelation == NULL );

    
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
     * Fetch 결과가 저장될 Target Column Block 할당
     */

    sDataArea->mTargetBlock = NULL;


    /**
     * Expression 을 위한 Data Context 생성 
     * - Expression Data Context 는
     *   Expression 의 Code Stack 이 참조할 수 있도록
     *   Value Block 과 Context 의 관계를 형성한다.
     */

    if( (sOptInsertStmt->mInsertColExprList != NULL) &&
        (sOptInsertStmt->mInsertColExprList->mHead != NULL) )
    {
        STL_DASSERT( sOptInsertStmt->mInsertColExprList->mHead->mExprPtr->mType == QLV_EXPR_TYPE_COLUMN );

        sDataOptInfo.mWriteRelationNode = 
            sOptInsertStmt->mInsertColExprList->mHead->mExprPtr->mExpr.mColumn->mRelationNode;
    }
    else
    {
        /* Do Nothing */
    }

    STL_TRY( qloDataOptimizeExpressionAll( & sDataOptInfo,
                                           aEnv )
             == STL_SUCCESS );


    /**
     * Write Row Count 초기화
     */
    
    sDataArea->mWriteRowCnt = 0;
                                            

    /**
     * Fetch Row Count 초기화
     */

    sDataArea->mFetchRowCnt = 0;


    /****************************************************************
     * RETURN 절 Data Optimization
     ****************************************************************/

    /**
     * 상위 Expression을 위한 Data Context 처리에서 이미 처리됐음
     */


    /****************************************************************
     * INTO 절 Data Optimization
     ****************************************************************/

    /**
     * INTO절을 위한 Cast가 유효한지 검사
     */

    if( sOptInsertStmt->mReturnExprCnt > 0 )
    {
        if ( aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_QUERY_TYPE )
        {
            /**
             * RETURNING Query
             */

            /**
             * RETURNING Fetch 를 위한 Target Block 구성
             */
            
            STL_TRY( qlndBuildRefBlockList( aSQLStmt,
                                            sDataArea,
                                            sOptInsertStmt->mStmtExprList, 
                                            sOptInsertStmt->mReturnExprList,
                                            & sDataArea->mTargetBlock,
                                            & QLL_DATA_PLAN( aDBCStmt ),
                                            aEnv )
                     == STL_SUCCESS );

            /**
             * Instant Table 에 저장하기 위한 Target Column 의 Order 부여 
             */

            STL_DASSERT( sDataArea->mTargetBlock != NULL );

            sTargetIdx = 0;
            for ( sTargetBlock = sDataArea->mTargetBlock;
                  sTargetBlock != NULL;
                  sTargetBlock = sTargetBlock->mNext )
            {
                sTargetBlock->mColumnOrder = sTargetIdx;
                sTargetIdx++;
            }
        }
        else
        {
            STL_ASSERT( aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_INTO_TYPE );
            STL_TRY( qlndDataOptIntoPhrase( aSQLStmt,
                                            sOptInsertStmt->mStmtExprList->mTotalExprInfo,
                                            aSQLStmt->mBindContext,
                                            sOptInsertStmt->mReturnExprCnt,
                                            sOptInsertStmt->mReturnExprList,
                                            sOptInsertStmt->mIntoTargetArray,
                                            sDataArea,
                                            &QLL_DATA_PLAN( aDBCStmt ),
                                            aEnv )
                     == STL_SUCCESS );
        }
    }


    /****************************************************************
     * Data Optimization For Execution Nodes
     ****************************************************************/

    /**
     * Execution Node에 대한 Data Optimization 수행
     */

    sNode = sOptInsertStmt->mOptNodeList->mHead->mNode;
    STL_TRY( qlndDataOptimizeInsertStmt( aTransID,
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


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} qlodInsert */

