/*******************************************************************************
 * knlRegionMem.c
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
 * @file knlRegionMem.c
 * @brief Kernel Layer Dynamic Memory Management
 */

#include <stl.h>
#include <dtl.h>
#include <knlDef.h>
#include <knlRing.h>
#include <knlLatch.h>
#include <knDef.h>
#include <knmRegion.h>
#include <knlRegionMem.h>
#include <knlProperty.h>
#include <knsSegment.h>
#include <knlSession.h>
#include <knpc.h>

extern knsEntryPoint    * gKnEntryPoint;
extern knlAllocatorDesc   gKnmAllocatorDesc[];

/**
 * @addtogroup knlRegionMem
 * @{
 */

/**
 * @brief 영역 기반 메모리 할당자를 생성한다.
 * @param[in,out] aRegionMem   영역 기반 메모리 할당자
 * @param[in]     aAllocatorId 메모리 관리자 아이디
 * @param[in]     aParentMem   Parent Dynamic Memory Manager
 * @param[in]     aStorageType 메모리 저장 공간 타입
 * <PRE>
 *     KNL_MEM_STORAGE_TYPE_HEAP : heap memory
 *     KNL_MEM_STORATE_TYPE_SHM : shared memory
 * </PRE>
 * @param[in]     aInitSize    미리 할당할 메모리의 최소 크기
 * @param[in]     aNextSize    추가적으로 메모리 할당시 할당할 메모리의 크기
 * @param[in,out] aEnv         커널 Environment
 * @par Error Codes :
 * @code
 * [KNL_ERRCODE_UNABLE_EXTEND_MEMORY]
 *     영역 기반 메모리 할당자의 메모리 할당 크기가 0으로 설정되어 있다.
 * @endcode
 * @see @a aType knlMemType
 * @remark 동시성 제어를 하지 않는다.
 *         @a aInitSize, @a aNextSize 는 KNL_DEFAULT_REGION_SIZE 으로 align 된다.
 * <BR> @a aEnv Shared Memory Env라면 Shared Memory Type을 허용할수 있지만,
 * <BR> @a aEnv가 Shared Memory Env라면 Shared Memory Type을 허용할수 없다.
 * <BR> 강제적으로 Heap Memory를 쓰도록 유도한다.
 */
