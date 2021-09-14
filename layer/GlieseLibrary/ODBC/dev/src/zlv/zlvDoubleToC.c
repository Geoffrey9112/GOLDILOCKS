/*******************************************************************************
 * zlvDoubleToC.c
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
#include <zlvSqlToC.h>

/**
 * @file zlvCharToC.c
 * @brief Gliese API Internal Converting Data from SQL Double to C Data Types Routines.
 */

/**
 * @addtogroup zlvDoubleToC
 * @{
 */

/*
 * http://msdn.microsoft.com/en-us/library/windows/desktop/ms712567%28v=VS.85%29.aspx
 */

/*
 * SQL_DOUBLE -> SQL_C_CHAR
 */
ZLV_DECLARE_SQL_TO_C( Double, Char )
{
    /*
     * ========================================================================================================================
     * Test                                                              | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ========================================================================================================================
     * Character byte length < BufferLength                              | Data            | Length of data in bytes | n/a
     * Number of whole (as opposed to fractional) digits < BufferLength  | Truncated data  | Length of data in bytes | 01004
     * Number of whole (as opposed to fractional) digits >= BufferLength | Undefined       | Undefined               | 22003
     * ========================================================================================================================
     */

    dtlDoubleType * sDataValue;
    stlInt64        sValidNumberLength;    
    stlInt64        sLength;
    stlInt64        sBufferLength;
    stlChar         sDoubleString[DTL_NATIVE_DOUBLE_STRING_SIZE + 1];
    stlStatus       sRet = STL_FAILURE;
    
    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlDoubleType*)aDataValue->mValue;

    STL_TRY_THROW( stlIsfinite( *sDataValue ) != STL_IS_FINITE_FALSE,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    sBufferLength = DTL_NATIVE_DOUBLE_DISPLAY_SIZE < ( aArdRec->mOctetLength -1 ) ?
        DTL_NATIVE_DOUBLE_DISPLAY_SIZE : ( aArdRec->mOctetLength -1 );

    STL_TRY( dtlGetValidNumberLengthFromFloat64( sDataValue,
                                                 & sValidNumberLength,
                                                 aErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( sValidNumberLength <= sBufferLength ,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    STL_TRY( dtlToStringDataValue( aDataValue,
                                   DTL_GET_BLOCK_ARG_NUM1( aValueBlockList ),
                                   DTL_GET_BLOCK_ARG_NUM2( aValueBlockList ),
                                   DTL_NATIVE_DOUBLE_DISPLAY_SIZE,
                                   sDoubleString,
                                   & sLength,
                                   ZLS_STMT_DT_VECTOR(aStmt),
                                   aStmt,
                                   aErrorStack )
             == STL_SUCCESS );

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = sLength;
    }

    if( sBufferLength >= sLength )
    {
        stlMemcpy( (stlChar *)aTargetValuePtr,
                   (const stlChar *)sDoubleString,
                   sLength );
        ((stlChar*)aTargetValuePtr)[ sLength ] = '\0';
    }
    else
    {
        stlMemcpy( (stlChar *)aTargetValuePtr,
                   (const stlChar *)sDoubleString,
                   sBufferLength );
        ((stlChar*)aTargetValuePtr)[ sBufferLength ] = '\0';

        STL_THROW( RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED );
    }

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                      "Returning the numeric value as numeric or string for one or "
                      "more bound columns would have caused the whole "
                      "(as opposed to fractional) part of the number to be truncated.",
                      aErrorStack );
    }
    
    STL_CATCH( RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED )
    {
        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_STRING_DATA_RIGHT_TRUNCATED,
                      "String or binary data returned for a column resulted "
                      "in the truncation of nonblank character or non-NULL binary data. "
                      "If it was a string value, it was right-truncated.",
                      aErrorStack );

        sRet     = STL_SUCCESS;
        *aReturn = SQL_SUCCESS_WITH_INFO;
    }

    STL_FINISH;
    
    return sRet;
}

/*
 * SQL_DOUBLE -> SQL_C_WCHAR
 */
