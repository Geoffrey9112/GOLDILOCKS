/*******************************************************************************
 * ztis.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztis.c 12157 2014-04-23 03:50:31Z lym999 $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file ztis.c
 * @brief Gliese gserver Main Routines
 */

#include <stl.h>
#include <cml.h>
#include <scl.h>
#include <sll.h>
#include <ztiDef.h>
#include <ztim.h>
#include <ztis.h>

/**
 * @brief send thread
 */
void * STL_THREAD_FUNC ztisSendResponse( stlThread *aThread, void *aArg )
{
    stlErrorStack       sErrorStack;
    ztiDispatcher     * sDispatcher = NULL;
    sllHandle         * sHandle;
    sclQueueCommand     sCommand;
    stlInt32            sState = 0;
    sllEnv            * sEnv = NULL;
    sllSessionEnv     * sSessionEnv = NULL;
    stlBool             sEnableAging = STL_FALSE;
    stlThreadHandle     sThread;
    stlTime             sNowTime;
    stlTime             sLastTime;
    stlTime             sIdleStart;
    stlTime             sIdleEnd;
    stlInt64            sCmUnitCount;
    stlInterval         aTimeout;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sDispatcher = aArg;

    STL_TRY( sslAllocEnv( (sslEnv**)&sEnv,
                          &sErrorStack ) == STL_SUCCESS );
    sState = 1;
    sDispatcher->mZlhlDispatcher->mSendThreadEnv = sEnv;

    STL_TRY( sllInitializeEnv( sEnv, KNL_ENV_SHARED ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( sslAllocSession( "",
                              (sslSessionEnv**)&sSessionEnv,
                              SSL_ENV(sEnv) )
             == STL_SUCCESS );
    sState = 3;

    STL_TRY( sllInitializeSessionEnv( sSessionEnv,
                                      KNL_CONNECTION_TYPE_DA,
                                      KNL_SESSION_TYPE_DEDICATE,
                                      KNL_SESS_ENV_SHARED,
                                      sEnv )
             == STL_SUCCESS );
    sState = 4;

    KNL_LINK_SESS_ENV( sEnv, sSessionEnv );
    KNL_ENABLE_SESSION_AGING( sSessionEnv );
    sEnableAging = STL_TRUE;

    STL_TRY( stlThreadSelf( &sThread,
                            &sErrorStack )
             == STL_SUCCESS );

    (void) sslSetSessionProcessInfo( &sThread.mProc,
                                     "dispatcher(send thread)",
                                     (sslSessionEnv*)sSessionEnv );

    sLastTime = knlGetSystemTime();
    sIdleEnd = knlGetSystemTime();

    while( (gZtiSharedMemory->mExitFlag == STL_FALSE) &&
           (sDispatcher->mExitProcess == STL_FALSE) )
    {
        STL_INIT_ERROR_STACK( KNL_ERROR_STACK(sEnv) );

        KNL_CHECK_THREAD_CANCELLATION( KNL_ENV(sEnv) );

        sDispatcher->mZlhlDispatcher->mSendStatus = SLL_STATUS_WAIT;

        sNowTime = knlGetSystemTime();

        /**
         * retry중인 data가 없으면 Dequeue timeout : 1초
         * retry중인 data가 있으면 Dequeue timeout : 0.01초
         */

        if( sDispatcher->mRetry == STL_FALSE )
        {
            aTimeout = STL_SET_SEC_TIME(1);
        }
        else
        {
            aTimeout = STL_SET_MSEC_TIME(10);
        }

        if( (sDispatcher->mRetry == STL_TRUE) &&
            (sLastTime + aTimeout < sNowTime) )
        {
            sDispatcher->mZlhlDispatcher->mSendStatus = SLL_STATUS_SEND;

            /* data send 재시도 */
            STL_TRY( ztisRetryResponse( sDispatcher,
                                        sEnv )
                     == STL_SUCCESS );

            sDispatcher->mZlhlDispatcher->mSendStatus = SLL_STATUS_WAIT;

            sLastTime = sNowTime;
        }


        /**
         * sIdleEnd 부터 sIdleStart까지는 BUSY time추가
         */
        sIdleStart = knlGetSystemTime();
        sDispatcher->mZlhlDispatcher->mSendBusy += sIdleStart - sIdleEnd;

        if( sclDequeueResponse( sDispatcher->mZlhlDispatcher->mResponseEvent,
                                (sclHandle **)&sHandle,
                                &sCommand,
                                &sCmUnitCount,
                                aTimeout,
                                SCL_ENV(sEnv) ) == STL_FAILURE )
        {
            STL_TRY( stlGetLastErrorCode( KNL_ERROR_STACK(sEnv) ) == STL_ERRCODE_AGAIN );
            (void)stlPopError( KNL_ERROR_STACK(sEnv) );


            /**
             * sIdleStart 부터 sIdleEnd까지는 IDLE time추가
             */
            sIdleEnd = knlGetSystemTime();
            sDispatcher->mZlhlDispatcher->mSendIdle += sIdleEnd - sIdleStart;

            continue;
        }

        /**
         * sIdleStart 부터 sIdleEnd까지는 IDLE time추가
         */
        sIdleEnd = knlGetSystemTime();
        sDispatcher->mZlhlDispatcher->mSendIdle += sIdleEnd - sIdleStart;


        sDispatcher->mZlhlDispatcher->mSendStatus = SLL_STATUS_SEND;

        STL_TRY( ztisProcessResponse( sHandle,
                                      sCommand,
                                      sDispatcher,
                                      sCmUnitCount,
                                      sEnv )
                 == STL_SUCCESS );
    }

    sState = 2;
    STL_TRY( sllFinalizeSessionEnv( sSessionEnv,
                                    sEnv )
             == STL_SUCCESS );

    sState = 0;
    STL_TRY( sllFinalizeEnv( sEnv ) == STL_SUCCESS );

    knlDisconnectSession( sSessionEnv );

    sDispatcher->mExitProcess = STL_TRUE;

    stlExitThread( aThread, STL_SUCCESS, &sErrorStack );

    return NULL;

    STL_FINISH;

    switch( sState )
    {
        case 4:
            (void)sllFinalizeSessionEnv( sSessionEnv, sEnv );
        case 3:
            knlLogErrorCallStack( NULL, KNL_ENV(sEnv) );
            knlLogMsg( NULL,
                       KNL_ENV(sEnv),
                       KNL_LOG_LEVEL_FATAL,
                       "[DISPATCHER-%d] send-thread abnormally terminated\n",
                       sDispatcher->mId );

            if( sEnableAging == STL_FALSE )
            {
                (void)sslFreeSession( (void *)sSessionEnv, SSL_ENV(sEnv) );
            }
        case 2:
            stlAppendErrors( &sErrorStack, KNL_ERROR_STACK(sEnv) );
            (void)sllFinalizeEnv( sEnv );
            if( sSessionEnv != NULL )
            {
                knlDisconnectSession( sSessionEnv );
            }
        case 1:
            if( sEnableAging == STL_FALSE )
            {
                (void)sslFreeEnv( SSL_ENV(sEnv), &sErrorStack );
            }
        default:
            break;
    }

    sDispatcher->mExitProcess = STL_TRUE;

    stlExitThread( aThread, STL_FAILURE, &sErrorStack );

    return NULL;
}

/* shared-server로 부터 받은 data가 유효한지 검증한다 */
stlStatus ztisValidAddress( sllHandle         * aHandle,
                            ztiDispatcher     * aDispatcher,
                            stlBool           * aValid,
                            sllEnv            * aEnv )
{

    ztiContext      * sContext;
    stlUInt64         sTemp;

    sContext = (ztiContext *)aHandle->mUserContext;

    /**
     * mUserContext 검증
     * 내가 보낸 request가 아닌 response일수도 있음.
     *
     * aHandle->mUserContext는 aDispatcher->mContext 배열 주소중 하나여야 함.
     */
    if( sContext >= &aDispatcher->mContext[0] &&
        sContext <= &aDispatcher->mContext[aDispatcher->mMaxContextCount-1] )
    {
        sTemp = ((stlUInt64)sContext - (stlUInt64)aDispatcher->mContext);

        if( sTemp % STL_SIZEOF(ztiContext) == 0 )
        {
            if( sContext->mDispatcherTime == aDispatcher->mZlhlDispatcher->mStartTime )
            {
                *aValid = STL_TRUE;
            }
            else
            {
                /* 잘못된 address임 */
                *aValid = STL_FALSE;
            }
        }
        else
        {
            /* 잘못된 address임 */
            *aValid = STL_FALSE;
        }
    }
    else
    {
        /* 잘못된 address임 */
        *aValid = STL_FALSE;
    }

    return STL_SUCCESS;
}

/* send thread로 부터 보낼data가 있다는 event를 받아서 client로 data를 전송한다 */
stlStatus ztisProcessResponse( sllHandle         * aHandle,
                               sclQueueCommand     aCommand,
                               ztiDispatcher     * aDispatcher,
                               stlInt64            aCmUnitCount,
                               sllEnv            * aEnv )
{
    ztiContext      * sContext;
    sclHandle       * sCmlHandle;
    stlBool           sValid = STL_FALSE;

    sContext = (ztiContext *)aHandle->mUserContext;

    STL_TRY( ztisValidAddress( aHandle,
                               aDispatcher,
                               &sValid,
                               aEnv )
             == STL_SUCCESS );

    if( sValid == STL_FALSE )
    {
        /* receive한 address가 valid하지 않으면 그냥 성공 리턴한다 */
        STL_THROW( RAMP_SUCCESS );
    }

    sCmlHandle = SCL_HANDLE(aHandle);

    STL_DASSERT( sContext->mSllHandle != NULL );
    if( sContext->mSllHandle == NULL )
    {
        STL_THROW( RAMP_SUCCESS );
    }

    STL_DASSERT( aHandle == sContext->mSllHandle );
    STL_DASSERT( sContext->mUsed == STL_TRUE );
    STL_DASSERT( sContext->mJobCount == 1 );

    if( aCommand == SCL_QCMD_SERVER_FAILURE )
    {
        sCmlHandle->mSocketPeriod = STL_TRUE;

        /* retry send data 하지 않도록 설정 */
        sContext->mCmUnitCountForSend = 0;

        stlAtomicDec32( (stlUInt32 *)&sContext->mJobCount );

        /**
         * stlAtomicDec32 이후에는 aContext에 접근하면 안됨.
         * main thread에서 context재사용 가능한 상태임.
         */
        sContext = NULL;

        STL_THROW( RAMP_SUCCESS );
    }

    if( aCommand == SCL_QCMD_NORMAL_PERIOD_END )
    {
        sContext->mPeriodEndForSend = STL_TRUE;
    }

    if( sContext->mCmUnitCountForSend > 0 )
    {
        /* 이미 전송대기중인 data가 있으면 그냥 성공 처리한다 */
        sContext->mCmUnitCountForSend += aCmUnitCount;

        STL_THROW( RAMP_SUCCESS );
    }

    sContext->mCmUnitCountForSend = aCmUnitCount;

    /* response시 LastTime을 update 한다 */
    sContext->mLastTime = knlGetSystemTime();

    STL_TRY( ztisResponse( aDispatcher,
                           sContext,
                           aEnv )
             == STL_SUCCESS );

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/* retry send data */
stlStatus ztisRetryResponse( ztiDispatcher     * aDispatcher,
                             sllEnv            * aEnv )
{
    stlInt32          sIdx;
    ztiContext      * sContext;
    stlBool           sFound = STL_FALSE;

    for( sIdx = 0; sIdx < aDispatcher->mMaxContextCount; sIdx++ )
    {
        sContext = &aDispatcher->mContext[sIdx];

        if( (sContext->mUsed == STL_TRUE) &&
            (sContext->mCmUnitCountForSend != 0) )
        {
            /**
             * 전송할 data있음.
             */
            sFound = STL_TRUE;

            STL_TRY( ztisResponse( aDispatcher,
                                   sContext,
                                   aEnv )
                     == STL_SUCCESS );
        }
    }

    if( sFound == STL_FALSE )
    {
        aDispatcher->mRetry = STL_FALSE;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/* retry send data */
stlStatus ztisResponse( ztiDispatcher     * aDispatcher,
                        ztiContext        * aContext,
                        sllEnv            * aEnv )
{
    sllHandle       * sHandle;
    stlErrorStack   * sErrorStack;
    stlSize           sOldSentSize;

    sErrorStack = KNL_ERROR_STACK(aEnv);

    sHandle = aContext->mSllHandle;

    while( aContext->mCmUnitCountForSend > 0 )
    {
        if( aContext->mDataForSend == NULL )
        {
            STL_TRY( sclGetRecvBuffer( &sHandle->mIpc.mResponseIpc,
                                       sHandle->mIpc.mArrayAllocator,
                                       &aContext->mDataForSend,
                                       &aContext->mDataLenForSend,
                                       SCL_ENV(aEnv) )
                     == STL_SUCCESS );
            aContext->mSentLenForSend = 0;

            if( aContext->mDataLenForSend < CML_MTU_SIZE )
            {
                aContext->mDataLenForSend = CML_MTU_SIZE;
            }
        }

        STL_DASSERT( aContext->mJobCount == 1 );

        sOldSentSize = aContext->mSentLenForSend;

        if( sclSendToSocket( &aContext->mStlContext,
                             aContext->mDataForSend,
                             aContext->mDataLenForSend,
                             &aContext->mSentLenForSend,
                             SCL_ENV(aEnv) ) == STL_FAILURE )
        {
            aContext->mIsBrokenSocket = STL_TRUE;

            STL_TRY( sclCompleteRecvBuffer( &sHandle->mIpc.mResponseIpc,
                                            sHandle->mIpc.mArrayAllocator,
                                            aContext->mDataForSend,
                                            SCL_ENV(aEnv) )
                     == STL_SUCCESS );

            aContext->mDataForSend = NULL;
            aContext->mDataLenForSend = 0;
            aContext->mSentLenForSend = 0;
            aContext->mCmUnitCountForSend--;

            if( (aContext->mCmUnitCountForSend == 0) &&
                (aContext->mPeriodEndForSend == STL_TRUE) )
            {
                aContext->mPeriodEndForSend = STL_FALSE;

                /* 마지막 cm unit 처리 SCL_QCMD_NORMAL_PERIOD_END 처리 */
                stlAtomicDec32( (stlUInt32 *)&aContext->mJobCount );

                /**
                 * stlAtomicDec32 이후에는 aContext에 접근하면 안됨.
                 * main thread에서 context재사용 가능한 상태임.
                 */
                aContext = NULL;
            }

            /* send하다 socket실패 나는 경우는 성공 처리한다 */
            if( stlGetLastErrorCode( sErrorStack ) != SCL_ERRCODE_COMMUNICATION_LINK_FAILURE )
            {
                (void) knlLogMsg( NULL,
                                  KNL_ENV(aEnv),
                                  KNL_LOG_LEVEL_WARNING,
                                  "[DISPATCHER-%d] failed to send data - session(%d) \n",
                                  aDispatcher->mId,
                                  sHandle->mSessionId );

                STL_TRY( STL_FALSE );
            }

            (void)stlPopError( KNL_ERROR_STACK(aEnv) );

            STL_THROW( RAMP_SUCCESS );
        }

        aDispatcher->mZlhlDispatcher->mTotalSendBytes += aContext->mSentLenForSend - sOldSentSize;

        if( aContext->mSentLenForSend == aContext->mDataLenForSend )
        {
            /**
             * data를 다 보냈으면 continue하여 보낼 data가 있으면 더 보냄.
             */
            STL_TRY( sclCompleteRecvBuffer( &sHandle->mIpc.mResponseIpc,
                                            sHandle->mIpc.mArrayAllocator,
                                            aContext->mDataForSend,
                                            SCL_ENV(aEnv) )
                     == STL_SUCCESS );

            aDispatcher->mZlhlDispatcher->mTotalSendUnits++;

            aContext->mDataForSend = NULL;
            aContext->mDataLenForSend = 0;
            aContext->mSentLenForSend = 0;
            aContext->mCmUnitCountForSend--;

            if( (aContext->mCmUnitCountForSend == 0) &&
                (aContext->mPeriodEndForSend == STL_TRUE) )
            {
                /* 마지막 cm unit 처리. SCL_QCMD_NORMAL_PERIOD_END 처리 */

                aContext->mPeriodEndForSend = STL_FALSE;

                /**
                 * 위에서 sclSendToSocket를 하고 stlAtomicDec32를 함으로
                 * (mJobCount를 decrease하기 전에 main thread에서 increase 될수 있음)
                 * mJobCount에 contention이 발생할수 있음으로 atomic을 사용해야 함.
                 */
                STL_DASSERT( aContext->mJobCount <= 2 );

                stlAtomicDec32( (stlUInt32 *)&aContext->mJobCount );

                /**
                 * stlAtomicDec32 이후에는 aContext에 접근하면 안됨.
                 * main thread에서 context재사용 가능한 상태임.
                 */
                aContext = NULL;

                break;
            }
        }
        else
        {
            /**
             * data를 다 못 보냈으면 break하여 block걸리지 않도록 함.
             */
            aDispatcher->mRetry = STL_TRUE;

            break;
        }
    }

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
