/*******************************************************************************
 * stlSemaphore.c
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
 * @file stlSemaphore.c
 * @brief Standard Layer Semaphore Routines
 */

#include <stlSemaphore.h>
#include <stlTime.h>
#include <stp.h>

/**
 * @addtogroup stlSemaphore
 * @{
 */

/**
 * @brief 세마포어를 생성한다.
 * @param[out] aSemaphore 생성된 세마포어
 * @param[in] aSemName 생성할 세마포어 이름
 * @param[in] aInitValue 세마포어 초기값
 * @param[out] aErrorStack 에러 스택
 * @remark @a aSemName은 Null Terminated String이다.
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_NAMETOOLONG]
 *     aSemName이 STL_MAX_SEM_NAME보다 큰 경우에 발생함.
 * [STL_ERRCODE_SEM_INIT]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
stlStatus stlCreateSemaphore( stlSemaphore  * aSemaphore,
                              stlChar       * aSemName,
                              stlUInt32       aInitValue,
                              stlErrorStack * aErrorStack )
{
    return stpCreate( aSemaphore, aSemName, aInitValue, aErrorStack );
}

/**
 * @brief 세마포어를 획득한다.
 * @param[in] aSemaphore 획득할 세마포어
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_SEM_ACQUIRE]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
stlStatus stlAcquireSemaphore( stlSemaphore  * aSemaphore,
                               stlErrorStack * aErrorStack )
{
    return stpAcquire( &aSemaphore->mHandle, aErrorStack );
}

/**
 * @brief 세마포어를 획득을 시도한다.
 * @param[in] aSemaphore 획득할 세마포어
 * @param[in] aIsSuccess 세마포어 획득 성공 여부
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_SEM_ACQUIRE]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
stlStatus stlTryAcquireSemaphore( stlSemaphore  * aSemaphore,
                                  stlBool       * aIsSuccess,
                                  stlErrorStack * aErrorStack )
{
    return stpTryAcquire( &aSemaphore->mHandle, aIsSuccess, aErrorStack );
}

/**
 * @brief 특정 대기 시간동안 세마포어를 획득을 시도한다.
 * @param[in] aSemaphore 획득할 세마포어
 * @param[in] aTimeout 대기 시간 (usec)
 * @param[in] aIsTimedOut 대기 시간 초과 여부
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_SEM_ACQUIRE]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 * @remarks aTimeout이 STL_UINT64_MAX라면 stlAcquireSemaphore를 사용한다.
 *    <BR>  aTimeout이 0이면 stlTryAcquireSemaphore를 사용한다.
 */
stlStatus stlTimedAcquireSemaphore( stlSemaphore  * aSemaphore,
                                    stlInterval     aTimeout,
                                    stlBool       * aIsTimedOut,
                                    stlErrorStack * aErrorStack )
{
    stlBool sIsSuccess = STL_TRUE;
    
    *aIsTimedOut = STL_FALSE;
            
    if( aTimeout == 0 )
    {
        /**
         * aTimeout이 0 이라면 Try 방식을 사용한다.
         * - stlNow를 사용하지 않기 위해서
         */
        STL_TRY( stpTryAcquire( &aSemaphore->mHandle,
                                &sIsSuccess,
                                aErrorStack )
                 == STL_SUCCESS );

        if( sIsSuccess == STL_FALSE )
        {
            *aIsTimedOut = STL_TRUE;
        }
    }
    else
    {
        if( aTimeout == STL_INFINITE_TIME )
        {
            /**
             * aTimeout이 무한대라면 무한대기 한다.
             */
            STL_TRY( stpAcquire( &aSemaphore->mHandle,
                                 aErrorStack )
                     == STL_SUCCESS );
        }
        else
        {
            STL_TRY( stpTimedAcquire( &aSemaphore->mHandle,
                                      aTimeout,
                                      aIsTimedOut,
                                      aErrorStack )
                     == STL_SUCCESS );
        }
    }

    return  STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 세마포어를 풀어준다.
 * @param[in] aSemaphore 풀어줄 세마포어
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_SEM_RELEASE]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
stlStatus stlReleaseSemaphore( stlSemaphore  * aSemaphore,
                               stlErrorStack * aErrorStack )
{
    return stpRelease( &aSemaphore->mHandle, aErrorStack );
}

