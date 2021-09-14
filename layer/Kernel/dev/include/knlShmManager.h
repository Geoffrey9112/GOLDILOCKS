/*******************************************************************************
 * knlShmManager.h
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


#ifndef _KNLSHMMANAGER_H_
#define _KNLSHMMANAGER_H_ 1

/**
 * @file knlShmManager.h
 * @brief Kernel Layer Shared Memory Segment Manager Routines
 */


/**
 * @defgroup knlShmManager Shared Memory Segment Manager
 * @ingroup knExternal
 * @{
 */

/*
 * entry point
 */
stlStatus knlGetEntryPoint( stlLayerClass aLayer,
                            void       ** aAddr,
                            knlEnv      * aEnv );

void *  knlGetLayerEntryAddr( void          * aBaseAddr,
                              stlLayerClass   aLayer );

/*
 * static area
 */
stlStatus knlAllocFixedStaticArea( stlSize   aSize,
                                   void   ** aAddr,
                                   knlEnv  * aEnv );

stlStatus knlCacheAlignedAllocFixedStaticArea( stlSize   aSize,
                                               void   ** aAddr,
                                               knlEnv  * aEnv );

stlStatus knlMarkStaticArea( knlStaticAreaMark * aMark,
                             knlEnv            * aEnv );

stlStatus knlRestoreStaticArea( knlStaticAreaMark * aMark,
                                knlEnv            * aEnv );

/*
 * database area
 */
stlStatus knlCreateDatabaseArea( stlChar   * aName,
                                 stlUInt16 * aIdx,
                                 stlSize     aReqSize,
                                 knlEnv    * aEnv );

stlStatus knlDestroyDatabaseArea(stlUInt16   aIdx,
                                 knlEnv    * aEnv );

/*
 * pysical address
 */
stlStatus knlGetShmSegmentAddrUnsafe( stlUInt16   aIdx,
                                      void     ** aAddr,
                                      knlEnv    * aEnv );

stlStatus knlGetPhysicalAddr( knlLogicalAddr   aLogicalAddr,
                              void          ** aPysicalAddr,
                              knlEnv         * aEnv );

stlBool knlIsValidStaticAddress( void    * aMemAddr,
                                 stlSize   aMemSize );

stlStatus knlIsValidShmSegment( stlUInt16   aShmIndex,
                                stlBool   * aIsValid,
                                knlEnv    * aEnv );

stlStatus knlReattachShmSegment( stlUInt16   aShmIndex,
                                 knlEnv    * aEnv );

void *knlToPhysicalAddr( knlLogicalAddr aLogicalAddr );

/*
 * logical address
 */
stlStatus knlGetLogicalAddr( void           * aPysicalAddr,
                             knlLogicalAddr * aLogicalAddr,
                             knlEnv         * aEnv );

/* 
 * Shm Idx
 */
stlStatus knlGetSegmentIdxByName( stlChar    * aName,
                                  stlUInt16  * aIdx,
                                  knlEnv     * aEnv );

/*
 * test function
 */
void knlTestPrintShmManager();
void knlTestPrintAllocVariableStaticArea();
void knlTestPrintFreeVariableStaticArea();

/** @} */
    
#endif /* _KNLSHMMANAGER_H_ */
