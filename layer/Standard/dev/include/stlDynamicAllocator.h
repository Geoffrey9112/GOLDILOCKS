/*******************************************************************************
 * stlDynamicAllocator.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: stlDynamicAllocator.h 2116 2011-10-20 02:57:10Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _STLDYNAMICALLOCATOR_H_
#define _STLDYNAMICALLOCATOR_H_ 1

/**
 * @file stlDynamicAllocator.h
 * @brief Memory Dynamic Allocator
 */

STL_BEGIN_DECLS

/**
 * @defgroup stlDynamicAllocator Memory Dynamic Allocator
 * @ingroup STL
 * @{
 */

stlStatus stlCreateDynamicAllocator( stlDynamicAllocator * aAllocator,
                                     stlUInt64             aInitSize,
                                     stlUInt64             aNextSize,
                                     stlErrorStack       * aErrorStack );

stlStatus stlAllocDynamicMem( stlDynamicAllocator * aAllocator,
                              stlInt32              aAllocSize,
                              void               ** aAddr,
                              stlErrorStack       * aErrorStack );

stlStatus stlFreeDynamicMem( stlDynamicAllocator  * aAllocator,
                             void                 * aAddr,
                             stlErrorStack        * aErrorStack );

stlStatus stlDestroyDynamicAllocator( stlDynamicAllocator * aAllocator,
                                      stlErrorStack       * aErrorStack );

/** @} */

STL_END_DECLS

#endif /* _STLDYNAMICALLOCATOR_H_ */