ZLV_DECLARE_SQL_TO_C( Double, Wchar )
{
    /*
     * ========================================================================================================================
     * Test                                                              | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ========================================================================================================================
     * Character byte length < BufferLength                              | Data            | Length of data in bytes | n/a
     * Number of whole (as opposed to fractional) digits < BufferLength  | Truncated data  | Length of data in bytes | 01004
     * Number of whole (as opposed to fractional) digits >= BufferLength | Undefined       | Undefined               | 22003
     * ========================================================================================================================
     */

    dtlDoubleType * sDataValue;
    stlInt64        sValidNumberLength;    
    stlInt64        sLength;
    stlInt64        sBufferLength;
    stlChar         sDoubleString[DTL_NATIVE_DOUBLE_STRING_SIZE + 1];
    stlStatus       sRet = STL_FAILURE;
    stlInt64        sAvailableSize = 0;
    stlInt64        i;
    
    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlDoubleType*)aDataValue->mValue;

    STL_TRY_THROW( stlIsfinite( *sDataValue ) != STL_IS_FINITE_FALSE,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    sAvailableSize = aArdRec->mOctetLength - STL_SIZEOF(SQLWCHAR);
    
    sBufferLength = DTL_NATIVE_DOUBLE_DISPLAY_SIZE < (sAvailableSize / STL_SIZEOF(SQLWCHAR)) ?
        DTL_NATIVE_DOUBLE_DISPLAY_SIZE : (sAvailableSize / STL_SIZEOF(SQLWCHAR));

    STL_TRY( dtlGetValidNumberLengthFromFloat64( sDataValue,
                                                 & sValidNumberLength,
                                                 aErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( sValidNumberLength <= sBufferLength ,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    STL_TRY( dtlToStringDataValue( aDataValue,
                                   DTL_GET_BLOCK_ARG_NUM1( aValueBlockList ),
                                   DTL_GET_BLOCK_ARG_NUM2( aValueBlockList ),
                                   DTL_NATIVE_DOUBLE_DISPLAY_SIZE,
                                   sDoubleString,
                                   & sLength,
                                   ZLS_STMT_DT_VECTOR(aStmt),
                                   aStmt,
                                   aErrorStack )
             == STL_SUCCESS );

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = sLength * STL_SIZEOF(SQLWCHAR);
    }

    if( sBufferLength >= sLength )
    {
        for( i = 0; i < sLength; i++ )
        {
            ((SQLWCHAR*)aTargetValuePtr)[i] = sDoubleString[i];
        }
        
        ((SQLWCHAR*)aTargetValuePtr)[i] = 0;
    }
    else
    {
        for( i = 0; i < sBufferLength; i++ )
        {
            ((SQLWCHAR*)aTargetValuePtr)[i] = sDoubleString[i];
        }
        
        ((SQLWCHAR*)aTargetValuePtr)[i] = 0;
        
        STL_THROW( RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED );
    }

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                      "Returning the numeric value as numeric or string for one or "
                      "more bound columns would have caused the whole "
                      "(as opposed to fractional) part of the number to be truncated.",
                      aErrorStack );
    }
    
    STL_CATCH( RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED )
    {
        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_STRING_DATA_RIGHT_TRUNCATED,
                      "String or binary data returned for a column resulted "
                      "in the truncation of nonblank character or non-NULL binary data. "
                      "If it was a string value, it was right-truncated.",
                      aErrorStack );

        sRet     = STL_SUCCESS;
        *aReturn = SQL_SUCCESS_WITH_INFO;
    }

    STL_FINISH;
    
    return sRet;
}

/*
 * SQL_DOUBLE -> SQL_C_LONGVARCHAR
 */
ZLV_DECLARE_SQL_TO_C( Double, LongVarChar )
{
    /*
     * ========================================================================================================================
     * Test                                                              | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ========================================================================================================================
     * Character byte length < BufferLength                              | Data            | Length of data in bytes | n/a
     * Number of whole (as opposed to fractional) digits < BufferLength  | Truncated data  | Length of data in bytes | 01004
     * Number of whole (as opposed to fractional) digits >= BufferLength | Undefined       | Undefined               | 22003
     * ========================================================================================================================
     */

    SQL_LONG_VARIABLE_LENGTH_STRUCT * sTarget;
    stlChar                         * sValue;
    stlInt64                          sBufferLength;

    dtlDoubleType * sDataValue;
    stlInt64        sValidNumberLength;
    stlInt64        sLength;
    stlChar         sDoubleString[ DTL_NATIVE_DOUBLE_STRING_SIZE ];
    stlStatus       sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlDoubleType*)aDataValue->mValue;

    sTarget       = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aTargetValuePtr;
    sValue        = (stlChar*)sTarget->arr;
    sBufferLength = DTL_NATIVE_DOUBLE_DISPLAY_SIZE < ( sTarget->len -1 ) ?
        DTL_NATIVE_DOUBLE_DISPLAY_SIZE : ( sTarget->len -1 );

    STL_TRY_THROW( stlIsfinite( *sDataValue ) != STL_IS_FINITE_FALSE,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    STL_TRY( dtlGetValidNumberLengthFromFloat64( sDataValue,
                                                 & sValidNumberLength,
                                                 aErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( sValidNumberLength <= sBufferLength ,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
    
    STL_TRY( dtlToStringDataValue( aDataValue,
                                   DTL_GET_BLOCK_ARG_NUM1( aValueBlockList ),
                                   DTL_GET_BLOCK_ARG_NUM2( aValueBlockList ),
                                   DTL_NATIVE_DOUBLE_DISPLAY_SIZE,
                                   sDoubleString,
                                   & sLength,
                                   ZLS_STMT_DT_VECTOR(aStmt),
                                   aStmt,
                                   aErrorStack )
             == STL_SUCCESS );

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = sLength;
    }
    else
    {
        /* Do Nothing */
    }

    if( sBufferLength >= sLength )
    {
        stlMemcpy( sValue,
                   (const stlChar *)sDoubleString,
                   sLength );
        sValue[ sLength ] = '\0';
    }
    else
    {
        stlMemcpy( sValue,
                   (const stlChar *)sDoubleString,
                   sBufferLength );
        sValue[ sBufferLength ] = '\0';

        STL_THROW( RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED );
    }

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                      "Returning the numeric value as numeric or string for one or "
                      "more bound columns would have caused the whole "
                      "(as opposed to fractional) part of the number to be truncated.",
                      aErrorStack );
    }
    
    STL_CATCH( RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED )
    {
        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_STRING_DATA_RIGHT_TRUNCATED,
                      "String or binary data returned for a column resulted "
                      "in the truncation of nonblank character or non-NULL binary data. "
                      "If it was a string value, it was right-truncated.",
                      aErrorStack );

        sRet     = STL_SUCCESS;
        *aReturn = SQL_SUCCESS_WITH_INFO;
    }

    STL_FINISH;
    
    return sRet;
}

