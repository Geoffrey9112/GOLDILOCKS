/*******************************************************************************
 * sty.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: sty.c 2116 2011-10-20 02:57:10Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stl.h>
#include <sty.h>

/**
 * @brief 메모리 chunk를 할당한다.
 * @param[in]  aAllocator    동적 메모리 할당자
 * @param[out] aChunkHeader  할당된 chunk의 헤더
 * @param[in]  aChunkSize    할당될 chunk의 크기
 * @param[in]  aErrorStack   에러 스택
 * @par Error Codes :
 * @code
 * [STL_ERRCODE_OUT_OF_MEMORY]
 *     힙 메모리를 할당받을수 없는 경우에 발생함.
 * @endcode
 */
stlStatus styAllocChunk( stlDynamicAllocator   * aAllocator,
                         styChunkHeader       ** aChunkHeader,
                         stlInt64                aChunkSize,
                         stlErrorStack         * aErrorStack )
{
    stlChar        * sAddr;
    styChunkHeader * sHeader;
    stlUInt64        sAllocSize;

    sAllocSize = aChunkSize + STL_SIZEOF(styChunkHeader);
    
    STL_TRY( stlAllocHeap( (void**)&sAddr,
                           sAllocSize,
                           aErrorStack ) == STL_SUCCESS );

    aAllocator->mTotalSize = aAllocator->mTotalSize + sAllocSize;

    /*
     * chunk header 초기화
     */

    /*
     * chunk header는 할당 받은 메모리의 제일 마지막에 위치한다.
     */
    sHeader = (styChunkHeader*)(sAddr + aChunkSize);
    sHeader->mSize = aChunkSize;
    sHeader->mFreeBlockCount = 0;
    sHeader->mFreeBlockSize  = 0;

    STL_RING_INIT_DATALINK( sHeader,
                            STL_OFFSETOF(styChunkHeader, mLink) );

    STL_RING_INIT_HEADLINK( (&sHeader->mAllocRing),
                            STL_OFFSETOF(styBlockHeader, mAllocLink) );

    STL_RING_INIT_HEADLINK( (&sHeader->mFreeRing),
                            STL_OFFSETOF(styBlockHeader, mFreeLink) );

    STL_RING_ADD_FIRST( &(aAllocator->mChunkRing), sHeader );

    *aChunkHeader = sHeader;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 메모리 chunk를 반환한다.
 * @param[in]  aAllocator    동적 메모리 할당자
 * @param[in]  aChunkHeader  반환할 chunk의 헤더
 * @param[in]  aErrorStack   에러 스택
 */
stlStatus styFreeChunk( stlDynamicAllocator  * aAllocator,
                        styChunkHeader       * aChunkHeader,
                        stlErrorStack        * aErrorStack )
{
    stlChar   * sAddr;
    stlUInt64   sChunkSize;

    STL_RING_UNLINK( &(aAllocator->mChunkRing), aChunkHeader );

    sChunkSize = aChunkHeader->mSize;
    sAddr = (stlChar*)aChunkHeader - sChunkSize;

    stlFreeHeap( sAddr );

    aAllocator->mTotalSize = ( aAllocator->mTotalSize -
                               sChunkSize -
                               STL_SIZEOF(styChunkHeader) );
    
    return STL_SUCCESS;
}

/**
 * @brief 모든 메모리 chunk를 반환한다.
 * @param[in]  aAllocator    동적 메모리 할당자
 * @param[in]  aErrorStack   에러 스택
 */
stlStatus styFreeAllChunk( stlDynamicAllocator  * aAllocator,
                           stlErrorStack        * aErrorStack )
{
    styChunkHeader * sChunkHeader;
    styChunkHeader * sNextHeader;

    STL_RING_FOREACH_ENTRY_SAFE( &(aAllocator->mChunkRing),
                                 sChunkHeader,
                                 sNextHeader )
    {
        STL_TRY( styFreeChunk( aAllocator,
                               sChunkHeader,
                               aErrorStack ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief chunk의 free list에서 할당 가능한 block을 찾는다.
 * @param[in]  aChunkHeader  할당 가능한 block을 찾을 chunk의 header
 * @param[in]  aSize         할당할 메모리 크기
 * @param[out] aFreeBlock    할당 가능한 메모리
 * @param[in]  aErrorStack   에러 스택
 */
stlStatus styFindInFreeList( styChunkHeader  * aChunkHeader,
                             stlUInt32         aSize,
                             styBlockHeader ** aFreeBlock,
                             stlErrorStack   * aErrorStack )
{
    styBlockHeader * sBlockHeader;

    *aFreeBlock = NULL;

    STL_TRY( aChunkHeader->mFreeBlockCount > 0 );

    /*
     * first fit 알고리즘을 사용한다.
     */

    STL_RING_FOREACH_ENTRY( &(aChunkHeader->mFreeRing), sBlockHeader )
    {
        STL_ASSERT( sBlockHeader->mIsUsed == STL_FALSE );

        if( sBlockHeader->mSize == aSize )
        {
            *aFreeBlock = sBlockHeader;
            break;
        }

        if( sBlockHeader->mSize >= (aSize + STL_SIZEOF(styBlockHeader)) )
        {
            *aFreeBlock = sBlockHeader;
            break;
        }
    }

    STL_TRY( STL_RING_IS_HEADLINK( &(aChunkHeader->mFreeRing),
                                   sBlockHeader )
             == STL_FALSE );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief free block을 사용해 메모리를 할당한다.
 * @param[in]  aChunkHeader  할당 가능한 block을 찾을 chunk의 header
 * @param[in]  aSize         할당할 메모리 크기
 * @param[in]  aFreeBlock    할당 가능한 free block
 * @param[out] aAllocBlock   할당한 메모리
 * @param[in]  aErrorStack   에러 스택
 */
stlStatus styAllocInFreeBlock( styChunkHeader  * aChunkHeader,
                               stlUInt32         aSize,
                               styBlockHeader  * aFreeBlock,
                               styBlockHeader ** aAllocBlock,
                               stlErrorStack   * aErrorStack )
{
    styBlockHeader * sNewBlockHeader;
    styBlockHeader * sAllocBlockHeader;

    sAllocBlockHeader = aFreeBlock;

    STL_RING_UNLINK( &(aChunkHeader->mFreeRing),
                     sAllocBlockHeader );

    aChunkHeader->mFreeBlockCount--;

    
    if( aSize == aFreeBlock->mSize )
    {
        aChunkHeader->mFreeBlockSize = aChunkHeader->mFreeBlockSize - aSize;
    }
    else
    {
        /*
         * 메모리 할당 후 남은 block 초기화
         */
        sNewBlockHeader = (styBlockHeader*)( (stlChar*)aFreeBlock +
                                             STL_SIZEOF( styBlockHeader ) +
                                             aSize );

        STL_RING_INIT_DATALINK( sNewBlockHeader, STL_OFFSETOF( styBlockHeader,
                                                               mAllocLink) );
        STL_RING_INIT_DATALINK( sNewBlockHeader, STL_OFFSETOF( styBlockHeader,
                                                               mFreeLink) );

        sNewBlockHeader->mSize   = aFreeBlock->mSize - aSize - STL_SIZEOF( styBlockHeader );
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
        
        sAllocBlockHeader->mSize = aSize;

        aChunkHeader->mFreeBlockSize = ( aChunkHeader->mFreeBlockSize -
                                         aSize -
                                         STL_SIZEOF(styBlockHeader) );
    }

    /*
     * 할당된 block 초기화
     */
    STL_RING_INIT_DATALINK( sAllocBlockHeader, STL_OFFSETOF( styBlockHeader,
                                                             mFreeLink ) );

    sAllocBlockHeader->mIsUsed = STL_TRUE;

    *aAllocBlock = sAllocBlockHeader;

    return STL_SUCCESS;
}

/**
 * @brief 아직 chunk에서 사용되지 않은 메모리 영역에서 할당한다.
 * @param[in]  aChunkHeader  메모리를 할당할 chunk의 header
 * @param[in]  aSize         할당할 메모리 크기
 * @param[out] aAllocBlock   할당한 메모리
 * @param[in]  aErrorStack   에러 스택
 */
stlStatus styAllocInChunk( styChunkHeader  * aChunkHeader,
                           stlUInt32         aSize,
                           styBlockHeader ** aAllocBlock,
                           stlErrorStack   * aErrorStack )
{
    styBlockHeader * sLastBlockHeader;
    styBlockHeader * sNewBlockHeader;

    /*
     * 새로운 block의 header는 할당된 메모리의 제일 앞에 존재한다.
     */
    sLastBlockHeader = (styBlockHeader*)STL_RING_GET_LAST_DATA( &(aChunkHeader->mAllocRing) );
    sNewBlockHeader = (styBlockHeader*)((stlChar*)sLastBlockHeader -
                                        aSize -
                                        STL_SIZEOF( styBlockHeader ));

    STL_RING_INIT_DATALINK( sNewBlockHeader,
                            STL_OFFSETOF( styBlockHeader, mAllocLink ) );

    STL_RING_INIT_DATALINK( sNewBlockHeader,
                            STL_OFFSETOF( styBlockHeader, mFreeLink ) );

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
 * @param[in]  aAllocator     동적 메모리 할당자
 * @param[in]  aSize          할당할 메모리 크기
 * @param[out] aFreeBlock     할당 가능한 메모리
 * @param[in]  aErrorStack    에러 스택
 */
stlStatus styFindFreeSpace( stlDynamicAllocator  * aAllocator,
                            stlUInt32              aSize,
                            styBlockHeader      ** aFreeBlock,
                            stlErrorStack        * aErrorStack )
{
    styChunkHeader * sChunkHeader;
    styBlockHeader * sBlockHeader;
    stlInt64         sRemain;
    stlInt64         sSize;

    STL_RING_FOREACH_ENTRY( &(aAllocator->mChunkRing), sChunkHeader )
    {
        if( sChunkHeader->mFreeBlockCount > 0 )
        {
            /*
             * 현재 chunk의 모든 free block을 검사해 할당 가능한 block이 있는지 찾는다.
             */
            if( styFindInFreeList( sChunkHeader,
                                   aSize,
                                   &sBlockHeader,
                                   aErrorStack ) == STL_SUCCESS )
            {
                STL_TRY( styAllocInFreeBlock( sChunkHeader,
                                              aSize,
                                              sBlockHeader,
                                              aFreeBlock,
                                              aErrorStack ) == STL_SUCCESS );

                break;
            }
        }

        /*
         * 할당할 수 있는 free block이 존재하지 않으므로 새로운 block을 할당할 수 있는지 확인한다.
         * 현재까지 할당된 크기 : chunk header pointer - last alloc block pointer
         */

        sBlockHeader = (styBlockHeader*)STL_RING_GET_LAST_DATA( &(sChunkHeader->mAllocRing) );

        sRemain = sChunkHeader->mSize - STL_DIFF_POINTER( sChunkHeader, sBlockHeader );
        sSize   = aSize + STL_SIZEOF(styBlockHeader);

        STL_DASSERT( sRemain >= 0 );

        if( sRemain >= sSize )
        {
            STL_TRY( styAllocInChunk( sChunkHeader,
                                      aSize,
                                      aFreeBlock,
                                      aErrorStack ) == STL_SUCCESS );
            break;
        }

        /*
         * 현재 chunk에서는 할당 할 수 없으므로 다음 chunk를 확인한다.
         */
    }

    STL_TRY( STL_RING_IS_HEADLINK( &(aAllocator->mChunkRing),
                                   sChunkHeader ) == STL_FALSE );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 입력 받은 block이 위치한 chunk를 찾는다.
 * @param[in]  aAllocator      동적 메모리 할당자
 * @param[in]  aBlockHeader    chunk를 찾을 block 
 * @param[out] aChunkHeader @a aBlockHeader 이 위치한 chunk header
 * @param[out] aIsDefaultChunk 반환할 chunk가 default chunk인지 여부
 * @param[in]  aErrorStack     에러 스택
 */
stlStatus styFindChunk( stlDynamicAllocator  * aAllocator,
                        styBlockHeader       * aBlockHeader,
                        styChunkHeader      ** aChunkHeader,
                        stlBool              * aIsDefaultChunk,
                        stlErrorStack        * aErrorStack )
{
    styChunkHeader * sChunkHeader;
    stlUInt32        sIdx = 0;

    *aIsDefaultChunk = STL_FALSE;

    STL_RING_FOREACH_ENTRY_REVERSE( &(aAllocator->mChunkRing), sChunkHeader )
    {
        if (((stlChar*)aBlockHeader < (stlChar*)sChunkHeader) &&
            ((stlChar*)aBlockHeader >= (stlChar*)sChunkHeader - sChunkHeader->mSize))
        {
            if( sIdx == 0 )
            {
                *aIsDefaultChunk = STL_TRUE;
            }

            break;
        }

        sIdx++;
    }

    STL_DASSERT( STL_RING_IS_HEADLINK( &(aAllocator->mChunkRing),
                                       sChunkHeader )
                 == STL_FALSE );

    *aChunkHeader = sChunkHeader;

    return STL_SUCCESS;
}

/**
 * @brief 동적 메모리를 할당한다.
 * @param[in]  aAllocator      동적 메모리 할당자
 * @param[in]  aSize           할당할 메모리 크기
 * @param[out] aAddr           할당된 메모리 주소
 * @param[in]  aErrorStack     에러 스택
 * @par Error Codes :
 * @code
 * [STL_ERRCODE_OUT_OF_MEMORY]
 *     힙 메모리를 할당받을수 없는 경우에 발생함.
 * @endcode
 */
stlStatus styAlloc( stlDynamicAllocator  * aAllocator,
                    stlUInt32              aSize,
                    void                ** aAddr,
                    stlErrorStack        * aErrorStack )
{
    styChunkHeader * sChunkHeader;
    styBlockHeader * sBlockHeader = NULL;
    stlUInt32        sSize = aSize;

    /*
     * 이미 할당된 chunk에서 할당 가능한 block이 없을 경우
     * 새로운 chunk를 할당해 메모리를 할당한다.
     */
    if( styFindFreeSpace( aAllocator,
                          sSize,
                          &sBlockHeader,
                          aErrorStack ) == STL_FAILURE )
    {
        STL_DASSERT( sSize < 300 * 1024 * 1024 );

        /*
         * 새로 할당할 chunk는 최소 ( 요청한 메모리 크기 + styBlockHeader 크기) 만큼 필요하다.
         */
        STL_TRY( styAllocChunk( aAllocator,
                                &sChunkHeader,
                                STL_ALIGN( sSize + STL_SIZEOF(styBlockHeader), aAllocator->mNextSize ),
                                aErrorStack ) == STL_SUCCESS );

        STL_TRY( styAllocInChunk( sChunkHeader,
                                  sSize,
                                  &sBlockHeader,
                                  aErrorStack ) == STL_SUCCESS );
    }

    STL_DASSERT( sBlockHeader != NULL );

    *aAddr = (stlChar*)sBlockHeader + STL_SIZEOF(styBlockHeader);

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 동적 메모리를 반납한다.
 * @param[in]  aAllocator      동적 메모리 할당자
 * @param[in]  aAddr           반환할 메모리 주소
 * @param[in]  aErrorStack     에러 스택
 */
stlStatus styFree( stlDynamicAllocator  * aAllocator,
                   void                 * aAddr,
                   stlErrorStack        * aErrorStack )
{
    styChunkHeader * sChunkHeader;
    styBlockHeader * sBlockHeader;
    styBlockHeader * sPrevHeader;
    styBlockHeader * sNextHeader;
    stlBool          sIsDefault;

    /*
     * 입력받은 메모리가 위치한 chunk를 찾는다.
     */
    sBlockHeader = (styBlockHeader*)((stlChar*)aAddr - STL_SIZEOF(styBlockHeader));

    STL_TRY( styFindChunk( aAllocator,
                           sBlockHeader,
                           &sChunkHeader,
                           &sIsDefault,
                           aErrorStack ) == STL_SUCCESS );

    STL_DASSERT( sBlockHeader->mIsUsed == STL_TRUE );

    sBlockHeader->mIsUsed = STL_FALSE;

    /*
     * 반환되는 메모리를 free link의 맨 앞에 추가한다.
     */
    STL_RING_ADD_FIRST( &(sChunkHeader->mFreeRing),
                        sBlockHeader );

    sChunkHeader->mFreeBlockCount++;
    sChunkHeader->mFreeBlockSize = ( sChunkHeader->mFreeBlockSize +
                                     STL_SIZEOF(styBlockHeader) +
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
                                    STL_SIZEOF(styBlockHeader) );

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

            sNextHeader->mSize = ( sNextHeader->mSize +
                                   sBlockHeader->mSize +
                                   STL_SIZEOF(styBlockHeader) );

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
                                             STL_SIZEOF(styBlockHeader) -
                                             sBlockHeader->mSize );
        }
        else
        {
            break;
        }
    }

    /*
     * 기본 chunk가 아니면서 모든 block이 사용중이지 않을 경우 현재 chunk를 제거한다.
     */
    if( sIsDefault == STL_FALSE )
    {
        sBlockHeader = (styBlockHeader*)STL_RING_GET_LAST_DATA( &(sChunkHeader->mAllocRing) );
        
        if( STL_RING_IS_HEADLINK( &(sChunkHeader->mAllocRing),
                                  sBlockHeader)
            == STL_TRUE )
        {
            STL_TRY( styFreeChunk( aAllocator,
                                   sChunkHeader,
                                   aErrorStack ) == STL_SUCCESS );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

