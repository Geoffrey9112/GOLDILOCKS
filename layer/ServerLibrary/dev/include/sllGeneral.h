/*******************************************************************************
 * sllGeneral.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: sllGeneral.h 7685 2013-03-20 08:05:24Z mycomman $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _SLLGENERAL_H_
#define _SLLGENERAL_H_ 1

/**
 * @file sllGeneral.h
 * @brief Server Library General Routines
 */


typedef stlStatus (*sllCommandFunc)( sllCommandArgs * aHandle,
                                     sllEnv         * aEnv );

extern sllCommandFunc     gSllCommandFunc[CML_COMMAND_MAX];

extern knlStartupFunc     gStartupSL[KNL_STARTUP_PHASE_MAX];
extern knlShutdownFunc    gShutdownSL[KNL_SHUTDOWN_PHASE_MAX];

extern knlWarmupFunc      gWarmupSL;
extern knlCooldownFunc    gCooldownSL;
extern knlCleanupHeapFunc gCleanupHeapSL;

stlStatus sllInitialize();
stlStatus sllTerminate();

stlStatus sllCheckTimeout( sllHandle  * aHandle,
                           sllEnv     * aEnv );

stlStatus sllKillSession( sllHandle   * aHandle,
                          sllEnv      * aEnv );

stlStatus sllGetSharedMemory( sllSharedMemory   ** aSharedMemory,
                              stlErrorStack      * aErrorStack );

stlStatus sllInitializeHandle( sllHandle        * aSllHandle,
                               stlContext       * aContext,
                               sclIpc           * aIpc,
                               stlSockAddr      * aClientAddr,
                               void             * aUserContext,
                               stlInt16           aMajorVersion,
                               stlInt16           aMinorVersion,
                               stlInt16           aPatchVersion,
                               sllEnv           * aEnv );

stlStatus sllFinalizeHandle( sllHandle     * aSllHandle,
                             sllEnv        * aEnv  );

stlStatus sllInitializeEnv( sllEnv     * aEnv,
                            knlEnvType   aEnvType );

stlStatus sllFinalizeEnv( sllEnv * aEnv );

stlStatus sllInitializeSessionEnv( sllSessionEnv     * aSessEnv,
                                   knlConnectionType   aConnectionType,
                                   knlSessionType      aSessType,
                                   knlSessEnvType      aSessEnvType,
                                   sllEnv            * aEnv );

stlStatus sllFinalizeSessionEnv( sllSessionEnv * aSessEnv,
                                 sllEnv        * aEnv );

stlStatus sllInitializeProtocolSentence( sllHandle               * aSllHandle,
                                         sllProtocolSentence     * aProtocolSentence,
                                         sclMemoryManager        * aMemoryMgr,
                                         sllEnv                  * aEnv );

stlStatus sllFinalizeProtocolSentence( sllProtocolSentence      * aProtocolSentence,
                                       sllEnv                   * aEnv );

stlStatus sllCheckContext( stlContext        * aContext,
                           stlBool           * aIsPollErr,
                           stlErrorStack     * aErrorStack );


stlStatus sllAddEdgeIntoWaitGraph( sllHandle      * aHandle,
                                   stlInt32         aRequestGroupId,
                                   stlErrorStack  * aErrorStack );

stlStatus sllRemoveEdgeFromWaitGraph( sllHandle      * aHandle,
                                      stlInt32         aRequestGroupId,
                                      stlErrorStack  * aErrorStack );

stlStatus sllCmdStartUp( sllHandle      * aHandle,
                         sllEnv         * aEnv );


#endif /* _SLLGENERAL_H_ */
