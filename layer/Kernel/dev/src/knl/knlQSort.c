/*******************************************************************************
 * knlQSort.c
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
 * @file knlQSort.c
 * @brief Kernel Layer Quick Sort wrapper routines
 */

#include <stl.h>
#include <dtl.h>
#include <knlDef.h>
#include <knoQSort.h>

/**
 * @brief 주어진 래치의 멤버변수들을 초기화 한다
 * @param[in] aList quick sort할 element들을 담고있는 list의 포인터.
 * 실제로는 void**(pointer sort)이거나 stlUInt16*(offset sort)이다
 * @param[in] aElemCount aList에 담긴 element의 총 개수
 * @param[out] aContext quick sort 방법과 특성등을 기술한 정보
 */
void knlQSort( void            * aList,
               stlInt32          aElemCount,
               knlQSortContext * aContext )
{
    if( aContext->mElemType == KNL_QSORT_ELEM_2BYTE )
    {
        knoQSortOffset( (stlUInt16*)aList, aElemCount, aContext );
    }
    else
    {
        knoQSortPointer( (void**)aList, aElemCount, aContext );
    }
}

/** @} */
