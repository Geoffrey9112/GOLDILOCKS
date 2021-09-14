/*******************************************************************************
 * zlvCIntervalYearToMonthToSql.c
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
 * @file zlvCIntervalYearToMonthToSql.c
 * @brief Gliese API Internal Converting Data from C Interval Year to Month to SQL Data Types Routines.
 */

/**
 * @addtogroup zlvCIntervalYearToMonthToSql
 * @{
 */

/*
 * http://msdn.microsoft.com/en-us/library/windows/desktop/ms713990%28v=VS.85%29.aspx
 */

/*
 * SQL_C_INTERVAL_YEAR_TO_MONTH -> SQL_CHAR
 */
ZLV_DECLARE_C_TO_SQL( IntervalYearToMonth, Char )
{
    /*
     * ======================================================
     * Test                                        | SQLSTATE
     * ======================================================
     * Column byte length >= Character byte length | n/a
     * Column byte length < Character byte length  | 22001
     * Data value is not a valid interval literal  | 22015
     * ======================================================
     */

    SQL_INTERVAL_STRUCT * sParameter;
    stlInt32              sLength;
    stlChar               sBuffer[DTL_UINT32_STRING_SIZE + 4];
    stlBool               sSuccessWithInfo;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mLength,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_OCTETS,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_INTERVAL_STRUCT*)aParameterValuePtr;

    if( sParameter->interval_sign == SQL_FALSE )
    {
        sLength = stlSnprintf(sBuffer,
                              DTL_UINT32_STRING_SIZE + 4,
                              "+%u-%02u",
                              sParameter->intval.year_month.year,
                              sParameter->intval.year_month.month );
    }
    else
    {
        sLength = stlSnprintf(sBuffer,
                              DTL_UINT32_STRING_SIZE + 4,
                              "-%u-%02u",
                              sParameter->intval.year_month.year,
                              sParameter->intval.year_month.month );
    }

    STL_TRY_THROW( sLength <= aIpdRec->mOctetLength,
                   RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED );

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
 * SQL_C_INTERVAL_YEAR_TO_MONTH -> SQL_VARCHAR
 */
ZLV_DECLARE_C_TO_SQL( IntervalYearToMonth, VarChar )
{
    /*
     * ======================================================
     * Test                                        | SQLSTATE
     * ======================================================
     * Column byte length >= Character byte length | n/a
     * Column byte length < Character byte length  | 22001
     * Data value is not a valid interval literal  | 22015
     * ======================================================
     */

    SQL_INTERVAL_STRUCT * sParameter;
    stlInt32              sLength;
    stlChar               sBuffer[DTL_UINT32_STRING_SIZE + 4];
    stlBool               sSuccessWithInfo;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mLength,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_OCTETS,
                                      DTL_INTERVAL_INDICATOR_NA,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_INTERVAL_STRUCT*)aParameterValuePtr;

    if( sParameter->interval_sign == SQL_FALSE )
    {
        sLength = stlSnprintf(sBuffer,
                              DTL_UINT32_STRING_SIZE + 4,
                              "+%u-%02u",
                              sParameter->intval.year_month.year,
                              sParameter->intval.year_month.month );
    }
    else
    {
        sLength = stlSnprintf(sBuffer,
                              DTL_UINT32_STRING_SIZE + 4,
                              "-%u-%02u",
                              sParameter->intval.year_month.year,
                              sParameter->intval.year_month.month );
    }

    STL_TRY_THROW( sLength <= aIpdRec->mOctetLength,
                   RAMP_ERR_STRING_DATA_RIGHT_TRUNCATED );

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
 * SQL_C_INTERVAL_YEAR_TO_MONTH -> SQL_LONGVARCHAR
 */
