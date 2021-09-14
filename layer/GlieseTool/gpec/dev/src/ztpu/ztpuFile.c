/*******************************************************************************
 * ztpuFile.c
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
#include <ztpuFile.h>
#include <ztpSqlLex.h>
#include <ztpSqlParser.h>
#include <ztppMacro.h>

/**
 * @file ztpuFile.c
 * @brief Gliese Embedded SQL in C precompiler utility functions
 */


/**
 * @addtogroup ztpuFile
 * @{
 */

stlStatus ztpuFindFileName(stlChar **aFileName, stlChar *aFilePath)
{
    stlChar    *sCur;

    *aFileName = NULL;
    for(sCur = aFilePath; *sCur != '\0'; sCur ++)
    {
        if(ZTP_IS_DIR_SEP(*sCur))
        {
            *aFileName = sCur + 1;
        }
    }

    return STL_SUCCESS;
}

stlStatus ztpuMatchInOutFilePath()
{
    stlChar    *sFileExtPtr = NULL;

    /*
     * Input file name에서 Output file name을 생성한다.
     * 결과적으로, *.gc -> *.c 의 이름 매칭을 수행한다.
     */
    /* Output file name 생성 */
    stlStrncpy(gOutFilePath, gInFilePath, STL_MAX_FILE_PATH_LENGTH + 1);
    sFileExtPtr = stlStrrchr(gOutFilePath, '.');
    stlStrncpy(sFileExtPtr + 1, "c", stlStrlen("c") + 1);

    return STL_SUCCESS;
}

stlStatus ztpuMakeInFilePath()
{
    stlChar    *sFileExtPtr = NULL;
    stlChar    *sFileNamePtr = NULL;

    /*
     * Input file name의 확장자가 없는 경우 .gc를 붙여준다.
     */
    ztpuFindFileName(&sFileNamePtr, gInFilePath);
    if( sFileNamePtr != NULL )
    {
        stlStrncpy(gInFileDir, gInFilePath, sFileNamePtr - gInFilePath);
        sFileExtPtr = stlStrrchr(sFileNamePtr, '.');
    }
    else
    {
        stlStrcpy(gInFileDir, ".");
        sFileExtPtr = stlStrrchr(gInFilePath, '.');
    }

    /* 확장자가 존재하지 않는 경우 */
    if(sFileExtPtr == NULL)
    {
        stlStrcat(gInFilePath, ".gc");
    }

    return STL_SUCCESS;
}

stlStatus ztpuReadFile(stlFile        *aInFile,
                       stlAllocator   *aAllocator,
                       stlErrorStack  *aErrorStack,
                       stlChar       **aBuffer,
                       stlSize        *aLength)
{
    stlInt32        sState = 0;
    stlFileInfo     sFileInfo;
    stlSize         sBytesRead;

    STL_TRY(stlGetFileStatByHandle(&sFileInfo,
                                   STL_FINFO_MIN,
                                   aInFile,
                                   aErrorStack)
            == STL_SUCCESS);

    STL_TRY(stlAllocRegionMem(aAllocator,
                              sFileInfo.mSize + 1,
                              (void **)aBuffer,
                              aErrorStack)
            == STL_SUCCESS);

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

stlStatus ztpuWriteFile(stlFile       *aFile,
                        void          *aBuffer,
                        stlSize        aLength,
                        stlErrorStack *aErrorStack)
{
    stlSize   sWrittenBytes;

    if( aLength > 0 )
    {
        STL_TRY(stlWriteFile(aFile,
                             aBuffer,
                             aLength,
                             &sWrittenBytes,
                             aErrorStack)
                == STL_SUCCESS);

        STL_TRY(aLength == sWrittenBytes);
    }
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztpuWriteOutputHeader(stlFile       *aFile,
                                stlChar       *aInFileName,
                                stlChar       *aOutFileName,
                                stlChar       *aOutputHeader,
                                stlSize        aOutputHeaderSize,
                                stlErrorStack *aErrorStack)
{
    stlChar          sLineBuffer[ZTP_LINE_BUF_SIZE];
    stlInt32         sLineLength;

    STL_TRY( ztpuWriteFile( aFile,
                            aOutputHeader,
                            aOutputHeaderSize,
                            aErrorStack )
            == STL_SUCCESS);

    if( gNoLineInfo == STL_FALSE )
    {
        stlMemset(sLineBuffer, 0x00, ZTP_LINE_BUF_SIZE);

        sLineLength = stlSnprintf(sLineBuffer, ZTP_LINE_BUF_SIZE,
                                  "#line __LINE__ \"%s\"\n", aOutFileName);
        STL_TRY( ztpuWriteFile( aFile,
                                sLineBuffer,
                                sLineLength,
                                aErrorStack )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztpuSendStringToOutFile(stlFile        *aOutFile,
                                  stlChar        *aStr,
                                  stlErrorStack  *aErrorStack)
{
    stlSize    sStrLen;

    sStrLen = stlStrlen(aStr);
    STL_TRY( ztpuWriteFile( aOutFile,
                            aStr,
                            sStrLen,
                            aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
