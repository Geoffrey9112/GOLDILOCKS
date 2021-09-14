/*******************************************************************************
 * zlvVarCharToC.c
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
 * @file zlvVarCharToC.c
 * @brief Gliese API Internal Converting Data from SQL Variable Character to C Data Types Routines.
 */

/**
 * @addtogroup zlvVarCharToC
 * @{
 */

/*
 * http://msdn.microsoft.com/en-us/library/windows/desktop/ms713346%28v=VS.85%29.aspx
 */

/*
 * SQL_VARCHAR -> SQL_C_CHAR
 */
ZLV_DECLARE_SQL_TO_C( VarChar, Char )
{
    /*
     * ==========================================================================================
     * Test                                | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ==========================================================================================
     * Byte length of data < BufferLength  | Data            | Length of data in bytes | n/a
     * Byte length of data >= BufferLength | Truncated data  | Length of data in bytes | 01004
     * ==========================================================================================
     */

    return zlvSqlCharToCChar( aStmt,
                              aValueBlockList,
                              aDataValue,
                              aOffset,
                              aTargetValuePtr,
                              aIndicator,
                              aArdRec,
                              aReturn,
                              aErrorStack );
}

/*
 * SQL_VARCHAR -> SQL_C_WCHAR
 */
ZLV_DECLARE_SQL_TO_C( VarChar, Wchar )
{
    /*
     * ==========================================================================================
     * Test                                | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ==========================================================================================
     * Byte length of data < BufferLength  | Data            | Length of data in bytes | n/a
     * Byte length of data >= BufferLength | Truncated data  | Length of data in bytes | 01004
     * ==========================================================================================
     */

    return zlvSqlCharToCWchar( aStmt,
                               aValueBlockList,
                               aDataValue,
                               aOffset,
                               aTargetValuePtr,
                               aIndicator,
                               aArdRec,
                               aReturn,
                               aErrorStack );
}

/*
 * SQL_VARCHAR -> SQL_C_LONGVARCHAR
 */
ZLV_DECLARE_SQL_TO_C( VarChar, LongVarChar )
{
    /*
     * ==========================================================================================
     * Test                                | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ==========================================================================================
     * Byte length of data < BufferLength  | Data            | Length of data in bytes | n/a
     * Byte length of data >= BufferLength | Truncated data  | Length of data in bytes | 01004
     * ==========================================================================================
     */

    return zlvSqlCharToCLongVarChar( aStmt,
                                     aValueBlockList,
                                     aDataValue,
                                     aOffset,
                                     aTargetValuePtr,
                                     aIndicator,
                                     aArdRec,
                                     aReturn,
                                     aErrorStack );
}

/*
 * SQL_VARCHAR -> SQL_C_STINYINT
 */
ZLV_DECLARE_SQL_TO_C( VarChar, STinyInt )
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
    
    return zlvSqlCharToCSTinyInt( aStmt,
                                  aValueBlockList,
                                  aDataValue,
                                  aOffset,
                                  aTargetValuePtr,
                                  aIndicator,
                                  aArdRec,
                                  aReturn,
                                  aErrorStack );
}

/*
 * SQL_VARCHAR -> SQL_C_UTINYINT
 */
ZLV_DECLARE_SQL_TO_C( VarChar, UTinyInt )
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
    
    return zlvSqlCharToCUTinyInt( aStmt,
                                  aValueBlockList,
                                  aDataValue,
                                  aOffset,
                                  aTargetValuePtr,
                                  aIndicator,
                                  aArdRec,
                                  aReturn,
                                  aErrorStack );
}

/*
 * SQL_VARCHAR -> SQL_C_SBIGINT
 */
ZLV_DECLARE_SQL_TO_C( VarChar, SBigInt )
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
    
    return zlvSqlCharToCSBigInt( aStmt,
                                 aValueBlockList,
                                 aDataValue,
                                 aOffset,
                                 aTargetValuePtr,
                                 aIndicator,
                                 aArdRec,
                                 aReturn,
                                 aErrorStack );
}

/*
 * SQL_VARCHAR -> SQL_C_UBIGINT
 */
ZLV_DECLARE_SQL_TO_C( VarChar, UBigInt )
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
    
    return zlvSqlCharToCUBigInt( aStmt,
                                 aValueBlockList,
                                 aDataValue,
                                 aOffset,
                                 aTargetValuePtr,
                                 aIndicator,
                                 aArdRec,
                                 aReturn,
                                 aErrorStack );
}

/*
 * SQL_VARCHAR -> SQL_C_SSHORT
 */
