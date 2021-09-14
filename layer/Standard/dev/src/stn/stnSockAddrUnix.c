/*******************************************************************************
 * stnSockAddrUnix.c
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


stlStatus stnGetSockName( stlSocket       aSock, 
                          stlSockAddr   * aSockAddr,
                          stlErrorStack * aErrorStack )
{
    stlChar    sMsg[64];
    stlSockLen sSaLen = STL_SIZEOF(stlSockAddr);

    STL_PARAM_VALIDATE( aSockAddr != NULL, aErrorStack );

    STL_TRY_THROW( getsockname( aSock,
                                (struct sockaddr *)aSockAddr,
                                &sSaLen ) >= 0,
                   RAMP_ERR_GETSOCKNAME );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_GETSOCKNAME )
    {
        stlSnprintf(sMsg, 64, "getsockname returned errno(%d)", errno);
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_GETSOCKNAME,
                      sMsg,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stnGetPeerName( stlSocket       aSock, 
                          stlSockAddr   * aSockAddr,
                          stlErrorStack * aErrorStack )
{
    stlChar    sMsg[64];
    stlSockLen sSaLen = STL_SIZEOF(stlSockAddr);

    STL_PARAM_VALIDATE( aSockAddr != NULL, aErrorStack );

    STL_TRY_THROW( getpeername( aSock,
                                (struct sockaddr *)aSockAddr,
                                &sSaLen) >= 0,
                   RAMP_ERR_GETPEERNAME );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_GETPEERNAME )
    {
        stlSnprintf(sMsg, 64, "getpeername returned errno(%d)", errno);
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_GETPEERNAME,
                      sMsg,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stnGetSockAddrIp( stlSockAddr   * aSockAddr,
                            stlChar       * aAddrBuf,
                            stlSize         aAddrBufLen,
                            stlErrorStack * aErrorStack )
{
    void *   sAddrPtr;
    stlChar  sMsg[64];
    stlInt16 sAf;

    STL_PARAM_VALIDATE( aAddrBuf != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aSockAddr != NULL, aErrorStack );

    sAf = ((struct sockaddr*)aSockAddr)->sa_family;

    if( sAf == STL_AF_INET )
    {
        sAddrPtr = (void*)&(aSockAddr->mSa.mIn4.sin_addr);
        STL_THROW( RAMP_CONTINUE );
    }
#if STC_HAVE_IPV6
    if( sAf == STL_AF_INET6 )
    {
        sAddrPtr = (void*)&(aSockAddr->mSa.mIn6.sin6_addr);
        STL_THROW( RAMP_CONTINUE );
    }
#endif
    STL_THROW( RAMP_ERR_NOSUPPORT );

    STL_RAMP( RAMP_CONTINUE ); 

    STL_TRY( stnGetStrAddrFromBin( sAf,
                                   sAddrPtr,
                                   aAddrBuf,
                                   aAddrBufLen,
                                   aErrorStack ) == STL_SUCCESS );

#if STC_HAVE_IPV6
    if( (sAf == STL_AF_INET6) &&
        (IN6_IS_ADDR_V4MAPPED((struct in6_addr *)sAddrPtr)) &&
        (aAddrBufLen > stlStrlen("::ffff:")) )
    {
        /* This is an IPv4-mapped IPv6 address; drop the leading
         * part of the address string so we're left with the familiar
         * IPv4 format.
         */
        stlMemmove( aAddrBuf,
                    aAddrBuf + strlen("::ffff:"),
                    stlStrlen(aAddrBuf + strlen("::ffff:"))+1 );
    }
