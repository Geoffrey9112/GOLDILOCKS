/*******************************************************************************
 * knoDef.h
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


#ifndef _KNO_DEF_H_
#define _KNO_DEF_H_ 1

/**
 * @file knoDef.h
 * @brief Kernel Layer 의 Quick Sort를 위한 내부 자료 구조 정의
 */


/**
 * @defgroup KNO Quick Sort
 * @ingroup knInternal
 * @internal
 * @{
 */
/** @} */

/**
 * @defgroup KNODEF Quick Sort 내부 자료 구조
 * @ingroup KNO
 * @internal
 * @{
 */

/**
 * @brief recursive function call을 없애기 위해 사용하는 스택의 크기
 */
#define KNO_QSORT_STACK_SIZE 32

/**
 * @brief 너무 작은 크기의 리스트를 quick sort하는 것을 방지하기 위해
 * 일정 크기 이하의 리스트는 sort하지 않고, 나중에 insertion sort를 한다.
 */
#define KNO_QSORT_CUTOFF 32

/**
 * @brief 두 offset 값을 교환한다
 */
#define KNO_QSORT_SWAP_OFFSET( aValA, aValB, aTemp) \
{                                                   \
    aTemp = aValA;                                  \
    aValA = aValB;                                  \
    aValB = aTemp;                                  \
}

/**
 * @brief 두 포인터의 pointer 값을 교환한다
 */
#define KNO_QSORT_SWAP_POINTER( aValA, aValB, aTemp)    \
{                                                       \
    aTemp = aValA;                                      \
    aValA = aValB;                                      \
    aValB = aTemp;                                      \
}

/**
 * @brief partition()에의해 나뉘어진 리스트의 구분지점
 */
typedef struct knoPivots {
    stlInt32   mLeft;  /**< 오른쪽(큰 값) 리스트의 시작 sequence 번호 */
    stlInt32   mRight; /**< 왼쪽(작은 값) 리스트의 종료 sequence 번호 */

} knoPivots;


/** @} */

#endif /* _KNX_DEF_H_ */
