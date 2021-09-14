/*******************************************************************************
 * sccIpc.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: sccIpc.c 6676 2012-12-13 08:39:46Z egon $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stl.h>
#include <dtl.h>
#include <knl.h>
#include <scl.h>
#include <cmlDef.h>
#include <scDef.h>
#include <sccCommunication.h>
/**
 * @file sccIpc.c
 * @brief Communication Layer IPC Component Routines
 */

/**
 * @addtogroup sccGeneral
 * @{
 */


/**
 * @brief array를 이용한 queue를 초기화 한다.
 * @param[in]  aQueue       Queue Handle
 * @param[in]  aItemSize    Queue의 item 한개의 크기
 * @param[in]  aItemCount   Queue에 저장되는 item 개수
 * @param[out] aUsedMemory  Queue에서 총 사용하는 메모리 byte
 * @param[in]  aEnv         env
 */
stlStatus sccInitializeQueue( sclQueue        * aQueue,
                              stlInt64          aItemSize,
                              stlInt64          aItemCount,
                              stlInt64        * aUsedMemory,
                              sclEnv          * aEnv )
{
    stlInt32        sState = 0;

    aQueue->mRear      = 0;
    aQueue->mFront     = 0;
    aQueue->mItemCount = aItemCount;
    aQueue->mItemSize  = aItemSize + STL_SIZEOF(stlTime);

    STL_TRY( stlCreateSemaphore( &aQueue->mEventSemaphore,
                                 "",
                                 0,
                                 KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    sState = 1;

    STL_TRY( stlCreateSemaphore( &aQueue->mLockForPush,
                                 "scc",
                                 1,
                                 KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );
    sState = 2;

    /**
     * performance view용으로 item에 기본적으로 time 포함
     */
    *aUsedMemory = STL_SIZEOF(sclQueue) + aQueue->mItemCount * aQueue->mItemSize;

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            stlDestroySemaphore( &aQueue->mLockForPush, KNL_ERROR_STACK(aEnv) );
        case 1:
            stlDestroySemaphore( &aQueue->mEventSemaphore, KNL_ERROR_STACK(aEnv) );
        default:
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief array를 이용한 process간 queue를 종료 한다.
 * @param[in]  aQueue       Queue Handle
 * @param[in]  aEnv         env
 */
stlStatus sccFinalizeQueue( sclQueue        * aQueue,
                            sclEnv          * aEnv )
{
    (void)stlDestroySemaphore( &aQueue->mLockForPush, KNL_ERROR_STACK(aEnv) );

    (void)stlDestroySemaphore( &aQueue->mEventSemaphore, KNL_ERROR_STACK(aEnv) );

    return STL_SUCCESS;
}

/**
 * @brief array를 이용한 process간 queue에 data를 넣는다.
 * @param[in]  aQueue       Queue Handle
 * @param[in]  aItem        넣을 data
 * @param[in]  aEnv         env
 */
stlStatus sccEnqueue( sclQueue        * aQueue,
                      void            * aItem,
                      sclEnv          * aEnv )
{
    stlInt32      sState = 0;
    stlChar     * sItem;
    stlTime       sTime;

    KNL_ENTER_CRITICAL_SECTION( aEnv );

    while( STL_TRUE )
    {
        KNL_CHECK_THREAD_CANCELLATION( KNL_ENV(aEnv) );

        /* push용 lock */
        STL_TRY( stlAcquireSemaphore( &aQueue->mLockForPush,
                                      KNL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );
        sState = 1;

        if( (aQueue->mFront - aQueue->mRear) != aQueue->mItemCount )
        {
            break;
        }

        /**
         * queue에 item을 넣을 공간이 없으면 lock풀고 대기한다.
         */
        sState = 0;
        STL_TRY( stlReleaseSemaphore( &aQueue->mLockForPush,
                                      KNL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );

        stlSleep( STL_SET_MSEC_TIME(100) );
    }

    sItem = SCL_GET_FIRST_ITEM_FROM_QUEUE(aQueue);
    sItem += (aQueue->mFront % aQueue->mItemCount) * aQueue->mItemSize;

    sTime = knlGetSystemTime();
    stlMemcpy( sItem, &sTime, STL_SIZEOF(stlTime) );
    stlMemcpy( sItem + STL_SIZEOF(sTime), aItem, aQueue->mItemSize - STL_SIZEOF(stlTime) );

    aQueue->mFront++;
    aQueue->mTotalQueue++;
    aQueue->mQueued = aQueue->mFront - aQueue->mRear;

    /* push용 unlock */
    sState = 0;
    STL_TRY( stlReleaseSemaphore( &aQueue->mLockForPush,
                                  KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    /* event 전달 */
    STL_TRY( stlReleaseSemaphore( &aQueue->mEventSemaphore,
                                  KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    KNL_LEAVE_CRITICAL_SECTION( aEnv );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void)stlReleaseSemaphore( &aQueue->mLockForPush,
                                       KNL_ERROR_STACK(aEnv) );
        default:
            break;
    }

    KNL_LEAVE_CRITICAL_SECTION( aEnv );

    return STL_FAILURE;
}

/**
 * @brief array를 이용한 process간 queue에서 data를 꺼낸다.
 * @param[in]  aQueue       Queue Handle
 * @param[in]  aItem        꺼낸 data 저장할 주소
 * @param[in] aTimeout      대기 시간 (usec)
 * @param[in]  aEnv         env
 */
stlStatus sccDequeue( sclQueue          * aQueue,
                      void              * aItem,
                      stlInterval         aTimeout,
                      sclEnv            * aEnv )
{
    stlChar     * sItem;
    stlBool       sTimeout;
    stlUInt64     sRear;
    stlUInt64     sOldRear;
    stlTime       sTime;
    stlTime       sNow;

    KNL_ENTER_CRITICAL_SECTION( aEnv );

    /* sTimeout동안 event를 받는다 */
    STL_TRY( stlTimedAcquireSemaphore( &aQueue->mEventSemaphore,
                                       aTimeout,
                                       &sTimeout,
                                       KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    /**
     * timeout 이 되도 그냥 리턴하지 않고 data가 있는지 확인한다.
     * event가 와도 data가 없으면 STL_ERRCODE_AGAIN 리턴한다
     */

    while( 1 )
    {
        sRear = aQueue->mRear;
        
        /* timeout 이거나 event받거나 상관 없이 queue에서 data를 꺼낸다 */
        STL_TRY_THROW( aQueue->mFront > aQueue->mRear, RAMP_ERROR_EMPTY );

        sItem = SCL_GET_FIRST_ITEM_FROM_QUEUE(aQueue);
        sItem += (sRear % aQueue->mItemCount) * aQueue->mItemSize;

        stlMemcpy( &sTime, sItem, STL_SIZEOF(stlTime) );
        stlMemcpy( aItem, sItem + STL_SIZEOF(stlTime), aQueue->mItemSize - STL_SIZEOF(stlTime) );

        sOldRear = stlAtomicCas64( &aQueue->mRear, sRear+1, sRear );

        if( sRear == sOldRear )
        {
            break;
        }
    }

    /**
     * Queue에 대기시간
     */
    sNow = knlGetSystemTime();
    aQueue->mWait += sNow - sTime;

    aQueue->mQueued = aQueue->mFront - aQueue->mRear;

    KNL_LEAVE_CRITICAL_SECTION( aEnv );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERROR_EMPTY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_AGAIN,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }
    STL_FINISH;

    KNL_LEAVE_CRITICAL_SECTION( aEnv );

    return STL_FAILURE;
}


stlStatus sccInitializeIpcSimplex( sclIpcSimplex       * aIpcSimplex,
                                   stlArrayAllocator   * aArrayAllocator,
                                   sclEnv              * aEnv )
{
    stlInt32        sState = 0;
    sclIpcPacket  * sNewElement;
    stlInt32        sIdx;

    sState = 1;
    for( sIdx = 0; sIdx < SCL_IPC_CACHE_COUNT; sIdx++ )
    {
        aIpcSimplex->mCache[sIdx].mPacket = NULL;

        STL_TRY( sccTimedAllocElement( aArrayAllocator,
                                       (void **)&aIpcSimplex->mCache[sIdx].mPacket,
                                       SCC_ALLOC_ELEMENT_TIMEOUT,
                                       aEnv )
                 == STL_SUCCESS );

        aIpcSimplex->mCache[sIdx].mPacket->mCacheIdx = sIdx;
        aIpcSimplex->mCache[sIdx].mIsUsed = STL_FALSE;
    }

    STL_DASSERT( SCL_IPC_CACHE_COUNT > 0 );

    sNewElement = aIpcSimplex->mCache[0].mPacket;
    aIpcSimplex->mCache[0].mIsUsed = STL_TRUE;

    sNewElement->mNext = NULL;
    sNewElement->mSize = 0;

    aIpcSimplex->mReadPosition = sNewElement;
    aIpcSimplex->mWritePosition = sNewElement;
    aIpcSimplex->mAllocatedCmUnitCount = 1;
    aIpcSimplex->mFreedCmUnitCount = 0;

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            for( sIdx = 0; sIdx < SCL_IPC_CACHE_COUNT; sIdx++ )
            {
                if( aIpcSimplex->mCache[sIdx].mPacket == NULL )
                {
                    break;
                }

                (void)sccFreeElement( aArrayAllocator,
                                      (void *)aIpcSimplex->mCache[sIdx].mPacket,
                                      aEnv );
            }
        default:
            break;
    }

    return STL_FAILURE;
}


stlStatus sccFinalizeIpcSimplex( sclIpcSimplex       * aIpcSimplex,
                                 stlArrayAllocator   * aArrayAllocator,
                                 sclEnv              * aEnv )
{
    stlInt32        sIdx;
    sclIpcPacket  * sNext;

    /* aIpcSimplex 메모리를 mPacketAllocator에 반환한다. */
    while( aIpcSimplex->mReadPosition != NULL )
    {
        sNext = aIpcSimplex->mReadPosition->mNext;

        /* cache가 아닌것만 free */
        if( aIpcSimplex->mReadPosition->mCacheIdx == -1 )
        {
            STL_TRY( sccFreeElement( aArrayAllocator,
                                     (void *)aIpcSimplex->mReadPosition,
                                     aEnv )
                     == STL_SUCCESS );
        }

        aIpcSimplex->mReadPosition = sNext;
    }
    aIpcSimplex->mReadPosition = NULL;

    /* cache는 여기서 free */
    for( sIdx = 0; sIdx < SCL_IPC_CACHE_COUNT; sIdx++ )
    {
        STL_TRY( sccFreeElement( aArrayAllocator,
                                 (void *)aIpcSimplex->mCache[sIdx].mPacket,
                                 aEnv )
                 == STL_SUCCESS );

        aIpcSimplex->mCache[sIdx].mPacket = NULL;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus sccGetSendBuffer( sclIpcSimplex       * aIpcSimplex,
                            stlArrayAllocator   * aArrayAllocator,
                            stlChar            ** aBuf,
                            sclEnv              * aEnv )
{
    sclIpcPacket  * sNewElement = NULL;
    stlInt32        sIdx = 0;
    stlInt64        sMaxSizePerSession = 0;

    sMaxSizePerSession = knlGetPropertyBigIntValueByID( KNL_PROPERTY_MAXIMUM_SESSION_CM_BUFFER_SIZE,
                                                        KNL_ENV(aEnv) );

    /* cache에서 먼저 찾는다 */

    for( sIdx = 0; sIdx < SCL_IPC_CACHE_COUNT; sIdx++ )
    {
        if( aIpcSimplex->mCache[sIdx].mIsUsed == STL_FALSE )
        {
            sNewElement = aIpcSimplex->mCache[sIdx].mPacket;
            aIpcSimplex->mCache[sIdx].mIsUsed = STL_TRUE;

            STL_DASSERT( sNewElement->mCacheIdx == sIdx );

            break;
        }
    }

    if( sNewElement == NULL )
    {
        /**
         * sclIpcSimplex당 cm unit buffer 사용량 제한
         */
        if( ((aIpcSimplex->mAllocatedCmUnitCount - aIpcSimplex->mFreedCmUnitCount) * aArrayAllocator->mElementSize) > sMaxSizePerSession )
        {
            STL_THROW( RAMP_ERR_OUT_OF_CM_UNIT_MEMORY );
        }

        STL_TRY( sccTimedAllocElement( aArrayAllocator,
                                       (void **)&sNewElement,
                                       SCC_ALLOC_ELEMENT_TIMEOUT,
                                       aEnv )
                 == STL_SUCCESS );

        sNewElement->mCacheIdx = -1;
    }

    sNewElement->mNext = NULL;
    sNewElement->mSize = 0;

    aIpcSimplex->mWritePosition->mNext = sNewElement;

    *aBuf = SCL_IPC_GET_BODY_FROM_HEADER( aIpcSimplex->mWritePosition );
    aIpcSimplex->mAllocatedCmUnitCount++;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_OUT_OF_CM_UNIT_MEMORY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SCL_ERRCODE_OUT_OF_CM_UNIT_MEMORY,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus sccCompleteSendBuffer( sclIpcSimplex      * aIpcSimplex,
                                 stlArrayAllocator  * aArrayAllocator,
                                 stlChar            * aBuffer,
                                 stlSize              aLen,
                                 sclEnv             * aEnv )
{
    STL_DASSERT( aLen != 0 );
    STL_DASSERT( aBuffer == SCL_IPC_GET_BODY_FROM_HEADER( aIpcSimplex->mWritePosition ) );

    aIpcSimplex->mWritePosition->mSize = aLen;
    aIpcSimplex->mWritePosition = aIpcSimplex->mWritePosition->mNext;

    return STL_SUCCESS;
}


stlStatus sccGetRecvBuffer( sclIpcSimplex       * aIpcSimplex,
                            stlArrayAllocator   * aArrayAllocator,
                            stlChar            ** aBuf,
                            stlInt32            * aSize,
                            sclEnv              * aEnv )
{
    STL_DASSERT( aIpcSimplex->mReadPosition->mSize > 0 );

    *aBuf = SCL_IPC_GET_BODY_FROM_HEADER( aIpcSimplex->mReadPosition );
    *aSize = aIpcSimplex->mReadPosition->mSize;
    aIpcSimplex->mReadPosition->mSize = 0;

    return STL_SUCCESS;
}

stlStatus sccCompleteRecvBuffer( sclIpcSimplex      * aIpcSimplex,
                                 stlArrayAllocator  * aArrayAllocator,
                                 stlChar            * aBuffer,
                                 sclEnv             * aEnv)
{
    sclIpcPacket   * sHeadNext;

    STL_DASSERT( SCL_IPC_GET_BODY_FROM_HEADER( aIpcSimplex->mReadPosition ) == aBuffer );

    sHeadNext = aIpcSimplex->mReadPosition->mNext;

    STL_DASSERT( sHeadNext != NULL );

    /* free할 메모리가 cache면 free하지 않고 unused로 변경한다 */
    if( aIpcSimplex->mReadPosition->mCacheIdx != -1 )
    {
#if defined( STL_DEBUG )
        /**
         * 동시성 문제 발생 가능성이 있어서 ASSERT가 발생하도록 debug모드에서 값을 설정함.
         */
        aIpcSimplex->mReadPosition->mNext = NULL;
        aIpcSimplex->mReadPosition->mSize = -1;
        stlMemBarrier();
#endif
        aIpcSimplex->mCache[aIpcSimplex->mReadPosition->mCacheIdx].mIsUsed = STL_FALSE;
    }
    else
    {
#if defined( STL_DEBUG )
        /**
         * 동시성 문제 발생 가능성이 있어서 ASSERT가 발생하도록 debug모드에서 값을 설정함.
         */
        aIpcSimplex->mReadPosition->mNext = NULL;
        aIpcSimplex->mReadPosition->mCacheIdx = 0xfdfd;
        aIpcSimplex->mReadPosition->mSize = -1;
        stlMemBarrier();
#endif

        STL_TRY( sccFreeElement( aArrayAllocator,
                                 (void *)aIpcSimplex->mReadPosition,
                                 aEnv )
                 == STL_SUCCESS );
    }

    aIpcSimplex->mReadPosition = sHeadNext;
    aIpcSimplex->mFreedCmUnitCount++;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief sharedserver에서 shared-memory로 data를 전송한다.
 * @param[in] aHandle       Communication Handle
 * @param[in] aPeriod       aPeriod
 * @param[in] aEnv   Error Stack Pointer
 * @remark 실제로는 mWriteStartPos가 이미 static area이기 때문에
 *         mWriteStartPos에 새로운 static area buffer를 할당만 한다.
 */
stlStatus sccSendToSharedMemory( sclHandle     * aHandle,
                                 stlInt8         aPeriod,
                                 sclEnv        * aEnv )
{
    sclProtocolSentence * sProtocolSentence = aHandle->mProtocolSentence;
    stlArrayAllocator   * sArrayAllocator   = aHandle->mCommunication.mIpc->mArrayAllocator;
    stlInt64              sNetBufferSize;

    sNetBufferSize = knlGetPropertyBigIntValueByID( KNL_PROPERTY_NET_BUFFER_SIZE,
                                                    KNL_ENV(aEnv) );

    STL_TRY( sccCompleteSendBuffer( &aHandle->mCommunication.mIpc->mResponseIpc,
                                    sArrayAllocator,
                                    sProtocolSentence->mWriteStartPos,
                                    sProtocolSentence->mWriteProtocolPos - sProtocolSentence->mWriteStartPos,
                                    aEnv)
             == STL_SUCCESS );

    sProtocolSentence->mPendingCmUnitCount++;

    if( aPeriod == CML_PERIOD_CONTINUE )
    {
        /**
         * response시 enqueue는 protocol sentence이거나 netbuffer size이상일때 한다.
         */

        if( sProtocolSentence->mPendingCmUnitCount >= (sNetBufferSize / sArrayAllocator->mBodySize) )
        {
            STL_TRY( sclEnqueueResponse( aHandle->mCommunication.mIpc->mResponseEvent,
                                         aHandle,
                                         SCL_QCMD_NORMAL_PERIOD_CONTINUE,
                                         sProtocolSentence->mPendingCmUnitCount,
                                         aEnv )
                     == STL_SUCCESS );

            sProtocolSentence->mPendingCmUnitCount = 0;
        }

        /* aHandle->mWriteStartPos를 새로 할당 한다 */
        STL_TRY( sccGetSendBuffer( &aHandle->mCommunication.mIpc->mResponseIpc,
                                   aHandle->mCommunication.mIpc->mArrayAllocator,
                                   &sProtocolSentence->mWriteStartPos,
                                   aEnv)
                 == STL_SUCCESS );
    }
    else
    {
        aHandle->mProtocolSentence = NULL;
        STL_TRY( sclEnqueueResponse( aHandle->mCommunication.mIpc->mResponseEvent,
                                     aHandle,
                                     SCL_QCMD_NORMAL_PERIOD_END,
                                     sProtocolSentence->mPendingCmUnitCount,
                                     aEnv )
                 == STL_SUCCESS );

        sProtocolSentence->mPendingCmUnitCount = 0;

        /* CML_PERIOD_END면 더이상 write buffer 필요 없음 */
        sProtocolSentence->mWriteStartPos = NULL;
    }

    return STL_SUCCESS;

    STL_FINISH;

    sProtocolSentence->mPendingCmUnitCount = 0;
    sProtocolSentence->mWriteStartPos = NULL;

    return STL_FAILURE;
}



stlStatus sccSendToSocket( stlContext    * aContext,
                           stlChar       * aData,
                           stlInt32        aDataLen,
                           stlSize       * aSentLen,
                           sclEnv        * aEnv )
{
    stlSize            sSendLen;

    while( *aSentLen < aDataLen )
    {
        sSendLen = aDataLen - *aSentLen;

        if( stlSend( STL_SOCKET_IN_CONTEXT(*aContext),
                     aData + *aSentLen,
                     &sSendLen,
                     KNL_ERROR_STACK(aEnv) ) == STL_FAILURE )
        {
            if( stlGetLastErrorCode( KNL_ERROR_STACK(aEnv) ) != STL_ERRCODE_AGAIN )
            {
                /* CodeReview : 상위에서 popError를 한번만 호출하는 경우 있음으로 error가 하나만 쌓이도록 */
                (void)stlPopError( KNL_ERROR_STACK(aEnv) );
                STL_THROW( RAMP_ERR_COMMUNICATION_LINK_FAILURE );
            }

            /* send에서 STL_ERRCODE_AGAIN이 나오는 경우는 buffer full이다 */
            (void)stlPopError( KNL_ERROR_STACK(aEnv) );

            break;
        }
        else
        {
            /* send를 성공했을 때만 sSentSize를 더해야 한다. */
            *aSentLen += sSendLen;
        }
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_COMMUNICATION_LINK_FAILURE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SCL_ERRCODE_COMMUNICATION_LINK_FAILURE,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief shared-memory로부터 message를 받는다.
 * @param[in]  aHandle     Communication Handle
 * @param[in]  aEnv        env
 * @remark message를 받는다는것은 aHandle->mReadStartPos, aHandle->mReadPayloadPos가 읽은 data를 가르키도록 하는것이다.
 *         (socket에서 data를 읽은것과 동일하게 처리를 하는것임. cmgRecv 대체)
 */
stlStatus sccRecvFromSharedMemory( sclHandle     * aHandle,
                                   sclEnv        * aEnv )
{
    stlInt32                sSize;
    sclProtocolSentence   * sProtocolSentence = aHandle->mProtocolSentence;

    if( sProtocolSentence->mReadStartPos != NULL )
    {
        STL_TRY( sccCompleteRecvBuffer( &aHandle->mCommunication.mIpc->mRequestIpc,
                                        aHandle->mCommunication.mIpc->mArrayAllocator,
                                        sProtocolSentence->mReadStartPos,
                                        aEnv )
                 == STL_SUCCESS );
    }

    STL_TRY( sclGetRecvBuffer( &aHandle->mCommunication.mIpc->mRequestIpc,
                               aHandle->mCommunication.mIpc->mArrayAllocator,
                               &sProtocolSentence->mReadStartPos,
                               &sSize,
                               aEnv )
             == STL_SUCCESS );

    STL_DASSERT( aHandle->mSendSequence == aHandle->mRecvSequence );

    SCP_READ_HEADER( aHandle,
                     &sProtocolSentence->mReadPayloadSize,
                     &sProtocolSentence->mReadPeriod );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus sccTimedAllocElement( stlArrayAllocator   * aArrayAllocator,
                                void               ** aElement,
                                stlInterval           aTimeout,
                                sclEnv              * aEnv )
{
    stlUInt64       sCount = 0;

    /* 1/10 second간격으로 (aTimeout / 100000)만큼 loop */
    while( STL_TRUE )
    {
        KNL_CHECK_THREAD_CANCELLATION( KNL_ENV(aEnv) );

        /* session이 죽었는지 체크 */
        STL_TRY( knlCheckEndSession( KNL_SESS_ENV(aEnv), KNL_ENV(aEnv) ) == STL_SUCCESS );

        KNL_ENTER_CRITICAL_SECTION( aEnv );

        if( stlAllocElement( aArrayAllocator,
                             (void **)aElement,
                             KNL_ERROR_STACK(aEnv) ) == STL_SUCCESS )
        {
            KNL_LEAVE_CRITICAL_SECTION( aEnv );

            break;
        }
        else
        {
            KNL_LEAVE_CRITICAL_SECTION( aEnv );

            /* STL_ERRCODE_OUT_OF_MEMORY가 아니면 error 처리 */
            STL_TRY( stlGetLastErrorCode( KNL_ERROR_STACK(aEnv) ) == STL_ERRCODE_OUT_OF_MEMORY );

            if( (aTimeout ==  STL_INFINITE_TIME) ||
                (sCount < (aTimeout / 100000) ) )
            {
                /* 재시도 */
                (void)stlPopError( KNL_ERROR_STACK(aEnv) );

                sCount++;
                stlSleep( STL_SET_MSEC_TIME(100) );
            }
            else
            {
                /**
                 * timeout이 되었음으로 error 처리
                 * timeout시 STL_ERRCODE_OUT_OF_MEMORY로 처리됨.
                 */
                STL_TRY( STL_FALSE );
            }
        }
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus sccFreeElement( stlArrayAllocator   * aArrayAllocator,
                          void                * aElement,
                          sclEnv              * aEnv )
{
    KNL_ENTER_CRITICAL_SECTION( aEnv );

    STL_TRY( stlFreeElement( aArrayAllocator,
                             aElement,
                             KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    KNL_LEAVE_CRITICAL_SECTION( aEnv );

    return STL_SUCCESS;

    STL_FINISH;

    KNL_LEAVE_CRITICAL_SECTION( aEnv );

    return STL_FAILURE;
}

/** @} */
