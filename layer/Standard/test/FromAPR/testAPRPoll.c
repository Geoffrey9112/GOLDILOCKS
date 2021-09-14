#include <stdio.h>
#include <stdlib.h>
#include <stl.h>

#define SMALL_NUM_SOCKETS 3
/* We can't use 64 here, because some platforms *ahem* Solaris *ahem* have
 * a default limit of 64 open file descriptors per process.  If we use
 * 64, the test will fail even though the code is correct.
 */
#define LARGE_NUM_SOCKETS 50

#define PRINTERR                                            \
    do                                                      \
    {                                                       \
        stlPrintf( "ERRCODE: %d, ERRSTRING: %s\n",          \
                   stlGetLastErrorCode( &sErrStack ),       \
                   stlGetLastErrorMessage( &sErrStack ) );  \
    } while(0)

stlSocket    gSocket[LARGE_NUM_SOCKETS];
stlSockAddr  gSockAddr[LARGE_NUM_SOCKETS];

stlPollSet   gPollSet;
stlPollFd    gPollFd[LARGE_NUM_SOCKETS];

stlInt32     gCount = 0;

/**
 * @brief Socket 생성
 */
stlBool MakeSocket( stlSocket     *aSocket,
                    stlSockAddr   *aSockAddr,
                    stlPort        aPort,
                    stlErrorStack *aErrStack)
{
    stlInt16  sSockFamily;
    stlChar   sAddrIP[256];
    stlPort   sPort;
    
    STL_TRY( stlSetSockAddr( aSockAddr,
                             STL_AF_INET,
                             "127.0.0.1",
                             aPort,
                             NULL,
                             aErrStack )
             == STL_SUCCESS);

    STL_TRY( stlGetSockAddr( aSockAddr,
                             sAddrIP,
                             256,
                             &sPort,
                             NULL,
                             0,
                             aErrStack )
             == STL_SUCCESS );
                             
    STL_TRY( stlCreateSocket( aSocket,
                              sSockFamily,
                              STL_SOCK_DGRAM,
                              0,
                              aErrStack )
             == STL_SUCCESS);

    STL_TRY( stlBind( *aSocket, aSockAddr, aErrStack ) == STL_SUCCESS);

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief Send Message 함수
 */
stlBool SendMsg(stlSocket     * aSocketArray,
                stlSockAddr   * aSockAddrs,
                stlInt32        aWhich,
                stlErrorStack * aErrStack )
{
    stlSize   sLen = 5;

    STL_TRY( aSocketArray != NULL);

    STL_TRY( stlSendTo( aSocketArray[aWhich],
                        &aSockAddrs[aWhich],
                        0,
                        "hello",
                        &sLen,
                        aErrStack )
             == STL_SUCCESS );
    STL_TRY( strlen("hello") == sLen );

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief Receive Message 함수
 */
stlBool RecvMsg(stlSocket     * aSocketArray,
                stlInt32        aWhich,
                stlErrorStack * aErrStack )
{
    stlSize      sBufLen = 5;
    stlChar      sBuffer[5];
    stlSockAddr  aRecvSockAddr;

    STL_TRY( aSocketArray != NULL );

    STL_TRY( stlSetSockAddr( &aRecvSockAddr,
                             STL_AF_INET,
                             "127.0.0.1",
                             7770,
                             0,
                             aErrStack )
             == STL_SUCCESS );

    STL_TRY( stlRecvFrom( aSocketArray[aWhich],
                          &aRecvSockAddr,
                          0,
                          sBuffer,
                          &sBufLen,
                          STL_TRUE,
                          aErrStack )
             == STL_SUCCESS);
    STL_TRY( strlen("hello") == sBufLen );
    STL_TRY( strncmp("hello", sBuffer, sBufLen ) == 0 );

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief 다수의 소켓 생성
 */
stlBool TestCreateAllSockets()
{
    stlInt32 sIdx;

    stlErrorStack  sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );
    
    for (sIdx = 0; sIdx < LARGE_NUM_SOCKETS; sIdx++)
    {
        STL_TRY( MakeSocket( &gSocket[sIdx],
                             &gSockAddr[sIdx],
                             7777 + sIdx,
                             &sErrStack )
                 == STL_TRUE );
    }

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;

    return STL_FALSE;
}

/**
 * @brief stlInitPollSet() 함수 테스트
 */
stlBool TestSetupPollSet()
{
    stlErrorStack  sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );

    STL_TRY( stlInitPollSet( &gPollSet,
                             gPollFd,
                             LARGE_NUM_SOCKETS,
                             &sErrStack )
             == STL_SUCCESS);

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlPollPollSet() 함수 테스트
 */
stlBool TestMultiEventPollSet()
{
    stlPollFd         sSocketPollFd;
    stlInt32          sNum;
    stlInt32          sEvent;

    stlErrorStack     sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );
    
    sSocketPollFd.mSocketHandle = gSocket[0];
    sSocketPollFd.mReqEvents = STL_POLLIN | STL_POLLOUT;

    STL_TRY( stlAddSockPollSet( &gPollSet,
                                &sSocketPollFd,
                                &sErrStack )
             == STL_SUCCESS);

    STL_TRY( SendMsg( gSocket,
                      gSockAddr,
                      0,
                      &sErrStack )
             == STL_TRUE);

    STL_TRY( stlPollPollSet( &gPollSet,
                             &sNum,
                             -1,
                             &sErrStack )
             == STL_SUCCESS );

    if (sNum == 1)
    {
        sEvent = gPollSet.mPollFd[0].mRetEvents;

        STL_TRY( gSocket[0] == sSocketPollFd.mSocketHandle );
        STL_TRY( ((sEvent & STL_POLLIN) != 0) ||
                 ((sEvent & STL_POLLOUT) != 0) );
    }
    else if (sNum == 2)
    {
        STL_TRY( gPollSet.mPollFd[0].mSocketHandle == gSocket[0] );
        STL_TRY( gPollSet.mPollFd[1].mSocketHandle == gSocket[1] );

        STL_TRY( (gPollSet.mPollFd[0].mRetEvents | gPollSet.mPollFd[1].mRetEvents )
                 == (STL_POLLIN | STL_POLLOUT) );
    }
    else
    {
        STL_TRY(0);
    }

    STL_TRY( RecvMsg( gSocket, 0, &sErrStack ) == STL_TRUE);

    STL_TRY( stlPollPollSet( &gPollSet,
                             &sNum,
                             0,
                             &sErrStack )
             == STL_SUCCESS );
    STL_TRY( sNum == 1);
    STL_TRY( gPollSet.mPollFd[0].mRetEvents == STL_POLLOUT );

    STL_TRY( stlRemoveSockPollSet( &gPollSet,
                                   &sSocketPollFd,
                                   &sErrStack )
             == STL_SUCCESS);

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;
}

/**
 * @brief stlAddSockPollSet() 함수 테스트
 */
stlBool TestAddSocketsPollSet()
{
    stlInt32  sIdx;
    stlPollFd sSocketPollFd;

    stlErrorStack     sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );
    
    for (sIdx = 0; sIdx < LARGE_NUM_SOCKETS; sIdx++)
    {
        sSocketPollFd.mSocketHandle = gSocket[sIdx];
        sSocketPollFd.mReqEvents = STL_POLLIN;

        STL_TRY( stlAddSockPollSet( &gPollSet,
                                    &sSocketPollFd,
                                    &sErrStack )
                 == STL_SUCCESS );
    }

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;
}

