/*******************************************************************************
 * stnSendRecviWin.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: stnSendRecvUnix.c 13675 2014-10-06 14:17:27Z lym999 $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include "stc.h"
#include "ste.h"
#include "stnUnix.h"


stlStatus stnSend( stlSocket       aSock,
                   const stlChar * aBuf, 
                   stlSize       * aLen,
                   stlErrorStack * aErrorStack )
{
    stlChar   sMsg[64];
    WSABUF    sWsaData;
    DWORD     sBytes = 0;
    stlInt32  sError;

    STL_PARAM_VALIDATE( aBuf != NULL,aErrorStack );
    STL_PARAM_VALIDATE( *aLen > 0, aErrorStack);

    sWsaData.len = (u_long)*aLen;
    sWsaData.buf = (stlChar*)aBuf;

    STL_TRY(WSASend(aSock, &sWsaData, 1, &sBytes, 0, NULL, NULL) != SOCKET_ERROR);

    *aLen = sBytes;

    return STL_SUCCESS;

    STL_FINISH;

    sError = WSAGetLastError();

    switch (sError)
    {
	case WSAEWOULDBLOCK:
            /* errno == EGAIN이거나 EWOULDBLOCK 일경우 다시 시도해야 함 */
            /* sRV < *aLen 일수도 있음. 이 경우도 다시 시도 */
            stlPushError(STL_ERROR_LEVEL_ABORT,
                         STL_ERRCODE_AGAIN,
                         "WSASend() returned EAGAIN or EWOULDBLOCK",
                         aErrorStack);
            break;
	case WSAENOTSOCK:
            stlPushError(STL_ERROR_LEVEL_ABORT,
                         STL_ERRCODE_FILE_BAD,
                         "WSASend() returned EBADF",
                         aErrorStack);
            break;
	default :
            stlSnprintf(sMsg, 64, "OS returned errno(%d)", sError);
            stlPushError(STL_ERROR_LEVEL_ABORT,
                         STL_ERRCODE_FILE_WRITE,
                         sMsg,
                         aErrorStack);
    }

    steSetSystemError(sError, aErrorStack);

    if (sBytes > 0)
    {
        *aLen = sBytes;
    }
    else
    {
        *aLen = 0;
    }

    return STL_FAILURE;
}

stlStatus stnRecv( stlSocket       aSock,
                   stlChar       * aBuf,
                   stlSize       * aLen,
                   stlErrorStack * aErrorStack)
{
    stlChar   sMsg[64];
    WSABUF    sWsaData;
    DWORD     sBytes = 0;
    DWORD     sFlags = 0;
    stlInt32  sError;
		
    STL_PARAM_VALIDATE( aBuf != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aLen != NULL, aErrorStack );

    sWsaData.len = (u_long)*aLen;
    sWsaData.buf = (stlChar*)aBuf;

    STL_TRY_THROW(WSARecv(aSock, &sWsaData, 1, &sBytes, &sFlags, NULL, NULL) != SOCKET_ERROR, RAMP_ERR_RECV);

    *aLen = sBytes;

    STL_TRY_THROW(sBytes != 0, RAMP_ERR_EOF);
    
    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_RECV)
    {
        sError = WSAGetLastError();

        switch (sError)
        {
            case WSAEWOULDBLOCK:
                /* errno == EGAIN이거나 EWOULDBLOCK 일경우 다시 시도해야 함 */
                /* sRV < *aLen 일수도 있음. 이 경우도 다시 시도 */
                stlPushError(STL_ERROR_LEVEL_ABORT,
                             STL_ERRCODE_AGAIN,
                             "WSARecv() returned EAGAIN or EWOULDBLOCK",
                             aErrorStack);
                break;
            case WSAENOTSOCK:
                stlPushError(STL_ERROR_LEVEL_ABORT,
                             STL_ERRCODE_FILE_BAD,
                             "WSARecv() returned EBADF",
                             aErrorStack);
                break;
            default:
                stlSnprintf(sMsg, 64, "OS returned errno(%d)", sError);
                stlPushError(STL_ERROR_LEVEL_ABORT,
                             STL_ERRCODE_FILE_WRITE,
                             sMsg,
                             aErrorStack);
        }

        steSetSystemError(sError, aErrorStack);
    }

    STL_CATCH( RAMP_ERR_EOF )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_EOF,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    if (sBytes > 0)
    {
        *aLen = sBytes;
    }
    else
    {
        *aLen = 0;
    }

    return STL_FAILURE;
}


stlStatus stnSendTo( stlSocket       aSock,
                     stlSockAddr   * aWhere,
                     stlInt32        aFlags,
                     const stlChar * aBuf,
                     stlSize       * aLen,
                     stlErrorStack * aErrorStack)
{
    /**
     * @todo 구현해야 함
     */
    *aLen = 0;

    stlPushError( STL_ERROR_LEVEL_ABORT,
                  STL_ERRCODE_NOTIMPL,
                  NULL,
                  aErrorStack );
    
    return STL_FAILURE;
}

stlStatus stnRecvFrom( stlSocket       aSock,
                       stlSockAddr   * aFrom,
                       stlInt32        aFlags,
                       stlChar       * aBuf, 
                       stlSize       * aLen,
                       stlBool         aIsSockStream,
                       stlErrorStack * aErrorStack )
{
    /**
     * @todo 구현해야 함
     */
    *aLen = 0;

    stlPushError( STL_ERROR_LEVEL_ABORT,
                  STL_ERRCODE_NOTIMPL,
                  NULL,
                  aErrorStack );

    return STL_FAILURE;
}

stlStatus stnSendMsg( stlSocket         aSock,
                      const stlMsgHdr  *aMsgHdr,
                      stlInt32          aFlags,
                      stlErrorStack    *aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  STL_ERRCODE_NOTIMPL,
                  NULL,
                  aErrorStack );

    return STL_FAILURE;
}

stlStatus stnRecvMsg( stlSocket         aSock,
                      stlMsgHdr        *aMsgHdr,
                      stlInt32          aFlags,
                      stlErrorStack    *aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  STL_ERRCODE_NOTIMPL,
                  NULL,
                  aErrorStack );

    return STL_FAILURE;
}
