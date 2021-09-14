/*******************************************************************************
 * qlnoGroupSortInstantAccess.c
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
 * @file qlnoGroupSortInstantAccess.c
 * @brief SQL Processor Layer Code Optimization Node - GROUP INSTANT ACCESS
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlno
 * @{
 */


#if 0
/**
 * @brief Sort Group Instant에 대한 Code Optimize를 수행한다.
 * @param[in]   aCodeOptParamInfo   Code Optimize Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlnoCodeOptimizeSortGroupInstant( qloCodeOptParamInfo * aCodeOptParamInfo,
                                            qllEnv              * aEnv )
{
    /*
     * Paramter Validation
     */

    QLO_CODE_OPT_DEFAULT_PARAM_VALIDATE( aCodeOptParamInfo, aEnv );
    STL_PARAM_VALIDATE( aCodeOptParamInfo->mCandPlan->mPlanType == QLNC_PLAN_TYPE_SORT_GROUP_INSTANT,
                        QLL_ERROR_STACK(aEnv) );


    STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      "qlnoCodeOptimizeSortGroupInstant()" );
    }

    STL_FINISH;

    return STL_FAILURE;
}
#endif


#if 0
/**
 * @brief GROUP INSTANT ACCESS의 Optimization Node 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoCompleteGroupSortInstantAccess( qllDBCStmt            * aDBCStmt,
                                              qllStatement          * aSQLStmt,
                                              qllOptNodeList        * aOptNodeList,
                                              qllOptimizationNode   * aOptNode,
                                              qloInitExprList       * aQueryExprList,
                                              qllEnv                * aEnv )
{
    qlnoInstant        * sOptGroupInstant    = NULL;

    knlExprStack       * sKeyLogicalStack    = NULL;
    knlExprStack       * sTableLogicalStack  = NULL;

    qlvRefExprItem     * sRefColumnItem      = NULL;
    qloDBType          * sExprDBType         = NULL;
    qlvInstantColDesc  * sColumnDesc         = NULL;
    qlvInstantColDesc  * sCurColumnDesc      = NULL;
    
    qlvInitExpression  * sCodeExpr           = NULL;
    knlExprEntry       * sExprEntry          = NULL;
    knlExprEntry         sTempExprEntry;

    knlExprStack       * sTempCodeStack      = NULL;

    qlvRefExprItem     * sListItem           = NULL;
    qloIndexScanInfo   * sScanInfo           = NULL;

    qlnfAggrOptInfo    * sAggrOptInfo        = NULL;
    qlvInitAggregation * sAggrExpr           = NULL;
    stlInt32             sArgCnt             = 0;
    
    knlExprStack       * sColExprStack       = NULL;
    knlExprStack       * sCodeStack          = NULL;
    knlExprStack       * sAggrStack          = NULL;
    knlExprStack         sTempColExprStack;

    stlInt32             sLoop               = 0;
    stlInt32             sIdx                = 0;
    stlInt32             sTotalColCnt        = 0;

    qlvInitInstantNode * sIniGroupInstant    = NULL;
    knlPredicateList   * sRangePred          = NULL;
    

    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_GROUP_SORT_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList->mStmtExprList != NULL, QLL_ERROR_STACK(aEnv) );    
    

    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/

    sOptGroupInstant = (qlnoInstant *) aOptNode->mOptNode;

    STL_DASSERT( sOptGroupInstant->mIndexScanInfo != NULL );

    sScanInfo = sOptGroupInstant->mIndexScanInfo;

    /* 임시 Column Expression Stack 초기화 */
    STL_TRY( knlExprInit( & sTempColExprStack,
                          1,
                          & QLL_CANDIDATE_MEM( aEnv ),
                          KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    
    /****************************************************************
     * 하위 node에 대한 Complete Node 수행
     ****************************************************************/

    QLNO_COMPLETE( aDBCStmt,
                   aSQLStmt,
                   aOptNodeList,
                   sOptGroupInstant->mChildNode,
                   aQueryExprList,
                   aEnv );

    
    /****************************************************************
     * Column Expression Stack 설정
     ****************************************************************/

    /**
     * Column Expression Code Stack 공간 할당
     */

    sTotalColCnt = ( sOptGroupInstant->mInstantNode->mKeyColCnt +
                     sOptGroupInstant->mInstantNode->mRecColCnt );
    
    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( knlExprStack ) * sTotalColCnt,
                                (void **) & sColExprStack,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    

    /**
     * Group Key Column Expression Stack 설정 (for Insert Columns)
     */
    
    sIdx = 0;

    sRefColumnItem = sOptGroupInstant->mInstantNode->mKeyColList->mHead;
    for( sLoop = 0 ; sLoop < sOptGroupInstant->mInstantNode->mKeyColCnt ; sLoop++, sIdx++ )
    {
        /**
         * Logical Expression Stack 생성
         */

        sCodeExpr = sRefColumnItem->mExprPtr->mExpr.mInnerColumn->mOrgExpr;

        STL_TRY( knlExprInit( & sColExprStack[ sIdx ],
                              sCodeExpr->mIncludeExprCnt,
                              QLL_CODE_PLAN( aDBCStmt ),
                              KNL_ENV( aEnv ) )
                 == STL_SUCCESS );

        
        /**
         * Column Expression Stack 구성
         */

        STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                            sCodeExpr,
                                            aSQLStmt->mBindContext,
                                            & sColExprStack[ sIdx ],
                                            aQueryExprList->mStmtExprList->mConstExprStack,
                                            aQueryExprList->mStmtExprList->mTotalExprInfo,
                                            & sExprEntry,
                                            QLL_CODE_PLAN( aDBCStmt ),
                                            aEnv )
                 == STL_SUCCESS );

        
        /**
         * Inner Column Expression Stack 구성
         */

        sCodeExpr = sRefColumnItem->mExprPtr;
        sTempColExprStack.mEntryCount = 0;

        STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                            sCodeExpr,
                                            aSQLStmt->mBindContext,
                                            & sTempColExprStack,
                                            aQueryExprList->mStmtExprList->mConstExprStack,
                                            aQueryExprList->mStmtExprList->mTotalExprInfo,
                                            & sExprEntry,
                                            & QLL_CANDIDATE_MEM( aEnv ),
                                            aEnv )
                 == STL_SUCCESS );

        sRefColumnItem = sRefColumnItem->mNext;
    }


    /**
     * Record Column Expression Stack 설정 (for Insert Columns)
     */

    sRefColumnItem = sOptGroupInstant->mInstantNode->mRecColList->mHead;
    for( sLoop = 0 ; sLoop < sOptGroupInstant->mInstantNode->mRecColCnt ; sLoop++, sIdx++ )
    {
        /**
         * Logical Expression Stack 생성
         */

        sCodeExpr = sRefColumnItem->mExprPtr->mExpr.mInnerColumn->mOrgExpr;

        STL_TRY( knlExprInit( & sColExprStack[ sIdx ],
                              sCodeExpr->mIncludeExprCnt,
                              QLL_CODE_PLAN( aDBCStmt ),
                              KNL_ENV( aEnv ) )
                 == STL_SUCCESS );

        
        /**
         * Column Expression Stack 구성
         */

        STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                            sCodeExpr,
                                            aSQLStmt->mBindContext,
                                            & sColExprStack[ sIdx ],
                                            aQueryExprList->mStmtExprList->mConstExprStack,
                                            aQueryExprList->mStmtExprList->mTotalExprInfo,
                                            & sExprEntry,
                                            QLL_CODE_PLAN( aDBCStmt ),
                                            aEnv )
                 == STL_SUCCESS );


        /**
         * Inner Column Expression Stack 구성
         */

        sCodeExpr = sRefColumnItem->mExprPtr;
        sTempColExprStack.mEntryCount = 0;

        STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                            sCodeExpr,
                                            aSQLStmt->mBindContext,
                                            & sTempColExprStack,
                                            aQueryExprList->mStmtExprList->mConstExprStack,
                                            aQueryExprList->mStmtExprList->mTotalExprInfo,
                                            & sExprEntry,
                                            & QLL_CANDIDATE_MEM( aEnv ),
                                            aEnv )
                 == STL_SUCCESS );

        sRefColumnItem = sRefColumnItem->mNext;
    }


    /**
     * 임시 Expression Code Stack 공간 할당
     */

    STL_TRY( knlAllocRegionMem( & QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( knlExprStack ),
                                (void **) & sTempCodeStack,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /****************************************************************
     * Range 관련 설정
     ****************************************************************/

    /**
     * 임시 Expression Code Stack 생성
     */

    STL_TRY( knlExprInit( sTempCodeStack,
                          4,
                          & QLL_CANDIDATE_MEM( aEnv ),
                          KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    
    /**
     * Min-Range Expression 정보 설정
     */
    
    for( sLoop = 0 ; sLoop < sScanInfo->mReadKeyCnt ; sLoop++ )
    {
        sListItem = sScanInfo->mMinRangeExpr[ sLoop ]->mHead;
        
        while( sListItem != NULL )
        {
            sTempCodeStack->mEntryCount = 0;
            sCodeExpr = sListItem->mExprPtr;

            STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                                sCodeExpr,
                                                aSQLStmt->mBindContext,
                                                sTempCodeStack,
                                                aQueryExprList->mStmtExprList->mConstExprStack,
                                                aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                & sExprEntry,
                                                & QLL_CANDIDATE_MEM( aEnv ),
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

            sListItem = sListItem->mNext;
        }
    }
    

    /**
     * Max-Range Expression 정보 설정
     */

    for( sLoop = 0 ; sLoop < sScanInfo->mReadKeyCnt ; sLoop++ )
    {
        sListItem = sScanInfo->mMaxRangeExpr[ sLoop ]->mHead;
        
        while( sListItem != NULL )
        {
            sTempCodeStack->mEntryCount = 0;
            sCodeExpr = sListItem->mExprPtr;

            STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                                sCodeExpr,
                                                aSQLStmt->mBindContext,
                                                sTempCodeStack,
                                                aQueryExprList->mStmtExprList->mConstExprStack,
                                                aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                & sExprEntry,
                                                & QLL_CANDIDATE_MEM( aEnv ),
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

            sListItem = sListItem->mNext;
        }
    }

    
    /****************************************************************
     * Key-Filter 관련 설정
     ****************************************************************/

    /**
     * Physical Key Filter를 구성하는 Expression에 대한 DB Type 정보 설정
     *  <BR> Physical Key Filter를 위한 임시 Predicate 생성
     */

    if( sScanInfo->mPhysicalKeyFilterExprList == NULL )
    {
        /* Do Nothing */
    }
    else
    {
        if( sScanInfo->mPhysicalKeyFilterExprList->mCount == 0 )
        {
            /* Do Nothing */
        }
        else
        {
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
                                  4,
                                  & QLL_CANDIDATE_MEM( aEnv ),
                                  KNL_ENV( aEnv ) )
                     == STL_SUCCESS );


            /**
             * 임시 Expression Code Stack 구성
             */

            sListItem = sScanInfo->mPhysicalKeyFilterExprList->mHead;

            while( sListItem != NULL )
            {
                sTempCodeStack->mEntryCount = 0;
                sCodeExpr = sListItem->mExprPtr;
                
                STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                                    sCodeExpr,
                                                    aSQLStmt->mBindContext,
                                                    sTempCodeStack,
                                                    aQueryExprList->mStmtExprList->mConstExprStack,
                                                    aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                    & sExprEntry,
                                                    & QLL_CANDIDATE_MEM( aEnv ),
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

                sListItem = sListItem->mNext;
            }
        }
    }
    
    
    /****************************************************************
     * Table Physical Filter 관련 설정
     ****************************************************************/
    
    /**
     * Table Physical Filter를 구성하는 Expression에 대한 DB Type 정보 설정
     *  <BR> Table Physical Filter를 위한 임시 Predicate 생성
     */
        
    if( sScanInfo->mPhysicalTableFilterExprList == NULL )
    {
        /* Do Nothing */
    }
    else
    {
        if( sScanInfo->mPhysicalTableFilterExprList->mCount == 0 )
        {
            /* Do Nothing */
        }
        else
        {
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
                                  4,
                                  & QLL_CANDIDATE_MEM( aEnv ),
                                  KNL_ENV( aEnv ) )
                     == STL_SUCCESS );


            /**
             * 임시 Expression Code Stack 구성
             */

            sListItem = sScanInfo->mPhysicalTableFilterExprList->mHead;

            while( sListItem != NULL )
            {
                sTempCodeStack->mEntryCount = 0;
                sCodeExpr = sListItem->mExprPtr;
                
                STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                                    sCodeExpr,
                                                    aSQLStmt->mBindContext,
                                                    sTempCodeStack,
                                                    aQueryExprList->mStmtExprList->mConstExprStack,
                                                    aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                    & sExprEntry,
                                                    & QLL_CANDIDATE_MEM( aEnv ),
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

                sListItem = sListItem->mNext;
            }
        }
    }
    

    /****************************************************************
     * Aggregation 에 대한 Expression Stack 구성
     ****************************************************************/
    
    /**
     * Aggregation Argument와 Filter에 대한 Expression Stack 구성
     */

    sAggrOptInfo = sOptGroupInstant->mAggrOptInfo;
    
    for( sLoop = 0 ; sLoop < sOptGroupInstant->mAggrFuncCnt ; sLoop++ )
    {
        /**
         * Aggregation Argument Expression Stack 설정
         */

        sAggrExpr = sAggrOptInfo->mAggrExpr->mExpr.mAggregation;

        if( sAggrExpr->mArgCount > 0 )
        {
            /**
             * Argument Expression Stack 공간 할당
             */
        
            STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                        STL_SIZEOF( knlExprStack * ) * sAggrExpr->mArgCount,
                                        (void **) & sAggrOptInfo->mArgCodeStack,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                        STL_SIZEOF( knlExprStack ) * sAggrExpr->mArgCount,
                                        (void **) & sAggrStack,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            for( sArgCnt = 0 ; sArgCnt < sAggrExpr->mArgCount ; sArgCnt++ )
            {
                sCodeExpr = sAggrExpr->mArgs[ sArgCnt ];

                /**
                 * Argument Expression Stack 생성
                 */

                sCodeStack = & sAggrStack[ sArgCnt ];

                STL_TRY( knlExprInit( sCodeStack,
                                      sCodeExpr->mIncludeExprCnt,
                                      QLL_CODE_PLAN( aDBCStmt ),
                                      KNL_ENV( aEnv ) )
                         == STL_SUCCESS );

        
                /**
                 * Argument Expression Stack 구성
                 */
            
                STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                                    sCodeExpr,
                                                    aSQLStmt->mBindContext,
                                                    sCodeStack,
                                                    aQueryExprList->mStmtExprList->mConstExprStack,
                                                    aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                    & sExprEntry,
                                                    QLL_CODE_PLAN( aDBCStmt ),
                                                    aEnv )
                         == STL_SUCCESS );

                /**
                 * Argument Expression Stack 구성
                 */

                if( ( sCodeStack->mEntries[1].mExprType == KNL_EXPR_TYPE_VALUE ) &&
                    ( sCodeStack->mEntryCount == 1 ) &&
                    ( aQueryExprList->mStmtExprList->mTotalExprInfo->mExprDBType
                      [ sCodeStack->mEntries[1].mExpr.mValueInfo.mOffset ].mIsNeedCast == STL_FALSE ) )
                {
                    sAggrOptInfo->mArgCodeStack[ sArgCnt ] = NULL;
                }
                else
                {
                    sAggrOptInfo->mArgCodeStack[ sArgCnt ] = sCodeStack;
                }
            }
        }
        else
        {
            sAggrOptInfo->mArgCodeStack = NULL;
        }


        /**
         * Aggregation에 대한 DB type 설정
         */
        
        STL_TRY( gQlnfAggrFuncInfoArr[sAggrOptInfo->mAggrFuncId].mSetDBTypeFuncPtr(
                     sAggrOptInfo->mAggrExpr,
                     aQueryExprList->mStmtExprList->mTotalExprInfo,
                     aEnv )
                 == STL_SUCCESS );


        /**
         * Filter Expression Stack 설정
         */

        if( sAggrExpr->mFilter != NULL )
        {
            /**
             * Filter Expression Stack 공간 할당
             */
            
            STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                        STL_SIZEOF( knlExprStack ),
                                        (void **) & sAggrOptInfo->mFilterCodeStack,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            

            /**
             * Filter Expression Stack 생성
             */

            STL_TRY( knlExprInit( sAggrOptInfo->mFilterCodeStack,
                                  sAggrExpr->mFilter->mIncludeExprCnt,
                                  QLL_CODE_PLAN( aDBCStmt ),
                                  KNL_ENV( aEnv ) )
                     == STL_SUCCESS );
            
    
            /**
             * Filter Expression Stack 구성
             */

            STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                                sAggrExpr->mFilter,
                                                aSQLStmt->mBindContext,
                                                sAggrOptInfo->mFilterCodeStack,
                                                aQueryExprList->mStmtExprList->mConstExprStack,
                                                aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                & sExprEntry,
                                                QLL_CODE_PLAN( aDBCStmt ),
                                                aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            sAggrOptInfo->mFilterCodeStack = NULL;
        }

        sAggrOptInfo++;
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
        stlMemset( & sTempExprEntry, 0x00, STL_SIZEOF( knlExprEntry ) );
        STL_TRY( qloSetExprDBType( sScanInfo->mNullValueExpr,
                                   aQueryExprList->mStmtExprList->mTotalExprInfo,
                                   & sTempExprEntry,
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
        stlMemset( & sTempExprEntry, 0x00, STL_SIZEOF( knlExprEntry ) );
        STL_TRY( qloSetExprDBType( sScanInfo->mFalseValueExpr,
                                   aQueryExprList->mStmtExprList->mTotalExprInfo,
                                   & sTempExprEntry,
                                   DTL_TYPE_BOOLEAN,
                                   gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mArgNum1,
                                   gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mArgNum2,
                                   gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mStringLengthUnit,
                                   gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mIntervalIndicator,
                                   aEnv )
                 == STL_SUCCESS );
    }    


    /**
     * Range Predicate 구성
     */

    if( sKeyLogicalStack == NULL )
    {
        sRangePred = NULL;
    }
    else
    {
        STL_TRY( knlMakeRange( NULL, /* 설정할 필요 없음 */
                               NULL, /* 설정할 필요 없음 */
                               sKeyLogicalStack,
                               & sRangePred,
                               QLL_CODE_PLAN(aDBCStmt),
                               KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
    }


    /****************************************************************
     * Column Description 정보 설정
     ****************************************************************/

    /**
     * Column Description 공간 할당
     */

    sIniGroupInstant = sOptGroupInstant->mInstantNode;

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlvInstantColDesc ) * sTotalColCnt,
                                (void **) & sColumnDesc,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Group Key Column Descriptor 설정
     */

    sIdx = 0;

    sRefColumnItem = sOptGroupInstant->mInstantNode->mKeyColList->mHead;
    for( sLoop = 0 ; sLoop < sOptGroupInstant->mInstantNode->mKeyColCnt ; sLoop++, sIdx++ )
    {
        STL_DASSERT( sRefColumnItem != NULL );
        STL_DASSERT( sIdx == *sRefColumnItem->mExprPtr->mExpr.mInnerColumn->mIdx );
        
        sCurColumnDesc = & sColumnDesc[ sIdx ];
        
        sCurColumnDesc->mIdx         = sIdx;
        
        sCurColumnDesc->mIsSortKey   = STL_TRUE;
        sCurColumnDesc->mKeyColFlags = sIniGroupInstant->mKeyFlags[ sIdx ];

        sExprDBType = & aQueryExprList->mStmtExprList->mTotalExprInfo->mExprDBType[ sRefColumnItem->mExprPtr->mOffset ];
            
        sCurColumnDesc->mType              = sExprDBType->mDBType;
        sCurColumnDesc->mArgNum1           = sExprDBType->mArgNum1;
        sCurColumnDesc->mArgNum2           = sExprDBType->mArgNum2;
        sCurColumnDesc->mStringLengthUnit  = sExprDBType->mStringLengthUnit;
        sCurColumnDesc->mIntervalIndicator = sExprDBType->mIntervalIndicator;

        sCurColumnDesc->mExpr = sRefColumnItem->mExprPtr;

        sRefColumnItem = sRefColumnItem->mNext;
    }
    

    /**
     * Record Column Descriptor 설정
     */

    sRefColumnItem = sOptGroupInstant->mInstantNode->mRecColList->mHead;
    for( sLoop = 0 ; sLoop < sOptGroupInstant->mInstantNode->mRecColCnt ; sLoop++, sIdx++ )
    {
        STL_DASSERT( sRefColumnItem != NULL );
        STL_DASSERT( sIdx == *sRefColumnItem->mExprPtr->mExpr.mInnerColumn->mIdx );
        
        sCurColumnDesc = & sColumnDesc[ sIdx ];
        
        sCurColumnDesc->mIdx         = sIdx;
        
        sCurColumnDesc->mIsSortKey   = STL_FALSE;
        sCurColumnDesc->mKeyColFlags = sIniGroupInstant->mKeyFlags[ sIdx ];

        sExprDBType = & aQueryExprList->mStmtExprList->mTotalExprInfo->mExprDBType[ sRefColumnItem->mExprPtr->mOffset ];
            
        sCurColumnDesc->mType              = sExprDBType->mDBType;
        sCurColumnDesc->mArgNum1           = sExprDBType->mArgNum1;
        sCurColumnDesc->mArgNum2           = sExprDBType->mArgNum2;
        sCurColumnDesc->mStringLengthUnit  = sExprDBType->mStringLengthUnit;
        sCurColumnDesc->mIntervalIndicator = sExprDBType->mIntervalIndicator;

        sCurColumnDesc->mExpr = sRefColumnItem->mExprPtr;

        sRefColumnItem = sRefColumnItem->mNext;
    }


    /****************************************************************
     * Node 정보 설정
     ****************************************************************/

    /**
     * Column 관련
     */
    
    sOptGroupInstant->mColumnDesc   = sColumnDesc;
    sOptGroupInstant->mColExprStack = sColExprStack;

    
    /**
     * Predicate 관련
     */

    sOptGroupInstant->mPredicate = sRangePred;

    
    /**
     * Table Filter 관련
     */

    sOptGroupInstant->mTablePhysicalFilterExpr = NULL;
    sOptGroupInstant->mTableLogicalFilterExpr  = sScanInfo->mLogicalTableFilter;
    sOptGroupInstant->mTableLogicalStack       = sTableLogicalStack;


    /**
     * Index Key Filter 관련
     */
    
    sOptGroupInstant->mIndexKeyLogicalStack = sKeyLogicalStack;


    /**
     * Aggregation 관련
     */

    /* sOptGroupInstant->mAggrOptInfoForFilter = NULL; */


    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_WHERE_CLAUSE )
    {
        (void)stlPopError( QLL_ERROR_STACK(aEnv) );

        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_BOOLEAN_EXPRESSION_EXPECTED,
                      qllMakeErrPosString( aSQLStmt->mRetrySQL,
                                           sCodeExpr->mPosition,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}
#endif


#if 0
/**
 * @brief GROUP INSTANT ACCESS의 All Expr 리스트 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoAddExprGroupSortInstantAccess( qllDBCStmt            * aDBCStmt,
                                             qllStatement          * aSQLStmt,
                                             qllOptNodeList        * aOptNodeList,
                                             qllOptimizationNode   * aOptNode,
                                             qloInitExprList       * aQueryExprList,
                                             qllEnv                * aEnv )
{
    qlnoInstant             * sOptGroupInstant   = NULL;
    qlvRefExprItem          * sRefColumnItem    = NULL;
    qlvInitInstantNode      * sInstantNode      = NULL;
    qlvRefExprItem          * sMinRefColumnItem = NULL;
    qlvRefExprItem          * sMaxRefColumnItem = NULL;
    qloIndexScanInfo        * sScanInfo         = NULL;
    stlInt32                  sLoop             = 0;

    
    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_GROUP_SORT_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );    

    
    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/

    sOptGroupInstant = (qlnoInstant *) aOptNode->mOptNode;
    sScanInfo = sOptGroupInstant->mIndexScanInfo;

    
    /****************************************************************
     * Child Node의 Add Expression 수행
     ****************************************************************/

    QLNO_ADD_EXPR( aDBCStmt,
                   aSQLStmt,
                   aOptNodeList,
                   sOptGroupInstant->mChildNode,
                   aQueryExprList,
                   aEnv );
    

    /****************************************************************
     * Add Expr
     ****************************************************************/

    /**
     * Add Expr Group Instant Table : Group Key Column
     */

    sInstantNode = sOptGroupInstant->mInstantNode;

    sRefColumnItem = sInstantNode->mKeyColList->mHead;
    while( sRefColumnItem != NULL )
    {
        STL_DASSERT( sRefColumnItem->mExprPtr->mType == QLV_EXPR_TYPE_INNER_COLUMN );

        STL_TRY( qloAddExpr( sRefColumnItem->mExprPtr->mExpr.mInnerColumn->mOrgExpr,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN( aDBCStmt ),
                             aEnv )
                 == STL_SUCCESS );

        STL_TRY( qloAddExpr( sRefColumnItem->mExprPtr,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN( aDBCStmt ),
                             aEnv )
                 == STL_SUCCESS );

        sRefColumnItem = sRefColumnItem->mNext;
    }


    /**
     * Add Expr Group Instant Table : Record Column
     */

    sRefColumnItem = sInstantNode->mRecColList->mHead;
    while( sRefColumnItem != NULL )
    {
        STL_DASSERT( sRefColumnItem->mExprPtr->mType == QLV_EXPR_TYPE_INNER_COLUMN );

        STL_TRY( qloAddExpr( sRefColumnItem->mExprPtr->mExpr.mInnerColumn->mOrgExpr,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN( aDBCStmt ),
                             aEnv )
                 == STL_SUCCESS );

        STL_TRY( qloAddExpr( sRefColumnItem->mExprPtr,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN( aDBCStmt ),
                             aEnv )
                 == STL_SUCCESS );

        sRefColumnItem = sRefColumnItem->mNext;
    }


    /**
     * Add Expr Group Instant Table Column
     * @remark Read Column은 Group Key Column과 Record Column의 부분 집합이기 때문에
     *         Read Column의 Original Expresssion을 전체 Expression List에 추가할 필요 없다.
     */

    /* if( sInstantNode->mReadColList != NULL ) */
    /* { */
    /*     sRefColumnItem = sInstantNode->mReadColList->mHead; */
    /*     while( sRefColumnItem != NULL ) */
    /*     { */
    /*         STL_DASSERT( sRefColumnItem->mExprPtr->mType == QLV_EXPR_TYPE_INNER_COLUMN ); */

    /*         STL_TRY( qloAddExpr( sRefColumnItem->mExprPtr, */
    /*                              aQueryExprList->mStmtExprList->mAllExprList, */
    /*                              QLL_CODE_PLAN( aDBCStmt ), */
    /*                              aEnv ) */
    /*                  == STL_SUCCESS ); */

    /*         sRefColumnItem = sRefColumnItem->mNext; */
    /*     } */
    /* } */

    
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


    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}
#endif


/** @} qlno */
