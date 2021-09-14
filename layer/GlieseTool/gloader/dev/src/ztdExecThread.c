/*******************************************************************************
 * ztdExecThread.c
 *
 * Copyright (c) 2012, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztdExecThread.c 
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stl.h>
#include <dtl.h>
#include <goldilocks.h>
#include <goldilocksImpExp.h>
#include <ztdDef.h>
#include <ztdDisplay.h>
#include <ztdExecute.h>
#include <ztdExecThread.h>
#include <ztdFileExecute.h>
#include <ztdConsumerThread.h>
#include <ztdBufferedFile.h>

extern dtlCharacterSet gZtdCharacterSet;
extern stlBool         gZtdRunState;
extern stlUInt32       gZtdErrorCount;
extern stlUInt32       gZtdMaxErrorCount;
extern stlBool         gZtdBindDATEAsTIMESTAMP;

extern ztdManager      gZtdManager;

ztdQueueInfo         * gZtdQueue;
volatile stlUInt32     gZtdTextModeConsumerRunningCount; /* Text Import에서의 동작중인 consumer thread 개수 */

/**
 * @brief gloader의 upload를  Multi-thread로 수행하는 Dispatcher Thread Func.
 * @param [in]     aEnvHandle        SQLHENV
 * @param [in]     aDbcHandle        SQLHDBC
 * @param [in]     aAllocator        Region Memory Allocator
 * @param [in]     aControlInfo      Control File의 내용을 저장한 struct
 * @param [in]     aFileAndBuffer    File Descriptor와 log를 담는 struct
 * @param [in]     aInputArguments   Commnad Argument를 저장한 struct
 * @param [in/out] aErrorStack       error stack
 */ 