#endif
    /* ensure NUL termination if the buffer is too short */
    aAddrBuf[aAddrBufLen - 1] = '\0';

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOSUPPORT )
    {
        stlSnprintf(sMsg,64, "not supported address family(%d)" , sAf);
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_NOTSUPPORT ,
                      sMsg,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus stnSetSockAddr( stlSockAddr   * aSockAddr,
                          stlInt16        aFamily,
                          stlChar       * aAddrStr,
                          stlPort         aPort,
                          stlChar       * aFilePath,
                          stlErrorStack * aErrorStack)
{
    stlChar  sMsg[64];

    STL_PARAM_VALIDATE( aSockAddr != NULL, aErrorStack );

    if( aFamily == STL_AF_INET )
    {
        aSockAddr->mSa.mIn4.sin_family = aFamily;
        STL_TRY( stnGetBinAddrFromStr( aFamily,
                                       aAddrStr,
                                       &aSockAddr->mSa.mIn4.sin_addr,
                                       aErrorStack )
                 == STL_SUCCESS );
        if( aPort != 0 )
        {
            aSockAddr->mSa.mIn4.sin_port = stlToNetShort(aPort);
        }
    }
#if STC_HAVE_IPV6
    else if( aFamily == STL_AF_INET6 )
    {
        struct if_nameindex * sIfIdx;
        struct if_nameindex * sIfIdxList;

        aSockAddr->mSa.mIn6.sin6_family = aFamily;
        aSockAddr->mSa.mIn6.sin6_flowinfo = 0;
        STL_TRY( stnGetBinAddrFromStr( aFamily,
                                       aAddrStr,
                                       &aSockAddr->mSa.mIn6.sin6_addr,
                                       aErrorStack )
                 == STL_SUCCESS );
        aSockAddr->mSa.mIn6.sin6_flowinfo = 0;
        if( aPort != 0 )
        {
            aSockAddr->mSa.mIn6.sin6_port = stlToNetShort(aPort);
        }

        /**
         * @todo 인터페이스 이름을 입력받을 수 있어야 한다.
         */
        sIfIdxList = if_nameindex();
        for( sIfIdx = sIfIdxList; sIfIdx->if_name != NULL; sIfIdx++ )
        {
            if( stlStrcmp( sIfIdx->if_name, "lo" )== 0 )
            {
                continue;
            }
            aSockAddr->mSa.mIn6.sin6_scope_id = sIfIdx->if_index;
            if_freenameindex( sIfIdxList );
            break;
        }
    }
#endif
#if STC_HAVE_UNIXDOMAIN
    else if( aFamily == STL_AF_UNIX )
    {
        aSockAddr->mSa.mUn.sun_family = aFamily;
        if( aFilePath != NULL )
        {
            stlStrcpy(aSockAddr->mSa.mUn.sun_path, aFilePath);
        }
    }
#endif
    else
    {
        STL_THROW( RAMP_ERR_NOSUPPORT );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOSUPPORT )
    {
        stlSnprintf(sMsg, 64, "not supported address family(%d)", aFamily);
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_NOTSUPPORT ,
                      sMsg,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus stnGetSockAddr( stlSockAddr   * aSockAddr,
                          stlChar       * aAddrBuf,
                          stlSize         aAddrBufLen,
                          stlPort       * aPort,
                          stlChar       * aFilePathBuf,
                          stlInt32        aFilePathBufLen,
                          stlErrorStack * aErrorStack)
{
    stlChar   sMsg[64];
    stlInt16  sFamily;

    STL_PARAM_VALIDATE( aSockAddr != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aAddrBuf != NULL, aErrorStack );

    sFamily = ((struct sockaddr*)aSockAddr)->sa_family;

    if( sFamily == STL_AF_INET )
    {
        STL_TRY( stlGetStrAddrFromBin( sFamily,
                                       &aSockAddr->mSa.mIn4.sin_addr,
                                       aAddrBuf,
                                       aAddrBufLen,
                                       aErrorStack )
                 == STL_SUCCESS );

        if( aPort != NULL )
        {
            *aPort = stlToHostShort(aSockAddr->mSa.mIn4.sin_port);
        }
    }
#if STC_HAVE_IPV6
    else if( sFamily == STL_AF_INET6 )
    {
        STL_TRY( stlGetStrAddrFromBin( sFamily,
                                       &aSockAddr->mSa.mIn6.sin6_addr,
                                       aAddrBuf,
                                       aAddrBufLen,
                                       aErrorStack )
                 == STL_SUCCESS );
        if( aPort != NULL )
        {
            *aPort = stlToHostShort(aSockAddr->mSa.mIn6.sin6_port);
        }
    }
#endif
#if STC_HAVE_UNIXDOMAIN
    else if( sFamily == STL_AF_UNIX )
    {
        if( aFilePathBuf != NULL )
        {
            if( aFilePathBufLen <= stlStrlen(aSockAddr->mSa.mUn.sun_path) )
            {
                aFilePathBufLen = stlStrlen(aSockAddr->mSa.mUn.sun_path);
            }
            stlStrncpy(aFilePathBuf, aSockAddr->mSa.mUn.sun_path, aFilePathBufLen);
        }
    }
#endif
    else
    {
        STL_THROW( RAMP_ERR_NOSUPPORT );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOSUPPORT )
    {
        stlSnprintf(sMsg, 64, "not supported address family(%d)", sFamily);
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_NOTSUPPORT ,
                      sMsg,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


