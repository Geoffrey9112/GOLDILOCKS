/*******************************************************************************
 * ztcsMemManager.c
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
 * @file ztcsMemManager.c
 * @brief GlieseTool Cyclone Sender Chunk Management Routines
 */

#include <goldilocks.h>
#include <ztc.h>

extern stlBool        gRunState;
extern ztcMasterMgr * gMasterMgr;

stlStatus ztcsGetChunkItemToRead( ztcChunkItem ** aChunkItem,
                                  stlErrorStack * aErrorStack )
{
    ztcChunkItem   * sChunkItem     = NULL;
    ztcChunkItem   * sNextChunkItem = NULL;
    stlBool          sIsTimeout     = STL_TRUE;
    
    if( STL_RING_IS_EMPTY( &gMasterMgr->mReadCkList ) == STL_FALSE )
    {
        sChunkItem = STL_RING_GET_FIRST_DATA( &gMasterMgr->mReadCkList );
    }
    else
    {
        ztcmAcquireSpinLock( &(gMasterMgr->mReadSpinLock) );
        
        if( STL_RING_IS_EMPTY( &gMasterMgr->mWaitReadCkList ) == STL_TRUE )
        {
            gMasterMgr->mWaitReadState = STL_TRUE;
            
            STL_RAMP( RAMP_RETRY );
            
            ztcmReleaseSpinLock( &(gMasterMgr->mReadSpinLock) );
            
            /**
             * RETRY 시를 위해서 초기화 해야함!!
             */
            sIsTimeout = STL_TRUE;
            
            while( sIsTimeout == STL_TRUE )
            {
                STL_TRY( stlTimedAcquireSemaphore( &(gMasterMgr->mWaitReadSem),
                                                   STL_SET_SEC_TIME( 1 ),
                                                   &sIsTimeout,
                                                   aErrorStack ) == STL_SUCCESS );
                /**
                 * Slave Stop Message Or Kill Check
                 */
                STL_TRY( ztcsCheckSlaveState( aErrorStack ) == STL_SUCCESS );
                
                STL_TRY( gRunState == STL_TRUE );
            }
            
            ztcmAcquireSpinLock( &(gMasterMgr->mReadSpinLock) );
            
            /**
             * gMasterMgr->mWaitReadState는 반드시 STL_FALSE여야 한다. 아닐 경우 다시 대기
             * SLAVE가 재접속했을 경우, MASTER는 기존의 SEMAPHORE를 유지하므로 기존 정보가 남아있을 수 있다.
             * 따라서, 아래와 같이 해결한다.
             */
            STL_TRY_THROW( gMasterMgr->mWaitReadState == STL_FALSE, RAMP_RETRY );
        }
        
        STL_ASSERT( STL_RING_IS_EMPTY( &gMasterMgr->mWaitReadCkList ) == STL_FALSE );
        
        STL_RING_FOREACH_ENTRY_SAFE( &gMasterMgr->mWaitReadCkList, sChunkItem, sNextChunkItem )
        {
            STL_RING_UNLINK( &gMasterMgr->mWaitReadCkList, sChunkItem );
            
            STL_RING_ADD_LAST( &gMasterMgr->mReadCkList, 
                               sChunkItem );
        }
        
        ztcmReleaseSpinLock( &(gMasterMgr->mReadSpinLock) );
        
        STL_ASSERT( STL_RING_IS_EMPTY( &gMasterMgr->mReadCkList ) == STL_FALSE );
        
        sChunkItem = STL_RING_GET_FIRST_DATA( &gMasterMgr->mReadCkList );
    }
    
    *aChunkItem = sChunkItem;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcsReadDataFromChunk( stlChar       * aDstBuffer,
                                 stlInt32      * aSize,
                                 stlErrorStack * aErrorStack )
{
    ztcChunkItem   * sChunkItem      = NULL;
    
    STL_TRY( ztcsGetChunkItemToRead( &sChunkItem,
                                     aErrorStack ) == STL_SUCCESS );
    
    STL_DASSERT( sChunkItem != NULL );
    
    /**
     * Memcpy 안하고 그냥 Send해도 되는데.... 그럼 Capture가 느려짐...
     */
    stlMemcpy( aDstBuffer,
               sChunkItem->mBuffer,
               sChunkItem->mWriteIdx );
    
    *aSize = sChunkItem->mWriteIdx;
    
    STL_TRY( ztcsPushChunkToWaitWriteList( sChunkItem,
                                           aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztcsPushChunkToWaitWriteList( ztcChunkItem  * aChunkItem,
                                        stlErrorStack * aErrorStack )
{
    stlBool  sDoRelease = STL_FALSE;
    
    STL_RING_UNLINK( &gMasterMgr->mReadCkList, aChunkItem );
    
    aChunkItem->mWriteIdx = 0;
    aChunkItem->mReadIdx  = 0;
    
    ztcmAcquireSpinLock( &(gMasterMgr->mWriteSpinLock) );
    
    STL_RING_ADD_LAST( &gMasterMgr->mWaitWriteCkList, 
                       aChunkItem );
    
    sDoRelease = gMasterMgr->mWaitWriteState;
    
    gMasterMgr->mWaitWriteState = STL_FALSE;
    
    ztcmReleaseSpinLock( &(gMasterMgr->mWriteSpinLock) );

    if( sDoRelease == STL_TRUE )
    {
        STL_TRY( stlReleaseSemaphore( &(gMasterMgr->mWaitWriteSem), 
                                      aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;    

}


stlStatus ztcsPushAllChunkToWaitWriteList( stlErrorStack * aErrorStack )
{
    ztcChunkItem   * sChunkItem      = NULL;
    ztcChunkItem   * sNextChunkItem  = NULL;
    
    ztcmAcquireSpinLock( &(gMasterMgr->mWriteSpinLock) );
    
    if( STL_RING_IS_EMPTY( &gMasterMgr->mReadCkList ) == STL_FALSE )
    {
        STL_RING_FOREACH_ENTRY_SAFE( &gMasterMgr->mReadCkList, sChunkItem, sNextChunkItem )
        {
            STL_RING_UNLINK( &gMasterMgr->mReadCkList, sChunkItem );
            
            sChunkItem->mWriteIdx = 0;
            sChunkItem->mReadIdx  = 0;
            
            STL_RING_ADD_LAST( &gMasterMgr->mWaitWriteCkList, 
                               sChunkItem );
        }
    }
    
    ztcmAcquireSpinLock( &(gMasterMgr->mReadSpinLock) );
        
    /**
     * Slave가 Restart 할 경우에 SenderWaitState가 TRUE이면 Chunk관리에 문제가 생긴다.
     * 즉 데이터가 없는데 읽으라고 할 수 있다.
     * 따라서, WaitReadState를 FALSE로 하고 대기하는 것이 없다고 세팅한다.
     */
    gMasterMgr->mWaitReadState = STL_FALSE;
        
    STL_RING_FOREACH_ENTRY_SAFE( &gMasterMgr->mWaitReadCkList, sChunkItem, sNextChunkItem )
    {
        STL_RING_UNLINK( &gMasterMgr->mWaitReadCkList, sChunkItem );
            
        sChunkItem->mWriteIdx = 0;
        sChunkItem->mReadIdx  = 0;
            
        STL_RING_ADD_LAST( &gMasterMgr->mWaitWriteCkList, 
                           sChunkItem );
    }
        
    ztcmReleaseSpinLock( &(gMasterMgr->mReadSpinLock) );
    
    ztcmReleaseSpinLock( &(gMasterMgr->mWriteSpinLock) );
    
    return STL_SUCCESS;
}

/** @} */

