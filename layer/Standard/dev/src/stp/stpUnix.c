/*******************************************************************************
 * stpUnix.c
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
    stlSize   sNameLen;

    sNameLen = stlStrlen( aSemName );
    STL_TRY_THROW( sNameLen <= STL_MAX_SEM_NAME, RAMP_ERR_NAMETOOLONG );

#ifdef STC_USE_SEMAPHORE
    
    /*
     * 프로세스간 동기화를 위해 unnamed semaphore를 생성한다.
     */
    STL_TRY_THROW( sem_init(&(aSemaphore->mHandle), 1, aInitValue) == 0,
                   RAMP_ERR_INIT );
    
#elif STC_USE_SHARED_MUTEX
    
    stlSemHandle        * sSemHandle;
    stlInt32              sResult;
    pthread_mutexattr_t   sMutexAttr;
    pthread_condattr_t    sCondAttr;

    sSemHandle = &(aSemaphore->mHandle);
    
    sResult = pthread_mutexattr_init( &sMutexAttr );
    STL_TRY_THROW( sResult == 0, RAMP_ERR_INIT );
    
    sResult = pthread_mutexattr_setpshared( &sMutexAttr, PTHREAD_PROCESS_SHARED );
    STL_TRY_THROW( sResult == 0, RAMP_ERR_INIT );

    sResult = pthread_mutex_init( &(sSemHandle->mMutex), &sMutexAttr );
    STL_TRY_THROW( sResult == 0, RAMP_ERR_INIT );

    sResult = pthread_condattr_init( &sCondAttr );
    STL_TRY_THROW( sResult == 0, RAMP_ERR_INIT );
    
    sResult = pthread_condattr_setpshared( &sCondAttr, PTHREAD_PROCESS_SHARED );
    STL_TRY_THROW( sResult == 0, RAMP_ERR_INIT );

    sResult = pthread_cond_init( &(sSemHandle->mCond), &sCondAttr );
    STL_TRY_THROW( sResult == 0, RAMP_ERR_INIT );

    sSemHandle->mCount = aInitValue;
    
#else
    
    STL_ASSERT( STL_FALSE );
    
#endif

    stlStrncpy( aSemaphore->mName, aSemName, STL_MAX_SEM_NAME + 1);

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NAMETOOLONG )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_NAMETOOLONG,
                      NULL,
                      aErrorStack,
                      aSemName );
        steSetSystemError( errno, aErrorStack );
    }

#ifdef STC_USE_SEMAPHORE
    STL_CATCH( RAMP_ERR_INIT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SEM_CREATE,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
#elif STC_USE_SHARED_MUTEX
    STL_CATCH( RAMP_ERR_INIT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SEM_CREATE,
                      NULL,
                      aErrorStack );
        steSetSystemError( sResult, aErrorStack );
    }
#else
    STL_ASSERT( STL_FALSE );
#endif

    STL_FINISH;

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
    stlInt32 sResult;

#ifdef STC_USE_SEMAPHORE
    
    do
    {
        sResult = sem_wait( aHandle );
    } while( (sResult == -1) && (errno == EINTR) );
    
    STL_TRY_THROW( sResult == 0, RAMP_ERR_WAIT );

#elif STC_USE_SHARED_MUTEX

    stlInt32 sState = 0;
    
    sResult = pthread_mutex_lock( &(aHandle->mMutex) );
    STL_TRY_THROW( sResult == 0, RAMP_ERR_WAIT );
    sState = 1;

    while( aHandle->mCount == 0 )
    {
        sResult = pthread_cond_wait( &(aHandle->mCond), &(aHandle->mMutex) );
        STL_TRY_THROW( sResult == 0, RAMP_ERR_WAIT );

        if( aHandle->mCount > 0 )
        {
            break;
        }
    }

    aHandle->mCount--;
    
    sState = 0;
    sResult = pthread_mutex_unlock( &(aHandle->mMutex) );
    STL_TRY_THROW( sResult == 0, RAMP_ERR_WAIT );
    
