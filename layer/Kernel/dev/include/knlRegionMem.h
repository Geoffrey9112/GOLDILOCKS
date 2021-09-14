/*******************************************************************************
 * knlRegionMem.h
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


#ifndef _KNLREGIONMEM_H_
#define _KNLREGIONMEM_H_ 1

/**
 * @file knlRegionMem.h
 * @brief Kernel Layer Region-based Memory Management
 */

/**
 * @defgroup knlRegionMem Region-based Memory
 * @ingroup knlMemManager
 * @{
 */

stlStatus knlCreateRegionMem( knlRegionMem   * aRegionMem,
                              knlAllocatorId   aAllocatorId,
                              knlDynamicMem  * aParentMem,
                              stlUInt32        aStorageType,
                              stlUInt64        aInitSize,
                              stlUInt64        aNextSize,
                              knlEnv         * aEnv );

stlStatus knlMarkRegionMem( knlRegionMem   * aRegionMem,
                            knlRegionMark  * aMark,
                            knlEnv         * aEnv );

stlStatus knlMarkRegionMemEx( knlRegionMem     * aRegionMem,
                              knlRegionMarkEx ** aRegionMarkEx,
                              knlEnv           * aEnv );

stlStatus knlAllocRegionMem( knlRegionMem * aRegionMem,
                             stlUInt32      aSize,
                             void        ** aAddr,
                             knlEnv       * aEnv );

stlStatus knlClearRegionMem( knlRegionMem * aRegionMem,
                             knlEnv       * aEnv );

stlStatus knlFreeUnmarkedRegionMem( knlRegionMem  * aRegionMem,
                                    knlRegionMark * aMark,
                                    stlBool         aFreeChunk,
                                    knlEnv        * aEnv );

stlStatus knlFreeUnmarkedRegionMemEx( knlRegionMem    * aRegionMem,
                                      knlRegionMarkEx * aRegionMarkEx,
                                      knlEnv          * aEnv );

stlStatus knlDestroyRegionMem( knlRegionMem * aRegionMem,
                               knlEnv       * aEnv );

stlStatus knlStealRegionMem( knlRegionMem  * aRegionMem,
                             void         ** aFirstArena,
                             knlEnv        * aEnv );

stlInt64 knlGetArenaSize( void * aFirstArena );

stlUInt32 knlGetParentMemType( knlRegionMem * aRegionMem );

void * knlGetFirstArena( knlRegionMem * aRegionMem );

void knlEnableAllocation( knlRegionMem * aRegionMem );

void knlDisableAllocation( knlRegionMem * aRegionMem );

stlBool knlGetEnableAllocation( knlRegionMem * aRegionMem );

void knlSetEnableAllocation( knlRegionMem * aRegionMem,
                             stlBool        aEnableAlloc );

void knlTestPrintRegionMem( knlRegionMem * aRegionMem );

/** @} */
    
#endif /* _KNLREGIONMEM_H_ */
