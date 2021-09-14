/*******************************************************************************
 * dtfDateAdd.c
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
 * @file dtfDateAdd.c
 * @brief Date and Time Function DataType Library Layer
 */

#include <dtl.h>
#include <dtlDef.h>
#include <dtDef.h>
#include <dtfNumeric.h>
#include <dtfArithmetic.h>

/**
 * @ingroup dtf
 * @internal
 * @{
 */


/* DTL_JULIAN_MAXYEAR + (-DTL_JULIAN_MINYEAR) => 14712 */
#define DTF_DATETIME_PART_YEAR_MAX_ADD_VALUE ( 14712 )
#define DTF_DATETIME_PART_YEAR_MIN_ADD_VALUE ( -14712 )

/* DTF_DATETIME_PART_YEAR_MAX_ADD_VALUE * 12 + 1 ) => 176545 */
#define DTF_DATETIME_PART_MONTH_MAX_ADD_VALUE ( 176545 )
#define DTF_DATETIME_PART_MONTH_MIN_ADD_VALUE ( -176545 )

/* DTL_JULIAN_MAXJULDAY => 5373484 */
#define DTF_DATETIME_PART_DAY_MAX_ADD_VALUE ( 5373484 )
#define DTF_DATETIME_PART_DAY_MIN_ADD_VALUE ( -5373484 )

/* DTF_DATETIME_PART_DAY_MAX_ADD_VALUE * 24 + 23 ) => 128963639 */
#define DTF_DATETIME_PART_HOUR_MAX_ADD_VALUE ( 128963639 )
#define DTF_DATETIME_PART_HOUR_MIN_ADD_VALUE ( -128963639 )

/* DTF_DATETIME_PART_HOUR_MAX_ADD_VALUE * 60 + 59 => 7737818399 */
#define DTF_DATETIME_PART_MINUTE_MAX_ADD_VALUE   STL_INT64_C( 7737818399 )
#define DTF_DATETIME_PART_MINUTE_MIN_ADD_VALUE   STL_INT64_C( -7737818399 )

/* DTF_DATETIME_PART_MINUTE_MAX_ADD_VALUE * 60 + 59 => 464269103999 */
#define DTF_DATETIME_PART_SECOND_MAX_ADD_VALUE   STL_INT64_C( 464269103999 )
#define DTF_DATETIME_PART_SECOND_MIN_ADD_VALUE   STL_INT64_C( -464269103999 )

/* DTF_DATETIME_PART_SECOND_MAX_ADD_VALUE * 1000 + 999 => 464269103999999 */
#define DTF_DATETIME_PART_MILLISECOND_MAX_ADD_VALUE   STL_INT64_C( 464269103999999 )
#define DTF_DATETIME_PART_MILLISECOND_MIN_ADD_VALUE   STL_INT64_C( -464269103999999 )

/* DTF_DATETIME_PART_MILLISECOND_MAX_ADD_VALUE * 1000 + 999 => 464269103999999999 */
#define DTF_DATETIME_PART_MICROSECOND_MAX_ADD_VALUE   STL_INT64_C( 464269103999999999 )
#define DTF_DATETIME_PART_MICROSECOND_MIN_ADD_VALUE   STL_INT64_C( -464269103999999999 )


