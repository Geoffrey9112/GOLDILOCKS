/****************************************
 * stlIbApi.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id:$
 *
 * NOTES
 *    
 *
 ***************************************/


#ifndef _STL_IB_API_H_
#define _STL_IB_API_H_ 1

/**
 * @file stlIbApi.h
 * @brief Standard Layer Infiniband API
 */


STL_BEGIN_DECLS

/**
 * @defgroup stlRcApi Infiniband_RC Routines
 * @ingroup STL 
 * @{
 */
stlStatus stlRcCreateForServer( stlRcHandle   * aHandle,
                                stlChar       * aBindAddr,
                                stlUInt16       aBindPort, 
                                stlUInt32       aSendBufferSize,
                                stlUInt32       aSendBufferCount,
                                stlUInt32       aRecvBufferSize,
                                stlUInt32       aRecvBufferCount,
                                stlInterval     aTimeoutUsec,
                                stlErrorStack * aErrorStack );

stlStatus stlRcCreateForClient( stlRcHandle   * aHandle,
                                stlChar       * aBindAddr,
                                stlChar       * aTargetAddr,
                                stlUInt16       aTargetPort, 
                                stlUInt32       aSendBufferSize,
                                stlUInt32       aSendBufferCount,
                                stlUInt32       aRecvBufferSize,
                                stlUInt32       aRecvBufferCount,
                                stlErrorStack * aErrorStack );

stlStatus stlRcSetSendTimeout( stlRcHandle     aHandle,
                               stlInterval     aTimeoutUsec,
                               stlInterval     aBusyWaitUsec,
                               stlErrorStack * aErrorStack );

stlStatus stlRcSetRecvTimeout( stlRcHandle     aHandle,
                               stlInterval     aTimeoutUsec,
                               stlInterval     aBusyWaitUsec,
                               stlErrorStack * aErrorStack );

stlStatus stlRcSend( stlRcHandle   * aHandle,
                     const stlChar * aBuf,
                     stlSize       * aLen,
                     stlErrorStack * aErrorStack );

stlStatus stlRcRecv( stlRcHandle     aHandle,
                     stlChar       * aBuf,
                     stlSize       * aLen,
                     stlErrorStack * aErrorStack);

stlStatus stlRcClose( stlRcHandle   * aHandle,
                      stlErrorStack * aErrorStack );


/* @} */
    
STL_END_DECLS

#endif /* _STL_IB_API_H_ */
