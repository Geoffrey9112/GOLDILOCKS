/*******************************************************************************
 * knlGeneral.h
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


#ifndef _KNLGENERAL_H_
#define _KNLGENERAL_H_ 1

/**
 * @file knlGeneral.h
 * @brief Kernel Layer General Routines
 */

/**
 * @defgroup knlGeneral General
 * @ingroup knExternal
 * @{
 */

stlStatus knlInitialize();
knlStartupPhase  knlGetCurrStartupPhase();
knlStartupPhase  knlGetStartupPhase( void * aEntryPoint );
knlShutdownPhase knlGetShutdownPhasePair( knlStartupPhase aStartupPhase );
void knlSetStartupPhase( knlStartupPhase aStartupPhase );
stlTime knlGetStartupTime();
stlStatus knlAddSar( stlUInt32 aSarMask, knlEnv * aEnv );
stlStatus knlUnsetSar( stlUInt32 aSarMask, knlEnv * aEnv );
stlStatus knlSetSar( stlUInt32 aSarMask, knlEnv * aEnv );
stlStatus knlValidateSar( stlUInt32       aSarMask,
                          stlLayerClass   aTopLayer,
                          knlEnv        * aEnv );
stlBool   knlCheckSar( stlUInt32 aSarMask );
stlStatus knlValidateProcessSar( stlUInt32       aSarMask,
                                 stlErrorStack * aErrorStack );
stlStatus knlValidateBinaryVersion( knlEnv * aEnv );
stlStatus knlExecutePendingSignal( stlInt32 aPendingSignalNo );
void * knlDumpBinaryForAssert( stlChar      * aBinary,
                               stlInt32       aSize,
                               knlRegionMem * aRegionMem,
                               knlEnv       * aEnv );
void knlSystemFatal( stlChar * aCauseString,
                     knlEnv  * aEnv );
stlBool knlResolveSystemFatal( stlBool   aDoResolve,
                               knlEnv  * aEnv );

void knlLockSignal();
void knlUnlockSignal();
stlStatus knlLockAndBlockSignal( stlErrorStack * aErrorStack );
stlStatus knlUnlockAndUnblockSignal( stlErrorStack * aErrorStack );
void knlDisableDiskIo( void );
void knlEnableDiskIo( void );
stlBool knlAllowDiskIo( knlEnv * aEnv );

extern knlStartupFunc     gStartupKN[KNL_STARTUP_PHASE_MAX];
extern knlWarmupFunc      gWarmupKN;
extern knlShutdownFunc    gShutdownKN[KNL_SHUTDOWN_PHASE_MAX];
extern knlCooldownFunc    gCooldownKN;

/** @} */
    
#endif /* _KNLGENERAL_H_ */
