/*******************************************************************************
 * ztnmCommunicate.c
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
 * @file ztnmCommunicate.c
 * @brief GlieseTool Cymon Communicate Routines
 */

#include <goldilocks.h>
#include <ztn.h>

stlStatus ztnmSendProtocolHdrNData( stlContext     * aContext,
                                    ztnProtocolHdr   aHeader,
                                    stlChar        * aData,
                                    stlErrorStack  * aErrorStack )
{
    ztnProtocolHdr sHeader;

    STL_WRITE_INT32( &sHeader.mCategory, &aHeader.mCategory );
    STL_WRITE_INT32( &sHeader.mType, &aHeader.mType );
    STL_WRITE_INT64( &sHeader.mLength, &aHeader.mLength );
    
    STL_TRY( ztnmSendData( aContext,
                           (stlChar*)&sHeader,
                           STL_SIZEOF( ztnProtocolHdr ),
                           aErrorStack ) == STL_SUCCESS );
    
    if( aHeader.mLength > 0 )
    {
        STL_DASSERT( aData != NULL );
        
        STL_TRY( ztnmSendData( aContext,
                               aData,
                               aHeader.mLength,
                               aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
       
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztnmReceiveProtocolHdrWithPoll( stlContext     * aContext,
                                          ztnProtocolHdr * aHeader,
                                          stlErrorStack  * aErrorStack )
{
    ztnProtocolHdr  sHeader;
    stlInt32        sSigFdNum;
    
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
        }
    }
    
    STL_TRY( ztnmReceiveData( aContext, 
                              (stlChar*)&sHeader,
                              STL_SIZEOF( ztnProtocolHdr ),
                              aErrorStack ) == STL_SUCCESS );
    
    STL_WRITE_INT32( &aHeader->mCategory, &sHeader.mCategory );
    STL_WRITE_INT32( &aHeader->mType, &sHeader.mType );
    STL_WRITE_INT64( &aHeader->mLength, &sHeader.mLength );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
        
}


stlStatus ztnmReceiveProtocolHdr( stlContext     * aContext,
                                  ztnProtocolHdr * aHeader,
                                  stlErrorStack  * aErrorStack )
{
    ztnProtocolHdr  sHeader;
    
    STL_TRY( ztnmReceiveData( aContext, 
                              (stlChar*)&sHeader,
                              STL_SIZEOF( ztnProtocolHdr ),
                              aErrorStack ) == STL_SUCCESS );
    
    STL_WRITE_INT32( &aHeader->mCategory, &sHeader.mCategory );
    STL_WRITE_INT32( &aHeader->mType, &sHeader.mType );
    STL_WRITE_INT64( &aHeader->mLength, &sHeader.mLength );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztnmSendData( stlContext    * aContext,
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
                      ZTN_ERRCODE_COMMUNICATION_LINK_FAILURE,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztnmReceiveData( stlContext    * aContext,
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
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_COMMUNICATION_LINK_FAILURE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTN_ERRCODE_COMMUNICATION_LINK_FAILURE,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztnmConnectToCyclone( ztnGroupItem  * aGroup,
                                stlErrorStack * aErrorStack )
{
    stlInt32 sState = 0;
    
    STL_TRY( stlInitializeContext( &(aGroup->mContext),
                                   STL_AF_INET,
                                   STL_SOCK_STREAM,
                                   0,
                                   STL_FALSE,
                                   "127.0.0.1",     //localhost
                                   aGroup->mPort,
                                   aErrorStack ) == STL_SUCCESS );
    
    aGroup->mContext.mPollFd.mReqEvents = STL_POLLIN | STL_POLLERR;
    
    sState = 1;
    
    STL_TRY( stlConnectContext( &(aGroup->mContext),
                                STL_SET_SEC_TIME( 1 ),      //Polling Time.
                                aErrorStack ) == STL_SUCCESS );
    
    aGroup->mIsConnect = STL_TRUE;
    
    return STL_SUCCESS;   
    
    STL_FINISH;
    
    switch( sState )
    {
        case 1:
            (void) stlFinalizeContext( &(aGroup->mContext),
                                       aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}


/** @} */
