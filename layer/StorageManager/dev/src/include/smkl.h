/*******************************************************************************
 * smkl.h
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


#ifndef _SMKL_H_
#define _SMKL_H_ 1

/**
 * @file smkl.h
 * @brief Storage Manager Layer Local Lock Internal Routines
 */

/**
 * @defgroup smkl Local Lock
 * @ingroup smInternal
 * @{
 */

stlStatus smklStartupNoMount( void   ** aWarmupEntry,
                              smlEnv  * aEnv );
stlStatus smklStartupMount( smlEnv  * aEnv );
stlStatus smklWarmup( void     * aWarmupEntry,
                      smlEnv   * aEnv );
stlStatus smklCooldown( smlEnv * aEnv );
stlStatus smklBootdown( smlEnv * aEnv );
stlStatus smklShutdownDismount( smlEnv * aEnv );
stlStatus smklShutdownClose( smlEnv * aEnv );
stlStatus smklRegisterFxTables( smlEnv * aEnv );

stlStatus smklLock( smxlTransId  aTransId,
                    stlInt32     aLockMode,
                    void       * aRelationHandle,
                    stlInt64     aTimeInterval,
                    stlBool    * aTableExist,
                    smlEnv     * aEnv );
stlStatus smklLockRecord( smxlTransId  aTransId,
                          stlInt32     aLockMode,
                          smlRid       aRid,
                          stlInt64     aTimeInterval,
                          smlEnv     * aEnv );
stlStatus smklWaitRecord( smxlTransId  aTransId,
                          stlInt32     aLockMode,
                          smlRid       aRid,
                          stlInt64     aTimeInterval,
                          smlEnv     * aEnv );
stlStatus smklIsActiveLockRecord( smxlTransId   aTransId,
                                  smlRid        aRid,
                                  void       ** aLockItem,
                                  stlBool     * aIsMyTrans,
                                  smlEnv      * aEnv );
stlStatus smklUnlock( smxlTransId  aTransId,
                      smlEnv     * aEnv );
stlStatus smklUnlockRecord( smxlTransId  aTransId,
                            smlRid       aRid,
                            smlEnv     * aEnv );
stlStatus smklSavepoint( smxlTransId        aTransId,
                         smlLockSavepoint * aSavepoint,
                         smlEnv           * aEnv );
stlStatus smklUnlockForSavepoint( smxlTransId        aTransId,
                                  smlLockSavepoint * aSavepoint,
                                  smlEnv           * aEnv );
stlStatus smklFinLock( stlInt16   aLockSlotId,
                       smlEnv   * aEnv );
stlStatus smklAllocLockHandle( void      * aRelationHandle,
                               void     ** aLockHandle,
                               smlEnv    * aEnv );
stlStatus smklFreeLockHandle( void   * aRelationHandle,
                              smlEnv * aEnv );
stlStatus smklAgingLockHandle( void   * aLockItem,
                               smlEnv * aEnv );
stlStatus smklMarkFreeLockHandle( void   * aLockHandle,
                                  smlEnv * aEnv );
stlStatus smklWaitTransLock( smxlTransId  aTransIdA,
                             smxlTransId  aTransIdB,
                             stlInt64     aTimeInterval,
                             smlEnv     * aEnv );
stlStatus smklWakeupWaitTrans( smxlTransId  aTransId,
                               smlEnv     * aEnv );
stlStatus smklCheckDeadlock( smxlTransId  aTransId,
                             stlBool    * aIsDeadlock,
                             smlEnv     * aEnv );
stlStatus smklRestructure( smlEnv * aEnv );
stlStatus smklAdjustGlobalVariables( smlEnv * aEnv );

void smklAddEdgeIntoWaitGraph( stlInt32    aTransSlotId,
                               stlInt32    aRequestGroupId );
void smklRemoveEdgeFromWaitGraph( stlInt32    aTransSlotId,
                                  stlInt32    aRequestGroupId );

void smklIncRequestGroupWorker( stlInt32 aRequestGroupId );
void smklDecRequestGroupWorker( stlInt32 aRequestGroupId );

void smklGetSystemInfo( smklSystemInfo * aSystemInfo );

/** @} */
    
#endif /* _SMKL_H_ */
