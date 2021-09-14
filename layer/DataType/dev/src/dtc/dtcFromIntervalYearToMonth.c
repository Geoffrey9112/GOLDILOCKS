/*******************************************************************************
 * dtcFromIntervalYearToMonth.c
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
 * @file dtcFromIntervalYearToMonth.c
 * @brief DataType Layer IntervalYearToMonth으로부터 다른 타입으로의 변환 
 */

#include <dtl.h>
#include <dtDef.h>

#include <dtcCast.h>

/**
 * @ingroup dtcFromIntervalYearToMonth
 * @{
 */

/**
 * @brief IntervalYearToMonth -> SmallInt 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastIntervalYearToMonthToSmallInt( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv )
{
    dtlDataValue  * sSourceValue;                 // aInputArgument[0].mValue.mDataValue
    dtlDataValue  * sDestPrecisionValue;          // aInputArgument[1].mValue.mDataValue
    dtlDataValue  * sDestScaleValue;              // aInputArgument[2].mValue.mDataValue
    dtlDataValue  * sDestStringLengthUnitValue;   // aInputArgument[3].mValue.mDataValue
    dtlDataValue  * sDestIntervalIndicatorValue;  // aInputArgument[4].mValue.mDataValue

    dtlDataValue  * sResultValue;

    dtlIntervalYearToMonthType  sIntervalYearToMonth;
    stlInt64                    sValue = 0;
    stlBool                     sSuccessWithInfo = STL_FALSE;    
    
    stlErrorStack * sErrorStack;

    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
    sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
    sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
    sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;

    sResultValue = (dtlDataValue *)aResultValue;

    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_INTERVAL_YEAR_TO_MONTH, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_SMALLINT, sErrorStack );

    sIntervalYearToMonth.mIndicator =
        ((dtlIntervalYearToMonthType *)(sSourceValue->mValue))->mIndicator;
    sIntervalYearToMonth.mMonth = ((dtlIntervalYearToMonthType *)(sSourceValue->mValue))->mMonth;

    /**
     * single field에 대해서만 casting 가능.
     */
    STL_DASSERT( (sIntervalYearToMonth.mIndicator == DTL_INTERVAL_INDICATOR_YEAR) ||
                 (sIntervalYearToMonth.mIndicator == DTL_INTERVAL_INDICATOR_MONTH) );

    switch( sIntervalYearToMonth.mIndicator )
    {
        case( DTL_INTERVAL_INDICATOR_YEAR ) :
            sValue = sIntervalYearToMonth.mMonth / DTL_MONTHS_PER_YEAR;
            break;
        case( DTL_INTERVAL_INDICATOR_MONTH ) :
            sValue = sIntervalYearToMonth.mMonth;
            break;
        default:
            // Do Nothing
            break;
    }

    STL_TRY( dtdSmallIntSetValueFromInteger(
                 sValue,
                 *(stlInt64 *)(sDestPrecisionValue->mValue),
                 *(stlInt64 *)(sDestScaleValue->mValue),
                 *(dtlStringLengthUnit*)sDestStringLengthUnitValue->mValue,
                 *(dtlIntervalIndicator*)sDestIntervalIndicatorValue->mValue,
                 dtlDataTypeMaxBufferSize[sResultValue->mType], //메모리할당은 충분히 받았다고 가정.
                 sResultValue,
                 & sSuccessWithInfo,
                 aVectorFunc,
                 aVectorArg,
                 sErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;    

    return STL_FAILURE;
}

/**
 * @brief IntervalYearToMonth -> Integer 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastIntervalYearToMonthToInteger( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv )
{
    dtlDataValue  * sSourceValue;                 // aInputArgument[0].mValue.mDataValue
    dtlDataValue  * sDestPrecisionValue;          // aInputArgument[1].mValue.mDataValue
    dtlDataValue  * sDestScaleValue;              // aInputArgument[2].mValue.mDataValue
    dtlDataValue  * sDestStringLengthUnitValue;   // aInputArgument[3].mValue.mDataValue
    dtlDataValue  * sDestIntervalIndicatorValue;  // aInputArgument[4].mValue.mDataValue

    dtlDataValue  * sResultValue;

    dtlIntervalYearToMonthType  sIntervalYearToMonth;
    stlInt64                    sValue = 0;
    stlBool                     sSuccessWithInfo = STL_FALSE;    
    
    stlErrorStack * sErrorStack;

    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
    sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
    sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
    sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;

    sResultValue = (dtlDataValue *)aResultValue;

    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_INTERVAL_YEAR_TO_MONTH, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_INTEGER, sErrorStack );

    sIntervalYearToMonth.mIndicator =
        ((dtlIntervalYearToMonthType *)(sSourceValue->mValue))->mIndicator;
    sIntervalYearToMonth.mMonth = ((dtlIntervalYearToMonthType *)(sSourceValue->mValue))->mMonth;

    /**
     * single field에 대해서만 casting 가능.
     */
    STL_DASSERT( (sIntervalYearToMonth.mIndicator == DTL_INTERVAL_INDICATOR_YEAR) ||
                 (sIntervalYearToMonth.mIndicator == DTL_INTERVAL_INDICATOR_MONTH) );

    switch( sIntervalYearToMonth.mIndicator )
    {
        case( DTL_INTERVAL_INDICATOR_YEAR ) :
            sValue = sIntervalYearToMonth.mMonth / DTL_MONTHS_PER_YEAR;
            break;
        case( DTL_INTERVAL_INDICATOR_MONTH ) :
            sValue = sIntervalYearToMonth.mMonth;
            break;
        default:
            // Do Nothing
            break;
    }

    STL_TRY( dtdIntegerSetValueFromInteger(
                 sValue,
                 *(stlInt64 *)(sDestPrecisionValue->mValue),
                 *(stlInt64 *)(sDestScaleValue->mValue),
                 *(dtlStringLengthUnit*)sDestStringLengthUnitValue->mValue,
                 *(dtlIntervalIndicator*)sDestIntervalIndicatorValue->mValue,
                 dtlDataTypeMaxBufferSize[sResultValue->mType], //메모리할당은 충분히 받았다고 가정.
                 sResultValue,
                 & sSuccessWithInfo,
                 aVectorFunc,
                 aVectorArg,
                 sErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;    

    return STL_FAILURE;
}

/**
 * @brief IntervalYearToMonth -> BigInt 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastIntervalYearToMonthToBigInt( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv )
{
    dtlDataValue  * sSourceValue;                 // aInputArgument[0].mValue.mDataValue
    dtlDataValue  * sDestPrecisionValue;          // aInputArgument[1].mValue.mDataValue
    dtlDataValue  * sDestScaleValue;              // aInputArgument[2].mValue.mDataValue
    dtlDataValue  * sDestStringLengthUnitValue;   // aInputArgument[3].mValue.mDataValue
    dtlDataValue  * sDestIntervalIndicatorValue;  // aInputArgument[4].mValue.mDataValue

    dtlDataValue  * sResultValue;

    dtlIntervalYearToMonthType  sIntervalYearToMonth;
    stlInt64                    sValue = 0;
    stlBool                     sSuccessWithInfo = STL_FALSE;    
    
    stlErrorStack * sErrorStack;

    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
    sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
    sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
    sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;

    sResultValue = (dtlDataValue *)aResultValue;

    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_INTERVAL_YEAR_TO_MONTH, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_BIGINT, sErrorStack );

    sIntervalYearToMonth.mIndicator =
        ((dtlIntervalYearToMonthType *)(sSourceValue->mValue))->mIndicator;
    sIntervalYearToMonth.mMonth = ((dtlIntervalYearToMonthType *)(sSourceValue->mValue))->mMonth;

    /**
     * single field에 대해서만 casting 가능.
     */
    STL_DASSERT( (sIntervalYearToMonth.mIndicator == DTL_INTERVAL_INDICATOR_YEAR) ||
                 (sIntervalYearToMonth.mIndicator == DTL_INTERVAL_INDICATOR_MONTH) );

    switch( sIntervalYearToMonth.mIndicator )
    {
        case( DTL_INTERVAL_INDICATOR_YEAR ) :
            sValue = sIntervalYearToMonth.mMonth / DTL_MONTHS_PER_YEAR;
            break;
        case( DTL_INTERVAL_INDICATOR_MONTH ) :
            sValue = sIntervalYearToMonth.mMonth;
            break;
        default:
            // Do Nothing
            break;
    }

    STL_TRY( dtdBigIntSetValueFromInteger(
                 sValue,
                 *(stlInt64 *)(sDestPrecisionValue->mValue),
                 *(stlInt64 *)(sDestScaleValue->mValue),
                 *(dtlStringLengthUnit*)sDestStringLengthUnitValue->mValue,
                 *(dtlIntervalIndicator*)sDestIntervalIndicatorValue->mValue,
                 dtlDataTypeMaxBufferSize[sResultValue->mType], //메모리할당은 충분히 받았다고 가정.
                 sResultValue,
                 & sSuccessWithInfo,
                 aVectorFunc,
                 aVectorArg,
                 sErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;    

    return STL_FAILURE;
}

/**
 * @brief IntervalYearToMonth -> Numeric 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastIntervalYearToMonthToNumeric( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv )
{
    dtlDataValue  * sSourceValue;                 // aInputArgument[0].mValue.mDataValue
    dtlDataValue  * sDestPrecisionValue;          // aInputArgument[1].mValue.mDataValue
    dtlDataValue  * sDestScaleValue;              // aInputArgument[2].mValue.mDataValue
    dtlDataValue  * sDestStringLengthUnitValue;   // aInputArgument[3].mValue.mDataValue
    dtlDataValue  * sDestIntervalIndicatorValue;  // aInputArgument[4].mValue.mDataValue

    dtlDataValue  * sResultValue;

    dtlIntervalYearToMonthType  sIntervalYearToMonth;
    stlInt64                    sValue = 0;
    stlBool                     sSuccessWithInfo = STL_FALSE;    
    
    stlErrorStack * sErrorStack;

    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
    sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
    sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
    sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;

    sResultValue = (dtlDataValue *)aResultValue;

    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_INTERVAL_YEAR_TO_MONTH, sErrorStack );
    DTF_CHECK_DTL_RESULT2( sResultValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );

    sIntervalYearToMonth.mIndicator =
        ((dtlIntervalYearToMonthType *)(sSourceValue->mValue))->mIndicator;
    sIntervalYearToMonth.mMonth = ((dtlIntervalYearToMonthType *)(sSourceValue->mValue))->mMonth;

    /**
     * single field에 대해서만 casting 가능.
     */
    STL_DASSERT( (sIntervalYearToMonth.mIndicator == DTL_INTERVAL_INDICATOR_YEAR) ||
                 (sIntervalYearToMonth.mIndicator == DTL_INTERVAL_INDICATOR_MONTH) );

    switch( sIntervalYearToMonth.mIndicator )
    {
        case( DTL_INTERVAL_INDICATOR_YEAR ) :
            sValue = sIntervalYearToMonth.mMonth / DTL_MONTHS_PER_YEAR;
            break;
        case( DTL_INTERVAL_INDICATOR_MONTH ) :
            sValue = sIntervalYearToMonth.mMonth;
            break;
        default:
            // Do Nothing
            break;
    }

    STL_TRY( dtdNumericSetValueFromInteger(
                 sValue,
                 DTL_GET_NUMBER_DECIMAL_PRECISION( sResultValue->mType,
                                                   DTF_BIGINT( sDestPrecisionValue ) ),
                 *(stlInt64 *)(sDestScaleValue->mValue),
                 *(dtlStringLengthUnit*)sDestStringLengthUnitValue->mValue,
                 *(dtlIntervalIndicator*)sDestIntervalIndicatorValue->mValue,
                 dtlDataTypeMaxBufferSize[sResultValue->mType], //메모리할당은 충분히 받았다고 가정.
                 sResultValue,
                 & sSuccessWithInfo,
                 aVectorFunc,
                 aVectorArg,
                 sErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;    

    return STL_FAILURE;
}

/**
 * @brief IntervalYearToMonth -> Char 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastIntervalYearToMonthToChar( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv )
{
    dtlDataValue  * sSourceValue;                 // aInputArgument[0].mValue.mDataValue
    dtlDataValue  * sDestPrecisionValue;          // aInputArgument[1].mValue.mDataValue
    dtlDataValue  * sDestScaleValue;              // aInputArgument[2].mValue.mDataValue
    dtlDataValue  * sDestStringLengthUnitValue;   // aInputArgument[3].mValue.mDataValue
    dtlDataValue  * sDestIntervalIndicatorValue;  // aInputArgument[4].mValue.mDataValue

    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;

    stlBool         sSuccessWithInfo = STL_FALSE;    
    
    stlChar               sIntervalString[DTL_INTERVAL_YEAR_TO_MONTH_MAX_DISPLAY_SIZE + 1];
    stlInt64              sIntervalStringLen;
    dtlDataTypeInfo     * sSrcTypeInfo = NULL;    

    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
    sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
    sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
    sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;

    sResultValue = (dtlDataValue *)aResultValue;

    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_INTERVAL_YEAR_TO_MONTH, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_CHAR, sErrorStack );

    sSrcTypeInfo = (dtlDataTypeInfo *)(aInfo);        

    STL_DASSERT( sSrcTypeInfo->mArgNum3 >= 0 );
    STL_DASSERT( sSrcTypeInfo->mArgNum3 < DTL_INTERVAL_INDICATOR_MAX );

    STL_TRY( dtdIntervalYearToMonthToFormatString( sSourceValue,
                                                   sSrcTypeInfo->mArgNum1,
                                                   DTL_INTERVAL_YEAR_TO_MONTH_MAX_DISPLAY_SIZE,
                                                   sIntervalString,
                                                   & sIntervalStringLen,
                                                   aVectorFunc,
                                                   aVectorArg,
                                                   sErrorStack )
             == STL_SUCCESS );    
    sIntervalString[sIntervalStringLen] = '\0';    

    STL_TRY( dtdCharSetValueFromString(
                 sIntervalString,
                 sIntervalStringLen,
                 *(stlInt64 *)sDestPrecisionValue->mValue,
                 *(stlInt64 *)sDestScaleValue->mValue,
                 *(dtlStringLengthUnit*)sDestStringLengthUnitValue->mValue,
                 *(dtlIntervalIndicator*)sDestIntervalIndicatorValue->mValue,
                 dtlDataTypeMaxBufferSize[sResultValue->mType], //메모리할당은 충분히 받았다고 가정.
                 sResultValue,
                 & sSuccessWithInfo,
                 aVectorFunc,
                 aVectorArg,
                 aVectorFunc,
                 aVectorArg,
                 sErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;    

    return STL_FAILURE;    
}

/**
 * @brief IntervalYearToMonth -> Varchar 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastIntervalYearToMonthToVarchar( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv )
{
    dtlDataValue  * sSourceValue;                 // aInputArgument[0].mValue.mDataValue
    dtlDataValue  * sDestPrecisionValue;          // aInputArgument[1].mValue.mDataValue
    dtlDataValue  * sDestScaleValue;              // aInputArgument[2].mValue.mDataValue
    dtlDataValue  * sDestStringLengthUnitValue;   // aInputArgument[3].mValue.mDataValue
    dtlDataValue  * sDestIntervalIndicatorValue;  // aInputArgument[4].mValue.mDataValue

    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;

    stlBool         sSuccessWithInfo = STL_FALSE;    

    stlChar               sIntervalString[DTL_INTERVAL_YEAR_TO_MONTH_MAX_DISPLAY_SIZE + 1];
    stlInt64              sIntervalStringLen;
    dtlDataTypeInfo     * sSrcTypeInfo = NULL;    

    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
    sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
    sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
    sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;

    sResultValue = (dtlDataValue *)aResultValue;

    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_INTERVAL_YEAR_TO_MONTH, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_VARCHAR, sErrorStack );

    sSrcTypeInfo = (dtlDataTypeInfo *)(aInfo);        

    STL_DASSERT( sSrcTypeInfo->mArgNum3 >= 0 );
    STL_DASSERT( sSrcTypeInfo->mArgNum3 < DTL_INTERVAL_INDICATOR_MAX );

    STL_TRY( dtdIntervalYearToMonthToFormatString( sSourceValue,
                                                   sSrcTypeInfo->mArgNum1,
                                                   DTL_INTERVAL_YEAR_TO_MONTH_MAX_DISPLAY_SIZE,
                                                   sIntervalString,
                                                   & sIntervalStringLen,
                                                   aVectorFunc,
                                                   aVectorArg,
                                                   sErrorStack )
             == STL_SUCCESS );    
    sIntervalString[sIntervalStringLen] = '\0';    

    STL_TRY( dtdVarcharSetValueFromString(
                 sIntervalString,
                 sIntervalStringLen,
                 *(stlInt64 *)sDestPrecisionValue->mValue,
                 *(stlInt64 *)sDestScaleValue->mValue,
                 *(dtlStringLengthUnit*)sDestStringLengthUnitValue->mValue,
                 *(dtlIntervalIndicator*)sDestIntervalIndicatorValue->mValue,
                 dtlDataTypeMaxBufferSize[sResultValue->mType], //메모리할당은 충분히 받았다고 가정.
                 sResultValue,
                 & sSuccessWithInfo,
                 aVectorFunc,
                 aVectorArg,
                 aVectorFunc,
                 aVectorArg,
                 sErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;    

    return STL_FAILURE;    
}

/**
 * @brief IntervalYearToMonth -> Longvarchar 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastIntervalYearToMonthToLongvarchar( stlUInt16        aInputArgumentCnt,
                                                   dtlValueEntry  * aInputArgument,
                                                   void           * aResultValue,
                                                   void           * aInfo,
                                                   dtlFuncVector  * aVectorFunc,
                                                   void           * aVectorArg,
                                                   void           * aEnv )
{
    dtlDataValue  * sSourceValue;                 // aInputArgument[0].mValue.mDataValue
    dtlDataValue  * sDestPrecisionValue;          // aInputArgument[1].mValue.mDataValue
    dtlDataValue  * sDestScaleValue;              // aInputArgument[2].mValue.mDataValue
    dtlDataValue  * sDestStringLengthUnitValue;   // aInputArgument[3].mValue.mDataValue
    dtlDataValue  * sDestIntervalIndicatorValue;  // aInputArgument[4].mValue.mDataValue

    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;

    stlBool         sSuccessWithInfo = STL_FALSE;    

    stlChar               sIntervalString[DTL_INTERVAL_YEAR_TO_MONTH_MAX_DISPLAY_SIZE + 1];
    stlInt64              sIntervalStringLen;
    dtlDataTypeInfo     * sSrcTypeInfo = NULL;    

    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
    sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
    sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
    sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;

    sResultValue = (dtlDataValue *)aResultValue;

    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_INTERVAL_YEAR_TO_MONTH, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_LONGVARCHAR, sErrorStack );

    sSrcTypeInfo = (dtlDataTypeInfo *)(aInfo);        

    STL_DASSERT( sSrcTypeInfo->mArgNum3 >= 0 );
    STL_DASSERT( sSrcTypeInfo->mArgNum3 < DTL_INTERVAL_INDICATOR_MAX );

    STL_TRY( dtdIntervalYearToMonthToFormatString( sSourceValue,
                                                   sSrcTypeInfo->mArgNum1,
                                                   DTL_INTERVAL_YEAR_TO_MONTH_MAX_DISPLAY_SIZE,
                                                   sIntervalString,
                                                   & sIntervalStringLen,
                                                   aVectorFunc,
                                                   aVectorArg,
                                                   sErrorStack )
             == STL_SUCCESS );    
    sIntervalString[sIntervalStringLen] = '\0';    

    STL_TRY( dtdLongvarcharSetValueFromString(
                 sIntervalString,
                 sIntervalStringLen,
                 *(stlInt64 *)sDestPrecisionValue->mValue,
                 *(stlInt64 *)sDestScaleValue->mValue,
                 *(dtlStringLengthUnit*)sDestStringLengthUnitValue->mValue,
                 *(dtlIntervalIndicator*)sDestIntervalIndicatorValue->mValue,
                 dtlDataTypeMaxBufferSize[sResultValue->mType], //메모리할당은 충분히 받았다고 가정.
                 sResultValue,
                 & sSuccessWithInfo,
                 aVectorFunc,
                 aVectorArg,
                 aVectorFunc,
                 aVectorArg,
                 sErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;    

    return STL_FAILURE;    
}

/**
 * @brief IntervalYearToMonth -> IntervalYearToMonth 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastIntervalYearToMonthToIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                                           dtlValueEntry  * aInputArgument,
                                                           void           * aResultValue,
                                                           void           * aInfo,
                                                           dtlFuncVector  * aVectorFunc,
                                                           void           * aVectorArg,
                                                           void           * aEnv )
{
    dtlDataValue  * sSourceValue;                 // aInputArgument[0].mValue.mDataValue
    dtlDataValue  * sDestPrecisionValue;          // aInputArgument[1].mValue.mDataValue
//  dtlDataValue  * sDestScaleValue;              // aInputArgument[2].mValue.mDataValue
//  dtlDataValue  * sDestStringLengthUnitValue;   // aInputArgument[3].mValue.mDataValue
    dtlDataValue  * sDestIntervalIndicatorValue;  // aInputArgument[4].mValue.mDataValue

    dtlDataValue  * sResultValue;

    dtlIntervalYearToMonthType * sSourceIntervalYearToMonthType;
    dtlIntervalYearToMonthType * sDestIntervalYearToMonthType;
//    dtlIntervalIndicator         sSourceIntervalIndicator;    /* asign만 하고 사용하지 않는 변수 주석처리 */
    dtlIntervalIndicator         sDestIntervalIndicator;

    dtlExpTime                   sDtlExpTime;
    dtlIntervalYearToMonthType   sIntervalYearToMonthType;
    dtlFractionalSecond          sFractionalSecond = 0;
    
    stlErrorStack * sErrorStack;

    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
//  sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
//  sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
    sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;
    
    sResultValue = (dtlDataValue *)aResultValue;

    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_INTERVAL_YEAR_TO_MONTH, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_INTERVAL_YEAR_TO_MONTH, sErrorStack );

    sSourceIntervalYearToMonthType = (dtlIntervalYearToMonthType *)(sSourceValue->mValue);
//    sSourceIntervalIndicator = sSourceIntervalYearToMonthType->mIndicator;
    sDestIntervalIndicator = *(dtlIntervalIndicator *)(sDestIntervalIndicatorValue->mValue);

    STL_TRY_THROW( (sDestIntervalIndicator == DTL_INTERVAL_INDICATOR_YEAR)  ||
                   (sDestIntervalIndicator == DTL_INTERVAL_INDICATOR_MONTH) ||
                   (sDestIntervalIndicator == DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH),
                   ERROR_NOT_APPLICABLE );

    sIntervalYearToMonthType.mIndicator = sDestIntervalIndicator;
    sIntervalYearToMonthType.mMonth = sSourceIntervalYearToMonthType->mMonth;

    /*
     * value truncated check
     */

    DTL_INIT_DTLEXPTIME( sDtlExpTime );
    
    dtdIntervalYearToMonth2DtlExpTime( sIntervalYearToMonthType,
                                       & sDtlExpTime );

    STL_TRY( dtdIntervalValueTruncatedCheck( & sDtlExpTime,
                                             sDestIntervalIndicator,
                                             & sFractionalSecond,
                                             sErrorStack )
             == STL_SUCCESS );

    /*
     * leading precision 체크
     */ 
    STL_TRY( dtdAdjustIntervalYearToMonth( & sIntervalYearToMonthType,
                                           sDestIntervalIndicator,
                                           *(stlInt64 *)(sDestPrecisionValue->mValue),
                                           sErrorStack )
             == STL_SUCCESS );

    /*
     * Result value 설정 
     */
    
    sDestIntervalYearToMonthType = (dtlIntervalYearToMonthType *)(sResultValue->mValue);

    sDestIntervalYearToMonthType->mIndicator = sDestIntervalIndicator;    
    sDestIntervalYearToMonthType->mMonth = sIntervalYearToMonthType.mMonth;
    
    sResultValue->mLength = DTL_INTERVAL_YEAR_TO_MONTH_SIZE;
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_NOT_APPLICABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_YM_INTERVAL_SOME_FILEDS_TRUNCATED_CONVERSION,
                      NULL,
                      sErrorStack );
    }

    STL_FINISH;    

    return STL_FAILURE;    
}


/** @} */