/**
 * @brief stlPollPollSet() 함수 테스트
 */
stlBool TestNoMessagePollSet()
{
    stlInt32          sNum;

    stlErrorStack     sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );

    STL_TRY( stlPollPollSet( &gPollSet,
                             &sNum,
                             1,
                             &sErrStack )
             != STL_SUCCESS );
    
    STL_TRY( stlGetLastErrorCode( &sErrStack ) == STL_ERRCODE_TIMEDOUT );

    STL_TRY(0 == sNum);

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;
}

/**
 * @brief stlPollPollSet() 함수 테스트
 */
stlBool TestSend0PollSet()
{
    stlInt32         sNum;

    stlErrorStack     sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );
    
    STL_TRY( SendMsg( gSocket,
                      gSockAddr,
                      0,
                      &sErrStack )
             == STL_TRUE );
    
    STL_TRY( stlPollPollSet( &gPollSet,
                             &sNum,
                             -1,
                             &sErrStack )
             == STL_SUCCESS );
    STL_TRY(1 == sNum);

    STL_TRY( gSocket[0] == gPollSet.mPollFd[0].mSocketHandle );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;
}

/**
 * @brief stlPollPollSet() 함수 테스트
 */
stlBool TestRecv0PollSet()
{
    stlInt32         sNum;

    stlErrorStack     sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );
    
    STL_TRY( RecvMsg( gSocket, 0, &sErrStack ) == STL_TRUE );

    STL_TRY( stlPollPollSet( &gPollSet,
                             &sNum,
                             0,
                             &sErrStack )
             != STL_SUCCESS );

    STL_TRY( stlGetLastErrorCode(&sErrStack) == STL_ERRCODE_TIMEDOUT );

    STL_TRY(0 == sNum);

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;
}

