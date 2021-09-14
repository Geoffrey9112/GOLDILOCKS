/*******************************************************************************
 * knlRing.h
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


#ifndef _KNLRING_H_
#define _KNLRING_H_ 1

/**
 * @file knlRing.h
 * @brief Kernel Layer Ring Routines
 */

/**
 * @defgroup knlRing Ring
 * @ingroup knExternal
 * @{
 */

/**
 * @defgroup knlLogicalRing Logical Address Ring
 * @{
 */

/**
 * @brief Ring의 처음 객체의 논리적 포인터를 반환한다.
 * @param[in] aHead Ring Head의 논리적 포인터
 * @param[out] aPointer 처음 객체의 논리적 포인터
 * @param[in,out] aEnv 커널 Environment
 * @return 성공시 STL_SUCCESS, 에러 발생시 STL_FAILURE
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_INITIALIZED_YET]
 *     knlInitialize() 함수가 호출되지 않았음.
 * [KNL_ERRCODE_INVALID_ARGUMENT]
 *     입력받은 논리적 포인터가 올바르지 않음.
 * @endcode
 */
#define knlGetFirstRing( aHead, aPointer, aEnv ) \
    knlGetFirstRingInternal( aHead,                     \
                             aPointer,                  \
                             aEnv )

stlStatus knlGetFirstRingInternal( knlLogicalAddr   aHead,
                                   knlLogicalAddr * aPointer,
                                   knlEnv         * aEnv );

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
 */
#define knlGetLastRing( aHead, aPointer, aEnv )  \
    knlGetLastRingInternal( aHead,                      \
                            aPointer,                   \
                            aEnv )

stlStatus knlGetLastRingInternal( knlLogicalAddr   aHead,
                                  knlLogicalAddr * aPointer,
                                  knlEnv         * aEnv );

/**
 * @brief 입력 받은 객체를 통해 다음 객체의 논리적 포인터를 반환한다.
 * @param[in] aElementPointer 현재 객체의 논리적 포인터
 * @param[out] aPointer 다음 객체의 논리적 포인터
 * @param[in] aTypeName 연결 객체의 타입 이름
 * @param[in] aEntryName 연결 객체에서 STL_RING_ENTRY의 이름
 * @param[in,out] aEnv 커널 Environment
 * @return 성공시 STL_SUCCESS, 에러 발생시 STL_FAILURE
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_INITIALIZED_YET]
 *     knlInitialize() 함수가 호출되지 않았음.
 * [KNL_ERRCODE_INVALID_ARGUMENT]
 *     입력받은 논리적 포인터가 올바르지 않음.
 * @endcode
 */
#define knlGetNextRing( aElementPointer, aPointer, aTypeName, aEntryName, aEnv ) \
    knlGetNextRingInternal( aElementPointer,                                            \
                            aPointer,                                                   \
                            STL_OFFSETOF(aTypeName, aEntryName),                        \
                            aEnv)

stlStatus knlGetNextRingInternal( knlLogicalAddr   aElementPointer,
                                  knlLogicalAddr * aPointer,
                                  stlSize          aOffset,
                                  knlEnv         * aEnv );

/**
 * @brief 입력 받은 객체를 통해 이전 객체의 논리적 포인터를 반환한다.
 * @param[in] aElementPointer 현재 객체의 논리적 포인터
 * @param[out] aPointer 이전 객체의 논리적 포인터
 * @param[in] aTypeName 연결 객체의 타입 이름
 * @param[in] aEntryName 연결 객체에서 STL_RING_ENTRY의 이름
 * @param[in,out] aEnv 커널 Environment
 * @return 성공시 STL_SUCCESS, 에러 발생시 STL_FAILURE
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_INITIALIZED_YET]
 *     knlInitialize() 함수가 호출되지 않았음.
 * [KNL_ERRCODE_INVALID_ARGUMENT]
 *     입력받은 논리적 포인터가 올바르지 않음.
 * @endcode
 */
#define knlGetPrevRing( aElementPointer, aPointer, aTypeName, aEntryName, aEnv ) \
    knlGetPrevRingInternal( aElementPointer,                                            \
                            aPointer,                                                   \
                            STL_OFFSETOF(aTypeName, aEntryName),                        \
                            aEnv)

stlStatus knlGetPrevRingInternal( knlLogicalAddr   aElementPointer,
                                  knlLogicalAddr * aPointer,
                                  stlSize          aOffset,
                                  knlEnv         * aEnv );

