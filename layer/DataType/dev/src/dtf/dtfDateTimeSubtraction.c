/*******************************************************************************
 * dtfDateTimeSubtraction.c
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
 * @file dtfDateTimeSubtraction.c
 * @brief Date and Time Function DataType Library Layer
 */

#include <dtl.h>
#include <dtlDef.h>
#include <dtDef.h>
#include <dtfNumeric.h>
#include <dtfArithmetic.h>

/**
 * @addtogroup dtfDateTime Date and Time
 * @ingroup dtf
 * @internal
 * @{
 */

/*******************************************************************************
 * subtraction ( - ) : date - date, DATEDIFF( date, date )
 *   date - date            => [ P, S ]
 *   DATEDIFF( date, date ) => [ M ]
 * ex) date'2001-10-01' - date'2001-09-28'  =>  integer'3' (days)
 *******************************************************************************/

/**
 * @brief date - date
 *        <BR> subtraction ( - ) : date - date, DATEDIFF( date, date )
 *        <BR>   date - date            => [ P, S ]
 *        <BR>   DATEDIFF( date, date ) => [ M ]
 *        <BR> ex) date'2001-10-01' - date'2001-09-28'  =>  integer'3' (days)
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> DATE - DATE
 * @param[out] aResultValue      연산 결과 (결과타입 INTEGER)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfDateSubDate( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sValue2;
    dtlDataValue  * sResultValue;

    stlInt64        sInterval;

    dtlDataValue    sNumericUsecsPerDay = { DTL_TYPE_NUMBER, 3, 3, (dtlNumericType*)"\xC6\x9\x41" };
    
    dtlDataValue    sNumericDateSubValue;
    stlUInt8        sNumericBuffer[ STL_ALIGN_DEFAULT(DTL_NUMERIC_MAX_SIZE) ];

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_DATE, (stlErrorStack *)aEnv );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_DATE, (stlErrorStack *)aEnv );
    
    sNumericDateSubValue.mType = DTL_TYPE_NUMBER;
    sNumericDateSubValue.mBufferSize = STL_ALIGN_DEFAULT(DTL_NUMERIC_MAX_SIZE);
    sNumericDateSubValue.mLength = 0;
    sNumericDateSubValue.mValue = (dtlNumericType *)(&sNumericBuffer);
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, (stlErrorStack *)aEnv );

    /*
     * - 연산 수행
     */
    
    sInterval = DTF_DATE( sValue1 ) - DTF_DATE( sValue2 );
    
    /*
     * result value 설정.
     */

    /* - 연산 수행결과를 numeric type으로 변환. */
    STL_TRY( dtdToNumericFromInt64( sInterval,
                                    gResultDataTypeDef[DTL_TYPE_NUMBER].mArgNum1,
                                    gResultDataTypeDef[DTL_TYPE_NUMBER].mArgNum2,
                                    & sNumericDateSubValue,
                                    (stlErrorStack *)aEnv )
             == STL_SUCCESS );

    /* ( - 연산 수행결과 ) / DTL_USECS_PER_DAY => day 단위로 변환 */
    STL_TRY( dtfNumericDivision( & sNumericDateSubValue,
                                 & sNumericUsecsPerDay,
                                 sResultValue,
                                 (stlErrorStack *)aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;    
}

/*******************************************************************************
 * subtraction ( - ) : date - integer
 *   date - integer         => [ P, S(?), O ]
 *   DATEDIFF( date, date ) => [ M ]
 * ex) date'2001-10-01' - integer'7'  =>  timestamp'2001-09-24 00:00:00'
 *******************************************************************************/

/**
 * @brief date - integer
 *        <BR> subtraction ( - ) : date - integer
 *        <BR>   date - integer         => [ P, S(?), O ]
 *        <BR>   DATEDIFF( date, date ) => [ M ]
 *        <BR> ex) date'2001-10-01' - integer'7'  =>  timestamp'2001-09-24 00:00:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> DATE - INTEGER
 * @param[out] aResultValue      연산 결과 (결과타입 DATE)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfDateSubInteger( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sValue2;
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;

    dtlDateType      sDate;
    dtlDateType      sIntegerDate = 0;
    stlBool          sIsValid = STL_FALSE;    

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_DATE, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_NATIVE_INTEGER, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_DATE, sErrorStack );

    STL_TRY_THROW( ( DTF_INTEGER( sValue2 ) <= DTL_JULIAN_MAXJULDAY ) &&
                   ( DTF_INTEGER( sValue2 ) >= -DTL_JULIAN_MAXJULDAY ),
                   ERROR_OUT_OF_RANGE );

    /* integer 값을 microsecond 단위로 변환한다. */
    sIntegerDate = DTF_INTEGER( sValue2 ) * DTL_USECS_PER_DAY;

    /* Sub 연산을 한다. */
    sDate = DTF_DATE( sValue1 ) - sIntegerDate;

    /* overflow check */    
    if( ( DTF_DIFFSIGN( DTF_DATE( sValue1 ), sIntegerDate )
          == STL_TRUE ) &&
        ( DTF_DIFFSIGN( sDate, DTF_DATE( sValue1 ) )
          == STL_TRUE ) )
    {
        STL_THROW( ERROR_OUT_OF_RANGE );
    }
    else
    {
        /* Do Nothing */
    }

    DTF_DATE( sResultValue ) = sDate;
    sResultValue->mLength = DTL_DATE_SIZE;

    /*
     * 연산결과 validate
     */
    
    DTF_DATETIME_VALUE_IS_VALID( sDate, sIsValid );
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
 * @brief date - numeric
 *        <BR> subtraction ( - ) : date - numeric
 *        <BR>   date - numeric         => [ P, S(?), O ]
 *        <BR>   DATEDIFF( date, date ) => [ M ]
 *        <BR> ex) date'2001-10-01' - numeric'7'  =>  timestamp'2001-09-24 00:00:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> DATE - NUMERIC
 * @param[out] aResultValue      연산 결과 (결과타입 DATE)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfDateSubNumeric( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aEnv )
{
    dtlDataValue   * sValue1;
    dtlDataValue   * sValue2;
    dtlDataValue   * sResultValue;
    stlErrorStack  * sErrorStack;

    dtlDateType      sDate = 0;    
    dtlDateType      sNumericDate = 0;

    stlBool          sIsTruncated;
    stlBool          sIsValid = STL_FALSE;    

    stlInt32         sNumericAlignMaxSize = STL_ALIGN_DEFAULT(DTL_NUMERIC_MAX_SIZE);

    stlUInt8         sNumeric[STL_ALIGN_DEFAULT(DTL_NUMERIC_MAX_SIZE)];
    stlUInt8         sTempNumeric[STL_ALIGN_DEFAULT(DTL_NUMERIC_MAX_SIZE)];
    dtlDataValue     sNumericValue;
    dtlDataValue     sTempNumericValue;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_DATE, sErrorStack );
    DTF_CHECK_DTL_VALUE2( sValue2, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_DATE, sErrorStack );

    sNumericValue.mType = DTL_TYPE_NUMBER;
    sNumericValue.mBufferSize = sNumericAlignMaxSize;
    sNumericValue.mValue = (dtlNumericType *)(&sNumeric);

    sTempNumericValue.mType = DTL_TYPE_NUMBER;
    sTempNumericValue.mBufferSize = sNumericAlignMaxSize;
    sTempNumericValue.mValue = (dtlNumericType *)(&sTempNumeric);

    if( DTL_NUMERIC_IS_ZERO( (dtlNumericType *)(sValue2->mValue), sValue2->mLength ) )
    {
        sNumericDate = 0;
    }
    else
    {
        /**
         * numeric value를 timestamp value로 변경.
         */

        /**
         * NUMERIC * DTL_USECS_PER_DAY 
         */

        STL_TRY_THROW( dtdToNumericFromInt64( (stlInt64)DTL_USECS_PER_DAY,
                                              DTL_NUMERIC_MAX_PRECISION,
                                              0,
                                              & sTempNumericValue,
                                              sErrorStack )
                       == STL_SUCCESS,
                       ERROR_OUT_OF_RANGE );       
        
        STL_TRY_THROW( dtfNumericMultiplication( sValue2,
                                                 & sTempNumericValue,
                                                 & sNumericValue,
                                                 sErrorStack )
                       == STL_SUCCESS,
                       ERROR_OUT_OF_RANGE );       
        
        /**
         * NUMERIC * DTL_USECS_PER_DAY 를 integer 로 변경.
         */
        
        STL_TRY_THROW( dtfNumericRounding( & sNumericValue,
                                           0,
                                           & sTempNumericValue,
                                           sErrorStack )
                       == STL_SUCCESS,
                       ERROR_OUT_OF_RANGE );       
        
        STL_TRY_THROW( dtlNumericToInt64( & sTempNumericValue,
                                          & sNumericDate,
                                          & sIsTruncated,
                                          sErrorStack )
                       == STL_SUCCESS,
                       ERROR_OUT_OF_RANGE );

        /* numeric value를 fractional second에서 반올림한다. */
        STL_TRY( dtdAdjustTimestamp( & sNumericDate,
                                     0,
                                     sErrorStack )
                 == STL_SUCCESS );        
    }
    

    /**
     * DATE - NUMERIC
     */

    sDate = DTF_DATE( sValue1 ) - sNumericDate;

    STL_TRY( dtdAdjustDate( & sDate,
                            sErrorStack )
             == STL_SUCCESS );

    /* overflow check */    
    if( ( DTF_DIFFSIGN( DTF_DATE( sValue1 ), sNumericDate )
          == STL_TRUE ) &&
        ( DTF_DIFFSIGN( sDate, DTF_DATE( sValue1 ) )
          == STL_TRUE ) )
    {
        STL_THROW( ERROR_OUT_OF_RANGE );
    }
    else
    {
        /* Do Nothing */
    }
    
    /**
     * result value 설정
     */
    
    DTF_DATE( sResultValue ) = sDate;
    sResultValue->mLength = DTL_DATE_SIZE;

    /*
     * 연산결과 validate
     */

    DTF_DATETIME_VALUE_IS_VALID( sDate, sIsValid );
    STL_TRY_THROW( sIsValid == STL_TRUE, ERROR_OUT_OF_RANGE );
    
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        (void)stlPopError( sErrorStack );
        
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATETIME_FIELD_OVERFLOW,
                      NULL,
                      sErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;    
}


