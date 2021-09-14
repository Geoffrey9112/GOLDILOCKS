/*******************************************************************************
 * zllDbcNameHash.h
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

#ifndef _ZLLDBCNAMEHASH_H_
#define _ZLLDBCNAMEHASH_H_ 1

/**
 * @file zllDbcNameHash.h
 * @brief Gliese API Internal Connect Routines.
 */

/**
 * @defgroup zll Internal Connect Routines
 * @{
 */

stlStatus zllInitializeDbcHash( stlErrorStack *aErrorStack );

stlStatus zllFinalizeDbcHash( stlErrorStack *aErrorStack );

stlStatus SQL_API zllAddDbcToNameHash( void          *aDbc,
                                       stlErrorStack *aErrorStack );

stlStatus SQL_API zllRemoveDbcFromNameHash( void          *aDbc,
                                            stlErrorStack *aErrorStack );

stlStatus SQL_API zllFindDbcFromNameHash( void          **aDbc,
                                          stlChar        *aConnName,
                                          stlErrorStack  *aErrorStack );

stlStatus SQL_API zllSetDbcName( void          *aDbc,
                                 stlChar       *aDbcName,
                                 stlErrorStack *aErrorStack );

/** @} */

#endif /* _ZLLDBCNAMEHASH_H_ */
