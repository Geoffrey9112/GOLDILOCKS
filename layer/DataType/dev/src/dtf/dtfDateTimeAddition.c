/*******************************************************************************
 * dtfDateTimeAddition.c
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
 * @file dtfDateTimeAddition.c
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

/*************************************************************************************************
 *   
 *   #################################################################
 *   # 1. DATETIME 산술연산 ( 표준구문 )
 *   #################################################################
 *   
 *   --###############
 *   --# 표준
 *   --###############
 *   
 *   <datetime value expression> ::=
 *   <datetime term>
 *   | <interval value expression> <plus sign> <datetime term>
 *   | <datetime value expression> <plus sign> <interval term>
 *   | <datetime value expression> <minus sign> <interval term>
 *  
 *   Syntax Rules
 *   5) If the <datetime value expression> immediately contains either <plus sign> or <minus sign>,
 *      then:
 *         a) The <interval value expression> or <interval term>
 *            shall contain only <primary datetime field>s that
 *            are contained within the <datetime value expression> or <datetime term>.
 *
 *    그렇다면, 
 *    DATE +(-) INTERVAL YEAR TO MONTH   => DATE
 *    DATE +(-) INTERVAL DAY             => DATE
 *    DATE +(-) INTERVAL HOUR TO SECOND  => ???????
 *    
 *    TIME +(-) INTERVAL YEAR TO MONTH   => ???????
 *    TIME +(-) INTERVAL DAY             => ???????
 *    TIME +(-) INTERVAL HOUR TO SECOND  => TIME
 *    
 *    TIMESTAMP +(-) INTERVAL YEAR TO MONTH   => TIMESTAMP
 *    TIMESTAMP +(-) INTERVAL DAY             => TIMESTAMP
 *    TIMESTAMP +(-) INTERVAL HOUR TO SECOND  => TIMESTAMP
 *    
 *     
 *    --###############
 *    --# Gliese
 *    --###############
 *
 *    Gliese의 datetime 산술연산 결과타입은 아래와 같이 정의한다.
 *     
 *    DATE +(-) INTERVAL YEAR TO MONTH   => DATE
 *    DATE +(-) INTERVAL DAY             => DATE
 *    DATE +(-) INTERVAL HOUR TO SECOND  => TIMESTAMP
 * 
 *    TIME +(-) INTERVAL YEAR TO MONTH   => TIME
 *    TIME +(-) INTERVAL DAY             => TIME
 *    TIME +(-) INTERVAL HOUR TO SECOND  => TIME
 * 
 *    TIMESTAMP +(-) INTERVAL YEAR TO MONTH   => TIMESTAMP
 *    TIMESTAMP +(-) INTERVAL DAY             => TIMESTAMP
 *    TIMESTAMP +(-) INTERVAL HOUR TO SECOND  => TIMESTAMP
 *
 *    
 *   #################################################################
 *   # 2. DATETIME 산술연산 ( 비표준구문 )
 *   #################################################################
 *
 *    DATE +(-) NUMERIC  => DATE
 *             ^^^^^^^^^
 *             ( 일/시/분/초 ) 
 *    
 *************************************************************************************************/

/*******************************************************************************
 * addition ( + ) : date + integer, ADDDATE( date, integer ), DATEADD( date, integer )
 *   date + integer        => [ P, S(?), O ] 
 *   ADDDATE( date, days ) => [ M ]
 * ex) date '2001-09-28' + integer '7'  =>  Timestamp '2001-10-05 00:00:00'
 *******************************************************************************/