/*******************************************************************************
 * subtraction ( - ) : date - interval year to month, SUBDATE(date, INTERVAL year to month expr)
 *   date - interval year to month          => [ P, S, M, O ]
 *   SUBDATE(date, INTERVAL year to month expr) => [ M ]
 * ex) date'2001-09-28' - interval'1'day  =>  timestamp'2001-09-27 00:00:00'
 *******************************************************************************/

/**
 * @brief date - interval year to month
 *        <BR> subtraction ( - ) :
 *        <BR>   date - interval year to month, SUBDATE(date, INTERVAL year to month expr)
 *        <BR>   date - interval year to month              => [ P, S, M, O ]
 *        <BR>   SUBDATE(date, INTERVAL year to month expr) => [ M ]
 *        <BR> ex) date'2001-09-28' - interval'1'day  =>  timestamp'2001-09-27 00:00:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> DATE - INTERVAL YEAR TO MONTH
 * @param[out] aResultValue      연산 결과 (결과타입 DATE)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfDateSubIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aEnv )
{
    dtlDataValue   * sValue1;
    dtlDataValue   * sValue2;
    dtlDataValue   * sResultValue;
    stlErrorStack  * sErrorStack;

    dtlExpTime       sDtlExpTime;
    dtlDateType      sDateType;
    dtlFractionalSecond  sFractionalSecond = 0;    

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack  = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_DATE, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_INTERVAL_YEAR_TO_MONTH, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_DATE, sErrorStack );

    /* dtlDateType을 dtlExpTime으로 변경. */
    DTL_INIT_DTLEXPTIME( sDtlExpTime );

    sDateType = DTF_DATE(sValue1);
    
    STL_TRY( dtdDateTime2dtlExpTime( sDateType,
                                     & sDtlExpTime,
                                     & sFractionalSecond,
                                     sErrorStack )
             == STL_SUCCESS );

    STL_DASSERT( sFractionalSecond == 0 );
     
    /* - 연산을 한다. */    
    sDtlExpTime.mMonth =
        sDtlExpTime.mMonth - ((dtlIntervalYearToMonthType *)(sValue2->mValue))->mMonth;

    if( sDtlExpTime.mMonth > DTL_MONTHS_PER_YEAR )
    {
        sDtlExpTime.mYear += (sDtlExpTime.mMonth - 1) / DTL_MONTHS_PER_YEAR;
        sDtlExpTime.mMonth = ((sDtlExpTime.mMonth - 1) % DTL_MONTHS_PER_YEAR) + 1;
    }
    else if( sDtlExpTime.mMonth < 1 )
    {
        sDtlExpTime.mYear += (sDtlExpTime.mMonth / DTL_MONTHS_PER_YEAR) - 1;
        sDtlExpTime.mMonth = (sDtlExpTime.mMonth % DTL_MONTHS_PER_YEAR) + DTL_MONTHS_PER_YEAR;
    }
    else
    {
        // Do Nothing
    }
                               
    if( sDtlExpTime.mDay > dtlDayTab[DTL_IS_LEAP(sDtlExpTime.mYear)][sDtlExpTime.mMonth - 1] )
    {
        sDtlExpTime.mDay = dtlDayTab[DTL_IS_LEAP(sDtlExpTime.mYear)][sDtlExpTime.mMonth - 1];
    }
    else
    {
        // Do Nothing
    }
    
    STL_TRY_THROW( dtdDtlExpTime2DateTimeOffset( & sDtlExpTime,
                                                 0,  /* fractional second */
                                                 & sDateType,
                                                 sErrorStack )
                   == STL_SUCCESS,
                   ERROR_OUT_OF_RANGE );       

    DTF_DATE(sResultValue) = sDateType;
    sResultValue->mLength = DTL_DATE_SIZE;

    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        (void)stlPopError( sErrorStack );
        
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATETIME_FIELD_OVERFLOW,
                      NULL,
                      sErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/*******************************************************************************
 * subtraction ( - ) : date - interval day to second, SUBDATE(date, INTERVAL day to second expr)
 *   date - interval day to second              => [ P, S, M, O ]
 *   SUBDATE(date, INTERVAL day to second expr) => [ M ]
 * ex) date'2001-09-28' - interval'1'hour  =>  timestamp'2001-09-27 23:00:00'
 *******************************************************************************/

/**
 * @brief date - interval day
 *        <BR> subtraction ( - ) :
 *        <BR?   date - interval day, SUBDATE(date, INTERVAL day)
 *        <BR>   date - interval day              => [ P, S, M, O ]
 *        <BR>   SUBDATE(date, INTERVAL day) => [ M ]
 *        <BR> ex) date'2001-09-28' - interval'1'day  =>  date'2001-09-27'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> DATE - INTERVAL DAY TO SECOND( interval day )
 * @param[out] aResultValue      연산 결과 (결과타입 DATE)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfDateSubIntervalDay( stlUInt16        aInputArgumentCnt,
                                 dtlValueEntry  * aInputArgument,
                                 void           * aResultValue,
                                 dtlFuncVector  * aVectorFunc,
                                 void           * aVectorArg,
                                 void           * aEnv )
{
    dtlDataValue   * sValue1;
    dtlDataValue   * sValue2;
    dtlDataValue   * sResultValue;
    stlErrorStack  * sErrorStack;

    dtlDateType      sDateType;
    dtlDateType      sIntervalDayDate = 0;
    stlBool          sIsValid = STL_FALSE;

    dtlIntervalDayToSecondType  * sIntervalDayToSecond;    

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack  = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_DATE, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_INTERVAL_DAY_TO_SECOND, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_DATE, sErrorStack );

    /*
     * (1) dtlDateType에 dtlIntervalDayToSecond.mDay를 더해서 dtlTimestampType으로 만든다.
     */

    sIntervalDayToSecond = ((dtlIntervalDayToSecondType *)(sValue2->mValue));

    /* DTL_INTERVAL_INDICATOR_DAY 만 이 함수로 올 수 있다. */
    STL_DASSERT( sIntervalDayToSecond->mIndicator == DTL_INTERVAL_INDICATOR_DAY );
    STL_DASSERT( sIntervalDayToSecond->mTime == 0 );    

    /* interval day 값을 microsecond 단위로 변환한다. */
    sIntervalDayDate = sIntervalDayToSecond->mDay * DTL_USECS_PER_DAY;
    
    /*
     * dtlDateType - dtlIntervalDayToSecond.mDay
     *
     * -에 대한 overflow는 발생하지 않는다. 
     *  '4174-11-24 23:59:59 BC' - '999999' 와 같이
     *  date min값 - interval day max값을 연산하여도 stlInt64 범위에 포함됨.
     */
    sDateType = DTF_DATE(sValue1) - sIntervalDayDate;

    DTF_DATE(sResultValue) = sDateType;
    sResultValue->mLength = DTL_DATE_SIZE;

    /*
     * 연산결과 validate
     */
    
    DTF_DATETIME_VALUE_IS_VALID( sDateType, sIsValid );
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
 * @brief date - interval day to second
 *        <BR> subtraction ( - ) :
 *        <BR?   date - interval day to second, SUBDATE(date, INTERVAL day to second expr)
 *        <BR>   date - interval day to second              => [ P, S, M, O ]
 *        <BR>   SUBDATE(date, INTERVAL day to second expr) => [ M ]
 *        <BR> ex) date'2001-09-28' - interval'1'hour  =>  timestamp'2001-09-27 23:00:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> DATE - INTERVAL DAY TO SECOND
 * @param[out] aResultValue      연산 결과 (결과타입 TIMESTAMP without time zone)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfDateSubIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv )
{
    dtlDataValue   * sValue1;
    dtlDataValue   * sValue2;
    dtlDataValue   * sResultValue;
    stlErrorStack  * sErrorStack;

    stlBool          sIsValid = STL_FALSE;    
    dtlTimestampType              sTimestampType;    
    dtlIntervalDayToSecondType  * sIntervalDayToSecond;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack  = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_DATE, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_INTERVAL_DAY_TO_SECOND, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIMESTAMP, sErrorStack );

    /*
     * (1) dtlDateType에 dtlIntervalDayToSecond.mDay를 더한다.
     */
    sIntervalDayToSecond = ((dtlIntervalDayToSecondType *)(sValue2->mValue));

    /* DTL_INTERVAL_INDICATOR_DAY 을 제외한 day to second indicator 가 함수로 올 수 있다. */
    STL_DASSERT( (sIntervalDayToSecond->mIndicator == DTL_INTERVAL_INDICATOR_HOUR) ||
                 (sIntervalDayToSecond->mIndicator == DTL_INTERVAL_INDICATOR_MINUTE) ||
                 (sIntervalDayToSecond->mIndicator == DTL_INTERVAL_INDICATOR_SECOND) ||
                 (sIntervalDayToSecond->mIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_HOUR) ||
                 (sIntervalDayToSecond->mIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE) ||
                 (sIntervalDayToSecond->mIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_SECOND) ||
                 (sIntervalDayToSecond->mIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE) ||
                 (sIntervalDayToSecond->mIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND) ||
                 (sIntervalDayToSecond->mIndicator == DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND) );
    
    /*
     * dtlDateType - dtlIntervalDayToSecond
     * 
     * -에 대한 overflow는 발생하지 않는다. 
     *  '4174-11-24 23:59:59 BC' - '999999 23:59:59.999999' 와 같이
     *  date min값 - interval day to second max값을 연산하여도 stlInt64 범위에 포함됨.
     */
    
    sTimestampType = DTF_DATE(sValue1) -
        ( (sIntervalDayToSecond->mDay * DTL_USECS_PER_DAY) + sIntervalDayToSecond->mTime );

    DTF_TIMESTAMP( sResultValue ) = sTimestampType;
    sResultValue->mLength = DTL_TIMESTAMP_SIZE;

    /*
     * 연산결과 validate
     */
    
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

/*******************************************************************************
 * subtraction ( - ) : time without time zone - time without time zone,
 *                     SUBTIME( time without time zone, time without time zone )
 *   time - time         => [ P, S, M ]
 *   SUBTIME(time, time) => [ M ]
 * ex) time'05:00' - time'03:00'  =>  interval'02:00:00'
 *******************************************************************************/