stlStatus ztdImportTextData( SQLHENV             aEnvHandle,
                             SQLHDBC             aDbcHandle,
                             stlAllocator      * aAllocator,
                             ztdControlInfo    * aControlInfo,
                             ztdFileAndBuffer  * aFileAndBuffer,
                             ztdInputArguments * aInputArguments,
                             stlErrorStack     * aErrorStack )
{
    SQLHSTMT              sStmt                                       = NULL;
    SQLHDESC              sDesc                                       = NULL;
    ztdColumnInfo      *  sColumnInfo                                 = NULL;
    ztdParseInfo          sParseInfo;

    stlStatus          *  sConsReturn                                 = NULL;
    stlChar               sSqlCommand[ZTD_MAX_COMMAND_BUFFER_SIZE];
    stlChar            *  sAlignedBuffer                              = NULL;
    
    stlInt64              sThreadUnit;
    stlInt64              sChunkSize                                  = ZTD_CHUNK_SIZE;
    stlInt64           *  sDataSize;    
    stlInt64              sTargetQueueNo                              = 0;
    stlInt32              sIndex;
    stlInt32              sState                                      = 0;

    stlThread          *  sConsThread                                 = NULL;
    ztdThreadArg       *  sConsThreadArg                              = NULL;
    ztdChunkItem          sChunkItem;

    stlThread             sReaderThread;
    ztdReadThreadArg      sReaderArg;
    ztdDataManager        sDataManager;
    stlStatus             sReaderReturn;

    stlErrorStack      *  sJoinErrorStack                             = NULL;

    stlBool               sIsAllocStmtHandle                          = STL_FALSE;

    char                  sLocalTypeName[128];
    
    sThreadUnit                                                       = aInputArguments->mThreadUnit;

    stlMemset(&sDataManager, 0x00, STL_SIZEOF(ztdDataManager));

    ZTD_INIT_PARSE_INFO( sParseInfo );
    ZTD_INIT_CHUNK_ITEM( sChunkItem );
    
    gZtdManager.mCntRecord     = 0;
    gZtdManager.mCntBad        = 0;
    
    gZtdTextModeConsumerRunningCount           = sThreadUnit;

    /**
     * phase 1.
     *  Table Information 얻어 Record Size 계산.
     *  Consumer Thread에 보낼 Column 정보 세팅.
     */

    /* Relation 정보 얻기 */
    STL_TRY( SQLAllocHandle( SQL_HANDLE_STMT,
                             aDbcHandle,
                             &sStmt )
             == SQL_SUCCESS );
    sIsAllocStmtHandle = STL_TRUE;

    if( aControlInfo->mSchema[0] == 0x00 )
    {
        stlSnprintf( sSqlCommand,
                     ZTD_MAX_COMMAND_BUFFER_SIZE,
                     "SELECT * FROM %s LIMIT 1",
                     aControlInfo->mTable );
    }
    else
    {
        stlSnprintf( sSqlCommand,
                     ZTD_MAX_COMMAND_BUFFER_SIZE,
                     "SELECT * FROM %s.%s LIMIT 1",
                     aControlInfo->mSchema,
                     aControlInfo->mTable );
    }

    STL_TRY( SQLPrepare( sStmt,
                         (SQLCHAR *) sSqlCommand,
                         SQL_NTS )
             == SQL_SUCCESS );
    
    STL_TRY( SQLNumResultCols( sStmt, &sParseInfo.mColumnCount )
             == SQL_SUCCESS );

    if( aControlInfo->mSchema[0] == 0x00 )
    {
        STL_TRY( SQLGetStmtAttr( sStmt,
                                 SQL_ATTR_IMP_ROW_DESC,
                                 &sDesc,
                                 SQL_IS_POINTER,
                                 NULL )
                 == SQL_SUCCESS );

        STL_TRY( SQLGetDescField( sDesc,
                                  1,
                                  SQL_DESC_SCHEMA_NAME,
                                  aControlInfo->mSchema,
                                  ZTD_MAX_SCHEMA_NAME,
                                  NULL )
                 == SQL_SUCCESS );
    }
    else
    {
        /* Do Notting */
    }

    /* Consumer Thread에 필요한 Column Size를 저장할 Buffer 할당. */
    STL_TRY( stlAllocRegionMem( aAllocator,
                                STL_SIZEOF(stlInt64) * sParseInfo.mColumnCount,
                                (void **) &sDataSize,
                                aErrorStack )
             == STL_SUCCESS );

    /* ztdColumnInfo Column 개수만큼 할당. */
    STL_TRY( stlAllocRegionMem( aAllocator,
                                STL_SIZEOF( ztdColumnInfo ) * sParseInfo.mColumnCount,
                                (void **) &sColumnInfo,
                                aErrorStack )
             == STL_SUCCESS );
    
    for( sIndex = 0; sIndex < sParseInfo.mColumnCount; sIndex++ )
    {
        STL_TRY( SQLDescribeCol( sStmt,
                                 (SQLUSMALLINT)sIndex + 1,
                                 (SQLCHAR *) sColumnInfo[sIndex].mName,
                                 (SQLSMALLINT)ZTD_MAX_COLUMN_NAME,
                                 &sColumnInfo[sIndex].mNameLength,
                                 &sColumnInfo[sIndex].mSQLType,
                                 &sColumnInfo[sIndex].mDataSize,
                                 &sColumnInfo[sIndex].mDecimalDigits,
                                 &sColumnInfo[sIndex].mNullable )
                 == SQL_SUCCESS );

        STL_TRY( SQLColAttribute( sStmt,
                                  (SQLUSMALLINT)sIndex + 1,
                                  SQL_DESC_LOCAL_TYPE_NAME,
                                  sLocalTypeName,
                                  STL_SIZEOF(sLocalTypeName),
                                  NULL,
                                  NULL ) == SQL_SUCCESS );

        /*
         * DB의 DATE 컬럼은 SQLDescribeCol() 시 data loss를 피하기 위해 SQL_TYPE_TIMESTAMP 을 반환한다.
         * gloader 에서는 DATE_FORMAT 을 적용하기 위해 SQL_TYPE_DATE로 변환한다.
         */
        if( (stlStrcasecmp( sLocalTypeName, "DATE" ) == 0) &&
            (gZtdBindDATEAsTIMESTAMP == STL_FALSE) )
        {
            sColumnInfo[sIndex].mSQLType = SQL_TYPE_DATE;
        }

        switch( sColumnInfo[sIndex].mSQLType )
        {
            case SQL_NUMERIC :
                sDataSize[sIndex] = sColumnInfo[sIndex].mDataSize + 3;
                break;
            case SQL_FLOAT :
                sDataSize[sIndex] = ZTD_FLOAT_SIGNIFICANT_FIGURE;
                break;
            case SQL_SMALLINT :
                sDataSize[sIndex] = ZTD_SMALLINT_SIGNIFICANT_FIGURE;
                break;
            case SQL_INTEGER :
                sDataSize[sIndex] = ZTD_INTEGER_SIGNIFICANT_FIGURE;
                break;
            case SQL_REAL :
                sDataSize[sIndex] = ZTD_REAL_SIGNIFICANT_FIGURE;
                break;
            case SQL_DOUBLE :
                sDataSize[sIndex] = ZTD_DOUBLE_SIGNIFICANT_FIGURE;
                break;
            case SQL_BIGINT :
                sDataSize[sIndex] = ZTD_BIGINT_SIGNIFICANT_FIGURE;
                break;
            case SQL_CHAR :
                sColumnInfo[sIndex].mSQLType = SQL_VARCHAR;
            case SQL_VARCHAR :
                sDataSize[sIndex] = sColumnInfo[sIndex].mDataSize * dtlGetMbMaxLength(gZtdCharacterSet) + 1;
                break;
            case SQL_LONGVARCHAR :
            case SQL_LONGVARBINARY :
                sDataSize[sIndex] = ZTD_LONG_DATA_INIT_SIZE + 1;
                break;
            default:
                sDataSize[sIndex] = sColumnInfo[sIndex].mDataSize;
                break;
        }
    }

    /**
     * 사용한 stmt handle을 Free한다.
     */
    sIsAllocStmtHandle = STL_FALSE;
    STL_TRY( SQLFreeHandle( SQL_HANDLE_STMT, sStmt ) == SQL_SUCCESS );

    /**
     * phase 2.
     *  Data File을 Read File 할 Reader Thread 생성과 변수 초기화.
     */

    STL_TRY( ztdInitializeDataManager( aAllocator,
                                       &sDataManager,
                                       aErrorStack ) == STL_SUCCESS );
    sState = 1;
    sReaderArg.mDataFile    = aFileAndBuffer;
    sReaderArg.mDataManager = &sDataManager;
    
    STL_INIT_ERROR_STACK( &sReaderArg.mErrorStack );
    
    STL_TRY( stlCreateThread( &sReaderThread,
                              NULL,
                              ztdReadDataFileByThread,
                              (void *) &sReaderArg,
                              aErrorStack )
             == STL_SUCCESS );
    sState = 2;
    
    /**
     * phase 3.
     * Semaphore와 Thread 생성.
     * Thread Argument    세팅.
     */
    /* Thread 할당 */
    STL_TRY( stlAllocRegionMem( aAllocator,
                                STL_SIZEOF(stlThread) * sThreadUnit,
                                (void **) &sConsThread,
                                aErrorStack  )
             == STL_SUCCESS );

    /* Thread의 Argument 할당. */
    STL_TRY( stlAllocRegionMem( aAllocator,
                                STL_SIZEOF(ztdThreadArg) * sThreadUnit,
                                (void **) &sConsThreadArg,
                                aErrorStack  )
             == STL_SUCCESS );
    
    /* Thread의 종료 Return Value 할당. */
    STL_TRY( stlAllocRegionMem( aAllocator,
                                STL_SIZEOF(stlStatus) * sThreadUnit,
                                (void **) &sConsReturn,
                                aErrorStack )
             == STL_SUCCESS );

    STL_TRY( stlAllocRegionMem( aAllocator,
                                STL_SIZEOF(ztdQueueInfo) * sThreadUnit,
                                (void **) &gZtdQueue,
                                aErrorStack )
             == STL_SUCCESS );    

    /* Producer에서 사용할 ChunkItem Buffer를 AllocHeap으로 할당. */
    STL_TRY( stlAllocHeap( (void **) &sChunkItem.mBuffer,
                           sChunkSize,
                           aErrorStack )
             == STL_SUCCESS );
    sState = 3;

    sChunkItem.mBuffer[0] = ZTD_CONSTANT_NULL;
    sChunkItem.mSize      = sChunkSize;

    /* Create Thread 전 Argument 세팅과 Create Thread */
    for( sIndex = 0; sIndex < sThreadUnit; sIndex++ )
    {
        sConsThreadArg[sIndex].mEnv            = aEnvHandle;
        sConsThreadArg[sIndex].mColumnCount    = sParseInfo.mColumnCount;
        sConsThreadArg[sIndex].mDataSize       = sDataSize;
        sConsThreadArg[sIndex].mControlInfo    = aControlInfo;
        sConsThreadArg[sIndex].mColumnInfo     = sColumnInfo;
        sConsThreadArg[sIndex].mFileAndBuffer  = aFileAndBuffer;
        sConsThreadArg[sIndex].mInputArguments = aInputArguments;

        /**
         * Queue 초기화.
         */
        gZtdQueue[sIndex].mFront = NULL;
        gZtdQueue[sIndex].mRear  = NULL;
        gZtdQueue[sIndex].mFreeCnt = 0;
        STL_TRY( ztdConstructQueue( aAllocator,
                                    &gZtdQueue[sIndex],
                                    sChunkSize,
                                    aErrorStack )
                 == STL_SUCCESS );
        
        sConsThreadArg[sIndex].mQueue = &(gZtdQueue[sIndex]);
        
        STL_INIT_ERROR_STACK( &sConsThreadArg[sIndex].mErrorStack );
    }
    sState = 4;
        
    for( sIndex = 0 ; sIndex < sThreadUnit; sIndex++ )
    {
        STL_TRY( stlCreateThread( &sConsThread[sIndex],
                                  NULL,
                                  ztdImportTextDataThread,
                                  (void *) &sConsThreadArg[sIndex],
                                  aErrorStack )
                 == STL_SUCCESS );
    }
    sState = 5;
    
    /**
     * phase 4.
     *  Data를 Parsing, Producer와 Consumers 동기화.
     */
    
    STL_TRY( ztdRequestData( &sAlignedBuffer,
                             &sDataManager,
                             &sParseInfo,
                             aErrorStack ) == STL_SUCCESS );
    
    sIndex = 0;
    while( sParseInfo.mNoDataFlag != STL_TRUE )
    {

        /**
         * STL_SIGNAL_QUIT로 process 종료체크.
         * Consumer Threads 비정상 종료 체크.
         */
        STL_TRY( gZtdRunState == STL_TRUE );

        if( gZtdTextModeConsumerRunningCount == 0 )
        {
            /**
             * Argument Option의 Errors 조건 확인.
             */ 
            if( (gZtdErrorCount > gZtdMaxErrorCount) &&
                (gZtdMaxErrorCount > 0) )
            {
                break;
            }
            else
            {
                STL_TRY( STL_FALSE );
            }
        }
        
        /**
         * Data File을 읽고 Record Chunk를 구성
         */
        STL_TRY( ztdParseDataToChunk( aInputArguments->mArraySize,
                                      &sAlignedBuffer,
                                      &sChunkItem,
                                      aControlInfo,
                                      &sParseInfo,
                                      &sDataManager,
                                      aErrorStack )
                 == STL_SUCCESS );
        
        /**
         * Record Chunk를 Queue에 삽입
         */
        STL_TRY( ztdPassChunkByRoundRobin( &sChunkItem,
                                           sThreadUnit,
                                           &sTargetQueueNo,
                                           gZtdQueue,
                                           aErrorStack )
                 == STL_SUCCESS );

        sChunkItem.mOffset = 0;
    }

    /**
     * phase 4. 작업 종료.
     */
    
    for( sIndex = 0; sIndex < sThreadUnit; sIndex++ )
    {
        STL_TRY( stlReleaseSemaphore( &gZtdQueue[sIndex].mQueueSema, aErrorStack )
                 == STL_SUCCESS );
    }
    
    sState = 4;
    for( sIndex = 0; sIndex < sThreadUnit; sIndex++ )
    {
        sJoinErrorStack = &sConsThreadArg->mErrorStack;
        STL_TRY( stlJoinThread( &sConsThread[sIndex],
                                &sConsReturn[sIndex],
                                aErrorStack )
                 == STL_SUCCESS );

        STL_TRY_THROW( sConsReturn[ sIndex ] == STL_SUCCESS,
                       RAMP_ERR_IMPORT_THREAD );
    }

    if( (gZtdErrorCount > gZtdMaxErrorCount) &&
        (gZtdMaxErrorCount > 0) )
    {
        stlSnprintf( aFileAndBuffer->mLogBuffer,
                     ZTD_MAX_LOG_STRING_SIZE,
                     "INCOMPLETED IN IMPORTING TABLE: %s.%s, SUCCEEDED %lu RECORDS, ERRORED %lu RECORDS ",
                     aControlInfo->mSchema,
                     aControlInfo->mTable,
                     gZtdManager.mCntRecord - gZtdManager.mCntBad,
                     gZtdManager.mCntBad );
    }
    else
    {
        stlSnprintf( aFileAndBuffer->mLogBuffer,
                     ZTD_MAX_LOG_STRING_SIZE,
                     "COMPLETED IN IMPORTING TABLE: %s.%s, TOTAL %lu RECORDS, SUCCEEDED %lu RECORDS ",
                     aControlInfo->mSchema,
                     aControlInfo->mTable,
                     gZtdManager.mCntRecord,
                     gZtdManager.mCntRecord - gZtdManager.mCntBad );
    }
    
    sState = 3;
    for( sIndex = 0; sIndex < sThreadUnit; sIndex++ )
    {
        STL_TRY( ztdDestructQueue( &gZtdQueue[sIndex],
                                   aErrorStack ) == STL_SUCCESS );
    }
    
    sState = 2;
    stlFreeHeap( sChunkItem.mBuffer );

    sState = 1;
    sJoinErrorStack = &sReaderArg.mErrorStack;
    STL_TRY( stlJoinThread( &sReaderThread,
                            &sReaderReturn,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sReaderReturn == STL_SUCCESS,
                   RAMP_ERR_READ_THREAD );
    
    sState = 0;
    STL_TRY( ztdTerminateDataManager( &sDataManager,
                                      aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_IMPORT_THREAD )
    {
        stlAppendErrors( aErrorStack, sJoinErrorStack );

        /**
         * Join하지 않은 ztdImportByThread 쓰레드를 Join한다.
         */
        for( sIndex = sIndex + 1; sIndex < sThreadUnit; sIndex++ )
        {
            (void) stlJoinThread( &sConsThread[sIndex],
                                  &sConsReturn[sIndex],
                                  aErrorStack );

            if( sConsReturn[ sIndex ] == STL_FAILURE )
            {
                stlAppendErrors( aErrorStack, &sConsThreadArg[ sIndex ].mErrorStack );
            }
        }
    }

    STL_CATCH( RAMP_ERR_READ_THREAD )
    {
        stlAppendErrors( aErrorStack, sJoinErrorStack );
    }
    
    /**
     * 1. Process 종료를 위해 STL_SIG_QUIT signal이 온다면 STL_FINISH로 온다.
     * 2. Consumer Thread의 DBConnection을 끊기 위해 Consumer Thread를 정상 종료 시킴.
     * 3. Consumer Thread의 Dead Lock을 피하기 위해 Queue에 Semaphore를 Release 해준다.
     */
    STL_FINISH;

    /**
     * 비정상 종료됨을 설정한다.
     */
    gZtdRunState = STL_FALSE;

    if( sIsAllocStmtHandle == STL_TRUE )
    {
        (void) ztdCheckError( aEnvHandle,
                              aDbcHandle,
                              sStmt,
                              0,
                              aFileAndBuffer,
                              NULL,
                              aErrorStack );
        
        (void) SQLFreeHandle( SQL_HANDLE_STMT, sStmt );
    }
    
    switch( sState )
    {
        case 5:
            for( sIndex = 0; sIndex < sThreadUnit; sIndex++ )
            {
                (void) stlReleaseSemaphore( &gZtdQueue[sIndex].mQueueSema, aErrorStack );
            }
            
            for( sIndex = 0; sIndex < sThreadUnit; sIndex++ )
            {
                stlJoinThread( &sConsThread[sIndex],
                               &sConsReturn[sIndex],
                               aErrorStack );

                if( sConsReturn[ sIndex ] == STL_FAILURE )
                {
                    stlAppendErrors( aErrorStack, &sConsThreadArg[ sIndex ].mErrorStack );
                }
            }
        case 4:
            for( sIndex = 0; sIndex < sThreadUnit; sIndex++ )
            {
                ztdDestructQueue( &gZtdQueue[sIndex],
                                  aErrorStack );
            }
        case 3:
            (void) stlFreeHeap( sChunkItem.mBuffer );
        case 2:
            if( gZtdRunState == STL_FALSE )
            {
                for( sIndex = 0; sIndex < ZTD_BUFFER_COUNT; sIndex++ )
                {
                    stlReleaseSemaphore( &sDataManager.mSema[sIndex],
                                         aErrorStack );
                }
            }
            (void) stlJoinThread( &sReaderThread,
                                  &sReaderReturn,
                                  aErrorStack );
            
            if( sReaderReturn == STL_FAILURE )
            {
                stlAppendErrors( aErrorStack, &sReaderArg.mErrorStack );
            }
        case 1:
            (void) ztdTerminateDataManager( &sDataManager,
                                            aErrorStack );
            break;
    }
    
    return STL_FAILURE;
}

/**
 *@brief 하나의 Queue를 초기화 하는 Func.
 *@param [in]     aAllocator    Region Memory Allocator.
 *@param [out]    aQueue        Queue structure.
 *@param [in]     aChunkSize    Chunk Total Size.
 *@param [in/out] aErrorStack   Error Stack.
 */ 
stlStatus ztdConstructQueue( stlAllocator  * aAllocator,
                             ztdQueueInfo  * aQueue,
                             stlInt64        aChunkSize,
                             stlErrorStack * aErrorStack )
{
    ztdQueueNode * sNewNode;
    stlInt32       sIdx;
    stlInt32       sState = 0;
    
    for( sIdx = 0; sIdx < ZTD_QUEUE_NODE_FACTOR; sIdx++ )
    {
        STL_TRY( stlAllocRegionMem( aAllocator,
                                    STL_SIZEOF( ztdQueueNode ),
                                    (void **) &sNewNode,
                                    aErrorStack )
                 == STL_SUCCESS );

        sNewNode->mItem.mSize     = aChunkSize;        
        sNewNode->mItem.mOffset   = 0;
        sNewNode->mItem.mStartRecNum = 0;
        
        STL_TRY( stlAllocHeap( (void **) &(sNewNode->mItem.mBuffer),
                               sNewNode->mItem.mSize,
                               aErrorStack )
                 == STL_SUCCESS );

        sNewNode->mItem.mBuffer[0] = '\0';
        
        if( aQueue->mFront == NULL )
        {
            aQueue->mFront = sNewNode;            
        }
        else
        {
            aQueue->mRear->mLink = sNewNode;
        }
        aQueue->mRear = sNewNode;
        aQueue->mRear->mLink = aQueue->mFront;
        aQueue->mFreeCnt++;        
    }
    sState = 1;

    aQueue->mRear = aQueue->mFront;
    
    STL_TRY( stlCreateSemaphore( &aQueue->mQueueSema,
                                 "Queue",
                                 0,
                                 aErrorStack )
             == STL_SUCCESS );
    sState = 2;
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            for( sIdx = 0; sIdx < ZTD_QUEUE_NODE_FACTOR; sIdx++ )
            {
                if( aQueue->mFront == NULL )
                {
                    break;
                }
                
                if( aQueue->mFront->mItem.mBuffer != NULL )
                {
                    stlFreeHeap( aQueue->mFront->mItem.mBuffer );
                }
                
                aQueue->mFront = aQueue->mFront->mLink;
            }
        default:
            break;
    }
    return STL_FAILURE;
}

/**
 *@brief 하나의 Queue를 해제하는 Func.
 *@param [in]     aQueue        Queue structure.
 *@param [in/out] aErrorStack   Error stack.
 */
stlStatus ztdDestructQueue( ztdQueueInfo  * aQueue,
                            stlErrorStack * aErrorStack )
{
    stlInt32 sIdx;
    stlInt32 sState = 1;
    
    STL_TRY( stlDestroySemaphore( &aQueue->mQueueSema,
                                  aErrorStack ) == STL_SUCCESS );

    sState = 0;
    for( sIdx = 0; sIdx < ZTD_QUEUE_NODE_FACTOR; sIdx++ )
    {
        STL_DASSERT( aQueue->mFront != NULL );
        STL_DASSERT( aQueue->mFront->mItem.mBuffer != NULL );
            
        stlFreeHeap( aQueue->mFront->mItem.mBuffer );

        aQueue->mFront = aQueue->mFront->mLink;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            for( sIdx = 0; sIdx < ZTD_QUEUE_NODE_FACTOR; sIdx++ )
            {
                STL_DASSERT( aQueue->mFront != NULL );
                STL_DASSERT( aQueue->mFront->mItem.mBuffer == NULL );
            
                stlFreeHeap( aQueue->mFront->mItem.mBuffer );

                aQueue->mFront = aQueue->mFront->mLink;
            }            
        default:
            break;
    }
    
    return STL_FAILURE;
}

/**
 *@brief Round robin으로 Record Array를 Enqueue 할 Queue를 선택.
 *@param [in]     aChunkItem     Array 단위로 Parsing된 Records.
 *@param [in]     aThreadCnt     Consumer(insert 수행) 하는 Thread 수.
 *@param [in]     aQueueNo       Corrunt Queue Number.
 *@param [in]     aQueue         Queue structure.
 *@param [in/out] aErrorStack    Error stack.
 */ 
stlStatus ztdPassChunkByRoundRobin( ztdChunkItem  * aChunkItem,
                                    stlInt64        aThreadCnt,
                                    stlInt64      * aQueueNo,
                                    ztdQueueInfo  * aQueue,
                                    stlErrorStack * aErrorStack )
{
    stlUInt32 sRetVal;
    stlInt32  sCount = 0;
    stlInt64  sQueueIdx = *aQueueNo;
    stlBool   sSucceeded = STL_FALSE;
    
    do
    {
        sRetVal = stlAtomicCas32( &aQueue[sQueueIdx].mFreeCnt,
                                  ZTD_QUEUE_STATE_EMPTY,
                                  ZTD_QUEUE_STATE_EMPTY );
        
        if( sRetVal != ZTD_QUEUE_STATE_EMPTY )
        {              
            STL_TRY( ztdSetChunkToQueue( aChunkItem,
                                         &aQueue[sQueueIdx],
                                         aErrorStack ) == STL_SUCCESS );
            sSucceeded = STL_TRUE;
        }
        
        sQueueIdx = (sQueueIdx + 1) % aThreadCnt;
        sCount++;

        if( sCount == aThreadCnt )
        {
            stlSleep( 100 );
            sCount = 0;
        }
        STL_TRY_THROW( gZtdRunState == STL_TRUE, EXIT_FINISH_FUNC );
        STL_TRY_THROW( gZtdTextModeConsumerRunningCount != 0, EXIT_FINISH_FUNC );
        /**
         * sSucceeded: Flag의 값이 False라면 While loop를 돈다.
         */ 
    } while( sSucceeded == STL_FALSE );

    *aQueueNo = sQueueIdx;

    STL_RAMP( EXIT_FINISH_FUNC );

    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 *@brief Queue의 Free Node 확인 후 Enqueue.
 *@param [in]     aChunkItem    Array 단위로 Parsing된 Records.
 *@param [in]     aQueue        Queue structure.
 *@param [in/out] aErrorStack   Error stack.
 **/
stlStatus ztdSetChunkToQueue( ztdChunkItem  * aChunkItem,
                              ztdQueueInfo  * aQueue,
                              stlErrorStack * aErrorStack )
{
    STL_TRY( ztdEnqueue( &aQueue->mRear,
                         aChunkItem,
                         aErrorStack ) == STL_SUCCESS );

    stlAtomicDec32( &aQueue->mFreeCnt );
        
    STL_TRY( stlReleaseSemaphore( &aQueue->mQueueSema, aErrorStack ) == STL_SUCCESS );
   
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 *@brief Queue에서 Record Array  Dequeue.
 *@param [in]     aChunkItem    Array 단위로 Parsing된 Records.
 *@param [in]     aQueue        Queue structure.
 *@param [in/out] aErrorStack   Error stack.
 **/
stlStatus ztdGetChunkFromQueue( ztdChunkItem  * aChunkItem,
                                ztdQueueInfo  * aQueue,
                                stlErrorStack * aErrorStack )
{
    stlInt32 sState = 0;    
    
    STL_TRY( stlAcquireSemaphore( &aQueue->mQueueSema, aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztdDequeue( &aQueue->mFront,
                         aChunkItem,
                         aErrorStack ) == STL_SUCCESS );

    stlAtomicInc32( &aQueue->mFreeCnt );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void) stlReleaseSemaphore( &aQueue->mQueueSema, aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}

/**
 * @brief 1개의 Chunk를 Queue에서 Copy하여 얻는다.
 * @param [in/out] aFront       Queue Get Position.
 * @param [in]     aItem        Record Chunk.
 * @param [in/out] aErrorStack  error stack.
 */
stlStatus ztdDequeue( ztdQueueNode  ** aFront,
                      ztdChunkItem  *  aItem,
                      stlErrorStack *  aErrorStack )
{
    while( (*aFront)->mItem.mSize > aItem->mSize )
    {
        STL_TRY( ztdExpandHeap( &(aItem->mBuffer),
                                (aItem->mSize),
                                &(aItem->mSize),
                                aErrorStack )
                 == STL_SUCCESS );
    }

    stlMemcpy( aItem->mBuffer,
               (*aFront)->mItem.mBuffer,
               (*aFront)->mItem.mOffset );
    aItem->mBuffer[(*aFront)->mItem.mOffset] = '\0';
    
    aItem->mSize     = (*aFront)->mItem.mSize;
    aItem->mOffset   = (*aFront)->mItem.mOffset;
    aItem->mStartRecNum = (*aFront)->mItem.mStartRecNum;

    (*aFront)->mItem.mBuffer[0] = '\0';

    (*aFront)->mItem.mStartRecNum = 0;
    (*aFront)->mItem.mOffset   = 0;

    (*aFront) = (*aFront)->mLink;

    return STL_SUCCESS;

    STL_FINISH;
    /* error 발생하였다면 Queue Node를 초기화 */
    (*aFront)->mItem.mBuffer[0] = '\0';
    (*aFront)->mItem.mStartRecNum  = 0;
    (*aFront)->mItem.mOffset    = 0;
    
    return STL_FAILURE;
}

/**
 * @brief 1개의 Chunk를 Queue에 Copy하여 넣는다.
 * @param [in/out] aRear        Queue Set Position.
 * @param [in]     aItem        Record Chunk.
 * @param [in/out] aErrorStack  error stack.
 */
stlStatus ztdEnqueue( ztdQueueNode  ** aRear,
                      ztdChunkItem  *  aItem,
                      stlErrorStack *  aErrorStack )
{
    while( aItem->mSize > (*aRear)->mItem.mSize )
    {
        STL_TRY( ztdExpandHeap( &((*aRear)->mItem.mBuffer),
                                (*aRear)->mItem.mSize,
                                &((*aRear)->mItem.mSize),
                                aErrorStack )
                 == STL_SUCCESS );
    }
        
    stlMemcpy( (*aRear)->mItem.mBuffer,
               aItem->mBuffer,
               aItem->mOffset );
    (*aRear)->mItem.mBuffer[aItem->mOffset] = '\0';
    
    (*aRear)->mItem.mSize     = aItem->mSize;
    (*aRear)->mItem.mOffset   = aItem->mOffset;
    (*aRear)->mItem.mStartRecNum = aItem->mStartRecNum;

    aItem->mBuffer[0] = '\0';

    (*aRear) = (*aRear)->mLink;
    return STL_SUCCESS;

    STL_FINISH;
    /* error 발생하였다면 Queue Node를 초기화 */
    (*aRear)->mItem.mBuffer[aItem->mOffset] = '\0';
    (*aRear)->mItem.mOffset                 = 0;
    (*aRear)->mItem.mStartRecNum               = 0;
    
    return STL_FAILURE;
}

/**
 * @brief Data File에서 읽어 온 Data(aReadBuffer)를 Array 단위로 분할.
 * @param [in]       aArraySize        Record Array Size.
 * @param [in]       aReadBuffer       Data File에서 얻은 Data Buffer.
 * @param [in]       aChunkItem        Parsing된 Record Array가 저장되는 Buffer.
 * @param [in]       aControlInfo      Control File의 내용
 * @param [in/out]   aParseInfo        Parsing 중 상태 정보.
 * @param [in/out]   aNeededData       Parsing 되는 Data의 필요 판단 Flag.
 * @param [in/out]   aErrorStack       error stack
 */ 
stlStatus ztdParseDataToChunk( stlInt64            aArraySize,
                               stlChar          ** aReadBuffer,
                               ztdChunkItem      * aChunkItem,
                               ztdControlInfo    * aControlInfo,
                               ztdParseInfo      * aParseInfo,
                               ztdDataManager    * aDataManager,
                               stlErrorStack     * aErrorStack )
{
    aChunkItem->mStartRecNum = aParseInfo->mIdxRecord;
    
    while( STL_TRUE )
    {
        STL_TRY( ztdParseDataToRecord( aReadBuffer,
                                       aChunkItem,
                                       aControlInfo,
                                       aParseInfo,
                                       aDataManager,
                                       aErrorStack ) == STL_SUCCESS );
        /**
         * while문 종료:
         *    Parsing할 Data 없음.
         *    Record Parsing을 Array Size만큼 함.
         * 변경:
         *      Array 개수 맞추거나 Chunk의 공간이 없음.
         *      ChunkItem이 1개의 Record로 인해 확장된 경우.
         */
        if( ((aParseInfo->mIdxRecord % aArraySize) == 0) ||
            (aParseInfo->mNoDataFlag == STL_TRUE ) ||
            (aParseInfo->mExitParsing == STL_TRUE ) )
        {
            STL_THROW( EXIT_SUCCESS_FUNCTION );
        }
    }

    STL_RAMP( EXIT_SUCCESS_FUNCTION );

    /**
     * ExitParsing은 Parsing종료되어서 FALSE로 Reset.
     * PrevRecOffset은 ChunkItem에 Parsing이 종료되어서 0으로 Reset.
     * 참고. ParseInfo의 PrevRecOffset은 Datafile을 다시 읽을 때 Reset된다.
     */
    aParseInfo->mExitParsing = STL_FALSE;
    aChunkItem->mPrevRecOffset = ZTD_PARSER_PREV_RECORD_INIT_OFFSET;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Data File에서 읽어 온 Data를 하나의 Record로 구성하도록 한다.
 * @param [in]       aReadBuffer       Data File에서 얻은 Data Buffer.
 * @param [in]       aChunkItem        Parsing된 Record Array가 저장되는 Buffer.
 * @param [in]       aControlInfo      Control File의 내용
 * @param [in/out]   aParseInfo        Parsing 중 상태 정보.
 * @param [in/out]   aNeededData       Parsing 되는 Data의 필요 판단 Flag.
 * @param [in/out]   aErrorStack       error stack
 */ 
stlStatus ztdParseDataToRecord( stlChar          ** aReadBuffer,
                                ztdChunkItem      * aChunkItem,
                                ztdControlInfo    * aControlInfo,
                                ztdParseInfo      * aParseInfo,
                                ztdDataManager    * aDataManager,
                                stlErrorStack     * aErrorStack )
{
    stlChar    sOpenQual;
    stlChar    sCurrChar;
    stlChar  * sRecordBuffer;
    stlBool    sCompletedRecord = STL_FALSE;  /* Parsing 중 Record의 끝 판단 */

    sOpenQual = aControlInfo->mOpQualifier;

    /**
     *  sCompletedRecord가 True인 경우.
     */
    while( (sCompletedRecord == STL_FALSE) &&
           (aParseInfo->mNoDataFlag == STL_FALSE) )
    {
        sRecordBuffer = &aChunkItem->mBuffer[aChunkItem->mOffset];

        /**
         * Column 시작에서 white space 체크.
         */ 
        while( STL_TRUE )
        {
            if( aParseInfo->mReadOffset >= aParseInfo->mReadSize )
            {
                STL_TRY( ztdRequestData( aReadBuffer,
                                         aDataManager,
                                         aParseInfo,
                                         aErrorStack ) == STL_SUCCESS );

                if( aParseInfo->mNoDataFlag == STL_TRUE )
                {
                    aParseInfo->mIdxRecord++;
                    STL_THROW( EXIT_SUCCESS_FUNCTION );
                }
                else
                {
                    aParseInfo->mReadOffset = 0;
                }
            }
            
            sCurrChar = (*aReadBuffer)[ aParseInfo->mReadOffset ];
            if( ( sCurrChar == ZTD_CONSTANT_TAB ) ||
                ( sCurrChar == ZTD_CONSTANT_SPACE ) )
            {
                /**
                 * Data가 저장되는 Buffer Overflow 체크 후에 저장.
                 */ 
                ZTD_PRODUCER_CHECK_ASSIGN_CHAR_TO_BUFFER( aChunkItem,
                                                          aParseInfo,
                                                          aParseInfo->mReadOffset,
                                                          sRecordBuffer,
                                                          sCurrChar,
                                                          sCompletedRecord,
                                                          aErrorStack );
                aParseInfo->mReadOffset++;
            }
            else
            {
                break;
            }
        }
        
        if( (sCurrChar == sOpenQual) )
        {
            ZTD_PRODUCER_CHECK_ASSIGN_CHAR_TO_BUFFER( aChunkItem,
                                                      aParseInfo,
                                                      aParseInfo->mReadOffset,
                                                      sRecordBuffer,
                                                      sCurrChar,
                                                      sCompletedRecord,
                                                      aErrorStack );

            aParseInfo->mReadOffset++;
            
            STL_TRY( ztdParseDataToColumnWithOption( aReadBuffer,
                                                     aChunkItem,
                                                     aControlInfo,
                                                     aParseInfo,
                                                     aDataManager,
                                                     &sCompletedRecord,
                                                     aErrorStack ) == STL_SUCCESS );
        }
        else
        {
            STL_TRY( ztdParseDataToColumnWithoutOption( aReadBuffer,
                                                        aChunkItem,
                                                        aControlInfo,
                                                        aParseInfo,
                                                        aDataManager,
                                                        &sCompletedRecord,
                                                        aErrorStack ) == STL_SUCCESS );
        }
    }

    STL_RAMP( EXIT_SUCCESS_FUNCTION );
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Data File에서 읽어 온 Data를 Parsing 하여 하나의 Column 구성한다.
 * @param [in]       aReadBuffer       Data File에서 얻은 Data Buffer.
 * @param [in]       aChunkItem        Parsing된 Record Array가 저장되는 Buffer.
 * @param [in]       aControlInfo      Control File의 내용
 * @param [in/out]   aParseInfo        Parsing 중 상태 정보.
 * @param [in/out]   aInsistData       Parsing 되는 Data의 필요 판단 Flag.
 * @param [in/out]   aCompleteRecord   Parsing 중 Record 완료 판단 Flag.
 * @param [in/out]   aErrorStack       error stack
 */ 
stlStatus ztdParseDataToColumnWithoutOption( stlChar          ** aReadBuffer,
                                             ztdChunkItem      * aChunkItem,
                                             ztdControlInfo    * aControlInfo,
                                             ztdParseInfo      * aParseInfo,
                                             ztdDataManager    * aDataManager,
                                             stlBool           * aCompleteRecord,
                                             stlErrorStack     * aErrorStack )
{
    stlInt64   sReadOffset    = aParseInfo->mReadOffset;
    stlChar  * sDelimiter     = aControlInfo->mDelimiter;
    stlChar    sCurrChar;
    stlChar  * sRecordBuffer  = &aChunkItem->mBuffer[aChunkItem->mOffset];
    stlInt32   sDelimiterLen;
    stlBool    sIsDelimiter;

    sDelimiterLen = stlStrlen( sDelimiter );
    aParseInfo->mState = ZTD_STATE_INIT;
  
    while( STL_TRUE )
    {
        if( sReadOffset >= aParseInfo->mReadSize )
        {
            /**
             * data 가 없는 경우
             **/
            STL_TRY( ztdRequestData( aReadBuffer,
                                     aDataManager,
                                     aParseInfo,
                                     aErrorStack ) == STL_SUCCESS );

            if( aParseInfo->mNoDataFlag == STL_TRUE )
            {
                /**
                 * 파싱한 유효 DATA가 있었는지 판단.
                 */
                if( sReadOffset != aParseInfo->mReadOffset )
                {
                    aParseInfo->mIdxRecord++;
                }
                break;
            }
            else
            {
                sReadOffset = 0;   
            }
        }
        else
        {
            /* do nothing */
        }
        
        sCurrChar = (*aReadBuffer)[ sReadOffset ];
        sReadOffset++;

        /**
         * Data가 저장되는 Buffer Overflow 체크 후에 저장.
         */ 
        ZTD_PRODUCER_CHECK_ASSIGN_CHAR_TO_BUFFER( aChunkItem,
                                                  aParseInfo,
                                                  sReadOffset,
                                                  sRecordBuffer,
                                                  sCurrChar,
                                                  *aCompleteRecord,
                                                  aErrorStack );

        /**
         * Delimiter와 Line Feed 체크.
         */
        if( sCurrChar == sDelimiter[0] )
        {
            if( sDelimiterLen > 1 )
            {   
                ZTD_PRODUCER_CHECK_DELIMITER( aReadBuffer,
                                              aDataManager,
                                              aParseInfo,
                                              sReadOffset,
                                              aChunkItem,
                                              sRecordBuffer,
                                              sDelimiter,
                                              sDelimiterLen,
                                              *aCompleteRecord,
                                              sIsDelimiter,
                                              aErrorStack );
                if( sIsDelimiter == STL_FALSE )
                {
                    continue;
                }
            }
            
            aParseInfo->mIdxColumn++;
            break;
        }
        else if( sCurrChar == ZTD_CONSTANT_LINE_FEED ) 
            
        {
            aParseInfo->mIdxColumn = 0;
            aParseInfo->mIdxRecord++;
            *aCompleteRecord = STL_TRUE;
            aChunkItem->mPrevRecOffset = aChunkItem->mOffset;
            aParseInfo->mPrevRecOffset = sReadOffset;
            break;
        }
        else
        {
            /* do nothing */
        }
    }

    STL_RAMP( EXIT_SUCCESS_FUNCTION );

    *sRecordBuffer = ZTD_CONSTANT_NULL;
    aParseInfo->mReadOffset = sReadOffset;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Data File에서 읽어 온 Data를 Parsing 하여 하나의 Column 구성한다.
 * @param [in]       aReadBuffer       Data File에서 얻은 Data Buffer.
 * @param [in]       aChunkItem        Parsing된 Record Array가 저장되는 Buffer.
 * @param [in]       aControlInfo      Control File의 내용
 * @param [in/out]   aParseInfo        Parsing 중 상태 정보.
 * @param [in/out]   aInsistData       Parsing 되는 Data의 필요 판단 Flag.
 * @param [in/out]   aCompleteRecord   Parsing 중 Record 완료 판단 Flag.
 * @param [in/out]   aErrorStack       error stack
 *
 * @remark
 *  ConsumerThread에서 Parsing과 차이점:
 *  - Producer Thread는 Column의 개수를 고려하지 않고, 한 Line의 Record를 전부 구할 때 까지 Parsing.
 *  - Consumer Thread는 Column의 개수를 고려하여 Record 생성여부를 판단. Over Column 처리는 Producer Thread 와 같은 방식으로 처리.
 *  한 Line의 Record를 인식하는 기준:
 *  - Qualifier 이후에 개행문자가 오면 Line의 끝이라고 판단하고 Record의 끝이라고 간주한다.
 */ 
stlStatus ztdParseDataToColumnWithOption( stlChar          ** aReadBuffer,
                                          ztdChunkItem      * aChunkItem,
                                          ztdControlInfo    * aControlInfo,
                                          ztdParseInfo      * aParseInfo,
                                          ztdDataManager    * aDataManager,
                                          stlBool           * aCompleteRecord,
                                          stlErrorStack     * aErrorStack )
{
    stlInt64   sReadOffset    = aParseInfo->mReadOffset;
    stlChar  * sDelimiter     = aControlInfo->mDelimiter;
    stlChar    sCloseQual     = aControlInfo->mClQualifier;
    stlChar    sCurrChar;
    stlChar    sNextChar;
    stlChar  * sRecordBuffer  = &aChunkItem->mBuffer[aChunkItem->mOffset];
    stlInt32   sDelimiterLen;
    stlBool    sSpaceFlag     = STL_FALSE;
    stlBool    sIsDelimiter;

    sDelimiterLen = stlStrlen( sDelimiter );

    /**
     * 이 Routine에 왔다면 이미 Open Qualifier를 만났다.
     * 조건문을 줄이기 위해 Data 에서 Close Qualifier만 찾으면 된다. 
     */
    while( STL_TRUE )
    {
        if( sReadOffset >= aParseInfo->mReadSize )
        {
            /**
             * data 가 없는 경우
             **/
            STL_TRY( ztdRequestData( aReadBuffer,
                                     aDataManager,
                                     aParseInfo,
                                     aErrorStack ) == STL_SUCCESS );

            if( aParseInfo->mNoDataFlag == STL_TRUE )
            {
                aParseInfo->mIdxRecord++;
                break;
            }
            else
            {
                sReadOffset = 0;
            }
        }

        sCurrChar = (*aReadBuffer)[ sReadOffset ];
        sReadOffset++;

        /**
         * Data가 저장되는 Buffer Overflow 체크 후에 저장.
         */
        ZTD_PRODUCER_CHECK_ASSIGN_CHAR_TO_BUFFER( aChunkItem,
                                                  aParseInfo,
                                                  sReadOffset,
                                                  sRecordBuffer,
                                                  sCurrChar,
                                                  *aCompleteRecord,
                                                  aErrorStack );
        
        if( sCurrChar == sCloseQual )
        {
            if( sReadOffset >= aParseInfo->mReadSize )
            {
                STL_TRY( ztdRequestData( aReadBuffer,
                                         aDataManager,
                                         aParseInfo,
                                         aErrorStack ) == STL_SUCCESS );

                if( aParseInfo->mNoDataFlag == STL_TRUE )
                {
                    aParseInfo->mIdxRecord++;
                    break;
                }
                else
                {
                    sReadOffset = 0;
                }
            }
            
            sNextChar = (*aReadBuffer)[ sReadOffset ];
            /**
             * Phase 1.
             * Close Qualifier 뒤에 White Space 체크.
             */
            do{
                if( ( sNextChar == ZTD_CONSTANT_TAB ) ||
                    ( sNextChar == ZTD_CONSTANT_SPACE ) ||
                    ( sNextChar == ZTD_CONSTANT_CARRIAGE_RETURN ) )
                {
                    ZTD_PRODUCER_CHECK_ASSIGN_CHAR_TO_BUFFER( aChunkItem,
                                                              aParseInfo,
                                                              sReadOffset,
                                                              sRecordBuffer,
                                                              sNextChar,
                                                              *aCompleteRecord,
                                                              aErrorStack );
                    sReadOffset++;
                    sNextChar = (*aReadBuffer)[ sReadOffset ];

                    sSpaceFlag = STL_TRUE;
                }

                /**
                 * White Space 체크 중 Parsing할 Data가 끝난지 체크.
                 */
                /* carriage return 이랑 다른 white space. */
                if( sReadOffset >= aParseInfo->mReadSize )
                {
                    STL_TRY( ztdRequestData( aReadBuffer,
                                             aDataManager,
                                             aParseInfo,
                                             aErrorStack ) == STL_SUCCESS );

                    if( aParseInfo->mNoDataFlag == STL_TRUE )
                    {
                        aParseInfo->mIdxRecord++;
                        STL_THROW( EXIT_SUCCESS_FUNCTION );
                    }
                    else
                    {
                        sReadOffset = 0;
                        sNextChar   = (*aReadBuffer)[ sReadOffset ];
                    }
                }
                
            } while( ( sNextChar == ZTD_CONSTANT_TAB ) ||
                     ( sNextChar == ZTD_CONSTANT_SPACE ) ||
                     ( sNextChar == ZTD_CONSTANT_CARRIAGE_RETURN ) );

            ZTD_PRODUCER_CHECK_ASSIGN_CHAR_TO_BUFFER( aChunkItem,
                                                      aParseInfo,
                                                      sReadOffset,
                                                      sRecordBuffer,
                                                      sNextChar,
                                                      *aCompleteRecord,
                                                      aErrorStack );
            sReadOffset++;

            /**
             * Phase 2. NextChar 체크.
             */
            if( sNextChar == sDelimiter[0] )
            {
                if( sDelimiterLen > 1 )
                {
                    ZTD_PRODUCER_CHECK_DELIMITER( aReadBuffer,
                                                  aDataManager,
                                                  aParseInfo,
                                                  sReadOffset,
                                                  aChunkItem,
                                                  sRecordBuffer,
                                                  sDelimiter,
                                                  sDelimiterLen,
                                                  *aCompleteRecord,
                                                  sIsDelimiter,
                                                  aErrorStack );

                    if( sIsDelimiter == STL_FALSE )
                    {
                        continue;
                    }
                }
                                    
                aParseInfo->mIdxColumn++;
                break;
            }
            else if( ( sNextChar == ZTD_CONSTANT_LINE_FEED ) )
            {
                aParseInfo->mIdxColumn = 0;
                aParseInfo->mIdxRecord++;
                *aCompleteRecord = STL_TRUE;
                aChunkItem->mPrevRecOffset = aChunkItem->mOffset;
                aParseInfo->mPrevRecOffset = sReadOffset;
                break;
            }
            else if( sNextChar == sCloseQual )
            {
                if( sSpaceFlag == STL_TRUE )
                {
                    /**
                     * Bad Record로 다음 개행문자까지 Copy 한다.
                     */
                    sRecordBuffer--;
                    aChunkItem->mOffset--;

                    ZTD_PRODUCER_BAD_RECORD( aChunkItem,
                                             sRecordBuffer,
                                             sNextChar,
                                             aReadBuffer,
                                             sReadOffset,
                                             aParseInfo,
                                             aDataManager,
                                             *aCompleteRecord,
                                             aErrorStack );

                    aParseInfo->mIdxRecord++;
                    *aCompleteRecord = STL_TRUE;
                    aChunkItem->mPrevRecOffset = aChunkItem->mOffset;
                    aParseInfo->mPrevRecOffset = sReadOffset;
                    break;
                }
                else
                {
                    /* do nothing */
                }
            }
            else
            {
                /**
                 * Bad Record로 다음 개행문자까지 Copy 한다.
                 */
                sRecordBuffer--;
                aChunkItem->mOffset--;

                ZTD_PRODUCER_BAD_RECORD( aChunkItem,
                                         sRecordBuffer,
                                         sNextChar,
                                         aReadBuffer,
                                         sReadOffset,
                                         aParseInfo,
                                         aDataManager,
                                         *aCompleteRecord,
                                         aErrorStack );

                aParseInfo->mIdxRecord++;
                *aCompleteRecord = STL_TRUE;
                aChunkItem->mPrevRecOffset = aChunkItem->mOffset;
                aParseInfo->mPrevRecOffset = sReadOffset;
                break;
            }
        }
        else 
        {
            /**
             * do nothing
             */ 
        }
        
    }
    
    STL_RAMP( EXIT_SUCCESS_FUNCTION );

    *sRecordBuffer = ZTD_CONSTANT_NULL;
    aParseInfo->mReadOffset = sReadOffset;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;    
}

/**
 * @brief Reader Thread를 실행하기 전 변수 초기화.
 * @param [in]  aAllocator       region memory allocator
 * @param [in]  aDataManager     Double buffering 관리하는 struct
 * @param [in]  aErrorStack      error stack
 */ 
stlStatus ztdInitializeDataManager( stlAllocator     * aAllocator,
                                    ztdDataManager   * aDataManager,
                                    stlErrorStack    * aErrorStack )
{
    stlInt32 sIndex;
    stlChar  sSemName[STL_MAX_SEM_NAME + 1];
    
    STL_TRY( stlAllocRegionMem( aAllocator,
                                STL_SIZEOF(stlSize) * ZTD_BUFFER_COUNT,
                                (void **) &aDataManager->mSize,
                                aErrorStack )
             == STL_SUCCESS );

    STL_TRY( stlAllocRegionMem( aAllocator,
                                STL_SIZEOF(stlBool) * ZTD_BUFFER_COUNT,
                                (void **) &aDataManager->mState,
                                aErrorStack )
             == STL_SUCCESS );

    STL_TRY( stlAllocRegionMem( aAllocator,
                                STL_SIZEOF(stlChar *) * ZTD_BUFFER_COUNT,
                                (void **) &aDataManager->mReadBuffer,
                                aErrorStack )
             == STL_SUCCESS );

    STL_TRY( stlAllocRegionMem( aAllocator,
                                STL_SIZEOF(stlChar *) * ZTD_BUFFER_COUNT,
                                (void **) &aDataManager->mAlignedBuffer,
                                aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( stlAllocRegionMem( aAllocator,
                                STL_SIZEOF(stlSemaphore) * ZTD_BUFFER_COUNT,
                                (void **) &aDataManager->mSema,
                                aErrorStack )
             == STL_SUCCESS );
    
    aDataManager->mProdIdx     = 0;
    aDataManager->mReadIdx     = 0;

    for( sIndex = 0; sIndex < ZTD_BUFFER_COUNT; sIndex++ )
    {
        aDataManager->mState[sIndex] = ZTD_BUFFER_STATE_INIT;
        aDataManager->mSize[sIndex]  = 0;

        STL_TRY( stlAllocRegionMem( aAllocator,
                                    ZTD_MAX_READ_RECORD_BUFFER_SIZE + ZTD_ALIGN_512,
                                    (void **) &aDataManager->mReadBuffer[sIndex],
                                    aErrorStack )
                 == STL_SUCCESS );
        
        ZTD_ALIGN_BUFFER( aDataManager->mAlignedBuffer[sIndex],
                          aDataManager->mReadBuffer[sIndex],
                          ZTD_ALIGN_512 );

        stlSnprintf( sSemName, STL_MAX_SEM_NAME + 1, "Buffer%d", sIndex );
        
        STL_TRY( stlCreateSemaphore( &aDataManager->mSema[sIndex],
                                     sSemName,
                                     1,
                                     aErrorStack )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;    
}

/**
 * @brief Data Manager의 Semaphore를 해제하는 Func.
 * @param [in]  aDataManager     Double buffering 관리하는 struct
 * @param [in]  aErrorStack      error stack
 */ 
stlStatus ztdTerminateDataManager( ztdDataManager * aDataManager,
                                   stlErrorStack  * aErrorStack )
{
    stlInt32 sIndex;

    for( sIndex = 0; sIndex < ZTD_BUFFER_COUNT; sIndex++ )
    {
        STL_TRY( stlDestroySemaphore( &aDataManager->mSema[sIndex],
                                      aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;    
}

/**
 * @brief Double Buffering에서 채워진 Buffer를 가져오는 Func.
 * @param [out] aReadBuffer      Double buffering에서 가져올 Buffer의 Pointer
 * @param [in]  aDataManager     Double buffering 관리하는 struct
 * @param [in]  aParseInfo       Buffer의 Data 사이즈와 Data의 종료.
 * @param [in]  aErrorStack      error stack
 */ 
stlStatus ztdRequestData( stlChar        ** aReadBuffer,
                          ztdDataManager  * aDataManager,
                          ztdParseInfo    * aParseInfo,
                          stlErrorStack   * aErrorStack )
                          
{
    stlInt32 sState = 0;
    while( STL_TRUE )
    {
        /**
         * Read Thread가 비정상 종료되어, Buffer에서 가져올 데이터가 없는 경우를 검사한다.
         */
        STL_TRY( gZtdRunState == STL_TRUE );
        
        STL_TRY( stlAcquireSemaphore( &aDataManager->mSema[ aDataManager->mProdIdx],
                                      aErrorStack ) == STL_SUCCESS );
        sState = 1;

        if( aDataManager->mState[aDataManager->mProdIdx] == ZTD_BUFFER_STATE_INIT )
        {
            sState = 0;
            STL_TRY( stlReleaseSemaphore( &aDataManager->mSema[aDataManager->mProdIdx],
                                          aErrorStack ) == STL_SUCCESS );
        }
        else if( aDataManager->mState[aDataManager->mProdIdx] == ZTD_BUFFER_STATE_PARSE )
        {
            aDataManager->mState[aDataManager->mProdIdx] = ZTD_BUFFER_STATE_INIT;
            aDataManager->mSize[aDataManager->mProdIdx] = 0;

            sState = 0;
            STL_TRY( stlReleaseSemaphore( &aDataManager->mSema[aDataManager->mProdIdx],
                                          aErrorStack ) == STL_SUCCESS );
            
            aDataManager->mProdIdx = (aDataManager->mProdIdx + 1) % ZTD_BUFFER_COUNT;
        }
        else if( aDataManager->mState[aDataManager->mProdIdx] == ZTD_BUFFER_STATE_READY )
        {
            aDataManager->mState[aDataManager->mProdIdx] = ZTD_BUFFER_STATE_PARSE;
            *aReadBuffer = aDataManager->mAlignedBuffer[aDataManager->mProdIdx];
            aParseInfo->mReadSize = aDataManager->mSize[aDataManager->mProdIdx];
            aParseInfo->mPrevRecOffset = ZTD_PARSER_PREV_RECORD_INIT_OFFSET;
            break;
        }
        else if( aDataManager->mState[aDataManager->mProdIdx] == ZTD_BUFFER_STATE_FINISH )
        {
            aParseInfo->mReadSize   = 0;
            aParseInfo->mNoDataFlag = STL_TRUE;
            break;
        }
        else
        {
            STL_DASSERT( !(STL_TRUE) );
        }
    }
    
    sState = 0;
    STL_TRY( stlReleaseSemaphore( &aDataManager->mSema[aDataManager->mProdIdx],
                                  aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            stlReleaseSemaphore( &aDataManager->mSema[aDataManager->mProdIdx],
                                 aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}


/**
 * @brief gloader의 Disk IO를 수행하는 Reader Thread Func.
 * @param [in] aThread  stlthread.
 * @param [in] aArg     thread arguments.
 */
void * STL_THREAD_FUNC ztdReadDataFileByThread(stlThread * aThread,
                                               void      * aArg )
{
    ztdFileAndBuffer  * sFile;
    ztdDataManager    * sDataManager;
    stlErrorStack     * sErrorStack;

    sFile        = ((ztdReadThreadArg *) aArg)->mDataFile;
    sDataManager = ((ztdReadThreadArg *) aArg)->mDataManager;
    sErrorStack  = &((ztdReadThreadArg *) aArg)->mErrorStack;

    STL_TRY( ztdFillData( sDataManager,
                          sFile,
                          sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlExitThread( aThread,
                            STL_SUCCESS,
                            sErrorStack )
             == STL_SUCCESS );
    
    return NULL;

    STL_FINISH;

    /**
     * Read Thread의 비정상 종료를 설정한다.
     */
    gZtdRunState = STL_FALSE;

    (void) stlExitThread( aThread,
                          STL_FAILURE,
                          sErrorStack );

    return NULL;
}


/**
 * @brief Data File을 Buffer에 Read하는 Func.
 * @param [in] aDataManager     Double buffering 관리하는 struct
 * @param [in] aFileAndBuffer   File Descriptor 와 string buffer 담는 struct
 * @param [in] aErrorStack      error stack
 */ 
stlStatus ztdFillData( ztdDataManager   * aDataManager,
                       ztdFileAndBuffer * aFileAndBuffer,
                       stlErrorStack    * aErrorStack )
{
    stlInt32 sState = 0;
    stlBool  sFinish = STL_FALSE;
    
    while( STL_TRUE )
    {
        STL_TRY( stlAcquireSemaphore( &aDataManager->mSema[aDataManager->mReadIdx],
                                      aErrorStack ) == STL_SUCCESS );
        sState = 1;
        
        STL_TRY( gZtdRunState == STL_TRUE );

        if( (gZtdErrorCount > gZtdMaxErrorCount) &&
            (gZtdMaxErrorCount > 0) )
        {
            aDataManager->mState[aDataManager->mReadIdx] = ZTD_BUFFER_STATE_FINISH;
            break;
        }
        
        /**
         * State 의 값이 INIT이 일때까지 기다린다.
         */ 
        if( aDataManager->mState[aDataManager->mReadIdx] != ZTD_BUFFER_STATE_INIT )
        {
            STL_TRY( stlReleaseSemaphore( &aDataManager->mSema[aDataManager->mReadIdx],
                                          aErrorStack ) == STL_SUCCESS );
            continue;
        }
        
        STL_TRY( ztdReadFile( aDataManager->mAlignedBuffer[aDataManager->mReadIdx],
                              aFileAndBuffer,
                              &aDataManager->mSize[aDataManager->mReadIdx],
                              &sFinish,
                              aErrorStack ) == STL_SUCCESS );
        
        if( sFinish == STL_TRUE )
        {
            aDataManager->mState[aDataManager->mReadIdx] = ZTD_BUFFER_STATE_FINISH;
            break;
        }
        else
        {
            aDataManager->mState[aDataManager->mReadIdx] = ZTD_BUFFER_STATE_READY;
            sState = 0;
            STL_TRY( stlReleaseSemaphore( &aDataManager->mSema[aDataManager->mReadIdx],
                                          aErrorStack ) == STL_SUCCESS );
            
            aDataManager->mReadIdx = (aDataManager->mReadIdx + 1) % ZTD_BUFFER_COUNT;
        }
    }

    sState = 0;
    STL_TRY( stlReleaseSemaphore( &aDataManager->mSema[aDataManager->mReadIdx],
                                  aErrorStack ) == STL_SUCCESS );
    return STL_SUCCESS;

    STL_FINISH;

    aDataManager->mState[aDataManager->mReadIdx] = ZTD_BUFFER_STATE_FINISH;

    switch( sState )
    {
        case 1:
            (void) stlReleaseSemaphore( &aDataManager->mSema[aDataManager->mReadIdx],
                                        aErrorStack );
        default:
            break;
    }
    return STL_FAILURE;
}
