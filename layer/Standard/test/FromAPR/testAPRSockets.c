#include <stdio.h>
#include <stl.h>

#define STRLEN 21

#define PRINTERR                                            \
    do                                                      \
    {                                                       \
        stlPrintf( "ERRCODE: %d, ERRSTRING: %s\n",          \
                   stlGetLastErrorCode( &sErrStack ),       \
                   stlGetLastErrorMessage( &sErrStack ) );  \
    } while(0)

/** 
 * @brief Datagram(비연결) Message 의 전송 테스트
 */ 
stlBool SendToReceiveFromHelper( stlChar       * aIPAddr,
                                 stlInt16        aFamily,
                                 stlErrorStack * aErrStack )
{
    stlSocket    sSock1;
    stlSocket    sSock2;

    stlSockAddr  sFromSockAddr;
    stlSockAddr  sToSockAddr;
    
    stlChar      sSendBuf[STRLEN] = "APR_INET, SOCK_DGRAM";
    stlChar      sRecvBuf[80];
    
    stlChar      sIpAddr[80];
    
    stlPort      sFromPort;
    stlInt16     sAddressFamily;
    
    stlSize      sLen = 30;

    STL_TRY( stlCreateSocket(&sSock1,
                             aFamily,
                             STL_SOCK_DGRAM,
                             0,
                             aErrStack )
             == STL_SUCCESS );

    STL_TRY( stlCreateSocket( &sSock2,
                              aFamily,
                              STL_SOCK_DGRAM,
                              0,
                              aErrStack )
             == STL_SUCCESS );

    STL_TRY( stlSetSockAddr( &sToSockAddr,
                             aFamily,
                             aIPAddr,
                             7772,
                             NULL,
                             aErrStack)
             == STL_SUCCESS );
    
    STL_TRY( stlSetSockAddr( &sFromSockAddr,
                             aFamily,
                             aIPAddr,
                             7771,
                             NULL,
                             aErrStack )
             == STL_SUCCESS );

    STL_TRY( stlSetSocketOption( sSock1,
                                 STL_SOPT_SO_REUSEADDR,
                                 1,
                                 aErrStack )
             == STL_SUCCESS);
    
    STL_TRY( stlSetSocketOption( sSock2,
                                 STL_SOPT_SO_REUSEADDR,
                                 1,
                                 aErrStack )
             == STL_SUCCESS);

    STL_TRY( stlBind( sSock1,
                      &sToSockAddr,
                      aErrStack )
             == STL_SUCCESS );

    STL_TRY( stlBind( sSock2,
                      &sFromSockAddr,
                      aErrStack )
             == STL_SUCCESS );

    sLen = STRLEN;
    STL_TRY( stlSendTo( sSock2,
                        &sToSockAddr,
                        0,
                        sSendBuf,
                        &sLen,
                        aErrStack )
             == STL_SUCCESS);
    STL_TRY(STRLEN == sLen);
    
    /* fill the "from" sockaddr with a random address from another
     * family to ensure that recvfrom sets it up properly. */
#if STL_FLAG_IPV6_SUPPORT
    if ( aFamily == STL_AF_INET )
    {
        STL_TRY( stlSetSockAddr( &sFromSockAddr,
                                 STL_AF_INET6,
                                 "3ffE:816e:abcd:1234::1",
                                 4242,
                                 NULL,
                                 aErrStack )
                 == STL_SUCCESS );
    }
    else
#endif
    {
        STL_TRY( stlSetSockAddr( &sFromSockAddr,
                                 STL_AF_INET,
                                 "127.1.2.3",
                                 4242,
                                 NULL,
                                 aErrStack  )
                 == STL_SUCCESS );
    }

    sLen = 80;
    STL_TRY( stlRecvFrom( sSock1,
                          & sFromSockAddr,
                          0,
                          sRecvBuf,
                          &sLen,
                          STL_FALSE,
                          aErrStack )
             == STL_SUCCESS );
    STL_TRY(STRLEN == sLen);

    STL_TRY( strcmp("APR_INET, SOCK_DGRAM", sRecvBuf) == 0 );

    STL_TRY( stlGetSockAddrIp( &sFromSockAddr,
                               sIpAddr,
                               80,
                               aErrStack )
             == STL_SUCCESS );
    STL_TRY( strcmp(aIPAddr, sIpAddr) == 0);

    STL_TRY( stlGetSockAddr( &sFromSockAddr,
                             sIpAddr,
                             80,
                             &sFromPort,
                             NULL,
                             0,
                             aErrStack )
             == STL_SUCCESS );
    
    STL_TRY( 7771 == sFromPort );

    STL_TRY( stlCloseSocket( sSock1, aErrStack ) == STL_SUCCESS );
    STL_TRY( stlCloseSocket( sSock2, aErrStack ) == STL_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief Datagram STL_AF_INET 테스트 
 */
stlBool TestSendToReceiveFrom()
{
    stlErrorStack  sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );

    STL_TRY( SendToReceiveFromHelper( "127.0.0.1",
                                      STL_AF_INET,
                                      &sErrStack )
             == STL_TRUE );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;

    return STL_FALSE;
}

#if STL_FLAG_IPV6_SUPPORT

/**
 * @brief IPv6 TCP Socket 테스트 
 */

stlBool TestTcp6Socket()
{
    stlSocket  sSock;

    stlErrorStack  sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );
    
    STL_TRY( stlCreateSocket( &sSock,
                              STL_AF_INET6,
                              STL_SOCK_STREAM,
                              0,
                              &sErrStack )
             == STL_SUCCESS );

    STL_TRY( stlCloseSocket( sSock, &sErrStack ) == STL_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;

    return STL_FALSE;
}

/**
 * @brief IPv6 TCP Socket 테스트 
 */

stlBool TestUdp6Socket()
{
    stlSocket  sSock;

    stlErrorStack  sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );
    
    STL_TRY( stlCreateSocket( &sSock,
                              STL_AF_INET6,
                              STL_SOCK_DGRAM,
                              0,
                              &sErrStack )
             == STL_SUCCESS);

    STL_TRY( stlCloseSocket( sSock, &sErrStack ) == STL_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;

    return STL_FALSE;
}

/**
 * @brief IPv6 Receive 테스트 
 */

stlBool TestSendToReceiveFrom6()
{
    stlErrorStack  sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );

    STL_TRY( SendToReceiveFromHelper( "::1",
                                      STL_AF_INET6,
                                      &sErrStack )
            == STL_TRUE );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;

    return STL_FALSE;
}
#endif


stlInt32 main(stlInt32 aArgc, stlChar** aArgv)
{
    stlInt32 sState = 0;

    STL_TRY(stlInitialize() == STL_SUCCESS);

    /*
     * Test SendToReceiveFrom
     */
    sState = 3;
    STL_TRY(TestSendToReceiveFrom() == STL_TRUE);

#if STL_FLAG_IPV6_SUPPORT
    /*
     * Test TcpSocket
     */
    sState = 4;
    STL_TRY(TestTcp6Socket() == STL_TRUE);

    /*
     * Test UdpSocket
     */
    sState = 5;
    STL_TRY(TestUdp6Socket() == STL_TRUE);

    /*
     * Test SendToReceiveFrom
     */
    sState = 6;
    STL_TRY(TestSendToReceiveFrom6() == STL_TRUE);
#endif

    stlPrintf("PASS");

    return (0);

    STL_FINISH;

    stlPrintf("FAILURE %d", sState);

    return (-1);
}

