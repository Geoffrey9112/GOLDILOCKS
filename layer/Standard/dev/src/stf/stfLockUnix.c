/*******************************************************************************
 * stfLockUnix.c
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
#include <stlDef.h>
#include <stfUnix.h>
#include <ste.h>
#include <stlError.h>
#include <stlStrings.h>
#include <stlLog.h>

stlStatus stfLock( stlFile        * aFile,
                   stlInt32         aType,
                   stlErrorStack  * aErrorStack )
{
    int sReturnCode;

    STL_PARAM_VALIDATE( aFile != NULL, aErrorStack );
    
#if defined( STC_HAVE_FCNTL_H )
    
    struct flock sLock = { 0 };
    stlInt32     sCommand;

    sLock.l_whence = SEEK_SET;  /* lock from current point */
    sLock.l_start = 0;          /* begin lock at this offset */
    sLock.l_len = 0;            /* lock to end of file */
        
    if( (aType & STL_FLOCK_TYPEMASK) == STL_FLOCK_SHARED )
    {
        sLock.l_type = F_RDLCK;
    }
    else
    {
        sLock.l_type = F_WRLCK;
    }

    sCommand = (aType & STL_FLOCK_NONBLOCK) ? F_SETLK : F_SETLKW;

    /* keep trying if fcntl() gets interrupted (by a signal) */
    do
    {
        sReturnCode = fcntl( aFile->mFileDesc, sCommand, &sLock );
    } while( (sReturnCode < 0) && (errno == EINTR) );

#elif defined( STC_HAVE_SYS_FILE_H )

    stlInt32 sLockType;

    if( (aType & STL_FLOCK_TYPEMASK) == STL_FLOCK_SHARED )
    {
        sLockType = LOCK_SH;
    }
    else
    {
        sLockType = LOCK_EX;
    }
        
    if( (aType & STL_FLOCK_NONBLOCK) != 0 )
    {
        sLockType |= LOCK_NB;
    }

    /* keep trying if flock() gets interrupted (by a signal) */
    do
    {
        sReturnCode = flock( aFile->mFileDesc, sLockType );
    } while( (sReturnCode < 0) && (errno == EINTR) );

#else
#error No file locking mechanism is available.
#endif

    STL_TRY_THROW( sReturnCode != -1, RAMP_ERR );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR )
    {
        switch( errno )
        {
            case EACCES:
            case EAGAIN:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_AGAIN,
                              NULL,
                              aErrorStack );
                break;
            default:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_FILE_LOCK,
                              NULL,
                              aErrorStack );
                break;
        }
        steSetSystemError( errno, aErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus stfUnlock( stlFile        * aFile,
                     stlErrorStack  * aErrorStack )
{
    stlInt32 sReturnCode;

    STL_PARAM_VALIDATE( aFile != NULL, aErrorStack );
    
#if defined( STC_HAVE_FCNTL_H )
    
    struct flock sLock = { 0 };

    sLock.l_whence = SEEK_SET;  /* lock from current point */
    sLock.l_start = 0;          /* begin lock at this offset */
    sLock.l_len = 0;            /* lock to end of file */
    sLock.l_type = F_UNLCK;

    /* keep trying if fcntl() gets interrupted (by a signal) */
    do
    {
        sReturnCode = fcntl( aFile->mFileDesc, F_SETLKW, &sLock );
    } while( (sReturnCode < 0) && (errno == EINTR) );

    STL_TRY_THROW( sReturnCode != -1, RAMP_ERR );
    
#elif defined( STC_HAVE_SYS_FILE_H )
    
    /* keep trying if flock() gets interrupted (by a signal) */
    do
    {
        sReturnCode = flock( aFile->mFileDesc, LOCK_UN );
    } while( (sReturnCode < 0) && (errno == EINTR) );
        
    STL_TRY_THROW( sReturnCode != -1, RAMP_ERR );
    
#else
#error No file locking mechanism is available.
#endif
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_FILE_UNLOCK,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}
