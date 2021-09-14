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

stlStatus ztcrCreateReceiver( stlErrorStack * aErrorStack );


/**
 * Protocol
 */

stlStatus ztcrDoProtocol( stlErrorStack * aErrorStack );

stlStatus ztcrInitProtocol( stlErrorStack * aErrorStack );

stlStatus ztcrHandshakeProtocol( stlErrorStack * aErrorStack );

stlStatus ztcrResponseTableInfo( stlErrorStack * aErrorStack );

stlStatus ztcrResponseTableCount( stlErrorStack * aErrorStack );

stlStatus ztcrResponseTableName( stlChar       * aData,
                                 stlInt32        aSize,
                                 stlErrorStack * aErrorStack );

stlStatus ztcrRequestTableId( stlInt32      * aTableId,
                              stlErrorStack * aErrorStack );

stlStatus ztcrRequestColumnCount( stlInt32      * aColumnCount,
                                  stlErrorStack * aErrorStack );

stlStatus ztcrRequestColumnInfo( ztcProtocolColumnInfo  * aColumnInfo,
                                 stlErrorStack          * aErrorStack );

stlStatus ztcrResponseColumnInfoResult( stlInt32        aResult,
                                        stlErrorStack * aErrorStack );

stlStatus ztcrResponseCaptureInfo( stlErrorStack * aErrorStack );

stlStatus ztcrReceiveFlowProtocol( stlErrorStack * aErrorStack );

stlStatus ztcrEndProtocol( stlErrorStack * aErrorStack );

stlStatus ztcrSyncProtocol( stlErrorStack * aErrorStack );


/**
 * Chunk
 */

stlStatus ztcrWriteDataToChunk( stlChar       * aBuffer,
                                stlInt64        aSize,
                                stlErrorStack * aErrorStack );

stlStatus ztcrGetChunkItemToWrite( ztcChunkItem  ** aChunkItem,
                                   stlErrorStack  * aErrorStack );

stlStatus ztcrPushChunkToWaitReadList( ztcChunkItem  * aChunkItem,
                                       stlErrorStack * aErrorStack );

stlStatus ztcrPushFirstChunkToWaitReadList( stlErrorStack * aErrorStack );


/** @} */

#endif /* _ZTCR_H_ */
