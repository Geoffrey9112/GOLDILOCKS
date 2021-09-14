/*******************************************************************************
 * knlStaticHash.h
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


#ifndef _KNL_STATICHASH_H_
#define _KNL_STATICHASH_H_ 1

/**
 * @file knlStaticHash.h
 * @brief Kernel Layer Static Hash Routines
 */

/**
 * @defgroup knlStaticHash Static Hash
 * @ingroup knExternal 
 * @{
 */

stlStatus knlCreateStaticHash( knlStaticHash ** aHashSpace, 
                               stlUInt16        aBucketCount,
                               stlUInt16        aLinkOffset,
                               stlUInt16        aKeyOffset,
                               stlUInt32        aLatchMode,
                               stlBool          aIsOnShm,
                               knlRegionMem   * aMemMgr,
                               knlEnv         * aEnv );

stlStatus knlInsertStaticHash( knlStaticHash            * aHash, 
                               knlStaticHashHashingFunc   aHashFunc,
                               void                     * aData,
                               knlEnv                   * aEnv );

stlStatus knlRemoveStaticHash( knlStaticHash            * aHash, 
                               knlStaticHashHashingFunc   aHashFunc,
                               void                     * aData,
                               knlEnv                   * aEnv );

stlStatus knlFindStaticHash( knlStaticHash             * aHash, 
                             knlStaticHashHashingFunc    aHashFunc,
                             knlStaticHashCompareFunc    aCompareFunc,
                             void                      * aKey,
                             void                     ** aData,
                             knlEnv                    * aEnv );

stlStatus knlGetFirstStaticHashItem( knlStaticHash             * aHash, 
                                     stlUInt32                 * aFirstBucketSeq,
                                     void                     ** aData,
                                     knlEnv                    * aEnv );

stlStatus knlGetNextStaticHashItem( knlStaticHash             * aHash, 
                                    stlUInt32                 * aBucketSeq,
                                    void                     ** aData,
                                    knlEnv                    * aEnv );
/** @} */
    
#endif /* _KNL_STATICHASH_H_ */
