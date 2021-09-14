/*******************************************************************************
 * stfOpenWin.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: stfOpenUnix.c 13496 2014-08-29 05:38:43Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stc.h>
#include <stlDef.h>
#include <ste.h>
#include <stlStrings.h>
#include <stlError.h>
#include <stlLog.h>

#include <io.h>

#define F_OK 0

stlStatus stfOpen( stlFile        * aNewFile, 
                   const stlChar  * aFileName, 
                   stlInt32         aFlag, 
                   stlInt32         aPerm,
                   stlErrorStack  * aErrorStack )
{    
    HANDLE sFileDesc = INVALID_HANDLE_VALUE;
    DWORD  sFlags = 0;
    DWORD  sCreateFlags = 0;
    DWORD  sAttributes = 0;
    DWORD  sShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;

    STL_PARAM_VALIDATE( aNewFile != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aFileName != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aErrorStack != NULL, aErrorStack );

    if (aFlag & STL_FOPEN_APPEND)
    {
        sFlags |= FILE_APPEND_DATA;
    }
    else
    {
        if (aFlag & STL_FOPEN_READ)
        {
            sFlags |= GENERIC_READ;
        }
        if (aFlag & STL_FOPEN_WRITE)
        {
            sFlags |= GENERIC_WRITE;
        }
    }

    if (aFlag & STL_FOPEN_CREATE)
    {
        if (aFlag & STL_FOPEN_EXCL)
        {
            /* only create new if file does not already exist */
            sCreateFlags = CREATE_NEW;
        }
        else if (aFlag & STL_FOPEN_TRUNCATE)
        {
            /* truncate existing file or create new */
            sCreateFlags = CREATE_ALWAYS;
        }
        else
        {
            /* open existing but create if necessary */
            sCreateFlags = OPEN_ALWAYS;
        }
    }
    else if (aFlag & STL_FOPEN_TRUNCATE)
    {
        /* only truncate if file already exists */
        sCreateFlags = TRUNCATE_EXISTING;
    }
    else
    {
        /* only open if file already exists */
        sCreateFlags = OPEN_EXISTING;
    }

    if ((aFlag & STL_FOPEN_EXCL) && !(aFlag & STL_FOPEN_CREATE))
    {
        STL_THROW(RAMP_ERR_EACCES);
    }

    sFileDesc = CreateFile( aFileName,
                            sFlags,
                            sShareMode,
                            NULL,
                            sCreateFlags,
                            sAttributes, 0);

    STL_TRY_THROW(sFileDesc != INVALID_HANDLE_VALUE, RAMP_ERR_FILE_OPEN);

    aNewFile->mFileDesc = sFileDesc;

    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_EACCES)
    {
        stlPushError(STL_ERROR_LEVEL_ABORT,
                     STL_ERRCODE_ACCESS_FILE,
                     NULL,
                     aErrorStack,
                     aFileName);
        steSetSystemError(GetLastError(), aErrorStack);
    }

    STL_CATCH(RAMP_ERR_FILE_OPEN)
    {
        stlPushError(STL_ERROR_LEVEL_ABORT,
                     STL_ERRCODE_FILE_OPEN,
                     NULL,
                     aErrorStack,
                     aFileName);
        steSetSystemError(GetLastError(), aErrorStack);
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
    
    sReturn = _access( aFileName, F_OK );

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
    STL_PARAM_VALIDATE( aFile != NULL, aErrorStack );

    if (aFile->mFileDesc != INVALID_HANDLE_VALUE)
    {
        STL_TRY_THROW(CloseHandle(aFile->mFileDesc) != 0, RAMP_ERR_FILE_CLOSE);

        aFile->mFileDesc = INVALID_HANDLE_VALUE;
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_FILE_CLOSE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_FILE_CLOSE,
                      NULL,
                      aErrorStack );
        steSetSystemError( GetLastError(), aErrorStack );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus stfRemove( const stlChar * aPath,
                     stlErrorStack * aErrorStack )
{
    STL_PARAM_VALIDATE( aPath != NULL, aErrorStack );
    
    STL_TRY_THROW(DeleteFile(aPath) != 0, RAMP_ERR_FILE_REMOVE);
    
    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_FILE_REMOVE)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_FILE_REMOVE,
                      NULL,
                      aErrorStack );
        steSetSystemError( GetLastError(), aErrorStack );
    }
    
    STL_FINISH;
    
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
    STL_TRY_THROW(CreateHardLink(aToPath, aFromPath, NULL) != 0, RAMP_ERR_FILE_LINK);
    
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
    STL_CATCH( RAMP_ERR_FILE_LINK )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_FILE_LINK,
                      NULL,
                      aErrorStack );
        steSetSystemError( GetLastError(), aErrorStack );
    }    
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus stfOpenStdin( stlFile       * aFile,
                        stlErrorStack * aErrorStack )
{
    HANDLE sHandle = INVALID_HANDLE_VALUE;

    sHandle = GetStdHandle(STD_INPUT_HANDLE);

    STL_TRY_THROW(sHandle != INVALID_HANDLE_VALUE, RAMP_ERR_FILE_OPEN);

    aFile->mFileDesc = sHandle;
    
    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_FILE_OPEN)
    {
        stlPushError(STL_ERROR_LEVEL_ABORT,
                     STL_ERRCODE_FILE_OPEN,
                     NULL,
                     aErrorStack,
                     "The standard input device");
        steSetSystemError(GetLastError(), aErrorStack);
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stfOpenStdout( stlFile       * aFile,
                         stlErrorStack * aErrorStack )
{
    HANDLE sHandle = INVALID_HANDLE_VALUE;

    sHandle = GetStdHandle(STD_OUTPUT_HANDLE);

    STL_TRY_THROW(sHandle != INVALID_HANDLE_VALUE, RAMP_ERR_FILE_OPEN);

    aFile->mFileDesc = sHandle;

    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_FILE_OPEN)
    {
        stlPushError(STL_ERROR_LEVEL_ABORT,
                     STL_ERRCODE_FILE_OPEN,
                     NULL,
                     aErrorStack,
                     "The standard output device");
        steSetSystemError(GetLastError(), aErrorStack);
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stfOpenStderr( stlFile       * aFile,
                         stlErrorStack * aErrorStack )
{
    HANDLE sHandle = INVALID_HANDLE_VALUE;

    sHandle = GetStdHandle(STD_ERROR_HANDLE);

    STL_TRY_THROW(sHandle != INVALID_HANDLE_VALUE, RAMP_ERR_FILE_OPEN);

    aFile->mFileDesc = sHandle;

    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_FILE_OPEN)
    {
        stlPushError(STL_ERROR_LEVEL_ABORT,
                     STL_ERRCODE_FILE_OPEN,
                     NULL,
                     aErrorStack,
                     "The standard error device");
        steSetSystemError(GetLastError(), aErrorStack);
    }

    STL_FINISH;

    return STL_FAILURE;
}

