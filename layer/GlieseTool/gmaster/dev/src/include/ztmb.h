/*******************************************************************************
 * ztmb.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztmb.h 5283 2012-08-10 03:01:37Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _ZTMB_H_
#define _ZTMB_H_ 1

/**
 * @file ztmb.h
 * @brief Gliese Master Boot Routines
 */

/**
 * @defgroup ztmBoot Gliese Master Boot Routines
 * @{
 */

/**
 * GLOBAL PHASE TRANSITION FUNCIONS
 */
stlStatus ztmbStartupPhase( knlStartupPhase   aStartPhase,
                            knlStartupPhase   aEndPhase,
                            knlStartupInfo  * aStartupInfo,
                            ztmEnv          * aEnv );
stlStatus ztmbShutdownPhase( knlShutdownPhase   aStartPhase,
                             knlShutdownPhase   aEndPhase,
                             knlShutdownInfo  * aShutdownInfo,
                             ztmEnv           * aEnv );

/**
 * GLIESE MASTER TRANSITION FUNCIONS
 */
stlStatus ztmbStartupNoMount( knlStartupInfo * aStartupInfo,
                              void           * aEnv );
stlStatus ztmbStartupMount( knlStartupInfo * aStartupInfo,
                            void           * aEnv );
stlStatus ztmbStartupOpen( knlStartupInfo * aStartupInfo,
                           void           * aEnv );
stlStatus ztmbWarmup( void * aEnv );
stlStatus ztmbShutdownInit( knlShutdownInfo * aShutdownInfo,
                            void            * aEnv );
stlStatus ztmbShutdownDismount( knlShutdownInfo * aShutdownInfo,
                                void            * aEnv );
stlStatus ztmbShutdownClose( knlShutdownInfo * aShutdownInfo,
                             void            * aEnv );

/**
 * MISC FUNCIONS
 */
stlStatus ztmbStartup( stlInt64      * aMsgKey,
                       stlErrorStack * aErrorStack );

stlStatus ztmbCheckStarted( ztmThread * aThread,
                            void      * aEnv );
stlStatus ztmbCreateMountSystemThreads( void * aEnv );
stlStatus ztmbStopMountSystemThreads();
stlStatus ztmbJoinMountSystemThreads();
stlStatus ztmbKillSystemThreads( ztmEnv * aEnv );
stlStatus ztmbKillProcessMonitorThreads( ztmEnv * aEnv );
stlStatus ztmbCancelIoSlaveThreads();

stlStatus ztmbShutdownBootdown( void * aEnv );
stlStatus ztmbStartupEventHandler( void      * aData,
                                   stlUInt32   aDataSize,
                                   stlBool   * aDone,
                                   void      * aEnv );
stlStatus ztmbShutdownEventHandler( void      * aData,
                                    stlUInt32   aDataSize,
                                    stlBool   * aDone,
                                    void      * aEnv );
stlStatus ztmbCleanupEventHandler( void      * aData,
                                   stlUInt32   aDataSize,
                                   stlBool   * aDone,
                                   void      * aEnv );
stlStatus ztmbRollbackTrans4Shutdown( void * aEnv );

/** @} */


/** @} */

#endif /* _ZTMB_H_ */
