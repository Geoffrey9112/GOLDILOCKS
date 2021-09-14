/*******************************************************************************
 * zlvCNumericToSql.c
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
 * @file zlvCNumericToSql.c
 * @brief Gliese API Internal Converting Data from C Numeric to SQL Data Types Routines.
 */

/**
 * @addtogroup zlvCNumericToSql
 * @{
 */

/*
 * http://msdn.microsoft.com/en-us/library/windows/desktop/ms714147%28v=VS.85%29.aspx
 */

static stlStatus zlvValidationCNumericWithAPD( SQL_NUMERIC_STRUCT   * aNumeric,
                                               zldDescElement       * aApdRec,
                                               stlErrorStack        * aErrorStack );

static stlStatus zlvSqlNumericStructToDecimalString( SQL_NUMERIC_STRUCT  * aSqlNumericStruct,
                                                     stlUInt32             aAvailableSize,
                                                     stlChar             * aDecimalString,
                                                     stlUInt32           * aLength,
                                                     stlInt16              aScale );

static stlStatus zlvGetUInt128FromCNumeric( SQL_NUMERIC_STRUCT  * aNumeric,
                                            stlUInt128          * aUInt128Value,
                                            stlInt16              aScale,
                                            stlErrorStack       * aErrorStack );

/*
 * SQL_C_NUMERIC -> SQL_CHAR
 */
