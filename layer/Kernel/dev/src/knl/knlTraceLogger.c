/*******************************************************************************
 * knlTraceLogger.c
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
 * @file knlTraceLogger.c
 * @brief Kernel Layer Trace Logger Routines
 */

#include <stl.h>
#include <dtl.h>
#include <knlDef.h>
#include <knDef.h>
#include <knlLatch.h>
#include <kngLogger.h>
#include <knlProperty.h>
#include <knlSession.h>
#include <knlTraceLogger.h>
#include <knlEnvironment.h>
#include <knlGeneral.h>

extern knsEntryPoint * gKnEntryPoint;

/**
 * @addtogroup knlTraceLogger
 * @{
 */


/**
 * @brief trace 로그 관리자를 초기화한다.
 * @param[in,out] aLogger 초기화될 trace 로그 관리자
 * @param[in] aName 로그 경로 & 파일 이름
 * @param[in] aMaxSize 로그 파일 하나의 최대 크기
 * @param[in] aCallstackLevel 콜스택에 출력될 로그의 최소 레벨
 * <PRE>
 *     KNL_LOG_LEVEL_FATAL : FATAL, ABORT, WARNING 시 콜스택 출력
 *     KNL_LOG_LEVEL_ABORT : ABORT, WARNING 시 콜스택 출력
 *     KNL_LOG_LEVEL_WARNING : WARNING 시 콜스택 출력
 * </PRE>
 * @param[in,out] aEnv 커널 Environment
 * @remark @a aLogger 는 공유 메모리에 존재해야 한다.
 * <BR> @a aName이 절대 경로라면 지정된 위치에 Logger File이 생성되며,
 * <BR> @a aName이 상대 경로라면 프로퍼티 "SYSTEM_LOGGER_DIR" 위치에 생성된다.
 */
