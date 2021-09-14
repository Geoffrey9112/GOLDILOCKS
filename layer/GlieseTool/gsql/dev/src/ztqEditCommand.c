/*******************************************************************************
 * ztqEditCommand.c
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

#include <dtl.h>
#include <goldilocks.h>
#include <ztqDef.h>
#include <ztqCommand.h>
#include <ztqQueryHistory.h>
#include <ztqEditCommand.h>
#include <ztqMain.h>
#include <ztqDisplay.h>

/**
 * @file ztqEditCommand.c
 * @brief Edit Command Routines
 */

/**
 * @addtogroup ztqCommand
 * @{
 */

stlStatus ztqEditCommand( stlChar       *aEditorName,
                          stlChar       *aSqlFilePath,
                          stlInt64       aHistoryNum,
                          stlAllocator  *aAllocator,
                          stlErrorStack *aErrorStack )
{
    stlChar   *sArgv[3];
#ifndef WIN32
    stlProc    sProc;
    stlBool    sIsChild;
#endif
    stlInt32   sExitCode = 0;
    stlChar   *sSqlBuffer = NULL;
    stlInt32   sState = 0;
    stlChar   *sCommand = NULL;
    stlChar   *sSqlFilePath = NULL;

    if( aSqlFilePath != NULL )
    {
        sSqlFilePath = aSqlFilePath;
    }
    else
    {
        sSqlFilePath = ZTQ_EDIT_BUF_FILE;

        STL_TRY( ztqGetCommandFromHistoryByNum( aHistoryNum,
                                                &sCommand )
                 == STL_SUCCESS );

        if( sCommand != NULL )
        {
            STL_TRY( ztqMakeEditBuferFile( sSqlFilePath,
                                           sCommand,
                                           aErrorStack )
                     == STL_SUCCESS );
        }
    }

    sArgv[0] = aEditorName;
    sArgv[1] = sSqlFilePath;
    sArgv[2] = NULL;

#ifdef WIN32
    STL_TRY( stlCreateProc( sArgv[0],
                            sArgv,
                            &sExitCode,
                            aErrorStack ) == STL_SUCCESS );
#else
    STL_TRY( stlForkProc( &sProc,
                          &sIsChild,
                          aErrorStack )
             == STL_SUCCESS );

    if( sIsChild == STL_TRUE )
    {
        if( stlExecuteProc( sArgv[0],
                            sArgv,
                            aErrorStack )
            != STL_SUCCESS )
        {
            /*
             * Editor 실행 명령이 실패한 경우에는,
             * 에러 메시지를 출력하고, fork된 child process를 종료한다.
             */

            if( stlGetErrorStackDepth( aErrorStack ) > 0 )
            {
                ztqPrintErrorStack( aErrorStack );
            }

            stlExitProc( -1 );
        }
    }
    else
    {
        STL_TRY( stlWaitProc( &sProc,
                              &sExitCode,
                              aErrorStack )
                 == STL_SUCCESS );
    }
#endif
    
    if( sExitCode == 0 )
    {
        STL_TRY( ztqGetEditedSql( sSqlFilePath,
                                  &sSqlBuffer,
                                  aAllocator,
                                  aErrorStack )
                 == STL_SUCCESS );
        sState = 1;

        ztqRemoveTrailingSpace( sSqlBuffer );
        STL_TRY( ztqAddHistory( sSqlBuffer,
                                aErrorStack ) == STL_SUCCESS );

        ztqPrintf( "%s", sSqlBuffer );

        sState = 0;
        ztqReleaseEditedSql( sSqlBuffer );
    }

    if( aSqlFilePath == NULL )
    {
        STL_TRY( stlRemoveFile( sSqlFilePath,
                                aErrorStack )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            ztqReleaseEditedSql( sSqlBuffer );
            break;
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus ztqMakeEditBuferFile( stlChar       *aSqlFilePath,
                                stlChar       *aCommand,
                                stlErrorStack *aErrorStack )
{
    stlFile      sSqlFileHandle;
    stlSize      sStringLen;
    stlSize      sWrittenBytes;
    stlInt32     sState = 0;

    STL_TRY( stlOpenFile( &sSqlFileHandle,
                          aSqlFilePath,
                          STL_FOPEN_WRITE | STL_FOPEN_CREATE | STL_FOPEN_TRUNCATE,
                          STL_FPERM_OS_DEFAULT,
                          aErrorStack )
             == STL_SUCCESS );
    sState = 1;

    sStringLen = stlStrlen( aCommand );

    STL_TRY( stlWriteFile( &sSqlFileHandle,
                           aCommand,
                           sStringLen,
                           &sWrittenBytes,
                           aErrorStack )
             == STL_SUCCESS );

    sState = 0;
    STL_TRY( stlCloseFile( &sSqlFileHandle,
                           aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void)stlCloseFile( &sSqlFileHandle, aErrorStack );
            break;
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus ztqRemoveTrailingSpace( stlChar     *aSqlBuffer )
{
    stlInt32     sBufIdx;

    sBufIdx = stlStrlen( aSqlBuffer );
    sBufIdx --;

    while( (sBufIdx >= 0) && (stlIsspace( aSqlBuffer[sBufIdx] ) != 0) )
    {
        aSqlBuffer[sBufIdx] = '\0';
        sBufIdx --;
    }

    return STL_SUCCESS;
}

stlStatus ztqGetEditedSql( stlChar       *aSqlFilePath,
                           stlChar      **aSqlBuffer,
                           stlAllocator  *aAllocator,
                           stlErrorStack *aErrorStack )
{
    stlFile      sSqlFileHandle;
    stlChar     *sSqlBuffer = NULL;
    stlFileInfo  sFileInfo;
    stlSize      sBytesRead;
    stlInt32     sState = 0;

    STL_TRY( stlOpenFile( &sSqlFileHandle,
                          aSqlFilePath,
                          STL_FOPEN_READ,
                          STL_FPERM_OS_DEFAULT,
                          aErrorStack )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( stlGetFileStatByHandle( &sFileInfo,
                                     STL_FINFO_MIN,
                                     &sSqlFileHandle,
                                     aErrorStack )
             == STL_SUCCESS );

    STL_TRY( stlAllocHeap( (void **)&sSqlBuffer,
                           sFileInfo.mSize + 1,
                           aErrorStack )
             == STL_SUCCESS );
    sState = 2;

    STL_TRY( stlReadFile( &sSqlFileHandle,
                          sSqlBuffer,
                          sFileInfo.mSize,
                          &sBytesRead,
                          aErrorStack )
             == STL_SUCCESS );

    sSqlBuffer[sBytesRead] = '\0';
    *aSqlBuffer = sSqlBuffer;

    sState = 0;
    STL_TRY( stlCloseFile( &sSqlFileHandle,
                           aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            stlFreeHeap( sSqlBuffer );
        case 1:
            (void)stlCloseFile( &sSqlFileHandle, aErrorStack );
            break;
        default:
            break;
    }

    return STL_FAILURE;
}

void ztqReleaseEditedSql( stlChar  *aSqlBuffer )
{
    stlFreeHeap( aSqlBuffer );
}



/** @} */
