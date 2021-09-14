/*******************************************************************************
 * ztccGeneral.c
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
 * @file ztccGeneral.c
 * @brief GlieseTool Cyclone Capture General Routines
 */

#include <goldilocks.h>
#include <ztc.h>

extern ztcMasterMgr * gMasterMgr;
extern stlBool        gRunState;

stlStatus ztccInitializeCapture( stlErrorStack * aErrorStack )
{
    stlInt32        sTransTableSize = ztcmGetTransTableSize();
    stlInt64        sIdx            = 0;
    stlInt32        sState          = 0;
    stlInt64        sSortAreaSize   = ztcmGetTransSortAreaSize();
    stlInt64        sChunkCount     = 0;
    ztcCaptureMgr * sCaptureMgr     = &(gMasterMgr->mCaptureMgr);
    ztcChunkItem  * sChunkItem      = NULL;
    
    /**
     * Transaction Sort Area 생성
     */ 
    STL_TRY( stlCreateRegionAllocator( &(sCaptureMgr->mRegionMem),
                                       ZTC_REGION_INIT_SIZE,
                                       ZTC_REGION_NEXT_SIZE,
                                       aErrorStack ) == STL_SUCCESS );
    
    sState = 1;
    
    /**
     * LogBody Buffer 할당
     */
    STL_TRY( stlAllocRegionMem( &(sCaptureMgr->mRegionMem),
                                ZTC_LOGBODY_MAX_SIZE,
                                (void**)&sCaptureMgr->mLogBodyBuffer,
                                aErrorStack ) == STL_SUCCESS );
    
    /**
     * SortArea의 Header를 할당 한다.
     */
    STL_TRY( stlAllocRegionMem( &(sCaptureMgr->mRegionMem),
                                STL_SIZEOF( ztcTransSlotHdr ) * sTransTableSize,
                                (void**)&sCaptureMgr->mTransSlotHdr,
                                aErrorStack ) == STL_SUCCESS );
    
    sCaptureMgr->mTransTableMax = sTransTableSize;
    
    /**
     * Transaction 정렬을 위한 Slot을 할당한다.
     */
    for( sIdx = 0; sIdx < sTransTableSize; sIdx++ )
    {
        STL_RING_INIT_HEADLINK( &sCaptureMgr->mTransSlotHdr[sIdx].mTransDataList,
                                STL_OFFSETOF( ztcTransData, mLink ) );
        
        STL_RING_INIT_HEADLINK( &sCaptureMgr->mTransSlotHdr[sIdx].mTransChunkList,
                                STL_OFFSETOF( ztcChunkItem, mLink ) );
        
        STL_RING_INIT_DATALINK( &sCaptureMgr->mTransSlotHdr[sIdx].mRestartInfo,
                                STL_OFFSETOF( ztcRestartInfo, mLink ) );
        
        ztccInitializeSlotHdr( &sCaptureMgr->mTransSlotHdr[sIdx] );
    }
    
    /**
     * Transaction 정렬을 위한 Chunk를 할당한다.
     */
    STL_RING_INIT_HEADLINK( &sCaptureMgr->mTransChunkFreeList,
                            STL_OFFSETOF( ztcChunkItem, mLink ) );
    
    sChunkCount = sSortAreaSize / ZTC_TRANS_CHUNK_ITEM_SIZE;
            
    for( sIdx = 0; sIdx < sChunkCount ; sIdx++ )
    {
        STL_TRY( stlAllocRegionMem( &(sCaptureMgr->mRegionMem),
                                    STL_SIZEOF( ztcChunkItem ),
                                    (void**)&sChunkItem,
                                    aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( stlAllocRegionMem( &(sCaptureMgr->mRegionMem),
                                    ZTC_TRANS_CHUNK_ITEM_SIZE,
                                    (void**)&sChunkItem->mBuffer,
                                    aErrorStack ) == STL_SUCCESS );
        
        sChunkItem->mWriteIdx = 0;
        sChunkItem->mReadIdx  = 0;
        
        STL_RING_INIT_DATALINK( sChunkItem,
                                STL_OFFSETOF( ztcChunkItem, mLink ) );
        
        STL_RING_ADD_LAST( &sCaptureMgr->mTransChunkFreeList,
                           sChunkItem );
    }

    /**
     * Restart 정보 초기화
     */
    STL_RING_INIT_HEADLINK( &sCaptureMgr->mRestartInfoList,
                            STL_OFFSETOF( ztcRestartInfo, mLink ) );
    
    sCaptureMgr->mRecoveryLastCommitLSN = ZTC_INVALID_LSN;
    sCaptureMgr->mTransSkipCount        = 0;
    sCaptureMgr->mCaptureMonitorStatus  = STL_FALSE;  //Recovery가 끝난 이후에 STL_TRUE로 변경한다.
    
    STL_TRY( ztccClearFirstWriteChunk( aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    switch( sState )
    {
        case 1:
            (void)stlDestroyRegionAllocator( &(sCaptureMgr->mRegionMem),
                                             aErrorStack );
            break;
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztccFinalizeCapture( stlErrorStack * aErrorStack )
{
    ztcCaptureMgr * sCaptureMgr = &(gMasterMgr->mCaptureMgr);
    
    STL_TRY( stlDestroyRegionAllocator( &(sCaptureMgr->mRegionMem),
                                        aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * TODO : MACRO로 빼자...
 */ 
void ztccInitializeSlotHdr( ztcTransSlotHdr * aTransSlotHdr )
{
    /**
     * mRestartInfo는 각 단계에서 따로 초기화해야 한다.
     */ 
    aTransSlotHdr->mIsBegin      = STL_FALSE;
    aTransSlotHdr->mGiveUp       = STL_FALSE;
    aTransSlotHdr->mTransId      = ZTC_INVALID_TRANSID;
    aTransSlotHdr->mSize         = 0;
    aTransSlotHdr->mLogCount     = 0;
    aTransSlotHdr->mFileSize     = 0;
    aTransSlotHdr->mFileLogCount = 0;
    aTransSlotHdr->mIsRebuild    = STL_FALSE;
}

/** @} */