/**
 * @brief DATEADD( datepart, number, DATE )
 *        <BR>   DATEADD( datepart, number, DATE ) => [ mssql ] 
 *        <BR> ex) DATEADD( YEAR, 3, DATE'2014-02-17' ) => DATE'2017-02-17'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> DATEADD( DTL_TYPE_NUMBER, DTL_TYPE_NUMBER, DATE )
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 DATE)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfDateAddForDateAddDatepart( stlUInt16        aInputArgumentCnt,
                                        dtlValueEntry  * aInputArgument,
                                        void           * aResultValue,
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
    stlInt64         sNum = 0;
    stlBool          sIsTruncated = STL_FALSE;
    stlBool          sIsValid = STL_FALSE;
    
    dtlExpTime          sDtlExpTime;
    dtlFractionalSecond sFractionalSecond = 0;    
    dtlDateType         sDateType;
    dtlDateType         sTempDateType;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 3, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sValue3 = aInputArgument[2].mValue.mDataValue;    
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue3, DTL_TYPE_DATE, sErrorStack );    

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_DATE, sErrorStack );

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
     * number ( sValue2 ) : 반올림하지 않는다.
     */
    
    STL_TRY( dtlNumericToInt64( sValue2,
                                & sNum,
                                & sIsTruncated,
                                sErrorStack )
             == STL_SUCCESS );

    /*
     * date ( sValue3 ) 
     */
    
    DTL_INIT_DTLEXPTIME( sDtlExpTime );
    
    sDateType = DTF_DATE(sValue3);
    
    /*
     * datepart에 대한 add 연산
     */ 
    
    switch( sDateTimePart )
    {
        case DTL_DATETIME_PART_YEAR:
            {
                STL_TRY_THROW( ( sNum >= DTF_DATETIME_PART_YEAR_MIN_ADD_VALUE ) &&
                               ( sNum <= DTF_DATETIME_PART_YEAR_MAX_ADD_VALUE ),
                               ERROR_OUT_OF_RANGE );
                
                STL_TRY( dtdDateTime2dtlExpTime( sDateType,
                                                 & sDtlExpTime,
                                                 & sFractionalSecond,
                                                 sErrorStack )
                         == STL_SUCCESS );
                
                sDtlExpTime.mYear = sDtlExpTime.mYear + sNum;
                
                /*
                 * validate
                 */
                
                STL_TRY_THROW( dtdDtlExpTime2DateTimeOffset( & sDtlExpTime,
                                                             sFractionalSecond,
                                                             & sDateType,
                                                             sErrorStack )
                               == STL_SUCCESS,
                               ERROR_OUT_OF_RANGE );
                
                STL_THROW( SUCCESS_ADD_DATEPART );
                
                break;
            }
        case DTL_DATETIME_PART_QUARTER:
            {
                sNum = sNum * 3;
            }
            /* FALL_THROUGH */
        case DTL_DATETIME_PART_MONTH:
            {
                STL_TRY_THROW( ( sNum >= DTF_DATETIME_PART_MONTH_MIN_ADD_VALUE ) &&
                               ( sNum <= DTF_DATETIME_PART_MONTH_MAX_ADD_VALUE ),
                               ERROR_OUT_OF_RANGE );

                STL_TRY( dtdDateTime2dtlExpTime( sDateType,
                                                 & sDtlExpTime,
                                                 & sFractionalSecond,
                                                 sErrorStack )
                         == STL_SUCCESS );
                
                sDtlExpTime.mMonth = sDtlExpTime.mMonth + sNum;
                
                if( sDtlExpTime.mMonth > DTL_MONTHS_PER_YEAR )
                {
                    sDtlExpTime.mYear += (sDtlExpTime.mMonth - 1) / DTL_MONTHS_PER_YEAR;
                    sDtlExpTime.mMonth = ((sDtlExpTime.mMonth - 1) % DTL_MONTHS_PER_YEAR) + 1;
                }
                else if( sDtlExpTime.mMonth < 1 )
                {
                    sDtlExpTime.mYear += (sDtlExpTime.mMonth / DTL_MONTHS_PER_YEAR) - 1;
                    sDtlExpTime.mMonth =
                        (sDtlExpTime.mMonth % DTL_MONTHS_PER_YEAR) + DTL_MONTHS_PER_YEAR;
                }
                else
                {
                    // Do Nothing
                }
                
                /*
                 * validate
                 */
                
                STL_TRY_THROW( dtdDtlExpTime2DateTimeOffset( & sDtlExpTime,
                                                             sFractionalSecond,
                                                             & sDateType,
                                                             sErrorStack )
                               == STL_SUCCESS,
                               ERROR_OUT_OF_RANGE );
                
                STL_THROW( SUCCESS_ADD_DATEPART );                
                
                break;
            }
        case DTL_DATETIME_PART_WEEK:
            {
                sNum = sNum * 7;
            }
            /* FALL_THROUGH */
        case DTL_DATETIME_PART_DAY:
        case DTL_DATETIME_PART_DAYOFYEAR:
        case DTL_DATETIME_PART_WEEKDAY:
            {
                STL_TRY_THROW( ( sNum >= DTF_DATETIME_PART_DAY_MIN_ADD_VALUE ) &&
                               ( sNum <= DTF_DATETIME_PART_DAY_MAX_ADD_VALUE ),
                               ERROR_OUT_OF_RANGE );
                
                sDateType = sDateType + ( sNum * DTL_USECS_PER_DAY );
                
                break;
            }
        case DTL_DATETIME_PART_HOUR:
        case DTL_DATETIME_PART_MINUTE:
        case DTL_DATETIME_PART_SECOND:
        case DTL_DATETIME_PART_MILLISECOND:
        case DTL_DATETIME_PART_MICROSECOND:
            {
                STL_THROW( ERROR_INVALID_DATEPART );
                break;
            }
        default:
            STL_DASSERT( STL_FALSE );
            break;
    }

    
    /*
     * validate
     */

    sTempDateType = sDateType;
    
    DTF_DATETIME_VALUE_IS_VALID( sTempDateType, sIsValid );
    STL_TRY_THROW( sIsValid == STL_TRUE, ERROR_OUT_OF_RANGE );
    
    STL_RAMP( SUCCESS_ADD_DATEPART );

    DTF_DATE(sResultValue) = sDateType;
    sResultValue->mLength  = DTL_DATE_SIZE;
    
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        (void)stlPopError( sErrorStack );
        
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATETIME_FIELD_OVERFLOW,
                      NULL,
                      sErrorStack );
    }
    
    STL_CATCH( ERROR_INVALID_DATEPART )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_DATEPART_FOR_DATA_TYPE,
                      NULL,
                      sErrorStack,
                      dtlDataTypeName[sValue3->mType] );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief DATEADD( datepart, number, TIMESTAMP )
 *        <BR>   DATEADD( datepart, number, TIMESTAMP ) => [ mssql ] 
 *        <BR> ex) DATEADD( YEAR, 3, TIMESTAMP'2014-02-17 11:22:33.123456' ) => TIMESTAMP'2017-02-17 11:22:33.123456'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> DATEADD( DTL_TYPE_NUMBER, DTL_TYPE_NUMBER, TIMESTAMP )
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 TIMESTAMP)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfDateAddForTimestampAddDatepart( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv )
{
    dtlDataValue   * sValue1;
    dtlDataValue   * sValue2;
    dtlDataValue   * sValue3;    
    dtlDataValue   * sResultValue;
    stlErrorStack  * sErrorStack;

    stlUInt8           * sNumericDataPtr;    
    stlInt32             sDigitCount = 0;
    stlInt32             sDateTimePart = 0;
    stlInt64             sNum = 0;
    stlBool              sIsTruncated = STL_FALSE;
    stlBool              sIsValid = STL_FALSE;
    
    dtlExpTime           sDtlExpTime;    
    dtlFractionalSecond  sFractionalSecond = 0;
    dtlTimestampType     sTimestampType = 0;
    dtlTimestampType     sTempTimestampType = 0;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 3, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sValue3 = aInputArgument[2].mValue.mDataValue;    
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue3, DTL_TYPE_TIMESTAMP, sErrorStack );    

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIMESTAMP, sErrorStack );

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
     * number ( sValue2 ) : 반올림하지 않는다.
     */
    
    STL_TRY( dtlNumericToInt64( sValue2,
                                & sNum,
                                & sIsTruncated,
                                sErrorStack )
             == STL_SUCCESS );

    /*
     * timestamp ( sValue3 ) 
     */
    
    DTL_INIT_DTLEXPTIME( sDtlExpTime );
    
    sTimestampType = DTF_TIMESTAMP(sValue3);
    
    /*
     * datepart에 대한 add 연산
     */ 
    
    switch( sDateTimePart )
    {
        case DTL_DATETIME_PART_YEAR:
            {
                STL_TRY_THROW( ( sNum >= DTF_DATETIME_PART_YEAR_MIN_ADD_VALUE ) &&
                               ( sNum <= DTF_DATETIME_PART_YEAR_MAX_ADD_VALUE ),
                               ERROR_OUT_OF_RANGE );

                STL_TRY( dtdDateTime2dtlExpTime( sTimestampType,
                                                 & sDtlExpTime,
                                                 & sFractionalSecond,
                                                 sErrorStack )
                         == STL_SUCCESS );
                
                sDtlExpTime.mYear = sDtlExpTime.mYear + sNum;
                
                /*
                 * validate
                 */
                
                STL_TRY_THROW( dtdDtlExpTime2DateTimeOffset( & sDtlExpTime,
                                                             sFractionalSecond,
                                                             & sTimestampType,
                                                             sErrorStack )
                               == STL_SUCCESS,
                               ERROR_OUT_OF_RANGE );
                
                STL_THROW( SUCCESS_ADD_DATEPART );

                break;
            }
        case DTL_DATETIME_PART_QUARTER:
            {
                sNum = sNum * 3;
            }
            /* FALL_THROUGH */
        case DTL_DATETIME_PART_MONTH:
            {
                STL_TRY_THROW( ( sNum >= DTF_DATETIME_PART_MONTH_MIN_ADD_VALUE ) &&
                               ( sNum <= DTF_DATETIME_PART_MONTH_MAX_ADD_VALUE ),
                               ERROR_OUT_OF_RANGE );

                STL_TRY( dtdDateTime2dtlExpTime( sTimestampType,
                                                 & sDtlExpTime,
                                                 & sFractionalSecond,
                                                 sErrorStack )
                         == STL_SUCCESS );
                
                sDtlExpTime.mMonth = sDtlExpTime.mMonth + sNum;
                
                if( sDtlExpTime.mMonth > DTL_MONTHS_PER_YEAR )
                {
                    sDtlExpTime.mYear += (sDtlExpTime.mMonth - 1) / DTL_MONTHS_PER_YEAR;
                    sDtlExpTime.mMonth = ((sDtlExpTime.mMonth - 1) % DTL_MONTHS_PER_YEAR) + 1;
                }
                else if( sDtlExpTime.mMonth < 1 )
                {
                    sDtlExpTime.mYear += (sDtlExpTime.mMonth / DTL_MONTHS_PER_YEAR) - 1;
                    sDtlExpTime.mMonth =
                        (sDtlExpTime.mMonth % DTL_MONTHS_PER_YEAR) + DTL_MONTHS_PER_YEAR;
                }
                else
                {
                    // Do Nothing
                }
                
                /*
                 * validate
                 */
                
                STL_TRY_THROW( dtdDtlExpTime2DateTimeOffset( & sDtlExpTime,
                                                             sFractionalSecond,
                                                             & sTimestampType,
                                                             sErrorStack )
                               == STL_SUCCESS,
                               ERROR_OUT_OF_RANGE );
                
                STL_THROW( SUCCESS_ADD_DATEPART );
                
                break;
            }
        case DTL_DATETIME_PART_WEEK:
            {
                sNum = sNum * 7;
            }
            /* FALL_THROUGH */
        case DTL_DATETIME_PART_DAY:
        case DTL_DATETIME_PART_DAYOFYEAR:
        case DTL_DATETIME_PART_WEEKDAY:
            {
                STL_TRY_THROW( ( sNum >= DTF_DATETIME_PART_DAY_MIN_ADD_VALUE ) &&
                               ( sNum <= DTF_DATETIME_PART_DAY_MAX_ADD_VALUE ),
                               ERROR_OUT_OF_RANGE );
                
                sTimestampType = sTimestampType + ( sNum * DTL_USECS_PER_DAY );
                
                break;
            }
        case DTL_DATETIME_PART_HOUR:
            {
                STL_TRY_THROW( ( sNum >= DTF_DATETIME_PART_HOUR_MIN_ADD_VALUE ) &&
                               ( sNum <= DTF_DATETIME_PART_HOUR_MAX_ADD_VALUE ),
                               ERROR_OUT_OF_RANGE );
                
                sTimestampType = sTimestampType + ( sNum * DTL_USECS_PER_HOUR );
                
                break;
            }
        case DTL_DATETIME_PART_MINUTE:
            {
                STL_TRY_THROW( ( sNum >= DTF_DATETIME_PART_MINUTE_MIN_ADD_VALUE ) &&
                               ( sNum <= DTF_DATETIME_PART_MINUTE_MAX_ADD_VALUE ),
                               ERROR_OUT_OF_RANGE );
                
                sTimestampType = sTimestampType + ( sNum * DTL_USECS_PER_MINUTE );
                
                break;
            }
        case DTL_DATETIME_PART_SECOND:
            {
                STL_TRY_THROW( ( sNum >= DTF_DATETIME_PART_SECOND_MIN_ADD_VALUE ) &&
                               ( sNum <= DTF_DATETIME_PART_SECOND_MAX_ADD_VALUE ),
                               ERROR_OUT_OF_RANGE );
                
                sTimestampType = sTimestampType + ( sNum * DTL_USECS_PER_SEC );
                
                break;
            }
        case DTL_DATETIME_PART_MILLISECOND:
            {
                STL_TRY_THROW( ( sNum >= DTF_DATETIME_PART_MILLISECOND_MIN_ADD_VALUE ) &&
                               ( sNum <= DTF_DATETIME_PART_MILLISECOND_MAX_ADD_VALUE ),
                               ERROR_OUT_OF_RANGE );
                
                sTimestampType = sTimestampType + ( sNum * DTL_USECS_PER_MILLISEC );
                
                break;
            }
        case DTL_DATETIME_PART_MICROSECOND:
            {
                STL_TRY_THROW( ( sNum >= DTF_DATETIME_PART_MICROSECOND_MIN_ADD_VALUE ) &&
                               ( sNum <= DTF_DATETIME_PART_MICROSECOND_MAX_ADD_VALUE ),
                               ERROR_OUT_OF_RANGE );
                
                sTimestampType = sTimestampType + sNum;

                break;
            }
        default:
            STL_DASSERT( STL_FALSE );
            break;
    }
    
    /*
     * validate
     */
    
    sTempTimestampType = sTimestampType;
    
    DTF_DATETIME_VALUE_IS_VALID( sTempTimestampType, sIsValid );
    STL_TRY_THROW( sIsValid == STL_TRUE, ERROR_OUT_OF_RANGE );
    
    STL_RAMP( SUCCESS_ADD_DATEPART );
    
    DTF_TIMESTAMP(sResultValue) = sTimestampType;
    sResultValue->mLength       = DTL_TIMESTAMP_SIZE;
    
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        (void)stlPopError( sErrorStack );
        
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATETIME_FIELD_OVERFLOW,
                      NULL,
                      sErrorStack );
    }
    
    STL_CATCH( ERROR_INVALID_DATEPART )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_DATEPART_FOR_DATA_TYPE,
                      NULL,
                      sErrorStack,
                      dtlDataTypeName[sValue3->mType] );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief DATEADD( datepart, number, TIMESTAMP WITH TIME ZONE )
 *        <BR>   DATEADD( datepart, number, TIMESTAMP WITH TIME ZONE ) => [ mssql ] 
 *        <BR> ex) DATEADD( YEAR, 3, TIMESTAMP WITH TIME ZONE'2014-02-17 11:22:33.123456 +09:00' )
 *        <BR>     => TIMESTAMP WITH TIME ZONE'2017-02-17 11:22:33.123456 +09:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> DATEADD( DTL_TYPE_NUMBER, DTL_TYPE_NUMBER, TIMESTAMP WITH TIME ZONE )
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 TIMESTAMP WITH TIME ZONE)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfDateAddForTimestampWithTimeZoneAddDatepart( stlUInt16        aInputArgumentCnt,
                                                         dtlValueEntry  * aInputArgument,
                                                         void           * aResultValue,
                                                         dtlFuncVector  * aVectorFunc,
                                                         void           * aVectorArg,
                                                         void           * aEnv )
{
    dtlDataValue   * sValue1;
    dtlDataValue   * sValue2;
    dtlDataValue   * sValue3;    
    dtlDataValue   * sResultValue;
    stlErrorStack  * sErrorStack;

    stlUInt8           * sNumericDataPtr;    
    stlInt32             sDigitCount = 0;
    stlInt32             sDateTimePart = 0;
    stlInt64             sNum = 0;
    stlBool              sIsTruncated = STL_FALSE;
    stlBool              sIsValid = STL_FALSE;

    dtlExpTime           sDtlExpTime;
    dtlFractionalSecond  sFractionalSecond = 0;
    dtlTimestampTzType   sTimestampTzType;
    stlInt32             sTimeZone = 0;
    dtlTimestampType     sTimestampType = 0;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 3, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sValue3 = aInputArgument[2].mValue.mDataValue;    
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue3, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, sErrorStack );    

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, sErrorStack );

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
     * number ( sValue2 ) : 반올림하지 않는다.
     */
    
    STL_TRY( dtlNumericToInt64( sValue2,
                                & sNum,
                                & sIsTruncated,
                                sErrorStack )
             == STL_SUCCESS );

    /*
     * timestamp with time zone ( sValue3 ) 
     */
    
    DTL_INIT_DTLEXPTIME( sDtlExpTime );

    sTimestampTzType.mTimestamp = ((dtlTimestampTzType *)(sValue3->mValue))->mTimestamp;
    sTimestampTzType.mTimeZone  = ((dtlTimestampTzType *)(sValue3->mValue))->mTimeZone;

    /* TimestampTz -> TimestampType */
    sTimestampType = sTimestampTzType.mTimestamp - ( sTimestampTzType.mTimeZone * DTL_USECS_PER_SEC );

    /*
     * datepart에 대한 add 연산
     */ 
    
    switch( sDateTimePart )
    {
        case DTL_DATETIME_PART_YEAR:
            {
                STL_TRY_THROW( ( sNum >= DTF_DATETIME_PART_YEAR_MIN_ADD_VALUE ) &&
                               ( sNum <= DTF_DATETIME_PART_YEAR_MAX_ADD_VALUE ),
                               ERROR_OUT_OF_RANGE );
                
                STL_TRY( dtdTimestampTz2dtlExpTime( sTimestampTzType,
                                                    & sTimeZone,
                                                    & sDtlExpTime,
                                                    & sFractionalSecond,
                                                    sErrorStack )
                         == STL_SUCCESS );
                
                sDtlExpTime.mYear = sDtlExpTime.mYear + sNum;
                
                /*
                 * validate
                 */
                
                if( dtdDtlExpTime2TimestampTz( gDataTypeDefinition[DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE].mSqlNormalTypeName,
                                               & sDtlExpTime,
                                               sFractionalSecond,
                                               & sTimeZone,
                                               & sTimestampTzType,
                                               sErrorStack )
                    == STL_FAILURE )
                {
                    (void)stlPopError( sErrorStack );
                    STL_THROW( ERROR_OUT_OF_RANGE );
                }
                
                STL_THROW( SUCCESS_ADD_DATEPART );

                break;
            }
        case DTL_DATETIME_PART_QUARTER:
            {
                sNum = sNum * 3;
            }
            /* FALL_THROUGH */
        case DTL_DATETIME_PART_MONTH:
            {
                STL_TRY_THROW( ( sNum >= DTF_DATETIME_PART_MONTH_MIN_ADD_VALUE ) &&
                               ( sNum <= DTF_DATETIME_PART_MONTH_MAX_ADD_VALUE ),
                               ERROR_OUT_OF_RANGE );
                
                STL_TRY( dtdTimestampTz2dtlExpTime( sTimestampTzType,
                                                    & sTimeZone,
                                                    & sDtlExpTime,
                                                    & sFractionalSecond,
                                                    sErrorStack )
                         == STL_SUCCESS );
                
                sDtlExpTime.mMonth = sDtlExpTime.mMonth + sNum;
                
                if( sDtlExpTime.mMonth > DTL_MONTHS_PER_YEAR )
                {
                    sDtlExpTime.mYear += (sDtlExpTime.mMonth - 1) / DTL_MONTHS_PER_YEAR;
                    sDtlExpTime.mMonth = ((sDtlExpTime.mMonth - 1) % DTL_MONTHS_PER_YEAR) + 1;
                }
                else if( sDtlExpTime.mMonth < 1 )
                {
                    sDtlExpTime.mYear += (sDtlExpTime.mMonth / DTL_MONTHS_PER_YEAR) - 1;
                    sDtlExpTime.mMonth =
                        (sDtlExpTime.mMonth % DTL_MONTHS_PER_YEAR) + DTL_MONTHS_PER_YEAR;
                }
                else
                {
                    // Do Nothing
                }
                
                /*
                 * validate
                 */
                
                if( dtdDtlExpTime2TimestampTz( gDataTypeDefinition[DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE].mSqlNormalTypeName,
                                               & sDtlExpTime,
                                               sFractionalSecond,
                                               & sTimeZone,
                                               & sTimestampTzType,
                                               sErrorStack )
                    == STL_FAILURE )
                {
                    (void)stlPopError( sErrorStack );
                    STL_THROW( ERROR_OUT_OF_RANGE );
                }
                
                STL_THROW( SUCCESS_ADD_DATEPART );
                
                break;
            }
        case DTL_DATETIME_PART_WEEK:
            {
                sNum = sNum * 7;
            }
            /* FALL_THROUGH */
        case DTL_DATETIME_PART_DAY:
        case DTL_DATETIME_PART_DAYOFYEAR:
        case DTL_DATETIME_PART_WEEKDAY:
            {
                STL_TRY_THROW( ( sNum >= DTF_DATETIME_PART_DAY_MIN_ADD_VALUE ) &&
                               ( sNum <= DTF_DATETIME_PART_DAY_MAX_ADD_VALUE ),
                               ERROR_OUT_OF_RANGE );
                
                sTimestampType = sTimestampType + ( sNum * DTL_USECS_PER_DAY );
                
                break;
            }
        case DTL_DATETIME_PART_HOUR:
            {
                STL_TRY_THROW( ( sNum >= DTF_DATETIME_PART_HOUR_MIN_ADD_VALUE ) &&
                               ( sNum <= DTF_DATETIME_PART_HOUR_MAX_ADD_VALUE ),
                               ERROR_OUT_OF_RANGE );
                
                sTimestampType = sTimestampType + ( sNum * DTL_USECS_PER_HOUR );
                
                break;
            }
        case DTL_DATETIME_PART_MINUTE:
            {
                STL_TRY_THROW( ( sNum >= DTF_DATETIME_PART_MINUTE_MIN_ADD_VALUE ) &&
                               ( sNum <= DTF_DATETIME_PART_MINUTE_MAX_ADD_VALUE ),
                               ERROR_OUT_OF_RANGE );
                
                sTimestampType = sTimestampType + ( sNum * DTL_USECS_PER_MINUTE );
                
                break;
            }
        case DTL_DATETIME_PART_SECOND:
            {
                STL_TRY_THROW( ( sNum >= DTF_DATETIME_PART_SECOND_MIN_ADD_VALUE ) &&
                               ( sNum <= DTF_DATETIME_PART_SECOND_MAX_ADD_VALUE ),
                               ERROR_OUT_OF_RANGE );
                
                sTimestampType = sTimestampType + ( sNum * DTL_USECS_PER_SEC );
                
                break;
            }
        case DTL_DATETIME_PART_MILLISECOND:
            {
                STL_TRY_THROW( ( sNum >= DTF_DATETIME_PART_MILLISECOND_MIN_ADD_VALUE ) &&
                               ( sNum <= DTF_DATETIME_PART_MILLISECOND_MAX_ADD_VALUE ),
                               ERROR_OUT_OF_RANGE );
                
                sTimestampType = sTimestampType + ( sNum * DTL_USECS_PER_MILLISEC );
                
                break;
            }
        case DTL_DATETIME_PART_MICROSECOND:
            {
                STL_TRY_THROW( ( sNum >= DTF_DATETIME_PART_MICROSECOND_MIN_ADD_VALUE ) &&
                               ( sNum <= DTF_DATETIME_PART_MICROSECOND_MAX_ADD_VALUE ),
                               ERROR_OUT_OF_RANGE );
                
                sTimestampType = sTimestampType + sNum;

                break;
            }
        default:
            STL_DASSERT( STL_FALSE );
            break;
    }
    
    /*
     * validate
     */
    /* Timezone을 원상복귀 */
    sTimestampTzType.mTimestamp = sTimestampType + ( sTimestampTzType.mTimeZone * DTL_USECS_PER_SEC );
    
    DTF_DATETIME_VALUE_IS_VALID( sTimestampType, sIsValid );
    STL_TRY_THROW( sIsValid == STL_TRUE, ERROR_OUT_OF_RANGE );
    
    STL_RAMP( SUCCESS_ADD_DATEPART );
    
    ((dtlTimestampTzType *)( sResultValue->mValue))->mTimestamp = sTimestampTzType.mTimestamp;
    ((dtlTimestampTzType *)( sResultValue->mValue))->mTimeZone = sTimestampTzType.mTimeZone;
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

    STL_CATCH( ERROR_INVALID_DATEPART )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_DATEPART_FOR_DATA_TYPE,
                      NULL,
                      sErrorStack,
                      dtlDataTypeName[sValue3->mType] );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief DATEADD( datepart, number, TIME )
 *        <BR>   DATEADD( datepart, number, TIME ) => [ mssql ] 
 *        <BR> ex) DATEADD( HOUR, 3, TIME'11:22:33.123456' ) => TIME'14:22:33.123456'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> DATEADD( DTL_TYPE_NUMBER, DTL_TYPE_NUMBER, DTL_TYPE_TIME )
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 TIME)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfDateAddForTimeAddDatepart( stlUInt16        aInputArgumentCnt,
                                        dtlValueEntry  * aInputArgument,
                                        void           * aResultValue,
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
    stlInt64         sNum = 0;
    stlBool          sIsTruncated = STL_FALSE;
    
    dtlTimeType      sTimeType;
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 3, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sValue3 = aInputArgument[2].mValue.mDataValue;    
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue3, DTL_TYPE_TIME, sErrorStack );    

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIME, sErrorStack );

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
     * number ( sValue2 ) : 반올림하지 않는다.
     */
    
    STL_TRY( dtlNumericToInt64( sValue2,
                                & sNum,
                                & sIsTruncated,
                                sErrorStack )
             == STL_SUCCESS );
    
    /*
     * time ( sValue3 ) 
     */
    
    sTimeType = DTF_TIME(sValue3);
    
    /*
     * datepart에 대한 add 연산
     */ 
    
    switch( sDateTimePart )
    {
        case DTL_DATETIME_PART_HOUR:
            {
                STL_TRY_THROW( ( sNum >= DTF_DATETIME_PART_HOUR_MIN_ADD_VALUE ) &&
                               ( sNum <= DTF_DATETIME_PART_HOUR_MAX_ADD_VALUE ),
                               ERROR_OUT_OF_RANGE );
                
                sTimeType = sTimeType + ( sNum * DTL_USECS_PER_HOUR );
                
                break;
            }
        case DTL_DATETIME_PART_MINUTE:
            {
                STL_TRY_THROW( ( sNum >= DTF_DATETIME_PART_MINUTE_MIN_ADD_VALUE ) &&
                               ( sNum <= DTF_DATETIME_PART_MINUTE_MAX_ADD_VALUE ),
                               ERROR_OUT_OF_RANGE );
                
                sTimeType = sTimeType + ( sNum * DTL_USECS_PER_MINUTE );
                
                break;
            }
        case DTL_DATETIME_PART_SECOND:
            {
                STL_TRY_THROW( ( sNum >= DTF_DATETIME_PART_SECOND_MIN_ADD_VALUE ) &&
                               ( sNum <= DTF_DATETIME_PART_SECOND_MAX_ADD_VALUE ),
                               ERROR_OUT_OF_RANGE );
                
                sTimeType = sTimeType + ( sNum * DTL_USECS_PER_SEC );
                
                break;
            }
        case DTL_DATETIME_PART_MILLISECOND:
            {
                STL_TRY_THROW( ( sNum >= DTF_DATETIME_PART_MILLISECOND_MIN_ADD_VALUE ) &&
                               ( sNum <= DTF_DATETIME_PART_MILLISECOND_MAX_ADD_VALUE ),
                               ERROR_OUT_OF_RANGE );
                
                sTimeType = sTimeType + ( sNum * DTL_USECS_PER_MILLISEC );
                
                break;
            }
        case DTL_DATETIME_PART_MICROSECOND:
            {
                STL_TRY_THROW( ( sNum >= DTF_DATETIME_PART_MICROSECOND_MIN_ADD_VALUE ) &&
                               ( sNum <= DTF_DATETIME_PART_MICROSECOND_MAX_ADD_VALUE ),
                               ERROR_OUT_OF_RANGE );
                
                sTimeType = sTimeType + sNum;

                break;
            }
        case DTL_DATETIME_PART_YEAR:
        case DTL_DATETIME_PART_QUARTER:
        case DTL_DATETIME_PART_MONTH:
        case DTL_DATETIME_PART_WEEK:
        case DTL_DATETIME_PART_DAY:
        case DTL_DATETIME_PART_DAYOFYEAR:
        case DTL_DATETIME_PART_WEEKDAY:
            {
                STL_THROW( ERROR_INVALID_DATEPART );                
                break;
            }
        default:
            STL_DASSERT( STL_FALSE );
            break;
    }
    
    /*
     * time 값 설정
     */
    sTimeType = sTimeType - ( sTimeType / DTL_USECS_PER_DAY * DTL_USECS_PER_DAY );
    
    if( sTimeType < 0 )
    {
        sTimeType += DTL_USECS_PER_DAY;
    }
    else
    {
        // Do Nothing
    }
    
    DTF_TIME(sResultValue) = sTimeType;
    sResultValue->mLength  = DTL_TIME_SIZE;
    
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATETIME_FIELD_OVERFLOW,
                      NULL,
                      sErrorStack );
    }

    STL_CATCH( ERROR_INVALID_DATEPART )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_DATEPART_FOR_DATA_TYPE,
                      NULL,
                      sErrorStack,
                      dtlDataTypeName[sValue3->mType] );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief DATEADD( datepart, number, TIME WITH TIME ZONE )
 *        <BR>   DATEADD( datepart, number, TIME WITH TIME ZONE ) => [ mssql ] 
 *        <BR> ex) DATEADD( HOUR, 3, TIME WITH TIME ZONE'11:22:33.123456 +09:00' )
 *        <BR>     => TIME WITH TIME ZONE'14:22:33.123456 +09:00'
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> DATEADD( DTL_TYPE_NUMBER, DTL_TYPE_NUMBER, DTL_TYPE_TIME_WITH_TIME_ZONE )
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 TIME WITH TIME ZONE)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfDateAddForTimeWithTimeZoneAddDatepart( stlUInt16        aInputArgumentCnt,
                                                    dtlValueEntry  * aInputArgument,
                                                    void           * aResultValue,
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
    stlInt64         sNum = 0;
    stlBool          sIsTruncated = STL_FALSE;
    
    dtlTimeType      sTimeType;
    stlInt32         sTimeZone;    
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 3, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sValue3 = aInputArgument[2].mValue.mDataValue;    
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue3, DTL_TYPE_TIME_WITH_TIME_ZONE, sErrorStack );    

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIME_WITH_TIME_ZONE, sErrorStack );

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
     * number ( sValue2 ) : 반올림하지 않는다.
     */
    
    STL_TRY( dtlNumericToInt64( sValue2,
                                & sNum,
                                & sIsTruncated,
                                sErrorStack )
             == STL_SUCCESS );
    
    /*
     * time ( sValue3 ) 
     */
    
    sTimeZone = ((dtlTimeTzType *)(sValue3->mValue))->mTimeZone;
    sTimeType = ((dtlTimeTzType *)(sValue3->mValue))->mTime - ( sTimeZone * DTL_USECS_PER_SEC );    
    
    /*
     * datepart에 대한 add 연산
     */ 
    
    switch( sDateTimePart )
    {
        case DTL_DATETIME_PART_HOUR:
            {
                STL_TRY_THROW( ( sNum >= DTF_DATETIME_PART_HOUR_MIN_ADD_VALUE ) &&
                               ( sNum <= DTF_DATETIME_PART_HOUR_MAX_ADD_VALUE ),
                               ERROR_OUT_OF_RANGE );
                
                sTimeType = sTimeType + ( sNum * DTL_USECS_PER_HOUR );
                
                break;
            }
        case DTL_DATETIME_PART_MINUTE:
            {
                STL_TRY_THROW( ( sNum >= DTF_DATETIME_PART_MINUTE_MIN_ADD_VALUE ) &&
                               ( sNum <= DTF_DATETIME_PART_MINUTE_MAX_ADD_VALUE ),
                               ERROR_OUT_OF_RANGE );
                
                sTimeType = sTimeType + ( sNum * DTL_USECS_PER_MINUTE );
                
                break;
            }
        case DTL_DATETIME_PART_SECOND:
            {
                STL_TRY_THROW( ( sNum >= DTF_DATETIME_PART_SECOND_MIN_ADD_VALUE ) &&
                               ( sNum <= DTF_DATETIME_PART_SECOND_MAX_ADD_VALUE ),
                               ERROR_OUT_OF_RANGE );
                
                sTimeType = sTimeType + ( sNum * DTL_USECS_PER_SEC );
                
                break;
            }
        case DTL_DATETIME_PART_MILLISECOND:
            {
                STL_TRY_THROW( ( sNum >= DTF_DATETIME_PART_MILLISECOND_MIN_ADD_VALUE ) &&
                               ( sNum <= DTF_DATETIME_PART_MILLISECOND_MAX_ADD_VALUE ),
                               ERROR_OUT_OF_RANGE );
                
                sTimeType = sTimeType + ( sNum * DTL_USECS_PER_MILLISEC );
                
                break;
            }
        case DTL_DATETIME_PART_MICROSECOND:
            {
                STL_TRY_THROW( ( sNum >= DTF_DATETIME_PART_MICROSECOND_MIN_ADD_VALUE ) &&
                               ( sNum <= DTF_DATETIME_PART_MICROSECOND_MAX_ADD_VALUE ),
                               ERROR_OUT_OF_RANGE );
                
                sTimeType = sTimeType + sNum;

                break;
            }
        case DTL_DATETIME_PART_YEAR:
        case DTL_DATETIME_PART_QUARTER:
        case DTL_DATETIME_PART_MONTH:
        case DTL_DATETIME_PART_WEEK:
        case DTL_DATETIME_PART_DAY:
        case DTL_DATETIME_PART_DAYOFYEAR:
        case DTL_DATETIME_PART_WEEKDAY:
            {
                STL_THROW( ERROR_INVALID_DATEPART );                
                break;
            }
        default:
            STL_DASSERT( STL_FALSE );
            break;
    }
    
    /*
     * time 값 설정
     */
    sTimeType = sTimeType - ( sTimeType / DTL_USECS_PER_DAY * DTL_USECS_PER_DAY );
    
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
    
    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATETIME_FIELD_OVERFLOW,
                      NULL,
                      sErrorStack );
    }

    STL_CATCH( ERROR_INVALID_DATEPART )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_DATEPART_FOR_DATA_TYPE,
                      NULL,
                      sErrorStack,
                      dtlDataTypeName[sValue3->mType] );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/** @} */
