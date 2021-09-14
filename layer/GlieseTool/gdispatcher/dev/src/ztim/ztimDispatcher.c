/*******************************************************************************
 * ztimDispatcher.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztimDispatcher.c 11483 2014-02-28 03:18:44Z lym $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file ztimDispatcher.c
 * @brief Gliese dispatcher shared Routines
 */

#include <stl.h>
#include <cml.h>
#include <scl.h>
#include <ssl.h>
#include <sll.h>
#include <ztiDef.h>
#include <ztim.h>
#include <ztis.h>

/**
 * 이 파일에서 구현된 함수는 process가 죽어야 하는 상황이 아니면 error를 리턴하지 않는다.
 *
 */

sllSharedMemory  * gZtiSharedMemory = NULL;
ztiDispatcher      gZtimDispatcher;

stlStatus ztimInitialize( ztiDispatcher      * aDispatcher,
                          stlInt32             aDispatcherId,
                          sllEnv             * aEnv )
{
    stlInt32        sState = 0;

    aDispatcher->mZlhlDispatcher = &gZtiSharedMemory->mDispatcher[aDispatcherId];
    aDispatcher->mZlhlDispatcher->mStartTime = knlGetSystemTime();
    aDispatcher->mRetry = STL_FALSE;


    STL_DASSERT( aDispatcher->mZlhlDispatcher->mResponseEvent != NULL );

    /* 초기 설정을 한다. */
    aDispatcher->mZlhlDispatcher->mSocketCount = 0;

    STL_TRY( ztimGetMaxContextCount( aDispatcher,
                                     &aDispatcher->mMaxContextCount,
                                     aEnv )
             == STL_SUCCESS );

    STL_TRY( knlCreateDynamicMem( &aEnv->mAllocator,
                                  NULL, /* aParentMem */
                                  KNL_ALLOCATOR_DISPATCHER_DYNAMIC,
                                  KNL_MEM_STORAGE_TYPE_HEAP,
                                  ZTI_HEAPMEM_CHUNK_SIZE,
                                  ZTI_HEAPMEM_CHUNK_SIZE,
                                  NULL, /* aMemController */
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 1;


    STL_TRY( knlAllocDynamicMem( &aEnv->mAllocator,
                                 STL_SIZEOF(ztiContext) * aDispatcher->mMaxContextCount,
                                 (void **)&aDispatcher->mContext,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 2;


    stlMemset( aDispatcher->mContext, 0x00, STL_SIZEOF(ztiContext) * aDispatcher->mMaxContextCount );

    aDispatcher->mId = aDispatcherId;

    STL_TRY( stlCreatePollSet( &aDispatcher->mPollSet,
                               aDispatcher->mMaxContextCount,
                               KNL_ERROR_STACK(aEnv) ) == STL_SUCCESS );

    sState = 3;

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 3:
            (void)stlDestroyPollSet( &aDispatcher->mPollSet, KNL_ERROR_STACK(aEnv) );
        case 2:
            (void)knlFreeDynamicMem( &aEnv->mAllocator,
                                     aDispatcher->mContext,
                                     KNL_ENV(aEnv) );
        case 1:
            (void)knlDestroyDynamicMem( &aEnv->mAllocator, KNL_ENV(aEnv) );
        default :
            break;
    }

    return STL_FAILURE;
}


stlStatus ztimFinalize( ztiDispatcher      * aDispatcher,
                        sllEnv             * aEnv )
{
    (void)stlDestroyPollSet( &aDispatcher->mPollSet, KNL_ERROR_STACK(aEnv) );
    (void)knlFreeDynamicMem( &aEnv->mAllocator,
                             aDispatcher->mContext,
                             KNL_ENV(aEnv) );
    (void)knlDestroyDynamicMem( &aEnv->mAllocator, KNL_ENV(aEnv) );

    return STL_SUCCESS;
}

stlStatus ztimInitializeUds( ztiContext      * aUds,
                             stlInt64          aKey,
                             stlInt32          aDispatcherIndex,
                             sllEnv          * aEnv )
{
    stlChar     sDispatcherPath[STL_MAX_FILE_PATH_LENGTH];

    /* uds용 dispatcher path를 생성한다 */
    stlSnprintf( sDispatcherPath,
                 STL_MAX_FILE_PATH_LENGTH,
                 "%stmp%s%s-%s.%ld.%d",
                 STL_PATH_SEPARATOR,
                 STL_PATH_SEPARATOR,
                 "goldilocks-unix",
                 STL_DISPATCHER_FD_NAME,
                 aKey,
                 aDispatcherIndex );

    /* UDS context를 생성 for Command  */
    if( stlInitializeContext( &aUds->mStlContext,
                              STL_AF_UNIX,
                              STL_SOCK_DGRAM,
                              0,
                              STL_FALSE,
                              sDispatcherPath,
                              0,
                              KNL_ERROR_STACK(aEnv) ) == STL_SUCCESS )
    {
        (void) knlLogMsg( NULL,
                          KNL_ENV(aEnv),
                          KNL_LOG_LEVEL_INFO,
                          "[DISPATCHER-%d] Unix Domain Socket [%s] is created \n",
                          aDispatcherIndex,
                          sDispatcherPath );
    }
    else
    {
        (void) knlLogMsg( NULL,
                          KNL_ENV(aEnv),
                          KNL_LOG_LEVEL_ABORT,
                          "[DISPATCHER-%d] failed to create Unix Domain Socket [%s]\n",
                          aDispatcherIndex,
                          sDispatcherPath );

        STL_TRY( STL_FALSE );
    }

    STL_TRY( stlSetSocketOption( STL_SOCKET_IN_CONTEXT(aUds->mStlContext),
                                 STL_SOPT_SO_SNDBUF,
                                 CML_BUFFER_SIZE,
                                 KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    STL_TRY( stlSetSocketOption( STL_SOCKET_IN_CONTEXT(aUds->mStlContext),
                                 STL_SOPT_SO_RCVBUF,
                                 CML_BUFFER_SIZE,
                                 KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztimFinalizeUds( ztiContext      * aUds,
                           sllEnv          * aEnv )
{
    (void)stlFinalizeContext( &aUds->mStlContext, KNL_ERROR_STACK(aEnv) );

    return STL_SUCCESS;
}

/**
 * Context를 초기화한다
 */
stlStatus ztimInitializeContext( ztiDispatcher     * aDispatcher,
                                 ztiContext        * aContext,
                                 stlInt32            aIdx,
                                 stlInt16            aMajorVersion,
                                 stlInt16            aMinorVersion,
                                 stlInt16            aPatchVersion,
                                 sllEnv            * aEnv )
{
    stlInt32        sState = 0;
    stlSockAddr     sClientAddr;

    aContext->mDataForSend = NULL;
    aContext->mCmUnitCountForSend = 0;
    aContext->mPeriodEndForSend = STL_FALSE;
    aContext->mReadSize = 0;
    aContext->mReadBuffer = NULL;
    aContext->mJobCount = 0;
    aContext->mIsBrokenSocket = STL_FALSE;

    STL_TRY( knlAllocDynamicMem( &(gZtiSharedMemory->mSessionAllocator),
                                 STL_SIZEOF(sllHandle),
                                 (void **) &aContext->mSllHandle,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( stlGetPeerName( STL_SOCKET_IN_CONTEXT(aContext->mStlContext),
                             &sClientAddr,
                             KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );


    STL_TRY( sllInitializeHandle( aContext->mSllHandle,
                                  NULL,
                                  &aContext->mSllHandle->mIpc,
                                  &sClientAddr,
                                  (void *)aContext,
                                  aMajorVersion,
                                  aMinorVersion,
                                  aPatchVersion,
                                  aEnv )
             == STL_SUCCESS );
    sState = 2;

    STL_TRY( sclInitializeIpc( &aContext->mSllHandle->mIpc,
                               gZtiSharedMemory->mPacketAllocator,
                               aDispatcher->mZlhlDispatcher->mResponseEvent,
                               SCL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 3;

    aContext->mDispatcherTime = aDispatcher->mZlhlDispatcher->mStartTime;
    aContext->mUsed = STL_TRUE;

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 3:
            (void)sclFinalizeIpc( &aContext->mSllHandle->mIpc,
                                  gZtiSharedMemory->mPacketAllocator,
                                  SCL_ENV(aEnv) );
        case 2:
            (void)sllFinalizeHandle( aContext->mSllHandle,
                                     aEnv );
        case 1:
            STL_TRY( knlFreeDynamicMem( &(gZtiSharedMemory->mSessionAllocator),
                                         (void *)aContext->mSllHandle,
                                         KNL_ENV(aEnv) )
                     == STL_SUCCESS );
        default:
            break;
    }

    return STL_FAILURE;
}


/* context에서 사용중인 자원을 회수한다. */
stlStatus ztimFinalizeContext( ztiContext     * aContext,
                               sllEnv         * aEnv )
{
    aContext->mUsed = STL_FALSE;

    STL_DASSERT( aContext->mSllHandle != NULL );

    STL_TRY( sclFinalizeIpc( &aContext->mSllHandle->mIpc,
                             aContext->mSllHandle->mIpc.mArrayAllocator,
                             SCL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( sllFinalizeHandle( aContext->mSllHandle,
                                aEnv )
             == STL_SUCCESS );


    /* mSllSession 메모리를 mSessionAllocator에 반환한다 */
    STL_TRY( knlFreeDynamicMem( &(gZtiSharedMemory->mSessionAllocator),
                                (void *)aContext->mSllHandle,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    aContext->mSllHandle = NULL;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztimCloseContext( stlPollSet     * aPollSet,
                            ztiContext     * aContext,
                            stlErrorStack  * aErrorStack )
{
    STL_DASSERT( aContext->mPollFdEx.mPollFd.mSocketHandle != -1 );
    (void)stlRemoveSockPollSet( aPollSet,
                                &aContext->mPollFdEx,
                                aErrorStack );
    aContext->mPollFdEx.mPollFd.mSocketHandle = -1;

    STL_DASSERT( aContext->mStlContext.mPollFd.mSocketHandle != -1 );
    (void)stlFinalizeContext( &aContext->mStlContext, aErrorStack);
    aContext->mStlContext.mPollFd.mSocketHandle = -1;

    return STL_SUCCESS;
}


/**
 * 사용중이 아닌 context를 하나 꺼내서 output 한다.
 * TODO : 성능을 위해 loop없애야 함.
 */
stlStatus ztimGetUnusedContext( ztiDispatcher     * aDispatcher,
                                ztiContext       ** aContext,
                                stlInt32          * aIdx,
                                stlErrorStack     * aErrorStack )
{
    stlInt32        sIdx = 0;

    *aContext = NULL;

    for( sIdx = ZTI_INDEX_SESSION; sIdx < aDispatcher->mMaxContextCount; sIdx++ )
    {
        if( aDispatcher->mContext[sIdx].mUsed == STL_FALSE )
        {
            *aContext = &aDispatcher->mContext[sIdx];
            *aIdx = sIdx;
            break;
        }
    }

    return STL_SUCCESS;
}

/**
 * 이 함수는 1초마다 호출된다.
 */
stlStatus ztimProcessCleanup( ztiDispatcher    * aDispatcher,
                              stlTime            aNow,
                              sllEnv           * aEnv )
{
    stlInt32            sIdx;
    ztiContext        * sContext;
    sclHandle         * sSclHandle;
    stlErrorStack     * sErrorStack;

    sErrorStack = KNL_ERROR_STACK(aEnv);

    /**
     *  check timeout
     * timeout check는 현재 job처리중이 아니고
     * 마지막 response처리가 1초이상 지난경우에 체크한다.
     */
    for( sIdx = ZTI_INDEX_SESSION; sIdx < aDispatcher->mMaxContextCount; sIdx++ )
    {
        STL_INIT_ERROR_STACK( KNL_ERROR_STACK(aEnv) );

        sContext = &aDispatcher->mContext[sIdx];

        /* 사용중이 아니면 continue */
        if( sContext->mUsed == STL_FALSE )
        {
            continue;
        }

        if( sContext->mJobCount != 0 )
        {
            continue;
        }

        sSclHandle = SCL_HANDLE(sContext->mSllHandle);

        /**
         * socket disconnect 처리
         */
        if( (sContext->mIsBrokenSocket == STL_TRUE) ||
            (sSclHandle->mSocketPeriod == STL_TRUE) )
        {
            if( sContext->mStlContext.mPollFd.mSocketHandle != -1 )
            {
                aDispatcher->mZlhlDispatcher->mSocketCount--;
                STL_DASSERT( aDispatcher->mZlhlDispatcher->mSocketCount >= 0 );

                (void)ztimCloseContext( &aDispatcher->mPollSet,
                                        sContext,
                                        sErrorStack );

                STL_DASSERT( aDispatcher->mPollSet.mCurPollFdNum == aDispatcher->mZlhlDispatcher->mSocketCount + 1 );
            }

            (void)sllKillSession( sContext->mSllHandle, aEnv );

            (void)ztimFinalizeContext( sContext, aEnv );

            continue;
        }

        /**
         * check timeout 을 처리한다.
         * 마지막 data처리후 timeout(1초)이 된것만 처리한다.
         */
        if( (sContext->mSllHandle->mConnectStatus == CML_CONN_STATUS_CONNECTED) &&
            ( (sContext->mLastTime + STL_SET_SEC_TIME(1)) < aNow ) )
        {
            sContext->mLastTime = aNow;

            if( sllCheckTimeout( sContext->mSllHandle, aEnv ) == STL_FAILURE )
            {
                (void) knlLogMsg( NULL,
                                  KNL_ENV(aEnv),
                                  KNL_LOG_LEVEL_WARNING,
                                  "[DISPATCHER-%d] idle timeout or terminated session\n",
                                  aDispatcher->mId );

               (void) sllKillSession( sContext->mSllHandle, aEnv );

               aDispatcher->mZlhlDispatcher->mSocketCount--;
               STL_DASSERT( aDispatcher->mZlhlDispatcher->mSocketCount >= 0 );

               (void)ztimCloseContext( &aDispatcher->mPollSet,
                                       sContext,
                                       sErrorStack );

               STL_DASSERT( aDispatcher->mPollSet.mCurPollFdNum == aDispatcher->mZlhlDispatcher->mSocketCount + 1 );

               (void)ztimFinalizeContext( sContext, aEnv );
            }
        }
    }

    return STL_SUCCESS;
}

/**
 *  fd를 받아서 처리하는 함수
 */
stlStatus ztimProcessUds( ztiContext        * aUds,
                          stlSockAddr       * aFdSenderAddr,
                          ztiDispatcher     * aDispatcher,
                          stlInt64            aBufferSize,
                          sllEnv            * aEnv )
{
    stlInt32            sState = 0;
    ztiContext        * sNewContext = NULL;
    stlInt32            sIdx = 0;
    cmlUdsForwardFd     sForwardFd;
    stlContext          sStlContext;

    stlMemset( &sForwardFd, 0x00, STL_SIZEOF(sForwardFd) );

    /**
     * recv시 fd와 sequence를 같이 받는다.
     */
    if( cmlRecvFdFrom( &aUds->mStlContext,
                       &sStlContext,
                       &sForwardFd,
                       KNL_ERROR_STACK( aEnv ) ) == STL_FAILURE )
    {
        (void) knlLogMsg( NULL,
                          KNL_ENV(aEnv),
                          KNL_LOG_LEVEL_WARNING,
                          "[DISPATCHER-%d] failed to receive FD \n",
                          aDispatcher->mId );

        (void)stlPopError( KNL_ERROR_STACK( aEnv ) );

        STL_THROW( RAMP_SUCCESS );
    }
    sState = 1;

    aDispatcher->mZlhlDispatcher->mConnections++;

    /* Context List에서 비어 있는 Context를 하나 가져온다 */
    STL_TRY( ztimGetUnusedContext( aDispatcher,
                                   &sNewContext,
                                   &sIdx,
                                   KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    if( sNewContext == NULL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTIE_ERRCODE_OVERFLOW_FD,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );

        STL_THROW( RAMP_SUCCESS_SEND_NACK );
    }

    sNewContext->mStlContext = sStlContext;

    if( ztimInitializeContext( aDispatcher,
                               sNewContext,
                               sIdx,
                               sForwardFd.mMajorVersion,
                               sForwardFd.mMinorVersion,
                               sForwardFd.mPatchVersion,
                               aEnv ) == STL_FAILURE )
    {
        STL_THROW( RAMP_SUCCESS_SEND_NACK );
    }

    sState = 2;

    STL_TRY( cmlInitSockOption( &sNewContext->mStlContext,
                                aBufferSize,
                                KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    sNewContext->mPollFdEx.mPollFd = sNewContext->mStlContext.mPollFd;
    sNewContext->mPollFdEx.mUserContext = sNewContext;

    if( stlAddSockPollSet( &aDispatcher->mPollSet,
                           &sNewContext->mPollFdEx,
                           KNL_ERROR_STACK(aEnv) ) == STL_FAILURE )
    {
        STL_TRY( stlGetLastErrorCode( KNL_ERROR_STACK(aEnv) ) == STL_ERRCODE_OUT_OF_MEMORY );

        (void) knlLogMsg( NULL,
                          KNL_ENV(aEnv),
                          KNL_LOG_LEVEL_INFO,
                          "[DISPATCHER-%d] fd overflow\n",
                          aDispatcher->mId );

        /*
         * STL_ERRCODE_OUT_OF_MEMORY 이면 socket 종료하고 nack를 보낸다.
         */

        STL_THROW( RAMP_SUCCESS_SEND_NACK );
    }

    aDispatcher->mZlhlDispatcher->mSocketCount++;
    if( aDispatcher->mZlhlDispatcher->mSocketCount > aDispatcher->mZlhlDispatcher->mConnectionsHighwater )
    {
        aDispatcher->mZlhlDispatcher->mConnectionsHighwater = aDispatcher->mZlhlDispatcher->mSocketCount;
    }
    sState = 3;

    STL_DASSERT( aDispatcher->mPollSet.mCurPollFdNum == aDispatcher->mZlhlDispatcher->mSocketCount + 1 );

    /* aFdSenderAddr address로 sBuffer 데이타를 보낸다 */
    if( cmlSendAckTo( &aUds->mStlContext,
                      aFdSenderAddr,
                      sForwardFd.mSenderStartTime,
                      sForwardFd.mSequence,
                      sForwardFd.mIdx,
                      KNL_ERROR_STACK( aEnv ) ) == STL_FAILURE )
    {
        knlLogMsg( NULL,
                   KNL_ENV(aEnv),
                   KNL_LOG_LEVEL_INFO,
                   "[DISPATCHER-%d] failed to send ACK (%u) \n",
                   aDispatcher->mId,
                   stlGetLastErrorCode( KNL_ERROR_STACK( aEnv ) ) );

        /* ack (or nack) send하다 실패면 성공 처리 한다. */
        (void)stlPopError( KNL_ERROR_STACK(aEnv) );
    }

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_SUCCESS_SEND_NACK )
    {
        if( cmlSendAckTo( &aUds->mStlContext,
                          aFdSenderAddr,
                          sForwardFd.mSenderStartTime,
                          sForwardFd.mSequence,
                          sForwardFd.mIdx,
                          KNL_ERROR_STACK( aEnv ) ) == STL_FAILURE )
        {
            /* ack (or nack) send하다 실패면 성공 처리 한다. */
            (void)stlPopError( KNL_ERROR_STACK(aEnv) );
        }

        /* ErrorStack을 전송했으니 여기서는 clear 하고 success리턴한다. */
        (void)stlPopError( KNL_ERROR_STACK(aEnv) );
    }

    switch( sState )
    {
        case 3:
            aDispatcher->mZlhlDispatcher->mSocketCount--;

            (void)stlRemoveSockPollSet( &aDispatcher->mPollSet,
                                        &sNewContext->mPollFdEx,
                                        KNL_ERROR_STACK(aEnv) );
            sNewContext->mPollFdEx.mPollFd.mSocketHandle = -1;
        case 2:
            (void)ztimFinalizeContext( sNewContext, aEnv );
        case 1:
            (void)stlFinalizeContext( &sNewContext->mStlContext, KNL_ERROR_STACK(aEnv) );
            sNewContext->mStlContext.mPollFd.mSocketHandle = -1;
        default:
            aDispatcher->mZlhlDispatcher->mAccept = STL_FALSE;
            break;
    }

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 3:
            aDispatcher->mZlhlDispatcher->mSocketCount--;

            (void)stlRemoveSockPollSet( &aDispatcher->mPollSet,
                                        &sNewContext->mPollFdEx,
                                        KNL_ERROR_STACK(aEnv) );
            sNewContext->mPollFdEx.mPollFd.mSocketHandle = -1;
        case 2:
            (void)ztimFinalizeContext( sNewContext, aEnv );
        case 1:
            (void)stlFinalizeContext( &sNewContext->mStlContext, KNL_ERROR_STACK(aEnv) );
            sNewContext->mStlContext.mPollFd.mSocketHandle = -1;
        default:
            aDispatcher->mZlhlDispatcher->mAccept = STL_FALSE;
            break;
    }

    return STL_FAILURE;
}



/**
 * client와 연결된 socket으로 data를 받아서 처리한다.
 */
stlStatus ztimProcessRequest( ztiDispatcher     * aDispatcher,
                              ztiContext        * aContext,
                              sllEnv            * aEnv )
{
    stlBool                 sPeriodEnd;

    /* protocol이 완성 즉 PeriodEnd까지 받은 경우 sPeriodEnd가 TRUE로 설정된다 */
    if( ztimRecvPacketAsync( aDispatcher,
                             aContext,
                             &sPeriodEnd,
                             aEnv ) == STL_FAILURE )
    {
        /*
         * socket이 끊어진 경우 또는 dispatcher에서 메모리 부족인 경우 처리
         * shared memory에서 memory할당 못한경우
         */
        (void)stlPopError( KNL_ERROR_STACK(aEnv) );

        /* shared server로 data를 보내지 않도록 설정 */
        sPeriodEnd = STL_FALSE;

        aDispatcher->mZlhlDispatcher->mSocketCount--;
        STL_DASSERT( aDispatcher->mZlhlDispatcher->mSocketCount >= 0 );

        STL_TRY( ztimCloseContext( &aDispatcher->mPollSet,
                                   aContext,
                                   KNL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );

        STL_DASSERT( aDispatcher->mPollSet.mCurPollFdNum == aDispatcher->mZlhlDispatcher->mSocketCount + 1 );

        /**
         * disconnect시에 이미 shared-server에서 작업중일수 있음으로 ztimDisconnect만 호출한다.
         * shared-memory가 부족한 경우일수도 있음.
         * ztimDisconnect 설정을 해 놓으면 timer proc에서 CML_QCMD_DISCONNECT를 보냄.
         */
        aContext->mIsBrokenSocket = STL_TRUE;

        /* socket이 하나 끊어졌음으로 mIsReady상태를 true로 변경한다 */
        aDispatcher->mZlhlDispatcher->mAccept = STL_TRUE;
    }

    if( sPeriodEnd == STL_TRUE )
    {
        STL_DASSERT( aContext->mSllHandle->mSclHandle.mSocketPeriod == STL_FALSE );

        /* atomic 아니면 문제 발생함 */
        stlAtomicInc32( (stlUInt32 *)&aContext->mJobCount );

        STL_DASSERT( aContext->mJobCount <= 2 );

        /**
         * 한 세션에서의 JOB은 2개(JOB-1 + JOB-2)를 초과할수 없다
         * - JOB-1 : gserver에서 완료되고 Dispatcher에 의해서 client로 데이타를 Sending 하고 있는 JOB
         * - JOB-2 : JOB-1의 응답으로 인하여 다시 client가 보낸 JOB
         */

        (void) sllAddEdgeIntoWaitGraph( aContext->mSllHandle,
                                        SLL_REQUEST_GROUP_ID( gZtiSharedMemory,
                                                              aDispatcher->mId ),
                                                              KNL_ERROR_STACK(aEnv) );

        aContext->mLastTime = knlGetSystemTime();

        STL_TRY( sclEnqueueRequest( SLL_REQUEST_QUEUE( gZtiSharedMemory, aDispatcher->mId ),
                                    (sclHandle *)aContext->mSllHandle,
                                    SCL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztimRun( stlInt64      * aMsgKey,
                   stlInt32        aDispatcherIndex,
                   sllEnv        * aEnv )
{
    stlInt32                sState = 0;
    stlInt32                sSigFdNum = 0;
    stlInt32                sIdx;
    ztiContext            * sContext;
    ztiContext            * sUdsContext;
    stlTime                 sNowTime;
    stlTime                 sLastTime;
    stlTime                 sIdleStart;
    stlTime                 sIdleEnd;
    stlSockAddr             sFdSenderAddr;
    stlStatus               sRtn;
    ztiDispatcher         * sDispatcher;
    stlPollFdEx           * sPollFdEx = NULL;
    sllSessionEnv         * sSessionEnv;
    stlErrorStack         * sErrorStack = NULL;
    stlChar                 sFdSenderPath[STL_MAX_FILE_PATH_LENGTH];
    stlInt64                sShmKey = 0;

    sErrorStack = KNL_ERROR_STACK(aEnv);
    sSessionEnv = (sllSessionEnv*)KNL_SESS_ENV( aEnv );
    
    STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_SHARED_MEMORY_KEY,
                                      &sShmKey,
                                      KNL_ENV(aEnv) ) == STL_SUCCESS );

    STL_TRY( sllGetSharedMemory( &gZtiSharedMemory,
                                 sErrorStack )
             == STL_SUCCESS );

    STL_DASSERT( gZtiSharedMemory != NULL );

    sDispatcher = &gZtimDispatcher;

    STL_TRY( ztimInitialize( sDispatcher,
                             aDispatcherIndex,
                             aEnv ) == STL_SUCCESS );
    sState = 1;

    sDispatcher->mZlhlDispatcher->mMaxConnections = sDispatcher->mMaxContextCount;


    /* fd를 받을 uds를 생성한다 */
    sUdsContext = &sDispatcher->mContext[ZTI_INDEX_UDS];

    STL_TRY( ztimInitializeUds( sUdsContext,
                                sShmKey,
                                aDispatcherIndex,
                                aEnv )
             == STL_SUCCESS );
    sState = 2;

    sUdsContext->mUsed = STL_TRUE;
    sUdsContext->mIsBrokenSocket = STL_FALSE;

    sUdsContext->mPollFdEx.mPollFd = sUdsContext->mStlContext.mPollFd;
    sUdsContext->mPollFdEx.mUserContext = sUdsContext;

    STL_TRY( stlAddSockPollSet( &sDispatcher->mPollSet,
                                &sUdsContext->mPollFdEx,
                                sErrorStack )
             == STL_SUCCESS );
    sState = 3;

    /* send thread를 생성한다 */
    STL_TRY( stlCreateThread( &sDispatcher->mSendThread,
                              NULL,
                              ztisSendResponse,
                              sDispatcher,
                              sErrorStack ) == STL_SUCCESS );
    sState = 4;


    sDispatcher->mZlhlDispatcher->mAccept = STL_TRUE;

    /* ack, nack보낼 address 설정 */
    STL_TRY( ztimGetUnixDomainPath( sFdSenderPath,
                                    STL_SIZEOF(sFdSenderPath),
                                    STL_BALANCER_NAME,
                                    sShmKey,
                                    sErrorStack )
             == STL_SUCCESS );

    STL_TRY( stlSetSockAddr( &sFdSenderAddr,
                             STL_AF_UNIX,
                             NULL,
                             0,
                             sFdSenderPath,
                             sErrorStack ) == STL_SUCCESS );


    sLastTime = knlGetSystemTime();

    STL_TRY( stlThreadSelf( (stlThreadHandle *)&sDispatcher->mZlhlDispatcher->mThread,
                            sErrorStack )
             == STL_SUCCESS );

    (void) sslSetSessionProcessInfo( &sDispatcher->mZlhlDispatcher->mThread.mProc,
                                     "dispatcher",
                                     (sslSessionEnv*)sSessionEnv );

    /* gmaster에서 control하기 위해서 env, thread정보를 shared-memory에 설정 */
    sDispatcher->mZlhlDispatcher->mMainThreadEnv = aEnv;

    STL_TRY( stlWakeupExecProc( *aMsgKey,
                                KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    /**
     * stlSendErrorStackMsgQueue를 한번 호출하고 나면 aMsgKey를 0으로 설정한다.
     */
    *aMsgKey = 0;
    sIdleEnd = knlGetSystemTime();

    while( (gZtiSharedMemory->mExitFlag == STL_FALSE) &&
           (sDispatcher->mExitProcess == STL_FALSE) )
    {
        STL_INIT_ERROR_STACK( KNL_ERROR_STACK(aEnv) );

        KNL_CHECK_THREAD_CANCELLATION( KNL_ENV(aEnv) );

        STL_TRY( knlExecuteProcessEvent( KNL_ENV(aEnv) ) == STL_SUCCESS );

        sDispatcher->mZlhlDispatcher->mRecvStatus = SLL_STATUS_WAIT;

        sNowTime = knlGetSystemTime();
        if( sLastTime + STL_SET_SEC_TIME(1) < sNowTime )
        {
            STL_TRY( ztimProcessCleanup( sDispatcher,
                                         sNowTime,
                                         aEnv )
                     == STL_SUCCESS );

            sLastTime = sNowTime;
        }

        STL_DASSERT( sDispatcher->mPollSet.mMaxPollFdNum >= sDispatcher->mPollSet.mCurPollFdNum );

        /**
         * sIdleEnd 부터 sIdleStart까지는 BUSY time추가
         */
        sIdleStart = knlGetSystemTime();
        sDispatcher->mZlhlDispatcher->mRecvBusy += sIdleStart - sIdleEnd;

        if( stlPollPollSet( &sDispatcher->mPollSet,
                            ZTI_POLL_TIMEOUT,
                            &sSigFdNum,
                            &sPollFdEx,
                            sErrorStack )
            != STL_SUCCESS )
        {
            STL_TRY( stlGetLastErrorCode(sErrorStack) == STL_ERRCODE_TIMEDOUT );
            (void)stlPopError( sErrorStack );

            /**
             * sIdleStart 부터 sIdleEnd까지는 IDLE time추가
             */
            sIdleEnd = knlGetSystemTime();
            sDispatcher->mZlhlDispatcher->mRecvIdle += sIdleEnd - sIdleStart;

            continue;
        }

        /**
         * sIdleStart 부터 sIdleEnd까지는 IDLE time추가
         */
        sIdleEnd = knlGetSystemTime();
        sDispatcher->mZlhlDispatcher->mRecvIdle += sIdleEnd - sIdleStart;

        sDispatcher->mZlhlDispatcher->mRecvStatus = SLL_STATUS_RECEIVE;

        for( sIdx = 0; sIdx < sSigFdNum; sIdx++ )
        {
            KNL_CHECK_THREAD_CANCELLATION( KNL_ENV(aEnv) );

            if( sPollFdEx[sIdx].mUserContext == &sDispatcher->mContext[ZTI_INDEX_UDS] )
            {
                /**
                 * UDS event check
                 * sPollFd[0] 는 UDS 임
                 */
                STL_TRY( ztimProcessUds( &sDispatcher->mContext[ZTI_INDEX_UDS],
                                         &sFdSenderAddr,
                                         sDispatcher,
                                         gZtiSharedMemory->mNetBufferSize,
                                         aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                sContext = sPollFdEx[sIdx].mUserContext;

                STL_DASSERT( (sContext->mUsed == STL_TRUE) && (sContext->mIsBrokenSocket == STL_FALSE) );

                if( (sContext->mUsed == STL_FALSE) || (sContext->mIsBrokenSocket == STL_TRUE) )
                {
                    continue;
                }

                STL_TRY( ztimProcessRequest( sDispatcher,
                                             sContext,
                                             aEnv )
                         == STL_SUCCESS );
            }
        }
    }

    /* 정상 종료 처리 */
    sState = 3;
    sDispatcher->mExitProcess = STL_TRUE;

    (void)stlJoinThread( &sDispatcher->mSendThread, &sRtn, sErrorStack );

    sState = 2;
    (void)stlRemoveSockPollSet( &sDispatcher->mPollSet,
                                &sUdsContext->mPollFdEx,
                                sErrorStack );

    sState = 1;
    (void)ztimFinalizeUds( &sDispatcher->mContext[ZTI_INDEX_UDS], aEnv );

    sState = 0;
    (void)ztimFinalize( sDispatcher, aEnv );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 4:
            sDispatcher->mExitProcess = STL_TRUE;
            (void)stlJoinThread( &sDispatcher->mSendThread, &sRtn, sErrorStack );
        case 3:
            (void)stlRemoveSockPollSet( &sDispatcher->mPollSet,
                                        &sUdsContext->mPollFdEx,
                                        sErrorStack );
        case 2:
            (void)ztimFinalizeUds( &sDispatcher->mContext[ZTI_INDEX_UDS], aEnv );
        case 1:
            (void)ztimFinalize( sDispatcher, aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief 원격으로 부터 Packet을 async로 전송받는다.
 * @param[in] aDispatcher       dispatcher
 * @param[in] aContext          ztiContext
 * @param[out] aPeriodEnd       period end 여부
 * @param[in] aEnv              env
 */
stlStatus ztimRecvPacketAsync( ztiDispatcher    * aDispatcher,
                               ztiContext       * aContext,
                               stlBool          * aPeriodEnd,
                               sllEnv           * aEnv )
{
    stlBool                 sReceived;
    stlInt32                sOldReceivedSize = 0;

    *aPeriodEnd = STL_FALSE;

    STL_TRY_THROW( aContext->mSllHandle->mSclHandle.mSocketPeriod == STL_FALSE,
                   RAMP_SOCKET_CLOSE );

    while( (gZtiSharedMemory->mExitFlag == STL_FALSE) &&
           (aDispatcher->mExitProcess == STL_FALSE) )
    {
        KNL_CHECK_THREAD_CANCELLATION( KNL_ENV(aEnv) );

        if( aContext->mReadBuffer == NULL )
        {
            STL_TRY( sclGetSendBuffer( &aContext->mSllHandle->mIpc.mRequestIpc,
                                       aContext->mSllHandle->mIpc.mArrayAllocator,
                                       &aContext->mReadBuffer,
                                       SCL_ENV(aEnv) )
                     == STL_SUCCESS );

            aContext->mReadSize = 0;
        }

        sOldReceivedSize = aContext->mReadSize;

        STL_TRY( cmlRecvRequestAsync( &aContext->mStlContext,
                                      aContext->mReadBuffer,
                                      aContext->mSllHandle->mIpc.mArrayAllocator->mBodySize,    /* buffer size */
                                      &aContext->mReadSize,
                                      &aContext->mPayloadSize,
                                      &aContext->mPeriod,
                                      &sReceived,
                                      KNL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );

        aDispatcher->mZlhlDispatcher->mTotalRecvBytes += aContext->mReadSize - sOldReceivedSize;

        if( sReceived == STL_TRUE )
        {
            STL_TRY( sclCompleteSendBuffer( &aContext->mSllHandle->mIpc.mRequestIpc,
                                            aContext->mSllHandle->mIpc.mArrayAllocator,
                                            aContext->mReadBuffer,
                                            aContext->mReadSize,
                                            SCL_ENV(aEnv) )
                     == STL_SUCCESS );

            aDispatcher->mZlhlDispatcher->mTotalRecvUnits++;

            aContext->mReadSize = 0;
            aContext->mReadBuffer = NULL;

            /* 패킷을 모두 다 받은 경우 */
            if( aContext->mPeriod == CML_PERIOD_END )
            {
                /* 모든 프로토콜 sentence을 다 받음 */
                *aPeriodEnd = STL_TRUE;
                break;
            }
            else
            {
                /**
                 * CML_PERIOD_CONTINUE임으로 다음 packet을 받기 위해 loop한다.
                 */
                continue;
            }
        }
        else
        {
            /**
             * unit을 채우지 못한 채 recv가 끝났음.
             * socket에 data가 없음으로 break한다.
             */
            break;
        }
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_SOCKET_CLOSE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_EOF,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztimGetUnixDomainPath( stlChar       * aUdsPath,
                                 stlSize         aSize,
                                 stlChar       * aName,
                                 stlInt64        aKey,
                                 stlErrorStack * aErrorStack )
{
    stlSnprintf( aUdsPath,
                 aSize,
                 "%stmp%s%s-%s.%ld",
                 STL_PATH_SEPARATOR,
                 STL_PATH_SEPARATOR,
                 "goldilocks-unix",
                 aName,
                 aKey );

    return STL_SUCCESS;
}

stlStatus ztimGetMaxContextCount( ztiDispatcher   * aDispatcher,
                                  stlInt64        * aMaxContextCount,
                                  sllEnv          * aEnv )
{
    stlResourceLimitType    sLimitType;
    stlResourceLimit        sLimit;

    sLimitType = STL_RESOURCE_LIMIT_NOFILE;

    STL_TRY( stlGetResourceLimit( sLimitType,
                                  &sLimit,
                                  KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    if( (sLimit.mCurrent - 10) < gZtiSharedMemory->mConnections )
    {
        *aMaxContextCount = (sLimit.mCurrent - 10);

        knlLogMsg( NULL,
                   KNL_ENV(aEnv),
                   KNL_LOG_LEVEL_INFO,
                   "[DISPATCHER-%d] Maximum number of open files is too small (%ld) \n",
                   aDispatcher->mId,
                   sLimit.mCurrent );
    }
    else
    {
        *aMaxContextCount = gZtiSharedMemory->mConnections;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
