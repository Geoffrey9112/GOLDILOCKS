/*******************************************************************************
 * stlRing.h
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


#ifndef _STLRING_H_
#define _STLRING_H_ 1

/**
 * @file stlRing.h
 * @brief Standard Layer Ring Routines
 */

/**
 * @defgroup stlPointerRing Pointer Ring
 * @ingroup STL 
 * @{
 */

/* ptrring의 header가 non-static shared memory 영역에 있을 경우(ex: pch의 latch),
 * data node가 가리키는 header의 포인터 정보다 process마다 다를 수 있어서,
 * header의 초기 next,prev를 모두 NULL로 세팅하고, 이후 첫 data node의 prev와
 * 마지막 데이터 노드의 next도 NULL로 세팅한다
 */

/**
 * @brief 주어진 pointer ring pointer로부터 이를 포함한 구조체의 시작주소를 구한다
 * @param[in] linkPtr stlRingEntry 타입의 포인터
 * @param[in] linkOffset Ring에 달릴 데이터 구조체에서 stlRingEntry 멤버의 Offset
 * @return linkPtr을 포함한 데이터 구조체의 시작 pointer(void*)
 */
#define STL_RING_GET_DATA(linkPtr,linkOffset)   \
    ((void*)((stlChar*)(linkPtr) - linkOffset))

/**
 * @brief 주어진 data pointer로부터 linkOffset 만큼 떨어진 곳에 있는 stlRingEntry 멤버의 시작주소를 구한다
 * @param[in] dataPtr stlRingEntry 타입의 포인터
 * @param[in] linkOffset Ring에 달릴 데이터 구조체에서 stlRingEntry 멤버의 Offset
 * @return stlRing 멤버의 시작 pointer(stlRingEntry*)
 */
#define STL_RING_GET_LINK(dataPtr,linkOffset)           \
    ((stlRingEntry*)((stlChar*)(dataPtr) + linkOffset))

/**
 * @brief 주어진 pointer ring head의 멤버변수들을 초기화 한다
 * @param[in,out] headPtr 초기화 할 stlRingHead 타입의 포인터
 * @param[in] linkOffset Ring에 달릴 데이터 구조체에서 stlRingEntry 멤버의 Offset
 */
#define STL_RING_INIT_HEADLINK(headPtr,linkOffset)      \
    {                                                   \
        (headPtr)->mLink.mNext = NULL;                  \
        (headPtr)->mLink.mPrev = NULL;                  \
        (headPtr)->mLinkOffset = linkOffset;            \
    }

/**
 * @brief 주어진 pointer ring data의 멤버변수들을 초기화 한다
 * @param[in,out] dataPtr 초기화 할 stlRing을 가진 구조체의 포인터
 * @param[in] linkOffset Ring에 달릴 데이터 구조체에서 stlRingEntry 멤버의 Offset
 */
#define STL_RING_INIT_DATALINK(dataPtr,linkOffset)                      \
    {                                                                   \
        (STL_RING_GET_LINK(dataPtr,linkOffset))->mNext = dataPtr;       \
        (STL_RING_GET_LINK(dataPtr,linkOffset))->mPrev = dataPtr;       \
    }

/**
 * @brief 주어진 head에 달린 Ring 중 첫 entry를 반환한다
 * @param[out] headPtr Ring이 시작되는  stlRingHead 타입의 포인터
 * @return 첫 데이터 구조체의 시작 pointer(void*)
 */
#define STL_RING_GET_FIRST_DATA(headPtr)                        \
    (((headPtr)->mLink.mNext) == NULL ?                         \
     (STL_RING_GET_DATA((headPtr), (headPtr)->mLinkOffset)) :   \
     ((headPtr)->mLink.mNext))


/**
 * @brief 주어진 head에 달린 Ring 중 마지막 entry를 반환한다
 * @param[out] headPtr Ring이 시작되는  stlRingHead 타입의 포인터
 * @return 마지막 entry의 시작 pointer(void*)
 */
