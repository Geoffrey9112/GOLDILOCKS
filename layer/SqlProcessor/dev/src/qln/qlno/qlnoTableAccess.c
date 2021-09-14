/*******************************************************************************
 * qlnoTableAccess.c
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
 * @file qlnoTableAccess.c
 * @brief SQL Processor Layer Code Optimization Node - TABLE ACCESS
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlno
 * @{
 */


/**
 * @brief Table Access에 대한 Code Optimize를 수행한다.
 * @param[in]   aCodeOptParamInfo   Code Optimize Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlnoCodeOptimizeTableAccess( qloCodeOptParamInfo  * aCodeOptParamInfo,
                                       qllEnv               * aEnv )
{
    qlncPlanTableScan   * sPlanTableScan        = NULL;
    qlnoTableAccess     * sOptTableAccess       = NULL;
    qllEstimateCost     * sOptCost              = NULL;
    qllOptimizationNode * sOptNode              = NULL;

    qlvRefExprList      * sReadColumnList       = NULL;
    qlvInitExpression   * sRowIdColumn          = NULL;

    stlInt32              sAggrFuncCnt          = 0;
    stlInt32              sAggrDistFuncCnt      = 0;
    qlvRefExprList      * sTotalAggrFuncList    = NULL;
    qlvRefExprList      * sAggrFuncList         = NULL;
    qlvRefExprList      * sAggrDistFuncList     = NULL;

    /* qlncPlanCommon      * sCandQueryPlan        = NULL; */
    /* qlncRefExprItem     * sExprItem             = NULL; */


    /*
     * Paramter Validation
     */

    QLO_CODE_OPT_DEFAULT_PARAM_VALIDATE( aCodeOptParamInfo, aEnv );
    STL_PARAM_VALIDATE( aCodeOptParamInfo->mCandPlan->mPlanType == QLNC_PLAN_TYPE_TABLE_SCAN,
                        QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Table Scan Plan 정보 획득
     ****************************************************************/

    sPlanTableScan = (qlncPlanTableScan*)(aCodeOptParamInfo->mCandPlan);


    /****************************************************************
     * Optimization Node 생성
     ****************************************************************/

    /**
     * TABLE ACCESS Optimization Node 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                STL_SIZEOF( qlnoTableAccess ),
                                (void **) & sOptTableAccess,
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
                                QLL_PLAN_NODE_TABLE_ACCESS_TYPE,
                                sOptTableAccess,
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

    sReadColumnList = sPlanTableScan->mReadColList;


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
    
    sRowIdColumn = sPlanTableScan->mRowidColumn;

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
     * Distinct 조건을 포함 여부로 Aggregation 구분
     ****************************************************************/

    if( sPlanTableScan->mCommon.mAggrExprList.mCount > 0 )
    {
        STL_TRY( qlnfClassifyAggrFunc( &(sPlanTableScan->mCommon.mAggrExprList),
                                       & sAggrFuncList,
                                       & sAggrDistFuncList,
                                       QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                       aEnv )
                 == STL_SUCCESS );

        STL_DASSERT( sAggrFuncList != NULL );
        STL_DASSERT( sAggrDistFuncList != NULL );

        sAggrFuncCnt = sAggrFuncList->mCount;
        sAggrDistFuncCnt = sAggrDistFuncList->mCount;

        if( sPlanTableScan->mCommon.mAggrExprList.mCount > 0 )
        {
            STL_TRY( qlvCreateRefExprList( &sTotalAggrFuncList,
                                           QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                           aEnv )
                     == STL_SUCCESS );

            sTotalAggrFuncList->mCount = sPlanTableScan->mCommon.mAggrExprList.mCount;
            sTotalAggrFuncList->mHead = sPlanTableScan->mCommon.mAggrExprList.mHead;
            sTotalAggrFuncList->mTail = sPlanTableScan->mCommon.mAggrExprList.mTail;
        }
        else
        {
            sTotalAggrFuncList = NULL;
        }
    }
    else
    {
        /* Do Nothing */
    }


    /****************************************************************
     * TABLE ACCESS Optimization Node 정보 설정
     ****************************************************************/

    
    /**
     * Relation Node
     */

    sOptTableAccess->mRelationNode = sPlanTableScan->mCommon.mInitNode;


    /**
     * Row Block이 필요한지 여부
     */

    sOptTableAccess->mNeedRowBlock = sPlanTableScan->mCommon.mNeedRowBlock;


    /**
     * Table Handle
     */

    STL_TRY( qlnoAllocAndCopyString( ellGetTableName( sPlanTableScan->mTableHandle ),
                                     & sOptTableAccess->mTableName,
                                     QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                     aEnv )
             == STL_SUCCESS );
    
    sOptTableAccess->mTableHandle = sPlanTableScan->mTableHandle;


    /**
     * Dump Option
     */

    sOptTableAccess->mDumpOption = sPlanTableScan->mDumpOption;

    
    /**
     * Read Column List
     */

    sOptTableAccess->mReadColList = sReadColumnList;

    
    /**
     * Read RowId Column
     */

    sOptTableAccess->mRowIdColumn = sRowIdColumn;


    /**
     * Outer Column List
     */

    sOptTableAccess->mOuterColumnList = sPlanTableScan->mOuterColumnList;


    /**
     * Total Filter Expression
     */

    sOptTableAccess->mTotalFilterExpr = sPlanTableScan->mFilterExpr;


    /**
     * Aggregation 관련
     */

    sOptTableAccess->mAggrFuncCnt       = sAggrFuncCnt;
    sOptTableAccess->mAggrFuncList      = sAggrFuncList;

    sOptTableAccess->mTotalAggrFuncList = sTotalAggrFuncList;
    sOptTableAccess->mAggrDistFuncCnt   = sAggrDistFuncCnt;
    sOptTableAccess->mAggrDistFuncList  = sAggrDistFuncList;
    

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
 * @brief TABLE ACCESS의 Optimization Node 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoCompleteTableAccess( qllDBCStmt            * aDBCStmt,
                                   qllStatement          * aSQLStmt,
                                   qllOptNodeList        * aOptNodeList,
                                   qllOptimizationNode   * aOptNode,
                                   qloInitExprList       * aQueryExprList,
                                   qllEnv                * aEnv )
{
    qlnoTableAccess    * sOptTableAccess      = NULL;

    qlvInitExpression  * sPhysicalFilterExpr  = NULL;
    qlvInitExpression  * sLogicalFilterExpr   = NULL;
    
    knlExprStack       * sLogicalFilterStack  = NULL;
    knlPredicateList   * sLogicalFilterPred   = NULL;
    
    qlvInitExpression  * sCodeExpr            = NULL;
    knlExprEntry       * sExprEntry           = NULL;

    knlExprStack       * sTempConstExprStack  = NULL;
    qlvRefExprItem     * sExprItem            = NULL;
    ellDictHandle      * sColumnHandle        = NULL;
    
    
    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_TABLE_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );    
    

    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/

    sOptTableAccess = (qlnoTableAccess *) aOptNode->mOptNode;
    

    /****************************************************************
     * Read Column에 대한 DB Type 설정
     ****************************************************************/

    if( sOptTableAccess->mReadColList != NULL )
    {
        sExprItem = sOptTableAccess->mReadColList->mHead;

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

    if( sOptTableAccess->mRowIdColumn != NULL )
    {
        /* ROWID COLUMN 에 대한 DBType 설정 */
        STL_TRY( qloSetExprDBType( sOptTableAccess->mRowIdColumn,
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
     * Physical & Logical Filter 구분
     ****************************************************************/
    
    /**
     * Logical Filter 와 Physical Filter 분리
     */

    if( sOptTableAccess->mTotalFilterExpr == NULL )
    {
        sLogicalFilterExpr  = NULL;
        sPhysicalFilterExpr = NULL;
        sLogicalFilterPred  = NULL;

        sOptTableAccess->mPrepareFilterStack = NULL;
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
                              sOptTableAccess->mTotalFilterExpr->mIncludeExprCnt,
                              & QLL_CANDIDATE_MEM( aEnv ),
                              KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
    
        /*******************************************
         * Physical & Logical Filter 구분
         *******************************************/
        
        /**
         * Logical Filter 와 Physical Filter Expression 분리
         */

        if( (ellGetTableType( sOptTableAccess->mTableHandle ) == ELL_TABLE_TYPE_BASE_TABLE) &&
            (ellIsBuiltInTable( sOptTableAccess->mTableHandle ) == STL_FALSE ))
        {
            STL_TRY( qloClassifyFilterExpr( aSQLStmt->mRetrySQL,
                                            sOptTableAccess->mTotalFilterExpr,
                                            sOptTableAccess->mRelationNode,
                                            QLL_CODE_PLAN( aDBCStmt ),
                                            aSQLStmt->mBindContext,
                                            & sLogicalFilterExpr,
                                            & sPhysicalFilterExpr,
                                            aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            sLogicalFilterExpr  = sOptTableAccess->mTotalFilterExpr;
            sPhysicalFilterExpr = NULL;
        }


        /*************************
         * Table Physical Filter 
         *************************/

        if( sPhysicalFilterExpr == NULL )
        {
            sOptTableAccess->mPrepareFilterStack = NULL;
        }
        else
        {
            STL_TRY( qloMakePhysicalFilterStack( aDBCStmt,
                                                 aSQLStmt,
                                                 aQueryExprList,
                                                 sPhysicalFilterExpr,
                                                 & sOptTableAccess->mPrepareFilterStack,
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
     * Aggregation 에 대한 Expression Stack 구성
     ****************************************************************/

    /**
     * Aggregation Function 정보 구축
     */

    if( sOptTableAccess->mAggrFuncCnt > 0 )
    {
        STL_TRY( qlnfGetAggrOptInfo( aSQLStmt,
                                     aQueryExprList,
                                     sOptTableAccess->mAggrFuncCnt,
                                     sOptTableAccess->mAggrFuncList,
                                     & sOptTableAccess->mAggrOptInfo,
                                     QLL_CODE_PLAN( aDBCStmt ),
                                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sOptTableAccess->mAggrOptInfo = NULL;
    }


    /****************************************************************
     * Aggregation Distinct에 대한 Expression Stack 구성
     ****************************************************************/

    /**
     * Aggregation Function 정보 구축
     */

    if( sOptTableAccess->mAggrDistFuncCnt > 0 )
    {
        STL_TRY( qlnfGetAggrOptInfo( aSQLStmt,
                                     aQueryExprList,
                                     sOptTableAccess->mAggrDistFuncCnt,
                                     sOptTableAccess->mAggrDistFuncList,
                                     & sOptTableAccess->mAggrDistOptInfo,
                                     QLL_CODE_PLAN( aDBCStmt ),
                                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sOptTableAccess->mAggrDistOptInfo = NULL;
    }


    /****************************************************************
     * Node 정보 설정
     ****************************************************************/

    /**
     * Physical Filter Expression
     */
    
    sOptTableAccess->mPhysicalFilterExpr = sPhysicalFilterExpr;

    
    /**
     * Logical Filter Expression
     */
    
    sOptTableAccess->mLogicalFilterExpr = sLogicalFilterExpr;


    /**
     * Logical Filter Predicate
     */
    
    sOptTableAccess->mLogicalFilterPred = sLogicalFilterPred;


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
 * @brief TABLE ACCESS의 All Expr 리스트 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoAddExprTableAccess( qllDBCStmt            * aDBCStmt,
                                  qllStatement          * aSQLStmt,
                                  qllOptNodeList        * aOptNodeList,
                                  qllOptimizationNode   * aOptNode,
                                  qloInitExprList       * aQueryExprList,
                                  qllEnv                * aEnv )
{
    qlnoTableAccess   * sOptTableAccess = NULL;
    qlvRefExprItem    * sRefExprItem    = NULL;
    

    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_TABLE_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );    
    

    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/

    sOptTableAccess = (qlnoTableAccess *) aOptNode->mOptNode;
    

    /****************************************************************
     * Add Expr
     ****************************************************************/

    /**
     * Add Expr Table Access Read Column
     */

    if( sOptTableAccess->mReadColList != NULL )
    {
        sRefExprItem = sOptTableAccess->mReadColList->mHead;
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


    /**
     * Add Expr Table Access Read RowId Column
     */

    if( sOptTableAccess->mRowIdColumn != NULL )
    {
        STL_TRY( qloAddExpr( sOptTableAccess->mRowIdColumn,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN(aDBCStmt),
                             aEnv )
                 == STL_SUCCESS );
    }

    
    /**
     * Add Expr Where Clause Expression
     */

    if( sOptTableAccess->mTotalFilterExpr != NULL )
    {
        STL_TRY( qloAddExpr( sOptTableAccess->mTotalFilterExpr,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN( aDBCStmt ),
                             aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Add Expr Aggregation Functions
     */

    if( sOptTableAccess->mAggrFuncCnt > 0 )
    {
        sRefExprItem = sOptTableAccess->mAggrFuncList->mHead;

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

    
    /**
     * Add Expr Aggregation Distinct Functions
     */

    if( sOptTableAccess->mAggrDistFuncCnt > 0 )
    {
        sRefExprItem = sOptTableAccess->mAggrDistFuncList->mHead;

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
