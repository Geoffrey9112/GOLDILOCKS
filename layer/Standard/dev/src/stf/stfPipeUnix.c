/*******************************************************************************
 * stfPipeUnix.c
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

stlStatus stfCreatePipe( stlFile        * aInFile,
                         stlFile        * aOutFile,
                         stlErrorStack  * aErrorStack )
{
    stlInt32 sFileDesc[2];

    STL_PARAM_VALIDATE( aInFile != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aOutFile != NULL, aErrorStack );
    
    STL_TRY_THROW( pipe( sFileDesc ) != -1, RAMP_ERR_PIPE );
    
    aInFile->mFileDesc  = sFileDesc[0];
    aOutFile->mFileDesc = sFileDesc[1];
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_PIPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_PIPE,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}

