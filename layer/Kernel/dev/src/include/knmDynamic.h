/*******************************************************************************
 * knmDynamic.h
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


#ifndef _KNMDYNAMIC_H_
#define _KNMDYNAMIC_H_ 1

/**
 * @file knmDynamic.h
 * @brief Kernel Layer Dynamic Memory Management Internal Routines
 */

/**
 * @defgroup knmDynamic Dynamic Memory
 * @ingroup knmMemory
 * @internal
 * @{
 */

stlStatus knmAllocChunk( knlMemController * aMemController,
                         knlDynamicMem    * aDynamicMem,
                         knmChunkHeader  ** aChunkHeader,
                         stlBool            aIsInit,
                         knlEnv           * aEnv);

stlStatus knmFreeChunk( knlMemController * aMemController,
                        knlDynamicMem    * aDynamicMem,
                        knmChunkHeader   * aChunkHeader,
                        knlEnv           * aEnv);

stlStatus knmFreeAllChunk( knlMemController * aMemController,
                           knlDynamicMem    * aDynamicMem,
                           knlEnv           * aEnv);

stlStatus knmFindFreeSpace( knlDynamicMem   * aDynamicMem,
                            stlUInt32         aSize,
                            knmBlockHeader ** aFreeBlock,
                            knlEnv          * aEnv );

stlStatus knmFindInFreeList( knmChunkHeader  * aChunkHeader,
                             stlUInt32         aSize,
                             knmBlockHeader ** aFreeBlock,
                             knlEnv          * aEnv );

stlStatus knmFindChunk( knlDynamicMem   * aDynamicMem,
                        knmBlockHeader  * aBlockHeader,
                        knmChunkHeader ** aChunkHeader,
                        stlBool         * aIsDefaultChunk,
                        knlEnv          * aEnv );

stlStatus knmAllocInChunk( knmChunkHeader  * aChunkHeader,
                           stlUInt32         aSize,
                           knmBlockHeader ** aAllocBlock,
                           knlEnv          * aEnv );

stlStatus knmAllocInFreeBlock( knlDynamicMem   * aDynamicMem,
                               knmChunkHeader  * aChunkHeader,
                               stlUInt32         aSize,
                               knmBlockHeader  * aFreeBlock,
                               knmBlockHeader ** aAllocBlock,
                               knlEnv          * aEnv );

stlStatus knmDAlloc( knlMemController * aMemController,
                     knlDynamicMem    * aDynamicMem,
                     void             * aRequestAllocator,
                     knlAllocatorType   aRequestAllocatorType,
                     stlUInt32          aSize,
                     void            ** aAddr,
                     knlEnv           * aEnv );

stlStatus knmCacheAlignedDAlloc( knlMemController * aMemController,
                                 knlDynamicMem    * aDynamicMem,
                                 void             * aRequestAllocator,
                                 knlAllocatorType   aRequestAllocatorType,
                                 stlUInt32          aSize,
                                 void            ** aAddr,
                                 knlEnv           * aEnv );

stlStatus knmDFree( knlMemController * aMemController,
                    knlDynamicMem    * aDynamicMem,
                    void             * aAddr,
                    knlEnv           * aEnv );

stlStatus knmCacheAlignedDFree( knlMemController * aMemController,
                                knlDynamicMem    * aDynamicMem,
                                void             * aAddr,
                                knlEnv           * aEnv );

stlStatus knmAnalyzeDynamicMem( knlDynamicMem    * aDynamicMem,
                                stlUInt64        * aTotalSize,
                                stlUInt64        * aUsedSize,
                                knlEnv           * aEnv );

stlStatus knmRegisterDynamicMem( knlDynamicMem * aDynamicMem,
                                 knlEnv        * aEnv );

stlStatus knmUnregisterDynamicMem( knlDynamicMem * aDynamicMem,
                                   knlEnv        * aEnv );

void knmTestPrintAllocList( knlDynamicMem * aDynamicMem );
void knmTestPrintFreeList( knlDynamicMem * aDynamicMem );

/** @} */
    
#endif /* _KNMDYNAMIC_H_ */
