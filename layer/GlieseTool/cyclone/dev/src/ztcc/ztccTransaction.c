/*******************************************************************************
 * ztccTransaction.c
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
 * @file ztccTransaction.c
 * @brief GlieseTool Cyclone Transaction Routines
 */

#include <goldilocks.h>
#include <ztc.h>

extern ztcMasterMgr * gMasterMgr;
extern stlBool        gRunState;


stlStatus ztccAddRestartInfo( ztcCaptureProgress * aProgress,
                              ztcRestartInfo     * aRestartInfo,
                              stlErrorStack      * aErrorStack )
{
    ztcCaptureMgr  * sCaptureMgr  = &(gMasterMgr->mCaptureMgr);
    
    aRestartInfo->mProgress.mLSN            = aProgress->mLSN;          //RESTART LSN
    aRestartInfo->mProgress.mFileSeqNo      = aProgress->mFileSeqNo;
    aRestartInfo->mProgress.mBlockSeq       = aProgress->mBlockSeq;
    aRestartInfo->mProgress.mReadLogNo      = aProgress->mReadLogNo;
    aRestartInfo->mProgress.mWrapNo         = aProgress->mWrapNo;
    aRestartInfo->mProgress.mRedoLogGroupId = aProgress->mRedoLogGroupId;

    STL_RING_ADD_LAST( &sCaptureMgr->mRestartInfoList, 
                       aRestartInfo );
    
    return STL_SUCCESS;
}

stlStatus ztccRemoveRestartInfo( ztcRestartInfo * aRestartInfo,
                                 stlErrorStack  * aErrorStack )
{
    ztcCaptureMgr * sCaptureMgr = &(gMasterMgr->mCaptureMgr);
    
    STL_DASSERT( STL_RING_IS_EMPTY( &(sCaptureMgr->mRestartInfoList) ) == STL_FALSE );
    
    STL_RING_UNLINK( &(sCaptureMgr->mRestartInfoList), 
                     aRestartInfo );
    
    return STL_SUCCESS;
}



stlStatus ztccRemoveListAtSlot( ztcTransSlotHdr * aTransSlotHdr,
                                stlErrorStack   * aErrorStack )
{
    ztcTransData   * sTransData      = NULL;
    ztcTransData   * sPrevTransData  = NULL;
    ztcChunkItem   * sChunkItem      = NULL;
    ztcChunkItem   * sPrevChunkItem  = NULL;
    ztcCaptureMgr  * sCaptureMgr     = &(gMasterMgr->mCaptureMgr);
    
    STL_RING_FOREACH_ENTRY_REVERSE_SAFE( &aTransSlotHdr->mTransDataList, 
                                         sTransData, 
                                         sPrevTransData )
    {
        STL_RING_UNLINK( &aTransSlotHdr->mTransDataList, sTransData );
    }
    
    STL_RING_FOREACH_ENTRY_REVERSE_SAFE( &aTransSlotHdr->mTransChunkList, 
                                         sChunkItem, 
                                         sPrevChunkItem )
    {
        STL_RING_UNLINK( &aTransSlotHdr->mTransChunkList, sChunkItem );
        
        sChunkItem->mWriteIdx = 0;
        sChunkItem->mReadIdx  = 0;
        
        STL_RING_ADD_LAST( &sCaptureMgr->mTransChunkFreeList, 
                           sChunkItem );
    }
    
    return STL_SUCCESS;
}


