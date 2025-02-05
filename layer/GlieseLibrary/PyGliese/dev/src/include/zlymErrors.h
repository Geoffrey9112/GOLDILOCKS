/*******************************************************************************
 * zlymErrors.h
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

#ifndef _ZLYMERRORS_H_
#define _ZLYMERRORS_H_ 1

#include <zlyDef.h>
#include <zlymDbg.h>

/**
 * @file zlymErrors.h
 * @brief Python exception handler for Gliese Python Database API
 */

// Sets an exception based on the ODBC SQLSTATE and error message and returns zero.  If either handle is not available,
// pass SQL_NULL_HANDLE.
//
// szFunction
//   The name of the function that failed.  Python generates a useful stack trace, but we often don't know where in the
//   C++ code we failed.
//
PyObject* RaiseErrorFromHandle(const stlChar* szFunction, HDBC hdbc, HSTMT hstmt);

// Sets an exception using a printf-like error message.
//
// szSqlState
//   The optional SQLSTATE reported by ODBC.  If not provided (sqlstate is NULL or sqlstate[0] is NULL), "HY000"
//   (General Error) is used.  Note that HY000 causes Error to be used if exc_class is not provided.
//
// exc_class
//   The optional exception class (DatabaseError, etc.) to construct.  If NULL, the appropriate class will be
//   determined from the SQLSTATE.
//
PyObject* RaiseErrorV(const stlChar* sqlstate, PyObject* exc_class, const stlChar* format, ...);


// Constructs an exception and returns it.
//
// This function is like RaiseErrorFromHandle, but gives you the ability to examine the error first (in particular,
// used to examine the SQLSTATE using HasSqlState).  If you want to use the error, call PyErr_SetObject(ex->ob_type,
// ex).  Otherwise, dispose of the error using Py_DECREF(ex).
//
// szFunction
//   The name of the function that failed.  Python generates a useful stack trace, but we often don't know where in the
//   C++ code we failed.
//
PyObject* GetErrorFromHandle(const stlChar* szFunction, HDBC hdbc, HSTMT hstmt);


// Returns true if `ex` is a database exception with SQLSTATE `szSqlState`.  Returns false otherwise.
// 
// It is safe to call with ex set to zero.  The SQLSTATE comparison is case-insensitive.
//
stlBool HasSqlStateByPyObject(PyObject* ex, const stlChar* szSqlState);


// Returns true if the HSTMT has a diagnostic record with the given SQLSTATE.  This is used after SQLGetData call that
// returned SQL_SUCCESS_WITH_INFO to see if it also has SQLSTATE 01004, indicating there is more data.
//
stlBool HasSqlStateByHandle(HSTMT hstmt, const stlChar* szSqlState);

PyObject *RaiseErrorFromException(PyObject* pError);

#endif /* _ZLYMERRORS_H_ */
