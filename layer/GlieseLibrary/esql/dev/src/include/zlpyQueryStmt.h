/*******************************************************************************
 * zlpyQueryStmt.h
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

#ifndef _ZLPYQUERYSTMT_H_
#define _ZLPYQUERYSTMT_H_ 1

/**
 * @file zlpyQueryStmt.h
 * @brief Gliese Embedded SQL in C precompiler library internal header file.
 */

#include <stl.h>
#include <goldilocks.h>
#include <goldilocksesql.h>
#include <zlplDef.h>

/************************************************************
 * Query Statement Symbol
 ************************************************************/

stlInt32 zlpyCompareSqlStmtFunc(void *aKeyA, void *aKeyB);
stlUInt32 zlpySqlStmtHashFunc(void * aKey, stlUInt32 aBucketCount);

stlStatus zlpyAddSqlStmtSymbol(zlplSqlContext      *aSqlContext,
                               zlplSqlStmtSymbol   *aSqlStmtSymbol);
stlStatus zlpyLookupSqlStmtSymbol(zlplSqlContext       *aSqlContext,
                                  zlplSqlStmtKey       *aSqlStmtKey,
                                  zlplSqlStmtSymbol   **aSqlStmtSymbol,
                                  stlBool              *aIsFound);
stlStatus zlpyDeleteSqlStmtSymbol(zlplSqlContext       *aSqlContext,
                                  zlplSqlStmtSymbol    *aSqlStmtSymbol);

#endif /* _ZLPYQUERYSTMT_H_ */
