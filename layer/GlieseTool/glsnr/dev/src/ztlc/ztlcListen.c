/*******************************************************************************
 * ztlcListen.c
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

/**
 * @file ztlcListen.c
 * @brief Gliese Listener routines
 */

#include <stl.h>
#include <cml.h>
#include <ztlDef.h>
#include <ztlc.h>
#include <ztlm.h>

extern stlLogger        gZtlLogger;

/*
 * 이 파일에서 구현된 함수는 process가 죽어야 하는 상황이 아니면 error를 리턴하지 않는다.
 */

/**
 * @brief Gliese Listener routines
 */

/**
 * @brief 이전버전packet의 binary data
 *
 * 이전 버전(2.1.0)과 호환성 문제 해결 방법
 * - 문제점
 * 1. header가 달라져서 이전버전의 gsqlnet이 접속시 parsing이 안된다.
 * 2. 이전 버전은 첫 packet이 handshake이고 현재는 preHandshake이다.
 *
 * 이런 문제로 이전 버전인지 여부는 binary data로 비교하여 처리하고
 * 이전버전이면 response도 저장된 binary를 보낸다. (현재 버전에서 이전버전 packet생성 불가능)
 * sVersion210Response의 내용은 handshake에 대한 response로 Invalid communication protocol 이다.
 *
 * TODO : goldilocks 2.1.0과 호환성이 필요 없어지면 sVersion210Request, sVersion210Response과 관련된 내용은 삭제한다.
 */
stlInt32    gZtlcVersion210RequestSize = 32;
stlChar     gZtlcVersion210Request[] = {
        0x01, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x01, 0x03, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00 };

stlChar     gZtlcVersion210Response[] = {
        0x00, 0x00, 0x00, 0x00, 0x2c, 0x00, 0x00, 0x00, 0x01, 0x02, 0x01, 0x00, 0x32, 0x02, 0x01, 0xc1,
        0x5d, 0x00, 0x00, 0x1e, 0x49, 0x6e, 0x76, 0x61, 0x6c, 0x69, 0x64, 0x20, 0x63, 0x6f, 0x6d, 0x6d,
        0x75, 0x6e, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x70, 0x72, 0x6f, 0x74, 0x6f, 0x63,
        0x6f, 0x6c, 0x00, 0xff };


stlStatus ztlcInitialize( ztlcListener       * aListener,
                          stlErrorStack      * aErrorStack )
{
    stlInt32    sState = 0;
    stlInt32    sIdx = 0;

    STL_TRY( ztlcGetMaxContextCount( &aListener->mMaxContextCount, aErrorStack ) == STL_SUCCESS );

    STL_TRY( stlAllocHeap( (void**)&aListener->mContext,
                           STL_SIZEOF(ztlcContext) * aListener->mMaxContextCount,
                           aErrorStack ) == STL_SUCCESS );
    sState = 1;

    stlMemset( aListener->mContext, 0x00, STL_SIZEOF(ztlcContext) * aListener->mMaxContextCount );

    for( sIdx = 0; sIdx < aListener->mMaxContextCount; sIdx++ )
    {
        aListener->mContext[sIdx].mIdx = sIdx;
    }

    STL_TRY( stlCreatePollSet( &aListener->mPollSet,
                               aListener->mMaxContextCount,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)stlDestroyPollSet( &aListener->mPollSet, aErrorStack );
        case 1:
            (void)stlFreeHeap( aListener->mContext );
        default :
            break;
    }

    return STL_FAILURE;
}



stlStatus ztlcFinalize( ztlcListener       * aListener,
                        stlErrorStack      * aErrorStack )
{
    stlDestroyPollSet( &aListener->mPollSet, aErrorStack );
    stlFreeHeap( aListener->mContext );

    return STL_SUCCESS;
}


/**
 * Context를 초기화한다
 */
stlStatus ztlcInitializeContext( ztlcListener      * aListener,
                                 ztlcContext       * aContext,
                                 stlErrorStack     * aErrorStack )
{
    aContext->mLastTime = stlNow();

    aContext->mUsed = STL_TRUE;

    aContext->mReceivedPreHandshake = STL_FALSE;

    aContext->mSessionType = CML_SESSION_NONE;

    aContext->mRecvedSize = 0;

    aContext->mSequence = aListener->mNextSequence;

    aListener->mNextSequence++;

    return STL_SUCCESS;
}

/* 자료구조에서 context를 제거한다 */
stlStatus ztlcFinalizeContext( ztlcContext     * aContext,
                               stlErrorStack   * aErrorStack )
{
    aContext->mLastTime = 0;
    aContext->mRecvedSize = 0;
    aContext->mUsed = STL_FALSE;
    aContext->mReceivedPreHandshake = STL_FALSE;
    aContext->mIsFdSending = STL_FALSE;

    aContext->mSessionType = CML_SESSION_NONE;

    return STL_SUCCESS;
}



stlStatus ztlcCloseContext( stlPollSet     * aPollSet,
                            ztlcContext    * aContext,
                            stlErrorStack  * aErrorStack )
{
    /**
     * listener는 fd를 가지고 있으면서 SockPollSet에는 fd가 없는 경우가 있음으로
     * stlRemoveSockPollSet에 assert하지 않는다.
     */
    if( aContext->mPollFdEx.mPollFd.mSocketHandle != -1 )
    {
        (void)stlRemoveSockPollSet( aPollSet,
                                    &aContext->mPollFdEx,
                                    aErrorStack );
        aContext->mPollFdEx.mPollFd.mSocketHandle = -1;
    }

    STL_DASSERT( aContext->mStlContext.mPollFd.mSocketHandle != -1 );
    (void)stlFinalizeContext( &aContext->mStlContext, aErrorStack);
    aContext->mStlContext.mPollFd.mSocketHandle = -1;

    return STL_SUCCESS;
}


