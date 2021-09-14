/*******************************************************************************
 * dtfExtract.c
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
 * @file dtfExtract.c
 * @brief DateTime and Interval Function DataType Library Layer
 */

#include <dtl.h>
#include <dtlDef.h>
#include <dtDef.h>
#include <dtfString.h>

/**
 * @addtogroup dtfDateTime Date and Time 
 * @ingroup dtf
 * @internal
 * @{
 */

/*******************************************************************************
 * date_part(text, timestamp)
 *   date_part(text, timestamp) => [ P ]
 * ex) date_part('year', timestamp '2001-02-16 20:38:40')  =>  2001
 *******************************************************************************/
/*******************************************************************************
 * extract(field from timestamp)
 *   extract(field from timestamp) => [ P, S, M, O ]
 * ex) extract(year from timestamp '2001-02-16 20:38:40')  =>  2001
 *******************************************************************************/

/**
 * @brief  date_part(text, timestamp without time zone )
 *         <BR> extract(field from timestamp without time zone)
 *         <BR> 지정된 field 반환
 *
 *         <BR> date_part(text, timestamp without time zone)
 *         <BR>   date_part(text, timestamp without time zone) => [ P ]
 *         <BR> ex) date_part('year', timestamp '2001-02-16 20:38:40')  =>  2001
 *         
 *         <BR> extract(field from timestamp without time zone)
 *         <BR>   extract(field from timestamp without time zone) => [ P, S, M, O ]
 *         <BR> ex) extract(year from timestamp without time zone '2001-02-16 20:38:40') => 2001
 *         
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> extract( NUMERIC, TIMESTAMP without time zone)
 *                               <BR> date_part(VARCHAR, TIMESTAMP without time zone)
 * @param[out] aResultValue      연산 결과 (결과타입 NUMBER)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfExtractFromTimestamp( stlUInt16        aInputArgumentCnt,
                                   dtlValueEntry  * aInputArgument,
                                   void           * aResultValue,
                                   void           * aEnv )
{
    /*
     * Timestamp Type은 
     * YEAR, MONTH, DAY, HOUR, MINUTE, SECOND
     * 지원 될 수 있도록 처리해야함.
     * TIMEZONE_HOUR, TIMEZONE_MINUTE은 error를 내보냄 
     */
    
    dtlDataValue       * sValue1;
    dtlDataValue       * sValue2;
    dtlDataValue       * sResultValue;
    stlErrorStack      * sErrorStack;
    
    dtlExpTime           sDtlExpTime;
    dtlFractionalSecond  sFractionalSecond;
        
    stlInt64             sExtractField = 0;
    stlUInt8           * sNumericDataPtr;
    stlInt32             sDigitCount;

    stlInt64             sInt64 = 0;
    stlInt64             sPrecision = DTL_NUMERIC_DEFAULT_PRECISION;
    stlInt64             sScale = DTL_SCALE_NA;    
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack  = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE2( sValue1, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_TIMESTAMP, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );

    /* dtlTimestampType을 dtlExpTime으로 변경 */
    DTL_INIT_DTLEXPTIME( sDtlExpTime );
    STL_TRY( dtdDateTime2dtlExpTime( DTF_TIMESTAMP(sValue2),
                                     & sDtlExpTime,
                                     & sFractionalSecond,
                                     sErrorStack )
             == STL_SUCCESS );
    
   sDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( sValue1->mLength );

    if( sDigitCount == 1 )
    {
        sNumericDataPtr = DTL_NUMERIC_GET_DIGIT_PTR( DTF_NUMERIC(sValue1) );
        sExtractField = DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sNumericDataPtr );
    }
    else
    {
        sExtractField = DTL_DATETIME_FIELD_NONE;
    }
    
    switch( sExtractField )
    {
        case DTL_DATETIME_FIELD_YEAR:
            sInt64 = DTD_GET_DATETIME_SYEAR( sDtlExpTime.mYear, STL_FALSE );
            break;

        case DTL_DATETIME_FIELD_MONTH:
            sInt64 = sDtlExpTime.mMonth;
            break;

        case DTL_DATETIME_FIELD_DAY:
            sInt64 = sDtlExpTime.mDay;
            break;

        case DTL_DATETIME_FIELD_HOUR:
            sInt64 = sDtlExpTime.mHour;
            break;

        case DTL_DATETIME_FIELD_MINUTE:
            sInt64 = sDtlExpTime.mMinute;
            break;

        case DTL_DATETIME_FIELD_SECOND:
            sInt64 = sDtlExpTime.mSecond * DTL_USECS_PER_SEC;
            sInt64 += sFractionalSecond;
            break;

        default:
            STL_THROW( ERROR_INVALID_EXTRACT_FIELD );
            
            break;
    }

    STL_TRY( dtdToNumericFromInt64( sInt64,
                                    sPrecision,
                                    sScale,
                                    sResultValue,
                                    sErrorStack )
             == STL_SUCCESS );

    if( (sInt64 != 0) && (sExtractField == DTL_DATETIME_FIELD_SECOND ) )
    {
        /* fractional second의 소수점 이하 표현을 위한 exponent 조정 */
        DTF_NUMERIC(sResultValue)->mData[0] -= DTL_DATETIME_FIELD_FRACTIONAL_SECOND_EXPONENT;
    }
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_INVALID_EXTRACT_FIELD )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_INVALID_EXTRACT_FIELD_FOR_EXTRACT_SOURCE,
                      NULL,
                      sErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;  
}

