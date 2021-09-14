/*******************************************************************************
 * knmRegion.h
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


#ifndef _KNMREGION_H_
#define _KNMREGION_H_ 1

/**
 * @file knmRegion.h
 * @brief Kernel Layer Region-based Memory Management Internal Routines
 */

/**
 * @defgroup knmRegion Region-based Memory
 * @ingroup knmMemory
 * @internal
 * @{
 */

stlStatus knmAllocArena( knlRegionMem    * aRegionMem,
                         knmArenaHeader ** aArenaHeader,
                         stlBool           aIsInit,
                         knlEnv          * aEnv);

stlStatus knmFreeArena( knlRegionMem   * aRegionMem,
                        stlRingHead    * aRing,
                        knmArenaHeader * aArenaHeader,
                        knlEnv         * aEnv);

stlStatus knmFreeAllArena( knlRegionMem * aRegionMem,
                           knlEnv       * aEnv);

stlStatus knmRFind( knlRegionMem    * aRegionMem,
                    stlUInt32         aSize,
                    knmArenaHeader ** aFreeArena,
                    knlEnv          * aEnv );

stlStatus knmRAlloc( knlRegionMem   * aRegionMem,
                     stlUInt32        aSize,
                     void          ** aAddr,
                     knlEnv         * aEnv );

stlStatus knmRClear( knlRegionMem   * aRegionMem,
                     knlEnv         * aEnv );

stlStatus knmRMark( knlRegionMem  * aRegionMem,
                    knlRegionMark * aMark,
                    knlEnv        * aEnv );

stlStatus knmRReset( knlRegionMem  * aRegionMem,
                     knlRegionMark * aMark,
                     stlBool         aFreeChunk,
                     knlEnv        * aEnv );

void knmRPrint( knlRegionMem  * aRegionMem );
/** @} */
    
#endif /* _KNMREGION_H_ */
