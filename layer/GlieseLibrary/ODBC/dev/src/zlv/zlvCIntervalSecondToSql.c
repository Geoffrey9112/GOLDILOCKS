/*******************************************************************************
 * zlvCIntervalSecondToSql.c
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
 * @file zlvCIntervalSecondToSql.c
 * @brief Gliese API Internal Converting Data from C Interval Day to SQL Data Types Routines.
 */

/**
 * @addtogroup zlvCIntervalSecondToSql
 * @{
 */

/*
 * http://msdn.microsoft.com/en-us/library/windows/desktop/ms716492%28v=VS.85%29.aspx
 */

/*
 * SQL_C_INTERVAL_SECOND -> SQL_CHAR
 */
ZLV_DECLARE_C_TO_SQL( IntervalSecond, Char )
{
    /*
     * ======================================================
     * Test                                        | SQLSTATE
     * ======================================================
     * Column byte length >= Character byte length | n/a
     * Column byte length < Character byte length  | 22001
     * Data value is not a valid interval literal  | 22015
     * ======================================================
     */

    SQL_INTERVAL_STRUCT * sParameter;
    stlInt32              sLength;
    stlChar               sBuffer[DTL_UINT32_STRING_SIZE + 10];
    stlBool               sSuccessWithInfo;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mLength,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_OCTETS,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_INTERVAL_STRUCT*)aParameterValuePtr;

    if( sParameter->interval_sign == SQL_FALSE )
    {
        sLength = stlSnprintf(sBuffer,
                              DTL_UINT32_STRING_SIZE + 10,
                              "%u.%09u",
                              sParameter->intval.day_second.second,
                              sParameter->intval.day_second.fraction );
    }
    else
    {
        sLength = stlSnprintf(sBuffer,
                              DTL_UINT32_STRING_SIZE + 10,
                              "-%u.%09u",
                              sParameter->intval.day_second.second,
                              sParameter->intval.day_second.fraction );
    }
    
    STL_TRY_THROW( sLength <= aIpdRec->mOctetLength,
                   RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED );

    STL_TRY( dtlCharSetValueFromStringWithoutPaddNull( sBuffer,
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
 * SQL_C_INTERVAL_SECOND -> SQL_VARCHAR
 */
ZLV_DECLARE_C_TO_SQL( IntervalSecond, VarChar )
{
    /*
     * ======================================================
     * Test                                        | SQLSTATE
     * ======================================================
     * Column byte length >= Character byte length | n/a
     * Column byte length < Character byte length  | 22001
     * Data value is not a valid interval literal  | 22015
     * ======================================================
     */

    SQL_INTERVAL_STRUCT * sParameter;
    stlInt32              sLength;
    stlChar               sBuffer[DTL_UINT32_STRING_SIZE + 10];
    stlBool               sSuccessWithInfo;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mLength,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_OCTETS,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_INTERVAL_STRUCT*)aParameterValuePtr;

    if( sParameter->interval_sign == SQL_FALSE )
    {
        sLength = stlSnprintf(sBuffer,
                              DTL_UINT32_STRING_SIZE + 10,
                              "%u.%09u",
                              sParameter->intval.day_second.second,
                              sParameter->intval.day_second.fraction );
    }
    else
    {
        sLength = stlSnprintf(sBuffer,
                              DTL_UINT32_STRING_SIZE + 10,
                              "-%u.%09u",
                              sParameter->intval.day_second.second,
                              sParameter->intval.day_second.fraction );
    }
    
    STL_TRY_THROW( sLength <= aIpdRec->mOctetLength,
                   RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED );

    STL_TRY( dtlVarcharSetValueFromString( sBuffer,
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
 * SQL_C_INTERVAL_SECOND -> SQL_LONGVARCHAR
 */
ZLV_DECLARE_C_TO_SQL( IntervalSecond, LongVarChar )
{
    /*
     * ======================================================
     * Test                                        | SQLSTATE
     * ======================================================
     * Column byte length >= Character byte length | n/a
     * Column byte length < Character byte length  | 22001
     * Data value is not a valid interval literal  | 22015
     * ======================================================
     */

    SQL_INTERVAL_STRUCT * sParameter;
    stlInt32              sLength;
    stlChar               sBuffer[DTL_UINT32_STRING_SIZE + 10];
    stlBool               sSuccessWithInfo;

    sParameter = (SQL_INTERVAL_STRUCT*)aParameterValuePtr;

    if( sParameter->interval_sign == SQL_FALSE )
    {
        sLength = stlSnprintf(sBuffer,
                              DTL_UINT32_STRING_SIZE + 10,
                              "%u.%09u",
                              sParameter->intval.day_second.second,
                              sParameter->intval.day_second.fraction );
    }
    else
    {
        sLength = stlSnprintf(sBuffer,
                              DTL_UINT32_STRING_SIZE + 10,
                              "-%u.%09u",
                              sParameter->intval.day_second.second,
                              sParameter->intval.day_second.fraction );
    }
    
    STL_TRY( dtlLongvarcharSetValueFromString( sBuffer,
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

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_INTERVAL_SECOND -> SQL_NUMERIC
 */
ZLV_DECLARE_C_TO_SQL( IntervalSecond, Numeric )
{
    /*
     * ======================================================================
     * Test                                                 | SQLSTATE
     * ======================================================================
     * Conversion of a single-field interval did not result | n/a
     * in truncation of whole digits.                       | 
     * Conversion resulted in truncation of whole digits.   | 22003
     * ======================================================================
     */

    SQL_INTERVAL_STRUCT * sParameter;
    stlInt32              sLength;
    stlChar               sBuffer[DTL_UINT32_STRING_SIZE + 10];
    stlBool               sSuccessWithInfo;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mPrecision,
                                      aIpdRec->mScale,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_INTERVAL_STRUCT*)aParameterValuePtr;

    if( sParameter->interval_sign == SQL_FALSE )
    {
        sLength = stlSnprintf(sBuffer,
                              DTL_UINT32_STRING_SIZE + 10,
                              "%u.%09u",
                              sParameter->intval.day_second.second,
                              sParameter->intval.day_second.fraction );
    }
    else
    {
        sLength = stlSnprintf(sBuffer,
                              DTL_UINT32_STRING_SIZE + 10,
                              "-%u.%09u",
                              sParameter->intval.day_second.second,
                              sParameter->intval.day_second.fraction );
    }
    
    STL_TRY( dtlNumericSetValueFromString( sBuffer,
                                           sLength,
                                           aIpdRec->mPrecision,
                                           aIpdRec->mScale,
                                           DTL_STRING_LENGTH_UNIT_NA,
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

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_INTERVAL_SECOND -> SQL_TINYINT
 */
ZLV_DECLARE_C_TO_SQL( IntervalSecond, TinyInt )
{
    /*
     * ======================================================================
     * Test                                                 | SQLSTATE
     * ======================================================================
     * Conversion of a single-field interval did not result | n/a
     * in truncation of whole digits.                       | 
     * Conversion resulted in truncation of whole digits.   | 22003
     * ======================================================================
     */

    SQL_INTERVAL_STRUCT * sParameter;
    stlBool               sSuccessWithInfo;

    sParameter = (SQL_INTERVAL_STRUCT*)aParameterValuePtr;

    STL_TRY_THROW( sParameter->intval.day_second.fraction == 0,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    STL_TRY_THROW( ( sParameter->intval.day_second.second >= 0 ) &&
                   ( sParameter->intval.day_second.second <= STL_INT8_MAX ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    /*
     * DataType에 TinyInt 타입이 없으므로 SmallInt로 변환한다.
     */
    if( sParameter->interval_sign == SQL_FALSE )
    {
        STL_TRY( dtlSmallIntSetValueFromInteger( sParameter->intval.day_second.second,
                                                 DTL_PRECISION_NA,
                                                 DTL_SCALE_NA,
                                                 DTL_STRING_LENGTH_UNIT_NA,
                                                 DTL_INTERVAL_INDICATOR_NA,
                                                 aIpdRec->mOctetLength,
                                                 aDataValue,
                                                 &sSuccessWithInfo,
                                                 ZLS_STMT_DT_VECTOR(aStmt),
                                                 aStmt,
                                                 aErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( dtlSmallIntSetValueFromInteger( -(stlInt64)(sParameter->intval.day_second.second),
                                                 DTL_PRECISION_NA,
                                                 DTL_SCALE_NA,
                                                 DTL_STRING_LENGTH_UNIT_NA,
                                                 DTL_INTERVAL_INDICATOR_NA,
                                                 aIpdRec->mOctetLength,
                                                 aDataValue,
                                                 &sSuccessWithInfo,
                                                 ZLS_STMT_DT_VECTOR(aStmt),
                                                 aStmt,
                                                 aErrorStack )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                      "The prepared statement associated with the StatementHandle "
                      "contained a bound numeric parameter, and the parameter value "
                      "caused the whole (as opposed to fractional) part of the number "
                      "to be truncated when assigned to the associated table column.",
                      aErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/*
 * SQL_C_INTERVAL_SECOND -> SQL_SMALLINT
 */
ZLV_DECLARE_C_TO_SQL( IntervalSecond, SmallInt )
{
    /*
     * ======================================================================
     * Test                                                 | SQLSTATE
     * ======================================================================
     * Conversion of a single-field interval did not result | n/a
     * in truncation of whole digits.                       | 
     * Conversion resulted in truncation of whole digits.   | 22003
     * ======================================================================
     */

    SQL_INTERVAL_STRUCT * sParameter;
    stlBool               sSuccessWithInfo;

    sParameter = (SQL_INTERVAL_STRUCT*)aParameterValuePtr;

    STL_TRY_THROW( sParameter->intval.day_second.fraction == 0,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    STL_TRY_THROW( ( sParameter->intval.day_second.second >= 0 ) &&
                   ( sParameter->intval.day_second.second <= STL_INT16_MAX ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    if( sParameter->interval_sign == SQL_FALSE )
    {
        STL_TRY( dtlSmallIntSetValueFromInteger( sParameter->intval.day_second.second,
                                                 DTL_PRECISION_NA,
                                                 DTL_SCALE_NA,
                                                 DTL_STRING_LENGTH_UNIT_NA,
                                                 DTL_INTERVAL_INDICATOR_NA,
                                                 aIpdRec->mOctetLength,
                                                 aDataValue,
                                                 &sSuccessWithInfo,
                                                 ZLS_STMT_DT_VECTOR(aStmt),
                                                 aStmt,
                                                 aErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( dtlSmallIntSetValueFromInteger( -(stlInt64)(sParameter->intval.day_second.second),
                                                 DTL_PRECISION_NA,
                                                 DTL_SCALE_NA,
                                                 DTL_STRING_LENGTH_UNIT_NA,
                                                 DTL_INTERVAL_INDICATOR_NA,
                                                 aIpdRec->mOctetLength,
                                                 aDataValue,
                                                 &sSuccessWithInfo,
                                                 ZLS_STMT_DT_VECTOR(aStmt),
                                                 aStmt,
                                                 aErrorStack )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                      "The prepared statement associated with the StatementHandle "
                      "contained a bound numeric parameter, and the parameter value "
                      "caused the whole (as opposed to fractional) part of the number "
                      "to be truncated when assigned to the associated table column.",
                      aErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/*
 * SQL_C_INTERVAL_SECOND -> SQL_INTEGER
 */
ZLV_DECLARE_C_TO_SQL( IntervalSecond, Integer )
{
    /*
     * ======================================================================
     * Test                                                 | SQLSTATE
     * ======================================================================
     * Conversion of a single-field interval did not result | n/a
     * in truncation of whole digits.                       | 
     * Conversion resulted in truncation of whole digits.   | 22003
     * ======================================================================
     */

    SQL_INTERVAL_STRUCT * sParameter;
    stlBool               sSuccessWithInfo;

    sParameter = (SQL_INTERVAL_STRUCT*)aParameterValuePtr;

    STL_TRY_THROW( sParameter->intval.day_second.fraction == 0,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    STL_TRY_THROW( ( sParameter->intval.day_second.second >= 0 ) &&
                   ( sParameter->intval.day_second.second <= STL_INT32_MAX ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    if( sParameter->interval_sign == SQL_FALSE )
    {
        STL_TRY( dtlIntegerSetValueFromInteger( sParameter->intval.day_second.second,
                                                DTL_PRECISION_NA,
                                                DTL_SCALE_NA,
                                                DTL_STRING_LENGTH_UNIT_NA,
                                                DTL_INTERVAL_INDICATOR_NA,
                                                aIpdRec->mOctetLength,
                                                aDataValue,
                                                &sSuccessWithInfo,
                                                ZLS_STMT_DT_VECTOR(aStmt),
                                                aStmt,
                                                aErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( dtlIntegerSetValueFromInteger( -(stlInt64)(sParameter->intval.day_second.second),
                                                DTL_PRECISION_NA,
                                                DTL_SCALE_NA,
                                                DTL_STRING_LENGTH_UNIT_NA,
                                                DTL_INTERVAL_INDICATOR_NA,
                                                aIpdRec->mOctetLength,
                                                aDataValue,
                                                &sSuccessWithInfo,
                                                ZLS_STMT_DT_VECTOR(aStmt),
                                                aStmt,
                                                aErrorStack )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                      "The prepared statement associated with the StatementHandle "
                      "contained a bound numeric parameter, and the parameter value "
                      "caused the whole (as opposed to fractional) part of the number "
                      "to be truncated when assigned to the associated table column.",
                      aErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/*
 * SQL_C_INTERVAL_SECOND -> SQL_BIGINT
 */
ZLV_DECLARE_C_TO_SQL( IntervalSecond, BigInt )
{
    /*
     * ======================================================================
     * Test                                                 | SQLSTATE
     * ======================================================================
     * Conversion of a single-field interval did not result | n/a
     * in truncation of whole digits.                       | 
     * Conversion resulted in truncation of whole digits.   | 22003
     * ======================================================================
     */

    SQL_INTERVAL_STRUCT * sParameter;
    stlBool               sSuccessWithInfo;

    sParameter = (SQL_INTERVAL_STRUCT*)aParameterValuePtr;

    STL_TRY_THROW( sParameter->intval.day_second.fraction == 0,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    if( sParameter->interval_sign == SQL_FALSE )
    {
        STL_TRY( dtlBigIntSetValueFromInteger( sParameter->intval.day_second.second,
                                               DTL_PRECISION_NA,
                                               DTL_SCALE_NA,
                                               DTL_STRING_LENGTH_UNIT_NA,
                                               DTL_INTERVAL_INDICATOR_NA,
                                               aIpdRec->mOctetLength,
                                               aDataValue,
                                               &sSuccessWithInfo,
                                               ZLS_STMT_DT_VECTOR(aStmt),
                                               aStmt,
                                               aErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( dtlBigIntSetValueFromInteger( -(stlInt64)(sParameter->intval.day_second.second),
                                               DTL_PRECISION_NA,
                                               DTL_SCALE_NA,
                                               DTL_STRING_LENGTH_UNIT_NA,
                                               DTL_INTERVAL_INDICATOR_NA,
                                               aIpdRec->mOctetLength,
                                               aDataValue,
                                               &sSuccessWithInfo,
                                               ZLS_STMT_DT_VECTOR(aStmt),
                                               aStmt,
                                               aErrorStack )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                      "The prepared statement associated with the StatementHandle "
                      "contained a bound numeric parameter, and the parameter value "
                      "caused the whole (as opposed to fractional) part of the number "
                      "to be truncated when assigned to the associated table column.",
                      aErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/*
 * SQL_C_INTERVAL_SECOND -> SQL_INTERVAL_DAY
 */
ZLV_DECLARE_C_TO_SQL( IntervalSecond, IntervalDay )
{
    /*
     * =============================================================================
     * Test                                                               | SQLSTATE
     * =============================================================================
     * Data value was converted without truncation of any fields.         | n/a 
     * One or more fields of data value were truncated during conversion. | 22015
     * =============================================================================
     */

    SQL_INTERVAL_STRUCT * sParameter;
    stlBool               sSuccessWithInfo;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_DAY,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_INTERVAL_STRUCT*)aParameterValuePtr;

    STL_TRY_THROW( ( ( sParameter->intval.day_second.second % ( 60 * 60 * 24 ) ) == 0 ) &&
                   ( sParameter->intval.day_second.fraction == 0 ),
                   RAMP_ERR_INTERVAL_FIELD_OVERFLOW );

    STL_TRY( dtlIntervalDayToSecondSetValueFromTimeInfo( 
                 0,
                 0,
                 0,
                 sParameter->intval.day_second.second,
                 0,
                 sParameter->interval_sign,
                 aIpdRec->mDatetimeIntervalPrecision,
                 DTL_SCALE_NA,
                 DTL_INTERVAL_INDICATOR_DAY,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTERVAL_FIELD_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INTERVAL_FIELD_OVERFLOW,
                      "*StatementText contained an exact numeric or interval parameter that, "
                      "when converted to an interval SQL data type, caused a loss of significant digits.",
                      aErrorStack );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}

/*
 * SQL_C_INTERVAL_SECOND -> SQL_INTERVAL_HOUR
 */
ZLV_DECLARE_C_TO_SQL( IntervalSecond, IntervalHour )
{
    /*
     * =============================================================================
     * Test                                                               | SQLSTATE
     * =============================================================================
     * Data value was converted without truncation of any fields.         | n/a 
     * One or more fields of data value were truncated during conversion. | 22015
     * =============================================================================
     */

    SQL_INTERVAL_STRUCT * sParameter;
    stlBool               sSuccessWithInfo;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_HOUR,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_INTERVAL_STRUCT*)aParameterValuePtr;

    STL_TRY_THROW( ( ( sParameter->intval.day_second.second % ( 60 * 60 ) ) == 0 ) &&
                   ( sParameter->intval.day_second.fraction == 0 ),
                   RAMP_ERR_INTERVAL_FIELD_OVERFLOW );

    STL_TRY( dtlIntervalDayToSecondSetValueFromTimeInfo( 
                 0,
                 0,
                 0,
                 sParameter->intval.day_second.second,
                 0,
                 sParameter->interval_sign,
                 aIpdRec->mDatetimeIntervalPrecision,
                 DTL_SCALE_NA,
                 DTL_INTERVAL_INDICATOR_HOUR,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTERVAL_FIELD_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INTERVAL_FIELD_OVERFLOW,
                      "*StatementText contained an exact numeric or interval parameter that, "
                      "when converted to an interval SQL data type, caused a loss of significant digits.",
                      aErrorStack );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}

/*
 * SQL_C_INTERVAL_SECOND -> SQL_INTERVAL_MINUTE
 */
ZLV_DECLARE_C_TO_SQL( IntervalSecond, IntervalMinute )
{
    /*
     * =============================================================================
     * Test                                                               | SQLSTATE
     * =============================================================================
     * Data value was converted without truncation of any fields.         | n/a 
     * One or more fields of data value were truncated during conversion. | 22015
     * =============================================================================
     */

    SQL_INTERVAL_STRUCT * sParameter;
    stlBool               sSuccessWithInfo;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_MINUTE,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_INTERVAL_STRUCT*)aParameterValuePtr;

    STL_TRY_THROW( ( ( sParameter->intval.day_second.second % 60 ) == 0 ) &&
                   ( sParameter->intval.day_second.fraction == 0 ),
                   RAMP_ERR_INTERVAL_FIELD_OVERFLOW );

    STL_TRY( dtlIntervalDayToSecondSetValueFromTimeInfo( 
                 0,
                 0,
                 0,
                 sParameter->intval.day_second.second,
                 0,
                 sParameter->interval_sign,
                 aIpdRec->mDatetimeIntervalPrecision,
                 DTL_SCALE_NA,
                 DTL_INTERVAL_INDICATOR_MINUTE,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTERVAL_FIELD_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INTERVAL_FIELD_OVERFLOW,
                      "*StatementText contained an exact numeric or interval parameter that, "
                      "when converted to an interval SQL data type, caused a loss of significant digits.",
                      aErrorStack );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}

/*
 * SQL_C_INTERVAL_SECOND -> SQL_INTERVAL_SECOND
 */
ZLV_DECLARE_C_TO_SQL( IntervalSecond, IntervalSecond )
{
    /*
     * =============================================================================
     * Test                                                               | SQLSTATE
     * =============================================================================
     * Data value was converted without truncation of any fields.         | n/a 
     * One or more fields of data value were truncated during conversion. | 22015
     * =============================================================================
     */

    SQL_INTERVAL_STRUCT * sParameter;
    stlBool               sSuccessWithInfo;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      aIpdRec->mPrecision,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_SECOND,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_INTERVAL_STRUCT*)aParameterValuePtr;

    STL_TRY_THROW( ( sParameter->intval.day_second.fraction % 1000 ) == 0,
                   RAMP_ERR_INTERVAL_FIELD_OVERFLOW );

    STL_TRY( dtlIntervalDayToSecondSetValueFromTimeInfo( 
                 0,
                 0,
                 0,
                 sParameter->intval.day_second.second,
                 sParameter->intval.day_second.fraction / 1000,
                 sParameter->interval_sign,
                 aIpdRec->mDatetimeIntervalPrecision,
                 aIpdRec->mPrecision,
                 DTL_INTERVAL_INDICATOR_SECOND,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTERVAL_FIELD_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INTERVAL_FIELD_OVERFLOW,
                      "*StatementText contained an exact numeric or interval parameter that, "
                      "when converted to an interval SQL data type, caused a loss of significant digits.",
                      aErrorStack );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}

/*
 * SQL_C_INTERVAL_SECOND -> SQL_INTERVAL_DAY_TO_HOUR
 */
ZLV_DECLARE_C_TO_SQL( IntervalSecond, IntervalDayToHour )
{
    /*
     * =============================================================================
     * Test                                                               | SQLSTATE
     * =============================================================================
     * Data value was converted without truncation of any fields.         | n/a 
     * One or more fields of data value were truncated during conversion. | 22015
     * =============================================================================
     */

    SQL_INTERVAL_STRUCT * sParameter;
    stlBool               sSuccessWithInfo;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_DAY_TO_HOUR,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_INTERVAL_STRUCT*)aParameterValuePtr;

    STL_TRY_THROW( ( ( sParameter->intval.day_second.second % ( 60 * 60 ) ) == 0 ) &&
                   ( sParameter->intval.day_second.fraction == 0 ),
                   RAMP_ERR_INTERVAL_FIELD_OVERFLOW );

    STL_TRY( dtlIntervalDayToSecondSetValueFromTimeInfo( 
                 0,
                 0,
                 0,
                 sParameter->intval.day_second.second,
                 0,
                 sParameter->interval_sign,
                 aIpdRec->mDatetimeIntervalPrecision,
                 DTL_SCALE_NA,
                 DTL_INTERVAL_INDICATOR_DAY_TO_HOUR,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTERVAL_FIELD_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INTERVAL_FIELD_OVERFLOW,
                      "*StatementText contained an exact numeric or interval parameter that, "
                      "when converted to an interval SQL data type, caused a loss of significant digits.",
                      aErrorStack );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}

/*
 * SQL_C_INTERVAL_SECOND -> SQL_INTERVAL_DAY_TO_MINUTE
 */
ZLV_DECLARE_C_TO_SQL( IntervalSecond, IntervalDayToMinute )
{
    /*
     * =============================================================================
     * Test                                                               | SQLSTATE
     * =============================================================================
     * Data value was converted without truncation of any fields.         | n/a 
     * One or more fields of data value were truncated during conversion. | 22015
     * =============================================================================
     */

    SQL_INTERVAL_STRUCT * sParameter;
    stlBool               sSuccessWithInfo;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_INTERVAL_STRUCT*)aParameterValuePtr;

    STL_TRY_THROW( ( ( sParameter->intval.day_second.second % 60 ) == 0 ) &&
                   ( sParameter->intval.day_second.fraction == 0 ),
                   RAMP_ERR_INTERVAL_FIELD_OVERFLOW );

    STL_TRY( dtlIntervalDayToSecondSetValueFromTimeInfo( 
                 0,
                 0,
                 0,
                 sParameter->intval.day_second.second,
                 0,
                 sParameter->interval_sign,
                 aIpdRec->mDatetimeIntervalPrecision,
                 DTL_SCALE_NA,
                 DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTERVAL_FIELD_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INTERVAL_FIELD_OVERFLOW,
                      "*StatementText contained an exact numeric or interval parameter that, "
                      "when converted to an interval SQL data type, caused a loss of significant digits.",
                      aErrorStack );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}

/*
 * SQL_C_INTERVAL_SECOND -> SQL_INTERVAL_DAY_TO_SECOND
 */
ZLV_DECLARE_C_TO_SQL( IntervalSecond, IntervalDayToSecond )
{
    /*
     * =============================================================================
     * Test                                                               | SQLSTATE
     * =============================================================================
     * Data value was converted without truncation of any fields.         | n/a 
     * One or more fields of data value were truncated during conversion. | 22015
     * =============================================================================
     */

    SQL_INTERVAL_STRUCT * sParameter;
    stlBool               sSuccessWithInfo;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      aIpdRec->mPrecision,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_DAY_TO_SECOND,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_INTERVAL_STRUCT*)aParameterValuePtr;

    STL_TRY_THROW( ( sParameter->intval.day_second.fraction % 1000 ) == 0,
                   RAMP_ERR_INTERVAL_FIELD_OVERFLOW );

    STL_TRY( dtlIntervalDayToSecondSetValueFromTimeInfo( 
                 0,
                 0,
                 0,
                 sParameter->intval.day_second.second,
                 sParameter->intval.day_second.fraction / 1000,
                 sParameter->interval_sign,
                 aIpdRec->mDatetimeIntervalPrecision,
                 aIpdRec->mPrecision,
                 DTL_INTERVAL_INDICATOR_DAY_TO_SECOND,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTERVAL_FIELD_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INTERVAL_FIELD_OVERFLOW,
                      "*StatementText contained an exact numeric or interval parameter that, "
                      "when converted to an interval SQL data type, caused a loss of significant digits.",
                      aErrorStack );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}

/*
 * SQL_C_INTERVAL_SECOND -> SQL_INTERVAL_HOUR_TO_MINUTE
 */
ZLV_DECLARE_C_TO_SQL( IntervalSecond, IntervalHourToMinute )
{
    /*
     * =============================================================================
     * Test                                                               | SQLSTATE
     * =============================================================================
     * Data value was converted without truncation of any fields.         | n/a 
     * One or more fields of data value were truncated during conversion. | 22015
     * =============================================================================
     */

    SQL_INTERVAL_STRUCT * sParameter;
    stlBool               sSuccessWithInfo;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_INTERVAL_STRUCT*)aParameterValuePtr;

    STL_TRY_THROW( ( ( sParameter->intval.day_second.second % 60 ) == 0 ) &&
                   ( sParameter->intval.day_second.fraction == 0 ),
                   RAMP_ERR_INTERVAL_FIELD_OVERFLOW );

    STL_TRY( dtlIntervalDayToSecondSetValueFromTimeInfo( 
                 0,
                 0,
                 0,
                 sParameter->intval.day_second.second,
                 0,
                 sParameter->interval_sign,
                 aIpdRec->mDatetimeIntervalPrecision,
                 DTL_SCALE_NA,
                 DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTERVAL_FIELD_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INTERVAL_FIELD_OVERFLOW,
                      "*StatementText contained an exact numeric or interval parameter that, "
                      "when converted to an interval SQL data type, caused a loss of significant digits.",
                      aErrorStack );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}

/*
 * SQL_C_INTERVAL_SECOND -> SQL_INTERVAL_HOUR_TO_SECOND
 */
ZLV_DECLARE_C_TO_SQL( IntervalSecond, IntervalHourToSecond )
{
    /*
     * =============================================================================
     * Test                                                               | SQLSTATE
     * =============================================================================
     * Data value was converted without truncation of any fields.         | n/a 
     * One or more fields of data value were truncated during conversion. | 22015
     * =============================================================================
     */

    SQL_INTERVAL_STRUCT * sParameter;
    stlBool               sSuccessWithInfo;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      aIpdRec->mPrecision,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_INTERVAL_STRUCT*)aParameterValuePtr;

    STL_TRY_THROW( ( sParameter->intval.day_second.fraction % 1000 ) == 0,
                   RAMP_ERR_INTERVAL_FIELD_OVERFLOW );

    STL_TRY( dtlIntervalDayToSecondSetValueFromTimeInfo( 
                 0,
                 0,
                 0,
                 sParameter->intval.day_second.second,
                 sParameter->intval.day_second.fraction / 1000,
                 sParameter->interval_sign,
                 aIpdRec->mDatetimeIntervalPrecision,
                 aIpdRec->mPrecision,
                 DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTERVAL_FIELD_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INTERVAL_FIELD_OVERFLOW,
                      "*StatementText contained an exact numeric or interval parameter that, "
                      "when converted to an interval SQL data type, caused a loss of significant digits.",
                      aErrorStack );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}

/*
 * SQL_C_INTERVAL_SECOND -> SQL_INTERVAL_MINUTE_TO_SECOND
 */
ZLV_DECLARE_C_TO_SQL( IntervalSecond, IntervalMinuteToSecond )
{
    /*
     * =============================================================================
     * Test                                                               | SQLSTATE
     * =============================================================================
     * Data value was converted without truncation of any fields.         | n/a 
     * One or more fields of data value were truncated during conversion. | 22015
     * =============================================================================
     */

    SQL_INTERVAL_STRUCT * sParameter;
    stlBool               sSuccessWithInfo;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      aIpdRec->mPrecision,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_INTERVAL_STRUCT*)aParameterValuePtr;

    STL_TRY_THROW( ( sParameter->intval.day_second.fraction % 1000 ) == 0,
                   RAMP_ERR_INTERVAL_FIELD_OVERFLOW );

    STL_TRY( dtlIntervalDayToSecondSetValueFromTimeInfo( 
                 0,
                 0,
                 0,
                 sParameter->intval.day_second.second,
                 sParameter->intval.day_second.fraction / 1000,
                 sParameter->interval_sign,
                 aIpdRec->mDatetimeIntervalPrecision,
                 aIpdRec->mPrecision,
                 DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTERVAL_FIELD_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INTERVAL_FIELD_OVERFLOW,
                      "*StatementText contained an exact numeric or interval parameter that, "
                      "when converted to an interval SQL data type, caused a loss of significant digits.",
                      aErrorStack );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}


/** @} */

