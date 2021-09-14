/*******************************************************************************
 * ztdExecThread.c
 *
 * Copyright (c) 2012, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
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
#include <ztdBinaryMode.h>
#include <ztdBinaryConsumerThread.h>
#include <ztdFileExecute.h>
#include <ztdConsumerThread.h>
#include <ztdBufferedFile.h>

extern stlFile             gZtdBadFile;
extern stlInt8             gZtdEndian;
extern dtlCharacterSet     gZtdCharacterSet;
extern dtlCharacterSet     gZtdDatabaseCharacterSet;
extern SQLSMALLINT         gZtdColumnCount;
extern stlInt32            gZtdReadBlockCount;
extern stlUInt32           gZtdCompleteCount;
extern stlUInt32           gZtdErrorCount;
extern stlUInt32           gZtdCompleteCountState;
extern stlBool             gZtdRunState;
extern volatile stlUInt32  gZtdBinaryModeExecuteRunningCount; /* binary import에서 동작 중인 execute thread의 개수 */
extern stlInt32            gZtdMaxErrorCount;
extern ztdManager          gZtdManager;

/**
 * @brief valeu block list를 fetch받아 queue에 전달
 * @param [in]     aThread  stlthread.
 * @param [in/out] aArg     thread arguments.
 */
void * STL_THREAD_FUNC ztdFetchValueBlockList( stlThread   * aThread,
                                               void        * aArg )
{
    SQLHENV                    sEnv                       = NULL;
    SQLHDBC                    sDbc                       = NULL;
    SQLHSTMT                   sStmt                      = NULL;
    SQLRETURN                  sReturn;

    stlAllocator               sAllocator;
    stlDynamicAllocator      * sDynamicAllocator          = NULL;
    
    stlChar                    sSqlCommand[ZTD_MAX_COMMAND_BUFFER_SIZE];

    ztdInputArguments        * sInputArguments            = NULL;
    ztdControlInfo           * sControlInfo               = NULL;
    
    dtlValueBlockList        * sValueBlockList            = NULL;
    dtlValueBlockList        * sFetchBlockList            = NULL;
    dtlValueBlockList        * sTempFetchBlockList        = NULL;
    dtlValueBlockList        * sTempValueBlockList        = NULL;

    dtlValueBlock            * sTempFetchBlock            = NULL;
    dtlValueBlock            * sTempValueBlock            = NULL;

    dtlDataValue             * sTempFetchDataValue        = NULL;
    dtlDataValue             * sTempValueDataValue        = NULL;

    stlInt32                   sBufferIndex               = 0;
    
    ztdBinaryWriteDataQueue  * sBinaryWriteQueue;
    ztdFileAndBuffer         * sFileAndBuffer             = NULL; 
    dtlValueBlockList        * sSwapValueBlockList        = NULL;
    stlInt32                   sColumnIndex               = 0;
    stlInt32                   sDataValueIndex            = 0;

    stlInterval                sAcquireTime               = ZTD_CPU_SLEEP_SEC;
    stlBool                    sIsAcquireTimeOut          = STL_FALSE;
    
    stlErrorStack            * sErrorStack                = NULL;
    
    stlInt32                   sState                     = 0;

    sEnv              = ( ( ztdBinaryModeWriteThreadArg * ) aArg )->mEnv;
    sDynamicAllocator = ( ( ztdBinaryModeWriteThreadArg * ) aArg )->mDynamicAllocator;
    sInputArguments   = ( ( ztdBinaryModeWriteThreadArg * ) aArg )->mInputArguments;
    sControlInfo      = ( ( ztdBinaryModeWriteThreadArg * ) aArg )->mControlInfo;
    sValueBlockList   = ( ( ztdBinaryModeWriteThreadArg * ) aArg )->mValueBlockList;
    sBinaryWriteQueue = ( ( ztdBinaryModeWriteThreadArg * ) aArg )->mBinaryWriteQueue;
    sFileAndBuffer    = ( ( ztdBinaryModeWriteThreadArg * ) aArg )->mFileAndBuffer;
    sErrorStack       = &( ( ztdBinaryModeWriteThreadArg * ) aArg )->mErrorStack;
    
    STL_TRY( stlCreateRegionAllocator( &sAllocator,
                                       ZTD_REGION_INIT_SIZE,
                                       ZTD_REGION_NEXT_SIZE,
                                       sErrorStack )
             == STL_SUCCESS );
    sState = 1;
    
    /**
     * database에 연결
     */
    STL_TRY( ztdOpenDatabaseThread( sEnv,
                                    &sDbc,
                                    &sStmt,
                                    sInputArguments->mDsn,
                                    sInputArguments->mId,
                                    sInputArguments->mPassword,
                                    sControlInfo->mCharacterSet,
                                    sErrorStack )
             == STL_SUCCESS );
    sState = 2;
    
    stlSnprintf( sSqlCommand,
                 ZTD_MAX_COMMAND_BUFFER_SIZE,
                 "ALTER SESSION SET BLOCK_READ_COUNT = %d",
                 gZtdReadBlockCount );

    STL_TRY( SQLExecDirect( sStmt,
                            (SQLCHAR *)sSqlCommand,
                            SQL_NTS )
             == SQL_SUCCESS );
    
    stlSnprintf( sSqlCommand,
                 ZTD_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT * FROM %s.%s",
                 sControlInfo->mSchema,
                 sControlInfo->mTable );
  
    STL_TRY( SQLExecDirect( sStmt,
                            (SQLCHAR *) sSqlCommand,
                            SQL_NTS )
             == SQL_SUCCESS );
    sState = 3;
    
    /**
     * value block list를 fetch받아서 queue에 넣는다.
     */
    while( STL_TRUE )
    {
        /**
         * gloader의 비정상 종료를 확인한다.
         */
        STL_TRY( gZtdRunState == STL_TRUE );
        
        /**
         * fetch
         */
        sReturn = zllFetch4Exp( sStmt,
                                &sAllocator,
                                &sFetchBlockList );

        if( sReturn == SQL_SUCCESS )
        {
            /**
             * copy value block list.
             */
            sTempFetchBlockList = sFetchBlockList;
            sTempValueBlockList = sValueBlockList;

            sColumnIndex = 0;
            while( sTempFetchBlockList != NULL )
            {
                sTempFetchBlock = sTempFetchBlockList->mValueBlock;
                sTempValueBlock = sTempValueBlockList->mValueBlock;

                sTempValueBlock->mAllocBlockCnt     = sTempFetchBlock->mAllocBlockCnt;
                sTempValueBlock->mUsedBlockCnt      = sTempFetchBlock->mUsedBlockCnt;
                sTempValueBlock->mArgNum1           = sTempFetchBlock->mArgNum1;
                sTempValueBlock->mArgNum2           = sTempFetchBlock->mArgNum2;
                sTempValueBlock->mStringLengthUnit  = sTempFetchBlock->mStringLengthUnit;
                sTempValueBlock->mIntervalIndicator = sTempFetchBlock->mStringLengthUnit;
                sTempValueBlock->mIntervalIndicator = sTempFetchBlock->mIntervalIndicator;

                for( sDataValueIndex = 0; sDataValueIndex < sTempValueBlock->mUsedBlockCnt; sDataValueIndex++ )
                {
                    sTempFetchDataValue = &sTempFetchBlock->mDataValue[sDataValueIndex];
                    sTempValueDataValue = &sTempValueBlock->mDataValue[sDataValueIndex];;                    
                    sTempValueDataValue->mType       = sTempFetchDataValue->mType;
                    sTempValueDataValue->mLength     = sTempFetchDataValue->mLength;

                    if( (sTempValueDataValue->mType == DTL_TYPE_LONGVARCHAR) ||
                        (sTempValueDataValue->mType == DTL_TYPE_LONGVARBINARY) )
                    {
                        if( sTempValueDataValue->mBufferSize < sTempFetchDataValue->mLength )
                        {
                            STL_TRY( stlFreeDynamicMem( sDynamicAllocator,
                                                        sTempValueDataValue->mValue,
                                                        sErrorStack )
                                     == STL_SUCCESS );

                            STL_TRY( stlAllocDynamicMem( sDynamicAllocator,
                                                         sTempFetchDataValue->mLength,
                                                         &sTempValueDataValue->mValue,
                                                         sErrorStack )
                                     == STL_SUCCESS );

                            stlMemset( sTempValueDataValue->mValue,
                                       0x00,
                                       sTempFetchDataValue->mLength );

                            sTempValueDataValue->mBufferSize = sTempFetchDataValue->mLength;
                        }
                        else
                        {
                            /* Do Nothing */
                        }
                    }
                    else
                    {
                        /* Do Nothing */
                    }

                    stlMemcpy( sTempValueDataValue->mValue,
                               sTempFetchDataValue->mValue,
                               sTempFetchDataValue->mLength );
                }
                
                sColumnIndex++;

                sTempFetchBlockList = sTempFetchBlockList->mNext;
                sTempValueBlockList = sTempValueBlockList->mNext;
            }
        }
        else if( sReturn == SQL_NO_DATA )
        {
            break;
        }
        else
        {
            STL_THROW( STL_FINISH_LABEL );
        }

        while( STL_TRUE )
        {
            /**
             * 일정 시간 queue의 empty의 획득을 시도하여, item을 queue에 넣는다.
             */
            sIsAcquireTimeOut = STL_FALSE;

            STL_TRY( stlTimedAcquireSemaphore( &sBinaryWriteQueue->mEmpty,
                                               sAcquireTime,
                                               &sIsAcquireTimeOut,
                                               sErrorStack )
                     == STL_SUCCESS );
            
            if( sIsAcquireTimeOut == STL_FALSE )
            {
                sBufferIndex = sBinaryWriteQueue->mPutNumber % ZTD_VALUE_BLOCK_LIST_WRITE_QUEUE_SIZE;
                
                sSwapValueBlockList                                = sBinaryWriteQueue->mBuffer[sBufferIndex]->mValueBlockList;
                sBinaryWriteQueue->mBuffer[sBufferIndex]->mValueBlockList = sValueBlockList;
                sValueBlockList                                    = sSwapValueBlockList;

                sBinaryWriteQueue->mPutNumber++;
                /**
                 * item이 추가 되었으므로 release해준다.
                 */
                STL_TRY( stlReleaseSemaphore( &sBinaryWriteQueue->mItem,
                                              sErrorStack )
                         == STL_SUCCESS );

                break;
            }
            else
            {
                /* Do Nothing */
            }
        }
    }

    /**
     * database의 연결 해지
     */
    sState = 2;
    STL_TRY( SQLCloseCursor( sStmt ) == SQL_SUCCESS );

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

    /**
     * 작업이 완료되었으므로 item Semaphore를 해지한다.
     */
    STL_TRY( stlReleaseSemaphore( &sBinaryWriteQueue->mItem,
                                  sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlExitThread( aThread,
                            STL_SUCCESS,
                            sErrorStack )
             == STL_SUCCESS );

    return NULL;
    
    STL_FINISH;

    gZtdRunState = STL_FALSE;
    
    switch( sState )
    {
        case 3:
            (void) ztdCheckError( sEnv,
                                  sDbc,
                                  sStmt,
                                  0,
                                  sFileAndBuffer,
                                  NULL,
                                  sErrorStack );
            (void) SQLCloseCursor( sStmt );
        case 2:
            (void) ztdCheckError( sEnv,
                                  sDbc,
                                  sStmt,
                                  0,
                                  sFileAndBuffer,
                                  NULL,
                                  sErrorStack );
            
            (void) ztdCloseDatabaseThread( sDbc,
                                           sStmt,
                                           STL_FALSE,   /* COMMIT */
                                           sErrorStack );
        case 1:
            (void) stlDestroyRegionAllocator( &sAllocator,
                                              sErrorStack );
        default :
            /**
             * 작업이 완료되었으므로 item Semaphore를 해지한다.
             */
            (void) stlReleaseSemaphore( &sBinaryWriteQueue->mItem,
                                        sErrorStack );
    
            (void) stlExitThread( aThread,
                                  STL_FAILURE,
                                  sErrorStack );
            break;
    }

    return NULL;
};

/**
 * @brief 자신의 queue를 확인하여 value block list를 file에 기록한다.
 * @param [in]     aThread  stlthread.
 * @param [in/out] aArg     thread arguments.
 */
void * STL_THREAD_FUNC ztdWriteValueBlockList( stlThread * aThread,
                                               void      * aArg )
{
    ztdInputArguments          * sInputArguments     = NULL;
    ztdControlInfo             * sControlInfo        = NULL;

    dtlValueBlockList          * sValueBlockList     = NULL;
    dtlValueBlockList          * sWorkBlockList      = NULL;

    ztdWriteBufferedFile         sWriteBufferedFile;
    
    ztdBinaryWriteDataQueue    * sBinaryWriteQueue   = NULL;

    stlBool                      sFirst              = STL_TRUE;
   
    stlChar                    * sHeader;
    stlInt32                     sHeaderSize         = 0;

    stlInt32                     sBufferIndex        = 0;

    stlInterval                  sAcquireTime          = ZTD_CPU_SLEEP_SEC;
    stlBool                      sIsAcquireTimeOut     = STL_FALSE;
    
    dtlValueBlockList          * sSwapValueBlockList = NULL;

    stlInt32                     sUsedDataCount      = 0;

    stlAllocator               * sAllocator          = 0;
    stlInt32                     sSize               = 0;

    stlUInt64                    sMonitorRecordUnit  = ZTD_MONITOR_RECORD_UNIT;
    
    stlErrorStack              * sErrorStack         = NULL;

    stlInt32                     sState              = 0;

    stlUInt32                    sOldCountState      = ZTD_COUNT_STATE_RELEASE;

    ztdColumnInfo              * sColumnInfo         = NULL;
    stlInt32                     i                   = 0;
    
    sInputArguments       = ( ( ztdBinaryModeWriteThreadArg * ) aArg )->mInputArguments;
    sControlInfo          = ( ( ztdBinaryModeWriteThreadArg * ) aArg )->mControlInfo;
    sValueBlockList       = ( ( ztdBinaryModeWriteThreadArg * ) aArg )->mValueBlockList;
    sBinaryWriteQueue     = ( ( ztdBinaryModeWriteThreadArg * ) aArg )->mBinaryWriteQueue;
    sHeaderSize           = ( ( ztdBinaryModeWriteThreadArg * ) aArg )->mHeaderSize;
    sAllocator            = ( ( ztdBinaryModeWriteThreadArg * ) aArg )->mAllocator;
    sColumnInfo           = ( ( ztdBinaryModeWriteThreadArg * ) aArg )->mColumnInfo;
    sErrorStack           = &( ( ztdBinaryModeWriteThreadArg * ) aArg )->mErrorStack;
    
    /**
     * binary file format으로 변경된 헤더 부분을 저장할 버퍼를 할당
     */
    sSize = sHeaderSize + ZTD_ALIGN_512;
    
    STL_TRY( stlAllocRegionMem( sAllocator,
                                sSize,
                                (void **) &sHeader,
                                sErrorStack )
             == STL_SUCCESS );
    stlMemset( sHeader, 0x00, sSize );

    ZTD_ALIGN_BUFFER( sHeader,
                      sHeader,
                      ZTD_ALIGN_512 );

    /**
     * sWriteBuferedFile 생성 및 초기화
     */
    STL_TRY( ztdInitializeWriteBufferedFile( &sWriteBufferedFile,
                                             sInputArguments,
                                             sControlInfo,
                                             sAllocator,
                                             sErrorStack )
             == STL_SUCCESS );
    sState = 1;
    
    /**
     * queue를 확인하여 데이터를 파일에 기록한다.
     */
    while( STL_TRUE )
    {
        /**
         * gloader의 비정상 종료를 확인한다.
         */
        STL_TRY( gZtdRunState == STL_TRUE );
        
        /**
         * 일정 시간 queue의 item의 획득을 시도하여, item을 queue에에서 가져온다.
         */
        sIsAcquireTimeOut = STL_FALSE;

        STL_TRY( stlTimedAcquireSemaphore( &sBinaryWriteQueue->mItem,
                                           sAcquireTime,
                                           &sIsAcquireTimeOut,
                                           sErrorStack )
                 == STL_SUCCESS );
        
        if( sIsAcquireTimeOut == STL_FALSE )
        {
            /**
             * queue의 데이터를 가져온다.
             */
            sBufferIndex = sBinaryWriteQueue->mGetNumber % ZTD_VALUE_BLOCK_LIST_WRITE_QUEUE_SIZE;
            
            sSwapValueBlockList                                = sBinaryWriteQueue->mBuffer[sBufferIndex]->mValueBlockList;
            sBinaryWriteQueue->mBuffer[sBufferIndex]->mValueBlockList = sValueBlockList;
            sValueBlockList                                    = sSwapValueBlockList;

            sUsedDataCount = sValueBlockList->mValueBlock->mUsedBlockCnt;
            
            sBinaryWriteQueue->mGetNumber++;
            
            /**
             * queue에 empty가 추가 되었으므로 release해준다.
             */
            STL_TRY( stlReleaseSemaphore( &sBinaryWriteQueue->mEmpty,
                                          sErrorStack )
                     == STL_SUCCESS );

            /**
             * 획득한 sValueBlockList의 Used Block Count가 0이면, 작업을 종료한다.
             */
            if( ZTD_GET_USED_BLOCK_COUNT( sValueBlockList ) == 0 )
            {
                /**
                 * Queue에 Item이 없으므로 작업을 종료한다.
                 */
                break;
            }
   
            /**
             * 처음인 경우, data file에 header를 쓴다.
             */
            if( sFirst == STL_TRUE )
            {
                STL_TRY( ztdOrganizeHeader( sValueBlockList,
                                            sHeader,
                                            sHeaderSize,
                                            sErrorStack )
                         == STL_SUCCESS );

                STL_TRY( stlWriteFile( &sWriteBufferedFile.mFileDesc,
                                       (void *) sHeader,
                                       sHeaderSize,
                                       NULL,
                                       sErrorStack )
                     == STL_SUCCESS );

                sWriteBufferedFile.mCurrentFileSize += sHeaderSize;
                sFirst                               = STL_FALSE;
            }

            /**
             * sValueBlock의 내용을 파일에 기록한다.
             */            
            STL_TRY( ztdWriteValueBlockListData( &sWriteBufferedFile,
                                                 sValueBlockList,
                                                 sInputArguments,
                                                 sControlInfo,
                                                 sHeader,
                                                 sHeaderSize,
                                                 sErrorStack )
                     == STL_SUCCESS );

            /**
             * Complete Count를 증가 시킨다.
             */
            while( STL_TRUE )
            {
                sOldCountState = stlAtomicCas32( (volatile stlUInt32 *) &gZtdCompleteCountState,
                                                 ZTD_COUNT_STATE_ACQUIRE,
                                                 ZTD_COUNT_STATE_RELEASE );

                if( sOldCountState == ZTD_COUNT_STATE_RELEASE )
                {
                    gZtdCompleteCount += sUsedDataCount;

                    stlAtomicSet32( (volatile stlUInt32*) &gZtdCompleteCountState,
                                    ZTD_COUNT_STATE_RELEASE );

                    break;
                }
            }
            
            /**
             * 모니터링 정보를 출력한다.
             */
            while( STL_TRUE )
            {
                if( gZtdCompleteCount >= sMonitorRecordUnit )
                {
                    if( sInputArguments->mSilent == STL_FALSE )
                    {
                        stlPrintf( "downloaded %ld records into %s.%s\n\n",
                                   sMonitorRecordUnit,
                                   sControlInfo->mSchema,
                                   sControlInfo->mTable );
                    }

                    sMonitorRecordUnit += ZTD_MONITOR_RECORD_UNIT;
                }
                else
                {
                    break;
                }
            }

            /**
             * 사용된 ValueBlockList의 Used Block Count을 0으로 초기화한다.
             */
            ZTD_SET_USED_BLOCK_COUNT( sValueBlockList, 0 );
        }
        else
        {
            /* Do Nothing */
        }
    }

    /**
     * buffered file에 남은 내용을 파일에 기록
     */
    STL_TRY( ztdWriteDataBufferedFile( &sWriteBufferedFile,
                                       sInputArguments,
                                       sControlInfo,
                                       NULL,
                                       0,
                                       sHeader,
                                       sHeaderSize,
                                       STL_TRUE,
                                       sErrorStack )
             == STL_SUCCESS );

    /**
     * Header가 기록되지 않은 경우 (No Row)
     */
    if( sFirst == STL_TRUE )
    {
        /**
         * 조회된 Value Block List가 없으므로, 임의 구성한다.
         */
        sWorkBlockList = sValueBlockList;
        for( i = 0; i < gZtdColumnCount; i++ )
        {
            sWorkBlockList->mValueBlock->mDataValue->mType = sColumnInfo[i].mDbType;
            sWorkBlockList->mValueBlock->mDataValue->mBufferSize = sColumnInfo[i].mBufferSize;
            sWorkBlockList = sWorkBlockList->mNext;
        }
        
        STL_TRY( ztdOrganizeHeader( sValueBlockList,
                                    sHeader,
                                    sHeaderSize,
                                    sErrorStack )
                 == STL_SUCCESS );
                
        STL_TRY( stlWriteFile( &sWriteBufferedFile.mFileDesc,
                               (void *) sHeader,
                               sHeaderSize,
                               NULL,
                               sErrorStack )
                 == STL_SUCCESS );
                
        sWriteBufferedFile.mCurrentFileSize += sHeaderSize;
        sFirst                               = STL_FALSE;
    }
    
    /**
     * sWriteBufferedFile을 끝낸다.
     */
    sState = 0;
    STL_TRY( ztdTerminateWriteBufferedFile( &sWriteBufferedFile,
                                            sErrorStack )
             == STL_SUCCESS );

    /**
     * 자신의 thread 상태를 complete로 변경한다.
     */
    
    STL_TRY( stlExitThread( aThread,
                            STL_SUCCESS,
                            sErrorStack ) == STL_SUCCESS );

    return NULL;

    STL_FINISH;

    gZtdRunState = STL_FALSE;

    switch ( sState )
    {
        case 1:
            (void) ztdTerminateWriteBufferedFile( &sWriteBufferedFile,
                                                  sErrorStack );
        default :
            (void) stlExitThread( aThread,
                                  STL_FAILURE,
                                  sErrorStack );
            break;
    }
    
    return NULL;
}

stlStatus ztdSetValueBlockList( dtlValueBlockList       ** aReadValueBlockList,
                                ztdValueBlockColumnInfo  * aValueBlockColumnInfo,
                                stlInt32                   aColumnCount )
{
    dtlValueBlockList   * sBlockList            = NULL;
    dtlValueBlock       * sValueBlock           = NULL;
    dtlDataValue        * sDataValue            = NULL;
    
    stlInt32              sValueBlockListIndex  = 0;
    stlInt32              sDataValueIndex       = 0;
    
    /**
     * aReadValueBlockList의 값을 설정한다.
     */
    for( sValueBlockListIndex = 0; sValueBlockListIndex < aColumnCount; sValueBlockListIndex++ )
    {
        sBlockList   = aReadValueBlockList[sValueBlockListIndex];
        sValueBlock  = sBlockList->mValueBlock;
        
        sBlockList->mTableID                    = 0;
        sBlockList->mColumnOrder                = 0;
        sBlockList->mColumnInRow.mValue         = NULL;
        sBlockList->mColumnInRow.mLength        = 0;
        sBlockList->mValueBlock->mIsSepBuff     = STL_TRUE;
        sBlockList->mValueBlock->mSkipBlockCnt  = 0;
                
        sValueBlock->mAllocBlockCnt      = gZtdReadBlockCount;
        sValueBlock->mSkipBlockCnt       = 0;
        sValueBlock->mArgNum1            = aValueBlockColumnInfo[sValueBlockListIndex].mArgNum1;
        sValueBlock->mArgNum2            = aValueBlockColumnInfo[sValueBlockListIndex].mArgNum2;
        sValueBlock->mStringLengthUnit   = aValueBlockColumnInfo[sValueBlockListIndex].mStringLengthUnit;
        sValueBlock->mIntervalIndicator  = aValueBlockColumnInfo[sValueBlockListIndex].mIntervalIndicator;
        sValueBlock->mAllocBufferSize    = aValueBlockColumnInfo[sValueBlockListIndex].mAllocBufferSize;

        for( sDataValueIndex = 0; sDataValueIndex < gZtdReadBlockCount; sDataValueIndex++ )
        {
            sDataValue        = ZTD_GET_DATA_VALUE( sValueBlock, sDataValueIndex );
            sDataValue->mType = aValueBlockColumnInfo[sValueBlockListIndex].mType;
        }
    }

    return STL_SUCCESS;
}

/**
 * @brief value block list가 저장된 데이터 파일의 내용을 읽는다.
 * @param [in]     aThread  stlthread.
 * @param [in/out] aArg     thread arguments.
 */
void * STL_THREAD_FUNC ztdReadValueBlockList( stlThread * aThread,
                                              void      * aArg )
{
    stlInt32                     sUsedBlockCnt                    = 0;
    dtlValueBlockList         ** sValueBlockList                  = NULL;    
    dtlValueBlockList          * sBlockList                       = NULL;
    dtlValueBlock              * sValueBlock                      = NULL;
    dtlDataValue               * sDataValue                       = NULL;
    stlInt32                     sValueBlockListIndex             = 0;
    stlInt32                     sDataValueIndex                  = 0;

    ztdImportValueBlockList    * sExecuteValueBlockList           = NULL;
    ztdImportValueBlockList    * sWriteBadValueBlockList          = NULL;

    stlThread                    sDataFileReadThread;
    ztdBinaryDoubleBuffer      * sDoubleBuffer                    = NULL;
    ztdDoubleBufferThreadArg     sDataFileReadThreadArg;
    
    stlChar                    * sHeader                          = NULL;
    stlInt32                     sHeaderSize                      = 0;

    stlAllocator                 sAllocator;
    stlDynamicAllocator        * sDynamicAllocator                = NULL;
    
    ztdBinaryReadDataQueue     * sExecuteValueBlockListQueue      = NULL;
    ztdBinaryReadDataQueue     * sWriteBadValueBlockListQueue     = NULL;
    stlInt32                     sQueueTop                        = 0;
    stlInt32                     sQueueIndex                      = 0;
    
    ztdReadBufferedFile          sReadBufferedFile;

    ztdColumnInfo              * sColumnInfo                      = NULL;
    ztdValueBlockColumnInfo    * sValueBlockColumnInfo            = NULL;

    stlInt32                     sThreadUnit                      = 0;
    
    stlInt8                      sEndian;
    stlInt32                     sVersion                         = 0;
    SQLSMALLINT                  sColumnCount                     = 0;
    dtlCharacterSet              sCharacterSet                    = DTL_CHARACTERSET_MAX;

    ztdBinaryReadDataQueue     * sWorkQueue                       = NULL;
    stlInt32                     sBufferIndex                     = 0;
    
    stlBool                      sIsEndOfFile                     = STL_FALSE;

    stlInterval                  sAcquireTime                     = ZTD_CPU_SLEEP_SEC;
    stlBool                      sIsAcquireTimeOut                = STL_FALSE;
    
    dtlValueBlockList         ** sSwapValueBlockList              = NULL;
    stlInt32                     sSize                            = 0;
    stlUInt8                     sLengthFlag                      = 0;
    stlUInt16                    sLength16                        = 0;
    stlUInt32                    sLength32                        = 0;
    
    stlInt32                     sState                           = 0;
    stlStatus                    sReturn                          = STL_FAILURE;
    
    stlErrorStack              * sErrorStack                      = NULL;

    stlChar                    * sConvertBuffer                   = NULL;
    stlInt64                     sConvertBufferSize               = 0;
    stlInt64                     sAllocSize                       = 0;
    stlInt64                     sOffset                          = 0;
    stlInt64                     sToTalLength                     = 0;
    
    stlInt32                     i                                = 0;
    ztdInputArguments          * sInputArgs;
    ztdBinaryModeReadThreadArg * sThreadArg;

    sThreadArg = (ztdBinaryModeReadThreadArg*) aArg;

    sDynamicAllocator             = sThreadArg->mDynamicAllocator;
    sValueBlockList               = sThreadArg->mValueBlockList;
    sExecuteValueBlockList        = sThreadArg->mExecuteValueBlockList;
    sWriteBadValueBlockList       = sThreadArg->mWriteBadValueBlockList;
    sExecuteValueBlockListQueue   = sThreadArg->mBinaryExecuteQueue;
    sWriteBadValueBlockListQueue  = sThreadArg->mBinaryWriteBadQueue;
    sValueBlockColumnInfo         = sThreadArg->mValueBlockColumnInfo;
    sColumnInfo                   = sThreadArg->mColumnInfo;
    sColumnCount                  = sThreadArg->mColumnCount;
    sHeaderSize                   = sThreadArg->mHeaderSize;
    sThreadUnit                   = sThreadArg->mThreadUnit;
    sErrorStack                   = &sThreadArg->mErrorStack;
    sInputArgs                    = sThreadArg->mInputArguments;
    
    STL_TRY( stlCreateRegionAllocator( &sAllocator,
                                       ZTD_REGION_INIT_SIZE,
                                       ZTD_REGION_NEXT_SIZE,
                                       sErrorStack )
             == STL_SUCCESS );
    sState = 1;
    
    sSize = sHeaderSize + ZTD_ALIGN_512;
    STL_TRY( stlAllocRegionMem( &sAllocator,
                                sSize,
                                (void **) &sHeader,
                                sErrorStack )
             == STL_SUCCESS );
    stlMemset( sHeader, 0x00, sSize );

    ZTD_ALIGN_BUFFER( sHeader, sHeader, ZTD_ALIGN_512 );

    /**
     * sReadBufferedFile 생성 및 초기화
     */
    STL_TRY( ztdInitializeReadBufferedFile( &sReadBufferedFile,
                                            sInputArgs,
                                            &sAllocator,
                                            sErrorStack )
             == STL_SUCCESS );
    sState = 2;

    sDoubleBuffer = &sReadBufferedFile.mDoubleBuffer;

    /**
     * header 부분을 읽는다.
     */
    STL_TRY( ztdReadHeaderBufferedFile( &sReadBufferedFile,
                                        sHeader,
                                        sHeaderSize,
                                        sColumnInfo,
                                        &sEndian,
                                        &sVersion,
                                        &sCharacterSet,
                                        &sColumnCount,
                                        sValueBlockColumnInfo,
                                        sErrorStack )
             == STL_SUCCESS );
    
    /*
     * header에 저장된 character set으로 변경한다.
     */
    gZtdCharacterSet = sCharacterSet;
    
    /**
     * data file read thread를 생성한다.
     */
    sDataFileReadThreadArg.mReadBufferedFile = &sReadBufferedFile;
    sDataFileReadThreadArg.mDoubleBuffer     = sDoubleBuffer;

    STL_INIT_ERROR_STACK( &sDataFileReadThreadArg.mErrorStack );
    
    STL_TRY( stlCreateThread( &sDataFileReadThread,
                              NULL,
                              ztdReadBinaryFile,
                              (void *) &sDataFileReadThreadArg,
                              sErrorStack )
             == STL_SUCCESS );
    sState = 3;
    
    /**
     * value block list에서 변하지 않는 고정 값은 미리 초기화해 놓는다.
     * read를 아직 한번도 수행하지 않은 상태이므로 다른 thread의 value block list와 queue의 value block list에 값을 설정해도 동시성 제어에 문제가 없다.
     */
    STL_TRY( ztdSetValueBlockList( sValueBlockList,
                                   sValueBlockColumnInfo,
                                   sColumnCount )
             == STL_SUCCESS );

    for( sValueBlockListIndex = 0; sValueBlockListIndex < sThreadUnit; sValueBlockListIndex++ )
    {
        STL_TRY( ztdSetValueBlockList( (dtlValueBlockList **) sExecuteValueBlockList[sValueBlockListIndex].mValueBlockList,
                                       sValueBlockColumnInfo,
                                       sColumnCount )
                 == STL_SUCCESS );
    }

    STL_TRY( ztdSetValueBlockList( (dtlValueBlockList **) sWriteBadValueBlockList->mValueBlockList,
                                   sValueBlockColumnInfo,
                                   sColumnCount )
             == STL_SUCCESS );

    for( sQueueIndex = 0; sQueueIndex < sThreadUnit; sQueueIndex++ )
    {
        for( sBufferIndex = 0; sBufferIndex < ZTD_EXECUTE_QUEUE_SIZE; sBufferIndex++ )
        {
            STL_TRY( ztdSetValueBlockList( sExecuteValueBlockListQueue[sQueueIndex].mBuffer[sBufferIndex].mValueBlockList,
                                           sValueBlockColumnInfo,
                                           sColumnCount )
                     == STL_SUCCESS );
        }
    }

    for( sBufferIndex = 0; sBufferIndex < ZTD_WRITE_BAD_QUEUE_SIZE; sBufferIndex++ )
    {
        STL_TRY( ztdSetValueBlockList( sWriteBadValueBlockListQueue->mBuffer[sBufferIndex].mValueBlockList,
                                       sValueBlockColumnInfo,
                                       sColumnCount )
                 == STL_SUCCESS );
    }
    
    /**
     * 모든 value block list를 읽을 때까지 작업을 반복한다.
     */
    while( STL_TRUE )
    {
        /**
         * gloader의 비정상 종료를 확인한다.
         */
        STL_TRY( gZtdRunState == STL_TRUE );

        STL_TRY( ztdReadDataBufferedFile( &sReadBufferedFile,
                                          (void *) &sUsedBlockCnt,
                                          STL_SIZEOF( stlInt32 ),
                                          &sIsEndOfFile,
                                          sErrorStack )
                 == STL_SUCCESS );

        if( gZtdEndian != sEndian )
        {
            STL_TRY( dtlReverseByteOrderValue( DTL_TYPE_NATIVE_INTEGER,
                                               (void *) &sUsedBlockCnt,
                                               STL_FALSE,
                                               sErrorStack )
                     == STL_SUCCESS );
        }
        else
        {
            /* Do Nothing */
        }
        
        if( ( sIsEndOfFile == STL_TRUE ) ||
            ( sUsedBlockCnt == 0 ) )
        {
            STL_THROW( RAMP_FINISH );
        }
        else
        {
            /**
             * value block list의 모든 column의 내용을 읽을 때까지 반복
             */
            for( sValueBlockListIndex = 0; sValueBlockListIndex < sColumnCount; sValueBlockListIndex++ )
            {
                sBlockList  = sValueBlockList[sValueBlockListIndex];
                sValueBlock = sBlockList->mValueBlock;

                sValueBlock->mUsedBlockCnt = sUsedBlockCnt;

                /**
                 * column의 모든 데이터를 읽을 때까지 반복한다.
                 */
                for( sDataValueIndex = 0; sDataValueIndex < sUsedBlockCnt; sDataValueIndex++ )
                {
                    sDataValue = ZTD_GET_DATA_VALUE( sValueBlock, sDataValueIndex );

                    /**
                     * Data의 Length를 읽는다.
                     */
                    STL_TRY( ztdReadDataBufferedFile( &sReadBufferedFile,
                                                      (void *) &sLengthFlag,
                                                      1,
                                                      &sIsEndOfFile,
                                                      sErrorStack )
                             == STL_SUCCESS );

                    if( sLengthFlag <= ZTD_VALUE_LEN_1BYTE_MAX)
                    {
                        sDataValue->mLength = (stlInt64)sLengthFlag;
                    }
                    else if( sLengthFlag == ZTD_VALUE_LEN_2BYTE_FLAG )
                    {
                        sDataValue->mLength = 0;
                            
                        STL_TRY( ztdReadDataBufferedFile( &sReadBufferedFile,
                                                          (void *) &sLength16,
                                                          (ZTD_VALUE_LEN_2BYTE_SIZE - 1),
                                                          &sIsEndOfFile,
                                                          sErrorStack )
                                 == STL_SUCCESS );
                            
                        if( gZtdEndian != sEndian )
                        {
                            STL_TRY( dtlReverseByteOrderValue( DTL_TYPE_NATIVE_SMALLINT,
                                                               &sLength16,
                                                               STL_FALSE,
                                                               sErrorStack )
                                     == STL_SUCCESS );
                        }
                            
                        sDataValue->mLength = sLength16;
                    }
                    else
                    {
                        sDataValue->mLength = 0;
                            
                        STL_TRY( ztdReadDataBufferedFile( &sReadBufferedFile,
                                                          (void *) &sLength32,
                                                          (ZTD_VALUE_LEN_4BYTE_SIZE - 1),
                                                          &sIsEndOfFile,
                                                          sErrorStack )
                                 == STL_SUCCESS );
                            
                        if( gZtdEndian != sEndian )
                        {
                            STL_TRY( dtlReverseByteOrderValue( DTL_TYPE_NATIVE_INTEGER,
                                                               &sLength32,
                                                               STL_FALSE,
                                                               sErrorStack )
                                     == STL_SUCCESS );
                        }
                            
                        sDataValue->mLength = sLength32;
                    }

                    switch( sDataValue->mType )
                    {
                        case DTL_TYPE_LONGVARCHAR :
                            if( dtlGetMbMaxLength( gZtdCharacterSet ) > dtlGetMbMaxLength( gZtdDatabaseCharacterSet ) )
                            {
                                sAllocSize = sDataValue->mLength;
                            }
                            else
                            {
                                sAllocSize = sDataValue->mLength * (dtlGetMbMaxLength( gZtdDatabaseCharacterSet ) / dtlGetMbMaxLength( gZtdCharacterSet ));
                            }

                            if( sDataValue->mBufferSize < sAllocSize )
                            {
                                STL_TRY( stlFreeDynamicMem( sDynamicAllocator,
                                                            sDataValue->mValue,
                                                            sErrorStack )
                                         == STL_SUCCESS );

                                STL_TRY( stlAllocDynamicMem( sDynamicAllocator,
                                                             sAllocSize,
                                                             &sDataValue->mValue,
                                                             sErrorStack )
                                         == STL_SUCCESS );

                                stlMemset( sDataValue->mValue,
                                           0x00,
                                           sAllocSize );

                                sDataValue->mBufferSize = sAllocSize;
                            }
                            break;
                        case DTL_TYPE_LONGVARBINARY :
                            if( sDataValue->mBufferSize < sDataValue->mLength )
                            {
                                STL_TRY( stlFreeDynamicMem( sDynamicAllocator,
                                                            sDataValue->mValue,
                                                            sErrorStack )
                                         == STL_SUCCESS );

                                STL_TRY( stlAllocDynamicMem( sDynamicAllocator,
                                                             sDataValue->mLength,
                                                             &sDataValue->mValue,
                                                             sErrorStack )
                                         == STL_SUCCESS );

                                stlMemset( sDataValue->mValue,
                                           0x00,
                                           sDataValue->mLength );

                                sDataValue->mBufferSize = sDataValue->mLength;
                            }
                            break;
                        default :
                            break;
                    }

                    /**
                     * file에서 읽은 데이터 사이즈와 실제 데이터 사이즈를 검사한다.
                     */
                    if( gZtdCharacterSet == gZtdDatabaseCharacterSet )
                    {
                        /**
                         * Long Varchar와 Long Varbinary는 비교하지 않는다.
                         */
                        if( (sDataValue->mType != DTL_TYPE_LONGVARCHAR) &&
                            (sDataValue->mType != DTL_TYPE_LONGVARBINARY) )
                        {
                            STL_TRY_THROW( ( sColumnInfo[sValueBlockListIndex].mBufferSize >= sDataValue->mLength ),
                                           RAMP_ERR_DATA_SIZE );
                        }
                    }
                    else
                    {
                        /**
                         * Character set에 영향을 받는 type과 long data type은 검사하지 않는다.
                         */
                        if( (sDataValue->mType != DTL_TYPE_CHAR) &&
                            (sDataValue->mType != DTL_TYPE_VARCHAR) &&
                            (sDataValue->mType != DTL_TYPE_LONGVARCHAR) &&
                            (sDataValue->mType != DTL_TYPE_LONGVARBINARY) )
                        {
                            STL_TRY_THROW( ( sColumnInfo[sValueBlockListIndex].mBufferSize >= sDataValue->mLength ),
                                           RAMP_ERR_DATA_SIZE );
                        }
                    }
                    
                    switch( sDataValue->mType )
                    {
                        case DTL_TYPE_CHAR :
                        case DTL_TYPE_VARCHAR :
                        case DTL_TYPE_LONGVARCHAR :
                            if( gZtdCharacterSet == gZtdDatabaseCharacterSet )
                            {
                                STL_TRY( ztdReadDataBufferedFile( &sReadBufferedFile,
                                                                  (void *) sDataValue->mValue,
                                                                  sDataValue->mLength,
                                                                  &sIsEndOfFile,
                                                                  sErrorStack )
                                         == STL_SUCCESS );
                            }
                            else
                            {
                                sAllocSize = sValueBlockColumnInfo[sValueBlockListIndex].mBufferSize;
                                
                                if( sConvertBufferSize < sAllocSize )
                                {
                                    if( sConvertBuffer != NULL )
                                    {
                                        STL_TRY( stlFreeDynamicMem( sDynamicAllocator,
                                                                    sConvertBuffer,
                                                                    sErrorStack )
                                                 == STL_SUCCESS );

                                        sConvertBuffer = NULL;
                                    }

                                    STL_TRY( stlAllocDynamicMem( sDynamicAllocator,
                                                                 sAllocSize,
                                                                 (void**)&sConvertBuffer,
                                                                 sErrorStack )
                                             == STL_SUCCESS );

                                    stlMemset( sConvertBuffer, 0x00, sAllocSize );

                                    sConvertBufferSize = sAllocSize;
                                }

                                STL_TRY( ztdReadDataBufferedFile( &sReadBufferedFile,
                                                                  (void *)sConvertBuffer,
                                                                  sDataValue->mLength,
                                                                  &sIsEndOfFile,
                                                                  sErrorStack )
                                         == STL_SUCCESS );

                                STL_TRY( dtlMbConversion( gZtdCharacterSet,
                                                          gZtdDatabaseCharacterSet,
                                                          sConvertBuffer,
                                                          sDataValue->mLength,
                                                          sDataValue->mValue,
                                                          sDataValue->mBufferSize,
                                                          &sOffset,
                                                          &sToTalLength,
                                                          sErrorStack ) == STL_SUCCESS );

                                sDataValue->mLength = sToTalLength;
                            }
                            break;
                        default :
                            STL_TRY( ztdReadDataBufferedFile( &sReadBufferedFile,
                                                              (void *) sDataValue->mValue,
                                                              sDataValue->mLength,
                                                              &sIsEndOfFile,
                                                              sErrorStack )
                                     == STL_SUCCESS );
                            break;
                    }

                    /**
                     * endian 변경
                     */
                    if( gZtdEndian != sEndian )
                    {
                        switch( sDataValue->mType )
                        {
                            case DTL_TYPE_NATIVE_SMALLINT :
                            case DTL_TYPE_NATIVE_INTEGER :
                            case DTL_TYPE_NATIVE_BIGINT :
                            case DTL_TYPE_NATIVE_REAL :
                            case DTL_TYPE_NATIVE_DOUBLE :
                            case DTL_TYPE_DATE :
                            case DTL_TYPE_TIME :
                            case DTL_TYPE_TIME_WITH_TIME_ZONE :
                            case DTL_TYPE_TIMESTAMP :
                            case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE :
                            case DTL_TYPE_INTERVAL_YEAR_TO_MONTH :
                            case DTL_TYPE_INTERVAL_DAY_TO_SECOND :
                            case DTL_TYPE_FLOAT :
                            case DTL_TYPE_NUMBER :
                            case DTL_TYPE_DECIMAL :
                                STL_TRY( dtlReverseByteOrderValue( sDataValue->mType,
                                                                   sDataValue->mValue,
                                                                   STL_FALSE,
                                                                   sErrorStack )
                                         == STL_SUCCESS );
                                break;
                            case DTL_TYPE_ROWID :
                            case DTL_TYPE_BOOLEAN :
                            case DTL_TYPE_CHAR :
                            case DTL_TYPE_VARCHAR :
                            case DTL_TYPE_LONGVARCHAR :
                            case DTL_TYPE_CLOB :
                            case DTL_TYPE_BINARY :
                            case DTL_TYPE_VARBINARY :
                            case DTL_TYPE_LONGVARBINARY :
                            case DTL_TYPE_BLOB :
                            default:
                                break;
                        }
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }
            }
            
            /**
             * execute thread queue에 read한 자료를 넣는다.
             */
            while( STL_TRUE )
            {
                sQueueIndex = sQueueTop % sThreadUnit;
                sWorkQueue  = &sExecuteValueBlockListQueue[sQueueIndex];

                sQueueTop++;
                
                /**
                 * 일정 시간 queue의 empty의 획득을 시도하여, item을 queue에 넣는다.
                 */
                sIsAcquireTimeOut = STL_FALSE;
                
                STL_TRY( stlTimedAcquireSemaphore( &sWorkQueue->mEmpty,
                                                   sAcquireTime,
                                                   &sIsAcquireTimeOut,
                                                   sErrorStack )
                         == STL_SUCCESS );
                
                if( sIsAcquireTimeOut == STL_FALSE )
                {
                    sBufferIndex = sWorkQueue->mPutNumber % ZTD_EXECUTE_QUEUE_SIZE;
                    
                    sSwapValueBlockList                               = sWorkQueue->mBuffer[sBufferIndex].mValueBlockList;
                    sWorkQueue->mBuffer[sBufferIndex].mValueBlockList = sValueBlockList;
                    sValueBlockList                                   = sSwapValueBlockList;

                    sWorkQueue->mPutNumber++;

                    /**
                     * item이 추가 되었으므로 release해준다.
                     */
                    STL_TRY( stlReleaseSemaphore( &sWorkQueue->mItem,
                                                  sErrorStack )
                             == STL_SUCCESS );

                    break;
                }
                else
                {
                    STL_TRY_THROW( gZtdBinaryModeExecuteRunningCount != 0, RAMP_FINISH );
                }
            }
        }
    } /* end file */

    STL_RAMP( RAMP_FINISH );

    /**
     * 사용된 자원을 반환하고 동작을 종료한다.
     */
    if( sConvertBuffer != NULL )
    {
        STL_TRY( stlFreeDynamicMem( sDynamicAllocator,
                                    sConvertBuffer,
                                    sErrorStack )
                 == STL_SUCCESS );

        sConvertBuffer = NULL;
    }
    
    sState = 2;
    STL_TRY( stlJoinThread( &sDataFileReadThread,
                            &sReturn,
                            sErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( sReturn == STL_SUCCESS,
                   RAMP_ERR_CONSUMER_THREAD );
    
    sState = 1;
    STL_TRY( ztdTerminateReadBufferedFile( &sReadBufferedFile,
                                           sErrorStack )
             == STL_SUCCESS );

    sState = 0;
    STL_TRY( stlDestroyRegionAllocator( &sAllocator,
                                        sErrorStack )
             == STL_SUCCESS );

    for( sQueueIndex = 0; sQueueIndex < sThreadUnit; sQueueIndex++ )
    {
        sWorkQueue = &sExecuteValueBlockListQueue[sQueueIndex];

        STL_TRY_THROW( stlReleaseSemaphore( &sWorkQueue->mItem,
                                            sErrorStack ) == STL_SUCCESS,
                       RAMP_ERR_RELEASE_ITEM_SEMAPHORE );
    }
    
    STL_TRY( stlExitThread( aThread,
                            STL_SUCCESS,
                            sErrorStack )
             == STL_SUCCESS );
    
    return NULL;

    STL_CATCH( RAMP_ERR_RELEASE_ITEM_SEMAPHORE )
    {
        sQueueIndex++;
        
        for( ; sQueueIndex < sQueueIndex; i++ )
        {
            sWorkQueue = &sExecuteValueBlockListQueue[sQueueIndex];
            
            (void) stlReleaseSemaphore( &sWorkQueue->mItem,
                                        sErrorStack );
        }
    }
    
    STL_CATCH( RAMP_ERR_DATA_SIZE )
    {
        (void) stlPushError( STL_ERROR_LEVEL_ABORT,
                             ZTD_ERRCODE_INVALID_DATA_SIZE,
                             NULL,
                             sErrorStack );
    }

    STL_CATCH( RAMP_ERR_CONSUMER_THREAD )
    {
        stlAppendErrors( sErrorStack, &sDataFileReadThreadArg.mErrorStack );
    }
    
    STL_FINISH;

    gZtdRunState = STL_FALSE;
    
    if( sConvertBuffer != NULL )
    {
        (void)stlFreeDynamicMem( sDynamicAllocator, sConvertBuffer, sErrorStack );

        sConvertBuffer = NULL;
    }
    
    switch( sState )
    {
        case 3:
            (void) stlJoinThread( &sDataFileReadThread,
                                  &sReturn,
                                  sErrorStack );
            if( sReturn == STL_FAILURE )
            {
                stlAppendErrors( sErrorStack,
                                 &sDataFileReadThreadArg.mErrorStack );
            }
        case 2:
            (void) ztdTerminateReadBufferedFile( &sReadBufferedFile,
                                                 sErrorStack );
        case 1:
            (void) stlDestroyRegionAllocator( &sAllocator,
                                              sErrorStack );
        default :
            for( sQueueIndex = 0; sQueueIndex < sThreadUnit; sQueueIndex++ )
            {
                sWorkQueue = &sExecuteValueBlockListQueue[sQueueIndex];

                (void) stlReleaseSemaphore( &sWorkQueue->mItem,
                                            sErrorStack );
            }
            
            (void) stlExitThread( aThread,
                                  STL_FAILURE,
                                  sErrorStack );
            break;
    }

    return NULL;
}

/**
 * @brief value block list를 database에 uploader한다.
 * @param [in]     aThread  stlthread.
 * @param [in/out] aArg     thread arguments.
 */
void * STL_THREAD_FUNC ztdExecuteValueBlockList( stlThread * aThread,
                                                 void      * aArg )
{
    SQLHENV                    sEnv                                      = NULL;
    SQLHDBC                    sDbc                                      = NULL;
    SQLHSTMT                   sStmt                                     = NULL;
    
    ztdInputArguments        * sInputArguments                           = NULL;
    ztdControlInfo           * sControlInfo                              = NULL;

    stlInt32                   sColumnCount                              = 0;

    ztdBinaryReadDataQueue   * sExecuteValueBlockQueue                   = NULL;
    ztdBinaryReadDataQueue   * sWriteBadValueBlockQueue                  = NULL;
    ztdBinaryReadDataQueue   * sMyQueue                                  = NULL;
    stlInt32                   sMyQueueIndex                             = 0;

    stlInt32                   sUsedDataCount                            = 0;

    dtlValueBlockList       ** sValueBlockList                           = NULL;

    stlInterval                sAcquireTime                              = ZTD_IO_SLEEP_SEC;
    stlBool                    sIsAcquireTimeOut                         = STL_FALSE;

    stlAllocator               sAllocator;
    stlInt32                   sInsertCommandSize                        = 0;
    stlChar                  * sInsertCommand                            = NULL;
    
    stlChar                    sSqlCommand[ZTD_MAX_COMMAND_BUFFER_SIZE];
    stlChar                    sQuestionComma[]                          = "?,";
    stlChar                    sQuestionBracket[]                        = "?)";

    dtlValueBlockList       ** sSwapValueBlockList                       = NULL;

    stlInt32                   sBufferIndex                              = 0;
    
    stlInt32                   i                                         = 0;
    
    stlInt32                   sState                                    = 1;
    stlUInt32                  sOldCountState                            = ZTD_COUNT_STATE_RELEASE;
    
    stlUInt64                  sMonitorRecordUnit                        = ZTD_MONITOR_RECORD_UNIT;

    ztdFileAndBuffer         * sFileAndBuffer                            = NULL;
    
    stlInt64                   sTempCommitCount                          = 0;
    stlInt64                   sCommitUnit                               = 0;
    
    stlErrorStack            * sErrorStack                               = NULL;
    
    sEnv                         = ( ( ztdBinaryModeReadThreadArg * ) aArg )->mEnv;
    sInputArguments              = ( ( ztdBinaryModeReadThreadArg * ) aArg )->mInputArguments;
    sControlInfo                 = ( ( ztdBinaryModeReadThreadArg * ) aArg )->mControlInfo;
    sValueBlockList              = ( ( ztdBinaryModeReadThreadArg * ) aArg )->mValueBlockList;
    sColumnCount                 = ( ( ztdBinaryModeReadThreadArg * ) aArg )->mColumnCount;
    sExecuteValueBlockQueue      = ( ( ztdBinaryModeReadThreadArg * ) aArg )->mBinaryExecuteQueue;
    sWriteBadValueBlockQueue     = ( ( ztdBinaryModeReadThreadArg * ) aArg )->mBinaryWriteBadQueue;
    sMyQueueIndex                = ( ( ztdBinaryModeReadThreadArg * ) aArg )->mMyQueueIndex;
    sFileAndBuffer               = ( ( ztdBinaryModeReadThreadArg * ) aArg )->mFileAndBuffer;
    sErrorStack                  = &( ( ztdBinaryModeReadThreadArg * ) aArg )->mErrorStack;
    
    sMyQueue                     = &sExecuteValueBlockQueue[sMyQueueIndex];
    sCommitUnit                  = sInputArguments->mCommitUnit;

    STL_TRY( stlCreateRegionAllocator( &sAllocator,
                                       ZTD_REGION_INIT_SIZE,
                                       ZTD_REGION_NEXT_SIZE,
                                       sErrorStack )
             == STL_SUCCESS );
    sState = 2;
    
    /**
     * database에 연결
     */
    STL_TRY( ztdOpenDatabaseThread( sEnv,
                                    &sDbc,
                                    &sStmt,
                                    sInputArguments->mDsn,
                                    sInputArguments->mId,
                                    sInputArguments->mPassword,
                                    sControlInfo->mCharacterSet,
                                    sErrorStack )
             == STL_SUCCESS );
    sState = 3;

    /**
     * read block count를 조정한다.
     */
    stlSnprintf( sSqlCommand,
                 ZTD_MAX_COMMAND_BUFFER_SIZE,
                 "ALTER SESSION SET BLOCK_READ_COUNT = %d",
                 gZtdReadBlockCount );

    STL_TRY( SQLExecDirect( sStmt,
                            (SQLCHAR *)sSqlCommand,
                            SQL_NTS )
             == SQL_SUCCESS );

    STL_TRY( SQLSetStmtAttr( sStmt,
                             SQL_ATTR_ATOMIC_EXECUTION,
                             (SQLPOINTER)SQL_ATOMIC_EXECUTION_ON,
                             0 )
             == SQL_SUCCESS );

    /**
     * Alloc Insert Command Buffer
     */
    sInsertCommandSize  = ZTD_MAX_SCHEMA_NAME + ZTD_MAX_TABLE_NAME + ZTD_DEFAULT_COMMAND_SIZE;
    sInsertCommandSize += gZtdColumnCount * stlStrlen( sQuestionComma );

    STL_TRY( stlAllocRegionMem( &sAllocator,
                                sInsertCommandSize,
                                (void **) &sInsertCommand,
                                sErrorStack )
             == STL_SUCCESS );
    
    stlSnprintf( sInsertCommand,
                 sInsertCommandSize,
                 "INSERT INTO %s.%s VALUES (",
                 sControlInfo->mSchema,
                 sControlInfo->mTable );

    for( i = 0; i < sColumnCount; i++ )
    {
        if( i < ( sColumnCount - 1 ) )
        {
            stlStrncat( sInsertCommand,
                        sQuestionComma,
                        ( stlStrlen( sQuestionComma ) + 1 ) );
        }
        else
        {
            stlStrncat( sInsertCommand,
                        sQuestionBracket,
                        ( stlStrlen( sQuestionBracket ) + 1 ) );
        }
    }

    STL_TRY( SQLPrepare( sStmt,
                         (SQLCHAR *) sInsertCommand,
                         SQL_NTS )
             == SQL_SUCCESS );
    
    /**
     * 자신의 queue를 확인하여 import를 반복적으로 수행한다.
     */
    while( STL_TRUE )
    {
        /**
         * gloader의 비정상 종료를 확인한다.
         */
        STL_TRY( gZtdRunState == STL_TRUE );

        if( (gZtdErrorCount > gZtdMaxErrorCount) &&
            (gZtdMaxErrorCount > 0) )
        {
            break;
        }
        
        /**
         * 일정 시간 queue의 item의 획득을 시도하여, item을 queue에에서 가져온다.
         */
        sIsAcquireTimeOut = STL_FALSE;

        STL_TRY( stlTimedAcquireSemaphore( &sMyQueue->mItem,
                                           sAcquireTime,
                                           &sIsAcquireTimeOut,
                                           sErrorStack )
                 == STL_SUCCESS );
        
        /**
         * 자신의 queue를 확인하여 item이 있다면 import 작업을 수행한다.
         */
        if( sIsAcquireTimeOut == STL_FALSE )
        {
            sBufferIndex = sMyQueue->mGetNumber % ZTD_EXECUTE_QUEUE_SIZE;
            
            sSwapValueBlockList                             = sMyQueue->mBuffer[sBufferIndex].mValueBlockList;
            sMyQueue->mBuffer[sBufferIndex].mValueBlockList = sValueBlockList;
            sValueBlockList                                 = sSwapValueBlockList;

            sUsedDataCount = (*sValueBlockList)->mValueBlock->mUsedBlockCnt;

            sMyQueue->mGetNumber++;

            /**
             * queue에 empty가 추가 되었으므로 release해준다.
             */
            STL_TRY( stlReleaseSemaphore( &sMyQueue->mEmpty,
                                          sErrorStack )
                     == STL_SUCCESS );

            /**
             * 획득한 sValueBlockList의 Used Block Count가 0이면, 작업을 종료한다.
             */
            if( ZTD_GET_USED_BLOCK_COUNT( *sValueBlockList ) == 0 )
            {
                /**
                 * Queue에 Item이 없으므로 작업을 종료한다.
                 */
                STL_THROW( RAMP_FINISH );
            }

            /**
             * Error Count 확인 
             */ 
            if( (gZtdErrorCount > gZtdMaxErrorCount) &&
                (gZtdMaxErrorCount > 0) )
            {
                STL_THROW( RAMP_FINISH );
            }
            
            /**
             * import를 시도한다.
             */
            if( zllExecute4Imp( sStmt,
                                sColumnCount,
                                sValueBlockList )
                == STL_SUCCESS )
            {
                /**
                 * Complete Count를 1 증가 시킨다.
                 */
                while( STL_TRUE )
                {
                    sOldCountState = stlAtomicCas32( (volatile stlUInt32 *) &gZtdCompleteCountState,
                                                     ZTD_COUNT_STATE_ACQUIRE,
                                                     ZTD_COUNT_STATE_RELEASE );

                    if( sOldCountState == ZTD_COUNT_STATE_RELEASE )
                    {
                        gZtdCompleteCount += sUsedDataCount;

                        stlAtomicSet32( (volatile stlUInt32*) &gZtdCompleteCountState,
                                        ZTD_COUNT_STATE_RELEASE );

                        break;
                    }
                }
                
                /**
                 * 모니터링 정보를 출력한다.
                 */
                if( sInputArguments->mSilent == STL_FALSE )
                {
                    while( STL_TRUE )
                    {
                        if( gZtdCompleteCount >= sMonitorRecordUnit )
                        {
                            stlPrintf( "loaded %ld records into %s.%s\n\n",
                                       sMonitorRecordUnit,
                                       sControlInfo->mSchema,
                                       sControlInfo->mTable );

                            sMonitorRecordUnit += ZTD_MONITOR_RECORD_UNIT;
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                else
                {
                    /* Do Nothing */
                }

                /**
                 * Commit
                 */
                sTempCommitCount += sUsedDataCount;
                if( sTempCommitCount >= sCommitUnit )
                {
                    sTempCommitCount = 0;

                    STL_TRY( SQLEndTran( SQL_HANDLE_DBC,
                                         sDbc,
                                         SQL_COMMIT )
                             == SQL_SUCCESS );
                }
            }
            else
            {
                /**
                 * error가 발생한 데이터를 write bad thread 에 전달한다.
                 */
                while( STL_TRUE )
                {
                    STL_TRY( gZtdRunState == STL_TRUE );

                    if( (gZtdErrorCount > gZtdMaxErrorCount) &&
                        (gZtdMaxErrorCount > 0) )
                    {
                        break;
                    }
                    
                    sIsAcquireTimeOut = STL_FALSE;
                    
                    STL_TRY( stlTimedAcquireSemaphore( &sWriteBadValueBlockQueue->mEmpty,
                                                       sAcquireTime,
                                                       &sIsAcquireTimeOut,
                                                       sErrorStack )
                             == STL_SUCCESS );
                    
                    if( sIsAcquireTimeOut == STL_FALSE )
                    {
                        /**
                         * Bad Queue에 Block List를 Push하기 위해서 접근할 수 있는 Thread가 여러 개이기 때문에 동시성 제어가 필요하다.
                         */
                        stlAcquireSpinLock( &sWriteBadValueBlockQueue->mPushLock,
                                            NULL /* Miss count */ );
                        
                        sBufferIndex = sWriteBadValueBlockQueue->mPutNumber % ZTD_WRITE_BAD_QUEUE_SIZE;
                        
                        sSwapValueBlockList                                             = sWriteBadValueBlockQueue->mBuffer[sBufferIndex].mValueBlockList;
                        sWriteBadValueBlockQueue->mBuffer[sBufferIndex].mValueBlockList = sValueBlockList;
                        sValueBlockList                                                 = sSwapValueBlockList;

                        sWriteBadValueBlockQueue->mPutNumber++;

                        /**
                         * Bad Queue의 Push연산에 동시성 제어를 위해서 Acquire한 Spin Lock를 Release한다.
                         */
                        stlReleaseSpinLock( &sWriteBadValueBlockQueue->mPushLock );
                        
                        /**
                         * queue에 item이 한 개 증가 하였으므로 release한다.
                         */
                        STL_TRY( stlReleaseSemaphore( &sWriteBadValueBlockQueue->mItem,
                                                      sErrorStack )
                                 == STL_SUCCESS );

                        break;
                    }
                    else
                    {
                        /* No Nothing */
                    }
                }

                /**
                 * Error Message를 Log File에 출력한다.
                 */
                STL_TRY( ztdWriteDiagnostic( SQL_HANDLE_STMT,
                                             sStmt,
                                             STL_FALSE,  /* Is Printf Record Number */
                                             0,
                                             sFileAndBuffer,
                                             sErrorStack )
                         == STL_SUCCESS );
            } /* send error data. */

            /**
             * 사용된 ValueBlockList의 Used Block Count를 0으로 초기화한다.
             */
            ZTD_SET_USED_BLOCK_COUNT( *sValueBlockList, 0 );
        }
        else
        {
            /* Do Noting */
        }
    }

    STL_RAMP( RAMP_FINISH );

    /**
     * database의 연결 해지
     */
    sState = 2;
    STL_TRY( ztdCloseDatabaseThread( sDbc,
                                     sStmt,
                                     STL_TRUE,  /* COMMIT */
                                     sErrorStack )
             == STL_SUCCESS );

    sState = 1;
    STL_TRY( stlDestroyRegionAllocator( &sAllocator,
                                        sErrorStack )
             == STL_SUCCESS );
    
    sState = 0;
    (void) stlAtomicDec32( &gZtdBinaryModeExecuteRunningCount );
    
    /**
     * 마지막으로 종료되는 ztdExecuteValueBlockList Thread에서 item semaphore를 release해준다.
     */
    if( gZtdBinaryModeExecuteRunningCount == 0 )
    {
        STL_TRY( stlReleaseSemaphore( &sWriteBadValueBlockQueue->mItem,
                                      sErrorStack )
                 == STL_SUCCESS);
    }
    
    STL_TRY( stlExitThread( aThread,
                            STL_SUCCESS,
                            sErrorStack )
             == STL_SUCCESS );
    
    return NULL;

    STL_FINISH;

    gZtdRunState = STL_FALSE;

    switch( sState )
    {
        case 4:
            (void) SQLEndTran( SQL_HANDLE_DBC,
                               sDbc,
                               SQL_ROLLBACK );
        case 3:
            (void) ztdCheckError( sEnv,
                                  sDbc,
                                  sStmt,
                                  0,
                                  sFileAndBuffer,
                                  &gZtdManager.mFileSema,
                                  sErrorStack );
            
            (void) ztdCloseDatabaseThread( sDbc,
                                           sStmt,
                                           STL_FALSE,  /* COMMIT */
                                           sErrorStack );
        case 2:
            (void) stlDestroyRegionAllocator( &sAllocator,
                                              sErrorStack );
        case 1:
            (void) stlAtomicDec32( &gZtdBinaryModeExecuteRunningCount );
        default :
            /**
             * 마지막으로 종료되는 ztdExecuteValueBlockList Thread에서 item semaphore를 release해준다.
             */
            if( gZtdBinaryModeExecuteRunningCount == 0 )
            {
                (void) stlReleaseSemaphore( &sWriteBadValueBlockQueue->mItem,
                                            sErrorStack );
            }

            (void) stlExitThread( aThread,
                                  STL_FAILURE,
                                  sErrorStack );
            break;
    }
    
    return NULL;
}

static dtlCharacterSet ztdCharSetIDFunc( void * aArgs )
{
    return gZtdCharacterSet;
}

static stlChar * ztdGetDateFormatStringFunc( void * aArgs )
{
    return DTL_DATE_FORMAT_FOR_ODBC;
}

dtlDateTimeFormatInfo * ztdGetDateFormatInfoFunc( void * aArgs )
{
    return gDtlNLSDateFormatInfoForODBC;    
}

static stlChar * ztdGetTimeFormatStringFunc( void * aArgs )
{
    return DTL_TIME_FORMAT_FOR_ODBC;    
}

dtlDateTimeFormatInfo * ztdGetTimeFormatInfoFunc( void * aArgs )
{
    return gDtlNLSTimeFormatInfoForODBC;
}

static stlChar * ztdGetTimeWithTimeZoneFormatStringFunc( void * aArgs )
{
    return DTL_TIME_WITH_TIME_ZONE_FORMAT_FOR_ODBC;    
}

dtlDateTimeFormatInfo * ztdGetTimeWithTimeZoneFormatInfoFunc( void * aArgs )
{
    return gDtlNLSTimeWithTimeZoneFormatInfoForODBC;    
}

static stlChar * ztdGetTimestampFormatStringFunc( void * aArgs )
{
    return DTL_TIMESTAMP_FORMAT_FOR_ODBC;    
}

dtlDateTimeFormatInfo * ztdGetTimestampFormatInfoFunc( void * aArgs )
{
    return gDtlNLSTimestampFormatInfoForODBC;    
}

static stlChar * ztdGetTimestampWithTimeZoneFormatStringFunc( void * aArgs )
{
    return DTL_TIMESTAMP_WITH_TIME_ZONE_FORMAT_FOR_ODBC;    
}

dtlDateTimeFormatInfo * ztdGetTimestampWithTimeZoneFormatInfoFunc( void * aArgs )
{
    return gDtlNLSTimestampWithTimeZoneFormatInfoForODBC;    
}

/**
 * @brief error가 발생한 value block list에 대해서 write 작업을 한다.
 * @param [in]     aThread  stlthread.
 * @param [in/out] aArg     thread arguments.
 */
void * STL_THREAD_FUNC ztdWriteBadValueBlockList( stlThread * aThread,
                                                  void      * aArg )
{
    stlInt32                   sColumnCount                     = 0;
    
    ztdBinaryReadDataQueue   * sMyQueue                         = NULL;
    stlInt32                   sBufferIndex                     = 0;

    dtlValueBlockList       ** sValueBlockList                  = NULL;
    dtlValueBlockList        * sBlockList                       = NULL;
    dtlValueBlock            * sValueBlock                      = NULL;
    dtlDataValue             * sDataValue                       = NULL;
    dtlFuncVector              sVector;

    stlChar                    sLineFeed                        = ZTD_CONSTANT_LINE_FEED;
    stlChar                    sDoubleQuote                     = ZTD_CONSTANT_DOUBLEQUOTE;
    stlChar                  * sCommaSpace                      = ZTD_CONSTANT_COMMASPACE;
    
    dtlValueBlockList       ** sSwapValueBlockList              = NULL;

    stlDynamicAllocator        sDynamicAllocator;
    stlInt32                   sUsedBlockCnt                    = 0;

    stlInt32                   sColumnIndex                     = 0;
    stlInt32                   sRecordIndex                     = 0;
    stlInt32                   i                                = 0;

    ztdColumnInfo            * sColumnInfo                      = NULL;
    stlChar                  * sTextFormatData                  = NULL;
    stlInt64                   sTextBufferIndex                 = 0;
    stlInt64                   sColumnTextLength                = 0;
    SQLLEN                     sTextFormatSize                  = 0;
    
    stlInt32                   sState                           = 0;
    
    stlErrorStack            * sErrorStack;

    stlInterval                sAcquireTime                     = ZTD_IO_SLEEP_SEC;
    stlBool                    sIsAcquireTimeOut                = STL_FALSE;

    SQLLEN                     sColumnLength                    = 0;

    ztdWriteBufferedFile       sWriteBufferedFile;
    stlBool                    sIsRealloc = STL_FALSE;

    sValueBlockList              = ( ( ztdBinaryModeReadThreadArg * ) aArg )->mValueBlockList;
    sColumnCount                 = ( ( ztdBinaryModeReadThreadArg * ) aArg )->mColumnCount;
    sMyQueue                     = ( ( ztdBinaryModeReadThreadArg * ) aArg )->mBinaryWriteBadQueue;
    sColumnInfo                  = ( ( ztdBinaryModeReadThreadArg * ) aArg )->mColumnInfo;
    sErrorStack                  = &( ( ztdBinaryModeReadThreadArg * ) aArg )->mErrorStack;

    sVector.mGetCharSetIDFunc = ztdCharSetIDFunc;

    sVector.mGetDateFormatStringFunc = ztdGetDateFormatStringFunc;
    sVector.mGetDateFormatInfoFunc   = ztdGetDateFormatInfoFunc;
    
    sVector.mGetTimeFormatStringFunc = ztdGetTimeFormatStringFunc;
    sVector.mGetTimeFormatInfoFunc   = ztdGetTimeFormatInfoFunc;

    sVector.mGetTimeWithTimeZoneFormatStringFunc = ztdGetTimeWithTimeZoneFormatStringFunc;
    sVector.mGetTimeWithTimeZoneFormatInfoFunc   = ztdGetTimeWithTimeZoneFormatInfoFunc;

    sVector.mGetTimestampFormatStringFunc = ztdGetTimestampFormatStringFunc;
    sVector.mGetTimestampFormatInfoFunc   = ztdGetTimestampFormatInfoFunc;

    sVector.mGetTimestampWithTimeZoneFormatStringFunc = ztdGetTimestampWithTimeZoneFormatStringFunc;
    sVector.mGetTimestampWithTimeZoneFormatInfoFunc   = ztdGetTimestampWithTimeZoneFormatInfoFunc;
        
    STL_TRY( stlCreateDynamicAllocator( &sDynamicAllocator,
                                        ZTD_DYNAMIC_INIT_SIZE,
                                        ZTD_DYNAMIC_NEXT_SIZE,
                                        sErrorStack )
             == STL_SUCCESS );
    sState = 1;

    /**
     * Buffered File을 초기화한다.
     */
    sWriteBufferedFile.mFileDesc = gZtdBadFile;
    
    STL_TRY( stlAllocDynamicMem( &sDynamicAllocator,
                                 ZTD_WRITE_BUFFER_SIZE,
                                 (void **) &sWriteBufferedFile.mBuffer,
                                 sErrorStack )
             == STL_SUCCESS );

    sWriteBufferedFile.mCurrentBufferIndex = 0;
    sWriteBufferedFile.mMaxFileSize        = 0;
    sWriteBufferedFile.mCurrentFileSize    = 0;

    /**
     * Max Display Size를 구한다. (String으로 변환된 Column 데이터를 저장할 Buffer Size)
     */
    for( i = 0; i < sColumnCount; i++ )
    {
        if( (sColumnInfo[i].mDbType == DTL_TYPE_LONGVARCHAR) ||
            (sColumnInfo[i].mDbType == DTL_TYPE_LONGVARBINARY) )
        {
            /**
             * Long Data Type이 포함된 경우 Max Bad Buffer Size로 한다.
             * 실제 Value의 길이가 Buffer Size보다 크다면 재할당해야 한다.
             */
            sTextFormatSize = STL_MAX( sTextFormatSize, ZTD_MAX_BAD_BUFFER_SIZE );
        }
        else
        {
            sTextFormatSize = STL_MAX( sTextFormatSize, sColumnInfo[i].mDisplaySize );
        }
    }

    /**
     *  String Format으로 변경될 경우, 추가적으로 Double Quote 2개 + Comma space 1개의 size 필요.
     */
    sTextFormatSize += ( ZTD_CONSTANT_LENGTH_DOUBLEQUOTE * 2) + ZTD_CONSTANT_LENGTH_COMMASPACE;

    /**
     * Row를 String format으로 변환하는데 필요한 Buffer를 할당받는다. 
     */
    STL_TRY( stlAllocDynamicMem( &sDynamicAllocator,
                                 sTextFormatSize,
                                 (void **) &sTextFormatData,
                                 sErrorStack )
             == STL_SUCCESS );

    stlMemset( (void *) sTextFormatData,
               0x00,
               sTextFormatSize );

    /**
     * 자신의 Queue를 확인하여, Error가 발생한 Value Block List를 Bad File에 쓴다.
     */
    while( STL_TRUE )
    {
        /**
         * gloader의 비정상 종료를 확인한다.
         */
        STL_TRY( gZtdRunState == STL_TRUE );

        if( (gZtdErrorCount > gZtdMaxErrorCount) &&
            (gZtdMaxErrorCount > 0) )
        {
            break;
        }
        
        sIsAcquireTimeOut = STL_FALSE;

        STL_TRY( stlTimedAcquireSemaphore( &sMyQueue->mItem,
                                           sAcquireTime,
                                           &sIsAcquireTimeOut,
                                           sErrorStack )
                 == STL_SUCCESS );
        
        if( sIsAcquireTimeOut == STL_FALSE )
        {
            /**
             * Queue에서 데이터를 가져온다.
             */
            sBufferIndex = sMyQueue->mGetNumber % ZTD_WRITE_BAD_QUEUE_SIZE;
            
            sSwapValueBlockList                             = sMyQueue->mBuffer[sBufferIndex].mValueBlockList;
            sMyQueue->mBuffer[sBufferIndex].mValueBlockList = sValueBlockList;
            sValueBlockList                                 = sSwapValueBlockList;

            sMyQueue->mGetNumber++;

            /**
             * Queue에 Empty가 추가 되었으므로 Release해준다.
             */
            STL_TRY( stlReleaseSemaphore( &sMyQueue->mEmpty,
                                          sErrorStack )
                     == STL_SUCCESS );

            /**
             * Used Block Count가 0이면, 작업을 정상 종료한다.
             */
            sUsedBlockCnt = ZTD_GET_USED_BLOCK_COUNT( *sValueBlockList );
            
            STL_TRY_THROW( sUsedBlockCnt != 0, RAMP_FINISH );
            
            /**
             * 데이터를 string format으로 변환하여 Bad File에 쓴다.
             */
            for( sRecordIndex = 0; sRecordIndex < sUsedBlockCnt; sRecordIndex++ )
            {
                for( sColumnIndex = 0; sColumnIndex < sColumnCount; sColumnIndex++ )
                {
                    sTextBufferIndex = 0;
                    
                    sBlockList  = sValueBlockList[sColumnIndex];
                    sValueBlock = sBlockList->mValueBlock;

                    sDataValue  = ZTD_GET_DATA_VALUE( sValueBlock, sRecordIndex );
                    
                    stlStrncat( sTextFormatData,
                                &sDoubleQuote,
                                (ZTD_CONSTANT_LENGTH_DOUBLEQUOTE + 1) );
                    sTextBufferIndex += ZTD_CONSTANT_LENGTH_DOUBLEQUOTE;

                    /**
                     * Value가 있는 경우만 변환한다.
                     */
                    if( sDataValue->mLength > 0 )
                    {
                        /**
                         * Size가 모자르다면 재할당해야 하는데 Long VarBinary는 MAX PRECISION의
                         * 2배까지 할당할 수 있다.
                         */ 
                        if( sColumnInfo[sColumnIndex].mDbType == DTL_TYPE_LONGVARBINARY )
                        {
                            if( sTextFormatSize < DTL_VARBINARY_DISPLAY_SIZE( sDataValue->mLength ) )
                            {
                                sColumnLength = DTL_VARBINARY_DISPLAY_SIZE( DTL_LONGVARCHAR_MAX_PRECISION );
                                sTextFormatSize = STL_MIN( sDataValue->mLength * 2,
                                                           sColumnLength );
                                sTextFormatSize += ( ZTD_CONSTANT_LENGTH_DOUBLEQUOTE * 2) + ZTD_CONSTANT_LENGTH_COMMASPACE;
                                sIsRealloc = STL_TRUE;
                            }
                        }
                        else
                        {
                            if( sTextFormatSize < sDataValue->mLength )
                            {
                                sTextFormatSize = STL_MIN( sDataValue->mLength * 2,
                                                           DTL_LONGVARCHAR_MAX_PRECISION );
                                sTextFormatSize += ( ZTD_CONSTANT_LENGTH_DOUBLEQUOTE * 2) + ZTD_CONSTANT_LENGTH_COMMASPACE;
                                sIsRealloc = STL_TRUE;
                            }
                        }

                        if( sIsRealloc == STL_TRUE )
                        {
                            STL_TRY( stlFreeDynamicMem( &sDynamicAllocator,
                                                        sTextFormatData,
                                                        sErrorStack ) == STL_SUCCESS );

                            STL_TRY( stlAllocDynamicMem( &sDynamicAllocator,
                                                         sTextFormatSize,
                                                         (void**)&sTextFormatData,
                                                         sErrorStack ) == STL_SUCCESS );
                            sIsRealloc = STL_FALSE;

                            sTextBufferIndex = 0;
                            stlStrncat( sTextFormatData,
                                        &sDoubleQuote,
                                        (ZTD_CONSTANT_LENGTH_DOUBLEQUOTE + 1) );
                            sTextBufferIndex += ZTD_CONSTANT_LENGTH_DOUBLEQUOTE;
                        }

                        STL_TRY( dtlToStringDataValue( sDataValue,
                                                       DTL_GET_BLOCK_ARG_NUM1( sBlockList ),
                                                       DTL_GET_BLOCK_ARG_NUM2( sBlockList ),
                                                       sTextFormatSize,
                                                       (void *)&sTextFormatData[sTextBufferIndex],
                                                       &sColumnTextLength,
                                                       &sVector,
                                                       NULL,
                                                       sErrorStack )
                                 == STL_SUCCESS );
                        sTextBufferIndex += sColumnTextLength;
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                    
                    stlStrncat( sTextFormatData,
                                &sDoubleQuote,
                                (ZTD_CONSTANT_LENGTH_DOUBLEQUOTE + 1) );
                    sTextBufferIndex += ZTD_CONSTANT_LENGTH_DOUBLEQUOTE;
                    
                    stlStrncat( sTextFormatData,
                                sCommaSpace,
                                ZTD_CONSTANT_LENGTH_COMMASPACE );
                    sTextBufferIndex += ZTD_CONSTANT_LENGTH_COMMASPACE;

                    /**
                     * Column의 Data를 Buffered File에 출력한다.
                     */
                    STL_TRY( ztdWriteBadBufferedFile( &sWriteBufferedFile,
                                                      sTextFormatData,
                                                      stlStrlen( sTextFormatData ),
                                                      STL_FALSE,  /* flush */
                                                      sErrorStack )
                             == STL_SUCCESS );

                    stlMemset( sTextFormatData,
                               0x00,
                               sTextFormatSize );
                }

                STL_TRY( ztdWriteBadBufferedFile( &sWriteBufferedFile,
                                                  &sLineFeed,
                                                  ZTD_CONSTANT_LENGTH_LINE_FEED,
                                                  STL_FALSE,  /* flush */
                                                  sErrorStack )
                         == STL_SUCCESS );

                /**
                 * Error Count를 1 증가 시킨다.
                 */
                stlAtomicInc32( &gZtdErrorCount );
            }

            /**
             * 사용된 ValueBlockList의 Used Block Count를 0으로 초기화한다.
             */
            ZTD_SET_USED_BLOCK_COUNT( *sValueBlockList, 0 );
        }
        else
        {
            /* Do Nothing */
        }
    }

    STL_RAMP( RAMP_FINISH );

    /**
     * Buffer에 남아있는 데이터를 Bad File에 출력한다.
     */
    STL_TRY( ztdWriteBadBufferedFile( &sWriteBufferedFile,
                                      NULL,
                                      0,
                                      STL_TRUE,  /* flush */
                                      sErrorStack )
             == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( stlDestroyDynamicAllocator( &sDynamicAllocator,
                                         sErrorStack )
             == STL_SUCCESS );

    STL_TRY( stlExitThread( aThread,
                            STL_SUCCESS,
                            sErrorStack )
             == STL_SUCCESS );
    
    return NULL;

    STL_FINISH;

    gZtdRunState = STL_FALSE;
    
    switch( sState )
    {
        case 1:
            (void) stlDestroyDynamicAllocator( &sDynamicAllocator,
                                               sErrorStack );
        default:            
            (void) stlExitThread( aThread,
                                  STL_FAILURE,
                                  sErrorStack );
            break;
    }

    return NULL;
}
