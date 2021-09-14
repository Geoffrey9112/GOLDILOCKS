/*******************************************************************************
 * stlNetworkIo.c
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

#include <stlDef.h>
#include <stn.h>
#include <stn.h>
#include <stlStrings.h>

/**
 * @file stlNetworkIo.c
 * @brief Standard Layer Network I/O Routines
 */


/**
 * @addtogroup stlNetworkIo
 * @{
 */


/**
 * @brief 주어진 Address Family와 Type, Protocol을 가지는 Socket을 생성한다.
 * @param[out] aNewSocket 생성된 소켓의 포인터
 * @param[in] aFamily Address Family
 * @param[in] aType Socket의 타입(OR 가능)
 * @param[in] aProtocol 프로토콜. 0이면 주어진 address family에 맞게 자동으로 프로토콜 설정
 * @param[out] aErrorStack 에러 스택
 * @see aFamily : stlAddressFamily
 * @see aType : stlSocketType
 * @see aProtocol : stlNetworkProtocol
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_SOCKET]
 *     socket()관련 대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * [STL_ERRCODE_GETFD]
 *     fcntl로 F_GETFD 도중 에러 발생 (자세한 에러는 System Error Code를 확인할것)
 * [STL_ERRCODE_SETFD]
 *     fcntl로 F_SETFD 도중 에러 발생 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
stlStatus stlCreateSocket( stlSocket     * aNewSocket, 
                           stlInt16         aFamily,
                           stlInt32         aType,
                           stlInt32         aProtocol,
                           stlErrorStack * aErrorStack )
{
    return stnCreateSocket( aNewSocket, 
                            aFamily,
                            aType,
                            aProtocol,
                            aErrorStack );
}

/**
 * @brief 주어진 Socket의 특정 연결을 끊는다.
 * @param[in] aSocket 소켓 descriptor
 * @param[in] aHow 연결을 끊을 방향
 * @param[out] aErrorStack 에러 스택
 * @see aHow : stlShutdownHow
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_SOCKET_SHUTDOWN]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
stlStatus stlShutdownSocket( stlSocket        aSocket, 
                             stlShutdownHow   aHow,
                             stlErrorStack  * aErrorStack )
{
    return stnShutdownSocket( aSocket, 
                              aHow,
                              aErrorStack );
}

/**
 * @brief 주어진 Socket의 연결을 완전히 끊는다.
 * @param[in] aSocket 소켓 descriptor
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_SOCKET_CLOSE]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
stlStatus stlCloseSocket( stlSocket       aSocket,
                          stlErrorStack * aErrorStack )
{
    return stnCloseSocket( aSocket,
                           aErrorStack );
}

/**
 * @brief 주어진 Socket을 주어진 주소와 포트에 연결시킨다.
 * @param[in] aSock 소켓 descriptor
 * @param[in] aSockAddr 연결시킬 주소와 포트 정보
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_FILE_BAD]
 *     유효하지않은 소켓 descriptor임
 * [STL_ERRCODE_ADDR_INUSE]
 *     해당 주소는 이미 사용중임
 * [STL_ERRCODE_INVALID_ARGUMENT]
 *     이미 Bind가 된 소켓임
 * [STL_ERRCODE_SOCKET_CLOSE]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
stlStatus stlBind( stlSocket       aSock,
                   stlSockAddr   * aSockAddr,
                   stlErrorStack * aErrorStack )
{
    return stnBind( aSock,
                    aSockAddr,
                    aErrorStack );
}

/**
 * @brief 주어진 Socket으로의 접속을 기다린다.
 * @param[in] aSock 소켓 descriptor
 * @param[in] aBacklog 동시에 대기할 수 있는 접속요청의 최대 개수
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_FILE_BAD]
 *     유효하지않은 소켓 descriptor임
 * [STL_ERRCODE_ADDR_INUSE]
 *     또다른 소켓이 이미 listen 중임
 * [STL_ERRCODE_SOCKET_LISTEN]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
stlStatus stlListen( stlSocket       aSock,
                     stlUInt32       aBacklog,
                     stlErrorStack * aErrorStack )
{
    return stnListen( aSock,
                      aBacklog,
                      aErrorStack );
}

/**
 * @brief 주어진 Socket으로의 접속요청을 받아서 새 소켓 desriptor를 만든다
 * @param[in] aSock 소켓 descriptor
 * @param[out] aNewSock 새로 접속된 소켓 descriptor
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_AGAIN]
 *     Non-blocking 소켓이며 현재는 accept할것이 없으므로 다시 시도하시오
 * [STL_ERRCODE_FILE_BAD]
 *     유효하지않은 소켓 descriptor임
 * [STL_ERRCODE_INVALID_ARGUMENT]
 *     listen 하고 있지 않은 소켓임
 * [STL_ERRCODE_SOCKET_ACCEPT]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * [STL_ERRCODE_SOCKET_EINTR]
 *     accept() 도중 시그날등에 의해 interrupt 발생
 * [STL_ERRCODE_GETFD]
 *     fcntl로 F_GETFD 도중 에러 발생 (자세한 에러는 System Error Code를 확인할것)
 * [STL_ERRCODE_SETFD]
 *     fcntl로 F_SETFD 도중 에러 발생 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
stlStatus stlAccept( stlSocket       aSock,
                     stlSocket     * aNewSock,
                     stlErrorStack * aErrorStack )
{
    return stnAccept( aSock,
                      aNewSock,
                      aErrorStack );
}

/**
 * @brief 주어진 Socket으로 주어진 주소와 포트로 non-blocking 접속요청을 한다.
 * @param[in] aSock 소켓 descriptor
 * @param[in] aSockAddr 접속할 주소와 포트등을 담은 구조체
 * @param[in] aTimeout connection timeout(usec)
 * @param[out] aErrorStack 에러 스택
 */
