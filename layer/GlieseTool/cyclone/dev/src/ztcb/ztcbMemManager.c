/*******************************************************************************
 * ztcbMemManager.c
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
 * @file ztcbMemManager.c
 * @brief GlieseTool Cyclone Distributor Chunk Management Routines
 */

#include <goldilocks.h>
#include <ztc.h>

extern ztcSlaveMgr  * gSlaveMgr;
extern stlBool        gRunState;

stlStatus ztcbGetChunkItemToRead( ztcChunkItem ** aChunkItem,
                                  stlErrorStack * aErrorStack )
{
    ztcChunkItem  * sChunkItem     = NULL;
    ztcChunkItem  * sNextChunkItem = NULL;
    stlBool         sIsTimeout     = STL_TRUE;
    
    if( STL_RING_IS_EMPTY( &gSlaveMgr->mReadCkList ) == STL_FALSE )
    {
        /**
         * Distribute할 데이터가 있을 경우 처음 Item을 가져온다.
         */ 
        sChunkItem = STL_RING_GET_FIRST_DATA( &gSlaveMgr->mReadCkList );
    }
    else
    {
        /**
         * Receive한 ChunkList에서 가져온다
         */
        ztcmAcquireSpinLock( &(gSlaveMgr->mReadSpinLock) );
        
        if( STL_RING_IS_EMPTY( &gSlaveMgr->mWaitReadCkList ) == STL_TRUE )
        {
            /**
             * Receive한 데이터가 없을 경우.
             */
            gSlaveMgr->mWaitReadState = STL_TRUE;
            
            ztcmReleaseSpinLock( &(gSlaveMgr->mReadSpinLock) );
            
            /**
             * Applier가 Apply할 수 있도록 Applier Buffer를 Flush 한다.
             */
            STL_TRY( ztcbFlushApplierChunk( aErrorStack ) == STL_SUCCESS );
            
            while( sIsTimeout == STL_TRUE )
            {
                STL_TRY( stlTimedAcquireSemaphore( &(gSlaveMgr->mWaitReadSem),
                                                   STL_SET_SEC_TIME( 1 ),
                                                   &sIsTimeout,
                                                   aErrorStack ) == STL_SUCCESS );
        
                STL_TRY( gRunState == STL_TRUE );
            }
            
            ztcmAcquireSpinLock( &(gSlaveMgr->mReadSpinLock) );
        }
        
        STL_RING_FOREACH_ENTRY_SAFE( &gSlaveMgr->mWaitReadCkList, sChunkItem, sNextChunkItem )
        {
            STL_RING_UNLINK( &gSlaveMgr->mWaitReadCkList, sChunkItem );
            
            STL_RING_ADD_LAST( &gSlaveMgr->mReadCkList, 
                               sChunkItem );
        }
        
        ztcmReleaseSpinLock( &(gSlaveMgr->mReadSpinLock) );
        
        STL_ASSERT( STL_RING_IS_EMPTY( &gSlaveMgr->mReadCkList ) == STL_FALSE );
        
        sChunkItem = STL_RING_GET_FIRST_DATA( &gSlaveMgr->mReadCkList );
    }
    
    *aChunkItem = sChunkItem;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
    
}

