/*******************************************************************************
 * elgStartup.c
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
 * @file elgStartup.c
 * @brief Execution Library Layer Startup Internal Routines
 */


#include <ell.h>
#include <eldt.h>
#include <eldc.h>

#include <eldDictionary.h>
#include <elgFixedTable.h>
#include <elgPendOp.h>
#include <elgStartup.h>
#include <elsSessionObjectMgr.h>

extern knlEventTable gElgEventTable[];
stlFatalHandler   gElgOldFatalHandler;


/**
 * @addtogroup elgStartup
 * @{
 */

/**
 * @brief Execution Library Layer를 No Mount상태로 전환시킨다.
 * @param[in] aStartupInfo Startup 시 정보 (Recovery type(restart or media), Tablespace Id)
 * @param [in,out] aEnv Environment 구조체
 * @remarks
 */
stlStatus elgStartupNoMount( knlStartupInfo * aStartupInfo,
                             void           * aEnv )
{
    void ** sEntryPoint = NULL;
    void  * sShareEntry = NULL;

    stlInt32      sState = 0;
    
    /***************************************************************
     * Database Shared 영역을 위한 초기화  
     ***************************************************************/

    /**********************************
     * Shared Memory 영역 연결 
     **********************************/
    
    /**
     * Execution Library 의 Shared Entry 초기화
     */
    
    STL_TRY( knlGetEntryPoint( STL_LAYER_EXECUTION_LIBRARY,
                               (void **) & sEntryPoint,
                               KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlAllocFixedStaticArea( STL_SIZEOF( ellSharedEntry ),
                                      & sShareEntry,
                                      KNL_ENV(aEnv) )
             == STL_SUCCESS );

    *sEntryPoint = sShareEntry;

    
    /**
     * Global 변수에 Shared Entry 연결
     */

    STL_TRY( knlGetEntryPoint( STL_LAYER_EXECUTION_LIBRARY,
                               (void **) & sEntryPoint,
                               KNL_ENV(aEnv) )
             == STL_SUCCESS );

    gEllSharedEntry = *sEntryPoint;
    stlMemset( gEllSharedEntry, 0x00, STL_SIZEOF(ellSharedEntry) );

    /**********************************
     * Cache Entry 초기화 
     **********************************/

    gEllSharedEntry->mCurrCacheEntry = NULL;
    gEllCacheBuildEntry = & gEllSharedEntry->mPhaseCacheEntry[KNL_STARTUP_PHASE_NO_MOUNT];
    
    /**********************************
     * 메모리 관리자 초기화 
     **********************************/
    
    /**
     * Dictionary Cache 를 위한 Memory 관리자 초기화
     */

    STL_TRY( knlCreateDynamicMem( & gEllCacheBuildEntry->mMemDictCache,
                                  NULL, /* aParentMem */
                                  KNL_ALLOCATOR_EXECUTION_LIBRARY_NOMOUNT_DICT_CACHE,
                                  KNL_MEM_STORAGE_TYPE_SHM,
                                  ELL_DICT_CACHE_NOMOUNT_MEM_INIT_SIZE,
                                  ELL_DICT_CACHE_NOMOUNT_MEM_NEXT_SIZE,
                                  NULL, /* aMemController */
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 1;

    /**
     * Dictionary Cache 의 Hash Element 를 위한 메모리 관리자 초기화
     */

    STL_TRY( knlCreateDynamicMem( & gEllCacheBuildEntry->mMemDictHashElement,
                                  NULL, /* aParentMem */
                                  KNL_ALLOCATOR_EXECUTION_LIBRARY_NOMOUNT_HASH_ELEMENT,
                                  KNL_MEM_STORAGE_TYPE_SHM,
                                  ELL_DICT_CACHE_NOMOUNT_HASH_ELEMENT_MEM_INIT_SIZE,
                                  ELL_DICT_CACHE_NOMOUNT_HASH_ELEMENT_MEM_NEXT_SIZE,
                                  NULL, /* aMemController */
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 2;

    /**
     * Dictionary Cache 의 Hash Related Object List 를 위한 메모리 관리자 초기화
     */

    STL_TRY( knlCreateDynamicMem( & gEllCacheBuildEntry->mMemDictHashRelated,
                                  NULL, /* aParentMem */
                                  KNL_ALLOCATOR_EXECUTION_LIBRARY_NOMOUNT_HASH_RELATED_OBJECT,
                                  KNL_MEM_STORAGE_TYPE_SHM,
                                  ELL_DICT_CACHE_NOMOUNT_HASH_RELATED_MEM_INIT_SIZE,
                                  ELL_DICT_CACHE_NOMOUNT_HASH_RELATED_MEM_NEXT_SIZE,
                                  NULL, /* aMemController */
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 3;

    /**
     * Pending Operation을 위한 메모리 관리자를 초기화
     * - Fatal 상황시 In-Memory Rollback 을 위해 Shared Memory 상에서 관리해야 한다.
     * - DDL 시에만 발생하는 메모리 관리자로 성능에 영향을 주는 요소가 아니다.
     */

    STL_TRY( knlCreateDynamicMem( & gEllCacheBuildEntry->mMemPendOP,
                                  NULL, /* aParentMem */
                                  KNL_ALLOCATOR_EXECUTION_LIBRARY_NOMOUNT_PENDING_OPERATION,
                                  KNL_MEM_STORAGE_TYPE_SHM,
                                  ELL_PEND_OP_NOMOUNT_MEM_INIT_SIZE,
                                  ELL_PEND_OP_NOMOUNT_MEM_NEXT_SIZE,
                                  NULL, /* aMemController */
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 4;

    /**********************************
     * NO_MOUNT 단계 초기화 
     **********************************/
        
    /**
     * Dictionary Table 의 physical Handle 공간 확보 
     */

    STL_TRY( knlAllocDynamicMem( & gEllCacheBuildEntry->mMemDictCache,
                                 STL_SIZEOF(void *) * ELDT_TABLE_ID_MAX,
                                 (void **) & gEllSharedEntry->mDictPhysicalHandle,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( gEllSharedEntry->mDictPhysicalHandle,
               0x00,
               STL_SIZEOF(void *) * ELDT_TABLE_ID_MAX );

    STL_TRY( knlAllocDynamicMem( & gEllCacheBuildEntry->mMemDictCache,
                                 STL_SIZEOF(void *) * ELDT_TABLE_ID_MAX,
                                 (void **) & gEllSharedEntry->mDictIdentityColumnHandle,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( gEllSharedEntry->mDictIdentityColumnHandle,
               0x00,
               STL_SIZEOF(void *) * ELDT_TABLE_ID_MAX );
    
    /**
     * Dictionary Table 의 physical Handle 연결 
     */
               
    gEldTablePhysicalHandle  = gEllSharedEntry->mDictPhysicalHandle;
    gEldIdentityColumnHandle = gEllSharedEntry->mDictIdentityColumnHandle;
    
    /**
     * NO-MOUNT 단계를 위한 SQL-Object Cache 용 Hash 관리자 할당
     */

    STL_TRY( eldAllocCacheMem( (void **) & gEllCacheBuildEntry->mObjectCache,
                               STL_SIZEOF(eldcDictHash *) * ELDC_OBJECTCACHE_MAX,
                               ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * NO-MOUNT 단계를 위한 Privilege Cache 용 Hash 관리자 할당
     */
    
    STL_TRY( eldAllocCacheMem( (void **) & gEllCacheBuildEntry->mPrivCache,
                               STL_SIZEOF(eldcDictHash *) * ELDC_PRIVCACHE_MAX,
                               ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * NO-MOUNT 단계를 위한 Object Cache 구축
     */

    STL_TRY( eldcNoMountBuildObjectCache( ELL_ENV(aEnv) ) == STL_SUCCESS );

    /**
     * NO-MOUNT 단계를 위한 Privilege Cache 구축
     */

    STL_TRY( eldcNonServiceBuildPrivCache( ELL_ENV(aEnv) ) == STL_SUCCESS );
    
    /**
     * Pending Operation Reset
     */

    STL_TRY( elgResetSessionPendOp4NotOpenPhase( ELL_ENV(aEnv) ) == STL_SUCCESS );
    
    /***************************************************************
     * Process Shared 영역을 위한 초기화  
     ***************************************************************/

    /**
     * Execution Library layer 의 Fixed Table(Dump Table) 들을 등록
     */
    
    STL_TRY( elgRegisterFixedTables( ELL_ENV(aEnv) ) == STL_SUCCESS );
    
    /**
     * Event Table 등록
     */
    
    knlRegisterEventTable( STL_LAYER_EXECUTION_LIBRARY, gElgEventTable );

    /**
     * Pending Operation 등록
     */
    
    elgRegisterPendOperations();

    /* gEllCacheBuildEntry = NULL; */
    
    return STL_SUCCESS;
    
    STL_FINISH;

    switch (sState)
    {
        case 4:
            (void) knlDestroyDynamicMem( & gEllCacheBuildEntry->mMemPendOP,
                                         KNL_ENV(aEnv) );
        case 3:
            (void) knlDestroyDynamicMem( & gEllCacheBuildEntry->mMemDictHashRelated,
                                         KNL_ENV(aEnv) );
        case 2:
            (void) knlDestroyDynamicMem( & gEllCacheBuildEntry->mMemDictHashElement,
                                         KNL_ENV(aEnv) );
        case 1:
            (void) knlDestroyDynamicMem( & gEllCacheBuildEntry->mMemDictCache,
                                         KNL_ENV(aEnv) );
        default:
            break;
    }

    /**
     * INIT 단계로 복귀
     */

    gEldTablePhysicalHandle  = NULL;
    gEldIdentityColumnHandle = NULL;
    
    gEllSharedEntry = NULL;
    gEllCacheBuildEntry = NULL;
    
    return STL_FAILURE;
}

/**
 * @brief Execution Library Layer를 Mount 상태로 전환시킨다.
 * @param[in] aStartupInfo Startup 시 정보 (Recovery type(restart or media), Tablespace Id)
 * @param[in,out] aEnv Environment 구조체
 * @remarks
 */
stlStatus elgStartupMount( knlStartupInfo * aStartupInfo,
                           void           * aEnv )
{
    stlInt32 sState = 0;

    /***************************************************************
     * Database Shared 영역을 위한 초기화  
     ***************************************************************/

    /**********************************
     * Cache Entry 초기화 
     **********************************/

    /* create db 시에는 설정되지 않음 */
    /* STL_DASSERT( gEllSharedEntry->mCurrCacheEntry */
    /*              == & gEllSharedEntry->mPhaseCacheEntry[KNL_STARTUP_PHASE_NO_MOUNT] ); */
    
    gEllCacheBuildEntry = & gEllSharedEntry->mPhaseCacheEntry[KNL_STARTUP_PHASE_MOUNT];
    
    /**********************************
     * 메모리 관리자 초기화 
     **********************************/
    
    /**
     * Dictionary Cache 를 위한 Memory 관리자 초기화
     */

    STL_TRY( knlCreateDynamicMem( & gEllCacheBuildEntry->mMemDictCache,
                                  NULL, /* aParentMem */
                                  KNL_ALLOCATOR_EXECUTION_LIBRARY_MOUNT_DICT_CACHE,
                                  KNL_MEM_STORAGE_TYPE_SHM,
                                  ELL_DICT_CACHE_MOUNT_MEM_INIT_SIZE,
                                  ELL_DICT_CACHE_MOUNT_MEM_NEXT_SIZE,
                                  NULL, /* aMemController */
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 1;

    /**
     * Dictionary Cache 의 Hash Element 를 위한 메모리 관리자 초기화
     */

    STL_TRY( knlCreateDynamicMem( & gEllCacheBuildEntry->mMemDictHashElement,
                                  NULL, /* aParentMem */
                                  KNL_ALLOCATOR_EXECUTION_LIBRARY_MOUNT_HASH_ELEMENT,
                                  KNL_MEM_STORAGE_TYPE_SHM,
                                  ELL_DICT_CACHE_MOUNT_HASH_ELEMENT_MEM_INIT_SIZE,
                                  ELL_DICT_CACHE_MOUNT_HASH_ELEMENT_MEM_NEXT_SIZE,
                                  NULL, /* aMemController */
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 2;

    /**
     * Dictionary Cache 의 Hash Related Object List 를 위한 메모리 관리자 초기화
     */

    STL_TRY( knlCreateDynamicMem( & gEllCacheBuildEntry->mMemDictHashRelated,
                                  NULL, /* aParentMem */
                                  KNL_ALLOCATOR_EXECUTION_LIBRARY_MOUNT_HASH_RELATED_OBJECT,
                                  KNL_MEM_STORAGE_TYPE_SHM,
                                  ELL_DICT_CACHE_MOUNT_HASH_RELATED_MEM_INIT_SIZE,
                                  ELL_DICT_CACHE_MOUNT_HASH_RELATED_MEM_NEXT_SIZE,
                                  NULL, /* aMemController */
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 3;

    /**
     * Pending Operation을 위한 메모리 관리자를 초기화
     */

    STL_TRY( knlCreateDynamicMem( & gEllCacheBuildEntry->mMemPendOP,
                                  NULL, /* aParentMem */
                                  KNL_ALLOCATOR_EXECUTION_LIBRARY_MOUNT_PENDING_OPERATION,
                                  KNL_MEM_STORAGE_TYPE_SHM,
                                  ELL_PEND_OP_MOUNT_MEM_INIT_SIZE,
                                  ELL_PEND_OP_MOUNT_MEM_NEXT_SIZE,
                                  NULL, /* aMemController */
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 4;

    /**********************************
     * MOUNT 단계 초기화 
     **********************************/
    
    /**
     * MOUNT 단계를 위한 SQL-Object Cache 용 Hash 관리자 할당
     */

    STL_TRY( eldAllocCacheMem( (void **) & gEllCacheBuildEntry->mObjectCache,
                               STL_SIZEOF(eldcDictHash *) * ELDC_OBJECTCACHE_MAX,
                               ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * MOUNT 단계를 위한 Privilege Cache 용 Hash 관리자 할당
     */

    STL_TRY( eldAllocCacheMem( (void **) & gEllCacheBuildEntry->mPrivCache,
                               STL_SIZEOF(eldcDictHash *) * ELDC_PRIVCACHE_MAX,
                               ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * MOUNT 단계를 위한 Object Cache 구축
     */

    STL_TRY( eldcMountBuildObjectCache( ELL_ENV(aEnv) ) == STL_SUCCESS );

    /**
     * MOUNT 단계를 위한 Privilege Cache 구축
     */

    STL_TRY( eldcNonServiceBuildPrivCache( ELL_ENV(aEnv) ) == STL_SUCCESS );
    
    /**
     * Pending Operation Reset
     */

    STL_TRY( elgResetSessionPendOp4NotOpenPhase( ELL_ENV(aEnv) ) == STL_SUCCESS );

    /***************************************************************
     * Process Shared 영역을 위한 초기화  
     ***************************************************************/

    /* gEllCacheBuildEntry = NULL; */
    
    return STL_SUCCESS;

    STL_FINISH;

    switch (sState)
    {
        case 4:
            (void) knlDestroyDynamicMem( & gEllCacheBuildEntry->mMemPendOP,
                                         KNL_ENV(aEnv) );
        case 3:
            (void) knlDestroyDynamicMem( & gEllCacheBuildEntry->mMemDictHashRelated,
                                         KNL_ENV(aEnv) );
        case 2:
            (void) knlDestroyDynamicMem( & gEllCacheBuildEntry->mMemDictHashElement,
                                         KNL_ENV(aEnv) );
        case 1:
            (void) knlDestroyDynamicMem( & gEllCacheBuildEntry->mMemDictCache,
                                         KNL_ENV(aEnv) );
        default:
            break;
    }

    gEllCacheBuildEntry = NULL;
    
    return STL_FAILURE;
}


/**
 * @brief Execution Library Layer를 PREOPEN 상태로 전환시킨다.
 * @param[in] aStartupInfo Startup 시 정보 (Recovery type(restart or media), Tablespace Id)
 * @param[in,out] aEnv Environment 구조체
 * @remarks
 */
stlStatus elgStartupPreOpen( knlStartupInfo * aStartupInfo,
                             void           * aEnv )
{
    smlServerState sServerState = 0;

    stlInt32 sState = 0;

    /***************************************************************
     * Database Shared 영역을 위한 초기화  
     ***************************************************************/

    /**********************************
     * Cache Entry 초기화 
     **********************************/

    /* create db 시에는 설정되지 않음 */
    /* STL_DASSERT( gEllSharedEntry->mCurrCacheEntry */
    /*              == & gEllSharedEntry->mPhaseCacheEntry[KNL_STARTUP_PHASE_MOUNT] ); */

    /* PRE-OPEN 단계이나 OPEN 단계 영역에 Cache 를 구축한다. */
    gEllCacheBuildEntry = & gEllSharedEntry->mPhaseCacheEntry[KNL_STARTUP_PHASE_OPEN];
    
    /**********************************
     * 메모리 관리자 초기화 
     **********************************/
    
    /**
     * Dictionary Cache 를 위한 Memory 관리자 초기화
     */

    STL_TRY( knlCreateDynamicMem( & gEllCacheBuildEntry->mMemDictCache,
                                  NULL, /* aParentMem */
                                  KNL_ALLOCATOR_EXECUTION_LIBRARY_DICT_CACHE,
                                  KNL_MEM_STORAGE_TYPE_SHM,
                                  ELL_DICT_CACHE_OPEN_MEM_INIT_SIZE,
                                  ELL_DICT_CACHE_OPEN_MEM_NEXT_SIZE,
                                  NULL, /* aMemController */
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 1;

    /**
     * Dictionary Cache 의 Hash Element 를 위한 메모리 관리자 초기화
     */

    STL_TRY( knlCreateDynamicMem( & gEllCacheBuildEntry->mMemDictHashElement,
                                  NULL, /* aParentMem */
                                  KNL_ALLOCATOR_EXECUTION_LIBRARY_HASH_ELEMENT,
                                  KNL_MEM_STORAGE_TYPE_SHM,
                                  ELL_DICT_CACHE_OPEN_HASH_ELEMENT_MEM_INIT_SIZE,
                                  ELL_DICT_CACHE_OPEN_HASH_ELEMENT_MEM_NEXT_SIZE,
                                  NULL, /* aMemController */
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 2;

    /**
     * Dictionary Cache 의 Hash Related Object List 를 위한 메모리 관리자 초기화
     */

    STL_TRY( knlCreateDynamicMem( & gEllCacheBuildEntry->mMemDictHashRelated,
                                  NULL, /* aParentMem */
                                  KNL_ALLOCATOR_EXECUTION_LIBRARY_HASH_RELATED_OBJECT,
                                  KNL_MEM_STORAGE_TYPE_SHM,
                                  ELL_DICT_CACHE_OPEN_HASH_RELATED_MEM_INIT_SIZE,
                                  ELL_DICT_CACHE_OPEN_HASH_RELATED_MEM_NEXT_SIZE,
                                  NULL, /* aMemController */
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 3;

    /**
     * Dictionary Cache 의 Hash Related Object List 를 위한 메모리 관리자 초기화
     */

    STL_TRY( knlCreateDynamicMem( & gEllCacheBuildEntry->mMemDictRelatedTrans,
                                  NULL, /* aParentMem */
                                  KNL_ALLOCATOR_EXECUTION_LIBRARY_DICT_RELATED_TRANSACTION,
                                  KNL_MEM_STORAGE_TYPE_SHM,
                                  ELL_DICT_CACHE_OPEN_HASH_RELATED_TRANS_MEM_INIT_SIZE,
                                  ELL_DICT_CACHE_OPEN_HASH_RELATED_TRANS_MEM_NEXT_SIZE,
                                  NULL, /* aMemController */
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 4;

    /**
     * Pending Operation을 위한 메모리 관리자를 초기화
     */

    STL_TRY( knlCreateDynamicMem( & gEllCacheBuildEntry->mMemPendOP,
                                  NULL, /* aParentMem */
                                  KNL_ALLOCATOR_EXECUTION_LIBRARY_PENDING_OPERATION,
                                  KNL_MEM_STORAGE_TYPE_SHM,
                                  ELL_PEND_OP_OPEN_MEM_INIT_SIZE,
                                  ELL_PEND_OP_OPEN_MEM_NEXT_SIZE,
                                  NULL, /* aMemController */
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 5;

    /**********************************
     * OPEN 단계 초기화 
     **********************************/
    
    /**
     * SQL-Object Cache 용 Hash 관리자 할당
     */

    STL_TRY( eldAllocCacheMem( (void **) & gEllCacheBuildEntry->mObjectCache,
                               STL_SIZEOF(eldcDictHash *) * ELDC_OBJECTCACHE_MAX,
                               ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Privilege Cache 용 Hash 관리자 할당
     */

    STL_TRY( eldAllocCacheMem( (void **) & gEllCacheBuildEntry->mPrivCache,
                               STL_SIZEOF(eldcDictHash *) * ELDC_PRIVCACHE_MAX,
                               ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Object Cache 와 Privilege Cache 구축
     */
    
    STL_TRY( smlGetServerState( & sServerState,
                                SML_ENV( aEnv ) )
             == STL_SUCCESS );

    switch ( sServerState )
    {
        case SML_SERVER_STATE_NONE:
            {
                /*
                 * 초기 상태 
                 * Dictionary 관련 정보가 구축되지 않은 상태로 관련 정보를 설정할 수 없다.
                 * - ex) CREATE DB 과정
                 */

                break;
            }
        case SML_SERVER_STATE_RECOVERED:  /* Recovery가 완료된 상태 */
        case SML_SERVER_STATE_SERVICE:    /* 서비스가 가능한 상태 */
        case SML_SERVER_STATE_SHUTDOWN:   /* 서버가 정상 종료된 상태 */
        default:
            {
                /**
                 * Dictionary Handle을 설정
                 */
                
                STL_TRY( eldSetDictionaryHandle( ELL_ENV(aEnv) ) == STL_SUCCESS );
                
                /**
                 * 서비스가 가능한 상태로 Dictionary Cache 를 구축 
                 */
        
                STL_TRY( eldBuildDictionaryCache( ELL_ENV(aEnv) ) == STL_SUCCESS );

            }
    }

    /***************************************************************
     * Process Shared 영역을 위한 초기화  
     ***************************************************************/

    /* gEllCacheBuildEntry = NULL; */
    
    return STL_SUCCESS;

    STL_FINISH;

    switch (sState)
    {
        case 5:
            (void) knlDestroyDynamicMem( & gEllCacheBuildEntry->mMemPendOP,
                                         KNL_ENV(aEnv) );
        case 4:
            (void) knlDestroyDynamicMem( & gEllCacheBuildEntry->mMemDictRelatedTrans,
                                         KNL_ENV(aEnv) );
        case 3:
            (void) knlDestroyDynamicMem( & gEllCacheBuildEntry->mMemDictHashRelated,
                                         KNL_ENV(aEnv) );
        case 2:
            (void) knlDestroyDynamicMem( & gEllCacheBuildEntry->mMemDictHashElement,
                                         KNL_ENV(aEnv) );
        case 1:
            (void) knlDestroyDynamicMem( & gEllCacheBuildEntry->mMemDictCache,
                                         KNL_ENV(aEnv) );
        default:
            break;
    }

    gEllCacheBuildEntry = NULL;
    
    return STL_FAILURE;
}

/**
 * @brief Execution Library Layer를 Warm-up 단계로 전환시킨다.
 * @param[in,out] aEnv Environment 구조체
 * @remarks
 */
stlStatus elgWarmup( void * aEnv )
{
    void  ** sEntryPoint = NULL;

    /***************************************************************
     * Database Shared 영역을 위한 초기화  
     ***************************************************************/
    
    /**
     * Global 변수에 Shared Entry 연결
     */
    
    STL_TRY( knlGetEntryPoint( STL_LAYER_EXECUTION_LIBRARY,
                               (void **) & sEntryPoint,
                               KNL_ENV(aEnv) )
             == STL_SUCCESS );

    gEllSharedEntry = *sEntryPoint;

    /**
     * Dictionary Table 의 physical Handle 연결
     */

    gEldTablePhysicalHandle  = gEllSharedEntry->mDictPhysicalHandle;
    gEldIdentityColumnHandle = gEllSharedEntry->mDictIdentityColumnHandle;

    /**
     * Dictionary Dump Handle 설정
     */

    STL_TRY( eldSetDictionaryDumpHandle( ELL_ENV(aEnv) ) == STL_SUCCESS );
    
    /***************************************************************
     * Process Shared 영역을 위한 초기화  
     ***************************************************************/
    
    /**
     * Execution Library layer 의 Fixed Table(Dump Table) 들을 등록
     */
    
    STL_TRY( elgRegisterFixedTables( ELL_ENV(aEnv) ) == STL_SUCCESS );

    /**
     * Event Table 등록
     */
    
    knlRegisterEventTable( STL_LAYER_EXECUTION_LIBRARY, gElgEventTable );

    /**
     * Pending Operation 등록
     */
    
    elgRegisterPendOperations();

    return STL_SUCCESS;
    
    STL_FINISH;

    knlLogMsgUnsafe( NULL,
                     KNL_ENV( aEnv ),
                     KNL_LOG_LEVEL_FATAL,
                     "[EXECUTION LIBRARY WARMUP FAIL] %s\n",
                     stlGetLastErrorMessage( KNL_ERROR_STACK( aEnv ) ) );
    
    return STL_FAILURE;
}

/**
 * @brief Execution Library Layer를 Cool-down 시킨다.
 * @param[in,out] aEnv Environment 구조체
 * @remarks
 */
stlStatus elgCooldown( void * aEnv )
{
    /***************************************************************
     * Process Shared 영역을 위한 종료 
     ***************************************************************/

    /**
     * Session Object 를 정리한다.
     */
    
    STL_TRY( elsFiniSessObjectMgr( ELL_ENV(aEnv) ) == STL_SUCCESS );
    
    gEllSharedEntry = NULL;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Execution Library Layer를 INIT 시킨다.
 * @param[in] aShutdownInfo Shutdown 시 정보 (shutdown mode, shutdown phase)
 * @param[in,out] aEnv Environment 구조체
 * @remarks
 */
stlStatus elgShutdownInit( knlShutdownInfo * aShutdownInfo,
                           void            * aEnv )
{
    stlInt32 sState = 0;
    ellCacheEntry * sLastCacheEntry = NULL;

    STL_TRY_THROW( aShutdownInfo->mShutdownMode > KNL_SHUTDOWN_MODE_ABORT,
                   RAMP_SHUTDOWN_ABORT );

    /***************************************************************
     * Database Shared 영역을 위한 종료 
     ***************************************************************/

    /**********************************
     * Cache Entry 초기화 
     **********************************/

    sLastCacheEntry = gEllSharedEntry->mCurrCacheEntry;
    gEllSharedEntry->mCurrCacheEntry = NULL;

    /**********************************
     * NOMOUNT 메모리 관리자 종료 
     **********************************/

    if ( sLastCacheEntry == & gEllSharedEntry->mPhaseCacheEntry[KNL_STARTUP_PHASE_NO_MOUNT] )
    {
        /**
         * Dictionary Cache 를 위한 메모리 관리자를 종료
         */

        sState = 1;
        STL_TRY( knlDestroyDynamicMem(
                     & gEllSharedEntry->mPhaseCacheEntry[KNL_STARTUP_PHASE_NO_MOUNT].mMemDictCache,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /**
         * Dictionary Cache 의 Hash Element 를 위한 메모리 관리자 종료 
         */

        sState = 2;

        STL_TRY( knlDestroyDynamicMem(
                     & gEllSharedEntry->mPhaseCacheEntry[KNL_STARTUP_PHASE_NO_MOUNT].mMemDictHashElement,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /**
         * Dictionary Cache 의 Hash Related Object 를 위한 메모리 관리자 종료 
         */
    
        sState = 3;
        STL_TRY( knlDestroyDynamicMem(
                     & gEllSharedEntry->mPhaseCacheEntry[KNL_STARTUP_PHASE_NO_MOUNT].mMemDictHashRelated,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /**
         * Pending Operation을 위한 메모리 관리자를 종료
         */

        sState = 4;
        STL_TRY( knlDestroyDynamicMem(
                     & gEllSharedEntry->mPhaseCacheEntry[KNL_STARTUP_PHASE_NO_MOUNT].mMemPendOP,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        /* nothing to do */
    }

    sState = 5;

    /**********************************
     * INIT 단계로 설정 
     **********************************/
    
    /**
     * Cache 연결
     */
    
    gEllSharedEntry = NULL;

    /**
     * Handle 설정 
     */

    gEldTablePhysicalHandle  = NULL;
    gEldIdentityColumnHandle = NULL;
    
    /***************************************************************
     * Process Shared 영역을 위한 종료 
     ***************************************************************/

    /**
     * Session Object 를 정리한다.
     */
    
    STL_TRY( elsFiniSessObjectMgr( ELL_ENV(aEnv) ) == STL_SUCCESS );

    /**
     * Fixed Table Map 메모리 관리자를 종료
     */

    STL_RAMP( RAMP_SHUTDOWN_ABORT );

    return STL_SUCCESS;

    STL_FINISH;

    switch (sState)
    {
        case 0:
            (void) knlDestroyDynamicMem(
                & gEllSharedEntry->mPhaseCacheEntry[KNL_STARTUP_PHASE_NO_MOUNT].mMemDictCache,
                KNL_ENV(aEnv) );
        case 1:
            (void) knlDestroyDynamicMem(
                & gEllSharedEntry->mPhaseCacheEntry[KNL_STARTUP_PHASE_NO_MOUNT].mMemDictHashElement,
                KNL_ENV(aEnv) );
        case 2:
            (void) knlDestroyDynamicMem(
                & gEllSharedEntry->mPhaseCacheEntry[KNL_STARTUP_PHASE_NO_MOUNT].mMemDictHashRelated,
                KNL_ENV(aEnv) );
        case 3:
                (void) knlDestroyDynamicMem(
                    & gEllSharedEntry->mPhaseCacheEntry[KNL_STARTUP_PHASE_NO_MOUNT].mMemPendOP,
                    KNL_ENV(aEnv) );
        case 4:
        default:
            break;
    }

    /**********************************
     * INIT 단계로 설정 
     **********************************/
    
    /**
     * Cache 연결
     */
    
    gEllSharedEntry = NULL;
    
    /**
     * Handle 설정 
     */

    gEldTablePhysicalHandle  = NULL;
    gEldIdentityColumnHandle = NULL;
    
    return STL_FAILURE;
}


/**
 * @brief Execution Library Layer를 DISMOUNT 시킨다.
 * @param[in] aShutdownInfo Shutdown 시 정보 (shutdown mode, shutdown phase)
 * @param[in,out] aEnv Environment 구조체
 * @remarks
 */
stlStatus elgShutdownDismount( knlShutdownInfo * aShutdownInfo,
                               void            * aEnv )
{
    stlInt32 sState = 0;
    ellCacheEntry * sLastCacheEntry = NULL;

    STL_TRY_THROW( aShutdownInfo->mShutdownMode > KNL_SHUTDOWN_MODE_ABORT,
                   RAMP_SHUTDOWN_ABORT );

    /***************************************************************
     * Database Shared 영역을 위한 종료 
     ***************************************************************/

    /**********************************
     * Cache Entry 초기화 
     **********************************/
    
    sLastCacheEntry = gEllSharedEntry->mCurrCacheEntry;
    gEllSharedEntry->mCurrCacheEntry = & gEllSharedEntry->mPhaseCacheEntry[KNL_STARTUP_PHASE_NO_MOUNT];

    /**********************************
     * MOUNT 메모리 관리자 종료 
     **********************************/

    if ( sLastCacheEntry == & gEllSharedEntry->mPhaseCacheEntry[KNL_STARTUP_PHASE_MOUNT] )
    {   
        /**
         * Dictionary Cache 를 위한 메모리 관리자를 종료
         */

        sState = 1;
        STL_TRY( knlDestroyDynamicMem(
                     & gEllSharedEntry->mPhaseCacheEntry[KNL_STARTUP_PHASE_MOUNT].mMemDictCache,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /**
         * Dictionary Cache 의 Hash Element 를 위한 메모리 관리자 종료 
         */

        sState = 2;
        STL_TRY( knlDestroyDynamicMem(
                     & gEllSharedEntry->mPhaseCacheEntry[KNL_STARTUP_PHASE_MOUNT].mMemDictHashElement,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /**
         * Dictionary Cache 의 Hash Related Object 를 위한 메모리 관리자 종료 
         */
    
        sState = 3;
        STL_TRY( knlDestroyDynamicMem(
                     & gEllSharedEntry->mPhaseCacheEntry[KNL_STARTUP_PHASE_MOUNT].mMemDictHashRelated,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /**
         * Pending Operation을 위한 메모리 관리자를 종료
         */

        sState = 4;
        STL_TRY( knlDestroyDynamicMem(
                     & gEllSharedEntry->mPhaseCacheEntry[KNL_STARTUP_PHASE_MOUNT].mMemPendOP,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        /* nothing to do */
    }

    sState = 5;
    
    /***************************************************************
     * Process Shared 영역을 위한 종료 
     ***************************************************************/

    STL_RAMP( RAMP_SHUTDOWN_ABORT );

    return STL_SUCCESS;

    STL_FINISH;

    switch (sState)
    {
        case 0:
            (void) knlDestroyDynamicMem(
                & gEllSharedEntry->mPhaseCacheEntry[KNL_STARTUP_PHASE_MOUNT].mMemDictCache,
                KNL_ENV(aEnv) );
        case 1:
            (void) knlDestroyDynamicMem(
                & gEllSharedEntry->mPhaseCacheEntry[KNL_STARTUP_PHASE_MOUNT].mMemDictHashElement,
                KNL_ENV(aEnv) );
        case 2:
            (void) knlDestroyDynamicMem(
                & gEllSharedEntry->mPhaseCacheEntry[KNL_STARTUP_PHASE_MOUNT].mMemDictHashRelated,
                KNL_ENV(aEnv) );
        case 3:
            (void) knlDestroyDynamicMem(
                & gEllSharedEntry->mPhaseCacheEntry[KNL_STARTUP_PHASE_MOUNT].mMemPendOP,
                KNL_ENV(aEnv) );
        default:
            break;
    }

    return STL_FAILURE;
}


/**
 * @brief Execution Library Layer를 POST_CLOSE 시킨다.
 * @param[in] aShutdownInfo Shutdown 시 정보 (shutdown mode, shutdown phase)
 * @param[in,out] aEnv Environment 구조체
 * @remarks
 */
stlStatus elgShutdownPostClose( knlShutdownInfo * aShutdownInfo,
                                void            * aEnv )
{
    stlInt32 sState = 0;
    ellCacheEntry * sLastCacheEntry = NULL;

    STL_TRY_THROW( aShutdownInfo->mShutdownMode > KNL_SHUTDOWN_MODE_ABORT,
                   RAMP_SHUTDOWN_ABORT );

    /***************************************************************
     * Database Shared 영역을 위한 종료 
     ***************************************************************/

    /**********************************
     * Cache Entry 초기화 
     **********************************/
    
    sLastCacheEntry = gEllSharedEntry->mCurrCacheEntry;
    gEllSharedEntry->mCurrCacheEntry = & gEllSharedEntry->mPhaseCacheEntry[KNL_STARTUP_PHASE_MOUNT];

    /**********************************
     * OPEN 메모리 관리자 종료 
     **********************************/

    if ( sLastCacheEntry == & gEllSharedEntry->mPhaseCacheEntry[KNL_STARTUP_PHASE_OPEN] )
    {   
    
        /**
         * Dictionary Cache 를 위한 메모리 관리자를 종료
         */

        sState = 1;
        STL_TRY( knlDestroyDynamicMem(
                     & gEllSharedEntry->mPhaseCacheEntry[KNL_STARTUP_PHASE_OPEN].mMemDictCache,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /**
         * Dictionary Cache 의 Hash Element 를 위한 메모리 관리자 종료 
         */

        sState = 2;
        STL_TRY( knlDestroyDynamicMem(
                     & gEllSharedEntry->mPhaseCacheEntry[KNL_STARTUP_PHASE_OPEN].mMemDictHashElement,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /**
         * Dictionary Cache 의 Hash Related Object 를 위한 메모리 관리자 종료 
         */
    
        sState = 3;
        STL_TRY( knlDestroyDynamicMem(
                     & gEllSharedEntry->mPhaseCacheEntry[KNL_STARTUP_PHASE_OPEN].mMemDictHashRelated,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /**
         * Pending Operation을 위한 메모리 관리자를 종료
         */

        sState = 4;
        STL_TRY( knlDestroyDynamicMem(
                     & gEllSharedEntry->mPhaseCacheEntry[KNL_STARTUP_PHASE_OPEN].mMemPendOP,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        /* nothing to do */
    }
    
    sState = 5;

    STL_RAMP( RAMP_SHUTDOWN_ABORT );

    return STL_SUCCESS;

    STL_FINISH;

    switch (sState)
    {
        case 0:
            (void) knlDestroyDynamicMem(
                & gEllSharedEntry->mPhaseCacheEntry[KNL_STARTUP_PHASE_OPEN].mMemDictCache,
                KNL_ENV(aEnv) );
        case 1:
            (void) knlDestroyDynamicMem(
                & gEllSharedEntry->mPhaseCacheEntry[KNL_STARTUP_PHASE_OPEN].mMemDictHashElement,
                KNL_ENV(aEnv) );
        case 2:
            (void) knlDestroyDynamicMem(
                & gEllSharedEntry->mPhaseCacheEntry[KNL_STARTUP_PHASE_OPEN].mMemDictHashRelated,
                KNL_ENV(aEnv) );
        case 3:
            (void) knlDestroyDynamicMem(
                & gEllSharedEntry->mPhaseCacheEntry[KNL_STARTUP_PHASE_OPEN].mMemPendOP,
                KNL_ENV(aEnv) );
        default:
            break;
    }
    
    return STL_FAILURE;
}


/**
 * @brief Execution Library Layer의 Fatal Handler.
 * @param[in] aCauseString Fatal 유발 원인을 알수 있는 스트링
 * @param[in] aSigInfo Signal Information
 * @param[in] aContext 이전 Context 정보
 */
void elgFatalHandler( const stlChar * aCauseString,
                      void          * aSigInfo,
                      void          * aContext )
{
    ellEnv        * sMyEnv = NULL;
    ellSessionEnv * sSessEnv = NULL;

    sMyEnv = (ellEnv*) knlGetMyEnv();

    STL_TRY_THROW( sMyEnv != NULL, RAMP_FINISH );
    STL_TRY_THROW( knlGetTopLayer(KNL_ENV(sMyEnv))
                   >= STL_LAYER_EXECUTION_LIBRARY,
                   RAMP_FINISH );

    if( KNL_GET_FATAL_HANDLING( sMyEnv, STL_LAYER_EXECUTION_LIBRARY ) == STL_FALSE )
    {
        /**
         * 무한 Fatal을 방지해야 한다.
         */
        
        KNL_SET_FATAL_HANDLING( sMyEnv, STL_LAYER_EXECUTION_LIBRARY );
    
        sSessEnv = ELL_SESS_ENV( sMyEnv );
        
        STL_TRY_THROW( sSessEnv != NULL, RAMP_SKIP_FATAL );
        
        /**
         * Fatal 당시의 정보를 남긴다.
         */
        
        (void) knlLogMsgUnsafe( NULL,
                                (knlEnv*) sMyEnv,
                                KNL_LOG_LEVEL_FATAL,
                                "[EXECUTION LIBRARY INFORMATION]\n"
                                "---- AUTHORIZATION_ID(%ld)\n",
                                sSessEnv->mLoginAuthID );
    }

    STL_RAMP( RAMP_FINISH );
    
    /**
     * Storage Layer의 Fatal Handler를 호출한다.
     */
    
    gElgOldFatalHandler( aCauseString, aSigInfo, aContext );

    STL_RAMP( RAMP_SKIP_FATAL );
}

/** @} elgStartup */
