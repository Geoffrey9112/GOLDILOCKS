/*******************************************************************************
 * smlLogSyncher.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smlLogSyncher.h  $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _SMLLOGSYNCHER_H_
#define _SMLLOGSYNCHER_H_ 1

/**
 * @file smlLogSyncher.h
 * @brief Storage Manager Syncher Routines
 */

/**
 * @defgroup smlLogSyncher Syncher
 * @ingroup smExternal
 * @{
 */

stlStatus smlReproduceRecoveryWarmupEntry( void      *  aSrcWarmupEntry,
                                           void      ** aDesWarmupEntry,
                                           void      *  aSmfWarmupEntry,
                                           smlEnv    *  aEnv );

stlStatus smlFlushAllLogs4Syncher( void      * aSmrWarmupEntry,
                                   stlBool   * aSwitchedLogfile,
                                   stlBool     aSilent,
                                   stlLogger * aLogger,
                                   smlEnv    * aEnv );

stlStatus smlSaveCtrlFile4Syncher( void      * aSmfWarmupEntry,
                                   void      * aSmrWarmupEntry,
                                   stlBool     aSielnt,
                                   stlLogger * aLogger,
                                   smlEnv    * aEnv );


/** @} */

#endif /* _SMLLOGSYNCHER_H_ */