stlStatus stlConnect( stlSocket       aSock,
                      stlSockAddr   * aSockAddr,
                      stlInterval     aTimeout,
                      stlErrorStack * aErrorStack )
{
    return stnConnect( aSock,
                       aSockAddr,
                       aTimeout,
                       aErrorStack );
}

/**
 * @brief 주어진 Socket으로 message를 전송한다.
 * @param[in] aSock 소켓 descriptor
 * @param[in] aBuf 전송할 message가 담긴 버퍼
 * @param[in,out] aLen 전송할 message의 길이 및 실제로 전송한 message의 길이
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_AGAIN]
 *     Non-blocking 소켓이며 다시 시도하시오
 * [STL_ERRCODE_FILE_BAD]
 *     유효하지않은 소켓 descriptor임
 * [STL_ERRCODE_FILE_WRITE]
 *     write() 함수의 대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
stlStatus stlSend( stlSocket       aSock,
                   const stlChar * aBuf, 
                   stlSize       * aLen,
                   stlErrorStack * aErrorStack )
{
    return stnSend( aSock,
                    aBuf, 
                    aLen,
                    aErrorStack );
}

/**
 * @brief 주어진 Socket으로부터 message를 받는다. 
 * @param[in] aSock 소켓 descriptor
 * @param[out] aBuf 전송받은 message가 담길 버퍼
 * @param[in,out] aLen 전송받을 message의 최대길이 및 실제로 전송받은 message의 길이
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_AGAIN]
 *     Non-blocking 소켓이며 다시 시도하시오
 * [STL_ERRCODE_FILE_BAD]
 *     유효하지않은 소켓 descriptor임
 * [STL_ERRCODE_FILE_READ]
 *     read() 함수의 대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * [STL_ERRCODE_EOF]
 *     connection이 정상적으로 shutdown 되었음
 * @endcode
 */
stlStatus stlRecv( stlSocket       aSock,
                   stlChar       * aBuf,
                   stlSize       * aLen,
                   stlErrorStack * aErrorStack)
{
    return stnRecv( aSock,
                    aBuf,
                    aLen,
                    aErrorStack);
}

