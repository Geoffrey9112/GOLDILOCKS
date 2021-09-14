/*******************************************************************************
 * dtfDateDiff.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: $
 *
 * NOTES
 * 
 *
 ******************************************************************************/

/**
 * @file dtfDateDiff.c
 * @brief Date and Time Function DataType Library Layer
 */

#include <dtl.h>
#include <dtlDef.h>
#include <dtDef.h>
#include <dtfNumeric.h>

/**
 * @ingroup dtf
 * @internal
 * @{
 */

#define DTF_DATETIME_TO_DTLEXPTIME( aStartDateTime,                                                 \
                                    aStartDateDtlExpTime,                                           \
                                    aStartDateFractionalSecond,                                     \
                                    aEndDateTime,                                                   \
                                    aEndDateDtlExpTime,                                             \
                                    aEndDateFractionalSecond,                                       \
                                    aErrorStack )                                                   \
    {                                                                                               \
        DTL_INIT_DTLEXPTIME( (aStartDateDtlExpTime) );                                              \
        STL_TRY( dtdDateTime2dtlExpTime( (aStartDateTime),                                          \
                                         & (aStartDateDtlExpTime),                                  \
                                         & (aStartDateFractionalSecond),                            \
                                         (aErrorStack) )                                            \
                 == STL_SUCCESS );                                                                  \
        DTL_INIT_DTLEXPTIME( (aEndDateDtlExpTime) );                                                \
        STL_TRY( dtdDateTime2dtlExpTime( (aEndDateTime),                                            \
                                         & (aEndDateDtlExpTime),                                    \
                                         & (aEndDateFractionalSecond),                              \
                                         (aErrorStack) )                                            \
                 == STL_SUCCESS );                                                                  \
    }

#define DTF_TIME_TO_DTLEXPTIME( aStartDateTime,                                                     \
                                aStartDateDtlExpTime,                                               \
                                aStartDateFractionalSecond,                                         \
                                aEndDateTime,                                                       \
                                aEndDateDtlExpTime,                                                 \
                                aEndDateFractionalSecond )                                          \
    {                                                                                               \
        DTL_INIT_DTLEXPTIME( (aStartDateDtlExpTime) );                                              \
        dtdTime2dtlExpTime( (aStartDateTime),                                                       \
                            & (aStartDateDtlExpTime),                                               \
                            & (aStartDateFractionalSecond) );                                       \
        DTL_INIT_DTLEXPTIME( (aEndDateDtlExpTime) );                                                \
        dtdTime2dtlExpTime( (aEndDateTime),                                                         \
                            & (aEndDateDtlExpTime),                                                 \
                            & (aEndDateFractionalSecond) );                                         \
    }

#define DTF_DATEDIFF_PART_YEAR( aStartDateDtlExpTime, aEndDateDtlExpTime, aDiffValue )     \
    {                                                                                      \
        (aDiffValue) = (aEndDateDtlExpTime).mYear - (aStartDateDtlExpTime).mYear;          \
    }

#define DTF_DATEDIFF_PART_QUARTER( aStartDateDtlExpTime, aEndDateDtlExpTime, aDiffValue )           \
    {                                                                                               \
        (aDiffValue) = ( (aEndDateDtlExpTime).mYear - (aStartDateDtlExpTime).mYear ) * DTL_QUARTERS_PER_YEAR; \
        (aDiffValue) +=                                                                             \
            ( ( (aEndDateDtlExpTime).mMonth - 1 ) / DTL_MONTHS_PER_QUARTER + 1 )                    \
            - ( ( (aStartDateDtlExpTime).mMonth - 1 ) / DTL_MONTHS_PER_QUARTER + 1 );               \
    }

#define DTF_DATEDIFF_PART_MONTH( aStartDateDtlExpTime, aEndDateDtlExpTime, aDiffValue )             \
    {                                                                                               \
        (aDiffValue) = ( ( (aEndDateDtlExpTime).mYear - (aStartDateDtlExpTime).mYear ) * DTL_MONTHS_PER_YEAR ); \
        (aDiffValue) += ( (aEndDateDtlExpTime).mMonth - (aStartDateDtlExpTime).mMonth );            \
    }

