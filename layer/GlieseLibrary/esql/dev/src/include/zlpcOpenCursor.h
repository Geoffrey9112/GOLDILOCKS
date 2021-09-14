/*******************************************************************************
 * zlpcOpenCursor.h
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

#ifndef _ZLPCOPENCURSOR_H_
#define _ZLPCOPENCURSOR_H_ 1

/**
 * @file zlpcOpenCursor.h
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
 * OPEN cursor
 */

stlStatus zlpcOpenCursor( zlplSqlContext   * aSqlContext,
                          zlplSqlArgs      * aSqlArgs );

stlStatus zlpcMakeCursorSymbol( zlplSqlContext    * aSqlContext,
                                zlplSqlArgs       * aSqlArgs,
                                zlplCursorSymbol ** aCursorSymbol );

stlStatus zlpcSetCursorProp( zlplSqlContext   * aSqlContext,
                             zlplSqlArgs      * aSqlArgs,
                             SQLHSTMT           aStmtHandle );

stlStatus zlpcGetCursorQuery( zlplSqlContext    * aSqlContext,
                              zlplSqlArgs       * aSqlArgs,
                              zlplCursorSymbol  * aCursorSymbol,
                              stlChar          ** aStringSQL,
                              stlInt32          * aStringLen );

stlStatus zlpcPrepareCursorQuery( zlplSqlContext   * aSqlContext,
                                  zlplSqlArgs      * aSqlArgs,
                                  zlplCursorSymbol * aCursorSymbol,
                                  stlChar          * aStringSQL,
                                  stlInt32           aStringLen,
                                  stlBool            aNeedSetCursorProp );

stlStatus zlpcAllocCursorSymbolParam( zlplSqlContext    * aSqlContext,
                                      zlplCursorSymbol  * aCursorSymbol,
                                      stlInt32            aArraySize,
                                      stlInt32            aParamCnt,
                                      zlplSqlHostVar    * aHostVar );

stlStatus zlpcFreeCursorSymbolParam( zlplSqlContext    * aSqlContext,
                                     zlplCursorSymbol  * aCursorSymbol );

#endif /* _ZLPCOPENCURSOR_H_ */
