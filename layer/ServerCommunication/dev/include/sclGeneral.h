/*******************************************************************************
 * sclGeneral.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: sclGeneral.h 7685 2013-03-20 08:05:24Z mycomman $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _SCLGENERAL_H_
#define _SCLGENERAL_H_ 1

/**
 * @file sclGeneral.h
 * @brief Server Communication General Routines
 */

extern knlStartupFunc     gStartupSC[KNL_STARTUP_PHASE_MAX];
extern knlShutdownFunc    gShutdownSC[KNL_SHUTDOWN_PHASE_MAX];

extern knlWarmupFunc      gWarmupSC;
extern knlCooldownFunc    gCooldownSC;
extern knlCleanupHeapFunc gCleanupHeapSC;

stlStatus sclInitialize();

stlStatus sclInitializeEnv( sclEnv     * aEnv,
                            knlEnvType   aEnvType );

stlStatus sclFinalizeEnv( sclEnv * aEnv );

stlStatus sclInitializeSessionEnv( sclSessionEnv     * aSessEnv,
                                   knlConnectionType   aConnectionType,
                                   knlSessionType      aSessType,
                                   knlSessEnvType      aSessEnvType,
                                   sclEnv            * aEnv );

stlStatus sclFinalizeSessionEnv( sclSessionEnv * aSessEnv,
                                 sclEnv        * aEnv );


#endif /* _SCLGENERAL_H_ */
