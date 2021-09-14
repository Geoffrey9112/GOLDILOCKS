/*******************************************************************************
 * zlvTimestampTzToC.c
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

#include <cml.h>
#include <goldilocks.h>
#include <zlDef.h>
#include <zle.h>
#include <zlvSqlToC.h>

/**
 * @file zlvTimestampTzToC.c
 * @brief Gliese API Internal Converting Data from SQL Timestamp With Time Zone to C Data Types Routines.
 */

/**
 * @addtogroup zlvTimestampTzToC
 * @{
 */

/*
 * http://msdn.microsoft.com/en-us/library/windows/desktop/ms712387%28v=VS.85%29.aspx
 */

/*
 * SQL 표준에 없는 타입
 */


/*
 * SQL_TYPE_TIMESTAMP_WITH_TIMEZONE -> SQL_C_CHAR
 */
ZLV_DECLARE_SQL_TO_C( TimestampTz, Char )
{
    stlInt64  sLength = 0;
    stlStatus sRet = STL_FAILURE;
    
    *aReturn = SQL_ERROR;
    
    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );
    
    STL_TRY( dtlToStringDataValue( aDataValue,
                                   DTL_GET_BLOCK_ARG_NUM1( aValueBlockList ),
                                   DTL_GET_BLOCK_ARG_NUM2( aValueBlockList ),
                                   aArdRec->mOctetLength - 1,
                                   aTargetValuePtr,
                                   & sLength,
                                   ZLS_STMT_DT_VECTOR(aStmt),
                                   aStmt,
                                   aErrorStack )
             == STL_SUCCESS );

    ((stlChar*)aTargetValuePtr)[sLength] = '\0';        

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = sLength;
    }

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_FINISH;

    return sRet;
}

/*
 * SQL_TYPE_TIMESTAMP_WITH_TIMEZONE -> SQL_C_WCHAR
 */
ZLV_DECLARE_SQL_TO_C( TimestampTz, Wchar )
{
    stlInt64           sLength;
    stlStatus          sRet = STL_FAILURE;
    dtlUnicodeEncoding sEncoding;
    stlInt64           sAvailableSize = 0;

#ifdef SQL_WCHART_CONVERT
    sEncoding = DTL_UNICODE_UTF32;
#else
    sEncoding = DTL_UNICODE_UTF16;
#endif
    
    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sAvailableSize = aArdRec->mOctetLength - STL_SIZEOF(SQLWCHAR);
    
    STL_TRY( dtlToWCharStringDataValue( sEncoding,
                                        aDataValue,
                                        DTL_GET_BLOCK_ARG_NUM1( aValueBlockList ),
                                        DTL_GET_BLOCK_ARG_NUM2( aValueBlockList ),
                                        sAvailableSize,
                                        aTargetValuePtr,
                                        & sLength,
                                        ZLS_STMT_DT_VECTOR(aStmt),
                                        aStmt,
                                        aErrorStack )
             == STL_SUCCESS );

    *(SQLWCHAR*)((stlChar*)aTargetValuePtr + sLength) = 0;
    
    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = sLength;
    }
    
    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_FINISH;

    return sRet;
}

/*
 * SQL_TYPE_TIMESTAMP_WITH_TIMEZONE -> SQL_C_LONGVARCHAR
 */
ZLV_DECLARE_SQL_TO_C( TimestampTz, LongVarChar )
{
    SQL_LONG_VARIABLE_LENGTH_STRUCT * sTarget;
    stlChar                         * sValue;
    stlInt64                          sBufferLength;

    stlInt64     sLength = 0;
    stlStatus    sRet = STL_FAILURE;

    dtlDateTimeFormatInfo * sDateTimeFormatInfo = NULL;
    
    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget       = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aTargetValuePtr;
    sValue        = (stlChar*)sTarget->arr;
    sBufferLength = sTarget->len;

    sDateTimeFormatInfo = ((ZLS_STMT_DT_VECTOR(aStmt))->mGetTimestampWithTimeZoneFormatInfoFunc(aStmt));
    
    STL_TRY_THROW( sBufferLength > sDateTimeFormatInfo->mToCharMaxResultLen,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
    
    STL_TRY( dtlToStringDataValue( aDataValue,
                                   DTL_GET_BLOCK_ARG_NUM1( aValueBlockList ),
                                   DTL_GET_BLOCK_ARG_NUM2( aValueBlockList ),
                                   sBufferLength,
                                   sValue,
                                   & sLength,
                                   ZLS_STMT_DT_VECTOR(aStmt),
                                   aStmt,
                                   aErrorStack )
             == STL_SUCCESS );
    sValue[sLength] = '\0';        
    
    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = sLength;
    }

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                      "Returning the numeric value as numeric or string for one or "
                      "more bound columns would have caused the whole "
                      "(as opposed to fractional) part of the number to be truncated.",
                      aErrorStack );
    }

    STL_FINISH;

    return sRet;
}

/*
 * SQL_TYPE_TIMESTAMP_WITH_TIMEZONE -> SQL_C_BINARY
 */
ZLV_DECLARE_SQL_TO_C( TimestampTz, Binary )
{
    /*
     * ==========================================================================================
     * Test                                | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ==========================================================================================
     * Byte length of data <= BufferLength | Data            | Length of data in bytes | n/a
     * Byte length of data > BufferLength  | Undefined       | Undefined               | 22003
     * ==========================================================================================
     */
    
    stlStatus sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    STL_TRY_THROW( DTL_TIMESTAMPTZ_SIZE <= aArdRec->mOctetLength,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    stlMemcpy( aTargetValuePtr,
               (const void*)aDataValue->mValue,
               DTL_TIMESTAMPTZ_SIZE );

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = DTL_TIMESTAMPTZ_SIZE;
    }

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                      "Returning the numeric value as numeric or string for one or "
                      "more bound columns would have caused the whole "
                      "(as opposed to fractional) part of the number to be truncated.",
                      aErrorStack );
    }
    
    STL_FINISH;
    
    return sRet;
}

/*
 * SQL_TYPE_TIMESTAMP_WITH_TIMEZONE -> SQL_C_LONGVARBINARY
 */
ZLV_DECLARE_SQL_TO_C( TimestampTz, LongVarBinary )
{
    /*
     * ==========================================================================================
     * Test                                | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ==========================================================================================
     * Byte length of data <= BufferLength | Data            | Length of data in bytes | n/a
     * Byte length of data > BufferLength  | Undefined       | Undefined               | 22003
     * ==========================================================================================
     */
    
    SQL_LONG_VARIABLE_LENGTH_STRUCT * sTarget;
    stlChar                         * sValue;
    stlInt64                          sBufferLength;

    stlStatus sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget       = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aTargetValuePtr;
    sValue        = (stlChar*)sTarget->arr;
    sBufferLength = sTarget->len;

    STL_TRY_THROW( DTL_TIMESTAMPTZ_SIZE <= sBufferLength,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    stlMemcpy( sValue,
               (const void*)aDataValue->mValue,
               DTL_TIMESTAMPTZ_SIZE );

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = DTL_TIMESTAMPTZ_SIZE;
    }

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                      "Returning the numeric value as numeric or string for one or "
                      "more bound columns would have caused the whole "
                      "(as opposed to fractional) part of the number to be truncated.",
                      aErrorStack );
    }
    
    STL_FINISH;
    
    return sRet;
}

/*
 * SQL_TYPE_TIMESTAMP_WITH_TIMEZONE -> SQL_C_TYPE_DATE
 */
ZLV_DECLARE_SQL_TO_C( TimestampTz, Date )
{
    /*
     * =====================================================================================
     * Test                                 | *TargetValuePtr | *StrLen_or_IndPtr | SQLSTATE
     * =====================================================================================
     * Time portion of timestamp is zero    | Data            | 6                 | n/a
     * Time portion of timestamp is nonzero | Truncated data  | 6                 | 01S07
     * =====================================================================================
     */
    
    dtlTimestampTzType * sDataValue;
    SQL_DATE_STRUCT    * sTarget;
    stlInt32             sYear;
    stlInt32             sMonth;
    stlInt32             sDay;
    stlInt32             sHour;
    stlInt32             sMinute;
    stlInt32             sSecond;
    stlInt32             sFraction;
    stlInt32             sTimeZoneSign;
    stlUInt32            sTimeZoneHour;
    stlUInt32            sTiemZoneMinute;
    stlStatus            sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlTimestampTzType*)aDataValue->mValue;
    sTarget    = (SQL_DATE_STRUCT*)aTargetValuePtr;

    STL_TRY( dtlGetTimeInfoFromTimestampTz( sDataValue,
                                            &sYear,
                                            &sMonth,
                                            &sDay,
                                            &sHour,
                                            &sMinute,
                                            &sSecond,
                                            &sFraction,
                                            &sTimeZoneSign,
                                            &sTimeZoneHour,
                                            &sTiemZoneMinute,
                                            aErrorStack ) == STL_SUCCESS );

    sTarget->year  = (stlInt16)sYear;
    sTarget->month = (stlUInt16)sMonth;
    sTarget->day   = (stlUInt16)sDay;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = 6;
    }

    STL_TRY_THROW( ( sHour == 0 ) &&
                   ( sMinute == 0 ) &&
                   ( sSecond == 0 ) &&
                   ( sFraction == 0 ), RAMP_WARNING_FRACTIONAL_TRUNCATION );

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_WARNING_FRACTIONAL_TRUNCATION )
    {
        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_FRACTIONAL_TRUNCATION,
                      "The data returned for a column was truncated. "
                      "For numeric data types, the fractional part of the number was truncated. "
                      "For time, timestamp, and interval data types that contain a time component, "
                      "the fractional part of the time was truncated.",
                      aErrorStack );

        sRet     = STL_SUCCESS;
        *aReturn = SQL_SUCCESS_WITH_INFO;
    }

    STL_FINISH;

    return sRet;
}

/*
 * SQL_TYPE_TIMESTAMP_WITH_TIMEZONE -> SQL_C_TYPE_TIME
 */
ZLV_DECLARE_SQL_TO_C( TimestampTz, Time )
{
    /*
     * ===================================================================================================
     * Test                                               | *TargetValuePtr | *StrLen_or_IndPtr | SQLSTATE
     * ===================================================================================================
     * Fractional seconds portion of timestamp is zero    | Data            | 6                 | n/a
     * Fractional seconds portion of timestamp is nonzero | Truncated data  | 6                 | 01S07
     * ===================================================================================================
     */
    
    dtlTimestampTzType * sDataValue;
    SQL_TIME_STRUCT    * sTarget;
    stlInt32             sYear;
    stlInt32             sMonth;
    stlInt32             sDay;
    stlInt32             sHour;
    stlInt32             sMinute;
    stlInt32             sSecond;
    stlInt32             sFraction;
    stlInt32             sTimeZoneSign;
    stlUInt32            sTimeZoneHour;
    stlUInt32            sTiemZoneMinute;
    stlStatus            sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlTimestampTzType*)aDataValue->mValue;
    sTarget    = (SQL_TIME_STRUCT*)aTargetValuePtr;

    STL_TRY( dtlAdjustTimestampTz( sDataValue,
                                   DTL_TIMESTAMP_MAX_PRECISION,
                                   aErrorStack ) == STL_SUCCESS );

    STL_TRY( dtlGetTimeInfoFromTimestampTz( sDataValue,
                                            &sYear,
                                            &sMonth,
                                            &sDay,
                                            &sHour,
                                            &sMinute,
                                            &sSecond,
                                            &sFraction,
                                            &sTimeZoneSign,
                                            &sTimeZoneHour,
                                            &sTiemZoneMinute,
                                            aErrorStack ) == STL_SUCCESS );

    sTarget->hour   = (stlUInt16)sHour;
    sTarget->minute = (stlUInt16)sMinute;
    sTarget->second = (stlUInt16)sSecond;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = 6;

    }

    STL_TRY_THROW( ( sFraction == 0 ), RAMP_WARNING_FRACTIONAL_TRUNCATION );

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_WARNING_FRACTIONAL_TRUNCATION )
    {
        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_FRACTIONAL_TRUNCATION,
                      "The data returned for a column was truncated. "
                      "For numeric data types, the fractional part of the number was truncated. "
                      "For time, timestamp, and interval data types that contain a time component, "
                      "the fractional part of the time was truncated.",
                      aErrorStack );

        sRet     = STL_SUCCESS;
        *aReturn = SQL_SUCCESS_WITH_INFO;

    }

    STL_FINISH;

    return sRet;
}

/*
 * SQL_TYPE_TIMESTAMP_WITH_TIMEZONE -> SQL_C_TYPE_TIME_WITH_TIMEZONE
 */
ZLV_DECLARE_SQL_TO_C( TimestampTz, TimeTz )
{
    dtlTimestampTzType * sDataValue;
    dtlTimestampTzType   sTimestampTz;
    stlInt32             sYear;
    stlInt32             sMonth;
    stlInt32             sDay;
    stlInt32             sHour;
    stlInt32             sMinute;
    stlInt32             sSecond;
    stlInt32             sFraction;
    stlInt32             sTimeZoneSign;
    stlUInt32            sTimeZoneHour;
    stlUInt32            sTiemZoneMinute;
    stlStatus            sRet = STL_FAILURE;
    
    SQL_TIME_WITH_TIMEZONE_STRUCT * sTarget;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlTimestampTzType*)aDataValue->mValue;
    sTarget    = (SQL_TIME_WITH_TIMEZONE_STRUCT*)aTargetValuePtr;

    sTimestampTz.mTimestamp = sDataValue->mTimestamp;
    sTimestampTz.mTimeZone  = sDataValue->mTimeZone;
    
    STL_TRY( dtlAdjustTimestampTz( sDataValue,
                                   aArdRec->mPrecision,
                                   aErrorStack ) == STL_SUCCESS );

    STL_TRY( dtlGetTimeInfoFromTimestampTz( sDataValue,
                                            &sYear,
                                            &sMonth,
                                            &sDay,
                                            &sHour,
                                            &sMinute,
                                            &sSecond,
                                            &sFraction,
                                            &sTimeZoneSign,
                                            &sTimeZoneHour,
                                            &sTiemZoneMinute,
                                            aErrorStack ) == STL_SUCCESS );

    sTarget->hour   = (stlUInt16)sHour;
    sTarget->minute = (stlUInt16)sMinute;
    sTarget->second = (stlUInt16)sSecond;
    sTarget->fraction = sFraction * 1000;

    if( sTimeZoneSign == 1 )
    {
        sTarget->timezone_hour   = sTimeZoneHour;
        sTarget->timezone_minute = sTiemZoneMinute;
    }
    else
    {
        sTarget->timezone_hour   = -(stlInt32)sTimeZoneHour;
        sTarget->timezone_minute = -(stlInt32)sTiemZoneMinute;
    }

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = 14;
    }

    STL_TRY_THROW( sDataValue->mTimestamp >= sTimestampTz.mTimestamp,
                   RAMP_WARNING_FRACTIONAL_TRUNCATION );

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_WARNING_FRACTIONAL_TRUNCATION )
    {
        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_FRACTIONAL_TRUNCATION,
                      "The data returned for a column was truncated. "
                      "For numeric data types, the fractional part of the number was truncated. "
                      "For time, timestamp, and interval data types that contain a time component, "
                      "the fractional part of the time was truncated.",
                      aErrorStack );

        sRet     = STL_SUCCESS;
        *aReturn = SQL_SUCCESS_WITH_INFO;
    }

    STL_FINISH;

    return sRet;
}

/*
 * SQL_TYPE_TIMESTAMP_WITH_TIMEZONE -> SQL_C_TYPE_TIMESTAMP
 */
ZLV_DECLARE_SQL_TO_C( TimestampTz, Timestamp )
{
    /*
     * =========================================================================================================
     * Test                                                     | *TargetValuePtr | *StrLen_or_IndPtr | SQLSTATE
     * =========================================================================================================
     * Fractional seconds portion of timestamp is not truncated | Data            | 16                | n/a
     * Fractional seconds portion of timestamp is truncated     | Truncated data  | 16                | 01S07
     * =========================================================================================================
     */
    
    dtlTimestampTzType   * sDataValue;
    dtlTimestampTzType     sTimestampTz;
    SQL_TIMESTAMP_STRUCT * sTarget;
    stlInt32               sYear;
    stlInt32               sMonth;
    stlInt32               sDay;
    stlInt32               sHour;
    stlInt32               sMinute;
    stlInt32               sSecond;
    stlInt32               sFraction;
    stlInt32               sTimeZoneSign;
    stlUInt32              sTimeZoneHour;
    stlUInt32              sTiemZoneMinute;
    stlStatus              sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlTimestampTzType*)aDataValue->mValue;
    sTarget    = (SQL_TIMESTAMP_STRUCT*)aTargetValuePtr;

    sTimestampTz.mTimestamp = sDataValue->mTimestamp;
    sTimestampTz.mTimeZone  = sDataValue->mTimeZone;
    
    STL_TRY( dtlAdjustTimestampTz( sDataValue,
                                   aArdRec->mPrecision,
                                   aErrorStack ) == STL_SUCCESS );

    STL_TRY( dtlGetTimeInfoFromTimestampTz( sDataValue,
                                            &sYear,
                                            &sMonth,
                                            &sDay,
                                            &sHour,
                                            &sMinute,
                                            &sSecond,
                                            &sFraction,
                                            &sTimeZoneSign,
                                            &sTimeZoneHour,
                                            &sTiemZoneMinute,
                                            aErrorStack ) == STL_SUCCESS );

    sTarget->year     = (stlInt16)sYear;
    sTarget->month    = (stlUInt16)sMonth;
    sTarget->day      = (stlUInt16)sDay;
    sTarget->hour     = (stlUInt16)sHour;
    sTarget->minute   = (stlUInt16)sMinute;
    sTarget->second   = (stlUInt16)sSecond;
    sTarget->fraction = (stlUInt32)sFraction * 1000;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = 16;
    }

    STL_TRY_THROW( ( sDataValue->mTimestamp >= sTimestampTz.mTimestamp ),
                   RAMP_WARNING_FRACTIONAL_TRUNCATION );

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_WARNING_FRACTIONAL_TRUNCATION )
    {
        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_FRACTIONAL_TRUNCATION,
                      "The data returned for a column was truncated. "
                      "For numeric data types, the fractional part of the number was truncated. "
                      "For time, timestamp, and interval data types that contain a time component, "
                      "the fractional part of the time was truncated.",
                      aErrorStack );

        sRet     = STL_SUCCESS;
        *aReturn = SQL_SUCCESS_WITH_INFO;
    }

    STL_FINISH;

    return sRet;
}

