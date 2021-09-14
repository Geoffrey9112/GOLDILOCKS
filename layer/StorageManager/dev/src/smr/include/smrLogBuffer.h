/*******************************************************************************
 * smrLogBuffer.h
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


#ifndef _SMRLOGBUFFER_H_
#define _SMRLOGBUFFER_H_ 1

/**
 * @file smrLogBuffer.h
 * @brief Storage Manager Layer Recovery Log Buffer Component Internal Routines
 */

/**
 * @defgroup smrLogBuffer Recovery Log Buffer
 * @ingroup smrInternal
 * @{
 */

stlStatus smrWriteLogInternal( smxlTransId      aTransId,
                               smrLogDataType   aLogDataType,
                               void           * aLogData,
                               stlUInt32        aLogSize,
                               stlUInt16        aPieceCount,
                               smlRid           aSegRid,
                               stlBool          aSwitchBlock,
                               smrSbsn        * aWrittenSbsn,
                               smrLsn         * aWrittenLsn,
                               smlEnv         * aEnv );
stlStatus smrWriteStreamLog( smrLogStream   * aLogStream,
                             smrLogHdr      * aLogHdr,
                             smrLogDataType   aLogDataType,
                             void           * aLogData,
                             stlBool          aUsePendingBuffer,
                             stlBool          aSwitchBlock,
                             smrLid         * aWrittenLid,
                             smrSbsn        * aWrittenSbsn,
                             smrLsn         * aWrittenLsn,
                             smlEnv         * aEnv );
stlStatus smrWriteLogToBlock( smrLogStream       * aLogStream,
                              smxmLogBlockCursor * aLogBlockCursor,
                              smrLogHdr          * aLogHdr,
                              smrLsn               aLsn,
                              smrLogDataType       aLogDataType,
                              void               * aLogData,
                              stlInt32             aLogSize,
                              stlInt32           * aWrittenBytes,
                              smrLid             * aWrittenLid,
                              smrSbsn            * aWrittenSbsn,
                              smlEnv             * aEnv );
stlStatus smrWriteStreamLogInternal( smrLogStream   * aLogStream,
                                     smrLogHdr      * aLogHdr,
                                     smrLogDataType   aLogDataType,
                                     void           * aLogData,
                                     stlBool          aPendingLog,
                                     stlBool          aWaitFlusher,
                                     smrLid         * aWrittenLid,
                                     smrSbsn        * aWrittenSbsn,
                                     smrLsn         * aWrittenLsn,
                                     stlBool        * aRetry,
                                     smlEnv         * aEnv );
stlStatus smrWriteDummyLog( smlEnv * aEnv );
stlStatus smrLogSwitching( smrLogStream * aLogStream,
                           stlBool        aFlushLog,
                           stlBool        aFileSwitching,
                           stlBool        aDoCheckpoint,
                           smlEnv       * aEnv );
stlStatus smrTryDisableLogging( smrLogStream * aLogStream,
                                smrLogGroup  * aLogGroup,
                                smlEnv       * aEnv );
stlStatus smrTryEnableLogging( smrLogStream * aLogStream,
                               smlEnv       * aEnv );
stlStatus smrFlushBuffer( smrLogStream * aLogStream,
                          smrSbsn        aTargetSbsn,
                          smlEnv       * aEnv );
stlStatus smrFlushBufferByTrans( smrLogStream * aLogStream,
                                 smrSbsn        aTargetSbsn,
                                 stlBool      * aIsSuccess,
                                 smlEnv       * aEnv );
stlStatus smrFlushBufferInternal( smrLogStream * aLogStream,
                                  smrSbsn        aTargetSbsn,
                                  smlEnv       * aEnv );
stlStatus smrCondBroadcast( smrLogStream * sLogStream,
                            smlEnv       * aEnv );
stlStatus smrWriteBufferBlocks( smrLogStream * aLogStream,
                                stlInt64       aFlushBlockCount,
                                smlEnv       * aEnv );

stlStatus smrFormatLogBuffer( void     * aBuffer,
                              stlInt64   aBufferSize,
                              stlInt16   aBlockSize,
                              smlEnv   * aEnv );
stlStatus smrFindLogInBlock( stlChar   * aBlock,
                             smrLsn      aTargetLsn,
                             stlInt16  * aBlockOffset,
                             smrLsn    * aFoundLsn,
                             stlInt32  * aLeftLogCount,
                             smlEnv    * aEnv );

stlStatus smrReadStreamLog( smrLogStream * aLogStream,
                            smrLogHdr    * aLogHdr,
                            stlChar      * aLogBody,
                            stlBool      * aEndOfLog,
                            smlEnv       * aEnv );

stlStatus smrReadChkptLog( smrLid * aChkptLid,
                           smrLsn   aChkptLsn,
                           smrLsn * aOldestLsn,
                           smlScn * aSystemScn,
                           smlEnv * aEnv );
stlStatus smrReadLogFromBlock( smrLogStream * aLogStream,
                               stlBool        aReadLogHdr,
                               stlBool      * aEndOfLog,
                               smlEnv       * aEnv );
stlInt32 smrNeedBlockCount( smrLogStream * aLogStream,
                            stlUInt32      aLogSize );
stlStatus smrWaitFlusher( smrLogStream * aLogStream,
                          smrLsn         aLsn,
                          smrSbsn        aFlushSbsn,
                          stlInt64       aSyncMode,
                          smlEnv       * aEnv );

stlStatus smrLogArchiving( smrLsn    aFlushedLsn,
                           smlEnv  * aEnv );

stlStatus smrWriteChkptBeginLog( smrLsn    aOldestLsn,
                                 smrLid  * aChkptLid,
                                 smrLsn  * aChkptLsn,
                                 smrSbsn * aChkptSbsn,
                                 smlEnv  * aEnv );
stlStatus smrWriteChkptBodyLogs( smxlTransId * aTransArr,
                                 smlRid      * aUndoSegArr,
                                 smlScn      * aScnArr,
                                 stlChar     * aStateArr,
                                 stlBool     * aRepreparableArr,
                                 stlInt32      aTransCount,
                                 smrLid      * aChkptLid,
                                 smrLsn      * aChkptLsn,
                                 smrSbsn     * aChkptSbsn,
                                 smlEnv      * aEnv );
stlStatus smrWriteChkptEndLog( smlEnv   * aEnv,
                               smrLid   * aChkptLid,
                               smrLsn   * aChkptLsn,
                               smrSbsn  * aChkptSbsn );
stlStatus smrMarkFileEnd( smrLogBuffer * aLogBuffer,
                          smrSbsn        aSbsn,
                          stlInt32       aBlockSize,
                          smlEnv       * aEnv );

stlBool smrValidateLogBuffer( smrLogBuffer * aLogBuffer,
                              stlInt32       aBlockSize );

stlStatus smrWriteDummyFileEndBlock( smrLogStream * aLogStream,
                                     smlEnv       * aEnv );

stlStatus smrAcquireLatchOrJoinGroup( smrLogStream * aLogStream,
                                      smrLsn         aLsn,
                                      smrLsn       * aGroupCommitLsn,
                                      stlBool      * aSkipFlush,
                                      smlEnv       * aEnv );

stlStatus smrGetLastChkptLsn( smrLogStream  * aLogStream,
                              smrLsn        * aLastChkptLsn,
                              smlEnv        * aEnv );

/** @} */

#endif /* _SMRLOGBUFFER_H_ */
