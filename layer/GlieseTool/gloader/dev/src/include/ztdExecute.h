/*******************************************************************************
 * ztdExecute.h
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

#ifndef _ZTDEXECUTE_H_
#define _ZTDEXECUTE_H_ 1


/**
 * @file ztdExecute.h
 * @brief Execute  Definition
 */

/**
 * @defgroup ztdExecute Execute
 * @ingroup ztd
 * @{
 */

/**
 * @addtogroup csv mode
 * @{
 */

stlStatus ztdExportTextData( SQLHENV             aEnvHandle,
                             SQLHDBC             aDbcHandle,
                             stlAllocator      * aAllocator,
                             ztdInputArguments * aInputArguments,
                             ztdControlInfo    * aControlInfo,
                             ztdFileAndBuffer  * aFileAndBuffe,
                             stlErrorStack     * aErrorStack );

stlStatus ztdGetLongData( SQLHSTMT           aStmt,
                          stlInt64           aRecCount,
                          stlInt32           aIdxColumn,
                          ztdFileAndBuffer * aFileAndBuffer,
                          ztdColDataExp    * aColumnData,
                          stlErrorStack    * aErrorStack );

stlStatus ztdWriteColumnToBuffer( stlFile         * aFile,
                                  ztdRecordBuffer * aRecordBuffer,
                                  ztdColDataExp   * aColumnData,
                                  SQLSMALLINT       aDataType,
                                  ztdControlInfo  * aControlInfo,
                                  stlErrorStack   * aErrorStack );

stlStatus ztdStringAppend( stlFile         * aFile,
                           ztdRecordBuffer * aRecordBuffer,
                           stlChar         * aSource,
                           stlInt64          aSourceLen,
                           stlErrorStack   * aErrorStack );

stlStatus ztdExpandHeap( stlChar       ** aString,
                         stlInt64         aOldSize,
                         stlInt64       * aNewSize,
                         stlErrorStack  * aErrorStack );

/** @} */

/** @} */

#endif /* _ZTDEXECUTE_H_ */
