/*******************************************************************************
 * dtcFromBoolean.c
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
 * @file dtcFromBoolean.c
 * @brief DataType Layer Boolean으로부터 다른 타입으로의 변환 
 */

#include <dtl.h>
#include <dtDef.h>

#include <dtcCast.h>

/**
 * @ingroup dtcCastFromBoolean
 * @{
 */

/**
 * @brief Boolean -> Boolean 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastBooleanToBoolean( stlUInt16        aInputArgumentCnt,
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
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_BOOLEAN, (stlErrorStack *)aEnv );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_BOOLEAN, (stlErrorStack *)aEnv );

    /*
     * sResultValue는 외부에서 초기화되었다고 가정함.
     * 따라서, value의 type은 초기화되어 있음.
     */
    *(dtlBooleanType *)(sResultValue->mValue) = *(dtlBooleanType *)sSourceValue->mValue;
    sResultValue->mLength = sSourceValue->mLength;

    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief Boolean -> CHAR 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastBooleanToChar( stlUInt16        aInputArgumentCnt,
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
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_BOOLEAN, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_CHAR, sErrorStack );

    /*
     * sResultValue는 외부에서 초기화되었다고 가정함.
     * 따라서, value의 type은 초기화되어 있음.
     */

    if( *(dtlBooleanType *)sSourceValue->mValue == (dtlBooleanType)STL_TRUE )
    {
        STL_TRY( dtdCharSetValueFromString(
                     DTL_BOOLEAN_STRING_TRUE,
                     DTL_BOOLEAN_STRING_TRUE_SIZE,
                     *(stlInt64 *)sDestPrecisionValue->mValue,
                     *(stlInt64 *)sDestScaleValue->mValue,
                     *(dtlStringLengthUnit*)sDestStringLengthUnitValue->mValue,
                     *(dtlIntervalIndicator*)sDestIntervalIndicatorValue->mValue,
                     dtlDataTypeMaxBufferSize[sResultValue->mType],//메모리할당은충분히받았다고가정.
                     sResultValue,
                     & sSuccessWithInfo,
                     aVectorFunc,
                     aVectorArg,
                     aVectorFunc,
                     aVectorArg,
                     sErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( dtdCharSetValueFromString(
                     DTL_BOOLEAN_STRING_FALSE,
                     DTL_BOOLEAN_STRING_FALSE_SIZE,
                     *(stlInt64 *)sDestPrecisionValue->mValue,
                     *(stlInt64 *)sDestScaleValue->mValue,
                     *(dtlStringLengthUnit*)sDestStringLengthUnitValue->mValue,
                     *(dtlIntervalIndicator*)sDestIntervalIndicatorValue->mValue,
                     dtlDataTypeMaxBufferSize[sResultValue->mType],//메모리할당은충분히받았다고가정.
                     sResultValue,
                     & sSuccessWithInfo,
                     aVectorFunc,
                     aVectorArg,
                     aVectorFunc,
                     aVectorArg,
                     sErrorStack )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief Boolean -> VARCHAR 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastBooleanToVarchar( stlUInt16        aInputArgumentCnt,
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
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_BOOLEAN, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_VARCHAR, sErrorStack );

    /*
     * sResultValue는 외부에서 초기화되었다고 가정함.
     * 따라서, value의 type은 초기화되어 있음.
     */

    if( *(dtlBooleanType *)sSourceValue->mValue == (dtlBooleanType)STL_TRUE )
    {
        STL_TRY( dtdVarcharSetValueFromString(
                     DTL_BOOLEAN_STRING_TRUE,
                     DTL_BOOLEAN_STRING_TRUE_SIZE,
                     *(stlInt64 *)sDestPrecisionValue->mValue,
                     *(stlInt64 *)sDestScaleValue->mValue,
                     *(dtlStringLengthUnit*)sDestStringLengthUnitValue->mValue,
                     *(dtlIntervalIndicator*)sDestIntervalIndicatorValue->mValue,
                     dtlDataTypeMaxBufferSize[sResultValue->mType],//메모리할당은충분히받았다고가정.
                     sResultValue,
                     & sSuccessWithInfo,
                     aVectorFunc,
                     aVectorArg,
                     aVectorFunc,
                     aVectorArg,
                     sErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( dtdVarcharSetValueFromString(
                     DTL_BOOLEAN_STRING_FALSE,
                     DTL_BOOLEAN_STRING_FALSE_SIZE,
                     *(stlInt64 *)sDestPrecisionValue->mValue,
                     *(stlInt64 *)sDestScaleValue->mValue,
                     *(dtlStringLengthUnit*)sDestStringLengthUnitValue->mValue,
                     *(dtlIntervalIndicator*)sDestIntervalIndicatorValue->mValue,
                     dtlDataTypeMaxBufferSize[sResultValue->mType],//메모리할당은충분히받았다고가정.
                     sResultValue,
                     & sSuccessWithInfo,
                     aVectorFunc,
                     aVectorArg,
                     aVectorFunc,
                     aVectorArg,
                     sErrorStack )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief Boolean -> LONGVARCHAR 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastBooleanToLongvarchar( stlUInt16        aInputArgumentCnt,
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
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_BOOLEAN, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_LONGVARCHAR, sErrorStack );

    /*
     * sResultValue는 외부에서 초기화되었다고 가정함.
     * 따라서, value의 type은 초기화되어 있음.
     */

    if( *(dtlBooleanType *)sSourceValue->mValue == (dtlBooleanType)STL_TRUE )
    {
        STL_TRY( dtdLongvarcharSetValueFromString(
                     DTL_BOOLEAN_STRING_TRUE,
                     DTL_BOOLEAN_STRING_TRUE_SIZE,
                     *(stlInt64 *)sDestPrecisionValue->mValue,
                     *(stlInt64 *)sDestScaleValue->mValue,
                     *(dtlStringLengthUnit*)sDestStringLengthUnitValue->mValue,
                     *(dtlIntervalIndicator*)sDestIntervalIndicatorValue->mValue,
                     dtlDataTypeMaxBufferSize[sResultValue->mType],//메모리할당은충분히받았다고가정.
                     sResultValue,
                     & sSuccessWithInfo,
                     aVectorFunc,
                     aVectorArg,
                     aVectorFunc,
                     aVectorArg,
                     sErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( dtdLongvarcharSetValueFromString(
                     DTL_BOOLEAN_STRING_FALSE,
                     DTL_BOOLEAN_STRING_FALSE_SIZE,
                     *(stlInt64 *)sDestPrecisionValue->mValue,
                     *(stlInt64 *)sDestScaleValue->mValue,
                     *(dtlStringLengthUnit*)sDestStringLengthUnitValue->mValue,
                     *(dtlIntervalIndicator*)sDestIntervalIndicatorValue->mValue,
                     dtlDataTypeMaxBufferSize[sResultValue->mType],//메모리할당은충분히받았다고가정.
                     sResultValue,
                     & sSuccessWithInfo,
                     aVectorFunc,
                     aVectorArg,
                     aVectorFunc,
                     aVectorArg,
                     sErrorStack )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
