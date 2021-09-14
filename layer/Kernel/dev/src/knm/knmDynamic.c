/*******************************************************************************
 * knmDynamic.c
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
 * @file knmDynamic.c
 * @brief Kernel Layer Dynamic Memory Management Internal Routines
 */

#include <stl.h>
#include <dtl.h>
#include <knlDef.h>
#include <knlRing.h>
#include <knlDynamicMem.h>
#include <knsSegment.h>
#include <knDef.h>
#include <knmDynamic.h>
#include <knlLatch.h>
#include <knmValidate.h>
#include <knlProperty.h>
#include <knlSession.h>

/**
 * @addtogroup knmDynamic
 * @{
 */

/**
 * @brief 커널의 각 모듈들에 대한 entry point
 */
extern knsEntryPoint    * gKnEntryPoint;
extern knlAllocatorDesc   gKnmAllocatorDesc[];

/**
 * @brief 메모리 chunk를 할당한다.
 * @param[in] aMemController Memory Controler
 * @param[in,out] aDynamicMem 동적 메모리 할당자
 * @param[out] aChunkHeader 할당된 chunk의 헤더
 * @param[in] aIsInit 기본 chunk 생성 여부
 * @param[in,out] aEnv 커널 Environment
 * @par Error Codes :
 * @code
 * [KNL_ERRCODE_UNABLE_EXTEND_MEMORY]
 *     동적 메모리 할당자의 메모리 할당 크기가 0으로 설정되어 있다.
 * @endcode
 * @todo 할당할 chunk의 크기가 0일 경우 에러를 반환해야 한다.
 */