/**
 * @brief date + integer
 *        <BR> addition ( + ) : date + integer, ADDDATE( date, integer ), DATEADD( date, integer )
 *        <BR>   date + integer        => [ P, S(?), O ] 
 *        <BR>   ADDDATE( date, days ) => [ M ]
 *        <BR> ex) date '2001-09-28' + integer '7'  =>  Timestamp '2001-10-05 00:00:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> DATE + INTEGER
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 DATE)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfDateAddInteger( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aEnv )
{
    dtlDataValue   * sValue1;
    dtlDataValue   * sValue2;
    dtlDataValue   * sResultValue;
    stlErrorStack  * sErrorStack;
    
    dtlDateType      sDate = 0;
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
    
    /* Addition 연산을 한다. */
    sDate = DTF_DATE( sValue1 ) + sIntegerDate;
    
    /* overflow check */    
    if( ( DTF_SAMESIGN( DTF_DATE( sValue1 ), sIntegerDate )
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

/*******************************************************************************
 * addition ( + ) : integer + date 
 *   date + integer        => [ P, S(?), O ] 
 *   ADDDATE( date, days ) => [ M ]
 * ex) integer '7' + date '2001-09-28' =>  timestamp '2001-10-05 00:00:00'
 *******************************************************************************/

/**
 * @brief integer + date
 *        <BR> addition ( + ) : integer + date
 *        <BR>   integer + dates       => [ P, S(?), O ] 
 *        <BR> ex) integer '7' + date '2001-09-28' =>  Timesamp '2001-10-05 00:00:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> INTEGER + DATE
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 DATE)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfIntegerAddDate( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aEnv )
{
    dtlDataValue   * sValue1;
    dtlDataValue   * sValue2;
    dtlDataValue   * sResultValue;
    stlErrorStack  * sErrorStack;

    dtlDateType      sDate = 0;
    dtlDateType      sIntegerDate = 0;
    stlBool          sIsValid = STL_FALSE;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_INTEGER, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_DATE, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_DATE, sErrorStack );

    STL_TRY_THROW( ( DTF_INTEGER( sValue1 ) <= DTL_JULIAN_MAXJULDAY ) &&
                   ( DTF_INTEGER( sValue1 ) >= -DTL_JULIAN_MAXJULDAY ),
                   ERROR_OUT_OF_RANGE );

    /* integer 값을 microsecond 단위로 변환한다. */
    sIntegerDate = DTF_INTEGER( sValue1 ) * DTL_USECS_PER_DAY;
    
    /* Addition 연산을 한다. */
    sDate = sIntegerDate + DTF_DATE( sValue2 );

    /* overflow check */    
    if( ( DTF_SAMESIGN( sIntegerDate, DTF_DATE( sValue2 ) )
          == STL_TRUE ) &&
        ( DTF_DIFFSIGN( sDate, sIntegerDate )
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
 * @brief date + numeric
 *        <BR> addition ( + ) : date + numeric, ADDDATE( date, numeric ), DATEADD( date, numeric )
 *        <BR>   date + numeric        => [ P, S(?), O ] 
 *        <BR>   ADDDATE( date, numeric ) => [ M ]
 *        <BR> ex) date '2001-09-28' + numeric '7'  =>  timestamp '2001-10-05 00:00:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> DATE + NUMERIC
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 DATE)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfDateAddNumeric( stlUInt16        aInputArgumentCnt,
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
     * DATE + NUMERIC
     */
    
    sDate = DTF_DATE( sValue1 ) + sNumericDate;

    STL_TRY( dtdAdjustDate( & sDate,
                            sErrorStack )
             == STL_SUCCESS );
    
    /* overflow check */
    if( ( DTF_SAMESIGN( DTF_DATE( sValue1 ), sNumericDate )
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

/**
 * @brief numeric + date
 *        <BR> addition ( + ) : numeric + date
 *        <BR>   numeric + date       => [ P, S(?), O ] 
 *        <BR>   ADDDATE( date, numeric ) => [ M ]
 *        <BR> ex) numeric '7' + date '2001-09-28' =>  Timestamp '2001-10-05 00:00:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> NUMERIC + DATE
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 DATE)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfNumericAddDate( stlUInt16        aInputArgumentCnt,
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
    stlBool          sIsValid = STL_FALSE;
    
    stlBool          sIsTruncated;

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

    DTF_CHECK_DTL_VALUE2( sValue1, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_DATE, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_DATE, sErrorStack );

    sNumericValue.mType = DTL_TYPE_NUMBER;
    sNumericValue.mBufferSize = sNumericAlignMaxSize;
    sNumericValue.mValue = (dtlNumericType *)(&sNumeric);

    sTempNumericValue.mType = DTL_TYPE_NUMBER;
    sTempNumericValue.mBufferSize = sNumericAlignMaxSize;
    sTempNumericValue.mValue = (dtlNumericType *)(&sTempNumeric);

    if( DTL_NUMERIC_IS_ZERO( (dtlNumericType *)(sValue1->mValue), sValue1->mLength ) )
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
        
        STL_TRY_THROW( dtfNumericMultiplication( sValue1,
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
     * DATE + NUMERIC
     */
    
    sDate = sNumericDate + DTF_DATE( sValue2 );
    
    STL_TRY( dtdAdjustDate( & sDate,
                            sErrorStack )
             == STL_SUCCESS );
    
    /* overflow check */    
    if( ( DTF_SAMESIGN( sNumericDate, DTF_DATE( sValue2 ) )
          == STL_TRUE ) &&
        ( DTF_DIFFSIGN( sDate, sNumericDate )
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
 * addition ( + ) : date + interval, ADDDATE( date, INTERVAL expr ), DATEADD(date, INTERVAL expr)
 *   date + interval                 => [ P, S, M, O ]
 *   ADDDATE( date, INTERVAL expr )  => [ M ]
 *   DATE_ADD( date, INTERVAL expr ) => [ M ]
 *   M, O사 interval literal 예) interval'1'month
 * ex) date'2001-09-28' + interval'1'month  =>  date'2001-10-28'
 *******************************************************************************/

/**
 * @brief date + interval_year_to_month
 *        <BR> addition ( + )
 *        <BR> date + interval, ADDDATE( date, INTERVAL expr ), DATEADD(date, INTERVAL expr)
 *        <BR>   date + interval                 => [ P, S, M, O ]
 *        <BR>   ADDDATE( date, INTERVAL expr )  => [ M ]
 *        <BR>   DATE_ADD( date, INTERVAL expr ) => [ M ]
 *        <BR>   M, O사 interval literal 예) interval'1'hour
 *        <BR> ex) date'2001-09-28' + interval'1 hour'  =>  date'2001-09-28'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> DATE + INTERVAL YEAR TO MONTH
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 DATE )
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfDateAddIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
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
    
    /*
     * Addition 연산을 한다.
     */
    sDtlExpTime.mMonth =
        sDtlExpTime.mMonth + ((dtlIntervalYearToMonthType *)(sValue2->mValue))->mMonth;
    
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
 * addition ( + ) : interval + date
 *   interval + date                 => [ P, S, M, O ]
 *   M, O사 interval literal 예) interval'1'month
 * ex) interval'1'month + date'2001-09-28'  =>  date'2001-10-28'
 *******************************************************************************/

/**
 * @brief interval_year_to_month + date
 *        <BR> addition ( + )
 *        <BR> interval + date
 *        <BR>   interval + date                  => [ P, S, M, O ]
 *        <BR>   M, O사 interval literal 예) interval'1'hour
 *        <BR> ex) interval'1 hour' + date'2001-09-28'  =>  date'2001-09-28'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> INTERVAL YEAR TO MONTH + DATE
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 DATE )
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfIntervalYearToMonthAddDate( stlUInt16        aInputArgumentCnt,
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

    
    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_INTERVAL_YEAR_TO_MONTH, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_DATE, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_DATE, sErrorStack );

    /* dtlDateType을 dtlExpTime으로 변경. */
    DTL_INIT_DTLEXPTIME( sDtlExpTime );

    sDateType = DTF_DATE(sValue2);
    
    STL_TRY( dtdDateTime2dtlExpTime( sDateType,
                                     & sDtlExpTime,
                                     & sFractionalSecond,
                                     sErrorStack )
             == STL_SUCCESS );

    STL_DASSERT( sFractionalSecond == 0 );
    
    /* Addition 연산을 한다. */    
    sDtlExpTime.mMonth =
        sDtlExpTime.mMonth + ((dtlIntervalYearToMonthType *)(sValue1->mValue))->mMonth;
    
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
 * addition ( + ) : date + interval, ADDDATE( date, INTERVAL expr ), DATEADD(date, INTERVAL expr)
 *   date + interval                 => [ P, S, M, O ]
 *   ADDDATE( date, INTERVAL expr )  => [ M ]
 *   DATE_ADD( date, INTERVAL expr ) => [ M ]
 *   M, O사 interval literal 예) interval'1'hour
 * ex) date'2001-09-28' + interval'1' day  =>  date'2001-09-29'
 *******************************************************************************/

/**
 * @brief date + interval_day
 *        <BR> addition ( + )
 *        <BR> date + interval_day, ADDDATE( date, INTERVAL day ), DATEADD(date, INTERVAL day)
 *        <BR>   date + interval_day                 => [ P, S, M, O ]
 *        <BR>   ADDDATE( date, INTERVAL day )  => [ M ]
 *        <BR>   DATE_ADD( date, INTERVAL day ) => [ M ]
 *        <BR>   M, O사 interval literal 예) interval'1'day
 *        <BR> ex) date'2001-09-28' + interval'1' day  =>  date'2001-09-29'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> DATE + INTERVAL DAY TO SECOND ( interval day )
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 DATE)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfDateAddIntervalDay( stlUInt16        aInputArgumentCnt,
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
     * (1) dtlDateType에 dtlIntervalDayToSecond.mDay를 더한다.
     */
    sIntervalDayToSecond = ((dtlIntervalDayToSecondType *)(sValue2->mValue));

    /* DTL_INTERVAL_INDICATOR_DAY 만 이 함수로 올 수 있다. */
    STL_DASSERT( sIntervalDayToSecond->mIndicator == DTL_INTERVAL_INDICATOR_DAY );
    STL_DASSERT( sIntervalDayToSecond->mTime == 0 );

    /* interval day 값을 microsecond 단위로 변환한다. */
    sIntervalDayDate = sIntervalDayToSecond->mDay * DTL_USECS_PER_DAY;
    
    /*
     * dtlDateType + dtlIntervalDayToSecond.mDay
     * 
     * overflow가 발생하지 않는다.
     * 예) '1999-12-31 23:59:59' + interval'999999'day(6) 과 같이
     *     date와 interval day(6) 의 최대값을 더해도 stlInt64 범위에 수용됨.
     */
    sDateType = DTF_DATE(sValue1) + sIntervalDayDate;

    DTF_DATE( sResultValue ) = sDateType;
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
 * @brief date + interval_day_to_second
 *        <BR> addition ( + )
 *        <BR> date + interval, ADDDATE( date, INTERVAL expr ), DATEADD(date, INTERVAL expr)
 *        <BR>   date + interval                 => [ P, S, M, O ]
 *        <BR>   ADDDATE( date, INTERVAL expr )  => [ M ]
 *        <BR>   DATE_ADD( date, INTERVAL expr ) => [ M ]
 *        <BR>   M, O사 interval literal 예) interval'1'hour
 *        <BR> ex) date'2001-09-28' + interval'1'hour  =>  timestamp'2001-09-28 01:00:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> DATE + INTERVAL DAY TO SECOND
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 TIMESTAMP)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfDateAddIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
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
     * dtlDateType + dtlIntervalDayToSecond
     * 
     * +의 overflow가 발생하지 않는다.
     * 예) '1999-12-31 23:59:59' + interval'999999 23:59:59.999999'day(6) to second 과 같이
     *     date와 interval day(6) to second 의 최대값을 더해도 stlInt64 범위에 수용됨.
     */     
    
    sTimestampType = DTF_DATE(sValue1);
    sTimestampType += ( sIntervalDayToSecond->mDay * DTL_USECS_PER_DAY ) + sIntervalDayToSecond->mTime;

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
 * addition ( + ) : interval + date
 *   interval + date                 => [ P, S, M, O ]
 *   M, O사 interval literal 예) interval'1'hour
 * ex) interval'1' day + date'2001-09-28'  =>  date '2001-09-29'
 *******************************************************************************/

