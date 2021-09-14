/*******************************************************************************
 * zlvCToSql.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zlvCToSql.h 13004 2014-06-25 01:40:07Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZLV_C_TO_SQL_H_
#define _ZLV_C_TO_SQL_H_ 1

/**
 * @file zlvCToSql.h
 * @brief Gliese API Internal Convert Routines.
 */


/**
 * C To SQL , SQL_NUMERIC_STRUCT
 */
#define ZLV_C_TO_SQL_MAX_PRECISION (38)
#define ZLV_C_TO_SQL_SIGN_POS      (1)
#define ZLV_C_TO_SQL_SIGN_NEG      (0)
#define ZLV_C_TO_SQL_MAX_SCALE     (127)
#define ZLV_C_TO_SQL_MIN_SCALE     (-128)


#define ZLV_C_TO_SQL_TINYINT_MAX_SCALE  (37)
#define ZLV_C_TO_SQL_TINYINT_MIN_SCALE  (-2)
#define ZLV_C_TO_SQL_SMALLINT_MAX_SCALE (37)
#define ZLV_C_TO_SQL_SMALLINT_MIN_SCALE (-4)
#define ZLV_C_TO_SQL_INTEGER_MAX_SCALE  (37)
#define ZLV_C_TO_SQL_INTEGER_MIN_SCALE  (-9)
#define ZLV_C_TO_SQL_BIGINT_MAX_SCALE   (37)
#define ZLV_C_TO_SQL_BIGINT_MIN_SCALE   (-18)
#define ZLV_C_TO_SQL_INTERVAL_MAX_SCALE (37)
#define ZLV_C_TO_SQL_INTERVAL_MIN_SCALE (-5)

/**
 * @defgroup zlv Internal Convert Routines
 * @{
 */

#define ZLV_DECLARE_C_TO_SQL( aCType, aSQLType )                                        \
    stlStatus zlvC##aCType##ToSql##aSQLType ( zlsStmt        * aStmt,                   \
                                              void           * aParameterValuePtr,      \
                                              SQLLEN         * aIndicator,              \
                                              zldDescElement * aApdRec,                 \
                                              zldDescElement * aIpdRec,                 \
                                              dtlDataValue   * aDataValue,              \
                                              stlBool        * aSuccessWithInfo,        \
                                              stlErrorStack  * aErrorStack )

stlStatus zlvGetCToSqlFunc( zlsStmt       * aStmt,
                            stlInt16        aCType,
                            stlInt16        aSqlType,
                            zlvCtoSqlFunc * aFuncPtr,
                            stlErrorStack * aErrorStack );

stlStatus zlvInvalidCtoSql( zlsStmt        * aStmt,
                            void           * aParameterValuePtr,
                            SQLLEN         * aIndicator,
                            zldDescElement * aApdRec,
                            zldDescElement * aIpdRec,
                            dtlDataValue   * aDataValue,
                            stlBool        * aSuccessWithInfo,
                            stlErrorStack  * aErrorStack );

/*
 * SQL_C_CHAR -> SQL
 */
ZLV_DECLARE_C_TO_SQL( Char, Char );
ZLV_DECLARE_C_TO_SQL( Char, VarChar );
ZLV_DECLARE_C_TO_SQL( Char, LongVarChar );
ZLV_DECLARE_C_TO_SQL( Char, Numeric );
ZLV_DECLARE_C_TO_SQL( Char, TinyInt );
ZLV_DECLARE_C_TO_SQL( Char, SmallInt );
ZLV_DECLARE_C_TO_SQL( Char, Integer );
ZLV_DECLARE_C_TO_SQL( Char, BigInt );
ZLV_DECLARE_C_TO_SQL( Char, Real );
ZLV_DECLARE_C_TO_SQL( Char, Double );
ZLV_DECLARE_C_TO_SQL( Char, Float );
ZLV_DECLARE_C_TO_SQL( Char, Bit );
ZLV_DECLARE_C_TO_SQL( Char, Boolean );
ZLV_DECLARE_C_TO_SQL( Char, Binary );
ZLV_DECLARE_C_TO_SQL( Char, VarBinary );
ZLV_DECLARE_C_TO_SQL( Char, LongVarBinary );
ZLV_DECLARE_C_TO_SQL( Char, Date );
ZLV_DECLARE_C_TO_SQL( Char, Time );
ZLV_DECLARE_C_TO_SQL( Char, TimeTz );
ZLV_DECLARE_C_TO_SQL( Char, Timestamp );
ZLV_DECLARE_C_TO_SQL( Char, TimestampTz );
ZLV_DECLARE_C_TO_SQL( Char, IntervalMonth );
ZLV_DECLARE_C_TO_SQL( Char, IntervalYear );
ZLV_DECLARE_C_TO_SQL( Char, IntervalYearToMonth );
ZLV_DECLARE_C_TO_SQL( Char, IntervalDay );
ZLV_DECLARE_C_TO_SQL( Char, IntervalHour );
ZLV_DECLARE_C_TO_SQL( Char, IntervalMinute );
ZLV_DECLARE_C_TO_SQL( Char, IntervalSecond );
ZLV_DECLARE_C_TO_SQL( Char, IntervalDayToHour );
ZLV_DECLARE_C_TO_SQL( Char, IntervalDayToMinute );
ZLV_DECLARE_C_TO_SQL( Char, IntervalDayToSecond );
ZLV_DECLARE_C_TO_SQL( Char, IntervalHourToMinute );
ZLV_DECLARE_C_TO_SQL( Char, IntervalHourToSecond );
ZLV_DECLARE_C_TO_SQL( Char, IntervalMinuteToSecond );
ZLV_DECLARE_C_TO_SQL( Char, RowId );

/*
 * SQL_C_CHAR -> SQL
 */
ZLV_DECLARE_C_TO_SQL( Wchar, Char );
ZLV_DECLARE_C_TO_SQL( Wchar, VarChar );
ZLV_DECLARE_C_TO_SQL( Wchar, LongVarChar );
ZLV_DECLARE_C_TO_SQL( Wchar, Numeric );
ZLV_DECLARE_C_TO_SQL( Wchar, TinyInt );
ZLV_DECLARE_C_TO_SQL( Wchar, SmallInt );
ZLV_DECLARE_C_TO_SQL( Wchar, Integer );
ZLV_DECLARE_C_TO_SQL( Wchar, BigInt );
ZLV_DECLARE_C_TO_SQL( Wchar, Real );
ZLV_DECLARE_C_TO_SQL( Wchar, Double );
ZLV_DECLARE_C_TO_SQL( Wchar, Float );
ZLV_DECLARE_C_TO_SQL( Wchar, Bit );
ZLV_DECLARE_C_TO_SQL( Wchar, Boolean );
ZLV_DECLARE_C_TO_SQL( Wchar, Binary );
ZLV_DECLARE_C_TO_SQL( Wchar, VarBinary );
ZLV_DECLARE_C_TO_SQL( Wchar, LongVarBinary );
ZLV_DECLARE_C_TO_SQL( Wchar, Date );
ZLV_DECLARE_C_TO_SQL( Wchar, Time );
ZLV_DECLARE_C_TO_SQL( Wchar, TimeTz );
ZLV_DECLARE_C_TO_SQL( Wchar, Timestamp );
ZLV_DECLARE_C_TO_SQL( Wchar, TimestampTz );
ZLV_DECLARE_C_TO_SQL( Wchar, IntervalMonth );
ZLV_DECLARE_C_TO_SQL( Wchar, IntervalYear );
ZLV_DECLARE_C_TO_SQL( Wchar, IntervalYearToMonth );
ZLV_DECLARE_C_TO_SQL( Wchar, IntervalDay );
ZLV_DECLARE_C_TO_SQL( Wchar, IntervalHour );
ZLV_DECLARE_C_TO_SQL( Wchar, IntervalMinute );
ZLV_DECLARE_C_TO_SQL( Wchar, IntervalSecond );
ZLV_DECLARE_C_TO_SQL( Wchar, IntervalDayToHour );
ZLV_DECLARE_C_TO_SQL( Wchar, IntervalDayToMinute );
ZLV_DECLARE_C_TO_SQL( Wchar, IntervalDayToSecond );
ZLV_DECLARE_C_TO_SQL( Wchar, IntervalHourToMinute );
ZLV_DECLARE_C_TO_SQL( Wchar, IntervalHourToSecond );
ZLV_DECLARE_C_TO_SQL( Wchar, IntervalMinuteToSecond );
ZLV_DECLARE_C_TO_SQL( Wchar, RowId );

/*
 * SQL_C_LONGVARCHAR -> SQL
 */
ZLV_DECLARE_C_TO_SQL( LongVarChar, Char );
ZLV_DECLARE_C_TO_SQL( LongVarChar, VarChar );
ZLV_DECLARE_C_TO_SQL( LongVarChar, LongVarChar );
ZLV_DECLARE_C_TO_SQL( LongVarChar, Numeric );
ZLV_DECLARE_C_TO_SQL( LongVarChar, TinyInt );
ZLV_DECLARE_C_TO_SQL( LongVarChar, SmallInt );
ZLV_DECLARE_C_TO_SQL( LongVarChar, Integer );
ZLV_DECLARE_C_TO_SQL( LongVarChar, BigInt );
ZLV_DECLARE_C_TO_SQL( LongVarChar, Real );
ZLV_DECLARE_C_TO_SQL( LongVarChar, Double );
ZLV_DECLARE_C_TO_SQL( LongVarChar, Float );
ZLV_DECLARE_C_TO_SQL( LongVarChar, Bit );
ZLV_DECLARE_C_TO_SQL( LongVarChar, Boolean );
ZLV_DECLARE_C_TO_SQL( LongVarChar, Binary );
ZLV_DECLARE_C_TO_SQL( LongVarChar, VarBinary );
ZLV_DECLARE_C_TO_SQL( LongVarChar, LongVarBinary );
ZLV_DECLARE_C_TO_SQL( LongVarChar, Date );
ZLV_DECLARE_C_TO_SQL( LongVarChar, Time );
ZLV_DECLARE_C_TO_SQL( LongVarChar, TimeTz );
ZLV_DECLARE_C_TO_SQL( LongVarChar, Timestamp );
ZLV_DECLARE_C_TO_SQL( LongVarChar, TimestampTz );
ZLV_DECLARE_C_TO_SQL( LongVarChar, IntervalMonth );
ZLV_DECLARE_C_TO_SQL( LongVarChar, IntervalYear );
ZLV_DECLARE_C_TO_SQL( LongVarChar, IntervalYearToMonth );
ZLV_DECLARE_C_TO_SQL( LongVarChar, IntervalDay );
ZLV_DECLARE_C_TO_SQL( LongVarChar, IntervalHour );
ZLV_DECLARE_C_TO_SQL( LongVarChar, IntervalMinute );
ZLV_DECLARE_C_TO_SQL( LongVarChar, IntervalSecond );
ZLV_DECLARE_C_TO_SQL( LongVarChar, IntervalDayToHour );
ZLV_DECLARE_C_TO_SQL( LongVarChar, IntervalDayToMinute );
ZLV_DECLARE_C_TO_SQL( LongVarChar, IntervalDayToSecond );
ZLV_DECLARE_C_TO_SQL( LongVarChar, IntervalHourToMinute );
ZLV_DECLARE_C_TO_SQL( LongVarChar, IntervalHourToSecond );
ZLV_DECLARE_C_TO_SQL( LongVarChar, IntervalMinuteToSecond );
ZLV_DECLARE_C_TO_SQL( LongVarChar, RowId );

/*
 * SQL_C_WCHAR -> SQL
 */
ZLV_DECLARE_C_TO_SQL( WChar, Char );
ZLV_DECLARE_C_TO_SQL( WChar, VarChar );
ZLV_DECLARE_C_TO_SQL( WChar, LongVarChar );
ZLV_DECLARE_C_TO_SQL( WChar, Numeric );
ZLV_DECLARE_C_TO_SQL( WChar, TinyInt );
ZLV_DECLARE_C_TO_SQL( WChar, SmallInt );
ZLV_DECLARE_C_TO_SQL( WChar, Integer );
ZLV_DECLARE_C_TO_SQL( WChar, BigInt );
ZLV_DECLARE_C_TO_SQL( WChar, Real );
ZLV_DECLARE_C_TO_SQL( WChar, Double );
ZLV_DECLARE_C_TO_SQL( WChar, Float );
ZLV_DECLARE_C_TO_SQL( WChar, Bit );
ZLV_DECLARE_C_TO_SQL( WChar, Boolean );
ZLV_DECLARE_C_TO_SQL( WChar, Binary );
ZLV_DECLARE_C_TO_SQL( WChar, VarBinary );
ZLV_DECLARE_C_TO_SQL( WChar, LongVarBinary );
ZLV_DECLARE_C_TO_SQL( WChar, Date );
ZLV_DECLARE_C_TO_SQL( WChar, Time );
ZLV_DECLARE_C_TO_SQL( WChar, TimeTz );
ZLV_DECLARE_C_TO_SQL( WChar, Timestamp );
ZLV_DECLARE_C_TO_SQL( WChar, TimestampTz );
ZLV_DECLARE_C_TO_SQL( WChar, IntervalMonth );
ZLV_DECLARE_C_TO_SQL( WChar, IntervalYear );
ZLV_DECLARE_C_TO_SQL( WChar, IntervalYearToMonth );
ZLV_DECLARE_C_TO_SQL( WChar, IntervalDay );
ZLV_DECLARE_C_TO_SQL( WChar, IntervalHour );
ZLV_DECLARE_C_TO_SQL( WChar, IntervalMinute );
ZLV_DECLARE_C_TO_SQL( WChar, IntervalSecond );
ZLV_DECLARE_C_TO_SQL( WChar, IntervalDayToHour );
ZLV_DECLARE_C_TO_SQL( WChar, IntervalDayToMinute );
ZLV_DECLARE_C_TO_SQL( WChar, IntervalDayToSecond );
ZLV_DECLARE_C_TO_SQL( WChar, IntervalHourToMinute );
ZLV_DECLARE_C_TO_SQL( WChar, IntervalHourToSecond );
ZLV_DECLARE_C_TO_SQL( WChar, IntervalMinuteToSecond );

