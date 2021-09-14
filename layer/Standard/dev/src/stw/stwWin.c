/*******************************************************************************
 * stwWin.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: stwUnix.c 13844 2014-11-04 03:52:09Z lym999 $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#define FD_SETSIZE 1024

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

    fd_set           sReadSet;
    fd_set           sWriteSet;
    fd_set           sExceptSet;
    stlInt32         sMaxFd = -1;
    struct timeval   sTv;
    struct timeval * sTvPtr;
    stlInt32         sSocketHandle;
    stlInt32         sIdx;

    if (aPollFdNum == 0)
    {
        *aSigFdNum = 0;

        if (aTimeout > 0)
        {
            stlSleep(aTimeout);

            STL_THROW(RAMP_ERR_TIMEDOUT);
        }

        STL_THROW(RAMP_SUCCESS);
    }

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

        if( aPollFd[sIdx].mReqEvents & STL_POLLIN )
        {
            FD_SET(sSocketHandle, &sReadSet);
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

    sSigFdNum = select( sMaxFd + 1, &sReadSet, &sWriteSet, &sExceptSet, sTvPtr );

    if( sSigFdNum == 0 )
    {
        STL_THROW( RAMP_ERR_TIMEDOUT );
    }

    STL_TRY_THROW( sSigFdNum > 0, RAMP_ERR_POLL );

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

    if( aSigFdNum != NULL )
    {
        *aSigFdNum = sSigFdNum;
    }

    STL_RAMP(RAMP_SUCCESS);

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
        stlPushError(STL_ERROR_LEVEL_ABORT,
                     STL_ERRCODE_POLL,
                     NULL,
                     aErrorStack);

        steSetSystemError(WSAGetLastError(), aErrorStack);
    }
    
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

    STL_TRY_THROW( aPollFdNum <= FD_SETSIZE, RAMP_ERR_FD_SETSIZE );

    FD_ZERO(&(aPollSet->mReadSet));
    FD_ZERO(&(aPollSet->mWriteSet));
    FD_ZERO(&(aPollSet->mExceptSet));

    aPollSet->mMaxFd = 0;

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
        aPollSet->mQuerySet[sIdx].mPollFd.mSocketHandle = INVALID_SOCKET;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_FD_SETSIZE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_OVERFLOW_FD,
                      NULL,
                      aErrorStack );
    }

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
    STL_PARAM_VALIDATE( aPollFd->mPollFd.mSocketHandle != INVALID_SOCKET, aErrorStack );

    STL_TRY_THROW( aPollSet->mMaxPollFdNum > aPollSet->mCurPollFdNum,
                   RAMP_ERR_OUT_OF_MEMORY);

    aPollSet->mQuerySet[aPollSet->mCurPollFdNum] = *aPollFd;

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
    
    aPollSet->mCurPollFdNum++;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_OUT_OF_MEMORY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_OUT_OF_MEMORY,
                      NULL,
                      aErrorStack );
    }

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

                    sDest++;
                }                
            }

            FD_CLR( aPollFd->mPollFd.mSocketHandle, &(aPollSet->mReadSet) );
            FD_CLR( aPollFd->mPollFd.mSocketHandle, &(aPollSet->mWriteSet) );
            FD_CLR( aPollFd->mPollFd.mSocketHandle, &(aPollSet->mExceptSet) );

            if( (aPollFd->mPollFd.mSocketHandle == aPollSet->mMaxFd) &&
                (aPollSet->mMaxFd) > 0 )
            {
                aPollSet->mMaxFd--;
            }
            
            STL_THROW( RAMP_EXIT_FUNCTION );
        }
    }

    STL_THROW( RAMP_ERR_NO_ENTRY );

    STL_RAMP( RAMP_EXIT_FUNCTION );

    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_NO_ENTRY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_NO_ENTRY,
                      NULL,
                      aErrorStack,
                      "" );
    }

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
    stlInt32      j = 0;
    stlPollFd   * sPollFd;
    
    struct timeval  sTv;
    struct timeval *sTvPtr;
    fd_set          sReadSet;
    fd_set          sWriteSet;
    fd_set          sExceptSet;
    stlInt32        sSocketHandle;

    if (aPollSet->mCurPollFdNum == 0)
    {
        *aSigFdNum = 0;

        if (aTimeout > 0)
        {
            stlSleep(aTimeout);

            STL_THROW(RAMP_ERR_TIMEDOUT);
        }

        STL_THROW(RAMP_SUCCESS);
    }

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

    sSigFdNum = select(aPollSet->mMaxFd + 1,
                       &sReadSet,
                       &sWriteSet,
                       &sExceptSet,
                       sTvPtr);
    
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
    
    if( aSigFdNum != NULL )
    {
        *aSigFdNum = sSigFdNum;
    }

    if( aResultSet != NULL )
    {
        *aResultSet = aPollSet->mResultSet;
    }
    
    STL_RAMP(RAMP_SUCCESS);

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
        stlPushError(STL_ERROR_LEVEL_ABORT,
                     STL_ERRCODE_POLL,
                     NULL,
                     aErrorStack);
        steSetSystemError(WSAGetLastError(), aErrorStack);
    }

    STL_FINISH;

    return STL_FAILURE;
}