/**
 * @brief stlPollPollSet() 함수 테스트
 */
stlBool TestSendMiddlePollSet()
{
    stlInt32         sNum;
    stlInt32         sEventCnt;
    stlInt32         i;

    stlErrorStack     sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );
    
    STL_TRY( SendMsg(gSocket, gSockAddr, 2, &sErrStack ) == STL_TRUE );
    STL_TRY( SendMsg(gSocket, gSockAddr, 5, &sErrStack ) == STL_TRUE );
    
    STL_TRY( stlPollPollSet( &gPollSet,
                             &sNum,
                             -1,
                             &sErrStack )
             == STL_SUCCESS );

    STL_TRY( (1 == sNum) || (2 == sNum) );

    sEventCnt = 0;
    for( i = 0; i < gPollSet.mCurPollFdNum; i++ )
    {
        if( (gPollSet.mPollFd[i].mRetEvents & STL_POLLIN) != 0 )
        {
            STL_TRY( gPollSet.mPollFd[i].mSocketHandle == gSocket[2] ||
                     gPollSet.mPollFd[i].mSocketHandle == gSocket[5] );
            sEventCnt++;
        }
    }
    STL_TRY( sNum == sEventCnt );
    
    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;
}

/**
 * @brief stlPollPollSet() 함수 테스트
 */
stlBool TestClearMiddlePollSet()
{
    stlInt32         sNum;

    stlErrorStack     sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );

    STL_TRY( RecvMsg(gSocket, 2, &sErrStack) == STL_TRUE );
    STL_TRY( RecvMsg(gSocket, 5, &sErrStack) == STL_TRUE );

    STL_TRY( stlPollPollSet( &gPollSet,
                             &sNum,
                             0,
                             &sErrStack )
             != STL_SUCCESS );
    STL_TRY( stlGetLastErrorCode( &sErrStack ) == STL_ERRCODE_TIMEDOUT );

    STL_TRY(0 == sNum);

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;
}

/**
 * @brief stlPollPollSet() 함수 테스트
 */
stlBool TestSendLastPollSet()
{
    stlInt32         i;
    stlInt32         sNum;

    stlErrorStack     sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );

    STL_TRY( SendMsg( gSocket,
                      gSockAddr,
                      LARGE_NUM_SOCKETS - 1,
                      &sErrStack )
             == STL_TRUE );

    STL_TRY( stlPollPollSet( &gPollSet,
                             &sNum,
                             -1,
                             &sErrStack )
             == STL_SUCCESS );
    STL_TRY( 1 == sNum );

    for ( i = 0; i < gPollSet.mCurPollFdNum; i++ )
    {
        if ( gPollSet.mPollFd[i].mRetEvents != 0 )
        {
            STL_TRY( gPollSet.mPollFd[i].mSocketHandle == gSocket[LARGE_NUM_SOCKETS - 1] );
        }
    }

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlPollPollSet() 함수 테스트
 */
stlBool TestClearLastPollSet()
{
    stlInt32         sNum;

    stlErrorStack     sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );
    
    STL_TRY( RecvMsg( gSocket,
                      LARGE_NUM_SOCKETS - 1,
                      &sErrStack )
             == STL_TRUE );

    STL_TRY( stlPollPollSet( &gPollSet,
                             &sNum,
                             0,
                             &sErrStack )
             != STL_SUCCESS );
    STL_TRY( stlGetLastErrorCode( &sErrStack ) == STL_ERRCODE_TIMEDOUT );

    STL_TRY(0 == sNum);

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;
}

/**
 * @brief stlRemovePollSet() 함수 테스트
 */
