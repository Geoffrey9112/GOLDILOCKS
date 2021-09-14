/*******************************************************************************
 * dtcFromIntervalDayToSecond.c
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
 * @file dtcFromIntervalDayToSecond.c
 * @brief DataType Layer IntervalDayToSecond으로부터 다른 타입으로의 변환 
 */

#include <dtl.h>
#include <dtDef.h>

#include <dtcCast.h>

/**
 * @ingroup dtcFromIntervalDayToSecond
 * @{
 */

/**
 * @brief IntervalDayToSecond -> SmallInt 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastIntervalDayToSecondToSmallInt( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aInfo,
                                                void           * aVectorArg,
                                                void           * aEnv )
{
    dtlDataValue  * sSourceValue;                 // aInputArgument[0].mValue.mDataValue
    dtlDataValue  * sDestPrecisionValue;          // aInputArgument[1].mValue.mDataValue
    dtlDataValue  * sDestScaleValue;              // aInputArgument[2].mValue.mDataValue
    dtlDataValue  * sDestStringLengthUnitValue;   // aInputArgument[3].mValue.mDataValue
    dtlDataValue  * sDestIntervalIndicatorValue;  // aInputArgument[4].mValue.mDataValue

    dtlDataValue  * sResultValue;

    dtlIntervalDayToSecondType  sIntervalDayToSecond;
    stlInt64                    sValue = 0;
    stlInt64                    sFractionalSecond = 0;
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
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_INTERVAL_DAY_TO_SECOND, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_SMALLINT, sErrorStack );

    sIntervalDayToSecond.mIndicator =
        ((dtlIntervalDayToSecondType *)(sSourceValue->mValue))->mIndicator;
    sIntervalDayToSecond.mDay = ((dtlIntervalDayToSecondType *)(sSourceValue->mValue))->mDay;
    sIntervalDayToSecond.mTime = ((dtlIntervalDayToSecondType *)(sSourceValue->mValue))->mTime;

    /**
     * single field에 대해서만 casting 가능.
     */
    STL_DASSERT( (sIntervalDayToSecond.mIndicator == DTL_INTERVAL_INDICATOR_DAY) ||
                 (sIntervalDayToSecond.mIndicator == DTL_INTERVAL_INDICATOR_HOUR) ||
                 (sIntervalDayToSecond.mIndicator == DTL_INTERVAL_INDICATOR_MINUTE) ||
                 (sIntervalDayToSecond.mIndicator == DTL_INTERVAL_INDICATOR_SECOND) );

    switch( sIntervalDayToSecond.mIndicator )
    {
        case( DTL_INTERVAL_INDICATOR_DAY ) :
            sValue = sIntervalDayToSecond.mDay;
            break;
        case ( DTL_INTERVAL_INDICATOR_HOUR) :
            sValue = (sIntervalDayToSecond.mDay * DTL_USECS_PER_DAY) + sIntervalDayToSecond.mTime;
            sValue = sValue / DTL_USECS_PER_HOUR;
            break;
        case ( DTL_INTERVAL_INDICATOR_MINUTE ) :
            sValue = (sIntervalDayToSecond.mDay * DTL_USECS_PER_DAY) + sIntervalDayToSecond.mTime;
            sValue = sValue / DTL_USECS_PER_MINUTE;
            break;
        case ( DTL_INTERVAL_INDICATOR_SECOND ) :
            {
                sValue =
                    (sIntervalDayToSecond.mDay * DTL_USECS_PER_DAY) + sIntervalDayToSecond.mTime;
                sFractionalSecond = sValue % DTL_USECS_PER_SEC;
                sValue = sValue / DTL_USECS_PER_SEC;
                
                // fractinalsecond 반올림 
                if( sFractionalSecond >= 500000 )
                {
                    sValue = sValue + 1;
                }
                else
                {
                    // Do Nothing
                }
                
                break;
            }
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
 * @brief IntervalDayToSecond -> Integer 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastIntervalDayToSecondToInteger( stlUInt16        aInputArgumentCnt,
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

    dtlIntervalDayToSecondType  sIntervalDayToSecond;
    stlInt64                    sValue = 0;
    stlInt64                    sFractionalSecond = 0;
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
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_INTERVAL_DAY_TO_SECOND, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_INTEGER, sErrorStack );

    sIntervalDayToSecond.mIndicator =
        ((dtlIntervalDayToSecondType *)(sSourceValue->mValue))->mIndicator;
    sIntervalDayToSecond.mDay = ((dtlIntervalDayToSecondType *)(sSourceValue->mValue))->mDay;
    sIntervalDayToSecond.mTime = ((dtlIntervalDayToSecondType *)(sSourceValue->mValue))->mTime;

    /**
     * single field에 대해서만 casting 가능.
     */
    STL_DASSERT( (sIntervalDayToSecond.mIndicator == DTL_INTERVAL_INDICATOR_DAY) ||
                 (sIntervalDayToSecond.mIndicator == DTL_INTERVAL_INDICATOR_HOUR) ||
                 (sIntervalDayToSecond.mIndicator == DTL_INTERVAL_INDICATOR_MINUTE) ||
                 (sIntervalDayToSecond.mIndicator == DTL_INTERVAL_INDICATOR_SECOND) );

    switch( sIntervalDayToSecond.mIndicator )
    {
        case( DTL_INTERVAL_INDICATOR_DAY ) :
            sValue = sIntervalDayToSecond.mDay;
            break;
        case ( DTL_INTERVAL_INDICATOR_HOUR) :
            sValue = (sIntervalDayToSecond.mDay * DTL_USECS_PER_DAY) + sIntervalDayToSecond.mTime;
            sValue = sValue / DTL_USECS_PER_HOUR;
            break;
        case ( DTL_INTERVAL_INDICATOR_MINUTE ) :
            sValue = (sIntervalDayToSecond.mDay * DTL_USECS_PER_DAY) + sIntervalDayToSecond.mTime;
            sValue = sValue / DTL_USECS_PER_MINUTE;
            break;
        case ( DTL_INTERVAL_INDICATOR_SECOND ) :
            {
                sValue =
                    (sIntervalDayToSecond.mDay * DTL_USECS_PER_DAY) + sIntervalDayToSecond.mTime;
                sFractionalSecond = sValue % DTL_USECS_PER_SEC;
                sValue = sValue / DTL_USECS_PER_SEC;
                
                // fractinalsecond 반올림 
                if( sFractionalSecond >= 500000 )
                {
                    sValue = sValue + 1;
                }
                else
                {
                    // Do Nothing
                }
                
                break;
            }
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
 * @brief IntervalDayToSecond -> BigInt 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastIntervalDayToSecondToBigInt( stlUInt16        aInputArgumentCnt,
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

    dtlIntervalDayToSecondType  sIntervalDayToSecond;
    stlInt64                    sValue = 0;
    stlInt64                    sFractionalSecond = 0;
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
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_INTERVAL_DAY_TO_SECOND, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_BIGINT, sErrorStack );

    sIntervalDayToSecond.mIndicator =
        ((dtlIntervalDayToSecondType *)(sSourceValue->mValue))->mIndicator;
    sIntervalDayToSecond.mDay = ((dtlIntervalDayToSecondType *)(sSourceValue->mValue))->mDay;
    sIntervalDayToSecond.mTime = ((dtlIntervalDayToSecondType *)(sSourceValue->mValue))->mTime;

    /**
     * single field에 대해서만 casting 가능.
     */
    STL_DASSERT( (sIntervalDayToSecond.mIndicator == DTL_INTERVAL_INDICATOR_DAY) ||
                 (sIntervalDayToSecond.mIndicator == DTL_INTERVAL_INDICATOR_HOUR) ||
                 (sIntervalDayToSecond.mIndicator == DTL_INTERVAL_INDICATOR_MINUTE) ||
                 (sIntervalDayToSecond.mIndicator == DTL_INTERVAL_INDICATOR_SECOND) );

    switch( sIntervalDayToSecond.mIndicator )
    {
        case( DTL_INTERVAL_INDICATOR_DAY ) :
            sValue = sIntervalDayToSecond.mDay;
            break;
        case ( DTL_INTERVAL_INDICATOR_HOUR) :
            sValue = (sIntervalDayToSecond.mDay * DTL_USECS_PER_DAY) + sIntervalDayToSecond.mTime;
            sValue = sValue / DTL_USECS_PER_HOUR;
            break;
        case ( DTL_INTERVAL_INDICATOR_MINUTE ) :
            sValue = (sIntervalDayToSecond.mDay * DTL_USECS_PER_DAY) + sIntervalDayToSecond.mTime;
            sValue = sValue / DTL_USECS_PER_MINUTE;
            break;
        case ( DTL_INTERVAL_INDICATOR_SECOND ) :
            {
                sValue =
                    (sIntervalDayToSecond.mDay * DTL_USECS_PER_DAY) + sIntervalDayToSecond.mTime;
                sFractionalSecond = sValue % DTL_USECS_PER_SEC;
                sValue = sValue / DTL_USECS_PER_SEC;
                
                // fractinalsecond 반올림 
                if( sFractionalSecond >= 500000 )
                {
                    sValue = sValue + 1;
                }
                else
                {
                    // Do Nothing
                }
                
                break;
            }
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
 * @brief IntervalDayToSecond -> Numeric 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastIntervalDayToSecondToNumeric( stlUInt16        aInputArgumentCnt,
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

    dtlIntervalDayToSecondType  sIntervalDayToSecond;
    stlInt64                    sValue = 0;
    stlFloat64                  sWithFractionalSecond = 0.0;
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
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_INTERVAL_DAY_TO_SECOND, sErrorStack );
    DTF_CHECK_DTL_RESULT2( sResultValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );

    sIntervalDayToSecond.mIndicator =
        ((dtlIntervalDayToSecondType *)(sSourceValue->mValue))->mIndicator;
    sIntervalDayToSecond.mDay = ((dtlIntervalDayToSecondType *)(sSourceValue->mValue))->mDay;
    sIntervalDayToSecond.mTime = ((dtlIntervalDayToSecondType *)(sSourceValue->mValue))->mTime;

    /**
     * single field에 대해서만 casting 가능.
     */
    STL_DASSERT( (sIntervalDayToSecond.mIndicator == DTL_INTERVAL_INDICATOR_DAY) ||
                 (sIntervalDayToSecond.mIndicator == DTL_INTERVAL_INDICATOR_HOUR) ||
                 (sIntervalDayToSecond.mIndicator == DTL_INTERVAL_INDICATOR_MINUTE) ||
                 (sIntervalDayToSecond.mIndicator == DTL_INTERVAL_INDICATOR_SECOND) );

    switch( sIntervalDayToSecond.mIndicator )
    {
        case( DTL_INTERVAL_INDICATOR_DAY ) :
            sValue = sIntervalDayToSecond.mDay;
            break;
        case ( DTL_INTERVAL_INDICATOR_HOUR) :
            sValue = (sIntervalDayToSecond.mDay * DTL_USECS_PER_DAY) + sIntervalDayToSecond.mTime;
            sValue = sValue / DTL_USECS_PER_HOUR;
            break;
        case ( DTL_INTERVAL_INDICATOR_MINUTE ) :
            sValue = (sIntervalDayToSecond.mDay * DTL_USECS_PER_DAY) + sIntervalDayToSecond.mTime;
            sValue = sValue / DTL_USECS_PER_MINUTE;
            break;
        case ( DTL_INTERVAL_INDICATOR_SECOND ) :
            sValue = (sIntervalDayToSecond.mDay * DTL_USECS_PER_DAY) + sIntervalDayToSecond.mTime;
            sWithFractionalSecond = (stlFloat64)sValue / DTL_USECS_PER_SEC;
            break;
        default:
            // Do Nothing
            break;
    }

    if( ((sIntervalDayToSecond.mIndicator == DTL_INTERVAL_INDICATOR_DAY)  ||
         (sIntervalDayToSecond.mIndicator == DTL_INTERVAL_INDICATOR_HOUR) ||
         (sIntervalDayToSecond.mIndicator == DTL_INTERVAL_INDICATOR_MINUTE)) == STL_TRUE )
    {
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
    }
    else
    {
        STL_TRY( dtdNumericSetValueFromReal(
                     sWithFractionalSecond,
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
    }
    
    return STL_SUCCESS;

    STL_FINISH;    

    return STL_FAILURE;
}

/**
 * @brief IntervalDayToSecond -> Char 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastIntervalDayToSecondToChar( stlUInt16        aInputArgumentCnt,
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

    stlChar               sIntervalString[DTL_INTERVAL_DAY_TO_SECOND_MAX_DISPLAY_SIZE + 1];
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
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_INTERVAL_DAY_TO_SECOND, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_CHAR, sErrorStack );

    sSrcTypeInfo = (dtlDataTypeInfo *)(aInfo);

    STL_DASSERT( sSrcTypeInfo->mArgNum3 >= 0 );
    STL_DASSERT( sSrcTypeInfo->mArgNum3 < DTL_INTERVAL_INDICATOR_MAX );

    STL_TRY( dtdIntervalDayToSecondToFormatString( sSourceValue,
                                                   sSrcTypeInfo->mArgNum1,
                                                   sSrcTypeInfo->mArgNum2,
                                                   DTL_INTERVAL_DAY_TO_SECOND_MAX_DISPLAY_SIZE,
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
 * @brief IntervalDayToSecond -> Varchar 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastIntervalDayToSecondToVarchar( stlUInt16        aInputArgumentCnt,
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

    stlChar               sIntervalString[DTL_INTERVAL_DAY_TO_SECOND_MAX_DISPLAY_SIZE + 1];
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
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_INTERVAL_DAY_TO_SECOND, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_VARCHAR, sErrorStack );

    sSrcTypeInfo = (dtlDataTypeInfo *)(aInfo);        

    STL_DASSERT( sSrcTypeInfo->mArgNum3 >= 0 );
    STL_DASSERT( sSrcTypeInfo->mArgNum3 < DTL_INTERVAL_INDICATOR_MAX );

    STL_TRY( dtdIntervalDayToSecondToFormatString( sSourceValue,
                                                   sSrcTypeInfo->mArgNum1,
                                                   sSrcTypeInfo->mArgNum2,
                                                   DTL_INTERVAL_DAY_TO_SECOND_MAX_DISPLAY_SIZE,
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
 * @brief IntervalDayToSecond -> Longvarchar 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastIntervalDayToSecondToLongvarchar( stlUInt16        aInputArgumentCnt,
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

    stlChar               sIntervalString[DTL_INTERVAL_DAY_TO_SECOND_MAX_DISPLAY_SIZE + 1];
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
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_INTERVAL_DAY_TO_SECOND, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_LONGVARCHAR, sErrorStack );

    sSrcTypeInfo = (dtlDataTypeInfo *)(aInfo);        

    STL_DASSERT( sSrcTypeInfo->mArgNum3 >= 0 );
    STL_DASSERT( sSrcTypeInfo->mArgNum3 < DTL_INTERVAL_INDICATOR_MAX );

    STL_TRY( dtdIntervalDayToSecondToFormatString( sSourceValue,
                                                   sSrcTypeInfo->mArgNum1,
                                                   sSrcTypeInfo->mArgNum2,
                                                   DTL_INTERVAL_DAY_TO_SECOND_MAX_DISPLAY_SIZE,
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
 * @brief IntervalDayToSecond -> IntervalDayToSecond 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastIntervalDayToSecondToIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
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
//  dtlDataValue  * sDestStringLengthUnitValue;   // aInputArgument[3].mValue.mDataValue
    dtlDataValue  * sDestIntervalIndicatorValue;  // aInputArgument[4].mValue.mDataValue

    dtlDataValue  * sResultValue;

    dtlIntervalDayToSecondType * sSourceIntervalDayToSecondType;
    dtlIntervalDayToSecondType * sDestIntervalDayToSecondType;
//    dtlIntervalIndicator         sSourceIntervalIndicator;    /* asign만 하고 사용하지 않는 변수 주석처리 */
    dtlIntervalIndicator         sDestIntervalIndicator;

    dtlExpTime                   sDtlExpTime;
    dtlIntervalDayToSecondType   sIntervalDayToSecondType;
    dtlFractionalSecond          sFractionalSecond = 0;
    
    stlErrorStack * sErrorStack;

    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
    sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
//  sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
    sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;
    
    sResultValue = (dtlDataValue *)aResultValue;

    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_INTERVAL_DAY_TO_SECOND, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_INTERVAL_DAY_TO_SECOND, sErrorStack );

    sSourceIntervalDayToSecondType = (dtlIntervalDayToSecondType *)(sSourceValue->mValue);
//    sSourceIntervalIndicator = sSourceIntervalDayToSecondType->mIndicator;
    sDestIntervalIndicator = *(dtlIntervalIndicator *)(sDestIntervalIndicatorValue->mValue);

    STL_TRY_THROW( (sDestIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY)            ||
                   (sDestIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR)           ||
                   (sDestIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE)         ||
                   (sDestIntervalIndicator == DTL_INTERVAL_INDICATOR_SECOND)         ||
                   (sDestIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_HOUR)    ||
                   (sDestIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE)  ||
                   (sDestIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_SECOND)  ||
                   (sDestIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE) ||
                   (sDestIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND) ||
                   (sDestIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND),
                   ERROR_NOT_APPLICABLE );

    sIntervalDayToSecondType.mIndicator = sDestIntervalIndicator;
    sIntervalDayToSecondType.mDay = sSourceIntervalDayToSecondType->mDay;
    sIntervalDayToSecondType.mTime = sSourceIntervalDayToSecondType->mTime;
    
    /*
     * value truncated check
     */

    DTL_INIT_DTLEXPTIME( sDtlExpTime );
    
    dtdIntervalDayToSecond2DtlExpTime( sIntervalDayToSecondType,
                                       & sDtlExpTime,
                                       & sFractionalSecond );
    
    STL_TRY( dtdIntervalValueTruncatedCheck( & sDtlExpTime,
                                             sDestIntervalIndicator,
                                             & sFractionalSecond,
                                             sErrorStack )
             == STL_SUCCESS );
    
    /*
     * leading precision, fractional second precision체크
     */ 
    STL_TRY( dtdAdjustIntervalDayToSecond( & sIntervalDayToSecondType,
                                           sDestIntervalIndicator,
                                           *(stlInt64 *)(sDestPrecisionValue->mValue),
                                           *(stlInt64 *)(sDestScaleValue->mValue),
                                           sErrorStack )
             == STL_SUCCESS );

    /*
     * Result value 설정 
     */

    sDestIntervalDayToSecondType = (dtlIntervalDayToSecondType *)(sResultValue->mValue);
    
    sDestIntervalDayToSecondType->mIndicator = sDestIntervalIndicator;    
    sDestIntervalDayToSecondType->mDay = sIntervalDayToSecondType.mDay;
    sDestIntervalDayToSecondType->mTime = sIntervalDayToSecondType.mTime;
    
    sResultValue->mLength = DTL_INTERVAL_DAY_TO_SECOND_SIZE;
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_NOT_APPLICABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_DT_INTERVAL_LEADING_PRECISION_NOT_BIG_ENOUGH_TO_HOLD,
                      NULL,
                      sErrorStack );
    }

    STL_FINISH;    

    return STL_FAILURE;    
}


/** @} */