/*
 * SQL_C_BIT -> SQL
 */
ZLV_DECLARE_C_TO_SQL( Bit, Char );
ZLV_DECLARE_C_TO_SQL( Bit, VarChar );
ZLV_DECLARE_C_TO_SQL( Bit, LongVarChar );
ZLV_DECLARE_C_TO_SQL( Bit, Numeric );
ZLV_DECLARE_C_TO_SQL( Bit, TinyInt );
ZLV_DECLARE_C_TO_SQL( Bit, SmallInt );
ZLV_DECLARE_C_TO_SQL( Bit, Integer );
ZLV_DECLARE_C_TO_SQL( Bit, BigInt );
ZLV_DECLARE_C_TO_SQL( Bit, Real );
ZLV_DECLARE_C_TO_SQL( Bit, Double );
ZLV_DECLARE_C_TO_SQL( Bit, Float );
ZLV_DECLARE_C_TO_SQL( Bit, Bit );
ZLV_DECLARE_C_TO_SQL( Bit, Boolean );

/*
 * SQL_C_BOOLEAN -> SQL
 */
ZLV_DECLARE_C_TO_SQL( Boolean, Char );
ZLV_DECLARE_C_TO_SQL( Boolean, VarChar );
ZLV_DECLARE_C_TO_SQL( Boolean, LongVarChar );
ZLV_DECLARE_C_TO_SQL( Boolean, Numeric );
ZLV_DECLARE_C_TO_SQL( Boolean, TinyInt );
ZLV_DECLARE_C_TO_SQL( Boolean, SmallInt );
ZLV_DECLARE_C_TO_SQL( Boolean, Integer );
ZLV_DECLARE_C_TO_SQL( Boolean, BigInt );
ZLV_DECLARE_C_TO_SQL( Boolean, Real );
ZLV_DECLARE_C_TO_SQL( Boolean, Double );
ZLV_DECLARE_C_TO_SQL( Boolean, Float );
ZLV_DECLARE_C_TO_SQL( Boolean, Bit );
ZLV_DECLARE_C_TO_SQL( Boolean, Boolean );

/*
 * SQL_C_NUMERIC -> SQL
 */
ZLV_DECLARE_C_TO_SQL( Numeric, Char );
ZLV_DECLARE_C_TO_SQL( Numeric, VarChar );
ZLV_DECLARE_C_TO_SQL( Numeric, LongVarChar );
ZLV_DECLARE_C_TO_SQL( Numeric, Numeric );
ZLV_DECLARE_C_TO_SQL( Numeric, TinyInt );
ZLV_DECLARE_C_TO_SQL( Numeric, SmallInt );
ZLV_DECLARE_C_TO_SQL( Numeric, Integer );
ZLV_DECLARE_C_TO_SQL( Numeric, BigInt );
ZLV_DECLARE_C_TO_SQL( Numeric, Real );
ZLV_DECLARE_C_TO_SQL( Numeric, Double );
ZLV_DECLARE_C_TO_SQL( Numeric, Float );
ZLV_DECLARE_C_TO_SQL( Numeric, Bit );
ZLV_DECLARE_C_TO_SQL( Numeric, Boolean );
ZLV_DECLARE_C_TO_SQL( Numeric, IntervalYear );
ZLV_DECLARE_C_TO_SQL( Numeric, IntervalMonth );
ZLV_DECLARE_C_TO_SQL( Numeric, IntervalDay );
ZLV_DECLARE_C_TO_SQL( Numeric, IntervalHour );
ZLV_DECLARE_C_TO_SQL( Numeric, IntervalMinute );
ZLV_DECLARE_C_TO_SQL( Numeric, IntervalSecond );

/*
 * SQL_C_STINYINT -> SQL
 */
ZLV_DECLARE_C_TO_SQL( STinyInt, Char );
ZLV_DECLARE_C_TO_SQL( STinyInt, VarChar );
ZLV_DECLARE_C_TO_SQL( STinyInt, LongVarChar );
ZLV_DECLARE_C_TO_SQL( STinyInt, Numeric );
ZLV_DECLARE_C_TO_SQL( STinyInt, TinyInt );
ZLV_DECLARE_C_TO_SQL( STinyInt, SmallInt );
ZLV_DECLARE_C_TO_SQL( STinyInt, Integer );
ZLV_DECLARE_C_TO_SQL( STinyInt, BigInt );
ZLV_DECLARE_C_TO_SQL( STinyInt, Real );
ZLV_DECLARE_C_TO_SQL( STinyInt, Double );
ZLV_DECLARE_C_TO_SQL( STinyInt, Float );
ZLV_DECLARE_C_TO_SQL( STinyInt, Bit );
ZLV_DECLARE_C_TO_SQL( STinyInt, Boolean );
ZLV_DECLARE_C_TO_SQL( STinyInt, IntervalYear );
ZLV_DECLARE_C_TO_SQL( STinyInt, IntervalMonth );
ZLV_DECLARE_C_TO_SQL( STinyInt, IntervalDay );
ZLV_DECLARE_C_TO_SQL( STinyInt, IntervalHour );
ZLV_DECLARE_C_TO_SQL( STinyInt, IntervalMinute );
ZLV_DECLARE_C_TO_SQL( STinyInt, IntervalSecond );

/*
 * SQL_C_UTINYINT -> SQL
 */
ZLV_DECLARE_C_TO_SQL( UTinyInt, Char );
ZLV_DECLARE_C_TO_SQL( UTinyInt, VarChar );
ZLV_DECLARE_C_TO_SQL( UTinyInt, LongVarChar );
ZLV_DECLARE_C_TO_SQL( UTinyInt, Numeric );
ZLV_DECLARE_C_TO_SQL( UTinyInt, TinyInt );
ZLV_DECLARE_C_TO_SQL( UTinyInt, SmallInt );
ZLV_DECLARE_C_TO_SQL( UTinyInt, Integer );
ZLV_DECLARE_C_TO_SQL( UTinyInt, BigInt );
ZLV_DECLARE_C_TO_SQL( UTinyInt, Real );
ZLV_DECLARE_C_TO_SQL( UTinyInt, Double );
ZLV_DECLARE_C_TO_SQL( UTinyInt, Float );
ZLV_DECLARE_C_TO_SQL( UTinyInt, Bit );
ZLV_DECLARE_C_TO_SQL( UTinyInt, Boolean );
ZLV_DECLARE_C_TO_SQL( UTinyInt, IntervalYear );
ZLV_DECLARE_C_TO_SQL( UTinyInt, IntervalMonth );
ZLV_DECLARE_C_TO_SQL( UTinyInt, IntervalDay );
ZLV_DECLARE_C_TO_SQL( UTinyInt, IntervalHour );
ZLV_DECLARE_C_TO_SQL( UTinyInt, IntervalMinute );
ZLV_DECLARE_C_TO_SQL( UTinyInt, IntervalSecond );

/*
 * SQL_C_SBIGINT -> SQL
 */
