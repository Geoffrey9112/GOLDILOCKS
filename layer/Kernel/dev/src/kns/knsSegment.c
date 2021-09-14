/*******************************************************************************
 * knsSegment.c
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
 * @file knsSegment.c
 * @brief Kernel Layer Shared Memory Segment Internal Routines
 */

#include <stl.h>
#include <dtl.h>
#include <cml.h>
#include <knlDef.h>
#include <knDef.h>
#include <knpDef.h>
#include <knlRing.h>
#include <knlDynamicMem.h>
#include <knlRegionMem.h>
#include <knDef.h>
#include <knsSegment.h>
#include <knlLatch.h>
#include <knlFixedTable.h>
#include <knlProperty.h>
#include <knlEvent.h>
#include <knlTraceLogger.h>
#include <knpPropertyManager.h>
#include <knmDynamic.h>

extern knlAllocatorDesc gKnmAllocatorDesc[];

/**
 * @addtogroup knsSegment
 * @{
 */

/**
 * @brief 커널의 각 모듈들에 대한 entry point
 */
knsEntryPoint * gKnEntryPoint;

/**
 * @brief 공유 메모리 세그먼트와 프로세스에서의 주소 관리를 위한 전역 변수
 */
knlShmAddrTable gShmAddrTable;

/**
 * @brief static area의 크기
 */
stlInt64 gShmStaticSize;

/**
 * @brief 공유 메모리 관리자 Magic Number
 */
#define KNS_SHM_STATIC_MAGIC_NUMBER 0x225DD623

/**
 * @brief 공유 메모리 할당 단위 (16M)
 */
#define KNS_SHM_GRANULE (16 * 1024 * 1024)

/**
 * @brief static area의 상태를 조사한다.
 */