/**
 * @brief interval_day + date 
 *        <BR> addition ( + )
 *        <BR> interval + date
 *        <BR>   interval + date                 => [ P, S, M, O ]
 *        <BR>   M, O사 interval literal 예) interval'1'day
 *        <BR> ex) interval'1' day + date'2001-09-28'  =>  date '2001-09-29'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> INTERVAL DAY TO SECOND ( interval day ) + DATE
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 DATE)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfIntervalDayAddDate( stlUInt16        aInputArgumentCnt,
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

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_INTERVAL_DAY_TO_SECOND, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_DATE, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_DATE, sErrorStack );

    /*
     * (1) dtlDateType에 dtlIntervalDayToSecond.mDay를 더한다.
     */
    sIntervalDayToSecond = ((dtlIntervalDayToSecondType *)(sValue1->mValue));

    /* DTL_INTERVAL_INDICATOR_DAY 만 이 함수로 올 수 있다. */
    STL_DASSERT( sIntervalDayToSecond->mIndicator == DTL_INTERVAL_INDICATOR_DAY );
    STL_DASSERT( sIntervalDayToSecond->mTime == 0 );

    /* interval day 값을 microsecond 단위로 변환한다. */
    sIntervalDayDate = sIntervalDayToSecond->mDay * DTL_USECS_PER_DAY;
    
    /*
     * dtlDateType + dtlIntervalDayToSecond.mDay
     * 
     * overflow가 발생하지 않는다.
     * 예) '1999-12-31 23:59:59' + interval'999999'day(6) 과 같이
     *     date와 interval day(6) 의 최대값을 더해도 stlInt64 범위에 수용됨.
     */
    sDateType = DTF_DATE(sValue2) + sIntervalDayDate;

    DTF_DATE( sResultValue ) = sDateType;
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
 * @brief interval_day_to_second + date 
 *        <BR> addition ( + )
 *        <BR> interval + date
 *        <BR>   interval + date                 => [ P, S, M, O ]
 *        <BR>   M, O사 interval literal 예) interval'1'hour
 *        <BR> ex) interval'1'hour  + date'2001-09-28'  =>  date '2001-09-28 01:00:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> INTERVAL DAY TO SECOND + DATE
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 TIMESTAMP)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfIntervalDayToSecondAddDate( stlUInt16        aInputArgumentCnt,
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

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_INTERVAL_DAY_TO_SECOND, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_DATE, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIMESTAMP, sErrorStack );

    /*
     * (1) dtlDateType에 dtlIntervalDayToSecond.mDay를 더한다.
     */
    sIntervalDayToSecond = ((dtlIntervalDayToSecondType *)(sValue1->mValue));

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
     * dtlDateType + dtlIntervalDayToSecond
     * 
     * +의 overflow가 발생하지 않는다.
     * 예) '1999-12-31 23:59:59' + interval'999999 23:59:59.999999'day(6) to second 과 같이
     *     date와 interval day(6) to second 의 최대값을 더해도 stlInt64 범위에 수용됨.
     */     

    sTimestampType = DTF_DATE(sValue2) +
        ( sIntervalDayToSecond->mDay * DTL_USECS_PER_DAY ) + sIntervalDayToSecond->mTime;    
    
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
 * interval_year_to_month + interval_year_to_month
 *   interval + interval => [ P, S, O ]
 *   O사의 interval literal 예)  interval'1'day
 * ex) interval'1'year + interval'1'year  =>  interval'2'year
 *******************************************************************************/

/**
 * @brief interval_year_to_month + interval_year_to_month
 *        <BR> interval_year_to_month + interval_year_to_month
 *        <BR>   interval + interval => [ P, S, O ]
 *        <BR>   O사의 interval literal 예)  interval'1'year
 *        <BR> ex) interval'1'year + interval'1'year  =>  interval'2'year
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> INTERVAL_YEAR_TO_MONTH + INTERVAL_YEAR_TO_MONTH
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 INTERVAL_YEAR_TO_MONTH)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfIntervalYearToMonthAddIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
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
     * Add
     * dtlIntervalYearToMonthType + dtlIntervalYearToMonthType 의 결과 indicator는  
     * 두개의 argument indicator를 포함할 수 있는 indicator이어야 한다.
     */
    sIntervalYearToMonth.mIndicator =
        dtlIntervalIndicatorContainArgIndicator
        [((dtlIntervalYearToMonthType *)sValue1->mValue)->mIndicator]
        [((dtlIntervalYearToMonthType *)sValue2->mValue)->mIndicator];
    sIntervalYearToMonth.mMonth = ((dtlIntervalYearToMonthType *)sValue1->mValue)->mMonth
        + ((dtlIntervalYearToMonthType *)sValue2->mValue)->mMonth;

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
 * interval_day_to_second + interval_day_to_second
 *   interval + interval => [ P, S, O ]
 *   O사의 interval literal 예)  interval'1'day
 * ex) interval'1'day + interval'1'hour  =>  interval'1 01:00:00'day to second
 *******************************************************************************/
/**
 * @brief interval_day_to_second + interval_day_to_second
 *        <BR> interval + interval
 *        <BR>   interval + interval => [ P, S, O ]
 *        <BR>   O사의 interval literal 예)  interval'1'day
 *        <BR> ex) interval'1'day + interval'1'hour  =>  interval'1 01:00:00'day to second
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> INTERVAL_DAY_TO_SECOND + INTERVAL_DAY_TO_SECOND
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 INTERVAL_DAY_TO_SECOND)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfIntervalDayToSecondAddIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
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

    sOffset1 += sOffset2;

    /*
     * Add
     * dtlIntervalYearToMonthType + dtlIntervalYearToMonthType 의 결과 indicator는  
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
 * timestamp + integer,  ADDDATE( date, days ), DATEADD(date, days )
 *   timestamp + integer => [ O, M, P ]
 * ex) timestamp'2001-09-28 01:00' + integer '2'  =>  timestamp'2001-09-30 01:00:00'
 *******************************************************************************/