#define STL_RING_GET_LAST_DATA(headPtr)                         \
    (((headPtr)->mLink.mPrev) == NULL ?                         \
     (STL_RING_GET_DATA((headPtr), (headPtr)->mLinkOffset)) :   \
     ((headPtr)->mLink.mPrev))

/**
 * @brief 주어진 data entry 다음의 entry를 반환한다
 * @param[out] headPtr Ring이 시작되는  stlRingHead 타입의 포인터
 * @param[out] dataPtr Ring에 달린 entry들 중 한개의 포인터
 * @return 다음 entry의 시작 pointer(void*)
 */
#define STL_RING_GET_NEXT_DATA(headPtr,dataPtr)                                 \
    ((STL_RING_GET_LINK(dataPtr,(headPtr)->mLinkOffset))->mNext == NULL ?       \
     (STL_RING_GET_DATA((headPtr), (headPtr)->mLinkOffset)) :                   \
     (STL_RING_GET_LINK(dataPtr,(headPtr)->mLinkOffset))->mNext)

/**
 * @brief 주어진 data entry 이전의 entry를 반환한다
 * @param[out] headPtr Ring이 시작되는  stlRingHead 타입의 포인터
 * @param[out] dataPtr Ring에 달린 entry들 중 한개의 포인터
 * @return 이전 entry의 시작 pointer(void*)
 */
#define STL_RING_GET_PREV_DATA(headPtr,dataPtr)                                 \
    ((STL_RING_GET_LINK(dataPtr,(headPtr)->mLinkOffset))->mPrev == NULL ?       \
     (STL_RING_GET_DATA((headPtr), (headPtr)->mLinkOffset)) :                   \
     (STL_RING_GET_LINK(dataPtr,(headPtr)->mLinkOffset))->mPrev)

/**
 * @brief Ring이 비었는지 확인한다.
 * @param[in] headPtr Ring이 시작되는  stlRingHead 타입의 포인터
 * @return Emtpty 여부(stlBool)
 */
#define STL_RING_IS_EMPTY(headPtr)                                      \
    ( (STL_RING_GET_FIRST_DATA(headPtr) ==                              \
       (STL_RING_GET_DATA((headPtr), (headPtr)->mLinkOffset))) ?        \
      STL_TRUE : STL_FALSE )

/**
 * @brief 주어진 headPtr에 달린 모든 entry들을 순회하며 dataPtr로 반환한다
 * @param[in] headPtr Ring이 시작되는  stlRingHead 타입의 포인터
 * @param[out] dataPtr Ring에 달린 entry들 중 한개의 포인터
 */
#define STL_RING_FOREACH_ENTRY(headPtr,dataPtr)                                 \
    for( dataPtr = STL_RING_GET_FIRST_DATA(headPtr);                            \
         dataPtr != (STL_RING_GET_DATA((headPtr), (headPtr)->mLinkOffset));     \
         dataPtr = STL_RING_GET_NEXT_DATA(headPtr,dataPtr) )

/**
 * @brief 주어진 startDataPtr 이후 부터 headPtr에 달린 모든 entry들을 순회하며 dataPtr로 반환한다
 * @param[in] headPtr Ring이 시작되는  stlRingHead 타입의 포인터
 * @param[in] startDataPtr 스캔의 시작 위치
 * @param[out] dataPtr Ring에 달린 entry들 중 한개의 포인터
 */
#define STL_RING_AT_FOREACH_ENTRY(headPtr,startDataPtr,dataPtr)                 \
    for( dataPtr = startDataPtr;                                                \
         dataPtr != (STL_RING_GET_DATA((headPtr), (headPtr)->mLinkOffset));     \
         dataPtr = STL_RING_GET_NEXT_DATA(headPtr,dataPtr) )

/**
 * @brief 주어진 headPtr에 달린 모든 entry들을 역순으로 순회하며 dataPtr로 반환한다
 * @param[in] headPtr Ring이 시작되는  stlRingHead 타입의 포인터
 * @param[out] dataPtr Ring에 달린 entry들 중 한개의 포인터
 */
