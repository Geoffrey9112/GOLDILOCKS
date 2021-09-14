/*******************************************************************************
 * ste.c
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

#include <stl.h>
#include <stlDef.h>
#include <stlError.h>
#include <ste.h>

void steSetSystemError( stlInt32        aErrorNo,
                        stlErrorStack * aErrorStack )
{
    stlErrorData   * sErrorData;
    
    sErrorData = &aErrorStack->mErrorData[aErrorStack->mTop];
    sErrorData->mSystemErrorCode = aErrorNo;
}

void steDefaultFatalHandler( const stlChar * aCauseString,
                             void          * aSigInfo,
                             void          * aContext )
{
    stlErrorStack sDummyErrorStack;
    stlFile       sErrFile;

    STL_INIT_ERROR_STACK( &sDummyErrorStack );
    
    stlOpenStderrFile( &sErrFile, NULL );

    /*
     * 실패하면 무시한다.
     */
    stlPutStringFile( aCauseString,
                      &sErrFile,
                      &sDummyErrorStack );
    stlPutStringFile( "\n",
                      &sErrFile,
                      &sDummyErrorStack );
    
    stlLogCallstack( aSigInfo, aContext );

#if defined( STL_DEBUG )
    abort();
#else
    stlNativeExitThread();
#endif    
}

void steFatalHandler( stlInt32   aSigNo,
                      void     * aSigInfo,
                      void     * aContext )
{
    stlChar  sBuffer[STL_MAX_ERROR_MESSAGE];

    stlSnprintf( sBuffer,
                 STL_MAX_ERROR_MESSAGE,
                 "SIGNAL(%d) recieved",
                 aSigNo );
    
    gStlFatalHandler( sBuffer, aSigInfo, aContext );
    
    abort();
}

