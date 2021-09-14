/*******************************************************************************
 * zlarResult.c
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
#include <zllRowStatusDef.h>
#include <zlDef.h>
#include <zlr.h>
#include <zle.h>
#include <zld.h>
#include <zli.h>
#include <zlvSqlToC.h>

/**
 * @file zlarResult.c
 * @brief Gliese API Internal Result Routines.
 */

/**
 * @addtogroup zlr
 * @{
 */

stlStatus zlarClose( zlcDbc        * aDbc,
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
    
    STL_TRY( sslCloseCursor( aStmt->mStmtId,
                             sEnv )
             == STL_SUCCESS );

    aStmt->mResult.mRowCount = ZLR_ROWCOUNT_UNKNOWN;

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

stlStatus zlarFetchInternal( zlcDbc             * aDbc,
                             stlInt64             aStatementId,
                             stlInt64             aOffset,
                             knlValueBlockList ** aTargetBuffer,
                             stlBool            * aIsEndOfScan,
                             stlBool            * aIsBlockRead,
                             stlInt32           * aOneRowIdx,
                             qllCursorRowStatus * aOneRowStatus,
                             stlInt64           * aAbsIdxOfFirstRow,
                             stlInt64           * aKnownLastRowIdx,
                             stlErrorStack      * aErrorStack )
{
    stlInt32    sState = 0;
    sslEnv    * sEnv;
    
    sEnv = SSL_WORKER_ENV( knlGetSessionEnv(aDbc->mSessionId) );
    
    STL_TRY( sslEnterSession( aDbc->mSessionId,
                              aDbc->mSessionSeq,
                              aErrorStack )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( sslFetch( aStatementId,
                       aOffset,
                       aTargetBuffer,
                       aIsEndOfScan,
                       aIsBlockRead,
                       aOneRowIdx,
                       aOneRowStatus,
                       aAbsIdxOfFirstRow,
                       aKnownLastRowIdx,
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
                     

stlStatus zlarFetch( zlcDbc        * aDbc,
                     zlsStmt       * aStmt,
                     stlUInt64       aArraySize,
                     stlInt64        aOffset,
                     stlBool       * aNotFound,
                     stlBool       * aSuccessWithInfo,
                     stlErrorStack * aErrorStack )
{
    zldDiag           * sDiag;
    zldDesc           * sArd;
    zldDesc           * sIrd;
    zldDescElement    * sArdRec;
    stlInt64            sBlockOffset = 0;
    stlInt64            sAbsOffset = 0;
    stlInt32            sColIdx;
    stlUInt64           i;
    stlStatus           sRet = STL_FAILURE;
    stlBool             sRowWithInfo;
    stlUInt64           sErrorRowCount;
    stlUInt64           sWarningRowCount;
    void              * sBuffer;
    SQLLEN            * sIndicator;
    dtlValueBlockList * sBlockList = NULL;
    dtlDataValue      * sDataValue = NULL;
    SQLRETURN           sReturn = SQL_SUCCESS;
    zlrResult         * sResult;

    stlBool             sIsBlockRead = STL_FALSE;
    stlInt32            sOneRowIdx = 0;

    SQLLEN              sOffset = 0;
    qllCursorRowStatus  sRowStatus;

    stlInt64            sLastResultRow;

    STL_PARAM_VALIDATE( aNotFound != NULL, aErrorStack );

    sDiag   = &aStmt->mDiag;
    sArd    = aStmt->mArd;
    sIrd    = &aStmt->mIrd;
    sResult = &aStmt->mResult;

    *aNotFound = STL_FALSE;

    STL_TRY( zlrInitRowStatusArray( aStmt ) == STL_SUCCESS );

    if( aStmt->mExplainPlanType == SQL_EXPLAIN_PLAN_ONLY )
    {
        sResult->mLastRowIdx = 0;
        
        STL_THROW( RAMP_NO_DATA_AFTER_END );
    }
    
    sErrorRowCount = 0;
    sWarningRowCount = 0;

    for( i = 0; i < aArraySize; i++ )
    {
        sColIdx = -1;

        sRowWithInfo = STL_FALSE;
        sRowStatus   = QLL_CURSOR_ROW_STATUS_NO_CHANGE;

        /* 원하는 레코드가 rowset에 없는 경우 */
        if( ( aStmt->mCursorSensitivity == SQL_SENSITIVE ) ||
            ( sResult->mRowsetFirstRowIdx == 0 ) ||
            ( aOffset < sResult->mRowsetFirstRowIdx ) ||
            ( aOffset >= sResult->mRowsetFirstRowIdx + sResult->mBlockLastRow ) )
        {
            if( sResult->mIsEndOfScan == STL_TRUE )
            {
                break;
            }
            
            sBlockOffset = aOffset;

            STL_TRY_THROW( zlarFetchInternal( aDbc,
                                              aStmt->mStmtId,
                                              sBlockOffset,
                                              &sResult->mTargetBuffer,
                                              &sResult->mIsEndOfScan,
                                              &sIsBlockRead,
                                              &sOneRowIdx,
                                              &sRowStatus,
                                              &sResult->mRowsetFirstRowIdx,
                                              &sResult->mLastRowIdx,
                                              aErrorStack ) == STL_SUCCESS,
                           RAMP_FETCH_ERROR );

            if( sResult->mLastRowIdx != SSL_LAST_ROW_IDX_UNKNOWN )
            {
                sAbsOffset = stlAbsInt64(sBlockOffset);

                if( aStmt->mMaxRows == 0 )
                {
                    sLastResultRow = sResult->mLastRowIdx;
                }
                else
                {
                    sLastResultRow = STL_MIN(sResult->mLastRowIdx, aStmt->mMaxRows);
                }

                if( (sAbsOffset == 0) || (sAbsOffset > sLastResultRow) )
                {
                    if( aOffset <= 0 )
                    {
                        STL_THROW( RAMP_NO_DATA_BEFORE_START );
                    }
                    else
                    {
                        STL_TRY_THROW( i != 0, RAMP_NO_DATA_AFTER_END );
                        break;
                    }
                }
            }

            if( aOffset < 0 )
            {
                sBlockOffset = sResult->mLastRowIdx + sBlockOffset + 1;

                if( aStmt->mMaxRows != 0 )
                {
                    if( sBlockOffset > aStmt->mMaxRows )
                    {
                        sBlockOffset = aStmt->mMaxRows + aOffset + 1;

                        STL_TRY_THROW( zlarFetchInternal( aDbc,
                                                          aStmt->mStmtId,
                                                          sBlockOffset,
                                                          &sResult->mTargetBuffer,
                                                          &sResult->mIsEndOfScan,
                                                          &sIsBlockRead,
                                                          &sOneRowIdx,
                                                          &sRowStatus,
                                                          &sResult->mRowsetFirstRowIdx,
                                                          &sResult->mLastRowIdx,
                                                          aErrorStack ) == STL_SUCCESS,
                                       RAMP_FETCH_ERROR );
                    }
                }
            }

            if ( sIsBlockRead == STL_TRUE )
            {
                sResult->mBlockCurrentRow
                    = DTL_GET_BLOCK_SKIP_CNT( sResult->mTargetBuffer );
                sResult->mBlockLastRow
                    = DTL_GET_BLOCK_USED_CNT( sResult->mTargetBuffer );

                if( sBlockOffset > 0 )
                {
                    STL_TRY_THROW( sResult->mBlockLastRow > 0, RAMP_NO_DATA_AFTER_END );
                    
                    if( sBlockOffset != sResult->mRowsetFirstRowIdx )
                    {
                        /* rowset의 첫번째 row가 요청한 offset 위치의 row가 아니라면
                         * offset보다 이전의 row여야 한다. */
                        STL_DASSERT( sBlockOffset > sResult->mRowsetFirstRowIdx );

                        /* 이 경우 offset 이전의 row들은 버린다. */
                        sResult->mBlockCurrentRow = sBlockOffset - sResult->mRowsetFirstRowIdx;
                    }

                    /* sSkipCount가 sUsedCount와 같다면,
                     * rowset에 원하는 offset의 row가 없다는 의미이다. */
                    STL_DASSERT( sResult->mBlockLastRow > sResult->mBlockCurrentRow );
                }
                else
                {
                    STL_TRY_THROW( sResult->mBlockLastRow > 0, RAMP_NO_DATA_BEFORE_START );
                    
                    /* offset이 음수값일 경우 현재 rowset의 첫번째 번호로 current로 설정한다. */
                    sResult->mCurrentRowIdx = sResult->mRowsetFirstRowIdx;
                }

                if( aStmt->mMaxRows != 0 )
                {
                    if( (sResult->mRowsetFirstRowIdx + sResult->mBlockCurrentRow) > aStmt->mMaxRows )
                    {
                        sResult->mLastRowIdx = aStmt->mMaxRows;

                        STL_THROW( RAMP_NO_DATA_AFTER_END );
                    }
                }

                sBlockOffset += (sResult->mBlockLastRow - sResult->mBlockCurrentRow);
            }
            else
            {
                if( sOneRowIdx < 0 )
                {
                    if( aOffset <= 0 )
                    {
                        STL_THROW( RAMP_NO_DATA_BEFORE_START );
                    }
                    else
                    {
                        STL_THROW( RAMP_NO_DATA_AFTER_END );
                    }
                }
                
                sResult->mBlockCurrentRow = sOneRowIdx;
                sResult->mBlockLastRow    = sOneRowIdx;

                if( aStmt->mMaxRows != 0 )
                {
                    if( sResult->mRowsetFirstRowIdx > aStmt->mMaxRows )
                    {
                        sResult->mLastRowIdx = aStmt->mMaxRows;

                        STL_THROW( RAMP_NO_DATA_AFTER_END );
                    }
                }

                sBlockOffset += 1;
            }
        }
        else
        {
            sResult->mBlockCurrentRow = aOffset - sResult->mRowsetFirstRowIdx;

            if( aStmt->mMaxRows != 0 )
            {
                if( (sResult->mRowsetFirstRowIdx + sResult->mBlockCurrentRow) > aStmt->mMaxRows )
                {
                    sResult->mLastRowIdx = aStmt->mMaxRows;

                    STL_THROW( RAMP_NO_DATA_AFTER_END );
                }
            }
        }

        if( sRowStatus != QLL_CURSOR_ROW_STATUS_DELETED )
        {
            if( aStmt->mRetrieveData == STL_TRUE )
            {
                sBlockList = sResult->mTargetBuffer;

                STL_RING_FOREACH_ENTRY( &sArd->mDescRecRing, sArdRec )
                {
                    if( sBlockList == NULL )
                    {
                        break;
                    }

                    sOffset = 0;

                    sDataValue = DTL_GET_BLOCK_DATA_VALUE( sBlockList, sResult->mBlockCurrentRow );

                    sColIdx++;

                    /*
                     * bind가 안되어 있음
                     */
                    if( sArdRec->mDataPtr == NULL )
                    {
                        sBlockList = sBlockList->mNext;
                        continue;
                    }

                    if( sArdRec->mType == SQL_C_DEFAULT )
                    {
                        STL_TRY( zlrGetDefaultCType( aStmt,
                                                     sDataValue,
                                                     sResult,
                                                     sArdRec,
                                                     aErrorStack ) == STL_SUCCESS );
                    }

                    sBuffer = zliGetDataBuffer( sArd,
                                                sArdRec,
                                                i + 1 );

                    sIndicator = zliGetIndicatorBuffer( sArd,
                                                        sArdRec,
                                                        i + 1 );

                    if( DTL_IS_NULL( sDataValue ) == STL_FALSE )
                    {
                        if( sArdRec->mSqlToCFunc == zlvInvalidSqlToC )
                        {
                            STL_TRY( zlvGetSqlToCFunc( aStmt,
                                                       sDataValue,
                                                       sArdRec->mConsiceType,
                                                       &sArdRec->mSqlToCFunc,
                                                       aErrorStack ) == STL_SUCCESS );
                        }
                    
                        if( sArdRec->mSqlToCFunc( aStmt,
                                                  sBlockList,
                                                  sDataValue,
                                                  &sOffset,
                                                  sBuffer,
                                                  sIndicator,
                                                  sArdRec,
                                                  &sReturn,
                                                  aErrorStack ) == STL_SUCCESS )
                        {
                            switch( sReturn )
                            {
                                case SQL_SUCCESS:
                                    break;
                                case SQL_SUCCESS_WITH_INFO:
                                    sRowWithInfo = STL_TRUE;

                                    zldDiagAdds( SQL_HANDLE_STMT,
                                                 (void*)aStmt,
                                                 i + 1,
                                                 sColIdx + 1,
                                                 aErrorStack );

                                    sWarningRowCount ++;
                                    break;
                                default:
                                    STL_ASSERT( STL_FALSE );
                                    break;
                            }
                        }
                        else
                        {
                            zldDiagAdds( SQL_HANDLE_STMT,
                                         (void*)aStmt,
                                         i + 1,
                                         sColIdx + 1,
                                         aErrorStack );

                            if( sIrd->mArrayStatusPtr != NULL )
                            {
                                sIrd->mArrayStatusPtr[i] = SQL_ROW_ERROR;
                            }

                            sErrorRowCount++;
                            break;
                        }
                    }
                    else
                    {
                        if( sIndicator == NULL )
                        {
                            zlrAddNullIndicatorError( aStmt,
                                                      i + 1,
                                                      sColIdx + 1,
                                                      aErrorStack );
                            sErrorRowCount++;
                            break;
                        }
                        else
                        {
                            *sIndicator = SQL_NULL_DATA;
                        }
                    }

                    sBlockList = sBlockList->mNext;
                }
            }
        }

        if( sIrd->mArrayStatusPtr != NULL )
        {
            switch( sRowStatus )
            {
                case QLL_CURSOR_ROW_STATUS_NO_CHANGE :
                    if( sRowWithInfo == STL_TRUE )
                    {
                        sIrd->mArrayStatusPtr[i] = SQL_ROW_SUCCESS_WITH_INFO;
                    }
                    else
                    {
                        sIrd->mArrayStatusPtr[i] = SQL_ROW_SUCCESS;
                    }
                    break;
                case QLL_CURSOR_ROW_STATUS_INSERTED :
                    sIrd->mArrayStatusPtr[i] = SQL_ROW_ADDED;
                    break;
                case QLL_CURSOR_ROW_STATUS_UPDATED :
                    sIrd->mArrayStatusPtr[i] = SQL_ROW_UPDATED;
                    break;
                case QLL_CURSOR_ROW_STATUS_DELETED :
                    sIrd->mArrayStatusPtr[i] = SQL_ROW_DELETED;
                    break;
                default :
                    STL_ASSERT( STL_FALSE );
                    break;
            }
        }

        aOffset++;
        zldDiagSetCursorRowCount( sDiag, aOffset );
    }

    if( sIrd->mRowProcessed != NULL )
    {
        *sIrd->mRowProcessed = i;
    }

    STL_TRY_THROW( aArraySize != sErrorRowCount,
                   RAMP_FAILURE );

    if( aSuccessWithInfo != NULL )
    {
        if( ( sWarningRowCount + sErrorRowCount ) == 0 )
        {
            *aSuccessWithInfo = STL_FALSE;
        }
        else
        {
            *aSuccessWithInfo = STL_TRUE;
        }
    }

    sResult->mPosition = sResult->mCurrentRowIdx;
    
    sRet = STL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA_BEFORE_START )
    {
        *aNotFound = STL_TRUE;

        sResult->mPosition        = 0;
        sResult->mBlockCurrentRow = ZLR_ROWSET_BEFORE_START;

        sRet = STL_SUCCESS;
    }

    STL_CATCH( RAMP_NO_DATA_AFTER_END )
    {
        *aNotFound = STL_TRUE;

        sResult->mPosition        = 0;
        sResult->mBlockCurrentRow = ZLR_ROWSET_AFTER_END;

        sRet = STL_SUCCESS;
    }

    STL_CATCH( RAMP_FETCH_ERROR )
    {
        sResult->mPosition        = 0;
        sResult->mStatus          = QLL_CURSOR_ROW_STATUS_NA;
        sResult->mBlockCurrentRow = ZLR_ROWSET_AFTER_END;
    }

    STL_CATCH( RAMP_FAILURE )
    {
        sRet = STL_FAILURE;
    }

    STL_FINISH;

    return sRet;
}

stlStatus zlarSetCursorName( zlcDbc        * aDbc,
                             zlsStmt       * aStmt,
                             stlChar       * aName,
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
    
    STL_TRY( sslSetCursorName( aStmt->mStmtId,
                               aName,
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

stlStatus zlarGetCursorName( zlcDbc        * aDbc,
                             zlsStmt       * aStmt,
                             stlErrorStack * aErrorStack )
{
    stlInt32           sState = 0;
    sslEnv           * sEnv;
    
    STL_TRY( sslEnterSession( aDbc->mSessionId,
                              aDbc->mSessionSeq,
                              aErrorStack )
             == STL_SUCCESS );
    sState = 1;

    sEnv = SSL_WORKER_ENV( knlGetSessionEnv(aDbc->mSessionId) );
    
    STL_TRY( sslGetCursorName( aStmt->mStmtId,
                               aStmt->mCursorName,
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

stlStatus zlarGetData( zlcDbc        * aDbc,
                       zlsStmt       * aStmt,
                       zlrResult     * aResult,
                       stlBool       * aEOF,
                       stlErrorStack * aErrorStack )
{
    stlBool            sIsBlockRead  = STL_FALSE;
    stlInt32           sOneRowIdx    = 0;
    stlStatus          sRet          = STL_FAILURE;
    qllCursorRowStatus sOneRowStatus = QLL_CURSOR_ROW_STATUS_NA;
    
    *aEOF = STL_FALSE;

    STL_TRY_THROW( zlarFetchInternal( aDbc,
                                      aStmt->mStmtId,
                                      aResult->mPosition,
                                      &aResult->mTargetBuffer,
                                      &aResult->mIsEndOfScan,
                                      &sIsBlockRead,
                                      &sOneRowIdx,
                                      &sOneRowStatus,
                                      &aResult->mRowsetFirstRowIdx,
                                      &aResult->mLastRowIdx,
                                      aErrorStack ) == STL_SUCCESS,
                   RAMP_FETCH_ERROR );

    aResult->mStatus = (stlInt8)sOneRowStatus;

    if( aResult->mLastRowIdx != -1 )
    {
        STL_TRY_THROW( aResult->mPosition <= aResult->mLastRowIdx, RAMP_NO_DATA );
    }

    if ( sIsBlockRead == STL_TRUE )
    {
        aResult->mBlockCurrentRow = DTL_GET_BLOCK_SKIP_CNT( aResult->mTargetBuffer );
        aResult->mBlockLastRow    = DTL_GET_BLOCK_USED_CNT( aResult->mTargetBuffer );

        STL_TRY_THROW( aResult->mBlockLastRow > 0, RAMP_NO_DATA );
                    
        if( aResult->mPosition != aResult->mRowsetFirstRowIdx )
        {
            /* rowset의 첫번째 row가 요청한 offset 위치의 row가 아니라면
             * offset보다 이전의 row여야 한다. */
            STL_DASSERT( aResult->mPosition > aResult->mRowsetFirstRowIdx );

            /* 이 경우 offset 이전의 row들은 버린다. */
            aResult->mBlockCurrentRow = aResult->mPosition - aResult->mRowsetFirstRowIdx;
        }

        /* sSkipCount가 sUsedCount와 같다면,
         * rowset에 원하는 offset의 row가 없다는 의미이다. */
        STL_DASSERT( aResult->mBlockLastRow > aResult->mBlockCurrentRow );
    }
    else
    {
        STL_TRY_THROW( sOneRowIdx >= 0, RAMP_NO_DATA );
                
        aResult->mBlockCurrentRow = sOneRowIdx;
        aResult->mBlockLastRow    = sOneRowIdx;
    }

    sRet = STL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA )
    {
        *aEOF = STL_TRUE;
        sRet  = STL_SUCCESS;
    }

    STL_CATCH( RAMP_FETCH_ERROR )
    {
        aResult->mStatus          = ZLL_CURSOR_ROW_STATUS_NA;
        aResult->mBlockCurrentRow = ZLR_ROWSET_AFTER_END;
    }
    
    STL_FINISH;

    return sRet;
}

stlStatus zlarFetch4Imp( zlcDbc             * aDbc,
                         zlsStmt            * aStmt,
                         dtlValueBlockList ** aDataPtr,
                         stlBool            * aNotFound,
                         stlErrorStack      * aErrorStack )
{
    stlStatus           sRet = STL_FAILURE;
    zlrResult         * sResult;

    stlBool             sIsBlockRead = STL_FALSE;
    stlInt32            sOneRowIdx = 0;
    qllCursorRowStatus  sRowStatus;
    stlInt64            sDummyFirstRowIdx;
    stlInt64            sDummyLastRowIdx;

    STL_PARAM_VALIDATE( aNotFound != NULL, aErrorStack );

    sResult = &aStmt->mResult;

    *aNotFound = STL_FALSE;

    STL_TRY_THROW( sResult->mIsEndOfScan != STL_TRUE, RAMP_NO_DATA );

    /* export를 위한 fetch에서 fetchScroll일 경우는 없다. 따라서 offset는 사용될 일이 없다.*/
    STL_TRY_THROW( zlarFetchInternal( aDbc,
                                      aStmt->mStmtId,
                                      sResult->mCurrentRowIdx,
                                      &sResult->mTargetBuffer,
                                      &sResult->mIsEndOfScan,
                                      &sIsBlockRead,
                                      &sOneRowIdx,
                                      &sRowStatus,
                                      &sDummyFirstRowIdx,
                                      &sDummyLastRowIdx,
                                      aErrorStack ) == STL_SUCCESS,
                   RAMP_FETCH_ERROR );

    if ( sIsBlockRead == STL_TRUE )
    {
        sResult->mBlockCurrentRow = DTL_GET_BLOCK_SKIP_CNT( sResult->mTargetBuffer );
        sResult->mBlockLastRow    = DTL_GET_BLOCK_USED_CNT( sResult->mTargetBuffer );
    }
    else
    {
        STL_ASSERT( STL_FALSE );
    }

    if( aDataPtr != NULL )
    {
        *aDataPtr = sResult->mTargetBuffer;
    }

    sRet = STL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA )
    {
        *aNotFound = STL_TRUE;

        sResult->mBlockCurrentRow = ZLR_ROWSET_AFTER_END;

        sRet = STL_SUCCESS;
    }

    STL_CATCH( RAMP_FETCH_ERROR )
    {
        sResult->mStatus          = ZLL_CURSOR_ROW_STATUS_NA;
        sResult->mBlockCurrentRow = ZLR_ROWSET_AFTER_END;
    }

    STL_FINISH;

    return sRet;
}

stlStatus zlarSetPos( zlcDbc        * aDbc,
                      zlsStmt       * aStmt,
                      stlInt64        aRowNumber,
                      stlInt16        aOperation,
                      stlUInt16       aLockType,
                      stlErrorStack * aErrorStack )
{
    stlInt32           sState = 0;
    sslEnv           * sEnv;
    
    STL_TRY( sslEnterSession( aDbc->mSessionId,
                              aDbc->mSessionSeq,
                              aErrorStack )
             == STL_SUCCESS );
    sState = 1;

    sEnv = SSL_WORKER_ENV( knlGetSessionEnv(aDbc->mSessionId) );
    
    STL_TRY( sslSetPos( aStmt->mStmtId,
                        aRowNumber,
                        aOperation,
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
