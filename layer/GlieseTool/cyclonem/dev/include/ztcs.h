/*******************************************************************************
 * ztcs.h
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


#ifndef _ZTCS_H_
#define _ZTCS_H_ 1

/**
 * @file ztcs.h
 * @brief GlieseTool Cyclone Sender Communicator Routines
 */


/**
 * @defgroup ztcs Cyclone Sender(Master) Routines
 * @{
 */

stlStatus ztcsCreateListener( stlErrorStack * aErrorStack );

stlStatus ztcsInitializeCM4Master( stlErrorStack * aErrorStack );

stlStatus ztcsFinalizeCM4Master( stlErrorStack * aErrorStack );

/**
 * PROTOCOL
 */

stlStatus ztcsDoProtocol( stlErrorStack * aErrorStack );

stlStatus ztcsDoPubProtocol( stlErrorStack * aErrorStack );

stlStatus ztcsInitProtocol( stlContext    * aContext,
                            stlChar       * aClientType,
                            stlErrorStack * aErrorStack );
                           
stlStatus ztcsHandshakeProtocol( stlErrorStack * aErrorStack );

stlStatus ztcsSendLogByteOrder( stlErrorStack * aErrorStack );

stlStatus ztcsRequestTableInfo( stlErrorStack * aErrorStack );

stlStatus ztcsRequestTableCount( stlInt32      * aTableCount,
                                 stlErrorStack * aErrorStack );
                                
stlStatus ztcsRequestTableName( stlChar       * aData,
                                stlErrorStack * aErrorStack );

stlStatus ztcsResponseTableId( stlInt32        aTableId,
                               stlErrorStack * aErrorStack );

stlStatus ztcsCompareColumnInfo( ztcMasterTableMeta * aTableMeta,
                                 stlErrorStack      * aErrorStack );
                                 
stlStatus ztcsResponseColumnCount( stlInt32        aColumnCount,
                                   stlErrorStack * aErrorStack );

stlStatus ztcsResponseColumnInfo( ztcColumnInfo         * aColumnInfo,
                                  stlErrorStack         * aErrorStack );


stlStatus ztcsPubFlowProtocol( stlErrorStack * aErrorStack );

stlStatus ztcsSendEndProtocol( stlErrorStack * aErrorStack );

stlStatus ztcsPubEndProtocol( stlErrorStack * aErrorStack );


stlStatus ztcsSyncProtocol( stlBool         aStart,
                            stlErrorStack * aErrorStack );

stlStatus ztcsMonitorProtocol( stlErrorStack * aErrorStack );

/**
 * Chunk
 */
                


/** @} */


#endif /* _ZTCS_H_ */