ZLV_DECLARE_C_TO_SQL( SBigInt, Char );
ZLV_DECLARE_C_TO_SQL( SBigInt, VarChar );
ZLV_DECLARE_C_TO_SQL( SBigInt, LongVarChar );
ZLV_DECLARE_C_TO_SQL( SBigInt, Numeric );
ZLV_DECLARE_C_TO_SQL( SBigInt, TinyInt );
ZLV_DECLARE_C_TO_SQL( SBigInt, SmallInt );
ZLV_DECLARE_C_TO_SQL( SBigInt, Integer );
ZLV_DECLARE_C_TO_SQL( SBigInt, BigInt );
ZLV_DECLARE_C_TO_SQL( SBigInt, Real );
ZLV_DECLARE_C_TO_SQL( SBigInt, Double );
ZLV_DECLARE_C_TO_SQL( SBigInt, Float );
ZLV_DECLARE_C_TO_SQL( SBigInt, Bit );
ZLV_DECLARE_C_TO_SQL( SBigInt, Boolean );
ZLV_DECLARE_C_TO_SQL( SBigInt, IntervalYear );
ZLV_DECLARE_C_TO_SQL( SBigInt, IntervalMonth );
ZLV_DECLARE_C_TO_SQL( SBigInt, IntervalDay );
ZLV_DECLARE_C_TO_SQL( SBigInt, IntervalHour );
ZLV_DECLARE_C_TO_SQL( SBigInt, IntervalMinute );
ZLV_DECLARE_C_TO_SQL( SBigInt, IntervalSecond );

/*
 * SQL_C_UBIGINT -> SQL
 */
ZLV_DECLARE_C_TO_SQL( UBigInt, Char );
ZLV_DECLARE_C_TO_SQL( UBigInt, VarChar );
ZLV_DECLARE_C_TO_SQL( UBigInt, LongVarChar );
ZLV_DECLARE_C_TO_SQL( UBigInt, Numeric );
ZLV_DECLARE_C_TO_SQL( UBigInt, TinyInt );
ZLV_DECLARE_C_TO_SQL( UBigInt, SmallInt );
ZLV_DECLARE_C_TO_SQL( UBigInt, Integer );
ZLV_DECLARE_C_TO_SQL( UBigInt, BigInt );
ZLV_DECLARE_C_TO_SQL( UBigInt, Real );
ZLV_DECLARE_C_TO_SQL( UBigInt, Double );
ZLV_DECLARE_C_TO_SQL( UBigInt, Float );
ZLV_DECLARE_C_TO_SQL( UBigInt, Bit );
ZLV_DECLARE_C_TO_SQL( UBigInt, Boolean );
ZLV_DECLARE_C_TO_SQL( UBigInt, IntervalYear );
ZLV_DECLARE_C_TO_SQL( UBigInt, IntervalMonth );
ZLV_DECLARE_C_TO_SQL( UBigInt, IntervalDay );
ZLV_DECLARE_C_TO_SQL( UBigInt, IntervalHour );
ZLV_DECLARE_C_TO_SQL( UBigInt, IntervalMinute );
ZLV_DECLARE_C_TO_SQL( UBigInt, IntervalSecond );

/*
 * SQL_C_SSHORT -> SQL
 */
ZLV_DECLARE_C_TO_SQL( SShort, Char );
ZLV_DECLARE_C_TO_SQL( SShort, VarChar );
ZLV_DECLARE_C_TO_SQL( SShort, LongVarChar );
ZLV_DECLARE_C_TO_SQL( SShort, Numeric );
ZLV_DECLARE_C_TO_SQL( SShort, TinyInt );
ZLV_DECLARE_C_TO_SQL( SShort, SmallInt );
ZLV_DECLARE_C_TO_SQL( SShort, Integer );
ZLV_DECLARE_C_TO_SQL( SShort, BigInt );
ZLV_DECLARE_C_TO_SQL( SShort, Real );
ZLV_DECLARE_C_TO_SQL( SShort, Double );
ZLV_DECLARE_C_TO_SQL( SShort, Float );
ZLV_DECLARE_C_TO_SQL( SShort, Bit );
ZLV_DECLARE_C_TO_SQL( SShort, Boolean );
ZLV_DECLARE_C_TO_SQL( SShort, IntervalYear );
ZLV_DECLARE_C_TO_SQL( SShort, IntervalMonth );
ZLV_DECLARE_C_TO_SQL( SShort, IntervalDay );
ZLV_DECLARE_C_TO_SQL( SShort, IntervalHour );
ZLV_DECLARE_C_TO_SQL( SShort, IntervalMinute );
ZLV_DECLARE_C_TO_SQL( SShort, IntervalSecond );

/*
 * SQL_C_USHORT -> SQL
 */
ZLV_DECLARE_C_TO_SQL( UShort, Char );
ZLV_DECLARE_C_TO_SQL( UShort, VarChar );
ZLV_DECLARE_C_TO_SQL( UShort, LongVarChar );
ZLV_DECLARE_C_TO_SQL( UShort, Numeric );
ZLV_DECLARE_C_TO_SQL( UShort, TinyInt );
ZLV_DECLARE_C_TO_SQL( UShort, SmallInt );
ZLV_DECLARE_C_TO_SQL( UShort, Integer );
ZLV_DECLARE_C_TO_SQL( UShort, BigInt );
ZLV_DECLARE_C_TO_SQL( UShort, Real );
ZLV_DECLARE_C_TO_SQL( UShort, Double );
ZLV_DECLARE_C_TO_SQL( UShort, Float );
ZLV_DECLARE_C_TO_SQL( UShort, Bit );
ZLV_DECLARE_C_TO_SQL( UShort, Boolean );
ZLV_DECLARE_C_TO_SQL( UShort, IntervalYear );
ZLV_DECLARE_C_TO_SQL( UShort, IntervalMonth );
ZLV_DECLARE_C_TO_SQL( UShort, IntervalDay );
ZLV_DECLARE_C_TO_SQL( UShort, IntervalHour );
ZLV_DECLARE_C_TO_SQL( UShort, IntervalMinute );
ZLV_DECLARE_C_TO_SQL( UShort, IntervalSecond );

/*
 * SQL_C_SLONG -> SQL
 */
ZLV_DECLARE_C_TO_SQL( SLong, Char );
ZLV_DECLARE_C_TO_SQL( SLong, VarChar );
ZLV_DECLARE_C_TO_SQL( SLong, LongVarChar );
ZLV_DECLARE_C_TO_SQL( SLong, Numeric );
ZLV_DECLARE_C_TO_SQL( SLong, TinyInt );
ZLV_DECLARE_C_TO_SQL( SLong, SmallInt );
ZLV_DECLARE_C_TO_SQL( SLong, Integer );
ZLV_DECLARE_C_TO_SQL( SLong, BigInt );
ZLV_DECLARE_C_TO_SQL( SLong, Real );
ZLV_DECLARE_C_TO_SQL( SLong, Double );
ZLV_DECLARE_C_TO_SQL( SLong, Float );
ZLV_DECLARE_C_TO_SQL( SLong, Bit );
ZLV_DECLARE_C_TO_SQL( SLong, Boolean );
ZLV_DECLARE_C_TO_SQL( SLong, IntervalYear );
ZLV_DECLARE_C_TO_SQL( SLong, IntervalMonth );
ZLV_DECLARE_C_TO_SQL( SLong, IntervalDay );
ZLV_DECLARE_C_TO_SQL( SLong, IntervalHour );
ZLV_DECLARE_C_TO_SQL( SLong, IntervalMinute );
ZLV_DECLARE_C_TO_SQL( SLong, IntervalSecond );

