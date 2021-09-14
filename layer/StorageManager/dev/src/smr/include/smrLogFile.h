/*******************************************************************************
 * smrLogFile.h
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


#ifndef _SMRLOGFILE_H_
#define _SMRLOGFILE_H_ 1

/**
 * @file smrLogFile.h
 * @brief Storage Manager Layer Recovery Log File Component Internal Routines
 */

/**
 * @defgroup smrLogFile Recovery Log File
 * @ingroup smrInternal
 * @{
 */

stlStatus smrWriteFileBlocks( smrLogStream * aLogStream,
                              void         * aBuffer,
                              stlInt64       aFlushBytes,
                              stlBool        aDoValidate,
                              stlBool      * aIsValid,
                              stlLogger    * aLogger,
                              smlEnv       * aEnv );
stlStatus smrFormatLogStreamFile( stlFile      * aLogFile,
                                  smrLogStream * aLogStream,
                                  smlEnv       * aEnv );
stlStatus smrFindFileBlock( smrLid              * aHintLid,
                            smrLsn                aTargetLsn,
                            stlChar             * aBuffer,
                            stlInt64              aBufferSize,
                            smrLid              * aTargetLid,
                            stlBool             * aFound,
                            smlEnv              * aEnv );
stlStatus smrFindFileBlockByLsn( smrLogGroup         * aLogGroup,
                                 smrLsn                aTargetLsn,
                                 stlChar             * aBuffer,
                                 stlInt64              aBufferSize,
                                 smrLid              * aTargetLid,
                                 stlBool             * aFound,
                                 smlEnv              * aEnv );
stlStatus smrValidateLogBlocks( smrLogGroup  * aLogGroup,
                                smrLogBuffer * aLogBuffer,
                                void         * aBuffer,
                                stlInt64       aFlushBytes,
                                stlInt16       aBlockSize,
                                smlEnv       * aEnv );
stlStatus smrReadFileBlocks( smrLid   * aLid,
                             stlChar  * aBuffer,
                             stlInt64   aBufferSize,
                             stlInt64 * aReadBytes,
                             stlBool  * aEndOfFile,
                             stlBool  * aEndOfLog,
                             smlEnv   * aEnv );
stlStatus smrReadFileBlocksInternal( smrLid      * aLid,
                                     smrLogGroup * aLogGroup,
                                     stlChar     * aBuffer,
                                     stlInt64      aBufferSize,
                                     stlInt64    * aReadBytes,
                                     stlBool     * aEndOfFile,
                                     stlBool     * aEndOfLog,
                                     smlEnv      * aEnv );
stlStatus smrReadOnlineFileBlocks( smrLid         * aLid,
                                   stlChar        * aBuffer,
                                   stlInt16         aWrapNo,
                                   stlInt64       * aReadBytes,
                                   stlBool        * aSwitched,
                                   stlBool        * aEndOfFile,
                                   stlBool        * aEndOfLog,
                                   smlEnv         * aEnv );

stlStatus smrLogFileSwitching( smrLogStream * aLogStream,
                               smrLsn         aLastLsn,
                               smlEnv       * aEnv );

stlStatus smrMoveWritableFileBlock(smrLid  * aHintLid,
                                   smrLid  * aTargetLid,
                                   stlBool * aEndOfFile,
                                   smlEnv  * aEnv );

stlInt32 smrGetMaxFileBlockCount( smrLogStream * aLogStream,
                                  smrLogGroup  * aLogGroup );

stlStatus smrResetLogFile( smrLogStream * aLogStream,
                           smrLid       * aRestartLid,
                           smlEnv       * aEnv );

stlStatus smrClearPartialLogBlocks( smrLid   * aPartialLid,
                                    stlInt32 * aClearBlockCount,
                                    smlEnv   * aEnv );

stlStatus smrLogFileSwitchingForce( smrLogStream * aLogStream,
                                    smrLogGroup  * aCurLogGroup,
                                    smrLsn         aLastLsn,
                                    smlEnv       * aEnv );

stlStatus smrWriteLogfileHdr( smrLogStream  * aLogStream,
                              smrLogGroup   * aLogGroup,
                              smrLogFileHdr * aLogFileHdr,
                              smlEnv        * aEnv );

stlStatus smrFindCurrentLogGroupAtDisk( smrLogStream  * aLogStream,
                                        smrLogGroup  ** aCurLogGroup,
                                        smlEnv        * aEnv );

/** @} */

#endif /* _SMRLOGFILE_H_ */
