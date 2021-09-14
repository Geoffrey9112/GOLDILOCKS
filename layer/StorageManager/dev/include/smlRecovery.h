/*******************************************************************************
 * smlRecovery.h
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


#ifndef _SMLRECOVERY_H_
#define _SMLRECOVERY_H_ 1

/**
 * @file smlRecovery.h
 * @brief Storage Manager Recovery Routines
 */

/**
 * @defgroup smlRecovery Recovery
 * @ingroup smExternal
 * @{
 */

stlStatus smlRecover( stlInt8   aLogOption,
                      smlEnv  * aEnv );

stlStatus smlFlushLogForFlusher( stlBool    aFlushAll,
                                 stlInt64 * aFlushedLsn,
                                 stlInt64 * aFlushedSbsn,
                                 smlEnv   * aEnv );

stlStatus smlFlushAllLogs( smlEnv * aEnv );

stlStatus smlCheckpoint( smlEnv  * aEnv );
stlStatus smlCheckpoint4Shutdown( smlEnv  * aEnv );

stlStatus smlFlushPages4Shutdown( smlEnv * aEnv );

stlStatus smlSetLogFlusherState( stlInt16   aState,
                                 smlEnv   * aEnv );

stlStatus smlSetCheckpointerState( stlInt16   aState,
                                   smlEnv   * aEnv );

stlStatus smlWriteSupplementalDdlLog( smlTransId     aTransId,
                                      void         * aLogData,
                                      stlUInt16      aLogSize,
                                      stlUInt32      aStmtType,
                                      stlInt64       aPhysicalId,
                                      smlEnv       * aEnv );

stlStatus smlAddIrrecoverableSegment( stlInt64    aSegmentId,
                                      smlEnv    * aEnv );

stlStatus smlConvAbsLogfilePath( stlChar  * aFileName,
                                 smlEnv   * aEnv );

stlStatus smlExistLogfile( stlChar  * aFileName,
                            stlBool  * aExist,
                            smlEnv   * aEnv );

stlStatus smlIsUsedLogfile( stlChar   * aFileName,
                            stlBool   * aExist,
                            stlInt16  * aGroupId,
                            smlEnv    * aEnv );

stlStatus smlCheckLogGroupId( stlInt32   aLogGroupId,
                              stlBool  * aFound,
                              smlEnv   * aEnv );

stlInt64 smlGetMinLogFileSize( smlEnv   * aEnv );

stlStatus smlMediaRecoverTablespace( smlTbsId   aTbsId,
                                     smlEnv   * aEnv );

stlStatus smlMediaRecoverDatafile( smlDatafileRecoveryInfo * aDatafileRecoveryInfo,
                                   smlEnv                  * aEnv );

stlStatus smlRestoreTablespace( smlTbsId   aTbsId,
                                smlEnv   * aEnv );

inline stlInt16 smlGetCheckpointState();

stlStatus smlRestoreCtrlFile( stlChar * aBackupCtrlFile,
                              smlEnv  * aEnv );
stlInt64 smlGetSystemLsn();
stlInt64 smlGetSystemSbsn();

stlStatus smlSwitchLogGroup( smlEnv * aEnv );

stlStatus smlExistValidIncBackup( smlTbsId   aTbsId,
                                  stlBool  * aExist,
                                  smlEnv   * aEnv );

stlStatus smlSetRecoveryPhaseAtMount( stlUInt8   aRecoveryPhase,
                                      smlEnv    * aEnv );

stlStatus smlGetRecoveryPhaseAtMount();

stlStatus smlValidateAlterArchivelog( stlInt32   aMode,
                                      smlEnv   * aEnv );

stlStatus smlGetAbsBackupFileName( stlChar  * aFileName,
                                   stlChar  * aAbsFileName,
                                   stlBool  * aExist,
                                   smlEnv   * aEnv );

stlStatus smlValidateMediaRecovery( smlTbsId   aTbsId,
                                    stlUInt8   aRecoveryObject,
                                    smlEnv   * aEnv );

/** @} */

#endif /* _SMLRECOVERY_H_ */