#else

    STL_ASSERT( STL_FALSE );
    
#endif
    
    return STL_SUCCESS;

#ifdef STC_USE_SEMAPHORE
    STL_CATCH( RAMP_ERR_WAIT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SEM_ACQUIRE,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
#elif STC_USE_SHARED_MUTEX
    STL_CATCH( RAMP_ERR_WAIT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SEM_ACQUIRE,
                      NULL,
                      aErrorStack );
        steSetSystemError( sResult, aErrorStack );
    }
#else
    STL_ASSERT( STL_FALSE );
#endif

    STL_FINISH;

#if STC_USE_SHARED_MUTEX
    if( sState == 1 )
    {
        (void) pthread_mutex_unlock( &(aHandle->mMutex) );
    }
#endif
    
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
    stlInt32 sResult;

    *aIsSuccess = STL_TRUE;
    
#ifdef STC_USE_SEMAPHORE
    
    do
    {
        sResult = sem_trywait( aHandle );
    } while( (sResult == -1) && (errno == EINTR) );

    if( sResult != 0 )
    {
        STL_TRY_THROW( errno == EAGAIN, RAMP_ERR );
        
        *aIsSuccess = STL_FALSE;
    }
    
#elif STC_USE_SHARED_MUTEX

    sResult = pthread_mutex_lock( &(aHandle->mMutex) );
    STL_TRY_THROW( sResult == 0, RAMP_ERR );

    if( aHandle->mCount == 0 )
    {
        *aIsSuccess = STL_FALSE;
    }
    else
    {
        aHandle->mCount--;
    }

    sResult = pthread_mutex_unlock( &(aHandle->mMutex) );
    STL_TRY_THROW( sResult == 0, RAMP_ERR );

#else
    
    STL_ASSERT( STL_FALSE );
    
#endif
    
    return STL_SUCCESS;

#ifdef STC_USE_SEMAPHORE
    STL_CATCH( RAMP_ERR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SEM_ACQUIRE,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
#elif STC_USE_SHARED_MUTEX
    STL_CATCH( RAMP_ERR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SEM_ACQUIRE,
                      NULL,
                      aErrorStack );
        steSetSystemError( sResult, aErrorStack );
    }
#else
    STL_ASSERT( STL_FALSE );
#endif

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
    stlInt32        sResult;
    struct timespec sTimeout;
    stlInt64        sUsec;

    *aIsTimedOut = STL_FALSE;

    clock_gettime( CLOCK_REALTIME, &sTimeout );
    
    sTimeout.tv_sec += STL_GET_SEC_TIME(aTimeout);
    sUsec = STL_GET_USEC_TIME(aTimeout) + (sTimeout.tv_nsec / 1000);
    sTimeout.tv_sec += STL_GET_SEC_TIME( sUsec );
    sTimeout.tv_nsec = STL_GET_USEC_TIME( sUsec ) * 1000;
    
#ifdef STC_USE_SEMAPHORE
    
    do
    {
        sResult = sem_timedwait( aHandle, &sTimeout );
    } while( (sResult == -1) && (errno == EINTR) );

    if( sResult != 0 )
    {
        STL_TRY_THROW( errno == ETIMEDOUT, RAMP_ERR );
        
        *aIsTimedOut = STL_TRUE;
    }
    
#elif STC_USE_SHARED_MUTEX

    stlInt32 sState = 0;

    sResult = pthread_mutex_lock( &(aHandle->mMutex) );
    STL_TRY_THROW( sResult == 0, RAMP_ERR );
    sState = 1;

    while( aHandle->mCount == 0 )
    {
        sResult = pthread_cond_timedwait( &(aHandle->mCond),
                                          &(aHandle->mMutex),
                                          &sTimeout );

        if( sResult != 0 )
        {
            STL_TRY_THROW( sResult == ETIMEDOUT, RAMP_ERR );
            *aIsTimedOut = STL_TRUE;
            break;
        }
    }

    if( sResult == 0 )
    {
        aHandle->mCount--;
    }

    sState = 0;
    sResult = pthread_mutex_unlock( &(aHandle->mMutex) );
    STL_TRY_THROW( sResult == 0, RAMP_ERR );
    
