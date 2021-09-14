/*******************************************************************************
 * zlymBuffer.h
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

#ifndef _ZLYMBUFFER_H_
#define _ZLYMBUFFER_H_ 1

/**
 * @file zlymBuffer.h
 * @brief Python Parameter's buffer for Gliese Python Database API
 */

#include <zlyDef.h>

#if PY_MAJOR_VERSION < 3

// If the buffer object has a single, accessible segment, returns the length of the buffer.  If 'pp' is not NULL, the
// address of the segment is also returned.  If there is more than one segment or if it cannot be accessed, -1 is
// returned and 'pp' is not modified.
Py_ssize_t PyBuffer_GetMemory(PyObject* buffer, const stlChar** pp);

// Returns the size of a Python buffer.
//
// If an error occurs, zero is returned, but zero is a valid buffer size (I guess), so use PyErr_Occurred to determine
// if it represents a failure.
Py_ssize_t PyBuffer_Size(PyObject* self);

void zlymInitBufSegIterator(zlyBufSegIterator *aIterator, PyObject* _pBuffer);
stlBool zlymGetNextBufSegIterator(zlyBufSegIterator *aIterator, stlUInt8** pb, SQLLEN *cb);

#endif // PY_MAJOR_VERSION

#endif /* _ZLYMBUFFER_H_ */
