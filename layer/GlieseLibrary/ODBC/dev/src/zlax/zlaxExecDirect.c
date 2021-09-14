/*******************************************************************************
 * zlaxExecDirect.c
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
#include <zlc.h>
#include <zld.h>

#include <zli.h>
#include <zlr.h>
#include <zlt.h>

#include <zlvSqlToC.h>
#include <zllRowStatusDef.h>

#include <zlai.h>

/**
 * @file zlaxExecDirect.c
 * @brief ODBC API Internal Execute Routines.
 */

/**
 * @addtogroup zlaxExecDirect
 * @{
 */

stlStatus zlaxExecDirect( zlcDbc               * aDbc,
                          zlsStmt              * aStmt,
                          stlChar              * aSql,
                          stlBool              * aResultSet,
                          stlBool              * aIsWithoutHoldCursor,
                          stlBool              * aIsUpdatable,
                          ellCursorSensitivity * aSensitivity,
                          stlInt64             * aAffectedRowCount,
                          stlBool              * aSuccessWithInfo,
                          stlErrorStack        * aErrorStack )
{
    zldDiag       * sDiag;
    stlInt32        sStmtType  = 0;
    stlInt64        sTotalRowCount = 0;
    zldDesc       * sApd;
    zldDesc       * sIpd;
    stlUInt64       sArraySize;
    stlUInt64       sArrayIdx;
    stlBool         sParamWithInfo;
    stlInt64        sWarningParamCount;
    stlInt64        sErrorParamCount;
    stlBool         sParamError;
    stlInt64        sIgnoreParamCount;
    stlInt64        sRowCount;
    stlBool         sIsAtomic = STL_FALSE;
    stlBool         sHasTransaction = STL_FALSE;
    stlBool         sIsRecompile = STL_FALSE;
    sslDCLContext   sDCLContext;
    stlBool         sIsFirst = STL_TRUE;
    stlInt32        sInitParamCount = 0;
    stlBool         sSuccessed = STL_FALSE;
    stlInt32        sState = 0;
    sslEnv        * sEnv;

    stlErrorData  * sErrorData = NULL;
    stlBool         sRollbacked = STL_FALSE;
    
    STL_TRY( sslEnterSession( aDbc->mSessionId,
                              aDbc->mSessionSeq,
                              aErrorStack )
             == STL_SUCCESS );
    sState = 1;

    sEnv = SSL_WORKER_ENV( knlGetSessionEnv(aDbc->mSessionId) );
    
    SSL_INIT_DCL_CONTEXT( &sDCLContext );
    
    sDiag = &aStmt->mDiag;

    sApd = aStmt->mApd;
    sIpd = &aStmt->mIpd;

    ZLR_INIT_RESULT_WITH_BUFFER( aStmt->mArd, &aStmt->mResult );

    sIsAtomic = ( (aStmt->mAtomicExecution == SQL_ATOMIC_EXECUTION_ON) ?
                  STL_TRUE : STL_FALSE );
    
    /*
     * parameter 정보 초기화
     */

    if( sIpd->mRowProcessed != NULL )
    {
        *sIpd->mRowProcessed = 0;
    }

    sErrorParamCount   = 0;
    sWarningParamCount = 0;
    sIgnoreParamCount  = 0;
    
    sArraySize = sApd->mArraySize;
    STL_DASSERT( sArraySize > 0 );

    STL_TRY( zlaiInitParameterBlock( aStmt,
                                     &sInitParamCount,
                                     sEnv ) == STL_SUCCESS );

    if( sIsAtomic == STL_FALSE )
    {
        sArrayIdx = 0;
        
        while( 1 )
        {
            sParamWithInfo = STL_FALSE;
            sParamError    = STL_FALSE;

            if( sIpd->mArrayStatusPtr != NULL )
            {
                sIpd->mArrayStatusPtr[sArrayIdx] = SQL_PARAM_UNUSED;
            }
        
            if( sApd->mArrayStatusPtr != NULL )
            {
                if( sApd->mArrayStatusPtr[sArrayIdx] == SQL_PARAM_IGNORE )
                {
                    sIgnoreParamCount++;
                    sArrayIdx++;
                    
                    if( sArrayIdx >= sArraySize )
                    {
                        break;
                    }
                    continue;
                }
            }
        
            if ( aStmt->mHasParamIN == STL_TRUE )
            {
                STL_TRY( zliSetParamValueIN( aStmt,
                                             sApd,
                                             sIpd,
                                             sArrayIdx,
                                             0,
                                             & sParamWithInfo,
                                             & sParamError,
                                             KNL_ERROR_STACK(sEnv) )
                         == STL_SUCCESS );
            }

            if ( sParamError == STL_TRUE )
            {
                sErrorParamCount++;
            
                if( sIpd->mArrayStatusPtr != NULL )
                {
                    sIpd->mArrayStatusPtr[sArrayIdx] = SQL_PARAM_ERROR;
                }
            }
            else
            {
                if ( sParamWithInfo == STL_TRUE )
                {
                    sWarningParamCount++;
            
                    if( sIpd->mArrayStatusPtr != NULL )
                    {
                        sIpd->mArrayStatusPtr[sArrayIdx] = SQL_PARAM_SUCCESS_WITH_INFO;
                    }
                }
                else
                {
                    if( sIpd->mArrayStatusPtr != NULL )
                    {
                        sIpd->mArrayStatusPtr[sArrayIdx] = SQL_PARAM_SUCCESS;
                    }
                }
            }
            
            if( sParamError == STL_FALSE )
            {
                if( sslExecDirect( aSql,
                                   aStmt->mStmtId,
                                   sIsFirst,
                                   &sHasTransaction,
                                   &sIsRecompile,
                                   &sStmtType,
                                   aResultSet,
                                   aIsWithoutHoldCursor,
                                   aIsUpdatable,
                                   aSensitivity,
                                   &sRowCount,
                                   &sDCLContext,
                                   sEnv ) != STL_SUCCESS )
                {
                    zldDiagAdds( SQL_HANDLE_STMT,
                                 (void*)aStmt,
                                 sArrayIdx + 1,
                                 SQL_NO_COLUMN_NUMBER,
                                 KNL_ERROR_STACK(sEnv) );

                    if( sIpd->mArrayStatusPtr != NULL )
                    {
                        sIpd->mArrayStatusPtr[sArrayIdx] = SQL_PARAM_ERROR;
                    }

                    sErrorParamCount++;
                }
                else
                {
                    /* SUCCESS_WITH_INFO */
                    if( STL_HAS_ERROR( KNL_ERROR_STACK(sEnv) ) == STL_TRUE )
                    {
                        (void)zldDiagAdds( SQL_HANDLE_STMT,
                                           (void*)aStmt,
                                           sArrayIdx + 1,
                                           SQL_NO_COLUMN_NUMBER,
                                           KNL_ERROR_STACK(sEnv) );

                        sWarningParamCount++;
                    }
                    
                    if( sRowCount >= 0 )
                    {
                        sTotalRowCount += sRowCount;
                    }
                    else
                    {
                        sTotalRowCount = -1;
                    }

                    if( sRowCount == 1 )
                    {
                        /**
                         * OUT Parameter 를 설정할 수 있음 
                         */
                    
                        sParamWithInfo = STL_FALSE;
                        sParamError    = STL_FALSE;
                
                        if ( aStmt->mHasParamOUT == STL_TRUE )
                        {
                            STL_TRY( zliSetParamValueOUT( aStmt,
                                                          sApd,
                                                          sIpd,
                                                          sArrayIdx,
                                                          0,
                                                          & sParamWithInfo,
                                                          & sParamError,
                                                          KNL_ERROR_STACK(sEnv) )
                                     == STL_SUCCESS );
                        }
                    
                        if ( sParamWithInfo == STL_TRUE )
                        {
                            sWarningParamCount++;
                        }
                    
                        if ( sParamError == STL_TRUE )
                        {
                            sErrorParamCount++;
                        }
                    }
                    else
                    {
                        /**
                         * OUT Parameter 를 설정할 수 없음
                         * - 결과가 없거나
                         * - OUT Parameter 가 존재할 수 없는 SQL 임.
                         */ 
                    }

                    if( (sSuccessed == STL_FALSE) &&
                        (sDCLContext.mType == SSL_DCL_TYPE_NONE) )
                    {
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
                    }
                
                    sSuccessed = STL_TRUE;
                }

                sIsFirst = STL_FALSE;
            }

            sArrayIdx++;

            if( sArrayIdx >= sArraySize )
            {
                break;
            }
        } /* while */
    }
    else
    {
        for( sArrayIdx = 0; sArrayIdx < sArraySize; sArrayIdx++ )
        {
            sParamWithInfo = STL_FALSE;
            sParamError    = STL_FALSE;

            if( sIpd->mArrayStatusPtr != NULL )
            {
                sIpd->mArrayStatusPtr[sArrayIdx] = SQL_PARAM_UNUSED;
            }
        
            if ( aStmt->mHasParamIN == STL_TRUE )
            {
                STL_TRY( zliSetParamValueIN( aStmt,
                                             sApd,
                                             sIpd,
                                             sArrayIdx,
                                             sArrayIdx,
                                             & sParamWithInfo,
                                             & sParamError,
                                             KNL_ERROR_STACK(sEnv) )
                         == STL_SUCCESS );
            }

            if ( sParamError == STL_TRUE )
            {
                sErrorParamCount++;
            
                /**
                 * Atomic에서는 all or nothing이다.
                 */
            
                STL_THROW( RAMP_ERR_ATOMIC );
            
                if( sIpd->mArrayStatusPtr != NULL )
                {
                    sIpd->mArrayStatusPtr[sArrayIdx] = SQL_PARAM_ERROR;
                }
            }
            else
            {
                if ( sParamWithInfo == STL_TRUE )
                {
                    sWarningParamCount++;
            
                    if( sIpd->mArrayStatusPtr != NULL )
                    {
                        sIpd->mArrayStatusPtr[sArrayIdx] = SQL_PARAM_SUCCESS_WITH_INFO;
                    }
                }
                else
                {
                    if( sIpd->mArrayStatusPtr != NULL )
                    {
                        sIpd->mArrayStatusPtr[sArrayIdx] = SQL_PARAM_SUCCESS;
                    }
                }
            }
        }

        if( sslExecDirect( aSql,
                           aStmt->mStmtId,
                           sIsFirst,
                           &sHasTransaction,
                           &sIsRecompile,
                           &sStmtType,
                           aResultSet,
                           aIsWithoutHoldCursor,
                           aIsUpdatable,
                           aSensitivity,
                           &sRowCount,
                           &sDCLContext,
                           sEnv ) != STL_SUCCESS )
        {
            sErrorParamCount = sArraySize;
            STL_THROW( RAMP_ERR_ATOMIC );
        }

        /* SUCCESS_WITH_INFO */
        if( STL_HAS_ERROR( KNL_ERROR_STACK(sEnv) ) == STL_TRUE )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               (void*)aStmt,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               KNL_ERROR_STACK(sEnv) );

            sWarningParamCount++;
        }

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

        sIsFirst = STL_FALSE;
        
        if( sRowCount >= 0 )
        {
            sTotalRowCount += sRowCount;
        }
        else
        {
            sTotalRowCount = -1;
        }
            
        if( (sRowCount / sArraySize) == 1 )
        {
            for( sArrayIdx = 0; sArrayIdx < sArraySize; sArrayIdx++ )
            {
                /**
                 * OUT Parameter 를 설정할 수 있음 
                 */
                    
                sParamWithInfo = STL_FALSE;
                sParamError    = STL_FALSE;
                
                if ( aStmt->mHasParamOUT == STL_TRUE )
                {
                    STL_TRY( zliSetParamValueOUT( aStmt,
                                                  sApd,
                                                  sIpd,
                                                  sArrayIdx,
                                                  sArrayIdx,
                                                  & sParamWithInfo,
                                                  & sParamError,
                                                  KNL_ERROR_STACK(sEnv) )
                             == STL_SUCCESS );
                }
                    
                if ( sParamWithInfo == STL_TRUE )
                {
                    sWarningParamCount++;
                }
                    
                if ( sParamError == STL_TRUE )
                {
                    sErrorParamCount++;
                    STL_THROW( RAMP_ERR_ATOMIC );
                }
            }
        }
        else
        {
            /**
             * OUT Parameter 를 설정할 수 없음
             * - 결과가 없거나
             * - OUT Parameter 가 존재할 수 없는 SQL 임.
             */ 
        }
    }

    STL_RAMP( RAMP_ERR_ATOMIC );

    if( sIsRecompile == STL_TRUE )
    {
        aStmt->mNeedResultDescriptor = STL_TRUE;
    }
    
    if( sIpd->mRowProcessed != NULL )
    {
        *sIpd->mRowProcessed = sArraySize;
    }

    if( sIsAtomic == STL_TRUE )
    {
        if( sErrorParamCount > 0 )
        {
            STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                                   (void*)aStmt,
                                   KNL_ERROR_STACK(sEnv) ) == STL_SUCCESS );
            
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          ZLE_ERRCODE_FAILED_TO_ATOMIC_EXECUTION,
                          NULL,
                          KNL_ERROR_STACK(sEnv) );
            
            STL_TRY( zldDiagAdds( SQL_HANDLE_STMT,
                                  (void*)aStmt,
                                  SQL_NO_ROW_NUMBER,
                                  SQL_NO_COLUMN_NUMBER,
                                  KNL_ERROR_STACK(sEnv) ) == STL_SUCCESS );
            
            sTotalRowCount = -1;
            sErrorParamCount = sArraySize;
            sWarningParamCount = 0;
        }
        else
        {
            if( sWarningParamCount > 0 )
            {
                STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                                       (void*)aStmt,
                                       KNL_ERROR_STACK(sEnv) ) == STL_SUCCESS );
                
                for( sArrayIdx = 0; sArrayIdx < sArraySize; sArrayIdx++ )
                {
                    if( sIpd->mArrayStatusPtr != NULL )
                    {
                        sIpd->mArrayStatusPtr[sArrayIdx] = SQL_PARAM_DIAG_UNAVAILABLE;
                    }
                }
                
                sWarningParamCount = sArraySize;
            }
        }
    }

    if( sArraySize > 0 )
    {
        STL_TRY( sArraySize != (sErrorParamCount + sIgnoreParamCount) );
    }

    zldDiagSetRowCount( sDiag, sTotalRowCount );
    zldDiagSetDynamicFunctionCode( sDiag, sStmtType );
    
    aStmt->mRowCount = sTotalRowCount;

    if( aAffectedRowCount != NULL )
    {
        *aAffectedRowCount = sDiag->mRowCount;
    }

    if( aSuccessWithInfo != NULL )
    {
        if( ( sWarningParamCount + sErrorParamCount ) == 0 )
        {
            *aSuccessWithInfo = STL_FALSE;
        }
        else
        {
            *aSuccessWithInfo = STL_TRUE;
        }
    }

    switch( sDCLContext.mType )
    {
        case SSL_DCL_TYPE_SET_TIME_ZONE :
            aDbc->mTimezone = sDCLContext.mGMTOffset;
            break;
        case SSL_DCL_TYPE_OPEN_DATABASE :
            aDbc->mNlsCharacterSet      = sDCLContext.mCharacterSet;
            aDbc->mNlsNCharCharacterSet = sDCLContext.mNCharCharacterSet;

            STL_TRY( zlcSetNlsDTFuncVector( aDbc ) == STL_SUCCESS );
            break;
        case SSL_DCL_TYPE_CLOSE_DATABASE :
            aDbc->mBrokenConnection = STL_TRUE;
            aDbc->mTransition       = ZLC_TRANSITION_STATE_C5;

            (void) knlCleanupHeapMem( KNL_ENV(sEnv) );
            knlDisconnectSession( SSL_SESS_ENV(sEnv) );
            
            STL_THROW( RAMP_FINISH );
            break;
        default:
            /**
             * @todo ALTER SESSION SET DATE_FORMAT이 지원되면 DATE_FORMAT 정보도 필요하다.
             */
            break;
    }

    sState = 0;
    sslLeaveSession( aDbc->mSessionId,
                     aDbc->mSessionSeq );

    switch( aDbc->mTransition )
    {
        case ZLC_TRANSITION_STATE_C5 :
            if( aDbc->mAttrAutoCommit == STL_TRUE )
            {
                if( sHasTransaction == STL_TRUE )
                {
                    STL_TRY( zltEndTran( aDbc,
                                         SQL_COMMIT,
                                         aErrorStack ) == STL_SUCCESS );
                }

                if( (*aResultSet == STL_TRUE) &&
                    (*aIsWithoutHoldCursor == STL_FALSE) )
                {
                    aDbc->mTransition = ZLC_TRANSITION_STATE_C6;
                }
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
            switch( sDCLContext.mType )
            {
                case SSL_DCL_TYPE_END_TRANSACTION :
                    STL_TRY( zlrCloseWithoutHold( aDbc,
                                                  aErrorStack ) == STL_SUCCESS );

                    aDbc->mTransition = ZLC_TRANSITION_STATE_C5;
                    break;
                default:
                    if( aDbc->mAttrAutoCommit == STL_TRUE )
                    {
                        STL_TRY( zltEndTran( aDbc,
                                             SQL_COMMIT,
                                             aErrorStack ) == STL_SUCCESS );
                    }
                    break;
            }
            break;
        default:
            break;
    }

    STL_RAMP( RAMP_FINISH );
    
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

    sErrorData = stlGetLastErrorData( aErrorStack );

    if( sErrorData != NULL )
    {
        switch( stlGetExternalErrorCode( sErrorData ) )
        {
            case STL_EXT_ERRCODE_TRANSACTION_ROLLBACK_NO_SUBCLASS :
            case STL_EXT_ERRCODE_TRANSACTION_ROLLBACK_SERIALIZATION_FAILURE :
            case STL_EXT_ERRCODE_TRANSACTION_ROLLBACK_INTEGRITY_CONSTRAINT_VIOLATION :
            case STL_EXT_ERRCODE_TRANSACTION_ROLLBACK_STATEMENT_COMPLETION_UNKNOWN :
            case STL_EXT_ERRCODE_TRANSACTION_ROLLBACK_TRIGGERED_ACTION_EXCEPTION :
                (void)zlrCloseWithoutHold( aDbc, aErrorStack );
                aDbc->mTransition = ZLC_TRANSITION_STATE_C5;
                sRollbacked = STL_TRUE;
                break;
            default :
                break;
        }
    }

    if( sRollbacked == STL_FALSE )
    {
        switch( aDbc->mTransition )
        {
            case ZLC_TRANSITION_STATE_C5 :
                if( aDbc->mAttrAutoCommit == STL_TRUE )
                {
                    (void)zltEndTran( aDbc,
                                      SQL_ROLLBACK,
                                      aErrorStack );
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
                    (void)zltEndTran( aDbc,
                                      SQL_ROLLBACK,
                                      aErrorStack );
                }
                break;
            default:
                break;
        }
    }

    return STL_FAILURE;
}

/** @} */
