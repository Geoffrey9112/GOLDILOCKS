/*******************************************************************************
 * zlcx.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zlcx.h 7308 2013-02-19 05:52:47Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZLCX_H_
#define _ZLCX_H_ 1

/**
 * @file zlcx.h
 * @brief ODBC API Internal Execute Routines.
 */

/**
 * @defgroup zlcx Internal Execute Routines
 * @{
 */

stlStatus zlcxExecute( zlcDbc        * aDbc,
                       zlsStmt       * aStmt,
                       stlBool       * aResultSet,
                       stlInt64      * aAffectedRowCount,
                       stlBool       * aSuccessWithInfo,
                       stlErrorStack * aErrorStack );

stlStatus zlcxExecDirect( zlcDbc               * aDbc,
                          zlsStmt              * aStmt,
                          stlChar              * aSql,
                          stlBool              * aResultSet,
                          stlBool              * aIsWithoutHoldCursor,
                          stlBool              * aIsUpdatable,
                          zllCursorSensitivity * aSensitivity,
                          stlInt64             * aAffectedRowCount,
                          stlBool              * aSuccessWithInfo,
                          stlErrorStack        * aErrorStack );

stlStatus zlcxExecute4Imp( zlcDbc             * aDbc,
                           zlsStmt            * aStmt,
                           stlUInt16            aParameterCount,
                           dtlValueBlockList ** aParameterData, 
                           stlInt64           * aAffectedRowCount,
                           stlBool            * aSuccessWithInfo,
                           stlErrorStack      * aErrorStack );

/** @} */

#endif /* _ZLCX_H_ */
