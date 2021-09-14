#include <stl.h>

stlInt32 gPortNo = 12345;
stlChar gSvrFile[] = "/tmp/testSockSvr.tmp";
stlChar gClnFile[] = "/tmp/testSockCln.tmp";


/* Client Thread Functions */
void * STL_THREAD_FUNC threadIPv4( stlThread * aThread, void * aArg )
{
    stlErrorStack sErrorStack;
    stlSocket     sClnSocket;
    stlSockAddr   sClnSockAddr;
    stlChar       sMsgBuf[32];
    stlSize       sMsgLen = 32;
    stlInt32      sState = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    STL_TRY( stlCreateSocket( &sClnSocket, 
                              STL_AF_INET,
                              SOCK_STREAM,
                              0,
                              &sErrorStack) == STL_SUCCESS );

    STL_TRY( stlSetSockAddr( &sClnSockAddr,
                             STL_AF_INET,
                             "127.0.0.1",
                             gPortNo,
                             NULL,
                             &sErrorStack ) == STL_SUCCESS );

    STL_TRY( stlConnect( sClnSocket,
                         &sClnSockAddr,
                         0,
                         &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    stlMemset( sMsgBuf, 0x00, 32 );
    STL_TRY( stlRecv( sClnSocket,
                      sMsgBuf,
                      &sMsgLen,
                      &sErrorStack ) == STL_SUCCESS );
    
    stlPrintf( "Client received message '%s'\n", sMsgBuf );

    stlStrcpy( sMsgBuf, "Dambae" );
    sMsgLen = stlStrlen(sMsgBuf);
    STL_TRY( stlSend( sClnSocket,
                      sMsgBuf,
                      &sMsgLen,
                      &sErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( stlCloseSocket( sClnSocket,
                             &sErrorStack ) == STL_SUCCESS );

    STL_TRY( stlExitThread( aThread,
                            STL_SUCCESS,
                            &sErrorStack ) == STL_SUCCESS );
    
    return NULL;

    STL_FINISH;
   
    stlPrintf( "Client threadIPv4 Failed with ErrorCode(%d)",
               stlGetLastErrorCode(&sErrorStack) );

    switch( sState )
    {
        case 1:
            stlCloseSocket( sClnSocket, &sErrorStack);
        default:
            break;
    }

    stlExitThread( aThread, STL_FAILURE, &sErrorStack );
   
    return NULL;
}

void * STL_THREAD_FUNC threadIPv6( stlThread * aThread, void * aArg )
{
    stlErrorStack sErrorStack;
    stlSocket     sClnSocket;
    stlSockAddr   sClnSockAddr;
    stlChar       sMsgBuf[32];
    stlSize       sMsgLen = 32;
    stlInt32      sState = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    STL_TRY( stlCreateSocket( &sClnSocket, 
                              STL_AF_INET6,
                              SOCK_STREAM,
                              0,
                              &sErrorStack) == STL_SUCCESS );
  
    STL_TRY( stlSetSockAddr( &sClnSockAddr,
                             STL_AF_INET6,
                             "::1",
                             gPortNo,
                             NULL,
                             &sErrorStack ) == STL_SUCCESS );

    STL_TRY( stlConnect( sClnSocket,
                         &sClnSockAddr,
                         0,
                         &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    stlMemset( sMsgBuf, 0x00, 32 );
    STL_TRY( stlRecv( sClnSocket,
                      sMsgBuf,
                      &sMsgLen,
                      &sErrorStack ) == STL_SUCCESS );
    
    stlPrintf( "Client received message '%s'\n", sMsgBuf );

    stlStrcpy( sMsgBuf, "Dambae" );
    sMsgLen = stlStrlen(sMsgBuf);
    STL_TRY( stlSend( sClnSocket,
                      sMsgBuf,
                      &sMsgLen,
                      &sErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( stlCloseSocket( sClnSocket,
                             &sErrorStack ) == STL_SUCCESS );

    STL_TRY( stlExitThread( aThread,
                            STL_SUCCESS,
                            &sErrorStack ) == STL_SUCCESS );
    
    return NULL;

    STL_FINISH;
   
    stlPrintf( "Client threadIPv6 Failed with ErrorCode(%d)",
               stlGetLastErrorCode(&sErrorStack) );

    switch( sState )
    {
        case 1:
            stlCloseSocket( sClnSocket, &sErrorStack);
        default:
            break;
    }

    stlExitThread( aThread, STL_FAILURE, &sErrorStack );
   
    return NULL;
}

void * STL_THREAD_FUNC threadUnix( stlThread * aThread, void * aArg )
{
    stlErrorStack sErrorStack;
    stlSocket     sClnSocket;
    stlSockAddr   sClnSockAddr;
    stlSockAddr   sSvrSockAddr;
    stlChar       sMsgBuf[32];
    stlSize       sMsgLen = 32;
    stlInt32      sState = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    STL_TRY( stlCreateSocket( &sClnSocket, 
                              STL_AF_UNIX,
                              SOCK_DGRAM,
                              0,
                              &sErrorStack) == STL_SUCCESS );

    (void)stlRemoveFile( gClnFile, &sErrorStack ); 

    stlMemset( &sClnSockAddr, 0x00, STL_SIZEOF(sClnSockAddr) );
    STL_TRY( stlSetSockAddr( &sClnSockAddr,
                             STL_AF_UNIX,
                             NULL,
                             0,
                             gClnFile,
                             &sErrorStack ) == STL_SUCCESS );

    STL_TRY( stlBind( sClnSocket,
                      &sClnSockAddr,
                      &sErrorStack) == STL_SUCCESS );

    stlMemset( sMsgBuf, 0x00, 32 );
    STL_TRY( stlRecvFrom( sClnSocket,
                          &sSvrSockAddr,
                          0,
                          sMsgBuf,
                          &sMsgLen,
                          STL_FALSE,
                          &sErrorStack ) == STL_SUCCESS );
    
    stlPrintf( "Client received message '%s'\n", sMsgBuf );

    stlStrcpy( sMsgBuf, "Dambae" );
    sMsgLen = stlStrlen(sMsgBuf);
    STL_TRY( stlSendTo( sClnSocket,
                        &sSvrSockAddr,
                        0,
                        sMsgBuf,
                        &sMsgLen,
                        &sErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( stlCloseSocket( sClnSocket,
                             &sErrorStack ) == STL_SUCCESS );

    STL_TRY( stlExitThread( aThread,
                            STL_SUCCESS,
                            &sErrorStack ) == STL_SUCCESS );
    
    return NULL;

    STL_FINISH;
   
    stlPrintf( "Client threadUnix Failed with ErrorCode(%d)",
               stlGetLastErrorCode(&sErrorStack) );

    switch( sState )
    {
        case 1:
            stlCloseSocket( sClnSocket, &sErrorStack);
        default:
            break;
    }

    stlExitThread( aThread, STL_FAILURE, &sErrorStack );
   
    return NULL;
}

void * STL_THREAD_FUNC threadUnixStream( stlThread * aThread, void * aArg )
{
    stlErrorStack sErrorStack;
    stlSocket     sClnSocket;
    stlSockAddr   sClnSockAddr;
    stlChar       sMsgBuf[32];
    stlSize       sMsgLen = 32;
    stlInt32      sState = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    STL_TRY( stlCreateSocket( &sClnSocket, 
                              STL_AF_UNIX,
                              SOCK_STREAM,
                              0,
                              &sErrorStack) == STL_SUCCESS );

    stlMemset( &sClnSockAddr, 0x00, STL_SIZEOF(sClnSockAddr) );
    STL_TRY( stlSetSockAddr( &sClnSockAddr,
                             STL_AF_UNIX,
                             NULL,
                             0,
                             gSvrFile,
                             &sErrorStack ) == STL_SUCCESS );

    STL_TRY( stlConnect( sClnSocket,
                         &sClnSockAddr,
                         0,
                         &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    stlMemset( sMsgBuf, 0x00, 32 );
    STL_TRY( stlRecv( sClnSocket,
                      sMsgBuf,
                      &sMsgLen,
                      &sErrorStack ) == STL_SUCCESS );
    
    stlPrintf( "Client received message '%s'\n", sMsgBuf );

    stlStrcpy( sMsgBuf, "Dambae" );
    sMsgLen = stlStrlen(sMsgBuf);
    STL_TRY( stlSend( sClnSocket,
                      sMsgBuf,
                      &sMsgLen,
                      &sErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( stlCloseSocket( sClnSocket,
                             &sErrorStack ) == STL_SUCCESS );

    STL_TRY( stlExitThread( aThread,
                            STL_SUCCESS,
                            &sErrorStack ) == STL_SUCCESS );
    
    return NULL;

    STL_FINISH;
   
    stlPrintf( "Client threadUnix Failed with ErrorCode(%d)",
               stlGetLastErrorCode(&sErrorStack) );

    switch( sState )
    {
        case 1:
            stlCloseSocket( sClnSocket, &sErrorStack);
        default:
            break;
    }

    stlExitThread( aThread, STL_FAILURE, &sErrorStack );
   
    return NULL;
}


/* Server Functions */
stlInt32 testIPv4( )
{
    stlInt32       sState = 0;
    stlErrorStack  sErrorStack;
    stlStatus      sThreadStatus;
    stlThread      sChildThread;
    stlSocket      sSvrSocket;
    stlSocket      sNewSocket;
    stlSockAddr    sSvrSockAddr;
    stlChar        sMsgBuf[32];
    stlSize        sMsgLen;
    
    STL_TRY( stlInitialize() == STL_SUCCESS );
    sState = 1;

    STL_INIT_ERROR_STACK( &sErrorStack );

    /* IPv4 test */
    STL_TRY( stlCreateSocket( &sSvrSocket, 
                              STL_AF_INET,
                              SOCK_STREAM,
                              0,
                              &sErrorStack) == STL_SUCCESS );
    sState = 2;

    stlMemset( &sSvrSockAddr, 0x00, STL_SIZEOF(sSvrSockAddr) );
    STL_TRY( stlSetSockAddr( &sSvrSockAddr,
                             STL_AF_INET,
                             "127.0.0.1",
                             gPortNo,
                             NULL,
                             &sErrorStack ) == STL_SUCCESS );

    STL_TRY( stlBind( sSvrSocket,
                      &sSvrSockAddr,
                      &sErrorStack) == STL_SUCCESS );

    STL_TRY( stlListen( sSvrSocket,
                        1,
                        &sErrorStack ) == STL_SUCCESS );

    STL_TRY( stlCreateThread( &sChildThread,
                              NULL,
                              threadIPv4,
                              NULL,
                              &sErrorStack ) == STL_SUCCESS );

    STL_TRY( stlAccept( sSvrSocket,
                        &sNewSocket,
                        &sErrorStack ) == STL_SUCCESS );
    sState = 3;

    stlStrcpy( sMsgBuf, "Hwarang" );
    sMsgLen = stlStrlen(sMsgBuf);
    STL_TRY( stlSend( sNewSocket,
                      sMsgBuf,
                      &sMsgLen,
                      &sErrorStack ) == STL_SUCCESS );

    stlMemset( sMsgBuf, 0x00, 32 );
    sMsgLen = 32;
    STL_TRY( stlRecv( sNewSocket,
                      sMsgBuf,
                      &sMsgLen,
                      &sErrorStack ) == STL_SUCCESS );

    stlPrintf( "Returned message is '%s'\n", sMsgBuf );


    STL_TRY( stlJoinThread( &sChildThread,
                            &sThreadStatus,
                            &sErrorStack ) == STL_SUCCESS );
    STL_TRY( sThreadStatus == STL_SUCCESS );

    sState = 1;
    STL_TRY( stlCloseSocket( sNewSocket,
                             &sErrorStack ) == STL_SUCCESS );
    STL_TRY( stlCloseSocket( sSvrSocket,
                             &sErrorStack ) == STL_SUCCESS );
    /* IPv6 test */


    
    sState = 0;
    stlTerminate();

    return STL_SUCCESS;

    STL_FINISH;

    stlPrintf( "Server Failed with ErrorCode(%d)",
               stlGetLastErrorCode(&sErrorStack) );
    stlPrintf( "sState(%d)",sState );

    switch( sState )
    {
        case 3:
            stlCloseSocket( sNewSocket, &sErrorStack);
        case 2:
            stlCloseSocket( sSvrSocket, &sErrorStack);
        case 1:
            stlTerminate();
        default:
            break;
    }

    return STL_FAILURE;
}

                                                                      
stlInt32 testIPv6( )
{
    stlInt32       sState = 0;
    stlErrorStack  sErrorStack;
    stlStatus      sThreadStatus;
    stlThread      sChildThread;
    stlSocket      sSvrSocket;
    stlSocket      sNewSocket;
    stlSockAddr    sSvrSockAddr;
    stlChar        sMsgBuf[32];
    stlSize        sMsgLen;
    
    STL_TRY( stlInitialize() == STL_SUCCESS );
    sState = 1;

    STL_INIT_ERROR_STACK( &sErrorStack );

    /* IPv4 test */
    STL_TRY( stlCreateSocket( &sSvrSocket, 
                              STL_AF_INET6,
                              SOCK_STREAM,
                              0,
                              &sErrorStack) == STL_SUCCESS );
    sState = 2;

    stlMemset( &sSvrSockAddr, 0x00, STL_SIZEOF(sSvrSockAddr) );
    STL_TRY( stlSetSockAddr( &sSvrSockAddr,
                             STL_AF_INET6,
                             "::1",
                             gPortNo,
                             NULL,
                             &sErrorStack ) == STL_SUCCESS );

    STL_TRY( stlBind( sSvrSocket,
                      &sSvrSockAddr,
                      &sErrorStack) == STL_SUCCESS );

    STL_TRY( stlListen( sSvrSocket,
                        1,
                        &sErrorStack ) == STL_SUCCESS );

    STL_TRY( stlCreateThread( &sChildThread,
                              NULL,
                              threadIPv6,
                              NULL,
                              &sErrorStack ) == STL_SUCCESS );

    STL_TRY( stlAccept( sSvrSocket,
                        &sNewSocket,
                        &sErrorStack ) == STL_SUCCESS );
    sState = 3;

    stlStrcpy( sMsgBuf, "Hwarang" );
    sMsgLen = stlStrlen(sMsgBuf);
    STL_TRY( stlSend( sNewSocket,
                      sMsgBuf,
                      &sMsgLen,
                      &sErrorStack ) == STL_SUCCESS );

    stlMemset( sMsgBuf, 0x00, 32 );
    sMsgLen = 32;
    STL_TRY( stlRecv( sNewSocket,
                      sMsgBuf,
                      &sMsgLen,
                      &sErrorStack ) == STL_SUCCESS );

    stlPrintf( "Returned message is '%s'\n", sMsgBuf );


    STL_TRY( stlJoinThread( &sChildThread,
                            &sThreadStatus,
                            &sErrorStack ) == STL_SUCCESS );
    STL_TRY( sThreadStatus == STL_SUCCESS );

    sState = 1;
    STL_TRY( stlCloseSocket( sNewSocket,
                             &sErrorStack ) == STL_SUCCESS );
    STL_TRY( stlCloseSocket( sSvrSocket,
                             &sErrorStack ) == STL_SUCCESS );
    /* IPv6 test */


    
    sState = 0;
    stlTerminate();

    return STL_SUCCESS;

    STL_FINISH;

    stlPrintf( "Server Failed with ErrorCode(%d)",
               stlGetLastErrorCode(&sErrorStack) );
    stlPrintf( "sState(%d)",sState );

    switch( sState )
    {
        case 3:
            stlCloseSocket( sNewSocket, &sErrorStack);
        case 2:
            stlCloseSocket( sSvrSocket, &sErrorStack);
        case 1:
            stlTerminate();
        default:
            break;
    }

    return STL_FAILURE;
}

stlInt32 testUnix( )
{
    stlInt32       sState = 0;
    stlErrorStack  sErrorStack;
    stlStatus      sThreadStatus;
    stlThread      sChildThread;
    stlSocket      sSvrSocket;
    stlSockAddr    sSvrSockAddr;
    stlSockAddr    sClnSockAddr;
    stlChar        sMsgBuf[32];
    stlSize        sMsgLen;
    
    STL_TRY( stlInitialize() == STL_SUCCESS );
    sState = 1;

    STL_INIT_ERROR_STACK( &sErrorStack );

    /* IPv4 test */
    STL_TRY( stlCreateSocket( &sSvrSocket, 
                              STL_AF_UNIX,
                              SOCK_DGRAM,
                              0,
                              &sErrorStack) == STL_SUCCESS );
    sState = 2;

    (void)stlRemoveFile( gSvrFile, &sErrorStack ); 

    stlMemset( &sSvrSockAddr, 0x00, STL_SIZEOF(sSvrSockAddr) );
    STL_TRY( stlSetSockAddr( &sSvrSockAddr,
                             STL_AF_UNIX,
                             NULL,
                             0,
                             gSvrFile,
                             &sErrorStack ) == STL_SUCCESS );

    STL_TRY( stlBind( sSvrSocket,
                      &sSvrSockAddr,
                      &sErrorStack) == STL_SUCCESS );

    stlMemset( &sClnSockAddr, 0x00, STL_SIZEOF(sClnSockAddr) );
    STL_TRY( stlSetSockAddr( &sClnSockAddr,
                             STL_AF_UNIX,
                             NULL,
                             0,
                             gClnFile,
                             &sErrorStack ) == STL_SUCCESS );

    STL_TRY( stlCreateThread( &sChildThread,
                              NULL,
                              threadUnix,
                              NULL,
                              &sErrorStack ) == STL_SUCCESS );

    stlSleep(100000);
    stlStrcpy( sMsgBuf, "Hwarang" );
    sMsgLen = stlStrlen(sMsgBuf);
    STL_TRY( stlSendTo( sSvrSocket,
                        &sClnSockAddr,
                        0,
                        sMsgBuf,
                        &sMsgLen,
                        &sErrorStack ) == STL_SUCCESS );

    stlMemset( sMsgBuf, 0x00, 32 );
    sMsgLen = 32;

    STL_TRY( stlRecvFrom( sSvrSocket,
                          &sSvrSockAddr,
                          0,
                          sMsgBuf,
                          &sMsgLen,
                          STL_FALSE,
                          &sErrorStack ) == STL_SUCCESS );

    stlPrintf( "Returned message is '%s'\n", sMsgBuf );


    STL_TRY( stlJoinThread( &sChildThread,
                            &sThreadStatus,
                            &sErrorStack ) == STL_SUCCESS );
    STL_TRY( sThreadStatus == STL_SUCCESS );

    sState = 1;
    STL_TRY( stlCloseSocket( sSvrSocket,
                             &sErrorStack ) == STL_SUCCESS );
    /* IPv6 test */


    
    sState = 0;
    stlTerminate();

    return STL_SUCCESS;

    STL_FINISH;

    stlPrintf( "Server Failed with ErrorCode(%d)",
               stlGetLastErrorCode(&sErrorStack) );
    stlPrintf( "sState(%d)",sState );

    switch( sState )
    {
        case 2:
            stlCloseSocket( sSvrSocket, &sErrorStack);
        case 1:
            stlTerminate();
        default:
            break;
    }

    return STL_FAILURE;
}

stlInt32 testUnixStream( )
{
    stlInt32       sState = 0;
    stlErrorStack  sErrorStack;
    stlStatus      sThreadStatus;
    stlThread      sChildThread;
    stlSocket      sSvrSocket;
    stlSocket      sNewSocket;
    stlSockAddr    sSvrSockAddr;
    stlChar        sMsgBuf[32];
    stlSize        sMsgLen;
    
    STL_TRY( stlInitialize() == STL_SUCCESS );
    sState = 1;

    STL_INIT_ERROR_STACK( &sErrorStack );

    /* IPv4 test */
    STL_TRY( stlCreateSocket( &sSvrSocket, 
                              STL_AF_UNIX,
                              SOCK_STREAM,
                              0,
                              &sErrorStack) == STL_SUCCESS );
    sState = 2;

    (void)stlRemoveFile( gSvrFile, &sErrorStack ); 

    stlMemset( &sSvrSockAddr, 0x00, STL_SIZEOF(sSvrSockAddr) );
    STL_TRY( stlSetSockAddr( &sSvrSockAddr,
                             STL_AF_UNIX,
                             NULL,
                             0,
                             gSvrFile,
                             &sErrorStack ) == STL_SUCCESS );

    STL_TRY( stlBind( sSvrSocket,
                      &sSvrSockAddr,
                      &sErrorStack) == STL_SUCCESS );

    STL_TRY( stlListen( sSvrSocket,
                        5,
                        &sErrorStack ) == STL_SUCCESS );

    STL_TRY( stlCreateThread( &sChildThread,
                              NULL,
                              threadUnixStream,
                              NULL,
                              &sErrorStack ) == STL_SUCCESS );

    STL_TRY( stlAccept( sSvrSocket,
                        &sNewSocket,
                        &sErrorStack ) == STL_SUCCESS );
    sState = 3;
    
    stlStrcpy( sMsgBuf, "Hwarang" );
    sMsgLen = stlStrlen(sMsgBuf);

    STL_TRY( stlSend( sNewSocket,
                      sMsgBuf,
                      &sMsgLen,
                      &sErrorStack ) == STL_SUCCESS );

    stlMemset( sMsgBuf, 0x00, 32 );
    sMsgLen = 32;
    STL_TRY( stlRecv( sNewSocket,
                      sMsgBuf,
                      &sMsgLen,
                      &sErrorStack ) == STL_SUCCESS );

    stlPrintf( "Returned message is '%s'\n", sMsgBuf );

    STL_TRY( stlJoinThread( &sChildThread,
                            &sThreadStatus,
                            &sErrorStack ) == STL_SUCCESS );
    STL_TRY( sThreadStatus == STL_SUCCESS );

    sState = 1;
    STL_TRY( stlCloseSocket( sNewSocket,
                             &sErrorStack ) == STL_SUCCESS );
    STL_TRY( stlCloseSocket( sSvrSocket,
                             &sErrorStack ) == STL_SUCCESS );
    /* IPv6 test */


    
    sState = 0;
    stlTerminate();

    return STL_SUCCESS;

    STL_FINISH;

    stlPrintf( "Server Failed with ErrorCode(%d)",
               stlGetLastErrorCode(&sErrorStack) );
    stlPrintf( "sState(%d)",sState );

    switch( sState )
    {
        case 3:
            stlCloseSocket( sNewSocket, &sErrorStack);
        case 2:
            stlCloseSocket( sSvrSocket, &sErrorStack);
        case 1:
            stlTerminate();
        default:
            break;
    }

    return STL_FAILURE;
}



stlInt32 main( stlInt32 aArgc, stlChar** aArgv )
{
    stlPrintf( "Start IPv4 Test...\n");
    testIPv4();

    stlPrintf( "Start IPv6 Test...\n");
    testIPv6();

    stlPrintf( "Start Unix Domain Test...\n");
    testUnix();

    stlPrintf( "Start Unix Domain Stream Test...\n");
    testUnixStream();

    return 0;
}