/*
 * SQL_C_ULONG -> SQL
 */
ZLV_DECLARE_C_TO_SQL( ULong, Char );
ZLV_DECLARE_C_TO_SQL( ULong, VarChar );
ZLV_DECLARE_C_TO_SQL( ULong, LongVarChar );
ZLV_DECLARE_C_TO_SQL( ULong, Numeric );
ZLV_DECLARE_C_TO_SQL( ULong, TinyInt );
ZLV_DECLARE_C_TO_SQL( ULong, SmallInt );
ZLV_DECLARE_C_TO_SQL( ULong, Integer );
ZLV_DECLARE_C_TO_SQL( ULong, BigInt );
ZLV_DECLARE_C_TO_SQL( ULong, Real );
ZLV_DECLARE_C_TO_SQL( ULong, Double );
ZLV_DECLARE_C_TO_SQL( ULong, Float );
ZLV_DECLARE_C_TO_SQL( ULong, Bit );
ZLV_DECLARE_C_TO_SQL( ULong, Boolean );
ZLV_DECLARE_C_TO_SQL( ULong, IntervalYear );
ZLV_DECLARE_C_TO_SQL( ULong, IntervalMonth );
ZLV_DECLARE_C_TO_SQL( ULong, IntervalDay );
ZLV_DECLARE_C_TO_SQL( ULong, IntervalHour );
ZLV_DECLARE_C_TO_SQL( ULong, IntervalMinute );
ZLV_DECLARE_C_TO_SQL( ULong, IntervalSecond );

/*
 * SQL_C_FLOAT -> SQL
 */
ZLV_DECLARE_C_TO_SQL( Float, Char );
ZLV_DECLARE_C_TO_SQL( Float, VarChar );
ZLV_DECLARE_C_TO_SQL( Float, LongVarChar );
ZLV_DECLARE_C_TO_SQL( Float, Numeric );
ZLV_DECLARE_C_TO_SQL( Float, TinyInt );
ZLV_DECLARE_C_TO_SQL( Float, SmallInt );
ZLV_DECLARE_C_TO_SQL( Float, Integer );
ZLV_DECLARE_C_TO_SQL( Float, BigInt );
ZLV_DECLARE_C_TO_SQL( Float, Real );
ZLV_DECLARE_C_TO_SQL( Float, Double );
ZLV_DECLARE_C_TO_SQL( Float, Float );
ZLV_DECLARE_C_TO_SQL( Float, Bit );
ZLV_DECLARE_C_TO_SQL( Float, Boolean );

/*
 * SQL_C_DOUBLE -> SQL
 */
ZLV_DECLARE_C_TO_SQL( Double, Char );
ZLV_DECLARE_C_TO_SQL( Double, VarChar );
ZLV_DECLARE_C_TO_SQL( Double, LongVarChar );
ZLV_DECLARE_C_TO_SQL( Double, Numeric );
ZLV_DECLARE_C_TO_SQL( Double, TinyInt );
ZLV_DECLARE_C_TO_SQL( Double, SmallInt );
ZLV_DECLARE_C_TO_SQL( Double, Integer );
ZLV_DECLARE_C_TO_SQL( Double, BigInt );
ZLV_DECLARE_C_TO_SQL( Double, Real );
ZLV_DECLARE_C_TO_SQL( Double, Double );
ZLV_DECLARE_C_TO_SQL( Double, Float );
ZLV_DECLARE_C_TO_SQL( Double, Bit );
ZLV_DECLARE_C_TO_SQL( Double, Boolean );

/*
 * SQL_C_BINARY -> SQL
 */
ZLV_DECLARE_C_TO_SQL( Binary, Char );
ZLV_DECLARE_C_TO_SQL( Binary, VarChar );
ZLV_DECLARE_C_TO_SQL( Binary, LongVarChar );
ZLV_DECLARE_C_TO_SQL( Binary, Numeric );
ZLV_DECLARE_C_TO_SQL( Binary, TinyInt );
ZLV_DECLARE_C_TO_SQL( Binary, SmallInt );
ZLV_DECLARE_C_TO_SQL( Binary, Integer );
ZLV_DECLARE_C_TO_SQL( Binary, BigInt );
ZLV_DECLARE_C_TO_SQL( Binary, Real );
ZLV_DECLARE_C_TO_SQL( Binary, Double );
ZLV_DECLARE_C_TO_SQL( Binary, Float );
ZLV_DECLARE_C_TO_SQL( Binary, Bit );
ZLV_DECLARE_C_TO_SQL( Binary, Boolean );
ZLV_DECLARE_C_TO_SQL( Binary, Binary );
ZLV_DECLARE_C_TO_SQL( Binary, VarBinary );
ZLV_DECLARE_C_TO_SQL( Binary, LongVarBinary );
ZLV_DECLARE_C_TO_SQL( Binary, Date );
ZLV_DECLARE_C_TO_SQL( Binary, Time );
ZLV_DECLARE_C_TO_SQL( Binary, TimeTz );
ZLV_DECLARE_C_TO_SQL( Binary, Timestamp );
ZLV_DECLARE_C_TO_SQL( Binary, TimestampTz );

/*
 * SQL_C_LONGVARBINARY -> SQL
 */
ZLV_DECLARE_C_TO_SQL( LongVarBinary, Char );
ZLV_DECLARE_C_TO_SQL( LongVarBinary, VarChar );
ZLV_DECLARE_C_TO_SQL( LongVarBinary, LongVarChar );
ZLV_DECLARE_C_TO_SQL( LongVarBinary, Numeric );
ZLV_DECLARE_C_TO_SQL( LongVarBinary, TinyInt );
ZLV_DECLARE_C_TO_SQL( LongVarBinary, SmallInt );
ZLV_DECLARE_C_TO_SQL( LongVarBinary, Integer );
ZLV_DECLARE_C_TO_SQL( LongVarBinary, BigInt );
ZLV_DECLARE_C_TO_SQL( LongVarBinary, Real );
ZLV_DECLARE_C_TO_SQL( LongVarBinary, Double );
ZLV_DECLARE_C_TO_SQL( LongVarBinary, Float );
ZLV_DECLARE_C_TO_SQL( LongVarBinary, Bit );
ZLV_DECLARE_C_TO_SQL( LongVarBinary, Boolean );
ZLV_DECLARE_C_TO_SQL( LongVarBinary, Binary );
ZLV_DECLARE_C_TO_SQL( LongVarBinary, VarBinary );
ZLV_DECLARE_C_TO_SQL( LongVarBinary, LongVarBinary );
ZLV_DECLARE_C_TO_SQL( LongVarBinary, Date );
ZLV_DECLARE_C_TO_SQL( LongVarBinary, Time );
ZLV_DECLARE_C_TO_SQL( LongVarBinary, TimeTz );
ZLV_DECLARE_C_TO_SQL( LongVarBinary, Timestamp );
ZLV_DECLARE_C_TO_SQL( LongVarBinary, TimestampTz );

