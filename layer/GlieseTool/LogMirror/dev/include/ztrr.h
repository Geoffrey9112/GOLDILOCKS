/*******************************************************************************
 * ztrr.h
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


#ifndef _ZTRR_H_
#define _ZTRR_H_ 1

/**
 * @file ztrr.h
 * @brief GlieseTool LogMirror Receiver Routines
 */


/**
 * @defgroup ztrr LogMirror Receiver Routines
 * @{
 */

stlStatus ztrrInitializeReceiver( stlErrorStack * aErrorStack );

stlStatus ztrrFinalizeReceiver( stlErrorStack * aErrorStack );

stlStatus ztrrReceiverMain( stlErrorStack * aErrorStack );

stlStatus ztrrCreateReceiver( stlErrorStack * aErrorStack );

stlStatus ztrrCreateReceiver4IB( stlErrorStack * aErrorStack );

/**
 * Recovery
 */

stlStatus ztrrGetRecoveryInfo( ztrRecoveryinfo * aRecoveryInfo,
                               stlBool         * aIsExist,
                               stlErrorStack   * aErrorStack );


/**
 * Communcate
 */

stlStatus ztrrDoProtocol( stlErrorStack * aErrorStack );

stlStatus ztrrInitProtocol( stlErrorStack * aErrorStack );

stlStatus ztrrHandShakeProtocol( ztrRecoveryinfo * aRecoveryInfo,
                                 stlBool         * aDoRecovery,
                                 stlErrorStack   * aErrorStack );

stlStatus ztrrRequestBufferInfo( stlErrorStack * aErrorStack );

stlStatus ztrrResponseRecoveryInfo( ztrRecoveryinfo * aRecoveryInfo,
                                    stlBool         * aDoRecovery,
                                    stlErrorStack   * aErrorStack );

stlStatus ztrrRecvRedoLogProtocol( stlErrorStack * aErrorStack );

stlStatus ztrrEndProtocol( void          * aHandle,
                           stlErrorStack * aErrorStack );
/** 
 * Buffer
 */

stlStatus ztrrGetChunkItemToWrite( ztrChunkItem ** aChunkItem,
                                   stlErrorStack * aErrorStack );

stlStatus ztrrWriteDataToChunk( stlChar       * aBuffer,
                                stlInt64        aSize,
                                stlBool         aIsLogFileHdr,
                                stlErrorStack * aErrorStack );

stlStatus ztrrPushChunkToWaitReadList( ztrChunkItem  * aChunkItem,
                                       stlErrorStack * aErrorStack );

stlStatus ztrrPushFirstChunkToWaitReadList( stlErrorStack * aErrorStack );


/** @} */

#endif /* _ZTRR_H_ */
