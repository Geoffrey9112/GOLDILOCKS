/*******************************************************************************
 * smrMediaRecoveryMgr.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smrMediaRecoveryMgr.h 6651 2012-12-12 02:55:34Z mkkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/


#ifndef _SMRMEDIARECOVERYMGR_H_
#define _SMRMEDIARECOVERYMGR_H_ 1

/**
 * @file smrMediaRecoveryMgr.h
 * @brief Storage Manager Layer Media Recovery Internal Routines
 */

/**
 * @defgroup smrMediaRecoveryMgr Media Recovery
 * @ingroup smrInternal
 * @{
 */

stlStatus smrGetMediaRecoveryBeginLsn( smrLsn   * aMinChkptLsn,
                                       smrLid   * aHintLid,
                                       smlEnv   * aEnv );

stlStatus smrMediaRecoverInternal( smrLsn      aUntilLsn,
                                   smrLsn    * aLastLsn,
                                   stlBool   * aIsFinishedRecovery,
                                   smlEnv    * aEnv );
stlStatus smrAnalysis4MediaRecovery( smrLogCursor   * aLogCursor,
                                     smrLsn           aMinChkptLsn,
                                     smrLid           aHintLid,
                                     smrLsn         * aOldestLsn,
                                     smrLid         * aRedoLid,
                                     stlInt64       * aFileSeqNo,
                                     smlEnv         * aEnv );
stlStatus smrPrepare4MediaRecovery( smrLid           aRedoLid,
                                    smrLsn           aOldestLsn,
                                    stlInt64         aFileSeqNo,
                                    smlEnv         * aEnv );
stlStatus smrFindFileBlock4MediaRecovery( smrLid        aChkptLid,
                                          smrLsn        aMinChkptLsn,
                                          stlChar     * aBuffer,
                                          stlInt64      aBufferSize,
                                          stlInt64    * aFileSeqNo,
                                          smrLid      * aRedoLid,
                                          smlEnv      * aEnv );
stlStatus smrReadFileBlocks4MediaRecovery( smrLid         * aLid,
                                           stlInt16         aWrapNo,
                                           stlChar        * aBuffer,
                                           stlInt64       * aReadBytes,
                                           stlBool        * aEndOfFile,
                                           stlBool        * aEndOfLog,
                                           smlEnv         * aEnv );
stlStatus smrReadNextLog4MediaRecovery( smrLogCursor   * aLogCursor,
                                        stlBool        * aEndOfLog,
                                        stlBool        * aEndOfFile,
                                        smlEnv         * aEnv );
stlStatus smrReadLog4MediaRecovery( smrLogHdr      * aLogHdr,
                                    stlChar        * aLogBody,
                                    stlBool        * aEndOfLog,
                                    stlBool        * aEndOfFile,
                                    smlEnv         * aEnv );
stlStatus smrReadLogFromBlock4MediaRecovery( stlBool          aReadLogHdr,
                                             stlBool        * aEndOfLog,
                                             stlBool        * aEndOfFile,
                                             smlEnv         * aEnv );

stlStatus smrRedo4MediaRecovery( smrLsn     aOldestLsn,
                                 smrLsn     aUntilLsn,
                                 smrLsn   * aLastLsn,
                                 smlEnv   * aEnv );

stlStatus smrUndo4MediaRecovery( smlEnv * aEnv );

stlBool smrIsRedoFinished( smrLogCursor * aLogCursor,
                           smrLsn         aUntilLsn,
                           smlEnv       * aEnv );

stlBool smrIsIncompleteRecoveryFinished( smrLogCursor  * aLogCursor,
                                         smlEnv        * aEnv );

stlStatus smrReadChkptLog4MediaRecovery( smrLid     aChkptLid,
                                         smrLsn     aChkptLsn,
                                         smrLid   * aHintRedoLid,
                                         smrLsn   * aOldestLsn,
                                         smlScn   * aSystemScn,
                                         stlInt64 * aFileSeqNo,
                                         smlEnv   * aEnv );

smlTbsId smrGetTbsId4MediaRecovery( smlEnv * aEnv );

stlStatus smrGetMinChkptLsn( smlTbsId    aTbsId,
                             smrLid    * aHintLid,
                             smrLsn    * aMinChkptLsn,
                             smlEnv    * aEnv );

stlStatus smrGetRestoredLsn( smrLid    * aChkptLid,
                             smrLsn    * aMinChkptLsn,
                             smlEnv    * aEnv );

stlUInt64 smrGetMediaRecoveryPhase( smlEnv  * aEnv );
void smrSetMediaRecoveryPhase( stlUInt64   aPhase,
                               smlEnv    * aEnv );
inline stlUInt8 smrGetImrCondition( smlEnv  * aEnv );

/** @} */

#endif /* _SMRMEDIARECOVERYMGR_H_ */
