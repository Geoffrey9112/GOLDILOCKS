/*******************************************************************************
 * zlvCToSql.c
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
#include <zlvCToSql.h>

/**
 * @file zlvCToSql.c
 * @brief Gliese API Internal Converting Data from C to SQL Data Types Routines.
 */

/**
 * @addtogroup zlvCToSql
 * @{
 */

stlStatus zlvGetCToSqlFunc( zlsStmt       * aStmt,
                            stlInt16        aCType,
                            stlInt16        aSqlType,
                            zlvCtoSqlFunc * aFuncPtr,
                            stlErrorStack * aErrorStack )
{
    switch( aCType )
    {
        case SQL_C_CHAR :
            switch( aSqlType )
            {
                case SQL_CHAR :
                    *aFuncPtr = zlvCCharToSqlChar;
                    break;
                case SQL_VARCHAR :
                    *aFuncPtr = zlvCCharToSqlVarChar;
                    break;
                case SQL_LONGVARCHAR :
                    *aFuncPtr = zlvCCharToSqlLongVarChar;
                    break;
                case SQL_NUMERIC :
                    *aFuncPtr = zlvCCharToSqlNumeric;
                    break;
                case SQL_TINYINT :
                    *aFuncPtr = zlvCCharToSqlTinyInt;
                    break;
                case SQL_SMALLINT :
                    *aFuncPtr = zlvCCharToSqlSmallInt;
                    break;
                case SQL_INTEGER :
                    *aFuncPtr = zlvCCharToSqlInteger;
                    break;
                case SQL_BIGINT :
                    *aFuncPtr = zlvCCharToSqlBigInt;
                    break;
                case SQL_REAL :
                    *aFuncPtr = zlvCCharToSqlReal;
                    break;
                case SQL_DOUBLE :
                    *aFuncPtr = zlvCCharToSqlDouble;
                    break;
                case SQL_FLOAT :
                    *aFuncPtr = zlvCCharToSqlFloat;
                    break;
                case SQL_BIT :
                    *aFuncPtr = zlvCCharToSqlBit;
                    break;
                case SQL_BOOLEAN :
                    *aFuncPtr = zlvCCharToSqlBoolean;
                    break;
                case SQL_BINARY :
                    *aFuncPtr = zlvCCharToSqlBinary;
                    break;
                case SQL_VARBINARY :
                    *aFuncPtr = zlvCCharToSqlVarBinary;
                    break;
                case SQL_LONGVARBINARY :
                    *aFuncPtr = zlvCCharToSqlLongVarBinary;
                    break;
                case SQL_TYPE_DATE :
                    *aFuncPtr = zlvCCharToSqlDate;
                    break;
                case SQL_TYPE_TIME :
                    *aFuncPtr = zlvCCharToSqlTime;
                    break;
                case SQL_TYPE_TIME_WITH_TIMEZONE :
                    *aFuncPtr = zlvCCharToSqlTimeTz;
                    break;
                case SQL_TYPE_TIMESTAMP :
                    *aFuncPtr = zlvCCharToSqlTimestamp;
                    break;
                case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE :
                    *aFuncPtr = zlvCCharToSqlTimestampTz;
                    break;
                case SQL_INTERVAL_MONTH :
                    *aFuncPtr = zlvCCharToSqlIntervalMonth;
                    break;
                case SQL_INTERVAL_YEAR :
                    *aFuncPtr = zlvCCharToSqlIntervalYear;
                    break;
                case SQL_INTERVAL_YEAR_TO_MONTH :
                    *aFuncPtr = zlvCCharToSqlIntervalYearToMonth;
                    break;
                case SQL_INTERVAL_DAY :
                    *aFuncPtr = zlvCCharToSqlIntervalDay;
                    break;
                case SQL_INTERVAL_HOUR :
                    *aFuncPtr = zlvCCharToSqlIntervalHour;
                    break;
                case SQL_INTERVAL_MINUTE :
                    *aFuncPtr = zlvCCharToSqlIntervalMinute;
                    break;
                case SQL_INTERVAL_SECOND :
                    *aFuncPtr = zlvCCharToSqlIntervalSecond;
                    break;
                case SQL_INTERVAL_DAY_TO_HOUR :
                    *aFuncPtr = zlvCCharToSqlIntervalDayToHour;
                    break;
                case SQL_INTERVAL_DAY_TO_MINUTE :
                    *aFuncPtr = zlvCCharToSqlIntervalDayToMinute;
                    break;
                case SQL_INTERVAL_DAY_TO_SECOND :
                    *aFuncPtr = zlvCCharToSqlIntervalDayToSecond;
                    break;
                case SQL_INTERVAL_HOUR_TO_MINUTE :
                    *aFuncPtr = zlvCCharToSqlIntervalHourToMinute;
                    break;
                case SQL_INTERVAL_HOUR_TO_SECOND :
                    *aFuncPtr = zlvCCharToSqlIntervalHourToSecond;
                    break;
                case SQL_INTERVAL_MINUTE_TO_SECOND :
                    *aFuncPtr = zlvCCharToSqlIntervalMinuteToSecond;
                    break;
                case SQL_ROWID :
                    *aFuncPtr = zlvCCharToSqlRowId;
                    break;                    
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case SQL_C_WCHAR :
            switch( aSqlType )
            {
                case SQL_CHAR :
                    *aFuncPtr = zlvCWcharToSqlChar;
                    break;
                case SQL_VARCHAR :
                    *aFuncPtr = zlvCWcharToSqlVarChar;
                    break;
                case SQL_LONGVARCHAR :
                    *aFuncPtr = zlvCWcharToSqlLongVarChar;
                    break;
                case SQL_NUMERIC :
                    *aFuncPtr = zlvCWcharToSqlNumeric;
                    break;
                case SQL_TINYINT :
                    *aFuncPtr = zlvCWcharToSqlTinyInt;
                    break;
                case SQL_SMALLINT :
                    *aFuncPtr = zlvCWcharToSqlSmallInt;
                    break;
                case SQL_INTEGER :
                    *aFuncPtr = zlvCWcharToSqlInteger;
                    break;
                case SQL_BIGINT :
                    *aFuncPtr = zlvCWcharToSqlBigInt;
                    break;
                case SQL_REAL :
                    *aFuncPtr = zlvCWcharToSqlReal;
                    break;
                case SQL_DOUBLE :
                    *aFuncPtr = zlvCWcharToSqlDouble;
                    break;
                case SQL_FLOAT :
                    *aFuncPtr = zlvCWcharToSqlFloat;
                    break;
                case SQL_BIT :
                    *aFuncPtr = zlvCWcharToSqlBit;
                    break;
                case SQL_BOOLEAN :
                    *aFuncPtr = zlvCWcharToSqlBoolean;
                    break;
                case SQL_BINARY :
                    *aFuncPtr = zlvCWcharToSqlBinary;
                    break;
                case SQL_VARBINARY :
                    *aFuncPtr = zlvCWcharToSqlVarBinary;
                    break;
                case SQL_LONGVARBINARY :
                    *aFuncPtr = zlvCWcharToSqlLongVarBinary;
                    break;
                case SQL_TYPE_DATE :
                    *aFuncPtr = zlvCWcharToSqlDate;
                    break;
                case SQL_TYPE_TIME :
                    *aFuncPtr = zlvCWcharToSqlTime;
                    break;
                case SQL_TYPE_TIME_WITH_TIMEZONE :
                    *aFuncPtr = zlvCWcharToSqlTimeTz;
                    break;
                case SQL_TYPE_TIMESTAMP :
                    *aFuncPtr = zlvCWcharToSqlTimestamp;
                    break;
                case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE :
                    *aFuncPtr = zlvCWcharToSqlTimestampTz;
                    break;
                case SQL_INTERVAL_MONTH :
                    *aFuncPtr = zlvCWcharToSqlIntervalMonth;
                    break;
                case SQL_INTERVAL_YEAR :
                    *aFuncPtr = zlvCWcharToSqlIntervalYear;
                    break;
                case SQL_INTERVAL_YEAR_TO_MONTH :
                    *aFuncPtr = zlvCWcharToSqlIntervalYearToMonth;
                    break;
                case SQL_INTERVAL_DAY :
                    *aFuncPtr = zlvCWcharToSqlIntervalDay;
                    break;
                case SQL_INTERVAL_HOUR :
                    *aFuncPtr = zlvCWcharToSqlIntervalHour;
                    break;
                case SQL_INTERVAL_MINUTE :
                    *aFuncPtr = zlvCWcharToSqlIntervalMinute;
                    break;
                case SQL_INTERVAL_SECOND :
                    *aFuncPtr = zlvCWcharToSqlIntervalSecond;
                    break;
                case SQL_INTERVAL_DAY_TO_HOUR :
                    *aFuncPtr = zlvCWcharToSqlIntervalDayToHour;
                    break;
                case SQL_INTERVAL_DAY_TO_MINUTE :
                    *aFuncPtr = zlvCWcharToSqlIntervalDayToMinute;
                    break;
                case SQL_INTERVAL_DAY_TO_SECOND :
                    *aFuncPtr = zlvCWcharToSqlIntervalDayToSecond;
                    break;
                case SQL_INTERVAL_HOUR_TO_MINUTE :
                    *aFuncPtr = zlvCWcharToSqlIntervalHourToMinute;
                    break;
                case SQL_INTERVAL_HOUR_TO_SECOND :
                    *aFuncPtr = zlvCWcharToSqlIntervalHourToSecond;
                    break;
                case SQL_INTERVAL_MINUTE_TO_SECOND :
                    *aFuncPtr = zlvCWcharToSqlIntervalMinuteToSecond;
                    break;
                case SQL_ROWID :
                    *aFuncPtr = zlvCWcharToSqlRowId;
                    break;                    
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case SQL_C_LONGVARCHAR :
            switch( aSqlType )
            {
                case SQL_CHAR :
                    *aFuncPtr = zlvCLongVarCharToSqlChar;
                    break;
                case SQL_VARCHAR :
                    *aFuncPtr = zlvCLongVarCharToSqlVarChar;
                    break;
                case SQL_LONGVARCHAR :
                    *aFuncPtr = zlvCLongVarCharToSqlLongVarChar;
                    break;
                case SQL_NUMERIC :
                    *aFuncPtr = zlvCLongVarCharToSqlNumeric;
                    break;
                case SQL_TINYINT :
                    *aFuncPtr = zlvCLongVarCharToSqlTinyInt;
                    break;
                case SQL_SMALLINT :
                    *aFuncPtr = zlvCLongVarCharToSqlSmallInt;
                    break;
                case SQL_INTEGER :
                    *aFuncPtr = zlvCLongVarCharToSqlInteger;
                    break;
                case SQL_BIGINT :
                    *aFuncPtr = zlvCLongVarCharToSqlBigInt;
                    break;
                case SQL_REAL :
                    *aFuncPtr = zlvCLongVarCharToSqlReal;
                    break;
                case SQL_DOUBLE :
                    *aFuncPtr = zlvCLongVarCharToSqlDouble;
                    break;
                case SQL_FLOAT :
                    *aFuncPtr = zlvCLongVarCharToSqlFloat;
                    break;
                case SQL_BIT :
                    *aFuncPtr = zlvCLongVarCharToSqlBit;
                    break;
                case SQL_BOOLEAN :
                    *aFuncPtr = zlvCLongVarCharToSqlBoolean;
                    break;
                case SQL_BINARY :
                    *aFuncPtr = zlvCLongVarCharToSqlBinary;
                    break;
                case SQL_VARBINARY :
                    *aFuncPtr = zlvCLongVarCharToSqlVarBinary;
                    break;
                case SQL_LONGVARBINARY :
                    *aFuncPtr = zlvCLongVarCharToSqlLongVarBinary;
                    break;
                case SQL_TYPE_DATE :
                    *aFuncPtr = zlvCLongVarCharToSqlDate;
                    break;
                case SQL_TYPE_TIME :
                    *aFuncPtr = zlvCLongVarCharToSqlTime;
                    break;
                case SQL_TYPE_TIME_WITH_TIMEZONE :
                    *aFuncPtr = zlvCLongVarCharToSqlTimeTz;
                    break;
                case SQL_TYPE_TIMESTAMP :
                    *aFuncPtr = zlvCLongVarCharToSqlTimestamp;
                    break;
                case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE :
                    *aFuncPtr = zlvCLongVarCharToSqlTimestampTz;
                    break;
                case SQL_INTERVAL_MONTH :
                    *aFuncPtr = zlvCLongVarCharToSqlIntervalMonth;
                    break;
                case SQL_INTERVAL_YEAR :
                    *aFuncPtr = zlvCLongVarCharToSqlIntervalYear;
                    break;
                case SQL_INTERVAL_YEAR_TO_MONTH :
                    *aFuncPtr = zlvCLongVarCharToSqlIntervalYearToMonth;
                    break;
                case SQL_INTERVAL_DAY :
                    *aFuncPtr = zlvCLongVarCharToSqlIntervalDay;
                    break;
                case SQL_INTERVAL_HOUR :
                    *aFuncPtr = zlvCLongVarCharToSqlIntervalHour;
                    break;
                case SQL_INTERVAL_MINUTE :
                    *aFuncPtr = zlvCLongVarCharToSqlIntervalMinute;
                    break;
                case SQL_INTERVAL_SECOND :
                    *aFuncPtr = zlvCLongVarCharToSqlIntervalSecond;
                    break;
                case SQL_INTERVAL_DAY_TO_HOUR :
                    *aFuncPtr = zlvCLongVarCharToSqlIntervalDayToHour;
                    break;
                case SQL_INTERVAL_DAY_TO_MINUTE :
                    *aFuncPtr = zlvCLongVarCharToSqlIntervalDayToMinute;
                    break;
                case SQL_INTERVAL_DAY_TO_SECOND :
                    *aFuncPtr = zlvCLongVarCharToSqlIntervalDayToSecond;
                    break;
                case SQL_INTERVAL_HOUR_TO_MINUTE :
                    *aFuncPtr = zlvCLongVarCharToSqlIntervalHourToMinute;
                    break;
                case SQL_INTERVAL_HOUR_TO_SECOND :
                    *aFuncPtr = zlvCLongVarCharToSqlIntervalHourToSecond;
                    break;
                case SQL_INTERVAL_MINUTE_TO_SECOND :
                    *aFuncPtr = zlvCLongVarCharToSqlIntervalMinuteToSecond;
                    break;
                case SQL_ROWID :
                    *aFuncPtr = zlvCLongVarCharToSqlRowId;
                    break;                    
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case SQL_C_NUMERIC :
            switch( aSqlType )
            {
                case SQL_CHAR :
                    *aFuncPtr = zlvCNumericToSqlChar;
                    break;
                case SQL_VARCHAR :
                    *aFuncPtr = zlvCNumericToSqlVarChar;
                    break;
                case SQL_LONGVARCHAR :
                    *aFuncPtr = zlvCNumericToSqlLongVarChar;
                    break;
                case SQL_NUMERIC :
                    *aFuncPtr = zlvCNumericToSqlNumeric;
                    break;
                case SQL_TINYINT :
                    *aFuncPtr = zlvCNumericToSqlTinyInt;
                    break;
                case SQL_SMALLINT :
                    *aFuncPtr = zlvCNumericToSqlSmallInt;
                    break;
                case SQL_INTEGER :
                    *aFuncPtr = zlvCNumericToSqlInteger;
                    break;
                case SQL_BIGINT :
                    *aFuncPtr = zlvCNumericToSqlBigInt;
                    break;
                case SQL_REAL :
                    *aFuncPtr = zlvCNumericToSqlReal;
                    break;
                case SQL_DOUBLE :
                    *aFuncPtr = zlvCNumericToSqlDouble;
                    break;
                case SQL_FLOAT :
                    *aFuncPtr = zlvCNumericToSqlFloat;
                    break;
                case SQL_BIT :
                    *aFuncPtr = zlvCNumericToSqlBit;
                    break;
                case SQL_BOOLEAN :
                    *aFuncPtr = zlvCNumericToSqlBoolean;
                    break;
                case SQL_INTERVAL_YEAR :
                    *aFuncPtr = zlvCNumericToSqlIntervalYear;
                    break;
                case SQL_INTERVAL_MONTH :
                    *aFuncPtr = zlvCNumericToSqlIntervalMonth;
                    break;
                case SQL_INTERVAL_DAY :
                    *aFuncPtr = zlvCNumericToSqlIntervalDay;
                    break;
                case SQL_INTERVAL_HOUR :
                    *aFuncPtr = zlvCNumericToSqlIntervalHour;
                    break;
                case SQL_INTERVAL_MINUTE :
                    *aFuncPtr = zlvCNumericToSqlIntervalMinute;
                    break;
                case SQL_INTERVAL_SECOND :
                    *aFuncPtr = zlvCNumericToSqlIntervalSecond;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case SQL_C_STINYINT :
        case SQL_C_TINYINT :
            switch( aSqlType )
            {
                case SQL_CHAR :
                    *aFuncPtr = zlvCSTinyIntToSqlChar;
                    break;
                case SQL_VARCHAR :
                    *aFuncPtr = zlvCSTinyIntToSqlVarChar;
                    break;
                case SQL_LONGVARCHAR :
                    *aFuncPtr = zlvCSTinyIntToSqlLongVarChar;
                    break;
                case SQL_NUMERIC :
                    *aFuncPtr = zlvCSTinyIntToSqlNumeric;
                    break;
                case SQL_TINYINT :
                    *aFuncPtr = zlvCSTinyIntToSqlTinyInt;
                    break;
                case SQL_SMALLINT :
                    *aFuncPtr = zlvCSTinyIntToSqlSmallInt;
                    break;
                case SQL_INTEGER :
                    *aFuncPtr = zlvCSTinyIntToSqlInteger;
                    break;
                case SQL_BIGINT :
                    *aFuncPtr = zlvCSTinyIntToSqlBigInt;
                    break;
                case SQL_REAL :
                    *aFuncPtr = zlvCSTinyIntToSqlReal;
                    break;
                case SQL_DOUBLE :
                    *aFuncPtr = zlvCSTinyIntToSqlDouble;
                    break;
                case SQL_FLOAT :
                    *aFuncPtr = zlvCSTinyIntToSqlFloat;
                    break;
                case SQL_BIT :
                    *aFuncPtr = zlvCSTinyIntToSqlBit;
                    break;
                case SQL_BOOLEAN :
                    *aFuncPtr = zlvCSTinyIntToSqlBoolean;
                    break;
                case SQL_INTERVAL_YEAR :
                    *aFuncPtr = zlvCSTinyIntToSqlIntervalYear;
                    break;
                case SQL_INTERVAL_MONTH :
                    *aFuncPtr = zlvCSTinyIntToSqlIntervalMonth;
                    break;
                case SQL_INTERVAL_DAY :
                    *aFuncPtr = zlvCSTinyIntToSqlIntervalDay;
                    break;
                case SQL_INTERVAL_HOUR :
                    *aFuncPtr = zlvCSTinyIntToSqlIntervalHour;
                    break;
                case SQL_INTERVAL_MINUTE :
                    *aFuncPtr = zlvCSTinyIntToSqlIntervalMinute;
                    break;
                case SQL_INTERVAL_SECOND :
                    *aFuncPtr = zlvCSTinyIntToSqlIntervalSecond;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case SQL_C_UTINYINT :
            switch( aSqlType )
            {
                case SQL_CHAR :
                    *aFuncPtr = zlvCUTinyIntToSqlChar;
                    break;
                case SQL_VARCHAR :
                    *aFuncPtr = zlvCUTinyIntToSqlVarChar;
                    break;
                case SQL_LONGVARCHAR :
                    *aFuncPtr = zlvCUTinyIntToSqlLongVarChar;
                    break;
                case SQL_NUMERIC :
                    *aFuncPtr = zlvCUTinyIntToSqlNumeric;
                    break;
                case SQL_TINYINT :
                    *aFuncPtr = zlvCUTinyIntToSqlTinyInt;
                    break;
                case SQL_SMALLINT :
                    *aFuncPtr = zlvCUTinyIntToSqlSmallInt;
                    break;
                case SQL_INTEGER :
                    *aFuncPtr = zlvCUTinyIntToSqlInteger;
                    break;
                case SQL_BIGINT :
                    *aFuncPtr = zlvCUTinyIntToSqlBigInt;
                    break;
                case SQL_REAL :
                    *aFuncPtr = zlvCUTinyIntToSqlReal;
                    break;
                case SQL_DOUBLE :
                    *aFuncPtr = zlvCUTinyIntToSqlDouble;
                    break;
                case SQL_FLOAT :
                    *aFuncPtr = zlvCUTinyIntToSqlFloat;
                    break;
                case SQL_BIT :
                    *aFuncPtr = zlvCUTinyIntToSqlBit;
                    break;
                case SQL_BOOLEAN :
                    *aFuncPtr = zlvCUTinyIntToSqlBoolean;
                    break;
                case SQL_INTERVAL_YEAR :
                    *aFuncPtr = zlvCUTinyIntToSqlIntervalYear;
                    break;
                case SQL_INTERVAL_MONTH :
                    *aFuncPtr = zlvCUTinyIntToSqlIntervalMonth;
                    break;
                case SQL_INTERVAL_DAY :
                    *aFuncPtr = zlvCUTinyIntToSqlIntervalDay;
                    break;
                case SQL_INTERVAL_HOUR :
                    *aFuncPtr = zlvCUTinyIntToSqlIntervalHour;
                    break;
                case SQL_INTERVAL_MINUTE :
                    *aFuncPtr = zlvCUTinyIntToSqlIntervalMinute;
                    break;
                case SQL_INTERVAL_SECOND :
                    *aFuncPtr = zlvCUTinyIntToSqlIntervalSecond;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case SQL_C_SBIGINT :
            switch( aSqlType )
            {
                case SQL_CHAR :
                    *aFuncPtr = zlvCSBigIntToSqlChar;
                    break;
                case SQL_VARCHAR :
                    *aFuncPtr = zlvCSBigIntToSqlVarChar;
                    break;
                case SQL_LONGVARCHAR :
                    *aFuncPtr = zlvCSBigIntToSqlLongVarChar;
                    break;
                case SQL_NUMERIC :
                    *aFuncPtr = zlvCSBigIntToSqlNumeric;
                    break;
                case SQL_TINYINT :
                    *aFuncPtr = zlvCSBigIntToSqlTinyInt;
                    break;
                case SQL_SMALLINT :
                    *aFuncPtr = zlvCSBigIntToSqlSmallInt;
                    break;
                case SQL_INTEGER :
                    *aFuncPtr = zlvCSBigIntToSqlInteger;
                    break;
                case SQL_BIGINT :
                    *aFuncPtr = zlvCSBigIntToSqlBigInt;
                    break;
                case SQL_REAL :
                    *aFuncPtr = zlvCSBigIntToSqlReal;
                    break;
                case SQL_DOUBLE :
                    *aFuncPtr = zlvCSBigIntToSqlDouble;
                    break;
                case SQL_FLOAT :
                    *aFuncPtr = zlvCSBigIntToSqlFloat;
                    break;
                case SQL_BIT :
                    *aFuncPtr = zlvCSBigIntToSqlBit;
                    break;
                case SQL_BOOLEAN :
                    *aFuncPtr = zlvCSBigIntToSqlBoolean;
                    break;
                case SQL_INTERVAL_YEAR :
                    *aFuncPtr = zlvCSBigIntToSqlIntervalYear;
                    break;
                case SQL_INTERVAL_MONTH :
                    *aFuncPtr = zlvCSBigIntToSqlIntervalMonth;
                    break;
                case SQL_INTERVAL_DAY :
                    *aFuncPtr = zlvCSBigIntToSqlIntervalDay;
                    break;
                case SQL_INTERVAL_HOUR :
                    *aFuncPtr = zlvCSBigIntToSqlIntervalHour;
                    break;
                case SQL_INTERVAL_MINUTE :
                    *aFuncPtr = zlvCSBigIntToSqlIntervalMinute;
                    break;
                case SQL_INTERVAL_SECOND :
                    *aFuncPtr = zlvCSBigIntToSqlIntervalSecond;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case SQL_C_UBIGINT :
            switch( aSqlType )
            {
                case SQL_CHAR :
                    *aFuncPtr = zlvCUBigIntToSqlChar;
                    break;
                case SQL_VARCHAR :
                    *aFuncPtr = zlvCUBigIntToSqlVarChar;
                    break;
                case SQL_LONGVARCHAR :
                    *aFuncPtr = zlvCUBigIntToSqlLongVarChar;
                    break;
                case SQL_NUMERIC :
                    *aFuncPtr = zlvCUBigIntToSqlNumeric;
                    break;
                case SQL_TINYINT :
                    *aFuncPtr = zlvCUBigIntToSqlTinyInt;
                    break;
                case SQL_SMALLINT :
                    *aFuncPtr = zlvCUBigIntToSqlSmallInt;
                    break;
                case SQL_INTEGER :
                    *aFuncPtr = zlvCUBigIntToSqlInteger;
                    break;
                case SQL_BIGINT :
                    *aFuncPtr = zlvCUBigIntToSqlBigInt;
                    break;
                case SQL_REAL :
                    *aFuncPtr = zlvCUBigIntToSqlReal;
                    break;
                case SQL_DOUBLE :
                    *aFuncPtr = zlvCUBigIntToSqlDouble;
                    break;
                case SQL_FLOAT :
                    *aFuncPtr = zlvCUBigIntToSqlFloat;
                    break;
                case SQL_BIT :
                    *aFuncPtr = zlvCUBigIntToSqlBit;
                    break;
                case SQL_BOOLEAN :
                    *aFuncPtr = zlvCUBigIntToSqlBoolean;
                    break;
                case SQL_INTERVAL_YEAR :
                    *aFuncPtr = zlvCUBigIntToSqlIntervalYear;
                    break;
                case SQL_INTERVAL_MONTH :
                    *aFuncPtr = zlvCUBigIntToSqlIntervalMonth;
                    break;
                case SQL_INTERVAL_DAY :
                    *aFuncPtr = zlvCUBigIntToSqlIntervalDay;
                    break;
                case SQL_INTERVAL_HOUR :
                    *aFuncPtr = zlvCUBigIntToSqlIntervalHour;
                    break;
                case SQL_INTERVAL_MINUTE :
                    *aFuncPtr = zlvCUBigIntToSqlIntervalMinute;
                    break;
                case SQL_INTERVAL_SECOND :
                    *aFuncPtr = zlvCUBigIntToSqlIntervalSecond;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case SQL_C_SSHORT :
        case SQL_C_SHORT :
            switch( aSqlType )
            {
                case SQL_CHAR :
                    *aFuncPtr = zlvCSShortToSqlChar;
                    break;
                case SQL_VARCHAR :
                    *aFuncPtr = zlvCSShortToSqlVarChar;
                    break;
                case SQL_LONGVARCHAR :
                    *aFuncPtr = zlvCSShortToSqlLongVarChar;
                    break;
                case SQL_NUMERIC :
                    *aFuncPtr = zlvCSShortToSqlNumeric;
                    break;
                case SQL_TINYINT :
                    *aFuncPtr = zlvCSShortToSqlTinyInt;
                    break;
                case SQL_SMALLINT :
                    *aFuncPtr = zlvCSShortToSqlSmallInt;
                    break;
                case SQL_INTEGER :
                    *aFuncPtr = zlvCSShortToSqlInteger;
                    break;
                case SQL_BIGINT :
                    *aFuncPtr = zlvCSShortToSqlBigInt;
                    break;
                case SQL_REAL :
                    *aFuncPtr = zlvCSShortToSqlReal;
                    break;
                case SQL_DOUBLE :
                    *aFuncPtr = zlvCSShortToSqlDouble;
                    break;
                case SQL_FLOAT :
                    *aFuncPtr = zlvCSShortToSqlFloat;
                    break;
                case SQL_BIT :
                    *aFuncPtr = zlvCSShortToSqlBit;
                    break;
                case SQL_BOOLEAN :
                    *aFuncPtr = zlvCSShortToSqlBoolean;
                    break;
                case SQL_INTERVAL_YEAR :
                    *aFuncPtr = zlvCSShortToSqlIntervalYear;
                    break;
                case SQL_INTERVAL_MONTH :
                    *aFuncPtr = zlvCSShortToSqlIntervalMonth;
                    break;
                case SQL_INTERVAL_DAY :
                    *aFuncPtr = zlvCSShortToSqlIntervalDay;
                    break;
                case SQL_INTERVAL_HOUR :
                    *aFuncPtr = zlvCSShortToSqlIntervalHour;
                    break;
                case SQL_INTERVAL_MINUTE :
                    *aFuncPtr = zlvCSShortToSqlIntervalMinute;
                    break;
                case SQL_INTERVAL_SECOND :
                    *aFuncPtr = zlvCSShortToSqlIntervalSecond;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case SQL_C_USHORT :
            switch( aSqlType )
            {
                case SQL_CHAR :
                    *aFuncPtr = zlvCUShortToSqlChar;
                    break;
                case SQL_VARCHAR :
                    *aFuncPtr = zlvCUShortToSqlVarChar;
                    break;
                case SQL_LONGVARCHAR :
                    *aFuncPtr = zlvCUShortToSqlLongVarChar;
                    break;
                case SQL_NUMERIC :
                    *aFuncPtr = zlvCUShortToSqlNumeric;
                    break;
                case SQL_TINYINT :
                    *aFuncPtr = zlvCUShortToSqlTinyInt;
                    break;
                case SQL_SMALLINT :
                    *aFuncPtr = zlvCUShortToSqlSmallInt;
                    break;
                case SQL_INTEGER :
                    *aFuncPtr = zlvCUShortToSqlInteger;
                    break;
                case SQL_BIGINT :
                    *aFuncPtr = zlvCUShortToSqlBigInt;
                    break;
                case SQL_REAL :
                    *aFuncPtr = zlvCUShortToSqlReal;
                    break;
                case SQL_DOUBLE :
                    *aFuncPtr = zlvCUShortToSqlDouble;
                    break;
                case SQL_FLOAT :
                    *aFuncPtr = zlvCUShortToSqlFloat;
                    break;
                case SQL_BIT :
                    *aFuncPtr = zlvCUShortToSqlBit;
                    break;
                case SQL_BOOLEAN :
                    *aFuncPtr = zlvCUShortToSqlBoolean;
                    break;
                case SQL_INTERVAL_YEAR :
                    *aFuncPtr = zlvCUShortToSqlIntervalYear;
                    break;
                case SQL_INTERVAL_MONTH :
                    *aFuncPtr = zlvCUShortToSqlIntervalMonth;
                    break;
                case SQL_INTERVAL_DAY :
                    *aFuncPtr = zlvCUShortToSqlIntervalDay;
                    break;
                case SQL_INTERVAL_HOUR :
                    *aFuncPtr = zlvCUShortToSqlIntervalHour;
                    break;
                case SQL_INTERVAL_MINUTE :
                    *aFuncPtr = zlvCUShortToSqlIntervalMinute;
                    break;
                case SQL_INTERVAL_SECOND :
                    *aFuncPtr = zlvCUShortToSqlIntervalSecond;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case SQL_C_SLONG :
        case SQL_C_LONG :
            switch( aSqlType )
            {
                case SQL_CHAR :
                    *aFuncPtr = zlvCSLongToSqlChar;
                    break;
                case SQL_VARCHAR :
                    *aFuncPtr = zlvCSLongToSqlVarChar;
                    break;
                case SQL_LONGVARCHAR :
                    *aFuncPtr = zlvCSLongToSqlLongVarChar;
                    break;
                case SQL_NUMERIC :
                    *aFuncPtr = zlvCSLongToSqlNumeric;
                    break;
                case SQL_TINYINT :
                    *aFuncPtr = zlvCSLongToSqlTinyInt;
                    break;
                case SQL_SMALLINT :
                    *aFuncPtr = zlvCSLongToSqlSmallInt;
                    break;
                case SQL_INTEGER :
                    *aFuncPtr = zlvCSLongToSqlInteger;
                    break;
                case SQL_BIGINT :
                    *aFuncPtr = zlvCSLongToSqlBigInt;
                    break;
                case SQL_REAL :
                    *aFuncPtr = zlvCSLongToSqlReal;
                    break;
                case SQL_DOUBLE :
                    *aFuncPtr = zlvCSLongToSqlDouble;
                    break;
                case SQL_FLOAT :
                    *aFuncPtr = zlvCSLongToSqlFloat;
                    break;
                case SQL_BIT :
                    *aFuncPtr = zlvCSLongToSqlBit;
                    break;
                case SQL_BOOLEAN :
                    *aFuncPtr = zlvCSLongToSqlBoolean;
                    break;
                case SQL_INTERVAL_YEAR :
                    *aFuncPtr = zlvCSLongToSqlIntervalYear;
                    break;
                case SQL_INTERVAL_MONTH :
                    *aFuncPtr = zlvCSLongToSqlIntervalMonth;
                    break;
                case SQL_INTERVAL_DAY :
                    *aFuncPtr = zlvCSLongToSqlIntervalDay;
                    break;
                case SQL_INTERVAL_HOUR :
                    *aFuncPtr = zlvCSLongToSqlIntervalHour;
                    break;
                case SQL_INTERVAL_MINUTE :
                    *aFuncPtr = zlvCSLongToSqlIntervalMinute;
                    break;
                case SQL_INTERVAL_SECOND :
                    *aFuncPtr = zlvCSLongToSqlIntervalSecond;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case SQL_C_ULONG :
            switch( aSqlType )
            {
                case SQL_CHAR :
                    *aFuncPtr = zlvCULongToSqlChar;
                    break;
                case SQL_VARCHAR :
                    *aFuncPtr = zlvCULongToSqlVarChar;
                    break;
                case SQL_LONGVARCHAR :
                    *aFuncPtr = zlvCULongToSqlLongVarChar;
                    break;
                case SQL_NUMERIC :
                    *aFuncPtr = zlvCULongToSqlNumeric;
                    break;
                case SQL_TINYINT :
                    *aFuncPtr = zlvCULongToSqlTinyInt;
                    break;
                case SQL_SMALLINT :
                    *aFuncPtr = zlvCULongToSqlSmallInt;
                    break;
                case SQL_INTEGER :
                    *aFuncPtr = zlvCULongToSqlInteger;
                    break;
                case SQL_BIGINT :
                    *aFuncPtr = zlvCULongToSqlBigInt;
                    break;
                case SQL_REAL :
                    *aFuncPtr = zlvCULongToSqlReal;
                    break;
                case SQL_DOUBLE :
                    *aFuncPtr = zlvCULongToSqlDouble;
                    break;
                case SQL_FLOAT :
                    *aFuncPtr = zlvCULongToSqlFloat;
                    break;
                case SQL_BIT :
                    *aFuncPtr = zlvCULongToSqlBit;
                    break;
                case SQL_BOOLEAN :
                    *aFuncPtr = zlvCULongToSqlBoolean;
                    break;
                case SQL_INTERVAL_YEAR :
                    *aFuncPtr = zlvCULongToSqlIntervalYear;
                    break;
                case SQL_INTERVAL_MONTH :
                    *aFuncPtr = zlvCULongToSqlIntervalMonth;
                    break;
                case SQL_INTERVAL_DAY :
                    *aFuncPtr = zlvCULongToSqlIntervalDay;
                    break;
                case SQL_INTERVAL_HOUR :
                    *aFuncPtr = zlvCULongToSqlIntervalHour;
                    break;
                case SQL_INTERVAL_MINUTE :
                    *aFuncPtr = zlvCULongToSqlIntervalMinute;
                    break;
                case SQL_INTERVAL_SECOND :
                    *aFuncPtr = zlvCULongToSqlIntervalSecond;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case SQL_C_FLOAT :
            switch( aSqlType )
            {
                case SQL_CHAR :
                    *aFuncPtr = zlvCFloatToSqlChar;
                    break;
                case SQL_VARCHAR :
                    *aFuncPtr = zlvCFloatToSqlVarChar;
                    break;
                case SQL_LONGVARCHAR :
                    *aFuncPtr = zlvCFloatToSqlLongVarChar;
                    break;
                case SQL_NUMERIC :
                    *aFuncPtr = zlvCFloatToSqlNumeric;
                    break;
                case SQL_TINYINT :
                    *aFuncPtr = zlvCFloatToSqlTinyInt;
                    break;
                case SQL_SMALLINT :
                    *aFuncPtr = zlvCFloatToSqlSmallInt;
                    break;
                case SQL_INTEGER :
                    *aFuncPtr = zlvCFloatToSqlInteger;
                    break;
                case SQL_BIGINT :
                    *aFuncPtr = zlvCFloatToSqlBigInt;
                    break;
                case SQL_REAL :
                    *aFuncPtr = zlvCFloatToSqlReal;
                    break;
                case SQL_DOUBLE :
                    *aFuncPtr = zlvCFloatToSqlDouble;
                    break;
                case SQL_FLOAT :
                    *aFuncPtr = zlvCFloatToSqlFloat;
                    break;
                case SQL_BIT :
                    *aFuncPtr = zlvCFloatToSqlBit;
                    break;
                case SQL_BOOLEAN :
                    *aFuncPtr = zlvCFloatToSqlBoolean;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case SQL_C_DOUBLE :
            switch( aSqlType )
            {
                case SQL_CHAR :
                    *aFuncPtr = zlvCDoubleToSqlChar;
                    break;
                case SQL_VARCHAR :
                    *aFuncPtr = zlvCDoubleToSqlVarChar;
                    break;
                case SQL_LONGVARCHAR :
                    *aFuncPtr = zlvCDoubleToSqlLongVarChar;
                    break;
                case SQL_NUMERIC :
                    *aFuncPtr = zlvCDoubleToSqlNumeric;
                    break;
                case SQL_TINYINT :
                    *aFuncPtr = zlvCDoubleToSqlTinyInt;
                    break;
                case SQL_SMALLINT :
                    *aFuncPtr = zlvCDoubleToSqlSmallInt;
                    break;
                case SQL_INTEGER :
                    *aFuncPtr = zlvCDoubleToSqlInteger;
                    break;
                case SQL_BIGINT :
                    *aFuncPtr = zlvCDoubleToSqlBigInt;
                    break;
                case SQL_REAL :
                    *aFuncPtr = zlvCDoubleToSqlReal;
                    break;
                case SQL_DOUBLE :
                    *aFuncPtr = zlvCDoubleToSqlDouble;
                    break;
                case SQL_FLOAT :
                    *aFuncPtr = zlvCDoubleToSqlFloat;
                    break;
                case SQL_BIT :
                    *aFuncPtr = zlvCDoubleToSqlBit;
                    break;
                case SQL_BOOLEAN :
                    *aFuncPtr = zlvCDoubleToSqlBoolean;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case SQL_C_BIT :
            switch( aSqlType )
            {
                case SQL_CHAR :
                    *aFuncPtr = zlvCBitToSqlChar;
                    break;
                case SQL_VARCHAR :
                    *aFuncPtr = zlvCBitToSqlVarChar;
                    break;
                case SQL_LONGVARCHAR :
                    *aFuncPtr = zlvCBitToSqlLongVarChar;
                    break;
                case SQL_NUMERIC :
                    *aFuncPtr = zlvCBitToSqlNumeric;
                    break;
                case SQL_TINYINT :
                    *aFuncPtr = zlvCBitToSqlTinyInt;
                    break;
                case SQL_SMALLINT :
                    *aFuncPtr = zlvCBitToSqlSmallInt;
                    break;
                case SQL_INTEGER :
                    *aFuncPtr = zlvCBitToSqlInteger;
                    break;
                case SQL_BIGINT :
                    *aFuncPtr = zlvCBitToSqlBigInt;
                    break;
                case SQL_REAL :
                    *aFuncPtr = zlvCBitToSqlReal;
                    break;
                case SQL_DOUBLE :
                    *aFuncPtr = zlvCBitToSqlDouble;
                    break;
                case SQL_FLOAT :
                    *aFuncPtr = zlvCBitToSqlFloat;
                    break;
                case SQL_BIT :
                    *aFuncPtr = zlvCBitToSqlBit;
                    break;
                case SQL_BOOLEAN :
                    *aFuncPtr = zlvCBitToSqlBoolean;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case SQL_C_BOOLEAN :
            switch( aSqlType )
            {
                case SQL_CHAR :
                    *aFuncPtr = zlvCBooleanToSqlChar;
                    break;
                case SQL_VARCHAR :
                    *aFuncPtr = zlvCBooleanToSqlVarChar;
                    break;
                case SQL_LONGVARCHAR :
                    *aFuncPtr = zlvCBooleanToSqlLongVarChar;
                    break;
                case SQL_NUMERIC :
                    *aFuncPtr = zlvCBooleanToSqlNumeric;
                    break;
                case SQL_TINYINT :
                    *aFuncPtr = zlvCBooleanToSqlTinyInt;
                    break;
                case SQL_SMALLINT :
                    *aFuncPtr = zlvCBooleanToSqlSmallInt;
                    break;
                case SQL_INTEGER :
                    *aFuncPtr = zlvCBooleanToSqlInteger;
                    break;
                case SQL_BIGINT :
                    *aFuncPtr = zlvCBooleanToSqlBigInt;
                    break;
                case SQL_REAL :
                    *aFuncPtr = zlvCBooleanToSqlReal;
                    break;
                case SQL_DOUBLE :
                    *aFuncPtr = zlvCBooleanToSqlDouble;
                    break;
                case SQL_FLOAT :
                    *aFuncPtr = zlvCBooleanToSqlFloat;
                    break;
                case SQL_BIT :
                    *aFuncPtr = zlvCBooleanToSqlBit;
                    break;
                case SQL_BOOLEAN :
                    *aFuncPtr = zlvCBooleanToSqlBoolean;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case SQL_C_BINARY :
            switch( aSqlType )
            {
                case SQL_CHAR :
                    *aFuncPtr = zlvCBinaryToSqlChar;
                    break;
                case SQL_VARCHAR :
                    *aFuncPtr = zlvCBinaryToSqlVarChar;
                    break;
                case SQL_LONGVARCHAR :
                    *aFuncPtr = zlvCBinaryToSqlLongVarChar;
                    break;
                case SQL_NUMERIC :
                    *aFuncPtr = zlvCBinaryToSqlNumeric;
                    break;
                case SQL_TINYINT :
                    *aFuncPtr = zlvCBinaryToSqlTinyInt;
                    break;
                case SQL_SMALLINT :
                    *aFuncPtr = zlvCBinaryToSqlSmallInt;
                    break;
                case SQL_INTEGER :
                    *aFuncPtr = zlvCBinaryToSqlInteger;
                    break;
                case SQL_BIGINT :
                    *aFuncPtr = zlvCBinaryToSqlBigInt;
                    break;
                case SQL_REAL :
                    *aFuncPtr = zlvCBinaryToSqlReal;
                    break;
                case SQL_DOUBLE :
                    *aFuncPtr = zlvCBinaryToSqlDouble;
                    break;
                case SQL_FLOAT :
                    *aFuncPtr = zlvCBinaryToSqlFloat;
                    break;
                case SQL_BIT :
                    *aFuncPtr = zlvCBinaryToSqlBit;
                    break;
                case SQL_BOOLEAN :
                    *aFuncPtr = zlvCBinaryToSqlBoolean;
                    break;
                case SQL_TYPE_DATE :
                    *aFuncPtr = zlvCBinaryToSqlDate;
                    break;
                case SQL_TYPE_TIME :
                    *aFuncPtr = zlvCBinaryToSqlTime;
                    break;
                case SQL_TYPE_TIME_WITH_TIMEZONE :
                    *aFuncPtr = zlvCBinaryToSqlTimeTz;
                    break;
                case SQL_TYPE_TIMESTAMP :
                    *aFuncPtr = zlvCBinaryToSqlTimestamp;
                    break;
                case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE :
                    *aFuncPtr = zlvCBinaryToSqlTimestampTz;
                    break;
                case SQL_BINARY :
                    *aFuncPtr = zlvCBinaryToSqlBinary;
                    break;
                case SQL_VARBINARY :
                    *aFuncPtr = zlvCBinaryToSqlVarBinary;
                    break;
                case SQL_LONGVARBINARY :
                    *aFuncPtr = zlvCBinaryToSqlLongVarBinary;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case SQL_C_LONGVARBINARY :
            switch( aSqlType )
            {
                case SQL_CHAR :
                    *aFuncPtr = zlvCLongVarBinaryToSqlChar;
                    break;
                case SQL_VARCHAR :
                    *aFuncPtr = zlvCLongVarBinaryToSqlVarChar;
                    break;
                case SQL_LONGVARCHAR :
                    *aFuncPtr = zlvCLongVarBinaryToSqlLongVarChar;
                    break;
                case SQL_NUMERIC :
                    *aFuncPtr = zlvCLongVarBinaryToSqlNumeric;
                    break;
                case SQL_TINYINT :
                    *aFuncPtr = zlvCLongVarBinaryToSqlTinyInt;
                    break;
                case SQL_SMALLINT :
                    *aFuncPtr = zlvCLongVarBinaryToSqlSmallInt;
                    break;
                case SQL_INTEGER :
                    *aFuncPtr = zlvCLongVarBinaryToSqlInteger;
                    break;
                case SQL_BIGINT :
                    *aFuncPtr = zlvCLongVarBinaryToSqlBigInt;
                    break;
                case SQL_REAL :
                    *aFuncPtr = zlvCLongVarBinaryToSqlReal;
                    break;
                case SQL_DOUBLE :
                    *aFuncPtr = zlvCLongVarBinaryToSqlDouble;
                    break;
                case SQL_FLOAT :
                    *aFuncPtr = zlvCLongVarBinaryToSqlFloat;
                    break;
                case SQL_BIT :
                    *aFuncPtr = zlvCLongVarBinaryToSqlBit;
                    break;
                case SQL_BOOLEAN :
                    *aFuncPtr = zlvCLongVarBinaryToSqlBoolean;
                    break;
                case SQL_TYPE_DATE :
                    *aFuncPtr = zlvCLongVarBinaryToSqlDate;
                    break;
                case SQL_TYPE_TIME :
                    *aFuncPtr = zlvCLongVarBinaryToSqlTime;
                    break;
                case SQL_TYPE_TIME_WITH_TIMEZONE :
                    *aFuncPtr = zlvCLongVarBinaryToSqlTimeTz;
                    break;
                case SQL_TYPE_TIMESTAMP :
                    *aFuncPtr = zlvCLongVarBinaryToSqlTimestamp;
                    break;
                case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE :
                    *aFuncPtr = zlvCLongVarBinaryToSqlTimestampTz;
                    break;
                case SQL_BINARY :
                    *aFuncPtr = zlvCLongVarBinaryToSqlBinary;
                    break;
                case SQL_VARBINARY :
                    *aFuncPtr = zlvCLongVarBinaryToSqlVarBinary;
                    break;
                case SQL_LONGVARBINARY :
                    *aFuncPtr = zlvCLongVarBinaryToSqlLongVarBinary;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case SQL_C_TYPE_DATE :
            switch( aSqlType )
            {
                case SQL_CHAR :
                    *aFuncPtr = zlvCDateToSqlChar;
                    break;
                case SQL_VARCHAR :
                    *aFuncPtr = zlvCDateToSqlVarChar;
                    break;
                case SQL_LONGVARCHAR :
                    *aFuncPtr = zlvCDateToSqlLongVarChar;
                    break;
                case SQL_TYPE_DATE :
                    *aFuncPtr = zlvCDateToSqlDate;
                    break;
                case SQL_TYPE_TIMESTAMP :
                    *aFuncPtr = zlvCDateToSqlTimestamp;
                    break;
                case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE :
                    *aFuncPtr = zlvCDateToSqlTimestampTz;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case SQL_C_TYPE_TIME :
            switch( aSqlType )
            {
                case SQL_CHAR :
                    *aFuncPtr = zlvCTimeToSqlChar;
                    break;
                case SQL_VARCHAR :
                    *aFuncPtr = zlvCTimeToSqlVarChar;
                    break;
                case SQL_LONGVARCHAR :
                    *aFuncPtr = zlvCTimeToSqlLongVarChar;
                    break;
                case SQL_TYPE_TIME :
                    *aFuncPtr = zlvCTimeToSqlTime;
                    break;
                case SQL_TYPE_TIME_WITH_TIMEZONE :
                    *aFuncPtr = zlvCTimeToSqlTimeTz;
                    break;
                case SQL_TYPE_TIMESTAMP :
                    *aFuncPtr = zlvCTimeToSqlTimestamp;
                    break;
                case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE :
                    *aFuncPtr = zlvCTimeToSqlTimestampTz;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case SQL_C_TYPE_TIME_WITH_TIMEZONE :
            switch( aSqlType )
            {
                case SQL_CHAR :
                    *aFuncPtr = zlvCTimeTzToSqlChar;
                    break;
                case SQL_VARCHAR :
                    *aFuncPtr = zlvCTimeTzToSqlVarChar;
                    break;
                case SQL_LONGVARCHAR :
                    *aFuncPtr = zlvCTimeTzToSqlLongVarChar;
                    break;
                case SQL_TYPE_TIME :
                    *aFuncPtr = zlvCTimeTzToSqlTime;
                    break;
                case SQL_TYPE_TIME_WITH_TIMEZONE :
                    *aFuncPtr = zlvCTimeTzToSqlTimeTz;
                    break;
                case SQL_TYPE_TIMESTAMP :
                    *aFuncPtr = zlvCTimeTzToSqlTimestamp;
                    break;
                case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE :
                    *aFuncPtr = zlvCTimeTzToSqlTimestampTz;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case SQL_C_TYPE_TIMESTAMP :
            switch( aSqlType )
            {
                case SQL_CHAR :
                    *aFuncPtr = zlvCTimestampToSqlChar;
                    break;
                case SQL_VARCHAR :
                    *aFuncPtr = zlvCTimestampToSqlVarChar;
                    break;
                case SQL_LONGVARCHAR :
                    *aFuncPtr = zlvCTimestampToSqlLongVarChar;
                    break;
                case SQL_TYPE_DATE :
                    *aFuncPtr = zlvCTimestampToSqlDate;
                    break;
                case SQL_TYPE_TIME :
                    *aFuncPtr = zlvCTimestampToSqlTime;
                    break;
                case SQL_TYPE_TIME_WITH_TIMEZONE :
                    *aFuncPtr = zlvCTimestampToSqlTimeTz;
                    break;
                case SQL_TYPE_TIMESTAMP :
                    *aFuncPtr = zlvCTimestampToSqlTimestamp;
                    break;
                case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE :
                    *aFuncPtr = zlvCTimestampToSqlTimestampTz;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case SQL_C_TYPE_TIMESTAMP_WITH_TIMEZONE :
            switch( aSqlType )
            {
                case SQL_CHAR :
                    *aFuncPtr = zlvCTimestampTzToSqlChar;
                    break;
                case SQL_VARCHAR :
                    *aFuncPtr = zlvCTimestampTzToSqlVarChar;
                    break;
                case SQL_LONGVARCHAR :
                    *aFuncPtr = zlvCTimestampTzToSqlLongVarChar;
                    break;
                case SQL_TYPE_DATE :
                    *aFuncPtr = zlvCTimestampTzToSqlDate;
                    break;
                case SQL_TYPE_TIME :
                    *aFuncPtr = zlvCTimestampTzToSqlTime;
                    break;
                case SQL_TYPE_TIME_WITH_TIMEZONE :
                    *aFuncPtr = zlvCTimestampTzToSqlTimeTz;
                    break;
                case SQL_TYPE_TIMESTAMP :
                    *aFuncPtr = zlvCTimestampTzToSqlTimestamp;
                    break;
                case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE :
                    *aFuncPtr = zlvCTimestampTzToSqlTimestampTz;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case SQL_C_INTERVAL_MONTH :
            switch( aSqlType )
            {
                case SQL_CHAR :
                    *aFuncPtr = zlvCIntervalMonthToSqlChar;
                    break;
                case SQL_VARCHAR :
                    *aFuncPtr = zlvCIntervalMonthToSqlVarChar;
                    break;
                case SQL_LONGVARCHAR :
                    *aFuncPtr = zlvCIntervalMonthToSqlLongVarChar;
                    break;
                case SQL_TINYINT :
                    *aFuncPtr = zlvCIntervalMonthToSqlTinyInt;
                    break;
                case SQL_SMALLINT :
                    *aFuncPtr = zlvCIntervalMonthToSqlSmallInt;
                    break;
                case SQL_INTEGER :
                    *aFuncPtr = zlvCIntervalMonthToSqlInteger;
                    break;
                case SQL_BIGINT :
                    *aFuncPtr = zlvCIntervalMonthToSqlBigInt;
                    break;
                case SQL_NUMERIC :
                    *aFuncPtr = zlvCIntervalMonthToSqlNumeric;
                    break;
                case SQL_INTERVAL_MONTH :
                    *aFuncPtr = zlvCIntervalMonthToSqlIntervalMonth;
                    break;
                case SQL_INTERVAL_YEAR :
                    *aFuncPtr = zlvCIntervalMonthToSqlIntervalYear;
                    break;
                case SQL_INTERVAL_YEAR_TO_MONTH :
                    *aFuncPtr = zlvCIntervalMonthToSqlIntervalYearToMonth;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case SQL_C_INTERVAL_YEAR :
            switch( aSqlType )
            {
                case SQL_CHAR :
                    *aFuncPtr = zlvCIntervalYearToSqlChar;
                    break;
                case SQL_VARCHAR :
                    *aFuncPtr = zlvCIntervalYearToSqlVarChar;
                    break;
                case SQL_LONGVARCHAR :
                    *aFuncPtr = zlvCIntervalYearToSqlLongVarChar;
                    break;
                case SQL_TINYINT :
                    *aFuncPtr = zlvCIntervalYearToSqlTinyInt;
                    break;
                case SQL_SMALLINT :
                    *aFuncPtr = zlvCIntervalYearToSqlSmallInt;
                    break;
                case SQL_INTEGER :
                    *aFuncPtr = zlvCIntervalYearToSqlInteger;
                    break;
                case SQL_BIGINT :
                    *aFuncPtr = zlvCIntervalYearToSqlBigInt;
                    break;
                case SQL_NUMERIC :
                    *aFuncPtr = zlvCIntervalYearToSqlNumeric;
                    break;
                case SQL_INTERVAL_MONTH :
                    *aFuncPtr = zlvCIntervalYearToSqlIntervalMonth;
                    break;
                case SQL_INTERVAL_YEAR :
                    *aFuncPtr = zlvCIntervalYearToSqlIntervalYear;
                    break;
                case SQL_INTERVAL_YEAR_TO_MONTH :
                    *aFuncPtr = zlvCIntervalYearToSqlIntervalYearToMonth;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case SQL_C_INTERVAL_YEAR_TO_MONTH :
            switch( aSqlType )
            {
                case SQL_CHAR :
                    *aFuncPtr = zlvCIntervalYearToMonthToSqlChar;
                    break;
                case SQL_VARCHAR :
                    *aFuncPtr = zlvCIntervalYearToMonthToSqlVarChar;
                    break;
                case SQL_LONGVARCHAR :
                    *aFuncPtr = zlvCIntervalYearToMonthToSqlLongVarChar;
                    break;
                case SQL_INTERVAL_MONTH :
                    *aFuncPtr = zlvCIntervalYearToMonthToSqlIntervalMonth;
                    break;
                case SQL_INTERVAL_YEAR :
                    *aFuncPtr = zlvCIntervalYearToMonthToSqlIntervalYear;
                    break;
                case SQL_INTERVAL_YEAR_TO_MONTH :
                    *aFuncPtr = zlvCIntervalYearToMonthToSqlIntervalYearToMonth;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case SQL_C_INTERVAL_DAY :
            switch( aSqlType )
            {
                case SQL_CHAR :
                    *aFuncPtr = zlvCIntervalDayToSqlChar;
                    break;
                case SQL_VARCHAR :
                    *aFuncPtr = zlvCIntervalDayToSqlVarChar;
                    break;
                case SQL_LONGVARCHAR :
                    *aFuncPtr = zlvCIntervalDayToSqlLongVarChar;
                    break;
                case SQL_TINYINT :
                    *aFuncPtr = zlvCIntervalDayToSqlTinyInt;
                    break;
                case SQL_SMALLINT :
                    *aFuncPtr = zlvCIntervalDayToSqlSmallInt;
                    break;
                case SQL_INTEGER :
                    *aFuncPtr = zlvCIntervalDayToSqlInteger;
                    break;
                case SQL_BIGINT :
                    *aFuncPtr = zlvCIntervalDayToSqlBigInt;
                    break;
                case SQL_NUMERIC :
                    *aFuncPtr = zlvCIntervalDayToSqlNumeric;
                    break;
                case SQL_INTERVAL_DAY :
                    *aFuncPtr = zlvCIntervalDayToSqlIntervalDay;
                    break;
                case SQL_INTERVAL_HOUR :
                    *aFuncPtr = zlvCIntervalDayToSqlIntervalHour;
                    break;
                case SQL_INTERVAL_MINUTE :
                    *aFuncPtr = zlvCIntervalDayToSqlIntervalMinute;
                    break;
                case SQL_INTERVAL_SECOND :
                    *aFuncPtr = zlvCIntervalDayToSqlIntervalSecond;
                    break;
                case SQL_INTERVAL_DAY_TO_HOUR :
                    *aFuncPtr = zlvCIntervalDayToSqlIntervalDayToHour;
                    break;
                case SQL_INTERVAL_DAY_TO_MINUTE :
                    *aFuncPtr = zlvCIntervalDayToSqlIntervalDayToMinute;
                    break;
                case SQL_INTERVAL_DAY_TO_SECOND :
                    *aFuncPtr = zlvCIntervalDayToSqlIntervalDayToSecond;
                    break;
                case SQL_INTERVAL_HOUR_TO_MINUTE :
                    *aFuncPtr = zlvCIntervalDayToSqlIntervalHourToMinute;
                    break;
                case SQL_INTERVAL_HOUR_TO_SECOND :
                    *aFuncPtr = zlvCIntervalDayToSqlIntervalHourToSecond;
                    break;
                case SQL_INTERVAL_MINUTE_TO_SECOND :
                    *aFuncPtr = zlvCIntervalDayToSqlIntervalMinuteToSecond;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case SQL_C_INTERVAL_HOUR :
            switch( aSqlType )
            {
                case SQL_CHAR :
                    *aFuncPtr = zlvCIntervalHourToSqlChar;
                    break;
                case SQL_VARCHAR :
                    *aFuncPtr = zlvCIntervalHourToSqlVarChar;
                    break;
                case SQL_LONGVARCHAR :
                    *aFuncPtr = zlvCIntervalHourToSqlLongVarChar;
                    break;
                case SQL_TINYINT :
                    *aFuncPtr = zlvCIntervalHourToSqlTinyInt;
                    break;
                case SQL_SMALLINT :
                    *aFuncPtr = zlvCIntervalHourToSqlSmallInt;
                    break;
                case SQL_INTEGER :
                    *aFuncPtr = zlvCIntervalHourToSqlInteger;
                    break;
                case SQL_BIGINT :
                    *aFuncPtr = zlvCIntervalHourToSqlBigInt;
                    break;
                case SQL_NUMERIC :
                    *aFuncPtr = zlvCIntervalHourToSqlNumeric;
                    break;
                case SQL_INTERVAL_DAY :
                    *aFuncPtr = zlvCIntervalHourToSqlIntervalDay;
                    break;
                case SQL_INTERVAL_HOUR :
                    *aFuncPtr = zlvCIntervalHourToSqlIntervalHour;
                    break;
                case SQL_INTERVAL_MINUTE :
                    *aFuncPtr = zlvCIntervalHourToSqlIntervalMinute;
                    break;
                case SQL_INTERVAL_SECOND :
                    *aFuncPtr = zlvCIntervalHourToSqlIntervalSecond;
                    break;
                case SQL_INTERVAL_DAY_TO_HOUR :
                    *aFuncPtr = zlvCIntervalHourToSqlIntervalDayToHour;
                    break;
                case SQL_INTERVAL_DAY_TO_MINUTE :
                    *aFuncPtr = zlvCIntervalHourToSqlIntervalDayToMinute;
                    break;
                case SQL_INTERVAL_DAY_TO_SECOND :
                    *aFuncPtr = zlvCIntervalHourToSqlIntervalDayToSecond;
                    break;
                case SQL_INTERVAL_HOUR_TO_MINUTE :
                    *aFuncPtr = zlvCIntervalHourToSqlIntervalHourToMinute;
                    break;
                case SQL_INTERVAL_HOUR_TO_SECOND :
                    *aFuncPtr = zlvCIntervalHourToSqlIntervalHourToSecond;
                    break;
                case SQL_INTERVAL_MINUTE_TO_SECOND :
                    *aFuncPtr = zlvCIntervalHourToSqlIntervalMinuteToSecond;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case SQL_C_INTERVAL_MINUTE :
            switch( aSqlType )
            {
                case SQL_CHAR :
                    *aFuncPtr = zlvCIntervalMinuteToSqlChar;
                    break;
                case SQL_VARCHAR :
                    *aFuncPtr = zlvCIntervalMinuteToSqlVarChar;
                    break;
                case SQL_LONGVARCHAR :
                    *aFuncPtr = zlvCIntervalMinuteToSqlLongVarChar;
                    break;
                case SQL_TINYINT :
                    *aFuncPtr = zlvCIntervalMinuteToSqlTinyInt;
                    break;
                case SQL_SMALLINT :
                    *aFuncPtr = zlvCIntervalMinuteToSqlSmallInt;
                    break;
                case SQL_INTEGER :
                    *aFuncPtr = zlvCIntervalMinuteToSqlInteger;
                    break;
                case SQL_BIGINT :
                    *aFuncPtr = zlvCIntervalMinuteToSqlBigInt;
                    break;
                case SQL_NUMERIC :
                    *aFuncPtr = zlvCIntervalMinuteToSqlNumeric;
                    break;
                case SQL_INTERVAL_DAY :
                    *aFuncPtr = zlvCIntervalMinuteToSqlIntervalDay;
                    break;
                case SQL_INTERVAL_HOUR :
                    *aFuncPtr = zlvCIntervalMinuteToSqlIntervalHour;
                    break;
                case SQL_INTERVAL_MINUTE :
                    *aFuncPtr = zlvCIntervalMinuteToSqlIntervalMinute;
                    break;
                case SQL_INTERVAL_SECOND :
                    *aFuncPtr = zlvCIntervalMinuteToSqlIntervalSecond;
                    break;
                case SQL_INTERVAL_DAY_TO_HOUR :
                    *aFuncPtr = zlvCIntervalMinuteToSqlIntervalDayToHour;
                    break;
                case SQL_INTERVAL_DAY_TO_MINUTE :
                    *aFuncPtr = zlvCIntervalMinuteToSqlIntervalDayToMinute;
                    break;
                case SQL_INTERVAL_DAY_TO_SECOND :
                    *aFuncPtr = zlvCIntervalMinuteToSqlIntervalDayToSecond;
                    break;
                case SQL_INTERVAL_HOUR_TO_MINUTE :
                    *aFuncPtr = zlvCIntervalMinuteToSqlIntervalHourToMinute;
                    break;
                case SQL_INTERVAL_HOUR_TO_SECOND :
                    *aFuncPtr = zlvCIntervalMinuteToSqlIntervalHourToSecond;
                    break;
                case SQL_INTERVAL_MINUTE_TO_SECOND :
                    *aFuncPtr = zlvCIntervalMinuteToSqlIntervalMinuteToSecond;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case SQL_C_INTERVAL_SECOND :
            switch( aSqlType )
            {
                case SQL_CHAR :
                    *aFuncPtr = zlvCIntervalSecondToSqlChar;
                    break;
                case SQL_VARCHAR :
                    *aFuncPtr = zlvCIntervalSecondToSqlVarChar;
                    break;
                case SQL_LONGVARCHAR :
                    *aFuncPtr = zlvCIntervalSecondToSqlLongVarChar;
                    break;
                case SQL_TINYINT :
                    *aFuncPtr = zlvCIntervalSecondToSqlTinyInt;
                    break;
                case SQL_SMALLINT :
                    *aFuncPtr = zlvCIntervalSecondToSqlSmallInt;
                    break;
                case SQL_INTEGER :
                    *aFuncPtr = zlvCIntervalSecondToSqlInteger;
                    break;
                case SQL_BIGINT :
                    *aFuncPtr = zlvCIntervalSecondToSqlBigInt;
                    break;
                case SQL_NUMERIC :
                    *aFuncPtr = zlvCIntervalSecondToSqlNumeric;
                    break;
                case SQL_INTERVAL_DAY :
                    *aFuncPtr = zlvCIntervalSecondToSqlIntervalDay;
                    break;
                case SQL_INTERVAL_HOUR :
                    *aFuncPtr = zlvCIntervalSecondToSqlIntervalHour;
                    break;
                case SQL_INTERVAL_MINUTE :
                    *aFuncPtr = zlvCIntervalSecondToSqlIntervalMinute;
                    break;
                case SQL_INTERVAL_SECOND :
                    *aFuncPtr = zlvCIntervalSecondToSqlIntervalSecond;
                    break;
                case SQL_INTERVAL_DAY_TO_HOUR :
                    *aFuncPtr = zlvCIntervalSecondToSqlIntervalDayToHour;
                    break;
                case SQL_INTERVAL_DAY_TO_MINUTE :
                    *aFuncPtr = zlvCIntervalSecondToSqlIntervalDayToMinute;
                    break;
                case SQL_INTERVAL_DAY_TO_SECOND :
                    *aFuncPtr = zlvCIntervalSecondToSqlIntervalDayToSecond;
                    break;
                case SQL_INTERVAL_HOUR_TO_MINUTE :
                    *aFuncPtr = zlvCIntervalSecondToSqlIntervalHourToMinute;
                    break;
                case SQL_INTERVAL_HOUR_TO_SECOND :
                    *aFuncPtr = zlvCIntervalSecondToSqlIntervalHourToSecond;
                    break;
                case SQL_INTERVAL_MINUTE_TO_SECOND :
                    *aFuncPtr = zlvCIntervalSecondToSqlIntervalMinuteToSecond;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case SQL_C_INTERVAL_DAY_TO_HOUR :
            switch( aSqlType )
            {
                case SQL_CHAR :
                    *aFuncPtr = zlvCIntervalDayToHourToSqlChar;
                    break;
                case SQL_VARCHAR :
                    *aFuncPtr = zlvCIntervalDayToHourToSqlVarChar;
                    break;
                case SQL_LONGVARCHAR :
                    *aFuncPtr = zlvCIntervalDayToHourToSqlLongVarChar;
                    break;
                case SQL_INTERVAL_DAY :
                    *aFuncPtr = zlvCIntervalDayToHourToSqlIntervalDay;
                    break;
                case SQL_INTERVAL_HOUR :
                    *aFuncPtr = zlvCIntervalDayToHourToSqlIntervalHour;
                    break;
                case SQL_INTERVAL_MINUTE :
                    *aFuncPtr = zlvCIntervalDayToHourToSqlIntervalMinute;
                    break;
                case SQL_INTERVAL_SECOND :
                    *aFuncPtr = zlvCIntervalDayToHourToSqlIntervalSecond;
                    break;
                case SQL_INTERVAL_DAY_TO_HOUR :
                    *aFuncPtr = zlvCIntervalDayToHourToSqlIntervalDayToHour;
                    break;
                case SQL_INTERVAL_DAY_TO_MINUTE :
                    *aFuncPtr = zlvCIntervalDayToHourToSqlIntervalDayToMinute;
                    break;
                case SQL_INTERVAL_DAY_TO_SECOND :
                    *aFuncPtr = zlvCIntervalDayToHourToSqlIntervalDayToSecond;
                    break;
                case SQL_INTERVAL_HOUR_TO_MINUTE :
                    *aFuncPtr = zlvCIntervalDayToHourToSqlIntervalHourToMinute;
                    break;
                case SQL_INTERVAL_HOUR_TO_SECOND :
                    *aFuncPtr = zlvCIntervalDayToHourToSqlIntervalHourToSecond;
                    break;
                case SQL_INTERVAL_MINUTE_TO_SECOND :
                    *aFuncPtr = zlvCIntervalDayToHourToSqlIntervalMinuteToSecond;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case SQL_C_INTERVAL_DAY_TO_MINUTE :
            switch( aSqlType )
            {
                case SQL_CHAR :
                    *aFuncPtr = zlvCIntervalDayToMinuteToSqlChar;
                    break;
                case SQL_VARCHAR :
                    *aFuncPtr = zlvCIntervalDayToMinuteToSqlVarChar;
                    break;
                case SQL_LONGVARCHAR :
                    *aFuncPtr = zlvCIntervalDayToMinuteToSqlLongVarChar;
                    break;
                case SQL_INTERVAL_DAY :
                    *aFuncPtr = zlvCIntervalDayToMinuteToSqlIntervalDay;
                    break;
                case SQL_INTERVAL_HOUR :
                    *aFuncPtr = zlvCIntervalDayToMinuteToSqlIntervalHour;
                    break;
                case SQL_INTERVAL_MINUTE :
                    *aFuncPtr = zlvCIntervalDayToMinuteToSqlIntervalMinute;
                    break;
                case SQL_INTERVAL_SECOND :
                    *aFuncPtr = zlvCIntervalDayToMinuteToSqlIntervalSecond;
                    break;
                case SQL_INTERVAL_DAY_TO_HOUR :
                    *aFuncPtr = zlvCIntervalDayToMinuteToSqlIntervalDayToHour;
                    break;
                case SQL_INTERVAL_DAY_TO_MINUTE :
                    *aFuncPtr = zlvCIntervalDayToMinuteToSqlIntervalDayToMinute;
                    break;
                case SQL_INTERVAL_DAY_TO_SECOND :
                    *aFuncPtr = zlvCIntervalDayToMinuteToSqlIntervalDayToSecond;
                    break;
                case SQL_INTERVAL_HOUR_TO_MINUTE :
                    *aFuncPtr = zlvCIntervalDayToMinuteToSqlIntervalHourToMinute;
                    break;
                case SQL_INTERVAL_HOUR_TO_SECOND :
                    *aFuncPtr = zlvCIntervalDayToMinuteToSqlIntervalHourToSecond;
                    break;
                case SQL_INTERVAL_MINUTE_TO_SECOND :
                    *aFuncPtr = zlvCIntervalDayToMinuteToSqlIntervalMinuteToSecond;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case SQL_C_INTERVAL_DAY_TO_SECOND :
            switch( aSqlType )
            {
                case SQL_CHAR :
                    *aFuncPtr = zlvCIntervalDayToSecondToSqlChar;
                    break;
                case SQL_VARCHAR :
                    *aFuncPtr = zlvCIntervalDayToSecondToSqlVarChar;
                    break;
                case SQL_LONGVARCHAR :
                    *aFuncPtr = zlvCIntervalDayToSecondToSqlLongVarChar;
                    break;
                case SQL_INTERVAL_DAY :
                    *aFuncPtr = zlvCIntervalDayToSecondToSqlIntervalDay;
                    break;
                case SQL_INTERVAL_HOUR :
                    *aFuncPtr = zlvCIntervalDayToSecondToSqlIntervalHour;
                    break;
                case SQL_INTERVAL_MINUTE :
                    *aFuncPtr = zlvCIntervalDayToSecondToSqlIntervalMinute;
                    break;
                case SQL_INTERVAL_SECOND :
                    *aFuncPtr = zlvCIntervalDayToSecondToSqlIntervalSecond;
                    break;
                case SQL_INTERVAL_DAY_TO_HOUR :
                    *aFuncPtr = zlvCIntervalDayToSecondToSqlIntervalDayToHour;
                    break;
                case SQL_INTERVAL_DAY_TO_MINUTE :
                    *aFuncPtr = zlvCIntervalDayToSecondToSqlIntervalDayToMinute;
                    break;
                case SQL_INTERVAL_DAY_TO_SECOND :
                    *aFuncPtr = zlvCIntervalDayToSecondToSqlIntervalDayToSecond;
                    break;
                case SQL_INTERVAL_HOUR_TO_MINUTE :
                    *aFuncPtr = zlvCIntervalDayToSecondToSqlIntervalHourToMinute;
                    break;
                case SQL_INTERVAL_HOUR_TO_SECOND :
                    *aFuncPtr = zlvCIntervalDayToSecondToSqlIntervalHourToSecond;
                    break;
                case SQL_INTERVAL_MINUTE_TO_SECOND :
                    *aFuncPtr = zlvCIntervalDayToSecondToSqlIntervalMinuteToSecond;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case SQL_C_INTERVAL_HOUR_TO_MINUTE :
            switch( aSqlType )
            {
                case SQL_CHAR :
                    *aFuncPtr = zlvCIntervalHourToMinuteToSqlChar;
                    break;
                case SQL_VARCHAR :
                    *aFuncPtr = zlvCIntervalHourToMinuteToSqlVarChar;
                    break;
                case SQL_LONGVARCHAR :
                    *aFuncPtr = zlvCIntervalHourToMinuteToSqlLongVarChar;
                    break;
                case SQL_INTERVAL_DAY :
                    *aFuncPtr = zlvCIntervalHourToMinuteToSqlIntervalDay;
                    break;
                case SQL_INTERVAL_HOUR :
                    *aFuncPtr = zlvCIntervalHourToMinuteToSqlIntervalHour;
                    break;
                case SQL_INTERVAL_MINUTE :
                    *aFuncPtr = zlvCIntervalHourToMinuteToSqlIntervalMinute;
                    break;
                case SQL_INTERVAL_SECOND :
                    *aFuncPtr = zlvCIntervalHourToMinuteToSqlIntervalSecond;
                    break;
                case SQL_INTERVAL_DAY_TO_HOUR :
                    *aFuncPtr = zlvCIntervalHourToMinuteToSqlIntervalDayToHour;
                    break;
                case SQL_INTERVAL_DAY_TO_MINUTE :
                    *aFuncPtr = zlvCIntervalHourToMinuteToSqlIntervalDayToMinute;
                    break;
                case SQL_INTERVAL_DAY_TO_SECOND :
                    *aFuncPtr = zlvCIntervalHourToMinuteToSqlIntervalDayToSecond;
                    break;
                case SQL_INTERVAL_HOUR_TO_MINUTE :
                    *aFuncPtr = zlvCIntervalHourToMinuteToSqlIntervalHourToMinute;
                    break;
                case SQL_INTERVAL_HOUR_TO_SECOND :
                    *aFuncPtr = zlvCIntervalHourToMinuteToSqlIntervalHourToSecond;
                    break;
                case SQL_INTERVAL_MINUTE_TO_SECOND :
                    *aFuncPtr = zlvCIntervalHourToMinuteToSqlIntervalMinuteToSecond;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case SQL_C_INTERVAL_HOUR_TO_SECOND :
            switch( aSqlType )
            {
                case SQL_CHAR :
                    *aFuncPtr = zlvCIntervalHourToSecondToSqlChar;
                    break;
                case SQL_VARCHAR :
                    *aFuncPtr = zlvCIntervalHourToSecondToSqlVarChar;
                    break;
                case SQL_LONGVARCHAR :
                    *aFuncPtr = zlvCIntervalHourToSecondToSqlLongVarChar;
                    break;
                case SQL_INTERVAL_DAY :
                    *aFuncPtr = zlvCIntervalHourToSecondToSqlIntervalDay;
                    break;
                case SQL_INTERVAL_HOUR :
                    *aFuncPtr = zlvCIntervalHourToSecondToSqlIntervalHour;
                    break;
                case SQL_INTERVAL_MINUTE :
                    *aFuncPtr = zlvCIntervalHourToSecondToSqlIntervalMinute;
                    break;
                case SQL_INTERVAL_SECOND :
                    *aFuncPtr = zlvCIntervalHourToSecondToSqlIntervalSecond;
                    break;
                case SQL_INTERVAL_DAY_TO_HOUR :
                    *aFuncPtr = zlvCIntervalHourToSecondToSqlIntervalDayToHour;
                    break;
                case SQL_INTERVAL_DAY_TO_MINUTE :
                    *aFuncPtr = zlvCIntervalHourToSecondToSqlIntervalDayToMinute;
                    break;
                case SQL_INTERVAL_DAY_TO_SECOND :
                    *aFuncPtr = zlvCIntervalHourToSecondToSqlIntervalDayToSecond;
                    break;
                case SQL_INTERVAL_HOUR_TO_MINUTE :
                    *aFuncPtr = zlvCIntervalHourToSecondToSqlIntervalHourToMinute;
                    break;
                case SQL_INTERVAL_HOUR_TO_SECOND :
                    *aFuncPtr = zlvCIntervalHourToSecondToSqlIntervalHourToSecond;
                    break;
                case SQL_INTERVAL_MINUTE_TO_SECOND :
                    *aFuncPtr = zlvCIntervalHourToSecondToSqlIntervalMinuteToSecond;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case SQL_C_INTERVAL_MINUTE_TO_SECOND :
            switch( aSqlType )
            {
                case SQL_CHAR :
                    *aFuncPtr = zlvCIntervalMinuteToSecondToSqlChar;
                    break;
                case SQL_VARCHAR :
                    *aFuncPtr = zlvCIntervalMinuteToSecondToSqlVarChar;
                    break;
                case SQL_LONGVARCHAR :
                    *aFuncPtr = zlvCIntervalMinuteToSecondToSqlLongVarChar;
                    break;
                case SQL_INTERVAL_DAY :
                    *aFuncPtr = zlvCIntervalMinuteToSecondToSqlIntervalDay;
                    break;
                case SQL_INTERVAL_HOUR :
                    *aFuncPtr = zlvCIntervalMinuteToSecondToSqlIntervalHour;
                    break;
                case SQL_INTERVAL_MINUTE :
                    *aFuncPtr = zlvCIntervalMinuteToSecondToSqlIntervalMinute;
                    break;
                case SQL_INTERVAL_SECOND :
                    *aFuncPtr = zlvCIntervalMinuteToSecondToSqlIntervalSecond;
                    break;
                case SQL_INTERVAL_DAY_TO_HOUR :
                    *aFuncPtr = zlvCIntervalMinuteToSecondToSqlIntervalDayToHour;
                    break;
                case SQL_INTERVAL_DAY_TO_MINUTE :
                    *aFuncPtr = zlvCIntervalMinuteToSecondToSqlIntervalDayToMinute;
                    break;
                case SQL_INTERVAL_DAY_TO_SECOND :
                    *aFuncPtr = zlvCIntervalMinuteToSecondToSqlIntervalDayToSecond;
                    break;
                case SQL_INTERVAL_HOUR_TO_MINUTE :
                    *aFuncPtr = zlvCIntervalMinuteToSecondToSqlIntervalHourToMinute;
                    break;
                case SQL_INTERVAL_HOUR_TO_SECOND :
                    *aFuncPtr = zlvCIntervalMinuteToSecondToSqlIntervalHourToSecond;
                    break;
                case SQL_INTERVAL_MINUTE_TO_SECOND :
                    *aFuncPtr = zlvCIntervalMinuteToSecondToSqlIntervalMinuteToSecond;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        default :
            STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
            break;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION,
                      "The data value identified by the ValueType argument in SQLBindParameter "
                      "for the bound parameter could not be converted to the data type "
                      "identified by the ParameterType argument in SQLBindParameter.",
                      aErrorStack );

        *aFuncPtr = zlvInvalidCtoSql;
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlvInvalidCtoSql( zlsStmt        * aStmt,
                            void           * aParameterValuePtr,
                            SQLLEN         * aIndicator,
                            zldDescElement * aApdRec,
                            zldDescElement * aIpdRec,
                            dtlDataValue   * aDataValue,
                            stlBool        * aSuccessWithInfo,
                            stlErrorStack  * aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  ZLE_ERRCODE_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION,
                  "The data value identified by the ValueType argument in SQLBindParameter "
                  "for the bound parameter could not be converted to the data type "
                  "identified by the ParameterType argument in SQLBindParameter.",
                  aErrorStack );

    return STL_FAILURE;
}

/** @} */

