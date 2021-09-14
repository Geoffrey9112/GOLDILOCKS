/*******************************************************************************
 * stfDupUnix.c
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

stlStatus stfDuplicate( stlFile        * aNewFile,
                        stlFile        * aOldFile,
                        stlInt32         aOption,
                        stlErrorStack  * aErrorStack )
{
    stlInt32 sFileDesc;

    STL_PARAM_VALIDATE( (aOption == 1) || (aOption == 2), aErrorStack );
    STL_PARAM_VALIDATE( aNewFile != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aOldFile != NULL, aErrorStack );
    
    if( aOption == 2 )
    {
        do
        {
            sFileDesc = dup2( aOldFile->mFileDesc,
                              aNewFile->mFileDesc );
        } while( (sFileDesc == -1) && (errno == EINTR) );
    }
    else
    {
        do
        {
            sFileDesc = dup( aOldFile->mFileDesc );
        } while( (sFileDesc == -1) && (errno == EINTR) );
    }

    STL_TRY_THROW( sFileDesc != -1, RAMP_ERR_DUP );
    
    if( aOption == 1 )
    {
        aNewFile->mFileDesc = sFileDesc;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_DUP )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_FILE_DUP,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}

