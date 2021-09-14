/*******************************************************************************
 * stnSocketsUnix.c
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
#include <stnUnix.h>
#include <stlPoll.h>

#ifdef BEOS_R5
#undef close
#define close closesocket
#endif /* BEOS_R5 */


stlStatus stnCreateSocket( stlSocket     * aSock,
                           stlInt16        aFamily,
                           stlInt32        aType,
                           stlInt32        aProtocol,
                           stlErrorStack * aErrorStack )
{
    stlChar  sMsg[64];
    stlInt32 sFamily = aFamily;
    stlInt32 sFlags = 0;

    STL_PARAM_VALIDATE( aSock != NULL, aErrorStack );

#ifdef STC_HAVE_SOCK_CLOEXEC
    sFlags |= SOCK_CLOEXEC;
#endif

    if (sFamily == STL_AF_UNSPEC) {
#if STC_HAVE_IPV6
        sFamily = STL_AF_INET6;
#else
        sFamily = STL_AF_INET;
#endif
    }


#ifndef BEOS_R5
    *aSock = socket(sFamily, aType|sFlags, aProtocol);
#else
    /* For some reason BeOS R5 has an unconventional protocol numbering,
     * so we need to translate here. */
    switch( aProtocol )
    {
        case 0:
            {
                *aSock = socket(sFamily, aType|sFlags, 0);
                break;
            }
        case STL_PROTO_TCP:
            {
                *aSock = socket(sFamily, aType|sFlags, IPPROTO_TCP);
                break;
            }
        case STL_PROTO_UDP:
            {
                *aSock = socket(sFamily, aType|sFlags, IPPROTO_UDP);
                break;
            }
        case STL_PROTO_SCTP:
        default:
            {
                STL_THROW( RAMP_ERR_PROTO );
            }
    }
#endif /* BEOS_R5 */

#if STC_HAVE_IPV6
    if( (*aSock < 0) && (aFamily == STL_AF_UNSPEC) )
    {
        sFamily = STL_AF_INET;
        *aSock = socket(sFamily, aType|sFlags, aProtocol);
    }
#endif

    STL_TRY_THROW( *aSock >= 0, RAMP_ERR_SOCKET );

#ifndef STC_HAVE_SOCK_CLOEXEC
    {
        stlInt32 sRet;
        
        do
        {
            sFlags = fcntl(*aSock, F_GETFD);
        } while( (sFlags == -1) && (errno == EINTR) );
        
        STL_TRY_THROW( sFlags != -1, RAMP_ERR_GETFD );

        sFlags |= FD_CLOEXEC;

        do
        {
            sRet = fcntl(*aSock, F_SETFD, sFlags);
        } while( (sRet == -1) && (errno == EINTR) );
        STL_TRY_THROW( sRet != -1, RAMP_ERR_SETFD );
    }
#endif

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SOCKET )
    {
        stlSnprintf(sMsg, 64, "socket() returns errno(%d)", errno);
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SOCKET,
                      sMsg,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
#ifndef STC_HAVE_SOCK_CLOEXEC
    STL_CATCH( RAMP_ERR_GETFD )
    {
        stlSnprintf(sMsg, 64, "fcntl(GETFD) returns errno(%d)", errno);
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_GETFD,
                      sMsg,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    STL_CATCH( RAMP_ERR_SETFD )
    {
        stlSnprintf(sMsg, 64, "fcntl(SETFD) returns errno(%d)", errno);
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SETFD,
                      sMsg,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
#endif
#ifdef BEOS_R5
    STL_CATCH( RAMP_ERR_PROTO )
    {
        aSock->mSocketDes = -1;
        stlSnprintf(sMsg, 64, "not supported protocol(%d)", aProtocol);
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_NOTSUPPORT,
                      sMsg,
                      aErrorStack );
    }
#endif

    STL_FINISH;

    return STL_FAILURE;
} 

stlStatus stnShutdownSocket( stlSocket        aSocket, 
                             stlShutdownHow   aHow,
                             stlErrorStack  * aErrorStack )
{
    stlChar sMsg[64];

    STL_TRY_THROW( shutdown(aSocket, aHow) != -1, RAMP_ERR_SHUTDOWN );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SHUTDOWN )
    {
        stlSnprintf(sMsg, 64, "shutdown() returns errno(%d)", errno);
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SOCKET_SHUTDOWN,
                      sMsg,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stnCloseSocket( stlSocket       aSock,
                          stlErrorStack * aErrorStack )
{
    /* Set socket descriptor to -1 before close(), so that there is no
     * chance of returning an already closed FD from apr_os_sock_get().
     */
    stlInt32 sRet;
    stlChar  sMsg[64];

    do
    {
        sRet = close(aSock);
    } while( (sRet == -1) && (errno == EINTR) );
    
    STL_TRY_THROW( sRet == 0, RAMP_ERR_CLOSE );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CLOSE )
    {
        stlSnprintf(sMsg, 64, "close() returns errno(%d)", errno);
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SOCKET_CLOSE,
                      sMsg,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stnBind( stlSocket       aSock,
                   stlSockAddr   * aSockAddr,
                   stlErrorStack * aErrorStack )
{
    stlSockLen sSaLen;
    stlChar    sMsg[64];
    stlInt16   sAf;

    STL_PARAM_VALIDATE( aSockAddr != NULL, aErrorStack );

    sAf = ((struct sockaddr*)aSockAddr)->sa_family;

    if( sAf == STL_AF_INET )
    {
        sSaLen = STL_SIZEOF(aSockAddr->mSa.mIn4);
        STL_THROW( RAMP_CONTINUE );
    }
#if STC_HAVE_IPV6
    if( sAf == STL_AF_INET6 )
    {
        sSaLen = STL_SIZEOF(aSockAddr->mSa.mIn6);
        STL_THROW( RAMP_CONTINUE );
    }
#endif
#if STC_HAVE_UNIXDOMAIN
    if( sAf == STL_AF_UNIX )
    {
        sSaLen = STL_SIZEOF(aSockAddr->mSa.mUn);
        STL_THROW( RAMP_CONTINUE );
    }
#endif
    STL_THROW( RAMP_ERR_NOTSUPPORT);

    STL_RAMP( RAMP_CONTINUE );

    if( bind( aSock, (struct sockaddr *)&aSockAddr->mSa, sSaLen ) == -1 )
    {
        STL_TRY_THROW( errno != EBADF, RAMP_ERR_BADF );
        STL_TRY_THROW( errno != EADDRINUSE, RAMP_ERR_ADDRINUSE );
        STL_TRY_THROW( errno != EINVAL, RAMP_ERR_INVAL );
        STL_THROW( RAMP_ERR_BIND );
    }

    return STL_SUCCESS;
   
    STL_CATCH( RAMP_ERR_BADF )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_FILE_BAD,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    STL_CATCH( RAMP_ERR_ADDRINUSE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_ADDR_INUSE,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    STL_CATCH( RAMP_ERR_INVAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INVALID_ARGUMENT,
                      "socket is already bound to an address",
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    STL_CATCH( RAMP_ERR_BIND )
    {
        stlSnprintf(sMsg, 64, "bind() returns errno(%d)", errno);
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_BIND,
                      sMsg,
                      aErrorStack,
                      errno );
        steSetSystemError( errno, aErrorStack );
    }
    STL_CATCH( RAMP_ERR_NOTSUPPORT )
    {
        stlSnprintf(sMsg, 64, "not supported address format(%d)", sAf);
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_NOTSUPPORT ,
                      sMsg,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stnListen( stlSocket       aSock,
                     stlUInt32       aBacklog,
                     stlErrorStack * aErrorStack )
{
    stlChar sMsg[64];

    if( listen(aSock, aBacklog) == -1 )
    {
        STL_TRY_THROW( errno != EBADF, RAMP_ERR_BADF );
        STL_TRY_THROW( errno != EADDRINUSE, RAMP_ERR_ADDRINUSE );
        STL_THROW( RAMP_ERR_LISTEN );
    }
    
    return STL_SUCCESS; 

    STL_CATCH( RAMP_ERR_BADF )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_FILE_BAD,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    STL_CATCH( RAMP_ERR_ADDRINUSE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_ADDR_INUSE,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    STL_CATCH( RAMP_ERR_LISTEN )
    {
        stlSnprintf(sMsg, 64, "listen() returns errno(%d)", errno);
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_LISTEN,
                      sMsg,
                      aErrorStack,
                      errno );
        steSetSystemError( errno, aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stnAccept( stlSocket       aSock,
                     stlSocket     * aNewSock,
                     stlErrorStack * aErrorStack )
{
    stlSocket   sSockFD;
    stlSockAddr sSockAddr;
    stlChar     sMsg[64];
    stlSockLen  sSaLen = STL_SIZEOF(sSockAddr.mSa);
#ifndef STC_HAVE_ACCEPT4
    stlInt32    sFlags;
#endif

    STL_PARAM_VALIDATE( aNewSock != NULL, aErrorStack );

    do
    {
#ifdef STC_HAVE_ACCEPT4
        sSockFD = accept4(aSock, (struct sockaddr *)&sSockAddr.mSa, &sSaLen, SOCK_CLOEXEC);
#else
        sSockFD = accept(aSock, (struct sockaddr *)&sSockAddr.mSa, &sSaLen);
#endif
    } while( (sSockFD == -1) && (errno == EINTR) );

    if( sSockFD < 0 )
    {
        STL_TRY_THROW( (errno != EAGAIN) && (errno != EWOULDBLOCK), RAMP_ERR_AGAIN );
        STL_TRY_THROW( errno != EBADF, RAMP_ERR_BADF );
        STL_TRY_THROW( errno != EINVAL, RAMP_ERR_INVAL );
        STL_THROW( RAMP_ERR_ACCEPT );
    }

#ifdef TPF
    /* 0 is an invalid socket for TPF */
    STL_TRY_THROW( sSockFD != 0, RAMP_ERR_EINTR );
#endif

    *aNewSock = sSockFD;

#ifndef STC_HAVE_ACCEPT4
    {
        sFlags = fcntl(*aNewSock, F_GETFD);
        STL_TRY_THROW( sFlags != -1, RAMP_ERR_GETFD );

        sFlags |= FD_CLOEXEC;
        STL_TRY_THROW( fcntl(*aNewSock, F_SETFD, sFlags) != -1, RAMP_ERR_SETFD );
    }
#endif

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_AGAIN )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_AGAIN,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    STL_CATCH( RAMP_ERR_BADF )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_FILE_BAD,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    STL_CATCH( RAMP_ERR_INVAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INVALID_ARGUMENT,
                      "socket is not listening for connection",
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    STL_CATCH( RAMP_ERR_ACCEPT )
    {
        stlSnprintf(sMsg, 64, "accept() returns errno(%d)", errno);
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_ACCEPT,
                      sMsg,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
#ifdef TPF
    STL_CATCH( RAMP_ERR_EINTR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INTR,
                      "accept() was interrupted by signal",
                      aErrorStack );
    }
#endif
#ifndef STC_HAVE_ACCEPT4
    STL_CATCH( RAMP_ERR_GETFD )
    {
        stlSnprintf(sMsg, 64, "[stnAccept] fcntl(GETFD) returns errno(%d)", errno);
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_GETFD,
                      sMsg,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    STL_CATCH( RAMP_ERR_SETFD )
    {
        stlSnprintf(sMsg, 64, "fcntl(SETFD) returns errno(%d)", errno);
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SETFD,
                      sMsg,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
#endif

    STL_FINISH;

    return STL_FAILURE;

}

stlStatus stnConnect( stlSocket       aSock,
                      stlSockAddr   * aSockAddr,
                      stlInterval     aTimeout,
                      stlErrorStack * aErrorStack )
{
    stlInt32   sRetVal;
    stlChar    sMsg[64];
    stlSockLen sSaLen;
    stlInt16   sAf;

    stlInt32   sFlagOrg = 0;
    stlInt32   sFlagNew = 0;
    stlBool    sSetFlag = STL_FALSE;

    stlPollFd  sPollFd;
    stlInt32   sSigFdNum = 0;

    stlInt32   sError = 0;
    stlSockLen sLen   = STL_SIZEOF(sError);
    
    
    STL_PARAM_VALIDATE( aSockAddr != NULL, aErrorStack );
    sAf = ((struct sockaddr*)aSockAddr)->sa_family;

    if( sAf == STL_AF_INET )
    {
        sSaLen = STL_SIZEOF(aSockAddr->mSa.mIn4);
        STL_THROW( RAMP_CONTINUE );
    }
#if STC_HAVE_IPV6
    if( sAf == STL_AF_INET6 )
    {
        sSaLen = STL_SIZEOF(aSockAddr->mSa.mIn6);
        STL_THROW( RAMP_CONTINUE );
    }
#endif
#if STC_HAVE_UNIXDOMAIN
    if( sAf == STL_AF_UNIX )
    {
        sSaLen = STL_SIZEOF(aSockAddr->mSa.mUn);
        STL_THROW( RAMP_CONTINUE );
    }
#endif
    STL_THROW( RAMP_ERR_NOTSUPPORT);

    STL_RAMP( RAMP_CONTINUE );

    sFlagNew = fcntl( aSock, F_GETFL, NULL );
    STL_TRY_THROW( sFlagNew != -1, RAMP_ERR_GETFL );

    sFlagOrg = sFlagNew;
    sFlagNew |= O_NONBLOCK;

    /*
     * nun blocking 상태로 만든다.
     */

    do
    {
        sRetVal = fcntl(aSock, F_SETFL, sFlagNew);
    } while( (sRetVal == -1) && (errno == EINTR) );
    
    STL_TRY_THROW( sRetVal != -1, RAMP_ERR_SETFL );

    sSetFlag = STL_TRUE;
    

    /*
     * 연결을 기다린다.
     * non blocking 상태이므로 바로 리턴된다.
     */
    do
    {
        sRetVal = connect( aSock,
                           (const struct sockaddr *)&aSockAddr->mSa,
                           sSaLen );
    } while( (sRetVal == -1) && ((errno == EINTR) || (errno == EAGAIN)) );

    STL_TRY_THROW( sRetVal != 0, RAMP_SUCCESS );

    STL_TRY_THROW( (errno == EINPROGRESS) || (errno == EALREADY),
                   RAMP_ERR_CONNECT );

    /*
     * 주어진 timeout 만큼 기다린다.
     */
    sPollFd.mSocketHandle = aSock;
    sPollFd.mReqEvents    = STL_POLLOUT | STL_POLLERR;

    errno = 0;
    STL_TRY( stlPoll( &sPollFd,
                      1,
                      &sSigFdNum,
                      aTimeout,
                      aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( (sPollFd.mRetEvents & STL_POLLERR) != STL_POLLERR,
                   RAMP_ERR_CONNECT_POLLERR );

    STL_DASSERT( (sPollFd.mRetEvents & STL_POLLOUT) == STL_POLLOUT );

    STL_RAMP( RAMP_SUCCESS );

    /*
     * 연결이 성공했을 경우 소켓을 원래 상태로 되돌린다.
     */

    sSetFlag = STL_FALSE;

    do
    {
        sRetVal = fcntl(aSock, F_SETFL, sFlagOrg);
    } while( (sRetVal == -1) && (errno == EINTR) );
    
    STL_TRY_THROW( sRetVal != -1, RAMP_ERR_SETFL );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_GETFL )
    {
        stlSnprintf(sMsg, 64, "fcntl(GETFL) returns errno(%d)", errno);
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_GETFL,
                      sMsg,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    STL_CATCH( RAMP_ERR_SETFL )
    {
        stlSnprintf(sMsg, 64, "fcntl(SETFL) returns errno(%d)", errno);
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SETFL,
                      sMsg,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    STL_CATCH( RAMP_ERR_CONNECT )
    {
        stlSnprintf(sMsg, 64, "connect() returns errno(%d)", errno);
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_CONNECT,
                      sMsg,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    STL_CATCH( RAMP_ERR_CONNECT_POLLERR)
    {
        getsockopt(aSock, SOL_SOCKET, SO_ERROR, &sError, &sLen);
        
        stlSnprintf(sMsg, 64, "connect() returns errno(%d)", sError);
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_CONNECT,
                      sMsg,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_NOTSUPPORT )
    {
        stlSnprintf(sMsg, 64, "not supported address format(%d)", sAf);
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_NOTSUPPORT ,
                      sMsg,
                      aErrorStack );
    }

    STL_FINISH;

    if( sSetFlag == STL_TRUE )
    {
        do
        {
            sRetVal = fcntl(aSock, F_SETFL, sFlagOrg);
        } while( (sRetVal == -1) && (errno == EINTR) );
    }

    return STL_FAILURE;
}

stlStatus stnSocketPair( stlInt32          aDomain,
                         stlInt32          aType,
                         stlInt32          aProtocol,
                         stlSocket         aSockets[2],
                         stlErrorStack    *aErrorStack )
{
    stlChar  sMsg[64];

    STL_TRY_THROW( socketpair( aDomain, aType, aProtocol, aSockets ) == 0,
                   RAMP_ERR_SOCKET_PAIR );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SOCKET_PAIR )
    {
        stlSnprintf(sMsg, 64, "socketpair() returns errno(%d)", errno);
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SOCKET,
                      sMsg,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    STL_FINISH;

    return STL_FAILURE;
}
