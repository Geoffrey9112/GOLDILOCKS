/*******************************************************************************
 * qlnfAggregation.c
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
 * @file qlnfAggregation.c
 * @brief SQL Processor Layer Aggregation Function Node
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnfAggr
 * @{
 */


/************************************************
 * Invalid Function
 ***********************************************/

/**
 * @brief Aggregation 내의 모든 Expression에 대한 DB type을 설정한다.
 * @param[in]      aInitExpr       Aggregation Expression
 * @param[in]      aDBType         Aggregation에 설정해 줄 DB type
 * @param[in,out]  aExprInfo       All Expression's Information
 * @param[in]      aEnv            Environment
 */
stlStatus qlnfInvalidSetAggrDBType( qlvInitExpression  * aInitExpr,
                                    dtlDataType          aDBType,
                                    qloExprInfo        * aExprInfo,
                                    qllEnv             * aEnv )
{
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aExprInfo != NULL, QLL_ERROR_STACK( aEnv ) );
    

    /**
     * Output 설정
     */

    STL_DASSERT( 0 );

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Aggregation Execution 정보를 얻는다.
 * @param[in,out]  aSQLStmt        SQL Statement
 * @param[in]      aAggrOptInfo    Aggregation Optimization Info
 * @param[in]      aDataArea       Data Area (Data Optimization 결과물)
 * @param[out]     aAggrExecInfo   Aggregation Execution Info
 * @param[in]      aRegionMem      Region Memory
 * @param[in]      aEnv            Environment
 */
stlStatus qlnfInvalidGetAggrExecInfo( qllStatement          * aSQLStmt,
                                      qlnfAggrOptInfo       * aAggrOptInfo,
                                      qllDataArea           * aDataArea,
                                      qlnfAggrExecInfo      * aAggrExecInfo,
                                      knlRegionMem          * aRegionMem,
                                      qllEnv                * aEnv )
{
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrOptInfo != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK( aEnv ) );
    

    /**
     * Output 설정
     */

    STL_DASSERT( 0 );

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Aggregation Distinct Execution 정보를 얻는다.
 * @param[in,out]  aSQLStmt        SQL Statement
 * @param[in]      aAggrOptInfo    Aggregation Optimization Info
 * @param[in]      aDataArea       Data Area (Data Optimization 결과물)
 * @param[out]     aAggrExecInfo   Aggregation Execution Info
 * @param[in]      aRegionMem      Region Memory
 * @param[in]      aEnv            Environment
 */
stlStatus qlnfInvalidGetAggrDistExecInfo( qllStatement          * aSQLStmt,
                                          qlnfAggrOptInfo       * aAggrOptInfo,
                                          qllDataArea           * aDataArea,
                                          qlnfAggrDistExecInfo  * aAggrExecInfo,
                                          knlRegionMem          * aRegionMem,
                                          qllEnv                * aEnv )
{
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrOptInfo != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK( aEnv ) );
    

    /**
     * Output 설정
     */

    STL_DASSERT( 0 );

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Aggregation을 초기화한다.
 * @param[in]      aAggrExecInfo   Aggregation Execution 정보
 * @param[in]      aBlockIdx       Block Idx
 * @param[in]      aEnv            Environment
 */
stlStatus qlnfInvalidInit( qlnfAggrExecInfo   * aAggrExecInfo,
                           stlInt32             aBlockIdx,
                           qllEnv             * aEnv )
{
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aBlockIdx >= 0, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, QLL_ERROR_STACK( aEnv ) );
    

    /**
     * Output 설정
     */

    STL_DASSERT( 0 );
    
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Aggregation을 수행한다.
 * @param[in]      aAggrExecInfo   Aggregation Execution 정보
 * @param[in]      aStartIdx       Start Block Idx
 * @param[in]      aBlockCnt       Block Used Count
 * @param[in]      aResultBlockIdx Result Block Idx
 * @param[in]      aEnv            Environment
 */
stlStatus qlnfInvalidBuild( qlnfAggrExecInfo   * aAggrExecInfo,
                            stlInt32             aStartIdx,
                            stlInt32             aBlockCnt,
                            stlInt32             aResultBlockIdx,
                            qllEnv             * aEnv )
{
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aStartIdx >= 0, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aBlockCnt > 0, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aResultBlockIdx >= 0, QLL_ERROR_STACK( aEnv ) );


    /**
     * Output 설정
     */

    STL_DASSERT( 0 );
    
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/************************************************
 * SUM 
 ***********************************************/

/**
 * @brief Aggregation 내의 모든 Expression에 대한 DB type을 설정한다.
 * @param[in]      aInitExpr       Init Expression
 * @param[in]      aDBType         Aggregation에 설정해 줄 DB type
 * @param[in,out]  aExprInfo       All Expression's Information
 * @param[in]      aEnv            Environment
 */
stlStatus qlnfSumSetAggrDBType( qlvInitExpression  * aInitExpr,
                                dtlDataType          aDBType,
                                qloExprInfo        * aExprInfo,
                                qllEnv             * aEnv )
{
//    qlvInitAggregation  * sAggrExpr = NULL;
    
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aInitExpr->mType == QLV_EXPR_TYPE_AGGREGATION,
                        QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aExprInfo != NULL, QLL_ERROR_STACK( aEnv ) );


    /**
     * Input Expression에 대한 DB type 설정
     */

//    sAggrExpr = aInitExpr->mExpr.mAggregation;

//    STL_DASSERT( sAggrExpr->mAggrId == KNL_BUILTIN_AGGREGATION_SUM );
//    STL_DASSERT( sAggrExpr->mArgCount == 1 ); 
//    STL_DASSERT( sAggrExpr->mArgs != NULL );
    STL_DASSERT( aInitExpr->mExpr.mAggregation->mAggrId == KNL_BUILTIN_AGGREGATION_SUM );
    STL_DASSERT( aInitExpr->mExpr.mAggregation->mArgCount == 1 ); 
    STL_DASSERT( aInitExpr->mExpr.mAggregation->mArgs != NULL );

    STL_DASSERT( ( aDBType >= DTL_TYPE_BOOLEAN ) &&
                 ( aDBType < DTL_TYPE_MAX ) );
    
    switch( dtlDataTypeGroup[ aDBType ] )
    {
        case DTL_GROUP_BINARY_INTEGER :
            {
                /**
                 * Result Expression에 대한 DB type 설정
                 */

                STL_TRY( qloSetExprDBType( aInitExpr,
                                           aExprInfo,
                                           DTL_TYPE_NATIVE_BIGINT,
                                           DTL_PRECISION_NA,
                                           DTL_SCALE_NA,
                                           DTL_STRING_LENGTH_UNIT_NA,
                                           DTL_INTERVAL_INDICATOR_NA,
                                           aEnv )
                         == STL_SUCCESS );
            }
            break;

        case DTL_GROUP_BINARY_REAL :
            {
                /**
                 * Result Expression에 대한 DB type 설정
                 */

                STL_TRY( qloSetExprDBType( aInitExpr,
                                           aExprInfo,
                                           DTL_TYPE_NATIVE_DOUBLE,
                                           DTL_PRECISION_NA,
                                           DTL_SCALE_NA,
                                           DTL_STRING_LENGTH_UNIT_NA,
                                           DTL_INTERVAL_INDICATOR_NA,
                                           aEnv )
                         == STL_SUCCESS );
            }
            break;

        case DTL_GROUP_CHAR_STRING :
        case DTL_GROUP_LONGVARCHAR_STRING :
        case DTL_GROUP_NUMBER :
            {
                /**
                 * Result Expression에 대한 DB type 설정
                 */

                STL_TRY( qloSetExprDBType( aInitExpr,
                                           aExprInfo,
                                           DTL_TYPE_NUMBER,
                                           DTL_NUMERIC_DEFAULT_PRECISION,
                                           DTL_SCALE_NA,
                                           DTL_STRING_LENGTH_UNIT_NA,
                                           DTL_INTERVAL_INDICATOR_NA,
                                           aEnv )
                         == STL_SUCCESS );

                break;
            }

        case DTL_GROUP_INTERVAL_YEAR_TO_MONTH :     
            {
                /**
                 * Result Expression에 대한 DB type 설정
                 */

                STL_TRY( qloSetExprDBType( aInitExpr,
                                           aExprInfo,
                                           DTL_TYPE_INTERVAL_YEAR_TO_MONTH,
                                           DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION,
                                           DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION,
                                           DTL_STRING_LENGTH_UNIT_NA,
                                           DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH,
                                           aEnv )
                         == STL_SUCCESS );

                break;
            }

        case DTL_GROUP_INTERVAL_DAY_TO_SECOND :
            {
                /**
                 * Result Expression에 대한 DB type 설정
                 */

                STL_TRY( qloSetExprDBType( aInitExpr,
                                           aExprInfo,
                                           DTL_TYPE_INTERVAL_DAY_TO_SECOND,
                                           DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION,
                                           DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION,
                                           DTL_STRING_LENGTH_UNIT_NA,
                                           DTL_INTERVAL_INDICATOR_DAY_TO_SECOND,
                                           aEnv )
                         == STL_SUCCESS );

                break;
            }

        /* case DTL_GROUP_BOOLEAN : */
        /* case DTL_GROUP_BINARY_STRING : */
        /* case DTL_GROUP_LONGVARBINARY_STRING : */
        /* case DTL_GROUP_DATE : */
        /* case DTL_GROUP_TIME : */
        /* case DTL_GROUP_TIMESTAMP : */
        /* case DTL_GROUP_TIME_WITH_TIME_ZONE : */
        /* case DTL_GROUP_TIMESTAMP_WITH_TIME_ZONE : */
        /* case DTL_GROUP_ROWID : */
        default :
            {
                /* validation 과 candicate plan 에서 type 평가를 수행하였기 때문에 들어올 수 없다. */
                STL_DASSERT( 0 );
                break;
            }
    }


    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Aggregation Execution 정보를 얻는다.
 * @param[in,out]  aSQLStmt        SQL Statement
 * @param[in]      aAggrOptInfo    Aggregation Optimization Info
 * @param[in]      aDataArea       Data Area (Data Optimization 결과물)
 * @param[out]     aAggrExecInfo   Aggregation Execution Info
 * @param[in]      aRegionMem      Region Memory
 * @param[in]      aEnv            Environment
 */
