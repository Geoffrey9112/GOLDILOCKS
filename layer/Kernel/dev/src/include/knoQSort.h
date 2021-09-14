/*******************************************************************************
 * knoQSort.h
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


#ifndef _KNOQSORT
#define _KNOQSORT_H_ 1

/**
 * @file knoQSort.h
 * @brief Kernel Layer Quick Sort Internal Routines
 */

/**
 * @defgroup knoQSort Quick Sort 내부 처리 루틴
 * @ingroup KNO
 * @internal
 * @{
 */

void knoQSortOffset( stlUInt16       * aList,
                     stlInt32          aElemCount,
                     knlQSortContext * aContext );

void knoQSortPointer( void            ** aList,
                      stlInt32           aElemCount,
                      knlQSortContext  * aContext );

/** @} */
    
#endif /* _KNA_LATCH_H_ */
