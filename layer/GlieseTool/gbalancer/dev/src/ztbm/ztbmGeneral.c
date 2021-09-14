/*******************************************************************************
 * ztbmGeneral.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztbmGeneral.c 11483 2014-02-28 03:18:44Z lym $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file ztbmGeneral.c
 * @brief Gliese Balancer general Routines
 */

#include <stl.h>
#include <knl.h>
#include <scl.h>
#include <sll.h>
#include <ztbDef.h>
#include <ztbm.h>

stlStatus ztbmRun( stlInt64      * aMsgKey,
                   sllEnv        * aEnv,
                   sllSessionEnv * aSessionEnv )
{
    stlInt32                sState     = 0;
    stlInt32                sSigFdNum;
    stlInt32                sIdx;
    stlContext              sUdsFdContext;
    stlContext              sUdsContext;
    stlPollSet              sPollSet;
    stlPollFdEx             sUdsPollFdEx;
    stlPollFdEx             sUdsFdPollFdEx;
    stlPollFdEx           * sPollFdExResult = NULL;
    stlTime                 sNowTime;
    stlTime                 sLastTime;
    stlInt64                sNextSequence = 0;
    ztbmDispatcher        * sDispatchers = NULL;
    ztbmContext           * sContextList = NULL;
    stlInt64                sShmKey = 0;
    sllSharedMemory       * sSharedMemory = NULL;
    stlInt64                sMaxContextCount;
    sllBalancer           * sBalancer;

    STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_SHARED_MEMORY_KEY,
                                      &sShmKey,
                                      KNL_ENV(aEnv) ) == STL_SUCCESS );

    STL_TRY( sllGetSharedMemory( &sSharedMemory,
                                 KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    sBalancer = &sSharedMemory->mBalancer;

    sBalancer->mEnv = aEnv;
    STL_TRY( ztbmGetMaxContextCount( &sMaxContextCount, aEnv ) == STL_SUCCESS );
    sBalancer->mMaxConnections = sMaxContextCount;


    STL_TRY( stlThreadSelf( (stlThreadHandle *)&sBalancer->mThread,
                            KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    (void) sslSetSessionProcessInfo( &sBalancer->mThread.mProc,
                                     "balancer",
                                     (sslSessionEnv*)aSessionEnv );

    /* recv fd 용 uds 생성 */
    STL_TRY( ztbmInitializeUds( &sUdsFdContext,
                                STL_BALANCER_FD_NAME,
                                sShmKey,
                                aEnv ) == STL_SUCCESS );
    sState = 1;

    /* non-block으로 만듬 */
    STL_TRY( stlSetSocketOption( STL_SOCKET_IN_CONTEXT(sUdsFdContext),
                                 STL_SOPT_SO_NONBLOCK,
                                 STL_YES,
                                 KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );
    
    /* recv ack용 uds생성 */
    STL_TRY( ztbmInitializeUds( &sUdsContext,
                                STL_BALANCER_NAME,
                                sShmKey,
                                aEnv ) == STL_SUCCESS );
    sState = 2;

    /* non-block으로 만듬 */
    STL_TRY( stlSetSocketOption( STL_SOCKET_IN_CONTEXT(sUdsContext),
                                 STL_SOPT_SO_NONBLOCK,
                                 STL_YES,
                                 KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY( stlCreatePollSet( &sPollSet,
                               2,
                               KNL_ERROR_STACK(aEnv) ) == STL_SUCCESS );
    sState = 3;

    sUdsFdPollFdEx.mPollFd = sUdsFdContext.mPollFd;
    sUdsFdPollFdEx.mUserContext = &sUdsFdContext;
    STL_TRY( stlAddSockPollSet( &sPollSet,
                                &sUdsFdPollFdEx,
                                KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );
    sState = 4;

    sUdsPollFdEx.mPollFd = sUdsContext.mPollFd;
    sUdsPollFdEx.mUserContext = &sUdsContext;
    STL_TRY( stlAddSockPollSet( &sPollSet,
                                &sUdsPollFdEx,
                                KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );
    sState = 5;

    STL_TRY( knlCreateDynamicMem( &aEnv->mAllocator,
                                  NULL, /* aParentMem */
                                  KNL_ALLOCATOR_GBALANCER,
                                  KNL_MEM_STORAGE_TYPE_HEAP,
                                  ZTB_HEAPMEM_CHUNK_SIZE,
                                  ZTB_HEAPMEM_CHUNK_SIZE,
                                  NULL,
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 6;

    STL_TRY( knlAllocDynamicMem( &aEnv->mAllocator,
                                 STL_SIZEOF(ztbmDispatcher) * KNL_PROPERTY_MAX(KNL_PROPERTY_DISPATCHERS),
                                 (void **) &sDispatchers,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 7;

    STL_TRY( knlAllocDynamicMem( &aEnv->mAllocator,
                                 STL_SIZEOF(ztbmContext) * sMaxContextCount,
                                 (void **) &sContextList,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 8;

    STL_TRY( ztbmInitializeContextList( sContextList,
                                        sMaxContextCount,
                                        aEnv )
             == STL_SUCCESS );
    sState = 9;

    /* dispatcher의 UDS 정보 설정 */
    for( sIdx = 0; sIdx < KNL_PROPERTY_MAX(KNL_PROPERTY_DISPATCHERS); sIdx++ )
    {
        sDispatchers[sIdx].mIsReady = STL_TRUE;
        /* set Dispatcher의 uds path*/
        STL_TRY( ztbmGetUnixDomainPathByIndex( sDispatchers[sIdx].mUdsPath,
                                               STL_MAX_FILE_PATH_LENGTH,
                                               STL_DISPATCHER_FD_NAME,
                                               sShmKey,
                                               sIdx,
                                               KNL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );
    }


    sLastTime = knlGetSystemTime();

    STL_TRY( stlWakeupExecProc( *aMsgKey,
                                KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    /**
     * stlSendErrorStackMsgQueue를 한번 호출하고 나면 aMsgKey를 0으로 설정한다.
     */
    *aMsgKey = 0;

    while( sSharedMemory->mExitFlag == STL_FALSE )
    {
        STL_INIT_ERROR_STACK( KNL_ERROR_STACK(aEnv) );

        KNL_CHECK_THREAD_CANCELLATION( KNL_ENV(aEnv) );

        STL_TRY( knlExecuteProcessEvent( KNL_ENV(aEnv) ) == STL_SUCCESS );

        sBalancer->mStatus = SLL_STATUS_WAIT;

        sNowTime = knlGetSystemTime();

        if( sLastTime + STL_SET_SEC_TIME(1) < sNowTime )
        {
            /**
             * 1초마다dispatcher의 socketCount 값을 가져와서 sDispatchers[sIdx].mSocketCount를 업데이트 한다.
             */
            for( sIdx = 0; sIdx < sSharedMemory->mDispatchers; sIdx++ )
            {
                sDispatchers[sIdx].mSocketCount = sSharedMemory->mDispatcher[sIdx].mSocketCount;
                sDispatchers[sIdx].mIsReady = sSharedMemory->mDispatcher[sIdx].mAccept;
            }

            /**
             *  shared-server 관련 fd를 관리한다.
             */
            (void)ztbmCleanupFd( sContextList,
                                 sMaxContextCount,
                                 aEnv );

            if( ztbmRetryForwardFd( sDispatchers,
                                    &sUdsContext,
                                    sContextList,
                                    sMaxContextCount,
                                    sSharedMemory,
                                    aEnv ) == STL_FAILURE )
            {
                knlLogMsg( NULL,
                           KNL_ENV(aEnv),
                           KNL_LOG_LEVEL_INFO,
                           "[BALANCER] failed to forward FD (%u)\n",
                           stlGetLastErrorCode( KNL_ERROR_STACK(aEnv) ) );
            }

            sLastTime = sNowTime;
        }

        if( stlPollPollSet( &sPollSet,
                            STL_SET_SEC_TIME(1),
                            &sSigFdNum,
                            &sPollFdExResult,
                            KNL_ERROR_STACK(aEnv) ) != STL_SUCCESS )
        {
            STL_TRY( stlGetLastErrorCode(KNL_ERROR_STACK(aEnv)) == STL_ERRCODE_TIMEDOUT );
            (void)stlPopError( KNL_ERROR_STACK(aEnv) );

            continue;
        }

        sBalancer->mStatus = SLL_STATUS_RUN;

        for( sIdx = 0; sIdx < sSigFdNum; sIdx++ )
        {
            if( sPollFdExResult[sIdx].mUserContext == &sUdsFdContext )
            {
                sNextSequence++;

                STL_TRY( ztbmProcessBalance( sDispatchers,
                                             sContextList,
                                             sMaxContextCount,
                                             sNextSequence,
                                             &sUdsFdContext,
                                             sSharedMemory,
                                             aEnv )
                         == STL_SUCCESS );
            }
            else if( sPollFdExResult[sIdx].mUserContext == &sUdsContext )
            {
                STL_TRY( ztbmProcessAck( sDispatchers,
                                         sContextList,
                                         &sUdsContext,
                                         aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                STL_DASSERT( 0 );
            }
        }
    }

    sState = 8;
    (void)ztbmFinalizeContextList( sContextList, aEnv );

    sState = 7;
    (void)knlFreeDynamicMem( &aEnv->mAllocator,
                             (void *)sContextList,
                             KNL_ENV(aEnv) );

    sState = 6;
    (void)knlFreeDynamicMem( &aEnv->mAllocator,
                             (void *)sDispatchers,
                             KNL_ENV(aEnv) );

    sState = 5;
    (void)knlDestroyDynamicMem( &aEnv->mAllocator,
                                KNL_ENV(aEnv) );

    sState = 4;
    (void)stlRemoveSockPollSet( &sPollSet,
                                &sUdsPollFdEx,
                                KNL_ERROR_STACK(aEnv) );

    sState = 3;
    (void)stlRemoveSockPollSet( &sPollSet,
                                &sUdsFdPollFdEx,
                                KNL_ERROR_STACK(aEnv) );

    sState = 2;
    (void)stlDestroyPollSet( &sPollSet, KNL_ERROR_STACK(aEnv) );

    sState = 1;
    (void)ztbmFinalizeUds( &sUdsContext, aEnv );

    sState = 0;
    (void)ztbmFinalizeUds( &sUdsFdContext, aEnv );

    return STL_SUCCESS;

    STL_FINISH;


    switch( sState )
    {
        case 9:
            (void)ztbmFinalizeContextList( sContextList, aEnv );
        case 8:
            (void)knlFreeDynamicMem( &aEnv->mAllocator,
                                     (void *)sContextList,
                                     KNL_ENV(aEnv) );

        case 7:
            (void)knlFreeDynamicMem( &aEnv->mAllocator,
                                     (void *)sDispatchers,
                                     KNL_ENV(aEnv) );

        case 6:
            (void)knlDestroyDynamicMem( &aEnv->mAllocator,
                                        KNL_ENV(aEnv) );
        case 5:
            (void)stlRemoveSockPollSet( &sPollSet,
                                        &sUdsPollFdEx,
                                        KNL_ERROR_STACK(aEnv) );
        case 4:
            (void)stlRemoveSockPollSet( &sPollSet,
                                        &sUdsFdPollFdEx,
                                        KNL_ERROR_STACK(aEnv) );
        case 3:
            (void)stlDestroyPollSet( &sPollSet,
                                     KNL_ERROR_STACK(aEnv) );
        case 2:
            (void)stlFinalizeContext( &sUdsContext,
                                      KNL_ERROR_STACK(aEnv) );
        case 1:
            (void)stlFinalizeContext( &sUdsFdContext,
                                      KNL_ERROR_STACK(aEnv) );
        default:
            break;
    }

    return STL_FAILURE;
}



stlStatus ztbmGetDispatcher( ztbmDispatcher    * aDispatchers,
                             stlInt32          * aDispatcherIndex,
                             sllSharedMemory   * aSharedMemory,
                             sllEnv            * aEnv )
{
    stlInt32            sIdx;
    stlInt32            sMin;
    ztbmDispatcher    * sDispatcher;

    /* dispatcher의 socket개수가 mConnections 보다 작은 경우 할당 */
    sMin = aSharedMemory->mConnections;
    *aDispatcherIndex = -1;

    /* session이 적게 붙어 있는 Dispatcher server를 찾는다. */
    for( sIdx = 0; sIdx < aSharedMemory->mDispatchers; sIdx++ )
    {
        sDispatcher = &aDispatchers[sIdx];

        if( sDispatcher->mIsReady == STL_TRUE &&
            sDispatcher->mSocketCount < sMin )
        {
            sMin = sDispatcher->mSocketCount;
            *aDispatcherIndex = sIdx;
        }
    }

    if( *aDispatcherIndex == -1 )
    {
        knlLogMsg( NULL,
                   KNL_ENV(aEnv),
                   KNL_LOG_LEVEL_INFO,
                   "[BALANCER] dispatcher is busy \n" );
    }


    return STL_SUCCESS;
}


stlStatus ztbmGetUnixDomainPath( stlChar       * aUdsPath,
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


stlStatus ztbmGetUnixDomainPathByIndex( stlChar       * aUdsPath,
                                        stlSize         aSize,
                                        stlChar       * aName,
                                        stlInt64        aKey,
                                        stlInt32        aIndex,
                                        stlErrorStack * aErrorStack )
{
    stlSnprintf( aUdsPath,
                 aSize,
                 "%stmp%s%s-%s.%ld.%d",
                 STL_PATH_SEPARATOR,
                 STL_PATH_SEPARATOR,
                 "goldilocks-unix",
                 aName,
                 aKey,
                 aIndex );

    return STL_SUCCESS;
}


stlStatus ztbmProcessBalance( ztbmDispatcher   * aDispatchers,
                              ztbmContext      * aContextList,
                              stlInt64           aMaxContextCount,
                              stlInt64           aNextSequence,
                              stlContext       * aUdsContext,
                              sllSharedMemory  * aSharedMemory,
                              sllEnv           * aEnv )
{
    stlInt32                sState = 0;
    stlContext              sNewContext;
    stlChar                 sListenerPath[STL_MAX_FILE_PATH_LENGTH];
    stlSockAddr             sUdsListenerAddr;
    stlInt32                sIdx = 0;
    cmlUdsForwardFd         sForwardFd;
    sllBalancer           * sBalancer;
    stlInt32                sUsedCount = 0;
    ztbmContext           * sFoundContext = NULL;

    sBalancer = &aSharedMemory->mBalancer;

    stlMemset( &sForwardFd, 0x00, STL_SIZEOF(sForwardFd) );

    if( cmlRecvFdFrom( aUdsContext,
                       &sNewContext,
                       &sForwardFd,
                       KNL_ERROR_STACK( aEnv ) ) == STL_FAILURE )
    {
        knlLogMsg( NULL,
                   KNL_ENV(aEnv),
                   KNL_LOG_LEVEL_INFO,
                   "[BALANCER] failed to receive FD \n" );

        (void)stlPopError( KNL_ERROR_STACK( aEnv ) );
        STL_THROW( RAMP_SUCCESS );
    }
    sState = 1;

    sBalancer->mConnections++;

    /* ack, nack보낼 address 설정 */
    STL_TRY( ztbmGetUnixDomainPath( sListenerPath,
                                    STL_SIZEOF(sListenerPath),
                                    STL_LISTENER_NAME,
                                    sForwardFd.mKey,
                                    KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    STL_TRY( stlSetSockAddr( &sUdsListenerAddr,
                             STL_AF_UNIX,
                             NULL,
                             0,
                             sListenerPath,
                             KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    /* aContextList의 비어 있는곳에 stlContet 저장 */
    for( sIdx = 0; sIdx < aMaxContextCount; sIdx++ )
    {
        STL_DASSERT( aContextList[sIdx].mIdx == sIdx );

        if( aContextList[sIdx].mUsed == STL_FALSE )
        {
            if( sFoundContext == NULL )
            {
                aContextList[sIdx].mUsed = STL_TRUE;
                aContextList[sIdx].mForwarding = STL_FALSE;
                aContextList[sIdx].mContext = sNewContext;
                aContextList[sIdx].mSequence = aNextSequence;
                aContextList[sIdx].mTime = knlGetSystemTime();

                aContextList[sIdx].mMajorVersion = sForwardFd.mMajorVersion;
                aContextList[sIdx].mMinorVersion = sForwardFd.mMinorVersion;
                aContextList[sIdx].mPatchVersion = sForwardFd.mPatchVersion;

                sFoundContext = &aContextList[sIdx];
                sUsedCount++;
            }
        }
        else
        {
            sUsedCount++;
        }
    }

    STL_TRY_THROW( sFoundContext != NULL, RAMP_SUCCESS_OVERFLOW_FD );

    sBalancer->mCurrentConnections = sUsedCount;
    if( sBalancer->mCurrentConnections > sBalancer->mConnectionsHighwater )
    {
        sBalancer->mConnectionsHighwater = sBalancer->mCurrentConnections;
    }

    /**
     * ack 를 보낸다
     */
    if( cmlSendAckTo( aUdsContext,
                      &sUdsListenerAddr,
                      sForwardFd.mSenderStartTime,
                      sForwardFd.mSequence,
                      sForwardFd.mIdx,
                      KNL_ERROR_STACK( aEnv ) ) == STL_FAILURE )
    {
        knlLogMsg( NULL,
                   KNL_ENV(aEnv),
                   KNL_LOG_LEVEL_INFO,
                   "[BALANCER] failed to send ACK \n" );

        /* listener로 ack (or nack) send하다 실패면 성공 처리 한다. */
        (void)stlPopError( KNL_ERROR_STACK( aEnv ) );
    }


    /**
     * fd dispatcher로 전달
     * fd 전달에 실패해도 ztbmForwardFd함수는 STL_SUCCESS 리턴됨.
     * fd전달 성공시 aFdInfo->mForwarding flag만 설정됨.
     */
    STL_TRY( ztbmForwardFdToDispatcher( aDispatchers,
                                        sFoundContext,
                                        aUdsContext,
                                        aSharedMemory,
                                        aEnv )
             == STL_SUCCESS );

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_SUCCESS_OVERFLOW_FD )
    {
        knlLogMsg( NULL,
                   KNL_ENV(aEnv),
                   KNL_LOG_LEVEL_INFO,
                   "[BALANCER] context full\n" );

        /* aContextList가 full임 */
        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZTBE_ERRCODE_OVERFLOW_FD,
                      NULL,
                      KNL_ERROR_STACK(aEnv)  );

        if( cmlSendAckTo( aUdsContext,
                          &sUdsListenerAddr,
                          sForwardFd.mSenderStartTime,
                          sForwardFd.mSequence,
                          sForwardFd.mIdx,
                          KNL_ERROR_STACK( aEnv ) ) == STL_FAILURE )
        {
            knlLogMsg( NULL,
                       KNL_ENV(aEnv),
                       KNL_LOG_LEVEL_INFO,
                       "[BALANCER] failed to send ACK \n" );

            /* listener로 ack (or nack) send하다 실패면 성공 처리 한다. */
            (void)stlPopError( KNL_ERROR_STACK( aEnv ) );
        }
    }

    switch( sState )
    {
        case 2:
            aContextList[sIdx].mUsed = STL_FALSE;
            aContextList[sIdx].mSequence = -1;
            aContextList[sIdx].mContext.mPollFd.mSocketHandle = -1;
        case 1:
            STL_DASSERT( sNewContext.mPollFd.mSocketHandle != -1 );
            (void)stlFinalizeContext( &sNewContext, KNL_ERROR_STACK( aEnv ) );
            sNewContext.mPollFd.mSocketHandle = -1;
        default:
            break;
    }

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            aContextList[sIdx].mUsed = STL_FALSE;
            aContextList[sIdx].mSequence = -1;
            aContextList[sIdx].mContext.mPollFd.mSocketHandle = -1;
        case 1:
            STL_DASSERT( sNewContext.mPollFd.mSocketHandle != -1 );
            (void)stlFinalizeContext( &sNewContext, KNL_ERROR_STACK( aEnv ) );
            sNewContext.mPollFd.mSocketHandle = -1;
        default:
            break;
    }

    return STL_FAILURE;
}


stlStatus ztbmProcessAck( ztbmDispatcher   * aDispatchers,
                          ztbmContext      * aContextList,
                          stlContext       * aUdsContext,
                          sllEnv           * aEnv )
{
    stlSockAddr             sDispatcherAddr;
    cmlUdsPacket            sPacket;
    cmlUdsAck             * sAck;

    if( cmlRecvFrom( aUdsContext,
                     &sDispatcherAddr,
                     &sPacket,
                     KNL_ERROR_STACK( aEnv ) ) == STL_FAILURE )
    {
        knlLogMsg( NULL,
                   KNL_ENV(aEnv),
                   KNL_LOG_LEVEL_WARNING,
                   "[BALANCER] failed to receive ACK \n" );

        (void)stlPopError( KNL_ERROR_STACK( aEnv ) );

        STL_THROW( RAMP_SUCCESS );
    }

    STL_DASSERT( sPacket.mCmd.mCommon.mCmdType == CML_UDS_CMD_ACK );

    sAck = &sPacket.mCmd.mAck;

    if( aContextList[sAck->mIdx].mSequence != sAck->mSequence )
    {
        /**
         * ack(nack)받은 context가 이미 close되었거나 재사용되었음.
         * 이때 ack(nack)는 무시함.
         */

        STL_THROW( RAMP_SUCCESS );
    }

    STL_DASSERT( aContextList[sAck->mIdx].mUsed == STL_TRUE );
    STL_DASSERT( aContextList[sAck->mIdx].mIdx == sAck->mIdx );

    if( stlGetErrorStackDepth(&sAck->mErrorStack) == 0 )
    {
        stlFinalizeContext( &aContextList[sAck->mIdx].mContext, KNL_ERROR_STACK( aEnv ) );
        aContextList[sAck->mIdx].mContext.mPollFd.mSocketHandle = -1;
        aContextList[sAck->mIdx].mUsed = STL_FALSE;
        aContextList[sAck->mIdx].mSequence = -1;
    }
    else
    {
        /**
         * nack 받으면 mForwarding = STL_FALSE 설정만 함.
         * timer에서 재시도 하거나 timeout 처리됨
         */
        aContextList[sAck->mIdx].mForwarding = STL_FALSE;
    }

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;
}


stlStatus ztbmForwardFdToDispatcher( ztbmDispatcher   * aDispatchers,
                                     ztbmContext      * aContext,
                                     stlContext       * aUdsContext,
                                     sllSharedMemory  * aSharedMemory,
                                     sllEnv           * aEnv )
{
    stlInt32                sDispatcherIdx;
    stlSockAddr             sUdsTarget;

    STL_TRY( ztbmGetDispatcher( aDispatchers,
                                &sDispatcherIdx,
                                aSharedMemory,
                                aEnv )
             == STL_SUCCESS );

    /**
     * dispatcher를 못찾으면 그냥 성공 처리하고
     * 나중에 retry로 처리한다.
     */
    STL_TRY_THROW( sDispatcherIdx != -1, RAMP_SUCCESS );

    STL_TRY( stlSetSockAddr( &sUdsTarget,
                             STL_AF_UNIX,
                             NULL,
                             0,
                             aDispatchers[sDispatcherIdx].mUdsPath,
                             KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    if( ztbmSendFdTo( aUdsContext,
                      &sUdsTarget,
                      aContext,
                      KNL_ERROR_STACK( aEnv ) ) == STL_FAILURE )
    {
        knlLogMsg( NULL,
                   KNL_ENV(aEnv),
                   KNL_LOG_LEVEL_WARNING,
                   "[BALANCER] failed to forward FD (%u) \n",
                   stlGetLastErrorCode( KNL_ERROR_STACK(aEnv) ) );

        (void)stlPopError( KNL_ERROR_STACK( aEnv ) );
        /* send 오류면 그냥 성공 처리 */
    }
    else
    {
        aContext->mForwarding = STL_TRUE;
        aDispatchers[sDispatcherIdx].mSocketCount++;
    }

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztbmCleanupFd( ztbmContext      * aContextList,
                         stlInt64           aMaxContextCount,
                         sllEnv           * aEnv )
{
    stlInt32 sIdx = 0;
    stlTime  sNow = knlGetSystemTime();

    for( sIdx = 0; sIdx < aMaxContextCount; sIdx++ )
    {
        if( (aContextList[sIdx].mUsed == STL_TRUE) &&
            (aContextList[sIdx].mTime + ZTB_FD_TIMEOUT < sNow) )
        {
            knlLogMsg( NULL,
                       KNL_ENV(aEnv),
                       KNL_LOG_LEVEL_WARNING,
                       "[BALANCER] disconnect connection\n" );

            STL_DASSERT( aContextList[sIdx].mContext.mPollFd.mSocketHandle != -1 );
            (void)stlFinalizeContext( &aContextList[sIdx].mContext, KNL_ERROR_STACK( aEnv ) );
            aContextList[sIdx].mContext.mPollFd.mSocketHandle = -1;
            aContextList[sIdx].mUsed = STL_FALSE;
            aContextList[sIdx].mSequence = -1;
        }
    }

    return STL_SUCCESS;
}


stlStatus ztbmRetryForwardFd( ztbmDispatcher   * aDispatchers,
                              stlContext       * aUdsContext,
                              ztbmContext      * aContextList,
                              stlInt64           aMaxContextCount,
                              sllSharedMemory  * aSharedMemory,
                              sllEnv           * aEnv )
{
    stlInt32 sIdx = 0;

    for( sIdx = 0; sIdx < aMaxContextCount; sIdx++ )
    {
        if( (aContextList[sIdx].mUsed == STL_TRUE) &&
            (aContextList[sIdx].mForwarding == STL_FALSE) )
        {
            STL_TRY( ztbmForwardFdToDispatcher( aDispatchers,
                                                &aContextList[sIdx],
                                                aUdsContext,
                                                aSharedMemory,
                                                aEnv )
                     == STL_SUCCESS );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztbmInitializeContextList( ztbmContext      * aContextList,
                                     stlInt64           aMaxContextCount,
                                     sllEnv           * aEnv )
{
    stlInt32 sIdx = 0;

    /* aContextList 초기화 */
    for( sIdx = 0; sIdx < aMaxContextCount; sIdx++ )
    {
        aContextList[sIdx].mIdx = sIdx;
        aContextList[sIdx].mUsed = STL_FALSE;
        aContextList[sIdx].mSequence = -1;
    }

    return STL_SUCCESS;
}

stlStatus ztbmFinalizeContextList( ztbmContext      * aContextList,
                                   sllEnv           * aEnv )
{
    /* do nothing */

    return STL_SUCCESS;
}



stlStatus ztbmInitializeUds( stlContext      * aUds,
                             stlChar         * aName,
                             stlInt64          aKey,
                             sllEnv          * aEnv )
{
    stlChar                 sUdsPath[STL_MAX_FILE_PATH_LENGTH]; /* fd receiver 용 uds */

    /* fd receiver 용 uds 생성 */
    STL_TRY( ztbmGetUnixDomainPath( sUdsPath,
                                    STL_SIZEOF(sUdsPath),
                                    aName,
                                    aKey,
                                    KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    if( stlInitializeContext( aUds,
                              STL_AF_UNIX,
                              STL_SOCK_DGRAM,
                              0,
                              STL_FALSE,
                              sUdsPath,
                              0,
                              KNL_ERROR_STACK(aEnv) ) == STL_SUCCESS )
    {
        (void) knlLogMsg( NULL,
                          KNL_ENV(aEnv),
                          KNL_LOG_LEVEL_INFO,
                          "[BALANCER] Unix Domain Socket [%s] is created \n",
                          sUdsPath );
    }
    else
    {
        (void) knlLogMsg( NULL,
                          KNL_ENV(aEnv),
                          KNL_LOG_LEVEL_WARNING,
                          "[BALANCER] failed to create Unix Domain Socket [%s] \n",
                          sUdsPath );

        STL_TRY( STL_FALSE );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztbmFinalizeUds( stlContext      * aUds,
                           sllEnv          * aEnv )
{
    (void)stlFinalizeContext( aUds, KNL_ERROR_STACK(aEnv));

    return STL_SUCCESS;
}


stlStatus ztbmSendFdTo( stlContext      * aUdsContext,
                        stlSockAddr     * aTargetAddr,
                        ztbmContext     * aContext,
                        stlErrorStack   * aErrorStack )
{
    STL_TRY( cmlSendFdTo( aUdsContext,
                          aTargetAddr,
                          &aContext->mContext,
                          0,     /* key */
                          0,     /* sender start time */
                          aContext->mSequence,
                          aContext->mIdx,
                          aContext->mMajorVersion,
                          aContext->mMinorVersion,
                          aContext->mPatchVersion,
                          aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



stlStatus ztbmGetMaxContextCount( stlInt64        * aMaxContextCount,
                                  sllEnv          * aEnv )
{
    stlResourceLimitType    sLimitType;
    stlResourceLimit        sLimit;

    sLimitType = STL_RESOURCE_LIMIT_NOFILE;

    STL_TRY( stlGetResourceLimit( sLimitType,
                                  &sLimit,
                                  KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    if( (sLimit.mCurrent - 10) < ZTB_MAX_CONTEXT_COUNT )
    {
        *aMaxContextCount = (sLimit.mCurrent - 10);

        knlLogMsg( NULL,
                   KNL_ENV(aEnv),
                   KNL_LOG_LEVEL_INFO,
                   "[BALANCER] Maximum number of open files is too small (%ld) \n",
                   sLimit.mCurrent );
    }
    else
    {
        *aMaxContextCount = ZTB_MAX_CONTEXT_COUNT;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
