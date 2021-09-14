/*******************************************************************************
 * zlvCWcharToSql.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zlvCWcharToSql.c 15492 2015-07-24 06:40:51Z htkim $
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
#include <zlvCharacterset.h>

/**
 * @file zlvCWcharToSql.c
 * @brief Gliese API Internal Converting Data from C Character to SQL Data Types Routines.
 */

/**
 * @addtogroup zlvCWcharToSql
 * @{
 */

/*
 * http://msdn.microsoft.com/en-us/library/windows/desktop/ms714651%28v=VS.85%29.aspx
 */

/*
 * SQL_C_WCHAR -> SQL_CHAR
 */
ZLV_DECLARE_C_TO_SQL( Wchar, Char )
{
    /*
     * ===================================================================
     * Test                                  | SQLSTATE
     * ===================================================================
     * Byte length of data <= Column length. | n/a
     * Byte length of data > Column length.  | 22001
     * ===================================================================
     */

    stlBool            sSuccessWithInfo;
    stlInt64           sLength;
    dtlUnicodeEncoding sEncoding;

#ifdef SQL_WCHART_CONVERT
    sEncoding = DTL_UNICODE_UTF32;
#else
    sEncoding = DTL_UNICODE_UTF16;
#endif

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mLength,
                                      DTL_SCALE_NA,
                                      aIpdRec->mStringLengthUnit,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    if( aIndicator == NULL )
    {
        sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
        }
        else
        {
            sLength = *aIndicator;
        }
    }
    
    STL_TRY( dtlCharSetValueFromWCharStringWithoutPaddNull( sEncoding,
                                                            aParameterValuePtr,
                                                            sLength,
                                                            aIpdRec->mLength,
                                                            DTL_SCALE_NA,
                                                            aIpdRec->mStringLengthUnit,
                                                            DTL_INTERVAL_INDICATOR_NA,
                                                            aIpdRec->mOctetLength,
                                                            aDataValue,
                                                            &sSuccessWithInfo,
                                                            ZLS_STMT_NLS_DT_VECTOR(aStmt),
                                                            aStmt,
                                                            aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/*
 * SQL_C_WCHAR -> SQL_VARCHAR
 */
ZLV_DECLARE_C_TO_SQL( Wchar, VarChar )
{
    /*
     * ===================================================================
     * Test                                  | SQLSTATE
     * ===================================================================
     * Byte length of data <= Column length. | n/a
     * Byte length of data > Column length.  | 22001
     * ===================================================================
     */

    stlBool            sSuccessWithInfo;
    stlInt64           sLength;
    dtlUnicodeEncoding sEncoding;

#ifdef SQL_WCHART_CONVERT
    sEncoding = DTL_UNICODE_UTF32;
#else
    sEncoding = DTL_UNICODE_UTF16;
#endif

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mLength,
                                      DTL_SCALE_NA,
                                      aIpdRec->mStringLengthUnit,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    if( aIndicator == NULL )
    {
        sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlVarcharSetValueFromWCharString( sEncoding,
                                                aParameterValuePtr,
                                                sLength,
                                                aIpdRec->mLength,
                                                DTL_SCALE_NA,
                                                aIpdRec->mStringLengthUnit,
                                                DTL_INTERVAL_INDICATOR_NA,
                                                aIpdRec->mOctetLength,
                                                aDataValue,
                                                &sSuccessWithInfo,
                                                ZLS_STMT_NLS_DT_VECTOR(aStmt),
                                                aStmt,
                                                aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_WCHAR -> SQL_LONGVARCHAR
 */
ZLV_DECLARE_C_TO_SQL( Wchar, LongVarChar )
{
    /*
     * ===================================================================
     * Test                                  | SQLSTATE
     * ===================================================================
     * Byte length of data <= Column length. | n/a
     * Byte length of data > Column length.  | 22001
     * ===================================================================
     */

    stlBool            sSuccessWithInfo;
    stlInt64           sLength;
    dtlUnicodeEncoding sEncoding;

#ifdef SQL_WCHART_CONVERT
    sEncoding = DTL_UNICODE_UTF32;
#else
    sEncoding = DTL_UNICODE_UTF16;
#endif

    if( aIndicator == NULL )
    {
        sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
        }
        else
        {
            sLength = *aIndicator;
        }
    }
    STL_TRY( dtlLongvarcharSetValueFromWCharString( sEncoding,
                                                    aParameterValuePtr,
                                                    sLength,
                                                    aIpdRec->mLength,
                                                    DTL_SCALE_NA,
                                                    DTL_STRING_LENGTH_UNIT_OCTETS,
                                                    DTL_INTERVAL_INDICATOR_NA,
                                                    aIpdRec->mOctetLength,
                                                    aDataValue,
                                                    &sSuccessWithInfo,
                                                    ZLS_STMT_NLS_DT_VECTOR(aStmt),
                                                    aStmt,
                                                    aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_WCHAR -> SQL_FLOAT
 */
ZLV_DECLARE_C_TO_SQL( Wchar, Float )
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

    stlBool            sSuccessWithInfo;
    stlInt64           sLength;
    dtlUnicodeEncoding sEncoding;

#ifdef SQL_WCHART_CONVERT
    sEncoding = DTL_UNICODE_UTF32;
#else
    sEncoding = DTL_UNICODE_UTF16;
#endif

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    if( aIndicator == NULL )
    {
        sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlFloatSetValueFromWCharString( sEncoding,
                                              aParameterValuePtr,
                                              sLength,
                                              aIpdRec->mPrecision,
                                              DTL_SCALE_NA,
                                              DTL_STRING_LENGTH_UNIT_NA,
                                              DTL_INTERVAL_INDICATOR_NA,
                                              aIpdRec->mOctetLength,
                                              aDataValue,
                                              &sSuccessWithInfo,
                                              ZLS_STMT_NLS_DT_VECTOR(aStmt),
                                              aStmt,
                                              aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_WCHAR -> SQL_NUMERIC
 */
ZLV_DECLARE_C_TO_SQL( Wchar, Numeric )
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

    stlBool            sSuccessWithInfo;
    stlInt64           sLength;
    dtlUnicodeEncoding sEncoding;

#ifdef SQL_WCHART_CONVERT
    sEncoding = DTL_UNICODE_UTF32;
#else
    sEncoding = DTL_UNICODE_UTF16;
#endif

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mPrecision,
                                      aIpdRec->mScale,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    if( aIndicator == NULL )
    {
        sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlNumericSetValueFromWCharString( sEncoding,
                                                aParameterValuePtr,
                                                sLength,
                                                aIpdRec->mPrecision,
                                                aIpdRec->mScale,
                                                DTL_STRING_LENGTH_UNIT_NA,
                                                DTL_INTERVAL_INDICATOR_NA,
                                                aIpdRec->mOctetLength,
                                                aDataValue,
                                                &sSuccessWithInfo,
                                                ZLS_STMT_NLS_DT_VECTOR(aStmt),
                                                aStmt,
                                                aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_WCHAR -> SQL_TINYINT
 */
ZLV_DECLARE_C_TO_SQL( Wchar, TinyInt )
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

    stlChar   * sParameter;
    stlInt64    sLength;
    stlInt64    sConvertedResult;
    stlChar   * sStartPtr;
    stlChar   * sEndPtr;
    stlBool     sSuccessWithInfo;
    stlInt64    i;

    if( aIndicator == NULL )
    {
        sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr );
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr );
        }
        else
        {
            sLength = *aIndicator / STL_SIZEOF(SQLWCHAR);
        }
    }

    STL_TRY( stlAllocRegionMem( &aStmt->mAllocator,
                                sLength + 1,
                                (void**)&sParameter,
                                aErrorStack ) == STL_SUCCESS );

    for( i = 0; i < sLength; i++ )
    {
        sParameter[i] = ((SQLWCHAR*)aParameterValuePtr)[i];
    }

    sParameter[i] = 0;
    
    sStartPtr = sParameter;
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
                                             ZLS_STMT_NLS_DT_VECTOR(aStmt),
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
 * SQL_C_WCHAR -> SQL_SMALLINT
 */
ZLV_DECLARE_C_TO_SQL( Wchar, SmallInt )
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

    stlChar  * sParameter;
    stlInt64   sLength;
    stlInt64   sConvertedResult;
    stlChar  * sStartPtr;
    stlChar  * sEndPtr;
    stlBool    sSuccessWithInfo;
    stlInt64   i;

    if( aIndicator == NULL )
    {
        sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr );
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr );
        }
        else
        {
            sLength = *aIndicator / STL_SIZEOF(SQLWCHAR);
        }
    }

    STL_TRY( stlAllocRegionMem( &aStmt->mAllocator,
                                sLength + 1,
                                (void**)&sParameter,
                                aErrorStack ) == STL_SUCCESS );

    for( i = 0; i < sLength; i++ )
    {
        sParameter[i] = ((SQLWCHAR*)aParameterValuePtr)[i];
    }

    sParameter[i] = 0;

    sStartPtr = sParameter;
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
                                             ZLS_STMT_NLS_DT_VECTOR(aStmt),
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
 * SQL_C_WCHAR -> SQL_INTEGER
 */
ZLV_DECLARE_C_TO_SQL( Wchar, Integer )
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

    stlChar  * sParameter;
    stlInt64   sLength;
    stlInt64   sConvertedResult;
    stlChar  * sStartPtr;
    stlChar  * sEndPtr;
    stlBool    sSuccessWithInfo;
    stlInt64   i;

    if( aIndicator == NULL )
    {
        sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr );
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr );
        }
        else
        {
            sLength = *aIndicator / STL_SIZEOF(SQLWCHAR);
        }
    }

    STL_TRY( stlAllocRegionMem( &aStmt->mAllocator,
                                sLength + 1,
                                (void**)&sParameter,
                                aErrorStack ) == STL_SUCCESS );

    for( i = 0; i < sLength; i++ )
    {
        sParameter[i] = ((SQLWCHAR*)aParameterValuePtr)[i];
    }

    sParameter[i] = 0;

    sStartPtr = sParameter;
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
                                            ZLS_STMT_NLS_DT_VECTOR(aStmt),
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
 * SQL_C_WCHAR -> SQL_BIGINT
 */
ZLV_DECLARE_C_TO_SQL( Wchar, BigInt )
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

    stlChar  * sParameter;
    stlInt64   sLength;
    stlInt64   sConvertedResult;
    stlChar  * sStartPtr;
    stlChar  * sEndPtr;
    stlBool    sSuccessWithInfo;
    stlInt64   i;

    if( aIndicator == NULL )
    {
        sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr );
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr );
        }
        else
        {
            sLength = *aIndicator / STL_SIZEOF(SQLWCHAR);
        }
    }

    STL_TRY( stlAllocRegionMem( &aStmt->mAllocator,
                                sLength + 1,
                                (void**)&sParameter,
                                aErrorStack ) == STL_SUCCESS );

    for( i = 0; i < sLength; i++ )
    {
        sParameter[i] = ((SQLWCHAR*)aParameterValuePtr)[i];
    }

    sParameter[i] = 0;

    sStartPtr = sParameter;
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
                                           ZLS_STMT_NLS_DT_VECTOR(aStmt),
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
 * SQL_C_WCHAR -> SQL_REAL
 */
