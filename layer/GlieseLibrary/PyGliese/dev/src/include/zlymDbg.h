/*******************************************************************************
 * zlymDbg.h
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

#ifndef _ZLYMDBG_H_
#define _ZLYMDBG_H_ 1

/**
 * @file zlymDbg.h
 * @brief Debug code for Gliese Python Database API
 */

#include <zlyDef.h>
#include <zlymMisc.h>

void PrintBytes(void* p, size_t len);
void DebugTrace(const char* szFmt, ...);

#ifdef PYODBC_LEAK_CHECK
#define pyodbc_malloc(len) _pyodbc_malloc(__FILE__, __LINE__, len)
void* _pyodbc_malloc(const char* filename, int lineno, size_t len);
void pyodbc_free(void* p);
void pyodbc_leak_check();
#else
#define pyodbc_malloc malloc
#define pyodbc_free free
#endif

#endif /* _ZLYMDBG_H_ */