stlStatus ztccOperateTransBegin( stlInt64             aTransId,
                                 ztcCaptureProgress * aProgress,
                                 stlBool              aPropagate,
                                 stlErrorStack      * aErrorStack )
{
    ztcTransSlotHdr  * sTransSlotHdr  = NULL;
    ztcCaptureMgr    * sCaptureMgr    = &(gMasterMgr->mCaptureMgr);
    stlInt16           sTransSlotId   = ZTC_TRANS_SLOT_ID( aTransId );
    ztcTransHdr        sTransHdr;
    
    /**
     * MAX 넘는지 비교
     * Restart 이후에 TRANSACTION_TABLE_SIZE가 변경될 수 있기 때문에 
     * 이에 대한 대비를 하기 위한 루틴이 들어가야 한다.
     * 우선은 이것만 체크!!!
     */
    STL_TRY_THROW( sTransSlotId < sCaptureMgr->mTransTableMax, RAMP_ERR_INVALID_TRANSACTION_SLOT_ID );
    
    sTransSlotHdr = &(sCaptureMgr->mTransSlotHdr[sTransSlotId]);
    
    STL_DASSERT( sTransSlotHdr->mIsBegin == STL_FALSE && 
                 sTransSlotHdr->mTransId == ZTC_INVALID_TRANSID );
    
    STL_DASSERT( STL_RING_IS_EMPTY( &sTransSlotHdr->mTransDataList ) == STL_TRUE );
    STL_DASSERT( STL_RING_IS_EMPTY( &sTransSlotHdr->mTransChunkList ) == STL_TRUE );
    
    sTransHdr.mLSN      = aProgress->mLSN;
    sTransHdr.mTransId  = aTransId;
    sTransHdr.mRecordId = 0;
    sTransHdr.mTablePId = ZTC_INVALID_TABLE_PHYSICAL_ID;
    sTransHdr.mTableId  = 0;
    sTransHdr.mSlotIdx  = ZTC_INVALID_SLOT_IDX;
    sTransHdr.mLogType  = ZTC_CAPTURE_TYPE_BEGIN;
    sTransHdr.mSize     = 0;
    
    ztccInitializeSlotHdr( sTransSlotHdr );
    
    /**
     * Transaction Slot Header 정보를 세팅 한다.
     */
    sTransSlotHdr->mTransId     = aTransId;
    sTransSlotHdr->mIsBegin     = STL_TRUE;
    sTransSlotHdr->mPropagated  = aPropagate;

    STL_TRY( ztccAddTransDataToSlot( sTransSlotHdr, 
                                     &sTransHdr, 
                                     NULL, 
                                     0,
                                     aErrorStack ) == STL_SUCCESS );
    
    /**
     * Restart 정보를 추가한다.
     */
    STL_TRY( ztccAddRestartInfo( aProgress, 
                                 &sTransSlotHdr->mRestartInfo,
                                 aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_TRANSACTION_SLOT_ID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_SLOT_ID,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztccOperateTransCommit( stlChar            * aData,
                                  stlInt64             aTransId,
                                  stlUInt64            aCommitSCN,
                                  ztcCaptureProgress * aProgress,
                                  stlErrorStack      * aErrorStack )
{
    ztcCaptureMgr       * sCaptureMgr   = &(gMasterMgr->mCaptureMgr);
    ztcCaptureProgress  * sCurProgress  = aProgress; 
    ztcCaptureProgress  * sProgress     = NULL; 
    ztcTransSlotHdr     * sTransSlotHdr = NULL;
    ztcRestartInfo      * sRestartInfo  = NULL;
    stlInt16              sTransSlotId  = ZTC_TRANS_SLOT_ID( aTransId );
    stlChar             * sPtr          = aData;
    ztcCaptureInfo        sCaptureInfo;
    ztcTransHdr           sTransHdr;
    stlInt32              sCommentLen   = 0;
    
    STL_TRY_THROW( sTransSlotId < sCaptureMgr->mTransTableMax, RAMP_ERR_INVALID_TRANSACTION_SLOT_ID );
    
    stlMemset( &sCaptureInfo, 0x00, STL_SIZEOF( ztcCaptureInfo ) );
    stlMemset( &sTransHdr, 0x00, STL_SIZEOF( ztcTransHdr ) );

    sTransSlotHdr = &(sCaptureMgr->mTransSlotHdr[sTransSlotId]);
    
    /**
     * Transaction 중간에 Capture된 Log는 Skip 한다.
     */
    STL_TRY_THROW( sTransSlotHdr->mIsBegin == STL_TRUE, RAMP_SUCCESS );
    
    /**
     * DDL이 포함되어 있을 경우 Meta파일을 Update 한다.
     */
    if( sTransSlotHdr->mGiveUp == STL_TRUE )
    {
        /**
         * DDL LSN을 Update해야 함!!
         */
        STL_TRY( ztccUpdateTableCommitScn4DDL( aTransId,
                                               aErrorStack ) == STL_SUCCESS );
        
        sTransSlotHdr->mGiveUp = STL_FALSE;
    }
    
    if( sTransSlotHdr->mLogCount > 1 || 
        sTransSlotHdr->mFileLogCount > 0 )
    {
        sPtr += ZTC_TRANS_COMMENT_SIZE_OFFSET;
        STL_WRITE_INT32( &sCommentLen, sPtr );
        
        sPtr = aData;
        sPtr += ZTC_TRANS_COMMENT_OFFSET;
       
        /**
         * 복구 및 Restart를 위한 정보 구성
         * 1. Commit된 Transaction의 Restart 정보를 RestartInfo List에서 제거
         * 2. RestartInfo List에서 첫번째 List로 Restart 정보 저장 (List가 없을 경우, 이후부터 Restart가 진행되어야 한다.)
         */
        STL_TRY( ztccRemoveRestartInfo( &sTransSlotHdr->mRestartInfo, 
                                        aErrorStack ) == STL_SUCCESS );
    
        /**
         * Restart Information이 없을 경우에는 현재 Commit 이후부터 재시작해야 한다.
         */
        if( STL_RING_IS_EMPTY( &(sCaptureMgr->mRestartInfoList) ) == STL_TRUE )
        {
            sProgress = aProgress;
            sCaptureInfo.mRestartLSN = ZTC_INVALID_LSN;
        }
        else
        {
            sRestartInfo = STL_RING_GET_FIRST_DATA( &(sCaptureMgr->mRestartInfoList) );
            sProgress    = &sRestartInfo->mProgress;
        
            sCaptureInfo.mRestartLSN = sProgress->mLSN;
        }
        
        sCaptureInfo.mLastCommitLSN  = sCurProgress->mLSN;
        sCaptureInfo.mFileSeqNo      = sProgress->mFileSeqNo;
        sCaptureInfo.mBlockSeq       = sProgress->mBlockSeq;
        sCaptureInfo.mReadLogNo      = sProgress->mReadLogNo;
        sCaptureInfo.mRedoLogGroupId = sProgress->mRedoLogGroupId;
        sCaptureInfo.mWrapNo         = sProgress->mWrapNo;
        
        /*
          stlPrintf("===================RESTART Info============\n");
          stlPrintf("= RestartLSN    : (%ld)\n", sCaptureInfo.mRestartLSN );
          stlPrintf("= LastCommitLSN : (%ld)\n", sCaptureInfo.mLastCommitLSN );
          stlPrintf("= FileSeqNo     : (%ld)\n", sCaptureInfo.mFileSeqNo );
          stlPrintf("= WrapNo        : (%d)\n", sCaptureInfo.mWrapNo );
          stlPrintf("= ReadLogNo     : (%d)\n", sCaptureInfo.mReadLogNo );
          stlPrintf("= BlockSeq      : (%d)\n", sCaptureInfo.mBlockSeq );
          stlPrintf("= LogGroupID    : (%ld)\n", sCaptureInfo.mRedoLogGroupId );
          stlPrintf("===========================================\n");
        */
        
        /**
         * Transaction Log가 있을 경우, Slave에 보내기 전에 Commit Log를 ADD 한다.
         */
        sTransHdr.mLSN      = sCurProgress->mLSN;
        sTransHdr.mTransId  = aTransId;
        sTransHdr.mRecordId = 0;
        sTransHdr.mTablePId = ZTC_INVALID_TABLE_PHYSICAL_ID;
        sTransHdr.mTableId  = 0;
        sTransHdr.mSlotIdx  = ZTC_INVALID_SLOT_IDX;
        sTransHdr.mLogType  = ZTC_CAPTURE_TYPE_COMMIT;
        sTransHdr.mSize     = STL_SIZEOF( ztcCaptureInfo );
        
        STL_TRY( ztccAddTransDataToSlot( sTransSlotHdr, 
                                         &sTransHdr, 
                                         (stlChar*)&sCaptureInfo, 
                                         STL_SIZEOF( ztcCaptureInfo ),
                                         aErrorStack ) == STL_SUCCESS );
        
        /**
         * Committed Transaction 처리한다.
         */
        STL_TRY( ztccWriteCommittedTransToChunk( sTransSlotHdr, 
                                                 aCommitSCN,
                                                 sCommentLen,
                                                 sPtr,
                                                 aErrorStack ) == STL_SUCCESS );
        /**
         * Transaction List Memory Free & re-Initialize
         */
        STL_TRY( ztccRemoveListAtSlot( sTransSlotHdr,
                                       aErrorStack ) == STL_SUCCESS );
        
        ztccInitializeSlotHdr( sTransSlotHdr );
        sCaptureMgr->mTransSkipCount = 0;
        
    }
    else
    {
        /**
         * RestartInfo를 제거한다.
         */
        STL_TRY( ztccRemoveRestartInfo( &sTransSlotHdr->mRestartInfo, 
                                        aErrorStack ) == STL_SUCCESS );
        
        if( sCaptureMgr->mTransSkipCount++ == ZTC_SKIP_COMMIT_MAX_SIZE )
        {
            /**
             * Restart Information이 없을 경우에는 현재 Commit 이후부터 재시작해야 한다.
             */
            if( STL_RING_IS_EMPTY( &(sCaptureMgr->mRestartInfoList) ) == STL_TRUE )
            {
                sProgress = aProgress;
                sCaptureInfo.mRestartLSN = ZTC_INVALID_LSN;
            }
            else
            {
                sRestartInfo = STL_RING_GET_FIRST_DATA( &(sCaptureMgr->mRestartInfoList) );
                sProgress    = &sRestartInfo->mProgress;
        
                sCaptureInfo.mRestartLSN = sProgress->mLSN;
            }
    
            sCaptureInfo.mLastCommitLSN  = sCurProgress->mLSN;
            sCaptureInfo.mFileSeqNo      = sProgress->mFileSeqNo;
            sCaptureInfo.mBlockSeq       = sProgress->mBlockSeq;
            sCaptureInfo.mReadLogNo      = sProgress->mReadLogNo;
            sCaptureInfo.mRedoLogGroupId = sProgress->mRedoLogGroupId;
            sCaptureInfo.mWrapNo         = sProgress->mWrapNo;
        
            /**
             * Transaction Log가 있을 경우, Slave에 보내기 전에 Commit Log를 ADD 한다.
             */
            sTransHdr.mLSN      = sCurProgress->mLSN;
            sTransHdr.mTransId  = aTransId;
            sTransHdr.mRecordId = 0;
            sTransHdr.mTablePId = ZTC_INVALID_TABLE_PHYSICAL_ID;
            sTransHdr.mTableId  = 0;
            sTransHdr.mSlotIdx  = ZTC_INVALID_SLOT_IDX;
            sTransHdr.mLogType  = ZTC_CAPTURE_TYPE_COMMIT;
            sTransHdr.mSize     = STL_SIZEOF( ztcCaptureInfo );
        
            STL_TRY( ztccAddTransDataToSlot( sTransSlotHdr, 
                                             &sTransHdr, 
                                             (stlChar*)&sCaptureInfo, 
                                             STL_SIZEOF( ztcCaptureInfo ),
                                             aErrorStack ) == STL_SUCCESS );

            /**
             * Committed Transaction 처리한다.
             */
            STL_TRY( ztccWriteCommittedTransToChunk( sTransSlotHdr, 
                                                     aCommitSCN,
                                                     0,
                                                     NULL,
                                                     aErrorStack ) == STL_SUCCESS );
            sCaptureMgr->mTransSkipCount = 0;
        }
        
        /**
         * Transaction을 Begin했지만 Filtered Transaction이 없는 경우에는 File Write할 필요가 없다.
         */
        STL_TRY( ztccRemoveListAtSlot( sTransSlotHdr,
                                       aErrorStack ) == STL_SUCCESS );
        
        
        ztccInitializeSlotHdr( sTransSlotHdr );
    }
    
    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_TRANSACTION_SLOT_ID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_SLOT_ID,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztccOperateTransRollback( stlChar            * aData,
                                    stlInt64             aTransId,
                                    stlUInt64            aCommitSCN,
                                    ztcCaptureProgress * aProgress,
                                    stlErrorStack      * aErrorStack )
{
    stlInt16              sTransSlotId  = ZTC_TRANS_SLOT_ID( aTransId );
    ztcTransSlotHdr     * sTransSlotHdr = NULL;
    ztcCaptureMgr       * sCaptureMgr   = &(gMasterMgr->mCaptureMgr);
    ztcCaptureTableInfo * sTableInfo    = NULL;
    stlUInt32             sBucketSeq    = 0;
    stlBool               sExist        = STL_FALSE;
    stlChar               sFileName[ STL_MAX_FILE_PATH_LENGTH ] = {0, };
    
    STL_TRY_THROW( sTransSlotId < sCaptureMgr->mTransTableMax, RAMP_ERR_INVALID_TRANSACTION_SLOT_ID );
    
    sTransSlotHdr = &(sCaptureMgr->mTransSlotHdr[sTransSlotId]);
    
    /**
     * Transaction 중간에 Capture된 Log는 Skip 한다.
     */
    STL_TRY_THROW( sTransSlotHdr->mIsBegin    == STL_TRUE &&
                   sTransSlotHdr->mPropagated == STL_TRUE, RAMP_SUCCESS );
    
    /**
     * Rollback Log를 만났을 경우에는 Begin Log가 남겨두고 모두 Undo 되어 있어야 하지만,
     * Capture Recovery 과정에서는 UNDO 작업 없이 Rollback 되므로 반드시 LogCount가 1이 되는건 아니다.
     */
    //STL_DASSERT( sTransSlotHdr->mLogCount == 1 );
    
    if( sTransSlotHdr->mGiveUp == STL_TRUE )
    {
        STL_TRY( ztcmGetFirstHashItem( gMasterMgr->mTableHash, 
                                       &sBucketSeq, 
                                       (void**)&sTableInfo, 
                                       aErrorStack ) == STL_SUCCESS );
    
        while( sTableInfo != NULL )
        {
            /**
             * Pk가 Update되었다면 rollback을 하더라도 Giveup Count는 1이된다.
             * Update에 대한 Compensation Log가 남지 않기 때문에 -1을 처리할 수 없어서 1이된다.
             * 따라서 여기에서 초기화를 해줘야한다...
             */
            if( sTableInfo->mGiveUpTransId == aTransId &&
                sTableInfo->mGiveUpCount > 0 )
            {
                //reset TableInfo...
                sTableInfo->mGiveUpCount   = 0;
                sTableInfo->mGiveUpTransId = ZTC_INVALID_TRANSID;
            }
            
            STL_TRY( ztcmGetNextHashItem( gMasterMgr->mTableHash, 
                                          &sBucketSeq, 
                                          (void**)&sTableInfo,
                                          aErrorStack ) == STL_SUCCESS );
        }
    }
    
    sTransSlotHdr->mGiveUp = STL_FALSE;
    
    /**
     * Transaction List Memory Free & re-Initialize
     */
    STL_TRY( ztccRemoveListAtSlot( sTransSlotHdr, 
                                   aErrorStack ) == STL_SUCCESS );
    
    /**
     * Restart Info 제거
     */
    STL_TRY( ztccRemoveRestartInfo( &sTransSlotHdr->mRestartInfo, 
                                    aErrorStack ) == STL_SUCCESS );
    
    /**
     * Temp File이 있을 경우 제거
     */
    if( sTransSlotHdr->mFileLogCount > 0 )
    {
        STL_TRY( ztcmGetTransFileName( sTransSlotId,
                                       sFileName,
                                       aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( stlExistFile( sFileName,
                               &sExist,
                               aErrorStack ) == STL_SUCCESS );
        
        STL_TRY_THROW( sExist == STL_TRUE, RAMP_ERR_NOT_EXIST_FILE );
        
        STL_TRY( stlRemoveFile( sFileName,
                                aErrorStack ) == STL_SUCCESS );
    }
    
    ztccInitializeSlotHdr( sTransSlotHdr );
    
    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_TRANSACTION_SLOT_ID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_SLOT_ID,
                      NULL,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_NOT_EXIST_FILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_FILE_NAME,
                      NULL,
                      aErrorStack,
                      sFileName );
    }
    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztccOperateTransaction( stlChar             * aData,
                                  stlInt64              aLSN,
                                  stlInt64              aRecordId,
                                  stlInt64              aTransId,
                                  ztcCaptureTableInfo * aTableInfo,
                                  stlUInt16             aLogType,
                                  stlUInt16             aSize,
                                  stlErrorStack       * aErrorStack )
{
    ztcTransSlotHdr * sTransSlotHdr = NULL;
    ztcCaptureMgr   * sCaptureMgr   = &(gMasterMgr->mCaptureMgr);
    ztcTransHdr       sTransHdr;
    stlInt16          sTransSlotId  = ZTC_TRANS_SLOT_ID( aTransId );
    
    STL_TRY_THROW( sTransSlotId < sCaptureMgr->mTransTableMax, RAMP_ERR_INVALID_TRANSACTION_SLOT_ID );
    
    sTransSlotHdr = &(sCaptureMgr->mTransSlotHdr[sTransSlotId]);
    
    /**
     * Transaction 중간에 Capture된 Log는 Skip 한다.
     */
    STL_TRY_THROW( sTransSlotHdr->mIsBegin    == STL_TRUE &&
                   sTransSlotHdr->mPropagated == STL_TRUE, RAMP_SUCCESS );
    
    sTransHdr.mLSN      = aLSN;
    sTransHdr.mTransId  = aTransId;
    sTransHdr.mRecordId = aRecordId;
    sTransHdr.mTablePId = aTableInfo->mPhysicalId;
    sTransHdr.mTableId  = aTableInfo->mTableId;
    sTransHdr.mSlotIdx  = ZTC_INVALID_SLOT_IDX;
    sTransHdr.mLogType  = aLogType;
    sTransHdr.mSize     = STL_ALIGN( aSize, ZTC_MEM_ALIGN );    //Chunk에 8Byte Align에 맞도록 넣기위해서 
    
    STL_TRY( ztccAddTransDataToSlot( sTransSlotHdr, 
                                     &sTransHdr, 
                                     aData, 
                                     sTransHdr.mSize,
                                     aErrorStack ) == STL_SUCCESS );
    
    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_TRANSACTION_SLOT_ID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_SLOT_ID,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;

    return STL_FAILURE;    
}

stlStatus ztccOperateTransUndo( stlInt64        aRecordId,
                                stlInt64        aTransId,
                                stlUInt16       aLogType,
                                stlErrorStack * aErrorStack )
{
    ztcTransSlotHdr * sTransSlotHdr = NULL;
    ztcCaptureMgr   * sCaptureMgr   = &(gMasterMgr->mCaptureMgr);
    stlInt16          sTransSlotId  = ZTC_TRANS_SLOT_ID( aTransId );
    
    STL_TRY_THROW( sTransSlotId < sCaptureMgr->mTransTableMax, RAMP_ERR_INVALID_TRANSACTION_SLOT_ID );
    
    sTransSlotHdr = &(sCaptureMgr->mTransSlotHdr[sTransSlotId]);
    
    /**
     * Transaction 중간에 Capture된 Log는 Skip 한다.
     */
    STL_TRY_THROW( sTransSlotHdr->mIsBegin    == STL_TRUE &&
                   sTransSlotHdr->mPropagated == STL_TRUE, RAMP_SUCCESS );
    
    STL_TRY( ztccRemoveTransDataAtSlot( sTransSlotHdr,
                                        aRecordId,
                                        aLogType,
                                        aErrorStack ) == STL_SUCCESS );
    
    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_TRANSACTION_SLOT_ID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_SLOT_ID,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;

    return STL_FAILURE;       
}

stlStatus ztccRemoveTransDataAtSlot( ztcTransSlotHdr * aTransSlotHdr,
                                     stlInt64          aRecordId,
                                     stlUInt16         aLogType,
                                     stlErrorStack   * aErrorStack )
{
    ztcTransData   * sTransData     = NULL;
    ztcTransData   * sPrevTransData = NULL;
    ztcTransHdr    * sTransHdr      = NULL;
    stlBool          sFound         = STL_FALSE;
    stlBool          sDoRemove      = STL_FALSE;
    stlBool          sUpdSupplCheck = STL_FALSE;         //Update 관련 Supplemental Logging 제거해야하는지 여부
    stlBool          sDelSupplCheck = STL_FALSE;         //Delete 관련 Supplemental Logging 제거해야하는지 여부
    
    if( STL_RING_IS_EMPTY( &aTransSlotHdr->mTransDataList ) == STL_TRUE )
    {
        /**
         * 만약 File로 쓰여진 Transaction이 Undo를 할 경우에는 Transaction을 재구성하도록 한다.
         */
        STL_ASSERT( aTransSlotHdr->mFileLogCount > 0 );
        
        STL_TRY( ztccRebuildTransData( aTransSlotHdr,
                                       aErrorStack ) == STL_SUCCESS );   
    }
    
    STL_RING_FOREACH_ENTRY_REVERSE_SAFE( &aTransSlotHdr->mTransDataList, 
                                         sTransData, 
                                         sPrevTransData )
    {
        sTransHdr = &sTransData->mTransHdr;

        /**
         * Undo가 Full이 났을 경우에는 Supplemental Log가 기록되고, 나머지 Log는 기록이 되지 않을 수 있다.
         * 따라서 맨 마지막 Log가 Supplemental Log일 경우에는 해당 Log를 Skip 하도록 해야 한다.
         * Before Image가 마지막에 남겨지므로 해당 로그를 skip 한다.
         */
        if( sTransHdr->mLogType == ZTC_CAPTURE_TYPE_SUPPL_DEL          ||
            sTransHdr->mLogType == ZTC_CAPTURE_TYPE_SUPPL_UPD_COLS     ||
            sTransHdr->mLogType == ZTC_CAPTURE_TYPE_SUPPL_UPD_BEF_COLS ||
            sTransHdr->mLogType == ZTC_CAPTURE_TYPE_SUPPL_DEL )
        {
            STL_TRY( ztccRemoveTransDataFromChunk( aTransSlotHdr,
                                                   sTransData,
                                                   aErrorStack ) == STL_SUCCESS );
            continue;   
        }
        
        switch( aLogType )
        {
            case ZTC_CAPTURE_TYPE_UNINSERT:
            {
                if( sTransHdr->mLogType == ZTC_CAPTURE_TYPE_INSERT )
                {
                    /** Nothing To Check. */
                }
                else if( sTransHdr->mLogType == ZTC_CAPTURE_TYPE_INSERT_FOR_UPDATE )
                {
                    /**
                     * Update Supplemental Logging이 나오는 경우는 Migrate / Insert_For_Update 가 나올 때이지만,
                     * Migrate 정보를 Capture하지 않으므로, Insert_For_Update를 만났을 경우, 이전 Log가 Supplemental Log인지 확인해야 한다.
                     */
                    sUpdSupplCheck = STL_TRUE;
                }
                else
                {
                    STL_ASSERT( 0 );
                }
                break;
            }
            case ZTC_CAPTURE_TYPE_UNDELETE:
            {
                /**
                 * ZTC_CAPTURE_TYPE_UNDELETE 처리는 두가지로 나뉜다.
                 * 1. ZTC_CAPTURE_TYPE_DELETE
                 * 2. ZTC_CAPTURE_TYPE_DELETE_FOR_UPDATE
                 */
                if( sTransHdr->mLogType == ZTC_CAPTURE_TYPE_DELETE )
                {
                    /**
                     * ZTC_CAPTURE_TYPE_DELETE Transaction 구성시 Undo를 위해서만 저장하고,
                     * 실제 처리를 위해 저장되는 값은 Primary Key 정보를 갖는 ZTC_CAPTURE_TYPE_SUPPL_DEL이다.
                     * 따라서, DELETE를 위한 UNDELETE에서는 Supplemental Log를 확인하고 제거해야 한다.
                     */
                    sDelSupplCheck = STL_TRUE;
                }
                else if( sTransHdr->mLogType == ZTC_CAPTURE_TYPE_DELETE_FOR_UPDATE )
                {
                    /**
                     * DELETE FOR UPDATE 이전에 Supplemental Logging은 쌓이지 않는다.
                     * so, Nothing To Check.
                     */
                }
                else
                {
                    STL_ASSERT( 0 );
                }
                break;
            }
            case ZTC_CAPTURE_TYPE_UNUPDATE:
            {
                if( sTransHdr->mLogType == ZTC_CAPTURE_TYPE_UPDATE )
                {
                    /**
                     * UPDATE 이전 Log에 Supplemental Log가 존재할 수 있다.
                     */
                    sUpdSupplCheck = STL_TRUE;      //해당 Log 전에 Update Supplemental Log가 있을 경우 제거해야 한다.
                }
                else
                {
                    STL_ASSERT( 0 );
                }
                break;   
            }
            case ZTC_CAPTURE_TYPE_UNAPPEND_FOR_UPDATE:
            {
                if( sTransHdr->mLogType == ZTC_CAPTURE_TYPE_APPEND_FOR_UPDATE )
                {
                    /**
                     * UPDATE 이전 Log에 Supplemental Log가 존재할 수 있다.
                     */
                    sUpdSupplCheck = STL_TRUE;      //해당 Log 전에 Update Supplemental Log가 있을 경우 제거해야 한다.
                }
                else
                {
                    STL_ASSERT( 0 );
                }
                break;
            }
            default:
                STL_ASSERT( 0 );
                break;
        }
        
        STL_ASSERT( sTransHdr->mRecordId == aRecordId );
        
        STL_TRY( ztccRemoveTransDataFromChunk( aTransSlotHdr,
                                               sTransData,
                                               aErrorStack ) == STL_SUCCESS );

        sFound = STL_TRUE;
        break;
    }
    
    STL_ASSERT( sFound == STL_TRUE );
    
    if( sUpdSupplCheck == STL_TRUE )
    {
        sDoRemove = STL_FALSE;
        
        while( 1 )
        {
            /**
             * Update일 경우에는 UpdateLog 앞에 Supplemental Log가 있고, 마지막 Log일 경우에 제거한다.
             */
            STL_RING_FOREACH_ENTRY_REVERSE_SAFE( &aTransSlotHdr->mTransDataList, 
                                                 sTransData, 
                                                 sPrevTransData )
            {
                sTransHdr = &sTransData->mTransHdr;
            
                if( sTransHdr->mLogType != ZTC_CAPTURE_TYPE_SUPPL_UPD_COLS &&
                    sTransHdr->mLogType != ZTC_CAPTURE_TYPE_SUPPL_UPD )
                {
                    sDoRemove = STL_TRUE;
                    break;   
                }
            
                STL_TRY( ztccRemoveTransDataFromChunk( aTransSlotHdr,
                                                       sTransData,
                                                       aErrorStack ) == STL_SUCCESS );
            }    
            
            /**
             * Undo 해야될 내용이 File에 있어서 Undo를 못했을 경우에는 Transaction을 Rebuild 한뒤에 다시 해봐야 한다.
             */
            if( sDoRemove == STL_TRUE )
            {
                break;   //while..   
            }
            else
            {
                STL_ASSERT( aTransSlotHdr->mFileLogCount > 0 );
        
                STL_TRY( ztccRebuildTransData( aTransSlotHdr,
                                               aErrorStack ) == STL_SUCCESS );
            }
        } // end of while
    }
    else if( sDelSupplCheck == STL_TRUE )
    {
        sDoRemove = STL_FALSE;
        
        while( 1 )
        {
            /**
             * Delete의 경우 Supplemental Log 제거
             */         
            STL_RING_FOREACH_ENTRY_REVERSE_SAFE( &aTransSlotHdr->mTransDataList, 
                                                 sTransData, 
                                                 sPrevTransData )
            {
                sTransHdr = &sTransData->mTransHdr;
            
                if( sTransHdr->mLogType != ZTC_CAPTURE_TYPE_SUPPL_DEL )
                {
                    sDoRemove = STL_TRUE;
                    break;   
                }
            
                STL_TRY( ztccRemoveTransDataFromChunk( aTransSlotHdr,
                                                       sTransData,
                                                       aErrorStack ) == STL_SUCCESS );
            }
            
            /**
             * Undo 해야될 내용이 File에 있어서 Undo를 못했을 경우에는 Transaction을 Rebuild 한뒤에 다시 해봐야 한다.
             */
            if( sDoRemove == STL_TRUE )
            {
                break;   //while..   
            }
            else
            {
                STL_ASSERT( aTransSlotHdr->mFileLogCount > 0 );
        
                STL_TRY( ztccRebuildTransData( aTransSlotHdr,
                                               aErrorStack ) == STL_SUCCESS );
            }
            
        }   //end of while
    } //end of if
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;   
}

stlStatus ztccCheckRecoveryEnd( stlInt64   aLSN,
                                stlBool  * aStatus )
{
    stlInt64  sLastCommitLSN = gMasterMgr->mCaptureMgr.mRecoveryLastCommitLSN;
    stlBool   sStatus        = STL_FALSE;
    
    STL_TRY( gRunState == STL_TRUE );
    
    
    if( aLSN == sLastCommitLSN )
    {
        /** Recovery Done !! */
        sStatus = STL_TRUE;
    }
    
    *aStatus = sStatus;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztccDummyFunc( stlInt64   aLSN,
                         stlBool  * aStatus )
{
    STL_TRY( gRunState == STL_TRUE );
    
    *aStatus = STL_FALSE;   //Recovery가 아님
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztccAddTransDataToSlot( ztcTransSlotHdr * aTransSlotHdr,
                                  ztcTransHdr     * aTransHdr,
                                  stlChar         * aData,
                                  stlUInt16         aSize,
                                  stlErrorStack   * aErrorStack )
{
    STL_TRY( ztccWriteTransDataToChunk( aTransSlotHdr,
                                        aTransHdr,
                                        aData,
                                        aSize,
                                        aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
    
}


stlStatus ztccOperateDDL( stlChar             * aData,
                          stlInt64              aLSN,
                          stlInt64              aRecordId,
                          stlInt64              aTransId,
                          ztcCaptureTableInfo * aTableInfo,
                          stlUInt16             aSize,
                          stlErrorStack       * aErrorStack )
{
    ztcTransSlotHdr * sTransSlotHdr = NULL;
    ztcCaptureMgr   * sCaptureMgr    = &(gMasterMgr->mCaptureMgr);
    stlInt16          sTransSlotId  = ZTC_TRANS_SLOT_ID( aTransId );
    stlInt32          sDdlType = 0;
    
    STL_WRITE_INT32( &sDdlType, aData );
    
    STL_TRY_THROW( sTransSlotId < sCaptureMgr->mTransTableMax, RAMP_ERR_INVALID_TRANSACTION_SLOT_ID );
    STL_TRY_THROW( sDdlType < ZTC_SUPP_LOG_MAX, RAMP_ERR_INVALID_DDL_SUPPL_LOG );
    
    sTransSlotHdr = &(sCaptureMgr->mTransSlotHdr[sTransSlotId]);
    
    /**
     * Transaction 중간에 Capture된 Log는 Skip 한다.
     */
    STL_TRY_THROW( sTransSlotHdr->mIsBegin == STL_TRUE, RAMP_SUCCESS );
    
    switch( sDdlType )
    {
        case ZTC_SUPP_LOG_TRUNCATE_TABLE:
        case ZTC_SUPP_LOG_DROP_TABLE:
        case ZTC_SUPP_LOG_ALTER_TABLE_RENAME_TABLE:
        case ZTC_SUPP_LOG_ALTER_TABLE_DROP_SUPP_LOG:
        case ZTC_SUPP_LOG_ADD_COLUMN:
        case ZTC_SUPP_LOG_DROP_COLUMN:
        case ZTC_SUPP_LOG_ALTER_COLUMN_SET_NOT_NULL:
        case ZTC_SUPP_LOG_ALTER_COLUMN_DROP_NOT_NULL:
        case ZTC_SUPP_LOG_ALTER_COLUMN_SET_DATA_TYPE:
        case ZTC_SUPP_LOG_SET_UNUSED_COLUMN:
        case ZTC_SUPP_LOG_ADD_CONSTRAINT_PRIMARY_KEY:
        case ZTC_SUPP_LOG_ADD_CONSTRAINT_UNIQUE_KEY:
        case ZTC_SUPP_LOG_ADD_CONSTRAINT_CHECK_NOT_NULL:
        case ZTC_SUPP_LOG_DROP_CONSTRAINT_PRIMARY_KEY:
        case ZTC_SUPP_LOG_DROP_CONSTRAINT_UNIQUE_KEY:
        case ZTC_SUPP_LOG_DROP_CONSTRAINT_CHECK_NOT_NULL:
        case ZTC_SUPP_LOG_ALTER_CONSTRAINT_PK_DISABLE:
        case ZTC_SUPP_LOG_ALTER_CONSTRAINT_UK_DISABLE:
        case ZTC_SUPP_LOG_ALTER_CONSTRAINT_NN_DISABLE:
        case ZTC_SUPP_LOG_DROP_UNUSED_COLUMN:
        {
            /**
             * Truncate Table시에는 아래의 조건을 판단할 필요가 없다..
             */
            if( sTransSlotHdr->mGiveUp == STL_FALSE )
            {
                sTransSlotHdr->mGiveUp      = STL_TRUE;
                aTableInfo->mDdlLSN        = aLSN;
                aTableInfo->mGiveUpTransId = aTransId;
            }
            aTableInfo->mGiveUpCount++;
            break;
        }
        case ZTC_SUPP_LOG_CREATE_TABLE:
        case ZTC_SUPP_LOG_ALTER_TABLE_PHYSICAL_ATTR:
        case ZTC_SUPP_LOG_ALTER_TABLE_ADD_SUPP_LOG:
        case ZTC_SUPP_LOG_RENAME_COLUMN:
        case ZTC_SUPP_LOG_DROP_CONSTRAINT_FOREIGN_KEY:
        case ZTC_SUPP_LOG_DROP_CONSTRAINT_CHECK_CLAUSE:
        {
            /** Nothing To Do */
            break;   
        }
        case ZTC_SUPP_LOG_ALTER_TABLE_MOVE_SPACE:
        case ZTC_SUPP_LOG_ALTER_COLUMN_SET_DEFAULT:
        case ZTC_SUPP_LOG_ALTER_COLUMN_DROP_DEFAULT:
        case ZTC_SUPP_LOG_ALTER_COLUMN_ALTER_IDENTITY:
        case ZTC_SUPP_LOG_ALTER_COLUMN_DROP_IDENTITY:
        case ZTC_SUPP_LOG_ADD_CONSTRAINT_FOREIGN_KEY:
        case ZTC_SUPP_LOG_ADD_CONSTRAINT_CHECK_CLAUSE:
        case ZTC_SUPP_LOG_RENAME_CONSTRAINT:
        case ZTC_SUPP_LOG_ALTER_CONSTRAINT_PK_ENABLE:
        case ZTC_SUPP_LOG_ALTER_CONSTRAINT_PK_VALIDATE:
        case ZTC_SUPP_LOG_ALTER_CONSTRAINT_PK_NOVALIDATE:
        case ZTC_SUPP_LOG_ALTER_CONSTRAINT_UK_ENABLE:
        case ZTC_SUPP_LOG_ALTER_CONSTRAINT_UK_VALIDATE:
        case ZTC_SUPP_LOG_ALTER_CONSTRAINT_UK_NOVALIDATE:
        case ZTC_SUPP_LOG_ALTER_CONSTRAINT_FK_ENABLE:
        case ZTC_SUPP_LOG_ALTER_CONSTRAINT_FK_DISABLE:
        case ZTC_SUPP_LOG_ALTER_CONSTRAINT_FK_VALIDATE:
        case ZTC_SUPP_LOG_ALTER_CONSTRAINT_FK_NOVALIDATE:
        case ZTC_SUPP_LOG_ALTER_CONSTRAINT_NN_ENABLE:
        case ZTC_SUPP_LOG_ALTER_CONSTRAINT_NN_VALIDATE:
        case ZTC_SUPP_LOG_ALTER_CONSTRAINT_NN_NOVALIDATE:
        case ZTC_SUPP_LOG_ALTER_CONSTRAINT_CC_ENABLE:
        case ZTC_SUPP_LOG_ALTER_CONSTRAINT_CC_DISABLE:
        case ZTC_SUPP_LOG_ALTER_CONSTRAINT_CC_VALIDATE:
        case ZTC_SUPP_LOG_ALTER_CONSTRAINT_CC_NOVALIDATE:
        {
            /** Not yet Supported */
            break;   
        }
        default:
            break;
    }
    
    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_DDL_SUPPL_LOG )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_SUPPLEMENT_LOG,
                      NULL,
                      aErrorStack,
                      sDdlType );
    }
    STL_CATCH( RAMP_ERR_INVALID_TRANSACTION_SLOT_ID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_SLOT_ID,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztccOperateDDLCLR( stlChar             * aData,
                             stlInt64              aTransId,
                             ztcCaptureTableInfo * aTableInfo,
                             stlUInt16             aSize,
                             stlErrorStack       * aErrorStack )
{
    ztcTransSlotHdr * sTransSlotHdr = NULL;
    ztcCaptureMgr   * sCaptureMgr    = &(gMasterMgr->mCaptureMgr);
    stlInt16          sTransSlotId  = ZTC_TRANS_SLOT_ID( aTransId );
    stlInt32          sDdlType = 0;
    
    STL_WRITE_INT32( &sDdlType, aData );
    
    STL_TRY_THROW( sTransSlotId < sCaptureMgr->mTransTableMax, RAMP_ERR_INVALID_TRANSACTION_SLOT_ID );
    STL_TRY_THROW( sDdlType < ZTC_SUPP_LOG_MAX, RAMP_ERR_INVALID_DDL_SUPPL_LOG );
    
    sTransSlotHdr = &(sCaptureMgr->mTransSlotHdr[sTransSlotId]);
    
    /**
     * Transaction 중간에 Capture된 Log는 Skip 한다.
     */
    STL_TRY_THROW( sTransSlotHdr->mIsBegin == STL_TRUE, RAMP_SUCCESS );
    
    switch( sDdlType )
    {
        case ZTC_SUPP_LOG_TRUNCATE_TABLE:
        case ZTC_SUPP_LOG_DROP_TABLE:
        case ZTC_SUPP_LOG_ALTER_TABLE_RENAME_TABLE:
        case ZTC_SUPP_LOG_ALTER_TABLE_DROP_SUPP_LOG:
        case ZTC_SUPP_LOG_ADD_COLUMN:
        case ZTC_SUPP_LOG_DROP_COLUMN:
        case ZTC_SUPP_LOG_ALTER_COLUMN_SET_NOT_NULL:
        case ZTC_SUPP_LOG_ALTER_COLUMN_DROP_NOT_NULL:
        case ZTC_SUPP_LOG_ALTER_COLUMN_SET_DATA_TYPE:
        case ZTC_SUPP_LOG_SET_UNUSED_COLUMN:
        case ZTC_SUPP_LOG_ADD_CONSTRAINT_PRIMARY_KEY:
        case ZTC_SUPP_LOG_ADD_CONSTRAINT_UNIQUE_KEY:
        case ZTC_SUPP_LOG_ADD_CONSTRAINT_CHECK_NOT_NULL:
        case ZTC_SUPP_LOG_DROP_CONSTRAINT_PRIMARY_KEY:
        case ZTC_SUPP_LOG_DROP_CONSTRAINT_UNIQUE_KEY:
        case ZTC_SUPP_LOG_DROP_CONSTRAINT_CHECK_NOT_NULL:
        case ZTC_SUPP_LOG_ALTER_CONSTRAINT_PK_DISABLE:
        case ZTC_SUPP_LOG_ALTER_CONSTRAINT_UK_DISABLE:
        case ZTC_SUPP_LOG_ALTER_CONSTRAINT_NN_DISABLE:
        case ZTC_SUPP_LOG_DROP_UNUSED_COLUMN:
        {
            STL_TRY_THROW( aTableInfo->mGiveUpCount > 0, RAMP_ERR_INVALID_DDL_COUNT );
            aTableInfo->mGiveUpCount--;
            
            if( aTableInfo->mGiveUpCount == 0 )
            {
                aTableInfo->mDdlLSN        = ZTC_INVALID_LSN;
                aTableInfo->mGiveUpTransId = ZTC_INVALID_TRANSID;
            }
            break;
        }
        case ZTC_SUPP_LOG_CREATE_TABLE:
        case ZTC_SUPP_LOG_ALTER_TABLE_PHYSICAL_ATTR:
        case ZTC_SUPP_LOG_ALTER_TABLE_ADD_SUPP_LOG:
        case ZTC_SUPP_LOG_RENAME_COLUMN:
        case ZTC_SUPP_LOG_DROP_CONSTRAINT_FOREIGN_KEY:
        case ZTC_SUPP_LOG_DROP_CONSTRAINT_CHECK_CLAUSE:
        {
            break;   
        }
        case ZTC_SUPP_LOG_ALTER_TABLE_MOVE_SPACE:
        case ZTC_SUPP_LOG_ALTER_COLUMN_SET_DEFAULT:
        case ZTC_SUPP_LOG_ALTER_COLUMN_DROP_DEFAULT:
        case ZTC_SUPP_LOG_ALTER_COLUMN_ALTER_IDENTITY:
        case ZTC_SUPP_LOG_ALTER_COLUMN_DROP_IDENTITY:
        case ZTC_SUPP_LOG_ADD_CONSTRAINT_FOREIGN_KEY:
        case ZTC_SUPP_LOG_ADD_CONSTRAINT_CHECK_CLAUSE:
        case ZTC_SUPP_LOG_RENAME_CONSTRAINT:
        case ZTC_SUPP_LOG_ALTER_CONSTRAINT_PK_ENABLE:
        case ZTC_SUPP_LOG_ALTER_CONSTRAINT_PK_VALIDATE:
        case ZTC_SUPP_LOG_ALTER_CONSTRAINT_PK_NOVALIDATE:
        case ZTC_SUPP_LOG_ALTER_CONSTRAINT_UK_ENABLE:
        case ZTC_SUPP_LOG_ALTER_CONSTRAINT_UK_VALIDATE:
        case ZTC_SUPP_LOG_ALTER_CONSTRAINT_UK_NOVALIDATE:
        case ZTC_SUPP_LOG_ALTER_CONSTRAINT_FK_ENABLE:
        case ZTC_SUPP_LOG_ALTER_CONSTRAINT_FK_DISABLE:
        case ZTC_SUPP_LOG_ALTER_CONSTRAINT_FK_VALIDATE:
        case ZTC_SUPP_LOG_ALTER_CONSTRAINT_FK_NOVALIDATE:
        case ZTC_SUPP_LOG_ALTER_CONSTRAINT_NN_ENABLE:
        case ZTC_SUPP_LOG_ALTER_CONSTRAINT_NN_VALIDATE:
        case ZTC_SUPP_LOG_ALTER_CONSTRAINT_NN_NOVALIDATE:
        case ZTC_SUPP_LOG_ALTER_CONSTRAINT_CC_ENABLE:
        case ZTC_SUPP_LOG_ALTER_CONSTRAINT_CC_DISABLE:
        case ZTC_SUPP_LOG_ALTER_CONSTRAINT_CC_VALIDATE:
        case ZTC_SUPP_LOG_ALTER_CONSTRAINT_CC_NOVALIDATE:
        {
            /** Not yet Supported */
            break;   
        }
        default:
            break;
    }
    
    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_DDL_SUPPL_LOG )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_SUPPLEMENT_LOG,
                      NULL,
                      aErrorStack,
                      sDdlType );
    }
    STL_CATCH( RAMP_ERR_INVALID_TRANSACTION_SLOT_ID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_SLOT_ID,
                      NULL,
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_INVALID_DDL_COUNT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_DDL_OPERATION,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztccUpdateTableCommitScn4DDL( stlInt64        aTransId,
                                        stlErrorStack * aErrorStack )
{
    stlUInt32             sBucketSeq = 0;
    ztcMasterTableMeta  * sTableMeta = NULL;
    ztcCaptureTableInfo * sTableInfo = NULL;

    STL_TRY( ztcmGetFirstHashItem( gMasterMgr->mTableHash, 
                                   &sBucketSeq, 
                                   (void**)&sTableInfo, 
                                   aErrorStack ) == STL_SUCCESS );
    
    while( sTableInfo != NULL )
    {
        if( sTableInfo->mGiveUpTransId == aTransId )
        {
            STL_TRY( ztcmFindTableMeta( &sTableMeta,
                                        sTableInfo->mSchemaName,
                                        sTableInfo->mTableName,
                                        aErrorStack ) == STL_SUCCESS );
            
            STL_DASSERT( sTableMeta != NULL );
            
            /**
             * CommitScn을 최대로 하여 Capture를 수행하지 못하도록 하여 Give-up을 구현했음.
             * Sync 구현시 추가된 내용임..
             */
            sTableMeta->mCommitSCN = ZTC_GIVEUP_SCN;
            sTableInfo->mCommitSCN = ZTC_GIVEUP_SCN;
            
            STL_TRY( ztcmUpdateMasterTableMeta( sTableMeta,
                                                aErrorStack ) == STL_SUCCESS );
            
            STL_TRY( ztcmLogMsg( aErrorStack,
                                 "[CAPTURE] DDL or Primary Key Update occured At %s.%s Table.\n",
                                 sTableMeta->mSchemaName, 
                                 sTableMeta->mTableName ) == STL_SUCCESS );
        }
        
        STL_TRY( ztcmGetNextHashItem( gMasterMgr->mTableHash, 
                                      &sBucketSeq, 
                                      (void**)&sTableInfo,
                                      aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztccRebuildTransData( ztcTransSlotHdr * aTransSlotHdr,
                                stlErrorStack   * aErrorStack )
{
    stlInt32    sState        = 0;
    stlBool     sExist        = STL_FALSE;
    stlInt64    sIdx          = 0;
    stlInt64    sSkipLogCount = 0;
    stlInt64    sReadLogCount = 0;
    stlSize     sFileReadSize = 0;
    stlOffset   sTruncatePos  = 0;
    
    stlFile     sFile;
    stlChar     sFileName[ STL_MAX_FILE_PATH_LENGTH ] = {0, };
    stlChar   * sLogBody    = NULL;
    stlChar   * sLogBodyPtr = NULL;
    
    ztcTransHdr sTransHdr;
    
    STL_TRY( ztcmGetTransFileName( ZTC_TRANS_SLOT_ID( aTransSlotHdr->mTransId ),
                                   sFileName,
                                   aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlExistFile( sFileName,
                           &sExist,
                           aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( sExist == STL_TRUE, RAMP_ERR_NOT_EXIST_FILE );
    
    STL_TRY( stlOpenFile( &sFile,
                          sFileName,
                          STL_FOPEN_READ | STL_FOPEN_WRITE | STL_FOPEN_BINARY,
                          STL_FPERM_OS_DEFAULT,
                          aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( stlAllocHeap( (void**)&sLogBodyPtr,
                           ( ZTC_LOG_PIECE_MAX_SIZE + ZTC_MEM_ALIGN ),
                           aErrorStack ) == STL_SUCCESS );
    sState = 2;
    
    sLogBody = (void*)STL_ALIGN( (stlUInt64)sLogBodyPtr, ZTC_MEM_ALIGN );
    
    if( aTransSlotHdr->mFileLogCount > ZTC_TRANS_READ_LOG_COUNT )
    {
        /**
         * File에 저장된 Log중에 ZTC_TRANS_READ_LOG_COUNT 만큼만 
         * Rebuild 하기 위해 앞 부분의 Log를 Skip 한다.
         */
        sSkipLogCount = aTransSlotHdr->mFileLogCount - ZTC_TRANS_READ_LOG_COUNT;
        
        while( 1 )
        {
            /**
             * 성능 튜닝해야함!!!
             */ 
            // TransHeader 읽기
            STL_TRY( stlReadFile( &sFile,
                                  &sTransHdr,
                                  STL_SIZEOF( ztcTransHdr ),
                                  &sFileReadSize,
                                  aErrorStack ) == STL_SUCCESS );
            
            STL_TRY_THROW( STL_SIZEOF( ztcTransHdr ) == sFileReadSize, RAMP_ERR_INVALID_PROCESS );
            STL_TRY_THROW( sTransHdr.mSize <= ZTC_LOG_PIECE_MAX_SIZE, RAMP_ERR_INVALID_PROCESS );
            
            // TransBody 읽기
            STL_TRY( stlReadFile( &sFile,
                                  sLogBody,
                                  sTransHdr.mSize,
                                  &sFileReadSize,
                                  aErrorStack ) == STL_SUCCESS );
            
            STL_TRY_THROW( sTransHdr.mSize == sFileReadSize, RAMP_ERR_INVALID_PROCESS );
            
            sSkipLogCount--;
            if( sSkipLogCount == 0 )
            {
                STL_TRY( stlSeekFile( &sFile,
                                      STL_FSEEK_CUR,
                                      &sTruncatePos,
                                      aErrorStack ) == STL_SUCCESS );
                break;   
            }
        }
        
    }
    
    sReadLogCount = STL_MIN( ZTC_TRANS_READ_LOG_COUNT, aTransSlotHdr->mFileLogCount );
    
    /**
     * 현재 Transaction Slot이 Rebuild임을 세팅하여 
     * Memory가 없을 시에 File로 저장하지 않도록 한다.
     * 다른 Transaction이 파일로 저장되도록 한다...
     */
    aTransSlotHdr->mIsRebuild = STL_TRUE;
    
    for( sIdx = 0; sIdx < sReadLogCount; sIdx++ )
    {
        STL_TRY( stlReadFile( &sFile,
                              &sTransHdr,
                              STL_SIZEOF( ztcTransHdr ),
                              &sFileReadSize,
                              aErrorStack ) == STL_SUCCESS );
            
        STL_TRY_THROW( STL_SIZEOF( ztcTransHdr ) == sFileReadSize, RAMP_ERR_INVALID_PROCESS );
        STL_TRY_THROW( sTransHdr.mSize <= ZTC_LOG_PIECE_MAX_SIZE, RAMP_ERR_INVALID_PROCESS );
            
        // TransBody 읽기
        STL_TRY( stlReadFile( &sFile,
                              sLogBody,
                              sTransHdr.mSize,
                              &sFileReadSize,
                              aErrorStack ) == STL_SUCCESS );
        
        STL_TRY_THROW( sTransHdr.mSize == sFileReadSize, RAMP_ERR_INVALID_PROCESS );
        
        STL_TRY( ztccAddTransDataToSlot( aTransSlotHdr, 
                                         &sTransHdr, 
                                         sLogBody, 
                                         sTransHdr.mSize,
                                         aErrorStack ) == STL_SUCCESS );
        aTransSlotHdr->mFileLogCount--;
        aTransSlotHdr->mFileSize = aTransSlotHdr->mFileSize - STL_SIZEOF( ztcTransHdr ) - sTransHdr.mSize;
    }
    
    aTransSlotHdr->mIsRebuild = STL_FALSE;
    
    if( sTruncatePos > 0 )
    {
        STL_TRY( stlTruncateFile( &sFile,
                                  sTruncatePos,
                                  aErrorStack ) == STL_SUCCESS );
    }
    
    sState = 1;
    stlFreeHeap( sLogBodyPtr );
    
    sState = 0;
    STL_TRY( stlCloseFile( &sFile,
                           aErrorStack ) == STL_SUCCESS );
    
    if( aTransSlotHdr->mFileLogCount == 0 )
    {
        STL_TRY( stlRemoveFile( sFileName,
                                aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_NOT_EXIST_FILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_FILE_NAME,
                      NULL,
                      aErrorStack,
                      sFileName );
    }
    STL_CATCH( RAMP_ERR_INVALID_PROCESS )
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
            stlFreeHeap( sLogBodyPtr );
        case 1:
            (void) stlCloseFile( &sFile,
                                 aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}
/** @} */

