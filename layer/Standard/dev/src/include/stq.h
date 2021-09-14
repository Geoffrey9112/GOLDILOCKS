/*******************************************************************************
 * stq.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: stq.h 6761 2012-12-18 04:49:11Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _STQ_H_
#define _STQ_H_ 1

#include <stlDef.h>

STL_BEGIN_DECLS

stlStatus stqOpen( stlMsgQueue   * aMsgQueue,
                   stlInt32        aMsgKey,
                   stlBool         aExcl,
                   stlErrorStack * aErrorStack );

stlStatus stqClose( stlMsgQueue   * aMsgQueue,
                    stlErrorStack * aErrorStack );

stlStatus stqSend( stlMsgQueue   * aMsgQueue,
                   stlMsgBuffer  * aMsgBuffer,
                   stlInt64        aMsgSize,
                   stlErrorStack * aErrorStack );

stlStatus stqRecv( stlMsgQueue   * aMsgQueue,
                   stlMsgBuffer  * aMsgBuffer,
                   stlInt32        aMsgSize,
                   stlInt64        aMsgType,
                   stlInt32        aMsgFlag,
                   stlSize       * aReceivedSize,
                   stlErrorStack * aErrorStack );

STL_END_DECLS

#endif /* _STQ_H_ */
