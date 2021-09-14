/*******************************************************************************
 * zlvCBitToSql.c
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
 * @file zlvCBitToSql.c
 * @brief Gliese API Internal Converting Data from C Bit to SQL Data Types Routines.
 */

/**
 * @addtogroup zlvCBitToSql
 * @{
 */

/*
 * http://msdn.microsoft.com/en-us/library/windows/desktop/ms710244%28v=VS.85%29.aspx
 */


/*
 * SQL_C_BIT -> SQL_CHAR
 */
ZLV_DECLARE_C_TO_SQL( Bit, Char )
{
    stlUInt8 * sParameter;
    stlBool    sSuccessWithInfo;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mLength,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_OCTETS,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (stlUInt8*)aParameterValuePtr;

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
 * SQL_C_BIT -> SQL_VARCHAR
 */
ZLV_DECLARE_C_TO_SQL( Bit, VarChar )
{
    stlUInt8 * sParameter;
    stlBool    sSuccessWithInfo;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mLength,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_OCTETS,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (stlUInt8*)aParameterValuePtr;

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
 * SQL_C_BIT -> SQL_LONGVARCHAR
 */
ZLV_DECLARE_C_TO_SQL( Bit, LongVarChar )
{
    stlUInt8 * sParameter;
    stlBool    sSuccessWithInfo;

    sParameter = (stlUInt8*)aParameterValuePtr;

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
 * SQL_C_BIT -> SQL_FLOAT
 */
ZLV_DECLARE_C_TO_SQL( Bit, Float )
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
 * SQL_C_BIT -> SQL_NUMERIC
 */
ZLV_DECLARE_C_TO_SQL( Bit, Numeric )
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
 * SQL_C_BIT -> SQL_TINYINT
 */
ZLV_DECLARE_C_TO_SQL( Bit, TinyInt )
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
 * SQL_C_BIT -> SQL_SMALLINT
 */
ZLV_DECLARE_C_TO_SQL( Bit, SmallInt )
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
 * SQL_C_BIT -> SQL_INTEGER
 */
ZLV_DECLARE_C_TO_SQL( Bit, Integer )
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
 * SQL_C_BIT -> SQL_BIGINT
 */
ZLV_DECLARE_C_TO_SQL( Bit, BigInt )
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
 * SQL_C_BIT -> SQL_REAL
 */
ZLV_DECLARE_C_TO_SQL( Bit, Real )
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
 * SQL_C_BIT -> SQL_DOUBLE
 */
ZLV_DECLARE_C_TO_SQL( Bit, Double )
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
 * SQL_C_BIT -> SQL_BIT
 */
ZLV_DECLARE_C_TO_SQL( Bit, Bit )
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
 * SQL_C_BIT -> SQL_BOOLEAN
 */
ZLV_DECLARE_C_TO_SQL( Bit, Boolean )
{
    return zlvCBitToSqlBit( aStmt,
                            aParameterValuePtr,
                            aIndicator,
                            aApdRec,
                            aIpdRec,
                            aDataValue,
                            aSuccessWithInfo,
                            aErrorStack );
}

/** @} */

