/*******************************************************************************
 * zlpyCursor.h
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

#ifndef _ZLPYCURSOR_H_
#define _ZLPYCURSOR_H_ 1

/**
 * @file zlpyCursor.h
 * @brief Gliese Embedded SQL in C precompiler library internal header file.
 */

#include <stl.h>
#include <goldilocks.h>
#include <goldilocksesql.h>
#include <zlplDef.h>

/************************************************************
 * Cursor Symbol
 ************************************************************/

stlInt32 zlpyCompareCursorSymbolFunc(void *aKeyA, void *aKeyB);
stlUInt32 zlpyCursorSymbolHashFunc(void *aKey, stlUInt32 aBucketCount);

stlStatus zlpyAddCursorSymbol(zlplSqlContext   *aSqlContext,
                              zlplCursorSymbol *aCursorSymbol);

stlStatus zlpyLookupCursorSymbol( zlplSqlContext    * aSqlContext,
                                  stlChar           * aCursorName,
                                  zlplCursorSymbol ** aCursorSymbol );

#endif /* _ZLPYCURSOR_H_ */
