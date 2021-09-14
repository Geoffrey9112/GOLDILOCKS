/*******************************************************************************
 * smklWaitGraph.h
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


#ifndef _SMKLWAITGRAPH_H_
#define _SMKLWAITGRAPH_H_ 1

/**
 * @file smklWaitGraph.h
 * @brief Storage Manager Layer Lock Wait Graph Internal Routines
 */

/**
 * @defgroup smklWaitGraph Lock Wait Graph
 * @ingroup smInternal
 * @{
 */

stlStatus smklInitWaitTable( stlInt64   aTransTableSize );
stlInt32 smklCheckDeadlockInternal( stlUInt16          aMyLockSlotId,
                                    stlUInt16          aTargetLockSlotId,
                                    smklDeadlockLink * aDeadlockLink );
inline void smklResetWaitSlot( smxlTransId aTransId );
inline void smklResolveDeadlock( smxlTransId aTransId );
stlInt32 smklCheckRequestGroupDeadlockInternal( stlUInt16          aMyLockSlotId,
                                                stlUInt16          aTargetLockSlotId,
                                                smklDeadlockLink * aDeadlockLink );

/** @} */
    
#endif /* _SMKLWAITGRAPH_H_ */
