/*******************************************************************************
 * cmgSocket.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: cmgSocket.h 6534 2012-12-04 00:37:52Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _CMGSOCKET_H_
#define _CMGSOCKET_H_ 1

/**
 * @file cmgSocket.h
 * @brief Communication Layer Protocol Socket Routines
 */

/**
 * @defgroup cmgSocket Socket
 * @ingroup cmExternal
 * @{
 */

stlStatus cmgSend( stlSocket       aSocket,
                   stlChar       * aBuffer,
                   stlInt32        aSize,
                   stlBool         aMtuSize,
                   stlErrorStack * aErrorStack );

stlStatus cmgRecv( cmlHandle     * aHandle,
                   stlErrorStack * aErrorStack );

stlStatus cmgRecvRequestAsync( stlContext    * aContext,
                               stlChar       * aReadBuffer,
                               stlInt64        aBufferSize,
                               stlInt32      * aReadSize,
                               stlInt32      * aPayloadSize,
                               stlUInt8      * aPeriod,
                               stlBool       * aReceived,
                               stlErrorStack * aErrorStack );



/** @} */

#endif /* _CMGSOCKET_H_ */
