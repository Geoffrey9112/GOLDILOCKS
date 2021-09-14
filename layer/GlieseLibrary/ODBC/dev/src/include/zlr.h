/*******************************************************************************
 * zlr.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zlr.h 11918 2014-04-09 08:49:01Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZLR_H_
#define _ZLR_H_ 1

/**
 * @file zlr.h
 * @brief ODBC API Internal Result Routines.
 */

/**
 * @defgroup zlr Internal Result Routines
 * @{
 */

void zlrAddNullIndicatorError( zlsStmt       * aStmt,
                               stlInt64        aRowNumber,
                               stlInt32        aColumnNumber,
                               stlErrorStack * aErrorStack );

stlStatus zlrGetDefaultCType( zlsStmt        * aStmt,
                              dtlDataValue   * aDataValue,
                              zlrResult      * aResult,
                              zldDescElement * aArdRec,
                              stlErrorStack  * aErrorStack );

stlStatus zlrInitRowStatusArray( zlsStmt * aStmt );

stlStatus zlrCloseWithoutHold( zlcDbc        * aDbc,
                               stlErrorStack * aErrorStack );

stlStatus zlrClose( zlsStmt       * aStmt,
                    stlErrorStack * aErrorStack );


stlStatus zlrFetch( zlsStmt       * aStmt,
                    stlUInt64       aArraySize,
                    stlInt64        aOffset,
                    stlBool       * aNotFound,
                    stlBool       * aSuccessWithInfo,
                    stlErrorStack * aErrorStack );

stlStatus zlrSetCursorName( zlsStmt       * aStmt,
                            stlChar       * aName,
                            stlInt16        aNameLen,
                            stlErrorStack * aErrorStack );

stlStatus zlrGetCursorName( zlsStmt       * aStmt,
                            stlChar       * aName,
                            stlInt16        aBufferLen,
                            stlInt16      * aNameLen,
                            stlErrorStack * aErrorStack );

stlStatus zlrGetData( zlsStmt       * aStmt,
                      stlUInt16       aIndex,
                      stlInt16        aCType,
                      void          * aCBuffer,
                      stlInt64        aBufferLen,
                      SQLLEN        * aInd,
                      SQLRETURN     * aReturn,
                      stlErrorStack * aErrorStack );

stlStatus zlrFetch4Imp( zlsStmt            * aStmt,
                        dtlValueBlockList ** aDataPtr,
                        stlBool            * aNotFound,
                        stlErrorStack      * aErrorStack );

stlStatus zlrSetPos( zlsStmt       * aStmt,
                     stlInt64        aRowNumber,
                     stlInt16        aOperation,
                     stlUInt16       aLockType,
                     stlErrorStack * aErrorStack );

/** @} */

#endif /* _ZLR_H_ */
