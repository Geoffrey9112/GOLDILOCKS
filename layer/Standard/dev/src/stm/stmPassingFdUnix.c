/*******************************************************************************
 * stmPassingFdUnix.c
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

#ifdef STC_HAVE_STRUCT_CMSGHDR

/**
 * OSs that support BSD 4.3 way only
 * HP-UX      OK      BSD4.3 way
 * IRIX       OK      BSD4.3 way
 * Solaris    OK      BSD4.3 way
 */

typedef union stmCMsgFd
{
    stlCMsgHdr   mCMsg;
    stlChar      mData[CMSG_SPACE(STL_SIZEOF(stlSocket))];
} stmCMsgFd;
#endif

stlStatus stmRecvContext( stlContext     * aContext,
                          stlContext     * aNewContext,
                          stlChar        * aBuf,
                          stlSize        * aLen,
                          stlErrorStack  * aErrorStack )
{
    stlMsgHdr    sMsg;
    stlIoVec     sIov[1];
    stlChar      sBuf[1];

    if( aBuf == NULL )
    {
        sIov[0].iov_base = sBuf;
        sIov[0].iov_len = STL_SIZEOF(sBuf);
    }
    else
    {
        sIov[0].iov_base = aBuf;
        sIov[0].iov_len = *aLen;
    }

    stlMemset( &sMsg, 0, STL_SIZEOF( stlMsgHdr ) );
    
    sMsg.msg_name = NULL;
    sMsg.msg_namelen = 0;
    sMsg.msg_iov = sIov;
    sMsg.msg_iovlen = 1;

#ifdef STC_HAVE_STRUCT_CMSGHDR
    {
        stlCMsgHdr  *sCMsgHdr;
        stmCMsgFd    sCMsg;
        
        sMsg.msg_control = &sCMsg;
        sMsg.msg_controllen = STL_SIZEOF(sCMsg);

        sCMsgHdr = CMSG_FIRSTHDR(&sMsg);
        sCMsgHdr->cmsg_len = CMSG_LEN(STL_SIZEOF(stlSocket));
        sCMsgHdr->cmsg_level = SOL_SOCKET;
        sCMsgHdr->cmsg_type = SCM_RIGHTS;

        STL_TRY( stlRecvMsg(STL_SOCKET_IN_CONTEXT(*aContext),
                            &sMsg,
                            0,
                            aErrorStack) == STL_SUCCESS);

        sCMsgHdr = CMSG_FIRSTHDR(&sMsg);
        STL_TRY_THROW( sCMsgHdr != NULL, RAMP_ERR );

        stlMemcpy( &STL_SOCKET_IN_CONTEXT(*aNewContext),
                   CMSG_DATA(sCMsgHdr),
                   STL_SIZEOF(stlSocket) );

        STL_TRY_THROW( (STL_SOCKET_IN_CONTEXT(*aNewContext) != -1) &&
                       (sCMsgHdr->cmsg_len == CMSG_LEN(STL_SIZEOF(stlSocket))) &&
                       (sCMsgHdr->cmsg_level == SOL_SOCKET) &&
                       (sCMsgHdr->cmsg_type == SCM_RIGHTS),
                       RAMP_ERR );
    }
#else
    /**
     * Old BSD 4.3 way.
     */
    sMsg.msg_accrights = (void*)&STL_SOCKET_IN_CONTEXT(*aNewContext);
    sMsg.msg_accrightslen = STL_SIZEOF(stlSocket);
    
    STL_TRY( stlRecvMsg(STL_SOCKET_IN_CONTEXT(*aContext),
                        &sMsg,
                        0,
                        aErrorStack) == STL_SUCCESS);
#endif

    STL_TRY( stlGetPeerName( STL_SOCKET_IN_CONTEXT(*aNewContext),
                             &aNewContext->mSocketAddr,
                             aErrorStack )
             == STL_SUCCESS );
    
    aNewContext->mPollFd.mReqEvents = STL_POLLIN | STL_POLLERR;

    return STL_SUCCESS;

#ifdef STC_HAVE_STRUCT_CMSGHDR
    STL_CATCH( RAMP_ERR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_RECV_CONTEXT,
                      NULL,
                      aErrorStack );
    }
