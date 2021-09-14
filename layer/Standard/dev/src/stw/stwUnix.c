/*******************************************************************************
 * stwUnix.c
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
#include <stlDef.h>
#include <stlError.h>
#include <stlMemorys.h>
#include <stw.h>
#include <ste.h>
#include <stl.h>

/*
 * @brief 특정 시간동안 주어진 소켓들에서 이벤트가 발생을 대기한다.
 * @param[in,out] aPollFd 이벤트를 대기할 소켓 배열
 * @param[in] aPollFdNum 이벤트를 대기할 소켓의 수
 * @param[out] aSigFdNum 이벤트가 발생한 소켓의 수
 * @param[in] aTimeout 이벤트를 대기할 마이크로 초
 *            대기 중 이벤트가 발생하면 @a aTimeout이 다 되지 않아도 함수가 반환된다.
 *            만약 @a aTimeout 이 음수값이면 이벤트가 발생하기 전까지 무한대기 한다.
 * @param aErrorStack 에러 스택
 * @par Error Codes :
 * @code
 * [STL_ERRCODE_TIMEDOUT]
 *     주어진 timeout 동안 이벤트가 발생하지 않았다.
 * [STL_ERRCODE_OUT_OF_MEMORY]
 *     커널 메모리를 할당받을수 없는 경우에 발생함.
 * [STL_ERRCODE_POLL]
 *     poll()이 실패했음. 시스템 에러를 확인해야 한다.
 * [STL_ERRCODE_OVERFLOW_FD]
 *     aPollFd에 있는 fd값이 처리가능한 범위가 아닌 경우 발생함.
 * @endcode
 * @remark 이벤트가 발생한 소켓의 수 @a aSigFdNum 는
 *         함수 호출 후 이벤트가 발생하거나 timeout이 발생해야 설정된다.
 * @remark stlPollFd 구조체의 mRetEvnets 는 STL_SUCCESS이 반환된 후 설정된다.
 */
stlStatus stwPoll( stlPollFd     * aPollFd,
                   stlInt32        aPollFdNum,
                   stlInt32      * aSigFdNum,
                   stlInterval     aTimeout,
                   stlErrorStack * aErrorStack )
{
    stlInt32 sSigFdNum;

#if defined( STC_POLL_USES_POLL )

    stlInt32 sTimeout;
    
    if( aTimeout == STL_INFINITE_TIME )
    {
        sTimeout = -1;
    }
    else
    {
        /* convert microseconds to milliseconds */
        aTimeout /= 1000;

        if( aTimeout > STL_INT32_MAX )
        {
            sTimeout = STL_INT32_MAX;
        }
        else
        {
            sTimeout = aTimeout;
        }
    }

    do
    {
        sSigFdNum = poll( (struct pollfd *)aPollFd, aPollFdNum, sTimeout);
    } while( (sSigFdNum == -1) && (errno == EINTR) );

#else /* SELECT */

    fd_set           sReadSet;
    fd_set           sWriteSet;
    fd_set           sExceptSet;
    stlInt32         sMaxFd = -1;
    struct timeval   sTv;
    struct timeval * sTvPtr;
    stlInt32         sSocketHandle;
    stlInt32         sIdx;

    if( aTimeout == STL_INFINITE_TIME )
    {
        sTvPtr = NULL;
    }
    else
    {
        sTv.tv_sec  = (long)STL_GET_SEC_TIME(aTimeout);
        sTv.tv_usec = (long)STL_GET_USEC_TIME(aTimeout);
        sTvPtr = &sTv;
    }

    FD_ZERO(&sReadSet);
    FD_ZERO(&sWriteSet);
    FD_ZERO(&sExceptSet);

    for( sIdx = 0; sIdx < aPollFdNum; sIdx++ )
    {
        aPollFd[sIdx].mRetEvents = 0;
        sSocketHandle = aPollFd[sIdx].mSocketHandle;

        STL_TRY_THROW( sSocketHandle < FD_SETSIZE, RAMP_ERR_FD_SETSIZE );
        
        if( aPollFd[sIdx].mReqEvents & STL_POLLIN )
        {
            FD_SET( sSocketHandle, &sReadSet );
        }

        if( aPollFd[sIdx].mReqEvents & STL_POLLOUT )
        {
            FD_SET( sSocketHandle, &sWriteSet );
        }

        if( aPollFd[sIdx].mReqEvents & STL_POLLERR )
        {
            FD_SET( sSocketHandle, &sExceptSet );
        }

        if( sSocketHandle > sMaxFd )
        {
            sMaxFd = sSocketHandle;
        }
    }

    do
    {
        sSigFdNum = select( sMaxFd + 1, &sReadSet, &sWriteSet, &sExceptSet, sTvPtr );
    } while( (sSigFdNum == -1) && (errno == EINTR) );

#endif

    if( sSigFdNum == 0 )
    {
        STL_THROW( RAMP_ERR_TIMEDOUT );
    }

    STL_TRY_THROW( sSigFdNum > 0, RAMP_ERR_POLL );

#if defined( STC_POLL_USES_SELECT )

    sSigFdNum = 0;

    for( sIdx = 0; sIdx < aPollFdNum; sIdx++ )
    {
        sSocketHandle = aPollFd[sIdx].mSocketHandle;

        if( FD_ISSET( sSocketHandle, &sReadSet ) )
        {
            aPollFd[sIdx].mRetEvents |= STL_POLLIN;
        }
        
        if( FD_ISSET( sSocketHandle, &sWriteSet ) )
        {
            aPollFd[sIdx].mRetEvents |= STL_POLLOUT;
        }
        
        if( FD_ISSET( sSocketHandle, &sExceptSet ) )
        {
            aPollFd[sIdx].mRetEvents |= STL_POLLERR;
        }

        if( aPollFd[sIdx].mRetEvents != 0 )
        {
            sSigFdNum++;
        }
    }

#endif

    if( aSigFdNum != NULL )
    {
        *aSigFdNum = sSigFdNum;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_TIMEDOUT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_TIMEDOUT,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_POLL )
    {
        switch( errno )
        {
            case ENOMEM :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_OUT_OF_MEMORY,
                              NULL,
                              aErrorStack );
                break;
            default :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_POLL,
                              NULL,
                              aErrorStack );
                break;
                
        }
        steSetSystemError( errno, aErrorStack );
    }
    
