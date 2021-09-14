/*******************************************************************************
 * qloUpdate.c
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
 * @file qloUpdate.c
 * @brief SQL Processor Layer UPDATE statement optimization
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @addtogroup qlocUpdate
 * @{
 */



/**
 * @brief UPDATE 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement 
 * @param[in] aSQLStmt   SQL Statement 객체
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qloCodeOptimizeUpdate( smlTransId     aTransID,
                                 qllDBCStmt   * aDBCStmt,
                                 qllStatement * aSQLStmt,
                                 qllEnv       * aEnv )
{
    qlvInitUpdate         * sInitPlan       = NULL;
    qllOptimizationNode   * sOptNode        = NULL;
    qlnoUpdateStmt        * sOptUpdateStmt  = NULL;
    
    qloExprInfo           * sTotalExprInfo  = NULL;
    knlExprStack          * sConstExprStack = NULL;

    qllOptNodeList        * sOptNodeList    = NULL;
    qllEstimateCost       * sOptCost        = NULL;

    qloInitStmtExprList   * sStmtExprList   = NULL;
    qloInitExprList       * sQueryExprList  = NULL;
    qlvInitExprList       * sInitExprList   = NULL;
    
    qlvRefExprItem        * sExprItem       = NULL;
    stlUInt64               sExprCnt        = 0;

    stlInt32                sLoop                   = 0;

    qlvRefExprList        * sReturnTargetColList    = NULL;

    stlInt32                sReturnTargetCount      = 0;
    knlExprStack          * sReturnTargetExprStack  = NULL;

    qlvInitExpression     * sExpr           = NULL;
    knlExprEntry          * sExprEntry      = NULL;
    qloDBType             * sDBType         = NULL;

    qlvInitExpression     * sWriteTableRowIdColumn   = NULL;    
   
    qlncUpdateStmtParamInfo   sUpdateStmtParamInfo;
    qloCodeOptParamInfo       sCodeOptParamInfo;

    qllBindParamInfo      * sBindParamInfo  = NULL;
    stlInt32                sInternalBindIdx = 0;
    
    QLL_OPT_DECLARE( stlTime sBeginTime );
    QLL_OPT_DECLARE( stlTime sEndTime );

    
    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_UPDATE_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_UPDATE_WHERE_CURRENT_OF_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_UPDATE_RETURNING_INTO_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_UPDATE_RETURNING_QUERY_TYPE),
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

    STL_TRY( qlxUpdateGetValidPlan( aTransID,
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
     * UPDATE STATEMENT Optimization Node 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlnoUpdateStmt ),
                                (void **) & sOptUpdateStmt,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sOptUpdateStmt, 0x00, STL_SIZEOF( qlnoUpdateStmt ) );

    
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
                                QLL_PLAN_NODE_STMT_UPDATE_TYPE,
                                sOptUpdateStmt,
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
     * Query Optimization
     */

    sUpdateStmtParamInfo.mStmtNode = (qlvInitNode *) sInitPlan;
    sUpdateStmtParamInfo.mOutReturnTargetArray = NULL;
    sUpdateStmtParamInfo.mOutSetOuterColumnList = NULL;
    sUpdateStmtParamInfo.mOutReturnOuterColumnList = NULL;
    sUpdateStmtParamInfo.mOutSetExprArray = NULL;
    sUpdateStmtParamInfo.mOutSetColExprList = NULL;
    sUpdateStmtParamInfo.mOutShareReadColList = NULL;
    sUpdateStmtParamInfo.mOutShareWriteColList = NULL;
    sUpdateStmtParamInfo.mOutPlanRootNode = NULL;
    sUpdateStmtParamInfo.mOutPlanReadTable = NULL;
    sUpdateStmtParamInfo.mOutPlanWriteTable = NULL;
    sUpdateStmtParamInfo.mOutPlanSetSubQueryList = NULL;
    sUpdateStmtParamInfo.mOutPlanReturnSubQueryList = NULL;
    sUpdateStmtParamInfo.mOutStmtNode = NULL;

    sInternalBindIdx = sInitPlan->mInitPlan.mInternalBindCount;
    
    QLL_OPT_CHECK_TIME( sBeginTime );

    STL_TRY( qlncUpdateOptimizer( aTransID,
                                  aDBCStmt,
                                  aSQLStmt,
                                  sQueryExprList,
                                  &sInternalBindIdx,
                                  &sUpdateStmtParamInfo,
                                  aEnv )
             == STL_SUCCESS );

    QLL_OPT_CHECK_TIME( sEndTime );
    QLL_OPT_ADD_ELAPSED_TIME( aSQLStmt->mOptInfo.mOptimizerTime, sBeginTime, sEndTime );
    QLL_OPT_ADD_COUNT( aSQLStmt->mOptInfo.mOptimizerCallCount, 1 );


    /**
     * Init Plan 연결 재설정
     */

    sInitPlan = (qlvInitUpdate *) sUpdateStmtParamInfo.mOutStmtNode;


    /**
     * Valid Table Statistic List
     */

    sOptUpdateStmt->mValidTblStatList = sUpdateStmtParamInfo.mOutValidTblStatList;


    /**
     * Code Optimization
     */

    sCodeOptParamInfo.mDBCStmt = aDBCStmt;
    sCodeOptParamInfo.mSQLStmt = aSQLStmt;
    sCodeOptParamInfo.mQueryExprList = sQueryExprList;
    sCodeOptParamInfo.mOptNodeList = sOptNodeList;
    sCodeOptParamInfo.mOptCost = sOptCost;
    sCodeOptParamInfo.mOptQueryNode = NULL;
    sCodeOptParamInfo.mCandPlan = sUpdateStmtParamInfo.mOutPlanRootNode;
    sCodeOptParamInfo.mOptNode = NULL;
    sCodeOptParamInfo.mRootQuerySetIdx = 0;
    sCodeOptParamInfo.mRootQuerySet = NULL;

    QLNO_CODE_OPTIMIZE( &sCodeOptParamInfo, aEnv );

    sOptUpdateStmt->mScanNode = sCodeOptParamInfo.mOptNode;


    /**
     * Return 공간 확보
     */

    if ( sInitPlan->mIntoTargetArray == NULL )
    {
        sOptUpdateStmt->mReturnTarget = sUpdateStmtParamInfo.mOutReturnTargetArray;
    }
    else
    {
        sOptUpdateStmt->mReturnTarget = NULL;
    }


    /****************************************************************
     * Trace Optimizer 종료
     ****************************************************************/

    QLL_SET_DISABLE_TRACE_LOGGER(aEnv);


    /****************************************************************
     * SET & RETURN clause Expression
     ****************************************************************/

    /**
     * SET clause Expression
     */

    sOptUpdateStmt->mSetColumnCnt   = sInitPlan->mSetColumnCnt;
    sOptUpdateStmt->mSetExprArray   = sUpdateStmtParamInfo.mOutSetExprArray;

    /* SubQuery Expression List가 존재하는 경우 처리 */
    if( sUpdateStmtParamInfo.mOutPlanSetSubQueryList != NULL )
    {
        sCodeOptParamInfo.mDBCStmt = aDBCStmt;
        sCodeOptParamInfo.mSQLStmt = aSQLStmt;
        sCodeOptParamInfo.mQueryExprList = sQueryExprList;
        sCodeOptParamInfo.mOptNodeList = sOptNodeList;
        sCodeOptParamInfo.mOptCost = sOptCost;
        sCodeOptParamInfo.mOptQueryNode = NULL;
        sCodeOptParamInfo.mCandPlan = sUpdateStmtParamInfo.mOutPlanSetSubQueryList;
        sCodeOptParamInfo.mOptNode = NULL;
        sCodeOptParamInfo.mRootQuerySetIdx = 0;
        sCodeOptParamInfo.mRootQuerySet = NULL;

        QLNO_CODE_OPTIMIZE( &sCodeOptParamInfo, aEnv );

        sOptUpdateStmt->mSetChildNode = sCodeOptParamInfo.mOptNode;

        /* optimizer에서 이미 할당되었음을 보장해야 한다. */
        STL_DASSERT( sUpdateStmtParamInfo.mOutSetOuterColumnList != NULL );
        sOptUpdateStmt->mSetOuterColumnList = sUpdateStmtParamInfo.mOutSetOuterColumnList;
    }
    else
    {
        sOptUpdateStmt->mSetChildNode = NULL;
        sOptUpdateStmt->mSetOuterColumnList = NULL;
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
                         sUpdateStmtParamInfo.mOutReturnTargetArray[ sLoop ].mExpr,
                         STL_FALSE,
                         QLL_CODE_PLAN( aDBCStmt ),
                         aEnv )
                     == STL_SUCCESS );
        }

        /* SubQuery Expression List가 존재하는 경우 처리 */
        if( sUpdateStmtParamInfo.mOutPlanReturnSubQueryList != NULL )
        {
            sCodeOptParamInfo.mDBCStmt = aDBCStmt;
            sCodeOptParamInfo.mSQLStmt = aSQLStmt;
            sCodeOptParamInfo.mQueryExprList = sQueryExprList;
            sCodeOptParamInfo.mOptNodeList = sOptNodeList;
            sCodeOptParamInfo.mOptCost = sOptCost;
            sCodeOptParamInfo.mOptQueryNode = NULL;
            sCodeOptParamInfo.mCandPlan = sUpdateStmtParamInfo.mOutPlanReturnSubQueryList;
            sCodeOptParamInfo.mOptNode = NULL;
            sCodeOptParamInfo.mRootQuerySetIdx = 0;
            sCodeOptParamInfo.mRootQuerySet = NULL;

            QLNO_CODE_OPTIMIZE( &sCodeOptParamInfo, aEnv );

            sOptUpdateStmt->mReturnChildNode = sCodeOptParamInfo.mOptNode;

            /* optimizer에서 이미 할당되었음을 보장해야 한다. */
            STL_DASSERT( sUpdateStmtParamInfo.mOutReturnOuterColumnList != NULL );
            sOptUpdateStmt->mReturnOuterColumnList = sUpdateStmtParamInfo.mOutReturnOuterColumnList;
        }
        else
        {
            sOptUpdateStmt->mReturnChildNode = NULL;
            sOptUpdateStmt->mReturnOuterColumnList = NULL;
        }
    }
    else
    {
        sReturnTargetColList = NULL;
        sOptUpdateStmt->mReturnChildNode = NULL;
        sOptUpdateStmt->mReturnOuterColumnList = NULL;
    }

    sOptUpdateStmt->mReturnExprCnt   = sInitPlan->mReturnTargetCnt;
    sOptUpdateStmt->mReturnExprList  = sReturnTargetColList;


    /****************************************************************
     * Add Expression 수행
     ****************************************************************/

    /**
     * Returning절에 올 수 있는 RowId Column을 처리하기 위해
     * Statement Expression List에 Read RowId Column 추가
     */

    sWriteTableRowIdColumn = ((qlvInitBaseTableNode*)(sUpdateStmtParamInfo.mOutPlanWriteTable->mInitNode))->mRefRowIdColumn;

    if( sWriteTableRowIdColumn != NULL )
    {
        /**
         * 예) UPDAE T1 SET I1 = 1
         *     WHERE I2 = 3
         *     RETURNING NEW ROWID INTO :V1;
         *               ---------
         */

        STL_TRY( qlvAddExprToRefExprList( sStmtExprList->mRowIdColumnExprList,
                                          sWriteTableRowIdColumn,
                                          STL_FALSE,
                                          QLL_CODE_PLAN( aDBCStmt ),
                                          aEnv )
                 == STL_SUCCESS );
    }
    
    /**
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
     * UPDATE STATEMENT Optimization Node 정보 설정
     ****************************************************************/

    /**
     * Total Optimization Node List
     * => Statement에 사용되는 모든 Optimization Node List
     */

    sOptUpdateStmt->mOptNodeList = sOptNodeList;

    
    /**
     * Init Plan Node
     */
    
    sOptUpdateStmt->mCachedInitNode = (void *) sUpdateStmtParamInfo.mOutStmtNode;


    /**
     * Update Values 관련 정보 설정
     */
    
    /* sOptUpdateStmt->mSetColumnCnt   = sInitPlan->mSetColumnCnt; */
    /* sOptUpdateStmt->mSetExprArray   = sIniSetExpr; */
    sOptUpdateStmt->mSetColExprList = sUpdateStmtParamInfo.mOutSetColExprList;


    /**
     * Table 의 전체 Column Count 만큼 Array 로 관리됨
     */
    
    sOptUpdateStmt->mTotalColumnCnt  = sInitPlan->mTotalColumnCnt;
    sOptUpdateStmt->mSetCodeStackIdx = sInitPlan->mSetCodeStackIdx;


    /**
     * Table 관련
     */
    
    STL_TRY( qlnoAllocAndCopyString( ellGetTableName( & sInitPlan->mTableHandle ),
                                     & sOptUpdateStmt->mTableName,
                                     QLL_CODE_PLAN( aDBCStmt ),
                                     aEnv )
             == STL_SUCCESS );

    sOptUpdateStmt->mSetColumnHandle = sInitPlan->mSetColumnHandle;
    sOptUpdateStmt->mPhysicalHandle = sInitPlan->mTablePhyHandle;
    sOptUpdateStmt->mTableHandle = & sInitPlan->mTableHandle;

    
    /**
     * Scan Node Idx
     */
    
    /* sOptUpdateStmt->mScanNodeIdx = sOptScanNode->mIdx; */
    
    
    /**
     * Value Block 관련
     */

    sOptUpdateStmt->mStmtExprList  = sStmtExprList;
    sOptUpdateStmt->mQueryExprList = sQueryExprList;

    sOptUpdateStmt->mRowIdColumn = sWriteTableRowIdColumn;

    /**
     * Update 대상 Table 관련
     */
    
    sOptUpdateStmt->mReadTableNode  = sUpdateStmtParamInfo.mOutPlanReadTable->mInitNode;
    sOptUpdateStmt->mWriteTableNode = sUpdateStmtParamInfo.mOutPlanWriteTable->mInitNode;

    
    /**
     * Related Object 관련
     */
    
    sOptUpdateStmt->mHasCheck  = sInitPlan->mHasCheck;
    sOptUpdateStmt->mHasIndex  = sInitPlan->mHasIndex;
    sOptUpdateStmt->mRelObject = & sInitPlan->mRelObject;

    
    /**
     * OFFSET .. LIMIT 절 관련
     */
    
    sOptUpdateStmt->mSkipCnt     = sInitPlan->mSkipCnt;
    sOptUpdateStmt->mFetchCnt    = sInitPlan->mFetchCnt;
    sOptUpdateStmt->mResultSkip  = sInitPlan->mResultSkip;
    sOptUpdateStmt->mResultLimit = sInitPlan->mResultLimit;
    
    
    /**
     * RETURN INTO 절 관련
     */
    
    /* sOptUpdateStmt->mReturnExprCnt   = 0; */
    /* sOptUpdateStmt->mReturnExprList  = NULL; */
    sOptUpdateStmt->mIntoTargetArray = sInitPlan->mIntoTargetArray;


    /**
     *  SET 절 Expression Stack: array
     */
    
    sOptUpdateStmt->mSetCodeStack = NULL;

    
    /**
     *  RETURN clause Expression Stack
     */
    
    sOptUpdateStmt->mReturnExprStack   = sReturnTargetExprStack;
    sOptUpdateStmt->mShareReadColList  = sUpdateStmtParamInfo.mOutShareReadColList;
    sOptUpdateStmt->mShareWriteColList = sUpdateStmtParamInfo.mOutShareWriteColList;

    
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

    /**
     * Node List를 순차적으로 순회하면서 추가 정보들을 구성함
     */
    
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
 * @brief UPDATE 구문의 Optimization Node 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoCompleteUpdateStmt( qllDBCStmt            * aDBCStmt,
                                  qllStatement          * aSQLStmt,
                                  qllOptNodeList        * aOptNodeList,
                                  qllOptimizationNode   * aOptNode,
                                  qloInitExprList       * aQueryExprList,
                                  qllEnv                * aEnv )
{
    qlvInitUpdate        * sInitPlan           = NULL;
    
    qlvInitExpression    * sExpr               = NULL;

    qloInitStmtExprList  * sStmtExprList       = NULL;
    qloExprInfo          * sTotalExprInfo      = NULL;
    knlExprStack         * sConstExprStack     = NULL;

    qlnoUpdateStmt       * sOptUpdateStmt      = NULL;
    
    knlExprEntry         * sExprEntry          = NULL;

    stlInt32               sSetIdx             = 0;
    knlExprStack         * sSetExprStack       = NULL;
    ellDictHandle        * sColumnHandle       = NULL;

    knlBindType           sUserBindType             = KNL_BIND_TYPE_INVALID;
    stlInt32              sLoop                     = 0;

    stlInt32              sReturnTargetCount        = 0;
    knlExprStack        * sReturnTargetExprStack    = NULL;
    qlvRefExprItem      * sReturnTargetItem         = NULL;
    qlvInitExpression   * sReturnTargetExpr         = NULL;

    qlvInitExpression   * sIntoTarget               = NULL;
    knlValueBlockList   * sValueBlockList           = NULL;
    knlValueBlock       * sValueBlock               = NULL;


    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_STMT_UPDATE_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );    



    /**********************************************************
     * 기본 정보 획득
     **********************************************************/

    /**
     * UPDATE STATEMENT Node 획득
     */

    sInitPlan       = (qlvInitUpdate *) QLL_GET_CACHED_INIT_PLAN( aSQLStmt->mCodePlan );
    sOptUpdateStmt  = (qlnoUpdateStmt *) aOptNode->mOptNode;

    
    /**
     * Expreesion에 대한 Optimization 정보 획득
     */

    sStmtExprList   = sOptUpdateStmt->mStmtExprList;
    sTotalExprInfo  = sStmtExprList->mTotalExprInfo;
    sConstExprStack = sStmtExprList->mConstExprStack;


    /****************************************************************
     * 하위 node에 대한 Complete Node 수행
     ****************************************************************/

    QLNO_COMPLETE( aDBCStmt,
                   aSQLStmt,
                   aOptNodeList,
                   sOptUpdateStmt->mScanNode,
                   aQueryExprList,
                   aEnv );

    if( sOptUpdateStmt->mSetChildNode != NULL )
    {
        QLNO_COMPLETE( aDBCStmt,
                       aSQLStmt,
                       aOptNodeList,
                       sOptUpdateStmt->mSetChildNode,
                       aQueryExprList,
                       aEnv );
    }

    if( sOptUpdateStmt->mReturnChildNode != NULL )
    {
        QLNO_COMPLETE( aDBCStmt,
                       aSQLStmt,
                       aOptNodeList,
                       sOptUpdateStmt->mReturnChildNode,
                       aQueryExprList,
                       aEnv );
    }

    
    /**********************************************************
     * SET 절 Expression Stack 구성 
     **********************************************************/

    /**
     * SET 절 Expression Stack 공간 할당
     */
    
    STL_TRY( knlAllocRegionMem(
                 QLL_CODE_PLAN( aDBCStmt ),
                 STL_SIZEOF( knlExprStack ) * sOptUpdateStmt->mSetColumnCnt,
                 (void **) & sOptUpdateStmt->mSetCodeStack,
                 KNL_ENV( aEnv ) )
             == STL_SUCCESS );


    /**
     * SET 절 Expression Stack 구성
     */

    for( sSetIdx = 0 ; sSetIdx < sOptUpdateStmt->mSetColumnCnt ; sSetIdx++ )

    {
        sExpr = sOptUpdateStmt->mSetExprArray[ sSetIdx ];
        sSetExprStack = & sOptUpdateStmt->mSetCodeStack[ sSetIdx ];
            
        /**
         * 각 SET Expression 에 대한 Expression Stack 생성
         */
        
        STL_TRY( knlExprInit( sSetExprStack,
                              sExpr->mIncludeExprCnt,
                              QLL_CODE_PLAN( aDBCStmt ),
                              KNL_ENV( aEnv ) )
                 == STL_SUCCESS );

        
        /**
         * 각 Value에 대한 Expression Stack 구성
         */
        
        STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                            sExpr,
                                            aSQLStmt->mBindContext,
                                            sSetExprStack,
                                            sConstExprStack,
                                            sTotalExprInfo,
                                            & sExprEntry,
                                            QLL_CODE_PLAN( aDBCStmt ),
                                            aEnv )
                 == STL_SUCCESS );
        
            
        /**
         * SET column 대한 DB type 설정
         */

        sColumnHandle = & sOptUpdateStmt->mSetColumnHandle[ sSetIdx ];
        
        STL_TRY( qloCastExprDBType( sExpr,
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
    }


    /****************************************************************
     * RETURN clause Expression Stack
     ****************************************************************/

    /**
     * RETURN Target절 Expression Code Stack 공간 할당
     */

    if( sOptUpdateStmt->mReturnExprCnt > 0 )
    {
        sReturnTargetCount = sOptUpdateStmt->mReturnExprCnt;
        sReturnTargetExprStack = sOptUpdateStmt->mReturnExprStack;


        /**
         * RETURN Target절 Expression Code Stack 생성
         */

        STL_DASSERT( sOptUpdateStmt->mReturnExprCnt ==
                     sOptUpdateStmt->mReturnExprList->mCount );

        sReturnTargetItem = sOptUpdateStmt->mReturnExprList->mHead;

        /**
         * Return 공간 확보
         */

        if ( sOptUpdateStmt->mIntoTargetArray == NULL )
        {
            /********************************************
             * UPDATE RETURNING Query
             ********************************************/

            /**
             * Return Target 정보 초기화 
             */
            
            STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                        STL_SIZEOF( qlvInitTarget ) * sReturnTargetCount,
                                        (void **) & sOptUpdateStmt->mReturnTarget,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            stlMemcpy( sOptUpdateStmt->mReturnTarget,
                       sInitPlan->mReturnTargetArray,
                       STL_SIZEOF(qlvInitTarget) * sReturnTargetCount );
        }
        else
        {
            /********************************************
             * UPDATE RETURNING INTO 
             ********************************************/

            sOptUpdateStmt->mReturnTarget = NULL;
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

            if ( sOptUpdateStmt->mIntoTargetArray == NULL )
            {
                /********************************************
                 * UPDATE RETURNING Query
                 ********************************************/

                /**
                 * target 정보 변경 (validation 정보를 optimization 정보로 변경 
                 */
                
                sOptUpdateStmt->mReturnTarget[ sLoop ].mExpr = sReturnTargetExpr;
            }
            else
            {
                /********************************************
                 * UPDATE RETURNING INTO 
                 ********************************************/
                
                sIntoTarget = &(sOptUpdateStmt->mIntoTargetArray[ sLoop ]);

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

    if( aSQLStmt->mStmtType == QLL_STMT_UPDATE_RETURNING_QUERY_TYPE )
    {
        STL_TRY( qlfMakeQueryTarget( aSQLStmt,
                                     QLL_CODE_PLAN( aDBCStmt ),
                                     & sOptUpdateStmt->mTargetInfo,
                                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sOptUpdateStmt->mTargetInfo = NULL;
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
 * @brief UPDATE 구문의 All Expr 리스트 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoAddExprUpdateStmt( qllDBCStmt            * aDBCStmt,
                                 qllStatement          * aSQLStmt,
                                 qllOptNodeList        * aOptNodeList,
                                 qllOptimizationNode   * aOptNode,
                                 qloInitExprList       * aQueryExprList,
                                 qllEnv                * aEnv )
{
    stlInt32               i                 = 0;
    qlnoUpdateStmt       * sOptUpdateStmt    = NULL;
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
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_STMT_UPDATE_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );    


    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/

    sOptUpdateStmt = (qlnoUpdateStmt*)aOptNode->mOptNode;


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
                   sOptUpdateStmt->mScanNode,
                   aQueryExprList,
                   aEnv );

    if( sOptUpdateStmt->mSetChildNode != NULL )
    {
        QLNO_ADD_EXPR( aDBCStmt,
                       aSQLStmt,
                       aOptNodeList,
                       sOptUpdateStmt->mSetChildNode,
                       aQueryExprList,
                       aEnv );
    }

    if( sOptUpdateStmt->mReturnChildNode != NULL )
    {
        QLNO_ADD_EXPR( aDBCStmt,
                       aSQLStmt,
                       aOptNodeList,
                       sOptUpdateStmt->mReturnChildNode,
                       aQueryExprList,
                       aEnv );
    }


    /****************************************************************
     * Add Expr
     ****************************************************************/

    /**
     * Add Expr SET clause Expression
     */

    for( i = 0; i < sOptUpdateStmt->mSetColumnCnt; i++ )
    {
        STL_TRY( qloAddExpr( sOptUpdateStmt->mSetExprArray[i],
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN(aDBCStmt),
                             aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Add RETURN Target Expression
     * ------------------------
     *  RETURN Target의 경우 Table Node에서 읽은 Column을
     *  Inner Column으로 한번더 감싸고 있기때문에
     *  OrgExpr도 All Expression List에 넣어야 한다.
     */

    if( sOptUpdateStmt->mReturnExprCnt > 0 )
    {
        sReturnTargetItem = sOptUpdateStmt->mReturnExprList->mHead;
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

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlocUpdate */







/**
 * @addtogroup qlodUpdate
 * @{
 */


/**
 * @brief UPDATE 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement 객체
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qloDataOptimizeUpdate( smlTransId     aTransID,
                                 qllDBCStmt   * aDBCStmt,
                                 qllStatement * aSQLStmt,
                                 qllEnv       * aEnv )
{
    qlvInitUpdate        * sInitPlan      = NULL;
    qllOptimizationNode  * sOptNode       = NULL;
    qllOptimizationNode  * sNode          = NULL;
    qlnoUpdateStmt       * sOptUpdateStmt = NULL;
    qllDataArea          * sDataArea      = NULL;
    
    qlvRefExprItem       * sReadColItem   = NULL;
    qlvRefExprItem       * sWriteColItem  = NULL;

    knlValueBlockList    * sReadValue     = NULL;
    knlValueBlockList    * sWriteValue    = NULL;

    knlValueBlockList    * sTargetBlock   = NULL;
    stlInt32               sTargetIdx = 0;
    
    qloDataOptExprInfo     sDataOptInfo;


    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_UPDATE_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_UPDATE_WHERE_CURRENT_OF_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_UPDATE_RETURNING_INTO_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_UPDATE_RETURNING_QUERY_TYPE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mCodePlan != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**
     * 기본 정보 획득
     */

    STL_TRY( qlxUpdateGetValidPlan( aTransID,
                                    aDBCStmt,
                                    aSQLStmt,
                                    QLL_PHASE_DATA_OPTIMIZE,
                                    & sInitPlan,
                                    & sOptNode,
                                    NULL,
                                    aEnv )
             == STL_SUCCESS );

    
    /**
     * UPDATE STATEMENT Optimization Node 획득
     */


    sOptUpdateStmt = (qlnoUpdateStmt *) sOptNode->mOptNode;


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

    if ( aSQLStmt->mStmtType == QLL_STMT_UPDATE_WHERE_CURRENT_OF_TYPE )
    {
        /**
         * RID 를 이용한 Fetch Record 를 수행함
         */

        sDataArea->mBlockAllocCnt = KNL_NO_BLOCK_READ_CNT; 
    }
    else
    {
        /**
         * Iterator 를 이용한 Fetch Next 를 수행함
         */
        sDataArea->mBlockAllocCnt = knlGetPropertyBigIntValueByID( KNL_PROPERTY_BLOCK_READ_COUNT,
                                                                   KNL_ENV(aEnv) );
    }


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
    
    sDataArea->mExecNodeCnt = sOptUpdateStmt->mOptNodeList->mCount;

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

    STL_DASSERT( sOptUpdateStmt->mStmtExprList->mTotalExprInfo != NULL );
    STL_TRY( knlCreateContextInfo( sOptUpdateStmt->mStmtExprList->mTotalExprInfo->mExprCnt,
                                   & sDataArea->mExprDataContext,
                                   & QLL_DATA_PLAN( aDBCStmt ),
                                   KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Data Optimization Info 설정
     */

    sDataOptInfo.mStmtExprList      = sOptUpdateStmt->mStmtExprList;
    sDataOptInfo.mDataArea          = sDataArea;
    sDataOptInfo.mSQLStmt           = aSQLStmt;
    sDataOptInfo.mWriteRelationNode = NULL;
    sDataOptInfo.mBindContext       = aSQLStmt->mBindContext;
    sDataOptInfo.mRegionMem         = & QLL_DATA_PLAN( aDBCStmt );


    /**
     * 전제 Pseudo Column 공간 확보 : Data Optimization 전에 수행 
     */

    STL_TRY( qlndAllocPseudoBlockList( aSQLStmt,
                                       sOptUpdateStmt->mStmtExprList->mInitExprList->mPseudoColExprList,
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
                                            sOptUpdateStmt->mStmtExprList->mRowIdColumnExprList,
                                            sDataArea->mBlockAllocCnt,
                                            & QLL_DATA_PLAN( aDBCStmt ),
                                            aEnv )
             == STL_SUCCESS );

    
    /**
     * Read RowId Column Block 과 Write RowId Column Block을 공유함
     */

    STL_TRY( qlndShareRowIdColumnBlockListForUpdate( sDataArea->mRowIdColumnBlock,
                                                     sOptUpdateStmt->mReadTableNode,
                                                     sOptUpdateStmt->mWriteTableNode,
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
                 sOptUpdateStmt->mStmtExprList->mColumnExprList,
                 sDataArea->mBlockAllocCnt,
                 aEnv )
             == STL_SUCCESS );


    /**
     * Write Column Block 할당
     * @remark UPDATE STATEMENT는 Write Column Block으로
     *    <BR> Update Values 대상 Column을 설정한다.
     */

    STL_TRY( qlndCreateColumnBlockList( & sDataArea->mWriteColumnBlock,
                                        & QLL_DATA_PLAN( aDBCStmt ),
                                        aEnv )
             == STL_SUCCESS );

    STL_TRY( qlndAddColumnListToColumnBlockList(
                 aSQLStmt,
                 & sDataOptInfo,
                 sDataArea->mWriteColumnBlock,
                 ((qlvInitBaseTableNode *) sOptUpdateStmt->mWriteTableNode)->mRefColumnList,
                 sDataArea->mBlockAllocCnt,
                 aEnv )
             == STL_SUCCESS );

    STL_DASSERT( sDataArea->mWriteColumnBlock->mHead != NULL );
    STL_DASSERT( sDataArea->mWriteColumnBlock->mHead->mNextRelation == NULL );


    /**
     * Share Column Block 연결
     */

    sReadColItem  = sOptUpdateStmt->mShareReadColList->mHead;
    sWriteColItem = sOptUpdateStmt->mShareWriteColList->mHead;
    
    while( sReadColItem != NULL )
    {
        STL_DASSERT( sWriteColItem != NULL );

        /* read column value block 얻기 */
        STL_TRY( qlndFindColumnBlockList( sDataArea,
                                          sDataArea->mReadColumnBlock,
                                          sReadColItem->mExprPtr,
                                          & QLL_DATA_PLAN( aDBCStmt ),
                                          & sReadValue,
                                          aEnv )
                 == STL_SUCCESS );
        STL_DASSERT( sReadValue != NULL );
    
        /* write column value block 얻기 */
        STL_TRY( qlndFindColumnBlockList( sDataArea,
                                          sDataArea->mWriteColumnBlock,
                                          sWriteColItem->mExprPtr,
                                          & QLL_DATA_PLAN( aDBCStmt ),
                                          & sWriteValue,
                                          aEnv )
                 == STL_SUCCESS );
        STL_DASSERT( sWriteValue != NULL );

        /* read & write column value block의 Value Block 연결 */
        sReadValue->mValueBlock = sWriteValue->mValueBlock;
        
        sReadColItem  = sReadColItem->mNext;
        sWriteColItem = sWriteColItem->mNext;
    }
    
    
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

    STL_DASSERT( ((qlvInitBaseTableNode *) sOptUpdateStmt->mWriteTableNode)->mRefColumnList->mHead->mExprPtr->mType ==
                 QLV_EXPR_TYPE_COLUMN );

    sDataOptInfo.mWriteRelationNode = 
        ((qlvInitBaseTableNode *) sOptUpdateStmt->mWriteTableNode)->mRefColumnList->mHead->mExprPtr->mExpr.mColumn->mRelationNode;

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

    if( sOptUpdateStmt->mReturnExprCnt > 0 )
    {
        if ( aSQLStmt->mStmtType == QLL_STMT_UPDATE_RETURNING_QUERY_TYPE )
        {
            /**
             * RETURNING Query
             */

            /**
             * RETURNING Fetch 를 위한 Target Block 구성
             */
            
            STL_TRY( qlndBuildRefBlockList( aSQLStmt,
                                            sDataArea,
                                            sOptUpdateStmt->mStmtExprList, 
                                            sOptUpdateStmt->mReturnExprList,
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
            /**
             * RETURNING INTO
             */

            STL_ASSERT(aSQLStmt->mStmtType == QLL_STMT_UPDATE_RETURNING_INTO_TYPE);

            STL_TRY( qlndDataOptIntoPhrase( aSQLStmt,
                                            sOptUpdateStmt->mStmtExprList->mTotalExprInfo,
                                            aSQLStmt->mBindContext,
                                            sOptUpdateStmt->mReturnExprCnt,
                                            sOptUpdateStmt->mReturnExprList,
                                            sOptUpdateStmt->mIntoTargetArray,
                                            sDataArea,
                                            &QLL_DATA_PLAN( aDBCStmt ),
                                            aEnv )
                     == STL_SUCCESS );
        }
    }
    else
    {
        /**
         * RETURNING 구문이 없는 UPDATE
         */

        sDataArea->mTargetBlock = NULL;
        sDataArea->mIntoBlock   = NULL;
    }


    /****************************************************************
     * Data Optimization For Execution Nodes
     ****************************************************************/

    /**
     * Execution Node에 대한 Data Optimization 수행
     */

    sNode = sOptUpdateStmt->mOptNodeList->mHead->mNode;
    STL_TRY( qlndDataOptimizeUpdateStmt( aTransID,
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


/** @} qlodUpdate */


