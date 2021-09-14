/*******************************************************************************
 * stpWin.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: stpUnix.c 14014 2014-12-15 01:44:48Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stc.h>
#include <stp.h>
#include <ste.h>
#include <stlError.h>
#include <stlStrings.h>
#include <stlLog.h>
#include <stlTime.h>

#ifndef SEM_FAILED
#define SEM_FAILED (-1)
#endif

typedef LONG NTSTATUS;

typedef NTSTATUS (NTAPI *stpNtQuerySemaphore)( HANDLE aSemaphoreHandle, 
                                               DWORD  aSemaphoreInformationClass,
                                               PVOID  aSemaphoreInformation,
                                               ULONG  aSemaphoreInformationLength, 
                                               PULONG aReturnLength OPTIONAL ); 

typedef struct stpSEMAPHORE_BASIC_INFORMATION
{   
    ULONG mCurrentCount; 
    ULONG mMaximumCount;
} stpSEMAPHORE_BASIC_INFORMATION;

/*
 * @brief 프로세스간 동기화를 위해 세마포어 초기화 값이 1인 unnamed semaphore를 생성한다.
 * @param aSemaphore 생성 세마포어
 * @param aSemName 생성될 세마포어의 이름
 * @param aInitValue 세마포어 초기값
 * @param aErrorStack 에러 스택
 * @remark POSIX 세마포어를 사용한다.
 * @see sem_init()
 */
stlStatus stpCreate( stlSemaphore  * aSemaphore,
                     stlChar       * aSemName,
                     stlUInt32       aInitValue,
                     stlErrorStack * aErrorStack )
{
    stlSize sNameLen;
    HANDLE  sSem;
    DWORD   sError;

    sNameLen = stlStrlen( aSemName );
    STL_TRY_THROW( sNameLen <= STL_MAX_SEM_NAME, RAMP_ERR_NAMETOOLONG );

    sSem = CreateSemaphore( NULL,
                            aInitValue,
                            STL_INT32_MAX,
                            NULL );

    STL_TRY_THROW( sSem != NULL, RAMP_ERR_SEM_CREATE );

    stlStrncpy( aSemaphore->mName, aSemName, STL_MAX_SEM_NAME + 1 );

    aSemaphore->mHandle = sSem;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NAMETOOLONG )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_NAMETOOLONG,
                      NULL,
                      aErrorStack,
                      aSemName );
        steSetSystemError( GetLastError(), aErrorStack );
    }

    STL_CATCH( RAMP_ERR_SEM_CREATE )
    {
        sError = GetLastError();

        switch( sError )
        {
            case ERROR_ALREADY_EXISTS:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_SEM_CREATE,
                              NULL,
                              aErrorStack );
                break;
            case ERROR_ACCESS_DENIED:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_SEM_CREATE,
                              NULL,
                              aErrorStack );
                break;
            default:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_SEM_CREATE,
                              NULL,
                              aErrorStack );
                break;
        }

        steSetSystemError( GetLastError(), aErrorStack );
    }

    STL_FINISH;

    aSemaphore->mName[0] = '\0';
    aSemaphore->mHandle = INVALID_HANDLE_VALUE;

    return STL_FAILURE;
}

/*
 * @brief 세마포어를 획득한다.
 * @param aHandle 획득할 세마포어 핸들
 * @param aErrorStack 에러 스택
 * @see sem_wait()
 */
