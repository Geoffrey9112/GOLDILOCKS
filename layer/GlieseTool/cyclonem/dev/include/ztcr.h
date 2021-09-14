/*******************************************************************************
 * ztcr.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _ZTCR_H_
#define _ZTCR_H_ 1

/**
 * @file ztcr.h
 * @brief GlieseTool Cyclone Receiver Communicator Routines
 */


/**
 * @defgroup ztcs Cyclone Receiver(slave) Routines
 * @{
 */

stlStatus ztcrInitializeCM4Slave( stlErrorStack * aErrorStack );

stlStatus ztcrFinalizeCM4Slave( stlErrorStack * aErrorStack );

stlStatus ztcrCreateSub( stlErrorStack * aErrorStack );


/**
 * Protocol
 */

stlStatus ztcrDoSubProtocol( stlErrorStack * aErrorStack );

stlStatus ztcrInitProtocol( stlErrorStack * aErrorStack );

stlStatus ztcrHandshakeProtocol( stlErrorStack * aErrorStack );

stlStatus ztcrRecvLogByteOrder( stlErrorStack * aErrorStack );

stlStatus ztcrResponseTableInfo( stlErrorStack * aErrorStack );

stlStatus ztcrResponseTableCount( stlErrorStack * aErrorStack );

stlStatus ztcrResponseTableName( stlChar       * aData,
                                 stlInt32        aSize,
                                 stlErrorStack * aErrorStack );

stlStatus ztcrRequestTableId( stlInt32      * aTableId,
                              stlErrorStack * aErrorStack );

stlStatus ztcrRequestColumnCount( stlInt32      * aColumnCount,
                                  stlErrorStack * aErrorStack );

stlStatus ztcrRequestColumnInfo( ztcColumnInfo          * aColumnInfo,
                                 stlErrorStack          * aErrorStack );

stlStatus ztcrResponseColumnInfoResult( stlInt32        aResult,
                                        stlErrorStack * aErrorStack );


stlStatus ztcrReceiveFlowProtocol( stlErrorStack * aErrorStack );

stlStatus ztcrSyncProtocol( stlErrorStack * aErrorStack );

ztclStatus ztcrOnRecvCallback( ztclSubscriber      * aContext,
                               ztclRecvData        * aRecvData,
                               const stlChar       * aDataType,
                               void                * aUser );

ztclStatus ztcrOnEventCallback( ztclEventType   aEventType,
                                void          * aData,
                                void          * aUser );


/**
 * Chunk
 */

//stlStatus ztcrWriteDataToChunk( stlChar       * aBuffer,
//                                stlInt64        aSize,
//                                stlErrorStack * aErrorStack );

stlStatus ztcrWriteDataArrayToChunk( ztclRecvData  * aRecvData,
                                     stlErrorStack * aErrorStack );

stlStatus ztcrGetChunkItemToWrite( ztcChunkItem  ** aChunkItem,
                                   stlErrorStack  * aErrorStack );

stlStatus ztcrPushChunkToWaitReadList( ztcChunkItem  * aChunkItem,
                                       stlErrorStack * aErrorStack );


/** @} */

#endif /* _ZTCR_H_ */
