/*******************************************************************************
 * knlQSort.h
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


#ifndef _KNLQSORT_H_
#define _KNLQSORT_H_ 1

/**
 * @file knlQSort.h
 * @brief Kernel Layer Quick Sort Routines
 */

/**
 * @defgroup knlQSort Quick Sort
 * @ingroup knExternal
 * @{
 */
void knlQSort( void            * aList,
               stlInt32          aElemCount,
               knlQSortContext * aContext );

/** @} */
    
#endif /* _KNLLATCH_H_ */
