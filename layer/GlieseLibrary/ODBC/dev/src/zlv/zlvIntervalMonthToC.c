/*******************************************************************************
 * zlvIntervalMonthToC.c
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
 * @brief Gliese API Internal Converting Data from SQL Interval Month to C Data Types Routines.
 */

/**
 * @addtogroup zlvIntervalMonthToC
 * @{
 */

/*
 * http://msdn.microsoft.com/en-us/library/windows/desktop/ms709456%28v=VS.85%29.aspx
 */

/*
 * SQL_INTERVAL_MONTH -> SQL_C_INTERVAL_MONTH
 */
ZLV_DECLARE_SQL_TO_C( IntervalMonth, IntervalMonth )
{
    /*
     * ====================================================================================================
     * Test                                          | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ====================================================================================================
     * Trailing fields portion not truncated         | Data            | Length of data in bytes | n/a
     * Trailing fields portion truncated             | Truncated data  | Length of data in bytes | 01S07
     * Leading precision of target is not big enough | Undefined       | Undefined               | 22015
     * to hold data from source                      |                 |                         |
     * ====================================================================================================
     */

    dtlIntervalYearToMonthType * sDataValue;
    SQL_INTERVAL_STRUCT        * sTarget;
    stlUInt32                    sYear;
    stlUInt32                    sMonth;
    stlBool                      sIsNegative;
    stlBool                      sSuccessWithInfo = STL_FALSE;    
    stlStatus                    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlIntervalYearToMonthType*)aDataValue->mValue;
    sTarget    = (SQL_INTERVAL_STRUCT*)aTargetValuePtr;
    stlMemset( sTarget, 0x00, STL_SIZEOF( SQL_INTERVAL_STRUCT ) );

    STL_TRY( dtlGetTimeInfoFromYearMonthInterval( sDataValue,
                                                  &sYear,
                                                  &sMonth,
                                                  &sIsNegative,
                                                  aErrorStack )
             == STL_SUCCESS );

    STL_TRY( dtlAdjustIntervalYearToMonth( DTL_INTERVAL_INDICATOR_MONTH,
                                           aArdRec->mDatetimeIntervalPrecision,
                                           sYear,
                                           sMonth,
                                           &sSuccessWithInfo,
                                           aErrorStack )
             == STL_SUCCESS );

    sTarget->interval_type           = SQL_IS_MONTH;
    sTarget->interval_sign           = (SQLSMALLINT)sIsNegative;
    sTarget->intval.year_month.year  = 0;
    sTarget->intval.year_month.month = ( sYear * 12 ) + sMonth;
    
    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_SIZEOF( SQL_INTERVAL_STRUCT );
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
 * SQL_INTERVAL_MONTH -> SQL_C_INTERVAL_YEAR
 */
ZLV_DECLARE_SQL_TO_C( IntervalMonth, IntervalYear )
{
    /*
     * ====================================================================================================
     * Test                                          | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ====================================================================================================
     * Trailing fields portion not truncated         | Data            | Length of data in bytes | n/a
     * Trailing fields portion truncated             | Truncated data  | Length of data in bytes | 01S07
     * Leading precision of target is not big enough | Undefined       | Undefined               | 22015
     * to hold data from source                      |                 |                         |
     * ====================================================================================================
     */

    dtlIntervalYearToMonthType * sDataValue;
    SQL_INTERVAL_STRUCT        * sTarget;
    stlUInt32                    sYear;
    stlUInt32                    sMonth;
    stlBool                      sIsNegative;
    stlBool                      sSuccessWithInfo = STL_FALSE;        
    stlStatus                    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlIntervalYearToMonthType*)aDataValue->mValue;
    sTarget    = (SQL_INTERVAL_STRUCT*)aTargetValuePtr;
    stlMemset( sTarget, 0x00, STL_SIZEOF( SQL_INTERVAL_STRUCT ) );

    STL_TRY( dtlGetTimeInfoFromYearMonthInterval( sDataValue,
                                                  &sYear,
                                                  &sMonth,
                                                  &sIsNegative,
                                                  aErrorStack )
             == STL_SUCCESS );

    STL_TRY( dtlAdjustIntervalYearToMonth( DTL_INTERVAL_INDICATOR_YEAR,
                                           aArdRec->mDatetimeIntervalPrecision,
                                           sYear,
                                           sMonth,
                                           &sSuccessWithInfo,
                                           aErrorStack )
             == STL_SUCCESS );

    sTarget->interval_type           = SQL_IS_YEAR;
    sTarget->interval_sign           = (SQLSMALLINT)sIsNegative;
    sTarget->intval.year_month.year  = sYear;
    sTarget->intval.year_month.month = 0;
    
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
 * SQL_INTERVAL_MONTH -> SQL_C_INTERVAL_YEAR_TO_MONTH
 */
