/*******************************************************************************
 * zlvCharToC.c
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
#include <zlvCharacterset.h>

/**
 * @file zlvCharToC.c
 * @brief Gliese API Internal Converting Data from SQL Character to C Data Types Routines.
 */

/**
 * @addtogroup zlvCharToC
 * @{
 */

/*
 * http://msdn.microsoft.com/en-us/library/windows/desktop/ms713346%28v=VS.85%29.aspx
 */

/*
 * SQL_CHAR -> SQL_C_CHAR
 */
ZLV_DECLARE_SQL_TO_C( Char, Char )
{
    /*
     * ==========================================================================================
     * Test                                | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ==========================================================================================
     * Byte length of data < BufferLength  | Data            | Length of data in bytes | n/a
     * Byte length of data >= BufferLength | Truncated data  | Length of data in bytes | 01004
     * ==========================================================================================
     */

    SQLLEN    sIndicator = 0;
    stlStatus sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    STL_TRY( (ZLS_STMT_DBC(aStmt))->mConvertResultFunc( aStmt,
                                                        aOffset,
                                                        aDataValue,
                                                        aTargetValuePtr,
                                                        aArdRec->mOctetLength,
                                                        &sIndicator,
                                                        aErrorStack ) == STL_SUCCESS );

    if( aIndicator != NULL )
    {
        *aIndicator = sIndicator;
    }
    
    STL_TRY_THROW( sIndicator < aArdRec->mOctetLength, 
                   RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED );
    
    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
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
 * SQL_CHAR -> SQL_C_WCHAR
 */
ZLV_DECLARE_SQL_TO_C( Char, Wchar )
{
    /*
     * ==========================================================================================
     * Test                                | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ==========================================================================================
     * Byte length of data < BufferLength  | Data            | Length of data in bytes | n/a
     * Byte length of data >= BufferLength | Truncated data  | Length of data in bytes | 01004
     * ==========================================================================================
     */

    stlStatus sRet          = STL_FAILURE;
    stlInt64  sOffset       = 0;
    stlInt64  sBufferLength = 0;
    stlInt64  sLength       = 0;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sOffset = *aOffset;
    
    sBufferLength = aArdRec->mOctetLength / STL_SIZEOF(SQLWCHAR);
    
    STL_TRY( zlvCharToWchar( ZLS_STMT_DBC(aStmt)->mNlsCharacterSet,
                             (SQLCHAR*)aDataValue->mValue + sOffset,
                             aDataValue->mLength - sOffset,
                             (SQLWCHAR*)aTargetValuePtr,
                             sBufferLength,
                             &sLength,
                             aErrorStack ) == STL_SUCCESS );

    if( aIndicator != NULL )
    {
        *aIndicator = sLength * STL_SIZEOF(SQLWCHAR);
    }
    
    STL_TRY_THROW( sLength < sBufferLength, RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED );
    
    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
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
 * SQL_CHAR -> SQL_C_LONGVARCHAR
 */
ZLV_DECLARE_SQL_TO_C( Char, LongVarChar )
{
    /*
     * ==========================================================================================
     * Test                                | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ==========================================================================================
     * Byte length of data < BufferLength  | Data            | Length of data in bytes | n/a
     * Byte length of data >= BufferLength | Truncated data  | Length of data in bytes | 01004
     * ==========================================================================================
     */

    SQL_LONG_VARIABLE_LENGTH_STRUCT * sTarget;

    SQLLEN    sIndicator = 0;
    stlStatus sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aTargetValuePtr;

    STL_TRY( (ZLS_STMT_DBC(aStmt))->mConvertResultFunc( aStmt,
                                                        aOffset,
                                                        aDataValue,
                                                        sTarget->arr,
                                                        sTarget->len,
                                                        &sIndicator,
                                                        aErrorStack ) == STL_SUCCESS );
    
    if( aIndicator != NULL )
    {
        *aIndicator = sIndicator;
    }

    STL_TRY_THROW( sIndicator < sTarget->len, RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED );

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
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
 * SQL_CHAR -> SQL_C_STINYINT
 */
ZLV_DECLARE_SQL_TO_C( Char, STinyInt )
{
    /*
     * =====================================================================================================================
     * Test                                                | *TargetValuePtr | *StrLen_or_IndPtr                  | SQLSTATE
     * =====================================================================================================================
     * Data converted without truncation                   | Data            | Number of bytes of the C data type | n/a
     * Data converted with truncation of fractional digits | Truncated data  | Number of bytes of the C data type | 01S07
     * Conversion of data would result in loss of whole    | Undefined       | Undefined                          | 22003
     * (as opposed to fractional) digits                   |                 |                                    |
     * Data is not a numeric-literal                       | Undefined       | Undefined                          | 22018
     * =====================================================================================================================
     */

    stlInt8   * sTarget;
    stlInt64    sConvertedResult;
    stlChar   * sStartPtr;
    stlChar   * sEndPtr;
    stlStatus   sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget = (stlInt8*)aTargetValuePtr;

    sStartPtr = aDataValue->mValue;
    STL_TRY_THROW( stlStrToInt64( (const stlChar*)sStartPtr,
                                  aDataValue->mLength,
                                  &sEndPtr,
                                  10,
                                  &sConvertedResult,
                                  aErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_CONVERSION );

    STL_TRY_THROW( sStartPtr != sEndPtr,
                   RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION );

    STL_TRY_THROW( ( sConvertedResult >= STL_INT8_MIN ) &&
                   ( sConvertedResult <= STL_INT8_MAX ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    *sTarget = (stlInt8)sConvertedResult;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_INT8_SIZE;
    }

    STL_TRY_THROW( (sStartPtr + aDataValue->mLength) == sEndPtr,
                   RAMP_WARNING_FRACTIONAL_TRUNCATION );

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_ERR_CONVERSION )
    {
        switch( stlGetLastErrorCode( aErrorStack ) )
        {
            case STL_ERRCODE_STRING_IS_NOT_NUMBER :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                              "The C type was an exact or approximate numeric, a datetime, or an interval data type; "
                              "the SQL type of the column was a character data type; "
                              "and the value in the column was not a valid literal of the bound C type.",
                              aErrorStack );
                break;
            case STL_ERRCODE_OUT_OF_RANGE :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                              "Returning the numeric value as numeric or string for one or "
                              "more bound columns would have caused the whole "
                              "(as opposed to fractional) part of the number to be truncated.",
                              aErrorStack );
                break;
            default :
                break;
        }
    }
    
    STL_CATCH( RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                      "The C type was an exact or approximate numeric, a datetime, or an interval data type; "
                      "the SQL type of the column was a character data type; "
                      "and the value in the column was not a valid literal of the bound C type.",
                      aErrorStack );
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
 * SQL_CHAR -> SQL_C_UTINYINT
 */
ZLV_DECLARE_SQL_TO_C( Char, UTinyInt )
{
    /*
     * =====================================================================================================================
     * Test                                                | *TargetValuePtr | *StrLen_or_IndPtr                  | SQLSTATE
     * =====================================================================================================================
     * Data converted without truncation                   | Data            | Number of bytes of the C data type | n/a
     * Data converted with truncation of fractional digits | Truncated data  | Number of bytes of the C data type | 01S07
     * Conversion of data would result in loss of whole    | Undefined       | Undefined                          | 22003
     * (as opposed to fractional) digits                   |                 |                                    |
     * Data is not a numeric-literal                       | Undefined       | Undefined                          | 22018
     * =====================================================================================================================
     */
    
    stlUInt8   * sTarget;
    stlInt64     sConvertedResult;
    stlChar    * sStartPtr;
    stlChar    * sEndPtr;
    stlStatus    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget = (stlUInt8*)aTargetValuePtr;

    sStartPtr = aDataValue->mValue;
    STL_TRY_THROW( stlStrToInt64( (const stlChar*)sStartPtr,
                                  aDataValue->mLength,
                                  &sEndPtr,
                                  10,
                                  &sConvertedResult,
                                  aErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_CONVERSION );

    STL_TRY_THROW( sStartPtr != sEndPtr,
                   RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION );

    STL_TRY_THROW( ( sConvertedResult >= STL_UINT8_MIN ) &&
                   ( sConvertedResult <= STL_UINT8_MAX ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    *sTarget = (stlUInt8)sConvertedResult;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_UINT8_SIZE;
    }

    STL_TRY_THROW( (sStartPtr + aDataValue->mLength) == sEndPtr,
                   RAMP_WARNING_FRACTIONAL_TRUNCATION );

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_ERR_CONVERSION )
    {
        switch( stlGetLastErrorCode( aErrorStack ) )
        {
            case STL_ERRCODE_STRING_IS_NOT_NUMBER :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                              "The C type was an exact or approximate numeric, a datetime, or an interval data type; "
                              "the SQL type of the column was a character data type; "
                              "and the value in the column was not a valid literal of the bound C type.",
                              aErrorStack );
                break;
            case STL_ERRCODE_OUT_OF_RANGE :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                              "Returning the numeric value as numeric or string for one or "
                              "more bound columns would have caused the whole "
                              "(as opposed to fractional) part of the number to be truncated.",
                              aErrorStack );
                break;
            default :
                break;
        }
    }
    
    STL_CATCH( RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                      "The C type was an exact or approximate numeric, a datetime, or an interval data type; "
                      "the SQL type of the column was a character data type; "
                      "and the value in the column was not a valid literal of the bound C type.",
                      aErrorStack );
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
 * SQL_CHAR -> SQL_C_SBIGINT
 */
ZLV_DECLARE_SQL_TO_C( Char, SBigInt )
{
    /*
     * =====================================================================================================================
     * Test                                                | *TargetValuePtr | *StrLen_or_IndPtr                  | SQLSTATE
     * =====================================================================================================================
     * Data converted without truncation                   | Data            | Number of bytes of the C data type | n/a
     * Data converted with truncation of fractional digits | Truncated data  | Number of bytes of the C data type | 01S07
     * Conversion of data would result in loss of whole    | Undefined       | Undefined                          | 22003
     * (as opposed to fractional) digits                   |                 |                                    |
     * Data is not a numeric-literal                       | Undefined       | Undefined                          | 22018
     * =====================================================================================================================
     */
    
    stlInt64  * sTarget;
    stlInt64    sConvertedResult;
    stlChar   * sStartPtr;
    stlChar   * sEndPtr;
    stlStatus   sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget = (stlInt64*)aTargetValuePtr;

    sStartPtr = aDataValue->mValue;
    STL_TRY_THROW( stlStrToInt64( (const stlChar*)sStartPtr,
                                  aDataValue->mLength,
                                  &sEndPtr,
                                  10,
                                  &sConvertedResult,
                                  aErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_CONVERSION );

    STL_TRY_THROW( sStartPtr != sEndPtr,
                   RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION );

    *sTarget = sConvertedResult;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_INT64_SIZE;
    }

    STL_TRY_THROW( (sStartPtr + aDataValue->mLength) == sEndPtr,
                   RAMP_WARNING_FRACTIONAL_TRUNCATION );

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_ERR_CONVERSION )
    {
        switch( stlGetLastErrorCode( aErrorStack ) )
        {
            case STL_ERRCODE_STRING_IS_NOT_NUMBER :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                              "The C type was an exact or approximate numeric, a datetime, or an interval data type; "
                              "the SQL type of the column was a character data type; "
                              "and the value in the column was not a valid literal of the bound C type.",
                              aErrorStack );
                break;
            case STL_ERRCODE_OUT_OF_RANGE :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                              "Returning the numeric value as numeric or string for one or "
                              "more bound columns would have caused the whole "
                              "(as opposed to fractional) part of the number to be truncated.",
                              aErrorStack );
                break;
            default :
                break;
        }
    }
    
    STL_CATCH( RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                      "The C type was an exact or approximate numeric, a datetime, or an interval data type; "
                      "the SQL type of the column was a character data type; "
                      "and the value in the column was not a valid literal of the bound C type.",
                      aErrorStack );
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
 * SQL_CHAR -> SQL_C_UBIGINT
 */
ZLV_DECLARE_SQL_TO_C( Char, UBigInt )
{
    /*
     * =====================================================================================================================
     * Test                                                | *TargetValuePtr | *StrLen_or_IndPtr                  | SQLSTATE
     * =====================================================================================================================
     * Data converted without truncation                   | Data            | Number of bytes of the C data type | n/a
     * Data converted with truncation of fractional digits | Truncated data  | Number of bytes of the C data type | 01S07
     * Conversion of data would result in loss of whole    | Undefined       | Undefined                          | 22003
     * (as opposed to fractional) digits                   |                 |                                    |
     * Data is not a numeric-literal                       | Undefined       | Undefined                          | 22018
     * =====================================================================================================================
     */
    
    stlUInt64  * sTarget;
    stlUInt64    sConvertedResult;
    stlChar    * sStartPtr;
    stlChar    * sEndPtr;
    stlStatus    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget = (stlUInt64*)aTargetValuePtr;

    sStartPtr = aDataValue->mValue;
    STL_TRY_THROW( stlStrToUInt64( (const stlChar*)sStartPtr,
                                   aDataValue->mLength,
                                   &sEndPtr,
                                   10,
                                   &sConvertedResult,
                                   aErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_CONVERSION );

    STL_TRY_THROW( sStartPtr != sEndPtr,
                   RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION );

    *sTarget = sConvertedResult;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_UINT64_SIZE;
    }

    STL_TRY_THROW( (sStartPtr + aDataValue->mLength) == sEndPtr,
                   RAMP_WARNING_FRACTIONAL_TRUNCATION );

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_ERR_CONVERSION )
    {
        switch( stlGetLastErrorCode( aErrorStack ) )
        {
            case STL_ERRCODE_STRING_IS_NOT_NUMBER :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                              "The C type was an exact or approximate numeric, a datetime, or an interval data type; "
                              "the SQL type of the column was a character data type; "
                              "and the value in the column was not a valid literal of the bound C type.",
                              aErrorStack );
                break;
            case STL_ERRCODE_OUT_OF_RANGE :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                              "Returning the numeric value as numeric or string for one or "
                              "more bound columns would have caused the whole "
                              "(as opposed to fractional) part of the number to be truncated.",
                              aErrorStack );
                break;
            default :
                break;
        }
    }
    
    STL_CATCH( RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                      "The C type was an exact or approximate numeric, a datetime, or an interval data type; "
                      "the SQL type of the column was a character data type; "
                      "and the value in the column was not a valid literal of the bound C type.",
                      aErrorStack );
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
 * SQL_CHAR -> SQL_C_SSHORT
 */
