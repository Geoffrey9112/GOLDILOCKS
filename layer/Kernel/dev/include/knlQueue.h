/*******************************************************************************
 * knlQueue.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: knlQueue.h 599 2011-04-21 04:34:52Z xcom73 $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _KNLQUEUE_H_
#define _KNLQUEUE_H_ 1

/**
 * @file knlQueue.h
 * @brief Kernel Layer Queue Routines
 */

/**
 * @defgroup knlQueue Queue
 * @ingroup knExternal
 * @{
 */

stlStatus knlInitQueue( knlQueueInfo  * aQueueInfo,
                        stlInt64        aInitArraySize,
                        stlInt64        aItemSize,
                        knlDynamicMem * aDynamicMem,
                        knlEnv        * aEnv );

stlStatus knlFiniQueue( knlQueueInfo  * aQueueInfo,
                        knlEnv        * aEnv );

stlStatus knlEnqueue( knlQueueInfo * aQueueInfo,
                      void         * aItem,
                      knlEnv       * aEnv );

stlStatus knlDequeue( knlQueueInfo * aQueueInfo,
                      void         * aItem,
                      stlBool      * aIsEmpty,
                      knlEnv       * aEnv );

stlInt64 knlGetQueueSize( knlQueueInfo * aQueueInfo );

/** @} */
    
#endif /* _KNLQUEUE_H_ */
