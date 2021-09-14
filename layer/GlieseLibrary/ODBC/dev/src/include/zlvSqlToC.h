/*******************************************************************************
 * zlvSqlToC.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zlvSqlToC.h 12898 2014-06-18 04:00:29Z chlee $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZLV_SQL_TO_C_H_
#define _ZLV_SQL_TO_C_H_ 1

/**
 * @file zlvSqlToC.h
 * @brief Gliese API Internal Convert Routines.
 */

/**
 * @defgroup zlv Internal Convert Routines
 * @{
 */

/**
 * SQLType <-> C_NUMERIC Precision
 */
#define ZLV_CHAR_TO_C_NUMERIC_PRECISION            (DTL_NUMERIC_DEFAULT_PRECISION)
#define ZLV_VARCHAR_TO_C_NUMERIC_PRECISION         ZLV_CHAR_TO_C_NUMERIC_PRECISION
#define ZLV_LONGVARCHAR_TO_C_NUMERIC_PRECISION     ZLV_CHAR_TO_C_NUMERIC_PRECISION
#define ZLV_NUMERIC_TO_C_NUMERIC_PRECISION         (DTL_NUMERIC_DEFAULT_PRECISION)
#define ZLV_FLOAT_TO_C_NUMERIC_PRECISION           ZLV_NUMERIC_TO_C_NUMERIC_PRECISION
#define ZLV_SMALLINT_TO_C_NUMERIC_PRECISION        (DTL_INT16_MAX_DIGIT_COUNT)
#define ZLV_TINYINY_TO_C_NUMERIC_PRECISION         ZLV_SMALLINT_TO_C_NUMERIC_PRECISION
#define ZLV_INTEGER_TO_C_NUMERIC_PRECISION         (DTL_INT32_MAX_DIGIT_COUNT)
#define ZLV_BIGINT_TO_C_NUMERIC_PRECISION          (DTL_INT64_MAX_DIGIT_COUNT)
#define ZLV_REAL_TO_C_NUMERIC_PRECISION            (STL_FLT_DIG)
#define ZLV_DOUBLE_TO_C_NUMERIC_PRECISION          (STL_DBL_DIG)
#define ZLV_BOOLEAN_TO_C_NUMERIC_PRECISION         (STL_BOOL_SIZE)
#define ZLV_INTERVAL_YEAR_TO_C_NUMERIC_PRECISION   (DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION)
#define ZLV_INTERVAL_MONTH_TO_C_NUMERIC_PRECISION  (DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION)
#define ZLV_INTERVAL_DAY_TO_C_NUMERIC_PRECISION    (DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION)
#define ZLV_INTERVAL_HOUR_TO_C_NUMERIC_PRECISION   (DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION)
#define ZLV_INTERVAL_MINUTE_TO_C_NUMERIC_PRECISION (DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION)
#define ZLV_INTERVAL_SECOND_TO_C_NUMERIC_PRECISION (DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION + DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION)


/**
 * SQLType <-> C_NUMERIC scale
 */
#define ZLV_SQL_TO_C_NUMERIC_MAX_PRECISION (38)
#define ZLV_SQL_TO_C_NUMERIC_MAX_SCALE (127)
#define ZLV_SQL_TO_C_NUMERIC_MIN_SCALE (-128)

#define ZLV_CHAR_TO_C_NUMERIC_MAX_SCALE (127)
#define ZLV_CHAR_TO_C_NUMERIC_MIN_SCALE (-128)
#define ZLV_NUMERIC_TO_C_NUMERIC_MAX_SCALE ( 127)
#define ZLV_NUMERIC_TO_C_NUMERIC_MIN_SCALE (-125)
#define ZLV_DOUBLE_TO_C_NUMERIC_MAX_SCALE (127)
#define ZLV_DOUBLE_TO_C_NUMERIC_MIN_SCALE (-128)



#define ZLV_CONVERT_SECOND_TO_HOUR_MINUTE( aSecond, aHour, aMinute )    \
    {                                                                   \
        stlInt64 sTotalMinute = aSecond / 60;                           \
        aHour = sTotalMinute / 60;                                      \
        aMinute = sTotalMinute % 60;                                    \
    }                                                                   \

#define ZLV_DECLARE_SQL_TO_C( aSQLType, aCType )                                        \
    stlStatus zlvSql##aSQLType##ToC##aCType ( zlsStmt           * aStmt,                \
                                              dtlValueBlockList * aValueBlockList,      \
                                              dtlDataValue      * aDataValue,           \
                                              SQLLEN            * aOffset,              \
                                              void              * aTargetValuePtr,      \
                                              SQLLEN            * aIndicator,           \
                                              zldDescElement    * aArdRec,              \
                                              SQLRETURN         * aReturn,              \
                                              stlErrorStack     * aErrorStack)

stlInt16 zlvGetNumericPrecision( stlFloat64 aValue );

stlInt16 zlvGetNumericPrecisionFromUInt64( stlUInt64 aValue );

stlStatus zlvMakeCNumericFromString( zlsStmt            * aStmt,
                                     SQL_NUMERIC_STRUCT * aCNumeric,
                                     stlChar            * aString,
                                     stlInt32             aLength,
                                     stlInt32             aPrecision,
                                     stlInt16             aScale,
                                     stlBool            * aIsTruncated,
                                     stlErrorStack      * aErrorStack );

stlStatus zlvMakeCNumericFromInteger( zlsStmt            * aStmt,
                                      SQL_NUMERIC_STRUCT * aCNumeric,
                                      stlUInt64            aUnsignedInteger,
                                      stlInt16             aPrecision,
                                      stlInt16             aScale,
                                      stlBool              aIsNegative,
                                      stlErrorStack      * aErrorStack );

stlStatus zlvMakeCNumericFromSecond( zlsStmt            * aStmt,
                                     SQL_NUMERIC_STRUCT * aCNumeric,
                                     stlUInt64            aSecond,
                                     stlUInt32            aFraction,
                                     stlInt16             aScale,
                                     stlBool              aIsNegative,
                                     stlBool            * aIsTruncated,
                                     stlErrorStack      * aErrorStack );

stlStatus zlvGetSqlToCFunc( zlsStmt       * aStmt,
                            dtlDataValue  * aDataValue,
                            stlInt16        aCType,
                            zlvSqlToCFunc * aFuncPtr,
                            stlErrorStack * aErrorStack );

stlStatus zlvInvalidSqlToC( zlsStmt           * aStmt,
                            dtlValueBlockList * aValueBlockList,
                            dtlDataValue      * aDataValue,
                            SQLLEN            * aOffset,
                            void              * aTargetValuePtr,
                            SQLLEN            * aIndicator,
                            zldDescElement    * aArdRec,
                            SQLRETURN         * aReturn,
                            stlErrorStack     * aErrorStack );

/*
 * SQL_CHAR -> C
 */
ZLV_DECLARE_SQL_TO_C( Char, Char );
ZLV_DECLARE_SQL_TO_C( Char, Wchar );
ZLV_DECLARE_SQL_TO_C( Char, LongVarChar );
ZLV_DECLARE_SQL_TO_C( Char, STinyInt );
ZLV_DECLARE_SQL_TO_C( Char, UTinyInt );
ZLV_DECLARE_SQL_TO_C( Char, SBigInt );
ZLV_DECLARE_SQL_TO_C( Char, UBigInt );
ZLV_DECLARE_SQL_TO_C( Char, SShort );
ZLV_DECLARE_SQL_TO_C( Char, UShort );
ZLV_DECLARE_SQL_TO_C( Char, SLong );
ZLV_DECLARE_SQL_TO_C( Char, ULong );
ZLV_DECLARE_SQL_TO_C( Char, Numeric );
ZLV_DECLARE_SQL_TO_C( Char, Float );
ZLV_DECLARE_SQL_TO_C( Char, Double );
ZLV_DECLARE_SQL_TO_C( Char, Bit );
ZLV_DECLARE_SQL_TO_C( Char, Boolean );
ZLV_DECLARE_SQL_TO_C( Char, Binary );
ZLV_DECLARE_SQL_TO_C( Char, LongVarBinary );
ZLV_DECLARE_SQL_TO_C( Char, Date );
ZLV_DECLARE_SQL_TO_C( Char, Time );
ZLV_DECLARE_SQL_TO_C( Char, TimeTz );
ZLV_DECLARE_SQL_TO_C( Char, Timestamp );
ZLV_DECLARE_SQL_TO_C( Char, TimestampTz );
ZLV_DECLARE_SQL_TO_C( Char, IntervalMonth );
ZLV_DECLARE_SQL_TO_C( Char, IntervalYear );
ZLV_DECLARE_SQL_TO_C( Char, IntervalYearToMonth );
ZLV_DECLARE_SQL_TO_C( Char, IntervalDay );
ZLV_DECLARE_SQL_TO_C( Char, IntervalHour );
ZLV_DECLARE_SQL_TO_C( Char, IntervalMinute );
ZLV_DECLARE_SQL_TO_C( Char, IntervalSecond );
ZLV_DECLARE_SQL_TO_C( Char, IntervalDayToHour );
ZLV_DECLARE_SQL_TO_C( Char, IntervalDayToMinute );
ZLV_DECLARE_SQL_TO_C( Char, IntervalDayToSecond );
ZLV_DECLARE_SQL_TO_C( Char, IntervalHourToMinute );
ZLV_DECLARE_SQL_TO_C( Char, IntervalHourToSecond );
ZLV_DECLARE_SQL_TO_C( Char, IntervalMinuteToSecond );

/*
 * SQL_VARCHAR -> C
 */
ZLV_DECLARE_SQL_TO_C( VarChar, Char );
ZLV_DECLARE_SQL_TO_C( VarChar, Wchar );
ZLV_DECLARE_SQL_TO_C( VarChar, LongVarChar );
ZLV_DECLARE_SQL_TO_C( VarChar, STinyInt );
ZLV_DECLARE_SQL_TO_C( VarChar, UTinyInt );
ZLV_DECLARE_SQL_TO_C( VarChar, SBigInt );
ZLV_DECLARE_SQL_TO_C( VarChar, UBigInt );
ZLV_DECLARE_SQL_TO_C( VarChar, SShort );
ZLV_DECLARE_SQL_TO_C( VarChar, UShort );
ZLV_DECLARE_SQL_TO_C( VarChar, SLong );
ZLV_DECLARE_SQL_TO_C( VarChar, ULong );
ZLV_DECLARE_SQL_TO_C( VarChar, Numeric );
ZLV_DECLARE_SQL_TO_C( VarChar, Float );
ZLV_DECLARE_SQL_TO_C( VarChar, Double );
ZLV_DECLARE_SQL_TO_C( VarChar, Bit );
ZLV_DECLARE_SQL_TO_C( VarChar, Boolean );
ZLV_DECLARE_SQL_TO_C( VarChar, Binary );
ZLV_DECLARE_SQL_TO_C( VarChar, LongVarBinary );
ZLV_DECLARE_SQL_TO_C( VarChar, Date );
ZLV_DECLARE_SQL_TO_C( VarChar, Time );
ZLV_DECLARE_SQL_TO_C( VarChar, TimeTz );
ZLV_DECLARE_SQL_TO_C( VarChar, Timestamp );
ZLV_DECLARE_SQL_TO_C( VarChar, TimestampTz );
ZLV_DECLARE_SQL_TO_C( VarChar, IntervalMonth );
ZLV_DECLARE_SQL_TO_C( VarChar, IntervalYear );
ZLV_DECLARE_SQL_TO_C( VarChar, IntervalYearToMonth );
ZLV_DECLARE_SQL_TO_C( VarChar, IntervalDay );
ZLV_DECLARE_SQL_TO_C( VarChar, IntervalHour );
ZLV_DECLARE_SQL_TO_C( VarChar, IntervalMinute );
ZLV_DECLARE_SQL_TO_C( VarChar, IntervalSecond );
ZLV_DECLARE_SQL_TO_C( VarChar, IntervalDayToHour );
ZLV_DECLARE_SQL_TO_C( VarChar, IntervalDayToMinute );
ZLV_DECLARE_SQL_TO_C( VarChar, IntervalDayToSecond );
ZLV_DECLARE_SQL_TO_C( VarChar, IntervalHourToMinute );
ZLV_DECLARE_SQL_TO_C( VarChar, IntervalHourToSecond );
ZLV_DECLARE_SQL_TO_C( VarChar, IntervalMinuteToSecond );

/*
 * SQL_LONGVARCHAR -> C
 */
ZLV_DECLARE_SQL_TO_C( LongVarChar, Char );
ZLV_DECLARE_SQL_TO_C( LongVarChar, Wchar );
ZLV_DECLARE_SQL_TO_C( LongVarChar, LongVarChar );
ZLV_DECLARE_SQL_TO_C( LongVarChar, STinyInt );
ZLV_DECLARE_SQL_TO_C( LongVarChar, UTinyInt );
ZLV_DECLARE_SQL_TO_C( LongVarChar, SBigInt );
ZLV_DECLARE_SQL_TO_C( LongVarChar, UBigInt );
ZLV_DECLARE_SQL_TO_C( LongVarChar, SShort );
ZLV_DECLARE_SQL_TO_C( LongVarChar, UShort );
ZLV_DECLARE_SQL_TO_C( LongVarChar, SLong );
ZLV_DECLARE_SQL_TO_C( LongVarChar, ULong );
ZLV_DECLARE_SQL_TO_C( LongVarChar, Numeric );
ZLV_DECLARE_SQL_TO_C( LongVarChar, Float );
ZLV_DECLARE_SQL_TO_C( LongVarChar, Double );
ZLV_DECLARE_SQL_TO_C( LongVarChar, Bit );
ZLV_DECLARE_SQL_TO_C( LongVarChar, Boolean );
ZLV_DECLARE_SQL_TO_C( LongVarChar, Binary );
ZLV_DECLARE_SQL_TO_C( LongVarChar, LongVarBinary );
ZLV_DECLARE_SQL_TO_C( LongVarChar, Date );
ZLV_DECLARE_SQL_TO_C( LongVarChar, Time );
ZLV_DECLARE_SQL_TO_C( LongVarChar, TimeTz );
ZLV_DECLARE_SQL_TO_C( LongVarChar, Timestamp );
ZLV_DECLARE_SQL_TO_C( LongVarChar, TimestampTz );
ZLV_DECLARE_SQL_TO_C( LongVarChar, IntervalMonth );
ZLV_DECLARE_SQL_TO_C( LongVarChar, IntervalYear );
ZLV_DECLARE_SQL_TO_C( LongVarChar, IntervalYearToMonth );
ZLV_DECLARE_SQL_TO_C( LongVarChar, IntervalDay );
ZLV_DECLARE_SQL_TO_C( LongVarChar, IntervalHour );
ZLV_DECLARE_SQL_TO_C( LongVarChar, IntervalMinute );
ZLV_DECLARE_SQL_TO_C( LongVarChar, IntervalSecond );
ZLV_DECLARE_SQL_TO_C( LongVarChar, IntervalDayToHour );
ZLV_DECLARE_SQL_TO_C( LongVarChar, IntervalDayToMinute );
ZLV_DECLARE_SQL_TO_C( LongVarChar, IntervalDayToSecond );
ZLV_DECLARE_SQL_TO_C( LongVarChar, IntervalHourToMinute );
ZLV_DECLARE_SQL_TO_C( LongVarChar, IntervalHourToSecond );
ZLV_DECLARE_SQL_TO_C( LongVarChar, IntervalMinuteToSecond );

