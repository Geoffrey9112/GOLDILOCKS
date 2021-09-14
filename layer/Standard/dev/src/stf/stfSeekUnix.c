/*******************************************************************************
 * stfSeekUnix.c
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

#include <stlDef.h>
#include <stfUnix.h>
#include <ste.h>
#include <stlError.h>
#include <stlStrings.h>
#include <stlLog.h>

stlStatus stfSeek( stlFile        * aFile,
                   stlInt32         aWhere,
                   stlOffset      * aOffset,
                   stlErrorStack  * aErrorStack )
{
    stlOffset sOffset;

    STL_PARAM_VALIDATE( aFile != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aOffset != NULL, aErrorStack );

#ifdef STC_TARGET_OS_HPUX
    sOffset = lseek64( aFile->mFileDesc, *aOffset, aWhere );
#else
    sOffset = lseek( aFile->mFileDesc, *aOffset, aWhere );
#endif

    STL_TRY( sOffset != -1 );

    *aOffset = sOffset;
    
    return STL_SUCCESS;

    STL_FINISH
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_FILE_SEEK,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    
    return STL_FAILURE;
}

stlStatus stfTruncate( stlFile       * aFile,
                       stlOffset       aOffset,
                       stlErrorStack * aErrorStack )
{
    stlInt32  sRet;
    stlOffset sOffset = aOffset;

    STL_PARAM_VALIDATE( aFile != NULL, aErrorStack );

    do
    {
        sRet = ftruncate( aFile->mFileDesc, aOffset );
    } while( (sRet == -1) && (errno == EINTR) );
    
    STL_TRY_THROW( sRet  != -1,
                   RAMP_ERR_FTRUNCATE );

    STL_TRY( stfSeek( aFile,
                      STL_FSEEK_SET,
                      &sOffset,
                      aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_FTRUNCATE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_FILE_TRUNCATE,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}