/**
 * @brief timestamp without time zone + integer
 *        <BR> timestamp without time zone + integer, 
 *        <BR>   timestamp without time zone + integer => [ O, M, P]
 *        <BR> ex) timestamp'2001-09-28 01:00' + integer '2'  =>  timestamp'2001-09-30 01:00:00' [ O ]
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> TIMESTAMP without time zone + INTEGER
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 timestamp without time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimestampAddInteger( stlUInt16        aInputArgumentCnt,
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

    sTimestamp = DTF_TIMESTAMP( sValue1 ) + sIntegerTimestamp;

    /* overflow check */
    if( ( DTF_SAMESIGN( DTF_TIMESTAMP( sValue1 ), sIntegerTimestamp )
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

/**************************************************************************************
 * addition ( + ) : integer + timestamp 
 *   integer + timestamp        => [ O, M ,P] 
 * ex) integer '7' + timestamp '2001-09-28 01:02:03' =>  Timestamp '2001-10-05 01:02:03'
 **************************************************************************************/

/**
 * @brief integer + timestamp without time zone
 *        <BR> addition ( + ) : integer + timestamp without time zone
 *        <BR>   integer + timestamp without time zone     => [ O, M, P ] 
 *        <BR> ex) integer '7' + timestamp '2001-09-28 01:02:02' =>  Timestamp '2001-10-05 01:02:03' [ O ]
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> INTEGER + TIMESTAMP without time zone
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 timestamp without time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfIntegerAddTimestamp( stlUInt16        aInputArgumentCnt,
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
    stlBool            sIsValid = STL_FALSE;    

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_INTEGER, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_TIMESTAMP, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIMESTAMP, sErrorStack );

    STL_TRY_THROW( ( DTF_INTEGER( sValue1 ) <= DTL_JULIAN_MAXJULDAY ) &&
                   ( DTF_INTEGER( sValue1 ) >= -DTL_JULIAN_MAXJULDAY ),
                   ERROR_OUT_OF_RANGE );

    sIntegerTimestamp = DTF_INTEGER( sValue1 ) * DTL_USECS_PER_DAY;

    sTimestamp = DTF_TIMESTAMP( sValue2 ) + sIntegerTimestamp;

    /* overflow check */
    if( ( DTF_SAMESIGN( DTF_TIMESTAMP( sValue2 ), sIntegerTimestamp )
          == STL_TRUE ) &&
        ( DTF_DIFFSIGN( sTimestamp, DTF_TIMESTAMP( sValue2 ) )
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


/**************************************************************************************
 * addition ( + ) : numeric + timestamp 
 *    numeric + timestamp        => [ O, M ] 
 * ex) numeric '7' + timestamp '2001-09-28 01:02:03' =>  Timestamp '2001-10-05 01:02:03'
 **************************************************************************************/

/**
 * @brief numeric + timestamp without time zone
 *        <BR> addition ( + ) : numeric + timestamp without time zone
 *        <BR>   numeric + timestamp without time zone   => [ O, M ] 
 *        <BR> ex) numeric '7' + timestamp '2001-09-28 01:02:02' =>  Timestamp '2001-10-05 01:02:03'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> NUMERIC + TIMESTAMP without time zone
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 timestamp without time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfNumericAddTimestamp( stlUInt16        aInputArgumentCnt,
                                  dtlValueEntry  * aInputArgument,
                                  void           * aResultValue,
                                  void           * aEnv )
{
    dtlDataValue     * sValue1;
    dtlDataValue     * sValue2;
    dtlDataValue     * sResultValue;
    stlErrorStack    * sErrorStack;

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
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE2( sValue1, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_TIMESTAMP, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIMESTAMP, sErrorStack );

    sNumericValue.mType = DTL_TYPE_NUMBER;
    sNumericValue.mBufferSize = sNumericAlignMaxSize;
    sNumericValue.mValue = (dtlNumericType *)(&sNumeric);

    sTempNumericValue.mType = DTL_TYPE_NUMBER;
    sTempNumericValue.mBufferSize = sNumericAlignMaxSize;
    sTempNumericValue.mValue = (dtlNumericType *)(&sTempNumeric);

    if( DTL_NUMERIC_IS_ZERO( (dtlNumericType *)(sValue1->mValue), sValue1->mLength ) )
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
        
        STL_TRY_THROW( dtfNumericMultiplication( sValue1,
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
     * NUMERIC + TIMESTAMP
     */

    sTimestamp = DTF_TIMESTAMP( sValue2 ) + sNumericTimestamp;

    /* overflow check */    
    if( ( DTF_SAMESIGN( DTF_TIMESTAMP(sValue2), sNumericTimestamp )
          == STL_TRUE ) &&
        ( DTF_DIFFSIGN( sTimestamp, DTF_TIMESTAMP(sValue2)  )
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
 * timestamp + numeric, ADDDATE( date, numeric ), DATEADD(date, numeric )
 *   timestamp + numeric => [ O, M ]
 * ex) timestamp'2001-09-28 01:00' + numeric '2'  =>  timestamp'2001-09-30 01:00:00'
 *******************************************************************************/

/**
 * @brief timestamp without time zone + numeric
 *        <BR> timestamp without time zone + numeric, 
 *        <BR>   timestamp without time zone + numeric => [ O, M ]
 *        <BR> ex) timestamp'2001-09-28 01:00' + numeric '2'  =>  timestamp'2001-09-30 01:00:00' [ O ]
 *       
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> TIMESTAMP without time zone + NUMERIC
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 timestamp without time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimestampAddNumeric( stlUInt16        aInputArgumentCnt,
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
     * TIMESTAMP + NUMERIC
     */

    sTimestamp = DTF_TIMESTAMP( sValue1 ) + sNumericTimestamp;

    /* overflow check */    
    if( ( DTF_SAMESIGN( DTF_TIMESTAMP(sValue1), sNumericTimestamp )
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
 * timestamp + interval_year_to_month, ADDDATE( date, INTERVAL expr ), DATEADD( date, INTERVAL expr )
 *   timestamp + interval_year_to_month => [ P, S, O, M ]
 * ex) timestamp'2001-09-28 01:00:00' + interval'1-2' year to month  =>  timestamp'2002-11-28 01:00:00'
 *******************************************************************************/

/**
 * @brief timestamp without time zone + interval_year_to_month
 *        <BR> timestamp without time zone + interval_year_to_month, 
 *        <BR>   timestamp without time zone + interval_year_to_month => [ P, S, O, M ]
 *        <BR> ex) timestamp'2001-09-28 01:00:00' + interval'1-2' year to month  =>  timestamp'2002-11-28 01:00:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> TIMESTAMP without time zone + INTERVAL YEAR TO MONTH
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 timestamp without time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimestampAddIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
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

    /* Add 연산 */
    sDtlExpTime.mMonth =
        sDtlExpTime.mMonth + ((dtlIntervalYearToMonthType *)(sValue2->mValue))->mMonth;

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
 * interval_year_to_month + timestamp
 *   interval_year_to_month + timestamp  => [ P, S, O, M ]
 * ex) interval'1-2' year to month +timestamp'2001-09-28 01:00:00'  =>  timestamp'2002-11-28 01:00:00'
 *******************************************************************************/

/**
 * @brief interval_year_to_month  + timestamp without time zone
 *        <BR> interval_year_to_month + timestamp without time zone, 
 *        <BR>  interval_year_to_month + timestamp without time zone => [ P, S, O, M ]
 *        <BR> ex) timestamp'2001-09-28 01:00:00' + interval'1-2' year to month  =>  timestamp'2002-11-28 01:00:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> TIMESTAMP without time zone + INTERVAL YEAR TO MONTH
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 timestamp without time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfIntervalYearToMonthAddTimestamp( stlUInt16        aInputArgumentCnt,
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
    
    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_INTERVAL_YEAR_TO_MONTH, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_TIMESTAMP, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIMESTAMP, sErrorStack );

    /* dtlTimestampType을 dtlExpTime으로 변경 */
    DTL_INIT_DTLEXPTIME( sDtlExpTime );
    STL_TRY( dtdDateTime2dtlExpTime( DTF_TIMESTAMP(sValue2),
                                     & sDtlExpTime,
                                     & sFractionalSecond,
                                     sErrorStack )
             == STL_SUCCESS );

    /* Add 연산 */
    sDtlExpTime.mMonth =
        sDtlExpTime.mMonth + ((dtlIntervalYearToMonthType *)(sValue1->mValue))->mMonth;

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
                   ERROR_OUT_OF_RANGE);   

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
 * timestamp + interval_day_to_second, ADDTIME( time, INTERVAL expr )
 *   timestamp + interval_day_to_second => [ P, S, O, M ]
 * ex) timestamp'2001-09-28 01:00' + interval'23'hours  =>  timestamp'2001-09-29 00:00:00'
 *******************************************************************************/

/**
 * @brief timestamp without time zone + interval_day_to_second
 *        <BR> timestamp without time zone + interval_day_to_second, 
 *        <BR>   timestamp without time zone + interval_day_to_second => [ P, S, O, M ]
 *        <BR> ex) timestamp'2001-09-28 01:00' + interval'23' hours  =>  timestamp'2001-09-29 00:00:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> TIMESTAMP without time zone + INTERVAL DAY TO SECOND
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 timestamp without time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimestampAddIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
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
     * 그리고 Time을 더한다.
     */ 
    sTimestampType += ( sIntervalDayToSecond->mDay * DTL_USECS_PER_DAY ) +
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
 * interval_day_to_second + timestamp
 *   interval_day_to_second + timestamp => [ P, S, O, M ]
 * ex) interval'23'hours + timestamp'2001-09-28 01:00'  =>  timestamp'2001-09-29 00:00:00'
 *******************************************************************************/
/**
 * @brief interval_day_to_second + timestamp without time zone
 *        <BR> interval_day_to_second + timestamp without time zone, 
 *        <BR>   interval_day_to_second + timestamp without time zone => [ P, S, O, M ]
 *        <BR> ex) interval'23' hours + timestamp'2001-09-28 01:00'  =>  timestamp'2001-09-29 00:00:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> INTERVAL DAY TO SECOND + TIMESTAMP without time zone
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 timestamp without time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfIntervalDayToSecondAddTimestamp( stlUInt16        aInputArgumentCnt,
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

    
    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_INTERVAL_DAY_TO_SECOND, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_TIMESTAMP, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIMESTAMP, sErrorStack );

    sTimestampType = DTF_TIMESTAMP(sValue2);

    /*
     * Interval day to second를 usec 단위로 변경
     */
    sIntervalDayToSecond = ((dtlIntervalDayToSecondType *)(sValue1->mValue));

    /*
     * sTimestampType은 julian과 같이 표현되있으므로
     * interval day에 USECS_PER_DAY를 곱해 Day를 julian과 같이 변환해준다.
     * 그리고 Time을 더한다.
     */ 
    sTimestampType += ( sIntervalDayToSecond->mDay * DTL_USECS_PER_DAY ) +
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
 * timestamptz + integer,  ADDDATE( date, integer ), DATEADD( date, integer )
 *   timestamptz + integer => [ O, M ]
 * ex) timestamptz'2001-09-28 01:02:03.123 -8:00' + integer '2'  =>  timestamptz'2001-09-30 01:02:03.123 -8:00'
 *******************************************************************************/

/**
 * @brief timestamp with time zone + integer
 *        <BR> timestamp with time zone + integer, 
 *        <BR>   timestamp with time zone + integer => [ O, M ]
 *        <BR> ex) timestamptz'2001-09-28 01:02:03.123 -8:00' + integer '2'  =>  timestamptz'2001-09-30 01:02:03.123 -8:00' [ O ]
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> TIMESTAMP with time zone + INTEGER
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 timestamp with time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimestampTzAddInteger( stlUInt16        aInputArgumentCnt,
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

    sAddTimestamp = sTimestamp + sIntegerTimestamp;

    /* overflow check */
    if( ( DTF_SAMESIGN( sTimestamp, sIntegerTimestamp )
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
 * integer + timestamptz
 *   integer + timestamptz => [ O, M ]
 * ex) integer '2' + timestamptz'2001-09-28 01:02:03.123 -8:00'  =>  timestamptz'2001-09-30 01:02:03.123 -8:00'
 *******************************************************************************/

/**
 * @brief integer + timestamp with time zone
 *        <BR> integer + timestamp with time zone, 
 *        <BR>   integer + timestamp with time zone => [ O, M ]
 *        <BR> ex) integer '2' + timestamptz'2001-09-28 01:02:03.123 -8:00'  =>  timestamptz'2001-09-30 01:02:03.123 -8:00' [ O ]
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> INTEGER + TIMESTAMP with time zone
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 timestamp with time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfIntegerAddTimestampTz( stlUInt16        aInputArgumentCnt,
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

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_INTEGER, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, sErrorStack );

    /* TimestampTz -> TimestampType */
    sTimestamp = ((dtlTimestampTzType *)(sValue2->mValue))->mTimestamp -
        (((dtlTimestampTzType *)(sValue2->mValue))->mTimeZone * DTL_USECS_PER_SEC);

    STL_TRY_THROW( ( DTF_INTEGER( sValue1 ) <= DTL_JULIAN_MAXJULDAY ) &&
                   ( DTF_INTEGER( sValue1 ) >= -DTL_JULIAN_MAXJULDAY ),
                   ERROR_OUT_OF_RANGE );

    sIntegerTimestamp = DTF_INTEGER( sValue1 ) * DTL_USECS_PER_DAY;

    sAddTimestamp = sTimestamp + sIntegerTimestamp;

    /* overflow check */
    if( ( DTF_SAMESIGN( sTimestamp, sIntegerTimestamp )
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
        sAddTimestamp + ( ((dtlTimestampTzType *)(sValue2->mValue))->mTimeZone * DTL_USECS_PER_SEC );
    ((dtlTimestampTzType *)( sResultValue->mValue ))->mTimeZone =
        ((dtlTimestampTzType *)(sValue2->mValue ))->mTimeZone;
    
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
 * timestamptz + numeric,  ADDDATE( date, numeric ), DATEADD( date, numeric )
 *   timestamptz + numeric => [ P, O, M ]
 * ex) timestamptz'2001-09-28 01:02:03.123 -8:00' + numeric '2'  =>  timestamptz'2001-09-30 01:02:03.123 -8:00'
 *******************************************************************************/

/**
 * @brief timestamp with time zone + numeric
 *        <BR> timestamp with time zone + numeric, 
 *        <BR>   timestamp with time zone + numeric => [ P, O, M ]
 *        <BR> ex) timestamptz'2001-09-28 01:02:03.123 -8:00' + numeric '2'  =>  timestamptz'2001-09-30 01:02:03.123 -8:00' [ O ]
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> TIMESTAMP with time zone + NUMERIC
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 timestamp with time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimestampTzAddNumeric( stlUInt16        aInputArgumentCnt,
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
    
    sAddTimestamp = sTimestamp + sNumericTimestamp;
    
    /* overflow check */    
    if( ( DTF_SAMESIGN( sTimestamp, sNumericTimestamp )
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
 * numeric + timestamptz
 *   numeric + timestamptz => [ P, O, M ]
 * ex) numeric '2' + timestamptz'2001-09-28 01:02:03.123 -8:00'  =>  timestamptz'2001-09-30  01:02:03.123 -8:00' 
 *******************************************************************************/

/**
 * @brief numeric + timestamp with time zone
 *        <BR> numeric + timestamp with time zone, 
 *        <BR>   numeric + timestamp with time zone => [ P, O, M ]
 *        <BR> ex) numeric '2' + timestamptz'2001-09-28 01:02:03.123 -8:00'  =>  timestamptz'2001-09-30 01:02:03.123 -8:00' [ O ]
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> NUMERIC + TIMESTAMP with time zone 
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 timestamp with time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfNumericAddTimestampTz( stlUInt16        aInputArgumentCnt,
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

    DTF_CHECK_DTL_VALUE2( sValue1, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, sErrorStack );

    sNumericValue.mType = DTL_TYPE_NUMBER;
    sNumericValue.mBufferSize = sNumericAlignMaxSize;
    sNumericValue.mValue = (dtlNumericType *)(&sNumeric);

    sTempNumericValue.mType = DTL_TYPE_NUMBER;
    sTempNumericValue.mBufferSize = sNumericAlignMaxSize;
    sTempNumericValue.mValue = (dtlNumericType *)(&sTempNumeric);
    
    if( DTL_NUMERIC_IS_ZERO( (dtlNumericType *)(sValue1->mValue), sValue1->mLength ) )
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
        
        STL_TRY_THROW( dtfNumericMultiplication( sValue1,
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
    sTimestamp = ((dtlTimestampTzType *)(sValue2->mValue))->mTimestamp -
        (((dtlTimestampTzType *)(sValue2->mValue))->mTimeZone * DTL_USECS_PER_SEC);
    
    sAddTimestamp = sTimestamp + sNumericTimestamp;
    
    /* overflow check */    
    if( ( DTF_SAMESIGN( sTimestamp, sNumericTimestamp )
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
        sAddTimestamp + (((dtlTimestampTzType *)(sValue2->mValue))->mTimeZone * DTL_USECS_PER_SEC );
    ((dtlTimestampTzType *)( sResultValue->mValue ))->mTimeZone =
        ((dtlTimestampTzType *)(sValue2->mValue ))->mTimeZone;

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
 * timestamp with time zone + interval_year_to_month,  ADDDATE( date, INTERVAL expr ), DATEADD( date, INTERVAL expr )
 *   timestamp  with time zone + interval_year_to_month => [ P, O, M, S ]
 * ex) timestamptz'2001-09-28 01:00:00 -8:00' with time zone + interval'1-2' year to month
 *     =>  timestamptz'2002-11-28 01:00:00 -8:00'
 *******************************************************************************/

/**
 * @brief timestamp with time zone + interval_year_to_month
 *        <BR> timestamp with time zone + interval_year_to_month, 
 *        <BR>   timestamp with time zone + interval_year_to_month => [ P, O, M, S ]
 *        <BR> ex) timestamptz'2001-09-28 01:00:00 -8:00' with time zone + interval'1-2' year to month  =>  timestamptz'2002-11-28 01:00:00 -8:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> TIMESTAMP with time zone + INTERVAL YEAR TO MONTH
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 timestamp with time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimestampTzAddIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
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
    
    /* Add 연산 */
    sDtlExpTime.mMonth =
        sDtlExpTime.mMonth + ((dtlIntervalYearToMonthType *)(sValue2->mValue))->mMonth;

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
 * interval_year_to_month + timestamp with time zone
 *  interval_year_to_month + timestamp  with time zone => [ P, O, M, S ]
 * ex) interval'1-2' year to month + timestamp'2001-09-28 01:00:00 -8:00' with time zone  =>  timestamp'2002-11-28 01:00:00 -8:00'
 *******************************************************************************/

/**
 * @brief interval_year_to_month + timestamp with time zone
 *        <BR> interval_year_to_month + timestamp with time zone, 
 *        <BR>   interval_year_to_month + timestamp with time zone => [ P, O, M, S ]
 *        <BR> ex) interval'1-2' year to month + timestamp'2001-09-28 01:00:00 -8:00' with time zone
 *        <BR>     =>  timestamp'2002-11-28 01:00:00 -8:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> INTERVAL YEAR TO MONTH + TIMESTAMP with time zone
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 timestamp with time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfIntervalYearToMonthAddTimestampTz( stlUInt16        aInputArgumentCnt,
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

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_INTERVAL_YEAR_TO_MONTH, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, sErrorStack );
    
    sTimestampTzType.mTimestamp = ((dtlTimestampTzType *)(sValue2->mValue))->mTimestamp;
    sTimestampTzType.mTimeZone  = ((dtlTimestampTzType *)(sValue2->mValue))->mTimeZone;
    
    /* dtlTimestampTzType을 dtlExpTime으로 변경 */
    STL_TRY( dtdTimestampTz2dtlExpTime( sTimestampTzType,
                                        & sTimeZone,
                                        & sDtlExpTime,
                                        & sFractionalSecond,
                                        sErrorStack )
             == STL_SUCCESS );
    
    /* Add 연산 */
    sDtlExpTime.mMonth =
        sDtlExpTime.mMonth + ((dtlIntervalYearToMonthType *)(sValue1->mValue))->mMonth;

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
 * timestamp with time zone + interval_day_to_second,  ADDTIME( time, INTERVAL expr )
 *   timestamp with time zone + interval_day_to_second => [ P, O, M, S ]
 * ex) timestamptz'2001-09-28 01:00:00.111 -8:00' + interval'23'hours
 *     =>  timestamptz'2001-09-29 01:00:00.111 -8:00'
 *******************************************************************************/

/**
 * @brief timestamp with time zone + interval_day_to_second
 *        <BR> timestamp with time zone + interval_day_to_second, 
 *        <BR>   timestamp with time zone + interval_day_to_second => [ P, O, M, S ]
 *        <BR> ex) timestamptz'2001-09-28 01:00:0.123 -8:00' + interval'23' hours
 *        <BR>     =>  timestamp with time zone'2001-09-29 01:00:00.123 -8:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> TIMESTAMP with time zone + INTERVAL DAY TO SECOND
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 timestamp with time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimestampTzAddIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
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
     * 그리고 Time을 더한다.
     */
    sTimestampType += ( sIntervalDayToSecond->mDay * DTL_USECS_PER_DAY ) +
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
 * interval_day_to_second + timestamp with time zone
 *   interval_day_to_second + timestamp with time zone => [ P, O, M, S ]
 * ex) interval'23'hours + timestamp'2001-09-28 01:00:00.111 -8:00'  =>  timestamp'2001-09-29 01:00:00.111 -8:00'
 *******************************************************************************/
/**
 * @brief interval_day_to_second + timestamp with time zone
 *        <BR> interval_day_to_second + timestamp with time zone, 
 *        <BR>   interval_day_to_second + timestamp with time zone => [ P, O, M, S ]
 *        <BR> ex) interval'23' hours + timestamp'2001-09-28 01:00:0.123 -8:00'  =>  timestamp with time zone'2001-09-29 01:00:00.123 -8:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> INTERVAL DAY TO SECOND + TIMESTAMP with time zone
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 timestamp with time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfIntervalDayToSecondAddTimestampTz( stlUInt16        aInputArgumentCnt,
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

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_INTERVAL_DAY_TO_SECOND, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, sErrorStack );

    /* TimestampTz -> TimestampType */
    sTimestampType = ((dtlTimestampTzType *)(sValue2->mValue))->mTimestamp -
        (((dtlTimestampTzType *)(sValue2->mValue))->mTimeZone * DTL_USECS_PER_SEC);

    /*
     * Interval day to second를 usec 단위로 변경
     */
    sIntervalDayToSecond = ((dtlIntervalDayToSecondType *)( sValue1->mValue)); 

    /*
     * sTimestampType은 julian과 같이 표현되있으므로
     * interval day에 USECS_PER_DAY를 곱해 Day를 julian과 같이 변환해준다.
     * 그리고 Time을 더한다.
     */
    sTimestampType += ( sIntervalDayToSecond->mDay * DTL_USECS_PER_DAY ) +
        ( sIntervalDayToSecond->mTime );

    /*
     * 연산결과 validate
     */

    sTempTimestampType = sTimestampType;
    DTF_DATETIME_VALUE_IS_VALID( sTempTimestampType, sIsValid );
    STL_TRY_THROW( sIsValid == STL_TRUE, ERROR_OUT_OF_RANGE );

    /* Timezone을 원상복귀 */
    sTimestampType += (((dtlTimestampTzType *)(sValue2->mValue))->mTimeZone * DTL_USECS_PER_SEC);

    ((dtlTimestampTzType *)( sResultValue->mValue))->mTimestamp = sTimestampType;
    ((dtlTimestampTzType *)( sResultValue->mValue))->mTimeZone =
        ((dtlTimestampTzType *)(sValue2->mValue))->mTimeZone;

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
 * time + integer,  ADDDATE( date, integer ), DATEADD(date, integer )
 * ex) time'01:00:00' + integer '2'  =>  time'01:00:00'
 *******************************************************************************/

/**
 * @brief time without time zone + integer
 *        <BR> time without time zone + integer, 
 *        <BR> ex) time'01:00:00' + integer '2'  =>  time'01:00:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> TIME without time zone + INTEGER
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 time without time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimeAddInteger( stlUInt16        aInputArgumentCnt,
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

/**
 * @brief integer + time without time zone 
 *        <BR> integer + time without time zone
 *        <BR> ex) + integer '2' + time'01:00:00'  =>  time'01:00:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> INTEGER + TIME without time zone
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 time without time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfIntegerAddTime( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aEnv )
{
    dtlDataValue    * sResultValue;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( aInputArgument[0].mValue.mDataValue, DTL_TYPE_NATIVE_INTEGER, (stlErrorStack *)aEnv );
    DTF_CHECK_DTL_VALUE( aInputArgument[1].mValue.mDataValue, DTL_TYPE_TIME, (stlErrorStack *)aEnv );    

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIME, (stlErrorStack *)aEnv );

    /* 연산할 필요없이 Time 값을 그대로 반환 */
    DTF_TIME(sResultValue) = DTF_TIME(aInputArgument[1].mValue.mDataValue);
    sResultValue->mLength  = DTL_TIME_SIZE;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*******************************************************************************
 * time + numeric,  ADDDATE( date, numeric ), DATEADD( date, numeric )
 * ex) time'01:00:00' + numeric'0.5'  =>  time'13:00:00'
 *******************************************************************************/

