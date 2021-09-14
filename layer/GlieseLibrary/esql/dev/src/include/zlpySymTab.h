/*******************************************************************************
 * zlpySymTab.h
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

#ifndef _ZLPYSYMTAB_H_
#define _ZLPYSYMTAB_H_ 1

/**
 * @file zlpySymTab.h
 * @brief Gliese Embedded SQL in C precompiler library internal header file.
 */

#include <stl.h>
#include <goldilocks.h>
#include <goldilocksesql.h>
#include <zlplDef.h>

/************************************************************
 * Symbol Table
 ************************************************************/

stlStatus zlpyCreateSymbolTable(zlplSqlContext *aSqlContext);
stlStatus zlpyDestroySymbolTable(zlplSqlContext *aSqlContext);

#endif /* _ZLPYSYMTAB_H_ */