ZLV_DECLARE_SQL_TO_C( Char, SShort )
{
    /*
     * =====================================================================================================================
     * Test                                                | *TargetValuePtr | *StrLen_or_IndPtr                  | SQLSTATE
     * =====================================================================================================================
     * Data converted without truncation                   | Data            | Number of bytes of the C data type | n/a
     * Data converted with truncation of fractional digits | Truncated data  | Number of bytes of the C data type | 01S07
     * Conversion of data would result in loss of whole    | Undefined       | Undefined                          | 22003
     * (as opposed to fractional) digits                   |                 |                                    |
     * Data is not a numeric-literal                       | Undefined       | Undefined                          | 22018
     * =====================================================================================================================
     */
    
    stlInt16  * sTarget;
    stlInt64    sConvertedResult;
    stlChar   * sStartPtr;
    stlChar   * sEndPtr;
    stlStatus   sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget = (stlInt16*)aTargetValuePtr;

    sStartPtr = aDataValue->mValue;
    STL_TRY_THROW( stlStrToInt64( (const stlChar*)sStartPtr,
                                  aDataValue->mLength,
                                  &sEndPtr,
                                  10,
                                  &sConvertedResult,
                                  aErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_CONVERSION );

    STL_TRY_THROW( sStartPtr != sEndPtr,
                   RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION );

    STL_TRY_THROW( ( sConvertedResult >= STL_INT16_MIN ) &&
                   ( sConvertedResult <= STL_INT16_MAX ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    *sTarget = (stlInt16)sConvertedResult;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_INT16_SIZE;
    }

    STL_TRY_THROW( (sStartPtr + aDataValue->mLength) == sEndPtr,
                   RAMP_WARNING_FRACTIONAL_TRUNCATION );

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_ERR_CONVERSION )
    {
        switch( stlGetLastErrorCode( aErrorStack ) )
        {
            case STL_ERRCODE_STRING_IS_NOT_NUMBER :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                              "The C type was an exact or approximate numeric, a datetime, or an interval data type; "
                              "the SQL type of the column was a character data type; "
                              "and the value in the column was not a valid literal of the bound C type.",
                              aErrorStack );
                break;
            case STL_ERRCODE_OUT_OF_RANGE :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                              "Returning the numeric value as numeric or string for one or "
                              "more bound columns would have caused the whole "
                              "(as opposed to fractional) part of the number to be truncated.",
                              aErrorStack );
                break;
            default :
                break;
        }
    }

    STL_CATCH( RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                      "The C type was an exact or approximate numeric, a datetime, or an interval data type; "
                      "the SQL type of the column was a character data type; "
                      "and the value in the column was not a valid literal of the bound C type.",
                      aErrorStack );
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
 * SQL_CHAR -> SQL_C_USHORT
 */
ZLV_DECLARE_SQL_TO_C( Char, UShort )
{
    /*
     * =====================================================================================================================
     * Test                                                | *TargetValuePtr | *StrLen_or_IndPtr                  | SQLSTATE
     * =====================================================================================================================
     * Data converted without truncation                   | Data            | Number of bytes of the C data type | n/a
     * Data converted with truncation of fractional digits | Truncated data  | Number of bytes of the C data type | 01S07
     * Conversion of data would result in loss of whole    | Undefined       | Undefined                          | 22003
     * (as opposed to fractional) digits                   |                 |                                    |
     * Data is not a numeric-literal                       | Undefined       | Undefined                          | 22018
     * =====================================================================================================================
     */
    
    stlUInt16  * sTarget;
    stlInt64     sConvertedResult;
    stlChar    * sStartPtr;
    stlChar    * sEndPtr;
    stlStatus    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget = (stlUInt16*)aTargetValuePtr;

    sStartPtr = aDataValue->mValue;
    STL_TRY_THROW( stlStrToInt64( (const stlChar*)sStartPtr,
                                  aDataValue->mLength,
                                  &sEndPtr,
                                  10,
                                  &sConvertedResult,
                                  aErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_CONVERSION );

    STL_TRY_THROW( sStartPtr != sEndPtr,
                   RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION );

    STL_TRY_THROW( ( sConvertedResult >= STL_UINT16_MIN ) &&
                   ( sConvertedResult <= STL_UINT16_MAX ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    *sTarget = (stlUInt16)sConvertedResult;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_INT16_SIZE;
    }

    STL_TRY_THROW( (sStartPtr + aDataValue->mLength) == sEndPtr,
                   RAMP_WARNING_FRACTIONAL_TRUNCATION );

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_ERR_CONVERSION )
    {
        switch( stlGetLastErrorCode( aErrorStack ) )
        {
            case STL_ERRCODE_STRING_IS_NOT_NUMBER :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                              "The C type was an exact or approximate numeric, a datetime, or an interval data type; "
                              "the SQL type of the column was a character data type; "
                              "and the value in the column was not a valid literal of the bound C type.",
                              aErrorStack );
                break;
            case STL_ERRCODE_OUT_OF_RANGE :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                              "Returning the numeric value as numeric or string for one or "
                              "more bound columns would have caused the whole "
                              "(as opposed to fractional) part of the number to be truncated.",
                              aErrorStack );
                break;
            default :
                break;
        }
    }
    
    STL_CATCH( RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                      "The C type was an exact or approximate numeric, a datetime, or an interval data type; "
                      "the SQL type of the column was a character data type; "
                      "and the value in the column was not a valid literal of the bound C type.",
                      aErrorStack );
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
 * SQL_CHAR -> SQL_C_SLONG
 */
ZLV_DECLARE_SQL_TO_C( Char, SLong )
{
    /*
     * =====================================================================================================================
     * Test                                                | *TargetValuePtr | *StrLen_or_IndPtr                  | SQLSTATE
     * =====================================================================================================================
     * Data converted without truncation                   | Data            | Number of bytes of the C data type | n/a
     * Data converted with truncation of fractional digits | Truncated data  | Number of bytes of the C data type | 01S07
     * Conversion of data would result in loss of whole    | Undefined       | Undefined                          | 22003
     * (as opposed to fractional) digits                   |                 |                                    |
     * Data is not a numeric-literal                       | Undefined       | Undefined                          | 22018
     * =====================================================================================================================
     */
    
    stlInt32  * sTarget;
    stlInt64    sConvertedResult;
    stlChar   * sStartPtr;
    stlChar   * sEndPtr;
    stlStatus   sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget = (stlInt32*)aTargetValuePtr;

    sStartPtr = aDataValue->mValue;
    STL_TRY_THROW( stlStrToInt64( (const stlChar*)sStartPtr,
                                  aDataValue->mLength,
                                  &sEndPtr,
                                  10,
                                  &sConvertedResult,
                                  aErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_CONVERSION );

    STL_TRY_THROW( sStartPtr != sEndPtr,
                   RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION );

    STL_TRY_THROW( ( sConvertedResult >= STL_INT32_MIN ) &&
                   ( sConvertedResult <= STL_INT32_MAX ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    *sTarget = (stlInt32)sConvertedResult;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_INT16_SIZE;
    }

    STL_TRY_THROW( (sStartPtr + aDataValue->mLength) == sEndPtr,
                   RAMP_WARNING_FRACTIONAL_TRUNCATION );

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_ERR_CONVERSION )
    {
        switch( stlGetLastErrorCode( aErrorStack ) )
        {
            case STL_ERRCODE_STRING_IS_NOT_NUMBER :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                              "The C type was an exact or approximate numeric, a datetime, or an interval data type; "
                              "the SQL type of the column was a character data type; "
                              "and the value in the column was not a valid literal of the bound C type.",
                              aErrorStack );
                break;
            case STL_ERRCODE_OUT_OF_RANGE :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                              "Returning the numeric value as numeric or string for one or "
                              "more bound columns would have caused the whole "
                              "(as opposed to fractional) part of the number to be truncated.",
                              aErrorStack );
                break;
            default :
                break;
        }
    }
    
    STL_CATCH( RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                      "The C type was an exact or approximate numeric, a datetime, or an interval data type; "
                      "the SQL type of the column was a character data type; "
                      "and the value in the column was not a valid literal of the bound C type.",
                      aErrorStack );
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
 * SQL_CHAR -> SQL_C_ULONG
 */
ZLV_DECLARE_SQL_TO_C( Char, ULong )
{
    /*
     * =====================================================================================================================
     * Test                                                | *TargetValuePtr | *StrLen_or_IndPtr                  | SQLSTATE
     * =====================================================================================================================
     * Data converted without truncation                   | Data            | Number of bytes of the C data type | n/a
     * Data converted with truncation of fractional digits | Truncated data  | Number of bytes of the C data type | 01S07
     * Conversion of data would result in loss of whole    | Undefined       | Undefined                          | 22003
     * (as opposed to fractional) digits                   |                 |                                    |
     * Data is not a numeric-literal                       | Undefined       | Undefined                          | 22018
     * =====================================================================================================================
     */
    
    stlUInt32  * sTarget;
    stlInt64     sConvertedResult;
    stlChar    * sStartPtr;
    stlChar    * sEndPtr;
    stlStatus    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget = (stlUInt32*)aTargetValuePtr;

    sStartPtr = aDataValue->mValue;
    STL_TRY_THROW( stlStrToInt64( (const stlChar*)sStartPtr,
                                  aDataValue->mLength,
                                  &sEndPtr,
                                  10,
                                  &sConvertedResult,
                                  aErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_CONVERSION );

    STL_TRY_THROW( sStartPtr != sEndPtr,
                   RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION );

    STL_TRY_THROW( ( sConvertedResult >= STL_UINT32_MIN ) &&
                   ( sConvertedResult <= STL_UINT32_MAX ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    *sTarget = (stlUInt32)sConvertedResult;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_INT16_SIZE;
    }

    STL_TRY_THROW( (sStartPtr + aDataValue->mLength) == sEndPtr,
                   RAMP_WARNING_FRACTIONAL_TRUNCATION );

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_ERR_CONVERSION )
    {
        switch( stlGetLastErrorCode( aErrorStack ) )
        {
            case STL_ERRCODE_STRING_IS_NOT_NUMBER :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                              "The C type was an exact or approximate numeric, a datetime, or an interval data type; "
                              "the SQL type of the column was a character data type; "
                              "and the value in the column was not a valid literal of the bound C type.",
                              aErrorStack );
                break;
            case STL_ERRCODE_OUT_OF_RANGE :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                              "Returning the numeric value as numeric or string for one or "
                              "more bound columns would have caused the whole "
                              "(as opposed to fractional) part of the number to be truncated.",
                              aErrorStack );
                break;
            default :
                break;
        }
    }
    
    STL_CATCH( RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                      "The C type was an exact or approximate numeric, a datetime, or an interval data type; "
                      "the SQL type of the column was a character data type; "
                      "and the value in the column was not a valid literal of the bound C type.",
                      aErrorStack );
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
 * SQL_CHAR -> SQL_C_NUMERIC
 */