ZLV_DECLARE_C_TO_SQL( IntervalYearToMonth, LongVarChar )
{
    /*
     * ======================================================
     * Test                                        | SQLSTATE
     * ======================================================
     * Column byte length >= Character byte length | n/a
     * Column byte length < Character byte length  | 22001
     * Data value is not a valid interval literal  | 22015
     * ======================================================
     */

    SQL_INTERVAL_STRUCT * sParameter;
    stlInt32              sLength;
    stlChar               sBuffer[DTL_UINT32_STRING_SIZE + 4];
    stlBool               sSuccessWithInfo;

    sParameter = (SQL_INTERVAL_STRUCT*)aParameterValuePtr;

    if( sParameter->interval_sign == SQL_FALSE )
    {
        sLength = stlSnprintf(sBuffer,
                              DTL_UINT32_STRING_SIZE + 4,
                              "+%u-%02u",
                              sParameter->intval.year_month.year,
                              sParameter->intval.year_month.month );
    }
    else
    {
        sLength = stlSnprintf(sBuffer,
                              DTL_UINT32_STRING_SIZE + 4,
                              "-%u-%02u",
                              sParameter->intval.year_month.year,
                              sParameter->intval.year_month.month );
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
 * SQL_C_INTERVAL_YEAR_TO_MONTH -> SQL_INTERVAL_MONTH
 */
ZLV_DECLARE_C_TO_SQL( IntervalYearToMonth, IntervalMonth )
{
    /*
     * =============================================================================
     * Test                                                               | SQLSTATE
     * =============================================================================
     * Data value was converted without truncation of any fields.         | n/a 
     * One or more fields of data value were truncated during conversion. | 22015
     * =============================================================================
     */

    SQL_INTERVAL_STRUCT * sParameter;
    stlBool               sSuccessWithInfo;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_MONTH,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_INTERVAL_STRUCT*)aParameterValuePtr;

    STL_TRY( dtlIntervalYearToMonthSetValueFromTimeInfo( 
                 sParameter->intval.year_month.year,
                 sParameter->intval.year_month.month,
                 sParameter->interval_sign,
                 aIpdRec->mDatetimeIntervalPrecision,
                 DTL_INTERVAL_INDICATOR_MONTH,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/*
 * SQL_C_INTERVAL_YEAR_TO_MONTH -> SQL_INTERVAL_YEAR
 */
ZLV_DECLARE_C_TO_SQL( IntervalYearToMonth, IntervalYear )
{
    /*
     * =============================================================================
     * Test                                                               | SQLSTATE
     * =============================================================================
     * Data value was converted without truncation of any fields.         | n/a 
     * One or more fields of data value were truncated during conversion. | 22015
     * =============================================================================
     */

    SQL_INTERVAL_STRUCT * sParameter;
    stlBool               sSuccessWithInfo;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_YEAR,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_INTERVAL_STRUCT*)aParameterValuePtr;

    STL_TRY_THROW( sParameter->intval.year_month.month == 0,
                   RAMP_ERR_INTERVAL_FIELD_OVERFLOW );

    STL_TRY( dtlIntervalYearToMonthSetValueFromTimeInfo( 
                 sParameter->intval.year_month.year,
                 0,
                 sParameter->interval_sign,
                 aIpdRec->mDatetimeIntervalPrecision,
                 DTL_INTERVAL_INDICATOR_YEAR,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
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
 * SQL_C_INTERVAL_YEAR_TO_MONTH -> SQL_INTERVAL_YEAR_TO_MONTH
 */
ZLV_DECLARE_C_TO_SQL( IntervalYearToMonth, IntervalYearToMonth )
{
    /*
     * =============================================================================
     * Test                                                               | SQLSTATE
     * =============================================================================
     * Data value was converted without truncation of any fields.         | n/a 
     * One or more fields of data value were truncated during conversion. | 22015
     * =============================================================================
     */

    SQL_INTERVAL_STRUCT * sParameter;
    stlBool               sSuccessWithInfo;

    /* Type Info Check */
    STL_TRY( dtlVaildateDataTypeInfo( aDataValue->mType,
                                      aIpdRec->mDatetimeIntervalPrecision,
                                      DTL_SCALE_NA,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH,
                                      aErrorStack )
             == STL_SUCCESS );

    sParameter = (SQL_INTERVAL_STRUCT*)aParameterValuePtr;

    STL_TRY( dtlIntervalYearToMonthSetValueFromTimeInfo( 
                 sParameter->intval.year_month.year,
                 sParameter->intval.year_month.month,
                 sParameter->interval_sign,
                 aIpdRec->mDatetimeIntervalPrecision,
                 DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH,
                 aIpdRec->mOctetLength,
                 aDataValue,
                 &sSuccessWithInfo,
                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/** @} */

