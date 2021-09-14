/*******************************************************************************
 * ztrmCommunicate.c
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
 * @file ztrmCommunicate.c
 * @brief GlieseTool LogMirror Communicate Routines
 */

#include <goldilocks.h>
#include <ztr.h>

extern ztrConfigure  gZtrConfigure;
extern stlBool       gRunState;

stlChar * gZtrSendBuff = NULL;
stlChar * gZtrRecvBuff  = NULL;

ztrCommunicateCallback gZtrCommCallback;

void ztrmSetCommunicateCallback( ztrCommunicateCallback * aCallback )
{
    gZtrCommCallback.mCreateListenerFunc       = aCallback->mCreateListenerFunc;
    gZtrCommCallback.mCreateReceiverFunc       = aCallback->mCreateReceiverFunc;
    gZtrCommCallback.mDestroyHandleFunc        = aCallback->mDestroyHandleFunc;
    gZtrCommCallback.mSendHdrNDataFunc         = aCallback->mSendHdrNDataFunc;
    gZtrCommCallback.mRecvHdrNDataFunc         = aCallback->mRecvHdrNDataFunc;
    gZtrCommCallback.mRecvHdrNDataWithPollFunc = aCallback->mRecvHdrNDataWithPollFunc;
    gZtrCommCallback.mCheckSlaveStateFunc      = aCallback->mCheckSlaveStateFunc;
}

