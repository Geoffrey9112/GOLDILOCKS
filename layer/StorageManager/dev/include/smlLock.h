/*******************************************************************************
 * smlLock.h
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


#ifndef _SMLLOCK_H_
#define _SMLLOCK_H_ 1

/**
 * @file smlLock.h
 * @brief Storage Manager Lock Routines
 */

/**
 * @defgroup smlLock Lock
 * @ingroup SML
 * @{
 */

stlStatus smlLockTable( smlTransId   aTransId,
                        stlInt32     aLockMode,
                        void       * aRelationHandle,
                        stlInt64     aTimeInterval,
                        stlBool    * aTableExist,
                        smlEnv     * aEnv );

stlStatus smlLockRecordForDdl( smlStatement * aStatement,
                               void         * aRelationHandle,
                               stlInt32       aLockMode,
                               smlRid         aRid,
                               stlBool      * aRecordExist,
                               smlEnv       * aEnv );

stlStatus smlLockRecordForUpdate( smlStatement * aStatement,
                                  void         * aRelationHandle,
                                  smlRid         aRowId,
                                  smlScn         aRowScn,
                                  stlInt64       aTimeInterval,
                                  stlBool      * aVersionConflict,
                                  smlEnv       * aEnv );

void smlAddEdgeIntoWaitGraph( smlTransId   aTransId,
                              stlInt32     aRequestGroupId );

void smlRemoveEdgeFromWaitGraph( smlTransId   aTransId,
                                 stlInt32     aRequestGroupId );

/** @} */

#endif /* _SMLLOCK_H_ */
