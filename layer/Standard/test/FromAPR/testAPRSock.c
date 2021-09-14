#include <stdlib.h>
#include <stl.h>

#define DATASTR "This is a test"
#define STRLEN 8092

#ifdef WIN32
#ifdef BINPATH
#define TESTBINPATH STL_STRINGIFY(BINPATH) "/"
#else
#define TESTBINPATH ""
#endif
#else
#define TESTBINPATH "./"
#endif

#ifdef WIN32
#define EXTENSION ".exe"
#elif NETWARE
#define EXTENSION ".nlm"
#else
#define EXTENSION
#endif

#define PRINTERR                                            \
    do                                                      \
    {                                                       \
        stlPrintf( "ERRCODE: %d, ERRSTRING: %s\n",          \
                   stlGetLastErrorCode( &sErrStack ),       \
                   stlGetLastErrorMessage( &sErrStack ) );  \
    } while(0)

/**
 * @brief Child Process를 구동
 */
stlStatus LaunchChild( stlProc       * aProc,
                       stlChar       * aArg,
                       stlErrorStack * aErrStack )
{
    stlBool        sIsChild;
    stlChar      * sArgs[3];

    STL_TRY( stlForkProc( aProc,
                          &sIsChild,
                          aErrStack )
             == STL_SUCCESS );
    
    if ( sIsChild == STL_TRUE )
    {
        /* Child Process */

        /*
         * shmConsumer 프로세스를 수행
         */

        sArgs[0] = TESTBINPATH "sockChild" EXTENSION;
        sArgs[1] = aArg;
        sArgs[2] = NULL;
        STL_TRY( stlExecuteProc( sArgs[0],
                                 sArgs,
                                 aErrStack )
                 == STL_SUCCESS );
    }
    else
    {
        /* Server Process */
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief stlGetSockAddrInfo() 함수 테스트
 */

stlBool TestAddrInfo()
{
    stlSockAddr    sSockAddr;
    stlChar        sSockStr[256];
    
    stlErrorStack  sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );

    STL_TRY( stlSetSockAddr( &sSockAddr,
                             STL_AF_INET,
                             "127.0.0.1",
                             8021,
                             NULL,
                             &sErrStack )
             == STL_SUCCESS );

    STL_TRY( stlGetSockAddrIp( &sSockAddr,
                               sSockStr,
                               256,
                               &sErrStack )
             == STL_SUCCESS );
    STL_TRY( strcmp("127.0.0.1", sSockStr ) == 0);

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;

    return STL_FALSE;
}

/**
 * brief 소켓을 생성, 설정한다.
 */
stlStatus SetupSocket( stlSocket     * aSocket,
                       stlSockAddr   * aSockAddr,
                       stlErrorStack * aErrStack )
{
    stlSocket    sSock;

    STL_TRY( stlSetSockAddr( aSockAddr,
                             STL_AF_INET,
                             "127.0.0.1",
                             8021,
                             NULL,
                             aErrStack )
             == STL_SUCCESS );
    
    STL_TRY( stlCreateSocket( &sSock,
                              STL_AF_INET,
                              SOCK_STREAM,
                              STL_PROTO_TCP,
                              aErrStack )
             == STL_SUCCESS);

    STL_TRY( stlSetSocketOption( sSock,
                              STL_SOPT_SO_REUSEADDR,
                              1,    /* ON */
                              aErrStack )
             == STL_SUCCESS );

    STL_TRY( stlBind( sSock,
                      aSockAddr,
                      aErrStack )
             == STL_SUCCESS );

    STL_TRY( stlListen( sSock,
                        5,        /* BACKLOG */
                        aErrStack )
             == STL_SUCCESS);

    *aSocket = sSock;
    
    return STL_SUCCESS;

    STL_FINISH;

    *aSocket = -1;
    
    return STL_FAILURE;
}

/**
 * @brief stlBind(), stlListen() 함수 테스트
 */
stlBool TestCreateBindListen()
{
    stlSocket   sSocket;
    stlSockAddr sSockAddr;

    stlErrorStack  sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );
    
    STL_TRY( SetupSocket( &sSocket, &sSockAddr, &sErrStack ) == STL_SUCCESS );
    STL_TRY( sSocket != -1 );
    
    STL_TRY( stlCloseSocket(sSocket, &sErrStack ) == STL_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;
}

/**
 * @brief stlSocketSend() 함수 테스트
 */
stlBool TestSend()
{
    stlSocket   sSock1;
    stlSocket   sSock2;

    stlSockAddr sSockAddr;
    
    stlProc    sProc;
    stlInt32   sExitCode;
    
    stlSize    sLength;

    stlErrorStack  sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );
    
    STL_TRY( SetupSocket( &sSock1, &sSockAddr, &sErrStack ) == STL_SUCCESS );

    STL_TRY( LaunchChild( &sProc,
                          "read",   /* sockChild Program Argument */
                          &sErrStack )
             == STL_SUCCESS );

    STL_TRY( stlAccept( sSock1, &sSock2, &sErrStack ) == STL_SUCCESS );

    sLength = strlen(DATASTR);
    STL_TRY( stlSend( sSock2,
                            DATASTR,
                            &sLength,
                            &sErrStack )
             == STL_SUCCESS );

    /* Make sure that the client received the data we sent */
    STL_TRY( stlWaitProc( &sProc,
                          &sExitCode,
                          &sErrStack )
             == STL_SUCCESS );
    
    STL_TRY( strlen(DATASTR) == sExitCode );

    STL_TRY( stlCloseSocket( sSock2, &sErrStack ) == STL_SUCCESS );
    STL_TRY( stlCloseSocket( sSock1, &sErrStack ) == STL_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;    
}

/**
 * @brief stlSocketRecv() 함수 테스트
 */
stlBool TestRecv()
{
    stlSocket  sSock1;
    stlSocket  sSock2;

    stlSockAddr sSockAddr;
    
    stlProc    sProc;
    stlInt32   sExitCode;
    
    stlSize    sLength = STRLEN;
    stlChar    sDataStr[STRLEN];

    stlErrorStack  sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );
    
    STL_TRY( SetupSocket( &sSock1, &sSockAddr, &sErrStack ) == STL_SUCCESS );

    STL_TRY( LaunchChild( &sProc,
                          "write",   /* sockChild Program Argument */
                          &sErrStack )
             == STL_SUCCESS );

    STL_TRY( stlAccept( sSock1, &sSock2, &sErrStack ) == STL_SUCCESS );

    memset(sDataStr, 0, STRLEN);
    STL_TRY( stlRecv( sSock2,
                      sDataStr,
                      &sLength,
                      &sErrStack )
             == STL_SUCCESS );

    /* Make sure that the server received the data we sent */
    STL_TRY( strcmp(DATASTR, sDataStr) == 0 );
    
    STL_TRY( stlWaitProc( &sProc,
                          &sExitCode,
                          &sErrStack )
             == STL_SUCCESS );
    
    STL_TRY( strlen(DATASTR) == sExitCode );

    STL_TRY( stlCloseSocket( sSock2, &sErrStack ) == STL_SUCCESS );
    STL_TRY( stlCloseSocket( sSock1, &sErrStack ) == STL_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;    
}

/**
 * @brief ST 에서는 APR 에서 제공하던 Socket 종료여부를 확인할 interface를 제공하지 않음
 *        이를 Socket Close 후에 recv()등을 통해 Simulation Test 함
 */
stlBool TestAtReadEof()
{
    stlSocket  sSock1;
    stlSocket  sSock2;

    stlSockAddr sSockAddr;
    
    stlProc    sProc;
    stlInt32   sExitCode;
    
    stlSize    sLength = STRLEN;
    stlChar    sDataStr[STRLEN];

    stlErrorStack  sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );
    
    STL_TRY( SetupSocket( &sSock1, &sSockAddr, &sErrStack ) == STL_SUCCESS );

    /*
     * Socket 활성화 후,
     * Child 가 종료되었을 때 recv 를 테스트함.
     */ 
    STL_TRY( LaunchChild( &sProc,
                          "write",   /* sockChild Program Argument */
                          &sErrStack )
             == STL_SUCCESS );

    STL_TRY( stlAccept( sSock1, &sSock2, &sErrStack ) == STL_SUCCESS );

    memset(sDataStr, 0, STRLEN);
    STL_TRY( stlRecv( sSock2,
                      sDataStr,
                      &sLength,
                      &sErrStack )
             == STL_SUCCESS );

    /* Make sure that the server received the data we sent */
    STL_TRY(strcmp(DATASTR, sDataStr) == 0);

    STL_TRY( stlWaitProc( &sProc,
                          &sExitCode,
                          &sErrStack )
             == STL_SUCCESS );
    
    STL_TRY( strlen(DATASTR) == sExitCode );
    
    /* The child is dead, so should be the remote socket */
    sLength = STRLEN;
    STL_TRY( stlRecv( sSock2,
                      sDataStr,
                      &sLength,
                      &sErrStack )
             != STL_SUCCESS );
    STL_TRY( stlGetLastErrorCode( &sErrStack ) == STL_ERRCODE_EOF );

    /*
     * Socket 활성화 후,
     * Child 가 소켓을 명시적으로 종료하였을 때 recv 를 테스트함.
     */ 
    STL_TRY( stlCloseSocket( sSock2, &sErrStack ) == STL_SUCCESS );

    STL_TRY( LaunchChild( &sProc,
                          "close",   /* sockChild Program Argument */
                          &sErrStack )
             == STL_SUCCESS );

    STL_TRY( stlAccept( sSock1,
                        &sSock2,
                        &sErrStack )
             == STL_SUCCESS);

    /* The child closed the socket as soon as it could... */
    STL_TRY( stlWaitProc( &sProc,
                          &sExitCode,
                          &sErrStack )
             == STL_SUCCESS );
    STL_TRY( sExitCode == 0 );

    sLength = STRLEN;
    STL_TRY( stlRecv( sSock2,
                      sDataStr,
                      &sLength,
                      &sErrStack )
             != STL_SUCCESS );
    STL_TRY( stlGetLastErrorCode( &sErrStack ) == STL_ERRCODE_EOF );
    
    STL_TRY( stlCloseSocket( sSock2, &sErrStack ) == STL_SUCCESS );
    STL_TRY( stlCloseSocket( sSock1, &sErrStack ) == STL_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;    
}


/**
 * @brief stlSocketGetAddr() 함수 테스트
 */
stlBool TestGetAddr()
{
    stlSocket    sAcceptSock;
    stlSocket    sLocalSock;
    stlSocket    sClientSock;

    stlSockAddr  sSockAddr;
    stlSockAddr  sClientSockAddr;
    
    stlChar      sSockStr[256];
    stlChar      sClientStr[256];
    
    stlPollFd    sPollFd;
    stlInt32     sNum;

    stlInt16     sSockFamily;
    stlChar      sSockAddress[256];
    stlPort      sSockPort;
    
    stlErrorStack  sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );

    STL_TRY( SetupSocket( &sLocalSock, &sSockAddr, &sErrStack ) == STL_SUCCESS );

    STL_TRY( stlGetSockAddr( &sSockAddr,
                             sSockAddress,
			     256,
                             &sSockPort,
                             NULL,
                             0,
                             &sErrStack )
             == STL_SUCCESS );
    
    STL_TRY( stlCreateSocket( &sClientSock,
                              sSockFamily,
                              SOCK_STREAM,
                              STL_PROTO_TCP,
                              &sErrStack )
             == STL_SUCCESS);

    STL_TRY( stlSetSocketOption( sClientSock,
                                 STL_SOPT_SO_NONBLOCK,
                                 1,
                                 &sErrStack )
             == STL_SUCCESS);

    /* It is valid for a connect() on a socket with NONBLOCK set to
     * succeed (if the connection can be established synchronously),
     * but if it does, this test cannot proceed.  */

    STL_TRY( stlConnect( sClientSock,
                         &sSockAddr,
                         STL_INFINITE_TIME,
                         &sErrStack )
             != STL_SUCCESS );  /* EINPROGRESS */
    
    STL_TRY( stlAccept( sLocalSock,
                        &sAcceptSock,
                        &sErrStack )
             == STL_SUCCESS );
    
    /* wait for writability */

    sPollFd.mSocketHandle = sAcceptSock;
    sPollFd.mReqEvents = STL_POLLOUT;

    STL_TRY( stlPoll( &sPollFd,
                      1,
                      &sNum,
                      5 * STL_USEC_PER_SEC,
                      &sErrStack )
             == STL_SUCCESS );

    STL_TRY(stlGetPeerName( sClientSock,
                            &sClientSockAddr,
                            &sErrStack )
            == STL_SUCCESS);

    STL_TRY( stlGetSockAddrIp( &sSockAddr,
                               sSockStr,
                               256,
                               &sErrStack )
             == STL_SUCCESS );

    STL_TRY( stlGetSockAddrIp( &sClientSockAddr,
                               sClientStr,
                               256,
                               &sErrStack )
             == STL_SUCCESS );
    
    STL_TRY( strcmp(sSockStr, sClientStr) == 0 );

    STL_TRY( stlCloseSocket( sClientSock, &sErrStack ) == STL_SUCCESS );
    STL_TRY( stlCloseSocket( sAcceptSock, &sErrStack ) == STL_SUCCESS );
    STL_TRY( stlCloseSocket( sLocalSock, &sErrStack ) == STL_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;    
}

stlInt32 main(stlInt32 aArgc, stlChar** aArgv)
{
    stlInt32 sState = 0;

    STL_TRY(stlInitialize() == STL_SUCCESS);

    /*
     * Test AddrInfo
     */
    sState = 1;
    STL_TRY(TestAddrInfo() == STL_TRUE);

    /*
     * Test CreateBindListen
     */
    sState = 3;
    STL_TRY(TestCreateBindListen() == STL_TRUE);

    /*
     * Test Send
     */
    sState = 4;
    STL_TRY(TestSend() == STL_TRUE);

    /*
     * Test Recv
     */
    sState = 5;
    STL_TRY(TestRecv() == STL_TRUE);

    /*
     * Test AtReadEof
     */
    sState = 6;
    STL_TRY(TestAtReadEof() == STL_TRUE);

    /*
     * Test GetAddr
     */
    sState = 9;
    STL_TRY(TestGetAddr() == STL_TRUE);

    stlPrintf("PASS");

    return (0);

    STL_FINISH;

    stlPrintf("FAILURE %d", sState);

    return (-1);
}
