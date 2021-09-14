/*******************************************************************************
 * ztrrProtocol.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file ztrrProtocol.c
 * @brief GlieseTool LogMirror Receiver Protocol Routines
 */

#include <goldilocks.h>
#include <ztr.h>

extern stlBool       gRunState;
extern ztrSlaveMgr   gSlaveMgr;

extern stlBool       gZtrDoRecovery;
extern stlInt64      gZtrRecoveryFileSeq;

extern ztrCommunicateCallback gZtrCommCallback;

stlStatus ztrrDoProtocol( stlErrorStack * aErrorStack )
{
    stlBool           sDoRecovery = STL_FALSE;
    ztrRecoveryinfo   sRecoveryInfo;
    
    STL_TRY( ztrrInitProtocol( aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztrrHandShakeProtocol( &sRecoveryInfo,
                                    &sDoRecovery,
                                    aErrorStack ) == STL_SUCCESS );
    
    if( sDoRecovery == STL_TRUE )
    {
        STL_TRY( ztrmLogMsg( aErrorStack,
                             "[RECEIVER] Send Recovery Information.\n"
                             " - File Sequence No  = %ld\n"
                             " - Block Sequence No = %ld\n"
                             " - Log Group Id      = %d\n",
                             sRecoveryInfo.mLogFileSeqNo,
                             sRecoveryInfo.mLastBlockSeq,
                             sRecoveryInfo.mLogGroupId ) == STL_SUCCESS );
        
        gZtrDoRecovery      = STL_TRUE;
        gZtrRecoveryFileSeq = sRecoveryInfo.mLogFileSeqNo;
    }
    else
    {
        STL_TRY( ztrmLogMsg( aErrorStack,
                             "[RECEIVER] No Recovery Information.\n" ) == STL_SUCCESS );
        
        gZtrDoRecovery = STL_FALSE;
    }
    
    /**
     * Recovery 정보를 세팅한 후에 flusher를 깨운다.
     */
    STL_TRY( stlReleaseSemaphore( &(gSlaveMgr.mWaitFlushSem), 
                                  aErrorStack ) == STL_SUCCESS );
    
    if( ztrrRecvRedoLogProtocol( aErrorStack ) != STL_SUCCESS )
    {
        if( gRunState == STL_FALSE )
        {
            STL_TRY( ztrrEndProtocol( gSlaveMgr.mRecvHandle,
                                      aErrorStack ) == STL_SUCCESS );
        }
        
        STL_TRY( gRunState == STL_FALSE );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztrrInitProtocol( stlErrorStack * aErrorStack )
{
    ztrLogMirrorHdr  sSndHeader;
    ztrLogMirrorHdr  sRcvHeader;
    
    /**
     * Recv Request ClientType
     */
    STL_TRY( gZtrCommCallback.mRecvHdrNDataFunc( gSlaveMgr.mRecvHandle,
                                                 &sRcvHeader,
                                                 NULL,
                                                 aErrorStack ) == STL_SUCCESS );

    if( sRcvHeader.mCategory == ZTR_CATEGORY_INIT && 
        sRcvHeader.mType     == ZTR_INIT_REQUEST_CLIENT_TYPE )
    {
        stlMemcpy( gSlaveMgr.mSendBuff, ZTR_PROTOCOL_INIT_STR, STL_SIZEOF( ZTR_PROTOCOL_INIT_STR ) );
        
        sSndHeader.mCategory     = ZTR_CATEGORY_INIT;
        sSndHeader.mType         = ZTR_INIT_RESPONSE_CLIENT_TYPE;
        sSndHeader.mSize         = STL_SIZEOF( ZTR_PROTOCOL_INIT_STR );
        sSndHeader.mWriteSeq     = 0;
        sSndHeader.mCurrentPiece = ZTR_INVALID_PIECE_COUNT;
        sSndHeader.mTotalPiece   = ZTR_INVALID_PIECE_COUNT;
        
        STL_TRY( gZtrCommCallback.mSendHdrNDataFunc( gSlaveMgr.mRecvHandle,
                                                     &sSndHeader,
                                                     gSlaveMgr.mSendBuff,
                                                     aErrorStack ) == STL_SUCCESS );
    }
    else
    {
        sSndHeader.mCategory     = ZTR_CATEGORY_ERROR;
        sSndHeader.mType         = ZTR_ERROR_PROTOCOL_ERROR;
        sSndHeader.mSize         = 0;
        sSndHeader.mWriteSeq     = 0;
        sSndHeader.mCurrentPiece = ZTR_INVALID_PIECE_COUNT;
        sSndHeader.mTotalPiece   = ZTR_INVALID_PIECE_COUNT;
        
        STL_TRY( gZtrCommCallback.mSendHdrNDataFunc( gSlaveMgr.mRecvHandle,
                                                     &sSndHeader,
                                                     NULL,
                                                     aErrorStack ) == STL_SUCCESS );
        
        STL_THROW( RAMP_ERR_INVALID_PROTOCOL );
    }
    
    return STL_SUCCESS; 
      
    STL_CATCH( RAMP_ERR_INVALID_PROTOCOL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTR_ERRCODE_PROTOCOL,
                      NULL,
                      aErrorStack,
                      "Invalid Init-Protocol" );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztrrHandShakeProtocol( ztrRecoveryinfo * aRecoveryInfo,
                                 stlBool         * aDoRecovery,
                                 stlErrorStack   * aErrorStack )
{
    STL_TRY( ztrrRequestBufferInfo( aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztrrResponseRecoveryInfo( aRecoveryInfo,
                                       aDoRecovery,
                                       aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztrrRequestBufferInfo( stlErrorStack * aErrorStack )
{
    ztrLogMirrorHdr   sSndHeader;
    ztrLogMirrorHdr   sRcvHeader;
    
    /**
     * Send Request Buffer Information
     */
    sSndHeader.mCategory     = ZTR_CATEGORY_HANDSHAKE;
    sSndHeader.mType         = ZTR_HANDSHAKE_REQUEST_BUFFER_INFO;
    sSndHeader.mSize         = 0;
    sSndHeader.mWriteSeq     = 0;
    sSndHeader.mCurrentPiece = ZTR_INVALID_PIECE_COUNT;
    sSndHeader.mTotalPiece   = ZTR_INVALID_PIECE_COUNT;
    
    STL_TRY( gZtrCommCallback.mSendHdrNDataFunc( gSlaveMgr.mRecvHandle,
                                                 &sSndHeader,
                                                 NULL,
                                                 aErrorStack ) == STL_SUCCESS );
    /**
     * Recv Buffer Info.
     */
    STL_TRY( gZtrCommCallback.mRecvHdrNDataFunc( gSlaveMgr.mRecvHandle,
                                                 &sRcvHeader,
                                                 gSlaveMgr.mRecvBuff,
                                                 aErrorStack ) == STL_SUCCESS );
    
    if( sRcvHeader.mCategory == ZTR_CATEGORY_HANDSHAKE )
    {
        switch( sRcvHeader.mType )
        {
            case ZTR_HANDSHAKE_RESPONSE_BUFFER_INFO:
            {
                /**
                 * Do Recovery
                 */
                STL_WRITE_INT64( &gSlaveMgr.mRecvBuffSize, gSlaveMgr.mRecvBuff );
                break;
            }
            default:
                STL_THROW( RAMP_ERR_INVALID_PROTOCOL );
                break;
        }
    }
    else
    {
        STL_THROW( RAMP_ERR_INVALID_PROTOCOL );
    }
        
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_PROTOCOL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTR_ERRCODE_PROTOCOL,
                      NULL,
                      aErrorStack,
                      "Invalid Handshake-Protocol(BufferInfo)" );
    }
    STL_FINISH;
    
    return STL_FAILURE;
    
}

stlStatus ztrrResponseRecoveryInfo( ztrRecoveryinfo * aRecoveryInfo,
                                    stlBool         * aDoRecovery,
                                    stlErrorStack   * aErrorStack )
{
    ztrRecoveryinfo   sSndRecoveryInfo;
    stlBool           sDoRecovery = STL_FALSE;
    ztrLogMirrorHdr   sSndHeader;
    ztrLogMirrorHdr   sRcvHeader;
    
    STL_TRY( ztrrGetRecoveryInfo( aRecoveryInfo,
                                  &sDoRecovery,
                                  aErrorStack ) == STL_SUCCESS );
    
    /**
     * Recv Request Recovery Info
     */
    STL_TRY( gZtrCommCallback.mRecvHdrNDataFunc( gSlaveMgr.mRecvHandle,
                                                 &sRcvHeader,
                                                 gSlaveMgr.mRecvBuff,
                                                 aErrorStack ) == STL_SUCCESS );
    
    if( sRcvHeader.mCategory == ZTR_CATEGORY_HANDSHAKE && 
        sRcvHeader.mType     == ZTR_HANDSHAKE_REQUEST_RECOVERY_INFO )
    {
        sSndHeader.mCategory     = ZTR_CATEGORY_HANDSHAKE;
        sSndHeader.mWriteSeq     = 0;
        sSndHeader.mCurrentPiece = ZTR_INVALID_PIECE_COUNT;
        sSndHeader.mTotalPiece   = ZTR_INVALID_PIECE_COUNT;
        
        if( sDoRecovery == STL_TRUE )
        {
            STL_WRITE_INT64( &sSndRecoveryInfo.mLogFileSeqNo, &aRecoveryInfo->mLogFileSeqNo );
            STL_WRITE_INT64( &sSndRecoveryInfo.mLastBlockSeq, &aRecoveryInfo->mLastBlockSeq );
            STL_WRITE_INT16( &sSndRecoveryInfo.mLogGroupId, &aRecoveryInfo->mLogGroupId );
            
            sSndHeader.mType = ZTR_HANDSHAKE_RESPONSE_RECOVERY_INFO;
            sSndHeader.mSize = STL_SIZEOF( ztrRecoveryinfo );
            
            STL_TRY( gZtrCommCallback.mSendHdrNDataFunc( gSlaveMgr.mRecvHandle,
                                                         &sSndHeader,
                                                         (stlChar*)&sSndRecoveryInfo,
                                                         aErrorStack ) == STL_SUCCESS );
        }
        else
        {
            sSndHeader.mType = ZTR_HANDSHAKE_RESPONSE_NO_RECOVERY_INFO;
            sSndHeader.mSize = 0;
            
            STL_TRY( gZtrCommCallback.mSendHdrNDataFunc( gSlaveMgr.mRecvHandle,
                                                         &sSndHeader,
                                                         NULL,
                                                         aErrorStack ) == STL_SUCCESS );
        }
        
        *aDoRecovery = sDoRecovery;
    }
    else
    {
        sSndHeader.mCategory     = ZTR_CATEGORY_ERROR;
        sSndHeader.mType         = ZTR_ERROR_PROTOCOL_ERROR;
        sSndHeader.mSize         = 0;
        sSndHeader.mWriteSeq     = 0;
        sSndHeader.mCurrentPiece = ZTR_INVALID_PIECE_COUNT;
        sSndHeader.mTotalPiece   = ZTR_INVALID_PIECE_COUNT;
        
        STL_TRY( gZtrCommCallback.mSendHdrNDataFunc( gSlaveMgr.mRecvHandle,
                                                     &sSndHeader,
                                                     NULL,
                                                     aErrorStack ) == STL_SUCCESS );
        
        STL_THROW( RAMP_ERR_INVALID_PROTOCOL );
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_PROTOCOL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTR_ERRCODE_PROTOCOL,
                      NULL,
                      aErrorStack,
                      "Invalid Handshake-Protocol (Recovery)" );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztrrRecvRedoLogProtocol( stlErrorStack * aErrorStack )
{
    ztrLogMirrorHdr   sSndHeader;
    ztrLogMirrorHdr   sRcvHeader;
    stlBool           sIsTimeout  = STL_FALSE;
    stlInt64          sRcvBuffSize = gSlaveMgr.mRecvBuffSize;
    
    /**
     * Rcv Buffer는 Master의 LogMirror Shm Size보다 크거나 같아야 한다.
     * HandShake 단계에서 Rcv Buffer Size를 얻어오고 이 값으로 Buffer를 생성한다.
     * 기존의 Rcv Buffer는 쓰레기로 나둔다.
     */
    STL_TRY( stlAllocRegionMem( &(gSlaveMgr.mRegionMem),
                                sRcvBuffSize + STL_SIZEOF( ztrLogMirrorHdr ),
                                (void**)&gSlaveMgr.mRecvBuff,
                                aErrorStack ) == STL_SUCCESS );
    
    while( 1 )
    {
        /**
         * Recv Request 
         */
        while( 1 )
        {
            STL_TRY( gRunState == STL_TRUE );
            
            STL_TRY( gZtrCommCallback.mRecvHdrNDataWithPollFunc( gSlaveMgr.mRecvHandle,
                                                                 &sRcvHeader,
                                                                 gSlaveMgr.mRecvBuff,
                                                                 &sIsTimeout,
                                                                 aErrorStack ) == STL_SUCCESS );
            
            if( sIsTimeout == STL_TRUE )
            {
                STL_TRY( ztrrPushFirstChunkToWaitReadList( aErrorStack ) == STL_SUCCESS );
            }
            else
            {
                break;
            }
        }
        
        if( sRcvHeader.mCategory == ZTR_CATEGORY_DATA )
        {
            switch( sRcvHeader.mType )
            {
                case ZTR_DATA_SEND:
                {
                    /**
                     * ALIGN 검사
                     */
                    STL_ASSERT( sRcvHeader.mSize % 8 == 0 );
                    
                    STL_TRY( ztrrWriteDataToChunk( gSlaveMgr.mRecvBuff,
                                                   sRcvHeader.mSize,
                                                   sRcvHeader.mIsLogFileHdr,
                                                   aErrorStack ) == STL_SUCCESS );
                    
                    sSndHeader.mCategory     = ZTR_CATEGORY_DATA;
                    sSndHeader.mType         = ZTR_DATA_RECEIVE_OK;
                    sSndHeader.mSize         = 0;
                    sSndHeader.mWriteSeq     = 0;
                    sSndHeader.mCurrentPiece = ZTR_INVALID_PIECE_COUNT;
                    sSndHeader.mTotalPiece   = ZTR_INVALID_PIECE_COUNT;

                    STL_TRY( gZtrCommCallback.mSendHdrNDataFunc( gSlaveMgr.mRecvHandle,
                                                                 &sSndHeader,
                                                                 NULL,
                                                                 aErrorStack ) == STL_SUCCESS );
                    
                    break;
                }
                default:
                {
                    STL_THROW( RAMP_ERR_INVALID_PROTOCOL );
                    break;
                }
            }
        }
        else if(sRcvHeader.mCategory == ZTR_CATEGORY_CONTROL )
        {
            switch( sRcvHeader.mType )
            {
                case ZTR_CONTROL_STOP:
                {
                    STL_TRY( ztrrPushFirstChunkToWaitReadList( aErrorStack ) == STL_SUCCESS );
                            
                    STL_TRY( ztrmLogMsg( aErrorStack,
                                         "[RECEIVER] Master Stopped normally.\n" ) == STL_SUCCESS );
                    
                    STL_THROW( RAMP_STOP );
                    break;
                }
                default:
                    break;
            }
        }
        else
        {
            STL_THROW( RAMP_ERR_INVALID_PROTOCOL );
        }
    }
    
    STL_RAMP( RAMP_STOP );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_PROTOCOL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTR_ERRCODE_PROTOCOL,
                      NULL,
                      aErrorStack,
                      "Invalid Data-Protocol" );
    }
    STL_FINISH;
    
    /**
     * 대기중인 Buffer가 있다면 Flush 할 수 있도록 해야한다.
     */
    (void) ztrrPushFirstChunkToWaitReadList( aErrorStack );
    
    return STL_FAILURE;
}

stlStatus ztrrEndProtocol( void          * aHandle,
                           stlErrorStack * aErrorStack )
{
    ztrLogMirrorHdr   sSndHeader;
    
    /**
     * Send Data
     */
    sSndHeader.mCategory     = ZTR_CATEGORY_CONTROL;
    sSndHeader.mType         = ZTR_CONTROL_STOP;
    sSndHeader.mSize         = 0;
    sSndHeader.mWriteSeq     = 0;
    sSndHeader.mCurrentPiece = ZTR_INVALID_PIECE_COUNT;
    sSndHeader.mTotalPiece   = ZTR_INVALID_PIECE_COUNT;
    
    STL_TRY( gZtrCommCallback.mSendHdrNDataFunc( aHandle,
                                                 &sSndHeader,
                                                 NULL,
                                                 aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;    
}


/** @} */
