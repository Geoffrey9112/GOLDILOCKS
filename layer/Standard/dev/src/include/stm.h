/*******************************************************************************
 * stm.h
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

#ifndef _STM_H_
#define _STM_H_ 1

STL_BEGIN_DECLS

#include <stc.h>
#include <stlDef.h>

#define STM_MSG_LEN          (1024)
#define STM_PACKET_LEN       (1024 * 512)  /* 512 KBytes */
#define STM_SND_BUFF_SIZE    (1024 * 32)   /* 32 KBytes */
#define STM_RCV_BUFF_SIZE    (1024 * 32)   /* 32 KBytes */

stlStatus stmInitializeContext( stlContext     * aContext,
                                stlInt16         aFamily,
                                stlInt32         aType,
                                stlInt32         aProtocol,
                                stlBool          aIsListen,
                                stlChar        * aAddr,
                                stlInt32         aPort,
                                stlErrorStack  * aErrorStack );
stlStatus stmFinalizeContext( stlContext     * aContext,
                              stlErrorStack  * aErrorStack );
stlStatus stmPollContext( stlContext     * aContext,
                          stlContext     * aNewContext,
                          stlInterval      aTimeout,
                          stlErrorStack  * aErrorStack );
stlStatus stmConnectContext( stlContext    * aContext,
                             stlInterval     aTimeout,
                             stlErrorStack * aErrorStack );

stlStatus stmWritePacket( stlContext     * aContext,
                          stlChar        * aPacketBuffer,
                          stlSize          aPacketLength,
                          stlErrorStack  * aErrorStack );
stlStatus stmGetPacketLength( stlContext     * aContext,
                              stlInt8        * aEndian,
                              stlSize        * aPacketLength,
                              stlErrorStack  * aErrorStack );
stlStatus stmReadPacket( stlContext     * aContext,
                         stlChar        * aPacketBuffer,
                         stlSize          aPacketLength,
                         stlErrorStack  * aErrorStack );
stlStatus stmRecvContext( stlContext     * aContext,
                          stlContext     * aNewContext,
                          stlChar        * aBuf,
                          stlSize        * aLen,
                          stlErrorStack  * aErrorStack );
stlStatus stmSendContext( stlContext     * aContext,
                          stlSockAddr    * aAddr,
                          stlContext     * aPassingContext,
                          const stlChar  * aBuf,
                          stlSize          aLen,
                          stlErrorStack  * aErrorStack );
stlStatus stmCreateContextPair( stlInt32          aDomain,
                                stlInt32          aType,
                                stlInt32          aProtocol,
                                stlContext        aContexts[2],
                                stlErrorStack    *aErrorStack );
STL_END_DECLS

#endif /* _STM_H_ */
