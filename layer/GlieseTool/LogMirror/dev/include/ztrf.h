/*******************************************************************************
 * ztrf.h
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


#ifndef _ZTRF_H_
#define _ZTRF_H_ 1

/**
 * @file ztrf.h
 * @brief GlieseTool LogMirror Flusher Routines
 */


/**
 * @defgroup ztrf LogMirror Flusher Routines
 * @{
 */

stlStatus ztrfInitializeFlusher( stlErrorStack * aErrorStack );

stlStatus ztrfFinalizeFlusher( stlErrorStack * aErrorStack );

stlStatus ztrfFlusherMain( stlErrorStack * aErrorStack );

stlStatus ztrfDoFlush( stlErrorStack * aErrorStack );

stlStatus ztrfUpdateControlFile( stlChar       * aBuffer,
                                 stlErrorStack * aErrorStack );

stlStatus ztrfReadControlFile( ztrLogFileHdr * aLogFileHdr,
                               stlBool       * aIsExist,
                               stlErrorStack * aErrorStack );

stlStatus ztrfGetLastBlockSeq( stlInt64        aLogFileSeqNo,
                               stlInt64      * aLastBlockSeq,
                               stlErrorStack * aErrorStack );

/**
 * Buffer
 */

stlStatus ztrfGetChunkItemToRead( ztrChunkItem ** aChunkItem,
                                  stlErrorStack * aErrorStack );

stlStatus ztrfPushChunkToWaitWriteList( ztrChunkItem  * aChunkItem,
                                        stlErrorStack * aErrorStack );

/** @} */

#endif /* _ZTRF_H_ */
