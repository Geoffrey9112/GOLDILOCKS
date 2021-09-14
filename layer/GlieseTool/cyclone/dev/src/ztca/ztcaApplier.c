/*******************************************************************************
 * ztcaApplier.c
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
 * @file ztcaApplier.c
 * @brief GlieseTool Cyclone Applier Routines
 */

#include <goldilocks.h>
#include <ztc.h>

extern stlBool        gRunState;
extern ztcSlaveMgr  * gSlaveMgr;

stlStatus ztcaProcessFlow( ztcApplierMgr * aApplierMgr,
                           stlErrorStack * aErrorStack )
{
    ztcTransHdr           sTransHdr;
    ztcTransHdr           sTmpTransHdr;
    stlChar             * sTransData    = NULL;
    stlChar             * sTransDataPtr = NULL;
    ztcApplierTableInfo * sTableInfo    = NULL;
    ztcCaptureInfo      * sCaptureInfo  = NULL;
    ztcStmtInfo         * sStmtInfo     = NULL;
    
    stlBool               sIsMasterRow     = STL_FALSE;
    stlBool               sDoRead          = STL_FALSE;
    stlBool               sNeedPrepare     = STL_FALSE;
    stlInt32              sState           = 0;
    stlInt32              sFirstColOrd     = 0;
    stlInt64              sCurrentIdx      = 0;
    stlInt64              sApplyCommitSize = ztcmGetApplyCommitSize();
    
    STL_TRY( stlAllocHeap( (void**)&sTransDataPtr,
                           ( ZTC_LOG_PIECE_MAX_SIZE + ZTC_MEM_ALIGN ),
                           aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    sTransData = (void*)STL_ALIGN( (stlUInt64)sTransDataPtr, ZTC_MEM_ALIGN );
    
    /**
     * Transaction Header 읽기
     */
    while( 1 )
    {
        STL_TRY( gRunState == STL_TRUE );
        
        sTableInfo = NULL;
        
        if( sDoRead == STL_FALSE )
        {
            /**
             * TransHeader
             */
            STL_TRY( ztcaReadDataFromChunk( aApplierMgr,
                                            (stlChar*)&sTransHdr,
                                            STL_SIZEOF( ztcTransHdr ),
                                            aErrorStack ) == STL_SUCCESS );
        }
        
        sDoRead = STL_FALSE;   
        
        switch( sTransHdr.mLogType )
        {
            case ZTC_CAPTURE_TYPE_BEGIN:
            {
                STL_TRY( ztcaReadDataFromChunk( aApplierMgr,
                                                sTransData,
                                                sTransHdr.mSize,
                                                aErrorStack ) == STL_SUCCESS );
                
                /**
                 * 해당 Tx를 수행하기전에 기존에 수행된 Tx를 Commit 해야할지 여부를 결정한다.
                 */
                if( sTransHdr.mTxBeginFlag == STL_TRUE )
                {
                    /**
                     * Active Tx가 있을 경우에만 Commit을 수행한다.
                     */
                    if( aApplierMgr->mAnalyzeState               == STL_TRUE &&
                        aApplierMgr->mApplyCountAfterCommit      == 0 &&
                        aApplierMgr->mRestartInfo.mFileSeqNo     != ZTC_INVALID_FILE_SEQ_NO &&
                        aApplierMgr->mRestartInfo.mLastCommitLSN != ZTC_INVALID_LSN )
                    {
                        STL_TRY( ztcdInsertCommitState( aApplierMgr,
                                                        aApplierMgr->mDbcHandle,
                                                        aApplierMgr->mStateStmtHandle,
                                                        &aApplierMgr->mRestartInfo,
                                                        aErrorStack ) == STL_SUCCESS );
                    }
                }
                break;
            }   
            case ZTC_CAPTURE_TYPE_COMMIT:
            {
                STL_TRY( ztcaReadDataFromChunk( aApplierMgr,
                                                sTransData,
                                                sTransHdr.mSize,
                                                aErrorStack ) == STL_SUCCESS );
                
                sCaptureInfo = (ztcCaptureInfo*)sTransData;
                
                /**
                 * COMMIT이 발생되었다고 무조건 COMMIT하지 않는다.
                 * 기존에 수행되었던 Operation이 ZTC_APPLIER_TRANS_COMMIT_MAX_SIZE 보다 클 경우에만 COMMIT을 수행하고
                 * 그렇지 않을 경우에는, 한꺼번에 모아서 COMMIT을 수행하도록 한다.
                 * 이는 INSERT -> COMMIT -> INSERT -> COMMIT 의 처리를 INSERT -> INSERT -> COMMIT으로 바꾸는 과정이다.
                 * 이 처리가 중요한 이유는 COMMIT을 할 경우에 Restart정보를 함께 Update하는데 
                 * Master에 없던 부하가 Slave에 추가가 되기 때문에 이렇게 처리하도록 한다.
                 */
                
                if( aApplierMgr->mApplyCount >= sApplyCommitSize ||
                    sTransHdr.mTxCommitFlag == STL_TRUE )
                {
                    STL_TRY( ztcdInsertCommitState( aApplierMgr,
                                                    aApplierMgr->mDbcHandle,
                                                    aApplierMgr->mStateStmtHandle,
                                                    sCaptureInfo,
                                                    aErrorStack ) == STL_SUCCESS );
                }
                else
                {
                    stlMemcpy( &aApplierMgr->mRestartInfo, sCaptureInfo, STL_SIZEOF( ztcCaptureInfo ) );
                    aApplierMgr->mApplyCountAfterCommit = 0;
                }
                break;
            }   
            case ZTC_CAPTURE_TYPE_INSERT:
            {
                STL_TRY( ztcaFindApplierTableInfo( aApplierMgr,
                                                   &sTableInfo,
                                                   sTransHdr.mTableId,
                                                   aErrorStack ) == STL_SUCCESS );
                
                if( sTableInfo != NULL )
                {
                    /**
                     * Array로 Execute 되어야 하는지를 판단한다.
                     */
                    if( ztcaCheckNeedExecute( aApplierMgr,
                                              sTableInfo,
                                              ZTC_CAPTURE_TYPE_INSERT ) == STL_TRUE )
                    {
                        STL_TRY( ztcaDoPreviousExecute( aApplierMgr,
                                                        aErrorStack ) == STL_SUCCESS );
                    }
                    
                    sStmtInfo = &sTableInfo->mStmtInfo;
                    
                    /**
                     * Record 분석중임을 세팅한다.
                     */
                    aApplierMgr->mAnalyzeState = STL_FALSE;
                    
                    /**
                     * 하나의 Record를 분석한다.
                     * MasterRow가 맨마지막에 나온다.
                     */
                    while( 1 )
                    {
                        STL_ASSERT( sTransHdr.mSize <= ZTC_LOG_PIECE_MAX_SIZE );
                    
                        /**
                         * TransData
                         */
                        STL_TRY( ztcaReadDataFromChunk( aApplierMgr,
                                                        sTransData,
                                                        sTransHdr.mSize,
                                                        aErrorStack ) == STL_SUCCESS );
                    
                        STL_TRY( ztcaIsMasterRow( sTransData, 
                                                  &sIsMasterRow,
                                                  &sFirstColOrd,
                                                  aErrorStack ) == STL_SUCCESS );
                    
                        STL_TRY( ztcaAnalyzeInsert( aApplierMgr,
                                                    sTableInfo,
                                                    sTransData, 
                                                    sTransHdr.mSize, 
                                                    sStmtInfo->mInsertIdx,
                                                    sFirstColOrd,
                                                    aErrorStack ) == STL_SUCCESS );
                        
                        if( sIsMasterRow == STL_TRUE )
                        {
                            break;
                        }
                        
                        /**
                         * TransHeader
                         */
                        STL_TRY( ztcaReadDataFromChunk( aApplierMgr,
                                                        (stlChar*)&sTransHdr,
                                                        STL_SIZEOF( ztcTransHdr ),
                                                        aErrorStack ) == STL_SUCCESS );
                        
                        STL_ASSERT( sTransHdr.mTableId == sTableInfo->mTableId );
                    }  //end of While
                    
                    /**
                     * 하나의 Record 분석이 끝났음을 세팅한다.
                     */
                    aApplierMgr->mAnalyzeState = STL_TRUE;
                    
                    sStmtInfo->mInsertIdx++;
                    aApplierMgr->mPrevTableInfo = sTableInfo;
                    aApplierMgr->mPrevTransType = ZTC_CAPTURE_TYPE_INSERT;
                    
                    /**
                     * 해당 Record가 처리되고 있음을 Record Item에 세팅
                     */
                    ztcaAddRecState( aApplierMgr, sTransHdr.mSlotIdx );
                    
                    if( sStmtInfo->mInsertIdx == aApplierMgr->mArraySize )
                    {
                        STL_TRY( ztcdDoInsertNExecute( aApplierMgr,
                                                       sTableInfo, 
                                                       aErrorStack ) == STL_SUCCESS );
                        
                        sStmtInfo->mInsertIdx = 0;
                    }
                }
                break;
            }
            case ZTC_CAPTURE_TYPE_SUPPL_DEL:
            {
                STL_TRY( ztcaFindApplierTableInfo( aApplierMgr,
                                                   &sTableInfo,
                                                   sTransHdr.mTableId,
                                                   aErrorStack ) == STL_SUCCESS );
                
                if( sTableInfo != NULL )
                {
                    if( ztcaCheckNeedExecute( aApplierMgr,
                                              sTableInfo,
                                              ZTC_CAPTURE_TYPE_DELETE ) == STL_TRUE )
                    {
                        STL_TRY( ztcaDoPreviousExecute( aApplierMgr,
                                                        aErrorStack ) == STL_SUCCESS );
                    }
                    
                    /**
                     * Record 분석중임을 세팅한다.
                     */
                    aApplierMgr->mAnalyzeState = STL_FALSE;
                    
                    sStmtInfo = &sTableInfo->mStmtInfo;
                    
                    STL_ASSERT( sTransHdr.mSize <= ZTC_LOG_PIECE_MAX_SIZE );
                
                    /**
                     * TransData
                     */
                    STL_TRY( ztcaReadDataFromChunk( aApplierMgr,
                                                    sTransData,
                                                    sTransHdr.mSize,
                                                    aErrorStack ) == STL_SUCCESS );
                    
                    /**
                     * Delete Query의 Where절에 사용될 PK Value를 얻어온다.
                     */ 
                    STL_TRY( ztcaAnalyzeSuppLog( sTableInfo,
                                                 sTransData, 
                                                 sTransHdr.mSize,
                                                 sStmtInfo->mDeleteIdx,
                                                 aErrorStack ) == STL_SUCCESS );
                    
                    /**
                     * 하나의 Record 분석이 끝났음을 세팅한다.
                     */
                    aApplierMgr->mAnalyzeState = STL_TRUE;
                    
                    sStmtInfo->mDeleteIdx++;
                    aApplierMgr->mPrevTableInfo = sTableInfo;
                    aApplierMgr->mPrevTransType = ZTC_CAPTURE_TYPE_DELETE;
                    
                    /**
                     * 해당 Record가 처리되고 있음을 Record Item에 세팅
                     */
                    ztcaAddRecState( aApplierMgr, sTransHdr.mSlotIdx );
                    
                    if( sStmtInfo->mDeleteIdx == aApplierMgr->mArraySize )
                    {
                        STL_TRY( ztcdDoDeleteNExecute( aApplierMgr,
                                                       sTableInfo,
                                                       aErrorStack ) == STL_SUCCESS );
                        
                        sStmtInfo->mDeleteIdx = 0;
                    }
                }
                
                break;
            }
            case ZTC_CAPTURE_TYPE_DELETE:
            {
                /**
                 * Capture에서 Undo를 위해서 저장한 데이터이며 분석할 필요 없음
                 * 실제 분석할 내용은 Delete Supplemental Log이다.
                 */
                break;   
            }
            case ZTC_CAPTURE_TYPE_SUPPL_UPD:
            {
                STL_TRY( ztcaFindApplierTableInfo( aApplierMgr,
                                                   &sTableInfo,
                                                   sTransHdr.mTableId,
                                                   aErrorStack ) == STL_SUCCESS );
                
                if( sTableInfo != NULL )
                {
                    if( ztcaCheckNeedExecute( aApplierMgr,
                                              sTableInfo,
                                              ZTC_CAPTURE_TYPE_UPDATE ) == STL_TRUE )
                    {
                        STL_TRY( ztcaDoPreviousExecute( aApplierMgr,
                                                        aErrorStack ) == STL_SUCCESS );
                    }
                    
                    /**
                     * Record 분석중임을 세팅한다.
                     */
                    aApplierMgr->mAnalyzeState = STL_FALSE;
                    
                    sStmtInfo = &sTableInfo->mStmtInfo;
                    
                    STL_ASSERT( sTransHdr.mSize <= ZTC_LOG_PIECE_MAX_SIZE );

                    /**
                     * TransData
                     */
                    STL_TRY( ztcaReadDataFromChunk( aApplierMgr,
                                                    sTransData,
                                                    sTransHdr.mSize,
                                                    aErrorStack ) == STL_SUCCESS );
                    
                    /**
                     * Update 관련 PrimaryKey 정보를 얻어온다.
                     */
                    STL_TRY( ztcaAnalyzeSuppLog( sTableInfo,
                                                 sTransData, 
                                                 sTransHdr.mSize, 
                                                 sStmtInfo->mUpdateIdx,
                                                 aErrorStack ) == STL_SUCCESS );
                    
                    /**
                     * 해당 Record가 처리되고 있음을 Record Item에 세팅
                     */
                    ztcaAddRecState( aApplierMgr, sTransHdr.mSlotIdx );
                    
                    /**
                     * Updated Column 정보 얻어오기
                     */
                    while( 1 )
                    {
                        /**
                         * TransHeader
                         */
                        STL_TRY( ztcaReadDataFromChunk( aApplierMgr,
                                                        (stlChar*)&sTransHdr,
                                                        STL_SIZEOF( ztcTransHdr ),
                                                        aErrorStack ) == STL_SUCCESS );

                        if( sTransHdr.mLogType != ZTC_CAPTURE_TYPE_SUPPL_UPD_COLS )
                        {
                            break;
                        }
                        
                        STL_ASSERT( sTransHdr.mTableId == sTableInfo->mTableId );
                        
                        /**
                         * TransData
                         */
                        STL_TRY( ztcaReadDataFromChunk( aApplierMgr,
                                                        sTransData,
                                                        sTransHdr.mSize,
                                                        aErrorStack ) == STL_SUCCESS );
                        
                        /**
                         * Array Update를 하기위해서는 기존과 동일한 Column이 변경되어야 한다.
                         * Insert,Delete의 경우에는 미리 Prepare되었기 때문에 상관없지만, Update의 경우에는 변경되는 Column이 다를 경우에는
                         * Prepare를 다시해야 한다. 이 경우에 기존에 Array에 들어가 있는 값들은 Execute되어야 한다.
                         * 그리고, Execute 이후에 처음에 새로운 Column을 Array에 넣을때 Prepare가 필요한 경우에는 NeedUpdatePrepare를 TRUE로 세팅해줘야 한다.
                         */
                        if( sStmtInfo->mUpdateIdx == 0 )
                        {
                            STL_TRY( ztcaAnalyzeSuppUpdateColLog( sTableInfo,
                                                                  sTransData, 
                                                                  sTransHdr.mSize, 
                                                                  &sNeedPrepare,
                                                                  aErrorStack ) == STL_SUCCESS );
                
                            sTableInfo->mNeedUpdatePrepare = sNeedPrepare;
                        }
                        else 
                        {
                            STL_TRY( ztcaValidateSuppUpdateColLog( sTableInfo,
                                                                   sTransData, 
                                                                   sTransHdr.mSize, 
                                                                   &sNeedPrepare,
                                                                   aErrorStack ) == STL_SUCCESS );
                            
                            if( sNeedPrepare == STL_TRUE )
                            {
                                /**
                                 * Execute 한 이후에 미리 읽은 PK Value를 Array 0에 Copy 해야 한다.
                                 */
                                sCurrentIdx = sStmtInfo->mUpdateIdx;
                                
                                STL_TRY( ztcdDoUpdateNExecute( aApplierMgr,
                                                               sTableInfo, 
                                                               aErrorStack ) == STL_SUCCESS );
                                
                                sStmtInfo->mUpdateIdx = 0;
                                
                                /**
                                 * TODO : Array 처리시 Reset 및 초기화를 해줘야 한다.
                                 */
                                STL_TRY( ztcaMovePrimaryValue( sTableInfo,
                                                               sCurrentIdx,
                                                               aErrorStack ) == STL_SUCCESS );
                                
                                STL_TRY( ztcaAnalyzeSuppUpdateColLog( sTableInfo,
                                                                      sTransData, 
                                                                      sTransHdr.mSize, 
                                                                      &sNeedPrepare,
                                                                      aErrorStack ) == STL_SUCCESS );
                                
                                sTableInfo->mNeedUpdatePrepare = STL_TRUE;
                            }
                        }
                    }
                    
                    /**
                     * UPDATE 관련 LOG 분석
                     */
                    while( 1 )
                    {
                        /**
                         * TransData
                         */
                        STL_TRY( ztcaReadDataFromChunk( aApplierMgr,
                                                        sTransData,
                                                        sTransHdr.mSize,
                                                        aErrorStack ) == STL_SUCCESS );
                        
                        switch( sTransHdr.mLogType )
                        {
                            case ZTC_CAPTURE_TYPE_DELETE_FOR_UPDATE:
                            {
                                /**
                                 * DELETE_FOR_UPDATE는 Capture에서 Undo 처리할때 필요하기 때문에 필요하긴 하다.
                                 * Applier에서는 따로 처리하지 않는다.
                                 */ 
                                break;   
                            }
                            case ZTC_CAPTURE_TYPE_UPDATE:
                            {
                                STL_TRY( ztcaAnalyzeUpdate( aApplierMgr,
                                                            sTableInfo,
                                                            sTransData,
                                                            sTransHdr.mSize,
                                                            sStmtInfo->mUpdateIdx,
                                                            aErrorStack ) == STL_SUCCESS );
                                break;
                            }
                            case ZTC_CAPTURE_TYPE_INSERT_FOR_UPDATE:
                            {
                                STL_TRY( ztcaAnalyzeInsert4Update( aApplierMgr,
                                                                   sTableInfo,
                                                                   sTransData,
                                                                   sTransHdr.mSize,
                                                                   sStmtInfo->mUpdateIdx,
                                                                   aErrorStack ) == STL_SUCCESS );
                                break;
                            }
                            case ZTC_CAPTURE_TYPE_APPEND_FOR_UPDATE:
                            {
                                STL_TRY( ztcaAnalyzeAppend4Update( aApplierMgr,
                                                                   sTableInfo,
                                                                   sTransData,
                                                                   sTransHdr.mSize,
                                                                   sStmtInfo->mUpdateIdx,
                                                                   aErrorStack ) == STL_SUCCESS );
                                break;
                            }
                            case ZTC_CAPTURE_TYPE_SUPPL_UPD_BEF_COLS:
                            {
                                STL_TRY( ztcaAnalyzeSuppUpdateBefColLog( aApplierMgr,
                                                                         sTableInfo,
                                                                         sTransData,
                                                                         sTransHdr.mSize,
                                                                         sStmtInfo->mUpdateIdx,
                                                                         aErrorStack ) == STL_SUCCESS );
                                break;
                            }
                            default:
                                //assert?
                                break;
                        }
                        
                        /**
                         * TransHeader
                         */
                        STL_TRY( ztcaReadDataFromChunk( aApplierMgr,
                                                        (stlChar*)&sTmpTransHdr,
                                                        STL_SIZEOF( ztcTransHdr ),
                                                        aErrorStack ) == STL_SUCCESS );
                        
                        if( sTmpTransHdr.mLogType != ZTC_CAPTURE_TYPE_DELETE_FOR_UPDATE &&
                            sTmpTransHdr.mLogType != ZTC_CAPTURE_TYPE_UPDATE &&
                            sTmpTransHdr.mLogType != ZTC_CAPTURE_TYPE_INSERT_FOR_UPDATE &&
                            sTmpTransHdr.mLogType != ZTC_CAPTURE_TYPE_APPEND_FOR_UPDATE &&
                            sTmpTransHdr.mLogType != ZTC_CAPTURE_TYPE_SUPPL_UPD_BEF_COLS )
                        {
                            sDoRead = STL_TRUE;
                            
                            /**
                             * 해당 Record가 처리되고 있음을 Record Item에 세팅
                             */
                            if( sTransHdr.mSlotIdx != ZTC_INVALID_SLOT_IDX )
                            {
                                ztcaAddRecState( aApplierMgr, sTransHdr.mSlotIdx );
                            }
                        
                            stlMemcpy( (stlChar*)&sTransHdr, (stlChar*)&sTmpTransHdr, STL_SIZEOF( ztcTransHdr ) );
                            break;   
                        }
                        else
                        {
                            stlMemcpy( (stlChar*)&sTransHdr, (stlChar*)&sTmpTransHdr, STL_SIZEOF( ztcTransHdr ) );
                        }
                    }
                    
                    /**
                     * 하나의 Record 분석이 끝났음을 세팅한다.
                     */
                    aApplierMgr->mAnalyzeState = STL_TRUE;
                    
                    sStmtInfo->mUpdateIdx++;
                    aApplierMgr->mPrevTableInfo = sTableInfo;
                    aApplierMgr->mPrevTransType = ZTC_CAPTURE_TYPE_UPDATE;
                    
                    if( sStmtInfo->mUpdateIdx == aApplierMgr->mArraySize )
                    {
                        STL_TRY( ztcdDoUpdateNExecute( aApplierMgr,
                                                       sTableInfo, 
                                                       aErrorStack ) == STL_SUCCESS );
                        
                        sStmtInfo->mUpdateIdx = 0;
                    }
                } //end of if
                break;   
            }
            default:
                break;
        }

    }// end of while
    
    sState = 0;
    stlFreeHeap( sTransDataPtr );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    switch( sState )
    {
        case 1:
            stlFreeHeap( sTransDataPtr );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlBool ztcaCheckNeedExecute( ztcApplierMgr       * aApplierMgr,
                              ztcApplierTableInfo * aTableInfo,
                              stlInt32              aTransType )
{
    stlBool sNeedExecute = STL_FALSE;
    
    if( aApplierMgr->mPrevTableInfo != NULL )
    {
        if( aApplierMgr->mPrevTableInfo != aTableInfo )
        {
            sNeedExecute = STL_TRUE;
        }
        else
        {
            if( aApplierMgr->mPrevTransType != aTransType )
            {
                sNeedExecute = STL_TRUE;
            }
        }
    }
    
    return sNeedExecute;
}

stlStatus ztcaDoPreviousExecute( ztcApplierMgr * aApplierMgr,
                                 stlErrorStack * aErrorStack )
{
    ztcApplierTableInfo * sTableInfo = NULL;

    if( aApplierMgr->mPrevTableInfo != NULL )
    {
        sTableInfo = aApplierMgr->mPrevTableInfo;
        
        switch( aApplierMgr->mPrevTransType )
        {
            case ZTC_CAPTURE_TYPE_INSERT:
            {
                STL_TRY( ztcdDoInsertNExecute( aApplierMgr,
                                               sTableInfo,
                                               aErrorStack ) == STL_SUCCESS );
                
                sTableInfo->mStmtInfo.mInsertIdx = 0;
                break;
            }
            case ZTC_CAPTURE_TYPE_DELETE:
            {
                STL_TRY( ztcdDoDeleteNExecute( aApplierMgr,
                                               sTableInfo,
                                               aErrorStack ) == STL_SUCCESS );
                
                sTableInfo->mStmtInfo.mDeleteIdx = 0;
                break;
            }
            case ZTC_CAPTURE_TYPE_UPDATE:
            {
                STL_TRY( ztcdDoUpdateNExecute( aApplierMgr,
                                               sTableInfo,
                                               aErrorStack ) == STL_SUCCESS );
                
                sTableInfo->mStmtInfo.mUpdateIdx = 0;
                break;
            }
            default:
                break;
        }
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztcaVacateSlot( ztcApplierMgr  * aApplierMgr,
                          stlErrorStack  * aErrorStack )
{
    ztcApplierRecItem * sAppItem  = NULL;
    ztcRecStateItem   * sItem     = NULL;
    
    /**
     * MASTER의 현재 CAPTURE상황을 저장하기 위한 Transaction의 경우에는 COMMIT만을 포함하고 있다. (Restart를 최근의 값으로 하기위해서..)
     * 따라서 매달려있는 Record List가 없다.
     */
    STL_TRY_THROW( STL_RING_IS_EMPTY( &aApplierMgr->mRecordList ) == STL_FALSE, RAMP_SUCCESS );
    
    while( 1 )
    {
        sAppItem = STL_RING_GET_FIRST_DATA( &aApplierMgr->mRecordList );
        
        sItem = &gSlaveMgr->mItemArr[ sAppItem->mIdx ];
        //stlPrintf("VACATE SLOT[APPID:%d][SLOTID:%d][Ref:%ld][Dec:%ld]\n", aApplierMgr->mAppId, sAppItem->mIdx, sItem->mRefCount, sAppItem->mRefCount );
        
        ztcmAcquireSpinLock( &(sItem->mSpinLock) );
        
        STL_ASSERT( sItem->mRefCount >= sAppItem->mRefCount );
        
        sItem->mRefCount -= sAppItem->mRefCount;
        
        if( sItem->mRefCount == 0 )
        {
            sItem->mAppId = ZTC_INVALID_APPLIER_ID;
        }
        
        ztcmReleaseSpinLock( &(sItem->mSpinLock) );
        
        sAppItem->mRefCount = 0;
        
        STL_RING_UNLINK( &aApplierMgr->mRecordList, sAppItem );
        
        if( STL_RING_IS_EMPTY( &aApplierMgr->mRecordList ) == STL_TRUE )
        {
            break;
        }
    }
    
    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;
}


void ztcaAddRecState( ztcApplierMgr * aApplierMgr,
                      stlInt32        aSlotIdx )
{
    ztcApplierRecItem * sItem = NULL;
    
    STL_ASSERT( aSlotIdx < ZTC_DISTRIBUTOR_SLOT_SIZE );
    
    sItem = &aApplierMgr->mItemArr[ aSlotIdx ];
    
    if( sItem->mRefCount == 0 )
    {
        STL_RING_ADD_LAST( &aApplierMgr->mRecordList, 
                           sItem );   
    }
    
    sItem->mRefCount++;
}

/** @} */

