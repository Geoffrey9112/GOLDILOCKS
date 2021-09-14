/*******************************************************************************
 * smlGeneral.h
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


#ifndef _SMLGENERAL_H_
#define _SMLGENERAL_H_ 1

/**
 * @file smlGeneral.h
 * @brief Storage Manager General Routines
 */

/**
 * @defgroup smlGeneral General
 * @ingroup smExternal
 * @{
 */

stlStatus smlInitialize();
stlStatus smlInitializeEnv( smlEnv     * aEnv,
                            knlEnvType   aEnvType );
stlStatus smlFinalizeEnv( smlEnv * aEnv );
stlStatus smlGetServerState( smlServerState * aServerState,
                             smlEnv         * aEnv );
smlDataAccessMode smlGetDataAccessMode();
stlStatus smlValidateMountDatabase( smlEnv * aEnv );
stlStatus smlValidateOpenDatabase( smlDataAccessMode    aAccessMode,
                                   stlChar              aLogOption,
                                   smlEnv             * aEnv );
stlStatus smlEnterStorageManager( smlEnv * aEnv );
stlStatus smlLeaveStorageManager( stlBool   aValidateErrorStack,
                                  smlEnv  * aEnv );

void smlSetDataAccessMode( smlDataAccessMode aAccessMode );
stlStatus smlExecutePendOp( stlUInt32    aActionFlag,
                            smlEnv     * aEnv );
stlStatus smlHandoverSession( smlSessionEnv * aTargetSessEnv,
                              smlSessionEnv * aSessEnv );
stlBool smlHasActiveStatement( smlEnv * aEnv );
void smlRegisterCheckpointEnv( smlEnv * aEnv );
void smlRegisterLogFlusherEnv( smlEnv * aEnv );
void smlRegisterPageFlusherEnv( smlEnv * aEnv );
void smlRegisterAgerEnv( smlEnv * aEnv );
void smlRegisterCleanupEnv( smlEnv * aEnv );
void smlRegisterArchivelogEnv( smlEnv * aEnv );
void smlRegisterIoSlaveEnv( stlInt64   aThreadIdx,
                            smlEnv   * aEnv );

stlStatus smlInitializeSessionEnv( smlSessionEnv     * aSessionEnv,
                                   knlConnectionType   aConnectionType,
                                   knlSessionType      aSessionType,
                                   knlSessEnvType      aSessEnvType,
                                   smlEnv            * aEnv );

stlStatus smlFinalizeSessionEnv( smlSessionEnv * aSessionEnv,
                                 smlEnv        * aEnv );

stlStatus smlCleanupSessionEnv( smlSessionEnv * aDeadSessEnv,
                                smlEnv        * aEnv );

stlUInt32 smlGetCheckpointEnvId();
stlUInt32 smlGetLogFlusherEnvId();
stlUInt32 smlGetPageFlusherEnvId();
stlUInt32 smlGetAgerEnvId();
stlUInt32 smlGetCleanupEnvId();
stlUInt32 smlGetArchivelogEnvId();
stlUInt32 smlGetIoSlaveEnvId( stlInt64 aThreadIdx );
stlInt16 smlGetAgerState();
stlStatus smlDeactivateAger( smlEnv * aEnv );
stlStatus smlActivateAger( smlEnv * aEnv );
stlStatus smlLoopbackAger( stlBool   aBuildAgableScn,
                           smlEnv  * aEnv );
stlStatus smlActivateLogFlushing( smlEnv * aEnv );
stlStatus smlDeactivateLogFlushing( smlEnv * aEnv );

void smlSetScnToDisableAging( smlTransId      aTransId,
                              smlSessionEnv * aSessionEnv );
void smlUnsetScnToEnableAging( smlSessionEnv * aSessionEnv );
void smlAddSessStat2SystemStat( smlSessionEnv * aSessEnv );

void smlIncRequestGroupWorker( stlInt32 aRequestGroupId );
void smlDecRequestGroupWorker( stlInt32 aRequestGroupId );

extern knlStartupFunc     gStartupSM[KNL_STARTUP_PHASE_MAX];
extern knlWarmupFunc      gWarmupSM;
extern knlShutdownFunc    gShutdownSM[KNL_SHUTDOWN_PHASE_MAX];
extern knlCooldownFunc    gCooldownSM;
extern knlCleanupHeapFunc gCleanupHeapSM;

/** @} */

#endif /* _SMLGENERAL_H_ */
