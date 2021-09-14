/*******************************************************************************
 * knhStaticHash.c
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

/**
 * @file knhStaticHash.c
 * @brief Kernel Layer Static Hash implementation routines
 */

#include <stl.h>
#include <dtl.h>
#include <knlDef.h>
#include <knlRing.h>
#include <knlLatch.h>
#include <knlShmManager.h>
#include <knlRegionMem.h>

stlStatus knhCreateStaticHash( knlStaticHash ** aHash, 
                               stlUInt16        aBucketCount,
                               stlUInt16        aLinkOffset,
                               stlUInt16        aKeyOffset,
                               stlUInt32        aLatchMode,
                               stlBool          aIsOnShm,
                               knlRegionMem   * aMemMgr,
                               knlEnv         * aEnv )
{
    knlStaticHashBucket * sBucket; 
    stlUInt32             i;
    stlUInt32             sTotalSize;
    knlLatch            * sLatch;

    STL_PARAM_VALIDATE( aHash != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ((aLatchMode == KNL_STATICHASH_MODE_NOLATCH) ||
                         (aLatchMode == KNL_STATICHASH_MODE_HASHLATCH) ||
                         (aLatchMode == KNL_STATICHASH_MODE_BUCKETLATCH)), 
                        KNL_ERROR_STACK(aEnv) );

    sTotalSize = STL_SIZEOF(knlStaticHash) + (aBucketCount * STL_SIZEOF(knlStaticHashBucket));
    if( aLatchMode == KNL_STATICHASH_MODE_HASHLATCH )
    {
        sTotalSize += STL_SIZEOF(knlLatch);
    }
    else if( aLatchMode == KNL_STATICHASH_MODE_BUCKETLATCH )
    {
        sTotalSize += aBucketCount * STL_SIZEOF(knlLatch);
    }

    if( aMemMgr == NULL )
    {
        /**
         * Heap을 원하는 경우라면 Private Static Area를 사용해야 하지만 Static Hash가
         * session에 선언되어 있다면 문제를 발생시킬수 있다.
         * 이러한 경우는 지원하지 않는다.
         */
        STL_DASSERT( aIsOnShm == STL_TRUE );
        
        STL_TRY( knlAllocFixedStaticArea( sTotalSize,
                                          (void**)aHash,
                                          aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( knlAllocRegionMem( aMemMgr, sTotalSize, (void**)aHash, aEnv ) == STL_SUCCESS );
    }

    sBucket = (knlStaticHashBucket*)((*aHash) + 1);
    sLatch = (knlLatch*)(sBucket + aBucketCount);

    /* initialize Hash */
    (*aHash)->mBucketCount = aBucketCount;
    (*aHash)->mLatchMode = aLatchMode;
    (*aHash)->mLinkOffset = aLinkOffset;
    (*aHash)->mKeyOffset = aKeyOffset;
    (*aHash)->mIsOnShm = aIsOnShm;
    if( aLatchMode == KNL_STATICHASH_MODE_HASHLATCH )
    {
        STL_TRY( knlInitLatch( sLatch, aIsOnShm, aEnv) == STL_SUCCESS );
        (*aHash)->mHashLatch = sLatch;
    }
    else
    {
        (*aHash)->mHashLatch = NULL;
    }

   for( i = 0; i < aBucketCount; i++ )
    {
        STL_RING_INIT_HEADLINK(&sBucket->mList, aLinkOffset);
        if( aLatchMode == KNL_STATICHASH_MODE_BUCKETLATCH )
        {
            STL_TRY( knlInitLatch( sLatch, aIsOnShm, aEnv) == STL_SUCCESS );
            sBucket->mLatch = sLatch;
            sLatch++;
        }
        sBucket++;
    }

    sLatch = (knlLatch*)sBucket;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus knhInsertStaticHash( knlStaticHash            * aHash, 
                               knlStaticHashHashingFunc   aHashFunc,
                               void                     * aData,
                               knlEnv                   * aEnv )
{
    stlUInt32             sBucketNo;
    knlStaticHashBucket * sBucket;
    stlBool               sIsSuccess;

    STL_PARAM_VALIDATE( aHash != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aData != NULL, KNL_ERROR_STACK(aEnv) );

    sBucketNo = aHashFunc( (stlChar*)aData + aHash->mKeyOffset,
                           aHash->mBucketCount );
    sBucket = KNL_STATICHASH_GET_NTH_BUCKET(aHash, sBucketNo);

    if( aHash->mLatchMode == KNL_STATICHASH_MODE_HASHLATCH )
    {
        STL_TRY( knlAcquireLatch(aHash->mHashLatch,
                                 KNL_LATCH_MODE_EXCLUSIVE,
                                 KNL_LATCH_PRIORITY_NORMAL,
                                 STL_INFINITE_TIME,
                                 &sIsSuccess,
                                 aEnv ) == STL_SUCCESS );
    }
    else if( aHash->mLatchMode == KNL_STATICHASH_MODE_BUCKETLATCH )
    {
        STL_TRY( knlAcquireLatch(sBucket->mLatch,
                                 KNL_LATCH_MODE_EXCLUSIVE,
                                 KNL_LATCH_PRIORITY_NORMAL,
                                 STL_INFINITE_TIME,
                                 &sIsSuccess,
                                 aEnv ) == STL_SUCCESS );
    }

    STL_RING_ADD_LAST( &sBucket->mList, aData );

    if( aHash->mLatchMode == KNL_STATICHASH_MODE_HASHLATCH )
    {
        STL_TRY( knlReleaseLatch(aHash->mHashLatch,
                                 aEnv ) == STL_SUCCESS );
    }
    else if( aHash->mLatchMode == KNL_STATICHASH_MODE_BUCKETLATCH )
    {
        STL_TRY( knlReleaseLatch(sBucket->mLatch,
                                 aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus knhRemoveStaticHash( knlStaticHash            * aHash, 
                               knlStaticHashHashingFunc   aHashFunc,
                               void                     * aData,
                               knlEnv                   * aEnv )
{   
    stlUInt32             sBucketNo;
    knlStaticHashBucket * sBucket;
    stlBool               sIsSuccess;

    STL_PARAM_VALIDATE( aHash != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aData != NULL, KNL_ERROR_STACK(aEnv) );

    sBucketNo = aHashFunc( (stlChar*)aData + aHash->mKeyOffset,
                           aHash->mBucketCount );
    sBucket = KNL_STATICHASH_GET_NTH_BUCKET(aHash, sBucketNo);

    if( aHash->mLatchMode == KNL_STATICHASH_MODE_HASHLATCH )
    {
        STL_TRY( knlAcquireLatch(aHash->mHashLatch,
                                 KNL_LATCH_MODE_EXCLUSIVE,
                                 KNL_LATCH_PRIORITY_NORMAL,
                                 STL_INFINITE_TIME,
                                 &sIsSuccess,
                                 aEnv ) == STL_SUCCESS );
    }
    else if( aHash->mLatchMode == KNL_STATICHASH_MODE_BUCKETLATCH )
    {
        STL_TRY( knlAcquireLatch(sBucket->mLatch,
                                 KNL_LATCH_MODE_EXCLUSIVE,
                                 KNL_LATCH_PRIORITY_NORMAL,
                                 STL_INFINITE_TIME,
                                 &sIsSuccess,
                                 aEnv ) == STL_SUCCESS );
    }

    STL_RING_UNLINK( &sBucket->mList, aData );

    if( aHash->mLatchMode == KNL_STATICHASH_MODE_HASHLATCH )
    {
        STL_TRY( knlReleaseLatch(aHash->mHashLatch,
                                 aEnv ) == STL_SUCCESS );
    }
    else if( aHash->mLatchMode == KNL_STATICHASH_MODE_BUCKETLATCH )
    {
        STL_TRY( knlReleaseLatch(sBucket->mLatch,
                                 aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus knhFindStaticHash( knlStaticHash             * aHash, 
                             knlStaticHashHashingFunc    aHashFunc,
                             knlStaticHashCompareFunc    aCompareFunc,
                             void                      * aKey,
                             void                     ** aData,
                             knlEnv                    * aEnv )
{   
    stlUInt32             sBucketNo;
    knlStaticHashBucket * sBucket;
    void *                sCurItem = NULL;
    stlBool               sIsSuccess;

    STL_PARAM_VALIDATE( aHash != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aData != NULL, KNL_ERROR_STACK(aEnv) );

    sBucketNo = aHashFunc( aKey,
                           aHash->mBucketCount );
    sBucket = KNL_STATICHASH_GET_NTH_BUCKET(aHash, sBucketNo);

    if( aHash->mLatchMode == KNL_STATICHASH_MODE_HASHLATCH )
    {
        STL_TRY( knlAcquireLatch(aHash->mHashLatch,
                                 KNL_LATCH_MODE_SHARED,
                                 KNL_LATCH_PRIORITY_NORMAL,
                                 STL_INFINITE_TIME,
                                 &sIsSuccess,
                                 aEnv ) == STL_SUCCESS );
    }
    else if( aHash->mLatchMode == KNL_STATICHASH_MODE_BUCKETLATCH )
    {
        STL_TRY( knlAcquireLatch(sBucket->mLatch,
                                 KNL_LATCH_MODE_SHARED,
                                 KNL_LATCH_PRIORITY_NORMAL,
                                 STL_INFINITE_TIME,
                                 &sIsSuccess,
                                 aEnv ) == STL_SUCCESS );
    }

    STL_RING_FOREACH_ENTRY( &sBucket->mList, sCurItem )
    {
        if( aCompareFunc(aKey, (stlChar*)sCurItem + aHash->mKeyOffset) == 0 )
        {
            break;
        }
    }
    if( sCurItem == STL_RING_GET_DATA(&sBucket->mList, aHash->mLinkOffset) )
    {
        sCurItem = NULL;
    }

    if( aHash->mLatchMode == KNL_STATICHASH_MODE_HASHLATCH )
    {
        STL_TRY( knlReleaseLatch(aHash->mHashLatch,
                                 aEnv ) == STL_SUCCESS );
    }
    else if( aHash->mLatchMode == KNL_STATICHASH_MODE_BUCKETLATCH )
    {
        STL_TRY( knlReleaseLatch(sBucket->mLatch,
                                 aEnv ) == STL_SUCCESS );
    }

    *aData = sCurItem;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus knhGetFirstItem( knlStaticHash             * aHash, 
                           stlUInt32                 * aFirstBucketSeq,
                           void                     ** aData,
                           knlEnv                    * aEnv )
{ 
    knlStaticHashBucket * sBucket;
    stlBool               sIsSuccess;

    STL_PARAM_VALIDATE( aHash != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aData != NULL, KNL_ERROR_STACK(aEnv) );

    *aFirstBucketSeq = 0;
    sBucket = KNL_STATICHASH_GET_NTH_BUCKET(aHash, *aFirstBucketSeq);
    while( *aFirstBucketSeq < aHash->mBucketCount )
    {
        if( aHash->mLatchMode == KNL_STATICHASH_MODE_HASHLATCH )
        {
            STL_TRY( knlAcquireLatch(aHash->mHashLatch,
                                     KNL_LATCH_MODE_SHARED,
                                     KNL_LATCH_PRIORITY_NORMAL,
                                     STL_INFINITE_TIME,
                                     &sIsSuccess,
                                     aEnv ) == STL_SUCCESS );
        }
        else if( aHash->mLatchMode == KNL_STATICHASH_MODE_BUCKETLATCH )
        {
            STL_TRY( knlAcquireLatch(sBucket->mLatch,
                                     KNL_LATCH_MODE_SHARED,
                                     KNL_LATCH_PRIORITY_NORMAL,
                                     STL_INFINITE_TIME,
                                     &sIsSuccess,
                                     aEnv ) == STL_SUCCESS );
        }

        *aData = STL_RING_GET_FIRST_DATA( &sBucket->mList );

        if( aHash->mLatchMode == KNL_STATICHASH_MODE_HASHLATCH )
        {
            STL_TRY( knlReleaseLatch(aHash->mHashLatch,
                        aEnv ) == STL_SUCCESS );
        }
        else if( aHash->mLatchMode == KNL_STATICHASH_MODE_BUCKETLATCH )
        {
            STL_TRY( knlReleaseLatch(sBucket->mLatch,
                        aEnv ) == STL_SUCCESS );
        }

        if( *aData != STL_RING_GET_DATA( &sBucket->mList, aHash->mLinkOffset ) )
        {
            break;
        }

        (*aFirstBucketSeq)++;
        *aData = NULL;
        sBucket = KNL_STATICHASH_GET_NTH_BUCKET(aHash, *aFirstBucketSeq);
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus knhGetNextItem( knlStaticHash             * aHash, 
                          stlUInt32                 * aBucketSeq,
                          void                     ** aData,
                          knlEnv                    * aEnv )
{ 
    knlStaticHashBucket * sBucket;
    void                * sNextData;
    stlBool               sIsSuccess;

    STL_PARAM_VALIDATE( aHash != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aData != NULL, KNL_ERROR_STACK(aEnv) );

    sBucket = KNL_STATICHASH_GET_NTH_BUCKET(aHash, *aBucketSeq);

    while( *aBucketSeq < aHash->mBucketCount )
    {
        if( aHash->mLatchMode == KNL_STATICHASH_MODE_HASHLATCH )
        {
            STL_TRY( knlAcquireLatch(aHash->mHashLatch,
                                     KNL_LATCH_MODE_SHARED,
                                     KNL_LATCH_PRIORITY_NORMAL,
                                     STL_INFINITE_TIME,
                                     &sIsSuccess,
                                     aEnv ) == STL_SUCCESS );
        }
        else if( aHash->mLatchMode == KNL_STATICHASH_MODE_BUCKETLATCH )
        {
            STL_TRY( knlAcquireLatch(sBucket->mLatch,
                                     KNL_LATCH_MODE_SHARED,
                                     KNL_LATCH_PRIORITY_NORMAL,
                                     STL_INFINITE_TIME,
                                     &sIsSuccess,
                                     aEnv ) == STL_SUCCESS );
        }

        if( *aData == NULL )
        {
            sNextData = STL_RING_GET_FIRST_DATA( &sBucket->mList );
        }
        else
        {
            sNextData = STL_RING_GET_NEXT_DATA( &sBucket->mList, *aData );
        }

        if( aHash->mLatchMode == KNL_STATICHASH_MODE_HASHLATCH )
        {
            STL_TRY( knlReleaseLatch(aHash->mHashLatch,
                        aEnv ) == STL_SUCCESS );
        }
        else if( aHash->mLatchMode == KNL_STATICHASH_MODE_BUCKETLATCH )
        {
            STL_TRY( knlReleaseLatch(sBucket->mLatch,
                        aEnv ) == STL_SUCCESS );
        }

        if( sNextData != STL_RING_GET_DATA( &sBucket->mList, aHash->mLinkOffset ) )
        {
            *aData = sNextData;
            break;
        }

        (*aBucketSeq)++;
        *aData = NULL;
        sBucket = KNL_STATICHASH_GET_NTH_BUCKET(aHash, *aBucketSeq);
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} */
