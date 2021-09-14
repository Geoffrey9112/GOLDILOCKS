/*******************************************************************************
 * strVprintf.c
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

#include <str.h>
#include <stlFile.h>

#define STR_OUTPUT_BUF_SIZE 1000

void strFlushFunc( strVformatterBuffer * aVBuffer )
{
    stlFile       sFile;
    stlErrorStack sDummyErrorStack;

    STL_INIT_ERROR_STACK( &sDummyErrorStack );

    stlOpenStdoutFile( &sFile, &sDummyErrorStack );
    
    *(aVBuffer->mCurrentPosition) = STR_NULCHAR;
    stlPutStringFile( aVBuffer->mStartPosition,
                      &sFile,
                      &sDummyErrorStack );
    aVBuffer->mCurrentPosition = aVBuffer->mStartPosition;
}

stlInt32 strVprintf( const stlChar * aFormat,
                     va_list         aVarArgList )
{
    stlInt32             sCharacterCount;
    strVformatterBuffer  sVBuffer;
    stlChar              sOutputBuffer[STR_OUTPUT_BUF_SIZE + 1];
    stlSize              sOutputBufferLength = STR_OUTPUT_BUF_SIZE;
                    
    /* save one byte for nul terminator */
    sVBuffer.mStartPosition = sOutputBuffer;
    sVBuffer.mCurrentPosition = sOutputBuffer;
    sVBuffer.mEndPosition = sOutputBuffer + sOutputBufferLength - 1;

    sCharacterCount = strVformatter( strFlushFunc,
                                     &sVBuffer,
                                     (stlChar *)aFormat,
                                     aVarArgList );

    strFlushFunc( &sVBuffer );
    
    return ( sCharacterCount == -1 ) ? (stlInt32)sOutputBufferLength - 1 : sCharacterCount;
}
