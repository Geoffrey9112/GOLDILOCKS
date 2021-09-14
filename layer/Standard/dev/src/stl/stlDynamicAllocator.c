/*******************************************************************************
 * stlDynamicAllocator.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: stlDynamicAllocator.c 2116 2011-10-20 02:57:10Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stl.h>
#include <sty.h>

/**
 * @file stlDynamicAllocator.c
 * @brief Dynamic Memory Allocator Routines
 */

/**
 * @addtogroup stlDunamicAllocator
 * @{
 */

/**
 * @brief Dynamic Allocator를 생성한다.
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
stlStatus stlCreateDynamicAllocator( stlDynamicAllocator   * aAllocator,
                                     stlUInt64               aInitSize,
                                     stlUInt64               aNextSize,
                                     stlErrorStack         * aErrorStack )
{
    styChunkHeader * sChunkHeader;
    
    STL_PARAM_VALIDATE( aAllocator != NULL, aErrorStack );
    
    aAllocator->mInitSize = aInitSize;
    aAllocator->mNextSize = aNextSize;
    aAllocator->mTotalSize = 0;
    
    STL_RING_INIT_HEADLINK( &(aAllocator->mChunkRing),
                            STL_OFFSETOF(styChunkHeader, mLink) );

    STL_TRY( styAllocChunk( aAllocator,
                            &sChunkHeader,
                            aInitSize,
                            aErrorStack ) == STL_SUCCESS);

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 동적 기반 메모리를 할당한다.
 * @param[in,out] aAllocator  메모리 할당자
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
stlStatus stlAllocDynamicMem( stlDynamicAllocator   * aAllocator,
                              stlInt32                aAllocSize,
                              void                 ** aAddr,
                              stlErrorStack         * aErrorStack )
{
    STL_PARAM_VALIDATE( aAllocator != NULL, aErrorStack );
    
    STL_TRY( styAlloc( aAllocator,
                       STL_ALIGN_DEFAULT(aAllocSize),
                       aAddr,
                       aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 동적 기반 메모리를 할당한다.
 * @param[in,out] aAllocator  메모리 할당자
 * @param[in]     aAddr       할당된 메모리 주소
 * @param[in,out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_OUT_OF_MEMORY]
 *     힙 메모리를 할당받을수 없는 경우에 발생함.
 * @endcode
 * @remark 동시성 제어를 하지 않는다.
 */
stlStatus stlFreeDynamicMem( stlDynamicAllocator  * aAllocator,
                             void                 * aAddr,
                             stlErrorStack        * aErrorStack )
{
    STL_PARAM_VALIDATE( aAllocator != NULL, aErrorStack );
    
    STL_TRY( styFree( aAllocator,
                      aAddr,
                      aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 생성된 모든 동적 기반 메모리를 반환한다.
 * @param[in,out] aAllocator  메모리 할당자
 * @param[in,out] aErrorStack 에러 스택
 * @remark 동시성 제어를 하지 않는다.
 */
stlStatus stlDestroyDynamicAllocator( stlDynamicAllocator * aAllocator,
                                      stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( aAllocator != NULL, aErrorStack );
    
    STL_TRY( styFreeAllChunk( aAllocator,
                              aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
