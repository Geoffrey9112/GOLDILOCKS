/*******************************************************************************
 * stnSockOptWin.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: stnSockOptUnix.c 13562 2014-09-12 08:31:43Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stc.h>
#include <stlDef.h>
#include <ste.h>
#include <stn.h>
#include <stlStrings.h>
#include <stlError.h>
#include <stlMemorys.h>

#ifndef IPV6_V6ONLY
#define IPV6_V6ONLY 27
#endif

static stlStatus stnSetSoBlock( stlSocket       aSock,
                                stlErrorStack * aErrorStack )
{
    stlChar  sMsg[64];
    u_long   sZero = 0;
    stlInt32 sError;

    STL_TRY_THROW(ioctlsocket(aSock, FIONBIO, &sZero) != SOCKET_ERROR, RAMP_ERR_IOCTLSOCKET);

    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_IOCTLSOCKET)
    {
        sError = WSAGetLastError();

        stlSnprintf(sMsg, 64, "ioctlsocket() returned errno(%d)", sError);
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_FCNTL,
                      sMsg,
                      aErrorStack );
        steSetSystemError(sError , aErrorStack);
    }

    STL_FINISH;

    return STL_FAILURE;
}


static stlStatus stnSetSoNonblock( stlSocket       aSock,
                                   stlErrorStack * aErrorStack )
{
    stlChar  sMsg[64];
    u_long   sOne = 1;
    stlInt32 sError;

    STL_TRY_THROW(ioctlsocket(aSock, FIONBIO, &sOne) != SOCKET_ERROR, RAMP_ERR_IOCTLSOCKET);

    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_IOCTLSOCKET)
    {
        sError = WSAGetLastError();

        stlSnprintf(sMsg, 64, "ioctlsocket() returned errno(%d)", sError);
        stlPushError(STL_ERROR_LEVEL_ABORT,
                     STL_ERRCODE_FCNTL,
                     sMsg,
                     aErrorStack);
        steSetSystemError(sError, aErrorStack);
    }

    STL_FINISH;

    return STL_FAILURE;}


stlStatus stnSetSocketOption( stlSocket       aSock, 
                              stlInt32        aOpt,
                              stlInt32        aOn,
                              stlErrorStack * aErrorStack )
{
    stlInt32  sOne;
    stlChar   sMsg[64];

    if( aOn )
    {
        sOne = 1;
    }
    else
    {
        sOne = 0;
    }

    switch( aOpt )
    {
        case STL_SOPT_SO_KEEPALIVE:
            {
                STL_TRY_THROW( setsockopt( aSock,
                                           SOL_SOCKET,
                                           SO_KEEPALIVE,
                                           (void *)&sOne,
                                           STL_SIZEOF(stlInt32) ) != -1,
                               RAMP_ERR_SETSOCKOPT );
                break;
            }
        case STL_SOPT_SO_DEBUG:
            {
                STL_TRY_THROW( setsockopt( aSock,
                                           SOL_SOCKET,
                                           SO_DEBUG,
                                           (void *)&sOne,
                                           STL_SIZEOF(stlInt32) ) != -1,
                               RAMP_ERR_SETSOCKOPT );
                break;
            }
        case STL_SOPT_SO_REUSEADDR:
            {
                STL_TRY_THROW( setsockopt( aSock,
                                           SOL_SOCKET,
                                           SO_REUSEADDR,
                                           (void *)&sOne,
                                           STL_SIZEOF(stlInt32) ) != -1,
                               RAMP_ERR_SETSOCKOPT );
                break;
            }
        case STL_SOPT_SO_SNDBUF:
            {
                STL_TRY_THROW( setsockopt( aSock,
                                           SOL_SOCKET,
                                           SO_SNDBUF,
                                           (void *)&aOn,
                                           STL_SIZEOF(stlInt32) ) != -1,
                               RAMP_ERR_SETSOCKOPT );
                break;
            }
        case STL_SOPT_SO_RCVBUF:
            {
                STL_TRY_THROW( setsockopt( aSock,
                                           SOL_SOCKET,
                                           SO_RCVBUF,
                                           (void *)&aOn,
                                           STL_SIZEOF(stlInt32) ) != -1,
                               RAMP_ERR_SETSOCKOPT );
                break;
            }
        case STL_SOPT_SO_NONBLOCK:
            {
                if( aOn != 0 )
                {
                    STL_TRY( stnSetSoNonblock( aSock,
                                               aErrorStack ) == STL_SUCCESS );
                }
                else
                {
                    STL_TRY( stnSetSoBlock( aSock,
                                            aErrorStack) == STL_SUCCESS );
                }
                break;
            }
        case STL_SOPT_SO_LINGER:
            {
                struct linger sLingerInfo;
                sLingerInfo.l_onoff = aOn;
                sLingerInfo.l_linger = STL_MAX_SECS_TO_LINGER;
                STL_TRY_THROW( setsockopt( aSock,
                                           SOL_SOCKET,
                                           SO_LINGER,
                                           (stlChar *) &sLingerInfo,
                                           STL_SIZEOF(struct linger) ) != -1,
                               RAMP_ERR_SETSOCKOPT );
                break;
            }
        case STL_SOPT_TCP_DEFER_ACCEPT:
            {
#if defined(TCP_DEFER_ACCEPT)
                stlInt32  sOptLevel = IPPROTO_TCP;
                stlInt32  sOptName = TCP_DEFER_ACCEPT;
                STL_TRY_THROW( setsockopt( aSock,
                                           sOptLevel,
                                           sOptName, 
                                           (void *)&aOn,
                                           STL_SIZEOF(stlInt32) ) != -1,
                               RAMP_ERR_SETSOCKOPT );
#else
                STL_THROW( RAMP_ERR_NOTIMPL );
#endif
                break;
            }
        case STL_SOPT_TCP_NODELAY:
            {
                int sOptLevel = IPPROTO_TCP;
                int sOptName = TCP_NODELAY;

#if STC_HAVE_SCTP
                if( aSock->protocol == IPPROTO_SCTP )
                {
                    sOptLevel = IPPROTO_SCTP;
                    sOptName = SCTP_NODELAY;
                }
#endif
                STL_TRY_THROW( setsockopt( aSock,
                                           sOptLevel,
                                           sOptName, 
                                           (void *)&aOn,
                                           STL_SIZEOF(stlInt32) ) != -1,
                               RAMP_ERR_SETSOCKOPT );
                break;
            }
        case STL_SOPT_IPV6_V6ONLY:
            {
#if STC_HAVE_IPV6
                /* we don't know the initial setting of this option,
                 * so don't check sock->options since that optimization
                 * won't work
                 */
                STL_TRY_THROW( setsockopt( aSock,
                                           IPPROTO_IPV6,
                                           IPV6_V6ONLY,
                                           (void *)&aOn,
                                           STL_SIZEOF(stlInt32) ) != -1,
                               RAMP_ERR_SETSOCKOPT );
