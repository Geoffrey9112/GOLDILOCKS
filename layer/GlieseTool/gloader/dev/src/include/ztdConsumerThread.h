/*******************************************************************************
 * ztdConsumerThread.h
 *
 * Copyright (c) 2012, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztdConsumerThread.h
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZTDCONSUMERTHREAD_H_
#define _ZTDCONSUMERTHREAD_H_ 1

/**
 * @file ztdConsumerThread.h
 * @brief ExecThread  Definition
 */

/**
 * @defgroup ztdConsumerThread ConsumerThread
 * @ingroup ztd
 * @{
 */

void * STL_THREAD_FUNC ztdImportTextDataThread( stlThread * aThread,
                                                void      * aArg );

stlStatus ztdOpenDatabaseThread( SQLHENV         aEnv,
                                 SQLHDBC       * aDbc,
                                 SQLHSTMT      * aStmt,
                                 stlChar       * aDsn,
                                 stlChar       * aId,
                                 stlChar       * aPassword,
                                 stlChar       * aCharacterSet,
                                 stlErrorStack * aErrorStack);

stlStatus ztdCloseDatabaseThread( SQLHDBC         aDbc,
                                  SQLHSTMT        aStmt,
                                  stlBool         aIsCommit,
                                  stlErrorStack * aErrorStack );

stlStatus ztdParseChunk( stlChar           * aReadBuffer,
                         ztdFileAndBuffer  * aFileAndBuffer,
                         ztdColDataImp     * aColumnData,
                         ztdParseInfo      * aParseInfo,
                         ztdControlInfo    * aControlInfo,
                         stlErrorStack     * aErrorStack );

stlStatus ztdParseRecord( stlChar           * aReadBuffer,
                          ztdBadRecord      * aBadRecord,
                          ztdColDataImp     * aColumnData,
                          ztdParseInfo      * aParseInfo,
                          ztdControlInfo    * aControlInfo,
                          stlErrorStack     * aErrorStack );

stlStatus ztdParseColumnWithoutOption( stlChar           * aReadBuffer,
                                       ztdBadRecord      * aBadRecord,
                                       ztdColDataImp     * aColumnData,
                                       ztdParseInfo      * aParseInfo,
                                       ztdControlInfo    * aControlInfo,
                                       stlErrorStack     * aErrorStack );

stlStatus ztdParseColumnWithOption( stlChar           * aReadBuffer,
                                    ztdBadRecord      * aBadRecord,
                                    ztdColDataImp     * aColumnData,
                                    ztdParseInfo      * aParseInfo,
                                    ztdControlInfo    * aControlInfo,
                                    stlErrorStack     * aErrorStack );

stlStatus ztdManageParsingState( stlChar       * aReadBuffer,
                                 ztdParseInfo  * aParseInfo,
                                 ztdBadRecord  * aBadRecord,
                                 stlBool       * aFinish,
                                 stlErrorStack * aErrorStack );

stlStatus ztdMakeBadRecord( stlChar       * aReadBuffer,
                            ztdBadRecord  * aBadRecord,
                            ztdParseInfo  * aParseInfo,
                            stlErrorStack * aErrorStack );

stlStatus ztdDiscardOverColumn( stlChar           * aReadBuffer,
                                ztdBadRecord      * aBadRecord,
                                ztdParseInfo      * aParseInfo,
                                ztdControlInfo    * aControlInfo,
                                stlErrorStack     * aErrorStack );
/** @} */
#endif /* _ZTDCONSUMERTHREAD_H_ */