/**
 * @brief  date_part(text, timestamp with time zone )
 *         <BR> extract(field from timestamp with time zone)
 *         <BR> 지정된 field 반환
 *
 *         <BR> date_part(text, timestamp with time zone)
 *         <BR>   date_part(text, timestamp with time zone) => [ P ]
 *         <BR> ex) date_part('year', timestamp '2001-02-16 20:38:40 -8:00')  =>  2001
 *         
 *         <BR> extract(field from timestamp with time zone)
 *         <BR>   extract(field from timestamp with time zone) => [ P, S, M, O ]
 *         <BR> ex) extract(year from timestamp with time zone '2001-02-16 20:38:40 -8:00') => 2001
 *         
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> extract( NUMERIC, TIMESTAMP with time zone)
 *                               <BR> date_part(VARCHAR, TIMESTAMP with time zone)
 * @param[out] aResultValue      연산 결과 (결과타입 NUMBER)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfExtractFromTimestampTz( stlUInt16        aInputArgumentCnt,
                                     dtlValueEntry  * aInputArgument,
                                     void           * aResultValue,
                                     void           * aEnv )
{
    /*
     * Timestamp Type은 
     * YEAR, MONTH, DAY, HOUR, MINUTE, SECOND, TIMEZONE_HOUR, TIMEZONE_MINUTE
     * 지원 될 수 있도록 처리해야함.
     */
    
    dtlDataValue       * sValue1;
    dtlDataValue       * sValue2;
    dtlDataValue       * sResultValue;
    stlErrorStack      * sErrorStack;
    
    dtlExpTime           sDtlExpTime;
    dtlFractionalSecond  sFractionalSecond;
    dtlTimestampTzType   sTimestampTzType;
    stlInt32             sTimeZone = 0;
    
    stlInt64             sExtractField = 0;
    stlUInt8           * sNumericDataPtr;
    stlInt32             sDigitCount;

    stlInt64             sInt64 = 0;
    stlInt64             sPrecision = DTL_NUMERIC_DEFAULT_PRECISION;
    stlInt64             sScale = DTL_SCALE_NA;    
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack  = (stlErrorStack *)aEnv;
    
    DTF_CHECK_DTL_VALUE2( sValue1, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );

    sTimestampTzType.mTimestamp = ((dtlTimestampTzType *)(sValue2->mValue))->mTimestamp;
    sTimestampTzType.mTimeZone  = ((dtlTimestampTzType *)(sValue2->mValue))->mTimeZone;

    /* dtlTimestampType을 dtlExpTime으로 변경 */
    DTL_INIT_DTLEXPTIME( sDtlExpTime );
    STL_TRY( dtdTimestampTz2dtlExpTime( sTimestampTzType,
                                        & sTimeZone,
                                        & sDtlExpTime,
                                        & sFractionalSecond,
                                        sErrorStack )
             == STL_SUCCESS );

    sDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( sValue1->mLength );

    if( sDigitCount == 1 )
    {
        sNumericDataPtr = DTL_NUMERIC_GET_DIGIT_PTR( DTF_NUMERIC(sValue1) );
        sExtractField = DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sNumericDataPtr );
    }
    else
    {
        sExtractField = DTL_DATETIME_FIELD_NONE;
    }
    
    sTimeZone *= -1;
    
    switch( sExtractField )
    {
        case DTL_DATETIME_FIELD_YEAR:
            sInt64 = DTD_GET_DATETIME_SYEAR( sDtlExpTime.mYear, STL_FALSE );
            break;

        case DTL_DATETIME_FIELD_MONTH:
            sInt64 = sDtlExpTime.mMonth;
            break;

        case DTL_DATETIME_FIELD_DAY:
            sInt64 = sDtlExpTime.mDay;
            break;

        case DTL_DATETIME_FIELD_HOUR:
            sInt64 = sDtlExpTime.mHour;
            break;

        case DTL_DATETIME_FIELD_MINUTE:
            sInt64 = sDtlExpTime.mMinute;
            break;

        case DTL_DATETIME_FIELD_SECOND:
            sInt64 = sDtlExpTime.mSecond * DTL_USECS_PER_SEC;
            sInt64 += sFractionalSecond;
            break;

        case DTL_DATETIME_FIELD_TIMEZONE_HOUR:
            sInt64 = sTimeZone / DTL_SECS_PER_HOUR;
            break;

        case DTL_DATETIME_FIELD_TIMEZONE_MINUTE:
            sInt64 = sTimeZone / DTL_SECS_PER_HOUR;
            sTimeZone  -= sInt64 * DTL_SECS_PER_HOUR; 
            sInt64 = sTimeZone / DTL_SECS_PER_MINUTE; 
            break;

        default:
            STL_THROW( ERROR_INVALID_EXTRACT_FIELD );
            
            break;
    }            

    STL_TRY( dtdToNumericFromInt64( sInt64,
                                    sPrecision,
                                    sScale,
                                    sResultValue,
                                    sErrorStack )
             == STL_SUCCESS );

    
    if( (sInt64 != 0) && (sExtractField == DTL_DATETIME_FIELD_SECOND) )
    {
        /* fractional second의 소수점 이하 표현을 위한 exponent 조정 */
        DTF_NUMERIC(sResultValue)->mData[0] -= DTL_DATETIME_FIELD_FRACTIONAL_SECOND_EXPONENT;
    }

    
    return STL_SUCCESS;

    STL_CATCH( ERROR_INVALID_EXTRACT_FIELD )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_INVALID_EXTRACT_FIELD_FOR_EXTRACT_SOURCE,
                      NULL,
                      sErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;  
}


