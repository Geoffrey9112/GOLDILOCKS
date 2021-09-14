/*******************************************************************************
 * elgStartup.h
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


#ifndef _ELG_STARTUP_H_
#define _ELG_STARTUP_H_ 1

/**
 * @file elgStartup.h
 * @brief Execution Library Layer Startup Internal Routines
 */

/**
 * @defgroup elgStartup Startup
 * @ingroup elg
 * @internal
 * @{
 */

stlStatus elgStartupNoMount( knlStartupInfo * aStartupInfo,
                             void           * aEnv );
stlStatus elgStartupMount( knlStartupInfo * aStartupInfo,
                           void           * aEnv );
stlStatus elgStartupPreOpen( knlStartupInfo * aStartupInfo,
                             void           * aEnv );
stlStatus elgWarmup( void * aEnv );
stlStatus elgCooldown( void * aEnv );
stlStatus elgShutdownInit( knlShutdownInfo * aShutdownInfo,
                           void            * aEnv );
stlStatus elgShutdownDismount( knlShutdownInfo * aShutdownInfo,
                               void            * aEnv );
stlStatus elgShutdownPostClose( knlShutdownInfo * aShutdownInfo,
                                void            * aEnv );

void elgFatalHandler( const stlChar * aCauseString,
                      void          * aSigInfo,
                      void          * aContext );

/** @} */
    
#endif /* _ELG_STARTUP_H_ */
