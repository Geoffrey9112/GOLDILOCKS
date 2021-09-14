/*******************************************************************************
 * zlct.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zlct.h 7308 2013-02-19 05:52:47Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZLCT_H_
#define _ZLCT_H_ 1

/**
 * @file zlct.h
 * @brief ODBC API Internal Transaction Routines.
 */

/**
 * @defgroup zlct Internal Transaction Routines
 * @{
 */

stlStatus zlctEndTran( zlcDbc        * aDbc,
                       SQLSMALLINT     aCompletionType,
                       stlErrorStack * aErrorStack );

/** @} */

#endif /* _ZLCT_H_ */
