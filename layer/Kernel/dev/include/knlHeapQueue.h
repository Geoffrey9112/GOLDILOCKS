/*******************************************************************************
 * knlHeapQueue.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: knlHeapQueue.h 599 2011-04-21 04:34:52Z xcom73 $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _KNLHEAPQUEUE_H_
#define _KNLHEAPQUEUE_H_ 1

/**
 * @file knlHeapQueue.h
 * @brief Kernel Layer Heap Queue Routines
 */

/**
 * @defgroup knlHeapQueue Heap Queue
 * @ingroup knExternal
 * @{
 */

stlStatus knlHeapify( knlHeapQueueInfo    * aHeapQueueInfo,
                      knlHeapQueueContext * aContext,
                      knlHeapQueueEntry   * aRunArray,
                      stlInt64              aArraySize,
                      stlInt64              aRunCount,
                      knlEnv              * aEnv );

stlStatus knlGetMinHeap( knlHeapQueueInfo  * aHeapQueueInfo,
                         void             ** aItem,
                         stlInt64          * aIteratorIdx,
                         knlEnv            * aEnv );

knlHeapQueueEntry * knlGetMinQueueEntry( knlHeapQueueInfo * aHeapQueueInfo );

/** @} */
    
#endif /* _KNLHEAPQUEUE_H_ */
