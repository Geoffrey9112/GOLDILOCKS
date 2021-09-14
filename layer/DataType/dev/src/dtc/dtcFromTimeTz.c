/*******************************************************************************
 * dtcFromTimeTz.c
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
 * @file dtcFromTimeTz.c
 * @brief DataType Layer TimeTz으로부터 다른 타입으로의 변환 
 */

#include <dtl.h>
#include <dtDef.h>

#include <dtcCast.h>
#include <dtfFormatting.h>

/**
 * @ingroup dtcFromTimeTz
 * @{
 */

/**
 * @brief TimeTz -> Char 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastTimeTzToChar( stlUInt16        aInputArgumentCnt,
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
    
    stlChar         sTimeTzString[DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE + 1];
    stlInt64        sTimeTzStringLen;

    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
    sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
    sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
    sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;

    sResultValue = (dtlDataValue *)aResultValue;

    sErrorStack = (stlErrorStack *)aEnv;
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_TIME_WITH_TIME_ZONE, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_CHAR, sErrorStack );

    STL_TRY( dtdTimeTzToString( sSourceValue,
                                DTL_PRECISION_NA,
                                DTL_SCALE_NA,
                                DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE,
                                sTimeTzString,
                                & sTimeTzStringLen,
                                aVectorFunc,
                                aVectorArg,
                                sErrorStack )
             == STL_SUCCESS );
    sTimeTzString[sTimeTzStringLen] = '\0';    
    
    STL_TRY( dtdCharSetValueFromString(
                 sTimeTzString,
                 sTimeTzStringLen,
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
 * @brief TimeTz -> Varchar 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastTimeTzToVarchar( stlUInt16        aInputArgumentCnt,
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
    
    stlChar         sTimeTzString[DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE + 1];
    stlInt64        sTimeTzStringLen;

    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
    sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
    sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
    sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;

    sResultValue = (dtlDataValue *)aResultValue;

    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_TIME_WITH_TIME_ZONE, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_VARCHAR, sErrorStack );

    STL_TRY( dtdTimeTzToString( sSourceValue,
                                DTL_PRECISION_NA,
                                DTL_SCALE_NA,
                                DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE,
                                sTimeTzString,
                                & sTimeTzStringLen,
                                aVectorFunc,
                                aVectorArg,
                                sErrorStack )
             == STL_SUCCESS );
    sTimeTzString[sTimeTzStringLen] = '\0';    
    
    STL_TRY( dtdVarcharSetValueFromString(
                 sTimeTzString,
                 sTimeTzStringLen,
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
 * @brief TimeTz -> Longvarchar 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastTimeTzToLongvarchar( stlUInt16        aInputArgumentCnt,
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
    
    stlChar         sTimeTzString[DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE + 1];
    stlInt64        sTimeTzStringLen;

    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
    sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
    sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
    sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;

    sResultValue = (dtlDataValue *)aResultValue;

    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_TIME_WITH_TIME_ZONE, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_LONGVARCHAR, sErrorStack );

    STL_TRY( dtdTimeTzToString( sSourceValue,
                                DTL_PRECISION_NA,
                                DTL_SCALE_NA,
                                DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE,
                                sTimeTzString,
                                & sTimeTzStringLen,
                                aVectorFunc,
                                aVectorArg,
                                sErrorStack )
             == STL_SUCCESS );
    sTimeTzString[sTimeTzStringLen] = '\0';    
    
    STL_TRY( dtdLongvarcharSetValueFromString(
                 sTimeTzString,
                 sTimeTzStringLen,
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
 * @brief TimeTz -> Time 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastTimeTzToTime( stlUInt16        aInputArgumentCnt,
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

    dtlTimeType     sTimeType;
    dtlTimeTzType * sTimeTzType;

    stlErrorStack * sErrorStack;

    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
//  sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
//  sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
//  sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;
    
    sResultValue = (dtlDataValue *)aResultValue;
    
    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_TIME_WITH_TIME_ZONE, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIME, sErrorStack );
    
    sTimeTzType = (dtlTimeTzType *)(sSourceValue->mValue);
    
    sTimeType = sTimeTzType->mTime - (sTimeTzType->mTimeZone * DTL_USECS_PER_SEC);
    
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

/**
 * @brief TimeTz -> TimeTz 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastTimeTzToTimeTz( stlUInt16        aInputArgumentCnt,
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

    dtlTimeType     sTime;    

    stlErrorStack * sErrorStack;

    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
//  sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
//  sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
//  sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;
    
    sResultValue = (dtlDataValue *)aResultValue;
    
    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_TIME_WITH_TIME_ZONE, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIME_WITH_TIME_ZONE, sErrorStack );
    
    sTime = (((dtlTimeTzType *)(sSourceValue->mValue))->mTime) -
        (((dtlTimeTzType *)(sSourceValue->mValue))->mTimeZone * DTL_USECS_PER_SEC);
    
    STL_TRY( dtdAdjustTime( & sTime,
                            *(stlInt64 *)(sDestPrecisionValue->mValue),
                            sErrorStack )
             == STL_SUCCESS );
    
    sTime -= sTime / DTL_USECS_PER_DAY * DTL_USECS_PER_DAY;
    
    if( sTime < 0 )
    {
        sTime += DTL_USECS_PER_DAY;
    }
    else
    {
        // Do Nothing
    }
    
    ((dtlTimeTzType *)(sResultValue->mValue))->mTime =
        sTime + (((dtlTimeTzType *)(sSourceValue->mValue))->mTimeZone * DTL_USECS_PER_SEC);
    ((dtlTimeTzType *)(sResultValue->mValue))->mTimeZone =
        ((dtlTimeTzType *)(sSourceValue->mValue))->mTimeZone;
    
    sResultValue->mLength = DTL_TIMETZ_SIZE;
    
    return STL_SUCCESS;
    
    STL_FINISH;    
    
    return STL_FAILURE;    
}

/* /\** */
/*  * @brief TimeTz -> Timestamp 으로의 type cast */
/*  * @param[in]  aInputArgumentCnt input argument count */
/*  * @param[in]  aInputArgument    input argument */
/*  * @param[out] aResultValue      cast연산결과 */
/*  * @param[in,out]  aInfo         function 수행에 필요한 부가 정보 */
/*  * @param[in]  aVectorFunc       Function Vector */
/*  * @param[in]  aVectorArg        Vector Argument */
/*  * @param[out] aEnv              environment (stlErrorStack)  */
/*  *\/ */
/* stlStatus dtcCastTimeTzToTimestamp( stlUInt16        aInputArgumentCnt, */
/*                                     dtlValueEntry  * aInputArgument, */
/*                                     void           * aResultValue, */
/*                                     void           * aInfo, */
/*                                     dtlFuncVector  * aVectorFunc, */
/*                                     void           * aVectorArg, */
/*                                     void           * aEnv ) */
/* { */
/*     /\** */
/*      * @todo time with time zone -> timestamp without time zone */
/*      *       표준에서 cast 지원된다고 되어 있음. */
/*      *\/ */
    
