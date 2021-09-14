/*******************************************************************************
 * ztdMain.c
 *
 * Copyright (c) 2012, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZTDBUFFEREDFILE_H_
#define _ZTDBUFFEREDFILE_H_ 1

/**
 * @file ztdBufferedFile.h
 * @brief BufferedFile Definition
 */

/**
 * @defgroup ztdBufferedFile
 * @ingroup ztd
 * @{
 */

stlStatus ztdInitializeWriteBufferedFile( ztdWriteBufferedFile * aWriteBufferedFile,
                                          ztdInputArguments    * aInputArguments,
                                          ztdControlInfo       * aControlInfo,
                                          stlAllocator         * aAllocator,
                                          stlErrorStack        * aErrorStack );

stlStatus ztdTerminateWriteBufferedFile( ztdWriteBufferedFile * aWriteBufferedFile,
                                         stlErrorStack        * aErrorStack );

stlStatus ztdInitializeReadBufferedFile( ztdReadBufferedFile  * aReadBufferedFile,
                                         ztdInputArguments    * aInputArguments,
                                         stlAllocator         * aAllocator,
                                         stlErrorStack        * aErrorStack );

stlStatus ztdTerminateReadBufferedFile( ztdReadBufferedFile  * aReadBufferedFile,
                                        stlErrorStack        * aErrorStack );

stlStatus ztdChangeBufferedFile( ztdWriteBufferedFile * aWriteBufferedFile,
                                 ztdInputArguments    * aInputArguments,
                                 stlChar              * aHeader,
                                 stlInt32               aHeaderSize,
                                 stlErrorStack        * aErrorStack );

stlStatus ztdWriteValueBlockListData( ztdWriteBufferedFile  * aWriteBufferedFile,
                                      dtlValueBlockList     * aValueBlockList,
                                      ztdInputArguments     * aInputArguments,
                                      ztdControlInfo        * aControlInfo,
                                      stlChar               * aHeader,
                                      stlInt32                aHeaderSize,
                                      stlErrorStack         * aErrorStack );

stlStatus ztdWriteDataBufferedFile( ztdWriteBufferedFile * aWriteBufferedFile,
                                    ztdInputArguments    * aInputArguments,
                                    ztdControlInfo       * aControlInfo,
                                    stlChar              * aData,
                                    stlInt32               aDataSize,
                                    stlChar              * aHeader,
                                    stlInt32               aHeaderSize,
                                    stlBool                aIsFlush,
                                    stlErrorStack        * aErrorStack );

stlStatus ztdWriteBadBufferedFile( ztdWriteBufferedFile * aWriteBufferedFile,
                                   stlChar              * aData,
                                   stlInt32               aDataSize,
                                   stlBool                aIsFlush,
                                   stlErrorStack        * aErrorStack );

stlStatus ztdOrganizeHeader( dtlValueBlockList * aValueBlockList,
                             stlChar           * aHeader,
                             stlInt32            aHeaderSize,
                             stlErrorStack     * aErrorStack );

stlStatus ztdReadHeaderBufferedFile( ztdReadBufferedFile     * aReadBufferedFile,
                                     stlChar                 * aHeader,
                                     stlInt32                  aHeaderSize,
                                     ztdColumnInfo           * aColumnInfo,
                                     stlInt8                 * aEndian,
                                     stlInt32                * aVersion,
                                     dtlCharacterSet         * aDatabaseCharacterSet,
                                     SQLSMALLINT             * aColumnCount,
                                     ztdValueBlockColumnInfo * aValueBlockColumnInfo,
                                     stlErrorStack           * aErrorStack );

stlStatus ztdReadDataBufferedFile( ztdReadBufferedFile * aReadBufferedFile,
                                   stlChar             * aReadData,
                                   stlInt32              aReadSize,
                                   stlBool             * aIsEndOfFile,
                                   stlErrorStack       * aErrorStack );

stlStatus ztdInitializeReadDoubleBuffer( ztdBinaryDoubleBuffer * aDoubleBuffer,
                                         stlInt32                aBufferSize,
                                         stlAllocator          * aAllocator,
                                         stlErrorStack         * aErrorStack );

stlStatus ztdTerminateReadDoubleBuffer( ztdBinaryDoubleBuffer * aDoubleBuffer,
                                        stlErrorStack         * aErrorStack );

void * STL_THREAD_FUNC ztdReadBinaryFile( stlThread * aThread,
                                          void      * aArg );

/** @} */

#endif /* _ZTDBUFFEREDFILE_H_ */
