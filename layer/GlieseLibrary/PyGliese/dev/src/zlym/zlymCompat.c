/*******************************************************************************
 * zlymCompat.c
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

#include <zlymCompat.h>
#include <zlymPythonWrapper.h>

/**
 * @file zlymCompat.c
 * @brief Python Module's compatibility functions
 */

/**
 * @addtogroup zlymCompat
 * @{
 */

/**
 * @brief Internal
 */

#if PY_MAJOR_VERSION >= 3
void PyString_ConcatAndDel(PyObject** lhs, PyObject* rhs)
{
    PyUnicode_Concat(*lhs, rhs);
    zlymPyDecRef(rhs);
}
#endif

stlBool Text_EqualsI(PyObject* lhs, const stlChar* rhs)
{
    Py_ssize_t  cchLHS;
    Py_ssize_t  cchRHS;
    Py_UNICODE *p;
    Py_ssize_t  i = 0;
    stlInt32    chL;
    stlInt32    chR;

#if PY_MAJOR_VERSION < 3
    // In Python 2, allow ANSI strings.
    if (lhs && PyString_Check(lhs))
        return _strcmpi(PyString_AS_STRING(lhs), rhs) == 0;
#endif

    if (lhs == 0 || !PyUnicode_Check(lhs))
        return STL_FALSE;

    cchLHS = PyUnicode_GET_SIZE(lhs);
    cchRHS = (Py_ssize_t)strlen(rhs);
    if (cchLHS != cchRHS)
        return STL_FALSE;

    p = PyUnicode_AS_UNICODE(lhs);
    for (i = 0; i < cchLHS; i++)
    {
        chL = (stlInt32)Py_UNICODE_TOUPPER(p[i]);
        chR = (stlInt32)toupper(rhs[i]);
        if (chL != chR)
            return STL_FALSE;
    }

    return STL_TRUE;
}

PyObject* Text_New(Py_ssize_t length)
{
    // Returns a new, uninitialized String (Python 2) or Unicode object (Python 3) object.
#if PY_MAJOR_VERSION < 3
    return PyString_FromStringAndSize(0, length);
#else
    return PyUnicode_FromUnicode(0, length);
#endif
}

TEXT_T* Text_Buffer(PyObject* o)
{
#if PY_MAJOR_VERSION < 3
    I(PyString_Check(o));
    return PyString_AS_STRING(o);
#else
    I(PyUnicode_Check(o));
    return PyUnicode_AS_UNICODE(o);
#endif
}

stlBool Text_Check(PyObject* o)
{
    // A compatibility function that determines if the object is a string, based on the version of Python.
    // For Python 2, an ASCII or Unicode string is allowed.  For Python 3, it must be a Unicode object.
#if PY_MAJOR_VERSION < 3
    if (o && PyString_Check(o))
        return STL_TRUE;
#endif
    return o && PyUnicode_Check(o);
}

Py_ssize_t Text_Size(PyObject* o)
{
#if PY_MAJOR_VERSION < 3
    if (o && PyString_Check(o))
        return PyString_GET_SIZE(o);
#endif
    return (o && PyUnicode_Check(o)) ? PyUnicode_GET_SIZE(o) : 0;
}

Py_ssize_t TextCopyToUnicode(Py_UNICODE* buffer, PyObject* o)
{
    // Copies a String or Unicode object to a Unicode buffer and returns the number of characters copied.
    // No NULL terminator is appended!

#if PY_MAJOR_VERSION < 3
    if (PyBytes_Check(o))
    {
        Py_ssize_t i = 0;
        const Py_ssize_t cch = PyBytes_GET_SIZE(o);
        const stlChar * pch = PyBytes_AS_STRING(o);

        for (i = 0; i < cch; i++)
            *buffer++ = (Py_UNICODE)*pch++;
        return cch;
    }
    else
    {
#endif
        Py_ssize_t cch = PyUnicode_GET_SIZE(o);
        memcpy(buffer, PyUnicode_AS_UNICODE(o), cch * sizeof(Py_UNICODE));
        return cch;
#if PY_MAJOR_VERSION < 3
    }
#endif
}

/**
 * @}
 */
