/*******************************************************************************
 * stlCommMgr.c
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
#include <stm.h>

/**
 * @file stlCommMgr.c
 * @brief Standard Layer Communication Manager Routines
 */


/**
 * @addtogroup stlCommMgr
 * @{
 */

/**
 * @brief 통신을 위한 Context를 초기화한다. 초기화 될 Context는 이미 할당되어<BR>
 * 있어야 하며, 여기서는 Context를 초기화하는 작업만 수행한다.
 * @param[in] aContext 초기화를 수행할 Context의 포인터
 * @param[in] aFamily Address Family.
 * <PRE>
 * STL_AF_INET : IPv4 socket
 * STL_AF_INET6 : IPv6 socket
 * STL_AF_UNSPEC : 위의 2가지를 알아서 선택함.
 * STL_AF_UNIX : unix domain socket
 * </PRE>
 * @param[in] aType Socket의 타입(OR 가능).
 * <PRE>
 * STL_SOCK_STREAM : 연결 기반의 양방향 스트림 통신
 * STL_SOCK_DGRAM : 연결이 없고 손실가능한 데이터그램 통신
 * </PRE>
 * @param[in] aProtocol 프로토콜. 0이면 주어진 address family에 맞게 자동으로 프로토콜 설정. 0/STL_PROTO_TCP/STL_PROTO_UDP/STL_PROTO_SCTP
 * @param[in] aIsListen 접속 요청을 대기하는 용도라면 STL_TRUE, 접속을 하는 Context라면 STL_FALSE로 설정
 * @param[in] aAddr 연결을 위한 주소. TCP/IP의 경우에는 IP 주소, Unix domain socket의 경우에는 filepath를 준다.
 * @param[in] aPort 연결을 위한 Port. TCP/IP의 경우에만 의미가 있으며, Unix domain socket에서는 무시된다.
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_INITIALIZE_CONTEXT]
 *     Context 초기화 에러. (자세한 에러는 aErrorStack의 하부를 확인할것)
 * @endcode
 */
inline stlStatus stlInitializeContext( stlContext     * aContext,
                                       stlInt16         aFamily,
                                       stlInt32         aType,
                                       stlInt32         aProtocol,
                                       stlBool          aIsListen,
                                       stlChar        * aAddr,
                                       stlInt32         aPort,
                                       stlErrorStack  * aErrorStack )
{
    return stmInitializeContext(aContext,
                                aFamily,
                                aType,
                                aProtocol,
                                aIsListen,
                                aAddr,
                                aPort,
                                aErrorStack);
}

/**
 * @brief 통신을 위한 Context를 종료한다. Context에서 사용한 연결 상태를<BR>
 * 종료하지만, 그 사용 메모리를 해제하지는 않으므로, 호출부에서 메모리 해제를 해주어야 한다.
 * @param[in] aContext 종료할 Context의 포인터
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_FINALIZE_CONTEXT]
 *     Context 종료 에러. (자세한 에러는 aErrorStack의 하부를 확인할것)
 * @endcode
 */
inline stlStatus stlFinalizeContext( stlContext     * aContext,
                                     stlErrorStack  * aErrorStack )
{
    return stmFinalizeContext(aContext, aErrorStack);
}

/**
 * @brief 주어진 Context에 대하여, 통신 요청을 받는다.
 * @param[in] aContext Poll을 수행할 Context의 포인터
 * @param[out] aNewContext Poll 결과로 새로 얻게된 Context의 포인터
 * @param[in] aTimeout Poll을 대기 할 시간(usec)
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_TIMEDOUT]
 *     Poll에서 time-out이 발생
 * [STL_ERRCODE_POLL_CONTEXT]
 *     Context Poll 에러. (자세한 에러는 aErrorStack의 하부를 확인할것)
 * @endcode
 */
inline stlStatus stlPollContext( stlContext     * aContext,
                                 stlContext     * aNewContext,
                                 stlInterval      aTimeout,
                                 stlErrorStack  * aErrorStack )
{
    return stmPollContext(aContext, aNewContext, aTimeout, aErrorStack);
}

/**
 * @brief 주어진 @a aContext로 주어진 주소와 포트로 non blocking 접속요청을 한다.
 * @param[in] aContext 접속 요청을 수행할 Context 포인터
 * @param[in] aTimeout connection timeout(usec.)
 * @param[out] aErrorStack 에러 스택
 */
inline stlStatus stlConnectContext( stlContext    * aContext,
                                    stlInterval     aTimeout,
                                    stlErrorStack * aErrorStack )
{
    return stmConnectContext(aContext, aTimeout, aErrorStack);
}

/**
 * @brief 주어진 @a aContext로 packet을 전송한다.
 * @param[in] aContext packet을 전송할 Context
 * @param[in] aPacketBuffer 전송할 packet이 담긴 버퍼
 * @param[in,out] aPacketLength 전송할 packet의 길이 및 실제로 전송한 packet의 길이
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_WRITE_PACKET]
 *     Write Packet 에러. (자세한 에러는 aErrorStack의 하부를 확인할것)
 * @endcode
 */
