/*******************************************************************************
 * zlymPythonWrapper.c
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

#include <zlyDef.h>

/**
 * @file zlymPythonWrapper.c
 * @brief Python API wrapper for Gliese Python Database API
 */

/**
 * @addtogroup zlymPythonWrapper
 * @{
 */

/**
 * @brief Internal
 */

void zlymPyDecRef(PyObject *aObj)
{
    Py_DECREF(aObj);
}

void zlymPyXDecRef(PyObject *aObj)
{
    Py_XDECREF(aObj);
}

/**
 * @}
 */
