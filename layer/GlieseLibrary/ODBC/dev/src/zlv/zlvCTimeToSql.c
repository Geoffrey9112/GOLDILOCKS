/*******************************************************************************
 * zlvCTimeToSql.c
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
 * @file zlvCTimeToSql.c
 * @brief Gliese API Internal Converting Data from C Time to SQL Data Types Routines.
 */

/**
 * @addtogroup zlvCTimeToSql
 * @{
 */

/*
 * http://msdn.microsoft.com/en-us/library/windows/desktop/ms714075%28v=VS.85%29.aspx
 */

#define ZLV_TIME_STRING_LENGTH 32

/*
 * SQL_C_TYPE_TIME -> SQL_CHAR
 */
ZLV_DECLARE_C_TO_SQL( Time, Char )
{
    /*
     * ===========================================
     * Test                             | SQLSTATE
     * ===========================================
     * Column byte length >= 8          | n/a
     * Column byte length < 8           | 22001
     * Data value is not a valid time.  | 22008
     * ===========================================
     */

    SQL_TIME_STRUCT * sParameter;
    stlChar           sTimeString[ZLV_TIME_STRING_LENGTH];
    stlBool           sSuccessWithInfo;
    stlInt32          sLength;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mLength,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_OCTETS,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_TIME_STRUCT*)aParameterValuePtr;

    STL_TRY_THROW( dtlIsValidateTime( sParameter->hour,
                                      sParameter->minute,
                                      sParameter->second ) == STL_TRUE,
                   RAMP_ERR_DATETIME_FIELD_OVERFLOW );

    STL_TRY_THROW( aIpdRec->mOctetLength >= 8,
                   RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED );

    sLength = stlSnprintf( sTimeString,
                           ZLV_TIME_STRING_LENGTH,
                           "%02u:%02u:%02u",
                           sParameter->hour,
                           sParameter->minute,
                           sParameter->second );
    
    STL_TRY( dtlCharSetValueFromStringWithoutPaddNull( sTimeString,
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
 * SQL_C_TYPE_TIME -> SQL_VARCHAR
 */
ZLV_DECLARE_C_TO_SQL( Time, VarChar )
{
    /*
     * ===========================================
     * Test                             | SQLSTATE
     * ===========================================
     * Column byte length >= 8          | n/a
     * Column byte length < 8           | 22001
     * Data value is not a valid time.  | 22008
     * ===========================================
     */

    SQL_TIME_STRUCT * sParameter;
    stlChar           sTimeString[ZLV_TIME_STRING_LENGTH];
    stlBool           sSuccessWithInfo;
    stlInt32          sLength;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mLength,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_OCTETS,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_TIME_STRUCT*)aParameterValuePtr;

    STL_TRY_THROW( dtlIsValidateTime( sParameter->hour,
                                      sParameter->minute,
                                      sParameter->second ) == STL_TRUE,
                   RAMP_ERR_DATETIME_FIELD_OVERFLOW );

    STL_TRY_THROW( aIpdRec->mOctetLength >= 8,
                   RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED );

    sLength = stlSnprintf( sTimeString,
                           ZLV_TIME_STRING_LENGTH,
                           "%02u:%02u:%02u",
                           sParameter->hour,
                           sParameter->minute,
                           sParameter->second );
    
    STL_TRY( dtlVarcharSetValueFromString( sTimeString,
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
 * SQL_C_TYPE_TIME -> SQL_LONGVARCHAR
 */
ZLV_DECLARE_C_TO_SQL( Time, LongVarChar )
{
    /*
     * ===========================================
     * Test                             | SQLSTATE
     * ===========================================
     * Column byte length >= 8          | n/a
     * Column byte length < 8           | 22001
     * Data value is not a valid time.  | 22008
     * ===========================================
     */

    SQL_TIME_STRUCT * sParameter;
    stlChar           sTimeString[ZLV_TIME_STRING_LENGTH];
    stlBool           sSuccessWithInfo;
    stlInt32          sLength;

    sParameter = (SQL_TIME_STRUCT*)aParameterValuePtr;

    STL_TRY_THROW( dtlIsValidateTime( sParameter->hour,
                                      sParameter->minute,
                                      sParameter->second ) == STL_TRUE,
                   RAMP_ERR_DATETIME_FIELD_OVERFLOW );

    sLength = stlSnprintf( sTimeString,
                           ZLV_TIME_STRING_LENGTH,
                           "%02u:%02u:%02u",
                           sParameter->hour,
                           sParameter->minute,
                           sParameter->second );
    
    STL_TRY( dtlLongvarcharSetValueFromString( sTimeString,
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
 * SQL_C_TYPE_TIME -> SQL_TYPE_TIME
 */
ZLV_DECLARE_C_TO_SQL( Time, Time )
{
    /*
     * ============================================
     * Test                             | SQLSTATE
     * ============================================
     * Data value is a valid time.      | n/a
     * Data value is not a valid time.  | 22007
     * ============================================
     */

    SQL_TIME_STRUCT * sParameter;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );
    
    sParameter = (SQL_TIME_STRUCT*)aParameterValuePtr;
    
    STL_TRY_THROW( dtlIsValidateTime( sParameter->hour,
                                      sParameter->minute,
                                      sParameter->second ) == STL_TRUE,
                   RAMP_ERR_DATETIME_FIELD_OVERFLOW );
    
    STL_TRY( dtlTimeSetValueFromTimeInfo( sParameter->hour,
                                          sParameter->minute,
                                          sParameter->second,
                                          0, /* fraction */
                                          0, /* precision */
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

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_TYPE_TIME -> SQL_TYPE_TIME_TZ
 */
ZLV_DECLARE_C_TO_SQL( Time, TimeTz )
{
    SQL_TIME_STRUCT * sParameter;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );
    
    sParameter = (SQL_TIME_STRUCT*)aParameterValuePtr;
    
    STL_TRY_THROW( dtlIsValidateTime( sParameter->hour,
                                      sParameter->minute,
                                      sParameter->second ) == STL_TRUE,
                   RAMP_ERR_DATETIME_FIELD_OVERFLOW );
    
    STL_TRY( dtlTimeTzSetValueFromTimeInfo(
                 sParameter->hour,
                 sParameter->minute,
                 sParameter->second,
                 0, /* fraction */
                 ((ZLS_STMT_DT_VECTOR(aStmt))->mGetGMTOffsetFunc(aStmt)) / DTL_SECS_PER_HOUR,
                 ((ZLS_STMT_DT_VECTOR(aStmt))->mGetGMTOffsetFunc(aStmt)) % DTL_SECS_PER_HOUR,
                 0, /* precision */
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

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_TYPE_TIME -> SQL_TYPE_TIMESTAMP
 */
ZLV_DECLARE_C_TO_SQL( Time, Timestamp )
{
    /*
     * ============================================
     * Test                             | SQLSTATE
     * ============================================
     * Data value is a valid time.      | n/a
     * Data value is not a valid time.  | 22007
     * ============================================
     */

    SQL_TIME_STRUCT * sParameter;
    stlExpTime        sExpTime;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_TIME_STRUCT*)aParameterValuePtr;

    STL_TRY_THROW( dtlIsValidateTime( sParameter->hour,
                                      sParameter->minute,
                                      sParameter->second ) == STL_TRUE,
                   RAMP_ERR_DATETIME_FIELD_OVERFLOW );

    /*
     * TIMESTAMP의 date portion에는 current date가 설정된다.
     */
    stlMakeExpTimeByLocalTz( &sExpTime,
                             stlNow() );

    STL_TRY( dtlTimestampSetValueFromTimeInfo( sExpTime.mYear + 1900,
                                               sExpTime.mMonth + 1,
                                               sExpTime.mDay,
                                               sParameter->hour,
                                               sParameter->minute,
                                               sParameter->second,
                                               0, /* fraction */
                                               0, /* precision */
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

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_TYPE_TIME -> SQL_TYPE_TIMESTAMP_TZ
 */
ZLV_DECLARE_C_TO_SQL( Time, TimestampTz )
{
    SQL_TIME_STRUCT * sParameter;
    stlExpTime        sExpTime;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_TIME_STRUCT*)aParameterValuePtr;

    STL_TRY_THROW( dtlIsValidateTime( sParameter->hour,
                                      sParameter->minute,
                                      sParameter->second ) == STL_TRUE,
                   RAMP_ERR_DATETIME_FIELD_OVERFLOW );

    /*
     * TIMESTAMP의 date portion에는 current date가 설정된다.
     */
    stlMakeExpTimeByLocalTz( &sExpTime,
                             stlNow() );

    STL_TRY( dtlTimestampTzSetValueFromTimeInfo(
                 sExpTime.mYear + 1900,
                 sExpTime.mMonth + 1,
                 sExpTime.mDay,
                 sParameter->hour,
                 sParameter->minute,
                 sParameter->second,
                 0, /* fraction */
                 ((ZLS_STMT_DT_VECTOR(aStmt))->mGetGMTOffsetFunc(aStmt)) / DTL_SECS_PER_HOUR,
                 ((ZLS_STMT_DT_VECTOR(aStmt))->mGetGMTOffsetFunc(aStmt)) % DTL_SECS_PER_HOUR,
                 0, /* precision */
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

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */

