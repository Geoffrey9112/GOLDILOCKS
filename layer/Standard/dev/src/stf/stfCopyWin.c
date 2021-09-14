/*******************************************************************************
 * stfCopyWin.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: stfCopyUnix.c 13496 2014-08-29 05:38:43Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stlDef.h>
#include <stf.h>
#include <ste.h>
#include <stlError.h>
#include <stlStrings.h>
#include <stlLog.h>

stlStatus stfTransferContents( const stlChar * aFromPath,
                               const stlChar * aToPath,
                               stlInt32        aFlags,
                               stlInt32        aToPerms,
                               stlChar       * aBuffer,
                               stlInt32        aBufferSize,
                               stlErrorStack * aErrorStack )
{
    stlFile       sSourceFile;
    stlFile       sTargetFile;
    stlFileInfo   sFileInfo;
    stlUInt32     sState = 0;
    stlInt32      sSourceFlag;
    stlInt32      sPerms;
    stlUInt32     sErrorCode = STL_INVALID_ERROR_CODE;
    stlStatus     sStatus;
    stlChar       sBuffer[STF_DEFAULT_BUFSIZE];
    stlChar     * sAlignedBlock;
    stlSize       sBytes;
    stlSize       sReadBytes;
    
    STL_PARAM_VALIDATE( aFromPath != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aToPath != NULL, aErrorStack );

    sSourceFlag = STL_FOPEN_READ;
    sSourceFlag |= ( aFlags & (STL_FOPEN_DIRECT | STL_FOPEN_LARGEFILE | STL_FOPEN_BINARY) );

    STL_TRY( stfOpen( &sSourceFile,
                      aFromPath,
                      sSourceFlag,
                      STL_FPERM_OS_DEFAULT,
                      aErrorStack ) == STL_SUCCESS );
    sState = 1;

    if( aToPerms == STL_FPERM_FILE_SOURCE )
    {
        STL_TRY( stfGetFileInfo( &sFileInfo,
                                 STL_FINFO_PROT,
                                 &sSourceFile,
                                 aErrorStack ) == STL_SUCCESS );
        sPerms = sFileInfo.mProtection;
    }
    else
    {
        sPerms = aToPerms;
    }

    STL_TRY( stfOpen( &sTargetFile,
                      aToPath,
                      aFlags,
                      sPerms,
                      aErrorStack ) == STL_SUCCESS );
    sState = 2;

    if( aBuffer == NULL )
    {
        if( aFlags & STL_FOPEN_DIRECT )
        {
            sAlignedBlock = (stlChar*)STL_ALIGN( STL_INT_FROM_POINTER(sBuffer),
                                                 STF_DIRECTIO_BUFSIZE );
            sBytes = STF_DIRECTIO_BUFSIZE;
        }
        else
        {
            sAlignedBlock = sBuffer;
            sBytes = STF_DEFAULT_BUFSIZE;
        }
    }
    else
    {
        sAlignedBlock = aBuffer;
        sBytes = aBufferSize;
    }

    while( sErrorCode != STL_ERRCODE_EOF )
    {
        sStatus = stfRead( &sSourceFile,
                           sAlignedBlock,
                           sBytes,
                           &sReadBytes,
                           aErrorStack );

        if( sStatus != STL_SUCCESS )
        {
            sErrorCode = stlGetLastErrorCode( aErrorStack );
            STL_TRY( sErrorCode == STL_ERRCODE_EOF );
            
            /*
             * STL_ERRCODE_EOF이지만 성공해야 하는 경우는 에러를
             * 에러 스택에서 삭제한다.
             */
            (void)stlPopError( aErrorStack );
        }
        
        STL_TRY( stfWrite( &sTargetFile,
                           sAlignedBlock,
                           sReadBytes,
                           NULL,
                           aErrorStack ) == STL_SUCCESS );
    }

    /*
     * Direct IO가 아니라면 Sync 한다.
     */
    if( !(aFlags & STL_FOPEN_DIRECT) )
    {
        STL_TRY( stfSync( &sTargetFile,
                          aErrorStack )
                 == STL_SUCCESS );
    }
        
    sState = 1;
    STL_TRY( stfClose( &sTargetFile,
                       aErrorStack ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( stfClose( &sSourceFile,
                       aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH
    {
        switch( sState )
        {
            case 2:
                (void)stfClose( &sTargetFile, aErrorStack );
            case 1:
                (void)stfClose( &sSourceFile, aErrorStack );
                break;
            default:
                break;
        }
    }
    
    return STL_FAILURE;
}

