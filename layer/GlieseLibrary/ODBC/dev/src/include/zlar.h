/*******************************************************************************
 * zlar.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zlar.h 11918 2014-04-09 08:49:01Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZLAR_H_
#define _ZLAR_H_ 1

/**
 * @file zlar.h
 * @brief Gliese API Internal Result Routines.
 */

/**
 * @defgroup zlar Internal Result Routines
 * @{
 */

stlStatus zlarClose( zlcDbc        * aDbc,
                     zlsStmt       * aStmt,
                     stlErrorStack * aErrorStack );

stlStatus zlarFetch( zlcDbc        * aDbc,
                     zlsStmt       * aStmt,
                     stlUInt64       aArraySize,
                     stlInt64        aOffset,
                     stlBool       * aNotFound,
                     stlBool       * aSuccessWithInfo,
                     stlErrorStack * aErrorStack );

stlStatus zlarSetCursorName( zlcDbc        * aDbc,
                             zlsStmt       * aStmt,
                             stlChar       * aName,
                             stlErrorStack * aErrorStack );

stlStatus zlarGetCursorName( zlcDbc        * aDbc,
                             zlsStmt       * aStmt,
                             stlErrorStack * aErrorStack );

stlStatus zlarGetData( zlcDbc        * aDbc,
                       zlsStmt       * aStmt,
                       zlrResult     * aResult,
                       stlBool       * aEOF,
                       stlErrorStack * aErrorStack );

stlStatus zlarFetch4Imp( zlcDbc             * aDbc,
                         zlsStmt            * aStmt,
                         dtlValueBlockList ** aDataPtr,
                         stlBool            * aNotFound,
                         stlErrorStack      * aErrorStack );

stlStatus zlarSetPos( zlcDbc        * aDbc,
                      zlsStmt       * aStmt,
                      stlInt64        aRowNumber,
                      stlInt16        aOperation,
                      stlUInt16       aLockType,
                      stlErrorStack * aErrorStack );
/** @} */

#endif /* _ZLAR_H_ */
