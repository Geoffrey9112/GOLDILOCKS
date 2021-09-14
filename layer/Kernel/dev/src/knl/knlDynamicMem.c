/*******************************************************************************
 * knlDynamicMem.c
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
 * @file knlDynamicMem.c
 * @brief Kernel Layer Dynamic Memory Management
 */

#include <stl.h>
#include <dtl.h>
#include <knlDef.h>
#include <knlRing.h>
#include <knlLatch.h>
#include <knDef.h>
#include <knmDynamic.h>
#include <knsSegment.h>
#include <knlSession.h>

extern knsEntryPoint    * gKnEntryPoint;
extern knlAllocatorDesc   gKnmAllocatorDesc[];

/**
 * @addtogroup knlDynamicMem
 * @{
 */

knlDynamicMem   gProcDynamicMem;


/**
 * @brief 동적 메모리 할당자를 생성한다.
 * @param[in,out] aDynamicMem   동적 메모리 할당자
 * @param[in]     aParentMem    Parent 동적 메모리 할당자
 * @param[in]     aAllocatorId  메모리 관리자 아이디
 * @param[in]     aStorageType  메모리 저장 공간 타입
 * <PRE>
 *     KNL_MEM_STORAGE_TYPE_HEAP : heap memory
 *     KNL_MEM_STORATE_TYPE_SHM : shared memory
 * </PRE>
 * @param[in]     aInitSize     미리 할당할 메모리의 최소 크기
 * @param[in]     aNextSize     추가적으로 메모리 할당시 할당할 메모리의 크기
 * @param[in]     aController    Memory Controller
 * @param[in,out] aEnv           커널 Environment
 * @par Error Codes :
 * @code
 * [KNL_ERRCODE_UNABLE_EXTEND_MEMORY]
 *     동적 메모리 할당자의 메모리 할당 크기가 0으로 설정되어 있다.
 * @endcode
 * @remark @a aInitSize, @a aNextSize 는 KNL_DEFAULT_CHUNK_SIZE 으로 align 된다.
 * <BR> @a aEnv Shared Memory Env라면 Shared Memory Type을 허용할수 있지만,
 * <BR> @a aEnv가 Shared Memory Env라면 Shared Memory Type을 허용할수 없다.
 * <BR> 강제적으로 Heap Memory를 쓰도록 유도한다.
 */
