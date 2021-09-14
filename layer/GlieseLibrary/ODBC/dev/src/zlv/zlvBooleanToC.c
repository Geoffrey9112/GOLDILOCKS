/*******************************************************************************
 * zlvBooleanToC.c
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
 * @file zlvBooleanToC.c
 * @brief Gliese API Internal Converting Data from SQL Boolean to C Data Types Routines.
 */

/**
 * @addtogroup zlvBooleanToC
 * @{
 */

/*
 * http://msdn.microsoft.com/en-us/library/windows/desktop/ms709401%28v=VS.85%29.aspx
 */

/*
 * SQL_BOOLEAN -> SQL_C_CHAR
 */
ZLV_DECLARE_SQL_TO_C( Boolean, Char )
{
    /*
     * ==================================================================
     * Test                 | *TargetValuePtr | *StrLen_or_IndPtr | SQLSTATE
     * ==================================================================
     * BufferLength > 4(5)  | Data            | 4(5)              | n/a
     * BufferLength <= 4(5) | Undefined       | Undefined         | 22003
     * ==================================================================
     */
    
    dtlBooleanType * sDataValue;
    stlStatus        sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlBooleanType*)aDataValue->mValue;

    if( *sDataValue == STL_TRUE )
    {
        STL_TRY_THROW( aArdRec->mOctetLength > 4,
                       RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

        stlSnprintf( (stlChar*)aTargetValuePtr,
                     aArdRec->mOctetLength,
                     "TRUE" );

        if( aIndicator != NULL )
        {
            *aIndicator = 4;
        }
    }
    else
    {
        STL_TRY_THROW( aArdRec->mOctetLength > 5,
                       RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

        stlSnprintf( (stlChar*)aTargetValuePtr,
                     aArdRec->mOctetLength,
                     "FALSE" );

        if( aIndicator != NULL )
        {
            *aIndicator = 5;
        }
    }

    *aOffset = aDataValue->mLength;

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
 * SQL_BOOLEAN -> SQL_C_WCHAR
 */
ZLV_DECLARE_SQL_TO_C( Boolean, Wchar )
{
    /*
     * ==================================================================
     * Test                 | *TargetValuePtr | *StrLen_or_IndPtr | SQLSTATE
     * ==================================================================
     * BufferLength > 4(5)  | Data            | 4(5)              | n/a
     * BufferLength <= 4(5) | Undefined       | Undefined         | 22003
     * ==================================================================
     */
    
    dtlBooleanType * sDataValue;
    stlStatus        sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlBooleanType*)aDataValue->mValue;

    if( *sDataValue == STL_TRUE )
    {
        STL_TRY_THROW( (5 * STL_SIZEOF(SQLWCHAR)) <= aArdRec->mOctetLength,
                       RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

        ((SQLWCHAR*)aTargetValuePtr)[0] = 'T';
        ((SQLWCHAR*)aTargetValuePtr)[1] = 'R';
        ((SQLWCHAR*)aTargetValuePtr)[2] = 'U';
        ((SQLWCHAR*)aTargetValuePtr)[3] = 'E';
        ((SQLWCHAR*)aTargetValuePtr)[4] = 0;

        if( aIndicator != NULL )
        {
            *aIndicator = 4 * STL_SIZEOF(SQLWCHAR);
        }
    }
    else
    {
        STL_TRY_THROW( (6 * STL_SIZEOF(SQLWCHAR)) <= aArdRec->mOctetLength,
                       RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

        ((SQLWCHAR*)aTargetValuePtr)[0] = 'F';
        ((SQLWCHAR*)aTargetValuePtr)[1] = 'A';
        ((SQLWCHAR*)aTargetValuePtr)[2] = 'L';
        ((SQLWCHAR*)aTargetValuePtr)[3] = 'S';
        ((SQLWCHAR*)aTargetValuePtr)[4] = 'E';
        ((SQLWCHAR*)aTargetValuePtr)[5] = 0;

        if( aIndicator != NULL )
        {
            *aIndicator = 5 * STL_SIZEOF(SQLWCHAR);
        }
    }

    *aOffset = aDataValue->mLength;

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
 * SQL_BOOLEAN -> SQL_C_LONGVARCHAR
 */
ZLV_DECLARE_SQL_TO_C( Boolean, LongVarChar )
{
    /*
     * ==================================================================
     * Test                 | *TargetValuePtr | *StrLen_or_IndPtr | SQLSTATE
     * ==================================================================
     * BufferLength > 4(5)  | Data            | 4(5)              | n/a
     * BufferLength <= 4(5) | Undefined       | Undefined         | 22003
     * ==================================================================
     */
    
    SQL_LONG_VARIABLE_LENGTH_STRUCT * sTarget;
    stlChar                         * sValue;
    stlInt64                          sBufferLength;

    dtlBooleanType * sDataValue;
    stlStatus        sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlBooleanType*)aDataValue->mValue;

    sTarget       = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aTargetValuePtr;
    sValue        = (stlChar*)sTarget->arr;
    sBufferLength = sTarget->len;

    if( *sDataValue == STL_TRUE )
    {
        STL_TRY_THROW( sBufferLength > 4,
                       RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

        stlSnprintf( sValue,
                     sBufferLength,
                     "TRUE" );

        if( aIndicator != NULL )
        {
            *aIndicator = 4;
        }
    }
    else
    {
        STL_TRY_THROW( sBufferLength > 5,
                       RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

        stlSnprintf( sValue,
                     sBufferLength,
                     "FALSE" );

        if( aIndicator != NULL )
        {
            *aIndicator = 5;
        }
    }

    *aOffset = aDataValue->mLength;

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
 * SQL_BOOLEAN -> SQL_C_STINYINT
 */
ZLV_DECLARE_SQL_TO_C( Boolean, STinyInt )
{
    /*
     * ===========================================================
     * Test | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ===========================================================
     * None | Data            | Size of the C data type | n/a
     * ===========================================================
     */
    
    dtlBooleanType * sDataValue;
    stlInt8        * sTarget;
    stlStatus        sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlBooleanType*)aDataValue->mValue;
    sTarget    = (stlInt8*)aTargetValuePtr;

    *sTarget = *sDataValue;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_INT8_SIZE;
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
 * SQL_BOOLEAN -> SQL_C_UTINYINT
 */
ZLV_DECLARE_SQL_TO_C( Boolean, UTinyInt )
{
    /*
     * ===========================================================
     * Test | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ===========================================================
     * None | Data            | Size of the C data type | n/a
     * ===========================================================
     */
    
    dtlBooleanType * sDataValue;
    stlUInt8       * sTarget;
    stlStatus        sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlBooleanType*)aDataValue->mValue;
    sTarget    = (stlUInt8*)aTargetValuePtr;

    *sTarget = *sDataValue;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_UINT8_SIZE;
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
 * SQL_BOOLEAN -> SQL_C_SBIGINT
 */
ZLV_DECLARE_SQL_TO_C( Boolean, SBigInt )
{
    /*
     * ===========================================================
     * Test | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ===========================================================
     * None | Data            | Size of the C data type | n/a
     * ===========================================================
     */
    
    dtlBooleanType * sDataValue;
    stlInt64       * sTarget;
    stlStatus        sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlBooleanType*)aDataValue->mValue;
    sTarget    = (stlInt64*)aTargetValuePtr;

    *sTarget = *sDataValue;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_INT64_SIZE;
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
 * SQL_BOOLEAN -> SQL_C_UBIGINT
 */
ZLV_DECLARE_SQL_TO_C( Boolean, UBigInt )
{
    /*
     * ===========================================================
     * Test | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ===========================================================
     * None | Data            | Size of the C data type | n/a
     * ===========================================================
     */
    
    dtlBooleanType * sDataValue;
    stlUInt64      * sTarget;
    stlStatus        sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlBooleanType*)aDataValue->mValue;
    sTarget    = (stlUInt64*)aTargetValuePtr;

    *sTarget = *sDataValue;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_UINT64_SIZE;
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
 * SQL_BOOLEAN -> SQL_C_SSHORT
 */
ZLV_DECLARE_SQL_TO_C( Boolean, SShort )
{
    /*
     * ===========================================================
     * Test | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ===========================================================
     * None | Data            | Size of the C data type | n/a
     * ===========================================================
     */
    
    dtlBooleanType * sDataValue;
    stlInt16       * sTarget;
    stlStatus        sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlBooleanType*)aDataValue->mValue;
    sTarget    = (stlInt16*)aTargetValuePtr;

    *sTarget = *sDataValue;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_INT16_SIZE;
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
 * SQL_BOOLEAN -> SQL_C_USHORT
 */
ZLV_DECLARE_SQL_TO_C( Boolean, UShort )
{
    /*
     * ===========================================================
     * Test | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ===========================================================
     * None | Data            | Size of the C data type | n/a
     * ===========================================================
     */
    
    dtlBooleanType * sDataValue;
    stlUInt16      * sTarget;
    stlStatus        sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlBooleanType*)aDataValue->mValue;
    sTarget    = (stlUInt16*)aTargetValuePtr;

    *sTarget = *sDataValue;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_UINT16_SIZE;
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
 * SQL_BOOLEAN -> SQL_C_SLONG
 */
ZLV_DECLARE_SQL_TO_C( Boolean, SLong )
{
    /*
     * ===========================================================
     * Test | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ===========================================================
     * None | Data            | Size of the C data type | n/a
     * ===========================================================
     */
    
    dtlBooleanType * sDataValue;
    stlInt32       * sTarget;
    stlStatus        sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlBooleanType*)aDataValue->mValue;
    sTarget    = (stlInt32*)aTargetValuePtr;

    *sTarget = *sDataValue;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_INT32_SIZE;
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
 * SQL_BOOLEAN -> SQL_C_ULONG
 */
ZLV_DECLARE_SQL_TO_C( Boolean, ULong )
{
    /*
     * ===========================================================
     * Test | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ===========================================================
     * None | Data            | Size of the C data type | n/a
     * ===========================================================
     */
    
    dtlBooleanType * sDataValue;
    stlUInt32      * sTarget;
    stlStatus        sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlBooleanType*)aDataValue->mValue;
    sTarget    = (stlUInt32*)aTargetValuePtr;

    *sTarget = *sDataValue;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_UINT32_SIZE;
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
 * SQL_BOOLEAN -> SQL_C_NUMERIC
 */
ZLV_DECLARE_SQL_TO_C( Boolean, Numeric )
{
    /*
     * ===========================================================
     * Test | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ===========================================================
     * None | Data            | Size of the C data type | n/a
     * ===========================================================
     */
    
    dtlBooleanType     * sDataValue;
    SQL_NUMERIC_STRUCT * sTarget;
    stlStatus            sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlBooleanType*)aDataValue->mValue;
    sTarget    = (SQL_NUMERIC_STRUCT*)aTargetValuePtr;

    STL_TRY_THROW( ((ZLV_SQL_TO_C_NUMERIC_MAX_SCALE >= aArdRec->mScale) &&
                    (ZLV_SQL_TO_C_NUMERIC_MIN_SCALE <= aArdRec->mScale)),
                   RAMP_ERR_SCALE_VALUE_OUT_OF_RANGE );

    STL_TRY( zlvMakeCNumericFromInteger( aStmt,
                                         sTarget,
                                         stlAbsInt64( *sDataValue ),
                                         aArdRec->mScale,
                                         ZLV_BOOLEAN_TO_C_NUMERIC_PRECISION,
                                         STL_FALSE,
                                         aErrorStack )
             == STL_SUCCESS );

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = STL_SIZEOF( SQL_NUMERIC_STRUCT );
    }

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_ERR_SCALE_VALUE_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                      "Specified scale value is out of range( -128 to 127 )",
                      aErrorStack );
    }

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
    }

    STL_FINISH;

    return sRet;
}

/*
 * SQL_BOOLEAN -> SQL_C_FLOAT
 */
ZLV_DECLARE_SQL_TO_C( Boolean, Float )
{
    /*
     * ===========================================================
     * Test | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ===========================================================
     * None | Data            | Size of the C data type | n/a
     * ===========================================================
     */
    
    dtlBooleanType * sDataValue;
    stlFloat32     * sTarget;
    stlStatus        sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlBooleanType*)aDataValue->mValue;
    sTarget    = (stlFloat32*)aTargetValuePtr;

    *sTarget = *sDataValue;

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
 * SQL_BOOLEAN -> SQL_C_DOUBLE
 */
ZLV_DECLARE_SQL_TO_C( Boolean, Double )
{
    /*
     * ===========================================================
     * Test | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ===========================================================
     * None | Data            | Size of the C data type | n/a
     * ===========================================================
     */
    
    dtlBooleanType * sDataValue;
    stlFloat64     * sTarget;
    stlStatus        sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    sDataValue = (dtlBooleanType*)aDataValue->mValue;
    sTarget    = (stlFloat64*)aTargetValuePtr;

    *sTarget = *sDataValue;

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
 * SQL_BOOLEAN -> SQL_C_BIT
 */
ZLV_DECLARE_SQL_TO_C( Boolean, Bit )
{
    /*
     * ===========================================================
     * Test | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ===========================================================
     * None | Data            | 1                       | n/a
     * ===========================================================
     */
    
    dtlBooleanType * sDataValue;
    stlUInt8       * sTarget;
    stlStatus        sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < 1, RAMP_NO_DATA );

    sDataValue = (dtlBooleanType*)aDataValue->mValue;
    sTarget    = (stlUInt8*)aTargetValuePtr;

    *sTarget = *sDataValue;

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = 1;
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
 * SQL_BOOLEAN -> SQL_C_BOOLEAN
 */
ZLV_DECLARE_SQL_TO_C( Boolean, Boolean )
{
    return zlvSqlBooleanToCBit( aStmt,
                                aValueBlockList,
                                aDataValue,
                                aOffset,
                                aTargetValuePtr,
                                aIndicator,
                                aArdRec,
                                aReturn,
                                aErrorStack );
}

/*
 * SQL_BOOLEAN -> SQL_C_BINARY
 */
ZLV_DECLARE_SQL_TO_C( Boolean, Binary )
{
    /*
     * ==================================================================
     * Test              | *TargetValuePtr | *StrLen_or_IndPtr | SQLSTATE
     * ==================================================================
     * BufferLength >= 1 | Data            | 1                 | n/a
     * BufferLength < 1  | Undefined       | Undefined         | 22003
     * ==================================================================
     */
    
    stlStatus sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;

    STL_TRY_THROW( *aOffset < aDataValue->mLength, RAMP_NO_DATA );

    STL_TRY_THROW( aArdRec->mOctetLength >= 1,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    stlMemcpy( aTargetValuePtr,
               (const void*)aDataValue->mValue,
               1 );

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = 1;
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
 * SQL_BOOLEAN -> SQL_C_LONGVARBINARY
 */
ZLV_DECLARE_SQL_TO_C( Boolean, LongVarBinary )
{
    /*
     * ==================================================================
     * Test              | *TargetValuePtr | *StrLen_or_IndPtr | SQLSTATE
     * ==================================================================
     * BufferLength >= 1 | Data            | 1                 | n/a
     * BufferLength < 1  | Undefined       | Undefined         | 22003
     * ==================================================================
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

    STL_TRY_THROW( sBufferLength >= 1,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    stlMemcpy( sValue,
               (const void*)aDataValue->mValue,
               1 );

    *aOffset = aDataValue->mLength;

    if( aIndicator != NULL )
    {
        *aIndicator = 1;
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

/** @} */

