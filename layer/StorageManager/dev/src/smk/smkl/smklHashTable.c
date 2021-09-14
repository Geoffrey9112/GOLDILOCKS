/*******************************************************************************
 * smklHashTable.c
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

#include <stl.h>
#include <dtl.h>
#include <knl.h>
#include <scl.h>
#include <smlDef.h>
#include <smDef.h>
#include <smxl.h>
#include <sme.h>
#include <smg.h>
#include <smklDef.h>
#include <smklHashTable.h>

/**
 * @file smklHashTable.c
 * @brief Storage Manager Layer Lock Node Internal Routines
 */

/**
 * @addtogroup smklHashTable
 * @{
 */

inline stlStatus smklInitHashTable( smlEnv * aEnv )
{
    stlInt32 i;
    
    for( i = 0; i < SMKL_LOCK_HASH_BUCKET_COUNT; i++ )
    {
        STL_RING_INIT_HEADLINK( &SMKL_LOCK_HASH_TABLE(i).mItemList,
                                STL_OFFSETOF( smklItemRecord, mItemLink ) );
        
        STL_TRY( knlCreateDynamicMem( &SMKL_LOCK_HASH_TABLE(i).mShmMem,
                                      &gSmklWarmupEntry->mShmMemForLock,
                                      KNL_ALLOCATOR_STORAGE_MANAGER_LOCK_TABLE,
                                      KNL_MEM_STORAGE_TYPE_SHM,
                                      SMKL_SHMMEM_CHUNK_SIZE,
                                      SMKL_SHMMEM_CHUNK_SIZE,
                                      NULL, /* aMemController */
                                      (knlEnv*)aEnv )
                 == STL_SUCCESS );
        
        STL_TRY( knlInitLatch( &SMKL_LOCK_HASH_TABLE(i).mLatch,
                               STL_TRUE,
                               (knlEnv*)aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

inline stlStatus smklDestHashTable( smlEnv * aEnv )
{
    stlInt32 i;
    
    for( i = 0; i < SMKL_LOCK_HASH_BUCKET_COUNT; i++ )
    {
        STL_TRY( knlDestroyDynamicMem( &SMKL_LOCK_HASH_TABLE(i).mShmMem,
                                       (knlEnv*)aEnv )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

inline stlUInt32 smklGetHashValue( smlRid aRid )
{
    return (((stlInt64)aRid.mTbsId * 100000000) +
            ((stlInt64)aRid.mPageId * 1000000) +
            (aRid.mOffset)) % SMKL_LOCK_HASH_BUCKET_COUNT;
}

inline smklHashBucket * smklGetHashBucket( smlRid aRid )
{
    stlUInt32 sHashValue;

    sHashValue = smklGetHashValue( aRid );
    return ( &SMKL_LOCK_HASH_TABLE(sHashValue) );
}

inline smklItemRecord * smklFindHashItem( smklHashBucket * aHashBucket,
                                          smlRid           aRid )
{
    smklItemRecord * sLockItemIter = NULL;
    smklItemRecord * sLockItem = NULL;
    
    STL_RING_FOREACH_ENTRY( &aHashBucket->mItemList, sLockItemIter )
    {
        if( SML_IS_EQUAL_RID( sLockItemIter->mRid, aRid ) == STL_TRUE )
        {
            sLockItem = sLockItemIter;
            break;
        }
    }
    
    return sLockItem;
}

inline void smklAddHashItem( smklHashBucket * aHashBucket,
                             smklItemRecord * aLockItem )
{
    STL_RING_ADD_LAST( &aHashBucket->mItemList, aLockItem );
}

inline stlStatus smklRemoveHashItem( smklItemRecord * aLockItem,
                                     smlEnv         * aEnv )
{
    smklHashBucket * sHashBucket;

    sHashBucket = smklGetHashBucket( aLockItem->mRid );
    
    STL_RING_UNLINK( &sHashBucket->mItemList, aLockItem );

    STL_TRY( knlFreeDynamicMem( &sHashBucket->mShmMem,
                                aLockItem,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} */