/*
 * date, timestamp, timestamp tz type의 value는
 * 지정된 날짜를 2000-01-01을 기준으로 저장되므로,
 * datediff 계산을 위해 이 값을 더해서 지정된 날짜로 만든다.
 * DTF_EPOCH_JDATE_MICROSECOND = DTL_EPOCH_JDATE(2451545) * DTL_USECS_PER_DAY(86400000000)
 */
#define DTF_EPOCH_JDATE_MICROSECOND ( STL_INT64_C(211813488000000000) )

#define DTF_DATEDIFF_PART_DAY( aStartDateTime, aEndDateTime, aDiffValue )                           \
    {                                                                                               \
        (aStartDateTime) = (aStartDateTime) + DTF_EPOCH_JDATE_MICROSECOND;                          \
        (aEndDateTime) = (aEndDateTime) + DTF_EPOCH_JDATE_MICROSECOND;                              \
        (aDiffValue) = ( (aEndDateTime) / DTL_USECS_PER_DAY ) - ( (aStartDateTime) / DTL_USECS_PER_DAY ); \
    }

#define DTF_DATEDIFF_PART_HOUR( aStartDateTime, aEndDateTime, aDiffValue )                          \
    {                                                                                               \
        (aStartDateTime) = (aStartDateTime) + DTF_EPOCH_JDATE_MICROSECOND;                          \
        (aEndDateTime) = (aEndDateTime) + DTF_EPOCH_JDATE_MICROSECOND;                              \
        (aDiffValue) = ( (aEndDateTime) / DTL_USECS_PER_HOUR ) - ( (aStartDateTime) / DTL_USECS_PER_HOUR ); \
    }

#define DTF_DATEDIFF_PART_MINUTE( aStartDateTime, aEndDateTime, aDiffValue )                        \
    {                                                                                               \
        (aStartDateTime) = (aStartDateTime) + DTF_EPOCH_JDATE_MICROSECOND;                          \
        (aEndDateTime) = (aEndDateTime) + DTF_EPOCH_JDATE_MICROSECOND;                              \
        (aDiffValue) = ( (aEndDateTime) / DTL_USECS_PER_MINUTE ) - ( (aStartDateTime) / DTL_USECS_PER_MINUTE ); \
    }

#define DTF_DATEDIFF_PART_SECOND( aStartDateTime, aEndDateTime, aDiffValue )                        \
    {                                                                                               \
        (aStartDateTime) = (aStartDateTime) + DTF_EPOCH_JDATE_MICROSECOND;                          \
        (aEndDateTime) = (aEndDateTime) + DTF_EPOCH_JDATE_MICROSECOND;                              \
        (aDiffValue) = ( (aEndDateTime) / DTL_USECS_PER_SEC ) - ( (aStartDateTime) / DTL_USECS_PER_SEC ); \
    }

#define DTF_DATEDIFF_PART_MILLISECOND( aStartDateTime, aEndDateTime, aDiffValue )                   \
    {                                                                                               \
        (aStartDateTime) = (aStartDateTime) + DTF_EPOCH_JDATE_MICROSECOND;                          \
        (aEndDateTime) = (aEndDateTime) + DTF_EPOCH_JDATE_MICROSECOND;                              \
        (aDiffValue) = ( (aEndDateTime) / DTL_USECS_PER_MILLISEC ) - ( (aStartDateTime) / DTL_USECS_PER_MILLISEC ); \
    }

#define DTF_DATEDIFF_PART_MICROSECOND( aStartDateTime, aEndDateTime, aDiffValue )                   \
    {                                                                                               \
        (aStartDateTime) = (aStartDateTime) + DTF_EPOCH_JDATE_MICROSECOND;                          \
        (aEndDateTime) = (aEndDateTime) + DTF_EPOCH_JDATE_MICROSECOND;                              \
        (aDiffValue) = (aEndDateTime) - (aStartDateTime);                                           \
    }



