/*******************************************************************************
 * dtcFromTimestamp.c
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
 * @file dtcFromTimestamp.c
 * @brief DataType Layer Timestamp으로부터 다른 타입으로의 변환 
 */

#include <dtl.h>
#include <dtDef.h>

#include <dtcCast.h>
#include <dtfFormatting.h>

/**
 * @ingroup dtcFromTimestamp
 * @{
 */

/**
 * @brief Timestamp -> Char 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastTimestampToChar( stlUInt16        aInputArgumentCnt,
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
    
    stlChar         sTimestampString[DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE + 1];
    stlInt64        sTimestampStringLen;

    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
    sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
    sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
    sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;

    sResultValue = (dtlDataValue *)aResultValue;

    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_TIMESTAMP, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_CHAR, sErrorStack );

    STL_TRY( dtdTimestampToString( sSourceValue,
                                   DTL_PRECISION_NA,
                                   DTL_SCALE_NA,
                                   DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE,
                                   sTimestampString,
                                   & sTimestampStringLen,
                                   aVectorFunc,
                                   aVectorArg,
                                   sErrorStack )
             == STL_SUCCESS );    
    sTimestampString[sTimestampStringLen] = '\0';

    STL_TRY( dtdCharSetValueFromString(
                 sTimestampString,
                 sTimestampStringLen,
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
 * @brief Timestamp -> Varchar 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastTimestampToVarchar( stlUInt16        aInputArgumentCnt,
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
    
    stlChar         sTimestampString[DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE + 1];
    stlInt64        sTimestampStringLen;

    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
    sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
    sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
    sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;

    sResultValue = (dtlDataValue *)aResultValue;

    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_TIMESTAMP, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_VARCHAR, sErrorStack );

    STL_TRY( dtdTimestampToString( sSourceValue,
                                   DTL_PRECISION_NA,
                                   DTL_SCALE_NA,                                   
                                   DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE,
                                   sTimestampString,
                                   & sTimestampStringLen,
                                   aVectorFunc,
                                   aVectorArg,
                                   sErrorStack )
             == STL_SUCCESS );    
    sTimestampString[sTimestampStringLen] = '\0';

    STL_TRY( dtdVarcharSetValueFromString(
                 sTimestampString,
                 sTimestampStringLen,
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
 * @brief Timestamp -> Longvarchar 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastTimestampToLongvarchar( stlUInt16        aInputArgumentCnt,
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
    
    stlChar         sTimestampString[DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE + 1];
    stlInt64        sTimestampStringLen;

    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
    sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
    sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
    sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;

    sResultValue = (dtlDataValue *)aResultValue;

    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_TIMESTAMP, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_LONGVARCHAR, sErrorStack );

    STL_TRY( dtdTimestampToString( sSourceValue,
                                   DTL_PRECISION_NA,
                                   DTL_SCALE_NA,
                                   DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE,
                                   sTimestampString,
                                   & sTimestampStringLen,
                                   aVectorFunc,
                                   aVectorArg,
                                   sErrorStack )
             == STL_SUCCESS );    
    sTimestampString[sTimestampStringLen] = '\0';

    STL_TRY( dtdLongvarcharSetValueFromString(
                 sTimestampString,
                 sTimestampStringLen,
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
 * @brief Timestamp -> Date 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastTimestampToDate( stlUInt16        aInputArgumentCnt,
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

    dtlDataValue        * sResultValue;

    dtlDateType           sDateType;
    dtlTimestampType      sTimestampType;

    stlErrorStack       * sErrorStack;

    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
//  sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
//  sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
//  sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
//  sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;
    
    sResultValue = (dtlDataValue *)aResultValue;
    
    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_TIMESTAMP, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_DATE, sErrorStack );

    sTimestampType = *(dtlTimestampType *)(sSourceValue->mValue);
    
    sDateType = sTimestampType;
    
    STL_TRY( dtdAdjustDate( & sDateType,
                            sErrorStack )
             == STL_SUCCESS );
    
    *(dtlDateType *)(sResultValue->mValue) = sDateType;
    sResultValue->mLength = DTL_DATE_SIZE;
    
    return STL_SUCCESS;
    
    STL_FINISH;    
    
    return STL_FAILURE;    
}

/**
 * @brief Timestamp -> Time 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastTimestampToTime( stlUInt16        aInputArgumentCnt,
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
//  dtlDataValue  * sDestIntervalIndicatorValue;  // aInputArgument[4].mValue.mDataValue

    dtlDataValue        * sResultValue;

    dtlTimeType           sTimeType;
    dtlDateType           sDate;
    dtlTimestampType      sTimestampType;

    stlErrorStack       * sErrorStack;

    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
//  sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
//  sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
//  sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;
    
    sResultValue = (dtlDataValue *)aResultValue;
    
    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_TIMESTAMP, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIME, sErrorStack );
    
    sTimestampType = *(dtlTimestampType *)(sSourceValue->mValue);
    
    DTL_TIMESTAMP_MODULO( sTimestampType, sDate, DTL_USECS_PER_DAY );
    
    sTimeType = sTimestampType;
    STL_TRY( dtdAdjustTime( & sTimeType,
                            *(stlInt64 *)(sDestPrecisionValue->mValue),
                            sErrorStack )
             == STL_SUCCESS );

    sTimeType -= sTimeType / DTL_USECS_PER_DAY * DTL_USECS_PER_DAY;
    
    if( sTimeType < 0 )
    {
        sTimeType += DTL_USECS_PER_DAY;
    }
    else
    {
        // Do Nothing
    }

    *(dtlTimeType *)(sResultValue->mValue) = sTimeType;
    sResultValue->mLength = DTL_TIME_SIZE;
    
    return STL_SUCCESS;
    
    STL_FINISH;    
    
    return STL_FAILURE;    
}

/* /\** */
/*  * @brief Timestamp -> TimeTz 으로의 type cast */
/*  * @param[in]  aInputArgumentCnt input argument count */
/*  * @param[in]  aInputArgument    input argument */
/*  * @param[out] aResultValue      cast연산결과 */
/*  * @param[in,out]  aInfo         function 수행에 필요한 부가 정보 */
/*  * @param[in]  aVectorFunc       Function Vector */
/*  * @param[in]  aVectorArg        Vector Argument */
/*  * @param[out] aEnv              environment (stlErrorStack)  */
/*  *\/ */
/* stlStatus dtcCastTimestampToTimeTz( stlUInt16        aInputArgumentCnt, */
/*                                     dtlValueEntry  * aInputArgument, */
/*                                     void           * aResultValue, */
/*                                     void           * aInfo, */
/*                                     dtlFuncVector  * aVectorFunc, */
/*                                     void           * aVectorArg, */
/*                                     void           * aEnv ) */
/* { */
/*     /\** */
/*      * @todo timestamp without time zone -> time with time zone */
/*      *       표준에서 cast 지원된다고 되어 있음. */
/*      *\/ */
    
