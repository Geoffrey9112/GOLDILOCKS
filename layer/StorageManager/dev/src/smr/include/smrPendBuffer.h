/*******************************************************************************
 * smrPendBuffer.h
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


#ifndef _SMRPENDBUFFER_H_
#define _SMRPENDBUFFER_H_ 1

/**
 * @file smrPendBuffer.h
 * @brief Storage Manager Layer Recovery Pending Log Buffer Component Internal Routines
 */

/**
 * @defgroup smrPendBuffer Recovery Log Buffer
 * @ingroup smrInternal
 * @{
 */

stlStatus smrFormatPendLogBuffer( smrPendLogBuffer * aPendLogBuffer,
                                  smlEnv           * aEnv );

stlStatus smrInitPendLogBuffer( smrPendLogBuffer * aPendLogBuffer,
                                smlEnv           * aEnv );

stlStatus smrWritePendingLog( smrLogStream   * aLogStream,
                              smrLogHdr      * aLogHdr,
                              smrLogDataType   aLogDataType,
                              void           * aLogData,
                              stlBool        * aRetry,
                              smrLsn         * aWrittenLsn,
                              smlEnv         * aEnv );

stlStatus smrWaitPendingLog( smrLogStream * aLogStream,
                             smrLsn         aEndLsn,
                             smlEnv       * aEnv );

stlStatus smrReadPendingLog( smrLogStream      * aLogStream,
                             smrPendLogCursor  * aPendLogCursor,
                             smrLogHdr        ** aLogHdr,
                             void             ** aLogData,
                             smlEnv            * aEnv );

stlStatus smrPreparePendingLogCursor( smrLogStream     * aLogStream,
                                      smrLsn             aEndLsn,
                                      smrPendLogCursor * aPendLogCursor,
                                      smlEnv           * aEnv );

stlStatus smrApplyPendingLogs( smrLogStream * aLogStream,
                               smrLsn         aEndLsn,
                               smrSbsn      * aSbsn,
                               stlBool      * aRetry,
                               stlBool        aWaitFlusher,
                               smlEnv       * aEnv );

stlStatus smrMovePendingLogCursor( smrPendLogCursor * aPendLogCursor,
                                   smlEnv           * aEnv );

stlInt32 smrNeedPendBlockCount( stlUInt32 aLogBodySize );

/** @} */

#endif /* _SMRPENDBUFFER_H_ */
