/*******************************************************************************
 * zlaiCol.c
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
#include <zld.h>
#include <zlai.h>

/**
 * @file zlaiCol.c
 * @brief ODBC API Internal Column Routines.
 */

/**
 * @addtogroup zlai
 * @{
 */

stlStatus zlaiBuildNumResultCols( zlsStmt       * aStmt,
                                  stlErrorStack * aErrorStack )
{
    stlInt32   sState = 0;
    sslEnv   * sEnv = NULL;
    zlcDbc   * sDbc;
    
    sDbc = aStmt->mParentDbc;
    
    STL_TRY( sslEnterSession( sDbc->mSessionId,
                              sDbc->mSessionSeq,
                              aErrorStack )
             == STL_SUCCESS );
    sState = 1;

    sEnv = SSL_WORKER_ENV( knlGetSessionEnv(sDbc->mSessionId) );

    STL_TRY( zlaiBuildNumResultColsInternal( aStmt,
                                             sEnv )
             == STL_SUCCESS );

    sState = 0;
    sslLeaveSession( sDbc->mSessionId,
                     sDbc->mSessionSeq );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            stlAppendErrors( aErrorStack, KNL_ERROR_STACK( sEnv ) );
            STL_INIT_ERROR_STACK( KNL_ERROR_STACK( sEnv ) );
            sslLeaveSession( sDbc->mSessionId, sDbc->mSessionSeq );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus zlaiBuildNumResultColsInternal( zlsStmt  * aStmt,
                                          void     * aEnv )
{
    zldDesc  * sIrd;
    stlInt16   sColumnCount;
    sslEnv   * sEnv = (sslEnv*)aEnv;

    sIrd = &aStmt->mIrd;

    STL_TRY( sslNumResultCols( aStmt->mStmtId,
                               &sColumnCount,
                               sEnv ) == STL_SUCCESS );

    STL_TRY( zldDescReallocRec( sIrd,
                                sColumnCount,
                                KNL_ERROR_STACK(sEnv) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
