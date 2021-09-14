/*******************************************************************************
 * ztdConsumerThread.c
 *
 * Copyright (c) 2012, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztdConsumerThread.c 
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stl.h>
#include <dtl.h>
#include <goldilocks.h>
#include <ztdDef.h>
#include <ztdDisplay.h>
#include <ztdExecute.h>
#include <ztdExecThread.h>
#include <ztdFileExecute.h>
#include <ztdConsumerThread.h>

extern stlBool            gZtdRunState;
extern ztdManager         gZtdManager;
extern ztdQueueInfo     * gZtdQueue;
extern volatile stlUInt32 gZtdTextModeConsumerRunningCount; /* Text Import에서의 동작중인 consumer thread 개수 */
extern stlUInt32          gZtdErrorCount;
extern stlUInt32          gZtdMaxErrorCount;
stlUInt64                 gZtdDisplayCount;

/**
 * @brief gloader의 upload를 Multi-thread로 수행하는 Consumer Thread Func.
 * @param [in] aThread  stlthread.
 * @param [in] aArg     thread arguments.
 */ 
void * STL_THREAD_FUNC ztdImportTextDataThread( stlThread * aThread, void * aArg )
{
    SQLHENV             sEnv                 = NULL;
    SQLHDBC             sDbc                 = NULL;
    SQLHSTMT            sStmt                = NULL;
    SQLHDESC            sDescIpd             = NULL;
    SQLHDESC            sDescIrd             = NULL;
    SQLULEN             sTmpIdxRecord        = 0;
    SQLULEN             sRecordProcessed     = 0;
    SQLUSMALLINT      * sRecordStatusArray   = NULL;
    SQLINTEGER          sIntegerPrecision    = 0;
    SQLSMALLINT         sStringLengthUnit    = SQL_CLU_NONE;
    SQLRETURN           sRet                 = 0;
    
    stlErrorStack     * sErrorStack;    
    stlAllocator        sAllocator;
    
    stlChar             sSqlCommand[ZTD_MAX_COMMAND_BUFFER_SIZE];
    stlChar             sQuestionComma[]     = "?,";
    stlChar             sQuestionBracket[]   = "?)";

    stlInt32            sInsertCommandSize   = 0;
    stlChar           * sInsertCommand       = NULL;
    
    stlInt64            sChunkSize           = ZTD_CHUNK_SIZE;
    stlInt64            sRecordSize          = 0;
    stlInt64            sIdxRecord           = 0;
    stlInt64            sTmpIdx              = 0;    /*SQLExecute에서 error 발생한 처음 record 번호를 위한 index.*/
    stlInt64            sTmpCntOffset        = 0;    /*error 발생한 처음 record 번호. */
    stlInt64            sTmpCommitCnt        = 0;    /*temporary commit count*/
    stlUInt32           sTmpDisplayCnt       = 0;
    stlInt64          * sDataSize            = NULL;
    stlInt32            sState               = 0;
    stlInt16            sIdxColumn           = 0;
    
    ztdColumnInfo     * sColumnInfo          = NULL;
    ztdColDataImp     * sColumnData          = NULL;
    ztdFileAndBuffer    sFileAndBuffer;
    ztdControlInfo    * sControlInfo         = NULL;
    ztdParseInfo        sParseInfo;
    ztdChunkItem        sChunkItem;
    ztdQueueInfo      * sQueue               = NULL;

    stlInt64            sAllocSize           = 0;
    stlInt64            sFreeColumnIndex     = 0;
    stlInt64            sFreeRecordIndex     = 0;
    stlInt64            sErrRecordIndex      = 0;
 
    stlBool             sIsSQLCursor         = STL_FALSE;
    ztdInputArguments * sInputArgs           = NULL;

    SQLPOINTER          sDataPtr             = NULL;
    
    /**
     * phase 0.
     * 변수 초기화.
     */
    gZtdDisplayCount = 0;
    
    sEnv           = ((ztdThreadArg *) aArg)->mEnv;
    sInputArgs     = ((ztdThreadArg *) aArg)->mInputArguments;    
    sColumnInfo    = ((ztdThreadArg *) aArg)->mColumnInfo;
    sControlInfo   = ((ztdThreadArg *) aArg)->mControlInfo;
    sDataSize      = ((ztdThreadArg *) aArg)->mDataSize;
    sErrorStack    = &((ztdThreadArg *) aArg)->mErrorStack;
    sQueue         = ((ztdThreadArg *) aArg)->mQueue;

    ZTD_INIT_PARSE_INFO( sParseInfo );
    sParseInfo.mColumnCount   = ((ztdThreadArg *) aArg)->mColumnCount;

    /* file descriptor 복사. */
    stlMemcpy( &sFileAndBuffer,
               ((ztdThreadArg *) aArg)->mFileAndBuffer,
               STL_SIZEOF(ztdFileAndBuffer) );

    sFileAndBuffer.mBadRecord = NULL;

    sChunkItem.mBuffer  = NULL;
    sChunkItem.mSize    = sChunkSize;
    sChunkItem.mOffset  = 0;
    /**
     * phase 1.
     * DB 연결, 변수 할당.
     */

    STL_TRY( stlCreateRegionAllocator( &sAllocator,
                                       ZTD_REGION_INIT_SIZE,
                                       ZTD_REGION_NEXT_SIZE,
                                       sErrorStack )
             == STL_SUCCESS );
    sState = 1;
    /* 각 Consumer Thread가 gsql db 에 연결. */
    STL_TRY( ztdOpenDatabaseThread( sEnv,
                                    &sDbc,
                                    &sStmt,
                                    sInputArgs->mDsn,
                                    sInputArgs->mId,
                                    sInputArgs->mPassword,
                                    sControlInfo->mCharacterSet,
                                    sErrorStack )
             == STL_SUCCESS );
    sState = 2;
    
    /* ztdColDataImp을 column 개수 만큼 할당 */
    sAllocSize = STL_SIZEOF( ztdColDataImp ) * sParseInfo.mColumnCount;
    STL_TRY( stlAllocRegionMem( &sAllocator,
                                sAllocSize,
                                (void **) &sColumnData,
                                sErrorStack )
             == STL_SUCCESS );
    
    stlMemset( sColumnData,
               0x00,
               sAllocSize );

    /* sRecordStatusArray을 record 개수 만큼 할당 */
    sAllocSize = STL_SIZEOF( SQLSMALLINT ) * sInputArgs->mArraySize;
    STL_TRY( stlAllocRegionMem( &sAllocator,
                                sAllocSize,
                                (void **) &sRecordStatusArray,
                                sErrorStack )
             == STL_SUCCESS );

    stlMemset( sRecordStatusArray,
               0x00,
               sAllocSize );
    
    /* ztdFileAndBuffer의 ztdBadRecord를 Record 개수 만큼 할당 */
    sAllocSize = STL_SIZEOF(ztdBadRecord) * sInputArgs->mArraySize;
    STL_TRY( stlAllocRegionMem( &sAllocator,
                                sAllocSize,
                                (void **) &(sFileAndBuffer.mBadRecord),
                                sErrorStack )
             == STL_SUCCESS );

    stlMemset( sFileAndBuffer.mBadRecord,
               0x00,
               sAllocSize );
    
    for( sIdxRecord = 0; sIdxRecord < sInputArgs->mArraySize; sIdxRecord++ )
    {
        sRecordStatusArray[sIdxRecord] = 0;
        sFileAndBuffer.mBadRecord[sIdxRecord].mBuffer = NULL;
    }
    
    /**
     * Interval/Time/TimeStamp Data type의 precision 세팅을 위해
     * SQLGetDescField을 사용하고,이것은 Cursor가 Open state 이어야 하기에 SQLExecDirect사용.
     */
    stlSnprintf( sSqlCommand,
                 ZTD_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT * FROM %s.%s LIMIT 1",
                 sControlInfo->mSchema,
                 sControlInfo->mTable );

    STL_TRY( SQLExecDirect( sStmt,
                            (SQLCHAR * ) sSqlCommand,
                            SQL_NTS )
             == SQL_SUCCESS );
    sIsSQLCursor = STL_TRUE;
    
    /**
     * Alloc Insert Command Buffer
     */
    sInsertCommandSize  = ZTD_MAX_SCHEMA_NAME + ZTD_MAX_TABLE_NAME + ZTD_DEFAULT_COMMAND_SIZE;
    sInsertCommandSize += sParseInfo.mColumnCount * stlStrlen( sQuestionComma );

    STL_TRY( stlAllocRegionMem( &sAllocator,
                                sInsertCommandSize,
                                (void **) &sInsertCommand,
                                sErrorStack )
             == STL_SUCCESS );

    stlMemset( sInsertCommand,
               0x00,
               sInsertCommandSize );
    
    stlSnprintf( sInsertCommand,
                 sInsertCommandSize,
                 "INSERT INTO %s.%s VALUES (",
                 sControlInfo->mSchema,
                 sControlInfo->mTable );

    for( sIdxColumn = 0; sIdxColumn < sParseInfo.mColumnCount; sIdxColumn++ )
    {
        /* SQL Command에 "?," 추가. */
        if( sIdxColumn < sParseInfo.mColumnCount - 1)
        {
            stlStrncat( sInsertCommand, sQuestionComma, stlStrlen( sQuestionComma ) + 1 );
        }
        /* 1개의 Record 길이 계산. */
        sRecordSize += sDataSize[sIdxColumn] + ZTD_RECORD_EXTRA_SIZE;
    }
    stlStrncat( sInsertCommand, sQuestionBracket, stlStrlen( sQuestionBracket ) + 1 );

    /**
     * ColumnData 구조체 생성과 초기화.
     */
    for( sIdxColumn = 0; sIdxColumn < sParseInfo.mColumnCount; sIdxColumn++ )
    {
        /* ztdColDataImp 의 mIndicator를 Array 개수 만큼 할당 */
        sAllocSize = STL_SIZEOF( SQLLEN ) * sInputArgs->mArraySize;
        STL_TRY( stlAllocRegionMem( &sAllocator,
                                    sAllocSize,
                                    (void **) &(sColumnData[sIdxColumn].mIndicator),
                                    sErrorStack )
                 == STL_SUCCESS );
        /* Indicator 초기화. */
        stlMemset( sColumnData[sIdxColumn].mIndicator,
                   -1,
                   sAllocSize );

        /* offset 초기화. */
        sColumnData[sIdxColumn].mDataOffset = 0;
        
        /* SQL_LONG_VARIABLE_LENGTH_STRUCT 구조체 Array 개수만큼 할당. */
        sAllocSize = STL_SIZEOF( SQL_LONG_VARIABLE_LENGTH_STRUCT ) * sInputArgs->mArraySize;
        STL_TRY( stlAllocRegionMem( &sAllocator,
                                    sAllocSize,
                                    (void **) &sColumnData[sIdxColumn].mData,
                                    sErrorStack )
                 == STL_SUCCESS );

        stlMemset( sColumnData[sIdxColumn].mData,
                   0x00,
                   sAllocSize );
        
        /* SQL_LONG_VARIABLE_LENGTH_STRUCT 구조체 len 설정과 arr 생성. */
        for( sIdxRecord = 0; sIdxRecord < sInputArgs->mArraySize; sIdxRecord++ )
        {
            sColumnData[sIdxColumn].mData[sIdxRecord].len = sDataSize[sIdxColumn];

            STL_TRY_THROW( stlAllocHeap( (void **) &sColumnData[sIdxColumn].mData[sIdxRecord].arr,
                                         sColumnData[sIdxColumn].mData[sIdxRecord].len,
                                         sErrorStack ) == STL_SUCCESS,
                           RAMP_ERR_ALLOC_DATA_ARR );

            stlMemset( sColumnData[sIdxColumn].mData[sIdxRecord].arr,
                       0x00,
                       sColumnData[sIdxColumn].mData[sIdxRecord].len );
            
            sColumnData[sIdxColumn].mData[sIdxRecord].arr[0] = ZTD_CONSTANT_NULL;
        }
                 
    }
    sState = 3;
    
    /**
     * ztdBadRecord의 mBuffer를 mBufferSize만큼 할당
     * mBufferSize 는 NULL 문자를 위하여 RecordSize + 1
     */
    for( sIdxRecord = 0; sIdxRecord < sInputArgs->mArraySize; sIdxRecord++ )
    {
        sFileAndBuffer.mBadRecord[sIdxRecord].mOffset      = 0;
        sFileAndBuffer.mBadRecord[sIdxRecord].mBufferSize  = sRecordSize + 1;

        STL_TRY_THROW( stlAllocHeap( (void **) &(sFileAndBuffer.mBadRecord[sIdxRecord].mBuffer),
                                     sFileAndBuffer.mBadRecord[sIdxRecord].mBufferSize,
                                     sErrorStack ) == STL_SUCCESS,
                       RAMP_ERR_ALLOC_BAD_BUFFER );

        stlMemset( sFileAndBuffer.mBadRecord[sIdxRecord].mBuffer,
                   0x00,
                   sFileAndBuffer.mBadRecord[sIdxRecord].mBufferSize );
        
        sFileAndBuffer.mBadRecord[sIdxRecord].mBuffer[0] = ZTD_CONSTANT_NULL;
    }
    sState = 4;

    STL_TRY( stlAllocHeap( (void**) &sChunkItem.mBuffer,
                           sChunkItem.mSize,
                           sErrorStack )
             == STL_SUCCESS );
    sState = 5;
    
    stlMemset( sChunkItem.mBuffer,
               0x00,
               sChunkItem.mSize );
    
    /**
     * phase 2.
     * BindParameter와 세팅.
     */
    STL_TRY( SQLGetStmtAttr( sStmt,
                             SQL_ATTR_IMP_ROW_DESC,
                             &sDescIrd,
                             SQL_IS_POINTER,
                             NULL )
             == SQL_SUCCESS );
    
    STL_TRY( SQLGetStmtAttr( sStmt,
                             SQL_ATTR_IMP_PARAM_DESC,
                             &sDescIpd,
                             SQL_IS_POINTER,
                             NULL )
             == SQL_SUCCESS );
    
    for( sIdxColumn = 0; sIdxColumn < sParseInfo.mColumnCount; sIdxColumn++ )
    {
        STL_TRY( SQLBindParameter( sStmt,
                                   sIdxColumn + 1,
                                   SQL_PARAM_INPUT,
                                   SQL_C_LONGVARCHAR,
                                   sColumnInfo[sIdxColumn].mSQLType,
                                   sColumnInfo[sIdxColumn].mDataSize,
                                   sColumnInfo[sIdxColumn].mDecimalDigits,
                                   sColumnData[sIdxColumn].mData,
                                   0,
                                   sColumnData[sIdxColumn].mIndicator )
                 == SQL_SUCCESS );
        
        /* Character Length Unit 이 필요한 Data Type */
        switch( sColumnInfo[sIdxColumn].mSQLType )
        {
            case SQL_CHAR :
            case SQL_VARCHAR :
                STL_TRY( SQLGetDescField( sDescIrd,
                                          sIdxColumn + 1,
                                          SQL_DESC_CHAR_LENGTH_UNITS,
                                          (SQLPOINTER) &sStringLengthUnit,
                                          STL_SIZEOF( SQLSMALLINT ),
                                          0 )
                         == SQL_SUCCESS );

                STL_TRY( SQLSetDescField( sDescIpd,
                                          sIdxColumn + 1,
                                          SQL_DESC_CHAR_LENGTH_UNITS,
                                          (SQLPOINTER)(stlVarInt) sStringLengthUnit,
                                          0 )
                         == SQL_SUCCESS );

                sDataPtr = NULL;
                STL_TRY( SQLSetDescField( sDescIpd,
                                          sIdxColumn + 1,
                                          SQL_DESC_DATA_PTR,
                                          sDataPtr,
                                          0 )
                         == SQL_SUCCESS );
                break;
            default :
                break;
        }
        
        /* Precision setting이 필요한 Data Type */
        switch( sColumnInfo[sIdxColumn].mSQLType )
        {
            case SQL_INTERVAL_YEAR:
            case SQL_INTERVAL_MONTH:
            case SQL_INTERVAL_DAY:
            case SQL_INTERVAL_HOUR:
            case SQL_INTERVAL_MINUTE:
            case SQL_INTERVAL_YEAR_TO_MONTH:
            case SQL_INTERVAL_DAY_TO_HOUR:
            case SQL_INTERVAL_DAY_TO_MINUTE:
            case SQL_INTERVAL_HOUR_TO_MINUTE:
            case SQL_INTERVAL_SECOND:
            case SQL_INTERVAL_DAY_TO_SECOND:
            case SQL_INTERVAL_HOUR_TO_SECOND:
            case SQL_INTERVAL_MINUTE_TO_SECOND:
                STL_TRY( SQLGetDescField( sDescIrd,
                                          sIdxColumn + 1,
                                          SQL_DESC_DATETIME_INTERVAL_PRECISION,
                                          (SQLPOINTER) &sIntegerPrecision,
                                          STL_SIZEOF( SQLINTEGER ),
                                          0 )
                         == SQL_SUCCESS );
                
                STL_TRY( SQLSetDescField( sDescIpd,
                                          sIdxColumn + 1,
                                          SQL_DESC_DATETIME_INTERVAL_PRECISION,
                                          (SQLPOINTER)(stlVarInt) sIntegerPrecision,
                                          0 )
                         == SQL_SUCCESS );

                sDataPtr = NULL;
                STL_TRY( SQLSetDescField( sDescIpd,
                                          sIdxColumn + 1,
                                          SQL_DESC_DATA_PTR,
                                          sDataPtr,
                                          0 )
                         == SQL_SUCCESS );
                break;
            default:
                break;
        }
    }
    
    sIsSQLCursor = STL_FALSE;
    STL_TRY( SQLCloseCursor( sStmt ) == SQL_SUCCESS );
    
    STL_TRY( SQLPrepare( sStmt,
                         (SQLCHAR *) sInsertCommand,
                         SQL_NTS )
             == SQL_SUCCESS );
    /**
     * Column Wise Bind을 위한 setting
     */
    STL_TRY( SQLSetStmtAttr( sStmt,
                             SQL_ATTR_PARAM_BIND_TYPE,
                             (SQLPOINTER) SQL_BIND_BY_COLUMN,
                             0 )
             == SQL_SUCCESS );
    
    STL_TRY( SQLSetStmtAttr( sStmt,
                             SQL_ATTR_PARAM_STATUS_PTR,
                             sRecordStatusArray,
                             0 )
             == SQL_SUCCESS );

    STL_TRY( SQLSetStmtAttr( sStmt,
                             SQL_ATTR_PARAMS_PROCESSED_PTR,
                             &sRecordProcessed,
                             0 )
             == SQL_SUCCESS );

        
    if( sInputArgs->mUseAtomic == STL_TRUE )
    {
        STL_TRY( SQLSetStmtAttr( sStmt,
                                 SQL_ATTR_ATOMIC_EXECUTION,
                                 (SQLPOINTER) SQL_ATOMIC_EXECUTION_ON,
                                 0 )
             == SQL_SUCCESS );
    }
    
    /**
     * phase 3.
     * Producer Thread에서 생성한 Data(gZtdRecordChunk)를 Copy하고,
     * Parsing하여 sColumnData[].mDataBufferArray 에 저장.
     * SQLExecute로 Data Insert.
     */
    
    while( STL_TRUE )
    {
        STL_TRY( gZtdRunState == STL_TRUE );

        STL_TRY( ztdGetChunkFromQueue( &sChunkItem,
                                       sQueue,
                                       sErrorStack ) == STL_SUCCESS );

        sParseInfo.mReadSize   = sChunkItem.mOffset;
        sParseInfo.mReadOffset = 0;
        sParseInfo.mIdxRecord  = 0;
        sParseInfo.mState      = ZTD_STATE_INIT;

        STL_TRY( ztdParseChunk( sChunkItem.mBuffer,
                                &sFileAndBuffer,
                                sColumnData,
                                &sParseInfo,
                                sControlInfo,
                                sErrorStack )
                 == STL_SUCCESS );
         
        sChunkItem.mBuffer[0] = '\0';
        sChunkItem.mOffset    = 0;

        /**
         * SQLExecute 종료 조건: ChunkItem의 Offset이 0 일 때 즉 ReadSize가 0인 경우.
         */ 
        if( (sParseInfo.mIdxRecord == 0) &&
            (sParseInfo.mReadSize == 0) )
        {
            break;
        }
        else
        {
            if( sParseInfo.mIdxRecord == 0 )
            {
                continue;
            }
            else if( sParseInfo.mIdxRecord != sTmpIdxRecord )
            {
                STL_TRY( SQLSetStmtAttr( sStmt,
                                         SQL_ATTR_PARAMSET_SIZE,
                                         (SQLPOINTER)(stlVarInt)sParseInfo.mIdxRecord,
                                         0 )
                         == SQL_SUCCESS );
            }
            sTmpIdxRecord = sParseInfo.mIdxRecord;
        }
                
        sRet = SQLExecute( sStmt );

        switch( sRet )
        {
            case SQL_SUCCESS:
                break;
            case SQL_SUCCESS_WITH_INFO:
                for( sIdxRecord = 0; sIdxRecord < sRecordProcessed; sIdxRecord++ )
                {
                    switch( sRecordStatusArray[sIdxRecord] )
                    {
                        case SQL_PARAM_SUCCESS:
                            break;
                        case SQL_PARAM_SUCCESS_WITH_INFO:
                        case SQL_PARAM_ERROR:
                        case SQL_PARAM_DIAG_UNAVAILABLE:
                            STL_TRY( ztdWriteBadSync( &sFileAndBuffer,
                                                      &(sFileAndBuffer.mBadRecord[ sIdxRecord ]),
                                                      &gZtdManager.mFileSema,
                                                      sErrorStack )
                                     == STL_SUCCESS );
                            sParseInfo.mErrorCount++;
                            break;
                        case SQL_PARAM_UNUSED:
                            break;
                        default :
                            break;
                    }
                }

                STL_TRY( ztdCheckError( sEnv,
                                        sDbc,
                                        sStmt,
                                        sChunkItem.mStartRecNum,
                                        &sFileAndBuffer,
                                        &gZtdManager.mFileSema,
                                        sErrorStack )
                         == STL_SUCCESS );
                break;
            case SQL_ERROR:
                /**
                 * Atomic Insert 에서 한 개의 레코드가 실패하면 전부 실패.
                 * 에러 발생한 처음 레코드를 찾고 그 record부터 log 파일에 기록.
                 * insert 실패한 모든 레코드를 bad 파일에 기록.
                 */
                for( sIdxRecord = 0, sTmpIdx = sRecordProcessed-1;
                     sIdxRecord < sRecordProcessed;
                     sIdxRecord++, sTmpIdx-- )
                {
                    switch( sRecordStatusArray[sTmpIdx] )
                    {
                        case SQL_PARAM_SUCCESS_WITH_INFO:
                        case SQL_PARAM_ERROR:
                        case SQL_PARAM_DIAG_UNAVAILABLE:
                            sTmpCntOffset = sTmpIdx;
                            break;
                    }
                    STL_TRY( ztdWriteBadSync( &sFileAndBuffer,
                                              &(sFileAndBuffer.mBadRecord[ sIdxRecord ]),
                                              &gZtdManager.mFileSema,
                                              sErrorStack )
                             == STL_SUCCESS );

                    sParseInfo.mErrorCount++;

                    stlAtomicInc32( &gZtdErrorCount );
                }

                if( sInputArgs->mUseAtomic == STL_TRUE )
                {
                    sChunkItem.mStartRecNum += sTmpCntOffset; /* error 발생한 처음 레코드 위치 계산*/
                }
                
                STL_TRY( ztdCheckError( sEnv,
                                        sDbc,
                                        sStmt,
                                        sChunkItem.mStartRecNum,
                                        &sFileAndBuffer,
                                        &gZtdManager.mFileSema,
                                        sErrorStack )
                         == STL_SUCCESS );
                break;
            case SQL_INVALID_HANDLE:
                STL_THROW( RAMP_ERR_INVALID_HANDLE );
                break;
            default:
                break;
        }    

        /**
         * Buffer & Indicator 초기화
         */ 
        for( sIdxRecord = 0; sIdxRecord < sInputArgs->mArraySize; sIdxRecord++ )
        {
            sFileAndBuffer.mBadRecord[sIdxRecord].mBuffer[0] = ZTD_CONSTANT_NULL;
            sFileAndBuffer.mBadRecord[sIdxRecord].mOffset = 0;               
        }

        for( sIdxColumn = 0; sIdxColumn < sParseInfo.mColumnCount; sIdxColumn++ )
        {
            stlMemset( sColumnData[sIdxColumn].mIndicator,
                       -1,
                       STL_SIZEOF(SQLLEN) * sInputArgs->mArraySize );
        }
        
        sParseInfo.mRecordCount += sParseInfo.mIdxRecord;
        sTmpCommitCnt           += sParseInfo.mIdxRecord;
        sTmpDisplayCnt          += sParseInfo.mIdxRecord;

        if( (gZtdErrorCount > gZtdMaxErrorCount) &&
            (gZtdMaxErrorCount > 0) )
        {
            STL_TRY( SQLEndTran( SQL_HANDLE_DBC,
                                 sDbc,
                                 SQL_COMMIT )
                     == SQL_SUCCESS );
            break;
        }

        if( sInputArgs->mSilent == STL_FALSE )
        {
            stlAcquireSpinLock( &gZtdManager.mConsAddLock,
                                NULL /* MissCount */ );
            
            gZtdDisplayCount += sParseInfo.mIdxRecord;

            stlReleaseSpinLock( &gZtdManager.mConsAddLock );

            if( sTmpDisplayCnt >= sInputArgs->mDisplayUnit )
            {
                sTmpDisplayCnt = 0;
                stlPrintf( "\nloaded %ld records into %s.%s\n\n",
                           gZtdDisplayCount,
                           sControlInfo->mSchema,
                           sControlInfo->mTable );
            }
        }
        
        if( sTmpCommitCnt >= sInputArgs->mCommitUnit )
        {
            sTmpCommitCnt = 0;
            STL_TRY( SQLEndTran( SQL_HANDLE_DBC,
                                 sDbc,
                                 SQL_COMMIT )
                     == SQL_SUCCESS );
        }
    }

    gZtdManager.mCntRecord += sParseInfo.mRecordCount;
    gZtdManager.mCntBad    += sParseInfo.mErrorCount;

    sState = 4;
    stlFreeHeap( (void *) sChunkItem.mBuffer );
    sChunkItem.mBuffer = NULL;
    
    sState = 3;
    for( sIdxRecord = 0; sIdxRecord < sInputArgs->mArraySize; sIdxRecord++ )
    {
        stlFreeHeap( (void *) sFileAndBuffer.mBadRecord[sIdxRecord].mBuffer );
        sFileAndBuffer.mBadRecord[sIdxRecord].mBuffer = NULL;
    }

    sState = 2;
    for( sIdxColumn = 0; sIdxColumn < sParseInfo.mColumnCount; sIdxColumn++ )
    {
        for( sIdxRecord = 0; sIdxRecord < sInputArgs->mArraySize; sIdxRecord++ )
        {
            stlFreeHeap( (void *) sColumnData[sIdxColumn].mData[sIdxRecord].arr );
            sColumnData[sIdxColumn].mData[sIdxRecord].arr = NULL;
        }
    }
    
    sState = 1;
    STL_TRY( ztdCloseDatabaseThread( sDbc,
                                     sStmt,
                                     STL_TRUE,  /* COMMIT */
                                     sErrorStack )
             == STL_SUCCESS );

    sState = 0;
    STL_TRY( stlDestroyRegionAllocator( &sAllocator,
                                        sErrorStack )
             == STL_SUCCESS );

    (void) stlAtomicDec32( &gZtdTextModeConsumerRunningCount );
    
    STL_TRY( stlExitThread( aThread,
                            STL_SUCCESS,
                            sErrorStack )
             == STL_SUCCESS );
    
    return NULL;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTD_ERRCODE_INVALID_HANDLE,
                      NULL,
                      sErrorStack );
    }

    STL_CATCH( RAMP_ERR_ALLOC_DATA_ARR )
    {
        for( sFreeColumnIndex = 0; sFreeColumnIndex <= sIdxColumn; sFreeColumnIndex++ )
        {
            if( sFreeColumnIndex == sIdxColumn )
            {
                sErrRecordIndex = sIdxRecord;
            }
            else
            {
                sErrRecordIndex = sInputArgs->mArraySize;
            }
            
            for( sFreeRecordIndex = 0; sFreeRecordIndex < sErrRecordIndex; sFreeRecordIndex++ )
            {
                stlFreeHeap( (void *) sColumnData[sFreeColumnIndex].mData[sFreeRecordIndex].arr );
                sColumnData[sFreeColumnIndex].mData[sFreeRecordIndex].arr = NULL;
            }
        }
    }

    STL_CATCH( RAMP_ERR_ALLOC_BAD_BUFFER )
    {
        for( sFreeRecordIndex = 0; sFreeRecordIndex < sIdxRecord; sFreeRecordIndex++ )
        {
            stlFreeHeap( (void *) sFileAndBuffer.mBadRecord[sFreeRecordIndex].mBuffer );
            sFileAndBuffer.mBadRecord[sFreeRecordIndex].mBuffer = NULL;
        }
    }
    
    STL_FINISH;

    /* stlAppendErrors( &((ztdThreadArg*)aArg)->mErrorStack, sErrorStack ); */

    if( sIsSQLCursor == STL_TRUE )
    {
        (void) SQLCloseCursor( sStmt );
    }
    else
    {
        /* Do Notting */
    }

    switch( sState )
    {
        case 5:
            if( sChunkItem.mBuffer != NULL )
            {
                stlFreeHeap( (void *) sChunkItem.mBuffer );
                sChunkItem.mBuffer = NULL;
            }
        case 4:
            if( sFileAndBuffer.mBadRecord != NULL )
            {
                for( sIdxRecord = 0; sIdxRecord < sInputArgs->mArraySize; sIdxRecord++ )
                {
                    if( sFileAndBuffer.mBadRecord[sIdxRecord].mBuffer != NULL )
                    {
                        stlFreeHeap( (void *) sFileAndBuffer.mBadRecord[sIdxRecord].mBuffer );
                        sFileAndBuffer.mBadRecord[sIdxRecord].mBuffer = NULL;
                    }
                }
            }
        case 3:
            for( sIdxColumn = 0; sIdxColumn < sParseInfo.mColumnCount; sIdxColumn++ )
            {
                for( sIdxRecord = 0; sIdxRecord < sInputArgs->mArraySize; sIdxRecord++ )
                {
                    if( sColumnData[sIdxColumn].mData[sIdxRecord].arr != NULL )
                    {
                        stlFreeHeap( (void *) sColumnData[sIdxColumn].mData[sIdxRecord].arr );
                        sColumnData[sIdxColumn].mData[sIdxRecord].arr = NULL;
                    }
                }
            }
        case 2:
            (void) ztdCheckError( sEnv,
                                  sDbc,
                                  sStmt,
                                  0,
                                  &sFileAndBuffer,
                                  &gZtdManager.mFileSema,
                                  sErrorStack );

            (void) ztdCloseDatabaseThread( sDbc,
                                           sStmt,
                                           STL_FALSE,  /* COMMIT */
                                           sErrorStack );
        case 1:
            (void) stlDestroyRegionAllocator( &sAllocator,
                                              sErrorStack );
        default:
            (void) stlAtomicDec32( &gZtdTextModeConsumerRunningCount );
            
            (void) stlExitThread( aThread,
                                  STL_FAILURE,
                                  sErrorStack );
            break;
    }
    return NULL;
}

