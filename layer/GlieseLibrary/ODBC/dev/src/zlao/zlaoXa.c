/*******************************************************************************
 * zlaoXa.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zlaoXa.c 12017 2014-04-15 05:44:19Z mae113 $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <goldilocks.h>
#include <cml.h>
#include <ssl.h>
#include <zlDef.h>
#include <zle.h>
#include <zlc.h>

/**
 * @file zlaoXa.c
 * @brief ODBC API Internal XA Routines.
 */

/**
 * @addtogroup zloXa
 * @{
 */

stlStatus zlaoClose( zlcDbc        * aDbc,
                     stlInt32      * aReturn,
                     stlErrorStack * aErrorStack )
{
    stlInt32   sState = 0;
    sslEnv   * sEnv;
    
    sEnv = SSL_WORKER_ENV( knlGetSessionEnv(aDbc->mSessionId) );
    
    STL_TRY( sslEnterSession( aDbc->mSessionId,
                              aDbc->mSessionSeq,
                              aErrorStack )
             == STL_SUCCESS );
    sState = 1;

    *aReturn = sslXaClose( sEnv );

    STL_TRY( *aReturn == SSL_XA_OK );
    
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

stlStatus zlaoStart( zlcDbc        * aDbc,
                     stlXid        * aXid,
                     stlInt32        aRmId,
                     stlInt32        aFlags,
                     stlInt32      * aReturn,
                     stlErrorStack * aErrorStack )
{
    stlInt32   sState = 0;
    sslEnv   * sEnv;
    
    sEnv = SSL_WORKER_ENV( knlGetSessionEnv(aDbc->mSessionId) );
    
    STL_TRY( sslEnterSession( aDbc->mSessionId,
                              aDbc->mSessionSeq,
                              aErrorStack )
             == STL_SUCCESS );
    sState = 1;

    *aReturn = sslXaStart( aXid,
                           aRmId,
                           (stlInt64)aFlags,
                           sEnv );

    if( STL_HAS_ERROR( KNL_ERROR_STACK(sEnv) ) == STL_TRUE )
    {
        stlAppendErrors( aErrorStack, KNL_ERROR_STACK( sEnv ) );
        STL_INIT_ERROR_STACK( KNL_ERROR_STACK( sEnv ) );
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

stlStatus zlaoEnd( zlcDbc        * aDbc,
                   stlXid        * aXid,
                   stlInt32        aRmId,
                   stlInt32        aFlags,
                   stlInt32      * aReturn,
                   stlErrorStack * aErrorStack )
{
    stlInt32   sState = 0;
    sslEnv   * sEnv;
    
    sEnv = SSL_WORKER_ENV( knlGetSessionEnv(aDbc->mSessionId) );
    
    STL_TRY( sslEnterSession( aDbc->mSessionId,
                              aDbc->mSessionSeq,
                              aErrorStack )
             == STL_SUCCESS );
    sState = 1;

    *aReturn = sslXaEnd( aXid,
                         aRmId,
                         (stlInt64)aFlags,
                         sEnv );

    if( STL_HAS_ERROR( KNL_ERROR_STACK(sEnv) ) == STL_TRUE )
    {
        stlAppendErrors( aErrorStack, KNL_ERROR_STACK( sEnv ) );
        STL_INIT_ERROR_STACK( KNL_ERROR_STACK( sEnv ) );
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

stlStatus zlaoRollback( zlcDbc        * aDbc,
                        stlXid        * aXid,
                        stlInt32        aRmId,
                        stlInt32        aFlags,
                        stlInt32      * aReturn,
                        stlErrorStack * aErrorStack )
{
    stlInt32   sState = 0;
    sslEnv   * sEnv;
    
    sEnv = SSL_WORKER_ENV( knlGetSessionEnv(aDbc->mSessionId) );
    
    STL_TRY( sslEnterSession( aDbc->mSessionId,
                              aDbc->mSessionSeq,
                              aErrorStack )
             == STL_SUCCESS );
    sState = 1;

    *aReturn = sslXaRollback( aXid,
                              aRmId,
                              (stlInt64)aFlags,
                              sEnv );

    if( STL_HAS_ERROR( KNL_ERROR_STACK(sEnv) ) == STL_TRUE )
    {
        stlAppendErrors( aErrorStack, KNL_ERROR_STACK( sEnv ) );
        STL_INIT_ERROR_STACK( KNL_ERROR_STACK( sEnv ) );
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

stlStatus zlaoPrepare( zlcDbc        * aDbc,
                       stlXid        * aXid,
                       stlInt32        aRmId,
                       stlInt32        aFlags,
                       stlInt32      * aReturn,
                       stlErrorStack * aErrorStack )
{
    stlInt32   sState = 0;
    sslEnv   * sEnv;
    
    sEnv = SSL_WORKER_ENV( knlGetSessionEnv(aDbc->mSessionId) );
    
    STL_TRY( sslEnterSession( aDbc->mSessionId,
                              aDbc->mSessionSeq,
                              aErrorStack )
             == STL_SUCCESS );
    sState = 1;

    *aReturn = sslXaPrepare( aXid,
                             aRmId,
                             (stlInt64)aFlags,
                             sEnv );

    if( STL_HAS_ERROR( KNL_ERROR_STACK(sEnv) ) == STL_TRUE )
    {
        stlAppendErrors( aErrorStack, KNL_ERROR_STACK( sEnv ) );
        STL_INIT_ERROR_STACK( KNL_ERROR_STACK( sEnv ) );
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

stlStatus zlaoCommit( zlcDbc        * aDbc,
                      stlXid        * aXid,
                      stlInt32        aRmId,
                      stlInt32        aFlags,
                      stlInt32      * aReturn,
                      stlErrorStack * aErrorStack )
{
    stlInt32   sState = 0;
    sslEnv   * sEnv;
    
    sEnv = SSL_WORKER_ENV( knlGetSessionEnv(aDbc->mSessionId) );
    
    STL_TRY( sslEnterSession( aDbc->mSessionId,
                              aDbc->mSessionSeq,
                              aErrorStack )
             == STL_SUCCESS );
    sState = 1;

    *aReturn = sslXaCommit( aXid,
                            aRmId,
                            (stlInt64)aFlags,
                            sEnv );

    if( STL_HAS_ERROR( KNL_ERROR_STACK(sEnv) ) == STL_TRUE )
    {
        stlAppendErrors( aErrorStack, KNL_ERROR_STACK( sEnv ) );
        STL_INIT_ERROR_STACK( KNL_ERROR_STACK( sEnv ) );
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

stlStatus zlaoRecover( zlcDbc        * aDbc,
                       stlXid        * aXids,
                       stlInt32        aCount,
                       stlInt32        aRmId,
                       stlInt32        aFlags,
                       stlInt32      * aReturn,
                       stlErrorStack * aErrorStack )
{
    stlInt32   sState = 0;
    sslEnv   * sEnv;
    
    sEnv = SSL_WORKER_ENV( knlGetSessionEnv(aDbc->mSessionId) );
    
    STL_TRY( sslEnterSession( aDbc->mSessionId,
                              aDbc->mSessionSeq,
                              aErrorStack )
             == STL_SUCCESS );
    sState = 1;

    *aReturn = sslXaRecover( aXids,
                             (stlInt64)aCount,
                             aRmId,
                             (stlInt64)aFlags,
                             sEnv );

    if( STL_HAS_ERROR( KNL_ERROR_STACK(sEnv) ) == STL_TRUE )
    {
        stlAppendErrors( aErrorStack, KNL_ERROR_STACK( sEnv ) );
        STL_INIT_ERROR_STACK( KNL_ERROR_STACK( sEnv ) );
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

stlStatus zlaoForget( zlcDbc        * aDbc,
                      stlXid        * aXid,
                      stlInt32        aRmId,
                      stlInt32        aFlags,
                      stlInt32      * aReturn,
                      stlErrorStack * aErrorStack )
{
    stlInt32   sState = 0;
    sslEnv   * sEnv;
    
    sEnv = SSL_WORKER_ENV( knlGetSessionEnv(aDbc->mSessionId) );
    
    STL_TRY( sslEnterSession( aDbc->mSessionId,
                              aDbc->mSessionSeq,
                              aErrorStack )
             == STL_SUCCESS );
    sState = 1;

    *aReturn = sslXaForget( aXid,
                            aRmId,
                            (stlInt64)aFlags,
                            sEnv );

    if( STL_HAS_ERROR( KNL_ERROR_STACK(sEnv) ) == STL_TRUE )
    {
        stlAppendErrors( aErrorStack, KNL_ERROR_STACK( sEnv ) );
        STL_INIT_ERROR_STACK( KNL_ERROR_STACK( sEnv ) );
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
