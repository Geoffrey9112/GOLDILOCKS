/*******************************************************************************
 * smrArchiveLog.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smrArchiveLog.h 6651 2012-12-12 02:55:34Z mkkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/


#ifndef _SMRARCHIVELOG_H_
#define _SMRARCHIVELOG_H_ 1

/**
 * @file smrArchiveLog.h
 * @brief Storage Manager Layer Recovery Log File Component Internal Routines
 */

/**
 * @defgroup smrArchiveLog Recovery Log File
 * @ingroup smrInternal
 * @{
 */
stlStatus smrValidateLogfileByName( stlChar             * aFileName,
                                    smrLogGroupPersData * sLogGroupPersData,
                                    stlUInt32             sValidationFlag,
                                    stlBool             * aIsValid,
                                    stlBool             * aIsExist,
                                    smlEnv              * aEnv );

stlStatus smrValidateArchiveLogfile( stlInt64   aLogFileSeq,
                                     stlChar  * aFileName,
                                     stlBool  * aIsValid,
                                     stlBool  * aIsExist,
                                     smlEnv   * aEnv );

stlStatus smrGetValidArchiveLogfileBySeqNo( stlInt64   aLogFileSeq,
                                            stlChar  * aFileName,
                                            stlBool  * aIsValid,
                                            stlBool  * aIsExist,
                                            smlEnv   * aEnv );

stlStatus smrLogFileArchiving( smrLogStream   * aLogStream,
                               smrLogGroup    * aLogGroup,
                               smlEnv         * aEnv );

stlStatus smrCheckArchiveLogfile( stlChar   * aSource,
                                  stlChar   * aTarget,
                                  stlBool   * aIsValid,
                                  smlEnv    * aEnv );

stlStatus smrOpenFile4MediaRecovery( stlChar  * aFileName,
                                     stlBool  * aValidFile,
                                     stlBool  * aExistFile,
                                     stlFile  * aFile,
                                     smlEnv   * aEnv );

stlStatus smrOpenArchiveLogfile( stlInt64   aFileSeqNo,
                                 stlBool  * aValidFile,
                                 stlBool  * aExistFile,
                                 stlFile  * aFile,
                                 smlEnv   * aEnv );

stlStatus smrReadArchiveFileBlocks( smrLid         * aLid,
                                    stlChar        * aBuffer,
                                    stlInt64       * aReadBytes,
                                    stlBool        * aEndOfFile,
                                    stlBool        * aEndOfLog,
                                    smlEnv         * aEnv );
stlStatus smrFindArchiveFileBlockByLsn( stlInt64   aFileSeqNo,
                                        smrLsn     aTargetLsn,
                                        stlChar  * aBuffer,
                                        stlInt64   aBufferSize,
                                        smrLid   * aRedoLid,
                                        stlBool  * aExist,
                                        smlEnv   * aEnv );

stlStatus smrMakeArchiveLogfileName( stlInt64   aFileSeqNo,
                                     stlInt32   aArchivelogDirId,
                                     stlChar  * aFileName,
                                     smlEnv   * aEnv );

stlStatus smrGetLogfile( smrLsn     aCurLsn,
                         stlInt64   aFileSeqNo,
                         stlBool  * aExistFile,
                         smlEnv   * aEnv );

stlStatus smrPushWarning4IncompleteRecovery( smrLsn     aSuggestLsn,
                                             stlInt64   aSuggestFileSeqNo,
                                             smlEnv   * aEnv );

/** @} */

#endif /* _SMRARCHIVELOG_H_ */
