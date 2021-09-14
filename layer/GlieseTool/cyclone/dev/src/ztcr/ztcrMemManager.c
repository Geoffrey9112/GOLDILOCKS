/*******************************************************************************
 * ztcrMemManager.c
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
 * @file ztcrMemManager.c
 * @brief GlieseTool Cyclone Receiver Chunk Management Routines
 */

#include <goldilocks.h>
#include <ztc.h>

extern ztcSlaveMgr * gSlaveMgr;
extern stlBool       gRunState;

stlStatus ztcrGetChunkItemToWrite( ztcChunkItem ** aChunkItem,
                                   stlErrorStack * aErrorStack )
{
    ztcChunkItem   * sChunkItem      = NULL;
    ztcChunkItem   * sNextChunkItem  = NULL;
    stlBool         sIsTimeout      = STL_TRUE;
    
    if( STL_RING_IS_EMPTY( &gSlaveMgr->mWriteCkList ) == STL_FALSE )
    {
        /**
         * 사용할 수 있는(Rcv) ChunkList가 있을 경우 처음 Item을 가져온다.
         */
        sChunkItem = STL_RING_GET_FIRST_DATA( &gSlaveMgr->mWriteCkList );
    }
    else
    {
        /**
         * Free ChunkList에서 가져온다
         */
        ztcmAcquireSpinLock( &(gSlaveMgr->mWriteSpinLock) );
        
        if( STL_RING_IS_EMPTY( &gSlaveMgr->mWaitWriteCkList ) == STL_TRUE )
        {
            gSlaveMgr->mWaitWriteState = STL_TRUE;
            
            ztcmReleaseSpinLock( &(gSlaveMgr->mWriteSpinLock) );

            
            while( sIsTimeout == STL_TRUE )
            {
                STL_TRY( stlTimedAcquireSemaphore( &(gSlaveMgr->mWaitWriteSem),
                                                   STL_SET_SEC_TIME( 1 ),
                                                   &sIsTimeout,
                                                   aErrorStack ) == STL_SUCCESS );
        
                STL_TRY( gRunState == STL_TRUE );
            }            
            
            ztcmAcquireSpinLock( &(gSlaveMgr->mWriteSpinLock) );
        }
        
        STL_RING_FOREACH_ENTRY_SAFE( &gSlaveMgr->mWaitWriteCkList, sChunkItem, sNextChunkItem )
        {
            STL_RING_UNLINK( &gSlaveMgr->mWaitWriteCkList, sChunkItem );
            
            STL_RING_ADD_LAST( &gSlaveMgr->mWriteCkList, 
                               sChunkItem );
        }
        
        ztcmReleaseSpinLock( &(gSlaveMgr->mWriteSpinLock) );

        STL_ASSERT( STL_RING_IS_EMPTY( &gSlaveMgr->mWriteCkList ) == STL_FALSE );
        
        sChunkItem = STL_RING_GET_FIRST_DATA( &gSlaveMgr->mWriteCkList );
    }
    
    *aChunkItem = sChunkItem;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcrWriteDataToChunk( stlChar       * aBuffer,
                                stlInt64        aSize,
                                stlErrorStack * aErrorStack )
{
    ztcChunkItem  * sChunkItem    = NULL;
    stlInt64       sWritableSize = 0;
    
    /**
     * Chunk 하나를 얻어온다.
     */
    STL_TRY( ztcrGetChunkItemToWrite( &sChunkItem,
                                      aErrorStack ) == STL_SUCCESS );
    
    STL_DASSERT( sChunkItem != NULL );
    
    if( aSize <= ZTC_CHUNK_ITEM_SIZE - sChunkItem->mWriteIdx )
    {
        /**
         * Receive 데이터가 하나의 Chunk에 모두 들어갈 경우..
         */
        stlMemcpy( &(sChunkItem->mBuffer[sChunkItem->mWriteIdx]),
                   aBuffer,
                   aSize );
            
        sChunkItem->mWriteIdx += aSize;
    }
    else
    {
        /**
         * Receive 데이터가 두개의 Chunk에 나눠서 저장될 경우
         */
        sWritableSize = ZTC_CHUNK_ITEM_SIZE - sChunkItem->mWriteIdx;
        
        stlMemcpy( &(sChunkItem->mBuffer[sChunkItem->mWriteIdx]),
                   aBuffer,
                   sWritableSize );
        
        sChunkItem->mWriteIdx += sWritableSize;
        
        STL_TRY( ztcrPushChunkToWaitReadList( sChunkItem,
                                              aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( ztcrGetChunkItemToWrite( &sChunkItem,
                                          aErrorStack ) == STL_SUCCESS );
        
        stlMemcpy( &(sChunkItem->mBuffer[sChunkItem->mWriteIdx]),
                   &aBuffer[ sWritableSize ],
                   aSize - sWritableSize );
        
        sChunkItem->mWriteIdx += ( aSize - sWritableSize );
    }
    
    /**
     * 수신한 데이터를 곧바로 적용할 수 있도록 WaitDistbt List에 넣는다.
     */
    STL_TRY( ztcrPushChunkToWaitReadList( sChunkItem,
                                          aErrorStack ) == STL_SUCCESS );
        
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
    
}

stlStatus ztcrPushChunkToWaitReadList( ztcChunkItem  * aChunkItem,
                                       stlErrorStack * aErrorStack )
{
    stlBool  sDoRelease = STL_FALSE;
    
    /**
     * Distributor가 처리할 수 있도록 Wait DistbtList 넣는다.
     */
    
    STL_RING_UNLINK( &gSlaveMgr->mWriteCkList, aChunkItem );
    
    ztcmAcquireSpinLock( &(gSlaveMgr->mReadSpinLock) );
    
    STL_RING_ADD_LAST( &gSlaveMgr->mWaitReadCkList, 
                       aChunkItem );
    
    sDoRelease = gSlaveMgr->mWaitReadState;
    
    gSlaveMgr->mWaitReadState = STL_FALSE;
    
    ztcmReleaseSpinLock( &(gSlaveMgr->mReadSpinLock) );

    if( sDoRelease == STL_TRUE )
    {
        /**
         * Distributor가 대기하고 있으면 깨워준다.
         */
        STL_TRY( stlReleaseSemaphore( &(gSlaveMgr->mWaitReadSem), 
                                      aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
    

}

stlStatus ztcrPushFirstChunkToWaitReadList( stlErrorStack * aErrorStack )
{
    stlBool         sDoRelease = STL_FALSE;
    ztcChunkItem   * sChunkItem = NULL;
    
    
    STL_TRY_THROW( STL_RING_IS_EMPTY( &gSlaveMgr->mWriteCkList ) == STL_FALSE, RAMP_SUCCESS );

    sChunkItem = STL_RING_GET_FIRST_DATA( &gSlaveMgr->mWriteCkList );
    
    STL_TRY_THROW( sChunkItem->mWriteIdx != 0, RAMP_SUCCESS );
    
    /**
     * Distributor가 처리할 수 있도록...
     */
    STL_RING_UNLINK( &gSlaveMgr->mWriteCkList, sChunkItem );
    
    ztcmAcquireSpinLock( &(gSlaveMgr->mReadSpinLock) );
    
    STL_RING_ADD_LAST( &gSlaveMgr->mWaitReadCkList, 
                       sChunkItem );
    
    sDoRelease = gSlaveMgr->mWaitReadState;
    
    gSlaveMgr->mWaitReadState = STL_FALSE;
    
    ztcmReleaseSpinLock( &(gSlaveMgr->mReadSpinLock) );

    if( sDoRelease == STL_TRUE )
    {
        /**
         * Distributor가 대기하고 있으면 깨워준다.
         */
        STL_TRY( stlReleaseSemaphore( &(gSlaveMgr->mWaitReadSem), 
                                      aErrorStack ) == STL_SUCCESS );
    }
    
    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/** @} */



