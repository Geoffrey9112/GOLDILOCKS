/*******************************************************************************
 * zlatTrans.c
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
#include <zlr.h>

/**
 * @file zlatTrans.c
 * @brief ODBC API Transaction Routines.
 */

/**
 * @addtogroup zlatTrans
 * @{
 */

stlStatus zlatEndTran( zlcDbc        * aDbc,
                       SQLSMALLINT     aCompletionType,
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
    
    switch( aCompletionType )
    {
        case SQL_COMMIT :
            STL_TRY( sslCommit( sEnv ) == STL_SUCCESS );
            break;
        case SQL_ROLLBACK :
            STL_TRY( sslRollback( sEnv ) == STL_SUCCESS );
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
