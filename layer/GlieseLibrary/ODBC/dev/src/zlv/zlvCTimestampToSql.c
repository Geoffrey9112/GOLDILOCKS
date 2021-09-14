/*******************************************************************************
 * zlvCTimestampToSql.c
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
 * @file zlvCTimestampToSql.c
 * @brief Gliese API Internal Converting Data from C Timestamp to SQL Data Types Routines.
 */

/**
 * @addtogroup zlvCTimestampToSql
 * @{
 */

/*
 * http://msdn.microsoft.com/en-us/library/windows/desktop/ms709385%28v=VS.85%29.aspx
 */

#define ZLV_TIMESTAMP_STRING_LENGTH 32

/*
 * SQL_C_TIMESTAMP -> SQL_CHAR
 */
ZLV_DECLARE_C_TO_SQL( Timestamp, Char )
{
    /*
     * ===========================================================
     * Test                                             | SQLSTATE
     * ===========================================================
     * Column byte length >= Character byte length      | n/a
     * 19 <= Column byte length < Character byte length | 22001
     * Column byte length < 19                          | 22001
     * Data value is not a valid timestamp              | 22008
     * ===========================================================
     */

    SQL_TIMESTAMP_STRUCT * sParameter;
    stlInt32               sLength;
    stlChar                sTimestampString[ZLV_TIMESTAMP_STRING_LENGTH];
    stlBool                sSuccessWithInfo;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mLength,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_OCTETS,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_TIMESTAMP_STRUCT*)aParameterValuePtr;

    STL_TRY_THROW( dtlIsValidateTimestamp( sParameter->year,
                                           sParameter->month,
                                           sParameter->day,
                                           sParameter->hour,
                                           sParameter->minute,
                                           sParameter->second,
                                           sParameter->fraction / 1000 ) == STL_TRUE,
                   RAMP_ERR_DATETIME_FIELD_OVERFLOW );

    STL_TRY_THROW( aIpdRec->mOctetLength >= 19,
                   RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED );

    if( sParameter->fraction == 0 )
    {
        sLength = stlSnprintf( sTimestampString,
                               ZLV_TIMESTAMP_STRING_LENGTH,
                               "%04d-%02u-%02u %02u:%02u:%02u",
                               sParameter->year,
                               sParameter->month,
                               sParameter->day,
                               sParameter->hour,
                               sParameter->minute,
                               sParameter->second );
    }
    else
    {
        sLength = stlSnprintf( sTimestampString,
                               ZLV_TIMESTAMP_STRING_LENGTH,
                               "%04d-%02u-%02u %02u:%02u:%02u.%09u",
                               sParameter->year,
                               sParameter->month,
                               sParameter->day,
                               sParameter->hour,
                               sParameter->minute,
                               sParameter->second,
                               sParameter->fraction );
        
        STL_TRY_THROW( sLength <= aIpdRec->mOctetLength,
                       RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED );
    }

    STL_TRY( dtlCharSetValueFromStringWithoutPaddNull( sTimestampString,
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
 * SQL_C_TIMESTAMP -> SQL_VARCHAR
 */
ZLV_DECLARE_C_TO_SQL( Timestamp, VarChar )
{
    /*
     * ===========================================================
     * Test                                             | SQLSTATE
     * ===========================================================
     * Column byte length >= Character byte length      | n/a
     * 19 <= Column byte length < Character byte length | 22001
     * Column byte length < 19                          | 22001
     * Data value is not a valid timestamp              | 22008
     * ===========================================================
     */

    SQL_TIMESTAMP_STRUCT * sParameter;
    stlInt32               sLength;
    stlChar                sTimestampString[ZLV_TIMESTAMP_STRING_LENGTH];
    stlBool                sSuccessWithInfo;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mLength,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_OCTETS,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_TIMESTAMP_STRUCT*)aParameterValuePtr;

    STL_TRY_THROW( dtlIsValidateTimestamp( sParameter->year,
                                           sParameter->month,
                                           sParameter->day,
                                           sParameter->hour,
                                           sParameter->minute,
                                           sParameter->second,
                                           sParameter->fraction / 1000 ) == STL_TRUE,
                   RAMP_ERR_DATETIME_FIELD_OVERFLOW );

    STL_TRY_THROW( aIpdRec->mOctetLength >= 19,
                   RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED );

    if( sParameter->fraction == 0 )
    {
        sLength = stlSnprintf( sTimestampString,
                               ZLV_TIMESTAMP_STRING_LENGTH,
                               "%04d-%02u-%02u %02u:%02u:%02u",
                               sParameter->year,
                               sParameter->month,
                               sParameter->day,
                               sParameter->hour,
                               sParameter->minute,
                               sParameter->second );
    }
    else
    {
        sLength = stlSnprintf( sTimestampString,
                               ZLV_TIMESTAMP_STRING_LENGTH,
                               "%04d-%02u-%02u %02u:%02u:%02u.%09u",
                               sParameter->year,
                               sParameter->month,
                               sParameter->day,
                               sParameter->hour,
                               sParameter->minute,
                               sParameter->second,
                               sParameter->fraction );

        STL_TRY_THROW( sLength <= aIpdRec->mOctetLength,
                       RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED );
    }

    STL_TRY( dtlVarcharSetValueFromString( sTimestampString,
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
 * SQL_C_TIMESTAMP -> SQL_LONGVARCHAR
 */
ZLV_DECLARE_C_TO_SQL( Timestamp, LongVarChar )
{
    /*
     * ===========================================================
     * Test                                             | SQLSTATE
     * ===========================================================
     * Column byte length >= Character byte length      | n/a
     * 19 <= Column byte length < Character byte length | 22001
     * Column byte length < 19                          | 22001
     * Data value is not a valid timestamp              | 22008
     * ===========================================================
     */

    SQL_TIMESTAMP_STRUCT * sParameter;
    stlInt32               sLength;
    stlChar                sTimestampString[ZLV_TIMESTAMP_STRING_LENGTH];
    stlBool                sSuccessWithInfo;

    sParameter = (SQL_TIMESTAMP_STRUCT*)aParameterValuePtr;

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
        sLength = stlSnprintf( sTimestampString,
                               ZLV_TIMESTAMP_STRING_LENGTH,
                               "%04d-%02u-%02u %02u:%02u:%02u",
                               sParameter->year,
                               sParameter->month,
                               sParameter->day,
                               sParameter->hour,
                               sParameter->minute,
                               sParameter->second );
    }
    else
    {
        sLength = stlSnprintf( sTimestampString,
                               ZLV_TIMESTAMP_STRING_LENGTH,
                               "%04d-%02u-%02u %02u:%02u:%02u.%09u",
                               sParameter->year,
                               sParameter->month,
                               sParameter->day,
                               sParameter->hour,
                               sParameter->minute,
                               sParameter->second,
                               sParameter->fraction );
    }

    STL_TRY( dtlLongvarcharSetValueFromString( sTimestampString,
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
 * SQL_C_TIMESTAMP -> SQL_DATE
 */
ZLV_DECLARE_C_TO_SQL( Timestamp, Date )
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

    SQL_TIMESTAMP_STRUCT * sParameter;
    
    sParameter = (SQL_TIMESTAMP_STRUCT*)aParameterValuePtr;
    
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
 * SQL_C_TIMESTAMP -> SQL_TYPE_TIME
 */
ZLV_DECLARE_C_TO_SQL( Timestamp, Time )
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
    
    SQL_TIMESTAMP_STRUCT * sParameter;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );
    
    sParameter = (SQL_TIMESTAMP_STRUCT*)aParameterValuePtr;
    
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
 * SQL_C_TIMESTAMP -> SQL_TYPE_TIME_TZ
 */
ZLV_DECLARE_C_TO_SQL( Timestamp, TimeTz )
{
    SQL_TIMESTAMP_STRUCT * sParameter;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );
    
    sParameter = (SQL_TIMESTAMP_STRUCT*)aParameterValuePtr;
    
    STL_TRY_THROW( (sParameter->fraction % 1000) == 0,
                   RAMP_ERR_DATETIME_FIELD_OVERFLOW );
    
    STL_TRY_THROW( dtlIsValidateTime( sParameter->hour,
                                      sParameter->minute,
                                      sParameter->second ) == STL_TRUE,
                   RAMP_ERR_INVALID_DATETIME_FORMAT );
    
    STL_TRY( dtlTimeTzSetValueFromTimeInfo(
                 sParameter->hour,
                 sParameter->minute,
                 sParameter->second,
                 sParameter->fraction/1000,
                 ((ZLS_STMT_DT_VECTOR(aStmt))->mGetGMTOffsetFunc(aStmt)) / DTL_SECS_PER_HOUR,
                 ((ZLS_STMT_DT_VECTOR(aStmt))->mGetGMTOffsetFunc(aStmt)) % DTL_SECS_PER_HOUR,
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
 * SQL_C_TIMESTAMP -> SQL_TYPE_TIMESTAMP
 */
ZLV_DECLARE_C_TO_SQL( Timestamp, Timestamp )
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

    SQL_TIMESTAMP_STRUCT * sParameter;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );
    
    sParameter = (SQL_TIMESTAMP_STRUCT*)aParameterValuePtr;

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
 * SQL_C_TIMESTAMP -> SQL_TYPE_TIMESTAMP_TZ
 */
ZLV_DECLARE_C_TO_SQL( Timestamp, TimestampTz )
{
    SQL_TIMESTAMP_STRUCT * sParameter;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );
    
    sParameter = (SQL_TIMESTAMP_STRUCT*)aParameterValuePtr;
    
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
    
    STL_TRY( dtlTimestampTzSetValueFromTimeInfo(
                 sParameter->year,
                 sParameter->month,
                 sParameter->day,
                 sParameter->hour,
                 sParameter->minute,
                 sParameter->second,
                 sParameter->fraction/1000,
                 ((ZLS_STMT_DT_VECTOR(aStmt))->mGetGMTOffsetFunc(aStmt)) / DTL_SECS_PER_HOUR,
                 ((ZLS_STMT_DT_VECTOR(aStmt))->mGetGMTOffsetFunc(aStmt)) % DTL_SECS_PER_HOUR,
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

