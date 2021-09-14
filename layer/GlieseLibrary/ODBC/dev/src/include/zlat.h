/*******************************************************************************
 * zlat.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zlat.h 7308 2013-02-19 05:52:47Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZLAT_H_
#define _ZLAT_H_ 1

/**
 * @file zlat.h
 * @brief ODBC API Internal Transaction Routines.
 */

/**
 * @defgroup zlat Internal Transaction Routines
 * @{
 */

stlStatus zlatEndTran( zlcDbc        * aDbc,
                       SQLSMALLINT     aCompletionType,
                       stlErrorStack * aErrorStack );

/** @} */

#endif /* _ZLAT_H_ */