stlStatus ztrmSendHdrNData( void            * aHandle,
                            ztrLogMirrorHdr * aHeader,
                            stlChar         * aData,
                            stlErrorStack   * aErrorStack )
{
    ztrLogMirrorHdr   sHeader;
    
    STL_WRITE_INT32( &sHeader.mCategory, &aHeader->mCategory );
    STL_WRITE_INT32( &sHeader.mType, &aHeader->mType );
    STL_WRITE_INT64( &sHeader.mSize, &aHeader->mSize );
    STL_WRITE_INT64( &sHeader.mWriteSeq, &aHeader->mWriteSeq );
    STL_WRITE_INT32( &sHeader.mCurrentPiece, &aHeader->mCurrentPiece );
    STL_WRITE_INT32( &sHeader.mTotalPiece, &aHeader->mTotalPiece );
    STL_WRITE_INT8 ( &sHeader.mIsLogFileHdr, &aHeader->mIsLogFileHdr );
    
    STL_TRY( ztrmSendData( (stlContext*)aHandle,
                           (stlChar*)&sHeader,
                           STL_SIZEOF( ztrLogMirrorHdr ),
                           aErrorStack ) == STL_SUCCESS );
    
    if( aData != NULL )
    {
        STL_TRY( ztrmSendData( (stlContext*)aHandle,
                               aData,
                               aHeader->mSize,
                               aErrorStack ) == STL_SUCCESS );
    }
                           
    return STL_SUCCESS;
       
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztrmSendHdrNData4IB( void            * aHandle,
                               ztrLogMirrorHdr * aHeader,
                               stlChar         * aData,
                               stlErrorStack   * aErrorStack )
{
    ztrLogMirrorHdr   sHeader;
    
    STL_WRITE_INT32( &sHeader.mCategory, &aHeader->mCategory );
    STL_WRITE_INT32( &sHeader.mType, &aHeader->mType );
    STL_WRITE_INT64( &sHeader.mSize, &aHeader->mSize );
    STL_WRITE_INT64( &sHeader.mWriteSeq, &aHeader->mWriteSeq );
    STL_WRITE_INT32( &sHeader.mCurrentPiece, &aHeader->mCurrentPiece );
    STL_WRITE_INT32( &sHeader.mTotalPiece, &aHeader->mTotalPiece );
    STL_WRITE_INT8 ( &sHeader.mIsLogFileHdr, &aHeader->mIsLogFileHdr );
    
    STL_TRY( ztrmSendData4IB( aHandle,
                              (stlChar*)&sHeader,
                              STL_SIZEOF( ztrLogMirrorHdr ),
                              aErrorStack ) == STL_SUCCESS );
    
    if( aData != NULL )
    {
        STL_TRY( ztrmSendData4IB( (stlContext*)aHandle,
                                  aData,
                                  aHeader->mSize,
                                  aErrorStack ) == STL_SUCCESS );
    }
                           
    return STL_SUCCESS;
       
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztrmRecvHdrNData( void            * aHandle,
                            ztrLogMirrorHdr * aHeader,
                            stlChar         * aData,
                            stlErrorStack   * aErrorStack )
{
    stlInt32          sSigFdNum;
    ztrLogMirrorHdr   sHeader;
    
    while( 1 )
    {
        if( stlPoll( &((stlContext*)aHandle)->mPollFd,
                     1,
                     &sSigFdNum,
                     STL_SET_SEC_TIME( 1 ),
                     aErrorStack ) == STL_SUCCESS )
        {
            break;
        }
        else
        {
            STL_TRY( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_TIMEDOUT );
            (void)stlPopError( aErrorStack );
            
            STL_TRY( gRunState == STL_TRUE );
        }
    }
    
    STL_TRY( ztrmRecvData( (stlContext*)aHandle, 
                           (stlChar*)&sHeader,
                           STL_SIZEOF( ztrLogMirrorHdr ),
                           aErrorStack ) == STL_SUCCESS );
    
    STL_WRITE_INT32( &aHeader->mCategory, &sHeader.mCategory );
    STL_WRITE_INT32( &aHeader->mType, &sHeader.mType );
    STL_WRITE_INT64( &aHeader->mSize, &sHeader.mSize );
    STL_WRITE_INT64( &aHeader->mWriteSeq, &sHeader.mWriteSeq );
    STL_WRITE_INT32( &aHeader->mCurrentPiece, &sHeader.mCurrentPiece );
    STL_WRITE_INT32( &aHeader->mTotalPiece, &sHeader.mTotalPiece );
    STL_WRITE_INT8(  &aHeader->mIsLogFileHdr, &sHeader.mIsLogFileHdr );
    
    if( aHeader->mSize > 0 )
    {
        //STL_ASSERT( aData != NULL );
        
        STL_TRY( ztrmRecvData( (stlContext*)aHandle, 
                               aData,
                               aHeader->mSize,
                               aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztrmRecvHdrNData4IB( void            * aHandle,
                               ztrLogMirrorHdr * aHeader,
                               stlChar         * aData,
                               stlErrorStack   * aErrorStack )
{
    ztrLogMirrorHdr   sHeader;
    stlBool           sIsTimeout = STL_FALSE;
    
    while( 1 )
    {
        STL_TRY( ztrmRecvData4IB( aHandle,
                                  (stlChar*)&sHeader,
                                  STL_SIZEOF( ztrLogMirrorHdr ),
                                  STL_FALSE,
                                  &sIsTimeout,
                                  aErrorStack ) == STL_SUCCESS );
        
        if( sIsTimeout == STL_FALSE )
        {
            break;   
        }
        
        STL_TRY( gRunState == STL_TRUE );
    }

    STL_WRITE_INT32( &aHeader->mCategory, &sHeader.mCategory );
    STL_WRITE_INT32( &aHeader->mType, &sHeader.mType );
    STL_WRITE_INT64( &aHeader->mSize, &sHeader.mSize );
    STL_WRITE_INT64( &aHeader->mWriteSeq, &sHeader.mWriteSeq );
    STL_WRITE_INT32( &aHeader->mCurrentPiece, &sHeader.mCurrentPiece );
    STL_WRITE_INT32( &aHeader->mTotalPiece, &sHeader.mTotalPiece );
    STL_WRITE_INT8(  &aHeader->mIsLogFileHdr, &sHeader.mIsLogFileHdr );
    
    if( aHeader->mSize > 0 )
    {
        while( 1 )
        {
            STL_TRY( ztrmRecvData4IB( aHandle,
                                      aData,
                                      aHeader->mSize,
                                      STL_TRUE,
                                      &sIsTimeout,
                                      aErrorStack ) == STL_SUCCESS );
            
            if( sIsTimeout == STL_FALSE )
            {
                break;   
            }
            
            STL_TRY( gRunState == STL_TRUE );
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztrmRecvHdrNDataWithPoll( void            * aHandle,
                                    ztrLogMirrorHdr * aHeader,
                                    stlChar         * aData,
                                    stlBool         * aIsTimeout,
                                    stlErrorStack   * aErrorStack )
{
    stlInt32          sSigFdNum;
    ztrLogMirrorHdr   sHeader;
    
    if( stlPoll( &((stlContext*)aHandle)->mPollFd,
                 1,
                 &sSigFdNum,
                 STL_SET_SEC_TIME( 1 ),
                 aErrorStack ) == STL_SUCCESS )
    {
        STL_TRY( ztrmRecvData( (stlContext*)aHandle, 
                               (stlChar*)&sHeader,
                               STL_SIZEOF( ztrLogMirrorHdr ),
                               aErrorStack ) == STL_SUCCESS );
    
        STL_WRITE_INT32( &aHeader->mCategory, &sHeader.mCategory );
        STL_WRITE_INT32( &aHeader->mType, &sHeader.mType );
        STL_WRITE_INT64( &aHeader->mSize, &sHeader.mSize );
        STL_WRITE_INT64( &aHeader->mWriteSeq, &sHeader.mWriteSeq );
        STL_WRITE_INT32( &aHeader->mCurrentPiece, &sHeader.mCurrentPiece );
        STL_WRITE_INT32( &aHeader->mTotalPiece, &sHeader.mTotalPiece );
        STL_WRITE_INT8(  &aHeader->mIsLogFileHdr, &sHeader.mIsLogFileHdr );
    
        if( aHeader->mSize > 0 )
        {
            //STL_ASSERT( aData != NULL );
            
            STL_TRY( ztrmRecvData( (stlContext*)aHandle, 
                                   aData,
                                   aHeader->mSize,
                                   aErrorStack ) == STL_SUCCESS );
        }
        
        *aIsTimeout = STL_FALSE;
    }
    else
    {
        STL_TRY( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_TIMEDOUT );
        (void)stlPopError( aErrorStack );
        
        *aIsTimeout = STL_TRUE;
    }
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztrmRecvHdrNDataWithPoll4IB( void            * aHandle,
                                       ztrLogMirrorHdr * aHeader,
                                       stlChar         * aData,
                                       stlBool         * aIsTimeout,
                                       stlErrorStack   * aErrorStack )
{
    ztrLogMirrorHdr   sHeader;
    stlBool           sIsTimeout = STL_FALSE;
    
    STL_TRY( ztrmRecvData4IB( aHandle,
                              (stlChar*)&sHeader,
                              STL_SIZEOF( ztrLogMirrorHdr ),
                              STL_FALSE,
                              &sIsTimeout,
                              aErrorStack ) == STL_SUCCESS );
        
    if( sIsTimeout == STL_FALSE )
    {
        STL_WRITE_INT32( &aHeader->mCategory, &sHeader.mCategory );
        STL_WRITE_INT32( &aHeader->mType, &sHeader.mType );
        STL_WRITE_INT64( &aHeader->mSize, &sHeader.mSize );
        STL_WRITE_INT64( &aHeader->mWriteSeq, &sHeader.mWriteSeq );
        STL_WRITE_INT32( &aHeader->mCurrentPiece, &sHeader.mCurrentPiece );
        STL_WRITE_INT32( &aHeader->mTotalPiece, &sHeader.mTotalPiece );
        STL_WRITE_INT8(  &aHeader->mIsLogFileHdr, &sHeader.mIsLogFileHdr );
        
        if( aHeader->mSize > 0 )
        {
            //STL_ASSERT( aData != NULL );
            while( 1 )
            {
                STL_TRY( ztrmRecvData4IB( aHandle,
                                          aData,
                                          aHeader->mSize,
                                          STL_TRUE,
                                          &sIsTimeout,
                                          aErrorStack ) == STL_SUCCESS );
            
                if( sIsTimeout == STL_FALSE )
                {
                    break;   
                }
                
                STL_TRY( gRunState == STL_TRUE );
            }
        }
        
        *aIsTimeout = STL_FALSE;
    }
    else
    {
        *aIsTimeout = STL_TRUE;
    }
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztrmSendData( stlContext    * aContext,
                        stlChar       * aData,
                        stlInt64        aSize,
                        stlErrorStack * aErrorStack )
{
    stlSize sSentSize = 0;
    stlSize sSize;
    
    while( sSentSize < aSize )
    {
        sSize = aSize - sSentSize;
        
        if( stlSend( STL_SOCKET_IN_CONTEXT( *aContext ),
                     aData + sSentSize,
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
                      ZTR_ERRCODE_COMMUNICATION_LINK_FAILURE,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztrmSendData4IB( stlRcHandle     aHandle,
                           stlChar       * aData,
                           stlInt64        aSize,
                           stlErrorStack * aErrorStack )
{
    stlInt32  sTotalSentLen = 0;
    stlSize   sSentLen = 0;

    while ( sTotalSentLen < aSize)
    {
        sSentLen = aSize - sTotalSentLen;
        if( stlRcSend( aHandle,
                       aData + sTotalSentLen,
                       &sSentLen,
                       aErrorStack ) == STL_SUCCESS )
        {
            sTotalSentLen += sSentLen;
        }
        else
        {
            /** timeout 처리 => 재시도
             *  timeout은 보낸 data가 하나고 없이 timeout된 경우임.
             */
            STL_TRY( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_TIMEDOUT );
            (void)stlPopError( aErrorStack );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE; 
}



stlStatus ztrmRecvData( stlContext    * aContext,
                        stlChar       * aData,
                        stlInt64        aSize,     
                        stlErrorStack * aErrorStack )
{
    stlSize sRecvSize = 0;
    stlSize sSize;
    
    while( sRecvSize < aSize )
    {
        sSize = aSize - sRecvSize;
        
        if( stlRecv( STL_SOCKET_IN_CONTEXT( *aContext ),
                     aData + sRecvSize,
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
                      ZTR_ERRCODE_COMMUNICATION_LINK_FAILURE,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztrmRecvData4IB( stlRcHandle     aHandle,
                           stlChar       * aData,
                           stlInt64        aSize,
                           stlBool         aIsPolling,
                           stlBool       * aIsTimeout,
                           stlErrorStack * aErrorStack )
{    
    stlSize   sTotalReceivedLen = 0;
    stlSize   sReceivedLen = 0;

    *aIsTimeout = STL_FALSE;

    while ( sTotalReceivedLen < aSize )
    {
        sReceivedLen = aSize - sTotalReceivedLen;
        if( stlRcRecv( aHandle,
                       aData + sTotalReceivedLen,
                       &sReceivedLen,
                       aErrorStack ) == STL_SUCCESS )
        {
            sTotalReceivedLen += sReceivedLen;
        }
        else
        {
            /** timeout 처리 => sTotalReceivedLen가 0이면 timedout 처리
             *                  sTotalReceivedLen가 0보다 크면 재시도
             */
            STL_TRY( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_TIMEDOUT );

            if ( sTotalReceivedLen == 0 )
            {
                /* timeout 처리 : *aIsTimeout 설정후 STL_SUCCESS 리턴 */
                (void)stlPopError( aErrorStack );
                *aIsTimeout = STL_TRUE;
                break;
            }
            else
            {
                /* 재시도 하여 data를 받는다 */
                (void)stlPopError( aErrorStack );
            }
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE; 
}

stlStatus ztrmDestroyHandle( void          * aHandle,
                             stlErrorStack * aErrorStack )
{
    STL_TRY( stlFinalizeContext( (stlContext*) aHandle,
                                 aErrorStack ) == STL_SUCCESS );
    
    stlFreeHeap( aHandle );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztrmDestroyHandle4IB( void          * aHandle,
                                stlErrorStack * aErrorStack )
{
    STL_TRY( stlRcClose( aHandle, 
                         aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/** @} */