ZLV_DECLARE_SQL_TO_C( VarChar, SShort )
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
    
    return zlvSqlCharToCSShort( aStmt,
                                aValueBlockList,
                                aDataValue,
                                aOffset,
                                aTargetValuePtr,
                                aIndicator,
                                aArdRec,
                                aReturn,
                                aErrorStack );
}

/*
 * SQL_VARCHAR -> SQL_C_USHORT
 */
ZLV_DECLARE_SQL_TO_C( VarChar, UShort )
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
    
    return zlvSqlCharToCUShort( aStmt,
                                aValueBlockList,
                                aDataValue,
                                aOffset,
                                aTargetValuePtr,
                                aIndicator,
                                aArdRec,
                                aReturn,
                                aErrorStack );
}

/*
 * SQL_VARCHAR -> SQL_C_SLONG
 */
ZLV_DECLARE_SQL_TO_C( VarChar, SLong )
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
    
    return zlvSqlCharToCSLong( aStmt,
                               aValueBlockList,
                               aDataValue,
                               aOffset,
                               aTargetValuePtr,
                               aIndicator,
                               aArdRec,
                               aReturn,
                               aErrorStack );
}

/*
 * SQL_VARCHAR -> SQL_C_ULONG
 */
ZLV_DECLARE_SQL_TO_C( VarChar, ULong )
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
    
    return zlvSqlCharToCULong( aStmt,
                               aValueBlockList,
                               aDataValue,
                               aOffset,
                               aTargetValuePtr,
                               aIndicator,
                               aArdRec,
                               aReturn,
                               aErrorStack );
}

/*
 * SQL_VARCHAR -> SQL_C_NUMERIC
 */
ZLV_DECLARE_SQL_TO_C( VarChar, Numeric )
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
    
    return zlvSqlCharToCNumeric( aStmt,
                                 aValueBlockList,
                                 aDataValue,
                                 aOffset,
                                 aTargetValuePtr,
                                 aIndicator,
                                 aArdRec,
                                 aReturn,
                                 aErrorStack );
}

/*
 * SQL_VARCHAR -> SQL_C_FLOAT
 */
ZLV_DECLARE_SQL_TO_C( VarChar, Float )
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
    
    return zlvSqlCharToCFloat( aStmt,
                               aValueBlockList,
                               aDataValue,
                               aOffset,
                               aTargetValuePtr,
                               aIndicator,
                               aArdRec,
                               aReturn,
                               aErrorStack );
}

/*
 * SQL_VARCHAR -> SQL_C_DOUBLE
 */
ZLV_DECLARE_SQL_TO_C( VarChar, Double )
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
    
    return zlvSqlCharToCDouble( aStmt,
                                aValueBlockList,
                                aDataValue,
                                aOffset,
                                aTargetValuePtr,
                                aIndicator,
                                aArdRec,
                                aReturn,
                                aErrorStack );
}

/*
 * SQL_VARCHAR -> SQL_C_BIT
 */
ZLV_DECLARE_SQL_TO_C( VarChar, Bit )
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
    
    return zlvSqlCharToCBit( aStmt,
                             aValueBlockList,
                             aDataValue,
                             aOffset,
                             aTargetValuePtr,
                             aIndicator,
                             aArdRec,
                             aReturn,
                             aErrorStack );
}

/*
 * SQL_VARCHAR -> SQL_C_BOOLEAN
 */
ZLV_DECLARE_SQL_TO_C( VarChar, Boolean )
{
    return zlvSqlCharToCBoolean( aStmt,
                                 aValueBlockList,
                                 aDataValue,
                                 aOffset,
                                 aTargetValuePtr,
                                 aIndicator,
                                 aArdRec,
                                 aReturn,
                                 aErrorStack );
}

/*
 * SQL_VARCHAR -> SQL_C_BINARY
 */
ZLV_DECLARE_SQL_TO_C( VarChar, Binary )
{
    /*
     * ==========================================================================================
     * Test                                | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ==========================================================================================
     * Byte length of data <= BufferLength | Data            | Length of data in bytes | n/a
     * Byte length of data > BufferLength  | Truncated data  | Length of data          | 01004
     * ==========================================================================================
     */
    
    return zlvSqlCharToCBinary( aStmt,
                                aValueBlockList,
                                aDataValue,
                                aOffset,
                                aTargetValuePtr,
                                aIndicator,
                                aArdRec,
                                aReturn,
                                aErrorStack );
}

/*
 * SQL_VARCHAR -> SQL_C_LONGVARBINARY
 */
