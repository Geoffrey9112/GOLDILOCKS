/*******************************************************************************
 * kngLogger.c
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

/**
 * @file kngLogger.c
 * @brief Kernel Layer Logger Internal Routines
 */

#include <stl.h>
#include <dtl.h>
#include <knlDef.h>
#include <knDef.h>
#include <knlLatch.h>
#include <knlGeneral.h>
#include <knlSession.h>
#include <knlEnvironment.h>

/**
 * @addtogroup kngLogger
 * @{
 */

/**
 * @brief 시간 출력을 위한 버퍼 크기
 */
#define KNG_TIMESTAMP_BUFFER_LENGTH 128

/**
 * @brief 로그 레벨 출력을 위한 버퍼 크기
 */
#define KNG_LOG_LEVEL_BUFFER_LENGTH 16

/**
 * @brief 콜 스택을 출력하기 위한 프레임의 크기
 */
#define KNG_MAX_FRAME_SIZE 100

/**
 * @brief 현재 시간을 파일에 출력한다.
 * @param[in] aFile 출력될 파일
 * @param[in] aLevel 로그의 레벨
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus kngPrintTimestamp( stlFile     * aFile,
                             knlLogLevel   aLevel,
                             knlEnv      * aEnv)
{
    stlExpTime      sExpTime;
    stlChar         sBuf[KNG_TIMESTAMP_BUFFER_LENGTH];
    stlChar         sLevel[KNG_LOG_LEVEL_BUFFER_LENGTH];
    stlThreadHandle sThread;

    stlMakeExpTimeByLocalTz(&sExpTime, stlNow() );

    switch( aLevel )
    {
        case KNL_LOG_LEVEL_FATAL :
            stlSnprintf( sLevel,
                         KNG_LOG_LEVEL_BUFFER_LENGTH,
                         "[FATAL]" );
            break;
        case KNL_LOG_LEVEL_ABORT :
            stlSnprintf( sLevel,
                         KNG_LOG_LEVEL_BUFFER_LENGTH,
                         "[ABORT]" );
            break;
        case KNL_LOG_LEVEL_WARNING :
            stlSnprintf( sLevel,
                         KNG_LOG_LEVEL_BUFFER_LENGTH,
                         "[WARNING]" );
            break;
        case KNL_LOG_LEVEL_INFO :
            stlSnprintf( sLevel,
                         KNG_LOG_LEVEL_BUFFER_LENGTH,
                         "[INFORMATION]" );
            break;
        default:
            STL_ASSERT(0);
            break;
    }

    stlThreadSelf( &sThread, KNL_ERROR_STACK(aEnv) );

    stlSnprintf( sBuf,
                 KNG_TIMESTAMP_BUFFER_LENGTH,
                 "\n[%04d-%02d-%02d %02d:%02d:%02d.%06d THREAD(%d,%ld)] %s\n",
                 sExpTime.mYear + 1900,
                 sExpTime.mMonth + 1,
                 sExpTime.mDay,
                 sExpTime.mHour,
                 sExpTime.mMinute,
                 sExpTime.mSecond,
                 sExpTime.mMicroSecond,
                 sThread.mProc.mProcID,
                 sThread.mHandle,
                 sLevel);

    STL_TRY( stlPutStringFile( sBuf,
                               aFile,
                               KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;    
}

/**
 * @brief 로그 파일에 헤더를 출력한다.
 * @param[in] aFile 출력될 파일
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus kngPrintHeader( stlFile * aFile,
                          knlEnv  * aEnv)
{
    stlExpTime sExpTime;
    stlChar    sBuffer[1024];
    stlChar    sVersionStr[256];
    
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
    
    STL_TRY( stlPutStringFile( sBuffer,
                               aFile,
                               KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 로그 파일에 Gliese Startup Message를 출력한다.
 * @param[in] aLogger 출력될 Logger
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus kngPrintStartMsg( knlTraceLogger * aLogger,
                            knlEnv         * aEnv )
{
    stlChar    sBuffer[1024];
    stlChar    sVersionStr[256];
    stlChar    sFileName[STL_MAX_FILE_PATH_LENGTH];
    stlFile    sFile;
    stlOffset  sOffset = 0;
    stlExpTime sExpTime;

    stlSnprintf( sFileName,
                 STL_MAX_FILE_PATH_LENGTH,
                 "%s",
                 aLogger->mName );

    STL_TRY( stlOpenFile( &sFile,
                          sFileName,
                          STL_FOPEN_CREATE |
                          STL_FOPEN_WRITE |
                          STL_FOPEN_READ,
                          STL_FPERM_OS_DEFAULT,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    STL_TRY( stlSeekFile( &sFile,
                          STL_FSEEK_END,
                          &sOffset,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    stlMakeExpTimeByLocalTz(&sExpTime, stlNow() );
    stlGetVersionString( sVersionStr, 256 );
    
    stlSnprintf( sBuffer,
                 1024,
                 "\n\n"
                 "=================================================\n"
                 " Startup Goldilocks\n"
                 " TIME    : %04d-%02d-%02d %02d:%02d:%02d.%06d\n"
                 " VERSION : %s\n"
                 "=================================================\n"
                 "\n",
                 sExpTime.mYear + 1900,
                 sExpTime.mMonth + 1,
                 sExpTime.mDay,
                 sExpTime.mHour,
                 sExpTime.mMinute,
                 sExpTime.mSecond,
                 sExpTime.mMicroSecond,
                 sVersionStr );
    
    STL_TRY( stlPutStringFile( sBuffer,
                               &sFile,
                               KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    STL_TRY( stlCloseFile( &sFile,
                           KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 로그 파일에 콜스택을 출력한다.
 * @param[in] aFile 출력될 파일
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus kngPrintCallStack( stlFile * aFile,
                             knlEnv  * aEnv )
{
    void * sFrameArray[KNG_MAX_FRAME_SIZE];

    STL_TRY( stlPutStringFile( "\n[CALL STACK]\n",
                               aFile,
                               KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    stlBacktraceToFile( aFile,
                        sFrameArray,
                        KNG_MAX_FRAME_SIZE,
                        NULL,
                        NULL );
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 기본 로그 파일을 생성한다.
 * @param[in] aLogger logger
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus kngCreateDefaultFile( knlTraceLogger * aLogger,
                                knlEnv         * aEnv )
{
    stlChar       sFileName[STL_MAX_FILE_PATH_LENGTH];
    stlFile       sFile;

    stlSnprintf( sFileName,
                 STL_MAX_FILE_PATH_LENGTH,
                 "%s",
                 aLogger->mName );

    STL_TRY( stlOpenFile( &sFile,
                          sFileName,
                          STL_FOPEN_CREATE |
                          STL_FOPEN_WRITE |
                          STL_FOPEN_READ,
                          STL_FPERM_OS_DEFAULT,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    STL_TRY( stlTruncateFile( &sFile,
                              0,
                              KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    STL_TRY( kngPrintHeader( &sFile,
                             aEnv ) == STL_SUCCESS );

    STL_TRY( stlCloseFile( &sFile,
                           KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 기본 로그 파일을 새로운 파일로 옮기고, 기본 로그 파일을 초기화한다.
 * @param[in] aLogger logger
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus kngCreateNewFile( knlTraceLogger * aLogger,
                            knlEnv         * aEnv )
{
    stlTime       sTime;
    stlExpTime    sExpTime;
    stlChar       sOldFileName[STL_MAX_FILE_PATH_LENGTH];
    stlChar       sDefaultFileName[STL_MAX_FILE_PATH_LENGTH];

    stlSnprintf( sDefaultFileName,
                 STL_MAX_FILE_PATH_LENGTH,
                 "%s",
                 aLogger->mName );

    sTime = stlNow();
    stlMakeExpTimeByLocalTz( &sExpTime, sTime );

    stlSnprintf( sOldFileName,
                 STL_MAX_FILE_PATH_LENGTH,
                 "%s_%4d%02d%02d_%02d%02d%02d_%d",
                 aLogger->mName,
                 sExpTime.mYear + 1900,
                 sExpTime.mMonth + 1,
                 sExpTime.mDay,
                 sExpTime.mHour,
                 sExpTime.mMinute,
                 sExpTime.mSecond,
                 aLogger->mLastFileNo );

    aLogger->mLastFileNo++;

    STL_TRY( stlCopyFile( sDefaultFileName,
                          sOldFileName,
                          ( STL_FOPEN_WRITE    |
                            STL_FOPEN_CREATE   |
                            STL_FOPEN_TRUNCATE ),
                          STL_FPERM_FILE_SOURCE,
                          NULL,
                          0,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    STL_TRY( kngCreateDefaultFile( aLogger,
                                   aEnv ) == STL_SUCCESS );
    

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 기본 로그 파일을 open 한다.
 * @param[in] aLogger logger
 * @param[in] aSize 저장될 로그의 크기
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus kngOpenDefaultFile( knlTraceLogger * aLogger,
                              stlSize          aSize,
                              knlEnv         * aEnv )
{
    stlFileInfo   sFileInfo;
    stlChar       sFileName[STL_MAX_FILE_PATH_LENGTH];
    stlInt32      sState = 0;
    stlBool       sIsTimedout;

    stlSnprintf( sFileName,
                 STL_MAX_FILE_PATH_LENGTH,
                 "%s",
                 aLogger->mName );

    STL_TRY( knlAcquireLatch( &(aLogger->mLatch),
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedout,
                              aEnv ) == STL_SUCCESS );
    sState = 1;

    if( stlGetFileStatByName( &sFileInfo,
                              sFileName,
                              STL_FINFO_SIZE,
                              KNL_ERROR_STACK( aEnv ) ) == STL_FAILURE )
    {
        /*
         * log file이 존재하지 않을 경우
         */
        STL_TRY( stlGetLastErrorCode( KNL_ERROR_STACK( aEnv ) ) == STL_ERRCODE_FILE_STAT );

        stlPopError( KNL_ERROR_STACK( aEnv ) );

        STL_TRY( kngCreateDefaultFile( aLogger,
                                       aEnv ) == STL_SUCCESS );
    }
    else
    {
        /*
         * log file에 충분한 공간이 없을 경우
         */
        if( ( sFileInfo.mSize + aSize ) > aLogger->mMaxSize )
        {
            STL_TRY( kngCreateNewFile( aLogger,
                                       aEnv ) == STL_SUCCESS );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            knlReleaseLatch( &(aLogger->mLatch),
                             aEnv);
        default:
            break;
    }

    return STL_FAILURE;    
}