/**
 * @brief ztdOpenDatabaseThread()  Sub-Thread에서 Database Close.
 * @param [in]     aEnv          Environment Handle.
 * @param [in/out] aDbc          Dbc Handle.
 * @param [in/out] aStmt         Statement Handle.
 * @param [in[     aId           User Id.
 * @param [in[     aPassword     Password.
 * @param [in[     aCharacterSet clinet character set.
 * @param [in/out] aErrorStack   error stack
 */ 
stlStatus ztdOpenDatabaseThread( SQLHENV         aEnv,
                                 SQLHDBC       * aDbc,
                                 SQLHSTMT      * aStmt,
                                 stlChar       * aDsn,
                                 stlChar       * aId,
                                 stlChar       * aPassword,
                                 stlChar       * aCharacterSet,
                                 stlErrorStack * aErrorStack)
{
    SQLRETURN sReturn;
    stlInt32  sState = 0;

    STL_TRY( SQLAllocHandle( SQL_HANDLE_DBC,
                             aEnv,
                             aDbc )
             == SQL_SUCCESS );
    sState = 1;

    sReturn = SQLConnect( *aDbc,
                          (SQLCHAR*) aDsn, //"GLIESE",
                          SQL_NTS,
                          (SQLCHAR *) aId,
                          SQL_NTS,
                          (SQLCHAR *) aPassword,
                          SQL_NTS );
    sState = 2;

    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_CONNECTION );
    
    STL_TRY( SQLAllocHandle( SQL_HANDLE_STMT,
                             *aDbc,
                             aStmt )
             == SQL_SUCCESS );
    sState = 3;

    sReturn = SQLSetConnectAttr( *aDbc,
                                 SQL_ATTR_CHARACTER_SET,
                                 (SQLPOINTER)aCharacterSet,
                                 SQL_NTS );

    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_CONNECTION );
    
    sReturn = SQLSetConnectAttr( *aDbc,
                                 SQL_ATTR_AUTOCOMMIT,
                                 (SQLPOINTER) SQL_AUTOCOMMIT_OFF,
                                 SQL_IS_UINTEGER );
    
    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_CONNECTION );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_CONNECTION )
    {
        (void)ztdPrintDiagnostic( SQL_HANDLE_DBC,
                                  *aDbc,
                                  aErrorStack );
    }
        
    STL_FINISH;

    switch( sState )
    {
        case 3:
            (void) SQLFreeHandle( SQL_HANDLE_STMT,
                                  *aStmt );
        case 2 :
            (void) SQLDisconnect( *aDbc );
        case 1:
            (void) SQLFreeHandle( SQL_HANDLE_DBC,
                                  *aDbc );
        default:
            break;
    }
    return STL_FAILURE;
}

/**
 * @brief ztdCloseDatabaseThread()  Sub-Thread에서 Database Close.
 * @param [in] aDbc         Dbc Handle
 * @param [in] aStmt        Statement Handle
 * @param [in] aErrorStack  error stack
 */ 
stlStatus ztdCloseDatabaseThread( SQLHDBC         aDbc,
                                  SQLHSTMT        aStmt,
                                  stlBool         aIsCommit,
                                  stlErrorStack * aErrorStack )
{
    stlInt32 sState = 3;
    
    STL_TRY( SQLFreeHandle( SQL_HANDLE_STMT, aStmt )
             == SQL_SUCCESS );
    
    sState = 2;
    if( aIsCommit == STL_TRUE )
    {
        STL_TRY( SQLEndTran( SQL_HANDLE_DBC,
                             aDbc,
                             SQL_COMMIT )
                 == SQL_SUCCESS );
    }
    else
    {
        STL_TRY( SQLEndTran( SQL_HANDLE_DBC,
                             aDbc,
                             SQL_ROLLBACK )
                 == SQL_SUCCESS );
    }
    
    sState = 1;
    STL_TRY( SQLDisconnect( aDbc )
             == SQL_SUCCESS );
    
    sState = 0;    
    STL_TRY( SQLFreeHandle( SQL_HANDLE_DBC, aDbc )
             == SQL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 3:
            if( aIsCommit == STL_TRUE )
            {
                (void) SQLEndTran( SQL_HANDLE_DBC,
                                   aDbc,
                                   SQL_COMMIT );
            }
            else
            {
                (void)SQLEndTran( SQL_HANDLE_DBC,
                                  aDbc,
                                  SQL_ROLLBACK );
            }
        case 2:
            (void) SQLDisconnect( aDbc );
        case 1:
            (void) SQLFreeHandle( SQL_HANDLE_DBC, aDbc );
        default:
            break;
    }
    return STL_FAILURE;
}

