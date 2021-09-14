/*******************************************************************************
 * sccCommunication.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: sccCommunication.c 6676 2012-12-13 08:39:46Z egon $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stl.h>
#include <dtl.h>
#include <cml.h>
#include <knl.h>
#include <sclDef.h>
#include <scDef.h>
#include <sccCommunication.h>
#include <scpProtocol.h>

/**
 * @file sccCommunication.c
 * @brief Communication Layer Communication Component Routines
 */

/**
 * @addtogroup sccCommunication
 * @{
 */


/**
 * @brief 주어진 Socket으로부터 message를 받는다.
 * @param[in] aHandle             communication handle
 * @param[in] aErrorStack         error stack
 * @remark client에서 packet의 일부만 전송하고 대기하는 경우
 *         이 함수에서 hang걸릴수 있으나 크게 문제되지 않음.
 */
stlStatus sccRecvFromSocket( sclHandle     * aHandle,
                             stlErrorStack * aErrorStack )
{
    stlSize                 sRecvSize = 0;
    stlSize                 sSize;
    stlInt32                sSigFdNum;
    sclProtocolSentence   * sProtocolSentence = aHandle->mProtocolSentence;

    while( sRecvSize < CML_MTU_SIZE )
    {
        sSize = CML_MTU_SIZE - sRecvSize;

        if( stlRecv( STL_SOCKET_IN_CONTEXT(*(aHandle->mCommunication.mContext)),
                     sProtocolSentence->mReadStartPos + sRecvSize,
                     &sSize,
                     aErrorStack ) == STL_FAILURE )
        {
            STL_TRY_THROW( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_AGAIN,
                           RAMP_ERR_COMMUNICATION_LINK_FAILURE );
            (void)stlPopError( aErrorStack );

            STL_TRY( stlPoll( &aHandle->mCommunication.mContext->mPollFd,
                              1,
                              &sSigFdNum,
                              STL_INFINITE_TIME,
                              aErrorStack )
                     == STL_SUCCESS );
        }
        else
        {
            sRecvSize += sSize;
        }
    }

    SCP_READ_HEADER( aHandle,
                     &(sProtocolSentence->mReadPayloadSize),
                     &sProtocolSentence->mReadPeriod );

    while( sRecvSize < sProtocolSentence->mReadPayloadSize + CML_PACKET_HEADER_SIZE )
    {
        sSize = (sProtocolSentence->mReadPayloadSize + CML_PACKET_HEADER_SIZE) - sRecvSize;

        if( stlRecv( STL_SOCKET_IN_CONTEXT(*(aHandle->mCommunication.mContext)),
                     sProtocolSentence->mReadStartPos + sRecvSize,
                     &sSize,
                     aErrorStack ) == STL_FAILURE )
        {
            STL_TRY_THROW( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_AGAIN,
                           RAMP_ERR_COMMUNICATION_LINK_FAILURE );
            (void)stlPopError( aErrorStack );

            STL_TRY( stlPoll( &aHandle->mCommunication.mContext->mPollFd,
                              1,
                              &sSigFdNum,
                              STL_INFINITE_TIME,
                              aErrorStack )
                     == STL_SUCCESS );
        }

        sRecvSize += sSize;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_COMMUNICATION_LINK_FAILURE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SCL_ERRCODE_COMMUNICATION_LINK_FAILURE,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus sccRecvPacket( sclHandle     * aHandle,
                         sclEnv        * aEnv )
{
    sclProtocolSentence * sProtocolSentence = aHandle->mProtocolSentence;

    if( aHandle->mCommunication.mType == SCL_COMMUNICATION_IPC )
    {
        STL_TRY( sccRecvFromSharedMemory( aHandle,
                                          aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( sccRecvFromSocket( aHandle,
                                    KNL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );
    }

    sProtocolSentence->mReadProtocolPos = sProtocolSentence->mReadStartPos + CML_PACKET_HEADER_SIZE;
    sProtocolSentence->mReadEndPos = sProtocolSentence->mReadProtocolPos + sProtocolSentence->mReadPayloadSize;

    STL_DASSERT( aHandle->mProtocolSentence->mMemoryMgr != NULL );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus sccFlushPacket( sclHandle     * aHandle,
                          sclEnv        * aEnv )
{
    stlInt32                 sTotalSize;
    stlInt32                 sPayloadSize;
    stlInt8                  sPeriod = CML_PERIOD_CONTINUE;
    sclProtocolSentence    * sProtocolSentence = aHandle->mProtocolSentence;
    sclMemoryManager       * sMemoryMgr = sProtocolSentence->mMemoryMgr;

    if( sProtocolSentence->mPoolingFlag == STL_TRUE )
    {
        /* pooling은 tcp만 가능함 */
        STL_DASSERT( aHandle->mCommunication.mType == SCL_COMMUNICATION_TCP );

        sTotalSize = sProtocolSentence->mWriteProtocolPos - sProtocolSentence->mWriteStartPos;
        sPayloadSize = sTotalSize - CML_PACKET_HEADER_SIZE;

        STL_DASSERT( sTotalSize <= sMemoryMgr->mPacketBufferSize );

        if( sPayloadSize > 0 )
        {
            SCP_WRITE_HEADER( aHandle,
                              &(sPayloadSize),
                              &sPeriod );

            STL_TRY( scpAllocSendPoolingBuf( aHandle,
                                             aEnv )
                     == STL_SUCCESS );

            stlMemcpy( sProtocolSentence->mSendPoolingBufCur->mBuf, sProtocolSentence->mWriteStartPos, sTotalSize );

            sProtocolSentence->mSendPoolingBufCur->mSize = sTotalSize;

            sProtocolSentence->mWriteProtocolPos = sProtocolSentence->mWriteStartPos + CML_PACKET_HEADER_SIZE;
            sProtocolSentence->mWriteEndPos = sProtocolSentence->mWriteStartPos + sMemoryMgr->mPacketBufferSize;
        }
    }
    else
    {
        STL_TRY( sccSendPacketInternal( aHandle,
                                        CML_PERIOD_CONTINUE,
                                        aEnv )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus sccSendPacket( sclHandle     * aHandle,
                         sclEnv        * aEnv )
{
    aHandle->mProtocolSentence->mCachedStatementId = CML_STATEMENT_ID_UNDEFINED;

    STL_TRY( sccSendPacketInternal( aHandle,
                                    CML_PERIOD_END,
                                    aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 버퍼의 내용을 원격으로 전송한다.
 *        (shared 모드인 경우 static area를 이용하여 dispatcher로 전송한다.)
 * @param[in] aHandle       Communication Handle
 * @param[in] aPeriod       aPeriod
 * @param[in] aEnv          env
 */
stlStatus sccSendPacketInternal( sclHandle     * aHandle,
                                 stlInt8         aPeriod,
                                 sclEnv        * aEnv )
{
    stlInt32                 sTotalSize;
    stlInt32                 sPayloadSize;
    sclSendPoolingBufChunk * sBufChunk;
    sclProtocolSentence    * sProtocolSentence = aHandle->mProtocolSentence;
    sclMemoryManager       * sMemoryMgr = sProtocolSentence->mMemoryMgr;

    sTotalSize = sProtocolSentence->mWriteProtocolPos - sProtocolSentence->mWriteStartPos;

    STL_DASSERT( sTotalSize <= sMemoryMgr->mPacketBufferSize );

    if( sProtocolSentence->mSendPoolingBufHead != NULL )
    {
        /* SendPoolingBuf 처리 */
        sBufChunk = sProtocolSentence->mSendPoolingBufHead;

        while( sBufChunk != NULL )
        {
            STL_TRY( cmlSend( STL_SOCKET_IN_CONTEXT(*(aHandle->mCommunication.mContext)),
                              sBufChunk->mBuf,
                              sBufChunk->mSize,
                              STL_TRUE,     /* send mtu size */
                              KNL_ERROR_STACK(aEnv) )
                     == STL_SUCCESS );

            aHandle->mSentPacketCount++;

            sBufChunk = sBufChunk->mNext;
        }

        STL_TRY( scpRestoreSendPoolingBuf( aHandle,
                                           aEnv )
                 == STL_SUCCESS );

        sProtocolSentence->mPoolingFlag = STL_FALSE;
    }

    sPayloadSize = sTotalSize - CML_PACKET_HEADER_SIZE;

    /**
     * payload가 0인경우 packet이 안보내짐.
     * => aPeriod가 CML_PERIOD_END인 경우 packet이 안보내지면 문제 발생함.
     * => 현재 논리적으로 payload가 0인 경우 없음.
     */
    STL_DASSERT( sPayloadSize > 0 );

    if( sPayloadSize > 0 )
    {
        SCP_WRITE_HEADER( aHandle,
                          &(sPayloadSize),
                          &aPeriod );

        aHandle->mSentPacketCount++;

        if( aHandle->mCommunication.mType == SCL_COMMUNICATION_IPC )
        {
            STL_TRY( sccSendToSharedMemory( aHandle,
                                            aPeriod,
                                            aEnv )
                     == STL_SUCCESS );

            aHandle = NULL;
        }
        else
        {
            STL_TRY( cmlSend( STL_SOCKET_IN_CONTEXT(*(aHandle->mCommunication.mContext)),
                              sProtocolSentence->mWriteStartPos,
                              sTotalSize,
                              STL_TRUE,     /* send mtu size */
                              KNL_ERROR_STACK(aEnv) )
                     == STL_SUCCESS );
        }

        sProtocolSentence->mWriteProtocolPos = sProtocolSentence->mWriteStartPos + CML_PACKET_HEADER_SIZE;
        sProtocolSentence->mWriteEndPos = sProtocolSentence->mWriteStartPos + sMemoryMgr->mPacketBufferSize;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */

