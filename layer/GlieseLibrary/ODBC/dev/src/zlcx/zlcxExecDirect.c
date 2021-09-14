/*******************************************************************************
 * zlcxExecDirect.c
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
#include <zlc.h>
#include <zld.h>

#include <zli.h>
#include <zlr.h>

#include <zlvSqlToC.h>
#include <zllRowStatusDef.h>

#include <zlci.h>
#include <zlcr.h>

/**
 * @file zlcxExecDirect.c
 * @brief ODBC API Internal Execute Routines.
 */

/**
 * @addtogroup zlcxExecDirect
 * @{
 */

stlStatus zlcxExecDirect( zlcDbc               * aDbc,
                          zlsStmt              * aStmt,
                          stlChar              * aSql,
                          stlBool              * aResultSet,
                          stlBool              * aIsWithoutHoldCursor,
                          stlBool              * aIsUpdatable,
                          zllCursorSensitivity * aSensitivity,
                          stlInt64             * aAffectedRowCount,
                          stlBool              * aSuccessWithInfo,
                          stlErrorStack        * aErrorStack )
{
    cmlHandle     * sHandle;
    stlInt32        sTargetCount;
    cmlTargetType * sTargetType;
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
    stlInt64        sIgnoreParamCount;
    stlBool         sParamError;
    stlInt64        sRowCount = -1;
    stlBool         sIsAtomic = STL_FALSE;
    stlBool         sHasTransaction = STL_FALSE;
    stlBool         sIsRecompile = STL_FALSE;
    stlInt8         sDclAttrCount = 0;
    stlInt32        sDclAttrType = ZLL_DCL_TYPE_NONE;
    dtlDataType     sDclAttrDataType;
    stlInt32        sDclAttrValue = 0;
    stlInt32        sDclAttrValueSize;
    stlInt32        sInitParamCount = 0;
    stlInt32        sExecParamCount = 0;
    stlInt32        sNumParams = 0;
    stlInt32        i;

    stlBool         sChanged;
    stlInt32        sState = 0;
    stlBool         sOutParameter = STL_FALSE;
    stlBool         sExecuteIgnored = STL_FALSE;
    stlBool         sIgnored = STL_FALSE;
    stlBool         sReqNumParams = STL_FALSE;
    stlBool         sReqTargetType = STL_FALSE;
    stlBool         sReqFetch = STL_FALSE;
    stlInt8         sControl;

    stlInt8         sSensitivity;

    stlBool         sSentCommit = STL_FALSE;

    stlErrorData  * sErrorData = NULL;
    stlBool         sRollbacked = STL_FALSE;

    *aSuccessWithInfo = STL_FALSE;
    
    sHandle = &aDbc->mComm;
    
    sDiag = &aStmt->mDiag;

    sApd = aStmt->mApd;
    sIpd = &aStmt->mIpd;

    ZLR_INIT_RESULT( aStmt->mArd, &aStmt->mResult );

    sIsAtomic = ( (aStmt->mAtomicExecution == SQL_ATOMIC_EXECUTION_ON) ?
                  STL_TRUE : STL_FALSE );

    /**
     * @todo IRD 메모리 정리
     */
    
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

    /*
     * 파라메터 타입 전달
     */
    STL_TRY( zlciInitParameterBlock( aStmt,
                                     &sInitParamCount,
                                     aErrorStack ) == STL_SUCCESS );

    if( sIsAtomic == STL_FALSE )
    {
        sArrayIdx = 0;
        
        while( 1 )
        {
            sControl       = CML_CONTROL_HEAD;

            sRowCount      = -1;
            
            sOutParameter  = STL_FALSE;
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
                                             aErrorStack )
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
                if( aStmt->mParamChanged == STL_TRUE )
                {
                    /*
                     * 파라메터 타입 전달
                     */
                    STL_TRY( zlciExecuteParameterType( aStmt,
                                                       &sControl,
                                                       aErrorStack ) == STL_SUCCESS );
                }
                
                /*
                 * 파라메터 데이터 전달
                 */
                STL_TRY( zlciExecuteParameterBlock( aStmt,
                                                    &sControl,
                                                    &sExecParamCount,
                                                    aErrorStack ) == STL_SUCCESS );

                STL_TRY( cmlWriteExecDirectCommand( sHandle,
                                                    sControl,
                                                    aStmt->mStmtId,
                                                    aStmt->mParamChanged,
                                                    aSql,
                                                    stlStrlen( aSql ),
                                                    aErrorStack ) == STL_SUCCESS );
                sControl = CML_CONTROL_NONE;

                if( aStmt->mHasParamOUT == STL_TRUE )
                {
                    STL_TRY( zlciExecuteOutParameterBlock( aStmt,
                                                           STL_FALSE,
                                                           aErrorStack ) == STL_SUCCESS );
                    sOutParameter = STL_TRUE;
                }

                STL_TRY( cmlWriteNumParamsCommand( sHandle,
                                                   sControl,
                                                   aStmt->mStmtId,
                                                   aErrorStack ) == STL_SUCCESS );
                sReqNumParams = STL_TRUE;
                
                /**
                 * @todo Array Size가 1이 아니면 FETCH가 필요 없나?
                 */
                if( sArraySize == 1 )
                {
                    STL_TRY( cmlWriteTargetTypeCommand( sHandle,
                                                        sControl,
                                                        aStmt->mStmtId,
                                                        aErrorStack ) == STL_SUCCESS );
                    sReqTargetType = STL_TRUE;

                    STL_TRY( zlcrReqFetch( aStmt, aErrorStack ) == STL_SUCCESS );
                    sReqFetch = STL_TRUE;
                }
                else
                {
                    STL_TRY( cmlWriteTargetTypeCommand( sHandle,
                                                        CML_CONTROL_TAIL,
                                                        aStmt->mStmtId,
                                                        aErrorStack ) == STL_SUCCESS );
                    sReqTargetType = STL_TRUE;
                }

                /*
                 * 마지막에 commit 프로토콜 전송
                 */
                if( (sArrayIdx + 1 == sArraySize) &&
                    (aDbc->mAttrAutoCommit == STL_TRUE) )
                {
                    STL_TRY( cmlWriteTransactionCommand( sHandle,
                                                         CML_CONTROL_HEAD | CML_CONTROL_TAIL,
                                                         ZLL_COMPLETION_TYPE_COMMIT,
                                                         aErrorStack ) == STL_SUCCESS );
                    sSentCommit = STL_TRUE;
                }

                STL_TRY( cmlSendPacket( sHandle,
                                        aErrorStack ) == STL_SUCCESS );

                if( aStmt->mParamChanged == STL_TRUE )
                {
                    for( i = 0; i < sInitParamCount; i++ )
                    {
                        sState = 6;
                        STL_TRY_THROW( cmlReadBindTypeResult( sHandle,
                                                              &sIgnored,
                                                              &aStmt->mStmtId,
                                                              aErrorStack ) == STL_SUCCESS,
                                       RAMP_ERR_BIND_TYPE );
                    }
                }
                    
                if( sExecParamCount > 0 )
                {
                    sState = 5;
                    STL_TRY_THROW( cmlReadBindDataResult( sHandle,
                                                          &sIgnored,
                                                          aErrorStack ) == STL_SUCCESS,
                                   RAMP_ERR_BIND_DATA );
                }

                sState = 4;
                if( cmlReadExecDirectResult( sHandle,
                                             &sExecuteIgnored,
                                             &aStmt->mStmtId,
                                             &sStmtType,
                                             &sRowCount,
                                             &sIsRecompile,
                                             aResultSet,
                                             aIsWithoutHoldCursor,
                                             aIsUpdatable,
                                             &sSensitivity,
                                             &sHasTransaction,
                                             &sDclAttrCount,
                                             &sDclAttrType,
                                             &sDclAttrDataType,
                                             (stlChar*)&sDclAttrValue,
                                             &sDclAttrValueSize,
                                             aErrorStack ) != STL_SUCCESS )
                {
                    zldDiagAdds( SQL_HANDLE_STMT,
                                 (void*)aStmt,
                                 sArrayIdx + 1,
                                 SQL_NO_COLUMN_NUMBER,
                                 aErrorStack );

                    if( sIpd->mArrayStatusPtr != NULL )
                    {
                        sIpd->mArrayStatusPtr[sArrayIdx] = SQL_PARAM_ERROR;
                    }

                    sErrorParamCount++;

                    if( (aStmt->mStmtId != -1) &&
                        (aStmt->mParamChanged == STL_TRUE) )
                    {
                        STL_TRY( zlciSetParameterComplete( aStmt, aErrorStack )
                                 == STL_SUCCESS );

                        aStmt->mParamChanged = STL_FALSE;
                    }
                }
                else
                {
                    if( sExecuteIgnored == STL_FALSE )
                    {
                        /* SUCCESS_WITH_INFO */
                        if( STL_HAS_ERROR( aErrorStack ) == STL_TRUE )
                        {
                            zldDiagAdds( SQL_HANDLE_STMT,
                                         (void*)aStmt,
                                         sArrayIdx + 1,
                                         SQL_NO_COLUMN_NUMBER,
                                         aErrorStack );

                            sWarningParamCount++;
                        }

                        *aSensitivity = sSensitivity;

                        if( aStmt->mParamChanged == STL_TRUE )
                        {
                            STL_TRY( zlciSetParameterComplete( aStmt,
                                                               aErrorStack )
                                     == STL_SUCCESS );

                            aStmt->mParamChanged = STL_FALSE;
                        }
                    }
                    
                    if( sRowCount >= 0 )
                    {
                        sTotalRowCount += sRowCount;
                    }
                    else
                    {
                        sTotalRowCount = -1;
                    }
                }

                if( sDclAttrType == ZLL_DCL_TYPE_CLOSE_DATABASE )
                {
                    if ( sOutParameter == STL_TRUE )
                    {
                        (void)zlciGetOutParameterBlock( aStmt,
                                                        &sIgnored,
                                                        aErrorStack );
                    }

                    if( sReqNumParams == STL_TRUE )
                    {
                        (void)cmlReadNumParamsResult( sHandle,
                                                      &sIgnored,
                                                      &sNumParams,
                                                      aErrorStack );
                    }
                    
                    if( sReqTargetType == STL_TRUE )
                    {
                        (void)cmlReadTargetTypeResult( sHandle,
                                                       &sIgnored,
                                                       &sChanged,
                                                       &sTargetCount,
                                                       &sTargetType,
                                                       aErrorStack );
                    }
                    
                    if( sReqFetch == STL_TRUE )
                    {
                        (void)zlcrResFetch( aStmt, aErrorStack );
                    }
                }
                else
                {
                    sParamWithInfo = STL_FALSE;
                    sParamError    = STL_FALSE;
                
                    if ( sOutParameter == STL_TRUE )
                    {
                        /*
                         * OUT Parameter 를 설정할 수 있음 
                         */
                    
                        sState = 3;
                        sOutParameter = STL_FALSE;
                        STL_TRY( zlciGetOutParameterBlock( aStmt,
                                                           &sIgnored,
                                                           aErrorStack ) == STL_SUCCESS );

                        if( sIgnored == STL_FALSE )
                        {
                            STL_TRY( zliSetParamValueOUT( aStmt,
                                                          sApd,
                                                          sIpd,
                                                          sArrayIdx,
                                                          0,
                                                          & sParamWithInfo,
                                                          & sParamError,
                                                          aErrorStack )
                                     == STL_SUCCESS );
                        }
                    }
                    
                    if ( sParamWithInfo == STL_TRUE )
                    {
                        sWarningParamCount++;
                    }
                    
                    if ( sParamError == STL_TRUE )
                    {
                        sErrorParamCount++;
                    }

                    if( sReqNumParams == STL_TRUE )
                    {
                        sState = 2;
                        STL_TRY( cmlReadNumParamsResult( sHandle,
                                                         &sIgnored,
                                                         &sNumParams,
                                                         aErrorStack ) == STL_SUCCESS );
                        aStmt->mParamCount = sNumParams;
                    }
                
                    if( sReqTargetType == STL_TRUE )
                    {
                        sState = 1;
                        STL_TRY( cmlReadTargetTypeResult( sHandle,
                                                          &sIgnored,
                                                          &sChanged,
                                                          &sTargetCount,
                                                          &sTargetType,
                                                          aErrorStack ) == STL_SUCCESS );

                        if( ( sIgnored == STL_FALSE) && ( sChanged == STL_TRUE ) )
                        {
                            STL_TRY( zlciBuildTargetType( aStmt,
                                                          sTargetCount,
                                                          sTargetType,
                                                          aErrorStack ) == STL_SUCCESS );
                        }
                    }

                    if( sReqFetch == STL_TRUE )
                    {
                        sReqFetch = STL_FALSE;
                        sState = 0;

                        STL_TRY( zlcrResFetch( aStmt, aErrorStack ) == STL_SUCCESS );
                    }
                }
            }
            else
            {
                /*
                 * 마지막에 commit 프로토콜 전송
                 */
                if( (sArrayIdx + 1 == sArraySize) &&
                    (aDbc->mAttrAutoCommit == STL_TRUE) )
                {
                    STL_TRY( cmlWriteTransactionCommand( sHandle,
                                                         CML_CONTROL_HEAD | CML_CONTROL_TAIL,
                                                         ZLL_COMPLETION_TYPE_COMMIT,
                                                         aErrorStack ) == STL_SUCCESS );
                    sSentCommit = STL_TRUE;
                }

                STL_TRY( cmlSendPacket( sHandle,
                                        aErrorStack ) == STL_SUCCESS );
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
                                             aErrorStack )
                         == STL_SUCCESS );
            }

            if ( sParamError == STL_TRUE )
            {
                sErrorParamCount++;
            
                /*
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

        sControl = CML_CONTROL_HEAD;
        
        if( aStmt->mParamChanged == STL_TRUE )
        {
            /*
             * 파라메터 타입 전달
             */
            STL_TRY( zlciExecuteParameterType( aStmt,
                                               &sControl,
                                               aErrorStack ) == STL_SUCCESS );
        }
                
        /*
         * 파라메터 데이터 전달
         */
        STL_TRY( zlciExecuteParameterBlock( aStmt,
                                            &sControl,
                                            &sExecParamCount,
                                            aErrorStack ) == STL_SUCCESS );

        STL_TRY( cmlWriteExecDirectCommand( sHandle,
                                            sControl,
                                            aStmt->mStmtId,
                                            aStmt->mParamChanged,
                                            aSql,
                                            stlStrlen( aSql ),
                                            aErrorStack ) == STL_SUCCESS );

        sControl |= CML_CONTROL_TAIL;
        STL_TRY( cmlWriteNumParamsCommand( sHandle,
                                           sControl,
                                           aStmt->mStmtId,
                                           aErrorStack ) == STL_SUCCESS );
        sReqNumParams = STL_TRUE;
        
        sControl = CML_CONTROL_NONE;

        /*
         * 마지막에 commit 프로토콜 전송
         */
        if( aDbc->mAttrAutoCommit == STL_TRUE )
        {
            STL_TRY( cmlWriteTransactionCommand( sHandle,
                                                 CML_CONTROL_HEAD | CML_CONTROL_TAIL,
                                                 ZLL_COMPLETION_TYPE_COMMIT,
                                                 aErrorStack ) == STL_SUCCESS );
            sSentCommit = STL_TRUE;
        }
        
        STL_TRY( cmlSendPacket( sHandle,
                                aErrorStack ) == STL_SUCCESS );

        if( aStmt->mParamChanged == STL_TRUE )
        {
            for( i = 0; i < sInitParamCount; i++ )
            {
                sState = 6;
                STL_TRY_THROW( cmlReadBindTypeResult( sHandle,
                                                      &sIgnored,
                                                      &aStmt->mStmtId,
                                                      aErrorStack ) == STL_SUCCESS,
                               RAMP_ERR_BIND_TYPE );
            }
        }
                    
        if( sExecParamCount > 0 )
        {
            sState = 5;
            STL_TRY_THROW( cmlReadBindDataResult( sHandle,
                                                  &sIgnored,
                                                  aErrorStack ) == STL_SUCCESS,
                           RAMP_ERR_BIND_DATA );
        }

        sState = 4;
        if( cmlReadExecDirectResult( sHandle,
                                     &sExecuteIgnored,
                                     &aStmt->mStmtId,
                                     &sStmtType,
                                     &sRowCount,
                                     &sIsRecompile,
                                     aResultSet,
                                     aIsWithoutHoldCursor,
                                     aIsUpdatable,
                                     &sSensitivity,
                                     &sHasTransaction,
                                     &sDclAttrCount,
                                     &sDclAttrType,
                                     &sDclAttrDataType,
                                     (stlChar*)&sDclAttrValue,
                                     &sDclAttrValueSize,
                                     aErrorStack ) != STL_SUCCESS )
        {
            sErrorParamCount = sArraySize;

            if( (aStmt->mStmtId != -1) &&
                (aStmt->mParamChanged == STL_TRUE) )
            {
                STL_TRY( zlciSetParameterComplete( aStmt, aErrorStack ) == STL_SUCCESS );

                aStmt->mParamChanged = STL_FALSE;
            }
            
            if( sReqNumParams == STL_TRUE )
            {
                sState = 2;
                (void)cmlReadNumParamsResult( sHandle,
                                              &sIgnored,
                                              &sNumParams,
                                              aErrorStack );
            }
            
            STL_THROW( RAMP_ERR_ATOMIC );
        }

        if( sExecuteIgnored == STL_FALSE )
        {
            /* SUCCESS_WITH_INFO */
            if( STL_HAS_ERROR( aErrorStack ) == STL_TRUE )
            {
                zldDiagAdds( SQL_HANDLE_STMT,
                             (void*)aStmt,
                             SQL_NO_ROW_NUMBER,
                             SQL_NO_COLUMN_NUMBER,
                             aErrorStack );

                sWarningParamCount++;
            }

            *aSensitivity = sSensitivity;

            if( aStmt->mParamChanged == STL_TRUE )
            {
                STL_TRY( zlciSetParameterComplete( aStmt, aErrorStack ) == STL_SUCCESS );

                aStmt->mParamChanged = STL_FALSE;
            }
        }

        if( sRowCount >= 0 )
        {
            sTotalRowCount += sRowCount;
        }
        else
        {
            sTotalRowCount = -1;
        }

        if( sReqNumParams == STL_TRUE )
        {
            sState = 2;
            STL_TRY( cmlReadNumParamsResult( sHandle,
                                             &sIgnored,
                                             &sNumParams,
                                             aErrorStack ) == STL_SUCCESS );
            aStmt->mParamCount = sNumParams;
        }
    }

    STL_RAMP( RAMP_ERR_ATOMIC );

    if( sDclAttrType == ZLL_DCL_TYPE_CLOSE_DATABASE )
    {
        if( sSentCommit == STL_TRUE )
        {
            (void)cmlReadTransactionResult( sHandle, aErrorStack );
        }
    }
    else
    {
        switch( aDbc->mTransition )
        {
            case ZLC_TRANSITION_STATE_C5 :
                if( aDbc->mAttrAutoCommit == STL_TRUE )
                {
                    if( sSentCommit == STL_TRUE )
                    {
                        sSentCommit = STL_FALSE;
                        STL_TRY( cmlReadTransactionResult( sHandle,
                                                           aErrorStack ) == STL_SUCCESS );

                        STL_TRY( zlrCloseWithoutHold( aDbc,
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
                switch( sDclAttrType )
                {
                    case ZLL_DCL_TYPE_END_TRANSACTION :
                        STL_TRY( zlrCloseWithoutHold( aDbc,
                                                      aErrorStack ) == STL_SUCCESS );
                        
                        aDbc->mTransition = ZLC_TRANSITION_STATE_C5;
                        break;
                    default:
                        if( (aDbc->mAttrAutoCommit == STL_TRUE) &&
                            (sSentCommit == STL_TRUE) )
                        {
                            sSentCommit = STL_FALSE;
                            STL_TRY( cmlReadTransactionResult( sHandle,
                                                               aErrorStack ) == STL_SUCCESS );
                            
                            STL_TRY( zlrCloseWithoutHold( aDbc,
                                                          aErrorStack ) == STL_SUCCESS );
                        }
                        break;
                }
                break;
            default:
                break;
        }
    }

    STL_TRY_THROW( sExecuteIgnored == STL_FALSE, RAMP_IGNORE );
    
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
                                   aErrorStack ) == STL_SUCCESS );
            
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          ZLE_ERRCODE_FAILED_TO_ATOMIC_EXECUTION,
                          NULL,
                          aErrorStack );
            
            STL_TRY( zldDiagAdds( SQL_HANDLE_STMT,
                                  (void*)aStmt,
                                  SQL_NO_ROW_NUMBER,
                                  SQL_NO_COLUMN_NUMBER,
                                  aErrorStack ) == STL_SUCCESS );
            
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
                                       aErrorStack ) == STL_SUCCESS );
                
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

    if( sDclAttrCount > 0 )
    {
        switch( sDclAttrType )
        {
            case ZLL_DCL_TYPE_SET_TIME_ZONE :
                aDbc->mTimezone = (stlInt16)sDclAttrValue;
                break;
            case ZLL_DCL_TYPE_OPEN_DATABASE :
                aDbc->mNlsCharacterSet =
                    (dtlCharacterSet)(sDclAttrValue % CML_NCHAR_CHARACTERSET_OFFSET);
                aDbc->mNlsNCharCharacterSet =
                    (dtlCharacterSet)(sDclAttrValue / CML_NCHAR_CHARACTERSET_OFFSET);

                STL_TRY( zlcSetNlsDTFuncVector( aDbc ) == STL_SUCCESS );
                break;
            case ZLL_DCL_TYPE_CLOSE_DATABASE :
                aDbc->mBrokenConnection = STL_TRUE;
                aDbc->mTransition       = ZLC_TRANSITION_STATE_C5;
            
                STL_THROW( RAMP_IGNORE );
                break;
            default:
                /**
                 * @todo ALTER SESSION SET DATE_FORMAT이 지원되면 DATE_FORMAT 정보도 필요하다.
                 */
                break;
        }
    }
        
    STL_RAMP( RAMP_IGNORE );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_BIND_TYPE )
    {
        for( i = i + 1; i < sInitParamCount; i++ )
        {
            (void)cmlReadBindTypeResult( sHandle,
                                         &sIgnored,
                                         &aStmt->mStmtId,
                                         aErrorStack );
        }
    }

    STL_CATCH( RAMP_ERR_BIND_DATA )
    {
        /* nothing to do */
    }

    STL_FINISH;

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
                sRollbacked = STL_TRUE;
                break;
            default :
                break;
        }
    }

    switch( sState )
    {
        case 6 :
            if( sExecParamCount > 0 )
            {
                (void)cmlReadBindDataResult( sHandle,
                                             &sIgnored,
                                             aErrorStack );
            }
        case 5 :
            (void)cmlReadExecDirectResult( sHandle,
                                           &sExecuteIgnored,
                                           &aStmt->mStmtId,
                                           &sStmtType,
                                           &sRowCount,
                                           &sIsRecompile,
                                           aResultSet,
                                           aIsWithoutHoldCursor,
                                           aIsUpdatable,
                                           &sSensitivity,
                                           &sHasTransaction,
                                           &sDclAttrCount,
                                           &sDclAttrType,
                                           &sDclAttrDataType,
                                           (stlChar*)&sDclAttrValue,
                                           &sDclAttrValueSize,
                                           aErrorStack );
        case 4 :
            if( sOutParameter == STL_TRUE )
            {
                (void)zlciGetOutParameterBlock( aStmt,
                                                &sIgnored,
                                                aErrorStack );
            }
        case 3 :
            if( sReqNumParams == STL_TRUE )
            {
                (void)cmlReadNumParamsResult( sHandle,
                                              &sIgnored,
                                              &sNumParams,
                                              aErrorStack );
            }
        case 2 :
            if( sReqTargetType == STL_TRUE )
            {
                (void)cmlReadTargetTypeResult( sHandle,
                                               &sIgnored,
                                               &sChanged,
                                               &sTargetCount,
                                               &sTargetType,
                                               aErrorStack );
            }
        case 1 :
            if( sReqFetch == STL_TRUE )
            {
                (void)zlcrResFetch( aStmt, aErrorStack );
            }
        default:
            break;
    }

    switch( aDbc->mTransition )
    {
        case ZLC_TRANSITION_STATE_C5 :
            if( aDbc->mAttrAutoCommit == STL_TRUE )
            {
                if( sSentCommit == STL_TRUE )
                {
                    sSentCommit = STL_FALSE;
                    (void)cmlReadTransactionResult( sHandle, aErrorStack );
                    (void)zlrCloseWithoutHold( aDbc, aErrorStack );
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
            if( (aDbc->mAttrAutoCommit == STL_TRUE) &&
                (sSentCommit == STL_TRUE) )
            {
                sSentCommit = STL_FALSE;
                (void)cmlReadTransactionResult( sHandle, aErrorStack );
                (void)zlrCloseWithoutHold( aDbc, aErrorStack );
            }
            break;
        default:
            break;
    }

    if( sRollbacked == STL_TRUE )
    {
        (void)zlrCloseWithoutHold( aDbc, aErrorStack );
        aDbc->mTransition = ZLC_TRANSITION_STATE_C5;
    }

    return STL_FAILURE;
}

/** @} */
