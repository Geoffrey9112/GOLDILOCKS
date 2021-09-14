/*******************************************************************************
 * stfReadWriteWin.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: stfReadWriteUnix.c 13496 2014-08-29 05:38:43Z leekmo $
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

stlStatus stfRead( stlFile       * aFile,
                   void          * aBuffer,
                   stlSize         aBytes,
                   stlSize       * aReadBytes,
                   stlErrorStack * aErrorStack )
{
    DWORD    sReadBytes;
    DWORD    sTotalReadBytes = 0;
    stlSsize sBytes = (stlSsize)aBytes;

    STL_PARAM_VALIDATE( aFile != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aBuffer != NULL, aErrorStack );
    
    STL_DASSERT(sBytes < STL_UINT32_MAX);

    do
    {
        sReadBytes = 0;

        STL_TRY_THROW( ReadFile( aFile->mFileDesc,
                                 (stlChar*)aBuffer + sTotalReadBytes,
                                 (DWORD)sBytes,
                                 &sReadBytes,
                                 NULL ) != 0, RAMP_ERR_FILE_READ );

        /* Check for eof. */
        if( sReadBytes == 0 )
        {
            /* at the end of the file */
            if (sTotalReadBytes == 0)
            {
                if (aReadBytes != NULL)
                {
                    *aReadBytes = sTotalReadBytes;
                }

                STL_THROW(RAMP_ERR_EOF);
            }
            else
            {
                STL_THROW(RAMP_SKIP_READ);
            }
        }
    
        sTotalReadBytes += sReadBytes;
        sBytes -= sReadBytes;
        
    } while (sBytes > 0);

    STL_RAMP( RAMP_SKIP_READ );

    if( aReadBytes != NULL )
    {
        *aReadBytes = sTotalReadBytes;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_EOF )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_EOF,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    
    STL_CATCH(RAMP_ERR_FILE_READ)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_FILE_READ,
                      NULL,
                      aErrorStack );
        steSetSystemError( GetLastError(), aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stfWrite( stlFile       * aFile,
                    void          * aBuffer,
                    stlSize         aBytes,
                    stlSize       * aWrittenBytes,
                    stlErrorStack * aErrorStack )
{
    DWORD    sTotalWrite = 0;
    DWORD    sPartialWrite;
    stlSsize sBytes = (stlSsize)aBytes;

    STL_PARAM_VALIDATE( aFile != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aBuffer != NULL, aErrorStack );

    STL_DASSERT(sBytes < STL_UINT32_MAX);
    
    do
    {
        sPartialWrite = 0;

        STL_TRY_THROW(WriteFile(aFile->mFileDesc, (stlChar*)aBuffer + sTotalWrite, (DWORD)sBytes, &sPartialWrite, NULL) != 0,
                      RAMP_ERR_FILE_WRITE);
    
        sTotalWrite += sPartialWrite;
        sBytes -= sPartialWrite;
    } while (sBytes > 0);

    if( aWrittenBytes != NULL )
    {
        *aWrittenBytes = sTotalWrite;
    }

    return STL_SUCCESS;
    
    STL_CATCH(RAMP_ERR_FILE_WRITE)
    {
        if (aWrittenBytes != NULL)
        {
            *aWrittenBytes = 0;
        }

        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_FILE_WRITE,
                      NULL,
                      aErrorStack );
        steSetSystemError( GetLastError(), aErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus stfPutCharacter( stlChar         aCharacter,
                           stlFile       * aFile,
                           stlErrorStack * aErrorStack )
{
    stlSize sBytes = 1;

    STL_PARAM_VALIDATE( aFile != NULL, aErrorStack );
    
    STL_TRY( stfWrite( aFile,
                       &aCharacter,
                       sBytes,
                       NULL,
                       aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus stfGetCharacter( stlChar       * aCharacter,
                           stlFile       * aFile,
                           stlErrorStack * aErrorStack )
{
    stlSize sBytes = 1;

    STL_PARAM_VALIDATE( aCharacter != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aFile != NULL, aErrorStack );
    
    STL_TRY( stfRead( aFile,
                      aCharacter,
                      sBytes,
                      NULL,
                      aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus stfPutString( const stlChar * aString,
                        stlFile       * aFile,
                        stlErrorStack * aErrorStack )
{
    stlSize sBytes;

    STL_PARAM_VALIDATE( aString != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aFile != NULL, aErrorStack );
    
    sBytes = strlen( aString );

    STL_TRY( stfWrite( aFile,
                       (void*)aString,
                       sBytes,
                       NULL,
                       aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus stfGetString( stlChar       * aString,
                        stlInt32        aLength,
                        stlFile       * aFile,
                        stlErrorStack * aErrorStack )
{
    stlUInt32 i = 0;
    stlStatus sStatus;

    STL_PARAM_VALIDATE( aString != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aFile != NULL, aErrorStack );
    
    STL_TRY_THROW( aLength >= 1, RAMP_SUCCESS );
    STL_TRY_THROW( aLength != 1, RAMP_SKIP_READ_CHARACTER );

    while( i < aLength - 1 ) /* leave room for trailing '\0' */
    { 
        sStatus = stfGetCharacter( &aString[i],
                                   aFile,
                                   aErrorStack );

        if( sStatus == STL_FAILURE )
        {
            STL_TRY( stlGetLastErrorCode( aErrorStack )
                     == STL_ERRCODE_EOF );

            /*
             * 처음 읽는 경우 STL_ERRCODE_EOF가 발생하다면 해당 에러를
             * 호출자에게 그대로 전달한다.
             */
            STL_TRY( i != 0 );

            /*
             * STL_ERRCODE_EOF이지만 성공해야 하는 경우는 에러를
             * 에러 스택에서 삭제한다.
             */
            (void)stlPopError( aErrorStack );
            
            break;
        }

        if( aString[i] == '\n' )
        {
            i += 1;
            break;
        }
        
        i++;
    }

    STL_RAMP( RAMP_SKIP_READ_CHARACTER );
    
    aString[i] = '\0';

    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus stfSync( stlFile       * aFile,
                   stlErrorStack * aErrorStack )
{
    STL_PARAM_VALIDATE( aFile != NULL, aErrorStack );

    STL_TRY_THROW( FlushFileBuffers( aFile->mFileDesc ) == 0, RAMP_ERR_FSYNC );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_FSYNC )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_FILE_SYNC,
                      NULL,
                      aErrorStack );
        steSetSystemError( GetLastError(), aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

