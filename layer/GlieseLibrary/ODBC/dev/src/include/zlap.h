/*******************************************************************************
 * zlap.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zlap.h 9950 2013-10-18 06:00:12Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZLAP_H_
#define _ZLAP_H_ 1

/**
 * @file zlap.h
 * @brief ODBC API Internal Prepare Routines.
 */

/**
 * @defgroup zlap Internal Prepare Routines
 * @{
 */

stlStatus zlapPrepare( zlcDbc               * aDbc,
                       zlsStmt              * aStmt,
                       stlChar              * aSql,
                       stlBool              * aResultSet,
                       stlBool              * aIsWithoutHoldCursor,
                       stlBool              * aIsUpdatable,
                       zllCursorSensitivity * aSensitivity,
                       stlBool              * aSuccessWithInfo,
                       stlErrorStack        * aErrorStack );

/** @} */

#endif /* _ZLAP_H_ */
