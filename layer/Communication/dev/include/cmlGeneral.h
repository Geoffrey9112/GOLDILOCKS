/*******************************************************************************
 * cmlGeneral.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: cmlGeneral.h 6534 2012-12-04 00:37:52Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _CMLGENERAL_H_
#define _CMLGENERAL_H_ 1

/**
 * @file cmlGeneral.h
 * @brief Communication Layer General Routines
 */

/**
 * @defgroup cmlGeneral General
 * @ingroup cmExternal
 * @{
 */

stlStatus cmlInitialize();
stlStatus cmlTerminate();

stlStatus cmlInitSockOption( stlContext    * aContext,
                             stlInt32        aBufferSize,
                             stlErrorStack * aErrorStack );

stlStatus cmlInitializeMemoryManager( cmlMemoryManager  * aCmlAllocator,
                                      stlInt64            aPacketBufferSize,
                                      stlInt64            aOperationBufferSize,
                                      stlErrorStack     * aErrorStack );

stlStatus cmlFinalizeMemoryManager( cmlMemoryManager  * aCmlAllocator,
                                    stlErrorStack     * aErrorStack );

stlStatus cmlInitializeHandle( cmlHandle        * aHandle,
                               stlContext       * aContext,
                               stlInt32           aEndian,
                               stlErrorStack    * aErrorStack );

stlStatus cmlFinalizeHandle( cmlHandle     * aHandle,
                             stlErrorStack * aErrorStack );

stlStatus cmlInitializeProtocolSentence( cmlHandle               * aHandle,
                                         cmlProtocolSentence     * aProtocolSentence,
                                         cmlMemoryManager        * aMemoryMgr,
                                         stlErrorStack           * aErrorStack );

stlStatus cmlFinalizeProtocolSentence( cmlProtocolSentence      * aProtocolSentence,
                                       stlErrorStack            * aErrorStack );

stlStatus cmlSendVersion( cmlHandle     * aHandle,
                          stlErrorStack * aErrorStack );

stlStatus cmlFlushPacket( cmlHandle     * aHandle,
                          stlErrorStack * aErrorStack );

stlStatus cmlSendPacket( cmlHandle     * aHandle,
                         stlErrorStack * aErrorStack );

stlStatus cmlRecvPacket( cmlHandle     * aHandle,
                         stlErrorStack * aErrorStack );

stlStatus cmlSendPacketInternal( cmlHandle     * aHandle,
                                 stlUInt8        aPeriod,
                                 stlErrorStack * aErrorStack );

stlStatus cmlSaveStaticAreaInfo( cmlShmStaticFileInfo * aShmInfo,
                                 stlErrorStack        * aErrorStack );

stlStatus cmlLoadStaticAreaInfo( cmlShmStaticFileInfo * aShmInfo,
                                 stlErrorStack        * aErrorStack );

stlStatus cmlRecvRequestAsync( stlContext    * aContext,
                               stlChar       * aReadBuffer,
                               stlInt64        aBufferSize,
                               stlInt32      * aReadSize,
                               stlInt32      * aPayloadSize,
                               stlUInt8      * aPeriod,
                               stlBool       * aReceived,
                               stlErrorStack * aErrorStack );

stlStatus cmlSend( stlSocket       aSocket,
                   stlChar       * aBuffer,
                   stlInt32        aSize,
                   stlBool         aMtuSize,
                   stlErrorStack * aErrorStack );

/** @} */

#endif /* _CMLGENERAL_H_ */
