#include <stl.h>
#include <ifaddrs.h>
#include <netdb.h>

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
        if( !aExpression )                                           \
        {                                                           \
            stlPrintf( "ERRORIGIN: %s(%d)\n", __FILE__, __LINE__ ); \
            goto STL_FINISH_LABEL;                                  \
        }                                                           \
    } while( 0 )


#define TEST_PORT       10340

stlChar         gBuff[16];
stlChar       * gIbIp = gBuff;

stlStatus getIbIp( stlChar       ** aIpAddr,
                   stlErrorStack  * aErrorStack );

stlStatus myRcSendWait( stlRcHandle   * aHandle,
                        stlChar       * aStartPtr,
                        stlSize         aDataLen,
                        stlErrorStack * aErrorStack );

stlStatus myRcRecvWait( stlRcHandle     aHandle,
                        stlChar       * aStartPtr,
                        stlSize         aBufferSize,
                        stlErrorStack * aErrorStack);


void * STL_THREAD_FUNC serverThreadSendRecv( stlThread * aThread, void * aArg )
{
    stlRcHandle     sHandle; 
    stlErrorStack   sErrorStack;
    stlChar         sBuf[4096];
    stlChar       * sLargeBuf;
    stlSize         sLen;
    stlInt32        i;
    stlInt32        sRc;

    STL_INIT_ERROR_STACK( &sErrorStack );

    STL_TRY( stlRcCreateForServer( &sHandle,
                                   gIbIp,
                                   TEST_PORT,
                                   4096,
                                   1023,
                                   4096,
                                   1024 * 3,
                                   STL_SET_SEC_TIME( 5 ),
                                   &sErrorStack ) == STL_SUCCESS );

    sLen = STL_SIZEOF(sBuf);
    STL_TRY( stlRcRecv( sHandle, sBuf, &sLen, &sErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( sLen == STL_SIZEOF(sBuf) );

    for( i = 0; i < 4096; i++ )
    {
        sBuf[i] ^= 0x37;
    }

    sLen = STL_SIZEOF(sBuf);
    STL_TRY( stlRcSend( sHandle, sBuf, &sLen, &sErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( sLen == STL_SIZEOF(sBuf) );

    /* msg size 1 ~ 300Mbyte  */

    sRc = stlAllocHeap( (void**)&sLargeBuf, 300 * 1024 * 1024, &sErrorStack );
    STL_TRY( sRc == STL_SUCCESS );

    for ( sLen = 8192; sLen <= 300 * 1024 * 1024; sLen *= 2 )
    {
        STL_TRY( myRcRecvWait( sHandle, sLargeBuf, sLen, &sErrorStack ) == STL_SUCCESS );
    }






    /* end */
    stlSleep( STL_SET_MSEC_TIME( 300 ) );

    STL_TRY( stlRcClose( sHandle, &sErrorStack ) == STL_SUCCESS );


    PRINTPASS;
    
    return NULL;

    STL_FINISH;
   
    PRINTERR( &sErrorStack );
    
    stlExitThread( aThread, STL_FAILURE, &sErrorStack );
   
    PRINTFAIL;

    return NULL;
}


stlStatus testSendRecv( stlErrorStack  * aErrorStack )
{        
    stlRcHandle     sHandle; 
    stlThread       sServerThread;
    stlStatus       sReturnStatus;
    stlChar         sSendBuf[4096];
    stlChar         sRecvBuf[4096];
    stlChar       * sLargeBuf;
    stlInt32        i;
    stlInt32        sRc;
    stlSize         sLen;

    STL_TRY( stlCreateThread( &sServerThread,
                              NULL,
                              serverThreadSendRecv,
                              NULL,
                              aErrorStack ) == STL_SUCCESS );

    stlSleep( STL_SET_MSEC_TIME( 300 ) );

    STL_TRY( stlRcCreateForClient( &sHandle,
                                   gIbIp,
                                   gIbIp,
                                   TEST_PORT,
                                   4096,
                                   1024,
                                   4096,
                                   1024*2,
                                   aErrorStack ) == STL_SUCCESS );

    for( i = 0; i < 4096; i++ )
    {
        sSendBuf[i] = stlRand() % 127;
    }

    sLen = STL_SIZEOF(sSendBuf);
    STL_TRY( stlRcSend( sHandle, sSendBuf, &sLen, aErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( sLen == STL_SIZEOF(sSendBuf) );

    sLen = STL_SIZEOF(sRecvBuf);
    STL_TRY( stlRcRecv( sHandle, sRecvBuf, &sLen, aErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( sLen == STL_SIZEOF(sRecvBuf) );

    for( i = 0; i < 4096; i++ )
    {
        TEST_VERIFY( ( sRecvBuf[i] ^ 0x37 ) == sSendBuf[i] );
    }

    /* msg size 1 ~ 300Mbyte  */
    sRc = stlAllocHeap( (void**)&sLargeBuf, 300 * 1024 * 1024, aErrorStack );
    STL_TRY( sRc == STL_SUCCESS );

    for ( sLen = 8192; sLen <= 300 * 1024 * 1024; sLen *= 2 )
    {
        STL_TRY( myRcSendWait( sHandle, sLargeBuf, sLen, aErrorStack ) == STL_SUCCESS );
    }




    /* end */
    STL_TRY( stlJoinThread( &sServerThread, &sReturnStatus, aErrorStack ) == STL_SUCCESS );
    stlSleep( STL_SET_MSEC_TIME( 300 ) );

    STL_TRY( stlRcClose( sHandle, aErrorStack ) == STL_SUCCESS );

    
 
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

void * STL_THREAD_FUNC serverThreadParameter( stlThread * aThread, void * aArg )
{
    stlRcHandle     sHandle; 
    stlErrorStack   sErrorStack;
    stlChar         sBuf[4096];
    stlSize         sLen;
    stlInt32        sRc;
    stlInt32        i;

    STL_INIT_ERROR_STACK( &sErrorStack );

    /* window size wrong */
    sRc = stlRcCreateForServer( &sHandle,
                                gIbIp,
                                TEST_PORT,
                                4096,
                                1024,
                                4096,
                                1024,
                                STL_SET_SEC_TIME( 5 ),
                                &sErrorStack );
    TEST_VERIFY( sRc == STL_FAILURE && 
                 stlGetLastErrorCode( &sErrorStack ) == STL_ERRCODE_MISMATCH );

    /* Create timeout */
    sRc = stlRcCreateForServer( &sHandle,
                                gIbIp,
                                TEST_PORT-1,
                                4096,
                                1024,
                                4096,
                                1024,
                                STL_SET_SEC_TIME( 1 ),
                                &sErrorStack );
    TEST_VERIFY( sRc == STL_FAILURE && 
                 stlGetLastErrorCode( &sErrorStack ) == STL_ERRCODE_TIMEDOUT );

    /* data test */
    sRc = stlRcCreateForServer( &sHandle,
                                gIbIp,
                                TEST_PORT,
                                4096,
                                1024,
                                4096,
                                1024,
                                STL_SET_SEC_TIME( 5 ),
                                &sErrorStack );
    TEST_VERIFY( sRc == STL_SUCCESS );

    /* send [4096], recv[4096-1] => STL_ERRCODE_IBV_VERBS_FAILURE */
    sLen = STL_SIZEOF(sBuf)-1;
    sRc = stlRcRecv( sHandle, sBuf, &sLen, &sErrorStack );
    TEST_VERIFY( sRc == STL_FAILURE && 
                 stlGetLastErrorCode( &sErrorStack ) == STL_ERRCODE_IBV_VERBS_FAILURE );

    /* send [4096-100], recv[4096] => STL_SUCCESS */
    sLen = STL_SIZEOF(sBuf);
    sRc = stlRcRecv( sHandle, sBuf, &sLen, &sErrorStack );
    TEST_VERIFY( sRc == STL_SUCCESS );
    TEST_VERIFY( sLen == STL_SIZEOF(sBuf)-100 );

    /* recv timeout */
    STL_TRY( stlRcSetSendTimeout( sHandle, 
                                  STL_SET_SEC_TIME( 10 ), 
                                  STL_SET_SEC_TIME( 5 ), 
                                  &sErrorStack ) == STL_SUCCESS );
    STL_TRY( stlRcSetRecvTimeout( sHandle, 
                                  STL_SET_SEC_TIME( 10 ), 
                                  STL_SET_SEC_TIME( 5 ), 
                                  &sErrorStack ) == STL_SUCCESS );
    sLen = STL_SIZEOF(sBuf);
    stlPrintf("recv timeout 10초 (5초 busywait) cpu 확인..\n");
    sRc = stlRcRecv( sHandle, sBuf, &sLen, &sErrorStack );
    TEST_VERIFY( sRc == STL_FAILURE && 
                 stlGetLastErrorCode( &sErrorStack ) == STL_ERRCODE_TIMEDOUT );

    for( i = 0; i < 2048+1; i++ )
    {
        if ( i == 2048  )
        {
            stlPrintf("send timeout 10초 (5초 busywait) cpu 확인..\n");
        }
        sLen = STL_SIZEOF(sBuf);
        sRc = stlRcSend( sHandle, sBuf, &sLen, &sErrorStack );
        if ( i < 2048  )
        {
            TEST_VERIFY( sRc == STL_SUCCESS );
            TEST_VERIFY( sLen == STL_SIZEOF(sBuf) );
        }
        else
        {
            TEST_VERIFY( sRc == STL_FAILURE && 
                         stlGetLastErrorCode( &sErrorStack ) == STL_ERRCODE_TIMEDOUT );
            break;
        }
    }

    /* end */
    stlSleep( STL_SET_MSEC_TIME( 300 ) );

    STL_TRY( stlRcClose( sHandle, &sErrorStack ) == STL_SUCCESS );


    PRINTPASS;
    
    return NULL;

    STL_FINISH;
   
    PRINTERR( &sErrorStack );
    
    stlExitThread( aThread, STL_FAILURE, &sErrorStack );
   
    PRINTFAIL;

    return NULL;
}


stlStatus testParameter( stlErrorStack  * aErrorStack )
{        
    stlRcHandle     sHandle; 
    stlThread       sServerThread;
    stlStatus       sReturnStatus;
    stlChar         sBuf[4096];
    stlInt32        sRc;
    stlSize         sLen;
    

    STL_TRY( stlCreateThread( &sServerThread,
                              NULL,
                              serverThreadParameter,
                              NULL,
                              aErrorStack ) == STL_SUCCESS );

    stlSleep( STL_SET_MSEC_TIME( 300 ) );

    /* window size wrong */
    sRc = stlRcCreateForClient( &sHandle,
                                gIbIp,
                                gIbIp,
                                TEST_PORT,
                                4096+1,
                                1024,
                                4096,
                                1024,
                                aErrorStack );
    TEST_VERIFY( sRc == STL_FAILURE && 
                 stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_MISMATCH );

    /* Create(connect) 오류  */
    sRc = stlRcCreateForClient( &sHandle,
                                gIbIp,
                                gIbIp,
                                TEST_PORT+1,
                                4096,
                                1024,
                                4096,
                                1024,
                                aErrorStack );
    TEST_VERIFY( sRc == STL_FAILURE && 
                 stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_CONNECT );
    stlSleep( STL_SET_SEC_TIME( 1 ) );

    /* data test */
    sRc = stlRcCreateForClient( &sHandle,
                                gIbIp,
                                gIbIp,
                                TEST_PORT,
                                4096,
                                1024,
                                4096,
                                1024,
                                aErrorStack );
    TEST_VERIFY( sRc == STL_SUCCESS );

    /* send [4096], recv[4096-1] => STL_ERRCODE_IBV_VERBS_FAILURE */
    sLen = STL_SIZEOF(sBuf);
    STL_TRY( stlRcSend( sHandle, sBuf, &sLen, aErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( sLen == STL_SIZEOF(sBuf) );

    /* send [4096-100], recv[4096] => STL_SUCCESS */
    sLen = STL_SIZEOF(sBuf) - 100;
    STL_TRY( stlRcSend( sHandle, sBuf, &sLen, aErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( sLen == STL_SIZEOF(sBuf) );

    /* recv timeout */
    /* do nothing */

    /* end */
    STL_TRY( stlJoinThread( &sServerThread, &sReturnStatus, aErrorStack ) == STL_SUCCESS );
    stlSleep( STL_SET_MSEC_TIME( 300 ) );

    STL_TRY( stlRcClose( sHandle, aErrorStack ) == STL_SUCCESS );

    
 
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

void * STL_THREAD_FUNC serverThreadNegative( stlThread * aThread, void * aArg )
{
    stlRcHandle     sHandle; 
    stlErrorStack   sErrorStack;
    stlChar         sBuf[4096];
    stlSize         sLen;
    stlInt32        sRc;
    stlInt32        i;

    STL_INIT_ERROR_STACK( &sErrorStack );

    STL_TRY( stlRcCreateForServer( &sHandle,
                                   gIbIp,
                                   TEST_PORT,
                                   4096,
                                   1024,
                                   4096,
                                   1024,
                                   STL_SET_SEC_TIME( 5 ),
                                   &sErrorStack ) == STL_SUCCESS );

    /* client가 send하고 있는데 server가 죽는 경우 */
    for( i = 0; i < 100; i++ )
    {
        sLen = STL_SIZEOF(sBuf);
        sRc = stlRcRecv( sHandle, sBuf, &sLen, &sErrorStack );
        TEST_VERIFY( sRc == STL_SUCCESS );
    }

    stlSleep( STL_SET_SEC_TIME( 3 ) );

    /* end */
    stlSleep( STL_SET_MSEC_TIME( 300 ) );

    STL_TRY( stlRcClose( sHandle, &sErrorStack ) == STL_SUCCESS );


    PRINTPASS;
    
    return NULL;

    STL_FINISH;
   
    PRINTERR( &sErrorStack );
    
    stlExitThread( aThread, STL_FAILURE, &sErrorStack );
   
    PRINTFAIL;

    return NULL;
}



stlStatus testNegative( stlErrorStack  * aErrorStack )
{        
    stlRcHandle     sHandle; 
    stlThread       sServerThread;
    stlStatus       sReturnStatus;
    stlChar         sBuf[4096];
    stlSize         sLen;
    stlInt32        sRc;
    stlInt32        i;

    STL_TRY( stlCreateThread( &sServerThread,
                              NULL,
                              serverThreadNegative,
                              NULL,
                              aErrorStack ) == STL_SUCCESS );

    stlSleep( STL_SET_MSEC_TIME( 300 ) );

    STL_TRY( stlRcCreateForClient( &sHandle,
                                   gIbIp,
                                   gIbIp,
                                   TEST_PORT,
                                   4096,
                                   1024,
                                   4096,
                                   1024,
                                   aErrorStack ) == STL_SUCCESS );

    /* client가 send하고 있는데 server가 죽는 경우 */
    STL_TRY( stlRcSetSendTimeout( sHandle, 
                                  STL_SET_SEC_TIME( 10 ), 
                                  STL_SET_SEC_TIME( 5 ), 
                                  aErrorStack ) == STL_SUCCESS );

    for( i = 0; ; i++ )
    {
        sLen = STL_SIZEOF(sBuf);
        sRc =  stlRcSend( sHandle, sBuf, &sLen, aErrorStack );
        if ( sRc == STL_FAILURE )
        {
            stlPrintf("send하고 있는데 server가 죽는 경우 [%d][%d]\n", 
                      i,
                      stlGetLastErrorCode( aErrorStack ) );
            break;
        }
    }

    /* end */
    STL_TRY( stlJoinThread( &sServerThread, &sReturnStatus, aErrorStack ) == STL_SUCCESS );
    stlSleep( STL_SET_MSEC_TIME( 300 ) );

    STL_TRY( stlRcClose( sHandle, aErrorStack ) == STL_SUCCESS );
 
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlInt32 main( stlInt32 aArgc, stlChar** aArgv )
{
    stlErrorStack  sErrorStack;

    stlSRand( stlNow() );

    STL_INIT_ERROR_STACK( &sErrorStack );

    STL_TRY( stlInitialize() == STL_SUCCESS );

    STL_TRY( getIbIp( &gIbIp, &sErrorStack) == STL_SUCCESS );

    /*
     * 1. data send and recv test
     */
    stlPrintf( "1. Test data send and recv\n" );
    STL_TRY( testSendRecv( &sErrorStack ) == STL_SUCCESS );

    /*
     * 2. parameter 설정 test
     * timeout, busywait 설정, window size
     */
    stlPrintf( "2. Test Parameter\n" );
    STL_TRY( testParameter( &sErrorStack ) == STL_SUCCESS );

    /*
     * 3. negative test
     *    timeout, wrong parameter
     */
    stlPrintf( "3. Test Negative\n" );
    STL_TRY( testNegative( &sErrorStack ) == STL_SUCCESS );

    stlTerminate();

    return STL_SUCCESS;

    STL_FINISH;
    
    PRINTERR( &sErrorStack );
    PRINTFAIL;
    
    return STL_FAILURE;
}

int getFile(
        const char* pFileName,
        char* pBuff,
        int nBuffSize)
{
    int nRtn;
    int nLen = 0;
    FILE* pFp = NULL;

    pFp = fopen(pFileName, "r");
    if( pFp == NULL )
    {
        return 0;
    }

    while( 1 )
    {
        nRtn = fread(pBuff+nLen, 1, nBuffSize-nLen, pFp);
        if ( nRtn == 0 )
        {
            break;
        }
        else if ( nRtn < 0 )
        {
            fprintf(stderr, "EXPMW(ib_init) getFile: fread fail [%s]\n", pFileName);
            STL_ASSERT( 0 );
        }

        nLen += nRtn;
        if ( nBuffSize == nLen )
        {
            fprintf(stderr, "EXPMW(ib_init) getFile: buffer overflow [%d] [%d]\n", nBuffSize, nLen);
            STL_ASSERT( 0 );
        }
    }

    fclose(pFp);

    return 0;
}

 

int getIbIpv4(
        const char* pIbDeviceName,
        int nIbPort,
        char* pIpv4)
{
    int nRtn;
    char szFileName[256];
    char szIbResource[1024];
    char szIbPort[32];
    char szEthResource[1024];
    struct ifaddrs *ifaddr, *ifa;
    int family, s;
    char host[NI_MAXHOST];
    int nFound = 0;
    int nLen;

    // ib resource를 가져온다
    snprintf(szFileName,
             sizeof(szFileName),
             "/sys/class/infiniband/%s/device/resource",
             pIbDeviceName);

    memset(szIbResource, 0x00, sizeof(szIbResource));
    nRtn = getFile(szFileName, szIbResource, sizeof(szIbResource));
    if (nRtn != 0 )
    {
        fprintf(stderr, "EXPMW(ib_init) getFile[%s] error \n", szFileName);
        STL_ASSERT( 0 );
    }

    // eth들을 검색한다
    if (getifaddrs(&ifaddr) == -1) {
        fprintf(stderr, "EXPMW(ib_init) getifaddrs \n");
        STL_ASSERT( 0 );
    }


    /* Walk through linked list, maintaining head pointer so we
    *               can free list later */

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;

        family = ifa->ifa_addr->sa_family;

        /* Display interface name and family (including symbolic
         *                   form of the latter for the common families) */

        if ( family != AF_INET )
        {
            continue;
        }

        /* For an AF_INET* interface address, display the address */

        s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),
                host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
        if (s != 0) {
            fprintf(stderr, "EXPMW(ib_init) getnameinfo() failed: %d\n", s);
            STL_ASSERT( 0 );
        }
        if ( memcmp(ifa->ifa_name, "ib", 2) != 0 )
        {
            continue;
        }

        // eth로 ib port를 가져온다
        snprintf(szFileName,
                 sizeof(szFileName),
                 "/sys/class/net/%s/dev_id",
                 ifa->ifa_name);

        memset(szIbPort, 0x00, sizeof(szIbPort));
        nRtn = getFile(szFileName, szIbPort, sizeof(szIbPort));
        if (nRtn != 0)
        {
            fprintf(stderr, "EXPMW(ib_init) getFile(%s) failed\n", szFileName);
            STL_ASSERT( 0 );
        }


        if ( memcmp(szIbPort, "0x0", 3) == 0 )
        {
            // 0x0이면 ib port 1이다.
            if ( nIbPort != 1 )
            {
                continue;
            }
        }
        else if ( memcmp(szIbPort, "0x1", 3) == 0 )
        {
            // 0x0이면 ib port 1이다.
            if ( nIbPort != 2 )
            {
                continue;
            }
        }
        else
        {
            continue;
        }

        // eth resource를 가져온다
        snprintf(szFileName,
                 sizeof(szFileName),
                 "/sys/class/net/%s/device/resource",
                 ifa->ifa_name);

        memset(szEthResource, 0x00, sizeof(szEthResource));

        if ( access(szFileName, F_OK) != 0 )
        {
            continue;
        }

        nRtn = getFile(szFileName, szEthResource, sizeof(szEthResource));
        if ( nRtn < 0 )
        {
            STL_ASSERT( 0 );
        }

        // length가 다르게 나오는 경우 있어서 작은 길이로 비교함.
        nLen = strlen(szEthResource);
        if ( nLen > strlen(szIbResource) )
        {
            nLen = strlen(szIbResource);
        }


        if ( memcmp(szIbResource, szEthResource, nLen) == 0 )
        {
            strcpy(pIpv4, host);
            nFound = 1;
            break;
        }
//        fprintf(stderr, "EXPMW(ib_init) \taddress: <%s>\n", host);
    }

    freeifaddrs(ifaddr);

    if ( nFound == 1 )
        return 0;
    else
    {
//        fprintf(stderr, "EXPMW(ib_init) not found\n");
        return -1;
    }
}

stlStatus getIbIp( stlChar       ** aIpAddr,
                   stlErrorStack  * aErrorStack )
{
    int nRtn;
    int i;
    int port;
    struct ibv_device **dev_list;
    int num_of_hcas;
    const char* pIbDeviceName;
    struct ibv_context *ctx;
    struct ibv_device_attr device_attr;
    struct ibv_port_attr port_attr;

    dev_list = ibv_get_device_list(&num_of_hcas);

    for(i=0; i<num_of_hcas; i++) 
    {    
        pIbDeviceName = ibv_get_device_name(dev_list[i]);
        STL_ASSERT(pIbDeviceName != NULL);
    
        ctx = ibv_open_device(dev_list[i]);
        STL_ASSERT(ctx != NULL);

        nRtn = ibv_query_device(ctx, &device_attr);
        STL_ASSERT(nRtn == 0);

        for(port = 1; port <= device_attr.phys_port_cnt; port++)
        {    
            nRtn = ibv_query_port(ctx, port, &port_attr);
            if (nRtn != 0) { 
                fprintf(stderr, "EXPMW(ib_init) Failed to query port %d props\n", port);
                STL_ASSERT( 0 );
            }    

            char szIpv4[16];
            nRtn = getIbIpv4(pIbDeviceName, port, szIpv4);
            if ( nRtn < 0 )
            {
                continue;
            }

            stlStrcpy( *aIpAddr, szIpv4 );

            break;


/*
            fprintf(stderr, "EXPMW(ib_init) IB addr [%s][%s][%x%x][%d][%d]\n",
                gaIbPortInfo[nIdx]->szName,
                gaIbPortInfo[nIdx]->szIpv4,
                gaIbPortInfo[nIdx]->abyIpv6[14],
                gaIbPortInfo[nIdx]->abyIpv6[15],
                gaIbPortInfo[nIdx]->nAltPort,
                gaIbPortInfo[nIdx]->nAltLid);
*/

        }

        nRtn = ibv_close_device(ctx);
        if ( nRtn != 0 )
        {
            STL_ASSERT( 0 );
        }

    }

    ibv_free_device_list(dev_list);

    return 0;
}

stlStatus myRcSendWait( stlRcHandle   * aHandle,
                        stlChar       * aStartPtr,
                        stlSize         aDataLen,
                        stlErrorStack * aErrorStack )
{
    stlSize sSentLen = 0;
    stlSize sTempLen = 0;

    while ( sSentLen < aDataLen)
    {
        sTempLen = aDataLen - sSentLen;
        if( stlRcSend( aHandle,
                       aStartPtr + sSentLen,
                       &sTempLen,
                       aErrorStack ) == STL_FAILURE )
        {
            STL_TRY( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_TIMEDOUT );
            (void)stlPopError( aErrorStack );
        }
        else
        {
            sSentLen += sTempLen;
        }

    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE; 
}


stlStatus myRcRecvWait( stlRcHandle     aHandle,
                        stlChar       * aStartPtr,
                        stlSize         aBufferSize,
                        stlErrorStack * aErrorStack)
{
    stlSize sReceivedLen = 0;
    stlSize sTempLen = 0;

    while ( sReceivedLen < aBufferSize )
    {
        sTempLen = aBufferSize - sReceivedLen;
        if( stlRcRecv( aHandle,
                       aStartPtr + sReceivedLen,
                       &sTempLen,
                       aErrorStack ) == STL_FAILURE )
        {
            STL_TRY( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_TIMEDOUT );
            (void)stlPopError( aErrorStack );
        }
        else
        {
            sReceivedLen += sTempLen;
        }
    }

    return STL_SUCCESS;

    STL_FINISH;
    return STL_FAILURE; 
}




