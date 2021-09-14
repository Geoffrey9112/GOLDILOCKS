/*******************************************************************************
 * ztcaMemManager.c
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
 * @file ztcaMemManager.c
 * @brief GlieseTool Cyclone Applier Chunk Management Routines
 */

#include <goldilocks.h>
#include <ztc.h>

extern ztcSlaveMgr  * gSlaveMgr;
extern stlBool        gRunState;

stlStatus ztcaWriteDataToChunk( ztcApplierMgr * aApplierMgr,
                                stlChar       * aData,
                                stlInt64        aSize,
                                stlErrorStack * aErrorStack )
{
    ztcChunkItem  * sChunkItem    = NULL;
    stlInt64        sWritableSize = 0;
    
    /**
     * Chunk 하나를 얻어온다.
     */
    STL_TRY( ztcaGetChunkItemToWrite( aApplierMgr,
                                      &sChunkItem,
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
        
        STL_TRY( ztcaPushChunkToWaitReadList( aApplierMgr,
                                              sChunkItem,
                                              aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( ztcaGetChunkItemToWrite( aApplierMgr,
                                          &sChunkItem,
                                          aErrorStack ) == STL_SUCCESS );
        
        stlMemcpy( &(sChunkItem->mBuffer[sChunkItem->mWriteIdx]),
                   &aData[ sWritableSize ],
                   aSize - sWritableSize );
        
        sChunkItem->mWriteIdx += ( aSize - sWritableSize );
    }

    if( sChunkItem->mWriteIdx == ZTC_APPLIER_CHUNK_ITEM_SIZE )
    {
        STL_TRY( ztcaPushChunkToWaitReadList( aApplierMgr,
                                              sChunkItem,
                                              aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}
                                

stlStatus ztcaReadDataFromChunk( ztcApplierMgr * aApplierMgr,
                                 stlChar       * aData,
                                 stlInt64        aSize,
                                 stlErrorStack * aErrorStack )
{
    ztcChunkItem  * sChunkItem      = NULL;
    stlInt64        sReadableSize = 0;
    stlInt64        sReadSize     = 0;
    stlInt64        sBufferIdx    = 0;
    
    STL_TRY( ztcaGetChunkItemToRead( aApplierMgr,
                                     &sChunkItem,
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
                STL_TRY( ztcaPushChunkToWaitWriteList( aApplierMgr,
                                                       sChunkItem,
                                                       aErrorStack ) == STL_SUCCESS );
                    
                STL_TRY( ztcaGetChunkItemToRead( aApplierMgr,
                                                 &sChunkItem,
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
        STL_TRY( ztcaPushChunkToWaitWriteList( aApplierMgr,
                                               sChunkItem,
                                               aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcaGetChunkItemToWrite( ztcApplierMgr  * aApplierMgr,
                                   ztcChunkItem  ** aChunkItem,
                                   stlErrorStack  * aErrorStack )
{
    ztcChunkItem  * sChunkItem     = NULL;
    ztcChunkItem  * sNextChunkItem = NULL;
    stlBool         sIsTimeout     = STL_TRUE;
    
    if( STL_RING_IS_EMPTY( &aApplierMgr->mWriteCkList ) == STL_FALSE )
    {
        sChunkItem = STL_RING_GET_FIRST_DATA( &aApplierMgr->mWriteCkList );
    }
    else
    {
        ztcmAcquireSpinLock( &(aApplierMgr->mWriteSpinLock) );
        
        if( STL_RING_IS_EMPTY( &aApplierMgr->mWaitWriteCkList ) == STL_TRUE )
        {
            aApplierMgr->mWaitWriteState = STL_TRUE;
            
            ztcmReleaseSpinLock( &(aApplierMgr->mWriteSpinLock) );
            
            /**
             * Trace Log에 남긴다.
             */
            STL_TRY( ztcmLogMsg( aErrorStack,
                                 "[APPLIER] WAIT WRITE CHUNK : INCREASE APPLIER BUFFER!!\n" ) == STL_SUCCESS );
            
            while( sIsTimeout == STL_TRUE )
            {
                STL_TRY( stlTimedAcquireSemaphore( &(aApplierMgr->mWaitWriteSem),
                                                   STL_SET_SEC_TIME( 1 ),
                                                   &sIsTimeout,
                                                   aErrorStack ) == STL_SUCCESS );
        
                STL_TRY( gRunState == STL_TRUE );
            }
            
            ztcmAcquireSpinLock( &(aApplierMgr->mWriteSpinLock) );
        }
        
        STL_RING_FOREACH_ENTRY_SAFE( &aApplierMgr->mWaitWriteCkList, sChunkItem, sNextChunkItem )
        {
            STL_RING_UNLINK( &aApplierMgr->mWaitWriteCkList, sChunkItem );
            
            STL_RING_ADD_LAST( &aApplierMgr->mWriteCkList, 
                               sChunkItem );
        }
        
        ztcmReleaseSpinLock( &(aApplierMgr->mWriteSpinLock) );
        
        STL_ASSERT( STL_RING_IS_EMPTY( &aApplierMgr->mWriteCkList ) == STL_FALSE );
        
        sChunkItem = STL_RING_GET_FIRST_DATA( &aApplierMgr->mWriteCkList );
    }
    
    *aChunkItem = sChunkItem;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcaGetChunkItemToRead( ztcApplierMgr  * aApplierMgr,
                                  ztcChunkItem  ** aChunkItem,
                                  stlErrorStack  * aErrorStack )
{
    ztcChunkItem  * sChunkItem     = NULL;
    ztcChunkItem  * sNextChunkItem = NULL;
    stlBool         sIsTimeout     = STL_TRUE;
    
    if( STL_RING_IS_EMPTY( &aApplierMgr->mReadCkList ) == STL_FALSE )
    {
        sChunkItem = STL_RING_GET_FIRST_DATA( &aApplierMgr->mReadCkList );
    }
    else
    {
        ztcmAcquireSpinLock( &(aApplierMgr->mReadSpinLock) );
        
        if( STL_RING_IS_EMPTY( &aApplierMgr->mWaitReadCkList ) == STL_TRUE )
        {
            aApplierMgr->mWaitReadState = STL_TRUE;
            
            ztcmReleaseSpinLock( &(aApplierMgr->mReadSpinLock) );
            
            /**
             * Heuristic Commit !!!
             * 새로 시작한 Transaction이 없고, 기존에 Commit되지 않았으면
             * 현재까지의 Transaction을 COMMIT하고 Restart정보를 Update한다.
             *
             * BUG fix
             * - 데이터 모두 읽지 않은 상태 즉, Record 구성이 완전히 되지 않은 상태에서는
             *   Execute를 하면 안된다...
             *   Record가 완전히 구성된 상태에서 Execute 및 Commit을 수행해야 한다.
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
          
            while( sIsTimeout == STL_TRUE )
            {
                STL_TRY( stlTimedAcquireSemaphore( &(aApplierMgr->mWaitReadSem),
                                                   STL_SET_SEC_TIME( 1 ),
                                                   &sIsTimeout,
                                                   aErrorStack ) == STL_SUCCESS );
        
                STL_TRY( gRunState == STL_TRUE );
            }
            
            ztcmAcquireSpinLock( &(aApplierMgr->mReadSpinLock) );
        }
        
        STL_RING_FOREACH_ENTRY_SAFE( &aApplierMgr->mWaitReadCkList, sChunkItem, sNextChunkItem )
        {
            STL_RING_UNLINK( &aApplierMgr->mWaitReadCkList, sChunkItem );
            
            STL_RING_ADD_LAST( &aApplierMgr->mReadCkList, 
                               sChunkItem );
        }
        
        ztcmReleaseSpinLock( &(aApplierMgr->mReadSpinLock) );
        
        STL_ASSERT( STL_RING_IS_EMPTY( &aApplierMgr->mReadCkList ) == STL_FALSE );
        
        sChunkItem = STL_RING_GET_FIRST_DATA( &aApplierMgr->mReadCkList );
    }
    
    *aChunkItem = sChunkItem;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;   
}

stlStatus ztcaPushChunkToWaitWriteList( ztcApplierMgr * aApplierMgr,
                                        ztcChunkItem  * aChunkItem,
                                        stlErrorStack * aErrorStack )
{
    stlBool  sDoRelease = STL_FALSE;

    STL_RING_UNLINK( &aApplierMgr->mReadCkList, aChunkItem );
    
    aChunkItem->mWriteIdx = 0;
    aChunkItem->mReadIdx  = 0;
    
    ztcmAcquireSpinLock( &(aApplierMgr->mWriteSpinLock) );
    
    STL_RING_ADD_LAST( &aApplierMgr->mWaitWriteCkList, 
                       aChunkItem );
    
    sDoRelease = aApplierMgr->mWaitWriteState;
    
    aApplierMgr->mWaitWriteState = STL_FALSE;
    
    ztcmReleaseSpinLock( &(aApplierMgr->mWriteSpinLock) );

    if( sDoRelease == STL_TRUE )
    {
        /**
         * 대기하고 있으면 깨워준다.
         */
        STL_TRY( stlReleaseSemaphore( &(aApplierMgr->mWaitWriteSem), 
                                      aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztcaPushChunkToWaitReadList( ztcApplierMgr * aApplierMgr,
                                       ztcChunkItem  * aChunkItem,
                                       stlErrorStack * aErrorStack )
{
    stlBool  sDoRelease = STL_FALSE;
    
    STL_RING_UNLINK( &aApplierMgr->mWriteCkList, aChunkItem );
    
    ztcmAcquireSpinLock( &(aApplierMgr->mReadSpinLock) );
        
    STL_RING_ADD_LAST( &aApplierMgr->mWaitReadCkList, 
                       aChunkItem );
    
    sDoRelease = aApplierMgr->mWaitReadState;
    
    aApplierMgr->mWaitReadState = STL_FALSE;
    
    ztcmReleaseSpinLock( &(aApplierMgr->mReadSpinLock) );

    if( sDoRelease == STL_TRUE )
    {
        /**
         * 대기하고 있으면 깨워준다.
         */
        STL_TRY( stlReleaseSemaphore( &(aApplierMgr->mWaitReadSem), 
                                      aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcaPushFirstChunkToWaitReadList( ztcApplierMgr * aApplierMgr,
                                            stlErrorStack * aErrorStack )
{
    stlBool         sDoRelease = STL_FALSE;
    ztcChunkItem  * sChunkItem = NULL;
    
    /**
     * 첫번째 Chunk를 얻어온다.
     */
    
    STL_TRY_THROW( STL_RING_IS_EMPTY( &aApplierMgr->mWriteCkList ) == STL_FALSE, RAMP_SUCCESS );
    
    sChunkItem = STL_RING_GET_FIRST_DATA( &aApplierMgr->mWriteCkList );
    
    STL_TRY_THROW( sChunkItem->mWriteIdx != 0, RAMP_SUCCESS );
    
    /**
     * Applier가 처리할 수 있도록 Wait Read에 넣는다.
     */
    STL_RING_UNLINK( &aApplierMgr->mWriteCkList, sChunkItem );
    
    ztcmAcquireSpinLock( &(aApplierMgr->mReadSpinLock) );
    
    STL_RING_ADD_LAST( &aApplierMgr->mWaitReadCkList, 
                       sChunkItem );
    
    sDoRelease = aApplierMgr->mWaitReadState;
    
    aApplierMgr->mWaitReadState = STL_FALSE;
    
    ztcmReleaseSpinLock( &(aApplierMgr->mReadSpinLock) );

    if( sDoRelease == STL_TRUE )
    {
        /**
         * Applier가 대기하고 있으면 깨워준다.
         */
        STL_TRY( stlReleaseSemaphore( &(aApplierMgr->mWaitReadSem), 
                                      aErrorStack ) == STL_SUCCESS );
    }
    
    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;   
    
}

/** @} */

