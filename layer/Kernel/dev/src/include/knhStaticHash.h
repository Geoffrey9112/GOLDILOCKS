/*******************************************************************************
 * knhStaticHash.h
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


#ifndef _KNH_STATICHASH_H_
#define _KNH_STATICHASH_H_ 1

/**
 * @file knhStaticHash.h
 * @brief Kernel Layer Static Hash Internal Routines
 */

/**
 * @defgroup knhStaticHash Static Hash
 * @ingroup knInternal
 * @{
 */

stlStatus knhCreateStaticHash( knlStaticHash ** aHash, 
                               stlUInt16        aBucketCount,
                               stlUInt16        aLinkOffset,
                               stlUInt16        aKeyOffset,
                               stlUInt32        aLatchMode,
                               stlBool          aIsOnShm,
                               knlRegionMem   * aMemMgr,
                               knlEnv         * aEnv );

stlStatus knhInsertStaticHash( knlStaticHash            * aHash, 
                               knlStaticHashHashingFunc   aHashFunc,
                               void                     * aData,
                               knlEnv                   * aEnv );

stlStatus knhRemoveStaticHash( knlStaticHash            * aHash, 
                               knlStaticHashHashingFunc   aHashFunc,
                               void                     * aData,
                               knlEnv                   * aEnv );

stlStatus knhFindStaticHash( knlStaticHash             * aHash, 
                             knlStaticHashHashingFunc    aHashFunc,
                             knlStaticHashCompareFunc    aCompareFunc,
                             void                      * aKey,
                             void                     ** aData,
                             knlEnv                    * aEnv );

stlStatus knhGetFirstItem( knlStaticHash             * aHash, 
                           stlUInt32                 * aFirstBucketSeq,
                           void                     ** aData,
                           knlEnv                    * aEnv );

stlStatus knhGetNextItem( knlStaticHash             * aHash, 
                          stlUInt32                 * aBucketSeq,
                          void                     ** aData,
                          knlEnv                    * aEnv );

/** @} */
    
#endif /* _KNH_STATICHASH_H_ */
