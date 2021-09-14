/*******************************************************************************
 * zlvTimeTzToC.c
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
 * @file zlvTimeTzToC.c
 * @brief Gliese API Internal Converting Data from SQL Time With Time Zone to C Data Types Routines.
 */

/**
 * @addtogroup zlvTimeTzToC
 * @{
 */

/*
 * http://msdn.microsoft.com/en-us/library/windows/desktop/ms712436%28v=VS.85%29.aspx
 */

/*
 * SQL 표준에 없는 타입
 */


/*
 * SQL_TYPE_TIME_WITH_TIMEZONE -> SQL_C_CHAR
 */
ZLV_DECLARE_SQL_TO_C( TimeTz, Char )
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
 * SQL_TYPE_TIME_WITH_TIMEZONE -> SQL_C_WCHAR
 */
ZLV_DECLARE_SQL_TO_C( TimeTz, Wchar )
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
 * SQL_TYPE_TIME_WITH_TIMEZONE -> SQL_C_LONGVARCHAR
 */
ZLV_DECLARE_SQL_TO_C( TimeTz, LongVarChar )
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

    sDateTimeFormatInfo = ((ZLS_STMT_DT_VECTOR(aStmt))->mGetTimeWithTimeZoneFormatInfoFunc(aStmt));

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
 * SQL_TYPE_TIME_WITH_TIMEZONE -> SQL_C_BINARY
 */
