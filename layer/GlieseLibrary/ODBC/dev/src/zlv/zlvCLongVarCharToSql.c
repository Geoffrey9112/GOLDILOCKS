/*******************************************************************************
 * zlvCLongVarCharToSql.c
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
 * @file zlvCLongVarCharToSql.c
 * @brief Gliese API Internal Converting Data from C Character to SQL Data Types Routines.
 */

/**
 * @addtogroup zlvCLongVarCharToSql
 * @{
 */

/*
 * http://msdn.microsoft.com/en-us/library/windows/desktop/ms714651%28v=VS.85%29.aspx
 */

/*
 * SQL_C_LONGVARCHAR -> SQL_CHAR
 */
ZLV_DECLARE_C_TO_SQL( LongVarChar, Char )
{
    /*
     * ===================================================================
     * Test                                  | SQLSTATE
     * ===================================================================
     * Byte length of data <= Column length. | n/a
     * Byte length of data > Column length.  | 22001
     * ===================================================================
     */

    SQL_LONG_VARIABLE_LENGTH_STRUCT * sParameter;
    stlChar                         * sValue;
    stlBool                           sSuccessWithInfo;
    stlInt64                          sLength;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mLength,
                                      DTL_SCALE_NA,
                                      aIpdRec->mStringLengthUnit,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aParameterValuePtr;
    sValue = (stlChar*)sParameter->arr;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( sValue );
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = stlStrlen( sValue );
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY_THROW( sLength <= aIpdRec->mOctetLength,
                   RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED);

    STL_TRY( dtlCharSetValueFromStringWithoutPaddNull( sValue,
                                                       sLength,
                                                       aIpdRec->mLength,
                                                       DTL_SCALE_NA,
                                                       aIpdRec->mStringLengthUnit,
                                                       DTL_INTERVAL_INDICATOR_NA,
                                                       aIpdRec->mOctetLength,
                                                       aDataValue,
                                                       &sSuccessWithInfo,
                                                       ZLS_STMT_DT_VECTOR(aStmt),
                                                       aStmt,
                                                       ZLS_STMT_NLS_DT_VECTOR(aStmt),
                                                       aStmt,
                                                       aErrorStack ) == STL_SUCCESS );
    
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
 * SQL_C_LONGVARCHAR -> SQL_VARCHAR
 */
ZLV_DECLARE_C_TO_SQL( LongVarChar, VarChar )
{
    /*
     * ===================================================================
     * Test                                  | SQLSTATE
     * ===================================================================
     * Byte length of data <= Column length. | n/a
     * Byte length of data > Column length.  | 22001
     * ===================================================================
     */

    SQL_LONG_VARIABLE_LENGTH_STRUCT * sParameter;
    stlChar                         * sValue;
    stlBool                           sSuccessWithInfo;
    stlInt64                          sLength;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mLength,
                                      DTL_SCALE_NA,
                                      aIpdRec->mStringLengthUnit,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aParameterValuePtr;
    sValue = (stlChar*)sParameter->arr;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( sValue );
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = stlStrlen( sValue );
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY_THROW( sLength <= aIpdRec->mOctetLength,
                   RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED);

    STL_TRY( dtlVarcharSetValueFromString( sValue,
                                           sLength,
                                           aIpdRec->mLength,
                                           DTL_SCALE_NA,
                                           aIpdRec->mStringLengthUnit,
                                           DTL_INTERVAL_INDICATOR_NA,
                                           aIpdRec->mOctetLength,
                                           aDataValue,
                                           &sSuccessWithInfo,
                                           ZLS_STMT_DT_VECTOR(aStmt),
                                           aStmt,
                                           ZLS_STMT_NLS_DT_VECTOR(aStmt),
                                           aStmt,
                                           aErrorStack ) == STL_SUCCESS );

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
 * SQL_C_LONGVARCHAR -> SQL_LONGVARCHAR
 */
ZLV_DECLARE_C_TO_SQL( LongVarChar, LongVarChar )
{
    /*
     * ===================================================================
     * Test                                  | SQLSTATE
     * ===================================================================
     * Byte length of data <= Column length. | n/a
     * Byte length of data > Column length.  | 22001
     * ===================================================================
     */

    SQL_LONG_VARIABLE_LENGTH_STRUCT * sParameter;
    stlChar                         * sValue;
    stlBool                           sSuccessWithInfo;
    stlInt64                          sLength;

    sParameter = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aParameterValuePtr;
    sValue = (stlChar*)sParameter->arr;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( sValue );
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = stlStrlen( sValue );
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlLongvarcharSetValueFromString( sValue,
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
                                               aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_LONGVARCHAR -> SQL_FLOAT
 */
ZLV_DECLARE_C_TO_SQL( LongVarChar, Float )
{
    /*
     * ======================================================================
     * Test                                                 | SQLSTATE
     * ======================================================================
     * Data is within the range of the data type to         | n/a
     * which the number is being converted                  | 
     * Data is outside the range of the data type to        | 22003
     * which the number is being converted                  |
     * Data value is not a numeric-literal                  | 22018
     * ======================================================================
     */

    SQL_LONG_VARIABLE_LENGTH_STRUCT * sParameter;
    stlChar                         * sValue;
    stlBool                           sSuccessWithInfo;
    stlInt64                          sLength;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aParameterValuePtr;
    sValue = (stlChar*)sParameter->arr;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( sValue );
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = stlStrlen( sValue );
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlFloatSetValueFromString( sValue,
                                         sLength,
                                         aIpdRec->mPrecision,
                                         DTL_SCALE_NA,
                                         DTL_STRING_LENGTH_UNIT_NA,
                                         DTL_INTERVAL_INDICATOR_NA,
                                         aIpdRec->mOctetLength,
                                         aDataValue,
                                         &sSuccessWithInfo,
                                         ZLS_STMT_DT_VECTOR(aStmt),
                                         aStmt,
                                         ZLS_STMT_NLS_DT_VECTOR(aStmt),
                                         aStmt,
                                         aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_LONGVARCHAR -> SQL_NUMERIC
 */
ZLV_DECLARE_C_TO_SQL( LongVarChar, Numeric )
{
    /*
     * ======================================================================
     * Test                                                 | SQLSTATE
     * ======================================================================
     * Data converted without truncation.                   | n/a
     * Data converted with truncation of fractional digits. | 22001
     * Conversion of data would result in loss of whole     | 22001
     * (as opposed to fractional) digits.                   |
     * Data value is not a numeric-literal                  | 22018
     * ======================================================================
     */

    SQL_LONG_VARIABLE_LENGTH_STRUCT * sParameter;
    stlChar                         * sValue;
    stlBool                           sSuccessWithInfo;
    stlInt64                          sLength;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mPrecision,
                                      aIpdRec->mScale,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aParameterValuePtr;
    sValue = (stlChar*)sParameter->arr;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( sValue );
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = stlStrlen( sValue );
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlNumericSetValueFromString( sValue,
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
                                           aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_LONGVARCHAR -> SQL_TINYINT
 */
ZLV_DECLARE_C_TO_SQL( LongVarChar, TinyInt )
{
    /*
     * ======================================================================
     * Test                                                 | SQLSTATE
     * ======================================================================
     * Data converted without truncation.                   | n/a
     * Data converted with truncation of fractional digits. | 22001
     * Conversion of data would result in loss of whole     | 22001
     * (as opposed to fractional) digits.                   |
     * Data value is not a numeric-literal                  | 22018
     * ======================================================================
     */

    SQL_LONG_VARIABLE_LENGTH_STRUCT * sParameter;
    stlChar                         * sValue;
    stlInt64                          sLength;
    stlInt64                          sConvertedResult;
    stlChar                         * sStartPtr;
    stlChar                         * sEndPtr;
    stlBool                           sSuccessWithInfo;

    sParameter = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aParameterValuePtr;
    sValue = (stlChar*)sParameter->arr;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( sValue );
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = stlStrlen( sValue );
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    sStartPtr = sValue;
    STL_TRY_THROW( stlStrToInt64( (const stlChar*)sStartPtr,
                                  sLength,
                                  &sEndPtr,
                                  10,
                                  &sConvertedResult,
                                  aErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_CONVERSION );
    
    STL_TRY_THROW( sStartPtr != sEndPtr,
                   RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION );

    STL_TRY_THROW( ( sConvertedResult >= STL_INT8_MIN ) &&
                   ( sConvertedResult <= STL_INT8_MAX ),
                   RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED );

    STL_TRY_THROW( ( sEndPtr == ( sStartPtr + sLength ) ) ||
                   ( sEndPtr == NULL ),
                   RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED );

    /*
     * DataType에 TinyInt 타입이 없으므로 SmallInt로 변환한다.
     */
    STL_TRY( dtlSmallIntSetValueFromInteger( sConvertedResult,
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

    STL_CATCH( RAMP_ERR_CONVERSION )
    {
        switch( stlGetLastErrorCode( aErrorStack ) )
        {
            case STL_ERRCODE_STRING_IS_NOT_NUMBER :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                              "*StatementText contained a C type that was an exact or "
                              "approximate numeric, a datetime, or an interval data type; "
                              "the SQL type of the column was a character data type; "
                              "and the value in the column was not a valid literal of the bound C type.",
                              aErrorStack );
                break;
            case STL_ERRCODE_OUT_OF_RANGE :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              ZLE_ERRCODE_STRING_DATA_RIGHT_TRUNCATED,
                              "The assignment of a character or binary value to a column resulted "
                              "in the truncation of nonblank (character) or "
                              "non-null (binary) characters or bytes.",
                              aErrorStack );
                break;
            default :
                break;
        }
    }
    
    STL_CATCH( RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                      "*StatementText contained a C type that was an exact or "
                      "approximate numeric, a datetime, or an interval data type; "
                      "the SQL type of the column was a character data type; "
                      "and the value in the column was not a valid literal of the bound C type.",
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
 * SQL_C_LONGVARCHAR -> SQL_SMALLINT
 */
ZLV_DECLARE_C_TO_SQL( LongVarChar, SmallInt )
{
    /*
     * ======================================================================
     * Test                                                 | SQLSTATE
     * ======================================================================
     * Data converted without truncation.                   | n/a
     * Data converted with truncation of fractional digits. | 22001
     * Conversion of data would result in loss of whole     | 22001
     * (as opposed to fractional) digits.                   |
     * Data value is not a numeric-literal                  | 22018
     * ======================================================================
     */

    SQL_LONG_VARIABLE_LENGTH_STRUCT * sParameter;
    stlChar                         * sValue;
    stlInt64                          sLength;
    stlInt64                          sConvertedResult;
    stlChar                         * sStartPtr;
    stlChar                         * sEndPtr;
    stlBool                           sSuccessWithInfo;

    sParameter = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aParameterValuePtr;
    sValue = (stlChar*)sParameter->arr;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( sValue );
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = stlStrlen( sValue );
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    sStartPtr = sValue;
    STL_TRY_THROW( stlStrToInt64( (const stlChar*)sStartPtr,
                                  sLength,
                                  &sEndPtr,
                                  10,
                                  &sConvertedResult,
                                  aErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_CONVERSION );
    
    STL_TRY_THROW( sStartPtr != sEndPtr,
                   RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION );

    STL_TRY_THROW( ( sConvertedResult >= STL_INT16_MIN ) &&
                   ( sConvertedResult <= STL_INT16_MAX ),
                   RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED );

    STL_TRY_THROW( ( sEndPtr == ( sStartPtr + sLength ) ) ||
                   ( sEndPtr == NULL ),
                   RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED );

    STL_TRY( dtlSmallIntSetValueFromInteger( sConvertedResult,
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

    STL_CATCH( RAMP_ERR_CONVERSION )
    {
        switch( stlGetLastErrorCode( aErrorStack ) )
        {
            case STL_ERRCODE_STRING_IS_NOT_NUMBER :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                              "*StatementText contained a C type that was an exact or "
                              "approximate numeric, a datetime, or an interval data type; "
                              "the SQL type of the column was a character data type; "
                              "and the value in the column was not a valid literal of the bound C type.",
                              aErrorStack );
                break;
            case STL_ERRCODE_OUT_OF_RANGE :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              ZLE_ERRCODE_STRING_DATA_RIGHT_TRUNCATED,
                              "The assignment of a character or binary value to a column resulted "
                              "in the truncation of nonblank (character) or "
                              "non-null (binary) characters or bytes.",
                              aErrorStack );
                break;
            default :
                break;
        }
    }

    STL_CATCH( RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                      "*StatementText contained a C type that was an exact or "
                      "approximate numeric, a datetime, or an interval data type; "
                      "the SQL type of the column was a character data type; "
                      "and the value in the column was not a valid literal of the bound C type.",
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
 * SQL_C_LONGVARCHAR -> SQL_INTEGER
 */
ZLV_DECLARE_C_TO_SQL( LongVarChar, Integer )
{
    /*
     * ======================================================================
     * Test                                                 | SQLSTATE
     * ======================================================================
     * Data converted without truncation.                   | n/a
     * Data converted with truncation of fractional digits. | 22001
     * Conversion of data would result in loss of whole     | 22001
     * (as opposed to fractional) digits.                   |
     * Data value is not a numeric-literal                  | 22018
     * ======================================================================
     */

    SQL_LONG_VARIABLE_LENGTH_STRUCT * sParameter;
    stlChar                         * sValue;
    stlInt64                          sLength;
    stlInt64                          sConvertedResult;
    stlChar                         * sStartPtr;
    stlChar                         * sEndPtr;
    stlBool                           sSuccessWithInfo;

    sParameter = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aParameterValuePtr;
    sValue = (stlChar*)sParameter->arr;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( sValue );
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = stlStrlen( sValue );
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    sStartPtr = sValue;
    STL_TRY_THROW( stlStrToInt64( (const stlChar*)sStartPtr,
                                  sLength,
                                  &sEndPtr,
                                  10,
                                  &sConvertedResult,
                                  aErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_CONVERSION );
    
    STL_TRY_THROW( sStartPtr != sEndPtr,
                   RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION );

    STL_TRY_THROW( ( sConvertedResult >= STL_INT32_MIN ) &&
                   ( sConvertedResult <= STL_INT32_MAX ),
                   RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED );

    STL_TRY_THROW( ( sEndPtr == ( sStartPtr + sLength ) ) ||
                   ( sEndPtr == NULL ),
                   RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED );

    STL_TRY( dtlIntegerSetValueFromInteger( sConvertedResult,
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

    STL_CATCH( RAMP_ERR_CONVERSION )
    {
        switch( stlGetLastErrorCode( aErrorStack ) )
        {
            case STL_ERRCODE_STRING_IS_NOT_NUMBER :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                              "*StatementText contained a C type that was an exact or "
                              "approximate numeric, a datetime, or an interval data type; "
                              "the SQL type of the column was a character data type; "
                              "and the value in the column was not a valid literal of the bound C type.",
                              aErrorStack );
                break;
            case STL_ERRCODE_OUT_OF_RANGE :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              ZLE_ERRCODE_STRING_DATA_RIGHT_TRUNCATED,
                              "The assignment of a character or binary value to a column resulted "
                              "in the truncation of nonblank (character) or "
                              "non-null (binary) characters or bytes.",
                              aErrorStack );
                break;
            default :
                break;
        }
    }
    
    STL_CATCH( RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                      "*StatementText contained a C type that was an exact or "
                      "approximate numeric, a datetime, or an interval data type; "
                      "the SQL type of the column was a character data type; "
                      "and the value in the column was not a valid literal of the bound C type.",
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
 * SQL_C_LONGVARCHAR -> SQL_BIGINT
 */
ZLV_DECLARE_C_TO_SQL( LongVarChar, BigInt )
{
    /*
     * ======================================================================
     * Test                                                 | SQLSTATE
     * ======================================================================
     * Data converted without truncation.                   | n/a
     * Data converted with truncation of fractional digits. | 22001
     * Conversion of data would result in loss of whole     | 22001
     * (as opposed to fractional) digits.                   |
     * Data value is not a numeric-literal                  | 22018
     * ======================================================================
     */

    SQL_LONG_VARIABLE_LENGTH_STRUCT * sParameter;
    stlChar                         * sValue;
    stlInt64                          sLength;
    stlInt64                          sConvertedResult;
    stlChar                         * sStartPtr;
    stlChar                         * sEndPtr;
    stlBool                           sSuccessWithInfo;

    sParameter = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aParameterValuePtr;
    sValue = (stlChar*)sParameter->arr;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( sValue );
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = stlStrlen( sValue );
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    sStartPtr = sValue;
    STL_TRY_THROW( stlStrToInt64( (const stlChar*)sStartPtr,
                                  sLength,
                                  &sEndPtr,
                                  10,
                                  &sConvertedResult,
                                  aErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_CONVERSION );
    
    STL_TRY_THROW( sStartPtr != sEndPtr,
                   RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION );

    STL_TRY_THROW( ( sConvertedResult >= STL_INT64_MIN ) &&
                   ( sConvertedResult <= STL_INT64_MAX ),
                   RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED );

    STL_TRY_THROW( ( sEndPtr == ( sStartPtr + sLength ) ) ||
                   ( sEndPtr == NULL ),
                   RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED );

    STL_TRY( dtlBigIntSetValueFromInteger( sConvertedResult,
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

    STL_CATCH( RAMP_ERR_CONVERSION )
    {
        switch( stlGetLastErrorCode( aErrorStack ) )
        {
            case STL_ERRCODE_STRING_IS_NOT_NUMBER :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                              "*StatementText contained a C type that was an exact or "
                              "approximate numeric, a datetime, or an interval data type; "
                              "the SQL type of the column was a character data type; "
                              "and the value in the column was not a valid literal of the bound C type.",
                              aErrorStack );
                break;
            case STL_ERRCODE_OUT_OF_RANGE :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              ZLE_ERRCODE_STRING_DATA_RIGHT_TRUNCATED,
                              "The assignment of a character or binary value to a column resulted "
                              "in the truncation of nonblank (character) or "
                              "non-null (binary) characters or bytes.",
                              aErrorStack );
                break;
            default :
                break;
        }
    }
    
    STL_CATCH( RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                      "*StatementText contained a C type that was an exact or "
                      "approximate numeric, a datetime, or an interval data type; "
                      "the SQL type of the column was a character data type; "
                      "and the value in the column was not a valid literal of the bound C type.",
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
 * SQL_C_LONGVARCHAR -> SQL_REAL
 */
ZLV_DECLARE_C_TO_SQL( LongVarChar, Real )
{
    /*
     * ======================================================================
     * Test                                                 | SQLSTATE
     * ======================================================================
     * Data is within the range of the data type to         | n/a
     * which the number is being converted                  | 
     * Data is outside the range of the data type to        | 22003
     * which the number is being converted                  |
     * Data value is not a numeric-literal                  | 22018
     * ======================================================================
     */

    SQL_LONG_VARIABLE_LENGTH_STRUCT * sParameter;
    stlChar                         * sValue;
    stlInt64                          sLength;
    stlFloat32                        sConvertedResult;
    stlChar                         * sStartPtr;
    stlChar                         * sEndPtr;
    stlBool                           sSuccessWithInfo;

    sParameter = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aParameterValuePtr;
    sValue = (stlChar*)sParameter->arr;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( sValue );
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = stlStrlen( sValue );
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    sStartPtr = sValue;
    STL_TRY_THROW( stlStrToFloat32( (const stlChar*)sStartPtr,
                                    sLength,
                                    &sEndPtr,
                                    &sConvertedResult,
                                    aErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_CONVERSION );
    
    STL_TRY_THROW( sStartPtr != sEndPtr,
                   RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION );

    STL_TRY( dtlRealSetValueFromReal( sConvertedResult,
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

    STL_CATCH( RAMP_ERR_CONVERSION )
    {
        switch( stlGetLastErrorCode( aErrorStack ) )
        {
            case STL_ERRCODE_STRING_IS_NOT_NUMBER :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                              "*StatementText contained a C type that was an exact or "
                              "approximate numeric, a datetime, or an interval data type; "
                              "the SQL type of the column was a character data type; "
                              "and the value in the column was not a valid literal of the bound C type.",
                              aErrorStack );
                break;
            case STL_ERRCODE_OUT_OF_RANGE :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                              "The prepared statement associated with the StatementHandle "
                              "contained a bound numeric parameter, and the parameter value "
                              "caused the whole (as opposed to fractional) part of the number "
                              "to be truncated when assigned to the associated table column.",
                              aErrorStack );
                break;
            default :
                break;
        }
    }

    STL_CATCH( RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                      "*StatementText contained a C type that was an exact or "
                      "approximate numeric, a datetime, or an interval data type; "
                      "the SQL type of the column was a character data type; "
                      "and the value in the column was not a valid literal of the bound C type.",
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_LONGVARCHAR -> SQL_DOUBLE
 */
ZLV_DECLARE_C_TO_SQL( LongVarChar, Double )
{
    /*
     * ======================================================================
     * Test                                                 | SQLSTATE
     * ======================================================================
     * Data is within the range of the data type to         | n/a
     * which the number is being converted                  | 
     * Data is outside the range of the data type to        | 22003
     * which the number is being converted                  |
     * Data value is not a numeric-literal                  | 22018
     * ======================================================================
     */

    SQL_LONG_VARIABLE_LENGTH_STRUCT * sParameter;
    stlChar                         * sValue;
    stlInt64                          sLength;
    stlFloat64                        sConvertedResult;
    stlChar                         * sStartPtr;
    stlChar                         * sEndPtr;
    stlBool                           sSuccessWithInfo;

    sParameter = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aParameterValuePtr;
    sValue = (stlChar*)sParameter->arr;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( sValue );
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = stlStrlen( sValue );
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    sStartPtr = sValue;
    STL_TRY_THROW( stlStrToFloat64( (const stlChar*)sStartPtr,
                                    sLength,
                                    &sEndPtr,
                                    &sConvertedResult,
                                    aErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_CONVERSION );
    
    STL_TRY_THROW( sStartPtr != sEndPtr,
                   RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION );

    STL_TRY( dtlDoubleSetValueFromReal( sConvertedResult,
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

    STL_CATCH( RAMP_ERR_CONVERSION )
    {
        switch( stlGetLastErrorCode( aErrorStack ) )
        {
            case STL_ERRCODE_STRING_IS_NOT_NUMBER :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                              "*StatementText contained a C type that was an exact or "
                              "approximate numeric, a datetime, or an interval data type; "
                              "the SQL type of the column was a character data type; "
                              "and the value in the column was not a valid literal of the bound C type.",
                              aErrorStack );
                break;
            case STL_ERRCODE_OUT_OF_RANGE :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                              "The prepared statement associated with the StatementHandle "
                              "contained a bound numeric parameter, and the parameter value "
                              "caused the whole (as opposed to fractional) part of the number "
                              "to be truncated when assigned to the associated table column.",
                              aErrorStack );
                break;
            default :
                break;
        }
    }

    STL_CATCH( RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                      "*StatementText contained a C type that was an exact or "
                      "approximate numeric, a datetime, or an interval data type; "
                      "the SQL type of the column was a character data type; "
                      "and the value in the column was not a valid literal of the bound C type.",
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_LONGVARCHAR -> SQL_BIT
 */
ZLV_DECLARE_C_TO_SQL( LongVarChar, Bit )
{
    /*
     * ======================================================================
     * Test                                                    | SQLSTATE
     * ======================================================================
     * Data is 0 or 1                                          | n/a
     * Data is greater than 0, less than 2, and not equal to 1 | 22001
     * Data is less than 0 or greater than or equal to 2       | 22003
     * Data value is not a numeric-literal                     | 22018
     * ======================================================================
     */

    SQL_LONG_VARIABLE_LENGTH_STRUCT * sParameter;
    stlChar                         * sValue;
    stlInt64                          sLength;
    stlInt64                          sConvertedResult;
    stlChar                         * sStartPtr;
    stlChar                         * sEndPtr;
    stlBool                           sSuccessWithInfo;

    sParameter = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aParameterValuePtr;
    sValue = (stlChar*)sParameter->arr;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( sValue );
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = stlStrlen( sValue );
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    sStartPtr = sValue;
    STL_TRY_THROW( stlStrToInt64( (const stlChar*)sStartPtr,
                                  sLength,
                                  &sEndPtr,
                                  10,
                                  &sConvertedResult,
                                  aErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_CONVERSION );
    
    STL_TRY_THROW( sStartPtr != sEndPtr,
                   RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION );

    STL_TRY_THROW( ( sConvertedResult >= 0 ) &&
                   ( sConvertedResult < 2 ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    STL_TRY_THROW( ( sEndPtr == ( sStartPtr + sLength ) ) ||
                   ( sEndPtr == NULL ),
                   RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED );

    STL_TRY( dtlBooleanSetValueFromInteger( sConvertedResult,
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

    STL_CATCH( RAMP_ERR_CONVERSION )
    {
        switch( stlGetLastErrorCode( aErrorStack ) )
        {
            case STL_ERRCODE_STRING_IS_NOT_NUMBER :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                              "*StatementText contained a C type that was an exact or "
                              "approximate numeric, a datetime, or an interval data type; "
                              "the SQL type of the column was a character data type; "
                              "and the value in the column was not a valid literal of the bound C type.",
                              aErrorStack );
                break;
            case STL_ERRCODE_OUT_OF_RANGE :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              ZLE_ERRCODE_STRING_DATA_RIGHT_TRUNCATED,
                              "The prepared statement associated with the StatementHandle "
                              "contained a bound numeric parameter, and the parameter value "
                              "caused the whole (as opposed to fractional) part of the number "
                              "to be truncated when assigned to the associated table column.",
                              aErrorStack );
                break;
            default :
                break;
        }
    }
    
    STL_CATCH( RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                      "*StatementText contained a C type that was an exact or "
                      "approximate numeric, a datetime, or an interval data type; "
                      "the SQL type of the column was a character data type; "
                      "and the value in the column was not a valid literal of the bound C type.",
                      aErrorStack );
    }

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
 * SQL_C_LONGVARCHAR -> SQL_BOOLEAN
 */
ZLV_DECLARE_C_TO_SQL( LongVarChar, Boolean )
{
    SQL_LONG_VARIABLE_LENGTH_STRUCT * sParameter;
    stlChar                         * sValue;
    stlInt64                          sLength;
    stlBool                           sSuccessWithInfo;

    sParameter = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aParameterValuePtr;
    sValue = (stlChar*)sParameter->arr;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( sValue );
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = stlStrlen( sValue );
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlBooleanSetValueFromString( sValue,
                                           sLength,
                                           DTL_PRECISION_NA,
                                           DTL_SCALE_NA,
                                           DTL_STRING_LENGTH_UNIT_NA,
                                           DTL_INTERVAL_INDICATOR_NA,
                                           aIpdRec->mOctetLength,
                                           aDataValue,
                                           &sSuccessWithInfo,
                                           ZLS_STMT_DT_VECTOR(aStmt),
                                           aStmt,
                                           ZLS_STMT_NLS_DT_VECTOR(aStmt),
                                           aStmt,
                                           aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_LONGVARCHAR -> SQL_BINARY
 */
ZLV_DECLARE_C_TO_SQL( LongVarChar, Binary )
{
    /*
     * ======================================================================
     * Test                                                    | SQLSTATE
     * ======================================================================
     * (Byte length of data) / 2 <= column byte length         | n/a
     * (Byte length of data) / 2 > column byte length          | 22001
     * Data value is not a hexadecimal value                   | 22018
     * ======================================================================
     */

    SQL_LONG_VARIABLE_LENGTH_STRUCT * sParameter;
    stlChar                         * sValue;
    stlBool                           sSuccessWithInfo;
    stlInt64                          sLength;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mLength,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aParameterValuePtr;
    sValue = (stlChar*)sParameter->arr;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( sValue );
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = stlStrlen( sValue );
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY_THROW( (sLength / 2 ) <= aIpdRec->mLength,
                   RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED);


    STL_TRY( dtlBinarySetValueFromStringWithoutPaddNull( sValue,
                                                         sLength,
                                                         aIpdRec->mLength,
                                                         DTL_SCALE_NA,
                                                         DTL_STRING_LENGTH_UNIT_NA,
                                                         DTL_INTERVAL_INDICATOR_NA,
                                                         aIpdRec->mOctetLength,
                                                         aDataValue,
                                                         &sSuccessWithInfo,
                                                         ZLS_STMT_DT_VECTOR(aStmt),
                                                         aStmt,
                                                         ZLS_STMT_NLS_DT_VECTOR(aStmt),
                                                         aStmt,
                                                         aErrorStack ) == STL_SUCCESS );

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
 * SQL_C_LONGVARCHAR -> SQL_VARBINARY
 */
ZLV_DECLARE_C_TO_SQL( LongVarChar, VarBinary )
{
    /*
     * ======================================================================
     * Test                                                    | SQLSTATE
     * ======================================================================
     * (Byte length of data) / 2 <= column byte length         | n/a
     * (Byte length of data) / 2 > column byte length          | 22001
     * Data value is not a hexadecimal value                   | 22018
     * ======================================================================
     */

    SQL_LONG_VARIABLE_LENGTH_STRUCT * sParameter;
    stlChar                         * sValue;
    stlBool                           sSuccessWithInfo;
    stlInt64                          sLength;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mLength,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aParameterValuePtr;
    sValue = (stlChar*)sParameter->arr;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( sValue );
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = stlStrlen( sValue );
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY_THROW( (sLength / 2 ) <= aIpdRec->mLength,
                   RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED);


    STL_TRY( dtlVarbinarySetValueFromString( sValue,
                                             sLength,
                                             aIpdRec->mLength,
                                             DTL_SCALE_NA,
                                             DTL_STRING_LENGTH_UNIT_NA,
                                             DTL_INTERVAL_INDICATOR_NA,
                                             aIpdRec->mOctetLength,
                                             aDataValue,
                                             &sSuccessWithInfo,
                                             ZLS_STMT_DT_VECTOR(aStmt),
                                             aStmt,
                                             ZLS_STMT_NLS_DT_VECTOR(aStmt),
                                             aStmt,
                                             aErrorStack ) == STL_SUCCESS );

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
 * SQL_C_LONGVARCHAR -> SQL_LONGVARBINARY
 */
ZLV_DECLARE_C_TO_SQL( LongVarChar, LongVarBinary )
{
    /*
     * ======================================================================
     * Test                                                    | SQLSTATE
     * ======================================================================
     * (Byte length of data) / 2 <= column byte length         | n/a
     * (Byte length of data) / 2 > column byte length          | 22001
     * Data value is not a hexadecimal value                   | 22018
     * ======================================================================
     */

    SQL_LONG_VARIABLE_LENGTH_STRUCT * sParameter;
    stlChar                         * sValue;
    stlBool                           sSuccessWithInfo;
    stlInt64                          sLength;

    sParameter = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aParameterValuePtr;
    sValue = (stlChar*)sParameter->arr;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( sValue );
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = stlStrlen( sValue );
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlLongvarbinarySetValueFromString( sValue,
                                                 sLength,
                                                 aIpdRec->mLength,
                                                 DTL_SCALE_NA,
                                                 DTL_STRING_LENGTH_UNIT_NA,
                                                 DTL_INTERVAL_INDICATOR_NA,
                                                 aIpdRec->mOctetLength,
                                                 aDataValue,
                                                 &sSuccessWithInfo,
                                                 ZLS_STMT_DT_VECTOR(aStmt),
                                                 aStmt,
                                                 ZLS_STMT_NLS_DT_VECTOR(aStmt),
                                                 aStmt,
                                                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_LONGVARCHAR -> SQL_TYPE_DATE
 */
ZLV_DECLARE_C_TO_SQL( LongVarChar, Date )
{
    /*
     * ================================================================================
     * Test                                                                  | SQLSTATE
     * ================================================================================
     * Data value is a valid ODBC-date-literal                               | n/a
     * Data value is a valid ODBC-timestamp-literal;                         | n/a
     * time portion is zero                                                  |
     * Data value is a valid ODBC-timestamp-literal;                         | 22008
     * time portion is nonzero                                               | 
     * Data value is not a valid ODBC-date-literal or ODBC-timestamp-literal | 22018
     * ================================================================================
     */

    SQL_LONG_VARIABLE_LENGTH_STRUCT * sParameter;
    stlChar                         * sValue;
    stlBool                           sSuccessWithInfo;
    stlInt64                          sLength;

    sParameter = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aParameterValuePtr;
    sValue = (stlChar*)sParameter->arr;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( sValue );
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = stlStrlen( sValue );
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlDateSetValueFromString( sValue,
                                        sLength,
                                        DTL_PRECISION_NA,
                                        DTL_SCALE_NA,
                                        DTL_STRING_LENGTH_UNIT_NA,
                                        DTL_INTERVAL_INDICATOR_NA,
                                        aIpdRec->mOctetLength,
                                        aDataValue,
                                        &sSuccessWithInfo,
                                        ZLS_STMT_DT_VECTOR(aStmt),
                                        aStmt,
                                        ZLS_STMT_NLS_DT_VECTOR(aStmt),
                                        aStmt,
                                        aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_LONGVARCHAR -> SQL_TYPE_TIME
 */
ZLV_DECLARE_C_TO_SQL( LongVarChar, Time )
{
    /*
     * ================================================================================
     * Test                                                                  | SQLSTATE
     * ================================================================================
     * Data value is a valid ODBC-time-literal                               | n/a
     * Data value is a valid ODBC-timestamp-literal;                         | n/a
     * fractional seconds portion is zero                                    |
     * Data value is a valid ODBC-timestamp-literal;                         | 22008
     * fractional seconds portion is nonzero                                 | 
     * Data value is not a valid ODBC-date-literal or ODBC-timestamp-literal | 22018
     * ================================================================================
     */

    SQL_LONG_VARIABLE_LENGTH_STRUCT * sParameter;
    stlChar                         * sValue;
    stlBool                           sSuccessWithInfo;
    stlInt64                          sLength;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aParameterValuePtr;
    sValue = (stlChar*)sParameter->arr;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( sValue );
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = stlStrlen( sValue );
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlTimeSetValueFromString( sValue,
                                        sLength,
                                        aIpdRec->mPrecision,
                                        DTL_SCALE_NA,
                                        DTL_STRING_LENGTH_UNIT_NA,
                                        DTL_INTERVAL_INDICATOR_NA,
                                        aIpdRec->mOctetLength,
                                        aDataValue,
                                        &sSuccessWithInfo,
                                        ZLS_STMT_DT_VECTOR(aStmt),
                                        aStmt,
                                        ZLS_STMT_NLS_DT_VECTOR(aStmt),
                                        aStmt,
                                        aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_LONGVARCHAR -> SQL_TYPE_TIME_TZ
 */
ZLV_DECLARE_C_TO_SQL( LongVarChar, TimeTz )
{
    SQL_LONG_VARIABLE_LENGTH_STRUCT * sParameter;
    stlChar                         * sValue;
    stlBool                           sSuccessWithInfo;
    stlInt64                          sLength;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aParameterValuePtr;
    sValue = (stlChar*)sParameter->arr;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( sValue );
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = stlStrlen( sValue );
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlTimeTzSetValueFromString( sValue,
                                          sLength,
                                          aIpdRec->mPrecision,
                                          DTL_SCALE_NA,
                                          DTL_STRING_LENGTH_UNIT_NA,
                                          DTL_INTERVAL_INDICATOR_NA,
                                          aIpdRec->mOctetLength,
                                          aDataValue,
                                          &sSuccessWithInfo,
                                          ZLS_STMT_DT_VECTOR(aStmt),
                                          aStmt,
                                          ZLS_STMT_NLS_DT_VECTOR(aStmt),
                                          aStmt,
                                          aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_LONGVARCHAR -> SQL_TYPE_TIMESTAMP
 */
ZLV_DECLARE_C_TO_SQL( LongVarChar, Timestamp )
{
    /*
     * ============================================================
     * Test                                              | SQLSTATE
     * ============================================================
     * Data value is a valid ODBC-timestamp-literal;     | n/a
     * fractional seconds portion not truncated          |
     * Data value is a valid ODBC-timestamp-literal;     | 22008
     * fractional seconds portion truncated              |
     * Data value is a valid ODBC-date-literal           | n/a
     * Data value is a valid ODBC-time-literal           | n/a
     * Data value is not a valid ODBC-date-literal,      | 22018
     * ODBC-time-literal, or ODBC-timestamp-literal      | 
     * ============================================================
     */

    SQL_LONG_VARIABLE_LENGTH_STRUCT * sParameter;
    stlChar                         * sValue;
    stlBool                           sSuccessWithInfo;
    stlInt64                          sLength;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aParameterValuePtr;
    sValue = (stlChar*)sParameter->arr;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( sValue );
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = stlStrlen( sValue );
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlTimestampSetValueFromString( sValue,
                                             sLength,
                                             aIpdRec->mPrecision,
                                             DTL_SCALE_NA,
                                             DTL_STRING_LENGTH_UNIT_NA,
                                             DTL_INTERVAL_INDICATOR_NA,
                                             aIpdRec->mOctetLength,
                                             aDataValue,
                                             &sSuccessWithInfo,
                                             ZLS_STMT_DT_VECTOR(aStmt),
                                             aStmt,
                                             ZLS_STMT_NLS_DT_VECTOR(aStmt),
                                             aStmt,
                                             aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_LONGVARCHAR -> SQL_TYPE_TIMESTAMP_TZ
 */
ZLV_DECLARE_C_TO_SQL( LongVarChar, TimestampTz )
{
    SQL_LONG_VARIABLE_LENGTH_STRUCT * sParameter;
    stlChar                         * sValue;
    stlBool                           sSuccessWithInfo;
    stlInt64                          sLength;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aParameterValuePtr;
    sValue = (stlChar*)sParameter->arr;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( sValue );
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = stlStrlen( sValue );
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlTimestampTzSetValueFromString( sValue,
                                               sLength,
                                               aIpdRec->mPrecision,
                                               DTL_SCALE_NA,
                                               DTL_STRING_LENGTH_UNIT_NA,
                                               DTL_INTERVAL_INDICATOR_NA,
                                               aIpdRec->mOctetLength,
                                               aDataValue,
                                               &sSuccessWithInfo,
                                               ZLS_STMT_DT_VECTOR(aStmt),
                                               aStmt,
                                               ZLS_STMT_NLS_DT_VECTOR(aStmt),
                                               aStmt,
                                               aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_LONGVARCHAR -> SQL_INTERVAL_MONTH
 */
ZLV_DECLARE_C_TO_SQL( LongVarChar, IntervalMonth )
{
    /*
     * =============================================================
     * Test                                               | SQLSTATE
     * =============================================================
     * Data value is a valid interval value;              | n/a 
     * no truncation occurs                               |
     * Data value is a valid interval value;              | 22015
     * the value in one of the fields is truncated        |
     * The data value is not a valid interval literal     | 22018
     * =============================================================
     */

    SQL_LONG_VARIABLE_LENGTH_STRUCT * sParameter;
    stlChar                         * sValue;
    stlBool                           sSuccessWithInfo;
    stlInt64                          sLength;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_MONTH,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aParameterValuePtr;
    sValue = (stlChar*)sParameter->arr;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( sValue );
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = stlStrlen( sValue );
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlIntervalYearToMonthSetValueFromString( 
                 sValue,
                 sLength,
                 aIpdRec->mDatetimeIntervalPrecision,
                 DTL_SCALE_NA,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_MONTH,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 ZLS_STMT_DT_VECTOR(aStmt),
                 aStmt,
                 ZLS_STMT_NLS_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_LONGVARCHAR -> SQL_INTERVAL_YEAR
 */
ZLV_DECLARE_C_TO_SQL( LongVarChar, IntervalYear )
{
    /*
     * =============================================================
     * Test                                               | SQLSTATE
     * =============================================================
     * Data value is a valid interval value;              | n/a 
     * no truncation occurs                               |
     * Data value is a valid interval value;              | 22015
     * the value in one of the fields is truncated        |
     * The data value is not a valid interval literal     | 22018
     * =============================================================
     */

    SQL_LONG_VARIABLE_LENGTH_STRUCT * sParameter;
    stlChar                         * sValue;
    stlBool                           sSuccessWithInfo;
    stlInt64                          sLength;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_YEAR,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aParameterValuePtr;
    sValue = (stlChar*)sParameter->arr;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( sValue );
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = stlStrlen( sValue );
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlIntervalYearToMonthSetValueFromString(
                 sValue,
                 sLength,
                 aIpdRec->mDatetimeIntervalPrecision,
                 DTL_SCALE_NA,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_YEAR,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 ZLS_STMT_DT_VECTOR(aStmt),
                 aStmt,
                 ZLS_STMT_NLS_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_LONGVARCHAR -> SQL_INTERVAL_YEAR_TO_MONTH
 */
ZLV_DECLARE_C_TO_SQL( LongVarChar, IntervalYearToMonth )
{
    /*
     * =============================================================
     * Test                                               | SQLSTATE
     * =============================================================
     * Data value is a valid interval value;              | n/a 
     * no truncation occurs                               |
     * Data value is a valid interval value;              | 22015
     * the value in one of the fields is truncated        |
     * The data value is not a valid interval literal     | 22018
     * =============================================================
     */

    SQL_LONG_VARIABLE_LENGTH_STRUCT * sParameter;
    stlChar                         * sValue;
    stlBool                           sSuccessWithInfo;
    stlInt64                          sLength;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aParameterValuePtr;
    sValue = (stlChar*)sParameter->arr;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( sValue );
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = stlStrlen( sValue );
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlIntervalYearToMonthSetValueFromString( 
                 sValue,
                 sLength,
                 aIpdRec->mDatetimeIntervalPrecision,
                 DTL_SCALE_NA,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 ZLS_STMT_DT_VECTOR(aStmt),
                 aStmt,
                 ZLS_STMT_NLS_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_LONGVARCHAR -> SQL_INTERVAL_DAY
 */
ZLV_DECLARE_C_TO_SQL( LongVarChar, IntervalDay )
{
    /*
     * =============================================================
     * Test                                               | SQLSTATE
     * =============================================================
     * Data value is a valid interval value;              | n/a 
     * no truncation occurs                               |
     * Data value is a valid interval value;              | 22015
     * the value in one of the fields is truncated        |
     * The data value is not a valid interval literal     | 22018
     * =============================================================
     */

    SQL_LONG_VARIABLE_LENGTH_STRUCT * sParameter;
    stlChar                         * sValue;
    stlBool                           sSuccessWithInfo;
    stlInt64                          sLength;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_DAY,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aParameterValuePtr;
    sValue = (stlChar*)sParameter->arr;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( sValue );
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = stlStrlen( sValue );
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlIntervalDayToSecondSetValueFromString( 
                 sValue,
                 sLength,
                 aIpdRec->mDatetimeIntervalPrecision,
                 DTL_SCALE_NA,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_DAY,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 ZLS_STMT_DT_VECTOR(aStmt),
                 aStmt,
                 ZLS_STMT_NLS_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_LONGVARCHAR -> SQL_INTERVAL_HOUR
 */
ZLV_DECLARE_C_TO_SQL( LongVarChar, IntervalHour )
{
    /*
     * =============================================================
     * Test                                               | SQLSTATE
     * =============================================================
     * Data value is a valid interval value;              | n/a 
     * no truncation occurs                               |
     * Data value is a valid interval value;              | 22015
     * the value in one of the fields is truncated        |
     * The data value is not a valid interval literal     | 22018
     * =============================================================
     */

    SQL_LONG_VARIABLE_LENGTH_STRUCT * sParameter;
    stlChar                         * sValue;
    stlBool                           sSuccessWithInfo;
    stlInt64                          sLength;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_HOUR,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aParameterValuePtr;
    sValue = (stlChar*)sParameter->arr;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( sValue );
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = stlStrlen( sValue );
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlIntervalDayToSecondSetValueFromString( 
                 sValue,
                 sLength,
                 aIpdRec->mDatetimeIntervalPrecision,
                 DTL_SCALE_NA,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_HOUR,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 ZLS_STMT_DT_VECTOR(aStmt),
                 aStmt,
                 ZLS_STMT_NLS_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_LONGVARCHAR -> SQL_INTERVAL_MINUTE
 */
ZLV_DECLARE_C_TO_SQL( LongVarChar, IntervalMinute )
{
    /*
     * =============================================================
     * Test                                               | SQLSTATE
     * =============================================================
     * Data value is a valid interval value;              | n/a 
     * no truncation occurs                               |
     * Data value is a valid interval value;              | 22015
     * the value in one of the fields is truncated        |
     * The data value is not a valid interval literal     | 22018
     * =============================================================
     */

    SQL_LONG_VARIABLE_LENGTH_STRUCT * sParameter;
    stlChar                         * sValue;
    stlBool                           sSuccessWithInfo;
    stlInt64                          sLength;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_MINUTE,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aParameterValuePtr;
    sValue = (stlChar*)sParameter->arr;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( sValue );
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = stlStrlen( sValue );
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlIntervalDayToSecondSetValueFromString( 
                 sValue,
                 sLength,
                 aIpdRec->mDatetimeIntervalPrecision,
                 DTL_SCALE_NA,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_MINUTE,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 ZLS_STMT_DT_VECTOR(aStmt),
                 aStmt,
                 ZLS_STMT_NLS_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_LONGVARCHAR -> SQL_INTERVAL_SECOND
 */
ZLV_DECLARE_C_TO_SQL( LongVarChar, IntervalSecond )
{
    /*
     * =============================================================
     * Test                                               | SQLSTATE
     * =============================================================
     * Data value is a valid interval value;              | n/a 
     * no truncation occurs                               |
     * Data value is a valid interval value;              | 22015
     * the value in one of the fields is truncated        |
     * The data value is not a valid interval literal     | 22018
     * =============================================================
     */

    SQL_LONG_VARIABLE_LENGTH_STRUCT * sParameter;
    stlChar                         * sValue;
    stlBool                           sSuccessWithInfo;
    stlInt64                          sLength;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      aIpdRec->mPrecision,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_SECOND,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aParameterValuePtr;
    sValue = (stlChar*)sParameter->arr;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( sValue );
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = stlStrlen( sValue );
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlIntervalDayToSecondSetValueFromString( 
                 sValue,
                 sLength,
                 aIpdRec->mDatetimeIntervalPrecision,
                 aIpdRec->mPrecision,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_SECOND,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 ZLS_STMT_DT_VECTOR(aStmt),
                 aStmt,
                 ZLS_STMT_NLS_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_LONGVARCHAR -> SQL_INTERVAL_DAY_TO_HOUR
 */
ZLV_DECLARE_C_TO_SQL( LongVarChar, IntervalDayToHour )
{
    /*
     * =============================================================
     * Test                                               | SQLSTATE
     * =============================================================
     * Data value is a valid interval value;              | n/a 
     * no truncation occurs                               |
     * Data value is a valid interval value;              | 22015
     * the value in one of the fields is truncated        |
     * The data value is not a valid interval literal     | 22018
     * =============================================================
     */

    SQL_LONG_VARIABLE_LENGTH_STRUCT * sParameter;
    stlChar                         * sValue;
    stlBool                           sSuccessWithInfo;
    stlInt64                          sLength;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_DAY_TO_HOUR,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aParameterValuePtr;
    sValue = (stlChar*)sParameter->arr;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( sValue );
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = stlStrlen( sValue );
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlIntervalDayToSecondSetValueFromString( 
                 sValue,
                 sLength,
                 aIpdRec->mDatetimeIntervalPrecision,
                 DTL_SCALE_NA,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_DAY_TO_HOUR,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 ZLS_STMT_DT_VECTOR(aStmt),
                 aStmt,
                 ZLS_STMT_NLS_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_LONGVARCHAR -> SQL_INTERVAL_DAY_TO_MINUTE
 */
ZLV_DECLARE_C_TO_SQL( LongVarChar, IntervalDayToMinute )
{
    /*
     * =============================================================
     * Test                                               | SQLSTATE
     * =============================================================
     * Data value is a valid interval value;              | n/a 
     * no truncation occurs                               |
     * Data value is a valid interval value;              | 22015
     * the value in one of the fields is truncated        |
     * The data value is not a valid interval literal     | 22018
     * =============================================================
     */

    SQL_LONG_VARIABLE_LENGTH_STRUCT * sParameter;
    stlChar                         * sValue;
    stlBool                           sSuccessWithInfo;
    stlInt64                          sLength;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aParameterValuePtr;
    sValue = (stlChar*)sParameter->arr;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( sValue );
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = stlStrlen( sValue );
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlIntervalDayToSecondSetValueFromString( 
                 sValue,
                 sLength,
                 aIpdRec->mDatetimeIntervalPrecision,
                 DTL_SCALE_NA,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 ZLS_STMT_DT_VECTOR(aStmt),
                 aStmt,
                 ZLS_STMT_NLS_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_LONGVARCHAR -> SQL_INTERVAL_DAY_TO_SECOND
 */
ZLV_DECLARE_C_TO_SQL( LongVarChar, IntervalDayToSecond )
{
    /*
     * =============================================================
     * Test                                               | SQLSTATE
     * =============================================================
     * Data value is a valid interval value;              | n/a 
     * no truncation occurs                               |
     * Data value is a valid interval value;              | 22015
     * the value in one of the fields is truncated        |
     * The data value is not a valid interval literal     | 22018
     * =============================================================
     */

    SQL_LONG_VARIABLE_LENGTH_STRUCT * sParameter;
    stlChar                         * sValue;
    stlBool                           sSuccessWithInfo;
    stlInt64                          sLength;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      aIpdRec->mPrecision,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_DAY_TO_SECOND,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aParameterValuePtr;
    sValue = (stlChar*)sParameter->arr;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( sValue );
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = stlStrlen( sValue );
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlIntervalDayToSecondSetValueFromString( 
                 sValue,
                 sLength,
                 aIpdRec->mDatetimeIntervalPrecision,
                 aIpdRec->mPrecision,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_DAY_TO_SECOND,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 ZLS_STMT_DT_VECTOR(aStmt),
                 aStmt,
                 ZLS_STMT_NLS_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_LONGVARCHAR -> SQL_INTERVAL_HOUR_TO_MINUTE
 */
ZLV_DECLARE_C_TO_SQL( LongVarChar, IntervalHourToMinute )
{
    /*
     * =============================================================
     * Test                                               | SQLSTATE
     * =============================================================
     * Data value is a valid interval value;              | n/a 
     * no truncation occurs                               |
     * Data value is a valid interval value;              | 22015
     * the value in one of the fields is truncated        |
     * The data value is not a valid interval literal     | 22018
     * =============================================================
     */

    SQL_LONG_VARIABLE_LENGTH_STRUCT * sParameter;
    stlChar                         * sValue;
    stlBool                           sSuccessWithInfo;
    stlInt64                          sLength;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aParameterValuePtr;
    sValue = (stlChar*)sParameter->arr;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( sValue );
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = stlStrlen( sValue );
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlIntervalDayToSecondSetValueFromString( 
                 sValue,
                 sLength,
                 aIpdRec->mDatetimeIntervalPrecision,
                 DTL_SCALE_NA,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 ZLS_STMT_DT_VECTOR(aStmt),
                 aStmt,
                 ZLS_STMT_NLS_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_LONGVARCHAR -> SQL_INTERVAL_HOUR_TO_SECOND
 */
ZLV_DECLARE_C_TO_SQL( LongVarChar, IntervalHourToSecond )
{
    /*
     * =============================================================
     * Test                                               | SQLSTATE
     * =============================================================
     * Data value is a valid interval value;              | n/a 
     * no truncation occurs                               |
     * Data value is a valid interval value;              | 22015
     * the value in one of the fields is truncated        |
     * The data value is not a valid interval literal     | 22018
     * =============================================================
     */

    SQL_LONG_VARIABLE_LENGTH_STRUCT * sParameter;
    stlChar                         * sValue;
    stlBool                           sSuccessWithInfo;
    stlInt64                          sLength;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      aIpdRec->mPrecision,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aParameterValuePtr;
    sValue = (stlChar*)sParameter->arr;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( sValue );
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = stlStrlen( sValue );
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlIntervalDayToSecondSetValueFromString( 
                 sValue,
                 sLength,
                 aIpdRec->mDatetimeIntervalPrecision,
                 aIpdRec->mPrecision,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 ZLS_STMT_DT_VECTOR(aStmt),
                 aStmt,
                 ZLS_STMT_NLS_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_LONGVARCHAR -> SQL_INTERVAL_MINUTE_TO_SECOND
 */
ZLV_DECLARE_C_TO_SQL( LongVarChar, IntervalMinuteToSecond )
{
    /*
     * =============================================================
     * Test                                               | SQLSTATE
     * =============================================================
     * Data value is a valid interval value;              | n/a 
     * no truncation occurs                               |
     * Data value is a valid interval value;              | 22015
     * the value in one of the fields is truncated        |
     * The data value is not a valid interval literal     | 22018
     * =============================================================
     */

    SQL_LONG_VARIABLE_LENGTH_STRUCT * sParameter;
    stlChar                         * sValue;
    stlBool                           sSuccessWithInfo;
    stlInt64                          sLength;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      aIpdRec->mPrecision,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aParameterValuePtr;
    sValue = (stlChar*)sParameter->arr;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( sValue );
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = stlStrlen( sValue );
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlIntervalDayToSecondSetValueFromString( sValue,
                                                       sLength,
                                                       aIpdRec->mDatetimeIntervalPrecision,
                                                       aIpdRec->mPrecision,
                                                       DTL_STRING_LENGTH_UNIT_NA,
                                                       DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND,
                                                       aIpdRec->mOctetLength,
                                                       aDataValue,
                                                       &sSuccessWithInfo,
                                                       ZLS_STMT_DT_VECTOR(aStmt),
                                                       aStmt,
                                                       ZLS_STMT_NLS_DT_VECTOR(aStmt),
                                                       aStmt,
                                                       aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_LONGVARCHAR -> SQL_ROWID
 */
ZLV_DECLARE_C_TO_SQL( LongVarChar, RowId )
{
    SQL_LONG_VARIABLE_LENGTH_STRUCT * sParameter;
    stlChar                         * sValue;
    stlBool                           sSuccessWithInfo;
    stlInt64                          sLength;

    sParameter = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aParameterValuePtr;
    sValue = (stlChar*)sParameter->arr;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( sValue );
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = stlStrlen( sValue );
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlRowIdSetValueFromString( sValue,
                                         sLength,
                                         DTL_PRECISION_NA,
                                         DTL_SCALE_NA,
                                         DTL_STRING_LENGTH_UNIT_NA,
                                         DTL_INTERVAL_INDICATOR_NA,
                                         aIpdRec->mOctetLength,
                                         aDataValue,
                                         &sSuccessWithInfo,
                                         ZLS_STMT_DT_VECTOR(aStmt),
                                         aStmt,
                                         ZLS_STMT_NLS_DT_VECTOR(aStmt),
                                         aStmt,
                                         aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;    
}

/** @} */