/**
 * @brief time without time zone - time without time zone
 *        <BR> subtraction ( - )
 *        <BR> time without time zone - time without time zone,
 *        <BR> SUBTIME(time without time zone, time without time zone)
 *        <BR>   time without time zone - time without time zone         => [ P, S, M ]
 *        <BR>   SUBTIME(time without time zone, time without time zone) => [ M ]
 *        <BR> ex) time'05:00' - time'03:00'  =>  interval'02:00:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> TIME without time zone - TIME without time zone
 * @param[out] aResultValue      연산 결과 (결과타입 INTERVAL DAY TO SECOND)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimeSubTime( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sValue2;
    dtlDataValue  * sResultValue;

    dtlTimeType     sTimeType;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_TIME, (stlErrorStack *)aEnv );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_TIME, (stlErrorStack *)aEnv );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_INTERVAL_DAY_TO_SECOND, (stlErrorStack *)aEnv );

    /* - 연산 수행 */
    sTimeType = DTF_TIME(sValue1) - DTF_TIME(sValue2);

    /* result 결과 생성 */
    ((dtlIntervalDayToSecondType *)(sResultValue->mValue))->mIndicator
        = DTL_INTERVAL_INDICATOR_DAY_TO_SECOND;
    ((dtlIntervalDayToSecondType *)(sResultValue->mValue))->mDay = 0;
    ((dtlIntervalDayToSecondType *)(sResultValue->mValue))->mTime = (dtlTimeOffset)sTimeType;
    sResultValue->mLength = DTL_INTERVAL_DAY_TO_SECOND_SIZE;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*******************************************************************************
 * subtraction ( - ) : time without time zone - integer
 * ex) time'05:00' - 2  =>  time'05:00:00'
 *******************************************************************************/

/**
 * @brief time without time zone - integer
 *        <BR> subtraction ( - ) : time without time zone - integer
 *        <BR> ex) time'05:00' - 2  =>  time'05:00:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> TIME without time zone - INTEGER
 * @param[out] aResultValue      연산 결과 (결과타입 TIME without time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimeSubInteger( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aEnv )
{
    dtlDataValue    * sResultValue;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( aInputArgument[0].mValue.mDataValue, DTL_TYPE_TIME, (stlErrorStack *)aEnv );
    DTF_CHECK_DTL_VALUE( aInputArgument[1].mValue.mDataValue, DTL_TYPE_NATIVE_INTEGER, (stlErrorStack *)aEnv );    

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIME, (stlErrorStack *)aEnv );

    /* 연산할 필요없이 Time 값을 그대로 반환 */
    DTF_TIME(sResultValue) = DTF_TIME(aInputArgument[0].mValue.mDataValue);
    sResultValue->mLength  = DTL_TIME_SIZE;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*******************************************************************************
 * time - numeric
 * ex) time'13:00:00' - numeric'0.5'  =>  time'01:00:00'
 *******************************************************************************/

/**
 * @brief time without time zone - numeric
 *        <BR> time without time zone - numeric
 *        <BR> ex) time'13:00:00' - numeric'0.5'  =>  time'01:00:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> TIME without time zone - NUMERIC
 * @param[out] aResultValue      연산 결과 (결과타입 time without time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimeSubNumeric( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;   
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;

    dtlTimeType       sNumericTime;
    dtlTimeType       sTime;

    stlInt32          sDigitCount;
    stlInt32          sExponent;
    
    stlBool           sIsTruncated;

    stlInt32          sNumericAlignMaxSize = STL_ALIGN_DEFAULT(DTL_NUMERIC_MAX_SIZE);

    stlUInt8          sNumeric[STL_ALIGN_DEFAULT(DTL_NUMERIC_MAX_SIZE)];
    stlUInt8          sTempNumeric[STL_ALIGN_DEFAULT(DTL_NUMERIC_MAX_SIZE)];
    stlUInt8          sTempTimeNumeric[STL_ALIGN_DEFAULT(DTL_NUMERIC_MAX_SIZE)];
    dtlDataValue      sNumericValue;
    dtlDataValue      sTempNumericValue;
    dtlDataValue      sTempTimeNumericValue;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack  = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_TIME, sErrorStack );
    DTF_CHECK_DTL_VALUE2( sValue2, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIME, sErrorStack );

    sNumericValue.mType = DTL_TYPE_NUMBER;
    sNumericValue.mBufferSize = sNumericAlignMaxSize;
    sNumericValue.mValue = (dtlNumericType *)(&sNumeric);

    sTempNumericValue.mType = DTL_TYPE_NUMBER;
    sTempNumericValue.mBufferSize = sNumericAlignMaxSize;
    sTempNumericValue.mValue = (dtlNumericType *)(&sTempNumeric);

    sTempTimeNumericValue.mType = DTL_TYPE_NUMBER;
    sTempTimeNumericValue.mBufferSize = sNumericAlignMaxSize;
    sTempTimeNumericValue.mValue = (dtlNumericType *)(&sTempTimeNumeric);
    
    if( DTL_NUMERIC_IS_ZERO( (dtlNumericType *)(sValue2->mValue), sValue2->mLength ) )
    {
        sNumericTime = 0;
    }

    sDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( sValue2->mLength );
    sExponent = DTL_NUMERIC_GET_EXPONENT( DTF_NUMERIC( sValue2 ) );
    
    if( ( sExponent < 0 ) || ( sDigitCount > (sExponent + 1) ) ) 
    {
        /**
         * numeric value를 timestamp value로 변경.
         * (소수점이 있는 경우로, 시분초계산한다.)
         */

        /**
         * NUMERIC * DTL_USECS_PER_DAY 
         */

        STL_TRY( dtdToNumericFromInt64( (stlInt64)DTL_USECS_PER_DAY,
                                        DTL_NUMERIC_MAX_PRECISION,
                                        0,
                                        & sTempNumericValue,
                                        sErrorStack )
                 == STL_SUCCESS );

        STL_TRY( dtfNumericMultiplication( sValue2,
                                           & sTempNumericValue,
                                           & sNumericValue,
                                           sErrorStack )
                 == STL_SUCCESS );
        
        /**
         * NUMERIC * DTL_USECS_PER_DAY 를 integer 로 변경.
         */
        
        STL_TRY( dtfNumericRounding( & sNumericValue,
                                     0,
                                     & sTempTimeNumericValue,
                                     sErrorStack )
                 == STL_SUCCESS );

        STL_TRY( dtlNumericToInt64( & sTempTimeNumericValue,
                                    & sNumericTime,
                                    & sIsTruncated,
                                    sErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        /* 일(day)만 있는 경우 */
        sNumericTime = 0;
    }
    
    /**
     * TIME + NUMERIC
     */

    sTime = DTF_TIME( sValue1 ) - sNumericTime;    

    sTime -= sTime / DTL_USECS_PER_DAY * DTL_USECS_PER_DAY;

    if( sTime < 0 )
    {
        sTime += DTL_USECS_PER_DAY;
    }
    else
    {
        // Do Nothing
    }

    DTF_TIME( sResultValue ) = sTime;
    sResultValue->mLength = DTL_TIME_SIZE;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/*******************************************************************************
 * subtraction ( - ) : time without time zone - interval year to month
 *   time without time zone - interval year to month => [ P, S ]
 * ex) time'05:00' - interval'2'day  =>  time'05:00:00'
 *******************************************************************************/

/**
 * @brief time without time zone - interval year to month
 *        <BR> subtraction ( - ) : time without time zone - interval year to month
 *        <BR>   time without time zone - interval year to month => [ P, S ]
 *        <BR> ex) time'05:00' - interval'2'day  =>  time'05:00:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> TIME without time zone - INTERVAL YEAR TO MONTH
 * @param[out] aResultValue      연산 결과 (결과타입 TIME without time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimeSubIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aEnv )
{
    dtlDataValue    * sResultValue;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( aInputArgument[0].mValue.mDataValue, DTL_TYPE_TIME, (stlErrorStack *)aEnv );
    DTF_CHECK_DTL_VALUE( aInputArgument[1].mValue.mDataValue, DTL_TYPE_INTERVAL_YEAR_TO_MONTH, (stlErrorStack *)aEnv );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIME, (stlErrorStack *)aEnv );

    /* 연산할 필요없이 Time 값을 그대로 반환 */
    DTF_TIME(sResultValue) = DTF_TIME(aInputArgument[0].mValue.mDataValue);
    sResultValue->mLength  = DTL_TIME_SIZE;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*******************************************************************************
 * subtraction ( - ) : time without time zone - interval day to second
 *   time without time zone - interval day to second => [ P, S ]
 * ex) time'05:00' - interval'2'hours  =>  time'03:00:00'
 *******************************************************************************/

/**
 * @brief time without time zone - interval day to second
 *        <BR> subtraction ( - ) : time without time zone - interval day to second
 *        <BR>   time without time zone - interval day to second => [ P, S ]
 *        <BR> ex) time'05:00' - interval'2'hours  =>  time'03:00:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> TIME without time zone - INTERVAL DAY TO SECOND
 * @param[out] aResultValue      연산 결과 (결과타입 TIME without time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimeSubIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlDataValue    * sResultValue;

    dtlTimeType       sTimeType;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_TIME, (stlErrorStack *)aEnv );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_INTERVAL_DAY_TO_SECOND, (stlErrorStack *)aEnv );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIME, (stlErrorStack *)aEnv );

    /* - 연산후, time 값 반환 */
    sTimeType = DTF_TIME(sValue1) - ((dtlIntervalDayToSecondType *)sValue2->mValue)->mTime;
    sTimeType -= sTimeType / DTL_USECS_PER_DAY * DTL_USECS_PER_DAY;

    if( sTimeType < 0 )
    {
        sTimeType += DTL_USECS_PER_DAY;
    }
    else
    {
        // Do Nothing
    }
    
    DTF_TIME( sResultValue ) = sTimeType;
    sResultValue->mLength = DTL_TIME_SIZE;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/*******************************************************************************
 * time with time zone - integer
 * ex) timetz'01:00:00 -08:00' - integer '2'  =>  timetz'01:00:00 -08:00'
 *******************************************************************************/

/**
 * @brief time with time zone - integer
 *        <BR> time with time zone + integer, 
 *        <BR> ex) timetz'01:00:00' - integer '2'  =>  timetz'01:00:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> TIME with time zone - INTEGER
 * @param[out] aResultValue      연산 결과 (결과타입 time with time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimeTzSubInteger( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aEnv )
{
    dtlDataValue    * sValue1;    
    dtlDataValue    * sResultValue;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1      = aInputArgument[0].mValue.mDataValue;    
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_TIME_WITH_TIME_ZONE, (stlErrorStack *)aEnv );
    DTF_CHECK_DTL_VALUE( aInputArgument[1].mValue.mDataValue, DTL_TYPE_NATIVE_INTEGER, (stlErrorStack *)aEnv );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIME_WITH_TIME_ZONE, (stlErrorStack *)aEnv );

    /* 연산할 필요없이 Time 값을 그대로 반환 */
    ((dtlTimeTzType *)(sResultValue->mValue))->mTime = ((dtlTimeTzType *)(sValue1->mValue))->mTime;
    ((dtlTimeTzType *)(sResultValue->mValue))->mTimeZone = ((dtlTimeTzType *)(sValue1->mValue))->mTimeZone;
    sResultValue->mLength  = DTL_TIMETZ_SIZE;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/*******************************************************************************
 * time with time zone - numeric
 * ex) timetz'01:00:00 -08:00' - numeric'0.5'  =>  timetz'13:00:00 -08:00'
 *******************************************************************************/

/**
 * @brief time with time zone - numeric
 *        <BR> time with time zone - numeric
 *        <BR> ex) timetz'01:00:00 -08:00' - numeric'0.5'  =>  timetz'13:00:00 -08:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> TIME with time zone - NUMERIC
 * @param[out] aResultValue      연산 결과 (결과타입 time with time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimeTzSubNumeric( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;   
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;

    dtlTimeType       sNumericTime;
    dtlTimeType       sTime;    

    stlInt32          sDigitCount;
    stlInt32          sExponent;
    
    stlBool           sIsTruncated;

    stlInt32          sNumericAlignMaxSize = STL_ALIGN_DEFAULT(DTL_NUMERIC_MAX_SIZE);

    stlUInt8          sNumeric[STL_ALIGN_DEFAULT(DTL_NUMERIC_MAX_SIZE)];
    stlUInt8          sTempNumeric[STL_ALIGN_DEFAULT(DTL_NUMERIC_MAX_SIZE)];
    stlUInt8          sTempTimeNumeric[STL_ALIGN_DEFAULT(DTL_NUMERIC_MAX_SIZE)];
    dtlDataValue      sNumericValue;
    dtlDataValue      sTempNumericValue;
    dtlDataValue      sTempTimeNumericValue;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack  = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_TIME_WITH_TIME_ZONE, sErrorStack );
    DTF_CHECK_DTL_VALUE2( sValue2, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIME_WITH_TIME_ZONE, sErrorStack );

    sNumericValue.mType = DTL_TYPE_NUMBER;
    sNumericValue.mBufferSize = sNumericAlignMaxSize;
    sNumericValue.mValue = (dtlNumericType *)(&sNumeric);

    sTempNumericValue.mType = DTL_TYPE_NUMBER;
    sTempNumericValue.mBufferSize = sNumericAlignMaxSize;
    sTempNumericValue.mValue = (dtlNumericType *)(&sTempNumeric);

    sTempTimeNumericValue.mType = DTL_TYPE_NUMBER;
    sTempTimeNumericValue.mBufferSize = sNumericAlignMaxSize;
    sTempTimeNumericValue.mValue = (dtlNumericType *)(&sTempTimeNumeric);
    
    if( DTL_NUMERIC_IS_ZERO( (dtlNumericType *)(sValue2->mValue), sValue2->mLength ) )
    {
        sNumericTime = 0;
    }

    sDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( sValue2->mLength );
    sExponent = DTL_NUMERIC_GET_EXPONENT( DTF_NUMERIC( sValue2 ) );
    
    if( ( sExponent < 0 ) || ( sDigitCount > (sExponent + 1) ) ) 
    {
        /**
         * numeric value를 timestamp value로 변경.
         * (소수점이 있는 경우로, 시분초계산한다.)
         */

        /**
         * NUMERIC * DTL_USECS_PER_DAY 
         */

        STL_TRY( dtdToNumericFromInt64( (stlInt64)DTL_USECS_PER_DAY,
                                        DTL_NUMERIC_MAX_PRECISION,
                                        0,
                                        & sTempNumericValue,
                                        sErrorStack )
                 == STL_SUCCESS );

        STL_TRY( dtfNumericMultiplication( sValue2,
                                           & sTempNumericValue,
                                           & sNumericValue,
                                           sErrorStack )
                 == STL_SUCCESS );
        
        /**
         * NUMERIC * DTL_USECS_PER_DAY 를 integer 로 변경.
         */
        
        STL_TRY( dtfNumericRounding( & sNumericValue,
                                     0,
                                     & sTempTimeNumericValue,
                                     sErrorStack )
                 == STL_SUCCESS );
        
        STL_TRY( dtlNumericToInt64( & sTempTimeNumericValue,
                                    & sNumericTime,
                                    & sIsTruncated,
                                    sErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        /* 일(day)만 있는 경우 */
        sNumericTime = 0;
    }
    
    /**
     * TIME WITH TIME ZONE - NUMERIC
     */

    sTime = ((dtlTimeTzType *)(sValue1->mValue))->mTime -
        (((dtlTimeTzType *)(sValue1->mValue))->mTimeZone  * DTL_USECS_PER_SEC);
    sTime = sTime - sNumericTime;    

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
        sTime + (((dtlTimeTzType *)(sValue1->mValue))->mTimeZone * DTL_USECS_PER_SEC );
    ((dtlTimeTzType *)(sResultValue->mValue))->mTimeZone = ((dtlTimeTzType *)(sValue1->mValue))->mTimeZone;

    sResultValue->mLength = DTL_TIMETZ_SIZE;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/*******************************************************************************
 * time with time zone - interval_year_to_month
 *   time with time zone - interval => [ P, S ]
 * ex) timetz'01:00:00.123 -8:00' + interval'1'year  =>  time'01:00:00.123 -8:00'
 *******************************************************************************/
/**
 * @brief time with time zone - interval_year_to_month
 *        <BR> time with time zone - interval_year_to_month
 *        <BR> time with time zone - interval_year_to_month => [ P, S ]
 *        <BR> ex) time'01:00:00.123 -8:00' - interval'1'year  =>  time'1:00:00.123 -8:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> time with time zone - interval YEAR TO MONTH
 * @param[out] aResultValue      연산 결과 (결과타입 TIME with time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimeTzSubIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sResultValue;
    dtlTimeTzType     sTimeTzType;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1      = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_TIME_WITH_TIME_ZONE, (stlErrorStack *)aEnv );
    DTF_CHECK_DTL_VALUE( aInputArgument[1].mValue.mDataValue, DTL_TYPE_INTERVAL_YEAR_TO_MONTH, (stlErrorStack *)aEnv );
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIME_WITH_TIME_ZONE, (stlErrorStack *)aEnv );

    sTimeTzType.mTime     = ((dtlTimeTzType *)(sValue1->mValue))->mTime;
    sTimeTzType.mTimeZone = ((dtlTimeTzType *)(sValue1->mValue))->mTimeZone;

    /* 연산할 필요없이 Time 값을 그대로 반환 */
    *(dtlTimeTzType *)(sResultValue->mValue ) = sTimeTzType;
    sResultValue->mLength = DTL_TIMETZ_SIZE;
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/*******************************************************************************
 * time with time zone - interval_day_to_second
 *   time with time zone - interval => [ P, S ]
 * ex) time with time zone'05:00:00.000 -8:00' - interval'3'hour  =>  time'02:00:00.000 -8:00'
 *******************************************************************************/
/**
 * @brief time with time zone - interval_day_to_second
 *        <BR> time with time zone - interval_day_to_second
 *        <BR> time with time zone - interval_day_to_second => [ P, S ]
 *        <BR> ex) time with time zone'05:00:01.123 -8:00' + interval'3' hours  =>  time'02:00:01.123 -8:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> time with time zone - interval DAY TO SECOND
 * @param[out] aResultValue      연산 결과 (결과타입 TIME with time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimeTzSubIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlDataValue    * sResultValue;

    dtlTimeType       sTimeType;
    stlInt32          sTimeZone;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1      = aInputArgument[0].mValue.mDataValue;
    sValue2      = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_TIME_WITH_TIME_ZONE, (stlErrorStack *)aEnv );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_INTERVAL_DAY_TO_SECOND, (stlErrorStack *)aEnv );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue,
                          DTL_TYPE_TIME_WITH_TIME_ZONE,
                          (stlErrorStack *)aEnv );

    sTimeZone = ((dtlTimeTzType *)(sValue1->mValue))->mTimeZone;
    
    /* TimeTzType -> TimeType */
    sTimeType = ((dtlTimeTzType *)(sValue1->mValue))->mTime - ( sTimeZone * DTL_USECS_PER_SEC );

    /* Sub */
    sTimeType -= ((dtlIntervalDayToSecondType *)sValue2->mValue )->mTime;
    
    sTimeType -= sTimeType / DTL_USECS_PER_DAY * DTL_USECS_PER_DAY;

    if( sTimeType < 0 )
    {
        sTimeType += DTL_USECS_PER_DAY;
    }
    else
    {
        // Do Nothing
    }
    
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


/*******************************************************************************
 * timestamp - integer
 *   timestamp - integer => [ O, M, P ]
 * ex) timestamp'2001-09-28 01:00:00' - integer '2'  =>  timestamp'2001-09-26 01:00:00'
 *******************************************************************************/

/**
 * @brief timestamp without time zone - integer
 *        <BR> timestamp without time zone - integer, 
 *        <BR>   timestamp without time zone - integer => [ O, M, P]
 *        <BR> ex) timestamp'2001-09-28 01:00:00' - integer '2'  =>  timestamp'2001-09-26 01:00:00' 
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> TIMESTAMP without time zone + INTEGER
 * @param[out] aResultValue      연산 결과 (결과타입 timestamp without time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimestampSubInteger( stlUInt16        aInputArgumentCnt,
                                  dtlValueEntry  * aInputArgument,
                                  void           * aResultValue,
                                  void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;

    dtlTimestampType  sTimestamp;
    dtlTimestampType  sIntegerTimestamp;
    stlBool           sIsValid = STL_FALSE;    

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack  = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_TIMESTAMP, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_NATIVE_INTEGER, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIMESTAMP, sErrorStack );

    STL_TRY_THROW( ( DTF_INTEGER( sValue2 ) <= DTL_JULIAN_MAXJULDAY ) &&
                   ( DTF_INTEGER( sValue2 ) >= -DTL_JULIAN_MAXJULDAY ),
                   ERROR_OUT_OF_RANGE );

    sIntegerTimestamp = DTF_INTEGER( sValue2 ) * DTL_USECS_PER_DAY;

    sTimestamp = DTF_TIMESTAMP( sValue1 ) - sIntegerTimestamp;

    /* overflow check */
    if( ( DTF_DIFFSIGN( DTF_TIMESTAMP( sValue1 ), sIntegerTimestamp )
          == STL_TRUE ) &&
        ( DTF_DIFFSIGN( sTimestamp, DTF_TIMESTAMP( sValue1 ) )
          == STL_TRUE ) )
    {
        STL_THROW( ERROR_OUT_OF_RANGE );
    }
    else
    {
        /* Do Nothing */
    }
        
    DTF_TIMESTAMP( sResultValue ) = sTimestamp;
    sResultValue->mLength = DTL_TIMESTAMP_SIZE;

    /*
     * 연산결과 validate
     */
    
    DTF_DATETIME_VALUE_IS_VALID( sTimestamp, sIsValid );
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

/*******************************************************************************
 * timestamp - numeric
 *   timestamp - numeric => [ O, M, P ]
 * ex) timestamp'2001-09-28 01:00:00' - numeric'2'  =>  timestamp'2001-09-26 01:00:00'
 *******************************************************************************/

/**
 * @brief timestamp without time zone - numeric
 *        <BR> timestamp without time zone - numeric, 
 *        <BR>   timestamp without time zone - numeric => [ O, M ]
 *        <BR> ex) timestamp'2001-09-28 01:00:00' - numeric '2'  =>  timestamp'2001-09-26 01:00:00'
 *       
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> TIMESTAMP without time zone + NUMERIC
 * @param[out] aResultValue      연산 결과 (결과타입 timestamp without time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimestampSubNumeric( stlUInt16        aInputArgumentCnt,
                                  dtlValueEntry  * aInputArgument,
                                  void           * aResultValue,
                                  void           * aEnv )
{
    dtlDataValue       * sValue1;
    dtlDataValue       * sValue2;
    dtlDataValue       * sResultValue;
    stlErrorStack      * sErrorStack;

    dtlTimestampType   sNumericTimestamp;
    dtlTimestampType   sTimestamp;    

    stlBool          sIsTruncated;
    stlBool          sIsValid = STL_FALSE;        

    stlInt32         sNumericAlignMaxSize = STL_ALIGN_DEFAULT(DTL_NUMERIC_MAX_SIZE);
    
    stlUInt8         sNumeric[STL_ALIGN_DEFAULT(DTL_NUMERIC_MAX_SIZE)];
    stlUInt8         sTempNumeric[STL_ALIGN_DEFAULT(DTL_NUMERIC_MAX_SIZE)];    
    dtlDataValue     sNumericValue;
    dtlDataValue     sTempNumericValue;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack  = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_TIMESTAMP, sErrorStack );
    DTF_CHECK_DTL_VALUE2( sValue2, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIMESTAMP, sErrorStack );

    sNumericValue.mType = DTL_TYPE_NUMBER;
    sNumericValue.mBufferSize = sNumericAlignMaxSize;
    sNumericValue.mValue = (dtlNumericType *)(&sNumeric);

    sTempNumericValue.mType = DTL_TYPE_NUMBER;
    sTempNumericValue.mBufferSize = sNumericAlignMaxSize;
    sTempNumericValue.mValue = (dtlNumericType *)(&sTempNumeric);

    if( DTL_NUMERIC_IS_ZERO( (dtlNumericType *)(sValue2->mValue), sValue2->mLength ) )
    {
        sNumericTimestamp = 0;
    }
    else
    {
        /**
         * numeric value를 timestamp value로 변경.
         */

        /**
         * NUMERIC * DTL_USECS_PER_DAY 
         */

        STL_TRY_THROW( dtdToNumericFromInt64( (stlInt64)DTL_USECS_PER_DAY,
                                              DTL_NUMERIC_MAX_PRECISION,
                                              0,
                                              & sTempNumericValue,
                                              sErrorStack )
                       == STL_SUCCESS,
                       ERROR_OUT_OF_RANGE );       
        
        STL_TRY_THROW( dtfNumericMultiplication( sValue2,
                                                 & sTempNumericValue,
                                                 & sNumericValue,
                                                 sErrorStack )
                       == STL_SUCCESS,
                       ERROR_OUT_OF_RANGE );       

        /**
         * NUMERIC * DTL_USECS_PER_DAY 를 integer 로 변경.
         */

        STL_TRY_THROW( dtfNumericRounding( & sNumericValue,
                                           0,
                                           & sTempNumericValue,
                                           sErrorStack )
                       == STL_SUCCESS,
                       ERROR_OUT_OF_RANGE );       
        
        STL_TRY_THROW( dtlNumericToInt64( & sTempNumericValue,
                                          & sNumericTimestamp,
                                          & sIsTruncated,
                                          sErrorStack )
                       == STL_SUCCESS,
                       ERROR_OUT_OF_RANGE );       
    }

    /**
     * TIMESTAMP - NUMERIC
     */

    sTimestamp = DTF_TIMESTAMP( sValue1 ) - sNumericTimestamp;

    /* overflow check */    
    if( ( DTF_DIFFSIGN( DTF_TIMESTAMP(sValue1), sNumericTimestamp )
          == STL_TRUE ) &&
        ( DTF_DIFFSIGN( sTimestamp, DTF_TIMESTAMP(sValue1)  )
          == STL_TRUE ) )
    {
        STL_THROW( ERROR_OUT_OF_RANGE );
    }
    else
    {
        /* Do Nothing */
    }

    /**
     * result value 설정
     */
    
    DTF_TIMESTAMP( sResultValue ) = sTimestamp;
    sResultValue->mLength = DTL_TIMESTAMP_SIZE;

    /*
     * 연산결과 validate
     */
    
    DTF_DATETIME_VALUE_IS_VALID( sTimestamp, sIsValid );
    STL_TRY_THROW( sIsValid == STL_TRUE, ERROR_OUT_OF_RANGE );
    
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        (void)stlPopError( sErrorStack );
        
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATETIME_FIELD_OVERFLOW,
                      NULL,
                      sErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;    
}


/*******************************************************************************
 * subtraction ( - ) : timestamp without time zone - interval year to month
 *   timestamp without time zone - interval year to month => [ P, S, M, O ]
 * ex) timestamp'2001-09-28 23:00' - interval'2'day  =>  timestamp'2001-09-26 23:00:00'
 *******************************************************************************/

/**
 * @brief timestamp without time zone - interval year to month
 *        <BR> subtraction ( - ) : timestamp without time zone - interval year to month
 *        <BR>   timestamp without time zone - interval year to month => [ P, S, M, O ]
 *        <BR> ex) timestamp'2001-09-28 23:00' - interval'2'day => timestamp'2001-09-26 23:00:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> TIMESTAMP without time zone - INTERVAL YEAR TO MONTH
 * @param[out] aResultValue      연산 결과 (결과타입 timestamp without time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimestampSubIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;

    dtlExpTime           sDtlExpTime;
    dtlFractionalSecond  sFractionalSecond;
    dtlTimestampType     sTimestampType;    

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack  = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_TIMESTAMP, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_INTERVAL_YEAR_TO_MONTH, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIMESTAMP, sErrorStack );

    /* dtlTimestampType을 dtlExpTime으로 변경 */
    DTL_INIT_DTLEXPTIME( sDtlExpTime );
    STL_TRY( dtdDateTime2dtlExpTime( DTF_TIMESTAMP(sValue1),
                                     & sDtlExpTime,
                                     & sFractionalSecond,
                                     sErrorStack )
             == STL_SUCCESS );

    /* - 연산 */
    sDtlExpTime.mMonth =
        sDtlExpTime.mMonth - ((dtlIntervalYearToMonthType *)(sValue2->mValue))->mMonth;

    if( sDtlExpTime.mMonth > DTL_MONTHS_PER_YEAR )
    {
        sDtlExpTime.mYear += (sDtlExpTime.mMonth - 1) / DTL_MONTHS_PER_YEAR;
        sDtlExpTime.mMonth = ((sDtlExpTime.mMonth - 1) % DTL_MONTHS_PER_YEAR) + 1;
    }
    else if( sDtlExpTime.mMonth < 1 )
    {
        sDtlExpTime.mYear += (sDtlExpTime.mMonth / DTL_MONTHS_PER_YEAR) - 1;
        sDtlExpTime.mMonth = (sDtlExpTime.mMonth % DTL_MONTHS_PER_YEAR) + DTL_MONTHS_PER_YEAR;
    }
    else
    {
        // Do Nothing
    }

    if( sDtlExpTime.mDay > dtlDayTab[DTL_IS_LEAP(sDtlExpTime.mYear)][sDtlExpTime.mMonth - 1] )
    {
        sDtlExpTime.mDay = dtlDayTab[DTL_IS_LEAP(sDtlExpTime.mYear)][sDtlExpTime.mMonth - 1];
    }
    else
    {
        // Do Nothing
    }

    STL_TRY_THROW( dtdDtlExpTime2DateTimeOffset( & sDtlExpTime,
                                                 sFractionalSecond,
                                                 & sTimestampType,
                                                 sErrorStack )
                   == STL_SUCCESS,
                   ERROR_OUT_OF_RANGE );       

    DTF_TIMESTAMP(sResultValue) = sTimestampType;
    sResultValue->mLength = DTL_TIMESTAMP_SIZE;

    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        (void)stlPopError( sErrorStack );
        
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATETIME_FIELD_OVERFLOW,
                      NULL,
                      sErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/*******************************************************************************
 * subtraction ( - ) : timestamp without time zone - interval day to second
 *   timestamp without time zone - interval day to second => [ P, S, M, O ]
 * ex) timestamp'2001-09-28 23:00' - interval'2'day  =>  timestamp'2001-09-26 23:00:00'
 *******************************************************************************/

