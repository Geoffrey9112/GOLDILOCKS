/*******************************************************************************
 * zlasStmt.c
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
#include <ssl.h>
#include <zlDef.h>
#include <zle.h>
#include <zld.h>

/**
 * @file zlasStmt.c
 * @brief ODBC API Internal Statement Routines.
 */

/**
 * @addtogroup zlasStmt
 * @{
 */

stlStatus zlasAlloc( zlcDbc        * aDbc,
                     zlsStmt       * aStmt,
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
    
    STL_TRY( sslAllocStatement( &aStmt->mStmtId,
                                sEnv )
             == STL_SUCCESS );

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

stlStatus zlasFree( zlcDbc            * aDbc,
                    zlsStmt           * aStmt,
                    zllFreeStmtOption   aOption,
                    stlBool             aIsCleanup,                   
                    stlErrorStack     * aErrorStack )
{
    stlInt32   sState = 0;
    sslEnv   * sEnv;
    
    if( aIsCleanup == STL_FALSE )
    {
        STL_TRY( sslEnterSession( aDbc->mSessionId,
                                  aDbc->mSessionSeq,
                                  aErrorStack )
                 == STL_SUCCESS );
        sState = 1;

        sEnv = SSL_WORKER_ENV( knlGetSessionEnv(aDbc->mSessionId) );
    
        STL_TRY( sslFreeStatement( aStmt->mStmtId,
                                   aOption,
                                   sEnv ) == STL_SUCCESS );

        sState = 0;
        sslLeaveSession( aDbc->mSessionId,
                         aDbc->mSessionSeq );
    }
    
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

stlStatus zlasGetAttr( zlcDbc           * aDbc,
                       zlsStmt          * aStmt,
                       zllStatementAttr   aAttr,
                       void             * aValue,
                       stlInt32           aBufferLen,
                       stlInt32         * aStrLen,
                       stlErrorStack    * aErrorStack )
{
    stlInt32           sState = 0;
    sslEnv           * sEnv;
    
    STL_TRY( sslEnterSession( aDbc->mSessionId,
                              aDbc->mSessionSeq,
                              aErrorStack )
             == STL_SUCCESS );
    sState = 1;

    sEnv = SSL_WORKER_ENV( knlGetSessionEnv(aDbc->mSessionId) );
    
    STL_TRY( sslGetStmtAttr( aStmt->mStmtId,
                             aAttr,
                             aValue,
                             (stlChar*)aValue,
                             aBufferLen,
                             aStrLen,
                             sEnv ) == STL_SUCCESS );

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

stlStatus zlasSetAttr( zlcDbc           * aDbc,
                       zlsStmt          * aStmt,
                       zllStatementAttr   aAttr,
                       stlInt64           aIntValue,
                       stlChar          * aStrValue,
                       stlInt32           aStrLength,
                       stlErrorStack *    aErrorStack )
{
    stlInt32   sState = 0;
    sslEnv   * sEnv;
    
    STL_TRY( sslEnterSession( aDbc->mSessionId,
                              aDbc->mSessionSeq,
                              aErrorStack )
             == STL_SUCCESS );
    sState = 1;

    sEnv = SSL_WORKER_ENV( knlGetSessionEnv(aDbc->mSessionId) );
    
    STL_TRY( sslSetStmtAttr( aStmt->mStmtId,
                             aAttr,
                             aIntValue,
                             aStrValue,
                             aStrLength,
                             sEnv ) == STL_SUCCESS );

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
