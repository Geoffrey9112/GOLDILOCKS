/*******************************************************************************
 * zlapPrepare.c
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

#include <zlai.h>

/**
 * @file zlapPrepare.c
 * @brief ODBC API Internal Prepare Routines.
 */

/**
 * @addtogroup zlapPrepare
 * @{
 */

stlStatus zlapPrepare( zlcDbc               * aDbc,
                       zlsStmt              * aStmt,
                       stlChar              * aSql,
                       stlBool              * aResultSet,
                       stlBool              * aIsWithoutHoldCursor,
                       stlBool              * aIsUpdatable,
                       zllCursorSensitivity * aSensitivity,
                       stlBool              * aSuccessWithInfo,
                       stlErrorStack        * aErrorStack )
{
    stlBool    sHasTransaction = STL_FALSE;
    stlInt32   sStmtType       = ZLL_STMT_TYPE_UNKNOWN;
    stlInt32   sState = 0;
    sslEnv   * sEnv = NULL;
    
    STL_TRY( sslEnterSession( aDbc->mSessionId,
                              aDbc->mSessionSeq,
                              aErrorStack )
             == STL_SUCCESS );
    sState = 1;

    sEnv = SSL_WORKER_ENV( knlGetSessionEnv(aDbc->mSessionId) );

    STL_TRY( sslPrepare( aSql,
                         aStmt->mStmtId,
                         &sHasTransaction,
                         &sStmtType,
                         aResultSet,
                         aIsWithoutHoldCursor,
                         aIsUpdatable,
                         (ellCursorSensitivity*)aSensitivity,
                         sEnv ) == STL_SUCCESS );

    /* SUCCESS_WITH_INFO */
    if( STL_HAS_ERROR( KNL_ERROR_STACK(sEnv) ) == STL_TRUE )
    {
        zldDiagAdds( SQL_HANDLE_STMT,
                     (void*)aStmt,
                     SQL_NO_ROW_NUMBER,
                     SQL_NO_COLUMN_NUMBER,
                     KNL_ERROR_STACK(sEnv) );

        *aSuccessWithInfo = STL_TRUE;
    }

    zldDiagSetDynamicFunctionCode( &aStmt->mDiag, sStmtType );

    aStmt->mIsPrepared = STL_TRUE;

    /*
     * IRD 할당
     */

    STL_TRY( zlaiBuildNumResultColsInternal( aStmt,
                                             sEnv )
             == STL_SUCCESS );
    
    STL_TRY( zlaiNumParamsInternal( aDbc,
                                    aStmt,
                                    &aStmt->mParamCount,
                                    sEnv )
             == STL_SUCCESS );

    if( aDbc->mTransition == ZLC_TRANSITION_STATE_C5 )
    {
        if( aDbc->mAttrAutoCommit == STL_TRUE )
        {
            STL_TRY( sslCommit( sEnv ) == STL_SUCCESS );
        }
        else
        {
            if( sHasTransaction == STL_TRUE )
            {
                aDbc->mTransition = ZLC_TRANSITION_STATE_C6;
            }
        }
    }

    sState = 0;
    sslLeaveSession( aDbc->mSessionId,
                     aDbc->mSessionSeq );

    return STL_SUCCESS;

    STL_FINISH;
    
    if( aDbc->mTransition == ZLC_TRANSITION_STATE_C5 )
    {
        if( aDbc->mAttrAutoCommit == STL_TRUE )
        {
            (void)sslRollback( sEnv );
        }
    }
    
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
