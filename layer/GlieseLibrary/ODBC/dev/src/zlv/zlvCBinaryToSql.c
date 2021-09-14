/*******************************************************************************
 * zlvCBinaryToSql.c
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
 * @file zlvCBinaryToSql.c
 * @brief Gliese API Internal Converting Data from C Binary to SQL Data Types Routines.
 */

/**
 * @addtogroup zlvCBinaryToSql
 * @{
 */

/*
 * http://msdn.microsoft.com/en-us/library/windows/desktop/ms711002%28v=VS.85%29.aspx
 */

/*
 * SQL_C_BINARY -> SQL_CHAR
 */
ZLV_DECLARE_C_TO_SQL( Binary, Char )
{
    /*
     * ===================================================================
     * Test                                  | SQLSTATE
     * ===================================================================
     * Byte length of data <= Column length. | n/a
     * Byte length of data > Column length.  | 22001
     * ===================================================================
     */

    /**
     * @bug MSDN의 내용이 맞는 것일까? 우선 1byte 바이너리를 2byte 문자로 변환하도록 한다.
     */

    stlAllocator  * sAllocator = NULL;
    stlRegionMark   sMark;
    stlChar       * sCharString = NULL;
    stlInt64        sCharStringLength;
    stlBool         sIsMarked = STL_FALSE;
    stlBool         sSuccessWithInfo;
    stlInt64        sLength;
    stlInt64        sHexLength;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mLength,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_OCTETS,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( (stlChar*)aParameterValuePtr );
    }
    else
    {
        sLength = *aIndicator;
    }

    sCharStringLength = sLength * 2;

    STL_TRY_THROW( sCharStringLength <= aIpdRec->mLength,
                   RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED );

    sAllocator = &aIpdRec->mAllocator;
    
    STL_TRY( stlMarkRegionMem( sAllocator,
                               &sMark,
                               aErrorStack ) == STL_SUCCESS );

    sIsMarked = STL_TRUE;

    STL_TRY( stlAllocRegionMem( sAllocator,
                                sCharStringLength + 1,
                                (void**)&sCharString,
                                aErrorStack ) == STL_SUCCESS );

    STL_TRY( dtlGetStringFromBinaryString( (stlInt8*)aParameterValuePtr,
                                           sLength,
                                           sCharStringLength,
                                           sCharString,
                                           &sHexLength,
                                           aErrorStack )
             == STL_SUCCESS );

    sCharString[sHexLength] = 0;

    STL_TRY( dtlCharSetValueFromStringWithoutPaddNull( sCharString,
                                                       sCharStringLength,
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
    
    if( sIsMarked == STL_TRUE )
    {
        STL_TRY( stlRestoreRegionMem( sAllocator,
                                      &sMark,
                                      STL_FALSE,
                                      aErrorStack ) == STL_SUCCESS );
        sCharString = NULL;
    }

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

    if( sIsMarked == STL_TRUE )
    {
        (void)stlRestoreRegionMem( sAllocator,
                                   &sMark,
                                   STL_FALSE,
                                   aErrorStack );
        sCharString = NULL;
    }

    return STL_FAILURE;
}

/*
 * SQL_C_BINARY -> SQL_VARCHAR
 */
ZLV_DECLARE_C_TO_SQL( Binary, VarChar )
{
    /*
     * ===================================================================
     * Test                                  | SQLSTATE
     * ===================================================================
     * Byte length of data <= Column length. | n/a
     * Byte length of data > Column length.  | 22001
     * ===================================================================
     */

    /**
     * @bug MSDN의 내용이 맞는 것일까? 우선 1byte 바이너리를 2byte 문자로 변환하도록 한다.
     */

    stlAllocator  * sAllocator = NULL;
    stlRegionMark   sMark;
    stlChar       * sCharString = NULL;
    stlInt64        sCharStringLength;
    stlBool         sIsMarked = STL_FALSE;
    stlBool         sSuccessWithInfo;
    stlInt64        sLength;
    stlInt64        sHexLength;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mLength,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_OCTETS,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( (stlChar*)aParameterValuePtr );
    }
    else
    {
        sLength = *aIndicator;
    }

    sCharStringLength = sLength * 2;

    STL_TRY_THROW( sCharStringLength <= aIpdRec->mLength,
                   RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED );

    sAllocator = &aIpdRec->mAllocator;
    
    STL_TRY( stlMarkRegionMem( sAllocator,
                               &sMark,
                               aErrorStack ) == STL_SUCCESS );

    sIsMarked = STL_TRUE;

    STL_TRY( stlAllocRegionMem( sAllocator,
                                sCharStringLength + 1,
                                (void**)&sCharString,
                                aErrorStack ) == STL_SUCCESS );

    STL_TRY( dtlGetStringFromBinaryString( (stlInt8*)aParameterValuePtr,
                                           sLength,
                                           sCharStringLength,
                                           sCharString,
                                           &sHexLength,
                                           aErrorStack )
             == STL_SUCCESS );

    sCharString[sHexLength] = 0;

    STL_TRY( dtlVarcharSetValueFromString( sCharString,
                                           sCharStringLength,
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

    if( sIsMarked == STL_TRUE )
    {
        STL_TRY( stlRestoreRegionMem( sAllocator,
                                      &sMark,
                                      STL_FALSE,
                                      aErrorStack ) == STL_SUCCESS );
        sCharString = NULL;
    }

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

    if( sIsMarked == STL_TRUE )
    {
        (void)stlRestoreRegionMem( sAllocator,
                                   &sMark,
                                   STL_FALSE,
                                   aErrorStack );
        sCharString = NULL;
    }

    return STL_FAILURE;
}


/*
 * SQL_C_BINARY -> SQL_LONGVARCHAR
 */
ZLV_DECLARE_C_TO_SQL( Binary, LongVarChar )
{
    /*
     * ===================================================================
     * Test                                  | SQLSTATE
     * ===================================================================
     * Byte length of data <= Column length. | n/a
     * Byte length of data > Column length.  | 22001
     * ===================================================================
     */

    /**
     * @bug MSDN의 내용이 맞는 것일까? 우선 1byte 바이너리를 2byte 문자로 변환하도록 한다.
     */

    stlAllocator  * sAllocator;
    stlRegionMark   sMark;
    stlChar       * sCharString = NULL;
    stlInt64        sCharStringLength;
    stlBool         sIsMarked = STL_FALSE;
    stlBool         sSuccessWithInfo;
    stlInt64        sLength;
    stlInt64        sHexLength;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( (stlChar*)aParameterValuePtr );
    }
    else
    {
        sLength = *aIndicator;
    }

    sCharStringLength = sLength * 2;

    sAllocator = &aIpdRec->mAllocator;
    
    STL_TRY( stlMarkRegionMem( sAllocator,
                               &sMark,
                               aErrorStack ) == STL_SUCCESS );

    sIsMarked = STL_TRUE;

    STL_TRY( stlAllocRegionMem( sAllocator,
                                sCharStringLength + 1,
                                (void**)&sCharString,
                                aErrorStack ) == STL_SUCCESS );

    STL_TRY( dtlGetStringFromBinaryString( (stlInt8*)aParameterValuePtr,
                                           sLength,
                                           sCharStringLength,
                                           sCharString,
                                           &sHexLength,
                                           aErrorStack )
             == STL_SUCCESS );

    sCharString[sHexLength] = 0;

    STL_TRY( dtlLongvarcharSetValueFromString( sCharString,
                                               sCharStringLength,
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

    if( sIsMarked == STL_TRUE )
    {
        STL_TRY( stlRestoreRegionMem( sAllocator,
                                      &sMark,
                                      STL_FALSE,
                                      aErrorStack ) == STL_SUCCESS );
        sCharString = NULL;
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sIsMarked == STL_TRUE )
    {
        (void)stlRestoreRegionMem( sAllocator,
                                   &sMark,
                                   STL_FALSE,
                                   aErrorStack );
        sCharString = NULL;
    }

    return STL_FAILURE;
}


/*
 * SQL_C_BINARY -> SQL_FLOAT
 */
ZLV_DECLARE_C_TO_SQL( Binary, Float )
{
    /*
     * ===================================================
     * Test                                     | SQLSTATE
     * ===================================================
     * Byte length of data = SQL data length.   | n/a
     * Byte length of data <> SQL data length.  | 22003
     * ===================================================
     */

    stlInt64         sLength;
    dtlNumericType * sNumeric;
    stlInt32         sExpo;
    stlInt32         sLoop;

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
        sLength = stlStrlen( (stlChar*)aParameterValuePtr );
    }
    else
    {
        sLength = *aIndicator;
    }

    /*
     * Numeric이 가변이므로, Precision 이 갖을 수 있는 최대 Buffer 크기보다 작거나 같으면 된다.
     * Numeric Header Size보다는 커야 한다.
     */

    STL_TRY_THROW( (sLength >= DTL_NUMERIC_MIN_SIZE) &&
                   (sLength <= DTL_NUMERIC_SIZE( dtlBinaryToDecimalPrecision[aIpdRec->mPrecision] )),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    /* numeric validation */
    sNumeric = (dtlNumericType *) aParameterValuePtr;
    if( sLength == 1 )
    {
        STL_TRY_THROW( DTL_NUMERIC_IS_ZERO( sNumeric, sLength ),
                       RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
    }
    else
    {
        if( DTL_NUMERIC_IS_POSITIVE( sNumeric ) )
        {
            sExpo = DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( sNumeric );
        
            STL_TRY_THROW( ( sExpo >= DTL_NUMERIC_MIN_EXPONENT ) &&
                           ( sExpo <= DTL_NUMERIC_MAX_EXPONENT ),
                           RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

            for( sLoop = 1 ; sLoop < sLength ; sLoop++ )
            {
                STL_TRY_THROW( ( sNumeric->mData[ sLoop ] >= DTL_NUMERIC_POSITIVE_DIGIT_MIN_VALUE ) &&
                               ( sNumeric->mData[ sLoop ] <= DTL_NUMERIC_POSITIVE_DIGIT_MAX_VALUE ),
                               RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
            }
        }
        else
        {
            sExpo = DTL_NUMERIC_GET_EXPONENT_FROM_NEGATIVE_VALUE( sNumeric );
        
            STL_TRY_THROW( ( sExpo >= DTL_NUMERIC_MIN_EXPONENT ) &&
                           ( sExpo <= DTL_NUMERIC_MAX_EXPONENT ),
                           RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

            for( sLoop = 1 ; sLoop < sLength ; sLoop++ )
            {
                STL_TRY_THROW( ( sNumeric->mData[ sLoop ] >= DTL_NUMERIC_NEGATIVE_DIGIT_MIN_VALUE ) &&
                               ( sNumeric->mData[ sLoop ] <= DTL_NUMERIC_NEGATIVE_DIGIT_MAX_VALUE ),
                               RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
            }
        }
    }
    
    stlMemcpy( aDataValue->mValue, aParameterValuePtr, sLength );
    aDataValue->mLength = sLength;

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
 * SQL_C_BINARY -> SQL_NUMERIC
 */
ZLV_DECLARE_C_TO_SQL( Binary, Numeric )
{
    /*
     * ===================================================
     * Test                                     | SQLSTATE
     * ===================================================
     * Byte length of data = SQL data length.   | n/a
     * Byte length of data <> SQL data length.  | 22003
     * ===================================================
     */

    stlInt64         sLength;
    dtlNumericType * sNumeric;
    stlInt32         sExpo;
    stlInt32         sLoop;

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
        sLength = stlStrlen( (stlChar*)aParameterValuePtr );
    }
    else
    {
        sLength = *aIndicator;
    }

    /*
     * Numeric이 가변이므로, Precision 이 갖을 수 있는 최대 Buffer 크기보다 작거나 같으면 된다.
     * Numeric Header Size보다는 커야 한다.
     */

    STL_TRY_THROW( sLength >= DTL_NUMERIC_MIN_SIZE && sLength <= DTL_NUMERIC_SIZE( aIpdRec->mPrecision ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    /* numeric validation */
    sNumeric = (dtlNumericType *) aParameterValuePtr;
    if( sLength == 1 )
    {
        STL_TRY_THROW( DTL_NUMERIC_IS_ZERO( sNumeric, sLength ),
                       RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
    }
    else
    {
        if( DTL_NUMERIC_IS_POSITIVE( sNumeric ) )
        {
            sExpo = DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( sNumeric );
        
            STL_TRY_THROW( ( sExpo >= DTL_NUMERIC_MIN_EXPONENT ) &&
                           ( sExpo <= DTL_NUMERIC_MAX_EXPONENT ),
                           RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

            for( sLoop = 1 ; sLoop < sLength ; sLoop++ )
            {
                STL_TRY_THROW( ( sNumeric->mData[ sLoop ] >= DTL_NUMERIC_POSITIVE_DIGIT_MIN_VALUE ) &&
                               ( sNumeric->mData[ sLoop ] <= DTL_NUMERIC_POSITIVE_DIGIT_MAX_VALUE ),
                               RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
            }
        }
        else
        {
            sExpo = DTL_NUMERIC_GET_EXPONENT_FROM_NEGATIVE_VALUE( sNumeric );
        
            STL_TRY_THROW( ( sExpo >= DTL_NUMERIC_MIN_EXPONENT ) &&
                           ( sExpo <= DTL_NUMERIC_MAX_EXPONENT ),
                           RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

            for( sLoop = 1 ; sLoop < sLength ; sLoop++ )
            {
                STL_TRY_THROW( ( sNumeric->mData[ sLoop ] >= DTL_NUMERIC_NEGATIVE_DIGIT_MIN_VALUE ) &&
                               ( sNumeric->mData[ sLoop ] <= DTL_NUMERIC_NEGATIVE_DIGIT_MAX_VALUE ),
                               RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
            }
        }
    }
    
    stlMemcpy( aDataValue->mValue, aParameterValuePtr, sLength );
    aDataValue->mLength = sLength;

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
 * SQL_C_BINARY -> SQL_TINYINT
 */
ZLV_DECLARE_C_TO_SQL( Binary, TinyInt )
{
    /*
     * ===================================================
     * Test                                     | SQLSTATE
     * ===================================================
     * Byte length of data = SQL data length.   | n/a
     * Byte length of data <> SQL data length.  | 22003
     * ===================================================
     */

    stlBool  sSuccessWithInfo;
    stlInt64 sLength;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( (stlChar*)aParameterValuePtr );
    }
    else
    {
        sLength = *aIndicator;
    }

    STL_TRY_THROW( sLength == 1, RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    /*
     * DataType에 TinyInt 타입이 없으므로 SmallInt로 변환한다.
     */
    STL_TRY( dtlSmallIntSetValueFromInteger( *(stlInt8*)aParameterValuePtr,
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
 * SQL_C_BINARY -> SQL_SMALLINT
 */
ZLV_DECLARE_C_TO_SQL( Binary, SmallInt )
{
    /*
     * ===================================================
     * Test                                     | SQLSTATE
     * ===================================================
     * Byte length of data = SQL data length.   | n/a
     * Byte length of data <> SQL data length.  | 22003
     * ===================================================
     */

    stlInt64 sLength;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( (stlChar*)aParameterValuePtr );
    }
    else
    {
        sLength = *aIndicator;
    }

    STL_TRY_THROW( sLength == DTL_NATIVE_SMALLINT_SIZE,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    stlMemcpy( aDataValue->mValue, aParameterValuePtr, sLength );
    aDataValue->mLength = DTL_NATIVE_SMALLINT_SIZE;

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
 * SQL_C_BINARY -> SQL_INTEGER
 */
ZLV_DECLARE_C_TO_SQL( Binary, Integer )
{
    /*
     * ===================================================
     * Test                                     | SQLSTATE
     * ===================================================
     * Byte length of data = SQL data length.   | n/a
     * Byte length of data <> SQL data length.  | 22003
     * ===================================================
     */

    stlInt64 sLength;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( (stlChar*)aParameterValuePtr );
    }
    else
    {
        sLength = *aIndicator;
    }

    STL_TRY_THROW( sLength == DTL_NATIVE_INTEGER_SIZE,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    stlMemcpy( aDataValue->mValue, aParameterValuePtr, sLength );
    aDataValue->mLength = DTL_NATIVE_INTEGER_SIZE;

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
 * SQL_C_BINARY -> SQL_BIGINT
 */
ZLV_DECLARE_C_TO_SQL( Binary, BigInt )
{
    /*
     * ===================================================
     * Test                                     | SQLSTATE
     * ===================================================
     * Byte length of data = SQL data length.   | n/a
     * Byte length of data <> SQL data length.  | 22003
     * ===================================================
     */

    stlInt64 sLength;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( (stlChar*)aParameterValuePtr );
    }
    else
    {
        sLength = *aIndicator;
    }

    STL_TRY_THROW( sLength == DTL_NATIVE_BIGINT_SIZE,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    stlMemcpy( aDataValue->mValue, aParameterValuePtr, sLength );
    aDataValue->mLength = DTL_NATIVE_BIGINT_SIZE;

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
 * SQL_C_BINARY -> SQL_REAL
 */
ZLV_DECLARE_C_TO_SQL( Binary, Real )
{
    /*
     * ===================================================
     * Test                                     | SQLSTATE
     * ===================================================
     * Byte length of data = SQL data length.   | n/a
     * Byte length of data <> SQL data length.  | 22003
     * ===================================================
     */

    stlInt64 sLength;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( (stlChar*)aParameterValuePtr );
    }
    else
    {
        sLength = *aIndicator;
    }

    STL_TRY_THROW( sLength == DTL_NATIVE_REAL_SIZE,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    stlMemcpy( aDataValue->mValue, aParameterValuePtr, sLength );
    aDataValue->mLength = DTL_NATIVE_REAL_SIZE;

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
 * SQL_C_BINARY -> SQL_DOUBLE
 */
ZLV_DECLARE_C_TO_SQL( Binary, Double )
{
    /*
     * ===================================================
     * Test                                     | SQLSTATE
     * ===================================================
     * Byte length of data = SQL data length.   | n/a
     * Byte length of data <> SQL data length.  | 22003
     * ===================================================
     */

    stlInt64 sLength;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( (stlChar*)aParameterValuePtr );
    }
    else
    {
        sLength = *aIndicator;
    }

    STL_TRY_THROW( sLength == DTL_NATIVE_DOUBLE_SIZE,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    stlMemcpy( aDataValue->mValue, aParameterValuePtr, sLength );
    aDataValue->mLength = DTL_NATIVE_DOUBLE_SIZE;

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
 * SQL_C_BINARY -> SQL_BIT
 */
ZLV_DECLARE_C_TO_SQL( Binary, Bit )
{
    /*
     * ===================================================
     * Test                                     | SQLSTATE
     * ===================================================
     * Byte length of data = SQL data length.   | n/a
     * Byte length of data <> SQL data length.  | 22003
     * ===================================================
     */

    stlInt64 sLength;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( (stlChar*)aParameterValuePtr );
    }
    else
    {
        sLength = *aIndicator;
    }

    STL_TRY_THROW( sLength == DTL_BOOLEAN_SIZE,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    stlMemcpy( aDataValue->mValue, aParameterValuePtr, sLength );
    aDataValue->mLength = DTL_BOOLEAN_SIZE;

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
 * SQL_C_BINARY -> SQL_BOOLEAN
 */
ZLV_DECLARE_C_TO_SQL( Binary, Boolean )
{
    return zlvCBinaryToSqlBit( aStmt,
                               aParameterValuePtr,
                               aIndicator,
                               aApdRec,
                               aIpdRec,
                               aDataValue,
                               aSuccessWithInfo,
                               aErrorStack );
}

/*
 * SQL_C_BINARY -> SQL_BINARY
 */
ZLV_DECLARE_C_TO_SQL( Binary, Binary )
{
    /*
     * ==================================================
     * Test                                    | SQLSTATE
     * ==================================================
     * Length of data <= column length         | n/a
     * Length of data > column length          | 22001
     * ==================================================
     */

    stlInt64   sLength;

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
        sLength = stlStrlen( (stlChar*)aParameterValuePtr );
    }
    else
    {
        sLength = *aIndicator;
    }

    STL_TRY_THROW( sLength <= aIpdRec->mLength,
                   RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED );

    stlMemcpy( aDataValue->mValue, aParameterValuePtr, sLength );

    aDataValue->mLength = sLength;

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
 * SQL_C_BINARY -> SQL_VARBINARY
 */
ZLV_DECLARE_C_TO_SQL( Binary, VarBinary )
{
    /*
     * ==================================================
     * Test                                    | SQLSTATE
     * ==================================================
     * Length of data <= column length         | n/a
     * Length of data > column length          | 22001
     * ==================================================
     */

    stlInt64 sLength;

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
        sLength = stlStrlen( (stlChar*)aParameterValuePtr );
    }
    else
    {
        sLength = *aIndicator;
    }

    STL_TRY_THROW( sLength <= aIpdRec->mLength,
                   RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED );

    stlMemcpy( aDataValue->mValue, aParameterValuePtr, sLength );
    aDataValue->mLength = sLength;

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
 * SQL_C_BINARY -> SQL_LONGVARBINARY
 */
ZLV_DECLARE_C_TO_SQL( Binary, LongVarBinary )
{
    /*
     * ==================================================
     * Test                                    | SQLSTATE
     * ==================================================
     * Length of data <= column length         | n/a
     * Length of data > column length          | 22001
     * ==================================================
     */

    stlInt64 sLength;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( (stlChar*)aParameterValuePtr );
    }
    else
    {
        sLength = *aIndicator;
    }

    STL_TRY_THROW( sLength <= DTL_LONGVARBINARY_MAX_PRECISION,
                   RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED );

    if( sLength > aDataValue->mBufferSize )
    {
        STL_TRY( (ZLS_STMT_DT_VECTOR(aStmt))->mReallocLongVaryingMemFunc( (void*)aStmt,
                                                                          sLength,
                                                                          &aDataValue->mValue,
                                                                          aErrorStack )
                 == STL_SUCCESS );

        aDataValue->mBufferSize = sLength;
    }

    stlMemcpy( aDataValue->mValue, aParameterValuePtr, sLength );
    aDataValue->mLength = sLength;

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
 * SQL_C_BINARY -> SQL_TYPE_DATE
 */
ZLV_DECLARE_C_TO_SQL( Binary, Date )
{
    /*
     * ===================================================
     * Test                                     | SQLSTATE
     * ===================================================
     * Byte length of data = SQL data length.   | n/a
     * Byte length of data <> SQL data length.  | 22003
     * ===================================================
     */

    stlInt64 sLength;

    if( aIndicator == NULL )
    {
        sLength = stlStrlen( (stlChar*)aParameterValuePtr );
    }
    else
    {
        sLength = *aIndicator;
    }

    STL_TRY_THROW( sLength == DTL_DATE_SIZE,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    stlMemcpy( aDataValue->mValue, aParameterValuePtr, sLength );
    aDataValue->mLength = DTL_DATE_SIZE;

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
 * SQL_C_BINARY -> SQL_TYPE_TIME
 */
ZLV_DECLARE_C_TO_SQL( Binary, Time )
{
    /*
     * ===================================================
     * Test                                     | SQLSTATE
     * ===================================================
     * Byte length of data = SQL data length.   | n/a
     * Byte length of data <> SQL data length.  | 22003
     * ===================================================
     */

    stlInt64 sLength;

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
        sLength = stlStrlen( (stlChar*)aParameterValuePtr );
    }
    else
    {
        sLength = *aIndicator;
    }

    STL_TRY_THROW( sLength == DTL_TIME_SIZE,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    stlMemcpy( aDataValue->mValue, aParameterValuePtr, sLength );
    aDataValue->mLength = DTL_TIME_SIZE;

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
 * SQL_C_BINARY -> SQL_TYPE_TIME_TZ
 */
ZLV_DECLARE_C_TO_SQL( Binary, TimeTz )
{
    stlInt64 sLength;

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
        sLength = stlStrlen( (stlChar*)aParameterValuePtr );
    }
    else
    {
        sLength = *aIndicator;
    }

    STL_TRY_THROW( sLength == DTL_TIMETZ_SIZE,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    stlMemcpy( aDataValue->mValue, aParameterValuePtr, sLength );
    aDataValue->mLength = DTL_TIMETZ_SIZE;

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
 * SQL_C_BINARY -> SQL_TYPE_TIMESTAMP
 */
ZLV_DECLARE_C_TO_SQL( Binary, Timestamp )
{
    /*
     * ===================================================
     * Test                                     | SQLSTATE
     * ===================================================
     * Byte length of data = SQL data length.   | n/a
     * Byte length of data <> SQL data length.  | 22003
     * ===================================================
     */

    stlInt64 sLength;

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
        sLength = stlStrlen( (stlChar*)aParameterValuePtr );
    }
    else
    {
        sLength = *aIndicator;
    }

    STL_TRY_THROW( sLength == DTL_TIMESTAMP_SIZE,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    stlMemcpy( aDataValue->mValue, aParameterValuePtr, sLength );
    aDataValue->mLength = DTL_TIMESTAMP_SIZE;

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
 * SQL_C_BINARY -> SQL_TYPE_TIMESTAMP_TZ
 */
ZLV_DECLARE_C_TO_SQL( Binary, TimestampTz )
{
    /*
     * ===================================================
     * Test                                     | SQLSTATE
     * ===================================================
     * Byte length of data = SQL data length.   | n/a
     * Byte length of data <> SQL data length.  | 22003
     * ===================================================
     */

    stlInt64 sLength;

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
        sLength = stlStrlen( (stlChar*)aParameterValuePtr );
    }
    else
    {
        sLength = *aIndicator;
    }

    STL_TRY_THROW( sLength == DTL_TIMESTAMPTZ_SIZE,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    stlMemcpy( aDataValue->mValue, aParameterValuePtr, sLength );
    aDataValue->mLength = DTL_TIMESTAMPTZ_SIZE;

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

/** @} */

