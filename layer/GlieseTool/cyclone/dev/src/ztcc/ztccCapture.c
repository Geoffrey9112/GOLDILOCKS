/*******************************************************************************
 * ztccCapture.c
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
 * @file ztccCapture.c
 * @brief GlieseTool Cyclone Capture Routines
 */

#include <goldilocks.h>
#include <ztc.h>


extern ztcMasterMgr *   gMasterMgr;
extern stlBool          gRunState;
extern ztcMonitorInfo   gMonitorInfo;

ztcCaptureCallback gCaptureCallback;

ztcCaptureCallback gCallbackForRecovery =
{
    ztccOperateTransRollback,        //Recovery 단계에서는 Commit Transaction은 Rollback 되어야 한다.
    ztccOperateTransRollback,
    ztccCheckRecoveryEnd
};

ztcCaptureCallback gCallbackForNormal =
{
    ztccOperateTransCommit,
    ztccOperateTransRollback,
    ztccDummyFunc
};


void ztccSetCaptureCallback( ztcCaptureCallback * aCaptureCallback )
{
    gCaptureCallback.mTransCommitFunc      = aCaptureCallback->mTransCommitFunc;
    gCaptureCallback.mTransRollbackFunc    = aCaptureCallback->mTransRollbackFunc;
    gCaptureCallback.mCheckRecoveryEndFunc = aCaptureCallback->mCheckRecoveryEndFunc;
}


stlStatus ztccRecoveryCapture( ztcLogFileInfo * aLogFileInfo,
                               stlErrorStack  * aErrorStack )
{
    ztcLogFileInfo  * sLogFileInfo   = NULL;
    ztcCaptureStatus  sCaptureStatus = ZTC_CAPTURE_STATUS_NONE;
    stlInt64          sFileSeqNo     = ZTC_INVALID_FILE_SEQ_NO;
    stlInt16          sGroupId       = ZTC_INVALID_FILE_GROUP_ID;
    stlBool           sIsArchiveMode = ztcmGetArchiveLogMode();
    stlChar           sLogFileName[ STL_MAX_FILE_PATH_LENGTH ] = {0,};
    
    /**
     * Recovery 수행을 위한 Callback을 등록한다.
     */
    ztccSetCaptureCallback( &gCallbackForRecovery );
    
    sLogFileInfo = aLogFileInfo;
    
    STL_TRY( ztcmLogMsg( aErrorStack,
                         "[CAPTURE] Recovery Start.\n" ) == STL_SUCCESS );
    
    while( 1 )
    {
        STL_ASSERT( sLogFileInfo != NULL );
        
        stlMemcpy( sLogFileName, sLogFileInfo->mName, STL_MAX_FILE_PATH_LENGTH );
        
        STL_TRY( ztccGetLogFileSeqNoNGroupId( sLogFileName,
                                              &sFileSeqNo,
                                              &sGroupId,
                                              aErrorStack ) == STL_SUCCESS );
    
        STL_TRY_THROW( sLogFileInfo->mCaptureInfo.mFileSeqNo <= sFileSeqNo, RAMP_ERR_INVALID_LOGFILE );
        
        if( sLogFileInfo->mCaptureInfo.mFileSeqNo != sFileSeqNo )
        {
            STL_TRY_THROW( ztcmGetUserLogFilePathStatus() == STL_FALSE, RAMP_ERR_INVALID_LOGFILE );
            
            /**
             * ArchiveLog에서 읽어와 함
             */
            STL_TRY_THROW( sIsArchiveMode == STL_TRUE, RAMP_ERR_INVALID_LOGFILE );
            
            STL_TRY( ztcmGetArchiveLogFile( sLogFileName,
                                            sLogFileInfo->mCaptureInfo.mFileSeqNo,
                                            aErrorStack ) == STL_SUCCESS );
            
            STL_TRY( ztccValidateArchiveLogFile( sLogFileName,
                                                 sLogFileInfo,
                                                 aErrorStack ) == STL_SUCCESS );
            
            STL_TRY( ztcmLogMsg( aErrorStack,
                                 "[CAPTURE] %s Archive Log File Capture.\n", 
                                 sLogFileName ) == STL_SUCCESS );
        }
        else
        {
            STL_TRY_THROW( sLogFileInfo->mCaptureInfo.mRedoLogGroupId == sGroupId, RAMP_ERR_INVALID_LOGFILE );
        }
        
        sCaptureStatus = ZTC_CAPTURE_STATUS_NONE;
        
        STL_TRY( ztccCaptureLogFile( sLogFileName,
                                     &sLogFileInfo->mCaptureInfo,
                                     sLogFileInfo->mCaptureInfo.mWrapNo,
                                     &sCaptureStatus,
                                     aErrorStack ) == STL_SUCCESS );

        if( sCaptureStatus == ZTC_CAPTURE_STATUS_EOF )
        {
            /**
             * 다음 RedoLogFile을 읽고 계속 Recovery
             */
            STL_TRY( ztcmLogMsg( aErrorStack,
                                 "[CAPTURE] %s file Capture Done.\n", 
                                 sLogFileName ) == STL_SUCCESS );
            
            STL_TRY( ztcmGetNextLogFileInfo( &sLogFileInfo, 
                                             aErrorStack ) == STL_SUCCESS );
            
            STL_TRY( ztcmLogMsg( aErrorStack,
                                 "[CAPTURE] Next %s file Capture.\n", 
                                 sLogFileInfo->mName ) == STL_SUCCESS );
        }
        else if( sCaptureStatus == ZTC_CAPTURE_STATUS_READ_DONE )
        {
            //Recovery Done...
            //Nothing To Do.
            STL_TRY( ztcmLogMsg( aErrorStack,
                                 "[CAPTURE] Recovery Done.\n" ) == STL_SUCCESS );
            break;
        }
        else if( sCaptureStatus == ZTC_CAPTURE_STATUS_INVALID_WRAP_NO )
        {
            /**
             * Recovery 도중에 해당 LogFile이 Reuse 된 경우이며,
             * Archive Log에서 읽도록 해야 한다.
             */ 
            continue;
        }
        else if( sCaptureStatus == ZTC_CAPTURE_STATUS_ERR )
        {
            //Error Handling...
            break;   
        }

    }//end while
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_LOGFILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_LOG_FILE_REUSED,
                      NULL,
                      aErrorStack,
                      sLogFileName );
        /**
         * TODO : Sender에서 Slave에 에러 메시지를 보내주도록 해야 함!!!
         */
    } 
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztccCapture( ztcLogFileInfo * aLogFileInfo,
                       stlErrorStack  * aErrorStack )
{
    ztcLogFileInfo  * sLogFileInfo   = NULL;
    ztcCaptureStatus  sCaptureStatus = ZTC_CAPTURE_STATUS_NONE;
    stlInt64          sFileSeqNo     = ZTC_INVALID_FILE_SEQ_NO;
    stlInt16          sGroupId       = ZTC_INVALID_FILE_GROUP_ID;
    stlBool           sIsArchiveMode = ztcmGetArchiveLogMode();
    stlChar           sLogFileName[ STL_MAX_FILE_PATH_LENGTH ] = {0,};
    stlInt32          sRetryCount    = 0;
    stlInt32          sBefBlockSeq   = 0;
    stlInt64          sSleepTime     = 1000;
    
    /**
     * 정상적인 Capture 수행을 위한 Callback 등록
     */
    ztccSetCaptureCallback( &gCallbackForNormal );

    sLogFileInfo = aLogFileInfo;
    
    while( 1 )
    {
        STL_ASSERT( sLogFileInfo != NULL );
        
        stlMemcpy( sLogFileName, sLogFileInfo->mName, STL_MAX_FILE_PATH_LENGTH );
        
        STL_TRY( ztccGetLogFileSeqNoNGroupId( sLogFileName,
                                              &sFileSeqNo,
                                              &sGroupId,
                                              aErrorStack ) == STL_SUCCESS );
        
        if( sLogFileInfo->mCaptureInfo.mFileSeqNo > sFileSeqNo )
        {
            /**
             * Log Switching 중일 경우에 잠시동안 대기한 후에 처리한다.
             * 약 50초 정도 대기... 이 정도면 되겠지? 무한대기는 아닌것 같음..
             */   
            //stlPrintf("WAIT [%ld][%ld]\n", sLogFileInfo->mCaptureInfo.mFileSeqNo, sFileSeqNo );
            stlSleep( STL_SET_MSEC_TIME( 500 ) );
            sRetryCount++;
            
            if( sRetryCount < 100 )
            {
                continue;
            }
        }

        sRetryCount = 0;
        
        STL_TRY_THROW( sLogFileInfo->mCaptureInfo.mFileSeqNo <= sFileSeqNo, RAMP_ERR_INVALID_LOGFILE );
        
        if( sLogFileInfo->mCaptureInfo.mFileSeqNo != sFileSeqNo )
        {
            STL_TRY_THROW( ztcmGetUserLogFilePathStatus() == STL_FALSE, RAMP_ERR_INVALID_LOGFILE );
            
            /**
             * ArchiveLog에서 읽어와 함
             */
            STL_TRY_THROW( sIsArchiveMode == STL_TRUE, RAMP_ERR_INVALID_LOGFILE );
            
            STL_TRY( ztcmGetArchiveLogFile( sLogFileName,
                                            sLogFileInfo->mCaptureInfo.mFileSeqNo,
                                            aErrorStack ) == STL_SUCCESS );
            
            STL_TRY( ztccValidateArchiveLogFile( sLogFileName,
                                                 sLogFileInfo,
                                                 aErrorStack ) == STL_SUCCESS );
            
            STL_TRY( ztcmLogMsg( aErrorStack,
                                 "[CAPTURE] %s Archive Log File Capture.\n", 
                                 sLogFileName ) == STL_SUCCESS );
            
        }
        else
        {
            STL_TRY_THROW( sLogFileInfo->mCaptureInfo.mRedoLogGroupId == sGroupId, RAMP_ERR_INVALID_LOGFILE );
        }
        
        sCaptureStatus = ZTC_CAPTURE_STATUS_NONE;

        sBefBlockSeq  = sLogFileInfo->mCaptureInfo.mBlockSeq;
        
        STL_TRY( ztccCaptureLogFile( sLogFileName,
                                     &sLogFileInfo->mCaptureInfo,
                                     sLogFileInfo->mCaptureInfo.mWrapNo,
                                     &sCaptureStatus,
                                     aErrorStack ) == STL_SUCCESS );
        
        switch( sCaptureStatus )
        {
            case ZTC_CAPTURE_STATUS_EOF:
            {
                /**
                 * EOF를 리턴하는 경우는 2가지 경우다.
                 * 1. 파일을 끝까지 읽은 경우
                 * 2. 파일을 끝까지 읽지 않았지만, 다음 파일을 쓰고 있을 경우(Chained Log가 발생하지 않도록 하기 위함)
                 */
                STL_TRY( ztcmLogMsg( aErrorStack,
                                 "[CAPTURE] %s file Capture Done.\n", 
                                 sLogFileName ) == STL_SUCCESS );
            
                STL_TRY( ztcmGetNextLogFileInfo( &sLogFileInfo, 
                                                 aErrorStack ) == STL_SUCCESS );
            
                STL_TRY( ztcmLogMsg( aErrorStack,
                                     "[CAPTURE] Next %s file Capture.\n", 
                                     sLogFileInfo->mName ) == STL_SUCCESS );
                break;
            }
            case ZTC_CAPTURE_STATUS_READ_DONE:
            {
                /**
                 * Slave로 Capture에 사용된 Chunk를 보낼 수 있도록 WaitSender List로 옮긴다.
                 */
                if( sBefBlockSeq != sLogFileInfo->mCaptureInfo.mBlockSeq )
                {
                    STL_TRY( ztccPushFirstChunkToWaitReadList( aErrorStack ) == STL_SUCCESS );
                    
                    /**
                     * 현재까지 쓰여진 Log를 모두 읽었을 경우.
                     * 0.2초 대기후에 한번 더 Capture해 본다.
                     */ 
                    sSleepTime = 200;
                }
                else
                {
                    // 1초 대기 후에 다시 읽어본다.
                    sSleepTime = 1000;   
                }
                
                STL_TRY( gRunState == STL_TRUE && 
                         gMasterMgr->mCaptureRestart == STL_FALSE );
                
                stlSleep( STL_SET_MSEC_TIME( sSleepTime ) );
                break;
            }
            case ZTC_CAPTURE_STATUS_INVALID_WRAP_NO:
            {
                /**
                 * Recovery 도중에 해당 LogFile이 Reuse 된 경우이며,
                 * Archive Log에서 읽도록 해야 한다.
                 */ 
                break;   
            }
            case ZTC_CAPTURE_STATUS_ERR:
            case ZTC_CAPTURE_STATUS_NONE:
            {
                /**
                 * Invalid Return Value
                 */
                //TODO Error Handle 
            }
            default:
                break;
        }//end of switch
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_LOGFILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_LOG_FILE_REUSED,
                      NULL,
                      aErrorStack,
                      sLogFileName );
        /**
         * TODO : Sender에서 Slave에 에러 메시지를 보내주도록 해야 함!!!
         */
    }
    STL_FINISH;
    
    return STL_FAILURE;
}