/**
 * @brief ztdParseChunk   Buffer를 Parsing하여 Array 단위로 Record 구성.
 * @param [in]     aReadBuffer     Parsed Records Chunk.
 * @param [in]     aFileAndBuffer  File Descriptor와 Log/Bad Buffer. 
 * @param [in/out] aColumnData     Column의 Data와 Indicator.
 * @param [in/out] aParseInfo      Parsing 상태 정보.
 * @param [in]     aControlInfo    Control File의 내용.
 * @param [in/out] aErrorSTack     error stack.
 */ 
stlStatus ztdParseChunk( stlChar           * aReadBuffer,
                         ztdFileAndBuffer  * aFileAndBuffer,
                         ztdColDataImp     * aColumnData,
                         ztdParseInfo      * aParseInfo,
                         ztdControlInfo    * aControlInfo,
                         stlErrorStack     * aErrorStack )
{
    /**
     * aParseInfo->mReadSize는 Producer에서 받아온
     * ChunkItem의 유효 Offset과 같고 여기까지만 진행.
     */
    while( aParseInfo->mReadOffset < aParseInfo->mReadSize )
    {
        /* 하나의 Record를 구성 */
        STL_TRY( ztdParseRecord( aReadBuffer,
                                 &aFileAndBuffer->mBadRecord[ aParseInfo->mIdxRecord ],
                                 aColumnData,
                                 aParseInfo,
                                 aControlInfo,
                                 aErrorStack )
                 == STL_SUCCESS );

        if( aParseInfo->mState == ZTD_STATE_BAD )
        {
            STL_TRY( ztdWriteBadSync( aFileAndBuffer,
                                      &aFileAndBuffer->mBadRecord[aParseInfo->mIdxRecord],
                                      &gZtdManager.mFileSema,
                                      aErrorStack )
                     == STL_SUCCESS );
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief ztdParseRecord   Data(aReadBuffer)를 Parsing 하여 하나의 Record 구성.
 * @param [in]     aReadBuffer     Parsed Records Chunk.
 * @param [in/out] aBadRecord      Bad Record 기록하는 struct.
 * @param [in/out] aColumnData     Column의 Data와 Indicator.
 * @param [in/out] aParseInfo      Parsing 상태 정보.
 * @param [in]     aControlInfo    Control File의 내용.
 * @param [in/out] aErrorSTack     error stack.
 */ 
stlStatus ztdParseRecord( stlChar           * aReadBuffer,
                          ztdBadRecord      * aBadRecord,
                          ztdColDataImp     * aColumnData,
                          ztdParseInfo      * aParseInfo,
                          ztdControlInfo    * aControlInfo,
                          stlErrorStack     * aErrorStack )
{
    stlChar  sCurrChar;
    stlChar  sOpenQual;
    stlBool  sFinish;
    
    sOpenQual  = aControlInfo->mOpQualifier;

    while( STL_TRUE )
    {        
        /* white space 체크 */
        while( STL_TRUE )
        {
            if( aParseInfo->mReadOffset == aParseInfo->mReadSize )
            {
                if( aParseInfo->mIdxColumn != 0 )
                {
                    aParseInfo->mIdxRecord++;
                }
                STL_THROW( RAMP_EXIT_SUCCESS );
            }
            
            sCurrChar = aReadBuffer[ aParseInfo->mReadOffset ];
            aParseInfo->mReadOffset++;

            ZTD_CONSUMER_SET_BAD_BUFFER( sCurrChar,
                                         aBadRecord,
                                         aErrorStack );
            
            if( ( sCurrChar == ZTD_CONSTANT_TAB ) ||
                ( sCurrChar == ZTD_CONSTANT_SPACE ) ||
                ( sCurrChar == ZTD_CONSTANT_CARRIAGE_RETURN ) )
            {
                /* nothing to do */
                continue;
            }
            else if( sCurrChar == ZTD_CONSTANT_LINE_FEED )
            {
                if( aParseInfo->mIdxColumn == 0 )
                {
                    if( aBadRecord->mBuffer[ (aBadRecord->mOffset - 1) ]
                        == ZTD_CONSTANT_CARRIAGE_RETURN )
                    {
                        aBadRecord->mOffset--;
                    }
                    /* Bad Record에 기록하지 않는다. */
                    aBadRecord->mOffset--;
                    continue;
                }
                else
                {
                    aParseInfo->mIdxRecord++;
                    aParseInfo->mIdxColumn = 0;
                    STL_THROW( RAMP_EXIT_SUCCESS );
                }
            }
            else
            {
                break;
            }
        }
        
        /* OVER COLUMN 체크 */
        if( aParseInfo->mIdxColumn == aParseInfo->mColumnCount )
        {
            aParseInfo->mReadOffset--;
            aBadRecord->mOffset--;
            STL_TRY( ztdDiscardOverColumn( aReadBuffer,
                                           aBadRecord,
                                           aParseInfo,
                                           aControlInfo,
                                           aErrorStack )
                     == STL_SUCCESS );

            STL_TRY( ztdManageParsingState( aReadBuffer,
                                            aParseInfo,
                                            aBadRecord,
                                            &sFinish,
                                            aErrorStack )
                     == STL_SUCCESS );

            if( sFinish == STL_TRUE )
            {
                STL_THROW( RAMP_EXIT_SUCCESS );
            }
            else
            {
                continue;
            }
        }
        
        if( sCurrChar == sOpenQual )
        {
            STL_TRY( ztdParseColumnWithOption( aReadBuffer,
                                               aBadRecord,
                                               &aColumnData[aParseInfo->mIdxColumn],
                                               aParseInfo,
                                               aControlInfo,
                                               aErrorStack )
                     == STL_SUCCESS );
        }
        else
        {   /* Qualifier 아니라면 첫 문자를 다시 Parsing */
            aBadRecord->mOffset--;
            aParseInfo->mReadOffset--;            
            STL_TRY( ztdParseColumnWithoutOption( aReadBuffer,
                                                  aBadRecord,
                                                  &aColumnData[aParseInfo->mIdxColumn],
                                                  aParseInfo,
                                                  aControlInfo,
                                                  aErrorStack )
                     == STL_SUCCESS );
        }

        STL_TRY( ztdManageParsingState( aReadBuffer,
                                        aParseInfo,
                                        aBadRecord,
                                        &sFinish,
                                        aErrorStack )
                 == STL_SUCCESS );

        if( sFinish == STL_TRUE )
        {
            STL_THROW( RAMP_EXIT_SUCCESS );
        }
    }

    STL_RAMP( RAMP_EXIT_SUCCESS );

    ZTD_CONSUMER_SET_BAD_BUFFER( ZTD_CONSTANT_NULL,
                                 aBadRecord,
                                 aErrorStack );
    aBadRecord->mOffset--;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief ztdParseColumnWithoutOption Qualifier 없는 Column 파싱.
 * @param [in]     aReadBuffer     Parsing 되는 Character Data
 * @param [out]    aBadRecord      Bad Record 기록하는 struct.
 * @param [out]    aColumnData     Column의 Data와 Indicator.
 * @param [in/out] aParseInfo      Parsing 상태 정보.
 * @param [in]     aControlInfo    Control File의 내용.
 * @param [in/out] aErrorSTack     error stack.
 */ 
stlStatus ztdParseColumnWithoutOption( stlChar           * aReadBuffer,
                                       ztdBadRecord      * aBadRecord,
                                       ztdColDataImp     * aColumnData,
                                       ztdParseInfo      * aParseInfo,
                                       ztdControlInfo    * aControlInfo,
                                       stlErrorStack     * aErrorStack )
{
    stlChar      * sDelimiter;
    stlChar        sCurrChar;
    stlChar        sNextChar;
    SQLCHAR      * sDataBuffer;
    stlInt64       sDataOffset;
    stlInt32       sDelimiterLen; 
    stlBool        sIsDelimiter;
    
    sDelimiter   = aControlInfo->mDelimiter;
    sDelimiterLen = stlStrlen( aControlInfo->mDelimiter );

    /**
     * sDataBuffer에 LONG_VARIABLE_LENGTH_STRUCT의 arr을 연결한다.
     */
    sDataOffset = 0;
    ZTD_SET_BUFFER( aColumnData,
                    sDataBuffer,
                    aParseInfo->mIdxRecord );
    
    aParseInfo->mState = ZTD_STATE_INIT;

    while( STL_TRUE )
    {
        if( aParseInfo->mReadOffset == aParseInfo->mReadSize )
        {
            ZTD_SET_INDICATOR( aColumnData->mIndicator[ aParseInfo->mIdxRecord ],
                               sDataOffset );
            aParseInfo->mState = ZTD_STATE_ROW_COMPLETE;
            break;
        }
        
        sCurrChar = aReadBuffer[ aParseInfo->mReadOffset ];
        aParseInfo->mReadOffset++;
        
        ZTD_CONSUMER_SET_BAD_BUFFER( sCurrChar,
                                     aBadRecord,
                                     aErrorStack );

        if( sCurrChar == sDelimiter[0] )
        {
            if( sDelimiterLen > 1 )
            { 
                ZTD_CONSUMER_CHECK_DELIMITER( aReadBuffer,
                                              aParseInfo,
                                              sDelimiter,
                                              sDelimiterLen,
                                              aBadRecord,
                                              sIsDelimiter,
                                              aErrorStack );

                if( sIsDelimiter == STL_FALSE )
                {
                    ZTD_CONSUMER_CHECK_ASSIGN_CHAR_TO_BUFFER( aColumnData,
                                                              aParseInfo,
                                                              sDataBuffer,
                                                              sDataOffset,
                                                              sCurrChar,
                                                              aErrorStack );
                    continue;
                }
            }
            
            ZTD_SET_INDICATOR( aColumnData->mIndicator[ aParseInfo->mIdxRecord ],
                               sDataOffset );

            aParseInfo->mState = ZTD_STATE_COLUMN_COMPLETE;
            break;
        }
        else if( ( sCurrChar == ZTD_CONSTANT_CARRIAGE_RETURN ) ||
                 ( sCurrChar == ZTD_CONSTANT_LINE_FEED  ) )
        {
            if( aParseInfo->mReadOffset == aParseInfo->mReadSize )
            {
                sNextChar = ZTD_CONSTANT_NULL;
            }
            else
            {
                sNextChar = aReadBuffer[ aParseInfo->mReadOffset ];
            }

            if( ( sCurrChar == ZTD_CONSTANT_CARRIAGE_RETURN ) &&
                ( sNextChar == ZTD_CONSTANT_LINE_FEED ) )
            {
                aParseInfo->mReadOffset++;
                ZTD_CONSUMER_SET_BAD_BUFFER( sNextChar,
                                             aBadRecord,
                                             aErrorStack );
            }

            if( ( sCurrChar == ZTD_CONSTANT_CARRIAGE_RETURN ) &&
                ( sNextChar != ZTD_CONSTANT_LINE_FEED ) )
            {
                ZTD_CONSUMER_CHECK_ASSIGN_CHAR_TO_BUFFER( aColumnData,
                                                          aParseInfo,
                                                          sDataBuffer,
                                                          sDataOffset,
                                                          sCurrChar,
                                                          aErrorStack );
                continue;
            }
            
            sDataBuffer[sDataOffset] = ZTD_CONSTANT_NULL;

            ZTD_SET_INDICATOR( aColumnData->mIndicator[ aParseInfo->mIdxRecord ],
                               sDataOffset );

            aParseInfo->mState = ZTD_STATE_ROW_COMPLETE;
            break;
        }
        else
        {
            ZTD_CONSUMER_CHECK_ASSIGN_CHAR_TO_BUFFER( aColumnData,
                                                      aParseInfo,
                                                      sDataBuffer,
                                                      sDataOffset,
                                                      sCurrChar,
                                                      aErrorStack );
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief ztdParseColumnWithOption Qualifier 있는 Column 파싱.
 * @param [in]     aReadBuffer     Parsing 되는 Character Data
 * @param [out]    aBadRecord      Bad Record 기록하는 struct.
 * @param [out]    aColumnData     Column의 Data와 Indicator.
 * @param [in/out] aParseInfo      Parsing 상태 정보.
 * @param [in]     aControlInfo    Control File의 내용.
 * @param [in/out] aErrorSTack     error stack.
 */ 
stlStatus ztdParseColumnWithOption( stlChar           * aReadBuffer,
                                    ztdBadRecord      * aBadRecord,
                                    ztdColDataImp     * aColumnData,
                                    ztdParseInfo      * aParseInfo,
                                    ztdControlInfo    * aControlInfo,
                                    stlErrorStack     * aErrorStack )
{
    stlChar      * sDelimiter;
    stlChar        sCloseQual;
    stlChar        sCurrChar;
    stlChar        sNextChar;

    stlBool        sWhiteSpaceOn;
    stlBool        sStartedData;

    stlInt64       sDataOffset;
    stlInt32       sDelimiterLen;
    
    SQLCHAR      * sDataBuffer;
    stlBool        sIsDelimiter;

    sDelimiter = aControlInfo->mDelimiter;
    sCloseQual = aControlInfo->mClQualifier;
    sDelimiterLen = stlStrlen( aControlInfo->mDelimiter );
    
    sWhiteSpaceOn = STL_FALSE;
    sStartedData  = STL_FALSE;
    
    /**
     * sDataBuffer에 LONG_VARIABLE_LENGTH_STRUCT의 arr을 연결한다.
     */
    sDataOffset = 0;
    ZTD_SET_BUFFER( aColumnData,
                    sDataBuffer,
                    aParseInfo->mIdxRecord );
    
    aParseInfo->mState = ZTD_STATE_INIT;

    /**
     * 이 Routine에 왔다면 이미 Open Qualifier를 만났다.
     * 조건문을 줄이기 위해 Data 에서 Close Qualifier만 찾으면 된다. 
     */
    while( STL_TRUE )
    {
        if( aParseInfo->mReadOffset == aParseInfo->mReadSize )
        {
            if( sStartedData == STL_TRUE )
            {
                aParseInfo->mState = ZTD_STATE_BAD;
            }
            else
            {
                aParseInfo->mState  = ZTD_STATE_ROW_COMPLETE;
            }
            break;
        }
        
        sCurrChar = aReadBuffer[ aParseInfo->mReadOffset ];
        aParseInfo->mReadOffset++;

        if( aParseInfo->mReadOffset == aParseInfo->mReadSize )
        {
            sNextChar = ZTD_CONSTANT_NULL;   
        }
        else
        {
            sNextChar = aReadBuffer[ aParseInfo->mReadOffset ];
        }

        ZTD_CONSUMER_SET_BAD_BUFFER( sCurrChar,
                                     aBadRecord,
                                     aErrorStack );

        if( sCurrChar == sCloseQual )
        {
            do
            { /**
               * trim white space
               * - CloseQual 다음 문자가 Tab 또는 Space라면 이 문자를 넘어간다.
               */
                if( (sNextChar != sDelimiter[0]) &&
                    ( (sNextChar == ZTD_CONSTANT_TAB) ||
                      (sNextChar == ZTD_CONSTANT_SPACE ) ) )
                {
                    sWhiteSpaceOn = STL_TRUE;

                    ZTD_CONSUMER_SET_BAD_BUFFER( sNextChar,
                                                 aBadRecord,
                                                 aErrorStack );
                        
                    aParseInfo->mReadOffset++;
                    sNextChar = aReadBuffer[ aParseInfo->mReadOffset ];
                }

                /**
                 * Data( aReadBuffer ) 끝 확인: 함수 종료.
                 */ 
                if( aParseInfo->mReadOffset == aParseInfo->mReadSize )
                {
                    ZTD_CONSUMER_SET_BAD_BUFFER( sNextChar,
                                                 aBadRecord,
                                                 aErrorStack );
                    
                    sDataBuffer[ sDataOffset ] = ZTD_CONSTANT_NULL;

                    ZTD_SET_INDICATOR( aColumnData->mIndicator[ aParseInfo->mIdxRecord ],
                                       sDataOffset );

                    aParseInfo->mState = ZTD_STATE_ROW_COMPLETE;

                    STL_THROW( EXIT_SUCCESS_FUNCTION );
                }

            } while( (sNextChar != sDelimiter[0]) &&
                     ( (sNextChar == ZTD_CONSTANT_TAB) ||
                       (sNextChar == ZTD_CONSTANT_SPACE ) ) );

            ZTD_CONSUMER_SET_BAD_BUFFER( sNextChar,
                                         aBadRecord,
                                         aErrorStack );

            aParseInfo->mReadOffset++;

            if( sNextChar == sDelimiter[0] )
            {
                if( sDelimiterLen > 1 )
                {
                    ZTD_CONSUMER_CHECK_DELIMITER( aReadBuffer,
                                                  aParseInfo,
                                                  sDelimiter,
                                                  sDelimiterLen,
                                                  aBadRecord,
                                                  sIsDelimiter,
                                                  aErrorStack );

                    if( sIsDelimiter == STL_FALSE )
                    {
                        aParseInfo->mState = ZTD_STATE_BAD;
                        break;
                    }
                }
                
                sDataBuffer[ sDataOffset ] = ZTD_CONSTANT_NULL;

                ZTD_SET_INDICATOR( aColumnData->mIndicator[ aParseInfo->mIdxRecord ],
                                   sDataOffset );

                aParseInfo->mState = ZTD_STATE_COLUMN_COMPLETE;
                break;
                    
            }
            else if( ( sNextChar == ZTD_CONSTANT_LINE_FEED ) ||
                     ( sNextChar == ZTD_CONSTANT_CARRIAGE_RETURN ) )
            {
                if( sNextChar == ZTD_CONSTANT_CARRIAGE_RETURN )
                {
                    sNextChar = aReadBuffer[ aParseInfo->mReadOffset ];
                    aParseInfo->mReadOffset++;
                    
                    ZTD_CONSUMER_SET_BAD_BUFFER( sNextChar,
                                                 aBadRecord,
                                                 aErrorStack );

                    if( sNextChar != ZTD_CONSTANT_LINE_FEED )
                    {
                        continue;
                    }
                }

                sDataBuffer[ sDataOffset ] = ZTD_CONSTANT_NULL;

                ZTD_SET_INDICATOR( aColumnData->mIndicator[ aParseInfo->mIdxRecord],
                                   sDataOffset );

                aParseInfo->mState = ZTD_STATE_ROW_COMPLETE;
                break;
            }
            else if( sNextChar == sCloseQual )
            {
                if( sWhiteSpaceOn == STL_TRUE )
                {
                    aParseInfo->mState = ZTD_STATE_BAD;
                    break;
                }
                else
                {
                    ZTD_CONSUMER_CHECK_ASSIGN_CHAR_TO_BUFFER( aColumnData,
                                                              aParseInfo,
                                                              sDataBuffer,
                                                              sDataOffset,
                                                              sCurrChar,
                                                              aErrorStack );
                }
            }
            else
            {
                aParseInfo->mState = ZTD_STATE_BAD;
                break;
            }
        }
        else /* 현재 문자를 Bind 된 Buffer에 기록 */
        {
            ZTD_CONSUMER_CHECK_ASSIGN_CHAR_TO_BUFFER( aColumnData,
                                                      aParseInfo,
                                                      sDataBuffer,
                                                      sDataOffset,
                                                      sCurrChar,
                                                      aErrorStack );
        }
    }
    
    STL_RAMP( EXIT_SUCCESS_FUNCTION );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Parsing 완료 후 State에 따른 처리를 한다.
 * @param[in]     aReadBuffer  Parsing 되는 Character Data
 * @param[in,out] aParseInfo   Parsing 중 informaion
 * @param[in,out] aBadRecord   Record가 기록되는 Buffer와 inforamtion
 * @param[out]    aFinish      State 처리 후 동작
 * @param[in,out] aErrorStack  Error stack
 */ 
stlStatus ztdManageParsingState( stlChar       * aReadBuffer,
                                 ztdParseInfo  * aParseInfo,
                                 ztdBadRecord  * aBadRecord,
                                 stlBool       * aFinish,
                                 stlErrorStack * aErrorStack )
{
    stlBool sFinish = STL_FALSE;
    
    switch( aParseInfo->mState )
    {
        case ZTD_STATE_COLUMN_COMPLETE:
            aParseInfo->mIdxColumn++;
            sFinish = STL_FALSE;
            break;
        case ZTD_STATE_ROW_COMPLETE:
            aParseInfo->mIdxColumn = 0;
            aParseInfo->mIdxRecord++;
            sFinish = STL_TRUE;
            break;
        case ZTD_STATE_BAD:
            aParseInfo->mIdxColumn = 0;
            aParseInfo->mRecordCount++;
            aParseInfo->mErrorCount++;

            STL_TRY( ztdMakeBadRecord( aReadBuffer,
                                       aBadRecord,
                                       aParseInfo,
                                       aErrorStack ) == STL_SUCCESS );

            sFinish = STL_TRUE;
            break;
        default:
            STL_DASSERT( 0 );
            break;
    }

    *aFinish = sFinish;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief ztdMakeBadRecord linefeed가 나올 때 까지 BadRecord에 문자를 저장.
 * @param [in]     aReadBuffer  Parsing 되는 Character Data
 * @param [out]    aBadRecord   Bad Record 기록하는 struct
 * @param [in/out] aParseInfo   parsing info
 * @param [in/out] aErrorStack  error stack
 */ 
stlStatus ztdMakeBadRecord( stlChar       * aReadBuffer,
                            ztdBadRecord  * aBadRecord,
                            ztdParseInfo  * aParseInfo,
                            stlErrorStack * aErrorStack )
{
    stlChar sCurrChar;
    
    while( STL_TRUE )
    {
        if( aParseInfo->mReadOffset == aParseInfo->mReadSize )
        {
            break;
        }
        
        sCurrChar = aReadBuffer[ aParseInfo->mReadOffset ];
        aParseInfo->mReadOffset++;

        ZTD_CONSUMER_SET_BAD_BUFFER( sCurrChar,
                                     aBadRecord,
                                     aErrorStack );
        
        if( sCurrChar == ZTD_CONSTANT_LINE_FEED )
        {
            break;
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 현재 Target Table의 column 개수를 넘은 Record를 Bad Record에 기록한다.
 * @param [in]     aReadBuffer   Parsing 되는 Character data
 * @param [out]    aBadRecord    Record가 기록되는 Buffer와 inforamtion
 * @param [in,out] aParseInfo    Parsing 중 informaion
 * @param [in]     aControlInfo  Control File Information
 * @param [in,out] aErrorStack   error stack
 */ 
stlStatus ztdDiscardOverColumn( stlChar           * aReadBuffer,
                                ztdBadRecord      * aBadRecord,
                                ztdParseInfo      * aParseInfo,
                                ztdControlInfo    * aControlInfo,
                                stlErrorStack     * aErrorStack )
{
    stlChar   sCurrChar;
    stlChar * sDelimiter;
    stlChar   sOpenQual;
    stlChar   sCloseQual;
    stlChar   sNextChar;
    stlBool   sWhiteSpaceOn;
    stlBool   sStartedData;
    stlBool   sUsedQual;
    stlInt32  sDelimiterLen;
    stlBool   sIsDelimiter;

    sDelimiter = aControlInfo->mDelimiter;
    sOpenQual  = aControlInfo->mOpQualifier;
    sCloseQual = aControlInfo->mClQualifier;

    sWhiteSpaceOn = STL_FALSE;
    sStartedData  = STL_FALSE;
    sUsedQual     = STL_FALSE;

    sDelimiterLen = stlStrlen( sDelimiter );

    /**
     * 종료 조건
     * 1. Buffer를 다 읽음.
     * 2. Data가 시작하지 않은 상태에서 개행 문자를 만났을 경우.
     */ 
    while( STL_TRUE )
    {
        if( aParseInfo->mReadOffset == aParseInfo->mReadSize )
        {
            aParseInfo->mIdxColumn = 0;
            break;
        }

        sCurrChar = aReadBuffer[ aParseInfo->mReadOffset ];
        aParseInfo->mReadOffset++;

        if( aParseInfo->mReadOffset == aParseInfo->mReadSize )
        {
            sNextChar = ZTD_CONSTANT_NULL;
        }
        else
        {
            sNextChar = aReadBuffer[ aParseInfo->mReadOffset ];
        }

        ZTD_CONSUMER_SET_BAD_BUFFER( sCurrChar,
                                     aBadRecord,
                                     aErrorStack );

        if( sStartedData == STL_FALSE )
        {
            if( sCurrChar == sOpenQual )
            {
                sStartedData = STL_TRUE;
                sUsedQual    = STL_TRUE;
            }
            else if( (sCurrChar == ZTD_CONSTANT_LINE_FEED) ||
                     (sCurrChar == ZTD_CONSTANT_CARRIAGE_RETURN) )
            {
                if( sCurrChar == ZTD_CONSTANT_CARRIAGE_RETURN )
                {
                    sNextChar = aReadBuffer[ aParseInfo->mReadOffset ];
                    aParseInfo->mReadOffset++;
                    
                    ZTD_CONSUMER_SET_BAD_BUFFER( sNextChar,
                                                 aBadRecord,
                                                 aErrorStack );

                    if( sNextChar != ZTD_CONSTANT_LINE_FEED )
                    {
                        continue;
                    }
                }

                aParseInfo->mState = ZTD_STATE_ROW_COMPLETE;
                break;
            }
            else
            {
                sStartedData = STL_TRUE;
                sUsedQual    = STL_FALSE;
            }
        }
        else
        {
            if( sUsedQual == STL_FALSE )
            {
                if( (sCurrChar == ZTD_CONSTANT_LINE_FEED) ||
                    (sCurrChar == ZTD_CONSTANT_CARRIAGE_RETURN) )
                {
                    if( sCurrChar == ZTD_CONSTANT_CARRIAGE_RETURN )
                    {
                        sNextChar = aReadBuffer[ aParseInfo->mReadOffset ];
                        aParseInfo->mReadOffset++;
                    
                        ZTD_CONSUMER_SET_BAD_BUFFER( sNextChar,
                                                     aBadRecord,
                                                     aErrorStack );

                        if( sNextChar != ZTD_CONSTANT_LINE_FEED )
                        {
                            continue;
                        }
                    }
                    
                    aParseInfo->mState = ZTD_STATE_ROW_COMPLETE;
                    break;
                }
                else if( sCurrChar == sDelimiter[0] )
                {
                    ZTD_CONSUMER_CHECK_DELIMITER( aReadBuffer,
                                                  aParseInfo,
                                                  sDelimiter,
                                                  sDelimiterLen,
                                                  aBadRecord,
                                                  sIsDelimiter,
                                                  aErrorStack );

                    if( sIsDelimiter == STL_FALSE )
                    {
                        continue;
                    }

                    sStartedData = STL_FALSE;
                }
                else
                {
                    /* nothing to do */
                }
            }
            else
            {
                if( sCurrChar == sCloseQual )
                {
                    do
                    {
                        if( (sNextChar == ZTD_CONSTANT_TAB) ||
                            (sNextChar == ZTD_CONSTANT_SPACE ) )
                        {
                            sWhiteSpaceOn = STL_TRUE;

                            ZTD_CONSUMER_SET_BAD_BUFFER( sNextChar,
                                                         aBadRecord,
                                                         aErrorStack );
                        
                            aParseInfo->mReadOffset++;
                            sNextChar = aReadBuffer[ aParseInfo->mReadOffset ];
                        }

                        /**
                         * Data( aReadBuffer ) 끝 확인: 함수 종료.
                         */ 
                        if( aParseInfo->mReadOffset == aParseInfo->mReadSize )
                        {
                            ZTD_CONSUMER_SET_BAD_BUFFER( sNextChar,
                                                         aBadRecord,
                                                         aErrorStack );
                            STL_THROW( EXIT_SUCCESS_FUNCTION );
                        }
                    } while( (sNextChar == ZTD_CONSTANT_TAB) ||
                             (sNextChar == ZTD_CONSTANT_SPACE ) );

                    ZTD_CONSUMER_SET_BAD_BUFFER( sNextChar,
                                                 aBadRecord,
                                                 aErrorStack );
                
                    aParseInfo->mReadOffset++;
                
                    if( ( sNextChar == ZTD_CONSTANT_LINE_FEED ) ||
                        ( sNextChar == ZTD_CONSTANT_CARRIAGE_RETURN ) )
                    {
                        if( sNextChar == ZTD_CONSTANT_CARRIAGE_RETURN )
                        {
                            sNextChar = aReadBuffer[ aParseInfo->mReadOffset ];
                            aParseInfo->mReadOffset++;
                            
                            ZTD_CONSUMER_SET_BAD_BUFFER( sNextChar,
                                                         aBadRecord,
                                                         aErrorStack );

                            if( sNextChar != ZTD_CONSTANT_LINE_FEED )
                            {
                                continue;
                            }
                        }
                        
                        aParseInfo->mState = ZTD_STATE_ROW_COMPLETE;
                        break;
                    }
                    else if( sNextChar == sDelimiter[0] )
                    {
                        if( sDelimiterLen > 1 )
                        {
                            ZTD_CONSUMER_CHECK_DELIMITER( aReadBuffer,
                                                          aParseInfo,
                                                          sDelimiter,
                                                          sDelimiterLen,
                                                          aBadRecord,
                                                          sIsDelimiter,
                                                          aErrorStack );

                            if( sIsDelimiter == STL_FALSE )
                            {
                                aParseInfo->mState = ZTD_STATE_BAD;
                                break;
                            }
                        }

                        
                        sStartedData = STL_FALSE;
                        sUsedQual    = STL_FALSE;
                    }
                    else if( sNextChar == sCloseQual )
                    {
                        if( sWhiteSpaceOn == STL_TRUE )
                        {
                            aParseInfo->mState = ZTD_STATE_BAD;
                            break;
                        }
                        else
                        {
                            /* nothing to do */
                        }
                    }
                    else
                    {
                        aParseInfo->mState = ZTD_STATE_BAD;
                        break;
                    }
                }
                else
                {
                    /* nothing to do */
                }
            }
        }
    }

    STL_RAMP( EXIT_SUCCESS_FUNCTION );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
                                
