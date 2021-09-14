/*******************************************************************************
 * ztlc.h
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


#ifndef _ZTLC_H_
#define _ZTLC_H_ 1

/**
 * @file ztlc.h
 * @brief Gliese Listener Communication Routines
 */

/**
 * @defgroup ztlc Gliese Listener Communication Routines
 * @{
 */

stlStatus ztlcRun( stlErrorStack         * aErrorStack );

stlStatus ztlcExecDedicator( ztlcContext     * aContext,
                             stlErrorStack   * aErrorStack );

stlStatus ztlcCloseContext( stlPollSet     * aPollSet,
                            ztlcContext    * aContext,
                            stlErrorStack  * aErrorStack );

stlStatus ztlcForwardFd( ztlcListener      * aListener,
                         ztlcContext       * aContext,
                         stlErrorStack     * aErrorStack );

stlStatus ztlcRequestQuit( stlErrorStack   * aErrorStack );

stlStatus ztlcGetStatus( stlBool           aPrintStatus,
                         stlBool         * aRunning,
                         stlErrorStack   * aErrorStack );

stlStatus ztlcSendStatusInfo( ztlcContext   * aContext,
                              stlSockAddr   * aAddr,
                              stlErrorStack * aErrorStack );

stlStatus ztlcGetUnixDomainPath( stlChar       * aUdsPath,
                                 stlSize         aSize,
                                 stlChar       * aName,
                                 stlInt64        aKey,
                                 stlErrorStack * aErrorStack );

stlStatus ztlcGetContext( ztlcListener     * aListener,
                          stlBool            aUsed,
                          cmlSessionType     aSessionType,
                          stlInt64           aSequence,
                          stlInt32           aReceivedPreHandshake,
                          stlInt32           aIsFdSending,
                          ztlcContext     ** aContext,
                          stlErrorStack    * aErrorStack );

stlStatus ztlcInitializeUds( ztlcContext     * aUds,
                             stlChar         * aPath,
                             stlErrorStack   * aErrorStack);

stlStatus ztlcFinalizeUds( ztlcContext     * aUds,
                           stlErrorStack   * aErrorStack);

stlStatus ztlcGetMaxContextCount( stlInt64        * aMaxContextCount,
                                  stlErrorStack   * aErrorStack );


/** @} */

/** @} */

#endif /* _ZTLC_H_ */
