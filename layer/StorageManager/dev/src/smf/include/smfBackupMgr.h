/*******************************************************************************
 * smfBackupMgr.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smfBackupMgr.h 9455 2013-08-27 08:23:04Z kyungoh $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _SMFBACKUPMGR_H_
#define _SMFBACKUPMGR_H_ 1

/**
 * @file smfBackupMgr.h
 * @brief Storage Manager Layer Backup Manager Component Internal Routines
 */

/**
 * @defgroup smfBackupMgr Tablespace Manager
 * @ingroup smfInternal
 * @{
 */

stlStatus smfAcquireBackupTbs( smlTbsId     aTbsId,
                               smlEnv     * aEnv );
stlStatus smfReleaseBackupTbs( smlTbsId     aTbsId,
                               smlEnv     * aEnv );
stlStatus smfBackupTablespaceBegin( stlChar    aBackupType,
                                    smlTbsId   aTbsId,
                                    stlBool  * aDropped,
                                    stlBool  * aOfflined,
                                    smlEnv   * aEnv );

stlStatus smfBackupTablespaceEnd( stlChar    aBackupType,
                                  smlTbsId   aTbsId,
                                  stlBool  * aSuccess,
                                  smlEnv   * aEnv );

stlStatus smfBackupDatabaseBegin( stlChar    aBackupType,
                                  smlEnv   * aEnv );

stlStatus smfBackupDatabaseEnd( stlChar     aBackupType,
                                stlInt16    aBackupLevel,
                                smlEnv    * aEnv );

stlStatus smfFullBackup( stlChar    aCommand,
                         smlTbsId   aTbsId,
                         smlEnv   * aEnv );

stlStatus smfPreventBackupForShutdown( smlEnv  * aEnv );

stlStatus smfRollbackPreventBackup( smlEnv  * aEnv );

stlStatus smfIncrementalBackup( smlTbsId   aTbsId,
                                stlChar    aBackupOption,
                                stlInt16   aBackupLevel,
                                smlEnv   * aEnv );

stlStatus smfBackupDatabase( stlChar    aBackupType,
                             stlChar    aCommand,
                             stlInt16   aBackupLevel,
                             smlEnv   * aEnv );

stlStatus smfBackupTablespace( stlChar     aBackupType,
                               stlChar     aCommand,
                               smlTbsId    aTbsId,
                               smlEnv    * aEnv );

void smfSetPrevBackupLsn( smlTbsId   aTbsId,
                          stlChar    aBackupOption,
                          stlInt16   aBackupLevel,
                          smrLsn     aBackupLsn );

smrLsn smfGetPrevBackupLsn( smlTbsId   aTbsId,
                            stlChar    aBackupOption,
                            stlInt16   aBackupLevel );

stlInt64 smfGetBackupSequence( smlTbsId   aTbsId );

stlStatus smfIncrementalBackupBegin( smlTbsId    aTbsId,
                                     stlChar     aBackupOption,
                                     stlInt16    aBackupLevel,
                                     smrLsn    * aPrevBackupLsn,
                                     stlInt64  * aBackupSeq,
                                     stlChar   * aFileName,
                                     stlChar   * aAbsFileName,
                                     stlTime   * aBeginTime,
                                     smlEnv    * aEnv );

stlStatus smfIncrementalBackupEnd( smlTbsId    aTbsId,
                                   stlInt16    aBackupLevel,
                                   smlEnv    * aEnv );

stlStatus smfGetBackupObjectName( smlTbsId                 aTbsId,
                                  smfIncBackupObjectType   aBackupObjectType,
                                  stlChar                * aString,
                                  smlEnv                 * aEnv );

/** @} */
    
#endif /* _SMFBACKUPMGR_H_ */
