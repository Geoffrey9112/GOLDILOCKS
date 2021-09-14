/*******************************************************************************
 * smrLogSyncher.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smrLogSyncher.h 13496 2014-08-29 05:38:43Z leekmo $
 *
 * NOTES
 *
 *
 ******************************************************************************/


#ifndef _SMRLOGSYNCHER_H_
#define _SMRLOGSYNCHER_H_ 1

/**
 * @file smrLogSyncher.h
 * @brief Storage Manager Layer Log Buffer Syncher Component Internal Routines
 */

/**
 * @defgroup smrLogSyncher Log buffer syncher
 * @ingroup smrInternal
 * @{
 */


stlStatus smrClearChainedLog( smrLogBuffer  * aLogBuffer,
                              smrSbsn         aSbsn,
                              smlEnv        * aEnv );

stlStatus smrClearPartialLog4Syncher( smrWarmupEntry * aWarmupEntry,
                                      smlEnv         * aEnv );

stlStatus smrRefineBufferPos( smrLogStream * aLogStream,
                              stlLogger    * aLogger,
                              smlEnv       * aEnv );

stlStatus smrSwitchBufferBlock( smrLogStream * aLogStream,
                                smlEnv       * aEnv );

stlStatus smrFlushBuffer4Syncher( smrLogStream * aLogStream,
                                  smrSbsn        aTargetSbsn,
                                  stlBool      * aSwitchedLogfile,
                                  stlBool        aSilent,
                                  stlLogger    * aLogger,
                                  smlEnv       * aEnv );

stlStatus smrWriteBufferBlocks4Syncher( smrLogStream * aLogStream,
                                        stlInt64       aFlushBlockCount,
                                        stlBool      * aSwitchedLogfile,
                                        stlLogger    * aLogger,
                                        smlEnv       * aEnv );

stlStatus smrWriteLogToBlock4Syncher( smrLogStream  * aLogStream,
                                      smrLogHdr     * aLogHdr,
                                      smrLsn          aLsn,
                                      void          * aLogData,
                                      stlInt32        aLogSize,
                                      stlInt32      * aWrittenBytes,
                                      smrSbsn       * aWrittenSbsn,
                                      smlEnv        * aEnv );

stlStatus smrApplyPendingLogs4Syncher( smrLogStream * aLogStream,
                                       smrLsn         aEndLsn,
                                       smrSbsn      * aSbsn,
                                       stlBool      * aSwitchedLogfile,
                                       stlBool        aSilent,
                                       stlLogger    * aLogger,
                                       smlEnv       * aEnv );

stlStatus smrWritePendingLogToLogBuffer( smrLogStream   * aLogStream,
                                         smrLogHdr      * aLogHdr,
                                         void           * aLogData,
                                         smrSbsn        * aWrittenSbsn,
                                         stlBool        * aSwitchedLogfile,
                                         stlBool          aSilent,
                                         stlLogger      * aLogger,
                                         smlEnv         * aEnv );

stlStatus smrLogSwitching4Syncher( smrLogStream * aLogStream,
                                   stlBool      * aSwitchedLogfile,
                                   stlLogger    * aLogger,
                                   smlEnv       * aEnv );

/** @} */

#endif /* _SMRLOGSYNCHER_H_ */
