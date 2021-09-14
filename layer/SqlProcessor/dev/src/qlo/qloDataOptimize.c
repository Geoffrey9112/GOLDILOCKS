/*******************************************************************************
 * qloDataOptimize.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qloDataOptimize.c 9358 2013-08-14 05:07:29Z bsyou $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file qloDataOptimize.c
 * @brief SQL Processor Layer Data Optimization
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @addtogroup qlo
 * @{
 */

const qloDataOptimizeExprFuncPtr qloDataOptExprFuncList[ QLV_EXPR_TYPE_MAX ] =
{
    qloDataOptimizeValue,         /* QLV_EXPR_TYPE_VALUE */
    qloDataOptimizeBindParam,     /* QLV_EXPR_TYPE_BIND_PARAM */
    qloDataOptimizeColumn,        /* QLV_EXPR_TYPE_COLUMN */
    qloDataOptimizeFunction,      /* QLV_EXPR_TYPE_FUNCTION */
    qloDataOptimizeFunction,      /* QLV_EXPR_TYPE_CAST */
    qloDataOptimizePseudoCol,     /* QLV_EXPR_TYPE_PSEUDO_COLUMN */
    qloDataOptimizePseudoArg,     /* QLV_EXPR_TYPE_PSEUDO_ARGUMENT */
    qloDataOptimizeConstExpr,     /* QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT */
    qloDataOptimizeReference,     /* QLV_EXPR_TYPE_REFERENCE */
    qloDataOptimizeSubQuery,      /* QLV_EXPR_TYPE_SUB_QUERY */
    qloDataOptimizeInnerColumn,   /* QLV_EXPR_TYPE_INNER_COLUMN */
    qloDataOptimizeOuterColumn,   /* QLV_EXPR_TYPE_OUTER_COLUMN */
    qloDataOptimizeRowIdColumn,   /* QLV_EXPR_TYPE_ROWID_COLUMN */
    qloDataOptimizeAggregation,   /* QLV_EXPR_TYPE_AGGREGATION */
    qloDataOptimizeFunction,      /* QLV_EXPR_TYPE_CASE_EXPR */
    qloDataOptimizeListFunction,  /* QLV_EXPR_TYPE_LIST_FUNCTION */
    qloDataOptimizeListColumn,    /* QLV_EXPR_TYPE_LIST_COLUMN */
    qloDataOptimizeRowExpr        /* QLV_EXPR_TYPE_ROW_EXPR */
};


/**
 * @brief 모든 Expression 의 Data Optimization을 수행한다.
 * @param[in]  aDataOptInfo    Data Optimization Info
 * @param[in]  aEnv            Environment
 * @remarks Column, Inner Column, Pseudo Column, RowID Column에 대한
 *     <BR> data optimization이 선행되어야 한다.
 */
stlStatus qloDataOptimizeExpressionAll( qloDataOptExprInfo  * aDataOptInfo,
                                        qllEnv              * aEnv )
{
    qlvRefExprItem      * sExprItem         = NULL;        
    stlInt32              sExprCount        = 0;
    stlInt32              sLoop             = 0;
    

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDataOptInfo != NULL, QLL_ERROR_STACK(aEnv) );

    sExprItem  = aDataOptInfo->mStmtExprList->mAllExprList->mHead;
    sExprCount = aDataOptInfo->mStmtExprList->mTotalExprInfo->mExprCnt;
        
    for( sLoop = 0 ; sLoop < sExprCount ; sLoop++ )
    {
        STL_DASSERT( sExprItem->mExprPtr->mOffset == sLoop );
        
        STL_TRY( qloDataOptimizeExpression( aDataOptInfo,
                                            sExprItem->mExprPtr,
                                            aEnv )
                 == STL_SUCCESS );
        
        sExprItem = sExprItem->mNext;
    }
    

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Expression 의 Data Optimization을 수행한다.
 * @param[in]  aDataOptInfo    Data Optimization Info
 * @param[in]  aInitExpr       Expression
 * @param[in]  aEnv            Environment
 * @remarks
 */
