/*******************************************************************************
 * ztqQueryHistory.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _ZTQQUERYHISTORY_H_
#define _ZTQQUERYHISTORY_H_ 1

/**
 * @file ztqQueryHistory.h
 * @brief Query History Definition
 */

/**
 * @defgroup ztqQueryHistory Query History
 * @ingroup ztq
 * @{
 */

stlStatus ztqAllocHistoryCommandBuffer( ztqHistoryQueue * aHistoryEntry,
                                        stlInt64          aBufSize,
                                        stlErrorStack   * aErrorStack );

void      ztqFreeHistoryCommandBuffer( ztqHistoryQueue * aHistoryEntry );

stlStatus ztqAllocHistoryQueue( ztqHistoryQueue **aHistoryQueue,
                                stlInt32          aHistorySize,
                                stlErrorStack    *aErrorStack );

stlStatus ztqFreeHistoryQueue( ztqHistoryQueue  *aHistoryQueue,
                               stlInt32          aHistorySize,
                               stlErrorStack    *aErrorStack );

stlStatus ztqSetHistoryCommand( ztqHistoryQueue *aHistoryEntry,
                                stlChar         *aString,
                                stlInt64         aId,
                                stlErrorStack   *aErrorStack );

stlStatus ztqInitHistory( stlErrorStack * aErrorStack );

stlStatus ztqAddHistory( stlChar       * aString,
                         stlErrorStack * aErrorStack );

stlStatus ztqGetCommandFromHistoryByNum( stlInt64   aHistoryNum,
                                         stlChar  **aCommand );

stlStatus ztqExecHistory( stlInt64        aHistoryNum,
                          stlAllocator  * aAllocator,
                          stlErrorStack * aErrorStack );

stlStatus ztqPrintHistory( stlAllocator  * aAllocator,
                           stlErrorStack * aErrorStack );

stlStatus ztqSetHistoryBufferSize( stlInt64        aHistorySize,
                                   stlErrorStack * aErrorStack );

stlStatus ztqClearHistoryBuffer( stlErrorStack * aErrorStack );

stlStatus ztqDestHistory( stlErrorStack * aErrorStack );

/** @} */

#endif /* _ZTQQUERYHISTORY_H_ */
