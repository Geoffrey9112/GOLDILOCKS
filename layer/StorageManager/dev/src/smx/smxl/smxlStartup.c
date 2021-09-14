/*******************************************************************************
 * smxlManger.c
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
#include <smrDef.h>
#include <smr.h>
#include <smf.h>
#include <smxl.h>
#include <smxlDef.h>

/**
 * @file smxlManager.c
 * @brief Storage Manager Layer Local Transaction Manager Internal Routines
 */

extern smrRedoVector  gSmxlRedoVectors[];
extern knlFxTableDesc gSmxlTransTableDesc;
extern knlFxTableDesc gSmxlUndoSegTableDesc;
extern smxlUndoFunc   gSmxlUndoFuncs[];

/**
 * @addtogroup smxl
 * @{
 */

smxlWarmupEntry   * gSmxlWarmupEntry;
stlInt64            gDataStoreMode;

stlInt32            gSmxlTransTableSize;
stlInt32            gSmxlUndoRelCount;
smxlTrans         * gSmxlTransTable;
smxlUndoRelEntry  * gSmxlUndoRelEntry;

/**
 * @brief Startup 단계에서 트랜잭션 관리자를 구성한다.
 * @param[out] aWarmupEntry Warmup Entry Point 구조체
 * @param[in,out] aEnv Environment 포인터
 */