#else
    
    STL_ASSERT( STL_FALSE );
    
#endif
    
    return STL_SUCCESS;

#ifdef STC_USE_SEMAPHORE
    STL_CATCH( RAMP_ERR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SEM_ACQUIRE,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
#elif STC_USE_SHARED_MUTEX
    STL_CATCH( RAMP_ERR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SEM_ACQUIRE,
                      NULL,
                      aErrorStack );
        steSetSystemError( sResult, aErrorStack );
    }
#else
    STL_ASSERT( STL_FALSE );
#endif

    STL_FINISH;

#if STC_USE_SHARED_MUTEX
    if( sState == 1 )
    {
        (void) pthread_mutex_unlock( &(aHandle->mMutex) );
    }
#endif
    
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
    stlInt32 sResult;

#ifdef STC_USE_SEMAPHORE
    
    do
    {
        sResult = sem_post( aHandle );
    } while( (sResult == -1) && (errno == EINTR) );

    STL_TRY_THROW( sResult == 0, RAMP_ERR_POST );

#elif STC_USE_SHARED_MUTEX

    sResult = pthread_mutex_lock( &(aHandle->mMutex) );
    STL_TRY_THROW( sResult == 0, RAMP_ERR_POST );

    aHandle->mCount++;

    sResult = pthread_cond_broadcast( &(aHandle->mCond) );
    STL_TRY_THROW( sResult == 0, RAMP_ERR_POST );

    sResult = pthread_mutex_unlock( &(aHandle->mMutex) );
    STL_TRY_THROW( sResult == 0, RAMP_ERR_POST );
    
#else
    
    STL_ASSERT( STL_FALSE );
    
#endif

    return STL_SUCCESS;

#ifdef STC_USE_SEMAPHORE
    STL_CATCH( RAMP_ERR_POST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SEM_RELEASE,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
#elif STC_USE_SHARED_MUTEX
    STL_CATCH( RAMP_ERR_POST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SEM_RELEASE,
                      NULL,
                      aErrorStack );
        steSetSystemError( sResult, aErrorStack );
    }
#else
    STL_ASSERT( STL_FALSE );
#endif

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
#ifdef STC_USE_SEMAPHORE
    
    STL_TRY_THROW( sem_destroy(&(aSemaphore->mHandle)) == 0,
                   RAMP_ERR_DESTROY );

#elif STC_USE_SHARED_MUTEX

    if( pthread_mutex_destroy( &(aSemaphore->mHandle.mMutex) ) == EBUSY )
    {
        (void)pthread_mutex_unlock( &(aSemaphore->mHandle.mMutex) );
    }
    (void)pthread_cond_destroy( &(aSemaphore->mHandle.mCond) );
    
#else
    
    STL_ASSERT( STL_FALSE );
    
#endif

    aSemaphore->mName[0] = '\0';

    return STL_SUCCESS;

#ifdef STC_USE_SEMAPHORE
    STL_CATCH( RAMP_ERR_DESTROY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SEM_DESTROY,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }

    STL_FINISH;
    
#elif STC_USE_SHARED_MUTEX
#else
    STL_ASSERT( STL_FALSE );
