/*******************************************************************************
 * stlStaticHash.c
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
 * @file stlStaticHash.c
 * @brief Kernel Layer Static Hash implementation routines
 */

#include <stl.h>


#define STL_ROT(x,k) (((x)<<(k)) | ((x)>>(32-(k))))

/**
 * Hash Value 계산 from postgres
 */
stlUInt32  stlGetHashValueString( stlChar * aName )
{
    stlSize   sLen = 0;
    stlChar * sStr = NULL;
    stlUInt32  i;
    stlUInt32  j;
    stlUInt32  k;

    sLen = stlStrlen( aName );
    k = 0x9e3779b9 + sLen + 3923095;

    i = j = k;
    sStr = aName;

    while ( sLen >= 12 )
    {
		i += ( sStr[3] +
               ((stlUInt32) sStr[2]  << 8) +
               ((stlUInt32) sStr[1]  << 16) +
               ((stlUInt32) sStr[0]  << 24) );
		j += ( sStr[7] +
               ((stlUInt32) sStr[6]  << 8) +
               ((stlUInt32) sStr[5]  << 16) +
               ((stlUInt32) sStr[4]  << 24) );
		k += ( sStr[11] +
               ((stlUInt32) sStr[10] << 8) +
               ((stlUInt32) sStr[9]  << 16) +
               ((stlUInt32) sStr[8]  << 24) );
        sStr += 12;
        sLen -= 12;
    }

    switch ( sLen )
    {
        case 11:
            k += ((stlUInt32) sStr[10] << 8);
        case 10:
            k += ((stlUInt32) sStr[9] << 16);
        case 9:
            k += ((stlUInt32) sStr[8] << 24);
        case 8:
            j += sStr[7];
        case 7:
            j += ((stlUInt32) sStr[6] << 8);
        case 6:
            j += ((stlUInt32) sStr[5] << 16);
        case 5:
            i += ((stlUInt32) sStr[4] << 24);
        case 4:
            i += sStr[3];
        case 3:
            i += ((stlUInt32) sStr[2] << 8);
        case 2:
            i += ((stlUInt32) sStr[1] << 16);
        case 1:
            i += ((stlUInt32) sStr[0] << 24);
        case 0:
            /* nothing left */
        default:
            break;
    }

    k ^= j;
    k -= STL_ROT(j,14);

    i ^= k;
    i -= STL_ROT(k,11);

    j ^= i;
    j -= STL_ROT(i,25);

    k ^= j;
    k -= STL_ROT(j,16);

    i ^= k;
    i -= STL_ROT(k, 4);

    j ^= i;
    j -= STL_ROT(i,14);

    k ^= j;
    k -= STL_ROT(j,24);

    return k;
}

