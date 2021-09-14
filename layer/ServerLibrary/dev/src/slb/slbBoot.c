/*******************************************************************************
 * slbBoot.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: slbBoot.c 13675 2014-10-06 14:17:27Z lym999 $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file slbBoot.c
 * @brief Server Library Startup Routines
 */

#include <stl.h>
#include <cml.h>
#include <knl.h>
#include <scl.h>
#include <sml.h>
#include <ell.h>
#include <qll.h>
#include <ssl.h>
#include <sll.h>
#include <slDef.h>
#include <slb.h>

extern knlFxTableDesc gSlbSystemInfoTableDesc;
extern knlFxTableDesc gSlbDispatcherTableDesc;
extern knlFxTableDesc gSlbSharedServerTableDesc;
extern knlFxTableDesc gSlbBalancerTableDesc;
extern knlFxTableDesc gSlbQueueTableDesc;

stlBool             gSlbWarmedUp = STL_FALSE;
sllSharedMemory   * gSlSharedMemory = NULL;
stlFatalHandler     gSlbOldFatalHandler;

/**
 * @brief gserver를 Warmup 한다.
 * @param[in,out] aEnv Environment 포인터
 */
stlStatus slbWarmUp( void  * aEnv )
{
    sllSharedMemory ** sWarmupEntry;
    
    STL_TRY( knlGetEntryPoint( STL_LAYER_SERVER_LIBRARY,
                               (void **) & sWarmupEntry,
                               KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Fixed Table 등록
     */

    STL_TRY( slbRegisterFxTables( aEnv ) == STL_SUCCESS );
    
    gSlSharedMemory = *sWarmupEntry;

    /**
     * 최상위 layer 에서 등록된 모든 Fixed Table 의 Heap Descriptor 를 Heap 영역에 구성한다.
     */
    
    STL_TRY( knlBuildHeapFixedTableDescMap( KNL_ENV(aEnv) ) == STL_SUCCESS );

    gSlbWarmedUp = STL_TRUE;
    
    return STL_SUCCESS;

    STL_FINISH;

    knlLogMsgUnsafe( NULL,
                     KNL_ENV( aEnv ),
                     KNL_LOG_LEVEL_FATAL,
                     "[SERVER LIBRARY WARMUP FAIL] %s\n",
                     stlGetLastErrorMessage( KNL_ERROR_STACK( aEnv ) ) );
    
    return STL_FAILURE;
}

/**
 * @brief gserver를 CoolDown 한다.
 * @param[in,out] aEnv Environment 포인터
 */
stlStatus slbCoolDown( void * aEnv )
{
    gSlSharedMemory = NULL;
    gSlbWarmedUp = STL_FALSE;
    
    return STL_SUCCESS;
}

/**
 * @brief Server Library Layer를 No-Mount 상태로 전환시킨다.
 * @param[in] aStartupInfo Startup 시 정보 (Recovery type(restart or media), Tablespace Id)
 * @param[in,out] aEnv Environment 구조체
 * @remarks
 */
stlStatus slbStartupNoMount( knlStartupInfo * aStartupInfo,
                             void           * aEnv )
{
    stlInt64          sMaxSessionCount = 0;
    stlInt64          sMaxProcessEnvCount = 0;
    void           ** sWarmupEntry;
    stlInt64          sStaticAreaSize = 0;
    stlErrorStack     sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    STL_ASSERT( aEnv != NULL );

    /**
     * Environment Pool 구성
     */
    STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_CLIENT_MAX_COUNT,
                                      &sMaxSessionCount,
                                      KNL_ENV( aEnv ) ) == STL_SUCCESS );

    STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_PROCESS_MAX_COUNT,
                                      &sMaxProcessEnvCount,
                                      KNL_ENV( aEnv ) ) == STL_SUCCESS );

    STL_TRY( knlCreateEnvPool( (stlUInt32)sMaxProcessEnvCount,
                               STL_SIZEOF( sllEnv ),
                               KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    STL_TRY( knlCreateSessionEnvPool( (stlUInt32)sMaxSessionCount,
                                      STL_SIZEOF( sllSessionEnv ),
                                      KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    /**
     * Warmup Entry 구성
     */
    
    STL_TRY( knlGetEntryPoint( STL_LAYER_SERVER_LIBRARY,
                               (void **) &sWarmupEntry,
                               KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_DISPATCHER_CM_BUFFER_SIZE,
                                      &sStaticAreaSize,
                                      KNL_ENV(aEnv) ) == STL_SUCCESS );

    STL_TRY( knlAllocFixedStaticArea( sStaticAreaSize,
                                      (void**)&gSlSharedMemory,
                                      KNL_ENV(aEnv) ) == STL_SUCCESS );

    *sWarmupEntry = gSlSharedMemory;

    stlMemset( gSlSharedMemory, 0x00, sStaticAreaSize );

    gSlSharedMemory->mTotalSize = sStaticAreaSize;

    /**
     * Fixed Table 등록
     */

    STL_TRY( slbRegisterFxTables( aEnv ) == STL_SUCCESS );
    
    /**
     * 최상위 layer 에서 등록된 모든 Fixed Table 의 Heap Descriptor 를 Heap 영역에 구성한다.
     */
    
    STL_TRY( knlBuildHeapFixedTableDescMap( KNL_ENV(aEnv) ) == STL_SUCCESS );

    /**
     * 모든 Fixed Table 의 Relation Header 를 Shared 영역에 구성한다.
     */
    
    STL_TRY( knlBuildSharedFixedRelationHeader( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    /**
     * NO MOUNT 용 Dictionary Cache 영역에 Fixed Table Cache 를 구축함
     * - Fixed Table 을 등록한 최상위 layer 에서 호출되어야 함.
     */
    
    STL_TRY( ellBuildFixedCacheAndCompleteCache( KNL_STARTUP_PHASE_NO_MOUNT, ELL_ENV(aEnv) ) == STL_SUCCESS );

    /**
     * NOMOUNT 단계용 Performance View 구축
     */

    STL_TRY( qllBuildPerfViewNoMount( QLL_ENV(aEnv) ) == STL_SUCCESS );
             
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Server Library Layer를 Mount 상태로 전환시킨다.
 * @param[in] aStartupInfo Startup 시 정보 (Recovery type(restart or media), Tablespace Id)
 * @param[in,out] aEnv Environment 구조체
 * @remarks
 */
stlStatus slbStartupMount( knlStartupInfo * aStartupInfo,
                           void           * aEnv )
{
    /**
     * MOUNT 용 Dictionary Cache 영역에 Fixed Table Cache 를 구축함
     * - Fixed Table 을 등록한 최상위 layer 에서 호출되어야 함.
     */
    
    STL_TRY( ellBuildFixedCacheAndCompleteCache( KNL_STARTUP_PHASE_MOUNT, ELL_ENV(aEnv) ) == STL_SUCCESS );

    /**
     * MOUNT 단계용 Performance View 구축
     */

    STL_TRY( qllBuildPerfViewMount( QLL_ENV(aEnv) ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Server Library Layer를 Open 상태로 전환시킨다.
 * @param[in] aStartupInfo Startup 시 정보 (Recovery type(restart or media), Tablespace Id)
 * @param[in,out] aEnv Environment 구조체
 * @remarks
 */
stlStatus slbStartupOpen( knlStartupInfo * aStartupInfo,
                          void           * aEnv )
{
    STL_ASSERT( aEnv != NULL );
    
    /**
     * OPEN 용 Dictionary Cache 영역에 Fixed Table Cache 를 구축함
     * - Fixed Table 을 등록한 최상위 layer 에서 호출되어야 함.
     */
    
    STL_TRY( ellBuildFixedCacheAndCompleteCache( KNL_STARTUP_PHASE_OPEN, ELL_ENV(aEnv) ) == STL_SUCCESS );
    
    STL_TRY( slbInitializeSharedMemory( gSlSharedMemory,
                                        aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Server Library Layer를 INIT 시킨다.
 * @param[in] aShutdownInfo ShutdownInfo
 * @param[in,out] aEnv      Environment 구조체
 * @remarks
 */
stlStatus slbShutdownInit( knlShutdownInfo  * aShutdownInfo,
                           void             * aEnv )
{
    STL_ASSERT( aEnv != NULL );

    STL_TRY_THROW( aShutdownInfo->mShutdownMode > KNL_SHUTDOWN_MODE_ABORT,
                   RAMP_SHUTDOWN_ABORT );

    (void) knlDestroyEnvPool( KNL_ERROR_STACK(aEnv) );
    
    STL_RAMP( RAMP_SHUTDOWN_ABORT );

    return STL_SUCCESS;
}

/**
 * @brief Server Library Layer를 DISMOUNT 시킨다.
 * @param[in] aShutdownInfo ShutdownInfo
 * @param[in,out] aEnv      Environment 구조체
 * @remarks
 */
stlStatus slbShutdownDismount( knlShutdownInfo  * aShutdownInfo,
                               void             * aEnv )
{
    STL_ASSERT( aEnv != NULL );
    
    STL_TRY_THROW( aShutdownInfo->mShutdownMode > KNL_SHUTDOWN_MODE_ABORT,
                   RAMP_SHUTDOWN_ABORT );

    STL_RAMP( RAMP_SHUTDOWN_ABORT );

    return STL_SUCCESS;
}

/**
 * @brief Server Library Layer를 CLOSE 시킨다.
 * @param[in] aShutdownInfo ShutdownInfo
 * @param[in,out] aEnv      Environment 구조체
 * @remarks
 */
stlStatus slbShutdownClose( knlShutdownInfo  * aShutdownInfo,
                            void             * aEnv )
{
    STL_ASSERT( aEnv != NULL );

    /**
     * shutdown abort시에도 수행되어야 한다.
     * - Semaphore를 삭제해야 한다.
     */
    STL_TRY( slbFinalizeSharedMemory( gSlSharedMemory,
                                      aEnv )
             == STL_SUCCESS );

    STL_TRY_THROW( aShutdownInfo->mShutdownMode > KNL_SHUTDOWN_MODE_ABORT,
                   RAMP_SHUTDOWN_ABORT );

    STL_RAMP( RAMP_SHUTDOWN_ABORT );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Fixed Table 들을 등록함
 * @param[in]  aEnv   Environment
 * @remarks
 * NO_MOUNT 단계와 Warm-Up 단계에서 동일한 순서로 등록되어야 함.
 */
stlStatus slbRegisterFxTables( void * aEnv )
{
    STL_TRY( knlRegisterFxTable( &gSlbSystemInfoTableDesc,
                                 KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    STL_TRY( knlRegisterFxTable( &gSlbDispatcherTableDesc,
                                 KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    STL_TRY( knlRegisterFxTable( &gSlbSharedServerTableDesc,
                                 KNL_ENV(aEnv) ) == STL_SUCCESS );

    STL_TRY( knlRegisterFxTable( &gSlbBalancerTableDesc,
                                 KNL_ENV(aEnv) ) == STL_SUCCESS );

    STL_TRY( knlRegisterFxTable( &gSlbQueueTableDesc,
                                 KNL_ENV(aEnv) ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_SUCCESS;
}

/**
 * @brief Server Library Layer의 Fatal Handler.
 * @param[in] aCauseString Fatal 유발 원인을 알수 있는 스트링
 * @param[in] aSigInfo Signal Information
 * @param[in] aContext 이전 Context 정보
 */
void slbFatalHandler( const stlChar * aCauseString,
                      void          * aSigInfo,
                      void          * aContext )
{
    sllEnv * sMyEnv   = NULL;
    
    sMyEnv = (sllEnv*) knlGetMyEnv();
    
    STL_TRY_THROW( sMyEnv != NULL, RAMP_FINISH );
    STL_TRY_THROW( knlGetTopLayer(KNL_ENV(sMyEnv))
                   >= STL_LAYER_SERVER_LIBRARY,
                   RAMP_FINISH );
    
    if( KNL_GET_FATAL_HANDLING( sMyEnv, STL_LAYER_SERVER_LIBRARY ) == STL_FALSE )
    {
        /**
         * 무한 Fatal을 방지해야 한다.
         */
        
        KNL_SET_FATAL_HANDLING( sMyEnv, STL_LAYER_SERVER_LIBRARY );
    }
    
    STL_RAMP( RAMP_FINISH );
    
    /**
     * SESSION Layer의 Fatal Handler를 호출한다.
     */
    
    gSlbOldFatalHandler( aCauseString, aSigInfo, aContext );
}


/**
 * @brief SharedMemory(ImWarmupEntry)를 초기화 한다.
 */
stlStatus slbInitializeSharedMemory( sllSharedMemory  * aSharedMemory,
                                     void             * aEnv )
{
    stlInt32            sIdx;
    void              * sPos;
    stlInt64            sMemorySize;

    /**
     * shared session을 위한 property 가져오기
     * property 변경 적용을 위해서 위치 변경 검토 필요함.
     *
     */
    stlBool             sActiveShared;
    stlInt32            sDispatchers;
    stlInt32            sSharedServers;
    stlInt32            sMaxDispatchers;
    stlInt32            sMaxSharedServers;
    stlInt32            sSharedQueueSize;
    stlInt32            sConnections;
    stlInt32            sUnitSize;
    stlInt32            sNetBufferSize;
    stlInt64            sElementBodySize;
    stlInt64            sElementCount;
    stlSize             sAlignmentSize;
    stlInt32            sRequestQueueCount;

    sActiveShared = knlGetPropertyBoolValueByID( KNL_PROPERTY_SHARED_SESSION,
                                                 KNL_ENV(aEnv) );

    aSharedMemory->mExitFlag = STL_FALSE;

    /* sActiveShared가 STL_FALSE면 초기화 할필요 없음 */
    STL_TRY_THROW( sActiveShared == STL_TRUE, RAMP_SUCCESS );

    sMaxDispatchers = KNL_PROPERTY_MAX(KNL_PROPERTY_DISPATCHERS);

    sMaxSharedServers = KNL_PROPERTY_MAX(KNL_PROPERTY_SHARED_SERVERS);

    sDispatchers = knlGetPropertyBigIntValueByID( KNL_PROPERTY_DISPATCHERS,
                                                  KNL_ENV(aEnv) );

    sSharedServers = knlGetPropertyBigIntValueByID( KNL_PROPERTY_SHARED_SERVERS,
                                                    KNL_ENV(aEnv) );

    sSharedQueueSize = knlGetPropertyBigIntValueByID( KNL_PROPERTY_DISPATCHER_QUEUE_SIZE,
                                                      KNL_ENV(aEnv) );

    sConnections = knlGetPropertyBigIntValueByID( KNL_PROPERTY_DISPATCHER_CONNECTIONS,
                                                  KNL_ENV(aEnv) );

    sUnitSize = knlGetPropertyBigIntValueByID( KNL_PROPERTY_DISPATCHER_CM_UNIT_SIZE,
                                               KNL_ENV(aEnv) );

    sNetBufferSize = knlGetPropertyBigIntValueByID( KNL_PROPERTY_NET_BUFFER_SIZE,
                                                    KNL_ENV(aEnv) );

    sAlignmentSize = knlGetPropertyBigIntValueByID( KNL_PROPERTY_CACHE_ALIGNED_SIZE,
                                                    KNL_ENV(aEnv) );

    sRequestQueueCount = knlGetPropertyBigIntValueByID( KNL_PROPERTY_SHARED_REQUEST_QUEUE_COUNT,
                                                        KNL_ENV(aEnv) );

    aSharedMemory->mNetBufferSize = sNetBufferSize;
    aSharedMemory->mUnitSize = sUnitSize;
    aSharedMemory->mMaxDispatchers = sMaxDispatchers;
    aSharedMemory->mMaxSharedServers = sMaxSharedServers;
    aSharedMemory->mDispatchers = sDispatchers;
    aSharedMemory->mSharedServers = sSharedServers;
    aSharedMemory->mConnections = sConnections;
    aSharedMemory->mRequestQueueCount = sRequestQueueCount;

    /* sNetBufferSize 는 MTU_SIZE보다 커야 함. */
    STL_DASSERT( sNetBufferSize >= CML_MTU_SIZE );

    /* KNL_PROPERTY_DISPATCHER_CM_UNIT_SIZE 는 MTU_SIZE보다 커야 함. */
    STL_DASSERT( sUnitSize >= CML_MTU_SIZE );

    /* mSharedServer 는 aSharedMemory 구조체 바로 다음부터 */
    sPos = aSharedMemory + STL_SIZEOF(sllSharedMemory);

    /* OPEN단계에서 shared-server 개수가 변경 가능함으로 property max로 할당한다 */
    aSharedMemory->mSharedServer = sPos;
    sPos += STL_SIZEOF(sllSharedServer) * sMaxSharedServers;

    /* OPEN단계에서 dispatcher 개수가 변경 가능함으로 property max로 할당한다 */
    aSharedMemory->mDispatcher = sPos;
    sPos += STL_SIZEOF(sllDispatcher) * sMaxDispatchers;

    /**
     * dispatcher 초기화
     * dispatcher는 dispatcher header와 dispatcher의 가변 queue로 구성.
     */
    for( sIdx = 0; sIdx < sMaxDispatchers; sIdx++ )
    {
        aSharedMemory->mDispatcher[sIdx].mResponseEvent = sPos;

        STL_TRY( sclInitializeResponseQueue( aSharedMemory->mDispatcher[sIdx].mResponseEvent,
                                             sSharedQueueSize,
                                             &sMemorySize,
                                             SCL_ENV(aEnv) )
                 == STL_SUCCESS );

        sPos += sMemorySize;
    }

    /**
     * request queue 초기화
     */

    for( sIdx = 0; sIdx < sRequestQueueCount; sIdx++ )
    {
        aSharedMemory->mRequestEvent[sIdx] = sPos;
        STL_TRY( sclInitializeRequestQueue( aSharedMemory->mRequestEvent[sIdx],
                                            sSharedQueueSize,
                                            &sMemorySize,
                                            SCL_ENV(aEnv) )
                 == STL_SUCCESS );
        sPos += sMemorySize;
    }

    STL_TRY( knlInitMemController( &(aSharedMemory->mMemController),
                                   sMaxDispatchers * sConnections * STL_SIZEOF(sllHandle),
                                   KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlCreateDynamicMem( &aSharedMemory->mSessionAllocator,
                                  NULL, /* aParentMem */
                                  KNL_ALLOCATOR_SERVER_LIBRARY_SHARED_HANDLE,
                                  KNL_MEM_STORAGE_TYPE_SHM,
                                  SLB_SESSION_MEM_INIT_SIZE,
                                  SLB_SESSION_MEM_NEXT_SIZE,
                                  &aSharedMemory->mMemController, /* aMemController */
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sElementBodySize = aSharedMemory->mUnitSize + STL_SIZEOF(sclIpcPacket);

    /**
     * allocator는 cache line align을 맞춘다
     */
    sPos = (void *)STL_ALIGN((stlSize)sPos, sAlignmentSize);

    /* 남은 메모리 계산 */
    sMemorySize = aSharedMemory->mTotalSize - ( (stlUInt64)sPos - (stlUInt64)aSharedMemory );

    /* data용도의 memory가 최소 5메가 이상이어야 한다 */
    STL_DASSERT( sMemorySize > 1024 * 1024 * 5 );

    aSharedMemory->mPacketAllocator = sPos;
    STL_TRY( stlInitializeArrayAllocatorByTotalSize( aSharedMemory->mPacketAllocator,
                                                     sElementBodySize,
                                                     sMemorySize,
                                                     &sElementCount,
                                                     sAlignmentSize,
                                                     KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    sPos += sMemorySize;

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief StaticArea에 detach한다.
 */
stlStatus slbFinalizeSharedMemory( sllSharedMemory  * aSharedMemory,
                                   void             * aEnv )
{
    stlInt32    sIdx;
    stlBool     sActiveShared;

    sActiveShared = knlGetPropertyBoolValueByID( KNL_PROPERTY_SHARED_SESSION,
                                                 KNL_ENV(aEnv) );

    if( sActiveShared == STL_TRUE )
    {
        for( sIdx = 0; sIdx < aSharedMemory->mMaxDispatchers; sIdx++ )
        {
            (void)sclFinalizeResponseQueue( aSharedMemory->mDispatcher[sIdx].mResponseEvent,
                                            SCL_ENV(aEnv) );
        }

        for( sIdx = 0; sIdx < aSharedMemory->mRequestQueueCount; sIdx++ )
        {
            (void)sclFinalizeRequestQueue( aSharedMemory->mRequestEvent[sIdx],
                                           SCL_ENV(aEnv) );
        }

        STL_TRY( knlFiniMemController( &(aSharedMemory->mMemController),
                                   KNL_ENV(aEnv) )
             == STL_SUCCESS );

        STL_TRY( stlFinalizeArrayAllocator( aSharedMemory->mPacketAllocator,
                                            KNL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

