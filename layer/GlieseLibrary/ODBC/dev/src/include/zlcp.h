/*******************************************************************************
 * zlcp.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zlcp.h 9950 2013-10-18 06:00:12Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZLCP_H_
#define _ZLCP_H_ 1

/**
 * @file zlcp.h
 * @brief ODBC API Internal Prepare Routines.
 */

/**
 * @defgroup zlcp Internal Prepare Routines
 * @{
 */

stlStatus zlcpPrepare( zlcDbc               * aDbc,
                       zlsStmt              * aStmt,
                       stlChar              * aSql,
                       stlBool              * aResultSet,
                       stlBool              * aIsWithoutHoldCursor,
                       stlBool              * aIsUpdatable,
                       zllCursorSensitivity * aSensitivity,
                       stlBool              * aSuccessWithInfo,
                       stlErrorStack        * aErrorStack );

/** @} */

#endif /* _ZLCP_H_ */
