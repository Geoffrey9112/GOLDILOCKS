/*******************************************************************************
 * zlymCursor.h
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

#ifndef _ZLYMCURSOR_H_
#define _ZLYMCURSOR_H_ 1

/**
 * @file zlymCursor.h
 * @brief Python Object for Gliese Python Database API
 */

void Cursor_init();

zlyCursor  *Cursor_New(zlyConnection* cnxn);
PyObject   *Cursor_execute(PyObject* self, PyObject* args);

#endif /* _ZLYMCURSOR_H_ */
