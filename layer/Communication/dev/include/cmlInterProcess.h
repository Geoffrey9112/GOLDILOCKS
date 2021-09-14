/*******************************************************************************
 * cmlInterProcess.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: cmlInterProcess.h 6534 2012-12-04 00:37:52Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _CMLINTERPROCESS_H_
#define _CMLINTERPROCESS_H_ 1

/**
 * @file cmlInterProcess.h
 * @brief Communication Layer Protocol Routines
 */

/**
 * @defgroup cmlInterProcess Protocol
 * @ingroup cmInterProcess
 * @{
 */

stlStatus cmlSendAckTo( stlContext      * aUdsContext,
                        stlSockAddr     * aTargetAddr,
                        stlTime           aSenderStartTime,
                        stlInt64          aSequence,
                        stlInt64          aIdx,
                        stlErrorStack   * aErrorStack );

stlStatus cmlSendRequestFdTo( stlContext      * aUdsContext,
                              stlSockAddr     * aTargetAddr,
                              stlErrorStack   * aErrorStack );

stlStatus cmlSendRequestQuitTo( stlContext      * aUdsContext,
                                stlSockAddr     * aTargetAddr,
                                stlErrorStack   * aErrorStack );

stlStatus cmlSendRequestStatusTo( stlContext      * aUdsContext,
                                  stlSockAddr     * aTargetAddr,
                                  stlErrorStack   * aErrorStack );

stlStatus cmlSendFdTo( stlContext      * aUdsContext,
                       stlSockAddr     * aTargetAddr,
                       stlContext      * aPassingContext,
                       stlInt64          aKey,
                       stlTime           aSenderStartTime,
                       stlInt64          aSequence,
                       stlInt64          aIdx,
                       stlUInt16         aMajorVersion,
                       stlUInt16         aMinorVersion,
                       stlUInt16         aPatchVersion,
                       stlErrorStack   * aErrorStack );

stlStatus cmlSendStatusTo( stlContext      * aUdsContext,
                           stlSockAddr     * aTargetAddr,
                           stlChar         * aStatus,
                           stlErrorStack   * aErrorStack );

stlStatus cmlRecvFrom( stlContext      * aUdsContext,
                       stlSockAddr     * aPeerAddr,
                       cmlUdsPacket    * aPacket,
                       stlErrorStack   * aErrorStack );

stlStatus cmlRecvFdFrom( stlContext      * aUdsContext,
                         stlContext      * aRecvContext,
                         cmlUdsForwardFd * aForwardFd,
                         stlErrorStack   * aErrorStack );

stlStatus cmlSetInterProcessProtocolVersion( cmlUdsVersion   * aVersion,
                                             stlErrorStack   * aErrorStack);

stlStatus cmlCheckInterProcessProtocolVersion( cmlUdsVersion   * aVersion,
                                               stlErrorStack   * aErrorStack);

/** @} */

#endif /* _CMLINTERPROCESS_H_ */