stlStatus knlCreateDynamicMem( knlDynamicMem    * aDynamicMem,
                               knlDynamicMem    * aParentMem,
                               knlAllocatorId     aAllocatorId,
                               stlUInt32          aStorageType,
                               stlUInt64          aInitSize,
                               stlUInt64          aNextSize,
                               knlMemController * aController,
                               knlEnv           * aEnv )
{
    knmChunkHeader   * sChunkHeader;
    knlAllocatorDesc * sAllocatorDesc;

    STL_PARAM_VALIDATE( aDynamicMem != NULL,
                        KNL_ERROR_STACK( aEnv ));
    STL_PARAM_VALIDATE( ( (aStorageType & KNL_MEM_STORAGE_TYPE_MASK) >= KNL_MEM_STORAGE_TYPE_HEAP ) &&
                        ( (aStorageType & KNL_MEM_STORAGE_TYPE_MASK) < KNL_MEM_STORAGE_TYPE_MAX ),
                        KNL_ERROR_STACK( aEnv ));
    STL_PARAM_VALIDATE( aInitSize > 0,
                        KNL_ERROR_STACK( aEnv ));

    sAllocatorDesc = &gKnmAllocatorDesc[aAllocatorId];

    STL_DASSERT( sAllocatorDesc->mAllocatorId == aAllocatorId );
    STL_DASSERT( aInitSize > sAllocatorDesc->mMinFragSize );
    STL_DASSERT( aNextSize > sAllocatorDesc->mMinFragSize );
    
    /**
     * SSA(shared static area)와 PSA(private static area)의 fragmentation을 막기 위해서
     */
    STL_DASSERT( aNextSize >= KNM_DYNAMIC_SSA_MINIMUM_FRAGMENT_SIZE );
    STL_DASSERT( aNextSize >= KNM_DYNAMIC_PSA_MINIMUM_FRAGMENT_SIZE );



    /**
     * 시스템에서 Shared Memory를 사용할수 있는 준비가 되어있지 않다면
     * 강제적으로 Heap Memory를 쓰도록 유도한다.
     */
    if( knsValidateStaticArea() == STL_TRUE )
    {
        if( (aStorageType & KNL_MEM_STORAGE_TYPE_MASK) == KNL_MEM_STORAGE_TYPE_SHM )
        {
            aDynamicMem->mType = KNL_MEM_STORAGE_TYPE_SHM;
        }
        else
        {
            if( (aStorageType & KNL_MEM_STORAGE_TYPE_MASK) == KNL_MEM_STORAGE_TYPE_PLAN )
            {
                aDynamicMem->mType = KNL_MEM_STORAGE_TYPE_PLAN;
            }
            else
            {
                aDynamicMem->mType = KNL_MEM_STORAGE_TYPE_HEAP;
            }
        }
    }
    else
    {
        aDynamicMem->mType = KNL_MEM_STORAGE_TYPE_HEAP;
    }

    aDynamicMem->mMaxSize = STL_INT64_MAX;
    aDynamicMem->mAttr = sAllocatorDesc->mMemAttr;
    aDynamicMem->mAllocatorId = aAllocatorId;

    if( (sAllocatorDesc->mAllocatorLevel & KNL_ALLOCATOR_LEVEL_MASK) == KNL_ALLOCATOR_LEVEL_ROOT )
    {
        /**
         * Allocator가 Leaf라는 것은 리소스를 제한하고 싶다는 의미로 간주한다.
         */
        
        STL_DASSERT( aDynamicMem->mType == KNL_MEM_STORAGE_TYPE_HEAP );
        
        aDynamicMem->mAllocatorLevel = KNL_ALLOCATOR_LEVEL_ROOT;

        /**
         * 프로퍼티를 인식가능한 경우에만 리소스를 제한한다.
         */
        
        if( (KNL_IS_SHARED_ENV(aEnv) == STL_TRUE) && (knsValidateStaticArea() == STL_TRUE) )
        {
            aDynamicMem->mMaxSize = KNL_MEMORY_TOTAL_SIZE_DEPEND_ON_PROPERTY;
        }
    }
    else
    {
        aDynamicMem->mAllocatorLevel = KNL_ALLOCATOR_LEVEL_INTERNAL;
    }
    
    aDynamicMem->mTotalSize = 0;
    aDynamicMem->mCategory  = sAllocatorDesc->mAllocatorCategory;
    aDynamicMem->mInitSize  = STL_ALIGN( aInitSize, KNL_DEFAULT_CHUNK_SIZE );
    aDynamicMem->mNextSize  = STL_ALIGN( aNextSize, KNL_DEFAULT_CHUNK_SIZE );
    aDynamicMem->mMinFragSize = sAllocatorDesc->mMinFragSize;
    aDynamicMem->mParentMem = aParentMem;

    KNL_CHECK_DYNAMIC_MEMORY_BOUNDARY( aDynamicMem, aEnv );

    STL_TRY( knlInitLatch( &(aDynamicMem->mLatch),
                           ((aDynamicMem->mType == KNL_MEM_STORAGE_TYPE_SHM) ||
                            (aDynamicMem->mType == KNL_MEM_STORAGE_TYPE_PLAN)) ? STL_TRUE : STL_FALSE,
                           aEnv ) == STL_SUCCESS );

    STL_RING_INIT_HEADLINK( &(aDynamicMem->mChunkRing),
                            STL_OFFSETOF(knmChunkHeader, mLink) );

    aDynamicMem->mController = aController;
    
    STL_TRY( knmAllocChunk( aController,
                            aDynamicMem,
                            &sChunkHeader,
                            STL_TRUE,
                            aEnv ) == STL_SUCCESS);

    /**
     * shared memory에 만들어지는 database용 메모리의 모니터링을 위해서
     * dynamic allocator를 시스템에 등록한다.
     * @see X$DYNAMIC_ALLOCATOR
     */
    if( aDynamicMem->mCategory == KNL_MEM_CATEGORY_DATABASE )
    {
        if( (aDynamicMem->mType == KNL_MEM_STORAGE_TYPE_PLAN) ||
            (aDynamicMem->mType == KNL_MEM_STORAGE_TYPE_SHM) )
        {
            STL_TRY( knmRegisterDynamicMem( aDynamicMem,
                                            aEnv )
                     == STL_SUCCESS );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 동적 메모리 할당자를 반환한다.
 * @param[in,out] aDynamicMem 동적 메모리 할당자
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knlDestroyDynamicMem( knlDynamicMem * aDynamicMem,
                                knlEnv        * aEnv )
{
    STL_PARAM_VALIDATE( aDynamicMem != NULL,
                        KNL_ERROR_STACK( aEnv ));
    STL_PARAM_VALIDATE( ( aDynamicMem->mType >= KNL_MEM_STORAGE_TYPE_HEAP ) &&
                        ( aDynamicMem->mType < KNL_MEM_STORAGE_TYPE_MAX ),
                        KNL_ERROR_STACK( aEnv ));

    KNL_CHECK_DYNAMIC_MEMORY_BOUNDARY( aDynamicMem, aEnv );

    STL_TRY( knmFreeAllChunk( aDynamicMem->mController,
                              aDynamicMem,
                              aEnv ) == STL_SUCCESS );

    /**
     * System에 등록된 dynamic allocator를 등록해제한다.
     */
    if( aDynamicMem->mCategory == KNL_MEM_CATEGORY_DATABASE )
    {
        if( (aDynamicMem->mType == KNL_MEM_STORAGE_TYPE_PLAN) ||
            (aDynamicMem->mType == KNL_MEM_STORAGE_TYPE_SHM) )
        {
            STL_TRY( knmUnregisterDynamicMem( aDynamicMem,
                                              aEnv )
                     == STL_SUCCESS );
        }
    }
            
    aDynamicMem->mType      = KNL_MEM_STORAGE_TYPE_MAX;
    aDynamicMem->mCategory  = KNL_MEM_CATEGORY_MAX;    
    aDynamicMem->mInitSize  = 0;
    aDynamicMem->mNextSize  = 0;
    aDynamicMem->mMaxSize   = 0;
    aDynamicMem->mTotalSize = 0;

    knlFinLatch( &(aDynamicMem->mLatch) );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 동적 메모리 할당자를 검증한다.
 * @param[in,out] aDynamicMem 동적 메모리 할당자
 * @param[in,out] aEnv 커널 Environment
 */
void knlValidateDynamicMem( knlDynamicMem * aDynamicMem,
                            knlEnv        * aEnv )
{
#ifdef STL_DEBUG
    knmChunkHeader * sChunkHeader;
    knmBlockHeader * sBlockHeader;
    
    STL_RING_FOREACH_ENTRY( &(aDynamicMem->mChunkRing), sChunkHeader )
    {
        STL_RING_FOREACH_ENTRY( &(sChunkHeader->mAllocRing), sBlockHeader )
        {
            if( sBlockHeader->mIsUsed == STL_TRUE )
            {
                if( KNM_CHECK_MEMORY_BOUNDARY(NULL) == STL_TRUE )
                {
                    stlFile sOutFile;
                
                    stlOpenStdoutFile( &sOutFile, NULL );
                    stlBacktraceSymbolsToFile( ((knmHeadFence*)(sBlockHeader + 1))->mCallstack,
                                               ((knmHeadFence*)(sBlockHeader + 1))->mCallstackCount,
                                               &sOutFile );
                }

                KNL_SYSTEM_FATAL_DASSERT( 0, aEnv );
            }
        }
    }
#endif
}

/**
 * @brief 동적 메모리를 할당한다.
 * @param[in,out] aDynamicMem 동적 메모리 할당자
 * @param[in] aSize 할당할 메모리 크기
 * @param[in] aAddr 할당된 메모리 주소
 * @param[in,out] aEnv 커널 Environment
 * @remark @a aSize 는 8byte align 되어 메모리가 할당된다.
 */
stlStatus knlAllocDynamicMem( knlDynamicMem  * aDynamicMem,
                              stlUInt32        aSize,
                              void          ** aAddr,
                              knlEnv         * aEnv )
{
    STL_PARAM_VALIDATE( aDynamicMem != NULL,
                        KNL_ERROR_STACK( aEnv ));
    STL_PARAM_VALIDATE( ( aDynamicMem->mType >= KNL_MEM_STORAGE_TYPE_HEAP ) &&
                        ( aDynamicMem->mType < KNL_MEM_STORAGE_TYPE_MAX ),
                        KNL_ERROR_STACK( aEnv ));
    STL_PARAM_VALIDATE( aSize != 0,
                        KNL_ERROR_STACK( aEnv ));

    KNL_CHECK_DYNAMIC_MEMORY_BOUNDARY( aDynamicMem, aEnv );

    aSize = STL_MAX( aDynamicMem->mMinFragSize, aSize );
    
    STL_TRY( knmDAlloc( aDynamicMem->mController,
                        aDynamicMem,
                        NULL,  /* aRequestAllocator */
                        KNL_ALLOCATOR_TYPE_NONE,
                        STL_ALIGN_DEFAULT(aSize),
                        aAddr,
                        aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 동적 메모리를 cache align에 맞는 주소에 할당한다.
 * @param[in,out] aDynamicMem 동적 메모리 할당자
 * @param[in] aSize 할당할 메모리 크기
 * @param[in] aAddr 할당된 메모리 주소
 * @param[in,out] aEnv 커널 Environment
 * @remark @a aSize 는 8byte align 되어 메모리가 할당된다.
 */
stlStatus knlCacheAlignedAllocDynamicMem( knlDynamicMem  * aDynamicMem,
                                          stlUInt32        aSize,
                                          void          ** aAddr,
                                          knlEnv         * aEnv )
{
    STL_PARAM_VALIDATE( aDynamicMem != NULL,
                        KNL_ERROR_STACK( aEnv ));
    STL_PARAM_VALIDATE( ( aDynamicMem->mType >= KNL_MEM_STORAGE_TYPE_HEAP ) &&
                        ( aDynamicMem->mType < KNL_MEM_STORAGE_TYPE_MAX ),
                        KNL_ERROR_STACK( aEnv ));
    STL_PARAM_VALIDATE( aSize != 0,
                        KNL_ERROR_STACK( aEnv ));

    KNL_CHECK_DYNAMIC_MEMORY_BOUNDARY( aDynamicMem, aEnv );

    aSize = STL_MAX( aDynamicMem->mMinFragSize, aSize );
    
    STL_TRY( knmCacheAlignedDAlloc( aDynamicMem->mController,
                                    aDynamicMem,
                                    NULL,   /* aRequestAllocator */
                                    KNL_ALLOCATOR_TYPE_NONE,
                                    STL_ALIGN_DEFAULT(aSize),
                                    aAddr,
                                    aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 동적 메모리를 반환한다.
 * @param[in,out] aDynamicMem 동적 메모리 할당자
 * @param[in] aAddr 할당할 메모리 주소
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knlFreeDynamicMem( knlDynamicMem * aDynamicMem,
                             void          * aAddr,
                             knlEnv        * aEnv )
{
    STL_PARAM_VALIDATE( aDynamicMem != NULL,
                        KNL_ERROR_STACK( aEnv ));
    STL_PARAM_VALIDATE( ( aDynamicMem->mType >= KNL_MEM_STORAGE_TYPE_HEAP ) &&
                        ( aDynamicMem->mType < KNL_MEM_STORAGE_TYPE_MAX ),
                        KNL_ERROR_STACK( aEnv ));
    STL_PARAM_VALIDATE( aAddr != NULL,
                        KNL_ERROR_STACK( aEnv ));

    KNL_CHECK_DYNAMIC_MEMORY_BOUNDARY( aDynamicMem, aEnv );

    STL_TRY( knmDFree( aDynamicMem->mController,
                       aDynamicMem,
                       aAddr,
                       aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief cache align에 맞게 할당된 동적 메모리를 반환한다.
 * @param[in,out] aDynamicMem 동적 메모리 할당자
 * @param[in] aAddr 할당할 메모리 주소
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knlCacheAlignedFreeDynamicMem( knlDynamicMem * aDynamicMem,
                                         void          * aAddr,
                                         knlEnv        * aEnv )
{
    STL_PARAM_VALIDATE( aDynamicMem != NULL,
                        KNL_ERROR_STACK( aEnv ));
    STL_PARAM_VALIDATE( ( aDynamicMem->mType >= KNL_MEM_STORAGE_TYPE_HEAP ) &&
                        ( aDynamicMem->mType < KNL_MEM_STORAGE_TYPE_MAX ),
                        KNL_ERROR_STACK( aEnv ));
    STL_PARAM_VALIDATE( aAddr != NULL,
                        KNL_ERROR_STACK( aEnv ));

    KNL_CHECK_DYNAMIC_MEMORY_BOUNDARY( aDynamicMem, aEnv );

    STL_TRY( knmCacheAlignedDFree( aDynamicMem->mController,
                                   aDynamicMem,
                                   aAddr,
                                   aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Arena로 연결된 모든 메모리를 Dynamic Memory 관리자에서 삭제한다.
 * @param[in]     aFirstArena    First Arena Address
 * @param[in]     aDynamicMem    Dynamic Memory Mananger
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knlFreeAllArena( void          * aFirstArena,
                           knlDynamicMem * aDynamicMem,
                           knlEnv        * aEnv )
{
    knmArenaHeader * sArena;
    knmArenaHeader * sNextArena;

    sArena = (knmArenaHeader*)aFirstArena;

    while( sArena != NULL )
    {
        sNextArena = (knmArenaHeader*)(sArena->mLink.mNext);
            
        STL_TRY( knlFreeDynamicMem( aDynamicMem,
                                    sArena->mAddr,
                                    aEnv )
                 == STL_SUCCESS );

        sArena = sNextArena;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 프로세스 내에서 공용으로 사용할 동적 메모리 할당자를 생성한다.
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knlCreateProcDynamicMem( knlEnv * aEnv )
{
    return knlCreateDynamicMem( &gProcDynamicMem,
                                NULL, /* aParentMem */
                                KNL_ALLOCATOR_KERNEL_OPERERATION,
                                KNL_MEM_STORAGE_TYPE_HEAP,
                                1024 * 1024,
                                1024 * 1024,
                                NULL, /* aController */
                                aEnv );
}

/**
 * @brief 프로세스 내에서 공용으로 사용하던 동적 메모리 할당자를 해제한다.
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knlDestroyProcDynamicMem( knlEnv * aEnv )
{
    return knlDestroyDynamicMem( &gProcDynamicMem, aEnv );
}


/**
 * @brief 프로세스 내에서 공용으로 사용하던 동적 메모리 할당자를 반환한다.
 */
inline knlDynamicMem * knlGetProcDynamicMem( )
{
    return &gProcDynamicMem;
}


/**
 * @brief 생성된 동적 메모리 정보를 출력한다.
 * @param[in,out] aDynamicMem 동적 메모리 할당자
 * @internal
 */
void knlTestPrintAllocDynamicMem(knlDynamicMem * aDynamicMem)
{
    knmTestPrintAllocList( aDynamicMem );
}

/**
 * @brief 반환된 동적 메모리 정보를 출력한다.
 * @param[in,out] aDynamicMem 동적 메모리 할당자
 * @internal
 */
void knlTestPrintFreeDynamicMem(knlDynamicMem * aDynamicMem)
{
    knmTestPrintFreeList( aDynamicMem );
}


/** @} */