#define STL_RING_FOREACH_ENTRY_REVERSE(headPtr,dataPtr)                         \
    for( dataPtr = STL_RING_GET_LAST_DATA(headPtr);                             \
         dataPtr != (STL_RING_GET_DATA((headPtr), (headPtr)->mLinkOffset));     \
         dataPtr = STL_RING_GET_PREV_DATA(headPtr,dataPtr) )

/**
 * @brief 주어진 headPtr에 달린 모든 entry들을 순회하며 dataPtr로 반환한다
 * @param[in] headPtr Ring이 시작되는  stlRingHead 타입의 포인터
 * @param[out] dataPtr Ring에 달린 entry들 중 한개의 포인터
 * @param[out] nextDataPtr 미리 저장해둔 dataPtr의 다음 entry
 */
#define STL_RING_FOREACH_ENTRY_SAFE(headPtr,dataPtr,nextDataPtr)                \
    for( dataPtr = STL_RING_GET_FIRST_DATA(headPtr),                            \
             nextDataPtr = STL_RING_GET_NEXT_DATA(headPtr,dataPtr);             \
         dataPtr != (STL_RING_GET_DATA((headPtr), (headPtr)->mLinkOffset));     \
         dataPtr = nextDataPtr,                                                 \
             nextDataPtr = STL_RING_GET_NEXT_DATA(headPtr,dataPtr) )      

/**
 * @brief 주어진 startDataPtr에 달린 모든 entry들을 순회하며 dataPtr로 반환한다
 * @param[in] headPtr Ring이 시작되는  stlRingHead 타입의 포인터
 * @param[in] startDataPtr 스캔의 시작 위치
 * @param[out] dataPtr Ring에 달린 entry들 중 한개의 포인터
 * @param[out] nextDataPtr 미리 저장해둔 dataPtr의 다음 entry
 */
#define STL_RING_AT_FOREACH_ENTRY_SAFE(headPtr,startDataPtr,dataPtr,nextDataPtr)        \
    for( dataPtr = startDataPtr,                                                        \
             nextDataPtr = STL_RING_GET_NEXT_DATA(headPtr,dataPtr);                     \
         dataPtr != (STL_RING_GET_DATA((headPtr), (headPtr)->mLinkOffset));             \
         dataPtr = nextDataPtr,                                                         \
             nextDataPtr = STL_RING_GET_NEXT_DATA(headPtr,dataPtr) )

/**
 * @brief 주어진 headPtr에 달린 모든 entry들을 역순으로 순회하며 dataPtr로 반환한다
 * @param[in] headPtr Ring이 시작되는  stlRingHead 타입의 포인터
 * @param[out] dataPtr Ring에 달린 entry들 중 한개의 포인터
 * @param[out] prevDataPtr 미리 저장해둔 dataPtr의 이전 entry
 */
#define STL_RING_FOREACH_ENTRY_REVERSE_SAFE(headPtr,dataPtr,prevDataPtr)        \
    for( dataPtr = STL_RING_GET_LAST_DATA(headPtr),                             \
             prevDataPtr = STL_RING_GET_PREV_DATA(headPtr,dataPtr);             \
         dataPtr != (STL_RING_GET_DATA((headPtr), (headPtr)->mLinkOffset));     \
         dataPtr = prevDataPtr,                                                 \
             prevDataPtr = STL_RING_GET_PREV_DATA(headPtr,dataPtr) )      

/**
 * @brief 주어진 startDataPtr에 달린 모든 entry들을 역순으로 순회하며 dataPtr로 반환한다
 * @param[in] headPtr Ring이 시작되는  stlRingHead 타입의 포인터
 * @param[in] startDataPtr 스캔의 시작 위치
 * @param[out] dataPtr Ring에 달린 entry들 중 한개의 포인터
 * @param[out] prevDataPtr 미리 저장해둔 dataPtr의 이전 entry
 */
