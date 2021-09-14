/*******************************************************************************
 * stfPipeWin.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: stfPipeUnix.c 13496 2014-08-29 05:38:43Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stlDef.h>
#include <ste.h>
#include <stlError.h>
#include <stlStrings.h>
#include <stlLog.h>

stlStatus stfCreatePipe( stlFile        * aInFile,
                         stlFile        * aOutFile,
                         stlErrorStack  * aErrorStack )
{
    SECURITY_ATTRIBUTES sSa;

    STL_PARAM_VALIDATE( aInFile != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aOutFile != NULL, aErrorStack );
    
    sSa.nLength = STL_SIZEOF(sSa);
    sSa.bInheritHandle = FALSE;
    sSa.lpSecurityDescriptor = NULL;

    STL_TRY_THROW( CreatePipe( &aInFile->mFileDesc,
                               &aOutFile->mFileDesc,
                               &sSa,
                               65536) != 0, RAMP_ERR_PIPE );
    
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

