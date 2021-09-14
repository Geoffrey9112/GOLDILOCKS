/*******************************************************************************
 * kneEvent.h
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


#ifndef _KNEEVENT_H_
#define _KNEEVENT_H_ 1

/**
 * @file kneEvent.h
 * @brief Kernel Layer Event Internal Routines
 */

/**
 * @defgroup kneEvent Event
 * @ingroup knInternal
 * @internal
 * @{
 */

stlStatus kneAllocEventMem( stlUInt32    aDataSize,
                            void      ** aEventMem,
                            knlEnv     * aEnv );

stlStatus kneAddEvent( knlEvent    * aEvent,
                       stlUInt32     aEventId,
                       void        * aEventMem,
                       void        * aData,
                       stlUInt32     aDataSize,
                       stlUInt32     aFailBehavior,
                       void        * aWaitEnv,
                       stlInt8       aWaitMode,
                       stlInt64    * aEsn,
                       knlEnv      * aEnv );

stlStatus kneCancelEvent( knlEvent    * aEvent,
                          stlInt64      aEsn,
                          knlEnv      * aEnv );

stlStatus kneRemoveEvent( knlEvent     * aEvent,
                          kneEventData * aEventData,
                          stlBool        aAcquireLock,
                          knlEnv       * aEnv );

/** @} */
    
#endif /* _KNEEVENT_H_ */
