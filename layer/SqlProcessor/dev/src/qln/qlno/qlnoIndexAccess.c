/*******************************************************************************
 * qlnoIndexAccess.c
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
 * @file qlnoIndexAccess.c
 * @brief SQL Processor Layer Optimization Node - INDEX ACCESS
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlno
 * @{
 */


/**
 * @brief Index Access에 대한 Code Optimize를 수행한다.
 * @param[in]   aCodeOptParamInfo   Code Optimize Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlnoCodeOptimizeIndexAccess( qloCodeOptParamInfo  * aCodeOptParamInfo,
                                       qllEnv               * aEnv )
{
    qlncPlanIndexScan       * sPlanIndexScan        = NULL;
    qlnoIndexAccess         * sOptIndexAccess       = NULL;
    qllOptimizationNode     * sOptNode              = NULL;

    qllEstimateCost         * sOptCost              = NULL;

    qlvRefExprList          * sTableReadColumnList  = NULL;
    qlvRefExprList          * sIndexReadColumnList  = NULL;

    ellIndexKeyDesc         * sIndexKeyDesc         = NULL;
    stlInt32                  sKeyColCnt            = 0;
    stlInt32                  sColIdx               = 0;
    stlInt32                  sLoop                 = 0;
    stlBool                   sIsIndexReadCol       = STL_FALSE;    

    qlvInitExpression       * sWhereExpr            = NULL;
    qlvInitExpression       * sRowIdColumn          = NULL;
    qloIndexScanInfo        * sIndexScanInfo        = NULL;
    qlvRefExprItem          * sListItem             = NULL;

    stlInt32                  sAggrFuncCnt          = 0;
    stlInt32                  sAggrDistFuncCnt      = 0;
    qlvRefExprList          * sTotalAggrFuncList    = NULL;
    qlvRefExprList          * sAggrFuncList         = NULL;
    qlvRefExprList          * sAggrDistFuncList     = NULL;


    /*
     * Paramter Validation
     */

    QLO_CODE_OPT_DEFAULT_PARAM_VALIDATE( aCodeOptParamInfo, aEnv );
    STL_PARAM_VALIDATE( aCodeOptParamInfo->mCandPlan->mPlanType == QLNC_PLAN_TYPE_INDEX_SCAN,
                        QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Index Scan Plan 정보 획득
     ****************************************************************/

    sPlanIndexScan = (qlncPlanIndexScan*)(aCodeOptParamInfo->mCandPlan);


    /****************************************************************
     * Optimization Node 생성
     ****************************************************************/

    /**
     * INDEX ACCESS Optimization Node 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                STL_SIZEOF( qlnoIndexAccess ),
                                (void **) & sOptIndexAccess,
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
                                QLL_PLAN_NODE_INDEX_ACCESS_TYPE,
                                sOptIndexAccess,
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
     * Index & Table Read Column List 공간 할당
     */
    
    STL_TRY( qlvCreateRefExprList( & sIndexReadColumnList,
                                   QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( & sTableReadColumnList,
                                   QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    
    /**
     * Index Read Column List 구성
     */

    sKeyColCnt    = ellGetIndexKeyCount( sPlanIndexScan->mIndexHandle );
    sIndexKeyDesc = ellGetIndexKeyDesc( sPlanIndexScan->mIndexHandle );
    if( sPlanIndexScan->mReadColList != NULL )
    {
        sListItem = sPlanIndexScan->mReadColList->mHead;
    
        while( sListItem != NULL )
        {
            STL_DASSERT( sListItem->mExprPtr->mType == QLV_EXPR_TYPE_COLUMN );

            sColIdx = ellGetColumnIdx( sListItem->mExprPtr->mExpr.mColumn->mColumnHandle );

            sIsIndexReadCol = STL_FALSE;

            for( sLoop = 0 ; sLoop < sKeyColCnt ; sLoop++ )
            {
                if( sColIdx == ellGetColumnIdx( & sIndexKeyDesc[ sLoop ].mKeyColumnHandle ) )
                {
                    sIsIndexReadCol = STL_TRUE;
                    break;
                }
            }

            if( sIsIndexReadCol == STL_TRUE )
            {
                /**
                 * Index Read Column List 구성
                 */

                STL_TRY( qlvAddExprToRefExprList( sIndexReadColumnList,
                                                  sListItem->mExprPtr,
                                                  STL_FALSE,
                                                  QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                                  aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                /**
                 * Table Read Column List 구성
                 */

                STL_TRY( qlvAddExprToRefExprList( sTableReadColumnList,
                                                  sListItem->mExprPtr,
                                                  STL_FALSE,
                                                  QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                                  aEnv )
                         == STL_SUCCESS );
            }

            sListItem = sListItem->mNext;
        }


        /**
         * Statement Expression List에 Read Column 추가
         */

        STL_TRY( qlvAddExprListToRefExprList(
                     aCodeOptParamInfo->mQueryExprList->mStmtExprList->mColumnExprList,
                     sPlanIndexScan->mReadColList,
                     STL_FALSE,
                     QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /* Do Nothing */
    }

    
    /**
     * Statement Expression List에 Read RowId Column 추가
     */

    sRowIdColumn = sPlanIndexScan->mRowidColumn;

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
     * Range 정보 구성
     ****************************************************************/

    /**
     * Range 요소별 Expression 분류 
     */

    sWhereExpr = sPlanIndexScan->mFilterExpr;
    STL_TRY( qloGetKeyRange( aCodeOptParamInfo->mSQLStmt->mRetrySQL,
                             sPlanIndexScan->mIndexHandle,
                             sPlanIndexScan->mCommon.mInitNode,
                             aCodeOptParamInfo->mSQLStmt->mBindContext,
                             sWhereExpr,
                             & sIndexScanInfo,
                             QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                             aEnv )
             == STL_SUCCESS );


    /****************************************************************
     * Distinct 조건을 포함 여부로 Aggregation 구분
     ****************************************************************/

    if( sPlanIndexScan->mCommon.mAggrExprList.mCount > 0 )
    {
        STL_TRY( qlnfClassifyAggrFunc( &(sPlanIndexScan->mCommon.mAggrExprList),
                                       & sAggrFuncList,
                                       & sAggrDistFuncList,
                                       QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                       aEnv )
                 == STL_SUCCESS );

        STL_DASSERT( sAggrFuncList != NULL );
        STL_DASSERT( sAggrDistFuncList != NULL );

        sAggrFuncCnt = sAggrFuncList->mCount;
        sAggrDistFuncCnt = sAggrDistFuncList->mCount;

        STL_TRY( qlvCreateRefExprList( &sTotalAggrFuncList,
                                       QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                       aEnv )
                 == STL_SUCCESS );

        sTotalAggrFuncList->mCount = sPlanIndexScan->mCommon.mAggrExprList.mCount;
        sTotalAggrFuncList->mHead = sPlanIndexScan->mCommon.mAggrExprList.mHead;
        sTotalAggrFuncList->mTail = sPlanIndexScan->mCommon.mAggrExprList.mTail;
    }
    else
    {
        /* Do Nothing */
    }


    /****************************************************************
     * INDEX ACCESS Optimization Node 정보 설정
     ****************************************************************/

    /**
     * Relation Node
     */
    
    sOptIndexAccess->mRelationNode = sPlanIndexScan->mCommon.mInitNode;


    /**
     * Row Block이 필요한지 여부
     */

    sOptIndexAccess->mNeedRowBlock = sPlanIndexScan->mCommon.mNeedRowBlock;

    
    /**
     * Index Handle
     */

    STL_TRY( qlnoAllocAndCopyString( ellGetTableName( sPlanIndexScan->mTableHandle ),
                                     & sOptIndexAccess->mTableName,
                                     QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                     aEnv )
             == STL_SUCCESS );
    
    sOptIndexAccess->mTableHandle          = sPlanIndexScan->mTableHandle;
    sOptIndexAccess->mTablePhysicalHandle  = sPlanIndexScan->mTablePhysicalHandle;

    STL_TRY( qlnoAllocAndCopyString( ellGetIndexName( sPlanIndexScan->mIndexHandle ),
                                     & sOptIndexAccess->mIndexName,
                                     QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                     aEnv )
             == STL_SUCCESS );

    sOptIndexAccess->mIndexHandle          = sPlanIndexScan->mIndexHandle;
    sOptIndexAccess->mIndexPhysicalHandle  = sPlanIndexScan->mIndexPhysicalHandle;
    sOptIndexAccess->mScanDirection        = (sPlanIndexScan->mIsIndexScanAsc == STL_TRUE ?
                                              SML_SCAN_FORWARD : SML_SCAN_BACKWARD);
    
    /**
     * Read Column List
     */

    sOptIndexAccess->mIndexReadColList = sIndexReadColumnList;
    sOptIndexAccess->mTableReadColList = sTableReadColumnList;
    sOptIndexAccess->mTotalReadColList = sPlanIndexScan->mReadColList;

    
    /**
     * Read RowId Column
     */

    sOptIndexAccess->mRowIdColumn = sRowIdColumn;


    /**
     * Outer Column 관련
     */

    sOptIndexAccess->mOuterColumnList = sPlanIndexScan->mOuterColumnList;


    /**
     * Index Scan Info
     */

    sOptIndexAccess->mIndexScanInfo = sIndexScanInfo;


    /**
     * Aggregation 관련
     */

    sOptIndexAccess->mAggrFuncCnt       = sAggrFuncCnt;
    sOptIndexAccess->mAggrFuncList      = sAggrFuncList;

    sOptIndexAccess->mTotalAggrFuncList = sTotalAggrFuncList;
    sOptIndexAccess->mAggrDistFuncCnt   = sAggrDistFuncCnt;
    sOptIndexAccess->mAggrDistFuncList  = sAggrDistFuncList;

    
    /**
     * IN Key Range 관련
     */

    sOptIndexAccess->mListColMapCount = sPlanIndexScan->mListColMapCount;
    sOptIndexAccess->mValueExpr       = sPlanIndexScan->mValueExpr;
    sOptIndexAccess->mListColExpr     = sPlanIndexScan->mListColExpr;


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
 * @brief INDEX ACCESS의 Optimization Node 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoCompleteIndexAccess( qllDBCStmt            * aDBCStmt,
                                   qllStatement          * aSQLStmt,
                                   qllOptNodeList        * aOptNodeList,
                                   qllOptimizationNode   * aOptNode,
                                   qloInitExprList       * aQueryExprList,
                                   qllEnv                * aEnv )
{
    qlnoIndexAccess    * sOptIndexAccess     = NULL;

    knlExprStack       * sKeyLogicalStack    = NULL;
    knlExprStack       * sTableLogicalStack  = NULL;

    qlvInitExpression  * sCodeExpr           = NULL;
    knlExprEntry       * sExprEntry          = NULL;

    qloIndexScanInfo   * sScanInfo           = NULL;
        
    qlvInitExpression  * sAndExpr            = NULL;
    qlvRefExprItem     * sExprItem           = NULL;
    ellDictHandle      * sColumnHandle       = NULL;

    
    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_INDEX_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );    
    

    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/

    sOptIndexAccess = (qlnoIndexAccess *) aOptNode->mOptNode;

    STL_DASSERT( sOptIndexAccess->mIndexScanInfo != NULL );

    sScanInfo = sOptIndexAccess->mIndexScanInfo;


    /****************************************************************
     * Read Column에 대한 DB Type 설정
     ****************************************************************/

    if( sOptIndexAccess->mIndexReadColList != NULL )
    {
        sExprItem = sOptIndexAccess->mIndexReadColList->mHead;

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

    if( sOptIndexAccess->mTableReadColList != NULL )
    {
        sExprItem = sOptIndexAccess->mTableReadColList->mHead;

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

    if( sOptIndexAccess->mRowIdColumn != NULL )
    {
        /* ROWID COLUMN 에 대한 DBType 설정 */
        STL_TRY( qloSetExprDBType( sOptIndexAccess->mRowIdColumn,
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
     * Range 관련 설정
     ****************************************************************/
    
    /**
     * Prepare Range Expression Stack 생성
     */

    STL_TRY( qloMakeRangeStack( aDBCStmt,
                                aSQLStmt,
                                aQueryExprList,
                                sScanInfo->mReadKeyCnt,
                                STL_FALSE,
                                sScanInfo->mMinRangeExpr,
                                sScanInfo->mMaxRangeExpr,
                                & sOptIndexAccess->mPrepareRangeStack,
                                aEnv )
             == STL_SUCCESS );

    
    /****************************************************************
     * Key-Filter 관련 설정
     ****************************************************************/

    /**
     * Prepare Key Filter Expression Stack 생성
     */

    if( sScanInfo->mPhysicalKeyFilterExprList == NULL )
    {
        sOptIndexAccess->mPrepareKeyFilterStack = NULL;
    }
    else
    {
        if( sScanInfo->mPhysicalKeyFilterExprList->mCount == 0 )
        {
            sOptIndexAccess->mPrepareKeyFilterStack = NULL;
        }
        else
        {
            STL_TRY( qlvMakeFuncAndFromExprList( & QLL_CANDIDATE_MEM( aEnv ),
                                                 0,
                                                 sScanInfo->mPhysicalKeyFilterExprList,
                                                 & sAndExpr,
                                                 aEnv )
                     == STL_SUCCESS );

            STL_TRY( qloMakePhysicalFilterStack( aDBCStmt,
                                                 aSQLStmt,
                                                 aQueryExprList,
                                                 sAndExpr,
                                                 & sOptIndexAccess->mPrepareKeyFilterStack,
                                                 aEnv )
                     == STL_SUCCESS );
        }
    }
    

    /****************************************************************
     * Table Physical Filter 관련 설정
     ****************************************************************/
    
    /**
     * Prepare Table Filter Expression Stack 생성
     */
        
    if( sScanInfo->mPhysicalTableFilterExprList == NULL )
    {
        sOptIndexAccess->mPrepareFilterStack = NULL;
    }
    else
    {
        if( sScanInfo->mPhysicalTableFilterExprList->mCount == 0 )
        {
            sOptIndexAccess->mPrepareFilterStack = NULL;
        }
        else
        {
            STL_TRY( qlvMakeFuncAndFromExprList( & QLL_CANDIDATE_MEM( aEnv ),
                                                 0,
                                                 sScanInfo->mPhysicalTableFilterExprList,
                                                 & sAndExpr,
                                                 aEnv )
                     == STL_SUCCESS );

            STL_TRY( qloMakePhysicalFilterStack( aDBCStmt,
                                                 aSQLStmt,
                                                 aQueryExprList,
                                                 sAndExpr,
                                                 & sOptIndexAccess->mPrepareFilterStack,
                                                 aEnv )
                     == STL_SUCCESS );
        }
    }
    
    
    /****************************************************************
     * Key Logical Filter 관련 설정
     ****************************************************************/

    /**
     * Key Logical Filter를 위한 Expression Stack 구성
     */
        
    if( sScanInfo->mLogicalKeyFilter == NULL )
    {
        sKeyLogicalStack = NULL;
    }
    else
    {
        /**
         * Logical Expression Code Stack 공간 할당
         */

        STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                    STL_SIZEOF( knlExprStack ),
                                    (void **) & sKeyLogicalStack,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );


        /**
         * Logical Expression Stack 생성
         */

        sCodeExpr = sScanInfo->mLogicalKeyFilter;

        STL_TRY( knlExprInit( sKeyLogicalStack,
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
                                            sKeyLogicalStack,
                                            aQueryExprList->mStmtExprList->mConstExprStack,
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
        
    
    /****************************************************************
     * Table Logical Filter 관련 설정
     ****************************************************************/

    /**
     * Table Logical Filter를 위한 Expression Stack 구성
     */
        
    if( sScanInfo->mLogicalTableFilter == NULL )
    {
        sTableLogicalStack = NULL;
    }
    else
    {
        /**
         * Logical Expression Code Stack 공간 할당
         */

        STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                    STL_SIZEOF( knlExprStack ),
                                    (void **) & sTableLogicalStack,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );


        /**
         * Logical Expression Stack 생성
         */

        sCodeExpr = sScanInfo->mLogicalTableFilter;

        STL_TRY( knlExprInit( sTableLogicalStack,
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
                                            sTableLogicalStack,
                                            aQueryExprList->mStmtExprList->mConstExprStack,
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


    /****************************************************************
     * Index Scan을 위해 추가된 Expression 관련 설정
     ****************************************************************/
    
    /**
     * Add FALSE Expression
     */

    if( sScanInfo->mNullValueExpr != NULL )
    {
        STL_TRY( qloSetExprDBType( sScanInfo->mNullValueExpr,
                                   aQueryExprList->mStmtExprList->mTotalExprInfo,
                                   DTL_TYPE_BOOLEAN,
                                   gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mArgNum1,
                                   gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mArgNum2,
                                   gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mStringLengthUnit,
                                   gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mIntervalIndicator,
                                   aEnv )
                 == STL_SUCCESS );
    }

    
    /**
     * Add FALSE Expression
     */

    if( sScanInfo->mFalseValueExpr != NULL )
    {
        STL_TRY( qloSetExprDBType( sScanInfo->mFalseValueExpr,
                                   aQueryExprList->mStmtExprList->mTotalExprInfo,
                                   DTL_TYPE_BOOLEAN,
                                   gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mArgNum1,
                                   gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mArgNum2,
                                   gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mStringLengthUnit,
                                   gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mIntervalIndicator,
                                   aEnv )
                 == STL_SUCCESS );
    }

    
    /****************************************************************
     * Aggregation 에 대한 Expression Stack 구성
     ****************************************************************/

    /**
     * Aggregation Function 정보 구축
     */

    if( sOptIndexAccess->mAggrFuncCnt > 0 )
    {
        STL_TRY( qlnfGetAggrOptInfo( aSQLStmt,
                                     aQueryExprList,
                                     sOptIndexAccess->mAggrFuncCnt,
                                     sOptIndexAccess->mAggrFuncList,
                                     & sOptIndexAccess->mAggrOptInfo,
                                     QLL_CODE_PLAN( aDBCStmt ),
                                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sOptIndexAccess->mAggrOptInfo = NULL;
    }


    /****************************************************************
     * Aggregation Distinct에 대한 Expression Stack 구성
     ****************************************************************/

    /**
     * Aggregation Function 정보 구축
     */

    if( sOptIndexAccess->mAggrDistFuncCnt > 0 )
    {
        STL_TRY( qlnfGetAggrOptInfo( aSQLStmt,
                                     aQueryExprList,
                                     sOptIndexAccess->mAggrDistFuncCnt,
                                     sOptIndexAccess->mAggrDistFuncList,
                                     & sOptIndexAccess->mAggrDistOptInfo,
                                     QLL_CODE_PLAN( aDBCStmt ),
                                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sOptIndexAccess->mAggrDistOptInfo = NULL;
    }
    

    /****************************************************************
     * IN Key Range 관련 Internal Cast 설정 (Range & Filter Expr Stack 구성 이후에 )
     ****************************************************************/

    /**
     * Code Stack 구성
     */

    STL_TRY( qloMakeExprStackForInKeyRange( aDBCStmt,
                                            aSQLStmt,
                                            aQueryExprList,
                                            sOptIndexAccess->mListColMapCount,
                                            sOptIndexAccess->mValueExpr,
                                            sOptIndexAccess->mListColExpr,
                                            & sOptIndexAccess->mPrepareInKeyRangeStack,
                                            aEnv )
             == STL_SUCCESS );


    /****************************************************************
     * Node 정보 설정
     ****************************************************************/
    
    /**
     * Table Logical Filter Stack
     */
    
    sOptIndexAccess->mKeyLogicalStack    = sKeyLogicalStack;
    sOptIndexAccess->mTableLogicalStack  = sTableLogicalStack;


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
 * @brief INDEX ACCESS의 All Expr 리스트 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoAddExprIndexAccess( qllDBCStmt            * aDBCStmt,
                                  qllStatement          * aSQLStmt,
                                  qllOptNodeList        * aOptNodeList,
                                  qllOptimizationNode   * aOptNode,
                                  qloInitExprList       * aQueryExprList,
                                  qllEnv                * aEnv )
{
    qlnoIndexAccess   * sOptIndexAccess     = NULL;
    qlvRefExprItem    * sRefColumnItem      = NULL;
    qlvRefExprItem    * sRefExprItem        = NULL;
    qlvRefExprItem    * sMinRefColumnItem   = NULL;
    qlvRefExprItem    * sMaxRefColumnItem   = NULL;
    qloIndexScanInfo  * sScanInfo           = NULL;
    qlvInitListCol    * sListCol            = NULL;
    qlvInitRowExpr    * sRowExpr            = NULL;
    stlInt32            sLoop               = 0;
    stlInt32            sRowIdx             = 0;


    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_INDEX_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );    
    

    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/

    sOptIndexAccess = (qlnoIndexAccess *) aOptNode->mOptNode;
    sScanInfo = sOptIndexAccess->mIndexScanInfo;

    /****************************************************************
     * Add Expr
     ****************************************************************/

    /**
     * Add Expr Index Read Column for Index Access
     */

    sRefColumnItem = sOptIndexAccess->mIndexReadColList->mHead;
    while( sRefColumnItem != NULL )
    {
        STL_TRY( qloAddExpr( sRefColumnItem->mExprPtr,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN( aDBCStmt ),
                             aEnv )
                 == STL_SUCCESS );

        sRefColumnItem = sRefColumnItem->mNext;
    }


    /**
     * Add Expr Table Read Column for Index Access
     */

    sRefColumnItem = sOptIndexAccess->mTableReadColList->mHead;
    while( sRefColumnItem != NULL )
    {
        STL_TRY( qloAddExpr( sRefColumnItem->mExprPtr,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN( aDBCStmt ),
                             aEnv )
                 == STL_SUCCESS );

        sRefColumnItem = sRefColumnItem->mNext;
    }

    /**
     * Add Expr Table Access Read RowId Column
     */

    if( sOptIndexAccess->mRowIdColumn != NULL )
    {
        STL_TRY( qloAddExpr( sOptIndexAccess->mRowIdColumn,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN(aDBCStmt),
                             aEnv )
                 == STL_SUCCESS );
    }

    /**
     * Add Expr Min-Range Expression
     */

    for( sLoop = 0 ; sLoop < sScanInfo->mReadKeyCnt ; sLoop++ )
    {
        sMinRefColumnItem = sScanInfo->mMinRangeExpr[ sLoop ]->mHead;
        
        while( sMinRefColumnItem != NULL )
        {
            STL_TRY( qloAddExpr( sMinRefColumnItem->mExprPtr,
                                 aQueryExprList->mStmtExprList->mAllExprList,
                                 QLL_CODE_PLAN( aDBCStmt ),
                                 aEnv )
                     == STL_SUCCESS );

            sMinRefColumnItem = sMinRefColumnItem->mNext;
        }
    }
    

    /**
     * Add Expr Max-Range Expression
     */

    for( sLoop = 0 ; sLoop < sScanInfo->mReadKeyCnt ; sLoop++ )
    {
        sMaxRefColumnItem = sScanInfo->mMaxRangeExpr[ sLoop ]->mHead;
        
        while( sMaxRefColumnItem != NULL )
        {
            sMinRefColumnItem = sScanInfo->mMinRangeExpr[ sLoop ]->mHead;
            while( sMinRefColumnItem != NULL )
            {
                if( sMaxRefColumnItem->mExprPtr == sMinRefColumnItem->mExprPtr )
                {
                    break;
                }

                sMinRefColumnItem = sMinRefColumnItem->mNext;
            }

            if( sMinRefColumnItem == NULL )
            {
                STL_TRY( qloAddExpr( sMaxRefColumnItem->mExprPtr,
                                     aQueryExprList->mStmtExprList->mAllExprList,
                                     QLL_CODE_PLAN( aDBCStmt ),
                                     aEnv )
                         == STL_SUCCESS );
            }

            sMaxRefColumnItem = sMaxRefColumnItem->mNext;
        }
    }


    /**
     * Add Expr List Column Expression For IN Key Range
     */

    for( sLoop = 0 ; sLoop < sOptIndexAccess->mListColMapCount ; sLoop++ )
    {
        STL_DASSERT( sOptIndexAccess->mListColExpr[ sLoop ]->mType ==
                     QLV_EXPR_TYPE_LIST_COLUMN );
        
        sListCol = sOptIndexAccess->mListColExpr[ sLoop ]->mExpr.mListCol;

        for( sRowIdx = 0 ; sRowIdx < sListCol->mArgCount ; sRowIdx++ )
        {
            sRowExpr = sListCol->mArgs[ sRowIdx ]->mExpr.mRowExpr;

            STL_DASSERT( sRowExpr->mArgCount == 1 );

            STL_TRY( qloAddExpr( sRowExpr->mArgs[ 0 ],
                                 aQueryExprList->mStmtExprList->mAllExprList,
                                 QLL_CODE_PLAN( aDBCStmt ),
                                 aEnv )
                     == STL_SUCCESS );
        }
    }
    

    /**
     * Add Expr Physical Key Filter Expression
     */

    if( sScanInfo->mPhysicalKeyFilterExprList != NULL )
    {
        sRefColumnItem = sScanInfo->mPhysicalKeyFilterExprList->mHead;
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
     * Add Expr Physical Table Filter Expression
     */

    if( sScanInfo->mPhysicalTableFilterExprList != NULL )
    {
        sRefColumnItem = sScanInfo->mPhysicalTableFilterExprList->mHead;
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
     * Add Expr Logical Key Filter Expression
     */

    if( sScanInfo->mLogicalKeyFilter != NULL )
    {
        STL_TRY( qloAddExpr( sScanInfo->mLogicalKeyFilter,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN( aDBCStmt ),
                             aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Add Expr Logical Table Filter Expression
     */

    if( sScanInfo->mLogicalTableFilter != NULL )
    {
        STL_TRY( qloAddExpr( sScanInfo->mLogicalTableFilter,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN( aDBCStmt ),
                             aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Add NULL Expression
     */

    if( sScanInfo->mNullValueExpr != NULL )
    {
        STL_TRY( qloAddExpr( sScanInfo->mNullValueExpr,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN( aDBCStmt ),
                             aEnv )
                 == STL_SUCCESS );
    }

    
    /**
     * Add FALSE Expression
     */

    if( sScanInfo->mFalseValueExpr != NULL )
    {
        STL_TRY( qloAddExpr( sScanInfo->mFalseValueExpr,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN( aDBCStmt ),
                             aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Add Expr Aggregation Functions
     */

    if( sOptIndexAccess->mAggrFuncCnt > 0 )
    {
        sRefExprItem = sOptIndexAccess->mAggrFuncList->mHead;

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

    if( sOptIndexAccess->mAggrDistFuncCnt > 0 )
    {
        sRefExprItem = sOptIndexAccess->mAggrDistFuncList->mHead;

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