/*     return STL_FAILURE;     */
/* } */

/* /\** */
/*  * @brief TimeTz -> TimestampTz 으로의 type cast */
/*  * @param[in]  aInputArgumentCnt input argument count */
/*  * @param[in]  aInputArgument    input argument */
/*  * @param[out] aResultValue      cast연산결과 */
/*  * @param[in,out]  aInfo         function 수행에 필요한 부가 정보 */
/*  * @param[in]  aVectorFunc       Function Vector */
/*  * @param[in]  aVectorArg        Vector Argument */
/*  * @param[out] aEnv              environment (stlErrorStack)  */
/*  *\/ */
/* stlStatus dtcCastTimeTzToTimestampTz( stlUInt16        aInputArgumentCnt, */
/*                                       dtlValueEntry  * aInputArgument, */
/*                                       void           * aResultValue, */
/*                                       void           * aInfo, */
/*                                       dtlFuncVector  * aVectorFunc, */
/*                                       void           * aVectorArg, */
/*                                       void           * aEnv ) */
/* { */
/*     /\** */
/*      * @todo time with time zone -> timestamp with time zone */
/*      *       표준에서 cast 지원된다고 되어 있음. */
/*      *\/ */
    
/*     return STL_FAILURE;     */
/* } */

/** @} */