/**
 * @brief timestamp without time zone - interval day to second
 *        <BR> subtraction ( - ) : timestamp without time zone - interval day to second
 *        <BR>   timestamp without time zone - interval day to second => [ P, S, M, O ]
 *        <BR> ex) timestamp'2001-09-28 23:00' - interval'23'hour => timestamp'2001-09-28 00:00:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> TIMESTAMP without time zone - INTERVAL DAY TO SECOND
 * @param[out] aResultValue      연산 결과 (결과타입 timestamp without time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimestampSubIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aEnv )
{
    dtlDataValue   * sValue1;
    dtlDataValue   * sValue2;
    dtlDataValue   * sResultValue;
    stlErrorStack  * sErrorStack;

    stlBool          sIsValid = STL_FALSE;        
    dtlTimestampType             sTimestampType;
    dtlIntervalDayToSecondType * sIntervalDayToSecond;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack  = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_TIMESTAMP, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_INTERVAL_DAY_TO_SECOND, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIMESTAMP, sErrorStack );

    sTimestampType = DTF_TIMESTAMP(sValue1);

    /*
     * Interval day to second를 usec 단위로 변경
     */
    sIntervalDayToSecond = ((dtlIntervalDayToSecondType *)(sValue2->mValue));

    /*
     * sTimestampType은 julian과 같이 표현되있으므로
     * interval day에 USECS_PER_DAY를 곱해 Day를 julian과 같이 변환해준다.
     * 그리고 Time을 뺀다.
     */ 
    sTimestampType -= ( sIntervalDayToSecond->mDay * DTL_USECS_PER_DAY ) +
        ( sIntervalDayToSecond->mTime );

    DTF_TIMESTAMP( sResultValue ) = sTimestampType;
    sResultValue->mLength = DTL_TIMESTAMP_SIZE;

    /*
     * 연산결과 validate
     */
    
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

/*******************************************************************************
 * subtraction ( - ) : timestamp with time zone - timestamp with time zone
 *   timestamp with time zone - timestamp with time zone => [ P, S, O ]
 * ex) timestamptz'2001-09-29 03:00:00 -08:00' - timestamptz'2001-09-27 12:00:00 07:00'
 *     =>  interval'2 06:00:00'day to second
 *******************************************************************************/

/**
 * @brief timestamp with time zone - timestamp with time zone
 *        <BR> subtraction ( - ) : timestamp with time zone - timestamp with time zone
 *        <BR>   timestamp with time zone - timestamp with time zone => [ P, S, O ]
 *        <BR> ex) timestamptz'2001-09-29 03:00:00 -08:00' - timestamptz'2001-09-27 12:00:00 07:00'
 *        <BR>     =>  interval'2 06:00:00'day to second
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> TIMESTAMP with time zone - TIMESTAMP with time zone
 * @param[out] aResultValue      연산 결과 (결과타입 INTERVAL DAY TO SECOND)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimestampTzSubTimestampTz( stlUInt16        aInputArgumentCnt,
                                        dtlValueEntry  * aInputArgument,
                                        void           * aResultValue,
                                        void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;

    dtlTimestampType           sTimestamp;    
    dtlIntervalDayToSecondType sIntervalDayToSecond;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack  = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_INTERVAL_DAY_TO_SECOND, sErrorStack );

    /* - 연산 수행 */
    sTimestamp = ((dtlTimestampTzType *)(sValue1->mValue))->mTimestamp -
        ((dtlTimestampTzType *)(sValue2->mValue))->mTimestamp;

    /* interval day to second type 결과 타입 생성 */
    sIntervalDayToSecond.mIndicator = DTL_INTERVAL_INDICATOR_DAY_TO_SECOND;
    sIntervalDayToSecond.mDay = sTimestamp / DTL_USECS_PER_DAY;
    sIntervalDayToSecond.mTime = sTimestamp % DTL_USECS_PER_DAY;    

    /* 결과 타입의 precision 체크 */
    STL_TRY( dtdAdjustIntervalDayToSecond( & sIntervalDayToSecond,
                                           sIntervalDayToSecond.mIndicator,
                                           DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION,
                                           DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION,
                                           sErrorStack )
             == STL_SUCCESS );

    ((dtlIntervalDayToSecondType *)sResultValue->mValue)->mIndicator
        = sIntervalDayToSecond.mIndicator;
    ((dtlIntervalDayToSecondType *)sResultValue->mValue)->mDay = sIntervalDayToSecond.mDay;
    ((dtlIntervalDayToSecondType *)sResultValue->mValue)->mTime = sIntervalDayToSecond.mTime;
    sResultValue->mLength = DTL_INTERVAL_DAY_TO_SECOND_SIZE;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*******************************************************************************
 * timestamptz - integer
 *   timestamptz - integer => [ O, M ]
 * ex) timestamptz'2001-09-28 01:02:03.123 -8:00' + integer '2'  =>  timestamptz'2001-09-26 01:02:03.123 -8:00'
 *******************************************************************************/

/**
 * @brief timestamp with time zone - integer
 *        <BR> timestamp with time zone - integer, 
 *        <BR>   timestamp with time zone - integer => [ O, M ]
 *        <BR> ex) timestamptz'2001-09-28 01:02:03.123 -8:00' + integer '2'  =>  timestamptz'2001-09-26 01:02:03.123 -8:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> TIMESTAMP with time zone - INTEGER
 * @param[out] aResultValue      연산 결과 (결과타입 timestamp with time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimestampTzSubInteger( stlUInt16        aInputArgumentCnt,
                                    dtlValueEntry  * aInputArgument,
                                    void           * aResultValue,
                                    void           * aEnv )
{
    dtlDataValue     * sValue1;
    dtlDataValue     * sValue2;
    dtlDataValue     * sResultValue;
    stlErrorStack    * sErrorStack;

    dtlTimestampType   sTimestamp;
    dtlTimestampType   sIntegerTimestamp;
    dtlTimestampType   sAddTimestamp;
    stlBool            sIsValid = STL_FALSE;    
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack  = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_NATIVE_INTEGER, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, sErrorStack );

    /* TimestampTz -> TimestampType */
    sTimestamp = ((dtlTimestampTzType *)(sValue1->mValue))->mTimestamp -
        (((dtlTimestampTzType *)(sValue1->mValue))->mTimeZone * DTL_USECS_PER_SEC);

    STL_TRY_THROW( ( DTF_INTEGER( sValue2 ) <= DTL_JULIAN_MAXJULDAY ) &&
                   ( DTF_INTEGER( sValue2 ) >= -DTL_JULIAN_MAXJULDAY ),
                   ERROR_OUT_OF_RANGE );

    sIntegerTimestamp = DTF_INTEGER( sValue2 ) * DTL_USECS_PER_DAY;

    sAddTimestamp = sTimestamp - sIntegerTimestamp;

    /* overflow check */
    if( ( DTF_DIFFSIGN( sTimestamp, sIntegerTimestamp )
          == STL_TRUE ) &&
        ( DTF_DIFFSIGN( sAddTimestamp, sTimestamp )
          == STL_TRUE ) )
    {
        STL_THROW( ERROR_OUT_OF_RANGE );
    }
    else
    {
        /* Do Nothing */
    }
    
    /* Timezone을 원상복귀후, result value 설정 */
    ((dtlTimestampTzType *)( sResultValue->mValue ))->mTimestamp =
        sAddTimestamp + (((dtlTimestampTzType *)(sValue1->mValue))->mTimeZone * DTL_USECS_PER_SEC );
    ((dtlTimestampTzType *)( sResultValue->mValue ))->mTimeZone =
        ((dtlTimestampTzType *)(sValue1->mValue ))->mTimeZone;

    sResultValue->mLength = DTL_TIMESTAMPTZ_SIZE;

    /*
     * 연산결과 validate
     */
    
    DTF_DATETIME_VALUE_IS_VALID( sAddTimestamp, sIsValid );
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

/*******************************************************************************
 * timestamptz - numeric
 *   timestamptz - numeric => [ P, O, M ]
 * ex) timestamptz'2001-09-28 01:02:03.123 -8:00' + numeric '2'  =>  timestamptz'2001-09-26 01:02:03.123 -8:00'
 *******************************************************************************/

