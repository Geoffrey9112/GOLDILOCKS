/*******************************************************************************
 * knmRegion.c
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
 * @file knmRegion.c
 * @brief Kernel Layer Region-based Memory Management Internal Routines
 */

#include <stl.h>
#include <dtl.h>
#include <knlDef.h>
#include <knlRing.h>
#include <knlDynamicMem.h>
#include <knsSegment.h>
#include <knDef.h>
#include <knmRegion.h>
#include <knmValidate.h>
#include <knlLatch.h>
#include <knmDynamic.h>

extern knsEntryPoint    * gKnEntryPoint;
extern knlAllocatorDesc   gKnmAllocatorDesc[];

/**
 * @addtogroup knmRegion
 * @{
 */

/**
 * @brief 메모리 arena를 할당한다.
 * @param[in,out] aRegionMem 영역 기반 메모리 할당자
 * @param[out] aArenaHeader 할당된 arena의 헤더
 * @param[in] aIsInit 기본 arena 생성 여부
 * @param[in,out] aEnv 커널 Environment
 * @par Error Codes :
 * @code
 * [KNL_ERRCODE_UNABLE_EXTEND_MEMORY]
 *     영역 기반 메모리 할당자의 메모리 할당 크기가 0으로 설정되어 있다.
 * @endcode
 * @todo 할당할 arena의 크기가 0일 경우 에러를 반환해야 한다.
 */
