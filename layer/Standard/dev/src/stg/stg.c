/*******************************************************************************
 * stgLogger.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: stgLogger.c 13742 2014-10-14 10:21:22Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file stg.c
 * @brief Standard Layer Logger Internal Routines
 */

#include <stlDef.h>
#include <stl.h>
#include <stg.h>

/**
 * @addtogroup stgLogger
 * @{
 */

/**
 * @brief Logger를 위한 물리적 파일을 연다.
 * @param[in]  aLogger        대상 Logger 포인터
 * @param[in]  aLogSize       출력 로그의 바이트 단위 크기
 * @param[out] aFile          물리적 파일 포인터
 * @param[in]  aErrorStack    에러스택 포인터
 */
stlStatus stgOpenFile( stlLogger     * aLogger,
                       stlInt64        aLogSize,
                       stlFile       * aFile,
                       stlErrorStack * aErrorStack )
{
    stlChar       sFileName[STL_MAX_FILE_PATH_LENGTH + STL_MAX_FILE_NAME_LENGTH];
    stlChar       sOldFileName[STL_MAX_FILE_PATH_LENGTH + STL_MAX_FILE_NAME_LENGTH];
    stlFileInfo   sFileInfo;
    stlFile       sFile;
    stlBool       sNeedPrintHeader = STL_FALSE;
    stlExpTime    sExpTime;
    stlTime       sTime;
    
    stlSnprintf( sFileName,
                 STL_MAX_FILE_PATH_LENGTH + STL_MAX_FILE_NAME_LENGTH,
                 "%s%s%s",
                 aLogger->mFilePath,
                 STL_PATH_SEPARATOR,
                 aLogger->mFileName );

    if( stlGetFileStatByName( &sFileInfo,
                              sFileName,
                              STL_FINFO_SIZE,
                              aErrorStack )
        == STL_SUCCESS )
    {
        if( ( sFileInfo.mSize + aLogSize ) > aLogger->mMaxFileSize )
        {
            sTime = stlNow();
            stlMakeExpTimeByLocalTz( &sExpTime, sTime );

            stlSnprintf( sOldFileName,
                         STL_MAX_FILE_PATH_LENGTH,
                         "%s%s%s_%4d%02d%02d_%02d%02d%02d_%d",
                         aLogger->mFilePath,
                         STL_PATH_SEPARATOR,
                         aLogger->mFileName,
                         sExpTime.mYear + 1900,
                         sExpTime.mMonth + 1,
                         sExpTime.mDay,
                         sExpTime.mHour,
                         sExpTime.mMinute,
                         sExpTime.mSecond,
                         aLogger->mLastFileNo );

            aLogger->mLastFileNo++;

            STL_TRY( stlCopyFile( sFileName,
                                  sOldFileName,
                                  ( STL_FOPEN_WRITE    |
                                    STL_FOPEN_CREATE   |
                                    STL_FOPEN_TRUNCATE ),
                                  STL_FPERM_FILE_SOURCE,
                                  NULL,
                                  0,
                                  aErrorStack )
                     == STL_SUCCESS );

            STL_TRY( stlOpenFile( &sFile,
                                  sFileName,
                                  STL_FOPEN_CREATE |
                                  STL_FOPEN_WRITE |
                                  STL_FOPEN_READ,
                                  STL_FPERM_OS_DEFAULT,
                                  aErrorStack )
                     == STL_SUCCESS );

            STL_TRY( stlTruncateFile( &sFile,
                                      0,
                                      aErrorStack )
                     == STL_SUCCESS );

            STL_TRY( stlCloseFile( &sFile,
                                   aErrorStack )
                     == STL_SUCCESS );
            
            sNeedPrintHeader = STL_TRUE;
        }        
    }
    else
    {
        (void)stlPopError( aErrorStack );

        sNeedPrintHeader = STL_TRUE;
    }

    if( sNeedPrintHeader == STL_TRUE )
    {
        STL_TRY( stgPrintFileHeader( aLogger,
                                     aErrorStack )
                 == STL_SUCCESS );
    }

    STL_TRY( stlOpenFile( aFile,
                          sFileName,
                          STL_FOPEN_CREATE |
                          STL_FOPEN_WRITE  |
                          STL_FOPEN_APPEND,
                          STL_FPERM_OS_DEFAULT,
                          aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
    
/**
 * @brief Logger를 위한 물리적 파일을 닫는다.
 * @param[in]  aLogger        대상 Logger 포인터
 * @param[out] aFile          물리적 파일 포인터
 * @param[in]  aErrorStack    에러스택 포인터
 */
stlStatus stgCloseFile( stlLogger     * aLogger,
                        stlFile       * aFile,
                        stlErrorStack * aErrorStack )
{
    STL_TRY( stlCloseFile( aFile,
                           aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
    
/**
 * @brief Logger에 Timestamp 로그를 출력한다.
 * @param[in]  aLogger        대상 Logger 포인터
 * @param[in]  aErrorStack    에러스택 포인터
 */
stlStatus stgPrintTimestamp( stlLogger     * aLogger,
                             stlErrorStack * aErrorStack )
{
    stlExpTime      sExpTime;
    stlThreadHandle sThread;
    stlInt32        sLogSize;
    stlFile         sFile;
    stlInt32        sState = 0;

    stlMakeExpTimeByLocalTz(&sExpTime, stlNow() );
    stlThreadSelf( &sThread, aErrorStack );

    sLogSize = stlSnprintf( aLogger->mLogMsg,
                            STL_MAX_LARGE_ERROR_MESSAGE,
                            "\n[%04d-%02d-%02d %02d:%02d:%02d.%06d THREAD(%d,%ld)] \n",
                            sExpTime.mYear + 1900,
                            sExpTime.mMonth + 1,
                            sExpTime.mDay,
                            sExpTime.mHour,
                            sExpTime.mMinute,
                            sExpTime.mSecond,
                            sExpTime.mMicroSecond,
                            sThread.mProc.mProcID,
                            sThread.mHandle );

    STL_TRY( stgOpenFile( aLogger,
                          sLogSize,
                          &sFile,
                          aErrorStack )
             == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( stlPutStringFile( aLogger->mLogMsg,
                               &sFile,
                               aErrorStack )
             == STL_SUCCESS );

    sState = 0;
    STL_TRY( stgCloseFile( aLogger,
                           &sFile,
                           aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void) stgCloseFile( aLogger, &sFile, aErrorStack );
    }

    return STL_FAILURE;    
}

/**
 * @brief Logger에 Timestamp 로그를 출력한다.
 * @param[in] aLogger        대상 Logger 포인터
 * @param[in] aFormat        String Format
 * @param[in] aVarArgList    포맷에 사용될 파라미터 리스트
 * @param[in] aErrorStack    에러스택 포인터
 */
stlStatus stgAddLogMsg( stlLogger     * aLogger,
                        const stlChar * aFormat,
                        va_list         aVarArgList,
                        stlErrorStack * aErrorStack )
{
    stlFile     sFile;
    stlInt32    sState = 0;
    stlSize     sWrittenBytes;
    stlInt32    sLogSize;
    
    sLogSize = stlVsnprintf( aLogger->mLogMsg,
                             STL_MAX_LARGE_ERROR_MESSAGE,
                             aFormat,
                             aVarArgList );

    STL_TRY( stgOpenFile( aLogger,
                          sLogSize,
                          &sFile,
                          aErrorStack )
             == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( stlWriteFile( &sFile,
                           aLogger->mLogMsg,
                           sLogSize,
                           &sWrittenBytes,
                           aErrorStack )
             == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( stgCloseFile( aLogger,
                           &sFile,
                           aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void) stgCloseFile( aLogger, &sFile, aErrorStack );
    }

    return STL_FAILURE;
}

/**
 * @brief 로그 파일에 헤더를 출력한다.
 * @param[in] aLogger       Logger Pointer
 * @param[in] aErrorStack   Error Stack Pointer
 */
stlStatus stgPrintFileHeader( stlLogger     * aLogger,
                              stlErrorStack * aErrorStack )
{
    stlExpTime sExpTime;
    stlChar    sFileName[STL_MAX_FILE_PATH_LENGTH + STL_MAX_FILE_NAME_LENGTH];
    stlChar    sBuffer[1024];
    stlChar    sVersionStr[256];
    stlFile    sFile;
    stlInt32   sState = 0;
    
    stlSnprintf( sFileName,
                 STL_MAX_FILE_PATH_LENGTH + STL_MAX_FILE_NAME_LENGTH,
                 "%s%s%s",
                 aLogger->mFilePath,
                 STL_PATH_SEPARATOR,
                 aLogger->mFileName );
    
    stlMakeExpTimeByLocalTz(&sExpTime, stlNow() );
    stlGetVersionString( sVersionStr, 256 );
    
    stlSnprintf( sBuffer,
                 1024,
                 "=================================================\n"
                 " TIME    : %04d-%02d-%02d %02d:%02d:%02d.%06d\n"
                 " VERSION : %s\n"
                 "=================================================\n",
                 sExpTime.mYear + 1900,
                 sExpTime.mMonth + 1,
                 sExpTime.mDay,
                 sExpTime.mHour,
                 sExpTime.mMinute,
                 sExpTime.mSecond,
                 sExpTime.mMicroSecond,
                 sVersionStr );
    
    STL_TRY( stlOpenFile( &sFile,
                          sFileName,
                          STL_FOPEN_CREATE |
                          STL_FOPEN_WRITE  |
                          STL_FOPEN_APPEND,
                          STL_FPERM_OS_DEFAULT,
                          aErrorStack )
             == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( stlPutStringFile( sBuffer,
                               &sFile,
                               aErrorStack )
             == STL_SUCCESS );

    sState = 0;
    STL_TRY( stlCloseFile( &sFile,
                           aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void) stlCloseFile( &sFile, aErrorStack );
    }

    return STL_FAILURE;
}

/** @} */
