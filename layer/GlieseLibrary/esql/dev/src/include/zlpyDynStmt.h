/*******************************************************************************
 * zlpyDynStmt.h
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

#ifndef _ZLPYDYNSTMT_H_
#define _ZLPYDYNSTMT_H_ 1

/**
 * @file zlpyDynStmt.h
 * @brief Gliese Embedded SQL in C precompiler library internal header file.
 */

#include <stl.h>
#include <goldilocks.h>
#include <goldilocksesql.h>
#include <zlplDef.h>

/************************************************************
 * Dynamic Statement Symbol
 ************************************************************/

stlInt32 zlpyCompareDynStmtSymbolFunc( void *aKeyA, void *aKeyB );
stlUInt32 zlpyDynStmtSymbolHashFunc( void *aKey, stlUInt32 aBucketCount );


stlStatus zlpyAddDynStmtSymbol( zlplSqlContext    * aSqlContext,
                                zlplDynStmtSymbol * aDynStmtSymbol );

stlStatus zlpyLookupDynStmtSymbol( zlplSqlContext     * aSqlContext,
                                   stlChar            * aDynStmtName,
                                   zlplDynStmtSymbol ** aDynStmtSymbol );

stlStatus zlpyPrepareDynStmtSymbol( zlplSqlContext    * aSqlContext,
                                    zlplSqlArgs       * aSqlArgs,
                                    zlplDynStmtSymbol * aDynStmtSymbol );

#endif /* _ZLPYDYNSTMT_H_ */