stlStatus knlCreateRegionMem( knlRegionMem   * aRegionMem,
                              knlAllocatorId   aAllocatorId,
                              knlDynamicMem  * aParentMem,
                              stlUInt32        aStorageType,
                              stlUInt64        aInitSize,
                              stlUInt64        aNextSize,
                              knlEnv         * aEnv )
{
    knmArenaHeader   * sArenaHeader;
    knlAllocatorDesc * sAllocatorDesc;

    STL_PARAM_VALIDATE( aRegionMem != NULL,
                        KNL_ERROR_STACK( aEnv ));
    STL_PARAM_VALIDATE( ( (aStorageType & KNL_MEM_STORAGE_TYPE_MASK) >= KNL_MEM_STORAGE_TYPE_HEAP ) &&
                        ( (aStorageType & KNL_MEM_STORAGE_TYPE_MASK) < KNL_MEM_STORAGE_TYPE_MAX ),
                        KNL_ERROR_STACK( aEnv ));

    sAllocatorDesc = &gKnmAllocatorDesc[aAllocatorId];

    /**
     * 시스템에서 Shared Memory를 사용할수 있는 준비가 되어있지 않다면
     * 강제적으로 Heap Memory를 쓰도록 유도한다.
     */
    if( knsValidateStaticArea() == STL_TRUE )
    {
        if( (aStorageType & KNL_MEM_STORAGE_TYPE_MASK) == KNL_MEM_STORAGE_TYPE_SHM )
        {
            aRegionMem->mMemType = KNL_MEM_STORAGE_TYPE_SHM;
        }
        else
        {
            /**
             * Region Memory은 Plan Type을 지원하지 않는다.
             */
            STL_DASSERT( (aStorageType & KNL_MEM_STORAGE_TYPE_MASK) == KNL_MEM_STORAGE_TYPE_HEAP );
            aRegionMem->mMemType = KNL_MEM_STORAGE_TYPE_HEAP;
        }
    }
    else
    {
        aRegionMem->mMemType = KNL_MEM_STORAGE_TYPE_HEAP;
    }

    /**
     * SSA(shared static area)와 PSA(private static area)의 fragmentation을 막기 위해서
     */
    STL_DASSERT( aNextSize >= KNM_DYNAMIC_SSA_MINIMUM_FRAGMENT_SIZE );
    STL_DASSERT( aNextSize >= KNM_DYNAMIC_PSA_MINIMUM_FRAGMENT_SIZE );

    aRegionMem->mAllocatorId = aAllocatorId;
    aRegionMem->mTermSeqType = sAllocatorDesc->mUnmarkType;
    aRegionMem->mEnableAlloc = STL_TRUE;
    aRegionMem->mTotalSize = 0;
    aRegionMem->mParentMem = aParentMem;
    aRegionMem->mCategory  = sAllocatorDesc->mAllocatorCategory;
    aRegionMem->mInitSize  = STL_ALIGN( aInitSize, KNL_DEFAULT_REGION_SIZE );
    aRegionMem->mNextSize  = STL_ALIGN( aNextSize, KNL_DEFAULT_REGION_SIZE );

    KNL_CHECK_REGION_MEMORY_BOUNDARY( aRegionMem, aEnv );

    STL_RING_INIT_HEADLINK( &(aRegionMem->mAllocRing),
                            STL_OFFSETOF(knmArenaHeader, mLink) );

    STL_RING_INIT_HEADLINK( &(aRegionMem->mFreeRing),
                            STL_OFFSETOF(knmArenaHeader, mLink) );

    if( aInitSize > 0 )
    {
        STL_TRY( knmAllocArena( aRegionMem,
                                &sArenaHeader,
                                STL_TRUE,
                                aEnv ) == STL_SUCCESS );
    }

    STL_RING_INIT_HEADLINK( &(aRegionMem->mMarkList),
                            STL_OFFSETOF(knlRegionMarkEx, mMarkLink) );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 현재까지 사용한 영역 기반 메모리의 정보를 저장한다.
 * @param[in,out] aRegionMem 영역 기반 메모리 할당자
 * @param[out] aMark 현재까지 사용한 영역 기반 메모리의 정보
 * @param[in,out] aEnv 커널 Environment
 * @remark 동시성 제어를 하지 않는다.
 * @note RegionMem의 Termination Sequence가 ORDERED여야만 한다.
 */
stlStatus knlMarkRegionMem( knlRegionMem  * aRegionMem,
                            knlRegionMark * aMark,
                            knlEnv        * aEnv )
{
    STL_PARAM_VALIDATE( aRegionMem != NULL,
                        KNL_ERROR_STACK( aEnv ));
    STL_PARAM_VALIDATE( ( aRegionMem->mMemType >= KNL_MEM_STORAGE_TYPE_HEAP ) &&
                        ( aRegionMem->mMemType < KNL_MEM_STORAGE_TYPE_MAX ),
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aRegionMem->mTermSeqType == KNL_UNMARK_SEQ_ORDERED,
                        KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aMark->mArena == NULL,
                        KNL_ERROR_STACK( aEnv ));

    KNL_CHECK_REGION_MEMORY_BOUNDARY( aRegionMem, aEnv );

    STL_DASSERT( STL_RING_IS_EMPTY( &aRegionMem->mMarkList ) == STL_TRUE );

    STL_TRY( knmRMark( aRegionMem,
                       aMark,
                       aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 현재까지 사용한 영역 기반 메모리의 정보를 저장한다.
 * @param[in,out] aRegionMem 영역 기반 메모리 할당자
 * @param[out]    aRegionMarkEx 현재까지 사용한 영역 기반 메모리의 정보
 * @param[in,out] aEnv 커널 Environment
 * @remark 동시성 제어를 하지 않는다.
 * @note RegionMem의 Termination Sequence가 RANDOM이여야만 한다.
 */
stlStatus knlMarkRegionMemEx( knlRegionMem     * aRegionMem,
                              knlRegionMarkEx ** aRegionMarkEx,
                              knlEnv           * aEnv )
{
    knlRegionMark sRegionMark;

    STL_PARAM_VALIDATE( aRegionMem != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem->mTermSeqType == KNL_UNMARK_SEQ_RANDOM,
                        KNL_ERROR_STACK(aEnv) );
    
    KNL_CHECK_REGION_MEMORY_BOUNDARY( aRegionMem, aEnv );

    STL_TRY( knmRMark( aRegionMem,
                       &sRegionMark,
                       aEnv ) == STL_SUCCESS );

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( knlRegionMarkEx ),
                                (void**)aRegionMarkEx,
                                aEnv )
             == STL_SUCCESS );

    STL_RING_INIT_DATALINK( *aRegionMarkEx,
                            STL_OFFSETOF( knlRegionMarkEx, mMarkLink ) );
    STL_RING_ADD_LAST( &aRegionMem->mMarkList, *aRegionMarkEx );
    
    (*aRegionMarkEx)->mRegionMark = sRegionMark;
    (*aRegionMarkEx)->mIsFree = STL_FALSE;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 영역 기반 메모리를 할당한다.
 * @param[in,out] aRegionMem 영역 기반 메모리 할당자
 * @param[in] aSize 할당할 메모리 크기
 * @param[in] aAddr 할당된 메모리 주소
 * @param[in,out] aEnv 커널 Environment
 * @remark 동시성 제어를 하지 않는다.
 *         @a aSize 는 8byte align 되어 메모리가 할당된다.
 */
stlStatus knlAllocRegionMem( knlRegionMem * aRegionMem,
                             stlUInt32      aSize,
                             void        ** aAddr,
                             knlEnv       * aEnv )
{
    stlBool     sIsSuccess;
    knpcClock * sClock;
    
    STL_PARAM_VALIDATE( aRegionMem != NULL,
                        KNL_ERROR_STACK( aEnv ));
    STL_PARAM_VALIDATE( ( aRegionMem->mMemType >= KNL_MEM_STORAGE_TYPE_HEAP ) &&
                        ( aRegionMem->mMemType < KNL_MEM_STORAGE_TYPE_MAX ),
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aSize != 0,
                        KNL_ERROR_STACK( aEnv ));

    KNL_CHECK_REGION_MEMORY_BOUNDARY( aRegionMem, aEnv );

    /**
     * 해당 메모리 관리자가 사용 가능한 상태여야 한다.
     */
            
    STL_DASSERT( aRegionMem->mEnableAlloc == STL_TRUE );
            
    if( aRegionMem->mParentMem == NULL )
    {
        STL_TRY( knmRAlloc( aRegionMem,
                            STL_ALIGN_DEFAULT(aSize),
                            aAddr,
                            aEnv ) == STL_SUCCESS );
    }
    else
    {
        if( aRegionMem->mParentMem->mType == KNL_MEM_STORAGE_TYPE_PLAN )
        {
            /**
             * Parent가 Plan Cache Allocator라면
             */

            if( knmRAlloc( aRegionMem,
                           STL_ALIGN_DEFAULT(aSize),
                           aAddr,
                           aEnv ) == STL_FAILURE )
            {
                stlPopError( KNL_ERROR_STACK(aEnv) );
        
                STL_TRY( knpcGetClock( &sClock, aEnv ) == STL_SUCCESS );
                
                STL_TRY( knpcAgingClock( sClock,
                                         KNPC_DEAD_AGE,
                                         KNPC_CLOCK_AGING_THRESHOLD,
                                         knlGetPropertyBigIntValueByID( KNL_PROPERTY_AGING_PLAN_INTERVAL,
                                                                        aEnv ),
                                         &sIsSuccess,
                                         aEnv )
                         == STL_SUCCESS );

                STL_TRY_THROW( sIsSuccess == STL_TRUE, RAMP_ERR_RECOMPILE );

                if( knmRAlloc( aRegionMem,
                               STL_ALIGN_DEFAULT(aSize),
                               aAddr,
                               aEnv ) == STL_FAILURE )
                {
                    stlPopError( KNL_ERROR_STACK(aEnv) );

                    STL_THROW( RAMP_ERR_RECOMPILE );
                }
            }
        }
        else
        {
            STL_TRY( knmRAlloc( aRegionMem,
                                STL_ALIGN_DEFAULT(aSize),
                                aAddr,
                                aEnv ) == STL_SUCCESS );
        }
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_RECOMPILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_INSUFFICIENT_PLAN_CACHE_MEMORY,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 생성된 모든 영역 기반 메모리를 정리한다.
 * @param[in,out] aRegionMem 영역 기반 메모리 할당자
 * @param[in,out] aEnv 커널 Environment
 * @remark 동시성 제어를 하지 않는다.
 */
stlStatus knlClearRegionMem( knlRegionMem * aRegionMem,
                             knlEnv       * aEnv )
{
    STL_PARAM_VALIDATE( aRegionMem != NULL,
                        KNL_ERROR_STACK( aEnv ));
    STL_PARAM_VALIDATE( ( aRegionMem->mMemType >= KNL_MEM_STORAGE_TYPE_HEAP ) &&
                        ( aRegionMem->mMemType < KNL_MEM_STORAGE_TYPE_MAX ),
                        KNL_ERROR_STACK( aEnv ) );

    KNL_CHECK_REGION_MEMORY_BOUNDARY( aRegionMem, aEnv );

    STL_DASSERT( STL_RING_IS_EMPTY( &(aRegionMem->mMarkList) ) == STL_TRUE );
        
    STL_RING_INIT_HEADLINK( &(aRegionMem->mMarkList),
                            STL_OFFSETOF(knlRegionMarkEx, mMarkLink) );
    
    STL_TRY( knmRClear( aRegionMem,
                        aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 영역 기반 메모리를 이전 상태로 되돌린다.
 * @param[in,out] aRegionMem 영역 기반 메모리 할당자
 * @param[in] aMark 이전의 영역 기반 메모리의 정보
 * @param[in] aFreeChunk 반납하는 Chunk를 삭제할지 여부
 * @param[in,out] aEnv 커널 Environment
 * @remark 동시성 제어를 하지 않는다.
 *         @a aMark 이후에 생성된 메모리는 시스템에 반환되지 않고 유지한다.
 * @note RegionMem의 Termination Sequence가 ORDERED여야만 한다.
 */
stlStatus knlFreeUnmarkedRegionMem( knlRegionMem  * aRegionMem,
                                    knlRegionMark * aMark,
                                    stlBool         aFreeChunk,
                                    knlEnv        * aEnv )
{
    STL_PARAM_VALIDATE( aRegionMem != NULL,
                        KNL_ERROR_STACK( aEnv ));
    STL_PARAM_VALIDATE( ( aRegionMem->mMemType >= KNL_MEM_STORAGE_TYPE_HEAP ) &&
                        ( aRegionMem->mMemType < KNL_MEM_STORAGE_TYPE_MAX ),
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aRegionMem->mTermSeqType == KNL_UNMARK_SEQ_ORDERED,
                        KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aMark != NULL,
                        KNL_ERROR_STACK( aEnv ));
    STL_PARAM_VALIDATE( KNL_IS_VALID_REGION_MARK( aMark ),
                        KNL_ERROR_STACK( aEnv ));
    
    KNL_CHECK_REGION_MEMORY_BOUNDARY( aRegionMem, aEnv );

    STL_TRY( knmRReset( aRegionMem,
                        aMark,
                        aFreeChunk,
                        aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 영역 기반 메모리를 이전 상태로 되돌린다.
 * @param[in,out] aRegionMem    영역 기반 메모리 할당자
 * @param[in]     aRegionMarkEx 이전의 영역 기반 메모리의 정보
 * @param[in,out] aEnv          커널 Environment
 * @remark 동시성 제어를 하지 않는다.
 *         @a aMark 이후에 생성된 메모리는 시스템에 반환되지 않고 유지한다.
 * @note RegionMem의 Termination Sequence가 RANDOM이여야만 한다.
 */
stlStatus knlFreeUnmarkedRegionMemEx( knlRegionMem    * aRegionMem,
                                      knlRegionMarkEx * aRegionMarkEx,
                                      knlEnv          * aEnv )
{
    knlRegionMarkEx * sRegionMark = NULL;
    knlRegionMarkEx * sPrvRegionMark = NULL;
    knlRegionMarkEx * sLstRegionMark = NULL;

    STL_PARAM_VALIDATE( aRegionMem != NULL, KNL_ERROR_STACK( aEnv ));
    STL_PARAM_VALIDATE( aRegionMem->mTermSeqType == KNL_UNMARK_SEQ_RANDOM,
                        KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMarkEx != NULL, KNL_ERROR_STACK( aEnv ));
    
    KNL_CHECK_REGION_MEMORY_BOUNDARY( aRegionMem, aEnv );

#ifdef STL_DEBUG

    stlBool sFound = STL_FALSE;
    
    STL_RING_FOREACH_ENTRY( &aRegionMem->mMarkList, sRegionMark )
    {
        if( sRegionMark == aRegionMarkEx )
        {
            sFound = STL_TRUE;
            break;
        }
    }

    STL_DASSERT( sFound == STL_TRUE );
    
#endif

    aRegionMarkEx->mIsFree = STL_TRUE;
    
    if( STL_RING_GET_LAST_DATA( &aRegionMem->mMarkList ) == aRegionMarkEx )
    {
        STL_RING_FOREACH_ENTRY_REVERSE_SAFE( &aRegionMem->mMarkList,
                                             sRegionMark,
                                             sPrvRegionMark )
        {
            if( sRegionMark->mIsFree == STL_TRUE )
            {
                STL_RING_UNLINK( &aRegionMem->mMarkList, sRegionMark );
                sLstRegionMark = sRegionMark;
            }
            else
            {
                break;
            }
        }
    }

    if( sLstRegionMark != NULL )
    {
        STL_TRY( knmRReset( aRegionMem,
                            &sLstRegionMark->mRegionMark,
                            STL_FALSE, /* aFreeChunk */
                            aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 생성된 모든 영역 기반 메모리를 반환한다.
 * @param[in,out] aRegionMem 영역 기반 메모리 할당자
 * @param[in,out] aEnv 커널 Environment
 * @remark 동시성 제어를 하지 않는다.
 */
stlStatus knlDestroyRegionMem( knlRegionMem * aRegionMem,
                               knlEnv       * aEnv )
{
    STL_PARAM_VALIDATE( aRegionMem != NULL,
                        KNL_ERROR_STACK( aEnv ));
    STL_PARAM_VALIDATE( ( aRegionMem->mMemType >= KNL_MEM_STORAGE_TYPE_HEAP ) &&
                        ( aRegionMem->mMemType < KNL_MEM_STORAGE_TYPE_MAX ),
                        KNL_ERROR_STACK( aEnv ) );

    KNL_CHECK_REGION_MEMORY_BOUNDARY( aRegionMem, aEnv );

    STL_TRY( knmFreeAllArena( aRegionMem,
                              aEnv ) == STL_SUCCESS );

    aRegionMem->mMemType     = KNL_MEM_STORAGE_TYPE_MAX;
    aRegionMem->mTermSeqType = KNL_UNMARK_SEQ_MAX;
    aRegionMem->mCategory    = KNL_MEM_CATEGORY_MAX;
    aRegionMem->mInitSize    = 0;
    aRegionMem->mNextSize    = 0;
    aRegionMem->mTotalSize   = 0;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 모든 Region Memory를 가져간다.
 * @param[in]     aRegionMem   영역 기반 메모리 할당자
 * @param[in]     aFirstArena  First Arena Address
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knlStealRegionMem( knlRegionMem  * aRegionMem,
                             void         ** aFirstArena,
                             knlEnv        * aEnv )
{
    stlInt64         sStealSize = 0;
    knmArenaHeader * sArenaHeader;
    
    STL_PARAM_VALIDATE( aRegionMem != NULL,
                        KNL_ERROR_STACK( aEnv ));
    STL_PARAM_VALIDATE( ( aRegionMem->mMemType >= KNL_MEM_STORAGE_TYPE_HEAP ) &&
                        ( aRegionMem->mMemType < KNL_MEM_STORAGE_TYPE_MAX ),
                        KNL_ERROR_STACK( aEnv ) );

    KNL_CHECK_REGION_MEMORY_BOUNDARY( aRegionMem, aEnv );

    STL_RING_FOREACH_ENTRY( &(aRegionMem->mAllocRing),
                            sArenaHeader )
    {
        sStealSize += ( sArenaHeader->mSize +
                        STL_SIZEOF(knmArenaHeader) +
                        gKnEntryPoint->mCacheAlignedSize );
    }

    if( aFirstArena != NULL )
    {
        *aFirstArena = knlGetFirstArena( aRegionMem );
    }
    
    STL_RING_INIT_HEADLINK( &(aRegionMem->mAllocRing),
                            STL_OFFSETOF(knmArenaHeader, mLink) );

    aRegionMem->mTotalSize -= sStealSize;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Arena로 연결된 모든 메모리의 크기를 반환한다.
 * @param[in]   aFirstArena    First Arena Address
 */
stlInt64 knlGetArenaSize( void * aFirstArena )
{
    stlInt64         sArenaSize = 0;
    knmArenaHeader * sArena;
    
    sArena = (knmArenaHeader*)aFirstArena;

    while( sArena != NULL )
    {
        sArenaSize += sArena->mSize;
        
        sArena = (knmArenaHeader*)(sArena->mLink.mNext);
    }

    return sArenaSize;
}


/**
 * @brief 영역 관리자의 Parent의 Memory Type을 반환한다.
 * @param[in] aRegionMem 영역 기반 메모리 할당자
 */
stlUInt32 knlGetParentMemType( knlRegionMem * aRegionMem )
{
    stlUInt32 sMemType = KNL_MEM_STORAGE_TYPE_NONE;
    
    if( aRegionMem->mParentMem != NULL )
    {
        sMemType = aRegionMem->mParentMem->mType;
    }

    return sMemType;
}
                               
/**
 * @brief 영역 관리자의 첫번째 Arena를 반환한다.
 * @param[in] aRegionMem 영역 기반 메모리 할당자
 */
void * knlGetFirstArena( knlRegionMem * aRegionMem )
{
    return STL_RING_GET_FIRST_DATA( &(aRegionMem->mAllocRing) );
}

/**
 * @brief Region Memory 사용을 허용한다.
 * @param[in]     aRegionMem   영역 기반 메모리 할당자
 */
void knlEnableAllocation( knlRegionMem * aRegionMem )
{
    aRegionMem->mEnableAlloc = STL_TRUE;
}


/**
 * @brief Region Memory 사용을 금지한다.
 * @param[in]     aRegionMem   영역 기반 메모리 할당자
 */
void knlDisableAllocation( knlRegionMem * aRegionMem )
{
    aRegionMem->mEnableAlloc = STL_FALSE;
}


/**
 * @brief Region Memory 허용여부를 반환한다.
 * @param[in]     aRegionMem   영역 기반 메모리 할당자
 */
stlBool knlGetEnableAllocation( knlRegionMem * aRegionMem )
{
    return aRegionMem->mEnableAlloc;
}


/**
 * @brief Region Memory 허용여부를 설정한다.
 * @param[in]     aRegionMem   영역 기반 메모리 할당자
 * @param[in]     aEnableAlloc 할당 허용 여부
 */
void knlSetEnableAllocation( knlRegionMem * aRegionMem,
                             stlBool        aEnableAlloc )
{
    aRegionMem->mEnableAlloc = aEnableAlloc;
}


/**
 * @brief 영역 기반 메모리 정보를 출력한다.
 * @param[in,out] aRegionMem 영역 기반 메모리 할당자
 * @internal
 */
void knlTestPrintRegionMem( knlRegionMem * aRegionMem )
{
    knmRPrint( aRegionMem );
}

/** @} */