/**
 * @brief  date_part(text, date )
 *         <BR> extract(field date )
 *         <BR> 지정된 field 반환
 *
 *         <BR> date_part(text, date )
 *         <BR>   date_part(text, date ) => [ P ]
 *         <BR> ex) date_part('year', date '2001-02-16')  =>  2001
 *         
 *         <BR> extract(field from date)
 *         <BR>   extract(field from date) => [ P, S, M, O ]
 *         <BR> ex) extract(year from date '2001-02-16') => 2001
 *         
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> extract( NUMERIC, DATE)
 *                               <BR> date_part(VARCHAR, DATE)
 * @param[out] aResultValue      연산 결과 (결과타입 NUMBER)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfExtractFromDate( stlUInt16        aInputArgumentCnt,
                              dtlValueEntry  * aInputArgument,
                              void           * aResultValue,
                              void           * aEnv )
{
    /*
     * Date Type은 
     * YEAR, MONTH, DAY, HOUR, MINUTE, SECOND
     * 지원 될 수 있도록 처리해야함.
     * HOUR, MINUTE, SECOND는 0으로 처리.
     * TIMEZONE_HOUR, TIMEZONE_MINUTE 는 에러를 내보냄.
     */
    
    dtlDataValue       * sValue1;
    dtlDataValue       * sValue2;
    dtlDataValue       * sResultValue;
    stlErrorStack      * sErrorStack;

    dtlDateType          sDateType;
    dtlFractionalSecond  sFractionalSecond = 0;
    dtlExpTime           sDtlExpTime;
    
    stlInt64             sExtractField = 0;
    stlUInt8           * sNumericDataPtr;
    stlInt32             sDigitCount;

    stlInt64             sInt64 = 0;
    stlInt64             sPrecision = DTL_NUMERIC_DEFAULT_PRECISION;
    stlInt64             sScale = DTL_SCALE_NA;    
  
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack  = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE2( sValue1, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_DATE, sErrorStack );
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );

    /* dtlTimestampType을 dtlExpTime으로 변경 */
    DTL_INIT_DTLEXPTIME( sDtlExpTime );

    sDateType = *(dtlDateType *)(sValue2->mValue);

    STL_TRY( dtdDateTime2dtlExpTime( sDateType,
                                     & sDtlExpTime,
                                     & sFractionalSecond,
                                     sErrorStack )
             == STL_SUCCESS );
    
    sDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( sValue1->mLength );

    if( sDigitCount == 1 )
    {
        sNumericDataPtr = DTL_NUMERIC_GET_DIGIT_PTR( DTF_NUMERIC(sValue1) );
        sExtractField = DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sNumericDataPtr );
    }
    else
    {
        sExtractField = DTL_DATETIME_FIELD_NONE;
    }

    switch( sExtractField )
    {
        case DTL_DATETIME_FIELD_YEAR:
            sInt64 = DTD_GET_DATETIME_SYEAR( sDtlExpTime.mYear, STL_FALSE );
            break;
            
        case DTL_DATETIME_FIELD_MONTH:
            sInt64 = sDtlExpTime.mMonth;
            break;
            
        case DTL_DATETIME_FIELD_DAY:
            sInt64 = sDtlExpTime.mDay;
            break;
            
        default:
            STL_THROW( ERROR_INVALID_EXTRACT_FIELD );
            break;
    }            
    
    STL_TRY( dtdToNumericFromInt64( sInt64,
                                    sPrecision,
                                    sScale,
                                    sResultValue,
                                    sErrorStack )
             == STL_SUCCESS );         

    return STL_SUCCESS;

    STL_FINISH;

    STL_CATCH( ERROR_INVALID_EXTRACT_FIELD )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_INVALID_EXTRACT_FIELD_FOR_EXTRACT_SOURCE,
                      NULL,
                      sErrorStack );
    }

    return STL_FAILURE;  
}


