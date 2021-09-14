/*******************************************************************************
 * smlDatabase.h
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


#ifndef _SMLDATABASE_H_
#define _SMLDATABASE_H_ 1

/**
 * @file smlDatabase.h
 * @brief Storage Manager Database Routines
 */

/**
 * @defgroup smlDatabase Database
 * @ingroup smExternal
 * @{
 */

stlStatus smlCreateDatabase( knlStartupPhase   aPhase,
                             smlEnv          * aEnv );
stlStatus smlDropDatabase( smlEnv * aEnv );

stlStatus smlCompleteCreateDatabase( smlEnv * aEnv );

stlInt64 smlGetFirstDicTableId();

inline stlBool smlIsArchivelogMode();

stlStatus smlBackupCtrlFile( stlChar * aTarget,
                             smlEnv  * aEnv );

stlStatus smlBackup( smlBackupMessage * aBackupMsg,
                     smlEnv           * aEnv );

stlStatus smlDeleteBackupList( stlChar    aTarget,
                               stlBool    aDeleteFile,
                               smlEnv   * aEnv );

/** @} */

#endif /* _SMLDATABASE_H_ */
