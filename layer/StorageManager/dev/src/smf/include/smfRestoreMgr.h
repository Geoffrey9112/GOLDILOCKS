/*******************************************************************************
 * smfRestoreMgr.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smfRestoreMgr.h 9304 2013-08-07 07:39:53Z kyungoh $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _SMFRESTOREMGR_H_
#define _SMFRESTOREMGR_H_ 1

/**
 * @file smfRestoreMgr.h
 * @brief Storage Manager Layer Tablespace Manager Component Internal Routines
 */

/**
 * @defgroup smfRestoreMgr Tablespace Manager
 * @ingroup smfInternal
 * @{
 */

stlStatus smfApplyBackup( smlTbsId            aTbsId,
                          stlChar           * aBuffer,
                          stlInt32            aBufferSize,
                          smfBackupIterator * aBackupList,
                          smrLsn            * aBackupChkptLsn,
                          smrLid            * aBackupChkptLid,
                          stlBool           * aIsSuccess,
                          smlEnv            * aEnv );

stlStatus smfInitDatafileRestoredLsn( smlTbsId   aTbsId,
                                      smlEnv   * aEnv );

stlStatus smfUpdateDatafileRestoredLsn( smlTbsId   aTbsId,
                                        smrLsn     aBackupChkptLsn,
                                        smrLid     aBackupChkptLid,
                                        smlEnv   * aEnv );

stlStatus smfGetMinRestoredLsn( smlTbsId    aTbsId,
                                smrLsn    * aMinRestoredLsn,
                                smlEnv    * aEnv );

stlStatus smfIsApplicableBackup( smlTbsId            aTbsId,
                                 smfBackupRecord   * aBackupRecord,
                                 stlBool           * aIsApplicable,
                                 smlEnv            * aEnv );

stlStatus smfIsApplicableTbs( smlTbsId           aTbsId,
                              smfBackupRecord  * aBackupRecord,
                              stlBool          * aIsApplicable,
                              smlEnv           * aEnv );


stlStatus smfMakeApplyBackupPlan( stlFile             * aFile,
                                  smlTbsId              aTbsId,
                                  stlUInt8              aUntilType,
                                  stlInt64              aUntilValue,
                                  smfApplyBackupPlan  * aApplyBackupPlan,
                                  smlEnv              * aEnv );

stlStatus smfApplyTbs( stlFile              * aFile,
                       smfIncBackupFileTail * sFileTail,
                       smfBackupIterator    * aBackupIterator,
                       stlChar              * aBuffer,
                       stlInt32               aBufferSize,
                       smlEnv               * aEnv );

stlStatus smfRestoreDatafileByFullBackup( smrRecoveryDatafileInfo  * aDatafileInfo,
                                          smrLsn                   * aRestoredLsn,
                                          smrLid                   * aRestoredLid,
                                          smlEnv                   * aEnv );

stlStatus smfRestoreDatafileByIncBackup( smrMediaRecoveryInfo * aRecoveryInfo,
                                         smrLsn               * aRestoredLsn,
                                         smrLid               * aRestoredLid,
                                         smlEnv               * aEnv );

stlStatus smfApplyBackup4MediaRecovery( smrMediaRecoveryInfo * aRecoveryInfo,
                                        stlChar              * aBuffer,
                                        stlInt32               aBufferSize,
                                        smfBackupIterator    * aBackupIterator,
                                        smrLsn               * aBackupChkptLsn,
                                        smrLid               * aBackupChkptLid,
                                        stlBool              * aIsSuccess,
                                        smlEnv               * aEnv );

/** @} */
    
#endif /* _SMFRESTOREMGR_H_ */
