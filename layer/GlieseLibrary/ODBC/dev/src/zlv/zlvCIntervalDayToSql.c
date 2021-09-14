/*******************************************************************************
 * zlvCIntervalDayToSql.c
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
 * @file zlvCIntervalDayToSql.c
 * @brief Gliese API Internal Converting Data from C Interval Day to SQL Data Types Routines.
 */

/**
 * @addtogroup zlvCIntervalDayToSql
 * @{
 */

/*
 * http://msdn.microsoft.com/en-us/library/windows/desktop/ms716492%28v=VS.85%29.aspx
 */

/*
 * SQL_C_INTERVAL_DAY -> SQL_CHAR
 */
ZLV_DECLARE_C_TO_SQL( IntervalDay, Char )
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

    sValue = (stlInt64)(sParameter->intval.day_second.day);

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
 * SQL_C_INTERVAL_DAY -> SQL_VARCHAR
 */
ZLV_DECLARE_C_TO_SQL( IntervalDay, VarChar )
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

    sValue = (stlInt64)(sParameter->intval.day_second.day);

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
 * SQL_C_INTERVAL_DAY -> SQL_LONGVARCHAR
 */
ZLV_DECLARE_C_TO_SQL( IntervalDay, LongVarChar )
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

    sValue = (stlInt64)(sParameter->intval.day_second.day);

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
 * SQL_C_INTERVAL_DAY -> SQL_NUMERIC
 */
ZLV_DECLARE_C_TO_SQL( IntervalDay, Numeric )
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
        STL_TRY( dtlNumericSetValueFromInteger( sParameter->intval.day_second.day,
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
        STL_TRY( dtlNumericSetValueFromInteger( -(stlInt64)(sParameter->intval.day_second.day),
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
 * SQL_C_INTERVAL_DAY -> SQL_TINYINT
 */
ZLV_DECLARE_C_TO_SQL( IntervalDay, TinyInt )
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

    STL_TRY_THROW( ( sParameter->intval.day_second.day >= 0 ) &&
                   ( sParameter->intval.day_second.day <= STL_INT8_MAX ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    /*
     * DataType에 TinyInt 타입이 없으므로 SmallInt로 변환한다.
     */
    if( sParameter->interval_sign == SQL_FALSE )
    {
        STL_TRY( dtlSmallIntSetValueFromInteger( sParameter->intval.day_second.day,
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
        STL_TRY( dtlSmallIntSetValueFromInteger( -(stlInt64)(sParameter->intval.day_second.day),
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
 * SQL_C_INTERVAL_DAY -> SQL_SMALLINT
 */
ZLV_DECLARE_C_TO_SQL( IntervalDay, SmallInt )
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

    STL_TRY_THROW( ( sParameter->intval.day_second.day >= 0 ) &&
                   ( sParameter->intval.day_second.day <= STL_INT16_MAX ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    if( sParameter->interval_sign == SQL_FALSE )
    {
        STL_TRY( dtlSmallIntSetValueFromInteger( sParameter->intval.day_second.day,
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
        STL_TRY( dtlSmallIntSetValueFromInteger( -(stlInt64)(sParameter->intval.day_second.day),
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
 * SQL_C_INTERVAL_DAY -> SQL_INTEGER
 */
ZLV_DECLARE_C_TO_SQL( IntervalDay, Integer )
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

    STL_TRY_THROW( ( sParameter->intval.day_second.day >= 0 ) &&
                   ( sParameter->intval.day_second.day <= STL_INT32_MAX ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    if( sParameter->interval_sign == SQL_FALSE )
    {
        STL_TRY( dtlIntegerSetValueFromInteger( sParameter->intval.day_second.day,
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
        STL_TRY( dtlIntegerSetValueFromInteger( -(stlInt64)(sParameter->intval.day_second.day),
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
 * SQL_C_INTERVAL_DAY -> SQL_BIGINT
 */
ZLV_DECLARE_C_TO_SQL( IntervalDay, BigInt )
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
        STL_TRY( dtlBigIntSetValueFromInteger( sParameter->intval.day_second.day,
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
        STL_TRY( dtlBigIntSetValueFromInteger( -(stlInt64)(sParameter->intval.day_second.day),
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
 * SQL_C_INTERVAL_DAY -> SQL_INTERVAL_DAY
 */
ZLV_DECLARE_C_TO_SQL( IntervalDay, IntervalDay )
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

    STL_TRY( dtlIntervalDayToSecondSetValueFromTimeInfo( 
                 sParameter->intval.day_second.day,
                 0,
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

    STL_FINISH;
    
    return STL_FAILURE;
}

/*
 * SQL_C_INTERVAL_DAY -> SQL_INTERVAL_HOUR
 */
ZLV_DECLARE_C_TO_SQL( IntervalDay, IntervalHour )
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
                 sParameter->intval.day_second.day,
                 0,
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
 * SQL_C_INTERVAL_DAY -> SQL_INTERVAL_MINUTE
 */
ZLV_DECLARE_C_TO_SQL( IntervalDay, IntervalMinute )
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
                 sParameter->intval.day_second.day,
                 0,
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
 * SQL_C_INTERVAL_DAY -> SQL_INTERVAL_SECOND
 */
ZLV_DECLARE_C_TO_SQL( IntervalDay, IntervalSecond )
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
                 sParameter->intval.day_second.day,
                 0,
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
 * SQL_C_INTERVAL_DAY -> SQL_INTERVAL_DAY_TO_HOUR
 */
ZLV_DECLARE_C_TO_SQL( IntervalDay, IntervalDayToHour )
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
                 sParameter->intval.day_second.day,
                 0,
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
 * SQL_C_INTERVAL_DAY -> SQL_INTERVAL_DAY_TO_MINUTE
 */
ZLV_DECLARE_C_TO_SQL( IntervalDay, IntervalDayToMinute )
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
                 sParameter->intval.day_second.day,
                 0,
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
 * SQL_C_INTERVAL_DAY -> SQL_INTERVAL_DAY_TO_SECOND
 */
ZLV_DECLARE_C_TO_SQL( IntervalDay, IntervalDayToSecond )
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
                 sParameter->intval.day_second.day,
                 0,
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
 * SQL_C_INTERVAL_DAY -> SQL_INTERVAL_HOUR_TO_MINUTE
 */
ZLV_DECLARE_C_TO_SQL( IntervalDay, IntervalHourToMinute )
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
                 sParameter->intval.day_second.day,
                 0,
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
 * SQL_C_INTERVAL_DAY -> SQL_INTERVAL_HOUR_TO_SECOND
 */
ZLV_DECLARE_C_TO_SQL( IntervalDay, IntervalHourToSecond )
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
                 sParameter->intval.day_second.day,
                 0,
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
 * SQL_C_INTERVAL_DAY -> SQL_INTERVAL_MINUTE_TO_SECOND
 */
ZLV_DECLARE_C_TO_SQL( IntervalDay, IntervalMinuteToSecond )
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
                 sParameter->intval.day_second.day,
                 0,
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