#endif

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
    stlSize   sNameLen;

    sNameLen = stlStrlen( aSemName );
    STL_TRY_THROW( sNameLen < STL_MAX_NAMED_SEM_NAME, RAMP_ERR_NAMETOOLONG );

    /*
     * 프로세스간 동기화를 위해 named semaphore를 생성한다.
     */

    stlSnprintf( aSemaphore->mName,
                 STL_MAX_NAMED_SEM_NAME,
                 "/%s",
                 aSemName );
    do
    {
        aSemaphore->mHandle = sem_open( aSemaphore->mName,
                                        O_CREAT,
                                        0644,
                                        aInitValue );
    } while( (aSemaphore->mHandle == (sem_t *)SEM_FAILED) && (errno == EINTR) );

    STL_TRY_THROW( aSemaphore->mHandle != (sem_t *)SEM_FAILED, RAMP_ERR_OPEN );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NAMETOOLONG )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_NAMETOOLONG,
                      NULL,
                      aErrorStack,
                      aSemName );
        steSetSystemError( errno, aErrorStack );
    }

    STL_CATCH( RAMP_ERR_OPEN )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SEM_OPEN,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }

    STL_FINISH;

    aSemaphore->mName[0] = '\0';

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
    stlSize   sNameLen;

    sNameLen = stlStrlen( aSemName );
    STL_TRY_THROW( sNameLen < STL_MAX_NAMED_SEM_NAME, RAMP_ERR_NAMETOOLONG );

    stlSnprintf( aSemaphore->mName,
                 STL_MAX_NAMED_SEM_NAME,
                 "/%s",
                 aSemName );

    do
    {
        aSemaphore->mHandle = sem_open( aSemaphore->mName,
                                        0,
                                        644,
                                        0 );
    } while( (aSemaphore->mHandle == (sem_t *)SEM_FAILED) && (errno == EINTR) );

    STL_TRY_THROW( aSemaphore->mHandle != (sem_t *)SEM_FAILED, RAMP_ERR_OPEN );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NAMETOOLONG )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_NAMETOOLONG,
                      NULL,
                      aErrorStack,
                      aSemName );
        steSetSystemError( errno, aErrorStack );
    }

    STL_CATCH( RAMP_ERR_OPEN )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SEM_OPEN,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }

    STL_FINISH;

    aSemaphore->mName[0] = '\0';

    return STL_FAILURE;
}

/*
 * @brief Named 세마포어를 획득한다.
 * @param aHandle 획득할 세마포어 핸들
 * @param aErrorStack 에러 스택
 * @see sem_wait()
 */
stlStatus stpNamedAcquire( stlNamedSemHandle  * aHandle,
                           stlErrorStack      * aErrorStack )
{
    stlInt32 sResult;

    do
    {
        sResult = sem_wait( aHandle );
    } while( (sResult == -1) && (errno == EINTR) );
    
    STL_TRY_THROW( sResult == 0, RAMP_ERR_WAIT );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_WAIT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SEM_ACQUIRE,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * @brief Named 세마포어를 풀어준다.
 * @param aSemaphore 풀어줄 세마포어 핸들
 * @param aErrorStack 에러 스택
 * @see sem_post()
 */
stlStatus stpNamedRelease( stlNamedSemHandle  * aHandle,
                           stlErrorStack      * aErrorStack )
{
    stlInt32 sResult;

    do
    {
        sResult = sem_post( aHandle );
    } while( (sResult == -1) && (errno == EINTR) );

    STL_TRY_THROW( sResult == 0, RAMP_ERR_POST );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_POST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SEM_RELEASE,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
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
    STL_TRY_THROW( sem_close( aSemaphore->mHandle ) == 0, RAMP_ERR_CLOSE );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CLOSE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SEM_CLOSE,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
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
    STL_TRY_THROW( sem_unlink( aSemaphore->mName ) == 0, RAMP_ERR_UNLINK );

    aSemaphore->mName[0] = '\0';

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_UNLINK )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SEM_UNLINK,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
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
#ifdef STC_USE_SEMAPHORE

    STL_TRY_THROW( sem_getvalue( &(aSemaphore->mHandle),
                                 aSemValue ) == 0,
                   RAMP_ERR_SEM_GET_VALUE );

#elif STC_USE_SHARED_MUTEX

    *aSemValue = aSemaphore->mHandle.mCount;

#else
    
    STL_ASSERT( STL_FALSE );
    
#endif
    return STL_SUCCESS;

#ifdef STC_USE_SEMAPHORE
    
    STL_CATCH( RAMP_ERR_SEM_GET_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SEM_GET_VALUE,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
    
#endif
}


