/*******************************************************************************
 * stnSendRecviUnix.c
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
#include "stnUnix.h"


stlStatus stnSend( stlSocket       aSock,
                   const stlChar * aBuf, 
                   stlSize       * aLen,
                   stlErrorStack * aErrorStack )
{
    stlSsize  sRetVal = 0;
    stlChar   sMsg[64];

    STL_PARAM_VALIDATE( aBuf != NULL,aErrorStack );
    STL_PARAM_VALIDATE( *aLen > 0, aErrorStack);

    do
    {
        sRetVal = write(aSock, aBuf, (*aLen));
    } while( (sRetVal == -1) && (errno == EINTR) );

    if( sRetVal == -1 )
    {
        STL_TRY_THROW( (errno != EAGAIN) && (errno != EWOULDBLOCK),
                       RAMP_ERR_EAGAIN );
        STL_TRY_THROW( errno != EBADF, RAMP_ERR_EBADF );
        STL_THROW( RAMP_ERR_FILE_WRITE );
    }
    
    (*aLen) = sRetVal;

    return STL_SUCCESS;

    /* errno == EGAIN이거나 EWOULDBLOCK 일경우 다시 시도해야 함 */
    /* sRV < *aLen 일수도 있음. 이 경우도 다시 시도 */
    STL_CATCH( RAMP_ERR_EAGAIN )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_AGAIN,
                      "write() returned EAGAIN or EWOULDBLOCK",
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    STL_CATCH( RAMP_ERR_EBADF )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_FILE_BAD,
                      "write() returned EBADF",
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    STL_CATCH( RAMP_ERR_FILE_WRITE )
    {
        stlSnprintf(sMsg, 64, "OS returned errno(%d)", errno);
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_FILE_WRITE,
                      sMsg,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }

    STL_FINISH;

    if( sRetVal > 0 )
    {
        *aLen = sRetVal;
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
    stlSsize  sRetVal = -1;
    stlChar   sMsg[64];

    STL_PARAM_VALIDATE( aBuf != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aLen != NULL, aErrorStack );

    do
    {
        sRetVal = read(aSock, aBuf, (*aLen));
    } while( (sRetVal == -1) && (errno == EINTR) );

    if( sRetVal == -1 )
    {
        STL_TRY_THROW( (errno != EAGAIN) && (errno != EWOULDBLOCK),
                       RAMP_ERR_EAGAIN );
        STL_TRY_THROW( errno != EBADF, RAMP_ERR_EBADF );
        STL_THROW( RAMP_ERR_FILE_READ );
    }


    (*aLen) = sRetVal;

    STL_TRY_THROW( sRetVal != 0, RAMP_ERR_EOF );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_EAGAIN )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_AGAIN,
                      "read() returned EAGAIN or EWOULDBLOCK",
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    STL_CATCH( RAMP_ERR_EBADF )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_FILE_BAD,
                      "read() returned EBADF",
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    STL_CATCH( RAMP_ERR_FILE_READ )
    {
        stlSnprintf(sMsg, 64, "OS returned errno(%d)", errno);
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_FILE_READ,
                      sMsg,
                      aErrorStack,
                      errno );
        steSetSystemError( errno, aErrorStack );
    }
    STL_CATCH( RAMP_ERR_EOF )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_EOF,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    if( sRetVal > 0 )
    {
        *aLen = sRetVal;
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
    stlSsize    sRetVal;
    stlSockLen  sSaLen;
    stlInt16    sAf;
    stlChar     sMsg[64];

    STL_PARAM_VALIDATE( aWhere != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aBuf != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aLen != NULL, aErrorStack );

    sAf = ((struct sockaddr*)aWhere)->sa_family;

    if( sAf == STL_AF_INET )
    {
        sSaLen = STL_SIZEOF(aWhere->mSa.mIn4);
        STL_THROW( RAMP_CONTINUE );
    }
#if STC_HAVE_IPV6
    if( sAf == STL_AF_INET6 )
    {
        sSaLen = STL_SIZEOF(aWhere->mSa.mIn6);
        STL_THROW( RAMP_CONTINUE );
    }
#endif
#if STC_HAVE_UNIXDOMAIN
    if( sAf == STL_AF_UNIX )
    {
        sSaLen = STL_SIZEOF(aWhere->mSa.mUn);
        STL_THROW( RAMP_CONTINUE );
    }
#endif

    STL_THROW( RAMP_ERR_NOSUPPORT );

    STL_RAMP( RAMP_CONTINUE );

    do
    {
        sRetVal = sendto( aSock,
                          aBuf,
                          (*aLen),
                          aFlags, 
                          (const struct sockaddr*)&aWhere->mSa, 
                          sSaLen );
    } while( (sRetVal == -1) && (errno == EINTR) );

    if( sRetVal == -1 )
    {
        STL_TRY_THROW( (errno != EAGAIN) && (errno != EWOULDBLOCK),
                       RAMP_ERR_EAGAIN );
        STL_TRY_THROW( errno != EBADF, RAMP_ERR_EBADF );
        STL_THROW( RAMP_ERR_SENDTO );
    }

    *aLen = sRetVal;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_EAGAIN )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_AGAIN,
                      "sendto() returned EAGAIN or EWOULDBLOCK",
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    STL_CATCH( RAMP_ERR_EBADF )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_FILE_BAD,
                      "sendto() returned EBADF",
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    STL_CATCH( RAMP_ERR_SENDTO )
    {
        stlSnprintf( sMsg, 64, "OS returned errno(%d)", errno );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SENDTO,
                      sMsg,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    STL_CATCH( RAMP_ERR_NOSUPPORT )
    {
        stlSnprintf( sMsg, 64, "not supported address format(%d)" , sAf );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_NOTSUPPORT ,
                      sMsg,
                      aErrorStack );
    }

    STL_FINISH;

    *aLen = 0;

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
    stlSsize   sRetVal;
    stlChar    sMsg[64];
    stlSockLen sSaLen = STL_SIZEOF(stlSockAddr);

    STL_PARAM_VALIDATE( aFrom != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aBuf != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aLen != NULL, aErrorStack );

    do
    {
        sRetVal = recvfrom( aSock,
                            aBuf,
                            (*aLen),
                            aFlags, 
                            (struct sockaddr*)&aFrom->mSa,
                            &sSaLen );
    } while( (sRetVal == -1) && (errno == EINTR) );

    if( sRetVal == -1 )
    {
        STL_TRY_THROW( (errno != EAGAIN) && (errno != EWOULDBLOCK),
                       RAMP_ERR_EAGAIN );
        STL_TRY_THROW( errno != EBADF, RAMP_ERR_EBADF );
        STL_THROW( RAMP_ERR_RECVFROM );
    }

    (*aLen) = sRetVal;

    STL_TRY_THROW( (sRetVal != 0) || (aIsSockStream != STL_TRUE), RAMP_ERR_EOF );

    return STL_SUCCESS;
 
    STL_CATCH( RAMP_ERR_EAGAIN )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_AGAIN,
                      "recvfrom() returned EAGAIN or EWOULDBLOCK",
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    STL_CATCH( RAMP_ERR_EBADF )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_FILE_BAD,
                      "recvfrom() returned EBADF",
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    STL_CATCH( RAMP_ERR_RECVFROM )
    {
        stlSnprintf( sMsg, 64, "OS returned errno(%d)", errno );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_RECVFROM,
                      sMsg,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    STL_CATCH( RAMP_ERR_EOF )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_EOF,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    *aLen = 0;

    return STL_FAILURE;
}