ZLV_DECLARE_SQL_TO_C( Char, Numeric )
{
    /*
     * =====================================================================================================================
     * Test                                                | *TargetValuePtr | *StrLen_or_IndPtr                  | SQLSTATE
     * =====================================================================================================================
     * Data converted without truncation                   | Data            | Number of bytes of the C data type | n/a
     * Data converted with truncation of fractional digits | Truncated data  | Number of bytes of the C data type | 01S07
     * Conversion of data would result in loss of whole    | Undefined       | Undefined                          | 22003
     * (as opposed to fractional) digits                   |                 |                                    |
     * Data is not a numeric-literal                       | Undefined       | Undefined                          | 22018
     * =====================================================================================================================
     */
    
    SQL_NUMERIC_STRUCT * sTarget;
    stlBool              sIsTruncated = STL_FALSE;
    stlStatus            sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget = (SQL_NUMERIC_STRUCT*)aTargetValuePtr;

    STL_TRY_THROW( ((ZLV_SQL_TO_C_NUMERIC_MAX_SCALE >= aArdRec->mScale) &&
                    (ZLV_SQL_TO_C_NUMERIC_MIN_SCALE <= aArdRec->mScale)),
                   RAMP_ERR_SCALE_VALUE_OUT_OF_RANGE );

    STL_TRY( zlvMakeCNumericFromString( aStmt,
                                        sTarget,
                                        (stlChar*)aDataValue->mValue,
                                        aDataValue->mLength,
                                        ZLV_CHAR_TO_C_NUMERIC_PRECISION,
                                        aArdRec->mScale,
                                        & sIsTruncated,
                                        aErrorStack )
             == STL_SUCCESS );          

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_SIZEOF( SQL_NUMERIC_STRUCT );
    }

    sRet     = STL_SUCCESS;

    if( sIsTruncated == STL_TRUE )
    {
        *aReturn = SQL_SUCCESS_WITH_INFO;
    }
    else
    {
        *aReturn = SQL_SUCCESS;
    }

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_ERR_SCALE_VALUE_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                      "Specified scale value is out of range( -128 to 127 )",
                      aErrorStack );
    }

    STL_FINISH;

    return sRet;
}

/*
 * SQL_CHAR -> SQL_C_FLOAT
 */
ZLV_DECLARE_SQL_TO_C( Char, Float )
{
    /*
     * ====================================================================================================
     * Test                                          | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ====================================================================================================
     * Data is within the range of the data type to  | Data            | Size of the C data type | n/a
     * which the number is being converted           |                 |                         |
     * Data is outside the range of the data type to | Undefined       | Undefined               | 22003
     * which the number is being converted           |                 |                         |
     * Data is not a numeric-literal                 | Undefined       | Undefined               | 22018
     * ====================================================================================================
     */
    
    stlFloat32 * sTarget;
    stlFloat32   sConvertedResult;
    stlChar    * sStartPtr;
    stlChar    * sEndPtr;
    stlStatus    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget = (stlFloat32*)aTargetValuePtr;

    sStartPtr = aDataValue->mValue;
    STL_TRY_THROW( stlStrToFloat32( (const stlChar*)sStartPtr,
                                    aDataValue->mLength,
                                    &sEndPtr,
                                    &sConvertedResult,
                                    aErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_CONVERSION );

    STL_TRY_THROW( sStartPtr != sEndPtr,
                   RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION );

    *sTarget = sConvertedResult;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_FLOAT32_SIZE;
    }

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_ERR_CONVERSION )
    {
        switch( stlGetLastErrorCode( aErrorStack ) )
        {
            case STL_ERRCODE_STRING_IS_NOT_NUMBER :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                              "The C type was an exact or approximate numeric, a datetime, or an interval data type; "
                              "the SQL type of the column was a character data type; "
                              "and the value in the column was not a valid literal of the bound C type.",
                              aErrorStack );
                break;
            case STL_ERRCODE_OUT_OF_RANGE :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                              "Returning the numeric value as numeric or string for one or "
                              "more bound columns would have caused the whole "
                              "(as opposed to fractional) part of the number to be truncated.",
                              aErrorStack );
                break;
            default :
                break;
        }
    }
    
    STL_CATCH( RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                      "The C type was an exact or approximate numeric, a datetime, or an interval data type; "
                      "the SQL type of the column was a character data type; "
                      "and the value in the column was not a valid literal of the bound C type.",
                      aErrorStack );
    }
    
    STL_FINISH;

    return sRet;
}

/*
 * SQL_CHAR -> SQL_C_DOUBLE
 */
ZLV_DECLARE_SQL_TO_C( Char, Double )
{
    /*
     * ====================================================================================================
     * Test                                          | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ====================================================================================================
     * Data is within the range of the data type to  | Data            | Size of the C data type | n/a
     * which the number is being converted           |                 |                         |
     * Data is outside the range of the data type to | Undefined       | Undefined               | 22003
     * which the number is being converted           |                 |                         |
     * Data is not a numeric-literal                 | Undefined       | Undefined               | 22018
     * ====================================================================================================
     */
    
    stlFloat64 * sTarget;
    stlFloat64   sConvertedResult;
    stlChar    * sStartPtr;
    stlChar    * sEndPtr;
    stlStatus    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget = (stlFloat64*)aTargetValuePtr;

    sStartPtr = aDataValue->mValue;
    STL_TRY_THROW( stlStrToFloat64( (const stlChar*)sStartPtr,
                                    aDataValue->mLength,
                                    &sEndPtr,
                                    &sConvertedResult,
                                    aErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_CONVERSION );

    STL_TRY_THROW( sStartPtr != sEndPtr,
                   RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION );

    *sTarget = sConvertedResult;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_FLOAT64_SIZE;
    }

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_ERR_CONVERSION )
    {
        switch( stlGetLastErrorCode( aErrorStack ) )
        {
            case STL_ERRCODE_STRING_IS_NOT_NUMBER :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                              "The C type was an exact or approximate numeric, a datetime, or an interval data type; "
                              "the SQL type of the column was a character data type; "
                              "and the value in the column was not a valid literal of the bound C type.",
                              aErrorStack );
                break;
            case STL_ERRCODE_OUT_OF_RANGE :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                              "Returning the numeric value as numeric or string for one or "
                              "more bound columns would have caused the whole "
                              "(as opposed to fractional) part of the number to be truncated.",
                              aErrorStack );
                break;
            default :
                break;
        }
    }
    
    STL_CATCH( RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                      "The C type was an exact or approximate numeric, a datetime, or an interval data type; "
                      "the SQL type of the column was a character data type; "
                      "and the value in the column was not a valid literal of the bound C type.",
                      aErrorStack );
    }
    
    STL_FINISH;

    return sRet;
}

/*
 * SQL_CHAR -> SQL_C_BIT
 */
ZLV_DECLARE_SQL_TO_C( Char, Bit )
{
    /*
     * ========================================================================================================
     * Test                                                    | *TargetValuePtr | *StrLen_or_IndPtr | SQLSTATE
     * ========================================================================================================
     * Data is 0 or 1                                          | Data            | 1                 | n/a
     * Data is greater than 0, less than 2, and not equal to 1 | Truncated data  | 1                 | 01S07
     * Data is less than 0 or greater than or equal to 2       | Undefined       | Undefined         | 22003
     * Data is not a numeric-literal                           | Undefined       | Undefined         | 22018
     * ========================================================================================================
     */
    
    stlUInt8   * sTarget;
    stlInt64     sConvertedResult;
    stlChar    * sStartPtr;
    stlChar    * sEndPtr;
    stlStatus    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget = (stlUInt8*)aTargetValuePtr;

    sStartPtr = aDataValue->mValue;
    STL_TRY_THROW( stlStrToInt64( (const stlChar*)sStartPtr,
                                  aDataValue->mLength,
                                  &sEndPtr,
                                  10,
                                  &sConvertedResult,
                                  aErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_CONVERSION );

    STL_TRY_THROW( sStartPtr != sEndPtr,
                   RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION );

    STL_TRY_THROW( ( sConvertedResult >= 0 ) &&
                   ( sConvertedResult < 2 ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    *sTarget = (stlUInt8)sConvertedResult;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = 1;
    }

    STL_TRY_THROW( (sStartPtr + aDataValue->mLength) == sEndPtr,
                   RAMP_WARNING_FRACTIONAL_TRUNCATION );

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_ERR_CONVERSION )
    {
        switch( stlGetLastErrorCode( aErrorStack ) )
        {
            case STL_ERRCODE_STRING_IS_NOT_NUMBER :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                              "The C type was an exact or approximate numeric, a datetime, or an interval data type; "
                              "the SQL type of the column was a character data type; "
                              "and the value in the column was not a valid literal of the bound C type.",
                              aErrorStack );
                break;
            case STL_ERRCODE_OUT_OF_RANGE :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                              "Returning the numeric value as numeric or string for one or "
                              "more bound columns would have caused the whole "
                              "(as opposed to fractional) part of the number to be truncated.",
                              aErrorStack );
                break;
            default :
                break;
        }
    }
    
    STL_CATCH( RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                      "The C type was an exact or approximate numeric, a datetime, or an interval data type; "
                      "the SQL type of the column was a character data type; "
                      "and the value in the column was not a valid literal of the bound C type.",
                      aErrorStack );
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
 * SQL_CHAR -> SQL_C_BOOLEAN
 */
ZLV_DECLARE_SQL_TO_C( Char, Boolean )
{
    /*
     * ========================================================================================================
     * Test                                                    | *TargetValuePtr | *StrLen_or_IndPtr | SQLSTATE
     * ========================================================================================================
     * Data is "TRUE" or "FALSE"                               | Data            | 4 or 5            | n/a
     * Data is not a numeric-literal                           | Undefined       | Undefined         | 22018
     * ========================================================================================================
     */
    
    stlUInt8   * sTarget;
    stlStatus    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget = (stlUInt8*)aTargetValuePtr;

    STL_TRY_THROW( (aDataValue->mLength == 4)
                   || (aDataValue->mLength == 5),
                   RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION );

    if( stlStrncasecmp( "TRUE", aDataValue->mValue, aDataValue->mLength ) == 0 )
    {
        *sTarget = SQL_TRUE;
    }
    else if( stlStrncasecmp( "FALSE", aDataValue->mValue, aDataValue->mLength ) == 0 )
    {
        *sTarget = SQL_FALSE;
    }
    else
    {
        /* Neither "TRUE" nor "FALSE" */
        STL_THROW( RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION );
    }
    
    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = 1;
    }

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                      "The C type was an exact or approximate numeric, a datetime, or an interval data type; "
                      "the SQL type of the column was a character data type; "
                      "and the value in the column was not a valid literal of the bound C type.",
                      aErrorStack );
    }

    STL_FINISH;

    return sRet;
}

/*
 * SQL_CHAR -> SQL_C_BINARY
 */