/*
 * SQL_DOUBLE -> SQL_C_STINYINT
 */
ZLV_DECLARE_SQL_TO_C( Double, STinyInt )
{
    /*
     * ==========================================================================================================
     * Test                                                | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ==========================================================================================================
     * Data converted without truncation                   | Data            | Size of the C data type | n/a
     * Data converted with truncation of fractional digits | Truncated data  | Size of the C data type | 01S07
     * Conversion of data would result in loss of whole    | Undefined       | Undefined               | 22003
     * (as opposed to fractional) digits                   |                 |                         |
     * ==========================================================================================================
     */

    dtlDoubleType * sDataValue;
    stlInt8       * sTarget;

    stlStatus       sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlDoubleType*)aDataValue->mValue;
    sTarget    = (stlInt8*)aTargetValuePtr;

    STL_TRY_THROW( stlIsfinite( *sDataValue ) != STL_IS_FINITE_FALSE,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    STL_TRY_THROW( ( *sDataValue >= STL_INT8_MIN ) &&
                   ( *sDataValue <= STL_INT8_MAX ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    *sTarget = (stlInt8)*sDataValue;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_INT8_SIZE;
    }

    STL_TRY_THROW( *sTarget >= *sDataValue, RAMP_WARNING_FRACTIONAL_TRUNCATION );

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                      "Returning the numeric value as numeric or string for one or "
                      "more bound columns would have caused the whole "
                      "(as opposed to fractional) part of the number to be truncated.",
                      aErrorStack );
    }
    
    STL_CATCH( RAMP_WARNING_FRACTIONAL_TRUNCATION )
    {
        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_FRACTIONAL_TRUNCATION,
                      "The data returned for a column was truncated. "
                      "For numeric data types, the fractional part of the number was truncated. "
                      "For time, timestamp, and interval data types that contain a time component, "
                      "the fractional part of the time was truncated.",
                      aErrorStack );

        sRet     = STL_SUCCESS;
        *aReturn = SQL_SUCCESS_WITH_INFO;
    }

    STL_FINISH;
    
    return sRet;
}

/*
 * SQL_DOUBLE -> SQL_C_UTINYINT
 */
ZLV_DECLARE_SQL_TO_C( Double, UTinyInt )
{
    /*
     * ==========================================================================================================
     * Test                                                | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ==========================================================================================================
     * Data converted without truncation                   | Data            | Size of the C data type | n/a
     * Data converted with truncation of fractional digits | Truncated data  | Size of the C data type | 01S07
     * Conversion of data would result in loss of whole    | Undefined       | Undefined               | 22003
     * (as opposed to fractional) digits                   |                 |                         |
     * ==========================================================================================================
     */

    dtlDoubleType * sDataValue;
    stlUInt8      * sTarget;

    stlStatus       sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlDoubleType*)aDataValue->mValue;
    sTarget    = (stlUInt8*)aTargetValuePtr;

    STL_TRY_THROW( stlIsfinite( *sDataValue ) != STL_IS_FINITE_FALSE,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    STL_TRY_THROW( ( *sDataValue >= STL_UINT8_MIN ) &&
                   ( *sDataValue <= STL_UINT8_MAX ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    *sTarget = (stlUInt8)*sDataValue;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_UINT8_SIZE;
    }

    STL_TRY_THROW( *sTarget >= *sDataValue, RAMP_WARNING_FRACTIONAL_TRUNCATION );

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                      "Returning the numeric value as numeric or string for one or "
                      "more bound columns would have caused the whole "
                      "(as opposed to fractional) part of the number to be truncated.",
                      aErrorStack );
    }
    
    STL_CATCH( RAMP_WARNING_FRACTIONAL_TRUNCATION )
    {
        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_FRACTIONAL_TRUNCATION,
                      "The data returned for a column was truncated. "
                      "For numeric data types, the fractional part of the number was truncated. "
                      "For time, timestamp, and interval data types that contain a time component, "
                      "the fractional part of the time was truncated.",
                      aErrorStack );

        sRet     = STL_SUCCESS;
        *aReturn = SQL_SUCCESS_WITH_INFO;
    }

    STL_FINISH;
    
    return sRet;
}

/*
 * SQL_DOUBLE -> SQL_C_SBIGINT
 */
