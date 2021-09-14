/*******************************************************************************
 * stlStaticHash.h
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

#ifndef _STLSTATICHASH_H_
#define _STLSTATICHASH_H_ 1

/**
 * @file stlStaticHash.h
 * @brief Hash Table Library.
 */

#include <stlDef.h>

STL_BEGIN_DECLS

/************************************************************
 * Static Hash
 ************************************************************/

stlUInt32 stlGetHashValueString( stlChar * aName );

stlStatus stlCreateStaticHash( stlStaticHash   **aHash,
                               stlUInt16         aBucketCount,
                               stlUInt16         aLinkOffset,
                               stlUInt16         aKeyOffset,
                               stlUInt32         aLatchMode,
                               stlErrorStack    *aErrorStack );

stlStatus stlDestroyStaticHash( stlStaticHash  **aHash,
                                stlErrorStack   *aErrorStack );

stlStatus stlInsertStaticHash( stlStaticHash             *aHash,
                               stlStaticHashHashingFunc   aHashFunc,
                               void                      *aData,
                               stlErrorStack             *aErrorStack );

stlStatus stlInsertUniqueStaticHash( stlStaticHash             *aHash,
                                     stlStaticHashHashingFunc   aHashFunc,
                                     stlStaticHashCompareFunc   aCompareFunc,
                                     void                      *aData,
                                     stlBool                   *aIsDuplicate,
                                     stlErrorStack             *aErrorStack );

stlStatus stlRemoveStaticHash( stlStaticHash             *aHash,
                               stlStaticHashHashingFunc   aHashFunc,
                               void                      *aData,
                               stlErrorStack             *aErrorStack );

stlStatus stlFindStaticHash( stlStaticHash             *aHash,
                             stlStaticHashHashingFunc   aHashFunc,
                             stlStaticHashCompareFunc   aCompareFunc,
                             void                      *aKey,
                             void                     **aData,
                             stlErrorStack             *aErrorStack );

stlStatus stlGetFirstStaticHashItem( stlStaticHash    *aHash,
                                     stlUInt32        *aFirstBucketSeq,
                                     void            **aData,
                                     stlErrorStack    *aErrorStack );

stlStatus stlGetNextStaticHashItem( stlStaticHash    *aHash,
                                    stlUInt32        *aFirstBucketSeq,
                                    void            **aData,
                                    stlErrorStack    *aErrorStack );

STL_END_DECLS

#endif /* _STLSTATICHASH_H_ */