stlStatus stnSendMsg( stlSocket         aSock,
                      const stlMsgHdr  *aMsgHdr,
                      stlInt32          aFlags,
                      stlErrorStack    *aErrorStack )
{
    stlSsize   sRetVal;
    stlChar    sMsg[64];

    do
    {
        sRetVal = sendmsg( aSock, aMsgHdr, aFlags );
    } while( (sRetVal == -1) && (errno == EINTR) );

    if(sRetVal == -1)
    {
        STL_TRY_THROW( (errno != EAGAIN) && (errno != EWOULDBLOCK),
                       RAMP_ERR_EAGAIN );
        STL_TRY_THROW( errno != EBADF, RAMP_ERR_BADF );
        STL_THROW( RAMP_ERR_SENDMSG );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_EAGAIN )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_AGAIN,
                      "[stnSendMsg] OS returned EAGAIN or EWOULDBLOCK",
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    STL_CATCH( RAMP_ERR_BADF )
    {
        stlSnprintf( sMsg, 64, "OS returned errno(%d), descriptor(%d)", errno, aSock );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_FILE_BAD,
                      sMsg,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    STL_CATCH( RAMP_ERR_SENDMSG )
    {
        stlSnprintf( sMsg, 64, "OS returned errno(%d)", errno );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SENDMSG,
                      sMsg,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stnRecvMsg( stlSocket         aSock,
                      stlMsgHdr        *aMsgHdr,
                      stlInt32          aFlags,
                      stlErrorStack    *aErrorStack )
{
    stlSsize   sRetVal;
    stlChar    sMsg[64];

    do
    {
        sRetVal = recvmsg( aSock, aMsgHdr, aFlags );
    } while( (sRetVal == -1) && (errno == EINTR) );

    if(sRetVal == -1)
    {
        STL_TRY_THROW( (errno != EAGAIN) && (errno != EWOULDBLOCK),
                       RAMP_ERR_EAGAIN );
        STL_TRY_THROW( errno != EBADF, RAMP_ERR_BADF );
        STL_THROW( RAMP_ERR_RECVMSG );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_EAGAIN )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_AGAIN,
                      "[stnRecvMsg] OS returned EAGAIN or EWOULDBLOCK",
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    STL_CATCH( RAMP_ERR_BADF )
    {
        stlSnprintf( sMsg, 64, "OS returned errno(%d), descriptor(%d)", errno, aSock );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_FILE_BAD,
                      sMsg,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    STL_CATCH( RAMP_ERR_RECVMSG )
    {
        stlSnprintf( sMsg, 64, "OS returned errno(%d)", errno );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_RECVMSG,
                      sMsg,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    STL_FINISH;

    return STL_FAILURE;
}
