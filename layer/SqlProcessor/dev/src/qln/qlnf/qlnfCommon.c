/*******************************************************************************
 * qlnfCommon.c
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
 * @file qlnfCommon.c
 * @brief SQL Processor Layer Built-In Function Node
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnf
 * @{
 */


/************************************************
 * Function Info
 ***********************************************/

qlnfAggrFuncInfo gQlnfAggrFuncInfoArr[ KNL_BUILTIN_AGGREGATION_UNARY_MAX ] =
{
    /**
     * UNARY FUNCTIONS
     */
    
    {
        KNL_BUILTIN_AGGREGATION_INVALID,
        0,
        0,
        "INVALID",
        qlnfInvalidSetAggrDBType,
        qlnfInvalidGetAggrExecInfo,
        qlnfInvalidGetAggrDistExecInfo,
        qlnfInvalidInit,
        qlnfInvalidBuild
    },  
    
    {
        KNL_BUILTIN_AGGREGATION_SUM,
        1,
        1,
        "SUM",
        qlnfSumSetAggrDBType,
        qlnfSumGetAggrExecInfo,
        qlnfSumGetAggrDistExecInfo,
        qlnfSumInit,
        qlnfSumBuild
    },
    
    {
        KNL_BUILTIN_AGGREGATION_COUNT,
        1,
        1,
        "COUNT",
        qlnfCountSetAggrDBType,
        qlnfCountGetAggrExecInfo,
        qlnfCountGetAggrDistExecInfo,
        qlnfCountInit,
        qlnfCountBuild
    },
    
    {
        KNL_BUILTIN_AGGREGATION_COUNT_ASTERISK,
        0,
        0,
        "COUNT(*)",
        qlnfCountAsteriskSetAggrDBType,
        qlnfCountAsteriskGetAggrExecInfo,
        qlnfInvalidGetAggrDistExecInfo,
        qlnfCountAsteriskInit,
        qlnfCountAsteriskBuild
    },
    
    {
        KNL_BUILTIN_AGGREGATION_MIN,
        1,
        1,
        "MIN",
        qlnfMinSetAggrDBType,
        qlnfMinGetAggrExecInfo,
        qlnfInvalidGetAggrDistExecInfo,
        qlnfMinInit,
        qlnfMinBuild
    },
    
    {
        KNL_BUILTIN_AGGREGATION_MAX,
        1,
        1,
        "MAX",
        qlnfMaxSetAggrDBType,
        qlnfMaxGetAggrExecInfo,
        qlnfInvalidGetAggrDistExecInfo,
        qlnfMaxInit,
        qlnfMaxBuild
    }
};


/************************************************
 * Get Aggregation Info
 ***********************************************/

/**
 * @brief Aggregation Function에 대한 Execution 정보를 구축한다.
 * @param[in,out]  aSQLStmt        SQL Statement
 * @param[in]      aAggrFuncCnt    Aggregation Optimization Info 개수
 * @param[in]      aAggrOptInfo    Aggregation Optimization Info
 * @param[in]      aDataArea       Data Area (Data Optimization 결과물)
 * @param[out]     aAggrExecInfo   Aggregation Execution Info
 * @param[in]      aRegionMem      Region Memory
 * @param[in]      aEnv            Environment
 */
