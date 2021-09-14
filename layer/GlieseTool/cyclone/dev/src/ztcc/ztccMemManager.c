/*******************************************************************************
 * ztccMemManager.c
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
 * @file ztccMemManager.c
 * @brief GlieseTool Cyclone Capture Chunk Management Routines
 */

#include <goldilocks.h>
#include <ztc.h>

extern ztcMasterMgr  * gMasterMgr;
extern stlBool         gRunState;


stlStatus ztccGetChunkItemToWrite( ztcChunkItem ** aChunkItem,
                                   stlErrorStack * aErrorStack )
{
    ztcChunkItem   * sChunkItem      = NULL;
    ztcChunkItem   * sNextChunkItem  = NULL;
    stlBool          sIsTimeout      = STL_TRUE;
    
    if( STL_RING_IS_EMPTY( &gMasterMgr->mWriteCkList ) == STL_FALSE )
    {
        /**
         * 처음 Item을 가져온다.
         */
        sChunkItem = STL_RING_GET_FIRST_DATA( &gMasterMgr->mWriteCkList );
    }
    else
    {
        ztcmAcquireSpinLock( &(gMasterMgr->mWriteSpinLock) );
        
        if( STL_RING_IS_EMPTY( &gMasterMgr->mWaitWriteCkList ) == STL_TRUE )
        {
            gMasterMgr->mWaitWriteState = STL_TRUE;
            
            ztcmReleaseSpinLock( &(gMasterMgr->mWriteSpinLock) );
            
            while( sIsTimeout == STL_TRUE )
            {
                STL_TRY( stlTimedAcquireSemaphore( &(gMasterMgr->mWaitWriteSem),
                                                   STL_SET_SEC_TIME( 1 ),
                                                   &sIsTimeout,
                                                   aErrorStack ) == STL_SUCCESS );
                
                if( gRunState == STL_TRUE && 
                    gMasterMgr->mCaptureRestart == STL_TRUE )
                {
                    /**
                     * Capture가 Restart 하는 것이므로 동시성 제어할 필요가 없다.
                     */
                    gMasterMgr->mWaitWriteState = STL_FALSE;
                    STL_TRY( 0 );   
                }
            }
            
            ztcmAcquireSpinLock( &(gMasterMgr->mWriteSpinLock) );
        }
        
        STL_RING_FOREACH_ENTRY_SAFE( &gMasterMgr->mWaitWriteCkList, sChunkItem, sNextChunkItem )
        {
            STL_RING_UNLINK( &gMasterMgr->mWaitWriteCkList, sChunkItem );
            
            STL_RING_ADD_LAST( &gMasterMgr->mWriteCkList, 
                               sChunkItem );
        }
        
        ztcmReleaseSpinLock( &(gMasterMgr->mWriteSpinLock) );
        
        STL_ASSERT( STL_RING_IS_EMPTY( &gMasterMgr->mWriteCkList ) == STL_FALSE );
        
        sChunkItem = STL_RING_GET_FIRST_DATA( &gMasterMgr->mWriteCkList );
    }
    
    *aChunkItem = sChunkItem;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztccWriteCommittedTransToChunk( ztcTransSlotHdr * aTransSlotHdr,
                                          stlUInt64         aCommitSCN,
                                          stlInt32          aCommentLength,
                                          stlChar         * aComment,
                                          stlErrorStack   * aErrorStack )
{
    ztcTransData   * sTransData     = NULL;
    ztcChunkItem   * sChunkItem     = NULL;
    stlChar        * sDataPtr       = NULL;
    stlInt64         sWriteIdx      = 0;
    stlInt64         sWritableSize  = 0;
    stlInt64         sWriteSize     = 0;
    stlBool          sExist         = STL_FALSE;
    stlSize          sFileReadSize  = 0;
    stlInt32         sState         = 0;
    stlInt64         sIdx           = 0;
    stlInt32         sCommentLength = STL_ALIGN( STL_SIZEOF( stlInt32 ) + aCommentLength, ZTC_MEM_ALIGN );
    
    stlFile     sFile;
    stlChar     sFileName[ STL_MAX_FILE_PATH_LENGTH ] = {0, };
    stlChar   * sLogBufferPtr = NULL;
    stlChar   * sLogBuffer    = NULL;
    
    ztcCaptureTableInfo * sTableInfo = NULL;
    ztcTransHdr         * sTransHdr  = NULL;
    
    STL_TRY( stlAllocHeap( (void**)&sLogBufferPtr,
                           ( ZTC_LOG_PIECE_MAX_SIZE + STL_SIZEOF(ztcTransHdr) + ZTC_MEM_ALIGN ),
                           aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    sLogBuffer = (void*)STL_ALIGN( (stlUInt64)sLogBufferPtr, ZTC_MEM_ALIGN );
    sTransHdr  = (ztcTransHdr*)sLogBuffer;
    
    /**
     * Chunk 하나를 얻어온다.
     */
    STL_TRY( ztccGetChunkItemToWrite( &sChunkItem,
                                      aErrorStack ) == STL_SUCCESS );
    
    if( aTransSlotHdr->mFileLogCount > 0 )
    {
        STL_TRY( ztcmGetTransFileName( ZTC_TRANS_SLOT_ID( aTransSlotHdr->mTransId ),
                                       sFileName,
                                       aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( stlExistFile( sFileName,
                               &sExist,
                               aErrorStack ) == STL_SUCCESS );
    
        STL_TRY_THROW( sExist == STL_TRUE, RAMP_ERR_NOT_EXIST_FILE );
    
        STL_TRY( stlOpenFile( &sFile,
                              sFileName,
                              STL_FOPEN_READ | STL_FOPEN_BINARY,
                              STL_FPERM_OS_DEFAULT,
                              aErrorStack ) == STL_SUCCESS );
        sState = 2;
    }
    
    if( ( aTransSlotHdr->mSize + aTransSlotHdr->mFileSize + sCommentLength ) <= ZTC_CHUNK_ITEM_SIZE - sChunkItem->mWriteIdx )
    {
        /**
         * 하나의 Chunk에 모두 넣을 수 있는 경우.
         * 1. File에 쓰여진 내용이 있으면 File 부터 읽어서 보낸다.
         * 2. Memory에 있는 내용을 보낸다.
         */
        if( aTransSlotHdr->mFileLogCount > 0 )
        {
            /**
             * 성능 튜닝해야함!!!
             */ 
            for( sIdx = 0; sIdx < aTransSlotHdr->mFileLogCount; sIdx++ )
            {
                // TransHeader 읽기
                STL_TRY( stlReadFile( &sFile,
                                      sTransHdr,
                                      STL_SIZEOF( ztcTransHdr ),
                                      &sFileReadSize,
                                      aErrorStack ) == STL_SUCCESS );
            
                STL_TRY_THROW( STL_SIZEOF( ztcTransHdr ) == sFileReadSize, RAMP_ERROR_FILE_READ );
                STL_TRY_THROW( sTransHdr->mSize <= ZTC_LOG_PIECE_MAX_SIZE, RAMP_ERROR_FILE_READ );
                
                // TransBody 읽기
                STL_TRY( stlReadFile( &sFile,
                                      &sLogBuffer[ STL_SIZEOF(ztcTransHdr) ],
                                      sTransHdr->mSize,
                                      &sFileReadSize,
                                      aErrorStack ) == STL_SUCCESS );
            
                STL_TRY_THROW( sTransHdr->mSize == sFileReadSize, RAMP_ERROR_FILE_READ );
                
                if( sTransHdr->mTablePId != ZTC_INVALID_TABLE_PHYSICAL_ID )
                {
                    STL_TRY( ztcmFindCaptureTableInfo( &sTableInfo,
                                                       sTransHdr->mTablePId,
                                                       aErrorStack ) == STL_SUCCESS );
                
                    STL_DASSERT( sTableInfo != NULL );
                    
                    /**
                     * Transaction에 포함된 Table의 CommitScn이 현재 Commit된 SCN 보다 크거나 같을 경우는 
                     * 이미 Slave에 반영된것 이기 때문에 Slave에 보내지 않는다. (SYNC 기능추가에 따른 구현인데 성능에 영향을 미칠수 있는 부분임..나중에 좀더 생각해 봐야겠음)
                     */ 
                    if( sTableInfo->mCommitSCN >= aCommitSCN && 
                        sTableInfo->mCommitSCN != ZTC_INVALID_SCN )
                    {
                        continue;
                    }
                }
                
                /**
                 * Commit Comment 가 있을 경우 Transaction Begin에 넣어서 처리하도록 함 
                 */
                if( sTransHdr->mLogType == ZTC_CAPTURE_TYPE_BEGIN )
                {
                    sTransHdr->mSize = sCommentLength;
                    STL_WRITE_INT32( &sLogBuffer[ STL_SIZEOF(ztcTransHdr) ], &aCommentLength );
                    
                    if( aCommentLength > 0 )
                    {
                        stlMemcpy( &sLogBuffer[ STL_SIZEOF(ztcTransHdr) + STL_SIZEOF( stlInt32 ) ], aComment, aCommentLength );
                    }
                }
                
                stlMemcpy( &(sChunkItem->mBuffer[sChunkItem->mWriteIdx]),
                           sLogBuffer,
                           STL_SIZEOF(ztcTransHdr) + sTransHdr->mSize );
            
                sChunkItem->mWriteIdx += STL_SIZEOF(ztcTransHdr) + sTransHdr->mSize;
            }   //end of for loop
        }
        
        STL_RING_FOREACH_ENTRY( &aTransSlotHdr->mTransDataList, sTransData )
        {
            sDataPtr = (stlChar*)&sTransData->mTransHdr;
            
            if( sTransData->mTransHdr.mTablePId != ZTC_INVALID_TABLE_PHYSICAL_ID )
            {
                STL_TRY( ztcmFindCaptureTableInfo( &sTableInfo,
                                                   sTransData->mTransHdr.mTablePId,
                                                   aErrorStack ) == STL_SUCCESS );
                
                STL_DASSERT( sTableInfo != NULL );
                
                /**
                 * Transaction에 포함된 Table의 CommitScn이 현재 Commit된 SCN 보다 크거나 같을 경우는 
                 * 이미 Slave에 반영된것 이기 때문에 Slave에 보내지 않는다. (SYNC 기능추가에 따른 구현인데 성능에 영향을 미칠수 있는 부분임..나중에 좀더 생각해 봐야겠음)
                 */ 
                if( sTableInfo->mCommitSCN >= aCommitSCN && 
                    sTableInfo->mCommitSCN != ZTC_INVALID_SCN )
                {
                    continue;
                }
            }
            
            /**
              * Commit Comment 가 있을 경우 Transaction Begin에 넣어서 처리하도록 함 
              */
            if( sTransData->mTransHdr.mLogType == ZTC_CAPTURE_TYPE_BEGIN )
            {
                sTransData->mTransHdr.mSize = sCommentLength;
                stlMemcpy( sLogBuffer, sDataPtr, STL_SIZEOF(ztcTransHdr) );
                STL_WRITE_INT32( &sLogBuffer[ STL_SIZEOF(ztcTransHdr) ], &aCommentLength );
                
                if( aCommentLength > 0 )
                {
                    stlMemcpy( &sLogBuffer[ STL_SIZEOF(ztcTransHdr) + STL_SIZEOF( stlInt32 ) ], aComment, aCommentLength );
                }
                
                stlMemcpy( &(sChunkItem->mBuffer[sChunkItem->mWriteIdx]),
                           sLogBuffer,
                           STL_SIZEOF(ztcTransHdr) + sCommentLength );
                
                sChunkItem->mWriteIdx += STL_SIZEOF(ztcTransHdr) + sCommentLength;
            }
            else
            {
                stlMemcpy( &(sChunkItem->mBuffer[sChunkItem->mWriteIdx]),
                           sDataPtr,
                           sTransData->mBufferSize );
                
                sChunkItem->mWriteIdx += sTransData->mBufferSize;
            }
        }
    }
    else
    {
        /**
         * 두개 이상의 Chunk에 나눠서 저장될 경우
         */
        if( aTransSlotHdr->mFileLogCount > 0 )
        {
            for( sIdx = 0; sIdx < aTransSlotHdr->mFileLogCount; sIdx++ )
            {
                // TransHeader 읽기
                STL_TRY( stlReadFile( &sFile,
                                      sTransHdr,
                                      STL_SIZEOF( ztcTransHdr ),
                                      &sFileReadSize,
                                      aErrorStack ) == STL_SUCCESS );
            
                STL_TRY_THROW( STL_SIZEOF( ztcTransHdr ) == sFileReadSize, RAMP_ERROR_FILE_READ );
                STL_TRY_THROW( sTransHdr->mSize <= ZTC_LOG_PIECE_MAX_SIZE, RAMP_ERROR_FILE_READ );
                
                if( sTransHdr->mSize > 0 )
                {
                    // TransBody 읽기
                    STL_TRY( stlReadFile( &sFile,
                                          &sLogBuffer[ STL_SIZEOF(ztcTransHdr) ],
                                          sTransHdr->mSize,
                                          &sFileReadSize,
                                          aErrorStack ) == STL_SUCCESS );
            
                    STL_TRY_THROW( sTransHdr->mSize == sFileReadSize, RAMP_ERROR_FILE_READ );
                }
                
                if( sTransHdr->mTablePId != ZTC_INVALID_TABLE_PHYSICAL_ID )
                {
                    STL_TRY( ztcmFindCaptureTableInfo( &sTableInfo,
                                                       sTransHdr->mTablePId,
                                                       aErrorStack ) == STL_SUCCESS );
                
                    STL_DASSERT( sTableInfo != NULL );
                    
                    /**
                     * Transaction에 포함된 Table의 CommitScn이 현재 Commit된 SCN 보다 크거나 같을 경우는 
                     * 이미 Slave에 반영된것 이기 때문에 Slave에 보내지 않는다. (SYNC 기능추가에 따른 구현인데 성능에 영향을 미칠수 있는 부분임..나중에 좀더 생각해 봐야겠음)
                     */ 
                    if( sTableInfo->mCommitSCN >= aCommitSCN && 
                        sTableInfo->mCommitSCN != ZTC_INVALID_SCN )
                    {
                        continue;
                    }
                }
                
                if( sTransHdr->mLogType == ZTC_CAPTURE_TYPE_BEGIN )
                {
                    sTransHdr->mSize = sCommentLength;
                    STL_WRITE_INT32( &sLogBuffer[ STL_SIZEOF(ztcTransHdr) ], &aCommentLength );
                    
                    if( aCommentLength > 0 )
                    {
                        stlMemcpy( &sLogBuffer[ STL_SIZEOF(ztcTransHdr) + STL_SIZEOF( stlInt32 ) ], aComment, aCommentLength );
                    }
                }
                
                sWritableSize = ZTC_CHUNK_ITEM_SIZE - sChunkItem->mWriteIdx;
                
                if( sWritableSize >= STL_SIZEOF(ztcTransHdr) + sTransHdr->mSize )
                {
                    stlMemcpy( &(sChunkItem->mBuffer[sChunkItem->mWriteIdx]),
                               sLogBuffer,
                               STL_SIZEOF(ztcTransHdr) + sTransHdr->mSize );
                    
                    sChunkItem->mWriteIdx += STL_SIZEOF(ztcTransHdr) + sTransHdr->mSize;
                }
                else
                {
                    /**
                     * 현재 Chunk에 복사할 수 있는 만큼만 넣는다.
                     */
                    stlMemcpy( &(sChunkItem->mBuffer[sChunkItem->mWriteIdx]),
                               sLogBuffer,
                               sWritableSize );
                    
                    sChunkItem->mWriteIdx += sWritableSize;
                    
                    STL_DASSERT( sChunkItem->mWriteIdx <= ZTC_CHUNK_ITEM_SIZE );
                    
                    STL_TRY( ztccPushChunkToWaitReadList( sChunkItem,
                                                          aErrorStack ) == STL_SUCCESS );
                    
                    /**
                     * 새로운 Chunk를 얻고 나머지를 복사한다.
                     */
                    STL_TRY( ztccGetChunkItemToWrite( &sChunkItem,
                                                      aErrorStack ) == STL_SUCCESS );
                    
                    sWriteSize = STL_SIZEOF(ztcTransHdr) + sTransHdr->mSize - sWritableSize;
                    
                    stlMemcpy( &(sChunkItem->mBuffer[sChunkItem->mWriteIdx]),
                               &sLogBuffer[sWritableSize],        //sWritableSize 만큼 이전에 복사했음..
                               sWriteSize );
                    
                    sChunkItem->mWriteIdx += sWriteSize;
                }
            } //end of for loop
        } //end of exist file
        
        STL_RING_FOREACH_ENTRY( &aTransSlotHdr->mTransDataList, sTransData )
        {
            sWriteIdx = 0;
            
            sDataPtr = (stlChar*)&sTransData->mTransHdr;
            
            if( sTransData->mTransHdr.mTablePId != ZTC_INVALID_TABLE_PHYSICAL_ID )
            {
                STL_TRY( ztcmFindCaptureTableInfo( &sTableInfo,
                                                   sTransData->mTransHdr.mTablePId,
                                                   aErrorStack ) == STL_SUCCESS );
                
                STL_DASSERT( sTableInfo != NULL );
                
                /**
                 * Transaction에 포함된 Table의 CommitScn이 현재 Commit된 SCN 보다 크거나 같을 경우는 
                 * 이미 Slave에 반영된것 이기 때문에 Slave에 보내지 않는다.
                 */ 
                if( sTableInfo->mCommitSCN >= aCommitSCN && 
                    sTableInfo->mCommitSCN != ZTC_INVALID_SCN )
                {
                    continue;
                }
            }
            
            if( sTransData->mTransHdr.mLogType == ZTC_CAPTURE_TYPE_BEGIN )
            {
                sTransData->mTransHdr.mSize = sCommentLength;
                stlMemcpy( sLogBuffer, sDataPtr, STL_SIZEOF(ztcTransHdr) );
                STL_WRITE_INT32( &sLogBuffer[ STL_SIZEOF(ztcTransHdr) ], &aCommentLength );
                
                if( aCommentLength > 0 )
                {
                    stlMemcpy( &sLogBuffer[ STL_SIZEOF(ztcTransHdr) + STL_SIZEOF( stlInt32 ) ], aComment, aCommentLength );
                }
                
                while( 1 )
                {
                    sWritableSize = ZTC_CHUNK_ITEM_SIZE - sChunkItem->mWriteIdx;
            
                    if( sWritableSize == 0 )
                    {
                        STL_TRY( ztccPushChunkToWaitReadList( sChunkItem,
                                                              aErrorStack ) == STL_SUCCESS );
                    
                        STL_TRY( ztccGetChunkItemToWrite( &sChunkItem,
                                                          aErrorStack ) == STL_SUCCESS );
                    
                        sWritableSize = ZTC_CHUNK_ITEM_SIZE - sChunkItem->mWriteIdx;
                    }
                
                    sWriteSize = STL_MIN( sWritableSize, STL_SIZEOF( ztcTransHdr ) + sCommentLength - sWriteIdx );
                
                    stlMemcpy( &(sChunkItem->mBuffer[sChunkItem->mWriteIdx]),
                               &sLogBuffer[sWriteIdx], 
                               sWriteSize );
            
                    sWriteIdx += sWriteSize;
                    sChunkItem->mWriteIdx += sWriteSize;
            
                    if( sWriteIdx == STL_SIZEOF( ztcTransHdr ) + sCommentLength )
                    {
                        break;
                    }
                } //End of While
                
                continue;
            }
            
            while( 1 )
            {
                sWritableSize = ZTC_CHUNK_ITEM_SIZE - sChunkItem->mWriteIdx;
            
                if( sWritableSize == 0 )
                {
                    STL_TRY( ztccPushChunkToWaitReadList( sChunkItem,
                                                          aErrorStack ) == STL_SUCCESS );
                    
                    STL_TRY( ztccGetChunkItemToWrite( &sChunkItem,
                                                      aErrorStack ) == STL_SUCCESS );
                    
                    sWritableSize = ZTC_CHUNK_ITEM_SIZE - sChunkItem->mWriteIdx;
                }
                
                sWriteSize = STL_MIN( sWritableSize, sTransData->mBufferSize - sWriteIdx );
                
                stlMemcpy( &(sChunkItem->mBuffer[sChunkItem->mWriteIdx]),
                           &sDataPtr[sWriteIdx], 
                           sWriteSize );
            
                sWriteIdx += sWriteSize;
                sChunkItem->mWriteIdx += sWriteSize;
            
                if( sWriteIdx == sTransData->mBufferSize )
                {
                    break;
                }
            } //End of While
            
        }//End of FOREACH
    }
    
    if( ZTC_CHUNK_ITEM_SIZE == sChunkItem->mWriteIdx )
    {
        STL_TRY( ztccPushChunkToWaitReadList( sChunkItem,
                                              aErrorStack ) == STL_SUCCESS );
    }
    
    if( aTransSlotHdr->mFileLogCount > 0 )
    {    
        sState = 1;
        STL_TRY( stlCloseFile( &sFile,
                               aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( stlRemoveFile( sFileName,
                                aErrorStack ) == STL_SUCCESS );
        
    }
    
    sState = 0;
    stlFreeHeap( sLogBufferPtr );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_NOT_EXIST_FILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_FILE_NAME,
                      NULL,
                      aErrorStack,
                      sFileName );
    }
    STL_CATCH( RAMP_ERROR_FILE_READ )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_TEMP_FILE,
                      NULL,
                      aErrorStack,
                      sFileName );
    }
    STL_FINISH;
    
    switch( sState )
    {
        case 2:
            (void) stlCloseFile( &sFile,
                             aErrorStack );
        case 1:
            stlFreeHeap( sLogBufferPtr );
        default:
            break;
    }
        
    return STL_FAILURE;
}


stlStatus ztccPushChunkToWaitReadList( ztcChunkItem  * aChunkItem,
                                       stlErrorStack * aErrorStack )
{
    stlBool  sDoRelease = STL_FALSE;
    
    /**
     * Sender가 보낼 수 있도록 Wait SendList에 넣는다.
     */
    STL_RING_UNLINK( &gMasterMgr->mWriteCkList, aChunkItem );
    
    ztcmAcquireSpinLock( &(gMasterMgr->mReadSpinLock) );

    STL_RING_ADD_LAST( &(gMasterMgr->mWaitReadCkList), 
                       aChunkItem );
    
    sDoRelease = gMasterMgr->mWaitReadState;
    
    gMasterMgr->mWaitReadState = STL_FALSE;
    
    ztcmReleaseSpinLock( &(gMasterMgr->mReadSpinLock) );
    
    if( sDoRelease == STL_TRUE )
    {
        /**
         * Sender가 대기하고 있으면 깨워준다.
         */
        STL_TRY( stlReleaseSemaphore( &(gMasterMgr->mWaitReadSem), 
                                      aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztccPushFirstChunkToWaitReadList( stlErrorStack * aErrorStack )
{
    stlBool         sDoRelease = STL_FALSE;
    ztcChunkItem  * sChunkItem = NULL;
    
    /**
     * 첫번째 Chunk를 얻어온다.
     */
    STL_TRY_THROW( STL_RING_IS_EMPTY( &gMasterMgr->mWriteCkList ) == STL_FALSE, RAMP_SUCCESS );
    
    sChunkItem = STL_RING_GET_FIRST_DATA( &gMasterMgr->mWriteCkList );
    
    STL_TRY_THROW( sChunkItem->mWriteIdx != 0, RAMP_SUCCESS );
    
    /**
     * Sender가 보낼 수 있도록 Wait SendList에 넣는다.
     */
    STL_RING_UNLINK( &gMasterMgr->mWriteCkList, sChunkItem );
    
    ztcmAcquireSpinLock( &(gMasterMgr->mReadSpinLock) );
    
    STL_RING_ADD_LAST( &gMasterMgr->mWaitReadCkList, 
                       sChunkItem );
    
    sDoRelease = gMasterMgr->mWaitReadState;
    
    gMasterMgr->mWaitReadState = STL_FALSE;
    
    ztcmReleaseSpinLock( &(gMasterMgr->mReadSpinLock) );

    if( sDoRelease == STL_TRUE )
    {
        /**
         * Sender가 대기하고 있으면 깨워준다.
         */
        STL_TRY( stlReleaseSemaphore( &(gMasterMgr->mWaitReadSem), 
                                      aErrorStack ) == STL_SUCCESS );
    }
    
    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztccClearFirstWriteChunk( stlErrorStack * aErrorStack )
{
    ztcChunkItem   * sChunkItem = NULL;
    
    STL_TRY_THROW( STL_RING_IS_EMPTY( &gMasterMgr->mWriteCkList ) == STL_FALSE, RAMP_SUCCESS );
    
    sChunkItem = STL_RING_GET_FIRST_DATA( &gMasterMgr->mWriteCkList );
    
    sChunkItem->mWriteIdx = 0;
    sChunkItem->mReadIdx  = 0;
    
    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;
}

/** @} */

