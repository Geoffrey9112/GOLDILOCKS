/*******************************************************************************
 * smklLockNode.h
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


#ifndef _SMKLLOCKNODE_H_
#define _SMKLLOCKNODE_H_ 1

/**
 * @file smklLockNode.h
 * @brief Storage Manager Layer Lock Node Internal Routines
 */

/**
 * @defgroup smklLockNode Lock Node
 * @ingroup smInternal
 * @{
 */

smklNode * smklFindLockNode( smxlTransId    aTransId,
                             smklItemCore * aLockItem );
stlStatus smklRemoveLockNode( smklNode * aLockNode );
stlStatus smklAddLockNode( smxlTransId     aTransId,
                           smklLockSlot  * aLockSlot,
                           smklNode      * aHoldLockNode,
                           stlUInt32       aLockMode,
                           smklNode     ** aReqLockNode,
                           stlBool       * aWaitFlag,
                           smlEnv        * aEnv );
stlStatus smklAddNewLockNode( smxlTransId    aTransId,
                              smklLockSlot * aLockSlot,
                              smklItemCore * aLockItem,
                              stlUInt32      aLockMode,
                              smklNode    ** aLockNode,
                              stlBool      * aWaitFlag,
                              smlEnv       * aEnv );
stlStatus smklCheckLockConflict( smklItemCore * aLockItem,
                                 smklNode     * aLockNode,
                                 stlUInt32      aLockMode,
                                 smlEnv       * aEnv );

/** @} */
    
#endif /* _SMKLLOCKNODE_H_ */
