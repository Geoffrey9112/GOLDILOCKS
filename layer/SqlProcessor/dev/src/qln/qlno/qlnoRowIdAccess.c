/*******************************************************************************
 * qlnoRowIdAccess.c
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
 * @file qlnoRowIdAccess.c
 * @brief SQL Processor Layer Code Optimization Node - ROWID ACCESS
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlno
 * @{
 */


/**
 * @brief Rowid Access에 대한 Code Optimize를 수행한다.
 * @param[in]   aCodeOptParamInfo   Code Optimize Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlnoCodeOptimizeRowidAccess( qloCodeOptParamInfo  * aCodeOptParamInfo,
                                       qllEnv               * aEnv )
{
    qlncPlanRowidScan   * sPlanRowidScan        = NULL;
    qlnoRowIdAccess     * sOptRowIdAccess       = NULL;
    qllEstimateCost     * sOptCost              = NULL;
    qllOptimizationNode * sOptNode              = NULL;
    
    qlvRefExprList      * sReadColumnList       = NULL;



    qlvInitExpression     * sWhereExpr          = NULL;
    qlvInitExpression     * sRowIdColumn        = NULL;

    qlvInitExpression     * sRowIdScanExpr      = NULL;
    qlvInitExpression     * sFilterExpr         = NULL;
  
    stlInt32                sAggrFuncCnt        = 0;
    qlvRefExprList        * sTotalAggrFuncList  = NULL;
    qlvRefExprList        * sAggrFuncList       = NULL;

    /*
     * Paramter Validation
     */

    QLO_CODE_OPT_DEFAULT_PARAM_VALIDATE( aCodeOptParamInfo, aEnv );
    STL_PARAM_VALIDATE( aCodeOptParamInfo->mCandPlan->mPlanType == QLNC_PLAN_TYPE_ROWID_SCAN,
                        QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Rowid Scan Plan 정보 획득
     ****************************************************************/

    sPlanRowidScan = (qlncPlanRowidScan*)(aCodeOptParamInfo->mCandPlan);


    /****************************************************************
     * Optimization Node 생성
     ****************************************************************/

    /**
     * ROWID ACCESS Optimization Node 생성
     */
    
    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                STL_SIZEOF( qlnoRowIdAccess ),
                                (void **) & sOptRowIdAccess,
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
                                QLL_PLAN_NODE_ROWID_ACCESS_TYPE,
                                sOptRowIdAccess,
                                aCodeOptParamInfo->mOptQueryNode,  /* Query Node */
                                sOptCost,
                                NULL,
                                & sOptNode,
                                QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                aEnv )
             == STL_SUCCESS );

    
    /****************************************************************
     * Read Column List 구성 : PUSH PROJECTION에 의해 변경 가능
     ****************************************************************/
    
    /**
     * Table의 Read Column List 구성
     */
    
    sReadColumnList = sPlanRowidScan->mReadColList;
    

    /**
     * Statement Expression List에 Read Column 추가
     */

    STL_DASSERT( sReadColumnList != NULL );

    STL_TRY( qlvAddExprListToRefExprList(
                 aCodeOptParamInfo->mQueryExprList->mStmtExprList->mColumnExprList,
                 sReadColumnList,
                 STL_FALSE,
                 QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                 aEnv )
             == STL_SUCCESS );

    
    /**
     * Statement Expression List에 Read RowId Column 추가
     */
    
    sRowIdColumn = sPlanRowidScan->mRowidColumn;

    if( sRowIdColumn != NULL )
    {
        STL_TRY( qlvAddExprToRefExprList(
                     aCodeOptParamInfo->mQueryExprList->mStmtExprList->mRowIdColumnExprList,
                     sRowIdColumn,
                     STL_FALSE,
                     QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /* Do Nothing */
    }


    /****************************************************************
     * ROWID ACCESS 정보 구성
     ****************************************************************/

    /**
     * RowId Scan Expression 을 분리한다.
     * 예) ROWID = 상수  또는  상수 = ROWID
     */

    sWhereExpr = sPlanRowidScan->mFilterExpr;
    STL_DASSERT( sWhereExpr != NULL );

    STL_TRY( qloGetRowIdScanExprAndFilter( sWhereExpr,
                                           QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                           & sRowIdScanExpr,
                                           & sFilterExpr,
                                           aEnv )
             == STL_SUCCESS );
    

    /****************************************************************
     * Distinct 조건을 포함 여부로 Aggregation 구분
     ****************************************************************/

    if( sPlanRowidScan->mCommon.mAggrExprList.mCount > 0 )
    {
        STL_TRY( qlnfSetNonDistAggrFunc( &(sPlanRowidScan->mCommon.mAggrExprList),
                                         & sAggrFuncList,
                                         QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                         aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlvCreateRefExprList( &sTotalAggrFuncList,
                                       QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                       aEnv )
                 == STL_SUCCESS );

        sTotalAggrFuncList->mCount = sPlanRowidScan->mCommon.mAggrExprList.mCount;
        sTotalAggrFuncList->mHead = sPlanRowidScan->mCommon.mAggrExprList.mHead;
        sTotalAggrFuncList->mTail = sPlanRowidScan->mCommon.mAggrExprList.mTail;

        sAggrFuncCnt       = sAggrFuncList->mCount;
    }
    else
    {
        sTotalAggrFuncList = NULL;
        sAggrFuncList      = NULL;
        sAggrFuncCnt       = 0;
    }


    /****************************************************************
     * ROWID ACCESS Optimization Node 정보 설정
     ****************************************************************/
    
    /**
     * Relation Node
     */

    sOptRowIdAccess->mRelationNode = sPlanRowidScan->mCommon.mInitNode;


    /**
     * Row Block이 필요한지 여부
     */

    sOptRowIdAccess->mNeedRowBlock = sPlanRowidScan->mCommon.mNeedRowBlock;

    
    /**
     * Table Handle
     */

    STL_TRY( qlnoAllocAndCopyString( ellGetTableName( sPlanRowidScan->mTableHandle ),
                                     & sOptRowIdAccess->mTableName,
                                     QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                     aEnv )
             == STL_SUCCESS );

    sOptRowIdAccess->mTableHandle = sPlanRowidScan->mTableHandle;


    /**
     * Table Physical Handle
     */

    sOptRowIdAccess->mPhysicalHandle = sPlanRowidScan->mPhysicalHandle;

    
    /**
     * Read Column List
     */

    sOptRowIdAccess->mReadColList = sReadColumnList;

    
    /**
     * Read RowId Column
     */

    sOptRowIdAccess->mRowIdColumn = sRowIdColumn;

    
    /**
     * RowId Scan Expression
     */

    sOptRowIdAccess->mRowIdScanExpr = sRowIdScanExpr;


    /**
     * Outer Column List
     */

    sOptRowIdAccess->mOuterColumnList = sPlanRowidScan->mOuterColumnList;

    
    /**
     * Total Filter Expression
     */

    sOptRowIdAccess->mTotalFilterExpr = sFilterExpr;


    /**
     * Aggregation 관련
     */

    sOptRowIdAccess->mAggrFuncCnt       = sAggrFuncCnt;
    sOptRowIdAccess->mAggrFuncList      = sAggrFuncList;
    sOptRowIdAccess->mTotalAggrFuncList = sTotalAggrFuncList;
    

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
 * @brief  ROWID ACCESS의 Optimization Node 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoCompleteRowIdAccess( qllDBCStmt            * aDBCStmt,
                                   qllStatement          * aSQLStmt,
                                   qllOptNodeList        * aOptNodeList,
                                   qllOptimizationNode   * aOptNode,
                                   qloInitExprList       * aQueryExprList,
                                   qllEnv                * aEnv )
{
    qlnoRowIdAccess    * sOptRowIdAccess     = NULL;

    qlvInitExpression  * sPhysicalFilterExpr = NULL;
    qlvInitExpression  * sLogicalFilterExpr  = NULL;

    knlExprStack       * sLogicalFilterStack  = NULL;

    knlExprStack       * sRowIdScanExprStack = NULL;

    qlvInitExpression  * sCodeExpr           = NULL;
    knlExprEntry       * sExprEntry          = NULL;

    knlExprStack       * sTempConstExprStack = NULL;
    qlvRefExprItem     * sExprItem           = NULL;
    ellDictHandle      * sColumnHandle       = NULL;
    
    
    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_ROWID_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );
    

    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/

    sOptRowIdAccess = (qlnoRowIdAccess *) aOptNode->mOptNode;


    /****************************************************************
     * Read Column에 대한 DB Type 설정
     ****************************************************************/

    if( sOptRowIdAccess->mReadColList != NULL )
    {
        sExprItem = sOptRowIdAccess->mReadColList->mHead;

        while( sExprItem != NULL )
        {
            sColumnHandle = sExprItem->mExprPtr->mExpr.mColumn->mColumnHandle;

            /* COLUMN 에 대한 DBType 설정 */
            STL_TRY( qloSetExprDBType( sExprItem->mExprPtr,
                                       aQueryExprList->mStmtExprList->mTotalExprInfo,
                                       ellGetColumnDBType( sColumnHandle ),
                                       ellGetColumnPrecision( sColumnHandle ),
                                       ellGetColumnScale( sColumnHandle ),
                                       ellGetColumnStringLengthUnit( sColumnHandle ),
                                       ellGetColumnIntervalIndicator( sColumnHandle ),
                                       aEnv )
                     == STL_SUCCESS );

            sExprItem = sExprItem->mNext;
        }
    }

    if( sOptRowIdAccess->mRowIdColumn != NULL )
    {
        /* ROWID COLUMN 에 대한 DBType 설정 */
        STL_TRY( qloSetExprDBType( sOptRowIdAccess->mRowIdColumn,
                                   aQueryExprList->mStmtExprList->mTotalExprInfo,
                                   DTL_TYPE_ROWID,
                                   gDefaultDataTypeDef[ DTL_TYPE_ROWID ].mArgNum1,
                                   gDefaultDataTypeDef[ DTL_TYPE_ROWID ].mArgNum2,
                                   gDefaultDataTypeDef[ DTL_TYPE_ROWID ].mStringLengthUnit,
                                   gDefaultDataTypeDef[ DTL_TYPE_ROWID ].mIntervalIndicator,
                                   aEnv )
                 == STL_SUCCESS );
    }


    /****************************************************************
     * RowId Scan Expr Code Optimize
     ****************************************************************/
    
    /* RowIdScanExpr Code Stack 공간 할당 */

    /**
     * RowIdScanExpr 를 위한 임시 Expression Stack 공간 할당
     * @remark Candidate memory manager 사용
     */

    STL_TRY( knlAllocRegionMem( & QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( knlExprStack ),
                                (void **) & sRowIdScanExprStack,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /* RowIdScanExpr Expression Stack 생성 */

    /**
     * RowIdScanExpr 를 위한 임시 Expression Stack 구성
     * @remark Candidate memory manager 사용
     */

    sCodeExpr = sOptRowIdAccess->mRowIdScanExpr;

    STL_TRY( knlExprInit( sRowIdScanExprStack,
                          sCodeExpr->mIncludeExprCnt,
                          & QLL_CANDIDATE_MEM( aEnv ),
                          KNL_ENV( aEnv ) )
             == STL_SUCCESS );


    /* RowIdScanExpr Expression Stack 구성 */

    STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                        sCodeExpr,
                                        aSQLStmt->mBindContext,
                                        sRowIdScanExprStack,
                                        aQueryExprList->mStmtExprList->mConstExprStack,
                                        aQueryExprList->mStmtExprList->mTotalExprInfo,
                                        & sExprEntry,
                                        & QLL_CANDIDATE_MEM(aEnv),
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

    /****************************************************************
     * Physical & Logical Filter 구분
     ****************************************************************/
    
    /**
     * Logical Filter 와 Physical Filter 분리
     */
    
    if( sOptRowIdAccess->mTotalFilterExpr == NULL )
    {
        sPhysicalFilterExpr = NULL;
        sLogicalFilterExpr  = NULL;
        sLogicalFilterStack = NULL;

        sOptRowIdAccess->mPrepareFilterStack = NULL;
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
                              sOptRowIdAccess->mTotalFilterExpr->mIncludeExprCnt,
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
                                        sOptRowIdAccess->mTotalFilterExpr,
                                        sOptRowIdAccess->mRelationNode,
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
            sOptRowIdAccess->mPrepareFilterStack = NULL;
        }
        else
        {
            STL_TRY( qloMakePhysicalFilterStack( aDBCStmt,
                                                 aSQLStmt,
                                                 aQueryExprList,
                                                 sPhysicalFilterExpr,
                                                 & sOptRowIdAccess->mPrepareFilterStack,
                                                 aEnv )
                     == STL_SUCCESS );
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
             * Logical Filter Predicate 생성
             */

            /**
             * Logical Expression Code Stack 공간 할당
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
        }
    }
    

    /****************************************************************
     * Aggregation 에 대한 Expression Stack 구성
     ****************************************************************/

    /**
     * Aggregation Function 정보 구축
     */

    if( sOptRowIdAccess->mAggrFuncCnt > 0 )
    {
        STL_TRY( qlnfGetAggrOptInfo( aSQLStmt,
                                     aQueryExprList,
                                     sOptRowIdAccess->mAggrFuncCnt,
                                     sOptRowIdAccess->mAggrFuncList,
                                     & sOptRowIdAccess->mAggrOptInfo,
                                     QLL_CODE_PLAN( aDBCStmt ),
                                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sOptRowIdAccess->mAggrOptInfo = NULL;
    }


    /****************************************************************
     * Node 정보 설정
     ****************************************************************/

    /**
     * Physical Filter Expression
     */
    
    sOptRowIdAccess->mPhysicalFilterExpr = sPhysicalFilterExpr;

    /**
     * Logical Filter Expression
     */

    sOptRowIdAccess->mLogicalFilterExpr = sLogicalFilterExpr;
  
    /**
     * Logical Filter Stack
     */

    sOptRowIdAccess->mLogicalStack = sLogicalFilterStack;

    
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
 * @brief ROWID ACCESS의 All Expr 리스트 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoAddExprRowIdAccess( qllDBCStmt            * aDBCStmt,
                                  qllStatement          * aSQLStmt,
                                  qllOptNodeList        * aOptNodeList,
                                  qllOptimizationNode   * aOptNode,
                                  qloInitExprList       * aQueryExprList,
                                  qllEnv                * aEnv )
{
    qlnoRowIdAccess   * sOptRowIdAccess     = NULL;
    qlvRefExprItem    * sRefColumnItem      = NULL;
    qlvRefExprItem    * sRefExprItem        = NULL;


    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_ROWID_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );
    

    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/

    sOptRowIdAccess = (qlnoRowIdAccess *) aOptNode->mOptNode;
    

    /****************************************************************
     * Add Expr
     ****************************************************************/
    
    /**
     * Add Expr RowId Access Read Column
     */

    if( sOptRowIdAccess->mReadColList != NULL )
    {
        sRefColumnItem = sOptRowIdAccess->mReadColList->mHead;
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
     * Add Expr RowId Access Read RowId Column
     */

    if( sOptRowIdAccess->mRowIdColumn != NULL )
    {
        STL_TRY( qloAddExpr( sOptRowIdAccess->mRowIdColumn,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN(aDBCStmt),
                             aEnv )
                 == STL_SUCCESS );
    }

    /**
     * Add Expr RowIdScanExpr
     */

    STL_DASSERT( sOptRowIdAccess->mRowIdScanExpr != NULL );

    STL_TRY( qloAddExpr( sOptRowIdAccess->mRowIdScanExpr,
                         aQueryExprList->mStmtExprList->mAllExprList,
                         QLL_CODE_PLAN( aDBCStmt ),
                         aEnv )
             == STL_SUCCESS );
    
    
    /**
     * Add Expr Where Clause Expression
     */

    if( sOptRowIdAccess->mTotalFilterExpr != NULL )
    {
        STL_TRY( qloAddExpr( sOptRowIdAccess->mTotalFilterExpr,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN( aDBCStmt ),
                             aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Add Expr Aggregation Functions
     */

    if( sOptRowIdAccess->mAggrFuncCnt > 0 )
    {
        sRefExprItem = sOptRowIdAccess->mAggrFuncList->mHead;

        while( sRefExprItem != NULL )
        {
            STL_TRY( qloAddExpr( sRefExprItem->mExprPtr,
                                 aQueryExprList->mStmtExprList->mAllExprList,
                                 QLL_CODE_PLAN( aDBCStmt ),
                                 aEnv )
                     == STL_SUCCESS );

            sRefExprItem = sRefExprItem->mNext;
        }
    }
    else
    {
        /* Do Nothing */
    }


    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

    
/** @} qlno */