ZLV_DECLARE_SQL_TO_C( VarChar, LongVarBinary )
{
    /*
     * ==========================================================================================
     * Test                                | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ==========================================================================================
     * Byte length of data <= BufferLength | Data            | Length of data in bytes | n/a
     * Byte length of data > BufferLength  | Truncated data  | Length of data          | 01004
     * ==========================================================================================
     */
    
    return zlvSqlCharToCLongVarBinary( aStmt,
                                       aValueBlockList,
                                       aDataValue,
                                       aOffset,
                                       aTargetValuePtr,
                                       aIndicator,
                                       aArdRec,
                                       aReturn,
                                       aErrorStack );
}

/*
 * SQL_VARCHAR -> SQL_C_TYPE_DATE
 */
ZLV_DECLARE_SQL_TO_C( VarChar, Date )
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
    
    return zlvSqlCharToCDate( aStmt,
                              aValueBlockList,
                              aDataValue,
                              aOffset,
                              aTargetValuePtr,
                              aIndicator,
                              aArdRec,
                              aReturn,
                              aErrorStack );
}

/*
 * SQL_VARCHAR -> SQL_C_TYPE_TIME
 */
ZLV_DECLARE_SQL_TO_C( VarChar, Time )
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
    
    return zlvSqlCharToCTime( aStmt,
                              aValueBlockList,
                              aDataValue,
                              aOffset,
                              aTargetValuePtr,
                              aIndicator,
                              aArdRec,
                              aReturn,
                              aErrorStack );
}

/*
 * SQL_VARCHAR -> SQL_C_TYPE_TIME_TZ
 */
ZLV_DECLARE_SQL_TO_C( VarChar, TimeTz )
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
    
    return zlvSqlCharToCTimeTz( aStmt,
                                aValueBlockList,
                                aDataValue,
                                aOffset,
                                aTargetValuePtr,
                                aIndicator,
                                aArdRec,
                                aReturn,
                                aErrorStack );
}

/*
 * SQL_VARCHAR -> SQL_C_TYPE_TIMESTAMP
 */
ZLV_DECLARE_SQL_TO_C( VarChar, Timestamp )
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
    
    return zlvSqlCharToCTimestamp( aStmt,
                                   aValueBlockList,
                                   aDataValue,
                                   aOffset,
                                   aTargetValuePtr,
                                   aIndicator,
                                   aArdRec,
                                   aReturn,
                                   aErrorStack );
}

/*
 * SQL_VARCHAR -> SQL_C_TYPE_TIMESTAMP_TZ
 */
ZLV_DECLARE_SQL_TO_C( VarChar, TimestampTz )
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
    
    return zlvSqlCharToCTimestampTz( aStmt,
                                     aValueBlockList,
                                     aDataValue,
                                     aOffset,
                                     aTargetValuePtr,
                                     aIndicator,
                                     aArdRec,
                                     aReturn,
                                     aErrorStack );
}

/*
 * SQL_VARCHAR -> SQL_C_INTERVAL_MONTH
 */
ZLV_DECLARE_SQL_TO_C( VarChar, IntervalMonth )
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
    
    return zlvSqlCharToCIntervalMonth( aStmt,
                                       aValueBlockList,
                                       aDataValue,
                                       aOffset,
                                       aTargetValuePtr,
                                       aIndicator,
                                       aArdRec,
                                       aReturn,
                                       aErrorStack );
}

/*
 * SQL_VARCHAR -> SQL_C_INTERVAL_YEAR
 */
ZLV_DECLARE_SQL_TO_C( VarChar, IntervalYear )
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
    
    return zlvSqlCharToCIntervalYear( aStmt,
                                      aValueBlockList,
                                      aDataValue,
                                      aOffset,
                                      aTargetValuePtr,
                                      aIndicator,
                                      aArdRec,
                                      aReturn,
                                      aErrorStack );
}

/*
 * SQL_VARCHAR -> SQL_C_INTERVAL_YEAR_TO_MONTH
 */
ZLV_DECLARE_SQL_TO_C( VarChar, IntervalYearToMonth )
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
    
    return zlvSqlCharToCIntervalYearToMonth( aStmt,
                                             aValueBlockList,
                                             aDataValue,
                                             aOffset,
                                             aTargetValuePtr,
                                             aIndicator,
                                             aArdRec,
                                             aReturn,
                                             aErrorStack );
}

/*
 * SQL_VARCHAR -> SQL_C_INTERVAL_DAY
 */
ZLV_DECLARE_SQL_TO_C( VarChar, IntervalDay )
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
    
    return zlvSqlCharToCIntervalDay( aStmt,
                                     aValueBlockList,
                                     aDataValue,
                                     aOffset,
                                     aTargetValuePtr,
                                     aIndicator,
                                     aArdRec,
                                     aReturn,
                                     aErrorStack );
}