ZLV_DECLARE_SQL_TO_C( IntervalMonth, IntervalYearToMonth )
{
    /*
     * ====================================================================================================
     * Test                                          | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ====================================================================================================
     * Trailing fields portion not truncated         | Data            | Length of data in bytes | n/a
     * Trailing fields portion truncated             | Truncated data  | Length of data in bytes | 01S07
     * Leading precision of target is not big enough | Undefined       | Undefined               | 22015
     * to hold data from source                      |                 |                         |
     * ====================================================================================================
     */

    dtlIntervalYearToMonthType * sDataValue;
    SQL_INTERVAL_STRUCT        * sTarget;
    stlUInt32                    sYear;
    stlUInt32                    sMonth;
    stlBool                      sIsNegative;
    stlBool                      sSuccessWithInfo = STL_FALSE;    
    stlStatus                    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlIntervalYearToMonthType*)aDataValue->mValue;
    sTarget    = (SQL_INTERVAL_STRUCT*)aTargetValuePtr;
    stlMemset( sTarget, 0x00, STL_SIZEOF( SQL_INTERVAL_STRUCT ) );

    STL_TRY( dtlGetTimeInfoFromYearMonthInterval( sDataValue,
                                                  &sYear,
                                                  &sMonth,
                                                  &sIsNegative,
                                                  aErrorStack )
             == STL_SUCCESS );

    STL_TRY( dtlAdjustIntervalYearToMonth( DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH,
                                           aArdRec->mDatetimeIntervalPrecision,
                                           sYear,
                                           sMonth,
                                           &sSuccessWithInfo,
                                           aErrorStack )
             == STL_SUCCESS );

    sTarget->interval_type           = SQL_IS_YEAR_TO_MONTH;
    sTarget->interval_sign           = (SQLSMALLINT)sIsNegative;
    sTarget->intval.year_month.year  = sYear;
    sTarget->intval.year_month.month = sMonth;
    
    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_SIZEOF( SQL_INTERVAL_STRUCT );
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
 * SQL_INTERVAL_MONTH -> SQL_C_STINYINT
 */
ZLV_DECLARE_SQL_TO_C( IntervalMonth, STinyInt )
{
    /*
     * ================================================================================================
     * Test                                      | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ================================================================================================
     * Interval precision was a single field and | Data            | Size of the C data type | n/a
     * the data was converted without truncation |                 |                         |
     * Interval precision was a single field and | Truncated data  | Length of data in bytes | 22003
     * truncated whole                           |                 |                         |
     * Interval precision was not a single field | Undefined       | Size of the C data type | 22015
     * ================================================================================================
     */

    dtlIntervalYearToMonthType * sDataValue;
    stlInt8                    * sTarget;
    stlUInt32                    sYear;
    stlUInt32                    sMonth;
    stlBool                      sIsNegative;
    stlInt64                     sTotalMonth;
    stlStatus                    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlIntervalYearToMonthType*)aDataValue->mValue;
    sTarget    = (stlInt8*)aTargetValuePtr;

    STL_TRY( dtlGetTimeInfoFromYearMonthInterval( sDataValue,
                                                  &sYear,
                                                  &sMonth,
                                                  &sIsNegative,
                                                  aErrorStack )
             == STL_SUCCESS );

    sTotalMonth = ( sYear * 12 ) + sMonth;

    STL_TRY_THROW( ( sTotalMonth >= STL_INT8_MIN ) &&
                   ( sTotalMonth <= STL_INT8_MAX ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    if( sIsNegative == STL_FALSE )
    {
        *sTarget = (stlInt8)sTotalMonth;
    }
    else
    {
        *sTarget = -(stlInt8)sTotalMonth;
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
 * SQL_INTERVAL_MONTH -> SQL_C_UTINYINT
 */
ZLV_DECLARE_SQL_TO_C( IntervalMonth, UTinyInt )
{
    /*
     * ================================================================================================
     * Test                                      | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ================================================================================================
     * Interval precision was a single field and | Data            | Size of the C data type | n/a
     * the data was converted without truncation |                 |                         |
     * Interval precision was a single field and | Truncated data  | Length of data in bytes | 22003
     * truncated whole                           |                 |                         |
     * Interval precision was not a single field | Undefined       | Size of the C data type | 22015
     * ================================================================================================
     */

    dtlIntervalYearToMonthType * sDataValue;
    stlUInt8                   * sTarget;
    stlUInt32                    sYear;
    stlUInt32                    sMonth;
    stlBool                      sIsNegative;
    stlUInt64                    sTotalMonth;
    stlStatus                    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlIntervalYearToMonthType*)aDataValue->mValue;
    sTarget    = (stlUInt8*)aTargetValuePtr;

    STL_TRY( dtlGetTimeInfoFromYearMonthInterval( sDataValue,
                                                  &sYear,
                                                  &sMonth,
                                                  &sIsNegative,
                                                  aErrorStack )
             == STL_SUCCESS );

    sTotalMonth = ( sYear * 12 ) + sMonth;

    STL_TRY_THROW( ( sTotalMonth >= STL_UINT8_MIN ) &&
                   ( sTotalMonth <= STL_UINT8_MAX ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    *sTarget = (stlUInt8)sTotalMonth;

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
 * SQL_INTERVAL_MONTH -> SQL_C_SBIGINT
 */
ZLV_DECLARE_SQL_TO_C( IntervalMonth, SBigInt )
{
    /*
     * ================================================================================================
     * Test                                      | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ================================================================================================
     * Interval precision was a single field and | Data            | Size of the C data type | n/a
     * the data was converted without truncation |                 |                         |
     * Interval precision was a single field and | Truncated data  | Length of data in bytes | 22003
     * truncated whole                           |                 |                         |
     * Interval precision was not a single field | Undefined       | Size of the C data type | 22015
     * ================================================================================================
     */

    dtlIntervalYearToMonthType * sDataValue;
    stlInt64                   * sTarget;
    stlUInt32                    sYear;
    stlUInt32                    sMonth;
    stlBool                      sIsNegative;
    stlInt64                     sTotalMonth;
    stlStatus                    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlIntervalYearToMonthType*)aDataValue->mValue;
    sTarget    = (stlInt64*)aTargetValuePtr;

    STL_TRY( dtlGetTimeInfoFromYearMonthInterval( sDataValue,
                                                  &sYear,
                                                  &sMonth,
                                                  &sIsNegative,
                                                  aErrorStack )
             == STL_SUCCESS );

    sTotalMonth = ( sYear * 12 ) + sMonth;

    if( sIsNegative == STL_FALSE )
    {
        *sTarget = (stlInt64)sTotalMonth;
    }
    else
    {
        *sTarget = -(stlInt64)sTotalMonth;
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
 * SQL_INTERVAL_MONTH -> SQL_C_UBIGINT
 */
ZLV_DECLARE_SQL_TO_C( IntervalMonth, UBigInt )
{
    /*
     * ================================================================================================
     * Test                                      | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ================================================================================================
     * Interval precision was a single field and | Data            | Size of the C data type | n/a
     * the data was converted without truncation |                 |                         |
     * Interval precision was a single field and | Truncated data  | Length of data in bytes | 22003
     * truncated whole                           |                 |                         |
     * Interval precision was not a single field | Undefined       | Size of the C data type | 22015
     * ================================================================================================
     */

    dtlIntervalYearToMonthType * sDataValue;
    stlUInt64                  * sTarget;
    stlUInt32                    sYear;
    stlUInt32                    sMonth;
    stlBool                      sIsNegative;
    stlUInt64                    sTotalMonth;
    stlStatus                    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlIntervalYearToMonthType*)aDataValue->mValue;
    sTarget    = (stlUInt64*)aTargetValuePtr;

    STL_TRY( dtlGetTimeInfoFromYearMonthInterval( sDataValue,
                                                  &sYear,
                                                  &sMonth,
                                                  &sIsNegative,
                                                  aErrorStack )
             == STL_SUCCESS );

    sTotalMonth = ( sYear * 12 ) + sMonth;

    *sTarget = (stlUInt64)sTotalMonth;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_UINT64_SIZE;
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
 * SQL_INTERVAL_MONTH -> SQL_C_SSHORT
 */
ZLV_DECLARE_SQL_TO_C( IntervalMonth, SShort )
{
    /*
     * ================================================================================================
     * Test                                      | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ================================================================================================
     * Interval precision was a single field and | Data            | Size of the C data type | n/a
     * the data was converted without truncation |                 |                         |
     * Interval precision was a single field and | Truncated data  | Length of data in bytes | 22003
     * truncated whole                           |                 |                         |
     * Interval precision was not a single field | Undefined       | Size of the C data type | 22015
     * ================================================================================================
     */

    dtlIntervalYearToMonthType * sDataValue;
    stlInt16                   * sTarget;
    stlUInt32                    sYear;
    stlUInt32                    sMonth;
    stlBool                      sIsNegative;
    stlInt64                     sTotalMonth;
    stlStatus                    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlIntervalYearToMonthType*)aDataValue->mValue;
    sTarget    = (stlInt16*)aTargetValuePtr;

    STL_TRY( dtlGetTimeInfoFromYearMonthInterval( sDataValue,
                                                  &sYear,
                                                  &sMonth,
                                                  &sIsNegative,
                                                  aErrorStack )
             == STL_SUCCESS );

    sTotalMonth = ( sYear * 12 ) + sMonth;

    STL_TRY_THROW( ( sTotalMonth >= STL_INT16_MIN ) &&
                   ( sTotalMonth <= STL_INT16_MAX ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    if( sIsNegative == STL_FALSE )
    {
        *sTarget = (stlInt16)sTotalMonth;
    }
    else
    {
        *sTarget = -(stlInt16)sTotalMonth;
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
 * SQL_INTERVAL_MONTH -> SQL_C_USHORT
 */
ZLV_DECLARE_SQL_TO_C( IntervalMonth, UShort )
{
    /*
     * ================================================================================================
     * Test                                      | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ================================================================================================
     * Interval precision was a single field and | Data            | Size of the C data type | n/a
     * the data was converted without truncation |                 |                         |
     * Interval precision was a single field and | Truncated data  | Length of data in bytes | 22003
     * truncated whole                           |                 |                         |
     * Interval precision was not a single field | Undefined       | Size of the C data type | 22015
     * ================================================================================================
     */

    dtlIntervalYearToMonthType * sDataValue;
    stlUInt16                  * sTarget;
    stlUInt32                    sYear;
    stlUInt32                    sMonth;
    stlBool                      sIsNegative;
    stlUInt64                    sTotalMonth;
    stlStatus                    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlIntervalYearToMonthType*)aDataValue->mValue;
    sTarget    = (stlUInt16*)aTargetValuePtr;

    STL_TRY( dtlGetTimeInfoFromYearMonthInterval( sDataValue,
                                                  &sYear,
                                                  &sMonth,
                                                  &sIsNegative,
                                                  aErrorStack )
             == STL_SUCCESS );

    sTotalMonth = ( sYear * 12 ) + sMonth;

    STL_TRY_THROW( ( sTotalMonth >= STL_UINT16_MIN ) &&
                   ( sTotalMonth <= STL_UINT16_MAX ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    *sTarget = (stlUInt16)sTotalMonth;

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
 * SQL_INTERVAL_MONTH -> SQL_C_SLONG
 */
ZLV_DECLARE_SQL_TO_C( IntervalMonth, SLong )
{
    /*
     * ================================================================================================
     * Test                                      | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ================================================================================================
     * Interval precision was a single field and | Data            | Size of the C data type | n/a
     * the data was converted without truncation |                 |                         |
     * Interval precision was a single field and | Truncated data  | Length of data in bytes | 22003
     * truncated whole                           |                 |                         |
     * Interval precision was not a single field | Undefined       | Size of the C data type | 22015
     * ================================================================================================
     */

    dtlIntervalYearToMonthType * sDataValue;
    stlInt32                   * sTarget;
    stlUInt32                    sYear;
    stlUInt32                    sMonth;
    stlBool                      sIsNegative;
    stlInt64                     sTotalMonth;
    stlStatus                    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlIntervalYearToMonthType*)aDataValue->mValue;
    sTarget    = (stlInt32*)aTargetValuePtr;

    STL_TRY( dtlGetTimeInfoFromYearMonthInterval( sDataValue,
                                                  &sYear,
                                                  &sMonth,
                                                  &sIsNegative,
                                                  aErrorStack )
             == STL_SUCCESS );

    sTotalMonth = ( sYear * 12 ) + sMonth;

    STL_TRY_THROW( ( sTotalMonth >= STL_INT32_MIN ) &&
                   ( sTotalMonth <= STL_INT32_MAX ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    if( sIsNegative == STL_FALSE )
    {
        *sTarget = (stlInt32)sTotalMonth;
    }
    else
    {
        *sTarget = -(stlInt32)sTotalMonth;
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
 * SQL_INTERVAL_MONTH -> SQL_C_ULONG
 */
ZLV_DECLARE_SQL_TO_C( IntervalMonth, ULong )
{
    /*
     * ================================================================================================
     * Test                                      | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ================================================================================================
     * Interval precision was a single field and | Data            | Size of the C data type | n/a
     * the data was converted without truncation |                 |                         |
     * Interval precision was a single field and | Truncated data  | Length of data in bytes | 22003
     * truncated whole                           |                 |                         |
     * Interval precision was not a single field | Undefined       | Size of the C data type | 22015
     * ================================================================================================
     */

    dtlIntervalYearToMonthType * sDataValue;
    stlUInt32                  * sTarget;
    stlUInt32                    sYear;
    stlUInt32                    sMonth;
    stlBool                      sIsNegative;
    stlUInt64                    sTotalMonth;
    stlStatus                    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlIntervalYearToMonthType*)aDataValue->mValue;
    sTarget    = (stlUInt32*)aTargetValuePtr;

    STL_TRY( dtlGetTimeInfoFromYearMonthInterval( sDataValue,
                                                  &sYear,
                                                  &sMonth,
                                                  &sIsNegative,
                                                  aErrorStack )
             == STL_SUCCESS );

    sTotalMonth = ( sYear * 12 ) + sMonth;

    STL_TRY_THROW( ( sTotalMonth >= STL_UINT32_MIN ) &&
                   ( sTotalMonth <= STL_UINT32_MAX ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    *sTarget = (stlUInt32)sTotalMonth;

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
 * SQL_INTERVAL_MONTH -> SQL_C_NUMERIC
 */
ZLV_DECLARE_SQL_TO_C( IntervalMonth, Numeric )
{
    /*
     * ================================================================================================
     * Test                                      | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ================================================================================================
     * Interval precision was a single field and | Data            | Size of the C data type | n/a
     * the data was converted without truncation |                 |                         |
     * Interval precision was a single field and | Truncated data  | Length of data in bytes | 22003
     * truncated whole                           |                 |                         |
     * Interval precision was not a single field | Undefined       | Size of the C data type | 22015
     * ================================================================================================
     */

    dtlIntervalYearToMonthType * sDataValue;
    SQL_NUMERIC_STRUCT         * sTarget;
    stlUInt32                    sYear;
    stlUInt32                    sMonth;
    stlBool                      sIsNegative;
    stlUInt64                    sTotalMonth;
    stlStatus                    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlIntervalYearToMonthType*)aDataValue->mValue;
    sTarget    = (SQL_NUMERIC_STRUCT*)aTargetValuePtr;

    STL_TRY_THROW( ((ZLV_SQL_TO_C_NUMERIC_MAX_SCALE >= aArdRec->mScale) &&
                    (ZLV_SQL_TO_C_NUMERIC_MIN_SCALE <= aArdRec->mScale)),
                   RAMP_ERR_SCALE_VALUE_OUT_OF_RANGE );

    STL_TRY( dtlGetTimeInfoFromYearMonthInterval( sDataValue,
                                                  &sYear,
                                                  &sMonth,
                                                  &sIsNegative,
                                                  aErrorStack )
             == STL_SUCCESS );

    sTotalMonth = ( sYear * 12 ) + sMonth;

    STL_TRY( zlvMakeCNumericFromInteger( aStmt,
                                         sTarget,
                                         sTotalMonth,
                                         aArdRec->mScale,
                                         ZLV_INTERVAL_MONTH_TO_C_NUMERIC_PRECISION,
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
 * SQL_INTERVAL_MONTH -> SQL_C_BINARY
 */
ZLV_DECLARE_SQL_TO_C( IntervalMonth, Binary )
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

    STL_TRY_THROW( DTL_INTERVAL_YEAR_TO_MONTH_SIZE <= aArdRec->mOctetLength,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    stlMemcpy( aTargetValuePtr,
               (const void*)aDataValue->mValue,
               DTL_INTERVAL_YEAR_TO_MONTH_SIZE );

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = DTL_INTERVAL_YEAR_TO_MONTH_SIZE;
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
 * SQL_INTERVAL_MONTH -> SQL_C_LONGVARBINARY
 */
ZLV_DECLARE_SQL_TO_C( IntervalMonth, LongVarBinary )
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

    STL_TRY_THROW( DTL_INTERVAL_YEAR_TO_MONTH_SIZE <= sBufferLength,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    stlMemcpy( sValue,
               (const void*)aDataValue->mValue,
               DTL_INTERVAL_YEAR_TO_MONTH_SIZE );

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = DTL_INTERVAL_YEAR_TO_MONTH_SIZE;
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
 * SQL_INTERVAL_MONTH -> SQL_C_CHAR
 */
ZLV_DECLARE_SQL_TO_C( IntervalMonth, Char )
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

    sLength = DTL_INTERVAL_YEAR_TO_MONTH_DISPLAY_SIZE( DTL_GET_BLOCK_ARG_NUM1( aValueBlockList ) );

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
 * SQL_INTERVAL_MONTH -> SQL_C_WCHAR
 */
ZLV_DECLARE_SQL_TO_C( IntervalMonth, Wchar )
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
    stlChar   sIntervalString[DTL_INTERVAL_YEAR_TO_MONTH_MAX_STRING_SIZE + 1];
    stlInt64  i;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sLength = DTL_INTERVAL_YEAR_TO_MONTH_DISPLAY_SIZE( DTL_GET_BLOCK_ARG_NUM1( aValueBlockList ) );

    STL_TRY_THROW( (sLength + 1) * STL_SIZEOF(SQLWCHAR) <= aArdRec->mOctetLength,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    STL_TRY( dtlToStringDataValue( aDataValue,
                                   DTL_GET_BLOCK_ARG_NUM1( aValueBlockList ),
                                   0,
                                   DTL_INTERVAL_YEAR_TO_MONTH_MAX_STRING_SIZE,
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
 * SQL_INTERVAL_MONTH -> SQL_C_LONGVARCHAR
 */
ZLV_DECLARE_SQL_TO_C( IntervalMonth, LongVarChar )
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

    sLength = DTL_INTERVAL_YEAR_TO_MONTH_DISPLAY_SIZE( DTL_GET_BLOCK_ARG_NUM1( aValueBlockList ) );

    STL_TRY_THROW( sBufferLength > sLength,
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

/** @} */
