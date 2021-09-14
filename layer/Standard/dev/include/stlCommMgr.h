/****************************************
 * stlCommMgr.h
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
 ***************************************/


#ifndef _STLCOMMMGR_H_
#define _STLCOMMMGR_H_ 1

/**
 * @file stlCommMgr.h
 * @brief Standard Layer Communication Manager
 */


STL_BEGIN_DECLS


/**
 * @defgroup stlCommMgr Communications
 * @ingroup STL 
 * @{
 */

stlStatus stlInitializeContext( stlContext     * aContext,
                                stlInt16         aFamily,
                                stlInt32         aType,
                                stlInt32         aProtocol,
                                stlBool          aIsListen,
                                stlChar        * aAddr,
                                stlInt32         aPort,
                                stlErrorStack  * aErrorStack );
stlStatus stlFinalizeContext( stlContext     * aContext,
                              stlErrorStack  * aErrorStack );
stlStatus stlPollContext( stlContext     * aContext,
                          stlContext     * aNewContext,
                          stlInterval      aTimeout,
                          stlErrorStack  * aErrorStack );
stlStatus stlConnectContext( stlContext    * aContext,
                             stlInterval     aTimeout,
                             stlErrorStack * aErrorStack );
stlStatus stlWritePacket( stlContext     * aContext,
                          stlChar        * aPacketBuffer,
                          stlSize          aPacketLength,
                          stlErrorStack  * aErrorStack );
stlStatus stlGetPacketLength( stlContext     * aContext,
                              stlInt8        * aEndian,
                              stlSize        * aPacketLength,
                              stlErrorStack  * aErrorStack );
stlStatus stlReadPacket( stlContext     * aContext,
                         stlChar        * aPacketBuffer,
                         stlSize          aPacketLength,
                         stlErrorStack  * aErrorStack );
stlStatus stlRecvContext( stlContext     * aContext,
                          stlContext     * aNewContext,
                          stlChar        * aBuf,
                          stlSize        * aLen,
                          stlErrorStack  * aErrorStack );

stlStatus stlSendContext( stlContext     * aContext,
                          stlSockAddr    * aAddr,
                          stlContext     * aPassingContext,
                          const stlChar  * aBuf,
                          stlSize          aLen,
                          stlErrorStack  * aErrorStack );
stlStatus stlCreateContextPair( stlInt32          aDomain,
                                stlInt32          aType,
                                stlInt32          aProtocol,
                                stlContext        aContexts[2],
                                stlErrorStack    *aErrorStack );


/* @} */
    
STL_END_DECLS

#endif /* _STLCOMMMGR_H_ */
