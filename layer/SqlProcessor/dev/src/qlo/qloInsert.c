/*******************************************************************************
 * qloInsert.c
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
 * @file qloInsert.c
 * @brief SQL Processor Layer INSERT statement optimization
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlocInsert
 * @{
 */



/**
 * @brief INSERT VALUES 구문의 Code Area 를 최적화한다.
 * @param[in]  aTransID     Transaction ID
 * @param[in]  aDBCStmt     DBC Statement
 * @param[in]  aSQLStmt     SQL Statement 객체
 * @param[in]  aEnv         Environment
 * @remarks
 */
stlStatus qloCodeOptimizeInsertValues( smlTransId       aTransID,
                                       qllDBCStmt     * aDBCStmt,
                                       qllStatement   * aSQLStmt,
                                       qllEnv         * aEnv )
{
    qlvInitInsertValues   * sInitPlan       = NULL;
    qllOptimizationNode   * sOptNode        = NULL;
    qlnoInsertStmt        * sOptInsertStmt  = NULL;

    qloExprInfo           * sTotalExprInfo  = NULL;
    knlExprStack          * sConstExprStack = NULL;

    qlvInitExpression    ** sValueExpr      = NULL;
    stlInt64                sValueCount     = 0;

    knlExprStack          * sExprStack      = NULL;

    qlvInitSingleRow      * sSingleRow      = NULL;
    stlInt32                sRowCnt         = 0;
    stlInt32                sColumnCnt      = 0;
    stlInt32                sValuesStackCnt = 0;

    stlInt32                sLoop           = 0;

    qllOptNodeList        * sOptNodeList    = NULL;
    knlExprStack         ** sValueStackList = NULL;
    qllEstimateCost       * sOptCost        = NULL;

    qloInitStmtExprList   * sStmtExprList   = NULL;
    qloInitExprList       * sQueryExprList  = NULL;
    qlvInitExprList       * sInitExprList   = NULL;
    
    qlvRefExprItem        * sExprItem       = NULL;
    stlUInt64               sExprCnt        = 0;

    qlvRefExprList        * sReturnTargetColList    = NULL;

    stlInt32                sReturnTargetCount      = 0;
    knlExprStack          * sReturnTargetExprStack  = NULL;

    qlvInitExpression     * sExpr           = NULL;
    knlExprEntry          * sExprEntry      = NULL;
    qloDBType             * sDBType         = NULL;

    qlvInitExpression     * sRowIdColumn    = NULL;    

    qlncInsertStmtParamInfo sInsertStmtParamInfo;
    qloCodeOptParamInfo     sCodeOptParamInfo;

    qllBindParamInfo      * sBindParamInfo  = NULL;
    stlInt32                sInternalBindIdx = 0;
    
    QLL_OPT_DECLARE( stlTime sBeginTime );
    QLL_OPT_DECLARE( stlTime sEndTime );


    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_INSERT_TYPE) ||
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

    STL_TRY( qlxInsertGetValidPlan( aTransID,
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

    
    /**
     * Atomic 여부 설정
     */

    sOptNode->mIsAtomic = aSQLStmt->mIsAtomic;

    
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
     * Base Table Optimization
     ****************************************************************/

    /**
     * Insert Stmt Optimization
     */

    sInsertStmtParamInfo.mStmtNode = (qlvInitNode *) sInitPlan;
    sInsertStmtParamInfo.mWriteTableNode = sInitPlan->mWriteTableNode;
    sInsertStmtParamInfo.mMultiValueCount = sInitPlan->mMultiValueCnt;
    sInsertStmtParamInfo.mTotalColumnCount = sInitPlan->mTotalColumnCnt;
    sInsertStmtParamInfo.mInsertColExprList = sInitPlan->mInsertColExprList;
    sInsertStmtParamInfo.mInsertTarget = &(sInitPlan->mInsertTarget);
    sInsertStmtParamInfo.mRowList = sInitPlan->mRowList;
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

    sInternalBindIdx = sInitPlan->mInitPlan.mInternalBindCount;
    
    QLL_OPT_CHECK_TIME( sBeginTime );

    STL_TRY( qlncInsertValuesOptimizer( aTransID,
                                        aDBCStmt,
                                        aSQLStmt,
                                        sQueryExprList,
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
        sInitPlan = ((qlvInitInsertReturnInto *) sInsertStmtParamInfo.mOutStmtNode)->mInitInsertValues;
    }
    else
    {
        sInitPlan = (qlvInitInsertValues *) sInsertStmtParamInfo.mOutStmtNode;
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


    /****************************************************************
     * Set Insert Values Expression
     ****************************************************************/
    
    /**
     * Set Insert Values Expression
     */

    sSingleRow = sInsertStmtParamInfo.mOutRowList;

    sValueCount = sInitPlan->mMultiValueCnt * sInitPlan->mTotalColumnCnt;
    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlvInitExpression * ) * sValueCount,
                                (void **) & sValueExpr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sValueCount = 0;
    for( sRowCnt = 0 ; sRowCnt < sInitPlan->mMultiValueCnt ; sRowCnt++ )
    {
        for( sColumnCnt = 0 ;
             sColumnCnt < sInitPlan->mTotalColumnCnt ;
             sColumnCnt++ )
        {
            if( sInitPlan->mInsertTarget.mIsRowDefault[ sColumnCnt ] == STL_TRUE )
            {
                if( sRowCnt == 0 )
                {
                    sValueExpr[ sValueCount ] = sSingleRow->mValueExpr[ sColumnCnt ];
                }
                else
                {
                    sValueExpr[ sValueCount ] = sValueExpr[ sColumnCnt ];
                }
            }
            else
            {
                sValueExpr[ sValueCount ] = sSingleRow->mValueExpr[ sColumnCnt ];
            }
            
            sValueCount++;
        }
        sSingleRow = sSingleRow->mNext;
    }

    sOptInsertStmt->mInsertValueCount = sValueCount;
    sOptInsertStmt->mInsertValueExpr  = sValueExpr;

    /* SubQuery Expression List가 존재하는 경우 처리 */
    if( sInsertStmtParamInfo.mOutPlanRowSubQueryList != NULL )
    {
        sCodeOptParamInfo.mDBCStmt = aDBCStmt;
        sCodeOptParamInfo.mSQLStmt = aSQLStmt;
        sCodeOptParamInfo.mQueryExprList = sQueryExprList;
        sCodeOptParamInfo.mOptNodeList = sOptNodeList;
        sCodeOptParamInfo.mOptCost = sOptCost;
        sCodeOptParamInfo.mOptQueryNode = NULL;
        sCodeOptParamInfo.mCandPlan = sInsertStmtParamInfo.mOutPlanRowSubQueryList;
        sCodeOptParamInfo.mOptNode = NULL;
        sCodeOptParamInfo.mRootQuerySetIdx = 0;
        sCodeOptParamInfo.mRootQuerySet = NULL;

        QLNO_CODE_OPTIMIZE( &sCodeOptParamInfo, aEnv );

        sOptInsertStmt->mInsertValueChildNode = sCodeOptParamInfo.mOptNode;
    }
    else
    {
        sOptInsertStmt->mInsertValueChildNode = NULL;
    }


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
    sOptInsertStmt->mInsertRowCount  = sInitPlan->mMultiValueCnt;

    sOptInsertStmt->mHasQuery     = STL_FALSE;
    sOptInsertStmt->mQueryNode    = NULL;


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
     * -------------------------
     *  bind param으로 array가 오는 경우 해당 크기만큼의
     *  stack 공간을 더 할당받아 놓는다.
     */

    sValuesStackCnt = sInitPlan->mMultiValueCnt * sInitPlan->mTotalColumnCnt;
    
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
                 STL_SIZEOF( knlExprStack * ) * sInitPlan->mMultiValueCnt,
                 (void **) & sValueStackList,
                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    stlMemset( sValueStackList,
               0x00,
               STL_SIZEOF( knlExprStack * ) * sInitPlan->mMultiValueCnt );

    for( sLoop = 0 ; sLoop < sInitPlan->mMultiValueCnt ; sLoop++ )
    {
        sValueStackList[ sLoop ] = & sExprStack[ sLoop * sInitPlan->mTotalColumnCnt ];
    }


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
    
    /* sOptInsertStmt->mHasQuery     = STL_FALSE; */
    /* sOptInsertStmt->mQueryNodeIdx = QLL_INVALID_NODE_IDX; */


    /**
     * Insert Values 관련 정보 설정
     */
    
    /* sOptInsertStmt->mInsertColumnCnt      = sInitPlan->mColumnCnt; */
    /* sOptInsertStmt->mInsertRowCount       = sInitPlan->mMultiValueCnt; */
    sOptInsertStmt->mInsertColExprList    = sInsertStmtParamInfo.mOutInsertColExprList;
    sOptInsertStmt->mInsertValueStackList = sValueStackList;
    /* sOptInsertStmt->mInsertValueExpr      = sValueExpr;  */
    /* sOptInsertStmt->mInsertValueCount     = sValueCount; */

    
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


/**
 * @brief INSERT 구문의 Optimization Node 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 * @remark INSERT에 관련된 INSERT / INSERT SELECT / INSERT RETURN INTO 
 */
stlStatus qlnoCompleteInsertStmt( qllDBCStmt            * aDBCStmt,
                                  qllStatement          * aSQLStmt,
                                  qllOptNodeList        * aOptNodeList,
                                  qllOptimizationNode   * aOptNode,
                                  qloInitExprList       * aQueryExprList,
                                  qllEnv                * aEnv )
{
    qlnoInsertStmt       * sOptInsertStmt  = NULL;

    knlExprStack         * sConstExprStack = NULL;
    qloExprInfo          * sTotalExprInfo  = NULL;
    qlvInitExpression    * sCodeExpr       = NULL;
    knlExprEntry         * sExprEntry      = NULL;
    knlExprStack         * sExprStack      = NULL;

    ellDictHandle        * sColumnHandle   = NULL;
    stlInt64               sRowCnt         = 0;
    stlInt32               sColumnCnt      = 0;
    stlInt64               sExprCount      = 0;

    knlBindType           sUserBindType             = KNL_BIND_TYPE_INVALID;
    stlInt32              sLoop                     = 0;

    stlInt32              sReturnTargetCount        = 0;
    knlExprStack        * sReturnTargetExprStack    = NULL;
    qlvRefExprItem      * sReturnTargetItem         = NULL;
    qlvInitExpression   * sReturnTargetExpr         = NULL;

    qlvInitExpression   * sIntoTarget               = NULL;
    knlValueBlockList   * sValueBlockList           = NULL;
    knlValueBlock       * sValueBlock               = NULL;

    qlvInitInsertReturnInto * sInitReturnPlan = NULL;
    qlvInitTarget           * sInitReturnTarget = NULL;
    
    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_STMT_INSERT_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );    


    /**********************************************************
     * 기본 정보 획득
     **********************************************************/

    /**
     * INSERT STATEMENT Node 획득
     */

    sOptInsertStmt  = (qlnoInsertStmt *) aOptNode->mOptNode;


    /****************************************************************
     * 하위 node에 대한 Complete Node 수행
     ****************************************************************/

    if( sOptInsertStmt->mHasQuery == STL_TRUE )
    {
        QLNO_COMPLETE( aDBCStmt,
                       aSQLStmt,
                       aOptNodeList,
                       sOptInsertStmt->mQueryNode,
                       aQueryExprList,
                       aEnv );
    }
    else
    {
        /* Do Nothing */
    }

    if( sOptInsertStmt->mInsertValueChildNode != NULL )
    {
        QLNO_COMPLETE( aDBCStmt,
                       aSQLStmt,
                       aOptNodeList,
                       sOptInsertStmt->mInsertValueChildNode,
                       aQueryExprList,
                       aEnv );
    }

    if( sOptInsertStmt->mReturnChildNode != NULL )
    {
        QLNO_COMPLETE( aDBCStmt,
                       aSQLStmt,
                       aOptNodeList,
                       sOptInsertStmt->mReturnChildNode,
                       aQueryExprList,
                       aEnv );
    }


    /****************************************************************
     * Expression 을 위한 Code Stack 생성
     ****************************************************************/

    /**
     * Expression Code Stack 생성
     */

    sConstExprStack = sOptInsertStmt->mStmtExprList->mConstExprStack;
    sTotalExprInfo  = sOptInsertStmt->mStmtExprList->mTotalExprInfo;
    sExprCount      = 0;
    
    for( sRowCnt = 0 ; sRowCnt < sOptInsertStmt->mInsertRowCount ; sRowCnt++ )
    {
        for( sColumnCnt = 0 ;
             sColumnCnt < sOptInsertStmt->mInsertColumnCnt ;
             sColumnCnt++ )
        {
            /**
             * 각 Value에 대한 Expression Stack 생성
             */

            sCodeExpr  = sOptInsertStmt->mInsertValueExpr[ sExprCount ];
            sExprStack = & sOptInsertStmt->mInsertValueStackList[ sRowCnt ][ sColumnCnt ];
            
            STL_TRY( knlExprInit( sExprStack,
                                  sCodeExpr->mIncludeExprCnt,
                                  QLL_CODE_PLAN( aDBCStmt ),
                                  KNL_ENV( aEnv ) )
                     == STL_SUCCESS );

            
            /**
             * 각 Value에 대한 Expression Stack 구성
             */

            STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                                sCodeExpr,
                                                aSQLStmt->mBindContext,
                                                sExprStack,
                                                sConstExprStack,
                                                sTotalExprInfo,
                                                & sExprEntry,
                                                QLL_CODE_PLAN( aDBCStmt ),
                                                aEnv )
                     == STL_SUCCESS );

            
            /**
             * Value에 대한 DB type 설정
             */

            sColumnHandle = & sOptInsertStmt->mColumnHandle[ sColumnCnt ];

            STL_TRY( qloCastExprDBType( sCodeExpr,
                                        sTotalExprInfo,
                                        ellGetColumnDBType( sColumnHandle ),
                                        ellGetColumnPrecision( sColumnHandle ),
                                        ellGetColumnScale( sColumnHandle ),
                                        ellGetColumnStringLengthUnit( sColumnHandle ),
                                        ellGetColumnIntervalIndicator( sColumnHandle ),
                                        STL_FALSE,
                                        STL_TRUE,
                                        aEnv )
                     == STL_SUCCESS );

            sExprCount++;
        }
    }


    /****************************************************************
     * RETURN clause Expression Stack
     ****************************************************************/

    /**
     * RETURN Target절 Expression Code Stack 공간 할당
     */

    if( sOptInsertStmt->mReturnExprCnt > 0 )
    {
        sReturnTargetCount = sOptInsertStmt->mReturnExprCnt;
        sReturnTargetExprStack = sOptInsertStmt->mReturnExprStack;


        /**
         * RETURN Target절 Expression Code Stack 생성
         */

        STL_DASSERT( sOptInsertStmt->mReturnExprCnt ==
                     sOptInsertStmt->mReturnExprList->mCount );

        sReturnTargetItem = sOptInsertStmt->mReturnExprList->mHead;

        /**
         * Return 공간 확보
         */
        
        if ( sOptInsertStmt->mIntoTargetArray == NULL )
        {
            /********************************************
             * INSERT .. RETURNING Query
             ********************************************/

            /**
             * Return Target 정보 초기화 
             */
            
            STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                        STL_SIZEOF( qlvInitTarget ) * sReturnTargetCount,
                                        (void **) & sOptInsertStmt->mReturnTarget,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            sInitReturnPlan = (qlvInitInsertReturnInto *) QLL_GET_CACHED_INIT_PLAN( aSQLStmt->mCodePlan );
            
            if ( sInitReturnPlan->mIsInsertValues == STL_TRUE )
            {
                sInitReturnTarget = sInitReturnPlan->mInitInsertValues->mReturnTargetArray;
            }
            else
            {
                sInitReturnTarget = sInitReturnPlan->mInitInsertSelect->mReturnTargetArray;
            }
            
            stlMemcpy( sOptInsertStmt->mReturnTarget,
                       sInitReturnTarget,
                       STL_SIZEOF(qlvInitTarget) * sReturnTargetCount );
        }
        else
        {
            /********************************************
             * INSERT .. RETURNING INTO 
             ********************************************/

            sOptInsertStmt->mReturnTarget = NULL;
        }

        for( sLoop = 0; sLoop < sReturnTargetCount; sLoop++ )
        {
            sReturnTargetExpr = sReturnTargetItem->mExprPtr;

            /**
             * RETURN Target절 Expression Stack 생성
             */

            STL_TRY( knlExprInit( &sReturnTargetExprStack[ sLoop ],
                                  sReturnTargetExpr->mIncludeExprCnt,
                                  QLL_CODE_PLAN( aDBCStmt ),
                                  KNL_ENV( aEnv ) )
                     == STL_SUCCESS );


            /**
             * RETURN Target절 Expression Stack 구성
             */

            STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                                sReturnTargetExpr,
                                                aSQLStmt->mBindContext,
                                                &sReturnTargetExprStack[ sLoop ],
                                                aQueryExprList->mStmtExprList->mConstExprStack,
                                                aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                &sExprEntry,
                                                QLL_CODE_PLAN( aDBCStmt ),
                                                aEnv )
                     == STL_SUCCESS );

            if ( sOptInsertStmt->mIntoTargetArray == NULL )
            {
                /********************************************
                 * INSERT VALUES .. RETURNING Query
                 ********************************************/

                /**
                 * target 정보 변경 (validation 정보를 optimization 정보로 변경 
                 */
                
                sOptInsertStmt->mReturnTarget[ sLoop ].mExpr = sReturnTargetExpr;
            }
            else
            {
                /********************************************
                 * INSERT VALUES .. RETURNING INTO 
                 ********************************************/
            
                sIntoTarget = &(sOptInsertStmt->mIntoTargetArray[ sLoop ]);

                STL_TRY( knlGetBindParamType( aSQLStmt->mBindContext,
                                              sIntoTarget->mExpr.mBindParam->mBindParamIdx + 1,
                                              & sUserBindType,
                                              KNL_ERROR_STACK( aEnv ) )
                         == STL_SUCCESS );
        
                STL_TRY_THROW( (sUserBindType == KNL_BIND_TYPE_OUT) || (sUserBindType == KNL_BIND_TYPE_INOUT),
                               RAMP_BINDTYPE_MISMATCH );

                STL_TRY( knlGetParamValueBlock( aSQLStmt->mBindContext,
                                                sIntoTarget->mExpr.mBindParam->mBindParamIdx + 1,
                                                NULL,             /* IN  */
                                                &sValueBlockList, /* OUT */
                                                QLL_ERROR_STACK(aEnv) )
                         == STL_SUCCESS );
                                                
                sValueBlock = sValueBlockList->mValueBlock;
                
                STL_TRY_THROW( qloCastExprDBType( sReturnTargetItem->mExprPtr,
                                                  aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                  sValueBlock->mDataValue[0].mType,
                                                  sValueBlock->mArgNum1,
                                                  sValueBlock->mArgNum2,
                                                  sValueBlock->mStringLengthUnit,
                                                  sValueBlock->mIntervalIndicator,
                                                  STL_FALSE,
                                                  STL_FALSE,
                                                  aEnv )
                           == STL_SUCCESS,
                           RAMP_ERR_INVALID_RETURNING_CLAUSE );
            }
            
            sReturnTargetItem = sReturnTargetItem->mNext;
        }
    }


    /****************************************************************
     * INTO 절 Optimization
     ****************************************************************/

    /**
     * INTO 절의 Code Optimization : CAST 정보 구축
     * - 별도의 최적화나 정보 구축은 없음.
     * - 단, Bind Type 과 Target Type 이 호환되는 지 검사함.
     */


    /****************************************************************
     * Target Info 설정
     ****************************************************************/

    if( aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_QUERY_TYPE )
    {
        STL_TRY( qlfMakeQueryTarget( aSQLStmt,
                                     QLL_CODE_PLAN( aDBCStmt ),
                                     & sOptInsertStmt->mTargetInfo,
                                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sOptInsertStmt->mTargetInfo = NULL;
    }
    

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_RETURNING_CLAUSE )
    {
        if( aSQLStmt->mRetrySQL != NULL )
        {
            stlChar * sLastError = NULL;
            sLastError = stlGetLastDetailErrorMessage( QLL_ERROR_STACK(aEnv) );
        
            if( sLastError[0] == '\0' )
            {
                stlSetLastDetailErrorMessage(
                    QLL_ERROR_STACK(aEnv),
                    qlgMakeErrPosString( aSQLStmt->mRetrySQL,
                                         sReturnTargetItem->mExprPtr->mPosition,
                                         aEnv ) );
            }
        }
    }

    STL_CATCH( RAMP_BINDTYPE_MISMATCH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_BIND_TYPE_MISMATCH,
                      qlgMakeErrPosString( aSQLStmt->mRetrySQL,
                                           sIntoTarget->mPosition,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sIntoTarget->mExpr.mBindParam->mBindParamIdx + 1 );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief INSERT 구문의 All Expr 리스트 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 * @remark INSERT에 관련된 INSERT / INSERT SELECT / INSERT RETURN INTO 
 */
stlStatus qlnoAddExprInsertStmt( qllDBCStmt            * aDBCStmt,
                                 qllStatement          * aSQLStmt,
                                 qllOptNodeList        * aOptNodeList,
                                 qllOptimizationNode   * aOptNode,
                                 qloInitExprList       * aQueryExprList,
                                 qllEnv                * aEnv )
{
    qlnoInsertStmt       * sOptInsertStmt    = NULL;
    stlInt64               sRowCnt           = 0;
    stlInt32               sColCnt           = 0;
    stlInt64               sExprCnt          = 0;
    qlvRefExprItem       * sReturnTargetItem = NULL;
    qlvRefExprItem       * sExprItem         = NULL;
    qlvInitExpression    * sExpr             = NULL;


    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_STMT_INSERT_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );    


    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/

    sOptInsertStmt = (qlnoInsertStmt*)aOptNode->mOptNode;


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

    if( sOptInsertStmt->mHasQuery == STL_TRUE )
    {
        QLNO_ADD_EXPR( aDBCStmt,
                       aSQLStmt,
                       aOptNodeList,
                       sOptInsertStmt->mQueryNode,
                       aQueryExprList,
                       aEnv );
    }
    else
    {
        /* Do Nothing */
    }

    if( sOptInsertStmt->mInsertValueChildNode != NULL )
    {
        QLNO_ADD_EXPR( aDBCStmt,
                       aSQLStmt,
                       aOptNodeList,
                       sOptInsertStmt->mInsertValueChildNode,
                       aQueryExprList,
                       aEnv );
    }

    if( sOptInsertStmt->mReturnChildNode != NULL )
    {
        QLNO_ADD_EXPR( aDBCStmt,
                       aSQLStmt,
                       aOptNodeList,
                       sOptInsertStmt->mReturnChildNode,
                       aQueryExprList,
                       aEnv );
    }

    
    /****************************************************************
     * Add Expr
     ****************************************************************/

    /**
     * Add Expr Insert Values Expression
     */

    sExprCnt = 0;
    for( sRowCnt = 0 ; sRowCnt < sOptInsertStmt->mInsertRowCount ; sRowCnt++ )
    {
        for( sColCnt = 0 ; sColCnt < sOptInsertStmt->mInsertColumnCnt ; sColCnt++ )
        {
            if( sRowCnt == 0 )
            {
                STL_TRY( qloAddExpr(
                             sOptInsertStmt->mInsertValueExpr[ sExprCnt ],
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN(aDBCStmt),
                             aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                if( sOptInsertStmt->mInsertValueExpr[ sExprCnt ] ==
                    sOptInsertStmt->mInsertValueExpr[ sColCnt ] )
                {
                    /* Do Nothing */
                }
                else
                {
                    STL_TRY( qloAddExpr(
                                 sOptInsertStmt->mInsertValueExpr[ sExprCnt ],
                                 aQueryExprList->mStmtExprList->mAllExprList,
                                 QLL_CODE_PLAN(aDBCStmt),
                                 aEnv )
                             == STL_SUCCESS );
                }
            }

            sExprCnt++;
        }
    }


    /**
     * Add RETURN Target Expression
     * ------------------------
     *  RETURN Target의 경우 Table Node에서 읽은 Column을
     *  Inner Column으로 한번더 감싸고 있기때문에
     *  OrgExpr도 All Expression List에 넣어야 한다.
     */

    if( sOptInsertStmt->mReturnExprCnt > 0 )
    {
        sReturnTargetItem = sOptInsertStmt->mReturnExprList->mHead;
        while( sReturnTargetItem != NULL )
        {
            /**
             * RETURN Target Expression을 All Expression List에 추가
             */

            STL_TRY( qloAddExpr( sReturnTargetItem->mExprPtr,
                                 aQueryExprList->mStmtExprList->mAllExprList,
                                 QLL_CODE_PLAN( aDBCStmt ),
                                 aEnv )
                     == STL_SUCCESS );

            sReturnTargetItem = sReturnTargetItem->mNext;
        }
    }

    /**
     * Add Expr RowId Column
     */

    /* Do Nothing */

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} qlocInsert */









/**
 * @addtogroup qlodInsert
 * @{
 */


/**
 * @brief INSERT VALUES 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement 객체
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qloDataOptimizeInsertValues( smlTransId     aTransID,
                                       qllDBCStmt   * aDBCStmt,
                                       qllStatement * aSQLStmt,
                                       qllEnv       * aEnv )
{
    qlvInitInsertValues  * sInitPlan      = NULL;
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
    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_INSERT_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_INTO_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_QUERY_TYPE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mCodePlan != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**
     * 기본 정보 획득
     */

    STL_TRY( qlxInsertGetValidPlan( aTransID,
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
     * Block Allocation Count 설정 => Insert Row Count로 설정
     */
    
    sDataArea->mBlockAllocCnt = sOptInsertStmt->mInsertRowCount;


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
     * 전체 Pseudo Column 공간 확보 : Data Optimization 전에 수행 
     */

    STL_TRY( qlndAllocPseudoBlockList( aSQLStmt,
                                       sOptInsertStmt->mStmtExprList->mInitExprList->mPseudoColExprList,
                                       ( aSQLStmt->mIsAtomic == STL_FALSE
                                         ? sDataArea->mBlockAllocCnt
                                         : knlGetPropertyBigIntValueByID( KNL_PROPERTY_BLOCK_READ_COUNT,
                                                                          KNL_ENV(aEnv) ) * sDataArea->mBlockAllocCnt ),
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
                                            ( aSQLStmt->mIsAtomic == STL_FALSE
                                              ? sDataArea->mBlockAllocCnt
                                              : knlGetPropertyBigIntValueByID( KNL_PROPERTY_BLOCK_READ_COUNT,
                                                                               KNL_ENV(aEnv) ) * sDataArea->mBlockAllocCnt ),
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
                 ( aSQLStmt->mIsAtomic == STL_FALSE
                   ? sDataArea->mBlockAllocCnt
                   : knlGetPropertyBigIntValueByID( KNL_PROPERTY_BLOCK_READ_COUNT,
                                                    KNL_ENV(aEnv) ) * sDataArea->mBlockAllocCnt ),
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