ZLV_DECLARE_SQL_TO_C( Double, SBigInt )
{
    /*
     * ==========================================================================================================
     * Test                                                | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ==========================================================================================================
     * Data converted without truncation                   | Data            | Size of the C data type | n/a
     * Data converted with truncation of fractional digits | Truncated data  | Size of the C data type | 01S07
     * Conversion of data would result in loss of whole    | Undefined       | Undefined               | 22003
     * (as opposed to fractional) digits                   |                 |                         |
     * ==========================================================================================================
     */

    dtlDoubleType * sDataValue;
    stlInt64      * sTarget;

    stlStatus       sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlDoubleType*)aDataValue->mValue;
    sTarget    = (stlInt64*)aTargetValuePtr;

    STL_TRY_THROW( stlIsfinite( *sDataValue ) != STL_IS_FINITE_FALSE,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    STL_TRY_THROW( ( *sDataValue >= STL_INT64_MIN ) &&
                   ( *sDataValue <= STL_INT64_MAX ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    *sTarget = (stlInt64)*sDataValue;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_INT64_SIZE;
    }

    STL_TRY_THROW( *sTarget >= *sDataValue, RAMP_WARNING_FRACTIONAL_TRUNCATION );

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                      "Returning the numeric value as numeric or string for one or "
                      "more bound columns would have caused the whole "
                      "(as opposed to fractional) part of the number to be truncated.",
                      aErrorStack );
    }
    
    STL_CATCH( RAMP_WARNING_FRACTIONAL_TRUNCATION )
    {
        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_FRACTIONAL_TRUNCATION,
                      "The data returned for a column was truncated. "
                      "For numeric data types, the fractional part of the number was truncated. "
                      "For time, timestamp, and interval data types that contain a time component, "
                      "the fractional part of the time was truncated.",
                      aErrorStack );

        sRet     = STL_SUCCESS;
        *aReturn = SQL_SUCCESS_WITH_INFO;
    }

    STL_FINISH;
    
    return sRet;
}

/*
 * SQL_DOUBLE -> SQL_C_UBIGINT
 */
ZLV_DECLARE_SQL_TO_C( Double, UBigInt )
{
    /*
     * ==========================================================================================================
     * Test                                                | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ==========================================================================================================
     * Data converted without truncation                   | Data            | Size of the C data type | n/a
     * Data converted with truncation of fractional digits | Truncated data  | Size of the C data type | 01S07
     * Conversion of data would result in loss of whole    | Undefined       | Undefined               | 22003
     * (as opposed to fractional) digits                   |                 |                         |
     * ==========================================================================================================
     */

    dtlDoubleType * sDataValue;
    stlUInt64     * sTarget;

    stlStatus       sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlDoubleType*)aDataValue->mValue;
    sTarget    = (stlUInt64*)aTargetValuePtr;

    STL_TRY_THROW( stlIsfinite( *sDataValue ) != STL_IS_FINITE_FALSE,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    STL_TRY_THROW( ( *sDataValue >= STL_UINT64_MIN ) &&
                   ( *sDataValue <= STL_UINT64_MAX ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    *sTarget = (stlUInt64)*sDataValue;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_UINT64_SIZE;
    }

    STL_TRY_THROW( *sTarget >= *sDataValue, RAMP_WARNING_FRACTIONAL_TRUNCATION );

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                      "Returning the numeric value as numeric or string for one or "
                      "more bound columns would have caused the whole "
                      "(as opposed to fractional) part of the number to be truncated.",
                      aErrorStack );
    }
    
    STL_CATCH( RAMP_WARNING_FRACTIONAL_TRUNCATION )
    {
        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_FRACTIONAL_TRUNCATION,
                      "The data returned for a column was truncated. "
                      "For numeric data types, the fractional part of the number was truncated. "
                      "For time, timestamp, and interval data types that contain a time component, "
                      "the fractional part of the time was truncated.",
                      aErrorStack );

        sRet     = STL_SUCCESS;
        *aReturn = SQL_SUCCESS_WITH_INFO;
    }

    STL_FINISH;
    
    return sRet;
}

/*
 * SQL_DOUBLE -> SQL_C_SSHORT
 */
ZLV_DECLARE_SQL_TO_C( Double, SShort )
{
    /*
     * ==========================================================================================================
     * Test                                                | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ==========================================================================================================
     * Data converted without truncation                   | Data            | Size of the C data type | n/a
     * Data converted with truncation of fractional digits | Truncated data  | Size of the C data type | 01S07
     * Conversion of data would result in loss of whole    | Undefined       | Undefined               | 22003
     * (as opposed to fractional) digits                   |                 |                         |
     * ==========================================================================================================
     */

    dtlDoubleType * sDataValue;
    stlInt16      * sTarget;

    stlStatus       sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlDoubleType*)aDataValue->mValue;
    sTarget    = (stlInt16*)aTargetValuePtr;

    STL_TRY_THROW( stlIsfinite( *sDataValue ) != STL_IS_FINITE_FALSE,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    STL_TRY_THROW( ( *sDataValue >= STL_INT16_MIN ) &&
                   ( *sDataValue <= STL_INT16_MAX ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    *sTarget = (stlInt16)*sDataValue;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_INT16_SIZE;
    }

    STL_TRY_THROW( *sTarget >= *sDataValue, RAMP_WARNING_FRACTIONAL_TRUNCATION );

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                      "Returning the numeric value as numeric or string for one or "
                      "more bound columns would have caused the whole "
                      "(as opposed to fractional) part of the number to be truncated.",
                      aErrorStack );
    }
    
    STL_CATCH( RAMP_WARNING_FRACTIONAL_TRUNCATION )
    {
        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_FRACTIONAL_TRUNCATION,
                      "The data returned for a column was truncated. "
                      "For numeric data types, the fractional part of the number was truncated. "
                      "For time, timestamp, and interval data types that contain a time component, "
                      "the fractional part of the time was truncated.",
                      aErrorStack );

        sRet     = STL_SUCCESS;
        *aReturn = SQL_SUCCESS_WITH_INFO;
    }

    STL_FINISH;
    
    return sRet;
}