inline stlStatus stlWritePacket( stlContext     * aContext,
                                 stlChar        * aPacketBuffer,
                                 stlSize          aPacketLength,
                                 stlErrorStack  * aErrorStack )
{
    return stmWritePacket(aContext, aPacketBuffer, aPacketLength, aErrorStack);
}

/**
 * @brief 주어진 @a aContext로부터 읽어들일 Packet의 길이를 구한다. Packet을 받는
 * 쪽에서는 받아야 할 Packet의 길이를 알 수 없으므로, 우선 본 함수를 이용하여
 * Packet의 길이를 얻어온 후에, stlReadPacket 함수를 이용하여 실제 Packet을 읽어들이도록 한다.
 * @param[in] aContext Packet을 읽어들일 Context
 * @param[out] aEndian 읽어들인 Packet의 endian, STL_BIG_ENDIAN/STL_LITTLE_ENDIAN
 * @param[out] aPacketLength 읽어들인 Packet의 길이
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_READ_PACKET]
 *     Write Packet 에러. (자세한 에러는 aErrorStack의 하부를 확인할것)
 * @endcode
 */
inline stlStatus stlGetPacketLength( stlContext     * aContext,
                                     stlInt8        * aEndian,
                                     stlSize        * aPacketLength,
                                     stlErrorStack  * aErrorStack )
{
    return stmGetPacketLength(aContext, aEndian, aPacketLength, aErrorStack);
}

/**
 * @brief 주어진 @a aContext로부터 packet을 읽어들인다.
 * @param[in] aContext Packet을 읽어들일 Context
 * @param[out] aPacketBuffer 읽어들인 Packet을 저장할 버퍼
 * @param[in] aPacketLength 읽어들인 Packet의 길이
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_READ_PACKET]
 *     Read Packet 에러. (자세한 에러는 aErrorStack의 하부를 확인할것)
 * @endcode
 */
inline stlStatus stlReadPacket( stlContext     * aContext,
                                stlChar        * aPacketBuffer,
                                stlSize          aPacketLength,
                                stlErrorStack  * aErrorStack )
{
    return stmReadPacket(aContext, aPacketBuffer, aPacketLength, aErrorStack);
}


/**
 * @brief Context와 data를 전송받는다
 * @param[in] aContext Context를 전송받는 수단이 되는 Context
 * @param[out] aNewContext 전달받은 새 Context
 * @param[out] aBuf 전송받은 message가 담길 버퍼
 * @param[in,out] aLen 전송받을 message의 최대길이 및 실제로 전송받은 message의 길이
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_RECV_CONTEXT]
 *     Socket descriptor를 전송받는 과정에서 에러 발생.(자세한 에러는 aErrorStack의 하부를 확인할것)
 * @endcode
 */
inline stlStatus stlRecvContext( stlContext     * aContext,
                                 stlContext     * aNewContext,
                                 stlChar        * aBuf,
                                 stlSize        * aLen,
                                 stlErrorStack  * aErrorStack )
{
    return stmRecvContext( aContext, aNewContext, aBuf, aLen, aErrorStack);
}

/**
 * @brief Context와 data를 전송한다.
 * @param[in] aContext Context를 전송하는 수단이 되는 Context
 * @param[in] aAddr 소켓이 접속되어 있지 않은 경우, 행선지 주소
 * @param[in] aPassingContext 전달할 Context
 * @param[in] aBuf 전송할 message가 담긴 버퍼
 * @param[in,out] aLen 전송할 message의 길이 및 실제로 전송한 message의 길이
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_SEND_CONTEXT]
 *     Socket descriptor를 전송하는 과정에서 에러 발생.(자세한 에러는 aErrorStack의 하부를 확인할것)
 * @endcode
 */
inline stlStatus stlSendContext( stlContext     * aContext,
                                 stlSockAddr    * aAddr,
                                 stlContext     * aPassingContext,
                                 const stlChar  * aBuf,
                                 stlSize          aLen,
                                 stlErrorStack  * aErrorStack )
{
    return stmSendContext( aContext, aAddr, aPassingContext, aBuf, aLen, aErrorStack );
}


/**
 * @brief 연결된 통신 Context pair를 생성한다.
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
 * @param[out] aContexts 연결된 socket을 반환. aContexts[0], aSocket[1]
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_INITIALIZE_CONTEXT]
 *     Context pair 생성 에러. (자세한 에러는 aErrorStack의 하부를 확인할것)
 * @endcode
 * @see stlSocketPair
 */
inline stlStatus stlCreateContextPair( stlInt32          aDomain,
                                       stlInt32          aType,
                                       stlInt32          aProtocol,
                                       stlContext        aContexts[2],
                                       stlErrorStack    *aErrorStack )
{
    return stmCreateContextPair( aDomain, aType, aProtocol, aContexts, aErrorStack );
}


/** @} */