/*
 * SQL_TYPE_TIMESTAMP_WITH_TIMEZONE -> SQL_C_TYPE_TIMESTAMP_WITH_TIMEZONE
 */
ZLV_DECLARE_SQL_TO_C( TimestampTz, TimestampTz )
{
    dtlTimestampTzType * sDataValue;
    dtlTimestampTzType   sTimestampTz;
    stlInt32             sYear;
    stlInt32             sMonth;
    stlInt32             sDay;
    stlInt32             sHour;
    stlInt32             sMinute;
    stlInt32             sSecond;
    stlInt32             sFraction;
    stlInt32             sTimeZoneSign;
    stlUInt32            sTimeZoneHour;
    stlUInt32            sTiemZoneMinute;
    stlStatus            sRet = STL_FAILURE;
    
    SQL_TIMESTAMP_WITH_TIMEZONE_STRUCT * sTarget;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlTimestampTzType*)aDataValue->mValue;
    sTarget    = (SQL_TIMESTAMP_WITH_TIMEZONE_STRUCT*)aTargetValuePtr;

    sTimestampTz.mTimestamp = sDataValue->mTimestamp;
    sTimestampTz.mTimeZone  = sDataValue->mTimeZone;
    
    STL_TRY( dtlAdjustTimestampTz( sDataValue,
                                   aArdRec->mPrecision,
                                   aErrorStack ) == STL_SUCCESS );

    STL_TRY( dtlGetTimeInfoFromTimestampTz( sDataValue,
                                            &sYear,
                                            &sMonth,
                                            &sDay,
                                            &sHour,
                                            &sMinute,
                                            &sSecond,
                                            &sFraction,
                                            &sTimeZoneSign,
                                            &sTimeZoneHour,
                                            &sTiemZoneMinute,
                                            aErrorStack ) == STL_SUCCESS );

    sTarget->year     = (stlInt16)sYear;
    sTarget->month    = (stlUInt16)sMonth;
    sTarget->day      = (stlUInt16)sDay;
    sTarget->hour     = (stlUInt16)sHour;
    sTarget->minute   = (stlUInt16)sMinute;
    sTarget->second   = (stlUInt16)sSecond;
    sTarget->fraction = (stlUInt32)sFraction * 1000;

    if( sTimeZoneSign == 0 )
    {
        /*
         * -
         */
        if( sTimeZoneHour != 0 )
        {
            sTarget->timezone_hour   = -( (SQLSMALLINT)sTimeZoneHour );
            sTarget->timezone_minute = -( (SQLSMALLINT)sTiemZoneMinute );
        }
        else
        {
            sTarget->timezone_hour   = 0;
            sTarget->timezone_minute = -( (SQLSMALLINT)sTiemZoneMinute );
        }
    }
    else
    {
        /*
         * +
         */
        sTarget->timezone_hour   = (SQLSMALLINT)sTimeZoneHour;
        sTarget->timezone_minute = (SQLSMALLINT)sTiemZoneMinute;
    }

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_SIZEOF( SQL_TIMESTAMP_WITH_TIMEZONE_STRUCT );
    }

    STL_TRY_THROW( ( sDataValue->mTimestamp >= sTimestampTz.mTimestamp ),
                   RAMP_WARNING_FRACTIONAL_TRUNCATION );

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_WARNING_FRACTIONAL_TRUNCATION )
    {
        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_FRACTIONAL_TRUNCATION,
                      "The data returned for a column was truncated. "
                      "For numeric data types, the fractional part of the number was truncated. "
                      "For time, timestamp, and interval data types that contain a time component, "
                      "the fractional part of the time was truncated.",
                      aErrorStack );

        sRet     = STL_SUCCESS;
        *aReturn = SQL_SUCCESS_WITH_INFO;
    }

    STL_FINISH;

    return sRet;
}

/** @} */

