/*******************************************************************************
 * ztphStaticHash.c
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
 * @file ztphStaticHash.c
 * @brief Kernel Layer Static Hash implementation routines
 */

#include <stl.h>
#include <goldilocks.h>
#include <ztphStaticHash.h>

#define ZTPH_ROT(x,k) (((x)<<(k)) | ((x)>>(32-(k))))

/**
 * Hash Value 계산 from postgres
 */
stlUInt32  ztphGetHashValueString( stlChar * aName )
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
    k -= ZTPH_ROT(j,14);

    i ^= k;
    i -= ZTPH_ROT(k,11);

    j ^= i;
    j -= ZTPH_ROT(i,25);

    k ^= j;
    k -= ZTPH_ROT(j,16);

    i ^= k;
    i -= ZTPH_ROT(k, 4);

    j ^= i;
    j -= ZTPH_ROT(i,14);

    k ^= j;
    k -= ZTPH_ROT(j,24);

    return k;
}

stlStatus ztphCreateStaticHash(stlAllocator    *aAllocator,
                               stlErrorStack   *aErrorStack,
                               ztphStaticHash **aHash,
                               stlUInt16        aBucketCount,
                               stlUInt16        aLinkOffset,
                               stlUInt16        aKeyOffset)
{
    ztphStaticHashBucket *sBucket;
    stlUInt32             i;
    stlUInt32             sTotalSize;

    STL_PARAM_VALIDATE( aHash != NULL, aErrorStack );

    sTotalSize = STL_SIZEOF(ztphStaticHash) + (aBucketCount * STL_SIZEOF(ztphStaticHashBucket));

    STL_PARAM_VALIDATE( aAllocator != NULL, aErrorStack);

    STL_TRY(stlAllocRegionMem(aAllocator,
                              sTotalSize,
                              (void **)aHash,
                              aErrorStack)
            == STL_SUCCESS);

    sBucket = (ztphStaticHashBucket*)((*aHash) + 1);

    /* initialize Hash */
    (*aHash)->mBucketCount = aBucketCount;
    (*aHash)->mLinkOffset = aLinkOffset;
    (*aHash)->mKeyOffset = aKeyOffset;
    (*aHash)->mHintNode = NULL;

    for( i = 0; i < aBucketCount; i++ )
    {
        STL_RING_INIT_HEADLINK(&sBucket->mList, aLinkOffset);
        sBucket++;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztphInsertStaticHash(stlErrorStack             *aErrorStack,
                               ztphStaticHash            *aHash,
                               ztphStaticHashHashingFunc  aHashFunc,
                               void                      *aData)
{
    stlUInt32             sBucketNo;
    ztphStaticHashBucket *sBucket;

    STL_PARAM_VALIDATE( aHash != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aData != NULL, aErrorStack );

    sBucketNo = aHashFunc( (stlChar*)aData + aHash->mKeyOffset,
                           aHash->mBucketCount );
    sBucket = ZTPH_STATICHASH_GET_NTH_BUCKET(aHash, sBucketNo);

    STL_RING_ADD_LAST( &sBucket->mList, aData );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztphRemoveStaticHash(stlErrorStack             *aErrorStack,
                               ztphStaticHash            *aHash,
                               ztphStaticHashHashingFunc  aHashFunc,
                               void                      *aData)
{
    stlUInt32             sBucketNo;
    ztphStaticHashBucket *sBucket;

    STL_PARAM_VALIDATE( aHash != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aData != NULL, aErrorStack );

    sBucketNo = aHashFunc( (stlChar*)aData + aHash->mKeyOffset,
                           aHash->mBucketCount );
    sBucket = ZTPH_STATICHASH_GET_NTH_BUCKET(aHash, sBucketNo);

    if(aData == aHash->mHintNode)
    {
        aHash->mHintNode = NULL;
    }

    STL_RING_UNLINK( &sBucket->mList, aData );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztphFindStaticHash(stlErrorStack              *aErrorStack,
                             ztphStaticHash             *aHash,
                             ztphStaticHashHashingFunc   aHashFunc,
                             ztphStaticHashCompareFunc   aCompareFunc,
                             void                       *aKey,
                             void                      **aData)
{
    stlUInt32             sBucketNo;
    ztphStaticHashBucket *sBucket;
    void *                sCurItem = NULL;

    STL_PARAM_VALIDATE( aHash != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aData != NULL, aErrorStack );

    sBucketNo = aHashFunc( aKey,
                           aHash->mBucketCount );
    sBucket = ZTPH_STATICHASH_GET_NTH_BUCKET(aHash, sBucketNo);

    if(aHash->mHintNode != NULL)
    {
        sCurItem = aHash->mHintNode;
        if( aCompareFunc(aKey, (stlChar*)sCurItem + aHash->mKeyOffset) == 0 )
        {
            /* Match Hint */
            STL_THROW(EXIT_FUNC);
        }
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
    else
    {
        aHash->mHintNode = sCurItem;
    }

    STL_RAMP(EXIT_FUNC);

    *aData = sCurItem;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztphGetFirstItem(stlErrorStack   *aErrorStack,
                           ztphStaticHash  *aHash,
                           stlUInt32       *aFirstBucketSeq,
                           void           **aData)
{
    ztphStaticHashBucket *sBucket;

    STL_PARAM_VALIDATE( aHash != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aData != NULL, aErrorStack );

    *aFirstBucketSeq = 0;
    sBucket = ZTPH_STATICHASH_GET_NTH_BUCKET(aHash, *aFirstBucketSeq);
    while( *aFirstBucketSeq < aHash->mBucketCount )
    {
        *aData = STL_RING_GET_FIRST_DATA( &sBucket->mList );

        if( *aData != STL_RING_GET_DATA( &sBucket->mList, aHash->mLinkOffset ) )
        {
            break;
        }

        (*aFirstBucketSeq)++;
        *aData = NULL;
        sBucket = ZTPH_STATICHASH_GET_NTH_BUCKET(aHash, *aFirstBucketSeq);
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztphGetNextItem(stlErrorStack   *aErrorStack,
                          ztphStaticHash  *aHash,
                          stlUInt32       *aBucketSeq,
                          void           **aData)
{
    ztphStaticHashBucket *sBucket;
    void                 *sNextData;

    STL_PARAM_VALIDATE( aHash != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aData != NULL, aErrorStack );

    sBucket = ZTPH_STATICHASH_GET_NTH_BUCKET(aHash, *aBucketSeq);

    while( *aBucketSeq < aHash->mBucketCount )
    {
        if( *aData == NULL )
        {
            sNextData = STL_RING_GET_FIRST_DATA( &sBucket->mList );
        }
        else
        {
            sNextData = STL_RING_GET_NEXT_DATA( &sBucket->mList, *aData );
        }

        if( sNextData != STL_RING_GET_DATA( &sBucket->mList, aHash->mLinkOffset ) )
        {
            *aData = sNextData;
            break;
        }

        (*aBucketSeq)++;
        *aData = NULL;
        sBucket = ZTPH_STATICHASH_GET_NTH_BUCKET(aHash, *aBucketSeq);
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
