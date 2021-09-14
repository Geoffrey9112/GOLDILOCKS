/*******************************************************************************
 * stlPoll.c
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
 * @file stlPoll.c
 * @brief Standard Layer Poll Routines
 */

#include <stc.h>
#include <stlDef.h>
#include <stlPoll.h>
#include <stw.h>
#include <stlError.h>


/**
 * @addtogroup stlPoll
 * @{
 */

/**
 * @brief 특정 시간동안 주어진 소켓들에서 이벤트가 발생을 대기한다.
 * @param[in,out] aPollFd 이벤트를 대기할 소켓 배열
 * @param[in] aPollFdNum 이벤트를 대기할 소켓의 수
 * @param[out] aSigFdNum 이벤트가 발생한 소켓의 수
 * @param[in] aTimeout 이벤트를 대기할 마이크로 초.
 *            대기 중 이벤트가 발생하면 @a aTimeout 이 다 되지 않아도 함수가 반환된다.
 *            만약 @a aTimeout 이 음수값이면 이벤트가 발생하기 전까지 무한대기 한다.
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes :
 * @code
 * [STL_ERRCODE_TIMEDOUT]
 *     주어진 timeout 동안 이벤트가 발생하지 않았다.
 * [STL_ERRCODE_OUT_OF_MEMORY]
 *     커널 메모리를 할당받을수 없는 경우에 발생함.
 * [STL_ERRCODE_POLL]
 *     poll()이 실패했음. 시스템 에러를 확인해야 한다.
 * [STL_ERRCODE_OVERFLOW_FD]
 *     입력받은 stlPollFd의 fd가 처리할수 있는 범위가 아닌 경우 발생함.
 * @endcode
 * @remark 이벤트가 발생한 소켓의 수 @a aSigFdNum 는
 *         함수 호출 후 이벤트가 발생하거나 timeout이 발생해야 설정된다.
 * @remark stlPollFd 구조체의 mRetEvnets 는 STL_SUCCESS이 반환된 후 설정된다.
 */
stlStatus stlPoll( stlPollFd     * aPollFd,
                   stlInt32        aPollFdNum,
                   stlInt32      * aSigFdNum,
                   stlInterval     aTimeout,
                   stlErrorStack * aErrorStack )
{
    return stwPoll( aPollFd, aPollFdNum, aSigFdNum, aTimeout, aErrorStack);
}

/**
 * @brief pollset 객체를 생성하고 초기화한다.
 * @param[in,out] aPollSet pollset 객체의 주소
 * @param[in] aPollFdNum @a aPollFd 배열을 구성하는 stlPollFd의 수
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_INVALID_FD]
 *     입력받은 stlPollFd의 수가 너무 크다.
 * @endcode
 */
stlStatus stlCreatePollSet( stlPollSet    * aPollSet,
                            stlInt32        aPollFdNum,
                            stlErrorStack * aErrorStack )
{
    return stwCreatePollSet(aPollSet, aPollFdNum, aErrorStack);
}

/**
 * @brief pollset 객체를 삭제한다.
 * @param[in] aPollSet pollset 객체의 주소
 * @param[out] aErrorStack 에러 스택
 */
stlStatus stlDestroyPollSet( stlPollSet    * aPollSet,
                             stlErrorStack * aErrorStack)
{
    return stwDestroyPollSet(aPollSet, aErrorStack);
}

/**
 * @brief pollset에 이벤트를 검사할 소켓을 추가한다.
 * @param[in] aPollSet pollset 객체의 주소
 * @param[in] aPollFd 이벤트를 대기할 stlPollFd
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes :
 * @code
 * [STL_ERRCODE_OUT_OF_MEMORY]
 *     aPollSet에 aPollFd 를 추가할 메모리가 없음
 * [STL_ERRCODE_INVALID_FD]
 *     입력받은 stlPollFd의 소켓 정보가 올바르지 않음
 * @endcode
 * @remark Thread Safe하지 않음
 */
stlStatus stlAddSockPollSet( stlPollSet        * aPollSet,
                             const stlPollFdEx * aPollFd,
                             stlErrorStack     * aErrorStack )
{
    return stwAddSockPollSet(aPollSet, aPollFd, aErrorStack);
}

/**
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
stlStatus stlRemoveSockPollSet( stlPollSet        * aPollSet,
                                const stlPollFdEx * aPollFd,
                                stlErrorStack     * aErrorStack )
{
    return stwRemoveSockPollSet( aPollSet, aPollFd, aErrorStack );
}

/**
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
stlStatus stlPollPollSet( stlPollSet     * aPollSet,
                          stlInterval      aTimeout,
                          stlInt32       * aSigFdNum,
                          stlPollFdEx   ** aResultSet,
                          stlErrorStack  * aErrorStack )
{
    return stwPollPollSet( aPollSet, aTimeout, aSigFdNum, aResultSet, aErrorStack );
}

/**
 * @brief pollset에 추가된 소켓의 이벤트 대기 중 이벤트 발생 시 callback 함수를 호출한다.
 * @param[in,out] aPollSet 이벤트를 대기할 pollset
 * @param[in] aTimeout 이벤트를 대기할 마이크로 초.
 *            대기 중 이벤트가 발생하면 @a aTimeout 이 다 되지 않아도 함수가 반환된다.
 *            만약 @a aTimeout 이 음수값이면 이벤트가 발생하기 전까지 무한대기 한다.
 * @param[in] aCallback 이벤트 발생 시 호출되는 callback 함수
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes :
 * @code
 * [STL_ERRCODE_TIMEDOUT]
 *     주어진 timeout 동안 이벤트가 발생하지 않았다.
 * [STL_ERRCODE_OUT_OF_MEMORY]
 *     커널 메모리를 할당받을수 없는 경우에 발생함.
 * [STL_ERRCODE_POLL]
 *     poll()이 실패했음. 시스템 에러를 확인해야 한다.
 * [STL_ERRCODE_INCOMPLETE]
 *     stlDispatchPollSet() 수행 중 입력 받은 callback 함수에 에러가 발생함.
 * @endcode
 */
stlStatus stlDispatchPollSet( stlPollSet       * aPollSet,
                              stlInterval        aTimeout,
                              stlPollSetCallback aCallback,
                              stlErrorStack    * aErrorStack )
{
    stlInt32      sSigFdNum;
    stlInt32      sIdx;
    stlPollFdEx * sResultSet;

    STL_TRY( stwPollPollSet( aPollSet,
                             aTimeout,
                             &sSigFdNum,
                             &sResultSet,
                             aErrorStack ) == STL_SUCCESS );

    for( sIdx = 0; sIdx < sSigFdNum; sIdx++ )
    {
        if( sResultSet[sIdx].mPollFd.mRetEvents != 0 )
        {
            STL_TRY_THROW( (*aCallback)(&(sResultSet[sIdx].mPollFd),
                                        aErrorStack) == STL_SUCCESS,
                           RAMP_ERR_CALLBACK);
        }
    }
             
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CALLBACK )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INCOMPLETE,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;    
}

/** @} */