/**
 * @brief time without time zone + numeric
 *        <BR> time without time zone + numeric
 *        <BR>   time without time zone + numeric => [ O, M, P]
 *        <BR> ex) time'01:00:00' + numeric'0.5'  =>  time'13:00:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> TIME without time zone + NUMERIC
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 time without time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimeAddNumeric( stlUInt16        aInputArgumentCnt,
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
    
    stlInt32         sNumericAlignMaxSize = STL_ALIGN_DEFAULT(DTL_NUMERIC_MAX_SIZE);
    
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
    
    sTime = DTF_TIME( sValue1 ) + sNumericTime;    
    
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

/**
 * @brief numeric + time without time zone
 *        <BR> numeric + time without time zone
 *        <BR> ex) numeric'0.5' + time'01:00:00'  =>  time'13:00:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> NUMERIC + TIME without time zone
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 time without time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfNumericAddTime( stlUInt16        aInputArgumentCnt,
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

    stlInt32         sNumericAlignMaxSize = STL_ALIGN_DEFAULT(DTL_NUMERIC_MAX_SIZE);

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

    DTF_CHECK_DTL_VALUE2( sValue1, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_TIME, sErrorStack );

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
    
    if( DTL_NUMERIC_IS_ZERO( (dtlNumericType *)(sValue1->mValue), sValue1->mLength ) )
    {
        sNumericTime = 0;
    }

    sDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( sValue1->mLength );
    sExponent = DTL_NUMERIC_GET_EXPONENT( DTF_NUMERIC( sValue1 ) );
    
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

        STL_TRY( dtfNumericMultiplication( sValue1,
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

    sTime = DTF_TIME( sValue2 ) + sNumericTime;    

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
 * time without time zone + interval_year_to_month
 *   time without time zone + interval => [ P, S ]
 * ex) time'01:00:00' + interval'3'year  =>  time'01:00:00'
 *******************************************************************************/

