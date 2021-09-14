/*******************************************************************************
 * zlvBinaryToC.c
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
 * @file zlvBinaryToC.c
 * @brief Gliese API Internal Converting Data from SQL Binary to C Data Types Routines.
 */

/**
 * @addtogroup zlvBinaryToC
 * @{
 */

/*
 * http://msdn.microsoft.com/en-us/library/windows/desktop/ms713559%28v=VS.85%29.aspx
 */

/*
 * SQL_BINARY -> SQL_C_CHAR
 */
ZLV_DECLARE_SQL_TO_C( Binary, Char )
{
    /*
     * ================================================================================================
     * Test                                      | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ================================================================================================
     * (Byte length of data) * 2 < BufferLength  | Data            | Length of data in bytes | n/a
     * (Byte length of data) * 2 >= BufferLength | Truncated data  | Length of data in bytes | 01004
     * ================================================================================================
     */
    stlInt64    sNumberLength;
    stlInt64    sBinaryLength;
    stlInt64    sHexLength;
    stlChar   * sTarget;
    SQLLEN      sOffset;
    SQLLEN      sLength;
    stlStatus   sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;
    sOffset  = *aOffset;

    STL_TRY_THROW( sOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget = (char*)aTargetValuePtr;

    sBinaryLength = ( aArdRec->mOctetLength - 1 ) / 2;
    sNumberLength = STL_MIN( (aDataValue->mLength - sOffset), sBinaryLength );

    if( (aArdRec->mOctetLength - 1) >= (sNumberLength * 2) )
    {
        STL_TRY( dtlGetStringFromBinaryString( ((stlInt8*)aDataValue->mValue + sOffset),
                                               sNumberLength,
                                               aArdRec->mOctetLength - 1,
                                               sTarget,
                                               &sHexLength,
                                               aErrorStack )
                 == STL_SUCCESS );

        sTarget[sHexLength] = 0;

        *aOffset = sOffset + (sHexLength / 2);
    }

    sLength = (aDataValue->mLength - sOffset) * 2;

    if( aIndicator != NULL )
    {
        *aIndicator = sLength;
    }

    STL_TRY_THROW( sLength < aArdRec->mOctetLength,
                   RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED );

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
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
 * SQL_BINARY -> SQL_C_WCHAR
 */
ZLV_DECLARE_SQL_TO_C( Binary, Wchar )
{
    /*
     * ================================================================================================
     * Test                                      | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ================================================================================================
     * (Byte length of data) * 2 < BufferLength  | Data            | Length of data in bytes | n/a
     * (Byte length of data) * 2 >= BufferLength | Truncated data  | Length of data in bytes | 01004
     * ================================================================================================
     */
    
    stlInt64             sNumberLength;
    stlInt64             sBinaryLength;
    stlInt64             sHexLength;
    SQLLEN               sOffset;
    SQLLEN               sLength;
    stlStatus            sRet = STL_FAILURE;
    dtlUnicodeEncoding   sEncoding;
    stlInt32             sOneCharSize = 0;
    stlInt64             sAvailableSize = 0;

#ifdef SQL_WCHART_CONVERT
    sEncoding = DTL_UNICODE_UTF32;
#else
    sEncoding = DTL_UNICODE_UTF16;
#endif
    
    *aReturn = SQL_ERROR;
    sOffset  = *aOffset;

    sOneCharSize = STL_SIZEOF(SQLWCHAR);
    sAvailableSize = aArdRec->mOctetLength - sOneCharSize;

    STL_TRY_THROW( sOffset < aDataValue->mLength, RAMP_NO_DATA );

    sBinaryLength = sAvailableSize / 2 / sOneCharSize;
    sNumberLength = STL_MIN( (aDataValue->mLength - sOffset), sBinaryLength );

    if( sAvailableSize >= (sNumberLength * 2 * sOneCharSize) )
    {
        STL_TRY( dtlGetWCharStringFromBinaryString( sEncoding,
                                                    ((stlInt8*)aDataValue->mValue + sOffset),
                                                    sNumberLength,
                                                    sAvailableSize,
                                                    aTargetValuePtr,
                                                    &sHexLength,
                                                    aErrorStack )
                 == STL_SUCCESS );

        *(SQLWCHAR*)((stlChar*)aTargetValuePtr + sHexLength) = 0;

        *aOffset = sOffset + sNumberLength;
    }

    sLength = (aDataValue->mLength - sOffset) * 2 * sOneCharSize;

    if( aIndicator != NULL )
    {
        *aIndicator = sLength;
    }

    STL_TRY_THROW( sLength <= sAvailableSize,
                   RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED );

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
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
 * SQL_BINARY -> SQL_C_LONGVARCHAR
 */
ZLV_DECLARE_SQL_TO_C( Binary, LongVarChar )
{
    /*
     * ================================================================================================
     * Test                                      | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ================================================================================================
     * (Byte length of data) * 2 < BufferLength  | Data            | Length of data in bytes | n/a
     * (Byte length of data) * 2 >= BufferLength | Truncated data  | Length of data in bytes | 01004
     * ================================================================================================
     */
    SQL_LONG_VARIABLE_LENGTH_STRUCT * sTarget;
    stlChar                         * sValue;
    stlInt64                          sBufferLength;

    stlInt64    sNumberLength;
    stlInt64    sBinaryLength;
    stlInt64    sHexLength;
    SQLLEN      sOffset;
    SQLLEN      sLength;
    stlStatus   sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;
    sOffset  = *aOffset;

    STL_TRY_THROW( sOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget       = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aTargetValuePtr;
    sValue        = (stlChar*)sTarget->arr;
    sBufferLength = sTarget->len;

    sBinaryLength = ( sBufferLength - 1 ) / 2;
    sNumberLength = STL_MIN( (aDataValue->mLength - sOffset), sBinaryLength );

    if( (sBufferLength - 1) >= (sNumberLength * 2) )
    {
        STL_TRY( dtlGetStringFromBinaryString( ((stlInt8*)aDataValue->mValue + sOffset),
                                               sNumberLength,
                                               sBufferLength - 1,
                                               sValue,
                                               &sHexLength,
                                               aErrorStack )
                 == STL_SUCCESS );

        sValue[sHexLength] = 0;

        *aOffset = sOffset + (sHexLength / 2);
    }

    sLength = (aDataValue->mLength - sOffset) * 2;

    if( aIndicator != NULL )
    {
        *aIndicator = sLength;
    }

    STL_TRY_THROW( sLength < sBufferLength,
                   RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED );

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
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
 * SQL_BINARY -> SQL_C_BINARY
 */
ZLV_DECLARE_SQL_TO_C( Binary, Binary )
{
    /*
     * ==========================================================================================
     * Test                                | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ==========================================================================================
     * Byte length of data <= BufferLength | Data            | Length of data in bytes | n/a
     * Byte length of data > BufferLength  | Truncated data  | Length of data in bytes | 01004
     * ==========================================================================================
     */
    
    SQLLEN    sOffset;
    SQLLEN    sLength;
    stlStatus sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;
    sOffset  = *aOffset;

    STL_TRY_THROW( sOffset < aDataValue->mLength, RAMP_NO_DATA );

    sLength = STL_MIN( (aDataValue->mLength - sOffset), aArdRec->mOctetLength );
    stlMemcpy( aTargetValuePtr,
               (const void*)((stlChar*)aDataValue->mValue + sOffset),
               sLength );

    *aOffset = sOffset + sLength;
                
    sLength = aDataValue->mLength - sOffset;

    if( aIndicator != NULL )
    {
        *aIndicator = sLength;
    }

    STL_TRY_THROW( sLength <= aArdRec->mOctetLength,
                   RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED );

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
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
 * SQL_BINARY -> SQL_C_LONGVARBINARY
 */
ZLV_DECLARE_SQL_TO_C( Binary, LongVarBinary )
{
    /*
     * ==========================================================================================
     * Test                                | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ==========================================================================================
     * Byte length of data <= BufferLength | Data            | Length of data in bytes | n/a
     * Byte length of data > BufferLength  | Truncated data  | Length of data in bytes | 01004
     * ==========================================================================================
     */
    
    SQL_LONG_VARIABLE_LENGTH_STRUCT * sTarget;
    stlChar                         * sValue;
    stlInt64                          sBufferLength;

    SQLLEN    sOffset;
    SQLLEN    sLength;
    stlStatus sRet = STL_FAILURE;

    *aReturn = SQL_ERROR;
    sOffset  = *aOffset;

    STL_TRY_THROW( sOffset < aDataValue->mLength, RAMP_NO_DATA );

    sTarget       = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)aTargetValuePtr;
    sValue        = (stlChar*)sTarget->arr;
    sBufferLength = sTarget->len;

    sLength = STL_MIN( (aDataValue->mLength - sOffset), sBufferLength );
    stlMemcpy( sValue,
               (const void*)((stlChar*)aDataValue->mValue + sOffset),
               sLength );

    *aOffset = sOffset + sLength;
                
    sLength = aDataValue->mLength - sOffset;

    if( aIndicator != NULL )
    {
        *aIndicator = sLength;
    }

    STL_TRY_THROW( sLength <= sBufferLength,
                   RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED );

    sRet     = STL_SUCCESS;
    *aReturn = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA)
    {
        sRet     = STL_SUCCESS;
        *aReturn = SQL_NO_DATA;
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

/** @} */