/**
 * @brief DATEDIFF( datepart, startdate, enddate )
 *        <BR>   DATEDIFF( datepart, startdate, enddate ) => [ mssql ] 
 *        <BR> ex) DATEDIFF( DAY, DATE'2014-05-10', DATE'2014-05-12' ) =>  2
 *        <BR> ex) DATEDIFF( DAY, DATE'2014-05-12', DATE'2014-05-10' ) => -2
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> DATEDIFF( NUMBER, DATE, DATE )
 * @param[out] aResultValue      연산 결과 (결과타입 NUMBER)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfDateDiffForDate( stlUInt16        aInputArgumentCnt,
                              dtlValueEntry  * aInputArgument,
                              void           * aResultValue,
                              void           * aInfo,
                              dtlFuncVector  * aVectorFunc,
                              void           * aVectorArg,
                              void           * aEnv )
{
    dtlDataValue   * sValue1;
    dtlDataValue   * sValue2;
    dtlDataValue   * sValue3;    
    dtlDataValue   * sResultValue;
    stlErrorStack  * sErrorStack;

    stlUInt8       * sNumericDataPtr;    
    stlInt32         sDigitCount = 0;
    stlInt32         sDateTimePart = 0;

    dtlDateType      sStartDateTime = 0;
    dtlDateType      sEndDateTime   = 0;

    dtlExpTime       sStartDateDtlExpTime;
    dtlExpTime       sEndDateDtlExpTime;    

    dtlFractionalSecond sStartDateFractionalSecond = 0;
    dtlFractionalSecond sEndDateFractionalSecond = 0;

    stlInt64         sDiffValue = 0;


    DTL_PARAM_VALIDATE( aInputArgumentCnt == 3, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sValue3 = aInputArgument[2].mValue.mDataValue;    
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;
    
    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_DATE, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue3, DTL_TYPE_DATE, sErrorStack );    
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NUMBER, sErrorStack );
    
    /*
     * datetime part value ( sValue1 )
     */ 
    
    sDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( sValue1->mLength );
    
    if( sDigitCount == 1 )
    {
        sNumericDataPtr = DTL_NUMERIC_GET_DIGIT_PTR( DTF_NUMERIC(sValue1) );
        sDateTimePart = DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sNumericDataPtr );
    }
    else
    {
        STL_THROW( ERROR_INVALID_DATEPART );
    }
    
    /*
     * startdate ( sValue2 )
     */ 
    
    sStartDateTime = DTF_DATE( sValue2 );

    /*
     * enddate ( sValue3 )
     */ 
    
    sEndDateTime = DTF_DATE( sValue3 );
    
    /*
     * datepart에 대한 diff 연산
     */

    switch( sDateTimePart )
    {
        case DTL_DATETIME_PART_YEAR:
            {
                DTF_DATETIME_TO_DTLEXPTIME( sStartDateTime,
                                            sStartDateDtlExpTime,
                                            sStartDateFractionalSecond,
                                            sEndDateTime,
                                            sEndDateDtlExpTime,
                                            sEndDateFractionalSecond,
                                            sErrorStack );
                DTF_DATEDIFF_PART_YEAR( sStartDateDtlExpTime, sEndDateDtlExpTime, sDiffValue );
                
                break;
            }
        case DTL_DATETIME_PART_QUARTER:
            {
                DTF_DATETIME_TO_DTLEXPTIME( sStartDateTime,
                                            sStartDateDtlExpTime,
                                            sStartDateFractionalSecond,
                                            sEndDateTime,
                                            sEndDateDtlExpTime,
                                            sEndDateFractionalSecond,
                                            sErrorStack );
                DTF_DATEDIFF_PART_QUARTER( sStartDateDtlExpTime, sEndDateDtlExpTime, sDiffValue );
                
                break;
            }
        case DTL_DATETIME_PART_MONTH:
            {
                DTF_DATETIME_TO_DTLEXPTIME( sStartDateTime,
                                            sStartDateDtlExpTime,
                                            sStartDateFractionalSecond,
                                            sEndDateTime,
                                            sEndDateDtlExpTime,
                                            sEndDateFractionalSecond,
                                            sErrorStack );
                DTF_DATEDIFF_PART_MONTH( sStartDateDtlExpTime, sEndDateDtlExpTime, sDiffValue );
                
                break;
            }
        case DTL_DATETIME_PART_DAY:
        case DTL_DATETIME_PART_DAYOFYEAR:            
            {
                DTF_DATEDIFF_PART_DAY( sStartDateTime, sEndDateTime, sDiffValue );
                
                break;
            }
        case DTL_DATETIME_PART_HOUR:
            {
                DTF_DATEDIFF_PART_HOUR( sStartDateTime, sEndDateTime, sDiffValue );
                
                break;
            }
        case DTL_DATETIME_PART_MINUTE:
            {
                DTF_DATEDIFF_PART_MINUTE( sStartDateTime, sEndDateTime, sDiffValue );
                
                break;
            }
        case DTL_DATETIME_PART_SECOND:
            {
                DTF_DATEDIFF_PART_SECOND( sStartDateTime, sEndDateTime, sDiffValue );
                
                break;
            }
        case DTL_DATETIME_PART_MILLISECOND:
            {
                DTF_DATEDIFF_PART_MILLISECOND( sStartDateTime, sEndDateTime, sDiffValue );
                
                break;
            }
        case DTL_DATETIME_PART_MICROSECOND:
            {
                DTF_DATEDIFF_PART_MICROSECOND( sStartDateTime, sEndDateTime, sDiffValue );
                
                break;
            }
        default:
            STL_THROW( ERROR_INVALID_DATEPART );
            break;
    }
    
    /* datediff 연산 수행결과를 numeric type으로 변환. */
    STL_TRY( dtdToNumericFromInt64( sDiffValue,
                                    gResultDataTypeDef[DTL_TYPE_NUMBER].mArgNum1,
                                    gResultDataTypeDef[DTL_TYPE_NUMBER].mArgNum2,
                                    sResultValue,
                                    sErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_INVALID_DATEPART )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_DATEPART_FOR_DATA_TYPE,
                      NULL,
                      sErrorStack,
                      dtlDataTypeName[ DTL_TYPE_DATE ] );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief DATEDIFF( datepart, startdate, enddate )
 *        <BR>   DATEDIFF( datepart, startdate, enddate ) => [ mssql ] 
 *        <BR> ex) DATEDIFF( DAY, DATE'2014-05-10', DATE'2014-05-12' ) =>  2
 *        <BR> ex) DATEDIFF( DAY, DATE'2014-05-12', DATE'2014-05-10' ) => -2
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> DATEDIFF( NUMBER, TIMESTAMP, TIMESTAMP )
 * @param[out] aResultValue      연산 결과 (결과타입 NUMBER)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfDateDiffForTimestamp( stlUInt16        aInputArgumentCnt,
                                   dtlValueEntry  * aInputArgument,
                                   void           * aResultValue,
                                   void           * aInfo,
                                   dtlFuncVector  * aVectorFunc,
                                   void           * aVectorArg,
                                   void           * aEnv )
{
    dtlDataValue   * sValue1;
    dtlDataValue   * sValue2;
    dtlDataValue   * sValue3;    
    dtlDataValue   * sResultValue;
    stlErrorStack  * sErrorStack;

    stlUInt8       * sNumericDataPtr;    
    stlInt32         sDigitCount = 0;
    stlInt32         sDateTimePart = 0;

    dtlTimestampType sStartDateTime = 0;
    dtlTimestampType sEndDateTime   = 0;

    dtlExpTime       sStartDateDtlExpTime;
    dtlExpTime       sEndDateDtlExpTime;    

    dtlFractionalSecond sStartDateFractionalSecond = 0;
    dtlFractionalSecond sEndDateFractionalSecond = 0;

    stlInt64         sDiffValue = 0;


    DTL_PARAM_VALIDATE( aInputArgumentCnt == 3, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sValue3 = aInputArgument[2].mValue.mDataValue;    
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;
    
    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_TIMESTAMP, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue3, DTL_TYPE_TIMESTAMP, sErrorStack );    
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NUMBER, sErrorStack );
    
    /*
     * datetime part value ( sValue1 )
     */ 
    
    sDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( sValue1->mLength );
    
    if( sDigitCount == 1 )
    {
        sNumericDataPtr = DTL_NUMERIC_GET_DIGIT_PTR( DTF_NUMERIC(sValue1) );
        sDateTimePart = DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sNumericDataPtr );
    }
    else
    {
        STL_THROW( ERROR_INVALID_DATEPART );
    }
    
    /*
     * startdate ( sValue2 )
     */ 
    
    sStartDateTime = DTF_TIMESTAMP( sValue2 );
    
    /*
     * enddate ( sValue3 )
     */ 
    
    sEndDateTime = DTF_TIMESTAMP( sValue3 );
    
    /*
     * datepart에 대한 diff 연산
     */

    switch( sDateTimePart )
    {
        case DTL_DATETIME_PART_YEAR:
            {
                DTF_DATETIME_TO_DTLEXPTIME( sStartDateTime,
                                            sStartDateDtlExpTime,
                                            sStartDateFractionalSecond,
                                            sEndDateTime,
                                            sEndDateDtlExpTime,
                                            sEndDateFractionalSecond,
                                            sErrorStack );                
                DTF_DATEDIFF_PART_YEAR( sStartDateDtlExpTime, sEndDateDtlExpTime, sDiffValue );
                
                break;
            }
        case DTL_DATETIME_PART_QUARTER:
            {
                DTF_DATETIME_TO_DTLEXPTIME( sStartDateTime,
                                            sStartDateDtlExpTime,
                                            sStartDateFractionalSecond,
                                            sEndDateTime,
                                            sEndDateDtlExpTime,
                                            sEndDateFractionalSecond,
                                            sErrorStack );
                DTF_DATEDIFF_PART_QUARTER( sStartDateDtlExpTime, sEndDateDtlExpTime, sDiffValue );
                
                break;
            }
        case DTL_DATETIME_PART_MONTH:
            {
                DTF_DATETIME_TO_DTLEXPTIME( sStartDateTime,
                                            sStartDateDtlExpTime,
                                            sStartDateFractionalSecond,
                                            sEndDateTime,
                                            sEndDateDtlExpTime,
                                            sEndDateFractionalSecond,
                                            sErrorStack );                
                DTF_DATEDIFF_PART_MONTH( sStartDateDtlExpTime, sEndDateDtlExpTime, sDiffValue );
                
                break;
            }
        case DTL_DATETIME_PART_DAY:
        case DTL_DATETIME_PART_DAYOFYEAR:                        
            {
                DTF_DATEDIFF_PART_DAY( sStartDateTime, sEndDateTime, sDiffValue );
                
                break;
            }
        case DTL_DATETIME_PART_HOUR:
            {
                DTF_DATEDIFF_PART_HOUR( sStartDateTime, sEndDateTime, sDiffValue );
                
                break;
            }
        case DTL_DATETIME_PART_MINUTE:
            {
                DTF_DATEDIFF_PART_MINUTE( sStartDateTime, sEndDateTime, sDiffValue );
                
                break;
            }
        case DTL_DATETIME_PART_SECOND:
            {
                DTF_DATEDIFF_PART_SECOND( sStartDateTime, sEndDateTime, sDiffValue );
                
                break;
            }
        case DTL_DATETIME_PART_MILLISECOND:
            {
                DTF_DATEDIFF_PART_MILLISECOND( sStartDateTime, sEndDateTime, sDiffValue );
                
                break;
            }
        case DTL_DATETIME_PART_MICROSECOND:
            {
                DTF_DATEDIFF_PART_MICROSECOND( sStartDateTime, sEndDateTime, sDiffValue );
                
                break;
            }
        default:
            STL_THROW( ERROR_INVALID_DATEPART );
            break;
    }
    
    /* datediff 연산 수행결과를 numeric type으로 변환. */
    STL_TRY( dtdToNumericFromInt64( sDiffValue,
                                    gResultDataTypeDef[DTL_TYPE_NUMBER].mArgNum1,
                                    gResultDataTypeDef[DTL_TYPE_NUMBER].mArgNum2,
                                    sResultValue,
                                    sErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_INVALID_DATEPART )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_DATEPART_FOR_DATA_TYPE,
                      NULL,
                      sErrorStack,
                      dtlDataTypeName[ DTL_TYPE_TIMESTAMP ] );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief DATEDIFF( datepart, startdate, enddate )
 *        <BR>   DATEDIFF( datepart, startdate, enddate ) => [ mssql ] 
 *        <BR> ex) DATEDIFF( DAY, DATE'2014-05-10', DATE'2014-05-12' ) =>  2
 *        <BR> ex) DATEDIFF( DAY, DATE'2014-05-12', DATE'2014-05-10' ) => -2
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> DATEDIFF( NUMBER,
 *                               <BR>           DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,
 *                               <BR>           DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE )
 * @param[out] aResultValue      연산 결과 (결과타입 NUMBER)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfDateDiffForTimestampWithTimeZone( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv )
{
    dtlDataValue   * sValue1;
    dtlDataValue   * sValue2;
    dtlDataValue   * sValue3;    
    dtlDataValue   * sResultValue;
    stlErrorStack  * sErrorStack;

    stlUInt8       * sNumericDataPtr;    
    stlInt32         sDigitCount = 0;
    stlInt32         sDateTimePart = 0;

    dtlTimestampType sStartDateTime = 0;
    dtlTimestampType sEndDateTime   = 0;

    dtlExpTime       sStartDateDtlExpTime;
    dtlExpTime       sEndDateDtlExpTime;    

    dtlFractionalSecond sStartDateFractionalSecond = 0;
    dtlFractionalSecond sEndDateFractionalSecond = 0;

    stlInt64         sDiffValue = 0;


    DTL_PARAM_VALIDATE( aInputArgumentCnt == 3, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sValue3 = aInputArgument[2].mValue.mDataValue;    
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;
    
    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue3, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, sErrorStack );    
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NUMBER, sErrorStack );
    
    /*
     * datetime part value ( sValue1 )
     */ 
    
    sDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( sValue1->mLength );
    
    if( sDigitCount == 1 )
    {
        sNumericDataPtr = DTL_NUMERIC_GET_DIGIT_PTR( DTF_NUMERIC(sValue1) );
        sDateTimePart = DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sNumericDataPtr );
    }
    else
    {
        STL_THROW( ERROR_INVALID_DATEPART );
    }
    
    /*
     * startdate ( sValue2 )
     */

    /* timestamptz->mTimestamp는 UTC로 계산된 값이다. */
    sStartDateTime = ((dtlTimestampTzType *)(sValue2->mValue))->mTimestamp;
    
    /*
     * enddate ( sValue3 )
     */

    /* timestamptz->mTimestamp는 UTC로 계산된 값이다. */    
    sEndDateTime = ((dtlTimestampTzType *)(sValue3->mValue))->mTimestamp;
    
    /*
     * datepart에 대한 diff 연산
     */

    switch( sDateTimePart )
    {
        case DTL_DATETIME_PART_YEAR:
            {
                DTF_DATETIME_TO_DTLEXPTIME( sStartDateTime,
                                            sStartDateDtlExpTime,
                                            sStartDateFractionalSecond,
                                            sEndDateTime,
                                            sEndDateDtlExpTime,
                                            sEndDateFractionalSecond,
                                            sErrorStack );                
                DTF_DATEDIFF_PART_YEAR( sStartDateDtlExpTime, sEndDateDtlExpTime, sDiffValue );
                
                break;
            }
        case DTL_DATETIME_PART_QUARTER:
            {
                DTF_DATETIME_TO_DTLEXPTIME( sStartDateTime,
                                            sStartDateDtlExpTime,
                                            sStartDateFractionalSecond,
                                            sEndDateTime,
                                            sEndDateDtlExpTime,
                                            sEndDateFractionalSecond,
                                            sErrorStack );                
                DTF_DATEDIFF_PART_QUARTER( sStartDateDtlExpTime, sEndDateDtlExpTime, sDiffValue );
                
                break;
            }
        case DTL_DATETIME_PART_MONTH:
            {
                DTF_DATETIME_TO_DTLEXPTIME( sStartDateTime,
                                            sStartDateDtlExpTime,
                                            sStartDateFractionalSecond,
                                            sEndDateTime,
                                            sEndDateDtlExpTime,
                                            sEndDateFractionalSecond,
                                            sErrorStack );                
                DTF_DATEDIFF_PART_MONTH( sStartDateDtlExpTime, sEndDateDtlExpTime, sDiffValue );
                
                break;
            }
        case DTL_DATETIME_PART_DAY:
        case DTL_DATETIME_PART_DAYOFYEAR:                        
            {
                DTF_DATEDIFF_PART_DAY( sStartDateTime, sEndDateTime, sDiffValue );
                
                break;
            }
        case DTL_DATETIME_PART_HOUR:
            {
                DTF_DATEDIFF_PART_HOUR( sStartDateTime, sEndDateTime, sDiffValue );
                
                break;
            }
        case DTL_DATETIME_PART_MINUTE:
            {
                DTF_DATEDIFF_PART_MINUTE( sStartDateTime, sEndDateTime, sDiffValue );
                
                break;
            }
        case DTL_DATETIME_PART_SECOND:
            {
                DTF_DATEDIFF_PART_SECOND( sStartDateTime, sEndDateTime, sDiffValue );
                
                break;
            }
        case DTL_DATETIME_PART_MILLISECOND:
            {
                DTF_DATEDIFF_PART_MILLISECOND( sStartDateTime, sEndDateTime, sDiffValue );
                
                break;
            }
        case DTL_DATETIME_PART_MICROSECOND:
            {
                DTF_DATEDIFF_PART_MICROSECOND( sStartDateTime, sEndDateTime, sDiffValue );
                
                break;
            }
        default:
            STL_THROW( ERROR_INVALID_DATEPART );
            break;
    }
    
    /* datediff 연산 수행결과를 numeric type으로 변환. */
    STL_TRY( dtdToNumericFromInt64( sDiffValue,
                                    gResultDataTypeDef[DTL_TYPE_NUMBER].mArgNum1,
                                    gResultDataTypeDef[DTL_TYPE_NUMBER].mArgNum2,
                                    sResultValue,
                                    sErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_INVALID_DATEPART )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_DATEPART_FOR_DATA_TYPE,
                      NULL,
                      sErrorStack,
                      dtlDataTypeName[ DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE ] );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief DATEDIFF( datepart, startdate, enddate )
 *        <BR>   DATEDIFF( datepart, startdate, enddate ) => [ mssql ] 
 *        <BR> ex) DATEDIFF( DAY, DATE'2014-05-10', DATE'2014-05-12' ) =>  2
 *        <BR> ex) DATEDIFF( DAY, DATE'2014-05-12', DATE'2014-05-10' ) => -2
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> DATEDIFF( NUMBER, TIME, TIME )
 * @param[out] aResultValue      연산 결과 (결과타입 NUMBER)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfDateDiffForTime( stlUInt16        aInputArgumentCnt,
                              dtlValueEntry  * aInputArgument,
                              void           * aResultValue,
                              void           * aInfo,
                              dtlFuncVector  * aVectorFunc,
                              void           * aVectorArg,
                              void           * aEnv )
{
    dtlDataValue   * sValue1;
    dtlDataValue   * sValue2;
    dtlDataValue   * sValue3;    
    dtlDataValue   * sResultValue;
    stlErrorStack  * sErrorStack;

    stlUInt8       * sNumericDataPtr;    
    stlInt32         sDigitCount = 0;
    stlInt32         sDateTimePart = 0;

    dtlTimeType      sStartDateTime = 0;
    dtlTimeType      sEndDateTime   = 0;

    stlInt64         sDiffValue = 0;


    DTL_PARAM_VALIDATE( aInputArgumentCnt == 3, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sValue3 = aInputArgument[2].mValue.mDataValue;    
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;
    
    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_TIME, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue3, DTL_TYPE_TIME, sErrorStack );    
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NUMBER, sErrorStack );
    
    /*
     * datetime part value ( sValue1 )
     */ 
    
    sDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( sValue1->mLength );
    
    if( sDigitCount == 1 )
    {
        sNumericDataPtr = DTL_NUMERIC_GET_DIGIT_PTR( DTF_NUMERIC(sValue1) );
        sDateTimePart = DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sNumericDataPtr );
    }
    else
    {
        STL_THROW( ERROR_INVALID_DATEPART );
    }
    
    /*
     * startdate ( sValue2 )
     */ 
    
    sStartDateTime = DTF_TIME( sValue2 );

    /*
     * enddate ( sValue3 )
     */ 
    
    sEndDateTime = DTF_TIME( sValue3 );
    
    /*
     * datepart에 대한 diff 연산
     */

    switch( sDateTimePart )
    {
        case DTL_DATETIME_PART_YEAR:
        case DTL_DATETIME_PART_QUARTER:
        case DTL_DATETIME_PART_MONTH:
        case DTL_DATETIME_PART_DAY:
        case DTL_DATETIME_PART_DAYOFYEAR:                        
            {
                sDiffValue = 0;                
                
                break;
            }
        case DTL_DATETIME_PART_HOUR:
            {
                sDiffValue = ( sEndDateTime / DTL_USECS_PER_HOUR ) - ( sStartDateTime / DTL_USECS_PER_HOUR );
                
                break;
            }
        case DTL_DATETIME_PART_MINUTE:
            {
                sDiffValue = ( sEndDateTime / DTL_USECS_PER_MINUTE ) - ( sStartDateTime / DTL_USECS_PER_MINUTE );
                
                break;
            }
        case DTL_DATETIME_PART_SECOND:
            {
                sDiffValue = ( sEndDateTime / DTL_USECS_PER_SEC ) - ( sStartDateTime / DTL_USECS_PER_SEC );
                
                break;
            }
        case DTL_DATETIME_PART_MILLISECOND:
            {
                sDiffValue = ( sEndDateTime / DTL_USECS_PER_MILLISEC ) - ( sStartDateTime / DTL_USECS_PER_MILLISEC );
                
                break;
            }
        case DTL_DATETIME_PART_MICROSECOND:
            {
                sDiffValue = sEndDateTime - sStartDateTime;
                
                break;
            }
        default:
            STL_THROW( ERROR_INVALID_DATEPART );
            break;
    }
    
    /* datediff 연산 수행결과를 numeric type으로 변환. */
    STL_TRY( dtdToNumericFromInt64( sDiffValue,
                                    gResultDataTypeDef[DTL_TYPE_NUMBER].mArgNum1,
                                    gResultDataTypeDef[DTL_TYPE_NUMBER].mArgNum2,
                                    sResultValue,
                                    sErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_INVALID_DATEPART )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_DATEPART_FOR_DATA_TYPE,
                      NULL,
                      sErrorStack,
                      dtlDataTypeName[ DTL_TYPE_TIME ] );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/** @} */