stlBool TestPollSetRemove()
{
    stlPollSet       sPollSet;
    stlPollFd        sFd;
    
    stlInt32          sNum;
    stlInt32          i;

    stlErrorStack     sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );
    
    STL_TRY( stlInitPollSet( &sPollSet,
                             gPollFd,
                             5,
                             &sErrStack )
             == STL_SUCCESS );

    sFd.mSocketHandle = gSocket[0];
    sFd.mReqEvents = STL_POLLOUT;
    STL_TRY( stlAddSockPollSet( &sPollSet,
                                &sFd,
                                &sErrStack )
             == STL_SUCCESS);

    sFd.mSocketHandle = gSocket[1];
    sFd.mReqEvents = STL_POLLOUT;
    STL_TRY( stlAddSockPollSet( &sPollSet,
                                &sFd,
                                &sErrStack )
             == STL_SUCCESS);

    sFd.mSocketHandle = gSocket[2];
    sFd.mReqEvents = STL_POLLOUT;
    STL_TRY( stlAddSockPollSet( &sPollSet,
                                &sFd,
                                &sErrStack )
             == STL_SUCCESS);

    sFd.mSocketHandle = gSocket[3];
    sFd.mReqEvents = STL_POLLOUT;
    STL_TRY( stlAddSockPollSet( &sPollSet,
                                &sFd,
                                &sErrStack )
             == STL_SUCCESS);

    STL_TRY( stlPollPollSet( &sPollSet,
                             &sNum,
                             1000,
                             &sErrStack )
             == STL_SUCCESS );
    STL_TRY(4 == sNum);
    
    /* now remove the pollset element referring to desc gSocket[1] */
    sFd.mSocketHandle = gSocket[1];
    STL_TRY( stlRemoveSockPollSet( &sPollSet,
                                   &sFd,
                                   &sErrStack )
             == STL_SUCCESS );

    /* this time only three should match */
    STL_TRY( stlPollPollSet( &sPollSet,
                             &sNum,
                             1000,
                             &sErrStack )
             == STL_SUCCESS );
    STL_TRY(3 == sNum);

    /* now remove the pollset elements referring to desc gSocket[2] */
    sFd.mSocketHandle = gSocket[2];
    STL_TRY( stlRemoveSockPollSet( &sPollSet,
                                   &sFd,
                                   &sErrStack )
             == STL_SUCCESS );

    /* this time only two should match */
    STL_TRY( stlPollPollSet( &sPollSet,
                             &sNum,
                             1000,
                             &sErrStack )
             == STL_SUCCESS );
    STL_TRY(2 == sNum);

    for ( i = 0; i < sPollSet.mCurPollFdNum; i++ )
    {
        if ( sPollSet.mPollFd[i].mRetEvents != 0 )
        {
            STL_TRY( (sPollSet.mPollFd[i].mSocketHandle == gSocket[0]) ||
                     (sPollSet.mPollFd[i].mSocketHandle == gSocket[3]) );
            sNum--;
        }
    }

    STL_TRY( sNum == 0 );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;
}

/**
 * @brief stlCloseSocket() 함수 테스트
 */
stlBool TestCloseAllSockets()
{
    stlInt32  sIdx;

    stlErrorStack     sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );
    
    for (sIdx = 0; sIdx < LARGE_NUM_SOCKETS; sIdx++)
    {
        STL_TRY( stlCloseSocket( gSocket[sIdx], &sErrStack ) == STL_SUCCESS);
    }

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;

    return STL_FALSE;
}

/**
 * @brief Poll Set Trigger Callback Function
 */
stlStatus TriggerPollCallbackFunc(stlPollFd * aPollFd, stlErrorStack * aErrStack  )
{
    STL_TRY( aPollFd != NULL );
    STL_TRY( aPollFd->mSocketHandle > 0 );
    gCount++;

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FALSE;
}

/**
 * @brief stlDispatchPollSet() 함수 테스트
 */
stlBool TestSetupPollCb()
{
    stlErrorStack     sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );

    STL_TRY( stlDispatchPollSet( &gPollSet,
                                 0,
                                 TriggerPollCallbackFunc,
                                 &sErrStack )
             == STL_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;
}

/**
 * @brief stlDispatchPollSet() 함수 테스트
 */
stlBool TestTriggerPollCb()
{
    stlPollFd sSocketPollFd;

    stlErrorStack     sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );

    gCount = 0;

    STL_TRY( stlInitPollSet( &gPollSet,
                             gPollFd,
                             50,
                             &sErrStack )
             == STL_SUCCESS );
    
    sSocketPollFd.mSocketHandle = gSocket[0];
    sSocketPollFd.mReqEvents = STL_POLLIN;

    STL_TRY( stlAddSockPollSet( &gPollSet,
                                &sSocketPollFd,
                                &sErrStack )
             == STL_SUCCESS );

    STL_TRY( SendMsg( gSocket,
                      gSockAddr,
                      0,
                      &sErrStack )
             == STL_TRUE);

    STL_TRY( stlDispatchPollSet( &gPollSet,
                                 0,
                                 TriggerPollCallbackFunc,
                                 &sErrStack )
             == STL_SUCCESS );
    STL_TRY( gCount == 1 );

    STL_TRY( stlRemoveSockPollSet( &gPollSet,
                                   &sSocketPollFd,
                                   &sErrStack )
             == STL_SUCCESS );
    
    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;
}

/**
 * @brief stlPollPollSet() 함수 테스트
 */