ZLV_DECLARE_C_TO_SQL( Wchar, Real )
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

    stlChar    * sParameter;
    stlInt64     sLength;
    stlFloat32   sConvertedResult;
    stlChar    * sStartPtr;
    stlChar    * sEndPtr;
    stlBool      sSuccessWithInfo;
    stlInt64     i;

    if( aIndicator == NULL )
    {
        sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr );
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr );
        }
        else
        {
            sLength = *aIndicator / STL_SIZEOF(SQLWCHAR);
        }
    }

    STL_TRY( stlAllocRegionMem( &aStmt->mAllocator,
                                sLength + 1,
                                (void**)&sParameter,
                                aErrorStack ) == STL_SUCCESS );

    for( i = 0; i < sLength; i++ )
    {
        sParameter[i] = ((SQLWCHAR*)aParameterValuePtr)[i];
    }

    sParameter[i] = 0;

    sStartPtr = sParameter;
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
                                      ZLS_STMT_NLS_DT_VECTOR(aStmt),
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
 * SQL_C_WCHAR -> SQL_DOUBLE
 */
ZLV_DECLARE_C_TO_SQL( Wchar, Double )
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

    stlChar    * sParameter;
    stlInt64     sLength;
    stlFloat64   sConvertedResult;
    stlChar    * sStartPtr;
    stlChar    * sEndPtr;
    stlBool      sSuccessWithInfo;
    stlInt64     i;

    if( aIndicator == NULL )
    {
        sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr );
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr );
        }
        else
        {
            sLength = *aIndicator / STL_SIZEOF(SQLWCHAR);
        }
    }

    STL_TRY( stlAllocRegionMem( &aStmt->mAllocator,
                                sLength + 1,
                                (void**)&sParameter,
                                aErrorStack ) == STL_SUCCESS );

    for( i = 0; i < sLength; i++ )
    {
        sParameter[i] = ((SQLWCHAR*)aParameterValuePtr)[i];
    }

    sParameter[i] = 0;

    sStartPtr = sParameter;
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
                                        ZLS_STMT_NLS_DT_VECTOR(aStmt),
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
 * SQL_C_WCHAR -> SQL_BIT
 */
