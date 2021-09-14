/*******************************************************************************
 * smklManager.h
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


#ifndef _SMKLMANAGER_H_
#define _SMKLMANAGER_H_ 1

/**
 * @file smklManager.h
 * @brief Storage Manager Layer Local Lock Manager Internal Routines
 */

/**
 * @defgroup smklManager Local Lock Manager
 * @ingroup smInternal
 * @{
 */

inline smklLockSlot * smklGetLockSlot( stlInt16 aLockSlotId );
inline stlInt16 smklGetLockSlotId( smxlTransId aTransId );
stlStatus smklAddRelation( smxlTransId    aTransId,
                           smklLockSlot * aLockSlot,
                           void         * aRelationHandle,
                           smlEnv       * aEnv );
stlStatus smklInitLockSlots( stlInt64     aTransTableSize,
                             smlEnv     * aEnv );
stlStatus smklLockPessimistic( smxlTransId    aTransId,
                               smklItemCore * aLockItem,
                               stlUInt32      aLockMode,
                               stlInt64       aTimeInterval,
                               knlLatch     * aLatch,
                               stlInt32       aLatchCount,
                               stlBool      * aSkipLock,
                               smlEnv       * aEnv );
stlStatus smklUnlockNode( smklNode * aLockNode,
                          smlEnv   * aEnv );

stlBool smklValidateLockItem( smklItemCore * aLockItem );

/** @} */
    
#endif /* _SMKLMANAGER_H_ */