stlStatus knmAllocChunk( knlMemController * aMemController,
                         knlDynamicMem    * aDynamicMem,
                         knmChunkHeader  ** aChunkHeader,
                         stlBool            aIsInit,
                         knlEnv           * aEnv)
{
    void           * sAddr;
    knmChunkHeader * sHeader;
    stlUInt64        sChunkSize;
    stlUInt64        sAllocSize;
    stlUInt64        sMaxSize;
    stlBool          sIsSuccess;
    stlInt32         sState = 0;

    if( aIsInit == STL_TRUE )
    {
        sChunkSize = aDynamicMem->mInitSize;
    }
    else
    {
        sChunkSize = aDynamicMem->mNextSize;
    }

    sAllocSize = sChunkSize + STL_SIZEOF(knmChunkHeader);

    /**
     * 할당 가능한 범위를 넘어가는지 검사한다.
     */
    if( ( KNL_SESS_ENV( aEnv ) != NULL ) &&
        ( aDynamicMem->mMaxSize == KNL_MEMORY_TOTAL_SIZE_DEPEND_ON_PROPERTY ) )

    {
        /**
         * 프로퍼티로 부터 max size를 결정한다.
         */
        sMaxSize = knlGetPropertyBigIntValueByID( KNL_PROPERTY_PRIVATE_STATIC_AREA_SIZE,
                                                  aEnv );
    }
    else
    {
        sMaxSize = aDynamicMem->mMaxSize;
    }

    /* Variable Static Area에서 공간 부족시 추가적인 메모리 할당은 없다. */
    STL_TRY_THROW( sChunkSize > 0, RAMP_ERR_UNABLE_EXTEND_MEMORY );

    STL_TRY_THROW( (aDynamicMem->mTotalSize + sAllocSize) < sMaxSize,
                   RAMP_ERR_UNABLE_EXTEND_MEMORY );

    if( aMemController != NULL )
    {
        STL_TRY( knlAcquireLatch( &(aMemController->mLatch),
                                  KNL_LATCH_MODE_EXCLUSIVE,
                                  KNL_LATCH_PRIORITY_NORMAL,
                                  STL_INFINITE_TIME,
                                  &sIsSuccess,
                                  aEnv ) == STL_SUCCESS );
        sState = 1;
        
        STL_TRY_THROW( (aMemController->mTotalSize + sAllocSize) < aMemController->mMaxSize,
                       RAMP_ERR_UNABLE_EXTEND_CONTROL_MEMORY );
    }
    
    if( aDynamicMem->mParentMem != NULL )
    {
        STL_TRY( knlAllocDynamicMem( aDynamicMem->mParentMem,
                                     sAllocSize,
                                     &sAddr,
                                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        if( aDynamicMem->mType == KNL_MEM_STORAGE_TYPE_HEAP )
        {
            if( (aDynamicMem->mAllocatorLevel == KNL_ALLOCATOR_LEVEL_ROOT) ||
                (aDynamicMem->mCategory == KNL_MEM_CATEGORY_DATABASE)      ||
                (aDynamicMem->mCategory == KNL_MEM_CATEGORY_NONE) )
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
                                aDynamicMem,
                                KNL_ALLOCATOR_TYPE_DYNAMIC,
                                STL_ALIGN_DEFAULT(sAllocSize),
                                (void**)&sAddr,
                                aEnv )
                     == STL_SUCCESS );
        }
    }

    /**
     * Dictionary Cache 의 고정길이 메모리의 재사용 횟수를 검사하기 위해 chunk 할당시 초기화 함
     * ( ellVisibleInfo.mMemUseCnt 정보 )
     */
    if ( (aDynamicMem->mAttr & KNL_MEM_ATTR_KEEP_EMPTY_CHUNK) == KNL_MEM_ATTR_KEEP_EMPTY_CHUNK )
    {
        stlMemset( sAddr, 0x00, sAllocSize );
    }

    if( aMemController != NULL )
    {
        aMemController->mTotalSize += sAllocSize;
        
        sState = 0;
        STL_TRY( knlReleaseLatch( &(aMemController->mLatch),
                                  aEnv ) == STL_SUCCESS );
    }

    aDynamicMem->mTotalSize = aDynamicMem->mTotalSize +
        sChunkSize +
        STL_SIZEOF(knmChunkHeader);

    /*
     * chunk header 초기화
     */

    /*
     * chunk header는 할당 받은 메모리의 제일 마지막에 위치한다.
     */
    sHeader = sAddr + sChunkSize;
    sHeader->mSize = sChunkSize;
    sHeader->mFreeBlockCount = 0;
    sHeader->mFreeBlockSize  = 0;

    STL_RING_INIT_DATALINK( sHeader,
                            STL_OFFSETOF(knmChunkHeader, mLink) );

    STL_RING_INIT_HEADLINK( (&sHeader->mAllocRing),
                            STL_OFFSETOF(knmBlockHeader, mAllocLink) );

    STL_RING_INIT_HEADLINK( (&sHeader->mFreeRing),
                            STL_OFFSETOF(knmBlockHeader, mFreeLink) );

    STL_RING_ADD_FIRST( &(aDynamicMem->mChunkRing),
                        sHeader );

    *aChunkHeader = sHeader;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_UNABLE_EXTEND_MEMORY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_UNABLE_EXTEND_MEMORY,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      sMaxSize,
                      aDynamicMem->mTotalSize,
                      sAllocSize,
                      gKnmAllocatorDesc[aDynamicMem->mAllocatorId].mDescription );
    }

    STL_CATCH( RAMP_ERR_UNABLE_EXTEND_CONTROL_MEMORY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_UNABLE_EXTEND_MEMORY,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      sMaxSize,
                      aMemController->mTotalSize,
                      sAllocSize,
                      gKnmAllocatorDesc[aDynamicMem->mAllocatorId].mDescription );
    }

    STL_FINISH;

    switch( sState )
    {
        case 1:
            knlReleaseLatch( &(aMemController->mLatch), aEnv );
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief 메모리 chunk를 반환한다.
 * @param[in] aMemController Memory Controler
 * @param[in,out] aDynamicMem 동적 메모리 할당자
 * @param[in] aChunkHeader 반환할 chunk의 헤더
 * @param[in,out] aEnv 커널 Environment
 * @todo 할당할 chunk의 크기가 0일 경우 에러를 반환해야 한다.
 */
stlStatus knmFreeChunk( knlMemController * aMemController,
                        knlDynamicMem    * aDynamicMem,
                        knmChunkHeader   * aChunkHeader,
                        knlEnv           * aEnv)
{
    void      * sAddr;
    stlUInt64   sChunkSize;
    stlBool     sIsSuccess;
    stlInt32    sState = 0;

    STL_RING_UNLINK( &(aDynamicMem->mChunkRing),
                     aChunkHeader );

    sChunkSize = aChunkHeader->mSize;

    sAddr = (void*)aChunkHeader - sChunkSize;

    if( aMemController != NULL )
    {
        STL_TRY( knlAcquireLatch( &(aMemController->mLatch),
                                  KNL_LATCH_MODE_EXCLUSIVE,
                                  KNL_LATCH_PRIORITY_NORMAL,
                                  STL_INFINITE_TIME,
                                  &sIsSuccess,
                                  aEnv ) == STL_SUCCESS );
        sState = 1;
    }
    
    if( aDynamicMem->mParentMem != NULL )
    {
        STL_TRY( knlFreeDynamicMem( aDynamicMem->mParentMem,
                                    sAddr,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        if( aDynamicMem->mType == KNL_MEM_STORAGE_TYPE_HEAP )
        {
            if( (aDynamicMem->mAllocatorLevel == KNL_ALLOCATOR_LEVEL_ROOT) ||
                (aDynamicMem->mCategory == KNL_MEM_CATEGORY_DATABASE)      ||
                (aDynamicMem->mCategory == KNL_MEM_CATEGORY_NONE) )
            {
                stlFreeHeap( sAddr );
            }
            else
            {
                STL_TRY( knlFreeDynamicMem( &aEnv->mPrivateStaticArea,
                                            sAddr,
                                            aEnv )
                         == STL_SUCCESS );
            }
        }
        else
        {
            STL_TRY( knmDFree( gKnEntryPoint->mDynamicMem->mController,
                               gKnEntryPoint->mDynamicMem,
                               sAddr,
                               aEnv )
                     == STL_SUCCESS );
        }
    }

    if( aMemController != NULL )
    {
        aMemController->mTotalSize -= ( sChunkSize +
                                        STL_SIZEOF(knmChunkHeader) );
        
        sState = 0;
        STL_TRY( knlReleaseLatch( &(aMemController->mLatch),
                                  aEnv ) == STL_SUCCESS );
    }

    aDynamicMem->mTotalSize = ( aDynamicMem->mTotalSize -
                                sChunkSize -
                                STL_SIZEOF(knmChunkHeader) );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            knlReleaseLatch( &(aMemController->mLatch), aEnv );
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief 모든 메모리 chunk를 반환한다.
 * @param[in] aMemController Memory Controler
 * @param[in,out] aDynamicMem 동적 메모리 할당자
 * @param[in,out] aEnv 커널 Environment
 * @todo 할당할 chunk의 크기가 0일 경우 에러를 반환해야 한다.
 */
stlStatus knmFreeAllChunk( knlMemController * aMemController,
                           knlDynamicMem    * aDynamicMem,
                           knlEnv           * aEnv)
{
    knmChunkHeader * sChunkHeader;
    knmChunkHeader * sNextHeader;

    STL_RING_FOREACH_ENTRY_SAFE( &(aDynamicMem->mChunkRing),
                                 sChunkHeader,
                                 sNextHeader )
    {
        STL_TRY( knmFreeChunk( aMemController,
                               aDynamicMem,
                               sChunkHeader,
                               aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief chunk의 free list에서 할당 가능한 block을 찾는다.
 * @param[in] aChunkHeader 할당 가능한 block을 찾을 chunk의 header
 * @param[in] aSize 할당할 메모리 크기
 * @param[out] aFreeBlock 할당 가능한 메모리
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knmFindInFreeList( knmChunkHeader  * aChunkHeader,
                             stlUInt32         aSize,
                             knmBlockHeader ** aFreeBlock,
                             knlEnv          * aEnv )
{
    knmBlockHeader * sBlockHeader;

    *aFreeBlock = NULL;

    STL_TRY( aChunkHeader->mFreeBlockCount > 0 );

    /*
     * first fit 알고리즘을 사용한다.
     */

    STL_RING_FOREACH_ENTRY( &(aChunkHeader->mFreeRing), sBlockHeader )
    {
        STL_ASSERT( sBlockHeader->mIsUsed == STL_FALSE );

        if( sBlockHeader->mSize >= aSize )
        {
            *aFreeBlock = sBlockHeader;
            break;
        }
    }

    STL_TRY( STL_RING_IS_HEADLINK( &(aChunkHeader->mFreeRing),
                                   sBlockHeader ) == STL_FALSE );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief free block을 사용해 메모리를 할당한다.
 * @param[in] aDynamicMem 동적 메모리 할당자
 * @param[in] aChunkHeader 할당 가능한 block을 찾을 chunk의 header
 * @param[in] aSize 할당할 메모리 크기
 * @param[in] aFreeBlock 할당 가능한 free block
 * @param[out] aAllocBlock 할당한 메모리
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knmAllocInFreeBlock( knlDynamicMem   * aDynamicMem,
                               knmChunkHeader  * aChunkHeader,
                               stlUInt32         aSize,
                               knmBlockHeader  * aFreeBlock,
                               knmBlockHeader ** aAllocBlock,
                               knlEnv          * aEnv )
{
    knmBlockHeader * sNewBlockHeader;
    knmBlockHeader * sAllocBlockHeader;

    sAllocBlockHeader = aFreeBlock;

    STL_RING_UNLINK( &(aChunkHeader->mFreeRing),
                     sAllocBlockHeader );

    aChunkHeader->mFreeBlockCount--;

    /**
     * fragment를 최소로 하기 위해 threshold를 검사한다.
     */

    if( (aFreeBlock->mSize - aSize) <= (aDynamicMem->mMinFragSize + STL_SIZEOF(knmBlockHeader)) )
    {
        aChunkHeader->mFreeBlockSize -= ( STL_SIZEOF(knmBlockHeader) + aFreeBlock->mSize );
    }
    else
    {
        /*
         * 메모리 할당 후 남은 block 초기화
         */
        sNewBlockHeader = (knmBlockHeader*)( (void*)aFreeBlock +
                                             STL_SIZEOF( knmBlockHeader ) +
                                             aSize );

        STL_RING_INIT_DATALINK( sNewBlockHeader, STL_OFFSETOF( knmBlockHeader,
                                                               mAllocLink) );
        STL_RING_INIT_DATALINK( sNewBlockHeader, STL_OFFSETOF( knmBlockHeader,
                                                               mFreeLink) );

        sNewBlockHeader->mSize   = aFreeBlock->mSize - aSize - STL_SIZEOF( knmBlockHeader );
        sNewBlockHeader->mIsUsed = STL_FALSE;

        /*
         * allocRing에 남은 block을 alloc된 block 앞에 추가한다.
         */
        STL_RING_INSERT_BEFORE( &(aChunkHeader->mAllocRing),
                                sAllocBlockHeader,
                                sNewBlockHeader );

        STL_RING_ADD_FIRST( &(aChunkHeader->mFreeRing),
                            sNewBlockHeader );
        
        aChunkHeader->mFreeBlockCount++;
        
        STL_DASSERT( (STL_RING_GET_PREV_DATA(&(aChunkHeader->mAllocRing),
                                             sNewBlockHeader) -
                      (void*)sNewBlockHeader -
                      STL_SIZEOF(knmBlockHeader))
                     == sNewBlockHeader->mSize );

        sAllocBlockHeader->mSize = aSize;

        aChunkHeader->mFreeBlockSize = ( aChunkHeader->mFreeBlockSize -
                                         aSize -
                                         STL_SIZEOF(knmBlockHeader) );
        
        STL_DASSERT( ((void*)sNewBlockHeader -
                      STL_RING_GET_NEXT_DATA(&(aChunkHeader->mAllocRing),
                                             sNewBlockHeader) -
                      STL_SIZEOF(knmBlockHeader))
                     == sAllocBlockHeader->mSize );
    }

    /*
     * 할당된 block 초기화
     */
    STL_RING_INIT_DATALINK( sAllocBlockHeader, STL_OFFSETOF( knmBlockHeader,
                                                             mFreeLink ) );

    sAllocBlockHeader->mIsUsed = STL_TRUE;

    STL_DASSERT( (STL_RING_GET_PREV_DATA(&(aChunkHeader->mAllocRing),
                                         sAllocBlockHeader) -
                  (void*)sAllocBlockHeader -
                  STL_SIZEOF(knmBlockHeader))
                 == sAllocBlockHeader->mSize );

    *aAllocBlock = sAllocBlockHeader;

    return STL_SUCCESS;
}

/**
 * @brief 아직 chunk에서 사용되지 않은 메모리 영역에서 할당한다.
 * @param[in] aChunkHeader 메모리를 할당할 chunk의 header
 * @param[in] aSize 할당할 메모리 크기
 * @param[out] aAllocBlock 할당한 메모리
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knmAllocInChunk( knmChunkHeader  * aChunkHeader,
                           stlUInt32         aSize,
                           knmBlockHeader ** aAllocBlock,
                           knlEnv          * aEnv )
{
    knmBlockHeader * sLastBlockHeader;
    knmBlockHeader * sNewBlockHeader;

    /*
     * 새로운 block의 header는 할당된 메모리의 제일 앞에 존재한다.
     */
    sLastBlockHeader = (knmBlockHeader*)STL_RING_GET_LAST_DATA( &(aChunkHeader->mAllocRing) );
    sNewBlockHeader = (knmBlockHeader*)((void*)sLastBlockHeader -
                                        aSize -
                                        STL_SIZEOF( knmBlockHeader ));

    STL_RING_INIT_DATALINK( sNewBlockHeader,
                            STL_OFFSETOF( knmBlockHeader, mAllocLink ) );

    STL_RING_INIT_DATALINK( sNewBlockHeader,
                            STL_OFFSETOF( knmBlockHeader, mFreeLink ) );

    sNewBlockHeader->mSize = aSize;
    sNewBlockHeader->mIsUsed = STL_TRUE;

    /*
     * 새로운 block의 위치는 마지막에 할당한 block 앞에 존재한다.
     */
    STL_RING_ADD_LAST( &(aChunkHeader->mAllocRing),
                       sNewBlockHeader );

    *aAllocBlock = sNewBlockHeader;

    return STL_SUCCESS;
}

/**
 * @brief 할당 가능한 동적 메모리 위치를 찾는다.
 * @param[in,out] aDynamicMem 동적 메모리 할당자
 * @param[in] aSize 할당할 메모리 크기
 * @param[out] aFreeBlock 할당 가능한 메모리
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knmFindFreeSpace( knlDynamicMem   * aDynamicMem,
                            stlUInt32         aSize,
                            knmBlockHeader ** aFreeBlock,
                            knlEnv          * aEnv )
{
    knmChunkHeader * sChunkHeader;
    knmBlockHeader * sBlockHeader;
    stlInt64         sRemain;
    stlInt64         sSize;

    STL_RING_FOREACH_ENTRY( &(aDynamicMem->mChunkRing), sChunkHeader )
    {
        if( sChunkHeader->mFreeBlockCount > 0 )
        {
            /*
             * 현재 chunk의 모든 free block을 검사해 할당 가능한 block이 있는지 찾는다.
             */
            if( knmFindInFreeList( sChunkHeader,
                                   aSize,
                                   &sBlockHeader,
                                   aEnv ) == STL_SUCCESS )
            {
                STL_TRY( knmAllocInFreeBlock( aDynamicMem,
                                              sChunkHeader,
                                              aSize,
                                              sBlockHeader,
                                              aFreeBlock,
                                              aEnv ) == STL_SUCCESS );

                break;
            }
        }

        /*
         * 할당할 수 있는 free block이 존재하지 않으므로 새로운 block을 할당할 수 있는지 확인한다.
         * 현재까지 할당된 크기 : chunk header pointer - last alloc block pointer
         */

        sBlockHeader = (knmBlockHeader*)STL_RING_GET_LAST_DATA( &(sChunkHeader->mAllocRing) );

        sRemain = sChunkHeader->mSize - ((stlUInt64)sChunkHeader - (stlUInt64)sBlockHeader);
        sSize   = aSize + STL_SIZEOF(knmBlockHeader);

        STL_DASSERT( sRemain >= 0 );

        if( sRemain >= sSize )
        {
            STL_TRY( knmAllocInChunk( sChunkHeader,
                                      aSize,
                                      aFreeBlock,
                                      aEnv ) == STL_SUCCESS );
            break;
        }

        /*
         * 현재 chunk에서는 할당 할 수 없으므로 다음 chunk를 확인한다.
         */
    }

    STL_TRY( STL_RING_IS_HEADLINK( &(aDynamicMem->mChunkRing),
                                   sChunkHeader ) == STL_FALSE );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 입력 받은 block이 위차한 chunk를 찾는다.
 * @param[in] aDynamicMem 동적 메모리 할당자
 * @param[in] aBlockHeader chunk를 찾을 block 
 * @param[out] aChunkHeader @a aBlockHeader 이 위치한 chunk header
 * @param[out] aIsDefaultChunk 반환할 chunk가 default chunk인지 여부
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knmFindChunk( knlDynamicMem   * aDynamicMem,
                        knmBlockHeader  * aBlockHeader,
                        knmChunkHeader ** aChunkHeader,
                        stlBool         * aIsDefaultChunk,
                        knlEnv          * aEnv )
{
    knmChunkHeader * sChunkHeader;
    stlUInt32        sIdx = 0;

    *aIsDefaultChunk = STL_FALSE;

    STL_RING_FOREACH_ENTRY_REVERSE( &(aDynamicMem->mChunkRing), sChunkHeader )
    {
        if( ((void*)aBlockHeader < (void*)sChunkHeader) &&
            ((void*)aBlockHeader >= (void*)sChunkHeader - sChunkHeader->mSize) )
        {
            if( sIdx == 0 )
            {
                *aIsDefaultChunk = STL_TRUE;
            }

            break;
        }

        sIdx++;
    }

    STL_DASSERT( STL_RING_IS_HEADLINK( &(aDynamicMem->mChunkRing),
                                       sChunkHeader )
                 == STL_FALSE );

    *aChunkHeader = sChunkHeader;

    return STL_SUCCESS;
}

/**
 * @brief 동적 메모리를 할당한다.
 * @param[in] aMemController Memory Controler
 * @param[in,out] aDynamicMem 동적 메모리 할당자
 * @param[in] aRequestAllocator 메모리 할당을 요구한 Allocator
 * @param[in] aRequestAllocatorType 메모리 할당을 요구한 Allocator Type
 * @param[in] aSize 할당할 메모리 크기
 * @param[out] aAddr 할당된 메모리 주소
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knmDAlloc( knlMemController * aMemController,
                     knlDynamicMem    * aDynamicMem,
                     void             * aRequestAllocator,
                     knlAllocatorType   aRequestAllocatorType,
                     stlUInt32          aSize,
                     void            ** aAddr,
                     knlEnv           * aEnv )
{
    stlInt32         sLatchState = 0;
    stlInt32         sSizeState = 0;
    knmChunkHeader * sChunkHeader;
    knmBlockHeader * sBlockHeader = NULL;
    stlUInt64        sChunkSize;
    stlBool          sIsSuccess;
    stlUInt32        sSize = aSize;

    if( (aDynamicMem->mType == KNL_MEM_STORAGE_TYPE_SHM) ||
        (aDynamicMem->mType == KNL_MEM_STORAGE_TYPE_PLAN) )
    {
        STL_TRY( knlAcquireLatch( &(aDynamicMem->mLatch),
                                  KNL_LATCH_MODE_EXCLUSIVE,
                                  KNL_LATCH_PRIORITY_NORMAL,
                                  STL_INFINITE_TIME,
                                  &sIsSuccess,
                                  aEnv ) == STL_SUCCESS );
        sLatchState = 1;
    }

    if( KNM_CHECK_MEMORY_BOUNDARY(aEnv) == STL_TRUE )
    {
        /**
         * Memory Boundary를 검사하기 위해서 Fence 메모리를 추가적으로
         * 확보한다.
         */
        sSize += ( STL_SIZEOF(knmHeadFence) + STL_SIZEOF(knmTailFence) );
    }

    /*
     * 이미 할당된 chunk에서 할당 가능한 block이 없을 경우
     * 새로운 chunk를 할당해 메모리를 할당한다.
     */
    if( knmFindFreeSpace( aDynamicMem,
                          sSize,
                          &sBlockHeader,
                          aEnv ) == STL_FAILURE )
    {
        STL_DASSERT( sSize <= ((65535 * DTL_VARCHAR_MAX_BUFFER_SIZE)*2) );

        /*
         * 새로 할당할 chunk는 최소 ( 요청한 메모리 크기 + knmBlockHeader 크기) 만큼 필요하다.
         */
        sChunkSize = aDynamicMem->mNextSize;

        /**
         * Variable Static Area는 에러가 발생해야 한다.
         */
        if( sChunkSize > 0 )
        {
            aDynamicMem->mNextSize = STL_MAX( STL_ALIGN_DEFAULT(sSize + STL_SIZEOF(knmBlockHeader)),
                                              sChunkSize );
            sSizeState = 1;
        }

        STL_TRY( knmAllocChunk( aMemController,
                                aDynamicMem,
                                &sChunkHeader,
                                STL_FALSE,
                                aEnv ) == STL_SUCCESS );

        sSizeState = 0;
        aDynamicMem->mNextSize = sChunkSize;

        STL_TRY( knmAllocInChunk( sChunkHeader,
                                  sSize,
                                  &sBlockHeader,
                                  aEnv ) == STL_SUCCESS );
    }

    STL_ASSERT( sBlockHeader != NULL );

    sBlockHeader->mRequestorAddress = aRequestAllocator;
    sBlockHeader->mRequestorId = KNL_INVALID_ALLOCATOR_ID;
    sBlockHeader->mRequestorSessionId = 0;
    
#ifdef STL_DEBUG

    /**
     * performance issue로 인해 debug mode에서만 정보를 구축한다.
     */
    
    if( (KNL_SESS_ENV(aEnv) != NULL) &&
        (KNL_IS_SHARED_SESS_ENV( KNL_SESS_ENV(aEnv) ) == STL_TRUE) )
    {
        knlGetSessionEnvId( KNL_SESS_ENV(aEnv),
                            &(sBlockHeader->mRequestorSessionId),
                            aEnv );
    }
    
#endif
    
    if( aRequestAllocatorType == KNL_ALLOCATOR_TYPE_REGION )
    {
        sBlockHeader->mRequestorId = ((knlRegionMem*)aRequestAllocator)->mAllocatorId;
    }
    else
    {
        if( aRequestAllocatorType == KNL_ALLOCATOR_TYPE_DYNAMIC )
        {
            sBlockHeader->mRequestorId = ((knlDynamicMem*)aRequestAllocator)->mAllocatorId;
        }
    }

    if( KNM_CHECK_MEMORY_BOUNDARY(aEnv) == STL_TRUE )
    {
        /**
         * Memory Boundary Fence를 초기화한다.
         */
        *aAddr = ( (void*)sBlockHeader +
                   STL_SIZEOF(knmBlockHeader) +
                   STL_SIZEOF(knmHeadFence) );
            
        if( ((aDynamicMem->mAttr & KNL_MEM_ATTR_KEEP_EMPTY_CHUNK) !=
             KNL_MEM_ATTR_KEEP_EMPTY_CHUNK) )
        {
            STL_TRY( knmInitMemory( (void*)sBlockHeader + STL_SIZEOF(knmBlockHeader),
                                    aSize,
                                    aEnv )
                     == STL_SUCCESS );
        }
    }
    else
    {
        *aAddr = (void*)sBlockHeader + STL_SIZEOF(knmBlockHeader);
    }

    if( sLatchState == 1 )
    {
        sLatchState = 0;
        STL_TRY( knlReleaseLatch( &(aDynamicMem->mLatch),
                                  aEnv ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sLatchState == 1 )
    {
        knlReleaseLatch( &(aDynamicMem->mLatch), aEnv );
    }

    if( sSizeState == 1 )
    {
        aDynamicMem->mNextSize = sChunkSize;
    }
    
    return STL_FAILURE;
}

/**
 * @brief 동적 메모리를 cache align에 맞는 주소에 할당한다.
 * @param[in] aMemController Memory Controler
 * @param[in,out] aDynamicMem 동적 메모리 할당자
 * @param[in] aRequestAllocator 메모리 할당을 요구한 Allocator
 * @param[in] aRequestAllocatorType 메모리 할당을 요구한 Allocator Type
 * @param[in] aSize 할당할 메모리 크기
 * @param[out] aAddr 할당된 메모리 주소
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knmCacheAlignedDAlloc( knlMemController * aMemController,
                                 knlDynamicMem    * aDynamicMem,
                                 void             * aRequestAllocator,
                                 knlAllocatorType   aRequestAllocatorType,
                                 stlUInt32          aSize,
                                 void            ** aAddr,
                                 knlEnv           * aEnv )
{
    stlSize       sSize;
    void        * sAllocMem;
    void        * sAddr;

    sSize = aSize + gKnEntryPoint->mCacheAlignedSize + STL_SIZEOF( stlInt64 );

    STL_TRY( knmDAlloc( aMemController,
                        aDynamicMem,
                        aRequestAllocator,
                        aRequestAllocatorType,
                        STL_ALIGN_DEFAULT(sSize),
                        &sAllocMem,
                        aEnv )
             == STL_SUCCESS );

    sAddr = (void*)STL_ALIGN( (stlUInt64)(sAllocMem + STL_SIZEOF( stlInt64 )), gKnEntryPoint->mCacheAlignedSize );
    ((stlInt64*)sAddr)[-1] = ( sAddr - sAllocMem );

    *aAddr = sAddr;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;    
}

/**
 * @brief 동적 메모리를 반납한다.
 * @param[in] aMemController Memory Controler
 * @param[in,out] aDynamicMem 동적 메모리 할당자
 * @param[out] aAddr 반환할 메모리 주소
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knmDFree( knlMemController * aMemController,
                    knlDynamicMem    * aDynamicMem,
                    void             * aAddr,
                    knlEnv           * aEnv )
{
    stlInt32         sState = 0;
    knmChunkHeader * sChunkHeader;
    knmBlockHeader * sBlockHeader;
    knmBlockHeader * sPrevHeader;
    knmBlockHeader * sNextHeader;
    stlBool          sIsDefault;
    stlBool          sIsSuccess;

    if( (aDynamicMem->mType == KNL_MEM_STORAGE_TYPE_SHM) ||
        (aDynamicMem->mType == KNL_MEM_STORAGE_TYPE_PLAN) )
    {
        STL_TRY( knlAcquireLatch( &(aDynamicMem->mLatch),
                                  KNL_LATCH_MODE_EXCLUSIVE,
                                  KNL_LATCH_PRIORITY_NORMAL,
                                  STL_INFINITE_TIME,
                                  &sIsSuccess,
                                  aEnv ) == STL_SUCCESS );
        sState = 1;
    }

    /*
     * 입력받은 메모리가 위치한 chunk를 찾는다.
     */
    if( KNM_CHECK_MEMORY_BOUNDARY(aEnv) == STL_TRUE )
    {
        /**
         * Memory Boundary Fence의 무결성을 검사한다.
         */
        sBlockHeader = (knmBlockHeader*)( aAddr -
                                          STL_SIZEOF(knmBlockHeader) -
                                          STL_SIZEOF(knmHeadFence) );

        if( ((aDynamicMem->mAttr & KNL_MEM_ATTR_KEEP_EMPTY_CHUNK) !=
             KNL_MEM_ATTR_KEEP_EMPTY_CHUNK) )
        {
            STL_TRY( knmValidateFence( aAddr - STL_SIZEOF(knmHeadFence),
                                       aEnv )
                     == STL_SUCCESS );
        }
    }
    else
    {
        sBlockHeader = (knmBlockHeader*)(aAddr - STL_SIZEOF(knmBlockHeader));
    }

    STL_TRY( knmFindChunk( aDynamicMem,
                           sBlockHeader,
                           &sChunkHeader,
                           &sIsDefault,
                           aEnv ) == STL_SUCCESS );

    STL_ASSERT( sBlockHeader->mIsUsed == STL_TRUE );

    sBlockHeader->mIsUsed = STL_FALSE;

    /*
     * 반환되는 메모리를 free link의 맨 앞에 추가한다.
     */
    STL_RING_ADD_FIRST( &(sChunkHeader->mFreeRing),
                        sBlockHeader );

    sChunkHeader->mFreeBlockCount++;
    sChunkHeader->mFreeBlockSize = ( sChunkHeader->mFreeBlockSize +
                                     STL_SIZEOF(knmBlockHeader) +
                                     sBlockHeader->mSize );

    /*
     * 이전에 할당된 block이 사용 상태가 아니면 merge 가능
     */
    sPrevHeader = STL_RING_GET_PREV_DATA( &(sChunkHeader->mAllocRing),
                                          sBlockHeader );

    if( (STL_RING_IS_HEADLINK( &(sChunkHeader->mAllocRing),
                               sPrevHeader) == STL_FALSE) )
    {
        if( sPrevHeader->mIsUsed == STL_FALSE )
        {
            STL_RING_UNLINK( &(sChunkHeader->mAllocRing),
                             sPrevHeader );

            STL_RING_UNLINK( &(sChunkHeader->mFreeRing),
                             sPrevHeader );

            sBlockHeader->mSize = ( sBlockHeader->mSize +
                                    sPrevHeader->mSize +
                                    STL_SIZEOF(knmBlockHeader) );

            sChunkHeader->mFreeBlockCount--;
        }
    }

    /*
     * 이후에 할당된 block이 사용 상태가 아니면 merge 가능
     */
    sNextHeader = STL_RING_GET_NEXT_DATA( &(sChunkHeader->mAllocRing),
                                          sBlockHeader );

    if( (STL_RING_IS_HEADLINK( &(sChunkHeader->mAllocRing),
                               sNextHeader) == STL_FALSE) )
    {
        if( sNextHeader->mIsUsed == STL_FALSE )
        {
            STL_RING_UNLINK( &(sChunkHeader->mAllocRing),
                             sBlockHeader );

            STL_RING_UNLINK( &(sChunkHeader->mFreeRing),
                             sBlockHeader );

            sNextHeader->mSize = sNextHeader->mSize +
                sBlockHeader->mSize +
                STL_SIZEOF(knmBlockHeader);

            sChunkHeader->mFreeBlockCount--;
        }
    }

    /*
     * 맨 마지막에 할당된 block이 사용 상태가 아닐 경우 chunk에 반환
     */
    STL_RING_FOREACH_ENTRY_REVERSE_SAFE( &(sChunkHeader->mAllocRing),
                                         sBlockHeader,
                                         sPrevHeader )
    {
        if( sBlockHeader->mIsUsed == STL_FALSE )
        {
            STL_RING_UNLINK( &(sChunkHeader->mAllocRing),
                             sBlockHeader );

            STL_RING_UNLINK( &(sChunkHeader->mFreeRing),
                             sBlockHeader );

            sChunkHeader->mFreeBlockCount--;
            sChunkHeader->mFreeBlockSize = ( sChunkHeader->mFreeBlockSize -
                                             STL_SIZEOF(knmBlockHeader) -
                                             sBlockHeader->mSize );
        }
        else
        {
            break;
        }
    }

    /*
     * 기본 chunk가 아니면서 모든 block이 사용중이지 않을 경우 현재 chunk를 제거한다.
     * - 단 Empty Chunk의 free를 허용하는 allocator에 한해서 chunk를 제거한다.
     */
    if( (sIsDefault == STL_FALSE) &&
        ((aDynamicMem->mAttr & KNL_MEM_ATTR_KEEP_EMPTY_CHUNK) !=
         KNL_MEM_ATTR_KEEP_EMPTY_CHUNK) )
    {
        sBlockHeader = (knmBlockHeader*)STL_RING_GET_LAST_DATA( &(sChunkHeader->mAllocRing) );
        if( STL_RING_IS_HEADLINK( &(sChunkHeader->mAllocRing), sBlockHeader) == STL_TRUE)
        {
            STL_TRY( knmFreeChunk( aMemController,
                                   aDynamicMem,
                                   sChunkHeader,
                                   aEnv ) == STL_SUCCESS );
        }
    }

    if( sState == 1 )
    {
        sState = 0;
        STL_TRY( knlReleaseLatch( &(aDynamicMem->mLatch),
                                  aEnv ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            knlReleaseLatch( &(aDynamicMem->mLatch), aEnv );
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief cache align에 맞춰 할당한 동적 메모리를 반납한다.
 * @param[in] aMemController Memory Controler
 * @param[in,out] aDynamicMem 동적 메모리 할당자
 * @param[out] aAddr 반환할 메모리 주소
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knmCacheAlignedDFree( knlMemController * aMemController,
                                knlDynamicMem    * aDynamicMem,
                                void             * aAddr,
                                knlEnv           * aEnv )
{
    void    * sAddr;

    sAddr = aAddr - ((stlInt64*)aAddr)[-1];

    return knmDFree( aMemController, aDynamicMem, sAddr, aEnv );
}

/**
 * @brief 동적 메모리 Allocator를 분석한다.
 * @param[in] aDynamicMem 동적 메모리 할당자
 * @param[out] aTotalSize 메모리 총 크기
 * @param[out] aUsedSize 사용되고 있는 메모리 총 크기
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knmAnalyzeDynamicMem( knlDynamicMem    * aDynamicMem,
                                stlUInt64        * aTotalSize,
                                stlUInt64        * aUsedSize,
                                knlEnv           * aEnv )
{
    stlInt32         sState = 0;
    knmChunkHeader * sChunkHeader;
    knmBlockHeader * sBlockHeader = NULL;
    stlUInt64        sUsedSize = 0;
    stlBool          sIsTimedout;

    if( (aDynamicMem->mType == KNL_MEM_STORAGE_TYPE_SHM) ||
        (aDynamicMem->mType == KNL_MEM_STORAGE_TYPE_PLAN) )
    {
        STL_TRY( knlAcquireLatch( &(aDynamicMem->mLatch),
                                  KNL_LATCH_MODE_SHARED,
                                  KNL_LATCH_PRIORITY_NORMAL,
                                  STL_INFINITE_TIME,
                                  &sIsTimedout,
                                  aEnv ) == STL_SUCCESS );
        sState = 1;
    }

    STL_RING_FOREACH_ENTRY( &(aDynamicMem->mChunkRing), sChunkHeader )
    {
        STL_RING_FOREACH_ENTRY( &(sChunkHeader->mAllocRing), sBlockHeader )
        {
            if( sBlockHeader->mIsUsed == STL_TRUE )
            {
                sUsedSize += ( sBlockHeader->mSize + STL_SIZEOF(knmBlockHeader) );
            }
        }
    }

    *aTotalSize = aDynamicMem->mTotalSize;
    *aUsedSize = sUsedSize;

    if( sState == 1 )
    {
        sState = 0;
        STL_TRY( knlReleaseLatch( &(aDynamicMem->mLatch),
                                  aEnv ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            knlReleaseLatch( &(aDynamicMem->mLatch), aEnv );
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief global dynamic allocator를 등록한다.
 * @param[in] aDynamicMem 동적 메모리 할당자
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knmRegisterDynamicMem( knlDynamicMem * aDynamicMem,
                                 knlEnv        * aEnv )
{
    STL_DASSERT( gKnEntryPoint->mDynamicArrayCount < KNM_MAX_DYNAMIC_ALLOCATOR_COUNT );
    
    gKnEntryPoint->mDynamicMemArray[gKnEntryPoint->mDynamicArrayCount] = aDynamicMem;
    gKnEntryPoint->mDynamicArrayCount += 1;

    return STL_SUCCESS;
}

/**
 * @brief global dynamic allocator를 등록 해제한다.
 * @param[in] aDynamicMem 동적 메모리 할당자
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knmUnregisterDynamicMem( knlDynamicMem * aDynamicMem,
                                   knlEnv        * aEnv )
{
    stlInt32 i;
    
    for( i = 0; i < gKnEntryPoint->mDynamicArrayCount; i++ )
    {
        if( gKnEntryPoint->mDynamicMemArray[i] == aDynamicMem )
        {
            if( i == (gKnEntryPoint->mDynamicArrayCount - 1) )
            {
                gKnEntryPoint->mDynamicMemArray[i] = NULL;
            }
            else
            {
                stlMemmove( &gKnEntryPoint->mDynamicMemArray[i],
                            &gKnEntryPoint->mDynamicMemArray[i+1],
                            ((gKnEntryPoint->mDynamicArrayCount - (i+1)) * STL_SIZEOF(knlDynamicMem*)) );
            }

            break;
        }
    }
    
    gKnEntryPoint->mDynamicArrayCount -= 1;

    return STL_SUCCESS;
}

void knmTestPrintAllocList( knlDynamicMem * aDynamicMem )
{
    knmChunkHeader * sChunkHeader;
    knmBlockHeader * sBlockHeader;
    stlUInt32        sIdx = 0;

    STL_RING_FOREACH_ENTRY( &(aDynamicMem->mChunkRing), sChunkHeader )
    {
        stlPrintf("    ===================================================================\n");
        stlPrintf("    CHUNK #%d[%p~%p] : SIZE[%ld] FREE_SIZE[%ld]\n", sIdx++,
                  ((void*)sChunkHeader - sChunkHeader->mSize),
                  sChunkHeader,
                  sChunkHeader->mSize,
                  sChunkHeader->mFreeBlockSize );
        stlPrintf("    ===================================================================\n");
        
        STL_RING_FOREACH_ENTRY( &(sChunkHeader->mAllocRing), sBlockHeader )
        {
            stlPrintf("        ===============================================================\n");
            stlPrintf("        BLOCK[%p] : SIZE[%ld], Used[%s]\n", sBlockHeader,
                      sBlockHeader->mSize, (sBlockHeader->mIsUsed == STL_TRUE) ? "TRUE" : "FALSE" );
            stlPrintf("        ===============================================================\n");

            if( KNM_CHECK_MEMORY_BOUNDARY(NULL) == STL_TRUE )
            {
                stlFile sOutFile;
                
                stlOpenStdoutFile( &sOutFile, NULL );
                stlBacktraceSymbolsToFile( ((knmHeadFence*)(sBlockHeader + 1))->mCallstack,
                                           ((knmHeadFence*)(sBlockHeader + 1))->mCallstackCount,
                                           &sOutFile );
            }
        }
    }
}

/**
 * @brief 반환된 block 정보를 출력한다.
 * @param[in,out] aDynamicMem 동적 메모리 할당자
 * @internal
 */
void knmTestPrintFreeList( knlDynamicMem * aDynamicMem)
{
    knmChunkHeader * sChunkHeader;
    knmBlockHeader * sBlockHeader;
    stlUInt32        sIdx = 0;

    STL_RING_FOREACH_ENTRY( &(aDynamicMem->mChunkRing), sChunkHeader )
    {
        stlPrintf("    ===================================================================\n");
        stlPrintf("    CHUNK #%d : SIZE[%ld] FREE_SIZE[%ld]\n", sIdx++, sChunkHeader->mSize,
                  sChunkHeader->mFreeBlockSize );
        stlPrintf("    ===================================================================\n");
        
        STL_RING_FOREACH_ENTRY( &(sChunkHeader->mFreeRing), sBlockHeader )
        {
            stlPrintf("        ===============================================================\n");
            stlPrintf("        BLOCK[%p] : SIZE[%ld]\n", sBlockHeader, sBlockHeader->mSize );
            stlPrintf("        ===============================================================\n");
        }
    }
}

/** @} */
