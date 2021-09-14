/*******************************************************************************
 * knlCondVar.h
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


#ifndef _KNLCONDVAR_H_
#define _KNLCONDVAR_H_ 1

/**
 * @file knlCondVar.h
 * @brief Kernel Layer Conditional Variable Routines
 */

/**
 * @defgroup knlCondVar Conditional Variable
 * @ingroup knExternal
 * @{
 */

stlStatus knlInitCondVar( knlCondVar  * aCondVar,
                          knlEnv      * aEnv );

void knlFinCondVar( knlCondVar * aCondVar );

stlStatus knlCondWait( knlCondVar   * aCondVar,
                       stlInterval    aTimeout,
                       stlBool      * aIsTimedOut,
                       knlEnv       * aEnv );

stlStatus knlCondBroadcast( knlCondVar  * aCondVar,
                            knlEnv      * aEnv );

/** @} */
    
#endif /* _KNLCONDVAR_H_ */
