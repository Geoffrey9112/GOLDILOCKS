/*******************************************************************************
 * stfOpenUnix.c
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
#include <stlStrings.h>
#include <stlError.h>
#include <stlLog.h>

stlStatus stfOpen( stlFile        * aNewFile, 
                   const stlChar  * aFileName, 
                   stlInt32         aFlag, 
                   stlInt32         aPerm,
                   stlErrorStack  * aErrorStack )
{
    stlInt32  sFileDesc;
    stlInt32  sFlags = 0;

    STL_PARAM_VALIDATE( aNewFile != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aFileName != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aErrorStack != NULL, aErrorStack );
    
    if( (aFlag & STL_FOPEN_READ) && (aFlag & STL_FOPEN_WRITE) )
    {
        sFlags = O_RDWR;
    }
    else
    {
        if( aFlag & STL_FOPEN_READ )
        {
            sFlags = O_RDONLY;
        }
        else
        {
            if( aFlag & STL_FOPEN_WRITE )
            {
                sFlags = O_WRONLY;
            }
            else
            {
                STL_THROW( RAMP_ERR_EACCES );
            }
        }
    }
    
    if( aFlag & STL_FOPEN_CREATE )
    {
        sFlags |= O_CREAT;
        
        if( aFlag & STL_FOPEN_EXCL )
        {
            sFlags |= O_EXCL;
        }
    }

    STL_TRY_THROW( !(aFlag & STL_FOPEN_EXCL) || (aFlag & STL_FOPEN_CREATE),
                   RAMP_ERR_EACCES );

    if( aFlag & STL_FOPEN_APPEND )
    {
        sFlags |= O_APPEND;
    }
    
    if( aFlag & STL_FOPEN_TRUNCATE )
    {
        sFlags |= O_TRUNC;
    }
    
#ifdef O_DIRECT
    if( aFlag & STL_FOPEN_DIRECT )
    {
        sFlags |= O_DIRECT;
        sFlags |= O_SYNC;
    }
#else
#ifndef STC_TARGET_OS_SOLARIS
    /*
     * STL_FOPEN_DIRECT가 설정되지 않았음을 보장해야 한다.
     */
    STL_PARAM_VALIDATE( (~aFlag) & STL_FOPEN_DIRECT, aErrorStack );
#endif
#endif
#ifdef O_BINARY
    if( aFlag & STL_FOPEN_BINARY )
    {
        sFlags |= O_BINARY;
    }
#endif

#ifdef STC_TARGET_OS_HPUX

    if( aPerm == STL_FPERM_OS_DEFAULT )
    {
        do
        {
            sFileDesc = open64( aFileName, sFlags, 0666 );
        } while( (sFileDesc == -1) && (errno == EINTR) );
    }
    else
    {
        do
        {
            sFileDesc = open64( aFileName, sFlags, stfUnixPerms2Mode( aPerm ) );
        } while( (sFileDesc == -1) && (errno == EINTR) );
    }

#else

#if defined(_LARGEFILE64_SOURCE)
    sFlags |= O_LARGEFILE;
#elif defined(O_LARGEFILE)
    if( aFlag & STL_FOPEN_LARGEFILE )
    {
        sFlags |= O_LARGEFILE;
    }
#endif

    if( aPerm == STL_FPERM_OS_DEFAULT )
    {
        do
        {
            sFileDesc = open( aFileName, sFlags, 0666 );
        } while( (sFileDesc == -1) && (errno == EINTR) );
    }
    else
    {
        do
        {
            sFileDesc = open( aFileName, sFlags, stfUnixPerms2Mode( aPerm ) );
        } while( (sFileDesc == -1) && (errno == EINTR) );
    }

#endif

    STL_TRY_THROW( sFileDesc >= 0, RAMP_ERR_OPEN );
    
#ifdef STC_TARGET_OS_SOLARIS

    if( aFlag & STL_FOPEN_DIRECT )
    {
        STL_TRY_THROW( directio( sFileDesc, DIRECTIO_ON ) == 0, RAMP_ERR_OPEN );
    }