/**
 * @brief Ring을 초기화 한다.
 * @param[in] aHead Ring Head의 논리적 포인터
 * @param[in] aTypeName 연결 객체의 타입 이름
 * @param[in] aEntryName 연결 객체에서 STL_RING_ENTRY의 이름
 * @param[in,out] aEnv 커널 Environment
 * @return 성공시 STL_SUCCESS, 에러 발생시 STL_FAILURE
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_INITIALIZED_YET]
 *     knlInitialize() 함수가 호출되지 않았음.
 * [KNL_ERRCODE_INVALID_ARGUMENT]
 *     입력받은 논리적 포인터가 올바르지 않음.
 * @endcode
 */
#define knlInitRing( aHead, aTypeName, aEntryName, aEnv )        \
    knlInitRingInternal( aHead,                                         \
                         STL_OFFSETOF(aTypeName, aEntryName),           \
                         aEnv )

stlStatus knlInitRingInternal( knlLogicalAddr  aHead,
                               stlSize         aOffset,
                               knlEnv        * aEnv );

/**
 * @brief Ring이 비었는지 확인한다.
 * @param[in] aHead Ring Head의 논리적 포인터
 * @param[out] aIsEmpty Ring이 비었는지 여부
 * @param[in] aTypeName 연결 객체의 타입 이름
 * @param[in] aEntryName 연결 객체에서 STL_RING_ENTRY의 이름
 * @param[in,out] aEnv 커널 Environment
 * @return 성공시 STL_SUCCESS, 에러 발생시 STL_FAILURE
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_INITIALIZED_YET]
 *     knlInitialize() 함수가 호출되지 않았음.
 * [KNL_ERRCODE_INVALID_ARGUMENT]
 *     입력받은 논리적 포인터가 올바르지 않음.
 * @endcode
 */
#define knlIsEmptyRing( aHead, aIsEmpty, aTypeName, aEntryName, aEnv )   \
    knlIsEmptyRingInternal( aHead,                                              \
                            aIsEmpty,                                           \
                            STL_OFFSETOF(aTypeName, aEntryName),                \
                            aEnv )

stlStatus knlIsEmptyRingInternal( knlLogicalAddr   aHead,
                                  stlBool        * aIsEmpty,
                                  stlSize          aOffset,
                                  knlEnv         * aEnv );

/**
 * @brief @a aElement 앞에 @a aFirstElement .. @a aLastElement 를 연결한다.
 * @param[in] aElement 기준이 되는 aElement의 논리적 포인터
 * @param[in] aFirstElement 연결되는 맨 처음 객체의 논리적 포인터
 * @param[in] aLastElement 연결되는 맨 마지막 객체의 논리적 포인터
 * @param[in] aTypeName 연결 객체의 타입 이름
 * @param[in] aEntryName 연결 객체에서 STL_RING_ENTRY의 이름
 * @param[in,out] aEnv 커널 Environment
 * @return 성공시 STL_SUCCESS, 에러 발생시 STL_FAILURE
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_INITIALIZED_YET]
 *     knlInitialize() 함수가 호출되지 않았음.
 * [KNL_ERRCODE_INVALID_ARGUMENT]
 *     입력받은 논리적 포인터가 올바르지 않음.
 * @endcode
 * @remark ..aElement.. 가 ..aFirstElement..aLastElement..aElement.. 으로 연결된다.
 */
#define knlSpliceRingBefore( aElement, aFirstElement, aLastElement, aTypeName, aEntryName, aEnv ) \
    knlSpliceRingBeforeInternal( aElement,                                                          \
                                 aFirstElement,                                                     \
                                 aLastElement,                                                      \
                                 STL_OFFSETOF(aTypeName, aEntryName),                               \
                                 aEnv)

stlStatus knlSpliceRingBeforeInternal( knlLogicalAddr  aElement,
                                       knlLogicalAddr  aFirstElement,
                                       knlLogicalAddr  aLastElement,
                                       stlSize         aOffset,
                                       knlEnv        * aEnv );
                                       
