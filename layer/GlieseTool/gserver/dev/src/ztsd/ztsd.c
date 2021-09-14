/*******************************************************************************
 * ztsd.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztsd.c 12157 2014-04-23 03:50:31Z lym999 $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file ztsd.c
 * @brief Gliese gserver Dedicate Routines
 */

#include <stl.h>
#include <cml.h>
#include <knl.h>
#include <scl.h>
#include <scl.h>
#include <sml.h>
#include <ell.h>
#include <qll.h>
#include <ssl.h>
#include <sll.h>
#include <ztsDef.h>
#include <ztsm.h>

extern stlBool gZtsmWarmedUp;

/* listener로 fd를 요청하고 fd를 하나 받는다 */
stlStatus ztsdGetContext( stlContext    * aContext,
                          stlChar       * aToListenerPath,
                          stlUInt16     * aMajorVersion,
                          stlUInt16     * aMinorVersion,
                          stlUInt16     * aPatchVersion,
                          sllEnv        * aEnv )
{
    stlInt32            sState = 0;
    stlProc             sProc;
    stlChar             sUnixDomainPath[STL_MAX_FILE_PATH_LENGTH];

    stlContext          sUdsContext;  /* fd를 요청하고 받을 uds */
    stlSockAddr         sListenerAddr;
    cmlUdsForwardFd     sForwardFd;
    stlInt32            sSigFdNum;

    STL_TRY( stlGetCurrentProc( &sProc, KNL_ERROR_STACK(aEnv) ) == STL_SUCCESS );

    /* uds 생성을 위한 path를 설정 한다 */
    stlSnprintf( sUnixDomainPath,
                 STL_MAX_FILE_PATH_LENGTH,
                 "%s.%d",
                 aToListenerPath,
                 sProc.mProcID );

    /* fd를 요청하기 위한 uds를 생성한다 */
    if( stlInitializeContext( &sUdsContext,
                              STL_AF_UNIX,
                              STL_SOCK_DGRAM,
                              0,
                              STL_FALSE,
                              sUnixDomainPath,
                              0,
                              KNL_ERROR_STACK(aEnv) ) == STL_FAILURE )
    {
        (void) knlLogMsg( NULL,
                          KNL_ENV(aEnv),
                          KNL_LOG_LEVEL_INFO,
                          "[DEDICATE_SERVER] failed to create Unix Domain Socket [%s] \n",
                          sUnixDomainPath,
                          stlGetLastErrorCode( KNL_ERROR_STACK(aEnv) ) );

        STL_TRY( STL_FALSE );
    }

    sState = 1;

    stlMemset( &sListenerAddr, 0x00, STL_SIZEOF(sListenerAddr) );

    /* sListenerAddr에 listener의 address를 설정한다 */
    STL_TRY( stlSetSockAddr( &sListenerAddr,
                             STL_AF_UNIX,
                             NULL,
                             0,
                             aToListenerPath,
                             KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    /* listener에게 fd요청을 보낸다. */
    if( cmlSendRequestFdTo( &sUdsContext,
                            &sListenerAddr,
                            KNL_ERROR_STACK(aEnv) ) == STL_FAILURE )
    {
        knlLogMsg( NULL,
                   KNL_ENV(aEnv),
                   KNL_LOG_LEVEL_INFO,
                   "[DEDICATE_SERVER] failed to request FD \n" );

        STL_TRY( STL_FALSE );
    }

    /**
     * listener로 부터 fd를 받는다.
     * recv시 fd와 time, sequence를 같이 받는다.
     * stlRecvContext가 실패한 경우 nack도 보낼수 없음.
     */
    if( stlPoll( &sUdsContext.mPollFd,
                 1,
                 &sSigFdNum,
                 ZTS_RECV_FD_TIMEOUT,
                 KNL_ERROR_STACK(aEnv) ) == STL_FAILURE )
    {
        STL_TRY( stlGetLastErrorCode( KNL_ERROR_STACK(aEnv) ) == STL_ERRCODE_TIMEDOUT );

        knlLogMsg( NULL,
                   KNL_ENV(aEnv),
                   KNL_LOG_LEVEL_INFO,
                   "[DEDICATE_SERVER] failed to receive FD (timeout) \n" );

        STL_TRY( STL_FALSE );
    }

    STL_TRY( cmlRecvFdFrom( &sUdsContext,
                            aContext,
                            &sForwardFd,
                            KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );
    sState = 2;

    *aMajorVersion = sForwardFd.mMajorVersion;
    *aMinorVersion = sForwardFd.mMinorVersion;
    *aPatchVersion = sForwardFd.mPatchVersion;

    /* UDS context를 finalize한다 */
    sState = 0;
    STL_TRY( stlFinalizeContext( &sUdsContext, KNL_ERROR_STACK(aEnv) ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)stlFinalizeContext( aContext, KNL_ERROR_STACK(aEnv) );
        case 1:
            (void)stlFinalizeContext( &sUdsContext, KNL_ERROR_STACK(aEnv) );
            break;
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus ztsdProcessDedicate( sllHandle             * aHandle,
                               sclMemoryManager      * aMemoryMgr,
                               sllProtocolSentence   * aProtocolSentence,
                               sllEnv                * aEnv )
{
    stlInt32               sState = 0;
    cmlCommandType         sCommandType;
    stlBool                sIsConnected = STL_FALSE;
    stlBool                sIsTimeout = STL_FALSE;
    sclHandle            * sSclHandle = SCL_HANDLE(aHandle);
    sllCommandArgs         sCommandArgs;
    stlUInt32              sSessionId = SSL_INVALID_SESSION_ID;
    stlInt64               sSessionSeq;
    
    STL_TRY( knlGetSessionEnvId( KNL_SESS_ENV(aEnv),
                                 &sSessionId,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sSessionSeq = KNL_GET_SESSION_SEQ( SSL_SESS_ENV(aEnv) );

    aHandle->mSessionId = sSessionId;
    aHandle->mSessionSeq = sSessionSeq;
        
    while( STL_TRUE )
    {
        STL_INIT_ERROR_STACK( KNL_ERROR_STACK(aEnv) );
        
        if( sState == 1 )
        {
            sState = 0;
            sslLeaveSession( sSessionId, sSessionSeq );
        }
        
        STL_TRY( sclMoveNextProtocol( sSclHandle,
                                      ZTS_POLL_TIMEOUT,
                                      &sIsTimeout,
                                      &sCommandType,
                                      KNL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );

        if( sIsTimeout == STL_TRUE )
        {
            STL_TRY( knlExecuteProcessEvent( KNL_ENV(aEnv) ) == STL_SUCCESS );

            if( sIsConnected == STL_TRUE )
            {
                STL_TRY( sllCheckTimeout( aHandle,
                                          aEnv )
                         == STL_SUCCESS );
            }

            continue;
        }

        STL_TRY( sslEnterSession( sSessionId,
                                  sSessionSeq,
                                  KNL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );
        sState = 1;

        STL_ASSERT( sCommandType < CML_COMMAND_MAX );

        SLL_MARSHALLING_COMMAND_ARGS( &sCommandArgs, aHandle, NULL );
        
        /**
         * process protocol
         */
        STL_TRY( gSllCommandFunc[sCommandType]( &sCommandArgs,
                                                aEnv )
                 == STL_SUCCESS );

        /**
         * slpCmdConnect 실패시, slpCmdDisconnect 성공시 mSocketPeriod = STL_TRUE로 설정됨.
         * break하여 dedicate server종료
         */
        if( sSclHandle->mSocketPeriod == STL_TRUE )
        {
            STL_TRY( sclSendPacket( sSclHandle, SCL_ENV(aEnv) ) == STL_SUCCESS );

            break;
        }

        if( sCommandType == CML_COMMAND_CONNECT )
        {
            sIsConnected = STL_TRUE;
        }
        else if( sCommandType == CML_COMMAND_DISCONNECT )
        {
            sIsConnected = STL_FALSE;
        }
        else if( sCommandType == CML_COMMAND_PERIOD )
        {
            if( aProtocolSentence->mCloseDatabase == STL_TRUE )
            {
                (void) knlCleanupHeapMem( KNL_ENV(aEnv) );
                (void) knlDisconnectSession( KNL_SESS_ENV(aEnv) );
                break;
            }
        }
    }

    if( sState == 1 )
    {
        sslLeaveSession( sSessionId, sSessionSeq );
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        sslLeaveSession( sSessionId, sSessionSeq );
    }

    return STL_FAILURE;
}


stlStatus ztsdInitializeDedicate( sllHandle             * aHandle,
                                  sclMemoryManager      * aMemoryMgr,
                                  sllProtocolSentence   * aProtocolSentence,
                                  stlContext            * aContext,
                                  stlChar               * aToListenerPath,
                                  cmlCommandType        * aCommandType,
                                  stlErrorStack         * aErrorStack )
{
    stlInt32              sState = 0;
    stlInt32              sEnvState = 0;
    stlUInt16             sMajorVersion = -1;
    stlUInt16             sMinorVersion = -1;
    stlUInt16             sPatchVersion = -1;
    stlSockAddr           sClientAddr;
    stlInt64              sBufferSize = CML_BUFFER_SIZE;
    sclHandle           * sSclHandle = SCL_HANDLE(aHandle);
    sllEnv                sEnv;
    sllSessionEnv         sSessionEnv;

    if( gZtsmWarmedUp == STL_TRUE )
    {
        sBufferSize = knlGetPropertyBigIntValueAtShmByID( KNL_PROPERTY_NET_BUFFER_SIZE );
    }

    STL_TRY( sllInitializeEnv( &sEnv,
                               KNL_ENV_HEAP )
             == STL_SUCCESS );
    sEnvState = 1;

    STL_TRY( sllInitializeSessionEnv( &sSessionEnv,
                                      KNL_CONNECTION_TYPE_DA,
                                      KNL_SESSION_TYPE_DEDICATE,
                                      KNL_SESS_ENV_HEAP,
                                      &sEnv )
             == STL_SUCCESS );
    sEnvState = 2;

    KNL_LINK_SESS_ENV( &sEnv, &sSessionEnv );
    
    /* listener에 fd를 요청하고 fd를 받는다.  */
    STL_TRY( ztsdGetContext( aContext,
                             aToListenerPath,
                             &sMajorVersion,
                             &sMinorVersion,
                             &sPatchVersion,
                             &sEnv )
             == STL_SUCCESS );
    sEnvState = 3;

    STL_TRY( stlGetPeerName( STL_SOCKET_IN_CONTEXT(*aContext),
                             &sClientAddr,
                             KNL_ERROR_STACK(&sEnv) )
             == STL_SUCCESS );


    STL_TRY( sclInitializeMemoryManager( aMemoryMgr,
                                         sBufferSize,     /* PacketBufferSize */
                                         CML_BUFFER_SIZE, /* OperationBufferSize */
                                         CML_BUFFER_SIZE, /* PollingBufferSize */
                                         SCL_ENV(&sEnv) ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( sllInitializeHandle( aHandle,
                                  aContext,
                                  NULL,            /* ipc */
                                  &sClientAddr,
                                  NULL,            /* user */
                                  sMajorVersion,
                                  sMinorVersion,
                                  sPatchVersion,
                                  &sEnv )
             == STL_SUCCESS );
    sState = 2;

    STL_TRY( sllInitializeProtocolSentence( aHandle,
                                            aProtocolSentence,
                                            aMemoryMgr,
                                            &sEnv ) 
             == STL_SUCCESS );
    sState = 3;

    STL_TRY( sclHandshakingForServer( sSclHandle,
                                      aHandle->mRoleName,
                                      SCL_ENV(&sEnv) )
             == STL_SUCCESS );

    STL_TRY( sclRecvPacket( sSclHandle, SCL_ENV(&sEnv) ) == STL_SUCCESS );
    /**
     * startup command이면 여기서 처리한다.
     */
    STL_TRY( sclParseCommand( (sclProtocolSentence*)aProtocolSentence,
                              aCommandType,
                              KNL_ERROR_STACK(&sEnv) )
             == STL_SUCCESS );

    if( *aCommandType == CML_COMMAND_STARTUP )
    {
        STL_TRY( sllCmdStartUp( aHandle,
                                &sEnv )
                 == STL_SUCCESS );

        STL_TRY_THROW( sSclHandle->mSocketPeriod == STL_FALSE, RAMP_ERR_STARTUP_FAILURE );

        STL_TRY( ztsmWarmup( KNL_ERROR_STACK(&sEnv), CML_SESSION_DEDICATE ) == STL_SUCCESS );

        STL_TRY( sclSendPacket( sSclHandle,
                                SCL_ENV(&sEnv) )
                 == STL_SUCCESS );

        STL_TRY( sclRecvPacket( sSclHandle, SCL_ENV(&sEnv) ) == STL_SUCCESS );

        STL_TRY( sclParseCommand( (sclProtocolSentence*)aProtocolSentence,
                                  aCommandType,
                                  KNL_ERROR_STACK(&sEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY_THROW( gZtsmWarmedUp == STL_TRUE, RAMP_ERR_SERVER_IS_NOT_RUNNING );
    }

    sEnvState = 1;
    (void) sllFinalizeSessionEnv( &sSessionEnv, &sEnv );
    sEnvState = 0;
    (void) sllFinalizeEnv( &sEnv );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_STARTUP_FAILURE )
    {
        STL_TRY( sclSendPacket( sSclHandle,
                                SCL_ENV(&sEnv) )
                 == STL_SUCCESS );
    }

    STL_CATCH( RAMP_ERR_SERVER_IS_NOT_RUNNING )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SLL_ERRCODE_SERVER_IS_NOT_RUNNING,
                      NULL,
                      KNL_ERROR_STACK(&sEnv)  );

        (void) sclWriteErrorResult( sSclHandle,
                                    *aCommandType,
                                    KNL_ERROR_STACK(&sEnv),
                                    SCL_ENV(&sEnv) );
        STL_TRY( sclSendPacket( sSclHandle,
                                SCL_ENV(&sEnv) )
                 == STL_SUCCESS );
    }

    STL_FINISH;

    switch( sState )
    {
        case 4:
            (void)sllFinalizeProtocolSentence( aProtocolSentence, &sEnv );
        case 3:
            (void)sllFinalizeHandle( aHandle, &sEnv );
        case 2:
            (void)sclFinalizeMemoryManager( aMemoryMgr, SCL_ENV(&sEnv) );
        case 1:
            (void)stlFinalizeContext( aContext, KNL_ERROR_STACK(&sEnv) );
        default:
            break;
    }

    switch( sEnvState )
    {
        case 2:
            (void)sllFinalizeSessionEnv( &sSessionEnv, &sEnv );
        case 1:
            (void)sllFinalizeEnv( &sEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus ztsdFinalizeDedicate( sllHandle            * aHandle,
                                sclMemoryManager     * aMemoryMgr,
                                sllProtocolSentence  * aProtocolSentence,
                                stlContext           * aContext,
                                cmlCommandType         aCommandType,
                                stlErrorStack        * aErrorStack )
{
    stlInt32              sState = 0;
    sllEnv                sEnv;
    sllSessionEnv         sSessionEnv;
    sclHandle           * sSclHandle = SCL_HANDLE(aHandle);

    STL_TRY( sllInitializeEnv( &sEnv,
                               KNL_ENV_HEAP )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( sllInitializeSessionEnv( &sSessionEnv,
                                      KNL_CONNECTION_TYPE_DA,
                                      KNL_SESSION_TYPE_DEDICATE,
                                      KNL_SESS_ENV_HEAP,
                                      &sEnv )
             == STL_SUCCESS );
    sState = 2;

    KNL_LINK_SESS_ENV( &sEnv, &sSessionEnv );

    if( aCommandType != CML_COMMAND_MAX )
    {
        knlLogMsg( NULL,
                   KNL_ENV(&sEnv),
                   KNL_LOG_LEVEL_INFO,
                   "[DEDICATE_SERVER] %s : %s \n",
                   stlGetLastErrorMessage( aErrorStack ),
                   stlGetLastDetailErrorMessage( aErrorStack ) );

        /**
         * sclRecvPacket를 한 상태임으로 sclSendPacket를 한다.
         */
        (void) sclWriteErrorResult( sSclHandle,
                                    aCommandType,
                                    aErrorStack,
                                    SCL_ENV(&sEnv) );

        STL_TRY( sclSendPacket( sSclHandle,
                                SCL_ENV(&sEnv) )
                 == STL_SUCCESS );
    }

    (void)sllFinalizeHandle( aHandle, &sEnv );

    (void)sclFinalizeMemoryManager( aMemoryMgr, SCL_ENV(&sEnv) );

    (void)stlFinalizeContext( aContext, KNL_ERROR_STACK(&sEnv) );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)sllFinalizeSessionEnv( &sSessionEnv, &sEnv );
        case 1:
            stlAppendErrors( aErrorStack, KNL_ERROR_STACK(&sEnv) );
            (void)sllFinalizeEnv( &sEnv );
        default:
            break;
    }

    return STL_FAILURE;
}