/*
 * SQL_C_DATE -> SQL
 */
ZLV_DECLARE_C_TO_SQL( Date, Char );
ZLV_DECLARE_C_TO_SQL( Date, VarChar );
ZLV_DECLARE_C_TO_SQL( Date, LongVarChar );
ZLV_DECLARE_C_TO_SQL( Date, Date );
ZLV_DECLARE_C_TO_SQL( Date, Timestamp );
ZLV_DECLARE_C_TO_SQL( Date, TimestampTz );

/*
 * SQL_C_TYPE_TIME -> SQL
 */
ZLV_DECLARE_C_TO_SQL( Time, Char );
ZLV_DECLARE_C_TO_SQL( Time, VarChar );
ZLV_DECLARE_C_TO_SQL( Time, LongVarChar );
ZLV_DECLARE_C_TO_SQL( Time, Time );
ZLV_DECLARE_C_TO_SQL( Time, TimeTz );
ZLV_DECLARE_C_TO_SQL( Time, Timestamp );
ZLV_DECLARE_C_TO_SQL( Time, TimestampTz );

/*
 * SQL_C_TYPE_TIME_TZ -> SQL
 */
ZLV_DECLARE_C_TO_SQL( TimeTz, Char );
ZLV_DECLARE_C_TO_SQL( TimeTz, VarChar );
ZLV_DECLARE_C_TO_SQL( TimeTz, LongVarChar );
ZLV_DECLARE_C_TO_SQL( TimeTz, Time );
ZLV_DECLARE_C_TO_SQL( TimeTz, TimeTz );
ZLV_DECLARE_C_TO_SQL( TimeTz, Timestamp );
ZLV_DECLARE_C_TO_SQL( TimeTz, TimestampTz );

/*
 * SQL_C_TYPE_TIMESTAMP -> SQL
 */
ZLV_DECLARE_C_TO_SQL( Timestamp, Char );
ZLV_DECLARE_C_TO_SQL( Timestamp, VarChar );
ZLV_DECLARE_C_TO_SQL( Timestamp, LongVarChar );
ZLV_DECLARE_C_TO_SQL( Timestamp, Date );
ZLV_DECLARE_C_TO_SQL( Timestamp, Time );
ZLV_DECLARE_C_TO_SQL( Timestamp, TimeTz );
ZLV_DECLARE_C_TO_SQL( Timestamp, Timestamp );
ZLV_DECLARE_C_TO_SQL( Timestamp, TimestampTz );

/*
 * SQL_C_TYPE_TIMESTAMP_TZ -> SQL
 */
ZLV_DECLARE_C_TO_SQL( TimestampTz, Char );
ZLV_DECLARE_C_TO_SQL( TimestampTz, VarChar );
ZLV_DECLARE_C_TO_SQL( TimestampTz, LongVarChar );
ZLV_DECLARE_C_TO_SQL( TimestampTz, Date );
ZLV_DECLARE_C_TO_SQL( TimestampTz, Time );
ZLV_DECLARE_C_TO_SQL( TimestampTz, TimeTz );
ZLV_DECLARE_C_TO_SQL( TimestampTz, Timestamp );
ZLV_DECLARE_C_TO_SQL( TimestampTz, TimestampTz );

/*
 * SQL_C_INTERVAL_MONTH -> SQL
 */
ZLV_DECLARE_C_TO_SQL( IntervalMonth, Char );
ZLV_DECLARE_C_TO_SQL( IntervalMonth, VarChar );
ZLV_DECLARE_C_TO_SQL( IntervalMonth, LongVarChar );
ZLV_DECLARE_C_TO_SQL( IntervalMonth, Numeric );
ZLV_DECLARE_C_TO_SQL( IntervalMonth, TinyInt );
ZLV_DECLARE_C_TO_SQL( IntervalMonth, SmallInt );
ZLV_DECLARE_C_TO_SQL( IntervalMonth, Integer );
ZLV_DECLARE_C_TO_SQL( IntervalMonth, BigInt );
ZLV_DECLARE_C_TO_SQL( IntervalMonth, IntervalMonth );
ZLV_DECLARE_C_TO_SQL( IntervalMonth, IntervalYear );
ZLV_DECLARE_C_TO_SQL( IntervalMonth, IntervalYearToMonth );

/*
 * SQL_C_INTERVAL_YEAR -> SQL
 */
ZLV_DECLARE_C_TO_SQL( IntervalYear, Char );
ZLV_DECLARE_C_TO_SQL( IntervalYear, VarChar );
ZLV_DECLARE_C_TO_SQL( IntervalYear, LongVarChar );
ZLV_DECLARE_C_TO_SQL( IntervalYear, Numeric );
ZLV_DECLARE_C_TO_SQL( IntervalYear, TinyInt );
ZLV_DECLARE_C_TO_SQL( IntervalYear, SmallInt );
ZLV_DECLARE_C_TO_SQL( IntervalYear, Integer );
ZLV_DECLARE_C_TO_SQL( IntervalYear, BigInt );
ZLV_DECLARE_C_TO_SQL( IntervalYear, IntervalMonth );
ZLV_DECLARE_C_TO_SQL( IntervalYear, IntervalYear );
ZLV_DECLARE_C_TO_SQL( IntervalYear, IntervalYearToMonth );

/*
 * SQL_C_INTERVAL_YEAR_TO_MONTH -> SQL
 */
ZLV_DECLARE_C_TO_SQL( IntervalYearToMonth, Char );
ZLV_DECLARE_C_TO_SQL( IntervalYearToMonth, VarChar );
ZLV_DECLARE_C_TO_SQL( IntervalYearToMonth, LongVarChar );
ZLV_DECLARE_C_TO_SQL( IntervalYearToMonth, IntervalMonth );
ZLV_DECLARE_C_TO_SQL( IntervalYearToMonth, IntervalYear );
ZLV_DECLARE_C_TO_SQL( IntervalYearToMonth, IntervalYearToMonth );

/*
 * SQL_C_INTERVAL_DAY -> SQL
 */
ZLV_DECLARE_C_TO_SQL( IntervalDay, Char );
ZLV_DECLARE_C_TO_SQL( IntervalDay, VarChar );
ZLV_DECLARE_C_TO_SQL( IntervalDay, LongVarChar );
ZLV_DECLARE_C_TO_SQL( IntervalDay, Numeric );
ZLV_DECLARE_C_TO_SQL( IntervalDay, TinyInt );
ZLV_DECLARE_C_TO_SQL( IntervalDay, SmallInt );
ZLV_DECLARE_C_TO_SQL( IntervalDay, Integer );
ZLV_DECLARE_C_TO_SQL( IntervalDay, BigInt );
ZLV_DECLARE_C_TO_SQL( IntervalDay, IntervalDay );
ZLV_DECLARE_C_TO_SQL( IntervalDay, IntervalHour );
ZLV_DECLARE_C_TO_SQL( IntervalDay, IntervalMinute );
ZLV_DECLARE_C_TO_SQL( IntervalDay, IntervalSecond );
ZLV_DECLARE_C_TO_SQL( IntervalDay, IntervalDayToHour );
ZLV_DECLARE_C_TO_SQL( IntervalDay, IntervalDayToMinute );
ZLV_DECLARE_C_TO_SQL( IntervalDay, IntervalDayToSecond );
ZLV_DECLARE_C_TO_SQL( IntervalDay, IntervalHourToMinute );
ZLV_DECLARE_C_TO_SQL( IntervalDay, IntervalHourToSecond );
ZLV_DECLARE_C_TO_SQL( IntervalDay, IntervalMinuteToSecond );

