/*******************************************************************************
 * knlLatch.h
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


#ifndef _KNLLATCH_H_
#define _KNLLATCH_H_ 1

/**
 * @file knlLatch.h
 * @brief Kernel Layer Latch Routines
 */

/**
 * @defgroup knlLatch Latch
 * @ingroup knExternal
 * @{
 */

stlStatus knlInitLatch( knlLatch      * aLatch,
                        stlBool         aIsInShm,
                        knlEnv        * aEnv );

stlStatus knlInitDbLatch( knlLatch      * aLatch,
                          stlUInt16       aShmIdx,
                          knlEnv        * aEnv );

void knlFinLatch( knlLatch      * aLatch );

stlStatus knlAcquireLatch( knlLatch    * aLatch,
                           stlUInt16     aMode,
                           stlInt32      aPriority,
                           stlInterval   aTimeout,
                           stlBool     * aIsTimedOut,
                           knlEnv      * aEnv );

stlStatus knlAcquireLatchWithInfo( knlLatch    * aLatch,
                                   stlUInt16     aMode,
                                   stlInt32      aPriority,
                                   stlInterval   aTimeout,
                                   stlInt64      aRetryCount,
                                   stlBool     * aIsTimedOut,
                                   stlBool     * aIsSuccess,
                                   knlEnv      * aEnv );

stlStatus knlReleaseLatchWithInfo( knlLatch * aLatch,
                                   stlInt32   aExclWakeupCount,
                                   knlEnv   * aEnv );

stlStatus knlEscalateLatch( knlLatch * aLatch,
                            stlBool  * aIsSuccess,
                            knlEnv   * aEnv );

stlStatus knlTryLatch( knlLatch  * aLatch,
                       stlUInt16   aMode,
                       stlBool   * aIsSuccess,
                       knlEnv    * aEnv );

stlStatus knlReleaseLatch( knlLatch * aLatch,
                           knlEnv   * aEnv );

stlStatus knlSuspend( stlInterval   aTimeout,
                      stlBool     * aIsTimedOut,
                      knlEnv      * aEnv );

stlStatus knlResume( knlEnv * aTargetEnv,
                     knlEnv * aEnv );

stlStatus knlResetLostSemValue( knlEnv * aEnv );

stlUInt16 knlCurLatchMode( knlLatch * aLatch );

stlUInt64 knlGetExclLockSeq( knlLatch * aLatch );

stlStatus knlRegisterLatchScanCallback( knlLatchScanCallback * aCallback,
                                        knlEnv               * aEnv );

/** @} */
    
#endif /* _KNLLATCH_H_ */