#endif

    aNewFile->mFileDesc = sFileDesc;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_EACCES )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_ACCESS_FILE,
                      NULL,
                      aErrorStack,
                      aFileName );
        steSetSystemError( errno, aErrorStack );
    }
    
    STL_CATCH( RAMP_ERR_OPEN )
    {
        switch( errno )
        {
            case ENFILE:
            case EMFILE:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_INSUFFICIENT_RESOURCE,
                              NULL,
                              aErrorStack );
                break;
            case ENOMEM:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_OUT_OF_MEMORY,
                              NULL,
                              aErrorStack );
                break;
            case EACCES:
            case EPERM:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_ACCESS_FILE,
                              NULL,
                              aErrorStack,
                              aFileName );
                break;
            case ENOSPC:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_DISK_FULL,
                              NULL,
                              aErrorStack );
                break;
            case ENOENT:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_NO_ENTRY,
                              NULL,
                              aErrorStack,
                              aFileName );
                break;
            default:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_FILE_OPEN,
                              NULL,
                              aErrorStack,
                              aFileName );
                break;
        }
        steSetSystemError( errno, aErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus stfExist( const stlChar  * aFileName,
                    stlBool        * aExist,
                    stlErrorStack  * aErrorStack )
{
    stlInt32  sReturn;

    STL_PARAM_VALIDATE( aFileName != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aExist != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aErrorStack != NULL, aErrorStack );
    
    *aExist = STL_FALSE;
    
    sReturn = access( aFileName, F_OK );

    if( sReturn < 0 )
    {
        STL_TRY_THROW( errno == ENOENT, RAMP_ERR_EXIST );
        *aExist = STL_FALSE;
    }
    else
    {
        *aExist = STL_TRUE;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_EXIST )
    {
        switch( errno )
        {
            case EACCES:
            case EPERM:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_ACCESS_FILE,
                              NULL,
                              aErrorStack,
                              aFileName );
                break;
            default:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_FILE_EXIST,
                              NULL,
                              aErrorStack,
                              aFileName );
                break;
        }
        steSetSystemError( errno, aErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus stfClose( stlFile       * aFile,
                    stlErrorStack * aErrorStack )
{
    stlInt32 sRet;
    
    STL_PARAM_VALIDATE( aFile != NULL, aErrorStack );

    do
    {
        sRet = close( aFile->mFileDesc );
    } while( (sRet == -1) && (errno == EINTR) );
    
    STL_TRY_THROW( sRet == 0,
                   RAMP_ERR_FILE_CLOSE );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_FILE_CLOSE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_FILE_CLOSE,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus stfRemove( const stlChar * aPath,
                     stlErrorStack * aErrorStack )
{
    STL_PARAM_VALIDATE( aPath != NULL, aErrorStack );
    
    STL_TRY_THROW( unlink( aPath ) == 0, RAMP_ERR );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR )
    {
        switch( errno )
        {
            case EACCES:
            case EPERM:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_ACCESS_FILE,
                              NULL,
                              aErrorStack,
                              aPath );
                break;
            case ENAMETOOLONG:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_NAMETOOLONG,
                              NULL,
                              aErrorStack,
                              aPath );
                break;
            case ENOENT:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_NO_ENTRY,
                              NULL,
                              aErrorStack,
                              aPath );
                break;
            case ENOMEM:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_OUT_OF_MEMORY,
                              NULL,
                              aErrorStack );
                break;
            default:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_FILE_REMOVE,
                              NULL,
                              aErrorStack );
                break;
        }
        steSetSystemError( errno, aErrorStack );
    }
    
    STL_FINISH
    
    return STL_FAILURE;
}

stlStatus stfLink( const stlChar * aFromPath, 
                   const stlChar * aToPath,
                   stlErrorStack * aErrorStack )
{
    STL_PARAM_VALIDATE( aFromPath != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aToPath != NULL, aErrorStack );

    STL_TRY_THROW( stlStrlen( aFromPath ) < STL_MAX_FILE_PATH_LENGTH,
                   RAMP_ERR_FROM_NAMETOOLONG );
    STL_TRY_THROW( stlStrlen( aToPath ) < STL_MAX_FILE_PATH_LENGTH,
                   RAMP_ERR_TO_NAMETOOLONG );
    STL_TRY_THROW( link( aFromPath, aToPath ) == 0, RAMP_ERR );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_FROM_NAMETOOLONG )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_NAMETOOLONG,
                      NULL,
                      aErrorStack,
                      aFromPath );
    }
    STL_CATCH( RAMP_ERR_TO_NAMETOOLONG )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_NAMETOOLONG,
                      NULL,
                      aErrorStack,
                      aToPath );
    }
    STL_CATCH( RAMP_ERR )
    {
        switch( errno )
        {
            case EACCES:
            case EPERM:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_ACCESS_FILE,
                              NULL,
                              aErrorStack,
                              aToPath );
                break;
            case ENAMETOOLONG:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_NAMETOOLONG,
                              NULL,
                              aErrorStack,
                              "" );
                break;
            case ENOENT:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_NO_ENTRY,
                              NULL,
                              aErrorStack,
                              aFromPath );
                break;
            case ENOSPC:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_DISK_FULL,
                              NULL,
                              aErrorStack );
                break;
            case EMLINK:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_INSUFFICIENT_RESOURCE,
                              NULL,
                              aErrorStack );
                break;
            case ENOMEM:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_OUT_OF_MEMORY,
                              NULL,
                              aErrorStack );
                break;
            default:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_FILE_LINK,
                              NULL,
                              aErrorStack );
                break;
        }
        steSetSystemError( errno, aErrorStack );
    }    
    
    STL_FINISH
    
    return STL_FAILURE;
}

stlStatus stfOpenStdin( stlFile       * aFile,
                        stlErrorStack * aErrorStack )
{
    aFile->mFileDesc = 0;
    
    return STL_SUCCESS;
}

stlStatus stfOpenStdout( stlFile       * aFile,
                         stlErrorStack * aErrorStack )
{
    aFile->mFileDesc = 1;
    
    return STL_SUCCESS;
}

stlStatus stfOpenStderr( stlFile       * aFile,
                         stlErrorStack * aErrorStack )
{
    aFile->mFileDesc = 2;
    
    return STL_SUCCESS;
}

