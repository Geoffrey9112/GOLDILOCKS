/*******************************************************************************
 * ztrt.h
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


#ifndef _ZTRT_H_
#define _ZTRT_H_ 1

/**
 * @file ztrt.h
 * @brief GlieseTool LogMirror Thread Routines
 */


/**
 * @defgroup ztrt LogMirror Thread Routines
 * @{
 */


stlStatus ztrtCreateSenderThread( stlErrorStack * aErrorStack );

stlStatus ztrtJoinSenderThread( stlErrorStack * aErrorStack );

stlStatus ztrtCreateReceiverThread( stlErrorStack * aErrorStack );

stlStatus ztrtJoinReceiverThread( stlErrorStack * aErrorStack );

stlStatus ztrtCreateFlusherThread( stlErrorStack * aErrorStack );

stlStatus ztrtJoinFlusherThread( stlErrorStack * aErrorStack );



/** @} */

#endif /* _ZTRT_H_ */