ZLV_DECLARE_C_TO_SQL( Numeric, Char )
{
    /*
     * ===================================================================
     * Test                                    | SQLSTATE
     * ===================================================================
     * Number of digits <= Column byte length. | n/a
     * Number of digits > Column byte length.  | 22001
     * ===================================================================
     */

    SQL_NUMERIC_STRUCT  * sSrcValue;
    stlUInt32             sLength;
    stlChar             * sBuffer = (stlChar*)aDataValue->mValue;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mLength,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_OCTETS,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    DTL_CHECK_TYPE( DTL_TYPE_CHAR, aDataValue , aErrorStack );
        
    sSrcValue = (SQL_NUMERIC_STRUCT *)aParameterValuePtr;

    /**
     * validate input source
     */
    STL_TRY( zlvValidationCNumericWithAPD( sSrcValue,
                                           aApdRec,
                                           aErrorStack )
             == STL_SUCCESS );


    
    /**
     * source to destination
     */
    STL_TRY_THROW( zlvSqlNumericStructToDecimalString( sSrcValue,
                                                       aIpdRec->mOctetLength,
                                                       sBuffer,
                                                       & sLength,
                                                       aApdRec->mScale )
                   == STL_SUCCESS,
                   RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED );

    /**
     * validate result data (DTL_STRING_LENGTH_UNIT_OCTETS, no convert)
     */
    STL_TRY_THROW( aIpdRec->mLength >= sLength,
                   RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED );

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
 * SQL_C_NUMERIC -> SQL_VARCHAR
 */
ZLV_DECLARE_C_TO_SQL( Numeric, VarChar )
{
    /*
     * ===================================================================
     * Test                                    | SQLSTATE
     * ===================================================================
     * Number of digits <= Column byte length. | n/a
     * Number of digits > Column byte length.  | 22001
     * ===================================================================
     */

    SQL_NUMERIC_STRUCT  * sSrcValue;
    stlUInt32             sLength;
    stlChar             * sBuffer = (stlChar*)aDataValue->mValue;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mLength,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_OCTETS,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    DTL_CHECK_TYPE( DTL_TYPE_VARCHAR, aDataValue , aErrorStack );

    sSrcValue = (SQL_NUMERIC_STRUCT *)aParameterValuePtr;

    /**
     * validate input source
     */
    STL_TRY( zlvValidationCNumericWithAPD( sSrcValue,
                                           aApdRec,
                                           aErrorStack )
             == STL_SUCCESS );

    /**
     * source to destination
     */
    STL_TRY_THROW( zlvSqlNumericStructToDecimalString( sSrcValue,
                                                       aIpdRec->mOctetLength,
                                                       sBuffer,
                                                       & sLength,
                                                       aApdRec->mScale )
                   == STL_SUCCESS,
                   RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED );

    /**
     * validate result data (DTL_STRING_LENGTH_UNIT_OCTETS, no covnert)
     */
    STL_TRY_THROW( aIpdRec->mLength >= sLength,
                   RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED );

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
 * SQL_C_NUMERIC -> SQL_LONGVARCHAR
 */
ZLV_DECLARE_C_TO_SQL( Numeric, LongVarChar )
{
    /*
     * ===================================================================
     * Test                                    | SQLSTATE
     * ===================================================================
     * Number of digits <= Column byte length. | n/a
     * Number of digits > Column byte length.  | 22001
     * ===================================================================
     */

    SQL_NUMERIC_STRUCT  * sSrcValue;
    stlUInt32             sLength;
    stlChar             * sBuffer = (stlChar*)aDataValue->mValue;

    DTL_CHECK_TYPE( DTL_TYPE_LONGVARCHAR, aDataValue , aErrorStack );
    
    sSrcValue = (SQL_NUMERIC_STRUCT *)aParameterValuePtr;
    
    STL_TRY( zlvValidationCNumericWithAPD( sSrcValue,
                                           aApdRec,
                                           aErrorStack )
             == STL_SUCCESS );

    /**
     * validate input source
     */
    STL_TRY( zlvValidationCNumericWithAPD( sSrcValue,
                                           aApdRec,
                                           aErrorStack )
             == STL_SUCCESS );

    /**
     * source to destination
     */
    STL_TRY_THROW( zlvSqlNumericStructToDecimalString( sSrcValue,
                                                       aIpdRec->mOctetLength,
                                                       sBuffer,
                                                       & sLength,
                                                       aApdRec->mScale )
                   == STL_SUCCESS,
                   RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED );

    /**
     * validate result data (DTL_STRING_LENGTH_UNIT_OCTETS, no covnert)
     * C_NUMERIC -> Longvarchar 에서 정상적인 상황에서 저장공간을
     * 늘려야 하는 경우는 없다.
     */
    STL_TRY_THROW( DTL_LONGVARCHAR_MAX_PRECISION >= sLength,
                   RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED );

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
 * SQL_C_NUMERIC -> SQL_FLOAT
 */
ZLV_DECLARE_C_TO_SQL( Numeric, Float )
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

    /*
     * SQL_NUMERIC_STRUCT 자료구조는 
     * (1) mPrecision과 mScale정보 : 사용자가 설정하는 정보임.
     * (2) mSign : 부호정보  1 (양수) , 0 (음수)
     * (3) mVal : 16진수로 little endian mode로 저장됨.
     *     예) 25.212의 정보는 다음과 같이 각 멤버에 저장됨.
     *         mPrecision = 5  ( 사용자가 지정한 정보 )
     *         mScale = 3       ( 사용자가 지정한 정보 )
     *         mSign = 1 (양수)
     *         mVal : mVal[0] = 0x7C, mVal[1] = 0x62
     */    

    stlBool sSuccessWithInfo;

    SQL_NUMERIC_STRUCT  * sSrcValue;
    stlChar               sBuffer[DTL_NUMERIC_STRING_MAX_SIZE];
    stlUInt32             sLength = 0;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );
    
    sSrcValue = (SQL_NUMERIC_STRUCT *)aParameterValuePtr;

    /*
     * SQL_C_NUMERIC -> SQL_NUMERIC 으로 변환
     * little endian으로 저장된 16진수를 10진수 string으로 변환후,
     * dtlNumericType 생성.
     */

    STL_TRY( zlvValidationCNumericWithAPD( sSrcValue,
                                           aApdRec,
                                           aErrorStack )
             == STL_SUCCESS );

    /* little endian으로 저장된 16진수를 10진수 string으로 변환 */
    zlvSqlNumericStructToDecimalString( sSrcValue,
                                        DTL_NUMERIC_STRING_MAX_SIZE,
                                        sBuffer,
                                        & sLength,
                                        aApdRec->mScale );

    /* dtlNumericType 생성 */
    STL_TRY( dtlFloatSetValueFromString( (stlChar *)sBuffer,
                                         sLength,
                                         aIpdRec->mPrecision,
                                         DTL_SCALE_NA,
                                         DTL_STRING_LENGTH_UNIT_NA,
                                         DTL_INTERVAL_INDICATOR_NA,
                                         aIpdRec->mOctetLength,
                                         aDataValue,
                                         & sSuccessWithInfo,
                                         ZLS_STMT_DT_VECTOR(aStmt),
                                         aStmt,
                                         ZLS_STMT_NLS_DT_VECTOR(aStmt),
                                         aStmt,
                                         aErrorStack )
             == STL_SUCCESS );    

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/*
 * SQL_C_NUMERIC -> SQL_NUMERIC
 */
ZLV_DECLARE_C_TO_SQL( Numeric, Numeric )
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

    /*
     * SQL_NUMERIC_STRUCT 자료구조는 
     * (1) mPrecision과 mScale정보 : 사용자가 설정하는 정보임.
     * (2) mSign : 부호정보  1 (양수) , 0 (음수)
     * (3) mVal : 16진수로 little endian mode로 저장됨.
     *     예) 25.212의 정보는 다음과 같이 각 멤버에 저장됨.
     *         mPrecision = 5  ( 사용자가 지정한 정보 )
     *         mScale = 3       ( 사용자가 지정한 정보 )
     *         mSign = 1 (양수)
     *         mVal : mVal[0] = 0x7C, mVal[1] = 0x62
     */    

    stlBool sSuccessWithInfo;

    SQL_NUMERIC_STRUCT  * sSrcValue;
    stlChar               sBuffer[DTL_NUMERIC_STRING_MAX_SIZE];
    stlUInt32             sLength = 0;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mPrecision,
                                      aIpdRec->mScale,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );
    
    sSrcValue = (SQL_NUMERIC_STRUCT *)aParameterValuePtr;

    /*
     * SQL_C_NUMERIC -> SQL_NUMERIC 으로 변환
     * little endian으로 저장된 16진수를 10진수 string으로 변환후,
     * dtlNumericType 생성.
     */

    STL_TRY( zlvValidationCNumericWithAPD( sSrcValue,
                                           aApdRec,
                                           aErrorStack )
             == STL_SUCCESS );

    /* little endian으로 저장된 16진수를 10진수 string으로 변환 */
    STL_TRY( zlvSqlNumericStructToDecimalString( sSrcValue,
                                                 DTL_NUMERIC_STRING_MAX_SIZE,
                                                 sBuffer,
                                                 & sLength,
                                                 aApdRec->mScale )
             == STL_SUCCESS );
    

    /* dtlNumericType 생성 */
    STL_TRY( dtlNumericSetValueFromString( (stlChar *)sBuffer,
                                           sLength,
                                           aIpdRec->mPrecision,
                                           aIpdRec->mScale,
                                           DTL_STRING_LENGTH_UNIT_NA,
                                           DTL_INTERVAL_INDICATOR_NA,
                                           aIpdRec->mOctetLength,
                                           aDataValue,
                                           & sSuccessWithInfo,
                                           ZLS_STMT_DT_VECTOR(aStmt),
                                           aStmt,
                                           ZLS_STMT_NLS_DT_VECTOR(aStmt),
                                           aStmt,
                                           aErrorStack )
             == STL_SUCCESS );    

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/*
 * SQL_C_NUMERIC -> SQL_TINYINT
 */
ZLV_DECLARE_C_TO_SQL( Numeric, TinyInt )
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

    SQL_NUMERIC_STRUCT * sParameter;
    stlUInt64            sUInt64Value;
    stlUInt64            sAvailableMaxValue = STL_INT64_MAX;
    stlInt64             sInt64Value;
    stlUInt128           sUInt128Value;

    DTL_CHECK_TYPE( DTL_TYPE_NATIVE_SMALLINT, aDataValue , aErrorStack );

    sParameter = (SQL_NUMERIC_STRUCT*)aParameterValuePtr;

    STL_TRY( zlvValidationCNumericWithAPD( sParameter,
                                           aApdRec,
                                           aErrorStack )
             == STL_SUCCESS );

    STL_TRY( zlvGetUInt128FromCNumeric( sParameter,
                                        &sUInt128Value,
                                        aApdRec->mScale,
                                        aErrorStack )
             == STL_SUCCESS );

    if( STL_IS_UINT128_ZERO( sUInt128Value ) == STL_TRUE )
    {
        sInt64Value = 0;
    }
    else
    {
        STL_TRY_THROW( aApdRec->mScale >= ZLV_C_TO_SQL_SMALLINT_MIN_SCALE,
                       RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

        STL_TRY_THROW( stlGetToUInt64FromUInt128( sUInt128Value, &sUInt64Value ) == STL_TRUE,
                       RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

        if( sParameter->sign == 0 )
        {
            STL_TRY_THROW( sUInt64Value <= sAvailableMaxValue+1,
                           RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

            sInt64Value = -(stlInt64)sUInt64Value;    
            STL_TRY_THROW( sInt64Value >= STL_INT8_MIN, RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
        }
        else
        {
            STL_TRY_THROW( sUInt64Value <= sAvailableMaxValue,
                           RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
            
            sInt64Value = (stlInt64)sUInt64Value;    
            STL_TRY_THROW( sUInt64Value <= STL_INT8_MAX, RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
        }
    }

    /*
     * DataType에 TinyInt 타입이 없으므로 SmallInt로 변환한다.
     */
    *((dtlSmallIntType *)aDataValue->mValue) = (dtlSmallIntType)sInt64Value;
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
 * SQL_C_NUMERIC -> SQL_SMALLINT
 */
ZLV_DECLARE_C_TO_SQL( Numeric, SmallInt )
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

    SQL_NUMERIC_STRUCT * sParameter;
    stlUInt64            sUInt64Value;
    stlUInt64            sAvailableMaxValue = STL_INT64_MAX;
    stlInt64             sInt64Value;
    stlUInt128           sUInt128Value;

    DTL_CHECK_TYPE( DTL_TYPE_NATIVE_SMALLINT, aDataValue , aErrorStack );
    
    sParameter = (SQL_NUMERIC_STRUCT*)aParameterValuePtr;

    STL_TRY( zlvValidationCNumericWithAPD( sParameter,
                                           aApdRec,
                                           aErrorStack )
             == STL_SUCCESS );

    STL_TRY( zlvGetUInt128FromCNumeric( sParameter,
                                        &sUInt128Value,
                                        aApdRec->mScale,
                                        aErrorStack )
             == STL_SUCCESS );

    if( STL_IS_UINT128_ZERO( sUInt128Value ) == STL_TRUE )
    {
        sInt64Value = 0;
    }
    else
    {
        STL_TRY_THROW( aApdRec->mScale >= ZLV_C_TO_SQL_SMALLINT_MIN_SCALE,
                       RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

        STL_TRY_THROW( stlGetToUInt64FromUInt128( sUInt128Value, &sUInt64Value ) == STL_TRUE,
                       RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );   

        if( sParameter->sign == 0 )
        {
            STL_TRY_THROW( sUInt64Value <= sAvailableMaxValue+1,
                           RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
            
            sInt64Value = -(stlInt64)sUInt64Value;    
            STL_TRY_THROW( sInt64Value >= STL_INT16_MIN, RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
        }
        else
        {
            STL_TRY_THROW( sUInt64Value <= sAvailableMaxValue,
                           RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
            
            sInt64Value = (stlInt64)sUInt64Value;    
            STL_TRY_THROW( sUInt64Value <= STL_INT16_MAX, RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
        }
    }

    *((dtlSmallIntType *)aDataValue->mValue) = (dtlSmallIntType)sInt64Value;
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
 * SQL_C_NUMERIC -> SQL_INTEGER
 */
ZLV_DECLARE_C_TO_SQL( Numeric, Integer )
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

    SQL_NUMERIC_STRUCT * sParameter;
    stlUInt64            sUInt64Value;
    stlUInt64            sAvailableMaxValue = STL_INT64_MAX;
    stlInt64             sInt64Value;
    stlUInt128           sUInt128Value;

    DTL_CHECK_TYPE( DTL_TYPE_NATIVE_INTEGER, aDataValue , aErrorStack );
    
    sParameter = (SQL_NUMERIC_STRUCT*)aParameterValuePtr;

    STL_TRY( zlvValidationCNumericWithAPD( sParameter,
                                           aApdRec,
                                           aErrorStack )
             == STL_SUCCESS );

    STL_TRY( zlvGetUInt128FromCNumeric( sParameter,
                                        &sUInt128Value,
                                        aApdRec->mScale,
                                        aErrorStack )
             == STL_SUCCESS );

    if( STL_IS_UINT128_ZERO( sUInt128Value ) == STL_TRUE )
    {
        sInt64Value = 0;
    }
    else
    {
        STL_TRY_THROW( aApdRec->mScale >= ZLV_C_TO_SQL_INTEGER_MIN_SCALE,
                       RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

        STL_TRY_THROW( stlGetToUInt64FromUInt128( sUInt128Value, &sUInt64Value ) == STL_TRUE,
                       RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );    

        if( sParameter->sign == 0 )
        {
            STL_TRY_THROW( sUInt64Value <= sAvailableMaxValue+1,
                           RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
            
            sInt64Value = -(stlInt64)sUInt64Value;
            STL_TRY_THROW( sInt64Value >= STL_INT32_MIN, RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
        }
        else
        {
            STL_TRY_THROW( sUInt64Value <= sAvailableMaxValue,
                           RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
            
            sInt64Value = (stlInt64)sUInt64Value;    
            STL_TRY_THROW( sUInt64Value <= STL_INT32_MAX, RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
        }
    }

    *((dtlIntegerType *)aDataValue->mValue) = (dtlIntegerType)sInt64Value;
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
 * SQL_C_NUMERIC -> SQL_BIGINT
 */
ZLV_DECLARE_C_TO_SQL( Numeric, BigInt )
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

    SQL_NUMERIC_STRUCT * sParameter;
    stlUInt64            sUInt64Value;
    stlUInt64            sAvailableMaxValue = STL_INT64_MAX;
    stlInt64             sInt64Value;
    stlUInt128           sUInt128Value;

    DTL_CHECK_TYPE( DTL_TYPE_NATIVE_BIGINT, aDataValue , aErrorStack );
    
    sParameter = (SQL_NUMERIC_STRUCT*)aParameterValuePtr;

    STL_TRY( zlvValidationCNumericWithAPD( sParameter,
                                           aApdRec,
                                           aErrorStack )
             == STL_SUCCESS );


    STL_TRY( zlvGetUInt128FromCNumeric( sParameter,
                                        &sUInt128Value,
                                        aApdRec->mScale,
                                        aErrorStack )
             == STL_SUCCESS );

    if( STL_IS_UINT128_ZERO( sUInt128Value ) == STL_TRUE )
    {
        sInt64Value = 0;
    }
    else
    {
        STL_TRY_THROW( aApdRec->mScale >= ZLV_C_TO_SQL_BIGINT_MIN_SCALE,
                       RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

        STL_TRY_THROW( stlGetToUInt64FromUInt128( sUInt128Value, &sUInt64Value ) == STL_TRUE,
                       RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

        if( sParameter->sign == 0 )
        {
            STL_TRY_THROW( sUInt64Value <= sAvailableMaxValue+1,
                           RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
            
            sInt64Value = -(stlInt64)sUInt64Value;    
            STL_TRY_THROW( sInt64Value >= STL_INT64_MIN, RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
        }
        else
        {
            STL_TRY_THROW( sUInt64Value <= sAvailableMaxValue,
                           RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
            
            sInt64Value = (stlInt64)sUInt64Value;    
            STL_TRY_THROW( sInt64Value <= STL_INT64_MAX, RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
        }
    }

    *((dtlBigIntType *)aDataValue->mValue) = (dtlBigIntType)sInt64Value;
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
 * SQL_C_NUMERIC -> SQL_REAL
 */
ZLV_DECLARE_C_TO_SQL( Numeric, Real )
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

    SQL_NUMERIC_STRUCT * sParameter;
    stlFloat64           sFloat64Value;
    stlUInt128           sUInt128Value;
    stlBool              sSuccessWithInfo;
    stlFloat64           sPow;
    
    sParameter = (SQL_NUMERIC_STRUCT*)aParameterValuePtr;

    STL_TRY( zlvValidationCNumericWithAPD( sParameter,
                                           aApdRec,
                                           aErrorStack )
             == STL_SUCCESS );

    STL_TRY( zlvGetUInt128FromCNumeric( sParameter,
                                        &sUInt128Value,
                                        0,
                                        aErrorStack )
             == STL_SUCCESS );

    stlGetToDoubleFromUInt128( sUInt128Value, &sFloat64Value );

    if( sParameter->sign == 0 )
    {
        sFloat64Value = -sFloat64Value;
    }
    else
    {
        /*
         * Do Nothing
         */
    }

    if( aApdRec->mScale > 0 )
    {
        STL_TRY_THROW( stlGet10Pow( aApdRec->mScale, &sPow, aErrorStack ) == STL_TRUE,
                       RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
        sFloat64Value = sFloat64Value / sPow;
    }
    else
    {
        STL_TRY_THROW( stlGet10Pow( -aApdRec->mScale, &sPow, aErrorStack ) == STL_TRUE,
                       RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
        sFloat64Value = sFloat64Value * sPow;
    }

    STL_TRY_THROW( stlIsinfinite( sFloat64Value ) == STL_IS_INFINITE_FALSE,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    STL_TRY( dtlRealSetValueFromReal( sFloat64Value,
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
 * SQL_C_NUMERIC -> SQL_DOUBLE
 */
ZLV_DECLARE_C_TO_SQL( Numeric, Double )
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

    SQL_NUMERIC_STRUCT * sParameter;
    stlFloat64           sFloat64Value;
    stlUInt128           sUInt128Value;
    stlBool              sSuccessWithInfo;
    stlFloat64           sPow;
    
    sParameter = (SQL_NUMERIC_STRUCT*)aParameterValuePtr;

    STL_TRY( zlvValidationCNumericWithAPD( sParameter,
                                           aApdRec,
                                           aErrorStack )
             == STL_SUCCESS );

    STL_TRY( zlvGetUInt128FromCNumeric( sParameter,
                                        &sUInt128Value,
                                        0,
                                        aErrorStack )
             == STL_SUCCESS );

    stlGetToDoubleFromUInt128( sUInt128Value, &sFloat64Value );

    if( sParameter->sign == 0 )
    {
        sFloat64Value = -sFloat64Value;
    }
    else
    {
        /*
         * Do Nothing
         */
    }

    if( aApdRec->mScale > 0 )
    {
        STL_TRY_THROW( stlGet10Pow( aApdRec->mScale, &sPow, aErrorStack ) == STL_TRUE,
                       RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
        sFloat64Value = sFloat64Value / sPow;
    }
    else
    {
        STL_TRY_THROW( stlGet10Pow( -aApdRec->mScale, &sPow, aErrorStack ) == STL_TRUE,
                       RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
        sFloat64Value = sFloat64Value * sPow;
    }

    STL_TRY_THROW( stlIsinfinite( sFloat64Value ) == STL_IS_INFINITE_FALSE,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    STL_TRY( dtlDoubleSetValueFromReal( sFloat64Value,
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
 * SQL_C_NUMERIC -> SQL_BIT
 */
ZLV_DECLARE_C_TO_SQL( Numeric, Bit )
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

    SQL_NUMERIC_STRUCT * sParameter;
    stlFloat64           sFloat64Value;
    stlUInt128           sUInt128Value;
    stlBool              sSuccessWithInfo;
    stlFloat64           sPow;

    sParameter = (SQL_NUMERIC_STRUCT*)aParameterValuePtr;

    STL_TRY( zlvValidationCNumericWithAPD( sParameter,
                                           aApdRec,
                                           aErrorStack )
             == STL_SUCCESS );

    STL_TRY( zlvGetUInt128FromCNumeric( sParameter,
                                        &sUInt128Value,
                                        0,
                                        aErrorStack )
             == STL_SUCCESS );

    stlGetToDoubleFromUInt128( sUInt128Value, &sFloat64Value );

    if( aApdRec->mScale > 0 )
    {
        STL_TRY_THROW( stlGet10Pow( aApdRec->mScale, &sPow, aErrorStack ) == STL_TRUE,
                       RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
        sFloat64Value = sFloat64Value / sPow;
    }
    else
    {
        STL_TRY_THROW( stlGet10Pow( -aApdRec->mScale, &sPow, aErrorStack ) == STL_TRUE,
                       RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
        sFloat64Value = sFloat64Value * sPow;
    }

    if( sParameter->sign == 0 )
    {
        sFloat64Value = -sFloat64Value;
    }
    else
    {
        /*
         * Do Nothing
         */
    }

    STL_TRY_THROW( ( sFloat64Value >= 0 ) &&
                   ( sFloat64Value < 2 ),
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    STL_TRY_THROW( ( sFloat64Value == 0 ) ||
                   ( sFloat64Value == 1 ),
                   RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED );

    STL_TRY( dtlBooleanSetValueFromInteger( (stlInt64)sFloat64Value,
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
 * SQL_C_NUMERIC -> SQL_BOOLEAN
 */
ZLV_DECLARE_C_TO_SQL( Numeric, Boolean )
{
    return zlvCNumericToSqlBit( aStmt,
                                aParameterValuePtr,
                                aIndicator,
                                aApdRec,
                                aIpdRec,
                                aDataValue,
                                aSuccessWithInfo,
                                aErrorStack );

}

/*
 * SQL_C_NUMERIC -> SQL_INTERVAL_YEAR
 */
ZLV_DECLARE_C_TO_SQL( Numeric, IntervalYear )
{
    /*
     * ================================
     * Test                | SQLSTATE
     * ================================
     * Data not truncated. | n/a 
     * Data truncated.     | 22015
     * ================================
     */

    SQL_NUMERIC_STRUCT * sParameter;
    stlUInt64            sUInt64Value;
    stlInt64             sInt64Value;
    stlUInt128           sUInt128Value;
    stlUInt128           sScaleValue;
    stlUInt128           sQuotient;
    stlUInt128           sRemainder;
    stlBool              sSuccessWithInfo;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_YEAR,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_NUMERIC_STRUCT*)aParameterValuePtr;

    STL_TRY( zlvValidationCNumericWithAPD( sParameter,
                                           aApdRec,
                                           aErrorStack )
             == STL_SUCCESS );

    STL_TRY( zlvGetUInt128FromCNumeric( sParameter,
                                        &sUInt128Value,
                                        0,
                                        aErrorStack )
             == STL_SUCCESS );

    if( STL_IS_UINT128_ZERO( sUInt128Value ) == STL_TRUE )
    {
        sInt64Value = 0;
    }
    else
    {
        if( aApdRec->mScale > 0 )
        {
            STL_TRY_THROW( (ZLV_C_TO_SQL_INTERVAL_MAX_SCALE >= aApdRec->mScale ),
                           RAMP_ERR_INTERVAL_FIELD_OVERFLOW );
        
            sScaleValue = gPreDefinedPowUInt128[ aApdRec->mScale ];

            STL_TRY_THROW( stlRemUInt128( sUInt128Value,
                                          sScaleValue,
                                          &sRemainder ) == STL_TRUE,
                           RAMP_ERR_INTERVAL_FIELD_OVERFLOW );

            STL_TRY_THROW( STL_IS_UINT128_ZERO( sRemainder ) == STL_TRUE,
                           RAMP_ERR_INTERVAL_FIELD_OVERFLOW );

            STL_TRY_THROW( stlDivUInt128( sUInt128Value, sScaleValue, &sQuotient ) == STL_TRUE,
                           RAMP_ERR_INTERVAL_FIELD_OVERFLOW );
        }
        else
        {
            STL_TRY_THROW( aApdRec->mScale >= ZLV_C_TO_SQL_INTERVAL_MIN_SCALE,
                           RAMP_ERR_INTERVAL_FIELD_OVERFLOW );
        
            sScaleValue = gPreDefinedPowUInt128[ -aApdRec->mScale ];

            STL_TRY_THROW( stlMulUInt128( sUInt128Value, sScaleValue, &sQuotient ) == STL_TRUE,
                           RAMP_ERR_INTERVAL_FIELD_OVERFLOW );
        }

        STL_TRY_THROW( stlGetToUInt64FromUInt128( sQuotient, &sUInt64Value ) == STL_TRUE,
                       RAMP_ERR_INTERVAL_FIELD_OVERFLOW );

        STL_TRY_THROW( sUInt64Value <= STL_INT64_MAX, RAMP_ERR_INTERVAL_FIELD_OVERFLOW );

        sInt64Value = (stlInt64)sUInt64Value;    

        if( sParameter->sign == 0 )
        {
            sInt64Value = -sInt64Value;
        }
        else
        {
            /*
             * Do Nothing
             */
        }
    }

    STL_TRY( dtlIntervalYearToMonthSetValueFromInteger( sInt64Value,
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
 * SQL_C_NUMERIC -> SQL_INTERVAL_MONTH
 */
ZLV_DECLARE_C_TO_SQL( Numeric, IntervalMonth )
{
    /*
     * ================================
     * Test                | SQLSTATE
     * ================================
     * Data not truncated. | n/a 
     * Data truncated.     | 22015
     * ================================
     */

    SQL_NUMERIC_STRUCT * sParameter;
    stlUInt64            sUInt64Value;
    stlInt64             sInt64Value;
    stlUInt128           sUInt128Value;
    stlUInt128           sScaleValue;
    stlUInt128           sQuotient;
    stlUInt128           sRemainder;
    stlBool              sSuccessWithInfo;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_MONTH,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_NUMERIC_STRUCT*)aParameterValuePtr;

    STL_TRY( zlvValidationCNumericWithAPD( sParameter,
                                           aApdRec,
                                           aErrorStack )
             == STL_SUCCESS );

    STL_TRY( zlvGetUInt128FromCNumeric( sParameter,
                                        &sUInt128Value,
                                        0,
                                        aErrorStack )
             == STL_SUCCESS );

    if( STL_IS_UINT128_ZERO( sUInt128Value ) == STL_TRUE )
    {
        sInt64Value = 0;
    }
    else
    {
        if( aApdRec->mScale > 0 )
        {
            STL_TRY_THROW( (ZLV_C_TO_SQL_INTERVAL_MAX_SCALE >= aApdRec->mScale ),
                           RAMP_ERR_INTERVAL_FIELD_OVERFLOW );
        
            sScaleValue = gPreDefinedPowUInt128[ aApdRec->mScale ];

            STL_TRY_THROW( stlRemUInt128( sUInt128Value,
                                          sScaleValue,
                                          &sRemainder ) == STL_TRUE,
                           RAMP_ERR_INTERVAL_FIELD_OVERFLOW );

            STL_TRY_THROW( STL_IS_UINT128_ZERO( sRemainder ) == STL_TRUE,
                           RAMP_ERR_INTERVAL_FIELD_OVERFLOW );
                       
            STL_TRY_THROW( stlDivUInt128( sUInt128Value, sScaleValue, &sQuotient ) == STL_TRUE,
                           RAMP_ERR_INTERVAL_FIELD_OVERFLOW );
        }
        else
        {
            STL_TRY_THROW( aApdRec->mScale >= ZLV_C_TO_SQL_INTERVAL_MIN_SCALE,
                           RAMP_ERR_INTERVAL_FIELD_OVERFLOW );
        
            sScaleValue = gPreDefinedPowUInt128[ -aApdRec->mScale ];

            STL_TRY_THROW( stlMulUInt128( sUInt128Value, sScaleValue, &sQuotient ) == STL_TRUE,
                           RAMP_ERR_INTERVAL_FIELD_OVERFLOW );
        }

        STL_TRY_THROW( stlGetToUInt64FromUInt128( sQuotient, &sUInt64Value ) == STL_TRUE,
                       RAMP_ERR_INTERVAL_FIELD_OVERFLOW );

        STL_TRY_THROW( sUInt64Value <= STL_INT64_MAX, RAMP_ERR_INTERVAL_FIELD_OVERFLOW );

        sInt64Value = (stlInt64)sUInt64Value;    

        if( sParameter->sign == 0 )
        {
            sInt64Value = -sInt64Value;
        }
        else
        {
            /*
             * Do Nothing
             */
        }
    }

    STL_TRY( dtlIntervalYearToMonthSetValueFromInteger( sInt64Value,
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
 * SQL_C_NUMERIC -> SQL_INTERVAL_DAY
 */
ZLV_DECLARE_C_TO_SQL( Numeric, IntervalDay )
{
    /*
     * ================================
     * Test                | SQLSTATE
     * ================================
     * Data not truncated. | n/a 
     * Data truncated.     | 22015
     * ================================
     */

    SQL_NUMERIC_STRUCT * sParameter;
    stlUInt64            sUInt64Value;
    stlInt64             sInt64Value;
    stlUInt128           sUInt128Value;
    stlUInt128           sScaleValue;
    stlUInt128           sQuotient;
    stlUInt128           sRemainder;
    stlBool              sSuccessWithInfo;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_DAY,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_NUMERIC_STRUCT*)aParameterValuePtr;

    STL_TRY( zlvValidationCNumericWithAPD( sParameter,
                                           aApdRec,
                                           aErrorStack )
             == STL_SUCCESS );

    STL_TRY( zlvGetUInt128FromCNumeric( sParameter,
                                        &sUInt128Value,
                                        0,
                                        aErrorStack )
             == STL_SUCCESS );

    if( STL_IS_UINT128_ZERO( sUInt128Value ) == STL_TRUE )
    {
        sInt64Value = 0;
    }
    else
    {
        if( aApdRec->mScale > 0 )
        {
            STL_TRY_THROW( (ZLV_C_TO_SQL_INTERVAL_MAX_SCALE >= aApdRec->mScale ),
                           RAMP_ERR_INTERVAL_FIELD_OVERFLOW );
        
            sScaleValue = gPreDefinedPowUInt128[ aApdRec->mScale ];

            STL_TRY_THROW( stlRemUInt128( sUInt128Value,
                                          sScaleValue,
                                          &sRemainder ) == STL_TRUE,
                           RAMP_ERR_INTERVAL_FIELD_OVERFLOW );

            STL_TRY_THROW( STL_IS_UINT128_ZERO( sRemainder ) == STL_TRUE,
                           RAMP_ERR_INTERVAL_FIELD_OVERFLOW );
                       
            STL_TRY_THROW( stlDivUInt128( sUInt128Value, sScaleValue, &sQuotient ) == STL_TRUE,
                           RAMP_ERR_INTERVAL_FIELD_OVERFLOW );
        }
        else
        {
            STL_TRY_THROW( aApdRec->mScale >= ZLV_C_TO_SQL_INTERVAL_MIN_SCALE,
                           RAMP_ERR_INTERVAL_FIELD_OVERFLOW );
        
            sScaleValue = gPreDefinedPowUInt128[ -aApdRec->mScale ];

            STL_TRY_THROW( stlMulUInt128( sUInt128Value, sScaleValue, &sQuotient ) == STL_TRUE,
                           RAMP_ERR_INTERVAL_FIELD_OVERFLOW );
        }

        STL_TRY_THROW( stlGetToUInt64FromUInt128( sQuotient, &sUInt64Value ) == STL_TRUE,
                       RAMP_ERR_INTERVAL_FIELD_OVERFLOW );

        STL_TRY_THROW( sUInt64Value <= STL_INT64_MAX, RAMP_ERR_INTERVAL_FIELD_OVERFLOW );

        sInt64Value = (stlInt64)sUInt64Value;    

        if( sParameter->sign == 0 )
        {
            sInt64Value = -sInt64Value;
        }
        else
        {
            /*
             * Do Nothing
             */
        }
    }

    STL_TRY( dtlIntervalDayToSecondSetValueFromInteger( sInt64Value,
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
 * SQL_C_NUMERIC -> SQL_INTERVAL_HOUR
 */
ZLV_DECLARE_C_TO_SQL( Numeric, IntervalHour )
{
    /*
     * ================================
     * Test                | SQLSTATE
     * ================================
     * Data not truncated. | n/a 
     * Data truncated.     | 22015
     * ================================
     */

    SQL_NUMERIC_STRUCT * sParameter;
    stlUInt64            sUInt64Value;
    stlInt64             sInt64Value;
    stlUInt128           sUInt128Value;
    stlUInt128           sScaleValue;
    stlUInt128           sQuotient;
    stlUInt128           sRemainder;
    stlBool              sSuccessWithInfo;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_HOUR,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_NUMERIC_STRUCT*)aParameterValuePtr;

    STL_TRY( zlvValidationCNumericWithAPD( sParameter,
                                           aApdRec,
                                           aErrorStack )
             == STL_SUCCESS );

    STL_TRY( zlvGetUInt128FromCNumeric( sParameter,
                                        &sUInt128Value,
                                        0,
                                        aErrorStack )
             == STL_SUCCESS );

    if( STL_IS_UINT128_ZERO( sUInt128Value ) == STL_TRUE )
    {
        sInt64Value = 0;
    }
    else
    {
        if( aApdRec->mScale > 0 )
        {
            STL_TRY_THROW( (ZLV_C_TO_SQL_INTERVAL_MAX_SCALE >= aApdRec->mScale ),
                           RAMP_ERR_INTERVAL_FIELD_OVERFLOW );
        
            sScaleValue = gPreDefinedPowUInt128[ aApdRec->mScale ];

            STL_TRY_THROW( stlRemUInt128( sUInt128Value,
                                          sScaleValue,
                                          &sRemainder ) == STL_TRUE,
                           RAMP_ERR_INTERVAL_FIELD_OVERFLOW );

            STL_TRY_THROW( STL_IS_UINT128_ZERO( sRemainder ) == STL_TRUE,
                           RAMP_ERR_INTERVAL_FIELD_OVERFLOW );
                       
            STL_TRY_THROW( stlDivUInt128( sUInt128Value, sScaleValue, &sQuotient ) == STL_TRUE,
                           RAMP_ERR_INTERVAL_FIELD_OVERFLOW );
        }
        else
        {
            STL_TRY_THROW( aApdRec->mScale >= ZLV_C_TO_SQL_INTERVAL_MIN_SCALE,
                           RAMP_ERR_INTERVAL_FIELD_OVERFLOW );
        
            sScaleValue = gPreDefinedPowUInt128[ -aApdRec->mScale ];
        
            STL_TRY_THROW( stlMulUInt128( sUInt128Value, sScaleValue, &sQuotient ) == STL_TRUE,
                           RAMP_ERR_INTERVAL_FIELD_OVERFLOW );
        }

        STL_TRY_THROW( stlGetToUInt64FromUInt128( sQuotient, &sUInt64Value ) == STL_TRUE,
                       RAMP_ERR_INTERVAL_FIELD_OVERFLOW );

        STL_TRY_THROW( sUInt64Value <= STL_INT64_MAX, RAMP_ERR_INTERVAL_FIELD_OVERFLOW );

        sInt64Value = (stlInt64)sUInt64Value;    

        if( sParameter->sign == 0 )
        {
            sInt64Value = -sInt64Value;
        }
        else
        {
            /*
             * Do Nothing
             */
        }
    }

    STL_TRY( dtlIntervalDayToSecondSetValueFromInteger( sInt64Value,
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
 * SQL_C_NUMERIC -> SQL_INTERVAL_MINUTE
 */
ZLV_DECLARE_C_TO_SQL( Numeric, IntervalMinute )
{
    /*
     * ================================
     * Test                | SQLSTATE
     * ================================
     * Data not truncated. | n/a 
     * Data truncated.     | 22015
     * ================================
     */

    SQL_NUMERIC_STRUCT * sParameter;
    stlUInt64            sUInt64Value;
    stlInt64             sInt64Value;
    stlUInt128           sUInt128Value;
    stlUInt128           sScaleValue;
    stlUInt128           sQuotient;
    stlUInt128           sRemainder;
    stlBool              sSuccessWithInfo;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_MINUTE,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_NUMERIC_STRUCT*)aParameterValuePtr;

    STL_TRY( zlvValidationCNumericWithAPD( sParameter,
                                           aApdRec,
                                           aErrorStack )
             == STL_SUCCESS );

    STL_TRY( zlvGetUInt128FromCNumeric( sParameter,
                                        &sUInt128Value,
                                        0,
                                        aErrorStack )
             == STL_SUCCESS );

    if( STL_IS_UINT128_ZERO( sUInt128Value ) == STL_TRUE )
    {
        sInt64Value = 0;
    }
    else
    {
        if( aApdRec->mScale > 0 )
        {
            STL_TRY_THROW( (ZLV_C_TO_SQL_INTERVAL_MAX_SCALE >= aApdRec->mScale ),
                           RAMP_ERR_INTERVAL_FIELD_OVERFLOW );
        
            sScaleValue = gPreDefinedPowUInt128[ aApdRec->mScale ];

            STL_TRY_THROW( stlRemUInt128( sUInt128Value,
                                          sScaleValue,
                                          &sRemainder ) == STL_TRUE,
                           RAMP_ERR_INTERVAL_FIELD_OVERFLOW );

            STL_TRY_THROW( STL_IS_UINT128_ZERO( sRemainder ) == STL_TRUE,
                           RAMP_ERR_INTERVAL_FIELD_OVERFLOW );
                       
            STL_TRY_THROW( stlDivUInt128( sUInt128Value, sScaleValue, &sQuotient ) == STL_TRUE,
                           RAMP_ERR_INTERVAL_FIELD_OVERFLOW );
        }
        else
        {
            STL_TRY_THROW( aApdRec->mScale >= ZLV_C_TO_SQL_INTERVAL_MIN_SCALE,
                           RAMP_ERR_INTERVAL_FIELD_OVERFLOW );
        
            sScaleValue = gPreDefinedPowUInt128[ -aApdRec->mScale ];

            STL_TRY_THROW( stlMulUInt128( sUInt128Value, sScaleValue, &sQuotient ) == STL_TRUE,
                           RAMP_ERR_INTERVAL_FIELD_OVERFLOW );
        }

        STL_TRY_THROW( stlGetToUInt64FromUInt128( sQuotient, &sUInt64Value ) == STL_TRUE,
                       RAMP_ERR_INTERVAL_FIELD_OVERFLOW );

        STL_TRY_THROW( sUInt64Value <= STL_INT64_MAX, RAMP_ERR_INTERVAL_FIELD_OVERFLOW );

        sInt64Value = (stlInt64)sUInt64Value;    

        if( sParameter->sign == 0 )
        {
            sInt64Value = -sInt64Value;
        }
        else
        {
            /*
             * Do Nothing
             */
        }
    }

    STL_TRY( dtlIntervalDayToSecondSetValueFromInteger( sInt64Value,
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
 * SQL_C_NUMERIC -> SQL_INTERVAL_SECOND
 */
ZLV_DECLARE_C_TO_SQL( Numeric, IntervalSecond )
{
    /*
     * ================================
     * Test                | SQLSTATE
     * ================================
     * Data not truncated. | n/a 
     * Data truncated.     | 22015
     * ================================
     */

    SQL_NUMERIC_STRUCT * sParameter;
    stlUInt64            sSecondValue;
    stlUInt64            sFractionValue;
    stlInt32             sCmpValue;
    stlInt32             sZeroLen = 0;
    stlInt32             sTmpFrac;
    stlUInt128           sUInt128Value;
    stlUInt128           sScaleValue;
    stlUInt128           sQuotient;
    stlUInt128           sRemainder;
    dtlTimeOffset        sTime;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      aIpdRec->mPrecision,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_SECOND,
                                      aErrorStack )
             == STL_SUCCESS );

    DTL_CHECK_TYPE( DTL_TYPE_INTERVAL_DAY_TO_SECOND, aDataValue, aErrorStack );

    sParameter = (SQL_NUMERIC_STRUCT*)aParameterValuePtr;

    STL_TRY( zlvValidationCNumericWithAPD( sParameter,
                                           aApdRec,
                                           aErrorStack )
             == STL_SUCCESS );

    STL_TRY( zlvGetUInt128FromCNumeric( sParameter,
                                        &sUInt128Value,
                                        0,
                                        aErrorStack )
             == STL_SUCCESS );

    if( STL_IS_UINT128_ZERO( sUInt128Value ) == STL_TRUE )
    {
        sSecondValue   = 0;
        sFractionValue = 0;
    }
    else
    {
        if( aApdRec->mScale > 0 )
        {
            STL_TRY_THROW( (ZLV_C_TO_SQL_INTERVAL_MAX_SCALE >= aApdRec->mScale ),
                           RAMP_ERR_INTERVAL_FIELD_OVERFLOW );
        
            sScaleValue = gPreDefinedPowUInt128[ aApdRec->mScale ];

            STL_TRY_THROW( stlDivUInt128( sUInt128Value,
                                          sScaleValue,
                                          &sQuotient ) == STL_TRUE,
                           RAMP_ERR_INTERVAL_FIELD_OVERFLOW );
        
            STL_TRY_THROW( stlRemUInt128( sUInt128Value,
                                          sScaleValue,
                                          &sRemainder ) == STL_TRUE,
                           RAMP_ERR_INTERVAL_FIELD_OVERFLOW );

            if( ( DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION - aApdRec->mScale ) > 0 )
            {
                sScaleValue =
                    gPreDefinedPowUInt128[ DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION - aApdRec->mScale ];
            
                STL_TRY_THROW( stlMulUInt128( sRemainder,
                                              sScaleValue,
                                              &sRemainder ) == STL_TRUE,
                               RAMP_ERR_INTERVAL_FIELD_OVERFLOW );
            }
            else
            {
                sScaleValue =
                    gPreDefinedPowUInt128[ aApdRec->mScale - DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION ];

                STL_TRY_THROW( stlDivUInt128( sRemainder,
                                              sScaleValue,
                                              &sRemainder ) == STL_TRUE,
                               RAMP_ERR_INTERVAL_FIELD_OVERFLOW );
            }
        }
        else
        {
            STL_TRY_THROW( aApdRec->mScale >= ZLV_C_TO_SQL_INTERVAL_MIN_SCALE,
                           RAMP_ERR_INTERVAL_FIELD_OVERFLOW );
        
            sScaleValue = gPreDefinedPowUInt128[ -aApdRec->mScale ];

            STL_TRY_THROW( stlMulUInt128( sUInt128Value, sScaleValue, &sQuotient ) == STL_TRUE,
                           RAMP_ERR_INTERVAL_FIELD_OVERFLOW );

            stlSetToUInt128FromUInt64( 0,
                                       &sRemainder );

        }

        STL_TRY_THROW( stlGetToUInt64FromUInt128( sQuotient, &sSecondValue ) == STL_TRUE,
                       RAMP_ERR_INTERVAL_FIELD_OVERFLOW );
        STL_TRY_THROW( sSecondValue <= STL_UINT32_MAX, RAMP_ERR_INTERVAL_FIELD_OVERFLOW );

        STL_TRY_THROW( stlGetToUInt64FromUInt128( sRemainder, &sFractionValue ) == STL_TRUE,
                       RAMP_ERR_INTERVAL_FIELD_OVERFLOW );
        STL_TRY_THROW( sFractionValue <= STL_UINT32_MAX, RAMP_ERR_INTERVAL_FIELD_OVERFLOW );
    }

    /**
     * Fractional precision check
     */
    if( sFractionValue > 0 )
    {
        sTmpFrac = sFractionValue;

        while( sTmpFrac > 0 )
        {
            if( sTmpFrac % 10 == 0 )
            {
                sZeroLen++;
                sTmpFrac /= 10;
            }
            else
            {
                break;
            }
        }

        STL_TRY_THROW( aIpdRec->mPrecision >= (DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION - sZeroLen),
                       RAMP_ERR_INTERVAL_FIELD_OVERFLOW );
    }

    sTime = (sSecondValue * DTL_USECS_PER_SEC) + sFractionValue;

    /**
     * Leading precision check
     */
    sCmpValue =  (aIpdRec->mDatetimeIntervalPrecision == 2 ? 100 :
                  (aIpdRec->mDatetimeIntervalPrecision == 3 ? 1000 :
                   (aIpdRec->mDatetimeIntervalPrecision == 4 ? 10000 :
                    (aIpdRec->mDatetimeIntervalPrecision == 5 ? 100000 :
                     1000000 ) ) ) );
    
    STL_TRY_THROW( sTime / DTL_USECS_PER_SEC < sCmpValue,
                   RAMP_ERR_INTERVAL_FIELD_OVERFLOW );

    sTime = (sParameter->sign == 0 ? -sTime : sTime);
    
    ((dtlIntervalDayToSecondType *)aDataValue->mValue)->mDay       = sTime / DTL_USECS_PER_DAY;
    ((dtlIntervalDayToSecondType *)aDataValue->mValue)->mTime      = sTime % DTL_USECS_PER_DAY;
    ((dtlIntervalDayToSecondType *)aDataValue->mValue)->mIndicator = DTL_INTERVAL_INDICATOR_SECOND;
    
    aDataValue->mLength = DTL_INTERVAL_DAY_TO_SECOND_SIZE;
   
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
 * SQL_NUMERIC_STRUCT 와 APD( Application Parameter Descriptor ) 정보를 조합하여
 * 유효성을 검사하는 함수
 */ 
static stlStatus zlvValidationCNumericWithAPD( SQL_NUMERIC_STRUCT   * aNumeric,
                                               zldDescElement       * aApdRec,
                                               stlErrorStack        * aErrorStack )
{
    SQLCHAR * sVal;
    stlInt64  sPrecision = 0;
    stlUInt128 sUInt128Value;

#ifdef STL_IS_BIGENDIAN
    sVal = (SQLCHAR*)&sUInt128Value.mLowDigit;
    sVal[0] = aNumeric->val[7];
    sVal[1] = aNumeric->val[6];
    sVal[2] = aNumeric->val[5];
    sVal[3] = aNumeric->val[4];
    sVal[4] = aNumeric->val[3];
    sVal[5] = aNumeric->val[2];
    sVal[6] = aNumeric->val[1];
    sVal[7] = aNumeric->val[0];

    sVal = (SQLCHAR*)&sUInt128Value.mHighDigit;
    sVal[0] = aNumeric->val[15];
    sVal[1] = aNumeric->val[14];
    sVal[2] = aNumeric->val[13];
    sVal[3] = aNumeric->val[12];
    sVal[4] = aNumeric->val[11];
    sVal[5] = aNumeric->val[10];
    sVal[6] = aNumeric->val[9];
    sVal[7] = aNumeric->val[8];
#else
    sVal = (SQLCHAR*)&sUInt128Value.mLowDigit;
    sVal[0] = aNumeric->val[0];
    sVal[1] = aNumeric->val[1];
    sVal[2] = aNumeric->val[2];
    sVal[3] = aNumeric->val[3];
    sVal[4] = aNumeric->val[4];
    sVal[5] = aNumeric->val[5];
    sVal[6] = aNumeric->val[6];
    sVal[7] = aNumeric->val[7];

    sVal = (SQLCHAR*)&sUInt128Value.mHighDigit;
    sVal[0] = aNumeric->val[8];
    sVal[1] = aNumeric->val[9];
    sVal[2] = aNumeric->val[10];
    sVal[3] = aNumeric->val[11];
    sVal[4] = aNumeric->val[12];
    sVal[5] = aNumeric->val[13];
    sVal[6] = aNumeric->val[14];
    sVal[7] = aNumeric->val[15];
#endif

    STL_TRY_THROW( (aNumeric->sign == ZLV_C_TO_SQL_SIGN_POS) ||
                   (aNumeric->sign == ZLV_C_TO_SQL_SIGN_NEG),
                   RAMP_ERR_INVALID_C_NUMERIC_INSTANCE );

    STL_TRY_THROW( (aApdRec->mScale <= ZLV_C_TO_SQL_MAX_SCALE) &&
                   (aApdRec->mScale >= ZLV_C_TO_SQL_MIN_SCALE),
                   RAMP_ERR_INVALID_C_NUMERIC_INSTANCE );

    stlGetPrecisionUInt128( sUInt128Value, sPrecision );

    STL_TRY_THROW( sPrecision <= ZLV_C_TO_SQL_MAX_PRECISION,
                   RAMP_ERR_INVALID_C_NUMERIC_INSTANCE );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_C_NUMERIC_INSTANCE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                      "The supplied value is not a valid instance of data type numeric. "
                      "Check the source data for invalid values. "
                      "An example of an invalid value is data of numeric type with scale greater than precision.",
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * SQL_NUMERIC_STRUCT의 16진수를 10진수 string으로 변환하는 함수
 */
static stlStatus zlvSqlNumericStructToDecimalString( SQL_NUMERIC_STRUCT  * aSqlNumericStruct,
                                                     stlUInt32             aAvailableSize,
                                                     stlChar             * aDecimalString,
                                                     stlUInt32           * aLength,
                                                     stlInt16              aScale )
{
    stlChar             * sBuffer = NULL;
    stlChar             * sBufferValue = NULL;
    stlChar             * sBufferValuePtr = NULL;
    SQLCHAR		  sVal[SQL_MAX_NUMERIC_LEN];
    stlChar               sTemp;
    stlInt32              i;
    stlInt32              j;
    stlInt32              sVar;
    stlInt32              sLength = 0;
    
    /*
     * SQL_C_NUMERIC -> SQL_NUMERIC 으로 변환
     * little endian으로 저장된 16진수를 10진수 string으로 변환후,
     * dtlNumericType 생성.
     */
    sBuffer = aDecimalString;
    sBufferValue = sBuffer;

    stlMemcpy( sVal, aSqlNumericStruct->val, SQL_MAX_NUMERIC_LEN );

    if( aSqlNumericStruct->sign == 0 )
    {
        sBuffer[0] = '-';
        sBufferValue++;
        sLength++;
    }
    else
    {
        // Do Nothing
    }

    /* value '0'은 skip */    
    for( i = (SQL_MAX_NUMERIC_LEN - 1), *sBufferValue = '0', sBufferValuePtr = sBufferValue;
         i >= 0;
         i-- )
    {
        if( sVal[i] != 0 )
        {
            break;
        }
        else
        {
            // Do Nothing
        }
    }

    /* 16진수를 10진수 string으로 변환 */    
    while( i >= 0 )
    {
        
        
        for( j = i, sVar = 0; 0 <= j ; j-- )
        {
            sVar <<= 8;            
            sVar += sVal[j];
            sVal[j] = sVar / 10;

            sVar %= 10;
        }

        STL_TRY( sLength < aAvailableSize );

        *sBufferValuePtr = sVar + '0';
        sBufferValuePtr++;
        sLength++;

        if( sVal[i] == 0 )
        {
            i--;
        }
        else
        {
            // Do Nothing
        }
    }

    if( sBufferValuePtr == aDecimalString )
    {
        sBufferValuePtr[0] = '0';
        sBufferValuePtr++;
        sLength++;
    }
    else
    {
        /* do nothing */
    }
    
    /*
     * sBufferValue가 '0' 문자가 가장 앞에 있더라도, string을 '0' 으로 만들지 않는다.
     * 사실 왜 이렇게 만드는지 모르겠음
     */
    /* string에 scale 정보를 더한다. */
    if( aScale > 0 )
    {
        sLength += stlSnprintf( sBufferValuePtr, 10, "E-%d", aScale );
    }
    else
    {
        sLength += stlSnprintf( sBufferValuePtr, 10, "E+%d", -aScale );
    }

    /* little endian mode로 저장되어 있던 value를 제대로 출력되게 ... */
    for( sBufferValuePtr--;
         sBufferValue < sBufferValuePtr;
         sBufferValue++, sBufferValuePtr-- )
    {
        sTemp = *sBufferValue;
        *sBufferValue = *sBufferValuePtr;
        *sBufferValuePtr = sTemp;
    }

    *aLength = sLength;

    return STL_SUCCESS;

    STL_FINISH;

    *aLength = 0;

    return STL_FAILURE;
    
}


static stlStatus zlvGetUInt128FromCNumeric( SQL_NUMERIC_STRUCT  * aNumeric,
                                            stlUInt128          * aUInt128Value,
                                            stlInt16              aScale,
                                            stlErrorStack       * aErrorStack )
{
    SQLCHAR     * sVal;
    stlUInt128    sUInt128Value;
    stlUInt128    sScaleValue;
    stlInt64      sPrecision = 0;

#ifdef STL_IS_BIGENDIAN
    sVal = (SQLCHAR*)&sUInt128Value.mLowDigit;
    sVal[0] = aNumeric->val[7];
    sVal[1] = aNumeric->val[6];
    sVal[2] = aNumeric->val[5];
    sVal[3] = aNumeric->val[4];
    sVal[4] = aNumeric->val[3];
    sVal[5] = aNumeric->val[2];
    sVal[6] = aNumeric->val[1];
    sVal[7] = aNumeric->val[0];

    sVal = (SQLCHAR*)&sUInt128Value.mHighDigit;
    sVal[0] = aNumeric->val[15];
    sVal[1] = aNumeric->val[14];
    sVal[2] = aNumeric->val[13];
    sVal[3] = aNumeric->val[12];
    sVal[4] = aNumeric->val[11];
    sVal[5] = aNumeric->val[10];
    sVal[6] = aNumeric->val[9];
    sVal[7] = aNumeric->val[8];
#else
    sVal = (SQLCHAR*)&sUInt128Value.mLowDigit;
    sVal[0] = aNumeric->val[0];
    sVal[1] = aNumeric->val[1];
    sVal[2] = aNumeric->val[2];
    sVal[3] = aNumeric->val[3];
    sVal[4] = aNumeric->val[4];
    sVal[5] = aNumeric->val[5];
    sVal[6] = aNumeric->val[6];
    sVal[7] = aNumeric->val[7];

    sVal = (SQLCHAR*)&sUInt128Value.mHighDigit;
    sVal[0] = aNumeric->val[8];
    sVal[1] = aNumeric->val[9];
    sVal[2] = aNumeric->val[10];
    sVal[3] = aNumeric->val[11];
    sVal[4] = aNumeric->val[12];
    sVal[5] = aNumeric->val[13];
    sVal[6] = aNumeric->val[14];
    sVal[7] = aNumeric->val[15];
#endif

    stlGetPrecisionUInt128( sUInt128Value, sPrecision );

    if( STL_IS_UINT128_ZERO( sUInt128Value ) == STL_TRUE )
    {
        aUInt128Value->mHighDigit = 0;
        aUInt128Value->mLowDigit  = 0;
        
        STL_THROW( RAMP_SKIP_SET_VALUE );
    }

    if( aScale > 0 )
    {
        if( sPrecision <= aScale )
        {
            aUInt128Value->mHighDigit = 0;
            aUInt128Value->mLowDigit   = 0;
            
            STL_THROW( RAMP_SKIP_SET_VALUE );
        }
        
        sScaleValue = gPreDefinedPowUInt128[ aScale ];

        STL_TRY_THROW( stlDivUInt128( sUInt128Value, sScaleValue, aUInt128Value ) == STL_TRUE,
                       RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
    }
    else
    {      
        sScaleValue = gPreDefinedPowUInt128[ -aScale ];
        
        STL_TRY_THROW( stlMulUInt128( sUInt128Value, sScaleValue, aUInt128Value ) == STL_TRUE,
                       RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
    }

    STL_RAMP( RAMP_SKIP_SET_VALUE );

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

