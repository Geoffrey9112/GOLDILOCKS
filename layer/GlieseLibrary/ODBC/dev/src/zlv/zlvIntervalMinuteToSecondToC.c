/*******************************************************************************
 * zlvIntervalMinuteToSecondToC.c
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
 * @file zlvCharToC.c
 * @brief Gliese API Internal Converting Data from SQL Interval Minute to Second to C Data Types Routines.
 */

/**
 * @addtogroup zlvIntervalMinuteToSecondToC
 * @{
 */

/*
 * http://msdn.microsoft.com/en-us/library/windows/desktop/ms713571%28v=VS.85%29.aspx
 */

/*
 * SQL_INTERVAL_MINUTE_TO_SECOND -> SQL_C_INTERVAL_DAY
 */
ZLV_DECLARE_SQL_TO_C( IntervalMinuteToSecond, IntervalDay )
{
    /*
     * ==============================================================================================
     * Test                                          | *TargetValuePtr | *StrLen_or_IndPtr | SQLSTATE
     * ==============================================================================================
     * Trailing fields portion not truncated         | Data            | Length of data    | n/a
     * Trailing fields portion truncated             | Truncated data  | Length of data    | 01S07
     * Leading precision of target is not big enough | Undefined       | Undefined         | 22015
     * to hold data from source                      |                 |                   |
     * ==============================================================================================
     */

    dtlIntervalDayToSecondType * sDataValue;
    SQL_INTERVAL_STRUCT        * sTarget;
    stlUInt32                    sDay;
    stlUInt32                    sHour;
    stlUInt32                    sMinute;
    stlUInt32                    sSecond;
    stlUInt32                    sFraction;
    stlBool                      sIsNegative;
    stlBool                      sSuccessWithInfo = STL_FALSE;
    stlStatus                    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlIntervalDayToSecondType*)aDataValue->mValue;
    sTarget    = (SQL_INTERVAL_STRUCT*)aTargetValuePtr;
    stlMemset( sTarget, 0x00, STL_SIZEOF( SQL_INTERVAL_STRUCT ) );

    STL_TRY( dtlGetTimeInfoFromDaySecondInterval( sDataValue,
                                                  &sDay,
                                                  &sHour,
                                                  &sMinute,
                                                  &sSecond,
                                                  &sFraction,
                                                  &sIsNegative,
                                                  aErrorStack )
             == STL_SUCCESS );

    STL_TRY( dtlAdjustIntervalDayToSecond( DTL_INTERVAL_INDICATOR_DAY,
                                           aArdRec->mDatetimeIntervalPrecision,
                                           aArdRec->mPrecision,
                                           sDay,
                                           sHour,
                                           sMinute,
                                           sSecond,
                                           &sFraction,
                                           &sSuccessWithInfo,
                                           aErrorStack )
             == STL_SUCCESS );

    sTarget->interval_type              = SQL_IS_DAY;
    sTarget->interval_sign              = (SQLSMALLINT)sIsNegative;
    sTarget->intval.day_second.day      = sDay;
    sTarget->intval.day_second.hour     = 0;
    sTarget->intval.day_second.minute   = 0;
    sTarget->intval.day_second.second   = 0;
    sTarget->intval.day_second.fraction = 0;
    
    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_SIZEOF( SQL_INTERVAL_STRUCT );
    }

    STL_TRY_THROW( sSuccessWithInfo == STL_FALSE, RAMP_WARNING_FRACTIONAL_TRUNCATION );

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
 * SQL_INTERVAL_MINUTE_TO_SECOND -> SQL_C_INTERVAL_HOUR
 */
ZLV_DECLARE_SQL_TO_C( IntervalMinuteToSecond, IntervalHour )
{
    /*
     * ==============================================================================================
     * Test                                          | *TargetValuePtr | *StrLen_or_IndPtr | SQLSTATE
     * ==============================================================================================
     * Trailing fields portion not truncated         | Data            | Length of data    | n/a
     * Trailing fields portion truncated             | Truncated data  | Length of data    | 01S07
     * Leading precision of target is not big enough | Undefined       | Undefined         | 22015
     * to hold data from source                      |                 |                   |
     * ==============================================================================================
     */

    dtlIntervalDayToSecondType * sDataValue;
    SQL_INTERVAL_STRUCT        * sTarget;
    stlUInt32                    sDay;
    stlUInt32                    sHour;
    stlUInt32                    sMinute;
    stlUInt32                    sSecond;
    stlUInt32                    sFraction;
    stlBool                      sIsNegative;
    stlBool                      sSuccessWithInfo = STL_FALSE;
    stlStatus                    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlIntervalDayToSecondType*)aDataValue->mValue;
    sTarget    = (SQL_INTERVAL_STRUCT*)aTargetValuePtr;
    stlMemset( sTarget, 0x00, STL_SIZEOF( SQL_INTERVAL_STRUCT ) );

    STL_TRY( dtlGetTimeInfoFromDaySecondInterval( sDataValue,
                                                  &sDay,
                                                  &sHour,
                                                  &sMinute,
                                                  &sSecond,
                                                  &sFraction,
                                                  &sIsNegative,
                                                  aErrorStack )
             == STL_SUCCESS );

    STL_TRY( dtlAdjustIntervalDayToSecond( DTL_INTERVAL_INDICATOR_HOUR,
                                           aArdRec->mDatetimeIntervalPrecision,
                                           aArdRec->mPrecision,
                                           sDay,
                                           sHour,
                                           sMinute,
                                           sSecond,
                                           &sFraction,
                                           &sSuccessWithInfo,
                                           aErrorStack )
             == STL_SUCCESS );

    sTarget->interval_type              = SQL_IS_HOUR;
    sTarget->interval_sign              = (SQLSMALLINT)sIsNegative;
    sTarget->intval.day_second.day      = 0;
    sTarget->intval.day_second.hour     = ( sDay * 24 ) + sHour;
    sTarget->intval.day_second.minute   = 0;
    sTarget->intval.day_second.second   = 0;
    sTarget->intval.day_second.fraction = 0;
    
    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_SIZEOF( SQL_INTERVAL_STRUCT );
    }

    STL_TRY_THROW( sSuccessWithInfo == STL_FALSE, RAMP_WARNING_FRACTIONAL_TRUNCATION );

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
 * SQL_INTERVAL_MINUTE_TO_SECOND -> SQL_C_INTERVAL_MINUTE
 */
ZLV_DECLARE_SQL_TO_C( IntervalMinuteToSecond, IntervalMinute )
{
    /*
     * ==============================================================================================
     * Test                                          | *TargetValuePtr | *StrLen_or_IndPtr | SQLSTATE
     * ==============================================================================================
     * Trailing fields portion not truncated         | Data            | Length of data    | n/a
     * Trailing fields portion truncated             | Truncated data  | Length of data    | 01S07
     * Leading precision of target is not big enough | Undefined       | Undefined         | 22015
     * to hold data from source                      |                 |                   |
     * ==============================================================================================
     */

    dtlIntervalDayToSecondType * sDataValue;
    SQL_INTERVAL_STRUCT        * sTarget;
    stlUInt32                    sDay;
    stlUInt32                    sHour;
    stlUInt32                    sMinute;
    stlUInt32                    sSecond;
    stlUInt32                    sFraction;
    stlBool                      sIsNegative;
    stlBool                      sSuccessWithInfo = STL_FALSE;
    stlStatus                    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlIntervalDayToSecondType*)aDataValue->mValue;
    sTarget    = (SQL_INTERVAL_STRUCT*)aTargetValuePtr;
    stlMemset( sTarget, 0x00, STL_SIZEOF( SQL_INTERVAL_STRUCT ) );

    STL_TRY( dtlGetTimeInfoFromDaySecondInterval( sDataValue,
                                                  &sDay,
                                                  &sHour,
                                                  &sMinute,
                                                  &sSecond,
                                                  &sFraction,
                                                  &sIsNegative,
                                                  aErrorStack )
             == STL_SUCCESS );

    STL_TRY( dtlAdjustIntervalDayToSecond( DTL_INTERVAL_INDICATOR_MINUTE,
                                           aArdRec->mDatetimeIntervalPrecision,
                                           aArdRec->mPrecision,
                                           sDay,
                                           sHour,
                                           sMinute,
                                           sSecond,
                                           &sFraction,
                                           &sSuccessWithInfo,
                                           aErrorStack )
             == STL_SUCCESS );

    sTarget->interval_type              = SQL_IS_MINUTE;
    sTarget->interval_sign              = (SQLSMALLINT)sIsNegative;
    sTarget->intval.day_second.day      = 0;
    sTarget->intval.day_second.hour     = 0;
    sTarget->intval.day_second.minute   = ( ( ( sDay * 24 ) + sHour ) * 60 ) + sMinute;
    sTarget->intval.day_second.second   = 0;
    sTarget->intval.day_second.fraction = 0;
    
    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_SIZEOF( SQL_INTERVAL_STRUCT );
    }

    STL_TRY_THROW( sSuccessWithInfo == STL_FALSE, RAMP_WARNING_FRACTIONAL_TRUNCATION );

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
 * SQL_INTERVAL_MINUTE_TO_SECOND -> SQL_C_INTERVAL_SECOND
 */
ZLV_DECLARE_SQL_TO_C( IntervalMinuteToSecond, IntervalSecond )
{
    /*
     * ==============================================================================================
     * Test                                          | *TargetValuePtr | *StrLen_or_IndPtr | SQLSTATE
     * ==============================================================================================
     * Trailing fields portion not truncated         | Data            | Length of data    | n/a
     * Trailing fields portion truncated             | Truncated data  | Length of data    | 01S07
     * Leading precision of target is not big enough | Undefined       | Undefined         | 22015
     * to hold data from source                      |                 |                   |
     * ==============================================================================================
     */

    dtlIntervalDayToSecondType * sDataValue;
    SQL_INTERVAL_STRUCT        * sTarget;
    stlUInt32                    sDay;
    stlUInt32                    sHour;
    stlUInt32                    sMinute;
    stlUInt32                    sSecond;
    stlUInt32                    sFraction;
    stlBool                      sIsNegative;
    stlBool                      sSuccessWithInfo = STL_FALSE;
    stlStatus                    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlIntervalDayToSecondType*)aDataValue->mValue;
    sTarget    = (SQL_INTERVAL_STRUCT*)aTargetValuePtr;
    stlMemset( sTarget, 0x00, STL_SIZEOF( SQL_INTERVAL_STRUCT ) );

    STL_TRY( dtlGetTimeInfoFromDaySecondInterval( sDataValue,
                                                  &sDay,
                                                  &sHour,
                                                  &sMinute,
                                                  &sSecond,
                                                  &sFraction,
                                                  &sIsNegative,
                                                  aErrorStack )
             == STL_SUCCESS );

    STL_TRY( dtlAdjustIntervalDayToSecond( DTL_INTERVAL_INDICATOR_SECOND,
                                           aArdRec->mDatetimeIntervalPrecision,
                                           aArdRec->mPrecision,
                                           sDay,
                                           sHour,
                                           sMinute,
                                           sSecond,
                                           &sFraction,
                                           &sSuccessWithInfo,
                                           aErrorStack )
             == STL_SUCCESS );

    sTarget->interval_type              = SQL_IS_SECOND;
    sTarget->interval_sign              = (SQLSMALLINT)sIsNegative;
    sTarget->intval.day_second.day      = 0;
    sTarget->intval.day_second.hour     = 0;
    sTarget->intval.day_second.minute   = 0;
    sTarget->intval.day_second.second   = ( ( ( ( ( sDay * 24 ) + sHour ) * 60 ) + sMinute ) * 60 ) + sSecond;
    sTarget->intval.day_second.fraction = sFraction * 1000;
    
    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_SIZEOF( SQL_INTERVAL_STRUCT );
    }

    STL_TRY_THROW( sSuccessWithInfo == STL_FALSE, RAMP_WARNING_FRACTIONAL_TRUNCATION );

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
 * SQL_INTERVAL_MINUTE_TO_SECOND -> SQL_C_INTERVAL_DAY_TO_HOUR
 */
ZLV_DECLARE_SQL_TO_C( IntervalMinuteToSecond, IntervalDayToHour )
{
    /*
     * ==============================================================================================
     * Test                                          | *TargetValuePtr | *StrLen_or_IndPtr | SQLSTATE
     * ==============================================================================================
     * Trailing fields portion not truncated         | Data            | Length of data    | n/a
     * Trailing fields portion truncated             | Truncated data  | Length of data    | 01S07
     * Leading precision of target is not big enough | Undefined       | Undefined         | 22015
     * to hold data from source                      |                 |                   |
     * ==============================================================================================
     */

    dtlIntervalDayToSecondType * sDataValue;
    SQL_INTERVAL_STRUCT        * sTarget;
    stlUInt32                    sDay;
    stlUInt32                    sHour;
    stlUInt32                    sMinute;
    stlUInt32                    sSecond;
    stlUInt32                    sFraction;
    stlBool                      sIsNegative;
    stlBool                      sSuccessWithInfo = STL_FALSE;
    stlStatus                    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlIntervalDayToSecondType*)aDataValue->mValue;
    sTarget    = (SQL_INTERVAL_STRUCT*)aTargetValuePtr;
    stlMemset( sTarget, 0x00, STL_SIZEOF( SQL_INTERVAL_STRUCT ) );

    STL_TRY( dtlGetTimeInfoFromDaySecondInterval( sDataValue,
                                                  &sDay,
                                                  &sHour,
                                                  &sMinute,
                                                  &sSecond,
                                                  &sFraction,
                                                  &sIsNegative,
                                                  aErrorStack )
             == STL_SUCCESS );

    STL_TRY( dtlAdjustIntervalDayToSecond( DTL_INTERVAL_INDICATOR_DAY_TO_HOUR,
                                           aArdRec->mDatetimeIntervalPrecision,
                                           aArdRec->mPrecision,
                                           sDay,
                                           sHour,
                                           sMinute,
                                           sSecond,
                                           &sFraction,
                                           &sSuccessWithInfo,
                                           aErrorStack )
             == STL_SUCCESS );

    sTarget->interval_type              = SQL_IS_DAY_TO_HOUR;
    sTarget->interval_sign              = (SQLSMALLINT)sIsNegative;
    sTarget->intval.day_second.day      = sDay;
    sTarget->intval.day_second.hour     = sHour;
    sTarget->intval.day_second.minute   = 0;
    sTarget->intval.day_second.second   = 0;
    sTarget->intval.day_second.fraction = 0;
    
    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_SIZEOF( SQL_INTERVAL_STRUCT );
    }

    STL_TRY_THROW( sSuccessWithInfo == STL_FALSE, RAMP_WARNING_FRACTIONAL_TRUNCATION );

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
 * SQL_INTERVAL_MINUTE_TO_SECOND -> SQL_C_INTERVAL_DAY_TO_MINUTE
 */
ZLV_DECLARE_SQL_TO_C( IntervalMinuteToSecond, IntervalDayToMinute )
{
    /*
     * ==============================================================================================
     * Test                                          | *TargetValuePtr | *StrLen_or_IndPtr | SQLSTATE
     * ==============================================================================================
     * Trailing fields portion not truncated         | Data            | Length of data    | n/a
     * Trailing fields portion truncated             | Truncated data  | Length of data    | 01S07
     * Leading precision of target is not big enough | Undefined       | Undefined         | 22015
     * to hold data from source                      |                 |                   |
     * ==============================================================================================
     */

    dtlIntervalDayToSecondType * sDataValue;
    SQL_INTERVAL_STRUCT        * sTarget;
    stlUInt32                    sDay;
    stlUInt32                    sHour;
    stlUInt32                    sMinute;
    stlUInt32                    sSecond;
    stlUInt32                    sFraction;
    stlBool                      sIsNegative;
    stlBool                      sSuccessWithInfo = STL_FALSE;
    stlStatus                    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlIntervalDayToSecondType*)aDataValue->mValue;
    sTarget    = (SQL_INTERVAL_STRUCT*)aTargetValuePtr;
    stlMemset( sTarget, 0x00, STL_SIZEOF( SQL_INTERVAL_STRUCT ) );

    STL_TRY( dtlGetTimeInfoFromDaySecondInterval( sDataValue,
                                                  &sDay,
                                                  &sHour,
                                                  &sMinute,
                                                  &sSecond,
                                                  &sFraction,
                                                  &sIsNegative,
                                                  aErrorStack )
             == STL_SUCCESS );

    STL_TRY( dtlAdjustIntervalDayToSecond( DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE,
                                           aArdRec->mDatetimeIntervalPrecision,
                                           aArdRec->mPrecision,
                                           sDay,
                                           sHour,
                                           sMinute,
                                           sSecond,
                                           &sFraction,
                                           &sSuccessWithInfo,
                                           aErrorStack )
             == STL_SUCCESS );

    sTarget->interval_type              = SQL_IS_DAY_TO_MINUTE;
    sTarget->interval_sign              = (SQLSMALLINT)sIsNegative;
    sTarget->intval.day_second.day      = sDay;
    sTarget->intval.day_second.hour     = sHour;
    sTarget->intval.day_second.minute   = sMinute;
    sTarget->intval.day_second.second   = 0;
    sTarget->intval.day_second.fraction = 0;
    
    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_SIZEOF( SQL_INTERVAL_STRUCT );
    }

    STL_TRY_THROW( sSuccessWithInfo == STL_FALSE, RAMP_WARNING_FRACTIONAL_TRUNCATION );

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
 * SQL_INTERVAL_MINUTE_TO_SECOND -> SQL_C_INTERVAL_DAY_TO_SECOND
 */
ZLV_DECLARE_SQL_TO_C( IntervalMinuteToSecond, IntervalDayToSecond )
{
    /*
     * ==============================================================================================
     * Test                                          | *TargetValuePtr | *StrLen_or_IndPtr | SQLSTATE
     * ==============================================================================================
     * Trailing fields portion not truncated         | Data            | Length of data    | n/a
     * Trailing fields portion truncated             | Truncated data  | Length of data    | 01S07
     * Leading precision of target is not big enough | Undefined       | Undefined         | 22015
     * to hold data from source                      |                 |                   |
     * ==============================================================================================
     */

    dtlIntervalDayToSecondType * sDataValue;
    SQL_INTERVAL_STRUCT        * sTarget;
    stlUInt32                    sDay;
    stlUInt32                    sHour;
    stlUInt32                    sMinute;
    stlUInt32                    sSecond;
    stlUInt32                    sFraction;
    stlBool                      sIsNegative;
    stlBool                      sSuccessWithInfo = STL_FALSE;
    stlStatus                    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlIntervalDayToSecondType*)aDataValue->mValue;
    sTarget    = (SQL_INTERVAL_STRUCT*)aTargetValuePtr;
    stlMemset( sTarget, 0x00, STL_SIZEOF( SQL_INTERVAL_STRUCT ) );

    STL_TRY( dtlGetTimeInfoFromDaySecondInterval( sDataValue,
                                                  &sDay,
                                                  &sHour,
                                                  &sMinute,
                                                  &sSecond,
                                                  &sFraction,
                                                  &sIsNegative,
                                                  aErrorStack )
             == STL_SUCCESS );

    STL_TRY( dtlAdjustIntervalDayToSecond( DTL_INTERVAL_INDICATOR_DAY_TO_SECOND,
                                           aArdRec->mDatetimeIntervalPrecision,
                                           aArdRec->mPrecision,
                                           sDay,
                                           sHour,
                                           sMinute,
                                           sSecond,
                                           &sFraction,
                                           &sSuccessWithInfo,
                                           aErrorStack )
             == STL_SUCCESS );

    sTarget->interval_type              = SQL_IS_DAY_TO_SECOND;
    sTarget->interval_sign              = (SQLSMALLINT)sIsNegative;
    sTarget->intval.day_second.day      = sDay;
    sTarget->intval.day_second.hour     = sHour;
    sTarget->intval.day_second.minute   = sMinute;
    sTarget->intval.day_second.second   = sSecond;
    sTarget->intval.day_second.fraction = sFraction * 1000;
    
    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_SIZEOF( SQL_INTERVAL_STRUCT );
    }

    STL_TRY_THROW( sSuccessWithInfo == STL_FALSE, RAMP_WARNING_FRACTIONAL_TRUNCATION );

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
 * SQL_INTERVAL_MINUTE_TO_SECOND -> SQL_C_INTERVAL_HOUR_TO_MINUTE
 */
ZLV_DECLARE_SQL_TO_C( IntervalMinuteToSecond, IntervalHourToMinute )
{
    /*
     * ==============================================================================================
     * Test                                          | *TargetValuePtr | *StrLen_or_IndPtr | SQLSTATE
     * ==============================================================================================
     * Trailing fields portion not truncated         | Data            | Length of data    | n/a
     * Trailing fields portion truncated             | Truncated data  | Length of data    | 01S07
     * Leading precision of target is not big enough | Undefined       | Undefined         | 22015
     * to hold data from source                      |                 |                   |
     * ==============================================================================================
     */

    dtlIntervalDayToSecondType * sDataValue;
    SQL_INTERVAL_STRUCT        * sTarget;
    stlUInt32                    sDay;
    stlUInt32                    sHour;
    stlUInt32                    sMinute;
    stlUInt32                    sSecond;
    stlUInt32                    sFraction;
    stlBool                      sIsNegative;
    stlBool                      sSuccessWithInfo = STL_FALSE;
    stlStatus                    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlIntervalDayToSecondType*)aDataValue->mValue;
    sTarget    = (SQL_INTERVAL_STRUCT*)aTargetValuePtr;
    stlMemset( sTarget, 0x00, STL_SIZEOF( SQL_INTERVAL_STRUCT ) );

    STL_TRY( dtlGetTimeInfoFromDaySecondInterval( sDataValue,
                                                  &sDay,
                                                  &sHour,
                                                  &sMinute,
                                                  &sSecond,
                                                  &sFraction,
                                                  &sIsNegative,
                                                  aErrorStack )
             == STL_SUCCESS );

    STL_TRY( dtlAdjustIntervalDayToSecond( DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE,
                                           aArdRec->mDatetimeIntervalPrecision,
                                           aArdRec->mPrecision,
                                           sDay,
                                           sHour,
                                           sMinute,
                                           sSecond,
                                           &sFraction,
                                           &sSuccessWithInfo,
                                           aErrorStack )
             == STL_SUCCESS );

    sTarget->interval_type              = SQL_IS_HOUR_TO_MINUTE;
    sTarget->interval_sign              = (SQLSMALLINT)sIsNegative;
    sTarget->intval.day_second.day      = 0;
    sTarget->intval.day_second.hour     = ( sDay * 24 ) + sHour;
    sTarget->intval.day_second.minute   = sMinute;
    sTarget->intval.day_second.second   = 0;
    sTarget->intval.day_second.fraction = 0;
    
    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_SIZEOF( SQL_INTERVAL_STRUCT );
    }

    STL_TRY_THROW( sSuccessWithInfo == STL_FALSE, RAMP_WARNING_FRACTIONAL_TRUNCATION );

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
 * SQL_INTERVAL_MINUTE_TO_SECOND -> SQL_C_INTERVAL_HOUR_TO_SECOND
 */
ZLV_DECLARE_SQL_TO_C( IntervalMinuteToSecond, IntervalHourToSecond )
{
    /*
     * ==============================================================================================
     * Test                                          | *TargetValuePtr | *StrLen_or_IndPtr | SQLSTATE
     * ==============================================================================================
     * Trailing fields portion not truncated         | Data            | Length of data    | n/a
     * Trailing fields portion truncated             | Truncated data  | Length of data    | 01S07
     * Leading precision of target is not big enough | Undefined       | Undefined         | 22015
     * to hold data from source                      |                 |                   |
     * ==============================================================================================
     */

    dtlIntervalDayToSecondType * sDataValue;
    SQL_INTERVAL_STRUCT        * sTarget;
    stlUInt32                    sDay;
    stlUInt32                    sHour;
    stlUInt32                    sMinute;
    stlUInt32                    sSecond;
    stlUInt32                    sFraction;
    stlBool                      sIsNegative;
    stlBool                      sSuccessWithInfo = STL_FALSE;
    stlStatus                    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlIntervalDayToSecondType*)aDataValue->mValue;
    sTarget    = (SQL_INTERVAL_STRUCT*)aTargetValuePtr;
    stlMemset( sTarget, 0x00, STL_SIZEOF( SQL_INTERVAL_STRUCT ) );

    STL_TRY( dtlGetTimeInfoFromDaySecondInterval( sDataValue,
                                                  &sDay,
                                                  &sHour,
                                                  &sMinute,
                                                  &sSecond,
                                                  &sFraction,
                                                  &sIsNegative,
                                                  aErrorStack )
             == STL_SUCCESS );

    STL_TRY( dtlAdjustIntervalDayToSecond( DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND,
                                           aArdRec->mDatetimeIntervalPrecision,
                                           aArdRec->mPrecision,
                                           sDay,
                                           sHour,
                                           sMinute,
                                           sSecond,
                                           &sFraction,
                                           &sSuccessWithInfo,
                                           aErrorStack )
             == STL_SUCCESS );

    sTarget->interval_type              = SQL_IS_HOUR_TO_SECOND;
    sTarget->interval_sign              = (SQLSMALLINT)sIsNegative;
    sTarget->intval.day_second.day      = 0;
    sTarget->intval.day_second.hour     = ( sDay * 24 ) + sHour;
    sTarget->intval.day_second.minute   = sMinute;
    sTarget->intval.day_second.second   = sSecond;
    sTarget->intval.day_second.fraction = sFraction * 1000;
    
    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_SIZEOF( SQL_INTERVAL_STRUCT );
    }

    STL_TRY_THROW( sSuccessWithInfo == STL_FALSE, RAMP_WARNING_FRACTIONAL_TRUNCATION );

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
 * SQL_INTERVAL_MINUTE_TO_SECOND -> SQL_C_INTERVAL_MINUTE_TO_SECOND
 */
ZLV_DECLARE_SQL_TO_C( IntervalMinuteToSecond, IntervalMinuteToSecond )
{
    /*
     * ==============================================================================================
     * Test                                          | *TargetValuePtr | *StrLen_or_IndPtr | SQLSTATE
     * ==============================================================================================
     * Trailing fields portion not truncated         | Data            | Length of data    | n/a
     * Trailing fields portion truncated             | Truncated data  | Length of data    | 01S07
     * Leading precision of target is not big enough | Undefined       | Undefined         | 22015
     * to hold data from source                      |                 |                   |
     * ==============================================================================================
     */

    dtlIntervalDayToSecondType * sDataValue;
    SQL_INTERVAL_STRUCT        * sTarget;
    stlUInt32                    sDay;
    stlUInt32                    sHour;
    stlUInt32                    sMinute;
    stlUInt32                    sSecond;
    stlUInt32                    sFraction;
    stlBool                      sIsNegative;
    stlBool                      sSuccessWithInfo = STL_FALSE;
    stlStatus                    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlIntervalDayToSecondType*)aDataValue->mValue;
    sTarget    = (SQL_INTERVAL_STRUCT*)aTargetValuePtr;
    stlMemset( sTarget, 0x00, STL_SIZEOF( SQL_INTERVAL_STRUCT ) );

    STL_TRY( dtlGetTimeInfoFromDaySecondInterval( sDataValue,
                                                  &sDay,
                                                  &sHour,
                                                  &sMinute,
                                                  &sSecond,
                                                  &sFraction,
                                                  &sIsNegative,
                                                  aErrorStack )
             == STL_SUCCESS );

    STL_TRY( dtlAdjustIntervalDayToSecond( DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND,
                                           aArdRec->mDatetimeIntervalPrecision,
                                           aArdRec->mPrecision,
                                           sDay,
                                           sHour,
                                           sMinute,
                                           sSecond,
                                           &sFraction,
                                           &sSuccessWithInfo,
                                           aErrorStack )
             == STL_SUCCESS );

    sTarget->interval_type              = SQL_IS_MINUTE_TO_SECOND;
    sTarget->interval_sign              = (SQLSMALLINT)sIsNegative;
    sTarget->intval.day_second.day      = 0;
    sTarget->intval.day_second.hour     = 0;
    sTarget->intval.day_second.minute   = ( ( ( sDay * 24 ) + sHour ) * 60 ) + sMinute;
    sTarget->intval.day_second.second   = sSecond;
    sTarget->intval.day_second.fraction = sFraction * 1000;
    
    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_SIZEOF( SQL_INTERVAL_STRUCT );
    }

    STL_TRY_THROW( sSuccessWithInfo == STL_FALSE, RAMP_WARNING_FRACTIONAL_TRUNCATION );

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
 * SQL_INTERVAL_MINUTE_TO_SECOND -> SQL_C_BINARY
 */