/**
 * @brief time without time zone + interval_year_to_month
 *        <BR> time without time zone + interval_year_to_month
 *        <BR> time without time zone + interval_year_to_month => [ P, S ]
 *        <BR> ex) time'01:00:00' + interval'3'day  =>  time'01:00:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> time without time zone + interval YEAR TO MONTH
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 TIME without time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimeAddIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
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
 * interval_year_to_month + time without time zone
 *   interval + time without time zone => [ P, S ]
 * ex) interval'3'year + time'01:00:00'  =>  time'01:00:00'
 *******************************************************************************/
/**
 * @brief interval_year_to_month + time without time zone
 *        <BR> interval_year_to_month + time without time zone
 *        <BR> interval_year_to_month + without time zone => [ P, S ]
 *        <BR> ex) interval'3'day + time'01:00:00'  =>  time'01:00:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> interval YEAR TO MONTH + time without time zone
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 TIME without time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfIntervalYearToMonthAddTime( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aEnv )
{
    dtlDataValue    * sResultValue;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( aInputArgument[0].mValue.mDataValue, DTL_TYPE_INTERVAL_YEAR_TO_MONTH, (stlErrorStack *)aEnv );
    DTF_CHECK_DTL_VALUE( aInputArgument[1].mValue.mDataValue, DTL_TYPE_TIME, (stlErrorStack *)aEnv );
   

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIME, (stlErrorStack *)aEnv );

    /* 연산할 필요없이 Time 값을 그대로 반환 */
    DTF_TIME(sResultValue) = DTF_TIME(aInputArgument[1].mValue.mDataValue);
    sResultValue->mLength  = DTL_TIME_SIZE;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*******************************************************************************
 * time without time zone + interval_day_to_second, ADDTIME( time, INTERVAL expr )
 *   time without time zone + interval => [ P, S ]
 * ex) time'01:00:00' + interval'3'hour  =>  time'04:00:00'
 *******************************************************************************/