stlBool TestTimeoutPollCb()
{
    stlInt32  sCount = 0;

    stlErrorStack     sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );
    
    STL_TRY( stlPollPollSet( &gPollSet,
                             &sCount,
                             1,
                             &sErrStack )
             != STL_SUCCESS );
    STL_TRY( stlGetLastErrorCode( &sErrStack ) == STL_ERRCODE_TIMEDOUT );
    STL_TRY( sCount == 0 );
    
    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;
}

/**
 * @brief stlDispatchPollSet() 함수 테스트
 */
stlBool TestTimeoutPollInPollCb()
{
    stlPollFd sSocketPollFd;
    stlInt32  sCount = 0;

    stlErrorStack     sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );
    
    STL_TRY( RecvMsg( gSocket, 0, &sErrStack ) == STL_TRUE );

    sSocketPollFd.mSocketHandle = gSocket[0];
    sSocketPollFd.mReqEvents    = STL_POLLIN;
    
    STL_TRY( stlAddSockPollSet( &gPollSet,
                                &sSocketPollFd,
                                &sErrStack )
             == STL_SUCCESS );
    
    STL_TRY( stlPollPollSet( &gPollSet,
                             &sCount,
                             1,
                             &sErrStack )
             != STL_SUCCESS );
    STL_TRY( stlGetLastErrorCode( &sErrStack ) == STL_ERRCODE_TIMEDOUT );
    STL_TRY(0 == sCount);

    STL_TRY( stlRemoveSockPollSet( &gPollSet,
                                   &sSocketPollFd,
                                   &sErrStack )
             == STL_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;
}

stlInt32 main(stlInt32 aArgc, stlChar** aArgv)
{
    stlInt32 sState = 0;

    STL_TRY(stlInitialize() == STL_SUCCESS);

    /*
     * Test CreateAllSockets
     */
    sState = 1;
    STL_TRY(TestCreateAllSockets() == STL_TRUE);

    /*
     * Test SetupPollSet
     */
    sState = 2;
    STL_TRY(TestSetupPollSet() == STL_TRUE);

    /*
     * Test MultiEventPollSet
     */
    sState = 3;
    STL_TRY(TestMultiEventPollSet() == STL_TRUE);

    /*
     * Test AddSocketsPollset
     */
    sState = 4;
    STL_TRY(TestAddSocketsPollSet() == STL_TRUE);

    /*
     * Test NoMessagePollset
     */
    sState = 5;
    STL_TRY(TestNoMessagePollSet() == STL_TRUE);

    /*
     * Test Send0PollSet
     */
    sState = 6;
    STL_TRY(TestSend0PollSet() == STL_TRUE);

    /*
     * Test Recv0PollSet
     */
    sState = 7;
    STL_TRY(TestRecv0PollSet() == STL_TRUE);

    /*
     * Test SendMiddlePollSet
     */
    sState = 8;
    STL_TRY(TestSendMiddlePollSet() == STL_TRUE);

    /*
     * Test ClearMiddlePollSet
     */
    sState = 9;
    STL_TRY(TestClearMiddlePollSet() == STL_TRUE);

    /*
     * Test SendLastPollSet
     */
    sState = 10;
    STL_TRY(TestSendLastPollSet() == STL_TRUE);

    /*
     * Test ClearLastPollSet
     */
    sState = 11;
    STL_TRY(TestClearLastPollSet() == STL_TRUE);

    /*
     * Test PollSetRemove
     */
    sState = 12;
    STL_TRY(TestPollSetRemove() == STL_TRUE);

    /*
     * Test CloseAllSockets
     */
    sState = 13;
    STL_TRY(TestCloseAllSockets() == STL_TRUE);

    /*
     * Test SetupPollCb
     */
    sState = 14;
    STL_TRY(TestCreateAllSockets() == STL_TRUE);
    STL_TRY(TestSetupPollCb() == STL_TRUE);

    /*
     * Test TriggerPollCb
     */
    sState = 15;
    STL_TRY(TestTriggerPollCb() == STL_TRUE);

    /*
     * Test TimeoutPollCb
     */
    sState = 16;
    STL_TRY(TestTimeoutPollCb() == STL_TRUE);

    /*
     * Test TimeoutPollInPollCb
     */
    sState = 17;
    STL_TRY(TestTimeoutPollInPollCb() == STL_TRUE);
    STL_TRY(TestCloseAllSockets() == STL_TRUE);

    stlPrintf("PASS");

    return (0);

    STL_FINISH;

    stlPrintf("FAILURE %d", sState);

    return (-1);
}