/**
 * @brief 기본 로그 파일을 close 한다.
 * @param[in] aLogger      logger
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus kngCloseDefaultFile( knlTraceLogger * aLogger,
                               knlEnv         * aEnv )
{
    knlReleaseLatch( &(aLogger->mLatch), aEnv);

    return STL_SUCCESS;
}


/**
 * @brief 기본 로그 파일에 로그 메세지를 출력한다.
 * @param[in] aLogger logger
 * @param[in] aLevel 로그의 레벨
 * @param[in] aMsg 로그 메세지
 * @param[in] aLength 로그 메세지의 길이
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus kngAddLogMsg( knlTraceLogger * aLogger,
                        knlLogLevel      aLevel,
                        stlChar        * aMsg,
                        stlInt32         aLength,
                        knlEnv         * aEnv )
{
    stlChar       sFileName[STL_MAX_FILE_PATH_LENGTH];
    stlFile       sFile;
    stlOffset     sOffset = 0;
    stlSize       sWrittenBytes;

    stlSnprintf( sFileName,
                 STL_MAX_FILE_PATH_LENGTH,
                 "%s",
                 aLogger->mName );

    STL_TRY( stlOpenFile( &sFile,
                          sFileName,
                          STL_FOPEN_CREATE |
                          STL_FOPEN_WRITE |
                          STL_FOPEN_READ,
                          STL_FPERM_OS_DEFAULT,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    
    STL_TRY( stlSeekFile( &sFile,
                          STL_FSEEK_END,
                          &sOffset,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    STL_TRY( kngPrintTimestamp( &sFile,
                                aLevel,
                                aEnv ) == STL_SUCCESS );

    STL_TRY( stlWriteFile( &sFile,
                           (void*)aMsg,
                           aLength,
                           &sWrittenBytes,
                           KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    STL_TRY( stlCloseFile( &sFile,
                           KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 기본 로그 파일에 timestamp가 포함되지 않은 로그 메세지를 출력한다.
 * @param[in] aLogger logger
 * @param[in] aMsg 로그 메세지
 * @param[in] aLength 로그 메세지의 길이
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus kngAddLogMsgWithoutTimestamp( knlTraceLogger  * aLogger,
                                        stlChar         * aMsg,
                                        stlInt32          aLength,
                                        knlEnv          * aEnv )
{
    stlChar       sFileName[STL_MAX_FILE_PATH_LENGTH];
    stlFile       sFile;
    stlOffset     sOffset = 0;
    stlSize       sWrittenBytes;

    stlSnprintf( sFileName,
                 STL_MAX_FILE_PATH_LENGTH,
                 "%s",
                 aLogger->mName );

    STL_TRY( stlOpenFile( &sFile,
                          sFileName,
                          STL_FOPEN_CREATE |
                          STL_FOPEN_WRITE |
                          STL_FOPEN_READ,
                          STL_FPERM_OS_DEFAULT,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    
    STL_TRY( stlSeekFile( &sFile,
                          STL_FSEEK_END,
                          &sOffset,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    STL_TRY( stlWriteFile( &sFile,
                           (void*)aMsg,
                           aLength,
                           &sWrittenBytes,
                           KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    STL_TRY( stlCloseFile( &sFile,
                           KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 기본 로그 파일에 에러 메세지를 출력한다.
 * @param[in] aLogger logger
 * @param[in] aErrorData 에러 데이터
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus kngAddLogError( knlTraceLogger * aLogger,
                          stlErrorData   * aErrorData,
                          knlEnv         * aEnv )
{
    stlChar     sFileName[STL_MAX_FILE_PATH_LENGTH];
    stlFile     sFile;
    stlOffset   sOffset = 0;
    stlSize     sWrittenBytes;
    stlChar     sBuffer[1024];
    stlInt32    sLength;
    stlChar     sSqlState[STL_SQLSTATE_LENGTH + 1];


    stlSnprintf( sFileName,
                 STL_MAX_FILE_PATH_LENGTH,
                 "%s",
                 aLogger->mName );

    STL_TRY( stlOpenFile( &sFile,
                          sFileName,
                          STL_FOPEN_CREATE |
                          STL_FOPEN_WRITE |
                          STL_FOPEN_READ,
                          STL_FPERM_OS_DEFAULT,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
    
    STL_TRY( stlSeekFile( &sFile,
                          STL_FSEEK_END,
                          &sOffset,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    stlMakeSqlState( stlToExternalErrorCode(aErrorData->mErrorCode),
                     sSqlState );
    
    sLength = stlSnprintf( sBuffer,
                           1024,
                           "ERR-%s:(%d-%d): %s\n",
                           sSqlState,
                           aErrorData->mErrorCode,
                           aErrorData->mSystemErrorCode,
                           aErrorData->mErrorMessage );
    
    STL_TRY( stlWriteFile( &sFile,
                           (void*)sBuffer,
                           sLength,
                           &sWrittenBytes,
                           KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    STL_TRY( stlCloseFile( &sFile,
                           KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 주어진 logger를 이용해 에러가 발생한 최초 콜스택을 출력한다.
 * @param[in] aLogger 대상 logger
 * @param[in] aFrameInErrorStack Frame이 위치한 저장소
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus kngAddErrorCallStack( knlTraceLogger  * aLogger,
                                stlBool           aFrameInErrorStack,
                                knlEnv          * aEnv )
{
    stlChar    sFileName[STL_MAX_FILE_PATH_LENGTH];
    stlFile    sFile;
    stlOffset  sOffset = 0;
    stlChar    sCallstackPrefix[512];
    stlChar    sVersion[256];
    void     * sFrameArray[STL_MAX_CALLSTACK_FRAME_SIZE];

    stlSnprintf( sFileName,
                 STL_MAX_FILE_PATH_LENGTH,
                 "%s",
                 aLogger->mName );

    stlGetVersionString( sVersion, 256 );
    stlSnprintf( sCallstackPrefix,
                 512,
                 "CALL STACK [%s]\n",
                 sVersion );
    
    STL_TRY( stlOpenFile( &sFile,
                          sFileName,
                          STL_FOPEN_CREATE |
                          STL_FOPEN_WRITE |
                          STL_FOPEN_READ,
                          STL_FPERM_OS_DEFAULT,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    
    STL_TRY( stlSeekFile( &sFile,
                          STL_FSEEK_END,
                          &sOffset,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    STL_TRY( stlPutStringFile( "\n=================================================\n",
                               &sFile,
                               KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
    STL_TRY( stlPutStringFile( sCallstackPrefix,
                               &sFile,
                               KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
    STL_TRY( stlPutStringFile( "=================================================\n",
                               &sFile,
                               KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    if( aFrameInErrorStack == STL_TRUE )
    {
        stlBacktraceSymbolsToFile( (KNL_ERROR_STACK(aEnv))->mErrorCallStack,
                                   (KNL_ERROR_STACK(aEnv))->mCallStackFrameCount,
                                   &sFile );
    }
    else
    {
        stlBacktraceToFile( &sFile,
                            sFrameArray,
                            STL_MAX_CALLSTACK_FRAME_SIZE,
                            NULL,
                            NULL );
    }
    
    STL_TRY( stlCloseFile( &sFile,
                           KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
