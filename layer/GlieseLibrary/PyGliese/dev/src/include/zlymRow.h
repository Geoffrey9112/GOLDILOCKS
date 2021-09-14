/*******************************************************************************
 * zlymRow.h
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

#ifndef _ZLYMROW_H_
#define _ZLYMROW_H_ 1

/**
 * @file zlymRow.h
 * @brief Python Row item for Gliese Python Database API
 */

#include <zlyDef.h>

/*
 * Used to make a new row from an array of column values.
 */
zlyRow* Row_New( PyObject    *description,
                 PyObject    *map_name_to_index,
                 Py_ssize_t   cValues,
                 PyObject   **apValues);

/*
 * Dereferences each object in apValues and frees apValue.  This is the internal format used by rows.
 *
 * cValues: The number of items to free in apValues.
 *
 * apValues: The array of values.  This can be NULL.
 */
void FreeRowValues(Py_ssize_t cValues, PyObject** apValues);

#define Row_Check(op) PyObject_TypeCheck(op, &RowType)
#define Row_CheckExact(op) (Py_TYPE(op) == &RowType)

#endif /* _ZLYMROW_H_ */
