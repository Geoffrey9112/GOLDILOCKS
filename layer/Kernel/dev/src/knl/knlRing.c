/*******************************************************************************
 * knlRing.c
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
 * @file knlRing.c
 * @brief Kernel Layer Ring Routines
 */

#include <stl.h>
#include <dtl.h>
#include <knlDef.h>
#include <knlRing.h>
#include <knlShmManager.h>

/**
 * @addtogroup knlLogicalRing
 * @{
 */

/**
 * @brief Ring의 처음 객체의 논리적 포인터를 반환한다.
 * @param[in] aHead Ring Head의 논리적 포인터
 * @param[out] aPointer 처음 객체의 논리적 포인터
 * @param[in,out] aEnv 커널 Environment
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_INITIALIZED_YET]
 *     knlInitialize() 함수가 호출되지 않았음.
 * [KNL_ERRCODE_INVALID_ARGUMENT]
 *     입력받은 논리적 포인터가 올바르지 않음.
 * @endcode
 * @internal
 */
stlStatus knlGetFirstRingInternal( knlLogicalAddr   aHead,
                                   knlLogicalAddr * aPointer,
                                   knlEnv         * aEnv )
{
    knlRingHead * sHead;

    STL_TRY( knlGetPhysicalAddr( aHead,
                                 (void**)&sHead,
                                 aEnv ) == STL_SUCCESS );

    *aPointer = sHead->mNext;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Ring의 마지막 객체의 논리적 포인터를 반환한다.
 * @param[in] aHead Ring Head의 논리적 포인터
 * @param[out] aPointer 마지막 객체의 논리적 포인터
 * @param[in,out] aEnv 커널 Environment
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_INITIALIZED_YET]
 *     knlInitialize() 함수가 호출되지 않았음.
 * [KNL_ERRCODE_INVALID_ARGUMENT]
 *     입력받은 논리적 포인터가 올바르지 않음.
 * @endcode
 * @internal
 */
stlStatus knlGetLastRingInternal( knlLogicalAddr   aHead,
                                  knlLogicalAddr * aPointer,
                                  knlEnv         * aEnv )
{
    knlRingHead * sHead;

    STL_TRY( knlGetPhysicalAddr( aHead,
                                 (void**)&sHead,
                                 aEnv ) == STL_SUCCESS );

    *aPointer = sHead->mPrev;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 입력 받은 객체를 통해 다음 객체의 논리적 포인터를 반환한다.
 * @param[in] aElementPointer 현재 객체의 논리적 포인터
 * @param[out] aPointer 다음 객체의 논리적 포인터
 * @param[in] aOffset 연결 객체에서 STL_RING_ENTRY의 offset
 * @param[in,out] aEnv 커널 Environment
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_INITIALIZED_YET]
 *     knlInitialize() 함수가 호출되지 않았음.
 * [KNL_ERRCODE_INVALID_ARGUMENT]
 *     입력받은 논리적 포인터가 올바르지 않음.
 * @endcode
 * @internal
 */
stlStatus knlGetNextRingInternal( knlLogicalAddr   aElementPointer,
                                  knlLogicalAddr * aPointer,
                                  stlSize          aOffset,
                                  knlEnv         * aEnv )
{
    void         * sElement;
    knlRingEntry * sEntry;

    STL_TRY( knlGetPhysicalAddr( aElementPointer,
                                 &sElement,
                                 aEnv ) == STL_SUCCESS );

    sEntry = (knlRingEntry*)((stlChar*)sElement + aOffset);

    *aPointer = sEntry->mNext;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 입력 받은 객체를 통해 이전 객체의 논리적 포인터를 반환한다.
 * @param[in] aElementPointer 현재 객체의 논리적 포인터
 * @param[out] aPointer 이전 객체의 논리적 포인터
 * @param[in] aOffset 연결 객체에서 STL_RING_ENTRY의 offset
 * @param[in,out] aEnv 커널 Environment
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_INITIALIZED_YET]
 *     knlInitialize() 함수가 호출되지 않았음.
 * [KNL_ERRCODE_INVALID_ARGUMENT]
 *     입력받은 논리적 포인터가 올바르지 않음.
 * @endcode
 * @internal
 */
stlStatus knlGetPrevRingInternal( knlLogicalAddr   aElementPointer,
                                  knlLogicalAddr * aPointer,
                                  stlSize          aOffset,
                                  knlEnv         * aEnv )
{
    void         * sElement;
    knlRingEntry * sEntry;

    STL_TRY( knlGetPhysicalAddr( aElementPointer,
                                 &sElement,
                                 aEnv ) == STL_SUCCESS );

    sEntry = (knlRingEntry*)((stlChar*)sElement + aOffset);

    *aPointer = sEntry->mPrev;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Ring을 초기화 한다.
 * @param[in] aHead Ring Head의 논리적 포인터
 * @param[in] aOffset 연결 객체에서 STL_RING_ENTRY의 offset
 * @param[in,out] aEnv 커널 Environment
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_INITIALIZED_YET]
 *     knlInitialize() 함수가 호출되지 않았음.
 * [KNL_ERRCODE_INVALID_ARGUMENT]
 *     입력받은 논리적 포인터가 올바르지 않음.
 * @endcode
 * @remark Ring Head에는 링크 정보만 유지하기 위해 Ring Head를 통해
 *         가상의 연결 객체의 논리적 포인터를 만든다.
 * <PRE>
 *        last                            first
 *     +->+------+<-+  +->sentinel<-+  +->+------+<-+
 *     |  |struct|  |  |            |  |  |struct|  |
 *    /   | elem |   \/              \/   | elem |  \
 * ...    |      |   /\              /\   |      |   ...
 *        +------+  |  |  +------+  |  |  +------+
 *   ...--|prev  |  |  +--|ring  |  |  +--|prev  |
 *        |  next|--+     |  head|--+     |  next|--...
 *        +------+        +------+        +------+
 *        | etc. |                        | etc. |
 *        :      :                        :      : 
 * </PRE>
 * @internal
 */
stlStatus knlInitRingInternal( knlLogicalAddr   aHead,
                               stlSize          aOffset,
                               knlEnv         * aEnv )
{
    knlRingHead  * sHead;
    knlLogicalAddr sPhantom;

    STL_TRY( knlGetPhysicalAddr( aHead,
                                 (void**)&sHead,
                                 aEnv ) == STL_SUCCESS );

    sPhantom = aHead - aOffset;

    sHead->mNext = sPhantom;
    sHead->mPrev = sPhantom;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;    
}

/**
 * @brief Ring이 비었는지 확인한다.
 * @param[in] aHead Ring Head의 논리적 포인터
 * @param[out] aIsEmpty Ring이 비었는지 여부
 * @param[in] aOffset 연결 객체에서 STL_RING_ENTRY의 offset
 * @param[in,out] aEnv 커널 Environment
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_INITIALIZED_YET]
 *     knlInitialize() 함수가 호출되지 않았음.
 * [KNL_ERRCODE_INVALID_ARGUMENT]
 *     입력받은 논리적 포인터가 올바르지 않음.
 * @endcode
 * @internal
 */
stlStatus knlIsEmptyRingInternal( knlLogicalAddr   aHead,
                                  stlBool        * aIsEmpty,
                                  stlSize          aOffset,
                                  knlEnv         * aEnv )
{
    knlRingHead * sHead;

    STL_TRY( knlGetPhysicalAddr( aHead,
                                 (void**)&sHead,
                                 aEnv ) == STL_SUCCESS );

    if( sHead->mNext == (aHead - aOffset) )
    {
        *aIsEmpty = STL_TRUE;
    }
    else
    {
        *aIsEmpty = STL_FALSE;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;    
}

/**
 * @brief @a aElement 앞에 @a aFirstElement .. @a aLastElement 를 연결한다.
 * @param[in] aElement 기준이 되는 aElement의 논리적 포인터
 * @param[in] aFirstElement 연결되는 맨 처음 객체의 논리적 포인터
 * @param[in] aLastElement 연결되는 맨 마지막 객체의 논리적 포인터
 * @param[in] aOffset 연결 객체에서 STL_RING_ENTRY의 offset
 * @param[in,out] aEnv 커널 Environment
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_INITIALIZED_YET]
 *     knlInitialize() 함수가 호출되지 않았음.
 * [KNL_ERRCODE_INVALID_ARGUMENT]
 *     입력받은 논리적 포인터가 올바르지 않음.
 * @endcode
 * @remark ..aElement.. 가 ..aFirstElement..aLastElement..aElement.. 으로 연결된다.
 * @internal
 */
stlStatus knlSpliceRingBeforeInternal( knlLogicalAddr  aElement,
                                       knlLogicalAddr  aFirstElement,
                                       knlLogicalAddr  aLastElement,
                                       stlSize         aOffset,
                                       knlEnv        * aEnv )
{
    void         * sElement;
    void         * sFirstElement;
    void         * sLastElement;
    void         * sElementPrev;

    knlRingEntry * sElementEntry;
    knlRingEntry * sFirstElementEntry;
    knlRingEntry * sLastElementEntry;
    knlRingEntry * sElementPrevEntry;
    
    STL_TRY( knlGetPhysicalAddr( aElement,
                                 &sElement,
                                 aEnv ) == STL_SUCCESS );

    sElementEntry = (knlRingEntry*)((stlChar*)sElement + aOffset);

    STL_TRY( knlGetPhysicalAddr( aFirstElement,
                                 &sFirstElement,
                                 aEnv ) == STL_SUCCESS );

    sFirstElementEntry = (knlRingEntry*)((stlChar*)sFirstElement + aOffset);

    if(aFirstElement == aLastElement)
    {
        sLastElementEntry = sFirstElementEntry;
    }
    else
    {
        STL_TRY( knlGetPhysicalAddr( aLastElement,
                                     &sLastElement,
                                     aEnv ) == STL_SUCCESS );

        sLastElementEntry = (knlRingEntry*)((stlChar*)sLastElement + aOffset);
    }

    STL_TRY( knlGetPhysicalAddr( sElementEntry->mPrev,
                                 &sElementPrev,
                                 aEnv ) == STL_SUCCESS );

    sElementPrevEntry = (knlRingEntry*)((stlChar*)sElementPrev + aOffset);

    sLastElementEntry->mNext = aElement;
    sFirstElementEntry->mPrev = sElementEntry->mPrev;    
    sElementPrevEntry->mNext = aFirstElement;
    sElementEntry->mPrev = aLastElement;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;    
    
}

/**
 * @brief @a aElement 뒤에 @a aFirstElement .. @a aLastElement 를 연결한다.
 * @param[in] aElement 기준이 되는 aElement의 논리적 포인터
 * @param[in] aFirstElement 연결되는 맨 처음 객체의 논리적 포인터
 * @param[in] aLastElement 연결되는 맨 마지막 객체의 논리적 포인터
 * @param[in] aOffset 연결 객체에서 STL_RING_ENTRY의 offset
 * @param[in,out] aEnv 커널 Environment
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_INITIALIZED_YET]
 *     knlInitialize() 함수가 호출되지 않았음.
 * [KNL_ERRCODE_INVALID_ARGUMENT]
 *     입력받은 논리적 포인터가 올바르지 않음.
 * @endcode
 * @remark ..aElement.. 가 ..aElement..aFirstElement..aLastElement.. 으로 연결된다.
 * @internal
 */
stlStatus knlSpliceRingAfterInternal( knlLogicalAddr  aElement,
                                      knlLogicalAddr  aFirstElement,
                                      knlLogicalAddr  aLastElement,
                                      stlSize         aOffset,
                                      knlEnv        * aEnv )
{
    void         * sElement;
    void         * sFirstElement;
    void         * sLastElement;
    void         * sElementNext;

    knlRingEntry * sElementEntry;
    knlRingEntry * sFirstElementEntry;
    knlRingEntry * sLastElementEntry;
    knlRingEntry * sElementNextEntry;
    
    STL_TRY( knlGetPhysicalAddr( aElement,
                                 &sElement,
                                 aEnv ) == STL_SUCCESS );

    sElementEntry = (knlRingEntry*)((stlChar*)sElement + aOffset);

    STL_TRY( knlGetPhysicalAddr( aFirstElement,
                                 &sFirstElement,
                                 aEnv ) == STL_SUCCESS );

    sFirstElementEntry = (knlRingEntry*)((stlChar*)sFirstElement + aOffset);

    if(aFirstElement == aLastElement)
    {
        sLastElementEntry = sFirstElementEntry;
    }
    else
    {
        STL_TRY( knlGetPhysicalAddr( aLastElement,
                                     &sLastElement,
                                     aEnv ) == STL_SUCCESS );

        sLastElementEntry = (knlRingEntry*)((stlChar*)sLastElement + aOffset);
    }

    STL_TRY( knlGetPhysicalAddr( sElementEntry->mNext,
                                 &sElementNext,
                                 aEnv ) == STL_SUCCESS );

    sElementNextEntry = (knlRingEntry*)((stlChar*)sElementNext + aOffset);

    sFirstElementEntry->mPrev = aElement;
    sLastElementEntry->mNext = sElementEntry->mNext;
    sElementNextEntry->mPrev = aLastElement;
    sElementEntry->mNext = aFirstElement;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;    
    
}

/**
 * @brief Ring @a aHead2 를 Ring @a aHead1 앞에 붙이고, @a aHead2 는 empty가 된다.
 * @param[in] aHead1 붙일 Ring Head의 논리적 포인터
 * @param[in] aHead2 붙여질 Ring Head의 논리적 포인터
 * @param[in] aOffset 연결 객체에서 STL_RING_ENTRY의 offset
 * @param[in,out] aEnv 커널 Environment
 * @return 성공시 STL_SUCCESS, 에러 발생시 STL_FAILURE
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_INITIALIZED_YET]
 *     knlInitialize() 함수가 호출되지 않았음.
 * [KNL_ERRCODE_INVALID_ARGUMENT]
 *     입력받은 논리적 포인터가 올바르지 않음.
 * @endcode
 * @internal
 */
stlStatus knlConcatRingInternal( knlLogicalAddr  aHead1,
                                 knlLogicalAddr  aHead2,
                                 stlSize         aOffset,
                                 knlEnv        * aEnv )
{
    stlBool       sIsEmpty;
    knlRingHead * sHead2;

    STL_TRY(knlIsEmptyRingInternal( aHead2,
                                    &sIsEmpty,
                                    aOffset,
                                    aEnv )
            == STL_SUCCESS );

    if( sIsEmpty == STL_TRUE )
    {
        STL_TRY( knlGetPhysicalAddr( aHead2,
                                     (void**)&sHead2,
                                     aEnv ) == STL_SUCCESS );

        STL_TRY( knlSpliceRingBeforeInternal( aHead1 - aOffset,
                                              sHead2->mNext,
                                              sHead2->mPrev,
                                              aOffset,
                                              aEnv )
                 == STL_SUCCESS );

        sHead2->mNext = aHead2 - aOffset;
        sHead2->mPrev = aHead2 - aOffset;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Ring @a aHead2 를 Ring @a aHead1 뒤에 붙이고, @a aHead2 는 empty가 된다.
 * @param[in] aHead1 붙일 Ring Head의 논리적 포인터
 * @param[in] aHead2 붙여질 Ring Head의 논리적 포인터
 * @param[in] aOffset 연결 객체에서 STL_RING_ENTRY의 offset
 * @param[in,out] aEnv 커널 Environment
 * @return 성공시 STL_SUCCESS, 에러 발생시 STL_FAILURE
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_INITIALIZED_YET]
 *     knlInitialize() 함수가 호출되지 않았음.
 * [KNL_ERRCODE_INVALID_ARGUMENT]
 *     입력받은 논리적 포인터가 올바르지 않음.
 * @endcode
 * @internal
 */
stlStatus knlPrependRingInternal( knlLogicalAddr  aHead1,
                                  knlLogicalAddr  aHead2,
                                  stlSize         aOffset,
                                  knlEnv        * aEnv )
{
    stlBool       sIsEmpty;
    knlRingHead * sHead2;

    STL_TRY(knlIsEmptyRingInternal( aHead2,
                                    &sIsEmpty,
                                    aOffset,
                                    aEnv )
            == STL_SUCCESS );

    if( sIsEmpty == STL_TRUE )
    {
        STL_TRY( knlGetPhysicalAddr( aHead2,
                                     (void**)&sHead2,
                                     aEnv ) == STL_SUCCESS );

        STL_TRY( knlSpliceRingAfterInternal( aHead1 - aOffset,
                                             sHead2->mNext,
                                             sHead2->mPrev,
                                             aOffset,
                                             aEnv )
                 == STL_SUCCESS );

        sHead2->mNext = aHead2 - aOffset;
        sHead2->mPrev = aHead2 - aOffset;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Ring에서 @a aFirstElenet 부터 @a aLastElement 까지 연결을 제거한다.
 * @param[in] aFirstElement 연결이 제거되는 맨 처음 객체의 논리적 포인터
 * @param[in] aLastElement 연결이 제거되는 맨 마지막 객체의 논리적 포인터
 * @param[in] aOffset 연결 객체에서 STL_RING_ENTRY의 offset
 * @param[in,out] aEnv 커널 Environment
 * @return 성공시 STL_SUCCESS, 에러 발생시 STL_FAILURE
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_INITIALIZED_YET]
 *     knlInitialize() 함수가 호출되지 않았음.
 * [KNL_ERRCODE_INVALID_ARGUMENT]
 *     입력받은 논리적 포인터가 올바르지 않음.
 * @endcode
 * @internal
 */
stlStatus knlUnspliceRingInternal( knlLogicalAddr  aFirstElement,
                                   knlLogicalAddr  aLastElement,
                                   stlSize         aOffset,
                                   knlEnv        * aEnv )
{
    void * sFirstElement;
    void * sLastElement;
    void * sFirstElementPrev;
    void * sLastElementNext;

    knlRingEntry * sFirstElementEntry;
    knlRingEntry * sLastElementEntry;
    knlRingEntry * sFirstElementPrevEntry;
    knlRingEntry * sLastElementNextEntry;
    
    STL_TRY( knlGetPhysicalAddr( aFirstElement,
                                 &sFirstElement,
                                 aEnv ) == STL_SUCCESS );

    sFirstElementEntry = (knlRingEntry*)((stlChar*)sFirstElement + aOffset);

    if( aFirstElement == aLastElement )
    {
        sLastElementEntry = sFirstElementEntry;
    }
    else
    {
        STL_TRY( knlGetPhysicalAddr( aLastElement,
                                     &sLastElement,
                                     aEnv ) == STL_SUCCESS );

        sLastElementEntry = (knlRingEntry*)((stlChar*)sLastElement + aOffset);
    }

    STL_TRY( knlGetPhysicalAddr( sFirstElementEntry->mPrev,
                                 &sFirstElementPrev,
                                 aEnv ) == STL_SUCCESS );

    sFirstElementPrevEntry = (knlRingEntry*)((stlChar*)sFirstElementPrev + aOffset);

    STL_TRY( knlGetPhysicalAddr( sLastElementEntry->mNext,
                                 &sLastElementNext,
                                 aEnv ) == STL_SUCCESS );

    sLastElementNextEntry = (knlRingEntry*)((stlChar*)sLastElementNext + aOffset);

    sFirstElementPrevEntry->mNext = sLastElementEntry->mNext;
    sLastElementNextEntry->mPrev = sFirstElementEntry->mPrev;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
                                   
/** @} */