/*
 * SQL_DOUBLE -> SQL_C_USHORT
 */
ZLV_DECLARE_SQL_TO_C( Double, UShort )
{
    /*
     * ==========================================================================================================
     * Test                                                | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ==========================================================================================================
     * Data converted without truncation                   | Data            | Size of the C data type | n/a
     * Data converted with truncation of fractional digits | Truncated data  | Size of the C data type | 01S07
     * Conversion of data would result in loss of whole    | Undefined       | Undefined               | 22003
     * (as opposed to fractional) digits                   |                 |                         |
     * ==========================================================================================================
     */

    dtlDoubleType * sDataValue;
    stlUInt16     * sTarget;

    stlStatus       sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlDoubleType*)aDataValue->mValue;
    sTarget    = (stlUInt16*)aTargetValuePtr;

    STL_TRY_THROW( stlIsfinite( *sDataValue ) != STL_IS_FINITE_FALSE,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    STL_TRY_THROW( ( *sDataValue >= STL_UINT16_MIN ) &&
                   ( *sDataValue <= STL_UINT16_MAX ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    *sTarget = (stlUInt16)*sDataValue;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_UINT16_SIZE;
    }

    STL_TRY_THROW( *sTarget >= *sDataValue, RAMP_WARNING_FRACTIONAL_TRUNCATION );

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                      "Returning the numeric value as numeric or string for one or "
                      "more bound columns would have caused the whole "
                      "(as opposed to fractional) part of the number to be truncated.",
                      aErrorStack );
    }
    
    STL_CATCH( RAMP_WARNING_FRACTIONAL_TRUNCATION )
    {
        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_FRACTIONAL_TRUNCATION,
                      "The data returned for a column was truncated. "
                      "For numeric data types, the fractional part of the number was truncated. "
                      "For time, timestamp, and interval data types that contain a time component, "
                      "the fractional part of the time was truncated.",
                      aErrorStack );

        sRet     = STL_SUCCESS;
        *aReturn = SQL_SUCCESS_WITH_INFO;
    }

    STL_FINISH;
    
    return sRet;
}

/*
 * SQL_DOUBLE -> SQL_C_SLONG
 */
ZLV_DECLARE_SQL_TO_C( Double, SLong )
{
    /*
     * ==========================================================================================================
     * Test                                                | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ==========================================================================================================
     * Data converted without truncation                   | Data            | Size of the C data type | n/a
     * Data converted with truncation of fractional digits | Truncated data  | Size of the C data type | 01S07
     * Conversion of data would result in loss of whole    | Undefined       | Undefined               | 22003
     * (as opposed to fractional) digits                   |                 |                         |
     * ==========================================================================================================
     */

    dtlDoubleType * sDataValue;
    stlInt32      * sTarget;

    stlStatus       sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlDoubleType*)aDataValue->mValue;
    sTarget    = (stlInt32*)aTargetValuePtr;

    STL_TRY_THROW( stlIsfinite( *sDataValue ) != STL_IS_FINITE_FALSE,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    STL_TRY_THROW( ( *sDataValue >= STL_INT32_MIN ) &&
                   ( *sDataValue <= STL_INT32_MAX ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    *sTarget = (stlInt32)*sDataValue;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_INT32_SIZE;
    }

    STL_TRY_THROW( *sTarget >= *sDataValue, RAMP_WARNING_FRACTIONAL_TRUNCATION );

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                      "Returning the numeric value as numeric or string for one or "
                      "more bound columns would have caused the whole "
                      "(as opposed to fractional) part of the number to be truncated.",
                      aErrorStack );
    }
    
    STL_CATCH( RAMP_WARNING_FRACTIONAL_TRUNCATION )
    {
        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_FRACTIONAL_TRUNCATION,
                      "The data returned for a column was truncated. "
                      "For numeric data types, the fractional part of the number was truncated. "
                      "For time, timestamp, and interval data types that contain a time component, "
                      "the fractional part of the time was truncated.",
                      aErrorStack );

        sRet     = STL_SUCCESS;
        *aReturn = SQL_SUCCESS_WITH_INFO;
    }

    STL_FINISH;
    
    return sRet;
}

/*
 * SQL_DOUBLE -> SQL_C_ULONG
 */
