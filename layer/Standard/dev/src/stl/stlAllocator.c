/*******************************************************************************
 * stlAllocator.c
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

#include <stlDef.h>
#include <stlMemorys.h>
#include <stlStrings.h>
#include <stlError.h>
#include <stz.h>

/**
 * @file stlAllocator.c
 * @brief Memory Allocator Routines
 */

/**
 * @addtogroup stlAllocator
 * @{
 */

/**
 * @brief Region Allocator를 생성한다.
 * @param[out]    aAllocator  초기화할 Allocator 주소
 * @param[in]     aInitSize   미리 할당할 메모리의 크기
 * @param[in]     aNextSize   추가적으로 메모리 할당시 할당할 메모리의 크기
 * @param[in,out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_OUT_OF_MEMORY]
 *     힙 메모리를 할당받을수 없는 경우에 발생함.
 * @endcode
 * @remark 동시성 제어를 하지 않는다.
 */
stlStatus stlCreateRegionAllocator( stlAllocator   * aAllocator,
                                    stlUInt64        aInitSize,
                                    stlUInt64        aNextSize,
                                    stlErrorStack  * aErrorStack )
{
    aAllocator->mInitSize = STL_ALIGN_DEFAULT( aInitSize );
    aAllocator->mNextSize = STL_ALIGN_DEFAULT( aNextSize );
    aAllocator->mTotalChunkCount = 0;
    aAllocator->mUsedChunkCount = 0;
    aAllocator->mCurChunk = NULL;
    aAllocator->mFirstChunk = NULL;
    aAllocator->mLastChunk = NULL;

    STL_TRY( stzAllocChunk( aAllocator->mInitSize,
                            &aAllocator->mFirstChunk,
                            aErrorStack )
             == STL_SUCCESS );

    aAllocator->mLastChunk = aAllocator->mFirstChunk;
    aAllocator->mCurChunk = aAllocator->mFirstChunk;
    aAllocator->mUsedChunkCount = 1;
    aAllocator->mTotalChunkCount = 1;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 영역 기반 메모리를 할당한다.
 * @param[in,out] aAllocator  영역 기반 메모리 할당자
 * @param[in]     aAllocSize  할당할 메모리 크기
 * @param[in]     aAddr       할당된 메모리 주소
 * @param[in,out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_OUT_OF_MEMORY]
 *     힙 메모리를 할당받을수 없는 경우에 발생함.
 * @endcode
 * @remark 동시성 제어를 하지 않는다.
 */
stlStatus stlAllocRegionMem( stlAllocator   * aAllocator,
                             stlInt32         aAllocSize,
                             void          ** aAddr,
                             stlErrorStack  * aErrorStack )
{
    stlInt32 sAllocSize;
    
    sAllocSize = STL_ALIGN_DEFAULT( aAllocSize );

    STL_TRY( stzAllocRegionMem( aAllocator,
                                aAddr,
                                sAllocSize,
                                aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 생성된 모든 영역 기반 메모리를 정리한다.
 * @param[in,out] aAllocator  영역 기반 메모리 할당자
 * @param[in,out] aErrorStack 에러 스택
 * @remark 동시성 제어를 하지 않는다.
 * <BR> 최초 생성된 Chunk는 삭제하지 않는다.
 */
stlStatus stlClearRegionMem( stlAllocator   * aAllocator,
                             stlErrorStack  * aErrorStack )
{
    STL_TRY( stzClearRegionMem( aAllocator,
                                aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 현재까지 사용한 영역 기반 메모리의 정보를 저장한다.
 * @param[in,out] aAllocator  영역 기반 메모리 할당자
 * @param[out]    aMark       현재까지 사용한 영역 기반 메모리의 정보
 * @param[in,out] aErrorStack 에러 스택
 * @remark 동시성 제어를 하지 않는다.
 */
stlStatus stlMarkRegionMem( stlAllocator   * aAllocator,
                            stlRegionMark  * aMark,
                            stlErrorStack  * aErrorStack )
{
    STL_TRY( stzMarkRegionMem( aAllocator,
                               aMark,
                               aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 영역 기반 메모리를 이전 상태로 되돌린다.
 * @param[in,out] aAllocator  영역 기반 메모리 할당자
 * @param[in]     aMark       이전의 영역 기반 메모리의 정보
 * @param[in]     aFree       aMark 이후에 할당한 메모리의 시스템 반환 여부
 * @param[in,out] aErrorStack 에러 스택
 * @remark 동시성 제어를 하지 않는다.
 */
stlStatus stlRestoreRegionMem( stlAllocator   * aAllocator,
                               stlRegionMark  * aMark,
                               stlBool          aFree,
                               stlErrorStack  * aErrorStack )
{
    STL_TRY( stzRestoreRegionMem( aAllocator,
                                  aMark,
                                  aFree,
                                  aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 생성된 모든 영역 기반 메모리를 반환한다.
 * @param[in,out] aAllocator  영역 기반 메모리 할당자
 * @param[in,out] aErrorStack 에러 스택
 * @remark 동시성 제어를 하지 않는다.
 */
stlStatus stlDestroyRegionAllocator( stlAllocator   * aAllocator,
                                     stlErrorStack  * aErrorStack )
{
    STL_TRY( stzDestroyRegionMem( aAllocator,
                                  aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief array를 이용한 메모리 관리자를 초기화한다.
 *        Element body 크기와 Element 개수를 입력하여 총 메모리 사용량이 정해진다.
 * @param[in]  aAllocator         array allocator
 * @param[in]  aElementBodySize   Data size
 * @param[in]  aElementCount      Element count
 * @param[out] aTotalUsedBytes    array allocator에서 총 사용하는 byte
 * @param[in]  aAlignment         Element alignment size
 * @param[in]  aErrorStack        에러 스택
 * @remark 배열메모리를 이용하여 고정크기를 사용한다.
 * 사용법 : aElementCount로 메모리 사용량을 계산하여 aUsedMemory로 output한다.
 */
stlStatus stlInitializeArrayAllocator( stlArrayAllocator    * aAllocator,
                                       stlInt64               aElementBodySize,
                                       stlInt64               aElementCount,
                                       stlInt64             * aTotalUsedBytes,
                                       stlSize                aAlignment,
                                       stlErrorStack        * aErrorStack )
{
    return stzInitializeArrayAllocator( aAllocator,
                                        aElementBodySize,
                                        aElementCount,
                                        aTotalUsedBytes,
                                        aAlignment,
                                        aErrorStack );
}


/**
 * @brief array를 이용한 메모리 관리자를 초기화한다.
 *        array 전체크기를 입력받고 Element 개수는 자동으로 정해진다.
 * @param[in]  aAllocator       array allocator
 * @param[in]  aElementBodySize Data size
 * @param[in]  aTotalUsedBytes  array allocator에서 총 사용하는 byte
 * @param[out] aElementCount    Element count
 * @param[in]  aAlignment       Element alignment size
 * @param[in]  aErrorStack      에러 스택
 * @remark 배열메모리를 이용하여 고정크기를 사용한다.
 * 사용법  총 사용할 메모리 크기를 입력하고 Element 개수는 설정되어 출력된다.
 */
stlStatus stlInitializeArrayAllocatorByTotalSize( stlArrayAllocator    * aAllocator,
                                                  stlInt64               aElementBodySize,
                                                  stlInt64               aTotalUsedBytes,
                                                  stlInt64             * aElementCount,
                                                  stlSize                aAlignment,
                                                  stlErrorStack        * aErrorStack )
{
    stlStatus   sResult;
    stlInt64    sTotalUsedBytes = 0;
    stlInt64    sElementSize = 0;

    /**
     * stzInitializeArrayAllocator 내부에서 cache line padding을 함으로
     * 여기에서도 cache line을 고려하여 aElementCount를 계산해야 한다.
     */
    sElementSize = STL_ALIGN(aElementBodySize + STL_SIZEOF(stlArrayElement), aAlignment);

    /* 메모리로 Element count계산 */
    *aElementCount = (aTotalUsedBytes - STL_SIZEOF(stlArrayAllocator)) / sElementSize;

    sResult = stzInitializeArrayAllocator( aAllocator,
                                           aElementBodySize,
                                           *aElementCount,
                                           &sTotalUsedBytes,
                                           aAlignment,
                                           aErrorStack );

    STL_DASSERT( aTotalUsedBytes >= sTotalUsedBytes );

    return sResult;
}

/**
 * @brief array를 이용한 메모리 관리자를 종료한다.
 * @param[in]  aAllocator   array allocator
 * @param[in]  aErrorStack  에러 스택
 */
stlStatus stlFinalizeArrayAllocator( stlArrayAllocator    * aAllocator,
                                     stlErrorStack        * aErrorStack )
{
    return stzFinalizeArrayAllocator( aAllocator,
                                      aErrorStack );
}

/**
 * @brief array를 이용한 메모리 관리자에서 메모리를 할당한다.
 * @param[in]  aAllocator   array allocator
 * @param[in]  aItem        할당된 메모리를 저장할 address
 * @param[in]  aErrorStack  에러 스택
 */
stlStatus stlAllocElement( stlArrayAllocator   * aAllocator,
                           void               ** aItem,
                           stlErrorStack       * aErrorStack )
{
    return stzAllocElement( aAllocator,
                            aItem,
                            aErrorStack );
}

/**
 * @brief array를 이용한 메모리 관리자에서 메모리를 해제한다.
 * @param[in]  aAllocator   array allocator
 * @param[in]  aItem        반환할 메모리
 * @param[in]  aErrorStack  에러 스택
 */
stlStatus stlFreeElement( stlArrayAllocator   * aAllocator,
                          void                * aItem,
                          stlErrorStack       * aErrorStack )
{
    return stzFreeElement( aAllocator,
                           aItem,
                           aErrorStack );
}

/**
 * @brief array를 이용한 메모리 관리자의 정보를 가져온다
 * @param[in]  aAllocator           array allocator
 * @param[in]  aTotalUsedBytes      전체 사용중인 array크기
 * @param[in]  aTotalElementCount   전체 Element 개수
 * @param[in]  aUsedCount           사용중인 Element개수
 * @param[in]  aElementBodySize     하나의 Element에서 사용자가 사용가능한 buffer 크기(Element의 body크기)
 * @param[in]  aElementSize         header를 포함한 Element크기
 * @param[in]  aErrorStack          에러 스택
 * @remark aUsedCount는 allocator의 메모리를 full scan함으로 비용이 비쌈.\n
 * aUsedMemoryByte값은 stlInitializeArrayAllocatorByTotalSize에 input으로 넣은 aUsedMemoryByte와 다를수 있음.\n
 * 여기서 출력되는 aUsedMemoryByte값이 실제 사용되는 메모리 크기임.
 */
stlStatus stlGetArrayInfo( stlArrayAllocator   * aAllocator,
                           stlInt32            * aTotalUsedBytes,
                           stlInt32            * aTotalElementCount,
                           stlInt32            * aUsedCount,
                           stlInt32            * aElementBodySize,
                           stlInt32            * aElementSize,
                           stlErrorStack       * aErrorStack )
{
    return stzGetInfoArray( aAllocator,
                            aTotalUsedBytes,
                            aTotalElementCount,
                            aUsedCount,
                            aElementBodySize,
                            aElementSize,
                            aErrorStack );
}


/** @} */