ZLV_DECLARE_C_TO_SQL( Wchar, Bit )
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

    stlChar  * sParameter;
    stlInt64   sLength;
    stlInt64   sConvertedResult;
    stlChar  * sStartPtr;
    stlChar  * sEndPtr;
    stlBool    sSuccessWithInfo;
    stlInt64   i;

    if( aIndicator == NULL )
    {
        sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr );
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr );
        }
        else
        {
            sLength = *aIndicator / STL_SIZEOF(SQLWCHAR);
        }
    }

    STL_TRY( stlAllocRegionMem( &aStmt->mAllocator,
                                sLength + 1,
                                (void**)&sParameter,
                                aErrorStack ) == STL_SUCCESS );

    for( i = 0; i < sLength; i++ )
    {
        sParameter[i] = ((SQLWCHAR*)aParameterValuePtr)[i];
    }

    sParameter[i] = 0;

    sStartPtr = sParameter;
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
                                            ZLS_STMT_NLS_DT_VECTOR(aStmt),
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
 * SQL_C_WCHAR -> SQL_BOOLEAN
 */
ZLV_DECLARE_C_TO_SQL( Wchar, Boolean )
{
    stlInt64           sLength;
    stlBool            sSuccessWithInfo;
    dtlUnicodeEncoding sEncoding;

#ifdef SQL_WCHART_CONVERT
    sEncoding = DTL_UNICODE_UTF32;
#else
    sEncoding = DTL_UNICODE_UTF16;
#endif

    if( aIndicator == NULL )
    {
        sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlBooleanSetValueFromWCharString( sEncoding,
                                                aParameterValuePtr,
                                                sLength,
                                                DTL_PRECISION_NA,
                                                DTL_SCALE_NA,
                                                DTL_STRING_LENGTH_UNIT_NA,
                                                DTL_INTERVAL_INDICATOR_NA,
                                                aIpdRec->mOctetLength,
                                                aDataValue,
                                                &sSuccessWithInfo,
                                                ZLS_STMT_NLS_DT_VECTOR(aStmt),
                                                aStmt,
                                                aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_WCHAR -> SQL_BINARY
 */
ZLV_DECLARE_C_TO_SQL( Wchar, Binary )
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

    stlBool            sSuccessWithInfo;
    stlInt64           sLength;
    dtlUnicodeEncoding sEncoding;

#ifdef SQL_WCHART_CONVERT
    sEncoding = DTL_UNICODE_UTF32;
#else
    sEncoding = DTL_UNICODE_UTF16;
#endif

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mLength,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    if( aIndicator == NULL )
    {
        sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY_THROW( (sLength / STL_SIZEOF(SQLWCHAR) / 2 ) <= aIpdRec->mLength,
                   RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED);

    STL_TRY( dtlBinarySetValueFromWCharStringWithoutPaddNull( sEncoding,
                                                              aParameterValuePtr,
                                                              sLength,
                                                              aIpdRec->mLength,
                                                              DTL_SCALE_NA,
                                                              DTL_STRING_LENGTH_UNIT_NA,
                                                              DTL_INTERVAL_INDICATOR_NA,
                                                              aIpdRec->mOctetLength,
                                                              aDataValue,
                                                              &sSuccessWithInfo,
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
 * SQL_C_WCHAR -> SQL_VARBINARY
 */
ZLV_DECLARE_C_TO_SQL( Wchar, VarBinary )
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

    stlBool            sSuccessWithInfo;
    stlInt64           sLength;
    dtlUnicodeEncoding sEncoding;

#ifdef SQL_WCHART_CONVERT
    sEncoding = DTL_UNICODE_UTF32;
#else
    sEncoding = DTL_UNICODE_UTF16;
#endif

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mLength,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    if( aIndicator == NULL )
    {
        sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY_THROW( (sLength / STL_SIZEOF(SQLWCHAR) / 2 ) <= aIpdRec->mLength,
                   RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED);

    STL_TRY( dtlVarbinarySetValueFromWCharString( sEncoding,
                                                  aParameterValuePtr,
                                                  sLength,
                                                  aIpdRec->mLength,
                                                  DTL_SCALE_NA,
                                                  DTL_STRING_LENGTH_UNIT_NA,
                                                  DTL_INTERVAL_INDICATOR_NA,
                                                  aIpdRec->mOctetLength,
                                                  aDataValue,
                                                  &sSuccessWithInfo,
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
 * SQL_C_WCHAR -> SQL_LONGVARBINARY
 */
ZLV_DECLARE_C_TO_SQL( Wchar, LongVarBinary )
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

    stlBool            sSuccessWithInfo;
    stlInt64           sLength;
    dtlUnicodeEncoding sEncoding;

#ifdef SQL_WCHART_CONVERT
    sEncoding = DTL_UNICODE_UTF32;
#else
    sEncoding = DTL_UNICODE_UTF16;
#endif

    if( aIndicator == NULL )
    {
        sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlLongvarbinarySetValueFromWCharString( sEncoding,
                                                      aParameterValuePtr,
                                                      sLength,
                                                      aIpdRec->mLength,
                                                      DTL_SCALE_NA,
                                                      DTL_STRING_LENGTH_UNIT_NA,
                                                      DTL_INTERVAL_INDICATOR_NA,
                                                      aIpdRec->mOctetLength,
                                                      aDataValue,
                                                      &sSuccessWithInfo,
                                                      ZLS_STMT_NLS_DT_VECTOR(aStmt),
                                                      aStmt,
                                                      aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_WCHAR -> SQL_TYPE_DATE
 */
ZLV_DECLARE_C_TO_SQL( Wchar, Date )
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

    stlBool            sSuccessWithInfo;
    stlInt64           sLength;
    dtlUnicodeEncoding sEncoding;

#ifdef SQL_WCHART_CONVERT
    sEncoding = DTL_UNICODE_UTF32;
#else
    sEncoding = DTL_UNICODE_UTF16;
#endif

    if( aIndicator == NULL )
    {
        sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlDateSetValueFromWCharString( sEncoding,
                                             aParameterValuePtr,
                                             sLength,
                                             DTL_PRECISION_NA,
                                             DTL_SCALE_NA,
                                             DTL_STRING_LENGTH_UNIT_NA,
                                             DTL_INTERVAL_INDICATOR_NA,
                                             aIpdRec->mOctetLength,
                                             aDataValue,
                                             &sSuccessWithInfo,
                                             ZLS_STMT_NLS_DT_VECTOR(aStmt),
                                             aStmt,
                                             aErrorStack ) == STL_SUCCESS );
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_WCHAR -> SQL_TYPE_TIME
 */
ZLV_DECLARE_C_TO_SQL( Wchar, Time )
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

    stlBool            sSuccessWithInfo;
    stlInt64           sLength;
    dtlUnicodeEncoding sEncoding;

#ifdef SQL_WCHART_CONVERT
    sEncoding = DTL_UNICODE_UTF32;
#else
    sEncoding = DTL_UNICODE_UTF16;
#endif

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    if( aIndicator == NULL )
    {
        sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlTimeSetValueFromWCharString( sEncoding,
                                             aParameterValuePtr,
                                             sLength,
                                             aIpdRec->mPrecision,
                                             DTL_SCALE_NA,
                                             DTL_STRING_LENGTH_UNIT_NA,
                                             DTL_INTERVAL_INDICATOR_NA,
                                             aIpdRec->mOctetLength,
                                             aDataValue,
                                             &sSuccessWithInfo,
                                             ZLS_STMT_NLS_DT_VECTOR(aStmt),
                                             aStmt,
                                             aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_WCHAR -> SQL_TYPE_TIME_TZ
 */
ZLV_DECLARE_C_TO_SQL( Wchar, TimeTz )
{
    stlBool            sSuccessWithInfo;
    stlInt64           sLength;
    dtlUnicodeEncoding sEncoding;

#ifdef SQL_WCHART_CONVERT
    sEncoding = DTL_UNICODE_UTF32;
#else
    sEncoding = DTL_UNICODE_UTF16;
#endif

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    if( aIndicator == NULL )
    {
        sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlTimeTzSetValueFromWCharString( sEncoding,
                                               aParameterValuePtr,
                                               sLength,
                                               aIpdRec->mPrecision,
                                               DTL_SCALE_NA,
                                               DTL_STRING_LENGTH_UNIT_NA,
                                               DTL_INTERVAL_INDICATOR_NA,
                                               aIpdRec->mOctetLength,
                                               aDataValue,
                                               &sSuccessWithInfo,
                                               ZLS_STMT_NLS_DT_VECTOR(aStmt),
                                               aStmt,
                                               aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_WCHAR -> SQL_TYPE_TIMESTAMP
 */
ZLV_DECLARE_C_TO_SQL( Wchar, Timestamp )
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

    stlBool            sSuccessWithInfo;
    stlInt64           sLength;
    dtlUnicodeEncoding sEncoding;

#ifdef SQL_WCHART_CONVERT
    sEncoding = DTL_UNICODE_UTF32;
#else
    sEncoding = DTL_UNICODE_UTF16;
#endif

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    if( aIndicator == NULL )
    {
        sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlTimestampSetValueFromWCharString( sEncoding,
                                                  aParameterValuePtr,
                                                  sLength,
                                                  aIpdRec->mPrecision,
                                                  DTL_SCALE_NA,
                                                  DTL_STRING_LENGTH_UNIT_NA,
                                                  DTL_INTERVAL_INDICATOR_NA,
                                                  aIpdRec->mOctetLength,
                                                  aDataValue,
                                                  &sSuccessWithInfo,
                                                  ZLS_STMT_NLS_DT_VECTOR(aStmt),
                                                  aStmt,
                                                  aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_WCHAR -> SQL_TYPE_TIMESTAMP_TZ
 */
ZLV_DECLARE_C_TO_SQL( Wchar, TimestampTz )
{
    stlBool            sSuccessWithInfo;
    stlInt64           sLength;
    dtlUnicodeEncoding sEncoding;

#ifdef SQL_WCHART_CONVERT
    sEncoding = DTL_UNICODE_UTF32;
#else
    sEncoding = DTL_UNICODE_UTF16;
#endif

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    if( aIndicator == NULL )
    {
        sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlTimestampTzSetValueFromWCharString( sEncoding,
                                                    aParameterValuePtr,
                                                    sLength,
                                                    aIpdRec->mPrecision,
                                                    DTL_SCALE_NA,
                                                    DTL_STRING_LENGTH_UNIT_NA,
                                                    DTL_INTERVAL_INDICATOR_NA,
                                                    aIpdRec->mOctetLength,
                                                    aDataValue,
                                                    &sSuccessWithInfo,
                                                    ZLS_STMT_NLS_DT_VECTOR(aStmt),
                                                    aStmt,
                                                    aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_WCHAR -> SQL_INTERVAL_MONTH
 */
ZLV_DECLARE_C_TO_SQL( Wchar, IntervalMonth )
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

    stlBool            sSuccessWithInfo;
    stlInt64           sLength;
    dtlUnicodeEncoding sEncoding;

#ifdef SQL_WCHART_CONVERT
    sEncoding = DTL_UNICODE_UTF32;
#else
    sEncoding = DTL_UNICODE_UTF16;
#endif

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_MONTH,
                                      aErrorStack )
             == STL_SUCCESS );

    if( aIndicator == NULL )
    {
        sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlIntervalYearToMonthSetValueFromWCharString(
                 sEncoding,
                 aParameterValuePtr,
                 sLength,
                 aIpdRec->mDatetimeIntervalPrecision,
                 DTL_SCALE_NA,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_MONTH,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 ZLS_STMT_NLS_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_WCHAR -> SQL_INTERVAL_YEAR
 */
ZLV_DECLARE_C_TO_SQL( Wchar, IntervalYear )
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

    stlBool            sSuccessWithInfo;
    stlInt64           sLength;
    dtlUnicodeEncoding sEncoding;

#ifdef SQL_WCHART_CONVERT
    sEncoding = DTL_UNICODE_UTF32;
#else
    sEncoding = DTL_UNICODE_UTF16;
#endif

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_YEAR,
                                      aErrorStack )
             == STL_SUCCESS );

    if( aIndicator == NULL )
    {
        sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlIntervalYearToMonthSetValueFromWCharString(
                 sEncoding,
                 aParameterValuePtr,
                 sLength,
                 aIpdRec->mDatetimeIntervalPrecision,
                 DTL_SCALE_NA,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_YEAR,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 ZLS_STMT_NLS_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_WCHAR -> SQL_INTERVAL_YEAR_TO_MONTH
 */
ZLV_DECLARE_C_TO_SQL( Wchar, IntervalYearToMonth )
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

    stlBool            sSuccessWithInfo;
    stlInt64           sLength;
    dtlUnicodeEncoding sEncoding;

#ifdef SQL_WCHART_CONVERT
    sEncoding = DTL_UNICODE_UTF32;
#else
    sEncoding = DTL_UNICODE_UTF16;
#endif

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH,
                                      aErrorStack )
             == STL_SUCCESS );

    if( aIndicator == NULL )
    {
        sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlIntervalYearToMonthSetValueFromWCharString(
                 sEncoding,
                 aParameterValuePtr,
                 sLength,
                 aIpdRec->mDatetimeIntervalPrecision,
                 DTL_SCALE_NA,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 ZLS_STMT_NLS_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_WCHAR -> SQL_INTERVAL_DAY
 */
ZLV_DECLARE_C_TO_SQL( Wchar, IntervalDay )
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

    stlBool            sSuccessWithInfo;
    stlInt64           sLength;
    dtlUnicodeEncoding sEncoding;

#ifdef SQL_WCHART_CONVERT
    sEncoding = DTL_UNICODE_UTF32;
#else
    sEncoding = DTL_UNICODE_UTF16;
#endif

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_DAY,
                                      aErrorStack )
             == STL_SUCCESS );

    if( aIndicator == NULL )
    {
        sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlIntervalDayToSecondSetValueFromWCharString(
                 sEncoding,
                 aParameterValuePtr,
                 sLength,
                 aIpdRec->mDatetimeIntervalPrecision,
                 DTL_SCALE_NA,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_DAY,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 ZLS_STMT_NLS_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_WCHAR -> SQL_INTERVAL_HOUR
 */
ZLV_DECLARE_C_TO_SQL( Wchar, IntervalHour )
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

    stlBool            sSuccessWithInfo;
    stlInt64           sLength;
    dtlUnicodeEncoding sEncoding;

#ifdef SQL_WCHART_CONVERT
    sEncoding = DTL_UNICODE_UTF32;
#else
    sEncoding = DTL_UNICODE_UTF16;
#endif

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_HOUR,
                                      aErrorStack )
             == STL_SUCCESS );

    if( aIndicator == NULL )
    {
        sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlIntervalDayToSecondSetValueFromWCharString(
                 sEncoding,
                 aParameterValuePtr,
                 sLength,
                 aIpdRec->mDatetimeIntervalPrecision,
                 DTL_SCALE_NA,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_HOUR,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 ZLS_STMT_NLS_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_WCHAR -> SQL_INTERVAL_MINUTE
 */
ZLV_DECLARE_C_TO_SQL( Wchar, IntervalMinute )
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

    stlBool            sSuccessWithInfo;
    stlInt64           sLength;
    dtlUnicodeEncoding sEncoding;

#ifdef SQL_WCHART_CONVERT
    sEncoding = DTL_UNICODE_UTF32;
#else
    sEncoding = DTL_UNICODE_UTF16;
#endif

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_MINUTE,
                                      aErrorStack )
             == STL_SUCCESS );

    if( aIndicator == NULL )
    {
        sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlIntervalDayToSecondSetValueFromWCharString(
                 sEncoding,
                 aParameterValuePtr,
                 sLength,
                 aIpdRec->mDatetimeIntervalPrecision,
                 DTL_SCALE_NA,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_MINUTE,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 ZLS_STMT_NLS_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_WCHAR -> SQL_INTERVAL_SECOND
 */
ZLV_DECLARE_C_TO_SQL( Wchar, IntervalSecond )
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

    stlBool            sSuccessWithInfo;
    stlInt64           sLength;
    dtlUnicodeEncoding sEncoding;

#ifdef SQL_WCHART_CONVERT
    sEncoding = DTL_UNICODE_UTF32;
#else
    sEncoding = DTL_UNICODE_UTF16;
#endif

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      aIpdRec->mPrecision,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_SECOND,
                                      aErrorStack )
             == STL_SUCCESS );

    if( aIndicator == NULL )
    {
        sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlIntervalDayToSecondSetValueFromWCharString(
                 sEncoding,
                 aParameterValuePtr,
                 sLength,
                 aIpdRec->mDatetimeIntervalPrecision,
                 aIpdRec->mPrecision,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_SECOND,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 ZLS_STMT_NLS_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_WCHAR -> SQL_INTERVAL_DAY_TO_HOUR
 */
ZLV_DECLARE_C_TO_SQL( Wchar, IntervalDayToHour )
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

    stlBool            sSuccessWithInfo;
    stlInt64           sLength;
    dtlUnicodeEncoding sEncoding;

#ifdef SQL_WCHART_CONVERT
    sEncoding = DTL_UNICODE_UTF32;
#else
    sEncoding = DTL_UNICODE_UTF16;
#endif

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_DAY_TO_HOUR,
                                      aErrorStack )
             == STL_SUCCESS );

    if( aIndicator == NULL )
    {
        sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlIntervalDayToSecondSetValueFromWCharString(
                 sEncoding,
                 aParameterValuePtr,
                 sLength,
                 aIpdRec->mDatetimeIntervalPrecision,
                 DTL_SCALE_NA,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_DAY_TO_HOUR,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 ZLS_STMT_NLS_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_WCHAR -> SQL_INTERVAL_DAY_TO_MINUTE
 */
ZLV_DECLARE_C_TO_SQL( Wchar, IntervalDayToMinute )
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

    stlBool            sSuccessWithInfo;
    stlInt64           sLength;
    dtlUnicodeEncoding sEncoding;

#ifdef SQL_WCHART_CONVERT
    sEncoding = DTL_UNICODE_UTF32;
#else
    sEncoding = DTL_UNICODE_UTF16;
#endif

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE,
                                      aErrorStack )
             == STL_SUCCESS );

    if( aIndicator == NULL )
    {
        sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlIntervalDayToSecondSetValueFromWCharString(
                 sEncoding,
                 aParameterValuePtr,
                 sLength,
                 aIpdRec->mDatetimeIntervalPrecision,
                 DTL_SCALE_NA,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 ZLS_STMT_NLS_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_WCHAR -> SQL_INTERVAL_DAY_TO_SECOND
 */
ZLV_DECLARE_C_TO_SQL( Wchar, IntervalDayToSecond )
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

    stlBool            sSuccessWithInfo;
    stlInt64           sLength;
    dtlUnicodeEncoding sEncoding;

#ifdef SQL_WCHART_CONVERT
    sEncoding = DTL_UNICODE_UTF32;
#else
    sEncoding = DTL_UNICODE_UTF16;
#endif

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      aIpdRec->mPrecision,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_DAY_TO_SECOND,
                                      aErrorStack )
             == STL_SUCCESS );

    if( aIndicator == NULL )
    {
        sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlIntervalDayToSecondSetValueFromWCharString(
                 sEncoding,
                 aParameterValuePtr,
                 sLength,
                 aIpdRec->mDatetimeIntervalPrecision,
                 aIpdRec->mPrecision,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_DAY_TO_SECOND,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 ZLS_STMT_NLS_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_WCHAR -> SQL_INTERVAL_HOUR_TO_MINUTE
 */
ZLV_DECLARE_C_TO_SQL( Wchar, IntervalHourToMinute )
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

    stlBool            sSuccessWithInfo;
    stlInt64           sLength;
    dtlUnicodeEncoding sEncoding;

#ifdef SQL_WCHART_CONVERT
    sEncoding = DTL_UNICODE_UTF32;
#else
    sEncoding = DTL_UNICODE_UTF16;
#endif

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE,
                                      aErrorStack )
             == STL_SUCCESS );

    if( aIndicator == NULL )
    {
        sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlIntervalDayToSecondSetValueFromWCharString(
                 sEncoding,
                 aParameterValuePtr,
                 sLength,
                 aIpdRec->mDatetimeIntervalPrecision,
                 DTL_SCALE_NA,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 ZLS_STMT_NLS_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_WCHAR -> SQL_INTERVAL_HOUR_TO_SECOND
 */
ZLV_DECLARE_C_TO_SQL( Wchar, IntervalHourToSecond )
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

    stlBool            sSuccessWithInfo;
    stlInt64           sLength;
    dtlUnicodeEncoding sEncoding;

#ifdef SQL_WCHART_CONVERT
    sEncoding = DTL_UNICODE_UTF32;
#else
    sEncoding = DTL_UNICODE_UTF16;
#endif

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      aIpdRec->mPrecision,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND,
                                      aErrorStack )
             == STL_SUCCESS );

    if( aIndicator == NULL )
    {
        sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlIntervalDayToSecondSetValueFromWCharString(
                 sEncoding,
                 aParameterValuePtr,
                 sLength,
                 aIpdRec->mDatetimeIntervalPrecision,
                 aIpdRec->mPrecision,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 ZLS_STMT_NLS_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_WCHAR -> SQL_INTERVAL_MINUTE_TO_SECOND
 */
ZLV_DECLARE_C_TO_SQL( Wchar, IntervalMinuteToSecond )
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

    stlBool            sSuccessWithInfo;
    stlInt64           sLength;
    dtlUnicodeEncoding sEncoding;

#ifdef SQL_WCHART_CONVERT
    sEncoding = DTL_UNICODE_UTF32;
#else
    sEncoding = DTL_UNICODE_UTF16;
#endif

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      aIpdRec->mPrecision,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND,
                                      aErrorStack )
             == STL_SUCCESS );

    if( aIndicator == NULL )
    {
        sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlIntervalDayToSecondSetValueFromWCharString(
                 sEncoding,
                 aParameterValuePtr,
                 sLength,
                 aIpdRec->mDatetimeIntervalPrecision,
                 aIpdRec->mPrecision,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 ZLS_STMT_NLS_DT_VECTOR(aStmt),
                 aStmt,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_C_WCHAR -> SQL_ROWID
 */
ZLV_DECLARE_C_TO_SQL( Wchar, RowId )
{
    stlBool            sSuccessWithInfo;
    stlInt64           sLength;
    dtlUnicodeEncoding sEncoding;

#ifdef SQL_WCHART_CONVERT
    sEncoding = DTL_UNICODE_UTF32;
#else
    sEncoding = DTL_UNICODE_UTF16;
#endif

    if( aIndicator == NULL )
    {
        sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
    }
    else
    {
        if( *aIndicator == SQL_NTS )
        {
            sLength = zlvStrlenW( (SQLWCHAR*)aParameterValuePtr ) * STL_SIZEOF(SQLWCHAR);
        }
        else
        {
            sLength = *aIndicator;
        }
    }

    STL_TRY( dtlRowIdSetValueFromWCharString( sEncoding,
                                              aParameterValuePtr,
                                              sLength,
                                              DTL_PRECISION_NA,
                                              DTL_SCALE_NA,
                                              DTL_STRING_LENGTH_UNIT_NA,
                                              DTL_INTERVAL_INDICATOR_NA,
                                              aIpdRec->mOctetLength,
                                              aDataValue,
                                              &sSuccessWithInfo,
                                              ZLS_STMT_NLS_DT_VECTOR(aStmt),
                                              aStmt,
                                              aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;    
}

/** @} */
