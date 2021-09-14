/*******************************************************************************
 * zlvCDateToSql.c
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
 * @file zlvCDateToSql.c
 * @brief Gliese API Internal Converting Data from C Date to SQL Data Types Routines.
 */

/**
 * @addtogroup zlvCDateToSql
 * @{
 */

/*
 * http://msdn.microsoft.com/en-us/library/windows/desktop/ms714660%28v=VS.85%29.aspx
 */

/*
 * SYYYY-MM-DD
 * 예) 1999-01-01 AD => 1999-01-01
 *     0001-01-01    => -1999-01-01
 *
 */ 
#define ZLV_DATE_STRING_LENGTH 12

/*
 * SQL_C_TYPE_DATE -> SQL_CHAR
 */
ZLV_DECLARE_C_TO_SQL( Date, Char )
{
    /*
     * ===========================================
     * Test                             | SQLSTATE
     * ===========================================
     * Column byte length >= 10         | n/a
     * Column byte length < 10          | 22001
     * Data value is not a valid date.  | 22008
     * ===========================================
     */

    SQL_DATE_STRUCT * sParameter;
    stlChar           sDateString[ZLV_DATE_STRING_LENGTH];
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

    sParameter = (SQL_DATE_STRUCT*)aParameterValuePtr;

    STL_TRY_THROW( dtlIsValidateDate( sParameter->year,
                                      sParameter->month,
                                      sParameter->day ) == STL_TRUE,
                   RAMP_ERR_DATETIME_FIELD_OVERFLOW );

    STL_TRY_THROW( aIpdRec->mOctetLength >= 10,
                   RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED );

    sLength = stlSnprintf( sDateString,
                           ZLV_DATE_STRING_LENGTH,
                           "%04d-%02u-%02u",
                           sParameter->year,
                           sParameter->month,
                           sParameter->day );

    STL_TRY( dtlCharSetValueFromStringWithoutPaddNull( sDateString,
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
 * SQL_C_TYPE_DATE -> SQL_VARCHAR
 */
ZLV_DECLARE_C_TO_SQL( Date, VarChar )
{
    /*
     * ===========================================
     * Test                             | SQLSTATE
     * ===========================================
     * Column byte length >= 10         | n/a
     * Column byte length < 10          | 22001
     * Data value is not a valid date.  | 22008
     * ===========================================
     */

    SQL_DATE_STRUCT * sParameter;
    stlChar           sDateString[ZLV_DATE_STRING_LENGTH];
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

    sParameter = (SQL_DATE_STRUCT*)aParameterValuePtr;

    STL_TRY_THROW( dtlIsValidateDate( sParameter->year,
                                      sParameter->month,
                                      sParameter->day ) == STL_TRUE,
                   RAMP_ERR_DATETIME_FIELD_OVERFLOW );

    STL_TRY_THROW( aIpdRec->mOctetLength >= 10,
                   RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED );

    sLength = stlSnprintf( sDateString,
                           ZLV_DATE_STRING_LENGTH,
                           "%04d-%02u-%02u",
                           sParameter->year,
                           sParameter->month,
                           sParameter->day );
    
    STL_TRY( dtlVarcharSetValueFromString( sDateString,
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
 * SQL_C_TYPE_DATE -> SQL_LONGVARCHAR
 */
ZLV_DECLARE_C_TO_SQL( Date, LongVarChar )
{
    /*
     * ===========================================
     * Test                             | SQLSTATE
     * ===========================================
     * Column byte length >= 10         | n/a
     * Column byte length < 10          | 22001
     * Data value is not a valid date.  | 22008
     * ===========================================
     */

    SQL_DATE_STRUCT * sParameter;
    stlChar           sDateString[ZLV_DATE_STRING_LENGTH];
    stlBool           sSuccessWithInfo;
    stlInt32          sLength;

    sParameter = (SQL_DATE_STRUCT*)aParameterValuePtr;

    STL_TRY_THROW( dtlIsValidateDate( sParameter->year,
                                      sParameter->month,
                                      sParameter->day ) == STL_TRUE,
                   RAMP_ERR_DATETIME_FIELD_OVERFLOW );

    sLength = stlSnprintf( sDateString,
                           ZLV_DATE_STRING_LENGTH,
                           "%04d-%02u-%02u",
                           sParameter->year,
                           sParameter->month,
                           sParameter->day );
    
    STL_TRY( dtlLongvarcharSetValueFromString( sDateString,
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
 * SQL_C_TYPE_DATE -> SQL_TYPE_DATE
 */
ZLV_DECLARE_C_TO_SQL( Date, Date )
{
    /*
     * ============================================
     * Test                             | SQLSTATE
     * ============================================
     * Data value is a valid date.      | n/a
     * Data value is not a valid date.  | 22007
     * ============================================
     */

    SQL_DATE_STRUCT * sParameter;
    
    sParameter = (SQL_DATE_STRUCT*)aParameterValuePtr;
    
    STL_TRY_THROW( dtlIsValidateDate( sParameter->year,
                                      sParameter->month,
                                      sParameter->day ) == STL_TRUE,
                   RAMP_ERR_DATETIME_FIELD_OVERFLOW );
    
    STL_TRY( dtlDateSetValueFromTimeInfo( sParameter->year,
                                          sParameter->month,
                                          sParameter->day,
                                          0, /* hour */
                                          0, /* minute */
                                          0, /* second */
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
 * SQL_C_TYPE_DATE -> SQL_TYPE_TIMESTAMP
 */
ZLV_DECLARE_C_TO_SQL( Date, Timestamp )
{
    /*
     * ============================================
     * Test                             | SQLSTATE
     * ============================================
     * Data value is a valid date.      | n/a
     * Data value is not a valid date.  | 22007
     * ============================================
     */
    
    SQL_DATE_STRUCT * sParameter;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );
    
    sParameter = (SQL_DATE_STRUCT*)aParameterValuePtr;
    
    STL_TRY_THROW( dtlIsValidateDate( sParameter->year,
                                      sParameter->month,
                                      sParameter->day ) == STL_TRUE,
                   RAMP_ERR_DATETIME_FIELD_OVERFLOW );
    
    STL_TRY( dtlTimestampSetValueFromTimeInfo( sParameter->year,
                                               sParameter->month,
                                               sParameter->day,
                                               0, /* hour */
                                               0, /* minute */
                                               0, /* second */
                                               0, /* fraction */
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

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_TYPE_DATE -> SQL_TYPE_TIMESTAMP_TZ
 */
ZLV_DECLARE_C_TO_SQL( Date, TimestampTz )
{
    /*
     * ============================================
     * Test                             | SQLSTATE
     * ============================================
     * Data value is a valid date.      | n/a
     * Data value is not a valid date.  | 22007
     * ============================================
     */

    SQL_DATE_STRUCT * sParameter;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_DATE_STRUCT*)aParameterValuePtr;
    
    STL_TRY_THROW( dtlIsValidateDate( sParameter->year,
                                      sParameter->month,
                                      sParameter->day ) == STL_TRUE,
                   RAMP_ERR_DATETIME_FIELD_OVERFLOW );
    
    STL_TRY( dtlTimestampTzSetValueFromTimeInfo(
                 sParameter->year,
                 sParameter->month,
                 sParameter->day,
                 0, /* hour */
                 0, /* minute */
                 0, /* second */
                 0, /* fraction */
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
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/** @} */

