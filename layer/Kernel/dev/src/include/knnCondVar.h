/*******************************************************************************
 * knnCondVar.h
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


#ifndef _KNN_CONDVAR_H_
#define _KNN_CONDVAR_H_ 1

/**
 * @file knnCondVar.h
 * @brief Kernel Layer Conditional Variable Internal Routines
 */

/**
 * @defgroup knnCondVar Conditional Variable
 * @ingroup knInternal
 * @internal
 * @{
 */

stlStatus knnInitCondVar( knlCondVar  * aCondVar,
                          knlEnv      * aEnv );

void knnFinCondVar( knlCondVar * aCondVar );

stlStatus knnCondWait( knlCondVar  * aCondVar,
                       stlInterval   aTimeout,
                       stlBool     * aIsTimedOut,
                       knlEnv      * aEnv );

stlStatus knnCondBroadcast( knlCondVar * aCondVar,
                            knlEnv    * aEnv );

/** @} */
    
#endif /* _KNN_CONDVAR_H_ */