stlStatus qlnfGetAggrFuncExecInfo( qllStatement          * aSQLStmt,
                                   stlInt32                aAggrFuncCnt,
                                   qlnfAggrOptInfo       * aAggrOptInfo,
                                   qllDataArea           * aDataArea,
                                   qlnfAggrExecInfo     ** aAggrExecInfo,
                                   knlRegionMem          * aRegionMem,
                                   qllEnv                * aEnv )
{
    qlnfAggrOptInfo     * sAggrOptInfo  = NULL;
    qlnfAggrExecInfo    * sAggrExecInfo = NULL;
    stlInt32              sLoop         = 0;

    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrFuncCnt > 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAggrOptInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Aggregation에 대한 Execution 정보 공간 할당
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlnfAggrExecInfo ) * aAggrFuncCnt,
                                (void **) & sAggrExecInfo,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sAggrExecInfo, 0x00, STL_SIZEOF( qlnfAggrExecInfo ) * aAggrFuncCnt );

    
    /**
     * 각 Aggregation에 대한 Execution 정보 구축
     */

    sAggrOptInfo = aAggrOptInfo;

    for( sLoop = 0 ; sLoop < aAggrFuncCnt ; sLoop++ )
    {
        STL_TRY( gQlnfAggrFuncInfoArr[sAggrOptInfo->mAggrFuncId].mGetExecInfoFuncPtr(
                     aSQLStmt,
                     sAggrOptInfo,
                     aDataArea,
                     & sAggrExecInfo[ sLoop ],
                     aRegionMem,
                     aEnv )
                 == STL_SUCCESS );

        sAggrOptInfo++;
    }
    

    /**
     * OUTPUT 설정
     */

    *aAggrExecInfo = sAggrExecInfo;
    

    return STL_SUCCESS;
                                 
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Aggregation Distinct Function에 대한 Execution 정보를 구축한다.
 * @param[in,out]  aSQLStmt        SQL Statement
 * @param[in]      aAggrFuncCnt    Aggregation Optimization Info 개수
 * @param[in]      aAggrOptInfo    Aggregation Optimization Info
 * @param[in]      aDataArea       Data Area (Data Optimization 결과물)
 * @param[out]     aAggrExecInfo   Aggregation Execution Info
 * @param[in]      aRegionMem      Region Memory
 * @param[in]      aEnv            Environment
 */
stlStatus qlnfGetAggrDistFuncExecInfo( qllStatement           * aSQLStmt,
                                       stlInt32                 aAggrFuncCnt,
                                       qlnfAggrOptInfo        * aAggrOptInfo,
                                       qllDataArea            * aDataArea,
                                       qlnfAggrDistExecInfo  ** aAggrExecInfo,
                                       knlRegionMem           * aRegionMem,
                                       qllEnv                 * aEnv )
{
    qlnfAggrOptInfo       * sAggrOptInfo  = NULL;
    qlnfAggrDistExecInfo  * sAggrExecInfo = NULL;
    stlInt32                sLoop         = 0;

    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrFuncCnt > 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAggrOptInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Aggregation에 대한 Execution 정보 공간 할당
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlnfAggrDistExecInfo ) * aAggrFuncCnt,
                                (void **) & sAggrExecInfo,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sAggrExecInfo, 0x00, STL_SIZEOF( qlnfAggrDistExecInfo ) * aAggrFuncCnt );

    
    /**
     * 각 Aggregation에 대한 Execution 정보 구축
     */

    sAggrOptInfo = aAggrOptInfo;

    for( sLoop = 0 ; sLoop < aAggrFuncCnt ; sLoop++ )
    {
        STL_TRY( gQlnfAggrFuncInfoArr[sAggrOptInfo->mAggrFuncId].mGetDistExecInfoFuncPtr(
                     aSQLStmt,
                     sAggrOptInfo,
                     aDataArea,
                     & sAggrExecInfo[ sLoop ],
                     aRegionMem,
                     aEnv )
                 == STL_SUCCESS );

        sAggrOptInfo++;
    }
    

    /**
     * OUTPUT 설정
     */

    *aAggrExecInfo = sAggrExecInfo;
    

    return STL_SUCCESS;
                                 
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Aggregation Function에 대한 Fetch Aggregation Info 정보를 구축한다.
 * @param[in,out]  aSQLStmt        SQL Statement
 * @param[in]      aAggrFuncCnt    Aggregation Optimization Info 개수
 * @param[in]      aAggrOptInfo    Aggregation Optimization Info
 * @param[in]      aDataArea       Data Area (Data Optimization 결과물)
 * @param[out]     aAggrFetchInfo  Aggregation Fetch Info (for SM)
 * @param[in]      aRegionMem      Region Memory
 * @param[in]      aEnv            Environment
 */
