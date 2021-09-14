/*******************************************************************************
 * zlvNumericToC.c
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
 * @brief Gliese API Internal Converting Data from SQL Numeric to C Data Types Routines.
 */

/**
 * @addtogroup zlvNumericToC
 * @{
 */

/*
 * http://msdn.microsoft.com/en-us/library/windows/desktop/ms712567%28v=VS.85%29.aspx
 */

/*
 * SQL_NUMERIC -> SQL_C_CHAR
 */
ZLV_DECLARE_SQL_TO_C( Numeric, Char )
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

    stlInt64         sLength     = 0;
    stlInt64         sBufferLength = 0;
    stlInt64         sNumericStrLength = 0;
    stlChar          sNumericStr[DTL_NUMERIC_STRING_MAX_SIZE + 1];
    stlInt64         sValidNumberLength = 0;
    stlInt64         sEntireNumberLength = 0;
    stlInt64         sExpNumberLength = 0;
    stlStatus        sRet = STL_FAILURE;
     
    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    /* Buffer 의 최대길이는 DISPLAYSIZE 를 넘지 않음 */
    sBufferLength = DTL_NUMERIC_STRING_MAX_SIZE < aArdRec->mOctetLength -1 ?
        DTL_NUMERIC_STRING_MAX_SIZE : aArdRec->mOctetLength -1;

    /*
     * dtlNumericType value를 string으로 변환
     */

    /* data 의 유효자리 확인 */
    STL_TRY( dtlGetStringLengthFromNumeric( aDataValue,
                                            & sValidNumberLength,
                                            & sEntireNumberLength,
                                            & sExpNumberLength,
                                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( sBufferLength >= sValidNumberLength,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    sNumericStrLength = sBufferLength >= sEntireNumberLength ?
        sBufferLength : ( sEntireNumberLength > sExpNumberLength ?
                          sExpNumberLength : sEntireNumberLength );

    STL_TRY( dtlToStringDataValue( aDataValue,
                                   DTL_GET_BLOCK_ARG_NUM1( aValueBlockList ),
                                   DTL_GET_BLOCK_ARG_NUM2( aValueBlockList ),
                                   sNumericStrLength,
                                   sNumericStr,
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

    /* Buffer 가 충분한 경우 */
    if( sBufferLength >= sLength )
    {
        stlMemcpy( (stlChar*)aTargetValuePtr,
                   (const stlChar*)sNumericStr,
                   sLength );
        ((stlChar*)aTargetValuePtr)[ sLength ] = '\0';   
    }
    /* Buffer 가 부족한 경우 ( string right truncated ) */
    else
    {
        stlMemcpy( (stlChar*)aTargetValuePtr,
                   (const stlChar*)sNumericStr,
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
 * SQL_NUMERIC -> SQL_C_WCHAR
 */
ZLV_DECLARE_SQL_TO_C( Numeric, Wchar )
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

    stlInt64  sLength     = 0;
    stlInt64  sBufferLength = 0;
    stlInt64  sNumericStrLength = 0;
    stlChar   sNumericStr[DTL_NUMERIC_STRING_MAX_SIZE + 1];
    stlInt64  sValidNumberLength = 0;
    stlInt64  sEntireNumberLength = 0;
    stlInt64  sExpNumberLength = 0;
    stlStatus sRet = STL_FAILURE;
    stlInt64  sAvailableSize = 0;
    stlInt64  i;
     
    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sAvailableSize = aArdRec->mOctetLength - STL_SIZEOF(SQLWCHAR);

    /* Buffer 의 최대길이는 DISPLAYSIZE 를 넘지 않음 */
    sBufferLength = DTL_NUMERIC_STRING_MAX_SIZE < (sAvailableSize / STL_SIZEOF(SQLWCHAR)) ?
        DTL_NUMERIC_STRING_MAX_SIZE : (sAvailableSize / STL_SIZEOF(SQLWCHAR));

    /*
     * dtlNumericType value를 string으로 변환
     */

    /* data 의 유효자리 확인 */
    STL_TRY( dtlGetStringLengthFromNumeric( aDataValue,
                                            & sValidNumberLength,
                                            & sEntireNumberLength,
                                            & sExpNumberLength,
                                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( sBufferLength >= sValidNumberLength,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    sNumericStrLength = sBufferLength >= sEntireNumberLength ?
        sBufferLength : ( sEntireNumberLength > sExpNumberLength ?
                          sExpNumberLength : sEntireNumberLength );

    STL_TRY( dtlToStringDataValue( aDataValue,
                                   DTL_GET_BLOCK_ARG_NUM1( aValueBlockList ),
                                   DTL_GET_BLOCK_ARG_NUM2( aValueBlockList ),
                                   sNumericStrLength,
                                   sNumericStr,
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

    /* Buffer 가 충분한 경우 */
    if( sBufferLength >= sLength )
    {
        for( i = 0; i < sLength; i++ )
        {
            ((SQLWCHAR*)aTargetValuePtr)[i] = sNumericStr[i];
        }

        ((SQLWCHAR*)aTargetValuePtr)[i] = 0;
    }
    /* Buffer 가 부족한 경우 ( string right truncated ) */
    else
    {
        for( i = 0; i < sBufferLength; i++ )
        {
            ((SQLWCHAR*)aTargetValuePtr)[i] = sNumericStr[i];
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
 * SQL_NUMERIC -> SQL_C_LONGVARCHAR
 */
ZLV_DECLARE_SQL_TO_C( Numeric, LongVarChar )
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

    stlInt64         sLength     = 0;
    stlInt64         sNumericStrLength = 0;
    stlChar          sNumericStr[DTL_NUMERIC_STRING_MAX_SIZE];
    stlInt64         sValidNumberLength = 0;
    stlInt64         sEntireNumberLength = 0;
    stlInt64         sExpNumberLength = 0;
    stlStatus        sRet = STL_FAILURE;
    
    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget       = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aTargetValuePtr;
    sValue        = (stlChar*)sTarget->arr;
    sBufferLength = DTL_NUMERIC_STRING_MAX_SIZE < ( sTarget->len -1 ) ?
        DTL_NUMERIC_STRING_MAX_SIZE : ( sTarget->len -1 );

    /*
     * dtlNumericType value를 string으로 변환
     */

    /* data 의 유효자리 확인 */
    STL_TRY( dtlGetStringLengthFromNumeric( aDataValue,
                                            & sValidNumberLength,
                                            & sEntireNumberLength,
                                            & sExpNumberLength,
                                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( sBufferLength >= sValidNumberLength,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
       
    sNumericStrLength = sBufferLength >= sEntireNumberLength ?
        sBufferLength : ( sEntireNumberLength > sExpNumberLength ?
                          sExpNumberLength : sEntireNumberLength );

    STL_TRY( dtlToStringDataValue( aDataValue,
                                   DTL_GET_BLOCK_ARG_NUM1( aValueBlockList ),
                                   DTL_GET_BLOCK_ARG_NUM2( aValueBlockList ),
                                   sNumericStrLength,
                                   sNumericStr,
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
        /* Buffer 가 충분한 경우 */
        stlMemcpy( sValue,
                   (const stlChar*)sNumericStr,
                   sLength );
        sValue[ sLength ] = '\0';   
    }
    else
    {
        /* Buffer 가 부족한 경우 ( string right truncated ) */
        stlMemcpy( sValue,
                   (const stlChar*)sNumericStr,
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
 * SQL_NUMERIC -> SQL_C_STINYINT
 */
ZLV_DECLARE_SQL_TO_C( Numeric, STinyInt )
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

    stlInt8        * sTarget;
    
    stlInt64         sInt64Value;
    stlBool          sIsTruncate = STL_FALSE;
    
    stlStatus        sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget    = (stlInt8*)aTargetValuePtr;    

    /*
     * dtlNumericType value를 stlInt64 value로 변환한다.
     */
    STL_TRY( dtlNumericToInt64( aDataValue,
                                & sInt64Value,
                                & sIsTruncate,
                                aErrorStack )
             == STL_SUCCESS );

    /*
     * SQLSTATE 상태 확인 
     */
    
    STL_TRY_THROW( (sInt64Value >= STL_INT8_MIN) &&
                   (sInt64Value <= STL_INT8_MAX),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    *sTarget = (stlInt8)(sInt64Value);

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_INT8_SIZE;
    }

    STL_TRY_THROW( sIsTruncate == STL_FALSE, RAMP_WARNING_FRACTIONAL_TRUNCATION );

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
 * SQL_NUMERIC -> SQL_C_UTINYINT
 */
ZLV_DECLARE_SQL_TO_C( Numeric, UTinyInt )
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

    stlUInt8       * sTarget;

    stlInt64         sInt64Value;
    stlBool          sIsTruncate = STL_FALSE;
    
    stlStatus        sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget    = (stlUInt8*)aTargetValuePtr;    
    
    /*
     * dtlNumericType value를 stlInt64 value로 변환한다.
     */
    STL_TRY( dtlNumericToInt64( aDataValue,
                                & sInt64Value,
                                & sIsTruncate,
                                aErrorStack )
             == STL_SUCCESS );

    /*
     * SQLSTATE 상태 확인 
     */
    
    STL_TRY_THROW( (sInt64Value >= STL_UINT8_MIN) &&
                   (sInt64Value <= STL_UINT8_MAX),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    *sTarget = (stlUInt8)(sInt64Value);

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_UINT8_SIZE;
    }

    STL_TRY_THROW( sIsTruncate == STL_FALSE, RAMP_WARNING_FRACTIONAL_TRUNCATION );

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
 * SQL_NUMERIC -> SQL_C_SBIGINT
 */
ZLV_DECLARE_SQL_TO_C( Numeric, SBigInt )
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

    stlInt64       * sTarget;

    stlInt64         sInt64Value;
    stlBool          sIsTruncate = STL_FALSE;
    
    stlStatus        sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget    = (stlInt64*)aTargetValuePtr;    
    
    /*
     * dtlNumericType value를 stlInt64 value로 변환한다.
     */
    STL_TRY( dtlNumericToInt64( aDataValue,
                                & sInt64Value,
                                & sIsTruncate,
                                aErrorStack )
             == STL_SUCCESS );
    
    /*
     * SQLSTATE 상태 확인 
     */
    
    *sTarget = sInt64Value;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_INT64_SIZE;
    }

    STL_TRY_THROW( sIsTruncate == STL_FALSE, RAMP_WARNING_FRACTIONAL_TRUNCATION );

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
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
 * SQL_NUMERIC -> SQL_C_UBIGINT
 */
ZLV_DECLARE_SQL_TO_C( Numeric, UBigInt )
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

    stlUInt64      * sTarget;

    stlUInt64        sUInt64Value;
    stlBool          sIsTruncate = STL_FALSE;
    
    stlStatus        sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget    = (stlUInt64*)aTargetValuePtr;    
    
    /*
     * dtlNumericType value를 stlUInt64 value로 변환한다.
     */
    STL_TRY( dtlNumericToUInt64( aDataValue,
                                 & sUInt64Value,
                                 & sIsTruncate,
                                 aErrorStack )
             == STL_SUCCESS );
    
    /*
     * SQLSTATE 상태 확인 
     */
    
    *sTarget = sUInt64Value;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_UINT64_SIZE;
    }

    STL_TRY_THROW( sIsTruncate == STL_FALSE, RAMP_WARNING_FRACTIONAL_TRUNCATION );

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;
    
    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
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
 * SQL_NUMERIC -> SQL_C_SSHORT
 */
ZLV_DECLARE_SQL_TO_C( Numeric, SShort )
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

    stlInt16       * sTarget;

    stlInt64         sInt64Value;
    stlBool          sIsTruncate = STL_FALSE;
    
    stlStatus        sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget    = (stlInt16*)aTargetValuePtr;    
    
    /*
     * dtlNumericType value를 stlInt64 value로 변환한다.
     */
    STL_TRY( dtlNumericToInt64( aDataValue,
                                & sInt64Value,
                                & sIsTruncate,
                                aErrorStack )
             == STL_SUCCESS );

    /*
     * SQLSTATE 상태 확인 
     */
    
    STL_TRY_THROW( (sInt64Value >= STL_INT16_MIN) &&
                   (sInt64Value <= STL_INT16_MAX),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    *sTarget = (stlInt16)(sInt64Value);

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_INT16_SIZE;
    }

    STL_TRY_THROW( sIsTruncate == STL_FALSE, RAMP_WARNING_FRACTIONAL_TRUNCATION );

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
 * SQL_NUMERIC -> SQL_C_USHORT
 */
ZLV_DECLARE_SQL_TO_C( Numeric, UShort )
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

    stlUInt16      * sTarget;

    stlInt64         sInt64Value;
    stlBool          sIsTruncate = STL_FALSE;
    
    stlStatus        sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget    = (stlUInt16*)aTargetValuePtr;    
    
    /*
     * dtlNumericType value를 stlInt64 value로 변환한다.
     */
    STL_TRY( dtlNumericToInt64( aDataValue,
                                & sInt64Value,
                                & sIsTruncate,
                                aErrorStack )
             == STL_SUCCESS );

    /*
     * SQLSTATE 상태 확인 
     */
    
    STL_TRY_THROW( (sInt64Value >= STL_UINT16_MIN) &&
                   (sInt64Value <= STL_UINT16_MAX),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    *sTarget = (stlUInt16)(sInt64Value);

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_UINT16_SIZE;
    }

    STL_TRY_THROW( sIsTruncate == STL_FALSE, RAMP_WARNING_FRACTIONAL_TRUNCATION );

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
 * SQL_NUMERIC -> SQL_C_SLONG
 */
ZLV_DECLARE_SQL_TO_C( Numeric, SLong )
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

    stlInt32       * sTarget;

    stlInt64         sInt64Value;
    stlBool          sIsTruncate = STL_FALSE;
    
    stlStatus        sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget    = (stlInt32*)aTargetValuePtr;    
    
    /*
     * dtlNumericType value를 stlInt64 value로 변환한다.
     */
    STL_TRY( dtlNumericToInt64( aDataValue,
                                & sInt64Value,
                                & sIsTruncate,
                                aErrorStack )
             == STL_SUCCESS );

    /*
     * SQLSTATE 상태 확인 
     */
    
    STL_TRY_THROW( (sInt64Value >= STL_INT32_MIN) &&
                   (sInt64Value <= STL_INT32_MAX),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    *sTarget = (stlInt32)(sInt64Value);

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_INT32_SIZE;
    }

    STL_TRY_THROW( sIsTruncate == STL_FALSE, RAMP_WARNING_FRACTIONAL_TRUNCATION );

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
 * SQL_NUMERIC -> SQL_C_ULONG
 */
ZLV_DECLARE_SQL_TO_C( Numeric, ULong )
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

    stlUInt32      * sTarget;

    stlInt64         sInt64Value;
    stlBool          sIsTruncate = STL_FALSE;
    
    stlStatus        sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget    = (stlUInt32*)aTargetValuePtr;    
    
    /*
     * dtlNumericType value를 stlInt64 value로 변환한다.
     */
    STL_TRY( dtlNumericToInt64( aDataValue,
                                & sInt64Value,
                                & sIsTruncate,
                                aErrorStack )
             == STL_SUCCESS );

    /*
     * SQLSTATE 상태 확인 
     */
    
    STL_TRY_THROW( (sInt64Value >= STL_UINT32_MIN) &&
                   (sInt64Value <= STL_UINT32_MAX),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    *sTarget = (stlUInt32)(sInt64Value);

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_UINT32_SIZE;
    }

    STL_TRY_THROW( sIsTruncate == STL_FALSE, RAMP_WARNING_FRACTIONAL_TRUNCATION );

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
 * SQL_NUMERIC -> SQL_C_NUMERIC
 */
ZLV_DECLARE_SQL_TO_C( Numeric, Numeric )
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

    /*
     * SQL_NUMERIC_STRUCT 자료구조는 
     * (1) mPrecision과 mScale정보 : 사용자가 설정하는 정보임.
     * (2) mSign : 부호정보  1 (양수) , 0 (음수)
     * (3) mVal : 16진수로 little endian mode로 저장됨.
     *     예) 25.212의 정보는 다음과 같이 각 멤버에 저장됨.
     *         mPrecision = 5  ( 사용자가 지정한 정보 )
     *         mScale = 3       ( 사용자가 지정한 정보 )
     *         mSign = 1 (양수)
     *         mVal : mVal[0] = 0x7C, mVal[1] = 0x62   (25212)
     *     예) 12300의 정보는 다음과 같이 각 멤버에 저장됨.
     *         mPrecision = 5  ( 사용자가 지정한 정보 )
     *         mScale = -1     ( 사용자가 지정한 정보 )
     *         mSign = 1 (양수)
     *         mVal : mVal[0] = 0xCE, mVal[1] = 0x04   (1230)
     * (4) aArdRec->mScale :
     *     mScale >= 0 : mScale (10진수) 만큼 소수값을 출력 한다.
     *     mScale <  0 : mScale (10진수) 만큼 정수값을 trunc 하여 출력 한다.
     */    

    dtlNumericType      * sSrcValue;
    SQL_NUMERIC_STRUCT  * sTarget;

    stlBool               sIsTruncated = STL_FALSE;

    stlStatus             sRet = STL_FAILURE;

    stlInt32              sTargetPrec;
    stlInt32              sTargetScale;
    stlInt32              sTargetSign;

    stlInt32              sRealPrecision;

    stlChar             * sCursor;

    stlInt32              i;
    stlInt32              sTmpValue;
    stlInt32              sDigitCount;
    stlInt32              sTmpDigitCount;
    stlInt32              sZeroLen;
    stlInt32              sExponent;
    stlUInt8            * sDigitPtr;
    stlInt32              sFirstDigitSize;
    stlInt32              sLastDigitSize;
    stlInt32              sMinExpo;
    stlInt32              sMaxExpo;
    stlInt32              sDiffExpo;
    stlInt32              sDiffPrec;
    stlUInt64             sHighDigit;
    stlUInt64             sLowDigit;
    stlUInt128            sResult;
    stlUInt128            sTmp128;

    STL_TRY_THROW( ((ZLV_SQL_TO_C_NUMERIC_MAX_SCALE >= aArdRec->mScale) &&
                    (ZLV_SQL_TO_C_NUMERIC_MIN_SCALE <= aArdRec->mScale)),
                   RAMP_ERR_SCALE_VALUE_OUT_OF_RANGE );
    
    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sSrcValue = (dtlNumericType *)aDataValue->mValue;
    sTarget   = (SQL_NUMERIC_STRUCT *)aTargetValuePtr;

    sTargetPrec  = ZLV_NUMERIC_TO_C_NUMERIC_PRECISION;
    sTargetScale = aArdRec->mScale;

    if( DTL_NUMERIC_IS_ZERO( sSrcValue, aDataValue->mLength ) == STL_TRUE )
    {
        sTargetSign  = STL_TRUE;
        stlMemset( sTarget->val, 0x00, SQL_MAX_NUMERIC_LEN );

        STL_THROW( RAMP_SKIP_SET_VALUE );
    }
    else
    {
        if( DTL_NUMERIC_IS_POSITIVE( sSrcValue ) == STL_TRUE )
        {
            sTargetSign = STL_TRUE;
            sDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( aDataValue->mLength );
            sExponent = DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( sSrcValue );
            sDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sSrcValue );
            sFirstDigitSize = ( (DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigitPtr[0] ) < 10)
                                ? 1 : 2 );
            sLastDigitSize = ( (DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigitPtr[sDigitCount-1] ) % 10) == 0
                               ? 1 : 2 );
        }
        else
        {
            sTargetSign = STL_FALSE;
            sDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( aDataValue->mLength );
            sExponent = DTL_NUMERIC_GET_EXPONENT_FROM_NEGATIVE_VALUE( sSrcValue );
            sDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sSrcValue );
            sFirstDigitSize = ( (DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigitPtr[0] ) < 10)
                                ? 1 : 2 );
            sLastDigitSize = ( (DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigitPtr[sDigitCount-1] ) % 10) == 0
                               ? 1 : 2 );
        }

        sRealPrecision = ((sDigitCount << 1) -
                          ( sFirstDigitSize == 2 ? 0 : 1 ) -
                          ( sLastDigitSize == 2 ? 0 : 1 ));

        /**
         * 구조적으로 39 precision 의 데이터가 존재 할 수 있음.
         */
        STL_TRY_THROW( sRealPrecision <= ZLV_NUMERIC_TO_C_NUMERIC_PRECISION + 1,
                       RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

        /**
         * max precision 을 초과 하는 경우
         * trunc 하며, success 로 간주 한다.
         */
        if( sRealPrecision > sTargetPrec )
        {
            sIsTruncated = STL_FALSE;
            sDiffPrec   = sTargetPrec - sRealPrecision;

            while( sDiffPrec < 0 )
            {               
                if( sLastDigitSize == 2 )
                {
                    sLastDigitSize = 1;
                }
                else
                {
                    STL_DASSERT( sLastDigitSize == 1 );

                    sDigitCount--;
                    STL_DASSERT( sDigitCount > 0 );
                    sLastDigitSize = 2;
                }
                sDiffPrec++;
            }
        }

        sMaxExpo  = sExponent * 2 + sFirstDigitSize;
        sMinExpo  = sExponent * 2 - (sDigitCount-2) * 2 - sLastDigitSize;
        sDiffExpo = sMinExpo + sTargetScale;

        /**
         * precision overflow check
         */       
        if( sExponent < 0 )
        {
            STL_TRY_THROW( sDiffExpo <= ZLV_NUMERIC_TO_C_NUMERIC_PRECISION - sRealPrecision,
                           RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
            
            sZeroLen = 0;
        }
        else if( sDigitCount > sExponent + 1 )
        {
            STL_TRY_THROW( sDiffExpo <= ZLV_NUMERIC_TO_C_NUMERIC_PRECISION - sRealPrecision,
                           RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
            
            if( sTargetScale <= 0 )
            {
                sTmpDigitCount = sDigitCount - ( (-sMinExpo >> 1) + (sLastDigitSize == 1 ? 1 : 0 ) );
                sZeroLen = 0;
            
                for( i = sTmpDigitCount -1; i >= 0; i-- )
                {
                    sTmpValue = DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sTargetSign, sDigitPtr[i] ) % 10;
                    if( sTmpValue != 0 )
                    {
                        break;
                    }

                    sZeroLen++;

                    sTmpValue = DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sTargetSign, sDigitPtr[i] ) / 10;
                    if( sTmpValue != 0 )
                    {
                        break;
                    }

                    sZeroLen++;
                }

                STL_TRY_THROW( sZeroLen +  sTargetScale >= 0,
                               RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
            }
            else
            {
                STL_TRY_THROW( sDiffExpo <= ZLV_NUMERIC_TO_C_NUMERIC_PRECISION - sRealPrecision,
                               RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
            }  
        }
        else
        {
            /**
             * min precision check
             */
            STL_TRY_THROW( sDiffExpo >= 0,
                           RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

            STL_TRY_THROW( (sRealPrecision + sDiffExpo) <= sTargetPrec,
                           RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

            sZeroLen = sMinExpo;
        }

        if( sMaxExpo <= -aArdRec->mScale )
        {
            sTargetSign  = STL_TRUE;
            stlMemset( sTarget->val, 0x00, SQL_MAX_NUMERIC_LEN );
            sIsTruncated = STL_TRUE;

            STL_THROW( RAMP_SKIP_SET_VALUE );
        }
        else
        {
            while( sDiffExpo < 0 )
            {
                sIsTruncated = STL_TRUE;
                
                if( sLastDigitSize == 2 )
                {
                    sLastDigitSize = 1;
                }
                else
                {
                    STL_DASSERT( sLastDigitSize == 1 );

                    sDigitCount--;
                    if( sDigitCount == 0 )
                    {
                        stlMemset( sTarget->val, 0x00, SQL_MAX_NUMERIC_LEN );
                        STL_THROW( RAMP_SKIP_SET_VALUE );
                    }
                    
                    sLastDigitSize = 2;
                }
                sDiffExpo++;
            }

            sHighDigit = 0;
            sLowDigit = 0;

            if( ( sDigitCount < 10 ) ||
                ( ( sDigitCount == 10 ) && ( ( sFirstDigitSize == 1 ) ||
                                             ( sLastDigitSize  == 1 ) ) ) )
            {
                /* low digit 설정 : 최대 19자리 설정 */
                for( i = 0; i < sDigitCount - 1; i++ )
                {
                    sLowDigit *= 100;
                    sLowDigit += DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sTargetSign, sDigitPtr[i] );
                }

                if( sLastDigitSize == 1 )
                {
                    sLowDigit *= 10;
                    sLowDigit += DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sTargetSign, sDigitPtr[i] ) / 10;
                }
                else
                {
                    sLowDigit *= 100;
                    sLowDigit += DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sTargetSign, sDigitPtr[i] );
                }
                
                sResult.mHighDigit = 0;
                sResult.mLowDigit = sLowDigit;
            }
            else
            {
                if( sLastDigitSize == 1 )
                {
                    /* low digit 설정 : 19자리 설정 */
                    for( i = sDigitCount - 10; i < sDigitCount - 1; i++ )
                    {
                        sLowDigit *= 100;
                        sLowDigit += DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sTargetSign, sDigitPtr[i] );
                    }

                    sLowDigit *= 10;
                    sLowDigit += DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sTargetSign, sDigitPtr[i] ) / 10;

                    /* high digit 설정 : 나머지 Digit은 모두 high digit */
                    for( i = 0; i < sDigitCount - 10; i++ )
                    {
                        sHighDigit *= 100;
                        sHighDigit += DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sTargetSign, sDigitPtr[i] );
                    }
                }
                else
                {
                    /* low digit 설정 : 19자리 설정 */
                    sLowDigit += DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sTargetSign, sDigitPtr[sDigitCount - 10] ) % 10;
                        
                    for( i = sDigitCount - 9; i < sDigitCount; i++ )
                    {
                        sLowDigit *= 100;
                        sLowDigit += DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sTargetSign, sDigitPtr[i] );
                    }

                    /* high digit 설정 : 나머지 Digit은 모두 high digit */
                    for( i = 0; i < sDigitCount - 10; i++ )
                    {
                        sHighDigit *= 100;
                        sHighDigit += DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sTargetSign, sDigitPtr[i] );
                    }
                    
                    sHighDigit *= 10;
                    sHighDigit += DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sTargetSign, sDigitPtr[sDigitCount - 10] ) / 10;
                }
                
                STL_TRY_THROW( stlMulUInt64( sHighDigit,
                                             gPreDefinedPow[19],
                                             &sResult )
                               == STL_TRUE,
                               RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

                sTmp128.mHighDigit = 0;
                sTmp128.mLowDigit = sLowDigit;
                STL_TRY_THROW( stlAddUInt128( sResult,
                                              sTmp128,
                                              &sResult )
                               == STL_TRUE,
                               RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
            }
            
            if( sDiffExpo > 0 )
            {
                STL_DASSERT( sDiffExpo < ZLV_NUMERIC_TO_C_NUMERIC_PRECISION );
                
                STL_TRY_THROW( stlMulUInt128( sResult,
                                              gPreDefinedPowUInt128[ sDiffExpo ],
                                              &sResult )
                               == STL_TRUE,
                               RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
            }
        }
        
        /* set var[] */
#ifdef STL_IS_BIGENDIAN
        sCursor = (stlChar*) & sResult.mLowDigit;
        sTarget->val[0]  = sCursor[7];
        sTarget->val[1]  = sCursor[6];
        sTarget->val[2]  = sCursor[5];
        sTarget->val[3]  = sCursor[4];
        sTarget->val[4]  = sCursor[3];
        sTarget->val[5]  = sCursor[2];
        sTarget->val[6]  = sCursor[1];
        sTarget->val[7]  = sCursor[0];

        sCursor = (stlChar*) & sResult.mHighDigit;
        sTarget->val[8]  = sCursor[7];
        sTarget->val[9]  = sCursor[6];
        sTarget->val[10] = sCursor[5];
        sTarget->val[11] = sCursor[4];
        sTarget->val[12] = sCursor[3];
        sTarget->val[13] = sCursor[2];
        sTarget->val[14] = sCursor[1];
        sTarget->val[15] = sCursor[0];
#else
        sCursor = (stlChar*) & sResult.mLowDigit;
        sTarget->val[0] = sCursor[0];
        sTarget->val[1] = sCursor[1];
        sTarget->val[2] = sCursor[2];
        sTarget->val[3] = sCursor[3];
        sTarget->val[4] = sCursor[4];
        sTarget->val[5] = sCursor[5];
        sTarget->val[6] = sCursor[6];
        sTarget->val[7] = sCursor[7];

        sCursor = (stlChar*) & sResult.mHighDigit;
        sTarget->val[8]  = sCursor[0];
        sTarget->val[9]  = sCursor[1];
        sTarget->val[10] = sCursor[2];
        sTarget->val[11] = sCursor[3];
        sTarget->val[12] = sCursor[4];
        sTarget->val[13] = sCursor[5];
        sTarget->val[14] = sCursor[6];
        sTarget->val[15] = sCursor[7];
#endif
    }

    STL_RAMP( RAMP_SKIP_SET_VALUE );
    
    /* SQL_NUMERIC_STRUCT에 sign, precision, scale 정보 설정 */
    sTarget->precision = sTargetPrec;
    sTarget->scale     = sTargetScale;
    sTarget->sign      = sTargetSign;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_SIZEOF( SQL_NUMERIC_STRUCT );
    }

    STL_TRY_THROW( sIsTruncated == STL_FALSE, RAMP_WARNING_FRACTIONAL_TRUNCATION );

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

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
                      "Specified scale value is out of range( -128 to 127)",
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
 * SQL_NUMERIC -> SQL_C_FLOAT
 */
ZLV_DECLARE_SQL_TO_C( Numeric, Float )
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

    stlFloat32     * sTarget;
    
    stlStatus        sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget    = (stlFloat32*)aTargetValuePtr;    

    /*
     * dtlNumericType value를 float32으로 변환
     */

    STL_TRY( dtlNumericToFloat32( aDataValue,
                                  sTarget,
                                  aErrorStack )
             == STL_SUCCESS );

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

    STL_FINISH;

    return sRet;
}

/*
 * SQL_NUMERIC -> SQL_C_DOUBLE
 */
ZLV_DECLARE_SQL_TO_C( Numeric, Double )
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

    stlFloat64     * sTarget;
    
    stlStatus        sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget    = (stlFloat64*)aTargetValuePtr;    

    /*
     * dtlNumericType value를 float32으로 변환
     */

    STL_TRY( dtlNumericToFloat64( aDataValue,
                                  sTarget,
                                  aErrorStack )
             == STL_SUCCESS );
    
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
 * SQL_NUMERIC -> SQL_C_BIT
 */
ZLV_DECLARE_SQL_TO_C( Numeric, Bit )
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

    stlUInt8       * sTarget;

    stlInt64         sInt64Value;
    stlBool          sIsTruncate = STL_FALSE;
    
    stlStatus        sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget    = (stlUInt8*)aTargetValuePtr;    
    
    /*
     * dtlNumericType value를 stlInt64 value로 변환한다.
     */
    STL_TRY( dtlNumericToInt64( aDataValue,
                                & sInt64Value,
                                & sIsTruncate,
                                aErrorStack )
             == STL_SUCCESS );

    /*
     * SQLSTATE 상태 확인 
     */
    
    STL_TRY_THROW( (sInt64Value >= 0) &&
                   (sInt64Value <  2), 
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    *sTarget = (stlUInt8)(sInt64Value);

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = 1;
    }

    STL_TRY_THROW( sIsTruncate == STL_FALSE, RAMP_WARNING_FRACTIONAL_TRUNCATION );

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
 * SQL_NUMERIC -> SQL_C_BINARY
 */
ZLV_DECLARE_SQL_TO_C( Numeric, Binary )
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

    STL_TRY_THROW( aDataValue->mLength <= aArdRec->mOctetLength,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
    
    stlMemcpy( aTargetValuePtr,
               (const void*)aDataValue->mValue,
               aDataValue->mLength );
    
    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = aDataValue->mLength;
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
 * SQL_NUMERIC -> SQL_C_LONGVARBINARY
 */
ZLV_DECLARE_SQL_TO_C( Numeric, LongVarBinary )
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

    STL_TRY_THROW( aDataValue->mLength <= sBufferLength,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
    
    stlMemcpy( sValue,
               (const void*)aDataValue->mValue,
               aDataValue->mLength );
    
    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = aDataValue->mLength;
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
 * SQL_NUMERIC -> SQL_C_INTERVAL_MONTH
 */
ZLV_DECLARE_SQL_TO_C( Numeric, IntervalMonth )
{
    /*
     * ===========================================================================================
     * Test                                 | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ===========================================================================================
     * Data not truncated                   | Data            | Length of data in bytes | n/a
     * Fractional seconds portion truncated | Truncated data  | Length of data in bytes | 01S07
     * Whole part of number truncated       | Undefined       | Undefined               | 22015
     * ===========================================================================================
     */

    SQL_INTERVAL_STRUCT        * sTarget;    

    stlInt64                     sInt64Value;
    stlInt64                     sAbsValue;
    stlBool                      sIsTruncate = STL_FALSE;
    stlFloat64                   sPow;
    
    stlStatus                    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget    = (SQL_INTERVAL_STRUCT*)aTargetValuePtr;
    stlMemset( sTarget, 0x00, STL_SIZEOF( SQL_INTERVAL_STRUCT ) );
    
    /*
     * dtlNumericType value를 stlInt64 value로 변환한다.
     */
    STL_TRY( dtlNumericToInt64( aDataValue,
                                & sInt64Value,
                                & sIsTruncate,
                                aErrorStack )
             == STL_SUCCESS );

    /*
     * SQLSTATE 상태 확인 
     */
    
    sAbsValue  = stlAbsInt64( sInt64Value );
    
    STL_TRY_THROW( stlGet10Pow( aArdRec->mDatetimeIntervalPrecision, &sPow, aErrorStack ) == STL_TRUE,
                   RAMP_ERR_INTERVAL_FIELD_OVERFLOW );
    STL_TRY_THROW( sAbsValue < sPow, RAMP_ERR_INTERVAL_FIELD_OVERFLOW );

    sTarget->interval_type = SQL_IS_MONTH;
    sTarget->intval.year_month.month = (SQLUINTEGER)sAbsValue;

    if( sInt64Value >= 0 )
    {
        sTarget->interval_sign = SQL_FALSE;
    }
    else
    {
        sTarget->interval_sign = SQL_TRUE;
    }

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_SIZEOF( SQL_INTERVAL_STRUCT );
    }

    STL_TRY_THROW( sIsTruncate == STL_FALSE, RAMP_WARNING_FRACTIONAL_TRUNCATION );

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_ERR_INTERVAL_FIELD_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INTERVAL_FIELD_OVERFLOW,
                      "Assigning from an exact numeric or interval SQL type to an interval C type "
                      "caused a loss of significant digits in the leading field.",
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
 * SQL_NUMERIC -> SQL_C_INTERVAL_YEAR
 */
ZLV_DECLARE_SQL_TO_C( Numeric, IntervalYear )
{
    /*
     * ===========================================================================================
     * Test                                 | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ===========================================================================================
     * Data not truncated                   | Data            | Length of data in bytes | n/a
     * Fractional seconds portion truncated | Truncated data  | Length of data in bytes | 01S07
     * Whole part of number truncated       | Undefined       | Undefined               | 22015
     * ===========================================================================================
     */

    SQL_INTERVAL_STRUCT        * sTarget;    

    stlInt64                     sInt64Value;
    stlInt64                     sAbsValue;
    stlBool                      sIsTruncate = STL_FALSE;
    stlFloat64                   sPow;
    
    stlStatus                    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget    = (SQL_INTERVAL_STRUCT*)aTargetValuePtr;
    stlMemset( sTarget, 0x00, STL_SIZEOF( SQL_INTERVAL_STRUCT ) );
    
    /*
     * dtlNumericType value를 stlInt64 value로 변환한다.
     */
    STL_TRY( dtlNumericToInt64( aDataValue,
                                & sInt64Value,
                                & sIsTruncate,
                                aErrorStack )
             == STL_SUCCESS );

    /*
     * SQLSTATE 상태 확인 
     */
    
    sAbsValue  = stlAbsInt64( sInt64Value );
    
    STL_TRY_THROW( stlGet10Pow( aArdRec->mDatetimeIntervalPrecision, &sPow, aErrorStack ) == STL_TRUE,
                   RAMP_ERR_INTERVAL_FIELD_OVERFLOW );
    STL_TRY_THROW( sAbsValue < sPow, RAMP_ERR_INTERVAL_FIELD_OVERFLOW );

    sTarget->interval_type = SQL_IS_YEAR;
    sTarget->intval.year_month.year = (SQLUINTEGER)sAbsValue;

    if( sInt64Value >= 0 )
    {
        sTarget->interval_sign = SQL_FALSE;
    }
    else
    {
        sTarget->interval_sign = SQL_TRUE;
    }

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_SIZEOF( SQL_INTERVAL_STRUCT );
    }

    STL_TRY_THROW( sIsTruncate == STL_FALSE, RAMP_WARNING_FRACTIONAL_TRUNCATION );

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_ERR_INTERVAL_FIELD_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INTERVAL_FIELD_OVERFLOW,
                      "Assigning from an exact numeric or interval SQL type to an interval C type "
                      "caused a loss of significant digits in the leading field.",
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
 * SQL_NUMERIC -> SQL_C_INTERVAL_DAY
 */
ZLV_DECLARE_SQL_TO_C( Numeric, IntervalDay )
{
    /*
     * ===========================================================================================
     * Test                                 | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ===========================================================================================
     * Data not truncated                   | Data            | Length of data in bytes | n/a
     * Fractional seconds portion truncated | Truncated data  | Length of data in bytes | 01S07
     * Whole part of number truncated       | Undefined       | Undefined               | 22015
     * ===========================================================================================
     */

    SQL_INTERVAL_STRUCT        * sTarget;    

    stlInt64                     sInt64Value;
    stlInt64                     sAbsValue;
    stlBool                      sIsTruncate = STL_FALSE;
    stlFloat64                   sPow;
    
    stlStatus                    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget    = (SQL_INTERVAL_STRUCT*)aTargetValuePtr;
    stlMemset( sTarget, 0x00, STL_SIZEOF( SQL_INTERVAL_STRUCT ) );
    
    /*
     * dtlNumericType value를 stlInt64 value로 변환한다.
     */
    STL_TRY( dtlNumericToInt64( aDataValue,
                                & sInt64Value,
                                & sIsTruncate,
                                aErrorStack )
             == STL_SUCCESS );

    /*
     * SQLSTATE 상태 확인 
     */
    
    sAbsValue  = stlAbsInt64( sInt64Value );
    
    STL_TRY_THROW( stlGet10Pow( aArdRec->mDatetimeIntervalPrecision, &sPow, aErrorStack ) == STL_TRUE,
                   RAMP_ERR_INTERVAL_FIELD_OVERFLOW );
    STL_TRY_THROW( sAbsValue < sPow, RAMP_ERR_INTERVAL_FIELD_OVERFLOW );

    sTarget->interval_type = SQL_IS_DAY;
    sTarget->intval.day_second.day = (SQLUINTEGER)sAbsValue;

    if( sInt64Value >= 0 )
    {
        sTarget->interval_sign = SQL_FALSE;
    }
    else
    {
        sTarget->interval_sign = SQL_TRUE;
    }

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_SIZEOF( SQL_INTERVAL_STRUCT );
    }

    STL_TRY_THROW( sIsTruncate == STL_FALSE, RAMP_WARNING_FRACTIONAL_TRUNCATION );

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_ERR_INTERVAL_FIELD_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INTERVAL_FIELD_OVERFLOW,
                      "Assigning from an exact numeric or interval SQL type to an interval C type "
                      "caused a loss of significant digits in the leading field.",
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
 * SQL_NUMERIC -> SQL_C_INTERVAL_HOUR
 */
ZLV_DECLARE_SQL_TO_C( Numeric, IntervalHour )
{
    /*
     * ===========================================================================================
     * Test                                 | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ===========================================================================================
     * Data not truncated                   | Data            | Length of data in bytes | n/a
     * Fractional seconds portion truncated | Truncated data  | Length of data in bytes | 01S07
     * Whole part of number truncated       | Undefined       | Undefined               | 22015
     * ===========================================================================================
     */

    SQL_INTERVAL_STRUCT        * sTarget;    

    stlInt64                     sInt64Value;
    stlInt64                     sAbsValue;
    stlBool                      sIsTruncate = STL_FALSE;
    stlFloat64                   sPow;
    
    stlStatus                    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget    = (SQL_INTERVAL_STRUCT*)aTargetValuePtr;
    stlMemset( sTarget, 0x00, STL_SIZEOF( SQL_INTERVAL_STRUCT ) );
    
    /*
     * dtlNumericType value를 stlInt64 value로 변환한다.
     */
    STL_TRY( dtlNumericToInt64( aDataValue,
                                & sInt64Value,
                                & sIsTruncate,
                                aErrorStack )
             == STL_SUCCESS );

    /*
     * SQLSTATE 상태 확인 
     */
    
    sAbsValue  = stlAbsInt64( sInt64Value );
    
    STL_TRY_THROW( stlGet10Pow( aArdRec->mDatetimeIntervalPrecision, &sPow, aErrorStack ) == STL_TRUE,
                   RAMP_ERR_INTERVAL_FIELD_OVERFLOW );
    STL_TRY_THROW( sAbsValue < sPow, RAMP_ERR_INTERVAL_FIELD_OVERFLOW );

    sTarget->interval_type = SQL_IS_HOUR;;
    sTarget->intval.day_second.hour = (SQLUINTEGER)sAbsValue;

    if( sInt64Value >= 0 )
    {
        sTarget->interval_sign = SQL_FALSE;
    }
    else
    {
        sTarget->interval_sign = SQL_TRUE;
    }

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_SIZEOF( SQL_INTERVAL_STRUCT );
    }

    STL_TRY_THROW( sIsTruncate == STL_FALSE, RAMP_WARNING_FRACTIONAL_TRUNCATION );

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_ERR_INTERVAL_FIELD_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INTERVAL_FIELD_OVERFLOW,
                      "Assigning from an exact numeric or interval SQL type to an interval C type "
                      "caused a loss of significant digits in the leading field.",
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
 * SQL_NUMERIC -> SQL_C_INTERVAL_MINUTE
 */
ZLV_DECLARE_SQL_TO_C( Numeric, IntervalMinute )
{    
    /*
     * ===========================================================================================
     * Test                                 | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ===========================================================================================
     * Data not truncated                   | Data            | Length of data in bytes | n/a
     * Fractional seconds portion truncated | Truncated data  | Length of data in bytes | 01S07
     * Whole part of number truncated       | Undefined       | Undefined               | 22015
     * ===========================================================================================
     */

    SQL_INTERVAL_STRUCT        * sTarget;    

    stlInt64                     sInt64Value;
    stlInt64                     sAbsValue;
    stlBool                      sIsTruncate = STL_FALSE;
    stlFloat64                   sPow;
    
    stlStatus                    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget    = (SQL_INTERVAL_STRUCT*)aTargetValuePtr;
    stlMemset( sTarget, 0x00, STL_SIZEOF( SQL_INTERVAL_STRUCT ) );
    
    /*
     * dtlNumericType value를 stlInt64 value로 변환한다.
     */
    STL_TRY( dtlNumericToInt64( aDataValue,
                                & sInt64Value,
                                & sIsTruncate,
                                aErrorStack )
             == STL_SUCCESS );

    /*
     * SQLSTATE 상태 확인 
     */
    
    sAbsValue  = stlAbsInt64( sInt64Value );
    
    STL_TRY_THROW( stlGet10Pow( aArdRec->mDatetimeIntervalPrecision, &sPow, aErrorStack ) == STL_TRUE,
                   RAMP_ERR_INTERVAL_FIELD_OVERFLOW );
    STL_TRY_THROW( sAbsValue < sPow, RAMP_ERR_INTERVAL_FIELD_OVERFLOW );

    sTarget->interval_type = SQL_IS_MINUTE;
    sTarget->intval.day_second.minute = (SQLUINTEGER)sAbsValue;

    if( sInt64Value >= 0 )
    {
        sTarget->interval_sign = SQL_FALSE;
    }
    else
    {
        sTarget->interval_sign = SQL_TRUE;
    }

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_SIZEOF( SQL_INTERVAL_STRUCT );
    }

    STL_TRY_THROW( sIsTruncate == STL_FALSE, RAMP_WARNING_FRACTIONAL_TRUNCATION );

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_ERR_INTERVAL_FIELD_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INTERVAL_FIELD_OVERFLOW,
                      "Assigning from an exact numeric or interval SQL type to an interval C type "
                      "caused a loss of significant digits in the leading field.",
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
 * SQL_NUMERIC -> SQL_C_INTERVAL_SECOND
 */
ZLV_DECLARE_SQL_TO_C( Numeric, IntervalSecond )
{
    /*
     * ===========================================================================================
     * Test                                 | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ===========================================================================================
     * Data not truncated                   | Data            | Length of data in bytes | n/a
     * Fractional seconds portion truncated | Truncated data  | Length of data in bytes | 01S07
     * Whole part of number truncated       | Undefined       | Undefined               | 22015
     * ===========================================================================================
     */

    SQL_INTERVAL_STRUCT        * sTarget;

    stlInt64                     sInt64Value;
    stlInt64                     sAbsValue;
    stlFloat64                   sPow;
    stlUInt32                    sSecond;
    stlUInt32                    sFraction;
    stlBool                      sIsNegative;
    stlBool                      sSuccessWithInfo = STL_FALSE;
    stlBool                      sIsTruncate;
    
    stlStatus                    sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget    = (SQL_INTERVAL_STRUCT*)aTargetValuePtr;
    stlMemset( sTarget, 0x00, STL_SIZEOF( SQL_INTERVAL_STRUCT ) );
    
    /*
     * dtlNumericType value를 stlInt64 value로 변환한다.
     */
    STL_TRY( dtlNumericToInt64( aDataValue,
                                & sInt64Value,
                                & sIsTruncate,
                                aErrorStack )
             == STL_SUCCESS );

    sAbsValue  = stlAbsInt64( sInt64Value );
    
    STL_TRY_THROW( stlGet10Pow( aArdRec->mDatetimeIntervalPrecision, &sPow, aErrorStack ) == STL_TRUE,
                   RAMP_ERR_INTERVAL_FIELD_OVERFLOW );
    STL_TRY_THROW( sAbsValue < sPow, RAMP_ERR_INTERVAL_FIELD_OVERFLOW );

    STL_TRY( dtlGetIntervalSecondInfoFromDtlNumericValue( aDataValue,
                                                          DTL_INTERVAL_INDICATOR_SECOND,
                                                          aArdRec->mDatetimeIntervalPrecision,
                                                          aArdRec->mPrecision,
                                                          & sSecond,
                                                          & sFraction,
                                                          & sIsNegative,
                                                          & sSuccessWithInfo,
                                                          aErrorStack )
             == STL_SUCCESS );

    sTarget->interval_type              = SQL_IS_SECOND;
    sTarget->interval_sign              = (SQLSMALLINT)sIsNegative;    
    sTarget->intval.day_second.second   = (SQLUINTEGER)sSecond;
    sTarget->intval.day_second.fraction = (SQLUINTEGER)sFraction * 1000;
    
    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_SIZEOF( SQL_INTERVAL_STRUCT );
    }

    STL_TRY_THROW( sSuccessWithInfo == STL_FALSE, RAMP_WARNING_FRACTIONAL_TRUNCATION );

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_ERR_INTERVAL_FIELD_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INTERVAL_FIELD_OVERFLOW,
                      "Assigning from an exact numeric or interval SQL type to an interval C type "
                      "caused a loss of significant digits in the leading field.",
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
 * SQL_NUMERIC -> SQL_C_INTERVAL_YEAR_TO_MONTH
 */
ZLV_DECLARE_SQL_TO_C( Numeric, IntervalYearToMonth )
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
                  "When fetching data to an interval C type, there was no representation of "
                  "the value of the SQL type in the interval C type.",
                  aErrorStack );

    return STL_FAILURE;
}

/*
 * SQL_NUMERIC -> SQL_C_INTERVAL_DAY_TO_HOUR
 */
ZLV_DECLARE_SQL_TO_C( Numeric, IntervalDayToHour )
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
                  "When fetching data to an interval C type, there was no representation of "
                  "the value of the SQL type in the interval C type.",
                  aErrorStack );

    return STL_FAILURE;
}

/*
 * SQL_NUMERIC -> SQL_C_INTERVAL_DAY_TO_MINUTE
 */
ZLV_DECLARE_SQL_TO_C( Numeric, IntervalDayToMinute )
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
                  "When fetching data to an interval C type, there was no representation of "
                  "the value of the SQL type in the interval C type.",
                  aErrorStack );

    return STL_FAILURE;
}

/*
 * SQL_NUMERIC -> SQL_C_INTERVAL_DAY_TO_SECOND
 */
ZLV_DECLARE_SQL_TO_C( Numeric, IntervalDayToSecond )
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
                  "When fetching data to an interval C type, there was no representation of "
                  "the value of the SQL type in the interval C type.",
                  aErrorStack );

    return STL_FAILURE;
}

/*
 * SQL_NUMERIC -> SQL_C_INTERVAL_HOUR_TO_MINUTE
 */
ZLV_DECLARE_SQL_TO_C( Numeric, IntervalHourToMinute )
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
                  "When fetching data to an interval C type, there was no representation of "
                  "the value of the SQL type in the interval C type.",
                  aErrorStack );

    return STL_FAILURE;
}

/*
 * SQL_NUMERIC -> SQL_C_INTERVAL_HOUR_TO_SECOND
 */
ZLV_DECLARE_SQL_TO_C( Numeric, IntervalHourToSecond )
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
                  "When fetching data to an interval C type, there was no representation of "
                  "the value of the SQL type in the interval C type.",
                  aErrorStack );

    return STL_FAILURE;
}

/*
 * SQL_NUMERIC -> SQL_C_INTERVAL_MINUTE_TO_SECOND
 */
ZLV_DECLARE_SQL_TO_C( Numeric, IntervalMinuteToSecond )
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
                  "When fetching data to an interval C type, there was no representation of "
                  "the value of the SQL type in the interval C type.",
                  aErrorStack );

    return STL_FAILURE;
}

/** @} */