/**
 * @brief time without time zone + interval_day_to_second
 *        <BR> time without time zone + interval_day_to_second
 *        <BR> time without time zone + interval_day_to_second => [ P, S ]
 *        <BR> ex) time'01:00' + interval'3' hours  =>  time'04:00:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> time without time zone + interval DAY TO SECOND
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 TIME without time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimeAddIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
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

    /* + 연산후, time 값 반환 */
    sTimeType = DTF_TIME(sValue1) + ((dtlIntervalDayToSecondType *)sValue2->mValue)->mTime;
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
 * interval_day_to_second + time without time zone
 *   interval + time without time zone => [ P, S ]
 * ex) interval'3'hour + time'01:00:00'  =>  time'04:00:00'
 *******************************************************************************/
/**
 * @brief interval_day_to_second + time without time zone
 *        <BR> interval_day_to_second + time without time zone
 *        <BR> interval_day_to_second + time without time zone => [ P, S ]
 *        <BR> ex) interval'3' hours + time'01:00'  =>  time'04:00:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> interval DAY TO SECOND + time without time zone
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 TIME without time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfIntervalDayToSecondAddTime( stlUInt16        aInputArgumentCnt,
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

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_INTERVAL_DAY_TO_SECOND, (stlErrorStack *)aEnv );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_TIME, (stlErrorStack *)aEnv );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIME, (stlErrorStack *)aEnv );

    /* + 연산후, time 값 반환 */
    sTimeType = DTF_TIME(sValue2) + ((dtlIntervalDayToSecondType *)sValue1->mValue)->mTime;
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
 * time with time zone + integer,  ADDDATE( date, integer ), DATEADD(date, integer )
 * ex) timetz'01:00:00 -08:00' + integer '2'  =>  timetz'01:00:00 -08:00'
 *******************************************************************************/

