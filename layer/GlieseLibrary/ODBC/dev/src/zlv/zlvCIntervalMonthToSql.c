/*******************************************************************************
 * zlvCIntervalMonthToSql.c
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
 * @file zlvCIntervalMonthToSql.c
 * @brief Gliese API Internal Converting Data from C Interval Month to SQL Data Types Routines.
 */

/**
 * @addtogroup zlvCIntervalMonthToSql
 * @{
 */

/*
 * http://msdn.microsoft.com/en-us/library/windows/desktop/ms713990%28v=VS.85%29.aspx
 */

/*
 * SQL_C_INTERVAL_MONTH -> SQL_CHAR
 */
ZLV_DECLARE_C_TO_SQL( IntervalMonth, Char )
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

    sValue = (stlInt64)(sParameter->intval.year_month.month);

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
 * SQL_C_INTERVAL_MONTH -> SQL_VARCHAR
 */
ZLV_DECLARE_C_TO_SQL( IntervalMonth, VarChar )
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

    sValue = (stlInt64)(sParameter->intval.year_month.month);

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
 * SQL_C_INTERVAL_MONTH -> SQL_LONGVARCHAR
 */
ZLV_DECLARE_C_TO_SQL( IntervalMonth, LongVarChar )
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

    sValue = (stlInt64)(sParameter->intval.year_month.month);

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
 * SQL_C_INTERVAL_MONTH -> SQL_NUMERIC
 */
