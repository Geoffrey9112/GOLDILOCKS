#ifndef  __STN_IB_IMPL_H__
#define  __STN_IB_IMPL_H__

#include <stl.h>

stlStatus stnRcCreateForServer( stlRcHandle   * aHandle,
                                stlChar       * aBindAddr,
                                stlUInt16       aBindPort, 
                                stlUInt32       aSendBufferSize,
                                stlUInt32       aSendBufferCount,
                                stlUInt32       aRecvBufferSize,
                                stlUInt32       aRecvBufferCount,
                                stlInterval     aTimeoutUsec,
                                stlErrorStack * aErrorStack );

stlStatus stnRcCreateForClient( stlRcHandle   * aHandle,
                                stlChar       * aBindAddr,
                                stlChar       * aTargetAddr,
                                stlUInt16       aTargetPort, 
                                stlUInt32       aSendBufferSize,
                                stlUInt32       aSendBufferCount,
                                stlUInt32       aRecvBufferSize,
                                stlUInt32       aRecvBufferCount,
                                stlErrorStack * aErrorStack );

stlStatus stnRcSetSendTimeout( stlRcHandle     aHandle,
                               stlInterval     aTimeoutUsec,
                               stlInterval     aBusyWaitUsec,
                               stlErrorStack * aErrorStack );

stlStatus stnRcSetRecvTimeout( stlRcHandle     aHandle,
                               stlInterval     aTimeoutUsec,
                               stlInterval     aBusyWaitUsec,
                               stlErrorStack * aErrorStack );

stlStatus stnRcSend( stlRcHandle   * aHandle,
                     const stlChar * aBuf,
                     stlSize       * aLen,
                     stlErrorStack * aErrorStack );

stlStatus stnRcRecv( stlRcHandle     aHandle,
                     stlChar       * aBuf,
                     stlSize       * aLen,
                     stlErrorStack * aErrorStack);

stlStatus stnRcClose( stlRcHandle   * aHandle,
                      stlErrorStack * aErrorStack );


#endif

