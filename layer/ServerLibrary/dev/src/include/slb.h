/*******************************************************************************
 * slb.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: slb.h 13675 2014-10-06 14:17:27Z lym999 $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _SLB_H_
#define _SLB_H_ 1

/**
 * @file slb.h
 * @brief Server Library Boot Routines
 */

/**
 * @defgroup SlBoot Server Library Boot Routines
 * @{
 */

/**
 * gserver Startup/Shutdown functions
 */

stlStatus slbWarmUp( void * aEnv );
stlStatus slbCoolDown( void * aEnv );

stlStatus slbStartupNoMount( knlStartupInfo * aStartupInfo,
                             void           * aEnv );
stlStatus slbStartupMount( knlStartupInfo * aStartupInfo,
                           void           * aEnv );
stlStatus slbStartupOpen( knlStartupInfo * aStartupInfo,
                          void           * aEnv );
stlStatus slbShutdownInit( knlShutdownInfo  * aShutdownInfo,
                           void             * aEnv );
stlStatus slbShutdownDismount( knlShutdownInfo  * aShutdownInfo,
                               void             * aEnv );
stlStatus slbShutdownClose( knlShutdownInfo  * aShutdownInfo,
                            void             * aEnv );
stlStatus slbRegisterFxTables( void * aEnv );

void slbFatalHandler( const stlChar * aCauseString,
                      void          * aSigInfo,
                      void          * aContext );

stlStatus slbInitializeSharedMemory( sllSharedMemory   * aSharedMemory,
                                     void              * aEnv );
stlStatus slbFinalizeSharedMemory( sllSharedMemory   * aSharedMemory,
                                   void              * aEnv );

void slbStatusString( sllStatus       aStatus,
                      stlChar       * aString );
/**
 * Misc functions
 */

/** @} */


/** @} */

#endif /* _SLB_H_ */