stlStatus qlnfGetAggrFuncAggrFetchInfo( qllStatement           * aSQLStmt,
                                        stlInt32                 aAggrFuncCnt,
                                        qlnfAggrOptInfo        * aAggrOptInfo,
                                        qllDataArea            * aDataArea,
                                        smlAggrFuncInfo       ** aAggrFetchInfo,
                                        knlRegionMem           * aRegionMem,
                                        qllEnv                 * aEnv )
{
    smlAggrFuncInfo     * sAggrFetchInfo  = NULL;
    qlvInitAggregation  * sAggrExpr       = NULL;

    knlValueBlockList   * sLastColBlock   = NULL;
    knlValueBlockList   * sNewBlock       = NULL;
    knlExprEvalInfo     * sEvalInfo       = NULL;
    knlExprContext      * sContexts       = NULL;
    knlBuiltInAggrFunc    sAggrFuncId     = KNL_BUILTIN_AGGREGATION_INVALID;
    dtlDataType           sDataType       = DTL_TYPE_NA;
    
    stlInt32              sLoop           = 0;
    stlInt32              sIdx            = 0;
    stlInt32              sOffset         = 0;
    stlBool               sIsExist        = STL_FALSE;
    stlBool               sIsListFunc     = STL_FALSE;


    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAggrFuncCnt > 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAggrOptInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAggrFetchInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**
     * Aggregation 정보 구성 (Distinct 조건 포함되지 않음)
     */

    /* Aggregation Fetch Info 공간 할당 */
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( smlAggrFuncInfo ) * aAggrFuncCnt,
                                (void **) & sAggrFetchInfo,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    *aAggrFetchInfo = sAggrFetchInfo;

    stlMemset( sAggrFetchInfo,
               0x00,
               STL_SIZEOF( smlAggrFuncInfo ) * aAggrFuncCnt );
            
    for( sLoop = 0 ; sLoop < aAggrFuncCnt ; sLoop++ )
    {
        sAggrFetchInfo[ sLoop ].mNext = & sAggrFetchInfo[ sLoop + 1 ];
    }
    sAggrFetchInfo[ sLoop - 1 ].mNext = NULL;

    
    /* Aggregation Fetch Info 설정 */
    sContexts = aDataArea->mExprDataContext->mContexts;
    
    for( sLoop = 0 ; sLoop < aAggrFuncCnt ; sLoop++ )
    {
        STL_DASSERT( aAggrOptInfo[ sLoop ].mAggrExpr->mType == QLV_EXPR_TYPE_AGGREGATION );
        
        sAggrExpr   = aAggrOptInfo[ sLoop ].mAggrExpr->mExpr.mAggregation;
        sAggrFuncId = sAggrExpr->mAggrId;

        
        /**
         * Input Argument Value Block List
         */

        if( sAggrExpr->mArgCount > 0 )
        {
            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF( knlExprEvalInfo * ) * sAggrExpr->mArgCount,
                                        (void **) & sAggrFetchInfo->mEvalInfo,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF( knlExprEvalInfo ) * sAggrExpr->mArgCount,
                                        (void **) & sEvalInfo,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
        else
        {
            /* Do Nothing */
        }

        sLastColBlock = NULL;
        for( sIdx = 0 ; sIdx < sAggrExpr->mArgCount ; sIdx++ )
        {
            switch( sAggrExpr->mArgs[ sIdx ]->mType )
            {
                case QLV_EXPR_TYPE_FUNCTION :
                case QLV_EXPR_TYPE_CAST :
                case QLV_EXPR_TYPE_CASE_EXPR :
                    {
                        sIsExist    = STL_FALSE;
                        sIsListFunc = STL_FALSE;
                        break;
                    }
                case QLV_EXPR_TYPE_LIST_FUNCTION:
                case QLV_EXPR_TYPE_LIST_COLUMN:
                    {
                        sIsExist    = STL_FALSE;
                        sIsListFunc = STL_TRUE;
                        break;
                    }
                default :
                    {
                        sIsExist = STL_TRUE;
                        break;
                    }
            }

            sOffset = sAggrExpr->mArgs[ sIdx ]->mOffset;

            if( ( sIsExist == STL_TRUE ) &&
                ( sContexts[ sOffset ].mCast == NULL ) )
            {
                /**
                 * 연관된 Value Block이 있고, Internal Cast가 없으면 Expression에 대한 Value Block을 연결
                 */
        
                STL_TRY( knlInitShareBlockFromBlock(
                             & sNewBlock,
                             sContexts[ sOffset ].mInfo.mValueInfo,
                             aRegionMem,
                             KNL_ENV( aEnv ) )
                         == STL_SUCCESS );
            }
            else
            {
                /**
                 * 새로운 Value Block 할당
                 */
                if( sContexts[ sOffset ].mCast != NULL )
                {
                    sDataType = sContexts[ sOffset ].mCast->mCastResultBuf.mType;
                }
                else
                {
                    if( sIsListFunc == STL_FALSE )
                    {
                        sDataType = sContexts[ sOffset ].mInfo.mFuncData->mResultValue.mType;
                    }
                    else
                    {
                        sDataType = sContexts[ sOffset ].mInfo.mListFunc->mResultValue.mType;
                    }
                }

                STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                            & sNewBlock,
                                            aDataArea->mBlockAllocCnt,
                                            STL_TRUE,
                                            STL_LAYER_SQL_PROCESSOR,
                                            0,   /* table ID */
                                            0,   /* column ID */
                                            sDataType,
                                            gResultDataTypeDef[ sDataType ].mArgNum1,
                                            gResultDataTypeDef[ sDataType ].mArgNum2,
                                            gResultDataTypeDef[ sDataType ].mStringLengthUnit,
                                            gResultDataTypeDef[ sDataType ].mIntervalIndicator,
                                            aRegionMem,
                                            aEnv )
                         == STL_SUCCESS );
            }

            if( sLastColBlock == NULL )
            {
                sAggrFetchInfo->mInputArgList = sNewBlock;
            }
            else
            {
                /* [coverage]
                 * 현재 aggregation function 중에 argument가 두개 이상인게 없으므로
                 * 이 코드가 실행되는 경우는 없음. 표준 편차 함수가 생기면 이 주석은 제거할 것
                 */
                KNL_LINK_BLOCK_LIST( sLastColBlock, sNewBlock );
            }
            sLastColBlock = sNewBlock;

            
            /**
             * Fetch Info 설정
             */
            
            /* expression stack */
            sAggrFetchInfo->mInputArgStack = aAggrOptInfo[ sLoop ].mArgCodeStack[ sIdx ];

            if( sAggrFetchInfo->mInputArgStack == NULL )
            {
                sAggrFetchInfo->mEvalInfo[ sIdx ] = NULL;
            }
            else
            {
                /* evaluate info */
                sEvalInfo->mExprStack   = sAggrFetchInfo->mInputArgStack;
                sEvalInfo->mContext     = aDataArea->mExprDataContext;
                sEvalInfo->mResultBlock = sAggrFetchInfo->mInputArgList;
                sEvalInfo->mBlockIdx    = 0;
                sEvalInfo->mBlockCount  = 0;

                sAggrFetchInfo->mEvalInfo[ sIdx ] = sEvalInfo;
            }
            sEvalInfo++;
        }


        /**
         * Aggregation Value Block List 생성
         */

        STL_TRY( knlInitShareBlockFromBlock(
                     & sAggrFetchInfo->mAggrValueList,
                     sContexts[ aAggrOptInfo[ sLoop ].mAggrExpr->mOffset ].mInfo.mValueInfo,
                     aRegionMem,
                     KNL_ENV( aEnv ) )
                 == STL_SUCCESS );

        
        /**
         * Fetch Info 설정
         */

        sAggrFetchInfo->mAggrFuncModule = &gSmlAggrFuncInfoArr[sAggrFuncId];
        sAggrFetchInfo->mAggrMergeFunc = gSmlAggrMergeFuncInfoArr[sAggrFuncId];
        
        sAggrFetchInfo++;
    }


    return STL_SUCCESS;
                                 
    STL_FINISH;

    return STL_FAILURE;
}