#define STL_RING_AT_FOREACH_ENTRY_REVERSE_SAFE(headPtr,startDataPtr,dataPtr,prevDataPtr)        \
    for( dataPtr = startDataPtr,                                                                \
             prevDataPtr = STL_RING_GET_PREV_DATA(headPtr,dataPtr);                             \
         dataPtr != (STL_RING_GET_DATA((headPtr), (headPtr)->mLinkOffset));                     \
         dataPtr = prevDataPtr,                                                                 \
             prevDataPtr = STL_RING_GET_PREV_DATA(headPtr,dataPtr) )      

/**
 * @brief 주어진 headPtr에 dataPtr을 맨 앞으로 삽입한다
 * @param[out] headPtr Ring이 시작되는  stlRingHead 타입의 포인터
 * @param[in] dataPtr Ring에 삽입할 데이터의 포인터
 */
#define STL_RING_ADD_FIRST(headPtr,dataPtr)                                             \
    {                                                                                   \
        (STL_RING_GET_LINK(dataPtr,(headPtr)->mLinkOffset))->mPrev = NULL;              \
        (STL_RING_GET_LINK(dataPtr,(headPtr)->mLinkOffset))->mNext =                    \
            (headPtr)->mLink.mNext;                                                     \
        if( (headPtr)->mLink.mNext != NULL )                                            \
        {                                                                               \
            (STL_RING_GET_LINK((headPtr)->mLink.mNext,(headPtr)->mLinkOffset))->mPrev = \
                dataPtr;                                                                \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            (headPtr)->mLink.mPrev = dataPtr;                                           \
        }                                                                               \
        (headPtr)->mLink.mNext = dataPtr;                                               \
    }

/**
 * @brief 주어진 headPtr에 dataPtr을 맨 뒤로 삽입한다
 * @param[out] headPtr Ring이 시작되는  stlRingHead 타입의 포인터
 * @param[in] dataPtr Ring에 삽입할 데이터의 포인터
 */
#define STL_RING_ADD_LAST(headPtr,dataPtr)                                              \
    {                                                                                   \
        STL_DASSERT( (headPtr)->mLink.mPrev != dataPtr );                               \
        STL_DASSERT( (headPtr)->mLink.mNext != dataPtr );                               \
        (STL_RING_GET_LINK(dataPtr,(headPtr)->mLinkOffset))->mNext = NULL;              \
        (STL_RING_GET_LINK(dataPtr,(headPtr)->mLinkOffset))->mPrev =                    \
            (headPtr)->mLink.mPrev;                                                     \
        if( (headPtr)->mLink.mPrev != NULL )                                            \
        {                                                                               \
            (STL_RING_GET_LINK((headPtr)->mLink.mPrev,(headPtr)->mLinkOffset))->mNext = \
                dataPtr;                                                                \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            (headPtr)->mLink.mNext = dataPtr;                                           \
        }                                                                               \
        (headPtr)->mLink.mPrev = dataPtr;                                               \
    }

/**
 * @brief 주어진 headPtr의 맨 앞에 firstDataPtr부터 lastDataPtr까지 삽입한다.
 * @param[out] headPtr Ring이 시작되는  stlRingHead 타입의 포인터
 * @param[in] firstDataPtr Ring에 삽입할 데이터의 시작 포인터
 * @param[in] lastDataPtr Ring에 삽입할 데이터의 마지막 포인터
 */
#define STL_RING_SPLICE_FIRST(headPtr, firstDataPtr, lastDataPtr)                       \
    {                                                                                   \
        (STL_RING_GET_LINK(firstDataPtr,(headPtr)->mLinkOffset))->mPrev = NULL;         \
        (STL_RING_GET_LINK(lastDataPtr,(headPtr)->mLinkOffset))->mNext =                \
            (headPtr)->mLink.mNext;                                                     \
        if( (headPtr)->mLink.mNext != NULL )                                            \
        {                                                                               \
            (STL_RING_GET_LINK((headPtr)->mLink.mNext,(headPtr)->mLinkOffset))->mPrev = \
                lastDataPtr;                                                            \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            (headPtr)->mLink.mPrev = lastDataPtr;                                       \
        }                                                                               \
        (headPtr)->mLink.mNext = firstDataPtr;                                          \
    }