stlStatus ztcbReadDataFromChunk( stlChar       * aData,
                                 stlInt64        aSize,
                                 stlErrorStack * aErrorStack )
{
    ztcChunkItem  * sChunkItem      = NULL;
    stlInt64        sReadableSize = 0;
    stlInt64        sReadSize     = 0;
    stlInt64        sBufferIdx    = 0;
    
    STL_TRY( ztcbGetChunkItemToRead( &sChunkItem,
                                     aErrorStack ) == STL_SUCCESS );
        
    
    if( aSize + sChunkItem->mReadIdx <= sChunkItem->mWriteIdx )
    {
        /**
         * 하나의 chunk에서 모두 읽을 수 있을 경우.
         */
        stlMemcpy( aData, 
                   &sChunkItem->mBuffer[ sChunkItem->mReadIdx ], 
                   aSize );
        
        sChunkItem->mReadIdx += aSize;
    }
    else
    {
        /**
         * 여러개의 Chunk를 읽어야 할 경우.
         */
        sBufferIdx = 0;
        
        while( 1 )
        {
            sReadableSize = sChunkItem->mWriteIdx - sChunkItem->mReadIdx;
            
            if( sReadableSize == 0 )
            {
                STL_TRY( ztcbPushChunkToWaitWriteList( sChunkItem,
                                                       aErrorStack ) == STL_SUCCESS );
                    
                STL_TRY( ztcbGetChunkItemToRead( &sChunkItem,
                                                 aErrorStack ) == STL_SUCCESS );
                
                sReadableSize = sChunkItem->mWriteIdx - sChunkItem->mReadIdx;
            }
            
            sReadSize = STL_MIN( sReadableSize, aSize - sBufferIdx );
            
            stlMemcpy( &aData[ sBufferIdx ],
                       &sChunkItem->mBuffer[ sChunkItem->mReadIdx ], 
                       sReadSize );
            
            sChunkItem->mReadIdx += sReadSize;
            sBufferIdx += sReadSize;
            
            if( sBufferIdx == aSize )
            {
                break;   
            }
        }//end of While
    }
    
    if( sChunkItem->mReadIdx == sChunkItem->mWriteIdx )
    {
        STL_TRY( ztcbPushChunkToWaitWriteList( sChunkItem,
                                               aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztcbSkipDataFromChunk( stlInt64        aSize,
                                 stlErrorStack * aErrorStack )
{
    ztcChunkItem  * sChunkItem      = NULL;
    stlInt64        sReadableSize = 0;
    stlInt64        sReadSize     = 0;
    stlInt64        sBufferIdx    = 0;
    
    STL_TRY( ztcbGetChunkItemToRead( &sChunkItem,
                                     aErrorStack ) == STL_SUCCESS );
    
    STL_DASSERT( sChunkItem != NULL );
    
    if( aSize + sChunkItem->mReadIdx <= sChunkItem->mWriteIdx )
    {
        /**
         * 하나의 chunk에서 모두 읽을 수 있을 경우.
         */
        sChunkItem->mReadIdx += aSize;
    }
    else
    {
        /**
         * 여러개의 Chunk를 읽어야 할 경우.
         */
        sBufferIdx = 0;
        
        while( 1 )
        {
            sReadableSize = sChunkItem->mWriteIdx - sChunkItem->mReadIdx;
            
            if( sReadableSize == 0 )
            {
                STL_TRY( ztcbPushChunkToWaitWriteList( sChunkItem,
                                                       aErrorStack ) == STL_SUCCESS );
                    
                STL_TRY( ztcbGetChunkItemToRead( &sChunkItem,
                                                 aErrorStack ) == STL_SUCCESS );
                
                sReadableSize = sChunkItem->mWriteIdx - sChunkItem->mReadIdx;
            }
            
            sReadSize = STL_MIN( sReadableSize, aSize - sBufferIdx );

            sChunkItem->mReadIdx += sReadSize;
            sBufferIdx += sReadSize;
            
            if( sBufferIdx == aSize )
            {
                break;   
            }
        }//end of While
    }
    
    if( sChunkItem->mReadIdx == sChunkItem->mWriteIdx )
    {
        STL_TRY( ztcbPushChunkToWaitWriteList( sChunkItem,
                                               aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;

}

stlStatus ztcbPushChunkToWaitWriteList( ztcChunkItem  * aChunkItem,
                                        stlErrorStack * aErrorStack )
{
    stlBool         sDoRelease = STL_FALSE;
    
    /**
     * Receiver에서 사용할 수 있도록...
     */
    STL_RING_UNLINK( &gSlaveMgr->mReadCkList, aChunkItem );
    
    aChunkItem->mWriteIdx = 0;
    aChunkItem->mReadIdx  = 0;
    
    ztcmAcquireSpinLock( &(gSlaveMgr->mWriteSpinLock) );
    
    STL_RING_ADD_LAST( &gSlaveMgr->mWaitWriteCkList, 
                       aChunkItem );
    
    sDoRelease = gSlaveMgr->mWaitWriteState;
    
    gSlaveMgr->mWaitWriteState = STL_FALSE;
    
    ztcmReleaseSpinLock( &(gSlaveMgr->mWriteSpinLock) );

    if( sDoRelease == STL_TRUE )
    {
        STL_TRY( stlReleaseSemaphore( &(gSlaveMgr->mWaitWriteSem), 
                                      aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;    
}


stlStatus ztcbGetSubChunkItemToRead( ztcChunkItem ** aChunkItem,
                                     stlErrorStack * aErrorStack )
{
    ztcChunkItem  * sChunkItem     = NULL;
    ztcChunkItem  * sNextChunkItem = NULL;
    stlBool         sIsTimeout     = STL_TRUE;
    
    if( STL_RING_IS_EMPTY( &gSlaveMgr->mSubReadCkList ) == STL_FALSE )
    {
        /**
         * Sub-Distribute할 데이터가 있을 경우 처음 Item을 가져온다.
         */ 
        sChunkItem = STL_RING_GET_FIRST_DATA( &gSlaveMgr->mSubReadCkList );
    }
    else
    {
        /**
         * Sub-Buffer에서 데이터가 채워져 있어서 읽어야 하는(read) ChunkList에서 가져온다
         */
        ztcmAcquireSpinLock( &(gSlaveMgr->mSubReadSpinLock) );
        
        if( STL_RING_IS_EMPTY( &gSlaveMgr->mSubWaitReadCkList ) == STL_TRUE )
        {
            /**
             * Sub-Buffer 데이터가 없을 경우.
             */
            gSlaveMgr->mSubWaitReadState = STL_TRUE;
            
            ztcmReleaseSpinLock( &(gSlaveMgr->mSubReadSpinLock) );
            
            while( sIsTimeout == STL_TRUE )
            {
                STL_TRY( stlTimedAcquireSemaphore( &(gSlaveMgr->mSubWaitReadSem),
                                                   STL_SET_SEC_TIME( 1 ),
                                                   &sIsTimeout,
                                                   aErrorStack ) == STL_SUCCESS );
        
                STL_TRY( gRunState == STL_TRUE );
            }
            
            ztcmAcquireSpinLock( &(gSlaveMgr->mSubReadSpinLock) );
        }
        
        STL_RING_FOREACH_ENTRY_SAFE( &gSlaveMgr->mSubWaitReadCkList, sChunkItem, sNextChunkItem )
        {
            STL_RING_UNLINK( &gSlaveMgr->mSubWaitReadCkList, sChunkItem );
            
            STL_RING_ADD_LAST( &gSlaveMgr->mSubReadCkList, 
                               sChunkItem );
        }
        
        ztcmReleaseSpinLock( &(gSlaveMgr->mSubReadSpinLock) );
        
        STL_ASSERT( STL_RING_IS_EMPTY( &gSlaveMgr->mSubReadCkList ) == STL_FALSE );
        
        sChunkItem = STL_RING_GET_FIRST_DATA( &gSlaveMgr->mSubReadCkList );
    }
    
    *aChunkItem = sChunkItem;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcbGetSubChunkItemToWrite( ztcChunkItem ** aChunkItem,
                                      stlErrorStack * aErrorStack )
{
    ztcChunkItem  * sChunkItem     = NULL;
    ztcChunkItem  * sNextChunkItem = NULL;
    stlBool         sIsTimeout     = STL_TRUE;
    
    if( STL_RING_IS_EMPTY( &gSlaveMgr->mSubWriteCkList ) == STL_FALSE )
    {
        sChunkItem = STL_RING_GET_FIRST_DATA( &gSlaveMgr->mSubWriteCkList );
    }
    else
    {
        ztcmAcquireSpinLock( &(gSlaveMgr->mSubWriteSpinLock) );
        
        if( STL_RING_IS_EMPTY( &gSlaveMgr->mSubWaitWriteCkList ) == STL_TRUE )
        {
            gSlaveMgr->mSubWaitWriteState = STL_TRUE;
            
            ztcmReleaseSpinLock( &(gSlaveMgr->mSubWriteSpinLock) );
            
            /**
             * Trace Log에 남긴다.
             */
            STL_TRY( ztcmLogMsg( aErrorStack,
                                 "[SUB-DISTRIBUTOR] WAIT WRITE CHUNK : INCREASE APPLIER BUFFER!!\n" ) == STL_SUCCESS );
            
            while( sIsTimeout == STL_TRUE )
            {
                STL_TRY( stlTimedAcquireSemaphore( &(gSlaveMgr->mSubWaitWriteSem),
                                                   STL_SET_SEC_TIME( 1 ),
                                                   &sIsTimeout,
                                                   aErrorStack ) == STL_SUCCESS );
        
                STL_TRY( gRunState == STL_TRUE );
            }
            
            ztcmAcquireSpinLock( &(gSlaveMgr->mSubWriteSpinLock) );
        }
        
        STL_RING_FOREACH_ENTRY_SAFE( &gSlaveMgr->mSubWaitWriteCkList, sChunkItem, sNextChunkItem )
        {
            STL_RING_UNLINK( &gSlaveMgr->mSubWaitWriteCkList, sChunkItem );
            
            STL_RING_ADD_LAST( &gSlaveMgr->mSubWriteCkList, 
                               sChunkItem );
        }
        
        ztcmReleaseSpinLock( &(gSlaveMgr->mSubWriteSpinLock) );
        
        STL_ASSERT( STL_RING_IS_EMPTY( &gSlaveMgr->mSubWriteCkList ) == STL_FALSE );
        
        sChunkItem = STL_RING_GET_FIRST_DATA( &gSlaveMgr->mSubWriteCkList );
    }
    
    *aChunkItem = sChunkItem;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcbPushSubChunkToWaitWriteList( ztcChunkItem  * aChunkItem,
                                           stlErrorStack * aErrorStack )
{
    stlBool  sDoRelease = STL_FALSE;

    STL_RING_UNLINK( &gSlaveMgr->mSubReadCkList, aChunkItem );
    
    aChunkItem->mWriteIdx = 0;
    aChunkItem->mReadIdx  = 0;
    
    ztcmAcquireSpinLock( &(gSlaveMgr->mSubWriteSpinLock) );
    
    STL_RING_ADD_LAST( &gSlaveMgr->mSubWaitWriteCkList, 
                       aChunkItem );
    
    sDoRelease = gSlaveMgr->mSubWaitWriteState;
    
    gSlaveMgr->mSubWaitWriteState = STL_FALSE;
    
    ztcmReleaseSpinLock( &(gSlaveMgr->mSubWriteSpinLock) );

    if( sDoRelease == STL_TRUE )
    {
        /**
         * 대기하고 있으면 깨워준다.
         */
        STL_TRY( stlReleaseSemaphore( &(gSlaveMgr->mSubWaitWriteSem), 
                                      aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcbPushSubChunkToWaitReadList( ztcChunkItem  * aChunkItem,
                                          stlErrorStack * aErrorStack )
{
    stlBool  sDoRelease = STL_FALSE;
    
    STL_RING_UNLINK( &gSlaveMgr->mSubWriteCkList, aChunkItem );
    
    ztcmAcquireSpinLock( &(gSlaveMgr->mSubReadSpinLock) );
    
    STL_RING_ADD_LAST( &gSlaveMgr->mSubWaitReadCkList, 
                       aChunkItem );
    
    sDoRelease = gSlaveMgr->mSubWaitReadState;
    
    gSlaveMgr->mSubWaitReadState = STL_FALSE;
    
    ztcmReleaseSpinLock( &(gSlaveMgr->mSubReadSpinLock) );

    if( sDoRelease == STL_TRUE )
    {
        /**
         * 대기하고 있으면 깨워준다.
         */
        STL_TRY( stlReleaseSemaphore( &(gSlaveMgr->mSubWaitReadSem), 
                                      aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcbReadDataFromSubChunk( stlChar       * aData,
                                    stlInt64        aSize,
                                    stlErrorStack * aErrorStack )
{
    ztcChunkItem  * sChunkItem      = NULL;
    stlInt64        sReadableSize = 0;
    stlInt64        sReadSize     = 0;
    stlInt64        sBufferIdx    = 0;
    
    STL_TRY( ztcbGetSubChunkItemToRead( &sChunkItem,
                                        aErrorStack ) == STL_SUCCESS );
    
    if( aSize + sChunkItem->mReadIdx <= sChunkItem->mWriteIdx )
    {
        /**
         * 하나의 chunk에서 모두 읽을 수 있을 경우.
         */
        stlMemcpy( aData, 
                   &sChunkItem->mBuffer[ sChunkItem->mReadIdx ], 
                   aSize );
        
        sChunkItem->mReadIdx += aSize;
    }
    else
    {
        /**
         * 여러개의 Chunk를 읽어야 할 경우.
         */
        sBufferIdx = 0;
        
        while( 1 )
        {
            sReadableSize = sChunkItem->mWriteIdx - sChunkItem->mReadIdx;
            
            if( sReadableSize == 0 )
            {
                STL_TRY( ztcbPushSubChunkToWaitWriteList( sChunkItem,
                                                          aErrorStack ) == STL_SUCCESS );
                    
                STL_TRY( ztcbGetSubChunkItemToRead( &sChunkItem,
                                                    aErrorStack ) == STL_SUCCESS );
                
                sReadableSize = sChunkItem->mWriteIdx - sChunkItem->mReadIdx;
            }
            
            sReadSize = STL_MIN( sReadableSize, aSize - sBufferIdx );
            
            stlMemcpy( &aData[ sBufferIdx ],
                       &sChunkItem->mBuffer[ sChunkItem->mReadIdx ], 
                       sReadSize );
            
            sChunkItem->mReadIdx += sReadSize;
            sBufferIdx += sReadSize;
            
            if( sBufferIdx == aSize )
            {
                break;   
            }
        }//end of While
    }
    
    if( sChunkItem->mReadIdx == sChunkItem->mWriteIdx )
    {
        STL_TRY( ztcbPushSubChunkToWaitWriteList( sChunkItem,
                                                  aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcbWriteDataToSubChunk( ztcApplierMgr * aApplierMgr,
                                   stlChar       * aData,
                                   stlInt64        aSize,
                                   stlErrorStack * aErrorStack )
{
    ztcChunkItem  * sChunkItem    = NULL;
    stlInt64        sWritableSize = 0;
    
    STL_DASSERT( aSize <= ZTC_APPLIER_CHUNK_ITEM_SIZE );
    
    /**
     * Chunk 하나를 얻어온다.
     */
    STL_TRY( ztcbGetSubChunkItemToWrite( &sChunkItem,
                                         aErrorStack ) == STL_SUCCESS );
    
    if( aSize <= ZTC_APPLIER_CHUNK_ITEM_SIZE - sChunkItem->mWriteIdx )
    {
        /**
         * Chunk에 모두 들어갈 경우..
         */
        stlMemcpy( &(sChunkItem->mBuffer[sChunkItem->mWriteIdx]),
                   aData,
                   aSize );
        
        sChunkItem->mWriteIdx += aSize;
    }
    else
    {
        /**
         * 두개 Chunk에 나눠서 저장될 경우
         */
        sWritableSize = ZTC_APPLIER_CHUNK_ITEM_SIZE - sChunkItem->mWriteIdx;
        
        stlMemcpy( &(sChunkItem->mBuffer[sChunkItem->mWriteIdx]),
                   aData,
                   sWritableSize );
        
        sChunkItem->mWriteIdx += sWritableSize;
        
        STL_TRY( ztcbPushSubChunkToWaitReadList( sChunkItem,
                                                 aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( ztcbGetSubChunkItemToWrite( &sChunkItem,
                                             aErrorStack ) == STL_SUCCESS );
        
        stlMemcpy( &(sChunkItem->mBuffer[sChunkItem->mWriteIdx]),
                   &aData[ sWritableSize ],
                   aSize - sWritableSize );
        
        sChunkItem->mWriteIdx += ( aSize - sWritableSize );
    }
    
    if( sChunkItem->mWriteIdx == ZTC_APPLIER_CHUNK_ITEM_SIZE )
    {
        STL_TRY( ztcbPushSubChunkToWaitReadList( sChunkItem,
                                                 aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcbPushFirstSubChunkToWaitReadList( stlErrorStack * aErrorStack )
{
    stlBool         sDoRelease = STL_FALSE;
    ztcChunkItem  * sChunkItem = NULL;
    
    /**
     * 첫번째 Chunk를 얻어온다.
     */
    STL_TRY_THROW( STL_RING_IS_EMPTY( &gSlaveMgr->mSubWriteCkList ) == STL_FALSE, RAMP_SUCCESS );
    
    sChunkItem = STL_RING_GET_FIRST_DATA( &gSlaveMgr->mSubWriteCkList );
    
    STL_TRY_THROW( sChunkItem->mWriteIdx != 0, RAMP_SUCCESS );
    
    /**
     * Sub-Distributor가 처리할 수 있도록 Wait Read에 넣는다.
     */
    STL_RING_UNLINK( &gSlaveMgr->mSubWriteCkList, sChunkItem );
    
    ztcmAcquireSpinLock( &(gSlaveMgr->mSubReadSpinLock) );
    
    STL_RING_ADD_LAST( &gSlaveMgr->mSubWaitReadCkList, 
                       sChunkItem );
    
    sDoRelease = gSlaveMgr->mSubWaitReadState;
    
    gSlaveMgr->mSubWaitReadState = STL_FALSE;
    
    ztcmReleaseSpinLock( &(gSlaveMgr->mSubReadSpinLock) );

    if( sDoRelease == STL_TRUE )
    {
        STL_TRY( stlReleaseSemaphore( &(gSlaveMgr->mSubWaitReadSem), 
                                      aErrorStack ) == STL_SUCCESS );
    }
    
    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;   
    
}


/** @} */