stlStatus smxlStartupNoMount( void   ** aWarmupEntry,
                              smlEnv  * aEnv )
{
    smxlWarmupEntry  * sWarmupEntry;
    
    STL_PARAM_VALIDATE( aWarmupEntry != NULL, KNL_ERROR_STACK(aEnv) );

    /**
     * structure validation
     */
    STL_DASSERT( (STL_SIZEOF(smxlTrans) % 64) == 0 );
    STL_DASSERT( (STL_SIZEOF(smxlUndoRelEntry) % 64) == 0 );
    STL_DASSERT( STL_SIZEOF(smxlTransRecord) == SMR_TRANS_COMMENT_OFFSET );
    STL_DASSERT( STL_OFFSETOF(smxlTransRecord, mCommentSize) == SMR_TRANS_COMMENT_SIZE_OFFSET );
    

    /**
     * register recovery functions
     */
    smrRegisterRedoVectors( SMG_COMPONENT_TRANSACTION,
                            gSmxlRedoVectors );
    
    smxlRegisterUndoFuncs( SMG_COMPONENT_TRANSACTION, 
                           gSmxlUndoFuncs );

    STL_TRY( smxlRegisterFxTables( aEnv ) == STL_SUCCESS );

    STL_TRY( knlCacheAlignedAllocFixedStaticArea( STL_SIZEOF( smxlWarmupEntry ),
                                                  (void**)&sWarmupEntry,
                                                  (knlEnv*)aEnv ) == STL_SUCCESS );
 
    sWarmupEntry->mSystemScn          = SMXL_INIT_SYSTEM_SCN;
    sWarmupEntry->mAgerState          = SML_AGER_STATE_INACTIVE;
    sWarmupEntry->mAgableScn          = SMXL_INIT_SYSTEM_SCN;
    sWarmupEntry->mAgableViewScn      = SMXL_INIT_SYSTEM_SCN - 2;
    sWarmupEntry->mMinTransViewScn    = SMXL_INIT_SYSTEM_SCN;
    sWarmupEntry->mOldSystemScn       = SMXL_INIT_SYSTEM_SCN;
    sWarmupEntry->mOldMinViewScn      = SMXL_INIT_SYSTEM_SCN - 1;
    sWarmupEntry->mTryStealUndoCount  = 0;
    sWarmupEntry->mOldIdleSystemScn   = SMXL_INIT_SYSTEM_SCN;

    STL_TRY( knlInitLatch( &(sWarmupEntry->mLatch),
                           STL_TRUE,  /* aIsInShm */
                           KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_DATA_STORE_MODE,
                                      &sWarmupEntry->mDataStoreMode,
                                      KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    gDataStoreMode  = sWarmupEntry->mDataStoreMode;
    
    gSmxlWarmupEntry = sWarmupEntry;
    *aWarmupEntry    = (void*)sWarmupEntry;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Transaction Component를 MOUNT 모드로 전이한다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smxlStartupMount( smlEnv * aEnv )
{
    smxlTrans          * sTransTable;
    smxlWarmupEntry    * sWarmupEntry;
    stlInt64             sTransTableSize = 0;
    stlInt64             sUndoRelCount   = 0;
    stlInt64             sOldTransTableSize = 0;
    stlInt64             sOldUndoRelCount   = 0;
    stlInt64             sNewTransTableSize = 0;
    stlInt64             sNewUndoRelCount   = 0;
    stlUInt32            i;

    sWarmupEntry = gSmxlWarmupEntry;
    
    sOldTransTableSize = smfGetTransTableSize();
    sOldUndoRelCount = smfGetUndoRelCount();

    STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_TRANSACTION_TABLE_SIZE,
                                      &sNewTransTableSize,
                                      KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    if( sOldTransTableSize == 0 )
    {
        /**
         * CREATE DATABASE의 경우는 Control 파일이 존재하지 않기 때문에 0 일수 있다.
         */
        sOldTransTableSize = sNewTransTableSize;
    }
    
    STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_UNDO_RELATION_COUNT,
                                      &sNewUndoRelCount,
                                      KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    if( sOldUndoRelCount == 0 )
    {
        /**
         * CREATE DATABASE의 경우는 Control 파일이 존재하지 않기 때문에 0 일수 있다.
         */
        sOldUndoRelCount = sNewUndoRelCount;
    }
    
    sTransTableSize = STL_MAX( sOldTransTableSize, sNewTransTableSize );
    sUndoRelCount = STL_MAX( sOldUndoRelCount, sNewUndoRelCount );
    
    STL_TRY( knlCacheAlignedAllocFixedStaticArea( STL_SIZEOF( smxlTrans ) * sTransTableSize,
                                                  (void**)&sTransTable,
                                                  (knlEnv*)aEnv ) == STL_SUCCESS );

    STL_TRY( knlCacheAlignedAllocFixedStaticArea( STL_SIZEOF( smxlUndoRelEntry ) * sUndoRelCount,
                                                  (void**)&(sWarmupEntry->mUndoRelEntry),
                                                  (knlEnv*)aEnv ) == STL_SUCCESS );

    sWarmupEntry->mTransTable         = sTransTable;
    sWarmupEntry->mTransTableSize     = sOldTransTableSize;
    sWarmupEntry->mUndoRelCount       = sOldUndoRelCount;

    for( i = 0; i < sUndoRelCount; i++ )
    {
        sWarmupEntry->mUndoRelEntry[i].mIsFree = STL_TRUE;
        sWarmupEntry->mUndoRelEntry[i].mRelHandle = NULL;
    }

    STL_TRY( knlCreateDynamicMem( &sWarmupEntry->mParentMem,
                                  NULL, /* aParentMem */
                                  KNL_ALLOCATOR_STORAGE_MANAGER_PARENT_TRANSACTION,
                                  KNL_MEM_STORAGE_TYPE_SHM,
                                  SMXL_PARENT_SHMMEM_CHUNK_SIZE,
                                  SMXL_PARENT_SHMMEM_CHUNK_SIZE,
                                  NULL, /* aMemController */
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY( smxlAdjustGlobalVariables( aEnv ) == STL_SUCCESS );
    STL_TRY( smxlInitTrans( sTransTableSize,
                            STL_TRUE,
                            aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Transaction Component를 OPEN 모드로 전이한다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smxlStartupPreOpen( smlEnv * aEnv )
{
    STL_TRY( smxlResetTrans( SML_ENV(aEnv) ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Transaction Component를 OPEN 모드로 전이한다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smxlStartupOpen( smlEnv * aEnv )
{
    STL_TRY( smxlAdjustGlobalVariables( aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Warmup 단계에서 트랜잭션 관리자를 초기화한다.
 * @param[in] aWarmupEntry Warmup Entry Point 구조체
 * @param[in,out] aEnv Environment 포인터
 */
stlStatus smxlWarmup( void     * aWarmupEntry,
                      smlEnv   * aEnv )
{
    STL_PARAM_VALIDATE( aWarmupEntry != NULL, KNL_ERROR_STACK(aEnv) );
    
    smrRegisterRedoVectors( SMG_COMPONENT_TRANSACTION,
                            gSmxlRedoVectors );
    
    smxlRegisterUndoFuncs( SMG_COMPONENT_TRANSACTION, 
                           gSmxlUndoFuncs );

    STL_TRY( smxlRegisterFxTables( aEnv ) == STL_SUCCESS );
    
    gSmxlWarmupEntry = (smxlWarmupEntry*)aWarmupEntry;
    gDataStoreMode  = gSmxlWarmupEntry->mDataStoreMode;
    
    STL_TRY( smxlAdjustGlobalVariables( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 트랜잭션 관리자를 Cool-down 시킨다.
 * @param[in,out] aEnv Environment 포인터
 */
stlStatus smxlCooldown( smlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief 트랜잭션 관리자를 Boot-down 시킨다.
 * @param[in,out] aEnv Environment 포인터
 */
stlStatus smxlBootdown( smlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief 트랜잭션 관리자를 dismount 시킨다.
 * @param[in,out] aEnv Environment 포인터
 */
stlStatus smxlShutdownDismount( smlEnv * aEnv )
{
    STL_TRY( smxlFinTrans( aEnv ) == STL_SUCCESS );

    STL_TRY( knlDestroyDynamicMem( &gSmxlWarmupEntry->mParentMem,
                                   KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Data Store Mode를 구한다.
 * @return Data Store Mode
 */
stlInt64 smxlGetDataStoreMode()
{
    return gDataStoreMode;
}

/**
 * @brief Transaction Component를 Post Close 시킨다.
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smxlShutdownPostClose( smlEnv * aEnv )
{
    STL_TRY( smxlResetTrans( SML_ENV(aEnv) ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Transaction Component를 Close 시킨다.
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smxlShutdownClose( smlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief Fixed Table 들을 등록함
 * @param[in]  aEnv   Environment
 * @remarks
 * NO_MOUNT 단계와 Warm-Up 단계에서 동일한 순서로 등록되어야 함.
 */
stlStatus smxlRegisterFxTables( smlEnv * aEnv )
{
    STL_TRY( knlRegisterFxTable( &gSmxlTransTableDesc,
                                 (knlEnv*)aEnv )
             == STL_SUCCESS );
    
    STL_TRY( knlRegisterFxTable( &gSmxlUndoSegTableDesc,
                                 (knlEnv*)aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Transaction Component의 Global variable을 설정한다.
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smxlAdjustGlobalVariables( smlEnv * aEnv )
{
    gSmxlTransTableSize = gSmxlWarmupEntry->mTransTableSize;
    gSmxlUndoRelCount = gSmxlWarmupEntry->mUndoRelCount;
    gSmxlTransTable = gSmxlWarmupEntry->mTransTable;
    gSmxlUndoRelEntry = gSmxlWarmupEntry->mUndoRelEntry;

    return STL_SUCCESS;
}

/**
 * @brief MOUNT phase에서 Media Recovery를 위해 transaction table의 크기를 최대로 만들어둔다.
 * @param[out] aOrgTransTable      기존에 생성된 transaction table
 * @param[out] aOrgTransTableSize  기존에 생성된 transaction table의 크기
 * @param[in]  aNewTransTable      Media recovery를 위해 생성된 최대 크기의 transaction table
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smxlReplaceTransactionTable( smxlTrans  ** aOrgTransTable,
                                       stlInt32    * aOrgTransTableSize,
                                       smxlTrans   * aNewTransTable,
                                       smlEnv      * aEnv )
{
    *aOrgTransTableSize = gSmxlTransTableSize;
    *aOrgTransTable     = gSmxlTransTable;

    gSmxlWarmupEntry->mTransTableSize = SMXL_MAX_TRANS_TABLE_SIZE;
    gSmxlWarmupEntry->mTransTable     = aNewTransTable;
    gSmxlTransTableSize = gSmxlWarmupEntry->mTransTableSize;
    gSmxlTransTable     = gSmxlWarmupEntry->mTransTable;

    STL_TRY( smxlInitTrans( SMXL_MAX_TRANS_TABLE_SIZE,
                            STL_FALSE,
                            aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief MOUNT phase에서 Media Recovery가 완료된 후 기존의 transaction table로 되돌린다.
 * @param[in] aOrgTransTable       기존에 생성된 transaction table
 * @param[in] aOrgTransTableSize   기존에 생성된 transaction table의 크기
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smxlRestoreTransactionTable( smxlTrans  * aOrgTransTable,
                                       stlInt32     aOrgTransTableSize,
                                       smlEnv     * aEnv )
{
    gSmxlWarmupEntry->mTransTableSize = aOrgTransTableSize;
    gSmxlWarmupEntry->mTransTable     = aOrgTransTable;
    gSmxlTransTableSize = gSmxlWarmupEntry->mTransTableSize;
    gSmxlTransTable     = gSmxlWarmupEntry->mTransTable;

    return STL_SUCCESS;
}

/** @} */