/*
 * SQL_BIGINT -> C
 */
ZLV_DECLARE_SQL_TO_C( BigInt, Char );
ZLV_DECLARE_SQL_TO_C( BigInt, Wchar );
ZLV_DECLARE_SQL_TO_C( BigInt, LongVarChar );
ZLV_DECLARE_SQL_TO_C( BigInt, STinyInt );
ZLV_DECLARE_SQL_TO_C( BigInt, UTinyInt );
ZLV_DECLARE_SQL_TO_C( BigInt, TinyInt );
ZLV_DECLARE_SQL_TO_C( BigInt, SBigInt );
ZLV_DECLARE_SQL_TO_C( BigInt, UBigInt );
ZLV_DECLARE_SQL_TO_C( BigInt, SShort );
ZLV_DECLARE_SQL_TO_C( BigInt, UShort );
ZLV_DECLARE_SQL_TO_C( BigInt, Short );
ZLV_DECLARE_SQL_TO_C( BigInt, SLong );
ZLV_DECLARE_SQL_TO_C( BigInt, ULong );
ZLV_DECLARE_SQL_TO_C( BigInt, Long );
ZLV_DECLARE_SQL_TO_C( BigInt, Numeric );
ZLV_DECLARE_SQL_TO_C( BigInt, Float );
ZLV_DECLARE_SQL_TO_C( BigInt, Double );
ZLV_DECLARE_SQL_TO_C( BigInt, Bit );
ZLV_DECLARE_SQL_TO_C( BigInt, Binary );
ZLV_DECLARE_SQL_TO_C( BigInt, LongVarBinary );
ZLV_DECLARE_SQL_TO_C( BigInt, IntervalMonth );
ZLV_DECLARE_SQL_TO_C( BigInt, IntervalYear );
ZLV_DECLARE_SQL_TO_C( BigInt, IntervalYearToMonth );
ZLV_DECLARE_SQL_TO_C( BigInt, IntervalDay );
ZLV_DECLARE_SQL_TO_C( BigInt, IntervalHour );
ZLV_DECLARE_SQL_TO_C( BigInt, IntervalMinute );
ZLV_DECLARE_SQL_TO_C( BigInt, IntervalSecond );
ZLV_DECLARE_SQL_TO_C( BigInt, IntervalDayToHour );
ZLV_DECLARE_SQL_TO_C( BigInt, IntervalDayToMinute );
ZLV_DECLARE_SQL_TO_C( BigInt, IntervalDayToSecond );
ZLV_DECLARE_SQL_TO_C( BigInt, IntervalHourToMinute );
ZLV_DECLARE_SQL_TO_C( BigInt, IntervalHourToSecond );
ZLV_DECLARE_SQL_TO_C( BigInt, IntervalMinuteToSecond );

/*
 * SQL_NUMERIC -> C
 */
ZLV_DECLARE_SQL_TO_C( Numeric, Char );
ZLV_DECLARE_SQL_TO_C( Numeric, Wchar );
ZLV_DECLARE_SQL_TO_C( Numeric, LongVarChar );
ZLV_DECLARE_SQL_TO_C( Numeric, STinyInt );
ZLV_DECLARE_SQL_TO_C( Numeric, UTinyInt );
ZLV_DECLARE_SQL_TO_C( Numeric, SBigInt );
ZLV_DECLARE_SQL_TO_C( Numeric, UBigInt );
ZLV_DECLARE_SQL_TO_C( Numeric, SShort );
ZLV_DECLARE_SQL_TO_C( Numeric, UShort );
ZLV_DECLARE_SQL_TO_C( Numeric, SLong );
ZLV_DECLARE_SQL_TO_C( Numeric, ULong );
ZLV_DECLARE_SQL_TO_C( Numeric, Numeric );
ZLV_DECLARE_SQL_TO_C( Numeric, Float );
ZLV_DECLARE_SQL_TO_C( Numeric, Double );
ZLV_DECLARE_SQL_TO_C( Numeric, Bit );
ZLV_DECLARE_SQL_TO_C( Numeric, Binary );
ZLV_DECLARE_SQL_TO_C( Numeric, LongVarBinary );
ZLV_DECLARE_SQL_TO_C( Numeric, IntervalMonth );
ZLV_DECLARE_SQL_TO_C( Numeric, IntervalYear );
ZLV_DECLARE_SQL_TO_C( Numeric, IntervalYearToMonth );
ZLV_DECLARE_SQL_TO_C( Numeric, IntervalDay );
ZLV_DECLARE_SQL_TO_C( Numeric, IntervalHour );
ZLV_DECLARE_SQL_TO_C( Numeric, IntervalMinute );
ZLV_DECLARE_SQL_TO_C( Numeric, IntervalSecond );
ZLV_DECLARE_SQL_TO_C( Numeric, IntervalDayToHour );
ZLV_DECLARE_SQL_TO_C( Numeric, IntervalDayToMinute );
ZLV_DECLARE_SQL_TO_C( Numeric, IntervalDayToSecond );
ZLV_DECLARE_SQL_TO_C( Numeric, IntervalHourToMinute );
ZLV_DECLARE_SQL_TO_C( Numeric, IntervalHourToSecond );
ZLV_DECLARE_SQL_TO_C( Numeric, IntervalMinuteToSecond );

/*
 * SQL_SMALLINT -> C
 */
ZLV_DECLARE_SQL_TO_C( SmallInt, Char );
ZLV_DECLARE_SQL_TO_C( SmallInt, Wchar );
ZLV_DECLARE_SQL_TO_C( SmallInt, LongVarChar );
ZLV_DECLARE_SQL_TO_C( SmallInt, STinyInt );
ZLV_DECLARE_SQL_TO_C( SmallInt, UTinyInt );
ZLV_DECLARE_SQL_TO_C( SmallInt, SBigInt );
ZLV_DECLARE_SQL_TO_C( SmallInt, UBigInt );
ZLV_DECLARE_SQL_TO_C( SmallInt, SShort );
ZLV_DECLARE_SQL_TO_C( SmallInt, UShort );
ZLV_DECLARE_SQL_TO_C( SmallInt, SLong );
ZLV_DECLARE_SQL_TO_C( SmallInt, ULong );
ZLV_DECLARE_SQL_TO_C( SmallInt, Numeric );
ZLV_DECLARE_SQL_TO_C( SmallInt, Float );
ZLV_DECLARE_SQL_TO_C( SmallInt, Double );
ZLV_DECLARE_SQL_TO_C( SmallInt, Bit );
ZLV_DECLARE_SQL_TO_C( SmallInt, Binary );
ZLV_DECLARE_SQL_TO_C( SmallInt, LongVarBinary );
ZLV_DECLARE_SQL_TO_C( SmallInt, IntervalMonth );
ZLV_DECLARE_SQL_TO_C( SmallInt, IntervalYear );
ZLV_DECLARE_SQL_TO_C( SmallInt, IntervalYearToMonth );
ZLV_DECLARE_SQL_TO_C( SmallInt, IntervalDay );
ZLV_DECLARE_SQL_TO_C( SmallInt, IntervalHour );
ZLV_DECLARE_SQL_TO_C( SmallInt, IntervalMinute );
ZLV_DECLARE_SQL_TO_C( SmallInt, IntervalSecond );
ZLV_DECLARE_SQL_TO_C( SmallInt, IntervalDayToHour );
ZLV_DECLARE_SQL_TO_C( SmallInt, IntervalDayToMinute );
ZLV_DECLARE_SQL_TO_C( SmallInt, IntervalDayToSecond );
ZLV_DECLARE_SQL_TO_C( SmallInt, IntervalHourToMinute );
ZLV_DECLARE_SQL_TO_C( SmallInt, IntervalHourToSecond );
ZLV_DECLARE_SQL_TO_C( SmallInt, IntervalMinuteToSecond );

