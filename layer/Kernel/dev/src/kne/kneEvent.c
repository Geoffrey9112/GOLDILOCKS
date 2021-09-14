/*******************************************************************************
 * kneEvent.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file kneEvent.c
 * @brief Kernel Layer Event Internal Routines
 */

#include <stl.h>
#include <dtl.h>
#include <knlDef.h>
#include <knlShmManager.h>
#include <knlDynamicMem.h>
#include <knlRing.h>
#include <knsSegment.h>
#include <knDef.h>
#include <knlLatch.h>
#include <knaLatch.h>
#include <kneEvent.h>

extern knsEntryPoint * gKnEntryPoint;
extern stlInt32        gProcessTableID;

/**
 * @addtogroup kneEvent
 * @{
 */

knlEventTable gKnEventTable[KNL_MAX_EVENT_COUNT] =
{
    {
        "attach shared memory",
        knsAttachShmEventHandler
    },
    {
        "detach shared memory",
        knsDetachShmEventHandler
    },
    {
        "system startup",
        NULL
    },
    {
        "system shutdown",
        NULL
    },
    {
        "cleanup session",
        NULL
    }
};

stlStatus kneAllocEventMem( stlUInt32    aDataSize,
                            void      ** aEventMem,
                            knlEnv     * aEnv )
{
    stlUInt32  sDataSize;
    
    sDataSize = STL_SIZEOF( kneEventData ) + aDataSize;
    
    STL_TRY( knlAllocDynamicMem( &gKnEntryPoint->mEventMem,
                                 sDataSize,
                                 (void**)aEventMem,
                                 aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus kneAddEvent( knlEvent    * aEvent,
                       stlUInt32     aEventId,
                       void        * aEventMem,
                       void        * aData,
                       stlUInt32     aDataSize,
                       stlUInt32     aFailBehavior,
                       void        * aWaitEnv,
                       stlInt8       aWaitMode,
                       stlInt64    * aEsn,
                       knlEnv      * aEnv )
{
    stlInt32       sState = 0;
    stlUInt32      sDataSize;
    kneEventData * sEventData;
    stlUInt64      sDummy = 0;
    
    KNL_ENTER_NON_CANCELLABLE_SECTION( aEnv );
    
    stlAcquireSpinLock( &aEvent->mSpinLock, &sDummy );
    sState = 1;

    if( aEventMem == NULL )
    {
        sDataSize = STL_SIZEOF( kneEventData ) + aDataSize;

        STL_TRY( knlAllocDynamicMem( &gKnEntryPoint->mEventMem,
                                     sDataSize,
                                     (void**)&sEventData,
                                     aEnv ) == STL_SUCCESS );
    }
    else
    {
        sEventData = (kneEventData*)aEventMem;
    }

    aEvent->mCurEsn++;
    
    KNE_INIT_EVENT_DATA( sEventData,
                         aEventId,
                         aData,
                         aDataSize,
                         aWaitEnv,
                         aWaitMode,
                         aEvent->mCurEsn,
                         aFailBehavior );
    STL_RING_ADD_LAST( &aEvent->mEventHead, sEventData );
    
    aEvent->mEventCount++;

    if( aEsn != NULL )
    {
        *aEsn = aEvent->mCurEsn;
    }
    
    sState = 0;
    stlReleaseSpinLock( &aEvent->mSpinLock );

    KNL_LEAVE_NON_CANCELLABLE_SECTION( aEnv );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            stlReleaseSpinLock( &aEvent->mSpinLock );
            KNL_LEAVE_NON_CANCELLABLE_SECTION( aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}


stlStatus kneCancelEvent( knlEvent    * aEvent,
                          stlInt64      aEsn,
                          knlEnv      * aEnv )
{
    stlInt32       sState = 0;
    kneEventData * sEventData;
    stlUInt64      sDummy = 0;

    STL_PARAM_VALIDATE( (aEvent != NULL), KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aEsn <= aEvent->mCurEsn), KNL_ERROR_STACK(aEnv) );

    KNL_ENTER_NON_CANCELLABLE_SECTION( aEnv );
    
    stlAcquireSpinLock( &aEvent->mSpinLock, &sDummy );
    sState = 1;

    STL_RING_FOREACH_ENTRY( &(aEvent->mEventHead), sEventData )
    {
        if( sEventData->mEsn == aEsn )
        {
            sEventData->mWaitEnv = NULL;

            KNE_CANCEL_EVENT_DATA( sEventData );
            break;
        }
    }

    sState = 0;
    stlReleaseSpinLock( &aEvent->mSpinLock );

    KNL_LEAVE_NON_CANCELLABLE_SECTION( aEnv );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            stlReleaseSpinLock( &aEvent->mSpinLock );
            KNL_LEAVE_NON_CANCELLABLE_SECTION( aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus kneRemoveEvent( knlEvent     * aEvent,
                          kneEventData * aEventData,
                          stlBool        aAcquireLock,
                          knlEnv       * aEnv )
{
    kneEventData * sEventData;
    stlInt32       sState = 0;
    stlUInt64      sDummy = 0;
    
    if( aAcquireLock == STL_TRUE )
    {
        KNL_ENTER_NON_CANCELLABLE_SECTION( aEnv );
        stlAcquireSpinLock( &aEvent->mSpinLock, &sDummy );
        sState = 1;
    }

    if( aEventData == NULL )
    {
        sEventData = STL_RING_GET_FIRST_DATA( &aEvent->mEventHead );
    }
    else
    {
        sEventData = aEventData;
    }
    
    STL_RING_UNLINK( &aEvent->mEventHead, sEventData );
    STL_TRY( knlFreeDynamicMem( &gKnEntryPoint->mEventMem,
                                (void*)sEventData,
                                aEnv ) == STL_SUCCESS );
    aEvent->mEventCount--;

    if( sState == 1 )
    {
        sState = 0;
        stlReleaseSpinLock( &aEvent->mSpinLock );
        KNL_LEAVE_NON_CANCELLABLE_SECTION( aEnv );
    }

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            stlReleaseSpinLock( &aEvent->mSpinLock );
            KNL_LEAVE_NON_CANCELLABLE_SECTION( aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

/** @} */