/**
 * @brief 주어진 headPtr의 맨 뒤에 firstDataPtr부터 lastDataPtr까지 삽입한다.
 * @param[out] headPtr Ring이 시작되는  stlRingHead 타입의 포인터
 * @param[in] firstDataPtr Ring에 삽입할 데이터의 시작 포인터
 * @param[in] lastDataPtr Ring에 삽입할 데이터의 마지막 포인터
 */
#define STL_RING_SPLICE_LAST(headPtr, firstDataPtr, lastDataPtr)                        \
    {                                                                                   \
        (STL_RING_GET_LINK(lastDataPtr,(headPtr)->mLinkOffset))->mNext = NULL;          \
        (STL_RING_GET_LINK(firstDataPtr,(headPtr)->mLinkOffset))->mPrev =               \
            (headPtr)->mLink.mPrev;                                                     \
        if( (headPtr)->mLink.mPrev != NULL )                                            \
        {                                                                               \
            (STL_RING_GET_LINK((headPtr)->mLink.mPrev,(headPtr)->mLinkOffset))->mNext = \
                firstDataPtr;                                                           \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            (headPtr)->mLink.mNext = firstDataPtr;                                      \
        }                                                                               \
        (headPtr)->mLink.mPrev = lastDataPtr;                                           \
    }

/**
 * @brief 주어진 headPtr로 시작되는 Ring에서 firstDataPtr부터 lastDataPtr까지 삭제한다
 * @param[out] headPtr Ring이 시작되는  stlRingHead 타입의 포인터
 * @param[in] firstDataPtr Ring에서 삭제할 데이터의 시작 포인터
 * @param[in] lastDataPtr Ring에서 삭제할 데이터의 마지막 포인터
 */
#define STL_RING_UNSPLICE(headPtr, firstDataPtr, lastDataPtr)                           \
    {                                                                                   \
        stlRingEntry * __sFirstLink;                                                    \
        stlRingEntry * __sLastLink;                                                     \
                                                                                        \
        __sFirstLink = STL_RING_GET_LINK((firstDataPtr),(headPtr)->mLinkOffset);        \
        __sLastLink  = STL_RING_GET_LINK((lastDataPtr),(headPtr)->mLinkOffset);         \
                                                                                        \
        if( __sFirstLink->mPrev != NULL )                                               \
        {                                                                               \
            (STL_RING_GET_LINK(__sFirstLink->mPrev,(headPtr)->mLinkOffset))->mNext =    \
                __sLastLink->mNext;                                                     \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            (headPtr)->mLink.mNext = __sLastLink->mNext;                                \
        }                                                                               \
        if( __sLastLink->mNext != NULL )                                                \
        {                                                                               \
            (STL_RING_GET_LINK(__sLastLink->mNext,(headPtr)->mLinkOffset))->mPrev =     \
                __sFirstLink->mPrev;                                                    \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            (headPtr)->mLink.mPrev =  __sFirstLink->mPrev;                              \
        }                                                                               \
    }

/**
 * @brief 주어진 nextDataPtr의 앞에 dataPtr을 삽입한다
 * @param[in] headPtr Ring이 시작되는  stlRingHead 타입의 포인터
 * @param[in,out] nextDataPtr 삽입될 위치 바로 뒤에 존재하는 데이터의 포인터
 * @param[in,out] dataPtr Ring에 삽입할 데이터의 포인터
 */
#define STL_RING_INSERT_BEFORE(headPtr,nextDataPtr,dataPtr)                                     \
    {                                                                                           \
        void * prevDataPtr = (STL_RING_GET_LINK(nextDataPtr,(headPtr)->mLinkOffset)->mPrev);    \
        (STL_RING_GET_LINK(dataPtr,(headPtr)->mLinkOffset))->mPrev = prevDataPtr;               \
        (STL_RING_GET_LINK(dataPtr,(headPtr)->mLinkOffset))->mNext = nextDataPtr;               \
        if( prevDataPtr != NULL )                                                               \
        {                                                                                       \
            (STL_RING_GET_LINK(prevDataPtr,(headPtr)->mLinkOffset))->mNext = dataPtr;           \
        }                                                                                       \
        else                                                                                    \
        {                                                                                       \
            (headPtr)->mLink.mNext = dataPtr;                                                   \
        }                                                                                       \
        (STL_RING_GET_LINK(nextDataPtr,(headPtr)->mLinkOffset))->mPrev = dataPtr;               \
    }