stlStatus ztlcSendPreHandshakeResult( ztlcListener     * aListener,
                                      stlContext       * aContext,
                                      stlErrorStack    * aErrorStack )
{
    stlInt32                sState = 0;
    cmlHandle               sCmlHandle;
    cmlProtocolSentence     sProtocolSentence;

    /**
     * preHandshake는 little endian으로 보내야만 한다.
     */
    STL_TRY( cmlInitializeHandle( &sCmlHandle,
                                  aContext,
                                  STL_LITTLE_ENDIAN,
                                  aErrorStack )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( cmlInitializeProtocolSentence( &sCmlHandle,
                                            &sProtocolSentence,
                                            &aListener->mMemoryMgr,
                                            aErrorStack )
             == STL_SUCCESS );
    sState = 2;


    if( (stlGetErrorStackDepth( aErrorStack ) > 0) &&
        ( stlGetLastErrorData(aErrorStack)->mErrorLevel != STL_ERROR_LEVEL_WARNING ) )
    {
        STL_TRY( cmlWriteErrorResult( &sCmlHandle,
                                      CML_COMMAND_PRE_HANDSHAKE,
                                      aErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( cmlWritePreHandshakeResult( &sCmlHandle,
                                             aErrorStack )
                 == STL_SUCCESS );
    }

    /* cmlSendPreHandshakeResult 실패해도 성공 처리 */
    if( cmlSendPacketInternal( &sCmlHandle,
                               CML_PERIOD_END,
                               aErrorStack ) == STL_FAILURE )
    {
        (void)stlPopError( aErrorStack );
    }

    sState = 1;
    (void)cmlFinalizeProtocolSentence( &sProtocolSentence,  aErrorStack );

    sState = 0;
    (void)cmlFinalizeHandle( &sCmlHandle, aErrorStack);

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)cmlFinalizeProtocolSentence( &sProtocolSentence,  aErrorStack );
        case 1:
            (void)cmlFinalizeHandle( &sCmlHandle, aErrorStack);
            break;
        default:
            break;
    }

    return STL_FAILURE;
}


stlStatus ztlcGetFdReceiverAddr( ztlcListener     * aListener,
                                 stlErrorStack    * aErrorStack )
{
    stlChar                 sUdsPath[STL_MAX_FILE_PATH_LENGTH];
    cmlShmStaticFileInfo    sShmInfo;

    /* mValidFdReceiverAddr == STL_FALSE 이면 shm정보를 다시 가져온다 */
    if( aListener->mValidFdReceiverAddr == STL_FALSE )
    {
        if( cmlLoadStaticAreaInfo( &sShmInfo,
                                   aErrorStack ) == STL_FAILURE )
        {
            (void)stlLogMsg( &gZtlLogger,
                             aErrorStack,
                             "[LISTENER] Load StaticArea Info failure \n" );

            (void)stlPopError( aErrorStack );

            /*
             * shm정보를 가져오지 못하면 그냥 성공 처리한다
             * 나중에 재시도 처리 한다.
             */

            STL_THROW( RAMP_SUCCESS );
        }

        STL_TRY( ztlcGetUnixDomainPath( sUdsPath,
                                        STL_SIZEOF(sUdsPath),
                                        STL_BALANCER_FD_NAME,
                                        sShmInfo.mKey,
                                        aErrorStack )
                 == STL_SUCCESS );

        STL_TRY( stlSetSockAddr( &aListener->mFdReceiverAddr,
                                 STL_AF_UNIX,
                                 NULL,
                                 0,
                                 sUdsPath,
                                 aErrorStack ) == STL_SUCCESS );

        aListener->mValidFdReceiverAddr = STL_TRUE;
    }

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * data주고 받은지 timeout이 지나면 close 처리한다.
 * 1초 마다 호출됨.
 */
stlStatus ztlcProcessCleanup( ztlcListener     * aListener,
                              stlTime            aNow,
                              stlErrorStack    * aErrorStack )
{
    stlInt32            sIdx;
    ztlcContext       * sContext;

    /* 모든 연결된 session에 대해서 timeout check */
    for( sIdx = ZTLC_INDEX_SESSION; sIdx < aListener->mMaxContextCount; sIdx++ )
    {
        STL_INIT_ERROR_STACK( aErrorStack );

        sContext = &aListener->mContext[sIdx];

        if( sContext->mUsed == STL_FALSE )
        {
            continue;
        }

        /**
         * 사용중이고 timeout이면
         */
        if( (gZtlConfig.mTimeout > 0) &&
            (sContext->mLastTime + STL_SET_SEC_TIME(gZtlConfig.mTimeout) < aNow) )
        {
            STL_DASSERT( sContext->mStlContext.mPollFd.mSocketHandle != -1 );

            (void)stlLogMsg( &gZtlLogger,
                             aErrorStack,
                             "[LISTENER] fd cleanup \n" );

            /**
             * timeout 처리 : handshake중 timeout, handshake는 완료했으나 dispatcher를 못찾는 경우
             * error를 client로 보내고 socket close한다
             */
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          ZTLE_ERRCODE_TIMEDOUT,
                          NULL,
                          aErrorStack );

            (void)ztlcSendPreHandshakeResult( aListener,
                                              &sContext->mStlContext,
                                              aErrorStack );

            (void)stlPopError( aErrorStack );

            (void)ztlcCloseContext( &aListener->mPollSet,
                                    sContext,
                                    aErrorStack );

            (void)ztlcFinalizeContext( sContext,
                                       aErrorStack );

            continue;
        }
    }

    return STL_SUCCESS;
}

/**
 * fd 재 전달 처리를 한다.
 * 1초 마다 호출됨.
 */
stlStatus ztlcRetryForwardFd( ztlcListener     * aListener,
                              stlErrorStack    * aErrorStack )
{
    stlInt32            sIdx;
    ztlcContext       * sContext;

    STL_TRY( ztlcGetFdReceiverAddr( aListener,
                                    aErrorStack )
             == STL_SUCCESS );

    /* 모든 연결된 session에 대해서 timeout check */
    for( sIdx = ZTLC_INDEX_SESSION; sIdx < aListener->mMaxContextCount; sIdx++ )
    {
        STL_INIT_ERROR_STACK( aErrorStack );

        sContext = &aListener->mContext[sIdx];

        if( sContext->mUsed == STL_FALSE )
        {
            continue;
        }

        /**
         * fd 전달 재시도
         */
        if( (sContext->mReceivedPreHandshake == STL_TRUE) &&
            (sContext->mIsFdSending == STL_FALSE) &&
            (sContext->mSessionType == CML_SESSION_SHARED) )
        {
            if( ztlcForwardFd( aListener,
                               sContext,
                               aErrorStack ) == STL_FAILURE )
            {
                (void)stlLogMsg( &gZtlLogger,
                                  aErrorStack,
                                  "[LISTENER] retry forward fd failure \n" );

                (void)ztlcSendPreHandshakeResult( aListener,
                                                  &sContext->mStlContext,
                                                  aErrorStack );

                (void)stlPopError( aErrorStack );

                (void)ztlcCloseContext( &aListener->mPollSet,
                                        sContext,
                                        aErrorStack );

                (void)ztlcFinalizeContext( sContext,
                                           aErrorStack );
            }
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * listen socket에서 event를 받았을때 처리하는 함수이다
 *
 */
stlStatus ztlcProcessListen( ztlcContext       * aListen,
                             ztlcListener      * aListener,
                             stlErrorStack     * aErrorStack )
{
    stlInt32            sState = 0;
    stlContext          sStlContext;
    ztlcContext       * sNewContext = NULL;
    stlBool             sAccess;

    /* sNewContext->mContext에 accept된 socket을 받아온다. */
    if( stlPollContext( &aListen->mStlContext,
                        &sStlContext,
                        0,
                        aErrorStack)
             == STL_FAILURE )
    {
        (void)stlLogMsg( &gZtlLogger,
                         aErrorStack,
                         "[LISTENER] accept failure : error code [%d] \n",
                         stlGetLastErrorCode(aErrorStack) );

        /* ulimit max에 걸려 accept가 실패하면 그냥 성공 처리하는 방법 뿐이다 */
        STL_TRY( stlGetLastErrorCode(aErrorStack) == STL_ERRCODE_ACCEPT );

        (void)stlPopError( aErrorStack );

        /*
         * ulimit max에 걸려 여기로 오는 경우 stlPoll에서는 계속 event가 발생되고
         * stlPollContext는 계속 error가 나서 무한 loop가 된다.
         * 그래서 sleep (100ms)을 한다.
         */
        stlSleep( STL_SET_MSEC_TIME(100) );

        STL_THROW( RAMP_SUCCESS );
    }
    sState = 1;

    /* Context List에서 비어 있는 Context를 하나 가져온다 */
    STL_TRY( ztlcGetContext( aListener,
                             STL_FALSE,         /* context 사용 아님 */
                             CML_SESSION_MAX,   /* sessionType filtering 안함 */
                             -1,                /* sequence filtering 안함 */
                             -1,                /* aReceivedPreHandshake filtering 안함 */
                             -1,                /* aIsFdSending  filtering 안함 */
                             &sNewContext,
                             aErrorStack )
             == STL_SUCCESS );

    if( sNewContext == NULL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTLE_ERRCODE_OVERFLOW_FD,
                      NULL,
                      aErrorStack );

        (void)ztlcSendPreHandshakeResult( aListener,
                                          &sStlContext,
                                          aErrorStack );

        (void)stlPopError( aErrorStack );

        STL_THROW( RAMP_SUCCESS );
    }

    /* new Context를 성공적으로 받은 경우임 */
    sNewContext->mStlContext = sStlContext;

    if( gZtlConfig.mTcpValidNodeChecking != ZTLM_TCP_FILTER_NONE )
    {
        /* client의 ip address를 허용할지 check */
        STL_TRY( ztlmCheckClientIpAddress( sNewContext,
                                           &sAccess,
                                           aErrorStack )
                 == STL_SUCCESS );

        if( sAccess == STL_FALSE )
        {
            /* access 못하는 ip 임. */
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          ZTLE_ERRCODE_ACCESS_DENIED,
                          NULL,
                          aErrorStack );

            (void)ztlcSendPreHandshakeResult( aListener,
                                              &sStlContext,
                                              aErrorStack );

            (void)stlPopError( aErrorStack );

            (void)ztlcCloseContext( &aListener->mPollSet,
                                    sNewContext,
                                    aErrorStack );

            STL_THROW( RAMP_SUCCESS );
        }
    }

    /* sNewContext를 초기화 한다 */
    STL_TRY( ztlcInitializeContext( aListener,
                                    sNewContext,
                                    aErrorStack )
             == STL_SUCCESS );

    sState = 2;

    /* non-block으로 만듬 */
    STL_TRY( stlSetSocketOption( STL_SOCKET_IN_CONTEXT(sNewContext->mStlContext),
                                 STL_SOPT_SO_NONBLOCK,
                                 STL_YES,
                                 aErrorStack )
             == STL_SUCCESS );

    /* stlPoll하기 위한 모든 작업이 성공한 경우임 */
    sNewContext->mPollFdEx.mPollFd = sNewContext->mStlContext.mPollFd;
    sNewContext->mPollFdEx.mUserContext = sNewContext;

    if( stlAddSockPollSet( &aListener->mPollSet,
                           &sNewContext->mPollFdEx,
                           aErrorStack ) == STL_FAILURE )
    {
        if( stlGetLastErrorCode(aErrorStack) == STL_ERRCODE_OUT_OF_MEMORY )
        {
            /* STL_ERRCODE_OUT_OF_MEMORY 를 ZTLE_ERRCODE_OVERFLOW_FD 로 변경 */
            (void)stlPopError( aErrorStack );

            stlPushError( STL_ERROR_LEVEL_ABORT,
                          ZTLE_ERRCODE_OVERFLOW_FD,
                          NULL,
                          aErrorStack );
        }

        /* error stack이 설정되었음으로 client로 error 전송됨 */
        (void)ztlcSendPreHandshakeResult( aListener,
                                          &sNewContext->mStlContext,
                                          aErrorStack );

        (void)stlPopError( aErrorStack );

        /* fd close 함 */
        (void)ztlcCloseContext( &aListener->mPollSet,
                                sNewContext,
                                aErrorStack );

        STL_THROW( RAMP_SUCCESS );
    }
    sState = 3;

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 3:
            (void)stlRemoveSockPollSet( &aListener->mPollSet,
                                        &sNewContext->mPollFdEx,
                                        aErrorStack );
        case 2:
            (void)ztlcFinalizeContext( sNewContext, aErrorStack );
        case 1:
            (void)stlFinalizeContext( &sStlContext, aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief  listener와 handshake를 끝난 context를 가져온다\n
 * sesstion type에 맞는 context만 가져온다.
 * @param[in] aListener             ztlcListener
 * @param[in] aUsed                 filtering할 Context 사용중 여부
 * @param[in] aSessionType          filtering할 session type ( CML_SESSION_MAX면 filtering안함)
 * @param[in] aSequence             filtering할 sequence (-1이면 filtering안함)
 * @param[in] ReceivedPreHandshake  filtering할 aReceivedPreHandshake (-1이면 filtering안함)
 * @param[in] aIsFdSending          filtering할 aIsFdSending (-1이면 filtering안함)
 * @param[out] aContext             찾은 context
 * @param[in] aErrorStack           error스텍
 *
 * @remark client에서 접속들어온 순서와 fd전달하는 순서가 바뀔수 있으나 문제 없을것 같음.\n
 * dedicated server에서 요청이 들어왔을때 호출된다.
 */
stlStatus ztlcGetContext( ztlcListener     * aListener,
                          stlBool            aUsed,
                          cmlSessionType     aSessionType,
                          stlInt64           aSequence,
                          stlInt32           aReceivedPreHandshake,
                          stlInt32           aIsFdSending,
                          ztlcContext     ** aContext,
                          stlErrorStack    * aErrorStack )
{
    stlInt32        sIdx;

    *aContext = NULL;

    for( sIdx = ZTLC_INDEX_SESSION; sIdx < aListener->mMaxContextCount; sIdx++ )
    {
        STL_DASSERT( aListener->mContext[sIdx].mIdx == sIdx );

        if( aListener->mContext[sIdx].mUsed != aUsed )
        {
            continue;
        }

        if( (aSequence != -1) &&
            (aListener->mContext[sIdx].mSequence != aSequence) )
        {
            continue;
        }

        /* mReceivedPreHandshake가 false이면 data를 받는 중이다 */
        if( (aReceivedPreHandshake != -1) &&
            (aListener->mContext[sIdx].mReceivedPreHandshake != aReceivedPreHandshake) )
        {
            continue;
        }

        /* fd를 이미 전송중인 상태다. ack를 못받은 상태임 */
        if( (aIsFdSending != -1) &&
            (aListener->mContext[sIdx].mIsFdSending != aIsFdSending) )
        {
            continue;
        }

        /* aSessionType에 포함되는것이 있으면 찾는다 */
        if( (aSessionType != CML_SESSION_MAX) &&
            (aListener->mContext[sIdx].mSessionType != aSessionType) )
        {
            continue;
        }

        *aContext = &aListener->mContext[sIdx];
        break;
    }

    return STL_SUCCESS;
}

/**
 * @brief command를 받아서 처리한다.
 * 다른 listener로 부터 command를 받거나 dedicated server로 부터 fd요청을 받는다.
 *
 */
stlStatus ztlcProcessUds( ztlcContext      * aUds,
                          ztlcListener     * aListener,
                          stlErrorStack    * aErrorStack )
{
    ztlcContext           * sContext = NULL;
    stlSockAddr             sPeerAddr;
    cmlUdsAck             * sAck;
    cmlUdsPacket            sPacket;

    if( cmlRecvFrom( &aUds->mStlContext,
                     &sPeerAddr,
                     &sPacket,
                     aErrorStack ) == STL_FAILURE )
    {
        (void)stlPopError( aErrorStack );

        STL_THROW( RAMP_SUCCESS );
    }

    /**
     * dispatcher에서 받을때는 command와 sequence가 오고
     * listener로부터 받을때는 command만 온다.
     * listener, dispatcher는 같은 서버임으로 byte order 문제 없음
     */

    switch( sPacket.mCmd.mCommon.mCmdType )
    {
        case CML_UDS_CMD_ACK:
            sAck = &sPacket.mCmd.mAck;

            if( sAck->mSenderStartTime != aListener->mStartTime )
            {
                /**
                 * 잘못된 ack임
                 * 그냥 성공 처리
                 */
                (void)stlLogMsg( &gZtlLogger,
                                 aErrorStack,
                                 "[LISTENER] wrong ack message ( current id %lu, receive id %lu )\n",
                                 aListener->mStartTime,
                                 sAck->mSenderStartTime );

                STL_THROW( RAMP_SUCCESS );
            }

            STL_DASSERT( sAck->mIdx < aListener->mMaxContextCount );
            STL_DASSERT( aListener->mContext[sAck->mIdx].mIdx == sAck->mIdx );

            /* context가 timeout으로 이미 cleanup되었을수 있음 */
            if( aListener->mContext[sAck->mIdx].mSequence == sAck->mSequence )
            {
                STL_DASSERT( aListener->mContext[sAck->mIdx].mUsed == STL_TRUE );
                STL_DASSERT( aListener->mContext[sAck->mIdx].mReceivedPreHandshake == STL_TRUE );
                STL_DASSERT( aListener->mContext[sAck->mIdx].mIsFdSending == STL_TRUE );

                sContext = &aListener->mContext[sAck->mIdx];

                if( stlGetErrorStackDepth(&sAck->mErrorStack) == 0 )
                {
                    /**
                     * dedicator or dispatcher에서 fd를 잘 받았음으로
                     * socket close하고 Context List에서 data를 삭제한다
                     */
                    (void)ztlcSendPreHandshakeResult( aListener,
                                                      &sContext->mStlContext,
                                                      aErrorStack );
                    (void)ztlcCloseContext( &aListener->mPollSet,
                                            sContext,
                                            aErrorStack );
                    (void)ztlcFinalizeContext( sContext, aErrorStack );
                }
                else
                {
                    /**
                     * error가 발생한 경우로
                     * mIsFdSending 만 false로 설정해놓는다
                     */

                    sContext->mIsFdSending = STL_FALSE;
                }
            }
            else
            {
                /* do nothing */
            }

            break;
        case CML_UDS_CMD_REQUEST_FD:
            /* dedicated server에서 fd요청 */

            /* handshake가 끝난 dedicated session을 가져온다 */
            STL_TRY( ztlcGetContext( aListener,
                                     STL_TRUE,              /* context 사용중 */
                                     CML_SESSION_DEDICATE,
                                     -1,                    /* sequence filtering 안함 */
                                     STL_TRUE,              /* aReceivedPreHandshake */
                                     STL_FALSE,             /* aIsFdSending  */
                                     &sContext,
                                     aErrorStack )
                     == STL_SUCCESS );

            if( sContext != NULL )
            {
                if( cmlSendFdTo( &aUds->mStlContext,
                                 &sPeerAddr,
                                 &sContext->mStlContext,
                                 gZtlConfig.mTcpPort,
                                 aListener->mStartTime,
                                 sContext->mSequence,
                                 sContext->mIdx,
                                 sContext->mMajorVersion,
                                 sContext->mMinorVersion,
                                 sContext->mPatchVersion,
                                 aErrorStack ) == STL_FAILURE )
                {
                    /**
                     * send시 STL_ERRCODE_AGAIN, STL_ERRCODE_FILE_BAD 나오면 정상 처리
                     * 추후 fd재전송 처리한다.
                     */
                    STL_TRY( (stlGetLastErrorCode(aErrorStack) == STL_ERRCODE_AGAIN) ||
                             (stlGetLastErrorCode(aErrorStack) == STL_ERRCODE_FILE_BAD) ||
                             (stlGetLastErrorCode(aErrorStack) == STL_ERRCODE_SENDMSG) );

                    (void)stlPopError( aErrorStack );
                }
                else
                {
                    (void)ztlcSendPreHandshakeResult( aListener,
                                                      &sContext->mStlContext,
                                                      aErrorStack );
                    (void)ztlcCloseContext( &aListener->mPollSet,
                                            sContext,
                                            aErrorStack );
                    (void)ztlcFinalizeContext( sContext, aErrorStack );
                }
            }
            else
            {
                /**
                 * do nothing
                 *
                 * 전송할 fd가 없는데 fd요청이 온 경우임
                 * listener에서는 아무것도 안보내고 dedicated server가 timeout으로 종료처리된다.
                 */
            }

            break;
        case CML_UDS_CMD_REQUEST_QUIT:
            gZtlQuitListen = STL_TRUE;
            break;
        case CML_UDS_CMD_REQUEST_STATUS:
            (void)ztlcSendStatusInfo( aUds, &sPeerAddr, aErrorStack );
            break;
        default:
            break;
    }

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief listener가 client와 handshake를 하기 위해서
 * client와 연결된 socket으로 data를 받아서 처리한다.
 */
stlStatus ztlcProcessRequest( ztlcListener      * aListener,
                              ztlcContext       * aContext,
                              stlInt64            aIdx,
                              stlTime             aNow,
                              stlErrorStack     * aErrorStack )
{
    stlInt32            sState = 0;
    stlSize             sSize;
    cmlSessionType      sCsMode;
    stlBool             sIsSend = STL_FALSE;        /* client로 data를 보낼것인지 여부 */
    stlBool             sIsClose = STL_FALSE;       /* socket close할지 여부 */
    stlBool             sIsUnsetFd = STL_FALSE;     /* stlPoll에서 fd를 제거할지 여부 */
    cmlHandle           sCmlHandle;
    cmlProtocolSentence sProtocolSentence;
    cmlMemoryManager    sMemoryManager;

    sSize = CML_VERSION_SIZE + CML_MTU_SIZE - aContext->mRecvedSize;

    aContext->mLastTime = aNow;

    /* client 로부터 data를 받는다 */
    if( stlRecv( STL_SOCKET_IN_CONTEXT(aContext->mStlContext),
                 (stlChar *)aContext->mRecvBuf + aContext->mRecvedSize,
                 &sSize,
                 aErrorStack ) == STL_FAILURE )
    {
        if( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_AGAIN )
        {
            sIsSend = STL_FALSE;
            sIsClose = STL_FALSE;
            sIsUnsetFd = STL_FALSE;
        }
        else
        {
            /* STL_ERRCODE_AGAIN 이외 error시 socket close */
            sIsSend = STL_FALSE;
            sIsClose = STL_TRUE;
            sIsUnsetFd = STL_TRUE;
        }

        /* socket error는 성공 처리 */
        (void)stlPopError( aErrorStack );
        STL_THROW( RAMP_SUCCESS );
    }

    /* client로 부터 data를 받은 경우 */
    aContext->mRecvedSize += sSize;

    /* 현재 prehandshake length가 old version의 data length보다 커야 함 */
    STL_DASSERT( CML_VERSION_SIZE + CML_MTU_SIZE >= gZtlcVersion210RequestSize );

    /**
     * 이전 version protocol인 경우 처리한다.
     * goldilocks 2.1.0 gsqlnet인 경우 01 00 00 00 09 00 00 00 01 03 01 00 01 00 01 00 + endian이 온다.
     * sVersion1Request와 16byte를 비교한다.
     */
    if( (aContext->mRecvedSize == gZtlcVersion210RequestSize) &&
        (stlMemcmp( aContext->mRecvBuf, gZtlcVersion210Request, STL_SIZEOF(gZtlcVersion210Request) ) == 0) )
    {
        /* version 1.0 protocol이 오면 : Invalid communication protocol을 보낸다. */

        sSize = STL_SIZEOF(gZtlcVersion210Response);

        if( stlSend( STL_SOCKET_IN_CONTEXT(aContext->mStlContext),
                     gZtlcVersion210Response,
                     &sSize,
                     aErrorStack )
                 == STL_FAILURE )
        {
            (void)stlPopError( aErrorStack );
        }

        sIsSend = STL_FALSE;
        sIsClose = STL_TRUE;
        sIsUnsetFd = STL_TRUE;

        STL_THROW( RAMP_SUCCESS );
    }

    /* packet을 다 못받았으면 성공 처리 */
    if( aContext->mRecvedSize < CML_MTU_SIZE + CML_VERSION_SIZE )
    {
        sIsSend = STL_FALSE;
        sIsClose = STL_FALSE;
        sIsUnsetFd = STL_FALSE;

        STL_THROW( RAMP_SUCCESS );
    }

    /* memory manager */
    sMemoryManager.mPacketBufferSize = aContext->mRecvedSize;
    sMemoryManager.mReadBuffer = NULL;
    sMemoryManager.mWriteBuffer = NULL;

    STL_TRY( cmlInitializeHandle( &sCmlHandle,
                                  &aContext->mStlContext,
                                  STL_PLATFORM_ENDIAN,
                                  aErrorStack )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( cmlInitializeProtocolSentence( &sCmlHandle,
                                            &sProtocolSentence,
                                            &sMemoryManager,
                                            aErrorStack )
             == STL_SUCCESS );
    sState = 2;

    sProtocolSentence.mReadProtocolPos = aContext->mRecvBuf;
    sProtocolSentence.mReadEndPos = sProtocolSentence.mReadProtocolPos + CML_VERSION_SIZE;

    STL_TRY( cmlReadVersion( &sCmlHandle,
                             &aContext->mMajorVersion,
                             &aContext->mMinorVersion,
                             &aContext->mPatchVersion,
                             aErrorStack )
             == STL_SUCCESS );

    /* client의 버전이 높으면 error 처리 */
    if( (aContext->mMajorVersion != CML_PROTOCOL_MAJOR_VERSION) ||
        (aContext->mMinorVersion > CML_PROTOCOL_MINOR_VERSION) ||
        (aContext->mPatchVersion > CML_PROTOCOL_PATCH_VERSION) )
    {
        (void)stlLogMsg( &gZtlLogger,
                         aErrorStack,
                         "[LISTENER] not support client (ver %d.%d.%d) \n",
                         aContext->mMajorVersion,
                         aContext->mMinorVersion,
                         aContext->mPatchVersion );

        (void)stlPushError( STL_ERROR_LEVEL_ABORT,
                            ZTLE_ERRCODE_INVALID_COMMUNICATION_PROTOCOL,
                            NULL,
                            aErrorStack );

        sIsSend = STL_TRUE;
        sIsClose = STL_TRUE;
        sIsUnsetFd = STL_TRUE;

        STL_THROW( RAMP_SUCCESS );
    }


    sProtocolSentence.mReadProtocolPos = aContext->mRecvBuf + CML_VERSION_SIZE ;
    sProtocolSentence.mReadEndPos = aContext->mRecvBuf + aContext->mRecvedSize;

    /**
     * data를 받은 경우 client에서 보낸 data를 parsing한다.
     */
    if( cmlReadPreHandshake( &sCmlHandle,
                             &sCsMode,
                             aErrorStack )
            == STL_FAILURE )
    {
        /* CML_ERRCODE_INVALID_COMMUNICATION_PROTOCOL error면 그대로 처리 */
        sIsSend = STL_TRUE;
        sIsClose = STL_TRUE;
        sIsUnsetFd = STL_TRUE;

        /* PreHandshake가 실패해도 성공 처리 */
        STL_THROW( RAMP_SUCCESS );
    }

    aContext->mReceivedPreHandshake = STL_TRUE;

    /**
     * preHandshake packet을 읽기 완료 했음으로 dedicated, shared 구분
     *
     * sCsMode는 client에서 설정한 모드이고
     * gZtlConfig.mDefaultCsMode는 listener의 config의 모드임.
     *
     * client에서 server모드를 CML_SESSION_DEFAULT로 한 경우만 listener의
     * mDefaultCsMode가 사용됨.
     */
    if( (sCsMode == CML_SESSION_DEDICATE) ||
        ( (sCsMode == CML_SESSION_DEFAULT) && (gZtlConfig.mDefaultCsMode == CML_SESSION_DEDICATE) ) )
    {
        /* dedicated로 처리 */
        sIsSend = STL_FALSE;    /* dedicated로 보낸후 ack받고 handshake result */
        sIsClose = STL_FALSE;   /* dedicated로 보낸후 ack받고 close함 */
        sIsUnsetFd = STL_TRUE;

        aContext->mSessionType = CML_SESSION_DEDICATE;

        if( ztlcExecDedicator( aContext, aErrorStack ) != STL_SUCCESS )
        {
            (void)stlLogMsg( &gZtlLogger,
                             aErrorStack,
                             "[LISTENER] execute gserver(dedicate server) error : %s \n",
                             stlGetLastErrorMessage( aErrorStack ) );

            sIsSend = STL_TRUE;
            sIsClose = STL_TRUE;
        }

        STL_THROW( RAMP_SUCCESS );
    }
    else if( (sCsMode == CML_SESSION_SHARED) ||
             ( (sCsMode == CML_SESSION_DEFAULT) && (gZtlConfig.mDefaultCsMode == CML_SESSION_SHARED) ) )
    {
        /* shared server로 접속 */

        aContext->mSessionType = CML_SESSION_SHARED;

        if( ztlcForwardFd( aListener,
                           aContext,
                           aErrorStack ) == STL_FAILURE )
        {
            /**
             * ztlcForwardFd가 실패하면 ztlcGetFdReceiverAddr()를 수행하고 한번 더 시도해본다.
             * gmaster가 key를 변경하여 재시작 된 경우 재시도 하면
             * aListener->mFdReceiverAddr가 변경되어 fd가 전달된다.
             */
            (void)stlPopError( aErrorStack );

            STL_TRY( ztlcGetFdReceiverAddr( aListener,
                                            aErrorStack )
                     == STL_SUCCESS );

            if( ztlcForwardFd( aListener,
                               aContext,
                               aErrorStack ) == STL_FAILURE )
            {
                /**
                 * ztlcForwardFd 첫번째, 두번째 모두 실패한 경우
                 */
                sIsSend = STL_TRUE;
                sIsClose = STL_TRUE;
                sIsUnsetFd = STL_TRUE;

                STL_THROW( RAMP_SUCCESS );
            }

            /**
             * ztlcForwardFd 첫번째 실패후 두번째 성공한 경우
             */

            sIsSend = STL_FALSE;    /* ack받고 preHandshake result 보냄 */
            sIsClose = STL_FALSE;   /* ack받고 close함 */
            sIsUnsetFd = STL_TRUE;

            STL_THROW( RAMP_SUCCESS );
        }
        /**
         * ztlcForwardFd 첫번째 성공한 경우
         */

        sIsSend = STL_FALSE;    /* ack받고 preHandshake result 보냄 */
        sIsClose = STL_FALSE;   /* ack받고 close함 */
        sIsUnsetFd = STL_TRUE;

        STL_THROW( RAMP_SUCCESS );
    }
    else
    {
        /* 없는 mode */
        (void)stlPushError( STL_ERROR_LEVEL_ABORT,
                            ZTLE_ERRCODE_INVALID_COMMUNICATION_PROTOCOL,
                            NULL,
                            aErrorStack );

        sIsSend = STL_TRUE;
        sIsClose = STL_TRUE;
        sIsUnsetFd = STL_TRUE;
    }

    STL_RAMP( RAMP_SUCCESS );

    if( sIsSend == STL_TRUE )
    {
        if( ztlcSendPreHandshakeResult( aListener,
                                        &aContext->mStlContext,
                                        aErrorStack )
                 == STL_FAILURE )
        {
            (void)stlPopError( aErrorStack );
            sIsClose = STL_TRUE;
            sIsUnsetFd = STL_TRUE;
        }

        /* 이 함수의 stlPushError()에 대한 popError임. */
        (void)stlPopError( aErrorStack );
    }

    if( sIsUnsetFd == STL_TRUE )
    {
        STL_TRY( stlRemoveSockPollSet( &aListener->mPollSet,
                                       &aContext->mPollFdEx,
                                       aErrorStack )
                 == STL_SUCCESS );
        aContext->mPollFdEx.mPollFd.mSocketHandle = -1;
    }

    if( sIsClose == STL_TRUE )
    {
        /* close하면서 UnsetFd를 안하면 안됨. */
        STL_DASSERT( sIsUnsetFd == STL_TRUE );

        (void)stlFinalizeContext( &aContext->mStlContext, aErrorStack );
        (void)ztlcFinalizeContext( aContext, aErrorStack );
    }

    sState = 1;
    (void)cmlFinalizeProtocolSentence( &sProtocolSentence,
                                       aErrorStack );

    sState = 0;
    (void)cmlFinalizeHandle( &sCmlHandle,
                             aErrorStack );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)cmlFinalizeProtocolSentence( &sProtocolSentence,
                                               aErrorStack );
        case 1:
            (void)cmlFinalizeHandle( &sCmlHandle,
                                     aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}



/**
 * @brief Gliese Listener routines
 */
stlStatus ztlcRun( stlErrorStack         * aErrorStack )
{
    ztlcContext           * sUdsContext = NULL;
    ztlcContext           * sListenContext = NULL;
    stlInt32                sSigFdNum;
    stlInt32                sState = 0;
    stlInt32                sIdx;
    ztlcListener            sListener;
    stlTime                 sNowTime;
    stlTime                 sLastTime;
    stlPollFdEx           * sPollFdEx = NULL;
    ztlcContext           * sContext;
    stlChar                 sVersionStr[256];

    if( gZtlConfig.mTcpValidNodeChecking != ZTLM_TCP_FILTER_NONE )
    {
        /* tcp access control list관련 파일을 읽는다 */
        STL_TRY( ztlmSetNetworkFilter( aErrorStack )
                 == STL_SUCCESS );
    }

    stlMemset( &sListener, 0x00, STL_SIZEOF(ztlcListener) );

    sListener.mStartTime = stlNow();

    /* sListener 초기화한다. */
    STL_TRY( ztlcInitialize( &sListener,
                             aErrorStack )
             == STL_SUCCESS );
    sState = 1;

    /* ListenContext 생성 */
    sListenContext = &sListener.mContext[ZTLC_INDEX_LISTEN_SOCKET];
    STL_TRY( stlInitializeContext( &sListenContext->mStlContext,
                                   STL_AF_INET,
                                   STL_SOCK_STREAM,
                                   0,
                                   STL_TRUE,  /* Listen */
                                   gZtlConfig.mTcpHost,
                                   gZtlConfig.mTcpPort,
                                   aErrorStack )
             == STL_SUCCESS );
    sState = 2;

    /* listen Context 초기화 */
    STL_TRY( ztlcInitializeContext( &sListener,
                                    sListenContext,
                                    aErrorStack )
             == STL_SUCCESS );

    sListenContext->mPollFdEx.mPollFd = sListenContext->mStlContext.mPollFd;
    sListenContext->mPollFdEx.mUserContext = sListenContext;

    STL_TRY( stlAddSockPollSet( &sListener.mPollSet,
                                &sListenContext->mPollFdEx,
                                aErrorStack )
             == STL_SUCCESS );

    /* command uds 생성 */
    sUdsContext = &sListener.mContext[ZTLC_INDEX_UDS];

    STL_TRY( ztlcInitializeUds( sUdsContext,
                                gZtlUdsPath,
                                aErrorStack ) == STL_SUCCESS );
    sState = 3;

    /* command uds Context 초기화 */
    STL_TRY( ztlcInitializeContext( &sListener,
                                    sUdsContext,
                                    aErrorStack )
             == STL_SUCCESS );

    sUdsContext->mPollFdEx.mPollFd = sUdsContext->mStlContext.mPollFd;
    sUdsContext->mPollFdEx.mUserContext = sUdsContext;

    STL_TRY( stlAddSockPollSet( &sListener.mPollSet,
                                &sUdsContext->mPollFdEx,
                                aErrorStack )
             == STL_SUCCESS );

    STL_TRY( cmlInitializeMemoryManager( &sListener.mMemoryMgr,
                                         CML_HANDSHAKE_BUFFER_SIZE,     /* PacketBufferSize */
                                         0,                             /* OperationBufferSize */
                                         aErrorStack ) == STL_SUCCESS );
    sState = 4;

    stlGetVersionString( sVersionStr, STL_SIZEOF(sVersionStr) );

    (void)stlLogMsg( &gZtlLogger,
                     aErrorStack,
                     "[LISTENER] started. VERSION : %s\n",
                     sVersionStr );
               
    ztlmPrintf( "Listener is started successfully.\n" );

    sLastTime = stlNow();

    while( gZtlQuitListen != STL_TRUE )
    {
        STL_INIT_ERROR_STACK( aErrorStack );

        sNowTime = stlNow();

        if( sLastTime + STL_SET_SEC_TIME(1) < sNowTime )
        {
            /*  cleanup */
            STL_TRY( ztlcProcessCleanup( &sListener,
                                         sNowTime,
                                         aErrorStack )
                     == STL_SUCCESS );

            /*  fd전달 재시도 */
            STL_TRY( ztlcRetryForwardFd( &sListener,
                                         aErrorStack )
                     == STL_SUCCESS );

            sLastTime = sNowTime;
        }

        STL_DASSERT( sListener.mPollSet.mMaxPollFdNum >= sListener.mPollSet.mCurPollFdNum );

        if( stlPollPollSet( &sListener.mPollSet,
                            STL_SET_SEC_TIME(1),
                            &sSigFdNum,
                            &sPollFdEx,
                            aErrorStack ) != STL_SUCCESS )
        {
            STL_TRY( stlGetLastErrorCode(aErrorStack) == STL_ERRCODE_TIMEDOUT );
            (void)stlPopError( aErrorStack );

            continue;
        }
        
        sNowTime = stlNow();

        for( sIdx = 0; sIdx < sSigFdNum; sIdx++ )
        {
            if( sPollFdEx[sIdx].mUserContext == sListenContext )
            {
                /**
                 * listen socket event 처리
                 */
                STL_TRY( ztlcProcessListen( &sListener.mContext[ZTLC_INDEX_LISTEN_SOCKET],
                                            &sListener,
                                            aErrorStack )
                         == STL_SUCCESS );
                /* continue 해서 ztlcSetPollFd를 처리하고 와야 아래 부분 수행 가능함 */
            }
            else if( sPollFdEx[sIdx].mUserContext == sUdsContext )
            {
                /**
                 * uds event ( command 및 fd전송에 대한 ack) 처리
                 */
                STL_TRY( ztlcProcessUds( &sListener.mContext[ZTLC_INDEX_UDS],
                                         &sListener,
                                         aErrorStack )
                         == STL_SUCCESS );

            }
            else
            {
                /**
                 * client와 연결된 socket 처리
                 */
                sContext = sPollFdEx[sIdx].mUserContext;

                STL_DASSERT( (sContext->mUsed == STL_TRUE) &&
                             (sContext->mReceivedPreHandshake == STL_FALSE) );

                if( (sContext->mUsed == STL_FALSE) ||
                    (sContext->mReceivedPreHandshake == STL_TRUE) )
                {
                    continue;
                }

                STL_TRY( ztlcProcessRequest( &sListener,
                                             sContext,
                                             sIdx,
                                             sNowTime,
                                             aErrorStack )
                         == STL_SUCCESS );
            }

            /* ZTL_COMMAND_LISTENER_QUIT 가 오면 break */
            if( gZtlQuitListen == STL_TRUE )
            {
                break;
            }
        }
    }

    (void)stlLogMsg( &gZtlLogger,
                     aErrorStack,
                     "[LISTENER] terminated\n" );
               
    for( sIdx = ZTLC_INDEX_SESSION; sIdx < sListener.mMaxContextCount; sIdx++ )
    {
        if( sListener.mContext[sIdx].mUsed == STL_TRUE )
        {
            (void)ztlcCloseContext( &sListener.mPollSet,
                                    &sListener.mContext[sIdx],
                                    aErrorStack );
            (void)ztlcFinalizeContext( &sListener.mContext[sIdx], aErrorStack );
        }
    }

    sState = 3;
    (void)cmlFinalizeMemoryManager( &sListener.mMemoryMgr, aErrorStack );

    sState = 2;
    (void)ztlcFinalizeUds( sUdsContext, aErrorStack );

    sState = 1;
    (void)stlFinalizeContext( &sListenContext->mStlContext, aErrorStack );

    sState = 0;
    (void)ztlcFinalize( &sListener, aErrorStack );


    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 4:
            for( sIdx = ZTLC_INDEX_SESSION; sIdx < sListener.mMaxContextCount; sIdx++ )
            {
                if( sListener.mContext[sIdx].mUsed == STL_TRUE )
                {
                    (void)ztlcCloseContext( &sListener.mPollSet,
                                            &sListener.mContext[sIdx],
                                            aErrorStack );
                    (void)ztlcFinalizeContext( &sListener.mContext[sIdx], aErrorStack );
                }
            }

            (void)cmlFinalizeMemoryManager( &sListener.mMemoryMgr, aErrorStack );
        case 3:
            (void)ztlcFinalizeUds( sUdsContext, aErrorStack );
        case 2:
            (void)stlFinalizeContext( &sListenContext->mStlContext, aErrorStack );
        case 1:
            (void)ztlcFinalize( &sListener, aErrorStack );
        default:

            break;
    }

    (void)stlLogMsg( &gZtlLogger,
                     aErrorStack,
                     "[LISTENER] abnormally terminated\n" );
               
    return STL_FAILURE;
}


/**
 * @brief Gliese Dedicate routines
 */
stlStatus ztlcExecDedicator( ztlcContext     * aContext,
                             stlErrorStack   * aErrorStack )
{
    stlInt32      sState = 0;
    stlProc       sProc;
    stlBool       sIsChild = STL_FALSE;
    stlChar     * sArgv[6];

    sArgv[0] = "gserver";
    sArgv[1] = "--dedicated";
    sArgv[2] = gZtlUdsPath;

    if( gZtlDaemonize == STL_FALSE )
    {
        sArgv[3] = "-n";
        sArgv[4] = NULL;
    }
    else
    {
        sArgv[3] = NULL;
    }

    STL_TRY( stlForkProc( &sProc,
                          &sIsChild,
                          aErrorStack ) == STL_SUCCESS );
    sState = 1;

    if( sIsChild == STL_FALSE )
    {
        /* Parent Process : do nothing */

    }
    else
    {
        /* Child Process */
        sState = 0;
        (void)stlFinalizeContext( &aContext->mStlContext, aErrorStack );

        STL_TRY( stlExecuteProc( sArgv[0],
                                 sArgv,
                                 aErrorStack )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            /* fork 이후 child process는 죽어야 함 */
            if( sIsChild == STL_TRUE )
            {
                stlExit( 0 );
            }
        default:
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief fd전달
 * @remark fd를 gmaster로 전달하면 성공을 리턴하고 gmaster로 전달하지 못하면 실패를 리턴한다.
 */
stlStatus ztlcForwardFd( ztlcListener      * aListener,
                         ztlcContext       * aContext,
                         stlErrorStack     * aErrorStack )
{
    ztlcContext     * sUds = NULL;

    sUds = &aListener->mContext[ZTLC_INDEX_UDS];

    STL_TRY_THROW( aListener->mValidFdReceiverAddr == STL_TRUE,
                   RAMP_ERROR_RECEIVER_ADDR_INVALID );

    if( cmlSendFdTo( &sUds->mStlContext,
                     &aListener->mFdReceiverAddr,
                     &aContext->mStlContext,
                     gZtlConfig.mTcpPort,
                     aListener->mStartTime,
                     aContext->mSequence,
                     aContext->mIdx,
                     aContext->mMajorVersion,
                     aContext->mMinorVersion,
                     aContext->mPatchVersion,
                     aErrorStack ) == STL_FAILURE )
    {
        STL_TRY( stlGetLastErrorCode(aErrorStack) == STL_ERRCODE_SENDMSG );

        /* 더이상 mFdReceiverAddr 가 유효하지 않음 */
        aListener->mValidFdReceiverAddr = STL_FALSE;

        (void)stlPopError( aErrorStack );

        STL_THROW( RAMP_ERROR_RECEIVER_ADDR_INVALID );
    }
    else
    {
        /* fd전달에 성공한 경우 */
        aContext->mIsFdSending = STL_TRUE;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERROR_RECEIVER_ADDR_INVALID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTLE_ERRCODE_SHARED_MODE_INACTIVE,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztlcInitializeUds( ztlcContext     * aUds,
                             stlChar         * aPath,
                             stlErrorStack   * aErrorStack )
{
    /* UDS context를 생성 for Command  */
    if( stlInitializeContext( &aUds->mStlContext,
                              STL_AF_UNIX,
                              STL_SOCK_DGRAM,
                              0,
                              STL_FALSE,
                              aPath,
                              0,
                              aErrorStack ) == STL_SUCCESS )
    {
        (void)stlLogMsg( &gZtlLogger,
                         aErrorStack,
                         "[LISTENER] Unix Domain Socket [%s] Create success \n",
                         aPath );
    }
    else
    {
        (void)stlLogMsg( &gZtlLogger,
                         aErrorStack,
                         "[LISTENER] Unix Domain Socket [%s] Create failure(%d) \n",
                         aPath,
                         stlGetLastErrorCode( aErrorStack ) );

        STL_TRY( STL_FALSE );
    }

    STL_TRY( stlSetSocketOption( STL_SOCKET_IN_CONTEXT(aUds->mStlContext),
                                 STL_SOPT_SO_SNDBUF,
                                 CML_BUFFER_SIZE,
                                 aErrorStack )
             == STL_SUCCESS );

    STL_TRY( stlSetSocketOption( STL_SOCKET_IN_CONTEXT(aUds->mStlContext),
                                 STL_SOPT_SO_RCVBUF,
                                 CML_BUFFER_SIZE,
                                 aErrorStack )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztlcFinalizeUds( ztlcContext     * aUds,
                           stlErrorStack   * aErrorStack )
{
    (void)stlFinalizeContext( &aUds->mStlContext, aErrorStack );

    return STL_SUCCESS;
}


stlStatus ztlcGetMaxContextCount( stlInt64        * aMaxContextCount,
                                  stlErrorStack   * aErrorStack )
{
    stlResourceLimitType    sLimitType;
    stlResourceLimit        sLimit;

    sLimitType = STL_RESOURCE_LIMIT_NOFILE;

    STL_TRY( stlGetResourceLimit( sLimitType,
                                  &sLimit,
                                  aErrorStack )
             == STL_SUCCESS );

    if( (sLimit.mCurrent - 10) < gZtlConfig.mBacklog )
    {
        *aMaxContextCount = (sLimit.mCurrent - 10);

        (void)stlLogMsg( &gZtlLogger,
                         aErrorStack,
                         "[LISTENER] Maximum number of open files is too small (%ld) \n",
                         sLimit.mCurrent );
    }
    else
    {
        *aMaxContextCount = gZtlConfig.mBacklog;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