stlStatus stpAcquire( stlSemHandle  * aHandle,
                      stlErrorStack * aErrorStack )
{
    DWORD sResult;

    sResult = WaitForSingleObject( *aHandle, INFINITE );

    STL_TRY_THROW( (sResult == WAIT_OBJECT_0) || (sResult == WAIT_ABANDONED), RAMP_ERR_WAIT );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_WAIT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SEM_ACQUIRE,
                      NULL,
                      aErrorStack );
        steSetSystemError( GetLastError(), aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * @brief 세마포어를 획득을 시도한다.
 * @param aHandle 획득할 세마포어 핸들
 * @param aErrorStack 에러 스택
 * @see sem_trywait()
 */
stlStatus stpTryAcquire( stlSemHandle  * aHandle,
                         stlBool       * aIsSuccess,
                         stlErrorStack * aErrorStack )
{
    DWORD sResult;

    *aIsSuccess = STL_TRUE;

    sResult = WaitForSingleObject( *aHandle, 0 );

    switch( sResult )
    {
        case WAIT_OBJECT_0:
        case WAIT_ABANDONED:
            break;
        case WAIT_TIMEOUT:
            *aIsSuccess = STL_FALSE;
            break;
        default:
            STL_THROW( RAMP_ERR_WAIT );
            break;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_WAIT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SEM_ACQUIRE,
                      NULL,
                      aErrorStack );
        steSetSystemError( GetLastError(), aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * @brief 특정 대기 시간동안 세마포어를 획득을 시도한다.
 * @param aHandle 획득할 세마포어 핸들
 * @param aTimeout 대기 시간
 * @param aErrorStack 에러 스택
 * @see sem_timedwait()
 */
stlStatus stpTimedAcquire( stlSemHandle  * aHandle,
                           stlInterval     aTimeout,
                           stlBool       * aIsTimedOut,
                           stlErrorStack * aErrorStack )
{
    DWORD sResult;

    *aIsTimedOut = STL_FALSE;

    sResult = WaitForSingleObject( *aHandle, STL_GET_AS_MSEC_TIME(aTimeout) );

    switch( sResult )
    {
        case WAIT_OBJECT_0:
        case WAIT_ABANDONED:
            break;
        case WAIT_TIMEOUT:
            *aIsTimedOut = STL_TRUE;
            break;
        default:
            STL_THROW( RAMP_ERR_WAIT );
            break;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_WAIT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SEM_ACQUIRE,
                      NULL,
                      aErrorStack );
        steSetSystemError( GetLastError(), aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * @brief 세마포어를 풀어준다.
 * @param aSemaphore 풀어줄 세마포어 핸들
 * @param aErrorStack 에러 스택
 * @see sem_post()
 */
stlStatus stpRelease( stlSemHandle  * aHandle,
                      stlErrorStack * aErrorStack )
{
    STL_TRY_THROW( ReleaseSemaphore(*aHandle, 1, NULL) != 0, RAMP_ERR_POST );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_POST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SEM_RELEASE,
                      NULL,
                      aErrorStack );
        steSetSystemError( GetLastError(), aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * @brief 세마포어를 제거한다.
 * @param aSemaphore 제거할 세마포어
 * @param aErrorStack 에러 스택
 * @see sem_destroy()
 */
stlStatus stpDestroy( stlSemaphore  * aSemaphore,
                      stlErrorStack * aErrorStack )
{
    STL_TRY_THROW( CloseHandle(aSemaphore->mHandle) != 0, RAMP_ERR_DESTROY );

    aSemaphore->mName[0] = '\0';
    aSemaphore->mHandle = INVALID_HANDLE_VALUE;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_DESTROY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SEM_DESTROY,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}

/*
 * @brief named 세마포어를 생성한다.
 * @param[in] aSemaphore 생성할 세마포어
 * @param[in] aSemName 생성할 세마포어 이름
 * @param[in] aInitValue 세마포어 초기값
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 */
stlStatus stpNamedCreate( stlNamedSemaphore * aSemaphore,
                          stlChar           * aSemName,
                          stlUInt32           aInitValue,
                          stlErrorStack     * aErrorStack )
{
    stlSize sNameLen;
    HANDLE  sSem;
    DWORD   sError;

    sNameLen = stlStrlen(aSemName);
    STL_TRY_THROW( sNameLen <= STL_MAX_NAMED_SEM_NAME, RAMP_ERR_NAMETOOLONG );

    sSem = CreateSemaphore( NULL,
                            aInitValue,
                            STL_INT32_MAX,
                            aSemName );

    STL_TRY_THROW( sSem != NULL, RAMP_ERR_SEM_CREATE );

    stlStrncpy( aSemaphore->mName, aSemName, STL_MAX_SEM_NAME + 1 );

    aSemaphore->mHandle = sSem;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NAMETOOLONG )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_NAMETOOLONG,
                      NULL,
                      aErrorStack,
                      aSemName );
        steSetSystemError( GetLastError(), aErrorStack );
    }

    STL_CATCH( RAMP_ERR_SEM_CREATE )
    {
        sError = GetLastError();

        switch( sError )
        {
            case ERROR_ALREADY_EXISTS:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_SEM_CREATE,
                              NULL,
                              aErrorStack );
                break;
            case ERROR_ACCESS_DENIED:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_SEM_CREATE,
                              NULL,
                              aErrorStack );
                break;
            default:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_SEM_CREATE,
                              NULL,
                              aErrorStack );
                break;
        }

        steSetSystemError( GetLastError(), aErrorStack );
    }

    STL_FINISH;

    aSemaphore->mName[0] = '\0';
    aSemaphore->mHandle = INVALID_HANDLE_VALUE;

    return STL_FAILURE;
}

