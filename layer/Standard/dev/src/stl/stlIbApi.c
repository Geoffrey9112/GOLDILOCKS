/*******************************************************************************
 * stlIbApi.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id:$
 *
 * NOTES
 *  ib 관련 주의 사항
 *      -std=c99 옵션이 포함되면 rdma_bind_addr에서 pd가 0으로 나옴.  
 *      => gcc 4.4.6에서 gcc 4.7.3로 변경하니 해결됨. 
 *      => gcc 4.4.6은 rdma/rdma_cma.h 에서도 문제 발생함. (unnamed union)
 *
 * 빌드 및 실행 환경 ( 2014/02/28현재 )
 *   빌드 환경 : centos6.5, gcc4.7.3, OFED.3.5-2.1,      rdmacm-1.0.17.1-1( https://www.openfabrics.org 에서 download )
 *   실행 환경 : RedHat6.3, gcc4.4.6, OFED.1.5.3.4.0.42, rdmacm-1.0.15-1  ( http://www.mellanox.com 에서 download )
 *
 *  TODO : 추가 기능
 *  - rc apm(Automatic Path Migration)(like ip bonding) 
 *  - UD(like UDP) protocol
 *  - 현재 한번에 send한 data보다 작게 받을수 없음. 가능하도록 하려면 추가 작업 필요함.
 *  - poll 에서 block시 즉시 종료 기능(poll에서 event fd도 추가로 확인하도록 하는 방법)
 *  - zero copy 기능
 *  - rdma_cm, async event 처리 기능
 *
 ******************************************************************************/

#include <stlDef.h>
#include <stnIbImpl.h>


/**
 * @brief infiniband rc protocol로 접속을 기다린다
 * @param[out] aHandle          접속 수신을 수행할 Handle 포인터
 * @param[in]  aBindAddr        접속을 기다릴 bind address(ip v4 format). (ib의 ip address)
 * @param[in]  aBindPort        접속을 기다릴 port.
 * @param[in]  aSendBufferSize  send buffer의 조각 크기
 * @param[in]  aSendBufferCount send buffer의 조각 개수
 * @param[in]  aRecvBufferSize  recv buffer의 조각 크기
 * @param[in]  aRecvBufferCount recv buffer의 조각 개수
 * @param[in]  aTimeoutUsec     Timeout ( micro second )
 * @param[out] aErrorStack      에러 스택
 * @par Error Codes :
 * @code
 * [STL_ERRCODE_IBV_VERBS_FAILURE]
 *     infiniband 에러 (자세한 에러는 System Error Code를 확인할것)
 * [STL_ERRCODE_TIMEDOUT]
 *     aTimeoutUsec 동안 Client의 접속 요청이 없었다.
 * [STL_ERRCODE_MISMATCH]
 *     aSendBufferSize, aRecvBufferSize가 상대방과 맞지 않는다.
 * @endcode
 * @remark buffer총 크기는 (aSendBufferSize * aSendBufferCount) + (aRecvBufferSize * aRecvBufferCount)이다
 */
stlStatus stlRcCreateForServer( stlRcHandle   * aHandle,
                                stlChar       * aBindAddr,
                                stlUInt16       aBindPort, 
                                stlUInt32       aSendBufferSize,
                                stlUInt32       aSendBufferCount,
                                stlUInt32       aRecvBufferSize,
                                stlUInt32       aRecvBufferCount,
                                stlInterval     aTimeoutUsec,
                                stlErrorStack * aErrorStack ) 
{
#ifdef SUPPORT_INFINIBAND
    STL_TRY( stnRcCreateForServer( aHandle,
                                   aBindAddr,
                                   aBindPort,
                                   aSendBufferSize,
                                   aSendBufferCount,
                                   aRecvBufferSize,
                                   aRecvBufferCount,
                                   aTimeoutUsec,
                                   aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
#else
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  STL_ERRCODE_NOTSUPPORT ,
                  "Infiniband Interfaces are not supported.",
                  aErrorStack );

    return STL_FAILURE;
#endif
} 

/**
 * @brief infiniband rc protocol로 접속을 수행한다.
 * @param[out] aHandle          접속을 수행할 Handle 포인터
 * @param[in]  aBindAddr        접속을 수행할 bind address(ip v4 format) (ib의 ip address)
 * @param[in]  aTargetAddr      접속할 target address(ip v4 format)      (peer ib의 ip address)
 * @param[in]  aTargetPort      접속할 target port
 * @param[in]  aSendBufferSize  send buffer의 조각 크기
 * @param[in]  aSendBufferCount send buffer의 조각 개수
 * @param[in]  aRecvBufferSize  recv buffer의 조각 크기
 * @param[in]  aRecvBufferCount recv buffer의 조각 개수
 * @param[out] aErrorStack      에러 스택
 * @par Error Codes :
 * @code
 * [STL_ERRCODE_IBV_VERBS_FAILURE]
 *     infiniband 에러 (자세한 에러는 System Error Code를 확인할것)
 * [STL_ERRCODE_CONNECT]
 *     Connect실패 (자세한 에러는 ib event 내용을 확인할것)
 * [STL_ERRCODE_MISMATCH]
 *     aSendBufferSize, aRecvBufferSize가 상대방과 맞지 않는다.
 * @endcode
 * @remark buffer총 크기는 (aSendBufferSize * aSendBufferCount) + (aRecvBufferSize * aRecvBufferCount)이다
 */
stlStatus stlRcCreateForClient( stlRcHandle   * aHandle,
                                stlChar       * aBindAddr,
                                stlChar       * aTargetAddr,
                                stlUInt16       aTargetPort, 
                                stlUInt32       aSendBufferSize,
                                stlUInt32       aSendBufferCount,
                                stlUInt32       aRecvBufferSize,
                                stlUInt32       aRecvBufferCount,
                                stlErrorStack * aErrorStack ) 
{
#ifdef SUPPORT_INFINIBAND
    STL_TRY( stnRcCreateForClient( aHandle,
                                   aBindAddr,
                                   aTargetAddr,
                                   aTargetPort,
                                   aSendBufferSize,
                                   aSendBufferCount,
                                   aRecvBufferSize,
                                   aRecvBufferCount,
                                   aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
#else
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  STL_ERRCODE_NOTSUPPORT ,
                  "Infiniband Interfaces are not supported.",
                  aErrorStack );

    return STL_FAILURE;
#endif
} 

/**
 * @brief infiniband rc protocol로 data를 송신하는 방법을 설정한다.
 *        송신할 data가 있는 경우 마지막 data송신후 aTimeoutUsec 동안 송신을 시도하는데
 *        aBusyWaitUsec 동안 busywait으로 시도하고 나머지 시간은 event로 송신대기한다.
 * @param[in]  aHandle          설정할 Handle 포인터
 * @param[in]  aTimeoutUsec     송신 timeout 시간
 * @param[in]  aBusyWaitUsec    busywait하는 시간
 * @param[out] aErrorStack      에러 스택
 * @remark aBusyWaitUsec가 aTimeoutUsec 보다 크면 aBusyWaitUsec는 aTimeoutUsec 로 설정된다.
 */
stlStatus stlRcSetSendTimeout( stlRcHandle     aHandle,
                               stlInterval     aTimeoutUsec,
                               stlInterval     aBusyWaitUsec,
                               stlErrorStack * aErrorStack ) 
{
#ifdef SUPPORT_INFINIBAND
    STL_TRY( stnRcSetSendTimeout( aHandle,
                                  aTimeoutUsec,
                                  aBusyWaitUsec,
                                  aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
#else
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  STL_ERRCODE_NOTSUPPORT ,
                  "Infiniband Interfaces are not supported.",
                  aErrorStack );

    return STL_FAILURE;
#endif
}

/**
 * @brief infiniband rc protocol로 data를 수신하는 방법을 설정한다.
 *        수신할 data가 있는 경우 마지막 data수신후 aTimeoutUsec 동안 수신을 시도하는데
 *        aBusyWaitUsec 동안 busywait으로 대기하고 나머지 시간은 event로 대기한다.
 * @param[in]  aHandle          설정할 Handle 포인터
 * @param[in]  aTimeoutUsec     수신 timeout 시간
 * @param[in]  aBusyWaitUsec    busywait하는 시간
 * @param[out] aErrorStack      에러 스택
 * @remark aBusyWaitUsec가 aTimeoutUsec 보다 크면 aBusyWaitUsec는 aTimeoutUsec 로 설정된다.
 */
stlStatus stlRcSetRecvTimeout( stlRcHandle     aHandle,
                               stlInterval     aTimeoutUsec,
                               stlInterval     aBusyWaitUsec,
                               stlErrorStack * aErrorStack ) 
{
#ifdef SUPPORT_INFINIBAND
    STL_TRY( stnRcSetRecvTimeout( aHandle,
                                  aTimeoutUsec,
                                  aBusyWaitUsec,
                                  aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
#else
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  STL_ERRCODE_NOTSUPPORT ,
                  "Infiniband Interfaces are not supported.",
                  aErrorStack );

    return STL_FAILURE;
#endif
}

/**
 * @brief infiniband rc protocol로 data를 송신한다.
 *        data를 모두 전송하지 못하면 aTimeoutUsec 동안 전송 시도한다.
 *        전송 시도는 aBusyWaitUsec 동안 busywait로 동작한다.
 *        (aTimeoutUsec, aBusyWaitUsec 는 stlRcSetSendMethod로 설정한 값이다.)
 * @param[in]       aHandle          송신을 수행할 Handle 포인터
 * @param[in]       aBuf             전송할 message가 담긴 버퍼 data의 시작 pointer
 * @param[in,out]   aLen             전송할 message의 길이 및 실제로 전송한 message의 길이
 * @param[out]      aErrorStack      에러 스택
 * @par Error Codes :
 * @code
 * [STL_ERRCODE_IBV_VERBS_FAILURE]
 *     infiniband 에러 (자세한 에러는 System Error Code를 확인할것)
 * [STL_ERRCODE_TIMEDOUT]
 *     stlRcSetSendMethod함수에서 설정한 aTimeoutUsec 동안 data를 못보냄.
 * @endcode
 * @remark 내부에서 timeout이 되어도 보낸 data가 있으면 STL_SUCCESS를 리턴한다.
 * @remark thread safe하지 않음
 */
stlStatus stlRcSend( stlRcHandle   * aHandle,
                     const stlChar * aBuf,
                     stlSize       * aLen,
                     stlErrorStack * aErrorStack ) 
{
#ifdef SUPPORT_INFINIBAND
    STL_TRY( stnRcSend( aHandle,
                        aBuf,
                        aLen,
                        aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
#else
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  STL_ERRCODE_NOTSUPPORT ,
                  "Infiniband Interfaces are not supported.",
                  aErrorStack );

    return STL_FAILURE;
#endif
} 

/**
 * @brief infiniband rc protocol로 data를 수신한다.
 *        data를 모두 수신하지 못하면 aTimeoutUsec 동안 수신 시도한다.
 *        수신 시도는 aBusyWaitUsec 동안 busywait로 동작한다.
 *        (aTimeoutUsec, aBusyWaitUsec 는 stlRcSetRecvMethod로 설정한 값이다.)
 * @param[in]     aHandle          수신을 수행할 Handle 포인터
 * @param[in]     aBuf             전송받은 message가 담길 버퍼
 * @param[in,out] aLen             전송받을 message의 최대길이 및 실제로 전송받은 message의 길이
 * @param[out]    aErrorStack      에러 스택
 * @par Error Codes :
 * @code
 * [STL_ERRCODE_IBV_VERBS_FAILURE]
 *     infiniband 에러 (자세한 에러는 System Error Code를 확인할것)
 * [STL_ERRCODE_TIMEDOUT]
 *     stlRcSetRecvMethod함수에서 설정한 aTimeoutUsec 동안 data를 못받음.
 * @endcode
 * @remark 내부에서 timeout이 되어도 받은 data가 있으면 STL_SUCCESS를 리턴한다.
 * @remark ib rc는 stream이지만 Sender에서 4096byte를 보냈는데 1024만 받을수 없다.
           그럼으로 보낸 data size와 받을 data size가 맞아야 한다.
 * @remark thread safe하지 않음
 */
stlStatus stlRcRecv( stlRcHandle     aHandle,
                     stlChar       * aBuf,
                     stlSize       * aLen,
                     stlErrorStack * aErrorStack ) 
{
#ifdef SUPPORT_INFINIBAND
    STL_TRY( stnRcRecv( aHandle,
                        aBuf,
                        aLen,
                        aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
#else
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  STL_ERRCODE_NOTSUPPORT ,
                  "Infiniband Interfaces are not supported.",
                  aErrorStack );

    return STL_FAILURE;
#endif
}


/**
 * @brief infiniband rc protocol로 연결을 종료한다.
 * @param[in]  aHandle          종료할 Handle 포인터
 * @param[out] aErrorStack      에러 스택
 * @par Error Codes :
 * @code
 * [STL_ERRCODE_IBV_VERBS_FAILURE]
 *     infiniband 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
stlStatus stlRcClose( stlRcHandle   * aHandle,
                      stlErrorStack * aErrorStack ) 
{
#ifdef SUPPORT_INFINIBAND
    STL_TRY( stnRcClose( aHandle,
                         aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
#else
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  STL_ERRCODE_NOTSUPPORT ,
                  "Infiniband Interfaces are not supported.",
                  aErrorStack );

    return STL_FAILURE;
#endif
} 


