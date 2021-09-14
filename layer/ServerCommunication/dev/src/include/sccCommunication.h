/*******************************************************************************
 * sccCommunication.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: sccCommunication.h 6534 2012-12-04 00:37:52Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _SCCCOMMUNICATION_H_
#define _SCCCOMMUNICATION_H_ 1

/**
 * @file sccCommunication.h
 * @brief Server Communication Layer Communication Component Routines
 */

/**
 * @defgroup sccCommunication Communication
 * @ingroup scExternal
 * @{
 */

stlStatus sccRecvFromSocket( sclHandle     * aHandle,
                             stlErrorStack * aErrorStack );

stlStatus sccRecvPacket( sclHandle     * aHandle,
                         sclEnv        * aEnv );

stlStatus sccFlushPacket( sclHandle     * aHandle,
                          sclEnv        * aEnv );

stlStatus sccSendPacket( sclHandle     * aHandle,
                         sclEnv        * aEnv );

stlStatus sccSendPacketInternal( sclHandle     * aHandle,
                                 stlInt8         aPeriod,
                                 sclEnv        * aEnv );

stlStatus sccInitializeQueue( sclQueue        * aQueue,
                              stlInt64          aItemSize,
                              stlInt64          aItemCount,
                              stlInt64        * aUsedMemory,
                              sclEnv          * aEnv );

stlStatus sccFinalizeQueue( sclQueue        * aQueue,
                            sclEnv          * aEnv );

stlStatus sccEnqueue( sclQueue          * aQueue,
                      void              * aItem,
                      sclEnv            * aEnv );

stlStatus sccDequeue( sclQueue          * aQueue,
                      void              * aItem,
                      stlInterval         aTimeout,
                      sclEnv            * aEnv );

stlStatus sccInitializeIpcSimplex( sclIpcSimplex       * aIpcSimplex,
                                   stlArrayAllocator   * aArrayAllocator,
                                   sclEnv              * aEnv );

stlStatus sccFinalizeIpcSimplex( sclIpcSimplex       * aIpcSimplex,
                                 stlArrayAllocator   * aArrayAllocator,
                                 sclEnv              * aEnv );


stlStatus sccGetSendBuffer( sclIpcSimplex       * aIpcSimplex,
                            stlArrayAllocator   * aArrayAllocator,
                            stlChar            ** aBuf,
                            sclEnv              * aEnv );

stlStatus sccCompleteSendBuffer( sclIpcSimplex      * aIpcSimplex,
                                 stlArrayAllocator  * aArrayAllocator,
                                 stlChar            * aBuffer,
                                 stlSize              aLen,
                                 sclEnv             * aEnv );

stlStatus sccGetRecvBuffer( sclIpcSimplex       * aIpcSimplex,
                            stlArrayAllocator   * aArrayAllocator,
                            stlChar            ** aBuf,
                            stlInt32            * aSize,
                            sclEnv              * aEnv );

stlStatus sccCompleteRecvBuffer( sclIpcSimplex      * aIpcSimplex,
                                 stlArrayAllocator  * aArrayAllocator,
                                 stlChar            * aBuffer,
                                 sclEnv             * aEnv);


stlStatus sccSendToSharedMemory( sclHandle     * aHandle,
                                 stlInt8         aPeriod,
                                 sclEnv        * aEnv );

stlStatus sccSendToSocket( stlContext    * aContext,
                           stlChar       * aData,
                           stlInt32        aDataLen,
                           stlSize       * aSentLen,
                           sclEnv        * aEnv );

stlStatus sccRecvFromSharedMemory( sclHandle     * aHandle,
                                   sclEnv        * aEnv );

stlStatus sccTimedAllocElement( stlArrayAllocator   * aArrayAllocator,
                                void               ** aElement,
                                stlInterval           aTimeout,
                                sclEnv              * aEnv );

stlStatus sccFreeElement( stlArrayAllocator   * aArrayAllocator,
                          void                * aElement,
                          sclEnv              * aEnv );

/** @} */

#endif /* _SCCCOMMUNICATION_H_ */