/*
 * SQL_INTEGER -> C
 */
ZLV_DECLARE_SQL_TO_C( Integer, Char );
ZLV_DECLARE_SQL_TO_C( Integer, Wchar );
ZLV_DECLARE_SQL_TO_C( Integer, LongVarChar );
ZLV_DECLARE_SQL_TO_C( Integer, STinyInt );
ZLV_DECLARE_SQL_TO_C( Integer, UTinyInt );
ZLV_DECLARE_SQL_TO_C( Integer, TinyInt );
ZLV_DECLARE_SQL_TO_C( Integer, SBigInt );
ZLV_DECLARE_SQL_TO_C( Integer, UBigInt );
ZLV_DECLARE_SQL_TO_C( Integer, SShort );
ZLV_DECLARE_SQL_TO_C( Integer, UShort );
ZLV_DECLARE_SQL_TO_C( Integer, Short );
ZLV_DECLARE_SQL_TO_C( Integer, SLong );
ZLV_DECLARE_SQL_TO_C( Integer, ULong );
ZLV_DECLARE_SQL_TO_C( Integer, Numeric );
ZLV_DECLARE_SQL_TO_C( Integer, Float );
ZLV_DECLARE_SQL_TO_C( Integer, Double );
ZLV_DECLARE_SQL_TO_C( Integer, Bit );
ZLV_DECLARE_SQL_TO_C( Integer, Binary );
ZLV_DECLARE_SQL_TO_C( Integer, LongVarBinary );
ZLV_DECLARE_SQL_TO_C( Integer, IntervalMonth );
ZLV_DECLARE_SQL_TO_C( Integer, IntervalYear );
ZLV_DECLARE_SQL_TO_C( Integer, IntervalYearToMonth );
ZLV_DECLARE_SQL_TO_C( Integer, IntervalDay );
ZLV_DECLARE_SQL_TO_C( Integer, IntervalHour );
ZLV_DECLARE_SQL_TO_C( Integer, IntervalMinute );
ZLV_DECLARE_SQL_TO_C( Integer, IntervalSecond );
ZLV_DECLARE_SQL_TO_C( Integer, IntervalDayToHour );
ZLV_DECLARE_SQL_TO_C( Integer, IntervalDayToMinute );
ZLV_DECLARE_SQL_TO_C( Integer, IntervalDayToSecond );
ZLV_DECLARE_SQL_TO_C( Integer, IntervalHourToMinute );
ZLV_DECLARE_SQL_TO_C( Integer, IntervalHourToSecond );
ZLV_DECLARE_SQL_TO_C( Integer, IntervalMinuteToSecond );

/*
 * SQL_REAL -> C
 */
ZLV_DECLARE_SQL_TO_C( Real, Char );
ZLV_DECLARE_SQL_TO_C( Real, Wchar );
ZLV_DECLARE_SQL_TO_C( Real, LongVarChar );
ZLV_DECLARE_SQL_TO_C( Real, STinyInt );
ZLV_DECLARE_SQL_TO_C( Real, UTinyInt );
ZLV_DECLARE_SQL_TO_C( Real, SBigInt );
ZLV_DECLARE_SQL_TO_C( Real, UBigInt );
ZLV_DECLARE_SQL_TO_C( Real, SShort );
ZLV_DECLARE_SQL_TO_C( Real, UShort );
ZLV_DECLARE_SQL_TO_C( Real, SLong );
ZLV_DECLARE_SQL_TO_C( Real, ULong );
ZLV_DECLARE_SQL_TO_C( Real, Numeric );
ZLV_DECLARE_SQL_TO_C( Real, Float );
ZLV_DECLARE_SQL_TO_C( Real, Double );
ZLV_DECLARE_SQL_TO_C( Real, Bit );
ZLV_DECLARE_SQL_TO_C( Real, Binary );
ZLV_DECLARE_SQL_TO_C( Real, LongVarBinary );
ZLV_DECLARE_SQL_TO_C( Real, IntervalYearToMonth );
ZLV_DECLARE_SQL_TO_C( Real, IntervalDayToHour );
ZLV_DECLARE_SQL_TO_C( Real, IntervalDayToMinute );
ZLV_DECLARE_SQL_TO_C( Real, IntervalDayToSecond );
ZLV_DECLARE_SQL_TO_C( Real, IntervalHourToMinute );
ZLV_DECLARE_SQL_TO_C( Real, IntervalHourToSecond );
ZLV_DECLARE_SQL_TO_C( Real, IntervalMinuteToSecond );

/*
 * SQL_DOUBLE -> C
 */
ZLV_DECLARE_SQL_TO_C( Double, Char );
ZLV_DECLARE_SQL_TO_C( Double, Wchar );
ZLV_DECLARE_SQL_TO_C( Double, LongVarChar );
ZLV_DECLARE_SQL_TO_C( Double, STinyInt );
ZLV_DECLARE_SQL_TO_C( Double, UTinyInt );
ZLV_DECLARE_SQL_TO_C( Double, SBigInt );
ZLV_DECLARE_SQL_TO_C( Double, UBigInt );
ZLV_DECLARE_SQL_TO_C( Double, SShort );
ZLV_DECLARE_SQL_TO_C( Double, UShort );
ZLV_DECLARE_SQL_TO_C( Double, SLong );
ZLV_DECLARE_SQL_TO_C( Double, ULong );
ZLV_DECLARE_SQL_TO_C( Double, Numeric );
ZLV_DECLARE_SQL_TO_C( Double, Float );
ZLV_DECLARE_SQL_TO_C( Double, Double );
ZLV_DECLARE_SQL_TO_C( Double, Bit );
ZLV_DECLARE_SQL_TO_C( Double, Binary );
ZLV_DECLARE_SQL_TO_C( Double, LongVarBinary );
ZLV_DECLARE_SQL_TO_C( Double, IntervalYearToMonth );
ZLV_DECLARE_SQL_TO_C( Double, IntervalDayToHour );
ZLV_DECLARE_SQL_TO_C( Double, IntervalDayToMinute );
ZLV_DECLARE_SQL_TO_C( Double, IntervalDayToSecond );
ZLV_DECLARE_SQL_TO_C( Double, IntervalHourToMinute );
ZLV_DECLARE_SQL_TO_C( Double, IntervalHourToSecond );
ZLV_DECLARE_SQL_TO_C( Double, IntervalMinuteToSecond );