/**
 * @brief  date_part(text, time )
 *         <BR> extract(field time )
 *         <BR> 지정된 field 반환
 *
 *         <BR> date_part(text, time )
 *         <BR>   date_part(text, time ) => [ P ]
 *         <BR> ex) date_part('second', time '11:22:33')  =>  33
 *         
 *         <BR> extract(field from time)
 *         <BR>   extract(field from time) => [ P, S, M ]
 *         <BR> ex) extract(second from time '11:22:33') => 33
 *         
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> extract( NUMERIC, TIME)
 *                               <BR> date_part(VARCHAR, TIME)
 * @param[out] aResultValue      연산 결과 (결과타입 NUMBER)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfExtractFromTime( stlUInt16        aInputArgumentCnt,
                              dtlValueEntry  * aInputArgument,
                              void           * aResultValue,
                              void           * aEnv )
{
    /*
     * Time Type은 
     * HOUR, MINUTE, SECOND
     * 지원 될 수 있도록 처리해야함.
     * YEAR, MONTH, DAY, TIMEZONE_HOUR, TIMEZONE_MINUTE 는 에러를 내보냄. (PostgreSQL)
     */
    
    dtlDataValue       * sValue1;
    dtlDataValue       * sValue2;
    dtlDataValue       * sResultValue;
    stlErrorStack      * sErrorStack;

    dtlExpTime           sDtlExpTime;
    dtlFractionalSecond  sFractionalSecond;
    
    stlInt64             sExtractField = 0;
    stlUInt8           * sNumericDataPtr;
    stlInt32             sDigitCount;

    stlInt64             sInt64 = 0;
    stlInt64             sPrecision = DTL_NUMERIC_DEFAULT_PRECISION;
    stlInt64             sScale = DTL_SCALE_NA;    
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack  = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE2( sValue1, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_TIME, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );

    /* dtlTimestampType을 dtlExpTime으로 변경 */
    DTL_INIT_DTLEXPTIME( sDtlExpTime );
    dtdTime2dtlExpTime( DTF_TIME( sValue2 ),
                        & sDtlExpTime,
                        & sFractionalSecond );

    sDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( sValue1->mLength );

    if( sDigitCount == 1 )
    {
        sNumericDataPtr = DTL_NUMERIC_GET_DIGIT_PTR( DTF_NUMERIC(sValue1) );
        sExtractField = DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sNumericDataPtr );
    }
    else
    {
        sExtractField = DTL_DATETIME_FIELD_NONE;
    }

    switch( sExtractField )
    {
        case DTL_DATETIME_FIELD_HOUR:
            sInt64 = sDtlExpTime.mHour;
            break;

        case DTL_DATETIME_FIELD_MINUTE:
            sInt64 = sDtlExpTime.mMinute;
            break;

        case DTL_DATETIME_FIELD_SECOND:
            sInt64 = sDtlExpTime.mSecond * DTL_USECS_PER_SEC;
            sInt64 += sFractionalSecond;
            break;

        default:
            STL_THROW( ERROR_INVALID_EXTRACT_FIELD );
            
            break;
    }            

    STL_TRY( dtdToNumericFromInt64( sInt64,
                                    sPrecision,
                                    sScale,
                                    sResultValue,
                                    sErrorStack )
             == STL_SUCCESS );

    
    if( (sInt64 != 0) && (sExtractField == DTL_DATETIME_FIELD_SECOND) )
    {
        /* fractional second의 소수점 이하 표현을 위한 exponent 조정 */
        DTF_NUMERIC(sResultValue)->mData[0] -= DTL_DATETIME_FIELD_FRACTIONAL_SECOND_EXPONENT;
    }

    return STL_SUCCESS;

    STL_CATCH( ERROR_INVALID_EXTRACT_FIELD )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_INVALID_EXTRACT_FIELD_FOR_EXTRACT_SOURCE,
                      NULL,
                      sErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;  
}


