#include <stl.h>

#define PRINTPASS stlPrintf( "  => PASS\n" );
#define PRINTFAIL stlPrintf( "  => FAIL\n" );
#define PRINTERR( aErrorStack )                                 \
    do                                                          \
    {                                                           \
        stlPrintf( "ERRNO: %d, ERRCODE: %d, ERRSTRING: %s\n",   \
                   stlGetLastSystemErrorCode( aErrorStack ),    \
                   stlGetLastErrorCode( aErrorStack ),          \
                   stlGetLastErrorMessage( aErrorStack ) );     \
    } while(0)

#define TEST_VERIFY( aExpression )                                  \
    do                                                              \
    {                                                               \
        if( aExpression )                                           \
        {                                                           \
            stlPrintf( "ERRORIGIN: %s(%d)\n", __FILE__, __LINE__ ); \
            goto STL_FINISH_LABEL;                                  \
        }                                                           \
    } while( 0 )

#define MAX_POLL_SIZE 10
#define CLIENT_COUNT  1

stlInt32   gPortNo = 12345;
stlPollSet gPollSet;
stlPollFd  gPollFd[MAX_POLL_SIZE];
stlInt32   gExitCount;
stlBool    gStart;

void * STL_THREAD_FUNC clientThread( stlThread * aThread, void * aArg )
{
    stlErrorStack sErrorStack;
    stlSocket     sClientSocket;
    stlSockAddr   sClientSockAddr;
    stlChar       sMsgBuf[32];
    stlSize       sMsgLen = 32;
    stlInt32      sState = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    while( gStart == STL_FALSE )
    {
        stlYieldThread();
    };
    
    TEST_VERIFY( stlCreateSocket( &sClientSocket, 
                                  STL_AF_INET,
                                  SOCK_STREAM,
                                  STL_PROTO_TCP,
                                  &sErrorStack) != STL_SUCCESS );

    TEST_VERIFY( stlSetSockAddr( &sClientSockAddr,
                                 STL_AF_INET,
                                 "127.0.0.1",
                                 gPortNo,
                                 NULL,
                                 &sErrorStack ) != STL_SUCCESS );

    TEST_VERIFY( stlConnect( sClientSocket,
                             &sClientSockAddr,
                             STL_INFINITE_TIME,
                             &sErrorStack ) != STL_SUCCESS );
    sState = 1;

    stlStrcpy( sMsgBuf, "Hwarang" );
    sMsgLen = stlStrlen( sMsgBuf );
    TEST_VERIFY( stlSend( sClientSocket,
                          sMsgBuf,
                          &sMsgLen,
                          &sErrorStack ) != STL_SUCCESS );

    stlMemset( sMsgBuf, 0x00, 32 );
    TEST_VERIFY( stlRecv( sClientSocket,
                          sMsgBuf,
                          &sMsgLen,
                          &sErrorStack ) != STL_SUCCESS );
    
    stlPrintf( "Client received message '%s'\n", sMsgBuf );

    sState = 0;
    TEST_VERIFY( stlCloseSocket( sClientSocket,
                                 &sErrorStack ) != STL_SUCCESS );

    gExitCount += 1;
    TEST_VERIFY( stlExitThread( aThread,
                                STL_SUCCESS,
                                &sErrorStack ) != STL_SUCCESS );
    
    PRINTPASS;
    
    return NULL;

    STL_FINISH;

    PRINTERR( &sErrorStack );
    
    switch( sState )
    {
        case 1:
            stlCloseSocket( sClientSocket, &sErrorStack);
        default:
            break;
    }

    gExitCount += 1;
    stlExitThread( aThread, STL_FAILURE, &sErrorStack );
   
    PRINTFAIL;

    return NULL;
}

void * STL_THREAD_FUNC serverThread( stlThread * aThread, void * aArg )
{
    stlErrorStack sErrorStack;
    stlChar       sMsgBuf[32];
    stlSize       sMsgLen = 32;
    stlInt32      sState = 0;
    stlInt32      sSigFdNum;
    stlInt32      i;

    STL_INIT_ERROR_STACK( &sErrorStack );

    while( gExitCount < CLIENT_COUNT )
    {
        if( stlPollPollSet( &gPollSet,
                            &sSigFdNum,
                            1000,
                            &sErrorStack ) != STL_SUCCESS )
        {
            TEST_VERIFY( stlGetLastErrorCode( &sErrorStack )
                         != STL_ERRCODE_TIMEDOUT );
            stlPopError( &sErrorStack );
        }

        for( i = 0; (i < gPollSet.mCurPollFdNum) && (sSigFdNum > 0); i++ )
        {
            if( (gPollSet.mPollFd[i].mRetEvents & STL_POLLIN) != 0 )
            {
                stlMemset( sMsgBuf, 0x00, 32 );
                if( stlRecv( gPollSet.mPollFd[i].mSocketHandle,
                             sMsgBuf,
                             &sMsgLen,
                             &sErrorStack ) != STL_SUCCESS )
                {
                    TEST_VERIFY( stlGetLastErrorCode( &sErrorStack )
                                 != STL_ERRCODE_EOF );

                    TEST_VERIFY( stlRemoveSockPollSet( &gPollSet,
                                                       &gPollSet.mPollFd[i],
                                                       &sErrorStack )
                                 != STL_SUCCESS );
                }
                else
                {
                    stlPrintf( "Server received message '%s'\n", sMsgBuf );
            
                    stlStrcpy( sMsgBuf, "Dambae" );
                    sMsgLen = stlStrlen(sMsgBuf);
                    TEST_VERIFY( stlSend( gPollSet.mPollFd[i].mSocketHandle,
                                          sMsgBuf,
                                          &sMsgLen,
                                          &sErrorStack ) != STL_SUCCESS );
                }
            }
        }
    }

    TEST_VERIFY( stlExitThread( aThread,
                                STL_SUCCESS,
                                &sErrorStack ) != STL_SUCCESS );
    
    PRINTPASS;
    
    return NULL;

    STL_FINISH;
   
    PRINTERR( &sErrorStack );
    
    switch( sState )
    {
        case 1:
            break;
        default:
            break;
    }

    stlExitThread( aThread, STL_FAILURE, &sErrorStack );
   
    PRINTFAIL;

    return NULL;
}

stlStatus testMultiplexing()
{
    stlInt32       sState = 0;
    stlErrorStack  sErrorStack;
    stlStatus      sThreadStatus;
    stlThread      sServerThread;
    stlThread      sClientThread[CLIENT_COUNT];
    stlSocket      sServerSocket;
    stlSocket      sNewSocket;
    stlSockAddr    sServerSockAddr;
    stlPollFd      sClientPollFd;
    stlPollFd      sServerPollFd;
    stlInt32       sSigFdNum;
    stlInt32       i;
    
    gExitCount = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    STL_TRY( stlInitPollSet( &gPollSet,
                             gPollFd,
                             MAX_POLL_SIZE,
                             &sErrorStack )
             == STL_SUCCESS );
    
    TEST_VERIFY( stlCreateSocket( &sServerSocket, 
                                  STL_AF_INET,
                                  SOCK_STREAM,
                                  STL_PROTO_TCP,
                                  &sErrorStack ) != STL_SUCCESS );
    sState = 1;

    stlMemset( &sServerSockAddr, 0x00, STL_SIZEOF(sServerSockAddr) );
    TEST_VERIFY( stlSetSockAddr( &sServerSockAddr,
                                 STL_AF_INET,
                                 "127.0.0.1",
                                 gPortNo,
                                 NULL,
                                 &sErrorStack ) != STL_SUCCESS );

    TEST_VERIFY( stlBind( sServerSocket,
                          &sServerSockAddr,
                          &sErrorStack ) != STL_SUCCESS );

    TEST_VERIFY( stlListen( sServerSocket,
                            3,
                            &sErrorStack ) != STL_SUCCESS );

    for( i = 0; i < CLIENT_COUNT; i++ )
    {
        TEST_VERIFY( stlCreateThread( &sClientThread[i],
                                      NULL,
                                      clientThread,
                                      NULL,
                                      &sErrorStack ) != STL_SUCCESS );
    }
    TEST_VERIFY( stlCreateThread( &sServerThread,
                                  NULL,
                                  serverThread,
                                  NULL,
                                  &sErrorStack ) != STL_SUCCESS );
    
    sServerPollFd.mSocketHandle = sServerSocket;
    sServerPollFd.mReqEvents = STL_POLLIN | STL_POLLERR | STL_POLLOUT;

    while( gExitCount < CLIENT_COUNT )
    {
        if( stlPoll( &sServerPollFd,
                     1,
                     &sSigFdNum,
                     1000,
                     &sErrorStack ) != STL_SUCCESS )
        {
            TEST_VERIFY( stlGetLastErrorCode( &sErrorStack )
                         != STL_ERRCODE_TIMEDOUT );
            stlPopError( &sErrorStack );
        }

        if( (sServerPollFd.mRetEvents & STL_POLLIN) != 0 )
        {
            TEST_VERIFY( stlAccept( sServerSocket,
                                    &sNewSocket,
                                    &sErrorStack ) != STL_SUCCESS );

            sClientPollFd.mSocketHandle = sNewSocket;
            sClientPollFd.mReqEvents = STL_POLLIN | STL_POLLERR;
        
            TEST_VERIFY( stlAddSockPollSet( &gPollSet,
                                            &sClientPollFd,
                                            &sErrorStack ) != STL_SUCCESS );
        }
        else
        {
            gStart = STL_TRUE;
        }
    }

    for( i = 0; i < CLIENT_COUNT; i++ )
    {
        TEST_VERIFY( stlJoinThread( &sClientThread[i],
                                    &sThreadStatus,
                                    &sErrorStack ) != STL_SUCCESS );
    }
    TEST_VERIFY( sThreadStatus != STL_SUCCESS );
    
    TEST_VERIFY( stlJoinThread( &sServerThread,
                                &sThreadStatus,
                                &sErrorStack ) != STL_SUCCESS );
    TEST_VERIFY( sThreadStatus != STL_SUCCESS );

    sState = 0;
    TEST_VERIFY( stlCloseSocket( sServerSocket,
                                 &sErrorStack ) != STL_SUCCESS );

    PRINTPASS;
    
    return STL_SUCCESS;

    STL_FINISH;
   
    PRINTERR( &sErrorStack );

    return STL_FAILURE;
    
}

stlInt32 main( stlInt32 aArgc, stlChar** aArgv )
{
    stlInt32       sState = 0;
    stlPollSet     sPollSet;
    stlPollFd      sBufferPollFd[1];
    stlPollFd      sPollFd;
    stlErrorStack  sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    STL_TRY( stlInitialize() == STL_SUCCESS );
    sState = 1;

    /*
     * 1.Test stlAddSockPollSet
     */
    stlPrintf( "1. Test stlAddSockPollSet\n" );
    TEST_VERIFY( stlInitPollSet( &sPollSet,
                                 sBufferPollFd,
                                 0,
                                 &sErrorStack ) != STL_SUCCESS );
    TEST_VERIFY( stlAddSockPollSet( &gPollSet,
                                    &sPollFd,
                                    &sErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( stlGetLastErrorCode( &sErrorStack )
                 != STL_ERRCODE_OUT_OF_MEMORY );
    PRINTPASS;
                                    
    /*
     * 2. Test stlRemoveSockPollSet
     */
    stlPrintf( "2. Test stlRemoveSockPollSet\n" );
    
    TEST_VERIFY( stlRemoveSockPollSet( &gPollSet,
                                       &sPollFd,
                                       &sErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( stlGetLastErrorCode( &sErrorStack )
                 != STL_ERRCODE_NO_ENTRY );
    PRINTPASS;
                                    
    /*
     * 3. Test stlPoll & stlPollPollSet
     */
    stlPrintf( "3. Test stlPoll & stlPollPollSet\n" );
    STL_TRY( testMultiplexing() == STL_SUCCESS );
    
    sState = 0;
    stlTerminate();

    return STL_SUCCESS;

    STL_FINISH;
    
    PRINTERR( &sErrorStack );
    PRINTFAIL;
    
    return STL_FAILURE;
}
