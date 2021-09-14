/*******************************************************************************
 * zlvCDoubleToSql.c
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
 * @file zlvCDoubleToSql.c
 * @brief Gliese API Internal Converting Data from C Double to SQL Data Types Routines.
 */

/**
 * @addtogroup zlvCDoubleToSql
 * @{
 */

/*
 * http://msdn.microsoft.com/en-us/library/windows/desktop/ms714147%28v=VS.85%29.aspx
 */


/*
 * SQL_C_DOUBLE -> SQL_CHAR
 */
ZLV_DECLARE_C_TO_SQL( Double, Char )
{
    /*
     * ===================================================================
     * Test                                    | SQLSTATE
     * ===================================================================
     * Number of digits <= Column byte length. | n/a
     * Number of digits > Column byte length.  | 22001
     * ===================================================================
     */
    stlFloat64 * sParameter;
    stlBool      sSuccessWithInfo;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mLength,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_OCTETS,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (stlFloat64*)aParameterValuePtr;

    STL_TRY( dtlCharSetValueFromRealWithoutPaddNull( *sParameter,
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
 * SQL_C_DOUBLE -> SQL_VARCHAR
 */
ZLV_DECLARE_C_TO_SQL( Double, VarChar )
{
    /*
     * ===================================================================
     * Test                                    | SQLSTATE
     * ===================================================================
     * Number of digits <= Column byte length. | n/a
     * Number of digits > Column byte length.  | 22001
     * ===================================================================
     */

    stlFloat64 * sParameter;
    stlBool      sSuccessWithInfo;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mLength,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_OCTETS,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (stlFloat64*)aParameterValuePtr;

    STL_TRY( dtlVarcharSetValueFromReal( *sParameter,
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
 * SQL_C_DOUBLE -> SQL_LONGVARCHAR
 */
ZLV_DECLARE_C_TO_SQL( Double, LongVarChar )
{
    /*
     * ===================================================================
     * Test                                    | SQLSTATE
     * ===================================================================
     * Number of digits <= Column byte length. | n/a
     * Number of digits > Column byte length.  | 22001
     * ===================================================================
     */

    stlFloat64 * sParameter;
    stlBool      sSuccessWithInfo;

    sParameter = (stlFloat64*)aParameterValuePtr;

    STL_TRY( dtlLongvarcharSetValueFromReal( *sParameter,
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
 * SQL_C_DOUBLE -> SQL_FLOAT
 */
ZLV_DECLARE_C_TO_SQL( Double, Float )
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

    STL_TRY( dtlFloatSetValueFromReal( *(stlFloat64*)aParameterValuePtr,
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
 * SQL_C_DOUBLE -> SQL_NUMERIC
 */
ZLV_DECLARE_C_TO_SQL( Double, Numeric )
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

    STL_TRY( dtlNumericSetValueFromReal( *(stlFloat64*)aParameterValuePtr,
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
 * SQL_C_DOUBLE -> SQL_TINYINT
 */
ZLV_DECLARE_C_TO_SQL( Double, TinyInt )
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

    stlFloat64 * sParameter;
    stlBool      sSuccessWithInfo;

    sParameter = (stlFloat64*)aParameterValuePtr;

    STL_TRY_THROW( ( *sParameter >= STL_INT8_MIN ) &&
                   ( *sParameter <= STL_INT8_MAX ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    /*
     * DataType에 TinyInt 타입이 없으므로 SmallInt로 변환한다.
     */
    STL_TRY( dtlSmallIntSetValueFromReal( *sParameter,
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
 * SQL_C_DOUBLE -> SQL_SMALLINT
 */
ZLV_DECLARE_C_TO_SQL( Double, SmallInt )
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

    stlFloat64 * sParameter;
    stlBool      sSuccessWithInfo;

    sParameter = (stlFloat64*)aParameterValuePtr;

    STL_TRY_THROW( ( *sParameter >= STL_INT16_MIN ) &&
                   ( *sParameter <= STL_INT16_MAX ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    STL_TRY( dtlSmallIntSetValueFromReal( *sParameter,
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
 * SQL_C_DOUBLE -> SQL_INTEGER
 */
ZLV_DECLARE_C_TO_SQL( Double, Integer )
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

    stlFloat64 * sParameter;
    stlBool      sSuccessWithInfo;

    sParameter = (stlFloat64*)aParameterValuePtr;

    STL_TRY_THROW( ( *sParameter >= STL_INT32_MIN ) &&
                   ( *sParameter <= STL_INT32_MAX ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    STL_TRY( dtlIntegerSetValueFromReal( *sParameter,
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
 * SQL_C_DOUBLE -> SQL_BIGINT
 */
ZLV_DECLARE_C_TO_SQL( Double, BigInt )
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

    stlFloat64 * sParameter;
    stlBool      sSuccessWithInfo;

    sParameter = (stlFloat64*)aParameterValuePtr;

    STL_TRY_THROW( ( *sParameter >= STL_INT64_MIN ) &&
                   ( *sParameter <= STL_INT64_MAX ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    STL_TRY( dtlBigIntSetValueFromReal( *sParameter,
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
 * SQL_C_DOUBLE -> SQL_REAL
 */
ZLV_DECLARE_C_TO_SQL( Double, Real )
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

    stlFloat64 * sParameter;
    stlBool      sSuccessWithInfo;

    sParameter = (stlFloat64*)aParameterValuePtr;

    STL_TRY_THROW( stlIsfinite( (stlFloat32)*sParameter ) != STL_IS_FINITE_FALSE,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    STL_TRY( dtlRealSetValueFromReal( (stlFloat32)*sParameter,
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
 * SQL_C_DOUBLE -> SQL_DOUBLE
 */
ZLV_DECLARE_C_TO_SQL( Double, Double )
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

    stlFloat64 * sParameter;
    stlBool      sSuccessWithInfo;

    sParameter = (stlFloat64*)aParameterValuePtr;

    STL_TRY( dtlDoubleSetValueFromReal( *sParameter,
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
 * SQL_C_DOUBLE -> SQL_BIT
 */
ZLV_DECLARE_C_TO_SQL( Double, Bit )
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

    stlFloat64 * sParameter;
    stlBool      sSuccessWithInfo;

    sParameter = (stlFloat64*)aParameterValuePtr;

    STL_TRY_THROW( ( *sParameter >= 0 ) &&
                   ( *sParameter < 2 ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    STL_TRY_THROW( ( *sParameter == 0 ) ||
                   ( *sParameter == 1 ),
                   RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED );

    STL_TRY( dtlBooleanSetValueFromInteger( (stlInt64)*sParameter,
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
 * SQL_C_DOUBLE -> SQL_BOOLEAN
 */
ZLV_DECLARE_C_TO_SQL( Double, Boolean )
{
    return zlvCDoubleToSqlBit( aStmt,
                               aParameterValuePtr,
                               aIndicator,
                               aApdRec,
                               aIpdRec,
                               aDataValue,
                               aSuccessWithInfo,
                               aErrorStack );
}

/** @} */

