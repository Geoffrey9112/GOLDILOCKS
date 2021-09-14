/*******************************************************************************
 * zlvCTimeTzToSql.c
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
 * @file zlvCTimeTzToSql.c
 * @brief Gliese API Internal Converting Data from C Time With Time Zone to SQL Data Types Routines.
 */

/**
 * @addtogroup zlvCTimeTzToSql
 * @{
 */

/*
 * http://msdn.microsoft.com/en-us/library/windows/desktop/ms714075%28v=VS.85%29.aspx
 */

#define ZLV_TIME_WITH_TIMEZONE_STRING_LENGTH 37

/*
 * SQL_C_TYPE_TIME_WITH_TIMEZONE -> SQL_CHAR
 */
ZLV_DECLARE_C_TO_SQL( TimeTz, Char )
{
    stlChar  sTimeTzString[ZLV_TIME_WITH_TIMEZONE_STRING_LENGTH];
    stlBool  sSuccessWithInfo;
    stlInt32 sLength;
    SQL_TIME_WITH_TIMEZONE_STRUCT * sParameter;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mLength,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_OCTETS,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_TIME_WITH_TIMEZONE_STRUCT*)aParameterValuePtr;

    STL_TRY_THROW( dtlIsValidateTime( sParameter->hour,
                                      sParameter->minute,
                                      sParameter->second ) == STL_TRUE,
                   RAMP_ERR_DATETIME_FIELD_OVERFLOW );

    if( sParameter->fraction == 0 )
    {
        STL_TRY_THROW( aIpdRec->mOctetLength >= 17,
                       RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED );

        if( sParameter->timezone_hour < 0 )
        {
            sLength = stlSnprintf( sTimeTzString,
                                   ZLV_TIME_WITH_TIMEZONE_STRING_LENGTH,
                                   "%02u:%02u:%02u -%02d:%02d",
                                   sParameter->hour,
                                   sParameter->minute,
                                   sParameter->second,
                                   stlAbsInt32( sParameter->timezone_hour ),
                                   stlAbsInt32( sParameter->timezone_minute ) );
        }
        else if( sParameter->timezone_hour > 0 )
        {
            sLength = stlSnprintf( sTimeTzString,
                                   ZLV_TIME_WITH_TIMEZONE_STRING_LENGTH,
                                   "%02u:%02u:%02u +%02d:%02d",
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
                sLength = stlSnprintf( sTimeTzString,
                                       ZLV_TIME_WITH_TIMEZONE_STRING_LENGTH,
                                       "%02u:%02u:%02u -%02d:%02d",
                                       sParameter->hour,
                                       sParameter->minute,
                                       sParameter->second,
                                       stlAbsInt32( sParameter->timezone_hour ),
                                       stlAbsInt32( sParameter->timezone_minute ) );
            }
            else
            {
                sLength = stlSnprintf( sTimeTzString,
                                       ZLV_TIME_WITH_TIMEZONE_STRING_LENGTH,
                                       "%02u:%02u:%02u +%02d:%02d",
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
        STL_TRY_THROW( aIpdRec->mOctetLength >= 26,
                       RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED );

        if( sParameter->timezone_hour < 0 )
        {
            sLength = stlSnprintf( sTimeTzString,
                                   ZLV_TIME_WITH_TIMEZONE_STRING_LENGTH,
                                   "%02u:%02u:%02u.%09u -%02d:%02d",
                                   sParameter->hour,
                                   sParameter->minute,
                                   sParameter->second,
                                   sParameter->fraction,
                                   stlAbsInt32( sParameter->timezone_hour ),
                                   stlAbsInt32( sParameter->timezone_minute ) );
        }
        else if( sParameter->timezone_hour > 0 )
        {
            sLength = stlSnprintf( sTimeTzString,
                                   ZLV_TIME_WITH_TIMEZONE_STRING_LENGTH,
                                   "%02u:%02u:%02u.%09u +%02d:%02d",
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
                sLength = stlSnprintf( sTimeTzString,
                                       ZLV_TIME_WITH_TIMEZONE_STRING_LENGTH,
                                       "%02u:%02u:%02u.%09u -%02d:%02d",
                                       sParameter->hour,
                                       sParameter->minute,
                                       sParameter->second,
                                       sParameter->fraction,
                                       stlAbsInt32( sParameter->timezone_hour ),
                                       stlAbsInt32( sParameter->timezone_minute ) );
            }
            else
            {
                sLength = stlSnprintf( sTimeTzString,
                                       ZLV_TIME_WITH_TIMEZONE_STRING_LENGTH,
                                       "%02u:%02u:%02u.%09u +%02d:%02d",
                                       sParameter->hour,
                                       sParameter->minute,
                                       sParameter->second,
                                       sParameter->fraction,
                                       sParameter->timezone_hour,
                                       sParameter->timezone_minute );
            }
        }
    }
    
    STL_TRY( dtlCharSetValueFromStringWithoutPaddNull( sTimeTzString,
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
 * SQL_C_TYPE_TIME_WITH_TIMEZONE -> SQL_VARCHAR
 */
ZLV_DECLARE_C_TO_SQL( TimeTz, VarChar )
{
    stlChar  sTimeTzString[ZLV_TIME_WITH_TIMEZONE_STRING_LENGTH];
    stlBool  sSuccessWithInfo;
    stlInt32 sLength;
    SQL_TIME_WITH_TIMEZONE_STRUCT * sParameter;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mLength,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_OCTETS,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_TIME_WITH_TIMEZONE_STRUCT*)aParameterValuePtr;

    STL_TRY_THROW( dtlIsValidateTime( sParameter->hour,
                                      sParameter->minute,
                                      sParameter->second ) == STL_TRUE,
                   RAMP_ERR_DATETIME_FIELD_OVERFLOW );

    if( sParameter->fraction == 0 )
    {
        STL_TRY_THROW( aIpdRec->mOctetLength >= 17,
                       RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED );

        if( sParameter->timezone_hour < 0 )
        {
            sLength = stlSnprintf( sTimeTzString,
                                   ZLV_TIME_WITH_TIMEZONE_STRING_LENGTH,
                                   "%02u:%02u:%02u -%02d:%02d",
                                   sParameter->hour,
                                   sParameter->minute,
                                   sParameter->second,
                                   stlAbsInt32( sParameter->timezone_hour ),
                                   stlAbsInt32( sParameter->timezone_minute ) );
        }
        else if( sParameter->timezone_hour > 0 )
        {
            sLength = stlSnprintf( sTimeTzString,
                                   ZLV_TIME_WITH_TIMEZONE_STRING_LENGTH,
                                   "%02u:%02u:%02u +%02d:%02d",
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
                sLength = stlSnprintf( sTimeTzString,
                                       ZLV_TIME_WITH_TIMEZONE_STRING_LENGTH,
                                       "%02u:%02u:%02u -%02d:%02d",
                                       sParameter->hour,
                                       sParameter->minute,
                                       sParameter->second,
                                       stlAbsInt32( sParameter->timezone_hour ),
                                       stlAbsInt32( sParameter->timezone_minute ) );
            }
            else
            {
                sLength = stlSnprintf( sTimeTzString,
                                       ZLV_TIME_WITH_TIMEZONE_STRING_LENGTH,
                                       "%02u:%02u:%02u +%02d:%02d",
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
        STL_TRY_THROW( aIpdRec->mOctetLength >= 26,
                       RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED );

        if( sParameter->timezone_hour < 0 )
        {
            sLength = stlSnprintf( sTimeTzString,
                                   ZLV_TIME_WITH_TIMEZONE_STRING_LENGTH,
                                   "%02u:%02u:%02u.%09u -%02d:%02d",
                                   sParameter->hour,
                                   sParameter->minute,
                                   sParameter->second,
                                   sParameter->fraction,
                                   stlAbsInt32( sParameter->timezone_hour ),
                                   stlAbsInt32( sParameter->timezone_minute ) );
        }
        else if( sParameter->timezone_hour > 0 )
        {
            sLength = stlSnprintf( sTimeTzString,
                                   ZLV_TIME_WITH_TIMEZONE_STRING_LENGTH,
                                   "%02u:%02u:%02u.%09u +%02d:%02d",
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
                sLength = stlSnprintf( sTimeTzString,
                                       ZLV_TIME_WITH_TIMEZONE_STRING_LENGTH,
                                       "%02u:%02u:%02u.%09u -%02d:%02d",
                                       sParameter->hour,
                                       sParameter->minute,
                                       sParameter->second,
                                       sParameter->fraction,
                                       stlAbsInt32( sParameter->timezone_hour ),
                                       stlAbsInt32( sParameter->timezone_minute ) );
            }
            else
            {
                sLength = stlSnprintf( sTimeTzString,
                                       ZLV_TIME_WITH_TIMEZONE_STRING_LENGTH,
                                       "%02u:%02u:%02u.%09u +%02d:%02d",
                                       sParameter->hour,
                                       sParameter->minute,
                                       sParameter->second,
                                       sParameter->fraction,
                                       sParameter->timezone_hour,
                                       sParameter->timezone_minute );
            }
        }
    }
    
    STL_TRY( dtlVarcharSetValueFromString( sTimeTzString,
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
 * SQL_C_TYPE_TIME_WITH_TIMEZONE -> SQL_LONGVARCHAR
 */
ZLV_DECLARE_C_TO_SQL( TimeTz, LongVarChar )
{
    stlChar  sTimeTzString[ZLV_TIME_WITH_TIMEZONE_STRING_LENGTH];
    stlBool  sSuccessWithInfo;
    stlInt32 sLength;
    SQL_TIME_WITH_TIMEZONE_STRUCT * sParameter;

    sParameter = (SQL_TIME_WITH_TIMEZONE_STRUCT*)aParameterValuePtr;

    STL_TRY_THROW( dtlIsValidateTime( sParameter->hour,
                                      sParameter->minute,
                                      sParameter->second ) == STL_TRUE,
                   RAMP_ERR_DATETIME_FIELD_OVERFLOW );

    if( sParameter->fraction == 0 )
    {
        if( sParameter->timezone_hour < 0 )
        {
            sLength = stlSnprintf( sTimeTzString,
                                   ZLV_TIME_WITH_TIMEZONE_STRING_LENGTH,
                                   "%02u:%02u:%02u -%02d:%02d",
                                   sParameter->hour,
                                   sParameter->minute,
                                   sParameter->second,
                                   stlAbsInt32( sParameter->timezone_hour ),
                                   stlAbsInt32( sParameter->timezone_minute ) );
        }
        else if( sParameter->timezone_hour > 0 )
        {
            sLength = stlSnprintf( sTimeTzString,
                                   ZLV_TIME_WITH_TIMEZONE_STRING_LENGTH,
                                   "%02u:%02u:%02u +%02d:%02d",
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
                sLength = stlSnprintf( sTimeTzString,
                                       ZLV_TIME_WITH_TIMEZONE_STRING_LENGTH,
                                       "%02u:%02u:%02u -%02d:%02d",
                                       sParameter->hour,
                                       sParameter->minute,
                                       sParameter->second,
                                       stlAbsInt32( sParameter->timezone_hour ),
                                       stlAbsInt32( sParameter->timezone_minute ) );
            }
            else
            {
                sLength = stlSnprintf( sTimeTzString,
                                       ZLV_TIME_WITH_TIMEZONE_STRING_LENGTH,
                                       "%02u:%02u:%02u +%02d:%02d",
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
            sLength = stlSnprintf( sTimeTzString,
                                   ZLV_TIME_WITH_TIMEZONE_STRING_LENGTH,
                                   "%02u:%02u:%02u.%09u -%02d:%02d",
                                   sParameter->hour,
                                   sParameter->minute,
                                   sParameter->second,
                                   sParameter->fraction,
                                   stlAbsInt32( sParameter->timezone_hour ),
                                   stlAbsInt32( sParameter->timezone_minute ) );
        }
        else if( sParameter->timezone_hour > 0 )
        {
            sLength = stlSnprintf( sTimeTzString,
                                   ZLV_TIME_WITH_TIMEZONE_STRING_LENGTH,
                                   "%02u:%02u:%02u.%09u +%02d:%02d",
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
                sLength = stlSnprintf( sTimeTzString,
                                       ZLV_TIME_WITH_TIMEZONE_STRING_LENGTH,
                                       "%02u:%02u:%02u.%09u -%02d:%02d",
                                       sParameter->hour,
                                       sParameter->minute,
                                       sParameter->second,
                                       sParameter->fraction,
                                       stlAbsInt32( sParameter->timezone_hour ),
                                       stlAbsInt32( sParameter->timezone_minute ) );
            }
            else
            {
                sLength = stlSnprintf( sTimeTzString,
                                       ZLV_TIME_WITH_TIMEZONE_STRING_LENGTH,
                                       "%02u:%02u:%02u.%09u +%02d:%02d",
                                       sParameter->hour,
                                       sParameter->minute,
                                       sParameter->second,
                                       sParameter->fraction,
                                       sParameter->timezone_hour,
                                       sParameter->timezone_minute );
            }
        }
    }
    
    STL_TRY( dtlLongvarcharSetValueFromString( sTimeTzString,
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
 * SQL_C_TYPE_TIME_WITH_TIMEZONE -> SQL_TYPE_TIME
 */
ZLV_DECLARE_C_TO_SQL( TimeTz, Time )
{
    SQL_TIME_WITH_TIMEZONE_STRUCT * sParameter;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );
    
    sParameter = (SQL_TIME_WITH_TIMEZONE_STRUCT*)aParameterValuePtr;

    STL_TRY_THROW( (sParameter->fraction % 1000) == 0,
                   RAMP_ERR_DATETIME_FIELD_OVERFLOW );
    
    STL_TRY_THROW( dtlIsValidateTime( sParameter->hour,
                                      sParameter->minute,
                                      sParameter->second ) == STL_TRUE,
                   RAMP_ERR_DATETIME_FIELD_OVERFLOW );
    
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
    
    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_TYPE_TIME_WITH_TIMEZONE -> SQL_TYPE_TIME_WITH_TIMEZONE
 */
ZLV_DECLARE_C_TO_SQL( TimeTz, TimeTz )
{
    /*
     * ============================================
     * Test                             | SQLSTATE
     * ============================================
     * Data value is a valid time.      | n/a
     * Data value is not a valid time.  | 22007
     * ============================================
     */

    SQL_TIME_WITH_TIMEZONE_STRUCT * sParameter;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_TIME_WITH_TIMEZONE_STRUCT*)aParameterValuePtr;

    STL_TRY_THROW( (sParameter->fraction % 1000) == 0,
                   RAMP_ERR_DATETIME_FIELD_OVERFLOW );
    
    STL_TRY_THROW( dtlIsValidateTime( sParameter->hour,
                                      sParameter->minute,
                                      sParameter->second ) == STL_TRUE,
                   RAMP_ERR_DATETIME_FIELD_OVERFLOW );
    
    STL_TRY_THROW( dtlIsValidateTimeZone( sParameter->timezone_hour,
                                          sParameter->timezone_minute )
                   == STL_TRUE,
                   RAMP_ERR_DATETIME_FIELD_OVERFLOW );

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

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_TYPE_TIME_WITH_TIMEZONE -> SQL_TIMESTAMP
 */
ZLV_DECLARE_C_TO_SQL( TimeTz, Timestamp )
{
    /*
     * ============================================
     * Test                             | SQLSTATE
     * ============================================
     * Data value is a valid time.      | n/a
     * Data value is not a valid time.  | 22007
     * ============================================
     */

    stlExpTime sExpTime;
    
    SQL_TIME_WITH_TIMEZONE_STRUCT * sParameter;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_TIME_WITH_TIMEZONE_STRUCT*)aParameterValuePtr;

    STL_TRY_THROW( (sParameter->fraction % 1000) == 0,
                   RAMP_ERR_DATETIME_FIELD_OVERFLOW );    

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
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/*
 * SQL_C_TYPE_TIME_WITH_TIMEZONE -> SQL_TYPE_TIMESTAMP_WITH_TIMEZONE
 */
ZLV_DECLARE_C_TO_SQL( TimeTz, TimestampTz )
{
    /*
     * ============================================
     * Test                             | SQLSTATE
     * ============================================
     * Data value is a valid time.      | n/a
     * Data value is not a valid time.  | 22007
     * ============================================
     */

    stlExpTime sExpTime;
    
    SQL_TIME_WITH_TIMEZONE_STRUCT * sParameter;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_TIME_WITH_TIMEZONE_STRUCT*)aParameterValuePtr;

    STL_TRY_THROW( (sParameter->fraction % 1000) == 0,
                   RAMP_ERR_DATETIME_FIELD_OVERFLOW );
    
    STL_TRY_THROW( dtlIsValidateTime( sParameter->hour,
                                      sParameter->minute,
                                      sParameter->second ) == STL_TRUE,
                   RAMP_ERR_DATETIME_FIELD_OVERFLOW );

    STL_TRY_THROW( dtlIsValidateTimeZone( sParameter->timezone_hour,
                                          sParameter->timezone_minute )
                   == STL_TRUE,
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
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/** @} */

