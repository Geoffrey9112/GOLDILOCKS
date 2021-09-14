/*******************************************************************************
 * zlacDbc.c
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

#include <goldilocks.h>
#include <cml.h>
#include <scl.h>
#include <sll.h>
#include <zlDef.h>
#include <zle.h>
#include <zld.h>

#include <zlab.h>

/**
 * @file zlacDbc.c
 * @brief ODBC API Internal Connect Routines.
 */

/**
 * @addtogroup zlacDbc
 * @{
 */

stlStatus zlacConnect( zlcDbc        * aDbc,
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
    sslEnv        * sEnv;
    sslSessionEnv * sSessionEnv;
    stlInt32        sState = 0;
    stlInt32        sSignalState = 0;
    
    STL_TRY( zlabWarmup( aErrorStack ) == STL_SUCCESS );

    /**
     * Thread Signal Lock을 획득한다.
     * - Env를 할당하는 동안 Singal로 인한 비정상적 프로세스 종료를 막는다.
     */
    STL_TRY( knlLockAndBlockSignal( aErrorStack ) == STL_SUCCESS );
    sSignalState = 1;

    /**
     * Signal에 의해서 이미 죽은 프로세스는 새로운 세션을 생성할수 없다.
     */
    STL_TRY( knlValidateProcessSar( KNL_SAR_SESSION,
                                    aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( sslAllocEnv( &sEnv,
                          aErrorStack ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( sslInitializeEnv( sEnv,
                               KNL_ENV_SHARED )
             == STL_SUCCESS );
    sState = 2;

    knlSetTopLayer( STL_LAYER_GLIESE_LIBRARY, KNL_ENV( sEnv ) );
    
    STL_TRY( sslAllocSession( aRole,
                              &sSessionEnv,
                              sEnv ) == STL_SUCCESS );
    sState = 3;

    KNL_LINK_SESS_ENV( sEnv, sSessionEnv );

    STL_TRY( sslInitSessionEnv( sSessionEnv,
                                KNL_CONNECTION_TYPE_DA,
                                KNL_SESSION_TYPE_DEDICATE,
                                KNL_SESS_ENV_SHARED,
                                sEnv ) == STL_SUCCESS );
    sState = 4;

    knlSetSessionTopLayer( (knlSessionEnv*)sSessionEnv, STL_LAYER_GLIESE_LIBRARY );

    STL_TRY( knlGetSessionEnvId( sSessionEnv,
                                 &aDbc->mSessionId,
                                 KNL_ENV(sEnv) )
             == STL_SUCCESS );

    aDbc->mSessionSeq = KNL_GET_SESSION_SEQ( sSessionEnv );
    
    /**
     * 해당 라인 이전 코드에서 SIGV가 발생하지 말도록 해야 한다.
     * - Cleanup이 수행되지 않을수 있다.
     */
    KNL_ENABLE_SESSION_AGING( sSessionEnv );
    sState = 5;

    STL_TRY( sslEnterSession( aDbc->mSessionId,
                              aDbc->mSessionSeq,
                              KNL_ERROR_STACK(sEnv) )
             == STL_SUCCESS );
    sState = 6;

    STL_TRY( sslConnect( aUser,
                         aEncryptedPassword,
                         aRole,
                         aOldPassword,
                         aNewPassword,
                         aProc,
                         aProgram,
                         NULL,   /* aAddress */
                         0,      /* aPort */
                         aDbc->mCharacterSet,
                         aDbc->mTimezone,
                         &aDbc->mNlsCharacterSet,
                         &aDbc->mNlsNCharCharacterSet,
                         &aDbc->mDBTimezone,
                         sEnv )
             == STL_SUCCESS );

    /* SUCCESS_WITH_INFO */
    if( STL_HAS_ERROR( KNL_ERROR_STACK(sEnv) ) == STL_TRUE )
    {
        zldDiagAdds( SQL_HANDLE_DBC,
                     (void*)aDbc,
                     SQL_NO_ROW_NUMBER,
                     SQL_NO_COLUMN_NUMBER,
                     KNL_ERROR_STACK(sEnv) );

        *aSuccessWithInfo = STL_TRUE;
    }    

    sSignalState = 0;
    STL_TRY( knlUnlockAndUnblockSignal( aErrorStack ) == STL_SUCCESS );

    sslLeaveSession( aDbc->mSessionId, aDbc->mSessionSeq );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 6:
            sslLeaveSession( aDbc->mSessionId, aDbc->mSessionSeq );
        case 5:
            stlAppendErrors( aErrorStack, KNL_ERROR_STACK( sEnv ) );
            STL_INIT_ERROR_STACK( KNL_ERROR_STACK( sEnv ) );
            (void) sslFiniSessionEnv( sSessionEnv, sEnv );
            (void) sslFinalizeEnv( sEnv );
            knlDisconnectSession( sSessionEnv );
            break;
        case 4:
            (void) sslFiniSessionEnv( sSessionEnv, sEnv );
        case 3:
            (void) sslFreeSession( sSessionEnv, sEnv );
        case 2:
            stlAppendErrors( aErrorStack, KNL_ERROR_STACK( sEnv ) );
            STL_INIT_ERROR_STACK( KNL_ERROR_STACK( sEnv ) );
            (void) sslFinalizeEnv( sEnv );
        case 1:
            (void) sslFreeEnv( sEnv, aErrorStack );
        default:
            break;
    }

    if( sSignalState == 1 )
    {
        (void) knlUnlockAndUnblockSignal( KNL_ERROR_STACK(sEnv) );
    }

    return STL_FAILURE;
}

stlStatus zlacDisconnect( zlcDbc        * aDbc,
                          stlErrorStack * aErrorStack )
{
    stlInt32        sState = 0;
    sslEnv        * sEnv = NULL;
    sslSessionEnv * sSessionEnv = NULL;
    
    STL_TRY( sslEnterSession( aDbc->mSessionId,
                              aDbc->mSessionSeq,
                              aErrorStack )
             == STL_SUCCESS );
    sState = 1;

    sSessionEnv = knlGetSessionEnv( aDbc->mSessionId );
    sEnv = SSL_WORKER_ENV( sSessionEnv );
    
    (void) sslDisconnect( sEnv );
    (void) sslFiniSessionEnv( sSessionEnv, sEnv );
    (void) sslFinalizeEnv( sEnv );

    sState = 0;
    sslLeaveSession( aDbc->mSessionId, aDbc->mSessionSeq );

    knlDisconnectSession( sSessionEnv );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            knlKillSession( sSessionEnv );
            stlAppendErrors( aErrorStack, KNL_ERROR_STACK( sEnv ) );
            STL_INIT_ERROR_STACK( KNL_ERROR_STACK( sEnv ) );
            sslLeaveSession( aDbc->mSessionId, aDbc->mSessionSeq );
        default:
            break;
    }

    return STL_SUCCESS;
}

stlStatus zlacSetAttr( zlcDbc        * aDbc,
                       stlInt32        aAttr,
                       stlInt32        aIntValue,
                       stlChar       * aStrValue,
                       stlErrorStack * aErrorStack )
{
    stlInt32   sState = 0;
    sslEnv   * sEnv;
    
    STL_TRY( sslEnterSession( aDbc->mSessionId,
                              aDbc->mSessionSeq,
                              aErrorStack )
             == STL_SUCCESS );
    sState = 1;

    sEnv = SSL_WORKER_ENV( knlGetSessionEnv(aDbc->mSessionId) );
    
    switch( aAttr )
    {
        case SQL_ATTR_TXN_ISOLATION :
            STL_TRY( sslSetSessionAttr( ZLL_CONNECTION_ATTR_TXN_ISOLATION,
                                        aIntValue,
                                        NULL,
                                        0,
                                        sEnv ) == STL_SUCCESS );
            break;
        case SQL_ATTR_TIMEZONE:
            STL_TRY( sslSetSessionAttr( ZLL_CONNECTION_ATTR_TIMEZONE,
                                        aIntValue,
                                        NULL,
                                        0,
                                        sEnv ) == STL_SUCCESS );
            break;
        default :
            STL_ASSERT( STL_FALSE );
            break;
    }

    sState = 0;
    sslLeaveSession( aDbc->mSessionId,
                     aDbc->mSessionSeq );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            stlAppendErrors( aErrorStack, KNL_ERROR_STACK( sEnv ) );
            STL_INIT_ERROR_STACK( KNL_ERROR_STACK( sEnv ) );
            sslLeaveSession( aDbc->mSessionId, aDbc->mSessionSeq );
        default:
            break;
    }

    return STL_FAILURE;
}

/** @} */
