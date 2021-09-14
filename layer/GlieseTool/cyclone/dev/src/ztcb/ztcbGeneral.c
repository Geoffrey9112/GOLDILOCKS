/*******************************************************************************
 * ztcbGeneral.c
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
 * @file ztcbGeneral.c
 * @brief GlieseTool Cyclone Distributor General  Routines
 */

#include <goldilocks.h>
#include <ztc.h>

extern stlBool        gRunState;
extern ztcSlaveMgr  * gSlaveMgr;

stlStatus ztcbInitializeSlotMgr( stlErrorStack * aErrorStack )
{
    ztcRecStateItem  * sItem = NULL;
    stlChar          * sPtr  = NULL;
    stlUInt64          sIdx  = 0;
    
    STL_TRY( stlAllocRegionMem( &(gSlaveMgr->mRegionMem),
                                ( STL_SIZEOF( ztcRecStateItem ) * ZTC_DISTRIBUTOR_SLOT_SIZE ) + 64,
                                (void**)&sPtr,
                                aErrorStack ) == STL_SUCCESS );
    
    /**
     * 64Byte Align 맞춰서...
     */
    gSlaveMgr->mItemArr = (void*)STL_ALIGN( (stlUInt64)sPtr, 64 );
    
    for( sIdx = 0; sIdx < ZTC_DISTRIBUTOR_SLOT_SIZE; sIdx++ )
    {
        sItem = &gSlaveMgr->mItemArr[ sIdx ];
        
        ztcmInitSpinLock( &sItem->mSpinLock );
        
        sItem->mAppId       = ZTC_INVALID_APPLIER_ID;
        sItem->mRefCount    = 0;
        sItem->mSubRefCount = 0;
        sItem->mSlotNo      = sIdx;
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcbFinalizeSlotMgr( stlErrorStack * aErrorStack )
{
    ztcRecStateItem  * sItem = NULL;
    stlUInt64          sIdx  = 0;
    
    for( sIdx = 0; sIdx < ZTC_DISTRIBUTOR_SLOT_SIZE; sIdx++ )
    {
        sItem = &gSlaveMgr->mItemArr[ sIdx ];
        
        ztcmFinSpinLock( &sItem->mSpinLock );
    }
    
    return STL_SUCCESS;
}


stlStatus ztcbInitializeSubDistributor( stlErrorStack * aErrorStack )
{
    stlInt32       sState      = 0;
    stlInt32       sIdx        = 0;
    ztcChunkItem * sChunkItem  = NULL;
    
    /**
     * Chunk Initialize
     */
    STL_RING_INIT_HEADLINK( &gSlaveMgr->mSubWaitWriteCkList,
                            STL_OFFSETOF( ztcChunkItem, mLink ) );
    
    STL_RING_INIT_HEADLINK( &gSlaveMgr->mSubWriteCkList,
                            STL_OFFSETOF( ztcChunkItem, mLink ) );
    
    gSlaveMgr->mSubWaitWriteState = STL_FALSE;
    
    for( sIdx = 0; sIdx < ZTC_APPLIER_CHUNK_ITEM_COUNT; sIdx++ )
    {
        STL_TRY( stlAllocRegionMem( &gSlaveMgr->mRegionMem,
                                    STL_SIZEOF( ztcChunkItem ),
                                    (void**)&sChunkItem,
                                    aErrorStack ) == STL_SUCCESS );    
        
        STL_TRY( stlAllocRegionMem( &gSlaveMgr->mRegionMem,
                                    ZTC_APPLIER_CHUNK_ITEM_SIZE,
                                    (void**)&(sChunkItem->mBuffer),
                                    aErrorStack ) == STL_SUCCESS );    
        
        sChunkItem->mWriteIdx = 0;
        sChunkItem->mReadIdx  = 0;
        
        STL_RING_INIT_DATALINK( sChunkItem,
                                STL_OFFSETOF( ztcChunkItem, mLink ) );
        
        STL_RING_ADD_LAST( &gSlaveMgr->mSubWriteCkList, 
                           sChunkItem );
    }

    ztcmInitSpinLock( &(gSlaveMgr->mSubWriteSpinLock) );
    
    STL_TRY( stlCreateSemaphore( &(gSlaveMgr->mSubWaitWriteSem),
                                 "SDWWSem",
                                 0,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_RING_INIT_HEADLINK( &gSlaveMgr->mSubWaitReadCkList,
                            STL_OFFSETOF( ztcChunkItem, mLink ) );
    
    STL_RING_INIT_HEADLINK( &gSlaveMgr->mSubReadCkList,
                            STL_OFFSETOF( ztcChunkItem, mLink ) );
    
    gSlaveMgr->mSubWaitReadState = STL_FALSE;

    ztcmInitSpinLock( &(gSlaveMgr->mSubReadSpinLock) );
    
    STL_TRY( stlCreateSemaphore( &(gSlaveMgr->mSubWaitReadSem),
                                 "SDRWSem",
                                 0,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 2;
    
    /**
     * Distributor를 수행할 수 있게 한다.
     */
    STL_TRY( stlReleaseSemaphore( &(gSlaveMgr->mDistbtStartSem),
                                  aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    switch( sState )
    {
        case 2:
            (void) stlDestroySemaphore( &(gSlaveMgr->mSubWaitReadSem),
                                        aErrorStack );
            
            ztcmFinSpinLock( &(gSlaveMgr->mReadSpinLock) );
        case 1:
            (void) stlDestroySemaphore( &(gSlaveMgr->mSubWaitWriteSem),
                                        aErrorStack );
            
            ztcmFinSpinLock(  &(gSlaveMgr->mSubWriteSpinLock) );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztcbFinalizeSubDistributor( stlErrorStack * aErrorStack )
{
    STL_TRY( stlDestroySemaphore( &(gSlaveMgr->mSubWaitReadSem),
                                  aErrorStack ) == STL_SUCCESS );
    
    ztcmFinSpinLock( &(gSlaveMgr->mSubReadSpinLock) );
    
    STL_TRY( stlDestroySemaphore( &(gSlaveMgr->mSubWaitWriteSem),
                                  aErrorStack ) == STL_SUCCESS );
    
    ztcmFinSpinLock(  &(gSlaveMgr->mSubWriteSpinLock) );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/** @} */
