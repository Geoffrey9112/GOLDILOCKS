/*******************************************************************************
 * knlDynamicMem.h
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


#ifndef _KNLDYNAMICMEM_H_
#define _KNLDYNAMICMEM_H_ 1

/**
 * @file knlDynamicMem.h
 * @brief Kernel Layer Dynamic Memory Management
 */

/**
 * @defgroup knlDynamicMem Dynamic Memory
 * @ingroup knlMemManager
 * @{
 */

stlStatus knlCreateDynamicMem( knlDynamicMem    * aDynamicMem,
                               knlDynamicMem    * aParentMem,
                               knlAllocatorId     aAllocatorId,
                               stlUInt32          aStorageType,
                               stlUInt64          aInitSize,
                               stlUInt64          aNextSize,
                               knlMemController * aController,
                               knlEnv           * aEnv );

stlStatus knlDestroyDynamicMem( knlDynamicMem * aDynamicMem,
                                knlEnv        * aEnv );

stlStatus knlAllocDynamicMem( knlDynamicMem  * aDynamicMem,
                              stlUInt32        aSize,
                              void          ** aAddr,
                              knlEnv         * aEnv );

stlStatus knlCacheAlignedAllocDynamicMem( knlDynamicMem  * aDynamicMem,
                                          stlUInt32        aSize,
                                          void          ** aAddr,
                                          knlEnv         * aEnv );

stlStatus knlFreeDynamicMem( knlDynamicMem * aDynamicMem,
                             void          * aAddr,
                             knlEnv        * aEnv );

stlStatus knlCacheAlignedFreeDynamicMem( knlDynamicMem * aDynamicMem,
                                         void          * aAddr,
                                         knlEnv        * aEnv );

stlStatus knlFreeAllArena( void          * aFirstArena,
                           knlDynamicMem * aDynamicMem,
                           knlEnv        * aEnv );

stlStatus knlCreateProcDynamicMem( knlEnv * aEnv );

stlStatus knlDestroyProcDynamicMem( knlEnv * aEnv );

knlDynamicMem * knlGetProcDynamicMem( );

void knlValidateDynamicMem( knlDynamicMem * aDynamicMem,
                            knlEnv        * aEnv );
void knlTestPrintAllocDynamicMem( knlDynamicMem * aDynamicMem );
void knlTestPrintFreeDynamicMem( knlDynamicMem * aDynamicMem );

/** @} */
    
#endif /* _KNLDYNAMICMEM_H_ */
