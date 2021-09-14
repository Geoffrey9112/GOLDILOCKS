/*******************************************************************************
 * smfCtrlFileMgr.h
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


#ifndef _SMFCTRLFILEMGR_H_
#define _SMFCTRLFILEMGR_H_ 1

/**
 * @file smfCtrlFileMgr.h
 * @brief Storage Manager Layer Control File Manager Component Internal Routines
 */

/**
 * @defgroup smfCtrlFileMgr Control File Manager
 * @ingroup smfInternal
 * @{
 */

stlStatus smfSaveCtrlFileInternal( stlFile  * aFile,
                                   smlEnv   * aEnv );

void smfFreeTbsInfoArray( smlEnv * aEnv );
stlStatus smfAdjustDbSection( smlEnv * aEnv );
stlStatus smfAdjustIncBackupSection( smlEnv * aEnv );
stlStatus smfWriteSysCtrlSection( stlFile  * aFile,
                                  stlSize  * aWrittenBytes,
                                  smlEnv   * aEnv );
stlStatus smfWriteBackupCtrlSection( stlFile  * aFile,
                                     stlSize  * aWrittenBytes,
                                     smlEnv   * aEnv );
stlStatus smfWriteCtrlFileChecksum( stlFile   * aFile,
                                    smlEnv    * aEnv );

stlStatus smfValidateCtrlFileVersion( stlFile   * aFile,
                                      stlChar   * aFilePath,
                                      smrLsn      aLastChkptLsn,
                                      stlTime     aDbCreationTime,
                                      stlBool   * aIsValid,
                                      smlEnv    * aEnv );

stlStatus smfCopyCtrlFile( stlChar  * aSource,
                           stlChar  * aTarget,
                           stlInt32   aTargetFlag,
                           smlEnv   * aEnv );
stlStatus smfCreateBackupCtrlSection( stlFile  * aFile,
                                      stlSize  * aWrittenBytes,
                                      smlEnv   * aEnv );

stlStatus smfSaveCtrlFileInternal4Syncher( stlFile        * aFile,
                                           smfSysPersData * aSysPersData,
                                           void           * aSmrWarmupEntry,  
                                           smlEnv         * aEnv );

/** @} */
    
#endif /* _SMFCTRLFILEMGR_H_ */