/**
 * @brief 세마포어를 제거한다.
 * @param[in] aSemaphore 제거할 세마포어
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_SEM_DESTROY]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
stlStatus stlDestroySemaphore( stlSemaphore  * aSemaphore,
                               stlErrorStack * aErrorStack )
{
    return stpDestroy( aSemaphore, aErrorStack );
}

/**
 * @brief named 세마포어를 생성 한다.
 * @param[out] aSemaphore 생성할 세마포어
 * @param[in] aSemName 생성할 세마포어 이름
 * @param[in] aInitValue 세마포어 초기값
 * @param[out] aErrorStack 에러 스택
 * @remark @a aSemName은 Null Terminated String이다.
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_NAMETOOLONG]
 *     aSemName이 STL_MAX_NAMED_SEM_NAME보다 큰 경우에 발생함.
 * [STL_ERRCODE_SEM_OPEN]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
stlStatus stlCreateNamedSemaphore( stlNamedSemaphore * aSemaphore,
                                   stlChar           * aSemName,
                                   stlUInt32           aInitValue,
                                   stlErrorStack     * aErrorStack )
{
    return stpNamedCreate( aSemaphore, aSemName, aInitValue, aErrorStack);
}

/**
 * @brief named 세마포어를 오픈 한다.
 * @param[out] aSemaphore 오픈 할 세마포어
 * @param[in] aSemName 생성할 세마포어 이름
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_NAMETOOLONG]
 *     aSemName이 STL_MAX_NAMED_SEM_NAME보다 큰 경우에 발생함.
 * [STL_ERRCODE_SEM_OPEN]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
stlStatus stlOpenNamedSemaphore( stlNamedSemaphore * aSemaphore,
                                 stlChar           * aSemName,
                                 stlErrorStack     * aErrorStack )
{
    return stpOpen( aSemaphore, aSemName, aErrorStack);
}

/**
 * @brief named 세마포어를 획득한다.
 * @param[in] aSemaphore 획득할 세마포어
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_SEM_ACQUIRE]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
stlStatus stlAcquireNamedSemaphore( stlNamedSemaphore * aSemaphore,
                                    stlErrorStack     * aErrorStack )
{
    return stpNamedAcquire( aSemaphore->mHandle, aErrorStack );
}

/**
 * @brief named 세마포어를 풀어준다.
 * @param[in] aSemaphore 풀어줄 세마포어
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_SEM_RELEASE]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
stlStatus stlReleaseNamedSemaphore( stlNamedSemaphore * aSemaphore,
                                    stlErrorStack     * aErrorStack )
{
    return stpNamedRelease( aSemaphore->mHandle, aErrorStack );
}

/**
 * @brief named 세마포어를 닫는다.
 * @param[in] aSemaphore 닫을 세마포어
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_SEM_CLOSE]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
stlStatus stlCloseNamedSemaphore( stlNamedSemaphore * aSemaphore,
                                  stlErrorStack     * aErrorStack )
{
    return stpClose( aSemaphore, aErrorStack );
}

/**
 * @brief named 세마포어를 제거한다.
 * @param[in] aSemaphore 제거할 세마포어
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_SEM_UNLINK]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
stlStatus stlDestroyNamedSemaphore( stlNamedSemaphore * aSemaphore,
                                    stlErrorStack     * aErrorStack )
{
    STL_TRY( stpClose( aSemaphore, aErrorStack ) == STL_SUCCESS );

    STL_TRY( stpUnlink( aSemaphore, aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 세마포어 값을 얻는다.
 * @param[in] aSemaphore 세마포어
 * @param[out] aSemValue 세마포어 값
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_SEM_GET_VALUE]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
stlStatus stlGetSemaphoreValue( stlSemaphore  * aSemaphore,
                                stlInt32      * aSemValue,
                                stlErrorStack * aErrorStack )
{
    STL_TRY( stpGetValue( aSemaphore,
                          aSemValue,
                          aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
