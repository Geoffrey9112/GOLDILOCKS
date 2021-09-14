/*******************************************************************************
 * zlx.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zlx.h 7308 2013-02-19 05:52:47Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZLX_H_
#define _ZLX_H_ 1

/**
 * @file zlx.h
 * @brief ODBC API Internal Execute Routines.
 */

/**
 * @defgroup zlx Internal Execute Routines
 * @{
 */

stlStatus zlxExecute( zlsStmt       * aStmt,
                      stlInt64      * aAffectedRowCount,
                      stlBool       * aSuccessWithInfo,
                      stlErrorStack * aErrorStack );

stlStatus zlxExecDirect( zlsStmt       * aStmt,
                         stlChar       * aSql,
                         stlInt64      * aAffectedRowCount,
                         stlBool       * aSuccessWithInfo,
                         stlErrorStack * aErrorStack );

stlStatus zlxExecute4Imp( zlsStmt            * aStmt,
                          stlUInt16            aParameterCount,
                          dtlValueBlockList ** aParameterData, 
                          stlInt64           * aAffectedRowCount,
                          stlBool            * aSuccessWithInfo,
                          stlErrorStack      * aErrorStack );
/** @} */

#endif /* _ZLX_H_ */
