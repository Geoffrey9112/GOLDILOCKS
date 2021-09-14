/*******************************************************************************
 * knlEvent.h
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


#ifndef _KNLEVENT_H_
#define _KNLEVENT_H_ 1

/**
 * @file knlEvent.h
 * @brief Kernel Layer Event Routines
 */

/**
 * @defgroup knlEvent Event
 * @ingroup knExternal
 * @{
 */

void knlRegisterEventTable( stlLayerClass   aLayerClass,
                            knlEventTable * aEventTable );
void knlRegisterEventHandler( stlInt32        aEventId,
                              knlEventHandler aEventHandler );
stlStatus knlInitEvent( knlEvent * aEvent,
                        knlEnv   * aEnv );
stlStatus knlAllocEnvEventMem( stlUInt32    aDataSize,
                               void      ** aEventMem,
                               knlEnv     * aEnv );
stlStatus knlFreeEnvEventMem( void    * aEventMem,
                              knlEnv  * aEnv );
stlStatus knlAddProcessEvent( stlUInt32    aEventId,
                              void       * aData,
                              stlInt32     aDataSize,
                              stlInt32     aProcessId,
                              knlEnv     * aEnv );
stlStatus knlAddEnvEvent( stlUInt32      aEventId,
                          void         * aEventMem,
                          void         * aData,
                          stlUInt32      aDataSize,
                          stlUInt32      aTargetEnvId,
                          stlUInt32      aEventAttr,
                          stlBool        aForceSuccess,
                          knlEnv       * aEnv );
stlStatus knlWakeupEventListener( knlEventListener * aEventListener,
                                  knlEnv           * aEnv );
stlStatus knlWaitEvent( knlEventListener * aEventListener,
                        stlInterval        aTimeout,
                        stlBool          * aIsTimedOut,
                        knlEnv           * aEnv );
void knlRegisterEventListener( knlEventListener * aEventListener,
                               knlEnv           * aEnv );
stlStatus knlInitEventListener( knlEventListener * aEventListener,
                                knlEnv           * aEnv );
stlStatus knlFiniEventListener( knlEventListener * aEventListener,
                                knlEnv           * aEnv );
stlStatus knlCheckEnvEvent( knlEnv    * aEnv );
stlStatus knlExecuteEnvEvent( knlEnv    * aEnv,
                              stlInt32  * aEventId,
                              stlBool   * aIsCanceled,
                              stlBool   * aIsSuccess );
stlStatus knlExecuteProcessEvent( knlEnv * aEnv );

stlChar * knlGetEventDesc( stlInt32   aEventId );
    
/** @} */
    
#endif /* _KNLEVENT_H_ */