/*
 * SQL_C_INTERVAL_HOUR -> SQL
 */
ZLV_DECLARE_C_TO_SQL( IntervalHour, Char );
ZLV_DECLARE_C_TO_SQL( IntervalHour, VarChar );
ZLV_DECLARE_C_TO_SQL( IntervalHour, LongVarChar );
ZLV_DECLARE_C_TO_SQL( IntervalHour, Numeric );
ZLV_DECLARE_C_TO_SQL( IntervalHour, TinyInt );
ZLV_DECLARE_C_TO_SQL( IntervalHour, SmallInt );
ZLV_DECLARE_C_TO_SQL( IntervalHour, Integer );
ZLV_DECLARE_C_TO_SQL( IntervalHour, BigInt );
ZLV_DECLARE_C_TO_SQL( IntervalHour, IntervalDay );
ZLV_DECLARE_C_TO_SQL( IntervalHour, IntervalHour );
ZLV_DECLARE_C_TO_SQL( IntervalHour, IntervalMinute );
ZLV_DECLARE_C_TO_SQL( IntervalHour, IntervalSecond );
ZLV_DECLARE_C_TO_SQL( IntervalHour, IntervalDayToHour );
ZLV_DECLARE_C_TO_SQL( IntervalHour, IntervalDayToMinute );
ZLV_DECLARE_C_TO_SQL( IntervalHour, IntervalDayToSecond );
ZLV_DECLARE_C_TO_SQL( IntervalHour, IntervalHourToMinute );
ZLV_DECLARE_C_TO_SQL( IntervalHour, IntervalHourToSecond );
ZLV_DECLARE_C_TO_SQL( IntervalHour, IntervalMinuteToSecond );

/*
 * SQL_C_INTERVAL_MINUTE -> SQL
 */
ZLV_DECLARE_C_TO_SQL( IntervalMinute, Char );
ZLV_DECLARE_C_TO_SQL( IntervalMinute, VarChar );
ZLV_DECLARE_C_TO_SQL( IntervalMinute, LongVarChar );
ZLV_DECLARE_C_TO_SQL( IntervalMinute, Numeric );
ZLV_DECLARE_C_TO_SQL( IntervalMinute, TinyInt );
ZLV_DECLARE_C_TO_SQL( IntervalMinute, SmallInt );
ZLV_DECLARE_C_TO_SQL( IntervalMinute, Integer );
ZLV_DECLARE_C_TO_SQL( IntervalMinute, BigInt );
ZLV_DECLARE_C_TO_SQL( IntervalMinute, IntervalDay );
ZLV_DECLARE_C_TO_SQL( IntervalMinute, IntervalHour );
ZLV_DECLARE_C_TO_SQL( IntervalMinute, IntervalMinute );
ZLV_DECLARE_C_TO_SQL( IntervalMinute, IntervalSecond );
ZLV_DECLARE_C_TO_SQL( IntervalMinute, IntervalDayToHour );
ZLV_DECLARE_C_TO_SQL( IntervalMinute, IntervalDayToMinute );
ZLV_DECLARE_C_TO_SQL( IntervalMinute, IntervalDayToSecond );
ZLV_DECLARE_C_TO_SQL( IntervalMinute, IntervalHourToMinute );
ZLV_DECLARE_C_TO_SQL( IntervalMinute, IntervalHourToSecond );
ZLV_DECLARE_C_TO_SQL( IntervalMinute, IntervalMinuteToSecond );

/*
 * SQL_C_INTERVAL_SECOND -> SQL
 */
ZLV_DECLARE_C_TO_SQL( IntervalSecond, Char );
ZLV_DECLARE_C_TO_SQL( IntervalSecond, VarChar );
ZLV_DECLARE_C_TO_SQL( IntervalSecond, LongVarChar );
ZLV_DECLARE_C_TO_SQL( IntervalSecond, Numeric );
ZLV_DECLARE_C_TO_SQL( IntervalSecond, TinyInt );
ZLV_DECLARE_C_TO_SQL( IntervalSecond, SmallInt );
ZLV_DECLARE_C_TO_SQL( IntervalSecond, Integer );
ZLV_DECLARE_C_TO_SQL( IntervalSecond, BigInt );
ZLV_DECLARE_C_TO_SQL( IntervalSecond, IntervalDay );
ZLV_DECLARE_C_TO_SQL( IntervalSecond, IntervalHour );
ZLV_DECLARE_C_TO_SQL( IntervalSecond, IntervalMinute );
ZLV_DECLARE_C_TO_SQL( IntervalSecond, IntervalSecond );
ZLV_DECLARE_C_TO_SQL( IntervalSecond, IntervalDayToHour );
ZLV_DECLARE_C_TO_SQL( IntervalSecond, IntervalDayToMinute );
ZLV_DECLARE_C_TO_SQL( IntervalSecond, IntervalDayToSecond );
ZLV_DECLARE_C_TO_SQL( IntervalSecond, IntervalHourToMinute );
ZLV_DECLARE_C_TO_SQL( IntervalSecond, IntervalHourToSecond );
ZLV_DECLARE_C_TO_SQL( IntervalSecond, IntervalMinuteToSecond );

/*
 * SQL_C_INTERVAL_DAY_TO_HOUR -> SQL
 */
ZLV_DECLARE_C_TO_SQL( IntervalDayToHour, Char );
ZLV_DECLARE_C_TO_SQL( IntervalDayToHour, VarChar );
ZLV_DECLARE_C_TO_SQL( IntervalDayToHour, LongVarChar );
ZLV_DECLARE_C_TO_SQL( IntervalDayToHour, IntervalDay );
ZLV_DECLARE_C_TO_SQL( IntervalDayToHour, IntervalHour );
ZLV_DECLARE_C_TO_SQL( IntervalDayToHour, IntervalMinute );
ZLV_DECLARE_C_TO_SQL( IntervalDayToHour, IntervalSecond );
ZLV_DECLARE_C_TO_SQL( IntervalDayToHour, IntervalDayToHour );
ZLV_DECLARE_C_TO_SQL( IntervalDayToHour, IntervalDayToMinute );
ZLV_DECLARE_C_TO_SQL( IntervalDayToHour, IntervalDayToSecond );
ZLV_DECLARE_C_TO_SQL( IntervalDayToHour, IntervalHourToMinute );
ZLV_DECLARE_C_TO_SQL( IntervalDayToHour, IntervalHourToSecond );
ZLV_DECLARE_C_TO_SQL( IntervalDayToHour, IntervalMinuteToSecond );

/*
 * SQL_C_INTERVAL_DAY_TO_MINUTE -> SQL
 */
