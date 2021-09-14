/*******************************************************************************
 * smklManger.c
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
#include <smf.h>
#include <smkl.h>
#include <smklDef.h>
#include <smklManager.h>
#include <smklWaitGraph.h>
#include <smklHashTable.h>

/**
 * @file smklStartup.c
 * @brief Storage Manager Layer Local Lock Manager Startup Internal Routines
 */

extern knlFxTableDesc gLockWaitTableDesc;

/**
 * @addtogroup smkl
 * @{
 */

smklWarmupEntry  * gSmklWarmupEntry;
knlLatch         * gSmklDeadlockLatch;

smklLockSlot   * gSmklLockTable;
smklHashBucket * gSmklHashTable;
stlChar        * gSmklWaitTable;
stlInt32         gSmklLockTableSize;
stlInt32         gSmklLockWaitTableSize;
stlInt32         gSmklHashTableSize;

/**
 * @brief Startup 단계에서 잠금 관리자를 구성한다.
 * @param[out] aWarmupEntry Warmup Entry Point 구조체
 * @param[in,out] aEnv Environment 포인터
 */
stlStatus smklStartupNoMount( void   ** aWarmupEntry,
                              smlEnv  * aEnv )
{
    smklWarmupEntry * sWarmupEntry;

    /**
     * structure validation
     */
    STL_DASSERT( (STL_SIZEOF(smklItemCore) % 64) == 0 );
    STL_DASSERT( (STL_SIZEOF(smklItemRelation) % 64) == 0 );
    STL_DASSERT( (STL_SIZEOF(smklHashBucket) % 64) == 0 );
    STL_DASSERT( (STL_SIZEOF(smklLockSlot) % 64) == 0 );
    
    STL_TRY( smklRegisterFxTables( aEnv ) == STL_SUCCESS );
    
    STL_TRY( knlCacheAlignedAllocFixedStaticArea( STL_SIZEOF( smklWarmupEntry ),
                                                  (void**)&sWarmupEntry,
                                                  (knlEnv*)aEnv ) == STL_SUCCESS );

    stlMemset( sWarmupEntry, 0x00, STL_SIZEOF( smklWarmupEntry ) );
    
    *aWarmupEntry = sWarmupEntry;
    gSmklWarmupEntry = sWarmupEntry;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Lock Component를 MOUNT 모드로 전이한다.
 * @param[in,out] aEnv Environment 포인터
 */
stlStatus smklStartupMount( smlEnv  * aEnv )
{
    stlChar         * sLockWaitTable;
    smklHashBucket  * sLockHashTable;
    smklLockSlot    * sLockTable;
    smklWarmupEntry * sWarmupEntry;
    stlInt64          sOldTransTableSize = 0;
    stlInt64          sNewTransTableSize = 0;
    stlInt64          sTransTableSize = 0;
    stlInt64          sHashTableSize = 0;
    stlInt64          sSharedRequestQueueCount = 0;

    sWarmupEntry = gSmklWarmupEntry;
    
    sOldTransTableSize = smfGetTransTableSize();
    
    STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_TRANSACTION_TABLE_SIZE,
                                      &sNewTransTableSize,
                                      KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    
    STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_SHARED_REQUEST_QUEUE_COUNT,
                                      &sSharedRequestQueueCount,
                                      KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    
    STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_LOCK_HASH_TABLE_SIZE,
                                      &sHashTableSize,
                                      KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    
    if( sOldTransTableSize == 0 )
    {
        /**
         * CREATE DATABASE의 경우는 Control 파일이 존재하지 않기 때문에 0 일수 있다.
         */
        sOldTransTableSize = sNewTransTableSize;
    }
    
    sTransTableSize = STL_MAX( sOldTransTableSize, sNewTransTableSize );
    
    STL_TRY( knlAllocFixedStaticArea( STL_SIZEOF( smklLockSlot ) * sTransTableSize,
                                      (void**)&sLockTable,
                                      (knlEnv*)aEnv ) == STL_SUCCESS );

    STL_TRY( knlAllocFixedStaticArea( (STL_SIZEOF( stlChar ) *
                                       (sTransTableSize + sSharedRequestQueueCount) *
                                       (sTransTableSize + sSharedRequestQueueCount)),
                                      (void**)&sLockWaitTable,
                                      (knlEnv*)aEnv ) == STL_SUCCESS );

    STL_TRY( knlCacheAlignedAllocFixedStaticArea( STL_SIZEOF( smklHashBucket ) * sHashTableSize,
                                                  (void**)&sLockHashTable,
                                                  (knlEnv*)aEnv ) == STL_SUCCESS );

    STL_TRY( knlInitLatch( &sWarmupEntry->mDeadlockLatch,
                           STL_TRUE,
                           (knlEnv*)aEnv ) == STL_SUCCESS );
    
    sWarmupEntry->mLockTable = sLockTable;
    sWarmupEntry->mLockTableSize = sOldTransTableSize;
    sWarmupEntry->mLockWaitTableSize = (sOldTransTableSize + sSharedRequestQueueCount);
    sWarmupEntry->mLockWaitTable = sLockWaitTable;
    sWarmupEntry->mLockHashTable = sLockHashTable;
    sWarmupEntry->mLockHashTableSize = sHashTableSize;
        
    STL_TRY( knlCreateDynamicMem( &sWarmupEntry->mShmMemForLock,
                                  NULL, /* aParentMem */
                                  KNL_ALLOCATOR_STORAGE_MANAGER_PARENT_LOCK_TABLE,
                                  KNL_MEM_STORAGE_TYPE_SHM,
                                  SMKL_PARENT_SHMMEM_CHUNK_SIZE,
                                  SMKL_PARENT_SHMMEM_CHUNK_SIZE,
                                  NULL, /* aMemController */
                                  (knlEnv*)aEnv )
             == STL_SUCCESS );
    
    STL_TRY( smklAdjustGlobalVariables( aEnv ) == STL_SUCCESS );
    STL_TRY( smklInitLockSlots( sTransTableSize, aEnv ) == STL_SUCCESS );
    STL_TRY( smklInitWaitTable( sTransTableSize ) == STL_SUCCESS );
    STL_TRY( smklInitHashTable( aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Warmup 단계에서 잠금 관리자를 초기화한다.
 * @param[in] aWarmupEntry Warmup Entry Point 구조체
 * @param[in,out] aEnv Environment 포인터
 */
stlStatus smklWarmup( void     * aWarmupEntry,
                      smlEnv   * aEnv )
{
    STL_PARAM_VALIDATE( aWarmupEntry != NULL, KNL_ERROR_STACK(aEnv) );

    STL_TRY( smklRegisterFxTables( aEnv ) == STL_SUCCESS );
    
    gSmklWarmupEntry = (smklWarmupEntry*)aWarmupEntry;

    STL_TRY( smklAdjustGlobalVariables( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 잠금 관리자를 Cool-down 시킨다.
 * @param[in,out] aEnv Environment 포인터
 */
stlStatus smklCooldown( smlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief 잠금 관리자를 Boot-down 시킨다.
 * @param[in,out] aEnv Environment 포인터
 */
stlStatus smklBootdown( smlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief 잠금 관리자를 dismount 시킨다.
 * @param[in,out] aEnv Environment 포인터
 */
stlStatus smklShutdownDismount( smlEnv * aEnv )
{
    STL_TRY( smklDestHashTable( aEnv ) == STL_SUCCESS );
    
    STL_TRY( knlDestroyDynamicMem( &gSmklWarmupEntry->mShmMemForLock,
                                   KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 잠금 관리자를 Close 시킨다.
 * @param[in,out] aEnv Environment 포인터
 */
stlStatus smklShutdownClose( smlEnv * aEnv )
{
    return STL_SUCCESS;
}


/**
 * @brief Fixed Table 들을 등록함
 * @param[in]  aEnv   Environment
 * @remarks
 * NO_MOUNT 단계와 Warm-Up 단계에서 동일한 순서로 등록되어야 함.
 */
stlStatus smklRegisterFxTables( smlEnv * aEnv )
{
    STL_TRY( knlRegisterFxTable( &gLockWaitTableDesc,
                                 (knlEnv*)aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Lock Component의 Global variable을 설정한다.
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smklAdjustGlobalVariables( smlEnv * aEnv )
{
    gSmklLockTable = gSmklWarmupEntry->mLockTable;
    gSmklHashTable = gSmklWarmupEntry->mLockHashTable;
    gSmklWaitTable = gSmklWarmupEntry->mLockWaitTable;
    gSmklLockTableSize = gSmklWarmupEntry->mLockTableSize;
    gSmklLockWaitTableSize = gSmklWarmupEntry->mLockWaitTableSize;
    gSmklHashTableSize = gSmklWarmupEntry->mLockHashTableSize;
    gSmklDeadlockLatch = &(gSmklWarmupEntry->mDeadlockLatch);

    return STL_SUCCESS;
}

/** @} */