/**
 * @brief @a aElement 뒤에 @a aFirstElement .. @a aLastElement 를 연결한다.
 * @param[in] aElement 기준이 되는 aElement의 논리적 포인터
 * @param[in] aFirstElement 연결되는 맨 처음 객체의 논리적 포인터
 * @param[in] aLastElement 연결되는 맨 마지막 객체의 논리적 포인터
 * @param[in] aTypeName 연결 객체의 타입 이름
 * @param[in] aEntryName 연결 객체에서 STL_RING_ENTRY의 이름
 * @param[in,out] aEnv 커널 Environment
 * @return 성공시 STL_SUCCESS, 에러 발생시 STL_FAILURE
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_INITIALIZED_YET]
 *     knlInitialize() 함수가 호출되지 않았음.
 * [KNL_ERRCODE_INVALID_ARGUMENT]
 *     입력받은 논리적 포인터가 올바르지 않음.
 * @endcode
 * @remark ..aElement.. 가 ..aElement..aFirstElement..aLastElement.. 으로 연결된다.
 */
#define knlSpliceRingAfter( aElement, aFirstElement, aLastElement, aTypeName, aEntryName, aEnv ) \
    knlSpliceRingAfterInternal( aElement,                                                           \
                                aFirstElement,                                                      \
                                aLastElement,                                                       \
                                STL_OFFSETOF(aTypeName, aEntryName),                                \
                                aEnv)

stlStatus knlSpliceRingAfterInternal( knlLogicalAddr  aElement,
                                      knlLogicalAddr  aFirstElement,
                                      knlLogicalAddr  aLastElement,
                                      stlSize         aOffset,
                                      knlEnv        * aEnv );

/**
 * @brief @a aElement 앞에 @a aInsertElement 를 삽입한다.
 * @param[in] aElement 기준이 되는 aElement의 논리적 포인터
 * @param[in] aInsertElement 삽입되는 객체의 논리적 포인터
 * @param[in] aTypeName 연결 객체의 타입 이름
 * @param[in] aEntryName 연결 객체에서 STL_RING_ENTRY의 이름
 * @param[in,out] aEnv 커널 Environment
 * @return 성공시 STL_SUCCESS, 에러 발생시 STL_FAILURE
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_INITIALIZED_YET]
 *     knlInitialize() 함수가 호출되지 않았음.
 * [KNL_ERRCODE_INVALID_ARGUMENT]
 *     입력받은 논리적 포인터가 올바르지 않음.
 * @endcode
 * @remark ..aElement.. 가 ..aInsertElement..aElement.. 으로 연결된다.
 */
#define knlInsertRingBefore( aElement, aInsertElement, aTypeName, aEntryName, aEnv )     \
    knlSpliceRingBeforeInternal( aElement,                                                      \
                                 aInsertElement,                                                \
                                 aInsertElement,                                                \
                                 STL_OFFSETOF(aTypeName, aEntryName),                           \
                                 aEnv)
                                       
/**
 * @brief @a aElement 뒤에 @a aInsertElement 를 삽입한다.
 * @param[in] aElement 기준이 되는 aElement의 논리적 포인터
 * @param[in] aInsertElement 삽입되는 객체의 논리적 포인터
 * @param[in] aTypeName 연결 객체의 타입 이름
 * @param[in] aEntryName 연결 객체에서 STL_RING_ENTRY의 이름
 * @param[in,out] aEnv 커널 Environment
 * @return 성공시 STL_SUCCESS, 에러 발생시 STL_FAILURE
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_INITIALIZED_YET]
 *     knlInitialize() 함수가 호출되지 않았음.
 * [KNL_ERRCODE_INVALID_ARGUMENT]
 *     입력받은 논리적 포인터가 올바르지 않음.
 * @endcode
 * @remark ..aElement.. 가 ..aElement..aInsertElement.. 으로 연결된다.
 */
#define knlInsertRingAfter( aElement, aInsertElement, aTypeName, aEntryName, aEnv )      \
    knlSpliceRingAfterInternal( aElement,                                                       \
                                aInsertElement,                                                 \
                                aInsertElement,                                                 \
                                STL_OFFSETOF(aTypeName, aEntryName),                            \
                                aEnv)

/**
 * @brief @a aHead 의 처음 Element 앞에 @a aFirstElement .. @a aLastElement 를 연결한다.
 * @param[in] aHead Ring Head의 논리적 포인터
 * @param[in] aFirstElement 연결되는 맨 처음 객체의 논리적 포인터
 * @param[in] aLastElement 연결되는 맨 마지막 객체의 논리적 포인터
 * @param[in] aTypeName 연결 객체의 타입 이름
 * @param[in] aEntryName 연결 객체에서 STL_RING_ENTRY의 이름
 * @param[in,out] aEnv 커널 Environment
 * @return 성공시 STL_SUCCESS, 에러 발생시 STL_FAILURE
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_INITIALIZED_YET]
 *     knlInitialize() 함수가 호출되지 않았음.
 * [KNL_ERRCODE_INVALID_ARGUMENT]
 *     입력받은 논리적 포인터가 올바르지 않음.
 * @endcode
 * @remark ..aHead.. 가 ..aHead..aFirstElement..aLastElement.. 으로 연결된다.
 */
