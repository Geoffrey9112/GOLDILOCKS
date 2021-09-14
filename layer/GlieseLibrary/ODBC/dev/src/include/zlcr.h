/*******************************************************************************
 * zlcr.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zlcr.h 11918 2014-04-09 08:49:01Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZLCR_H_
#define _ZLCR_H_ 1

/**
 * @file zlcr.h
 * @brief ODBC API Internal Result Routines.
 */

/**
 * @defgroup zlcr Internal Result Routines
 * @{
 */

stlStatus zlcrClose( zlcDbc        * aDbc,
                     zlsStmt       * aStmt,
                     stlErrorStack * aErrorStack );

stlStatus zlcrReallocLongVaryingMem( void          * aHandle,
                                     dtlDataValue  * aDataValue,
                                     void          * aCallbackContext,
                                     stlErrorStack * aErrorStack );

stlStatus zlcrReqFetch( zlsStmt       * aStmt,
                        stlErrorStack * aErrorStack );

stlStatus zlcrResFetch( zlsStmt       * aStmt,
                        stlErrorStack * aErrorStack );

stlStatus zlcrFetch( zlcDbc        * aDbc,
                     zlsStmt       * aStmt,
                     stlUInt64       aArraySize,
                     stlInt64        aOffset,
                     stlBool       * aNotFound,
                     stlBool       * aSuccessWithInfo,
                     stlErrorStack * aErrorStack );

stlStatus zlcrSetCursorName( zlcDbc        * aDbc,
                             zlsStmt       * aStmt,
                             stlChar       * aName,
                             stlErrorStack * aErrorStack );

stlStatus zlcrGetCursorName( zlcDbc        * aDbc,
                             zlsStmt       * aStmt,
                             stlErrorStack * aErrorStack );

stlStatus zlcrGetData( zlcDbc        * aDbc,
                       zlsStmt       * aStmt,
                       zlrResult     * aResult,
                       stlBool       * aEOF,
                       stlErrorStack * aErrorStack );

stlStatus zlcrFetch4Imp( zlcDbc             * aDbc,
                         zlsStmt            * aStmt,
                         dtlValueBlockList ** aDataPtr,
                         stlBool            * aNotFound,
                         stlErrorStack      * aErrorStack );

stlStatus zlcrSetPos( zlcDbc        * aDbc,
                      zlsStmt       * aStmt,
                      stlInt64        aRowNumber,
                      stlInt16        aOperation,
                      stlUInt16       aLockType,
                      stlErrorStack * aErrorStack );

/** @} */

#endif /* _ZLCR_H_ */
