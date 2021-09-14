/*******************************************************************************
 * smrRecoveryMgr.h
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


#ifndef _SMRRECOVERYMGR_H_
#define _SMRRECOVERYMGR_H_ 1

/**
 * @file smrRecoveryMgr.h
 * @brief Storage Manager Layer Recovery Manager Component Internal Routines
 */

/**
 * @defgroup smrRecoveryMgr Recovery Manager
 * @ingroup smrInternal
 * @{
 */

stlStatus smrAnalysis( smrLsn * aOldestLsn,
                       smrLid * aRedoLid,
                       smlScn * aSystemScn,
                       smlEnv * aEnv );
stlStatus smrRestartRedo( smrLsn   aOldestLsn,
                          smrLid   aRedoLid,
                          smlScn   aSystemScn,
                          smlEnv * aEnv );
stlStatus smrRestartUndo( smlEnv * aEnv );
stlStatus smrPrepareRedo( smrLid         aRedoLid,
                          smrLsn         aOldestLsn,
                          smlEnv       * aEnv );
stlStatus smrResetLog( smlEnv * aEnv );
stlStatus smrClearPartialLog( smlEnv * aEnv );

/** @} */

#endif /* _SMRRECOVERYMGR_H_ */
