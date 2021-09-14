/*******************************************************************************
 * stnSockOptUnix.c
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

#include "stc.h"
#include "ste.h"
#include "stn.h"
#include "stnUnix.h"
#include <netinet/tcp.h>


static stlStatus stnSetSoBlock( stlSocket       aSock,
                                stlErrorStack * aErrorStack )
{
/* BeOS uses setsockopt at present for non blocking... */
    stlChar sMsg[64];
#ifndef BEOS
    stlInt32 sFdFlags;
    stlInt32 sRet;

    do
    {
        sFdFlags = fcntl(aSock, F_GETFL, 0);
    } while( (sFdFlags == -1) && (errno == EINTR) );
    
#if defined(O_NONBLOCK)
    sFdFlags &= ~O_NONBLOCK;
#elif defined(O_NDELAY)
    sFdFlags &= ~O_NDELAY;
#elif defined(FNDELAY)
    sFdFlags &= ~FNDELAY;
#else
#error Please teach APR how to make sockets blocking on your platform.
#endif
    do
    {
        sRet = fcntl(aSock, F_SETFL, sFdFlags);
    } while( (sRet == -1) && (errno == EINTR) );
    
    STL_TRY_THROW( sRet != -1, RAMP_ERR_FCNTL );
#else
    stlInt32 sOn = 0;
    STL_TRY_THROW( setsockopt( aSock,
                               SOL_SOCKET, 
                               SO_NONBLOCK,
                               &sOn,
                               STL_SIZEOF(stlInt32)) >= 0,
                   RAMP_ERR_SETSOCKOPT );
#endif /* BEOS */
    return STL_SUCCESS;

#ifndef BEOS
    STL_CATCH( RAMP_ERR_FCNTL )
    {
        stlSnprintf(sMsg, 64, "fcntl() returned errno(%d)", errno);
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_FCNTL,
                      sMsg,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
#else
    STL_CATCH( RAMP_ERR_SETSOCKOPT )
    {
        stlSnprintf(sMsg, 64, "setsockopt() returned errno(%d)", errno);
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SETSOCKOPT,
                      sMsg,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
#endif

    STL_FINISH;

    return STL_FAILURE;

}


static stlStatus stnIsSoBlock( stlSocket       aSock,
                               stlBool       * aSet,
                               stlErrorStack * aErrorStack )
{
/* BeOS uses setsockopt at present for non blocking... */
#ifdef BEOS
    stlChar sMsg[64];
#endif
    STL_PARAM_VALIDATE( aSet != NULL, aErrorStack );
#ifndef BEOS
    stlInt32 sFdFlags;

    do
    {
        sFdFlags = fcntl(aSock, F_GETFL, 0);
    } while( (sFdFlags == -1) && (errno == EINTR) );
    
#if defined(O_NONBLOCK)
    if( (sFdFlags & O_NONBLOCK) == 0 )
    {
        *aSet = STL_TRUE;
    }
    else
    {
        *aSet = STL_FALSE;
    }
    sFdFlags &= ~O_NONBLOCK;
#elif defined(O_NDELAY)
    if( (sFdFlags & O_NDELAY) == 0 )
    {
        *aSet = STL_TRUE;
    }
    else
    {
        *aSet = STL_FALSE;
    }
#elif defined(FNDELAY)
    if( (sFdFlags & O_FNDELAY) == 0 )
    {
        *aSet = STL_TRUE;
    }
    else
    {
        *aSet = STL_FALSE;
    }
#else
#error Please teach APR how to make sockets blocking on your platform.
#endif
#else
    stlInt32 sOn;
    stlSize  sLen = STL_SIZEOF(stlInt32);
    STL_TRY_THROW( getsockopt( aSock,
                               SOL_SOCKET, 
                               SO_NONBLOCK,
                               &sOn,
                               &sLen ) >= 0,
                   RAMP_ERR_SETSOCKOPT );
    if( sOn == 0 )
    {
        *aSet = STL_TRUE;
    }
    else
    {
        *aSet = STL_TRUE;
    }
#endif /* BEOS */
    return STL_SUCCESS;

#ifdef BEOS
    STL_CATCH( RAMP_ERR_SETSOCKOPT )
    {
        stlSnprintf(sMsg, 64, "setsockopt() returned errno(%d)", errno);
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SETSOCKOPT,
                      sMsg,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
#endif

    STL_FINISH;

    return STL_FAILURE;

}


static stlStatus stnSetSoNonblock( stlSocket       aSock,
                                   stlErrorStack * aErrorStack )
{
    stlChar  sMsg[64];
#ifndef BEOS
    stlInt32 sFdFlags;
    stlInt32 sRet;

    do
    {
        sFdFlags = fcntl(aSock, F_GETFL, 0);
    } while( (sFdFlags == -1) && (errno == EINTR) );

    STL_TRY_THROW( sFdFlags != -1, RAMP_ERR_FCNTL );
    
#if defined(O_NONBLOCK)
    sFdFlags |= O_NONBLOCK;
#elif defined(O_NDELAY)
    sFdFlags |= O_NDELAY;
#elif defined(FNDELAY)
    sFdFlags |= FNDELAY;
#else
#error Please teach STL how to make sockets non-blocking on your platform.
#endif
    do
    {
        sRet = fcntl(aSock, F_SETFL, sFdFlags);
    } while( (sRet == -1) && (errno == EINTR) );
    
    STL_TRY_THROW( sRet != -1, RAMP_ERR_FCNTL );
#else
    stlInt32 sOn = 1;
    STL_TRY_THROW( setsockopt( aSock,
                               SOL_SOCKET,
                               SO_NONBLOCK,
                               &sOn,
                               STL_SIZEOF(stlInt32)) >= 0,
                   RAMP_ERR_SETSOCKOPT );
#endif /* BEOS */
    return STL_SUCCESS;

#ifndef BEOS
    STL_CATCH( RAMP_ERR_FCNTL )
    {
        stlSnprintf(sMsg, 64, "fcntl() returned errno(%d)", errno);
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_FCNTL,
                      sMsg,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
#else
    STL_CATCH( RAMP_ERR_SETSOCKOPT )
    {
        stlSnprintf(sMsg, 64, "setsockopt() returned errno(%d)", errno);
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SETSOCKOPT,
                      sMsg,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
#endif

    STL_FINISH;

    return STL_FAILURE;
}


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
#ifdef SO_KEEPALIVE
                STL_TRY_THROW( setsockopt( aSock,
                                           SOL_SOCKET,
                                           SO_KEEPALIVE,
                                           (void *)&sOne,
                                           STL_SIZEOF(stlInt32) ) != -1,
                               RAMP_ERR_SETSOCKOPT );
#else
                STL_THROW( RAMP_ERR_NOTIMPL );
#endif
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
            }
        case STL_SOPT_SO_SNDBUF:
            {
#ifdef SO_SNDBUF
                STL_TRY_THROW( setsockopt( aSock,
                                           SOL_SOCKET,
                                           SO_SNDBUF,
                                           (void *)&aOn,
                                           STL_SIZEOF(stlInt32) ) != -1,
                               RAMP_ERR_SETSOCKOPT );
#else
                STL_THROW( RAMP_ERR_NOTIMPL );
#endif
                break;
            }
        case STL_SOPT_SO_RCVBUF:
            {
#ifdef SO_RCVBUF
                STL_TRY_THROW( setsockopt( aSock,
                                           SOL_SOCKET,
                                           SO_RCVBUF,
                                           (void *)&aOn,
                                           STL_SIZEOF(stlInt32) ) != -1,
                               RAMP_ERR_SETSOCKOPT );
#else
                STL_THROW( RAMP_ERR_NOTIMPL );
#endif
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
#ifdef SO_LINGER
                struct linger sLingerInfo;
                sLingerInfo.l_onoff = aOn;
                sLingerInfo.l_linger = STL_MAX_SECS_TO_LINGER;
                STL_TRY_THROW( setsockopt( aSock,
                                           SOL_SOCKET,
                                           SO_LINGER,
                                           (stlChar *) &sLingerInfo,
                                           STL_SIZEOF(struct linger) ) != -1,
                               RAMP_ERR_SETSOCKOPT );
#else
                STL_THROW( RAMP_ERR_NOTIMPL );
#endif
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
#if defined(TCP_NODELAY)
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
#else
                /* BeOS pre-BONE has TCP_NODELAY set by default.
                 * As it can't be turned off we might as well check if they're asking
                 * for it to be turned on!
                 */
#ifdef BEOS
                if( aOn == 1 )
                {
                    STL_THROW( RAMP_TERMINATE );
                }
                else
                {
                    STL_THROW( RAMP_ERR_NOTIMPL );
                }
#endif
                STL_THROW( RAMP_ERR_NOTIMPL );
#endif
                break;
            }
        case STL_SOPT_TCP_NOPUSH:
            {
#if STC_TCP_NOPUSH_FLAG
                /* TCP_NODELAY and TCP_CORK are mutually exclusive on Linux
                 * kernels < 2.6; on newer kernels they can be used together
                 * and TCP_CORK takes preference, which is the desired
                 * behaviour.  On older kernels, TCP_NODELAY must be toggled
                 * to "off" whilst TCP_CORK is in effect. */
#ifndef STC_HAVE_TCP_NODELAY_WITH_CORK
                stlInt32 sOptLevel = IPPROTO_TCP;
                stlInt32 sOptName = TCP_NODELAY;

#if STC_HAVE_SCTP
                if( aSock->protocol == IPPROTO_SCTP )
                {
                    sOptLevel = IPPROTO_SCTP;
                    sOptName = SCTP_NODELAY;
                }
#endif
                /* Now toggle TCP_NODELAY to off, if TCP_CORK is being
                 * turned on: */
                {
                    stlInt32 sTmpFlag = 0;
                    STL_TRY_THROW( setsockopt( aSock,
                                               sOptLevel, sOptName,
                                               (void*)&sTmpFlag,
                                               STL_SIZEOF(stlInt32) ) != -1,
                                   RAMP_ERR_SETSOCKOPT );
                }
#endif /* HAVE_TCP_NODELAY_WITH_CORK */

                /* OK, now we can just set the TCP_NOPUSH flag accordingly...*/
                STL_TRY_THROW( setsockopt( aSock,
                                           IPPROTO_TCP,
                                           STL_TCP_NOPUSH_FLAG,
                                           (void*)&aOn,
                                           STL_SIZEOF(stlInt32) ) != -1,
                               RAMP_ERR_SETSOCKOPT );
#ifndef STC_HAVE_TCP_NODELAY_WITH_CORK
                {
                    /* Now, if TCP_CORK was just turned off, turn
                     * TCP_NODELAY back on again if it was earlier toggled
                     * to off: */
                    stlInt32 sTmpFlag = 1;
                    STL_TRY_THROW( setsockopt( aSock,
                                               sOptLevel,
                                               sOptName,
                                               (void*)&sTmpFlag,
                                               STL_SIZEOF(stlInt32) ) != -1,
                                   RAMP_ERR_SETSOCKOPT );
                }
#endif /* HAVE_TCP_NODELAY_WITH_CORK */
#else
                STL_THROW( RAMP_ERR_NOTIMPL );
#endif
                break;
            }
        case STL_SOPT_IPV6_V6ONLY:
            {
#if STC_HAVE_IPV6 && defined(IPV6_V6ONLY)
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
        stlSnprintf(sMsg, 64, "setsockopt() returned errno(%d)", errno);
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SETSOCKOPT,
                      sMsg,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
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
    stlBool     sIsSet;
    stlSockLen  sOptLen;
    stlChar     sMsg[64];

    STL_PARAM_VALIDATE( aRet != NULL, aErrorStack );

    switch( aOpt )
    {
        case STL_SOPT_SO_KEEPALIVE:
            {
#ifdef SO_KEEPALIVE
                sOptLen = STL_SIZEOF(stlSize);
                STL_TRY_THROW( getsockopt( aSock,
                                           SOL_SOCKET,
                                           SO_KEEPALIVE,
                                           (void *)aRet,
                                           &sOptLen ) != -1,
                               RAMP_ERR_GETSOCKOPT );
#else
                STL_THROW( RAMP_ERR_NOTIMPL );
#endif
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
#ifdef SO_SNDBUF
                sOptLen = STL_SIZEOF(stlSize);
                STL_TRY_THROW( getsockopt( aSock,
                                           SOL_SOCKET,
                                           SO_SNDBUF,
                                           (void *)aRet,
                                           &sOptLen ) != -1,
                               RAMP_ERR_GETSOCKOPT );
#else
                STL_THROW( RAMP_ERR_NOTIMPL );
#endif
                break;
            }
        case STL_SOPT_SO_RCVBUF:
            {
#ifdef SO_RCVBUF
                sOptLen = STL_SIZEOF(stlSize);
                STL_TRY_THROW( getsockopt( aSock,
                                           SOL_SOCKET,
                                           SO_RCVBUF,
                                           (void *)aRet,
                                           &sOptLen ) != -1,
                               RAMP_ERR_GETSOCKOPT );
#else
                STL_THROW( RAMP_ERR_NOTIMPL );
#endif
                break;
            }
        case STL_SOPT_SO_NONBLOCK:
            {
                sOptLen = STL_SIZEOF(stlSize);
                STL_TRY( stnIsSoBlock( aSock,
                                       &sIsSet,
                                       aErrorStack ) == STL_SUCCESS );
                if( sIsSet == STL_TRUE )
                {
                    *aRet = 1;
                }
                else
                {
                    *aRet = 0;
                }
                break;
            }
        case STL_SOPT_SO_LINGER:
            {
#ifdef SO_LINGER
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
#else
                STL_THROW( RAMP_ERR_NOTIMPL );
#endif
                break;
            }
        case STL_SOPT_TCP_DEFER_ACCEPT:
            {
#if defined(TCP_DEFER_ACCEPT)
                stlInt32  sOptLevel = IPPROTO_TCP;
                stlInt32  sOptName = TCP_DEFER_ACCEPT;
                sOptLen = STL_SIZEOF(stlSize);
                STL_TRY_THROW( getsockopt( aSock,
                                           sOptLevel,
                                           sOptName, 
                                           (void *)aRet,
                                           &sOptLen ) != -1,
                               RAMP_ERR_GETSOCKOPT );
#else
                STL_THROW( RAMP_ERR_NOTIMPL );
#endif
                break;
            }
        case STL_SOPT_TCP_NODELAY:
            {
#if defined(TCP_NODELAY)
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
#else
                /* BeOS pre-BONE has TCP_NODELAY set by default.
                 * As it can't be turned off we might as well check if they're asking
                 * for it to be turned on!
                 */
#ifdef BEOS
                if( aOn == 1 )
                {
                    STL_THROW( RAMP_TERMINATE );
                }
                else
                {
                    STL_THROW( RAMP_ERR_NOTIMPL );
                }
#endif
                STL_THROW( RAMP_ERR_NOTIMPL );
#endif
                break;
            }
        case STL_SOPT_TCP_NOPUSH:
            {
                sOptLen = STL_SIZEOF(stlSize);
#if STC_TCP_NOPUSH_FLAG
                /* OK, now we can just set the TCP_NOPUSH flag accordingly...*/
                STL_TRY_THROW( getsockopt( aSock,
                                           IPPROTO_TCP,
                                           STL_TCP_NOPUSH_FLAG,
                                           (void*)aRet,
                                           &sOptLen ) != -1,
                               RAMP_ERR_GETSOCKOPT );
#else
                STL_THROW( RAMP_ERR_NOTIMPL );
#endif
                break;
            }
        case STL_SOPT_IPV6_V6ONLY:
            {
#if STC_HAVE_IPV6 && defined(IPV6_V6ONLY)
                sOptLen = STL_SIZEOF(stlSize);
                STL_TRY_THROW( getsockopt( aSock,
                                           IPPROTO_IPV6,
                                           IPV6_V6ONLY,
                                           (void *)aRet,
                                           &sOptLen ) != -1,
                               RAMP_ERR_GETSOCKOPT );
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

    STL_CATCH( RAMP_ERR_GETSOCKOPT )
    {
        stlSnprintf(sMsg, 64, "getsockopt() returned errno(%d)", errno);
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_GETSOCKOPT,
                      sMsg,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
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

#ifdef BEOS_R5
    if( gethostname(aBuf, aLen) == 0 )
    {
#else
    if( gethostname(aBuf, aLen) != 0 )
    {
#endif  
        aBuf[0] = '\0';
        STL_THROW( RAMP_ERR_GETHOSTNAME );
    }
    else if( stlMemchr(aBuf, '\0', aLen) == NULL )
    {   /* buffer too small */
        /* note... most platforms just truncate in this condition
         *         linux+glibc return an error
         */
        aBuf[0] = '\0';
        STL_THROW( RAMP_ERR_ENAMETOOLONG );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_GETHOSTNAME )
    {
        stlSnprintf(sMsg, 64, "gethostname() returned errno(%d)", errno);
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_GETHOSTNAME,
                      sMsg,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
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