#define knlSpliceRingHead( aHead, aFirstElement, aLastElement, aTypeName, aEntryName, aEnv ) \
    knlSpliceRingAfterInternal( aHead - STL_OFFSETOF(aTypeName, aEntryName),                        \
                                aFirstElement,                                                      \
                                aLastElement,                                                       \
                                STL_OFFSETOF(aTypeName, aEntryName),                                \
                                aEnv)
                                       
/**
 * @brief @a aHead 의 마지막 Element 뒤에 @a aFirstElement .. @a aLastElement 를 연결한다.
 * @param[in] aHead Ring Head의 논리적 포인터
 * @param[in] aFirstElement 연결되는 맨 처음 객체의 논리적 포인터
 * @param[in] aLastElement 연결되는 맨 마지막 객체의 논리적 포인터
 * @param[in] aTypeName 연결 객체의 타입 이름
 * @param[in] aEntryName 연결 객체에서 STL_RING_ENTRY의 이름
 * @param[in,out] aEnv 커널 Environment
 * @return 성공시 STL_SUCCESS, 에러 발생시 STL_FAILURE
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_INITIALIZED_YET]
 *     knlInitialize() 함수가 호출되지 않았음.
 * [KNL_ERRCODE_INVALID_ARGUMENT]
 *     입력받은 논리적 포인터가 올바르지 않음.
 * @endcode
 * @remark ..aHead.. 가 ..aFirstElement..aLastElement..aHead.. 으로 연결된다.
 */
#define knlSpliceRingTail( aHead, aFirstElement, aLastElement, aTypeName, aEntryName, aEnv ) \
    knlSpliceRingBeforeInternal( aHead - STL_OFFSETOF(aTypeName, aEntryName),                       \
                                 aFirstElement,                                                     \
                                 aLastElement,                                                      \
                                 STL_OFFSETOF(aTypeName, aEntryName),                               \
                                 aEnv)

/**
 * @brief @a aHead 의 처음 Element 앞에 @a aInsertElement 삽입한다.
 * @param[in] aHead Ring Head의 논리적 포인터
 * @param[in] aInsertElement 삽입되는 객체의 논리적 포인터
 * @param[in] aTypeName 연결 객체의 타입 이름
 * @param[in] aEntryName 연결 객체에서 STL_RING_ENTRY의 이름
 * @param[in,out] aEnv 커널 Environment
 * @return 성공시 STL_SUCCESS, 에러 발생시 STL_FAILURE
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_INITIALIZED_YET]
 *     knlInitialize() 함수가 호출되지 않았음.
 * [KNL_ERRCODE_INVALID_ARGUMENT]
 *     입력받은 논리적 포인터가 올바르지 않음.
 * @endcode
 * @remark ..aHead.. 가 ..aHead..aInsertElement.. 으로 연결된다.
 */
#define knlInsertRingHead( aHead, aInsertElement, aTypeName, aEntryName, aEnv )  \
    knlSpliceRingHead( aHead,                                                           \
                       aInsertElement,                                                  \
                       aInsertElement,                                                  \
                       aTypeName,                                                       \
                       aEntryName,                                                      \
                       aEnv )
                                       
/**
 * @brief @a aHead 의 마지막 Element 뒤에 @a aInsertElement 삽입한다.
 * @param[in] aHead Ring Head의 논리적 포인터
 * @param[in] aInsertElement 삽입되는 객체의 논리적 포인터
 * @param[in] aTypeName 연결 객체의 타입 이름
 * @param[in] aEntryName 연결 객체에서 STL_RING_ENTRY의 이름
 * @param[in,out] aEnv 커널 Environment
 * @return 성공시 STL_SUCCESS, 에러 발생시 STL_FAILURE
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_INITIALIZED_YET]
 *     knlInitialize() 함수가 호출되지 않았음.
 * [KNL_ERRCODE_INVALID_ARGUMENT]
 *     입력받은 논리적 포인터가 올바르지 않음.
 * @endcode
 * @remark ..aHead.. 가 ..aInsertElement..aHead.. 으로 연결된다.
 */
