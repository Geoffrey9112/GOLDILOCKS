/*******************************************************************************
 * ztim.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztim.h 12157 2014-04-23 03:50:31Z lym999 $
 *
 * NOTES
 *
 *
 ******************************************************************************/


#ifndef _ZTIM_H_
#define _ZTIM_H_ 1

/**
 * @file ztim.h
 * @brief Gliese dispatcher routines
 */

/**
 * @defgroup ztiMain dispatcher main routines
 * @{
 */

stlStatus ztimDaemonize( stlBool         aDaemonize,
                         stlErrorStack * aErrorStack );

stlStatus ztimStartup( stlBool         aDaemonize,
                      stlErrorStack * aErrorStack );

stlStatus ztimShutdown( stlErrorStack * aErrorStack );

stlStatus ztimWarmup( stlErrorStack * aErrorStack );

stlStatus ztimCooldown( stlErrorStack * aErrorStack );

stlStatus ztimRun( stlInt64      * aMsgKey,
                   stlInt32        aDispatcherIndex,
                   sllEnv        * aEnv );

stlStatus ztimInitializeUds( ztiContext      * aUds,
                             stlInt64          aKey,
                             stlInt32          aDispatcherIndex,
                             sllEnv          * aEnv );

stlStatus ztimFinalizeUds( ztiContext      * aUds,
                           sllEnv          * aEnv );

stlStatus ztimRecvPacketAsync( ztiDispatcher    * aDispatcher,
                               ztiContext       * aContext,
                               stlBool          * aPeriodEnd,
                               sllEnv           * aEnv );

stlStatus ztimGetUnixDomainPath( stlChar       * aUdsPath,
                                 stlSize         aSize,
                                 stlChar       * aName,
                                 stlInt64        aKey,
                                 stlErrorStack * aErrorStack );


stlStatus ztimGetMaxContextCount( ztiDispatcher   * aDispatcher,
                                  stlInt64        * aMaxContextCount,
                                  sllEnv          * aEnv );

void ztimFatalHandler( const stlChar * aCauseString,
                       void          * aSigInfo,
                       void          * aContext );

/** @} */



#endif /* _ZTIM_H_ */
