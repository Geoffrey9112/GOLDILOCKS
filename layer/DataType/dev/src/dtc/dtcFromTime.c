/*******************************************************************************
 * dtcFromTime.c
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
 * @file dtcFromTime.c
 * @brief DataType Layer Time으로부터 다른 타입으로의 변환 
 */

#include <dtl.h>
#include <dtDef.h>

#include <dtcCast.h>
#include <dtfFormatting.h>

/**
 * @ingroup dtcFromTime
 * @{
 */

/**
 * @brief Time -> Char 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastTimeToChar( stlUInt16        aInputArgumentCnt,
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
    
    stlChar         sTimeString[DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE + 1];
    stlInt64        sTimeStringLen;
    
    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
    sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
    sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
    sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;

    sResultValue = (dtlDataValue *)aResultValue;

    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_TIME, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_CHAR, sErrorStack );

    STL_TRY( dtdTimeToString( sSourceValue,
                              DTL_PRECISION_NA,
                              DTL_SCALE_NA,
                              DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE,
                              sTimeString,
                              & sTimeStringLen,
                              aVectorFunc,
                              aVectorArg,
                              sErrorStack )
             == STL_SUCCESS );    
    sTimeString[sTimeStringLen] = '\0';    

    STL_TRY( dtdCharSetValueFromString(
                 sTimeString,
                 sTimeStringLen,
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
 * @brief Time -> Varchar 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastTimeToVarchar( stlUInt16        aInputArgumentCnt,
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
    
    stlChar         sTimeString[DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE + 1];
    stlInt64        sTimeStringLen;

    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
    sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
    sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
    sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;

    sResultValue = (dtlDataValue *)aResultValue;

    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_TIME, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_VARCHAR, sErrorStack );

    STL_TRY( dtdTimeToString( sSourceValue,
                              DTL_PRECISION_NA,
                              DTL_SCALE_NA,
                              DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE,
                              sTimeString,
                              & sTimeStringLen,
                              aVectorFunc,
                              aVectorArg,
                              sErrorStack )
             == STL_SUCCESS );    
    sTimeString[sTimeStringLen] = '\0';    

    STL_TRY( dtdVarcharSetValueFromString(
                 sTimeString,
                 sTimeStringLen,
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
 * @brief Time -> Longvarchar 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastTimeToLongvarchar( stlUInt16        aInputArgumentCnt,
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
    
    stlChar         sTimeString[DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE + 1];
    stlInt64        sTimeStringLen;

    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
    sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
    sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
    sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;

    sResultValue = (dtlDataValue *)aResultValue;

    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_TIME, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_LONGVARCHAR, sErrorStack );

    STL_TRY( dtdTimeToString( sSourceValue,
                              DTL_PRECISION_NA,
                              DTL_SCALE_NA,
                              DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE,
                              sTimeString,
                              & sTimeStringLen,
                              aVectorFunc,
                              aVectorArg,
                              sErrorStack )
             == STL_SUCCESS );    
    sTimeString[sTimeStringLen] = '\0';    

    STL_TRY( dtdLongvarcharSetValueFromString(
                 sTimeString,
                 sTimeStringLen,
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
 * @brief Time -> Time 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastTimeToTime( stlUInt16        aInputArgumentCnt,
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

    stlErrorStack * sErrorStack;

    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
//  sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
//  sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
//  sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;
    
    sResultValue = (dtlDataValue *)aResultValue;
    
    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_TIME, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIME, sErrorStack );

    sTimeType = *(dtlTimeType *)(sSourceValue->mValue);
    
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
 * @brief Time -> TimeTz 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastTimeToTimeTz( stlUInt16        aInputArgumentCnt,
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

    dtlTimeType            sTimeType;
    stlInt32               sTimeZone;

    stlErrorStack        * sErrorStack;

    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
//  sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
//  sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
//  sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;
    
    sResultValue = (dtlDataValue *)aResultValue;
    
    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_TIME, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIME_WITH_TIME_ZONE, sErrorStack );
    
    sTimeType = *(dtlTimeType *)(sSourceValue->mValue);
    
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
    
    sTimeZone = -(aVectorFunc->mGetGMTOffsetFunc(aVectorArg));
    
    dtdTime2Local( sTimeType,
                   -sTimeZone,
                   & sTimeType );
    
    ((dtlTimeTzType *)(sResultValue->mValue))->mTime = sTimeType;
    ((dtlTimeTzType *)(sResultValue->mValue))->mTimeZone = sTimeZone;
    
    sResultValue->mLength = DTL_TIMETZ_SIZE;
    
    return STL_SUCCESS;
    
    STL_FINISH;    
    
    return STL_FAILURE;    
}


/* /\** */
/*  * @brief Time -> Timestamp 으로의 type cast */
/*  * @param[in]  aInputArgumentCnt input argument count */
/*  * @param[in]  aInputArgument    input argument */
/*  * @param[out] aResultValue      cast연산결과 */
/*  * @param[in,out]  aInfo         function 수행에 필요한 부가 정보 */
/*  * @param[in]  aVectorFunc       Function Vector */
/*  * @param[in]  aVectorArg        Vector Argument */
/*  * @param[out] aEnv              environment (stlErrorStack)  */
/*  *\/ */
/* stlStatus dtcCastTimeToTimestamp( stlUInt16        aInputArgumentCnt, */
/*                                   dtlValueEntry  * aInputArgument, */
/*                                   void           * aResultValue, */
/*                                   void           * aInfo, */
/*                                   dtlFuncVector  * aVectorFunc, */
/*                                   void           * aVectorArg, */
/*                                   void           * aEnv ) */
/* { */
/*     /\** */
/*      * @todo time without time zone -> timestamp without time zone */
/*      *       표준에서 cast 지원된다고 되어 있음. */
/*      *\/ */
    
/*     return STL_FAILURE;     */
/* } */

/* /\** */
/*  * @brief Time -> TimestampTz 으로의 type cast */
/*  * @param[in]  aInputArgumentCnt input argument count */
/*  * @param[in]  aInputArgument    input argument */
/*  * @param[out] aResultValue      cast연산결과 */
/*  * @param[in,out]  aInfo         function 수행에 필요한 부가 정보 */
/*  * @param[in]  aVectorFunc       Function Vector */
/*  * @param[in]  aVectorArg        Vector Argument */
/*  * @param[out] aEnv              environment (stlErrorStack)  */
/*  *\/ */
/* stlStatus dtcCastTimeToTimestampTz( stlUInt16        aInputArgumentCnt, */
/*                                     dtlValueEntry  * aInputArgument, */
/*                                     void           * aResultValue, */
/*                                     void           * aInfo, */
/*                                     dtlFuncVector  * aVectorFunc, */
/*                                     void           * aVectorArg, */
/*                                     void           * aEnv ) */
/* { */
/*     /\** */
/*      * @todo time without time zone -> timestamp with time zone */
/*      *       표준에서 cast 지원된다고 되어 있음. */
/*      *\/ */
    
/*     return STL_FAILURE;     */
/* } */



/** @} */