/*
 * SQL_BOOLEAN -> C
 */
ZLV_DECLARE_SQL_TO_C( Boolean, Char );
ZLV_DECLARE_SQL_TO_C( Boolean, Wchar );
ZLV_DECLARE_SQL_TO_C( Boolean, LongVarChar );
ZLV_DECLARE_SQL_TO_C( Boolean, STinyInt );
ZLV_DECLARE_SQL_TO_C( Boolean, UTinyInt );
ZLV_DECLARE_SQL_TO_C( Boolean, SBigInt );
ZLV_DECLARE_SQL_TO_C( Boolean, UBigInt );
ZLV_DECLARE_SQL_TO_C( Boolean, SShort );
ZLV_DECLARE_SQL_TO_C( Boolean, UShort );
ZLV_DECLARE_SQL_TO_C( Boolean, SLong );
ZLV_DECLARE_SQL_TO_C( Boolean, ULong );
ZLV_DECLARE_SQL_TO_C( Boolean, Numeric );
ZLV_DECLARE_SQL_TO_C( Boolean, Float );
ZLV_DECLARE_SQL_TO_C( Boolean, Double );
ZLV_DECLARE_SQL_TO_C( Boolean, Bit );
ZLV_DECLARE_SQL_TO_C( Boolean, Boolean );
ZLV_DECLARE_SQL_TO_C( Boolean, Binary );
ZLV_DECLARE_SQL_TO_C( Boolean, LongVarBinary );

/*
 * SQL_BINARY -> C
 */
ZLV_DECLARE_SQL_TO_C( Binary, Char );
ZLV_DECLARE_SQL_TO_C( Binary, Wchar );
ZLV_DECLARE_SQL_TO_C( Binary, LongVarChar );
ZLV_DECLARE_SQL_TO_C( Binary, Binary );
ZLV_DECLARE_SQL_TO_C( Binary, LongVarBinary );

/*
 * SQL_VARBINARY -> C
 */
ZLV_DECLARE_SQL_TO_C( VarBinary, Char );
ZLV_DECLARE_SQL_TO_C( VarBinary, Wchar );
ZLV_DECLARE_SQL_TO_C( VarBinary, LongVarChar );
ZLV_DECLARE_SQL_TO_C( VarBinary, Binary );
ZLV_DECLARE_SQL_TO_C( VarBinary, LongVarBinary );

/*
 * SQL_VARBINARY -> C
 */
ZLV_DECLARE_SQL_TO_C( LongVarBinary, Char );
ZLV_DECLARE_SQL_TO_C( LongVarBinary, Wchar );
ZLV_DECLARE_SQL_TO_C( LongVarBinary, LongVarChar );
ZLV_DECLARE_SQL_TO_C( LongVarBinary, Binary );
ZLV_DECLARE_SQL_TO_C( LongVarBinary, LongVarBinary );

/*
 * SQL_DATE -> C
 */
ZLV_DECLARE_SQL_TO_C( Date, Char );
ZLV_DECLARE_SQL_TO_C( Date, Wchar );
ZLV_DECLARE_SQL_TO_C( Date, LongVarChar );
ZLV_DECLARE_SQL_TO_C( Date, Binary );
ZLV_DECLARE_SQL_TO_C( Date, LongVarBinary );
ZLV_DECLARE_SQL_TO_C( Date, Date );
ZLV_DECLARE_SQL_TO_C( Date, Time );
ZLV_DECLARE_SQL_TO_C( Date, TimeTz );
ZLV_DECLARE_SQL_TO_C( Date, Timestamp );
ZLV_DECLARE_SQL_TO_C( Date, TimestampTz );

/*
 * SQL_TIME -> C
 */
ZLV_DECLARE_SQL_TO_C( Time, Char );
ZLV_DECLARE_SQL_TO_C( Time, Wchar );
ZLV_DECLARE_SQL_TO_C( Time, LongVarChar );
ZLV_DECLARE_SQL_TO_C( Time, Binary );
ZLV_DECLARE_SQL_TO_C( Time, LongVarBinary );
ZLV_DECLARE_SQL_TO_C( Time, Time );
ZLV_DECLARE_SQL_TO_C( Time, TimeTz );
ZLV_DECLARE_SQL_TO_C( Time, Timestamp );
ZLV_DECLARE_SQL_TO_C( Time, TimestampTz );

/*
 * SQL_TYPE_TIME_TZ -> C
 */
ZLV_DECLARE_SQL_TO_C( TimeTz, Char );
ZLV_DECLARE_SQL_TO_C( TimeTz, Wchar );
ZLV_DECLARE_SQL_TO_C( TimeTz, LongVarChar );
ZLV_DECLARE_SQL_TO_C( TimeTz, Binary );
ZLV_DECLARE_SQL_TO_C( TimeTz, LongVarBinary );
ZLV_DECLARE_SQL_TO_C( TimeTz, Time );
ZLV_DECLARE_SQL_TO_C( TimeTz, TimeTz );
ZLV_DECLARE_SQL_TO_C( TimeTz, Timestamp );
ZLV_DECLARE_SQL_TO_C( TimeTz, TimestampTz );

/*
 * SQL_TIMESTAMP -> C
 */
ZLV_DECLARE_SQL_TO_C( Timestamp, Char );
ZLV_DECLARE_SQL_TO_C( Timestamp, Wchar );
ZLV_DECLARE_SQL_TO_C( Timestamp, LongVarChar );
ZLV_DECLARE_SQL_TO_C( Timestamp, Binary );
ZLV_DECLARE_SQL_TO_C( Timestamp, LongVarBinary );
ZLV_DECLARE_SQL_TO_C( Timestamp, Date );
ZLV_DECLARE_SQL_TO_C( Timestamp, Time );
ZLV_DECLARE_SQL_TO_C( Timestamp, TimeTz );
ZLV_DECLARE_SQL_TO_C( Timestamp, Timestamp );
ZLV_DECLARE_SQL_TO_C( Timestamp, TimestampTz );

/*
 * SQL_TYPE_TIMESTAMP_TZ -> C
 */
ZLV_DECLARE_SQL_TO_C( TimestampTz, Char );
ZLV_DECLARE_SQL_TO_C( TimestampTz, Wchar );
ZLV_DECLARE_SQL_TO_C( TimestampTz, LongVarChar );
ZLV_DECLARE_SQL_TO_C( TimestampTz, Binary );
ZLV_DECLARE_SQL_TO_C( TimestampTz, LongVarBinary );
ZLV_DECLARE_SQL_TO_C( TimestampTz, Date );
ZLV_DECLARE_SQL_TO_C( TimestampTz, Time );
ZLV_DECLARE_SQL_TO_C( TimestampTz, TimeTz );
ZLV_DECLARE_SQL_TO_C( TimestampTz, Timestamp );
ZLV_DECLARE_SQL_TO_C( TimestampTz, TimestampTz );

/*
 * SQL_INTERVAL_YEAR -> C
 */