/**
 * @brief 주어진 주소와 Socket으로 Datagram(비연결) message를 전송한다.
 * @param[in] aSock 소켓 descriptor
 * @param[in] aWhere 메시지를 보낼 주소와 포트등을 담은 구조체
 * @param[in] aFlags sendto()에 전달할 소켓옵션(플랫폼에 따라 다를 수 있음)
 * <PRE>
 * 0 : Default
 * MSG_CONFIRM : Forward Progress가 일어났음을 link layer에 알린다
 * MSG_DONTROUTE : gateway를 통한 전송은 하지 않고 직접 연결된 host들로만 전송한다
 * MSG_DONTWAIT: Non-Blocking 연산을 가능하게 한다
 * MSG_MORE : TCP_CORK처럼 동작하기 위해 보낼 데이터가 더 있음을 알린다(전송지연)
 * </PRE>
 * @param[in] aBuf 전송할 message가 담긴 버퍼
 * @param[in,out] aLen 전송할 message의 길이 및 실제로 전송한 message의 길이
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_AGAIN]
 *     Non-blocking 소켓이며 다시 시도하시오
 * [STL_ERRCODE_FILE_BAD]
 *     유효하지않은 소켓 descriptor임
 * [STL_ERRCODE_SENDTO]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * [STL_ERRCODE_NOTSUPPORT]
 *     지원하지 않는 address family가 세팅된 경우
 * @endcode
 */
stlStatus stlSendTo( stlSocket       aSock,
                     stlSockAddr   * aWhere,
                     stlInt32        aFlags,
                     const stlChar * aBuf,
                     stlSize       * aLen,
                     stlErrorStack * aErrorStack)
{
    return stnSendTo( aSock,
                      aWhere,
                      aFlags,
                      aBuf,
                      aLen,
                      aErrorStack);
}

/**
 * @brief 주어진 Socket으로부터 Datagram(비연결) message를 받는다. 
 * @param[in] aSock 소켓 descriptor
 * @param[out] aFrom 메시지를 받은 주소
 * @param[in] aFlags recvfrom()에 전달할 소켓옵션(플랫폼에 따라 다를 수 있음)
 * <PRE>
 * 0 : Default
 * MSG_DONTWAIT: Non-Blocking 연산을 가능하게 한다
 * MSG_ERRQUEUE : 소켓 에러 큐로부터 쌓인 에러를 받도록 한다
 * MSG_PEEK : 데이터를 큐에서 삭제하지 않고 받을 수 있게 한다
 * MSG_WAITALL : 모든 요청사항이 완료될때까지 block되도록 한다
 * </PRE>
 * @param[out] aBuf 전송받은 message가 담길 버퍼
 * @param[in,out] aLen 전송받을 message의 최대길이 및 실제로 전송받은 message의 길이
 * @param[in] aIsSockStream 현재 소켓이 SOCK_STREAM으로 생성된것인지
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_AGAIN]
 *     Non-blocking 소켓이며 다시 시도하시오
 * [STL_ERRCODE_FILE_BAD]
 *     유효하지않은 소켓 descriptor임
 * [STL_ERRCODE_RECVFROM]
 *     read() 함수의 대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * [STL_ERRCODE_EOF]
 *     SOCK_STREAM 방식의 socket에서 전송받은 메시지의 길이가 0인 경우(EOF)
 *     connection이 정상적으로 shutdown 되었음
 * @endcode
 */
stlStatus stlRecvFrom( stlSocket       aSock,
                       stlSockAddr   * aFrom,
                       stlInt32        aFlags,
                       stlChar       * aBuf, 
                       stlSize       * aLen,
                       stlBool         aIsSockStream,
                       stlErrorStack * aErrorStack )
{
    return stnRecvFrom( aSock,
                        aFrom,
                        aFlags,
                        aBuf, 
                        aLen,
                        aIsSockStream,
                        aErrorStack );
}

