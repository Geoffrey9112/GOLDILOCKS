/*******************************************************************************
 * zlvCBooleanToSql.c
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
 * @file zlvCBooleanToSql.c
 * @brief Gliese API Internal Converting Data from C Boolean to SQL Data Types Routines.
 */

/**
 * @addtogroup zlvCBooleanToSql
 * @{
 */

/*
 * http://msdn.microsoft.com/en-us/library/windows/desktop/ms710244%28v=VS.85%29.aspx
 */


/*
 * SQL_C_BOOLEAN -> SQL_CHAR
 */
ZLV_DECLARE_C_TO_SQL( Boolean, Char )
{
    stlChar  sBuffer[DTL_BOOLEAN_STRING_SIZE];
    stlBool  sSuccessWithInfo;
    stlInt32 sLength;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mLength,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_OCTETS,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    if( (stlUInt8*)aParameterValuePtr == 0 )
    {
        sLength = stlSnprintf(sBuffer,
                              DTL_BOOLEAN_STRING_SIZE,
                              "FALSE" );
    }
    else
    {
        sLength = stlSnprintf(sBuffer,
                              DTL_BOOLEAN_STRING_SIZE,
                              "TRUE" );
    }
    
    STL_TRY( dtlCharSetValueFromStringWithoutPaddNull( sBuffer,
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
                                                       aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/*
 * SQL_C_BOOLEAN -> SQL_VARCHAR
 */
ZLV_DECLARE_C_TO_SQL( Boolean, VarChar )
{
    stlChar  sBuffer[DTL_BOOLEAN_STRING_SIZE];
    stlBool  sSuccessWithInfo;
    stlInt32 sLength;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mLength,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_OCTETS,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    if( (stlUInt8*)aParameterValuePtr == 0 )
    {
        sLength = stlSnprintf(sBuffer,
                              DTL_BOOLEAN_STRING_SIZE,
                              "FALSE" );
    }
    else
    {
        sLength = stlSnprintf(sBuffer,
                              DTL_BOOLEAN_STRING_SIZE,
                              "TRUE" );
    }
    
    STL_TRY( dtlVarcharSetValueFromString( sBuffer,
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
                                           aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/*
 * SQL_C_BOOLEAN -> SQL_LONGVARCHAR
 */
ZLV_DECLARE_C_TO_SQL( Boolean, LongVarChar )
{
    stlChar  sBuffer[DTL_BOOLEAN_STRING_SIZE];
    stlBool  sSuccessWithInfo;
    stlInt32 sLength;

    if( (stlUInt8*)aParameterValuePtr == 0 )
    {
        sLength = stlSnprintf(sBuffer,
                              DTL_BOOLEAN_STRING_SIZE,
                              "FALSE" );
    }
    else
    {
        sLength = stlSnprintf(sBuffer,
                              DTL_BOOLEAN_STRING_SIZE,
                              "TRUE" );
    }
    
    STL_TRY( dtlLongvarcharSetValueFromString( sBuffer,
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
                                               aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/*
 * SQL_C_BOOLEAN -> SQL_FLOAT
 */
ZLV_DECLARE_C_TO_SQL( Boolean, Float )
{
    stlUInt8 * sParameter;
    stlBool    sSuccessWithInfo;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (stlUInt8*)aParameterValuePtr;

    STL_TRY( dtlFloatSetValueFromInteger( *sParameter,
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
 * SQL_C_BOOLEAN -> SQL_NUMERIC
 */
ZLV_DECLARE_C_TO_SQL( Boolean, Numeric )
{
    stlUInt8 * sParameter;
    stlBool    sSuccessWithInfo;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mPrecision,
                                      aIpdRec->mScale,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (stlUInt8*)aParameterValuePtr;

    STL_TRY( dtlNumericSetValueFromInteger( *sParameter,
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
 * SQL_C_BOOLEAN -> SQL_TINYINT
 */
ZLV_DECLARE_C_TO_SQL( Boolean, TinyInt )
{
    stlUInt8 * sParameter;
    stlBool    sSuccessWithInfo;

    sParameter = (stlUInt8*)aParameterValuePtr;

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
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/*
 * SQL_C_BOOLEAN -> SQL_SMALLINT
 */
ZLV_DECLARE_C_TO_SQL( Boolean, SmallInt )
{
    stlUInt8 * sParameter;
    stlBool    sSuccessWithInfo;

    sParameter = (stlUInt8*)aParameterValuePtr;

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
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/*
 * SQL_C_BOOLEAN -> SQL_INTEGER
 */
ZLV_DECLARE_C_TO_SQL( Boolean, Integer )
{
    stlUInt8 * sParameter;
    stlBool    sSuccessWithInfo;

    sParameter = (stlUInt8*)aParameterValuePtr;

    STL_TRY( dtlIntegerSetValueFromInteger( *sParameter,
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
 * SQL_C_BOOLEAN -> SQL_BIGINT
 */
ZLV_DECLARE_C_TO_SQL( Boolean, BigInt )
{
    stlUInt8 * sParameter;
    stlBool    sSuccessWithInfo;

    sParameter = (stlUInt8*)aParameterValuePtr;

    STL_TRY( dtlBigIntSetValueFromInteger( *sParameter,
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
 * SQL_C_BOOLEAN -> SQL_REAL
 */
ZLV_DECLARE_C_TO_SQL( Boolean, Real )
{
    stlUInt8 * sParameter;
    stlBool    sSuccessWithInfo;

    sParameter = (stlUInt8*)aParameterValuePtr;

    STL_TRY( dtlRealSetValueFromInteger( *sParameter,
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
 * SQL_C_BOOLEAN -> SQL_DOUBLE
 */
ZLV_DECLARE_C_TO_SQL( Boolean, Double )
{
    stlUInt8 * sParameter;
    stlBool    sSuccessWithInfo;

    sParameter = (stlUInt8*)aParameterValuePtr;

    STL_TRY( dtlDoubleSetValueFromInteger( *sParameter,
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
 * SQL_C_BOOLEAN -> SQL_BIT
 */
ZLV_DECLARE_C_TO_SQL( Boolean, Bit )
{
    stlUInt8 * sParameter;
    stlBool    sSuccessWithInfo;

    sParameter = (stlUInt8*)aParameterValuePtr;

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
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/*
 * SQL_C_BOOLEAN -> SQL_BOOLEAN
 */
ZLV_DECLARE_C_TO_SQL( Boolean, Boolean )
{
    return zlvCBooleanToSqlBit( aStmt,
                                aParameterValuePtr,
                                aIndicator,
                                aApdRec,
                                aIpdRec,
                                aDataValue,
                                aSuccessWithInfo,
                                aErrorStack );
}

/** @} */

