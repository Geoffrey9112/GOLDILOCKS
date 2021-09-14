/*******************************************************************************
 * ztppExpandFile.c
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

#include <stl.h>
#include <dtl.h>
#include <goldilocks.h>
#include <sqlext.h>
#include <ztpDef.h>
#include <ztppExpandFile.h>
#include <ztpSqlLex.h>
#include <ztpSqlParser.h>
#include <ztppFunc.h>
#include <ztpMisc.h>
#include <ztpuFile.h>

/**
 * @file ztppExpandFile.c
 * @brief Preprocessor expand file (translation phse 3~4)
 */

/**
 * @addtogroup ztppExpandFile
 * @{
 */

stlStatus ztppReadFile(stlFile        *aInFile,
                       stlAllocator   *aAllocator,
                       stlErrorStack  *aErrorStack,
                       stlChar       **aBuffer,
                       stlSize        *aLength)
{
    stlInt32        sState = 0;
    stlFileInfo     sFileInfo;
    stlSize         sBytesRead;
    stlChar        *sBuffer;

    STL_TRY(stlGetFileStatByHandle(&sFileInfo,
                                   STL_FINFO_MIN,
                                   aInFile,
                                   aErrorStack)
            == STL_SUCCESS);

    STL_TRY(stlAllocRegionMem(aAllocator,
                              sFileInfo.mSize + 1,
                              (void **)&sBuffer,
                              aErrorStack)
            == STL_SUCCESS);
    stlMemset( sBuffer, 0x00, sFileInfo.mSize + 1 );

    *aBuffer = sBuffer;

    STL_TRY(stlReadFile(aInFile,
                        *aBuffer,
                        sFileInfo.mSize,
                        &sBytesRead,
                        aErrorStack)
            == STL_SUCCESS);

    STL_TRY(sFileInfo.mSize == sBytesRead);

    *aLength = sBytesRead;
    (*aBuffer)[sBytesRead] = '\0';

    return STL_SUCCESS;

    STL_FINISH;

    switch(sState)
    {
        case 1:
            break;
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus ztppSendStringToOutFile(ztpPPParseParam *aParam,
                                  stlChar         *aStr)
{
    if( aParam->mNeedPrint == STL_TRUE )
    {
        STL_TRY(ztpuSendStringToOutFile(aParam->mOutFile,
                                        aStr,
                                        aParam->mErrorStack)
                == STL_SUCCESS);
    }

    return STL_SUCCESS;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;

    return STL_FAILURE;
}

stlStatus ztppBypassCCode(ztpPPParseParam *aParam)
{
    if( aParam->mNeedPrint == STL_TRUE )
    {
        while( aParam->mBuffer[aParam->mCCodeStartLoc] == '\0' )
        {
            aParam->mCCodeStartLoc ++;
        }

        STL_TRY(ztpuWriteFile(aParam->mOutFile,
                              aParam->mBuffer + aParam->mCCodeStartLoc,
                              aParam->mCCodeEndLoc - aParam->mCCodeStartLoc,
                              aParam->mErrorStack)
                == STL_SUCCESS);
    }

    return STL_SUCCESS;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;

    return STL_FAILURE;
}

stlStatus ztppBypassCCodeByLoc(ztpPPParseParam *aParam,
                               stlInt32         aStartLoc,
                               stlInt32         aEndLoc)
{
    stlChar   *sStartPos;

    if( aParam->mNeedPrint == STL_TRUE )
    {
        /* convert comment mark to other string */
        stlMemset(aParam->mTempBuffer, 0x00, aParam->mFileLength);
        stlMemcpy(aParam->mTempBuffer, aParam->mBuffer + aStartLoc, aEndLoc - aStartLoc);

        sStartPos = aParam->mTempBuffer;
        while((sStartPos = stlStrstr(sStartPos, "/*")) != NULL)
        {
            *(sStartPos + 1) = 'g';
        }

        sStartPos = aParam->mTempBuffer;
        while((sStartPos = stlStrstr(sStartPos, "*/")) != NULL)
        {
            *sStartPos = 'g';
        }

        STL_TRY(ztpuWriteFile(aParam->mOutFile,
                              aParam->mTempBuffer,
                              aEndLoc - aStartLoc,
                              aParam->mErrorStack)
                == STL_SUCCESS);
    }

    return STL_SUCCESS;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;

    return STL_FAILURE;
}

stlStatus ztppExpandHeaderFileInternal(stlAllocator    *aAllocator,
                                       stlErrorStack   *aErrorStack,
                                       stlFile         *aResultFile,
                                       stlChar         *aReadFilePath)
{
    stlInt32           sState = 0;
    ztpConvertContext  sContext;
    stlFile            sInFile;
    stlAllocator       sAllocator;

    STL_TRY( stlCreateRegionAllocator( &sAllocator,
                                       ZTP_REGION_INIT_SIZE,
                                       ZTP_REGION_NEXT_SIZE,
                                       aErrorStack )
              == STL_SUCCESS );
    sState = 1;

    sContext.mAllocator = aAllocator;
    sContext.mErrorStack = aErrorStack;
    sContext.mInFile = NULL;
    sContext.mOutFile = aResultFile;
    sContext.mInFileName = aReadFilePath;
    sContext.mOutFileName = gOutFilePath;
    sContext.mNeedPrint = STL_TRUE;

    STL_TRY(stlOpenFile(&sInFile,
                        aReadFilePath,
                        STL_FOPEN_READ,
                        STL_FPERM_OS_DEFAULT,
                        sContext.mErrorStack) == STL_SUCCESS);
    sContext.mInFile = &sInFile;
    sState = 2;

    STL_TRY(ztppReadFile(sContext.mInFile,
                         sContext.mAllocator,
                         sContext.mErrorStack,
                         &sContext.mBuffer,
                         &sContext.mLength)
            == STL_SUCCESS);

    STL_TRY(ztppParseIt(&sContext)
            == STL_SUCCESS);

    sState = 1;
    STL_TRY(stlCloseFile(sContext.mInFile, sContext.mErrorStack)
            == STL_SUCCESS);

    sState = 0;
    STL_TRY(stlDestroyRegionAllocator(&sAllocator,
                                      aErrorStack)
            == STL_SUCCESS);

    return STL_SUCCESS;

    STL_FINISH;

    switch(sState)
    {
        case 2:
            (void)stlCloseFile(sContext.mInFile, sContext.mErrorStack);
        case 1:
            (void)stlDestroyRegionAllocator(&sAllocator, aErrorStack);
            break;
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus ztppExpandHeaderFile(stlAllocator    *aAllocator,
                               stlErrorStack   *aErrorStack,
                               stlChar         *aReadFilePath,
                               stlChar         *aResultFilePath)
{
    stlInt32           sState = 0;
    stlFile            sResultFile;

    stlSnprintf( aResultFilePath, STL_MAX_FILE_PATH_LENGTH + 1,
                 "%s.intermediate.h", gInFilePath );

    sState = 0;
    STL_TRY(stlOpenFile(&sResultFile,
                        aResultFilePath,
                        STL_FOPEN_READ | STL_FOPEN_WRITE | STL_FOPEN_CREATE | STL_FOPEN_TRUNCATE,
                        STL_FPERM_OS_DEFAULT,
                        aErrorStack) == STL_SUCCESS);
    sState = 1;

    STL_TRY(ztppExpandHeaderFileInternal(aAllocator,
                                         aErrorStack,
                                         &sResultFile,
                                         aReadFilePath)
            == STL_SUCCESS);

    sState = 0;
    STL_TRY(stlCloseFile(&sResultFile, aErrorStack)
            == STL_SUCCESS);

    return STL_SUCCESS;

    STL_FINISH;

    switch(sState)
    {
        case 1:
            (void)stlCloseFile(&sResultFile, aErrorStack);
            break;
        default:
            break;
    }

    return STL_FAILURE;
}

/** @} */
