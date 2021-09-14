/*******************************************************************************
 * zlvCUShortToSql.c
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
 * @file zlvCUShortToSql.c
 * @brief Gliese API Internal Converting Data from C Unsigned Short Integer to SQL Data Types Routines.
 */

/**
 * @addtogroup zlvCUShortToSql
 * @{
 */

/*
 * http://msdn.microsoft.com/en-us/library/windows/desktop/ms714147%28v=VS.85%29.aspx
 */


/*
 * SQL_C_USHORT -> SQL_CHAR
 */
ZLV_DECLARE_C_TO_SQL( UShort, Char )
{
    /*
     * ===================================================================
     * Test                                    | SQLSTATE
     * ===================================================================
     * Number of digits <= Column byte length. | n/a
     * Number of digits > Column byte length.  | 22001
     * ===================================================================
     */

    stlUInt16 * sParameter;
    stlBool     sSuccessWithInfo;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mLength,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_OCTETS,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (stlUInt16*)aParameterValuePtr;

    STL_TRY( dtlCharSetValueFromIntegerWithoutPaddNull( (stlInt64)(*sParameter),
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
 * SQL_C_USHORT -> SQL_VARCHAR
 */
ZLV_DECLARE_C_TO_SQL( UShort, VarChar )
{
    /*
     * ===================================================================
     * Test                                    | SQLSTATE
     * ===================================================================
     * Number of digits <= Column byte length. | n/a
     * Number of digits > Column byte length.  | 22001
     * ===================================================================
     */

    stlUInt16 * sParameter;
    stlBool     sSuccessWithInfo;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mLength,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_OCTETS,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (stlUInt16*)aParameterValuePtr;

    STL_TRY( dtlVarcharSetValueFromInteger( (stlInt64)(*sParameter),
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
 * SQL_C_USHORT -> SQL_LONGVARCHAR
 */
ZLV_DECLARE_C_TO_SQL( UShort, LongVarChar )
{
    /*
     * ===================================================================
     * Test                                    | SQLSTATE
     * ===================================================================
     * Number of digits <= Column byte length. | n/a
     * Number of digits > Column byte length.  | 22001
     * ===================================================================
     */

    stlUInt16 * sParameter;
    stlBool     sSuccessWithInfo;

    sParameter = (stlUInt16*)aParameterValuePtr;

    STL_TRY( dtlLongvarcharSetValueFromInteger( (stlInt64)(*sParameter),
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
 * SQL_C_USHORT -> SQL_FLOAT
 */
ZLV_DECLARE_C_TO_SQL( UShort, Float )
{
    /*
     * ======================================================================
     * Test                                                 | SQLSTATE
     * ======================================================================
     * Data converted without truncation or                 | n/a
     * with truncated of fractional digits.                 |
     * Data converted with truncation of whole digits.      | 22003
     * ======================================================================
     */

    stlBool sSuccessWithInfo;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    STL_TRY( dtlFloatSetValueFromInteger( *(stlUInt16*)aParameterValuePtr,
                                          aIpdRec->mPrecision,
                                          DTL_SCALE_NA,
                                          DTL_STRING_LENGTH_UNIT_NA,
                                          DTL_INTERVAL_INDICATOR_NA,
                                          aIpdRec->mOctetLength,
                                          aDataValue,
                                          &sSuccessWithInfo,
                                          ZLS_STMT_DT_VECTOR(aStmt),
                                          aStmt,
                                          aErrorStack ) == STL_SUCCESS );
                                     
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_USHORT -> SQL_NUMERIC
 */
ZLV_DECLARE_C_TO_SQL( UShort, Numeric )
{
    /*
     * ======================================================================
     * Test                                                 | SQLSTATE
     * ======================================================================
     * Data converted without truncation or                 | n/a
     * with truncated of fractional digits.                 |
     * Data converted with truncation of whole digits.      | 22003
     * ======================================================================
     */

    stlBool sSuccessWithInfo;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mPrecision,
                                      aIpdRec->mScale,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    STL_TRY( dtlNumericSetValueFromInteger( *(stlUInt16*)aParameterValuePtr,
                                            aIpdRec->mPrecision,
                                            aIpdRec->mScale,
                                            DTL_STRING_LENGTH_UNIT_NA,
                                            DTL_INTERVAL_INDICATOR_NA,
                                            aIpdRec->mOctetLength,
                                            aDataValue,
                                            &sSuccessWithInfo,
                                            ZLS_STMT_DT_VECTOR(aStmt),
                                            aStmt,
                                            aErrorStack ) == STL_SUCCESS );
                                     
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_USHORT -> SQL_TINYINT
 */
ZLV_DECLARE_C_TO_SQL( UShort, TinyInt )
{
    /*
     * ======================================================================
     * Test                                                 | SQLSTATE
     * ======================================================================
     * Data converted without truncation or                 | n/a
     * with truncated of fractional digits.                 |
     * Data converted with truncation of whole digits.      | 22003
     * ======================================================================
     */

    stlUInt16 * sParameter;
    stlBool     sSuccessWithInfo;

    sParameter = (stlUInt16*)aParameterValuePtr;

    STL_TRY_THROW( *sParameter <= STL_INT8_MAX,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    /*
     * DataType에 TinyInt 타입이 없으므로 SmallInt로 변환한다.
     */
    STL_TRY( dtlSmallIntSetValueFromInteger( *sParameter,
                                             DTL_PRECISION_NA,
                                             DTL_SCALE_NA,
                                             DTL_STRING_LENGTH_UNIT_NA,
                                             DTL_INTERVAL_INDICATOR_NA,
                                             aIpdRec->mOctetLength,
                                             aDataValue,
                                             &sSuccessWithInfo,
                                             ZLS_STMT_DT_VECTOR(aStmt),
                                             aStmt,
                                             aErrorStack ) == STL_SUCCESS );

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
 * SQL_C_USHORT -> SQL_SMALLINT
 */
ZLV_DECLARE_C_TO_SQL( UShort, SmallInt )
{
    /*
     * ======================================================================
     * Test                                                 | SQLSTATE
     * ======================================================================
     * Data converted without truncation or                 | n/a
     * with truncated of fractional digits.                 |
     * Data converted with truncation of whole digits.      | 22003
     * ======================================================================
     */

    stlUInt16 * sParameter;
    stlBool     sSuccessWithInfo;

    sParameter = (stlUInt16*)aParameterValuePtr;

    STL_TRY_THROW( *sParameter <= STL_INT16_MAX,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    STL_TRY( dtlSmallIntSetValueFromInteger( *sParameter,
                                             DTL_PRECISION_NA,
                                             DTL_SCALE_NA,
                                             DTL_STRING_LENGTH_UNIT_NA,
                                             DTL_INTERVAL_INDICATOR_NA,
                                             aIpdRec->mOctetLength,
                                             aDataValue,
                                             &sSuccessWithInfo,
                                             ZLS_STMT_DT_VECTOR(aStmt),
                                             aStmt,
                                             aErrorStack ) == STL_SUCCESS );

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
 * SQL_C_USHORT -> SQL_INTEGER
 */
ZLV_DECLARE_C_TO_SQL( UShort, Integer )
{
    /*
     * ======================================================================
     * Test                                                 | SQLSTATE
     * ======================================================================
     * Data converted without truncation or                 | n/a
     * with truncated of fractional digits.                 |
     * Data converted with truncation of whole digits.      | 22003
     * ======================================================================
     */

    stlBool sSuccessWithInfo;

    STL_TRY( dtlIntegerSetValueFromInteger( *(stlUInt16*)aParameterValuePtr,
                                            DTL_PRECISION_NA,
                                            DTL_SCALE_NA,
                                            DTL_STRING_LENGTH_UNIT_NA,
                                            DTL_INTERVAL_INDICATOR_NA,
                                            aIpdRec->mOctetLength,
                                            aDataValue,
                                            &sSuccessWithInfo,
                                            ZLS_STMT_DT_VECTOR(aStmt),
                                            aStmt,
                                            aErrorStack ) == STL_SUCCESS );
                                     
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_USHORT -> SQL_BIGINT
 */
ZLV_DECLARE_C_TO_SQL( UShort, BigInt )
{
    /*
     * ======================================================================
     * Test                                                 | SQLSTATE
     * ======================================================================
     * Data converted without truncation or                 | n/a
     * with truncated of fractional digits.                 |
     * Data converted with truncation of whole digits.      | 22003
     * ======================================================================
     */

    stlBool sSuccessWithInfo;

    STL_TRY( dtlBigIntSetValueFromInteger( *(stlUInt16*)aParameterValuePtr,
                                           DTL_PRECISION_NA,
                                           DTL_SCALE_NA,
                                           DTL_STRING_LENGTH_UNIT_NA,
                                           DTL_INTERVAL_INDICATOR_NA,
                                           aIpdRec->mOctetLength,
                                           aDataValue,
                                           &sSuccessWithInfo,
                                           ZLS_STMT_DT_VECTOR(aStmt),
                                           aStmt,
                                           aErrorStack ) == STL_SUCCESS );
                                     
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_USHORT -> SQL_REAL
 */
ZLV_DECLARE_C_TO_SQL( UShort, Real )
{
    /*
     * ======================================================================
     * Test                                                 | SQLSTATE
     * ======================================================================
     * Data is within the range of the data type to         | n/a
     * which the number is being converted                  | 
     * Data is outside the range of the data type to        | 22003
     * which the number is being converted                  |
     * ======================================================================
     */

    stlBool sSuccessWithInfo;

    STL_TRY( dtlRealSetValueFromInteger( *(stlUInt16*)aParameterValuePtr,
                                         DTL_PRECISION_NA,
                                         DTL_SCALE_NA,
                                         DTL_STRING_LENGTH_UNIT_NA,
                                         DTL_INTERVAL_INDICATOR_NA,
                                         aIpdRec->mOctetLength,
                                         aDataValue,
                                         &sSuccessWithInfo,
                                         ZLS_STMT_DT_VECTOR(aStmt),
                                         aStmt,
                                         aErrorStack ) == STL_SUCCESS );
                                     
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_USHORT -> SQL_DOUBLE
 */
ZLV_DECLARE_C_TO_SQL( UShort, Double )
{
    /*
     * ======================================================================
     * Test                                                 | SQLSTATE
     * ======================================================================
     * Data is within the range of the data type to         | n/a
     * which the number is being converted                  | 
     * Data is outside the range of the data type to        | 22003
     * which the number is being converted                  |
     * ======================================================================
     */

    stlBool sSuccessWithInfo;

    STL_TRY( dtlDoubleSetValueFromInteger( *(stlUInt16*)aParameterValuePtr,
                                           DTL_PRECISION_NA,
                                           DTL_SCALE_NA,
                                           DTL_STRING_LENGTH_UNIT_NA,
                                           DTL_INTERVAL_INDICATOR_NA,
                                           aIpdRec->mOctetLength,
                                           aDataValue,
                                           &sSuccessWithInfo,
                                           ZLS_STMT_DT_VECTOR(aStmt),
                                           aStmt,
                                           aErrorStack ) == STL_SUCCESS );
                                     
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_USHORT -> SQL_BIT
 */
ZLV_DECLARE_C_TO_SQL( UShort, Bit )
{
    /*
     * ======================================================================
     * Test                                                    | SQLSTATE
     * ======================================================================
     * Data is 0 or 1                                          | n/a
     * Data is greater than 0, less than 2, and not equal to 1 | 22001
     * Data is less than 0 or greater than or equal to 2       | 22003
     * ======================================================================
     */

    stlUInt16 * sParameter;
    stlBool     sSuccessWithInfo;

    sParameter = (stlUInt16*)aParameterValuePtr;

    STL_TRY_THROW( ( *sParameter == 0 ) ||
                   ( *sParameter == 1 ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    STL_TRY( dtlBooleanSetValueFromInteger( *sParameter,
                                            DTL_PRECISION_NA,
                                            DTL_SCALE_NA,
                                            DTL_STRING_LENGTH_UNIT_NA,
                                            DTL_INTERVAL_INDICATOR_NA,
                                            aIpdRec->mOctetLength,
                                            aDataValue,
                                            &sSuccessWithInfo,
                                            ZLS_STMT_DT_VECTOR(aStmt),
                                            aStmt,
                                            aErrorStack ) == STL_SUCCESS );

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
 * SQL_C_USHORT -> SQL_BOOLEAN
 */
ZLV_DECLARE_C_TO_SQL( UShort, Boolean )
{
    return zlvCUShortToSqlBit( aStmt,
                               aParameterValuePtr,
                               aIndicator,
                               aApdRec,
                               aIpdRec,
                               aDataValue,
                               aSuccessWithInfo,
                               aErrorStack );
}

/*
 * SQL_C_USHORT -> SQL_INTERVAL_YEAR
 */
ZLV_DECLARE_C_TO_SQL( UShort, IntervalYear )
{
    /*
     * ================================
     * Test                | SQLSTATE
     * ================================
     * Data not truncated. | n/a 
     * Data truncated.     | 22015
     * ================================
     */

    stlBool sSuccessWithInfo;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_YEAR,
                                      aErrorStack )
             == STL_SUCCESS );

    STL_TRY( dtlIntervalYearToMonthSetValueFromInteger(
                 *(stlUInt16*)aParameterValuePtr,
                 aIpdRec->mDatetimeIntervalPrecision,
                 DTL_SCALE_NA,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_YEAR,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 ZLS_STMT_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_USHORT -> SQL_INTERVAL_MONTH
 */
ZLV_DECLARE_C_TO_SQL( UShort, IntervalMonth )
{
    /*
     * ================================
     * Test                | SQLSTATE
     * ================================
     * Data not truncated. | n/a 
     * Data truncated.     | 22015
     * ================================
     */

    stlBool sSuccessWithInfo;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_MONTH,
                                      aErrorStack )
             == STL_SUCCESS );

    STL_TRY( dtlIntervalYearToMonthSetValueFromInteger(
                 *(stlUInt16*)aParameterValuePtr,
                 aIpdRec->mDatetimeIntervalPrecision,
                 DTL_SCALE_NA,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_MONTH,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 ZLS_STMT_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_USHORT -> SQL_INTERVAL_DAY
 */
ZLV_DECLARE_C_TO_SQL( UShort, IntervalDay )
{
    /*
     * ================================
     * Test                | SQLSTATE
     * ================================
     * Data not truncated. | n/a 
     * Data truncated.     | 22015
     * ================================
     */

    stlBool sSuccessWithInfo;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_DAY,
                                      aErrorStack )
             == STL_SUCCESS );

    STL_TRY( dtlIntervalDayToSecondSetValueFromInteger(
                 *(stlUInt16*)aParameterValuePtr,
                 aIpdRec->mDatetimeIntervalPrecision,
                 DTL_SCALE_NA,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_DAY,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 ZLS_STMT_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_USHORT -> SQL_INTERVAL_HOUR
 */
ZLV_DECLARE_C_TO_SQL( UShort, IntervalHour )
{
    /*
     * ================================
     * Test                | SQLSTATE
     * ================================
     * Data not truncated. | n/a 
     * Data truncated.     | 22015
     * ================================
     */

    stlBool sSuccessWithInfo;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_HOUR,
                                      aErrorStack )
             == STL_SUCCESS );

    STL_TRY( dtlIntervalDayToSecondSetValueFromInteger(
                 *(stlUInt16*)aParameterValuePtr,
                 aIpdRec->mDatetimeIntervalPrecision,
                 DTL_SCALE_NA,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_HOUR,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 ZLS_STMT_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_USHORT -> SQL_INTERVAL_MINUTE
 */
ZLV_DECLARE_C_TO_SQL( UShort, IntervalMinute )
{
    /*
     * ================================
     * Test                | SQLSTATE
     * ================================
     * Data not truncated. | n/a 
     * Data truncated.     | 22015
     * ================================
     */

    stlBool sSuccessWithInfo;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_MINUTE,
                                      aErrorStack )
             == STL_SUCCESS );

    STL_TRY( dtlIntervalDayToSecondSetValueFromInteger(
                 *(stlUInt16*)aParameterValuePtr,
                 aIpdRec->mDatetimeIntervalPrecision,
                 DTL_SCALE_NA,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_MINUTE,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 ZLS_STMT_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_USHORT -> SQL_INTERVAL_SECOND
 */
ZLV_DECLARE_C_TO_SQL( UShort, IntervalSecond )
{
    /*
     * ================================
     * Test                | SQLSTATE
     * ================================
     * Data not truncated. | n/a 
     * Data truncated.     | 22015
     * ================================
     */

    stlBool sSuccessWithInfo;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      aIpdRec->mPrecision,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_SECOND,
                                      aErrorStack )
             == STL_SUCCESS );

    STL_TRY( dtlIntervalDayToSecondSetValueFromInteger(
                 *(stlUInt16*)aParameterValuePtr,
                 aIpdRec->mDatetimeIntervalPrecision,
                 aIpdRec->mPrecision,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_SECOND,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 ZLS_STMT_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */

