/*******************************************************************************
 * zlymCompat.h
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

#ifndef _ZLYMCOMPAT_H_
#define _ZLYMCOMPAT_H_ 1

#include <zlyDef.h>
#include <zlymModule.h>

/**
 * @file zlymCompat.h
 * @brief Python Object for Gliese Python Database API
 */

// Macros and functions to ease compatibility with Python 2 and Python 3.

#if PY_VERSION_HEX >= 0x03000000 && PY_VERSION_HEX < 0x03010000
#error Python 3.0 is not supported.  Please use 3.1 and higher.
#endif

// Macros introduced in 2.6, backported for 2.4 and 2.5.
#ifndef PyVarObject_HEAD_INIT
#define PyVarObject_HEAD_INIT(type, size) PyObject_HEAD_INIT(type) size,
#endif
#ifndef Py_TYPE
#define Py_TYPE(ob) (((PyObject*)(ob))->ob_type)
#endif

// Macros were introduced in 2.6 to map "bytes" to "str" in Python 2.  Back port to 2.5.
#if PY_VERSION_HEX >= 0x02060000
    #include <bytesobject.h>
#else
    #define PyBytes_AS_STRING PyString_AS_STRING
    #define PyBytes_Check PyString_Check
    #define PyBytes_CheckExact PyString_CheckExact
    #define PyBytes_FromStringAndSize PyString_FromStringAndSize
    #define PyBytes_GET_SIZE PyString_GET_SIZE
    #define PyBytes_Size PyString_Size
    #define _PyBytes_Resize _PyString_Resize
#endif

// Used for items that are ANSI in Python 2 and Unicode in Python 3 or in int 2 and long in 3.

#if PY_MAJOR_VERSION >= 3
  #define PyString_FromString PyUnicode_FromString
  #define PyString_FromStringAndSize PyUnicode_FromStringAndSize
  #define PyString_Check PyUnicode_Check
  #define PyString_Type PyUnicode_Type
  #define PyString_Size PyUnicode_Size
  #define PyInt_FromLong PyLong_FromLong
  #define PyInt_AsLong PyLong_AsLong
  #define PyInt_AS_LONG PyLong_AS_LONG
  #define PyInt_Type PyLong_Type
  #define PyString_FromFormatV PyUnicode_FromFormatV
  #define PyString_FromFormat PyUnicode_FromFormat
  #define Py_TPFLAGS_HAVE_ITER 0

  #define PyString_AsString PyUnicode_AsString

  #define TEXT_T Py_UNICODE

  #define PyString_Join PyUnicode_Join

void PyString_ConcatAndDel(PyObject** lhs, PyObject* rhs);

#else
  #include <stringobject.h>
  #include <intobject.h>
  #include <bufferobject.h>

  #define TEXT_T stlChar

  #define PyString_Join _PyString_Join

#endif

PyObject* Text_New(Py_ssize_t length);
TEXT_T* Text_Buffer(PyObject* o);
stlBool Text_Check(PyObject* o);

// Case-insensitive comparison for a Python string object (Unicode in Python 3, ASCII or Unicode in Python 2) against
// an ASCII string.  If lhs is 0 or None, false is returned.
stlBool Text_EqualsI(PyObject* lhs, const stlChar* rhs);
Py_ssize_t Text_Size(PyObject* o);
Py_ssize_t TextCopyToUnicode(Py_UNICODE* buffer, PyObject* o);

#endif /* _ZLYMCOMPAT_H_ */
