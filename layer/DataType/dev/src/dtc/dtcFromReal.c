/*******************************************************************************
 * dtcFromReal.c
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
 * @file dtcFromReal.c
 * @brief DataType Layer Real으로부터 다른 타입으로의 변환 
 */

#include <dtl.h>
#include <dtDef.h>

#include <dtcCast.h>

/**
 * @ingroup dtcFromReal
 * @{
 */

/**
 * @brief Real -> SmallInt 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastRealToSmallInt( stlUInt16        aInputArgumentCnt,
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

    stlBool         sSuccessWithInfo = STL_FALSE;    

    stlErrorStack * sErrorStack;

    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
    sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
    sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
    sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;

    sResultValue = (dtlDataValue *)aResultValue;
    
    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_NATIVE_REAL, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_SMALLINT, sErrorStack );

    STL_TRY( dtdSmallIntSetValueFromReal(
                 *(dtlRealType *)(sSourceValue->mValue),
                 *(stlInt64 *)sDestPrecisionValue->mValue,
                 *(stlInt64 *)sDestScaleValue->mValue,
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
 * @brief Real -> Integer 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastRealToInteger( stlUInt16        aInputArgumentCnt,
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

    stlBool         sSuccessWithInfo = STL_FALSE;    
    
    stlErrorStack * sErrorStack;
    
    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
    sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
    sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
    sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;

    sResultValue = (dtlDataValue *)aResultValue;
    
    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_NATIVE_REAL, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_INTEGER, sErrorStack );

    STL_TRY( dtdIntegerSetValueFromReal(
                 *(dtlRealType *)(sSourceValue->mValue),
                 *(stlInt64 *)sDestPrecisionValue->mValue,
                 *(stlInt64 *)sDestScaleValue->mValue,
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
 * @brief Real -> BigInt 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastRealToBigInt( stlUInt16        aInputArgumentCnt,
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

    stlBool         sSuccessWithInfo = STL_FALSE;    

    stlErrorStack * sErrorStack;

    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
    sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
    sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
    sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;

    sResultValue = (dtlDataValue *)aResultValue;
    
    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_NATIVE_REAL, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_BIGINT, sErrorStack );

    STL_TRY( dtdBigIntSetValueFromReal(
                 *(dtlRealType *)(sSourceValue->mValue),
                 *(stlInt64 *)sDestPrecisionValue->mValue,
                 *(stlInt64 *)sDestScaleValue->mValue,
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
 * @brief Real -> Real 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastRealToReal( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aInfo,
                             dtlFuncVector  * aVectorFunc,
                             void           * aVectorArg,
                             void           * aEnv )
{
    dtlDataValue  * sSourceValue;                 // aInputArgument[0].mValue.mDataValue
//  dtlDataValue  * sDestPrecisionValue;          // aInputArgument[1].mValue.mDataValue
//  dtlDataValue  * sDestScaleValue;              // aInputArgument[2].mValue.mDataValue
//  dtlDataValue  * sDestStringLengthUnitValue;   // aInputArgument[3].mValue.mDataValue
//  dtlDataValue  * sDestIntervalIndicatorValue;  // aInputArgument[4].mValue.mDataValue

    dtlDataValue  * sResultValue;

    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
//  sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
//  sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
//  sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
//  sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;

    sResultValue = (dtlDataValue *)aResultValue;
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, (stlErrorStack *)aEnv );    
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_NATIVE_REAL, (stlErrorStack *)aEnv );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_REAL, (stlErrorStack *)aEnv );

    /*
     * sResultValue는 외부에서 초기화되었다고 가정함.
     * 따라서, value의 type은 초기화되어 있음.
     */
    *(dtlRealType *)(sResultValue->mValue) = *(dtlRealType *)sSourceValue->mValue;
    sResultValue->mLength = sSourceValue->mLength;

    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief Real -> Double 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastRealToDouble( stlUInt16        aInputArgumentCnt,
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

    stlBool         sSuccessWithInfo = STL_FALSE;    

    stlErrorStack * sErrorStack;

    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
    sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
    sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
    sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;

    sResultValue = (dtlDataValue *)aResultValue;
    
    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_NATIVE_REAL, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_DOUBLE, sErrorStack );

    STL_TRY( dtdDoubleSetValueFromReal(
                 *(dtlRealType *)(sSourceValue->mValue),
                 *(stlInt64 *)sDestPrecisionValue->mValue,
                 *(stlInt64 *)sDestScaleValue->mValue,
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
 * @brief Real -> Numeric 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastRealToNumeric( stlUInt16        aInputArgumentCnt,
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
    
    stlBool         sSuccessWithInfo = STL_FALSE;

    stlErrorStack * sErrorStack;

    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
    sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
    sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
    sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;

    sResultValue = (dtlDataValue *)aResultValue;

    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_NATIVE_REAL, sErrorStack );
    DTF_CHECK_DTL_RESULT2( sResultValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );

    STL_TRY( dtdNumericSetValueFromFloat32(
                 *(dtlRealType *)(sSourceValue->mValue),
                 DTL_GET_NUMBER_DECIMAL_PRECISION( sResultValue->mType,
                                                   DTF_BIGINT( sDestPrecisionValue ) ),
                 *(stlInt64 *)sDestScaleValue->mValue,
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
 * @brief Real -> Char 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastRealToChar( stlUInt16        aInputArgumentCnt,
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
    
    stlChar         sRealString[DTL_FLOAT32_STRING_SIZE];
    stlInt64        sRealStringLen;

    stlBool         sSuccessWithInfo = STL_FALSE;

    stlErrorStack * sErrorStack;

    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
    sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
    sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
    sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;

    sResultValue = (dtlDataValue *)aResultValue;

    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_NATIVE_REAL, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_CHAR, sErrorStack );

    if( *(stlInt64 *)sDestPrecisionValue->mValue < DTL_FLOAT32_STRING_SIZE )
    {
        STL_TRY( dtdRealToNonTruncateExponentString( sSourceValue,
                                                     *(stlInt64 *)sDestPrecisionValue->mValue,
                                                     sRealString,
                                                     & sRealStringLen,
                                                     aVectorFunc,
                                                     aVectorArg,
                                                     sErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( dtdRealToNonTruncateExponentString( sSourceValue,
                                                     DTL_FLOAT32_STRING_SIZE,
                                                     sRealString,
                                                     & sRealStringLen,
                                                     aVectorFunc,
                                                     aVectorArg,
                                                     sErrorStack )
                 == STL_SUCCESS );
    }

    sRealString[sRealStringLen] = '\0';    

    STL_TRY( dtdCharSetValueFromString(
                 sRealString,
                 sRealStringLen,
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
 * @brief Real -> Varchar 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastRealToVarchar( stlUInt16        aInputArgumentCnt,
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
    
    stlChar         sRealString[DTL_FLOAT32_STRING_SIZE];
    stlInt64        sRealStringLen;

    stlBool         sSuccessWithInfo = STL_FALSE;

    stlErrorStack * sErrorStack;

    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
    sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
    sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
    sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;

    sResultValue = (dtlDataValue *)aResultValue;

    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_NATIVE_REAL, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_VARCHAR, sErrorStack );

    if( *(stlInt64 *)sDestPrecisionValue->mValue < DTL_FLOAT32_STRING_SIZE )
    {
        STL_TRY( dtdRealToExponentString( sSourceValue,
                                          *(stlInt64 *)sDestPrecisionValue->mValue,
                                          sRealString,
                                          & sRealStringLen,
                                          aVectorFunc,
                                          aVectorArg,
                                          & sSuccessWithInfo,
                                          sErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( dtdRealToExponentString( sSourceValue,
                                          DTL_FLOAT32_STRING_SIZE,
                                          sRealString,
                                          & sRealStringLen,
                                          aVectorFunc,
                                          aVectorArg,
                                          & sSuccessWithInfo,
                                          sErrorStack )
                 == STL_SUCCESS );
    }

    sRealString[sRealStringLen] = '\0';    

    STL_TRY( dtdVarcharSetValueFromString(
                 sRealString,
                 sRealStringLen,
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
 * @brief Real -> Longvarchar 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastRealToLongvarchar( stlUInt16        aInputArgumentCnt,
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
    
    stlChar         sRealString[DTL_FLOAT32_STRING_SIZE];
    stlInt64        sRealStringLen;

    stlBool         sSuccessWithInfo = STL_FALSE;

    stlErrorStack * sErrorStack;

    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
    sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
    sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
    sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;

    sResultValue = (dtlDataValue *)aResultValue;

    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_NATIVE_REAL, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_LONGVARCHAR, sErrorStack );

    STL_TRY( dtdRealToExponentString( sSourceValue,
                                      DTL_FLOAT32_STRING_SIZE,
                                      sRealString,
                                      & sRealStringLen,
                                      aVectorFunc,
                                      aVectorArg,
                                      & sSuccessWithInfo,
                                      sErrorStack )
             == STL_SUCCESS );
    
    sRealString[sRealStringLen] = '\0';    

    STL_TRY( dtdLongvarcharSetValueFromString(
                 sRealString,
                 sRealStringLen,
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


/** @} */