stlInt16 ztccIsValidLog( ztcLogCursor    * aLogCursor,
                         ztcLogBlockHdr  * aLogBlockHdr )
{
    stlInt16 sLogStatus = ZTC_LOG_STATUS_NONE;
    
    if( aLogBlockHdr->mLsn < 0 )
    {
        /**
         * 초기화 되어 있는 LogBlock
         */
        sLogStatus = ZTC_LOG_STATUS_INVALID_LOG;
        STL_THROW( RAMP_RETURN );
    }
    
    if( aLogBlockHdr->mLsn < aLogCursor->mPrevLastLsn )
    {
        /**
         * LogFile이 재사용되었지만 기존의 데이터가 아직 남아있는 경우
         */
        sLogStatus = ZTC_LOG_STATUS_INVALID_LOG;
        STL_THROW( RAMP_RETURN );
    }

    if( ZTC_BLOCK_WRAP_NO( aLogBlockHdr ) != (aLogCursor->mWrapNo) )
    {
        /**
         * LogFile이 재사용되어 해당 LogFile을 더이상 읽을 수 없는 경우 
         */
        sLogStatus = ZTC_LOG_STATUS_INVALID_WRAP_NO;
        STL_THROW( RAMP_RETURN );
    }

    STL_RAMP( RAMP_RETURN );
    
    return sLogStatus;
}

