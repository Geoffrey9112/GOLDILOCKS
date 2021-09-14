/*******************************************************************************
 * stoWin.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: stoUnix.c 13496 2014-08-29 05:38:43Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <ste.h>
#include <stlStrings.h>
#include <stlError.h>

stlStatus stoOpen( stlChar        * aLibraryPath,
                   stlDsoHandle   * aLibraryHandle,
                   stlErrorStack  * aErrorStack )
{
    HMODULE   sHandle;
    stlChar * sPos = aLibraryPath;
    UINT      sErrorMode;
    stlInt32  sState = 0;
    stlSize   sLen = 0;

    sLen = stlStrlen( aLibraryPath );

    while ((sPos = stlStrchr(sPos, '/')) != NULL)
    {
        *sPos = '\\';
    }

    if( (aLibraryPath[0] == '\"') && (aLibraryPath[sLen - 1] == '\"') )
    {
        aLibraryPath[sLen - 1] = '\0';
        aLibraryPath++;
    }

    sErrorMode = SetErrorMode(SEM_FAILCRITICALERRORS);
    sState = 1;

    sHandle = LoadLibraryEx(aLibraryPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

    if (sHandle == NULL)
    {
        sHandle = LoadLibraryEx(aLibraryPath, NULL, 0);

        STL_TRY_THROW(sHandle != NULL, RAMP_ERR_OPEN);
    }

    sState = 0;
    SetErrorMode(sErrorMode);

    *aLibraryHandle = sHandle;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_OPEN )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_OPEN_LIBRARY,
                      NULL,
                      aErrorStack,
                      aLibraryPath );
        steSetSystemError(GetLastError(), aErrorStack);
    }

    STL_FINISH;

    switch (sState)
    {
	case 1:
            SetErrorMode(sErrorMode);
	default:
            break;
    }

    return STL_FAILURE;
}

stlStatus stoClose( stlDsoHandle    aLibraryHandle,
                    stlErrorStack * aErrorStack )
{
    if (aLibraryHandle != NULL)
    {
        STL_TRY_THROW(FreeLibrary(aLibraryHandle) != FALSE, RAMP_ERR_CLOSE);
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CLOSE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_CLOSE_LIBRARY,
                      NULL,
                      aErrorStack );
        steSetSystemError(GetLastError(), aErrorStack);
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stoGetSymbol( stlDsoHandle      aLibraryHandle,
                        stlChar         * aSymbolName,
                        stlDsoSymHandle * aSymbolAddr,
                        stlErrorStack   * aErrorStack )
{
    FARPROC sSymbolAddr;

    sSymbolAddr = GetProcAddress(aLibraryHandle, aSymbolName);
	
    STL_TRY_THROW( sSymbolAddr != NULL, RAMP_ERR_GET_SYMBOL );

    *aSymbolAddr = sSymbolAddr;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_GET_SYMBOL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_GET_SYMBOL,
                      NULL,
                      aErrorStack,
                      aSymbolName );
        steSetSystemError(GetLastError(), aErrorStack);
    }

    STL_FINISH;

    return STL_FAILURE;
}