/*     return STL_FAILURE;     */
/* } */

/**
 * @brief Timestamp -> Timestamp 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastTimestampToTimestamp( stlUInt16        aInputArgumentCnt,
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
//  dtlDataValue  * sDestIntervalIndicatorValue;  // aInputArgument[4].mValue.mDataValue

    dtlDataValue  * sResultValue;

    dtlTimestampType sTimestampType;
    stlBool          sIsValid;

    stlErrorStack * sErrorStack;

    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
//  sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
//  sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
//  sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;
    
    sResultValue = (dtlDataValue *)aResultValue;
    
    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_TIMESTAMP, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIMESTAMP, sErrorStack );

    sTimestampType = *(dtlTimestampType *)(sSourceValue->mValue);
    
    STL_TRY( dtdAdjustTimestamp( & sTimestampType,
                                 *(stlInt64 *)(sDestPrecisionValue->mValue),
                                 sErrorStack )
             == STL_SUCCESS );
    
    *(dtlTimestampType *)(sResultValue->mValue) = sTimestampType;
    sResultValue->mLength = DTL_TIMESTAMP_SIZE;
    
    DTF_DATETIME_VALUE_IS_VALID( sTimestampType, sIsValid );
    STL_TRY_THROW( sIsValid == STL_TRUE, ERROR_OUT_OF_RANGE );
    
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATETIME_FIELD_OVERFLOW,
                      NULL,
                      sErrorStack );
    }
    
    STL_FINISH;    
    
    return STL_FAILURE;    
}

/**
 * @brief Timestamp -> TimestampTz 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastTimestampToTimestampTz( stlUInt16        aInputArgumentCnt,
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
//  dtlDataValue  * sDestIntervalIndicatorValue;  // aInputArgument[4].mValue.mDataValue

    dtlDataValue  * sResultValue;
    
    dtlTimestampType   sTimestamp;
    stlInt32           sTimeZone;    
    stlBool            sIsValid;
    
    stlErrorStack * sErrorStack;
    
    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
//  sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
//  sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
//  sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;
    
    sResultValue = (dtlDataValue *)aResultValue;
    
    sErrorStack = (stlErrorStack *)aEnv;
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_TIMESTAMP, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, sErrorStack );
    
    sTimestamp = *(dtlTimestampType *)(sSourceValue->mValue);
    
    STL_TRY( dtdAdjustTimestamp( & sTimestamp,
                                 *(stlInt64 *)(sDestPrecisionValue->mValue),
                                 sErrorStack )
             == STL_SUCCESS );
    
    DTF_DATETIME_VALUE_IS_VALID( sTimestamp, sIsValid );
    STL_TRY_THROW( sIsValid == STL_TRUE, ERROR_OUT_OF_RANGE );
    
    sTimeZone = -(aVectorFunc->mGetGMTOffsetFunc(aVectorArg));
    
    dtdTimestamp2Local( sTimestamp,
                        -sTimeZone,
                        & sTimestamp );
    
    ((dtlTimestampTzType *)(sResultValue->mValue))->mTimestamp = sTimestamp;
    ((dtlTimestampTzType *)(sResultValue->mValue))->mTimeZone  = sTimeZone;
    sResultValue->mLength = DTL_TIMESTAMPTZ_SIZE;
    
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATETIME_FIELD_OVERFLOW,
                      NULL,
                      sErrorStack );
    }
    
    STL_FINISH;    
    
    return STL_FAILURE;    
}


/** @} */
