/*******************************************************************************
 * stbBacktraceUnix.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: stbBacktraceUnix.c 13496 2014-08-29 05:38:43Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stc.h>
#include <stlDef.h>
#include <stlError.h>
#include <stb.h>
#include <stlMemorys.h>
#include <stlFile.h>
#include <stlStrings.h>
#include <DbgHelp.h>

HANDLE gStbProcess = NULL;

inline stlInt32 stbBacktrace( stlFile   * aFile,
                              void     ** aFrame,
                              stlInt32    aSize,
                              void      * aSigInfo,
                              void      * aContext )
{
    if (gStbProcess == NULL)
    {
        gStbProcess = GetCurrentProcess();

        SymInitialize( gStbProcess, NULL, TRUE );
    }

    return CaptureStackBackTrace( 0, aSize, aFrame, NULL);
}

inline void stbBacktraceSymbolsToFile( void     ** aFrame,
                                       stlInt32    aSize,
                                       stlFile   * aFile )
{
    stlChar         sOutputString[1024];
    stlInt32        sCnt;
    stlSize         sWriteBytes;
    stlSize         sWrittenBytes;
    stlErrorStack   sErrorStack;
    SYMBOL_INFO   * sSymbol;
    stlChar         sBuffer[STL_SIZEOF(SYMBOL_INFO) + 256];

    STL_INIT_ERROR_STACK(&sErrorStack);

    sSymbol = (SYMBOL_INFO*)sBuffer;
    sSymbol->MaxNameLen = 255;
    sSymbol->SizeOfStruct = STL_SIZEOF(SYMBOL_INFO);

    for( sCnt = 0; sCnt < aSize; ++sCnt )
    {
        STL_DASSERT( gStbProcess != NULL );

        SymFromAddr( gStbProcess, (DWORD64)(stlVarInt)aFrame[sCnt], 0, sSymbol );

        sWriteBytes = stlSnprintf( sOutputString, 
                                   1024, 
                                   "%s+0x%lx[0x%lx]\n", 
                                   sSymbol->Name,
                                   ((stlUInt64)(stlVarInt)aFrame[sCnt] - (stlUInt64)sSymbol->Address),
                                   aFrame[sCnt] );

        stlWriteFile( aFile,
                      sOutputString,
                      sWriteBytes,
                      &sWrittenBytes,
                      &sErrorStack );
    }
}

inline void stbBacktraceToFile( stlFile    * aFile,
                                void      ** aFrame,
                                stlInt32     aSize,
                                void       * aSigInfo,
                                void       * aContext )
{
    stlChar         sOutputString[1024];
    stlInt32        sCnt;
    stlSize         sWriteBytes;
    stlSize         sWrittenBytes;
    stlErrorStack   sErrorStack;
    stlInt32        sFrameSize;
    SYMBOL_INFO   * sSymbol;
    stlChar         sBuffer[STL_SIZEOF(SYMBOL_INFO) + 256];

    sFrameSize = stbBacktrace( NULL,
                               aFrame,
                               aSize,
                               aSigInfo,
                               aContext );

    STL_INIT_ERROR_STACK(&sErrorStack);

    sSymbol = (SYMBOL_INFO*)sBuffer;
    sSymbol->MaxNameLen = 255;
    sSymbol->SizeOfStruct = STL_SIZEOF(SYMBOL_INFO);

    for( sCnt = 0; sCnt < sFrameSize; ++sCnt )
    {
        STL_DASSERT( gStbProcess != NULL );

        SymFromAddr( gStbProcess, (DWORD64)(stlVarInt)aFrame[sCnt], 0, sSymbol );

        sWriteBytes = stlSnprintf( sOutputString,
                                   1024,
                                   "%s+0x%lx[0x%lx]\n",
                                   sSymbol->Name,
                                   ((stlUInt64)(stlVarInt)aFrame[sCnt] - (stlUInt64)sSymbol->Address),
                                   aFrame[sCnt]);

        stlWriteFile( aFile,
                      sOutputString,
                      sWriteBytes,
                      &sWrittenBytes,
                      &sErrorStack );
    }
}

inline void stbBacktraceSymbolsToStr( void     ** aFrame,
                                      void      * aBuffer,
                                      stlInt32    aFrameSize,
                                      stlInt32    aBufferSize )
{
    stlChar       sOutputString[1024];
    stlInt32      sCnt = 0;
    SYMBOL_INFO * sSymbol;
    stlChar       sBuffer[STL_SIZEOF(SYMBOL_INFO) + 256];

    if( aBufferSize > 0 )
    {
        ((stlChar*)aBuffer)[0] = '\0';
    }

    sSymbol = (SYMBOL_INFO*)sBuffer;
    sSymbol->MaxNameLen = 255;
    sSymbol->SizeOfStruct = STL_SIZEOF(SYMBOL_INFO);

    for( sCnt = 0; sCnt < aFrameSize; ++sCnt )
    {
        STL_DASSERT( gStbProcess != NULL );

        SymFromAddr( gStbProcess, (DWORD64)(stlVarInt)aFrame[sCnt], 0, sSymbol );

        stlSnprintf( sOutputString,
                     1024,
                     "%s+0x%lx[0x%lx]\n",
                     sSymbol->Name,
                     ((stlUInt64)(stlVarInt)aFrame[sCnt] - (stlUInt64)sSymbol->Address),
                     aFrame[sCnt]);
        
        stlStrnncat( aBuffer, sOutputString, aBufferSize, STL_UINT32_MAX );
    }
}
