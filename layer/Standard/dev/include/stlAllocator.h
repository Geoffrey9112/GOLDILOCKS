/*******************************************************************************
 * stlAllocator.h
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


#ifndef _STLALLOCATOR_H_
#define _STLALLOCATOR_H_ 1

/**
 * @file stlAllocator.h
 * @brief Memory Region Allocator
 */

STL_BEGIN_DECLS

/**
 * @defgroup stlAllocator Memory Region Allocator
 * @ingroup STL
 * @{
 */

stlStatus stlCreateRegionAllocator( stlAllocator   * aAllocator,
                                    stlUInt64        aInitSize,
                                    stlUInt64        aNextSize,
                                    stlErrorStack  * aErrorStack );

stlStatus stlAllocRegionMem( stlAllocator   * aAllocator,
                             stlInt32         aAllocSize,
                             void          ** aAddr,
                             stlErrorStack  * aErrorStack );

stlStatus stlClearRegionMem( stlAllocator   * aAllocator,
                             stlErrorStack  * aErrorStack );

stlStatus stlMarkRegionMem( stlAllocator   * aAllocator,
                            stlRegionMark  * aMark,
                            stlErrorStack  * aErrorStack );

stlStatus stlRestoreRegionMem( stlAllocator   * aAllocator,
                               stlRegionMark  * aMark,
                               stlBool          aFree,
                               stlErrorStack  * aErrorStack );

stlStatus stlDestroyRegionAllocator( stlAllocator   * aAllocator,
                                     stlErrorStack  * aErrorStack );

stlStatus stlInitializeArrayAllocator( stlArrayAllocator    * aAllocator,
                                       stlInt64               aElementBodySize,
                                       stlInt64               aElementCount,
                                       stlInt64             * aTotalUsedBytes,
                                       stlSize                aAlignment,
                                       stlErrorStack        * aErrorStack );

stlStatus stlInitializeArrayAllocatorByTotalSize( stlArrayAllocator    * aAllocator,
                                                  stlInt64               aElementBodySize,
                                                  stlInt64               aTotalUsedBytes,
                                                  stlInt64             * aElementCount,
                                                  stlSize                aAlignment,
                                                  stlErrorStack        * aErrorStack );

stlStatus stlFinalizeArrayAllocator( stlArrayAllocator    * aAllocator,
                                     stlErrorStack        * aErrorStack );

stlStatus stlAllocElement( stlArrayAllocator   * aAllocator,
                           void               ** aItem,
                           stlErrorStack       * aErrorStack );

stlStatus stlFreeElement( stlArrayAllocator   * aAllocator,
                          void                * aItem,
                          stlErrorStack       * aErrorStack );

stlStatus stlGetArrayInfo( stlArrayAllocator   * aAllocator,
                           stlInt32            * aTotalUsedBytes,
                           stlInt32            * aTotalElementCount,
                           stlInt32            * aUsedCount,
                           stlInt32            * aElementBodySize,
                           stlInt32            * aElementSize,
                           stlErrorStack       * aErrorStack );

/** @} */

STL_END_DECLS

#endif /* _STLALLOCATOR_H_ */