stlStatus qloDataOptimizeExpression( qloDataOptExprInfo  * aDataOptInfo,
                                     qlvInitExpression   * aInitExpr,
                                     qllEnv              * aEnv )
{
    qloDBType           * sDBTypes          = NULL;
    knlExprContext      * sContexts         = NULL;
    
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDataOptInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Data Optimize
     */

    sContexts = QLO_GET_CONTEXT( aDataOptInfo, aInitExpr->mOffset );
    
    if( sContexts->mInfo.mValueInfo == NULL )
    {
        /**
         * Expression에 대한 Value Block 구성
         */
        
        STL_TRY( qloDataOptExprFuncList[ aInitExpr->mType ] ( aDataOptInfo,
                                                              aInitExpr,
                                                              aEnv )
                 == STL_SUCCESS );

        
        /**
         * internal cast 정보 설정
         */

        sDBTypes = QLO_GET_DB_TYPE( aDataOptInfo, aInitExpr->mOffset );

        STL_TRY( qloDataSetInternalCastInfo( aDataOptInfo->mSQLStmt,
                                             aDataOptInfo->mRegionMem,
                                             sDBTypes,
                                             & sContexts->mCast,
                                             STL_TRUE, /* aIsAllocCastFuncInfo */
                                             STL_TRUE, /* aIsAllocCastResultBuf */
                                             aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /* Do Nothing */
    }

    
    return STL_SUCCESS;
    
    STL_FINISH;

    if( aDataOptInfo->mSQLStmt != NULL )
    {
        if( aDataOptInfo->mSQLStmt->mRetrySQL != NULL )
        {
            stlChar * sLastError = NULL;
            sLastError = stlGetLastDetailErrorMessage( QLL_ERROR_STACK(aEnv) );
        
            if ( sLastError[0] == '\0' )
            {
                stlSetLastDetailErrorMessage( QLL_ERROR_STACK(aEnv),
                                              qlgMakeErrPosString( aDataOptInfo->mSQLStmt->mRetrySQL,
                                                                   aInitExpr->mPosition,
                                                                   aEnv ) );
            }
        }
    }

    return STL_FAILURE;
}


/**
 * @brief Value Expression 의 Data Area 를 최적화한다.
 * @param[in]  aDataOptInfo    Data Optimization Info
 * @param[in]  aInitExpr       Expression
 * @param[in]  aEnv            Environment
 * @remarks
 */
stlStatus qloDataOptimizeValue( qloDataOptExprInfo  * aDataOptInfo,
                                qlvInitExpression   * aInitExpr,
                                qllEnv              * aEnv )
{
    qlvInitValue              * sValueCode     = NULL;
    qloDBType                 * sDBTypes       = NULL;
    dtlDataValue              * sDataValue     = NULL;
    knlValueBlockList         * sBlockList     = NULL;
    const dtlDataTypeDefInfo  * sTypeInfo      = NULL;
    
    stlInt64                    sArg1 = 0;
    dtlStringLengthUnit         sLenUnit;
    stlBool                     sIsValid = STL_FALSE;
    

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDataOptInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExpr->mType == QLV_EXPR_TYPE_VALUE,
                        QLL_ERROR_STACK(aEnv) );
    

    /**
     * Data Value 설정
     */
                    
    sValueCode = aInitExpr->mExpr.mValue;
    sDBTypes   = QLO_GET_DB_TYPE( aDataOptInfo, aInitExpr->mOffset );

    if( sValueCode->mInternalBindIdx != QLV_INTERNAL_BIND_VARIABLE_IDX_NA )
    {
        STL_TRY( qloSetNullDataValue(
                     & sDataValue,
                     sDBTypes->mDBType,
                     sDBTypes->mArgNum1,
                     sDBTypes->mStringLengthUnit,
                     aDataOptInfo->mRegionMem,
                     aEnv )
                 == STL_SUCCESS );
                                
        STL_TRY( knlInitBlockWithDataValue(
                     & sBlockList,
                     sDataValue,
                     sDBTypes->mDBType,
                     STL_LAYER_SQL_PROCESSOR,
                     sDBTypes->mArgNum1,
                     sDBTypes->mArgNum2,
                     sDBTypes->mStringLengthUnit,
                     sDBTypes->mIntervalIndicator,
                     aDataOptInfo->mRegionMem,
                     KNL_ENV( aEnv ) )
                 == STL_SUCCESS );

        STL_THROW( RAMP_FINISH );
    }

    switch( sValueCode->mValueType )
    {
        case QLV_VALUE_TYPE_NULL :
            {
                /**
                 * Value Block List 생성
                 */

                STL_TRY( qloSetNullDataValue(
                             & sDataValue,
                             sDBTypes->mDBType,
                             sDBTypes->mArgNum1,
                             sDBTypes->mStringLengthUnit,
                             aDataOptInfo->mRegionMem,
                             aEnv )
                         == STL_SUCCESS );
                                
                STL_TRY( knlInitBlockWithDataValue(
                             & sBlockList,
                             sDataValue,
                             sDBTypes->mDBType,
                             STL_LAYER_SQL_PROCESSOR,
                             sDBTypes->mArgNum1,
                             sDBTypes->mArgNum2,
                             sDBTypes->mStringLengthUnit,
                             sDBTypes->mIntervalIndicator,
                             aDataOptInfo->mRegionMem,
                             KNL_ENV( aEnv ) )
                         == STL_SUCCESS );

                break;
            }
        case QLV_VALUE_TYPE_BOOLEAN :
        case QLV_VALUE_TYPE_BINARY_INTEGER :
            {
                /**
                 * Value Block List 생성
                 */

                sTypeInfo = & gResultDataTypeDef[ sDBTypes->mSourceDBType ];
                
                STL_TRY( qloSetDataValueFromInteger(
                             aDataOptInfo->mSQLStmt,
                             & sDataValue,
                             sDBTypes->mSourceDBType,
                             sTypeInfo->mArgNum1,
                             sTypeInfo->mArgNum2,
                             sValueCode->mData.mInteger,
                             aDataOptInfo->mRegionMem,
                             aEnv )
                         == STL_SUCCESS );

                STL_TRY( knlInitBlockWithDataValue(
                             & sBlockList,
                             sDataValue,
                             sDBTypes->mSourceDBType,
                             STL_LAYER_SQL_PROCESSOR,
                             sTypeInfo->mArgNum1,
                             sTypeInfo->mArgNum2,
                             sTypeInfo->mStringLengthUnit,
                             sTypeInfo->mIntervalIndicator,
                             aDataOptInfo->mRegionMem,
                             KNL_ENV( aEnv ) )
                         == STL_SUCCESS );

                break;
            }
        case QLV_VALUE_TYPE_NUMERIC :
        case QLV_VALUE_TYPE_BINARY_DOUBLE :
        case QLV_VALUE_TYPE_BINARY_REAL :
        case QLV_VALUE_TYPE_INTERVAL_YEAR_TO_MONTH_STRING :
        case QLV_VALUE_TYPE_INTERVAL_DAY_TO_SECOND_STRING :
        case QLV_VALUE_TYPE_ROWID_STRING :
            {
                /**
                 * Value Block List 생성
                 */

                sTypeInfo = &gResultDataTypeDef[sDBTypes->mSourceDBType];
                STL_TRY( qloSetDataValueFromString(
                             aDataOptInfo->mSQLStmt,
                             & sDataValue,
                             sDBTypes->mSourceDBType,
                             sTypeInfo->mArgNum1,
                             sTypeInfo->mArgNum2,
                             sTypeInfo->mStringLengthUnit,
                             sTypeInfo->mIntervalIndicator,
                             sValueCode->mData.mString,
                             aDataOptInfo->mRegionMem,
                             aEnv )
                         == STL_SUCCESS );

                STL_TRY( knlInitBlockWithDataValue(
                             & sBlockList,
                             sDataValue,
                             sDBTypes->mSourceDBType,
                             STL_LAYER_SQL_PROCESSOR,
                             sTypeInfo->mArgNum1,
                             sTypeInfo->mArgNum2,
                             sTypeInfo->mStringLengthUnit,
                             sTypeInfo->mIntervalIndicator,
                             aDataOptInfo->mRegionMem,
                             KNL_ENV( aEnv ) )
                         == STL_SUCCESS );

                break;
            }
        case QLV_VALUE_TYPE_DATE_LITERAL_STRING :
        case QLV_VALUE_TYPE_TIME_LITERAL_STRING :
        case QLV_VALUE_TYPE_TIME_TZ_LITERAL_STRING :
        case QLV_VALUE_TYPE_TIMESTAMP_LITERAL_STRING :
        case QLV_VALUE_TYPE_TIMESTAMP_TZ_LITERAL_STRING :
            {
                /**
                 * Value Block List 생성
                 */

                sTypeInfo = &gResultDataTypeDef[sDBTypes->mSourceDBType];
                STL_TRY( qloSetDataValueFromDateTimeLiteral(
                             aDataOptInfo->mSQLStmt,
                             & sDataValue,
                             sDBTypes->mSourceDBType,
                             sTypeInfo->mArgNum1,
                             sTypeInfo->mArgNum2,
                             sTypeInfo->mStringLengthUnit,
                             sTypeInfo->mIntervalIndicator,
                             sValueCode->mData.mString,
                             aDataOptInfo->mRegionMem,
                             aEnv )
                         == STL_SUCCESS );

                STL_TRY( knlInitBlockWithDataValue(
                             & sBlockList,
                             sDataValue,
                             sDBTypes->mSourceDBType,
                             STL_LAYER_SQL_PROCESSOR,
                             sTypeInfo->mArgNum1,
                             sTypeInfo->mArgNum2,
                             sTypeInfo->mStringLengthUnit,
                             sTypeInfo->mIntervalIndicator,
                             aDataOptInfo->mRegionMem,
                             KNL_ENV( aEnv ) )
                         == STL_SUCCESS );

                break;
            }
        case QLV_VALUE_TYPE_STRING_FIXED :
        case QLV_VALUE_TYPE_STRING_VARYING :
            {
                /**
                 * Value Block List 생성
                 */

                sTypeInfo = &gResultDataTypeDef[sDBTypes->mSourceDBType];
                                    
                sArg1 = stlStrlen( sValueCode->mData.mString );
                                    
                if( sArg1 == 0 )
                {
                    sArg1 = 1;                                        
                }
                else
                {                                    
                    STL_TRY( dtlGetMbstrlenWithLen( (KNL_DT_VECTOR(aEnv))->mGetCharSetIDFunc(aEnv),
                                                    sValueCode->mData.mString,
                                                    sArg1,
                                                    & sArg1,
                                                    KNL_DT_VECTOR(aEnv),
                                                    aEnv,
                                                    QLL_ERROR_STACK(aEnv) )
                             == STL_SUCCESS );
                }
                
                sLenUnit = DTL_STRING_LENGTH_UNIT_CHARACTERS;
                                    
                STL_TRY( qloSetDataValueFromString(
                             aDataOptInfo->mSQLStmt,
                             & sDataValue,
                             sDBTypes->mSourceDBType,
                             sArg1,
                             sTypeInfo->mArgNum2,
                             sLenUnit,
                             sTypeInfo->mIntervalIndicator,
                             sValueCode->mData.mString,
                             aDataOptInfo->mRegionMem,
                             aEnv )
                         == STL_SUCCESS );

                STL_TRY( knlInitBlockWithDataValue(
                             & sBlockList,
                             sDataValue,
                             sDBTypes->mSourceDBType,
                             STL_LAYER_SQL_PROCESSOR,
                             sArg1,
                             sTypeInfo->mArgNum2,
                             sLenUnit,
                             sTypeInfo->mIntervalIndicator,
                             aDataOptInfo->mRegionMem,
                             KNL_ENV( aEnv ) )
                         == STL_SUCCESS );

                break;
            }
        case QLV_VALUE_TYPE_BITSTRING_FIXED :
        case QLV_VALUE_TYPE_BITSTRING_VARYING :
            {
                /**
                 * Value Block List 생성
                 */
                                    
                sTypeInfo = &gResultDataTypeDef[sDBTypes->mSourceDBType];
                                    
                sArg1 = stlStrlen( sValueCode->mData.mString );
                                    
                if( sArg1 == 0 )
                {
                    sArg1 = 1;
                }
                else
                {
                    STL_TRY( dtlValidateBitString( sValueCode->mData.mString,
                                                   & sIsValid,
                                                   & sArg1,
                                                   QLL_ERROR_STACK(aEnv) )
                             == STL_SUCCESS );
                                        
                    sArg1 = STL_MAX( sArg1, 1 );
                    
                    STL_DASSERT( sIsValid == STL_TRUE );
                }
                                    
                STL_TRY( qloSetDataValueFromString(
                             aDataOptInfo->mSQLStmt,
                             & sDataValue,
                             sDBTypes->mSourceDBType,
                             sArg1,
                             sTypeInfo->mArgNum2,
                             sTypeInfo->mStringLengthUnit,
                             sTypeInfo->mIntervalIndicator,
                             sValueCode->mData.mString,
                             aDataOptInfo->mRegionMem,
                             aEnv )
                         == STL_SUCCESS );
                                    
                STL_TRY( knlInitBlockWithDataValue(
                             & sBlockList,
                             sDataValue,
                             sDBTypes->mSourceDBType,
                             STL_LAYER_SQL_PROCESSOR,
                             sArg1,
                             sTypeInfo->mArgNum2,
                             sTypeInfo->mStringLengthUnit,
                             sTypeInfo->mIntervalIndicator,
                             aDataOptInfo->mRegionMem,
                             KNL_ENV( aEnv ) )
                         == STL_SUCCESS );
                                    
                break;
            }
        default :
            {
                STL_DASSERT( 0 );
                break;
            }
    }

    STL_RAMP( RAMP_FINISH );
        
    QLO_GET_CONTEXT( aDataOptInfo, aInitExpr->mOffset )->mInfo.mValueInfo =
        sBlockList->mValueBlock;
    
    sBlockList->mValueBlock->mUsedBlockCnt = sBlockList->mValueBlock->mAllocBlockCnt;
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Bind Parameter Expression 의 Data Area 를 최적화한다.
 * @param[in]  aDataOptInfo    Data Optimization Info
 * @param[in]  aInitExpr       Expression
 * @param[in]  aEnv            Environment
 * @remarks
 */
stlStatus qloDataOptimizeBindParam( qloDataOptExprInfo  * aDataOptInfo,
                                    qlvInitExpression   * aInitExpr,
                                    qllEnv              * aEnv )
{
    qlvInitBindParam          * sBindParamCode = NULL;
    knlValueBlockList         * sBlockList     = NULL;
    

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDataOptInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExpr->mType == QLV_EXPR_TYPE_BIND_PARAM,
                        QLL_ERROR_STACK(aEnv) );
    

    /**
     * Data Value 설정
     */
                    
    sBindParamCode = aInitExpr->mExpr.mBindParam;

    STL_TRY( knlGetExecuteParamValueBlock( aDataOptInfo->mBindContext,
                                           sBindParamCode->mBindParamIdx + 1,
                                           & sBlockList,
                                           KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );
    STL_DASSERT( sBlockList != NULL );
    
    QLO_GET_CONTEXT( aDataOptInfo, aInitExpr->mOffset )->mInfo.mValueInfo =
        sBlockList->mValueBlock;
    
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Column Expression 의 Data Area 를 최적화한다.
 * @param[in]  aDataOptInfo    Data Optimization Info
 * @param[in]  aInitExpr       Expression
 * @param[in]  aEnv            Environment
 * @remarks
 */
stlStatus qloDataOptimizeColumn( qloDataOptExprInfo  * aDataOptInfo,
                                 qlvInitExpression   * aInitExpr,
                                 qllEnv              * aEnv )
{
    qlvInitColumn             * sColumnCode    = NULL;
    knlValueBlockList         * sBlockList     = NULL;
    

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDataOptInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExpr->mType == QLV_EXPR_TYPE_COLUMN,
                        QLL_ERROR_STACK(aEnv) );
    
    
    /**
     * Column의 Value Block을 context로 설정
     */
                    
    sColumnCode = aInitExpr->mExpr.mColumn;

    STL_DASSERT( sColumnCode->mRelationNode != NULL );

    if( sColumnCode->mRelationNode == aDataOptInfo->mWriteRelationNode )
    {
        /**
         * Write Column List에서 검색
         */

        STL_TRY( qlndFindColumnBlockList( aDataOptInfo->mDataArea,
                                          aDataOptInfo->mDataArea->mWriteColumnBlock,
                                          aInitExpr,
                                          aDataOptInfo->mRegionMem,
                                          & sBlockList,
                                          aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /**
         * Read Column List에서 검색
         */

        STL_TRY( qlndFindColumnBlockList( aDataOptInfo->mDataArea,
                                          aDataOptInfo->mDataArea->mReadColumnBlock,
                                          aInitExpr,
                                          aDataOptInfo->mRegionMem,
                                          & sBlockList,
                                          aEnv )
                 == STL_SUCCESS );
    }
                            
    STL_DASSERT( sBlockList != NULL );
    STL_DASSERT( sBlockList->mValueBlock->mIsSepBuff == STL_TRUE );

    
    QLO_GET_CONTEXT( aDataOptInfo, aInitExpr->mOffset )->mInfo.mValueInfo =
        sBlockList->mValueBlock;
    
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Function Expression 의 Data Area 를 최적화한다.
 * @param[in]  aDataOptInfo    Data Optimization Info
 * @param[in]  aInitExpr       Expression
 * @param[in]  aEnv            Environment
 * @remarks
 */
stlStatus qloDataOptimizeFunction( qloDataOptExprInfo  * aDataOptInfo,
                                   qlvInitExpression   * aInitExpr,
                                   qllEnv              * aEnv )
{
    qlvInitExpression         * sExprCode           = NULL;
    knlFunctionData           * sFuncData           = NULL;
    qloFunctionInfo           * sExprFunc           = NULL;
    knlValueBlockList         * sFuncValueBlock     = NULL;
    qloDBType                 * sDBTypes            = NULL;
    
    dtlDataTypeInfo           * sDtlDataTypeInfo    = NULL;
    qloDBType                 * sSrcDBTypes         = NULL;
    

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDataOptInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aInitExpr->mType == QLV_EXPR_TYPE_FUNCTION ) ||
                        ( aInitExpr->mType == QLV_EXPR_TYPE_CAST ) ||
                        ( aInitExpr->mType == QLV_EXPR_TYPE_CASE_EXPR ),
                        QLL_ERROR_STACK(aEnv) );
    

    /**
     * Data Value 설정
     */

    sDBTypes  = QLO_GET_DB_TYPE( aDataOptInfo, aInitExpr->mOffset );
    sExprFunc = & aDataOptInfo->mStmtExprList->mTotalExprInfo->mExprFunc[ aInitExpr->mOffset ];
    

    /**
     * Function Context 공간 할당
     */

    STL_TRY( knlAllocRegionMem( aDataOptInfo->mRegionMem,
                                STL_SIZEOF( knlFunctionData ),
                                (void **) & sFuncData,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sFuncData, 0x00, STL_SIZEOF( knlFunctionData ) );
    
                        
    /**
     * Function Pointer 설정
     */
                        
    STL_TRY( gBuiltInFuncInfoArr[sExprFunc->mFuncID].mGetFuncPointer(
                 sExprFunc->mFuncIdx,
                 & sFuncData->mFuncPtr,
                 QLL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    if( sExprFunc->mFuncID == KNL_BUILTIN_FUNC_TO_CHAR )
    {
        /*
         * src value의 data type info 정보를 구한다.
         * 예) to_char( date, format str )
         *             ^^^^^^
         */
        
        sSrcDBTypes = QLO_GET_DB_TYPE( aDataOptInfo, aInitExpr->mExpr.mFunction->mArgs[0]->mOffset );
        
        /* src value의 data type info 정보를 KNL_BUILTIN_FUNC_TO_CHAR 함수에서 참조할 수 있도록 설정. */
        
        STL_TRY( knlAllocRegionMem( aDataOptInfo->mRegionMem,
                                    STL_SIZEOF( dtlDataTypeInfo ),
                                    (void **) & sFuncData->mInfo,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( sFuncData->mInfo, 0x00, STL_SIZEOF( dtlDataTypeInfo ) );
        
        sDtlDataTypeInfo = (dtlDataTypeInfo *)(sFuncData->mInfo);
        
        sDtlDataTypeInfo->mDataType = sSrcDBTypes->mDBType;
        sDtlDataTypeInfo->mArgNum1  = sSrcDBTypes->mArgNum1;
        sDtlDataTypeInfo->mArgNum2  = sSrcDBTypes->mArgNum2;

        if( ( sSrcDBTypes->mDBType == DTL_GROUP_CHAR_STRING ) ||
            ( sSrcDBTypes->mDBType == DTL_GROUP_LONGVARCHAR_STRING ) )
        {
            sDtlDataTypeInfo->mArgNum3 = sSrcDBTypes->mStringLengthUnit;
        }
        else if( ( sSrcDBTypes->mDBType == DTL_GROUP_INTERVAL_YEAR_TO_MONTH ) ||
                 ( sSrcDBTypes->mDBType == DTL_GROUP_INTERVAL_DAY_TO_SECOND ) )
        {
            sDtlDataTypeInfo->mArgNum3 = sSrcDBTypes->mIntervalIndicator;
        }
        else
        {
            sDtlDataTypeInfo->mArgNum3 = 0;
        }
    }
    else if( sExprFunc->mFuncID == KNL_BUILTIN_FUNC_LIKE_PATTERN )
    {
        STL_TRY( knlAllocRegionMem( aDataOptInfo->mRegionMem,
                                    STL_SIZEOF( dtlAllocInfo ),
                                    (void **) & sFuncData->mInfo,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( sFuncData->mInfo, 0x00, STL_SIZEOF( dtlAllocInfo ) );
    }
    else if( sExprFunc->mFuncID == KNL_BUILTIN_FUNC_CAST )
    {
        STL_TRY( knlAllocRegionMem( aDataOptInfo->mRegionMem,
                                    STL_SIZEOF( dtlDataTypeInfo ),
                                    (void **) & sFuncData->mInfo,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( sFuncData->mInfo, 0x00, STL_SIZEOF( dtlDataTypeInfo ) );
        
        sExprCode = aInitExpr->mExpr.mTypeCast->mArgs[0];
        
        sSrcDBTypes = QLO_GET_DB_TYPE( aDataOptInfo, sExprCode->mOffset );
        
        sDtlDataTypeInfo = (dtlDataTypeInfo *)(sFuncData->mInfo);
        
        sDtlDataTypeInfo->mDataType = sSrcDBTypes->mDBType;
        sDtlDataTypeInfo->mArgNum1  = sSrcDBTypes->mArgNum1;
        sDtlDataTypeInfo->mArgNum2  = sSrcDBTypes->mArgNum2;

        if( ( sSrcDBTypes->mDBType == DTL_GROUP_CHAR_STRING ) ||
            ( sSrcDBTypes->mDBType == DTL_GROUP_LONGVARCHAR_STRING ) )
        {
            sDtlDataTypeInfo->mArgNum3 = sSrcDBTypes->mStringLengthUnit;
        }
        else if( ( sSrcDBTypes->mDBType == DTL_GROUP_INTERVAL_YEAR_TO_MONTH ) ||
                 ( sSrcDBTypes->mDBType == DTL_GROUP_INTERVAL_DAY_TO_SECOND ) )
        {
            sDtlDataTypeInfo->mArgNum3 = sSrcDBTypes->mIntervalIndicator;
        }
        else
        {
            sDtlDataTypeInfo->mArgNum3 = 0;
        }
    }
    else if( ( sExprFunc->mFuncID == KNL_BUILTIN_FUNC_GREATEST ) ||
             ( sExprFunc->mFuncID == KNL_BUILTIN_FUNC_LEAST ) )
    {
        /*
         * result type의 type info 정보 정보를 구성해서 함수의 인자로 넘길수 있게 한다.
         */
        
        STL_TRY( knlAllocRegionMem( aDataOptInfo->mRegionMem,
                                    STL_SIZEOF( dtlDataTypeInfo ),
                                    (void **) & sFuncData->mInfo,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( sFuncData->mInfo, 0x00, STL_SIZEOF( dtlDataTypeInfo ) );
        
        sDtlDataTypeInfo = (dtlDataTypeInfo *)(sFuncData->mInfo);
        
        sDtlDataTypeInfo->mDataType = sDBTypes->mDBType;
        sDtlDataTypeInfo->mArgNum1 = sDBTypes->mArgNum1;
        sDtlDataTypeInfo->mArgNum2 = sDBTypes->mArgNum2;
        
        if( ( dtlDataTypeGroup[ sDBTypes->mDBType ] == DTL_GROUP_CHAR_STRING ) ||
            ( dtlDataTypeGroup[ sDBTypes->mDBType ] == DTL_GROUP_LONGVARCHAR_STRING ) )
        {
            sDtlDataTypeInfo->mArgNum3 = sDBTypes->mStringLengthUnit;
        }
        else if( ( dtlDataTypeGroup[ sDBTypes->mDBType ] == DTL_GROUP_INTERVAL_YEAR_TO_MONTH ) ||
                 ( dtlDataTypeGroup[ sDBTypes->mDBType ] == DTL_GROUP_INTERVAL_DAY_TO_SECOND ) )
        {
            sDtlDataTypeInfo->mArgNum3 = sDBTypes->mIntervalIndicator;
        }
        else
        {
            sDtlDataTypeInfo->mArgNum3 = 0;
        }
    }
    else if( sExprFunc->mFuncID == KNL_BUILTIN_FUNC_STATEMENT_VIEW_SCN )
    {
        sFuncData->mInfo = (void *)&(aDataOptInfo->mSQLStmt->mViewSCN);
    }
    else
    {
        switch( sExprFunc->mFuncID )
        {
            case KNL_BUILTIN_FUNC_SYSDATE :
            case KNL_BUILTIN_FUNC_SYSTIME :
            case KNL_BUILTIN_FUNC_SYSTIMESTAMP :
                {
                    if( aDataOptInfo->mDataArea->mSysTimestampTzBlock == NULL )
                    {
                        STL_TRY( qlndInitBlockList( & aDataOptInfo->mSQLStmt->mLongTypeValueList,
                                                    & aDataOptInfo->mDataArea->mSysTimestampTzBlock,
                                                    aDataOptInfo->mDataArea->mBlockAllocCnt,
                                                    STL_FALSE,
                                                    STL_LAYER_SQL_PROCESSOR,
                                                    0,   /* table ID */
                                                    0,   /* column ID */
                                                    DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,
                                                    gResultDataTypeDef[ DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE ].mArgNum1,
                                                    gResultDataTypeDef[ DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE ].mArgNum2,
                                                    gResultDataTypeDef[ DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE ].mStringLengthUnit,
                                                    gResultDataTypeDef[ DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE ].mIntervalIndicator,
                                                    aDataOptInfo->mRegionMem,
                                                    aEnv )
                                 == STL_SUCCESS );
                    }
                    
                    sFuncData->mInfo = (void*)aDataOptInfo->mDataArea->mSysTimestampTzBlock;
                    break;
                }
            case KNL_BUILTIN_FUNC_STATEMENT_DATE :
            case KNL_BUILTIN_FUNC_STATEMENT_TIME :
            case KNL_BUILTIN_FUNC_STATEMENT_TIMESTAMP :
            case KNL_BUILTIN_FUNC_STATEMENT_LOCALTIME :
            case KNL_BUILTIN_FUNC_STATEMENT_LOCALTIMESTAMP :
                {
                    if( aDataOptInfo->mDataArea->mStmtTimestampTzBlock == NULL )
                    {
                        STL_TRY( qlndInitBlockList( & aDataOptInfo->mSQLStmt->mLongTypeValueList,
                                                    & aDataOptInfo->mDataArea->mStmtTimestampTzBlock,
                                                    aDataOptInfo->mDataArea->mBlockAllocCnt,
                                                    STL_FALSE,
                                                    STL_LAYER_SQL_PROCESSOR,
                                                    0,   /* table ID */
                                                    0,   /* column ID */
                                                    DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,
                                                    gResultDataTypeDef[ DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE ].mArgNum1,
                                                    gResultDataTypeDef[ DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE ].mArgNum2,
                                                    gResultDataTypeDef[ DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE ].mStringLengthUnit,
                                                    gResultDataTypeDef[ DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE ].mIntervalIndicator,
                                                    aDataOptInfo->mRegionMem,
                                                    aEnv )
                                 == STL_SUCCESS );
                    }
                    
                    sFuncData->mInfo = (void*)aDataOptInfo->mDataArea->mStmtTimestampTzBlock;
                    break;
                }
            case KNL_BUILTIN_FUNC_TRANSACTION_DATE :
            case KNL_BUILTIN_FUNC_TRANSACTION_TIME :
            case KNL_BUILTIN_FUNC_TRANSACTION_TIMESTAMP :
            case KNL_BUILTIN_FUNC_TRANSACTION_LOCALTIME :
            case KNL_BUILTIN_FUNC_TRANSACTION_LOCALTIMESTAMP :
                {
                    if( aDataOptInfo->mDataArea->mTransactionTimestampTzBlock == NULL )
                    {
                        STL_TRY( qlndInitBlockList( & aDataOptInfo->mSQLStmt->mLongTypeValueList,
                                                    & aDataOptInfo->mDataArea->mTransactionTimestampTzBlock,
                                                    aDataOptInfo->mDataArea->mBlockAllocCnt,
                                                    STL_FALSE,
                                                    STL_LAYER_SQL_PROCESSOR,
                                                    0,   /* table ID */
                                                    0,   /* column ID */
                                                    DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,
                                                    gResultDataTypeDef[ DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE ].mArgNum1,
                                                    gResultDataTypeDef[ DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE ].mArgNum2,
                                                    gResultDataTypeDef[ DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE ].mStringLengthUnit,
                                                    gResultDataTypeDef[ DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE ].mIntervalIndicator,
                                                    aDataOptInfo->mRegionMem,
                                                    aEnv )
                                 == STL_SUCCESS );
                    }
                    
                    sFuncData->mInfo = (void*)aDataOptInfo->mDataArea->mTransactionTimestampTzBlock;
                    break;
                }
            default :
                {
                    sFuncData->mInfo = NULL;
                    break;
                }
        }
    }
    

    /**
     * Statement 단위 Function이면 Statement Function Block List에 Value Block 등록
     */
    
    if( gBuiltInFuncInfoArr[ sExprFunc->mFuncID ].mIterationTime ==
        DTL_ITERATION_TIME_FOR_EACH_STATEMENT )
    {
        /**
         * 동일한 Func ID를 가지는 Value Block 찾기
         */
        
        STL_TRY( qloGetStmtFunctionBlock( aDataOptInfo->mDataArea->mStmtFuncBlock,
                                          sExprFunc->mFuncID,
                                          & sFuncValueBlock,
                                          aEnv )
                 == STL_SUCCESS );

    
        /**
         * Value Block 없으면 Function Value Block 생성하여 리스트 구성
         */

        if( sFuncValueBlock == NULL )
        {
            /**
             * Result Data Value 설정
             */

            STL_DASSERT( (sDBTypes->mSourceDBType != DTL_TYPE_LONGVARCHAR) &&
                         (sDBTypes->mSourceDBType != DTL_TYPE_LONGVARBINARY) );

            STL_TRY( qlndInitSingleDataValue( & aDataOptInfo->mSQLStmt->mLongTypeValueList,
                                              sDBTypes->mSourceDBType,
                                              & sFuncData->mResultValue,
                                              sExprFunc->mResultBufferSize,
                                              aDataOptInfo->mRegionMem,
                                              aEnv )
                     == STL_SUCCESS );

                                
            /**
             * Value Block 설정
             */

            STL_TRY( knlAllocRegionMem( aDataOptInfo->mRegionMem,
                                        STL_SIZEOF( knlValueBlockList ),
                                        (void**) & sFuncValueBlock,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
                                
                                
            /* column ID => Function ID로 사용 */
            stlMemset( sFuncValueBlock, 0x00, STL_SIZEOF( knlValueBlockList ) );
            KNL_SET_BLOCK_COLUMN_ORDER( sFuncValueBlock, sExprFunc->mFuncID );
                                    
            STL_TRY( knlCreateStackBlock( & sFuncValueBlock->mValueBlock,
                                          & sFuncData->mResultValue,
                                          sExprFunc->mResultBufferSize,
                                          sDBTypes->mSourceDBType,
                                          sDBTypes->mSourceArgNum1,
                                          sDBTypes->mSourceArgNum2,
                                          sDBTypes->mSourceStringLengthUnit,
                                          sDBTypes->mSourceIntervalIndicator,
                                          aDataOptInfo->mRegionMem,
                                          KNL_ENV( aEnv ) )
                     == STL_SUCCESS );

            if( aDataOptInfo->mDataArea->mStmtFuncBlock == NULL )
            {
                aDataOptInfo->mDataArea->mStmtFuncBlock = sFuncValueBlock;
            }
            else
            {
                sFuncValueBlock->mNext = aDataOptInfo->mDataArea->mStmtFuncBlock;
                aDataOptInfo->mDataArea->mStmtFuncBlock = sFuncValueBlock;
            }

            if( sFuncData->mInfo == NULL )
            {
                sFuncData->mInfo = (void*) sFuncValueBlock;
            }
        }
        else
        {
            /**
             * Result Data Value 설정
             */

            sFuncData->mResultValue.mValue =
                KNL_GET_BLOCK_FIRST_DATA_VALUE( sFuncValueBlock )->mValue;

            STL_TRY( dtlInitDataValue( KNL_GET_BLOCK_DB_TYPE( sFuncValueBlock ),
                                       KNL_GET_BLOCK_DATA_BUFFER_SIZE( sFuncValueBlock ),
                                       & sFuncData->mResultValue,
                                       QLL_ERROR_STACK( aEnv ) )
                     == STL_SUCCESS );

            if( sFuncData->mInfo == NULL )
            {
                sFuncData->mInfo = (void*) sFuncValueBlock;
            }
        }
    }
    else
    {
        /**
         * Result Data Value 설정 (Buffer 공간 할당)
         */

        STL_TRY( qlndInitSingleDataValue( & aDataOptInfo->mSQLStmt->mLongTypeValueList,
                                          sDBTypes->mSourceDBType,
                                          & sFuncData->mResultValue,
                                          sExprFunc->mResultBufferSize,
                                          aDataOptInfo->mRegionMem,
                                          aEnv )
                 == STL_SUCCESS );
    }

                        
    QLO_GET_CONTEXT( aDataOptInfo, aInitExpr->mOffset )->mInfo.mFuncData = sFuncData;
    
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Pseudo Column Expression 의 Data Area 를 최적화한다.
 * @param[in]  aDataOptInfo    Data Optimization Info
 * @param[in]  aInitExpr       Expression
 * @param[in]  aEnv            Environment
 * @remarks
 */
stlStatus qloDataOptimizePseudoCol( qloDataOptExprInfo  * aDataOptInfo,
                                    qlvInitExpression   * aInitExpr,
                                    qllEnv              * aEnv )
{
    qlvInitPseudoCol          * sPseudoColCode = NULL;
    knlValueBlockList         * sBlockList     = NULL;
    

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDataOptInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExpr->mType == QLV_EXPR_TYPE_PSEUDO_COLUMN,
                        QLL_ERROR_STACK(aEnv) );
    

    /**
     * Pseudo Column의 Value Block 연결
     */
                        
    sPseudoColCode = aInitExpr->mExpr.mPseudoCol;
                        
    STL_TRY( knlGetBlockList( aDataOptInfo->mDataArea->mPseudoColBlock,
                              sPseudoColCode->mPseudoOffset,
                              & sBlockList,
                              KNL_ENV( aEnv ) )
             == STL_SUCCESS );
                        
    STL_DASSERT( sBlockList != NULL );
    
    QLO_GET_CONTEXT( aDataOptInfo, aInitExpr->mOffset )->mInfo.mValueInfo =
        sBlockList->mValueBlock;
    
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Pseudo Argument Expression 의 Data Area 를 최적화한다.
 * @param[in]  aDataOptInfo    Data Optimization Info
 * @param[in]  aInitExpr       Expression
 * @param[in]  aEnv            Environment
 * @remarks
 */
stlStatus qloDataOptimizePseudoArg( qloDataOptExprInfo  * aDataOptInfo,
                                    qlvInitExpression   * aInitExpr,
                                    qllEnv              * aEnv )
{
    return STL_SUCCESS;
}


/**
 * @brief Constant Expression 의 Data Area 를 최적화한다.
 * @param[in]  aDataOptInfo    Data Optimization Info
 * @param[in]  aInitExpr       Expression
 * @param[in]  aEnv            Environment
 * @remarks
 */
stlStatus qloDataOptimizeConstExpr( qloDataOptExprInfo  * aDataOptInfo,
                                    qlvInitExpression   * aInitExpr,
                                    qllEnv              * aEnv )
{
    qlvInitExpression         * sConstExprCode    = NULL;
    qloDBType                 * sDBTypes          = NULL;
    knlValueBlockList         * sFuncValueBlock   = NULL;
    knlValueBlock             * sValueBlock       = NULL;
    knlExprContext            * sConstContext     = NULL;    
    stlInt64                    sBufferLen        = 0;
    

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDataOptInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExpr->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT,
                        QLL_ERROR_STACK(aEnv) );
    
    
    /**
     * context list에서 offset에 해당하는 context를 찾음
     * => context의 function 또는 cast result (dtlDataValue)를
     * 값으로 하는 value block을 구성
     */

    STL_DASSERT( QLO_GET_DB_TYPE( aDataOptInfo, aInitExpr->mOffset )->mIsNeedCast == STL_FALSE );

    sConstExprCode = aInitExpr->mExpr.mConstExpr->mOrgExpr;
    sConstContext  = QLO_GET_CONTEXT( aDataOptInfo, sConstExprCode->mOffset );
    sDBTypes       = QLO_GET_DB_TYPE( aDataOptInfo, sConstExprCode->mOffset );
                        

    /**
     * Constant Expression의 Value Block 구성
     */

    /* data optimize original expression */
    STL_TRY( qloDataOptimizeExpression( aDataOptInfo,
                                        sConstExprCode,
                                        aEnv )
             == STL_SUCCESS );

    /* Value Block 얻기 */
    sValueBlock = NULL;
                        
    if( sConstContext->mCast == NULL )
    {
        switch( sConstExprCode->mType )
        {
            case QLV_EXPR_TYPE_VALUE :
            case QLV_EXPR_TYPE_BIND_PARAM :
            case QLV_EXPR_TYPE_PSEUDO_COLUMN :
                {
                    /* value block 간의 연결 */
                    STL_PARAM_VALIDATE(
                        sConstContext->mInfo.mValueInfo != NULL,
                        QLL_ERROR_STACK(aEnv) );

                    sValueBlock = sConstContext->mInfo.mValueInfo;
                    break;
                }
                                    
            case QLV_EXPR_TYPE_FUNCTION :
            case QLV_EXPR_TYPE_CAST :
            case QLV_EXPR_TYPE_CASE_EXPR :
                {
                    if( sConstExprCode->mType == QLV_EXPR_TYPE_FUNCTION )
                    {
                        if( gBuiltInFuncInfoArr[ sConstExprCode->mExpr.mFunction->mFuncId ].mIterationTime ==
                            DTL_ITERATION_TIME_FOR_EACH_STATEMENT )
                        {
                            /* Statement 단위 Function인 경우 Value Block 만들어서 공유 */
                            STL_TRY( qloGetStmtFunctionBlock( aDataOptInfo->mDataArea->mStmtFuncBlock,
                                                              sConstExprCode->mExpr.mFunction->mFuncId,
                                                              & sFuncValueBlock,
                                                              aEnv )
                                     == STL_SUCCESS );

                            STL_DASSERT( sFuncValueBlock != NULL );
                            STL_DASSERT( sFuncValueBlock->mValueBlock != NULL );
                                            
                            sValueBlock = sFuncValueBlock->mValueBlock;
                                                
                            break;
                        }
                        else
                        {
                            /* Do Nothing */
                        }
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                                        
                    /* result datavalue를 value block에 연결 */
                    STL_PARAM_VALIDATE(
                        sConstContext->mInfo.mFuncData != NULL,
                        QLL_ERROR_STACK(aEnv) );
                    STL_PARAM_VALIDATE(
                        sConstContext->mInfo.mFuncData->mResultValue.mValue != NULL,
                        QLL_ERROR_STACK(aEnv) );

                    /* dtlDataValue의 buffer 공간 크기 */
                    STL_TRY( dtlGetDataValueBufferSize(
                                 sDBTypes->mDBType,
                                 sDBTypes->mArgNum1,
                                 sDBTypes->mStringLengthUnit,
                                 & sBufferLen,
                                 KNL_DT_VECTOR(aEnv),
                                 aEnv,
                                 QLL_ERROR_STACK( aEnv ) )
                             == STL_SUCCESS );

                    sBufferLen = STL_ALIGN_DEFAULT( sBufferLen );

                    /* constant value로 value block 생성 */
                    STL_TRY( knlCreateStackBlock(
                                 & sValueBlock,
                                 & sConstContext->mInfo.mFuncData->mResultValue,
                                 sBufferLen,
                                 sDBTypes->mDBType,
                                 sDBTypes->mArgNum1,
                                 sDBTypes->mArgNum2,
                                 sDBTypes->mStringLengthUnit,
                                 sDBTypes->mIntervalIndicator,
                                 aDataOptInfo->mRegionMem,
                                 KNL_ENV( aEnv ) )
                             == STL_SUCCESS );

                    break;
                }

            case QLV_EXPR_TYPE_LIST_FUNCTION :
                {
                    /* result datavalue를 value block에 연결 */
                    STL_PARAM_VALIDATE(
                        sConstContext->mInfo.mListFunc != NULL,
                        QLL_ERROR_STACK(aEnv) );
                    STL_PARAM_VALIDATE(
                        sConstContext->mInfo.mListFunc->mResultValue.mValue != NULL,
                        QLL_ERROR_STACK(aEnv) );

                    /* dtlDataValue의 buffer 공간 크기 */
                    STL_TRY( dtlGetDataValueBufferSize(
                                 sDBTypes->mDBType,
                                 sDBTypes->mArgNum1,
                                 sDBTypes->mStringLengthUnit,
                                 & sBufferLen,
                                 KNL_DT_VECTOR(aEnv),
                                 aEnv,
                                 QLL_ERROR_STACK( aEnv ) )
                             == STL_SUCCESS );
                                        
                    sBufferLen = STL_ALIGN_DEFAULT( sBufferLen );

                    /* constant value로 value block 생성 */
                    STL_TRY( knlCreateStackBlock(
                                 & sValueBlock,
                                 & sConstContext->mInfo.mListFunc->mResultValue,
                                 sBufferLen,
                                 sDBTypes->mDBType,
                                 sDBTypes->mArgNum1,
                                 sDBTypes->mArgNum2,
                                 sDBTypes->mStringLengthUnit,
                                 sDBTypes->mIntervalIndicator,
                                 aDataOptInfo->mRegionMem,
                                 KNL_ENV( aEnv ) )
                             == STL_SUCCESS );

                    break;
                }
                                        
            case QLV_EXPR_TYPE_LIST_COLUMN :
            case QLV_EXPR_TYPE_ROW_EXPR:
            case QLV_EXPR_TYPE_COLUMN :
            case QLV_EXPR_TYPE_PSEUDO_ARGUMENT :
            case QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT :
            case QLV_EXPR_TYPE_REFERENCE :
            case QLV_EXPR_TYPE_SUB_QUERY :
            case QLV_EXPR_TYPE_INNER_COLUMN :
            case QLV_EXPR_TYPE_OUTER_COLUMN :
            case QLV_EXPR_TYPE_ROWID_COLUMN :
            case QLV_EXPR_TYPE_AGGREGATION :
            default :
                {
                    STL_DASSERT( 0 );
                    break;
                }
        }
    }
    else
    {
        /* internal cast 결과를 value block에 연결 */
        STL_PARAM_VALIDATE(
            sConstContext->mCast->mCastResultBuf.mValue != NULL,
            QLL_ERROR_STACK(aEnv) );

        /* dtlDataValue의 buffer 공간 크기 */
        STL_TRY( dtlGetDataValueBufferSize(
                     sDBTypes->mDBType,
                     sDBTypes->mArgNum1,
                     sDBTypes->mStringLengthUnit,
                     & sBufferLen,
                     KNL_DT_VECTOR(aEnv),
                     aEnv,
                     QLL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );

        sBufferLen = STL_ALIGN_DEFAULT( sBufferLen );

        /* constant value로 value block 생성 */
        STL_TRY( knlCreateStackBlock(
                     & sValueBlock,
                     & sConstContext->mCast->mCastResultBuf,
                     sBufferLen,
                     sDBTypes->mDBType,
                     sDBTypes->mArgNum1,
                     sDBTypes->mArgNum2,
                     sDBTypes->mStringLengthUnit,
                     sDBTypes->mIntervalIndicator,
                     aDataOptInfo->mRegionMem,
                     KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
    }

    QLO_GET_CONTEXT( aDataOptInfo, aInitExpr->mOffset )->mInfo.mValueInfo =
        sValueBlock;
    
    sValueBlock->mUsedBlockCnt = sValueBlock->mAllocBlockCnt;

    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Reference Expression 의 Data Area 를 최적화한다.
 * @param[in]  aDataOptInfo    Data Optimization Info
 * @param[in]  aInitExpr       Expression
 * @param[in]  aEnv            Environment
 * @remarks
 */
stlStatus qloDataOptimizeReference( qloDataOptExprInfo  * aDataOptInfo,
                                    qlvInitExpression   * aInitExpr,
                                    qllEnv              * aEnv )
{
    qlvInitExpression         * sRefExprCode      = NULL;
    qloDBType                 * sDBTypes          = NULL;
    knlValueBlockList         * sBlockList        = NULL;
    /* knlExprContext            * sRefColContext    = NULL; */
    

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDataOptInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExpr->mType == QLV_EXPR_TYPE_REFERENCE,
                        QLL_ERROR_STACK(aEnv) );
    


    /**
     * context list에서 offset에 해당하는 context를 찾음
     * => context의 function 또는 cast result (dtlDataValue)를
     * 값으로 하는 value block을 구성
     */

    sRefExprCode   = aInitExpr->mExpr.mReference->mOrgExpr;
    /* sRefColContext = QLO_GET_CONTEXT( aDataOptInfo, sRefExprCode->mOffset ); */
    sDBTypes       = QLO_GET_DB_TYPE( aDataOptInfo, sRefExprCode->mOffset );


    /**
     * Expression의 Value Block 구성
     */

    /* data optimize original expression */
    STL_TRY( qloDataOptimizeExpression( aDataOptInfo,
                                        sRefExprCode,
                                        aEnv )
             == STL_SUCCESS );

    /* Value Block 얻기 */
    if( aInitExpr->mIterationTime >= DTL_ITERATION_TIME_FOR_EACH_STATEMENT )
    {
        STL_TRY( qlndInitBlockList( & aDataOptInfo->mSQLStmt->mLongTypeValueList,
                                    & sBlockList,
                                    aDataOptInfo->mDataArea->mBlockAllocCnt,
                                    STL_FALSE,
                                    STL_LAYER_SQL_PROCESSOR,
                                    0,   /* table ID */
                                    0,   /* column ID */
                                    sDBTypes->mDBType,
                                    sDBTypes->mArgNum1,
                                    sDBTypes->mArgNum2,
                                    sDBTypes->mStringLengthUnit,
                                    sDBTypes->mIntervalIndicator,
                                    aDataOptInfo->mRegionMem,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_DASSERT( aInitExpr->mIterationTime == DTL_ITERATION_TIME_FOR_EACH_QUERY );
                            
        STL_TRY( qlndInitBlockList( & aDataOptInfo->mSQLStmt->mLongTypeValueList,
                                    & sBlockList,
                                    aDataOptInfo->mDataArea->mBlockAllocCnt,
                                    STL_TRUE,
                                    STL_LAYER_SQL_PROCESSOR,
                                    0,   /* table ID */
                                    0,   /* column ID */
                                    sDBTypes->mDBType,
                                    sDBTypes->mArgNum1,
                                    sDBTypes->mArgNum2,
                                    sDBTypes->mStringLengthUnit,
                                    sDBTypes->mIntervalIndicator,
                                    aDataOptInfo->mRegionMem,
                                    aEnv )
                 == STL_SUCCESS );
    }
    
    QLO_GET_CONTEXT( aDataOptInfo, aInitExpr->mOffset )->mInfo.mValueInfo =
        sBlockList->mValueBlock;
    
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Sub Query Expression 의 Data Area 를 최적화한다.
 * @param[in]  aDataOptInfo    Data Optimization Info
 * @param[in]  aInitExpr       Expression
 * @param[in]  aEnv            Environment
 * @remarks
 */
stlStatus qloDataOptimizeSubQuery( qloDataOptExprInfo  * aDataOptInfo,
                                   qlvInitExpression   * aInitExpr,
                                   qllEnv              * aEnv )
{
    STL_DASSERT( 0 );

    return STL_FAILURE;
}


/**
 * @brief Inner Column Expression 의 Data Area 를 최적화한다.
 * @param[in]  aDataOptInfo    Data Optimization Info
 * @param[in]  aInitExpr       Expression
 * @param[in]  aEnv            Environment
 * @remarks
 */
stlStatus qloDataOptimizeInnerColumn( qloDataOptExprInfo  * aDataOptInfo,
                                      qlvInitExpression   * aInitExpr,
                                      qllEnv              * aEnv )
{
    qlvInitExpression         * sInnerColExprCode = NULL;
    qloDBType                 * sDBTypes          = NULL;
    knlValueBlockList         * sBlockList        = NULL;
    knlValueBlock             * sValueBlock       = NULL;
    knlExprContext            * sInnerColContext  = NULL;    
    stlInt64                    sBufferLen        = 0;
    

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDataOptInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN,
                        QLL_ERROR_STACK(aEnv) );
    

    /**
     * Join Table의 column이라면 Block List가 존재하므로
     * 이를 Value Block에 달아준다.
     */
    
    STL_TRY( qlndFindColumnBlockList( aDataOptInfo->mDataArea,
                                      aDataOptInfo->mDataArea->mReadColumnBlock,
                                      aInitExpr,
                                      aDataOptInfo->mRegionMem,
                                      & sBlockList,
                                      aEnv )
             == STL_SUCCESS );

    
    /**
     * Value Block 설정
     */
    
    if( sBlockList != NULL )
    {
        sValueBlock = sBlockList->mValueBlock;
    }
    else
    {
        /**
         * context list에서 offset에 해당하는 context를 찾음
         * => context의 function 또는 cast result (dtlDataValue)를
         * 값으로 하는 value block을 구성
         */

        sInnerColExprCode = aInitExpr->mExpr.mInnerColumn->mOrgExpr;
        sInnerColContext  = QLO_GET_CONTEXT( aDataOptInfo, sInnerColExprCode->mOffset );
        sDBTypes          = QLO_GET_DB_TYPE( aDataOptInfo, sInnerColExprCode->mOffset );

    
        /**
         * Expression의 Value Block 구성
         */

        /* data optimize original expression */
        STL_TRY( qloDataOptimizeExpression( aDataOptInfo,
                                            sInnerColExprCode,
                                            aEnv )
                 == STL_SUCCESS );

        /* Value Block 얻기 */
        sValueBlock = NULL;

        if( sInnerColContext->mCast == NULL )
        {
            switch( sInnerColExprCode->mType )
            {
                case QLV_EXPR_TYPE_VALUE :
                case QLV_EXPR_TYPE_BIND_PARAM :
                case QLV_EXPR_TYPE_PSEUDO_COLUMN :
                case QLV_EXPR_TYPE_COLUMN :
                case QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT :
                case QLV_EXPR_TYPE_INNER_COLUMN :
                case QLV_EXPR_TYPE_REFERENCE :
                case QLV_EXPR_TYPE_OUTER_COLUMN :
                case QLV_EXPR_TYPE_ROWID_COLUMN :
                case QLV_EXPR_TYPE_AGGREGATION :
                case QLV_EXPR_TYPE_LIST_COLUMN :
                case QLV_EXPR_TYPE_ROW_EXPR :
                    {
                        /* value block 간의 연결 */
                        STL_PARAM_VALIDATE( sInnerColContext->mInfo.mValueInfo != NULL,
                                            QLL_ERROR_STACK(aEnv) );

                        sValueBlock = sInnerColContext->mInfo.mValueInfo;
                        break;
                    }

                case QLV_EXPR_TYPE_FUNCTION :
                case QLV_EXPR_TYPE_CAST :
                case QLV_EXPR_TYPE_CASE_EXPR :
                    {
                        /**
                         * @todo INNER COLUMN의 TABLE ID와 COLUMN ID를 설정
                         * @remark INNER COLUMN 구조체에 TABLE ID와 COLUMN ID 정보 필요
                         */

                        /* value block을 할당 받아 연결 */
                        STL_TRY( qlndInitBlockList( & aDataOptInfo->mSQLStmt->mLongTypeValueList,
                                                    & sBlockList,
                                                    aDataOptInfo->mDataArea->mBlockAllocCnt,
                                                    STL_TRUE,
                                                    STL_LAYER_SQL_PROCESSOR,
                                                    0,   /* table ID */
                                                    0,   /* column ID */
                                                    sDBTypes->mDBType,
                                                    sDBTypes->mArgNum1,
                                                    sDBTypes->mArgNum2,
                                                    sDBTypes->mStringLengthUnit,
                                                    sDBTypes->mIntervalIndicator,
                                                    aDataOptInfo->mRegionMem,
                                                    aEnv )
                                 == STL_SUCCESS );

                        sValueBlock = sBlockList->mValueBlock;
                        break;
                    }

                case QLV_EXPR_TYPE_LIST_FUNCTION :
                    {
                        /**
                         * @todo INNER COLUMN의 TABLE ID와 COLUMN ID를 설정
                         * @remark INNER COLUMN 구조체에 TABLE ID와 COLUMN ID 정보 필요
                         */

                        /* value block을 할당 받아 연결 */
                        STL_TRY( qlndInitBlockList( & aDataOptInfo->mSQLStmt->mLongTypeValueList,
                                                    & sBlockList,
                                                    aDataOptInfo->mDataArea->mBlockAllocCnt,
                                                    STL_TRUE,
                                                    STL_LAYER_SQL_PROCESSOR,
                                                    0,   /* table ID */
                                                    0,   /* column ID */
                                                    sDBTypes->mDBType,
                                                    sDBTypes->mArgNum1,
                                                    sDBTypes->mArgNum2,
                                                    sDBTypes->mStringLengthUnit,
                                                    sDBTypes->mIntervalIndicator,
                                                    aDataOptInfo->mRegionMem,
                                                    aEnv )
                                 == STL_SUCCESS );

                        sValueBlock = sBlockList->mValueBlock;
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
        }
        else
        {
            if ( sInnerColContext->mInfo.mValueInfo->mIsSepBuff == STL_TRUE )
            {
                /* value block을 할당 받아 연결 */
                STL_TRY( qlndInitBlockList( & aDataOptInfo->mSQLStmt->mLongTypeValueList,
                                            & sBlockList,
                                            aDataOptInfo->mDataArea->mBlockAllocCnt,
                                            STL_TRUE,
                                            STL_LAYER_SQL_PROCESSOR,
                                            0,   /* table ID */
                                            0,   /* column ID */
                                            sDBTypes->mDBType,
                                            sDBTypes->mArgNum1,
                                            sDBTypes->mArgNum2,
                                            sDBTypes->mStringLengthUnit,
                                            sDBTypes->mIntervalIndicator,
                                            aDataOptInfo->mRegionMem,
                                            aEnv )
                         == STL_SUCCESS );

                sValueBlock = sBlockList->mValueBlock;
            }
            else
            {
                /* internal cast 결과를 value block에 연결 */
                STL_PARAM_VALIDATE( sInnerColContext->mCast->mCastResultBuf.mValue != NULL,
                                    QLL_ERROR_STACK(aEnv) );
                                    
                /* dtlDataValue의 buffer 공간 크기 */
                STL_TRY( dtlGetDataValueBufferSize( sDBTypes->mDBType,
                                                    sDBTypes->mArgNum1,
                                                    sDBTypes->mStringLengthUnit,
                                                    & sBufferLen,
                                                    KNL_DT_VECTOR(aEnv),
                                                    aEnv,
                                                    QLL_ERROR_STACK( aEnv ) )
                         == STL_SUCCESS );
                                    
                sBufferLen = STL_ALIGN_DEFAULT( sBufferLen );
                                    
                /* constant value로 value block 생성 */
                STL_TRY( knlCreateStackBlock( & sValueBlock,
                                              & sInnerColContext->mCast->mCastResultBuf,
                                              sBufferLen,
                                              sDBTypes->mDBType,
                                              sDBTypes->mArgNum1,
                                              sDBTypes->mArgNum2,
                                              sDBTypes->mStringLengthUnit,
                                              sDBTypes->mIntervalIndicator,
                                              aDataOptInfo->mRegionMem,
                                              KNL_ENV( aEnv ) )
                         == STL_SUCCESS );
            }
        }
    }

    
    QLO_GET_CONTEXT( aDataOptInfo, aInitExpr->mOffset )->mInfo.mValueInfo =
        sValueBlock;
    
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Outer Column Expression 의 Data Area 를 최적화한다.
 * @param[in]  aDataOptInfo    Data Optimization Info
 * @param[in]  aInitExpr       Expression
 * @param[in]  aEnv            Environment
 * @remarks
 */
stlStatus qloDataOptimizeOuterColumn( qloDataOptExprInfo  * aDataOptInfo,
                                      qlvInitExpression   * aInitExpr,
                                      qllEnv              * aEnv )
{
    qlvInitExpression         * sOuterColExprCode = NULL;
    knlValueBlockList         * sBlockList        = NULL;
    knlValueBlock             * sValueBlock       = NULL;
    knlExprContext            * sOuterColContext  = NULL;    

    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDataOptInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExpr->mType == QLV_EXPR_TYPE_OUTER_COLUMN,
                        QLL_ERROR_STACK(aEnv) );
    

    /**
     * context list에서 offset에 해당하는 context를 찾음
     * => context의 function 또는 cast result (dtlDataValue)를
     * 값으로 하는 value block을 구성
     */

    sOuterColExprCode = aInitExpr->mExpr.mOuterColumn->mOrgExpr;
    sOuterColContext  = QLO_GET_CONTEXT( aDataOptInfo, sOuterColExprCode->mOffset );


    /**
     * Expression의 Value Block 구성
     */

    /* data optimize original expression */
    STL_TRY( qloDataOptimizeExpression( aDataOptInfo,
                                        sOuterColExprCode,
                                        aEnv )
             == STL_SUCCESS );

    /* Value Block 얻기 */
    sValueBlock = NULL;
    
    STL_DASSERT( sOuterColContext->mCast == NULL );

    switch( sOuterColExprCode->mType )
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
        case QLV_EXPR_TYPE_LIST_COLUMN :
        case QLV_EXPR_TYPE_ROW_EXPR :
            {
                /* value block 간의 연결 */
                STL_PARAM_VALIDATE( sOuterColContext->mInfo.mValueInfo != NULL,
                                    QLL_ERROR_STACK(aEnv) );

                sValueBlock = sOuterColContext->mInfo.mValueInfo;
                                            
                /* value block을 할당 받아 연결 */
                STL_TRY( qlndInitBlockList( & aDataOptInfo->mSQLStmt->mLongTypeValueList,
                                            & sBlockList,
                                            aDataOptInfo->mDataArea->mBlockAllocCnt,
                                            STL_FALSE,
                                            STL_LAYER_SQL_PROCESSOR,
                                            0,   /* table ID */
                                            0,   /* column ID */
                                            sValueBlock->mDataValue->mType,
                                            sValueBlock->mArgNum1,
                                            sValueBlock->mArgNum2,
                                            sValueBlock->mStringLengthUnit,
                                            sValueBlock->mIntervalIndicator,
                                            aDataOptInfo->mRegionMem,
                                            aEnv )
                         == STL_SUCCESS );
                                            
                sValueBlock = sBlockList->mValueBlock;
                break;
            }

        case QLV_EXPR_TYPE_FUNCTION :
        case QLV_EXPR_TYPE_CAST :
        case QLV_EXPR_TYPE_CASE_EXPR :                                        
        case QLV_EXPR_TYPE_LIST_FUNCTION :                                        
        case QLV_EXPR_TYPE_SUB_QUERY :
        case QLV_EXPR_TYPE_PSEUDO_ARGUMENT :
        default :
            {
                STL_DASSERT( 0 );
                break;
            }
    }

    QLO_GET_CONTEXT( aDataOptInfo, aInitExpr->mOffset )->mInfo.mValueInfo =
        sValueBlock;
    
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief RowId Column Expression 의 Data Area 를 최적화한다.
 * @param[in]  aDataOptInfo    Data Optimization Info
 * @param[in]  aInitExpr       Expression
 * @param[in]  aEnv            Environment
 * @remarks
 */
stlStatus qloDataOptimizeRowIdColumn( qloDataOptExprInfo  * aDataOptInfo,
                                      qlvInitExpression   * aInitExpr,
                                      qllEnv              * aEnv )
{
    knlValueBlockList         * sBlockList     = NULL;
    

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDataOptInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExpr->mType == QLV_EXPR_TYPE_ROWID_COLUMN,
                        QLL_ERROR_STACK(aEnv) );
    
    
    /**
     * RowId의 Value Block을 context로 설정
     */

    STL_DASSERT( aInitExpr->mExpr.mRowIdColumn->mRelationNode != NULL );

    
    /**
     * RowId Column List 에서 검색
     */

    STL_TRY( qlndFindRowIdColumnBlockList( aDataOptInfo->mDataArea->mRowIdColumnBlock,
                                           aInitExpr,
                                           & sBlockList,
                                           aEnv )
             == STL_SUCCESS );

    STL_DASSERT( sBlockList != NULL );
    STL_DASSERT( sBlockList->mValueBlock->mIsSepBuff == STL_TRUE );

    QLO_GET_CONTEXT( aDataOptInfo, aInitExpr->mOffset )->mInfo.mValueInfo =
        sBlockList->mValueBlock;
    
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Aggregation Expression 의 Data Area 를 최적화한다.
 * @param[in]  aDataOptInfo    Data Optimization Info
 * @param[in]  aInitExpr       Expression
 * @param[in]  aEnv            Environment
 * @remarks
 */
stlStatus qloDataOptimizeAggregation( qloDataOptExprInfo  * aDataOptInfo,
                                      qlvInitExpression   * aInitExpr,
                                      qllEnv              * aEnv )
{
    qloDBType                 * sDBTypes       = NULL;
    knlValueBlockList         * sBlockList     = NULL;
    

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDataOptInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExpr->mType == QLV_EXPR_TYPE_AGGREGATION,
                        QLL_ERROR_STACK(aEnv) );
    

    /**
     * Data Value 설정
     */
                    
    sDBTypes = & aDataOptInfo->mStmtExprList->mTotalExprInfo->mExprDBType[ aInitExpr->mOffset ];

    STL_TRY( qlndInitBlockList( & aDataOptInfo->mSQLStmt->mLongTypeValueList,
                                & sBlockList,
                                aDataOptInfo->mDataArea->mBlockAllocCnt,
                                STL_TRUE,
                                STL_LAYER_SQL_PROCESSOR,
                                0,   /* table ID */
                                0,   /* column ID */
                                sDBTypes->mSourceDBType,
                                sDBTypes->mSourceArgNum1,
                                sDBTypes->mSourceArgNum2,
                                sDBTypes->mSourceStringLengthUnit,
                                sDBTypes->mSourceIntervalIndicator,
                                aDataOptInfo->mRegionMem,
                                aEnv )
             == STL_SUCCESS );
    
    QLO_GET_CONTEXT( aDataOptInfo, aInitExpr->mOffset )->mInfo.mValueInfo =
        sBlockList->mValueBlock;
    
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief List Function Expression 의 Data Area 를 최적화한다.
 * @param[in]  aDataOptInfo    Data Optimization Info
 * @param[in]  aInitExpr       Expression
 * @param[in]  aEnv            Environment
 * @remarks
 */
stlStatus qloDataOptimizeListFunction( qloDataOptExprInfo  * aDataOptInfo,
                                       qlvInitExpression   * aInitExpr,
                                       qllEnv              * aEnv )
{
    qloDBType                 * sDBTypes        = NULL;
    
    qloFunctionInfo           * sExprFunc       = NULL;
    qloListFuncInfo           * sListInfo       = NULL;
    knlListFunction           * sListFunc       = NULL;

    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDataOptInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExpr->mType == QLV_EXPR_TYPE_LIST_FUNCTION,
                        QLL_ERROR_STACK(aEnv) );
    

    /**
     * Data Value 설정
     */
                    
    sDBTypes   = QLO_GET_DB_TYPE( aDataOptInfo, aInitExpr->mOffset );
    sExprFunc  = & aDataOptInfo->mStmtExprList->mTotalExprInfo->mExprFunc[ aInitExpr->mOffset ];
    sListInfo  = & aDataOptInfo->mStmtExprList->mTotalExprInfo->mListFunc[ aInitExpr->mOffset ];

    STL_DASSERT( sListInfo->mListFuncType != KNL_LIST_FUNCTION_NA );
                         

    /**
     * Function Context 공간 할당
     */

    STL_TRY( knlAllocRegionMem( aDataOptInfo->mRegionMem,
                                STL_SIZEOF( knlListFunction ),
                                (void **) & sListFunc,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sListFunc, 0x00, STL_SIZEOF( knlListFunction ) );

    
    /**
     * Function Pointer 설정
     */

    sListFunc->mListFuncType = sListInfo->mListFuncType;

    if( ( KNL_LIST_FUNC_START_ID <= sExprFunc->mFuncID ) &&
        ( sExprFunc->mFuncID <= KNL_LIST_FUNC_END_ID) )
    {
        STL_TRY( gListFunctionPtr
                 [ KNL_LIST_FUNC_ID_PTR( sExprFunc->mFuncID ) ]
                 [ sListInfo->mListFuncType ](
                     & sListFunc->mListFuncPtr,
                     QLL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );
    }
    
                  
    /**
     * Result Data Value 설정 (Buffer 공간 할당)
     */

    STL_TRY( qlndInitSingleDataValue( & aDataOptInfo->mSQLStmt->mLongTypeValueList,
                                      sDBTypes->mSourceDBType,
                                      & sListFunc->mResultValue,
                                      sExprFunc->mResultBufferSize,
                                      aDataOptInfo->mRegionMem,
                                      aEnv )
             == STL_SUCCESS );

    
    /**
     * List Context 설정
     */
    sListFunc->mValueCount  = sListInfo->mValueCount;
    sListFunc->mListStack   = sListInfo->mListStack;
    
    QLO_GET_CONTEXT( aDataOptInfo, aInitExpr->mOffset )->mInfo.mListFunc =
        sListFunc;
    
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief List Column Expression 의 Data Area 를 최적화한다.
 * @param[in]  aDataOptInfo    Data Optimization Info
 * @param[in]  aInitExpr       Expression
 * @param[in]  aEnv            Environment
 * @remarks
 */
stlStatus qloDataOptimizeListColumn( qloDataOptExprInfo  * aDataOptInfo,
                                     qlvInitExpression   * aInitExpr,
                                     qllEnv              * aEnv )
{
    return STL_SUCCESS;
}


/**
 * @brief Row Expression 의 Data Area 를 최적화한다.
 * @param[in]  aDataOptInfo    Data Optimization Info
 * @param[in]  aInitExpr       Expression
 * @param[in]  aEnv            Environment
 * @remarks
 */
stlStatus qloDataOptimizeRowExpr( qloDataOptExprInfo  * aDataOptInfo,
                                  qlvInitExpression   * aInitExpr,
                                  qllEnv              * aEnv )
{
    return STL_SUCCESS;
}


/**
 * @brief internal cast 정보를 구성한다.
 * @param[in]  aSQLStmt                SQL Statement
 * @param[in]  aRegionMem              knlRegionMem
 * @param[in]  aDBType                 qloDBType
 * @param[out] aCastFuncInfo           knlCastFuncInfo
 * @param[in]  aIsAllocCastFuncInfo    knlCastFuncInfo 를 할당받을지 여부
 * @param[in]  aIsAllocCastResultBuf   cast result buffer를 할당받을지 여부
 * @param[in]  aEnv                    Environment
 */
stlStatus qloDataSetInternalCastInfo( qllStatement      * aSQLStmt,
                                      knlRegionMem      * aRegionMem,
                                      qloDBType         * aDBType,
                                      knlCastFuncInfo  ** aCastFuncInfo,
                                      stlBool             aIsAllocCastFuncInfo,
                                      stlBool             aIsAllocCastResultBuf,
                                      qllEnv            * aEnv )
{
    knlCastFuncInfo  * sCastFuncInfo = NULL;
    stlInt64           sBufferSize   = 0;
    

    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBType != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCastFuncInfo != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * internal cast 정보 설정
     */

    if( aDBType->mIsNeedCast == STL_TRUE )
    {
        /**
         * cast result buffer 설정
         */
        
        if( aIsAllocCastFuncInfo == STL_TRUE )
        {
            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF( knlCastFuncInfo ),
                                        (void**) & sCastFuncInfo,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            stlMemset( sCastFuncInfo, 0x00, STL_SIZEOF( knlCastFuncInfo ) );
        }
        else
        {
            sCastFuncInfo = *aCastFuncInfo;
        }
        
        
        /**
         * cast function pointer 설정
         */
        
        STL_TRY( gBuiltInFuncInfoArr[ KNL_BUILTIN_FUNC_CAST ].mGetFuncPointer(
                     aDBType->mCastFuncIdx,
                     & sCastFuncInfo->mCastFuncPtr,
                     QLL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );


        /**
         * cast result buffer 설정
         */

        /* dtlValueEntry 공간 할당 */
        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( dtlValueEntry ) * KNL_CAST_FUNC_ARG_COUNT,
                                    (void**) & sCastFuncInfo->mArgs,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemset( sCastFuncInfo->mArgs,
                   0x00,
                   STL_SIZEOF( dtlValueEntry ) * KNL_CAST_FUNC_ARG_COUNT );

        /* result buffer : dtlDataValue 공간 할당 */
        sCastFuncInfo->mCastResultBuf.mType = DTL_TYPE_NA;
        sCastFuncInfo->mCastResultBuf.mLength = 0;
        sCastFuncInfo->mCastResultBuf.mValue = NULL;


        /**
         * cast arguments 설정
         */

        /* dest precision */
        STL_TRY( qloSetDataValueFromInteger( aSQLStmt,
                                             & sCastFuncInfo->mArgs[1].mValue.mDataValue,
                                             DTL_TYPE_NATIVE_BIGINT,
                                             DTL_PRECISION_NA,
                                             DTL_SCALE_NA,
                                             aDBType->mArgNum1,
                                             aRegionMem,
                                             aEnv )
                 == STL_SUCCESS );

        /* dest scale */
        STL_TRY( qloSetDataValueFromInteger( aSQLStmt,
                                             & sCastFuncInfo->mArgs[2].mValue.mDataValue,
                                             DTL_TYPE_NATIVE_BIGINT,
                                             DTL_PRECISION_NA,
                                             DTL_SCALE_NA,
                                             aDBType->mArgNum2,
                                             aRegionMem,
                                             aEnv )
                 == STL_SUCCESS );

        /* dest string length unit */
        STL_TRY( qloSetDataValueFromInteger( aSQLStmt,
                                             & sCastFuncInfo->mArgs[3].mValue.mDataValue,
                                             DTL_TYPE_NATIVE_INTEGER,
                                             DTL_PRECISION_NA,
                                             DTL_SCALE_NA,
                                             aDBType->mStringLengthUnit,
                                             aRegionMem,
                                             aEnv )
                 == STL_SUCCESS );

        /* dest interval indicator */
        STL_TRY( qloSetDataValueFromInteger( aSQLStmt,
                                             & sCastFuncInfo->mArgs[4].mValue.mDataValue,
                                             DTL_TYPE_NATIVE_INTEGER,
                                             DTL_PRECISION_NA,
                                             DTL_SCALE_NA,
                                             aDBType->mIntervalIndicator,
                                             aRegionMem,
                                             aEnv )
                 == STL_SUCCESS );
        
        if( aIsAllocCastResultBuf == STL_TRUE )
        {
            /* dtlDataValue의 mValue 공간 할당 */
            STL_TRY( qlndAllocDataValueArray( & aSQLStmt->mLongTypeValueList,
                                              & sCastFuncInfo->mCastResultBuf,
                                              aDBType->mDBType,
                                              aDBType->mArgNum1,
                                              aDBType->mStringLengthUnit,
                                              1,
                                              aRegionMem,
                                              & sBufferSize,
                                              aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /* Do Nothing */
        }
        
        if( ( aDBType->mSourceDBType == DTL_TYPE_INTERVAL_YEAR_TO_MONTH ) ||
            ( aDBType->mSourceDBType == DTL_TYPE_INTERVAL_DAY_TO_SECOND ) )
        {
            /**
             * interval type인 경우,
             * source db type의 정보 설정
             */
            
            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF( dtlDataTypeInfo ),
                                        (void**) & sCastFuncInfo->mSrcTypeInfo,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            stlMemset( sCastFuncInfo->mSrcTypeInfo,
                       0x00,
                       STL_SIZEOF( dtlDataTypeInfo ) );

            sCastFuncInfo->mSrcTypeInfo->mDataType = aDBType->mSourceDBType;
            sCastFuncInfo->mSrcTypeInfo->mArgNum1  = aDBType->mSourceArgNum1;
            sCastFuncInfo->mSrcTypeInfo->mArgNum2  = aDBType->mSourceArgNum2;
            sCastFuncInfo->mSrcTypeInfo->mArgNum3  = aDBType->mSourceIntervalIndicator;
            
            STL_DASSERT( sCastFuncInfo->mSrcTypeInfo->mArgNum3 >= 0 );
            STL_DASSERT( sCastFuncInfo->mSrcTypeInfo->mArgNum3 < DTL_INTERVAL_INDICATOR_MAX );
        }
        else
        {
            /* Do Nothing */
        }
    }
    else
    {
        sCastFuncInfo = NULL;
    }


    /**
     * Output 설정
     */

    *aCastFuncInfo = sCastFuncInfo;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Source Value Block으로부터 Dest Value Block으로의 internal cast 정보를 구성한다.
 * @param[in]  aSQLStmt                SQL Statement
 * @param[in]  aSrcValueBlock          Source Value Block
 * @param[in]  aDstValueBlock          Dest Value Block
 * @param[out] aCastFuncInfo           knlCastFuncInfo
 * @param[in]  aRegionMem              knlRegionMem
 * @param[in]  aEnv                    Environment
 */
stlStatus qloDataSetInternalCastInfoBetweenValueBlock( qllStatement      * aSQLStmt,
                                                       knlValueBlock     * aSrcValueBlock,
                                                       knlValueBlock     * aDstValueBlock,
                                                       knlCastFuncInfo  ** aCastFuncInfo,
                                                       knlRegionMem      * aRegionMem,
                                                       qllEnv            * aEnv )
{
    qloDBType          sDBType;
    stlUInt8           sDataType;
    stlInt32           sPrecision;
    stlInt32           sScale;
    stlUInt8           sStringLengthUnit;
    stlUInt8           sIntervalIndicator;
    stlUInt32          sCastFuncIdx = 0;

    stlBool            sIsConstantData[ 2 ];

    dtlDataType        sResultType = DTL_TYPE_NA;
    dtlDataType        sInputArgDataTypes[ 2 ];
    dtlDataType        sResultArgDataTypes[ 2 ];

    dtlDataTypeDefInfo    sResultTypeDefInfo;
    dtlDataTypeDefInfo    sInputArgDataTypeDefInfo[ 2 ];
    dtlDataTypeDefInfo    sResultArgDataTypeDefInfo[ 2 ];


    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSrcValueBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDstValueBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCastFuncInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**
     * Cast 필요 여부 판단
     */

    STL_DASSERT( aSrcValueBlock->mDataValue != NULL );
    STL_DASSERT( aDstValueBlock->mDataValue != NULL );
    
    STL_PARAM_VALIDATE( aSrcValueBlock->mDataValue->mType < DTL_TYPE_MAX,
                        QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aDstValueBlock->mDataValue->mType < DTL_TYPE_MAX,
                        QLL_ERROR_STACK(aEnv) );

    sDBType.mIsNeedCast        = STL_FALSE;
    sDBType.mCastFuncIdx       = 0;

    sDBType.mDBType            = aSrcValueBlock->mDataValue->mType;
    sDBType.mArgNum1           = aSrcValueBlock->mArgNum1;
    sDBType.mArgNum2           = aSrcValueBlock->mArgNum2;
    sDBType.mStringLengthUnit  = aSrcValueBlock->mStringLengthUnit;
    sDBType.mIntervalIndicator = aSrcValueBlock->mIntervalIndicator;

    sDBType.mSourceDBType            = aSrcValueBlock->mDataValue->mType;
    sDBType.mSourceArgNum1           = aSrcValueBlock->mArgNum1;
    sDBType.mSourceArgNum2           = aSrcValueBlock->mArgNum2;
    sDBType.mSourceStringLengthUnit  = aSrcValueBlock->mStringLengthUnit;
    sDBType.mSourceIntervalIndicator = aSrcValueBlock->mIntervalIndicator;

    sDataType          = aDstValueBlock->mDataValue->mType;
    sPrecision         = aDstValueBlock->mArgNum1;
    sScale             = aDstValueBlock->mArgNum2;
    sStringLengthUnit  = aDstValueBlock->mStringLengthUnit;
    sIntervalIndicator = aDstValueBlock->mIntervalIndicator;

    sIsConstantData[ 0 ] = STL_FALSE;
    sIsConstantData[ 1 ] = STL_FALSE;
        
    sInputArgDataTypes[ 0 ] = sDBType.mDBType;
    sInputArgDataTypes[ 1 ] = sDataType;

    sInputArgDataTypeDefInfo[0].mArgNum1 = sDBType.mArgNum1;
    sInputArgDataTypeDefInfo[0].mArgNum2 = sDBType.mArgNum2;
    sInputArgDataTypeDefInfo[0].mStringLengthUnit = sDBType.mStringLengthUnit;
    sInputArgDataTypeDefInfo[0].mIntervalIndicator = sDBType.mIntervalIndicator;

    sInputArgDataTypeDefInfo[1].mArgNum1 = sPrecision;
    sInputArgDataTypeDefInfo[1].mArgNum2 = sScale;
    sInputArgDataTypeDefInfo[1].mStringLengthUnit = sStringLengthUnit;
    sInputArgDataTypeDefInfo[1].mIntervalIndicator = sIntervalIndicator;

    if( sDBType.mDBType == sDataType )
    {
        switch( sDataType )
        {
            case DTL_TYPE_FLOAT :
                {
                    /**
                     * Cast가 필요한 경우
                     * 1. Precision이 줄어드는 경우
                     */

                    if( dtlBinaryToDecimalPrecision[ sDBType.mArgNum1 ] <= 
                        dtlBinaryToDecimalPrecision[ sPrecision ] )
                    {
                        sDBType.mArgNum1    = sPrecision;
                        sDBType.mArgNum2    = sScale;
                    }
                    else
                    {
                        sDBType.mIsNeedCast = STL_TRUE;
                        sDBType.mArgNum1    = sPrecision;
                        sDBType.mArgNum2    = sScale;

                        if( dtlGetFuncInfoCast( 2,
                                                sIsConstantData,
                                                sInputArgDataTypes,
                                                sInputArgDataTypeDefInfo,
                                                1,
                                                sResultArgDataTypes,
                                                sResultArgDataTypeDefInfo,
                                                & sResultType,
                                                & sResultTypeDefInfo,
                                                & sCastFuncIdx,
                                                KNL_DT_VECTOR(aEnv),
                                                aEnv,
                                                QLL_ERROR_STACK( aEnv ) )
                            != STL_SUCCESS )
                        {
                            (void)stlPopError( QLL_ERROR_STACK(aEnv) );
                            STL_THROW( RAMP_ERR_NOT_APPLICABLE_CAST );
                        }
                        else
                        {
                            sDBType.mCastFuncIdx = sCastFuncIdx;
                        }

                        STL_PARAM_VALIDATE( sResultType == sDataType,
                                            QLL_ERROR_STACK( aEnv ) );
                    }
                    
                    break;
                }
            case DTL_TYPE_NUMBER :
                {
                    /**
                     * Cast가 필요한 경우
                     * 1. domain 변경 :
                     *  - 최대값의 범위가 줄어드는 경우
                     *    => 최대값의 범위 = Precision - Scale
                     * 2. round 수행 :
                     *  - 반올림 대상 범위가 커지는 경우
                     *    => 반올림 대상 범위 = -sScale
                     */

                    if( ( ( (stlInt64)sDBType.mArgNum1 - (stlInt64)sDBType.mArgNum2 )
                          <= ( (stlInt64)sPrecision - (stlInt64)sScale ) ) &&
                        ( sDBType.mArgNum2 <= sScale ) )
                    {
                        sDBType.mArgNum1    = sPrecision;
                        sDBType.mArgNum2    = sScale;
                    }
                    else
                    {
                        sDBType.mIsNeedCast = STL_TRUE;
                        sDBType.mArgNum1    = sPrecision;
                        sDBType.mArgNum2    = sScale;

                        if( dtlGetFuncInfoCast( 2,
                                                sIsConstantData,
                                                sInputArgDataTypes,
                                                sInputArgDataTypeDefInfo,
                                                1,
                                                sResultArgDataTypes,
                                                sResultArgDataTypeDefInfo,
                                                & sResultType,
                                                & sResultTypeDefInfo,
                                                & sCastFuncIdx,
                                                KNL_DT_VECTOR(aEnv),
                                                aEnv,
                                                QLL_ERROR_STACK( aEnv ) )
                            != STL_SUCCESS )
                        {
                            (void)stlPopError( QLL_ERROR_STACK(aEnv) );
                            STL_THROW( RAMP_ERR_NOT_APPLICABLE_CAST );
                        }
                        else
                        {
                            sDBType.mCastFuncIdx = sCastFuncIdx;
                        }

                        STL_PARAM_VALIDATE( sResultType == sDataType,
                                            QLL_ERROR_STACK( aEnv ) );
                    }
                    break;
                }

            case DTL_TYPE_TIME :
            case DTL_TYPE_TIMESTAMP :
            case DTL_TYPE_TIME_WITH_TIME_ZONE : 
            case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE : 
                {
                    if( sDBType.mArgNum1 == sPrecision )
                    {
                        sDBType.mArgNum1    = sPrecision;
                        sDBType.mArgNum2    = DTL_SCALE_NA;
                    }
                    else
                    {
                        sDBType.mIsNeedCast = STL_TRUE;
                        sDBType.mArgNum1    = sPrecision;
                        sDBType.mArgNum2    = DTL_SCALE_NA;

                        if( dtlGetFuncInfoCast( 2,
                                                sIsConstantData,
                                                sInputArgDataTypes,
                                                sInputArgDataTypeDefInfo,
                                                1,
                                                sResultArgDataTypes,
                                                sResultArgDataTypeDefInfo,
                                                & sResultType,
                                                & sResultTypeDefInfo,
                                                & sCastFuncIdx,
                                                KNL_DT_VECTOR(aEnv),
                                                aEnv,
                                                QLL_ERROR_STACK( aEnv ) )
                            != STL_SUCCESS )
                        {
                            (void)stlPopError( QLL_ERROR_STACK(aEnv) );
                            STL_THROW( RAMP_ERR_NOT_APPLICABLE_CAST );
                        }
                        else
                        {
                            sDBType.mCastFuncIdx = sCastFuncIdx;
                        }

                        STL_PARAM_VALIDATE( sResultType == sDataType,
                                            QLL_ERROR_STACK( aEnv ) );
                    }
                    break;
                }

            case DTL_TYPE_CHAR :
                {
                    if( (sDBType.mArgNum1 == sPrecision) &&
                        (sDBType.mStringLengthUnit == sStringLengthUnit ) )
                    {
                        /* Do Nothing */
                    }
                    else
                    {
                        sDBType.mIsNeedCast       = STL_TRUE;
                        sDBType.mArgNum1          = sPrecision;
                        sDBType.mStringLengthUnit = sStringLengthUnit;

                        if( dtlGetFuncInfoCast( 2,
                                                sIsConstantData,
                                                sInputArgDataTypes,
                                                sInputArgDataTypeDefInfo,
                                                1,
                                                sResultArgDataTypes,
                                                sResultArgDataTypeDefInfo,
                                                & sResultType,
                                                & sResultTypeDefInfo,
                                                & sCastFuncIdx,
                                                KNL_DT_VECTOR(aEnv),
                                                aEnv,
                                                QLL_ERROR_STACK( aEnv ) )
                            != STL_SUCCESS )
                        {
                            (void)stlPopError( QLL_ERROR_STACK(aEnv) );
                            STL_THROW( RAMP_ERR_NOT_APPLICABLE_CAST );
                        }
                        else
                        {
                            sDBType.mCastFuncIdx = sCastFuncIdx;
                        }

                        STL_PARAM_VALIDATE( sResultType == sDataType,
                                            QLL_ERROR_STACK( aEnv ) );
                    }

                    break;
                }
                            
            case DTL_TYPE_VARCHAR :
            case DTL_TYPE_LONGVARCHAR :
                {
                    if( (sDBType.mArgNum1 <= sPrecision) &&
                        (sDBType.mStringLengthUnit == sStringLengthUnit ) )
                    {
                        sDBType.mArgNum1          = sPrecision;
                        sDBType.mStringLengthUnit = sStringLengthUnit;
                    }
                    else
                    {
                        sDBType.mIsNeedCast       = STL_TRUE;
                        sDBType.mArgNum1          = sPrecision;
                        sDBType.mStringLengthUnit = sStringLengthUnit;

                        if( dtlGetFuncInfoCast( 2,
                                                sIsConstantData,
                                                sInputArgDataTypes,
                                                sInputArgDataTypeDefInfo,
                                                1,
                                                sResultArgDataTypes,
                                                sResultArgDataTypeDefInfo,
                                                & sResultType,
                                                & sResultTypeDefInfo,
                                                & sCastFuncIdx,
                                                KNL_DT_VECTOR(aEnv),
                                                aEnv,
                                                QLL_ERROR_STACK( aEnv ) )
                            != STL_SUCCESS )
                        {
                            (void)stlPopError( QLL_ERROR_STACK(aEnv) );
                            STL_THROW( RAMP_ERR_NOT_APPLICABLE_CAST );
                        }
                        else
                        {
                            sDBType.mCastFuncIdx = sCastFuncIdx;
                        }

                        STL_PARAM_VALIDATE( sResultType == sDataType,
                                            QLL_ERROR_STACK( aEnv ) );
                    }

                    break;
                }

            case DTL_TYPE_BINARY :
            case DTL_TYPE_VARBINARY :
            case DTL_TYPE_LONGVARBINARY :
                {
                    if( sDBType.mArgNum1 == sPrecision )
                    {
                        /* Do Nothing */
                    }
                    else
                    {
                        sDBType.mIsNeedCast = STL_TRUE;
                        sDBType.mArgNum1    = sPrecision;

                        if( dtlGetFuncInfoCast( 2,
                                                sIsConstantData,
                                                sInputArgDataTypes,
                                                sInputArgDataTypeDefInfo,
                                                1,
                                                sResultArgDataTypes,
                                                sResultArgDataTypeDefInfo,
                                                & sResultType,
                                                & sResultTypeDefInfo,
                                                & sCastFuncIdx,
                                                KNL_DT_VECTOR(aEnv),
                                                aEnv,
                                                QLL_ERROR_STACK( aEnv ) )
                            != STL_SUCCESS )
                        {
                            (void)stlPopError( QLL_ERROR_STACK(aEnv) );
                            STL_THROW( RAMP_ERR_NOT_APPLICABLE_CAST );
                        }
                        else
                        {
                            sDBType.mCastFuncIdx = sCastFuncIdx;
                        }

                        STL_PARAM_VALIDATE( sResultType == sDataType,
                                            QLL_ERROR_STACK( aEnv ) );
                    }
                    break;
                }

            case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:
            case DTL_TYPE_INTERVAL_DAY_TO_SECOND:
                {
                    if( ( sDBType.mArgNum1 == sPrecision ) &&
                        ( sDBType.mArgNum2 == sScale ) && 
                        ( sDBType.mIntervalIndicator == sIntervalIndicator ) )
                    {
                        /* Do Nothing */
                    }
                    else
                    {
                        sDBType.mIsNeedCast        = STL_TRUE;
                        sDBType.mArgNum1           = sPrecision;
                        sDBType.mArgNum2           = sScale;
                        sDBType.mIntervalIndicator = sIntervalIndicator;

                        if( dtlGetFuncInfoCast( 2,
                                                sIsConstantData,
                                                sInputArgDataTypes,
                                                sInputArgDataTypeDefInfo,
                                                1,
                                                sResultArgDataTypes,
                                                sResultArgDataTypeDefInfo,
                                                & sResultType,
                                                & sResultTypeDefInfo,
                                                & sCastFuncIdx,
                                                KNL_DT_VECTOR(aEnv),
                                                aEnv,
                                                QLL_ERROR_STACK( aEnv ) )
                            != STL_SUCCESS )
                        {
                            (void)stlPopError( QLL_ERROR_STACK(aEnv) );
                            STL_THROW( RAMP_ERR_NOT_APPLICABLE_CAST );
                        }
                        else
                        {
                            sDBType.mCastFuncIdx = sCastFuncIdx; 
                        }

                        STL_PARAM_VALIDATE( sResultType == sDataType,
                                            QLL_ERROR_STACK( aEnv ) );
                    }
                    break;
                }

            case DTL_TYPE_BOOLEAN :
            case DTL_TYPE_NATIVE_SMALLINT :
            case DTL_TYPE_NATIVE_INTEGER :
            case DTL_TYPE_NATIVE_BIGINT :
            case DTL_TYPE_NATIVE_REAL :
            case DTL_TYPE_NATIVE_DOUBLE :
            case DTL_TYPE_DATE :
            case DTL_TYPE_ROWID :
                {
                    /* ignore */
                    /* Do Nothing */
                    break;
                }
            default :
                {
                    STL_DASSERT( 0 );
                    break;
                }
                            
        }
    }
    else if( ( ( dtlDataTypeGroup[sDBType.mDBType] == DTL_GROUP_NUMBER ) &&
               ( dtlDataTypeGroup[sDataType] == DTL_GROUP_NUMBER ) ) &&
             ( sDBType.mDBType != sDataType ) )
    {
        /* FLOAT  NUMBER 간 conversion이 발생하지 않도록. */

        if( sDBType.mDBType == DTL_TYPE_FLOAT )
        {
            /**
             * Cast가 필요한 경우
             * 1. precision이 줄어드는 경우
             * 2. target scale 이 N/A가 아닌 경우
             */
            
            STL_DASSERT( sDataType == DTL_TYPE_NUMBER );

            if( ( sScale == DTL_SCALE_NA ) &&
                ( dtlBinaryToDecimalPrecision[ sDBType.mArgNum1 ] <= sPrecision ) )
            {
                sDBType.mDBType     = sDataType;
                sDBType.mArgNum1    = sPrecision;
                sDBType.mArgNum2    = sScale;
            }
            else
            {
                sDBType.mDBType     = sDataType;
                sDBType.mIsNeedCast = STL_TRUE;
                sDBType.mArgNum1    = sPrecision;
                sDBType.mArgNum2    = sScale;
                        
                if( dtlGetFuncInfoCast( 2,
                                        sIsConstantData,
                                        sInputArgDataTypes,
                                        sInputArgDataTypeDefInfo,
                                        1,
                                        sResultArgDataTypes,
                                        sResultArgDataTypeDefInfo,
                                        & sResultType,
                                        & sResultTypeDefInfo,
                                        & sCastFuncIdx,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        QLL_ERROR_STACK( aEnv ) )
                    != STL_SUCCESS )
                {
                    (void)stlPopError( QLL_ERROR_STACK(aEnv) );
                    STL_THROW( RAMP_ERR_NOT_APPLICABLE_CAST );
                }
                else
                {
                    sDBType.mCastFuncIdx = sCastFuncIdx;
                }

                STL_PARAM_VALIDATE( sResultType == sDataType,
                                    QLL_ERROR_STACK( aEnv ) );
            }
        }
        else
        {
            /**
             * Cast가 필요한 경우
             * 1. Precision 줄어드는 경우
             */

            STL_DASSERT( sDBType.mDBType == DTL_TYPE_NUMBER );
            STL_DASSERT( sDataType == DTL_TYPE_FLOAT );
        
            if( sDBType.mArgNum1 <= dtlBinaryToDecimalPrecision[ sPrecision ] )
            {
                sDBType.mDBType     = sDataType;
                sDBType.mArgNum1    = sPrecision;
                sDBType.mArgNum2    = sScale;
            }
            else
            {
                sDBType.mDBType     = sDataType;
                sDBType.mIsNeedCast = STL_TRUE;
                sDBType.mArgNum1    = sPrecision;
                sDBType.mArgNum2    = sScale;
                        
                if( dtlGetFuncInfoCast( 2,
                                        sIsConstantData,
                                        sInputArgDataTypes,
                                        sInputArgDataTypeDefInfo,
                                        1,
                                        sResultArgDataTypes,
                                        sResultArgDataTypeDefInfo,
                                        & sResultType,
                                        & sResultTypeDefInfo,
                                        & sCastFuncIdx,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        QLL_ERROR_STACK( aEnv ) )
                    != STL_SUCCESS )
                {
                    (void)stlPopError( QLL_ERROR_STACK(aEnv) );
                    STL_THROW( RAMP_ERR_NOT_APPLICABLE_CAST );
                }
                else
                {
                    sDBType.mCastFuncIdx = sCastFuncIdx;
                }

                STL_PARAM_VALIDATE( sResultType == sDataType,
                                    QLL_ERROR_STACK( aEnv ) );
            }
        }
    }
    else if( sDBType.mDBType != sDataType )
    {
        sDBType.mDBType            = sDataType;
        sDBType.mIsNeedCast        = STL_TRUE;
        sDBType.mArgNum1           = sPrecision;
        sDBType.mArgNum2           = sScale;
        sDBType.mStringLengthUnit  = sStringLengthUnit;
        sDBType.mIntervalIndicator = sIntervalIndicator;

        if( dtlGetFuncInfoCast( 2,
                                sIsConstantData,
                                sInputArgDataTypes,
                                sInputArgDataTypeDefInfo,
                                1,
                                sResultArgDataTypes,
                                sResultArgDataTypeDefInfo,
                                & sResultType,
                                & sResultTypeDefInfo,
                                & sCastFuncIdx,
                                KNL_DT_VECTOR(aEnv),
                                aEnv,
                                QLL_ERROR_STACK( aEnv ) )
            != STL_SUCCESS )
        {
            (void)stlPopError( QLL_ERROR_STACK(aEnv) );
            STL_THROW( RAMP_ERR_NOT_APPLICABLE_CAST );
        }
        else
        {
            sDBType.mCastFuncIdx = sCastFuncIdx;
        }
                    
        STL_PARAM_VALIDATE( sResultType == sDataType,
                            QLL_ERROR_STACK( aEnv ) );
    }
    else
    {
        /* Do Nothing */
    }

    
    /**
     * Cast Function Info 설정
     */

    if( sDBType.mIsNeedCast == STL_TRUE )
    {
        STL_TRY( qloDataSetInternalCastInfo( aSQLStmt,
                                             aRegionMem,
                                             & sDBType,
                                             aCastFuncInfo,
                                             STL_TRUE,
                                             STL_FALSE,
                                             aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        *aCastFuncInfo = NULL;
    }

    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_APPLICABLE_CAST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INCONSISTENT_DATATYPES,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      gDataTypeDefinition[ sDataType ].mSqlNormalTypeName );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlo */
