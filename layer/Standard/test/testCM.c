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
#define STM_TEST_CLIENT_COUNT  1
#define STM_TEST_PACKET_SIZE  (32*1024) // 32KB

#define STM_IPV4               0
#define STM_IPV6               1
#define STM_UNIX               2

stlInt32   gPortNo = 12345;
stlChar    gUnixPath[] = "/tmp/test-unixpath";
stlInt32   gAF;
stlChar    gSendPacketBuffer[STM_TEST_PACKET_SIZE];
volatile stlBool    gStart = STL_FALSE;
volatile stlBool    gProcess = STL_FALSE;

void * STL_THREAD_FUNC workerThread( stlThread * aThread, void * aArg )
{
    stlErrorStack  sErrorStack;
    stlContext    *sWorkerContext;
    stlChar       *sWritePacketBuffer = gSendPacketBuffer;
    stlSize        sWritePacketLength = STM_TEST_PACKET_SIZE;
    stlChar       *sReadPacketBuffer;
    stlSize        sReadPacketLength;
    stlInt8        sEndian;
    stlInt32       sState = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );
    sWorkerContext = (stlContext *)aArg;

    TEST_VERIFY( stlAllocHeap( (void **)&sReadPacketBuffer,
                               STM_TEST_PACKET_SIZE,
                               &sErrorStack )
                 != STL_SUCCESS );

    TEST_VERIFY( stlGetPacketLength( sWorkerContext,
                                     &sEndian,
                                     &sReadPacketLength,
                                     &sErrorStack )
                 != STL_SUCCESS );

    TEST_VERIFY( stlReadPacket( sWorkerContext,
                                sReadPacketBuffer,
                                sReadPacketLength,
                                &sErrorStack )
                 != STL_SUCCESS );

    stlMemset( sWritePacketBuffer, 0x00, sWritePacketLength );
    TEST_VERIFY( stlWritePacket( sWorkerContext,
                                 sWritePacketBuffer,
                                 sWritePacketLength,
                                 &sErrorStack )
                 != STL_SUCCESS );

    stlFreeHeap( sReadPacketBuffer );
    TEST_VERIFY( stlFinalizeContext( sWorkerContext,
                                     &sErrorStack )
                 != STL_SUCCESS );

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

    stlFreeHeap( sReadPacketBuffer );
    stlExitThread( aThread, STL_FAILURE, &sErrorStack );

    PRINTFAIL;

    return NULL;
}

void * STL_THREAD_FUNC listenThread( stlThread * aThread, void * aArg )
{
    stlErrorStack  sErrorStack;
    stlThread      sWorkerThread[STM_TEST_CLIENT_COUNT];
    stlContext     sListenContext;
    stlContext     sNewContext;
    stlStatus      sThreadStatus;
    stlInt32       sState = 0;
    stlInt32       i;

    STL_INIT_ERROR_STACK( &sErrorStack );

    if(gAF == STM_IPV4)
    {
        TEST_VERIFY( stlInitializeContext( &sListenContext,
                                           STL_AF_INET,
                                           SOCK_STREAM,
                                           0,
                                           STL_TRUE,
                                           "127.0.0.1",
                                           gPortNo,
                                           &sErrorStack )
                     != STL_SUCCESS );
    }
    else if(gAF == STM_IPV6)
    {
        TEST_VERIFY( stlInitializeContext( &sListenContext,
                                           STL_AF_INET6,
                                           SOCK_STREAM,
                                           0,
                                           STL_TRUE,
                                           "::1",
                                           gPortNo,
                                           &sErrorStack )
                     != STL_SUCCESS );
    }
    else if(gAF == STM_UNIX)
    {
        TEST_VERIFY( stlInitializeContext( &sListenContext,
                                           STL_AF_UNIX,
                                           SOCK_STREAM,
                                           0,
                                           STL_TRUE,
                                           gUnixPath,
                                           gPortNo,
                                           &sErrorStack )
                     != STL_SUCCESS );
    }
    else
    {
        TEST_VERIFY( STL_FALSE );
    }

    gStart = STL_TRUE;

    for( i = 0; i < STM_TEST_CLIENT_COUNT; i ++ )
    {
        /* Poll */
        if( stlPollContext( &sListenContext,
                            &sNewContext,
                            1000000,
                            &sErrorStack )
            != STL_SUCCESS )
        {
            /* Poll fail */
            TEST_VERIFY( stlGetLastErrorCode( &sErrorStack )
                         != STL_ERRCODE_TIMEDOUT );
            stlPopError( &sErrorStack );
            i --;
        }
        else
        {   /* Poll success */
            /* Create server thread */
            TEST_VERIFY( stlCreateThread( &sWorkerThread[i],
                                          NULL,
                                          workerThread,
                                          (void *)&sNewContext,
                                          &sErrorStack ) != STL_SUCCESS );
        }
    }

    for( i = 0; i < STM_TEST_CLIENT_COUNT; i++ )
    {
        TEST_VERIFY( stlJoinThread( &sWorkerThread[i],
                                    &sThreadStatus,
                                    &sErrorStack ) != STL_SUCCESS );
        TEST_VERIFY( sThreadStatus != STL_SUCCESS );
    }

    STL_TRY( stlFinalizeContext( &sListenContext,
                                 &sErrorStack )
             == STL_SUCCESS );

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

void * STL_THREAD_FUNC clientThread( stlThread * aThread, void * aArg )
{
    stlErrorStack  sErrorStack;
    stlContext     sClientContext;
    stlChar       *sWritePacketBuffer = gSendPacketBuffer;
    stlSize        sWritePacketLength = STM_TEST_PACKET_SIZE;
    stlChar       *sReadPacketBuffer;
    stlSize        sReadPacketLength;
    stlInt8        sEndian;
    stlInt32       sState = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    TEST_VERIFY( stlAllocHeap( (void **)&sReadPacketBuffer,
                               STM_TEST_PACKET_SIZE,
                               &sErrorStack )
                 != STL_SUCCESS );

    while( gStart != STL_TRUE )
    {
        // Wait gStart is TRUE
    }

    if(gAF == STM_IPV4)
    {
        TEST_VERIFY( stlInitializeContext( &sClientContext,
                                           STL_AF_INET,
                                           STL_SOCK_STREAM,
                                           0,
                                           STL_FALSE,
                                           "127.0.0.1",
                                           gPortNo,
                                           &sErrorStack )
                     != STL_SUCCESS );
    }
    else if(gAF == STM_IPV6)
    {
        TEST_VERIFY( stlInitializeContext( &sClientContext,
                                           STL_AF_INET6,
                                           STL_SOCK_STREAM,
                                           0,
                                           STL_FALSE,
                                           "::1",
                                           gPortNo,
                                           &sErrorStack )
                     != STL_SUCCESS );
    }
    else if(gAF == STM_UNIX)
    {
        TEST_VERIFY( stlInitializeContext( &sClientContext,
                                           STL_AF_UNIX,
                                           STL_SOCK_STREAM,
                                           0,
                                           STL_FALSE,
                                           gUnixPath,
                                           gPortNo,
                                           &sErrorStack )
                     != STL_SUCCESS );
    }
    else
    {
        TEST_VERIFY( STL_FALSE );
    }

    TEST_VERIFY( stlConnectContext( &sClientContext,
                                    STL_INFINITE_TIME,
                                    &sErrorStack )
                 != STL_SUCCESS );
    sState = 1;

    stlMemset( sWritePacketBuffer, 0x00, sWritePacketLength );
    TEST_VERIFY( stlWritePacket( &sClientContext,
                                 sWritePacketBuffer,
                                 sWritePacketLength,
                                 &sErrorStack )
                 != STL_SUCCESS );

    TEST_VERIFY( stlGetPacketLength( &sClientContext,
                                     &sEndian,
                                     &sReadPacketLength,
                                     &sErrorStack )
                 != STL_SUCCESS );

    TEST_VERIFY( stlReadPacket( &sClientContext,
                                sReadPacketBuffer,
                                sReadPacketLength,
                                &sErrorStack )
                 != STL_SUCCESS );

    sState = 0;
    TEST_VERIFY( stlFinalizeContext( &sClientContext,
                                     &sErrorStack ) != STL_SUCCESS );

    stlFreeHeap( sReadPacketBuffer );
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
            stlFinalizeContext( &sClientContext, &sErrorStack);
        default:
            break;
    }

    stlFreeHeap( sReadPacketBuffer );
    stlExitThread( aThread, STL_FAILURE, &sErrorStack );

    PRINTFAIL;

    return NULL;
}

stlStatus stmDoWorkerJob( stlContext *aWorkerContext, stlErrorStack *aErrorStack )
{
    stlChar       *sWritePacketBuffer = gSendPacketBuffer;
    stlSize        sWritePacketLength = STM_TEST_PACKET_SIZE;
    stlChar       *sReadPacketBuffer;
    stlSize        sReadPacketLength;
    stlInt8        sEndian;
    stlInt32       sState = 0;

    TEST_VERIFY( stlAllocHeap( (void **)&sReadPacketBuffer,
                               STM_TEST_PACKET_SIZE,
                               aErrorStack )
                 != STL_SUCCESS );

    TEST_VERIFY( stlGetPacketLength( aWorkerContext,
                                     &sEndian,
                                     &sReadPacketLength,
                                     aErrorStack )
                 != STL_SUCCESS );


    TEST_VERIFY( stlReadPacket( aWorkerContext,
                                sReadPacketBuffer,
                                sReadPacketLength,
                                aErrorStack )
                 != STL_SUCCESS );
    TEST_VERIFY( stlStrncmp( sReadPacketBuffer, gSendPacketBuffer, STM_TEST_PACKET_SIZE )
                 != 0 );
    stlFreeHeap( sReadPacketBuffer );

    stlMemset( sWritePacketBuffer, 0x00, sWritePacketLength );
    TEST_VERIFY( stlWritePacket( aWorkerContext,
                                 sWritePacketBuffer,
                                 sWritePacketLength,
                                 aErrorStack )
                 != STL_SUCCESS );

    TEST_VERIFY( stlFinalizeContext( aWorkerContext,
                                     aErrorStack )
                 != STL_SUCCESS );

    PRINTPASS;

    return STL_SUCCESS;

    STL_FINISH;

    PRINTERR( aErrorStack );

    switch( sState )
    {
        case 1:
            break;
        default:
            break;
    }

    stlFreeHeap( sReadPacketBuffer );

    PRINTFAIL;

    return STL_FAILURE;
}

void * STL_THREAD_FUNC listenThread4Process( stlThread * aThread, void * aArg )
{
    stlErrorStack sErrorStack;
    stlProc       sWorkerProc[STM_TEST_CLIENT_COUNT];
    stlInt32      sExitCode[STM_TEST_CLIENT_COUNT];
    stlBool       sIsChild;
    stlContext    sListenContext;
    stlContext    sNewContext;
    stlContext    sWorkerContext;
    stlContext    sPairContexts[STM_TEST_CLIENT_COUNT][2];
    stlInt32      sState = 0;
    stlInt64      i;

    STL_INIT_ERROR_STACK( &sErrorStack );

    if(gAF == STM_IPV4)
    {
        TEST_VERIFY( stlInitializeContext( &sListenContext,
                                           STL_AF_INET,
                                           STL_SOCK_STREAM,
                                           0,
                                           STL_TRUE,
                                           "127.0.0.1",
                                           gPortNo,
                                           &sErrorStack )
                     != STL_SUCCESS );
    }
    else if(gAF == STM_IPV6)
    {
        TEST_VERIFY( stlInitializeContext( &sListenContext,
                                           STL_AF_INET6,
                                           STL_SOCK_STREAM,
                                           0,
                                           STL_TRUE,
                                           "::1",
                                           gPortNo,
                                           &sErrorStack )
                     != STL_SUCCESS );
    }
    else if(gAF == STM_UNIX)
    {
        TEST_VERIFY( stlInitializeContext( &sListenContext,
                                           STL_AF_UNIX,
                                           STL_SOCK_STREAM,
                                           0,
                                           STL_TRUE,
                                           gUnixPath,
                                           gPortNo,
                                           &sErrorStack )
                     != STL_SUCCESS );
    }
    else
    {
        TEST_VERIFY( STL_FALSE );
    }

    gStart = STL_TRUE;

    for( i = 0; i < STM_TEST_CLIENT_COUNT; i ++ )
    {
        /* Poll */
        if( stlPollContext( &sListenContext,
                            &sNewContext,
                            1000000,
                            &sErrorStack )
            != STL_SUCCESS )
        {
            /* Poll fail */
            TEST_VERIFY( stlGetLastErrorCode( &sErrorStack )
                         != STL_ERRCODE_TIMEDOUT );
            stlPopError( &sErrorStack );
            i --;
        }
        else
        {   /* Poll success */
            /* Create server process */
            TEST_VERIFY( stlCreateContextPair( STL_AF_UNIX,
                                               STL_SOCK_STREAM,
                                               0,
                                               sPairContexts[i],
                                               &sErrorStack ) != STL_SUCCESS );

            TEST_VERIFY( stlForkProc( &sWorkerProc[i],
                                      &sIsChild,
                                      &sErrorStack ) != STL_SUCCESS );
            if( sIsChild != STL_TRUE )
            {
                /* Parent process */
                TEST_VERIFY( stlFinalizeContext( &sPairContexts[i][1], &sErrorStack) != STL_SUCCESS );
                TEST_VERIFY( stlSendContext( &sPairContexts[i][0],
                                             NULL,
                                             &sNewContext,
                                             NULL,
                                             0,
                                             &sErrorStack )
                             != STL_SUCCESS );

                TEST_VERIFY( stlFinalizeContext( &sNewContext,
                                                 &sErrorStack )
                             != STL_SUCCESS );
            }
            else
            {
                /* Child process */
                TEST_VERIFY( stlFinalizeContext( &sPairContexts[i][0], &sErrorStack) != STL_SUCCESS );
                TEST_VERIFY( stlRecvContext( &sPairContexts[i][1],
                                             &sWorkerContext,
                                             NULL,
                                             NULL,
                                             &sErrorStack )
                             != STL_SUCCESS );

                TEST_VERIFY( stmDoWorkerJob( &sWorkerContext, &sErrorStack )
                             != STL_SUCCESS );

                TEST_VERIFY( stlFinalizeContext( &sWorkerContext,
                                                 &sErrorStack )
                             != STL_SUCCESS );

                TEST_VERIFY( stlExitProc( 0 ) != STL_SUCCESS );
            }
        }
    }

    for( i = 0; i < STM_TEST_CLIENT_COUNT; i ++ )
    {
        TEST_VERIFY( stlFinalizeContext( &sPairContexts[i][0], &sErrorStack) != STL_SUCCESS );
        TEST_VERIFY( stlWaitProc( &sWorkerProc[i], &sExitCode[i], &sErrorStack ) != STL_SUCCESS );
    }

    STL_TRY( stlFinalizeContext( &sListenContext,
                                 &sErrorStack )
             == STL_SUCCESS );

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

stlStatus testCommMgr()
{
    stlInt32       sState = 0;
    stlErrorStack  sErrorStack;
    stlStatus      sThreadStatus;
    stlThread      sListenThread;
    stlThread      sClientThread[STM_TEST_CLIENT_COUNT];
    stlInt32       i;

    STL_INIT_ERROR_STACK( &sErrorStack );

    /*
     * 1. Create client thread
     */
    if(gProcess == STL_TRUE)
    {
        TEST_VERIFY( stlCreateThread( &sListenThread,
                                      NULL,
                                      listenThread4Process,
                                      NULL,
                                      &sErrorStack ) != STL_SUCCESS );
    }
    else
    {
        TEST_VERIFY( stlCreateThread( &sListenThread,
                                      NULL,
                                      listenThread,
                                      NULL,
                                      &sErrorStack ) != STL_SUCCESS );
    }
    
    /*
     * 2. Create client thread
     */
    for( i = 0; i < STM_TEST_CLIENT_COUNT; i++ )
    {
        if(i != 0)
        {
            stlSleep(100000);
        }

        TEST_VERIFY( stlCreateThread( &sClientThread[i],
                                      NULL,
                                      clientThread,
                                      NULL,
                                      &sErrorStack ) != STL_SUCCESS );
    }

    for( i = 0; i < STM_TEST_CLIENT_COUNT; i++ )
    {
        TEST_VERIFY( stlJoinThread( &sClientThread[i],
                                    &sThreadStatus,
                                    &sErrorStack ) != STL_SUCCESS );
        TEST_VERIFY( sThreadStatus != STL_SUCCESS );
    }

    TEST_VERIFY( stlJoinThread( &sListenThread,
                                &sThreadStatus,
                                &sErrorStack ) != STL_SUCCESS );
    TEST_VERIFY( sThreadStatus != STL_SUCCESS );

    sState = 0;

    PRINTPASS;

    return STL_SUCCESS;

    STL_FINISH;

    PRINTERR( &sErrorStack );

    return STL_FAILURE;

}

stlInt32 main( stlInt32 aArgc, stlChar** aArgv )
{
    stlInt32       sState = 0;
    stlInt64       i;

    STL_TRY( stlInitialize() == STL_SUCCESS );
    sState = 1;
    for( i = 0; i < STM_TEST_PACKET_SIZE; i ++ )
    {
        gSendPacketBuffer[i] = 'A' + ( i / 26 );
    }


    stlPrintf( "1. Test communication manager : IPv4\n" );
    gAF = STM_IPV4;
    gStart = STL_FALSE;
    gProcess = STL_FALSE;
    STL_TRY( testCommMgr() == STL_SUCCESS );

    stlPrintf( "2. Test communication manager : IPv6\n" );
    gAF = STM_IPV6;
    gStart = STL_FALSE;
    gProcess = STL_FALSE;
    STL_TRY( testCommMgr() == STL_SUCCESS );

    stlPrintf( "3. Test communication manager : Unix domain\n" );
    gAF = STM_UNIX;
    gStart = STL_FALSE;
    gProcess = STL_FALSE;
    STL_TRY( testCommMgr() == STL_SUCCESS );

    stlPrintf( "4. Test passing FD :\n" );
    gAF = STM_IPV4;
    gStart = STL_FALSE;
    gProcess = STL_TRUE;
    STL_TRY( testCommMgr() == STL_SUCCESS );

    sState = 0;
    stlTerminate();

    return STL_SUCCESS;

    STL_FINISH;

    PRINTFAIL;

    return STL_FAILURE;
}