stlStatus knlCreateLogger( knlTraceLogger * aLogger,
                           stlChar        * aName,
                           stlUInt64        aMaxSize,
                           knlLogLevel      aCallstackLevel,
                           knlEnv         * aEnv )
{
    stlChar  sSystemLoggerDir[STL_MAX_FILE_PATH_LENGTH];
    stlChar  sAbsLoggerDir[STL_MAX_FILE_PATH_LENGTH];
    stlInt32 sAbsLoggerLen = 0;
    
    STL_PARAM_VALIDATE( aLogger != NULL,
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( ( aCallstackLevel >= KNL_LOG_LEVEL_FATAL ) &&
                        ( aCallstackLevel <= KNL_LOG_LEVEL_INFO ),
                        KNL_ERROR_STACK( aEnv ) );

    STL_TRY( knlGetPropertyValueByName( "SYSTEM_LOGGER_DIR",
                                        sSystemLoggerDir,
                                        KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    STL_TRY( stlMergeAbsFilePath( sSystemLoggerDir,
                                  aName,
                                  sAbsLoggerDir,
                                  STL_MAX_FILE_PATH_LENGTH,
                                  &sAbsLoggerLen,
                                  KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
                               
    stlStrncpy(aLogger->mName,
               sAbsLoggerDir,
               STL_MAX_FILE_PATH_LENGTH );

    aLogger->mMaxSize        = aMaxSize;
    aLogger->mLastFileNo     = 0;
    aLogger->mCallstackLevel = aCallstackLevel;

    STL_TRY( knlInitLatch( &(aLogger->mLatch),
                           STL_TRUE,
                           aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;    
}

/**
 * @brief trace 로그 관리자를 제거한다.
 * @param[in,out] aLogger 제거할 trace 로그 관리자
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knlDestroyLogger( knlTraceLogger * aLogger,
                            knlEnv         * aEnv )
{
    STL_PARAM_VALIDATE( aLogger != NULL,
                        KNL_ERROR_STACK( aEnv ) );

    aLogger->mName[0]    = 0;
    aLogger->mMaxSize    = 0;
    aLogger->mLastFileNo = 0;

    knlFinLatch( &(aLogger->mLatch) );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;    
}

/**
 * @brief 특정 process ID에 대한 trace 로그 관리자를 초기화한다.
 * @param[in,out] aLogger 초기화될 trace 로그 관리자
 * @param[in]     aPrefix 파일명의 Prefix
 * @param[in,out] aEnv 커널 Environment
 * @remark @a aLogger 는 공유 메모리에 존재해야 한다.
 */
stlStatus knlCreateProcessLogger( knlTraceLogger  * aLogger,
                                  stlChar         * aPrefix,
                                  knlEnv          * aEnv )
{
    stlThreadHandle   sThread;
    stlChar           sTraceFileName[STL_MAX_FILE_NAME_LENGTH];
    stlChar           sProcessLoggerDir[STL_MAX_FILE_PATH_LENGTH];
    stlChar           sAbsLoggerDir[STL_MAX_FILE_PATH_LENGTH];
    stlInt32          sAbsLoggerLen = 0;
    stlUInt32         sSessionEnvId = -1;
    
    STL_PARAM_VALIDATE( aLogger != NULL,
                        KNL_ERROR_STACK( aEnv ) );


    /* Session Environment ID */
    STL_TRY( knlGetSessionEnvId( KNL_SESS_ENV(aEnv),
                                 &sSessionEnvId,
                                 aEnv )
             == STL_SUCCESS );


    stlThreadSelf( &sThread, KNL_ERROR_STACK(aEnv) );

    /* Trace Logger File Name */
    stlSnprintf( sTraceFileName, STL_MAX_FILE_PATH_LENGTH,
                 "%s%sp%d%ss%u.trc",
                 aPrefix,
                 KNL_TRACE_LOG_FILE_NAME_SEPERATOR,
                 sThread.mProc.mProcID,
                 KNL_TRACE_LOG_FILE_NAME_SEPERATOR,
                 sSessionEnvId );

    /* Abstract Trace Log Path */
    STL_TRY( knlGetPropertyValueByName( "PROCESS_LOGGER_DIR",
                                        sProcessLoggerDir,
                                        KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    STL_TRY( stlMergeAbsFilePath( sProcessLoggerDir,
                                  sTraceFileName,
                                  sAbsLoggerDir,
                                  STL_MAX_FILE_PATH_LENGTH,
                                  &sAbsLoggerLen,
                                  KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
                               
    stlStrncpy(aLogger->mName,
               sAbsLoggerDir,
               STL_MAX_FILE_PATH_LENGTH );

    aLogger->mMaxSize        = KNL_DEFAULT_TRACE_LOG_FILE_SIZE;
    aLogger->mLastFileNo     = 0;
    aLogger->mCallstackLevel = KNL_LOG_LEVEL_INFO;

    STL_TRY( knlInitLatch( &(aLogger->mLatch),
                           STL_TRUE,
                           aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;    
}

/**
 * @brief 특정 process ID에 대한 trace 로그 관리자를 제거한다.
 * @param[in,out] aLogger 제거할 trace 로그 관리자
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knlDestroyProcessLogger( knlTraceLogger * aLogger,
                                   knlEnv         * aEnv )
{
    STL_PARAM_VALIDATE( aLogger != NULL,
                        KNL_ERROR_STACK( aEnv ) );

    aLogger->mName[0]    = 0;
    aLogger->mMaxSize    = 0;
    aLogger->mLastFileNo = 0;

    knlFinLatch( &(aLogger->mLatch) );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;    
}


/**
 * @brief trace 로그 관리자를 통해 로그 메세지를 기록한다.
 * @param[in,out] aLogger trace 로그 관리자
 * @param[in,out] aEnv 커널 Environment
 * @param[in] aLevel 로그의 레벨
 * <PRE>
 *     KNL_LOG_LEVEL_FATAL : FATAL LOG (운영될 수 없는 수준의 LOG)
 *     KNL_LOG_LEVEL_ABORT : ABORT LOG (롤백 가능한 수준의 LOG)
 *     KNL_LOG_LEVEL_WARNING : WARNING LOG (경고 수준의 LOG)
 *     KNL_LOG_LEVEL_INFO : INFOMATION (정보 수준의 LOG)
 * </PRE>
 * @param[in] aFormat 출력할 로그 문자열의 format
 * @param[in] ... 출력 인자
 * @remark 동시성 제어를 한다.
 */
stlStatus knlLogMsg( knlTraceLogger * aLogger,
                     knlEnv         * aEnv,
                     knlLogLevel      aLevel,
                     const stlChar  * aFormat,
                     ... )
{
    stlInt32         sLogSize;
    stlVarList       sVarArgList;
    stlChar        * sLogMsg = NULL;
    stlInt32         sState  = 0;
    knlTraceLogger * sLogger;
    stlBool          sAllowLogMsg = STL_TRUE;
    stlUInt32        sEnvId;
    stlErrorStack  * sErrorStack;
    stlInt32         i;

    if( aLogger == NULL )
    {
        STL_TRY_THROW( gKnEntryPoint != NULL, RAMP_IGNORE );
        sLogger = knlGetSystemTraceLogger();
    }
    else
    {
        sLogger = aLogger;
    }

    /*
     * Trace 로그를 찍지 못하는 상황이라고 Fatal 상황을 만들면 안된다.
     * 즉, 무시한다.
     */
    STL_TRY_THROW( sLogger != NULL, RAMP_IGNORE );
    STL_TRY_THROW( sLogger->mName[0] != 0, RAMP_IGNORE );
    STL_TRY_THROW( sLogger->mMaxSize != 0, RAMP_IGNORE );
    STL_TRY_THROW( ( aLevel >= KNL_LOG_LEVEL_FATAL ) &&
                   ( aLevel <= KNL_LOG_LEVEL_INFO ),
                   RAMP_IGNORE );

    /*
     * formatting된 문자열의 길이만큼 메모리를 할당한다.
     */
    stlVarStart( sVarArgList, aFormat );
    sLogSize = stlGetVprintfBufferSize( aFormat, sVarArgList );
    stlVarEnd( sVarArgList );
    
    STL_TRY( stlAllocHeap( (void**)&sLogMsg,
                           sLogSize + 1,
                           KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
    sState = 1;

    stlVarStart( sVarArgList, aFormat );
    stlVsnprintf( sLogMsg,
                  sLogSize + 1,
                  aFormat,
                  sVarArgList );
    stlVarEnd( sVarArgList );

    if( (KNL_SESS_ENV(aEnv) != NULL) &&
        (knlIsSharedSessionEnv( KNL_SESS_ENV(aEnv) ) == STL_TRUE) )
    {
        knlIncOpenFileCount( KNL_SESS_ENV(aEnv) );
        stlMemBarrier();
        knlGetEnvId( aEnv, &sEnvId );

        /**
         * gmaster main thread는 처리하지 않는다.
         */
        
        if( sEnvId > 0 )
        {
            sAllowLogMsg = knlAllowDiskIo( aEnv );
        }
    }

    sState = 2;
    
    if( sAllowLogMsg == STL_TRUE )
    {
        STL_TRY( kngOpenDefaultFile( sLogger,
                                     sLogSize,
                                     aEnv ) == STL_SUCCESS );
        sState = 3;

        STL_TRY( kngAddLogMsg( sLogger,
                               aLevel,
                               sLogMsg,
                               sLogSize,
                               aEnv ) == STL_SUCCESS );

        if( aLevel <= KNL_LOG_LEVEL_WARNING )
        {
            sErrorStack = KNL_ERROR_STACK(aEnv);
            
            for( i = sErrorStack->mTop; i >= 0; i-- )
            {
                STL_TRY( kngAddLogError( sLogger,
                                         &(sErrorStack->mErrorData[i]),
                                         aEnv ) == STL_SUCCESS );
            }
        }
        
        sState = 2;
        STL_TRY( kngCloseDefaultFile( sLogger,
                                      aEnv ) == STL_SUCCESS );
    }

    sState = 1;
    if( (KNL_SESS_ENV(aEnv) != NULL) &&
        (knlIsSharedSessionEnv( KNL_SESS_ENV(aEnv) ) == STL_TRUE) )
    {
        knlDecOpenFileCount( KNL_SESS_ENV(aEnv) );
    }

    sState = 0;
    stlFreeHeap( (void*)sLogMsg );
    
    STL_RAMP( RAMP_IGNORE );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 3:
            kngCloseDefaultFile( sLogger,
                                 aEnv ) ;
        case 2:
            if( knlIsSharedSessionEnv( KNL_SESS_ENV(aEnv) ) == STL_TRUE )
            {
                knlDecOpenFileCount( KNL_SESS_ENV(aEnv) );
            }
        case 1:
            stlFreeHeap( (void*)sLogMsg );
            break;
        default:
            break;
    }

    
    stlPopError( KNL_ERROR_STACK( aEnv ) );

    return STL_SUCCESS;    
}


/**
 * @brief trace 로그 관리자를 통해 timestamp를 제외한 로그 메세지를 기록한다.
 * @param[in,out] aLogger trace 로그 관리자
 * @param[in,out] aEnv 커널 Environment
 * @param[in] aFormat 출력할 로그 문자열의 format
 * @param[in] ... 출력 인자
 * @remark 동시성 제어를 한다.
 */
stlStatus knlLogMsgWithoutTimestamp( knlTraceLogger * aLogger,
                                     knlEnv         * aEnv,
                                     const stlChar  * aFormat,
                                     ... )
{
    stlInt32         sLogSize;
    stlVarList       sVarArgList;
    stlChar        * sLogMsg = NULL;
    stlInt32         sState  = 0;
    knlTraceLogger * sLogger;
    stlBool          sAllowLogMsg = STL_TRUE;
    stlUInt32        sEnvId;

    if( aLogger == NULL )
    {
        STL_TRY_THROW( gKnEntryPoint != NULL, RAMP_IGNORE );
        sLogger = &gKnEntryPoint->mSystemLogger;
    }
    else
    {
        sLogger = aLogger;
    }

    /*
     * Trace 로그를 찍지 못하는 상황이라고 Fatal 상황을 만들면 안된다.
     * 즉, 무시한다.
     */
    STL_TRY_THROW( sLogger != NULL, RAMP_IGNORE );
    STL_TRY_THROW( sLogger->mName[0] != 0, RAMP_IGNORE );
    STL_TRY_THROW( sLogger->mMaxSize != 0, RAMP_IGNORE );

    /*
     * formatting된 문자열의 길이만큼 메모리를 할당한다.
     */
    stlVarStart( sVarArgList, aFormat );
    sLogSize = stlGetVprintfBufferSize( aFormat, sVarArgList );
    stlVarEnd( sVarArgList );
    
    STL_TRY( stlAllocHeap( (void**)&sLogMsg,
                           sLogSize + 1,
                           KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
    sState = 1;

    stlVarStart( sVarArgList, aFormat );
    stlVsnprintf( sLogMsg,
                  sLogSize + 1,
                  aFormat,
                  sVarArgList );
    stlVarEnd( sVarArgList );

    if( knlIsSharedSessionEnv( KNL_SESS_ENV(aEnv) ) == STL_TRUE )
    {
        knlIncOpenFileCount( KNL_SESS_ENV(aEnv) );
        stlMemBarrier();
        knlGetEnvId( aEnv, &sEnvId );

        /**
         * gmaster main thread는 처리하지 않는다.
         */
        
        if( sEnvId > 0 )
        {
            sAllowLogMsg = knlAllowDiskIo( aEnv );
        }
    }
    
    if( sAllowLogMsg == STL_TRUE )
    {
        STL_TRY( kngOpenDefaultFile( sLogger,
                                     sLogSize,
                                     aEnv ) == STL_SUCCESS );
        sState = 2;

        STL_TRY( kngAddLogMsgWithoutTimestamp( sLogger,
                                               sLogMsg,
                                               sLogSize,
                                               aEnv ) == STL_SUCCESS );

        sState = 1;
        STL_TRY( kngCloseDefaultFile( sLogger,
                                      aEnv ) == STL_SUCCESS );
    }

    sState = 0;
    stlFreeHeap( (void*)sLogMsg );

    if( knlIsSharedSessionEnv( KNL_SESS_ENV(aEnv) ) == STL_TRUE )
    {
        knlDecOpenFileCount( KNL_SESS_ENV(aEnv) );
    }
    
    STL_RAMP( RAMP_IGNORE );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            kngCloseDefaultFile( sLogger,
                                 aEnv ) ;
        case 1:
            stlFreeHeap( (void*)sLogMsg );
            break;
        default:
            break;
    }

    if( knlIsSharedSessionEnv( KNL_SESS_ENV(aEnv) ) == STL_TRUE )
    {
        knlDecOpenFileCount( KNL_SESS_ENV(aEnv) );
    }
    
    stlPopError( KNL_ERROR_STACK( aEnv ) );

    return STL_SUCCESS;    
}

/**
 * @brief trace 로그 관리자를 통해 timestamp를 제외한 간단한 로그 메세지를 기록한다.
 * @param[in,out] aLogger trace 로그 관리자
 * @param[in] aLogMsg 출력할 로그 문자열
 * @param[in,out] aEnv 커널 Environment
 * @remark 동시성 제어를 한다.
 */
stlStatus knlSimpleLogMsgWithoutTimestamp( knlTraceLogger   * aLogger,
                                           stlChar          * aLogMsg,
                                           knlEnv           * aEnv )
{
    stlInt32         sLogSize;
    stlInt32         sState  = 0;
    knlTraceLogger * sLogger;
    stlBool          sAllowLogMsg = STL_TRUE;
    stlUInt32        sEnvId;

    if( aLogger == NULL )
    {
        STL_TRY_THROW( gKnEntryPoint != NULL, RAMP_IGNORE );
        sLogger = &gKnEntryPoint->mSystemLogger;
    }
    else
    {
        sLogger = aLogger;
    }

    /*
     * Trace 로그를 찍지 못하는 상황이라고 Fatal 상황을 만들면 안된다.
     * 즉, 무시한다.
     */
    STL_TRY_THROW( sLogger != NULL, RAMP_IGNORE );
    STL_TRY_THROW( sLogger->mName[0] != 0, RAMP_IGNORE );
    STL_TRY_THROW( sLogger->mMaxSize != 0, RAMP_IGNORE );

    sLogSize = stlStrlen( aLogMsg );

    if( knlIsSharedSessionEnv( KNL_SESS_ENV(aEnv) ) == STL_TRUE )
    {
        knlIncOpenFileCount( KNL_SESS_ENV(aEnv) );
        stlMemBarrier();
        knlGetEnvId( aEnv, &sEnvId );

        /**
         * gmaster main thread는 처리하지 않는다.
         */
        
        if( sEnvId > 0 )
        {
            sAllowLogMsg = knlAllowDiskIo( aEnv );
        }
    }
    
    if( sAllowLogMsg == STL_TRUE )
    {
        STL_TRY( kngOpenDefaultFile( sLogger,
                                     sLogSize,
                                     aEnv ) == STL_SUCCESS );
        sState = 1;

        STL_TRY( kngAddLogMsgWithoutTimestamp( sLogger,
                                               aLogMsg,
                                               sLogSize,
                                               aEnv ) == STL_SUCCESS );

        sState = 0;
        STL_TRY( kngCloseDefaultFile( sLogger,
                                      aEnv ) == STL_SUCCESS );
    }

    if( knlIsSharedSessionEnv( KNL_SESS_ENV(aEnv) ) == STL_TRUE )
    {
        knlDecOpenFileCount( KNL_SESS_ENV(aEnv) );
    }
    
    STL_RAMP( RAMP_IGNORE );
 

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        kngCloseDefaultFile( sLogger,
                             aEnv ) ;
    }

    if( knlIsSharedSessionEnv( KNL_SESS_ENV(aEnv) ) == STL_TRUE )
    {
        knlDecOpenFileCount( KNL_SESS_ENV(aEnv) );
    }
    
    stlPopError( KNL_ERROR_STACK( aEnv ) );

    return STL_SUCCESS;    
}

/**
 * @brief 래치 획득없이 trace 로그 관리자를 통해 로그 메세지를 기록한다.
 * @param[in,out] aLogger trace 로그 관리자
 * @param[in,out] aEnv 커널 Environment
 * @param[in] aLevel 로그의 레벨
 * <PRE>
 *     KNL_LOG_LEVEL_FATAL : FATAL LOG (운영될 수 없는 수준의 LOG)
 *     KNL_LOG_LEVEL_ABORT : ABORT LOG (롤백 가능한 수준의 LOG)
 *     KNL_LOG_LEVEL_WARNING : WARNING LOG (경고 수준의 LOG)
 *     KNL_LOG_LEVEL_INFO : INFOMATION (정보 수준의 LOG)
 * </PRE>
 * @param[in] aFormat 출력할 로그 문자열의 format
 * @param[in] ... 출력 인자
 * @remark 동시성 제어를 한다.
 */
stlStatus knlLogMsgUnsafe( knlTraceLogger * aLogger,
                           knlEnv         * aEnv,
                           knlLogLevel      aLevel,
                           const stlChar  * aFormat,
                           ... )
{
    stlInt32         sLogSize;
    stlVarList       sVarArgList;
    stlChar        * sLogMsg = NULL;
    stlInt32         sState  = 0;
    knlTraceLogger * sLogger;
    stlErrorStack  * sErrorStack;
    stlInt32         i;

    if( aLogger == NULL )
    {
        STL_TRY_THROW( gKnEntryPoint != NULL, RAMP_IGNORE );
        sLogger = knlGetSystemTraceLogger();
    }
    else
    {
        sLogger = aLogger;
    }

    /*
     * Trace 로그를 찍지 못하는 상황이라고 Fatal 상황을 만들면 안된다.
     * 즉, 무시한다.
     */
    STL_TRY_THROW( sLogger != NULL, RAMP_IGNORE );
    STL_TRY_THROW( sLogger->mName[0] != 0, RAMP_IGNORE );
    STL_TRY_THROW( sLogger->mMaxSize != 0, RAMP_IGNORE );
    STL_TRY_THROW( ( aLevel >= KNL_LOG_LEVEL_FATAL ) &&
                   ( aLevel <= KNL_LOG_LEVEL_INFO ),
                   RAMP_IGNORE );

    /*
     * formatting된 문자열의 길이만큼 메모리를 할당한다.
     */
    stlVarStart( sVarArgList, aFormat );
    sLogSize = stlGetVprintfBufferSize( aFormat, sVarArgList );
    stlVarEnd( sVarArgList );
    
    STL_TRY( stlAllocHeap( (void**)&sLogMsg,
                           sLogSize + 1,
                           KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
    sState = 1;

    stlVarStart( sVarArgList, aFormat );
    stlVsnprintf( sLogMsg,
                  sLogSize + 1,
                  aFormat,
                  sVarArgList );
    stlVarEnd( sVarArgList );

    STL_TRY( kngAddLogMsg( sLogger,
                           aLevel,
                           sLogMsg,
                           sLogSize,
                           aEnv ) == STL_SUCCESS );

    if( aLevel <= KNL_LOG_LEVEL_WARNING )
    {
        sErrorStack = KNL_ERROR_STACK(aEnv);
            
        for( i = sErrorStack->mTop; i >= 0; i-- )
        {
            STL_TRY( kngAddLogError( sLogger,
                                     &(sErrorStack->mErrorData[i]),
                                     aEnv ) == STL_SUCCESS );
        }
    }
    
    sState = 0;
    stlFreeHeap( (void*)sLogMsg );

    STL_RAMP( RAMP_IGNORE );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            stlFreeHeap( (void*)sLogMsg );
            break;
        default:
            break;
    }

    stlPopError( KNL_ERROR_STACK( aEnv ) );

    return STL_SUCCESS;    
}

/**
 * @brief trace 로그 관리자를 통해 로그 메세지를 기록한다.
 * @param[in,out] aLogger trace 로그 관리자
 * @param[in,out] aEnv 커널 Environment
 * @param[in] aLevel 로그의 레벨
 * <PRE>
 *     KNL_LOG_LEVEL_FATAL : FATAL LOG (운영될 수 없는 수준의 LOG)
 *     KNL_LOG_LEVEL_ABORT : ABORT LOG (롤백 가능한 수준의 LOG)
 *     KNL_LOG_LEVEL_WARNING : WARNING LOG (경고 수준의 LOG)
 *     KNL_LOG_LEVEL_INFO : INFOMATION (정보 수준의 LOG)
 * </PRE>
 * @param[in] aMsgSize 출력할 메세지의 총 길이
 * @param[in] aFormat 출력할 로그 문자열의 format
 * @param[in] aVarArgList 출력 인자
 * @remark 동시성 제어를 한다.
 */
stlStatus knlVarLogMsg( knlTraceLogger * aLogger,
                        knlEnv         * aEnv,
                        knlLogLevel      aLevel,
                        stlInt32         aMsgSize,
                        const stlChar  * aFormat,
                        stlVarList       aVarArgList )
{
    stlChar        * sLogMsg = NULL;
    stlInt32         sState  = 0;
    knlTraceLogger * sLogger;
    stlBool          sAllowLogMsg = STL_TRUE;
    stlUInt32        sEnvId;
    stlErrorStack  * sErrorStack;
    stlInt32         i;

    if( aLogger == NULL )
    {
        STL_TRY_THROW( gKnEntryPoint != NULL, RAMP_IGNORE );
        sLogger = knlGetSystemTraceLogger();
    }
    else
    {
        sLogger = aLogger;
    }

    /*
     * Trace 로그를 찍지 못하는 상황이라고 Fatal 상황을 만들면 안된다.
     * 즉, 무시한다.
     */
    STL_TRY_THROW( sLogger != NULL, RAMP_IGNORE );
    STL_TRY_THROW( sLogger->mName[0] != 0, RAMP_IGNORE );
    STL_TRY_THROW( sLogger->mMaxSize != 0, RAMP_IGNORE );
    STL_TRY_THROW( ( aLevel >= KNL_LOG_LEVEL_FATAL ) &&
                   ( aLevel <= KNL_LOG_LEVEL_INFO ),
                   RAMP_IGNORE );

    STL_TRY( stlAllocHeap( (void**)&sLogMsg,
                           aMsgSize + 1,
                           KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
    sState = 1;

    stlVsnprintf( sLogMsg,
                  aMsgSize + 1,
                  aFormat,
                  aVarArgList );

    if( knlIsSharedSessionEnv( KNL_SESS_ENV(aEnv) ) == STL_TRUE )
    {
        knlIncOpenFileCount( KNL_SESS_ENV(aEnv) );
        stlMemBarrier();
        knlGetEnvId( aEnv, &sEnvId );

        /**
         * gmaster main thread는 처리하지 않는다.
         */
        
        if( sEnvId > 0 )
        {
            sAllowLogMsg = knlAllowDiskIo( aEnv );
        }
    }
    
    if( sAllowLogMsg == STL_TRUE )
    {
        STL_TRY( kngOpenDefaultFile( sLogger,
                                     aMsgSize,
                                     aEnv ) == STL_SUCCESS );
        sState = 2;

        STL_TRY( kngAddLogMsg( sLogger,
                               aLevel,
                               sLogMsg,
                               aMsgSize,
                               aEnv ) == STL_SUCCESS );

        if( aLevel <= KNL_LOG_LEVEL_WARNING )
        {
            sErrorStack = KNL_ERROR_STACK(aEnv);
            
            for( i = sErrorStack->mTop; i >= 0; i-- )
            {
                STL_TRY( kngAddLogError( sLogger,
                                         &(sErrorStack->mErrorData[i]),
                                         aEnv ) == STL_SUCCESS );
            }
        }
        
        sState = 1;
        STL_TRY( kngCloseDefaultFile( sLogger,
                                      aEnv ) == STL_SUCCESS );
    }

    sState = 0;
    stlFreeHeap( (void*)sLogMsg );

    if( knlIsSharedSessionEnv( KNL_SESS_ENV(aEnv) ) == STL_TRUE )
    {
        knlDecOpenFileCount( KNL_SESS_ENV(aEnv) );
    }
    
    STL_RAMP( RAMP_IGNORE );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            kngCloseDefaultFile( sLogger,
                                 aEnv ) ;
        case 1:
            stlFreeHeap( (void*)sLogMsg );
            break;
        default:
            break;
    }

    if( knlIsSharedSessionEnv( KNL_SESS_ENV(aEnv) ) == STL_TRUE )
    {
        knlDecOpenFileCount( KNL_SESS_ENV(aEnv) );
    }
    
    stlPopError( KNL_ERROR_STACK( aEnv ) );

    return STL_SUCCESS;    
}

/**
 * @brief trace 로그 관리자를 통해 전체 에러 로그를 기록한다.
 * @param[in,out] aLogger trace 로그 관리자
 * @param[in] aErrorStack 출력할 에러 Stack
 * @param[in,out] aEnv 커널 Environment
 * @remark 동시성 제어를 한다.
 */
stlStatus knlLogErrorStack( knlTraceLogger * aLogger,
                            stlErrorStack  * aErrorStack,
                            knlEnv         * aEnv )
{
    stlInt32         sState = 0;
    knlTraceLogger * sLogger;
    stlInt32         i;
    stlErrorData   * sErrorData;
    stlBool          sAllowLogMsg = STL_TRUE;
    stlUInt32        sEnvId;

    if( aLogger == NULL )
    {
        STL_TRY_THROW( gKnEntryPoint != NULL, RAMP_IGNORE );
        sLogger = &gKnEntryPoint->mSystemLogger;
    }
    else
    {
        sLogger = aLogger;
    }
    
    /*
     * Trace 로그를 찍지 못하는 상황이라고 Fatal 상황을 만들면 안된다.
     * 즉, 무시한다.
     */
    STL_TRY_THROW( sLogger != NULL, RAMP_IGNORE );
    STL_TRY_THROW( sLogger->mName[0] != 0, RAMP_IGNORE );
    STL_TRY_THROW( sLogger->mMaxSize != 0, RAMP_IGNORE );

    if( knlIsSharedSessionEnv( KNL_SESS_ENV(aEnv) ) == STL_TRUE )
    {
        knlIncOpenFileCount( KNL_SESS_ENV(aEnv) );
        stlMemBarrier();
        knlGetEnvId( aEnv, &sEnvId );

        /**
         * gmaster main thread는 처리하지 않는다.
         */
        
        if( sEnvId > 0 )
        {
            sAllowLogMsg = knlAllowDiskIo( aEnv );
        }
    }
    
    if( sAllowLogMsg == STL_TRUE )
    {
        STL_TRY( kngOpenDefaultFile( sLogger,
                                     STL_SIZEOF(stlErrorData) * (aErrorStack->mTop + 1),
                                     aEnv ) == STL_SUCCESS );
        sState = 1;

        for( i = aErrorStack->mTop; i >= 0; i-- )
        {
            sErrorData = &aErrorStack->mErrorData[i];
            STL_TRY( kngAddLogError( sLogger,
                                     sErrorData,
                                     aEnv ) == STL_SUCCESS );
        }

        sState = 0;
        STL_TRY( kngCloseDefaultFile( sLogger,
                                      aEnv ) == STL_SUCCESS );
    }
    
    if( knlIsSharedSessionEnv( KNL_SESS_ENV(aEnv) ) == STL_TRUE )
    {
        knlDecOpenFileCount( KNL_SESS_ENV(aEnv) );
    }
    
    STL_RAMP( RAMP_IGNORE );
    
    return STL_SUCCESS;

    STL_FINISH;

    if ( sState == 1 )
    {
        kngCloseDefaultFile( sLogger,
                             aEnv);
    }

    if( knlIsSharedSessionEnv( KNL_SESS_ENV(aEnv) ) == STL_TRUE )
    {
        knlDecOpenFileCount( KNL_SESS_ENV(aEnv) );
    }
    
    stlPopError( KNL_ERROR_STACK( aEnv ) );

    return STL_SUCCESS;    
}

/**
 * @brief 래치 획득없이 trace 로그 관리자를 통해 전체 에러 로그를 기록한다.
 * @param[in,out] aLogger trace 로그 관리자
 * @param[in] aErrorStack 출력할 에러 Stack
 * @param[in,out] aEnv 커널 Environment
 * @remark 동시성 제어를 안한다.
 */
stlStatus knlLogErrorStackUnsafe( knlTraceLogger * aLogger,
                                  stlErrorStack  * aErrorStack,
                                  knlEnv         * aEnv )
{
    knlTraceLogger * sLogger;
    stlInt32         i;
    stlErrorData   * sErrorData;

    if( aLogger == NULL )
    {
        STL_TRY_THROW( gKnEntryPoint != NULL, RAMP_IGNORE );
        sLogger = &gKnEntryPoint->mSystemLogger;
    }
    else
    {
        sLogger = aLogger;
    }
    
    /*
     * Trace 로그를 찍지 못하는 상황이라고 Fatal 상황을 만들면 안된다.
     * 즉, 무시한다.
     */
    STL_TRY_THROW( sLogger != NULL, RAMP_IGNORE );
    STL_TRY_THROW( sLogger->mName[0] != 0, RAMP_IGNORE );
    STL_TRY_THROW( sLogger->mMaxSize != 0, RAMP_IGNORE );

    for( i = aErrorStack->mTop; i >= 0; i-- )
    {
        sErrorData = &aErrorStack->mErrorData[i];
        STL_TRY( kngAddLogError( sLogger,
                                 sErrorData,
                                 aEnv ) == STL_SUCCESS );
    }

    STL_RAMP( RAMP_IGNORE );
    
    return STL_SUCCESS;

    STL_FINISH;

    stlPopError( KNL_ERROR_STACK( aEnv ) );

    return STL_SUCCESS;    
}

/**
 * @brief trace 로그 관리자를 통해 에러 콜스택을 로그에 기록한다.
 * @param[in,out] aLogger trace 로그 관리자
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knlLogErrorCallStack( knlTraceLogger * aLogger,
                                knlEnv         * aEnv )
{
    knlTraceLogger * sLogger;

    if( aLogger == NULL )
    {
        STL_TRY_THROW( gKnEntryPoint != NULL, RAMP_IGNORE );
        sLogger = &gKnEntryPoint->mSystemLogger;
    }
    else
    {
        sLogger = aLogger;
    }
    
    /*
     * Trace 로그를 찍지 못하는 상황이라고 Fatal 상황을 만들면 안된다.
     * 즉, 무시한다.
     */
    STL_TRY_THROW( sLogger != NULL, RAMP_IGNORE );
    STL_TRY_THROW( sLogger->mName[0] != 0, RAMP_IGNORE );
    STL_TRY_THROW( sLogger->mMaxSize != 0, RAMP_IGNORE );

    STL_TRY( kngAddErrorCallStack( sLogger,
                                   STL_TRUE,  /* aFrameInErrorStack */
                                   aEnv ) == STL_SUCCESS);

    STL_RAMP( RAMP_IGNORE );
    
    return STL_SUCCESS;

    STL_FINISH;

    stlPopError( KNL_ERROR_STACK( aEnv ) );

    return STL_SUCCESS;    
}

/**
 * @brief trace 로그 관리자를 통해 현재 콜스택을 로그에 기록한다.
 * @param[in,out] aLogger trace 로그 관리자
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knlLogCallStack( knlTraceLogger * aLogger,
                           knlEnv         * aEnv )
{
    stlInt32         sState = 0;
    knlTraceLogger * sLogger;
    stlBool          sAllowLogMsg = STL_TRUE;
    stlUInt32        sEnvId;

    if( aLogger == NULL )
    {
        STL_TRY_THROW( gKnEntryPoint != NULL, RAMP_IGNORE );
        sLogger = &gKnEntryPoint->mSystemLogger;
    }
    else
    {
        sLogger = aLogger;
    }
    
    /*
     * Trace 로그를 찍지 못하는 상황이라고 Fatal 상황을 만들면 안된다.
     * 즉, 무시한다.
     */
    STL_TRY_THROW( sLogger != NULL, RAMP_IGNORE );
    STL_TRY_THROW( sLogger->mName[0] != 0, RAMP_IGNORE );
    STL_TRY_THROW( sLogger->mMaxSize != 0, RAMP_IGNORE );

    if( knlIsSharedSessionEnv( KNL_SESS_ENV(aEnv) ) == STL_TRUE )
    {
        knlIncOpenFileCount( KNL_SESS_ENV(aEnv) );
        stlMemBarrier();
        knlGetEnvId( aEnv, &sEnvId );

        /**
         * gmaster main thread는 처리하지 않는다.
         */
        
        if( sEnvId > 0 )
        {
            sAllowLogMsg = knlAllowDiskIo( aEnv );
        }
    }
    
    if( sAllowLogMsg == STL_TRUE )
    {
        STL_TRY( kngOpenDefaultFile( sLogger,
                                     0,
                                     aEnv ) == STL_SUCCESS );
        sState = 1;

        STL_TRY( kngAddErrorCallStack( sLogger,
                                       STL_FALSE, /* aFrameInErrorStack */
                                       aEnv ) == STL_SUCCESS);

        sState = 0;
        STL_TRY( kngCloseDefaultFile( sLogger,
                                      aEnv ) == STL_SUCCESS );
    }

    if( knlIsSharedSessionEnv( KNL_SESS_ENV(aEnv) ) == STL_TRUE )
    {
        knlDecOpenFileCount( KNL_SESS_ENV(aEnv) );
    }
    
    STL_RAMP( RAMP_IGNORE );
    
    return STL_SUCCESS;

    STL_FINISH;

    if ( sState == 1 )
    {
        kngCloseDefaultFile( sLogger,
                             aEnv);
    }

    if( knlIsSharedSessionEnv( KNL_SESS_ENV(aEnv) ) == STL_TRUE )
    {
        knlDecOpenFileCount( KNL_SESS_ENV(aEnv) );
    }
    
    stlPopError( KNL_ERROR_STACK( aEnv ) );

    return STL_SUCCESS;    
}

/**
 * @brief Unsafe fashion으로 trace 로그 관리자를 통해 현재 콜스택을 로그에 기록한다.
 * @param[in,out] aLogger trace 로그 관리자
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knlLogCallStackUnsafe( knlTraceLogger * aLogger,
                                 knlEnv         * aEnv )
{
    knlTraceLogger * sLogger;

    if( aLogger == NULL )
    {
        STL_TRY_THROW( gKnEntryPoint != NULL, RAMP_IGNORE );
        sLogger = &gKnEntryPoint->mSystemLogger;
    }
    else
    {
        sLogger = aLogger;
    }
    
    /*
     * Trace 로그를 찍지 못하는 상황이라고 Fatal 상황을 만들면 안된다.
     * 즉, 무시한다.
     */
    STL_TRY_THROW( sLogger != NULL, RAMP_IGNORE );
    STL_TRY_THROW( sLogger->mName[0] != 0, RAMP_IGNORE );
    STL_TRY_THROW( sLogger->mMaxSize != 0, RAMP_IGNORE );

    STL_TRY( kngAddErrorCallStack( sLogger,
                                   STL_FALSE, /* aFrameInErrorStack */
                                   aEnv ) == STL_SUCCESS);

    STL_RAMP( RAMP_IGNORE );
    
    return STL_SUCCESS;

    STL_FINISH;

    stlPopError( KNL_ERROR_STACK( aEnv ) );
    
    return STL_SUCCESS;    
}

/**
 * @brief System Logger를 반환한다.
 * @return System Logger pointer
 */
knlTraceLogger * knlGetSystemTraceLogger()
{
    return &(gKnEntryPoint->mSystemLogger);
}


/** @} */
