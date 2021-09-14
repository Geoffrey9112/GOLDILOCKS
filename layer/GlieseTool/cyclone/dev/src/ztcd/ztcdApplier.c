/*******************************************************************************
 * ztcdApplier.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file ztcdApplier.c
 * @brief GlieseTool Cyclone ODBC Operation Routines
 */

#include <goldilocks.h>
#include <ztc.h>

extern ztcSlaveMgr  * gSlaveMgr;
extern ztcConfigure   gConfigure;

stlStatus ztcdDoInsertNExecute( ztcApplierMgr        * aApplierMgr,
                                ztcApplierTableInfo  * aTableInfo,
                                stlErrorStack        * aErrorStack )
{
    ztcColumnValue * sColumnValue = NULL;
    ztcStmtInfo    * sStmtInfo    = &aTableInfo->mStmtInfo;
    stlInt64         sArraySize   = aApplierMgr->mArraySize;
       
    dtlDataValue     sDataValue;    //for Conversion.
    stlInt64         sLength;
    stlInt64         sIdx;
    stlChar          sMsg[ZTC_TRACE_BUFFER_SIZE] = {0,};
    SQLSMALLINT      sErrRecNum = 1;
    
    SQL_LONG_VARIABLE_LENGTH_STRUCT * sVariableStr = NULL;
    
    /**
     * NULL Check 및 Time, Date, Interval, Numeric 데이터타입 Convertion
     */
    STL_RING_FOREACH_ENTRY( &aTableInfo->mColumnList, sColumnValue )
    {
        /**
         * Set Indicator
         * 넘어오는 Data는 Null Terminate 되어있지 않다.
         */
        switch( sColumnValue->mInfoPtr->mDataTypeId )
        {
            case DTL_TYPE_CHAR:
            case DTL_TYPE_VARCHAR:
            case DTL_TYPE_BINARY:
            case DTL_TYPE_VARBINARY:
            case DTL_TYPE_BOOLEAN:
            case DTL_TYPE_NATIVE_SMALLINT:
            case DTL_TYPE_NATIVE_INTEGER:
            case DTL_TYPE_NATIVE_BIGINT:
            case DTL_TYPE_NATIVE_REAL:
            case DTL_TYPE_NATIVE_DOUBLE:
            case DTL_TYPE_FLOAT:
            case DTL_TYPE_NUMBER:
            case DTL_TYPE_LONGVARCHAR:
            case DTL_TYPE_LONGVARBINARY:
            case DTL_TYPE_DATE:
            case DTL_TYPE_TIME:
            case DTL_TYPE_TIME_WITH_TIME_ZONE:
            case DTL_TYPE_TIMESTAMP:
            case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:
            {
                for( sIdx = 0; sIdx < sStmtInfo->mInsertIdx; sIdx++ )
                {
                    ZTC_SET_INDICATOR( sColumnValue->mIndicator[ sIdx ], sColumnValue->mValueSize[ sIdx ] );
                    
                    /**
                     * INDICATOR 세팅 후에는 mValueSize를 반드시 0으로 세팅해야 한다.
                     * 해당 값은 Continuous Column을 구성하는데 반드시 필요한 정보이다. (VARCHAR TYPE)
                     */
                    sColumnValue->mValueSize[ sIdx ] = 0;
                }
                break;
            }
            case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:
            case DTL_TYPE_INTERVAL_DAY_TO_SECOND:
            {
                for( sIdx = 0; sIdx < sStmtInfo->mInsertIdx; sIdx++ )
                {
                    if( sColumnValue->mValueSize[ sIdx ] != 0 )
                    {
                        sDataValue.mType   = sColumnValue->mInfoPtr->mDataTypeId;
                        sDataValue.mLength = sColumnValue->mValueSize[ sIdx ];
                        sDataValue.mBufferSize = sColumnValue->mInfoPtr->mBufferLength;
                        sDataValue.mValue  = &sColumnValue->mValue[ sColumnValue->mInfoPtr->mValueMaxSize * sIdx ];
                        
                        STL_TRY( dtlToStringDataValue( (dtlDataValue*)&sDataValue,
                                                       sColumnValue->mInfoPtr->mLeadingPrecision,
                                                       sColumnValue->mInfoPtr->mSecondPrecision,
                                                       sColumnValue->mInfoPtr->mBufferLength,
                                                       &sColumnValue->mStrValue[ sColumnValue->mInfoPtr->mStrValueMaxSize * sIdx ],
                                                       &sLength,
                                                       &gSlaveMgr->mDTVector,
                                                       NULL,
                                                       aErrorStack ) == STL_SUCCESS );
                        
                        sColumnValue->mIndicator[ sIdx ] = sLength;
                    }
                    else
                    {
                        sColumnValue->mIndicator[ sIdx ] = SQL_NULL_DATA;
                    }//end of if 
                    
                    sColumnValue->mValueSize[ sIdx ] = 0;
                }//end of for
                break;   
            }
            case DTL_TYPE_DECIMAL:
            case DTL_TYPE_BLOB:
            case DTL_TYPE_CLOB:
            {
                STL_THROW( RAMP_ERR_INVALID_DATATYPE );
                break;
            }
            default:
                break;
        }
    }
    
    /**
     * Array중 비어있는 Array는 Ignore를 세팅한다.
     */
    if( sStmtInfo->mInsertIdx != sStmtInfo->mInsertPrevIdx )
    {
        sStmtInfo->mInsertPrevIdx = sStmtInfo->mInsertIdx;
        
        for( sIdx = 0 ; sIdx < sArraySize; sIdx++ )
        {
            if( sIdx >= sStmtInfo->mInsertIdx )
            {
                sStmtInfo->mInsertOperation[ sIdx ] = SQL_PARAM_IGNORE;
            }
            else
            {
                sStmtInfo->mInsertOperation[ sIdx ] = SQL_PARAM_PROCEED;
            }
        }
    }
    
    /**
     * Array Errorcode 얻어오기로 변경해야 함!!
     */
    if( SQLExecute( sStmtInfo->mInsertStmt ) != SQL_SUCCESS )
    {
        for( sIdx = 0; sIdx < sStmtInfo->mInsertIdx; sIdx++ )
        {
            switch( sStmtInfo->mInsertStatus[ sIdx ] )
            {
                case SQL_PARAM_SUCCESS:
                case SQL_PARAM_UNUSED :
                case SQL_PARAM_DIAG_UNAVAILABLE :
                    break;
                case SQL_PARAM_SUCCESS_WITH_INFO :
                case SQL_PARAM_ERROR :    
                {
                    stlStrcpy( sMsg, "[APPLIER-INSERT] "); 
                    
                    STL_TRY( ztcdGetDiagnosticMsg( SQL_HANDLE_STMT,
                                                   sStmtInfo->mInsertStmt,
                                                   sErrRecNum++,
                                                   &sMsg[ stlStrlen( sMsg ) ],
                                                   aErrorStack ) == STL_SUCCESS );
                    
                    STL_TRY( ztcaWriteErrorValueToLog( sMsg, 
                                                       STL_TRUE,
                                                       aTableInfo,
                                                       aErrorStack ) == STL_SUCCESS );
                    break;
                }
                default:
                    break;
                    
            }
        }
    }
    else
    {
        /**
         * EXECUTE가 성공했을때만 증가시킨다.
         */
        aApplierMgr->mApplyCount            += sStmtInfo->mInsertIdx;
        aApplierMgr->mApplyCountAfterCommit += sStmtInfo->mInsertIdx;
    }
    
    /**
     * Long Variable Column이 존재할 경우에는 사용했던 메모리를 반납해야 한다.
     */
    if( aTableInfo->mHasLongVariableCol == STL_TRUE )
    {
        STL_RING_FOREACH_ENTRY( &aTableInfo->mColumnList, sColumnValue )
        {
            if( sColumnValue->mInfoPtr->mDataTypeId == DTL_TYPE_LONGVARCHAR ||
                sColumnValue->mInfoPtr->mDataTypeId == DTL_TYPE_LONGVARBINARY )
            {
                for( sIdx = 0; sIdx < sStmtInfo->mInsertIdx; sIdx++ )
                {
                    sVariableStr = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)&sColumnValue->mValue[ STL_SIZEOF( SQL_LONG_VARIABLE_LENGTH_STRUCT ) * sIdx ];
                    
                    STL_TRY( stlFreeDynamicMem( &(aApplierMgr->mLongVaryingMem),
                                                 sVariableStr->arr,
                                                 aErrorStack ) == STL_SUCCESS );
                    sVariableStr->len = 0;
                    sVariableStr->arr = NULL;
                    
                }
            }
        } //end of RING
    }
    
        
    aApplierMgr->mPrevTableInfo = NULL;
    aApplierMgr->mPrevTransType = ZTC_CAPTURE_TYPE_NONE;
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_DATATYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_DATA_TYPE,
                      NULL,
                      aErrorStack,
                      sColumnValue->mInfoPtr->mDataTypeId );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcdInsertCommitState( ztcApplierMgr  * aApplierMgr,
                                 SQLHDBC          aDbcHandle,
                                 SQLHSTMT         aStmtHandle,
                                 ztcCaptureInfo * aCaptureInfo,
                                 stlErrorStack  * aErrorStack )
{
    stlChar    sSqlString[ZTC_COMMAND_BUFFER_SIZE];    
    SQLRETURN  sResult;

    /*
    stlPrintf("========================= RESTART INFO[%d] ===\n", aApplierMgr->mAppId);
    stlPrintf("RestartLSN      = %ld\n", aCaptureInfo->mRestartLSN );
    stlPrintf("LastCommitLSN   = %ld\n", aCaptureInfo->mLastCommitLSN );
    stlPrintf("mRedoLogGroupId = %d\n", aCaptureInfo->mRedoLogGroupId );
    stlPrintf("BlockSeq        = %d\n", aCaptureInfo->mBlockSeq );
    stlPrintf("ReadLogNo       = %d\n", aCaptureInfo->mReadLogNo );
    stlPrintf("WrapNo          = %d\n", aCaptureInfo->mWrapNo );    
    stlPrintf("=========================================\n");
    */
    
    STL_TRY( ztcaDoPreviousExecute( aApplierMgr,
                                    aErrorStack ) == STL_SUCCESS );
    
    stlSnprintf( sSqlString,
                 ZTC_COMMAND_BUFFER_SIZE,
                 "UPDATE CYCLONE_SLAVE_STATE SET RESTART_LSN=%ld, LASTCOMMIT_LSN=%ld, FILESEQ_NO=%ld, GROUPID=%d, BLOCKSEQ=%d, LOGNO=%d, WRAPNO=%d "
                 "WHERE GROUP_NAME='%s' AND APPLIER_ID=%d",
                 aCaptureInfo->mRestartLSN,
                 aCaptureInfo->mLastCommitLSN,
                 aCaptureInfo->mFileSeqNo,
                 aCaptureInfo->mRedoLogGroupId,
                 aCaptureInfo->mBlockSeq,
                 aCaptureInfo->mReadLogNo,
                 aCaptureInfo->mWrapNo,
                 gConfigure.mCurrentGroupPtr->mName,
                 aApplierMgr->mAppId );
    
    STL_TRY( ztcdSQLExecDirect( aStmtHandle, 
                                (SQLCHAR*)sSqlString, 
                                stlStrlen( sSqlString ),
                                &sResult,
                                aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_UPDATE_STATE );

    STL_TRY( ztcdSQLEndTran( SQL_HANDLE_DBC,
                             aDbcHandle,
                             SQL_COMMIT,
                             STL_TRUE,
                             aErrorStack ) == STL_SUCCESS );
    
    aApplierMgr->mApplyCount            = 0;
    aApplierMgr->mApplyCountAfterCommit = 0;
    
    /**
     * Reset Restart Info 
     */
    aApplierMgr->mRestartInfo.mFileSeqNo      = ZTC_INVALID_FILE_SEQ_NO;
    aApplierMgr->mRestartInfo.mRestartLSN     = ZTC_INVALID_LSN;
    aApplierMgr->mRestartInfo.mLastCommitLSN  = ZTC_INVALID_LSN;
    aApplierMgr->mRestartInfo.mRedoLogGroupId = 0;
    aApplierMgr->mRestartInfo.mBlockSeq       = 0; 
    aApplierMgr->mRestartInfo.mReadLogNo      = 1;
    aApplierMgr->mRestartInfo.mWrapNo         = ZTC_INVALID_WRAP_NO; 
    
    /**
     * COMMIT 이후에, 다른 Transaction이 해당 Slot을 사용할 수 있도록
     * 획득된 RidSlot을 반환한다.
     */
    STL_TRY( ztcaVacateSlot( aApplierMgr,
                             aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_UPDATE_STATE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_FAIL_UPDATE_STATE,
                      NULL,
                      aErrorStack,
                      "CYCLONE_SLAVE_STATE");
    }
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztcdSelectCommitState( stlErrorStack * aErrorStack )
{
    stlChar          sSqlString[ZTC_COMMAND_BUFFER_SIZE];
    ztcCaptureInfo * sRestartInfo = &gSlaveMgr->mRestartInfo;
    
    stlInt32        sFetchCount     = 0;
    stlInt64        sRestartLSN     = ZTC_INVALID_LSN;
    stlInt64        sLastCommitLSN  = ZTC_INVALID_LSN;
    stlInt64        sFileSeqNo      = ZTC_INVALID_FILE_SEQ_NO;
    stlInt32        sRedoLogGroupId;
    stlInt32        sBlockSeq;
    stlInt32        sReadLogNo;
    stlInt16        sWrapNo;    
    stlInt32        sState = 0;
    
    SQLHSTMT        sStmtHandle;
    SQLRETURN       sResult;
    
    stlMemset( sRestartInfo, 0x00, STL_SIZEOF( ztcCaptureInfo ) );
    
    stlSnprintf( sSqlString,
                 ZTC_COMMAND_BUFFER_SIZE,
                 "SELECT RESTART_LSN, LASTCOMMIT_LSN, FILESEQ_NO, GROUPID, BLOCKSEQ, LOGNO, WRAPNO "
                 "FROM CYCLONE_SLAVE_STATE WHERE GROUP_NAME='%s' AND LASTCOMMIT_LSN != %d",
                 gConfigure.mCurrentGroupPtr->mName,
                 ZTC_INVALID_LSN );
    
    STL_TRY( ztcdSQLAllocHandle( gSlaveMgr->mDbcHandle,
                                 &sStmtHandle,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             1,
                             SQL_C_SBIGINT, 
                             &sRestartLSN,
                             STL_SIZEOF( sRestartLSN ),
                             0,
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             2,
                             SQL_C_SBIGINT, 
                             &sLastCommitLSN,
                             STL_SIZEOF( sLastCommitLSN ),
                             0,
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             3,
                             SQL_C_SBIGINT, 
                             &sFileSeqNo,
                             STL_SIZEOF( sFileSeqNo ),
                             0,
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             4,
                             SQL_C_SLONG, 
                             &sRedoLogGroupId,
                             STL_SIZEOF( sRedoLogGroupId ),
                             0,
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             5,
                             SQL_C_SLONG, 
                             &sBlockSeq,
                             STL_SIZEOF( sBlockSeq ),
                             0,
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             6,
                             SQL_C_SLONG, 
                             &sReadLogNo,
                             STL_SIZEOF( sReadLogNo ),
                             0,
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             7,
                             SQL_C_SSHORT, 
                             &sWrapNo,
                             STL_SIZEOF( sWrapNo ),
                             0,
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLExecDirect( sStmtHandle, 
                                (SQLCHAR*)sSqlString, 
                                stlStrlen( sSqlString ),
                                &sResult,
                                aErrorStack ) == STL_SUCCESS );
    
    while( 1 )
    {
        STL_TRY( ztcdSQLFetch( sStmtHandle, 
                               &sResult, 
                               aErrorStack ) == STL_SUCCESS );
        
        if( sResult == SQL_NO_DATA )
        {
            break;
        }
        
        if( sFetchCount == 0 ||
            sRestartInfo->mLastCommitLSN > sLastCommitLSN )     //RestartLSN으로??
        {
            sRestartInfo->mRestartLSN     = sRestartLSN;
            sRestartInfo->mLastCommitLSN  = sLastCommitLSN;
            sRestartInfo->mFileSeqNo      = sFileSeqNo;
            sRestartInfo->mRedoLogGroupId = sRedoLogGroupId;
            sRestartInfo->mBlockSeq       = sBlockSeq; 
            sRestartInfo->mReadLogNo      = sReadLogNo;
            sRestartInfo->mWrapNo         = sWrapNo;
        }
        
        sFetchCount++;
    }
    
    if( sFetchCount == 0 )
    {
        sRestartInfo->mRestartLSN     = ZTC_INVALID_LSN;
        sRestartInfo->mLastCommitLSN  = ZTC_INVALID_LSN;
        sRestartInfo->mFileSeqNo      = ZTC_INVALID_FILE_SEQ_NO;
        sRestartInfo->mRedoLogGroupId = 0;
        sRestartInfo->mBlockSeq       = 0; 
        sRestartInfo->mReadLogNo      = 1;
        sRestartInfo->mWrapNo         = ZTC_INVALID_WRAP_NO;   
    }

    sState = 0;
    STL_TRY( ztcdSQLFreeHandle( sStmtHandle, 
                                aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdInsertBlankCommitState( gSlaveMgr->mDbcHandle,
                                         sRestartInfo,
                                         aErrorStack )== STL_SUCCESS );
    
    return SQL_SUCCESS;
    
    STL_FINISH;
    
    if( sState > 0 )
    {
        (void) ztcdSQLFreeHandle( sStmtHandle, 
                                  aErrorStack );
    }

    return STL_FAILURE;
}

stlStatus ztcdInsertBlankCommitState( SQLHDBC          aDbcHandle,
                                      ztcCaptureInfo * aRestartInfo,
                                      stlErrorStack  * aErrorStack )
{
    stlChar     sSqlString[ZTC_COMMAND_BUFFER_SIZE];
    stlInt32    sState        = 0;
    stlInt64    sIdx          = 0;
    stlInt64    sApplierCount = gSlaveMgr->mApplierCount;
    SQLHSTMT    sStmtHandle;
    SQLRETURN   sResult;
    
    /**
     * 1. 기존의 Slave_state를 삭제
     * 2. 마지막에 반영된 최소 LastCommitLSN을 갖는 Record를 INSERT
     * 3. 각 Applier가 UPDATE할 수 있도록 Blank Record INSERT
     * 이렇게 처리하는 이유는 Applier(Session) 개수가 전에 실행했던 값보다 작을 수 있기 때문에 
     * 잘못된 Restart 정보를 재실행시 교정해준다.
     */
    
    STL_TRY( ztcdSQLAllocHandle( aDbcHandle,
                                 &sStmtHandle,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    stlSnprintf( sSqlString,
                 ZTC_COMMAND_BUFFER_SIZE,
                 "DELETE FROM CYCLONE_SLAVE_STATE WHERE GROUP_NAME='%s'", 
                 gConfigure.mCurrentGroupPtr->mName );
    
    STL_TRY( ztcdSQLExecDirect( sStmtHandle, 
                                (SQLCHAR*)sSqlString, 
                                stlStrlen( sSqlString ),
                                &sResult,
                                aErrorStack ) == STL_SUCCESS );
    
    for( sIdx = 0; sIdx < sApplierCount; sIdx++ )
    {
        stlSnprintf( sSqlString,
                     ZTC_COMMAND_BUFFER_SIZE,
                     "INSERT INTO CYCLONE_SLAVE_STATE VALUES( '%s', %d, %ld, %ld, %ld, %d, %d, %d, %d )", 
                     gConfigure.mCurrentGroupPtr->mName,
                     sIdx,
                     aRestartInfo->mRestartLSN,
                     aRestartInfo->mLastCommitLSN,
                     aRestartInfo->mFileSeqNo,
                     aRestartInfo->mRedoLogGroupId,
                     aRestartInfo->mBlockSeq,
                     aRestartInfo->mReadLogNo,
                     aRestartInfo->mWrapNo );
        
        STL_TRY( ztcdSQLExecDirect( sStmtHandle, 
                                    (SQLCHAR*)sSqlString, 
                                    stlStrlen( sSqlString ),
                                    &sResult,
                                    aErrorStack ) == STL_SUCCESS );
        // Result Check???
    }
    
    sState = 0;
    STL_TRY( ztcdSQLFreeHandle( sStmtHandle, 
                                aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLEndTran( SQL_HANDLE_DBC,
                             aDbcHandle,
                             SQL_COMMIT,
                             STL_TRUE,
                             aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    if( sState > 0 )
    {
        (void) SQLFreeHandle( SQL_HANDLE_STMT,
                              sStmtHandle );
    }
    
    return STL_FAILURE;
    
}

stlStatus ztcdDoDeleteNExecute( ztcApplierMgr        * aApplierMgr,
                                ztcApplierTableInfo  * aTableInfo,
                                stlErrorStack        * aErrorStack )
{
    ztcColumnValue  * sColumnValue = NULL;
    ztcStmtInfo     * sStmtInfo    = &aTableInfo->mStmtInfo;
    stlInt64          sArraySize   = aApplierMgr->mArraySize;
    
    dtlDataValue     sDataValue;    //Conversion
    stlInt64         sLength;
    stlInt64         sIdx;
    stlChar          sMsg[ZTC_TRACE_BUFFER_SIZE] = {0,};
    SQLSMALLINT      sErrRecNum = 1;
    SQLRETURN        sResult;
    
    /**
     * NULL Check 및 Time, Date, Interval, Numeric 데이터타입 Convertion
     */
    STL_RING_FOREACH_ENTRY( &aTableInfo->mColumnList, sColumnValue )
    {
        if( sColumnValue->mInfoPtr->mIsPrimary != STL_TRUE )
        {
            continue;
        }
        
        switch( sColumnValue->mInfoPtr->mDataTypeId )
        {
            case DTL_TYPE_CHAR:
            case DTL_TYPE_VARCHAR:
            case DTL_TYPE_BINARY:
            case DTL_TYPE_VARBINARY:
            case DTL_TYPE_BOOLEAN:
            case DTL_TYPE_NATIVE_SMALLINT:
            case DTL_TYPE_NATIVE_INTEGER:
            case DTL_TYPE_NATIVE_BIGINT:
            case DTL_TYPE_NATIVE_REAL:
            case DTL_TYPE_NATIVE_DOUBLE:
            case DTL_TYPE_FLOAT:
            case DTL_TYPE_NUMBER:
            case DTL_TYPE_DATE:
            case DTL_TYPE_TIME:
            case DTL_TYPE_TIME_WITH_TIME_ZONE:
            case DTL_TYPE_TIMESTAMP:
            case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:
            {
                for( sIdx = 0; sIdx < sStmtInfo->mDeleteIdx; sIdx++ )
                {
                    sColumnValue->mKeyIndicator[ sIdx ] = sColumnValue->mKeyValueSize[ sIdx ];
                }
                break;
            }
            case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:
            case DTL_TYPE_INTERVAL_DAY_TO_SECOND:
            {
                for( sIdx = 0; sIdx < sStmtInfo->mDeleteIdx; sIdx++ )
                {
                    sDataValue.mType   = sColumnValue->mInfoPtr->mDataTypeId;
                    sDataValue.mLength = sColumnValue->mKeyValueSize[ sIdx ];
                    sDataValue.mBufferSize = sColumnValue->mInfoPtr->mBufferLength;
                    sDataValue.mValue  = &sColumnValue->mKeyValue[ sColumnValue->mInfoPtr->mValueMaxSize * sIdx ];
            
                    STL_TRY( dtlToStringDataValue( (dtlDataValue*)&sDataValue,
                                                   sColumnValue->mInfoPtr->mLeadingPrecision,
                                                   sColumnValue->mInfoPtr->mSecondPrecision,
                                                   sColumnValue->mInfoPtr->mBufferLength,
                                                   &sColumnValue->mKeyStrValue[ sColumnValue->mInfoPtr->mStrValueMaxSize * sIdx ],
                                                   &sLength,
                                                   &gSlaveMgr->mDTVector,
                                                   NULL,
                                                   aErrorStack ) == STL_SUCCESS );
                
                    sColumnValue->mKeyIndicator[ sIdx ] = sLength;
                }
                break;
            }
            case DTL_TYPE_DECIMAL:
            case DTL_TYPE_LONGVARBINARY:
            case DTL_TYPE_LONGVARCHAR:
            case DTL_TYPE_BLOB:
            case DTL_TYPE_CLOB:
            {
                /** Not Supported Yet.*/
                STL_THROW( RAMP_ERR_INVALID_DATATYPE );
                break;
            }
            default:
                break;
        }
    }
    
    /**
     * Array중 비어있는 Array는 Ignore를 세팅한다.
     */
    if( sStmtInfo->mDeleteIdx != sStmtInfo->mDeletePrevIdx )
    {
        sStmtInfo->mDeletePrevIdx = sStmtInfo->mDeleteIdx;
        
        for( sIdx = 0 ; sIdx < sArraySize; sIdx++ )
        {
            if( sIdx >= sStmtInfo->mDeleteIdx )
            {
                sStmtInfo->mDeleteOperation[ sIdx ] = SQL_PARAM_IGNORE;
            }
            else
            {
                sStmtInfo->mDeleteOperation[ sIdx ] = SQL_PARAM_PROCEED;
            }
        }
    }
    
    sResult = SQLExecute( sStmtInfo->mDeleteStmt );
    if( sResult != SQL_SUCCESS )
    {
        if( sResult == SQL_NO_DATA ) 
        {
            stlStrcpy( sMsg, "[APPLIER-DELETE] Conflict.\n"); 
            
            STL_TRY( ztcaWriteErrorValueToLog( sMsg, 
                                               STL_FALSE,
                                               aTableInfo,
                                               aErrorStack ) == STL_SUCCESS );
        }
        else
        {
            for( sIdx = 0; sIdx < sStmtInfo->mDeleteIdx; sIdx++ )
            {
                switch( sStmtInfo->mDeleteStatus[ sIdx ] )
                {
                    case SQL_PARAM_SUCCESS:
                    case SQL_PARAM_UNUSED :
                    case SQL_PARAM_DIAG_UNAVAILABLE :
                        break;
                    case SQL_PARAM_SUCCESS_WITH_INFO :
                    case SQL_PARAM_ERROR :
                    {
                        stlStrcpy( sMsg, "[APPLIER-DELETE] "); 
                    
                        STL_TRY( ztcdGetDiagnosticMsg( SQL_HANDLE_STMT,
                                                       sStmtInfo->mDeleteStmt,
                                                       sErrRecNum++,
                                                       &sMsg[ stlStrlen( sMsg ) ],
                                                       aErrorStack ) == STL_SUCCESS );
                    
                        STL_TRY( ztcaWriteErrorValueToLog( sMsg, 
                                                           STL_FALSE,
                                                           aTableInfo,
                                                           aErrorStack ) == STL_SUCCESS );
                        break;
                    }
                    default:
                        break;
                }
            }
        }
    }
    else
    {
        /**
         * EXECUTE가 성공했을때만 증가시킨다.
         */
        aApplierMgr->mApplyCount            += sStmtInfo->mDeleteIdx;
        aApplierMgr->mApplyCountAfterCommit += sStmtInfo->mDeleteIdx;   
    }
    
    aApplierMgr->mPrevTableInfo = NULL;
    aApplierMgr->mPrevTransType = ZTC_CAPTURE_TYPE_NONE;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_DATATYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_DATA_TYPE,
                      NULL,
                      aErrorStack,
                      sColumnValue->mInfoPtr->mDataTypeId );
    }
    STL_FINISH;
    
    return STL_FAILURE; 
}


stlStatus ztcdDoUpdateNExecute( ztcApplierMgr        * aApplierMgr,
                                ztcApplierTableInfo  * aTableInfo,
                                stlErrorStack        * aErrorStack )
{
    stlChar          sPrepareStr[ZTC_COMMAND_BUFFER_SIZE] = {0,};
    ztcColumnValue * sColumnValue = NULL;
    ztcStmtInfo    * sStmtInfo    = &aTableInfo->mStmtInfo;
    stlInt64         sArraySize   = aApplierMgr->mArraySize;
    stlInt32         sUpdateMode  = ztcmGetUpdateApplyMode();
    
    stlUInt16        sColumnIdx   = 0;
    stlInt64         sIdx;   
    stlBool          sCompare     = STL_TRUE;
    
    dtlDataValue     sDataValue;    //Conversion
    stlInt64         sLength;
    stlChar          sMsg[ZTC_TRACE_BUFFER_SIZE] = {0,};
    SQLSMALLINT      sErrRecNum = 1;
    SQLRETURN        sResult;
    
    SQL_LONG_VARIABLE_LENGTH_STRUCT * sVariableStr = NULL;
    
    /**
     * 전에 Prepare했던 Column 정보와 다를 경우만 다시 Prepare를 하도록 한다.
     * 판단은 Log 분석시에 한다.(ztcaAnalyzeSuppUpdateColLog)
     */
    if( aTableInfo->mNeedUpdatePrepare == STL_TRUE )
    {
        /**
         * Bind Param을 Reset 한다.
         */
        STL_TRY( ztcdSQLFreeStmt( sStmtInfo->mUpdateStmt,
                                  SQL_RESET_PARAMS,
                                  &sResult,
                                  aErrorStack ) == STL_SUCCESS );
        
        stlMemset( sPrepareStr, 0x00, STL_SIZEOF( sPrepareStr ) );
        
        stlSnprintf( sPrepareStr,
                     STL_SIZEOF( sPrepareStr ),
                     "UPDATE /*+ INDEX(\"%s\", %s) */ \"%s\".\"%s\" SET ",
                     aTableInfo->mInfoPtr->mSlaveTable, 
                     aTableInfo->mInfoPtr->mPKIndexName,
                     aTableInfo->mInfoPtr->mSlaveSchema,
                     aTableInfo->mInfoPtr->mSlaveTable );

        /**
         * Updated Column 정보
         */
        sColumnIdx = 0;
        STL_RING_FOREACH_ENTRY( &aTableInfo->mColumnList, sColumnValue )
        {
            if( sColumnValue->mIsUpdated == STL_TRUE )
            {
                if( sColumnIdx == 0 )
                {
                    stlSnprintf( sPrepareStr,
                                 STL_SIZEOF( sPrepareStr ),
                                 "%s \"%s\"=?", sPrepareStr, sColumnValue->mInfoPtr->mColumnName );
                    
                }
                else
                {
                    stlSnprintf( sPrepareStr,
                                 STL_SIZEOF( sPrepareStr ),
                                 "%s, \"%s\"=?", sPrepareStr, sColumnValue->mInfoPtr->mColumnName );
                }
                sColumnIdx++;
            }
        }
    
        /**
         * WHERE 절에 들어갈 정보
         */
        sColumnIdx = 0;
        STL_RING_FOREACH_ENTRY( &aTableInfo->mColumnList, sColumnValue )
        {
            if( sColumnValue->mInfoPtr->mIsPrimary == STL_TRUE )
            {
                if( sColumnIdx == 0 )
                {
                    stlSnprintf( sPrepareStr,
                                 STL_SIZEOF( sPrepareStr ),
                                 "%s WHERE \"%s\"=?", sPrepareStr, sColumnValue->mInfoPtr->mColumnName );
                }
                else
                {
                    stlSnprintf( sPrepareStr,
                                 STL_SIZEOF( sPrepareStr ),
                                 "%s AND \"%s\"=?", sPrepareStr, sColumnValue->mInfoPtr->mColumnName );
                }
                sColumnIdx++;
            }
        }
        
        /**
         * Before Image를 Where절에 추가..
         */
        if( sUpdateMode == 1 )
        {
            STL_RING_FOREACH_ENTRY( &aTableInfo->mColumnList, sColumnValue )
            {
                if( sColumnValue->mInfoPtr->mIsPrimary != STL_TRUE )
                {
                    if( sColumnValue->mIsUpdated == STL_TRUE )
                    {
                        stlSnprintf( sPrepareStr,
                                     STL_SIZEOF( sPrepareStr ),
                                     "%s AND NVL2( ?, \"%s\"=?, \"%s\" IS NULL) ",
                                     sPrepareStr, sColumnValue->mInfoPtr->mColumnName, sColumnValue->mInfoPtr->mColumnName );
                    }
                }
            }
        }
        else if( sUpdateMode == 2 )
        {
            sColumnIdx = 0;
            STL_RING_FOREACH_ENTRY( &aTableInfo->mColumnList, sColumnValue )
            {
                if( sColumnValue->mInfoPtr->mIsPrimary != STL_TRUE )
                {
                    if( sColumnValue->mIsUpdated == STL_TRUE )
                    {
                        if( sColumnIdx == 0 )
                        {
                            stlSnprintf( sPrepareStr,
                                         STL_SIZEOF( sPrepareStr ),
                                         "%s RETURNING OLD \"%s\" ", sPrepareStr, sColumnValue->mInfoPtr->mColumnName );
                        }
                        else
                        {
                            stlSnprintf( sPrepareStr,
                                         STL_SIZEOF( sPrepareStr ),
                                         "%s, \"%s\" ", sPrepareStr, sColumnValue->mInfoPtr->mColumnName );
                        }

                        sColumnIdx++;
                    }
                }
            }

            if( sColumnIdx > 0 )
            {
                stlSnprintf( sPrepareStr,
                             STL_SIZEOF( sPrepareStr ),
                             "%s INTO ?", sPrepareStr );

                for( sIdx = 1; sIdx < sColumnIdx; sIdx++ )
                {
                    stlSnprintf( sPrepareStr,
                                 STL_SIZEOF( sPrepareStr ),
                                 "%s, ?", sPrepareStr );
                }
            }
        }
        
        //stlPrintf("[%s]\n", sPrepareStr);
        
        STL_TRY( ztcdSQLPrepare( sStmtInfo->mUpdateStmt,
                                 (SQLCHAR*)sPrepareStr,
                                 stlStrlen( sPrepareStr ),
                                 aErrorStack ) == STL_SUCCESS );
    } //end of NeedPrepare
    
    /**
     * SET Clause
     */
    sColumnIdx = 1;
    STL_RING_FOREACH_ENTRY( &aTableInfo->mColumnList, sColumnValue )
    {
        if( sColumnValue->mIsUpdated != STL_TRUE )
        {
            continue;
        }
    
        switch( sColumnValue->mInfoPtr->mDataTypeId )
        {
            case DTL_TYPE_CHAR:
            case DTL_TYPE_VARCHAR:
            case DTL_TYPE_BINARY:
            case DTL_TYPE_VARBINARY:
            case DTL_TYPE_BOOLEAN:
            case DTL_TYPE_NATIVE_SMALLINT:
            case DTL_TYPE_NATIVE_INTEGER:
            case DTL_TYPE_NATIVE_BIGINT:
            case DTL_TYPE_NATIVE_REAL:
            case DTL_TYPE_NATIVE_DOUBLE:
            case DTL_TYPE_FLOAT:
            case DTL_TYPE_NUMBER:
            case DTL_TYPE_LONGVARCHAR:
            case DTL_TYPE_LONGVARBINARY:
            case DTL_TYPE_DATE:
            case DTL_TYPE_TIME:
            case DTL_TYPE_TIME_WITH_TIME_ZONE:
            case DTL_TYPE_TIMESTAMP:
            case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:
            {
                for( sIdx = 0; sIdx < sStmtInfo->mUpdateIdx; sIdx++ )
                {
                    ZTC_SET_INDICATOR( sColumnValue->mIndicator[ sIdx ], sColumnValue->mValueSize[ sIdx ] );
                    
                    /**
                     * INDICATOR 세팅 후에는 mValueSize를 반드시 0으로 세팅해야 한다.
                     * 해당 값은 Continuous Column을 구성하는데 반드시 필요한 정보이다. (VARCHAR TYPE)
                     */
                    sColumnValue->mValueSize[ sIdx ] = 0;
                }
                
                if( aTableInfo->mNeedUpdatePrepare == STL_TRUE )
                {
                    STL_TRY( ztcdSQLBindParameter( sStmtInfo->mUpdateStmt,
                                                   sColumnIdx,
                                                   SQL_PARAM_INPUT,
                                                   sColumnValue->mInfoPtr->mValueType,
                                                   sColumnValue->mInfoPtr->mParameterType,
                                                   sColumnValue->mInfoPtr->mStringLengthUnit,
                                                   sColumnValue->mInfoPtr->mColumnSize,
                                                   sColumnValue->mInfoPtr->mDecimalDigits,
                                                   sColumnValue->mValue,
                                                   sColumnValue->mInfoPtr->mBufferLength,
                                                   sColumnValue->mIndicator,
                                                   aErrorStack ) == STL_SUCCESS );
                }
                break;
            }
            case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:
            case DTL_TYPE_INTERVAL_DAY_TO_SECOND:
            {
                for( sIdx = 0; sIdx < sStmtInfo->mUpdateIdx; sIdx++ )
                {
                    if( sColumnValue->mValueSize[ sIdx ] != 0 )
                    {
                        sDataValue.mType   = sColumnValue->mInfoPtr->mDataTypeId;
                        sDataValue.mLength = sColumnValue->mValueSize[ sIdx ];
                        sDataValue.mBufferSize = sColumnValue->mInfoPtr->mBufferLength;
                        sDataValue.mValue  = &sColumnValue->mValue[ sColumnValue->mInfoPtr->mValueMaxSize * sIdx ];
        
                        STL_TRY( dtlToStringDataValue( (dtlDataValue*)&sDataValue,
                                                       sColumnValue->mInfoPtr->mLeadingPrecision,
                                                       sColumnValue->mInfoPtr->mSecondPrecision,
                                                       sColumnValue->mInfoPtr->mBufferLength,
                                                       &sColumnValue->mStrValue[ sColumnValue->mInfoPtr->mStrValueMaxSize * sIdx ],
                                                       &sLength,
                                                       &gSlaveMgr->mDTVector,
                                                       NULL,
                                                       aErrorStack ) == STL_SUCCESS );
            
                        sColumnValue->mIndicator[ sIdx ] = sLength;
                    }
                    else
                    {
                        sColumnValue->mIndicator[ sIdx ] = SQL_NULL_DATA;
                    } //end of if
                    
                    sColumnValue->mValueSize[ sIdx ] = 0;
                    
                } //end of for loop
                
                if( aTableInfo->mNeedUpdatePrepare == STL_TRUE )
                {
                    STL_TRY( ztcdSQLBindParameter( sStmtInfo->mUpdateStmt,
                                                   sColumnIdx,
                                                   SQL_PARAM_INPUT,
                                                   sColumnValue->mInfoPtr->mValueType,
                                                   sColumnValue->mInfoPtr->mParameterType,
                                                   sColumnValue->mInfoPtr->mStringLengthUnit,
                                                   sColumnValue->mInfoPtr->mColumnSize,
                                                   sColumnValue->mInfoPtr->mDecimalDigits,
                                                   sColumnValue->mStrValue,
                                                   sColumnValue->mInfoPtr->mBufferLength,
                                                   sColumnValue->mIndicator,
                                                   aErrorStack ) == STL_SUCCESS );
                }
                break;
            }
            case DTL_TYPE_DECIMAL:
            case DTL_TYPE_BLOB:
            case DTL_TYPE_CLOB:
            {
                /** Not Supported Yet.*/
                STL_THROW( RAMP_ERR_INVALID_DATATYPE );
                break;
            }
            default:
                break;
        }
        
        sColumnIdx++;
    }
    
    /**
     * Primary Key
     */
    STL_RING_FOREACH_ENTRY( &aTableInfo->mColumnList, sColumnValue )
    {
        if( sColumnValue->mInfoPtr->mIsPrimary != STL_TRUE )
        {
            continue;
        }
        
        switch( sColumnValue->mInfoPtr->mDataTypeId )
        {
            case DTL_TYPE_CHAR:
            case DTL_TYPE_VARCHAR:
            case DTL_TYPE_BINARY:
            case DTL_TYPE_VARBINARY:
            case DTL_TYPE_BOOLEAN:
            case DTL_TYPE_NATIVE_SMALLINT:
            case DTL_TYPE_NATIVE_INTEGER:
            case DTL_TYPE_NATIVE_BIGINT:
            case DTL_TYPE_NATIVE_REAL:
            case DTL_TYPE_NATIVE_DOUBLE:
            case DTL_TYPE_FLOAT:
            case DTL_TYPE_NUMBER:
            case DTL_TYPE_DATE:
            case DTL_TYPE_TIME:
            case DTL_TYPE_TIME_WITH_TIME_ZONE:
            case DTL_TYPE_TIMESTAMP:
            case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:
            {
                for( sIdx = 0; sIdx < sStmtInfo->mUpdateIdx; sIdx++ )
                {
                    sColumnValue->mKeyIndicator[ sIdx ] = sColumnValue->mKeyValueSize[ sIdx ];
                }
                
                if( aTableInfo->mNeedUpdatePrepare == STL_TRUE )
                {
                    STL_TRY( ztcdSQLBindParameter( sStmtInfo->mUpdateStmt,
                                                   sColumnIdx,
                                                   SQL_PARAM_INPUT,
                                                   sColumnValue->mInfoPtr->mValueType,
                                                   sColumnValue->mInfoPtr->mParameterType,
                                                   sColumnValue->mInfoPtr->mStringLengthUnit,
                                                   sColumnValue->mInfoPtr->mColumnSize,
                                                   sColumnValue->mInfoPtr->mDecimalDigits,
                                                   sColumnValue->mKeyValue,
                                                   sColumnValue->mInfoPtr->mBufferLength,
                                                   sColumnValue->mKeyIndicator,
                                                   aErrorStack ) == STL_SUCCESS );
                }
                break;
            }
            case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:
            case DTL_TYPE_INTERVAL_DAY_TO_SECOND:
            {
                for( sIdx = 0; sIdx < sStmtInfo->mUpdateIdx; sIdx++ )
                {
                    sDataValue.mType   = sColumnValue->mInfoPtr->mDataTypeId;
                    sDataValue.mLength = sColumnValue->mKeyValueSize[ sIdx ];
                    sDataValue.mValue  = &sColumnValue->mKeyValue[ sColumnValue->mInfoPtr->mValueMaxSize * sIdx ];
            
                    STL_TRY( dtlToStringDataValue( (dtlDataValue*)&sDataValue,
                                                   sColumnValue->mInfoPtr->mLeadingPrecision,
                                                   sColumnValue->mInfoPtr->mSecondPrecision,
                                                   sColumnValue->mInfoPtr->mBufferLength,
                                                   &sColumnValue->mKeyStrValue[ sColumnValue->mInfoPtr->mStrValueMaxSize * sIdx ],
                                                   &sLength,
                                                   &gSlaveMgr->mDTVector,
                                                   NULL,
                                                   aErrorStack ) == STL_SUCCESS );
                
                    sColumnValue->mKeyIndicator[ sIdx ] = sLength;
                }
                
                if( aTableInfo->mNeedUpdatePrepare == STL_TRUE )
                {
                    STL_TRY( ztcdSQLBindParameter( sStmtInfo->mUpdateStmt,
                                                   sColumnIdx,
                                                   SQL_PARAM_INPUT,
                                                   sColumnValue->mInfoPtr->mValueType,
                                                   sColumnValue->mInfoPtr->mParameterType,
                                                   sColumnValue->mInfoPtr->mStringLengthUnit,
                                                   sColumnValue->mInfoPtr->mColumnSize,
                                                   sColumnValue->mInfoPtr->mDecimalDigits,
                                                   sColumnValue->mKeyStrValue,
                                                   sColumnValue->mInfoPtr->mBufferLength,
                                                   sColumnValue->mKeyIndicator,
                                                   aErrorStack ) == STL_SUCCESS );
                }
                break;
            }
            case DTL_TYPE_DECIMAL:
            case DTL_TYPE_LONGVARCHAR:          //Long variable Column은 Primary Key가 될 수 엄따...
            case DTL_TYPE_LONGVARBINARY:
            case DTL_TYPE_BLOB:
            case DTL_TYPE_CLOB:
            {
                /** Not Supported Yet.*/
                STL_THROW( RAMP_ERR_INVALID_DATATYPE );
                break;
            }
            default:
                break;
        }
        
        sColumnIdx++;
    }
    
    /**
     * Before Image
     */
    if( sUpdateMode == 1 )
    {
        STL_RING_FOREACH_ENTRY( &aTableInfo->mColumnList, sColumnValue )
        {
            if( sColumnValue->mInfoPtr->mIsPrimary == STL_TRUE )
            {
                continue;
            }
        
            if( sColumnValue->mIsUpdated != STL_TRUE )
            {
                continue;
            }
    
            switch( sColumnValue->mInfoPtr->mDataTypeId )
            {
                case DTL_TYPE_CHAR:
                case DTL_TYPE_VARCHAR:
                case DTL_TYPE_BINARY:
                case DTL_TYPE_VARBINARY:
                case DTL_TYPE_BOOLEAN:
                case DTL_TYPE_NATIVE_SMALLINT:
                case DTL_TYPE_NATIVE_INTEGER:
                case DTL_TYPE_NATIVE_BIGINT:
                case DTL_TYPE_NATIVE_REAL:
                case DTL_TYPE_NATIVE_DOUBLE:
                case DTL_TYPE_FLOAT:
                case DTL_TYPE_NUMBER:
                case DTL_TYPE_DATE:
                case DTL_TYPE_LONGVARCHAR:
                case DTL_TYPE_LONGVARBINARY:
                case DTL_TYPE_TIME:
                case DTL_TYPE_TIME_WITH_TIME_ZONE:
                case DTL_TYPE_TIMESTAMP:
                case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:
                {
                    for( sIdx = 0; sIdx < sStmtInfo->mUpdateIdx; sIdx++ )
                    {
                        ZTC_SET_INDICATOR( sColumnValue->mBefIndicator[ sIdx ], sColumnValue->mBefValueSize[ sIdx ] );
                        
                        /**
                         * INDICATOR 세팅 후에는 mValueSize를 반드시 0으로 세팅해야 한다.
                         * 해당 값은 Continuous Column을 구성하는데 반드시 필요한 정보이다. (VARCHAR TYPE)
                         */
                        sColumnValue->mBefValueSize[ sIdx ] = 0;
                    }
                    
                    if( aTableInfo->mNeedUpdatePrepare == STL_TRUE )
                    {
                        STL_TRY( ztcdSQLBindParameter( sStmtInfo->mUpdateStmt,
                                                       sColumnIdx,
                                                       SQL_PARAM_INPUT,
                                                       sColumnValue->mInfoPtr->mValueType,
                                                       sColumnValue->mInfoPtr->mParameterType,
                                                       sColumnValue->mInfoPtr->mStringLengthUnit,
                                                       sColumnValue->mInfoPtr->mColumnSize,
                                                       sColumnValue->mInfoPtr->mDecimalDigits,
                                                       sColumnValue->mBefValue,
                                                       sColumnValue->mInfoPtr->mBufferLength,
                                                       sColumnValue->mBefIndicator,
                                                       aErrorStack ) == STL_SUCCESS );
                        
                        sColumnIdx++;
                        
                        STL_TRY( ztcdSQLBindParameter( sStmtInfo->mUpdateStmt,
                                                       sColumnIdx,
                                                       SQL_PARAM_INPUT,
                                                       sColumnValue->mInfoPtr->mValueType,
                                                       sColumnValue->mInfoPtr->mParameterType,
                                                       sColumnValue->mInfoPtr->mStringLengthUnit,
                                                       sColumnValue->mInfoPtr->mColumnSize,
                                                       sColumnValue->mInfoPtr->mDecimalDigits,
                                                       sColumnValue->mBefValue,
                                                       sColumnValue->mInfoPtr->mBufferLength,
                                                       sColumnValue->mBefIndicator,
                                                       aErrorStack ) == STL_SUCCESS );
                    }
                    break;
                }
                case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:
                case DTL_TYPE_INTERVAL_DAY_TO_SECOND:
                {
                    for( sIdx = 0; sIdx < sStmtInfo->mUpdateIdx; sIdx++ )
                    {
                        if( sColumnValue->mBefValueSize[ sIdx ] != 0 )
                        {
                            sDataValue.mType   = sColumnValue->mInfoPtr->mDataTypeId;
                            sDataValue.mLength = sColumnValue->mBefValueSize[ sIdx ];
                            sDataValue.mValue  = &sColumnValue->mBefValue[ sColumnValue->mInfoPtr->mValueMaxSize * sIdx ];
                
                            STL_TRY( dtlToStringDataValue( (dtlDataValue*)&sDataValue,
                                                           sColumnValue->mInfoPtr->mLeadingPrecision,
                                                           sColumnValue->mInfoPtr->mSecondPrecision,
                                                           sColumnValue->mInfoPtr->mBufferLength,
                                                           &sColumnValue->mBefStrValue[ sColumnValue->mInfoPtr->mStrValueMaxSize * sIdx ],
                                                           &sLength,
                                                           &gSlaveMgr->mDTVector,
                                                           NULL,
                                                           aErrorStack ) == STL_SUCCESS );
                    
                            sColumnValue->mBefIndicator[ sIdx ] = sLength;
                            //stlPrintf("[%s][%d]\n", sColumnValue->mBefStrValue, sLength );
                        }
                        else
                        {
                            sColumnValue->mBefIndicator[ sIdx ] = SQL_NULL_DATA;
                        } //end of if
                        
                        sColumnValue->mBefValueSize[ sIdx ] = 0;
                    }
                    
                    if( aTableInfo->mNeedUpdatePrepare == STL_TRUE )
                    {
                        STL_TRY( ztcdSQLBindParameter( sStmtInfo->mUpdateStmt,
                                                       sColumnIdx,
                                                       SQL_PARAM_INPUT,
                                                       sColumnValue->mInfoPtr->mValueType,
                                                       sColumnValue->mInfoPtr->mParameterType,
                                                       sColumnValue->mInfoPtr->mStringLengthUnit,
                                                       sColumnValue->mInfoPtr->mColumnSize,
                                                       sColumnValue->mInfoPtr->mDecimalDigits,
                                                       sColumnValue->mBefStrValue,
                                                       sColumnValue->mInfoPtr->mBufferLength,
                                                       sColumnValue->mBefIndicator,
                                                       aErrorStack ) == STL_SUCCESS );
                        sColumnIdx++;
                        
                        STL_TRY( ztcdSQLBindParameter( sStmtInfo->mUpdateStmt,
                                                       sColumnIdx,
                                                       SQL_PARAM_INPUT,
                                                       sColumnValue->mInfoPtr->mValueType,
                                                       sColumnValue->mInfoPtr->mParameterType,
                                                       sColumnValue->mInfoPtr->mStringLengthUnit,
                                                       sColumnValue->mInfoPtr->mColumnSize,
                                                       sColumnValue->mInfoPtr->mDecimalDigits,
                                                       sColumnValue->mBefStrValue,
                                                       sColumnValue->mInfoPtr->mBufferLength,
                                                       sColumnValue->mBefIndicator,
                                                       aErrorStack ) == STL_SUCCESS );
                    }
                    break;
                }
                case DTL_TYPE_DECIMAL:
                case DTL_TYPE_BLOB:
                case DTL_TYPE_CLOB:
                {
                    /** Not Supported Yet.*/
                    STL_THROW( RAMP_ERR_INVALID_DATATYPE );
                    break;
                }
                default:
                    break;
            }
            
            sColumnIdx++;
        }
    }  // end of if that before image ....
    else if( sUpdateMode == 2 ) 
    {
        STL_RING_FOREACH_ENTRY( &aTableInfo->mColumnList, sColumnValue )
        {
            if( sColumnValue->mInfoPtr->mIsPrimary == STL_TRUE )
            {
                continue;
            }
        
            if( sColumnValue->mIsUpdated != STL_TRUE )
            {
                continue;
            }
    
            switch( sColumnValue->mInfoPtr->mDataTypeId )
            {
                case DTL_TYPE_CHAR:
                case DTL_TYPE_VARCHAR:
                case DTL_TYPE_BINARY:
                case DTL_TYPE_VARBINARY:
                case DTL_TYPE_BOOLEAN:
                case DTL_TYPE_NATIVE_SMALLINT:
                case DTL_TYPE_NATIVE_INTEGER:
                case DTL_TYPE_NATIVE_BIGINT:
                case DTL_TYPE_NATIVE_REAL:
                case DTL_TYPE_NATIVE_DOUBLE:
                case DTL_TYPE_FLOAT:
                case DTL_TYPE_NUMBER:
                case DTL_TYPE_LONGVARCHAR:
                case DTL_TYPE_LONGVARBINARY:
                case DTL_TYPE_DATE:
                case DTL_TYPE_TIME:
                case DTL_TYPE_TIME_WITH_TIME_ZONE:
                case DTL_TYPE_TIMESTAMP:
                case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:
                {
                    ZTC_SET_INDICATOR( sColumnValue->mBefIndicator[ 0 ], sColumnValue->mBefValueSize[ 0 ] );
                    sColumnValue->mBefValueSize[ 0 ] = 0;
                    
                    if( aTableInfo->mNeedUpdatePrepare == STL_TRUE )
                    {
                        STL_TRY( ztcdSQLBindParameter( sStmtInfo->mUpdateStmt,
                                                       sColumnIdx,
                                                       SQL_PARAM_OUTPUT,
                                                       sColumnValue->mInfoPtr->mValueType,
                                                       sColumnValue->mInfoPtr->mParameterType,
                                                       sColumnValue->mInfoPtr->mStringLengthUnit,
                                                       sColumnValue->mInfoPtr->mColumnSize,
                                                       sColumnValue->mInfoPtr->mDecimalDigits,
                                                       sColumnValue->mRetValue,
                                                       sColumnValue->mInfoPtr->mBufferLength,
                                                       sColumnValue->mRetIndicator,
                                                       aErrorStack ) == STL_SUCCESS );
                    }
                    break;
                }
                case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:
                case DTL_TYPE_INTERVAL_DAY_TO_SECOND:
                {
                    /**
                     * Before Image 
                     */
                    if( sColumnValue->mBefValueSize[ 0 ] != 0 )
                    {
                        sDataValue.mType   = sColumnValue->mInfoPtr->mDataTypeId;
                        sDataValue.mLength = sColumnValue->mBefValueSize[ 0 ];
                        sDataValue.mValue  = &sColumnValue->mBefValue[ 0 ];
                
                        STL_TRY( dtlToStringDataValue( (dtlDataValue*)&sDataValue,
                                                       sColumnValue->mInfoPtr->mLeadingPrecision,
                                                       sColumnValue->mInfoPtr->mSecondPrecision,
                                                       sColumnValue->mInfoPtr->mBufferLength,
                                                       &sColumnValue->mBefStrValue[ 0 ],
                                                       &sLength,
                                                       &gSlaveMgr->mDTVector,
                                                       NULL,
                                                       aErrorStack ) == STL_SUCCESS );
                    
                        sColumnValue->mBefIndicator[ 0 ] = sLength;
                    }
                    else
                    {
                        sColumnValue->mBefIndicator[ 0 ] = SQL_NULL_DATA;
                    } //end of if
                        
                    sColumnValue->mBefValueSize[ 0 ] = 0;
                    
                    /**
                     * Bind Param
                     */
                    if( aTableInfo->mNeedUpdatePrepare == STL_TRUE )
                    {
                        STL_TRY( ztcdSQLBindParameter( sStmtInfo->mUpdateStmt,
                                                       sColumnIdx,
                                                       SQL_PARAM_OUTPUT,
                                                       sColumnValue->mInfoPtr->mValueType,
                                                       sColumnValue->mInfoPtr->mParameterType,
                                                       sColumnValue->mInfoPtr->mStringLengthUnit,
                                                       sColumnValue->mInfoPtr->mColumnSize,
                                                       sColumnValue->mInfoPtr->mDecimalDigits,
                                                       sColumnValue->mRetStrValue,
                                                       sColumnValue->mInfoPtr->mBufferLength,
                                                       sColumnValue->mRetIndicator,
                                                       aErrorStack ) == STL_SUCCESS );
                    }
                    break;
                }
                case DTL_TYPE_DECIMAL:
                case DTL_TYPE_BLOB:
                case DTL_TYPE_CLOB:
                {
                    /** Not Supported Yet.*/
                    STL_THROW( RAMP_ERR_INVALID_DATATYPE );
                    break;
                }
                default:
                    break;
            }
            
            sColumnIdx++;
        }
    }
            
    
    /**
     * Array중 비어있는 Array는 Ignore를 세팅한다.
     */
    if( aTableInfo->mNeedUpdatePrepare == STL_TRUE ||
        sStmtInfo->mUpdateIdx != sStmtInfo->mUpdatePrevIdx )
    {
        sStmtInfo->mUpdatePrevIdx = sStmtInfo->mUpdateIdx;
        
        for( sIdx = 0 ; sIdx < sArraySize; sIdx++ )
        {
            if( sIdx >= sStmtInfo->mUpdateIdx )
            {
                sStmtInfo->mUpdateOperation[ sIdx ] = SQL_PARAM_IGNORE;
            }
            else
            {
                sStmtInfo->mUpdateOperation[ sIdx ] = SQL_PARAM_PROCEED;
            }
        }
    }
    
    sResult = SQLExecute( sStmtInfo->mUpdateStmt );
    
    if( sResult == SQL_SUCCESS || 
        ( sResult == SQL_SUCCESS_WITH_INFO && sUpdateMode == 2 ) )
    {
        if( sUpdateMode == 2 )
        {
            //Compare .. TODO Array 처리 안했음..!!
            STL_RING_FOREACH_ENTRY( &aTableInfo->mColumnList, sColumnValue )
            {
                if( sColumnValue->mInfoPtr->mIsPrimary == STL_TRUE )
                {
                    continue;
                }
        
                if( sColumnValue->mIsUpdated != STL_TRUE )
                {
                    continue;
                }
                
                switch( sColumnValue->mInfoPtr->mDataTypeId )
                {
                    case DTL_TYPE_CHAR:
                    case DTL_TYPE_VARCHAR:
                    case DTL_TYPE_BINARY:
                    case DTL_TYPE_VARBINARY:
                    case DTL_TYPE_BOOLEAN:
                    case DTL_TYPE_NATIVE_SMALLINT:
                    case DTL_TYPE_NATIVE_INTEGER:
                    case DTL_TYPE_NATIVE_BIGINT:
                    case DTL_TYPE_NATIVE_REAL:
                    case DTL_TYPE_NATIVE_DOUBLE:
                    case DTL_TYPE_FLOAT:
                    case DTL_TYPE_NUMBER:
                    case DTL_TYPE_DATE:
                    case DTL_TYPE_TIME:
                    case DTL_TYPE_TIME_WITH_TIME_ZONE:
                    case DTL_TYPE_TIMESTAMP:
                    case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:
                    {
                        if( *sColumnValue->mRetIndicator == sColumnValue->mBefIndicator[0] )
                        {
                            for( sIdx = 0; sIdx < *sColumnValue->mRetIndicator; sIdx++ )
                            {
                                if( sColumnValue->mRetValue[sIdx] != sColumnValue->mBefValue[sIdx] )
                                {
                                    sCompare = STL_FALSE;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            sCompare = STL_FALSE;
                        }
                        break;
                    }
                    case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:
                    case DTL_TYPE_INTERVAL_DAY_TO_SECOND:
                    {
                        //stlPrintf("[%s][%d][%s][%d]\n", sColumnValue->mBefStrValue, *sColumnValue->mRetIndicator, sColumnValue->mRetStrValue, sColumnValue->mBefIndicator[0] );
                        if( *sColumnValue->mRetIndicator == sColumnValue->mBefIndicator[0] )
                        {
                            
                            for( sIdx = 0; sIdx < sColumnValue->mBefIndicator[0]; sIdx++ )
                            {
                                if( sColumnValue->mBefStrValue[sIdx] != sColumnValue->mRetStrValue[sIdx] )
                                {
                                    sCompare = STL_FALSE;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            sCompare = STL_FALSE;
                        }
                        break;
                    }
                    case DTL_TYPE_LONGVARCHAR:
                    case DTL_TYPE_LONGVARBINARY:
                    {
                        if( *sColumnValue->mRetIndicator == sColumnValue->mBefValueSize[0] )
                        {
                            /**
                             * long varchar는 우선 길이만 체크하도록 함!!
                             */
                            if( *sColumnValue->mRetIndicator != 0 )
                            {
                                //sVariableStr = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)&sColumnValue->mBefValue[ 0 ];
                            }
                        }
                        else
                        {
                            sCompare = STL_FALSE;
                        }
                        break;
                    }
                    case DTL_TYPE_DECIMAL:
                    case DTL_TYPE_BLOB:
                    case DTL_TYPE_CLOB:
                    {
                        /** Not Supported Yet.*/
                        STL_THROW( RAMP_ERR_INVALID_DATATYPE );
                        break;
                    }
                    default:
                        break;
                }
                
                if( sCompare != STL_TRUE )
                {
                    stlStrcpy( sMsg, "[APPLIER-UPDATE] Different Before-images.\n"); 
                    
                    STL_TRY( ztcaWriteErrorValueToLog( sMsg, 
                                                       STL_FALSE,
                                                       aTableInfo,
                                                       aErrorStack ) == STL_SUCCESS );
                    
                    break;
                }
            }
        }
        
        /**
         * EXECUTE가 성공했을때만 증가시킨다.
         */
        aApplierMgr->mApplyCount            += sStmtInfo->mUpdateIdx;
        aApplierMgr->mApplyCountAfterCommit += sStmtInfo->mUpdateIdx;
    }
    else
    {
        if( sResult == SQL_NO_DATA )
        {
            stlStrcpy( sMsg, "[APPLIER-UPDATE] Conflict.\n"); 
            
            STL_TRY( ztcaWriteErrorValueToLog( sMsg, 
                                               STL_FALSE,
                                               aTableInfo,
                                               aErrorStack ) == STL_SUCCESS );
        }
        else
        {
            for( sIdx = 0; sIdx < sStmtInfo->mUpdateIdx; sIdx++ )
            {
                switch( sStmtInfo->mUpdateStatus[ sIdx ] )
                {
                    case SQL_PARAM_SUCCESS:
                    case SQL_PARAM_UNUSED :
                    case SQL_PARAM_DIAG_UNAVAILABLE :
                        break;
                    case SQL_PARAM_SUCCESS_WITH_INFO :
                    case SQL_PARAM_ERROR :
                    {
                        stlStrcpy( sMsg, "[APPLIER-UPDATE] "); 
                        
                        STL_TRY( ztcdGetDiagnosticMsg( SQL_HANDLE_STMT,
                                                       sStmtInfo->mUpdateStmt,
                                                       sErrRecNum++,
                                                       &sMsg[ stlStrlen( sMsg ) ],
                                                       aErrorStack ) == STL_SUCCESS );
                    
                        STL_TRY( ztcaWriteErrorValueToLog( sMsg, 
                                                           STL_FALSE,
                                                           aTableInfo,
                                                           aErrorStack ) == STL_SUCCESS );
                        break;
                    }
                    default:
                        break;
                }
            }
        }
    }
    
    /**
     * Long Variable Column이 Update된 경우에는 사용했던 메모리를 반납해야 한다.
     * Update인 경우에는 Updated 된 Column만 Free해야 한다.
     */
    if( aTableInfo->mUpdateLongVariableCol == STL_TRUE )
    {
        STL_RING_FOREACH_ENTRY( &aTableInfo->mColumnList, sColumnValue )
        {
            if( ( sColumnValue->mInfoPtr->mDataTypeId == DTL_TYPE_LONGVARCHAR || sColumnValue->mInfoPtr->mDataTypeId == DTL_TYPE_LONGVARBINARY ) && 
                  sColumnValue->mIsUpdated == STL_TRUE )
            {
                for( sIdx = 0; sIdx < sStmtInfo->mUpdateIdx; sIdx++ )
                {
                    sVariableStr = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)&sColumnValue->mValue[ STL_SIZEOF( SQL_LONG_VARIABLE_LENGTH_STRUCT ) * sIdx ];
                    
                    if( sVariableStr->len > 0 )
                    {
                        STL_TRY( stlFreeDynamicMem( &(aApplierMgr->mLongVaryingMem),
                                                     sVariableStr->arr,
                                                     aErrorStack ) == STL_SUCCESS );
                        sVariableStr->len = 0;
                        sVariableStr->arr = NULL;
                    }
                    
                    if( sUpdateMode == 1 )
                    {
                        sVariableStr = (SQL_LONG_VARIABLE_LENGTH_STRUCT*)&sColumnValue->mBefValue[ STL_SIZEOF( SQL_LONG_VARIABLE_LENGTH_STRUCT ) * sIdx ];
                        
                        if( sVariableStr->len > 0 )  //Null인 경우가 있으나...
                        {
                            STL_TRY( stlFreeDynamicMem( &(aApplierMgr->mLongVaryingMem),
                                                         sVariableStr->arr,
                                                         aErrorStack ) == STL_SUCCESS );
                        
                            sVariableStr->len = 0;
                            sVariableStr->arr = NULL;
                        }
                    }
                }
            }
        } //end of RING
    }
    
    aTableInfo->mUpdateLongVariableCol = STL_FALSE;
    
    aApplierMgr->mPrevTableInfo = NULL;
    aApplierMgr->mPrevTransType = ZTC_CAPTURE_TYPE_NONE;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_DATATYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_DATA_TYPE,
                      NULL,
                      aErrorStack,
                      sColumnValue->mInfoPtr->mDataTypeId );
    }
    STL_FINISH;
    
    return STL_FAILURE;    
}


stlStatus ztcdInitializeApplierStatement( ztcApplierMgr * aApplierMgr,
                                          ztcStmtInfo   * aStmtInfo,
                                          stlErrorStack * aErrorStack )
{
    stlInt32        sState = 0;
    stlInt64        sArrarySize = aApplierMgr->mArraySize;
    
    /**
     * INSERT STATEMENT
     */ 
    STL_TRY( ztcdSQLAllocHandle( aApplierMgr->mDbcHandle,
                                 &aStmtInfo->mInsertStmt,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( stlAllocRegionMem( &(aApplierMgr->mRegionMem),
                                STL_SIZEOF( SQLUSMALLINT ) * sArrarySize,
                                 (void**)&aStmtInfo->mInsertStatus,
                                aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlAllocRegionMem( &(aApplierMgr->mRegionMem),
                                STL_SIZEOF( SQLUSMALLINT ) * sArrarySize,
                                 (void**)&aStmtInfo->mInsertOperation,
                                aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( SQLSetStmtAttr( aStmtInfo->mInsertStmt,
                             SQL_ATTR_PARAM_BIND_TYPE,
                             (SQLPOINTER) SQL_BIND_BY_COLUMN,
                             0 ) == SQL_SUCCESS );

    STL_TRY( SQLSetStmtAttr( aStmtInfo->mInsertStmt,
                             SQL_ATTR_PARAMSET_SIZE,
                             (SQLPOINTER) sArrarySize,
                             0 ) == SQL_SUCCESS );
    
    STL_TRY( SQLSetStmtAttr( aStmtInfo->mInsertStmt,
                             SQL_ATTR_PARAM_STATUS_PTR,
                             aStmtInfo->mInsertStatus,
                             0 ) == SQL_SUCCESS );

    STL_TRY( SQLSetStmtAttr( aStmtInfo->mInsertStmt,
                             SQL_ATTR_PARAM_OPERATION_PTR, 
                             aStmtInfo->mInsertOperation, 
                             0 ) == SQL_SUCCESS );
    
    aStmtInfo->mInsertIdx     = 0;
    aStmtInfo->mInsertPrevIdx = 0;
    
    /**
     * DELETE STATEMENT
     */ 
    STL_TRY( ztcdSQLAllocHandle( aApplierMgr->mDbcHandle,
                                 &aStmtInfo->mDeleteStmt,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 2;
    
    STL_TRY( stlAllocRegionMem( &(aApplierMgr->mRegionMem),
                                STL_SIZEOF( SQLUSMALLINT ) * sArrarySize,
                                (void**)&aStmtInfo->mDeleteStatus,
                                aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlAllocRegionMem( &(aApplierMgr->mRegionMem),
                                STL_SIZEOF( SQLUSMALLINT ) * sArrarySize,
                                (void**)&aStmtInfo->mDeleteOperation,
                                aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( SQLSetStmtAttr( aStmtInfo->mDeleteStmt,
                             SQL_ATTR_PARAM_BIND_TYPE,
                             (SQLPOINTER) SQL_BIND_BY_COLUMN,
                             0 ) == SQL_SUCCESS );

    STL_TRY( SQLSetStmtAttr( aStmtInfo->mDeleteStmt,
                             SQL_ATTR_PARAMSET_SIZE,
                             (SQLPOINTER) sArrarySize,
                             0 ) == SQL_SUCCESS );
    
    STL_TRY( SQLSetStmtAttr( aStmtInfo->mDeleteStmt,
                             SQL_ATTR_PARAM_STATUS_PTR,
                             aStmtInfo->mDeleteStatus,
                             0 ) == SQL_SUCCESS );

    STL_TRY( SQLSetStmtAttr( aStmtInfo->mDeleteStmt,
                             SQL_ATTR_PARAM_OPERATION_PTR, 
                             aStmtInfo->mDeleteOperation, 
                             0 ) == SQL_SUCCESS );
    
    aStmtInfo->mDeleteIdx     = 0;
    aStmtInfo->mDeletePrevIdx = 0;
    
    /**
     * UPDATE STATEMENT
     */ 
    STL_TRY( ztcdSQLAllocHandle( aApplierMgr->mDbcHandle,
                                 &aStmtInfo->mUpdateStmt,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 3;
    
    STL_TRY( stlAllocRegionMem( &(aApplierMgr->mRegionMem),
                                STL_SIZEOF( SQLUSMALLINT ) * sArrarySize,
                                 (void**)&aStmtInfo->mUpdateStatus,
                                aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlAllocRegionMem( &(aApplierMgr->mRegionMem),
                                STL_SIZEOF( SQLUSMALLINT ) * sArrarySize,
                                 (void**)&aStmtInfo->mUpdateOperation,
                                aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( SQLSetStmtAttr( aStmtInfo->mUpdateStmt,
                             SQL_ATTR_PARAM_BIND_TYPE,
                             (SQLPOINTER) SQL_BIND_BY_COLUMN,
                             0 ) == SQL_SUCCESS );

    STL_TRY( SQLSetStmtAttr( aStmtInfo->mUpdateStmt,
                             SQL_ATTR_PARAMSET_SIZE,
                             (SQLPOINTER) sArrarySize,
                             0 ) == SQL_SUCCESS );
    
    STL_TRY( SQLSetStmtAttr( aStmtInfo->mUpdateStmt,
                             SQL_ATTR_PARAM_STATUS_PTR,
                             aStmtInfo->mUpdateStatus,
                             0 ) == SQL_SUCCESS );

    STL_TRY( SQLSetStmtAttr( aStmtInfo->mUpdateStmt,
                             SQL_ATTR_PARAM_OPERATION_PTR, 
                             aStmtInfo->mUpdateOperation, 
                             0 ) == SQL_SUCCESS );
    
    aStmtInfo->mUpdateIdx     = 0;
    aStmtInfo->mUpdatePrevIdx = 0;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    switch( sState )
    {
        case 3:
            (void)SQLFreeHandle( SQL_HANDLE_STMT,
                                 aStmtInfo->mUpdateStmt );
        case 2:
            (void)SQLFreeHandle( SQL_HANDLE_STMT,
                                 aStmtInfo->mDeleteStmt );
        case 1:
            (void)SQLFreeHandle( SQL_HANDLE_STMT,
                                 aStmtInfo->mInsertStmt );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztcdFinalizeApplierStatement( ztcStmtInfo   * aStmtInfo,
                                        stlErrorStack * aErrorStack )
{
    /**
     * INSERT STATEMENT
     */ 
    STL_TRY( ztcdSQLFreeHandle( aStmtInfo->mInsertStmt, 
                                aErrorStack ) == STL_SUCCESS );
    
    /**
     * DELETE STATEMENT
     */ 
    STL_TRY( ztcdSQLFreeHandle( aStmtInfo->mDeleteStmt, 
                                aErrorStack ) == STL_SUCCESS );
    
    /**
     * UPDATE STATEMENT
     */ 
    STL_TRY( ztcdSQLFreeHandle( aStmtInfo->mUpdateStmt, 
                                aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcdInsertTableSlaveMeta( SQLHDBC             aDbcHandle,
                                    ztcSlaveTableInfo * aTableInfo,
                                    stlErrorStack     * aErrorStack )
{
    stlInt32    sState = 0;
    SQLHSTMT    sStmtHandle;
    SQLRETURN   sResult;
    
    STL_TRY( ztcdSQLAllocHandle( aDbcHandle,
                                 &sStmtHandle,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztcdSQLPrepare( sStmtHandle,
                             (SQLCHAR*)"INSERT INTO CYCLONE_SLAVE_META VALUES( ?, ?, ?, ?, ?, ? )",
                             SQL_NTS,
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindParameter( sStmtHandle,
                                   1,
                                   SQL_PARAM_INPUT,
                                   SQL_C_CHAR,
                                   SQL_VARCHAR,
                                   SQL_CLU_OCTETS,
                                   STL_MAX_SQL_IDENTIFIER_LENGTH,
                                   0,
                                   gConfigure.mCurrentGroupPtr->mName,
                                   STL_MAX_SQL_IDENTIFIER_LENGTH,
                                   0,
                                   aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindParameter( sStmtHandle,
                                   2,
                                   SQL_PARAM_INPUT,
                                   SQL_C_CHAR,
                                   SQL_VARCHAR,
                                   SQL_CLU_OCTETS,
                                   STL_MAX_SQL_IDENTIFIER_LENGTH,
                                   0,
                                   aTableInfo->mMasterSchema,
                                   STL_MAX_SQL_IDENTIFIER_LENGTH,
                                   0,
                                   aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindParameter( sStmtHandle,
                                   3,
                                   SQL_PARAM_INPUT,
                                   SQL_C_CHAR,
                                   SQL_VARCHAR,
                                   SQL_CLU_OCTETS,
                                   STL_MAX_SQL_IDENTIFIER_LENGTH,
                                   0,
                                   aTableInfo->mMasterTable,
                                   STL_MAX_SQL_IDENTIFIER_LENGTH,
                                   0,
                                   aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindParameter( sStmtHandle,
                                   4,
                                   SQL_PARAM_INPUT,
                                   SQL_C_CHAR,
                                   SQL_VARCHAR,
                                   SQL_CLU_OCTETS,
                                   STL_MAX_SQL_IDENTIFIER_LENGTH,
                                   0,
                                   aTableInfo->mSlaveSchema,
                                   STL_MAX_SQL_IDENTIFIER_LENGTH,
                                   0,
                                   aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindParameter( sStmtHandle,
                                   5,
                                   SQL_PARAM_INPUT,
                                   SQL_C_CHAR,
                                   SQL_VARCHAR,
                                   SQL_CLU_OCTETS,
                                   STL_MAX_SQL_IDENTIFIER_LENGTH,
                                   0,
                                   aTableInfo->mSlaveTable,
                                   STL_MAX_SQL_IDENTIFIER_LENGTH,
                                   0,
                                   aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindParameter( sStmtHandle,
                                   6,
                                   SQL_PARAM_INPUT,
                                   SQL_C_SLONG,
                                   SQL_INTEGER,
                                   SQL_CLU_NONE,
                                   0,
                                   0,
                                   &aTableInfo->mTableId,
                                   0,
                                   0,
                                   aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLExecute( sStmtHandle, 
                             &sResult, 
                             aErrorStack ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( ztcdSQLFreeHandle( sStmtHandle, 
                                aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLEndTran( SQL_HANDLE_DBC,
                             aDbcHandle,
                             SQL_COMMIT,
                             STL_TRUE,
                             aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    if( sState > 0 )
    {
        (void) ztcdSQLFreeHandle( sStmtHandle, 
                                  aErrorStack );
    }
    
    return STL_FAILURE;
}

/** @} */
