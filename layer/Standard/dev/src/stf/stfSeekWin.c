/*******************************************************************************
 * stfSeekWin.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: stfSeekUnix.c 13496 2014-08-29 05:38:43Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stlDef.h>
#include <ste.h>
#include <stlError.h>
#include <stlStrings.h>

stlStatus stfSeek( stlFile        * aFile,
                   stlInt32         aWhere,
                   stlOffset      * aOffset,
                   stlErrorStack  * aErrorStack )
{
    DWORD sHowMove;
    DWORD sOffsetLo;
    DWORD sOffsetHi;

    STL_PARAM_VALIDATE( aFile != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aOffset != NULL, aErrorStack );

    sOffsetLo = (DWORD)*aOffset;
    sOffsetHi = (DWORD)(*aOffset >> 32);
	
    switch (aWhere)
    {
	case STL_FSEEK_SET:
            sHowMove = FILE_BEGIN;
            break;
	case STL_FSEEK_CUR:
            sHowMove = FILE_CURRENT;
            break;
	case STL_FSEEK_END:
            sHowMove = FILE_END;
            break;
	default:
            STL_THROW(RAMP_ERR_INVALID_ARGUMENT);
            break;
    }

    sOffsetLo = SetFilePointer(aFile->mFileDesc, (LONG)sOffsetLo, (LONG*)&sOffsetHi, sHowMove);

    STL_TRY_THROW(sOffsetLo != 0xFFFFFFFF, RAMP_ERR_FILE_SEEK);

    *aOffset = ((stlOffset)sOffsetHi << 32) | sOffsetLo;
    
    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_INVALID_ARGUMENT)
    {
        stlPushError(STL_ERROR_LEVEL_ABORT,
                     STL_ERRCODE_INVALID_ARGUMENT,
                     NULL,
                     aErrorStack);
    }

    STL_CATCH(RAMP_ERR_FILE_SEEK)
    {
        stlPushError(STL_ERROR_LEVEL_ABORT,
                     STL_ERRCODE_FILE_SEEK,
                     NULL,
                     aErrorStack);

        steSetSystemError(GetLastError(), aErrorStack);
    }

    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus stfTruncate( stlFile       * aFile,
                       stlOffset       aOffset,
                       stlErrorStack * aErrorStack )
{
    DWORD sOffsetLo;
    DWORD sOffsetHi;
    DWORD sRet;

    STL_PARAM_VALIDATE( aFile != NULL, aErrorStack );

    sOffsetLo = (DWORD)aOffset;
    sOffsetHi = (DWORD)(aOffset >> 32);

    sRet = SetFilePointer(aFile->mFileDesc, (LONG)sOffsetLo, (LONG*)&sOffsetHi, FILE_BEGIN);

    STL_TRY_THROW(sRet != 0xFFFFFFFF, RAMP_ERR_FTRUNCATE);

    sRet = SetEndOfFile(aFile->mFileDesc);

    STL_TRY_THROW(sRet != 0, RAMP_ERR_FTRUNCATE);

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_FTRUNCATE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_FILE_TRUNCATE,
                      NULL,
                      aErrorStack );
        steSetSystemError( GetLastError(), aErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}

