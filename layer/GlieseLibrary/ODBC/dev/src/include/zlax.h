/*******************************************************************************
 * zlax.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zlax.h 7308 2013-02-19 05:52:47Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZLAX_H_
#define _ZLAX_H_ 1

/**
 * @file zlax.h
 * @brief ODBC API Internal Execute Routines.
 */

/**
 * @defgroup zlax Internal Execute Routines
 * @{
 */

stlStatus zlaxExecute( zlcDbc        * aDbc,
                       zlsStmt       * aStmt,
                       stlBool       * aResultSet,
                       stlInt64      * aAffectedRowCount,
                       stlBool       * aSuccessWithInfo,
                       stlErrorStack * aErrorStack );

stlStatus zlaxExecDirect( zlcDbc               * aDbc,
                          zlsStmt              * aStmt,
                          stlChar              * aSql,
                          stlBool              * aResultSet,
                          stlBool              * aIsWithoutHoldCursor,
                          stlBool              * aIsUpdatable,
                          zllCursorSensitivity * aSensitivity,
                          stlInt64             * aAffectedRowCount,
                          stlBool              * aSuccessWithInfo,
                          stlErrorStack        * aErrorStack );

stlStatus zlaxExecute4Imp( zlcDbc             * aDbc,
                           zlsStmt            * aStmt,
                           stlUInt16            aParameterCount,
                           dtlValueBlockList ** aParameterData, 
                           stlInt64           * aAffectedRowCount,
                           stlBool            * aSuccessWithInfo,
                           stlErrorStack      * aErrorStack );

/** @} */

#endif /* _ZLAX_H_ */