/**
 * @brief time with time zone + integer
 *        <BR> time with time zone + integer, 
 *        <BR> ex) timetz'01:00:00 -08:00' + integer '2'  =>  timetz'01:00:00 -08:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> TIME with time zone + INTEGER
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 time with time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimeTzAddInteger( stlUInt16        aInputArgumentCnt,
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

/**
 * @brief integer + time with time zone
 *        <BR> integer + time with time zone 
 *        <BR> ex) integer'2' + timetz'01:00:00 -08:00' timetz'01:00:00 -08:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> INTEGER + TIME with time zone
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 time with time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfIntegerAddTimeTz( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aEnv )
{
    dtlDataValue    * sValue1;    
    dtlDataValue    * sResultValue;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1      = aInputArgument[1].mValue.mDataValue;    
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( aInputArgument[0].mValue.mDataValue, DTL_TYPE_NATIVE_INTEGER, (stlErrorStack *)aEnv );
    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_TIME_WITH_TIME_ZONE, (stlErrorStack *)aEnv );

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
 * time with time zone + numeric,  ADDDATE( date, numeric ), DATEADD( date, numeric )
 * ex) timetz'01:00:00 -08:00' + numeric'0.5'  =>  timetz'13:00:00 -08:00'
 *******************************************************************************/

/**
 * @brief time with time zone + numeric
 *        <BR> time with time zone + numeric
 *        <BR> ex) timetz'01:00:00 -08:00' + numeric'0.5'  =>  timetz'13:00:00 -08:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> TIME with time zone + NUMERIC
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 time with time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimeTzAddNumeric( stlUInt16        aInputArgumentCnt,
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

    stlInt32         sNumericAlignMaxSize = STL_ALIGN_DEFAULT(DTL_NUMERIC_MAX_SIZE);

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
     * TIME WITH TIME ZONE + NUMERIC
     */

    sTime = ((dtlTimeTzType *)(sValue1->mValue))->mTime -
        (((dtlTimeTzType *)(sValue1->mValue))->mTimeZone  * DTL_USECS_PER_SEC);
    sTime = sTime + sNumericTime;    

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

/**
 * @brief numeric + time with time zone
 *        <BR> numeric + time with time zone
 *        <BR> ex) numeric'0.5' + timetz'01:00:00 -08:00'  =>  timetz'13:00:00 -08:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> NUMERIC + TIME with time zone
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 time with time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfNumericAddTimeTz( stlUInt16        aInputArgumentCnt,
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

    stlInt32         sNumericAlignMaxSize = STL_ALIGN_DEFAULT(DTL_NUMERIC_MAX_SIZE);

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
    
    DTF_CHECK_DTL_VALUE2( sValue1, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_TIME_WITH_TIME_ZONE, sErrorStack );
    
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
    
    if( DTL_NUMERIC_IS_ZERO( (dtlNumericType *)(sValue1->mValue), sValue1->mLength ) )
    {
        sNumericTime = 0;
    }

    sDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( sValue1->mLength );
    sExponent = DTL_NUMERIC_GET_EXPONENT( DTF_NUMERIC( sValue1 ) );
    
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

        STL_TRY( dtfNumericMultiplication( sValue1,
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
     * TIME WITH TIME ZONE + NUMERIC
     */

    sTime = ((dtlTimeTzType *)(sValue2->mValue))->mTime -
        (((dtlTimeTzType *)(sValue2->mValue))->mTimeZone  * DTL_USECS_PER_SEC);
    sTime = sTime + sNumericTime;    

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
        sTime + (((dtlTimeTzType *)(sValue2->mValue))->mTimeZone * DTL_USECS_PER_SEC );
    ((dtlTimeTzType *)(sResultValue->mValue))->mTimeZone = ((dtlTimeTzType *)(sValue2->mValue))->mTimeZone;

    sResultValue->mLength = DTL_TIMETZ_SIZE;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/*******************************************************************************
 * time with time zone + interval_year_to_month
 *   time with time zone + interval => [ P, S ]
 * ex) time'01:00:00.123 -8:00' + interval'1'year  =>  time'01:00:00.123 -8:00'
 *******************************************************************************/
/**
 * @brief time with time zone + interval_year_to_month
 *        <BR> time with time zone + interval_year_to_month
 *        <BR> time with time zone + interval_year_to_month => [ P, S ]
 *        <BR> ex) time'01:00:00.123 -8:00' + interval'1'year  =>  time'1:00:00.123 -8:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> time with time zone + interval YEAR TO MONTH
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 TIME with time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimeTzAddIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
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
 * interval_year_to_month + time with time zone
 *   interval + time with time zone => [ P, S ]
 * ex) interval'1'year + time'01:00:00.123 -8:00'  =>  time'01:00:00.123 -8:00'
 *******************************************************************************/
/**
 * @brief interval_year_to_month + time with time zone
 *        <BR> interval_year_to_month + time with time zone
 *        <BR> interval_year_to_month + time with time zone => [ P, S ]
 *        <BR> ex) interval'1'year + time'01:00:00.123 -8:00'  =>  time'1:00:00.123 -8:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> interval YEAR TO MONTH + time with time zone
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 TIME with time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfIntervalYearToMonthAddTimeTz( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sResultValue;
    dtlTimeTzType     sTimeTzType;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1      = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( aInputArgument[0].mValue.mDataValue, DTL_TYPE_INTERVAL_YEAR_TO_MONTH, (stlErrorStack *)aEnv );
    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_TIME_WITH_TIME_ZONE, (stlErrorStack *)aEnv );
   
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
 * time with time zone + interval_day_to_second, ADDTIME( time, INTERVAL expr )
 *   time with time zone + interval => [ P, S ]
 * ex) time with time zone'01:00:00.000 -8:00' + interval'3'hour  =>  time'04:00:00.000 -8:00'
 *******************************************************************************/
/**
 * @brief time with time zone + interval_day_to_second
 *        <BR> time with time zone + interval_day_to_second
 *        <BR> time with time zone + interval_day_to_second => [ P, S ]
 *        <BR> ex) time with time zone'01:00:01.123 -8:00' + interval'3' hours  =>  time'04:00:01.123 -8:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> time with time zone + interval DAY TO SECOND
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 TIME with time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfTimeTzAddIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
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

    /* Add */
    sTimeType += ((dtlIntervalDayToSecondType *)sValue2->mValue )->mTime;
    
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
 * interval_day_to_second + time with time zone
 *   interval + time with time zone => [ P, S ]
 * ex) interval'3'hour + time with time zone'01:00:00.000 -8:00'  =>  time'04:00:00.000 -8:00'
 *******************************************************************************/
/**
 * @brief interval_day_to_second + time with time zone
 *        <BR> interval_day_to_second + time with time zone
 *        <BR> interval_day_to_second + time with time zone => [ P, S ]
 *        <BR> ex) interval'3' hours + time with time zone'01:00:01.123 -8:00'  =>  time'04:00:01.123 -8:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> interval DAY TO SECOND + time with time zone
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 TIME with time zone)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfIntervalDayToSecondAddTimeTz( stlUInt16        aInputArgumentCnt,
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

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_INTERVAL_DAY_TO_SECOND, (stlErrorStack *)aEnv );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_TIME_WITH_TIME_ZONE, (stlErrorStack *)aEnv );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue,
                          DTL_TYPE_TIME_WITH_TIME_ZONE,
                          (stlErrorStack *)aEnv );

    sTimeZone = ((dtlTimeTzType *)(sValue2->mValue))->mTimeZone;

    /* TimeTzType -> TimeType */
    sTimeType = ((dtlTimeTzType *)(sValue2->mValue))->mTime - ( sTimeZone * DTL_USECS_PER_SEC );

    /* Add */
    sTimeType += ((dtlIntervalDayToSecondType *)sValue1->mValue )->mTime;
    
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

/** @} */
