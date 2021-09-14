/*******************************************************************************
 * stfDupWin.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: stfDupUnix.c 13496 2014-08-29 05:38:43Z leekmo $
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

stlStatus stfDuplicate( stlFile        * aNewFile,
                        stlFile        * aOldFile,
                        stlInt32         aOption,
                        stlErrorStack  * aErrorStack )
{
    HANDLE sFileDesc;
    HANDLE sProc;

    STL_PARAM_VALIDATE( (aOption == 1) || (aOption == 2), aErrorStack );
    STL_PARAM_VALIDATE( aNewFile != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aOldFile != NULL, aErrorStack );
    
    sProc = GetCurrentProcess();

    STL_TRY_THROW(DuplicateHandle(sProc, aOldFile->mFileDesc,
                                  sProc, &sFileDesc, 0, FALSE, DUPLICATE_SAME_ACCESS) != 0, RAMP_ERR_FILE_DUP);

    if (aOption == 2)
    {
        if ((aNewFile->mFileDesc != 0) && (aNewFile->mFileDesc != INVALID_HANDLE_VALUE))
        {
            CloseHandle(aNewFile->mFileDesc);
        }
    }

    aNewFile->mFileDesc = sFileDesc;

    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_FILE_DUP)
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

