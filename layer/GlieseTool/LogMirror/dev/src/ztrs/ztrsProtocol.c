/*******************************************************************************
 * ztrsProtocol.c
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
 * @file ztrsProtocol.c
 * @brief GlieseTool LogMirror Sender Protocol Routines
 */

#include <goldilocks.h>
#include <ztr.h>

extern stlBool       gRunState;
extern ztrMasterMgr  gMasterMgr;

extern ztrCommunicateCallback gZtrCommCallback;

stlStatus ztrsDoProtocol( stlErrorStack * aErrorStack )
{
    stlBool         sDoRecovery = STL_FALSE;
    ztrRecoveryinfo sRecoveryInfo;
    
    STL_TRY( ztrsInitProtocol( aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztrsHandShakeProtocol( &sRecoveryInfo,
                                    &sDoRecovery,
                                    aErrorStack ) == STL_SUCCESS );
    
    if( sDoRecovery == STL_TRUE )
    {
        STL_TRY( ztrsDoRecovery( &sRecoveryInfo,
                                 aErrorStack ) == STL_SUCCESS );   
    }
    
    STL_TRY( ztrsSendRedoLogProtocol( aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    if( gRunState == STL_FALSE )
    {
        (void) ztrsEndProtocol( gMasterMgr.mSenderHandle,
                                aErrorStack );
    }
    
    return STL_FAILURE;
}


stlStatus ztrsInitProtocol( stlErrorStack * aErrorStack )
{
    ztrLogMirrorHdr   sSndHeader;
    ztrLogMirrorHdr   sRcvHeader;
    stlChar           sClientType[10];
    
    /**
     * Send Request ClientType
     */
    sSndHeader.mCategory     = ZTR_CATEGORY_INIT;
    sSndHeader.mType         = ZTR_INIT_REQUEST_CLIENT_TYPE;
    sSndHeader.mSize         = 0;
    sSndHeader.mWriteSeq     = 0;
    sSndHeader.mCurrentPiece = ZTR_INVALID_PIECE_COUNT;
    sSndHeader.mTotalPiece   = ZTR_INVALID_PIECE_COUNT;
    
    /**
     * Request Client Type.
     */
    STL_TRY( gZtrCommCallback.mSendHdrNDataFunc( gMasterMgr.mSenderHandle,
                                                 &sSndHeader,
                                                 NULL,
                                                 aErrorStack ) == STL_SUCCESS );
    
    /**
     * Recv Client Type.
     */
    STL_TRY( gZtrCommCallback.mRecvHdrNDataFunc( gMasterMgr.mSenderHandle,
                                                 &sRcvHeader,
                                                 sClientType,
                                                 aErrorStack ) == STL_SUCCESS );
    
    
    if( sRcvHeader.mCategory == ZTR_CATEGORY_INIT && 
        sRcvHeader.mType     == ZTR_INIT_RESPONSE_CLIENT_TYPE )
    {
        STL_TRY_THROW( sRcvHeader.mSize == STL_SIZEOF( ZTR_PROTOCOL_INIT_STR ), RAMP_ERR_INVALID_UTILITY );
        
        if( stlStrncmp( ZTR_PROTOCOL_INIT_STR, 
                        sClientType, 
                        STL_SIZEOF( ZTR_PROTOCOL_INIT_STR ) ) != 0 )
        {
            STL_THROW( RAMP_ERR_INVALID_UTILITY );
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
                      "Invalid Init-Protocol" );
    }
    STL_CATCH( RAMP_ERR_INVALID_UTILITY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTR_ERRCODE_PROTOCOL,
                      NULL,
                      aErrorStack,
                      "Invalid Utility .. Not LogMirror" );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztrsHandShakeProtocol( ztrRecoveryinfo * aRecoveryInfo,
                                 stlBool         * aDoRecovery,
                                 stlErrorStack   * aErrorStack )
{
    //BufferSize Request...
    STL_TRY( ztrsResponseBufferInfo( aErrorStack ) == STL_SUCCESS );
    
    //RecoveryInfo Request
    STL_TRY( ztrsRequestRecoveryInfoProtocol( aRecoveryInfo,
                                              aDoRecovery,
                                              aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztrsResponseBufferInfo( stlErrorStack * aErrorStack )
{
    ztrLogMirrorHdr  sSndHeader;
    ztrLogMirrorHdr  sRcvHeader;
    stlInt64         sBufferSize = 0;
    
    /**
     * Recv Request Buffer Information
     */
    STL_TRY( gZtrCommCallback.mRecvHdrNDataFunc( gMasterMgr.mSenderHandle,
                                                 &sRcvHeader,
                                                 NULL,
                                                 aErrorStack ) == STL_SUCCESS );
    
    if( sRcvHeader.mCategory == ZTR_CATEGORY_HANDSHAKE )
    {
        switch( sRcvHeader.mType )
        {
            case ZTR_HANDSHAKE_REQUEST_BUFFER_INFO:
            {
                sSndHeader.mCategory     = ZTR_CATEGORY_HANDSHAKE;
                sSndHeader.mType         = ZTR_HANDSHAKE_RESPONSE_BUFFER_INFO;
                sSndHeader.mSize         = STL_SIZEOF( stlInt64 );
                sSndHeader.mWriteSeq     = 0;
                sSndHeader.mCurrentPiece = ZTR_INVALID_PIECE_COUNT;
                sSndHeader.mTotalPiece   = ZTR_INVALID_PIECE_COUNT;
                
                /**
                 * SET BUFFER INFO
                 */
                sBufferSize = gMasterMgr.mLmInfo->mSize;
                
                STL_TRY( gZtrCommCallback.mSendHdrNDataFunc( gMasterMgr.mSenderHandle,
                                                             &sSndHeader,
                                                             (stlChar*)&sBufferSize,
                                                             aErrorStack ) == STL_SUCCESS );
                break;   
            }
            default:
                STL_THROW( RAMP_ERR_INVALID_PROTOCOL );
                break;
        }
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


stlStatus ztrsRequestRecoveryInfoProtocol( ztrRecoveryinfo * aRecoveryInfo,
                                           stlBool         * aDoRecovery,
                                           stlErrorStack   * aErrorStack )
{
    ztrLogMirrorHdr   sSndHeader;
    ztrLogMirrorHdr   sRcvHeader;
    ztrRecoveryinfo   sRcvRecoveryInfo;
    
    /**
     * Send Request Recovery Information
     */
    sSndHeader.mCategory     = ZTR_CATEGORY_HANDSHAKE;
    sSndHeader.mType         = ZTR_HANDSHAKE_REQUEST_RECOVERY_INFO;
    sSndHeader.mSize         = 0;
    sSndHeader.mWriteSeq     = 0;
    sSndHeader.mCurrentPiece = ZTR_INVALID_PIECE_COUNT;
    sSndHeader.mTotalPiece   = ZTR_INVALID_PIECE_COUNT;
    
    STL_TRY( gZtrCommCallback.mSendHdrNDataFunc( gMasterMgr.mSenderHandle,
                                                 &sSndHeader,
                                                 NULL,
                                                 aErrorStack ) == STL_SUCCESS );
    
    /**
     * Recv Recovery Info.
     */
    STL_TRY( gZtrCommCallback.mRecvHdrNDataFunc( gMasterMgr.mSenderHandle,
                                                 &sRcvHeader,
                                                 (stlChar*)&sRcvRecoveryInfo,
                                                 aErrorStack ) == STL_SUCCESS );
    
    if( sRcvHeader.mCategory == ZTR_CATEGORY_HANDSHAKE )
    {
        switch( sRcvHeader.mType )
        {
            case ZTR_HANDSHAKE_RESPONSE_RECOVERY_INFO:
            {
                /**
                 * Do Recovery
                 */
                STL_WRITE_INT64( &aRecoveryInfo->mLogFileSeqNo, &sRcvRecoveryInfo.mLogFileSeqNo );
                STL_WRITE_INT64( &aRecoveryInfo->mLastBlockSeq, &sRcvRecoveryInfo.mLastBlockSeq );
                STL_WRITE_INT16( &aRecoveryInfo->mLogGroupId, &sRcvRecoveryInfo.mLogGroupId );
                
                *aDoRecovery = STL_TRUE;
                break;
            }
            case ZTR_HANDSHAKE_RESPONSE_NO_RECOVERY_INFO:
            {
                /**
                 * Nothing To Do.
                 */
                *aDoRecovery = STL_FALSE;
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
                      "Invalid Handshake-Protocol (Recovery)" );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztrsRecoveryProtocol( stlBool         aIsLogFileHdr,
                                stlErrorStack * aErrorStack )
{
    ztrRecoveryMgr  * sRecoveryMgr = &(gMasterMgr.mRecoveryMgr);
    ztrLogMirrorHdr   sSndHeader;
    ztrLogMirrorHdr   sRcvHeader;
    
    STL_TRY_THROW( sRecoveryMgr->mWriteIdx > 0, RAMP_SKIP );
    
    /**
     * DB가 Shutdown 되었는지 확인해야 하나?
     * STL_TRY( sLmInfo->mConnected == ZTR_LOGMIRROR_CONNECTED ); 
     */
    STL_TRY( gRunState == STL_TRUE );
    
    /**
     * Header 구성
     */
    sSndHeader.mCategory     = ZTR_CATEGORY_DATA;
    sSndHeader.mType         = ZTR_DATA_SEND;
    sSndHeader.mSize         = sRecoveryMgr->mWriteIdx;
    sSndHeader.mWriteSeq     = 0;
    sSndHeader.mCurrentPiece = 1;
    sSndHeader.mTotalPiece   = 1;
    sSndHeader.mIsLogFileHdr = aIsLogFileHdr;
    
    STL_TRY( gZtrCommCallback.mSendHdrNDataFunc( gMasterMgr.mSenderHandle,
                                                 &sSndHeader,
                                                 sRecoveryMgr->mSendBuff,
                                                 aErrorStack ) == STL_SUCCESS );
    
    /**
     * Recv Recovery Info.
     */
    STL_TRY( gZtrCommCallback.mRecvHdrNDataFunc( gMasterMgr.mSenderHandle,
                                                 &sRcvHeader,
                                                 NULL,
                                                 aErrorStack ) == STL_SUCCESS );
    
    if( sRcvHeader.mCategory == ZTR_CATEGORY_DATA )
    {
        switch( sRcvHeader.mType )
        {
            case ZTR_DATA_RECEIVE_OK:
            {
                /**
                 * Nothing To Do.
                 */
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
    
    /**
     * Buffer 초기화
     */
    sRecoveryMgr->mWriteIdx = 0;
    
    STL_RAMP( RAMP_SKIP );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_PROTOCOL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTR_ERRCODE_PROTOCOL,
                      NULL,
                      aErrorStack,
                      "Invalid Recovery-Protocol" );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztrsSendRedoLogProtocol( stlErrorStack * aErrorStack )
{
    stlBool sIsTimeout = STL_FALSE;
    
    ztrLogMirrorInfo * sLmInfo = gMasterMgr.mLmInfo;
    ztrLogMirrorHdr  * sLmHdr  = gMasterMgr.mLmHdr;
    ztrLogMirrorHdr    sRcvHeader;
    stlBool            sDoRelease  = STL_FALSE;
    stlInt32           sState      = 0;
    
    if( sLmInfo->mConnected != ZTR_LOGMIRROR_CONNECTED )
    {
        sLmInfo->mConnected = ZTR_LOGMIRROR_CONNECTED;
    
        STL_TRY( ztrmLogMsg( aErrorStack,
                             "[SENDER] LogMirror Attached to Shared Memory\n" ) == STL_SUCCESS );
    }
    
    while( 1 )
    {
        STL_TRY( stlTimedAcquireSemaphore( &(sLmInfo->mWaitDataSem),
                                           STL_SET_SEC_TIME( 1 ),
                                           &sIsTimeout,
                                           aErrorStack ) == STL_SUCCESS );
        
        /**
         * DB가 Shutdown 또는 Service Give-up되었는지 확인해야 한다.
         */
        STL_TRY_THROW( sLmInfo->mConnected == ZTR_LOGMIRROR_CONNECTED, RAMP_ERR_SERVICE_FAIL ); 
        
        STL_TRY( gRunState == STL_TRUE );
        
        if ( sIsTimeout == STL_TRUE )
        {
            /**
             * Slave Stop Message Or Kill Check
             */
            STL_TRY_THROW( gZtrCommCallback.mCheckSlaveStateFunc( gMasterMgr.mSenderHandle,
                                                                  aErrorStack ) == STL_SUCCESS, RAMP_STOP );
        }
        else
        {
            ztrmAcquireSpinLock( &(sLmInfo->mSpinLock ) );
            sState = 1;
            /**
             * Send RedoLog
             */
            sLmHdr->mCategory = ZTR_CATEGORY_DATA;
            sLmHdr->mType     = ZTR_DATA_SEND;
            
            //stlPrintf("SendSize = %ld\n", sLmHdr->mSize );
            
            STL_TRY( gZtrCommCallback.mSendHdrNDataFunc( gMasterMgr.mSenderHandle,
                                                         sLmHdr,
                                                         gMasterMgr.mLogData,
                                                         aErrorStack ) == STL_SUCCESS );

            /**
             * Recv Result
             */
            STL_TRY( gZtrCommCallback.mRecvHdrNDataFunc( gMasterMgr.mSenderHandle,
                                                         &sRcvHeader,
                                                         gMasterMgr.mRecvBuff,
                                                         aErrorStack ) == STL_SUCCESS );


            if( sRcvHeader.mCategory == ZTR_CATEGORY_DATA )
            {
                switch( sRcvHeader.mType )
                {
                    case ZTR_DATA_RECEIVE_OK:
                    {
                        STL_ASSERT( sLmHdr->mCurrentPiece <= sLmHdr->mTotalPiece );
                        
                        if( sLmHdr->mCurrentPiece < sLmHdr->mTotalPiece )
                        {
                            sDoRelease = STL_TRUE;
                        }
                        else
                        {
                            sDoRelease = STL_FALSE;
                            sLmInfo->mSendSeq = sLmInfo->mWriteSeq;
                        }
                        break;
                    }   
                    default:
                        STL_THROW( RAMP_ERR_INVALID_PROTOCOL );
                        break;
                }
            }
            else if( sRcvHeader.mCategory == ZTR_CATEGORY_CONTROL )
            {
                switch( sRcvHeader.mType )
                {
                    case ZTR_CONTROL_STOP:
                    {
                        STL_THROW( RAMP_STOP );
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
            
            sState = 0;
            ztrmReleaseSpinLock( &(sLmInfo->mSpinLock ) );
            
            /**
             * Data가 나눠서 보내질 경우에 Sm layer에 데이터를 마져 채우도록 한다.
             */
            if( sDoRelease == STL_TRUE )
            {
                STL_TRY( stlReleaseSemaphore( &(sLmInfo->mWaitSendSem),
                                              aErrorStack ) == STL_SUCCESS );
            }
        }
    }
    STL_RAMP( RAMP_STOP );
    
    if( sState > 0 )
    {
        ztrmReleaseSpinLock( &(sLmInfo->mSpinLock ) );
    }
    
    /**
     * SENDER가 종료할 경우에 DISCONNECT로 세팅해야 한다.
     * Gliese가 정상적인 서비스를 할 수 있다.
     */
    sLmInfo->mConnected = ZTR_LOGMIRROR_DISCONNECTED;
    sLmInfo->mIsStarted = STL_FALSE;
    
    STL_TRY( ztrmLogMsg( aErrorStack,
                         "[SENDER] LogMirror Detached to Shared Memory\n" ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_PROTOCOL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTR_ERRCODE_PROTOCOL,
                      NULL,
                      aErrorStack,
                      "Invalid Data-Protocol" );
    }
    STL_CATCH( RAMP_ERR_SERVICE_FAIL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTR_ERRCODE_SERVICE_FAIL,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    /**
     * SENDER가 종료할 경우에 DISCONNECT로 세팅해야 한다.
     * GOLDILOCKS가 정상적인 서비스를 할 수 있다.
     */
    sLmInfo->mConnected = ZTR_LOGMIRROR_DISCONNECTED;
    sLmInfo->mIsStarted = STL_FALSE;
    
    STL_TRY( ztrmLogMsg( aErrorStack,
                         "[SENDER] LogMirror Detached to Shared Memory\n" ) == STL_SUCCESS );
    
    switch( sState )
    {
        case 1:
            ztrmReleaseSpinLock( &(sLmInfo->mSpinLock ) );
        default:
            break;
    }
    
    return STL_FAILURE;
}

stlStatus ztrsCheckSlaveState( void          * aHandle,
                               stlErrorStack * aErrorStack )
{
    stlInt32           sSigFdNum;
    ztrLogMirrorHdr    sRcvHeader;
    
    if( stlPoll( &((stlContext*)aHandle)->mPollFd,
                 1,
                 &sSigFdNum,
                 1000,
                 aErrorStack ) != STL_SUCCESS )
    {
        if( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_TIMEDOUT )
        {
            (void)stlPopError( aErrorStack );
        }
    }
    else
    {
        if( gZtrCommCallback.mRecvHdrNDataFunc( aHandle,
                                                &sRcvHeader,
                                                gMasterMgr.mRecvBuff,
                                                aErrorStack ) == STL_SUCCESS )
        {
            if( sRcvHeader.mCategory == ZTR_CATEGORY_CONTROL &&
                sRcvHeader.mType     == ZTR_CONTROL_STOP )
            {
                STL_TRY( ztrmLogMsg( aErrorStack,
                                     "[SENDER] Slave Stopped normally.\n" ) == STL_SUCCESS );
            }
        }
        else
        {
            if( stlGetLastErrorCode( aErrorStack ) == ZTR_ERRCODE_COMMUNICATION_LINK_FAILURE )
            {
                STL_TRY( ztrmLogMsg( aErrorStack,
                                     "[SENDER] Slave stopped abnormally.\n" ) == STL_SUCCESS );
            }
        }
        
        STL_TRY( STL_FALSE );
    }

    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;    
}


stlStatus ztrsCheckSlaveState4IB( void          * aHandle,
                                  stlErrorStack * aErrorStack )
{
    ztrLogMirrorHdr  sRcvHeader;
    stlBool          sIsTimeout;
    
    STL_TRY( ztrmRecvHdrNDataWithPoll4IB( aHandle,
                                          &sRcvHeader,
                                          gMasterMgr.mRecvBuff,
                                          &sIsTimeout,
                                          aErrorStack ) == STL_SUCCESS );
    
    if( sIsTimeout == STL_FALSE )
    {
        if( sRcvHeader.mCategory == ZTR_CATEGORY_CONTROL &&
            sRcvHeader.mType     == ZTR_CONTROL_STOP )
        {
            STL_TRY( ztrmLogMsg( aErrorStack,
                                 "[SENDER] Slave Stopped normally.\n" ) == STL_SUCCESS );
        }
        
        STL_TRY( STL_FALSE );
    }

    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;    
}

stlStatus ztrsEndProtocol( void          * aHandle,
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