stlStatus stlCreateStaticHash(stlStaticHash   **aHash,
                              stlUInt16         aBucketCount,
                              stlUInt16         aLinkOffset,
                              stlUInt16         aKeyOffset,
                              stlUInt32         aLockMode,
                              stlErrorStack    *aErrorStack)
{
    stlStaticHashBucket *sBucket;
    stlUInt32            i;
    stlUInt32            sTotalSize;
    stlSpinLock         *sSpinLock;

    STL_PARAM_VALIDATE( aHash != NULL, aErrorStack );
    STL_PARAM_VALIDATE( ((aLockMode == STL_STATICHASH_MODE_NOLOCK) ||
                         (aLockMode == STL_STATICHASH_MODE_HASHLOCK) ||
                         (aLockMode == STL_STATICHASH_MODE_BUCKETLOCK)),
                        aErrorStack );

    sTotalSize = STL_SIZEOF(stlStaticHash) + (aBucketCount * STL_SIZEOF(stlStaticHashBucket));
    if( aLockMode == STL_STATICHASH_MODE_HASHLOCK )
    {
        sTotalSize += STL_SIZEOF(stlSpinLock);
    }
    else if( aLockMode == STL_STATICHASH_MODE_BUCKETLOCK )
    {
        sTotalSize += aBucketCount * STL_SIZEOF(stlSpinLock);
    }

    STL_TRY(stlAllocHeap((void **)aHash,
                         sTotalSize,
                         aErrorStack)
            == STL_SUCCESS);

    sBucket = (stlStaticHashBucket*)((*aHash) + 1);
    sSpinLock = (stlSpinLock*)(sBucket + aBucketCount);

    /* initialize Hash */
    (*aHash)->mBucketCount = aBucketCount;
    (*aHash)->mLockMode = aLockMode;
    (*aHash)->mLinkOffset = aLinkOffset;
    (*aHash)->mKeyOffset = aKeyOffset;
    if( aLockMode == STL_STATICHASH_MODE_HASHLOCK )
    {
        stlInitSpinLock(sSpinLock);
        (*aHash)->mHashLock = sSpinLock;
    }
    else
    {
        (*aHash)->mHashLock = NULL;
    }

   for( i = 0; i < aBucketCount; i++ )
    {
        STL_RING_INIT_HEADLINK(&sBucket->mList, aLinkOffset);
        if( aLockMode == STL_STATICHASH_MODE_BUCKETLOCK )
        {
            stlInitSpinLock(sSpinLock);
            sBucket->mLock = sSpinLock;
            sSpinLock++;
        }
        sBucket++;
    }

    sSpinLock = (stlSpinLock*)sBucket;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stlDestroyStaticHash( stlStaticHash  **aHash,
                                stlErrorStack   *aErrorStack )
{
    /*
    stlUInt32     sBucketSeq;
    void         *sData = NULL;

    STL_TRY(stlGetFirstStaticHashItem( *aHash,
                             & sBucketSeq,
                             (void **) & sData,
                             aErrorStack )
            == STL_SUCCESS);

    while( sData != NULL )
    {
        // sData link를 끊고, 해제 작업을 수행해야 함

        STL_TRY(stlGetNextStaticHashItem( *aHash,
                                & sBucketSeq,
                                (void **) & sData,
                                aErrorStack )
                == STL_SUCCESS);
    }
    */

    stlFreeHeap( *aHash );

    *aHash = NULL;

    return STL_SUCCESS;
/*
    STL_FINISH;

    return STL_FAILURE;
*/
}

stlStatus stlInsertStaticHash( stlStaticHash             *aHash,
                               stlStaticHashHashingFunc   aHashFunc,
                               void                      *aData,
                               stlErrorStack             *aErrorStack )
{
    stlUInt32            sBucketNo;
    stlStaticHashBucket *sBucket;
    stlUInt64            sMissCount;

    STL_PARAM_VALIDATE( aHash != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aData != NULL, aErrorStack );

    sBucketNo = aHashFunc( (stlChar*)aData + aHash->mKeyOffset,
                           aHash->mBucketCount );
    sBucket = STL_STATICHASH_GET_NTH_BUCKET(aHash, sBucketNo);

    if( aHash->mLockMode == STL_STATICHASH_MODE_HASHLOCK )
    {
        stlAcquireSpinLock(aHash->mHashLock,
                           &sMissCount);
    }
    else if( aHash->mLockMode == STL_STATICHASH_MODE_BUCKETLOCK )
    {
        stlAcquireSpinLock(sBucket->mLock,
                           &sMissCount);
    }

    STL_RING_ADD_LAST( &sBucket->mList, aData );

    if( aHash->mLockMode == STL_STATICHASH_MODE_HASHLOCK )
    {
        stlReleaseSpinLock(aHash->mHashLock);
    }
    else if( aHash->mLockMode == STL_STATICHASH_MODE_BUCKETLOCK )
    {
        stlReleaseSpinLock(sBucket->mLock);
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stlInsertUniqueStaticHash( stlStaticHash             *aHash,
                                     stlStaticHashHashingFunc   aHashFunc,
                                     stlStaticHashCompareFunc   aCompareFunc,
                                     void                      *aData,
                                     stlBool                   *aIsDuplicate,
                                     stlErrorStack             *aErrorStack )
{
    stlUInt32            sBucketNo;
    stlStaticHashBucket *sBucket;
    void                *sCurItem = NULL;
    stlUInt64            sMissCount;

    STL_PARAM_VALIDATE( aHash != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aData != NULL, aErrorStack );

    sBucketNo = aHashFunc( (stlChar*)aData + aHash->mKeyOffset,
                           aHash->mBucketCount );
    sBucket = STL_STATICHASH_GET_NTH_BUCKET(aHash, sBucketNo);

    *aIsDuplicate = STL_FALSE;

    if( aHash->mLockMode == STL_STATICHASH_MODE_HASHLOCK )
    {
        stlAcquireSpinLock(aHash->mHashLock,
                           &sMissCount);
    }
    else if( aHash->mLockMode == STL_STATICHASH_MODE_BUCKETLOCK )
    {
        stlAcquireSpinLock(sBucket->mLock,
                           &sMissCount);
    }

    STL_RING_FOREACH_ENTRY( &sBucket->mList, sCurItem )
    {
        if( aCompareFunc((stlChar*)aData + aHash->mKeyOffset,
                         (stlChar*)sCurItem + aHash->mKeyOffset) == 0 )
        {
            break;
        }
    }

    if( sCurItem == STL_RING_GET_DATA(&sBucket->mList, aHash->mLinkOffset) )
    {
        /*
         * 현재 Hash table에 없음
         */
        STL_RING_ADD_LAST( &sBucket->mList, aData );
    }
    else
    {
        /*
         * Hash table에서 같은 Key를 가진 entry를 발견
         */
        *aIsDuplicate = STL_TRUE;
    }

    if( aHash->mLockMode == STL_STATICHASH_MODE_HASHLOCK )
    {
        stlReleaseSpinLock(aHash->mHashLock);
    }
    else if( aHash->mLockMode == STL_STATICHASH_MODE_BUCKETLOCK )
    {
        stlReleaseSpinLock(sBucket->mLock);
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stlRemoveStaticHash( stlStaticHash             *aHash,
                               stlStaticHashHashingFunc   aHashFunc,
                               void                      *aData,
                               stlErrorStack             *aErrorStack )
{
    stlUInt32            sBucketNo;
    stlStaticHashBucket *sBucket;
    stlUInt64            sMissCount;

    STL_PARAM_VALIDATE( aHash != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aData != NULL, aErrorStack );

    sBucketNo = aHashFunc( (stlChar*)aData + aHash->mKeyOffset,
                           aHash->mBucketCount );
    sBucket = STL_STATICHASH_GET_NTH_BUCKET(aHash, sBucketNo);

    if( aHash->mLockMode == STL_STATICHASH_MODE_HASHLOCK )
    {
        stlAcquireSpinLock(aHash->mHashLock,
                           &sMissCount);
    }
    else if( aHash->mLockMode == STL_STATICHASH_MODE_BUCKETLOCK )
    {
        stlAcquireSpinLock(sBucket->mLock,
                           &sMissCount);
    }

    STL_RING_UNLINK( &sBucket->mList, aData );

    if( aHash->mLockMode == STL_STATICHASH_MODE_HASHLOCK )
    {
        stlReleaseSpinLock(aHash->mHashLock);
    }
    else if( aHash->mLockMode == STL_STATICHASH_MODE_BUCKETLOCK )
    {
        stlReleaseSpinLock(sBucket->mLock);
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stlFindStaticHash( stlStaticHash             *aHash,
                             stlStaticHashHashingFunc   aHashFunc,
                             stlStaticHashCompareFunc   aCompareFunc,
                             void                      *aKey,
                             void                     **aData,
                             stlErrorStack             *aErrorStack )
{
    stlUInt32             sBucketNo;
    stlStaticHashBucket  *sBucket;
    void *                sCurItem = NULL;
    stlUInt64             sMissCount;

    STL_PARAM_VALIDATE( aData != NULL, aErrorStack );

    STL_PARAM_VALIDATE( aHash != NULL, aErrorStack );

    sBucketNo = aHashFunc( aKey,
                           aHash->mBucketCount );
    sBucket = STL_STATICHASH_GET_NTH_BUCKET(aHash, sBucketNo);

    if( aHash->mLockMode == STL_STATICHASH_MODE_HASHLOCK )
    {
        stlAcquireSpinLock(aHash->mHashLock,
                           &sMissCount);
    }
    else if( aHash->mLockMode == STL_STATICHASH_MODE_BUCKETLOCK )
    {
        stlAcquireSpinLock(sBucket->mLock,
                           &sMissCount);
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

    if( aHash->mLockMode == STL_STATICHASH_MODE_HASHLOCK )
    {
        stlReleaseSpinLock(aHash->mHashLock);
    }
    else if( aHash->mLockMode == STL_STATICHASH_MODE_BUCKETLOCK )
    {
        stlReleaseSpinLock(sBucket->mLock);
    }

    *aData = sCurItem;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stlGetFirstStaticHashItem( stlStaticHash   *aHash,
                                     stlUInt32       *aFirstBucketSeq,
                                     void           **aData,
                                     stlErrorStack   *aErrorStack )
{
    stlStaticHashBucket *sBucket;
    stlUInt64            sMissCount;

    STL_PARAM_VALIDATE( aHash != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aData != NULL, aErrorStack );

    *aFirstBucketSeq = 0;
    sBucket = STL_STATICHASH_GET_NTH_BUCKET(aHash, *aFirstBucketSeq);
    while( *aFirstBucketSeq < aHash->mBucketCount )
    {
        if( aHash->mLockMode == STL_STATICHASH_MODE_HASHLOCK )
        {
            stlAcquireSpinLock(aHash->mHashLock,
                               &sMissCount);
        }
        else if( aHash->mLockMode == STL_STATICHASH_MODE_BUCKETLOCK )
        {
            stlAcquireSpinLock(sBucket->mLock,
                               &sMissCount);
        }

        *aData = STL_RING_GET_FIRST_DATA( &sBucket->mList );

        if( aHash->mLockMode == STL_STATICHASH_MODE_HASHLOCK )
        {
            stlReleaseSpinLock(aHash->mHashLock);
        }
        else if( aHash->mLockMode == STL_STATICHASH_MODE_BUCKETLOCK )
        {
            stlReleaseSpinLock(sBucket->mLock);
        }

        if( *aData != STL_RING_GET_DATA( &sBucket->mList, aHash->mLinkOffset ) )
        {
            break;
        }

        (*aFirstBucketSeq)++;
        *aData = NULL;
        sBucket = STL_STATICHASH_GET_NTH_BUCKET(aHash, *aFirstBucketSeq);
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus stlGetNextStaticHashItem( stlStaticHash   *aHash,
                                    stlUInt32       *aBucketSeq,
                                    void           **aData,
                                    stlErrorStack   *aErrorStack )
{
    stlStaticHashBucket *sBucket;
    void                *sNextData;
    stlUInt64            sMissCount;

    STL_PARAM_VALIDATE( aHash != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aData != NULL, aErrorStack );

    sBucket = STL_STATICHASH_GET_NTH_BUCKET(aHash, *aBucketSeq);

    while( *aBucketSeq < aHash->mBucketCount )
    {
        if( aHash->mLockMode == STL_STATICHASH_MODE_HASHLOCK )
        {
            stlAcquireSpinLock(aHash->mHashLock,
                               &sMissCount);
        }
        else if( aHash->mLockMode == STL_STATICHASH_MODE_BUCKETLOCK )
        {
            stlAcquireSpinLock(sBucket->mLock,
                               &sMissCount);
        }

        if( *aData == NULL )
        {
            sNextData = STL_RING_GET_FIRST_DATA( &sBucket->mList );
        }
        else
        {
            sNextData = STL_RING_GET_NEXT_DATA( &sBucket->mList, *aData );
        }

        if( aHash->mLockMode == STL_STATICHASH_MODE_HASHLOCK )
        {
            stlReleaseSpinLock(aHash->mHashLock);
        }
        else if( aHash->mLockMode == STL_STATICHASH_MODE_BUCKETLOCK )
        {
            stlReleaseSpinLock(sBucket->mLock);
        }

        if( sNextData != STL_RING_GET_DATA( &sBucket->mList, aHash->mLinkOffset ) )
        {
            *aData = sNextData;
            break;
        }

        (*aBucketSeq)++;
        *aData = NULL;
        sBucket = STL_STATICHASH_GET_NTH_BUCKET(aHash, *aBucketSeq);
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