ZLV_DECLARE_SQL_TO_C( IntervalYear, IntervalMonth );
ZLV_DECLARE_SQL_TO_C( IntervalYear, IntervalYear );
ZLV_DECLARE_SQL_TO_C( IntervalYear, IntervalYearToMonth );
ZLV_DECLARE_SQL_TO_C( IntervalYear, STinyInt );
ZLV_DECLARE_SQL_TO_C( IntervalYear, UTinyInt );
ZLV_DECLARE_SQL_TO_C( IntervalYear, TinyInt );
ZLV_DECLARE_SQL_TO_C( IntervalYear, SBigInt );
ZLV_DECLARE_SQL_TO_C( IntervalYear, UBigInt );
ZLV_DECLARE_SQL_TO_C( IntervalYear, SShort );
ZLV_DECLARE_SQL_TO_C( IntervalYear, UShort );
ZLV_DECLARE_SQL_TO_C( IntervalYear, Short );
ZLV_DECLARE_SQL_TO_C( IntervalYear, SLong );
ZLV_DECLARE_SQL_TO_C( IntervalYear, ULong );
ZLV_DECLARE_SQL_TO_C( IntervalYear, Long );
ZLV_DECLARE_SQL_TO_C( IntervalYear, Numeric );
ZLV_DECLARE_SQL_TO_C( IntervalYear, Binary );
ZLV_DECLARE_SQL_TO_C( IntervalYear, LongVarBinary );
ZLV_DECLARE_SQL_TO_C( IntervalYear, Char );
ZLV_DECLARE_SQL_TO_C( IntervalYear, Wchar );
ZLV_DECLARE_SQL_TO_C( IntervalYear, LongVarChar );

/*
 * SQL_INTERVAL_MONTH -> C
 */
ZLV_DECLARE_SQL_TO_C( IntervalMonth, IntervalMonth );
ZLV_DECLARE_SQL_TO_C( IntervalMonth, IntervalYear );
ZLV_DECLARE_SQL_TO_C( IntervalMonth, IntervalYearToMonth );
ZLV_DECLARE_SQL_TO_C( IntervalMonth, STinyInt );
ZLV_DECLARE_SQL_TO_C( IntervalMonth, UTinyInt );
ZLV_DECLARE_SQL_TO_C( IntervalMonth, SBigInt );
ZLV_DECLARE_SQL_TO_C( IntervalMonth, UBigInt );
ZLV_DECLARE_SQL_TO_C( IntervalMonth, SShort );
ZLV_DECLARE_SQL_TO_C( IntervalMonth, UShort );
ZLV_DECLARE_SQL_TO_C( IntervalMonth, SLong );
ZLV_DECLARE_SQL_TO_C( IntervalMonth, ULong );
ZLV_DECLARE_SQL_TO_C( IntervalMonth, Numeric );
ZLV_DECLARE_SQL_TO_C( IntervalMonth, Binary );
ZLV_DECLARE_SQL_TO_C( IntervalMonth, LongVarBinary );
ZLV_DECLARE_SQL_TO_C( IntervalMonth, Char );
ZLV_DECLARE_SQL_TO_C( IntervalMonth, Wchar );
ZLV_DECLARE_SQL_TO_C( IntervalMonth, LongVarChar );

/*
 * SQL_INTERVAL_YEAR_TO_MONTH -> C
 */
ZLV_DECLARE_SQL_TO_C( IntervalYearToMonth, IntervalMonth );
ZLV_DECLARE_SQL_TO_C( IntervalYearToMonth, IntervalYear );
ZLV_DECLARE_SQL_TO_C( IntervalYearToMonth, IntervalYearToMonth );
ZLV_DECLARE_SQL_TO_C( IntervalYearToMonth, Binary );
ZLV_DECLARE_SQL_TO_C( IntervalYearToMonth, LongVarBinary );
ZLV_DECLARE_SQL_TO_C( IntervalYearToMonth, Char );
ZLV_DECLARE_SQL_TO_C( IntervalYearToMonth, Wchar );
ZLV_DECLARE_SQL_TO_C( IntervalYearToMonth, LongVarChar );

/*
 * SQL_INTERVAL_DAY
 */
ZLV_DECLARE_SQL_TO_C( IntervalDay, IntervalDay );
ZLV_DECLARE_SQL_TO_C( IntervalDay, IntervalHour );
ZLV_DECLARE_SQL_TO_C( IntervalDay, IntervalMinute );
ZLV_DECLARE_SQL_TO_C( IntervalDay, IntervalSecond );
ZLV_DECLARE_SQL_TO_C( IntervalDay, IntervalDayToHour );
ZLV_DECLARE_SQL_TO_C( IntervalDay, IntervalDayToMinute );
ZLV_DECLARE_SQL_TO_C( IntervalDay, IntervalDayToSecond );
ZLV_DECLARE_SQL_TO_C( IntervalDay, IntervalHourToMinute );
ZLV_DECLARE_SQL_TO_C( IntervalDay, IntervalHourToSecond );
ZLV_DECLARE_SQL_TO_C( IntervalDay, IntervalMinuteToSecond );
ZLV_DECLARE_SQL_TO_C( IntervalDay, STinyInt );
ZLV_DECLARE_SQL_TO_C( IntervalDay, UTinyInt );
ZLV_DECLARE_SQL_TO_C( IntervalDay, SBigInt );
ZLV_DECLARE_SQL_TO_C( IntervalDay, UBigInt );
ZLV_DECLARE_SQL_TO_C( IntervalDay, SShort );
ZLV_DECLARE_SQL_TO_C( IntervalDay, UShort );
ZLV_DECLARE_SQL_TO_C( IntervalDay, SLong );
ZLV_DECLARE_SQL_TO_C( IntervalDay, ULong );
ZLV_DECLARE_SQL_TO_C( IntervalDay, Numeric );
ZLV_DECLARE_SQL_TO_C( IntervalDay, Binary );
ZLV_DECLARE_SQL_TO_C( IntervalDay, LongVarBinary );
ZLV_DECLARE_SQL_TO_C( IntervalDay, Char );
ZLV_DECLARE_SQL_TO_C( IntervalDay, Wchar );
ZLV_DECLARE_SQL_TO_C( IntervalDay, LongVarChar );

/*
 * SQL_INTERVAL_HOUR
 */
ZLV_DECLARE_SQL_TO_C( IntervalHour, IntervalDay );
ZLV_DECLARE_SQL_TO_C( IntervalHour, IntervalHour );
ZLV_DECLARE_SQL_TO_C( IntervalHour, IntervalMinute );
ZLV_DECLARE_SQL_TO_C( IntervalHour, IntervalSecond );
ZLV_DECLARE_SQL_TO_C( IntervalHour, IntervalDayToHour );
ZLV_DECLARE_SQL_TO_C( IntervalHour, IntervalDayToMinute );
ZLV_DECLARE_SQL_TO_C( IntervalHour, IntervalDayToSecond );
ZLV_DECLARE_SQL_TO_C( IntervalHour, IntervalHourToMinute );
ZLV_DECLARE_SQL_TO_C( IntervalHour, IntervalHourToSecond );
ZLV_DECLARE_SQL_TO_C( IntervalHour, IntervalMinuteToSecond );
ZLV_DECLARE_SQL_TO_C( IntervalHour, STinyInt );
ZLV_DECLARE_SQL_TO_C( IntervalHour, UTinyInt );
ZLV_DECLARE_SQL_TO_C( IntervalHour, SBigInt );
ZLV_DECLARE_SQL_TO_C( IntervalHour, UBigInt );
ZLV_DECLARE_SQL_TO_C( IntervalHour, SShort );
ZLV_DECLARE_SQL_TO_C( IntervalHour, UShort );
ZLV_DECLARE_SQL_TO_C( IntervalHour, SLong );
ZLV_DECLARE_SQL_TO_C( IntervalHour, ULong );
ZLV_DECLARE_SQL_TO_C( IntervalHour, Numeric );
ZLV_DECLARE_SQL_TO_C( IntervalHour, Binary );
ZLV_DECLARE_SQL_TO_C( IntervalHour, LongVarBinary );
ZLV_DECLARE_SQL_TO_C( IntervalHour, Char );
ZLV_DECLARE_SQL_TO_C( IntervalHour, Wchar );
ZLV_DECLARE_SQL_TO_C( IntervalHour, LongVarChar );


