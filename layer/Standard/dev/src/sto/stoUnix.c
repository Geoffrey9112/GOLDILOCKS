/*******************************************************************************
 * stoUnix.c
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

#include <stc.h>
#include <stlTypes.h>
#include <ste.h>
#include <stlError.h>

stlStatus stoOpen( stlChar        * aLibraryPath,
                   stlDsoHandle   * aLibraryHandle,
                   stlErrorStack  * aErrorStack )
{
    *aLibraryHandle = dlopen( aLibraryPath, RTLD_LAZY );

    STL_TRY_THROW( *aLibraryHandle != NULL, RAMP_ERR_OPEN );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_OPEN )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_OPEN_LIBRARY,
                      dlerror(),
                      aErrorStack,
                      aLibraryPath );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stoClose( stlDsoHandle    aLibraryHandle,
                    stlErrorStack * aErrorStack )
{
    STL_TRY_THROW( dlclose( aLibraryHandle ) == 0, RAMP_ERR_CLOSE );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CLOSE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_CLOSE_LIBRARY,
                      dlerror(),
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stoGetSymbol( stlDsoHandle      aLibraryHandle,
                        stlChar         * aSymbolName,
                        stlDsoSymHandle * aSymbolAddr,
                        stlErrorStack   * aErrorStack )
{
    void * sSymbolAddr;
    
    sSymbolAddr = dlsym( aLibraryHandle, aSymbolName );

    STL_TRY_THROW( sSymbolAddr != NULL, RAMP_ERR_GET_SYMBOL );

    *aSymbolAddr = sSymbolAddr;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_GET_SYMBOL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_GET_SYMBOL,
                      dlerror(),
                      aErrorStack,
                      aSymbolName );
    }

    STL_FINISH;

    return STL_FAILURE;
}