/**
 * @brief  date_part(text, time with time zone )
 *         <BR> extract(field time with time zone)
 *         <BR> 지정된 field 반환
 *
 *         <BR> date_part(text, time with time zone)
 *         <BR>   date_part(text, time with time zone) => [ P ]
 *         <BR> ex) date_part('second', time with time zone '11:22:33')  =>  33
 *         
 *         <BR> extract(field from time with time zone)
 *         <BR>   extract(field from time with time zone) => [ P, S, M ]
 *         <BR> ex) extract(second from time with time zone '11:22:33') => 33
 *         
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> extract( NUMERIC, TIME WITH TIME ZONE)
 *                               <BR> date_part(VARCHAR, TIME WITH TIME ZONE)
 * @param[out] aResultValue      연산 결과 (결과타입 NUMBER)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfExtractFromTimeTz( stlUInt16        aInputArgumentCnt,
                                dtlValueEntry  * aInputArgument,
                                void           * aResultValue,
                                void           * aEnv )
{
    /*
     * Time Type은 
     * HOUR, MINUTE, SECOND, TIMEZONE_HOUR, TIMEZONE_MINUTE
     * 지원 될 수 있도록 처리해야함.
     * YEAR, MONTH, DAY 는 에러를 내보냄. (PostgreSQL)
     */
    
    dtlDataValue       * sValue1;
    dtlDataValue       * sValue2;
    dtlDataValue       * sResultValue;
    stlErrorStack      * sErrorStack;

    dtlExpTime           sDtlExpTime;
    dtlFractionalSecond  sFractionalSecond;
    dtlTimeTzType        sTimeTzType;
    stlInt32             sTimeZone = 0;
    
    stlInt64             sExtractField = 0;
    stlUInt8           * sNumericDataPtr;
    stlInt32             sDigitCount;
    
    stlInt64             sInt64 = 0;
    stlInt64             sPrecision = DTL_NUMERIC_DEFAULT_PRECISION;
    stlInt64             sScale = DTL_SCALE_NA;    
        
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack  = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE2( sValue1, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_TIME_WITH_TIME_ZONE, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );

    sTimeTzType.mTime     = ((dtlTimeTzType *)(sValue2->mValue))->mTime;
    sTimeTzType.mTimeZone = ((dtlTimeTzType *)(sValue2->mValue))->mTimeZone;
    
    /* dtlTimestampType을 dtlExpTime으로 변경 */
    DTL_INIT_DTLEXPTIME( sDtlExpTime );
    dtdTimeTz2dtlExpTime( & sTimeTzType,
                          & sDtlExpTime,
                          & sFractionalSecond,
                          & sTimeZone );

    sDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( sValue1->mLength );

    if( sDigitCount == 1 )
    {
        sNumericDataPtr = DTL_NUMERIC_GET_DIGIT_PTR( DTF_NUMERIC(sValue1) );
        sExtractField = DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sNumericDataPtr );
    }
    else
    {
        sExtractField = DTL_DATETIME_FIELD_NONE;
    }

    sTimeZone *= -1;
    switch( sExtractField )
    {
        case DTL_DATETIME_FIELD_HOUR:
            sInt64 = sDtlExpTime.mHour;
            break;

        case DTL_DATETIME_FIELD_MINUTE:
            sInt64 = sDtlExpTime.mMinute;
            break;

        case DTL_DATETIME_FIELD_SECOND:
            sInt64 = sDtlExpTime.mSecond * DTL_USECS_PER_SEC;
            sInt64 += sFractionalSecond;
            break;
            
        case DTL_DATETIME_FIELD_TIMEZONE_HOUR:
            sInt64 = sTimeZone / DTL_SECS_PER_HOUR;
            break;
            
        case DTL_DATETIME_FIELD_TIMEZONE_MINUTE:
            sInt64 = sTimeZone / DTL_SECS_PER_HOUR;
            sTimeZone  -= sInt64 * DTL_SECS_PER_HOUR; 
            sInt64 = sTimeZone / DTL_SECS_PER_MINUTE; 
            break;

        default:
            STL_THROW( ERROR_INVALID_EXTRACT_FIELD );
            
            break;
    }
    
    STL_TRY( dtdToNumericFromInt64( sInt64,
                                    sPrecision,
                                    sScale,
                                    sResultValue,
                                    sErrorStack )
             == STL_SUCCESS );

    if( (sInt64 != 0) && (sExtractField == DTL_DATETIME_FIELD_SECOND) )
    {
        /* fractional second의 소수점 이하 표현을 위한 exponent 조정 */
        DTF_NUMERIC(sResultValue)->mData[0] -= DTL_DATETIME_FIELD_FRACTIONAL_SECOND_EXPONENT;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_CATCH( ERROR_INVALID_EXTRACT_FIELD )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_INVALID_EXTRACT_FIELD_FOR_EXTRACT_SOURCE,
                      NULL,
                      sErrorStack );
    }

    return STL_FAILURE;  
}