ZLV_DECLARE_C_TO_SQL( IntervalMonth, Numeric )
{
    /*
     * ======================================================================
     * Test                                                 | SQLSTATE
     * ======================================================================
     * Conversion of a single-field interval did not result | n/a
     * in truncation of whole digits.                       |
     * Conversion resulted in truncation of whole digits  . | 22003
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
        STL_TRY( dtlNumericSetValueFromInteger( sParameter->intval.year_month.month,
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
        STL_TRY( dtlNumericSetValueFromInteger( -(stlInt64)(sParameter->intval.year_month.month),
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
 * SQL_C_INTERVAL_MONTH -> SQL_TINYINT
 */
ZLV_DECLARE_C_TO_SQL( IntervalMonth, TinyInt )
{
    /*
     * ======================================================================
     * Test                                                 | SQLSTATE
     * ======================================================================
     * Conversion of a single-field interval did not result | n/a
     * in truncation of whole digits.                       |
     * Conversion resulted in truncation of whole digits  . | 22003
     * ======================================================================
     */

    SQL_INTERVAL_STRUCT * sParameter;
    stlBool               sSuccessWithInfo;

    sParameter = (SQL_INTERVAL_STRUCT*)aParameterValuePtr;

    STL_TRY_THROW( ( sParameter->intval.year_month.month >= 0 ) &&
                   ( sParameter->intval.year_month.month <= STL_INT8_MAX ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    /*
     * DataType에 TinyInt 타입이 없으므로 SmallInt로 변환한다.
     */
    if( sParameter->interval_sign == SQL_FALSE )
    {
        STL_TRY( dtlSmallIntSetValueFromInteger( sParameter->intval.year_month.month,
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
        STL_TRY( dtlSmallIntSetValueFromInteger( -(stlInt64)(sParameter->intval.year_month.month),
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
 * SQL_C_INTERVAL_MONTH -> SQL_SMALLINT
 */
ZLV_DECLARE_C_TO_SQL( IntervalMonth, SmallInt )
{
    /*
     * ======================================================================
     * Test                                                 | SQLSTATE
     * ======================================================================
     * Conversion of a single-field interval did not result | n/a
     * in truncation of whole digits.                       |
     * Conversion resulted in truncation of whole digits  . | 22003
     * ======================================================================
     */

    SQL_INTERVAL_STRUCT * sParameter;
    stlBool               sSuccessWithInfo;

    sParameter = (SQL_INTERVAL_STRUCT*)aParameterValuePtr;

    STL_TRY_THROW( ( sParameter->intval.year_month.month >= 0 ) &&
                   ( sParameter->intval.year_month.month <= STL_INT16_MAX ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    if( sParameter->interval_sign == SQL_FALSE )
    {
        STL_TRY( dtlSmallIntSetValueFromInteger( sParameter->intval.year_month.month,
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
        STL_TRY( dtlSmallIntSetValueFromInteger( -(stlInt64)(sParameter->intval.year_month.month),
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
 * SQL_C_INTERVAL_MONTH -> SQL_INTEGER
 */
ZLV_DECLARE_C_TO_SQL( IntervalMonth, Integer )
{
    /*
     * ======================================================================
     * Test                                                 | SQLSTATE
     * ======================================================================
     * Conversion of a single-field interval did not result | n/a
     * in truncation of whole digits.                       |
     * Conversion resulted in truncation of whole digits  . | 22003
     * ======================================================================
     */

    SQL_INTERVAL_STRUCT * sParameter;
    stlBool               sSuccessWithInfo;

    sParameter = (SQL_INTERVAL_STRUCT*)aParameterValuePtr;

    STL_TRY_THROW( ( sParameter->intval.year_month.month >= 0 ) &&
                   ( sParameter->intval.year_month.month <= STL_INT32_MAX ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    if( sParameter->interval_sign == SQL_FALSE )
    {
        STL_TRY( dtlIntegerSetValueFromInteger( sParameter->intval.year_month.month,
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
        STL_TRY( dtlIntegerSetValueFromInteger( -(stlInt64)(sParameter->intval.year_month.month),
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
 * SQL_C_INTERVAL_MONTH -> SQL_BIGINT
 */
ZLV_DECLARE_C_TO_SQL( IntervalMonth, BigInt )
{
    /*
     * ======================================================================
     * Test                                                 | SQLSTATE
     * ======================================================================
     * Conversion of a single-field interval did not result | n/a
     * in truncation of whole digits.                       |
     * Conversion resulted in truncation of whole digits  . | 22003
     * ======================================================================
     */

    SQL_INTERVAL_STRUCT * sParameter;
    stlBool               sSuccessWithInfo;

    sParameter = (SQL_INTERVAL_STRUCT*)aParameterValuePtr;

    if( sParameter->interval_sign == SQL_FALSE )
    {
        STL_TRY( dtlBigIntSetValueFromInteger( sParameter->intval.year_month.month,
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
        STL_TRY( dtlBigIntSetValueFromInteger( -(stlInt64)(sParameter->intval.year_month.month),
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
 * SQL_C_INTERVAL_MONTH -> SQL_INTERVAL_MONTH
 */
ZLV_DECLARE_C_TO_SQL( IntervalMonth, IntervalMonth )
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
                                      DTL_INTERVAL_INDICATOR_MONTH,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_INTERVAL_STRUCT*)aParameterValuePtr;

    STL_TRY( dtlIntervalYearToMonthSetValueFromTimeInfo( 
                 0,
                 sParameter->intval.year_month.month,
                 sParameter->interval_sign,
                 aIpdRec->mDatetimeIntervalPrecision,
                 DTL_INTERVAL_INDICATOR_MONTH,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/*
 * SQL_C_INTERVAL_MONTH -> SQL_INTERVAL_YEAR
 */
ZLV_DECLARE_C_TO_SQL( IntervalMonth, IntervalYear )
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
                                      DTL_INTERVAL_INDICATOR_YEAR,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_INTERVAL_STRUCT*)aParameterValuePtr;

    STL_TRY_THROW( ( sParameter->intval.year_month.month % 12 ) == 0,
                   RAMP_ERR_INTERVAL_FIELD_OVERFLOW );

    STL_TRY( dtlIntervalYearToMonthSetValueFromTimeInfo( 
                 0,
                 sParameter->intval.year_month.month,
                 sParameter->interval_sign,
                 aIpdRec->mDatetimeIntervalPrecision,
                 DTL_INTERVAL_INDICATOR_YEAR,
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
 * SQL_C_INTERVAL_MONTH -> SQL_INTERVAL_YEAR_TO_MONTH
 */
ZLV_DECLARE_C_TO_SQL( IntervalMonth, IntervalYearToMonth )
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
                                      DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_INTERVAL_STRUCT*)aParameterValuePtr;

    STL_TRY( dtlIntervalYearToMonthSetValueFromTimeInfo( 
                 0,
                 sParameter->intval.year_month.month,
                 sParameter->interval_sign,
                 aIpdRec->mDatetimeIntervalPrecision,
                 DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/** @} */

