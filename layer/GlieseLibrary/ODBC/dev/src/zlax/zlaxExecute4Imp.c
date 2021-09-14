/*******************************************************************************
 * zlaxExecute4Imp.c
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
 * @file zlaxExecute4Imp.c
 * @brief ODBC API Internal Execute Routines.
 */

/**
 * @addtogroup zlxExecute
 * @{
 */

stlStatus zlaxExecute4Imp( zlcDbc             * aDbc,
                           zlsStmt            * aStmt,
                           stlUInt16            aParameterCount,
                           dtlValueBlockList ** aParameterData, 
                           stlInt64           * aAffectedRowCount,
                           stlBool            * aSuccessWithInfo,
                           stlErrorStack      * aErrorStack )
{
    zldDiag       * sDiag;
    stlBool         sResultSet = STL_FALSE;
    stlInt32        sArraySize;
    stlInt64        sRowCount;
    stlBool         sHasTransaction = STL_FALSE;
    stlBool         sIsRecompile = STL_FALSE;
    stlBool         sNeedInitParameterBlock = STL_FALSE;
    sslDCLContext   sDCLContext;
    stlInt32        sState = 0;
    sslEnv        * sEnv = NULL;
    
    sDiag = &aStmt->mDiag;

    SSL_INIT_DCL_CONTEXT( &sDCLContext );

    STL_TRY( sslEnterSession( aDbc->mSessionId,
                              aDbc->mSessionSeq,
                              aErrorStack )
             == STL_SUCCESS );
    sState = 1;

    sEnv = SSL_WORKER_ENV( knlGetSessionEnv(aDbc->mSessionId) );
    
    /*
     * atomic 설정
     */
    if( aStmt->mAtomicExecution == SQL_ATOMIC_EXECUTION_OFF )
    {
        STL_TRY( sslSetStmtAttr( aStmt->mStmtId,
                                 ZLL_STATEMENT_ATTR_ATOMIC_EXECUTION,
                                 SQL_ATOMIC_EXECUTION_ON,
                                 NULL,
                                 0,
                                 sEnv ) == STL_SUCCESS );
        
        aStmt->mAtomicExecution = SQL_ATOMIC_EXECUTION_ON;
        sNeedInitParameterBlock = STL_TRUE;
    }

    /*
     * array size를 dtlValueBlockList의 크기로 설정
     */

    sArraySize = DTL_GET_BLOCK_USED_CNT( aParameterData[0] );
    
    if( sArraySize != aStmt->mReadBlockCount4Imp )
    {
        STL_TRY( sslSetStmtAttr( aStmt->mStmtId,
                                 ZLL_STATEMENT_ATTR_PARAMSET_SIZE,
                                 (stlInt64)sArraySize,
                                 NULL,
                                 0,
                                 sEnv ) == STL_SUCCESS );

        aStmt->mReadBlockCount4Imp = sArraySize;
        sNeedInitParameterBlock = STL_TRUE;
    }

    if( sNeedInitParameterBlock == STL_TRUE )
    {
        STL_TRY( sslCreateParameter4Imp( aStmt->mStmtId,
                                         aParameterCount,
                                         aParameterData,
                                         sEnv ) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( sslSetParameterData4Imp( aStmt->mStmtId,
                                          aParameterCount,
                                          aParameterData,
                                          sEnv ) == STL_SUCCESS );
    }

    STL_TRY_THROW( sslExecute( aStmt->mStmtId,
                               STL_TRUE,
                               &sHasTransaction,
                               &sIsRecompile,
                               &sResultSet,
                               &sRowCount,
                               &sDCLContext,
                               sEnv ) == STL_SUCCESS,
                   RAMP_ERR_ATOMIC_EXECUTION );

    zldDiagSetRowCount( sDiag, sRowCount );
    aStmt->mRowCount = sRowCount;

    if( aAffectedRowCount != NULL )
    {
        *aAffectedRowCount = sRowCount;
    }

    if( aSuccessWithInfo != NULL )
    {
        *aSuccessWithInfo = STL_FALSE;
    }

    STL_ASSERT( sResultSet == STL_FALSE );

    switch( aDbc->mTransition )
    {
        case ZLC_TRANSITION_STATE_C5 :
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
            break;
        case ZLC_TRANSITION_STATE_C6 :
            if( aDbc->mAttrAutoCommit == STL_TRUE )
            {
                STL_TRY( sslCommit( sEnv ) == STL_SUCCESS );
            }
            break;
        default:
            break;
    }

    sState = 0;
    sslLeaveSession( aDbc->mSessionId,
                     aDbc->mSessionSeq );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_ATOMIC_EXECUTION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FAILED_TO_ATOMIC_EXECUTION,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    switch( aDbc->mTransition )
    {
        case ZLC_TRANSITION_STATE_C5 :
            if( aDbc->mAttrAutoCommit == STL_TRUE )
            {
                (void)sslRollback( sEnv );
            }
            else
            {
                if( sHasTransaction == STL_TRUE )
                {
                    aDbc->mTransition = ZLC_TRANSITION_STATE_C6;
                }
            }
            break;
        case ZLC_TRANSITION_STATE_C6 :
            if( aDbc->mAttrAutoCommit == STL_TRUE )
            {
                (void)sslRollback( sEnv );
            }
            break;
        default:
            break;
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