ZLV_DECLARE_SQL_TO_C( Double, ULong )
{
    /*
     * ==========================================================================================================
     * Test                                                | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ==========================================================================================================
     * Data converted without truncation                   | Data            | Size of the C data type | n/a
     * Data converted with truncation of fractional digits | Truncated data  | Size of the C data type | 01S07
     * Conversion of data would result in loss of whole    | Undefined       | Undefined               | 22003
     * (as opposed to fractional) digits                   |                 |                         |
     * ==========================================================================================================
     */

    dtlDoubleType * sDataValue;
    stlUInt32     * sTarget;

    stlStatus       sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlDoubleType*)aDataValue->mValue;
    sTarget    = (stlUInt32*)aTargetValuePtr;

    STL_TRY_THROW( stlIsfinite( *sDataValue ) != STL_IS_FINITE_FALSE,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    STL_TRY_THROW( ( *sDataValue >= STL_UINT32_MIN ) &&
                   ( *sDataValue <= STL_UINT32_MAX ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    *sTarget = (stlUInt32)*sDataValue;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_UINT32_SIZE;
    }

    STL_TRY_THROW( *sTarget >= *sDataValue, RAMP_WARNING_FRACTIONAL_TRUNCATION );

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                      "Returning the numeric value as numeric or string for one or "
                      "more bound columns would have caused the whole "
                      "(as opposed to fractional) part of the number to be truncated.",
                      aErrorStack );
    }
    
    STL_CATCH( RAMP_WARNING_FRACTIONAL_TRUNCATION )
    {
        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_FRACTIONAL_TRUNCATION,
                      "The data returned for a column was truncated. "
                      "For numeric data types, the fractional part of the number was truncated. "
                      "For time, timestamp, and interval data types that contain a time component, "
                      "the fractional part of the time was truncated.",
                      aErrorStack );

        sRet     = STL_SUCCESS;
        *aReturn = SQL_SUCCESS_WITH_INFO;
    }

    STL_FINISH;
    
    return sRet;
}

/*
 * SQL_DOUBLE -> SQL_C_NUMERIC
 */
ZLV_DECLARE_SQL_TO_C( Double, Numeric )
{
    /*
     * ==========================================================================================================
     * Test                                                | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ==========================================================================================================
     * Data converted without truncation                   | Data            | Size of the C data type | n/a
     * Data converted with truncation of fractional digits | Truncated data  | Size of the C data type | 01S07
     * Conversion of data would result in loss of whole    | Undefined       | Undefined               | 22003
     * (as opposed to fractional) digits                   |                 |                         |
     * ==========================================================================================================
     */

    dtlDoubleType      * sDataValue;
    SQL_NUMERIC_STRUCT * sTarget;
    stlStatus            sRet = STL_FAILURE;
    stlBool              sIsTruncated = STL_FALSE;
    stlChar              sFloatString[ DTL_FLOAT64_STRING_SIZE + 1 ];
    stlInt32             sLength = 0;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlDoubleType*)aDataValue->mValue;
    sTarget    = (SQL_NUMERIC_STRUCT *)aTargetValuePtr;

    STL_TRY_THROW( stlIsfinite( *sDataValue ) != STL_IS_FINITE_FALSE,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    STL_TRY_THROW( ((ZLV_SQL_TO_C_NUMERIC_MAX_SCALE >= aArdRec->mScale) &&
                    (ZLV_SQL_TO_C_NUMERIC_MIN_SCALE <= aArdRec->mScale)),
                   RAMP_ERR_SCALE_VALUE_OUT_OF_RANGE );

    if( *(double*)aDataValue == 0.0 )
    {
        sTarget->precision = ZLV_DOUBLE_TO_C_NUMERIC_PRECISION;
        sTarget->scale     = 0;
        stlMemset( sTarget->val , 0x00 , SQL_MAX_NUMERIC_LEN );
    }
    else
    {

        sLength = stlSnprintf( sFloatString, DTL_FLOAT64_STRING_SIZE, "%1.*E",
                               (STL_DBL_DIG -1),
                               *sDataValue );

        STL_TRY( zlvMakeCNumericFromString( aStmt,
                                            sTarget,
                                            sFloatString,
                                            sLength,
                                            ZLV_DOUBLE_TO_C_NUMERIC_PRECISION,
                                            aArdRec->mScale,
                                            & sIsTruncated,
                                            aErrorStack )
                 == STL_SUCCESS );
    }

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_SIZEOF( SQL_NUMERIC_STRUCT );
    }

    if( sIsTruncated == STL_TRUE )
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_SUCCESS_WITH_INFO;
    }
    else
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_SUCCESS;
    }

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_ERR_SCALE_VALUE_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                      "Specified scale value is out of range( -128 to 127 )",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                      "Returning the numeric value as numeric or string for one or "
                      "more bound columns would have caused the whole "
                      "(as opposed to fractional) part of the number to be truncated.",
                      aErrorStack );
    }
    
    STL_FINISH;
    
    return sRet;
}

/*
 * SQL_DOUBLE -> SQL_C_FLOAT
 */
ZLV_DECLARE_SQL_TO_C( Double, Float )
{
    /*
     * ====================================================================================================
     * Test                                          | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ====================================================================================================
     * Data is within the range of the data type to  | Data            | Size of the C data type | n/a
     * which the number is being converted           |                 |                         |
     * Data is outside the range of the data type to | Undefined       | Undefined               | 22003
     * which the number is being converted           |                 |                         |
     * ====================================================================================================
     */

    dtlDoubleType * sDataValue;
    stlFloat32    * sTarget;
    stlStatus       sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlDoubleType*)aDataValue->mValue;
    sTarget    = (stlFloat32*)aTargetValuePtr;

    STL_TRY_THROW( DTL_CHECK_FLOAT_VALUE( (stlFloat32)*sDataValue,
                                          stlIsinfinite( *sDataValue ) ) == STL_TRUE,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
    
    *sTarget = (stlFloat32)*sDataValue;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_FLOAT32_SIZE;
    }

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                      "Returning the numeric value as numeric or string for one or "
                      "more bound columns would have caused the whole "
                      "(as opposed to fractional) part of the number to be truncated.",
                      aErrorStack );
    }

    STL_FINISH;

    return sRet;
}

