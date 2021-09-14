/*******************************************************************************
 * sma.h
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


#ifndef _SMA_H_
#define _SMA_H_ 1

/**
 * @file sma.h
 * @brief Storage Manager Layer Statement Internal Routines
 */

/**
 * @defgroup sma Statement
 * @ingroup smInternal
 * @{
 */

stlStatus smaAllocStatement( smxlTransId     aTransId,
                             smlTransId      aSmlTransId,
                             void          * aUpdataRelHandle,
                             stlInt32        aAttribute,
                             stlInt64        aLockTimeout,
                             stlBool         aNeedSnapshotIterator,
                             smlStatement ** aStatement,
                             smlEnv        * aEnv );

stlStatus smaUpdateStmtState( smlEnv * aEnv );

stlStatus smaFreeStatement( smlStatement * aStatement,
                            smlEnv       * aEnv );

stlStatus smaRollbackStatement( smlStatement * aStatement,
                                stlBool        aReleaseLock,
                                smlEnv       * aEnv );

stlStatus smaResetStatement( smlStatement * aStatement,
                             stlBool        aDoRollback,
                             smlEnv       * aEnv );

stlStatus smaCleanupActiveStatements( smlEnv * aEnv );

stlStatus smaAllocIterator( smlStatement              * aStatement,
                            void                      * aRelationHandle,
                            smlTransReadMode            aTransReadMode,
                            smlStmtReadMode             aStmtReadMode,
                            smlIteratorProperty       * aProperty,
                            smlIteratorScanDirection    aScanDirection,
                            void                     ** aIterator,
                            smlEnv                    * aEnv );

stlStatus smaFreeIterator( void   * aIterator,
                           smlEnv * aEnv );

stlStatus smaCloseNonHoldableIterators( stlInt64   aTimestamp,
                                        smlEnv   * aEnv );

stlStatus smaUpdateTransInfoOnReadOnlyStmt( smxlTransId  aTransId,
                                            smlScn       aCommitScn,
                                            smlEnv     * aEnv );

stlStatus smaUpdateTransIdOnStmt( smxlTransId  aTransId,
                                  smlEnv     * aEnv );

stlStatus smaAllocRegionMem( smlStatement * aStatement,
                             stlInt32       aSize,
                             void        ** aAddr,
                             smlEnv       * aEnv );

stlStatus smaCheckMutating( smlStatement * aMyStatement,
                            void         * aTargetRelHandle,
                            smlEnv       * aEnv );


/** @} */
    
#endif /* _SMA_H_ */