/**
 * @brief 주어진 Socket을 통하여 message를 전송한다.
 * @param[in] aSock 소켓 descriptor
 * @param[in] aMsgHdr 보낼 메시지와 길이등을 담은 구조체
 * @param[in] aFlags sendmsg()에 전달할 소켓옵션(플랫폼에 따라 다를 수 있음)
 * <PRE>
 * 0 : Default
 * MSG_CONFIRM : Forward Progress가 일어났음을 link layer에 알린다
 * MSG_DONTROUTE : gateway를 통한 전송은 하지 않고 직접 연결된 host들로만 전송한다
 * MSG_DONTWAIT: Non-Blocking 연산을 가능하게 한다
 * MSG_MORE : TCP_CORK처럼 동작하기 위해 보낼 데이터가 더 있음을 알린다(전송지연)
 * MSG_EOR : record를 종료한다.(socket type이 SOCK_SEQPACKET으로 설정되어야 함)
 * MSG_NOSIGNAL : 상대편 socket이 종료되었을때 SIGPIPE을 발생시키지 않는다.(EPIPE 에러는 반환)
 * MSG_OOB : out-of-band data를 보낸다.
 * </PRE>
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_AGAIN]
 *     Non-blocking 소켓이며 다시 시도하시오
 * [STL_ERRCODE_FILE_BAD]
 *     유효하지않은 소켓 descriptor임
 * [STL_ERRCODE_SENDTO]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * [STL_ERRCODE_NOTSUPPORT]
 *     지원하지 않는 address family가 세팅된 경우
 * @endcode
 * @see sendmsg
 */
stlStatus stlSendMsg( stlSocket         aSock,
                      const stlMsgHdr  *aMsgHdr,
                      stlInt32          aFlags,
                      stlErrorStack    *aErrorStack )
{
    return stnSendMsg( aSock,
                       aMsgHdr,
                       aFlags,
                       aErrorStack);
}

/**
 * @brief 주어진 Socket으로부터 message를 받는다. 
 * @param[in] aSock 소켓 descriptor
 * @param[out] aMsgHdr 받은 메시지와 길이를 저장할 구조체
 * @param[in] aFlags recvmsg()에 전달할 소켓옵션(플랫폼에 따라 다를 수 있음)
 * <PRE>
 * 0 : Default
 * MSG_CMSG_CLOEXEC : unix domain을 socket에서 SCM_RIGHTS 연산을 통해 전달받은 file descriptor에 대하여 close-on-exec flag를 설정한다.
 * MSG_DONTWAIT: Non-Blocking 연산을 가능하게 한다
 * MSG_ERRQUEUE : 소켓 에러 큐로부터 쌓인 에러를 받도록 한다
 * MSG_OOB : out-of-band data를 받는다.
 * MSG_PEEK : 데이터를 큐에서 삭제하지 않고 받을 수 있게 한다
 * MSG_WAITALL : 모든 요청사항이 완료될때까지 block되도록 한다
 * </PRE>
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_AGAIN]
 *     Non-blocking 소켓이며 다시 시도하시오
 * [STL_ERRCODE_FILE_BAD]
 *     유효하지않은 소켓 descriptor임
 * [STL_ERRCODE_RECVFROM]
 *     read() 함수의 대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * [STL_ERRCODE_EOF]
 *     SOCK_STREAM 방식의 socket에서 전송받은 메시지의 길이가 0인 경우(EOF)
 *     connection이 정상적으로 shutdown 되었음
 * @endcode
 * @see recvmsg
 */
stlStatus stlRecvMsg( stlSocket         aSock,
                      stlMsgHdr        *aMsgHdr,
                      stlInt32          aFlags,
                      stlErrorStack    *aErrorStack )
{
    return stnRecvMsg( aSock,
                       aMsgHdr,
                       aFlags,
                       aErrorStack );
}

/**
 * @brief 연결된 socket의 unnamed pair를 생성한다.
 * @param[in] aDomain 생성할 socket의 domain
 * <PRE>
 * STL_AF_INET : IPv4 socket
 * STL_AF_INET6 : IPv6 socket
 * STL_AF_UNSPEC : 위의 2가지를 알아서 선택함.
 * STL_AF_UNIX : unix domain socket
 * </PRE>
 * @param[in] aType 생성할 socket의 type
 * <PRE>
 * STL_SOCK_STREAM : 연결 기반의 양방향 스트림 통신
 * STL_SOCK_DGRAM : 연결이 없고 손실가능한 데이터그램 통신
 * </PRE>
 * @param[in] aProtocol socket에서 사용될 특별한 protocol.
 * @param[out] aSockets 연결된 socket을 반환. aSockets[0], aSocket[1]
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_SOCKET]
 *     socket()관련 대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 * @see socketpair
 */
stlStatus stlSocketPair( stlInt32          aDomain,
                         stlInt32          aType,
                         stlInt32          aProtocol,
                         stlSocket         aSockets[2],
                         stlErrorStack    *aErrorStack )
{
    return stnSocketPair( aDomain,
                          aType,
                          aProtocol,
                          aSockets,
                          aErrorStack );
}

/**
 * @brief 주어진 Socket의 option을 세팅한다
 * @param[in] aSock 소켓 descriptor
 * @param[in] aOpt 변경시킬 option
 * <PRE>
 * STL_SOPT_SO_KEEPALIVE : 주기적으로 KEEPALIVE 메시지를 보내서 서로 연결을 확인하도록 한다
 * STL_SOPT_SO_DEBUG : 디버깅 정보를 출력하도록 한다
 * STL_SOPT_SO_REUSEADDR : 해당 address와 port에 해당하는 연결이 TIME_WAIT에 걸려있을때 바로 재사용하도록 한다
 * STL_SOPT_SO_SNDBUF : 송신 버퍼의 크기를 설정한다
 * STL_SOPT_SO_RCVBUF : 수신 버퍼의 크기를 설정한다
 * STL_SOPT_SO_NONBLOCK : 통신시의 BLOCKING 여부를 설정한다
 * STL_SOPT_SO_LINGER : 종료시 대기 타이머값을 세팅한다
 * STL_SOPT_TCP_DEFER_ACCEPT : TCP connect시에 실제 데이터가 날라올때까지 ESTABLISHED를 수행하지 않고 기다리게 한다
 * STL_SOPT_TCP_NODELAY : 서버측 부하를 줄이기 위해 사용하는 Nagle 알고리즘을 사용하지 않게 한다
 * STL_SOPT_TCP_NOPUSH : TCP_CORK와 유사. packet frame이 완료될때까지 보내지 않는다
 * STL_SOPT_IPV6_V6ONLY : IPv6 연결만 가능하도록 설정한다
 * </PRE>
 * @param[in] aOn ON(1)/OFF(0) Flag
 * @param[out] aErrorStack 에러 스택
 * @see aOpt : stlSocketOption
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_SETSOCKOPT]
 *     setsockopt() 함수의 대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * [STL_ERRCODE_NOTIMPL]
 *     현재 시스템에서 구현되지 않은 옵션임
 * [STL_ERRCODE_INVAL]
 *     정의되지 않은 option 값임
 * @endcode
 */
stlStatus stlSetSocketOption( stlSocket       aSock, 
                              stlInt32        aOpt,
                              stlInt32        aOn,
                              stlErrorStack * aErrorStack )
{
    return stnSetSocketOption( aSock, 
                               aOpt,
                               aOn,
                               aErrorStack );
}

/**
 * @brief 주어진 Socket의 option을 확인한다
 * @param[in] aSock 소켓 descriptor
 * @param[in] aOpt 확인할 option
 * <PRE>
 * STL_SOPT_SO_KEEPALIVE : 주기적으로 KEEPALIVE 메시지를 보내서 서로 연결을 확인하도록 한다
 * STL_SOPT_SO_DEBUG : 디버깅 정보를 출력하도록 한다
 * STL_SOPT_SO_REUSEADDR : 해당 address와 port에 해당하는 연결이 TIME_WAIT에 걸려있을때 바로 재사용하도록 한다
 * STL_SOPT_SO_SNDBUF : 송신 버퍼의 크기를 설정한다
 * STL_SOPT_SO_RCVBUF : 수신 버퍼의 크기를 설정한다
 * STL_SOPT_SO_NONBLOCK : 통신시의 BLOCKING 여부를 설정한다
 * STL_SOPT_SO_LINGER : 종료시 대기 타이머값을 세팅한다
 * STL_SOPT_TCP_DEFER_ACCEPT : TCP connect시에 실제 데이터가 날라올때까지 ESTABLISHED를 수행하지 않고 기다리게 한다
 * STL_SOPT_TCP_NODELAY : 서버측 부하를 줄이기 위해 사용하는 Nagle 알고리즘을 사용하지 않게 한다
 * STL_SOPT_TCP_NOPUSH : TCP_CORK와 유사. packet frame이 완료될때까지 보내지 않는다
 * STL_SOPT_IPV6_V6ONLY : IPv6 연결만 가능하도록 설정한다
 * </PRE>
 * @param[out] aRet ON(1)/OFF(0) 혹은 해당 옵션에 세팅된 정수값
 * @param[out] aErrorStack 에러 스택
 * @see aOpt : stlSocketOption
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_GETSOCKOPT]
 *     setsockopt() 함수의 대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * [STL_ERRCODE_NOTIMPL]
 *     현재 시스템에서 구현되지 않은 옵션임
 * [STL_ERRCODE_INVAL]
 *     정의되지 않은 option 값임
 * @endcode
 */
stlStatus stlGetSocketOption( stlSocket       aSock, 
                              stlInt32        aOpt,
                              stlSize       * aRet,
                              stlErrorStack * aErrorStack )
{
    return stnGetSocketOption( aSock, 
                               aOpt,
                               aRet,
                               aErrorStack );
}

/**
 * @brief 현재 서버의 hostname을 구해온다. 
 * @param[out] aBuf hostname을 저장할 버퍼
 * @param[in,out] aLen 저장할 수 있는 hostname의 최대길이
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_GETHOSTNAME]
 *     gethostname()의 대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * [STL_ERRCODE_ENAMETOOLONG]
 *     hostname이 주어진 버퍼보다 커서 truncate가 발생했음
 * @endcode
 */
stlStatus stlGetHostName( stlChar       * aBuf,
                          stlSize         aLen,
                          stlErrorStack * aErrorStack )
{
    return stnGetHostName( aBuf, aLen, aErrorStack );
}

/**
 * @brief 주어진 socket이 bind된 address를 반환한다. 
 * @param[in] aSock 소켓 descriptor
 * @param[out] aSockAddr 반환될 주소값
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_GETSOCKNAME]
 *     getsockname()의 대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
stlStatus stlGetSockName( stlSocket        aSock,
                          stlSockAddr    * aSockAddr,
                          stlErrorStack  * aErrorStack)
{
    return stnGetSockName( aSock,
                           aSockAddr,
                           aErrorStack);
}
 
/**
 * @brief 주어진 socket이 연결된 peer의 address를 반환한다. 
 * @param[in] aSock 소켓 descriptor
 * @param[out] aSockAddr 반환될 주소값
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_GETPEERNAME]
 *     getpeername()의 대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
stlStatus stlGetPeerName( stlSocket        aSock,
                          stlSockAddr    * aSockAddr,
                          stlErrorStack  * aErrorStack)
{
    return stnGetPeerName( aSock,
                           aSockAddr,
                           aErrorStack);
}

/**
 * @brief 주어진 address를 문자열로 표한하여 반환한다. 
 * @param[in] aSockAddr 문자열로 변환할 주소 정보
 * @param[out] aAddrBuf 주소의 문자열을 저장할 버퍼
 * @param[in] aAddrBufLen 주소 무자열을 저장할 버퍼의 최대 길이
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_NOTSUPPORT]
 *     지원하지 않는 address family임
 * @endcode
 */
stlStatus stlGetSockAddrIp( stlSockAddr   * aSockAddr,
                            stlChar       * aAddrBuf,
                            stlSize         aAddrBufLen,
                            stlErrorStack * aErrorStack )
{
    return stnGetSockAddrIp( aSockAddr,
                             aAddrBuf,
                             aAddrBufLen,
                             aErrorStack );
}

/**
 * @brief 주어진 Binary 형태의 address를 문자열로 표현하여 반환한다. 
 * @param[in] aAf 주소의 address family
 * @param[in] aSrc Binary 형태의 주소를 저장하고 있는 포인터
 * @param[out] aDst 변환된 문자열 형태의 주소를 저장할 버퍼
 * @param[in] aSize 주소 문자열을 저장할 버퍼의 최대 길이
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_NOTSUPPORT]
 *     지원하지 않는 address family임
 * @endcode
 */
stlStatus stlGetStrAddrFromBin( stlInt32        aAf,
                                const void    * aSrc,
                                stlChar       * aDst,
                                stlSize         aSize,
                                stlErrorStack * aErrorStack )
{
    return stnGetStrAddrFromBin( aAf,
                                 aSrc,
                                 aDst,
                                 aSize,
                                 aErrorStack );

}

/**
 * @brief address 문자열을 binary address로 표현하여 반환한다. 
 * @param[in] aAf 주소의 address family
 * @param[in] aSrc 주소의 문자열
 * @param[out] aDst Binary 형태로 변환된 주소를 저장할 공간
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_NOTSUPPORT]
 *     지원하지 않는 address family임
 * @endcode
 */
stlStatus stlGetBinAddrFromStr( stlInt32        aAf,
                                const stlChar * aSrc,
                                void          * aDst,
                                stlErrorStack * aErrorStack)
{
    return stnGetBinAddrFromStr( aAf,
                                 aSrc,
                                 aDst,
                                 aErrorStack );
}

/**
 * @brief 주어진 stlSockAddr에 값들을 세팅한다
 * @param[out] aSockAddr 값들을 채울 stlSockAddr 포인터
 * @param[in] aFamily address family
 * @param[in] aAddrStr 주소 문자열
 * @param[in] aPort host byte order로 표현된 포트 번호
 * @param[in] aFilePath Unix domain 소켓에 사용할 file의 path
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_NOSUPPORT]
 *     지원되지 않는 Address Family임
 * @endcode
 */
stlStatus stlSetSockAddr( stlSockAddr   * aSockAddr,
                          stlInt16        aFamily,
                          stlChar       * aAddrStr,
                          stlPort         aPort,
                          stlChar       * aFilePath,
                          stlErrorStack * aErrorStack)
{
    return stnSetSockAddr( aSockAddr,
                           aFamily,
                           aAddrStr,
                           aPort,
                           aFilePath,
                           aErrorStack);
}

/**
 * @brief 주어진 stlSockAddr으로부터 값들을 추출한다
 * @param[in] aSockAddr 소켓 주소에 관한 정보들을 갖고있는 stlSockAddr 포인터
 * @param[out] aAddrBuf 문자열로 표시된 주소값을 저장할 버퍼공간
 * @param[in] aAddrBufLen @a aAddrBufLen 의 길이
 * @param[out] aPort host byte order로 표현된 포트 번호
 * @param[out] aFilePathBuf Unix domain 소켓에 사용된 file의 path를 담을 버퍼
 * @param[in] aFilePathBufLen file의 path를 담을 버퍼의 크기
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_NOSUPPORT]
 *     지원되지 않는 Address Family임
 * @endcode
 */
stlStatus stlGetSockAddr( stlSockAddr   * aSockAddr,
                          stlChar       * aAddrBuf,
                          stlSize         aAddrBufLen,
                          stlPort       * aPort,
                          stlChar       * aFilePathBuf,
                          stlInt32        aFilePathBufLen,
                          stlErrorStack * aErrorStack)
{
    return stnGetSockAddr( aSockAddr,
                           aAddrBuf,
                           aAddrBufLen,
                           aPort,
                           aFilePathBuf,
                           aFilePathBufLen,
                           aErrorStack);
}


/** @} */
