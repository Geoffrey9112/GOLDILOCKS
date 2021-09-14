/*******************************************************************************
 * knlSession.h
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


#ifndef _KNLSESSION_H_
#define _KNLSESSION_H_ 1

/**
 * @file knlSession.h
 * @brief Kernel Layer Session Environment Routines
 */

/**
 * @defgroup knlSession Session Environment
 * @ingroup knExternal
 * @{
 */

stlStatus knlCreateSessionEnvPool( stlUInt32       aSessionEnvCount,
                                   stlUInt32       aSessionEnvSize,                            
                                   stlErrorStack * aErrorStack );

stlStatus knlAllocSessionEnv( void    ** aSessionEnv,
                              knlEnv   * aEnv );

stlStatus knlAllocSessionEnvUnsafe( void    ** aSessionEnv,
                                    knlEnv   * aEnv );

stlStatus knlAllocAdminSessionEnv( void    ** aSessionEnv,
                                   knlEnv   * aEnv );

stlStatus knlSetInitializedFlag( knlSessionEnv * aSessionEnv,
                                 stlLayerClass   aLayerClass,
                                 knlEnv        * aEnv );
stlBool knlGetInitializedFlag( knlSessionEnv * aSessionEnv,
                               stlLayerClass   aLayerClass );

stlStatus knlFreeSessionEnv( void    * aSessionEnv,
                             knlEnv  * aErrorStack );

stlStatus knlInitializeSessionEnv( knlSessionEnv     * aSessionEnv,
                                   knlConnectionType   aConnectionType,
                                   knlSessionType      aSessionType,
                                   knlSessEnvType      aSessEnvType,
                                   knlEnv            * aEnv );

stlStatus knlFinalizeSessionEnv( knlSessionEnv * aSessionEnv,
                                 knlEnv        * aEnv );

stlStatus knlCleanupSessionEnv( knlSessionEnv * aDeadSessionEnv,
                                knlEnv        * aEnv );

stlBool knlIsAdminSession( void * aSessionEnv );
void * knlGetFirstSessionEnv();
void * knlGetLastSessionEnv();
void * knlGetNextSessionEnv( void * aSessionEnv );

void * knlGetSessionEnv( stlUInt32 aSessionEnvId );

stlStatus knlGetSessionEnvId( void      * aSessionEnv,
                              stlUInt32 * aId,
                              knlEnv    * aEnv );

stlBool knlIsUsedSessionEnv( knlSessionEnv * aSessionEnv );
stlBool knlIsTerminatedSessionEnv( knlSessionEnv * aSessionEnv );
stlBool knlIsDeadSessionEnv( knlSessionEnv * aSessionEnv );

stlBool knlIsSharedSessionEnv( knlSessionEnv * aSessionEnv );

stlStatus knlCleanupKilledSession( stlUInt32    aSessionEnvId,
                                   stlUInt64    aSessionSerial,
                                   knlEnv     * aEnv );
stlBool knlIsEndSession( knlSessionEnv * aSessionEnv );
stlBool knlIsKilledSession( knlSessionEnv * aSessionEnv );
knlSessEndOption knlGetEndSessionOption( knlSessionEnv * aSessionEnv );
stlStatus knlCheckEndSession( void * aSessionEnv, knlEnv * aEnv );
stlStatus knlCheckEndStatement( knlEnv * aEnv );
stlStatus knlEndSession( knlSessionEnv    * aSessionEnv,
                         knlSessEndOption   aOption,
                         knlEnv           * aEnv );
stlStatus knlEndStatement( knlSessionEnv    * aSessionEnv,
                           knlStmtEndOption   aOption,
                           knlEnv           * aEnv );
stlStatus knlEndSessionByID( stlUInt32          aSessionEnvId,
                             stlUInt64          aSessionSerial,
                             knlSessEndOption   aOption,
                             knlEnv           * aEnv );
stlStatus knlKillSessionByID( stlUInt32    aSessionEnvId,
                              stlUInt64    aSessionSerial,
                              knlEnv     * aEnv );
stlStatus knlIsUserSession( stlUInt32   aSessionEnvId,
                            stlBool   * aIsUserSession,
                            knlEnv    * aEnv );
stlStatus knlWaitAllUserSession( knlEnv * aEnv );
stlStatus knlCloseAllUserSession( knlEnv * aEnv );

void knlInitSessionEvent( knlSessionEvent * aSessionEvent );

void knlEnableQueryTimeout( knlEnv  * aEnv,
                            stlInt32  aStmtQueryTimeout );
void knlDisableQueryTimeout( knlEnv  * aEnv );
void knlEnableSessionEvent( knlEnv  * aEnv );
void knlDisableSessionEvent( knlEnv  * aEnv );

stlStatus knlCheckQueryTimeout( knlEnv * aEnv );
stlStatus knlCheckParentQueryTimeout( knlEnv * aParentEnv,
                                      knlEnv * aEnv );

void knlEnableIdleTimeout( knlEnv  * aEnv );
void knlDisableIdleTimeout( knlEnv  * aEnv );

stlStatus knlCheckIdleTimeout( knlSessionEnv * aSessionEnv,
                               knlEnv        * aEnv );

knlConnectionType knlGetSessionConnectionType( knlSessionEnv * aSessEnv );
void knlSetSessionConnectionType( knlSessionEnv     * aSessEnv,
                                  knlConnectionType   aConnectionType );

stlLayerClass knlGetSessionTopLayer( knlSessionEnv * aEnv );
void knlSetSessionTopLayer( knlSessionEnv * aSessEnv,
                            stlLayerClass   aTopLayer );

void knlDisconnectSession( void * aSessionEnv );
void knlKillSession( void * aSessionEnv );
void knlDeadSession( knlSessionEnv * aSessionEnv );

knlTermination knlGetTerminationStatus( knlSessionEnv * aSessionEnv );

void knlSetTransScope( void          * aSessEnv,
                       knlTransScope   aTransScope );
knlTransScope knlGetTransScope( void * aSessEnv );

stlBool knlHasFileOpenedSession();
stlBool knlHasOpenedFile( knlSessionEnv * aSessionEnv );
void  knlIncOpenFileCount( knlSessionEnv * aSessionEnv );
void  knlDecOpenFileCount( knlSessionEnv * aSessionEnv );

/** @} */
    
#endif /* _KNLSESSION_H_ */