/**
 * @brief 주어진 prevDataPtr의 뒤에 dataPtr을 삽입한다
 * @param[in] headPtr Ring이 시작되는  stlRingHead 타입의 포인터
 * @param[in,out] prevDataPtr 삽입될 위치 바로 앞에 존재하는 데이터의 포인터
 * @param[in,out] dataPtr Ring에 삽입할 데이터의 포인터
 */
#define STL_RING_INSERT_AFTER(headPtr,prevDataPtr,dataPtr)                                      \
    {                                                                                           \
        void * nextDataPtr = (STL_RING_GET_LINK(prevDataPtr,(headPtr)->mLinkOffset)->mNext);    \
        (STL_RING_GET_LINK(dataPtr,(headPtr)->mLinkOffset))->mPrev = prevDataPtr;               \
        (STL_RING_GET_LINK(dataPtr,(headPtr)->mLinkOffset))->mNext = nextDataPtr;               \
        if( nextDataPtr != NULL )                                                               \
        {                                                                                       \
            (STL_RING_GET_LINK(nextDataPtr,(headPtr)->mLinkOffset))->mPrev = dataPtr;           \
        }                                                                                       \
        else                                                                                    \
        {                                                                                       \
            (headPtr)->mLink.mPrev = dataPtr;                                                   \
        }                                                                                       \
        (STL_RING_GET_LINK(prevDataPtr,(headPtr)->mLinkOffset))->mNext = dataPtr;               \
    }

/**
 * @brief 주어진 headPtr로 시작되는 Ring에서 dataPtr을 삭제한다
 * @param[in] headPtr Ring이 시작되는  stlRingHead 타입의 포인터
 * @param[out] dataPtr Ring에서 삭제할 데이터의 포인터
 */
#define STL_RING_UNLINK(headPtr,dataPtr)                                                \
    {                                                                                   \
        stlRingEntry * sLink = STL_RING_GET_LINK(dataPtr,(headPtr)->mLinkOffset);       \
        if( sLink->mPrev != NULL )                                                      \
        {                                                                               \
            (STL_RING_GET_LINK(sLink->mPrev,(headPtr)->mLinkOffset))->mNext             \
                = sLink->mNext;                                                         \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            (headPtr)->mLink.mNext = sLink->mNext;                                      \
        }                                                                               \
        if( sLink->mNext != NULL )                                                      \
        {                                                                               \
            (STL_RING_GET_LINK(sLink->mNext,(headPtr)->mLinkOffset))->mPrev             \
                = sLink->mPrev;                                                         \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            (headPtr)->mLink.mPrev = sLink->mPrev;                                      \
        }                                                                               \
        STL_DASSERT( (headPtr)->mLink.mPrev != (dataPtr) );                             \
        STL_DASSERT( (headPtr)->mLink.mNext != (dataPtr) );                             \
        STL_RING_INIT_DATALINK(dataPtr,(headPtr)->mLinkOffset);                         \
    }

/**
 * @brief 주어진 dataPtr이 headPtr 인지 검사한다.
 * @param[in] headPtr Ring이 시작되는  stlRingHead 타입의 포인터
 * @param[in] dataPtr ring head와 검사할 데이터의 포인터
 */
#define STL_RING_IS_HEADLINK(headPtr,dataPtr)                                   \
    ((dataPtr == (STL_RING_GET_DATA((headPtr), (headPtr)->mLinkOffset))) ?      \
     STL_TRUE : STL_FALSE)

/** @} */

/** @} */

#endif /* _STLRING_H_ */
