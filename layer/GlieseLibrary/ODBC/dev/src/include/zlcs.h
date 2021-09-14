/*******************************************************************************
 * zlcs.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zlcs.h 10757 2013-12-24 09:06:47Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZLCS_H_
#define _ZLCS_H_ 1

/**
 * @file zlcs.h
 * @brief Gliese API Internal Statement Routines.
 */

/**
 * @defgroup zlcs Internal Statement Routines
 * @{
 */

stlStatus zlcsAlloc( zlcDbc        * aDbc,
                     zlsStmt       * aStmt,
                     stlErrorStack * aErrorStack );

stlStatus zlcsFree( zlcDbc            * aDbc,
                    zlsStmt           * aStmt,
                    zllFreeStmtOption   aOption,
                    stlBool             aIsCleanup,                   
                    stlErrorStack     * aErrorStack );

stlStatus zlcsGetAttr( zlcDbc           * aDbc,
                       zlsStmt          * aStmt,
                       zllStatementAttr   aAttr,
                       void             * aValue,
                       stlInt32           aBufferLen,
                       stlInt32         * aStrLen,
                       stlErrorStack    * aErrorStack );

stlStatus zlcsSetAttr( zlcDbc           * aDbc,
                       zlsStmt          * aStmt,
                       zllStatementAttr   aAttr,
                       stlInt64           aIntValue,
                       stlChar          * aStrValue,
                       stlInt32           aStrLength,
                       stlErrorStack *    aErrorStack );

/** @} */

#endif /* _ZLCS_H_ */
