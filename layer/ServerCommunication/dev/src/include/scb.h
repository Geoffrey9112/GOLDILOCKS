/*******************************************************************************
 * scb.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: scb.h 13675 2014-10-06 14:17:27Z lym999 $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _SCB_H_
#define _SCB_H_ 1

/**
 * @file scb.h
 * @brief Server Communication Boot Routines
 */

/**
 * @defgroup ScBoot Server Communication Boot Routines
 * @{
 */

/**
 * functions
 */

stlStatus scbWarmUp( void * aEnv );
stlStatus scbCoolDown( void * aEnv );

stlStatus scbStartupNoMount( knlStartupInfo * aStartupInfo,
                             void           * aEnv );
stlStatus scbStartupMount( knlStartupInfo * aStartupInfo,
                           void           * aEnv );
stlStatus scbStartupOpen( knlStartupInfo * aStartupInfo,
                          void           * aEnv );
stlStatus scbShutdownInit( knlShutdownInfo  * aShutdownInfo,
                           void             * aEnv );
stlStatus scbShutdownDismount( knlShutdownInfo  * aShutdownInfo,
                               void             * aEnv );
stlStatus scbShutdownClose( knlShutdownInfo  * aShutdownInfo,
                            void             * aEnv );

void scbFatalHandler( const stlChar * aCauseString,
                      void          * aSigInfo,
                      void          * aContext );

/**
 * Misc functions
 */

/** @} */


/** @} */

#endif /* _SLB_H_ */
