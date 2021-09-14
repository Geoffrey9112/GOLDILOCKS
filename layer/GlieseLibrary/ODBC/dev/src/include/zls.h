/*******************************************************************************
 * zls.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zls.h 10757 2013-12-24 09:06:47Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZLS_H_
#define _ZLS_H_ 1

/**
 * @file zls.h
 * @brief Gliese API Internal Statement Routines.
 */

/**
 * @defgroup zls Internal Statement Routines
 * @{
 */

stlStatus zlsAlloc( zlcDbc         * aDbc,
                    void          ** aStmt,
                    stlErrorStack  * aErrorStack );

stlStatus zlsFree( zlsStmt       * aStmt,
                   stlBool         aIsCleanup,
                   stlErrorStack * aErrorStack );

stlStatus zlsClear( zlsStmt       * aStmt,
                    stlUInt16       aOption,
                    stlErrorStack * aErrorStack );

stlStatus zlsGetAttr( zlsStmt       * aStmt,
                      stlInt32        aAttr,
                      void          * aValue,
                      stlInt32        aBufferLen,
                      stlInt32      * aStrLen,
                      stlErrorStack * aErrorStack );

stlStatus zlsGetAttrW( zlsStmt       * aStmt,
                       stlInt32        aAttr,
                       void          * aValue,
                       stlInt32        aBufferLen,
                       stlInt32      * aStrLen,
                       stlErrorStack * aErrorStack );


stlStatus zlsSetAttr( zlsStmt       * aStmt,
                      stlInt32        aAttr,
                      void          * aValue,
                      stlInt32        aStrLen,
                      stlBool       * aSuccessWithInfo,
                      stlErrorStack * aErrorStack );

/** @} */

#endif /* _ZLS_H_ */
