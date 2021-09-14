/*******************************************************************************
 * zllExtendedDescribeParam.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZLL_EXTENDED_DESCRIBE_PARAM_H_
#define _ZLL_EXTENDED_DESCRIBE_PARAM_H_ 1

/**
 * @file zllExtendedDescribeParam.h
 * @brief Gliese API Internal Extended DescribeParam Routines.
 */

STL_BEGIN_DECLS

/**
 * @defgroup zllExtendedDescribeParam Internal Extended DescribeParam Routines
 * @{
 */

SQLRETURN SQL_API zllExtendedDescribeParam( SQLHSTMT       aStatementHandle,
                                            SQLUSMALLINT   aParameterNumber,
                                            SQLSMALLINT  * aInputOutputType,
                                            SQLSMALLINT  * aDataTypePtr,
                                            SQLULEN      * aParameterSizePtr,
                                            SQLSMALLINT  * aDecimalDigitsPtr,
                                            SQLSMALLINT  * aNullablePtr );


/** @} */

STL_END_DECLS

#endif /* _ZLL_EXTENDED_DESCRIBE_PARAM_H_ */