/**
 * @brief timestamp with time zone - numeric
 *        <BR> timestamp with time zone - numeric, 
 *        <BR>   timestamp with time zone - numeric => [ P, O, M ]
 *        <BR> ex) timestamptz'2001-09-28 01:02:03.123 -8:00' - numeric '2'  =>  timestamptz'2001-09-30 01:02:03.123 -8:00']
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> TIMESTAMP with time zone - NUMERIC
 * @param[out] aResultValue      연산 결과 (결과타입 timestamp with time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimestampTzSubNumeric( stlUInt16        aInputArgumentCnt,
                                    dtlValueEntry  * aInputArgument,
                                    void           * aResultValue,
                                    void           * aEnv )
{
    dtlDataValue      * sValue1;
    dtlDataValue      * sValue2;
    dtlDataValue      * sResultValue;
    stlErrorStack     * sErrorStack;

    dtlTimestampType   sTimestamp;
    dtlTimestampType   sNumericTimestamp;
    dtlTimestampType   sAddTimestamp;        

    stlBool          sIsTruncated;    
    stlBool          sIsValid = STL_FALSE;
    
    stlInt32         sNumericAlignMaxSize = STL_ALIGN_DEFAULT(DTL_NUMERIC_MAX_SIZE);
    
    stlUInt8         sNumeric[STL_ALIGN_DEFAULT(DTL_NUMERIC_MAX_SIZE)];
    stlUInt8         sTempNumeric[STL_ALIGN_DEFAULT(DTL_NUMERIC_MAX_SIZE)];    
    dtlDataValue     sNumericValue;
    dtlDataValue     sTempNumericValue;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack  = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, sErrorStack );
    DTF_CHECK_DTL_VALUE2( sValue2, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, sErrorStack );

    sNumericValue.mType = DTL_TYPE_NUMBER;
    sNumericValue.mBufferSize = sNumericAlignMaxSize;
    sNumericValue.mValue = (dtlNumericType *)(&sNumeric);

    sTempNumericValue.mType = DTL_TYPE_NUMBER;
    sTempNumericValue.mBufferSize = sNumericAlignMaxSize;
    sTempNumericValue.mValue = (dtlNumericType *)(&sTempNumeric);
    
    if( DTL_NUMERIC_IS_ZERO( (dtlNumericType *)(sValue2->mValue), sValue2->mLength ) )
    {
        sNumericTimestamp = 0;
    }
    else
    {
        /**
         * numeric value를 timestamp value로 변경.
         */

        /**
         * NUMERIC * DTL_USECS_PER_DAY 
         */

        STL_TRY_THROW( dtdToNumericFromInt64( (stlInt64)DTL_USECS_PER_DAY,
                                              DTL_NUMERIC_MAX_PRECISION,
                                              0,
                                              & sTempNumericValue,
                                              sErrorStack )
                       == STL_SUCCESS,
                       ERROR_OUT_OF_RANGE );
        
        STL_TRY_THROW( dtfNumericMultiplication( sValue2,
                                                 & sTempNumericValue,
                                                 & sNumericValue,
                                                 sErrorStack )
                       == STL_SUCCESS,
                       ERROR_OUT_OF_RANGE );

        /**
         * NUMERIC * DTL_USECS_PER_DAY 를 integer 로 변경.
         */

        STL_TRY_THROW( dtfNumericRounding( & sNumericValue,
                                           0,
                                           & sTempNumericValue,
                                           sErrorStack )
                       == STL_SUCCESS,
                       ERROR_OUT_OF_RANGE );
        
        STL_TRY_THROW( dtlNumericToInt64( & sTempNumericValue,
                                          & sNumericTimestamp,
                                          & sIsTruncated,
                                          sErrorStack )
                       == STL_SUCCESS,
                       ERROR_OUT_OF_RANGE );
    }

    /**
     * TIMESTAMP + NUMERIC
     */
    
    /* TimestampTz -> TimestampType */
    sTimestamp = ((dtlTimestampTzType *)(sValue1->mValue))->mTimestamp -
        (((dtlTimestampTzType *)(sValue1->mValue))->mTimeZone * DTL_USECS_PER_SEC);
    
    sAddTimestamp = sTimestamp - sNumericTimestamp;
    
    /* overflow check */    
    if( ( DTF_DIFFSIGN( sTimestamp, sNumericTimestamp )
          == STL_TRUE ) &&
        ( DTF_DIFFSIGN( sAddTimestamp, sTimestamp  )
          == STL_TRUE ) )
    {
        STL_THROW( ERROR_OUT_OF_RANGE );
    }
    else
    {
        /* Do Nothing */
    }

    /**
     * result value 설정
     */

    /* Timezone을 원상복귀후, result value 설정 */
    ((dtlTimestampTzType *)( sResultValue->mValue ))->mTimestamp =
        sAddTimestamp + (((dtlTimestampTzType *)(sValue1->mValue))->mTimeZone * DTL_USECS_PER_SEC );
    ((dtlTimestampTzType *)( sResultValue->mValue ))->mTimeZone =
        ((dtlTimestampTzType *)(sValue1->mValue ))->mTimeZone;

    sResultValue->mLength = DTL_TIMESTAMPTZ_SIZE;

    /*
     * 연산결과 validate
     */
    
    DTF_DATETIME_VALUE_IS_VALID( sAddTimestamp, sIsValid );
    STL_TRY_THROW( sIsValid == STL_TRUE, ERROR_OUT_OF_RANGE );
    
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        (void)stlPopError( sErrorStack );
        
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATETIME_FIELD_OVERFLOW,
                      NULL,
                      sErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;    
}

/*******************************************************************************
 * timestamp with time zone - interval_year_to_month
 *   timestamp  with time zone - interval_year_to_month => [ P, O, M, S ]
 * ex) timestamptz'2001-09-28 01:00:00 -8:00' with time zone - interval'1-2' year to month
 *     =>  timestamptz'2000-07-28 01:00:00 -8:00'
 *******************************************************************************/

/**
 * @brief timestamp with time zone - interval_year_to_month
 *        <BR> timestamp with time zone - interval_year_to_month, 
 *        <BR>   timestamp with time zone - interval_year_to_month => [ P, O, M, S ]
 *        <BR> ex) timestamptz'2001-09-28 01:00:00 -8:00' with time zone - interval'1-2' year to month
 *        <BR>     =>  timestamptz'2000-07-28 01:00:00 -8:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> TIMESTAMP with time zone - INTERVAL YEAR TO MONTH
 * @param[out] aResultValue      연산 결과 (결과타입 timestamp with time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimestampTzSubIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;

    dtlExpTime           sDtlExpTime;
    dtlFractionalSecond  sFractionalSecond;
    dtlTimestampTzType   sTimestampTzType;
    stlInt32             sTimeZone;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack  = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_INTERVAL_YEAR_TO_MONTH, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, sErrorStack );
    
    sTimestampTzType.mTimestamp = ((dtlTimestampTzType *)(sValue1->mValue))->mTimestamp;
    sTimestampTzType.mTimeZone  = ((dtlTimestampTzType *)(sValue1->mValue))->mTimeZone;
    
    /* dtlTimestampTzType을 dtlExpTime으로 변경 */
    STL_TRY( dtdTimestampTz2dtlExpTime( sTimestampTzType,
                                        & sTimeZone,
                                        & sDtlExpTime,
                                        & sFractionalSecond,
                                        sErrorStack )
             == STL_SUCCESS );
    
    /* Sub 연산 */
    sDtlExpTime.mMonth =
        sDtlExpTime.mMonth - ((dtlIntervalYearToMonthType *)(sValue2->mValue))->mMonth;

    if( sDtlExpTime.mMonth > DTL_MONTHS_PER_YEAR )
    {
        sDtlExpTime.mYear += (sDtlExpTime.mMonth - 1) / DTL_MONTHS_PER_YEAR;
        sDtlExpTime.mMonth = ((sDtlExpTime.mMonth - 1) % DTL_MONTHS_PER_YEAR) + 1;
    }
    else if( sDtlExpTime.mMonth < 1 )
    {
        sDtlExpTime.mYear += (sDtlExpTime.mMonth / DTL_MONTHS_PER_YEAR) - 1;
        sDtlExpTime.mMonth = (sDtlExpTime.mMonth % DTL_MONTHS_PER_YEAR) + DTL_MONTHS_PER_YEAR;
    }
    else
    {
        // Do Nothing
    }

    if( sDtlExpTime.mDay > dtlDayTab[DTL_IS_LEAP(sDtlExpTime.mYear)][sDtlExpTime.mMonth - 1] )
    {
        sDtlExpTime.mDay = dtlDayTab[DTL_IS_LEAP(sDtlExpTime.mYear)][sDtlExpTime.mMonth - 1];
    }
    else
    {
        // Do Nothing
    }

    STL_TRY_THROW( dtdDtlExpTime2TimestampTz(
                       gDataTypeDefinition[DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE].mSqlNormalTypeName,
                       & sDtlExpTime,
                       sFractionalSecond,
                       & sTimeZone,
                       & sTimestampTzType,
                       sErrorStack )
                   == STL_SUCCESS,
                   ERROR_OUT_OF_RANGE );

    *(dtlTimestampTzType *)( sResultValue->mValue ) = sTimestampTzType;
    sResultValue->mLength = DTL_TIMESTAMPTZ_SIZE;
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        (void)stlPopError( sErrorStack );
        
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATETIME_FIELD_OVERFLOW,
                      NULL,
                      sErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/*******************************************************************************
 * timestamp with time zone - interval_day_to_second
 *   timestamp with time zone - interval_day_to_second => [ P, O, M, S ]
 * ex) timestamptz'2001-09-28 01:00:00.111 -8:00' - interval'23'hours  =>  timestamptz'2001-09-27 02:00:00.111 -8:00'
 *******************************************************************************/

/**
 * @brief timestamp with time zone - interval_day_to_second
 *        <BR> timestamp with time zone - interval_day_to_second, 
 *        <BR>   timestamp with time zone - interval_day_to_second => [ P, O, M, S ]
 *        <BR> ex) timestamptz'2001-09-28 01:00:0.123 -8:00' - interval'23' hours
 *        <BR>     =>  timestamptz'2001-09-27 02:00:00.111 -8:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> TIMESTAMP with time zone + INTERVAL DAY TO SECOND
 * @param[out] aResultValue      연산 결과 (결과타입 timestamp with time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimestampTzSubIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aEnv )
{
    dtlDataValue   * sValue1;
    dtlDataValue   * sValue2;
    dtlDataValue   * sResultValue;
    stlErrorStack  * sErrorStack;

    stlBool          sIsValid = STL_FALSE;    
    dtlTimestampType              sTimestampType;
    dtlTimestampType              sTempTimestampType;    
    dtlIntervalDayToSecondType  * sIntervalDayToSecond;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack  = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_INTERVAL_DAY_TO_SECOND, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, sErrorStack );

    /* TimestampTz -> TimestampType */
    sTimestampType = ((dtlTimestampTzType *)(sValue1->mValue))->mTimestamp -
        (((dtlTimestampTzType *)(sValue1->mValue))->mTimeZone * DTL_USECS_PER_SEC);

    /*
     * Interval day to second를 usec 단위로 변경
     */
    sIntervalDayToSecond = ((dtlIntervalDayToSecondType *)( sValue2->mValue)); 

    /*
     * sTimestampType은 julian과 같이 표현되있으므로
     * interval day에 USECS_PER_DAY를 곱해 Day를 julian과 같이 변환해준다.
     * 그리고 Time을 뺀다.
     */
    sTimestampType -= ( sIntervalDayToSecond->mDay * DTL_USECS_PER_DAY ) +
        ( sIntervalDayToSecond->mTime );

    /*
     * 연산결과 validate
     */

    sTempTimestampType = sTimestampType;    
    DTF_DATETIME_VALUE_IS_VALID( sTempTimestampType, sIsValid );
    STL_TRY_THROW( sIsValid == STL_TRUE, ERROR_OUT_OF_RANGE );
    

    /* Timezone을 원상복귀 */
    sTimestampType += (((dtlTimestampTzType *)(sValue1->mValue))->mTimeZone * DTL_USECS_PER_SEC);

    ((dtlTimestampTzType *)( sResultValue->mValue))->mTimestamp = sTimestampType;
    ((dtlTimestampTzType *)( sResultValue->mValue))->mTimeZone =
        ((dtlTimestampTzType *)(sValue1->mValue))->mTimeZone;

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