/************************************************
 * Aggregation Expression 
 ***********************************************/

/**
 * @brief Aggregation을 Distinct와 Non-Distinct로 구분한다.
 * @param[in]      aSrcAggrFuncList   Source Aggregation Function Expression List
 * @param[out]     aAggrFuncList      Aggregation Function Expression List
 * @param[out]     aAggrDistFuncList  Aggregation Distinct Function Expression List
 * @param[in]      aRegionMem         Region Memory
 * @param[in]      aEnv               Environment
 */
stlStatus qlnfClassifyAggrFunc( qlvRefExprList   * aSrcAggrFuncList,
                                qlvRefExprList  ** aAggrFuncList,
                                qlvRefExprList  ** aAggrDistFuncList,
                                knlRegionMem     * aRegionMem,
                                qllEnv           * aEnv )
{
    qlvInitExpression  * sExpr             = NULL;
    qlvRefExprItem     * sRefExprItem      = NULL;
    qlvRefExprList     * sAggrFuncList     = NULL;
    qlvRefExprList     * sAggrDistFuncList = NULL;

    
    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aSrcAggrFuncList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAggrFuncList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAggrDistFuncList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );

    
    if( aSrcAggrFuncList->mCount > 0 )
    {
        /**
         * Aggregation Function Expression List 공간 할당
         */
    
        STL_TRY( qlvCreateRefExprList( & sAggrFuncList,
                                       aRegionMem,
                                       aEnv )
                 == STL_SUCCESS );
    

        /**
         * Aggregation Distinct Function Expression List 공간 할당
         */
    
        STL_TRY( qlvCreateRefExprList( & sAggrDistFuncList,
                                       aRegionMem,
                                       aEnv )
                 == STL_SUCCESS );

    
        /**
         * Aggregation Function Expression List 구성
         */

        sRefExprItem = aSrcAggrFuncList->mHead;

        while( sRefExprItem != NULL )
        {
            STL_DASSERT( sRefExprItem->mExprPtr->mType == QLV_EXPR_TYPE_AGGREGATION );
            sExpr = sRefExprItem->mExprPtr;

            if( sExpr->mExpr.mAggregation->mIsDistinct == STL_FALSE )
            {
                /**
                 * Aggregation Function Expression List에 Expression 추가
                 */

                STL_TRY( qlvAddExprToRefExprList( sAggrFuncList,
                                                  sExpr,
                                                  STL_FALSE,
                                                  aRegionMem,
                                                  aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                /**
                 * Aggregation Distinct Function Expression List에 Expression 추가
                 */

                STL_TRY( qlvAddExprToRefExprList( sAggrDistFuncList,
                                                  sExpr,
                                                  STL_FALSE,
                                                  aRegionMem,
                                                  aEnv )
                         == STL_SUCCESS );
            }
        
            sRefExprItem = sRefExprItem->mNext;
        }
    }
    else
    {
        /* Do Nothing */
    }

    /**
     * OUTPUT 설정
     */

    *aAggrFuncList     = sAggrFuncList;
    *aAggrDistFuncList = sAggrDistFuncList;
    

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Aggregation을 모두 Non-Distinct Aggregation으로 변경한다.
 * @param[in]      aSrcAggrFuncList   Source Aggregation Function Expression List
 * @param[out]     aAggrFuncList      Aggregation Function Expression List
 * @param[in]      aRegionMem         Region Memory
 * @param[in]      aEnv               Environment
 */
stlStatus qlnfSetNonDistAggrFunc( qlvRefExprList   * aSrcAggrFuncList,
                                  qlvRefExprList  ** aAggrFuncList,
                                  knlRegionMem     * aRegionMem,
                                  qllEnv           * aEnv )
{
    qlvInitExpression  * sExpr         = NULL;
    qlvRefExprItem     * sRefExprItem  = NULL;
    qlvRefExprList     * sAggrFuncList = NULL;

    
    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aSrcAggrFuncList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAggrFuncList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Aggregation Function Expression List 공간 할당
     */
    
    STL_TRY( qlvCreateRefExprList( & sAggrFuncList,
                                   aRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    
    /**
     * Aggregation Function Expression List 구성
     */

    STL_DASSERT( aSrcAggrFuncList->mCount > 0 );

    sRefExprItem = aSrcAggrFuncList->mHead;

    while( sRefExprItem != NULL )
    {
        STL_DASSERT( sRefExprItem->mExprPtr->mType == QLV_EXPR_TYPE_AGGREGATION );
        sExpr = sRefExprItem->mExprPtr;

        /**
         * Non-Distinct로 Aggregation 정보 변경
         */

        sExpr->mExpr.mAggregation->mIsDistinct = STL_FALSE;

        STL_TRY( qlvAddExprToRefExprList( sAggrFuncList,
                                          sExpr,
                                          STL_FALSE,
                                          aRegionMem,
                                          aEnv )
                 == STL_SUCCESS );
        
        sRefExprItem = sRefExprItem->mNext;
    }


    /**
     * OUTPUT 설정
     */

    *aAggrFuncList = sAggrFuncList;
    

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Aggregation Function에 대한 Optimization 정보를 구축한다.
 * @param[in]      aSQLStmt        SQL Statement 객체
 * @param[in,out]  aQueryExprList  Query 단위 Expression 정보
 * @param[in]      aAggrFuncCnt    Aggregation Function 개수
 * @param[in]      aAggrFuncList   Aggregation Function Expression List
 * @param[out]     aAggrOptInfo    Aggregation Optimization Info
 * @param[in]      aRegionMem      Region Memory
 * @param[in]      aEnv            Environment
 */
stlStatus qlnfGetAggrOptInfo( qllStatement         * aSQLStmt,
                              qloInitExprList      * aQueryExprList,
                              stlInt32               aAggrFuncCnt,
                              qlvRefExprList       * aAggrFuncList,
                              qlnfAggrOptInfo     ** aAggrOptInfo,
                              knlRegionMem         * aRegionMem,
                              qllEnv               * aEnv )
{
    qlvRefExprItem      * sRefExprItem     = NULL;

    qlnfAggrOptInfo     * sCurAggrOptInfo  = NULL;
    qlnfAggrOptInfo     * sAggrOptInfo     = NULL;

    qlvInitExpression   * sExpr            = NULL;
    qlvInitAggregation  * sIniAggrExpr     = NULL;
    knlExprStack        * sCodeStack       = NULL;
    knlExprStack        * sAggrStack       = NULL;
    knlExprEntry        * sExprEntry       = NULL;
    
    stlInt32              sLoop            = 0;
    stlInt32              sArgCnt          = 0;
    dtlDataType           sDBType          = DTL_TYPE_NA;
    dtlDataType           sArgDBType       = DTL_TYPE_NA;

    
    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aAggrFuncCnt > 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAggrFuncList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAggrOptInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Aggregation Optimization Info 공간 할당
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlnfAggrOptInfo ) * aAggrFuncCnt,
                                (void **) & sAggrOptInfo,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
        
    stlMemset( sAggrOptInfo,
               0x00,
               STL_SIZEOF( qlnfAggrOptInfo ) * aAggrFuncCnt );


    /**
     * 각 Aggregation Function 마다 Optimization Info 정보 설정
     */

    sRefExprItem    = aAggrFuncList->mHead;
    sCurAggrOptInfo = sAggrOptInfo;

    for( sLoop = 0 ; sLoop < aAggrFuncCnt ; sLoop++ )
    {
        /**
         * 정보 설정
         */

        STL_DASSERT( sRefExprItem->mExprPtr->mType == QLV_EXPR_TYPE_AGGREGATION );

        sIniAggrExpr = sRefExprItem->mExprPtr->mExpr.mAggregation;
            
        sCurAggrOptInfo->mAggrExpr      = sRefExprItem->mExprPtr;
        sCurAggrOptInfo->mAggrFuncId    = sIniAggrExpr->mAggrId;


        /**
         * Aggregation Argument Expression Stack 설정
         */
            
        if( sIniAggrExpr->mArgCount > 0 )
        {
            /**
             * Argument Expression Stack 공간 할당
             */
        
            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF( knlExprStack * ) * sIniAggrExpr->mArgCount,
                                        (void **) & sCurAggrOptInfo->mArgCodeStack,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF( knlExprStack ) * sIniAggrExpr->mArgCount,
                                        (void **) & sAggrStack,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

                
            /**
             * Aggregation Argument와 Filter에 대한 Expression Stack 구성
             */

            for( sArgCnt = 0 ; sArgCnt < sIniAggrExpr->mArgCount ; sArgCnt++ )
            {
                sExpr = sIniAggrExpr->mArgs[ sArgCnt ];
                

                /**
                 * Argument Expression Stack 생성
                 */

                sCodeStack = & sAggrStack[ sArgCnt ];

                STL_TRY( knlExprInit( sCodeStack,
                                      sExpr->mIncludeExprCnt,
                                      aRegionMem,
                                      KNL_ENV( aEnv ) )
                         == STL_SUCCESS );

        
                /**
                 * Argument Expression Stack 구성
                 */
            
                STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                                    sExpr,
                                                    aSQLStmt->mBindContext,
                                                    sCodeStack,
                                                    aQueryExprList->mStmtExprList->mConstExprStack,
                                                    aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                    & sExprEntry,
                                                    aRegionMem,
                                                    aEnv )
                         == STL_SUCCESS );


                switch( sIniAggrExpr->mAggrId )
                {
                    case KNL_BUILTIN_AGGREGATION_SUM :
                        {
                            sArgDBType = aQueryExprList->mStmtExprList->mTotalExprInfo
                                ->mExprDBType[ sExpr->mOffset ].mDBType;

                            if( sIniAggrExpr->mIsDistinct == STL_TRUE )
                            {
                                STL_TRY_THROW( dtlCheckKeyCompareType( sArgDBType ) == STL_TRUE,
                                               RAMP_ERR_INVALID_KEY_COMP_TYPE );
                            }
                            else
                            {
                                if( ( dtlDataTypeGroup[ sArgDBType ] == DTL_GROUP_CHAR_STRING ) ||
                                    ( dtlDataTypeGroup[ sArgDBType ] == DTL_GROUP_LONGVARCHAR_STRING ) )
                                {
                                    STL_TRY( qloCastExprDBType( sExpr,
                                                                aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                                DTL_TYPE_NUMBER,
                                                                DTL_NUMERIC_DEFAULT_PRECISION,
                                                                DTL_SCALE_NA,
                                                                DTL_STRING_LENGTH_UNIT_NA,
                                                                DTL_INTERVAL_INDICATOR_NA,
                                                                STL_TRUE,
                                                                STL_FALSE,
                                                                aEnv )
                                             == STL_SUCCESS );
                                }
                                else
                                {
                                    STL_TRY_THROW( dtlCheckKeyCompareType( sArgDBType ) == STL_TRUE,
                                                   RAMP_ERR_INVALID_KEY_COMP_TYPE );
                                }
                            }
                                
                            break;
                        }
                    case KNL_BUILTIN_AGGREGATION_COUNT :
                        {                            
                            if( sIniAggrExpr->mIsDistinct == STL_TRUE )
                            {
                                sArgDBType = aQueryExprList->mStmtExprList->mTotalExprInfo
                                    ->mExprDBType[ sExpr->mOffset ].mDBType;

                                STL_TRY_THROW( dtlCheckKeyCompareType( sArgDBType ) == STL_TRUE,
                                               RAMP_ERR_INVALID_KEY_COMP_TYPE );
                            }
                            break;
                        }
                    case KNL_BUILTIN_AGGREGATION_MIN :
                    case KNL_BUILTIN_AGGREGATION_MAX :
                        {
                            sArgDBType = aQueryExprList->mStmtExprList->mTotalExprInfo
                                ->mExprDBType[ sExpr->mOffset ].mDBType;

                            STL_TRY_THROW( dtlCheckKeyCompareType( sArgDBType ) == STL_TRUE,
                                           RAMP_ERR_INVALID_KEY_COMP_TYPE );
                            break;
                        }
                        /* case KNL_BUILTIN_AGGREGATION_COUNT_ASTERISK : */
                    default :
                        {
                            break;
                        }
                }

                    
                /**
                 * Argument Expression Stack 설정
                 */

                if( sIniAggrExpr->mIsDistinct == STL_TRUE )
                {
                    sCurAggrOptInfo->mArgCodeStack[ sArgCnt ] = sCodeStack;
                }
                else
                {
                    if( ( sCodeStack->mEntries[1].mExprType == KNL_EXPR_TYPE_VALUE ) &&
                        ( sCodeStack->mEntryCount == 1 ) &&
                        ( aQueryExprList->mStmtExprList->mTotalExprInfo->mExprDBType
                          [ sCodeStack->mEntries[1].mExpr.mValueInfo.mOffset ].mIsNeedCast == STL_FALSE ) )
                    {
                        /**
                         * @remark Stack에 포함된 value block과 output value block은 반드시 공유되어 있어야 한다.
                         */

                        sCurAggrOptInfo->mArgCodeStack[ sArgCnt ] = NULL;
                    }
                    else
                    {
                        sCurAggrOptInfo->mArgCodeStack[ sArgCnt ] = sCodeStack;
                    }
                }
            }

            sDBType = aQueryExprList->mStmtExprList->mTotalExprInfo->mExprDBType[ sIniAggrExpr->mArgs[0]->mOffset ].mDBType;
        }
        else
        {
            sCurAggrOptInfo->mArgCodeStack = NULL;
        }
            

        /**
         * Aggregation에 대한 DB type 설정
         */

        STL_TRY( gQlnfAggrFuncInfoArr[sCurAggrOptInfo->mAggrFuncId].mSetDBTypeFuncPtr(
                     sRefExprItem->mExprPtr,
                     sDBType,
                     aQueryExprList->mStmtExprList->mTotalExprInfo,
                     aEnv )
                 == STL_SUCCESS );

            
        /**
         * Filter Expression Stack 설정
         */

        if( sIniAggrExpr->mFilter != NULL )
        {
            /* [coverage]
             * aggregation filter expression은 아직 구현 안돼있음
             */

            /**
             * Filter Expression Stack 공간 할당
             */
            
            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF( knlExprStack ),
                                        (void **) & sCurAggrOptInfo->mFilterCodeStack,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            

            /**
             * Filter Expression Stack 생성
             */

            STL_TRY( knlExprInit( sCurAggrOptInfo->mFilterCodeStack,
                                  sIniAggrExpr->mFilter->mIncludeExprCnt,
                                  aRegionMem,
                                  KNL_ENV( aEnv ) )
                     == STL_SUCCESS );
            
    
            /**
             * Filter Expression Stack 구성
             */

            STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                                sIniAggrExpr->mFilter,
                                                aSQLStmt->mBindContext,
                                                sCurAggrOptInfo->mFilterCodeStack,
                                                aQueryExprList->mStmtExprList->mConstExprStack,
                                                aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                & sExprEntry,
                                                aRegionMem,
                                                aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            sCurAggrOptInfo->mFilterCodeStack = NULL;
        }
            
        sRefExprItem = sRefExprItem->mNext;
        sCurAggrOptInfo++;
    }

    
    /**
     * OUTPUT 설정
     */

    *aAggrOptInfo = sAggrOptInfo;
    
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_KEY_COMP_TYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_ILLEGAL_USE_OF_DATA_TYPE,
                      qlgMakeErrPosString( aSQLStmt->mRetrySQL,
                                           sExpr->mPosition,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      dtlDataTypeName[ sArgDBType ] );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlnf */
