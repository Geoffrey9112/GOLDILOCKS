/*******************************************************************************
 * zlas.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zlas.h 10757 2013-12-24 09:06:47Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZLAS_H_
#define _ZLAS_H_ 1

/**
 * @file zlas.h
 * @brief Gliese API Internal Statement Routines.
 */

/**
 * @defgroup zlas Internal Statement Routines
 * @{
 */

stlStatus zlasAlloc( zlcDbc        * aDbc,
                     zlsStmt       * aStmt,
                     stlErrorStack * aErrorStack );

stlStatus zlasFree( zlcDbc            * aDbc,
                    zlsStmt           * aStmt,
                    zllFreeStmtOption   aOption,
                    stlBool             aIsCleanup,                   
                    stlErrorStack     * aErrorStack );

stlStatus zlasGetAttr( zlcDbc           * aDbc,
                       zlsStmt          * aStmt,
                       zllStatementAttr   aAttr,
                       void             * aValue,
                       stlInt32           aBufferLen,
                       stlInt32         * aStrLen,
                       stlErrorStack    * aErrorStack );

stlStatus zlasSetAttr( zlcDbc           * aDbc,
                       zlsStmt          * aStmt,
                       zllStatementAttr   aAttr,
                       stlInt64           aIntValue,
                       stlChar          * aStrValue,
                       stlInt32           aStrLength,
                       stlErrorStack *    aErrorStack );

/** @} */

#endif /* _ZLAS_H_ */
