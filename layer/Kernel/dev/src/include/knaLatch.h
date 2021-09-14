/*******************************************************************************
 * knaLatch.h
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


#ifndef _KNA_LATCH_H_
#define _KNA_LATCH_H_ 1

/**
 * @file knaLatch.h
 * @brief Kernel Layer Latch Internal Routines
 */

/**
 * @defgroup knaLatch Latch
 * @ingroup knInternal
 * @internal
 * @{
 */

stlChar * knaGetLatchModeString( stlUInt16 aMode );

stlStatus knaInitLatch( knlLatch      * aLatch,
                        stlBool         aIsInShm,
                        knlLogicalAddr  aLogicalAddr,
                        knlEnv        * aEnv );

void knaFinLatch( knlLatch      * aLatch );

stlStatus knaAcquireLatchInternal( knlLatch    * aLatch,
                                   stlUInt16     aMode,
                                   stlInt32      aPriority,
                                   stlInterval   aTimeout,
                                   stlInt64      aRetryCount,
                                   stlBool     * aIsTimedOut,
                                   stlBool     * aIsSuccess,
                                   knlEnv      * aEnv );

stlStatus knaEscalateLatch( knlLatch    * aLatch,
                            stlBool     * aIsSuccess,
                            knlEnv      * aEnv );

stlStatus knaReleaseLatch( knlLatch * aLatch,
                           stlInt32   aWakeupCount,
                           knlEnv   * aEnv );

void knaTrySpinLock4EnterLatch( knlLatch  * aLatch,
                                stlUInt16   aMode,
                                stlBool   * aIsSuccess,
                                knlEnv    * aEnv );

stlStatus knaSuspend( stlInterval   aTimeout,
                      stlBool     * aIsTimedOut,
                      knlEnv      * aEnv );

stlStatus knaResume( knlEnv * aTargetEnv,
                     knlEnv * aEnv );

stlStatus knaResetLostSemValue( knlEnv * aEnv );

stlStatus knaRegisterLatchScanCallback( knlLatchScanCallback * aCallback,
                                        knlEnv               * aEnv );

stlStatus openFxLatchCallback( void   * aStmt,
                               void   * aDumpObjHandle,
                               void   * aPathCtrl,
                               knlEnv * aEnv );

stlStatus closeFxLatchCallback( void   * aDumpObjHandle,
                                void   * aPathCtrl,
                                knlEnv * aEnv );

stlStatus buildRecordFxLatchCallback( void               * aDumpObjHandle,
                                      void               * aPathCtrl,
                                      knlValueBlockList  * aValueList,
                                      stlInt32             aBlockIdx,
                                      stlBool            * aTupleExist,
                                      knlEnv             * aEnv );
/** @} */
    
#endif /* _KNA_LATCH_H_ */