ZLV_DECLARE_C_TO_SQL( IntervalDayToMinute, Char );
ZLV_DECLARE_C_TO_SQL( IntervalDayToMinute, VarChar );
ZLV_DECLARE_C_TO_SQL( IntervalDayToMinute, LongVarChar );
ZLV_DECLARE_C_TO_SQL( IntervalDayToMinute, IntervalDay );
ZLV_DECLARE_C_TO_SQL( IntervalDayToMinute, IntervalHour );
ZLV_DECLARE_C_TO_SQL( IntervalDayToMinute, IntervalMinute );
ZLV_DECLARE_C_TO_SQL( IntervalDayToMinute, IntervalSecond );
ZLV_DECLARE_C_TO_SQL( IntervalDayToMinute, IntervalDayToHour );
ZLV_DECLARE_C_TO_SQL( IntervalDayToMinute, IntervalDayToMinute );
ZLV_DECLARE_C_TO_SQL( IntervalDayToMinute, IntervalDayToSecond );
ZLV_DECLARE_C_TO_SQL( IntervalDayToMinute, IntervalHourToMinute );
ZLV_DECLARE_C_TO_SQL( IntervalDayToMinute, IntervalHourToSecond );
ZLV_DECLARE_C_TO_SQL( IntervalDayToMinute, IntervalMinuteToSecond );

/*
 * SQL_C_INTERVAL_DAY_TO_SECOND -> SQL
 */
ZLV_DECLARE_C_TO_SQL( IntervalDayToSecond, Char );
ZLV_DECLARE_C_TO_SQL( IntervalDayToSecond, VarChar );
ZLV_DECLARE_C_TO_SQL( IntervalDayToSecond, LongVarChar );
ZLV_DECLARE_C_TO_SQL( IntervalDayToSecond, IntervalDay );
ZLV_DECLARE_C_TO_SQL( IntervalDayToSecond, IntervalHour );
ZLV_DECLARE_C_TO_SQL( IntervalDayToSecond, IntervalMinute );
ZLV_DECLARE_C_TO_SQL( IntervalDayToSecond, IntervalSecond );
ZLV_DECLARE_C_TO_SQL( IntervalDayToSecond, IntervalDayToHour );
ZLV_DECLARE_C_TO_SQL( IntervalDayToSecond, IntervalDayToMinute );
ZLV_DECLARE_C_TO_SQL( IntervalDayToSecond, IntervalDayToSecond );
ZLV_DECLARE_C_TO_SQL( IntervalDayToSecond, IntervalHourToMinute );
ZLV_DECLARE_C_TO_SQL( IntervalDayToSecond, IntervalHourToSecond );
ZLV_DECLARE_C_TO_SQL( IntervalDayToSecond, IntervalMinuteToSecond );

/*
 * SQL_C_INTERVAL_HOUR_TO_MINUTE -> SQL
 */
ZLV_DECLARE_C_TO_SQL( IntervalHourToMinute, Char );
ZLV_DECLARE_C_TO_SQL( IntervalHourToMinute, VarChar );
ZLV_DECLARE_C_TO_SQL( IntervalHourToMinute, LongVarChar );
ZLV_DECLARE_C_TO_SQL( IntervalHourToMinute, IntervalDay );
ZLV_DECLARE_C_TO_SQL( IntervalHourToMinute, IntervalHour );
ZLV_DECLARE_C_TO_SQL( IntervalHourToMinute, IntervalMinute );
ZLV_DECLARE_C_TO_SQL( IntervalHourToMinute, IntervalSecond );
ZLV_DECLARE_C_TO_SQL( IntervalHourToMinute, IntervalDayToHour );
ZLV_DECLARE_C_TO_SQL( IntervalHourToMinute, IntervalDayToMinute );
ZLV_DECLARE_C_TO_SQL( IntervalHourToMinute, IntervalDayToSecond );
ZLV_DECLARE_C_TO_SQL( IntervalHourToMinute, IntervalHourToMinute );
ZLV_DECLARE_C_TO_SQL( IntervalHourToMinute, IntervalHourToSecond );
ZLV_DECLARE_C_TO_SQL( IntervalHourToMinute, IntervalMinuteToSecond );

/*
 * SQL_C_INTERVAL_HOUR_TO_SECOND -> SQL
 */
ZLV_DECLARE_C_TO_SQL( IntervalHourToSecond, Char );
ZLV_DECLARE_C_TO_SQL( IntervalHourToSecond, VarChar );
ZLV_DECLARE_C_TO_SQL( IntervalHourToSecond, LongVarChar );
ZLV_DECLARE_C_TO_SQL( IntervalHourToSecond, IntervalDay );
ZLV_DECLARE_C_TO_SQL( IntervalHourToSecond, IntervalHour );
ZLV_DECLARE_C_TO_SQL( IntervalHourToSecond, IntervalMinute );
ZLV_DECLARE_C_TO_SQL( IntervalHourToSecond, IntervalSecond );
ZLV_DECLARE_C_TO_SQL( IntervalHourToSecond, IntervalDayToHour );
ZLV_DECLARE_C_TO_SQL( IntervalHourToSecond, IntervalDayToMinute );
ZLV_DECLARE_C_TO_SQL( IntervalHourToSecond, IntervalDayToSecond );
ZLV_DECLARE_C_TO_SQL( IntervalHourToSecond, IntervalHourToMinute );
ZLV_DECLARE_C_TO_SQL( IntervalHourToSecond, IntervalHourToSecond );
ZLV_DECLARE_C_TO_SQL( IntervalHourToSecond, IntervalMinuteToSecond );

/*
 * SQL_C_INTERVAL_MINUTE_TO_SECOND -> SQL
 */
ZLV_DECLARE_C_TO_SQL( IntervalMinuteToSecond, Char );
ZLV_DECLARE_C_TO_SQL( IntervalMinuteToSecond, VarChar );
ZLV_DECLARE_C_TO_SQL( IntervalMinuteToSecond, LongVarChar );
ZLV_DECLARE_C_TO_SQL( IntervalMinuteToSecond, IntervalDay );
ZLV_DECLARE_C_TO_SQL( IntervalMinuteToSecond, IntervalHour );
ZLV_DECLARE_C_TO_SQL( IntervalMinuteToSecond, IntervalMinute );
ZLV_DECLARE_C_TO_SQL( IntervalMinuteToSecond, IntervalSecond );
ZLV_DECLARE_C_TO_SQL( IntervalMinuteToSecond, IntervalDayToHour );
ZLV_DECLARE_C_TO_SQL( IntervalMinuteToSecond, IntervalDayToMinute );
ZLV_DECLARE_C_TO_SQL( IntervalMinuteToSecond, IntervalDayToSecond );
ZLV_DECLARE_C_TO_SQL( IntervalMinuteToSecond, IntervalHourToMinute );
ZLV_DECLARE_C_TO_SQL( IntervalMinuteToSecond, IntervalHourToSecond );
ZLV_DECLARE_C_TO_SQL( IntervalMinuteToSecond, IntervalMinuteToSecond );

/** @} */

#endif /* _ZLV_C_TO_SQL_H_ */