#if defined( STC_POLL_USES_SELECT )

    STL_CATCH( RAMP_ERR_FD_SETSIZE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_OVERFLOW_FD,
                      NULL,
                      aErrorStack );
    }

#endif

    STL_FINISH;

    if( aSigFdNum != NULL )
    {
        *aSigFdNum = 0;
    }

    return STL_FAILURE;
}

/*
 * @brief pollset 객체를 생성하고 초기화한다.
 * @param[in] aPollSet pollset 객체의 주소
 * @param[in] aPollFdNum @a aPollFd 배열을 구성하는 stlPollFd의 수
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_OVERFLOW_FD]
 *     입력받은 stlPollFd의 수가 너무 크다.
 * @endcode
 */
stlStatus stwCreatePollSet( stlPollSet    * aPollSet,
                            stlInt32        aPollFdNum,
                            stlErrorStack * aErrorStack )
{
    stlInt32        sIdx;

    aPollSet->mMaxPollFdNum = aPollFdNum;
    aPollSet->mCurPollFdNum = 0;
    aPollSet->mQuerySet = NULL;
    aPollSet->mResultSet = NULL;

#if defined( STC_POLLSET_USES_SELECT )

    STL_TRY_THROW( aPollFdNum <= FD_SETSIZE, RAMP_ERR_FD_SETSIZE );

    FD_ZERO(&(aPollSet->mReadSet));
    FD_ZERO(&(aPollSet->mWriteSet));
    FD_ZERO(&(aPollSet->mExceptSet));

    aPollSet->mMaxFd = 0;

#endif

#if defined( STC_POLLSET_USES_EPOLL )

    aPollSet->mEpollEvent = NULL;
    aPollSet->mEpollFd = epoll_create( aPollFdNum );

    STL_TRY_THROW( aPollSet->mEpollFd >= 0, RAMP_ERR_EPOLL_CREATE );
    
    STL_TRY( stlAllocHeap( (void**)&aPollSet->mEpollEvent,
                           aPollFdNum * STL_SIZEOF(stlEpollEvent),
                           aErrorStack )
             == STL_SUCCESS );

#endif

#if defined( STC_POLLSET_USES_POLL )

    aPollSet->mPollFds = NULL;
    
    STL_TRY( stlAllocHeap( (void**)&aPollSet->mPollFds,
                           aPollFdNum * STL_SIZEOF(stlPollFd),
                           aErrorStack )
             == STL_SUCCESS );

#endif

    STL_TRY( stlAllocHeap( (void**)&aPollSet->mQuerySet,
                           aPollFdNum * STL_SIZEOF(stlPollFdEx),
                           aErrorStack )
             == STL_SUCCESS );

    STL_TRY( stlAllocHeap( (void**)&aPollSet->mResultSet,
                           aPollFdNum * STL_SIZEOF(stlPollFdEx),
                           aErrorStack )
             == STL_SUCCESS );

    for( sIdx = 0; sIdx < aPollFdNum; sIdx++ )
    {
        aPollSet->mQuerySet[sIdx].mPollFd.mSocketHandle = -1;
    }

    return STL_SUCCESS;

#if defined( STC_POLLSET_USES_SELECT )

    STL_CATCH( RAMP_ERR_FD_SETSIZE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_OVERFLOW_FD,
                      NULL,
                      aErrorStack );
    }

#endif

#if defined( STC_POLLSET_USES_EPOLL )

    STL_CATCH( RAMP_ERR_EPOLL_CREATE )
    {
        switch( errno )
        {
            case ENFILE:
            case EMFILE:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_INSUFFICIENT_RESOURCE,
                              NULL,
                              aErrorStack );
                break;
            case EINVAL:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_INVALID_ARGUMENT,
                              NULL,
                              aErrorStack );
                break;
            case ENOMEM:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_OUT_OF_MEMORY,
                              NULL,
                              aErrorStack );
                break;
            default :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_POLLSET_CREATE,
                              NULL,
                              aErrorStack );
                break;
                
        }
        steSetSystemError( errno, aErrorStack );
    }
    
#endif
    
    STL_FINISH;

    return STL_FAILURE;

}

/*
 * @brief pollset 객체를 삭제한다.
 * @param[in] aPollSet pollset 객체의 주소
 * @param[out] aErrorStack 에러 스택
 */
stlStatus stwDestroyPollSet( stlPollSet    * aPollSet,
                             stlErrorStack * aErrorStack )
{
    if( aPollSet->mQuerySet != NULL )
    {
        stlFreeHeap( (void*)aPollSet->mQuerySet );
        aPollSet->mQuerySet = NULL;
    }
    
    if( aPollSet->mResultSet != NULL )
    {
        stlFreeHeap( (void*)aPollSet->mResultSet );
        aPollSet->mResultSet = NULL;
    }

#if defined( STC_POLLSET_USES_EPOLL )
    
    if( aPollSet->mEpollEvent != NULL )
    {
        stlFreeHeap( (void*)aPollSet->mEpollEvent );
        aPollSet->mEpollEvent = NULL;
    }

    close( aPollSet->mEpollFd );
    
#endif

#if defined( STC_POLLSET_USES_POLL )
    
    if( aPollSet->mPollFds != NULL )
    {
        stlFreeHeap( (void*)aPollSet->mPollFds );
        aPollSet->mPollFds = NULL;
    }
    
#endif

    return STL_SUCCESS;
}

/*
 * @brief pollset에 이벤트를 검사할 소켓을 추가한다.
 * @param[in] aPollSet pollset 객체의 주소
 * @param[in] aPollFd 이벤트를 대기할 stlPollFd
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes :
 * @code
 * [STL_ERRCODE_OUT_OF_MEMORY]
 *     aPollSet에 aPollFd 를 추가할 메모리가 없음
 * [STL_ERRCODE_OVERFLOW_FD]
 *     입력받은 stlPollFd의 소켓 정보가 올바르지 않음
 *
 * @endcode
 * @remark Thread Safe하지 않음
 */
