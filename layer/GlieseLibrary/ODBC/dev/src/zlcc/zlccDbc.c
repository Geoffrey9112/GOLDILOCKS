/*******************************************************************************
 * zlccDbc.c
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

#include <cml.h>
#include <goldilocks.h>

#include <zlDef.h>
#include <zle.h>
#include <zld.h>

#include <zlcc.h>

/**
 * @file zlccDbc.c
 * @brief ODBC API Internal Connect Routines.
 */

/**
 * @addtogroup zlccDbc
 * @{
 */

stlStatus zlccConnect( zlcDbc        * aDbc,
                       stlChar       * aHost,
                       stlInt32        aPort,
                       stlChar       * aCsMode,
                       stlChar       * aUser,
                       stlChar       * aEncryptedPassword,
                       stlChar       * aRole,
                       stlChar       * aOldPassword,
                       stlChar       * aNewPassword,
                       stlBool         aIsStartup,
                       stlProc         aProc,
                       stlChar       * aProgram,
                       stlBool       * aSuccessWithInfo,
                       stlErrorStack * aErrorStack )
{
    cmlHandle    * sHandle = NULL;
    stlContext   * sContext = NULL;
    stlInt32       sState = 0;
    stlInterval    sTimeout;
    stlErrorData * sErrorData = NULL;
    cmlSessionType sCsMode = CML_SESSION_NONE;
    stlInt32       sBufferSize;

    STL_TRY_THROW( aHost[0] != '\0', RAMP_ERR_CLIENT_UNABLE_TO_ESTABLISH_CONNECTION );
    STL_TRY_THROW( aPort != 0, RAMP_ERR_CLIENT_UNABLE_TO_ESTABLISH_CONNECTION );
    

    if( aCsMode == NULL || aCsMode[0] == 0x00 )
    {
        /* CML_SESSION_DEFAULT로 설정되면 listener에서 판단함. */
        sCsMode = CML_SESSION_DEFAULT;
    }
    else if( stlStrncasecmp( aCsMode, "dedicated", STL_MAX_INI_PROPERTY_NAME ) == 0 )
    {
        sCsMode = CML_SESSION_DEDICATE;
    }
    else if( stlStrncasecmp( aCsMode, "shared", STL_MAX_INI_PROPERTY_NAME ) == 0 )
    {
        sCsMode = CML_SESSION_SHARED;
    }
    else if( stlStrncasecmp( aCsMode, "default", STL_MAX_INI_PROPERTY_NAME ) == 0 )
    {
        sCsMode = CML_SESSION_DEFAULT;
    }
    else
    {
        STL_THROW( RAMP_ERR_INVALID_CS_MODE );
    }

    /*
     * Context 초기화
     */

    sHandle  = &aDbc->mComm;
    sContext = &aDbc->mContext;

    STL_TRY_THROW( stlInitializeContext( sContext,
                                         STL_AF_INET,
                                         STL_SOCK_STREAM,
                                         0,
                                         STL_FALSE,
                                         aHost,
                                         aPort,
                                         aErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_CLIENT_UNABLE_TO_ESTABLISH_CONNECTION );
    sState = 1;

    if( aDbc->mAttrLoginTimeout == 0 )
    {
        sTimeout = STL_INFINITE_TIME;
    }
    else
    {
        sTimeout = aDbc->mAttrLoginTimeout;
    }

    if( stlConnectContext( sContext,
                           sTimeout,
                           aErrorStack ) != STL_SUCCESS )
    {
        sErrorData = stlGetLastErrorData( aErrorStack );

        STL_DASSERT( sErrorData != NULL );

        if( sErrorData->mErrorCode == STL_ERRCODE_TIMEDOUT )
        {
            STL_THROW( RAMP_ERR_TIMEOUT_EXPIRED );
        }
        else
        {
            STL_THROW( RAMP_ERR_CLIENT_UNABLE_TO_ESTABLISH_CONNECTION);
        }
    }
    
    STL_TRY( cmlInitSockOption( sContext,
                                0,
                                aErrorStack )
             == STL_SUCCESS );


    /* listener에게 session info 보냄.   */
    STL_TRY( zlccPreHandshake( aDbc,
                               sCsMode,
                               aErrorStack )
             == STL_SUCCESS );

    /*
     * Communication 초기화
     */

    STL_TRY( zlccHandshake( aDbc,
                            aRole,
                            &sBufferSize,
                            aErrorStack ) == STL_SUCCESS );

    if( aDbc->mMemoryMgr.mPacketBufferSize != sBufferSize )
    {
        STL_TRY( cmlFinalizeMemoryManager( &aDbc->mMemoryMgr,
                                           aErrorStack )
                 == STL_SUCCESS );

        STL_TRY( cmlInitializeMemoryManager( &aDbc->mMemoryMgr,
                                             sBufferSize,       /* PacketBufferSize */
                                             CML_BUFFER_SIZE,   /* OperationBufferSize */
                                             aErrorStack ) == STL_SUCCESS );

        /* MemoryManager를 초기화 했음으로 다시 ProtocolSentence를 초기화 해야 함 */
        STL_TRY( cmlInitializeProtocolSentence( &aDbc->mComm,
                                                &aDbc->mProtocolSentence,
                                                &aDbc->mMemoryMgr,
                                                aErrorStack ) == STL_SUCCESS );
    }


    /*
     * Connect Protocol
     */

    if( aIsStartup == STL_TRUE )
    {
        STL_TRY( cmlWriteStartupCommand( sHandle,
                                         CML_CONTROL_HEAD | CML_CONTROL_TAIL, 
                                         aErrorStack ) == STL_SUCCESS );

        STL_TRY( cmlSendPacket( sHandle,
                                aErrorStack ) == STL_SUCCESS );

        STL_TRY( cmlReadStartupResult( sHandle,
                                       aErrorStack ) == STL_SUCCESS );
    }

    STL_TRY( cmlWriteConnectCommand( sHandle,
                                     CML_CONTROL_HEAD | CML_CONTROL_TAIL,
                                     aUser,
                                     aEncryptedPassword,
                                     aOldPassword,
                                     aNewPassword,
                                     aProgram,
                                     aProc.mProcID,
                                     aDbc->mCharacterSet,
                                     aDbc->mTimezone,
                                     CML_DBC_ODBC,
                                     aErrorStack ) == STL_SUCCESS );

    STL_TRY( cmlSendPacket( sHandle,
                            aErrorStack ) == STL_SUCCESS );

    STL_TRY( cmlReadConnectResult( sHandle,
                                   &aDbc->mSessionId,
                                   &aDbc->mSessionSeq,
                                   &aDbc->mNlsCharacterSet,
                                   &aDbc->mNlsNCharCharacterSet,
                                   &aDbc->mDBTimezone,
                                   aErrorStack ) == STL_SUCCESS );

    /* SUCCESS_WITH_INFO */
    if( STL_HAS_ERROR( aErrorStack ) == STL_TRUE )
    {
        zldDiagAdds( SQL_HANDLE_DBC,
                     (void*)aDbc,
                     SQL_NO_ROW_NUMBER,
                     SQL_NO_COLUMN_NUMBER,
                     aErrorStack );

        *aSuccessWithInfo = STL_TRUE;
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_TIMEOUT_EXPIRED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_TIMEOUT_EXPIRED,
                      "The timeout period expired before the connection to the data source "
                      "completed. The timeout period is set through SQLSetConnectAttr, "
                      "SQL_ATTR_LOGIN_TIMEOUT.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_CLIENT_UNABLE_TO_ESTABLISH_CONNECTION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_CLIENT_UNABLE_TO_ESTABLISH_CONNECTION,
                      NULL,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_INVALID_CS_MODE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_ODBC_INI,
                      NULL,
                      aErrorStack,
                      "CS_MODE");
    }

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void)stlFinalizeContext( sContext, aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus zlccDisconnect( zlcDbc        * aDbc,
                          stlErrorStack * aErrorStack )
{
    cmlHandle * sHandle;

    sHandle = &aDbc->mComm;
    
    /*
     * Disconnect Protocol
     */

    STL_TRY( cmlWriteDisconnectCommand( sHandle,
                                        aErrorStack ) == STL_SUCCESS );

    STL_TRY( cmlSendPacket( sHandle, aErrorStack ) == STL_SUCCESS );

    (void)cmlReadDisconnectResult( sHandle, aErrorStack );

    return STL_SUCCESS;

    STL_FINISH;

    STL_ASSERT( 0 );

    return STL_FAILURE;
}

stlStatus zlccSetAttr( zlcDbc        * aDbc,
                       stlInt32        aAttr,
                       stlInt32        aIntValue,
                       stlChar       * aStrValue,
                       stlErrorStack * aErrorStack )
{
    cmlHandle * sHandle = NULL;
    stlBool     sIgnored = STL_FALSE;

    sHandle = &aDbc->mComm;
    
    switch( aAttr )
    {
        case SQL_ATTR_TXN_ISOLATION :
            STL_TRY( cmlWriteSetConnAttrCommand( sHandle,
                                                 CML_CONTROL_HEAD | CML_CONTROL_TAIL,
                                                 ZLL_CONNECTION_ATTR_TXN_ISOLATION,
                                                 DTL_TYPE_NATIVE_INTEGER,
                                                 &aIntValue,
                                                 0,
                                                 aErrorStack ) == STL_SUCCESS );

            STL_TRY( cmlSendPacket( sHandle,
                                    aErrorStack ) == STL_SUCCESS );

            STL_TRY( cmlReadSetConnAttrResult( sHandle,
                                               &sIgnored,
                                               aErrorStack ) == STL_SUCCESS );
            break;
        case SQL_ATTR_TIMEZONE:
            STL_TRY( cmlWriteSetConnAttrCommand( sHandle,
                                                 CML_CONTROL_HEAD | CML_CONTROL_TAIL,
                                                 ZLL_CONNECTION_ATTR_TIMEZONE,
                                                 DTL_TYPE_NATIVE_INTEGER,
                                                 &aIntValue,
                                                 0,
                                                 aErrorStack ) == STL_SUCCESS );

            STL_TRY( cmlSendPacket( sHandle,
                                    aErrorStack ) == STL_SUCCESS );

            STL_TRY( cmlReadSetConnAttrResult( sHandle,
                                               &sIgnored,
                                               aErrorStack ) == STL_SUCCESS );
            break;
        default :
            STL_ASSERT( STL_FALSE );
            break;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief listener와 PreHandshake를 한다
 */
stlStatus zlccPreHandshake( zlcDbc         * aDbc,
                            cmlSessionType   aCsMode,
                            stlErrorStack  * aErrorStack )
{
    cmlHandle * sHandle = NULL;

    sHandle = &aDbc->mComm;

    STL_TRY( cmlWriteVersion( sHandle,
                              aErrorStack )
             == STL_SUCCESS );

    STL_TRY( cmlSendVersion( sHandle,
                             aErrorStack )
             == STL_SUCCESS );

    STL_TRY( cmlWritePreHandshakeCommand( sHandle,
                                          aCsMode,
                                          aErrorStack )
             == STL_SUCCESS );

    /**
     * SQLConnect가 실패하여 SQLConnect를 다시 호출하는 경우도 있음으로
     * 여기서 sequence를 초기화 한다.
     */
    sHandle->mSendSequence = 0;
    sHandle->mRecvSequence = 0;

    STL_TRY( cmlSendPacket( sHandle,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( cmlRecvPacket( sHandle,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( cmlReadPreHandshakeResult( sHandle,
                                        aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Protocol Handshaking
 */
stlStatus zlccHandshake( zlcDbc        * aDbc,
                         stlChar       * aRoleName,
                         stlInt32      * aBufferSize,
                         stlErrorStack * aErrorStack )
{
    cmlHandle * sHandle = NULL;

    sHandle = &aDbc->mComm;

    STL_TRY( cmlWriteHandshakeCommand( sHandle,
                                       aRoleName,
                                       aErrorStack )
             == STL_SUCCESS );

    STL_TRY( cmlSendPacket( sHandle,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( cmlRecvPacket( sHandle,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( cmlReadHandshakeResult( sHandle,
                                     aBufferSize,
                                     aErrorStack )
             == STL_SUCCESS );

#if 0   /* TODO : 성능 test 필요함 */
    STL_TRY( stlSetSocketOption( STL_SOCKET_IN_CONTEXT(*sHandle->mCommunication.mContext),
                                 STL_SOPT_SO_SNDBUF,
                                 sBufferSize,
                                 aErrorStack )
             == STL_SUCCESS );

    STL_TRY( stlSetSocketOption( STL_SOCKET_IN_CONTEXT(*sHandle->mCommunication.mContext),
                                 STL_SOPT_SO_RCVBUF,
                                 sBufferSize,
                                 aErrorStack )
             == STL_SUCCESS );
#endif

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