/*
 * SQL_VARCHAR -> SQL_C_INTERVAL_HOUR
 */
ZLV_DECLARE_SQL_TO_C( VarChar, IntervalHour )
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
    
    return zlvSqlCharToCIntervalHour( aStmt,
                                      aValueBlockList,
                                      aDataValue,
                                      aOffset,
                                      aTargetValuePtr,
                                      aIndicator,
                                      aArdRec,
                                      aReturn,
                                      aErrorStack );
}

/*
 * SQL_VARCHAR -> SQL_C_INTERVAL_MINUTE
 */
ZLV_DECLARE_SQL_TO_C( VarChar, IntervalMinute )
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
    
    return zlvSqlCharToCIntervalMinute( aStmt,
                                        aValueBlockList,
                                        aDataValue,
                                        aOffset,
                                        aTargetValuePtr,
                                        aIndicator,
                                        aArdRec,
                                        aReturn,
                                        aErrorStack );
}

/*
 * SQL_VARCHAR -> SQL_C_INTERVAL_SECOND
 */
ZLV_DECLARE_SQL_TO_C( VarChar, IntervalSecond )
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
    
    return zlvSqlCharToCIntervalSecond( aStmt,
                                        aValueBlockList,
                                        aDataValue,
                                        aOffset,
                                        aTargetValuePtr,
                                        aIndicator,
                                        aArdRec,
                                        aReturn,
                                        aErrorStack );
}

/*
 * SQL_VARCHAR -> SQL_C_INTERVAL_DAY_TO_HOUR
 */
ZLV_DECLARE_SQL_TO_C( VarChar, IntervalDayToHour )
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
    
    return zlvSqlCharToCIntervalDayToHour( aStmt,
                                           aValueBlockList,
                                           aDataValue,
                                           aOffset,
                                           aTargetValuePtr,
                                           aIndicator,
                                           aArdRec,
                                           aReturn,
                                           aErrorStack );
}

/*
 * SQL_VARCHAR -> SQL_C_INTERVAL_DAY_TO_MINUTE
 */
ZLV_DECLARE_SQL_TO_C( VarChar, IntervalDayToMinute )
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
    
    return zlvSqlCharToCIntervalDayToMinute( aStmt,
                                             aValueBlockList,
                                             aDataValue,
                                             aOffset,
                                             aTargetValuePtr,
                                             aIndicator,
                                             aArdRec,
                                             aReturn,
                                             aErrorStack );
}

/*
 * SQL_VARCHAR -> SQL_C_INTERVAL_DAY_TO_SECOND
 */
ZLV_DECLARE_SQL_TO_C( VarChar, IntervalDayToSecond )
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
    
    return zlvSqlCharToCIntervalDayToSecond( aStmt,
                                             aValueBlockList,
                                             aDataValue,
                                             aOffset,
                                             aTargetValuePtr,
                                             aIndicator,
                                             aArdRec,
                                             aReturn,
                                             aErrorStack );
}

/*
 * SQL_VARCHAR -> SQL_C_INTERVAL_HOUR_TO_MINUTE
 */
ZLV_DECLARE_SQL_TO_C( VarChar, IntervalHourToMinute )
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
    
    return zlvSqlCharToCIntervalHourToMinute( aStmt,
                                              aValueBlockList,
                                              aDataValue,
                                              aOffset,
                                              aTargetValuePtr,
                                              aIndicator,
                                              aArdRec,
                                              aReturn,
                                              aErrorStack );
}

/*
 * SQL_VARCHAR -> SQL_C_INTERVAL_HOUR_TO_SECOND
 */
ZLV_DECLARE_SQL_TO_C( VarChar, IntervalHourToSecond )
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
    
    return zlvSqlCharToCIntervalHourToSecond( aStmt,
                                              aValueBlockList,
                                              aDataValue,
                                              aOffset,
                                              aTargetValuePtr,
                                              aIndicator,
                                              aArdRec,
                                              aReturn,
                                              aErrorStack );
}

/*
 * SQL_VARCHAR -> SQL_C_INTERVAL_MINUTE_TO_SECOND
 */
ZLV_DECLARE_SQL_TO_C( VarChar, IntervalMinuteToSecond )
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
    
    return zlvSqlCharToCIntervalMinuteToSecond( aStmt,
                                                aValueBlockList,
                                                aDataValue,
                                                aOffset,
                                                aTargetValuePtr,
                                                aIndicator,
                                                aArdRec,
                                                aReturn,
                                                aErrorStack );
}

/** @} */

