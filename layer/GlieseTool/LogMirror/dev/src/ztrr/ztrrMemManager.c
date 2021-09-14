/*******************************************************************************
 * ztrrMemManager.c
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
 * @file ztrrMemManager.c
 * @brief GlieseTool LogMirror Receiver Memory Manager Routines
 */

#include <goldilocks.h>
#include <ztr.h>

extern stlBool       gRunState;
extern ztrSlaveMgr   gSlaveMgr;

stlInt64 gRcvTotalSize = 0;

stlStatus ztrrGetChunkItemToWrite( ztrChunkItem ** aChunkItem,
                                   stlErrorStack * aErrorStack )
{
    ztrChunkItem   * sChunkItem      = NULL;
    ztrChunkItem   * sNextChunkItem  = NULL;
    stlBool          sIsTimeout      = STL_TRUE;
    
    if( STL_RING_IS_EMPTY( &(gSlaveMgr.mWriteCkList) ) == STL_FALSE )
    {
        /**
         * 처음 Item을 가져온다.
         */
        sChunkItem = STL_RING_GET_FIRST_DATA( &gSlaveMgr.mWriteCkList );
    }
    else
    {
        ztrmAcquireSpinLock( &(gSlaveMgr.mWriteSpinLock) );
        
        if( STL_RING_IS_EMPTY( &(gSlaveMgr.mWaitWriteCkList) ) == STL_TRUE )
        {
            gSlaveMgr.mWaitWriteState = STL_TRUE;
            
            ztrmReleaseSpinLock( &(gSlaveMgr.mWriteSpinLock) );
            
            while( sIsTimeout == STL_TRUE )
            {
                STL_TRY( stlTimedAcquireSemaphore( &(gSlaveMgr.mWaitWriteSem),
                                                   STL_SET_SEC_TIME( 1 ),
                                                   &sIsTimeout,
                                                   aErrorStack ) == STL_SUCCESS );
                
                STL_TRY( gRunState == STL_TRUE );
            }
            
            ztrmAcquireSpinLock( &(gSlaveMgr.mWriteSpinLock) );
        }
        
        STL_RING_FOREACH_ENTRY_SAFE( &(gSlaveMgr.mWaitWriteCkList), sChunkItem, sNextChunkItem )
        {
            STL_RING_UNLINK( &(gSlaveMgr.mWaitWriteCkList), sChunkItem );
            
            STL_RING_ADD_LAST( &(gSlaveMgr.mWriteCkList), 
                               sChunkItem );
        }
        
        ztrmReleaseSpinLock( &(gSlaveMgr.mWriteSpinLock) );
        
        STL_ASSERT( STL_RING_IS_EMPTY( &(gSlaveMgr.mWriteCkList) ) == STL_FALSE );
        
        sChunkItem = STL_RING_GET_FIRST_DATA( &(gSlaveMgr.mWriteCkList) );
    }
    
    *aChunkItem = sChunkItem;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztrrWriteDataToChunk( stlChar       * aBuffer,
                                stlInt64        aSize,
                                stlBool         aIsLogFileHdr,
                                stlErrorStack * aErrorStack )
{
    ztrChunkItem  * sChunkItem    = NULL;
    stlInt64        sWritableSize = 0;
    stlInt64        sWriteSize    = 0;
    stlInt64        sWriteIdx     = 0;
    stlChar         sLogFileHdrBuff[ ZTR_DEFAULT_BLOCK_SIZE ];
    
    /**
     * Chunk 하나를 얻어온다.
     */

    if( aIsLogFileHdr == STL_TRUE )
    {
        /**
         * LogFileHeader일 경우에는 새로운 Chunk를 얻고 Header임을 세팅한다.
         */
        STL_TRY( ztrrPushFirstChunkToWaitReadList( aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( ztrrGetChunkItemToWrite( &sChunkItem,
                                          aErrorStack ) == STL_SUCCESS );
        
        sChunkItem->mHasLogFileHdr = STL_TRUE;
        
        stlMemset( sLogFileHdrBuff, 0x00, STL_SIZEOF( sLogFileHdrBuff ) );
        
        stlMemcpy( sLogFileHdrBuff, aBuffer, STL_SIZEOF( ztrLogFileHdr ) );
        
        stlMemcpy( &(sChunkItem->mBuffer[sChunkItem->mWriteIdx]),
                   sLogFileHdrBuff,
                   STL_SIZEOF( sLogFileHdrBuff ) );
            
        sChunkItem->mWriteIdx += STL_SIZEOF( sLogFileHdrBuff );
    }
    else
    {
        STL_TRY( ztrrGetChunkItemToWrite( &sChunkItem,
                                          aErrorStack ) == STL_SUCCESS );    
        
        STL_DASSERT( sChunkItem != NULL );
    
        if( aSize <= ZTR_DEFAULT_SLAVE_CHUNK_SIZE - sChunkItem->mWriteIdx )
        {
            stlMemcpy( &(sChunkItem->mBuffer[sChunkItem->mWriteIdx]),
                       aBuffer,
                       aSize );
            
            sChunkItem->mWriteIdx += aSize;
        }
        else
        {
            sWriteIdx  = 0;
            
            while( 1 )
            {
                sWritableSize = ZTR_DEFAULT_SLAVE_CHUNK_SIZE - sChunkItem->mWriteIdx;
                
                if( sWritableSize == 0 )
                {
                    STL_TRY( ztrrPushChunkToWaitReadList( sChunkItem,
                                                          aErrorStack ) == STL_SUCCESS );
                    
                    STL_TRY( ztrrGetChunkItemToWrite( &sChunkItem,
                                                      aErrorStack ) == STL_SUCCESS );
                    
                    sWritableSize = ZTR_DEFAULT_SLAVE_CHUNK_SIZE - sChunkItem->mWriteIdx;
                }
                
                sWriteSize = STL_MIN( sWritableSize, aSize - sWriteIdx );
                
                stlMemcpy( &(sChunkItem->mBuffer[sChunkItem->mWriteIdx]),
                           &aBuffer[ sWriteIdx ],
                           sWriteSize );
                
                sWriteIdx += sWriteSize;
                sChunkItem->mWriteIdx += sWriteSize;
                
                if( sWriteIdx == aSize )
                {
                    break;   
                }
            }
        }
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztrrPushChunkToWaitReadList( ztrChunkItem  * aChunkItem,
                                       stlErrorStack * aErrorStack )
{
    stlBool  sDoRelease = STL_FALSE;
    
    ztrmAcquireSpinLock( &(gSlaveMgr.mReadSpinLock) );
    
    STL_RING_UNLINK( &(gSlaveMgr.mWriteCkList), aChunkItem );
    
    STL_RING_ADD_LAST( &(gSlaveMgr.mWaitReadCkList), 
                       aChunkItem );
    
    sDoRelease = gSlaveMgr.mWaitReadState;
    
    gSlaveMgr.mWaitReadState = STL_FALSE;
    
    ztrmReleaseSpinLock( &(gSlaveMgr.mReadSpinLock) );
    
    if( sDoRelease == STL_TRUE )
    {
        /**
         * Sender가 대기하고 있으면 깨워준다.
         */
        STL_TRY( stlReleaseSemaphore( &(gSlaveMgr.mWaitReadSem), 
                                      aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztrrPushFirstChunkToWaitReadList( stlErrorStack * aErrorStack )
{
    stlBool         sDoRelease = STL_FALSE;
    ztrChunkItem  * sChunkItem = NULL;
    
    STL_TRY_THROW( STL_RING_IS_EMPTY( &(gSlaveMgr.mWriteCkList) ) == STL_FALSE, RAMP_SUCCESS );
    
    sChunkItem = STL_RING_GET_FIRST_DATA( &(gSlaveMgr.mWriteCkList) );
    
    STL_TRY_THROW( sChunkItem->mWriteIdx != 0, RAMP_SUCCESS );
    
    ztrmAcquireSpinLock( &(gSlaveMgr.mReadSpinLock) );
    
    STL_RING_UNLINK( &(gSlaveMgr.mWriteCkList), sChunkItem );
    
    STL_RING_ADD_LAST( &(gSlaveMgr.mWaitReadCkList), 
                       sChunkItem );
    
    sDoRelease = gSlaveMgr.mWaitReadState;
    
    gSlaveMgr.mWaitReadState = STL_FALSE;
    
    ztrmReleaseSpinLock( &(gSlaveMgr.mReadSpinLock) );

    if( sDoRelease == STL_TRUE )
    {
        STL_TRY( stlReleaseSemaphore( &(gSlaveMgr.mWaitReadSem), 
                                      aErrorStack ) == STL_SUCCESS );
    }
    
    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}



/** @} */

