/*******************************************************************************
 * zlt.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zlt.h 7308 2013-02-19 05:52:47Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZLT_H_
#define _ZLT_H_ 1

/**
 * @file zlt.h
 * @brief ODBC API Internal Transaction Routines.
 */

/**
 * @defgroup zlt Internal Transaction Routines
 * @{
 */

stlStatus zltEndTran( zlcDbc        * aDbc,
                      SQLSMALLINT     aCompletionType,
                      stlErrorStack * aErrorStack );

/** @} */

#endif /* _ZLT_H_ */
