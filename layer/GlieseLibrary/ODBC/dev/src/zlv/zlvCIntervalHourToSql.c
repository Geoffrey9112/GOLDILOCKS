/*******************************************************************************
 * zlvCIntervalHourToSql.c
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
 * @file zlvCIntervalHourToSql.c
 * @brief Gliese API Internal Converting Data from C Interval Hour to SQL Data Types Routines.
 */

/**
 * @addtogroup zlvCIntervalHourToSql
 * @{
 */

/*
 * http://msdn.microsoft.com/en-us/library/windows/desktop/ms716492%28v=VS.85%29.aspx
 */

/*
 * SQL_C_INTERVAL_HOUR -> SQL_CHAR
 */
ZLV_DECLARE_C_TO_SQL( IntervalHour, Char )
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
    stlInt64              sValue;
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

    sValue = (stlInt64)(sParameter->intval.day_second.hour);

    if( sParameter->interval_sign == SQL_FALSE )
    {
        /* Do Nothing */
    }
    else
    {
        sValue = -sValue;
    }

    STL_TRY( dtlCharSetValueFromIntegerWithoutPaddNull( sValue,
                                                        aIpdRec->mLength,
                                                        DTL_SCALE_NA,
                                                        DTL_STRING_LENGTH_UNIT_OCTETS,
                                                        DTL_INTERVAL_INDICATOR_NA,
                                                        aIpdRec->mOctetLength,
                                                        aDataValue,
                                                        &sSuccessWithInfo,
                                                        ZLS_STMT_DT_VECTOR(aStmt),
                                                        aStmt,
                                                        aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_INTERVAL_HOUR -> SQL_VARCHAR
 */
ZLV_DECLARE_C_TO_SQL( IntervalHour, VarChar )
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
    stlInt64              sValue;
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

    sValue = (stlInt64)(sParameter->intval.day_second.hour);

    if( sParameter->interval_sign == SQL_FALSE )
    {
        /* Do Nothing */
    }
    else
    {
        sValue = -sValue;
    }

    STL_TRY( dtlVarcharSetValueFromInteger( sValue,
                                            aIpdRec->mLength,
                                            DTL_SCALE_NA,
                                            DTL_STRING_LENGTH_UNIT_OCTETS,
                                            DTL_INTERVAL_INDICATOR_NA,
                                            aIpdRec->mOctetLength,
                                            aDataValue,
                                            &sSuccessWithInfo,
                                            ZLS_STMT_DT_VECTOR(aStmt),
                                            aStmt,
                                            aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_INTERVAL_HOUR -> SQL_LONGVARCHAR
 */
ZLV_DECLARE_C_TO_SQL( IntervalHour, LongVarChar )
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
    stlInt64              sValue;
    stlBool               sSuccessWithInfo;

    sParameter = (SQL_INTERVAL_STRUCT*)aParameterValuePtr;

    sValue = (stlInt64)(sParameter->intval.day_second.hour);

    if( sParameter->interval_sign == SQL_FALSE )
    {
        /* Do Nothing */
    }
    else
    {
        sValue = -sValue;
    }

    STL_TRY( dtlLongvarcharSetValueFromInteger( sValue,
                                                aIpdRec->mLength,
                                                DTL_SCALE_NA,
                                                DTL_STRING_LENGTH_UNIT_OCTETS,
                                                DTL_INTERVAL_INDICATOR_NA,
                                                aIpdRec->mOctetLength,
                                                aDataValue,
                                                &sSuccessWithInfo,
                                                ZLS_STMT_DT_VECTOR(aStmt),
                                                aStmt,
                                                aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_INTERVAL_HOUR -> SQL_NUMERIC
 */
ZLV_DECLARE_C_TO_SQL( IntervalHour, Numeric )
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
        STL_TRY( dtlNumericSetValueFromInteger( sParameter->intval.day_second.hour,
                                                aIpdRec->mPrecision,
                                                aIpdRec->mScale,
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
        STL_TRY( dtlNumericSetValueFromInteger( -(stlInt64)(sParameter->intval.day_second.hour),
                                                aIpdRec->mPrecision,
                                                aIpdRec->mScale,
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

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_INTERVAL_HOUR -> SQL_TINYINT
 */
ZLV_DECLARE_C_TO_SQL( IntervalHour, TinyInt )
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

    STL_TRY_THROW( ( sParameter->intval.day_second.hour >= 0 ) &&
                   ( sParameter->intval.day_second.hour <= STL_INT8_MAX ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    /*
     * DataType에 TinyInt 타입이 없으므로 SmallInt로 변환한다.
     */
    if( sParameter->interval_sign == SQL_FALSE )
    {
        STL_TRY( dtlSmallIntSetValueFromInteger( sParameter->intval.day_second.hour,
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
        STL_TRY( dtlSmallIntSetValueFromInteger( -(stlInt64)(sParameter->intval.day_second.hour),
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
 * SQL_C_INTERVAL_HOUR -> SQL_SMALLINT
 */
ZLV_DECLARE_C_TO_SQL( IntervalHour, SmallInt )
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

    STL_TRY_THROW( ( sParameter->intval.day_second.hour >= 0 ) &&
                   ( sParameter->intval.day_second.hour <= STL_INT16_MAX ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    if( sParameter->interval_sign == SQL_FALSE )
    {
        STL_TRY( dtlSmallIntSetValueFromInteger( sParameter->intval.day_second.hour,
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
        STL_TRY( dtlSmallIntSetValueFromInteger( -(stlInt64)(sParameter->intval.day_second.hour),
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
 * SQL_C_INTERVAL_HOUR -> SQL_INTEGER
 */
ZLV_DECLARE_C_TO_SQL( IntervalHour, Integer )
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

    STL_TRY_THROW( ( sParameter->intval.day_second.hour >= 0 ) &&
                   ( sParameter->intval.day_second.hour <= STL_INT32_MAX ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    if( sParameter->interval_sign == SQL_FALSE )
    {
        STL_TRY( dtlIntegerSetValueFromInteger( sParameter->intval.day_second.hour,
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
        STL_TRY( dtlIntegerSetValueFromInteger( -(stlInt64)(sParameter->intval.day_second.hour),
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
 * SQL_C_INTERVAL_HOUR -> SQL_BIGINT
 */
ZLV_DECLARE_C_TO_SQL( IntervalHour, BigInt )
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

    if( sParameter->interval_sign == SQL_FALSE )
    {
        STL_TRY( dtlBigIntSetValueFromInteger( sParameter->intval.day_second.hour,
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
        STL_TRY( dtlBigIntSetValueFromInteger( -(stlInt64)(sParameter->intval.day_second.hour),
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

    STL_FINISH;
    
    return STL_FAILURE;
}

/*
 * SQL_C_INTERVAL_HOUR -> SQL_INTERVAL_DAY
 */
ZLV_DECLARE_C_TO_SQL( IntervalHour, IntervalDay )
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

    STL_TRY_THROW( ( sParameter->intval.day_second.hour % 24 ) == 0,
                   RAMP_ERR_INTERVAL_FIELD_OVERFLOW );

    STL_TRY( dtlIntervalDayToSecondSetValueFromTimeInfo( 
                 0,
                 sParameter->intval.day_second.hour,
                 0,
                 0,
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
 * SQL_C_INTERVAL_HOUR -> SQL_INTERVAL_HOUR
 */
ZLV_DECLARE_C_TO_SQL( IntervalHour, IntervalHour )
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

    STL_TRY( dtlIntervalDayToSecondSetValueFromTimeInfo( 
                 0,
                 sParameter->intval.day_second.hour,
                 0,
                 0,
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

    STL_FINISH;
    
    return STL_FAILURE;
}

/*
 * SQL_C_INTERVAL_HOUR -> SQL_INTERVAL_MINUTE
 */
ZLV_DECLARE_C_TO_SQL( IntervalHour, IntervalMinute )
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

    STL_TRY( dtlIntervalDayToSecondSetValueFromTimeInfo( 
                 0,
                 sParameter->intval.day_second.hour,
                 0,
                 0,
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

    STL_FINISH;
    
    return STL_FAILURE;
}

/*
 * SQL_C_INTERVAL_HOUR -> SQL_INTERVAL_SECOND
 */
ZLV_DECLARE_C_TO_SQL( IntervalHour, IntervalSecond )
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

    STL_TRY( dtlIntervalDayToSecondSetValueFromTimeInfo( 
                 0,
                 sParameter->intval.day_second.hour,
                 0,
                 0,
                 0,
                 sParameter->interval_sign,
                 aIpdRec->mDatetimeIntervalPrecision,
                 aIpdRec->mPrecision,
                 DTL_INTERVAL_INDICATOR_SECOND,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/*
 * SQL_C_INTERVAL_HOUR -> SQL_INTERVAL_DAY_TO_HOUR
 */
ZLV_DECLARE_C_TO_SQL( IntervalHour, IntervalDayToHour )
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

    STL_TRY( dtlIntervalDayToSecondSetValueFromTimeInfo( 
                 0,
                 sParameter->intval.day_second.hour,
                 0,
                 0,
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

    STL_FINISH;
    
    return STL_FAILURE;
}

/*
 * SQL_C_INTERVAL_HOUR -> SQL_INTERVAL_DAY_TO_MINUTE
 */
ZLV_DECLARE_C_TO_SQL( IntervalHour, IntervalDayToMinute )
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

    STL_TRY( dtlIntervalDayToSecondSetValueFromTimeInfo( 
                 0,
                 sParameter->intval.day_second.hour,
                 0,
                 0,
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

    STL_FINISH;
    
    return STL_FAILURE;
}

/*
 * SQL_C_INTERVAL_HOUR -> SQL_INTERVAL_DAY_TO_SECOND
 */
ZLV_DECLARE_C_TO_SQL( IntervalHour, IntervalDayToSecond )
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

    STL_TRY( dtlIntervalDayToSecondSetValueFromTimeInfo( 
                 0,
                 sParameter->intval.day_second.hour,
                 0,
                 0,
                 0,
                 sParameter->interval_sign,
                 aIpdRec->mDatetimeIntervalPrecision,
                 aIpdRec->mPrecision,
                 DTL_INTERVAL_INDICATOR_DAY_TO_SECOND,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/*
 * SQL_C_INTERVAL_HOUR -> SQL_INTERVAL_HOUR_TO_MINUTE
 */
ZLV_DECLARE_C_TO_SQL( IntervalHour, IntervalHourToMinute )
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

    STL_TRY( dtlIntervalDayToSecondSetValueFromTimeInfo( 
                 0,
                 sParameter->intval.day_second.hour,
                 0,
                 0,
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

    STL_FINISH;
    
    return STL_FAILURE;
}

/*
 * SQL_C_INTERVAL_HOUR -> SQL_INTERVAL_HOUR_TO_SECOND
 */
ZLV_DECLARE_C_TO_SQL( IntervalHour, IntervalHourToSecond )
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

    STL_TRY( dtlIntervalDayToSecondSetValueFromTimeInfo( 
                 0,
                 sParameter->intval.day_second.hour,
                 0,
                 0,
                 0,
                 sParameter->interval_sign,
                 aIpdRec->mDatetimeIntervalPrecision,
                 aIpdRec->mPrecision,
                 DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/*
 * SQL_C_INTERVAL_HOUR -> SQL_INTERVAL_MINUTE_TO_SECOND
 */
ZLV_DECLARE_C_TO_SQL( IntervalHour, IntervalMinuteToSecond )
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

    STL_TRY( dtlIntervalDayToSecondSetValueFromTimeInfo( 
                 0,
                 sParameter->intval.day_second.hour,
                 0,
                 0,
                 0,
                 sParameter->interval_sign,
                 aIpdRec->mDatetimeIntervalPrecision,
                 aIpdRec->mPrecision,
                 DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/** @} */

