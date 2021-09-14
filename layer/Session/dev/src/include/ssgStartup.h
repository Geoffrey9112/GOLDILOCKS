/*******************************************************************************
 * ssgStartup.h
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


#ifndef _SSG_STARTUP_H_
#define _SSG_STARTUP_H_ 1

/**
 * @file ssgStartup.h
 * @brief Session Layer Startup Internal Routines
 */

stlStatus ssgStartupNoMount( knlStartupInfo * aStartupInfo,
                             void           * aEnv );
stlStatus ssgStartupMount( knlStartupInfo * aStartupInfo,
                           void           * aEnv );
stlStatus ssgStartupOpen( knlStartupInfo * aStartupInfo,
                          void           * aEnv );
stlStatus ssgWarmup( void * aEnv );

stlStatus ssgCooldown( void * aEnv );
stlStatus ssgShutdownInit( knlShutdownInfo * aShutdownInfo,
                           void            * aEnv );
stlStatus ssgShutdownDismount( knlShutdownInfo * aShutdownInfo,
                               void            * aEnv );
stlStatus ssgShutdownClose( knlShutdownInfo * aShutdownInfo,
                            void            * aEnv );
void ssgFatalHandler( const stlChar * aCauseString,
                      void          * aSigInfo,
                      void          * aContext );

stlStatus ssgRegisterFxTables( void * aEnv );

#endif /* _SSG_STARTUP_H_ */