#define knlInsertRingTail( aHead, aInsertElement, aTypeName, aEntryName, aEnv )  \
    knlSpliceRingTail( aHead,                                                           \
                       aInsertElement,                                                  \
                       aInsertElement,                                                  \
                       aTypeName,                                                       \
                       aEntryName,                                                      \
                       aEnv )
                                       
/**
 * @brief Ring @a aHead2 를 Ring @a aHead1 앞에 붙이고, @a aHead2 는 empty가 된다.
 * @param[in] aHead1 붙일 Ring Head의 논리적 포인터
 * @param[in] aHead2 붙여질 Ring Head의 논리적 포인터
 * @param[in] aTypeName 연결 객체의 타입 이름
 * @param[in] aEntryName 연결 객체에서 STL_RING_ENTRY의 이름
 * @param[in,out] aEnv 커널 Environment
 * @return 성공시 STL_SUCCESS, 에러 발생시 STL_FAILURE
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_INITIALIZED_YET]
 *     knlInitialize() 함수가 호출되지 않았음.
 * [KNL_ERRCODE_INVALID_ARGUMENT]
 *     입력받은 논리적 포인터가 올바르지 않음.
 * @endcode
 */
#define knlConcatRing( aHead1, aHead2, aTypeName, aEntryName, aEnv )  \
    knlConcatRingInternal( aHead1,                                            \
                           aHead2,                                            \
                           STL_OFFSETOF( aTypeName, aEntryName ),             \
                           aEnv )

stlStatus knlConcatRingInternal( knlLogicalAddr  aHead1,
                                 knlLogicalAddr  aHead2,
                                 stlSize         aOffset,
                                 knlEnv        * aEnv );

/**
 * @brief Ring @a aHead2 를 Ring @a aHead1 뒤에 붙이고, @a aHead2 는 empty가 된다.
 * @param[in] aHead1 붙일 Ring Head의 논리적 포인터
 * @param[in] aHead2 붙여질 Ring Head의 논리적 포인터
 * @param[in] aTypeName 연결 객체의 타입 이름
 * @param[in] aEntryName 연결 객체에서 STL_RING_ENTRY의 이름
 * @param[in,out] aEnv 커널 Environment
 * @return 성공시 STL_SUCCESS, 에러 발생시 STL_FAILURE
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_INITIALIZED_YET]
 *     knlInitialize() 함수가 호출되지 않았음.
 * [KNL_ERRCODE_INVALID_ARGUMENT]
 *     입력받은 논리적 포인터가 올바르지 않음.
 * @endcode
 */
#define knlPrependRing( aHead1, aHead2, aTypeName, aEntryName, aEnv )    \
    knlPrependRingInternal( aHead1,                                             \
                            aHead2,                                             \
                            STL_OFFSETOF( aTypeName, aEntryName ),              \
                            aEnv )

stlStatus knlPrependRingInternal( knlLogicalAddr  aHead1,
                                  knlLogicalAddr  aHead2,
                                  stlSize         aOffset,
                                  knlEnv        * aEnv );

/**
 * @brief Ring에서 @a aFirstElenet 부터 @a aLastElement 까지 연결을 제거한다.
 * @param[in] aFirstElement 연결이 제거되는 맨 처음 객체의 논리적 포인터
 * @param[in] aLastElement 연결이 제거되는 맨 마지막 객체의 논리적 포인터
 * @param[in] aTypeName 연결 객체의 타입 이름
 * @param[in] aEntryName 연결 객체에서 STL_RING_ENTRY의 이름
 * @param[in,out] aEnv 커널 Environment
 * @return 성공시 STL_SUCCESS, 에러 발생시 STL_FAILURE
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_INITIALIZED_YET]
 *     knlInitialize() 함수가 호출되지 않았음.
 * [KNL_ERRCODE_INVALID_ARGUMENT]
 *     입력받은 논리적 포인터가 올바르지 않음.
 * @endcode
 */
#define knlUnspliceRing( aFirstElement, aLastElement, aTypeName, aEntryName, aEnv )      \
    knlUnspliceRingInternal( aFirstElement,                                                     \
                             aLastElement,                                                      \
                             STL_OFFSETOF( aTypeName, aEntryName ),                             \
                             aEnv )                                                      \

stlStatus knlUnspliceRingInternal( knlLogicalAddr  aFirstElement,
                                   knlLogicalAddr  aLastElement,
                                   stlSize         aOffset,
                                   knlEnv        * aEnv );
    
