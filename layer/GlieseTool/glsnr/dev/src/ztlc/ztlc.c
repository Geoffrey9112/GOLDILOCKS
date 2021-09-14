/*******************************************************************************
 * ztlc.c
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

/**
 * @file ztlc.c
 * @brief Gliese Listener communication routines
 */

#include <stl.h>
#include <ztlDef.h>
#include <ztlc.h>
#include <ztlm.h>

extern stlLogger gZtlLogger;

/**
 * @brief Gliese Listener communication routines
 */

stlStatus ztlcRequestQuit( stlErrorStack   * aErrorStack )
{
    stlChar             sUnixDomainPath[STL_MAX_FILE_PATH_LENGTH];
    stlProc             sProc;
    stlInt32            sState = 0;
    stlContext          sContext;
    stlSockAddr         sListenerSocketAddr;
    stlChar           * sMessage = NULL;

    STL_TRY( stlGetCurrentProc( &sProc, aErrorStack ) == STL_SUCCESS );
    stlSnprintf( sUnixDomainPath,
                 STL_MAX_FILE_PATH_LENGTH,
                 "%s.%d",
                 gZtlUdsPath,
                 sProc.mProcID );

    if( stlInitializeContext( &sContext,
                              STL_AF_UNIX,
                              STL_SOCK_DGRAM,
                              0,
                              STL_FALSE,
                              sUnixDomainPath,
                              0,
                              aErrorStack ) == STL_FAILURE )
    {
        stlPrintf( "[LISTENER] Unix Domain Socket [%s] Create failure(%d) \n" );

        STL_TRY( STL_FALSE );
    }
    sState = 1;

    stlMemset( &sListenerSocketAddr, 0x00, STL_SIZEOF(sListenerSocketAddr) );
    STL_TRY( stlSetSockAddr( &sListenerSocketAddr,
                             STL_AF_UNIX,
                             NULL,
                             0,
                             gZtlUdsPath,
                             aErrorStack ) == STL_SUCCESS );

    if( cmlSendRequestQuitTo( &sContext,
                              &sListenerSocketAddr,
                              aErrorStack ) == STL_SUCCESS )
    {
        sMessage = "Listener is stopped.\n";
    }
    else
    {
        (void)stlPopError( aErrorStack );

        sMessage = "Listener is not running.\n";
    }
    ztlmPrintf( sMessage );

    sState = 0;
    STL_TRY( stlFinalizeContext( &sContext, aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void)stlFinalizeContext( &sContext, aErrorStack );
            break;
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus ztlcGetStatus( stlBool           aPrintStatus,
                         stlBool         * aRunning,
                         stlErrorStack   * aErrorStack )
{
    stlChar                 sUnixDomainPath[STL_MAX_FILE_PATH_LENGTH];
    stlProc                 sProc;
    stlInt32                sState = 0;
    stlContext              sContext;
    stlSockAddr             sListenerSocketAddr;
    cmlUdsPacket            sUdsPacket;
    cmlUdsResponseStatus  * sResponseStatus;
    stlChar               * sMessage = NULL;

    STL_TRY( stlGetCurrentProc( &sProc, aErrorStack ) == STL_SUCCESS );
    stlSnprintf( sUnixDomainPath,
                 STL_MAX_FILE_PATH_LENGTH,
                 "%s.%d",
                 gZtlUdsPath,
                 sProc.mProcID );

    if( stlInitializeContext( &sContext,
                              STL_AF_UNIX,
                              STL_SOCK_DGRAM,
                              0,
                              STL_FALSE,
                              sUnixDomainPath,
                              0,
                              aErrorStack ) == STL_FAILURE )
    {
        stlPrintf( "[LISTENER] Unix Domain Socket [%s] Create failure(%d) \n",
                   sUnixDomainPath,
                   stlGetLastErrorCode( aErrorStack ) );

        STL_TRY( STL_FALSE );
    }
    sState = 1;

    stlMemset( &sListenerSocketAddr, 0x00, STL_SIZEOF(sListenerSocketAddr) );
    STL_TRY( stlSetSockAddr( &sListenerSocketAddr,
                             STL_AF_UNIX,
                             NULL,
                             0,
                             gZtlUdsPath,
                             aErrorStack ) == STL_SUCCESS );

    if( cmlSendRequestStatusTo( &sContext,
                                &sListenerSocketAddr,
                                aErrorStack ) == STL_SUCCESS )
    {
        if( cmlRecvFrom( &sContext,
                         &sListenerSocketAddr,
                         &sUdsPacket,
                         aErrorStack ) == STL_SUCCESS )
        {
            sResponseStatus = &sUdsPacket.mCmd.mResponseStatus;

            sMessage = sResponseStatus->mData;

            if( aRunning != NULL )
            {
                *aRunning = STL_TRUE;
            }
        }
        else
        {
            (void)stlPopError( aErrorStack );

            sMessage = "Listener is not running.\n";

            if( aRunning != NULL )
            {
                *aRunning = STL_FALSE;
            }
        }
    }
    else
    {
        (void)stlPopError( aErrorStack );

        sMessage = "Listener is not running.\n";

        if( aRunning != NULL )
        {
            *aRunning = STL_FALSE;
        }
    }

    if( aPrintStatus == STL_TRUE )
    {
        ztlmPrintf( sMessage );
    }


    sState = 0;
    STL_TRY( stlFinalizeContext( &sContext, aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void)stlFinalizeContext( &sContext, aErrorStack );
            break;
        default:
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief 상태 조회 요청(CML_COMMAND_LISTENER_STATUS)이 오면
 *        현재 상태를 보낸다.
 */
stlStatus ztlcSendStatusInfo( ztlcContext   * aContext,
                              stlSockAddr   * aAddr,
                              stlErrorStack * aErrorStack )
{
    stlChar                 sData[CML_STATUS_INFO_LENGTH];

    stlSnprintf( sData, CML_STATUS_INFO_LENGTH,
                 "Listener process ID : %d\n"
                 "Listener configuration file : %s\n"
                 "Unix Domain Path : %s\n"
                 "TCP Listen Host : %s, Port : %d\n"
                 "default C/S mode : %s\n"
                 "\nListener is running.\n",
                 gZtlProcess.mProcID,
                 gZtlConfFilePath,
                 gZtlUdsPath,
                 gZtlConfig.mTcpHost,
                 gZtlConfig.mTcpPort,
                 (gZtlConfig.mDefaultCsMode==CML_SESSION_DEDICATE)?"Dedicated":"Shared" );

    STL_TRY(cmlSendStatusTo( &aContext->mStlContext,
                             aAddr,
                             sData,
                             aErrorStack )
            == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztlcGetUnixDomainPath( stlChar       * aUdsPath,
                                 stlSize         aSize,
                                 stlChar       * aName,
                                 stlInt64        aKey,
                                 stlErrorStack * aErrorStack )
{
    stlSnprintf( aUdsPath, aSize,
                 "%stmp%s%s-%s.%ld",
                 STL_PATH_SEPARATOR,
                 STL_PATH_SEPARATOR,
                 "goldilocks-unix",
                 aName,
                 aKey );

    return STL_SUCCESS;
}