stlStatus qlnfSumGetAggrExecInfo( qllStatement          * aSQLStmt,
                                  qlnfAggrOptInfo       * aAggrOptInfo,
                                  qllDataArea           * aDataArea,
                                  qlnfAggrExecInfo      * aAggrExecInfo,
                                  knlRegionMem          * aRegionMem,
                                  qllEnv                * aEnv )
{
    qlvInitAggregation    * sAggrExpr  = NULL;
    knlExprContext        * sContexts  = NULL;
    knlExprEvalInfo       * sEvalInfo  = NULL;
    stlInt32                sOffset    = 0;
    dtlDataType             sDataType  = DTL_TYPE_NA;

    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrOptInfo != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK( aEnv ) );

    
    /**
     * Input Argument Count 설정
     */

    sAggrExpr = aAggrOptInfo->mAggrExpr->mExpr.mAggregation;
    sContexts = aDataArea->mExprDataContext->mContexts;

    STL_DASSERT( sAggrExpr->mArgCount == 1 );    
    aAggrExecInfo->mInputArgCnt = sAggrExpr->mArgCount;

    
    /**
     * Input Value Block List 찾기
     */

    /* value block 간의 연결 */
    sOffset = sAggrExpr->mArgs[ 0 ]->mOffset;

    if( sContexts[ sOffset ].mCast != NULL )
    {
        /**
         * 연관된 Value Block이 있고, Internal Cast가 없으면 Expression에 대한 Value Block을 연결
         */
        sDataType = sContexts[ sOffset ].mCast->mCastResultBuf.mType;
        sDataType = dtlGroupRepresentType[ dtlDataTypeGroup[ sDataType ] ];

        STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                    & aAggrExecInfo->mInputArgList,
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
    else
    {
        switch( sAggrExpr->mArgs[ 0 ]->mType )
        {
            case QLV_EXPR_TYPE_VALUE :
            case QLV_EXPR_TYPE_BIND_PARAM :
            case QLV_EXPR_TYPE_PSEUDO_COLUMN :
            case QLV_EXPR_TYPE_COLUMN :
            case QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT :
            case QLV_EXPR_TYPE_REFERENCE :
            case QLV_EXPR_TYPE_INNER_COLUMN :
            case QLV_EXPR_TYPE_OUTER_COLUMN :
            case QLV_EXPR_TYPE_ROWID_COLUMN :
            case QLV_EXPR_TYPE_AGGREGATION :
            case QLV_EXPR_TYPE_ROW_EXPR:
                {
                    /**
                     * 연관된 Value Block이 있고, Internal Cast가 없으면 Expression에 대한 Value Block을 연결
                     */
        
                    STL_TRY( knlInitShareBlockFromBlock(
                                 & aAggrExecInfo->mInputArgList,
                                 sContexts[ sOffset ].mInfo.mValueInfo,
                                 aRegionMem,
                                 KNL_ENV( aEnv ) )
                             == STL_SUCCESS );
                    break;
                }
            case QLV_EXPR_TYPE_FUNCTION :
            case QLV_EXPR_TYPE_CAST :
            case QLV_EXPR_TYPE_CASE_EXPR :
                {
                    /**
                     * 새로운 Value Block 할당
                     */

                    sDataType = sContexts[ sOffset ].mInfo.mFuncData->mResultValue.mType;
                    sDataType = dtlGroupRepresentType[ dtlDataTypeGroup[ sDataType ] ];

                    STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                                & aAggrExecInfo->mInputArgList,
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
                    
                    break;
                }
            case QLV_EXPR_TYPE_LIST_FUNCTION:
            case QLV_EXPR_TYPE_LIST_COLUMN:
            case QLV_EXPR_TYPE_SUB_QUERY :
            case QLV_EXPR_TYPE_PSEUDO_ARGUMENT :
            default :
                {
                    STL_DASSERT( 0 );
                    break;
                }
        }
    }
        

    /**
     * Aggregation Value Block List 찾기
     */

    STL_TRY( knlInitShareBlockFromBlock(
                 & aAggrExecInfo->mAggrValueList,
                 sContexts[ aAggrOptInfo->mAggrExpr->mOffset ].mInfo.mValueInfo,
                 aRegionMem,
                 KNL_ENV( aEnv ) )
             == STL_SUCCESS );


    /**
     * Argument Expression을 평가하기 위한 Block Eval Info 설정
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( knlExprEvalInfo * ),
                                (void **) & aAggrExecInfo->mEvalInfo,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    if( aAggrOptInfo->mArgCodeStack[ 0 ] == NULL )
    {
        aAggrExecInfo->mEvalInfo[ 0 ] = NULL;
    }
    else
    {
        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( knlExprEvalInfo ),
                                    (void **) & sEvalInfo,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemset( sEvalInfo, 0x00, STL_SIZEOF( knlExprEvalInfo ) );

        sEvalInfo->mExprStack       = aAggrOptInfo->mArgCodeStack[ 0 ];
        sEvalInfo->mContext         = aDataArea->mExprDataContext;
        sEvalInfo->mResultBlock     = aAggrExecInfo->mInputArgList;
        sEvalInfo->mBlockIdx        = 0;
        sEvalInfo->mBlockCount      = 0;

        aAggrExecInfo->mEvalInfo[ 0 ] = sEvalInfo;
    }   


    /**
     * Optimization 정보 설정
     */
    
    aAggrExecInfo->mAggrOptInfo = aAggrOptInfo;

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Aggregation Distinct Execution 정보를 얻는다.
 * @param[in,out]  aSQLStmt        SQL Statement
 * @param[in]      aAggrOptInfo    Aggregation Optimization Info
 * @param[in]      aDataArea       Data Area (Data Optimization 결과물)
 * @param[out]     aAggrExecInfo   Aggregation Execution Info
 * @param[in]      aRegionMem      Region Memory
 * @param[in]      aEnv            Environment
 */
stlStatus qlnfSumGetAggrDistExecInfo( qllStatement          * aSQLStmt,
                                      qlnfAggrOptInfo       * aAggrOptInfo,
                                      qllDataArea           * aDataArea,
                                      qlnfAggrDistExecInfo  * aAggrExecInfo,
                                      knlRegionMem          * aRegionMem,
                                      qllEnv                * aEnv )
{
    qlvInitAggregation    * sAggrExpr  = NULL;
    knlExprContext        * sContexts  = NULL;
    knlExprEvalInfo       * sEvalInfo  = NULL;

    smlRowBlock           * sRowBlock  = NULL;
    smlFetchInfo          * sFetchInfo = NULL;
    smlAggrFuncInfo       * sAggrFetch = NULL;
    
    stlInt32                sOffset    = 0;
    dtlDataType             sDataType  = DTL_TYPE_NA;

    
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrOptInfo != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK( aEnv ) );


    /**
     * Input Argument Count 설정
     */

    sAggrExpr = aAggrOptInfo->mAggrExpr->mExpr.mAggregation;
    sContexts = aDataArea->mExprDataContext->mContexts;

    STL_DASSERT( sAggrExpr->mArgCount == 1 );    
    aAggrExecInfo->mInputArgCnt = sAggrExpr->mArgCount;
    
    /* value block 간의 연결 */
    sOffset = sAggrExpr->mArgs[ 0 ]->mOffset;

    /* distinct 처리하기 전에 aggregation에 의한 cast를 선행하지 않는다. */
    STL_DASSERT( sContexts[ sOffset ].mCast == NULL );

    switch( sAggrExpr->mArgs[ 0 ]->mType )
    {
        case QLV_EXPR_TYPE_VALUE :
        case QLV_EXPR_TYPE_BIND_PARAM :
        case QLV_EXPR_TYPE_PSEUDO_COLUMN :
        case QLV_EXPR_TYPE_COLUMN :
        case QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT :
        case QLV_EXPR_TYPE_REFERENCE :
        case QLV_EXPR_TYPE_INNER_COLUMN :
        case QLV_EXPR_TYPE_OUTER_COLUMN :
        case QLV_EXPR_TYPE_ROWID_COLUMN :
        case QLV_EXPR_TYPE_AGGREGATION :
        case QLV_EXPR_TYPE_ROW_EXPR:
            {
                /**
                 * 연관된 Value Block이 있고, Internal Cast가 없으면 Expression에 대한 Value Block을 연결
                 */
        
                STL_TRY( knlInitShareBlockFromBlock(
                             & aAggrExecInfo->mInputArgList,
                             sContexts[ sOffset ].mInfo.mValueInfo,
                             aRegionMem,
                             KNL_ENV( aEnv ) )
                         == STL_SUCCESS );
                break;
            }
        case QLV_EXPR_TYPE_FUNCTION :
        case QLV_EXPR_TYPE_CAST :
        case QLV_EXPR_TYPE_CASE_EXPR :
            {
                /**
                 * 새로운 Value Block 할당
                 */

                sDataType = sContexts[ sOffset ].mInfo.mFuncData->mResultValue.mType;
        
                sDataType = dtlGroupRepresentType[ dtlDataTypeGroup[ sDataType ] ];

                STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                            & aAggrExecInfo->mInputArgList,
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
                break;
            }
        case QLV_EXPR_TYPE_LIST_FUNCTION:
        case QLV_EXPR_TYPE_LIST_COLUMN:
        case QLV_EXPR_TYPE_SUB_QUERY :
        case QLV_EXPR_TYPE_PSEUDO_ARGUMENT :
        default :
            {
                STL_DASSERT( 0 );
                break;
            }
    }
        

    /**
     * Aggregation Value Block List 찾기
     */

    STL_TRY( knlInitShareBlockFromBlock(
                 & aAggrExecInfo->mAggrValueList,
                 sContexts[ aAggrOptInfo->mAggrExpr->mOffset ].mInfo.mValueInfo,
                 aRegionMem,
                 KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    
    /**
     * Insert Column Value Block List 구성
     */

    STL_TRY( knlInitShareBlock( & aAggrExecInfo->mInsertColList,
                                aAggrExecInfo->mInputArgList,
                                aRegionMem,
                                KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    KNL_SET_BLOCK_COLUMN_ORDER( aAggrExecInfo->mInsertColList, 0 );

    aAggrExecInfo->mInsertColList->mNext = NULL;


    /**
     * Argument Expression을 평가하기 위한 Block Eval Info 설정
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( knlExprEvalInfo * ),
                                (void **) & aAggrExecInfo->mEvalInfo,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_DASSERT( aAggrOptInfo->mArgCodeStack[ 0 ] != NULL );

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( knlExprEvalInfo ),
                                (void **) & sEvalInfo,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sEvalInfo, 0x00, STL_SIZEOF( knlExprEvalInfo ) );

    sEvalInfo->mExprStack       = aAggrOptInfo->mArgCodeStack[ 0 ];
    sEvalInfo->mContext         = aDataArea->mExprDataContext;
    sEvalInfo->mResultBlock     = aAggrExecInfo->mInputArgList;
    sEvalInfo->mBlockIdx        = 0;
    sEvalInfo->mBlockCount      = 0;

    aAggrExecInfo->mEvalInfo[ 0 ] = sEvalInfo;


    /**
     * Row Block 공간 할당
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( smlRowBlock ),
                                (void **) & sRowBlock,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( smlInitRowBlock( sRowBlock,
                              aDataArea->mBlockAllocCnt,
                              aRegionMem,
                              SML_ENV(aEnv) )
             == STL_SUCCESS );

    
    /**
     * Instant Table 설정
     */

    stlMemset( & aAggrExecInfo->mInstantTable, 0x00, STL_SIZEOF( qlnInstantTable ) );
    aAggrExecInfo->mInstantTable.mIsNeedBuild = STL_TRUE;
            

    /**
     * Fetch Info 설정
     */

    sFetchInfo = & aAggrExecInfo->mFetchInfo;
    
    stlMemset( sFetchInfo, 0x00, STL_SIZEOF( smlFetchInfo ) );

    if( DTL_GET_BLOCK_IS_SEP_BUFF( aAggrExecInfo->mInputArgList ) == STL_TRUE )
    {
        STL_TRY( knlInitShareBlock( & sFetchInfo->mColList,
                                    aAggrExecInfo->mInputArgList,
                                    aRegionMem,
                                    KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
    }
    else
    {
        sFetchInfo->mColList = NULL;
    }

    sFetchInfo->mRowBlock = sRowBlock;

    /* aggregation fetch 정보 설정 */
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( smlAggrFuncInfo ),
                                (void **) & sAggrFetch,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sAggrFetch, 0x00, STL_SIZEOF( smlAggrFuncInfo ) );

    sFetchInfo->mAggrFetchInfo = sAggrFetch;

    sAggrFetch->mAggrFuncModule = &gSmlAggrFuncInfoArr[KNL_BUILTIN_AGGREGATION_SUM];
    sAggrFetch->mAggrMergeFunc = gSmlAggrMergeFuncInfoArr[KNL_BUILTIN_AGGREGATION_SUM];
    
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( knlExprEvalInfo * ),
                                (void **) & sAggrFetch->mEvalInfo,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* distinct는 aggregation에 의해 cast되기 전의 type으로 평가 & distinct 평가 이후에 type cast 수행.
     * sAggrFetch->mInputArgStack은 NULL 이 아닐 수 있다. (BUGBUG) */
    sAggrFetch->mInputArgStack = NULL;
    sAggrFetch->mEvalInfo[ 0 ] = NULL;
    sAggrFetch->mNext          = NULL;
    

    STL_TRY( knlInitShareBlock( & sAggrFetch->mInputArgList,
                                aAggrExecInfo->mInputArgList,
                                aRegionMem,
                                KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    KNL_SET_BLOCK_COLUMN_ORDER( sAggrFetch->mInputArgList, 0 );
    
    STL_TRY( knlInitShareBlock( & sAggrFetch->mAggrValueList,
                                aAggrExecInfo->mAggrValueList,
                                aRegionMem,
                                KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    

    /**
     * Optimization 정보 설정
     */
    
    aAggrExecInfo->mAggrOptInfo = aAggrOptInfo;


    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Aggregation을 초기화한다.
 * @param[in]      aAggrExecInfo   Aggregation Execution 정보
 * @param[in]      aBlockIdx       Block Idx
 * @param[in]      aEnv            Environment
 */
stlStatus qlnfSumInit( qlnfAggrExecInfo   * aAggrExecInfo,
                       stlInt32             aBlockIdx,
                       qllEnv             * aEnv )
{
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aBlockIdx >= 0, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mInputArgList != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList->mNext == NULL, QLL_ERROR_STACK( aEnv ) );

    
    /**
     * Aggregation Value Block List 초기화
     */

    STL_DASSERT( aAggrExecInfo->mAggrValueList->mNext == NULL );
    DTL_SET_NULL( KNL_GET_BLOCK_DATA_VALUE( aAggrExecInfo->mAggrValueList, aBlockIdx ) );
    KNL_SET_ONE_BLOCK_USED_CNT( aAggrExecInfo->mAggrValueList, aBlockIdx + 1 );
    
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Aggregation을 수행한다.
 * @param[in]      aAggrExecInfo   Aggregation Execution 정보
 * @param[in]      aStartIdx       Start Block Idx
 * @param[in]      aBlockCnt       Block Used Count
 * @param[in]      aResultBlockIdx Result Block Idx
 * @param[in]      aEnv            Environment
 */
stlStatus qlnfSumBuild( qlnfAggrExecInfo   * aAggrExecInfo,
                        stlInt32             aStartIdx,
                        stlInt32             aBlockCnt,
                        stlInt32             aResultBlockIdx,
                        qllEnv             * aEnv )
{
    knlExprEvalInfo  * sEvalInfo = NULL;

    
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mInputArgList != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList->mNext == NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aStartIdx >= 0, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aBlockCnt > 0, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aResultBlockIdx >= 0, QLL_ERROR_STACK( aEnv ) );
    

    /**
     * Aggregation내 Expression Stack 수행
     */

    sEvalInfo = aAggrExecInfo->mEvalInfo[ 0 ];
    if( sEvalInfo == NULL )
    {
        /* Do Nothing */
    }
    else
    {
        sEvalInfo->mBlockIdx   = aStartIdx;
        sEvalInfo->mBlockCount = aBlockCnt;
        STL_TRY( knlEvaluateBlockExpression( sEvalInfo,
                                             KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
        
        
    /**
     * SUM 연산 수행
     */

    STL_TRY( gKnlAddToNumFuncPtrArr[ KNL_GET_BLOCK_DB_TYPE( aAggrExecInfo->mInputArgList ) ] (
                 aAggrExecInfo->mInputArgList,
                 aAggrExecInfo->mAggrValueList,
                 aStartIdx, /* aStartBlockIdx */
                 aBlockCnt,
                 aResultBlockIdx,
                 KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    
        
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/************************************************
 * COUNT 
 ***********************************************/

/**
 * @brief Aggregation 내의 모든 Expression에 대한 DB type을 설정한다.
 * @param[in]      aInitExpr       Init Expression
 * @param[in]      aDBType         Aggregation에 설정해 줄 DB type
 * @param[in,out]  aExprInfo       All Expression's Information
 * @param[in]      aEnv            Environment
 */
stlStatus qlnfCountSetAggrDBType( qlvInitExpression  * aInitExpr,
                                  dtlDataType          aDBType,
                                  qloExprInfo        * aExprInfo,
                                  qllEnv             * aEnv )
{
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aInitExpr->mType == QLV_EXPR_TYPE_AGGREGATION,
                        QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aExprInfo != NULL, QLL_ERROR_STACK( aEnv ) );


    /**
     * Input Expression에 대한 DB type 설정
     */

    STL_DASSERT( aInitExpr->mExpr.mAggregation->mAggrId ==
                 KNL_BUILTIN_AGGREGATION_COUNT );
    STL_DASSERT( aInitExpr->mExpr.mAggregation->mArgCount == 1 ); 
    STL_DASSERT( aInitExpr->mExpr.mAggregation->mArgs != NULL );

    
    /**
     * Result Expression에 대한 DB type 설정
     */

    STL_TRY( qloSetExprDBType( aInitExpr,
                               aExprInfo,
                               DTL_TYPE_NATIVE_BIGINT,
                               DTL_PRECISION_NA,
                               DTL_SCALE_NA,
                               DTL_STRING_LENGTH_UNIT_NA,
                               DTL_INTERVAL_INDICATOR_NA,
                               aEnv )
             == STL_SUCCESS );
    

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Aggregation Execution 정보를 얻는다.
 * @param[in,out]  aSQLStmt        SQL Statement
 * @param[in]      aAggrOptInfo    Aggregation Optimization Info
 * @param[in]      aDataArea       Data Area (Data Optimization 결과물)
 * @param[out]     aAggrExecInfo   Aggregation Execution Info
 * @param[in]      aRegionMem      Region Memory
 * @param[in]      aEnv            Environment
 */
stlStatus qlnfCountGetAggrExecInfo( qllStatement          * aSQLStmt,
                                    qlnfAggrOptInfo       * aAggrOptInfo,
                                    qllDataArea           * aDataArea,
                                    qlnfAggrExecInfo      * aAggrExecInfo,
                                    knlRegionMem          * aRegionMem,
                                    qllEnv                * aEnv )
{
    qlvInitAggregation    * sAggrExpr  = NULL;
    knlExprContext        * sContexts  = NULL;
    knlExprEvalInfo       * sEvalInfo  = NULL;
    stlInt32                sOffset    = 0;
    dtlDataType             sDataType  = DTL_TYPE_NA;

    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrOptInfo != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK( aEnv ) );

    
    /**
     * Input Argument Count 설정
     */

    sAggrExpr = aAggrOptInfo->mAggrExpr->mExpr.mAggregation;
    sContexts = aDataArea->mExprDataContext->mContexts;

    STL_DASSERT( sAggrExpr->mArgCount == 1 );    
    aAggrExecInfo->mInputArgCnt = sAggrExpr->mArgCount;

    
    /**
     * Input Value Block List 찾기
     */

    /* value block 간의 연결 */
    sOffset = sAggrExpr->mArgs[ 0 ]->mOffset;

    STL_DASSERT( sContexts[ sOffset ].mCast == NULL );

    switch( sAggrExpr->mArgs[ 0 ]->mType )
    {
        case QLV_EXPR_TYPE_VALUE :
        case QLV_EXPR_TYPE_BIND_PARAM :
        case QLV_EXPR_TYPE_PSEUDO_COLUMN :
        case QLV_EXPR_TYPE_COLUMN :
        case QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT :
        case QLV_EXPR_TYPE_REFERENCE :
        case QLV_EXPR_TYPE_INNER_COLUMN :
        case QLV_EXPR_TYPE_OUTER_COLUMN :
        case QLV_EXPR_TYPE_ROWID_COLUMN :
        case QLV_EXPR_TYPE_AGGREGATION :
        case QLV_EXPR_TYPE_ROW_EXPR:
            {
                /**
                 * 연관된 Value Block이 있고, Internal Cast가 없으면 Expression에 대한 Value Block을 연결
                 */
        
                STL_TRY( knlInitShareBlockFromBlock(
                             & aAggrExecInfo->mInputArgList,
                             sContexts[ sOffset ].mInfo.mValueInfo,
                             aRegionMem,
                             KNL_ENV( aEnv ) )
                         == STL_SUCCESS );
                break;
            }
        case QLV_EXPR_TYPE_FUNCTION :
        case QLV_EXPR_TYPE_CAST :
        case QLV_EXPR_TYPE_CASE_EXPR :
            {
                sDataType = sContexts[ sOffset ].mInfo.mFuncData->mResultValue.mType;

                STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                            & aAggrExecInfo->mInputArgList,
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
                break;
            }
        case QLV_EXPR_TYPE_LIST_FUNCTION:
        case QLV_EXPR_TYPE_LIST_COLUMN:
            {
                sDataType = sContexts[ sOffset ].mInfo.mListFunc->mResultValue.mType;

                STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                            & aAggrExecInfo->mInputArgList,
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
                break;
            }
        case QLV_EXPR_TYPE_SUB_QUERY :
        case QLV_EXPR_TYPE_PSEUDO_ARGUMENT :
        default :
            {
                STL_DASSERT( 0 );
                break;
            }
    }


    /**
     * Aggregation Value Block List 찾기
     */

    STL_TRY( knlInitShareBlockFromBlock(
                 & aAggrExecInfo->mAggrValueList,
                 sContexts[ aAggrOptInfo->mAggrExpr->mOffset ].mInfo.mValueInfo,
                 aRegionMem,
                 KNL_ENV( aEnv ) )
             == STL_SUCCESS );


    /**
     * Argument Expression을 평가하기 위한 Block Eval Info 설정
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( knlExprEvalInfo * ),
                                (void **) & aAggrExecInfo->mEvalInfo,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    if( aAggrOptInfo->mArgCodeStack[ 0 ] == NULL )
    {
        aAggrExecInfo->mEvalInfo[ 0 ] = NULL;
    }
    else
    {
        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( knlExprEvalInfo ),
                                    (void **) & sEvalInfo,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemset( sEvalInfo, 0x00, STL_SIZEOF( knlExprEvalInfo ) );

        sEvalInfo->mExprStack       = aAggrOptInfo->mArgCodeStack[ 0 ];
        sEvalInfo->mContext         = aDataArea->mExprDataContext;
        sEvalInfo->mResultBlock     = aAggrExecInfo->mInputArgList;
        sEvalInfo->mBlockIdx        = 0;
        sEvalInfo->mBlockCount      = 0;

        aAggrExecInfo->mEvalInfo[ 0 ] = sEvalInfo;
    }   
    

    /**
     * Optimization 정보 설정
     */
    
    aAggrExecInfo->mAggrOptInfo = aAggrOptInfo;


    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Aggregation Distinct Execution 정보를 얻는다.
 * @param[in,out]  aSQLStmt        SQL Statement
 * @param[in]      aAggrOptInfo    Aggregation Optimization Info
 * @param[in]      aDataArea       Data Area (Data Optimization 결과물)
 * @param[out]     aAggrExecInfo   Aggregation Execution Info
 * @param[in]      aRegionMem      Region Memory
 * @param[in]      aEnv            Environment
 */
stlStatus qlnfCountGetAggrDistExecInfo( qllStatement          * aSQLStmt,
                                        qlnfAggrOptInfo       * aAggrOptInfo,
                                        qllDataArea           * aDataArea,
                                        qlnfAggrDistExecInfo  * aAggrExecInfo,
                                        knlRegionMem          * aRegionMem,
                                        qllEnv                * aEnv )
{
    qlvInitAggregation    * sAggrExpr  = NULL;
    knlExprContext        * sContexts  = NULL;
    knlExprEvalInfo       * sEvalInfo  = NULL;

    smlRowBlock           * sRowBlock  = NULL;
    smlFetchInfo          * sFetchInfo = NULL;
    smlAggrFuncInfo       * sAggrFetch = NULL;
    
    stlInt32                sOffset    = 0;
    dtlDataType             sDataType  = DTL_TYPE_NA;

    
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrOptInfo != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK( aEnv ) );


    /**
     * Input Argument Count 설정
     */

    sAggrExpr = aAggrOptInfo->mAggrExpr->mExpr.mAggregation;
    sContexts = aDataArea->mExprDataContext->mContexts;

    STL_DASSERT( sAggrExpr->mArgCount == 1 );    
    aAggrExecInfo->mInputArgCnt = sAggrExpr->mArgCount;
    
    /* value block 간의 연결 */
    sOffset = sAggrExpr->mArgs[ 0 ]->mOffset;

    STL_DASSERT( sContexts[ sOffset ].mCast == NULL );

    switch( sAggrExpr->mArgs[ 0 ]->mType )
    {
        case QLV_EXPR_TYPE_VALUE :
        case QLV_EXPR_TYPE_BIND_PARAM :
        case QLV_EXPR_TYPE_PSEUDO_COLUMN :
        case QLV_EXPR_TYPE_COLUMN :
        case QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT :
        case QLV_EXPR_TYPE_REFERENCE :
        case QLV_EXPR_TYPE_INNER_COLUMN :
        case QLV_EXPR_TYPE_OUTER_COLUMN :
        case QLV_EXPR_TYPE_ROWID_COLUMN :
        case QLV_EXPR_TYPE_AGGREGATION :
            {
                /**
                 * 연관된 Value Block이 있고, Internal Cast가 없으면 Expression에 대한 Value Block을 연결
                 */
        
                STL_TRY( knlInitShareBlockFromBlock(
                             & aAggrExecInfo->mInputArgList,
                             sContexts[ sOffset ].mInfo.mValueInfo,
                             aRegionMem,
                             KNL_ENV( aEnv ) )
                         == STL_SUCCESS );
                break;
            }
        case QLV_EXPR_TYPE_FUNCTION :
        case QLV_EXPR_TYPE_CAST :
        case QLV_EXPR_TYPE_CASE_EXPR :
            {
                /**
                 * 새로운 Value Block 할당
                 */

                sDataType = sContexts[ sOffset ].mInfo.mFuncData->mResultValue.mType;

                STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                            & aAggrExecInfo->mInputArgList,
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
                break;
            }
        case QLV_EXPR_TYPE_LIST_FUNCTION:
        case QLV_EXPR_TYPE_LIST_COLUMN:
            {
                /**
                 * 새로운 Value Block 할당
                 */

                sDataType = sContexts[ sOffset ].mInfo.mListFunc->mResultValue.mType;

                STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                            & aAggrExecInfo->mInputArgList,
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
                break;
            }
        case QLV_EXPR_TYPE_SUB_QUERY :
        case QLV_EXPR_TYPE_PSEUDO_ARGUMENT :
        default :
            {
                STL_DASSERT( 0 );
                break;
            }
    }
        

    /**
     * Aggregation Value Block List 찾기
     */

    STL_TRY( knlInitShareBlockFromBlock(
                 & aAggrExecInfo->mAggrValueList,
                 sContexts[ aAggrOptInfo->mAggrExpr->mOffset ].mInfo.mValueInfo,
                 aRegionMem,
                 KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    
    /**
     * Insert Column Value Block List 구성
     */

    STL_TRY( knlInitShareBlock( & aAggrExecInfo->mInsertColList,
                                aAggrExecInfo->mInputArgList,
                                aRegionMem,
                                KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    KNL_SET_BLOCK_COLUMN_ORDER( aAggrExecInfo->mInsertColList, 0 );

    aAggrExecInfo->mInsertColList->mNext = NULL;
    

    /**
     * Argument Expression을 평가하기 위한 Block Eval Info 설정
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( knlExprEvalInfo * ),
                                (void **) & aAggrExecInfo->mEvalInfo,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_DASSERT( aAggrOptInfo->mArgCodeStack[ 0 ] != NULL );

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( knlExprEvalInfo ),
                                (void **) & sEvalInfo,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sEvalInfo, 0x00, STL_SIZEOF( knlExprEvalInfo ) );

    sEvalInfo->mExprStack       = aAggrOptInfo->mArgCodeStack[ 0 ];
    sEvalInfo->mContext         = aDataArea->mExprDataContext;
    sEvalInfo->mResultBlock     = aAggrExecInfo->mInputArgList;
    sEvalInfo->mBlockIdx        = 0;
    sEvalInfo->mBlockCount      = 0;

    aAggrExecInfo->mEvalInfo[ 0 ] = sEvalInfo;
    

    /**
     * Row Block 공간 할당
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( smlRowBlock ),
                                (void **) & sRowBlock,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( smlInitRowBlock( sRowBlock,
                              aDataArea->mBlockAllocCnt,
                              aRegionMem,
                              SML_ENV(aEnv) )
             == STL_SUCCESS );

    
    /**
     * Instant Table 설정
     */

    stlMemset( & aAggrExecInfo->mInstantTable, 0x00, STL_SIZEOF( qlnInstantTable ) );
    aAggrExecInfo->mInstantTable.mIsNeedBuild = STL_TRUE;
            

    /**
     * Fetch Info 설정
     */

    sFetchInfo = & aAggrExecInfo->mFetchInfo;
    
    stlMemset( sFetchInfo, 0x00, STL_SIZEOF( smlFetchInfo ) );

    if( DTL_GET_BLOCK_IS_SEP_BUFF( aAggrExecInfo->mInputArgList ) == STL_TRUE )
    {
        STL_TRY( knlInitShareBlock( & sFetchInfo->mColList,
                                    aAggrExecInfo->mInputArgList,
                                    aRegionMem,
                                    KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
    }
    else
    {
        sFetchInfo->mColList = NULL;
    }

    sFetchInfo->mRowBlock = sRowBlock;

    /* aggregation fetch 정보 설정 */
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( smlAggrFuncInfo ),
                                (void **) & sAggrFetch,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sAggrFetch, 0x00, STL_SIZEOF( smlAggrFuncInfo ) );

    sFetchInfo->mAggrFetchInfo = sAggrFetch;

    sAggrFetch->mAggrFuncModule = &gSmlAggrFuncInfoArr[KNL_BUILTIN_AGGREGATION_COUNT];
    sAggrFetch->mAggrMergeFunc = gSmlAggrMergeFuncInfoArr[KNL_BUILTIN_AGGREGATION_COUNT];
    
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( knlExprEvalInfo * ),
                                (void **) & sAggrFetch->mEvalInfo,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sAggrFetch->mInputArgStack = NULL;
    sAggrFetch->mEvalInfo[ 0 ] = NULL;
    sAggrFetch->mNext          = NULL;
    

    STL_TRY( knlInitShareBlock( & sAggrFetch->mInputArgList,
                                aAggrExecInfo->mInputArgList,
                                aRegionMem,
                                KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    KNL_SET_BLOCK_COLUMN_ORDER( sAggrFetch->mInputArgList, 0 );
    
    STL_TRY( knlInitShareBlock( & sAggrFetch->mAggrValueList,
                                aAggrExecInfo->mAggrValueList,
                                aRegionMem,
                                KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    

    /**
     * Optimization 정보 설정
     */
    
    aAggrExecInfo->mAggrOptInfo = aAggrOptInfo;


    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Aggregation을 초기화한다.
 * @param[in]      aAggrExecInfo   Aggregation Execution 정보
 * @param[in]      aBlockIdx       Block Idx
 * @param[in]      aEnv            Environment
 */
stlStatus qlnfCountInit( qlnfAggrExecInfo   * aAggrExecInfo,
                         stlInt32             aBlockIdx,
                         qllEnv             * aEnv )
{
    dtlDataValue     * sDataValue    = NULL;
    dtlBigIntType    * sBigInt       = NULL;
    
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aBlockIdx >= 0, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mInputArgList != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList != NULL, QLL_ERROR_STACK( aEnv ) );

    
    /**
     * Aggregation Value Block List 초기화
     */

    STL_DASSERT( aAggrExecInfo->mAggrValueList->mNext == NULL );

    /* Set Zero */
    sDataValue = KNL_GET_BLOCK_DATA_VALUE( aAggrExecInfo->mAggrValueList, aBlockIdx );
    sDataValue->mLength = DTL_NATIVE_BIGINT_SIZE;
    sBigInt    = (dtlBigIntType *) sDataValue->mValue;
    *sBigInt   = 0;
    
    KNL_SET_ONE_BLOCK_USED_CNT( aAggrExecInfo->mAggrValueList, aBlockIdx + 1 );
    
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Aggregation을 수행한다.
 * @param[in]      aAggrExecInfo   Aggregation Execution 정보
 * @param[in]      aStartIdx       Start Block Idx
 * @param[in]      aBlockCnt       Block Used Count
 * @param[in]      aResultBlockIdx Result Block Idx
 * @param[in]      aEnv            Environment
 */
stlStatus qlnfCountBuild( qlnfAggrExecInfo   * aAggrExecInfo,
                          stlInt32             aStartIdx,
                          stlInt32             aBlockCnt,
                          stlInt32             aResultBlockIdx,
                          qllEnv             * aEnv )
{
    stlInt32                sLoop      = 0;
    stlInt32                sAddCnt    = 0;
    dtlDataValue          * sDataValue = NULL;
    dtlBigIntType         * sBigInt    = NULL;
    knlExprEvalInfo       * sEvalInfo  = NULL;

    
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mInputArgList != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList->mNext == NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aStartIdx >= 0, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aBlockCnt > 0, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aResultBlockIdx >= 0, QLL_ERROR_STACK( aEnv ) );


    /**
     * Aggregation내 Expression Stack 수행
     */

    sEvalInfo = aAggrExecInfo->mEvalInfo[ 0 ];
    if( sEvalInfo == NULL )
    {
        /* Do Nothing */
    }
    else
    {
        sEvalInfo->mBlockIdx   = aStartIdx;
        sEvalInfo->mBlockCount = aBlockCnt;
        STL_TRY( knlEvaluateBlockExpression( sEvalInfo,
                                             KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }


    /**
     * COUNT 연산 수행
     */

    sDataValue = KNL_GET_BLOCK_DATA_VALUE( aAggrExecInfo->mInputArgList, aStartIdx );
    sBigInt    = (dtlBigIntType *) KNL_GET_BLOCK_DATA_VALUE( aAggrExecInfo->mAggrValueList, aResultBlockIdx )->mValue;
        
    if( KNL_GET_BLOCK_IS_SEP_BUFF( aAggrExecInfo->mInputArgList ) == STL_TRUE )
    {
        for( sLoop = 0 ; sLoop < aBlockCnt ; sLoop++ )
        {
            sAddCnt += ( DTL_IS_NULL( sDataValue ) == STL_FALSE );
            sDataValue++;
        }

        *sBigInt += sAddCnt;
    }
    else
    {
        if( DTL_IS_NULL( sDataValue ) )
        {
            /* Do Nothing */
        }
        else
        {
            *sBigInt += aBlockCnt;
        }
    }
    
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/************************************************
 * COUNT ASTERISK
 ***********************************************/

/**
 * @brief Aggregation 내의 모든 Expression에 대한 DB type을 설정한다.
 * @param[in]      aInitExpr       Init Expression
 * @param[in]      aDBType         Aggregation에 설정해 줄 DB type
 * @param[in,out]  aExprInfo       All Expression's Information
 * @param[in]      aEnv            Environment
 */
stlStatus qlnfCountAsteriskSetAggrDBType( qlvInitExpression  * aInitExpr,
                                          dtlDataType          aDBType,
                                          qloExprInfo        * aExprInfo,
                                          qllEnv             * aEnv )
{
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aInitExpr->mType == QLV_EXPR_TYPE_AGGREGATION,
                        QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aExprInfo != NULL, QLL_ERROR_STACK( aEnv ) );


    /**
     * Input Expression에 대한 DB type 설정
     */

    STL_DASSERT( aInitExpr->mExpr.mAggregation->mAggrId ==
                 KNL_BUILTIN_AGGREGATION_COUNT_ASTERISK );
    STL_DASSERT( aInitExpr->mExpr.mAggregation->mArgCount == 0 ); 
    STL_DASSERT( aInitExpr->mExpr.mAggregation->mArgs == NULL );

    
    /**
     * Result Expression에 대한 DB type 설정
     */

    STL_TRY( qloSetExprDBType( aInitExpr,
                               aExprInfo,
                               DTL_TYPE_NATIVE_BIGINT,
                               DTL_PRECISION_NA,
                               DTL_SCALE_NA,
                               DTL_STRING_LENGTH_UNIT_NA,
                               DTL_INTERVAL_INDICATOR_NA,
                               aEnv )
             == STL_SUCCESS );
    

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Aggregation Execution 정보를 얻는다.
 * @param[in,out]  aSQLStmt        SQL Statement
 * @param[in]      aAggrOptInfo    Aggregation Optimization Info
 * @param[in]      aDataArea       Data Area (Data Optimization 결과물)
 * @param[out]     aAggrExecInfo   Aggregation Execution Info
 * @param[in]      aRegionMem      Region Memory
 * @param[in]      aEnv            Environment
 */
stlStatus qlnfCountAsteriskGetAggrExecInfo( qllStatement          * aSQLStmt,
                                            qlnfAggrOptInfo       * aAggrOptInfo,
                                            qllDataArea           * aDataArea,
                                            qlnfAggrExecInfo      * aAggrExecInfo,
                                            knlRegionMem          * aRegionMem,
                                            qllEnv                * aEnv )
{
    knlExprContext  * sContexts = NULL;

    
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrOptInfo != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK( aEnv ) );


    /**
     * Input Value Block List 찾기
     */

    STL_DASSERT( aAggrOptInfo->mAggrExpr->mExpr.mAggregation->mArgCount == 0 );
    aAggrExecInfo->mInputArgList = NULL;
    aAggrExecInfo->mInputArgCnt  = 0;
            

    /**
     * Aggregation Value Block List 찾기
     */

    sContexts = aDataArea->mExprDataContext->mContexts;
    STL_TRY( knlInitShareBlockFromBlock(
                 & aAggrExecInfo->mAggrValueList,
                 sContexts[ aAggrOptInfo->mAggrExpr->mOffset].mInfo.mValueInfo,
                 aRegionMem,
                 KNL_ENV( aEnv ) )
             == STL_SUCCESS );
        

    /**
     * Argument Expression을 평가하기 위한 Block Eval Info 설정
     * @remark 구문에서 설정할 수 없음
     */

    aAggrOptInfo->mArgCodeStack = NULL;
    
    
    /**
     * Optimization 정보 설정
     */
    
    aAggrExecInfo->mAggrOptInfo = aAggrOptInfo;


    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Aggregation을 초기화한다.
 * @param[in]      aAggrExecInfo   Aggregation Execution 정보
 * @param[in]      aBlockIdx       Block Idx
 * @param[in]      aEnv            Environment
 */
stlStatus qlnfCountAsteriskInit( qlnfAggrExecInfo   * aAggrExecInfo,
                                 stlInt32             aBlockIdx,
                                 qllEnv             * aEnv )
{
    dtlDataValue     * sDataValue    = NULL;
    dtlBigIntType    * sBigInt       = NULL;
    
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aBlockIdx >= 0, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mInputArgList == NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList->mNext == NULL, QLL_ERROR_STACK( aEnv ) );

    
    /**
     * Aggregation Value Block List 초기화
     */

    STL_DASSERT( aAggrExecInfo->mAggrValueList->mNext == NULL );

    /* Set Zero */
    sDataValue = KNL_GET_BLOCK_DATA_VALUE( aAggrExecInfo->mAggrValueList, aBlockIdx );
    sDataValue->mLength = DTL_NATIVE_BIGINT_SIZE;
    sBigInt    = (dtlBigIntType *) sDataValue->mValue;
    *sBigInt   = 0;
    
    KNL_SET_ONE_BLOCK_USED_CNT( aAggrExecInfo->mAggrValueList, aBlockIdx + 1 );
    
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Aggregation을 수행한다.
 * @param[in]      aAggrExecInfo   Aggregation Execution 정보
 * @param[in]      aStartIdx       Start Block Idx
 * @param[in]      aBlockCnt       Block Used Count
 * @param[in]      aResultBlockIdx Result Block Idx
 * @param[in]      aEnv            Environment
 */
stlStatus qlnfCountAsteriskBuild( qlnfAggrExecInfo   * aAggrExecInfo,
                                  stlInt32             aStartIdx,
                                  stlInt32             aBlockCnt,
                                  stlInt32             aResultBlockIdx,
                                  qllEnv             * aEnv )
{
    dtlBigIntType  * sBigInt    = NULL;


    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mInputArgList == NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList->mNext == NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aStartIdx >= 0, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aBlockCnt > 0, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aResultBlockIdx >= 0, QLL_ERROR_STACK( aEnv ) );


    /**
     * COUNT 연산 수행
     */

    sBigInt = (dtlBigIntType *) KNL_GET_BLOCK_DATA_VALUE( aAggrExecInfo->mAggrValueList, aResultBlockIdx )->mValue;

    *sBigInt += aBlockCnt;
    
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/************************************************
 * MIN 
 ***********************************************/

/**
 * @brief Aggregation 내의 모든 Expression에 대한 DB type을 설정한다.
 * @param[in]      aInitExpr       Init Expression
 * @param[in]      aDBType         Aggregation에 설정해 줄 DB type
 * @param[in,out]  aExprInfo       All Expression's Information
 * @param[in]      aEnv            Environment
 */
stlStatus qlnfMinSetAggrDBType( qlvInitExpression  * aInitExpr,
                                dtlDataType          aDBType,
                                qloExprInfo        * aExprInfo,
                                qllEnv             * aEnv )
{
//    qlvInitAggregation  * sAggrExpr = NULL;

    qloDBType * sArgDBType = NULL;
    
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aInitExpr->mType == QLV_EXPR_TYPE_AGGREGATION,
                        QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aExprInfo != NULL, QLL_ERROR_STACK( aEnv ) );


    /**
     * Input Expression에 대한 DB type 설정
     * => 원본 type 유지
     */

//    sAggrExpr = aInitExpr->mExpr.mAggregation;

//    STL_DASSERT( sAggrExpr->mAggrId == KNL_BUILTIN_AGGREGATION_MIN );
//    STL_DASSERT( sAggrExpr->mArgCount == 1 ); 
//    STL_DASSERT( sAggrExpr->mArgs != NULL );
    STL_DASSERT( aInitExpr->mExpr.mAggregation->mAggrId == KNL_BUILTIN_AGGREGATION_MIN );
    STL_DASSERT( aInitExpr->mExpr.mAggregation->mArgCount == 1 ); 
    STL_DASSERT( aInitExpr->mExpr.mAggregation->mArgs != NULL );

    STL_DASSERT( ( aDBType >= DTL_TYPE_BOOLEAN ) &&
                 ( aDBType < DTL_TYPE_MAX ) );


    /**
     * Result Expression에 대한 DB type 설정
     */

    sArgDBType = & aExprInfo->mExprDBType[ aInitExpr->mExpr.mAggregation->mArgs[0]->mOffset ];

    STL_TRY( qloSetExprDBType( aInitExpr,
                               aExprInfo,
                               aDBType,
                               sArgDBType->mArgNum1,
                               sArgDBType->mArgNum2,
                               sArgDBType->mStringLengthUnit,
                               sArgDBType->mIntervalIndicator,
                               aEnv )
             == STL_SUCCESS );

    

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Aggregation Execution 정보를 얻는다.
 * @param[in,out]  aSQLStmt        SQL Statement
 * @param[in]      aAggrOptInfo    Aggregation Optimization Info
 * @param[in]      aDataArea       Data Area (Data Optimization 결과물)
 * @param[out]     aAggrExecInfo   Aggregation Execution Info
 * @param[in]      aRegionMem      Region Memory
 * @param[in]      aEnv            Environment
 */
stlStatus qlnfMinGetAggrExecInfo( qllStatement          * aSQLStmt,
                                  qlnfAggrOptInfo       * aAggrOptInfo,
                                  qllDataArea           * aDataArea,
                                  qlnfAggrExecInfo      * aAggrExecInfo,
                                  knlRegionMem          * aRegionMem,
                                  qllEnv                * aEnv )
{
    qlvInitAggregation    * sAggrExpr = NULL;
    knlExprContext        * sContexts = NULL;
    knlExprEvalInfo       * sEvalInfo = NULL;
    stlInt32                sOffset   = 0;
    dtlDataType             sDataType = DTL_TYPE_NA;

    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrOptInfo != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK( aEnv ) );


    /**
     * Input Argument Count 설정
     */

    sAggrExpr = aAggrOptInfo->mAggrExpr->mExpr.mAggregation;
    sContexts = aDataArea->mExprDataContext->mContexts;

    STL_DASSERT( sAggrExpr->mArgCount == 1 );    
    aAggrExecInfo->mInputArgCnt = sAggrExpr->mArgCount;
    
    /* value block 간의 연결 */
    sOffset = sAggrExpr->mArgs[ 0 ]->mOffset;

    STL_DASSERT( sContexts[ sOffset ].mCast == NULL );

    switch( sAggrExpr->mArgs[ 0 ]->mType )
    {
        case QLV_EXPR_TYPE_VALUE :
        case QLV_EXPR_TYPE_BIND_PARAM :
        case QLV_EXPR_TYPE_PSEUDO_COLUMN :
        case QLV_EXPR_TYPE_COLUMN :
        case QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT :
        case QLV_EXPR_TYPE_REFERENCE :
        case QLV_EXPR_TYPE_INNER_COLUMN :
        case QLV_EXPR_TYPE_OUTER_COLUMN :
        case QLV_EXPR_TYPE_ROWID_COLUMN :
        case QLV_EXPR_TYPE_AGGREGATION :
        case QLV_EXPR_TYPE_ROW_EXPR:
            {
                /**
                 * 연관된 Value Block이 있고, Internal Cast가 없으면 Expression에 대한 Value Block을 연결
                 */
        
                STL_TRY( knlInitShareBlockFromBlock(
                             & aAggrExecInfo->mInputArgList,
                             sContexts[ sOffset ].mInfo.mValueInfo,
                             aRegionMem,
                             KNL_ENV( aEnv ) )
                         == STL_SUCCESS );
                break;
            }
        case QLV_EXPR_TYPE_FUNCTION :
        case QLV_EXPR_TYPE_CAST :
        case QLV_EXPR_TYPE_CASE_EXPR :
            {
                /**
                 * 새로운 Value Block 할당
                 */

                sDataType = sContexts[ sOffset ].mInfo.mFuncData->mResultValue.mType;

                STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                            & aAggrExecInfo->mInputArgList,
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
                break;
            }
        case QLV_EXPR_TYPE_LIST_FUNCTION:
        case QLV_EXPR_TYPE_LIST_COLUMN:
            {
                /**
                 * 새로운 Value Block 할당
                 */

                sDataType = sContexts[ sOffset ].mInfo.mListFunc->mResultValue.mType;

                STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                            & aAggrExecInfo->mInputArgList,
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
                break;
            }
        case QLV_EXPR_TYPE_SUB_QUERY :
        case QLV_EXPR_TYPE_PSEUDO_ARGUMENT :
        default :
            {
                STL_DASSERT( 0 );
                break;
            }
    }

    
    /**
     * Aggregation Value Block List 찾기
     */

    STL_TRY( knlInitShareBlockFromBlock(
                 & aAggrExecInfo->mAggrValueList,
                 sContexts[ aAggrOptInfo->mAggrExpr->mOffset ].mInfo.mValueInfo,
                 aRegionMem,
                 KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    
    /**
     * Argument Expression을 평가하기 위한 Block Eval Info 설정
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( knlExprEvalInfo * ),
                                (void **) & aAggrExecInfo->mEvalInfo,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    if( aAggrOptInfo->mArgCodeStack[ 0 ] == NULL )
    {
        aAggrExecInfo->mEvalInfo[ 0 ] = NULL;
    }
    else
    {
        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( knlExprEvalInfo ),
                                    (void **) & sEvalInfo,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemset( sEvalInfo, 0x00, STL_SIZEOF( knlExprEvalInfo ) );

        sEvalInfo->mExprStack       = aAggrOptInfo->mArgCodeStack[ 0 ];
        sEvalInfo->mContext         = aDataArea->mExprDataContext;
        sEvalInfo->mResultBlock     = aAggrExecInfo->mInputArgList;
        sEvalInfo->mBlockIdx        = 0;
        sEvalInfo->mBlockCount      = 0;

        aAggrExecInfo->mEvalInfo[ 0 ] = sEvalInfo;
    }   
    

    /**
     * Optimization 정보 설정
     */
    
    aAggrExecInfo->mAggrOptInfo = aAggrOptInfo;


    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Aggregation을 초기화한다.
 * @param[in]      aAggrExecInfo   Aggregation Execution 정보
 * @param[in]      aBlockIdx       Block Idx
 * @param[in]      aEnv            Environment
 */
stlStatus qlnfMinInit( qlnfAggrExecInfo   * aAggrExecInfo,
                       stlInt32             aBlockIdx,
                       qllEnv             * aEnv )
{
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aBlockIdx >= 0, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mInputArgList != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList != NULL, QLL_ERROR_STACK( aEnv ) );

    
    /**
     * Aggregation Value Block List 초기화
     */

    STL_DASSERT( aAggrExecInfo->mAggrValueList->mNext == NULL );
    DTL_SET_NULL( KNL_GET_BLOCK_DATA_VALUE( aAggrExecInfo->mAggrValueList, aBlockIdx ) );
    KNL_SET_ONE_BLOCK_USED_CNT( aAggrExecInfo->mAggrValueList, aBlockIdx + 1 );
        

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Aggregation을 수행한다.
 * @param[in]      aAggrExecInfo   Aggregation Execution 정보
 * @param[in]      aStartIdx       Start Block Idx
 * @param[in]      aBlockCnt       Block Used Count
 * @param[in]      aResultBlockIdx Result Block Idx
 * @param[in]      aEnv            Environment
 */
stlStatus qlnfMinBuild( qlnfAggrExecInfo   * aAggrExecInfo,
                        stlInt32             aStartIdx,
                        stlInt32             aBlockCnt,
                        stlInt32             aResultBlockIdx,
                        qllEnv             * aEnv )
{
    stlInt32                sLoop        = 0;
    dtlBooleanType          sBoolean;
    dtlDataValue            sTempValue   = { DTL_TYPE_BOOLEAN, STL_SIZEOF(dtlBooleanType), 0, & sBoolean };
    dtlDataValue          * sDataValue   = NULL;
    dtlDataValue          * sResultValue = NULL;
    dtlDataValue          * sCompResult  = NULL;
    knlExprEvalInfo       * sEvalInfo    = NULL;
    dtlBuiltInFuncPtr       sCompFunc    = NULL;
    dtlDataType             sDataType    = DTL_TYPE_NA;
    dtlValueEntry           sValueEntry[ 2 ];
    
    
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mInputArgList != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aStartIdx >= 0, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aBlockCnt > 0, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aResultBlockIdx >= 0, QLL_ERROR_STACK( aEnv ) );
    

    /**
     * Aggregation내 Expression Stack 수행
     */

    sEvalInfo = aAggrExecInfo->mEvalInfo[ 0 ];
    if( sEvalInfo == NULL )
    {
        /* Do Nothing */
    }
    else
    {
        sEvalInfo->mBlockIdx   = aStartIdx;
        sEvalInfo->mBlockCount = aBlockCnt;
        STL_TRY( knlEvaluateBlockExpression( sEvalInfo,
                                             KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
        
        
    /**
     * MIN 연산 수행
     */

    sDataValue   = KNL_GET_BLOCK_DATA_VALUE( aAggrExecInfo->mInputArgList, aStartIdx );
    sResultValue = KNL_GET_BLOCK_DATA_VALUE( aAggrExecInfo->mAggrValueList, aResultBlockIdx );
    sCompResult  = & sTempValue;
    sDataType    = sDataValue->mType;
    sCompFunc    = dtlCompOperArg2FuncList[ sDataType ][ sDataType ][ DTL_COMP_OPER_IS_LESS_THAN ];
        
    STL_DASSERT( sCompFunc != NULL );

    if( KNL_GET_BLOCK_IS_SEP_BUFF( aAggrExecInfo->mInputArgList ) == STL_TRUE )
    {
        sValueEntry[1].mValue.mDataValue = sResultValue;
            
        for( sLoop = 0 ; sLoop < aBlockCnt ; sLoop++ )
        {
            if( DTL_IS_NULL( sDataValue ) )
            {
                /* Do Nothing */
            }
            else
            {
                if( DTL_IS_NULL( sResultValue ) )
                {
                    STL_TRY( knlCopyDataValue( sDataValue,
                                               sResultValue,
                                               KNL_ENV(aEnv) )
                             == STL_SUCCESS );
                }
                else
                {
                    sValueEntry[0].mValue.mDataValue = sDataValue;
                
                    STL_TRY( sCompFunc( 2,
                                        sValueEntry,
                                        sCompResult,
                                        NULL,
                                        NULL,
                                        NULL,
                                        aEnv )
                             == STL_SUCCESS );

                    if( DTL_BOOLEAN_IS_TRUE( sCompResult ) )
                    {
                        STL_TRY( knlCopyDataValue( sDataValue,
                                                   sResultValue,
                                                   KNL_ENV(aEnv) )
                                 == STL_SUCCESS );
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }
            }
                
            sDataValue++;
        }
    }
    else
    {
        if( DTL_IS_NULL( sDataValue ) )
        {
            /* Do Nothing */
        }
        else
        {
            if( DTL_IS_NULL( sResultValue ) )
            {
                STL_TRY( knlCopyDataValue( sDataValue,
                                           sResultValue,
                                           KNL_ENV(aEnv) )
                         == STL_SUCCESS );
            }
            else
            {
                /* Do Nothing */
            }
        }
    }
    
        
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/************************************************
 * MAX 
 ***********************************************/

/**
 * @brief Aggregation 내의 모든 Expression에 대한 DB type을 설정한다.
 * @param[in]      aInitExpr       Init Expression
 * @param[in]      aDBType         Aggregation에 설정해 줄 DB type
 * @param[in,out]  aExprInfo       All Expression's Information
 * @param[in]      aEnv            Environment
 */
stlStatus qlnfMaxSetAggrDBType( qlvInitExpression  * aInitExpr,
                                dtlDataType          aDBType,
                                qloExprInfo        * aExprInfo,
                                qllEnv             * aEnv )
{
//    qlvInitAggregation  * sAggrExpr = NULL;

    qloDBType * sArgDBType = NULL;
    
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aInitExpr->mType == QLV_EXPR_TYPE_AGGREGATION,
                        QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aExprInfo != NULL, QLL_ERROR_STACK( aEnv ) );


    /**
     * Input Expression에 대한 DB type 설정
     * => 원본 type 유지
     */

//    sAggrExpr = aInitExpr->mExpr.mAggregation;

//    STL_DASSERT( sAggrExpr->mAggrId == KNL_BUILTIN_AGGREGATION_MAX );
//    STL_DASSERT( sAggrExpr->mArgCount == 1 ); 
//    STL_DASSERT( sAggrExpr->mArgs != NULL );
    STL_DASSERT( aInitExpr->mExpr.mAggregation->mAggrId == KNL_BUILTIN_AGGREGATION_MAX );
    STL_DASSERT( aInitExpr->mExpr.mAggregation->mArgCount == 1 ); 
    STL_DASSERT( aInitExpr->mExpr.mAggregation->mArgs != NULL );

    STL_DASSERT( ( aDBType >= DTL_TYPE_BOOLEAN ) &&
                 ( aDBType < DTL_TYPE_MAX ) );


    /**
     * Result Expression에 대한 DB type 설정
     */

    sArgDBType = & aExprInfo->mExprDBType[ aInitExpr->mExpr.mAggregation->mArgs[0]->mOffset ];

    STL_TRY( qloSetExprDBType( aInitExpr,
                               aExprInfo,
                               aDBType,
                               sArgDBType->mArgNum1,
                               sArgDBType->mArgNum2,
                               sArgDBType->mStringLengthUnit,
                               sArgDBType->mIntervalIndicator,
                               aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Aggregation Execution 정보를 얻는다.
 * @param[in,out]  aSQLStmt        SQL Statement
 * @param[in]      aAggrOptInfo    Aggregation Optimization Info
 * @param[in]      aDataArea       Data Area (Data Optimization 결과물)
 * @param[out]     aAggrExecInfo   Aggregation Execution Info
 * @param[in]      aRegionMem      Region Memory
 * @param[in]      aEnv            Environment
 */
stlStatus qlnfMaxGetAggrExecInfo( qllStatement          * aSQLStmt,
                                  qlnfAggrOptInfo       * aAggrOptInfo,
                                  qllDataArea           * aDataArea,
                                  qlnfAggrExecInfo      * aAggrExecInfo,
                                  knlRegionMem          * aRegionMem,
                                  qllEnv                * aEnv )
{
    qlvInitAggregation    * sAggrExpr = NULL;
    knlExprContext        * sContexts = NULL;
    knlExprEvalInfo       * sEvalInfo = NULL;
    stlInt32                sOffset   = 0;
    dtlDataType             sDataType = DTL_TYPE_NA;

    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrOptInfo != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK( aEnv ) );


    /**
     * Input Argument Count 설정
     */

    sAggrExpr = aAggrOptInfo->mAggrExpr->mExpr.mAggregation;
    sContexts = aDataArea->mExprDataContext->mContexts;

    STL_DASSERT( sAggrExpr->mArgCount == 1 );    
    aAggrExecInfo->mInputArgCnt = sAggrExpr->mArgCount;
    
    /* value block 간의 연결 */
    sOffset = sAggrExpr->mArgs[ 0 ]->mOffset;

    STL_DASSERT( sContexts[ sOffset ].mCast == NULL );

    switch( sAggrExpr->mArgs[ 0 ]->mType )
    {
        case QLV_EXPR_TYPE_VALUE :
        case QLV_EXPR_TYPE_BIND_PARAM :
        case QLV_EXPR_TYPE_PSEUDO_COLUMN :
        case QLV_EXPR_TYPE_COLUMN :
        case QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT :
        case QLV_EXPR_TYPE_REFERENCE :
        case QLV_EXPR_TYPE_INNER_COLUMN :
        case QLV_EXPR_TYPE_OUTER_COLUMN :
        case QLV_EXPR_TYPE_ROWID_COLUMN :
        case QLV_EXPR_TYPE_AGGREGATION :
        case QLV_EXPR_TYPE_ROW_EXPR:
            {
                /**
                 * 연관된 Value Block이 있고, Internal Cast가 없으면 Expression에 대한 Value Block을 연결
                 */
        
                STL_TRY( knlInitShareBlockFromBlock(
                             & aAggrExecInfo->mInputArgList,
                             sContexts[ sOffset ].mInfo.mValueInfo,
                             aRegionMem,
                             KNL_ENV( aEnv ) )
                         == STL_SUCCESS );
                break;
            }
        case QLV_EXPR_TYPE_FUNCTION :
        case QLV_EXPR_TYPE_CAST :
        case QLV_EXPR_TYPE_CASE_EXPR :
            {
                /**
                 * 새로운 Value Block 할당
                 */

                sDataType = sContexts[ sOffset ].mInfo.mFuncData->mResultValue.mType;

                STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                            & aAggrExecInfo->mInputArgList,
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
                break;
            }
        case QLV_EXPR_TYPE_LIST_FUNCTION:
        case QLV_EXPR_TYPE_LIST_COLUMN:
            {
                /**
                 * 새로운 Value Block 할당
                 */

                sDataType = sContexts[ sOffset ].mInfo.mListFunc->mResultValue.mType;

                STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                            & aAggrExecInfo->mInputArgList,
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
                break;
            }
        case QLV_EXPR_TYPE_SUB_QUERY :
        case QLV_EXPR_TYPE_PSEUDO_ARGUMENT :
        default :
            {
                STL_DASSERT( 0 );
                break;
            }
    }

    
    /**
     * Aggregation Value Block List 찾기
     */

    STL_TRY( knlInitShareBlockFromBlock(
                 & aAggrExecInfo->mAggrValueList,
                 sContexts[ aAggrOptInfo->mAggrExpr->mOffset ].mInfo.mValueInfo,
                 aRegionMem,
                 KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    
    /**
     * Argument Expression을 평가하기 위한 Block Eval Info 설정
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( knlExprEvalInfo * ),
                                (void **) & aAggrExecInfo->mEvalInfo,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    if( aAggrOptInfo->mArgCodeStack[ 0 ] == NULL )
    {
        aAggrExecInfo->mEvalInfo[ 0 ] = NULL;
    }
    else
    {
        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( knlExprEvalInfo ),
                                    (void **) & sEvalInfo,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemset( sEvalInfo, 0x00, STL_SIZEOF( knlExprEvalInfo ) );

        sEvalInfo->mExprStack       = aAggrOptInfo->mArgCodeStack[ 0 ];
        sEvalInfo->mContext         = aDataArea->mExprDataContext;
        sEvalInfo->mResultBlock     = aAggrExecInfo->mInputArgList;
        sEvalInfo->mBlockIdx        = 0;
        sEvalInfo->mBlockCount      = 0;

        aAggrExecInfo->mEvalInfo[ 0 ] = sEvalInfo;
    }   
    
    
    /**
     * Optimization 정보 설정
     */
    
    aAggrExecInfo->mAggrOptInfo = aAggrOptInfo;


    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Aggregation을 초기화한다.
 * @param[in]      aAggrExecInfo   Aggregation Execution 정보
 * @param[in]      aBlockIdx       Block Idx
 * @param[in]      aEnv            Environment
 */
stlStatus qlnfMaxInit( qlnfAggrExecInfo   * aAggrExecInfo,
                       stlInt32             aBlockIdx,
                       qllEnv             * aEnv )
{
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aBlockIdx >= 0, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mInputArgList != NULL, QLL_ERROR_STACK( aEnv ) );

    
    /**
     * Aggregation Value Block List 초기화
     */

    STL_DASSERT( aAggrExecInfo->mAggrValueList->mNext == NULL );
    DTL_SET_NULL( KNL_GET_BLOCK_DATA_VALUE( aAggrExecInfo->mAggrValueList, aBlockIdx ) );
    KNL_SET_ONE_BLOCK_USED_CNT( aAggrExecInfo->mAggrValueList, aBlockIdx + 1 );
        

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Aggregation을 수행한다.
 * @param[in]      aAggrExecInfo   Aggregation Execution 정보
 * @param[in]      aStartIdx       Start Block Idx
 * @param[in]      aBlockCnt       Block Used Count
 * @param[in]      aResultBlockIdx Result Block Idx
 * @param[in]      aEnv            Environment
 */
stlStatus qlnfMaxBuild( qlnfAggrExecInfo   * aAggrExecInfo,
                        stlInt32             aStartIdx,
                        stlInt32             aBlockCnt,
                        stlInt32             aResultBlockIdx,
                        qllEnv             * aEnv )
{
    stlInt32                sLoop        = 0;
    dtlBooleanType          sBoolean;
    dtlDataValue            sTempValue   = { DTL_TYPE_BOOLEAN, STL_SIZEOF(dtlBooleanType), 0, & sBoolean };
    dtlDataValue          * sDataValue   = NULL;
    dtlDataValue          * sResultValue = NULL;
    dtlDataValue          * sCompResult  = NULL;
    knlExprEvalInfo       * sEvalInfo    = NULL;
    dtlBuiltInFuncPtr       sCompFunc    = NULL;
    dtlDataType             sDataType    = DTL_TYPE_NA;
    dtlValueEntry           sValueEntry[ 2 ];
    
    
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mInputArgList != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aStartIdx >= 0, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aBlockCnt > 0, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aResultBlockIdx >= 0, QLL_ERROR_STACK( aEnv ) );
    

    /**
     * Aggregation내 Expression Stack 수행
     */

    sEvalInfo = aAggrExecInfo->mEvalInfo[ 0 ];
    if( sEvalInfo == NULL )
    {
        /* Do Nothing */
    }
    else
    {
        sEvalInfo->mBlockIdx   = aStartIdx;
        sEvalInfo->mBlockCount = aBlockCnt;
        STL_TRY( knlEvaluateBlockExpression( sEvalInfo,
                                             KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
        
        
    /**
     * MAX 연산 수행
     */

    sDataValue   = KNL_GET_BLOCK_DATA_VALUE( aAggrExecInfo->mInputArgList, aStartIdx );
    sResultValue = KNL_GET_BLOCK_DATA_VALUE( aAggrExecInfo->mAggrValueList, aResultBlockIdx );
    sCompResult  = & sTempValue;
    sDataType    = sDataValue->mType;
    sCompFunc    = dtlCompOperArg2FuncList[ sDataType ][ sDataType ][ DTL_COMP_OPER_IS_GREATER_THAN ];
        
    STL_DASSERT( sCompFunc != NULL );

    if( KNL_GET_BLOCK_IS_SEP_BUFF( aAggrExecInfo->mInputArgList ) == STL_TRUE )
    {
        sValueEntry[1].mValue.mDataValue = sResultValue;
            
        for( sLoop = 0 ; sLoop < aBlockCnt ; sLoop++ )
        {
            if( DTL_IS_NULL( sDataValue ) )
            {
                /* Do Nothing */
            }
            else
            {
                if( DTL_IS_NULL( sResultValue ) )
                {
                    STL_TRY( knlCopyDataValue( sDataValue,
                                               sResultValue,
                                               KNL_ENV(aEnv) )
                             == STL_SUCCESS );
                }
                else
                {
                    sValueEntry[0].mValue.mDataValue = sDataValue;
                
                    STL_TRY( sCompFunc( 2,
                                        sValueEntry,
                                        sCompResult,
                                        NULL,
                                        NULL,
                                        NULL,
                                        aEnv )
                             == STL_SUCCESS );

                    if( DTL_BOOLEAN_IS_TRUE( sCompResult ) )
                    {
                        STL_TRY( knlCopyDataValue( sDataValue,
                                                   sResultValue,
                                                   KNL_ENV(aEnv) )
                                 == STL_SUCCESS );
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }
            }
                
            sDataValue++;
        }
    }
    else
    {
        if( DTL_IS_NULL( sDataValue ) )
        {
            /* Do Nothing */
        }
        else
        {
            if( DTL_IS_NULL( sResultValue ) )
            {
                STL_TRY( knlCopyDataValue( sDataValue,
                                           sResultValue,
                                           KNL_ENV(aEnv) )
                         == STL_SUCCESS );
            }
            else
            {
                /* Do Nothing */
            }
        }
    }
    
        
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/** @} qlnfAggr */