stlStatus knmAllocArena( knlRegionMem    * aRegionMem,
                         knmArenaHeader ** aArenaHeader,
                         stlBool           aIsInit,
                         knlEnv          * aEnv)
{
    knmArenaHeader * sHeader;
    stlUInt64        sArenaSize;
    stlSize          sAllocSize = 0;
    void           * sAddr;
    stlInt64         sCacheAlignedSize;

    if( aIsInit == STL_TRUE )
    {
        sArenaSize = aRegionMem->mInitSize;
    }
    else
    {
        sArenaSize = aRegionMem->mNextSize;
    }

    STL_TRY_THROW( sArenaSize > 0, RAMP_ERR_UNABLE_EXTEND_MEMORY );

    /*
     * warm-up 이전에 들어올 경우
     */
    if( gKnEntryPoint == NULL )
    {
        sCacheAlignedSize = 64;
    }
    else
    {
        sCacheAlignedSize = gKnEntryPoint->mCacheAlignedSize;
    }

    sAllocSize = sArenaSize + STL_SIZEOF(knmArenaHeader) + sCacheAlignedSize;

    if( aRegionMem->mParentMem != NULL )
    {
        STL_TRY( knlAllocDynamicMem( aRegionMem->mParentMem,
                                     sAllocSize,
                                     &sAddr,
                                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        if( aRegionMem->mMemType == KNL_MEM_STORAGE_TYPE_HEAP )
        {
            if( (aRegionMem->mCategory == KNL_MEM_CATEGORY_DATABASE) ||
                (aRegionMem->mCategory == KNL_MEM_CATEGORY_NONE) )
            {
                STL_TRY( stlAllocHeap( &sAddr,
                                       sAllocSize,
                                       KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
            }
            else
            {
                STL_TRY( knlAllocDynamicMem( &aEnv->mPrivateStaticArea,
                                             sAllocSize,
                                             &sAddr,
                                             aEnv )
                         == STL_SUCCESS );
            }
        }
        else
        {
            STL_TRY( knmDAlloc( gKnEntryPoint->mDynamicMem->mController,
                                gKnEntryPoint->mDynamicMem,
                                aRegionMem,
                                KNL_ALLOCATOR_TYPE_REGION,
                                STL_ALIGN_DEFAULT(sAllocSize),
                                (void**)&sAddr,
                                aEnv )
                     == STL_SUCCESS );
        }
    }

    aRegionMem->mTotalSize = aRegionMem->mTotalSize + sAllocSize;

    /*
     * arena header 초기화
     */

    /*
     * arena header는 할당 받은 메모리의 제일 처음에 위치한다.
     */
    sHeader = (knmArenaHeader*)STL_ALIGN( (stlUInt64)sAddr, sCacheAlignedSize );

    sHeader->mAddr        = sAddr;
    sHeader->mSize        = sArenaSize;
    sHeader->mAllocOffset = 0;

    STL_RING_INIT_DATALINK( sHeader,
                            STL_OFFSETOF(knmArenaHeader, mLink) );

    STL_RING_ADD_LAST( &(aRegionMem->mAllocRing),
                       sHeader );

    *aArenaHeader = sHeader;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_UNABLE_EXTEND_MEMORY )
    {
        STL_DASSERT( aRegionMem->mTotalSize != 0 );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_UNABLE_EXTEND_MEMORY,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      STL_INT64_MAX,   /**< MAX 제한이 없음 */
                      aRegionMem->mTotalSize,
                      sAllocSize,
                      gKnmAllocatorDesc[aRegionMem->mAllocatorId].mDescription );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 반환된 arena에서 할당 가능한 arena를 찾는다.
 * @param[in,out] aRegionMem 영역 기반 메모리 할당자
 * @param[in] aSize 할당할 메모리 크기
 * @param[out] aFreeArena 사용 가능한 arena의 주소
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knmRFind( knlRegionMem    * aRegionMem,
                    stlUInt32         aSize,
                    knmArenaHeader ** aFreeArena,
                    knlEnv          * aEnv )
{
    knmArenaHeader * sArenaHeader;
    knmArenaHeader * sNextHeader;

    STL_RING_FOREACH_ENTRY_SAFE( &(aRegionMem->mFreeRing),
                                 sArenaHeader,
                                 sNextHeader )
    {
        if( aSize <= sArenaHeader->mSize )
        {
            sArenaHeader->mAllocOffset = 0;
            break;
        }
    }

    STL_TRY( STL_RING_IS_HEADLINK( &(aRegionMem->mFreeRing),
                                   sArenaHeader ) == STL_FALSE );

    STL_RING_UNLINK( &(aRegionMem->mFreeRing),
                     sArenaHeader );

    STL_RING_ADD_LAST( &(aRegionMem->mAllocRing),
                       sArenaHeader );

    *aFreeArena = sArenaHeader;

    return STL_SUCCESS;

    STL_FINISH;

    *aFreeArena = NULL;

    return STL_FAILURE;
}

/**
 * @brief 영역 기반 메모리를 할당한다.
 * @param[in,out] aRegionMem 영역 기반 메모리 할당자
 * @param[in] aSize 할당할 메모리 크기
 * @param[out] aAddr 할당된 메모리 주소
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knmRAlloc( knlRegionMem   * aRegionMem,
                     stlUInt32        aSize,
                     void          ** aAddr,
                     knlEnv         * aEnv )
{
    knmArenaHeader * sLastAreaHeader;
    stlUInt64        sArenaSize;
    stlUInt32        sState = 0;
    stlUInt32        sSize = aSize;

    sLastAreaHeader = STL_RING_GET_LAST_DATA( &(aRegionMem->mAllocRing) );

    /**
     * allocated arena가 없다면
     */
    if( STL_RING_IS_HEADLINK( &(aRegionMem->mAllocRing),
                              sLastAreaHeader ) == STL_TRUE )
    {
        STL_TRY( knmAllocArena( aRegionMem,
                                &sLastAreaHeader,
                                STL_FALSE,
                                aEnv ) == STL_SUCCESS );
    }

    if( KNM_CHECK_MEMORY_BOUNDARY(aEnv) == STL_TRUE )
    {
        /**
         * Memory Boundary를 검사하기 위해서 Fence 메모리를 추가적으로
         * 확보한다.
         */
        sSize += ( STL_SIZEOF(knmHeadFence) + STL_SIZEOF(knmTailFence) );
    }

    if( (sLastAreaHeader->mSize - sLastAreaHeader->mAllocOffset) < sSize )
    {
        /*
         * 할당 가능한 free arena를 찾는다.
         */
        if( knmRFind( aRegionMem,
                      sSize,
                      &sLastAreaHeader,
                      aEnv ) == STL_FAILURE )
        {
            /*
             * 새로운 arena를 생성한다.
             * max block(65535) * max precision(DTL_VARCHAR_MAX_BUFFER_SIZE)
             */
            STL_DASSERT( sSize <= ((65535 * DTL_VARCHAR_MAX_BUFFER_SIZE)*2) );

            sArenaSize = aRegionMem->mNextSize;
            aRegionMem->mNextSize = STL_MAX( STL_ALIGN_DEFAULT(sSize),
                                             sArenaSize );
            sState = 1;

            STL_TRY( knmAllocArena(aRegionMem,
                                   &sLastAreaHeader,
                                   STL_FALSE,
                                   aEnv) == STL_SUCCESS );
            sState = 0;
            aRegionMem->mNextSize = sArenaSize;
        }
    }

    if( KNM_CHECK_MEMORY_BOUNDARY(aEnv) == STL_TRUE )
    {
        /**
         * Memory Boundary Fence를 초기화한다.
         */
        *aAddr = ((void*)sLastAreaHeader +
                  STL_SIZEOF(knmArenaHeader) +
                  sLastAreaHeader->mAllocOffset +
                  STL_SIZEOF(knmHeadFence));

        STL_TRY( knmInitMemory( ((void*)sLastAreaHeader +
                                 STL_SIZEOF(knmArenaHeader) +
                                 sLastAreaHeader->mAllocOffset),
                                aSize,
                                aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        *aAddr = ((void*)sLastAreaHeader +
                  STL_SIZEOF(knmArenaHeader) +
                  sLastAreaHeader->mAllocOffset);
    }
    
    sLastAreaHeader->mAllocOffset = sLastAreaHeader->mAllocOffset + sSize;

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        aRegionMem->mNextSize = sArenaSize;
    }

    return STL_FAILURE;
}

/**
 * @brief 입력받은 메모리 arena를 반환한다.
 * @param[in,out] aRegionMem   영역 기반 메모리 할당자
 * @param[in]     aRing        반환할 메모리 ring
 * @param[in]     aArenaHeader 반환할 메모리 arena
 * @param[in,out] aEnv         커널 Environment
 */
stlStatus knmFreeArena( knlRegionMem   * aRegionMem,
                        stlRingHead    * aRing,
                        knmArenaHeader * aArenaHeader,
                        knlEnv         * aEnv)
{
    stlInt64 sCacheAlignedSize;

    STL_RING_UNLINK( aRing, aArenaHeader );

    /*
     * warm-up 이전에 들어올 경우
     */
    if( gKnEntryPoint == NULL )
    {
        sCacheAlignedSize = 64;
    }
    else
    {
        sCacheAlignedSize = gKnEntryPoint->mCacheAlignedSize;
    }

    aRegionMem->mTotalSize = aRegionMem->mTotalSize -
        aArenaHeader->mSize -
        STL_SIZEOF(knmArenaHeader) -
        sCacheAlignedSize;

    if( KNM_CHECK_MEMORY_BOUNDARY(aEnv) == STL_TRUE )
    {
        /**
         * Memory Boundary Fence의 무결성을 검사한다.
         */
        STL_TRY( knmValidateArena( aArenaHeader,
                                   aEnv )
                 == STL_SUCCESS );
    }

    if( aRegionMem->mParentMem != NULL )
    {
        STL_TRY( knlFreeDynamicMem( aRegionMem->mParentMem,
                                    aArenaHeader->mAddr,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        if( aRegionMem->mMemType == KNL_MEM_STORAGE_TYPE_HEAP )
        {
            if( (aRegionMem->mCategory == KNL_MEM_CATEGORY_DATABASE) ||
                (aRegionMem->mCategory == KNL_MEM_CATEGORY_NONE) )
            {
                stlFreeHeap( aArenaHeader->mAddr );
            }
            else
            {
                STL_TRY( knlFreeDynamicMem( &aEnv->mPrivateStaticArea,
                                            aArenaHeader->mAddr,
                                            aEnv )
                         == STL_SUCCESS );
            }
        }
        else
        {
            STL_TRY( knmDFree( gKnEntryPoint->mDynamicMem->mController,
                               gKnEntryPoint->mDynamicMem,
                               aArenaHeader->mAddr,
                               aEnv )
                     == STL_SUCCESS );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 생성된 모든 메모리 arena를 반환한다.
 * @param[in,out] aRegionMem 영역 기반 메모리 할당자
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knmFreeAllArena( knlRegionMem * aRegionMem,
                           knlEnv       * aEnv)
{
    knmArenaHeader * sArenaHeader;
    knmArenaHeader * sNextHeader;

    /*
     * alloc arena ring
     */
    STL_RING_FOREACH_ENTRY_SAFE( &(aRegionMem->mAllocRing),
                                 sArenaHeader,
                                 sNextHeader )
    {
        STL_TRY( knmFreeArena( aRegionMem,
                               &(aRegionMem->mAllocRing),
                               sArenaHeader,
                               aEnv ) == STL_SUCCESS );
    }

    /*
     * free arena ring
     */
    STL_RING_FOREACH_ENTRY_SAFE( &(aRegionMem->mFreeRing),
                                 sArenaHeader,
                                 sNextHeader )
    {
        STL_TRY( knmFreeArena( aRegionMem,
                               &(aRegionMem->mFreeRing),
                               sArenaHeader,
                               aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 생성된 모든 영역 기반 메모리를 정리한다.
 * @param[in,out] aRegionMem 영역 기반 메모리 할당자
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knmRClear( knlRegionMem   * aRegionMem,
                     knlEnv         * aEnv )
{
    knmArenaHeader * sArenaHeader;
    knmArenaHeader * sNextHeader;
    stlBool          sIsInitArea = STL_TRUE;

    /*
     * alloc arena ring
     */
    STL_RING_FOREACH_ENTRY_SAFE( &(aRegionMem->mAllocRing),
                                 sArenaHeader,
                                 sNextHeader )
    {
        /*
         * arena init size로 생성된 한개의 arena만 유지한다.
         * - InitSize가 0 이면 모두 해제한다.
         */
        if( (aRegionMem->mInitSize > 0) && (sIsInitArea == STL_TRUE) )
        {
            sArenaHeader->mAllocOffset = 0;

            sIsInitArea = STL_FALSE;
            continue;
        }

        STL_TRY( knmFreeArena( aRegionMem,
                               &(aRegionMem->mAllocRing),
                               sArenaHeader,
                               aEnv ) == STL_SUCCESS );
    }

    /*
     * free arena ring
     */
    STL_RING_FOREACH_ENTRY_SAFE( &(aRegionMem->mFreeRing),
                                 sArenaHeader,
                                 sNextHeader )
    {
        STL_TRY( knmFreeArena( aRegionMem,
                               &(aRegionMem->mFreeRing),
                               sArenaHeader,
                               aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 현재까지 사용한 영역 기반 메모리의 정보를 저장한다.
 * @param[in,out] aRegionMem 영역 기반 메모리 할당자
 * @param[out] aMark 현재까지 사용한 영역 기반 메모리의 정보
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knmRMark( knlRegionMem  * aRegionMem,
                    knlRegionMark * aMark,
                    knlEnv        * aEnv )
{
    knmArenaHeader * sLastAreaHeader;

    sLastAreaHeader = STL_RING_GET_LAST_DATA( &(aRegionMem->mAllocRing) );

    STL_TRY( STL_RING_IS_HEADLINK( &(aRegionMem->mAllocRing),
                                   sLastAreaHeader ) == STL_FALSE );

    aMark->mArena  = (void*)sLastAreaHeader;
    aMark->mOffset = sLastAreaHeader->mAllocOffset;

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
 * @remark @a aMark 이후에 생성된 메모리는 시스템에 반환되지 않고 유지한다.
 */
stlStatus knmRReset( knlRegionMem  * aRegionMem,
                     knlRegionMark * aMark,
                     stlBool         aFreeChunk,
                     knlEnv        * aEnv )
{
    knmArenaHeader * sArenaHeader;
    knmArenaHeader * sNextHeader;
    knmArenaHeader * sHeader;
    knmArenaHeader * sLastHeader;

    sArenaHeader = (knmArenaHeader *)(aMark->mArena);
    
    if( KNM_CHECK_MEMORY_BOUNDARY(aEnv) == STL_TRUE )
    {
        STL_TRY( knmValidateArena( sArenaHeader,
                                   aEnv )
                 == STL_SUCCESS );
    }

    sArenaHeader->mAllocOffset = aMark->mOffset;

    sNextHeader = STL_RING_GET_NEXT_DATA( &(aRegionMem->mAllocRing),
                                          sArenaHeader );

    if( STL_RING_IS_HEADLINK( &(aRegionMem->mAllocRing),
                              sNextHeader ) == STL_FALSE )
    {
        if( aFreeChunk == STL_FALSE )
        {
            /*
             * Mark 이후에 생성된 arena를 free arena ring에 추가한다.
             */
            sLastHeader = STL_RING_GET_LAST_DATA( &(aRegionMem->mAllocRing) );

            STL_RING_UNSPLICE( &(aRegionMem->mAllocRing),
                               sNextHeader,
                               sLastHeader );

            if( KNM_CHECK_MEMORY_BOUNDARY(aEnv) == STL_TRUE )
            {
                sHeader = sNextHeader;
            
                while( sHeader != sLastHeader )
                {
                    STL_TRY( knmValidateArena( sHeader,
                                               aEnv )
                             == STL_SUCCESS );

                    sHeader = STL_RING_GET_NEXT_DATA( &(aRegionMem->mAllocRing),
                                                      sHeader );
                }
            
                STL_TRY( knmValidateArena( sHeader,
                                           aEnv )
                         == STL_SUCCESS );
            }

            STL_RING_SPLICE_LAST( &(aRegionMem->mFreeRing),
                                  sNextHeader,
                                  sLastHeader );
        }
        else
        {
            /*
             * Mark 이후에 생성된 arena를 삭제한다.
             */
            
            STL_RING_AT_FOREACH_ENTRY_SAFE( &(aRegionMem->mAllocRing),
                                            sNextHeader,
                                            sArenaHeader,
                                            sNextHeader )
            {
                if( KNM_CHECK_MEMORY_BOUNDARY(aEnv) == STL_TRUE )
                {
                    STL_TRY( knmValidateArena( sArenaHeader,
                                               aEnv )
                             == STL_SUCCESS );
                }
                
                STL_TRY( knmFreeArena( aRegionMem,
                                       &(aRegionMem->mAllocRing),
                                       sArenaHeader,
                                       aEnv ) == STL_SUCCESS );
            }
        }
    }
                          
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 영역 기반 메모리 정보를 출력한다.
 * @param[in,out] aRegionMem 영역 기반 메모리 할당자
 */
void knmRPrint( knlRegionMem  * aRegionMem )
{
    knmArenaHeader * sArenaHeader;
    stlUInt32        sIdx = 0;

    STL_RING_FOREACH_ENTRY( &(aRegionMem->mAllocRing), sArenaHeader )
    {
        stlPrintf("    ===================================================================\n");
        stlPrintf("    ALLOC ARENA #%d : SIZE[%ld], ALLOC OFFSET[%ld]\n", sIdx++,
                  sArenaHeader->mSize,
                  sArenaHeader->mAllocOffset);
        stlPrintf("    ===================================================================\n");
    }

    sIdx = 0;

    STL_RING_FOREACH_ENTRY( &(aRegionMem->mFreeRing), sArenaHeader )
    {
        stlPrintf("        ===============================================================\n");
        stlPrintf("        FREE ARENA #%d : SIZE[%ld]\n", sIdx++,
                                                          sArenaHeader->mSize);
        stlPrintf("        ===============================================================\n");
    }
}


/** @} */