#endif
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stmSendContext( stlContext     * aContext,
                          stlSockAddr    * aAddr,
                          stlContext     * aPassingContext,
                          const stlChar  * aBuf,
                          stlSize          aLen,
                          stlErrorStack  * aErrorStack )
{
    stlMsgHdr    sMsg;
    stlIoVec     sIov[1];
    stlChar      sBuf[1];
    stlChar      sBuffer[64];
    stlInt16     sAf;

    if( aBuf == NULL )
    {
        sBuf[0] = 'Z';
        sIov[0].iov_base = sBuf;
        sIov[0].iov_len = STL_SIZEOF(sBuf);
    }
    else
    {
        sIov[0].iov_base = (void *)aBuf;
        sIov[0].iov_len = aLen;
    }
    
    stlMemset( &sMsg, 0, STL_SIZEOF( stlMsgHdr ) );

    sMsg.msg_iov = sIov;
    sMsg.msg_iovlen = 1;

    if( aAddr == NULL )
    {
        sMsg.msg_name = NULL;
        sMsg.msg_namelen = 0;
    }
    else
    {
        sAf = ((struct sockaddr*)aAddr)->sa_family;
        
        switch( sAf )
        {
            case STL_AF_INET :
                sMsg.msg_name  = (void*)&aAddr->mSa.mIn4;
                sMsg.msg_namelen = STL_SIZEOF(struct sockaddr_in);
                break;
            case STL_AF_INET6 :
                sMsg.msg_name = (void*)&aAddr->mSa.mIn6;
                sMsg.msg_namelen = STL_SIZEOF(struct sockaddr_in6);
                break;
            case STL_AF_UNIX :
                sMsg.msg_name = (void*)&aAddr->mSa.mUn;
                sMsg.msg_namelen = STL_SIZEOF(struct sockaddr_un);
                break;
            default :
                STL_THROW( RAMP_ERR_NOTSUPPORT );
                break;
        }
    }

#ifdef STC_HAVE_STRUCT_CMSGHDR
    {
        stlCMsgHdr  *sCMsgHdr;
        stmCMsgFd    sCMsg;
        
        stlMemset( &sCMsg, 0x00, STL_SIZEOF( stmCMsgFd ) );
        
        sMsg.msg_control = &sCMsg;
        sMsg.msg_controllen = STL_SIZEOF(sCMsg);

        sCMsgHdr = CMSG_FIRSTHDR(&sMsg);
        STL_TRY_THROW( sCMsgHdr != NULL, RAMP_ERR );

        sCMsgHdr->cmsg_len = CMSG_LEN(STL_SIZEOF(stlSocket));
        sCMsgHdr->cmsg_level = SOL_SOCKET;
        sCMsgHdr->cmsg_type = SCM_RIGHTS;

        stlMemcpy( CMSG_DATA(sCMsgHdr),
                   &STL_SOCKET_IN_CONTEXT(*aPassingContext),
                   STL_SIZEOF(stlSocket) );
    }
#else
    /* Old BSD 4.3 way. */
    sMsg.msg_accrights = (void*)&STL_SOCKET_IN_CONTEXT(*aPassingContext);
    sMsg.msg_accrightslen = STL_SIZEOF( stlSocket );
#endif
    
    STL_TRY( stlSendMsg( STL_SOCKET_IN_CONTEXT(*aContext),
                         &sMsg,
                         0,
                         aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

#ifdef STC_HAVE_STRUCT_CMSGHDR
    STL_CATCH( RAMP_ERR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SEND_CONTEXT,
                      NULL,
                      aErrorStack );
    }
#endif
    
    STL_CATCH( RAMP_ERR_NOTSUPPORT )
    {
        stlSnprintf( sBuffer, 64,
                     "not supported address format(%d)", sAf );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_NOTSUPPORT ,
                      sBuffer,
                      aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stmCreateContextPair( stlInt32          aDomain,
                                stlInt32          aType,
                                stlInt32          aProtocol,
                                stlContext        aContexts[2],
                                stlErrorStack    *aErrorStack )
{
    stlSocket   sSockets[2];

    STL_TRY( stlSocketPair( aDomain, aType, aProtocol, sSockets, aErrorStack ) == STL_SUCCESS );
    STL_SOCKET_IN_CONTEXT(aContexts[0]) = sSockets[0];
    STL_TRY( stnGetPeerName( sSockets[0],
                             &aContexts[0].mSocketAddr,
                             aErrorStack )
             == STL_SUCCESS );

    aContexts[0].mPollFd.mReqEvents = STL_POLLIN | STL_POLLERR;

    STL_SOCKET_IN_CONTEXT(aContexts[1]) = sSockets[1];
    STL_TRY( stnGetPeerName( sSockets[1],
                             &aContexts[1].mSocketAddr,
                             aErrorStack )
             == STL_SUCCESS );

    aContexts[1].mPollFd.mReqEvents = STL_POLLIN | STL_POLLERR;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