stlStatus stwAddSockPollSet( stlPollSet        * aPollSet,
                             const stlPollFdEx * aPollFd,
                             stlErrorStack     * aErrorStack )
{
#if defined( STC_POLLSET_USES_EPOLL )
    stlInt32           sIdx = 0;
#endif

    STL_PARAM_VALIDATE( aPollFd->mPollFd.mSocketHandle != -1, aErrorStack );

    STL_TRY_THROW( aPollSet->mMaxPollFdNum > aPollSet->mCurPollFdNum,
                   RAMP_ERR_OUT_OF_MEMORY);

#if defined( STC_POLLSET_USES_EPOLL )

    stlEpollEvent sEpollEvent = {0};
    stlInt32      sReturn = -1;

    for( sIdx = 0; sIdx < aPollSet->mMaxPollFdNum; sIdx++ )
    {
        if( aPollSet->mQuerySet[sIdx].mPollFd.mSocketHandle == -1 )
        {
            aPollSet->mQuerySet[sIdx] = *aPollFd;

            break;
        }
    }

    STL_DASSERT( sIdx < aPollSet->mMaxPollFdNum );

    /*
     * EPOLLIN == POLLIN, EPOLLOUT == POLLOUT임.
     */
    sEpollEvent.events = aPollFd->mPollFd.mReqEvents;
    sEpollEvent.data.ptr = (void*)&(aPollSet->mQuerySet[sIdx]);

    sReturn = epoll_ctl( aPollSet->mEpollFd,
                         EPOLL_CTL_ADD,
                         aPollFd->mPollFd.mSocketHandle,
                         &sEpollEvent );

    STL_TRY_THROW( sReturn == 0, RAMP_ERR_POLLSET_CTRL );

#else /* POLL | SELECT */

    aPollSet->mQuerySet[aPollSet->mCurPollFdNum] = *aPollFd;

#if defined( STC_POLLSET_USES_SELECT )

    STL_TRY_THROW( aPollFd->mPollFd.mSocketHandle < FD_SETSIZE, RAMP_ERR_FD_SETSIZE );

    if( aPollFd->mPollFd.mReqEvents & STL_POLLIN )
    {
        FD_SET( aPollFd->mPollFd.mSocketHandle, &(aPollSet->mReadSet) );
    }

    if( aPollFd->mPollFd.mReqEvents & STL_POLLOUT )
    {
        FD_SET( aPollFd->mPollFd.mSocketHandle, &(aPollSet->mWriteSet) );
    }

    if( aPollFd->mPollFd.mReqEvents & STL_POLLERR )
    {
        FD_SET( aPollFd->mPollFd.mSocketHandle, &(aPollSet->mExceptSet) );
    }

    if( aPollFd->mPollFd.mSocketHandle > aPollSet->mMaxFd )
    {
        aPollSet->mMaxFd = aPollFd->mPollFd.mSocketHandle;
    }

#else /* POLL */

    aPollSet->mPollFds[aPollSet->mCurPollFdNum] = aPollFd->mPollFd;
    
#endif

#endif
    
    aPollSet->mCurPollFdNum++;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_OUT_OF_MEMORY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_OUT_OF_MEMORY,
                      NULL,
                      aErrorStack );
    }

#if defined( STC_POLLSET_USES_EPOLL )

    STL_CATCH( RAMP_ERR_POLLSET_CTRL )
    {
        aPollSet->mQuerySet[sIdx].mPollFd.mSocketHandle = -1;

        switch( errno )
        {
            case ENOMEM :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_OUT_OF_MEMORY,
                              NULL,
                              aErrorStack );
                break;
            case EPERM:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_ACCESS_FILE,
                              NULL,
                              aErrorStack,
                              "pollset" );
                break;
            case EINVAL:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_INVALID_ARGUMENT,
                              NULL,
                              aErrorStack );
                break;
            case ENOENT:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_NO_ENTRY,
                              NULL,
                              aErrorStack,
                              "pollset" );
                break;
            default :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_POLLSET_CTRL,
                              NULL,
                              aErrorStack );
                break;
                
        }
        steSetSystemError( errno, aErrorStack );
    }
    
#endif
    
#if defined( STC_POLLSET_USES_SELECT )

    STL_CATCH( RAMP_ERR_FD_SETSIZE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_OVERFLOW_FD,
                      NULL,
                      aErrorStack );
    }

