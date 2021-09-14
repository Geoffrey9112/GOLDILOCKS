/*******************************************************************************
 * ztdExecThread.h
 *
 * Copyright (c) 2012, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztdExecThread.h
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZTDEXECTHREAD_H_
#define _ZTDEXECTHREAD_H_ 1

/**
 * @file ztdExecThread.h
 * @brief ExecThread  Definition
 */

/**
 * @defgroup ztdExecThread ExecThread
 * @ingroup ztd
 * @{
 */

/**
 *@addtogroup csv mode
 * @{
 */
stlStatus ztdImportTextData( SQLHENV              aEnvHandle,
                             SQLHDBC              aDbcHandle,
                             stlAllocator       * aAllocator,
                             ztdControlInfo     * aControlInfo,
                             ztdFileAndBuffer   * aFileAndBuffer,
                             ztdInputArguments  * aInputArguments,
                             stlErrorStack      * aErrorStack );

stlStatus ztdConstructQueue( stlAllocator  * aAllocator,
                             ztdQueueInfo  * aQueue,
                             stlInt64        aChunkSize,
                             stlErrorStack * aErrorStack );

stlStatus ztdDestructQueue( ztdQueueInfo  * aQueue,
                            stlErrorStack * aErrorStack );

stlStatus ztdPassChunkByRoundRobin( ztdChunkItem  * aChunkItem,
                                    stlInt64        aThreadCnt,
                                    stlInt64      * aQueueNo,
                                    ztdQueueInfo  * aQueue,
                                    stlErrorStack * aErrorStack );

stlStatus ztdSetChunkToQueue( ztdChunkItem  * aChunkItem,
                              ztdQueueInfo  * aQueue,
                              stlErrorStack * aErrorStack );

stlStatus ztdGetChunkFromQueue( ztdChunkItem  * aChunkItem,
                                ztdQueueInfo  * aQueue,
                                stlErrorStack * aErrorStack );

stlStatus ztdDequeue( ztdQueueNode  ** aFront,
                      ztdChunkItem  *  aItem,
                      stlErrorStack *  aErrorStack );

stlStatus ztdEnqueue( ztdQueueNode  ** aRear,
                      ztdChunkItem  *  aItem,
                      stlErrorStack *  aErrorStack );

stlStatus ztdParseDataToChunk( stlInt64            aArraySize,
                               stlChar          ** aReadBuffer,
                               ztdChunkItem      * aChunkItem,
                               ztdControlInfo    * aControlInfo,
                               ztdParseInfo      * aParseInfo,
                               ztdDataManager    * aDataManager,
                               stlErrorStack     * aErrorSTack );

stlStatus ztdParseDataToRecord( stlChar          ** aReadBuffer,
                                ztdChunkItem      * aChunkItem,
                                ztdControlInfo    * aControlInfo,
                                ztdParseInfo      * aParseInfo,
                                ztdDataManager    * aDataManager,
                                stlErrorStack     * aErrorStack );

stlStatus ztdParseDataToColumnWithoutOption( stlChar          ** aReadBuffer,
                                             ztdChunkItem      * aChunkItem,
                                             ztdControlInfo    * aControlInfo,
                                             ztdParseInfo      * aParseInfo,
                                             ztdDataManager    * aDataManager,
                                             stlBool           * aCompleteRecord,
                                             stlErrorStack     * aErrorStack );

stlStatus ztdParseDataToColumnWithOption( stlChar          ** aReadBuffer,
                                          ztdChunkItem      * aChunkItem,
                                          ztdControlInfo    * aControlInfo,
                                          ztdParseInfo      * aParseInfo,
                                          ztdDataManager    * aDataManager,
                                          stlBool           * aCompleteRecord,
                                          stlErrorStack     * aErrorStack );

stlStatus ztdInitializeDataManager( stlAllocator     * aAllocator,
                                    ztdDataManager   * aDataManager,
                                    stlErrorStack    * aErrorStack );

stlStatus ztdTerminateDataManager( ztdDataManager * aDataManager,
                                   stlErrorStack  * aErrorStack );

stlStatus ztdRequestData( stlChar        ** aReadBuffer,
                          ztdDataManager  * aDataManager,
                          ztdParseInfo    * aParseInfo,
                          stlErrorStack   * aErrorStack );

void * STL_THREAD_FUNC ztdReadDataFileByThread( stlThread * aThread,
                                                void      * aArg );

stlStatus ztdFillData( ztdDataManager   * aDataManager,
                       ztdFileAndBuffer * aFileAndBuffer,
                       stlErrorStack    * aErrorStack );

/** @} */

/** @} */

#endif /* _ZTDEXECTHREAD_H_ */
