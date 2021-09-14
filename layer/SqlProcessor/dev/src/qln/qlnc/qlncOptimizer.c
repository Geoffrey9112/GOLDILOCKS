/*******************************************************************************
 * qlncOptimizer.c
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
 * @file qlncOptimizer.c
 * @brief SQL Processor Layer SELECT statement optimization
 */

#include <qll.h>
#include <qlDef.h>



/**
 * @addtogroup qlnc
 * @{
 */


/**
 * @brief Query Node의 Candidate Plan을 구한다.
 * @param[in]       aTransID            Transaction ID
 * @param[in]       aDBCStmt            DBC Statement
 * @param[in]       aSQLStmt            SQL Statement 객체
 * @param[in]       aQueryNode          Validation Query Node
 * @param[in]       aInitStmtExprList   Init Statement Expression List
 * @param[in]       aTableStatList      Table Stat List
 * @param[in,out]   aInternalBindIdx    Internal Bind Idx
 * @param[out]      aCandQueryPlan      Candidate Query Plan
 * @param[in]       aEnv                Environment
 * @remarks
 */
stlStatus qlncQueryOptimizer( smlTransId              aTransID,
                              qllDBCStmt            * aDBCStmt,
                              qllStatement          * aSQLStmt,
                              qlvInitNode           * aQueryNode,
                              qlvInitStmtExprList   * aInitStmtExprList,
                              qloValidTblStatList   * aTableStatList,
                              stlInt32              * aInternalBindIdx,
                              qlncPlanCommon       ** aCandQueryPlan,
                              qllEnv                * aEnv )
{
    stlInt32                  sGlobalID;
    qlncParamInfo             sParamInfo;
    qlncCreateNodeParamInfo   sCreateNodeParamInfo;

    qlncQBMapArray          * sQBMapArray       = NULL;
    qlncNodeCommon          * sCandQueryNode    = NULL;
    qlncPlanCommon          * sCandQueryPlan    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aSQLStmt->mStmtType == QLL_STMT_SELECT_TYPE ) ||
                        ( aSQLStmt->mStmtType == QLL_STMT_SELECT_FOR_UPDATE_TYPE ) ||
                        ( aSQLStmt->mStmtType == QLL_STMT_SELECT_INTO_TYPE ) ||
                        ( aSQLStmt->mStmtType == QLL_STMT_SELECT_INTO_FOR_UPDATE_TYPE ) ||
                        ( aSQLStmt->mStmtType == QLL_STMT_INSERT_TYPE ) ||
                        ( aSQLStmt->mStmtType == QLL_STMT_INSERT_SELECT_TYPE ) ||
                        ( aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_QUERY_TYPE ) ||
                        ( aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_INTO_TYPE ) ||
                        ( aSQLStmt->mStmtType == QLL_STMT_CREATE_TABLE_AS_SELECT_TYPE ),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInternalBindIdx != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableStatList != NULL, QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Break Point 설정
     ****************************************************************/

    KNL_BREAKPOINT( KNL_BREAKPOINT_QLNC_QUERY_OPTIMIZER, KNL_ENV(aEnv) );


    /****************************************************************
     * Default Trace Information 시작
     ****************************************************************/

    if( QLL_IS_ENABLE_TRACE_LOGGER(aEnv) )
    {
        (void)qllTraceHead( QLL_TRACE_OPTIMIZER_DUMP_TITLE, aEnv );
        (void)qllTraceString( "--- Current SQL Query String ---\n", aEnv );

        /* Input Query String 출력 */
        STL_TRY( qllTraceBody( &QLL_CANDIDATE_MEM( aEnv ),
                               aEnv,
                               "%s\n",
                               aSQLStmt->mRetrySQL )
                 == STL_SUCCESS );
    }


    /****************************************************************
     * Parameter Information 설정
     ****************************************************************/

    sGlobalID = 0;
    sParamInfo.mTransID = aTransID;
    sParamInfo.mDBCStmt = aDBCStmt;
    sParamInfo.mSQLStmt = aSQLStmt;
    sParamInfo.mGlobalNodeID = &sGlobalID;
    sParamInfo.mGlobalInternalBindIdx = aInternalBindIdx;


    /****************************************************************
     * Prepare Candidate Nodes for Finding the Best Candidate Plan
     ****************************************************************/

    if( QLL_IS_ENABLE_TRACE_LOGGER(aEnv) )
    {
        (void)qllTraceMidTitle( "[STEP 01] Created Node for Optimizer", aEnv );
    }


    /**
     * Query Block Array 생성
     */

    STL_TRY( qlncCreateQBMapArray( &sParamInfo,
                                   &sQBMapArray,
                                   aEnv )
             == STL_SUCCESS );


    /**
     * Create Node Parameter Information 설정
     */

    QLNC_INIT_CREATE_NODE_PARAM_INFO( &sCreateNodeParamInfo,
                                      &sParamInfo,
                                      aQueryNode,
                                      NULL,
                                      NULL,
                                      NULL,
                                      NULL,
                                      sQBMapArray );

    STL_TRY( qlncCreateQueryCandNode( &sCreateNodeParamInfo,
                                      aEnv )
             == STL_SUCCESS );

    STL_DASSERT( sCreateNodeParamInfo.mCandNode != NULL );
    sCandQueryNode = sCreateNodeParamInfo.mCandNode;

    if( QLL_IS_ENABLE_TRACE_LOGGER(aEnv) )
    {
        (void)qllTraceString( "--- Current SQL Query String ---\n", aEnv );

        /* Candidate Node의 Query String 출력 */
        STL_TRY( qlncTraceQueryFromCandNode( &QLL_CANDIDATE_MEM( aEnv ),
                                             sCandQueryNode,
                                             aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Optimizer 수행
     */

    STL_TRY( qlncQueryNodeOptimizer( &sParamInfo,
                                     sCandQueryNode,
                                     aEnv )
             == STL_SUCCESS );


    /****************************************************************
     * Default Trace Information 종료
     ****************************************************************/

    if( QLL_IS_ENABLE_TRACE_LOGGER(aEnv) )
    {
        (void)qllTraceTail( QLL_TRACE_OPTIMIZER_DUMP_TITLE, aEnv );
    }


    /****************************************************************
     * Make Candidate Plan
     ****************************************************************/

    STL_TRY( qlncMakeCandPlan( aTransID,
                               aDBCStmt,
                               aSQLStmt,
                               aInitStmtExprList,
                               aTableStatList,
                               sCandQueryNode,
                               NULL,
                               &sCandQueryPlan,
                               aEnv )
             == STL_SUCCESS );


    /****************************************************************
     * Output 설정
     ****************************************************************/

    *aCandQueryPlan = sCandQueryPlan;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Query Node Optimizer를 수행한다.
 * @param[in]       aParamInfo  Parameter Info
 * @param[in,out]   aNode       Candidate Node
 * @param[in]       aEnv        Environment
 */
stlStatus qlncQueryNodeOptimizer( qlncParamInfo     * aParamInfo,
                                  qlncNodeCommon    * aNode,
                                  qllEnv            * aEnv )
{
    qlncCBOptParamInfo        sCBOptParamInfo;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode != NULL, QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Heuristic Query Transformation (HQT)
     ****************************************************************/

    if( QLL_IS_ENABLE_TRACE_LOGGER(aEnv) )
    {
        (void)qllTraceMidTitle( "[STEP 02] Heuristic Query Transformation", aEnv );
    }

    STL_TRY( qlncHeuristicQueryTransformation( aParamInfo,
                                               aNode,
                                               aEnv )
             == STL_SUCCESS );

    if( QLL_IS_ENABLE_TRACE_LOGGER(aEnv) &&
        (QLL_OPT_TRACE_CODE_PLAN_ON(aEnv) == STL_TRUE) )
    {
        /* Heuristic Query transformation 처리된 Candidate Node 형태 출력 */
        (void)qllTraceString( "--- Current SQL Query Block ---\n", aEnv );

        STL_TRY( qlncTraceTreeFromCandNode( &QLL_CANDIDATE_MEM( aEnv ),
                                            aNode,
                                            aEnv )
                 == STL_SUCCESS );
    }


    /****************************************************************
     * Cost Based Query Transformation (CBQT)
     ****************************************************************/

    if( QLL_IS_ENABLE_TRACE_LOGGER(aEnv) )
    {
        (void)qllTraceMidTitle( "[STEP 03] Cost Based Query Transformation", aEnv );
    }

    STL_TRY( qlncCostBasedQueryTransformation( aParamInfo,
                                               aNode,
                                               aEnv )
             == STL_SUCCESS );

    if( QLL_IS_ENABLE_TRACE_LOGGER(aEnv) &&
        (QLL_OPT_TRACE_CODE_PLAN_ON(aEnv) == STL_TRUE) )
    {
        /* Costed Based Query transformation 처리된 Candidate Node 형태 출력 */
        (void)qllTraceString( "--- Current SQL Query Block ---\n", aEnv );

        STL_TRY( qlncTraceTreeFromCandNode( &QLL_CANDIDATE_MEM( aEnv ),
                                            aNode,
                                            aEnv )
                 == STL_SUCCESS );
    }


    /****************************************************************
     * Cost Based Optimizer
     ****************************************************************/

    if( QLL_IS_ENABLE_TRACE_LOGGER(aEnv) )
    {
        (void)qllTraceMidTitle( "[STEP 04] Cost Based Optimizer", aEnv );
    }

    /* SubQuery에 대한 Optimizer 먼저 수행 */
    stlMemset( &sCBOptParamInfo, 0x00, STL_SIZEOF( qlncCBOptParamInfo ) );
    sCBOptParamInfo.mParamInfo = *aParamInfo;
    sCBOptParamInfo.mLeftNode = aNode;

    STL_TRY( qlncCBOptSubQueryFuncList[ aNode->mNodeType ]( &sCBOptParamInfo,
                                                            aEnv )
             == STL_SUCCESS );


    /* Query Node에 대한 Cost Based Optimizer를 수행한다.
     * 여기서는 절대 Transformation이 일어나지 않는다. */
    stlMemset( &sCBOptParamInfo, 0x00, STL_SIZEOF( qlncCBOptParamInfo ) );
    sCBOptParamInfo.mParamInfo = *aParamInfo;
    sCBOptParamInfo.mLeftNode = aNode;

    STL_TRY( qlncCostBasedOptimizerFuncList[ aNode->mNodeType ]( &sCBOptParamInfo,
                                                                 aEnv )
             == STL_SUCCESS );

    /**
     * Post Optimizer
     */

    STL_TRY( qlncPostOptimizer( aParamInfo,
                                aNode,
                                aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief DML Node들의 Optimizer를 수행한다.
 * @param[in]   aParamInfo  Parameter Info
 * @param[in]   aNode       Candidate Node
 * @param[out]  aOutNode    Candidate Output Node
 * @param[in]   aEnv        Environment
 */
stlStatus qlncDMLNodeOptimizer( qlncParamInfo   * aParamInfo,
                                qlncNodeCommon  * aNode,
                                qlncNodeCommon ** aOutNode,
                                qllEnv          * aEnv )
{
    qlncCBOptParamInfo        sCBOptParamInfo;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode != NULL, QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Heuristic Query Transformation (HQT)
     ****************************************************************/

    if( QLL_IS_ENABLE_TRACE_LOGGER(aEnv) )
    {
        (void)qllTraceMidTitle( "[STEP 02] Heuristic Query Transformation", aEnv );
    }

    STL_TRY( qlncHeuristicQueryTransformation( aParamInfo,
                                               aNode,
                                               aEnv )
             == STL_SUCCESS );

    if( QLL_IS_ENABLE_TRACE_LOGGER(aEnv) &&
        (QLL_OPT_TRACE_CODE_PLAN_ON(aEnv) == STL_TRUE) )
    {
        /* Heuristic Query transformation 처리된 Candidate Node 형태 출력 */
        (void)qllTraceString( "--- Current SQL Query Block ---\n", aEnv );

        STL_TRY( qlncTraceTreeFromCandNode( &QLL_CANDIDATE_MEM( aEnv ),
                                            aNode,
                                            aEnv )
                 == STL_SUCCESS );
    }


    /****************************************************************
     * Cost Based Query Transformation (CBQT)
     ****************************************************************/

    if( QLL_IS_ENABLE_TRACE_LOGGER(aEnv) )
    {
        (void)qllTraceMidTitle( "[STEP 03] Cost Based Query Transformation", aEnv );
    }

    STL_TRY( qlncCostBasedQueryTransformation( aParamInfo,
                                               aNode,
                                               aEnv )
             == STL_SUCCESS );

    if( QLL_IS_ENABLE_TRACE_LOGGER(aEnv) &&
        (QLL_OPT_TRACE_CODE_PLAN_ON(aEnv) == STL_TRUE) )
    {
        /* Costed Based Query transformation 처리된 Candidate Node 형태 출력 */
        (void)qllTraceString( "--- Current SQL Query Block ---\n", aEnv );

        STL_TRY( qlncTraceTreeFromCandNode( &QLL_CANDIDATE_MEM( aEnv ),
                                            aNode,
                                            aEnv )
                 == STL_SUCCESS );
    }


    /****************************************************************
     * Cost Based Optimizer
     ****************************************************************/

    if( QLL_IS_ENABLE_TRACE_LOGGER(aEnv) )
    {
        (void)qllTraceMidTitle( "[STEP 04] Cost Based Optimizer", aEnv );
    }

    /* SubQuery에 대한 Optimizer 먼저 수행 */
    stlMemset( &sCBOptParamInfo, 0x00, STL_SIZEOF( qlncCBOptParamInfo ) );
    sCBOptParamInfo.mParamInfo = *aParamInfo;
    sCBOptParamInfo.mLeftNode = aNode;

    STL_TRY( qlncCBOptSubQueryFuncList[ aNode->mNodeType ]( &sCBOptParamInfo,
                                                            aEnv )
             == STL_SUCCESS );


    /* Query Node에 대한 Cost Based Optimizer를 수행한다.
     * 여기서는 절대 Transformation이 일어나지 않는다. */
    stlMemset( &sCBOptParamInfo, 0x00, STL_SIZEOF( qlncCBOptParamInfo ) );
    sCBOptParamInfo.mParamInfo = *aParamInfo;
    sCBOptParamInfo.mLeftNode = aNode;

    STL_TRY( qlncCostBasedOptimizerFuncList[ aNode->mNodeType ]( &sCBOptParamInfo,
                                                                 aEnv )
             == STL_SUCCESS );

    /**
     * Post Optimizer
     */

    STL_TRY( qlncPostOptimizer_DML( aParamInfo,
                                    aNode,
                                    aOutNode,
                                    aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Select Node의 Candidate Plan을 구한다.
 * @param[in]       aTransID                Transaction ID
 * @param[in]       aDBCStmt                DBC Statement
 * @param[in]       aSQLStmt                SQL Statement 객체
 * @param[in]       aQueryExprList          Query Expression List
 * @param[in,out]   aInternalBindIdx        Internal Bind Idx
 * @param[in,out]   aSelectStmtParamInfo    Select Statement Parameter Info
 * @param[in]       aEnv                    Environment
 * @remarks
 */
stlStatus qlncSelectOptimizer( smlTransId                 aTransID,
                               qllDBCStmt               * aDBCStmt,
                               qllStatement             * aSQLStmt,
                               qloInitExprList          * aQueryExprList,
                               stlInt32                 * aInternalBindIdx,
                               qlncSelectStmtParamInfo  * aSelectStmtParamInfo,
                               qllEnv                   * aEnv )
{
    qlvInitSelect           * sInitSelect        = NULL;
    qlvInitSelect           * sOutInitSelect     = NULL;

    qlncPlanCommon          * sCandQueryPlan     = NULL;

    stlInt32                * sBindNodePos       = NULL;
    knlBindType             * sBindIOType        = NULL;
    qlvInitExpression       * sExpr              = NULL;
    
    qlvRefTableList         * sScanTableList     = NULL;
    qlvRefTableList         * sLockTableList     = NULL;
    qlvRefTableList         * sTempTableList     = NULL;
    qlvRefTableItem         * sSrcTableItem      = NULL;
    qlvRefTableItem         * sDestTableItem     = NULL;
    qlvRefTableItem         * sNewTableItem      = NULL;
    qlvRefTableItem         * sLastTableItem     = NULL;

    qloValidTblStatList     * sTableStatList     = NULL;
    
    stlInt32                  sLoop              = 0;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aSQLStmt->mStmtType == QLL_STMT_SELECT_TYPE ) ||
                        ( aSQLStmt->mStmtType == QLL_STMT_SELECT_FOR_UPDATE_TYPE ) ||
                        ( aSQLStmt->mStmtType == QLL_STMT_SELECT_INTO_TYPE ) ||
                        ( aSQLStmt->mStmtType == QLL_STMT_SELECT_INTO_FOR_UPDATE_TYPE ),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSelectStmtParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSelectStmtParamInfo->mStmtNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSelectStmtParamInfo->mQueryNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInternalBindIdx != NULL, QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Query Optimize
     ****************************************************************/

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( qloValidTblStatList ),
                                (void**) &sTableStatList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sTableStatList->mCount = 0;
    sTableStatList->mHead = NULL;
    sTableStatList->mTail = NULL;

    STL_TRY( qlncQueryOptimizer( aTransID,
                                 aDBCStmt,
                                 aSQLStmt,
                                 aSelectStmtParamInfo->mQueryNode,
                                 aQueryExprList->mStmtExprList->mInitExprList,
                                 sTableStatList,
                                 aInternalBindIdx,
                                 & sCandQueryPlan,
                                 aEnv )
             == STL_SUCCESS );
    
    
    /****************************************************************
     * Statement Node 정보 구축
     ****************************************************************/

    sInitSelect = (qlvInitSelect *) aSelectStmtParamInfo->mStmtNode;

    /* Init Plan 공간 할당 */
    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlvInitSelect ),
                                (void**) & sOutInitSelect,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Init Plan 공통 정보 */
    sOutInitSelect->mInitPlan.mBindCount = sInitSelect->mInitPlan.mBindCount;
    sOutInitSelect->mInitPlan.mResultCursorProperty = sInitSelect->mInitPlan.mResultCursorProperty;

    STL_TRY( ellCopyObjectItem( & sOutInitSelect->mInitPlan.mValidationObjList,
                                sInitSelect->mInitPlan.mValidationObjList,
                                QLL_CODE_PLAN( aDBCStmt ),
                                ELL_ENV(aEnv) )
             == STL_SUCCESS );

    if( sInitSelect->mInitPlan.mBindCount > 0 )
    {
        STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                    STL_SIZEOF( stlInt32 ) * sInitSelect->mInitPlan.mBindCount,
                                    (void**) & sBindNodePos,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemcpy( sBindNodePos,
                   sInitSelect->mInitPlan.mBindNodePos,
                   STL_SIZEOF( stlInt32 ) * sInitSelect->mInitPlan.mBindCount );
    
        STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                    STL_SIZEOF( knlBindType ) * sInitSelect->mInitPlan.mBindCount,
                                    (void**) & sBindIOType,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemcpy( sBindIOType,
                   sInitSelect->mInitPlan.mBindIOType,
                   STL_SIZEOF( knlBindType ) * sInitSelect->mInitPlan.mBindCount );
    }
    else
    {
        sBindNodePos = NULL;
        sBindIOType  = NULL;
    }
    sOutInitSelect->mInitPlan.mBindNodePos = sBindNodePos;
    sOutInitSelect->mInitPlan.mBindIOType = sBindIOType;
    sOutInitSelect->mInitPlan.mHasHintError = sInitSelect->mInitPlan.mHasHintError;

    /* Query Node */
    sOutInitSelect->mQueryNode = sCandQueryPlan->mInitNode;

    /* Statement 단위 Expression 정보 */
    sOutInitSelect->mStmtExprList = NULL;

    /* for INTO clause */
    sOutInitSelect->mIntoTargetCnt = sInitSelect->mIntoTargetCnt;

    if( sInitSelect->mIntoTargetCnt > 0 )
    {
        STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN(aDBCStmt),
                                    STL_SIZEOF( qlvInitExpression ) * sInitSelect->mIntoTargetCnt,
                                    (void **) & sOutInitSelect->mIntoTargetArray,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        for( sLoop = 0 ; sLoop < sInitSelect->mIntoTargetCnt ; sLoop++ )
        {
            STL_TRY( qlvCopyExpr( & sInitSelect->mIntoTargetArray[ sLoop ],
                                  & sExpr,
                                  QLL_CODE_PLAN(aDBCStmt),
                                  aEnv )
                     == STL_SUCCESS );

            sOutInitSelect->mIntoTargetArray[ sLoop ] = *sExpr;
        }
    }
    else
    {
        sOutInitSelect->mIntoTargetArray = NULL;
    }

    /* for Sensitivity */
    if( sInitSelect->mScanTableList != NULL )
    {
        if( sInitSelect->mScanTableList->mCount > 0 )
        {
            /**
             * Scan Table List 초기화 
             */

            STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN(aDBCStmt),
                                        STL_SIZEOF( qlvRefTableList ),
                                        (void **) & sScanTableList,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            sScanTableList->mCount = 0;
            sScanTableList->mHead  = NULL;

        
            /**
             * FROM list 에서 scan table 의 list 를 구축
             */
    
            STL_TRY( qlvSetCursorItemListByFROM(
                         QLL_CODE_PLAN(aDBCStmt),
                         ((qlvInitQuerySpecNode *) sOutInitSelect->mQueryNode)->mTableNode,
                         sScanTableList,
                         aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /* @todo Validation에서 scan table list가
             * 하나 이상 존재하는 것이 보장되는 것으로 판단됨 */

            sScanTableList = NULL;
        }
    }
    else
    {
        sScanTableList = NULL;
    }
    sOutInitSelect->mScanTableList = sScanTableList;
    
    /* for Updatability */
    sOutInitSelect->mSerialAction = sInitSelect->mSerialAction; 
    sOutInitSelect->mUpdatableMode = sInitSelect->mUpdatableMode; 
    sOutInitSelect->mLockWaitMode = sInitSelect->mLockWaitMode; 
    sOutInitSelect->mLockTimeInterval = sInitSelect->mLockTimeInterval; 

    if( sInitSelect->mLockTableList != NULL )
    {
        if( sInitSelect->mLockTableList->mCount > 0 )
        {
            /**
             * Lock Table List 초기화 
             */

            STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN(aDBCStmt),
                                        STL_SIZEOF( qlvRefTableList ),
                                        (void **) & sLockTableList,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            sLockTableList->mCount = 0;
            sLockTableList->mHead  = NULL;

            STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN(aDBCStmt),
                                        STL_SIZEOF( qlvRefTableList ),
                                        (void **) & sTempTableList,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            sTempTableList->mCount = 0;
            sTempTableList->mHead  = NULL;

        
            /**
             * FROM list 에서 lockable table 의 list 를 구축
             */
    
            STL_TRY( qlvSetCursorItemListByFROM(
                         & QLL_CANDIDATE_MEM(aEnv),
                         ((qlvInitQuerySpecNode *) sOutInitSelect->mQueryNode)->mTableNode,
                         sTempTableList,
                         aEnv )
                     == STL_SUCCESS );


            /**
             * 실제 LockTableList에 있는 Table들만 lockable table list로 구성
             */

            sSrcTableItem = sInitSelect->mLockTableList->mHead;
            
            for( sLoop = 0 ; sLoop < sInitSelect->mLockTableList->mCount ; sLoop++ )
            {
                STL_DASSERT( sSrcTableItem->mTableNode->mType == QLV_NODE_TYPE_BASE_TABLE );
                
                sDestTableItem = sTempTableList->mHead;
                while( sDestTableItem != NULL )
                {
                    STL_DASSERT( sDestTableItem->mTableNode->mType == QLV_NODE_TYPE_BASE_TABLE );
                    
                    if( ((qlvInitBaseTableNode*) sSrcTableItem->mTableNode)->mTablePhyHandle ==
                        ((qlvInitBaseTableNode*) sDestTableItem->mTableNode)->mTablePhyHandle )
                    {
                        STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN(aDBCStmt),
                                                    STL_SIZEOF( qlvRefTableItem ),
                                                    (void **) & sNewTableItem,
                                                    KNL_ENV(aEnv) )
                                 == STL_SUCCESS );

                        sNewTableItem->mTableNode   = sDestTableItem->mTableNode;
                        sNewTableItem->mTargetCount = sDestTableItem->mTargetCount;
                        sNewTableItem->mTargets     = sDestTableItem->mTargets;
                        sNewTableItem->mNext        = NULL;

                        if( sLockTableList->mHead == NULL )
                        {
                            sLockTableList->mHead = sNewTableItem;
                        }
                        else
                        {
                            sLastTableItem->mNext = sNewTableItem;
                        }
                        sLastTableItem = sNewTableItem;
                        sLockTableList->mCount++;
                        break;
                    }

                    sDestTableItem = sDestTableItem->mNext;
                }

                sSrcTableItem = sSrcTableItem->mNext;
            }
        }
        else
        {
            /* @todo Validation에서 lock table list가
             * 하나 이상 존재하는 것이 보장되는 것으로 판단됨 */

            sLockTableList = NULL;
        }
    }
    else
    {
        sLockTableList = NULL;
    }
    sOutInitSelect->mLockTableList = sLockTableList;

    /* target info */
    if( sInitSelect->mTargetInfo != NULL )
    {
        STL_TRY( qlfCopyTarget( sInitSelect->mTargetInfo,
                                & sOutInitSelect->mTargetInfo,
                                QLL_CODE_PLAN(aDBCStmt),
                                aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sOutInitSelect->mTargetInfo = NULL;
    }

    
    /****************************************************************
     * Output 설정
     ****************************************************************/

    /* Query Candidate Plan Node */
    aSelectStmtParamInfo->mOutPlanQuery = sCandQueryPlan;
    
    /* Init Plan Node for Statement */
    aSelectStmtParamInfo->mOutStmtNode = (qlvInitNode *) sOutInitSelect;

    /* Valid Table Statistic List */
    aSelectStmtParamInfo->mOutValidTblStatList = sTableStatList;
    

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Insert Values Node의 Candidate Plan을 구한다.
 * @param[in]       aTransID                Transaction ID
 * @param[in]       aDBCStmt                DBC Statement
 * @param[in]       aSQLStmt                SQL Statement 객체
 * @param[in]       aQueryExprList          Query Expression List
 * @param[in,out]   aInternalBindIdx        Internal Bind Idx
 * @param[in,out]   aInsertStmtParamInfo    Insert Statement Parameter Info
 * @param[in]       aEnv                    Environment
 * @remarks
 */
stlStatus qlncInsertValuesOptimizer( smlTransId                 aTransID,
                                     qllDBCStmt               * aDBCStmt,
                                     qllStatement             * aSQLStmt,
                                     qloInitExprList          * aQueryExprList,
                                     stlInt32                 * aInternalBindIdx,
                                     qlncInsertStmtParamInfo  * aInsertStmtParamInfo,
                                     qllEnv                   * aEnv )
{
    qlvInitInsertValues     * sInitInsert           = NULL;
    qlvInitInsertValues     * sOutInitInsert        = NULL;
    qlvInitInsertReturnInto * sInitInsertReturn     = NULL;
    qlvInitInsertReturnInto * sOutInitInsertReturn  = NULL;

    qloValidTblStatList     * sTableStatList        = NULL;

    stlInt32                * sBindNodePos          = NULL;
    knlBindType             * sBindIOType           = NULL;
    qlvInitExpression       * sExpr                 = NULL;
    stlInt32                  sLoop                 = 0;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aSQLStmt->mStmtType == QLL_STMT_INSERT_TYPE ) ||
                        ( aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_QUERY_TYPE ) ||
                        ( aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_INTO_TYPE ),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInsertStmtParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInternalBindIdx != NULL, QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Make Insert Candidate Plan
     ****************************************************************/

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( qloValidTblStatList ),
                                (void**) &sTableStatList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sTableStatList->mCount = 0;
    sTableStatList->mHead = NULL;
    sTableStatList->mTail = NULL;

    STL_TRY( qlncInsertOptimizerInternal( aTransID,
                                          aDBCStmt,
                                          aSQLStmt,
                                          aQueryExprList,
                                          sTableStatList,
                                          aInternalBindIdx,
                                          aInsertStmtParamInfo,
                                          aEnv )
             == STL_SUCCESS );    
    
    
    /****************************************************************
     * Statement Node 정보 구축
     ****************************************************************/

    sInitInsert = (qlvInitInsertValues *) aInsertStmtParamInfo->mStmtNode;

    /* Init Plan 공간 할당 */
    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlvInitInsertValues ),
                                (void**) & sOutInitInsert,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Init Plan 공통 정보 */
    sOutInitInsert->mInitPlan.mBindCount = sInitInsert->mInitPlan.mBindCount;
    sOutInitInsert->mInitPlan.mResultCursorProperty = sInitInsert->mInitPlan.mResultCursorProperty;

    STL_TRY( ellCopyObjectItem( & sOutInitInsert->mInitPlan.mValidationObjList,
                                sInitInsert->mInitPlan.mValidationObjList,
                                QLL_CODE_PLAN( aDBCStmt ),
                                ELL_ENV(aEnv) )
             == STL_SUCCESS );

    if( sInitInsert->mInitPlan.mBindCount > 0 )
    {
        STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                    STL_SIZEOF( stlInt32 ) * sInitInsert->mInitPlan.mBindCount,
                                    (void**) & sBindNodePos,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemcpy( sBindNodePos,
                   sInitInsert->mInitPlan.mBindNodePos,
                   STL_SIZEOF( stlInt32 ) * sInitInsert->mInitPlan.mBindCount );
    
        STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                    STL_SIZEOF( knlBindType ) * sInitInsert->mInitPlan.mBindCount,
                                    (void**) & sBindIOType,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemcpy( sBindIOType,
                   sInitInsert->mInitPlan.mBindIOType,
                   STL_SIZEOF( knlBindType ) * sInitInsert->mInitPlan.mBindCount );
    }
    else
    {
        sBindNodePos = NULL;
        sBindIOType  = NULL;
    }
    sOutInitInsert->mInitPlan.mBindNodePos = sBindNodePos;
    sOutInitInsert->mInitPlan.mBindIOType = sBindIOType;
    sOutInitInsert->mInitPlan.mHasHintError = sInitInsert->mInitPlan.mHasHintError;

    /* Write Table 정보 */
    stlMemcpy( &(sOutInitInsert->mTableHandle),
               &(sInitInsert->mTableHandle),
               STL_SIZEOF( ellDictHandle ) );
    sOutInitInsert->mTablePhyHandle = sInitInsert->mTablePhyHandle;
    sOutInitInsert->mMultiValueCnt = sInitInsert->mMultiValueCnt;

    /* Expression List */
    sOutInitInsert->mStmtExprList = NULL;    /* default */
    sOutInitInsert->mQueryExprList = NULL;   /* default */
    
    /* Read Table 정보 */
    sOutInitInsert->mWriteTableNode = aInsertStmtParamInfo->mOutPlanWriteTable->mInitNode;

    /* Column 정보 */
    sOutInitInsert->mTotalColumnCnt = sInitInsert->mTotalColumnCnt;

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( ellDictHandle ) * sInitInsert->mTotalColumnCnt,
                                (void**) & (sOutInitInsert->mColumnHandle),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemcpy( sOutInitInsert->mColumnHandle,
               sInitInsert->mColumnHandle,
               STL_SIZEOF( ellDictHandle ) * sInitInsert->mTotalColumnCnt );

    sOutInitInsert->mInsertColExprList = aInsertStmtParamInfo->mInsertColExprList;

    /* Insert Target 정보 */
    sOutInitInsert->mInsertTarget = *aInsertStmtParamInfo->mOutInsertTarget;
    
    /* Single Row 정보 */
    sOutInitInsert->mRowList = aInsertStmtParamInfo->mOutRowList;

    /* Insert 에 영향을 받는 Object */
    sOutInitInsert->mHasCheck = sInitInsert->mHasCheck;
    sOutInitInsert->mHasIndex = sInitInsert->mHasIndex;

    STL_TRY( qlncCopyInsertRelObject( & sInitInsert->mRelObject,
                                      & sOutInitInsert->mRelObject,
                                      QLL_CODE_PLAN(aDBCStmt),
                                      aEnv )
             == STL_SUCCESS );
    
    /* RETURN ... INTO 관련 */
    sOutInitInsert->mReturnTargetCnt = sInitInsert->mReturnTargetCnt;
    sOutInitInsert->mReturnTargetArray = aInsertStmtParamInfo->mOutReturnTargetArray;
    
    if( ( sInitInsert->mReturnTargetCnt > 0 ) &&
        ( sInitInsert->mIntoTargetArray != NULL ) )
    {
        STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN(aDBCStmt),
                                    STL_SIZEOF( qlvInitExpression ) * sInitInsert->mReturnTargetCnt,
                                    (void **) & sOutInitInsert->mIntoTargetArray,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        for( sLoop = 0 ; sLoop < sInitInsert->mReturnTargetCnt ; sLoop++ )
        {
            STL_TRY( qlvCopyExpr( & sInitInsert->mIntoTargetArray[ sLoop ],
                                  & sExpr,
                                  QLL_CODE_PLAN(aDBCStmt),
                                  aEnv )
                     == STL_SUCCESS );

            sOutInitInsert->mIntoTargetArray[ sLoop ] = *sExpr;
        }
    }
    else
    {
        sOutInitInsert->mIntoTargetArray = NULL;
    }
    
    /* Insert Return Statement */
    if( (aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_INTO_TYPE) ||
        (aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_QUERY_TYPE) )
    {
        sInitInsertReturn = (qlvInitInsertReturnInto *) aSQLStmt->mInitPlan;
        
        /* Init Plan 공간 할당 */
        STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                    STL_SIZEOF( qlvInitInsertReturnInto ),
                                    (void**) & sOutInitInsertReturn,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Init Plan 공통 정보 */
        sOutInitInsertReturn->mInitPlan.mBindCount =
            sInitInsertReturn->mInitPlan.mBindCount;
        sOutInitInsertReturn->mInitPlan.mResultCursorProperty =
            sInitInsertReturn->mInitPlan.mResultCursorProperty;

        STL_TRY( ellCopyObjectItem(
                     & sOutInitInsertReturn->mInitPlan.mValidationObjList,
                     sInitInsertReturn->mInitPlan.mValidationObjList,
                     QLL_CODE_PLAN( aDBCStmt ),
                     ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if( sInitInsertReturn->mInitPlan.mBindCount > 0 )
        {
            STL_TRY( knlAllocRegionMem(
                         QLL_CODE_PLAN( aDBCStmt ),
                         STL_SIZEOF( stlInt32 ) * sInitInsertReturn->mInitPlan.mBindCount,
                         (void**) & sBindNodePos,
                         KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            stlMemcpy( sBindNodePos,
                       sInitInsertReturn->mInitPlan.mBindNodePos,
                       STL_SIZEOF( stlInt32 ) * sInitInsertReturn->mInitPlan.mBindCount );
    
            STL_TRY( knlAllocRegionMem(
                         QLL_CODE_PLAN( aDBCStmt ),
                         STL_SIZEOF( knlBindType ) * sInitInsertReturn->mInitPlan.mBindCount,
                         (void**) & sBindIOType,
                         KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            stlMemcpy( sBindIOType,
                       sInitInsertReturn->mInitPlan.mBindIOType,
                       STL_SIZEOF( knlBindType ) * sInitInsertReturn->mInitPlan.mBindCount );
        }
        else
        {
            sBindNodePos = NULL;
            sBindIOType  = NULL;
        }
        sOutInitInsertReturn->mInitPlan.mBindNodePos = sBindNodePos;
        sOutInitInsertReturn->mInitPlan.mBindIOType = sBindIOType;
        sOutInitInsertReturn->mInitPlan.mHasHintError = sInitInsertReturn->mInitPlan.mHasHintError;

        /* Insert Stmt 정보 */
        sOutInitInsertReturn->mIsInsertValues = STL_TRUE;
        sOutInitInsertReturn->mInitInsertValues = sOutInitInsert;
        sOutInitInsertReturn->mInitInsertSelect = NULL;

        sOutInitInsertReturn->mTableHandle = &(sOutInitInsert->mTableHandle);

        /* target info */
        if( sInitInsertReturn->mTargetInfo != NULL )
        {
            STL_TRY( qlfCopyTarget( sInitInsertReturn->mTargetInfo,
                                    & sOutInitInsertReturn->mTargetInfo,
                                    QLL_CODE_PLAN(aDBCStmt),
                                    aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            sOutInitInsertReturn->mTargetInfo = NULL;
        }
    }
    else
    {
        sOutInitInsertReturn = NULL;
    }

    
    /****************************************************************
     * Output 설정
     ****************************************************************/

    /* Init Plan Node for Statement */
    if( sOutInitInsertReturn != NULL )
    {
        aInsertStmtParamInfo->mOutStmtNode = (qlvInitNode *) sOutInitInsertReturn;
    }
    else
    {
        aInsertStmtParamInfo->mOutStmtNode = (qlvInitNode *) sOutInitInsert;
    }

    /* Valid Table Statistic List */
    aInsertStmtParamInfo->mOutValidTblStatList = sTableStatList;
    
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Insert Select Node의 Candidate Plan을 구한다.
 * @param[in]       aTransID                Transaction ID
 * @param[in]       aDBCStmt                DBC Statement
 * @param[in]       aSQLStmt                SQL Statement 객체
 * @param[in]       aQueryExprList          Query Expression List
 * @param[in]       aTableStatList          Table Stat List
 * @param[in,out]   aInternalBindIdx        Internal Bind Idx
 * @param[in,out]   aInsertStmtParamInfo    Insert Statement Parameter Info
 * @param[in]       aEnv                    Environment
 * @remarks
 */
stlStatus qlncInsertSelectOptimizer( smlTransId                 aTransID,
                                     qllDBCStmt               * aDBCStmt,
                                     qllStatement             * aSQLStmt,
                                     qloInitExprList          * aQueryExprList,
                                     qloValidTblStatList      * aTableStatList,
                                     stlInt32                 * aInternalBindIdx,
                                     qlncInsertStmtParamInfo  * aInsertStmtParamInfo,
                                     qllEnv                   * aEnv )
{
    qlvInitInsertSelect     * sInitInsert           = NULL;
    qlvInitInsertSelect     * sOutInitInsert        = NULL;
    qlvInitInsertReturnInto * sInitInsertReturn     = NULL;
    qlvInitInsertReturnInto * sOutInitInsertReturn  = NULL;

    stlInt32                * sBindNodePos          = NULL;
    knlBindType             * sBindIOType           = NULL;
    qlvInitExpression       * sExpr                 = NULL;
    stlInt32                  sLoop                 = 0;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aSQLStmt->mStmtType == QLL_STMT_INSERT_SELECT_TYPE ) ||
                        ( aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_QUERY_TYPE ) ||
                        ( aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_INTO_TYPE ),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInsertStmtParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInternalBindIdx != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableStatList != NULL, QLL_ERROR_STACK(aEnv) );
    

    /****************************************************************
     * Make Insert Candidate Plan
     ****************************************************************/

    STL_TRY( qlncInsertOptimizerInternal( aTransID,
                                          aDBCStmt,
                                          aSQLStmt,
                                          aQueryExprList,
                                          aTableStatList,
                                          aInternalBindIdx,
                                          aInsertStmtParamInfo,
                                          aEnv )
             == STL_SUCCESS );    
    
    
    /****************************************************************
     * Statement Node 정보 구축
     ****************************************************************/

    sInitInsert = (qlvInitInsertSelect *) aInsertStmtParamInfo->mStmtNode;

    /* Init Plan 공간 할당 */
    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlvInitInsertSelect ),
                                (void**) & sOutInitInsert,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Init Plan 공통 정보 */
    sOutInitInsert->mInitPlan.mBindCount = sInitInsert->mInitPlan.mBindCount;
    sOutInitInsert->mInitPlan.mResultCursorProperty = sInitInsert->mInitPlan.mResultCursorProperty;

    STL_TRY( ellCopyObjectItem( & sOutInitInsert->mInitPlan.mValidationObjList,
                                sInitInsert->mInitPlan.mValidationObjList,
                                QLL_CODE_PLAN( aDBCStmt ),
                                ELL_ENV(aEnv) )
             == STL_SUCCESS );

    if( sInitInsert->mInitPlan.mBindCount > 0 )
    {
        STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                    STL_SIZEOF( stlInt32 ) * sInitInsert->mInitPlan.mBindCount,
                                    (void**) & sBindNodePos,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemcpy( sBindNodePos,
                   sInitInsert->mInitPlan.mBindNodePos,
                   STL_SIZEOF( stlInt32 ) * sInitInsert->mInitPlan.mBindCount );
    
        STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                    STL_SIZEOF( knlBindType ) * sInitInsert->mInitPlan.mBindCount,
                                    (void**) & sBindIOType,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemcpy( sBindIOType,
                   sInitInsert->mInitPlan.mBindIOType,
                   STL_SIZEOF( knlBindType ) * sInitInsert->mInitPlan.mBindCount );
    }
    else
    {
        sBindNodePos = NULL;
        sBindIOType  = NULL;
    }
    sOutInitInsert->mInitPlan.mBindNodePos = sBindNodePos;
    sOutInitInsert->mInitPlan.mBindIOType = sBindIOType;
    sOutInitInsert->mInitPlan.mHasHintError = sInitInsert->mInitPlan.mHasHintError;

    /* Write Table 정보 */
    stlMemcpy( &(sOutInitInsert->mTableHandle),
               &(sInitInsert->mTableHandle),
               STL_SIZEOF( ellDictHandle ) );
    sOutInitInsert->mTablePhyHandle = sInitInsert->mTablePhyHandle;

    /* Expression List */
    sOutInitInsert->mStmtExprList = NULL;    /* default */
    sOutInitInsert->mQueryExprList = NULL;   /* default */
    
    /* Read Table 정보 */
    sOutInitInsert->mWriteTableNode = aInsertStmtParamInfo->mOutPlanWriteTable->mInitNode;

    /* Column 정보 */
    sOutInitInsert->mTotalColumnCnt = sInitInsert->mTotalColumnCnt;

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( ellDictHandle ) * sInitInsert->mTotalColumnCnt,
                                (void**) & (sOutInitInsert->mColumnHandle),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemcpy( sOutInitInsert->mColumnHandle,
               sInitInsert->mColumnHandle,
               STL_SIZEOF( ellDictHandle ) * sInitInsert->mTotalColumnCnt );

    sOutInitInsert->mInsertColExprList = aInsertStmtParamInfo->mInsertColExprList;

    /* Insert Target 정보 */
    sOutInitInsert->mInsertTarget = *aInsertStmtParamInfo->mOutInsertTarget;
    
    /* Single Row 정보 */
    sOutInitInsert->mRowList = aInsertStmtParamInfo->mOutRowList;

    /* Select 정보 */
    sOutInitInsert->mSubQueryNode = NULL;   /* default */

    /* Insert 에 영향을 받는 Object */
    sOutInitInsert->mHasCheck = sInitInsert->mHasCheck;
    sOutInitInsert->mHasIndex = sInitInsert->mHasIndex;

    STL_TRY( qlncCopyInsertRelObject( & sInitInsert->mRelObject,
                                      & sOutInitInsert->mRelObject,
                                      QLL_CODE_PLAN(aDBCStmt),
                                      aEnv )
             == STL_SUCCESS );
    
    /* RETURN ... INTO 관련 */
    sOutInitInsert->mReturnTargetCnt = sInitInsert->mReturnTargetCnt;
    sOutInitInsert->mReturnTargetArray = aInsertStmtParamInfo->mOutReturnTargetArray;
    
    if( ( sInitInsert->mReturnTargetCnt > 0 ) &&
        ( sInitInsert->mIntoTargetArray != NULL ) )
    {
        STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN(aDBCStmt),
                                    STL_SIZEOF( qlvInitExpression ) * sInitInsert->mReturnTargetCnt,
                                    (void **) & sOutInitInsert->mIntoTargetArray,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        for( sLoop = 0 ; sLoop < sInitInsert->mReturnTargetCnt ; sLoop++ )
        {
            STL_TRY( qlvCopyExpr( & sInitInsert->mIntoTargetArray[ sLoop ],
                                  & sExpr,
                                  QLL_CODE_PLAN(aDBCStmt),
                                  aEnv )
                     == STL_SUCCESS );

            sOutInitInsert->mIntoTargetArray[ sLoop ] = *sExpr;
        }
    }
    else
    {
        sOutInitInsert->mIntoTargetArray = NULL;
    }

    /* Insert Return Statement */
    if( (aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_INTO_TYPE) ||
        (aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_QUERY_TYPE) )
    {
        sInitInsertReturn = (qlvInitInsertReturnInto *) aSQLStmt->mInitPlan;
        
        /* Init Plan 공간 할당 */
        STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                    STL_SIZEOF( qlvInitInsertReturnInto ),
                                    (void**) & sOutInitInsertReturn,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Init Plan 공통 정보 */
        sOutInitInsertReturn->mInitPlan.mBindCount =
            sInitInsertReturn->mInitPlan.mBindCount;
        sOutInitInsertReturn->mInitPlan.mResultCursorProperty =
            sInitInsertReturn->mInitPlan.mResultCursorProperty;

        STL_TRY( ellCopyObjectItem(
                     & sOutInitInsertReturn->mInitPlan.mValidationObjList,
                     sInitInsertReturn->mInitPlan.mValidationObjList,
                     QLL_CODE_PLAN( aDBCStmt ),
                     ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if( sInitInsertReturn->mInitPlan.mBindCount > 0 )
        {
            STL_TRY( knlAllocRegionMem(
                         QLL_CODE_PLAN( aDBCStmt ),
                         STL_SIZEOF( stlInt32 ) * sInitInsertReturn->mInitPlan.mBindCount,
                         (void**) & sBindNodePos,
                         KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            stlMemcpy( sBindNodePos,
                       sInitInsertReturn->mInitPlan.mBindNodePos,
                       STL_SIZEOF( stlInt32 ) * sInitInsertReturn->mInitPlan.mBindCount );
    
            STL_TRY( knlAllocRegionMem(
                         QLL_CODE_PLAN( aDBCStmt ),
                         STL_SIZEOF( knlBindType ) * sInitInsertReturn->mInitPlan.mBindCount,
                         (void**) & sBindIOType,
                         KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            stlMemcpy( sBindIOType,
                       sInitInsertReturn->mInitPlan.mBindIOType,
                       STL_SIZEOF( knlBindType ) * sInitInsertReturn->mInitPlan.mBindCount );
        }
        else
        {
            sBindNodePos = NULL;
            sBindIOType  = NULL;
        }
        sOutInitInsertReturn->mInitPlan.mBindNodePos = sBindNodePos;
        sOutInitInsertReturn->mInitPlan.mBindIOType = sBindIOType;
        sOutInitInsertReturn->mInitPlan.mHasHintError = sInitInsertReturn->mInitPlan.mHasHintError;
        
        /* Insert Stmt 정보 */
        sOutInitInsertReturn->mIsInsertValues = STL_FALSE;
        sOutInitInsertReturn->mInitInsertValues = NULL;
        sOutInitInsertReturn->mInitInsertSelect = sOutInitInsert;

        sOutInitInsertReturn->mTableHandle = &(sOutInitInsert->mTableHandle);

        /* target info */
        if( sInitInsertReturn->mTargetInfo != NULL )
        {
            STL_TRY( qlfCopyTarget( sInitInsertReturn->mTargetInfo,
                                    & sOutInitInsertReturn->mTargetInfo,
                                    QLL_CODE_PLAN(aDBCStmt),
                                    aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            sOutInitInsertReturn->mTargetInfo = NULL;
        }
    }
    else
    {
        sOutInitInsertReturn = NULL;
    }

    
    /****************************************************************
     * Output 설정
     ****************************************************************/

    /* Init Plan Node for Statement */
    if( sOutInitInsertReturn != NULL )
    {
        aInsertStmtParamInfo->mOutStmtNode = (qlvInitNode *) sOutInitInsertReturn;
    }
    else
    {
        aInsertStmtParamInfo->mOutStmtNode = (qlvInitNode *) sOutInitInsert;
    }

    /* Valid Table Statistic List */
    aInsertStmtParamInfo->mOutValidTblStatList = aTableStatList;
    
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Delete Node의 Candidate Plan을 구한다.
 * @param[in]       aTransID                Transaction ID
 * @param[in]       aDBCStmt                DBC Statement
 * @param[in]       aSQLStmt                SQL Statement 객체
 * @param[in]       aQueryExprList          Query Expression List
 * @param[in,out]   aInternalBindIdx        Internal Bind Idx
 * @param[in,out]   aDeleteStmtParamInfo    Delete Statement Parameter Info
 * @param[in]       aEnv                    Environment
 * @remarks
 */
stlStatus qlncDeleteOptimizer( smlTransId                 aTransID,
                               qllDBCStmt               * aDBCStmt,
                               qllStatement             * aSQLStmt,
                               qloInitExprList          * aQueryExprList,
                               stlInt32                 * aInternalBindIdx,
                               qlncDeleteStmtParamInfo  * aDeleteStmtParamInfo,
                               qllEnv                   * aEnv )
{
    qlvInitDelete           * sInitDelete    = NULL;
    qlvInitDelete           * sOutInitDelete = NULL;

    stlInt32                  i;
    stlInt32                  sGlobalID;
    qlncParamInfo             sParamInfo;
    qlncCreateNodeParamInfo   sCreateNodeParamInfo;

    qlncQBMapArray          * sQBMapArray       = NULL;
    qlncTableMapArray       * sTableMapArray    = NULL;
    qlncNodeCommon          * sCandDeleteNode   = NULL;
    qlncPlanCommon          * sPlanDelete       = NULL;
    qlncPlanCommon          * sPlanReadTable    = NULL;
    qlncPlanCommon          * sPlanSubQueryList = NULL;

    qlncRefExprList           sSubQueryExprList;
    qlncRefExprList           sNonFilterSubQueryExprList;
    qlncRefExprList           sReadColumnList;
    qlncRefExprItem         * sRefCandExprItem  = NULL;
    qlvRefExprItem          * sRefInitExprItem  = NULL;

    qlncTarget              * sReturnTargetArr      = NULL;
    qlvInitTarget           * sInitReturnTargetArr  = NULL;
    qlvRefExprList          * sOuterColumnList      = NULL;

    qloValidTblStatList     * sTableStatList        = NULL;

    stlInt32                * sBindNodePos          = NULL;
    knlBindType             * sBindIOType           = NULL;
    qlvInitExpression       * sExpr                 = NULL;
    stlInt32                  sLoop                 = 0;

    
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aSQLStmt->mStmtType == QLL_STMT_DELETE_TYPE ) ||
                        ( aSQLStmt->mStmtType == QLL_STMT_DELETE_RETURNING_QUERY_TYPE ) ||
                        ( aSQLStmt->mStmtType == QLL_STMT_DELETE_RETURNING_INTO_TYPE ) ||
                        ( aSQLStmt->mStmtType == QLL_STMT_DELETE_WHERE_CURRENT_OF_TYPE ),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDeleteStmtParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDeleteStmtParamInfo->mStmtNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInternalBindIdx != NULL, QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Default Trace Information 시작
     ****************************************************************/

    if( QLL_IS_ENABLE_TRACE_LOGGER(aEnv) )
    {
        (void)qllTraceHead( QLL_TRACE_OPTIMIZER_DUMP_TITLE, aEnv );
        (void)qllTraceString( "--- Current SQL Query String ---\n", aEnv );

        /* Input Query String 출력 */
        STL_TRY( qllTraceBody( &QLL_CANDIDATE_MEM( aEnv ),
                               aEnv,
                               "%s\n",
                               aSQLStmt->mRetrySQL )
                 == STL_SUCCESS );
    }


    /****************************************************************
     * Parameter Information 설정
     ****************************************************************/

    sGlobalID = 0;
    sParamInfo.mTransID = aTransID;
    sParamInfo.mDBCStmt = aDBCStmt;
    sParamInfo.mSQLStmt = aSQLStmt;
    sParamInfo.mGlobalNodeID = &sGlobalID;
    sParamInfo.mGlobalInternalBindIdx = aInternalBindIdx;


    /****************************************************************
     * Prepare Candidate Nodes for Finding the Best Candidate Plan
     ****************************************************************/

    sInitDelete = (qlvInitDelete *) aDeleteStmtParamInfo->mStmtNode;

    if( QLL_IS_ENABLE_TRACE_LOGGER(aEnv) )
    {
        (void)qllTraceMidTitle( "[STEP 01] Created Node for Optimizer", aEnv );
    }


    /**
     * Query Block Array 생성
     */

    STL_TRY( qlncCreateQBMapArray( &sParamInfo,
                                   &sQBMapArray,
                                   aEnv )
             == STL_SUCCESS );


    /**
     * Table Map Array 생성
     */

    STL_TRY( qlncCreateTableMapArray( &sParamInfo,
                                      &sTableMapArray,
                                      aEnv )
             == STL_SUCCESS );


    /**
     * Create Node Parameter Information 설정
     */

    QLNC_INIT_CREATE_NODE_PARAM_INFO( &sCreateNodeParamInfo,
                                      &sParamInfo,
                                      sInitDelete->mReadTableNode,
                                      sInitDelete->mHintInfo,
                                      sTableMapArray,
                                      NULL,
                                      NULL,
                                      sQBMapArray );

    /* Delete Node에 대한 Candidate Node 생성 */
    STL_TRY( qlncCreateTableCandNode( &sCreateNodeParamInfo,
                                      aEnv )
             == STL_SUCCESS );

    STL_DASSERT( sCreateNodeParamInfo.mCandNode != NULL );
    sCandDeleteNode = sCreateNodeParamInfo.mCandNode;


    /**
     * Where절
     */

    if( sInitDelete->mWhereExpr != NULL )
    {
        qlncAndFilter   * sWhere    = NULL;

        STL_TRY( qlncConvertCondition( &sCreateNodeParamInfo,
                                       sInitDelete->mWhereExpr,
                                       QLNC_EXPR_PHRASE_WHERE,
                                       &sWhere,
                                       aEnv )
                 == STL_SUCCESS );

        /* Where절을 Delete Node로 Push */
        STL_DASSERT( sCandDeleteNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE );
        STL_DASSERT( ((qlncBaseTableNode*)sCandDeleteNode)->mFilter == NULL );
        ((qlncBaseTableNode*)sCandDeleteNode)->mFilter = sWhere;
    }


    /**
     * Read Column List
     */

    QLNC_INIT_LIST( &sReadColumnList );
    if( ((qlvInitBaseTableNode*)sInitDelete->mReadTableNode)->mRefColumnList != NULL )
    {
        sRefInitExprItem = ((qlvInitBaseTableNode*)sInitDelete->mReadTableNode)->mRefColumnList->mHead;
        while( sRefInitExprItem != NULL )
        {
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncRefExprItem ),
                                        (void**) &sRefCandExprItem,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            QLNC_INIT_LIST( &sSubQueryExprList );
            STL_TRY( qlncConvertExpression( &sCreateNodeParamInfo,
                                            sRefInitExprItem->mExprPtr,
                                            QLNC_EXPR_PHRASE_TARGET,
                                            &sSubQueryExprList,
                                            &(sRefCandExprItem->mExpr),
                                            aEnv )
                     == STL_SUCCESS );

            sRefCandExprItem->mNext = NULL;

            QLNC_APPEND_ITEM_TO_LIST( &sReadColumnList, sRefCandExprItem );

            sRefInitExprItem = sRefInitExprItem->mNext;
        }
    }


    /**
     * Return Target절
     */

    QLNC_INIT_LIST( &sNonFilterSubQueryExprList );
    if( sInitDelete->mReturnTargetCnt > 0 )
    {
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncTarget ) * sInitDelete->mReturnTargetCnt,
                                    (void**) &sReturnTargetArr,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        for( i = 0; i < sInitDelete->mReturnTargetCnt; i++ )
        {
            sReturnTargetArr[i].mDisplayPos = sInitDelete->mReturnTargetArray[i].mDisplayPos;

            QLNC_ALLOC_AND_COPY_STRING( sReturnTargetArr[i].mDisplayName,
                                        sInitDelete->mReturnTargetArray[i].mDisplayName,
                                        &QLL_CANDIDATE_MEM( aEnv ),
                                        aEnv );

            QLNC_ALLOC_AND_COPY_STRING( sReturnTargetArr[i].mAliasName,
                                        sInitDelete->mReturnTargetArray[i].mAliasName,
                                        &QLL_CANDIDATE_MEM( aEnv ),
                                        aEnv );

            QLNC_INIT_LIST( &sSubQueryExprList );

            STL_TRY( qlncConvertExpression( &sCreateNodeParamInfo,
                                            sInitDelete->mReturnTargetArray[i].mExpr,
                                            QLNC_EXPR_PHRASE_TARGET,
                                            &sSubQueryExprList,
                                            &(sReturnTargetArr[i].mExpr),
                                            aEnv )
                     == STL_SUCCESS );

            sReturnTargetArr[i].mDataTypeInfo = sInitDelete->mReturnTargetArray[i].mDataTypeInfo;

            if( sSubQueryExprList.mCount > 0 )
            {
                if( sNonFilterSubQueryExprList.mCount == 0 )
                {
                    QLNC_COPY_LIST_INFO( &sSubQueryExprList, &sNonFilterSubQueryExprList );
                }
                else
                {
                    sNonFilterSubQueryExprList.mTail->mNext = sSubQueryExprList.mHead;
                    sNonFilterSubQueryExprList.mTail = sSubQueryExprList.mTail;
                    sNonFilterSubQueryExprList.mCount += sSubQueryExprList.mCount;
                }
            }
        }
    }
    else
    {
        sReturnTargetArr = NULL;
    }


    /**
     * Optimizer 수행
     */

    /* Return SubQuery에 대한 Cost Based Optimizer 수행 */
    sRefCandExprItem = sNonFilterSubQueryExprList.mHead;
    while( sRefCandExprItem != NULL )
    {
        STL_DASSERT( sRefCandExprItem->mExpr->mType == QLNC_EXPR_TYPE_SUB_QUERY );

        STL_TRY( qlncQueryNodeOptimizer( &sParamInfo,
                                         ((qlncExprSubQuery*)(sRefCandExprItem->mExpr))->mNode,
                                         aEnv )
                 == STL_SUCCESS );

        sRefCandExprItem = sRefCandExprItem->mNext;
    }

    /* Delete Node에 대한 Cost Based Optimizer 수행 */
    STL_TRY( qlncDMLNodeOptimizer( &sParamInfo,
                                   sCandDeleteNode,
                                   &sCandDeleteNode,
                                   aEnv )
             == STL_SUCCESS );

    /****************************************************************
     * Default Trace Information 종료
     ****************************************************************/

    if( QLL_IS_ENABLE_TRACE_LOGGER(aEnv) )
    {
        (void)qllTraceTail( QLL_TRACE_OPTIMIZER_DUMP_TITLE, aEnv );
    }


    /****************************************************************
     * Make Candidate Plan
     ****************************************************************/

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( qloValidTblStatList ),
                                (void**) &sTableStatList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sTableStatList->mCount = 0;
    sTableStatList->mHead = NULL;
    sTableStatList->mTail = NULL;

    STL_TRY( qlncMakeCandPlanForDelete( aTransID,
                                        aDBCStmt,
                                        aSQLStmt,
                                        aQueryExprList,
                                        sTableStatList,
                                        sCandDeleteNode,
                                        &sReadColumnList,
                                        sInitDelete->mReturnTargetCnt,
                                        sReturnTargetArr,
                                        &sInitReturnTargetArr,
                                        &sOuterColumnList,
                                        &sPlanDelete,
                                        &sPlanReadTable,
                                        &sPlanSubQueryList,
                                        aEnv )
             == STL_SUCCESS );


    /****************************************************************
     * Statement Node 정보 구축
     ****************************************************************/

    /* Init Plan 공간 할당 */
    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlvInitDelete ),
                                (void**) & sOutInitDelete,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Init Plan 공통 정보 */
    sOutInitDelete->mInitPlan.mBindCount = sInitDelete->mInitPlan.mBindCount;
    sOutInitDelete->mInitPlan.mResultCursorProperty = sInitDelete->mInitPlan.mResultCursorProperty;

    STL_TRY( ellCopyObjectItem( & sOutInitDelete->mInitPlan.mValidationObjList,
                                sInitDelete->mInitPlan.mValidationObjList,
                                QLL_CODE_PLAN( aDBCStmt ),
                                ELL_ENV(aEnv) )
             == STL_SUCCESS );

    if( sInitDelete->mInitPlan.mBindCount > 0 )
    {
        STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                    STL_SIZEOF( stlInt32 ) * sInitDelete->mInitPlan.mBindCount,
                                    (void**) & sBindNodePos,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemcpy( sBindNodePos,
                   sInitDelete->mInitPlan.mBindNodePos,
                   STL_SIZEOF( stlInt32 ) * sInitDelete->mInitPlan.mBindCount );
    
        STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                    STL_SIZEOF( knlBindType ) * sInitDelete->mInitPlan.mBindCount,
                                    (void**) & sBindIOType,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemcpy( sBindIOType,
                   sInitDelete->mInitPlan.mBindIOType,
                   STL_SIZEOF( knlBindType ) * sInitDelete->mInitPlan.mBindCount );
    }
    else
    {
        sBindNodePos = NULL;
        sBindIOType  = NULL;
    }
    sOutInitDelete->mInitPlan.mBindNodePos = sBindNodePos;
    sOutInitDelete->mInitPlan.mBindIOType = sBindIOType;
    sOutInitDelete->mInitPlan.mHasHintError = sInitDelete->mInitPlan.mHasHintError;

    /* Version Conflict Resolution Action */
    sOutInitDelete->mSerialAction = sInitDelete->mSerialAction;

    /* Hint 정보 */
    sOutInitDelete->mHintInfo = NULL;   /* default */

    /* Write Table 정보 */
    stlMemcpy( &(sOutInitDelete->mTableHandle),
               &(sInitDelete->mTableHandle),
               STL_SIZEOF( ellDictHandle ) );
    sOutInitDelete->mTablePhyHandle = sInitDelete->mTablePhyHandle;

    /* Expression List */
    sOutInitDelete->mStmtExprList = NULL;    /* default */
    sOutInitDelete->mQueryExprList = NULL;   /* default */

    /* Read Table 정보 */
    sOutInitDelete->mReadTableNode = sPlanReadTable->mInitNode;

    /* WHERE expression */
    sOutInitDelete->mWhereExpr = NULL;   /* default */

    /* result offset .. limit */
    if( sInitDelete->mResultSkip == NULL )
    {
        sOutInitDelete->mResultSkip = NULL;
    }
    else
    {
        STL_TRY( qlvCopyExpr( sInitDelete->mResultSkip,
                              & sOutInitDelete->mResultSkip,
                              QLL_CODE_PLAN(aDBCStmt),
                              aEnv )
                 == STL_SUCCESS );
    }
    
    if( sInitDelete->mResultLimit == NULL )
    {
        sOutInitDelete->mResultLimit = NULL;
    }
    else
    {
        STL_TRY( qlvCopyExpr( sInitDelete->mResultLimit,
                              & sOutInitDelete->mResultLimit,
                              QLL_CODE_PLAN(aDBCStmt),
                              aEnv )
                 == STL_SUCCESS );
    }
    sOutInitDelete->mSkipCnt = sInitDelete->mSkipCnt;
    sOutInitDelete->mFetchCnt = sInitDelete->mFetchCnt;

    /* Delete 에 영향을 받는 Object */
    sOutInitDelete->mHasIndex = sInitDelete->mHasIndex;
    sOutInitDelete->mHasCheck = sInitDelete->mHasCheck;

    STL_TRY( qlncCopyDeleteRelObject( & sInitDelete->mRelObject,
                                      & sOutInitDelete->mRelObject,
                                      QLL_CODE_PLAN(aDBCStmt),
                                      aEnv )
             == STL_SUCCESS );

    /* RETURN ... INTO 관련 */
    sOutInitDelete->mReturnTargetCnt = sInitDelete->mReturnTargetCnt;
    sOutInitDelete->mReturnTargetArray = sInitReturnTargetArr;
    
    if( ( sInitDelete->mReturnTargetCnt > 0 ) &&
        ( sInitDelete->mIntoTargetArray != NULL ) )
    {
        STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN(aDBCStmt),
                                    STL_SIZEOF( qlvInitExpression ) * sInitDelete->mReturnTargetCnt,
                                    (void **) & sOutInitDelete->mIntoTargetArray,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        for( sLoop = 0 ; sLoop < sInitDelete->mReturnTargetCnt ; sLoop++ )
        {
            STL_TRY( qlvCopyExpr( & sInitDelete->mIntoTargetArray[ sLoop ],
                                  & sExpr,
                                  QLL_CODE_PLAN(aDBCStmt),
                                  aEnv )
                     == STL_SUCCESS );

            sOutInitDelete->mIntoTargetArray[ sLoop ] = *sExpr;
        }
    }
    else
    {
        sOutInitDelete->mIntoTargetArray = NULL;
    }
    
    /* WHERE CURRENT OF 관련  */
    QLNC_ALLOC_AND_COPY_STRING( sOutInitDelete->mCursorName,
                                sInitDelete->mCursorName,
                                QLL_CODE_PLAN( aDBCStmt ),
                                aEnv );
    sOutInitDelete->mCursorProcID = sInitDelete->mCursorProcID;

    /* target info */
    if( sInitDelete->mTargetInfo != NULL )
    {
        STL_TRY( qlfCopyTarget( sInitDelete->mTargetInfo,
                                & sOutInitDelete->mTargetInfo,
                                QLL_CODE_PLAN(aDBCStmt),
                                aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sOutInitDelete->mTargetInfo = NULL;
    }
    

    /****************************************************************
     * Output 설정
     ****************************************************************/

    /* Optimization Info */
    aDeleteStmtParamInfo->mOutReturnTargetArray = sInitReturnTargetArr;
    aDeleteStmtParamInfo->mOutReturnOuterColumnList = sOuterColumnList;
    aDeleteStmtParamInfo->mOutPlanDelete = sPlanDelete;
    aDeleteStmtParamInfo->mOutPlanReadTable = sPlanReadTable;
    aDeleteStmtParamInfo->mOutPlanReturnSubQueryList = sPlanSubQueryList;

    /* Init Plan Node for Statement */
    aDeleteStmtParamInfo->mOutStmtNode = (qlvInitNode *) sOutInitDelete;

    /* Valid Table Statistic List */
    aDeleteStmtParamInfo->mOutValidTblStatList = sTableStatList;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Update Node의 Candidate Plan을 구한다.
 * @param[in]       aTransID                Transaction ID
 * @param[in]       aDBCStmt                DBC Statement
 * @param[in]       aSQLStmt                SQL Statement 객체
 * @param[in]       aQueryExprList          Query Expression List
 * @param[in,out]   aInternalBindIdx        Internal Bind Idx
 * @param[in,out]   aUpdateStmtParamInfo    Update Statement Parameter Info
 * @param[in]       aEnv                    Environment
 * @remarks
 */
stlStatus qlncUpdateOptimizer( smlTransId                 aTransID,
                               qllDBCStmt               * aDBCStmt,
                               qllStatement             * aSQLStmt,
                               qloInitExprList          * aQueryExprList,
                               stlInt32                 * aInternalBindIdx,
                               qlncUpdateStmtParamInfo  * aUpdateStmtParamInfo,
                               qllEnv                   * aEnv )
{
    qlvInitUpdate           * sInitUpdate           = NULL;
    qlvInitUpdate           * sOutInitUpdate        = NULL;

    stlInt32                  i;
    stlInt32                  sGlobalID;
    qlncParamInfo             sParamInfo;
    qlncCreateNodeParamInfo   sCreateNodeParamInfo;

    qlncQBMapArray          * sQBMapArray           = NULL;
    qlncTableMapArray       * sTableMapArray        = NULL;
    qlncNodeCommon          * sCandReadTableNode    = NULL;
    qlncNodeCommon          * sCandWriteTableNode   = NULL;

    qlncRefExprList           sSubQueryExprList;
    qlncRefExprList           sSetSubQueryExprList;
    qlncRefExprList           sReturnSubQueryExprList;
    qlncRefExprList           sReadColumnList;
    qlncRefExprList           sWriteColumnList;
    qlncRefExprList           sShareReadColumnList;
    qlncRefExprList           sShareWriteColumnList;
    qlncRefExprList           sSetColExprList;
    qlncExprCommon         ** sSetValueArray        = NULL;
    qlncRefExprItem         * sRefCandExprItem      = NULL;
    qlvRefExprItem          * sRefInitExprItem      = NULL;
    qlncTarget              * sReturnTargetArr      = NULL;

    qloValidTblStatList     * sTableStatList        = NULL;

    stlInt32                * sBindNodePos          = NULL;
    knlBindType             * sBindIOType           = NULL;
    qlvInitExpression       * sExpr                 = NULL;
    stlInt32                  sLoop                 = 0;

    qlncPlanUpdateStmtParamInfo   sPlanUpdateStmtParamInfo;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aSQLStmt->mStmtType == QLL_STMT_UPDATE_TYPE ) ||
                        ( aSQLStmt->mStmtType == QLL_STMT_UPDATE_RETURNING_QUERY_TYPE ) ||
                        ( aSQLStmt->mStmtType == QLL_STMT_UPDATE_RETURNING_INTO_TYPE ) ||
                        ( aSQLStmt->mStmtType == QLL_STMT_UPDATE_WHERE_CURRENT_OF_TYPE ),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aUpdateStmtParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aUpdateStmtParamInfo->mStmtNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInternalBindIdx != NULL, QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Default Trace Information 시작
     ****************************************************************/

    if( QLL_IS_ENABLE_TRACE_LOGGER(aEnv) )
    {
        (void)qllTraceHead( QLL_TRACE_OPTIMIZER_DUMP_TITLE, aEnv );
        (void)qllTraceString( "--- Current SQL Query String ---\n", aEnv );

        /* Input Query String 출력 */
        STL_TRY( qllTraceBody( &QLL_CANDIDATE_MEM( aEnv ),
                               aEnv,
                               "%s\n",
                               aSQLStmt->mRetrySQL )
                 == STL_SUCCESS );
    }


    /****************************************************************
     * Parameter Information 설정
     ****************************************************************/

    sGlobalID = 0;
    sParamInfo.mTransID = aTransID;
    sParamInfo.mDBCStmt = aDBCStmt;
    sParamInfo.mSQLStmt = aSQLStmt;
    sParamInfo.mGlobalNodeID = &sGlobalID;
    sParamInfo.mGlobalInternalBindIdx = aInternalBindIdx;


    /****************************************************************
     * Prepare Candidate Nodes for Finding the Best Candidate Plan
     ****************************************************************/

    sInitUpdate = (qlvInitUpdate *) aUpdateStmtParamInfo->mStmtNode;

    if( QLL_IS_ENABLE_TRACE_LOGGER(aEnv) )
    {
        (void)qllTraceMidTitle( "[STEP 01] Created Node for Optimizer", aEnv );
    }

    /* Query Block Array 생성 */
    STL_TRY( qlncCreateQBMapArray( &sParamInfo,
                                   &sQBMapArray,
                                   aEnv )
             == STL_SUCCESS );

    /* Table Map Array 생성 */
    STL_TRY( qlncCreateTableMapArray( &sParamInfo,
                                      &sTableMapArray,
                                      aEnv )
             == STL_SUCCESS );


    /**
     * Read Table Node에 대한 Candidate Node 생성
     */

    /* Create Node Parameter Information 설정 */
    QLNC_INIT_CREATE_NODE_PARAM_INFO( &sCreateNodeParamInfo,
                                      &sParamInfo,
                                      sInitUpdate->mReadTableNode,
                                      sInitUpdate->mHintInfo,
                                      sTableMapArray,
                                      NULL,
                                      NULL,
                                      sQBMapArray );

    /* Read Table Node에 대한 Candidate Node 생성 */
    STL_TRY( qlncCreateTableCandNode( &sCreateNodeParamInfo,
                                      aEnv )
             == STL_SUCCESS );

    STL_DASSERT( sCreateNodeParamInfo.mCandNode != NULL );
    sCandReadTableNode = sCreateNodeParamInfo.mCandNode;


    /**
     * Write Table Node에 대한 Candidate Node 생성
     */

    /* Create Node Parameter Information 설정 */
    QLNC_INIT_CREATE_NODE_PARAM_INFO( &sCreateNodeParamInfo,
                                      &sParamInfo,
                                      sInitUpdate->mWriteTableNode,
                                      NULL,
                                      sTableMapArray,
                                      NULL,
                                      NULL,
                                      sQBMapArray );

    /* Write Table Node에 대한 Candidate Node 생성 */
    STL_TRY( qlncCreateTableCandNode( &sCreateNodeParamInfo,
                                      aEnv )
             == STL_SUCCESS );

    STL_DASSERT( sCreateNodeParamInfo.mCandNode != NULL );
    sCandWriteTableNode = sCreateNodeParamInfo.mCandNode;


    /**
     * Where절
     */

    if( sInitUpdate->mWhereExpr != NULL )
    {
        qlncAndFilter   * sWhere    = NULL;

        STL_TRY( qlncConvertCondition( &sCreateNodeParamInfo,
                                       sInitUpdate->mWhereExpr,
                                       QLNC_EXPR_PHRASE_WHERE,
                                       &sWhere,
                                       aEnv )
                 == STL_SUCCESS );

        /* Where절을 Read Node로 Push */
        STL_DASSERT( sCandReadTableNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE );
        STL_DASSERT( ((qlncBaseTableNode*)sCandReadTableNode)->mFilter == NULL );
        ((qlncBaseTableNode*)sCandReadTableNode)->mFilter = sWhere;
    }


    /**
     * Read Column List
     */

    QLNC_INIT_LIST( &sReadColumnList );
    if( ((qlvInitBaseTableNode*)(sInitUpdate->mReadTableNode))->mRefColumnList != NULL )
    {
        sRefInitExprItem =
            ((qlvInitBaseTableNode*)(sInitUpdate->mReadTableNode))->mRefColumnList->mHead;
        while( sRefInitExprItem != NULL )
        {
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncRefExprItem ),
                                        (void**) &sRefCandExprItem,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            QLNC_INIT_LIST( &sSubQueryExprList );
            STL_TRY( qlncConvertExpression( &sCreateNodeParamInfo,
                                            sRefInitExprItem->mExprPtr,
                                            QLNC_EXPR_PHRASE_TARGET,
                                            &sSubQueryExprList,
                                            &(sRefCandExprItem->mExpr),
                                            aEnv )
                     == STL_SUCCESS );

            sRefCandExprItem->mNext = NULL;

            QLNC_APPEND_ITEM_TO_LIST( &sReadColumnList, sRefCandExprItem );

            sRefInitExprItem = sRefInitExprItem->mNext;
        }
    }

    if( ((qlvInitBaseTableNode*)(sInitUpdate->mReadTableNode))->mRefRowIdColumn != NULL )
    {
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncRefExprItem ),
                                    (void**) &sRefCandExprItem,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        QLNC_INIT_LIST( &sSubQueryExprList );
        STL_TRY( qlncConvertExpression( &sCreateNodeParamInfo,
                                        ((qlvInitBaseTableNode*)(sInitUpdate->mReadTableNode))->mRefRowIdColumn,
                                        QLNC_EXPR_PHRASE_TARGET,
                                        &sSubQueryExprList,
                                        &(sRefCandExprItem->mExpr),
                                        aEnv )
                 == STL_SUCCESS );

        sRefCandExprItem->mNext = NULL;

        QLNC_APPEND_ITEM_TO_LIST( &sReadColumnList, sRefCandExprItem );
    }


    /**
     * Write Column List
     */

    QLNC_INIT_LIST( &sWriteColumnList );
    if( ((qlvInitBaseTableNode*)(sInitUpdate->mWriteTableNode))->mRefColumnList != NULL )
    {
        sRefInitExprItem =
            ((qlvInitBaseTableNode*)(sInitUpdate->mWriteTableNode))->mRefColumnList->mHead;
        while( sRefInitExprItem != NULL )
        {
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncRefExprItem ),
                                        (void**) &sRefCandExprItem,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            QLNC_INIT_LIST( &sSubQueryExprList );
            STL_TRY( qlncConvertExpression( &sCreateNodeParamInfo,
                                            sRefInitExprItem->mExprPtr,
                                            QLNC_EXPR_PHRASE_TARGET,
                                            &sSubQueryExprList,
                                            &(sRefCandExprItem->mExpr),
                                            aEnv )
                     == STL_SUCCESS );

            sRefCandExprItem->mNext = NULL;

            QLNC_APPEND_ITEM_TO_LIST( &sWriteColumnList, sRefCandExprItem );

            sRefInitExprItem = sRefInitExprItem->mNext;
        }
    }

    if( ((qlvInitBaseTableNode*)(sInitUpdate->mWriteTableNode))->mRefRowIdColumn != NULL )
    {
        qlvInitExpression   * sInitExpr = NULL;

        sInitExpr =
            ((qlvInitBaseTableNode*)(sInitUpdate->mWriteTableNode))->mRefRowIdColumn;

        /* Rowid의 Relation Node가 Read Table Node로 변경되어 있을 수 있으므로 변경시킨다. */
        sInitExpr->mExpr.mRowIdColumn->mRelationNode = sInitUpdate->mWriteTableNode;

        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncRefExprItem ),
                                    (void**) &sRefCandExprItem,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        QLNC_INIT_LIST( &sSubQueryExprList );
        STL_TRY( qlncConvertExpression( &sCreateNodeParamInfo,
                                        ((qlvInitBaseTableNode*)(sInitUpdate->mWriteTableNode))->mRefRowIdColumn,
                                        QLNC_EXPR_PHRASE_TARGET,
                                        &sSubQueryExprList,
                                        &(sRefCandExprItem->mExpr),
                                        aEnv )
                 == STL_SUCCESS );

        sRefCandExprItem->mNext = NULL;

        QLNC_APPEND_ITEM_TO_LIST( &sWriteColumnList, sRefCandExprItem );
    }


    /**
     * Set Column Expression List (Write Table Node)
     */

    QLNC_INIT_LIST( &sSetColExprList );
    STL_DASSERT( sInitUpdate->mSetColExprList != NULL );
    sRefInitExprItem = sInitUpdate->mSetColExprList->mHead;
    while( sRefInitExprItem != NULL )
    {
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncRefExprItem ),
                                    (void**) &sRefCandExprItem,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        QLNC_INIT_LIST( &sSubQueryExprList );
        STL_TRY( qlncConvertExpression( &sCreateNodeParamInfo,
                                        sRefInitExprItem->mExprPtr,
                                        QLNC_EXPR_PHRASE_TARGET,
                                        &sSubQueryExprList,
                                        &(sRefCandExprItem->mExpr),
                                        aEnv )
                 == STL_SUCCESS );

        sRefCandExprItem->mNext = NULL;

        QLNC_APPEND_ITEM_TO_LIST( &sSetColExprList, sRefCandExprItem );

        sRefInitExprItem = sRefInitExprItem->mNext;
    }


    /**
     * Set Column Value Expression Array (Read Table Node)
     */

    STL_DASSERT( sInitUpdate->mSetExpr != NULL );
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprCommon* ) * sInitUpdate->mSetColumnCnt,
                                (void**) &sSetValueArray,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_INIT_LIST( &sSetSubQueryExprList );
    for( i = 0; i < sInitUpdate->mSetColumnCnt; i++ )
    {
        QLNC_INIT_LIST( &sSubQueryExprList );
        STL_TRY( qlncConvertExpression( &sCreateNodeParamInfo,
                                        sInitUpdate->mSetExpr[i],
                                        QLNC_EXPR_PHRASE_TARGET,
                                        &sSubQueryExprList,
                                        &(sSetValueArray[i]),
                                        aEnv )
                 == STL_SUCCESS );

        if( sSubQueryExprList.mCount > 0 )
        {
            if( sSetSubQueryExprList.mCount == 0 )
            {
                QLNC_COPY_LIST_INFO( &sSubQueryExprList, &sSetSubQueryExprList );
            }
            else
            {
                sSetSubQueryExprList.mTail->mNext = sSubQueryExprList.mHead;
                sSetSubQueryExprList.mTail = sSubQueryExprList.mTail;
                sSetSubQueryExprList.mCount += sSubQueryExprList.mCount;
            }
        }
    }


    /**
     * Share Read Column List
     */

    QLNC_INIT_LIST( &sShareReadColumnList );
    if( sInitUpdate->mShareReadColList != NULL )
    {
        sRefInitExprItem = sInitUpdate->mShareReadColList->mHead;
        while( sRefInitExprItem != NULL )
        {
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncRefExprItem ),
                                        (void**) &sRefCandExprItem,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            QLNC_INIT_LIST( &sSubQueryExprList );
            STL_TRY( qlncConvertExpression( &sCreateNodeParamInfo,
                                            sRefInitExprItem->mExprPtr,
                                            QLNC_EXPR_PHRASE_TARGET,
                                            &sSubQueryExprList,
                                            &(sRefCandExprItem->mExpr),
                                            aEnv )
                     == STL_SUCCESS );

            sRefCandExprItem->mNext = NULL;

            QLNC_APPEND_ITEM_TO_LIST( &sShareReadColumnList, sRefCandExprItem );

            sRefInitExprItem = sRefInitExprItem->mNext;
        }
    }


    /**
     * Share Write Column List
     */

    QLNC_INIT_LIST( &sShareWriteColumnList );
    if( sInitUpdate->mShareWriteColList != NULL )
    {
        sRefInitExprItem = sInitUpdate->mShareWriteColList->mHead;
        while( sRefInitExprItem != NULL )
        {
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncRefExprItem ),
                                        (void**) &sRefCandExprItem,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            QLNC_INIT_LIST( &sSubQueryExprList );
            STL_TRY( qlncConvertExpression( &sCreateNodeParamInfo,
                                            sRefInitExprItem->mExprPtr,
                                            QLNC_EXPR_PHRASE_TARGET,
                                            &sSubQueryExprList,
                                            &(sRefCandExprItem->mExpr),
                                            aEnv )
                     == STL_SUCCESS );

            sRefCandExprItem->mNext = NULL;

            QLNC_APPEND_ITEM_TO_LIST( &sShareWriteColumnList, sRefCandExprItem );

            sRefInitExprItem = sRefInitExprItem->mNext;
        }
    }


    /**
     * Return Target절
     */

    QLNC_INIT_LIST( &sReturnSubQueryExprList );
    if( sInitUpdate->mReturnTargetCnt > 0 )
    {
        /* Return New절에 존재하는 Rowid들의 Relation Node가 Read Table로 되어 있을 경우
         * 이를 Write Table로 변경시킨다. */
        if( (sInitUpdate->mIsReturnNew == STL_TRUE) &&
            (((qlvInitBaseTableNode*)(sInitUpdate->mWriteTableNode))->mRefRowIdColumn != NULL) &&
            (((qlvInitBaseTableNode*)(sInitUpdate->mReadTableNode))->mRefRowIdColumn != NULL) )
        {
            for( i = 0; i < sInitUpdate->mReturnTargetCnt; i++ )
            {
                STL_TRY( qlvChangeRelationNodeForRowIdCol(
                             sInitUpdate->mReadTableNode,
                             sInitUpdate->mWriteTableNode,
                             sInitUpdate->mReturnTargetArray[ i ].mExpr,
                             aEnv )
                         == STL_SUCCESS );
            }
        }

        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncTarget ) * sInitUpdate->mReturnTargetCnt,
                                    (void**) &sReturnTargetArr,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        for( i = 0; i < sInitUpdate->mReturnTargetCnt; i++ )
        {
            sReturnTargetArr[i].mDisplayPos = sInitUpdate->mReturnTargetArray[i].mDisplayPos;

            QLNC_ALLOC_AND_COPY_STRING( sReturnTargetArr[i].mDisplayName,
                                        sInitUpdate->mReturnTargetArray[i].mDisplayName,
                                        &QLL_CANDIDATE_MEM( aEnv ),
                                        aEnv );

            QLNC_ALLOC_AND_COPY_STRING( sReturnTargetArr[i].mAliasName,
                                        sInitUpdate->mReturnTargetArray[i].mAliasName,
                                        &QLL_CANDIDATE_MEM( aEnv ),
                                        aEnv );

            QLNC_INIT_LIST( &sSubQueryExprList );

            STL_TRY( qlncConvertExpression( &sCreateNodeParamInfo,
                                            sInitUpdate->mReturnTargetArray[i].mExpr,
                                            QLNC_EXPR_PHRASE_TARGET,
                                            &sSubQueryExprList,
                                            &(sReturnTargetArr[i].mExpr),
                                            aEnv )
                     == STL_SUCCESS );

            sReturnTargetArr[i].mDataTypeInfo = sInitUpdate->mReturnTargetArray[i].mDataTypeInfo;

            if( sSubQueryExprList.mCount > 0 )
            {
                if( sReturnSubQueryExprList.mCount == 0 )
                {
                    QLNC_COPY_LIST_INFO( &sSubQueryExprList, &sReturnSubQueryExprList );
                }
                else
                {
                    sReturnSubQueryExprList.mTail->mNext = sSubQueryExprList.mHead;
                    sReturnSubQueryExprList.mTail = sSubQueryExprList.mTail;
                    sReturnSubQueryExprList.mCount += sSubQueryExprList.mCount;
                }
            }
        }
    }
    else
    {
        sReturnTargetArr = NULL;
    }


    /**
     * Optimizer 수행
     */

    /* SubQuery에 대한 Optimizer 수행 */
    sRefCandExprItem = sSetSubQueryExprList.mHead;
    while( sRefCandExprItem != NULL )
    {
        STL_DASSERT( sRefCandExprItem->mExpr->mType == QLNC_EXPR_TYPE_SUB_QUERY );

        STL_TRY( qlncQueryNodeOptimizer( &sParamInfo,
                                         ((qlncExprSubQuery*)(sRefCandExprItem->mExpr))->mNode,
                                         aEnv )
                 == STL_SUCCESS );

        sRefCandExprItem = sRefCandExprItem->mNext;
    }

    sRefCandExprItem = sReturnSubQueryExprList.mHead;
    while( sRefCandExprItem != NULL )
    {
        STL_DASSERT( sRefCandExprItem->mExpr->mType == QLNC_EXPR_TYPE_SUB_QUERY );

        STL_TRY( qlncQueryNodeOptimizer( &sParamInfo,
                                         ((qlncExprSubQuery*)(sRefCandExprItem->mExpr))->mNode,
                                         aEnv )
                 == STL_SUCCESS );

        sRefCandExprItem = sRefCandExprItem->mNext;
    }

    /* Read Table Node의 Cost Based Optimizer 수행 */
    STL_TRY( qlncDMLNodeOptimizer( &sParamInfo,
                                   sCandReadTableNode,
                                   &sCandReadTableNode,
                                   aEnv )
             == STL_SUCCESS );

    /* Write Table Node의 Cost Based Optimizer 수행 */
    STL_TRY( qlncDMLNodeOptimizer( &sParamInfo,
                                   sCandWriteTableNode,
                                   &sCandWriteTableNode,
                                   aEnv )
             == STL_SUCCESS );


    /****************************************************************
     * Default Trace Information 종료
     ****************************************************************/

    if( QLL_IS_ENABLE_TRACE_LOGGER(aEnv) )
    {
        (void)qllTraceTail( QLL_TRACE_OPTIMIZER_DUMP_TITLE, aEnv );
    }


    /****************************************************************
     * Make Candidate Plan
     ****************************************************************/

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( qloValidTblStatList ),
                                (void**) &sTableStatList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sTableStatList->mCount = 0;
    sTableStatList->mHead = NULL;
    sTableStatList->mTail = NULL;

    sPlanUpdateStmtParamInfo.mReadTableNode = sCandReadTableNode;
    sPlanUpdateStmtParamInfo.mWriteTableNode = sCandWriteTableNode;
    sPlanUpdateStmtParamInfo.mReadColumnList = &sReadColumnList;
    sPlanUpdateStmtParamInfo.mWriteColumnList = &sWriteColumnList;
    sPlanUpdateStmtParamInfo.mSetColumnCount = sInitUpdate->mSetColumnCnt;
    sPlanUpdateStmtParamInfo.mSetColExprList = &sSetColExprList;
    sPlanUpdateStmtParamInfo.mSetValueArray = sSetValueArray;
    sPlanUpdateStmtParamInfo.mReturnTargetCount = sInitUpdate->mReturnTargetCnt;
    sPlanUpdateStmtParamInfo.mReturnTargetArray = sReturnTargetArr;
    sPlanUpdateStmtParamInfo.mShareReadColumnList = &sShareReadColumnList;
    sPlanUpdateStmtParamInfo.mShareWriteColumnList = &sShareWriteColumnList;
    sPlanUpdateStmtParamInfo.mSetSubQueryExprList = &sSetSubQueryExprList;
    sPlanUpdateStmtParamInfo.mReturnSubQueryExprList = &sReturnSubQueryExprList;
    sPlanUpdateStmtParamInfo.mIsReturnNew = sInitUpdate->mIsReturnNew;
    sPlanUpdateStmtParamInfo.mOutReturnTargetArray = NULL;
    sPlanUpdateStmtParamInfo.mOutSetOuterColumnList = NULL;
    sPlanUpdateStmtParamInfo.mOutReturnOuterColumnList = NULL;
    sPlanUpdateStmtParamInfo.mOutSetExprArray = NULL;
    sPlanUpdateStmtParamInfo.mOutSetColExprList = NULL;
    sPlanUpdateStmtParamInfo.mOutShareReadColList = NULL;
    sPlanUpdateStmtParamInfo.mOutShareWriteColList = NULL;
    sPlanUpdateStmtParamInfo.mOutPlanRootNode = NULL;
    sPlanUpdateStmtParamInfo.mOutPlanReadTable = NULL;
    sPlanUpdateStmtParamInfo.mOutPlanWriteTable = NULL;
    sPlanUpdateStmtParamInfo.mOutPlanSetSubQueryList = NULL;
    sPlanUpdateStmtParamInfo.mOutPlanReturnSubQueryList = NULL;

    STL_TRY( qlncMakeCandPlanForUpdate( aTransID,
                                        aDBCStmt,
                                        aSQLStmt,
                                        aQueryExprList,
                                        sTableStatList,
                                        &sPlanUpdateStmtParamInfo,
                                        aEnv )
             == STL_SUCCESS );


    /****************************************************************
     * Statement Node 정보 구축
     ****************************************************************/

    /* Init Plan 공간 할당 */
    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlvInitUpdate ),
                                (void**) & sOutInitUpdate,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Init Plan 공통 정보 */
    sOutInitUpdate->mInitPlan.mBindCount = sInitUpdate->mInitPlan.mBindCount;
    sOutInitUpdate->mInitPlan.mResultCursorProperty = sInitUpdate->mInitPlan.mResultCursorProperty;

    STL_TRY( ellCopyObjectItem( & sOutInitUpdate->mInitPlan.mValidationObjList,
                                sInitUpdate->mInitPlan.mValidationObjList,
                                QLL_CODE_PLAN( aDBCStmt ),
                                ELL_ENV(aEnv) )
             == STL_SUCCESS );

    if( sInitUpdate->mInitPlan.mBindCount > 0 )
    {
        STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                    STL_SIZEOF( stlInt32 ) * sInitUpdate->mInitPlan.mBindCount,
                                    (void**) & sBindNodePos,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemcpy( sBindNodePos,
                   sInitUpdate->mInitPlan.mBindNodePos,
                   STL_SIZEOF( stlInt32 ) * sInitUpdate->mInitPlan.mBindCount );
    
        STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                    STL_SIZEOF( knlBindType ) * sInitUpdate->mInitPlan.mBindCount,
                                    (void**) & sBindIOType,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemcpy( sBindIOType,
                   sInitUpdate->mInitPlan.mBindIOType,
                   STL_SIZEOF( knlBindType ) * sInitUpdate->mInitPlan.mBindCount );
    }
    else
    {
        sBindNodePos = NULL;
        sBindIOType  = NULL;
    }
    sOutInitUpdate->mInitPlan.mBindNodePos = sBindNodePos;
    sOutInitUpdate->mInitPlan.mBindIOType = sBindIOType;
    sOutInitUpdate->mInitPlan.mHasHintError = sInitUpdate->mInitPlan.mHasHintError;

    /* Version Conflict Resolution Action */
    sOutInitUpdate->mSerialAction = sInitUpdate->mSerialAction;

    /* Hint 정보 */
    sOutInitUpdate->mHintInfo = NULL;   /* default */

    /* Write Table 정보 */
    stlMemcpy( &(sOutInitUpdate->mTableHandle),
               &(sInitUpdate->mTableHandle),
               STL_SIZEOF( ellDictHandle ) );
    sOutInitUpdate->mTablePhyHandle = sInitUpdate->mTablePhyHandle;

    /* Expression List */
    sOutInitUpdate->mStmtExprList = NULL;    /* default */
    sOutInitUpdate->mQueryExprList = NULL;   /* default */

    /* Read Table 정보 */
    sOutInitUpdate->mReadTableNode = sPlanUpdateStmtParamInfo.mOutPlanReadTable->mInitNode;

    /* Write Table 정보 */
    sOutInitUpdate->mWriteTableNode = sPlanUpdateStmtParamInfo.mOutPlanWriteTable->mInitNode;

    /* SET Column : Column Count 만큼 Array 로 관리됨 */
    sOutInitUpdate->mSetColumnCnt = sInitUpdate->mSetColumnCnt;
    
    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( ellDictHandle ) * sInitUpdate->mSetColumnCnt,
                                (void**) & (sOutInitUpdate->mSetColumnHandle),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemcpy( sOutInitUpdate->mSetColumnHandle,
               sInitUpdate->mSetColumnHandle,
               STL_SIZEOF( ellDictHandle ) * sInitUpdate->mSetColumnCnt );
    
    sOutInitUpdate->mSetExpr = sPlanUpdateStmtParamInfo.mOutSetExprArray;
    sOutInitUpdate->mSetColExprList = sPlanUpdateStmtParamInfo.mOutSetColExprList;

    /* Table Column : Table 의 전체 Column Count 만큼 Array 로 관리됨 */
    sOutInitUpdate->mTotalColumnCnt = sInitUpdate->mTotalColumnCnt;

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( stlInt32 ) * sInitUpdate->mTotalColumnCnt,
                                (void**) & sOutInitUpdate->mSetCodeStackIdx,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemcpy( sOutInitUpdate->mSetCodeStackIdx,
               sInitUpdate->mSetCodeStackIdx,
               STL_SIZEOF( stlInt32 ) * sInitUpdate->mTotalColumnCnt );

    /* WHERE expression */
    sOutInitUpdate->mWhereExpr = NULL;   /* default */

    /* result offset .. limit */
    if( sInitUpdate->mResultSkip == NULL )
    {
        sOutInitUpdate->mResultSkip = NULL;
    }
    else
    {
        STL_TRY( qlvCopyExpr( sInitUpdate->mResultSkip,
                              & sOutInitUpdate->mResultSkip,
                              QLL_CODE_PLAN(aDBCStmt),
                              aEnv )
                 == STL_SUCCESS );
    }
    
    if( sInitUpdate->mResultLimit == NULL )
    {
        sOutInitUpdate->mResultLimit = NULL;
    }
    else
    {
        STL_TRY( qlvCopyExpr( sInitUpdate->mResultLimit,
                              & sOutInitUpdate->mResultLimit,
                              QLL_CODE_PLAN(aDBCStmt),
                              aEnv )
                 == STL_SUCCESS );
    }
    sOutInitUpdate->mSkipCnt = sInitUpdate->mSkipCnt;
    sOutInitUpdate->mFetchCnt = sInitUpdate->mFetchCnt;
    
    /* Update 에 영향을 받는 Object */
    sOutInitUpdate->mHasCheck = sInitUpdate->mHasCheck;
    sOutInitUpdate->mHasIndex = sInitUpdate->mHasIndex;

    STL_TRY( qlncCopyUpdateRelObject( & sInitUpdate->mRelObject,
                                      & sOutInitUpdate->mRelObject,
                                      QLL_CODE_PLAN(aDBCStmt),
                                      aEnv )
             == STL_SUCCESS );

    /* RETURN ... INTO 관련 */
    sOutInitUpdate->mIsReturnNew = sInitUpdate->mIsReturnNew;
    sOutInitUpdate->mReturnTargetCnt = sInitUpdate->mReturnTargetCnt;
    sOutInitUpdate->mReturnTargetArray = sPlanUpdateStmtParamInfo.mOutReturnTargetArray;
    
    if( ( sInitUpdate->mReturnTargetCnt > 0 ) &&
        ( sInitUpdate->mIntoTargetArray != NULL ) )
    {
        STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN(aDBCStmt),
                                    STL_SIZEOF( qlvInitExpression ) * sInitUpdate->mReturnTargetCnt,
                                    (void **) & sOutInitUpdate->mIntoTargetArray,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        for( sLoop = 0 ; sLoop < sInitUpdate->mReturnTargetCnt ; sLoop++ )
        {
            STL_TRY( qlvCopyExpr( & sInitUpdate->mIntoTargetArray[ sLoop ],
                                  & sExpr,
                                  QLL_CODE_PLAN(aDBCStmt),
                                  aEnv )
                     == STL_SUCCESS );

            sOutInitUpdate->mIntoTargetArray[ sLoop ] = *sExpr;
        }
    }
    else
    {
        sOutInitUpdate->mIntoTargetArray = NULL;
    }

    /* sOutInitUpdate->mIntoTargetArray = ; */
    sOutInitUpdate->mShareReadColList = sPlanUpdateStmtParamInfo.mOutShareReadColList;
    sOutInitUpdate->mShareWriteColList = sPlanUpdateStmtParamInfo.mOutShareWriteColList;

    /* WHERE CURRENT OF 관련  */
    QLNC_ALLOC_AND_COPY_STRING( sOutInitUpdate->mCursorName,
                                sInitUpdate->mCursorName,
                                QLL_CODE_PLAN( aDBCStmt ),
                                aEnv );
    sOutInitUpdate->mCursorProcID = sInitUpdate->mCursorProcID;
    
    /* target info */
    if( sInitUpdate->mTargetInfo != NULL )
    {
        STL_TRY( qlfCopyTarget( sInitUpdate->mTargetInfo,
                                & sOutInitUpdate->mTargetInfo,
                                QLL_CODE_PLAN(aDBCStmt),
                                aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sOutInitUpdate->mTargetInfo = NULL;
    }
    
    
    /****************************************************************
     * Output 설정
     ****************************************************************/

    /* Optimization Info */
    aUpdateStmtParamInfo->mOutReturnTargetArray = sPlanUpdateStmtParamInfo.mOutReturnTargetArray;
    aUpdateStmtParamInfo->mOutSetOuterColumnList = sPlanUpdateStmtParamInfo.mOutSetOuterColumnList;
    aUpdateStmtParamInfo->mOutReturnOuterColumnList = sPlanUpdateStmtParamInfo.mOutReturnOuterColumnList;
    aUpdateStmtParamInfo->mOutSetExprArray = sPlanUpdateStmtParamInfo.mOutSetExprArray;
    aUpdateStmtParamInfo->mOutSetColExprList = sPlanUpdateStmtParamInfo.mOutSetColExprList;
    aUpdateStmtParamInfo->mOutShareReadColList = sPlanUpdateStmtParamInfo.mOutShareReadColList;
    aUpdateStmtParamInfo->mOutShareWriteColList = sPlanUpdateStmtParamInfo.mOutShareWriteColList;
    aUpdateStmtParamInfo->mOutPlanRootNode = sPlanUpdateStmtParamInfo.mOutPlanRootNode;
    aUpdateStmtParamInfo->mOutPlanReadTable = sPlanUpdateStmtParamInfo.mOutPlanReadTable;
    aUpdateStmtParamInfo->mOutPlanWriteTable = sPlanUpdateStmtParamInfo.mOutPlanWriteTable;
    aUpdateStmtParamInfo->mOutPlanSetSubQueryList = sPlanUpdateStmtParamInfo.mOutPlanSetSubQueryList;
    aUpdateStmtParamInfo->mOutPlanReturnSubQueryList = sPlanUpdateStmtParamInfo.mOutPlanReturnSubQueryList;

    /* Init Plan Node for Statement */
    aUpdateStmtParamInfo->mOutStmtNode = (qlvInitNode *) sOutInitUpdate;

    /* Valid Table Statistic List */
    aUpdateStmtParamInfo->mOutValidTblStatList = sTableStatList;
    

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Update Relation Object를 복사한다.
 * @param[in]   aSrcRelObject    Source Relation Object
 * @param[out]  aDestRelObject   Dest Relation Object
 * @param[in]   aRegionMem       Region Memory
 * @param[in]   aEnv             Environment
 * @remarks
 */
stlStatus qlncCopyUpdateRelObject( qlvInitUpdateRelObject  * aSrcRelObject,
                                   qlvInitUpdateRelObject  * aDestRelObject,
                                   knlRegionMem            * aRegionMem,
                                   qllEnv                  * aEnv )
{
    qlvInitUpdateRelObject  * sDestRelObject = aDestRelObject;
    

    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aSrcRelObject != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDestRelObject != NULL, QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Macros
     ****************************************************************/

#define QLNC_ALLOC_COPY_ARRAY( aSrc, aDest, aSize )             \
    {                                                           \
        if( (aSize) > 0 )                                       \
        {                                                       \
            STL_TRY( knlAllocRegionMem( aRegionMem,             \
                                        (aSize),                \
                                        (void**) & (aDest),     \
                                        KNL_ENV( aEnv ) )       \
                     == STL_SUCCESS );                          \
            stlMemcpy( (aDest),                                 \
                       (aSrc),                                  \
                       (aSize) );                               \
        }                                                       \
        else                                                    \
        {                                                       \
            (aDest) = NULL;                                     \
        }                                                       \
    }

#define QLNC_COPY_DICT_HANDLE_ARRAY( aSrc, aDest, aCnt )                \
    {                                                                   \
        QLNC_ALLOC_COPY_ARRAY( aSrc,                                    \
                               aDest,                                   \
                               STL_SIZEOF( ellDictHandle ) * (aCnt) );  \
    }

#define QLNC_COPY_PHYSICAL_HANDLE_ARRAY( aSrc, aDest, aCnt )    \
    {                                                           \
        QLNC_ALLOC_COPY_ARRAY( aSrc,                            \
                               aDest,                           \
                               STL_SIZEOF( void* ) * (aCnt) );  \
    }

#define QLNC_COPY_BOOL_ARRAY( aSrc, aDest, aCnt )                   \
    {                                                               \
        QLNC_ALLOC_COPY_ARRAY( aSrc,                                \
                               aDest,                               \
                               STL_SIZEOF( stlBool ) * (aCnt) );    \
    }

#define QLNC_COPY_IDX_ARRAY( aSrc, aDest, aCnt )                        \
    {                                                                   \
        QLNC_ALLOC_COPY_ARRAY( aSrc,                                    \
                               aDest,                                   \
                               STL_SIZEOF( stlInt32 ) * (aCnt) );       \
    }


    /****************************************************************
     * 정보 설정
     ****************************************************************/
    
    /**
     * Primary Key
     */

    sDestRelObject->mTotalPrimaryKeyCnt = aSrcRelObject->mTotalPrimaryKeyCnt;
    
    QLNC_COPY_DICT_HANDLE_ARRAY( aSrcRelObject->mTotalPrimaryKeyHandle,
                                 sDestRelObject->mTotalPrimaryKeyHandle,
                                 aSrcRelObject->mTotalPrimaryKeyCnt );

    sDestRelObject->mAffectPrimaryKeyCnt = aSrcRelObject->mAffectPrimaryKeyCnt;

    QLNC_COPY_DICT_HANDLE_ARRAY( aSrcRelObject->mAffectPrimaryKeyHandle,
                                 sDestRelObject->mAffectPrimaryKeyHandle,
                                 aSrcRelObject->mAffectPrimaryKeyCnt );
    
    QLNC_COPY_PHYSICAL_HANDLE_ARRAY( aSrcRelObject->mAffectPrimaryKeyIndexPhyHandle,
                                     sDestRelObject->mAffectPrimaryKeyIndexPhyHandle,
                                     aSrcRelObject->mAffectPrimaryKeyCnt );

    /**
     * Unique Key
     */

    sDestRelObject->mTotalUniqueKeyCnt = aSrcRelObject->mTotalUniqueKeyCnt;

    QLNC_COPY_DICT_HANDLE_ARRAY( aSrcRelObject->mTotalUniqueKeyHandle,
                                 sDestRelObject->mTotalUniqueKeyHandle,
                                 aSrcRelObject->mTotalUniqueKeyCnt );

    sDestRelObject->mAffectUniqueKeyCnt = aSrcRelObject->mAffectUniqueKeyCnt;

    QLNC_COPY_DICT_HANDLE_ARRAY( aSrcRelObject->mAffectUniqueKeyHandle,
                                 sDestRelObject->mAffectUniqueKeyHandle,
                                 aSrcRelObject->mAffectUniqueKeyCnt );
    
    QLNC_COPY_PHYSICAL_HANDLE_ARRAY( aSrcRelObject->mAffectUniqueKeyIndexPhyHandle,
                                     sDestRelObject->mAffectUniqueKeyIndexPhyHandle,
                                     aSrcRelObject->mAffectUniqueKeyCnt );

    /**
     * @todo Foreign Key
     */

    sDestRelObject->mTotalForeignKeyCnt = aSrcRelObject->mTotalForeignKeyCnt;

    QLNC_COPY_DICT_HANDLE_ARRAY( aSrcRelObject->mTotalForeignKeyHandle,
                                 sDestRelObject->mTotalForeignKeyHandle,
                                 aSrcRelObject->mTotalForeignKeyCnt );
    
    sDestRelObject->mAffectForeignKeyCnt = aSrcRelObject->mAffectForeignKeyCnt;
    
    /**
     * @todo Child Foreign Key
     */

    sDestRelObject->mTotalChildForeignKeyCnt = aSrcRelObject->mTotalChildForeignKeyCnt;

    QLNC_COPY_DICT_HANDLE_ARRAY( aSrcRelObject->mTotalChildForeignKeyHandle,
                                 sDestRelObject->mTotalChildForeignKeyHandle,
                                 aSrcRelObject->mTotalChildForeignKeyCnt );

    sDestRelObject->mAffectChildForeignKeyCnt = aSrcRelObject->mAffectChildForeignKeyCnt;
    
    /**
     * Unique Index
     */

    sDestRelObject->mTotalUniqueIndexCnt = aSrcRelObject->mTotalUniqueIndexCnt;

    QLNC_COPY_DICT_HANDLE_ARRAY( aSrcRelObject->mTotalUniqueIndexHandle,
                                 sDestRelObject->mTotalUniqueIndexHandle,
                                 aSrcRelObject->mTotalUniqueIndexCnt );
    
    sDestRelObject->mAffectUniqueIndexCnt = aSrcRelObject->mAffectUniqueIndexCnt;

    QLNC_COPY_DICT_HANDLE_ARRAY( aSrcRelObject->mAffectUniqueIndexHandle,
                                 sDestRelObject->mAffectUniqueIndexHandle,
                                 aSrcRelObject->mAffectUniqueIndexCnt );
    
    QLNC_COPY_PHYSICAL_HANDLE_ARRAY( aSrcRelObject->mAffectUniqueIndexPhyHandle,
                                     sDestRelObject->mAffectUniqueIndexPhyHandle,
                                     aSrcRelObject->mAffectUniqueIndexCnt );

    /**
     * Non-Unique Index
     */

    sDestRelObject->mTotalNonUniqueIndexCnt = aSrcRelObject->mTotalNonUniqueIndexCnt;

    QLNC_COPY_DICT_HANDLE_ARRAY( aSrcRelObject->mTotalNonUniqueIndexHandle,
                                 sDestRelObject->mTotalNonUniqueIndexHandle,
                                 aSrcRelObject->mTotalNonUniqueIndexCnt );

    sDestRelObject->mAffectNonUniqueIndexCnt = aSrcRelObject->mAffectNonUniqueIndexCnt;

    QLNC_COPY_DICT_HANDLE_ARRAY( aSrcRelObject->mAffectNonUniqueIndexHandle,
                                 sDestRelObject->mAffectNonUniqueIndexHandle,
                                 aSrcRelObject->mAffectNonUniqueIndexCnt );
    
    QLNC_COPY_PHYSICAL_HANDLE_ARRAY( aSrcRelObject->mAffectNonUniqueIndexPhyHandle,
                                     sDestRelObject->mAffectNonUniqueIndexPhyHandle,
                                     aSrcRelObject->mAffectNonUniqueIndexCnt );

    /**
     * Check Not Null
     */

    sDestRelObject->mTotalCheckNotNullCnt = aSrcRelObject->mTotalCheckNotNullCnt;

    QLNC_COPY_DICT_HANDLE_ARRAY( aSrcRelObject->mTotalCheckNotNullHandle,
                                 sDestRelObject->mTotalCheckNotNullHandle,
                                 aSrcRelObject->mTotalCheckNotNullCnt );
    
    sDestRelObject->mAffectCheckNotNullCnt = aSrcRelObject->mAffectCheckNotNullCnt;

    QLNC_COPY_DICT_HANDLE_ARRAY( aSrcRelObject->mAffectCheckNotNullHandle,
                                 sDestRelObject->mAffectCheckNotNullHandle,
                                 aSrcRelObject->mAffectCheckNotNullCnt );
    
    QLNC_COPY_BOOL_ARRAY( aSrcRelObject->mAffectCheckNotNullDefer,
                          sDestRelObject->mAffectCheckNotNullDefer,
                          aSrcRelObject->mAffectCheckNotNullCnt );

    /**
     * Check Clause
     */

    sDestRelObject->mTotalCheckClauseCnt = aSrcRelObject->mTotalCheckClauseCnt;

    QLNC_COPY_DICT_HANDLE_ARRAY( aSrcRelObject->mTotalCheckClauseHandle,
                                 sDestRelObject->mTotalCheckClauseHandle,
                                 aSrcRelObject->mTotalCheckClauseCnt );

    sDestRelObject->mAffectCheckClauseCnt = aSrcRelObject->mAffectCheckClauseCnt;

    /**
     * Identity Column
     */

    sDestRelObject->mHasAffectIdentity = aSrcRelObject->mHasAffectIdentity;

    stlMemcpy( & sDestRelObject->mIdentityColumnHandle,
               & aSrcRelObject->mIdentityColumnHandle,
               STL_SIZEOF(ellDictHandle) );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Delete Relation Object를 복사한다.
 * @param[in]   aSrcRelObject    Source Relation Object
 * @param[out]  aDestRelObject   Dest Relation Object
 * @param[in]   aRegionMem       Region Memory
 * @param[in]   aEnv             Environment
 * @remarks
 */
stlStatus qlncCopyDeleteRelObject( qlvInitDeleteRelObject  * aSrcRelObject,
                                   qlvInitDeleteRelObject  * aDestRelObject,
                                   knlRegionMem            * aRegionMem,
                                   qllEnv                  * aEnv )
{
    qlvInitDeleteRelObject  * sDestRelObject = aDestRelObject;
    

    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aSrcRelObject != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDestRelObject != NULL, QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Macros
     ****************************************************************/

#define QLNC_ALLOC_COPY_ARRAY( aSrc, aDest, aSize )             \
    {                                                           \
        if( (aSize) > 0 )                                       \
        {                                                       \
            STL_TRY( knlAllocRegionMem( aRegionMem,             \
                                        (aSize),                \
                                        (void**) & (aDest),     \
                                        KNL_ENV( aEnv ) )       \
                     == STL_SUCCESS );                          \
            stlMemcpy( (aDest),                                 \
                       (aSrc),                                  \
                       (aSize) );                               \
        }                                                       \
        else                                                    \
        {                                                       \
            (aDest) = NULL;                                     \
        }                                                       \
    }

#define QLNC_COPY_DICT_HANDLE_ARRAY( aSrc, aDest, aCnt )                \
    {                                                                   \
        QLNC_ALLOC_COPY_ARRAY( aSrc,                                    \
                               aDest,                                   \
                               STL_SIZEOF( ellDictHandle ) * (aCnt) );  \
    }

#define QLNC_COPY_PHYSICAL_HANDLE_ARRAY( aSrc, aDest, aCnt )    \
    {                                                           \
        QLNC_ALLOC_COPY_ARRAY( aSrc,                            \
                               aDest,                           \
                               STL_SIZEOF( void* ) * (aCnt) );  \
    }


    /****************************************************************
     * 정보 설정
     ****************************************************************/
    
    /**
     * Primary Key
     */

    sDestRelObject->mPrimaryKeyCnt = aSrcRelObject->mPrimaryKeyCnt;
    
    QLNC_COPY_DICT_HANDLE_ARRAY( aSrcRelObject->mPrimaryKeyHandle,
                                 sDestRelObject->mPrimaryKeyHandle,
                                 aSrcRelObject->mPrimaryKeyCnt );
    
    QLNC_COPY_PHYSICAL_HANDLE_ARRAY( aSrcRelObject->mPrimaryKeyIndexPhyHandle,
                                     sDestRelObject->mPrimaryKeyIndexPhyHandle,
                                     aSrcRelObject->mPrimaryKeyCnt );


    /**
     * Unique Key
     */

    sDestRelObject->mUniqueKeyCnt = aSrcRelObject->mUniqueKeyCnt;

    QLNC_COPY_DICT_HANDLE_ARRAY( aSrcRelObject->mUniqueKeyHandle,
                                 sDestRelObject->mUniqueKeyHandle,
                                 aSrcRelObject->mUniqueKeyCnt );
    
    QLNC_COPY_PHYSICAL_HANDLE_ARRAY( aSrcRelObject->mUniqueKeyIndexPhyHandle,
                                     sDestRelObject->mUniqueKeyIndexPhyHandle,
                                     aSrcRelObject->mUniqueKeyCnt );

    
    /**
     * Foreign Key
     */

    sDestRelObject->mForeignKeyCnt = aSrcRelObject->mForeignKeyCnt;

    QLNC_COPY_DICT_HANDLE_ARRAY( aSrcRelObject->mForeignKeyHandle,
                                 sDestRelObject->mForeignKeyHandle,
                                 aSrcRelObject->mForeignKeyCnt );
    
    QLNC_COPY_PHYSICAL_HANDLE_ARRAY( aSrcRelObject->mForeignKeyIndexPhyHandle,
                                     sDestRelObject->mForeignKeyIndexPhyHandle,
                                     aSrcRelObject->mForeignKeyCnt );

    
    /**
     * Child Foreign Key
     */

    sDestRelObject->mChildForeignKeyCnt = aSrcRelObject->mChildForeignKeyCnt;

    QLNC_COPY_DICT_HANDLE_ARRAY( aSrcRelObject->mChildForeignKeyHandle,
                                 sDestRelObject->mChildForeignKeyHandle,
                                 aSrcRelObject->mChildForeignKeyCnt );

    
    /**
     * Unique Index
     */

    sDestRelObject->mUniqueIndexCnt = aSrcRelObject->mUniqueIndexCnt;

    QLNC_COPY_DICT_HANDLE_ARRAY( aSrcRelObject->mUniqueIndexHandle,
                                 sDestRelObject->mUniqueIndexHandle,
                                 aSrcRelObject->mUniqueIndexCnt );
    
    QLNC_COPY_PHYSICAL_HANDLE_ARRAY( aSrcRelObject->mUniqueIndexPhyHandle,
                                     sDestRelObject->mUniqueIndexPhyHandle,
                                     aSrcRelObject->mUniqueIndexCnt );

    
    /**
     * Non-Unique Index
     */

    sDestRelObject->mNonUniqueIndexCnt = aSrcRelObject->mNonUniqueIndexCnt;

    QLNC_COPY_DICT_HANDLE_ARRAY( aSrcRelObject->mNonUniqueIndexHandle,
                                 sDestRelObject->mNonUniqueIndexHandle,
                                 aSrcRelObject->mNonUniqueIndexCnt );
    
    QLNC_COPY_PHYSICAL_HANDLE_ARRAY( aSrcRelObject->mNonUniqueIndexPhyHandle,
                                     sDestRelObject->mNonUniqueIndexPhyHandle,
                                     aSrcRelObject->mNonUniqueIndexCnt );

    /**
     * Deferrable NOT NULL
     */

    sDestRelObject->mDeferNotNullCnt = aSrcRelObject->mDeferNotNullCnt;

    QLNC_COPY_DICT_HANDLE_ARRAY( aSrcRelObject->mDeferNotNullHandle,
                                 sDestRelObject->mDeferNotNullHandle,
                                 aSrcRelObject->mDeferNotNullCnt );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Insert Relation Object를 복사한다.
 * @param[in]   aSrcRelObject    Source Relation Object
 * @param[out]  aDestRelObject   Dest Relation Object
 * @param[in]   aRegionMem       Region Memory
 * @param[in]   aEnv             Environment
 * @remarks
 */
stlStatus qlncCopyInsertRelObject( qlvInitInsertRelObject  * aSrcRelObject,
                                   qlvInitInsertRelObject  * aDestRelObject,
                                   knlRegionMem            * aRegionMem,
                                   qllEnv                  * aEnv )
{
    qlvInitInsertRelObject  * sDestRelObject = aDestRelObject;
    

    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aSrcRelObject != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDestRelObject != NULL, QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Macros
     ****************************************************************/

#define QLNC_ALLOC_COPY_ARRAY( aSrc, aDest, aSize )             \
    {                                                           \
        if( (aSize) > 0 )                                       \
        {                                                       \
            STL_TRY( knlAllocRegionMem( aRegionMem,             \
                                        (aSize),                \
                                        (void**) & (aDest),     \
                                        KNL_ENV( aEnv ) )       \
                     == STL_SUCCESS );                          \
            stlMemcpy( (aDest),                                 \
                       (aSrc),                                  \
                       (aSize) );                               \
        }                                                       \
        else                                                    \
        {                                                       \
            (aDest) = NULL;                                     \
        }                                                       \
    }

#define QLNC_COPY_DICT_HANDLE_ARRAY( aSrc, aDest, aCnt )                \
    {                                                                   \
        QLNC_ALLOC_COPY_ARRAY( aSrc,                                    \
                               aDest,                                   \
                               STL_SIZEOF( ellDictHandle ) * (aCnt) );  \
    }

#define QLNC_COPY_PHYSICAL_HANDLE_ARRAY( aSrc, aDest, aCnt )    \
    {                                                           \
        QLNC_ALLOC_COPY_ARRAY( aSrc,                            \
                               aDest,                           \
                               STL_SIZEOF( void* ) * (aCnt) );  \
    }

#define QLNC_COPY_IDX_ARRAY( aSrc, aDest, aCnt )                        \
    {                                                                   \
        QLNC_ALLOC_COPY_ARRAY( aSrc,                                    \
                               aDest,                                   \
                               STL_SIZEOF( stlInt32 ) * (aCnt) );       \
    }

    
    /****************************************************************
     * 정보 설정
     ****************************************************************/
    
    /**
     * Primary Key
     */

    sDestRelObject->mPrimaryKeyCnt = aSrcRelObject->mPrimaryKeyCnt;
    
    QLNC_COPY_DICT_HANDLE_ARRAY( aSrcRelObject->mPrimaryKeyHandle,
                                 sDestRelObject->mPrimaryKeyHandle,
                                 aSrcRelObject->mPrimaryKeyCnt );
    
    QLNC_COPY_PHYSICAL_HANDLE_ARRAY( aSrcRelObject->mPrimaryKeyIndexPhyHandle,
                                     sDestRelObject->mPrimaryKeyIndexPhyHandle,
                                     aSrcRelObject->mPrimaryKeyCnt );


    /**
     * Unique Key
     */

    sDestRelObject->mUniqueKeyCnt = aSrcRelObject->mUniqueKeyCnt;

    QLNC_COPY_DICT_HANDLE_ARRAY( aSrcRelObject->mUniqueKeyHandle,
                                 sDestRelObject->mUniqueKeyHandle,
                                 aSrcRelObject->mUniqueKeyCnt );
    
    QLNC_COPY_PHYSICAL_HANDLE_ARRAY( aSrcRelObject->mUniqueKeyIndexPhyHandle,
                                     sDestRelObject->mUniqueKeyIndexPhyHandle,
                                     aSrcRelObject->mUniqueKeyCnt );

    
    /**
     * Foreign Key
     */

    sDestRelObject->mForeignKeyCnt = aSrcRelObject->mForeignKeyCnt;

    QLNC_COPY_DICT_HANDLE_ARRAY( aSrcRelObject->mForeignKeyHandle,
                                 sDestRelObject->mForeignKeyHandle,
                                 aSrcRelObject->mForeignKeyCnt );
    
    QLNC_COPY_PHYSICAL_HANDLE_ARRAY( aSrcRelObject->mForeignKeyIndexPhyHandle,
                                     sDestRelObject->mForeignKeyIndexPhyHandle,
                                     aSrcRelObject->mForeignKeyCnt );

    
    /**
     * Unique Index
     */

    sDestRelObject->mUniqueIndexCnt = aSrcRelObject->mUniqueIndexCnt;

    QLNC_COPY_DICT_HANDLE_ARRAY( aSrcRelObject->mUniqueIndexHandle,
                                 sDestRelObject->mUniqueIndexHandle,
                                 aSrcRelObject->mUniqueIndexCnt );
    
    QLNC_COPY_PHYSICAL_HANDLE_ARRAY( aSrcRelObject->mUniqueIndexPhyHandle,
                                     sDestRelObject->mUniqueIndexPhyHandle,
                                     aSrcRelObject->mUniqueIndexCnt );

    
    /**
     * Non-Unique Index
     */

    sDestRelObject->mNonUniqueIndexCnt = aSrcRelObject->mNonUniqueIndexCnt;

    QLNC_COPY_DICT_HANDLE_ARRAY( aSrcRelObject->mNonUniqueIndexHandle,
                                 sDestRelObject->mNonUniqueIndexHandle,
                                 aSrcRelObject->mNonUniqueIndexCnt );
    
    QLNC_COPY_PHYSICAL_HANDLE_ARRAY( aSrcRelObject->mNonUniqueIndexPhyHandle,
                                     sDestRelObject->mNonUniqueIndexPhyHandle,
                                     aSrcRelObject->mNonUniqueIndexCnt );

    
    /**
     * Check Not Null
     */

    sDestRelObject->mCheckNotNullCnt = aSrcRelObject->mCheckNotNullCnt;

    QLNC_COPY_DICT_HANDLE_ARRAY( aSrcRelObject->mCheckNotNullHandle,
                                 sDestRelObject->mCheckNotNullHandle,
                                 aSrcRelObject->mCheckNotNullCnt );

    /**
     * Check Clause
     */

    sDestRelObject->mCheckClauseCnt = aSrcRelObject->mCheckClauseCnt;

    QLNC_COPY_DICT_HANDLE_ARRAY( aSrcRelObject->mCheckClauseHandle,
                                 sDestRelObject->mCheckClauseHandle,
                                 aSrcRelObject->mCheckClauseCnt );

    /**
     * Identity Column
     */

    sDestRelObject->mHasIdentity = aSrcRelObject->mHasIdentity;

    stlMemcpy( & sDestRelObject->mIdentityColumnHandle,
               & aSrcRelObject->mIdentityColumnHandle,
               STL_SIZEOF(ellDictHandle) );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Insert Node의 Candidate Plan을 구한다.
 * @param[in]       aTransID                Transaction ID
 * @param[in]       aDBCStmt                DBC Statement
 * @param[in]       aSQLStmt                SQL Statement 객체
 * @param[in]       aQueryExprList          Query Expression List
 * @param[in]       aTableStatList          Table Stat List
 * @param[in,out]   aInternalBindIdx        Internal Bind Idx
 * @param[in,out]   aInsertStmtParamInfo    Insert Statement Parameter Info
 * @param[in]       aEnv                    Environment
 * @remarks
 */
stlStatus qlncInsertOptimizerInternal( smlTransId                 aTransID,
                                       qllDBCStmt               * aDBCStmt,
                                       qllStatement             * aSQLStmt,
                                       qloInitExprList          * aQueryExprList,
                                       qloValidTblStatList      * aTableStatList,
                                       stlInt32                 * aInternalBindIdx,
                                       qlncInsertStmtParamInfo  * aInsertStmtParamInfo,
                                       qllEnv                   * aEnv )
{
    stlInt32                  i;
    stlInt32                  j;
    stlInt32                  sGlobalID;
    qlncParamInfo             sParamInfo;
    qlncCreateNodeParamInfo   sCreateNodeParamInfo;

    qlncQBMapArray          * sQBMapArray           = NULL;
    qlncTableMapArray       * sTableMapArray        = NULL;
    qlncNodeCommon          * sCandWriteTableNode   = NULL;

    qlncRefExprList           sSubQueryExprList;
    qlncRefExprList           sRowSubQueryExprList;
    qlncRefExprList           sReturnSubQueryExprList;
    qlncRefExprList           sWriteColumnList;
    qlncExprCommon         ** sRowDefaultExprArr    = NULL;
    qlncRefExprList           sInsertColExprList;
    qlncExprCommon        *** sRowValueArray        = NULL;
    qlncRefExprItem         * sRefCandExprItem      = NULL;
    qlvRefExprItem          * sRefInitExprItem      = NULL;
    qlvInitSingleRow        * sSingleRow            = NULL;
    qlncTarget              * sReturnTargetArr      = NULL;

    qlncPlanInsertStmtParamInfo   sPlanInsertStmtParamInfo;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aSQLStmt->mStmtType == QLL_STMT_INSERT_TYPE ) ||
                        ( aSQLStmt->mStmtType == QLL_STMT_INSERT_SELECT_TYPE ) ||
                        ( aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_QUERY_TYPE ) ||
                        ( aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_INTO_TYPE ),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInsertStmtParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInternalBindIdx != NULL, QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Default Trace Information 시작
     ****************************************************************/

    if( QLL_IS_ENABLE_TRACE_LOGGER(aEnv) )
    {
        (void)qllTraceHead( QLL_TRACE_OPTIMIZER_DUMP_TITLE, aEnv );
        (void)qllTraceString( "--- Current SQL Query String ---\n", aEnv );

        /* Input Query String 출력 */
        STL_TRY( qllTraceBody( &QLL_CANDIDATE_MEM( aEnv ),
                               aEnv,
                               "%s\n",
                               aSQLStmt->mRetrySQL )
                 == STL_SUCCESS );
    }


    /****************************************************************
     * Parameter Information 설정
     ****************************************************************/

    sGlobalID = 0;
    sParamInfo.mTransID = aTransID;
    sParamInfo.mDBCStmt = aDBCStmt;
    sParamInfo.mSQLStmt = aSQLStmt;
    sParamInfo.mGlobalNodeID = &sGlobalID;
    sParamInfo.mGlobalInternalBindIdx = aInternalBindIdx;


    /****************************************************************
     * Prepare Candidate Nodes for Finding the Best Candidate Plan
     ****************************************************************/

    if( QLL_IS_ENABLE_TRACE_LOGGER(aEnv) )
    {
        (void)qllTraceMidTitle( "[STEP 01] Created Node for Optimizer", aEnv );
    }

    /* Query Block Array 생성 */
    STL_TRY( qlncCreateQBMapArray( &sParamInfo,
                                   &sQBMapArray,
                                   aEnv )
             == STL_SUCCESS );

    /* Table Map Array 생성 */
    STL_TRY( qlncCreateTableMapArray( &sParamInfo,
                                      &sTableMapArray,
                                      aEnv )
             == STL_SUCCESS );


    /**
     * Write Table Node에 대한 Candidate Node 생성
     */

    /* Create Node Parameter Information 설정 */
    QLNC_INIT_CREATE_NODE_PARAM_INFO( &sCreateNodeParamInfo,
                                      &sParamInfo,
                                      aInsertStmtParamInfo->mWriteTableNode,
                                      NULL,
                                      sTableMapArray,
                                      NULL,
                                      NULL,
                                      sQBMapArray );

    /* Write Table Node에 대한 Candidate Node 생성 */
    STL_TRY( qlncCreateTableCandNode( &sCreateNodeParamInfo,
                                      aEnv )
             == STL_SUCCESS );

    STL_DASSERT( sCreateNodeParamInfo.mCandNode != NULL );
    sCandWriteTableNode = sCreateNodeParamInfo.mCandNode;


    /**
     * Write Column List
     */

    QLNC_INIT_LIST( &sWriteColumnList );
    if( ((qlvInitBaseTableNode*)(aInsertStmtParamInfo->mWriteTableNode))->mRefColumnList != NULL )
    {
        sRefInitExprItem =
            ((qlvInitBaseTableNode*)(aInsertStmtParamInfo->mWriteTableNode))->mRefColumnList->mHead;
        while( sRefInitExprItem != NULL )
        {
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncRefExprItem ),
                                        (void**) &sRefCandExprItem,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            QLNC_INIT_LIST( &sSubQueryExprList );
            STL_TRY( qlncConvertExpression( &sCreateNodeParamInfo,
                                            sRefInitExprItem->mExprPtr,
                                            QLNC_EXPR_PHRASE_TARGET,
                                            &sSubQueryExprList,
                                            &(sRefCandExprItem->mExpr),
                                            aEnv )
                     == STL_SUCCESS );

            sRefCandExprItem->mNext = NULL;

            QLNC_APPEND_ITEM_TO_LIST( &sWriteColumnList, sRefCandExprItem );

            sRefInitExprItem = sRefInitExprItem->mNext;
        }
    }

    if( ((qlvInitBaseTableNode*)(aInsertStmtParamInfo->mWriteTableNode))->mRefRowIdColumn != NULL )
    {
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncRefExprItem ),
                                    (void**) &sRefCandExprItem,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        QLNC_INIT_LIST( &sSubQueryExprList );
        STL_TRY( qlncConvertExpression( &sCreateNodeParamInfo,
                                        ((qlvInitBaseTableNode*)(aInsertStmtParamInfo->mWriteTableNode))->mRefRowIdColumn,
                                        QLNC_EXPR_PHRASE_TARGET,
                                        &sSubQueryExprList,
                                        &(sRefCandExprItem->mExpr),
                                        aEnv )
                 == STL_SUCCESS );

        sRefCandExprItem->mNext = NULL;

        QLNC_APPEND_ITEM_TO_LIST( &sWriteColumnList, sRefCandExprItem );
    }


    /**
     * Insert Column Expression List (Write Table Node)
     */

    QLNC_INIT_LIST( &sInsertColExprList );
    STL_DASSERT( aInsertStmtParamInfo->mInsertColExprList != NULL );
    sRefInitExprItem = aInsertStmtParamInfo->mInsertColExprList->mHead;
    while( sRefInitExprItem != NULL )
    {
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncRefExprItem ),
                                    (void**) &sRefCandExprItem,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        QLNC_INIT_LIST( &sSubQueryExprList );
        STL_TRY( qlncConvertExpression( &sCreateNodeParamInfo,
                                        sRefInitExprItem->mExprPtr,
                                        QLNC_EXPR_PHRASE_TARGET,
                                        &sSubQueryExprList,
                                        &(sRefCandExprItem->mExpr),
                                        aEnv )
                 == STL_SUCCESS );

        sRefCandExprItem->mNext = NULL;

        QLNC_APPEND_ITEM_TO_LIST( &sInsertColExprList, sRefCandExprItem );

        sRefInitExprItem = sRefInitExprItem->mNext;
    }


    /**
     * Insert Target (이미 Multi Row Array에 등록되어 있으므로 할 필요 없음)
     */

    STL_DASSERT( aInsertStmtParamInfo->mInsertTarget != NULL );
    if( aInsertStmtParamInfo->mRowList != NULL )
    {
        /* Insert Values인 경우 Row Default Expr이 이미 Value에 설정되어 있다. */
        sRowDefaultExprArr = NULL;
    }
    else
    {
        /* Insert Select인 경우 Optimizer가 끝난 다음 
         * Row Default Expr을 참조하여 설정하므로 
         * 여기서 값을 셋팅해야 한다. */
        if( aInsertStmtParamInfo->mInsertTarget->mRowDefaultExpr != NULL )
        {
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncExprCommon* ) * aInsertStmtParamInfo->mTotalColumnCount,
                                        (void**) &sRowDefaultExprArr,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            for( i = 0; i < aInsertStmtParamInfo->mTotalColumnCount; i++ )
            {
                if( (aInsertStmtParamInfo->mInsertTarget->mIsRowDefault[i] == STL_TRUE) &&
                    (aInsertStmtParamInfo->mInsertTarget->mRowDefaultExpr[i] != NULL) )
                {
                    QLNC_INIT_LIST( &sSubQueryExprList );
                    STL_TRY( qlncConvertExpression( &sCreateNodeParamInfo,
                                                    aInsertStmtParamInfo->mInsertTarget->mRowDefaultExpr[i],
                                                    QLNC_EXPR_PHRASE_TARGET,
                                                    &sSubQueryExprList,
                                                    &(sRowDefaultExprArr[i]),
                                                    aEnv )
                             == STL_SUCCESS );

                    STL_DASSERT( sSubQueryExprList.mCount == 0 );
                }
                else
                {
                    sRowDefaultExprArr[i] = NULL;
                }
            }
        }
        else
        {
            /* @todo Validation에서 row default expr을 무조건 할당해 놓는 것으로 판단됨 */

            sRowDefaultExprArr = NULL;
        }
    }


    /**
     * Insert Column Value Expression Array (Read Table Node)
     */

    QLNC_INIT_LIST( &sRowSubQueryExprList );
    if( aInsertStmtParamInfo->mRowList != NULL )
    {
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncExprCommon** ) * aInsertStmtParamInfo->mMultiValueCount,
                                    (void**) &sRowValueArray,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sSingleRow = aInsertStmtParamInfo->mRowList;
        for( i = 0; i < aInsertStmtParamInfo->mMultiValueCount; i++ )
        {
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncExprCommon* ) * aInsertStmtParamInfo->mTotalColumnCount,
                                        (void**) &(sRowValueArray[i]),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            for( j = 0; j < aInsertStmtParamInfo->mTotalColumnCount; j++ )
            {
                QLNC_INIT_LIST( &sSubQueryExprList );
                STL_TRY( qlncConvertExpression( &sCreateNodeParamInfo,
                                                sSingleRow->mValueExpr[j],
                                                QLNC_EXPR_PHRASE_TARGET,
                                                &sSubQueryExprList,
                                                &(sRowValueArray[i][j]),
                                                aEnv )
                         == STL_SUCCESS );

                if( sSubQueryExprList.mCount > 0 )
                {
                    if( sRowSubQueryExprList.mCount == 0 )
                    {
                        QLNC_COPY_LIST_INFO( &sSubQueryExprList, &sRowSubQueryExprList );
                    }
                    else
                    {
                        sRowSubQueryExprList.mTail->mNext = sSubQueryExprList.mHead;
                        sRowSubQueryExprList.mTail = sSubQueryExprList.mTail;
                        sRowSubQueryExprList.mCount += sSubQueryExprList.mCount;
                    }
                }
            }

            sSingleRow = sSingleRow->mNext;
        }
    }
    else
    {
        sRowValueArray = NULL;
    }


    /**
     * Return Target절
     */

    QLNC_INIT_LIST( &sReturnSubQueryExprList );
    if( aInsertStmtParamInfo->mReturnTargetCount > 0 )
    {
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncTarget ) * aInsertStmtParamInfo->mReturnTargetCount,
                                    (void**) &sReturnTargetArr,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        for( i = 0; i < aInsertStmtParamInfo->mReturnTargetCount; i++ )
        {
            sReturnTargetArr[i].mDisplayPos = aInsertStmtParamInfo->mReturnTargetArray[i].mDisplayPos;

            QLNC_ALLOC_AND_COPY_STRING( sReturnTargetArr[i].mDisplayName,
                                        aInsertStmtParamInfo->mReturnTargetArray[i].mDisplayName,
                                        &QLL_CANDIDATE_MEM( aEnv ),
                                        aEnv );

            QLNC_ALLOC_AND_COPY_STRING( sReturnTargetArr[i].mAliasName,
                                        aInsertStmtParamInfo->mReturnTargetArray[i].mAliasName,
                                        &QLL_CANDIDATE_MEM( aEnv ),
                                        aEnv );

            QLNC_INIT_LIST( &sSubQueryExprList );

            STL_TRY( qlncConvertExpression( &sCreateNodeParamInfo,
                                            aInsertStmtParamInfo->mReturnTargetArray[i].mExpr,
                                            QLNC_EXPR_PHRASE_TARGET,
                                            &sSubQueryExprList,
                                            &(sReturnTargetArr[i].mExpr),
                                            aEnv )
                     == STL_SUCCESS );

            sReturnTargetArr[i].mDataTypeInfo = aInsertStmtParamInfo->mReturnTargetArray[i].mDataTypeInfo;

            if( sSubQueryExprList.mCount > 0 )
            {
                if( sReturnSubQueryExprList.mCount == 0 )
                {
                    QLNC_COPY_LIST_INFO( &sSubQueryExprList, &sReturnSubQueryExprList );
                }
                else
                {
                    sReturnSubQueryExprList.mTail->mNext = sSubQueryExprList.mHead;
                    sReturnSubQueryExprList.mTail = sSubQueryExprList.mTail;
                    sReturnSubQueryExprList.mCount += sSubQueryExprList.mCount;
                }
            }
        }
    }
    else
    {
        sReturnTargetArr = NULL;
    }


    /**
     * Optimizer 수행
     */

    /* SubQuery에 대한 Optimizer 수행 */
    sRefCandExprItem = sRowSubQueryExprList.mHead;
    while( sRefCandExprItem != NULL )
    {
        STL_DASSERT( sRefCandExprItem->mExpr->mType == QLNC_EXPR_TYPE_SUB_QUERY );

        STL_TRY( qlncQueryNodeOptimizer( &sParamInfo,
                                         ((qlncExprSubQuery*)(sRefCandExprItem->mExpr))->mNode,
                                         aEnv )
                 == STL_SUCCESS );

        sRefCandExprItem = sRefCandExprItem->mNext;
    }

    sRefCandExprItem = sReturnSubQueryExprList.mHead;
    while( sRefCandExprItem != NULL )
    {
        STL_DASSERT( sRefCandExprItem->mExpr->mType == QLNC_EXPR_TYPE_SUB_QUERY );

        STL_TRY( qlncQueryNodeOptimizer( &sParamInfo,
                                         ((qlncExprSubQuery*)(sRefCandExprItem->mExpr))->mNode,
                                         aEnv )
                 == STL_SUCCESS );

        sRefCandExprItem = sRefCandExprItem->mNext;
    }

    /* Write Table Node의 Cost Based Optimizer 수행 */
    STL_TRY( qlncDMLNodeOptimizer( &sParamInfo,
                                   sCandWriteTableNode,
                                   &sCandWriteTableNode,
                                   aEnv )
             == STL_SUCCESS );


    /****************************************************************
     * Default Trace Information 종료
     ****************************************************************/

    if( QLL_IS_ENABLE_TRACE_LOGGER(aEnv) )
    {
        (void)qllTraceTail( QLL_TRACE_OPTIMIZER_DUMP_TITLE, aEnv );
    }


    /****************************************************************
     * Make Candidate Plan
     ****************************************************************/

    sPlanInsertStmtParamInfo.mWriteTableNode = sCandWriteTableNode;
    sPlanInsertStmtParamInfo.mWriteColumnList = &sWriteColumnList;
    sPlanInsertStmtParamInfo.mMultiValueCount = aInsertStmtParamInfo->mMultiValueCount;
    sPlanInsertStmtParamInfo.mRowDefaultExprArr = sRowDefaultExprArr;
    sPlanInsertStmtParamInfo.mInsertColumnCount = aInsertStmtParamInfo->mTotalColumnCount;
    sPlanInsertStmtParamInfo.mInsertColExprList = &sInsertColExprList;
    sPlanInsertStmtParamInfo.mRowValueArray = sRowValueArray;
    sPlanInsertStmtParamInfo.mReturnTargetCount = aInsertStmtParamInfo->mReturnTargetCount;
    sPlanInsertStmtParamInfo.mReturnTargetArray = sReturnTargetArr;
    sPlanInsertStmtParamInfo.mRowSubQueryExprList = &sRowSubQueryExprList;
    sPlanInsertStmtParamInfo.mReturnSubQueryExprList = &sReturnSubQueryExprList;
    sPlanInsertStmtParamInfo.mOutReturnTargetArray = NULL;
    sPlanInsertStmtParamInfo.mOutReturnOuterColumnList = NULL;
    sPlanInsertStmtParamInfo.mOutRowDefaultExprArr = NULL;
    sPlanInsertStmtParamInfo.mOutRowList = NULL;
    sPlanInsertStmtParamInfo.mOutInsertColExprList = NULL;
    sPlanInsertStmtParamInfo.mOutPlanWriteTable = NULL;
    sPlanInsertStmtParamInfo.mOutPlanRowSubQueryList = NULL;
    sPlanInsertStmtParamInfo.mOutPlanReturnSubQueryList = NULL;

    STL_TRY( qlncMakeCandPlanForInsert( aTransID,
                                        aDBCStmt,
                                        aSQLStmt,
                                        aQueryExprList,
                                        aTableStatList,
                                        &sPlanInsertStmtParamInfo,
                                        aEnv )
             == STL_SUCCESS );


    /****************************************************************
     * Output 설정
     ****************************************************************/

    aInsertStmtParamInfo->mOutReturnTargetArray = sPlanInsertStmtParamInfo.mOutReturnTargetArray;
    aInsertStmtParamInfo->mOutReturnOuterColumnList = sPlanInsertStmtParamInfo.mOutReturnOuterColumnList;
    aInsertStmtParamInfo->mOutRowList = sPlanInsertStmtParamInfo.mOutRowList;
    aInsertStmtParamInfo->mOutInsertColExprList = sPlanInsertStmtParamInfo.mOutInsertColExprList;
    aInsertStmtParamInfo->mOutPlanWriteTable = sPlanInsertStmtParamInfo.mOutPlanWriteTable;
    aInsertStmtParamInfo->mOutPlanRowSubQueryList = sPlanInsertStmtParamInfo.mOutPlanRowSubQueryList;
    aInsertStmtParamInfo->mOutPlanReturnSubQueryList = sPlanInsertStmtParamInfo.mOutPlanReturnSubQueryList;

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlvInitInsertTarget ),
                                (void**) &(aInsertStmtParamInfo->mOutInsertTarget),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( stlInt32 ) * aInsertStmtParamInfo->mTotalColumnCount,
                                (void**) &(aInsertStmtParamInfo->mOutInsertTarget->mTargetColumnIdx),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemcpy( aInsertStmtParamInfo->mOutInsertTarget->mTargetColumnIdx,
               aInsertStmtParamInfo->mInsertTarget->mTargetColumnIdx,
               STL_SIZEOF( stlInt32 ) * aInsertStmtParamInfo->mTotalColumnCount );

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( stlBool ) * aInsertStmtParamInfo->mTotalColumnCount,
                                (void**) &(aInsertStmtParamInfo->mOutInsertTarget->mIsDeclareColumn),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemcpy( aInsertStmtParamInfo->mOutInsertTarget->mIsDeclareColumn,
               aInsertStmtParamInfo->mInsertTarget->mIsDeclareColumn,
               STL_SIZEOF( stlBool ) * aInsertStmtParamInfo->mTotalColumnCount );

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( stlBool ) * aInsertStmtParamInfo->mTotalColumnCount,
                                (void**) &(aInsertStmtParamInfo->mOutInsertTarget->mIsRowDefault),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemcpy( aInsertStmtParamInfo->mOutInsertTarget->mIsRowDefault,
               aInsertStmtParamInfo->mInsertTarget->mIsRowDefault,
               STL_SIZEOF( stlBool ) * aInsertStmtParamInfo->mTotalColumnCount );

    aInsertStmtParamInfo->mOutInsertTarget->mRowDefaultExpr = sPlanInsertStmtParamInfo.mOutRowDefaultExprArr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlnc */
