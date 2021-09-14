/*******************************************************************************
 * zlvIntervalHourToC.c
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
 * @brief Gliese API Internal Converting Data from SQL Interval Hour to C Data Types Routines.
 */

/**
 * @addtogroup zlvIntervalHourToC
 * @{
 */

/*
 * http://msdn.microsoft.com/en-us/library/windows/desktop/ms713571%28v=VS.85%29.aspx
 */

/*
 * SQL_INTERVAL_HOUR -> SQL_C_INTERVAL_DAY
 */
ZLV_DECLARE_SQL_TO_C( IntervalHour, IntervalDay )
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
 * SQL_INTERVAL_HOUR -> SQL_C_INTERVAL_HOUR
 */
ZLV_DECLARE_SQL_TO_C( IntervalHour, IntervalHour )
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
 * SQL_INTERVAL_HOUR -> SQL_C_INTERVAL_MINUTE
 */
ZLV_DECLARE_SQL_TO_C( IntervalHour, IntervalMinute )
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
    sTarget->intval.day_second.minute   = ( ( sDay * 24 ) + sHour ) * 60;
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
 * SQL_INTERVAL_HOUR -> SQL_C_INTERVAL_SECOND
 */
ZLV_DECLARE_SQL_TO_C( IntervalHour, IntervalSecond )
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
    sTarget->intval.day_second.second   = ( ( sDay * 24 ) + sHour ) * 60 * 60;
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
 * SQL_INTERVAL_HOUR -> SQL_C_INTERVAL_DAY_TO_HOUR
 */
ZLV_DECLARE_SQL_TO_C( IntervalHour, IntervalDayToHour )
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
 * SQL_INTERVAL_HOUR -> SQL_C_INTERVAL_DAY_TO_MINUTE
 */
ZLV_DECLARE_SQL_TO_C( IntervalHour, IntervalDayToMinute )
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
 * SQL_INTERVAL_HOUR -> SQL_C_INTERVAL_DAY_TO_SECOND
 */
ZLV_DECLARE_SQL_TO_C( IntervalHour, IntervalDayToSecond )
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
 * SQL_INTERVAL_HOUR -> SQL_C_INTERVAL_HOUR_TO_MINUTE
 */
ZLV_DECLARE_SQL_TO_C( IntervalHour, IntervalHourToMinute )
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
 * SQL_INTERVAL_HOUR -> SQL_C_INTERVAL_HOUR_TO_SECOND
 */
ZLV_DECLARE_SQL_TO_C( IntervalHour, IntervalHourToSecond )
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
 * SQL_INTERVAL_HOUR -> SQL_C_INTERVAL_MINUTE_TO_SECOND
 */
ZLV_DECLARE_SQL_TO_C( IntervalHour, IntervalMinuteToSecond )
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
    sTarget->intval.day_second.minute   = ( ( sDay * 24 ) + sHour ) * 60;
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
 * SQL_INTERVAL_HOUR -> SQL_C_STINYINT
 */
ZLV_DECLARE_SQL_TO_C( IntervalHour, STinyInt )
{
    /*
     * ================================================================================================
     * Test                                      | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ================================================================================================
     * Interval precision was a single field and | Data            | Size of the C data type | n/a
     * the data was converted without truncation |                 |                         |
     * Interval precision was a single field and | Truncated data  | Length of data          | 01S07
     * truncated fractional                      |                 |                         |
     * Interval precision was a single field and | Truncated data  | Length of data          | 22003 
     * truncated whole                           |                 |                         |
     * Interval precision was not a single field | Undefined       | Size of the C data type | 07006
     * ================================================================================================
     */

    dtlIntervalDayToSecondType * sDataValue;
    stlInt8                    * sTarget;
    stlUInt32                    sDay;
    stlUInt32                    sHour;
    stlUInt32                    sMinute;
    stlUInt32                    sSecond;
    stlUInt32                    sFraction;
    stlBool                      sIsNegative;
    stlInt64                     sTotalHour;
    stlStatus                    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlIntervalDayToSecondType*)aDataValue->mValue;
    sTarget    = (stlInt8*)aTargetValuePtr;

    STL_TRY( dtlGetTimeInfoFromDaySecondInterval( sDataValue,
                                                  &sDay,
                                                  &sHour,
                                                  &sMinute,
                                                  &sSecond,
                                                  &sFraction,
                                                  &sIsNegative,
                                                  aErrorStack )
             == STL_SUCCESS );

    sTotalHour = ( sDay * 24 ) + sHour;

    STL_TRY_THROW( ( sTotalHour >= STL_INT8_MIN ) &&
                   ( sTotalHour <= STL_INT8_MAX ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    if( sIsNegative == STL_FALSE )
    {
        *sTarget = (stlInt8)sTotalHour;
    }
    else
    {
        *sTarget = -(stlInt8)sTotalHour;
    }

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_INT8_SIZE;
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
 * SQL_INTERVAL_HOUR -> SQL_C_UTINYINT
 */
ZLV_DECLARE_SQL_TO_C( IntervalHour, UTinyInt )
{
    /*
     * ================================================================================================
     * Test                                      | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ================================================================================================
     * Interval precision was a single field and | Data            | Size of the C data type | n/a
     * the data was converted without truncation |                 |                         |
     * Interval precision was a single field and | Truncated data  | Length of data          | 01S07
     * truncated fractional                      |                 |                         |
     * Interval precision was a single field and | Truncated data  | Length of data          | 22003 
     * truncated whole                           |                 |                         |
     * Interval precision was not a single field | Undefined       | Size of the C data type | 07006
     * ================================================================================================
     */

    dtlIntervalDayToSecondType * sDataValue;
    stlUInt8                   * sTarget;
    stlUInt32                    sDay;
    stlUInt32                    sHour;
    stlUInt32                    sMinute;
    stlUInt32                    sSecond;
    stlUInt32                    sFraction;
    stlBool                      sIsNegative;
    stlUInt64                    sTotalHour;
    stlStatus                    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlIntervalDayToSecondType*)aDataValue->mValue;
    sTarget    = (stlUInt8*)aTargetValuePtr;

    STL_TRY( dtlGetTimeInfoFromDaySecondInterval( sDataValue,
                                                  &sDay,
                                                  &sHour,
                                                  &sMinute,
                                                  &sSecond,
                                                  &sFraction,
                                                  &sIsNegative,
                                                  aErrorStack )
             == STL_SUCCESS );

    sTotalHour = ( sDay * 24 ) + sHour;

    STL_TRY_THROW( ( sTotalHour >= STL_UINT8_MIN ) &&
                   ( sTotalHour <= STL_UINT8_MAX ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    *sTarget = (stlUInt8)sTotalHour;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_UINT8_SIZE;
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
 * SQL_INTERVAL_HOUR -> SQL_C_SBIGINT
 */
ZLV_DECLARE_SQL_TO_C( IntervalHour, SBigInt )
{
    /*
     * ================================================================================================
     * Test                                      | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ================================================================================================
     * Interval precision was a single field and | Data            | Size of the C data type | n/a
     * the data was converted without truncation |                 |                         |
     * Interval precision was a single field and | Truncated data  | Length of data          | 01S07
     * truncated fractional                      |                 |                         |
     * Interval precision was a single field and | Truncated data  | Length of data          | 22003 
     * truncated whole                           |                 |                         |
     * Interval precision was not a single field | Undefined       | Size of the C data type | 07006
     * ================================================================================================
     */

    dtlIntervalDayToSecondType * sDataValue;
    stlInt64                   * sTarget;
    stlUInt32                    sDay;
    stlUInt32                    sHour;
    stlUInt32                    sMinute;
    stlUInt32                    sSecond;
    stlUInt32                    sFraction;
    stlBool                      sIsNegative;
    stlInt64                     sTotalHour;
    stlStatus                    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlIntervalDayToSecondType*)aDataValue->mValue;
    sTarget    = (stlInt64*)aTargetValuePtr;

    STL_TRY( dtlGetTimeInfoFromDaySecondInterval( sDataValue,
                                                  &sDay,
                                                  &sHour,
                                                  &sMinute,
                                                  &sSecond,
                                                  &sFraction,
                                                  &sIsNegative,
                                                  aErrorStack )
             == STL_SUCCESS );

    sTotalHour = ( sDay * 24 ) + sHour;

    if( sIsNegative == STL_FALSE )
    {
        *sTarget = (stlInt64)sTotalHour;
    }
    else
    {
        *sTarget = -(stlInt64)sTotalHour;
    }

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_INT64_SIZE;
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
 * SQL_INTERVAL_HOUR -> SQL_C_UBIGINT
 */
ZLV_DECLARE_SQL_TO_C( IntervalHour, UBigInt )
{
    /*
     * ================================================================================================
     * Test                                      | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ================================================================================================
     * Interval precision was a single field and | Data            | Size of the C data type | n/a
     * the data was converted without truncation |                 |                         |
     * Interval precision was a single field and | Truncated data  | Length of data          | 01S07
     * truncated fractional                      |                 |                         |
     * Interval precision was a single field and | Truncated data  | Length of data          | 22003 
     * truncated whole                           |                 |                         |
     * Interval precision was not a single field | Undefined       | Size of the C data type | 07006
     * ================================================================================================
     */

    dtlIntervalDayToSecondType * sDataValue;
    stlUInt64                  * sTarget;
    stlUInt32                    sDay;
    stlUInt32                    sHour;
    stlUInt32                    sMinute;
    stlUInt32                    sSecond;
    stlUInt32                    sFraction;
    stlBool                      sIsNegative;
    stlUInt64                    sTotalHour;
    stlStatus                    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlIntervalDayToSecondType*)aDataValue->mValue;
    sTarget    = (stlUInt64*)aTargetValuePtr;

    STL_TRY( dtlGetTimeInfoFromDaySecondInterval( sDataValue,
                                                  &sDay,
                                                  &sHour,
                                                  &sMinute,
                                                  &sSecond,
                                                  &sFraction,
                                                  &sIsNegative,
                                                  aErrorStack )
             == STL_SUCCESS );

    sTotalHour = ( sDay * 24 ) + sHour;

    *sTarget = (stlUInt64)sTotalHour;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_INT64_SIZE;
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
 * SQL_INTERVAL_HOUR -> SQL_C_SSHORT
 */
ZLV_DECLARE_SQL_TO_C( IntervalHour, SShort )
{
    /*
     * ================================================================================================
     * Test                                      | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ================================================================================================
     * Interval precision was a single field and | Data            | Size of the C data type | n/a
     * the data was converted without truncation |                 |                         |
     * Interval precision was a single field and | Truncated data  | Length of data          | 01S07
     * truncated fractional                      |                 |                         |
     * Interval precision was a single field and | Truncated data  | Length of data          | 22003 
     * truncated whole                           |                 |                         |
     * Interval precision was not a single field | Undefined       | Size of the C data type | 07006
     * ================================================================================================
     */

    dtlIntervalDayToSecondType * sDataValue;
    stlInt16                   * sTarget;
    stlUInt32                    sDay;
    stlUInt32                    sHour;
    stlUInt32                    sMinute;
    stlUInt32                    sSecond;
    stlUInt32                    sFraction;
    stlBool                      sIsNegative;
    stlInt64                     sTotalHour;
    stlStatus                    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlIntervalDayToSecondType*)aDataValue->mValue;
    sTarget    = (stlInt16*)aTargetValuePtr;

    STL_TRY( dtlGetTimeInfoFromDaySecondInterval( sDataValue,
                                                  &sDay,
                                                  &sHour,
                                                  &sMinute,
                                                  &sSecond,
                                                  &sFraction,
                                                  &sIsNegative,
                                                  aErrorStack )
             == STL_SUCCESS );

    sTotalHour = ( sDay * 24 ) + sHour;

    STL_TRY_THROW( ( sTotalHour >= STL_INT16_MIN ) &&
                   ( sTotalHour <= STL_INT16_MAX ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    if( sIsNegative == STL_FALSE )
    {
        *sTarget = (stlInt16)sTotalHour;
    }
    else
    {
        *sTarget = -(stlInt16)sTotalHour;
    }

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_INT16_SIZE;
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
 * SQL_INTERVAL_HOUR -> SQL_C_USHORT
 */
ZLV_DECLARE_SQL_TO_C( IntervalHour, UShort )
{
    /*
     * ================================================================================================
     * Test                                      | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ================================================================================================
     * Interval precision was a single field and | Data            | Size of the C data type | n/a
     * the data was converted without truncation |                 |                         |
     * Interval precision was a single field and | Truncated data  | Length of data          | 01S07
     * truncated fractional                      |                 |                         |
     * Interval precision was a single field and | Truncated data  | Length of data          | 22003 
     * truncated whole                           |                 |                         |
     * Interval precision was not a single field | Undefined       | Size of the C data type | 07006
     * ================================================================================================
     */

    dtlIntervalDayToSecondType * sDataValue;
    stlUInt16                  * sTarget;
    stlUInt32                    sDay;
    stlUInt32                    sHour;
    stlUInt32                    sMinute;
    stlUInt32                    sSecond;
    stlUInt32                    sFraction;
    stlBool                      sIsNegative;
    stlUInt64                    sTotalHour;
    stlStatus                    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlIntervalDayToSecondType*)aDataValue->mValue;
    sTarget    = (stlUInt16*)aTargetValuePtr;

    STL_TRY( dtlGetTimeInfoFromDaySecondInterval( sDataValue,
                                                  &sDay,
                                                  &sHour,
                                                  &sMinute,
                                                  &sSecond,
                                                  &sFraction,
                                                  &sIsNegative,
                                                  aErrorStack )
             == STL_SUCCESS );

    sTotalHour = ( sDay * 24 ) + sHour;

    STL_TRY_THROW( ( sTotalHour >= STL_UINT16_MIN ) &&
                   ( sTotalHour <= STL_UINT16_MAX ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    *sTarget = (stlUInt16)sTotalHour;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_UINT16_SIZE;
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
 * SQL_INTERVAL_HOUR -> SQL_C_SLONG
 */
ZLV_DECLARE_SQL_TO_C( IntervalHour, SLong )
{
    /*
     * ================================================================================================
     * Test                                      | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ================================================================================================
     * Interval precision was a single field and | Data            | Size of the C data type | n/a
     * the data was converted without truncation |                 |                         |
     * Interval precision was a single field and | Truncated data  | Length of data          | 01S07
     * truncated fractional                      |                 |                         |
     * Interval precision was a single field and | Truncated data  | Length of data          | 22003 
     * truncated whole                           |                 |                         |
     * Interval precision was not a single field | Undefined       | Size of the C data type | 07006
     * ================================================================================================
     */

    dtlIntervalDayToSecondType * sDataValue;
    stlInt32                   * sTarget;
    stlUInt32                    sDay;
    stlUInt32                    sHour;
    stlUInt32                    sMinute;
    stlUInt32                    sSecond;
    stlUInt32                    sFraction;
    stlBool                      sIsNegative;
    stlInt64                     sTotalHour;
    stlStatus                    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlIntervalDayToSecondType*)aDataValue->mValue;
    sTarget    = (stlInt32*)aTargetValuePtr;

    STL_TRY( dtlGetTimeInfoFromDaySecondInterval( sDataValue,
                                                  &sDay,
                                                  &sHour,
                                                  &sMinute,
                                                  &sSecond,
                                                  &sFraction,
                                                  &sIsNegative,
                                                  aErrorStack )
             == STL_SUCCESS );

    sTotalHour = ( sDay * 24 ) + sHour;

    STL_TRY_THROW( ( sTotalHour >= STL_INT32_MIN ) &&
                   ( sTotalHour <= STL_INT32_MAX ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    if( sIsNegative == STL_FALSE )
    {
        *sTarget = (stlInt32)sTotalHour;
    }
    else
    {
        *sTarget = -(stlInt32)sTotalHour;
    }

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_INT32_SIZE;
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
 * SQL_INTERVAL_HOUR -> SQL_C_ULONG
 */
ZLV_DECLARE_SQL_TO_C( IntervalHour, ULong )
{
    /*
     * ================================================================================================
     * Test                                      | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ================================================================================================
     * Interval precision was a single field and | Data            | Size of the C data type | n/a
     * the data was converted without truncation |                 |                         |
     * Interval precision was a single field and | Truncated data  | Length of data          | 01S07
     * truncated fractional                      |                 |                         |
     * Interval precision was a single field and | Truncated data  | Length of data          | 22003 
     * truncated whole                           |                 |                         |
     * Interval precision was not a single field | Undefined       | Size of the C data type | 07006
     * ================================================================================================
     */

    dtlIntervalDayToSecondType * sDataValue;
    stlUInt32                  * sTarget;
    stlUInt32                    sDay;
    stlUInt32                    sHour;
    stlUInt32                    sMinute;
    stlUInt32                    sSecond;
    stlUInt32                    sFraction;
    stlBool                      sIsNegative;
    stlUInt64                    sTotalHour;
    stlStatus                    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlIntervalDayToSecondType*)aDataValue->mValue;
    sTarget    = (stlUInt32*)aTargetValuePtr;

    STL_TRY( dtlGetTimeInfoFromDaySecondInterval( sDataValue,
                                                  &sDay,
                                                  &sHour,
                                                  &sMinute,
                                                  &sSecond,
                                                  &sFraction,
                                                  &sIsNegative,
                                                  aErrorStack )
             == STL_SUCCESS );

    sTotalHour = ( sDay * 24 ) + sHour;

    STL_TRY_THROW( ( sTotalHour >= STL_UINT32_MIN ) &&
                   ( sTotalHour <= STL_UINT32_MAX ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    *sTarget = (stlUInt32)sTotalHour;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_UINT32_SIZE;
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
 * SQL_INTERVAL_HOUR -> SQL_C_NUMERIC
 */
ZLV_DECLARE_SQL_TO_C( IntervalHour, Numeric )
{
    /*
     * ================================================================================================
     * Test                                      | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ================================================================================================
     * Interval precision was a single field and | Data            | Size of the C data type | n/a
     * the data was converted without truncation |                 |                         |
     * Interval precision was a single field and | Truncated data  | Length of data          | 01S07
     * truncated fractional                      |                 |                         |
     * Interval precision was a single field and | Truncated data  | Length of data          | 22003 
     * truncated whole                           |                 |                         |
     * Interval precision was not a single field | Undefined       | Size of the C data type | 07006
     * ================================================================================================
     */

    dtlIntervalDayToSecondType * sDataValue;
    SQL_NUMERIC_STRUCT         * sTarget;
    stlUInt32                    sDay;
    stlUInt32                    sHour;
    stlUInt32                    sMinute;
    stlUInt32                    sSecond;
    stlUInt32                    sFraction;
    stlBool                      sIsNegative;
    stlUInt64                    sTotalHour;
    stlStatus                    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlIntervalDayToSecondType*)aDataValue->mValue;
    sTarget    = (SQL_NUMERIC_STRUCT*)aTargetValuePtr;

    STL_TRY_THROW( ((ZLV_SQL_TO_C_NUMERIC_MAX_SCALE >= aArdRec->mScale) &&
                    (ZLV_SQL_TO_C_NUMERIC_MIN_SCALE <= aArdRec->mScale)),
                   RAMP_ERR_SCALE_VALUE_OUT_OF_RANGE );

    STL_TRY( dtlGetTimeInfoFromDaySecondInterval( sDataValue,
                                                  &sDay,
                                                  &sHour,
                                                  &sMinute,
                                                  &sSecond,
                                                  &sFraction,
                                                  &sIsNegative,
                                                  aErrorStack )
             == STL_SUCCESS );

    sTotalHour = ( sDay * 24 ) + sHour;

    STL_TRY( zlvMakeCNumericFromInteger( aStmt,
                                         sTarget,
                                         sTotalHour,
                                         aArdRec->mScale,
                                         ZLV_INTERVAL_HOUR_TO_C_NUMERIC_PRECISION,
                                         sIsNegative,
                                         aErrorStack )
             == STL_SUCCESS );

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_SIZEOF( SQL_NUMERIC_STRUCT );
    }

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_ERR_SCALE_VALUE_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                      "Specified scale value is out of range( -128 to 127 )",
                      aErrorStack );
    }

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_FINISH;
    
    return sRet;
}

/*
 * SQL_INTERVAL_HOUR -> SQL_C_BINARY
 */
ZLV_DECLARE_SQL_TO_C( IntervalHour, Binary )
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
 * SQL_INTERVAL_HOUR -> SQL_C_LONGVARBINARY
 */
ZLV_DECLARE_SQL_TO_C( IntervalHour, LongVarBinary )
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
 * SQL_INTERVAL_HOUR -> SQL_C_CHAR
 */
ZLV_DECLARE_SQL_TO_C( IntervalHour, Char )
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

    stlInt64  sLength = 0;
    stlStatus sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sLength = DTL_INTERVAL_DAY_TO_SECOND_DISPLAY_SIZE( 
                  DTL_GET_BLOCK_ARG_NUM1( aValueBlockList ),
                  0,
                  ((dtlIntervalDayToSecondType*)(aDataValue->mValue))->mIndicator );

    STL_TRY_THROW( aArdRec->mOctetLength > sLength,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    STL_TRY( dtlToStringDataValue( aDataValue,
                                   DTL_GET_BLOCK_ARG_NUM1( aValueBlockList ),
                                   0,
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
 * SQL_INTERVAL_HOUR -> SQL_C_WCHAR
 */
ZLV_DECLARE_SQL_TO_C( IntervalHour, Wchar )
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

    stlInt64  sLength = 0;
    stlStatus sRet = STL_FAILURE;
    stlChar   sIntervalString[DTL_INTERVAL_DAY_TO_SECOND_MAX_STRING_SIZE + 1];
    stlInt64  i;
    
    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sLength = DTL_INTERVAL_DAY_TO_SECOND_DISPLAY_SIZE( 
                  DTL_GET_BLOCK_ARG_NUM1( aValueBlockList ),
                  0,
                  ((dtlIntervalDayToSecondType*)(aDataValue->mValue))->mIndicator );

    STL_TRY_THROW( (sLength + 1) * STL_SIZEOF(SQLWCHAR) <= aArdRec->mOctetLength,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    STL_TRY( dtlToStringDataValue( aDataValue,
                                   DTL_GET_BLOCK_ARG_NUM1( aValueBlockList ),
                                   0,
                                   DTL_INTERVAL_DAY_TO_SECOND_MAX_STRING_SIZE,
                                   sIntervalString,
                                   & sLength,
                                   ZLS_STMT_DT_VECTOR(aStmt),
                                   aStmt,
                                   aErrorStack )
             == STL_SUCCESS );

    for( i = 0; i < sLength; i++ )
    {
        ((SQLWCHAR*)aTargetValuePtr)[i] = sIntervalString[i];
    }

    ((SQLWCHAR*)aTargetValuePtr)[i] = 0;
    
    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = sLength * STL_SIZEOF(SQLWCHAR);
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
 * SQL_INTERVAL_HOUR -> SQL_C_LONGVARCHAR
 */
ZLV_DECLARE_SQL_TO_C( IntervalHour, LongVarChar )
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
    stlStatus                         sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget       = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aTargetValuePtr;
    sValue        = (stlChar*)sTarget->arr;
    sBufferLength = sTarget->len;

    sLength = DTL_INTERVAL_DAY_TO_SECOND_DISPLAY_SIZE( 
                  DTL_GET_BLOCK_ARG_NUM1( aValueBlockList ),
                  0,
                  ((dtlIntervalDayToSecondType*)(aDataValue->mValue))->mIndicator );

    STL_TRY_THROW( sBufferLength > sLength,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    STL_TRY( dtlToStringDataValue( aDataValue,
                                   DTL_GET_BLOCK_ARG_NUM1( aValueBlockList ),
                                   0,
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

/** @} */
