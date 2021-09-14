/*******************************************************************************
 * zlymMisc.c
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

/**
 * @file zlymMisc.c
 * @brief Miscellaneous functions for Gliese Python Database API
 */

#include <zlymMisc.h>
#include <zlymPythonWrapper.h>

/**
 * @addtogroup zlymMisc
 * @{
 */

/**
 * @brief Internal
 */

stlBool IsSet(DWORD grf, DWORD flags)
{
    return (grf & flags) == flags;
}

void UNUSED(void *aDummy, ...)
{
}

void _strlwr(char* name)
{
    while( *name )
    {
        *name = tolower(*name);
        name++;
    }
}

stlBool PyDecimal_Check(PyObject* p)
{
    return Py_TYPE(p) == (PyTypeObject *)decimal_type;
}

stlBool lowercase()
{
    return PyObject_GetAttrString(pModule, "lowercase") == Py_True;
}

Py_ssize_t SQLWCHAR_SIZE = sizeof(SQLWCHAR);

#ifdef HAVE_WCHAR_H
static int WCHAR_T_SIZE  = sizeof(wchar_t);
#endif

Py_UNICODE CalculateMaxSQL()
{
    Py_UNICODE m = 0;
    unsigned int i = 0;

    if (SQLWCHAR_SIZE >= Py_UNICODE_SIZE)
        return 0;

    for (i = 0; i < sizeof(SQLWCHAR); i++)
    {
        m <<= 8;
        m |= 0xFF;
    }
    return m;
}

static stlBool sqlwchar_copy(SQLWCHAR* pdest, const Py_UNICODE* psrc, Py_ssize_t len)
{
    // Copies a Python Unicode string to a SQLWCHAR buffer.  Note that this does copy the NULL terminator, but `len`
    // should not include it.  That is, it copies (len + 1) characters.

    // If SQLWCHAR is larger than Py_UNICODE, this is the largest value that can be held in a Py_UNICODE.  Because it is
    // stored in a Py_UNICODE, it is undefined when sizeof(SQLWCHAR) <= sizeof(Py_UNICODE).
    int i = 0;
    Py_UNICODE MAX_SQLWCHAR = CalculateMaxSQL();

    if (Py_UNICODE_SIZE == SQLWCHAR_SIZE)
    {
        memcpy(pdest, psrc, sizeof(SQLWCHAR) * (len + 1));
    }
    else
    {
        if (SQLWCHAR_SIZE < Py_UNICODE_SIZE)
        {
            for (i = 0; i < len; i++)
            {
                if ((Py_ssize_t)psrc[i] > MAX_SQLWCHAR)
                {
                    PyErr_Format(PyExc_ValueError, "Cannot convert from Unicode %zd to SQLWCHAR.  Value is too large.", (Py_ssize_t)psrc[i]);
                    return STL_FALSE;
                }
            }
        }

        for (i = 0; i <= len; i++) // ('<=' to include the NULL)
            pdest[i] = (SQLWCHAR)psrc[i];
    }

    return STL_TRUE;
}

SQLWCHAR* SQLWCHAR_FromUnicode(const Py_UNICODE* pch, Py_ssize_t len)
{
    SQLWCHAR* p = (SQLWCHAR*)pyodbc_malloc(sizeof(SQLWCHAR) * (len+1));
    if (p != 0)
    {
        if (!sqlwchar_copy(p, pch, len))
        {
            pyodbc_free(p);
            p = 0;
        }
    }
    return p;
}

PyObject* PyUnicode_FromSQLWCHAR(const SQLWCHAR* sz, Py_ssize_t cch)
{
    // Create a Python Unicode object from a zero-terminated SQLWCHAR.
    // If SQLWCHAR is larger than Py_UNICODE, this is the largest value that can be held in a Py_UNICODE.  Because it is
    // stored in a Py_UNICODE, it is undefined when sizeof(SQLWCHAR) <= sizeof(Py_UNICODE).
    const SQLWCHAR MAX_PY_UNICODE = (SQLWCHAR)PyUnicode_GetMax();
    PyObject    *result;
    Py_UNICODE  *pch;
    Py_ssize_t   i = 0;

    if (SQLWCHAR_SIZE == Py_UNICODE_SIZE)
    {
        // The ODBC Unicode and Python Unicode types are the same size.  Cast the ODBC type to the Python type and use
        // a fast function.
        return PyUnicode_FromUnicode((const Py_UNICODE*)sz, cch);
    }

#ifdef HAVE_WCHAR_H
    if (WCHAR_T_SIZE == SQLWCHAR_SIZE)
    {
        // The ODBC Unicode is the same as wchar_t.  Python provides a function for that.
        return PyUnicode_FromWideChar((const wchar_t*)sz, cch);
    }
#endif

    // There is no conversion, so we will copy it ourselves with a simple cast.

    if (Py_UNICODE_SIZE < SQLWCHAR_SIZE)
    {
        // We are casting from a larger size to a smaller one, so we'll make sure they all fit.

        for (i = 0; i < cch; i++)
        {
            if (((Py_ssize_t)sz[i]) > MAX_PY_UNICODE)
            {
                PyErr_Format(PyExc_ValueError, "Cannot convert from SQLWCHAR %zd to Unicode.  Value is too large.", (Py_ssize_t)sz[i]);
                return 0;
            }
        }

    }

    result = PyUnicode_FromUnicode(0, cch);
    STL_TRY( ZLY_IS_VALID_PYOBJECT( result ) == STL_TRUE );

    pch = PyUnicode_AS_UNICODE( result );
    for(i = 0; i < cch; i++)
    {
        pch[i] = (Py_UNICODE)sz[i];
    }

    return result;

    STL_FINISH;

    return NULL;
}

/**
 * @}
 */