/*
 * SQL_INTERVAL_MINUTE
 */
ZLV_DECLARE_SQL_TO_C( IntervalMinute, IntervalDay );
ZLV_DECLARE_SQL_TO_C( IntervalMinute, IntervalHour );
ZLV_DECLARE_SQL_TO_C( IntervalMinute, IntervalMinute );
ZLV_DECLARE_SQL_TO_C( IntervalMinute, IntervalSecond );
ZLV_DECLARE_SQL_TO_C( IntervalMinute, IntervalDayToHour );
ZLV_DECLARE_SQL_TO_C( IntervalMinute, IntervalDayToMinute );
ZLV_DECLARE_SQL_TO_C( IntervalMinute, IntervalDayToSecond );
ZLV_DECLARE_SQL_TO_C( IntervalMinute, IntervalHourToMinute );
ZLV_DECLARE_SQL_TO_C( IntervalMinute, IntervalHourToSecond );
ZLV_DECLARE_SQL_TO_C( IntervalMinute, IntervalMinuteToSecond );
ZLV_DECLARE_SQL_TO_C( IntervalMinute, STinyInt );
ZLV_DECLARE_SQL_TO_C( IntervalMinute, UTinyInt );
ZLV_DECLARE_SQL_TO_C( IntervalMinute, SBigInt );
ZLV_DECLARE_SQL_TO_C( IntervalMinute, UBigInt );
ZLV_DECLARE_SQL_TO_C( IntervalMinute, SShort );
ZLV_DECLARE_SQL_TO_C( IntervalMinute, UShort );
ZLV_DECLARE_SQL_TO_C( IntervalMinute, SLong );
ZLV_DECLARE_SQL_TO_C( IntervalMinute, ULong );
ZLV_DECLARE_SQL_TO_C( IntervalMinute, Numeric );
ZLV_DECLARE_SQL_TO_C( IntervalMinute, Binary );
ZLV_DECLARE_SQL_TO_C( IntervalMinute, LongVarBinary );
ZLV_DECLARE_SQL_TO_C( IntervalMinute, Char );
ZLV_DECLARE_SQL_TO_C( IntervalMinute, Wchar );
ZLV_DECLARE_SQL_TO_C( IntervalMinute, LongVarChar );

/*
 * SQL_INTERVAL_SECOND
 */
ZLV_DECLARE_SQL_TO_C( IntervalSecond, IntervalDay );
ZLV_DECLARE_SQL_TO_C( IntervalSecond, IntervalHour );
ZLV_DECLARE_SQL_TO_C( IntervalSecond, IntervalMinute );
ZLV_DECLARE_SQL_TO_C( IntervalSecond, IntervalSecond );
ZLV_DECLARE_SQL_TO_C( IntervalSecond, IntervalDayToHour );
ZLV_DECLARE_SQL_TO_C( IntervalSecond, IntervalDayToMinute );
ZLV_DECLARE_SQL_TO_C( IntervalSecond, IntervalDayToSecond );
ZLV_DECLARE_SQL_TO_C( IntervalSecond, IntervalHourToMinute );
ZLV_DECLARE_SQL_TO_C( IntervalSecond, IntervalHourToSecond );
ZLV_DECLARE_SQL_TO_C( IntervalSecond, IntervalMinuteToSecond );
ZLV_DECLARE_SQL_TO_C( IntervalSecond, STinyInt );
ZLV_DECLARE_SQL_TO_C( IntervalSecond, UTinyInt );
ZLV_DECLARE_SQL_TO_C( IntervalSecond, SBigInt );
ZLV_DECLARE_SQL_TO_C( IntervalSecond, UBigInt );
ZLV_DECLARE_SQL_TO_C( IntervalSecond, SShort );
ZLV_DECLARE_SQL_TO_C( IntervalSecond, UShort );
ZLV_DECLARE_SQL_TO_C( IntervalSecond, SLong );
ZLV_DECLARE_SQL_TO_C( IntervalSecond, ULong );
ZLV_DECLARE_SQL_TO_C( IntervalSecond, Numeric );
ZLV_DECLARE_SQL_TO_C( IntervalSecond, Binary );
ZLV_DECLARE_SQL_TO_C( IntervalSecond, LongVarBinary );
ZLV_DECLARE_SQL_TO_C( IntervalSecond, Char );
ZLV_DECLARE_SQL_TO_C( IntervalSecond, Wchar );
ZLV_DECLARE_SQL_TO_C( IntervalSecond, LongVarChar );

/*
 * SQL_INTERVAL_DAY_TO_HOUR
 */
ZLV_DECLARE_SQL_TO_C( IntervalDayToHour, IntervalDay );
ZLV_DECLARE_SQL_TO_C( IntervalDayToHour, IntervalHour );
ZLV_DECLARE_SQL_TO_C( IntervalDayToHour, IntervalMinute );
ZLV_DECLARE_SQL_TO_C( IntervalDayToHour, IntervalSecond );
ZLV_DECLARE_SQL_TO_C( IntervalDayToHour, IntervalDayToHour );
ZLV_DECLARE_SQL_TO_C( IntervalDayToHour, IntervalDayToMinute );
ZLV_DECLARE_SQL_TO_C( IntervalDayToHour, IntervalDayToSecond );
ZLV_DECLARE_SQL_TO_C( IntervalDayToHour, IntervalHourToMinute );
ZLV_DECLARE_SQL_TO_C( IntervalDayToHour, IntervalHourToSecond );
ZLV_DECLARE_SQL_TO_C( IntervalDayToHour, IntervalMinuteToSecond );
ZLV_DECLARE_SQL_TO_C( IntervalDayToHour, Binary );
ZLV_DECLARE_SQL_TO_C( IntervalDayToHour, LongVarBinary );
ZLV_DECLARE_SQL_TO_C( IntervalDayToHour, Char );
ZLV_DECLARE_SQL_TO_C( IntervalDayToHour, Wchar );
ZLV_DECLARE_SQL_TO_C( IntervalDayToHour, LongVarChar );

/*
 * SQL_INTERVAL_DAY_TO_MINUTE
 */
ZLV_DECLARE_SQL_TO_C( IntervalDayToMinute, IntervalDay );
ZLV_DECLARE_SQL_TO_C( IntervalDayToMinute, IntervalHour );
ZLV_DECLARE_SQL_TO_C( IntervalDayToMinute, IntervalMinute );
ZLV_DECLARE_SQL_TO_C( IntervalDayToMinute, IntervalSecond );
ZLV_DECLARE_SQL_TO_C( IntervalDayToMinute, IntervalDayToHour );
ZLV_DECLARE_SQL_TO_C( IntervalDayToMinute, IntervalDayToMinute );
ZLV_DECLARE_SQL_TO_C( IntervalDayToMinute, IntervalDayToSecond );
ZLV_DECLARE_SQL_TO_C( IntervalDayToMinute, IntervalHourToMinute );
ZLV_DECLARE_SQL_TO_C( IntervalDayToMinute, IntervalHourToSecond );
ZLV_DECLARE_SQL_TO_C( IntervalDayToMinute, IntervalMinuteToSecond );
ZLV_DECLARE_SQL_TO_C( IntervalDayToMinute, Binary );
ZLV_DECLARE_SQL_TO_C( IntervalDayToMinute, LongVarBinary );
ZLV_DECLARE_SQL_TO_C( IntervalDayToMinute, Char );
ZLV_DECLARE_SQL_TO_C( IntervalDayToMinute, Wchar );
ZLV_DECLARE_SQL_TO_C( IntervalDayToMinute, LongVarChar );