ZLV_DECLARE_SQL_TO_C( TimeTz, Binary )
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

    STL_TRY_THROW( DTL_TIMETZ_SIZE <= aArdRec->mOctetLength,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    stlMemcpy( aTargetValuePtr,
               (const void*)aDataValue->mValue,
               DTL_TIMETZ_SIZE );

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = DTL_TIMETZ_SIZE;
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
 * SQL_TYPE_TIME_WITH_TIMEZONE -> SQL_C_LONGVARBINARY
 */
ZLV_DECLARE_SQL_TO_C( TimeTz, LongVarBinary )
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

    STL_TRY_THROW( DTL_TIMETZ_SIZE <= sBufferLength,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    stlMemcpy( sValue,
               (const void*)aDataValue->mValue,
               DTL_TIME_SIZE );

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = DTL_TIMETZ_SIZE;
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
 * SQL_TYPE_TIME_WITH_TIMEZONE -> SQL_C_TYPE_TIME
 */
ZLV_DECLARE_SQL_TO_C( TimeTz, Time )
{
    /*
     * =====================================================
     * Test | *TargetValuePtr | *StrLen_or_IndPtr | SQLSTATE
     * =====================================================
     * None | Data            | 6                 | n/a
     * =====================================================
     */
    
    dtlTimeTzType    * sDataValue;
    SQL_TIME_STRUCT  * sTarget;
    stlInt32           sHour;
    stlInt32           sMinute;
    stlInt32           sSecond;
    stlInt32           sFraction;
    stlInt32           sTimeZoneSign;
    stlUInt32          sTimeZoneHour;
    stlUInt32          sTimeZoneMinute;
    stlStatus          sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlTimeTzType*)aDataValue->mValue;
    sTarget    = (SQL_TIME_STRUCT*)aTargetValuePtr;

    STL_TRY( dtlAdjustTimeTz( sDataValue,
                              aArdRec->mPrecision,
                              aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( dtlGetTimeInfoFromTimeTz( sDataValue,
                                       &sHour,
                                       &sMinute,
                                       &sSecond,
                                       &sFraction,
                                       &sTimeZoneSign,
                                       &sTimeZoneHour,
                                       &sTimeZoneMinute,
                                       aErrorStack ) == STL_SUCCESS );

    sTarget->hour     = (stlUInt16)sHour;
    sTarget->minute   = (stlUInt16)sMinute;
    sTarget->second   = (stlUInt16)sSecond;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = 6;
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
 * SQL_TYPE_TIME_WITH_TIMEZONE -> SQL_C_TYPE_TIME_WITH_TIMEZONE
 */
ZLV_DECLARE_SQL_TO_C( TimeTz, TimeTz )
{
    /*
     * =====================================================
     * Test | *TargetValuePtr | *StrLen_or_IndPtr | SQLSTATE
     * =====================================================
     * None | Data            | 14                 | n/a
     * =====================================================
     */
    
    dtlTimeTzType * sDataValue;
    stlInt32        sHour;
    stlInt32        sMinute;
    stlInt32        sSecond;
    stlInt32        sFraction;
    stlInt32        sTimeZoneSign;
    stlUInt32       sTimeZoneHour;
    stlUInt32       sTimeZoneMinute;
    stlStatus       sRet = STL_FAILURE;

    SQL_TIME_WITH_TIMEZONE_STRUCT * sTarget;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlTimeTzType*)aDataValue->mValue;
    sTarget    = (SQL_TIME_WITH_TIMEZONE_STRUCT*)aTargetValuePtr;

    STL_TRY( dtlGetTimeInfoFromTimeTz( sDataValue,
                                       &sHour,
                                       &sMinute,
                                       &sSecond,
                                       &sFraction,
                                       &sTimeZoneSign,
                                       &sTimeZoneHour,
                                       &sTimeZoneMinute,
                                       aErrorStack ) == STL_SUCCESS );

    sTarget->hour     = (stlUInt16)sHour;
    sTarget->minute   = (stlUInt16)sMinute;
    sTarget->second   = (stlUInt16)sSecond;
    sTarget->fraction = sFraction * 1000;

    if( sTimeZoneSign == 1 )
    {
        sTarget->timezone_hour   = sTimeZoneHour;
        sTarget->timezone_minute = sTimeZoneMinute;
    }
    else
    {
        sTarget->timezone_hour   = -(stlInt32)sTimeZoneHour;
        sTarget->timezone_minute = -(stlInt32)sTimeZoneMinute;
    }

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = 14;
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
 * SQL_TYPE_TIME_WITH_TIMEZONE -> SQL_C_TYPE_TIMESTAMP
 */
ZLV_DECLARE_SQL_TO_C( TimeTz, Timestamp )
{
    /*
     * =====================================================
     * Test | *TargetValuePtr | *StrLen_or_IndPtr | SQLSTATE
     * =====================================================
     * None | Data            | 16                | n/a
     * =====================================================
     */
    
    dtlTimeTzType        * sDataValue;
    SQL_TIMESTAMP_STRUCT * sTarget;
    stlInt32               sHour;
    stlInt32               sMinute;
    stlInt32               sSecond;
    stlInt32               sFraction;
    stlInt32               sTimeZoneSign;
    stlUInt32              sTimeZoneHour;
    stlUInt32              sTimeZoneMinute;
    stlStatus              sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlTimeTzType*)aDataValue->mValue;
    sTarget    = (SQL_TIMESTAMP_STRUCT*)aTargetValuePtr;

    STL_TRY( dtlGetTimeInfoFromTimeTz( sDataValue,
                                       &sHour,
                                       &sMinute,
                                       &sSecond,
                                       &sFraction,
                                       &sTimeZoneSign,
                                       &sTimeZoneHour,
                                       &sTimeZoneMinute,
                                       aErrorStack ) == STL_SUCCESS );

    sTarget->year     = 0;
    sTarget->month    = 0;
    sTarget->day      = 0;
    sTarget->hour     = (stlUInt16)sHour;
    sTarget->minute   = (stlUInt16)sMinute;
    sTarget->second   = (stlUInt16)sSecond;
    sTarget->fraction = sFraction * 1000;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = 16;
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
 * SQL_TYPE_TIME_WITH_TIMEZONE -> SQL_C_TYPE_TIMESTAMP_WITH_TIMEZONE
 */
ZLV_DECLARE_SQL_TO_C( TimeTz, TimestampTz )
{
    /*
     * =====================================================
     * Test | *TargetValuePtr | *StrLen_or_IndPtr | SQLSTATE
     * =====================================================
     * None | Data            | 20                | n/a
     * =====================================================
     */
    
    dtlTimeTzType * sDataValue;
    stlInt32        sHour;
    stlInt32        sMinute;
    stlInt32        sSecond;
    stlInt32        sFraction;
    stlInt32        sTimeZoneSign;
    stlUInt32       sTimeZoneHour;
    stlUInt32       sTimeZoneMinute;
    stlStatus       sRet = STL_FAILURE;
    
    SQL_TIMESTAMP_WITH_TIMEZONE_STRUCT * sTarget;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlTimeTzType*)aDataValue->mValue;
    sTarget    = (SQL_TIMESTAMP_WITH_TIMEZONE_STRUCT*)aTargetValuePtr;

    STL_TRY( dtlGetTimeInfoFromTimeTz( sDataValue,
                                       &sHour,
                                       &sMinute,
                                       &sSecond,
                                       &sFraction,
                                       &sTimeZoneSign,
                                       &sTimeZoneHour,
                                       &sTimeZoneMinute,
                                       aErrorStack ) == STL_SUCCESS );

    sTarget->year     = 0;
    sTarget->month    = 0;
    sTarget->day      = 0;
    sTarget->hour     = (stlUInt16)sHour;
    sTarget->minute   = (stlUInt16)sMinute;
    sTarget->second   = (stlUInt16)sSecond;
    sTarget->fraction = sFraction * 1000;

    if( sTimeZoneSign == 1 )
    {
        sTarget->timezone_hour   = sTimeZoneHour;
        sTarget->timezone_minute = sTimeZoneMinute;
    }
    else
    {
        sTarget->timezone_hour   = -(stlInt32)sTimeZoneHour;
        sTarget->timezone_minute = -(stlInt32)sTimeZoneMinute;
    }

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = 20;
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

/** @} */

