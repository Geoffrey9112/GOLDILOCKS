/*******************************************************************************
 * zlpcFetchCursor.h
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

#ifndef _ZLPCFETCHCURSOR_H_
#define _ZLPCFETCHCURSOR_H_ 1

/**
 * @file zlpcFetchCursor.h
 * @brief Gliese Embedded SQL in C precompiler SQL cursor library function header file
 */

#include <stl.h>
#include <goldilocks.h>
#include <goldilocksesql.h>
#include <zlplDef.h>

/************************************************************
 * Cursor
 ************************************************************/

/*
 * FETCH cursor
 */

stlStatus zlpcFetchCursor( zlplSqlContext  * aSqlContext,
                           zlplSqlArgs     * aSqlArgs );

stlBool zlpcCheckColumnCacheExist( zlplCursorSymbol * sCursorSymbol,
                                   stlInt32           aColumnCnt );

stlStatus zlpcAllocCursorSymbolColumn( zlplSqlContext    * aSqlContext,
                                       zlplCursorSymbol  * aCursorSymbol,
                                       stlInt32            aArraySize,
                                       stlInt32            aColumnCnt,
                                       zlplSqlHostVar    * aHostVar );

stlStatus zlpcFreeCursorSymbolColumn( zlplSqlContext    * aSqlContext,
                                      zlplCursorSymbol  * aCursorSymbol );


#endif /* _ZLPCFETCHCURSOR_H_ */
