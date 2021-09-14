/*******************************************************************************
 * ztcmCommunicate.c
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
 * @file ztcmCommunicate.c
 * @brief GlieseTool Cyclone Communicate Routines
 */

#include <goldilocks.h>
#include <ztc.h>

extern stlBool gRunState;

stlStatus ztcmSendProtocolHdrNData( stlContext     * aContext,
                                    ztcProtocolHdr   aHeader,
                                    stlChar        * aData,
                                    stlErrorStack  * aErrorStack )
{
    ztcProtocolHdr sHeader;

    STL_WRITE_INT32( &sHeader.mCategory, &aHeader.mCategory );
    STL_WRITE_INT32( &sHeader.mType, &aHeader.mType );
    STL_WRITE_INT64( &sHeader.mLength, &aHeader.mLength );
    
    STL_TRY( ztcmSendData( aContext,
                           (stlChar*)&sHeader,
                           STL_SIZEOF( ztcProtocolHdr ),
                           aErrorStack ) == STL_SUCCESS );
    
    if( aHeader.mLength > 0 )
    {
        STL_DASSERT( aData != NULL );
        
        STL_TRY( ztcmSendData( aContext,
                               aData,
                               aHeader.mLength,
                               aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
       
    STL_FINISH;
    
    return STL_FAILURE;
}



stlStatus ztcmReceiveProtocolHdrWithPoll( stlContext     * aContext,
                                          ztcProtocolHdr * aHeader,
                                          stlBool          aTimeoutCheck,
                                          stlErrorStack  * aErrorStack )
{
    ztcProtocolHdr  sHeader;
    stlInt32        sSigFdNum;
    stlInt32        sTryTimeout = ZTC_COMM_HEARTBEAT_TIMEOUT;
    stlInt32        sTryCount   = 0;
    
    while( 1 )
    {
        if( stlPoll( &aContext->mPollFd,
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
            
            /**
             * Timeout을 체크할 경우에만 ...
             */
            if( aTimeoutCheck == STL_TRUE )
            {
                sTryCount++;
            
                if( sTryCount > sTryTimeout )
                {
/**
 * DEBUG Mode일 때는 Heartbeat을 체크하지 않도록 한다.
 */
#ifndef STL_DEBUG
                    STL_TRY( ztcmLogMsg( aErrorStack,
                                         "[RECEIVER] Master not responding timeout.(Heartbeat)\n" ) == STL_SUCCESS );
                    STL_TRY( STL_FALSE );
#endif
                }
            }

        }
    }
    
    STL_TRY( ztcmReceiveData( aContext, 
                              (stlChar*)&sHeader,
                              STL_SIZEOF( ztcProtocolHdr ),
                              aErrorStack ) == STL_SUCCESS );
    
    STL_WRITE_INT32( &aHeader->mCategory, &sHeader.mCategory );
    STL_WRITE_INT32( &aHeader->mType, &sHeader.mType );
    STL_WRITE_INT64( &aHeader->mLength, &sHeader.mLength );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
        
}


stlStatus ztcmReceiveProtocolHdr( stlContext     * aContext,
                                  ztcProtocolHdr * aHeader,
                                  stlErrorStack  * aErrorStack )
{
    ztcProtocolHdr  sHeader;
    
    STL_TRY( ztcmReceiveData( aContext, 
                              (stlChar*)&sHeader,
                              STL_SIZEOF( ztcProtocolHdr ),
                              aErrorStack ) == STL_SUCCESS );
    
    STL_WRITE_INT32( &aHeader->mCategory, &sHeader.mCategory );
    STL_WRITE_INT32( &aHeader->mType, &sHeader.mType );
    STL_WRITE_INT64( &aHeader->mLength, &sHeader.mLength );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcmSendData( stlContext    * aContext,
                        stlChar       * aData,
                        stlInt32        aSize,
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
                      ZTC_ERRCODE_COMMUNICATION_LINK_FAILURE,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztcmReceiveData( stlContext    * aContext,
                           stlChar       * aData,
                           stlInt32        aSize,     
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
    
    /**
     * DATA를 받더라도 종료되었을 경우 처리하지 않는다.
     */
    STL_TRY( gRunState == STL_TRUE );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_COMMUNICATION_LINK_FAILURE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_COMMUNICATION_LINK_FAILURE,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztcmGetPeerIPAddr( stlContext    * aContext,
                             stlChar       * aIPAddr,
                             stlSize         aSize,
                             stlErrorStack * aErrorStack )
{
    stlSockAddr  sPeerAddr;
    
    STL_TRY( stlGetPeerName( STL_SOCKET_IN_CONTEXT( *aContext ),
                             &sPeerAddr,
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlGetSockAddrIp( &sPeerAddr,
                               aIPAddr,
                               aSize,
                               aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/** @} */
