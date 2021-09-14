/*******************************************************************************
 * cmgSocket.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: cmgSocket.c 6676 2012-12-13 08:39:46Z egon $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stl.h>
#include <dtl.h>
#include <cmlDef.h>
#include <cmDef.h>
#include <cmgMemory.h>
#include <cmgIpc.h>
#include <cml.h>

/**
 * @file cmgSocket.c
 * @brief Communication Layer General Routines
 */

/**
 * @addtogroup cmlGeneral
 * @{
 */

/**
 * @brief 주어진 Socket으로 message를 전송한다.
 * @param[in] aSocket     소켓 descriptor
 * @param[in] aBuffer     전송할 message가 담긴 버퍼
 * @param[in] aSize       전송할 message의 길이 및 실제로 전송한 message의 길이
 * @param[in] aMtuSize    MTU size로 보낼것인지 여부
 * @param[in] aErrorStack 에러 스택
 */
stlStatus cmgSend( stlSocket       aSocket,
                   stlChar       * aBuffer,
                   stlInt32        aSize,
                   stlBool         aMtuSize,
                   stlErrorStack * aErrorStack )
{
    stlSize sSentSize = 0;
    stlSize sSize;

    if( (aMtuSize == STL_TRUE) && (aSize < CMG_MTU_SIZE) )
    {
#ifdef STL_VALGRIND
        stlMemset( &aBuffer[aSize], 0x00, CMG_MTU_SIZE - aSize );
#endif
        aSize = CMG_MTU_SIZE;
    }

    while( sSentSize < aSize )
    {
        sSize = aSize - sSentSize;
        
        if( stlSend( aSocket,
                     aBuffer + sSentSize,
                     &sSize,
                     aErrorStack ) == STL_FAILURE )
        {
            STL_TRY_THROW( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_AGAIN,
                           RAMP_ERR_COMMUNICATION_LINK_FAILURE );
            (void)stlPopError( aErrorStack );
        }
        else
        {
            /* send를 성공했을 때만 sSentSize를 더해야 한다. */
            sSentSize += sSize;
        }
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_COMMUNICATION_LINK_FAILURE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      CML_ERRCODE_COMMUNICATION_LINK_FAILURE,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 주어진 Socket으로부터 message를 받는다. 
 * @param[in] aHandle             communication handle
 * @param[in] aErrorStack         에러 스택
 */
stlStatus cmgRecv( cmlHandle     * aHandle,
                   stlErrorStack * aErrorStack )
{
    stlSize                 sRecvSize = 0;
    stlSize                 sSize;
    cmlProtocolSentence   * sProtocolSentence = aHandle->mProtocolSentence;

    while( sRecvSize < CMG_MTU_SIZE )
    {
        sSize = CMG_MTU_SIZE - sRecvSize;
        
        if( stlRecv( STL_SOCKET_IN_CONTEXT(*(aHandle->mCommunication.mContext)),
                     sProtocolSentence->mReadStartPos + sRecvSize,
                     &sSize,
                     aErrorStack ) == STL_FAILURE )
        {
            STL_TRY_THROW( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_AGAIN,
                           RAMP_ERR_COMMUNICATION_LINK_FAILURE );
            (void)stlPopError( aErrorStack );
        }
        else
        {
            sRecvSize += sSize;
        }
    }

    STL_DASSERT( (aHandle->mSendSequence - aHandle->mRecvSequence == 1) ||
                 (128 + aHandle->mSendSequence - aHandle->mRecvSequence == 1) );

    CMG_READ_HEADER( aHandle,
                     &(sProtocolSentence->mReadPayloadSize),
                     &sProtocolSentence->mReadPeriod );

    while( sRecvSize < sProtocolSentence->mReadPayloadSize + CMG_PACKET_HEADER_SIZE )
    {
        sSize = (sProtocolSentence->mReadPayloadSize + CMG_PACKET_HEADER_SIZE) - sRecvSize;
        
        if( stlRecv( STL_SOCKET_IN_CONTEXT(*(aHandle->mCommunication.mContext)),
                     sProtocolSentence->mReadStartPos + sRecvSize,
                     &sSize,
                     aErrorStack ) == STL_FAILURE )
        {
            STL_TRY_THROW( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_AGAIN,
                           RAMP_ERR_COMMUNICATION_LINK_FAILURE );
            (void)stlPopError( aErrorStack );
        }

        sRecvSize += sSize;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_COMMUNICATION_LINK_FAILURE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      CML_ERRCODE_COMMUNICATION_LINK_FAILURE,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief 주어진 Socket으로부터 async로 message를 받는다.
 * @param[in] aContext            socket context
 * @param[in] aReadBuffer         recv buffer
 * @param[in] aBufferSize         recv buffer size
 * @param[out] aReadSize          read size
 * @param[out] aPayloadSize       payload size
 * @param[out] aPeriod            period
 * @param[out] aReceived          data를 다 받았는지 여부
 * @param[in] aErrorStack         에러 스택
 * @remark data을 다 받거나 EAGAIN이 나오면 STL_SUCCESS를 리턴한다.
 *         socket에러 발생한 경우만 STL_FAILURE 리턴한다.
 *         recv하는 buffer는 CM_UNIT_SIZE로 packet size보다 작을수 있다.
 */
stlStatus cmgRecvRequestAsync( stlContext    * aContext,
                               stlChar       * aReadBuffer,
                               stlInt64        aBufferSize,
                               stlInt32      * aReadSize,
                               stlInt32      * aPayloadSize,
                               stlUInt8      * aPeriod,
                               stlBool       * aReceived,
                               stlErrorStack * aErrorStack )
{
    stlSize     sReceiveSize;

    *aReceived = STL_FALSE;

    /**
     * 현재 MTU보다 작게 받았으면 일단 MTU만큼 받는다.
     * loop돌면서 CMG_MTU_SIZE만큼 data받다가 STL_ERRCODE_AGAIN 나오면 break
     * MTU는 CM_UNIT_SIZE보다는 큼으로 CM_UNIT_SIZE는 고려하지 않는다.
     */

    while( *aReadSize < CMG_MTU_SIZE )
    {
        /* sReceiveSize : 받을 size */
        sReceiveSize = CMG_MTU_SIZE - *aReadSize;

        if( stlRecv( STL_SOCKET_IN_CONTEXT(*aContext),
                     aReadBuffer + *aReadSize,
                     &sReceiveSize,
                     aErrorStack ) == STL_FAILURE )
        {
            if( stlGetLastErrorCode( aErrorStack ) != STL_ERRCODE_AGAIN )
            {
                /* CodeReview : 상위에서 popError를 한번만 호출하는 경우 있음으로 error가 하나만 쌓이도록 */
                (void)stlPopError( aErrorStack );
                STL_THROW( RAMP_ERR_COMMUNICATION_LINK_FAILURE );
            }

            (void)stlPopError( aErrorStack );

            /* socket에 data가 없으면 break하여 STL_SUCCESS리턴 */
            break;
        }

        *aReadSize += sReceiveSize;

        /* CMG_READ_HEADER 는 mtu size받았을때 한번만 수행 */
        if( *aReadSize == CMG_MTU_SIZE )
        {
            STL_WRITE_INT32( aPayloadSize, aReadBuffer );
            CML_GET_PERIOD( *aPeriod, aReadBuffer[4] );

            STL_DASSERT( CMG_PACKET_HEADER_SIZE + *aPayloadSize <= aBufferSize );
        }
    }

    /* MTU size만큼 못받았으면 STL_ERRCODE_AGAIN 처리 */
    STL_TRY_THROW( *aReadSize >= CMG_MTU_SIZE, RAMP_FINISH );

    /**
     * 추가로 Payload부분을 받는다.
     * loop돌면서 packet받다가 STL_ERRCODE_AGAIN 나오면 break
     */
    while( *aReadSize < (*aPayloadSize + CMG_PACKET_HEADER_SIZE) )
    {
        /* 총 받아야 하는 size */
        sReceiveSize = (*aPayloadSize + CMG_PACKET_HEADER_SIZE) - *aReadSize;

        if( stlRecv( STL_SOCKET_IN_CONTEXT(*aContext),
                     aReadBuffer + (*aReadSize),
                     &sReceiveSize,
                     aErrorStack ) == STL_FAILURE )
        {
            if( stlGetLastErrorCode( aErrorStack ) != STL_ERRCODE_AGAIN )
            {
                /* CodeReview : 상위에서 popError를 한번만 호출하는 경우 있음으로 error가 하나만 쌓이도록 */
                (void)stlPopError( aErrorStack );
                STL_THROW( RAMP_ERR_COMMUNICATION_LINK_FAILURE );
            }

            (void)stlPopError( aErrorStack );

            /* socket에 data가 없으면 break하여 STL_SUCCESS리턴 */
            break;
        }

        *aReadSize += sReceiveSize;
    }

    if(  (*aReadSize) >= (*aPayloadSize) + CMG_PACKET_HEADER_SIZE )
    {
        *aReceived = STL_TRUE;
    }

    STL_RAMP( RAMP_FINISH );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_COMMUNICATION_LINK_FAILURE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      CML_ERRCODE_COMMUNICATION_LINK_FAILURE,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
