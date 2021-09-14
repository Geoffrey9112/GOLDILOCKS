/*******************************************************************************
 * ztcmStaticHash.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file ztcmStaticHash.c
 * @brief GlieseTool Cyclone Static Hash Routines
 */

#include <goldilocks.h>
#include <ztc.h>

stlStatus ztcmCreateStaticHash( ztcStaticHash ** aHash,
                                stlUInt16        aBucketCount,
                                stlUInt16        aLinkOffset,
                                stlUInt16        aKeyOffset,
                                stlAllocator   * aAllocator,
                                stlErrorStack  * aErrorStack )
{
    ztcStaticHashBucket * sBucket; 
    stlUInt32             sTotalSize;
    stlUInt32             sIdx;
    
    STL_PARAM_VALIDATE( aHash != NULL, aErrorStack );
    
    sTotalSize = STL_SIZEOF( ztcStaticHash ) + ( aBucketCount * STL_SIZEOF( ztcStaticHashBucket ) );
    
    STL_TRY( stlAllocRegionMem( aAllocator,
                                sTotalSize,
                                (void**)aHash,
                                aErrorStack ) == STL_SUCCESS);
    
    sBucket = (ztcStaticHashBucket*)( (*aHash) + 1 );
    
    /**
     * Value Initialize
     */
    (*aHash)->mBucketCount = aBucketCount;
    (*aHash)->mLinkOffset  = aLinkOffset;
    (*aHash)->mKeyOffset   = aKeyOffset;
    
    /**
     * Bucket Initialize
     */
    for( sIdx = 0; sIdx < aBucketCount; sIdx++ )
    {
        STL_RING_INIT_HEADLINK( &sBucket->mList, aLinkOffset );
        sBucket++;
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztcmInsertStaticHash( ztcStaticHash  * aHash, 
                                ztcHashingFunc   aHashFunc,
                                void           * aData,
                                stlErrorStack  * aErrorStack )
{
    stlUInt32             sBucketNo;
    ztcStaticHashBucket * sBucket;
    
    STL_PARAM_VALIDATE( aHash != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aData != NULL, aErrorStack );
    
    sBucketNo = aHashFunc( (stlChar*)aData + aHash->mKeyOffset,
                           aHash->mBucketCount );
    
    sBucket   = ZTC_STATICHASH_GET_NTH_BUCKET(aHash, sBucketNo);
    
    STL_RING_ADD_LAST( &sBucket->mList, aData );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztcmRemoveStaticHash( ztcStaticHash  * aHash, 
                                ztcHashingFunc   aHashFunc,
                                void           * aData,
                                stlErrorStack  * aErrorStack )
{
    stlUInt32             sBucketNo;
    ztcStaticHashBucket * sBucket;

    STL_PARAM_VALIDATE( aHash != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aData != NULL, aErrorStack );
    
    sBucketNo = aHashFunc( (stlChar*)aData + aHash->mKeyOffset,
                           aHash->mBucketCount );
    
    sBucket   = ZTC_STATICHASH_GET_NTH_BUCKET( aHash, sBucketNo );
    
    STL_RING_UNLINK( &sBucket->mList, aData );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztcmFindStaticHash( ztcStaticHash   * aHash, 
                              ztcHashingFunc    aHashFunc,
                              ztcCompareFunc    aCompareFunc,
                              void            * aKey,
                              void           ** aData,
                              stlErrorStack   * aErrorStack )
{
    stlUInt32             sBucketNo;
    ztcStaticHashBucket * sBucket;
    void *                sCurItem = NULL;
    
    STL_PARAM_VALIDATE( aHash != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aData != NULL, aErrorStack );
    
    sBucketNo = aHashFunc( aKey,
                           aHash->mBucketCount );
    
    sBucket   = ZTC_STATICHASH_GET_NTH_BUCKET(aHash, sBucketNo);
    
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
    
    *aData = sCurItem;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztcmGetFirstHashItem( ztcStaticHash  * aHash, 
                                stlUInt32      * aFirstBucketSeq,
                                void          ** aData,
                                stlErrorStack  * aErrorStack )
{
    ztcStaticHashBucket * sBucket;
    
    STL_PARAM_VALIDATE( aHash != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aData != NULL, aErrorStack );
    
    *aFirstBucketSeq = 0;
    
    sBucket = ZTC_STATICHASH_GET_NTH_BUCKET( aHash, *aFirstBucketSeq );
    
    while( *aFirstBucketSeq < aHash->mBucketCount )
    {
        *aData = STL_RING_GET_FIRST_DATA( &sBucket->mList );
        
        if( *aData != STL_RING_GET_DATA( &sBucket->mList, aHash->mLinkOffset ) )
        {
            break;
        }
        
        (*aFirstBucketSeq)++;
        
        *aData  = NULL;
        sBucket = ZTC_STATICHASH_GET_NTH_BUCKET( aHash, *aFirstBucketSeq );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztcmGetNextHashItem( ztcStaticHash  * aHash, 
                               stlUInt32      * aBucketSeq,
                               void          ** aData,
                               stlErrorStack  * aErrorStack )
{
    ztcStaticHashBucket * sBucket;
    void                * sNextData;
    
    STL_PARAM_VALIDATE( aHash != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aData != NULL, aErrorStack );
    
    sBucket = ZTC_STATICHASH_GET_NTH_BUCKET( aHash, *aBucketSeq );
    
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
        
        *aData  = NULL;
        sBucket = ZTC_STATICHASH_GET_NTH_BUCKET( aHash, *aBucketSeq );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;    
}


/** @} */