ZLV_DECLARE_SQL_TO_C( Char, Binary )
{
    /*
     * ==========================================================================================
     * Test                                | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ==========================================================================================
     * Byte length of data <= BufferLength | Data            | Length of data in bytes | n/a
     * Byte length of data > BufferLength  | Truncated data  | Length of data          | 01004
     * ==========================================================================================
     */

    SQLLEN    sOffset;
    SQLLEN    sLength;
    stlStatus sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;
    sOffset  = *aOffset;

    STL_TRY_THROW( sOffset < aDataValue->mLength, RAMP_NO_DATA );

    sLength = STL_MIN( (aDataValue->mLength - sOffset), aArdRec->mOctetLength );

    stlMemcpy( aTargetValuePtr,
               (const void*)((stlChar*)aDataValue->mValue + sOffset),
               sLength );
                
    *aOffset = sOffset + sLength;
                
    sLength = aDataValue->mLength - sOffset;

    if( aIndicator != NULL )
    {
        *aIndicator = sLength;
    }

    STL_TRY_THROW( sLength <= aArdRec->mOctetLength,
                   RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED );

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
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
 * SQL_CHAR -> SQL_C_LONGVARBINARY
 */
ZLV_DECLARE_SQL_TO_C( Char, LongVarBinary )
{
    /*
     * ==========================================================================================
     * Test                                | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ==========================================================================================
     * Byte length of data <= BufferLength | Data            | Length of data in bytes | n/a
     * Byte length of data > BufferLength  | Truncated data  | Length of data          | 01004
     * ==========================================================================================
     */

    SQL_LONG_VARIABLE_LENGTH_STRUCT * sTarget;
    stlChar                         * sValue;
    stlInt64                          sBufferLength;

    SQLLEN    sOffset;
    SQLLEN    sLength;
    stlStatus sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;
    sOffset  = *aOffset;

    STL_TRY_THROW( sOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget       = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aTargetValuePtr;
    sValue        = (stlChar*)sTarget->arr;
    sBufferLength = sTarget->len;

    sLength = STL_MIN( (aDataValue->mLength - sOffset), sBufferLength );

    stlMemcpy( sValue,
               (const void*)((stlChar*)aDataValue->mValue + sOffset),
               sLength );
                
    *aOffset = sOffset + sLength;
                
    sLength = aDataValue->mLength - sOffset;

    if( aIndicator != NULL )
    {
        *aIndicator = sLength;
    }

    STL_TRY_THROW( sLength <= sBufferLength,
                   RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED );

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
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
 * SQL_CHAR -> SQL_C_TYPE_DATE
 */
ZLV_DECLARE_SQL_TO_C( Char, Date )
{
    /*
     * =======================================================================================================================
     * Test                                                                   | *TargetValuePtr | *StrLen_or_IndPtr | SQLSTATE
     * =======================================================================================================================
     * Data value is a valid date-value                                       | Data            | 6                 | n/a
     * Data value is a valid timestamp-value; time portion is zero            | Data            | 6                 | n/a
     * Data value is a valid timestamp-value; time portion is nonzero         | Truncated data  | 6                 | 01S07
     * Data value is not a valid date-value or timestamp-value                | Undefined       | Undefined         | 22018
     * =======================================================================================================================
     */

    dtlDataValue         sTimestampTz;
    dtlTimestampTzType   sTimestampTzValue;
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
    stlUInt32            sTimeZoneMinute;
    stlStatus            sRet = STL_FAILURE;
    stlBool              sSuccessWithInfo = STL_FALSE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget = (SQL_DATE_STRUCT*)aTargetValuePtr;

    STL_TRY( dtlInitDataValue( DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,
                               STL_SIZEOF(dtlTimestampTzType),
                               &sTimestampTz,
                               aErrorStack ) == STL_SUCCESS );

    sTimestampTz.mValue = &sTimestampTzValue;

    STL_TRY_THROW( dtlTimestampTzSetValueFromString( (stlChar*)aDataValue->mValue,
                                                     aDataValue->mLength,
                                                     DTL_TIMESTAMP_MAX_PRECISION,
                                                     0,
                                                     DTL_STRING_LENGTH_UNIT_NA,
                                                     DTL_INTERVAL_INDICATOR_NA,
                                                     STL_SIZEOF( dtlTimestampTzType ),
                                                     &sTimestampTz,
                                                     &sSuccessWithInfo,
                                                     ZLS_STMT_NLS_DT_VECTOR(aStmt),
                                                     aStmt,
                                                     ZLS_STMT_DT_VECTOR(aStmt),
                                                     aStmt,
                                                     aErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION );

    STL_TRY( dtlGetTimeInfoFromTimestampTz( &sTimestampTzValue,
                                            &sYear,
                                            &sMonth,
                                            &sDay,
                                            &sHour,
                                            &sMinute,
                                            &sSecond,
                                            &sFraction,
                                            &sTimeZoneSign,
                                            &sTimeZoneHour,
                                            &sTimeZoneMinute,
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

    STL_CATCH( RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                      "The C type was an exact or approximate numeric, a datetime, or an interval data type; "
                      "the SQL type of the column was a character data type; "
                      "and the value in the column was not a valid literal of the bound C type.",
                      aErrorStack );
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
 * SQL_CHAR -> SQL_C_TYPE_TIME
 */
ZLV_DECLARE_SQL_TO_C( Char, Time )
{
    /*
     * =======================================================================================================================
     * Test                                                                   | *TargetValuePtr | *StrLen_or_IndPtr | SQLSTATE
     * =======================================================================================================================
     * Data value is a valid time-value and the fractional seconds value is 0 | Data            | 6                 | n/a
     * Data value is a valid timestamp-value or a valid time-value;           | Data            | 6                 | n/a
     * fractional seconds portion is zero                                     |                 |                   |
     * Data value is a valid timestamp-value;                                 | Truncated data  | 6                 | 01S07
     * fractional seconds portion is nonzero                                  |                 |                   |
     * Data value is not a valid time-value or timestamp-value                | Undefined       | Undefined         | 22018
     * =======================================================================================================================
     */
    
    dtlDataValue         sTimeTz;
    dtlTimeTzType        sTimeTzValue;
    dtlDataValue         sTimestampTz;
    dtlTimestampTzType   sTimestampTzValue;
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
    stlUInt32            sTimeZoneMinute;
    stlStatus            sRet = STL_FAILURE;
    stlBool              sSuccessWithInfo = STL_FALSE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget = (SQL_TIME_STRUCT*)aTargetValuePtr;

    /* Input String (aDataValue->mValue)은
       Time의 형태를 가질 수도 있고, Timestamp의 형태도 가질 수 있다.
       따라서, 우선 Time 형식의 변환이 가능한지 살펴보고,
       실패할 경우 Timestamp 형식의 변환을 시도한다.
    */
    STL_TRY( dtlInitDataValue( DTL_TYPE_TIME_WITH_TIME_ZONE,
                               STL_SIZEOF(dtlTimeTzType),
                               &sTimeTz,
                               aErrorStack ) == STL_SUCCESS );

    sTimeTz.mValue = &sTimeTzValue;

    if( dtlTimeTzSetValueFromString( (stlChar*)aDataValue->mValue,
                                     aDataValue->mLength,
                                     DTL_TIME_MAX_PRECISION,
                                     0,
                                     DTL_STRING_LENGTH_UNIT_NA,
                                     DTL_INTERVAL_INDICATOR_NA,
                                     STL_SIZEOF( dtlTimeTzType ),
                                     &sTimeTz,
                                     &sSuccessWithInfo,
                                     ZLS_STMT_NLS_DT_VECTOR(aStmt),
                                     aStmt,
                                     ZLS_STMT_DT_VECTOR(aStmt),
                                     aStmt,
                                     aErrorStack ) == STL_SUCCESS )
    {
        /* Input string이 Time 형식(HH:MM:SS.SSS)을 갖는 경우 */
        STL_TRY( dtlGetTimeInfoFromTimeTz( &sTimeTzValue,
                                           &sHour,
                                           &sMinute,
                                           &sSecond,
                                           &sFraction,
                                           &sTimeZoneSign,
                                           &sTimeZoneHour,
                                           &sTimeZoneMinute,
                                           aErrorStack ) == STL_SUCCESS );
    }
    else
    {
        /* Input string이 Time 형식을 갖지 않는 경우,
           Timestamp 형식(YYYY-MM-DD HH:MM:SS.SSS)으로 시도
        */

        STL_INIT_ERROR_STACK( aErrorStack );
        
        STL_TRY( dtlInitDataValue( DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,
                                   STL_SIZEOF(dtlTimestampTzType),
                                   &sTimestampTz,
                                   aErrorStack ) == STL_SUCCESS );

        sTimestampTz.mValue = &sTimestampTzValue;
        
        STL_TRY_THROW( dtlTimestampTzSetValueFromString( (stlChar*)aDataValue->mValue,
                                                         aDataValue->mLength,
                                                         DTL_TIMESTAMP_MAX_PRECISION,
                                                         0,
                                                         DTL_STRING_LENGTH_UNIT_NA,
                                                         DTL_INTERVAL_INDICATOR_NA,
                                                         STL_SIZEOF( dtlTimestampTzType ),
                                                         &sTimestampTz,
                                                         &sSuccessWithInfo,
                                                         ZLS_STMT_NLS_DT_VECTOR(aStmt),
                                                         aStmt,
                                                         ZLS_STMT_DT_VECTOR(aStmt),
                                                         aStmt,
                                                         aErrorStack ) == STL_SUCCESS,
                       RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION );

        STL_TRY( dtlGetTimeInfoFromTimestampTz( &sTimestampTzValue,
                                                &sYear,
                                                &sMonth,
                                                &sDay,
                                                &sHour,
                                                &sMinute,
                                                &sSecond,
                                                &sFraction,
                                                &sTimeZoneSign,
                                                &sTimeZoneHour,
                                                &sTimeZoneMinute,
                                                aErrorStack ) == STL_SUCCESS );
    }

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

    STL_CATCH( RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                      "The C type was an exact or approximate numeric, a datetime, or an interval data type; "
                      "the SQL type of the column was a character data type; "
                      "and the value in the column was not a valid literal of the bound C type.",
                      aErrorStack );
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
 * SQL_CHAR -> SQL_C_TYPE_TIME_WITH_TIMEZONE
 */
ZLV_DECLARE_SQL_TO_C( Char, TimeTz )
{
    /*
     * =======================================================================================================================
     * Test                                                                   | *TargetValuePtr | *StrLen_or_IndPtr | SQLSTATE
     * =======================================================================================================================
     * Data value is a valid time-value and the fractional seconds value is 0 | Data            | 14                | n/a
     * Data value is a valid timestamp-value or a valid time-value;           | Data            | 14                | n/a
     * fractional seconds portion is zero                                     |                 |                   |
     * Data value is a valid timestamp-value;                                 | Truncated data  | 14                | 01S07
     * fractional seconds portion is nonzero                                  |                 |                   |
     * Data value is not a valid time-value or timestamp-value                | Undefined       | Undefined         | 22018
     * =======================================================================================================================
     */
    
    dtlDataValue       sTimeTz;
    dtlDataValue       sTimestampTz;
    dtlTimeTzType      sTimeTzValue;
    dtlTimestampTzType sTimestampTzValue;
    stlInt32           sYear;
    stlInt32           sMonth;
    stlInt32           sDay;
    stlInt32           sHour;
    stlInt32           sMinute;
    stlInt32           sSecond;
    stlInt32           sFraction;
    stlInt32           sTimeZoneSign;
    stlUInt32          sTimeZoneHour;
    stlUInt32          sTimeZoneMinute;
    stlStatus          sRet = STL_FAILURE;
    stlBool            sSuccessWithInfo = STL_FALSE;
    
    SQL_TIME_WITH_TIMEZONE_STRUCT * sTarget;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget = (SQL_TIME_WITH_TIMEZONE_STRUCT*)aTargetValuePtr;

    /* Input String (aDataValue->mValue)은
       Time의 형태를 가질 수도 있고, Timestamp의 형태도 가질 수 있다.
       따라서, 우선 Time 형식의 변환이 가능한지 살펴보고,
       실패할 경우 Timestamp 형식의 변환을 시도한다.
    */
    STL_TRY( dtlInitDataValue( DTL_TYPE_TIME_WITH_TIME_ZONE,
                               STL_SIZEOF(dtlTimeTzType),
                               &sTimeTz,
                               aErrorStack ) == STL_SUCCESS );
    STL_TRY( dtlInitDataValue( DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,
                               STL_SIZEOF(dtlTimestampTzType),
                               &sTimestampTz,
                               aErrorStack ) == STL_SUCCESS );
    sTimeTz.mValue = &sTimeTzValue;
    sTimestampTz.mValue = &sTimestampTzValue;

    if( dtlTimeTzSetValueFromString( (stlChar*)aDataValue->mValue,
                                     aDataValue->mLength,
                                     DTL_TIMETZ_MAX_PRECISION,
                                     0,
                                     DTL_STRING_LENGTH_UNIT_NA,
                                     DTL_INTERVAL_INDICATOR_NA,
                                     DTL_TIMETZ_SIZE,
                                     &sTimeTz,
                                     &sSuccessWithInfo,
                                     ZLS_STMT_NLS_DT_VECTOR(aStmt),
                                     aStmt,
                                     ZLS_STMT_DT_VECTOR(aStmt),
                                     aStmt,
                                     aErrorStack )
        == STL_SUCCESS )
    {
        /* Input string이 Time 형식(HH:MM:SS.SSS)을 갖는 경우 */
        STL_TRY( dtlGetTimeInfoFromTimeTz( &sTimeTzValue,
                                           &sHour,
                                           &sMinute,
                                           &sSecond,
                                           &sFraction,
                                           &sTimeZoneSign,
                                           &sTimeZoneHour,
                                           &sTimeZoneMinute,
                                           aErrorStack ) == STL_SUCCESS );
    }
    else
    {
        /* Input string이 Time 형식을 갖지 않는 경우,
           Timestamp 형식(YYYY-MM-DD HH:MM:SS.SSS)으로 시도
        */
        STL_TRY_THROW( dtlTimestampTzSetValueFromString( (stlChar*)aDataValue->mValue,
                                                         aDataValue->mLength,
                                                         DTL_TIMESTAMPTZ_MAX_PRECISION,
                                                         0,
                                                         DTL_STRING_LENGTH_UNIT_NA,
                                                         DTL_INTERVAL_INDICATOR_NA,
                                                         DTL_TIMESTAMPTZ_SIZE,
                                                         &sTimestampTz,
                                                         &sSuccessWithInfo,
                                                         ZLS_STMT_NLS_DT_VECTOR(aStmt),
                                                         aStmt,
                                                         ZLS_STMT_DT_VECTOR(aStmt),
                                                         aStmt,
                                                         aErrorStack ) == STL_SUCCESS,
                       RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION );

        STL_TRY( dtlGetTimeInfoFromTimestampTz( &sTimestampTzValue,
                                                &sYear,
                                                &sMonth,
                                                &sDay,
                                                &sHour,
                                                &sMinute,
                                                &sSecond,
                                                &sFraction,
                                                &sTimeZoneSign,
                                                &sTimeZoneHour,
                                                &sTimeZoneMinute,
                                                aErrorStack ) == STL_SUCCESS );
    }

    sTarget->hour   = (stlUInt16)sHour;
    sTarget->minute = (stlUInt16)sMinute;
    sTarget->second = (stlUInt16)sSecond;
    sTarget->fraction = (stlUInt32)sFraction;
    switch( sTimeZoneSign )
    {
        case 0:  /* '-' */
            sTarget->timezone_hour = -(stlInt16)sTimeZoneHour;
            sTarget->timezone_minute = -(stlInt16)sTimeZoneMinute;
            break;
        case 1:  /* '+' */
            sTarget->timezone_hour = (stlInt16)sTimeZoneHour;
            sTarget->timezone_minute = (stlInt16)sTimeZoneMinute;
            break;
        default:
            STL_THROW( RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION );
            break;
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

    STL_CATCH( RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                      "The C type was an exact or approximate numeric, a datetime, or an interval data type; "
                      "the SQL type of the column was a character data type; "
                      "and the value in the column was not a valid literal of the bound C type.",
                      aErrorStack );
    }

    STL_FINISH;

    return sRet;
}

/*
 * SQL_CHAR -> SQL_C_TYPE_TIMESTAMP
 */
ZLV_DECLARE_SQL_TO_C( Char, Timestamp )
{
    /*
     * =====================================================================================================================
     * Test                                                                 | *TargetValuePtr | *StrLen_or_IndPtr | SQLSTATE
     * =====================================================================================================================
     * Data value is a valid timestamp-value or a valid time-value;         | Data            | 16                | n/a
     * fractional seconds portion not truncated                             |                 |                   |
     * Data value is a valid timestamp-value or a valid time-value;         | Truncated data  | 16                | 01S07
     * fractional seconds portion truncated                                 |                 |                   |
     * Data value is a valid date-value                                     | Data            | 16                | n/a
     * Data value is a valid time-value                                     | Data            | 16                | n/a
     * Data value is not a valid date-value, time-value, or timestamp-value | Undefined       | Undefined         | 22018
     * =====================================================================================================================
     */

    dtlDataValue           sTimeTz;
    dtlTimeTzType          sTimeTzValue;
    dtlDataValue           sTimestampTz;
    dtlTimestampTzType     sTimestampTzValue;
    SQL_TIMESTAMP_STRUCT * sTarget;
    stlInt32               sYear = 0;
    stlInt32               sMonth = 0;
    stlInt32               sDay = 0;
    stlInt32               sHour = 0;
    stlInt32               sMinute = 0;
    stlInt32               sSecond = 0;
    stlInt32               sFraction = 0;
    stlInt32               sTimeZoneSign;
    stlUInt32              sTimeZoneHour;
    stlUInt32              sTimeZoneMinute;
    stlStatus              sRet = STL_FAILURE;
    stlBool                sSuccessWithInfo = STL_FALSE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget = (SQL_TIMESTAMP_STRUCT*)aTargetValuePtr;

    /* Input String (aDataValue->mValue)은
       Time의 형태를 가질 수도 있고, Timestamp의 형태도 가질 수 있다.
       따라서, 우선 Time 형식의 변환이 가능한지 살펴보고,
       실패할 경우 Timestamp 형식의 변환을 시도한다.
    */
    STL_TRY( dtlInitDataValue( DTL_TYPE_TIME_WITH_TIME_ZONE,
                               STL_SIZEOF(dtlTimeTzType),
                               &sTimeTz,
                               aErrorStack ) == STL_SUCCESS );

    sTimeTz.mValue = &sTimeTzValue;

    if( dtlTimeTzSetValueFromString( (stlChar*)aDataValue->mValue,
                                     aDataValue->mLength,
                                     DTL_TIME_MAX_PRECISION,
                                     0,
                                     DTL_STRING_LENGTH_UNIT_NA,
                                     DTL_INTERVAL_INDICATOR_NA,
                                     STL_SIZEOF( dtlTimeTzType ),
                                     &sTimeTz,
                                     &sSuccessWithInfo,
                                     ZLS_STMT_NLS_DT_VECTOR(aStmt),
                                     aStmt,
                                     ZLS_STMT_DT_VECTOR(aStmt),
                                     aStmt,
                                     aErrorStack ) == STL_SUCCESS )
    {
        /* Input string이 Time 형식(HH:MM:SS.SSS)을 갖는 경우 */
        STL_TRY( dtlGetTimeInfoFromTimeTz( &sTimeTzValue,
                                           &sHour,
                                           &sMinute,
                                           &sSecond,
                                           &sFraction,
                                           &sTimeZoneSign,
                                           &sTimeZoneHour,
                                           &sTimeZoneMinute,
                                           aErrorStack ) == STL_SUCCESS );
    }
    else
    {
        /* Input string이 Time 형식을 갖지 않는 경우,
           Timestamp 형식(YYYY-MM-DD HH:MM:SS.SSS)으로 시도
        */
        STL_INIT_ERROR_STACK( aErrorStack );
        
        STL_TRY( dtlInitDataValue( DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,
                                   STL_SIZEOF(dtlTimestampTzType),
                                   &sTimestampTz,
                                   aErrorStack ) == STL_SUCCESS );

        sTimestampTz.mValue = &sTimestampTzValue;
        
        STL_TRY_THROW( dtlTimestampTzSetValueFromString( (stlChar*)aDataValue->mValue,
                                                         aDataValue->mLength,
                                                         DTL_TIMESTAMP_MAX_PRECISION,
                                                         0,
                                                         DTL_STRING_LENGTH_UNIT_NA,
                                                         DTL_INTERVAL_INDICATOR_NA,
                                                         STL_SIZEOF( dtlTimestampTzType ),
                                                         &sTimestampTz,
                                                         &sSuccessWithInfo,
                                                         ZLS_STMT_NLS_DT_VECTOR(aStmt),
                                                         aStmt,
                                                         ZLS_STMT_DT_VECTOR(aStmt),
                                                         aStmt,
                                                         aErrorStack ) == STL_SUCCESS,
                       RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION );

        STL_TRY( dtlGetTimeInfoFromTimestampTz( &sTimestampTzValue,
                                                &sYear,
                                                &sMonth,
                                                &sDay,
                                                &sHour,
                                                &sMinute,
                                                &sSecond,
                                                &sFraction,
                                                &sTimeZoneSign,
                                                &sTimeZoneHour,
                                                &sTimeZoneMinute,
                                                aErrorStack ) == STL_SUCCESS );
    }

    sTarget->year     = (stlInt16)sYear;
    sTarget->month    = (stlUInt16)sMonth;
    sTarget->day      = (stlUInt16)sDay;
    sTarget->hour     = (stlUInt16)sHour;
    sTarget->minute   = (stlUInt16)sMinute;
    sTarget->second   = (stlUInt16)sSecond;
    sTarget->fraction = (stlUInt32)sFraction;
                                    
    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = 16;
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

    STL_CATCH( RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                      "The C type was an exact or approximate numeric, a datetime, or an interval data type; "
                      "the SQL type of the column was a character data type; "
                      "and the value in the column was not a valid literal of the bound C type.",
                      aErrorStack );
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
 * SQL_CHAR -> SQL_C_TYPE_TIMESTAMP_WITH_TIMEZONE
 */
ZLV_DECLARE_SQL_TO_C( Char, TimestampTz )
{
    /*
     * =====================================================================================================================
     * Test                                                                 | *TargetValuePtr | *StrLen_or_IndPtr | SQLSTATE
     * =====================================================================================================================
     * Data value is a valid timestamp-value or a valid time-value;         | Data            | 20                | n/a
     * fractional seconds portion not truncated                             |                 |                   |
     * Data value is a valid timestamp-value or a valid time-value;         | Truncated data  | 20                | 01S07
     * fractional seconds portion truncated                                 |                 |                   |
     * Data value is a valid date-value                                     | Data            | 20                | n/a
     * Data value is a valid time-value                                     | Data            | 20                | n/a
     * Data value is not a valid date-value, time-value, or timestamp-value | Undefined       | Undefined         | 22018
     * =====================================================================================================================
     */
    
    dtlDataValue       sTimeTz;
    dtlDataValue       sTimestampTz;
    dtlTimeTzType      sTimeTzValue;
    dtlTimestampTzType sTimestampTzValue;
    stlInt32           sYear = 0;
    stlInt32           sMonth = 0;
    stlInt32           sDay = 0;
    stlInt32           sHour = 0;
    stlInt32           sMinute = 0;
    stlInt32           sSecond = 0;
    stlInt32           sFraction = 0;
    stlInt32           sTimeZoneSign = 0;
    stlUInt32          sTimeZoneHour = 0;
    stlUInt32          sTimeZoneMinute = 0;
    stlStatus          sRet = STL_FAILURE;
    stlBool            sSuccessWithInfo = STL_FALSE;
    
    SQL_TIMESTAMP_WITH_TIMEZONE_STRUCT * sTarget;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget = (SQL_TIMESTAMP_WITH_TIMEZONE_STRUCT*)aTargetValuePtr;

    /* Input String (aDataValue->mValue)은
       Time의 형태를 가질 수도 있고, Timestamp의 형태도 가질 수 있다.
       따라서, 우선 Time 형식의 변환이 가능한지 살펴보고,
       실패할 경우 Timestamp 형식의 변환을 시도한다.
    */
    STL_TRY( dtlInitDataValue( DTL_TYPE_TIME_WITH_TIME_ZONE,
                               STL_SIZEOF(dtlTimeTzType),
                               &sTimeTz,
                               aErrorStack ) == STL_SUCCESS );
    STL_TRY( dtlInitDataValue( DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,
                               STL_SIZEOF(dtlTimestampTzType),
                               &sTimestampTz,
                               aErrorStack ) == STL_SUCCESS );

    sTimeTz.mValue = &sTimeTzValue;
    sTimestampTz.mValue = &sTimestampTzValue;

    if( dtlTimeTzSetValueFromString( (stlChar*)aDataValue->mValue,
                                     aDataValue->mLength,
                                     DTL_TIMETZ_MAX_PRECISION,
                                     0,
                                     DTL_STRING_LENGTH_UNIT_NA,
                                     DTL_INTERVAL_INDICATOR_NA,
                                     DTL_TIMETZ_SIZE,
                                     &sTimeTz,
                                     &sSuccessWithInfo,
                                     ZLS_STMT_NLS_DT_VECTOR(aStmt),
                                     aStmt,
                                     ZLS_STMT_DT_VECTOR(aStmt),
                                     aStmt,
                                     aErrorStack )
        == STL_SUCCESS )
    {
        /* Input string이 Time 형식(HH:MM:SS.SSS)을 갖는 경우 */
        STL_TRY( dtlGetTimeInfoFromTimeTz( &sTimeTzValue,
                                           &sHour,
                                           &sMinute,
                                           &sSecond,
                                           &sFraction,
                                           &sTimeZoneSign,
                                           &sTimeZoneHour,
                                           &sTimeZoneMinute,
                                           aErrorStack ) == STL_SUCCESS );
    }
    else
    {
        /* Input string이 Time 형식을 갖지 않는 경우,
           Timestamp 형식(YYYY-MM-DD HH:MM:SS.SSS)으로 시도
        */
        STL_TRY_THROW( dtlTimestampTzSetValueFromString( (stlChar*)aDataValue->mValue,
                                                         aDataValue->mLength,
                                                         DTL_TIMESTAMPTZ_MAX_PRECISION,
                                                         0,
                                                         DTL_STRING_LENGTH_UNIT_NA,
                                                         DTL_INTERVAL_INDICATOR_NA,
                                                         DTL_TIMESTAMPTZ_SIZE,
                                                         &sTimestampTz,
                                                         &sSuccessWithInfo,
                                                         ZLS_STMT_NLS_DT_VECTOR(aStmt),
                                                         aStmt,
                                                         ZLS_STMT_DT_VECTOR(aStmt),
                                                         aStmt,
                                                         aErrorStack ) == STL_SUCCESS,
                       RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION );

        STL_TRY( dtlGetTimeInfoFromTimestampTz( &sTimestampTzValue,
                                                &sYear,
                                                &sMonth,
                                                &sDay,
                                                &sHour,
                                                &sMinute,
                                                &sSecond,
                                                &sFraction,
                                                &sTimeZoneSign,
                                                &sTimeZoneHour,
                                                &sTimeZoneMinute,
                                                aErrorStack ) == STL_SUCCESS );
    }

    sTarget->year     = (stlInt16)sYear;
    sTarget->month    = (stlUInt16)sMonth;
    sTarget->day      = (stlUInt16)sDay;
    sTarget->hour     = (stlUInt16)sHour;
    sTarget->minute   = (stlUInt16)sMinute;
    sTarget->second   = (stlUInt16)sSecond;
    sTarget->fraction = (stlUInt32)sFraction;
    switch( sTimeZoneSign )
    {
        case 0:  /* '-' */
            sTarget->timezone_hour = -(stlInt16)sTimeZoneHour;
            sTarget->timezone_minute = -(stlInt16)sTimeZoneMinute;
            break;
        case 1:  /* '+' */
            sTarget->timezone_hour = (stlInt16)sTimeZoneHour;
            sTarget->timezone_minute = (stlInt16)sTimeZoneMinute;
            break;
        default:
            STL_THROW( RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION );
            break;
    }
                                    
    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = 20;
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

    STL_CATCH( RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                      "The C type was an exact or approximate numeric, a datetime, or an interval data type; "
                      "the SQL type of the column was a character data type; "
                      "and the value in the column was not a valid literal of the bound C type.",
                      aErrorStack );
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
 * SQL_CHAR -> SQL_C_INTERVAL_MONTH
 */
ZLV_DECLARE_SQL_TO_C( Char, IntervalMonth )
{
    /*
     * ==========================================================================================================================
     * Test                                                                | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ==========================================================================================================================
     * Data value is a valid interval value; no truncation                 | Data            | Length of data in bytes | n/a
     * Data value is a valid interval value; truncation of one or          | Truncated       | Length of data in bytes | 01S07
     * more trailing fields                                                |                 |                         |
     * Data is valid interval; leading field significant precision is lost | Undefined       | Undefined               | 22015
     * The data value is not a valid interval value                        | Undefined       | Undefined               | 22018
     * ==========================================================================================================================
     */
    
    dtlDataValue                 sInterval;
    dtlIntervalYearToMonthType   sIntervalValue;
    SQL_INTERVAL_STRUCT        * sTarget;
    stlUInt32                    sYear;
    stlUInt32                    sMonth;
    stlBool                      sIsNegative;
    stlStatus                    sRet = STL_FAILURE;
    stlBool                      sSuccessWithInfo = STL_FALSE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget = (SQL_INTERVAL_STRUCT*)aTargetValuePtr;
    stlMemset( sTarget, 0x00, STL_SIZEOF( SQL_INTERVAL_STRUCT ) );

    STL_TRY( dtlInitDataValue( DTL_TYPE_INTERVAL_YEAR_TO_MONTH,
                               STL_SIZEOF(dtlIntervalYearToMonthType),
                               &sInterval,
                               aErrorStack ) == STL_SUCCESS );

    sIntervalValue.mIndicator = aArdRec->mDatetimeIntervalCode;
    sInterval.mValue = &sIntervalValue;

    STL_TRY( dtlIntervalYearToMonthSetNotTruncatedValueFromString(
                 (stlChar*)aDataValue->mValue,
                 aDataValue->mLength,
                 aArdRec->mDatetimeIntervalPrecision,
                 aArdRec->mPrecision,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_MONTH,
                 STL_SIZEOF( dtlIntervalYearToMonthType ),
                 &sInterval,
                 &sSuccessWithInfo,
                 ZLS_STMT_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    STL_TRY( dtlGetTimeInfoFromYearMonthInterval( &sIntervalValue,
                                                  &sYear,
                                                  &sMonth,
                                                  &sIsNegative,
                                                  aErrorStack ) == STL_SUCCESS );

    sTarget->interval_type = aArdRec->mDatetimeIntervalCode;
    sTarget->interval_sign = ( sIsNegative == STL_TRUE ) ? SQL_TRUE : SQL_FALSE;

    sTarget->intval.year_month.month = ( sYear * DTL_MONTHS_PER_YEAR ) + sMonth;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_SIZEOF( *sTarget );
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
 * SQL_CHAR -> SQL_C_INTERVAL_YEAR
 */
ZLV_DECLARE_SQL_TO_C( Char, IntervalYear )
{
    /*
     * ==========================================================================================================================
     * Test                                                                | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ==========================================================================================================================
     * Data value is a valid interval value; no truncation                 | Data            | Length of data in bytes | n/a
     * Data value is a valid interval value; truncation of one or          | Truncated       | Length of data in bytes | 01S07
     * more trailing fields                                                |                 |                         |
     * Data is valid interval; leading field significant precision is lost | Undefined       | Undefined               | 22015
     * The data value is not a valid interval value                        | Undefined       | Undefined               | 22018
     * ==========================================================================================================================
     */
    
    dtlDataValue                 sInterval;
    dtlIntervalYearToMonthType   sIntervalValue;
    SQL_INTERVAL_STRUCT        * sTarget;
    stlUInt32                    sYear;
    stlUInt32                    sMonth;
    stlBool                      sIsNegative;
    stlStatus                    sRet = STL_FAILURE;
    stlFloat64                   sPow;
    stlBool                      sSuccessWithInfo = STL_FALSE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget = (SQL_INTERVAL_STRUCT*)aTargetValuePtr;
    stlMemset( sTarget, 0x00, STL_SIZEOF( SQL_INTERVAL_STRUCT ) );

    STL_TRY( dtlInitDataValue( DTL_TYPE_INTERVAL_YEAR_TO_MONTH,
                               STL_SIZEOF(dtlIntervalYearToMonthType),
                               &sInterval,
                               aErrorStack ) == STL_SUCCESS );

    sIntervalValue.mIndicator = aArdRec->mDatetimeIntervalCode;
    sInterval.mValue = &sIntervalValue;

    STL_TRY( dtlIntervalYearToMonthSetNotTruncatedValueFromString(
                 (stlChar*)aDataValue->mValue,
                 aDataValue->mLength,
                 aArdRec->mDatetimeIntervalPrecision,
                 aArdRec->mPrecision,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_YEAR,
                 STL_SIZEOF( dtlIntervalYearToMonthType ),
                 &sInterval,
                 &sSuccessWithInfo,
                 ZLS_STMT_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    STL_TRY( dtlGetTimeInfoFromYearMonthInterval( &sIntervalValue,
                                                  &sYear,
                                                  &sMonth,
                                                  &sIsNegative,
                                                  aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( stlGet10Pow( aArdRec->mDatetimeIntervalPrecision, &sPow, aErrorStack ) == STL_TRUE,
                   RAMP_ERR_INTERVAL_FIELD_OVERFLOW );
    STL_TRY_THROW( sYear < sPow, RAMP_ERR_INTERVAL_FIELD_OVERFLOW );

    sTarget->interval_type = aArdRec->mDatetimeIntervalCode;
    sTarget->interval_sign = ( sIsNegative == STL_TRUE ) ? SQL_TRUE : SQL_FALSE;

    sTarget->intval.year_month.year = sYear;
                                    
    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_SIZEOF( *sTarget );
    }

    STL_TRY_THROW( ( sMonth == 0 ), RAMP_WARNING_FRACTIONAL_TRUNCATION );
    STL_TRY_THROW( sSuccessWithInfo == STL_FALSE, RAMP_WARNING_FRACTIONAL_TRUNCATION );

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_ERR_INTERVAL_FIELD_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INTERVAL_FIELD_OVERFLOW,
                      "Assigning from an exact numeric or interval SQL type to an interval C type "
                      "caused a loss of significant digits in the leading field.",
                      aErrorStack );
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
 * SQL_CHAR -> SQL_C_INTERVAL_YEAR_TO_MONTH
 */
ZLV_DECLARE_SQL_TO_C( Char, IntervalYearToMonth )
{
    /*
     * ==========================================================================================================================
     * Test                                                                | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ==========================================================================================================================
     * Data value is a valid interval value; no truncation                 | Data            | Length of data in bytes | n/a
     * Data value is a valid interval value; truncation of one or          | Truncated       | Length of data in bytes | 01S07
     * more trailing fields                                                |                 |                         |
     * Data is valid interval; leading field significant precision is lost | Undefined       | Undefined               | 22015
     * The data value is not a valid interval value                        | Undefined       | Undefined               | 22018
     * ==========================================================================================================================
     */
    
    dtlDataValue                 sInterval;
    dtlIntervalYearToMonthType   sIntervalValue;
    SQL_INTERVAL_STRUCT        * sTarget;
    stlUInt32                    sYear;
    stlUInt32                    sMonth;
    stlBool                      sIsNegative;
    stlStatus                    sRet = STL_FAILURE;
    stlBool                      sSuccessWithInfo = STL_FALSE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget = (SQL_INTERVAL_STRUCT*)aTargetValuePtr;
    stlMemset( sTarget, 0x00, STL_SIZEOF( SQL_INTERVAL_STRUCT ) );

    STL_TRY( dtlInitDataValue( DTL_TYPE_INTERVAL_YEAR_TO_MONTH,
                               STL_SIZEOF(dtlIntervalYearToMonthType),
                               &sInterval,
                               aErrorStack ) == STL_SUCCESS );

    sIntervalValue.mIndicator = aArdRec->mDatetimeIntervalCode;
    sInterval.mValue = &sIntervalValue;

    STL_TRY( dtlIntervalYearToMonthSetNotTruncatedValueFromString(
                 (stlChar*)aDataValue->mValue,
                 aDataValue->mLength,
                 aArdRec->mDatetimeIntervalPrecision,
                 aArdRec->mPrecision,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH,
                 STL_SIZEOF( dtlIntervalYearToMonthType ),
                 &sInterval,
                 &sSuccessWithInfo,
                 ZLS_STMT_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    STL_TRY( dtlGetTimeInfoFromYearMonthInterval( &sIntervalValue,
                                                  &sYear,
                                                  &sMonth,
                                                  &sIsNegative,
                                                  aErrorStack ) == STL_SUCCESS );

    sTarget->interval_type = aArdRec->mDatetimeIntervalCode;
    sTarget->interval_sign = ( sIsNegative == STL_TRUE ) ? SQL_TRUE : SQL_FALSE;

    sTarget->intval.year_month.year  = sYear;
    sTarget->intval.year_month.month = sMonth;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_SIZEOF( *sTarget );
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
 * SQL_CHAR -> SQL_C_INTERVAL_DAY
 */
ZLV_DECLARE_SQL_TO_C( Char, IntervalDay )
{
    /*
     * ==========================================================================================================================
     * Test                                                                | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ==========================================================================================================================
     * Data value is a valid interval value; no truncation                 | Data            | Length of data in bytes | n/a
     * Data value is a valid interval value; truncation of one or          | Truncated       | Length of data in bytes | 01S07
     * more trailing fields                                                |                 |                         |
     * Data is valid interval; leading field significant precision is lost | Undefined       | Undefined               | 22015
     * The data value is not a valid interval value                        | Undefined       | Undefined               | 22018
     * ==========================================================================================================================
     */
    
    dtlDataValue                 sInterval;
    dtlIntervalDayToSecondType   sIntervalValue;
    SQL_INTERVAL_STRUCT        * sTarget;
    stlUInt32                    sDay;
    stlUInt32                    sHour;
    stlUInt32                    sMinute;
    stlUInt32                    sSecond;
    stlUInt32                    sFraction;
    stlBool                      sIsNegative;
    stlStatus                    sRet = STL_FAILURE;
    stlBool                      sSuccessWithInfo = STL_FALSE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget = (SQL_INTERVAL_STRUCT*)aTargetValuePtr;
    stlMemset( sTarget, 0x00, STL_SIZEOF( SQL_INTERVAL_STRUCT ) );

    STL_TRY( dtlInitDataValue( DTL_TYPE_INTERVAL_DAY_TO_SECOND,
                               STL_SIZEOF(dtlIntervalDayToSecondType),
                               &sInterval,
                               aErrorStack ) == STL_SUCCESS );

    sIntervalValue.mIndicator = aArdRec->mDatetimeIntervalCode;
    sInterval.mValue = &sIntervalValue;

    STL_TRY( dtlIntervalDayToSecondSetNotTruncatedValueFromString(
                 (stlChar*)aDataValue->mValue,
                 aDataValue->mLength,
                 aArdRec->mDatetimeIntervalPrecision,
                 aArdRec->mPrecision,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_DAY,
                 STL_SIZEOF( dtlIntervalDayToSecondType ),
                 &sInterval,
                 &sSuccessWithInfo,
                 ZLS_STMT_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    STL_TRY( dtlGetTimeInfoFromDaySecondInterval( &sIntervalValue,
                                                  &sDay,
                                                  &sHour,
                                                  &sMinute,
                                                  &sSecond,
                                                  &sFraction,
                                                  &sIsNegative,
                                                  aErrorStack ) == STL_SUCCESS );
    
    sTarget->interval_type = aArdRec->mDatetimeIntervalCode;
    sTarget->interval_sign = ( sIsNegative == STL_TRUE ) ? SQL_TRUE : SQL_FALSE;

    sTarget->intval.day_second.day = sDay;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_SIZEOF( *sTarget );
    }

    STL_TRY_THROW( ( sHour == 0 ) &&
                   ( sMinute == 0 ) &&
                   ( sSecond == 0 ) &&
                   ( sFraction == 0 ),
                   RAMP_WARNING_FRACTIONAL_TRUNCATION );

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
 * SQL_CHAR -> SQL_C_INTERVAL_HOUR
 */
ZLV_DECLARE_SQL_TO_C( Char, IntervalHour )
{
    /*
     * ==========================================================================================================================
     * Test                                                                | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ==========================================================================================================================
     * Data value is a valid interval value; no truncation                 | Data            | Length of data in bytes | n/a
     * Data value is a valid interval value; truncation of one or          | Truncated       | Length of data in bytes | 01S07
     * more trailing fields                                                |                 |                         |
     * Data is valid interval; leading field significant precision is lost | Undefined       | Undefined               | 22015
     * The data value is not a valid interval value                        | Undefined       | Undefined               | 22018
     * ==========================================================================================================================
     */
    
    dtlDataValue                 sInterval;
    dtlIntervalDayToSecondType   sIntervalValue;
    SQL_INTERVAL_STRUCT        * sTarget;
    stlUInt32                    sDay;
    stlUInt32                    sHour;
    stlUInt32                    sMinute;
    stlUInt32                    sSecond;
    stlUInt32                    sFraction;
    stlBool                      sIsNegative;
    stlStatus                    sRet = STL_FAILURE;
    stlBool                      sSuccessWithInfo = STL_FALSE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget = (SQL_INTERVAL_STRUCT*)aTargetValuePtr;
    stlMemset( sTarget, 0x00, STL_SIZEOF( SQL_INTERVAL_STRUCT ) );

    STL_TRY( dtlInitDataValue( DTL_TYPE_INTERVAL_DAY_TO_SECOND,
                               STL_SIZEOF(dtlIntervalDayToSecondType),
                               &sInterval,
                               aErrorStack ) == STL_SUCCESS );

    sIntervalValue.mIndicator = aArdRec->mDatetimeIntervalCode;
    sInterval.mValue = &sIntervalValue;

    STL_TRY( dtlIntervalDayToSecondSetNotTruncatedValueFromString(
                 (stlChar*)aDataValue->mValue,
                 aDataValue->mLength,
                 aArdRec->mDatetimeIntervalPrecision,
                 aArdRec->mPrecision,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_HOUR,
                 STL_SIZEOF( dtlIntervalDayToSecondType ),
                 &sInterval,
                 &sSuccessWithInfo,
                 ZLS_STMT_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    STL_TRY( dtlGetTimeInfoFromDaySecondInterval( &sIntervalValue,
                                                  &sDay,
                                                  &sHour,
                                                  &sMinute,
                                                  &sSecond,
                                                  &sFraction,
                                                  &sIsNegative,
                                                  aErrorStack ) == STL_SUCCESS );
    
    sTarget->interval_type = aArdRec->mDatetimeIntervalCode;
    sTarget->interval_sign = ( sIsNegative == STL_TRUE ) ? SQL_TRUE : SQL_FALSE;

    sTarget->intval.day_second.hour = ( sDay * DTL_HOURS_PER_DAY ) + sHour;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_SIZEOF( *sTarget );
    }

    STL_TRY_THROW( ( sMinute == 0 ) &&
                   ( sSecond == 0 ) &&
                   ( sFraction == 0 ),
                   RAMP_WARNING_FRACTIONAL_TRUNCATION );

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
 * SQL_CHAR -> SQL_C_INTERVAL_MINUTE
 */
ZLV_DECLARE_SQL_TO_C( Char, IntervalMinute )
{
    /*
     * ==========================================================================================================================
     * Test                                                                | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ==========================================================================================================================
     * Data value is a valid interval value; no truncation                 | Data            | Length of data in bytes | n/a
     * Data value is a valid interval value; truncation of one or          | Truncated       | Length of data in bytes | 01S07
     * more trailing fields                                                |                 |                         |
     * Data is valid interval; leading field significant precision is lost | Undefined       | Undefined               | 22015
     * The data value is not a valid interval value                        | Undefined       | Undefined               | 22018
     * ==========================================================================================================================
     */
    
    dtlDataValue                 sInterval;
    dtlIntervalDayToSecondType   sIntervalValue;
    SQL_INTERVAL_STRUCT        * sTarget;
    stlUInt32                    sDay;
    stlUInt32                    sHour;
    stlUInt32                    sMinute;
    stlUInt32                    sSecond;
    stlUInt32                    sFraction;
    stlBool                      sIsNegative;
    stlStatus                    sRet = STL_FAILURE;
    stlBool                      sSuccessWithInfo = STL_FALSE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget = (SQL_INTERVAL_STRUCT*)aTargetValuePtr;
    stlMemset( sTarget, 0x00, STL_SIZEOF( SQL_INTERVAL_STRUCT ) );

    STL_TRY( dtlInitDataValue( DTL_TYPE_INTERVAL_DAY_TO_SECOND,
                               STL_SIZEOF(dtlIntervalDayToSecondType),
                               &sInterval,
                               aErrorStack ) == STL_SUCCESS );

    sIntervalValue.mIndicator = aArdRec->mDatetimeIntervalCode;
    sInterval.mValue = &sIntervalValue;

    STL_TRY( dtlIntervalDayToSecondSetNotTruncatedValueFromString(
                 (stlChar*)aDataValue->mValue,
                 aDataValue->mLength,
                 aArdRec->mDatetimeIntervalPrecision,
                 aArdRec->mPrecision,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_MINUTE,
                 STL_SIZEOF( dtlIntervalDayToSecondType ),
                 &sInterval,
                 &sSuccessWithInfo,
                 ZLS_STMT_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    STL_TRY( dtlGetTimeInfoFromDaySecondInterval( &sIntervalValue,
                                                  &sDay,
                                                  &sHour,
                                                  &sMinute,
                                                  &sSecond,
                                                  &sFraction,
                                                  &sIsNegative,
                                                  aErrorStack ) == STL_SUCCESS );
    
    sTarget->interval_type = aArdRec->mDatetimeIntervalCode;
    sTarget->interval_sign = ( sIsNegative == STL_TRUE ) ? SQL_TRUE : SQL_FALSE;

    sTarget->intval.day_second.minute = ( ( ( sDay * DTL_HOURS_PER_DAY ) + sHour ) * DTL_MINS_PER_HOUR ) + sMinute;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_SIZEOF( *sTarget );
    }

    STL_TRY_THROW( ( sSecond == 0 ) &&
                   ( sFraction == 0 ),
                   RAMP_WARNING_FRACTIONAL_TRUNCATION );

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
 * SQL_CHAR -> SQL_C_INTERVAL_SECOND
 */
ZLV_DECLARE_SQL_TO_C( Char, IntervalSecond )
{
    /*
     * ==========================================================================================================================
     * Test                                                                | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ==========================================================================================================================
     * Data value is a valid interval value; no truncation                 | Data            | Length of data in bytes | n/a
     * Data value is a valid interval value; truncation of one or          | Truncated       | Length of data in bytes | 01S07
     * more trailing fields                                                |                 |                         |
     * Data is valid interval; leading field significant precision is lost | Undefined       | Undefined               | 22015
     * The data value is not a valid interval value                        | Undefined       | Undefined               | 22018
     * ==========================================================================================================================
     */
    
    dtlDataValue                 sInterval;
    dtlIntervalDayToSecondType   sIntervalValue;
    SQL_INTERVAL_STRUCT        * sTarget;
    stlUInt32                    sDay;
    stlUInt32                    sHour;
    stlUInt32                    sMinute;
    stlUInt32                    sSecond;
    stlUInt32                    sFraction;
    stlBool                      sIsNegative;
    stlStatus                    sRet = STL_FAILURE;
    stlBool                      sSuccessWithInfo = STL_FALSE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget = (SQL_INTERVAL_STRUCT*)aTargetValuePtr;
    stlMemset( sTarget, 0x00, STL_SIZEOF( SQL_INTERVAL_STRUCT ) );

    STL_TRY( dtlInitDataValue( DTL_TYPE_INTERVAL_DAY_TO_SECOND,
                               STL_SIZEOF(dtlIntervalDayToSecondType),
                               &sInterval,
                               aErrorStack ) == STL_SUCCESS );

    sIntervalValue.mIndicator = aArdRec->mDatetimeIntervalCode;
    sInterval.mValue = &sIntervalValue;

    STL_TRY( dtlIntervalDayToSecondSetNotTruncatedValueFromString(
                 (stlChar*)aDataValue->mValue,
                 aDataValue->mLength,
                 aArdRec->mDatetimeIntervalPrecision,
                 aArdRec->mPrecision,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_SECOND,
                 STL_SIZEOF( dtlIntervalDayToSecondType ),
                 &sInterval,
                 &sSuccessWithInfo,
                 ZLS_STMT_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    STL_TRY( dtlGetTimeInfoFromDaySecondInterval( &sIntervalValue,
                                                  &sDay,
                                                  &sHour,
                                                  &sMinute,
                                                  &sSecond,
                                                  &sFraction,
                                                  &sIsNegative,
                                                  aErrorStack ) == STL_SUCCESS );
    
    sTarget->interval_type = aArdRec->mDatetimeIntervalCode;
    sTarget->interval_sign = ( sIsNegative == STL_TRUE ) ? SQL_TRUE : SQL_FALSE;

    sTarget->intval.day_second.second   = ( ( ( ( ( sDay * DTL_HOURS_PER_DAY ) + sHour ) * DTL_MINS_PER_HOUR ) + sMinute ) * DTL_SECS_PER_MINUTE ) + sSecond;
    sTarget->intval.day_second.fraction = sFraction * 1000;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_SIZEOF( *sTarget );
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
 * SQL_CHAR -> SQL_C_INTERVAL_DAY_TO_HOUR
 */
ZLV_DECLARE_SQL_TO_C( Char, IntervalDayToHour )
{
    /*
     * ==========================================================================================================================
     * Test                                                                | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ==========================================================================================================================
     * Data value is a valid interval value; no truncation                 | Data            | Length of data in bytes | n/a
     * Data value is a valid interval value; truncation of one or          | Truncated       | Length of data in bytes | 01S07
     * more trailing fields                                                |                 |                         |
     * Data is valid interval; leading field significant precision is lost | Undefined       | Undefined               | 22015
     * The data value is not a valid interval value                        | Undefined       | Undefined               | 22018
     * ==========================================================================================================================
     */
    
    dtlDataValue                 sInterval;
    dtlIntervalDayToSecondType   sIntervalValue;
    SQL_INTERVAL_STRUCT        * sTarget;
    stlUInt32                    sDay;
    stlUInt32                    sHour;
    stlUInt32                    sMinute;
    stlUInt32                    sSecond;
    stlUInt32                    sFraction;
    stlBool                      sIsNegative;
    stlStatus                    sRet = STL_FAILURE;
    stlBool                      sSuccessWithInfo = STL_FALSE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget = (SQL_INTERVAL_STRUCT*)aTargetValuePtr;
    stlMemset( sTarget, 0x00, STL_SIZEOF( SQL_INTERVAL_STRUCT ) );

    STL_TRY( dtlInitDataValue( DTL_TYPE_INTERVAL_DAY_TO_SECOND,
                               STL_SIZEOF(dtlIntervalDayToSecondType),
                               &sInterval,
                               aErrorStack ) == STL_SUCCESS );

    sIntervalValue.mIndicator = aArdRec->mDatetimeIntervalCode;
    sInterval.mValue = &sIntervalValue;

    STL_TRY( dtlIntervalDayToSecondSetNotTruncatedValueFromString(
                 (stlChar*)aDataValue->mValue,
                 aDataValue->mLength,
                 aArdRec->mDatetimeIntervalPrecision,
                 aArdRec->mPrecision,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_DAY_TO_HOUR,
                 STL_SIZEOF( dtlIntervalDayToSecondType ),
                 &sInterval,
                 &sSuccessWithInfo,
                 ZLS_STMT_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    STL_TRY( dtlGetTimeInfoFromDaySecondInterval( &sIntervalValue,
                                                  &sDay,
                                                  &sHour,
                                                  &sMinute,
                                                  &sSecond,
                                                  &sFraction,
                                                  &sIsNegative,
                                                  aErrorStack ) == STL_SUCCESS );
    
    sTarget->interval_type = aArdRec->mDatetimeIntervalCode;
    sTarget->interval_sign = ( sIsNegative == STL_TRUE ) ? SQL_TRUE : SQL_FALSE;

    sTarget->intval.day_second.day  = sDay;
    sTarget->intval.day_second.hour = sHour;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_SIZEOF( *sTarget );
    }

    STL_TRY_THROW( ( sMinute == 0 ) &&
                   ( sSecond == 0 ) &&
                   ( sFraction == 0 ),
                   RAMP_WARNING_FRACTIONAL_TRUNCATION );

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
 * SQL_CHAR -> SQL_C_INTERVAL_DAY_TO_MINUTE
 */
ZLV_DECLARE_SQL_TO_C( Char, IntervalDayToMinute )
{
    /*
     * ==========================================================================================================================
     * Test                                                                | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ==========================================================================================================================
     * Data value is a valid interval value; no truncation                 | Data            | Length of data in bytes | n/a
     * Data value is a valid interval value; truncation of one or          | Truncated       | Length of data in bytes | 01S07
     * more trailing fields                                                |                 |                         |
     * Data is valid interval; leading field significant precision is lost | Undefined       | Undefined               | 22015
     * The data value is not a valid interval value                        | Undefined       | Undefined               | 22018
     * ==========================================================================================================================
     */
    
    dtlDataValue                 sInterval;
    dtlIntervalDayToSecondType   sIntervalValue;
    SQL_INTERVAL_STRUCT        * sTarget;
    stlUInt32                    sDay;
    stlUInt32                    sHour;
    stlUInt32                    sMinute;
    stlUInt32                    sSecond;
    stlUInt32                    sFraction;
    stlBool                      sIsNegative;
    stlStatus                    sRet = STL_FAILURE;
    stlBool                      sSuccessWithInfo = STL_FALSE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget = (SQL_INTERVAL_STRUCT*)aTargetValuePtr;
    stlMemset( sTarget, 0x00, STL_SIZEOF( SQL_INTERVAL_STRUCT ) );

    STL_TRY( dtlInitDataValue( DTL_TYPE_INTERVAL_DAY_TO_SECOND,
                               STL_SIZEOF(dtlIntervalDayToSecondType),
                               &sInterval,
                               aErrorStack ) == STL_SUCCESS );

    sIntervalValue.mIndicator = aArdRec->mDatetimeIntervalCode;
    sInterval.mValue = &sIntervalValue;

    STL_TRY( dtlIntervalDayToSecondSetNotTruncatedValueFromString(
                 (stlChar*)aDataValue->mValue,
                 aDataValue->mLength,
                 aArdRec->mDatetimeIntervalPrecision,
                 aArdRec->mPrecision,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE,
                 STL_SIZEOF( dtlIntervalDayToSecondType ),
                 &sInterval,
                 &sSuccessWithInfo,
                 ZLS_STMT_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    STL_TRY( dtlGetTimeInfoFromDaySecondInterval( &sIntervalValue,
                                                  &sDay,
                                                  &sHour,
                                                  &sMinute,
                                                  &sSecond,
                                                  &sFraction,
                                                  &sIsNegative,
                                                  aErrorStack ) == STL_SUCCESS );
    
    sTarget->interval_type = aArdRec->mDatetimeIntervalCode;
    sTarget->interval_sign = ( sIsNegative == STL_TRUE ) ? SQL_TRUE : SQL_FALSE;

    sTarget->intval.day_second.day    = sDay;
    sTarget->intval.day_second.hour   = sHour;
    sTarget->intval.day_second.minute = sMinute;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_SIZEOF( *sTarget );
    }

    STL_TRY_THROW( ( sSecond == 0 ) &&
                   ( sFraction == 0 ),
                   RAMP_WARNING_FRACTIONAL_TRUNCATION );

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
 * SQL_CHAR -> SQL_C_INTERVAL_DAY_TO_SECOND
 */
ZLV_DECLARE_SQL_TO_C( Char, IntervalDayToSecond )
{
    /*
     * ==========================================================================================================================
     * Test                                                                | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ==========================================================================================================================
     * Data value is a valid interval value; no truncation                 | Data            | Length of data in bytes | n/a
     * Data value is a valid interval value; truncation of one or          | Truncated       | Length of data in bytes | 01S07
     * more trailing fields                                                |                 |                         |
     * Data is valid interval; leading field significant precision is lost | Undefined       | Undefined               | 22015
     * The data value is not a valid interval value                        | Undefined       | Undefined               | 22018
     * ==========================================================================================================================
     */
    
    dtlDataValue                 sInterval;
    dtlIntervalDayToSecondType   sIntervalValue;
    SQL_INTERVAL_STRUCT        * sTarget;
    stlUInt32                    sDay;
    stlUInt32                    sHour;
    stlUInt32                    sMinute;
    stlUInt32                    sSecond;
    stlUInt32                    sFraction;
    stlBool                      sIsNegative;
    stlStatus                    sRet = STL_FAILURE;
    stlBool                      sSuccessWithInfo = STL_FALSE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget = (SQL_INTERVAL_STRUCT*)aTargetValuePtr;
    stlMemset( sTarget, 0x00, STL_SIZEOF( SQL_INTERVAL_STRUCT ) );

    STL_TRY( dtlInitDataValue( DTL_TYPE_INTERVAL_DAY_TO_SECOND,
                               STL_SIZEOF(dtlIntervalDayToSecondType),
                               &sInterval,
                               aErrorStack ) == STL_SUCCESS );

    sIntervalValue.mIndicator = aArdRec->mDatetimeIntervalCode;
    sInterval.mValue = &sIntervalValue;

    STL_TRY( dtlIntervalDayToSecondSetNotTruncatedValueFromString(
                 (stlChar*)aDataValue->mValue,
                 aDataValue->mLength,
                 aArdRec->mDatetimeIntervalPrecision,
                 aArdRec->mPrecision,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_DAY_TO_SECOND,
                 STL_SIZEOF( dtlIntervalDayToSecondType ),
                 &sInterval,
                 &sSuccessWithInfo,
                 ZLS_STMT_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    STL_TRY( dtlGetTimeInfoFromDaySecondInterval( &sIntervalValue,
                                                  &sDay,
                                                  &sHour,
                                                  &sMinute,
                                                  &sSecond,
                                                  &sFraction,
                                                  &sIsNegative,
                                                  aErrorStack ) == STL_SUCCESS );
    
    sTarget->interval_type = aArdRec->mDatetimeIntervalCode;
    sTarget->interval_sign = ( sIsNegative == STL_TRUE ) ? SQL_TRUE : SQL_FALSE;

    sTarget->intval.day_second.day      = sDay;
    sTarget->intval.day_second.hour     = sHour;
    sTarget->intval.day_second.minute   = sMinute;
    sTarget->intval.day_second.second   = sSecond;
    sTarget->intval.day_second.fraction = sFraction * 1000;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_SIZEOF( *sTarget );
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
 * SQL_CHAR -> SQL_C_INTERVAL_HOUR_TO_MINUTE
 */
ZLV_DECLARE_SQL_TO_C( Char, IntervalHourToMinute )
{
    /*
     * ==========================================================================================================================
     * Test                                                                | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ==========================================================================================================================
     * Data value is a valid interval value; no truncation                 | Data            | Length of data in bytes | n/a
     * Data value is a valid interval value; truncation of one or          | Truncated       | Length of data in bytes | 01S07
     * more trailing fields                                                |                 |                         |
     * Data is valid interval; leading field significant precision is lost | Undefined       | Undefined               | 22015
     * The data value is not a valid interval value                        | Undefined       | Undefined               | 22018
     * ==========================================================================================================================
     */
    
    dtlDataValue                 sInterval;
    dtlIntervalDayToSecondType   sIntervalValue;
    SQL_INTERVAL_STRUCT        * sTarget;
    stlUInt32                    sDay;
    stlUInt32                    sHour;
    stlUInt32                    sMinute;
    stlUInt32                    sSecond;
    stlUInt32                    sFraction;
    stlBool                      sIsNegative;
    stlStatus                    sRet = STL_FAILURE;
    stlBool                      sSuccessWithInfo = STL_FALSE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget = (SQL_INTERVAL_STRUCT*)aTargetValuePtr;
    stlMemset( sTarget, 0x00, STL_SIZEOF( SQL_INTERVAL_STRUCT ) );

    STL_TRY( dtlInitDataValue( DTL_TYPE_INTERVAL_DAY_TO_SECOND,
                               STL_SIZEOF(dtlIntervalDayToSecondType),
                               &sInterval,
                               aErrorStack ) == STL_SUCCESS );

    sIntervalValue.mIndicator = aArdRec->mDatetimeIntervalCode;
    sInterval.mValue = &sIntervalValue;

    STL_TRY( dtlIntervalDayToSecondSetNotTruncatedValueFromString(
                 (stlChar*)aDataValue->mValue,
                 aDataValue->mLength,
                 aArdRec->mDatetimeIntervalPrecision,
                 aArdRec->mPrecision,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE,
                 STL_SIZEOF( dtlIntervalDayToSecondType ),
                 &sInterval,
                 &sSuccessWithInfo,
                 ZLS_STMT_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    STL_TRY( dtlGetTimeInfoFromDaySecondInterval( &sIntervalValue,
                                                  &sDay,
                                                  &sHour,
                                                  &sMinute,
                                                  &sSecond,
                                                  &sFraction,
                                                  &sIsNegative,
                                                  aErrorStack ) == STL_SUCCESS );
    
    sTarget->interval_type = aArdRec->mDatetimeIntervalCode;
    sTarget->interval_sign = ( sIsNegative == STL_TRUE ) ? SQL_TRUE : SQL_FALSE;

    sTarget->intval.day_second.hour   = ( sDay * DTL_HOURS_PER_DAY ) + sHour;
    sTarget->intval.day_second.minute = sMinute;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_SIZEOF( *sTarget );
    }

    STL_TRY_THROW( ( sSecond == 0 ) &&
                   ( sFraction == 0 ),
                   RAMP_WARNING_FRACTIONAL_TRUNCATION );

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
 * SQL_CHAR -> SQL_C_INTERVAL_HOUR_TO_SECOND
 */
ZLV_DECLARE_SQL_TO_C( Char, IntervalHourToSecond )
{
    /*
     * ==========================================================================================================================
     * Test                                                                | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ==========================================================================================================================
     * Data value is a valid interval value; no truncation                 | Data            | Length of data in bytes | n/a
     * Data value is a valid interval value; truncation of one or          | Truncated       | Length of data in bytes | 01S07
     * more trailing fields                                                |                 |                         |
     * Data is valid interval; leading field significant precision is lost | Undefined       | Undefined               | 22015
     * The data value is not a valid interval value                        | Undefined       | Undefined               | 22018
     * ==========================================================================================================================
     */
    
    dtlDataValue                 sInterval;
    dtlIntervalDayToSecondType   sIntervalValue;
    SQL_INTERVAL_STRUCT        * sTarget;
    stlUInt32                    sDay;
    stlUInt32                    sHour;
    stlUInt32                    sMinute;
    stlUInt32                    sSecond;
    stlUInt32                    sFraction;
    stlBool                      sIsNegative;
    stlStatus                    sRet = STL_FAILURE;
    stlBool                      sSuccessWithInfo = STL_FALSE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget = (SQL_INTERVAL_STRUCT*)aTargetValuePtr;
    stlMemset( sTarget, 0x00, STL_SIZEOF( SQL_INTERVAL_STRUCT ) );

    STL_TRY( dtlInitDataValue( DTL_TYPE_INTERVAL_DAY_TO_SECOND,
                               STL_SIZEOF(dtlIntervalDayToSecondType),
                               &sInterval,
                               aErrorStack ) == STL_SUCCESS );

    sIntervalValue.mIndicator = aArdRec->mDatetimeIntervalCode;
    sInterval.mValue = &sIntervalValue;

    STL_TRY( dtlIntervalDayToSecondSetNotTruncatedValueFromString(
                 (stlChar*)aDataValue->mValue,
                 aDataValue->mLength,
                 aArdRec->mDatetimeIntervalPrecision,
                 aArdRec->mPrecision,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND,
                 STL_SIZEOF( dtlIntervalDayToSecondType ),
                 &sInterval,
                 &sSuccessWithInfo,
                 ZLS_STMT_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    STL_TRY( dtlGetTimeInfoFromDaySecondInterval( &sIntervalValue,
                                                  &sDay,
                                                  &sHour,
                                                  &sMinute,
                                                  &sSecond,
                                                  &sFraction,
                                                  &sIsNegative,
                                                  aErrorStack ) == STL_SUCCESS );
    
    sTarget->interval_type = aArdRec->mDatetimeIntervalCode;
    sTarget->interval_sign = ( sIsNegative == STL_TRUE ) ? SQL_TRUE : SQL_FALSE;

    sTarget->intval.day_second.hour     = ( sDay * DTL_HOURS_PER_DAY ) + sHour;
    sTarget->intval.day_second.minute   = sMinute;
    sTarget->intval.day_second.second   = sSecond;
    sTarget->intval.day_second.fraction = sFraction * 1000;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_SIZEOF( *sTarget );
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
 * SQL_CHAR -> SQL_C_INTERVAL_MINUTE_TO_SECOND
 */
ZLV_DECLARE_SQL_TO_C( Char, IntervalMinuteToSecond )
{
    /*
     * ==========================================================================================================================
     * Test                                                                | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ==========================================================================================================================
     * Data value is a valid interval value; no truncation                 | Data            | Length of data in bytes | n/a
     * Data value is a valid interval value; truncation of one or          | Truncated       | Length of data in bytes | 01S07
     * more trailing fields                                                |                 |                         |
     * Data is valid interval; leading field significant precision is lost | Undefined       | Undefined               | 22015
     * The data value is not a valid interval value                        | Undefined       | Undefined               | 22018
     * ==========================================================================================================================
     */
    
    dtlDataValue                 sInterval;
    dtlIntervalDayToSecondType   sIntervalValue;
    SQL_INTERVAL_STRUCT        * sTarget;
    stlUInt32                    sDay;
    stlUInt32                    sHour;
    stlUInt32                    sMinute;
    stlUInt32                    sSecond;
    stlUInt32                    sFraction;
    stlBool                      sIsNegative;
    stlStatus                    sRet = STL_FAILURE;
    stlBool                      sSuccessWithInfo = STL_FALSE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget = (SQL_INTERVAL_STRUCT*)aTargetValuePtr;
    stlMemset( sTarget, 0x00, STL_SIZEOF( SQL_INTERVAL_STRUCT ) );

    STL_TRY( dtlInitDataValue( DTL_TYPE_INTERVAL_DAY_TO_SECOND,
                               STL_SIZEOF(dtlIntervalDayToSecondType),
                               &sInterval,
                               aErrorStack ) == STL_SUCCESS );

    sIntervalValue.mIndicator = aArdRec->mDatetimeIntervalCode;
    sInterval.mValue = &sIntervalValue;

    STL_TRY( dtlIntervalDayToSecondSetNotTruncatedValueFromString(
                 (stlChar*)aDataValue->mValue,
                 aDataValue->mLength,
                 aArdRec->mDatetimeIntervalPrecision,
                 aArdRec->mPrecision,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND,
                 STL_SIZEOF( dtlIntervalDayToSecondType ),
                 &sInterval,
                 &sSuccessWithInfo,
                 ZLS_STMT_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    STL_TRY( dtlGetTimeInfoFromDaySecondInterval( &sIntervalValue,
                                                  &sDay,
                                                  &sHour,
                                                  &sMinute,
                                                  &sSecond,
                                                  &sFraction,
                                                  &sIsNegative,
                                                  aErrorStack ) == STL_SUCCESS );
    
    sTarget->interval_type = aArdRec->mDatetimeIntervalCode;
    sTarget->interval_sign = ( sIsNegative == STL_TRUE ) ? SQL_TRUE : SQL_FALSE;

    sTarget->intval.day_second.minute   = ( ( ( sDay * DTL_HOURS_PER_DAY ) + sHour ) * DTL_MINS_PER_HOUR ) + sMinute;
    sTarget->intval.day_second.second   = sSecond;
    sTarget->intval.day_second.fraction = sFraction * 1000;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_SIZEOF( *sTarget );
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

/** @} */

