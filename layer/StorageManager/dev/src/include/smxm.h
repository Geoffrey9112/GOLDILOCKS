/*******************************************************************************
 * smxm.h
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


#ifndef _SMXM_H_
#define _SMXM_H_ 1

/**
 * @file smxm.h
 * @brief Storage Manager Layer Mini-Transaction Log Block Internal Routines
 */

/**
 * @defgroup smxm Mini-Transaction
 * @ingroup smInternal
 * @{
 */

stlStatus smxmCopyLogFromBlock( smxmLogBlockCursor * aLogBlockCursor,
                                void               * aLogBlock,
                                void               * aLogBuffer,
                                stlUInt32            aReadSize,
                                smlEnv             * aEnv );

stlStatus smxmBegin( smxmTrans   * aMiniTrans,
                     smxlTransId   aTransId,
                     smlRid        aSegRid,
                     stlInt32      aAttr,
                     smlEnv      * aEnv );
stlStatus smxmCommit( smxmTrans * aMiniTrans,
                      smlEnv    * aEnv );
stlStatus smxmSyncCommit( smxmTrans * aMiniTrans,
                          smlEnv    * aEnv );
stlStatus smxmCommitWithInfo( smxmTrans * aMiniTrans,
                              smrSbsn   * aSbsn,
                              smrLsn    * aLsn,
                              smlEnv    * aEnv );
stlStatus smxmSnapshotAndCommit( smxmTrans      * aMiniTrans,
                                 smpCtrlHeader  * aPageStack,
                                 smxmTrans      * aSnapshotMtx,
                                 smlEnv         * aEnv );
stlStatus smxmBeginWithSnapshot( smxmTrans      * aMiniTrans,
                                 smxmTrans      * aSnapshotMtx,
                                 smlEnv         * aEnv );
stlStatus smxmRollback( smxmTrans * aMiniTrans,
                        smlEnv    * aEnv );
smpHandle * smxmGetPageHandle( smxmTrans * aMiniTrans,
                               smlTbsId    aTbsId,
                               smlPid      aPageId,
                               stlUInt16   aLatchMode );
stlStatus smxmMarkSavepoint( smxmTrans     * aMiniTrans,
                             smxmSavepoint * aSavepoint,
                             smlEnv        * aEnv );
stlStatus smxmRollbackToSavepoint( smxmTrans     * aMiniTrans,
                                   smxmSavepoint * aSavepoint,
                                   smlEnv        * aEnv );
stlStatus smxmWriteLog( smxmTrans   * aMiniTrans,
                        stlChar       aComponentClass,
                        stlUInt16     aLogType,
                        stlChar       aRedoTargetType,
                        stlChar     * aLogBody,
                        stlUInt16     aLogSize,
                        smlTbsId      aTbsId,
                        smlPid        aPageId,
                        stlUInt16     aOffset,
                        stlUInt32     aLoggingFlag,
                        smlEnv      * aEnv );
stlStatus smxmWriteTbsLog( smxmTrans   * aMiniTrans,
                           stlChar       aComponentClass,
                           stlUInt16     aLogType,
                           stlChar       aRedoTargetType,
                           stlChar     * aLogBody,
                           stlUInt16     aLogSize,
                           smlTbsId      aTbsId,
                           smlPid        aPageId,
                           stlUInt16     aOffset,
                           smlEnv      * aEnv );
void smxmPushPage( smxmTrans * aMiniTrans,
                   stlUInt32   aLatchMode,
                   smpHandle   aHandle,
                   smlEnv    * aEnv );
inline void smxmSetSegmentRid( smxmTrans * aMiniTrans,
                               smlRid      aSegmentRid );
smxlTransId smxmGetTransId( smxmTrans * aMiniTrans );
smlRid smxmGetSegRid( smxmTrans * aMiniTrans );
stlUInt32 smxmGetAttr( smxmTrans * aMiniTrans );

stlStatus smxmResetAttr( smxmTrans * aMiniTrans,
                         stlInt32    aAttr,
                         smlEnv    * aEnv );

inline stlInt16 smxmGetLatchedPageCount( smxmTrans * aMiniTrans );

/** @} */
    
#endif /* _SMXM_H_ */
