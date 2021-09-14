/*******************************************************************************
 * zlymPythonWrapper.h
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

#ifndef _ZLYMPYTHONWRAPPER_H_
#define _ZLYMPYTHONWRAPPER_H_ 1

/**
 * @file zlymPythonWrapper.h
 * @brief Python API wrapper for Gliese Python Database API
 */

void zlymPyDecRef(PyObject *aObj);
void zlymPyXDecRef(PyObject *aObj);

//#define ZLY_PY_XDECREF(op)    do { if((op) != NULL) zlymPyDecRef((PyObject *)(op)); } while (0)

#define  ZLY_ATTACH_PYOBJECT(aSelf, aNewPyObject)   \
{                                                   \
    zlymPyXDecRef(aSelf);                           \
    aSelf = aNewPyObject;                           \
}

#define  ZLY_IS_VALID_PYOBJECT(x)       ( (x) != NULL ? STL_TRUE : STL_FALSE )

#endif /* _ZLYMPYTHONWRAPPER_H_ */