/**
 * @brief Ring에서 @a aRemoveElement 연결을 제거한다.
 * @param[in] aRemoveElement 연결이 제거되는 객체의 논리적 포인터
 * @param[in] aTypeName 연결 객체의 타입 이름
 * @param[in] aEntryName 연결 객체에서 STL_RING_ENTRY의 이름
 * @param[in,out] aEnv 커널 Environment
 * @return 성공시 STL_SUCCESS, 에러 발생시 STL_FAILURE
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_INITIALIZED_YET]
 *     knlInitialize() 함수가 호출되지 않았음.
 * [KNL_ERRCODE_INVALID_ARGUMENT]
 *     입력받은 논리적 포인터가 올바르지 않음.
 * @endcode
 */
#define knlRemoveRing( aRemoveElement, aTypeName, aEntryName, aEnv )     \
    knlUnspliceRing( aRemoveElement,                                            \
                     aRemoveElement,                                            \
                     aTypeName,                                                 \
                     aEntryName,                                                \
                     aEnv )                                              \

/**
 * @brief Ring의 각 Element를 순회한다.
 * @param[out] aElement 현재 객체의 논리적 포인터
 * @param[in] aHead Ring Head의 논리적 포인터
 * @param[in] aTypeName 연결 객체의 타입 이름
 * @param[in] aEntryName 연결 객체에서 STL_RING_ENTRY의 이름
 * @param[in,out] aEnv 커널 Environment
 * @param[in] aRampName 에러 발생시 수행될 STL_CATCH()의 이름
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_INITIALIZED_YET]
 *     knlInitialize() 함수가 호출되지 않았음.
 * [KNL_ERRCODE_INVALID_ARGUMENT]
 *     입력받은 논리적 포인터가 올바르지 않음.
 * @endcode
 * @remark 반드시 코드에 STL_CATCH( @a aRampName ) 가 존재해야 한다.
 */
#define KNL_FOREACH_RING( aElement, aHead, aTypeName, aEntryName, aEnv, aRampName )      \
    for( STL_TRY_THROW( knlGetFirstRing( aHead,                                                 \
                                         aElement,                                              \
                                         aEnv ) == STL_SUCCESS,  aRampName );            \
         *aElement != aHead - STL_OFFSETOF( aTypeName, aEntryName );                            \
         STL_TRY_THROW( knlGetNextRing( *aElement,                                              \
                                        aElement,                                               \
                                        aTypeName,                                              \
                                        aEntryName,                                             \
                                        aEnv ) == STL_SUCCESS, aRampName ) )

/**
 * @brief 현재 객체의 연결을 삭제가 가능하도록 Ring의 각 Element를 순회한다.
 * @param[out] aElement 현재 객체의 논리적 포인터
 * @param[out] aNextElement 현재 객체 다음에 있는 객체의 논리적 포인터
 * @param[in] aHead Ring Head의 논리적 포인터
 * @param[in] aTypeName 연결 객체의 타입 이름
 * @param[in] aEntryName 연결 객체에서 STL_RING_ENTRY의 이름
 * @param[in,out] aEnv 커널 Environment
 * @param[in] aRampName 에러 발생시 수행될 STL_CATCH()의 이름
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_INITIALIZED_YET]
 *     knlInitialize() 함수가 호출되지 않았음.
 * [KNL_ERRCODE_INVALID_ARGUMENT]
 *     입력받은 논리적 포인터가 올바르지 않음.
 * @endcode
 * @remark 반드시 코드에 STL_CATCH( @a aRampName ) 가 존재해야 한다.
 */
#define KNL_FOREACH_SAFE_RING( aElement, aNextElement, aHead, aTypeName, aEntryName, aEnv, aRampName ) \
    for( STL_TRY_THROW( knlGetFirstRing( aHead,                                                               \
                                         aElement,                                                            \
                                         aEnv ) == STL_SUCCESS, aRampName ),                           \
         STL_TRY_THROW( knlGetNextRing( *aElement,                                                            \
                                        aNextElement,                                                         \
                                        aTypeName,                                                            \
                                        aEntryName,                                                           \
                                        aEnv ) == STL_SUCCESS, aRampName );                            \
         *aElement != aHead - STL_OFFSETOF( aTypeName, aEntryName );                                          \
         *aElement = *aNextElement,                                                                           \
         STL_TRY_THROW( knlGetNextRing( *aElement,                                                            \
                                        aNextElement,                                                         \
                                        aTypeName,                                                            \
                                        aEntryName,                                                           \
                                        aEnv ) == STL_SUCCESS, aRampName ) 

/** @} */

/** @} */

#endif /* _KNLRING_H_ */
