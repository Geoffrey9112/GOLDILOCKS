/*******************************************************************************
 * stfLockWin.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: stfLockUnix.c 13496 2014-08-29 05:38:43Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stc.h>
#include <stlDef.h>
#include <ste.h>
#include <stlError.h>
#include <stlStrings.h>
#include <stlMemorys.h>

stlStatus stfLock( stlFile        * aFile,
                   stlInt32         aType,
                   stlErrorStack  * aErrorStack )
{
    const DWORD sLen = 0xffffffff;
    DWORD sFlags;
    OVERLAPPED sOffset;

    STL_PARAM_VALIDATE( aFile != NULL, aErrorStack );

    sFlags = ((aType & STL_FLOCK_NONBLOCK) ? LOCKFILE_FAIL_IMMEDIATELY : 0)
        + (((aType & STL_FLOCK_TYPEMASK) == STL_FLOCK_SHARED) ? 0 : LOCKFILE_EXCLUSIVE_LOCK);

    stlMemset(&sOffset, 0, STL_SIZEOF(sOffset));

    STL_TRY_THROW(LockFileEx(&aFile->mFileDesc, sFlags, 0, sLen, sLen, &sOffset) != 0, RAMP_ERR_FILE_LOCK);

    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_FILE_LOCK)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_FILE_LOCK,
                      NULL,
                      aErrorStack );
        steSetSystemError( GetLastError(), aErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus stfUnlock( stlFile        * aFile,
                     stlErrorStack  * aErrorStack )
{
    const DWORD sLen = 0xffffffff;
    OVERLAPPED sOffset;

    STL_PARAM_VALIDATE( aFile != NULL, aErrorStack );

    stlMemset(&sOffset, 0, STL_SIZEOF(sOffset));

    STL_TRY_THROW(UnlockFileEx(aFile->mFileDesc, 0, sLen, sLen, &sOffset) != 0, RAMP_ERR_FILE_UNLOCK);
    
    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_FILE_UNLOCK)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_FILE_UNLOCK,
                      NULL,
                      aErrorStack );
        steSetSystemError( GetLastError(), aErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}