/*******************************************************************************
 * subtraction ( - ) : interval year to month - interval year to month
 *   interval - interval => [ P, S, O ]
 * ex) interval'1'year - interval'1'month  =>  interval'0-11'year to month
 *******************************************************************************/

/**
 * @brief interval year to month - interval year to month
 *        <BR> subtraction ( - ) : interval year to month - interval year to month
 *        <BR>   interval year to month - interval year to month => [ P, S, O ]
 *        <BR> ex) interval'1'year - interval'1'month  =>  interval'0-11'year to month
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> INTERVAL YEAR TO MONTH - INTERVAL YEAR TO MONTH
 * @param[out] aResultValue      연산 결과 (결과타입 INTERVAL YEAR TO MONTH)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfIntervalYearToMonthSubIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                                        dtlValueEntry  * aInputArgument,
                                                        void           * aResultValue,
                                                        void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;

    dtlIntervalYearToMonthType  sIntervalYearToMonth;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack  = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_INTERVAL_YEAR_TO_MONTH, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_INTERVAL_YEAR_TO_MONTH, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_INTERVAL_YEAR_TO_MONTH, sErrorStack );

    /*
     * -
     * dtlIntervalYearToMonthType - dtlIntervalYearToMonthType 의 결과 indicator는
     * dtlIntervalYearToMonthType + dtlIntervalYearToMonthType 의 결과 indicator는  
     * 두개의 argument indicator를 포함할 수 있는 indicator이어야 한다.
     */
    sIntervalYearToMonth.mIndicator =
        dtlIntervalIndicatorContainArgIndicator
        [((dtlIntervalYearToMonthType *)sValue1->mValue)->mIndicator]
        [((dtlIntervalYearToMonthType *)sValue2->mValue)->mIndicator];
    sIntervalYearToMonth.mMonth = ((dtlIntervalYearToMonthType *)sValue1->mValue)->mMonth
        - ((dtlIntervalYearToMonthType *)sValue2->mValue)->mMonth;

    /* 결과 타입의 precision 체크 */
    STL_DASSERT( (sIntervalYearToMonth.mIndicator == DTL_INTERVAL_INDICATOR_YEAR) ||
                 (sIntervalYearToMonth.mIndicator == DTL_INTERVAL_INDICATOR_MONTH) ||
                 (sIntervalYearToMonth.mIndicator == DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH) );
    STL_TRY( dtdAdjustIntervalYearToMonth( & sIntervalYearToMonth,
                                           sIntervalYearToMonth.mIndicator,
                                           DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION,
                                           sErrorStack )
             == STL_SUCCESS );

    ((dtlIntervalYearToMonthType *)sResultValue->mValue)->mIndicator
        = sIntervalYearToMonth.mIndicator;    
    ((dtlIntervalYearToMonthType *)sResultValue->mValue)->mMonth = sIntervalYearToMonth.mMonth;
    sResultValue->mLength = DTL_INTERVAL_YEAR_TO_MONTH_SIZE;    
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*******************************************************************************
 * subtraction ( - ) : interval day to second - interval day to second
 *   interval day to second - interval day to second => [ P, S, O ]
 * ex) interval'1'day - interval'1'hour  =>  interval'+0 23:00:00'day to second
 *******************************************************************************/

/**
 * @brief interval day to second - interval day to second
 *        <BR> subtraction ( - ) : interval day to second - interval day to second
 *        <BR>   interval day to second - interval day to second => [ P, S, O ]
 *        <BR> ex) interval'1'day - interval'1'hour  =>  interval'+0 23:00:00'day to second
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> INTERVAL DAY TO SECOND - INTERVAL DAY TO SECOND
 * @param[out] aResultValue      연산 결과 (결과타입 INTERVAL DAY TO SECOND)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfIntervalDayToSecondSubIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                                        dtlValueEntry  * aInputArgument,
                                                        void           * aResultValue,
                                                        void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;

    dtlTimeOffset     sOffset1;
    dtlTimeOffset     sOffset2;
    dtlIntervalDayToSecondType  sIntervalDayToSecond;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack  = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_INTERVAL_DAY_TO_SECOND, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_INTERVAL_DAY_TO_SECOND, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_INTERVAL_DAY_TO_SECOND, sErrorStack );

    /*
     * dtlIntervalDayToSecond를 dtlTimeOffset(microsecond)으로 변환 후 Add 연산 수행.
     */
    sOffset1 = (((dtlIntervalDayToSecondType *)sValue1->mValue)->mDay * DTL_USECS_PER_DAY)
        + ((dtlIntervalDayToSecondType *)sValue1->mValue)->mTime;
    sOffset2 = (((dtlIntervalDayToSecondType *)sValue2->mValue)->mDay * DTL_USECS_PER_DAY)
        + ((dtlIntervalDayToSecondType *)sValue2->mValue)->mTime;

    sOffset1 -= sOffset2;

    /*
     * dtlIntervalDayToSecond + dtlIntervalDayToSecond 의 결과 indicator는
     * 두개의 argument indicator를 포함할 수 있는 indicator이어야 한다.
     */
    sIntervalDayToSecond.mIndicator =
        dtlIntervalIndicatorContainArgIndicator
        [((dtlIntervalDayToSecondType *)sValue1->mValue)->mIndicator]
        [((dtlIntervalDayToSecondType *)sValue2->mValue)->mIndicator];
    sIntervalDayToSecond.mDay = sOffset1 / DTL_USECS_PER_DAY;
    sIntervalDayToSecond.mTime = sOffset1 % DTL_USECS_PER_DAY;    

    /* 결과 타입의 precision 체크 */
    STL_DASSERT( (sIntervalDayToSecond.mIndicator == DTL_INTERVAL_INDICATOR_DAY) ||
                 (sIntervalDayToSecond.mIndicator == DTL_INTERVAL_INDICATOR_HOUR) ||
                 (sIntervalDayToSecond.mIndicator == DTL_INTERVAL_INDICATOR_MINUTE) ||
                 (sIntervalDayToSecond.mIndicator == DTL_INTERVAL_INDICATOR_SECOND) ||
                 (sIntervalDayToSecond.mIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_HOUR) ||
                 (sIntervalDayToSecond.mIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE) ||
                 (sIntervalDayToSecond.mIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_SECOND) ||
                 (sIntervalDayToSecond.mIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE) ||
                 (sIntervalDayToSecond.mIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND) ||
                 (sIntervalDayToSecond.mIndicator == DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND) );
    STL_TRY( dtdAdjustIntervalDayToSecond( & sIntervalDayToSecond,
                                           sIntervalDayToSecond.mIndicator,
                                           DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION,
                                           DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION,
                                           sErrorStack )
             == STL_SUCCESS );

    ((dtlIntervalDayToSecondType *)sResultValue->mValue)->mIndicator
        = sIntervalDayToSecond.mIndicator;
    ((dtlIntervalDayToSecondType *)sResultValue->mValue)->mDay = sIntervalDayToSecond.mDay;
    ((dtlIntervalDayToSecondType *)sResultValue->mValue)->mTime = sIntervalDayToSecond.mTime;
    sResultValue->mLength = DTL_INTERVAL_DAY_TO_SECOND_SIZE;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*******************************************************************************
 * subtraction ( - ) : timestamp without time zone - timestamp without time zone
 *   timestamp without time zone - timestamp without time zone => [ P, S, O ]
 * ex) timestamp'2001-09-29 03:00' - timestamp'2001-09-27 12:00'  =>  interval'1 day 15:00:00'
 *******************************************************************************/

/**
 * @brief timestamp without time zone - timestamp without time zone
 *        <BR> subtraction ( - ) : timestamp without time zone - timestamp without time zone
 *        <BR>   timestamp without time zone - timestamp without time zone => [ P, S, O ]
 *        <BR> ex) timestamp'2001-09-29 03:00' - timestamp'2001-09-27 12:00'  =>  interval'1 15:00:00'day to second
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> TIMESTAMP without time zone - TIMESTAMP without time zone
 * @param[out] aResultValue      연산 결과 (결과타입 INTERVAL DAY TO SECOND)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimestampSubTimestamp( stlUInt16        aInputArgumentCnt,
                                    dtlValueEntry  * aInputArgument,
                                    void           * aResultValue,
                                    void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;

    dtlTimestampType           sTimestampType;
    dtlIntervalDayToSecondType sIntervalDayToSecond;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack  = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_TIMESTAMP, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_TIMESTAMP, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_INTERVAL_DAY_TO_SECOND, sErrorStack );

    /* - 연산 수행 */
    sTimestampType = DTF_TIMESTAMP(sValue1) - DTF_TIMESTAMP(sValue2);

    /* interval day to second type 결과 타입 생성 */
    sIntervalDayToSecond.mIndicator = DTL_INTERVAL_INDICATOR_DAY_TO_SECOND;
    sIntervalDayToSecond.mDay = sTimestampType / DTL_USECS_PER_DAY;
    sIntervalDayToSecond.mTime = sTimestampType % DTL_USECS_PER_DAY;    
    
    /* 결과 타입의 precision 체크 */
    STL_TRY( dtdAdjustIntervalDayToSecond( & sIntervalDayToSecond,
                                           sIntervalDayToSecond.mIndicator,
                                           DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION,
                                           DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION,
                                           sErrorStack )
             == STL_SUCCESS );

    ((dtlIntervalDayToSecondType *)sResultValue->mValue)->mIndicator
        = sIntervalDayToSecond.mIndicator;
    ((dtlIntervalDayToSecondType *)sResultValue->mValue)->mDay = sIntervalDayToSecond.mDay;
    ((dtlIntervalDayToSecondType *)sResultValue->mValue)->mTime = sIntervalDayToSecond.mTime;
    sResultValue->mLength = DTL_INTERVAL_DAY_TO_SECOND_SIZE;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