stlBool knsValidateStaticArea()
{
    knlShmManager      * sShmManager;
    knlShmTableElement * sElement;
    stlShm             * sStaticShm;

    sShmManager = gShmAddrTable.mShmManager;

    STL_TRY( sShmManager != NULL );
    STL_TRY( sShmManager->mSegmentCount > 0 );
    STL_TRY( sShmManager->mMagicNumber == KNS_SHM_STATIC_MAGIC_NUMBER );

    sStaticShm = &(gShmAddrTable.mStaticArea);
    sElement = &(sShmManager->mSegment[KNS_ADDR_TABLE_STATIC_AREA]);

    STL_TRY( stlStrcmp(sElement->mName, sStaticShm->mName) == 0 );
    STL_TRY( sElement->mSize == sStaticShm->mSize );
    STL_TRY( sElement->mKey == sStaticShm->mKey );

    STL_TRY( gKnEntryPoint != NULL );
    STL_TRY( gKnEntryPoint->mDynamicMem != NULL );

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief 프로세스에서 유효한 공유 메모리 세그먼트 물리적 주소를 초기화 한다.
 * @param[in]      aStaticArea  Address of Static Area Segment    
 * @param[in,out]  aEnv         커널 Environment
 */
stlStatus knsInitializeAddrTable( void   * aStaticArea,
                                  knlEnv * aEnv )
{
    stlMemset( gShmAddrTable.mAddr,
               0x00,
               STL_SIZEOF(void*) * KNL_SHM_TABLE_ELEMENT_COUNT );

    gShmAddrTable.mAddr[KNS_ADDR_TABLE_STATIC_AREA] = aStaticArea;

    STL_TRY( knlInitLatch( &gShmAddrTable.mLatch,
                           STL_FALSE,  /* aIsShm */
                           aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 프로세스에서 유효한 공유 메모리 세그먼트 물리적 주소를 정리한다.
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knsFinalizeAddrTable( knlEnv * aEnv )
{
    knlFinLatch( &gShmAddrTable.mLatch );
    
    stlMemset( gShmAddrTable.mAddr,
               0x00,
               STL_SIZEOF(void*) * KNL_SHM_TABLE_ELEMENT_COUNT );

    return STL_SUCCESS;
}

/**
 * @brief static area의 variable 영역을 초기화한다.
 * @param[in] aStartAddr static area의 시작 주소
 * @param[in] aSize static area의 크기
 * @param[in,out] aEnv 커널 Environment
 * @remark fixed 영역은 static area의 앞에서부터 할당되며
 *         variable 영역은 static area의 뒤에서부터 할당된다.
 */
stlStatus knsInitVariableStaticArea( void    * aStartAddr,
                                     stlSize   aSize,
                                     knlEnv  * aEnv )
{
    knlDynamicMem    * sDynamicMem;
    knmChunkHeader   * sChunkHeader;
    stlUInt64          sInitSize;
    knlAllocatorDesc * sAllocatorDesc;

    sAllocatorDesc = &(gKnmAllocatorDesc[KNL_ALLOCATOR_KERNEL_SHARED_VARIABLE_STATIC_AREA]);
    
    /*
     * 할당된 공유 메모리 세그먼트의 마지막 영역에
     * variable static area 관리자가 존재한다.
     */
    sInitSize = aSize - gKnEntryPoint->mFixedOffset -
                STL_SIZEOF(knlDynamicMem) - STL_SIZEOF(knmChunkHeader);
    sDynamicMem = aStartAddr + aSize - STL_SIZEOF(knlDynamicMem);

    sDynamicMem->mCategory = sAllocatorDesc->mAllocatorCategory;
    sDynamicMem->mType = sAllocatorDesc->mStorageType;
    sDynamicMem->mAllocatorLevel = sAllocatorDesc->mAllocatorLevel;
    sDynamicMem->mInitSize = sInitSize;
    sDynamicMem->mNextSize = 0;
    sDynamicMem->mMaxSize = STL_INT64_MAX;
    sDynamicMem->mTotalSize = 0;
    sDynamicMem->mMinFragSize = sAllocatorDesc->mMinFragSize;
    sDynamicMem->mController = NULL;

    STL_TRY( knlInitLatch( &(sDynamicMem->mLatch),
                           STL_TRUE,
                           aEnv ) == STL_SUCCESS );

    STL_RING_INIT_HEADLINK( (&sDynamicMem->mChunkRing),
                            STL_OFFSETOF(knmChunkHeader, mLink) );
    
    /*
     * variable static area 관리자 앞에 chunk header가 있다.
     */
    sChunkHeader = (void*)sDynamicMem - STL_SIZEOF(knmChunkHeader);
    sChunkHeader->mFreeBlockCount = 0;
    sChunkHeader->mFreeBlockSize  = 0;
    sChunkHeader->mSize = sInitSize;

    STL_RING_INIT_DATALINK( sChunkHeader,
                            STL_OFFSETOF(knmChunkHeader, mLink) );

    STL_RING_INIT_HEADLINK( (&sChunkHeader->mAllocRing),
                            STL_OFFSETOF(knmBlockHeader, mAllocLink) );

    STL_RING_INIT_HEADLINK( (&sChunkHeader->mFreeRing),
                            STL_OFFSETOF(knmBlockHeader, mFreeLink) );

    STL_RING_ADD_FIRST( &(sDynamicMem->mChunkRing),
                        sChunkHeader );

    gKnEntryPoint->mDynamicMem = sDynamicMem;

    /**
     * Resister Dynamic Allocator
     */
    
    STL_TRY( knmRegisterDynamicMem( sDynamicMem,
                                    aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief static area의 variable 영역에서 메모리를 할당 받는다.
 * @param[in] aSize 메모리 요청 크기
 * @param[out] aAddr 할당된 메모리 주소
 * @param[in,out] aEnv 커널 Environment
 * @par Error Codes :
 * @code
 * [KNL_ERRCODE_INVALID_STATIC_SHM]
 *     attach 된 static shared memory segment가 invalid 하다.
 * @endcode
 * @todo 공유 메모리 부족 에러를 추가해야 한다.
 */
stlStatus knsAllocVariableStaticArea( stlSize   aSize,
                                      void   ** aAddr,
                                      knlEnv  * aEnv )
{
    STL_TRY_THROW( knsValidateStaticArea() == STL_TRUE,
                   RAMP_ERR_INVALID_STATIC_SHM );

    STL_TRY( knlAllocDynamicMem( gKnEntryPoint->mDynamicMem,
                                 aSize,
                                 aAddr,
                                 aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_STATIC_SHM )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_INVALID_STATIC_SHM,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;
                                 
    return STL_FAILURE;
}

/**
 * @brief static area의 variable 영역에서 할당된 메모리를 반환한다.
 * @param[in] aAddr 반환할 메모리 주소
 * @param[in,out] aEnv 커널 Environment
 * @par Error Codes :
 * @code
 * [KNL_ERRCODE_INVALID_STATIC_SHM]
 *     attach 된 static shared memory segment가 invalid 하다.
 * @endcode
 */
stlStatus knsFreeVariableStaticArea( void   * aAddr,
                                     knlEnv * aEnv )
{
    STL_TRY_THROW( knsValidateStaticArea() == STL_TRUE,
                   RAMP_ERR_INVALID_STATIC_SHM );

    STL_TRY( knlFreeDynamicMem( gKnEntryPoint->mDynamicMem,
                                aAddr,
                                aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_STATIC_SHM )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_INVALID_STATIC_SHM,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;
                                 
    return STL_FAILURE;
}

/**
 * @brief Variable Static Area가 사용하고 있는 메모리 공간을 반환한다.
 * @return Unused Variable Static Area Size
 */
stlSize knsGetVariableStaticAreaTotalSize()
{
    void           * sStaticEndAddr;
    knmChunkHeader * sChunkHeader;
    void           * sLastUsedVariableAddr;

    STL_DASSERT( gKnEntryPoint != NULL );
    
    /**
     * |                   <-------unused----->|<----------used---------->|
     * |<-----------------><--------------------------------------------->|
     * |    fixed area                     variable area                  |
     */
    
    sStaticEndAddr = ( gShmAddrTable.mStaticArea.mAddr +
                       gShmAddrTable.mStaticArea.mSize );

    sChunkHeader = (knmChunkHeader*)STL_RING_GET_FIRST_DATA( &(gKnEntryPoint->mDynamicMem->mChunkRing) );
    sLastUsedVariableAddr = STL_RING_GET_LAST_DATA( &(sChunkHeader->mAllocRing) );

    return (sStaticEndAddr - sLastUsedVariableAddr);
}

/**
 * @brief Variable Static Area중 단편화된 메모리중 free 메모리 공간을 반환한다.
 * @return fragmented free size
 */
stlSize knsGetVariableStaticAreaFragSize()
{
    knmChunkHeader * sChunkHeader;

    STL_DASSERT( gKnEntryPoint != NULL );
    
    sChunkHeader = (knmChunkHeader*)STL_RING_GET_FIRST_DATA( &(gKnEntryPoint->mDynamicMem->mChunkRing) );

    return sChunkHeader->mFreeBlockSize;
}

/**
 * @brief static area를 생성한다.
 * @param[in,out] aEnv 커널 Environment
 * @remark 동시성 제어를 하지 않는다.
 * @todo 생성할 공유 메모리 크기를 property에서 읽어와야 한다.
 */
stlStatus knsCreateStaticArea( knlEnv * aEnv )
{
    knlShmManager      * sShmManager;
    knlShmTableElement * sElement;
    stlShm             * sShm;
    stlInt32             sIdx;
    void               * sBaseAddr;
    stlInt32             sLayerEntrySize;
    stlUInt64          * sFixedOffset;
    stlChar              sShmStaticName[KNL_PROPERTY_STRING_MAX_LENGTH] = {0};
    stlInt64             sShmStaticKey  = 0;
    stlInt64             sShmAddress = 0;
    stlInt64             sCacheAlignedSize = 0;
    cmlShmStaticFileInfo sShmInfo;
    stlBool              sHugeTlb;
    void              ** sKnEntryAddr;

    /*
     * Property에서 필요한 값을 읽어온다.
     */
    
    STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_SHARED_MEMORY_NAME,
                                      sShmStaticName,
                                      aEnv ) == STL_SUCCESS );
    
    STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_SHARED_MEMORY_KEY,
                                      &sShmStaticKey,
                                      aEnv ) == STL_SUCCESS );

    STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_SHARED_MEMORY_ADDRESS,
                                      &sShmAddress,
                                      aEnv ) == STL_SUCCESS );

    STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_CACHE_ALIGNED_SIZE,
                                      &sCacheAlignedSize,
                                      aEnv ) == STL_SUCCESS );
    
    /**
     * @bug 공유메모리 크기를 align 해야 한다.
     */
    STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_SHARED_MEMORY_SIZE,
                                      &gShmStaticSize,
                                      aEnv ) == STL_SUCCESS );

    stlMemset( &sShmInfo, 0x00, STL_SIZEOF( cmlShmStaticFileInfo ) );
    
    sHugeTlb = knlGetPropertyBoolValueByID( KNL_PROPERTY_USE_LARGE_PAGES,
                                            aEnv );
    
    if( stlSupportShareAddressShm() == STL_TRUE )
    {
        /**
         * Semaphore와의 동시성 문제로 인하여 Static Area는 HugeTlb를 사용하지 않는다.
         */
        STL_TRY( stlCreateShm( &(gShmAddrTable.mStaticArea),
                               sShmStaticName,
                               (stlInt32)(sShmStaticKey),
                               (void*)sShmAddress,
                               STL_ALIGN((stlSize)gShmStaticSize, KNS_SHM_GRANULE),
                               sHugeTlb,  /* aHugeTlb */
                               KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
        
        sBaseAddr = stlGetShmBaseAddr( &(gShmAddrTable.mStaticArea) );
    }
    else
    {
        /**
         * address를 지정해서 Shared Memory를 생성할수 없는 경우라면
         * 파일에 address를 저장하고, attach시에 이를 사용한다.
         */
        STL_TRY( stlCreateShm( &(gShmAddrTable.mStaticArea),
                               sShmStaticName,
                               (stlInt32)(sShmStaticKey),
                               (void*)NULL,
                               STL_ALIGN((stlSize)gShmStaticSize, KNS_SHM_GRANULE),
                               sHugeTlb,  /* aHugeTlb */
                               KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

        sBaseAddr = stlGetShmBaseAddr( &(gShmAddrTable.mStaticArea) );
    }
    
    /**
     * Attach에 사용될 정보를 저장한다.
     */
    stlStrcpy( sShmInfo.mName, sShmStaticName );
    sShmInfo.mAddr = (stlInt64)sBaseAddr;
    sShmInfo.mKey  = sShmStaticKey;
    
    STL_TRY( cmlSaveStaticAreaInfo( &sShmInfo,
                                    KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    /*
     * 커널 entry point 초기화
     */
    sLayerEntrySize = STL_SIZEOF(void*) * STL_LAYER_KERNEL;
    sKnEntryAddr = (void*)(sBaseAddr + sLayerEntrySize);

    sLayerEntrySize = STL_SIZEOF(void*) * STL_LAYER_MAX;
    gKnEntryPoint = (knsEntryPoint*)(sBaseAddr + sLayerEntrySize);

    *sKnEntryAddr = gKnEntryPoint;
    
    gKnEntryPoint->mStartupPhase     = KNL_STARTUP_PHASE_NONE;
    gKnEntryPoint->mSar              = KNL_SAR_NONE;
    gKnEntryPoint->mStartupTime      = stlNow();
    gKnEntryPoint->mSystemTime       = stlNow();
    gKnEntryPoint->mFixedOffset      = sLayerEntrySize + STL_SIZEOF(knsEntryPoint);
    gKnEntryPoint->mShmManager       = NULL;
    gKnEntryPoint->mDynamicMem       = NULL;
    gKnEntryPoint->mEnvManager       = NULL;
    gKnEntryPoint->mProcessManager   = NULL;
    gKnEntryPoint->mBreakPointInfo   = NULL;
    gKnEntryPoint->mCacheAlignedSize = sCacheAlignedSize;
    gKnEntryPoint->mDynamicArrayCount = 0;

    stlMemset( gKnEntryPoint->mDynamicMemArray,
               0x00,
               STL_SIZEOF(knlDynamicMem*) * KNM_MAX_DYNAMIC_ALLOCATOR_COUNT );
    
    KNL_INIT_STATIC_AREA_MARK( &(gKnEntryPoint->mStaticMark4Mount) );
    KNL_INIT_STATIC_AREA_MARK( &(gKnEntryPoint->mStaticMark4Open) );

    sFixedOffset = &(gKnEntryPoint->mFixedOffset);

    /*
     * 공유 메모리 관리자 할당
     */
    sShmManager = (knlShmManager*)(sBaseAddr + *sFixedOffset);
    *sFixedOffset = (*sFixedOffset) + STL_SIZEOF(knlShmManager);

    gKnEntryPoint->mShmManager = sShmManager;
    gShmAddrTable.mShmManager  = sShmManager;

    /*
     * 공유 메모리 관리자 초기화
     */
    STL_TRY( knlInitLatch( &(sShmManager->mLatch),
                           STL_TRUE,
                           aEnv ) == STL_SUCCESS );

    sShmManager->mSegmentCount = 1;
    sShmManager->mMagicNumber = KNS_SHM_STATIC_MAGIC_NUMBER;
    sShmManager->mKeySeed = (stlInt32)sShmStaticKey;

    /**
     * Binding Static Area Segment
     */
    sShm = &(gShmAddrTable.mStaticArea);
    sElement = &(sShmManager->mSegment[KNS_ADDR_TABLE_STATIC_AREA]);

    sElement->mSize = sShm->mSize;
    sElement->mKey = sShm->mKey;
    sElement->mSeq = 0;
    stlStrcpy( sElement->mName, sShm->mName );

    for( sIdx = (KNS_ADDR_TABLE_STATIC_AREA + 1);
         sIdx < KNL_SHM_TABLE_ELEMENT_COUNT;
         sIdx++ )
    {
        sElement = &(sShmManager->mSegment[sIdx]);
        sElement->mSize = 0;
        sElement->mSeq = 0;
        sElement->mKey = KNS_INVALID_SHM_KEY;
        sElement->mName[0] = '\0';
    }

    /*
     * 공유 메모리 세그먼트와 물리적 주소를 저장하는 테이블 초기화
     */
    STL_TRY( knsInitializeAddrTable( (void*)sBaseAddr,
                                     aEnv ) == STL_SUCCESS );

    /*
     * variable static area 초기화
     */
    STL_TRY( knsInitVariableStaticArea( sBaseAddr,
                                        (stlSize)gShmStaticSize,
                                        aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;    
}

/**
 * @brief static area를 attach한다.
 * @param[in,out] aEnv 커널 Environment
 * @par Error Codes :
 * @code
 * [KNL_ERRCODE_INVALID_STATIC_SHM]
 *     attach 된 static shared memory segment가 invalid 하다.
 * @remark 동시성 제어를 하지 않는다.
 * @endcode
 */
stlStatus knsAttachStaticArea( knlEnv * aEnv )
{
    void                  * sBaseAddr;
    stlInt32                sLayerEntrySize;
    cmlShmStaticFileInfo    sShmInfo;
    void                 ** sKnEntryAddr;
    
    if( knsValidateStaticArea() == STL_FALSE )
    {
        /**
         * attach static area segment
         */
        STL_TRY( cmlLoadStaticAreaInfo( &sShmInfo,
                                        KNL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );

        STL_TRY( stlAttachShm( &(gShmAddrTable.mStaticArea),
                               sShmInfo.mName,
                               (stlInt32)sShmInfo.mKey,
                               (void*)sShmInfo.mAddr,
                               KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

        sBaseAddr = stlGetShmBaseAddr( &(gShmAddrTable.mStaticArea) );        

        sLayerEntrySize = STL_SIZEOF(void*) * STL_LAYER_KERNEL;
        sKnEntryAddr  = (void**)(sBaseAddr + sLayerEntrySize);
        
        sLayerEntrySize = STL_SIZEOF(void*) * STL_LAYER_MAX;
        gKnEntryPoint   = (knsEntryPoint*)(sBaseAddr + sLayerEntrySize);

        *sKnEntryAddr = gKnEntryPoint;
        
        STL_TRY_THROW( gKnEntryPoint != NULL, RAMP_ERR_INVALID_STATIC_SHM );
        STL_TRY_THROW( gKnEntryPoint->mFixedOffset != 0,
                       RAMP_ERR_INVALID_STATIC_SHM );
        STL_TRY_THROW( gKnEntryPoint->mShmManager != NULL,
                       RAMP_ERR_INVALID_STATIC_SHM );
        STL_TRY_THROW( gKnEntryPoint->mDynamicMem != NULL,
                       RAMP_ERR_INVALID_STATIC_SHM );
        STL_TRY_THROW( gKnEntryPoint->mEnvManager != NULL,
                       RAMP_ERR_INVALID_STATIC_SHM );
        STL_TRY_THROW( gKnEntryPoint->mProcessManager != NULL,
                       RAMP_ERR_INVALID_STATIC_SHM );

        gShmAddrTable.mShmManager = gKnEntryPoint->mShmManager;

        STL_TRY_THROW( knsValidateStaticArea() == STL_TRUE,
                       RAMP_ERR_INVALID_STATIC_SHM );
        
        gShmStaticSize = gShmAddrTable.mShmManager->mSegment[KNS_ADDR_TABLE_STATIC_AREA].mSize;

        /*
         * 공유 메모리 세그먼트와 물리적 주소를 저장하는 테이블 초기화 */
        STL_TRY( knsInitializeAddrTable( (void*)sBaseAddr,
                                         aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_INVALID_STATIC_SHM)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_INVALID_STATIC_SHM,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    return STL_FAILURE;    
}

/**
 * @brief static area를 프로세스에서 detach 한다.
 * @param[in,out] aEnv 커널 Environment
 * @par Error Codes :
 * @code
 * [KNL_ERRCODE_INVALID_STATIC_SHM]
 *     static shared memory segment가 invalid 하다.
 * @remark 동시성 제어를 하지 않는다.
 * @endcode
 */
stlStatus knsDetachStaticArea( knlEnv * aEnv )
{
    stlShm sShm;

    STL_TRY_THROW( knsValidateStaticArea() == STL_TRUE,
                   RAMP_ERR_INVALID_STATIC_SHM );

    sShm.mAddr = gShmAddrTable.mAddr[KNS_ADDR_TABLE_STATIC_AREA];

    STL_TRY( stlDetachShm( &sShm,
                           KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    STL_TRY( knsFinalizeAddrTable( aEnv ) == STL_SUCCESS );

    gShmAddrTable.mShmManager = NULL;
    gKnEntryPoint = NULL;

    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_INVALID_STATIC_SHM)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_INVALID_STATIC_SHM,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    return STL_FAILURE;    
}

/**
 * @brief static area를 destroy 한다.
 * @param[in,out] aEnv 커널 Environment
 * @par Error Codes :
 * @code
 * [KNL_ERRCODE_INVALID_STATIC_SHM]
 *     static shared memory segment가 invalid 하다.
 * @endcode
 * @remark 동시성 제어를 하지 않는다.
 */
stlStatus knsDestroyStaticArea( knlEnv * aEnv )
{
    STL_TRY( stlDestroyShm( &(gShmAddrTable.mStaticArea),
                            KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    STL_TRY( knsFinalizeAddrTable( aEnv ) == STL_SUCCESS );

    gShmAddrTable.mShmManager = NULL;
    gKnEntryPoint = NULL;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;    
}

/**
 * @brief 공유 메모리 세그먼트를 생성한다.
 * @param[in]     aType      생성할 공유 메모리 세그먼트 타입
 * @param[in]     aName      공유 메모리 이름
 * @param[out]    aIdx       생성된 공유 메모리 세그먼트의 ID
 * @param[in]     aReqSize   생성할 공유메모리 세그먼트 크기
 * @param[in,out] aEnv       커널 Environment
 * @par Error Codes :
 * @code
 * [KNL_ERRCODE_INVALID_SHM_TYPE]
 *     입력받은 공유 메모리 세그먼트의 타입이 올바르지 않음
 * [KNL_ERRCODE_INVALID_STATIC_SHM]
 *     static shared memory segment가 invalid 하다.
 * [KNL_ERRCODE_EXCEEDED_SHM_SEGMENT_COUNT]
 *     생성할 수 있는 공유 메모리 세그먼트의 최대값을 넘었다.
 * @endcode
 * @internal
 */
stlStatus knsCreateSegment( knsShmSemgnetType    aType,
                            stlChar            * aName,
                            stlUInt16          * aIdx,
                            stlSize              aReqSize,
                            knlEnv             * aEnv )
{
    knlShmManager      * sShmManager;
    knlShmTableElement * sElement;
    stlUInt32            sIdx;
    stlShm               sShm;
    stlChar              sShmName[STL_MAX_SHM_NAME + 1];
    stlBool              sIsSuccess;
    stlInt32             sState = 0;
    stlBool              sHugeTlb;

    /* static shared memory는 할당 할 수 없다. */
    STL_TRY_THROW( (aType > KNS_SHM_SEGMENT_TYPE_STATIC) &&
                   (aType < KNS_SHM_SEGMENT_TYPE_INVALID),
                   RAMP_ERR_INVALID_SHM_TYPE );

    STL_TRY_THROW( knsValidateStaticArea() == STL_TRUE,
                   RAMP_ERR_INVALID_STATIC_SHM );

    sShmManager = gShmAddrTable.mShmManager;

    /**
     * CREATE TABLESPACE -> DROP TABLESPACE -> CREATE TABLESPACE 를 연속적으로 할경우
     * 여기서 knlExecuteProcessEvent()를 호출해야지만 shared memory detach가 정삭적으로 됨.
     */
    STL_TRY( knlExecuteProcessEvent( aEnv ) == STL_SUCCESS );

    /*
     * 공유 메모리 세그먼트를 생성 후 공유 메모리 세그먼트 정보를
     * 공유 메모리 세그먼트 관리 array에 추가한다.
     */
    STL_TRY( knlAcquireLatch( &(gShmAddrTable.mLatch),
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsSuccess,
                              aEnv ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( knlAcquireLatch( &(sShmManager->mLatch),
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsSuccess,
                              aEnv ) == STL_SUCCESS );
    sState = 2;

    /**
     * @todo 빈 슬롯을 찾는 알고리즘을 변경해야 한다.
     *       현재 래치를 획득하고 연산하는 시간이 길다.
     */

    for( sIdx = (KNS_ADDR_TABLE_STATIC_AREA + 1); sIdx < KNL_SHM_TABLE_ELEMENT_COUNT; sIdx++ )
    {
        sElement = &(sShmManager->mSegment[sIdx]);

        /**
         * shared-memory영역(sElement)과 heap영역(gShmAddrTable)이 모두 비어 있는 index를 찾는다.
         */
        if( (sElement->mSize == 0) &&
            (sElement->mKey == KNS_INVALID_SHM_KEY) &&
            (gShmAddrTable.mAddr[sIdx] == NULL) &&
            (gShmAddrTable.mSeq[sIdx] == 0) )
        {
            break;
        }
    }

    STL_TRY_THROW( sIdx != KNL_SHM_TABLE_ELEMENT_COUNT,
                   RAMP_ERR_EXCEEDED_SHM_SEGMENT_COUNT );

    if( aIdx != NULL )
    {
        *aIdx = sIdx;
    }

    if( aName != NULL )
    {
        stlSnprintf( sShmName,
                     STL_MAX_SHM_NAME + 1,
                     "%s",
                     aName );
    }
    else
    {
        sShmName[0] = '\0';
    }

    sHugeTlb = knlGetPropertyBoolValueByID( KNL_PROPERTY_USE_LARGE_PAGES,
                                            aEnv );
    
    /*
     * 할당 가능한 공유 메모리 세그먼트 key를  찾는다.
     */
    while(1)
    {
        sShmManager->mKeySeed++;

        if( stlCreateShm( &sShm,
                          sShmName,
                          sShmManager->mKeySeed,
                          NULL,
                          STL_ALIGN(aReqSize, KNS_SHM_GRANULE),
                          sHugeTlb,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS )
        {
            break;
        }

        STL_TRY( stlGetLastErrorCode(KNL_ERROR_STACK( aEnv ) ) == STL_ERRCODE_SHM_EXIST );
        stlPopError( KNL_ERROR_STACK( aEnv ) );
    }

    sShmManager->mSegmentCount = sShmManager->mSegmentCount + 1;
    sElement->mSize = sShm.mSize;
    sElement->mSeq += 1;
    sElement->mKey = sShmManager->mKeySeed;
    stlStrcpy( sElement->mName, sShmName );

    /*
     * 프로세스에서 유효한 공유 메모리의 주소를 설정한다.
     */
    gShmAddrTable.mAddr[sIdx] = stlGetShmBaseAddr(&sShm);
    gShmAddrTable.mSeq[sIdx] = sElement->mSeq;

    sState = 1;
    STL_TRY( knlReleaseLatch( &(sShmManager->mLatch),
                              aEnv ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( knlReleaseLatch( &(gShmAddrTable.mLatch),
                              aEnv ) == STL_SUCCESS );
    
    /*
     * 다른 프로세스에게 공유 메모리 세그먼트 추가를 알린다.
     */
    STL_TRY( knlAddProcessEvent( KNL_EVENT_ATTACH_SHM,
                                 (void*)&sIdx,
                                 STL_SIZEOF( stlUInt16 ),
                                 KNL_BROADCAST_PROCESS_ID,
                                 aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_INVALID_SHM_TYPE)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_INVALID_SHM_TYPE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_CATCH(RAMP_ERR_INVALID_STATIC_SHM)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_INVALID_STATIC_SHM,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_CATCH(RAMP_ERR_EXCEEDED_SHM_SEGMENT_COUNT)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_EXCEEDED_SHM_SEGMENT_COUNT,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void) knlReleaseLatch( &(sShmManager->mLatch), aEnv );
        case 1:
            (void) knlReleaseLatch( &(gShmAddrTable.mLatch), aEnv );
        default:
            break;
    }

    return STL_FAILURE;    
}

/**
 * @brief 공유 메모리 세그먼트를 프로세스에 attach 한다.
 * @param[in] aData attach할 공유 메모리 세그먼트의 ID
 * @param[in] aDataSize 세그먼트 ID의 물리적 크기
 * @param[out] aDone 성공여부
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knsAttachShmEventHandler( void      * aData,
                                    stlUInt32   aDataSize,
                                    stlBool   * aDone,
                                    void      * aEnv )
{
    stlUInt16       sSegmentId;
    knlShmManager * sShmManager;
    stlBool         sIsSuccess;
    stlInt32        sState = 0;

    *aDone = STL_FALSE;
    
    STL_ASSERT( aDataSize == STL_SIZEOF( stlUInt16 ) );
    stlMemcpy( (void*)&sSegmentId, aData, STL_SIZEOF( stlUInt16 ) );

    if( knsValidateStaticArea() == STL_TRUE )
    {
        sShmManager = gShmAddrTable.mShmManager;

        STL_TRY( knlAcquireLatch( &(gShmAddrTable.mLatch),
                                  KNL_LATCH_MODE_EXCLUSIVE,
                                  KNL_LATCH_PRIORITY_NORMAL,
                                  STL_INFINITE_TIME,
                                  &sIsSuccess,
                                  aEnv ) == STL_SUCCESS );
        sState = 1;
        
        STL_TRY( knlAcquireLatch( &(sShmManager->mLatch),
                                  KNL_LATCH_MODE_EXCLUSIVE,
                                  KNL_LATCH_PRIORITY_NORMAL,
                                  STL_INFINITE_TIME,
                                  &sIsSuccess,
                                  aEnv ) == STL_SUCCESS );
        sState = 2;
    
        STL_TRY( knsAttachSegment( sSegmentId,
                                   STL_TRUE, /* aDoLatch */
                                   (knlEnv*)aEnv ) == STL_SUCCESS );
        
        sState = 1;
        STL_TRY( knlReleaseLatch( &(sShmManager->mLatch),
                                  aEnv ) == STL_SUCCESS );
        
        sState = 0;
        STL_TRY( knlReleaseLatch( &(gShmAddrTable.mLatch),
                                  aEnv ) == STL_SUCCESS );
    }

    *aDone = STL_TRUE;

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void) knlReleaseLatch( &(sShmManager->mLatch), aEnv );
        case 1:
            (void) knlReleaseLatch( &(gShmAddrTable.mLatch), aEnv );
        default:
            break;
    }

    /**
     * 실패해도 무시한다.
     * - 이미 삭제된 공유메모리일수 있다.
     */
    STL_INIT_ERROR_STACK( KNL_ERROR_STACK(aEnv) );
    
    return STL_SUCCESS;
}

/**
 * @brief 공유 메모리 세그먼트를 프로세스에 detach 한다.
 * @param[in] aData detach할 공유 메모리 세그먼트의 ID
 * @param[in] aDataSize 세그먼트 ID의 물리적 크기
 * @param[out] aDone 성공여부
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knsDetachShmEventHandler( void      * aData,
                                    stlUInt32   aDataSize,
                                    stlBool   * aDone,
                                    void      * aEnv )
{
    stlUInt16 sSegmentId;

    *aDone = STL_FALSE;
    
    STL_ASSERT( aDataSize == STL_SIZEOF( stlUInt16 ) );
    stlMemcpy( (void*)&sSegmentId, aData, STL_SIZEOF( stlUInt16 ) );
    
    STL_TRY( knsDetachSegment( sSegmentId,
                               (knlEnv*)aEnv ) == STL_SUCCESS );

    *aDone = STL_TRUE;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 이름을 사용하여 공유 메모리 세그먼트 Idx를 얻는다.
 * @param[in] aName Segment 이름
 * @param[out] aIdx 세그먼트 ID
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knsGetSegmentIdxByName( stlChar    * aName,
                                  stlUInt16  * aIdx,
                                  knlEnv     * aEnv )
{
    stlInt32             sIdx;
    stlUInt16            sMatchIdx = 0;
    knlShmTableElement * sElement;
    knlShmManager      * sShmManager;
    stlBool              sIsSuccess;
    stlInt32             sState = 0;

    STL_TRY_THROW( knsValidateStaticArea() == STL_TRUE,
                   RAMP_ERR_INVALID_STATIC_SHM );

    sShmManager = gShmAddrTable.mShmManager;

    STL_TRY( knlAcquireLatch( &(gShmAddrTable.mLatch),
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsSuccess,
                              aEnv ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( knlAcquireLatch( &(sShmManager->mLatch),
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsSuccess,
                              aEnv ) == STL_SUCCESS );
    sState = 2;
    
    for( sIdx = (KNS_ADDR_TABLE_STATIC_AREA + 1); sIdx < KNL_SHM_TABLE_ELEMENT_COUNT; sIdx++ )
    {
        sElement = &(sShmManager->mSegment[sIdx]);

        if( (sElement->mSize == 0) ||
            (sElement->mKey  == KNS_INVALID_SHM_KEY) )
        {
            continue;
        }

        if( stlStrcmp( aName, sElement->mName ) == 0 )
        {
            sMatchIdx = sIdx;
            *aIdx     = sMatchIdx;
            break;
        }
    }

    sState = 1;
    STL_TRY( knlReleaseLatch( &(sShmManager->mLatch),
                              aEnv ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( knlReleaseLatch( &(gShmAddrTable.mLatch),
                              aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_INVALID_STATIC_SHM)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_INVALID_STATIC_SHM,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void) knlReleaseLatch( &(sShmManager->mLatch), aEnv );
        case 1:
            (void) knlReleaseLatch( &(gShmAddrTable.mLatch), aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;    
}

/**
 * @brief 공유 메모리 세그먼트를 프로세스에 attach 한다.
 * @param[in] aIdx attach할 공유 메모리 세그먼트의 ID
 * @param[in] aDoLatch Do Latch
 * @param[in,out] aEnv 커널 Environment
 * @par Error Codes :
 * @code
 * [KNL_ERRCODE_INVALID_STATIC_SHM]
 *     static shared memory segment가 invalid 하다.
 * [KNL_ERRCODE_INVALID_ARGUMENT]
 *     입력받은 공유 메모리 세그먼트의 ID가 올바르지 않음.
 * @endcode
 * @todo 공유 메모리 관리자에 세그먼트 타입 별로 관리 리스트가 필요하다.
 */
stlStatus knsAttachSegment( stlUInt16        aIdx,
                            stlBool          aDoLatch,
                            knlEnv         * aEnv )
{
    knlShmTableElement * sElement;
    knlShmManager      * sShmManager;
    stlShm               sShm;
    stlInt32             sState = 1;
    stlUInt64            sValidSeq = 0;
    stlChar              sName[STL_MAX_SHM_NAME + 1];
    stlInt32             sKey;
    stlBool              sIsSuccess;

    STL_TRY_THROW( knsValidateStaticArea() == STL_TRUE,
                   RAMP_ERR_INVALID_STATIC_SHM );

    sShmManager = gShmAddrTable.mShmManager;
    sElement = &(sShmManager->mSegment[aIdx]);

    STL_TRY_THROW( sElement->mSize != 0, RAMP_SUCCESS );
    STL_TRY_THROW( sElement->mKey != KNS_INVALID_SHM_KEY, RAMP_SUCCESS );

    while( gShmAddrTable.mAddr[aIdx] == NULL )
    {
        if( (sElement->mSize == 0) ||
            (sElement->mKey == KNS_INVALID_SHM_KEY) )
        {
            STL_THROW( RAMP_SUCCESS );
        }

        /**
         * Segment를 2번 이상 attach할 수 없다.
         */
        if( gShmAddrTable.mAddr[aIdx] == NULL )
        {
            if( aDoLatch == STL_TRUE )
            {
                sValidSeq = sElement->mSeq;
                sKey = sElement->mKey;
                stlStrcpy( sName, sElement->mName );

                /**
                 * attach 직렬화로 인한 성능 저하를 막기 위해서 latch를 release 한다.
                 */
                sState = 0;
                STL_TRY( knlReleaseLatch( &(sShmManager->mLatch),
                                          aEnv ) == STL_SUCCESS );

                STL_TRY( stlAttachShm( &sShm,
                                       sName,
                                       sKey,
                                       NULL,
                                       KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

                STL_TRY( knlAcquireLatch( &(sShmManager->mLatch),
                                          KNL_LATCH_MODE_EXCLUSIVE,
                                          KNL_LATCH_PRIORITY_NORMAL,
                                          STL_INFINITE_TIME,
                                          &sIsSuccess,
                                          aEnv ) == STL_SUCCESS );
                sState = 1;

                /**
                 * latch 획득 이후 Validation
                 */
                if( (sElement->mSize == 0) ||                   /** 다른 Process에서 삭제한 경우 */
                    (sElement->mKey == KNS_INVALID_SHM_KEY) ||  /** 다른 Process에서 삭제한 경우 */
                    (gShmAddrTable.mAddr[aIdx] != NULL) )       /** 다른 Thread에서 attach한 경우 */
                {
                    STL_ASSERT( stlDetachShm( &sShm,
                                              KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
                
                    STL_THROW( RAMP_SUCCESS );
                }
            
                if( sElement->mSeq == sValidSeq )
                {
                    gShmAddrTable.mAddr[aIdx] = stlGetShmBaseAddr(&sShm);
                    gShmAddrTable.mSeq[aIdx] = sElement->mSeq;
                    break;
                }
                else
                {
                    /**
                     * Latch를 release한 이후에 segment에 변경이 발생한 경우
                     * - 재시도한다.
                     */
                }
            }
            else
            {
                STL_TRY( stlAttachShm( &sShm,
                                       sElement->mName,
                                       sElement->mKey,
                                       NULL,
                                       KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

                gShmAddrTable.mAddr[aIdx] = stlGetShmBaseAddr(&sShm);
                gShmAddrTable.mSeq[aIdx] = sElement->mSeq;
            }
        }
    }

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_INVALID_STATIC_SHM)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_INVALID_STATIC_SHM,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    if( sState == 0 )
    {
        (void) knlAcquireLatch( &(sShmManager->mLatch),
                                KNL_LATCH_MODE_EXCLUSIVE,
                                KNL_LATCH_PRIORITY_NORMAL,
                                STL_INFINITE_TIME,
                                &sIsSuccess,
                                aEnv );
    }

    return STL_FAILURE;    
}

/**
 * @brief 모든 공유 메모리 세그먼트를 프로세스에 attach 한다.
 * @param[in,out] aEnv 커널 Environment
 * @par Error Codes :
 * @code
 * [KNL_ERRCODE_INVALID_STATIC_SHM]
 *     static shared memory segment가 invalid 하다.
 * [KNL_ERRCODE_INVALID_ARGUMENT]
 *     입력받은 공유 메모리 세그먼트의 ID가 올바르지 않음.
 * @endcode
 * @todo 공유 메모리 관리자에 세그먼트 타입 별로 관리 리스트가 필요하다.
 */
stlStatus knsAttachAllSegment( knlEnv * aEnv )
{
    stlInt32             sIdx;
    knlShmTableElement * sElement;
    knlShmManager      * sShmManager;
    stlBool              sIsSuccess;
    stlInt32             sState = 0;

    STL_TRY_THROW( knsValidateStaticArea() == STL_TRUE,
                   RAMP_ERR_INVALID_STATIC_SHM );

    sShmManager =  gShmAddrTable.mShmManager;

    /**
     * 같은 process에서 동시 attach와 detach를 방어한다.
     */
    STL_TRY( knlAcquireLatch( &(gShmAddrTable.mLatch),
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsSuccess,
                              aEnv ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( knlAcquireLatch( &(sShmManager->mLatch),
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsSuccess,
                              aEnv ) == STL_SUCCESS );
    sState = 2;
    
    for( sIdx = (KNS_ADDR_TABLE_STATIC_AREA + 1);
         sIdx < KNL_SHM_TABLE_ELEMENT_COUNT;
         sIdx++ )
    {
        sElement = &(sShmManager->mSegment[sIdx]);

        if( (sElement->mSize    == 0) ||
            (sElement->mKey     == KNS_INVALID_SHM_KEY) )
        {
            continue;
        }

        STL_TRY( knsAttachSegment( sIdx,
                                   STL_TRUE,  /* aDoLatch */
                                   aEnv ) == STL_SUCCESS );
    }

    sState = 1;
    STL_TRY( knlReleaseLatch( &(sShmManager->mLatch),
                              aEnv ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( knlReleaseLatch( &(gShmAddrTable.mLatch),
                              aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_INVALID_STATIC_SHM)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_INVALID_STATIC_SHM,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void) knlReleaseLatch( &(sShmManager->mLatch), aEnv );
        case 1:
            (void) knlReleaseLatch( &(gShmAddrTable.mLatch), aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;    
}

/**
 * @brief 모든 공유 메모리 세그먼트를 프로세스에 detach 한다.
 * @param[in,out] aEnv 커널 Environment
 * @par Error Codes :
 * @code
 * [KNL_ERRCODE_INVALID_STATIC_SHM]
 *     static shared memory segment가 invalid 하다.
 * [KNL_ERRCODE_INVALID_ARGUMENT]
 *     입력받은 공유 메모리 세그먼트의 ID가 올바르지 않음.
 * @endcode
 * @todo 공유 메모리 관리자에 세그먼트 타입 별로 관리 리스트가 필요하다.
 */
stlStatus knsDetachAllSegment( knlEnv * aEnv )
{
    stlInt32 sIdx;

    STL_TRY_THROW( knsValidateStaticArea() == STL_TRUE,
                   RAMP_ERR_INVALID_STATIC_SHM );

    for( sIdx = (KNS_ADDR_TABLE_STATIC_AREA + 1);
         sIdx < KNL_SHM_TABLE_ELEMENT_COUNT;
         sIdx++ )
    {
        if( gShmAddrTable.mAddr[sIdx] != NULL )
        {
            STL_TRY( knsDetachSegment( sIdx, aEnv ) == STL_SUCCESS );
        }
    }

    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_INVALID_STATIC_SHM)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_INVALID_STATIC_SHM,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    return STL_FAILURE;    
}

/**
 * @brief 공유 메모리 세그먼트를 프로세스에서 detach 한다.
 * @param[in] aIdx detach할 공유 메모리 세그먼트의 ID
 * @param[in,out] aEnv 커널 Environment
 * @par Error Codes :
 * @code
 * [KNL_ERRCODE_INVALID_STATIC_SHM]
 *     static shared memory segment가 invalid 하다.
 * [KNL_ERRCODE_INVALID_ARGUMENT]
 *     입력받은 공유 메모리 세그먼트의 ID가 올바르지 않음.
 * @endcode
 * @todo 공유 메모리 관리자에 세그먼트 타입 별로 관리 리스트가 필요하다.
 */
stlStatus knsDetachSegment(stlUInt16        aIdx,
                           knlEnv         * aEnv)
{
    stlShm        sShm;
    stlBool       sIsSuccess;
    stlInt32      sState = 0;

    STL_TRY_THROW( knsValidateStaticArea() == STL_TRUE,
                   RAMP_ERR_INVALID_STATIC_SHM );

    /**
     * 같은 process에서 동시 attach와 detach를 방어한다.
     */
    STL_TRY( knlAcquireLatch( &(gShmAddrTable.mLatch),
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsSuccess,
                              aEnv ) == STL_SUCCESS );
    sState = 1;

    sShm.mAddr = gShmAddrTable.mAddr[aIdx];
    
    if( sShm.mAddr != NULL )
    {
        STL_ASSERT( stlDetachShm( &sShm,
                                  KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

        gShmAddrTable.mAddr[aIdx] = NULL;
        gShmAddrTable.mSeq[aIdx] = 0;
    }

    sState = 0;
    STL_TRY( knlReleaseLatch( &(gShmAddrTable.mLatch),
                              aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_INVALID_STATIC_SHM)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_INVALID_STATIC_SHM,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    if( sState == 1 )
    {
        (void)knlReleaseLatch( &(gShmAddrTable.mLatch), aEnv );
    }

    return STL_FAILURE;    
}

/**
 * @brief Shared memory segment를 다시 attach 한다.
 * @param[in] aIdx Shared Memory Segment Identifier
 * @param[in,out] aEnv 커널 Environment
 * @par Error Codes :
 * @code
 * [KNL_ERRCODE_INVALID_STATIC_SHM]
 *     static shared memory segment가 invalid 하다.
 * @endcode
 */
stlStatus knsReattachSegment( stlUInt16   aIdx,
                              knlEnv    * aEnv )
{
    stlShm               sShm;
    stlBool              sIsSuccess;
    stlInt32             sState = 0;
    knlShmManager      * sShmManager;
    knlShmTableElement * sElement;

    STL_TRY_THROW( knsValidateStaticArea() == STL_TRUE,
                   RAMP_ERR_INVALID_STATIC_SHM );

    sShmManager = gShmAddrTable.mShmManager;
    sElement = &(sShmManager->mSegment[aIdx]);

    STL_TRY( knlAcquireLatch( &(gShmAddrTable.mLatch),
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsSuccess,
                              aEnv ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( knlAcquireLatch( &(sShmManager->mLatch),
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsSuccess,
                              aEnv ) == STL_SUCCESS );
    sState = 2;

    if( gShmAddrTable.mSeq[aIdx] != sShmManager->mSegment[aIdx].mSeq )
    {
        if( gShmAddrTable.mAddr[aIdx] != NULL )
        {
            sShm.mAddr = gShmAddrTable.mAddr[aIdx];

            stlMemBarrier();
            gShmAddrTable.mAddr[aIdx] = NULL;
            gShmAddrTable.mSeq[aIdx] = 0;
            
            STL_ASSERT( stlDetachShm( &sShm,
                                      KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
        }

        if( (sElement->mSize != 0) &&
            (sElement->mKey != KNS_INVALID_SHM_KEY) )
        {
            STL_TRY( stlAttachShm( &sShm,
                                   sElement->mName,
                                   sElement->mKey,
                                   NULL,
                                   KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
            
            gShmAddrTable.mAddr[aIdx] = stlGetShmBaseAddr(&sShm);
            gShmAddrTable.mSeq[aIdx] = sElement->mSeq;
        }
        else
        {
            STL_DASSERT( STL_FALSE );
        }
    }

    sState = 1;
    STL_TRY( knlReleaseLatch( &(sShmManager->mLatch),
                              aEnv ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( knlReleaseLatch( &(gShmAddrTable.mLatch),
                              aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_INVALID_STATIC_SHM)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_INVALID_STATIC_SHM,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void) knlReleaseLatch( &(sShmManager->mLatch), aEnv );
        case 1:
            (void) knlReleaseLatch( &(gShmAddrTable.mLatch), aEnv );
        default:
            break;
    }

    return STL_FAILURE;    
}

/**
 * @brief 공유 메모리 세그먼트를 destroy 한다.
 * @param[in] aIdx detach할 공유 메모리 세그먼트의 ID
 * @param[in,out] aEnv 커널 Environment
 * @par Error Codes :
 * @code
 * [KNL_ERRCODE_INVALID_STATIC_SHM]
 *     static shared memory segment가 invalid 하다.
 * [KNL_ERRCODE_INVALID_ARGUMENT]
 *     입력받은 공유 메모리 세그먼트의 ID가 올바르지 않음.
 * @endcode
 * @todo 공유 메모리 관리자에 세그먼트 타입 별로 관리 리스트가 필요하다.
 */
stlStatus knsDestroySegment(stlUInt16   aIdx,
                            knlEnv    * aEnv)
{
    knlShmManager      * sShmManager;
    knlShmTableElement * sElement;
    stlShm               sShm;
    stlBool              sIsSuccess;
    stlInt32             sState = 0;

    STL_TRY_THROW( knsValidateStaticArea() == STL_TRUE,
                   RAMP_ERR_INVALID_STATIC_SHM );

    sShmManager =  gShmAddrTable.mShmManager;

    STL_TRY( knlAcquireLatch( &(gShmAddrTable.mLatch),
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsSuccess,
                              aEnv ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( knlAcquireLatch( &(sShmManager->mLatch),
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsSuccess,
                              aEnv ) == STL_SUCCESS );
    sState = 2;

    sElement = &(sShmManager->mSegment[aIdx]);

    STL_TRY_THROW( sElement->mSize != 0, RAMP_ERR_INVALID_INDEX );
    STL_TRY_THROW( sElement->mKey != KNS_INVALID_SHM_KEY, RAMP_ERR_INVALID_INDEX );

    sShm.mKey = sElement->mKey;
    sShm.mAddr = gShmAddrTable.mAddr[aIdx];

    if( sShm.mAddr == NULL )
    {
        STL_TRY( stlAttachShm( &sShm,
                               sElement->mName,
                               sElement->mKey,
                               NULL,
                               KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
    }
        
    STL_TRY( stlDestroyShm( &sShm,
                            KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    gShmAddrTable.mAddr[aIdx] = NULL;
    gShmAddrTable.mSeq[aIdx] = 0;
    
    sElement->mSize = 0;
    sElement->mKey = KNS_INVALID_SHM_KEY;
    sElement->mName[0] = '\0';

    sShmManager->mSegmentCount = sShmManager->mSegmentCount - 1;

    sState = 1;
    STL_TRY( knlReleaseLatch( &(sShmManager->mLatch),
                              aEnv ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( knlReleaseLatch( &(gShmAddrTable.mLatch),
                              aEnv ) == STL_SUCCESS );

    /*
     * 다른 프로세스에게 공유 메모리 세그먼트 삭제를 알린다.
     */
    STL_TRY( knlAddProcessEvent( KNL_EVENT_DETACH_SHM,
                                 (void*)&aIdx,
                                 STL_SIZEOF( stlUInt16 ),
                                 KNL_BROADCAST_PROCESS_ID,
                                 aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_INVALID_STATIC_SHM)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_INVALID_STATIC_SHM,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_CATCH(RAMP_ERR_INVALID_INDEX)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_INVALID_ARGUMENT,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void) knlReleaseLatch( &(sShmManager->mLatch), aEnv );
        case 1:
            (void) knlReleaseLatch( &(gShmAddrTable.mLatch), aEnv );
        default:
            break;
    }

    return STL_FAILURE;    
}

/**
 * @brief 모든 공유 메모리 세그먼트를 destroy 한다.
 * @param[in,out] aEnv 커널 Environment
 * @par Error Codes :
 * @code
 * [KNL_ERRCODE_INVALID_STATIC_SHM]
 *     static shared memory segment가 invalid 하다.
 * @endcode
 */
stlStatus knsDestroyAllSegment( knlEnv * aEnv )
{
    knlShmManager      * sShmManager;
    knlShmTableElement * sElement;
    stlShm               sShm;
    stlUInt32            sIdx;
    stlBool              sIsSuccess;
    stlInt32             sState = 0;

    STL_TRY_THROW( knsValidateStaticArea() == STL_TRUE,
                   RAMP_ERR_INVALID_STATIC_SHM );

    sShmManager =  gShmAddrTable.mShmManager;

    STL_TRY( knlAcquireLatch( &(gShmAddrTable.mLatch),
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsSuccess,
                              aEnv ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( knlAcquireLatch( &(sShmManager->mLatch),
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsSuccess,
                              aEnv ) == STL_SUCCESS );
    sState = 2;
    
    /*
     * static area를 제외한 모든 공유 메모리 세그먼트 destroy
     */
    for( sIdx = (KNS_ADDR_TABLE_STATIC_AREA + 1);
         sIdx < KNL_SHM_TABLE_ELEMENT_COUNT;
         sIdx++ )
    {
        sElement = &(sShmManager->mSegment[sIdx]);

        if( sElement->mKey == KNS_INVALID_SHM_KEY )
        {
            continue;
        }

        sShm.mKey  = sElement->mKey;
        sShm.mAddr = gShmAddrTable.mAddr[sIdx];

        if( sShm.mAddr == NULL )
        {
            (void) stlAttachShm( &sShm,
                                 sElement->mName,
                                 sElement->mKey,
                                 NULL,
                                 KNL_ERROR_STACK( aEnv ) );
        }
        
        (void) stlDestroyShm( &sShm, KNL_ERROR_STACK( aEnv ) );
        gShmAddrTable.mAddr[sIdx] = NULL;
        gShmAddrTable.mSeq[sIdx] = 0;

        sElement->mSize = 0;
        sElement->mKey = KNS_INVALID_SHM_KEY;
        sElement->mName[0] = '\0';
    }

    sState = 1;
    STL_TRY( knlReleaseLatch( &(sShmManager->mLatch),
                              aEnv ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( knlReleaseLatch( &(gShmAddrTable.mLatch),
                              aEnv ) == STL_SUCCESS );
    
    (void) knsDestroyStaticArea( aEnv );

    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_INVALID_STATIC_SHM)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_INVALID_STATIC_SHM,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void) knlReleaseLatch( &(sShmManager->mLatch), aEnv );
        case 1:
            (void) knlReleaseLatch( &(gShmAddrTable.mLatch), aEnv );
        default:
            break;
    }

    return STL_FAILURE;    
}

/**
 * @brief 물리적 주소를 논리적 주소로 변환한다.
 * @param[in] aPysicalAddr 물리적 주소
 * @param[out] aLogicalAddr 변환된 논리적 주소
 * @param[in,out] aEnv 커널 Environment
 * @par Error Codes :
 * @code
 * [KNL_ERRCODE_INVALID_STATIC_SHM]
 *     static shared memory segment가 invalid 하다.
 * [KNL_ERRCODE_INVALID_ARGUMENT]
 *     입력받은 물리적 주소가 올바르지 않음.
 * @endcode
 */
stlStatus knsGetLogicalAddr( void           * aPysicalAddr,
                             knlLogicalAddr * aLogicalAddr,
                             knlEnv         * aEnv )
{
    knlShmManager      * sShmManager;
    knlShmTableElement * sElement;
    void               * sBaseAddr;
    stlUInt32            sIdx;
    stlUInt16            sSegID;
    stlUInt64            sOffset;

    sShmManager =  gShmAddrTable.mShmManager;

    sBaseAddr = gShmAddrTable.mStaticArea.mAddr;
    STL_ASSERT( sBaseAddr != NULL );
    
    /*
     * static area가 생성되기 전에 호출될 수 있으므로
     * shm manager에서 값을 갖고 오지 않는다.
     */
    if( (aPysicalAddr >= sBaseAddr) &&
        (aPysicalAddr < sBaseAddr + gShmStaticSize) )
    {
        sOffset = aPysicalAddr - sBaseAddr;
        *aLogicalAddr = KNL_MAKE_LOGICAL_ADDR( KNS_ADDR_TABLE_STATIC_AREA, sOffset );
    }
    else
    {
        STL_TRY_THROW( knsValidateStaticArea() == STL_TRUE,
                       RAMP_ERR_INVALID_STATIC_SHM );

        for( sIdx = (KNS_ADDR_TABLE_STATIC_AREA + 1);
             sIdx < KNL_SHM_TABLE_ELEMENT_COUNT;
             sIdx++ )
        {
            if( gShmAddrTable.mAddr[sIdx] == NULL )
            {
                continue;
            }

            sElement = &(sShmManager->mSegment[sIdx]);

            if( (aPysicalAddr >= gShmAddrTable.mAddr[sIdx]) &&
                (aPysicalAddr < gShmAddrTable.mAddr[sIdx] + sElement->mSize) )
            {
                break;
            }
        }

        STL_TRY_THROW( sIdx != KNL_SHM_TABLE_ELEMENT_COUNT,
                       RAMP_ERR_INVALID_ARGUMENT );

        sSegID = sIdx;
        sOffset = aPysicalAddr - gShmAddrTable.mAddr[sSegID];

        *aLogicalAddr = KNL_MAKE_LOGICAL_ADDR( sSegID, sOffset );
    }

    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_INVALID_STATIC_SHM)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_INVALID_STATIC_SHM,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_CATCH( RAMP_ERR_INVALID_ARGUMENT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_INVALID_ARGUMENT,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief X$LATCHES에 공유 메모리 관리자의 latch 정보를 출력
 */
knlLatchScanCallback gShmManagerLatchCallback =
{
    knsOpenShmManagerLatch,
    knsGetNextShmManagerLatch,
    knsCloseShmManagerLatch
};

/**
 * @brief X$LATCHES에 공유 메모리 관리자의 latch 정보를 출력을 위한 open 함수
 */
stlStatus knsOpenShmManagerLatch()
{
    return STL_SUCCESS;
}

/**
 * @brief X$LATCHES에 공유 메모리 관리자의 latch 정보를 출력을 위한 close 함수
 */
stlStatus knsCloseShmManagerLatch()
{
    return STL_SUCCESS;
}

/**
 * @brief X$LATCHES에 공유 메모리 관리자의 latch 정보를 출력을 위한 getNext 함수
 * @param[in,out] aLatch 반환될 공유 메모리 관리자의 latch
 * @param[out] aBuf 공유 메모리 관리자의 latch 정보 기술
 * @param[in,out] aPrevPosition 이전에 반환한 latch의 위치 정보
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knsGetNextShmManagerLatch( knlLatch ** aLatch,
                                     stlChar   * aBuf,
                                     void      * aPrevPosition,
                                     knlEnv    * aEnv )
{
    stlUInt32  * sCurSeq = (stlUInt32*)aPrevPosition;

    if( *aLatch == NULL)
    {
        /*
         * getNext()이 처음 호출된 경우
         */
        *sCurSeq = 0;
        *aLatch = &(gShmAddrTable.mShmManager->mLatch);
        stlSnprintf(aBuf,
                    KNL_LATCH_MAX_DESC_BUF_SIZE,
                    "Shared Memory Segment Manager Latch");
    }
    else
    {
        if( *sCurSeq == 1 )
        {
            *aLatch = NULL;
            aBuf[0] = '\0';
        }
        else
        {
            *sCurSeq += 1 ;
            *aLatch = &(gKnEntryPoint->mDynamicMem->mLatch);
            stlSnprintf(aBuf,
                        KNL_LATCH_MAX_DESC_BUF_SIZE,
                        "Variable Static Area Latch");
        }
    }

    return STL_SUCCESS;
}

/**
 * @brief X$SHM_SEGMENT Table 정의
 */
knlFxColumnDesc gShmColumnDesc[] =
{
    {
        "NAME",
        "shared memory segment name",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( knsFxRecord, mName ),
        STL_MAX_SHM_NAME + 1,
        STL_TRUE  /* nullable */
    },
    {
        "ID",
        "shared memory segment identifier",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( knsFxRecord, mId ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "SIZE",
        "shared memory segment size",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( knsFxRecord, mSize ),
        STL_SIZEOF( stlUInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "KEY",
        "shared memory segment key",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( knsFxRecord, mKey ),
        STL_SIZEOF( stlUInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "SEQ",
        "reused sequence",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( knsFxRecord, mSeq ),
        STL_SIZEOF( stlUInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "ADDR",
        "shared memory address",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( knsFxRecord, mAddr ),
        20,
        STL_FALSE  /* nullable */
    },
    {
        NULL,
        NULL,
        0,
        0,
        0,
        STL_FALSE  /* nullable */
    }
};

/**
 * @brief X$SHM_SEGMENT Table의 open 함수
 * @param[in] aStmt Statement Pointer
 * @param[in] aDumpObjHandle Dump Object의 Handle
 * @param[in] aPathCtrl Path Controller
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus openFxShmCallback( void   * aStmt,
                             void   * aDumpObjHandle,
                             void   * aPathCtrl,
                             knlEnv * aEnv )
{
    knsFxPathControl * sCtl = (knsFxPathControl*)aPathCtrl;

    sCtl->mSegmentID = -1;

    return STL_SUCCESS;
}

/**
 * @brief X$SHM_SEGMENT Table의 close 함수
 * @param[in] aDumpObjHandle Dump Object의 Handle
 * @param[in] aPathCtrl Path Controller
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus closeFxShmCallback( void   * aDumpObjHandle,
                              void   * aPathCtrl,
                              knlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief X$SHM_SEGMENT Table의 레코드 구축 함수
 * @param[in] aDumpObjHandle Dump Object Handle
 * @param[in] aPathCtrl Path Controller
 * @param[in,out] aValueList 값을 담을 공간
 * @param[in]  aBlockIdx   Block Idx
 * @param[out] aTupleExist 레코드 존재 여부
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus buildRecordFxShmCallback( void              * aDumpObjHandle,
                                    void              * aPathCtrl,
                                    knlValueBlockList * aValueList,
                                    stlInt32            aBlockIdx,
                                    stlBool           * aTupleExist,
                                    knlEnv            * aEnv )
{
    knsFxPathControl     * sCtl = (knsFxPathControl*)aPathCtrl;
    knsFxRecord          sRec;
    stlInt32             sIdx;
    knlShmManager      * sShmManager;
    knlShmTableElement * sElement;

    *aTupleExist = STL_FALSE;

    sShmManager =  gShmAddrTable.mShmManager;

    for( sIdx = sCtl->mSegmentID + 1; sIdx < KNL_SHM_TABLE_ELEMENT_COUNT; sIdx++ )
    {
        sElement = &(sShmManager->mSegment[sIdx]);

        if( sElement->mKey == KNS_INVALID_SHM_KEY )
        {
            continue;
        }

        stlStrcpy( sRec.mName, sElement->mName );
        sRec.mSize = sElement->mSize;
        sRec.mSeq = sElement->mSeq;
        sRec.mKey  = sElement->mKey;
        sRec.mId   = sIdx;

        stlSnprintf( sRec.mAddr,
                     20,
                     "0x%P",
                     gShmAddrTable.mAddr[sIdx] );

        STL_TRY( knlBuildFxRecord( gShmColumnDesc,
                                   &sRec,
                                   aValueList,
                                   aBlockIdx,
                                   aEnv )
                 == STL_SUCCESS );

        *aTupleExist = STL_TRUE;
        break;
    }

    sCtl->mSegmentID = sIdx;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief X$SHM_SEGMENT Table 정보
 */
knlFxTableDesc gShmTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    NULL,
    openFxShmCallback,
    closeFxShmCallback,
    buildRecordFxShmCallback,
    STL_SIZEOF( knsFxPathControl ),
    "X$SHM_SEGMENT",
    "shared memory segment information fixed table",
    gShmColumnDesc
};

/** @} */
