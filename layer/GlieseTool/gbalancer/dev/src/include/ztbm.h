/*******************************************************************************
 * ztbm.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztbm.h 12157 2014-04-23 03:50:31Z lym999 $
 *
 * NOTES
 *
 *
 ******************************************************************************/


#ifndef _ZTBM_H_
#define _ZTBM_H_ 1

/**
 * @file ztbm.h
 * @brief Gliese Balancer routines
 */

/**
 * @defgroup ztbMain Balancer main routines
 * @{
 */

extern stlGetOptOption gZtbmOptOption[];

stlStatus ztbmDaemonize( stlBool         aDaemonize,
                         stlErrorStack * aErrorStack );

stlStatus ztbmStartup( stlBool         aDaemonize,
                       stlErrorStack * aErrorStack );

stlStatus ztbmShutdown( stlErrorStack * aErrorStack );

stlStatus ztbmWarmup( stlErrorStack * aErrorStack );

stlStatus ztbmCooldown( stlErrorStack * aErrorStack );

stlStatus ztbmRun( stlInt64      * aMsgKey,
                   sllEnv        * aEnv,
                   sllSessionEnv * aSessionEnv );

void ztbmFatalHandler( const stlChar * aCauseString,
                       void          * aSigInfo,
                       void          * aContext );



stlStatus ztbmGetDispatcher( ztbmDispatcher    * aDispatcherList,
                             stlInt32          * aDispatcherIndex,
                             sllSharedMemory   * aSharedMemory,
                             sllEnv            * aEnv );

stlStatus ztbmGetUnixDomainPath( stlChar       * aUdsPath,
                                 stlSize         aSize,
                                 stlChar       * aName,
                                 stlInt64        aKey,
                                 stlErrorStack * aErrorStack );

stlStatus ztbmGetUnixDomainPathByIndex( stlChar       * aUdsPath,
                                        stlSize         aSize,
                                        stlChar       * aName,
                                        stlInt64        aKey,
                                        stlInt32        aIndex,
                                        stlErrorStack * aErrorStack );

stlStatus ztbmProcessBalance( ztbmDispatcher   * aDispatchers,
                              ztbmContext      * aContextList,
                              stlInt64           aMaxContextCount,
                              stlInt64           aNextSequence,
                              stlContext       * aUdsContext,
                              sllSharedMemory  * aSharedMemory,
                              sllEnv           * aEnv );

stlStatus ztbmProcessAck( ztbmDispatcher   * aDispatchers,
                          ztbmContext      * aContextList,
                          stlContext       * aUdsContext,
                          sllEnv           * aEnv );

stlStatus ztbmForwardFdToDispatcher( ztbmDispatcher   * aDispatchers,
                                     ztbmContext      * aContext,
                                     stlContext       * aUdsContext,
                                     sllSharedMemory  * aSharedMemory,
                                     sllEnv           * aEnv );

stlStatus ztbmCleanupFd( ztbmContext      * aContextList,
                         stlInt64           aMaxContextCount,
                         sllEnv           * aEnv );

stlStatus ztbmRetryForwardFd( ztbmDispatcher   * aDispatchers,
                              stlContext       * aUdsContext,
                              ztbmContext      * aContextList,
                              stlInt64           aMaxContextCount,
                              sllSharedMemory  * aSharedMemory,
                              sllEnv           * aEnv );

stlStatus ztbmInitializeContextList( ztbmContext      * aContextList,
                                     stlInt64           aMaxContextCount,
                                     sllEnv           * aEnv );

stlStatus ztbmFinalizeContextList( ztbmContext      * aContextList,
                                   sllEnv           * aEnv );

stlStatus ztbmInitializeUds( stlContext      * aUds,
                             stlChar         * aName,
                             stlInt64          aKey,
                             sllEnv          * aEnv );

stlStatus ztbmFinalizeUds( stlContext      * aUds,
                           sllEnv          * aEnv );

stlStatus ztbmSendFdTo( stlContext      * aUdsContext,
                        stlSockAddr     * aTargetAddr,
                        ztbmContext     * aContext,
                        stlErrorStack   * aErrorStack );

stlStatus ztbmGetMaxContextCount( stlInt64        * aMaxContextCount,
                                  sllEnv          * aEnv );
/** @} */



#endif /* _ZTBM_H_ */
