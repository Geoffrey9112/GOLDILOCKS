/*******************************************************************************
 * stlMsgQueue.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: stlMsgQueue.h 9323 2013-08-09 03:26:54Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _STLMSGQUEUE_H_
#define _STLMSGQUEUE_H_ 1

/**
 * @file stlMsgQueue.h
 * @brief Standard Layer Message Queue Routines
 */

#include <stlDef.h>

STL_BEGIN_DECLS


/**
 * @defgroup stlMsgQueue Message Queue
 * @ingroup STL 
 * @{
 */


stlStatus stlCreateMsgQueue( stlMsgQueue   * aMsgQueue,
                             stlInt32        aMsgKey,
                             stlErrorStack * aErrorStack );

stlStatus stlGetMsgQueue( stlMsgQueue   * aMsgQueue,
                          stlInt32        aMsgKey,
                          stlErrorStack * aErrorStack );

stlStatus stlDestroyMsgQueue( stlMsgQueue   * aMsgQueue,
                              stlErrorStack * aErrorStack );

stlStatus stlSendMsgQueue( stlMsgQueue   * aMsgQueue,
                           stlMsgBuffer  * aMsgBuffer,
                           stlInt64        aMsgSize,
                           stlErrorStack * aErrorStack );

stlStatus stlRecvMsgQueue( stlMsgQueue   * aMsgQueue,
                           stlMsgBuffer  * aMsgBuffer,
                           stlInt32        aMsgSize,
                           stlInt64        aMsgType,
                           stlInt32        aMsgFlag,
                           stlSize       * aReceivedSize,
                           stlErrorStack * aErrorStack );

stlStatus stlAllocMsgBuffer( stlInt64         aMsgSize,
                             stlMsgBuffer  ** aMsgBuffer,
                             stlErrorStack  * aErrorStack );

stlStatus stlFreeMsgBuffer( stlMsgBuffer  * aMsgBuffer,
                            stlErrorStack * aErrorStack );

stlStatus stlCopyToMsgBuffer( stlMsgBuffer  * aMsgBuffer,
                              void          * aMsg,
                              stlInt64        aMsgSize,
                              stlInt64        aMsgType,
                              stlErrorStack * aErrorStack );

stlStatus stlCreateMsgQueue4Startup( stlMsgQueue   * aMsgQueue,
                                     stlInt32      * aMsgKey,
                                     stlErrorStack * aErrorStack );


/** @} */
    
STL_END_DECLS

#endif /* _STLMSGQUEUE_H_ */