/*******************************************************************************
 * date_part(text, interval)
 *   date_part(text, interval) => [ P ]
 * ex) date_part('year', interval '2 years 3 months')  =>  3
 *******************************************************************************/
/*******************************************************************************
 * extract(field from interval)
 *   extract(field from interval) => [ P, S, 0 ]
 * ex) extract(year from interval '2 years 3 months')  =>  3
 *******************************************************************************/
/**
 * @brief  date_part(text, interval), extract(field from interval)
 *         <BR> 지정된 field 반환
 *
 *         <BR> date_part(text, interval)
 *         <BR>   date_part(text, interval) => [ P ]
 *         <BR> ex) date_part('year', interval '2 years 3 months')  =>  3
 *
 *         <BR> extract(field from interval)
 *         <BR>   extract(field from interval) => [ P, S, 0 ]
 *         <BR> ex) extract(year from interval '2 years 3 months')  =>  2
 *         <BR> ex) extract(day from interval '2 years 3 months')  =>  0
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> extract( NUMERIC, INTERVAL YEAR TO MONTH )
 *                               <BR> date_part(VARCHAR, INTERVAL YEAR TO MOMTH)
 * @param[out] aResultValue      연산 결과 (결과타입 NUMBER)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfExtractIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aEnv )
{
    
    dtlDataValue       * sValue1;
    dtlDataValue       * sValue2;
    dtlDataValue       * sResultValue;
    stlErrorStack      * sErrorStack;

    dtlIntervalYearToMonthType  sIntervalYearToMonth;
    
    stlInt64             sExtractField = 0;
    stlUInt8           * sNumericDataPtr;
    stlInt32             sDigitCount;

    stlInt64             sInt64 = 0;
    stlInt64             sPrecision = DTL_NUMERIC_DEFAULT_PRECISION;
    stlInt64             sScale = DTL_SCALE_NA;    
        
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack  = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE2( sValue1, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_INTERVAL_YEAR_TO_MONTH, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );

    sIntervalYearToMonth.mIndicator = ((dtlIntervalYearToMonthType *)(sValue2->mValue))->mIndicator;
    sIntervalYearToMonth.mMonth     = ((dtlIntervalYearToMonthType *)(sValue2->mValue))->mMonth;

    sDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( sValue1->mLength );

    if( sDigitCount == 1 )
    {
        sNumericDataPtr = DTL_NUMERIC_GET_DIGIT_PTR( DTF_NUMERIC(sValue1) );
        sExtractField = DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sNumericDataPtr );
    }
    else
    {
        sExtractField = DTL_DATETIME_FIELD_NONE;
    }
    
    switch( sExtractField )
    {
        case DTL_DATETIME_FIELD_YEAR:
            sInt64 = sIntervalYearToMonth.mMonth / DTL_MONTHS_PER_YEAR;
            break;

        case DTL_DATETIME_FIELD_MONTH:
            sInt64 = sIntervalYearToMonth.mMonth / DTL_MONTHS_PER_YEAR;
            sInt64 = sIntervalYearToMonth.mMonth - ( sInt64 * DTL_MONTHS_PER_YEAR );
            break;

        default:
            STL_THROW( ERROR_INVALID_EXTRACT_FIELD );
            break;
    }            
 
    STL_TRY( dtdToNumericFromInt64( sInt64,
                                    sPrecision,
                                    sScale,
                                    sResultValue,
                                    sErrorStack )
             == STL_SUCCESS );
   
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_INVALID_EXTRACT_FIELD )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_INVALID_EXTRACT_FIELD_FOR_EXTRACT_SOURCE,
                      NULL,
                      sErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief  date_part(text, interval), extract(field from interval)
 *         <BR> 지정된 field 반환
 *
 *         <BR> date_part(text, interval)
 *         <BR>   date_part(text, interval) => [ P ]
 *         <BR> ex) date_part('year', interval '2 day 3 second')  =>  3
 *
 *         <BR> extract(field from interval)
 *         <BR>   extract(field from interval) => [ P, S, 0 ]
 *         <BR> ex) extract(year from interval '2 day 3 second')  =>  2
 *         <BR> ex) extract(day from interval '2 day 3 second')  =>  0
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> extract( NUMERIC, INTERVAL DAY TO SECOND )
 *                               <BR> date_part(VARCHAR, INTERVAL DAY TO SECOND )
 * @param[out] aResultValue      연산 결과 (결과타입 NUMBER)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfExtractIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aEnv )
{
    
    dtlDataValue       * sValue1;
    dtlDataValue       * sValue2;
    dtlDataValue       * sResultValue;
    stlErrorStack      * sErrorStack;

    dtlIntervalDayToSecondType  sIntervalDayToSecond;
    
    stlInt64             sExtractField = 0;
    stlUInt8           * sNumericDataPtr;
    stlInt32             sDigitCount;
    stlInt64             sTime = 0;
    
    stlInt64             sInt64 = 0;
    stlInt64             sPrecision = DTL_NUMERIC_DEFAULT_PRECISION;
    stlInt64             sScale = DTL_SCALE_NA;    
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack  = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE2( sValue1, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_INTERVAL_DAY_TO_SECOND, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );

    sIntervalDayToSecond.mIndicator = ((dtlIntervalDayToSecondType *)(sValue2->mValue))->mIndicator;
    sIntervalDayToSecond.mDay       = ((dtlIntervalDayToSecondType *)(sValue2->mValue))->mDay;
    sIntervalDayToSecond.mTime      = ((dtlIntervalDayToSecondType *)(sValue2->mValue))->mTime;

   sDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( sValue1->mLength );

    if( sDigitCount == 1 )
    {
        sNumericDataPtr = DTL_NUMERIC_GET_DIGIT_PTR( DTF_NUMERIC(sValue1) );
        sExtractField = DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sNumericDataPtr );
    }
    else
    {
        sExtractField = DTL_DATETIME_FIELD_NONE;
    }
    
    switch( sExtractField )
    {
        case DTL_DATETIME_FIELD_DAY:
            sInt64 = sIntervalDayToSecond.mDay;
            break;

        case DTL_DATETIME_FIELD_HOUR:
            sInt64 = sIntervalDayToSecond.mTime / DTL_USECS_PER_HOUR;
            break;

        case DTL_DATETIME_FIELD_MINUTE:
            sInt64  = sIntervalDayToSecond.mTime / DTL_USECS_PER_HOUR;
            sTime   = sIntervalDayToSecond.mTime - ( sInt64 * DTL_USECS_PER_HOUR );
            sInt64  = sTime / DTL_USECS_PER_MINUTE;            
            break;

        case DTL_DATETIME_FIELD_SECOND:
            sInt64  = sIntervalDayToSecond.mTime / DTL_USECS_PER_HOUR;
            sTime   = sIntervalDayToSecond.mTime - ( sInt64 * DTL_USECS_PER_HOUR );
            sInt64  = sTime / DTL_USECS_PER_MINUTE;
            sInt64  = sTime - ( sInt64 * DTL_USECS_PER_MINUTE );
            break;

        default:
            STL_THROW( ERROR_INVALID_EXTRACT_FIELD );
            
            break;
    }            
    
    STL_TRY( dtdToNumericFromInt64( sInt64,
                                    sPrecision,
                                    sScale,
                                    sResultValue,
                                    sErrorStack )
             == STL_SUCCESS );

    if( (sInt64 != 0) && (sExtractField == DTL_DATETIME_FIELD_SECOND) )
    {
        /* fractional second의 소수점 이하 표현을 위한 exponent 조정 */
        DTF_NUMERIC(sResultValue)->mData[0] -= DTL_DATETIME_FIELD_FRACTIONAL_SECOND_EXPONENT;
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_INVALID_EXTRACT_FIELD )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_INVALID_EXTRACT_FIELD_FOR_EXTRACT_SOURCE,
                      NULL,
                      sErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
