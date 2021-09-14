/*******************************************************************************
 * zlcrResult.c
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
#include <zllRowStatusDef.h>
#include <zlDef.h>
#include <zlr.h>
#include <zle.h>
#include <zld.h>
#include <zli.h>
#include <zlvSqlToC.h>

/**
 * @file zlcrResult.c
 * @brief ODBC API Internal Result Routines.
 */

/**
 * @addtogroup zlr
 * @{
 */

stlStatus zlcrClose( zlcDbc        * aDbc,
                     zlsStmt       * aStmt,
                     stlErrorStack * aErrorStack )
{
    cmlHandle * sHandle;
    zlrResult * sResult;
    stlBool     sIgnored;

    sHandle = &aDbc->mComm;

    sResult = &aStmt->mResult;

    if( (sResult->mIsEndOfScan == STL_FALSE) &&
        (aStmt->mStmtId != ZLS_INVALID_STATEMENT_ID) )
    {
        STL_TRY( cmlWriteCloseCursorCommand( sHandle,
                                             CML_CONTROL_HEAD | CML_CONTROL_TAIL,
                                             aStmt->mStmtId,
                                             aErrorStack ) == STL_SUCCESS );

        STL_TRY( cmlSendPacket( sHandle,
                                aErrorStack ) == STL_SUCCESS );

        STL_TRY( cmlReadCloseCursorResult( sHandle,
                                           &sIgnored,
                                           aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlcrReallocLongVaryingMem( void          * aHandle,
                                     dtlDataValue  * aDataValue,
                                     void          * aCallbackContext,
                                     stlErrorStack * aErrorStack )
{
    zlsStmt * sStmt = (zlsStmt*)aCallbackContext;

    STL_TRY( stlFreeDynamicMem( &sStmt->mLongVaryingMem,
                                aDataValue->mValue,
                                aErrorStack )
             == STL_SUCCESS );

    STL_TRY( stlAllocDynamicMem( &sStmt->mLongVaryingMem,
                                 aDataValue->mLength,
                                 (void**)&aDataValue->mValue,
                                 aErrorStack )
             == STL_SUCCESS );

    aDataValue->mBufferSize = aDataValue->mLength;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlcrReqFetch( zlsStmt       * aStmt,
                        stlErrorStack * aErrorStack )
{
    cmlHandle * sHandle;
    zlcDbc    * sDbc;
    zlrResult * sResult;

    sDbc    = aStmt->mParentDbc;
    sHandle = &sDbc->mComm;

    sResult = &aStmt->mResult;
    
    STL_TRY_THROW( cmlWriteFetchCommand( sHandle,
                                         CML_CONTROL_TAIL,
                                         aStmt->mStmtId,
                                         1,
                                         aStmt->mPreFetchSize,
                                         aErrorStack ) == STL_SUCCESS,
                   RAMP_FETCH_ERROR );

    return STL_SUCCESS;

    STL_CATCH( RAMP_FETCH_ERROR )
    {
        sResult->mStatus          = ZLL_CURSOR_ROW_STATUS_NA;
        sResult->mBlockCurrentRow = ZLR_ROWSET_AFTER_END;
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlcrResFetch( zlsStmt       * aStmt,
                        stlErrorStack * aErrorStack )
{
    cmlHandle * sHandle;
    zlcDbc    * sDbc;
    zlrResult * sResult;
    stlBool     sIgnored;
    stlStatus   sRet = STL_FAILURE;

    sDbc    = aStmt->mParentDbc;
    sHandle = &sDbc->mComm;
    
    sResult = &aStmt->mResult;
    sResult->mStatus = ZLL_CURSOR_ROW_STATUS_NO_CHANGE;

    if( sResult->mTargetBuffer != NULL )
    {
        DTL_SET_ONE_BLOCK_USED_CNT( sResult->mTargetBuffer, 0 );
        DTL_SET_ONE_BLOCK_SKIP_CNT( sResult->mTargetBuffer, 0 );
    }

    STL_TRY_THROW( cmlReadFetchResult( sHandle,
                                       &sIgnored,
                                       sResult->mTargetBuffer,
                                       &sResult->mRowsetFirstRowIdx,
                                       &sResult->mLastRowIdx,
                                       &sResult->mStatus,
                                       &sResult->mIsEndOfScan,
                                       zlcrReallocLongVaryingMem,
                                       (void*)aStmt,
                                       aErrorStack ) == STL_SUCCESS,
                   RAMP_FETCH_ERROR );

    STL_TRY_THROW( sIgnored == STL_FALSE, RAMP_IGNORE );

    sResult->mBlockCurrentRow = ZLR_ROWSET_BEFORE_START;
    sResult->mBlockLastRow    = DTL_GET_BLOCK_USED_CNT( sResult->mTargetBuffer );
    sResult->mCurrentRowIdx   = 0;

    STL_TRY_THROW( sResult->mBlockLastRow > 0, RAMP_NO_DATA );

    sRet = STL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_FETCH_ERROR )
    {
        sResult->mStatus          = ZLL_CURSOR_ROW_STATUS_NA;
        sResult->mBlockCurrentRow = ZLR_ROWSET_AFTER_END;
    }

    STL_CATCH( RAMP_IGNORE )
    {
        ZLR_INIT_RESULT( aStmt->mArd, sResult );

        sRet = STL_SUCCESS;
    }
    
    STL_CATCH( RAMP_NO_DATA )
    {
        sResult->mBlockCurrentRow = ZLR_ROWSET_AFTER_END;

        sRet = STL_SUCCESS;
    }
    
    STL_FINISH;

    return sRet;
}

stlStatus zlcrFetch( zlcDbc        * aDbc,
                     zlsStmt       * aStmt,
                     stlUInt64       aArraySize,
                     stlInt64        aOffset,
                     stlBool       * aNotFound,
                     stlBool       * aSuccessWithInfo,
                     stlErrorStack * aErrorStack )
{
    cmlHandle         * sHandle;
    zldDiag           * sDiag;
    zldDesc           * sArd;
    zldDesc           * sIrd;
    zldDescElement    * sArdRec;
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

    SQLLEN              sOffset = 0;
    stlBool             sIgnored;

    stlInt64            sFetchPos;
    stlInt64            sLastResultRow;
    stlInt64            sAbsOffset = 0;
    
    STL_PARAM_VALIDATE( aNotFound != NULL, aErrorStack );

    sHandle = &aDbc->mComm;

    sDiag   = &aStmt->mDiag;
    sArd    = aStmt->mArd;
    sIrd    = &aStmt->mIrd;
    sResult = &aStmt->mResult;

    *aNotFound = STL_FALSE;

    STL_TRY( zlrInitRowStatusArray( aStmt ) == STL_SUCCESS );

    STL_TRY_THROW( aStmt->mExplainPlanType != SQL_EXPLAIN_PLAN_ONLY,
                   RAMP_NO_DATA_AFTER_END );
    
    sErrorRowCount = 0;
    sWarningRowCount = 0;

    for( i = 0; i < aArraySize; i++ )
    {
        sColIdx = -1;

        sRowWithInfo     = STL_FALSE;
        sResult->mStatus = ZLL_CURSOR_ROW_STATUS_NO_CHANGE;

        /* 원하는 레코드가 rowset에 없는 경우 */
        if( ( aStmt->mCursorSensitivity == SQL_SENSITIVE ) ||
            ( sResult->mRowsetFirstRowIdx == 0 ) ||
            ( aOffset < sResult->mRowsetFirstRowIdx ) ||
            ( aOffset >= sResult->mRowsetFirstRowIdx + sResult->mBlockLastRow ) )
        {
            if( sResult->mLastRowIdx != -1 )
            {
                if( aOffset > sResult->mLastRowIdx )
                {
                    break;
                }
            }
            
            DTL_SET_ONE_BLOCK_USED_CNT( sResult->mTargetBuffer, 0 );
            DTL_SET_ONE_BLOCK_SKIP_CNT( sResult->mTargetBuffer, 0 );

            STL_TRY_THROW( cmlWriteFetchCommand( sHandle,
                                                 CML_CONTROL_HEAD | CML_CONTROL_TAIL,
                                                 aStmt->mStmtId,
                                                 aOffset,
                                                 aStmt->mPreFetchSize,
                                                 aErrorStack ) == STL_SUCCESS,
                           RAMP_FETCH_ERROR );

            STL_TRY( cmlSendPacket( sHandle,
                                    aErrorStack ) == STL_SUCCESS );

            STL_TRY_THROW( cmlReadFetchResult( sHandle,
                                               &sIgnored,
                                               sResult->mTargetBuffer,
                                               &sResult->mRowsetFirstRowIdx,
                                               &sResult->mLastRowIdx,
                                               &sResult->mStatus,
                                               &sResult->mIsEndOfScan,
                                               zlcrReallocLongVaryingMem,
                                               (void*)aStmt,
                                               aErrorStack ) == STL_SUCCESS,
                           RAMP_FETCH_ERROR );

            
            if( sResult->mLastRowIdx != -1 )
            {
                sAbsOffset = stlAbsInt64(aOffset);

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
                        STL_THROW( RAMP_NO_DATA_AFTER_END );
                    }
                }
            }
            
            if( aOffset < 0 )
            {
                sFetchPos = sResult->mLastRowIdx + aOffset + 1;

                if( aStmt->mMaxRows != 0 )
                {
                    if( sFetchPos > aStmt->mMaxRows )
                    {
                        sFetchPos = aStmt->mMaxRows + aOffset + 1;

                        DTL_SET_ONE_BLOCK_USED_CNT( sResult->mTargetBuffer, 0 );
                        DTL_SET_ONE_BLOCK_SKIP_CNT( sResult->mTargetBuffer, 0 );

                        STL_TRY_THROW( cmlWriteFetchCommand( sHandle,
                                                             CML_CONTROL_HEAD | CML_CONTROL_TAIL,
                                                             aStmt->mStmtId,
                                                             sFetchPos,
                                                             aStmt->mPreFetchSize,
                                                             aErrorStack ) == STL_SUCCESS,
                                       RAMP_FETCH_ERROR );

                        STL_TRY( cmlSendPacket( sHandle,
                                                aErrorStack ) == STL_SUCCESS );

                        STL_TRY_THROW( cmlReadFetchResult( sHandle,
                                                           &sIgnored,
                                                           sResult->mTargetBuffer,
                                                           &sResult->mRowsetFirstRowIdx,
                                                           &sResult->mLastRowIdx,
                                                           &sResult->mStatus,
                                                           &sResult->mIsEndOfScan,
                                                           zlcrReallocLongVaryingMem,
                                                           (void*)aStmt,
                                                           aErrorStack ) == STL_SUCCESS,
                                       RAMP_FETCH_ERROR );
                    }
                }

                aOffset = sFetchPos;
            }

            sResult->mBlockCurrentRow = DTL_GET_BLOCK_SKIP_CNT( sResult->mTargetBuffer );
            sResult->mBlockLastRow    = DTL_GET_BLOCK_USED_CNT( sResult->mTargetBuffer );

            if( sResult->mBlockLastRow == 0 )
            {
                if( i == 0 )
                {
                    if( aOffset < 0 )
                    {
                        STL_THROW( RAMP_NO_DATA_BEFORE_START );
                    }
                    else
                    {
                        STL_THROW( RAMP_NO_DATA_AFTER_END );
                    }
                }
                else
                {
                    break;
                }
            }
        }
        else
        {
            sResult->mBlockCurrentRow = aOffset - sResult->mRowsetFirstRowIdx;
        }

        if( aStmt->mMaxRows != 0 )
        {
            if( (sResult->mRowsetFirstRowIdx + sResult->mBlockCurrentRow) > aStmt->mMaxRows )
            {
                sResult->mLastRowIdx = aStmt->mMaxRows;

                STL_THROW( RAMP_NO_DATA_AFTER_END );
            }
        }
        
        if( sResult->mStatus != ZLL_CURSOR_ROW_STATUS_DELETED )
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
            switch( sResult->mStatus )
            {
                case ZLL_CURSOR_ROW_STATUS_NO_CHANGE :
                    if( sRowWithInfo == STL_TRUE )
                    {
                        sIrd->mArrayStatusPtr[i] = SQL_ROW_SUCCESS_WITH_INFO;
                    }
                    else
                    {
                        sIrd->mArrayStatusPtr[i] = SQL_ROW_SUCCESS;
                    }
                    break;
                case ZLL_CURSOR_ROW_STATUS_INSERTED :
                    sIrd->mArrayStatusPtr[i] = SQL_ROW_ADDED;
                    break;
                case ZLL_CURSOR_ROW_STATUS_UPDATED :
                    sIrd->mArrayStatusPtr[i] = SQL_ROW_UPDATED;
                    break;
                case ZLL_CURSOR_ROW_STATUS_DELETED :
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
        sResult->mStatus          = ZLL_CURSOR_ROW_STATUS_NA;
        sResult->mBlockCurrentRow = ZLR_ROWSET_AFTER_END;
    }

    STL_CATCH( RAMP_FAILURE )
    {
        sRet = STL_FAILURE;
    }

    STL_FINISH;

    return sRet;
}

stlStatus zlcrSetCursorName( zlcDbc        * aDbc,
                             zlsStmt       * aStmt,
                             stlChar       * aName,
                             stlErrorStack * aErrorStack )
{
    cmlHandle * sHandle  = &aDbc->mComm;
    stlBool     sIgnored = STL_FALSE;
    
    STL_TRY( cmlWriteSetCursorNameCommand( sHandle,
                                           CML_CONTROL_HEAD | CML_CONTROL_TAIL,
                                           aStmt->mStmtId,
                                           aName,
                                           aErrorStack ) == STL_SUCCESS );

    STL_TRY( cmlSendPacket( sHandle,
                            aErrorStack ) == STL_SUCCESS );

    STL_TRY( cmlReadSetCursorNameResult( sHandle,
                                         &sIgnored,
                                         &aStmt->mStmtId,
                                         aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlcrGetCursorName( zlcDbc        * aDbc,
                             zlsStmt       * aStmt,
                             stlErrorStack * aErrorStack )
{
    cmlHandle * sHandle  = &aDbc->mComm;
    stlBool     sIgnored = STL_FALSE;

    STL_TRY( cmlWriteGetCursorNameCommand( sHandle,
                                           CML_CONTROL_HEAD | CML_CONTROL_TAIL,
                                           aStmt->mStmtId,
                                           aErrorStack ) == STL_SUCCESS );

    STL_TRY( cmlSendPacket( sHandle,
                            aErrorStack ) == STL_SUCCESS );

    STL_TRY( cmlReadGetCursorNameResult( sHandle,
                                         &sIgnored,
                                         &aStmt->mStmtId,
                                         aStmt->mCursorName,
                                         aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlcrGetData( zlcDbc        * aDbc,
                       zlsStmt       * aStmt,
                       zlrResult     * aResult,
                       stlBool       * aEOF,
                       stlErrorStack * aErrorStack )
{
    cmlHandle * sHandle  = &aDbc->mComm;
    stlBool     sIgnored = STL_FALSE;
    stlStatus   sRet     = STL_FAILURE;

    *aEOF = STL_FALSE;
    
    DTL_SET_ONE_BLOCK_USED_CNT( aResult->mTargetBuffer, 0 );
    DTL_SET_ONE_BLOCK_SKIP_CNT( aResult->mTargetBuffer, 0 );

    STL_TRY_THROW( cmlWriteFetchCommand( sHandle,
                                         CML_CONTROL_HEAD | CML_CONTROL_TAIL,
                                         aStmt->mStmtId,
                                         aResult->mPosition,
                                         aStmt->mPreFetchSize,
                                         aErrorStack ) == STL_SUCCESS,
                   RAMP_FETCH_ERROR );

    STL_TRY( cmlSendPacket( sHandle,
                            aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( cmlReadFetchResult( sHandle,
                                       &sIgnored,
                                       aResult->mTargetBuffer,
                                       &aResult->mRowsetFirstRowIdx,
                                       &aResult->mLastRowIdx,
                                       &aResult->mStatus,
                                       &aResult->mIsEndOfScan,
                                       zlcrReallocLongVaryingMem,
                                       (void*)aStmt,
                                       aErrorStack ) == STL_SUCCESS,  RAMP_FETCH_ERROR );

    aResult->mBlockCurrentRow = DTL_GET_BLOCK_SKIP_CNT( aResult->mTargetBuffer );
    aResult->mBlockLastRow    = DTL_GET_BLOCK_USED_CNT( aResult->mTargetBuffer );

    STL_TRY_THROW( aResult->mBlockLastRow != 0, RAMP_NO_DATA );

    sRet = STL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_NO_DATA )
    {
        *aEOF = STL_TRUE;
        sRet = STL_SUCCESS;
    }

    STL_CATCH( RAMP_FETCH_ERROR )
    {
        aResult->mStatus          = ZLL_CURSOR_ROW_STATUS_NA;
        aResult->mBlockCurrentRow = ZLR_ROWSET_AFTER_END;
    }
    
    STL_FINISH;

    return sRet;
}

stlStatus zlcrFetch4Imp( zlcDbc             * aDbc,
                         zlsStmt            * aStmt,
                         dtlValueBlockList ** aDataPtr,
                         stlBool            * aNotFound,
                         stlErrorStack      * aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  ZLE_ERRCODE_DRIVER_DOES_NOT_SUPPORT_THIS_FUNCTION,
                  NULL,
                  aErrorStack );

    return STL_FAILURE;
}

stlStatus zlcrSetPos( zlcDbc        * aDbc,
                      zlsStmt       * aStmt,
                      stlInt64        aRowNumber,
                      stlInt16        aOperation,
                      stlUInt16       aLockType,
                      stlErrorStack * aErrorStack )
{
    cmlHandle * sHandle  = &aDbc->mComm;
    stlBool     sIgnored = STL_FALSE;
    
    STL_TRY( cmlWriteSetPosCommand( sHandle,
                                    CML_CONTROL_HEAD | CML_CONTROL_TAIL,
                                    aStmt->mStmtId,
                                    aRowNumber,
                                    aOperation,
                                    aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( cmlSendPacket( sHandle,
                            aErrorStack ) == STL_SUCCESS );

    STL_TRY( cmlReadSetPosResult( sHandle,
                                  &sIgnored,
                                  &aStmt->mStmtId,
                                  aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
