/*******************************************************************************
 * stmCommMgr.c
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

#include <stc.h>
#include <ste.h>
#include <stn.h>
#include <stl.h>
#include <stm.h>
#include <stlError.h>

stlStatus stmInitializeContext( stlContext     * aContext,
                                stlInt16         aFamily,
                                stlInt32         aType,
                                stlInt32         aProtocol,
                                stlBool          aIsListen,
                                stlChar        * aAddr,
                                stlInt32         aPort,
                                stlErrorStack  * aErrorStack )
{
    stlInt32 sState = 0;
    stlChar  sBuffer[STM_MSG_LEN];

    STL_TRY_THROW( aAddr != NULL, RAMP_ERR_ADDR_NOT_GIVEN );

    STL_TRY( stnCreateSocket( &STL_SOCKET_IN_CONTEXT(*aContext),
                              aFamily,
                              aType,
                              aProtocol,
                              aErrorStack )
             == STL_SUCCESS );
    sState = 1;

    aContext->mPollFd.mReqEvents = STL_POLLIN | STL_POLLERR;

    STL_TRY( stnSetSocketOption( STL_SOCKET_IN_CONTEXT(*aContext),
                                 STL_SOPT_SO_REUSEADDR,
                                 STL_YES,
                                 aErrorStack )
             == STL_SUCCESS );

    if( ( aFamily != STL_AF_UNIX ) && ( aType == STL_SOCK_STREAM ) )
    {
        STL_TRY( stnSetSocketOption( STL_SOCKET_IN_CONTEXT(*aContext),
                                     STL_SOPT_TCP_NODELAY,
                                     STL_YES,
                                     aErrorStack )
                 == STL_SUCCESS );
    }
    
    STL_TRY( stnSetSocketOption( STL_SOCKET_IN_CONTEXT(*aContext),
                                 STL_SOPT_SO_SNDBUF,
                                 STM_SND_BUFF_SIZE,
                                 aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stnSetSocketOption( STL_SOCKET_IN_CONTEXT(*aContext),
                                 STL_SOPT_SO_RCVBUF,
                                 STM_RCV_BUFF_SIZE,
                                 aErrorStack ) == STL_SUCCESS );

    stlMemset( &aContext->mSocketAddr, 0x00, STL_SIZEOF(stlSockAddr) );
    STL_TRY( stlSetSockAddr( &aContext->mSocketAddr,
                             aFamily,
                             aAddr,
                             aPort,
                             aAddr,
                             aErrorStack ) == STL_SUCCESS );


    switch( aType )
    {
        case STL_SOCK_STREAM :
            if(aIsListen == STL_TRUE)
            {
                if( aFamily == STL_AF_UNIX )
                {
                    if( stlRemoveFile( aAddr, aErrorStack ) != STL_SUCCESS )
                    {
                        STL_TRY( stlGetLastErrorCode(aErrorStack) == STL_ERRCODE_NO_ENTRY );
                        (void)stlPopError( aErrorStack );
                    }
                }

                STL_TRY( stlBind( STL_SOCKET_IN_CONTEXT(*aContext),
                                  &aContext->mSocketAddr,
                                  aErrorStack) == STL_SUCCESS );

                STL_TRY( stlListen( STL_SOCKET_IN_CONTEXT(*aContext),
                                    64,
                                    aErrorStack ) == STL_SUCCESS );
            }
            break;
        case STL_SOCK_DGRAM :
            if( aFamily == STL_AF_UNIX )
            {
                if( stlRemoveFile( aAddr, aErrorStack ) != STL_SUCCESS )
                {
                    STL_TRY( stlGetLastErrorCode(aErrorStack) == STL_ERRCODE_NO_ENTRY );
                    (void)stlPopError( aErrorStack );
                }
            }

            STL_TRY( stlBind( STL_SOCKET_IN_CONTEXT(*aContext),
                              &aContext->mSocketAddr,
                              aErrorStack) == STL_SUCCESS );

            break;
        default :
            STL_THROW( RAMP_ERR_NOTSUPPORT );
            break;
            
    }

    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_ADDR_NOT_GIVEN)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_ADDR_NOTGIVEN,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_NOTSUPPORT )
    {
        stlSnprintf( sBuffer, STM_MSG_LEN,
                     "not supported type(%d)", aType );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_NOTSUPPORT ,
                      sBuffer,
                      aErrorStack );
    }

    STL_FINISH;

    switch(sState)
    {
        case 1:
            stnCloseSocket( STL_SOCKET_IN_CONTEXT(*aContext), aErrorStack );
            break;
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus stmPollContext( stlContext     * aContext,
                          stlContext     * aNewContext,
                          stlInterval      aTimeout,
                          stlErrorStack  * aErrorStack )
{
    stlInt32    sState = 0;
    stlInt32    sSigFdNum;
    stlInt16    sFamily;
    stlChar     sAddrBuf[STL_MAX_FILE_NAME_LENGTH];
    stlPort     sPort;
    stlChar     sFilePathBuf[STL_MAX_FILE_PATH_LENGTH];

    sFamily = aContext->mSocketAddr.mSa.mSockCommon.sa_family;
    
    STL_TRY( stlPoll( &aContext->mPollFd,
                      1,
                      &sSigFdNum,
                      aTimeout,
                      aErrorStack )
             == STL_SUCCESS );

    STL_TRY( stlAccept( STL_SOCKET_IN_CONTEXT(*aContext),
                        &STL_SOCKET_IN_CONTEXT(*aNewContext),
                        aErrorStack )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( stlGetSockName( STL_SOCKET_IN_CONTEXT(*aNewContext),
                             &aNewContext->mSocketAddr,
                             aErrorStack )
             == STL_SUCCESS );

    aNewContext->mPollFd.mReqEvents = STL_POLLIN | STL_POLLERR;

    STL_TRY( stnGetSockAddr( &aNewContext->mSocketAddr,
                             sAddrBuf,
                             STL_MAX_FILE_NAME_LENGTH,
                             &sPort,
                             sFilePathBuf,
                             STL_MAX_FILE_PATH_LENGTH,
                             aErrorStack )
             == STL_SUCCESS );

    if( sFamily != STL_AF_UNIX )
    {
        STL_TRY( stnSetSocketOption( STL_SOCKET_IN_CONTEXT(*aNewContext),
                                     STL_SOPT_TCP_NODELAY,
                                     STL_YES,
                                     aErrorStack )
                 == STL_SUCCESS );
    }

    STL_TRY( stnSetSocketOption( STL_SOCKET_IN_CONTEXT(*aContext),
                                 STL_SOPT_SO_SNDBUF,
                                 STM_SND_BUFF_SIZE,
                                 aErrorStack ) == STL_SUCCESS );

    STL_TRY( stnSetSocketOption( STL_SOCKET_IN_CONTEXT(*aContext),
                                 STL_SOPT_SO_RCVBUF,
                                 STM_RCV_BUFF_SIZE,
                                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;
    
    switch( sState )
    {
        case 1:
            (void)stlCloseSocket( STL_SOCKET_IN_CONTEXT(*aNewContext),
                                  aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus stmConnectContext( stlContext    * aContext,
                             stlInterval     aTimeout,
                             stlErrorStack * aErrorStack )
{
    STL_TRY( stlConnect( STL_SOCKET_IN_CONTEXT(*aContext),
                         &aContext->mSocketAddr,
                         aTimeout,
                         aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stmWritePacket( stlContext     * aContext,
                          stlChar        * aPacketBuffer,
                          stlSize          aPacketLength,
                          stlErrorStack  * aErrorStack )
{
    stlPacketHeader    sPacketHeader;
    stlSize            sBytesWritten;
    stlSize            sRemainLength;
    stlSize            sBufferOffset;

    sPacketHeader.mEndian = STL_PLATFORM_ENDIAN;

    sPacketHeader.mPacketLength = stlToNetLong(aPacketLength);

    sBytesWritten = STL_SIZEOF(stlPacketHeader);
    STL_TRY( stlSend( STL_SOCKET_IN_CONTEXT(*aContext),
                      (const stlChar *)&sPacketHeader,
                      &sBytesWritten,
                      aErrorStack )
             == STL_SUCCESS );

    STL_TRY( sBytesWritten == STL_SIZEOF(stlPacketHeader) );

    sRemainLength = aPacketLength;
    sBufferOffset = 0;

    do
    {
        sBytesWritten = sRemainLength < STM_PACKET_LEN ? sRemainLength : STM_PACKET_LEN;
        STL_TRY( stlSend( STL_SOCKET_IN_CONTEXT(*aContext),
                          aPacketBuffer + sBufferOffset,
                          &sBytesWritten,
                          aErrorStack )
                 == STL_SUCCESS );
        sRemainLength -= sBytesWritten;
        sBufferOffset += sBytesWritten;
    } while( sRemainLength > 0 );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stmGetPacketLength( stlContext     * aContext,
                              stlInt8        * aEndian,
                              stlSize        * aPacketLength,
                              stlErrorStack  * aErrorStack )
{
    stlPacketHeader    sPacketHeader;
    stlSize            sRemainLength;
    stlSize            sBytesRead;
    stlSize            sBufferOffset;

    sRemainLength = STL_SIZEOF(stlPacketHeader);
    sBufferOffset = 0;

    do
    {
        sBytesRead = sRemainLength < STL_SIZEOF(stlPacketHeader) ?
            sRemainLength : STL_SIZEOF(stlPacketHeader);
        STL_TRY( stlRecv( STL_SOCKET_IN_CONTEXT(*aContext),
                          (stlChar *)&sPacketHeader + sBufferOffset,
                          &sBytesRead,
                          aErrorStack )
                 == STL_SUCCESS );
        sRemainLength -= sBytesRead;
        sBufferOffset += sBytesRead;
    } while( sRemainLength > 0 );

    *aEndian = sPacketHeader.mEndian;
    *aPacketLength = stlToHostLong(sPacketHeader.mPacketLength);

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stmReadPacket( stlContext     * aContext,
                         stlChar        * aPacketBuffer,
                         stlSize          aPacketLength,
                         stlErrorStack  * aErrorStack )
{
    stlSize            sBytesRead;
    stlSize            sRemainLength;
    stlSize            sBufferOffset;

    sRemainLength = aPacketLength;
    sBufferOffset = 0;

    do
    {
        sBytesRead = sRemainLength < STM_PACKET_LEN ? sRemainLength : STM_PACKET_LEN;
        STL_TRY( stlRecv( STL_SOCKET_IN_CONTEXT(*aContext),
                          aPacketBuffer + sBufferOffset,
                          &sBytesRead,
                          aErrorStack )
                 == STL_SUCCESS );
        sRemainLength -= sBytesRead;
        sBufferOffset += sBytesRead;
    } while( sRemainLength > 0 );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
