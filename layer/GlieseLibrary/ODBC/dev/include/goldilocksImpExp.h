/*******************************************************************************
 * zllImpExp.h
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

#ifndef _GOLDILOCKSIMPEXP_H_
#define _GOLDILOCKSIMPEXP_H_ 1

/**
 * @file goldilocksImpExp.h
 * @brief Gliese API Internal Import/Export Routines.
 */

STL_BEGIN_DECLS

/**
 * @defgroup zllImpExp Internal Import/Export Routines
 * @{
 */

SQLRETURN SQL_API zllExecute4Imp( SQLHSTMT             aStatementHandle,
                                  SQLUSMALLINT         aParameterCount,
                                  dtlValueBlockList ** aParameterDataPtr );

SQLRETURN SQL_API zllFetch4Exp( SQLHSTMT             aStatementHandle,
                                stlAllocator       * aAllocator,
                                dtlValueBlockList ** aDataPtr );

/** @} */

STL_END_DECLS

#endif /* _GOLDILOCKSIMPEXP_H_ */
