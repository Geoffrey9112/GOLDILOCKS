/*******************************************************************************
 * sclCommunication.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: sclCommunication.h 6534 2012-12-04 00:37:52Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _SCLCOMMUNICATION_H_
#define _SCLCOMMUNICATION_H_ 1

/**
 * @file sclCommunication.h
 * @brief Communication Layer Protocol Routines
 */

/**
 * @defgroup sclCommunication Protocol
 * @ingroup sclExternal
 * @{
 */


stlStatus sclInitializeQueue( sclQueue        * aQueue,
                              stlInt64          aItemSize,
                              stlInt64          aItemCount,
                              stlInt64        * aUsedMemory,
                              sclEnv          * aEnv );

stlStatus sclFinalizeQueue( sclQueue        * aQueue,
                            sclEnv          * aEnv );

stlStatus sclEnqueue( sclQueue          * aQueue,
                      void              * aItem,
                      sclEnv            * aEnv );

stlStatus sclDequeue( sclQueue          * aQueue,
                      void              * aItem,
                      stlInterval         aTimeout,
                      sclEnv            * aEnv );

stlStatus sclInitializeRequestQueue( sclQueue        * aQueue,
                                     stlInt64          aItemCount,
                                     stlInt64        * aUsedMemory,
                                     sclEnv          * aEnv );

stlStatus sclFinalizeRequestQueue( sclQueue      * aQueue,
                                   sclEnv        * aEnv );

stlStatus sclEnqueueRequest( sclQueue         * aQueue,
                             sclHandle        * aSclHandle,
                             sclEnv           * aEnv );

stlStatus sclDequeueRequest( sclQueue          * aQueue,
                             sclHandle        ** aSclHandle,
                             stlInterval         aTimeout,
                             sclEnv            * aEnv );


stlStatus sclInitializeResponseQueue( sclQueue        * aQueue,
                                      stlInt64          aItemCount,
                                      stlInt64        * aUsedMemory,
                                      sclEnv          * aEnv );

stlStatus sclFinalizeResponseQueue( sclQueue      * aQueue,
                                    sclEnv        * aEnv );

stlStatus sclEnqueueResponse( sclQueue         * aQueue,
                              sclHandle        * aSclHandle,
                              sclQueueCommand    aCommand,
                              stlInt64           aCmUnitCount,
                              sclEnv           * aEnv );

stlStatus sclDequeueResponse( sclQueue          * aQueue,
                              sclHandle        ** aSclHandle,
                              sclQueueCommand   * aCommand,
                              stlInt64          * aCmUnitCount,
                              stlInterval         aTimeout,
                              sclEnv            * aEnv );

stlStatus sclInitializeIpc( sclIpc              * aIpc,
                            stlArrayAllocator   * aArrayAllocator,
                            sclQueue            * aResponseEvent,
                            sclEnv              * aEnv );

stlStatus sclFinalizeIpc( sclIpc              * aIpc,
                          stlArrayAllocator   * aArrayAllocator,
                          sclEnv              * aEnv );

stlStatus sclInitializeIpcSimplex( sclIpcSimplex       * aIpcSimplex,
                                   stlArrayAllocator   * aArrayAllocator,
                                   sclEnv              * aEnv );

stlStatus sclFinalizeIpcSimplex( sclIpcSimplex       * aIpcSimplex,
                                 stlArrayAllocator   * aArrayAllocator,
                                 sclEnv              * aEnv );

stlStatus sclGetSendBuffer( sclIpcSimplex       * aIpcSimplex,
                            stlArrayAllocator   * aArrayAllocator,
                            stlChar            ** aBuf,
                            sclEnv              * aEnv );

stlStatus sclCompleteSendBuffer( sclIpcSimplex      * aIpcSimplex,
                                 stlArrayAllocator  * aArrayAllocator,
                                 stlChar            * aBuffer,
                                 stlSize              aLen,
                                 sclEnv             * aEnv );

stlStatus sclGetRecvBuffer( sclIpcSimplex       * aIpcSimplex,
                            stlArrayAllocator   * aArrayAllocator,
                            stlChar            ** aBuf,
                            stlInt32            * aSize,
                            sclEnv              * aEnv );

stlStatus sclCompleteRecvBuffer( sclIpcSimplex      * aIpcSimplex,
                                 stlArrayAllocator  * aArrayAllocator,
                                 stlChar            * aBuffer,
                                 sclEnv             * aEnv);


stlStatus sclSendToSocket( stlContext    * aContext,
                           stlChar       * aData,
                           stlInt32        aDataLen,
                           stlSize       * aSentLen,
                           sclEnv        * aEnv );


/** @} */

#endif /* _SCLCOMMUNICATION_H_ */