stlStatus ztccCloseLogCursor( ztcLogCursor  * aLogCursor,
                              stlErrorStack * aErrorStack )
{
    if( aLogCursor->mBufferOrg != NULL )
    {
        stlFreeHeap( aLogCursor->mBufferOrg );
    }

    STL_TRY( stlCloseFile( &aLogCursor->mFile,
                           aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztccReadLogBlock( ztcLogCursor  * aLogCursor,
                            stlChar      ** aLogBlock,
                            stlInt32      * aRetType,
                            stlErrorStack * aErrorStack )
{
    stlOffset sOffset;
    stlSize   sReadSize      = 0;
    stlInt32  sBlockCount    = 0;
    stlInt64  sMaxBlockCount = ztcmGetReadLogBlockCount();
    stlInt32  sRetType       = ZTC_CAPTURE_READ_NORMAL;
    
    aLogCursor->mBufferBlockSeq++;

    if( aLogCursor->mBufferBlockSeq >= aLogCursor->mBufferBlockCount )
    {
        if( aLogCursor->mFileBlockSeq >= aLogCursor->mMaxBlockCount )
        {
            sRetType = ZTC_CAPTURE_READ_EOF;
            STL_THROW( RAMP_SUCCESS );
        }
        
        sOffset = ZTC_BLOCK_SEQ_FILE_OFFSET( aLogCursor->mFileBlockSeq,
                                             aLogCursor->mBlockSize );
        STL_TRY( stlSeekFile( &aLogCursor->mFile,
                              STL_FSEEK_SET,
                              &sOffset,
                              aErrorStack ) == STL_SUCCESS );
        
        sBlockCount = STL_MIN( aLogCursor->mMaxBlockCount - aLogCursor->mFileBlockSeq,
                               sMaxBlockCount );

        STL_ASSERT( sBlockCount > 0 );
        
        if( stlReadFile( &aLogCursor->mFile,
                         (void*)aLogCursor->mBuffer,
                         ZTC_DEFAULT_BLOCK_SIZE * sBlockCount,
                         &sReadSize,
                         aErrorStack ) != STL_SUCCESS )
        {
            STL_TRY( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_EOF );
        
            (void)stlPopError( aErrorStack );
            
            sRetType = ZTC_CAPTURE_READ_WRITING;
            
            STL_THROW( RAMP_SUCCESS );
        }
        else
        {
            if( ZTC_DEFAULT_BLOCK_SIZE * sBlockCount != sReadSize )
            {
                sBlockCount = sReadSize / ZTC_DEFAULT_BLOCK_SIZE;
            }
        }
        
        aLogCursor->mFileBlockSeq    += sBlockCount;
        aLogCursor->mBufferBlockCount = sBlockCount;
        aLogCursor->mBufferBlockSeq   = 0;
    }
    
    *aLogBlock = (aLogCursor->mBuffer +
                 (aLogCursor->mBufferBlockSeq * aLogCursor->mBlockSize));

    STL_RAMP( RAMP_SUCCESS );
    
    *aRetType = sRetType;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztccOpenLogCursor( ztcLogCursor  * aLogCursor,
                             stlInt32        aBlockSeq,
                             stlChar       * aFilePath,
                             stlInt32      * aRetType,
                             stlErrorStack * aErrorStack )
{
    stlUInt32        sState         = 0;
    stlInt32         sBlockCount    = 0;
    stlInt32         sRetType       = ZTC_CAPTURE_READ_NORMAL;
    stlInt64         sMaxBlockCount = ztcmGetReadLogBlockCount();
    stlOffset        sOffset        = 0;
    stlSize          sReadSize      = 0;
    ztcLogFileHdr    sLogFileHdr;
    ztcLogBlockHdr   sLogBlockHdr;
    void           * sLogFileHdrBuffer = NULL;
    void           * sAlignedPtr       = NULL;
    
    aLogCursor->mBuffer         = NULL;
    aLogCursor->mBufferOrg      = NULL;
    aLogCursor->mFileBlockSeq   = 0;
    aLogCursor->mBufferBlockSeq = -1;
    
    STL_TRY( stlOpenFile( &aLogCursor->mFile,
                          aFilePath,
                          STL_FOPEN_READ | STL_FOPEN_DIRECT  | STL_FOPEN_LARGEFILE,
                          STL_FPERM_OS_DEFAULT,
                          aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( stlAllocHeap( (void**)&sLogFileHdrBuffer,
                           ZTC_DEFAULT_BLOCK_SIZE * 2,
                           aErrorStack ) == STL_SUCCESS );
    
    sAlignedPtr = (void*)STL_ALIGN( (stlUInt64)sLogFileHdrBuffer , ZTC_DIRECT_IO_ALIGN );
    
    /**
     * LogFile Header 읽기
     */
    STL_TRY( stlReadFile( &aLogCursor->mFile,
                          sAlignedPtr,
                          ZTC_DEFAULT_BLOCK_SIZE,
                          NULL,
                          aErrorStack ) == STL_SUCCESS );
    
    stlMemcpy( (void*)&sLogFileHdr, sAlignedPtr, STL_SIZEOF( ztcLogFileHdr ) );
    
    stlFreeHeap( sLogFileHdrBuffer );
    
    STL_TRY( stlAllocHeap( (void**)&aLogCursor->mBufferOrg,
                           sLogFileHdr.mBlockSize * ( sMaxBlockCount + 1 ),        //BUFFER 크기를 조정해야 함!!!
                           aErrorStack ) == STL_SUCCESS );
    
    aLogCursor->mBuffer = (stlChar*)STL_ALIGN( (stlUInt64)aLogCursor->mBufferOrg, ZTC_DIRECT_IO_ALIGN );
    
    sState = 2;
    
    aLogCursor->mBlockSize     = sLogFileHdr.mBlockSize;
    aLogCursor->mMaxBlockCount = sLogFileHdr.mMaxBlockCount - 1;
    aLogCursor->mPrevLastLsn   = sLogFileHdr.mPrevLastLsn;
    
    /**
     * WrapNo는 처음 위치한 BlockHeader에서 읽어야 한다.
     */
    sOffset = ZTC_BLOCK_SEQ_FILE_OFFSET( 0, sLogFileHdr.mBlockSize );
    
    STL_TRY( stlSeekFile( &aLogCursor->mFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          aErrorStack ) == STL_SUCCESS );
    /**
     * WrapNo 읽기
     */
    if( stlReadFile( &aLogCursor->mFile,
                     (void*)aLogCursor->mBuffer,
                     aLogCursor->mBlockSize,
                     &sReadSize,
                     aErrorStack ) != STL_SUCCESS )
    {
        STL_TRY( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_EOF );
        
        (void)stlPopError( aErrorStack );
        
        sRetType = ZTC_CAPTURE_READ_WRITING;
        
        STL_THROW( RAMP_SUCCESS );
    }
    else
    {
        if( aLogCursor->mBlockSize != sReadSize )
        {
            sRetType = ZTC_CAPTURE_READ_WRITING;
        
            STL_THROW( RAMP_SUCCESS );
        }
    }
    
    ZTC_READ_LOG_BLOCK_HDR( &sLogBlockHdr, aLogCursor->mBuffer );
    
    aLogCursor->mWrapNo = ZTC_BLOCK_WRAP_NO( &sLogBlockHdr );
    
    /**
     * 처음 읽을 때는 1 Mbytes만 읽도록 한다.
     * MaxBlockCount가 1Mbytes보다 작을 수 있으므로 한번 더 작은 값을 선택하도록 한다.
     */
    sBlockCount = STL_MIN( aLogCursor->mMaxBlockCount - aBlockSeq,
                           ZTC_READ_BUFFER_BLOCK_MIN_COUNT );
    
    sBlockCount = STL_MIN( sBlockCount, sMaxBlockCount );
    
    sOffset = ZTC_BLOCK_SEQ_FILE_OFFSET( aBlockSeq, sLogFileHdr.mBlockSize );  //(stlOffset)(aBlockSeq * sLogFileHdr.mBlockSize);
    
    STL_TRY( stlSeekFile( &aLogCursor->mFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          aErrorStack ) == STL_SUCCESS );

    if( stlReadFile( &aLogCursor->mFile,
                     (void*)aLogCursor->mBuffer,
                     aLogCursor->mBlockSize * sBlockCount,
                     &sReadSize,
                     aErrorStack ) != STL_SUCCESS )
    {
        STL_TRY( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_EOF );
        
        (void)stlPopError( aErrorStack );
        
        sRetType = ZTC_CAPTURE_READ_WRITING;
        
        STL_THROW( RAMP_SUCCESS );
    }
    else
    {
        /**
         * 읽어들인 Data량이 작다면 BlockSize를 재조정 한다.
         */
        if( aLogCursor->mBlockSize * sBlockCount != sReadSize )
        {
            sBlockCount = sReadSize / aLogCursor->mBlockSize;
        }
    }
    
    aLogCursor->mFileBlockSeq     = sBlockCount + aBlockSeq;  //sBlockCount + aBlockSeq - 1;    //Buffer에 저장된 Block Sequence (File 에서의 절대 위치 - 1(LogFileHeader)) 
    aLogCursor->mBufferBlockCount = sBlockCount;              //Buffer에 저장된 Block의 개수
    
    STL_RAMP( RAMP_SUCCESS );
    
    *aRetType = sRetType;
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)stlFreeHeap( aLogCursor->mBufferOrg );
        case 1:
            (void)stlCloseFile( &aLogCursor->mFile, aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}


stlStatus ztccGetLogFileWrapNo( stlChar       * aFileName,
                                stlInt16      * aWrapNo,
                                stlErrorStack * aErrorStack )
{
    stlFile        sFile;
    stlInt32       sState    = 0;
    stlOffset      sOffset   = 0;
    
    ztcLogFileHdr    sLogFileHdr;
    ztcLogBlockHdr   sLogBlockHdr;
    stlChar          sBuffer[ZTC_DEFAULT_BLOCK_SIZE];
    stlChar        * sBlock = sBuffer;
    
    STL_TRY( stlOpenFile( &sFile,
                          aFileName,
                          STL_FOPEN_READ,
                          STL_FPERM_OS_DEFAULT,
                          aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( stlReadFile( &sFile,
                          &sLogFileHdr,
                          STL_SIZEOF( ztcLogFileHdr ),
                          NULL,
                          aErrorStack ) == STL_SUCCESS );
    
    sOffset = ZTC_BLOCK_SEQ_FILE_OFFSET( 0, sLogFileHdr.mBlockSize );
    
    STL_TRY( stlSeekFile( &sFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlReadFile( &sFile,
                          sBlock,
                          ZTC_DEFAULT_BLOCK_SIZE,
                          NULL, 
                          aErrorStack ) == STL_SUCCESS );

    ZTC_READ_LOG_BLOCK_HDR( &sLogBlockHdr, sBlock );

    sState = 0;
    STL_TRY( stlCloseFile( &sFile, 
                           aErrorStack ) == STL_SUCCESS );
    
    *aWrapNo = ZTC_BLOCK_WRAP_NO( &sLogBlockHdr );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    switch( sState )
    {
        case 1:
        (void) stlCloseFile( &sFile, 
                             aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}

stlStatus ztccGetLogFileSeqNoNGroupId( stlChar       * aFileName,
                                       stlInt64      * aFileSeqNo,
                                       stlInt16      * aGroupId,
                                       stlErrorStack * aErrorStack )
{
    stlFile        sFile;
    stlInt32       sState  = 0;
    stlInt32       sRetry  = 0;
    stlBool        sExist  = STL_FALSE;
    ztcLogFileHdr  sLogFileHdr;
    
    void         * sLogFileHdrBuffer = NULL;
    void         * sAlignedPtr       = NULL;
    
    STL_RAMP( RAMP_RETRY );
    
    STL_TRY( stlExistFile( aFileName,
                           &sExist,
                           aErrorStack ) == STL_SUCCESS );
    
    if( sExist == STL_FALSE )
    {
        if( ztcmGetUserLogFilePathStatus() == STL_TRUE )
        {
            /**
             * LogMirror와 연동시를 위하여 파일이 없을 경우에 10초 정도 대기해 본다.
             */
            if( sRetry++ < 20 )
            {
                stlSleep( STL_SET_MSEC_TIME( 500 ) );
                STL_THROW( RAMP_RETRY );
            }
        }
        
        STL_THROW( RAMP_ERR_READ_TIMEOUT );
    }
    
    STL_TRY( stlOpenFile( &sFile,
                          aFileName,
                          STL_FOPEN_READ | STL_FOPEN_DIRECT | STL_FOPEN_LARGEFILE,
                          STL_FPERM_OS_DEFAULT,
                          aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( stlAllocHeap( (void**)&sLogFileHdrBuffer,
                           ZTC_DEFAULT_BLOCK_SIZE * 2,
                           aErrorStack ) == STL_SUCCESS );
    
    sAlignedPtr = (void*)STL_ALIGN( (stlUInt64)sLogFileHdrBuffer , ZTC_DIRECT_IO_ALIGN );
    
    STL_TRY( stlReadFile( &sFile,
                          sAlignedPtr,
                          ZTC_DEFAULT_BLOCK_SIZE,
                          NULL,
                          aErrorStack ) == STL_SUCCESS );
    
    stlMemcpy( (void*)&sLogFileHdr, sAlignedPtr, STL_SIZEOF( ztcLogFileHdr ) );
    
    stlFreeHeap( sLogFileHdrBuffer );
    
    sState = 0;
    STL_TRY( stlCloseFile( &sFile, 
                           aErrorStack ) == STL_SUCCESS );
    
    *aFileSeqNo = sLogFileHdr.mLogFileSeqNo;
    *aGroupId   = sLogFileHdr.mLogGroupId;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_READ_TIMEOUT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_REDOLOG_READ_TIMEOUT,
                      NULL,
                      aErrorStack,
                      aFileName );
    }
    STL_FINISH;
    
    switch( sState )
    {
        case 1:
        (void) stlCloseFile( &sFile, 
                             aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztccCaptureLogFile( stlChar          * aFilePath,
                              ztcCaptureInfo   * aCaptureInfo,
                              stlInt16           aWrapNo,
                              ztcCaptureStatus * aCaptureStatus,
                              stlErrorStack    * aErrorStack )
{
    stlInt32          sState = 0;
    ztcLogHdr         sLogHdr;
    ztcLogPieceHdr    sLogPieceHdr;
    ztcLogBlockHdr    sLogBlockHdr;
    ztcLogCursor      sLogCursor;
    stlChar         * sLogBlock;
    stlChar         * sLogPieceBody;
    stlChar         * sLogBody = NULL;
    stlChar         * sPtr     = NULL;
    stlUInt32         sLogBodyOffset;
    stlInt32          sIdx;
    stlInt32          sBlockOffset;
    stlInt32          sLeftLogCount;
    stlInt32          sTotalReadBytes;
    stlInt32          sReadBytes;
    stlInt32          sReadBlockCount   = 0;            //해당 함수가 실행되는 동안 읽은 Log Block Count
    stlInt32          sPartialLogReadBlockCount = 0;    //Partial Log를 읽기 위해 읽은 Log Block Count
    stlInt32          sLogNoInBlock     = 0;            //한개의 LogBlock 안에서 Log 번호.. SKIP하기 위해서 사용된다.(다음 로그를 읽기 위해 필요)
    stlInt32          sLogSequence      = 0;            //한개의 LogHeader에 포함된 Log들의 순서.. Partial Log라 하더라도 한번은 증가해야 한다.(현재 로그를 다시 읽을때 필요 )
    stlInt64          sTablePID;
    stlInt64          sRecordID;
    stlBool           sFound     = STL_FALSE;
    stlBool           sStatus    = STL_FALSE;
    stlBool           sPropagate = STL_FALSE;
    stlInt64          sTransId;
    stlInt32          sRetType;
    stlUInt64         sCommitSCN;

    stlInt32          sReadLogNo = aCaptureInfo->mReadLogNo;        //읽어야할 LogNo.. 해당 LogNo보다 작을 경우에는 Skip해야 한다.
    stlInt32          sBlockSeq  = aCaptureInfo->mBlockSeq;
    
    ztcCaptureStatus      sCaptureStatus = ZTC_CAPTURE_STATUS_NONE;
    ztcCaptureTableInfo * sTableInfo     = NULL;
    ztcCaptureProgress    sProgress;
    
    /**
     * Set LogBody Buffer
     */
    sLogBody = gMasterMgr->mCaptureMgr.mLogBodyBuffer;

    /**
     * LogFile Header를 읽고 특정 위치로 Offset을 이동한다.
     */
    STL_TRY( ztccOpenLogCursor( &sLogCursor,
                                sBlockSeq,
                                aFilePath,
                                &sRetType,
                                aErrorStack ) == STL_SUCCESS );
    
    sState                    = 1;
    sReadBlockCount           = 0;
    sProgress.mWrapNo         = sLogCursor.mWrapNo;
    sProgress.mRedoLogGroupId = aCaptureInfo->mRedoLogGroupId;
    sProgress.mFileSeqNo      = aCaptureInfo->mFileSeqNo;
    
    /**
     * OpenCursor 시에 EOF는 현재 파일이 쓰여지고 있음을 얘기하므로,
     * 기다렸다가 다시 시도해야 한다.
     */
    if( sRetType == ZTC_CAPTURE_READ_WRITING )
    {
        sCaptureStatus = ZTC_CAPTURE_STATUS_READ_DONE;
        STL_THROW( RAMP_SUCCESS );
    }
    
    if( sLogCursor.mWrapNo != aWrapNo )
    {
        sCaptureStatus = ZTC_CAPTURE_STATUS_INVALID_WRAP_NO;
        STL_THROW( RAMP_SUCCESS );
    }
    
    while( 1 )
    {
        sLogNoInBlock   = 0;
        sBlockOffset    = 0;
        sLogSequence    = 0;
        
        STL_TRY( ztccReadLogBlock( &sLogCursor,
                                   &sLogBlock,
                                   &sRetType,
                                   aErrorStack ) == STL_SUCCESS );
        
        if( sRetType == ZTC_CAPTURE_READ_EOF )
        {
            /**
             * 다음 파일을 읽도록 설정
             */
            sCaptureStatus = ZTC_CAPTURE_STATUS_EOF;
            STL_THROW( RAMP_SUCCESS );
        }
        else if( sRetType == ZTC_CAPTURE_READ_WRITING )
        {
            /**
             * File을 쓰고 있는 중이므로 대기해야 한다.
             */
            sCaptureStatus = ZTC_CAPTURE_STATUS_READ_DONE;
            STL_THROW( RAMP_SUCCESS );
        }

        ZTC_READ_MOVE_LOG_BLOCK_HDR( &sLogBlockHdr, sLogBlock, sBlockOffset );
        
        if( ZTC_IS_DUMMY_FILE_END_BLOCK( &sLogBlockHdr ) == STL_TRUE )
        {
            /**
             * 다음 파일을 읽도록 설정
             * DummyFileEnd가 TRUE인 경우에는 LogBlock을 읽을 필요가 없다.
             */
            sCaptureStatus = ZTC_CAPTURE_STATUS_EOF;
            STL_THROW( RAMP_SUCCESS );
        }

        /**
         * Old(Garbage) Log를 만날 경우 경우에 여기서 빠져나간다.
         */
        switch( ztccIsValidLog( &sLogCursor, &sLogBlockHdr ) )
        {
            case ZTC_LOG_STATUS_INVALID_LOG:
            {
                /**
                 * 추가된 Redo Log가 없을 경우에는 다시 해당 LogNo부터 읽을 수 있도록 세팅해야 한다.
                 */
                sLogNoInBlock  = sReadLogNo;
                sCaptureStatus = ZTC_CAPTURE_STATUS_READ_DONE;
                STL_THROW( RAMP_SUCCESS );
                break;
            }
            case ZTC_LOG_STATUS_INVALID_WRAP_NO:
            {
                /**
                 * Wrap No가 다를 경우, 더 이상 로그를 읽으면 안된다.
                 */
                sLogNoInBlock  = sReadLogNo;
                sCaptureStatus = ZTC_CAPTURE_STATUS_INVALID_WRAP_NO;
                STL_THROW( RAMP_SUCCESS );
                break;
            }
            case ZTC_LOG_STATUS_NONE:
            {
                /**
                 * 정상적인 경우
                 * Nothing To Do.
                 */
                break;
            }
            case ZTC_LOG_STATUS_MAX:
            default:
                break;
        }
        
        sReadBlockCount++;
        
        /**
         * CAPTURE의 모니터링 정보를 위해서 중간에 Update 하도록 한다.
         */
        gMonitorInfo.mFileSeqNo = aCaptureInfo->mFileSeqNo;
        gMonitorInfo.mBlockSeq  = sBlockSeq + sReadBlockCount - sPartialLogReadBlockCount;
        
        sBlockOffset  = sLogBlockHdr.mFirstLogOffset;
        sLeftLogCount = sLogBlockHdr.mLogCount;
        
        /** 
         * LogBlock에 포함되어 있는 Logs를 읽는다.
         */
        while( sLeftLogCount > 0 )
        {
            /**
             * Important...
             * Capture를 진행하는 중에라도 Master 종료 또는 Slave 접속 종료가 발생할 경우에는
             * 더이상 Capture를 진행하지 말아야 한다.
             * P.S. Capture가 종료되지 않은 시점에서 Slave가 재접속하게 되면 잘못된 CAPTURE데이터를 전송할 수 있고, 
             *      SLAVE의 비정상종료가 발생할 수 있다.!!!
             */
            STL_TRY( gRunState == STL_TRUE && 
                     gMasterMgr->mCaptureRestart == STL_FALSE );
            
            sFound = STL_FALSE;
            sLogNoInBlock++;
            sLeftLogCount--;
            sLogSequence++;
            
            ZTC_READ_LOG_HDR( &sLogHdr, sLogBlock + sBlockOffset );
            sBlockOffset += STL_SIZEOF( ztcLogHdr );
            
            if( sLogHdr.mSize > (sLogCursor.mBlockSize - sBlockOffset) )
            {
                sReadBytes = sLogCursor.mBlockSize - sBlockOffset;
            }
            else
            {
                sReadBytes = sLogHdr.mSize;
            }
            
            /**
             * LogBlock안에 있는 Log중에 Partialed Log 이전의 Log는 
             * 이미 처리되어 있으므로 Skip 해야 한다.
             */
            if( sLogNoInBlock < sReadLogNo )
            {
                sTotalReadBytes = sReadBytes;
                sBlockOffset   += sReadBytes;
                
                /**
                 * Gliese가 Recovery하는 과정에서 Partial Log를 Clear하는 경우가 있다. 
                 * 이 경우에는, LogBlockHeader의 LogCount가 변경된다.
                 * 현재 Block을 다 읽었을 경우에는 다음 Block을 읽고 Skip하지 않도록 해야한다.
                 */
                if( sLeftLogCount == 0 )
                {
                    sReadLogNo = 1;   
                }
                
                continue;   
            }
            
            stlMemcpy( sLogBody, sLogBlock + sBlockOffset, sReadBytes );
            sTotalReadBytes = sReadBytes;
            sBlockOffset   += sReadBytes;
            
            /**
             * 해당 Table이 Replicate 되어야 하는지 확인
             */
            
            sTablePID = sLogHdr.mSegRid;
            
            STL_TRY( ztcmFindCaptureTableInfo( &sTableInfo,
                                               sTablePID,
                                               aErrorStack ) == STL_SUCCESS );
           
            /**
             * Replicated 될 Table 관련 Log
             */
            if( sTableInfo != NULL )
            {
                sFound = STL_TRUE;
            }
            else
            {
                sFound = STL_FALSE;
            }
            
            /**
             * PartialLog를 읽었을 때는 새로운 LogBlock을 읽었기 때문에 LogSequence를 초기화한다.
             */
            if( sPartialLogReadBlockCount > 0 )
            {
                sLogSequence = 1;
            }

            /**
             * Log Body를 구성한다.
             * 여러개의 LogBlock에 걸쳐 LogBody가 있을 경우에 Block Block을 이동하며 LogBody를 만들어준다.
             * 즉, LogHeader의 LogBody Size가 하나의 LogBlock에 못들어가 여러개의 LogBlock에 걸쳐서 읽을 경우이다.
             */
            sPartialLogReadBlockCount = 0;
            
            while( sTotalReadBytes < sLogHdr.mSize )
            {
                sBlockOffset = 0;
                
                STL_TRY( ztccReadLogBlock( &sLogCursor,
                                           &sLogBlock,
                                           &sRetType,
                                           aErrorStack ) == STL_SUCCESS );
                
                if( sRetType == ZTC_CAPTURE_READ_EOF )
                {
                    /**
                     * Partial Log는 File에 나눠져서 저장되지 않는다.
                     * 이부분은 발생하지 않을 것임!! 즉, Chained Log는 없다!!!!
                     */
                    sCaptureStatus = ZTC_CAPTURE_STATUS_ERR; //or ASSERT !!
                    STL_THROW( RAMP_SUCCESS );
                }
                else if( sRetType == ZTC_CAPTURE_READ_WRITING )
                {
                    /**
                     * File을 쓰고 있는 중이므로 대기해야 한다.
                     */
                    sPartialLogReadBlockCount++;
                    sCaptureStatus = ZTC_CAPTURE_STATUS_READ_DONE;
                    STL_THROW( RAMP_SUCCESS );
                }

                if( ZTC_IS_DUMMY_FILE_END_BLOCK( &sLogBlockHdr ) == STL_TRUE )
                {
                    /**
                     * Partial Log는 File에 나눠져서 저장되지 않는다.
                     * 이부분은 발생하지 않을 것임!! 즉, Chained Log는 없다!!!!
                     */
                    sCaptureStatus = ZTC_CAPTURE_STATUS_ERR; //or ASSERT !!
                    STL_THROW( RAMP_SUCCESS );
                }

                ZTC_READ_MOVE_LOG_BLOCK_HDR( &sLogBlockHdr, sLogBlock, sBlockOffset );
                
                /**
                 * Old(Garbage) Log를 만날 경우 경우에 여기서 빠져나간다.
                 */
                switch( ztccIsValidLog( &sLogCursor, &sLogBlockHdr ) )
                {
                    case ZTC_LOG_STATUS_INVALID_LOG:
                    {
                        /**
                         * 추가된 Redo Log가 없을 경우에는 다시 해당 LogNo부터 읽을 수 있도록 세팅해야 한다.
                         */
                        sPartialLogReadBlockCount++;
                        sCaptureStatus = ZTC_CAPTURE_STATUS_READ_DONE;
                        STL_THROW( RAMP_SUCCESS );
                        break;
                    }
                    case ZTC_LOG_STATUS_INVALID_WRAP_NO:
                    {
                        /**
                         * Wrap No가 다를 경우, 더 이상 로그를 읽으면 안된다.
                         */
                        sPartialLogReadBlockCount++;
                        sCaptureStatus = ZTC_CAPTURE_STATUS_INVALID_WRAP_NO;
                        STL_THROW( RAMP_SUCCESS );
                        break;
                    }
                    case ZTC_LOG_STATUS_NONE:
                    {
                        /**
                         * 정상적인 경우
                         * Nothing To Do.
                         */
                        break;
                    }
                    case ZTC_LOG_STATUS_MAX:
                    default:
                        break;
                }
                
                sReadBlockCount++;
                sPartialLogReadBlockCount++;
                
                if( (sLogHdr.mSize - sTotalReadBytes) > (sLogCursor.mBlockSize - sBlockOffset) )
                {
                    sReadBytes = sLogCursor.mBlockSize - sBlockOffset;
                }
                else
                {
                    sReadBytes = (sLogHdr.mSize - sTotalReadBytes);
                }
        
                stlMemcpy( sLogBody + sTotalReadBytes, sLogBlock + sBlockOffset, sReadBytes );
                
                sTotalReadBytes += sReadBytes;
                sBlockOffset    += sReadBytes;
                sLeftLogCount    = sLogBlockHdr.mLogCount;
                
            }
            
            /**
             * Partial Log를 읽었을 경우에는 해당 Block에서 LogNo는 0이 된다.
             */
            if( sPartialLogReadBlockCount > 0 )
            {
                /**
                 * LogNoInBlock이 1일 경우에는 Block안에 첫번째 Log를 얘기한다.
                 * LogNoInBlock이 0 이라는 것은 Partial Log의 경우에 새로 읽은 LogBlock 앞부분에는 전 로그의 내용이 마저 쓰여졌다는 것이다. 
                 * 즉, 1이 아니다. Partial Log일 경우에는 LogBlock을 새로 읽었기 때문에 0이 되어야 한다.
                 */
                sLogNoInBlock    = 0;                
            }
            
            /**
             * Valid 한 Log이므로 Skip하지 않도록 1로 세팅한다.
             */
            sReadLogNo     = 1;
            sLogBodyOffset = 0;
            
            /**
             * Log Body를 분석한다.
             */
            for( sIdx = 0; sIdx < sLogHdr.mLogPieceCount; sIdx++ )
            {
                ZTC_READ_MOVE_LOGPIECE_HDR( &sLogPieceHdr,
                                            sLogBody,
                                            sLogBodyOffset );
                
                sLogPieceBody = sLogBody + sLogBodyOffset;
                
                sRecordID = *(stlInt64*)&sLogPieceHdr.mDataRid;
                
                if( sFound == STL_TRUE )
                {
                    /**
                     * DDL이 수행되었고, COMMIT이 되어있지 않은 TABLE만 CAPTURE한다.
                     * 또는 해당 Table의 Pk가 Update된 경우.
                     */
                    if( sTableInfo->mGiveUpCount > 0 )
                    {
                        /**
                         * 해당 테이블에 DDL이 수행되었을 경우에는 
                         * DDL이 ROLLBACK될 수 있으므로 DDL만 처리하도록 해야 한다.
                         */
                        if( sLogPieceHdr.mComponentClass == ZTC_COMPONENT_EXTERNAL )
                        {
                            switch( sLogPieceHdr.mType )
                            {
                                case ZTC_SUPPL_LOG_DDL:
                                {
                                    STL_TRY( ztccOperateDDL( sLogPieceBody,
                                                             sLogHdr.mLsn,
                                                             sRecordID,
                                                             sLogHdr.mTransId,
                                                             sTableInfo,
                                                             sLogPieceHdr.mSize,
                                                             aErrorStack ) == STL_SUCCESS );
                                    break;
                                }
                                case ZTC_SUPPL_LOG_DDL_CLR:
                                {
                                    /** Compensation Log For DDL */    
                                    STL_TRY( ztccOperateDDLCLR( sLogPieceBody,
                                                                sLogHdr.mTransId,
                                                                sTableInfo,
                                                                sLogPieceHdr.mSize,
                                                                aErrorStack ) == STL_SUCCESS );
                                    break;
                                }
                                default:
                                    break;
                            }
                        }
                        sLogBodyOffset += sLogPieceHdr.mSize;
                        continue;
                    }
                }

                /**
                 * Supplemental Logging 처리
                 */
                if( sLogPieceHdr.mComponentClass == ZTC_COMPONENT_EXTERNAL && 
                    sFound == STL_TRUE )
                {
                
                    switch( sLogPieceHdr.mType )
                    {
                        case ZTC_SUPPL_LOG_DELETE:
                        {
                            STL_TRY( ztccOperateTransaction( sLogPieceBody,
                                                             sLogHdr.mLsn,
                                                             sRecordID,
                                                             sLogHdr.mTransId,
                                                             sTableInfo,
                                                             ZTC_CAPTURE_TYPE_SUPPL_DEL,
                                                             sLogPieceHdr.mSize,
                                                             aErrorStack ) == STL_SUCCESS );
                            break;
                        }
                        case ZTC_SUPPL_LOG_UPDATE:
                        {
                            /**
                             * PK가 Update되었는지를 확인한다.
                             */
                            ztccCheckPrimaryKeyUpdated( sTableInfo,
                                                        sLogHdr.mTransId,
                                                        sLogPieceBody );
                            
                            STL_TRY( ztccOperateTransaction( sLogPieceBody,
                                                             sLogHdr.mLsn,
                                                             sRecordID,
                                                             sLogHdr.mTransId,
                                                             sTableInfo,
                                                             ZTC_CAPTURE_TYPE_SUPPL_UPD,
                                                             sLogPieceHdr.mSize,
                                                             aErrorStack ) == STL_SUCCESS );
                            break;
                        }
                        case ZTC_SUPPL_LOG_UPDATE_COLS:
                        {
                            STL_TRY( ztccOperateTransaction( sLogPieceBody,
                                                             sLogHdr.mLsn,
                                                             sRecordID,
                                                             sLogHdr.mTransId,
                                                             sTableInfo,
                                                             ZTC_CAPTURE_TYPE_SUPPL_UPD_COLS,
                                                             sLogPieceHdr.mSize,
                                                             aErrorStack ) == STL_SUCCESS );
                            break;
                        }
                        case ZTC_SUPPL_LOG_UPDATE_BEFORE_COLS:
                        {
                            STL_TRY( ztccOperateTransaction( sLogPieceBody,
                                                             sLogHdr.mLsn,
                                                             sRecordID,
                                                             sLogHdr.mTransId,
                                                             sTableInfo,
                                                             ZTC_CAPTURE_TYPE_SUPPL_UPD_BEF_COLS,
                                                             sLogPieceHdr.mSize,
                                                             aErrorStack ) == STL_SUCCESS );
                            break;
                        }
                        case ZTC_SUPPL_LOG_DDL:
                        {
                            STL_TRY( ztccOperateDDL( sLogPieceBody,
                                                     sLogHdr.mLsn,
                                                     sRecordID,
                                                     sLogHdr.mTransId,
                                                     sTableInfo,
                                                     sLogPieceHdr.mSize,
                                                     aErrorStack ) == STL_SUCCESS );
                            break;
                        }
                        case ZTC_SUPPL_LOG_DDL_CLR:
                        {
                            /** Compensation Log For DDL */    
                            STL_TRY( ztccOperateDDLCLR( sLogPieceBody,
                                                        sLogHdr.mTransId,
                                                        sTableInfo,
                                                        sLogPieceHdr.mSize,
                                                        aErrorStack ) == STL_SUCCESS );
                            break;
                        }
                        default:
                            break;  
                    } //end of switch
                }   //end of if
                
                
                /**
                 * INSERT / UPDATE / DELETE log 처리
                 */
                if( sLogPieceHdr.mComponentClass == ZTC_COMPONENT_TABLE && 
                    sFound == STL_TRUE )
                {
                    
                    switch( sLogPieceHdr.mType )
                    {
                        case ZTC_LOG_MEMORY_HEAP_INSERT:
                        {
                            STL_TRY( ztccOperateTransaction( sLogPieceBody,
                                                             sLogHdr.mLsn,
                                                             sRecordID,
                                                             sLogHdr.mTransId,
                                                             sTableInfo,
                                                             ZTC_CAPTURE_TYPE_INSERT,
                                                             sLogPieceHdr.mSize,
                                                             aErrorStack ) == STL_SUCCESS );
                            break;
                        }
                        case ZTC_LOG_MEMORY_HEAP_UNINSERT:
                        {
                            STL_TRY( ztccOperateTransUndo( sRecordID,
                                                           sLogHdr.mTransId,
                                                           ZTC_CAPTURE_TYPE_UNINSERT,
                                                           aErrorStack ) == STL_SUCCESS );
                            break;
                        }
                        case ZTC_LOG_MEMORY_HEAP_DELETE:
                        {
                            /**
                             * Supplemental Logging으로 Delete가 처리되기는 하나, 
                             * Undo 처리를 위하여 DELETE Log를 저장하고 있어야 한다.
                             * Data는 저장할 필요없음...
                             */
                            STL_TRY( ztccOperateTransaction( NULL,
                                                             sLogHdr.mLsn,
                                                             sRecordID,
                                                             sLogHdr.mTransId,
                                                             sTableInfo,
                                                             ZTC_CAPTURE_TYPE_DELETE,
                                                             0,
                                                             aErrorStack ) == STL_SUCCESS );
                            break;
                        }
                        case ZTC_LOG_MEMORY_HEAP_UNDELETE:
                        {
                            STL_TRY( ztccOperateTransUndo( sRecordID,
                                                           sLogHdr.mTransId,
                                                           ZTC_CAPTURE_TYPE_UNDELETE,
                                                           aErrorStack ) == STL_SUCCESS );
                            break;
                        }
                        case ZTC_LOG_MEMORY_HEAP_UPDATE:
                        {
                            STL_TRY( ztccOperateTransaction( sLogPieceBody,
                                                             sLogHdr.mLsn,
                                                             sRecordID,
                                                             sLogHdr.mTransId,
                                                             sTableInfo,
                                                             ZTC_CAPTURE_TYPE_UPDATE,
                                                             sLogPieceHdr.mSize,
                                                             aErrorStack ) == STL_SUCCESS );
                            break;
                        }
                        case ZTC_LOG_MEMORY_HEAP_UNUPDATE:
                        {
                            STL_TRY( ztccOperateTransUndo( sRecordID,
                                                           sLogHdr.mTransId,
                                                           ZTC_CAPTURE_TYPE_UNUPDATE,
                                                           aErrorStack ) == STL_SUCCESS );
                            break;
                        }
                        case ZTC_LOG_MEMORY_HEAP_INSERT_FOR_UPDATE:
                        {
                            STL_TRY( ztccOperateTransaction( sLogPieceBody,
                                                             sLogHdr.mLsn,
                                                             sRecordID,
                                                             sLogHdr.mTransId,
                                                             sTableInfo,
                                                             ZTC_CAPTURE_TYPE_INSERT_FOR_UPDATE,
                                                             sLogPieceHdr.mSize,
                                                             aErrorStack ) == STL_SUCCESS );
                            break;   
                        }
                        case ZTC_LOG_MEMORY_HEAP_DELETE_FOR_UPDATE:
                        {
                            /**
                             * DELETE_FOR_UPDATE는 Applier에서 사용하지 않지만,
                             * Undo 과정에 나오는 UNDELETE를 처리하기 위해서 저장해야 한다.
                             * Ps. UNDELETE가 DELETE의 Undo 인지, DELETE_FOR_UPDATE의 Undo인지 구분하지 못하기 때문에..
                             */
                            STL_TRY( ztccOperateTransaction( sLogPieceBody,
                                                             sLogHdr.mLsn,
                                                             sRecordID,
                                                             sLogHdr.mTransId,
                                                             sTableInfo,
                                                             ZTC_CAPTURE_TYPE_DELETE_FOR_UPDATE,
                                                             sLogPieceHdr.mSize,
                                                             aErrorStack ) == STL_SUCCESS );
                            break;
                        }
                        case ZTC_LOG_MEMORY_HEAP_APPEND:
                        {
                            /**
                             * ADD COLUMN시에 발생할 수 있는 로그 처리..
                             */
                            STL_TRY( ztccOperateTransaction( sLogPieceBody,
                                                             sLogHdr.mLsn,
                                                             sRecordID,
                                                             sLogHdr.mTransId,
                                                             sTableInfo,
                                                             ZTC_CAPTURE_TYPE_APPEND_FOR_UPDATE,
                                                             sLogPieceHdr.mSize,
                                                             aErrorStack ) == STL_SUCCESS );
                            break;
                        }
                        case ZTC_LOG_MEMORY_HEAP_UNAPPEND:
                        {
                            STL_TRY( ztccOperateTransUndo( sRecordID,
                                                           sLogHdr.mTransId,
                                                           ZTC_CAPTURE_TYPE_UNAPPEND_FOR_UPDATE,
                                                           aErrorStack ) == STL_SUCCESS );
                            break;
                        }
                        default:
                            break;
                    }   //end of switch
                }   //end of if
                
                /**
                 * COMMIT / ROLLBACK 처리
                 */
                if( sLogPieceHdr.mComponentClass == ZTC_COMPONENT_TRANSACTION )
                {
                    switch( sLogPieceHdr.mType )
                    {
                        case ZTC_LOG_INSERT_TRANSACTION_RECORD:
                        {
                            /**
                             * Transaction Begin
                             */
                            
                            /**
                             * Restart 시에 사용될 정보를 저장한다.
                             * Transaction Begin 시에는 현재 Redo Log의 위치 정보를 저장해야 한다.
                             */
                            sProgress.mBlockSeq  = sBlockSeq + sReadBlockCount - sPartialLogReadBlockCount - 1;
                            sProgress.mReadLogNo = sLogSequence;
                            sProgress.mLSN       = sLogHdr.mLsn;
                            
                            sPropagate = ZTC_IS_PROPAGATE_REDO_LOG( &sLogPieceHdr );
                            
                            STL_TRY( ztccOperateTransBegin( sLogHdr.mTransId,
                                                            &sProgress,
                                                            sPropagate,
                                                            aErrorStack ) == STL_SUCCESS );
                            
                            break;
                        }
                        case ZTC_LOG_COMMIT:
                        {
                            /**
                             * Restart 시에 사용될 정보를 저장한다.
                             * Commit시에는 다음에 읽어야할 정보를 저장해야 한다.
                             * 1. 남은 Redo Log가 없다면 다음 LogBlock을 읽도록 해야한다.
                             * 2. 남은 Redo Log가 있다면 현재 LogBlock에 다음 Log를 읽도록 해야한다.
                             */
                            if( sLeftLogCount == 0 )
                            {
                                sProgress.mBlockSeq  = sBlockSeq + sReadBlockCount - sPartialLogReadBlockCount;
                                sProgress.mReadLogNo = 1;
                            }
                            else
                            {
                                sProgress.mBlockSeq  = sBlockSeq + sReadBlockCount - sPartialLogReadBlockCount - 1;
                                sProgress.mReadLogNo = sLogNoInBlock + 1;
                            }
                            sProgress.mLSN = sLogHdr.mLsn;
                            
                            /**
                             * XA에서는 COMMIT LogHeader의 TransID가 정확하지 않을 수 있기 때문에
                             * Log Body에서 읽어와야 한다.
                             */
                            sPtr = sLogPieceBody;
                            ZTC_GET_TRANSID_FROM_PIECE_BODY( sLogPieceBody, &sTransId );
                            sPtr += STL_SIZEOF( sTransId );
                            STL_WRITE_INT64( &sCommitSCN, sPtr );
                            
                            STL_TRY( gCaptureCallback.mTransCommitFunc( sLogPieceBody, 
                                                                        sTransId,
                                                                        sCommitSCN,
                                                                        &sProgress,
                                                                        aErrorStack ) == STL_SUCCESS );
                            break;
                        }
                        case ZTC_LOG_ROLLBACK:
                        {
                            /**
                             * XA에서는 Rollback LogHeader의 TransID가 정확하지 않을 수 있기 때문에
                             * Log Body에서 읽어와야 한다.
                             */
                            ZTC_GET_TRANSID_FROM_PIECE_BODY( sLogPieceBody, &sTransId );
                            
                            STL_TRY( gCaptureCallback.mTransRollbackFunc( sLogPieceBody,
                                                                          sTransId,
                                                                          ZTC_INVALID_SCN,
                                                                          NULL,
                                                                          aErrorStack ) == STL_SUCCESS );
                            
                            break;
                        }
                        default:
                            break;
                    }
                }
                
                sLogBodyOffset += sLogPieceHdr.mSize;
            } //for LogPiece

            /**
             * Recovery시에 LastCommitLSN 까지 진행했는지 확인한다.
             */
            STL_TRY( gCaptureCallback.mCheckRecoveryEndFunc( sLogHdr.mLsn,
                                                             &sStatus ) == STL_SUCCESS );
            
            if( sStatus == STL_TRUE )
            {
                /**
                 * Recovery가 끝났을 경우에는 다음에 읽을 Redo Log의 위치를 정해준다.
                 */
                if( sLeftLogCount == 0 )
                {
                    sLogNoInBlock = 1;
                }
                else
                {
                    sReadBlockCount--;
                    sLogNoInBlock++;
                }
                /**
                 * 이미 쓰여진 Commit Log이므로 Partial Log로 처리되지 않는다.
                 */
                sCaptureStatus = ZTC_CAPTURE_STATUS_READ_DONE;
                STL_THROW( RAMP_SUCCESS );
            }

        }  //while( sLeftLogCount > 0 )
        
        /**
         * 현재 읽은 Log가 끝인지를 확인한다.
         */
        if ( ZTC_IS_LAST_FILE_BLOCK( &sLogBlockHdr ) == STL_TRUE )
        {
            sLogNoInBlock  = sReadLogNo;
            sCaptureStatus = ZTC_CAPTURE_STATUS_EOF;
            STL_THROW( RAMP_SUCCESS );
        }
        
    } // while( 1 )

    STL_RAMP( RAMP_SUCCESS );
    
    /**
     * 다음 Log를 읽기 위해서 Return Value 설정
     */
    aCaptureInfo->mBlockSeq  = sBlockSeq + sReadBlockCount - sPartialLogReadBlockCount;
    aCaptureInfo->mReadLogNo = sLogNoInBlock;
    *aCaptureStatus          = sCaptureStatus;
    
    sState = 0;
    STL_TRY( ztccCloseLogCursor( &sLogCursor,
                                 aErrorStack ) == STL_SUCCESS );
        
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void)ztccCloseLogCursor( &sLogCursor, aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}


stlStatus ztccValidateArchiveLogFile( stlChar        * aFileName,
                                      ztcLogFileInfo * aLogFileInfo,
                                      stlErrorStack  * aErrorStack )
{
    stlFile        sFile;
    stlInt32       sState     = 0;
    stlInt32       sIdx       = 0;
    stlInt64       sFileSeqNo = ZTC_INVALID_FILE_SEQ_NO;
    stlInt16       sGroupId   = ZTC_INVALID_FILE_GROUP_ID;
    stlBool        sIsValid   = STL_FALSE;
    ztcLogFileHdr  sLogFileHdr;
    stlFileInfo    sFileInfo;
    
    STL_TRY( stlOpenFile( &sFile,
                          aFileName,
                          STL_FOPEN_READ,
                          STL_FPERM_OS_DEFAULT,
                          aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( stlReadFile( &sFile,
                          &sLogFileHdr,
                          STL_SIZEOF( ztcLogFileHdr ),
                          NULL,
                          aErrorStack ) == STL_SUCCESS );
    sState = 0;
    STL_TRY( stlCloseFile( &sFile, 
                           aErrorStack ) == STL_SUCCESS );
    
    /**
     * 최대 10초까지 기다려보고 ArchiveLog File이 정상적이지 않으면 Failure로 처리한다.
     */
    for( sIdx = 0; sIdx < 10; sIdx++ )
    {
        STL_TRY( stlGetFileStatByName( &sFileInfo,
                                       aFileName,
                                       STL_FINFO_MIN,
                                       aErrorStack ) == STL_SUCCESS );
        
        if( sLogFileHdr.mFileSize == sFileInfo.mSize )
        {
            sIsValid = STL_TRUE;
            break;
        }
                
        stlSleep( STL_SET_SEC_TIME( 1 ) );
    }
    
    STL_TRY_THROW( sIsValid == STL_TRUE, RAMP_ERR_ARCHIVELOG_READ_TIMEOUT );
    
    STL_TRY( ztccGetLogFileSeqNoNGroupId( aFileName,
                                          &sFileSeqNo,
                                          &sGroupId,
                                          aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( aLogFileInfo->mCaptureInfo.mFileSeqNo == sFileSeqNo, RAMP_ERR_INVALID_ARCHIVELOG_FILE );
    STL_TRY_THROW( aLogFileInfo->mCaptureInfo.mRedoLogGroupId == sGroupId, RAMP_ERR_INVALID_ARCHIVELOG_FILE );
            
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_ARCHIVELOG_READ_TIMEOUT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_REDOLOG_READ_TIMEOUT,
                      NULL,
                      aErrorStack,
                      aFileName );
    }
    STL_CATCH( RAMP_ERR_INVALID_ARCHIVELOG_FILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_ARCHIVELOG,
                      NULL,
                      aErrorStack,
                      aFileName );
    }
    STL_FINISH;
    
    switch( sState )
    {
        case 1:
        (void) stlCloseFile( &sFile, 
                             aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
    
    
}


stlStatus ztccCheckCaptureStatus( stlErrorStack * aErrorStack )
{
    stlInt16    sGroupId;
    stlInt32    sBlockSeq;
    stlInt64    sFileSeqNo;
    stlInt64    sInterval;
    stlInt64    sGiveupInterval = ztcmGetGiveupInterval();
    stlInt32    sRetry = 0;
    
    ztcCaptureMgr * sCaptureMgr = &(gMasterMgr->mCaptureMgr);
    
    while( 1 )
    {
        STL_TRY( gRunState == STL_TRUE );
        
        /**
         * SLAVE가 접속되어있을 경우에만 동작하도록 한다.
         * Capture가 Recovery중에는 동작하지 않고 운영중일 경우에만 동작하도록 한다. (mCaptureMonitorStatus)..
         */
        if( ( gMonitorInfo.mMasterState == ZTC_MASTER_STATE_RUNNING ) && 
            ( sCaptureMgr->mCaptureMonitorStatus == STL_TRUE ) )
        {
            STL_TRY( ztcdGetLogFileStatus( gMasterMgr->mDbcHandle,
                                           &sGroupId,
                                           &sFileSeqNo,
                                           &sBlockSeq,
                                           aErrorStack ) == STL_SUCCESS );
            
            STL_TRY( ztccGetIntervalInfo( sGroupId,
                                          sFileSeqNo,
                                          sBlockSeq,
                                          &sInterval,
                                          aErrorStack ) == STL_SUCCESS );
            
            if( sInterval > sGiveupInterval )
            {
                sRetry++;
                
                /**
                 * 3초간 Capture가 못따라가면 Giveup 하도록 한다.
                 */
                if( sRetry >= 3 )
                {
                    STL_TRY( ztcmLogMsg( aErrorStack,
                                         "[CAPTURE] Interval has reached the limit value that GIVEUP_INTERVAL. (%ld)(%ld)\n", 
                                         sGiveupInterval,  
                                         sInterval ) == STL_SUCCESS );
                    gRunState = STL_FALSE;
                }
            }
            else
            {
                sRetry = 0;   
            }
        }
        
        stlSleep( STL_SET_SEC_TIME( 1 ) );
    }
    
    return STL_SUCCESS;   
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztccGetIntervalInfo( stlInt16        aGroupId,
                               stlInt64        aFileSeqNo,
                               stlInt32        aBlockSeq,
                               stlInt64      * aInterval,
                               stlErrorStack * aErrorStack )
{
    stlInt64   sInterval = 0;
    stlInt16   sGroupId  = aGroupId;
    stlInt64   sLoop     = 0;
    stlInt64   sIdx      = 0;
    
    ztcLogFileInfo * sLogFileInfo     = NULL;
    ztcLogFileInfo * sPrevLogFileInfo = NULL;
    
    /*
    stlPrintf("MONITORINFO [%ld][%d][%ld][%d]\n", 
               aFileSeqNo, 
               aBlockSeq,  
               gMonitorInfo.mFileSeqNo,
               gMonitorInfo.mBlockSeq );
    */
    
    /**
     * 아직 Capture의 정보가 수집되지 않은 상태라면 무시한다.
     */
    if( gMonitorInfo.mFileSeqNo == 0 &&
        gMonitorInfo.mBlockSeq  == 0 )
    {
        sInterval = 0;
        STL_THROW( RAMP_DONE );
    }
    
    if( gMonitorInfo.mFileSeqNo > aFileSeqNo )
    {
        sInterval = 0;
        STL_THROW( RAMP_DONE );
    }
    else
    {
        if( gMonitorInfo.mFileSeqNo == aFileSeqNo )
        {
            if( gMonitorInfo.mBlockSeq >= aBlockSeq )
            {
                sInterval = 0;
                STL_THROW( RAMP_DONE );
            }
            else
            {
                sInterval = aBlockSeq - gMonitorInfo.mBlockSeq;
                STL_THROW( RAMP_DONE );
            }
        }
    }
    
    /**
     * FILE 하나이상 차이날 경우.
     */
    sLoop = aFileSeqNo - gMonitorInfo.mFileSeqNo;
    sInterval += aBlockSeq;
    
    STL_RING_FOREACH_ENTRY( &gMasterMgr->mLogFileList, sLogFileInfo )
    {
        if( sLogFileInfo->mGroupId == sGroupId )
        {
            break;
        }
    }
    
    STL_DASSERT( sLogFileInfo != NULL );
    
    for( sIdx = 0; sIdx < sLoop; sIdx++ )
    {
        sPrevLogFileInfo = STL_RING_GET_PREV_DATA( &gMasterMgr->mLogFileList, sLogFileInfo );
        
        if( STL_RING_IS_HEADLINK( &gMasterMgr->mLogFileList, 
                                   sPrevLogFileInfo ) == STL_TRUE )
        {
            sLogFileInfo = STL_RING_GET_LAST_DATA( &gMasterMgr->mLogFileList );
        }
        else
        {
            sLogFileInfo = sPrevLogFileInfo;
        }
        
        if( sIdx + 1 == sLoop )
        {
            break;
        }
        
        sInterval += sLogFileInfo->mBlockCount;
    }
    
    sInterval += sLogFileInfo->mBlockCount - gMonitorInfo.mBlockSeq;
    
    STL_RAMP( RAMP_DONE );
    
    *aInterval = sInterval;
    
    return STL_SUCCESS;
}


stlStatus ztccGetLogFileBlockSeq( stlChar          * aFilePath,
                                  ztcCaptureInfo   * aCaptureInfo,
                                  stlInt64           aBeginLsn,
                                  stlInt16           aWrapNo,
                                  stlBool          * aFound,
                                  ztcCaptureStatus * aCaptureStatus,
                                  stlErrorStack    * aErrorStack )
{
    stlInt32          sState = 0;
    ztcLogHdr         sLogHdr;
    ztcLogBlockHdr    sLogBlockHdr;
    ztcLogCursor      sLogCursor;
    stlChar         * sLogBlock;
    stlInt32          sBlockOffset;
    stlInt32          sLeftLogCount;
    stlInt32          sTotalReadBytes;
    stlInt32          sReadBytes;
    stlInt32          sReadBlockCount   = 0;            //해당 함수가 실행되는 동안 읽은 Log Block Count
    stlBool           sFound     = STL_FALSE;
    stlInt32          sRetType;

    stlInt32          sBlockSeq      = aCaptureInfo->mBlockSeq;
    ztcCaptureStatus  sCaptureStatus = ZTC_CAPTURE_STATUS_NONE;
    
    STL_TRY( ztccOpenLogCursor( &sLogCursor,
                                sBlockSeq,
                                aFilePath,
                                &sRetType,
                                aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    sReadBlockCount = 0;
    
    if( sRetType == ZTC_CAPTURE_READ_WRITING )
    {
        sCaptureStatus = ZTC_CAPTURE_STATUS_READ_DONE;
        STL_THROW( RAMP_SUCCESS );
    }
    
    if( sLogCursor.mWrapNo != aWrapNo )
    {
        sCaptureStatus = ZTC_CAPTURE_STATUS_INVALID_WRAP_NO;
        STL_THROW( RAMP_SUCCESS );
    }
    
    while( 1 )
    {
        sBlockOffset    = 0;
        
        STL_TRY( ztccReadLogBlock( &sLogCursor,
                                   &sLogBlock,
                                   &sRetType,
                                   aErrorStack ) == STL_SUCCESS );
        
        if( sRetType == ZTC_CAPTURE_READ_EOF )
        {
            /**
             * 다음 파일을 읽도록 설정
             */
            sCaptureStatus = ZTC_CAPTURE_STATUS_EOF;
            STL_THROW( RAMP_SUCCESS );
        }
        else if( sRetType == ZTC_CAPTURE_READ_WRITING )
        {
            /**
             * File을 쓰고 있는 중이므로 해당 파일을 다시 읽도록 해야한다.
             */
            sCaptureStatus = ZTC_CAPTURE_STATUS_READ_DONE;
            STL_THROW( RAMP_SUCCESS );
        }

        ZTC_READ_MOVE_LOG_BLOCK_HDR( &sLogBlockHdr, sLogBlock, sBlockOffset );
        
        if( ZTC_IS_DUMMY_FILE_END_BLOCK( &sLogBlockHdr ) == STL_TRUE )
        {
            /**
             * 다음 파일을 읽도록 설정
             * DummyFileEnd가 TRUE인 경우에는 LogBlock을 읽을 필요가 없다.
             */
            sCaptureStatus = ZTC_CAPTURE_STATUS_EOF;
            STL_THROW( RAMP_SUCCESS );
        }
        
        /**
         * Old(Garbage) Log를 만날 경우 경우에 여기서 빠져나간다.
         */
        switch( ztccIsValidLog( &sLogCursor, &sLogBlockHdr ) )
        {
            case ZTC_LOG_STATUS_INVALID_LOG:
            {
                /**
                 * 추가된 Redo Log가 없을 경우에는 다시 해당 LogNo부터 읽을 수 있도록 세팅해야 한다.
                 */
                sCaptureStatus = ZTC_CAPTURE_STATUS_READ_DONE;
                STL_THROW( RAMP_SUCCESS );
                break;
            }
            case ZTC_LOG_STATUS_INVALID_WRAP_NO:
            {
                /**
                 * Wrap No가 다를 경우, 더 이상 로그를 읽으면 안된다.
                 */
                sCaptureStatus = ZTC_CAPTURE_STATUS_INVALID_WRAP_NO;
                STL_THROW( RAMP_SUCCESS );
                break;
            }
            case ZTC_LOG_STATUS_NONE:
            {
                /**
                 * 정상적인 경우
                 * Nothing To Do.
                 */
                break;
            }
            case ZTC_LOG_STATUS_MAX:
            default:
                break;
        }
        
        sReadBlockCount++;
        
        sBlockOffset  = sLogBlockHdr.mFirstLogOffset;
        sLeftLogCount = sLogBlockHdr.mLogCount;
        
        /** 
         * LogBlock에 포함되어 있는 Logs를 읽는다.
         */
        while( sLeftLogCount > 0 )
        {
            sLeftLogCount--;
            
            ZTC_READ_LOG_HDR( &sLogHdr, sLogBlock + sBlockOffset );
            sBlockOffset += STL_SIZEOF( ztcLogHdr );
            
            if( sLogHdr.mLsn > aBeginLsn )
            {
                /**
                 * 해당 파일에는 해당 LSN이 없으므로 다음 파일로 넘어가야 한다.
                 */
                sCaptureStatus = ZTC_CAPTURE_STATUS_EOF;
                STL_THROW( RAMP_SUCCESS );
            }
            
            /**
             * LSN을 찾았을 경우..
             */
            if( aBeginLsn == sLogHdr.mLsn )
            {
                sFound = STL_TRUE;
                STL_THROW( RAMP_SUCCESS );
            }
            
            if( sLogHdr.mSize > (sLogCursor.mBlockSize - sBlockOffset) )
            {
                sReadBytes = sLogCursor.mBlockSize - sBlockOffset;
            }
            else
            {
                sReadBytes = sLogHdr.mSize;
            }
            
            sTotalReadBytes = sReadBytes;
            sBlockOffset   += sReadBytes;
            
            while( sTotalReadBytes < sLogHdr.mSize )
            {
                sBlockOffset = 0;
                
                STL_TRY( ztccReadLogBlock( &sLogCursor,
                                           &sLogBlock,
                                           &sRetType,
                                           aErrorStack ) == STL_SUCCESS );
                
                if( sRetType == ZTC_CAPTURE_READ_EOF )
                {
                    /**
                     * Partial Log는 File에 나눠져서 저장되지 않는다.
                     * 이부분은 발생하지 않을 것임!! 즉, Chained Log는 없다!!!!
                     */
                    sCaptureStatus = ZTC_CAPTURE_STATUS_ERR; //or ASSERT !!
                    STL_THROW( RAMP_SUCCESS );
                }
                else if( sRetType == ZTC_CAPTURE_READ_WRITING )
                {
                    /**
                     * File을 쓰고 있는 중이므로 대기해야 한다.
                     */
                    sCaptureStatus = ZTC_CAPTURE_STATUS_READ_DONE;
                    STL_THROW( RAMP_SUCCESS );
                }
                
                /**
                 * BLOCKSEQ ++
                 */
                ZTC_READ_MOVE_LOG_BLOCK_HDR( &sLogBlockHdr, sLogBlock, sBlockOffset );
                
                /**
                 * Old(Garbage) Log를 만날 경우 경우에 여기서 빠져나간다.
                 */
                switch( ztccIsValidLog( &sLogCursor, &sLogBlockHdr ) )
                {
                    case ZTC_LOG_STATUS_INVALID_LOG:
                    {
                        /**
                         * 추가된 Redo Log가 없을 경우에는 다시 해당 LogNo부터 읽을 수 있도록 세팅해야 한다.
                         */
                        sCaptureStatus = ZTC_CAPTURE_STATUS_READ_DONE;
                        STL_THROW( RAMP_SUCCESS );
                        break;
                    }
                    case ZTC_LOG_STATUS_INVALID_WRAP_NO:
                    {
                        /**
                         * Wrap No가 다를 경우, 더 이상 로그를 읽으면 안된다.
                         */
                        sCaptureStatus = ZTC_CAPTURE_STATUS_INVALID_WRAP_NO;
                        STL_THROW( RAMP_SUCCESS );
                        break;
                    }
                    case ZTC_LOG_STATUS_NONE:
                    {
                        /**
                         * 정상적인 경우
                         * Nothing To Do.
                         */
                        break;
                    }
                    case ZTC_LOG_STATUS_MAX:
                    default:
                        break;
                }
                
                sReadBlockCount++;
                
                if( (sLogHdr.mSize - sTotalReadBytes) > (sLogCursor.mBlockSize - sBlockOffset) )
                {
                    sReadBytes = sLogCursor.mBlockSize - sBlockOffset;
                }
                else
                {
                    sReadBytes = (sLogHdr.mSize - sTotalReadBytes);
                }
        
                sTotalReadBytes += sReadBytes;
                sBlockOffset += sReadBytes;
                sLeftLogCount = sLogBlockHdr.mLogCount;
            }
        }
    }
    
    STL_RAMP( RAMP_SUCCESS );
    
    *aFound         = sFound;
    *aCaptureStatus = sCaptureStatus;
    
    /**
     * Block Sequence는 sBlockSeq + sReadBlockCount -1 이 된다....!!!!
     */
    aCaptureInfo->mBlockSeq = sBlockSeq + sReadBlockCount - 1;
    
    sState = 0;
    STL_TRY( ztccCloseLogCursor( &sLogCursor,
                                 aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    switch( sState )
    {
        case 1:
            (void) ztccCloseLogCursor( &sLogCursor,
                                       aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}

stlStatus ztccWakeUpCapture( stlErrorStack * aErrorStack )
{
    STL_TRY( stlReleaseSemaphore( &(gMasterMgr->mCaptureStartSem),
                                  aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

void ztccCheckPrimaryKeyUpdated( ztcCaptureTableInfo * aTableInfo,
                                 stlInt64              aTransId,
                                 stlChar             * aData )
{
    ztcTransSlotHdr * sTransSlotHdr = NULL;
    ztcCaptureMgr   * sCaptureMgr    = &(gMasterMgr->mCaptureMgr);
    stlInt16          sTransSlotId  = ZTC_TRANS_SLOT_ID( aTransId );
    stlBool           sUpdatePk = STL_FALSE;
    stlInt32          sOffset   = 0;

    sTransSlotHdr = &(sCaptureMgr->mTransSlotHdr[sTransSlotId]);
    
    STL_READ_MOVE_INT8( &sUpdatePk, aData, sOffset );
    
    if( sUpdatePk == STL_TRUE )
    {
        if( sTransSlotHdr->mGiveUp == STL_FALSE )
        {
            sTransSlotHdr->mGiveUp     = STL_TRUE;
            aTableInfo->mGiveUpTransId = aTransId;
        }
        aTableInfo->mGiveUpCount++;
    }
}

/** @} */