/*
 * SQL_INTERVAL_DAY_TO_SECOND
 */
ZLV_DECLARE_SQL_TO_C( IntervalDayToSecond, IntervalDay );
ZLV_DECLARE_SQL_TO_C( IntervalDayToSecond, IntervalHour );
ZLV_DECLARE_SQL_TO_C( IntervalDayToSecond, IntervalMinute );
ZLV_DECLARE_SQL_TO_C( IntervalDayToSecond, IntervalSecond );
ZLV_DECLARE_SQL_TO_C( IntervalDayToSecond, IntervalDayToHour );
ZLV_DECLARE_SQL_TO_C( IntervalDayToSecond, IntervalDayToMinute );
ZLV_DECLARE_SQL_TO_C( IntervalDayToSecond, IntervalDayToSecond );
ZLV_DECLARE_SQL_TO_C( IntervalDayToSecond, IntervalHourToMinute );
ZLV_DECLARE_SQL_TO_C( IntervalDayToSecond, IntervalHourToSecond );
ZLV_DECLARE_SQL_TO_C( IntervalDayToSecond, IntervalMinuteToSecond );
ZLV_DECLARE_SQL_TO_C( IntervalDayToSecond, Binary );
ZLV_DECLARE_SQL_TO_C( IntervalDayToSecond, LongVarBinary );
ZLV_DECLARE_SQL_TO_C( IntervalDayToSecond, Char );
ZLV_DECLARE_SQL_TO_C( IntervalDayToSecond, Wchar );
ZLV_DECLARE_SQL_TO_C( IntervalDayToSecond, LongVarChar );

/*
 * SQL_INTERVAL_HOUR_TO_MINUTE
 */
ZLV_DECLARE_SQL_TO_C( IntervalHourToMinute, IntervalDay );
ZLV_DECLARE_SQL_TO_C( IntervalHourToMinute, IntervalHour );
ZLV_DECLARE_SQL_TO_C( IntervalHourToMinute, IntervalMinute );
ZLV_DECLARE_SQL_TO_C( IntervalHourToMinute, IntervalSecond );
ZLV_DECLARE_SQL_TO_C( IntervalHourToMinute, IntervalDayToHour );
ZLV_DECLARE_SQL_TO_C( IntervalHourToMinute, IntervalDayToMinute );
ZLV_DECLARE_SQL_TO_C( IntervalHourToMinute, IntervalDayToSecond );
ZLV_DECLARE_SQL_TO_C( IntervalHourToMinute, IntervalHourToMinute );
ZLV_DECLARE_SQL_TO_C( IntervalHourToMinute, IntervalHourToSecond );
ZLV_DECLARE_SQL_TO_C( IntervalHourToMinute, IntervalMinuteToSecond );
ZLV_DECLARE_SQL_TO_C( IntervalHourToMinute, Binary );
ZLV_DECLARE_SQL_TO_C( IntervalHourToMinute, LongVarBinary );
ZLV_DECLARE_SQL_TO_C( IntervalHourToMinute, Char );
ZLV_DECLARE_SQL_TO_C( IntervalHourToMinute, Wchar );
ZLV_DECLARE_SQL_TO_C( IntervalHourToMinute, LongVarChar );

/*
 * SQL_INTERVAL_HOUR_TO_SECOND
 */
ZLV_DECLARE_SQL_TO_C( IntervalHourToSecond, IntervalDay );
ZLV_DECLARE_SQL_TO_C( IntervalHourToSecond, IntervalHour );
ZLV_DECLARE_SQL_TO_C( IntervalHourToSecond, IntervalMinute );
ZLV_DECLARE_SQL_TO_C( IntervalHourToSecond, IntervalSecond );
ZLV_DECLARE_SQL_TO_C( IntervalHourToSecond, IntervalDayToHour );
ZLV_DECLARE_SQL_TO_C( IntervalHourToSecond, IntervalDayToMinute );
ZLV_DECLARE_SQL_TO_C( IntervalHourToSecond, IntervalDayToSecond );
ZLV_DECLARE_SQL_TO_C( IntervalHourToSecond, IntervalHourToMinute );
ZLV_DECLARE_SQL_TO_C( IntervalHourToSecond, IntervalHourToSecond );
ZLV_DECLARE_SQL_TO_C( IntervalHourToSecond, IntervalMinuteToSecond );
ZLV_DECLARE_SQL_TO_C( IntervalHourToSecond, Binary );
ZLV_DECLARE_SQL_TO_C( IntervalHourToSecond, LongVarBinary );
ZLV_DECLARE_SQL_TO_C( IntervalHourToSecond, Char );
ZLV_DECLARE_SQL_TO_C( IntervalHourToSecond, Wchar );
ZLV_DECLARE_SQL_TO_C( IntervalHourToSecond, LongVarChar );

/*
 * SQL_INTERVAL_MINUTE_TO_SECOND
 */
ZLV_DECLARE_SQL_TO_C( IntervalMinuteToSecond, IntervalDay );
ZLV_DECLARE_SQL_TO_C( IntervalMinuteToSecond, IntervalHour );
ZLV_DECLARE_SQL_TO_C( IntervalMinuteToSecond, IntervalMinute );
ZLV_DECLARE_SQL_TO_C( IntervalMinuteToSecond, IntervalSecond );
ZLV_DECLARE_SQL_TO_C( IntervalMinuteToSecond, IntervalDayToHour );
ZLV_DECLARE_SQL_TO_C( IntervalMinuteToSecond, IntervalDayToMinute );
ZLV_DECLARE_SQL_TO_C( IntervalMinuteToSecond, IntervalDayToSecond );
ZLV_DECLARE_SQL_TO_C( IntervalMinuteToSecond, IntervalHourToMinute );
ZLV_DECLARE_SQL_TO_C( IntervalMinuteToSecond, IntervalHourToSecond );
ZLV_DECLARE_SQL_TO_C( IntervalMinuteToSecond, IntervalMinuteToSecond );
ZLV_DECLARE_SQL_TO_C( IntervalMinuteToSecond, Binary );
ZLV_DECLARE_SQL_TO_C( IntervalMinuteToSecond, LongVarBinary );
ZLV_DECLARE_SQL_TO_C( IntervalMinuteToSecond, Char );
ZLV_DECLARE_SQL_TO_C( IntervalMinuteToSecond, Wchar );
ZLV_DECLARE_SQL_TO_C( IntervalMinuteToSecond, LongVarChar );

/*
 * SQL_ROWID -> C
 */
ZLV_DECLARE_SQL_TO_C( RowId, Char );
ZLV_DECLARE_SQL_TO_C( RowId, Wchar );
ZLV_DECLARE_SQL_TO_C( RowId, LongVarChar );

/** @} */

#endif /* _ZLV_SQL_TO_C_H_ */