#endif

    STL_FINISH;

    return STL_FAILURE;    
}

/*
 * @brief pollset에 추가된 소켓을 제거한다.
 * @param[in] aPollSet pollset 객체의 주소
 * @param[in] aPollFd 제거할 stlPollFd
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes :
 * @code
 * [STL_ERRCODE_NO_ENTRY]
 *     PollSet에 삭제할 PollFd 가 존재하지 않음
 * @endcode
 * @remark Thread Safe하지 않음
 */
stlStatus stwRemoveSockPollSet( stlPollSet        * aPollSet,
                                const stlPollFdEx * aPollFd,
                                stlErrorStack     * aErrorStack )
{
    stlInt32    sIdx = 0;

#if defined( STC_POLLSET_USES_EPOLL )

    for( sIdx = 0; sIdx < aPollSet->mMaxPollFdNum; sIdx++ )
    {
        if( aPollSet->mQuerySet[sIdx].mPollFd.mSocketHandle == aPollFd->mPollFd.mSocketHandle )
        {
            aPollSet->mQuerySet[sIdx].mPollFd.mSocketHandle = -1;
            break;
        }
    }

    STL_TRY_THROW( sIdx < aPollSet->mMaxPollFdNum, RAMP_EXIT_FUNCTION );

    struct epoll_event sEpollEvent = {0};
    stlInt32           sReturn = -1;

    sReturn = epoll_ctl( aPollSet->mEpollFd,
                         EPOLL_CTL_DEL,
                         aPollFd->mPollFd.mSocketHandle,
                         &sEpollEvent );

    STL_TRY_THROW( sReturn == 0, RAMP_ERR_POLLSET_CTRL );

    aPollSet->mCurPollFdNum--;

#else /* POLL | SELECT */
    
    stlInt32    sDest;
    stlInt32    sOldCurPollFdNum;


    for( sIdx = 0; sIdx < aPollSet->mCurPollFdNum; sIdx++ )
    {
        if( aPollFd->mPollFd.mSocketHandle == aPollSet->mQuerySet[sIdx].mPollFd.mSocketHandle )
        {
            /* Found an instance of the fd: remove this and any other copies */
            sDest = sIdx;
            sOldCurPollFdNum = aPollSet->mCurPollFdNum;
            aPollSet->mCurPollFdNum--;

            for( sIdx++; sIdx < sOldCurPollFdNum; sIdx++)
            {
                if( aPollFd->mPollFd.mSocketHandle == aPollSet->mQuerySet[sIdx].mPollFd.mSocketHandle )
                {
                    aPollSet->mCurPollFdNum--;
                }
                else
                {
                    aPollSet->mQuerySet[sDest] = aPollSet->mQuerySet[sIdx];

#if defined( STC_POLLSET_USES_POLL )

                    aPollSet->mPollFds[sDest] = aPollSet->mPollFds[sIdx];
                    
#endif
            
                    sDest++;
                }                
            }

#if defined( STC_POLLSET_USES_SELECT )

            FD_CLR( aPollFd->mPollFd.mSocketHandle, &(aPollSet->mReadSet) );
            FD_CLR( aPollFd->mPollFd.mSocketHandle, &(aPollSet->mWriteSet) );
            FD_CLR( aPollFd->mPollFd.mSocketHandle, &(aPollSet->mExceptSet) );

            if( (aPollFd->mPollFd.mSocketHandle == aPollSet->mMaxFd) &&
                (aPollSet->mMaxFd) > 0 )
            {
                aPollSet->mMaxFd--;
            }
            
#endif
            
            STL_THROW( RAMP_EXIT_FUNCTION );
        }
    }

    STL_THROW( RAMP_ERR_NO_ENTRY );

#endif

    STL_RAMP( RAMP_EXIT_FUNCTION );

    return STL_SUCCESS;

#if defined( STC_POLLSET_USES_EPOLL )

    STL_CATCH( RAMP_ERR_POLLSET_CTRL )
    {
        switch( errno )
        {
            case ENOMEM :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_OUT_OF_MEMORY,
                              NULL,
                              aErrorStack );
                break;
            case EPERM:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_ACCESS_FILE,
                              NULL,
                              aErrorStack,
                              "pollset" );
                break;
            case EINVAL:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_INVALID_ARGUMENT,
                              NULL,
                              aErrorStack );
                break;
            case ENOENT:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_NO_ENTRY,
                              NULL,
                              aErrorStack,
                              "pollset" );
                break;
            default :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_POLLSET_CTRL,
                              NULL,
                              aErrorStack );
                break;
                
        }
        steSetSystemError( errno, aErrorStack );
    }

#else /* POLL | SELECT */
    
    STL_CATCH( RAMP_ERR_NO_ENTRY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_NO_ENTRY,
                      NULL,
                      aErrorStack,
                      "" );
    }

#endif
    
    STL_FINISH;

    return STL_FAILURE;    
}

/*
 * @brief pollset에 추가된 소켓의 이벤트 발생을 대기한다.
 * @param[in,out] aPollSet 이벤트를 대기할 pollset
 * @param[in] aTimeout 이벤트를 대기할 마이크로 초.
 *            대기 중 이벤트가 발생하면 @a aTimeout 이 다 되지 않아도 함수가 반환된다.
 *            만약 @a aTimeout 이 음수값이면 이벤트가 발생하기 전까지 무한대기 한다.
 * @param[out] aSigFdNum 이벤트가 발생한 소켓의 수
 * @param[out] aResultSet 이벤트가 발생한 소켓들
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes :
 * @code
 * [STL_ERRCODE_TIMEDOUT]
 *     주어진 timeout 동안 이벤트가 발생하지 않았다.
 * [STL_ERRCODE_OUT_OF_MEMORY]
 *     커널 메모리를 할당받을수 없는 경우에 발생함.
 * [STL_ERRCODE_POLL]
 *     poll()이 실패했음. 시스템 에러를 확인해야 한다.
 * @endcode
 * @remark 이벤트가 발생한 소켓의 수 @a aSigFdNum 는
 *         함수 호출 후 이벤트가 발생하거나 timeout이 발생해야 설정된다.
 * @remark stlPollFd 구조체의 mRetEvnets 는 STL_SUCCESS이 반환된 후 설정된다.
 */
stlStatus stwPollPollSet( stlPollSet     * aPollSet,
                          stlInterval      aTimeout,
                          stlInt32       * aSigFdNum,
                          stlPollFdEx   ** aResultSet,
                          stlErrorStack  * aErrorStack )
{
    stlInt32      i = 0;
    stlInt32      sSigFdNum = 0;
    
#if defined( STC_POLLSET_USES_SELECT ) || defined( STC_POLLSET_USES_POLL )
    stlInt32      j = 0;
    stlPollFd   * sPollFd;
#else
    stlPollFdEx * sPollFd;
#endif
    
#if defined( STC_POLLSET_USES_SELECT )
    struct timeval  sTv;
    struct timeval *sTvPtr;
    fd_set          sReadSet;
    fd_set          sWriteSet;
    fd_set          sExceptSet;
    stlInt32        sSocketHandle;

    if( aTimeout == 0 )
    {
        sTvPtr = NULL;
    }
    else
    {
        sTv.tv_sec  = (long)STL_GET_SEC_TIME(aTimeout);
        sTv.tv_usec = (long)STL_GET_USEC_TIME(aTimeout);
        sTvPtr = &sTv;
    }

    stlMemcpy( &sReadSet, &(aPollSet->mReadSet), STL_SIZEOF(fd_set) );
    stlMemcpy( &sWriteSet, &(aPollSet->mWriteSet), STL_SIZEOF(fd_set) );
    stlMemcpy( &sExceptSet, &(aPollSet->mExceptSet), STL_SIZEOF(fd_set) );

    do
    {
        sSigFdNum = select( aPollSet->mMaxFd + 1,
                            &sReadSet,
                            &sWriteSet,
                            &sExceptSet,
                            sTvPtr );
    } while( (sSigFdNum == -1) && (errno == EINTR) );

    if( sSigFdNum == 0 )
    {
        STL_THROW( RAMP_ERR_TIMEDOUT );
    }

    STL_TRY_THROW( sSigFdNum > 0, RAMP_ERR_SELECT );

    sSigFdNum = 0;

    for( i = 0; i < aPollSet->mCurPollFdNum; i++ )
    {
        sPollFd = &aPollSet->mQuerySet[i].mPollFd;
        sSocketHandle = sPollFd->mSocketHandle;

        if( FD_ISSET(sSocketHandle, &sReadSet)   ||
            FD_ISSET(sSocketHandle, &sWriteSet) ||
            FD_ISSET(sSocketHandle, &sExceptSet) )
        {
            aPollSet->mResultSet[j] = aPollSet->mQuerySet[i];
            aPollSet->mResultSet[j].mPollFd.mRetEvents = 0;
            
            if( FD_ISSET( sSocketHandle, &sReadSet ) )
            {
                aPollSet->mResultSet[j].mPollFd.mRetEvents |= STL_POLLIN;
            }
        
            if( FD_ISSET( sSocketHandle, &sWriteSet ) )
            {
                aPollSet->mResultSet[j].mPollFd.mRetEvents |= STL_POLLOUT;
            }
        
            if( FD_ISSET( sSocketHandle, &sExceptSet ) )
            {
                aPollSet->mResultSet[j].mPollFd.mRetEvents |= STL_POLLERR;
            }        

            if( aPollSet->mResultSet[j].mPollFd.mRetEvents != 0 )
            {
                sSigFdNum++;
            }

            j++;
        }
    }
    
#else  /* POLL | EPOLL */
    
    stlInt32      sTimeout;

    if( aTimeout == STL_INFINITE_TIME )
    {
        sTimeout = -1;
    }
    else
    {
        /* convert microseconds to milliseconds */
        aTimeout /= 1000;

        if( aTimeout > STL_INT32_MAX )
        {
            sTimeout = STL_INT32_MAX;
        }
        else
        {
            sTimeout = aTimeout;
        }
    }

#if defined( STC_POLLSET_USES_EPOLL )

    do
    {
        sSigFdNum = epoll_wait( aPollSet->mEpollFd,
                                aPollSet->mEpollEvent,
                                aPollSet->mCurPollFdNum,
                                sTimeout );
    } while( (sSigFdNum == -1) && (errno == EINTR) );

#else /* POLL */
    
    do
    {
        sSigFdNum = poll( (struct pollfd *)aPollSet->mPollFds,
                          aPollSet->mCurPollFdNum,
                          sTimeout );
    } while( (sSigFdNum == -1) && (errno == EINTR) );

#endif
    
    if( sSigFdNum == 0 )
    {
        STL_THROW( RAMP_ERR_TIMEDOUT );
    }

    STL_TRY_THROW( sSigFdNum > 0, RAMP_ERR_SELECT );

#if defined( STC_POLLSET_USES_EPOLL )
    
    for( i = 0; i < sSigFdNum; i++ )
    {
        sPollFd = (stlPollFdEx*)(aPollSet->mEpollEvent[i].data.ptr);

        aPollSet->mResultSet[i] = *sPollFd;
        aPollSet->mResultSet[i].mPollFd.mRetEvents = aPollSet->mEpollEvent[i].events;
    }

#else /* POLL */
    
    sSigFdNum = 0;

    for( i = 0; i < aPollSet->mCurPollFdNum; i++ )
    {
        sPollFd = &aPollSet->mPollFds[i];

        if( sPollFd->mRetEvents != 0 )
        {
            aPollSet->mResultSet[j] = aPollSet->mQuerySet[i];
            aPollSet->mResultSet[j].mPollFd = *sPollFd;
            sSigFdNum++;
            j++;
        }
    }

#endif

#endif

    if( aSigFdNum != NULL )
    {
        *aSigFdNum = sSigFdNum;
    }

    if( aResultSet != NULL )
    {
        *aResultSet = aPollSet->mResultSet;
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_TIMEDOUT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_TIMEDOUT,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_SELECT )
    {
        switch( errno )
        {
            case ENOMEM :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_OUT_OF_MEMORY,
                              NULL,
                              aErrorStack );
                break;
            default :
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_POLL,
                              NULL,
                              aErrorStack );
                break;
                
        }
        steSetSystemError( errno, aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}
