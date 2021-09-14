/*******************************************************************************
 * zlymMisc.h
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

#ifndef _ZLYMMISC_H_
#define _ZLYMMISC_H_ 1

#include <zlyDef.h>

/**
 * @file zlymMisc.h
 * @brief Miscellaneous functions for Gliese Python Database API
 */

#if defined(__SUNPRO_CC) || defined(__SUNPRO_C) || (defined(__GNUC__) && !defined(__MINGW32__))
#include <alloca.h>
#define CDECL cdecl
#define min(X,Y) ((X) < (Y) ? (X) : (Y))
#define max(X,Y) ((X) > (Y) ? (X) : (Y))
#define _alloca alloca
void _strlwr(char* name);
#else
#define CDECL
#endif

stlBool IsSet(DWORD grf, DWORD flags);
void UNUSED(void *aDummy, ...);
stlBool PyDecimal_Check(PyObject* p);
stlBool lowercase();

SQLWCHAR* SQLWCHAR_FromUnicode(const Py_UNICODE* pch, Py_ssize_t len);
PyObject* PyUnicode_FromSQLWCHAR(const SQLWCHAR* sz, Py_ssize_t cch);

#endif /* _ZLYMMISC_H_ */
