/*******************************************************************************
 * zlpxDynPrepare.h
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

#ifndef _ZLPXDYNPREPARE_H_
#define _ZLPXDYNPREPARE_H_ 1

/**
 * @file zlpxDynPrepare.h
 * @brief Gliese Embedded SQL in C precompiler SQL execution library header file.
 */

#include <stl.h>
#include <goldilocks.h>
#include <goldilocksesql.h>
#include <zlplDef.h>


/****************************************************************
 * Dynamic SQL
 ****************************************************************/

stlStatus zlpxDynPrepare( zlplSqlContext   * aSqlContext,
                          zlplSqlArgs      * aSqlArgs,
                          stlChar          * aStmtName );

stlStatus zlpxMakeDynStmtSymbol(zlplSqlContext     * aSqlContext,
                                zlplSqlArgs        * aSqlArgs,
                                stlChar            * aDynStmtName,
                                zlplDynStmtSymbol ** aDynStmtSymbol );

stlStatus zlpxPrepareDynStmtSymbol( zlplSqlContext    * aSqlContext,
                                    zlplSqlArgs       * aSqlArgs,
                                    zlplDynStmtSymbol * aDynStmtSymbol );

#endif /* _ZLPXDYNPREPARE_H_ */
