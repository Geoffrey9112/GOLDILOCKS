/*******************************************************************************
 * stn.h
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

#include <stc.h>
#include <stlDef.h>

#ifndef _STN_H_
#define _STN_H_ 1

STL_BEGIN_DECLS

/* stnSockets */
stlStatus stnCreateSocket( stlSocket     * aNewSocket, 
                           stlInt16        aFamily,
                           stlInt32        aType,
                           stlInt32        aProtocol,
                           stlErrorStack * aErrorStack );

stlStatus stnShutdownSocket( stlSocket        aSocket, 
                             stlShutdownHow   aHow,
                             stlErrorStack  * aErrorStack );

stlStatus stnCloseSocket( stlSocket       aSocket,
                          stlErrorStack * aErrorStack );

stlStatus stnBind( stlSocket       aSock,
                   stlSockAddr   * aSockAddr,
                   stlErrorStack * aErrorStack );

stlStatus stnListen( stlSocket       aSock,
                     stlUInt32       aBacklog,
                     stlErrorStack * aErrorStack );

stlStatus stnAccept( stlSocket       aSock,
                     stlSocket     * aNewSock,
                     stlErrorStack * aErrorStack );

stlStatus stnConnect( stlSocket       aSock,
                      stlSockAddr   * aSockAddr,
                      stlInterval     aTimeout,
                      stlErrorStack * aErrorStack );

/* stnSendRecv */
stlStatus stnSend( stlSocket      aSock,
                   const stlChar * aBuf, 
                   stlSize       * aLen,
                   stlErrorStack * aErrorStack );

stlStatus stnRecv( stlSocket       aSock,
                   stlChar       * aBuf,
                   stlSize       * aLen,
                   stlErrorStack * aErrorStack);

stlStatus stnSendTo( stlSocket       aSock,
                     stlSockAddr   * aWhere,
                     stlInt32        aFlags,
                     const stlChar * aBuf,
                     stlSize       * aLen,
                     stlErrorStack * aErrorStack);

stlStatus stnRecvFrom( stlSocket       aSock,
                       stlSockAddr   * aFrom,
                       stlInt32        aFlags,
                       stlChar       * aBuf, 
                       stlSize       * aLen,
                       stlBool         aIsSockStream,
                       stlErrorStack * aErrorStack );

stlStatus stnSendMsg( stlSocket         aSock,
                      const stlMsgHdr  *aMsgHdr,
                      stlInt32          aFlags,
                      stlErrorStack    *aErrorStack );

stlStatus stnRecvMsg( stlSocket         aSock,
                      stlMsgHdr        *aMsgHdr,
                      stlInt32          aFlags,
                      stlErrorStack    *aErrorStack );

stlStatus stnSocketPair( stlInt32          aDomain,
                         stlInt32          aType,
                         stlInt32          aProtocol,
                         stlSocket         aSockets[2],
                         stlErrorStack    *aErrorStack );

/* stnSockOpt */
stlStatus stnSetSocketOption( stlSocket       aSock, 
                              stlInt32        aOpt,
                              stlInt32        aOn,
                              stlErrorStack * aErrorStack );

stlStatus stnGetSocketOption( stlSocket       aSock, 
                              stlInt32        aOpt,
                              stlSize       * aRet,
                              stlErrorStack * aErrorStack );

stlStatus stnGetHostName( stlChar       * aBuf,
                          stlSize         aLen,
                          stlErrorStack * aErrorStack );

stlStatus stnGetSockName( stlSocket       aSock,
                          stlSockAddr   * aSockAddr,
                          stlErrorStack * aErrorStack);
 
stlStatus stnGetPeerName( stlSocket        aSock,
                          stlSockAddr    * aSockAddr,
                          stlErrorStack  * aErrorStack);

stlStatus stnGetSockAddrIp( stlSockAddr   * aSockAddr,
                            stlChar       * aAddrBuf,
                            stlSize         aAddrBufLen,
                            stlErrorStack * aErrorStack );

stlStatus stnGetStrAddrFromBin( stlInt32        aAf,
                                const void    * aSrc,
                                stlChar       * aDst,
                                stlSize         aSize,
                                stlErrorStack * aErrorStack );

stlStatus stnGetBinAddrFromStr( stlInt32        aAf,
                                const stlChar * aSrc,
                                void          * aDst,
                                stlErrorStack * aErrorStack);

stlStatus stnSetSockAddr( stlSockAddr   * aSockAddr,
                          stlInt16        aFamily,
                          stlChar       * aAddrStr,
                          stlPort         aPort,
                          stlChar       * aFilePath,
                          stlErrorStack * aErrorStack);

stlStatus stnGetSockAddr( stlSockAddr   * aSockAddr,
                          stlChar       * aAddrBuf,
                          stlSize         aAddrBufLen,
                          stlPort       * aPort,
                          stlChar       * aFilePathBuf,
                          stlInt32        aFilePathBufLen,
                          stlErrorStack * aErrorStack);

STL_END_DECLS

#endif /* _STN_H_ */
