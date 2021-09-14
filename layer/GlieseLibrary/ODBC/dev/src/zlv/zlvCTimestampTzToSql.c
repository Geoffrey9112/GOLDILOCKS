/*******************************************************************************
 * zlvCTimestampTzToSql.c
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
 * @file zlvCTimestampTzToSql.c
 * @brief Gliese API Internal Converting Data from C Timestamp With Time Zone to SQL Data Types Routines.
 */

/**
 * @addtogroup zlvCTimestampTzToSql
 * @{
 */

/*
 * http://msdn.microsoft.com/en-us/library/windows/desktop/ms709385%28v=VS.85%29.aspx
 */

/*
 * SYYYY-MM-DD HH24:MI:SS.FF6 TZH:TZM
 * 예) 1999-01-01 11:22:33.123456 +09:00
 *     -1999-01-01 11:22:33.123456 +09:00
 */
#define ZLV_TIMESTAMP_WITH_TIMEZONE_STRING_LENGTH 37

/*
 * SQL_C_TIMESTAMP_WITH_TIMEZONE -> SQL_CHAR
 */
ZLV_DECLARE_C_TO_SQL( TimestampTz, Char )
{
    stlInt32 sLength = 0;
    stlChar  sTimestampTzString[ZLV_TIMESTAMP_WITH_TIMEZONE_STRING_LENGTH];
    stlBool  sSuccessWithInfo;
    SQL_TIMESTAMP_WITH_TIMEZONE_STRUCT * sParameter;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mLength,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_OCTETS,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_TIMESTAMP_WITH_TIMEZONE_STRUCT*)aParameterValuePtr;

    STL_TRY_THROW( dtlIsValidateTimestamp( sParameter->year,
                                           sParameter->month,
                                           sParameter->day,
                                           sParameter->hour,
                                           sParameter->minute,
                                           sParameter->second,
                                           sParameter->fraction / 1000 ) == STL_TRUE,
                   RAMP_ERR_DATETIME_FIELD_OVERFLOW );

    if( sParameter->fraction == 0 )
    {
        STL_TRY_THROW( aIpdRec->mOctetLength >= 28,
                       RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED );

        if( sParameter->timezone_hour < 0 )
        {
            sLength = stlSnprintf( sTimestampTzString,
                                   ZLV_TIMESTAMP_WITH_TIMEZONE_STRING_LENGTH,
                                   "%04d-%02u-%02u %02u:%02u:%02u -%02d:%02d",
                                   sParameter->year,
                                   sParameter->month,
                                   sParameter->day,
                                   sParameter->hour,
                                   sParameter->minute,
                                   sParameter->second,
                                   stlAbsInt32( sParameter->timezone_hour ),
                                   stlAbsInt32( sParameter->timezone_minute ) );
        }
        else if( sParameter->timezone_hour > 0 )
        {
            sLength = stlSnprintf( sTimestampTzString,
                                   ZLV_TIMESTAMP_WITH_TIMEZONE_STRING_LENGTH,
                                   "%04d-%02u-%02u %02u:%02u:%02u +%02d:%02d",
                                   sParameter->year,
                                   sParameter->month,
                                   sParameter->day,
                                   sParameter->hour,
                                   sParameter->minute,
                                   sParameter->second,
                                   sParameter->timezone_hour,
                                   sParameter->timezone_minute );
        }
        else
        {
            if( sParameter->timezone_minute < 0 )
            {
                sLength = stlSnprintf( sTimestampTzString,
                                       ZLV_TIMESTAMP_WITH_TIMEZONE_STRING_LENGTH,
                                       "%04d-%02u-%02u %02u:%02u:%02u -%02d:%02d",
                                       sParameter->year,
                                       sParameter->month,
                                       sParameter->day,
                                       sParameter->hour,
                                       sParameter->minute,
                                       sParameter->second,
                                       stlAbsInt32( sParameter->timezone_hour ),
                                       stlAbsInt32( sParameter->timezone_minute ) );
            }
            else if( sParameter->timezone_minute > 0 )
            {
                sLength = stlSnprintf( sTimestampTzString,
                                       ZLV_TIMESTAMP_WITH_TIMEZONE_STRING_LENGTH,
                                       "%04d-%02u-%02u %02u:%02u:%02u +%02d:%02d",
                                       sParameter->year,
                                       sParameter->month,
                                       sParameter->day,
                                       sParameter->hour,
                                       sParameter->minute,
                                       sParameter->second,
                                       sParameter->timezone_hour,
                                       sParameter->timezone_minute );
            }
        }
    }
    else
    {
        STL_TRY_THROW( aIpdRec->mOctetLength >= ZLV_TIMESTAMP_WITH_TIMEZONE_STRING_LENGTH,
                       RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED );

        if( sParameter->timezone_hour < 0 )
        {
            sLength = stlSnprintf( sTimestampTzString,
                                   ZLV_TIMESTAMP_WITH_TIMEZONE_STRING_LENGTH,
                                   "%04d-%02u-%02u %02u:%02u:%02u.%09u -%02d:%02d",
                                   sParameter->year,
                                   sParameter->month,
                                   sParameter->day,
                                   sParameter->hour,
                                   sParameter->minute,
                                   sParameter->second,
                                   sParameter->fraction,
                                   stlAbsInt32( sParameter->timezone_hour ),
                                   stlAbsInt32( sParameter->timezone_minute ) );
        }
        else if( sParameter->timezone_hour > 0 )
        {
            sLength = stlSnprintf( sTimestampTzString,
                                   ZLV_TIMESTAMP_WITH_TIMEZONE_STRING_LENGTH,
                                   "%04d-%02u-%02u %02u:%02u:%02u.%09u +%02d:%02d",
                                   sParameter->year,
                                   sParameter->month,
                                   sParameter->day,
                                   sParameter->hour,
                                   sParameter->minute,
                                   sParameter->second,
                                   sParameter->fraction,
                                   sParameter->timezone_hour,
                                   sParameter->timezone_minute );
        }
        else
        {
            if( sParameter->timezone_minute < 0 )
            {
                sLength = stlSnprintf( sTimestampTzString,
                                       ZLV_TIMESTAMP_WITH_TIMEZONE_STRING_LENGTH,
                                       "%04d-%02u-%02u %02u:%02u:%02u.%09u -%02d:%02d",
                                       sParameter->year,
                                       sParameter->month,
                                       sParameter->day,
                                       sParameter->hour,
                                       sParameter->minute,
                                       sParameter->second,
                                       sParameter->fraction,
                                       stlAbsInt32( sParameter->timezone_hour ),
                                       stlAbsInt32( sParameter->timezone_minute ) );
            }
            else if( sParameter->timezone_minute > 0 )
            {
                sLength = stlSnprintf( sTimestampTzString,
                                       ZLV_TIMESTAMP_WITH_TIMEZONE_STRING_LENGTH,
                                       "%04d-%02u-%02u %02u:%02u:%02u.%09u +%02d:%02d",
                                       sParameter->year,
                                       sParameter->month,
                                       sParameter->day,
                                       sParameter->hour,
                                       sParameter->minute,
                                       sParameter->second,
                                       sParameter->fraction,
                                       sParameter->timezone_hour,
                                       sParameter->timezone_minute );
            }
        }
    }

    STL_TRY( dtlCharSetValueFromStringWithoutPaddNull( sTimestampTzString,
                                                       sLength,
                                                       aIpdRec->mLength,
                                                       DTL_SCALE_NA,
                                                       DTL_STRING_LENGTH_UNIT_OCTETS,
                                                       DTL_INTERVAL_INDICATOR_NA,
                                                       aIpdRec->mOctetLength,
                                                       aDataValue,
                                                       &sSuccessWithInfo,
                                                       ZLS_STMT_DT_VECTOR(aStmt),
                                                       aStmt,
                                                       ZLS_STMT_NLS_DT_VECTOR(aStmt),
                                                       aStmt,
                                                       aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_DATETIME_FIELD_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_STRING_DATA_RIGHT_TRUNCATED,
                      "The prepared statement associated with the StatementHandle contained "
                      "an SQL statement that contained a datetime expression that, "
                      "when computed, resulted in a date, time, or timestamp structure that was invalid.",
                      aErrorStack );
    }
    
    STL_CATCH( RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_STRING_DATA_RIGHT_TRUNCATED,
                      "The assignment of a character or binary value to a column resulted "
                      "in the truncation of nonblank (character) or "
                      "non-null (binary) characters or bytes.",
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_TIMESTAMP_WITH_TIMEZONE -> SQL_VARCHAR
 */
ZLV_DECLARE_C_TO_SQL( TimestampTz, VarChar )
{
    stlInt32 sLength = 0;
    stlChar  sTimestampTzString[ZLV_TIMESTAMP_WITH_TIMEZONE_STRING_LENGTH];
    stlBool  sSuccessWithInfo;
    SQL_TIMESTAMP_WITH_TIMEZONE_STRUCT * sParameter;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mLength,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_OCTETS,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_TIMESTAMP_WITH_TIMEZONE_STRUCT*)aParameterValuePtr;

    STL_TRY_THROW( dtlIsValidateTimestamp( sParameter->year,
                                           sParameter->month,
                                           sParameter->day,
                                           sParameter->hour,
                                           sParameter->minute,
                                           sParameter->second,
                                           sParameter->fraction / 1000 ) == STL_TRUE,
                   RAMP_ERR_DATETIME_FIELD_OVERFLOW );

    if( sParameter->fraction == 0 )
    {
        STL_TRY_THROW( aIpdRec->mOctetLength >= 28,
                       RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED );

        if( sParameter->timezone_hour < 0 )
        {
            sLength = stlSnprintf( sTimestampTzString,
                                   ZLV_TIMESTAMP_WITH_TIMEZONE_STRING_LENGTH,
                                   "%04d-%02u-%02u %02u:%02u:%02u -%02d:%02d",
                                   sParameter->year,
                                   sParameter->month,
                                   sParameter->day,
                                   sParameter->hour,
                                   sParameter->minute,
                                   sParameter->second,
                                   stlAbsInt32( sParameter->timezone_hour ),
                                   stlAbsInt32( sParameter->timezone_minute ) );
        }
        else if( sParameter->timezone_hour > 0 )
        {
            sLength = stlSnprintf( sTimestampTzString,
                                   ZLV_TIMESTAMP_WITH_TIMEZONE_STRING_LENGTH,
                                   "%04d-%02u-%02u %02u:%02u:%02u +%02d:%02d",
                                   sParameter->year,
                                   sParameter->month,
                                   sParameter->day,
                                   sParameter->hour,
                                   sParameter->minute,
                                   sParameter->second,
                                   sParameter->timezone_hour,
                                   sParameter->timezone_minute );
        }
        else
        {
            if( sParameter->timezone_minute < 0 )
            {
                sLength = stlSnprintf( sTimestampTzString,
                                       ZLV_TIMESTAMP_WITH_TIMEZONE_STRING_LENGTH,
                                       "%04d-%02u-%02u %02u:%02u:%02u -%02d:%02d",
                                       sParameter->year,
                                       sParameter->month,
                                       sParameter->day,
                                       sParameter->hour,
                                       sParameter->minute,
                                       sParameter->second,
                                       stlAbsInt32( sParameter->timezone_hour ),
                                       stlAbsInt32( sParameter->timezone_minute ) );
            }
            else if( sParameter->timezone_minute > 0 )
            {
                sLength = stlSnprintf( sTimestampTzString,
                                       ZLV_TIMESTAMP_WITH_TIMEZONE_STRING_LENGTH,
                                       "%04d-%02u-%02u %02u:%02u:%02u +%02d:%02d",
                                       sParameter->year,
                                       sParameter->month,
                                       sParameter->day,
                                       sParameter->hour,
                                       sParameter->minute,
                                       sParameter->second,
                                       sParameter->timezone_hour,
                                       sParameter->timezone_minute );
            }
        }
    }
    else
    {
        STL_TRY_THROW( aIpdRec->mOctetLength >= ZLV_TIMESTAMP_WITH_TIMEZONE_STRING_LENGTH,
                       RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED );

        if( sParameter->timezone_hour < 0 )
        {
            sLength = stlSnprintf( sTimestampTzString,
                                   ZLV_TIMESTAMP_WITH_TIMEZONE_STRING_LENGTH,
                                   "%04d-%02u-%02u %02u:%02u:%02u.%09u -%02d:%02d",
                                   sParameter->year,
                                   sParameter->month,
                                   sParameter->day,
                                   sParameter->hour,
                                   sParameter->minute,
                                   sParameter->second,
                                   sParameter->fraction,
                                   stlAbsInt32( sParameter->timezone_hour ),
                                   stlAbsInt32( sParameter->timezone_minute ) );
        }
        else if( sParameter->timezone_hour > 0 )
        {
            sLength = stlSnprintf( sTimestampTzString,
                                   ZLV_TIMESTAMP_WITH_TIMEZONE_STRING_LENGTH,
                                   "%04d-%02u-%02u %02u:%02u:%02u.%09u +%02d:%02d",
                                   sParameter->year,
                                   sParameter->month,
                                   sParameter->day,
                                   sParameter->hour,
                                   sParameter->minute,
                                   sParameter->second,
                                   sParameter->fraction,
                                   sParameter->timezone_hour,
                                   sParameter->timezone_minute );
        }
        else
        {
            if( sParameter->timezone_minute < 0 )
            {
                sLength = stlSnprintf( sTimestampTzString,
                                       ZLV_TIMESTAMP_WITH_TIMEZONE_STRING_LENGTH,
                                       "%04d-%02u-%02u %02u:%02u:%02u.%09u -%02d:%02d",
                                       sParameter->year,
                                       sParameter->month,
                                       sParameter->day,
                                       sParameter->hour,
                                       sParameter->minute,
                                       sParameter->second,
                                       sParameter->fraction,
                                       stlAbsInt32( sParameter->timezone_hour ),
                                       stlAbsInt32( sParameter->timezone_minute ) );
            }
            else if( sParameter->timezone_minute > 0 )
            {
                sLength = stlSnprintf( sTimestampTzString,
                                       ZLV_TIMESTAMP_WITH_TIMEZONE_STRING_LENGTH,
                                       "%04d-%02u-%02u %02u:%02u:%02u.%09u +%02d:%02d",
                                       sParameter->year,
                                       sParameter->month,
                                       sParameter->day,
                                       sParameter->hour,
                                       sParameter->minute,
                                       sParameter->second,
                                       sParameter->fraction,
                                       sParameter->timezone_hour,
                                       sParameter->timezone_minute );
            }
        }
    }

    STL_TRY( dtlVarcharSetValueFromString( sTimestampTzString,
                                           sLength,
                                           aIpdRec->mLength,
                                           DTL_SCALE_NA,
                                           DTL_STRING_LENGTH_UNIT_OCTETS,
                                           DTL_INTERVAL_INDICATOR_NA,
                                           aIpdRec->mOctetLength,
                                           aDataValue,
                                           &sSuccessWithInfo,
                                           ZLS_STMT_DT_VECTOR(aStmt),
                                           aStmt,
                                           ZLS_STMT_NLS_DT_VECTOR(aStmt),
                                           aStmt,
                                           aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_DATETIME_FIELD_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_STRING_DATA_RIGHT_TRUNCATED,
                      "The prepared statement associated with the StatementHandle contained "
                      "an SQL statement that contained a datetime expression that, "
                      "when computed, resulted in a date, time, or timestamp structure that was invalid.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_STRING_DATA_RIGHT_TRUNCATED,
                      "The assignment of a character or binary value to a column resulted "
                      "in the truncation of nonblank (character) or "
                      "non-null (binary) characters or bytes.",
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_TIMESTAMP_WITH_TIMEZONE -> SQL_LONGVARCHAR
 */
ZLV_DECLARE_C_TO_SQL( TimestampTz, LongVarChar )
{
    stlInt32 sLength = 0;
    stlChar  sTimestampTzString[ZLV_TIMESTAMP_WITH_TIMEZONE_STRING_LENGTH];
    stlBool  sSuccessWithInfo;
    SQL_TIMESTAMP_WITH_TIMEZONE_STRUCT * sParameter;

    sParameter = (SQL_TIMESTAMP_WITH_TIMEZONE_STRUCT*)aParameterValuePtr;

    STL_TRY_THROW( dtlIsValidateTimestamp( sParameter->year,
                                           sParameter->month,
                                           sParameter->day,
                                           sParameter->hour,
                                           sParameter->minute,
                                           sParameter->second,
                                           sParameter->fraction / 1000 ) == STL_TRUE,
                   RAMP_ERR_DATETIME_FIELD_OVERFLOW );

    if( sParameter->fraction == 0 )
    {
        if( sParameter->timezone_hour < 0 )
        {
            sLength = stlSnprintf( sTimestampTzString,
                                   ZLV_TIMESTAMP_WITH_TIMEZONE_STRING_LENGTH,
                                   "%04d-%02u-%02u %02u:%02u:%02u -%02d:%02d",
                                   sParameter->year,
                                   sParameter->month,
                                   sParameter->day,
                                   sParameter->hour,
                                   sParameter->minute,
                                   sParameter->second,
                                   stlAbsInt32( sParameter->timezone_hour ),
                                   stlAbsInt32( sParameter->timezone_minute ) );
        }
        else if( sParameter->timezone_hour > 0 )
        {
            sLength = stlSnprintf( sTimestampTzString,
                                   ZLV_TIMESTAMP_WITH_TIMEZONE_STRING_LENGTH,
                                   "%04d-%02u-%02u %02u:%02u:%02u +%02d:%02d",
                                   sParameter->year,
                                   sParameter->month,
                                   sParameter->day,
                                   sParameter->hour,
                                   sParameter->minute,
                                   sParameter->second,
                                   sParameter->timezone_hour,
                                   sParameter->timezone_minute );
        }
        else
        {
            if( sParameter->timezone_minute < 0 )
            {
                sLength = stlSnprintf( sTimestampTzString,
                                       ZLV_TIMESTAMP_WITH_TIMEZONE_STRING_LENGTH,
                                       "%04d-%02u-%02u %02u:%02u:%02u -%02d:%02d",
                                       sParameter->year,
                                       sParameter->month,
                                       sParameter->day,
                                       sParameter->hour,
                                       sParameter->minute,
                                       sParameter->second,
                                       stlAbsInt32( sParameter->timezone_hour ),
                                       stlAbsInt32( sParameter->timezone_minute ) );
            }
            else if( sParameter->timezone_minute > 0 )
            {
                sLength = stlSnprintf( sTimestampTzString,
                                       ZLV_TIMESTAMP_WITH_TIMEZONE_STRING_LENGTH,
                                       "%04d-%02u-%02u %02u:%02u:%02u +%02d:%02d",
                                       sParameter->year,
                                       sParameter->month,
                                       sParameter->day,
                                       sParameter->hour,
                                       sParameter->minute,
                                       sParameter->second,
                                       sParameter->timezone_hour,
                                       sParameter->timezone_minute );
            }
        }
    }
    else
    {
        if( sParameter->timezone_hour < 0 )
        {
            sLength = stlSnprintf( sTimestampTzString,
                                   ZLV_TIMESTAMP_WITH_TIMEZONE_STRING_LENGTH,
                                   "%04d-%02u-%02u %02u:%02u:%02u.%09u -%02d:%02d",
                                   sParameter->year,
                                   sParameter->month,
                                   sParameter->day,
                                   sParameter->hour,
                                   sParameter->minute,
                                   sParameter->second,
                                   sParameter->fraction,
                                   stlAbsInt32( sParameter->timezone_hour ),
                                   stlAbsInt32( sParameter->timezone_minute ) );
        }
        else if( sParameter->timezone_hour > 0 )
        {
            sLength = stlSnprintf( sTimestampTzString,
                                   ZLV_TIMESTAMP_WITH_TIMEZONE_STRING_LENGTH,
                                   "%04d-%02u-%02u %02u:%02u:%02u.%09u +%02d:%02d",
                                   sParameter->year,
                                   sParameter->month,
                                   sParameter->day,
                                   sParameter->hour,
                                   sParameter->minute,
                                   sParameter->second,
                                   sParameter->fraction,
                                   sParameter->timezone_hour,
                                   sParameter->timezone_minute );
        }
        else
        {
            if( sParameter->timezone_minute < 0 )
            {
                sLength = stlSnprintf( sTimestampTzString,
                                       ZLV_TIMESTAMP_WITH_TIMEZONE_STRING_LENGTH,
                                       "%04d-%02u-%02u %02u:%02u:%02u.%09u -%02d:%02d",
                                       sParameter->year,
                                       sParameter->month,
                                       sParameter->day,
                                       sParameter->hour,
                                       sParameter->minute,
                                       sParameter->second,
                                       sParameter->fraction,
                                       stlAbsInt32( sParameter->timezone_hour ),
                                       stlAbsInt32( sParameter->timezone_minute ) );
            }
            else if( sParameter->timezone_minute > 0 )
            {
                sLength = stlSnprintf( sTimestampTzString,
                                       ZLV_TIMESTAMP_WITH_TIMEZONE_STRING_LENGTH,
                                       "%04d-%02u-%02u %02u:%02u:%02u.%09u +%02d:%02d",
                                       sParameter->year,
                                       sParameter->month,
                                       sParameter->day,
                                       sParameter->hour,
                                       sParameter->minute,
                                       sParameter->second,
                                       sParameter->fraction,
                                       sParameter->timezone_hour,
                                       sParameter->timezone_minute );
            }
        }
    }

    STL_TRY( dtlLongvarcharSetValueFromString( sTimestampTzString,
                                               sLength,
                                               aIpdRec->mLength,
                                               DTL_SCALE_NA,
                                               DTL_STRING_LENGTH_UNIT_OCTETS,
                                               DTL_INTERVAL_INDICATOR_NA,
                                               aIpdRec->mOctetLength,
                                               aDataValue,
                                               &sSuccessWithInfo,
                                               ZLS_STMT_DT_VECTOR(aStmt),
                                               aStmt,
                                               ZLS_STMT_NLS_DT_VECTOR(aStmt),
                                               aStmt,
                                               aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_DATETIME_FIELD_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_STRING_DATA_RIGHT_TRUNCATED,
                      "The prepared statement associated with the StatementHandle contained "
                      "an SQL statement that contained a datetime expression that, "
                      "when computed, resulted in a date, time, or timestamp structure that was invalid.",
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_TIMESTAMP_WITH_TIMEZONE -> SQL_TYPE_DATE
 */
ZLV_DECLARE_C_TO_SQL( TimestampTz, Date )
{
    /*
     * ===================================================
     * Test                                     | SQLSTATE
     * ===================================================
     * Time fields are zero                     | n/a
     * Time fields are nonzero                  | 22008
     * Data value does not contain a valid date | 22007
     * ===================================================
     */

    SQL_TIMESTAMP_WITH_TIMEZONE_STRUCT * sParameter;
    
    sParameter = (SQL_TIMESTAMP_WITH_TIMEZONE_STRUCT*)aParameterValuePtr;
    
    STL_TRY_THROW( sParameter->fraction == 0, RAMP_ERR_DATETIME_FIELD_OVERFLOW );
    
    STL_TRY_THROW( dtlIsValidateDate( sParameter->year,
                                      sParameter->month,
                                      sParameter->day ) == STL_TRUE,
                   RAMP_ERR_INVALID_DATETIME_FORMAT );
    
    STL_TRY_THROW( dtlIsValidateTime( sParameter->hour,
                                      sParameter->minute,
                                      sParameter->second ) == STL_TRUE,
                   RAMP_ERR_INVALID_DATETIME_FORMAT );
    
    STL_TRY( dtlDateSetValueFromTimeInfo( sParameter->year,
                                          sParameter->month,
                                          sParameter->day,
                                          sParameter->hour,
                                          sParameter->minute,
                                          sParameter->second,
                                          aIpdRec->mOctetLength,
                                          aDataValue,
                                          ZLS_STMT_DT_VECTOR(aStmt),
                                          aStmt,
                                          aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_DATETIME_FIELD_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_STRING_DATA_RIGHT_TRUNCATED,
                      "The prepared statement associated with the StatementHandle contained "
                      "an SQL statement that contained a datetime expression that, "
                      "when computed, resulted in a date, time, or timestamp structure that was invalid.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_DATETIME_FORMAT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_DATETIME_FORMAT,
                      "The prepared statement associated with the StatementHandle contained "
                      "an SQL statement that contained a date, time, or timestamp structure "
                      "as a bound parameter, and the parameter was, respectively, "
                      "an invalid date, time, or timestamp.",
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_TIMESTAMP_WITH_TIMEZONE -> SQL_TYPE_TIME
 */
ZLV_DECLARE_C_TO_SQL( TimestampTz, Time )
{
    /*
     * ===================================================
     * Test                                        | SQLSTATE
     * ===================================================
     * Fractional seconds fields are not truncated | n/a
     * Fractional seconds fields are truncated     | 22008
     * Data value is not a valid timestamp         | 22007
     * ===================================================
     */

    SQL_TIMESTAMP_WITH_TIMEZONE_STRUCT * sParameter;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_TIMESTAMP_WITH_TIMEZONE_STRUCT*)aParameterValuePtr;

    STL_TRY_THROW( (sParameter->fraction % 1000) == 0,
                   RAMP_ERR_DATETIME_FIELD_OVERFLOW );
    
    STL_TRY_THROW( dtlIsValidateTime( sParameter->hour,
                                      sParameter->minute,
                                      sParameter->second ) == STL_TRUE,
                   RAMP_ERR_INVALID_DATETIME_FORMAT );

    STL_TRY( dtlTimeSetValueFromTimeInfo( sParameter->hour,
                                          sParameter->minute,
                                          sParameter->second,
                                          sParameter->fraction/1000,
                                          aIpdRec->mPrecision,
                                          aIpdRec->mOctetLength,
                                          aDataValue,
                                          ZLS_STMT_DT_VECTOR(aStmt),
                                          aStmt,
                                          aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_DATETIME_FIELD_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_STRING_DATA_RIGHT_TRUNCATED,
                      "The prepared statement associated with the StatementHandle contained "
                      "an SQL statement that contained a datetime expression that, "
                      "when computed, resulted in a date, time, or timestamp structure that was invalid.",
                      aErrorStack );
    }
    
    STL_CATCH( RAMP_ERR_INVALID_DATETIME_FORMAT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_DATETIME_FORMAT,
                      "The prepared statement associated with the StatementHandle contained "
                      "an SQL statement that contained a date, time, or timestamp structure "
                      "as a bound parameter, and the parameter was, respectively, "
                      "an invalid date, time, or timestamp.",
                      aErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/*
 * SQL_C_TIMESTAMP_WITH_TIMEZONE -> SQL_TYPE_TIME_WITH_TIMEZONE
 */
ZLV_DECLARE_C_TO_SQL( TimestampTz, TimeTz )
{
    SQL_TIMESTAMP_WITH_TIMEZONE_STRUCT * sParameter;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_TIMESTAMP_WITH_TIMEZONE_STRUCT*)aParameterValuePtr;

    STL_TRY_THROW( (sParameter->fraction % 1000) == 0,
                   RAMP_ERR_DATETIME_FIELD_OVERFLOW );
    
    STL_TRY_THROW( dtlIsValidateTime( sParameter->hour,
                                      sParameter->minute,
                                      sParameter->second ) == STL_TRUE,
                   RAMP_ERR_INVALID_DATETIME_FORMAT );

    STL_TRY_THROW( dtlIsValidateTimeZone( sParameter->timezone_hour,
                                          sParameter->timezone_minute )
                   == STL_TRUE,
                   RAMP_ERR_INVALID_DATETIME_FORMAT );
    
    STL_TRY( dtlTimeTzSetValueFromTimeInfo(
                 sParameter->hour,
                 sParameter->minute,
                 sParameter->second,
                 sParameter->fraction/1000,
                 sParameter->timezone_hour,
                 sParameter->timezone_minute,
                 aIpdRec->mPrecision,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 ZLS_STMT_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_DATETIME_FIELD_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_STRING_DATA_RIGHT_TRUNCATED,
                      "The prepared statement associated with the StatementHandle contained "
                      "an SQL statement that contained a datetime expression that, "
                      "when computed, resulted in a date, time, or timestamp structure that was invalid.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_DATETIME_FORMAT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_DATETIME_FORMAT,
                      "The prepared statement associated with the StatementHandle contained "
                      "an SQL statement that contained a date, time, or timestamp structure "
                      "as a bound parameter, and the parameter was, respectively, "
                      "an invalid date, time, or timestamp.",
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_TIMESTAMP_WITH_TIMEZONE -> SQL_TIMESTAMP
 */
ZLV_DECLARE_C_TO_SQL( TimestampTz, Timestamp )
{
    /*
     * =======================================================
     * Test                                         | SQLSTATE
     * =======================================================
     * Fractional seconds fields are not truncated  | n/a
     * Fractional seconds fields are truncated      | 22008
     * Data value does not a valid timestamp        | 22007
     * =======================================================
     */

    SQL_TIMESTAMP_WITH_TIMEZONE_STRUCT * sParameter;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );
    
    sParameter = (SQL_TIMESTAMP_WITH_TIMEZONE_STRUCT*)aParameterValuePtr;
    
    STL_TRY_THROW( (sParameter->fraction % 1000) == 0,
                   RAMP_ERR_DATETIME_FIELD_OVERFLOW );

    STL_TRY_THROW( dtlIsValidateTimestamp( sParameter->year,
                                           sParameter->month,
                                           sParameter->day,
                                           sParameter->hour,
                                           sParameter->minute,
                                           sParameter->second,
                                           sParameter->fraction/1000 ) == STL_TRUE,
                   RAMP_ERR_INVALID_DATETIME_FORMAT );

    STL_TRY( dtlTimestampSetValueFromTimeInfo( sParameter->year,
                                               sParameter->month,
                                               sParameter->day,
                                               sParameter->hour,
                                               sParameter->minute,
                                               sParameter->second,
                                               sParameter->fraction/1000,
                                               aIpdRec->mPrecision,
                                               aIpdRec->mOctetLength,
                                               aDataValue,
                                               ZLS_STMT_DT_VECTOR(aStmt),
                                               aStmt,
                                               aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_DATETIME_FIELD_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_STRING_DATA_RIGHT_TRUNCATED,
                      "The prepared statement associated with the StatementHandle contained "
                      "an SQL statement that contained a datetime expression that, "
                      "when computed, resulted in a date, time, or timestamp structure that was invalid.",
                      aErrorStack );
    }
    
    STL_CATCH( RAMP_ERR_INVALID_DATETIME_FORMAT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_DATETIME_FORMAT,
                      "The prepared statement associated with the StatementHandle contained "
                      "an SQL statement that contained a date, time, or timestamp structure "
                      "as a bound parameter, and the parameter was, respectively, "
                      "an invalid date, time, or timestamp.",
                      aErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/*
 * SQL_C_TIMESTAMP_WITH_TIMEZONE -> SQL_TYPE_TIMESTAMP_WITH_TIMEZONE
 */
ZLV_DECLARE_C_TO_SQL( TimestampTz, TimestampTz )
{
    /*
     * =======================================================
     * Test                                         | SQLSTATE
     * =======================================================
     * Fractional seconds fields are not truncated  | n/a
     * Fractional seconds fields are truncated      | 22008
     * Data value does not a valid timestamp        | 22007
     * =======================================================
     */

    SQL_TIMESTAMP_WITH_TIMEZONE_STRUCT * sParameter;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );
    
    sParameter = (SQL_TIMESTAMP_WITH_TIMEZONE_STRUCT*)aParameterValuePtr;
    
    STL_TRY_THROW( (sParameter->fraction % 1000) == 0,
                   RAMP_ERR_DATETIME_FIELD_OVERFLOW );
    
    STL_TRY_THROW( dtlIsValidateTimestamp( sParameter->year,
                                           sParameter->month,
                                           sParameter->day,
                                           sParameter->hour,
                                           sParameter->minute,
                                           sParameter->second,
                                           sParameter->fraction/1000 ) == STL_TRUE,
                   RAMP_ERR_INVALID_DATETIME_FORMAT );

    STL_TRY_THROW( dtlIsValidateTimeZone( sParameter->timezone_hour,
                                          sParameter->timezone_minute )
                   == STL_TRUE,
                   RAMP_ERR_INVALID_DATETIME_FORMAT );    
    
    STL_TRY( dtlTimestampTzSetValueFromTimeInfo(
                 sParameter->year,
                 sParameter->month,
                 sParameter->day,
                 sParameter->hour,
                 sParameter->minute,
                 sParameter->second,
                 sParameter->fraction/1000,
                 sParameter->timezone_hour,
                 sParameter->timezone_minute,
                 aIpdRec->mPrecision,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 ZLS_STMT_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_DATETIME_FIELD_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_STRING_DATA_RIGHT_TRUNCATED,
                      "The prepared statement associated with the StatementHandle contained "
                      "an SQL statement that contained a datetime expression that, "
                      "when computed, resulted in a date, time, or timestamp structure that was invalid.",
                      aErrorStack );
    }
    
    STL_CATCH( RAMP_ERR_INVALID_DATETIME_FORMAT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_DATETIME_FORMAT,
                      "The prepared statement associated with the StatementHandle contained "
                      "an SQL statement that contained a date, time, or timestamp structure "
                      "as a bound parameter, and the parameter was, respectively, "
                      "an invalid date, time, or timestamp.",
                      aErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/** @} */