#else
                STL_THROW( RAMP_ERR_NOTIMPL );
#endif
                break;
            }
        default:
            {
                STL_THROW( RAMP_ERR_INVAL );
            }
    }

    return STL_SUCCESS; 

    STL_CATCH( RAMP_ERR_SETSOCKOPT )
    {
        stlSnprintf(sMsg, 64, "setsockopt() returned errno(%d)", WSAGetLastError());
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SETSOCKOPT,
                      sMsg,
                      aErrorStack );
        steSetSystemError(WSAGetLastError(), aErrorStack);
    }
    STL_CATCH( RAMP_ERR_NOTIMPL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_NOTIMPL,
                      NULL,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_INVAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INVALID_ARGUMENT,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}         



stlStatus stnGetSocketOption( stlSocket       aSock, 
                              stlInt32        aOpt,
                              stlSize       * aRet,
                              stlErrorStack * aErrorStack )
{
    stlSockLen  sOptLen;
    stlChar     sMsg[64];

    STL_PARAM_VALIDATE( aRet != NULL, aErrorStack );

    switch( aOpt )
    {
        case STL_SOPT_SO_KEEPALIVE:
            {
                sOptLen = STL_SIZEOF(stlSize);
                STL_TRY_THROW( getsockopt( aSock,
                                           SOL_SOCKET,
                                           SO_KEEPALIVE,
                                           (void *)aRet,
                                           &sOptLen ) != -1,
                               RAMP_ERR_GETSOCKOPT );
                break;
            }
        case STL_SOPT_SO_DEBUG:
            {
                sOptLen = STL_SIZEOF(stlSize);
                STL_TRY_THROW( getsockopt( aSock,
                                           SOL_SOCKET,
                                           SO_DEBUG,
                                           (void *)aRet,
                                           &sOptLen ) != -1,
                               RAMP_ERR_GETSOCKOPT );
                break;
            }
        case STL_SOPT_SO_REUSEADDR:
            {
                sOptLen = STL_SIZEOF(stlSize);
                STL_TRY_THROW( getsockopt( aSock,
                                           SOL_SOCKET,
                                           SO_REUSEADDR,
                                           (void *)aRet,
                                           &sOptLen ) != -1,
                               RAMP_ERR_GETSOCKOPT );
            }
        case STL_SOPT_SO_SNDBUF:
            {
                sOptLen = STL_SIZEOF(stlSize);
                STL_TRY_THROW( getsockopt( aSock,
                                           SOL_SOCKET,
                                           SO_SNDBUF,
                                           (void *)aRet,
                                           &sOptLen ) != -1,
                               RAMP_ERR_GETSOCKOPT );
                break;
            }
        case STL_SOPT_SO_RCVBUF:
            {
                sOptLen = STL_SIZEOF(stlSize);
                STL_TRY_THROW( getsockopt( aSock,
                                           SOL_SOCKET,
                                           SO_RCVBUF,
                                           (void *)aRet,
                                           &sOptLen ) != -1,
                               RAMP_ERR_GETSOCKOPT );
                break;
            }
        case STL_SOPT_SO_NONBLOCK:
            STL_THROW(RAMP_ERR_NOTIMPL);
            break;
        case STL_SOPT_SO_LINGER:
            {
                struct linger sLingerInfo;
                sOptLen = STL_SIZEOF(struct linger);
                STL_TRY_THROW( getsockopt( aSock,
                                           SOL_SOCKET,
                                           SO_LINGER,
                                           (stlChar *) &sLingerInfo,
                                           &sOptLen ) != -1,
                               RAMP_ERR_GETSOCKOPT );
                if( sLingerInfo.l_onoff == 1 )
                {
                    *aRet = sLingerInfo.l_linger;
                }
                else
                {
                    *aRet = 0;
                }
                break;
            }
        case STL_SOPT_TCP_NODELAY:
            {
                int sOptLevel = IPPROTO_TCP;
                int sOptName = TCP_NODELAY;
                sOptLen = STL_SIZEOF(stlSize);

#if STC_HAVE_SCTP
                if( aSock->protocol == IPPROTO_SCTP )
                {
                    sOptLevel = IPPROTO_SCTP;
                    sOptName = SCTP_NODELAY;
                }
#endif
                STL_TRY_THROW( getsockopt( aSock,
                                           sOptLevel,
                                           sOptName, 
                                           (void *)aRet,
                                           &sOptLen ) != -1,
                               RAMP_ERR_GETSOCKOPT );
                break;
            }
        case STL_SOPT_IPV6_V6ONLY:
            {
#if STC_HAVE_IPV6
                sOptLen = STL_SIZEOF(stlSize);
                STL_TRY_THROW( getsockopt( aSock,
                                           IPPROTO_IPV6,
                                           IPV6_V6ONLY,
                                           (void *)aRet,
                                           &sOptLen ) != -1,
                               RAMP_ERR_GETSOCKOPT );
#else
                STL_THROW( RAMP_ERR_NOTIMPL);
#endif
                break;
            }
        default:
            {
                STL_THROW( RAMP_ERR_INVAL );
            }
    }

    return STL_SUCCESS; 

    STL_CATCH( RAMP_ERR_GETSOCKOPT )
    {
        stlSnprintf(sMsg, 64, "getsockopt() returned errno(%d)", WSAGetLastError());
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_GETSOCKOPT,
                      sMsg,
                      aErrorStack );
        steSetSystemError(WSAGetLastError(), aErrorStack);
    }
    STL_CATCH( RAMP_ERR_NOTIMPL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_NOTIMPL,
                      NULL,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_INVAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INVALID_ARGUMENT,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}         


stlStatus stnGetHostName( stlChar * aBuf, stlSize aLen, stlErrorStack * aErrorStack )
{
    stlChar  sMsg[64];

    STL_PARAM_VALIDATE( aBuf != NULL, aErrorStack );

    if( gethostname(aBuf, aLen) == -1 )
    {
        aBuf[0] = '\0';
        STL_THROW( RAMP_ERR_GETHOSTNAME );
    }
    else if( stlMemchr(aBuf, '\0', aLen) == NULL )
    {   
        /* buffer too small */
        aBuf[0] = '\0';
        STL_THROW( RAMP_ERR_ENAMETOOLONG );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_GETHOSTNAME )
    {
        stlSnprintf(sMsg, 64, "gethostname() returned errno(%d)", WSAGetLastError());
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_GETHOSTNAME,
                      sMsg,
                      aErrorStack );
        steSetSystemError(WSAGetLastError(), aErrorStack);
    }
    STL_CATCH( RAMP_ERR_ENAMETOOLONG )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_NAMETOOLONG,
                      NULL,
                      aErrorStack,
                      aBuf );
    }

    STL_FINISH;

    return STL_FAILURE;
}