/*
 * @brief named 세마포어를 오픈 한다.
 * @param[out] aSemaphore 오픈할 세마포어
 * @param[in] aSemName 오픈할 세마포어 이름
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 */
stlStatus stpOpen( stlNamedSemaphore * aSemaphore,
                   stlChar           * aSemName,
                   stlErrorStack     * aErrorStack )
{
    stlSize sNameLen;
    HANDLE  sSem;

    sNameLen = stlStrlen( aSemName );
    STL_TRY_THROW( sNameLen < STL_MAX_NAMED_SEM_NAME, RAMP_ERR_NAMETOOLONG );

    stlSnprintf( aSemaphore->mName,
                 STL_MAX_NAMED_SEM_NAME + 1,
                 "%s",
                 aSemName );

    sSem= OpenSemaphore( SEMAPHORE_ALL_ACCESS, FALSE, aSemName );

    STL_TRY_THROW( aSemaphore->mHandle != NULL, RAMP_ERR_OPEN );

    aSemaphore->mHandle = sSem;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NAMETOOLONG )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_NAMETOOLONG,
                      NULL,
                      aErrorStack,
                      aSemName );
        steSetSystemError( GetLastError(), aErrorStack );
    }

    STL_CATCH( RAMP_ERR_OPEN )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SEM_OPEN,
                      NULL,
                      aErrorStack );
        steSetSystemError( GetLastError(), aErrorStack );
    }

    STL_FINISH;

    aSemaphore->mName[0] = '\0';
    aSemaphore->mHandle = INVALID_HANDLE_VALUE;

    return STL_FAILURE;
}

/*
 * @brief Named 세마포어를 획득한다.
 * @param aHandle 획득할 세마포어 핸들
 * @param aErrorStack 에러 스택
 * @see sem_wait()
 */
stlStatus stpNamedAcquire( stlNamedSemHandle * aHandle,
                           stlErrorStack     * aErrorStack )
{
    return stpAcquire( aHandle, aErrorStack );
}

/*
 * @brief Named 세마포어를 풀어준다.
 * @param aSemaphore 풀어줄 세마포어 핸들
 * @param aErrorStack 에러 스택
 * @see sem_post()
 */
stlStatus stpNamedRelease( stlNamedSemHandle * aHandle,
                           stlErrorStack     * aErrorStack )
{
    return stpRelease( aHandle, aErrorStack );
}

/*
 * @brief named 세마포어를 닫는다.
 * @param[in] aSemaphore 닫을 세마포어
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 */
stlStatus stpClose( stlNamedSemaphore * aSemaphore,
                    stlErrorStack     * aErrorStack )
{
    STL_TRY_THROW( CloseHandle(aSemaphore->mHandle) != 0, RAMP_ERR_CLOSE );

    aSemaphore->mName[0] = '\0';
    aSemaphore->mHandle = INVALID_HANDLE_VALUE;

    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_CLOSE)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SEM_CLOSE,
                      NULL,
                      aErrorStack );
        steSetSystemError( GetLastError(), aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * @brief named 세마포어를 제거한다.
 * @param[in] aSemaphore 제거할 세마포어
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 */
stlStatus stpUnlink( stlNamedSemaphore * aSemaphore,
                     stlErrorStack     * aErrorStack )
{
    STL_TRY_THROW( CloseHandle(aSemaphore->mHandle) != 0, RAMP_ERR_DESTROY );

    aSemaphore->mName[0] = '\0';
    aSemaphore->mHandle = INVALID_HANDLE_VALUE;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_DESTROY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SEM_DESTROY,
                      NULL,
                      aErrorStack );
        steSetSystemError( GetLastError(), aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * @brief 세마포어 값을 얻는다.
 * @param[in] aSemaphore 세마포어
 * @param[out] aSemValue 세마포어 값
 * @param[out] aErrorStack 에러 스택
 */
stlStatus stpGetValue( stlSemaphore  * aSemaphore,
                       stlInt32      * aSemValue,
                       stlErrorStack * aErrorStack )
{
    stpNtQuerySemaphore            sNtQuerySemaphore = NULL;
    stpSEMAPHORE_BASIC_INFORMATION sBasicInfo = {0, };
    NTSTATUS                       sStatus;

    sNtQuerySemaphore = (stpNtQuerySemaphore)GetProcAddress( GetModuleHandle ("ntdll.dll"), "NtQuerySemaphore" );

    STL_ASSERT( sNtQuerySemaphore != NULL );
    
    sStatus = sNtQuerySemaphore( aSemaphore->mHandle,
                                 0,
                                 &sBasicInfo,
                                 STL_SIZEOF(stpSEMAPHORE_BASIC_INFORMATION),
                                 NULL );

    STL_TRY_THROW( sStatus == ERROR_SUCCESS, RAMP_ERR_SEM_GET_VALUE );

    *aSemValue = (stlInt32)sBasicInfo.mCurrentCount;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SEM_GET_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SEM_GET_VALUE,
                      NULL,
                      aErrorStack );
        steSetSystemError( GetLastError(), aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

