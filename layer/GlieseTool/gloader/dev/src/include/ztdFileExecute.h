/*******************************************************************************
 * ztdFileExecute.h
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

#ifndef _ZTDFILEEXECUTE_H_
#define _ZTDFILEEXECUTE_H_ 1


/**
 * @file ztdFileExecute.h
 * @brief Excute  Definition
 */

/**
 * @defgroup ztdFileExecute FileExecute
 * @ingroup ztd
 * @{
 */

stlStatus ztdOpenFiles( ztdFileAndBuffer  * aFileAndBuffer,
                        ztdInputArguments * aInputArguments,
                        stlErrorStack     * aErrorStack  );

stlStatus ztdCloseFiles( ztdFileAndBuffer * aFileAndBuffer,
                         stlErrorStack    * aErrorStack );

stlStatus ztdReadFile( stlChar          * aReadBuffer,
                       ztdFileAndBuffer * aFileAndBuffer,
                       stlSize          * aDataReadSize,
                       stlBool          * aNoDataFlag,
                       stlErrorStack    * aErrorStack );

stlStatus ztdWriteData( stlFile         * aFile,
                        ztdRecordBuffer * aRecordBuffer,
                        stlErrorStack   * aErrorStack );

stlStatus ztdWriteLog( ztdFileAndBuffer * aFileAndBuffer,
                       stlErrorStack    * aErrorStack );

stlStatus ztdWriteBad( ztdFileAndBuffer * aFileAndBuffer,
                       ztdBadRecord     * aBadRecord,
                       stlErrorStack    * aErrorStack );

stlStatus ztdWriteBadSync( ztdFileAndBuffer * aFileAndBuffer,
                           ztdBadRecord     * aBadRecord,
                           stlSemaphore     * aSema,
                           stlErrorStack    * aErrorStack );

stlStatus ztdCreateDataFile( stlFile           * aFileDesc,
                             ztdInputArguments * aInputArguments,
                             stlErrorStack     * aErrorStack );

/** @} */

#endif /* _ZTDFILEEXECUTE_H_ */
