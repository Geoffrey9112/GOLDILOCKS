/****************************************
 * stlNetworkIo.h
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


#ifndef _STLNETWORKIO_H_
#define _STLNETWORKIO_H_ 1

/**
 * @file stlNetworkIo.h
 * @brief Standard Layer Network I/O
 */


STL_BEGIN_DECLS


/**
 * @defgroup stlNetworkIo Network I/O Routines
 * @ingroup STL 
 * @{
 */

/** @see htonl */
#define stlToNetLong( aHostLong ) (htonl( aHostLong ))
/** @see htons */
#define stlToNetShort( aHostShort ) (htons( aHostShort ))
/** @see ntohl */
#define stlToHostLong( aNetLong ) (ntohl( aNetLong ))
/** @see ntohs */
#define stlToHostShort( aNetShort ) (ntohs( aNetShort ))


stlStatus stlCreateSocket( stlSocket     * aNewSocket, 
                           stlInt16        aFamily,
                           stlInt32        aType,
                           stlInt32        aProtocol,
                           stlErrorStack * aErrorStack );

stlStatus stlShutdownSocket( stlSocket        aSocket, 
                             stlShutdownHow   aHow,
                             stlErrorStack  * aErrorStack );

stlStatus stlCloseSocket( stlSocket       aSocket,
                          stlErrorStack * aErrorStack );

stlStatus stlBind( stlSocket       aSock,
                   stlSockAddr   * aSockAddr,
                   stlErrorStack * aErrorStack );

stlStatus stlListen( stlSocket       aSock,
                     stlUInt32       aBacklog,
                     stlErrorStack * aErrorStack );

stlStatus stlAccept( stlSocket       aSock,
                     stlSocket     * aNewSock,
                     stlErrorStack * aErrorStack );

stlStatus stlConnect( stlSocket       aSock,
                      stlSockAddr   * aSockAddr,
                      stlInterval     aTimeout,
                      stlErrorStack * aErrorStack );

stlStatus stlSend( stlSocket       aSock,
                   const stlChar * aBuf, 
                   stlSize       * aLen,
                   stlErrorStack * aErrorStack );

stlStatus stlRecv( stlSocket       aSock,
                   stlChar       * aBuf,
                   stlSize       * aLen,
                   stlErrorStack * aErrorStack);

stlStatus stlSendTo( stlSocket       aSock,
                     stlSockAddr   * aWhere,
                     stlInt32        aFlags,
                     const stlChar * aBuf,
                     stlSize       * aLen,
                     stlErrorStack * aErrorStack);

stlStatus stlRecvFrom( stlSocket       aSock,
                       stlSockAddr   * aFrom,
                       stlInt32        aFlags,
                       stlChar       * aBuf, 
                       stlSize       * aLen,
                       stlBool         aIsSockStream,
                       stlErrorStack * aErrorStack );

stlStatus stlSendMsg( stlSocket         aSock,
                      const stlMsgHdr  *aMsgHdr,
                      stlInt32          aFlags,
                      stlErrorStack    *aErrorStack );

stlStatus stlRecvMsg( stlSocket         aSock,
                      stlMsgHdr        *aMsgHdr,
                      stlInt32          aFlags,
                      stlErrorStack    *aErrorStack );

stlStatus stlSocketPair( stlInt32          aDomain,
                         stlInt32          aType,
                         stlInt32          aProtocol,
                         stlSocket         aSockets[2],
                         stlErrorStack    *aErrorStack );

stlStatus stlSetSocketOption( stlSocket       aSock, 
                              stlInt32        aOpt,
                              stlInt32        aOn,
                              stlErrorStack * aErrorStack );

stlStatus stlGetSocketOption( stlSocket       aSock, 
                              stlInt32        aOpt,
                              stlSize       * aRet,
                              stlErrorStack * aErrorStack );

stlStatus stlGetHostName( stlChar       * aBuf,
                          stlSize         aLen,
                          stlErrorStack * aErrorStack );

stlStatus stlGetSockName( stlSocket        aSock,
                          stlSockAddr    * aSockAddr,
                          stlErrorStack  * aErrorStack);
 
stlStatus stlGetPeerName( stlSocket        aSock,
                          stlSockAddr    * aSockAddr,
                          stlErrorStack  * aErrorStack);
 
stlStatus stlGetSockAddrIp( stlSockAddr   * aSockAddr,
                            stlChar       * aAddrBuf,
                            stlSize         aAddrBufLen,
                            stlErrorStack * aErrorStack );

stlStatus stlGetStrAddrFromBin( stlInt32        aAf,
                                const void    * aSrc,
                                stlChar       * aDst,
                                stlSize         aSize,
                                stlErrorStack * aErrorStack );

stlStatus stlGetBinAddrFromStr( stlInt32        aAf,
                                const stlChar * aSrc,
                                void          * aDst,
                                stlErrorStack * aErrorStack);

stlStatus stlSetSockAddr( stlSockAddr   * aSockAddr,
                          stlInt16        aFamily,
                          stlChar       * aAddrStr,
                          stlPort         aPort,
                          stlChar *       aFilePath,
                          stlErrorStack * aErrorStack);

stlStatus stlGetSockAddr( stlSockAddr   * aSockAddr,
                          stlChar       * aAddrBuf,
                          stlSize         aAddrBufLen,
                          stlPort       * aPort,
                          stlChar       * aFilePathBuf,
                          stlInt32        aFilePathBufLen,
                          stlErrorStack * aErrorStack);

/* @} */
    
STL_END_DECLS

#endif /* _STLNETWORKIO_H_ */
