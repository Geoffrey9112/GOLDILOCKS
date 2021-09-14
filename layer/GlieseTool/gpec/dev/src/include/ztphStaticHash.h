/*******************************************************************************
 * ztphStaticHash.h
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

#ifndef _ZTPHSTATICHASH_H_
#define _ZTPHSTATICHASH_H_ 1

/**
 * @file ztphStaticHash.h
 * @brief Gliese Embedded SQL in C precompiler library StaticHash header file.
 */

#include <stl.h>
#include <ztpDef.h>

/**
 * @brief Static Hash
 */

#define ZTPH_STATICHASH_GET_NTH_BUCKET(hashPtr,Seq)  \
    ((ztphStaticHashBucket*)(((ztphStaticHash*)hashPtr) + 1) + (Seq))

stlUInt32  ztphGetHashValueString( stlChar * aName );

stlStatus ztphCreateStaticHash(stlAllocator    *aAllocator,
                               stlErrorStack   *aErrorStack,
                               ztphStaticHash **aHash,
                               stlUInt16        aBucketCount,
                               stlUInt16        aLinkOffset,
                               stlUInt16        aKeyOffset);

stlStatus ztphInsertStaticHash(stlErrorStack             *aErrorStack,
                               ztphStaticHash            *aHash,
                               ztphStaticHashHashingFunc  aHashFunc,
                               void                      *aData);

stlStatus ztphRemoveStaticHash(stlErrorStack             *aErrorStack,
                               ztphStaticHash            *aHash,
                               ztphStaticHashHashingFunc  aHashFunc,
                               void                      *aData);


stlStatus ztphFindStaticHash(stlErrorStack              *aErrorStack,
                             ztphStaticHash             *aHash,
                             ztphStaticHashHashingFunc   aHashFunc,
                             ztphStaticHashCompareFunc   aCompareFunc,
                             void                       *aKey,
                             void                      **aData);

stlStatus ztphGetFirstItem(stlErrorStack   *aErrorStack,
                           ztphStaticHash  *aHash,
                           stlUInt32       *aFirstBucketSeq,
                           void           **aData);

stlStatus ztphGetNextItem(stlErrorStack   *aErrorStack,
                          ztphStaticHash  *aHash,
                          stlUInt32       *aBucketSeq,
                          void           **aData);

/** @} */

#endif /* _ZTPHSTATICHASH_H_ */
