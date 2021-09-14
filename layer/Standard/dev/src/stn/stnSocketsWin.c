/*******************************************************************************
 * stnSocketsWin.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: stnSocketsUnix.c 13562 2014-09-12 08:31:43Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stc.h>
#include <stlDef.h>
#include <ste.h>
#include <stlStrings.h>
#include <stlError.h>

stlStatus stnCreateSocket( stlSocket     * aSock,
                           stlInt16        aFamily,
                           stlInt32        aType,
                           stlInt32        aProtocol,
                           stlErrorStack * aErrorStack )
{
    stlChar sMsg[64];
    HANDLE  sProcess;
    HANDLE  sDup;

    STL_PARAM_VALIDATE( aSock != NULL, aErrorStack );
	
    if (aFamily == STL_AF_UNSPEC)
    {
#if STC_HAVE_IPV6
        aFamily = STL_AF_INET6;
#else
        aFamily = STL_AF_INET;
#endif
    }

    *aSock = socket(aFamily, aType, aProtocol);

#if STC_HAVE_IPV6
    if( (*aSock == INVALID_SOCKET) && (aFamily == STL_AF_UNSPEC) )
    {
        aFamily = STL_AF_INET;
        *aSock = socket(aFamily, aType, aProtocol);
    }
#endif

    STL_TRY_THROW( *aSock != INVALID_SOCKET, RAMP_ERR_SOCKET );

    sProcess = GetCurrentProcess();

    if (DuplicateHandle(sProcess, (HANDLE)*aSock, sProcess, &sDup, 0, FALSE, DUPLICATE_SAME_ACCESS))
    {
        closesocket(*aSock);
        *aSock = (SOCKET)sDup;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SOCKET )
    {
        stlSnprintf(sMsg, 64, "socket() returns errno(%d)", WSAGetLastError());
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SOCKET,
                      sMsg,
                      aErrorStack );
        steSetSystemError(WSAGetLastError(), aErrorStack);
    }

    STL_FINISH;

    return STL_FAILURE;
} 

stlStatus stnShutdownSocket( stlSocket        aSocket, 
                             stlShutdownHow   aHow,
                             stlErrorStack  * aErrorStack )
{
    stlChar sMsg[64];
	
#ifdef SD_RECEIVE
    switch (aHow)
    {
	case STL_SHUTDOWN_READ:
            aHow = SD_RECEIVE;
            break;
	case STL_SHUTDOWN_WRITE:
            aHow = SD_SEND;
            break;
	case STL_SHUTDOWN_READWRITE:
            aHow = SD_BOTH;
            break;
	default:
            STL_ASSERT(STL_FALSE);
            break;
    }
#endif

    STL_TRY_THROW( shutdown(aSocket, aHow) == 0, RAMP_ERR_SHUTDOWN );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SHUTDOWN )
    {
        stlSnprintf(sMsg, 64, "shutdown() returns errno(%d)", WSAGetLastError());
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SOCKET_SHUTDOWN,
                      sMsg,
                      aErrorStack );
        steSetSystemError(WSAGetLastError(), aErrorStack);
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stnCloseSocket( stlSocket       aSock,
                          stlErrorStack * aErrorStack )
{
    stlChar sMsg[64];

    if (aSock != INVALID_SOCKET)
    {
        STL_TRY_THROW(closesocket(aSock) != SOCKET_ERROR, RAMP_ERR_CLOSE);
        aSock = INVALID_SOCKET;
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CLOSE )
    {
        stlSnprintf(sMsg, 64, "closesocket() returns errno(%d)", WSAGetLastError());
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SOCKET_CLOSE,
                      sMsg,
                      aErrorStack );
        steSetSystemError(WSAGetLastError(), aErrorStack);
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
    stlInt32   sError;

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
    STL_THROW( RAMP_ERR_NOTSUPPORT);

    STL_RAMP( RAMP_CONTINUE );

    if( bind( aSock, (struct sockaddr *)&aSockAddr->mSa, sSaLen ) == -1 )
    {
        sError = WSAGetLastError();

        STL_TRY_THROW(sError != WSAEBADF, RAMP_ERR_BADF);
        STL_TRY_THROW(sError != WSAEADDRINUSE, RAMP_ERR_ADDRINUSE);
        STL_TRY_THROW(sError != WSAEINVAL, RAMP_ERR_INVAL);
        STL_THROW( RAMP_ERR_BIND );
    }

    return STL_SUCCESS;
   
    STL_CATCH( RAMP_ERR_BADF )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_FILE_BAD,
                      NULL,
                      aErrorStack );
        steSetSystemError(sError, aErrorStack);
    }
    STL_CATCH( RAMP_ERR_ADDRINUSE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_ADDR_INUSE,
                      NULL,
                      aErrorStack );
        steSetSystemError(sError, aErrorStack);
    }
    STL_CATCH( RAMP_ERR_INVAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INVALID_ARGUMENT,
                      "socket is already bound to an address",
                      aErrorStack );
        steSetSystemError(sError, aErrorStack);
    }
    STL_CATCH( RAMP_ERR_BIND )
    {
        stlSnprintf(sMsg, 64, "bind() returns errno(%d)", sError);
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_BIND,
                      sMsg,
                      aErrorStack,
                      errno );
        steSetSystemError(sError, aErrorStack);
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
    stlChar  sMsg[64];
    stlInt32 sError;

    if( listen(aSock, aBacklog) == -1 )
    {
        sError = WSAGetLastError();

        STL_TRY_THROW(sError != WSAEBADF, RAMP_ERR_BADF);
        STL_TRY_THROW(sError != WSAEADDRINUSE, RAMP_ERR_ADDRINUSE);
        STL_THROW( RAMP_ERR_LISTEN );
    }
    
    return STL_SUCCESS; 

    STL_CATCH( RAMP_ERR_BADF )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_FILE_BAD,
                      NULL,
                      aErrorStack );
        steSetSystemError(sError, aErrorStack);
    }
    STL_CATCH( RAMP_ERR_ADDRINUSE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_ADDR_INUSE,
                      NULL,
                      aErrorStack );
        steSetSystemError(sError, aErrorStack);
    }
    STL_CATCH( RAMP_ERR_LISTEN )
    {
        stlSnprintf(sMsg, 64, "listen() returns errno(%d)", sError);
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_LISTEN,
                      sMsg,
                      aErrorStack );
        steSetSystemError(sError, aErrorStack);
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
    stlInt32    sError;

    STL_PARAM_VALIDATE( aNewSock != NULL, aErrorStack );

    sSockFD = accept(aSock, (struct sockaddr *)&sSockAddr.mSa, &sSaLen);

    if( sSockFD == INVALID_SOCKET )
    {
        sError = WSAGetLastError();

        STL_TRY_THROW(sError != WSAEWOULDBLOCK, RAMP_ERR_AGAIN);
        STL_TRY_THROW(sError != WSAEBADF, RAMP_ERR_BADF);
        STL_TRY_THROW(sError != WSAEINVAL, RAMP_ERR_INVAL);
        STL_THROW( RAMP_ERR_ACCEPT );
    }

    *aNewSock = sSockFD;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_AGAIN )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_AGAIN,
                      NULL,
                      aErrorStack );
        steSetSystemError(sError, aErrorStack);
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
        steSetSystemError(sError, aErrorStack);
    }
    STL_CATCH( RAMP_ERR_ACCEPT )
    {
        stlSnprintf(sMsg, 64, "accept() returns errno(%d)", sError);
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_ACCEPT,
                      sMsg,
                      aErrorStack );
        steSetSystemError(sError, aErrorStack);
    }

    STL_FINISH;

    return STL_FAILURE;

}

stlStatus stnConnect( stlSocket       aSock,
                      stlSockAddr   * aSockAddr,
                      stlInterval     aTimeout,
                      stlErrorStack * aErrorStack )
{
    stlChar    sMsg[64];
    stlSockLen sSaLen;
    stlInt16   sAf;
    DWORD      sError = 0;
    
    STL_PARAM_VALIDATE( aSockAddr != NULL, aErrorStack );

    STL_TRY_THROW(aSock != INVALID_SOCKET, RAMP_ERR_INVAL);

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
    STL_THROW( RAMP_ERR_NOTSUPPORT);

    STL_RAMP(RAMP_CONTINUE);

    STL_TRY_THROW(connect(aSock, (const struct sockaddr *)&aSockAddr->mSa, sSaLen) != SOCKET_ERROR, RAMP_ERR_CONNECT);

    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_INVAL)
    {
        stlPushError(STL_ERROR_LEVEL_ABORT,
                     STL_ERRCODE_INVALID_ARGUMENT,
                     "the descriptor is not a socket",
                     aErrorStack);
        steSetSystemError(sError, aErrorStack);
    }

    STL_CATCH( RAMP_ERR_CONNECT )
    {
        stlSnprintf(sMsg, 64, "connect() returns errno(%d)", WSAGetLastError());
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_CONNECT,
                      sMsg,
                      aErrorStack );
        steSetSystemError(WSAGetLastError(), aErrorStack);
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

stlStatus stnSocketPair( stlInt32          aDomain,
                         stlInt32          aType,
                         stlInt32          aProtocol,
                         stlSocket         aSockets[2],
                         stlErrorStack    *aErrorStack )
{
    stlPushError(STL_ERROR_LEVEL_ABORT,
                 STL_ERRCODE_NOTIMPL,
                 NULL,
                 aErrorStack);

    return STL_FAILURE;
}