ZLV_DECLARE_SQL_TO_C( IntervalMinuteToSecond, Binary )
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

    STL_TRY_THROW( DTL_INTERVAL_DAY_TO_SECOND_SIZE <= aArdRec->mOctetLength,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    stlMemcpy( aTargetValuePtr,
               (const void*)aDataValue->mValue,
               DTL_INTERVAL_DAY_TO_SECOND_SIZE );

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = DTL_INTERVAL_DAY_TO_SECOND_SIZE;
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
 * SQL_INTERVAL_MINUTE_TO_SECOND -> SQL_C_LONGVARBINARY
 */
ZLV_DECLARE_SQL_TO_C( IntervalMinuteToSecond, LongVarBinary )
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

    STL_TRY_THROW( DTL_INTERVAL_DAY_TO_SECOND_SIZE <= sBufferLength,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    stlMemcpy( sValue,
               (const void*)aDataValue->mValue,
               DTL_INTERVAL_DAY_TO_SECOND_SIZE );

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = DTL_INTERVAL_DAY_TO_SECOND_SIZE;
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
 * SQL_INTERVAL_MINUTE_TO_SECOND -> SQL_C_CHAR
 */
ZLV_DECLARE_SQL_TO_C( IntervalMinuteToSecond, Char )
{
    /*
     * ========================================================================================================================
     * Test                                                              | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ========================================================================================================================
     * Character byte length < BufferLength                              | Data            | Size of the C data type | n/a
     * Number of whole (as opposed to fractional) digits < BufferLength  | Truncated data  | Size of the C data type | 01004
     * Number of whole (as opposed to fractional) digits >= BufferLength | Undefined       | Undefined               | 22003
     * ========================================================================================================================
     */

    stlInt64  sLength        = 0;
    stlInt32  sTimeLength    = 0;
    stlStatus sRet           = STL_FAILURE;
    stlInt64  sAvailableSize = 0;
    stlChar   sIntervalString[DTL_INTERVAL_DAY_TO_SECOND_MAX_DISPLAY_SIZE + 1];

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTimeLength = DTL_INTERVAL_DAY_TO_SECOND_DISPLAY_SIZE( 
                      DTL_GET_BLOCK_ARG_NUM1( aValueBlockList ),
                      0,
                      ((dtlIntervalDayToSecondType*)(aDataValue->mValue))->mIndicator );

    STL_TRY_THROW( aArdRec->mOctetLength > sTimeLength,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    sLength = DTL_INTERVAL_DAY_TO_SECOND_DISPLAY_SIZE( 
                  DTL_GET_BLOCK_ARG_NUM1( aValueBlockList ),
                  DTL_GET_BLOCK_ARG_NUM2( aValueBlockList ),
                  ((dtlIntervalDayToSecondType*)(aDataValue->mValue))->mIndicator );

    sAvailableSize = aArdRec->mOctetLength - STL_SIZEOF(SQLCHAR);

    if( sAvailableSize > sLength )
    {
        STL_TRY( dtlToStringDataValue( aDataValue,
                                       DTL_GET_BLOCK_ARG_NUM1( aValueBlockList ),
                                       DTL_GET_BLOCK_ARG_NUM2( aValueBlockList ),
                                       sAvailableSize,
                                       aTargetValuePtr,
                                       & sLength,
                                       ZLS_STMT_DT_VECTOR(aStmt),
                                       aStmt,
                                       aErrorStack )
                 == STL_SUCCESS );

        ((stlChar*)aTargetValuePtr)[sLength] = '\0';
    }
    else
    {
        STL_TRY( dtlToStringDataValue( aDataValue,
                                       DTL_GET_BLOCK_ARG_NUM1( aValueBlockList ),
                                       DTL_GET_BLOCK_ARG_NUM2( aValueBlockList ),
                                       DTL_INTERVAL_DAY_TO_SECOND_MAX_DISPLAY_SIZE,
                                       sIntervalString,
                                       & sLength,
                                       ZLS_STMT_DT_VECTOR(aStmt),
                                       aStmt,
                                       aErrorStack )
                 == STL_SUCCESS );

        stlMemcpy( aTargetValuePtr, sIntervalString, sAvailableSize );

        ((stlChar*)aTargetValuePtr)[sAvailableSize] = '\0';
    }

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = sLength;
    }

    STL_DASSERT( sLength == DTL_INTERVAL_DAY_TO_SECOND_DISPLAY_SIZE( 
                                DTL_GET_BLOCK_ARG_NUM1( aValueBlockList ),
                                DTL_GET_BLOCK_ARG_NUM2( aValueBlockList ),
                                ((dtlIntervalDayToSecondType*)(aDataValue->mValue))->mIndicator ) );

    STL_TRY_THROW( aArdRec->mOctetLength > sLength,
                   RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED );

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

    STL_CATCH( RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED )
    {
        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_STRING_DATA_RIGHT_TRUNCATED,
                      "String or binary data returned for a column resulted "
                      "in the truncation of nonblank character or non-NULL binary data. "
                      "If it was a string value, it was right-truncated.",
                      aErrorStack );

        sRet     = STL_SUCCESS;
        *aReturn = SQL_SUCCESS_WITH_INFO;
    }

    STL_FINISH;

    return sRet;
}

/*
 * SQL_INTERVAL_MINUTE_TO_SECOND -> SQL_C_WCHAR
 */
ZLV_DECLARE_SQL_TO_C( IntervalMinuteToSecond, Wchar )
{
    /*
     * ========================================================================================================================
     * Test                                                              | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ========================================================================================================================
     * Character byte length < BufferLength                              | Data            | Size of the C data type | n/a
     * Number of whole (as opposed to fractional) digits < BufferLength  | Truncated data  | Size of the C data type | 01004
     * Number of whole (as opposed to fractional) digits >= BufferLength | Undefined       | Undefined               | 22003
     * ========================================================================================================================
     */

    stlInt64  sLength        = 0;
    stlInt32  sTimeLength    = 0;
    stlStatus sRet           = STL_FAILURE;
    stlInt64  sAvailableSize = 0;
    stlChar   sIntervalString[DTL_INTERVAL_DAY_TO_SECOND_MAX_DISPLAY_SIZE + 1];
    stlInt64  i;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTimeLength = DTL_INTERVAL_DAY_TO_SECOND_DISPLAY_SIZE( 
                      DTL_GET_BLOCK_ARG_NUM1( aValueBlockList ),
                      0,
                      ((dtlIntervalDayToSecondType*)(aDataValue->mValue))->mIndicator );

    STL_TRY_THROW( (sTimeLength + 1) * STL_SIZEOF(SQLWCHAR) <= aArdRec->mOctetLength,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    STL_TRY( dtlToStringDataValue( aDataValue,
                                   DTL_GET_BLOCK_ARG_NUM1( aValueBlockList ),
                                   DTL_GET_BLOCK_ARG_NUM2( aValueBlockList ),
                                   DTL_INTERVAL_DAY_TO_SECOND_MAX_DISPLAY_SIZE,
                                   sIntervalString,
                                   & sLength,
                                   ZLS_STMT_DT_VECTOR(aStmt),
                                   aStmt,
                                   aErrorStack )
             == STL_SUCCESS );
    
    STL_DASSERT( sLength == DTL_INTERVAL_DAY_TO_SECOND_DISPLAY_SIZE( 
                                DTL_GET_BLOCK_ARG_NUM1( aValueBlockList ),
                                DTL_GET_BLOCK_ARG_NUM2( aValueBlockList ),
                                ((dtlIntervalDayToSecondType*)(aDataValue->mValue))->mIndicator) );

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = sLength * STL_SIZEOF(SQLWCHAR);
    }

    sAvailableSize = (aArdRec->mOctetLength - STL_SIZEOF(SQLWCHAR)) / STL_SIZEOF(SQLWCHAR);

    if( sAvailableSize >= sLength )
    {
        for( i = 0; i < sLength; i++ )
        {
            ((SQLWCHAR*)aTargetValuePtr)[i] = sIntervalString[i];
        }

        ((SQLWCHAR*)aTargetValuePtr)[i] = 0;
    }
    else
    {
        for( i = 0; i < sAvailableSize; i++ )
        {
            ((SQLWCHAR*)aTargetValuePtr)[i] = sIntervalString[i];
        }

        ((SQLWCHAR*)aTargetValuePtr)[i] = 0;

        STL_THROW( RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED );
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

    STL_CATCH( RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED )
    {
        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_STRING_DATA_RIGHT_TRUNCATED,
                      "String or binary data returned for a column resulted "
                      "in the truncation of nonblank character or non-NULL binary data. "
                      "If it was a string value, it was right-truncated.",
                      aErrorStack );

        sRet     = STL_SUCCESS;
        *aReturn = SQL_SUCCESS_WITH_INFO;
    }

    STL_FINISH;

    return sRet;
}

/*
 * SQL_INTERVAL_MINUTE_TO_SECOND -> SQL_C_LONGVARCHAR
 */
ZLV_DECLARE_SQL_TO_C( IntervalMinuteToSecond, LongVarChar )
{
    /*
     * ========================================================================================================================
     * Test                                                              | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ========================================================================================================================
     * Character byte length < BufferLength                              | Data            | Size of the C data type | n/a
     * Number of whole (as opposed to fractional) digits < BufferLength  | Truncated data  | Size of the C data type | 01004
     * Number of whole (as opposed to fractional) digits >= BufferLength | Undefined       | Undefined               | 22003
     * ========================================================================================================================
     */

    SQL_LONG_VARIABLE_LENGTH_STRUCT * sTarget;
    stlChar                         * sValue;
    stlInt64                          sBufferLength;

    stlInt64                          sLength = 0;
    stlInt32                          sTimeLength = 0;
    stlStatus                         sRet = STL_FAILURE;

    stlChar  sIntervalString[DTL_INTERVAL_DAY_TO_SECOND_MAX_DISPLAY_SIZE];

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget       = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aTargetValuePtr;
    sValue        = (stlChar*)sTarget->arr;
    sBufferLength = sTarget->len;

    sTimeLength = DTL_INTERVAL_DAY_TO_SECOND_DISPLAY_SIZE( 
                      DTL_GET_BLOCK_ARG_NUM1( aValueBlockList ),
                      0,
                      ((dtlIntervalDayToSecondType*)(aDataValue->mValue))->mIndicator );

    STL_TRY_THROW( sBufferLength > sTimeLength,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    sLength = DTL_INTERVAL_DAY_TO_SECOND_DISPLAY_SIZE( 
                  DTL_GET_BLOCK_ARG_NUM1( aValueBlockList ),
                  DTL_GET_BLOCK_ARG_NUM2( aValueBlockList ),
                  ((dtlIntervalDayToSecondType*)(aDataValue->mValue))->mIndicator );

    if( sBufferLength > sLength )
    {
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
    }
    else
    {
        STL_TRY( dtlToStringDataValue( aDataValue,
                                       DTL_GET_BLOCK_ARG_NUM1( aValueBlockList ),
                                       DTL_GET_BLOCK_ARG_NUM2( aValueBlockList ),
                                       DTL_INTERVAL_DAY_TO_SECOND_MAX_DISPLAY_SIZE,
                                       sIntervalString,
                                       & sLength,
                                       ZLS_STMT_DT_VECTOR(aStmt),
                                       aStmt,
                                       aErrorStack )
                 == STL_SUCCESS );

        stlMemcpy( sValue, sIntervalString, (sBufferLength - 1) );

        sValue[sBufferLength - 1] = '\0';        
    }

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = sLength;
    }

    STL_DASSERT( sLength == DTL_INTERVAL_DAY_TO_SECOND_DISPLAY_SIZE( 
                                DTL_GET_BLOCK_ARG_NUM1( aValueBlockList ),
                                DTL_GET_BLOCK_ARG_NUM2( aValueBlockList ),
                                ((dtlIntervalDayToSecondType*)(aDataValue->mValue))->mIndicator ) );

    STL_TRY_THROW( sBufferLength > sLength,
                   RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED );

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

    STL_CATCH( RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED )
    {
        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_STRING_DATA_RIGHT_TRUNCATED,
                      "String or binary data returned for a column resulted "
                      "in the truncation of nonblank character or non-NULL binary data. "
                      "If it was a string value, it was right-truncated.",
                      aErrorStack );

        sRet     = STL_SUCCESS;
        *aReturn = SQL_SUCCESS_WITH_INFO;
    }

    STL_FINISH;

    return sRet;
}

/** @} */
