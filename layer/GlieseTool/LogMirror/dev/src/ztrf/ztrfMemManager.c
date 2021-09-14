/*******************************************************************************
 * ztrfMemManager.c
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
 * @file ztrfMemManager.c
 * @brief GlieseTool LogMirror Flusher Memory Manager Routines
 */

#include <goldilocks.h>
#include <ztr.h>

extern ztrSlaveMgr   gSlaveMgr;
extern stlBool       gZtrFlusherState;

stlStatus ztrfGetChunkItemToRead( ztrChunkItem ** aChunkItem,
                                  stlErrorStack * aErrorStack )
{
    ztrChunkItem   * sChunkItem     = NULL;
    ztrChunkItem   * sNextChunkItem = NULL;
    stlBool          sIsTimeout     = STL_TRUE;
    
    if( STL_RING_IS_EMPTY( &(gSlaveMgr.mReadCkList) ) == STL_FALSE )
    {
        sChunkItem = STL_RING_GET_FIRST_DATA( &(gSlaveMgr.mReadCkList) );
    }
    else
    {
        ztrmAcquireSpinLock( &(gSlaveMgr.mReadSpinLock) );
        
        if( STL_RING_IS_EMPTY( &(gSlaveMgr.mWaitReadCkList) ) == STL_TRUE )
        {
            gSlaveMgr.mWaitReadState = STL_TRUE;
            
            ztrmReleaseSpinLock( &(gSlaveMgr.mReadSpinLock) );
            
            while( sIsTimeout == STL_TRUE )
            {
                STL_TRY( stlTimedAcquireSemaphore( &(gSlaveMgr.mWaitReadSem),
                                                   STL_SET_SEC_TIME( 1 ),
                                                   &sIsTimeout,
                                                   aErrorStack ) == STL_SUCCESS );
                
                STL_TRY( gZtrFlusherState == STL_TRUE );
            }
            
            ztrmAcquireSpinLock( &(gSlaveMgr.mReadSpinLock) );
        }
        
        STL_ASSERT( STL_RING_IS_EMPTY( &(gSlaveMgr.mWaitReadCkList) ) == STL_FALSE );
        
        STL_RING_FOREACH_ENTRY_SAFE( &(gSlaveMgr.mWaitReadCkList), sChunkItem, sNextChunkItem )
        {
            STL_RING_UNLINK( &(gSlaveMgr.mWaitReadCkList), sChunkItem );
            
            STL_RING_ADD_LAST( &(gSlaveMgr.mReadCkList), 
                               sChunkItem );
        }
        
        ztrmReleaseSpinLock( &(gSlaveMgr.mReadSpinLock) );
        
        STL_ASSERT( STL_RING_IS_EMPTY( &(gSlaveMgr.mReadCkList) ) == STL_FALSE );
        
        sChunkItem = STL_RING_GET_FIRST_DATA( &(gSlaveMgr.mReadCkList) );
    }
    
    *aChunkItem = sChunkItem;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztrfPushChunkToWaitWriteList( ztrChunkItem  * aChunkItem,
                                        stlErrorStack * aErrorStack )
{
    stlBool  sDoRelease = STL_FALSE;
    
    ztrmAcquireSpinLock( &(gSlaveMgr.mWriteSpinLock) );
    
    STL_RING_UNLINK( &(gSlaveMgr.mReadCkList), aChunkItem );
    
    aChunkItem->mWriteIdx      = 0;
    aChunkItem->mReadIdx       = 0;
    aChunkItem->mHasLogFileHdr = STL_FALSE;
    
    STL_RING_ADD_LAST( &(gSlaveMgr.mWaitWriteCkList),
                       aChunkItem );
    
    sDoRelease = gSlaveMgr.mWaitWriteState;
    
    gSlaveMgr.mWaitWriteState = STL_FALSE;
    
    ztrmReleaseSpinLock( &(gSlaveMgr.mWriteSpinLock) );

    if( sDoRelease == STL_TRUE )
    {
        STL_TRY( stlReleaseSemaphore( &(gSlaveMgr.mWaitWriteSem), 
                                      aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;    

}




/** @} */