/*
 * SQL_DOUBLE -> SQL_C_DOUBLE
 */
ZLV_DECLARE_SQL_TO_C( Double, Double )
{
    /*
     * ====================================================================================================
     * Test                                          | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ====================================================================================================
     * Data is within the range of the data type to  | Data            | Size of the C data type | n/a
     * which the number is being converted           |                 |                         |
     * Data is outside the range of the data type to | Undefined       | Undefined               | 22003
     * which the number is being converted           |                 |                         |
     * ====================================================================================================
     */

    dtlDoubleType * sDataValue;
    stlFloat64    * sTarget;
    stlStatus       sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlDoubleType*)aDataValue->mValue;
    sTarget    = (stlFloat64*)aTargetValuePtr;

    *sTarget = (stlFloat64)*sDataValue;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_FLOAT64_SIZE;
    }

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_FINISH;
    
    return sRet;
}

/*
 * SQL_DOUBLE -> SQL_C_BIT
 */
ZLV_DECLARE_SQL_TO_C( Double, Bit )
{
    /*
     * ========================================================================================================
     * Test                                                    | *TargetValuePtr | *StrLen_or_IndPtr | SQLSTATE
     * ========================================================================================================
     * Data is 0 or 1                                          | Data            | 1                 | n/a
     * Data is greater than 0, less than 2, and not equal to 1 | Truncated data  | 1                 | 01S07
     * Data is less than 0 or greater than or equal to 2       | Undefined       | Undefined         | 22003
     * ========================================================================================================
     */

    dtlDoubleType * sDataValue;
    stlUInt8      * sTarget;

    stlStatus       sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlDoubleType*)aDataValue->mValue;
    sTarget    = (stlUInt8*)aTargetValuePtr;

    STL_TRY_THROW( stlIsfinite( *sDataValue ) != STL_IS_FINITE_FALSE,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    STL_TRY_THROW( ( *sDataValue >= 0 ) &&
                   ( *sDataValue < 2 ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    *sTarget = (stlUInt8)*sDataValue;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = 1;
    }

    STL_TRY_THROW( *sTarget >= *sDataValue, RAMP_WARNING_FRACTIONAL_TRUNCATION );

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                      "Returning the numeric value as numeric or string for one or "
                      "more bound columns would have caused the whole "
                      "(as opposed to fractional) part of the number to be truncated.",
                      aErrorStack );
    }
    
    STL_CATCH( RAMP_WARNING_FRACTIONAL_TRUNCATION )
    {
        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_FRACTIONAL_TRUNCATION,
                      "The data returned for a column was truncated. "
                      "For numeric data types, the fractional part of the number was truncated. "
                      "For time, timestamp, and interval data types that contain a time component, "
                      "the fractional part of the time was truncated.",
                      aErrorStack );

        sRet     = STL_SUCCESS;
        *aReturn = SQL_SUCCESS_WITH_INFO;
    }

    STL_FINISH;
    
    return sRet;
}

/*
 * SQL_DOUBLE -> SQL_C_BINARY
 */
ZLV_DECLARE_SQL_TO_C( Double, Binary )
{
    /*
     * ====================================================================================
     * Test                                | *TargetValuePtr | *StrLen_or_IndPtr | SQLSTATE
     * ====================================================================================
     * Byte length of data <= BufferLength | Data            | Length of data    | n/a
     * Byte length of data > BufferLength  | Undefined       | Undefined         | 22003
     * ====================================================================================
     */

    stlStatus sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    STL_TRY_THROW( STL_FLOAT64_SIZE <= aArdRec->mOctetLength,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    stlMemcpy( aTargetValuePtr,
               (const void*)aDataValue->mValue,
               STL_FLOAT64_SIZE );

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_FLOAT64_SIZE;
    }

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                      "Returning the numeric value as numeric or string for one or "
                      "more bound columns would have caused the whole "
                      "(as opposed to fractional) part of the number to be truncated.",
                      aErrorStack );
    }
    
    STL_FINISH;
    
    return sRet;
}

/*
 * SQL_DOUBLE -> SQL_C_LONGVARBINARY
 */
ZLV_DECLARE_SQL_TO_C( Double, LongVarBinary )
{
    /*
     * ====================================================================================
     * Test                                | *TargetValuePtr | *StrLen_or_IndPtr | SQLSTATE
     * ====================================================================================
     * Byte length of data <= BufferLength | Data            | Length of data    | n/a
     * Byte length of data > BufferLength  | Undefined       | Undefined         | 22003
     * ====================================================================================
     */

    SQL_LONG_VARIABLE_LENGTH_STRUCT * sTarget;
    stlChar                         * sValue;
    stlInt64                          sBufferLength;

    stlStatus sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget       = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aTargetValuePtr;
    sValue        = (stlChar*)sTarget->arr;
    sBufferLength = sTarget->len;

    STL_TRY_THROW( STL_FLOAT64_SIZE <= sBufferLength,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    stlMemcpy( sValue,
               (const void*)aDataValue->mValue,
               STL_FLOAT64_SIZE );

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_FLOAT64_SIZE;
    }

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                      "Returning the numeric value as numeric or string for one or "
                      "more bound columns would have caused the whole "
                      "(as opposed to fractional) part of the number to be truncated.",
                      aErrorStack );
    }
    
    STL_FINISH;
    
    return sRet;
}

/*
 * SQL_DOUBLE -> SQL_C_INTERVAL_YEAR_TO_MONTH
 */
ZLV_DECLARE_SQL_TO_C( Double, IntervalYearToMonth )
{
    /*
     * ===============================================================================
     * Test                           | *TargetValuePtr | *StrLen_or_IndPtr | SQLSTATE
     * ===============================================================================
     * Whole part of number truncated | Undefined       | Undefined         | 22015
     * ===============================================================================
     */

    stlPushError( STL_ERROR_LEVEL_ABORT,
                  ZLE_ERRCODE_INTERVAL_FIELD_OVERFLOW,
                  NULL,
                  aErrorStack );

    return STL_FAILURE;
}

/*
 * SQL_DOUBLE -> SQL_C_INTERVAL_DAY_TO_HOUR
 */
ZLV_DECLARE_SQL_TO_C( Double, IntervalDayToHour )
{
    /*
     * ===============================================================================
     * Test                           | *TargetValuePtr | *StrLen_or_IndPtr | SQLSTATE
     * ===============================================================================
     * Whole part of number truncated | Undefined       | Undefined         | 22015
     * ===============================================================================
     */

    stlPushError( STL_ERROR_LEVEL_ABORT,
                  ZLE_ERRCODE_INTERVAL_FIELD_OVERFLOW,
                  NULL,
                  aErrorStack );

    return STL_FAILURE;
}

/*
 * SQL_DOUBLE -> SQL_C_INTERVAL_DAY_TO_MINUTE
 */
ZLV_DECLARE_SQL_TO_C( Double, IntervalDayToMinute )
{
    /*
     * ===============================================================================
     * Test                           | *TargetValuePtr | *StrLen_or_IndPtr | SQLSTATE
     * ===============================================================================
     * Whole part of number truncated | Undefined       | Undefined         | 22015
     * ===============================================================================
     */

    stlPushError( STL_ERROR_LEVEL_ABORT,
                  ZLE_ERRCODE_INTERVAL_FIELD_OVERFLOW,
                  NULL,
                  aErrorStack );

    return STL_FAILURE;
}

/*
 * SQL_DOUBLE -> SQL_C_INTERVAL_DAY_TO_SECOND
 */
ZLV_DECLARE_SQL_TO_C( Double, IntervalDayToSecond )
{
    /*
     * ===============================================================================
     * Test                           | *TargetValuePtr | *StrLen_or_IndPtr | SQLSTATE
     * ===============================================================================
     * Whole part of number truncated | Undefined       | Undefined         | 22015
     * ===============================================================================
     */

    stlPushError( STL_ERROR_LEVEL_ABORT,
                  ZLE_ERRCODE_INTERVAL_FIELD_OVERFLOW,
                  NULL,
                  aErrorStack );

    return STL_FAILURE;
}

/*
 * SQL_DOUBLE -> SQL_C_INTERVAL_HOUR_TO_MINUTE
 */
ZLV_DECLARE_SQL_TO_C( Double, IntervalHourToMinute )
{
    /*
     * ===============================================================================
     * Test                           | *TargetValuePtr | *StrLen_or_IndPtr | SQLSTATE
     * ===============================================================================
     * Whole part of number truncated | Undefined       | Undefined         | 22015
     * ===============================================================================
     */

    stlPushError( STL_ERROR_LEVEL_ABORT,
                  ZLE_ERRCODE_INTERVAL_FIELD_OVERFLOW,
                  NULL,
                  aErrorStack );

    return STL_FAILURE;
}

/*
 * SQL_DOUBLE -> SQL_C_INTERVAL_HOUR_TO_SECOND
 */
ZLV_DECLARE_SQL_TO_C( Double, IntervalHourToSecond )
{
    /*
     * ===============================================================================
     * Test                           | *TargetValuePtr | *StrLen_or_IndPtr | SQLSTATE
     * ===============================================================================
     * Whole part of number truncated | Undefined       | Undefined         | 22015
     * ===============================================================================
     */

    stlPushError( STL_ERROR_LEVEL_ABORT,
                  ZLE_ERRCODE_INTERVAL_FIELD_OVERFLOW,
                  NULL,
                  aErrorStack );

    return STL_FAILURE;
}

/*
 * SQL_DOUBLE -> SQL_C_INTERVAL_MINUTE_TO_SECOND
 */
ZLV_DECLARE_SQL_TO_C( Double, IntervalMinuteToSecond )
{
    /*
     * ===============================================================================
     * Test                           | *TargetValuePtr | *StrLen_or_IndPtr | SQLSTATE
     * ===============================================================================
     * Whole part of number truncated | Undefined       | Undefined         | 22015
     * ===============================================================================
     */

    stlPushError( STL_ERROR_LEVEL_ABORT,
                  ZLE_ERRCODE_INTERVAL_FIELD_OVERFLOW,
                  NULL,
                  aErrorStack );

    return STL_FAILURE;
}

/** @} */
