/*******************************************************************************
 * zlvCharacterset.c
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
#include <zllDef.h>
#include <zlDef.h>
#include <zle.h>

/**
 * @file zlvCharacterset.c
 * @brief Gliese API Internal Character Set Routines.
 */

/**
 * @addtogroup zlv
 * @{
 */

stlSize zlvStrlenW( SQLWCHAR * aString )
{
    stlSize i = 0;

    while( aString[ i ] )
    {
        i++;
    }

    return i;    
}

stlStatus zlvNonConvertSQL( zlsStmt        * aStmt,
                            stlChar        * aStatementText,
                            stlInt32         aTextLength,
                            stlChar       ** aSQL,
                            stlErrorStack  * aErrorStack )
{
    stlInt32 sTextLength = 0;

    if( aTextLength == SQL_NTS )
    {
        *aSQL       = aStatementText;
        sTextLength = stlStrlen( aStatementText );

        STL_TRY_THROW( sTextLength > 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );
    }
    else
    {
        STL_TRY_THROW( aTextLength > 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

        if( aStmt->mSQLBufferSize <= aTextLength )
        {
            if( aStmt->mSQLBufferSize > 0 )
            {
                stlFreeHeap( aStmt->mSQLBuffer );

                aStmt->mSQLBuffer     = NULL;
                aStmt->mSQLBufferSize = 0;
            }

            STL_TRY( stlAllocHeap( (void**)&aStmt->mSQLBuffer,
                                   aTextLength + 1,
                                   aErrorStack ) == STL_SUCCESS );

            aStmt->mSQLBufferSize = aTextLength + 1;
        }

        stlMemset( aStmt->mSQLBuffer, 0x00, aTextLength + 1 );
        stlMemcpy( aStmt->mSQLBuffer, aStatementText, aTextLength );

        *aSQL = aStmt->mSQLBuffer;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_STRING_OR_BUFFER_LENGTH,
                      "The argument TextLength was less than or equal to 0 "
                      "but not equal to SQL_NTS.",
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlvConvertSQL( zlsStmt        * aStmt,
                         stlChar        * aStatementText,
                         stlInt32         aTextLength,
                         stlChar       ** aSQL,
                         stlErrorStack  * aErrorStack )
{
    stlInt32        sTextLength    = 0;
    stlInt64        sMaxTextLength = 0;
    dtlCharacterSet sSourceCharacterSet;
    dtlCharacterSet sDestCharacterSet;
    stlInt64        sOffset;
    stlInt64        sLen;
    
    sSourceCharacterSet = (ZLS_STMT_DT_VECTOR(aStmt))->mGetCharSetIDFunc(aStmt);
    sDestCharacterSet   = (ZLS_STMT_NLS_DT_VECTOR(aStmt))->mGetCharSetIDFunc(aStmt);

    if( aTextLength == SQL_NTS )
    {
        sTextLength = stlStrlen( aStatementText );

        STL_TRY_THROW( sTextLength > 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );
    }
    else
    {
        STL_TRY_THROW( aTextLength > 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

        sTextLength = aTextLength;
    }

    /*
     * source code 전체가 ascii 인 경우라면, sMaxTextLength == sTextLength 이겠지만,
     * multi byte인 경우, 최소단위인 2byte로 구성된 글자로 가정하고 sMaxTextLength룰 예측한다.
     */
    sMaxTextLength = (sTextLength / 2 + 1) * ( dtlGetMbMaxLength(sDestCharacterSet) );

    if( aStmt->mSQLBufferSize <= sMaxTextLength )
    {
        if( aStmt->mSQLBufferSize > 0 )
        {
            stlFreeHeap( aStmt->mSQLBuffer );

            aStmt->mSQLBuffer     = NULL;
            aStmt->mSQLBufferSize = 0;
        }

        STL_TRY( stlAllocHeap( (void**)&aStmt->mSQLBuffer,
                               sMaxTextLength + 1,
                               aErrorStack ) == STL_SUCCESS );

        aStmt->mSQLBufferSize = sMaxTextLength + 1;
    }

    stlMemset( aStmt->mSQLBuffer, 0x00, sMaxTextLength + 1 );

    STL_TRY( dtlMbConversion( sSourceCharacterSet,
                              sDestCharacterSet,
                              (stlChar*)aStatementText,
                              sTextLength,
                              aStmt->mSQLBuffer,
                              sMaxTextLength,
                              &sOffset,
                              &sLen,
                              aErrorStack ) == STL_SUCCESS );

    STL_DASSERT( sLen < sMaxTextLength + 1 );

    aStmt->mSQLBuffer[sLen] = '\0';

    *aSQL = aStmt->mSQLBuffer;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_STRING_OR_BUFFER_LENGTH,
                      "The argument TextLength was less than or equal to 0 "
                      "but not equal to SQL_NTS.",
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * convert resultset
 */
stlStatus zlvNonConvertResultSet( zlsStmt        * aStmt,
                                  SQLLEN         * aOffset,
                                  dtlDataValue   * aDataValue,
                                  void           * aTargetValuePtr,
                                  stlInt64         aTargetLength,
                                  SQLLEN         * aIndicator,
                                  stlErrorStack  * aErrorStack )
{
    SQLLEN sOffset;
    SQLLEN sLength;

    sOffset = *aOffset;

    sLength = STL_MIN( (aDataValue->mLength - sOffset), (aTargetLength - 1) );

    if( sLength >= 0 )
    {
        stlMemcpy( aTargetValuePtr,
                   (const void*)((stlChar*)aDataValue->mValue + sOffset),
                   sLength );

        ((stlChar*)aTargetValuePtr)[sLength] = '\0';

        *aOffset = sOffset + sLength;
    }

    sLength = aDataValue->mLength - sOffset;

    if( aIndicator != NULL )
    {
        *aIndicator = sLength;
    }

    return STL_SUCCESS;
}

stlStatus zlvConvertResultSet( zlsStmt        * aStmt,
                               SQLLEN         * aOffset,
                               dtlDataValue   * aDataValue,
                               void           * aTargetValuePtr,
                               stlInt64         aTargetLength,
                               SQLLEN         * aIndicator,
                               stlErrorStack  * aErrorStack )
{
    stlInt64 sBeginOffset;
    stlInt64 sEndOffset;
    stlInt64 sIndicator;
    stlInt64 sLength;

    sBeginOffset = *aOffset;
    sLength = aTargetLength - 1;
    
    STL_TRY( dtlMbConversion( (ZLS_STMT_NLS_DT_VECTOR(aStmt))->mGetCharSetIDFunc(aStmt),
                              (ZLS_STMT_DT_VECTOR(aStmt))->mGetCharSetIDFunc(aStmt),
                              (stlChar*)aDataValue->mValue + sBeginOffset,
                              aDataValue->mLength - sBeginOffset,
                              (stlChar*)aTargetValuePtr,
                              sLength,
                              &sEndOffset,
                              &sIndicator,
                              aErrorStack ) == STL_SUCCESS );

    if( sLength >= 0 )
    {
        if( sIndicator < sLength )
        {
            ((stlChar*)aTargetValuePtr)[sIndicator] = '\0';
        }
        else
        {
            ((stlChar*)aTargetValuePtr)[sLength] = '\0';
        }

        if( sEndOffset > 0 )
        {
            *aOffset = sBeginOffset + sEndOffset + 1;
        }
    }

    if( aIndicator != NULL )
    {
        *aIndicator = sIndicator;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * Error Message
 */

stlStatus zlvNonConvertError( zlsStmt        * aStmt,
                              stlErrorData   * aErrorData,
                              stlChar        * aExtraMessage,
                              zldDiag        * aDiag,
                              zldDiagElement * aDiagRec,
                              stlErrorStack  * aErrorStack )
{
    stlSize   sMessageLen = 0;
    stlChar * sExtraMessage;
    stlChar * sEmptyMessage = "";

    sMessageLen += stlStrlen( ZLL_VENDOR_IDENTIFIER ) +
        stlStrlen( ZLL_ODBC_COMPONENT_IDENTIFIER ) +
        stlStrlen( ZLL_DATA_SOURCE_IDENTIFIER ) + 1;
    sMessageLen += stlStrlen(aErrorData->mErrorMessage ) + 3;
    sMessageLen += stlStrlen(aErrorData->mDetailErrorMessage ) + 1;
    
    if( aExtraMessage != NULL )
    {
        sMessageLen += stlStrlen(aExtraMessage) + 1;
        sExtraMessage = aExtraMessage;
    }
    else
    {
        sExtraMessage = sEmptyMessage;
    }

    STL_TRY( stlAllocRegionMem( &aDiag->mRegionMem,
                                sMessageLen,
                                (void**)&aDiagRec->mMessageText,
                                aErrorStack ) == STL_SUCCESS );

    if( aErrorData->mDetailErrorMessage[0] == '\0' )
    {
        stlSnprintf( aDiagRec->mMessageText,
                     sMessageLen,
                     "%s%s%s%s\n%s",
                     ZLL_VENDOR_IDENTIFIER,
                     ZLL_ODBC_COMPONENT_IDENTIFIER,
                     ZLL_DATA_SOURCE_IDENTIFIER,
                     aErrorData->mErrorMessage,
                     sExtraMessage );
    }
    else
    {
        stlSnprintf( aDiagRec->mMessageText,
                     sMessageLen,
                     "%s%s%s%s : %s\n%s",
                     ZLL_VENDOR_IDENTIFIER,
                     ZLL_ODBC_COMPONENT_IDENTIFIER,
                     ZLL_DATA_SOURCE_IDENTIFIER,
                     aErrorData->mErrorMessage,
                     aErrorData->mDetailErrorMessage,
                     sExtraMessage );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlvConvertError( zlsStmt        * aStmt,
                           stlErrorData   * aErrorData,
                           stlChar        * aExtraMessage,
                           zldDiag        * aDiag,
                           zldDiagElement * aDiagRec,
                           stlErrorStack  * aErrorStack )
{
    stlSize         sMessageLen = 0;
    dtlCharacterSet sSourceCharacterSet;
    dtlCharacterSet sDestCharacterSet;
    stlInt64        sMaxLength = 0;
    stlInt64        sPos = 0;
    stlInt64        sOffset = 0;
    stlInt64        sLength = 0;

    sSourceCharacterSet = (ZLS_STMT_NLS_DT_VECTOR(aStmt))->mGetCharSetIDFunc(aStmt);
    sDestCharacterSet   = (ZLS_STMT_DT_VECTOR(aStmt))->mGetCharSetIDFunc(aStmt);

    sMessageLen += stlStrlen( ZLL_VENDOR_IDENTIFIER ) +
        stlStrlen( ZLL_ODBC_COMPONENT_IDENTIFIER ) +
        stlStrlen( ZLL_DATA_SOURCE_IDENTIFIER ) + 1;
    sMessageLen += stlStrlen(aErrorData->mErrorMessage ) + 3;
    sMessageLen += stlStrlen(aErrorData->mDetailErrorMessage ) + 1;
    
    if( aExtraMessage != NULL )
    {
        sMessageLen += stlStrlen(aExtraMessage) + 1;
    }

    if( dtlGetMbMaxLength( sSourceCharacterSet ) > dtlGetMbMaxLength( sDestCharacterSet ) )
    {
        sMaxLength = sMessageLen;
    }
    else
    {
        sMaxLength = sMessageLen * (dtlGetMbMaxLength( sDestCharacterSet ) / dtlGetMbMaxLength( sSourceCharacterSet ));
    }

    STL_TRY( stlAllocRegionMem( &aDiag->mRegionMem,
                                sMaxLength,
                                (void**)&aDiagRec->mMessageText,
                                aErrorStack ) == STL_SUCCESS );

    sPos = stlSnprintf( aDiagRec->mMessageText,
                        sMaxLength,
                        "%s%s%s",
                        ZLL_VENDOR_IDENTIFIER,
                        ZLL_ODBC_COMPONENT_IDENTIFIER,
                        ZLL_DATA_SOURCE_IDENTIFIER );

    sMaxLength -= sPos;

    STL_TRY( dtlMbConversion( sSourceCharacterSet,
                              sDestCharacterSet,
                              aErrorData->mErrorMessage,
                              stlStrlen(aErrorData->mErrorMessage),
                              aDiagRec->mMessageText + sPos,
                              sMaxLength,
                              &sOffset,
                              &sLength,
                              aErrorStack ) == STL_SUCCESS );

    sPos += sLength;
    sMaxLength -= sLength;
    
    if( aErrorData->mDetailErrorMessage[0] != '\0' )
    {
        sOffset = stlSnprintf( aDiagRec->mMessageText + sPos,
                               sMaxLength,
                               " : " );

        sPos += sOffset;
        sMaxLength -= sOffset;

        STL_TRY( dtlMbConversion( sSourceCharacterSet,
                                  sDestCharacterSet,
                                  aErrorData->mDetailErrorMessage,
                                  stlStrlen(aErrorData->mDetailErrorMessage),
                                  aDiagRec->mMessageText + sPos,
                                  sMaxLength,
                                  &sOffset,
                                  &sLength,
                                  aErrorStack ) == STL_SUCCESS );

        sPos += sLength;
        sMaxLength -= sLength;
    }

    sOffset = stlSnprintf( aDiagRec->mMessageText + sPos,
                           sMaxLength,
                           "\n" );

    sPos += sOffset;
    sMaxLength -= sOffset;

    if( aExtraMessage != NULL )
    {
        STL_TRY( dtlMbConversion( sSourceCharacterSet,
                                  sDestCharacterSet,
                                  aExtraMessage,
                                  stlStrlen(aExtraMessage),
                                  aDiagRec->mMessageText + sPos,
                                  sMaxLength,
                                  &sOffset,
                                  &sLength,
                                  aErrorStack ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlvAllocCharBuffer( SQLWCHAR         * aSrc,
                              stlInt64           aSrcLength,
                              dtlCharacterSet    aCharacterset,
                              SQLCHAR         ** aBuffer,
                              stlInt64         * aBufferLength,
                              stlAllocator     * aAllocator,
                              stlErrorStack    * aErrorStack )
{
    stlInt64 sBufferLength = 0;

    if( aBufferLength != NULL )
    {
        *aBufferLength = 0;
    }
    
    if( aSrcLength == SQL_NTS )
    {
        if( aSrc != NULL )
        {
            aSrcLength = zlvStrlenW(aSrc);
        }
    }
    else
    {
        STL_TRY_THROW( aSrcLength >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );
    }

    if( aSrcLength > 0 )
    {
        sBufferLength = dtlGetMbMaxLength(aCharacterset) * (aSrcLength + 1);

        STL_TRY( stlAllocRegionMem( aAllocator,
                                    sBufferLength,
                                    (void**)aBuffer,
                                    aErrorStack ) == STL_SUCCESS );
    }

    if( aBufferLength != NULL )
    {
        *aBufferLength = sBufferLength;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_STRING_OR_BUFFER_LENGTH,
                      "The value of one of the length arguments was less than 0 "
                      "but not equal to SQL_NTS.",
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlvAllocWcharBuffer( SQLCHAR        * aSrc,
                               stlInt64         aSrcLength,
                               SQLWCHAR      ** aBuffer,
                               stlInt64       * aBufferLength,
                               stlAllocator   * aAllocator,
                               stlErrorStack  * aErrorStack )
{
    stlInt64 sBufferLength = 0;

    if( aBufferLength != NULL )
    {
        *aBufferLength = 0;
    }
    
    if( aSrcLength == SQL_NTS )
    {
        if( aSrc != NULL )
        {
            aSrcLength = stlStrlen((stlChar*)aSrc);
        }
    }
    else
    {
        STL_TRY_THROW( aSrcLength >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );
    }

    if( aSrcLength > 0 )
    {
        sBufferLength = (aSrcLength + 1) * STL_SIZEOF(SQLWCHAR);

        STL_TRY( stlAllocRegionMem( aAllocator,
                                    sBufferLength,
                                    (void**)aBuffer,
                                    aErrorStack ) == STL_SUCCESS );
    }

    if( aBufferLength != NULL )
    {
        *aBufferLength = sBufferLength;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_STRING_OR_BUFFER_LENGTH,
                      "The value of one of the length arguments was less than 0 "
                      "but not equal to SQL_NTS.",
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlvWcharToChar( SQLWCHAR         * aSrc,
                          stlInt64           aSrcLength,
                          dtlCharacterSet    aCharacterset,
                          SQLCHAR          * aDest,
                          stlInt64           aDestLength,
                          stlInt64         * aTotalLength,
                          stlErrorStack    * aErrorStack )
{
    stlInt64           sLength = 0;
    dtlUnicodeEncoding sEncoding;
    stlInt64           sOffset = 0;
    stlInt64           sBufferLength = 0;
    
#ifdef SQL_WCHART_CONVERT
    sEncoding = DTL_UNICODE_UTF32;
#else
    sEncoding = DTL_UNICODE_UTF16;
#endif

    if( aSrcLength == SQL_NTS )
    {
        if( aSrc != NULL )
        {
            aSrcLength = zlvStrlenW(aSrc);
        }
    }
    else
    {
        STL_TRY_THROW( aSrcLength >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );
    }

    if( aSrcLength > 0 )
    {
        if( aDestLength > 0 )
        {
            sBufferLength = aDestLength - 1;
        }
        
        STL_TRY( dtlUnicodeToMbConversion( sEncoding,
                                           aCharacterset,
                                           (void*)aSrc,
                                           aSrcLength * STL_SIZEOF(SQLWCHAR),
                                           (stlChar*)aDest,
                                           sBufferLength,
                                           &sOffset,
                                           &sLength,
                                           aErrorStack ) == STL_SUCCESS );
        
        STL_DASSERT( sLength < aDestLength );
    }

    if( (aDest != NULL) && (aDestLength > 0) )
    {
        aDest[sLength] = '\0';
    }

    if( aTotalLength != NULL )
    {
        *aTotalLength = sLength;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_STRING_OR_BUFFER_LENGTH,
                      "The value of one of the length arguments was less than 0 "
                      "but not equal to SQL_NTS.",
                      aErrorStack );

    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlvCharToWchar( dtlCharacterSet    aCharacterset,
                          SQLCHAR          * aSrc,
                          stlInt64           aSrcLength,
                          SQLWCHAR         * aDest,
                          stlInt64           aDestLength,
                          stlInt64         * aTotalLength,
                          stlErrorStack    * aErrorStack )
{
    stlInt64           sLength = 0;
    dtlUnicodeEncoding sEncoding;
    stlInt64           sOffset = 0;

    stlInt64           sBufferLength = 0;
    stlInt64           sNumChars = 0;
    
#ifdef SQL_WCHART_CONVERT
    sEncoding = DTL_UNICODE_UTF32;
#else
    sEncoding = DTL_UNICODE_UTF16;
#endif
    
    if( aSrcLength == STL_NTS )
    {
        if( aSrc != NULL )
        {
            aSrcLength = stlStrlen((stlChar*)aSrc);
        }
    }
    else
    {
        STL_TRY_THROW( aSrcLength >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );
    }

    if( aSrcLength > 0 )
    {
        if( aDestLength > 0 )
        {
            sBufferLength = (aDestLength - 1) * STL_SIZEOF(SQLWCHAR);
        }
        
        STL_TRY( dtlMbToUnicodeConversion( aCharacterset,
                                           sEncoding,
                                           (stlChar*)aSrc,
                                           aSrcLength,
                                           (void*)aDest,
                                           sBufferLength,
                                           &sOffset,
                                           &sLength,
                                           aErrorStack ) == STL_SUCCESS );
        
        sNumChars = sLength / STL_SIZEOF(SQLWCHAR);
    }

    if( (aDest != NULL) && (aDestLength > 0) )
    {
        if( sLength > sBufferLength )
        {
            aDest[aDestLength - 1] = 0;
        }
        else
        {
            aDest[sNumChars] = 0;
        }
    }
    
    if( aTotalLength != NULL )
    {
        *aTotalLength = sNumChars;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_STRING_OR_BUFFER_LENGTH,
                      "The value of one of the length arguments was less than 0 "
                      "but not equal to SQL_NTS.",
                      aErrorStack );

    }

    STL_FINISH;

    return STL_FAILURE;    
}

/** @} */
