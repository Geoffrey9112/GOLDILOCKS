/*******************************************************************************
 * zlymGetData.h
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

#ifndef _ZLYMGETDATA_H_
#define _ZLYMGETDATA_H_ 1

/**
 * @file zlymGetData.h
 * @brief Python Get Data functions for Gliese Python Database API
 */

#include <zlyDef.h>

void GetData_init();

PyObject* GetData(zlyCursor* cur, Py_ssize_t iCol);

stlStatus zlymInitDataBuffer( zlyDataBuffer *aDataBuffer,
                              SQLSMALLINT   dataType,
                              stlChar *stackBuffer,
                              SQLLEN stackBufferSize );
stlStatus zlymFiniDataBuffer( zlyDataBuffer *aDataBuffer );
stlChar* zlymGetBuffer( zlyDataBuffer *aDataBuffer );
SQLLEN zlymGetBufferRemainingSize( zlyDataBuffer *aDataBuffer );
void zlymAddUsedBufferSize( zlyDataBuffer *aDataBuffer, SQLLEN cbRead );
stlBool zlymAllocateMoreDataBuffer( zlyDataBuffer *aDataBuffer, SQLLEN cbAdd );
PyObject* zlymDetachValueFromDataBuffer( zlyDataBuffer *aDataBuffer );
PyObject* GetDataString(zlyCursor* cur, Py_ssize_t iCol);
PyObject* GetDataUser(zlyCursor* cur, Py_ssize_t iCol, stlInt32 conv);
PyObject* GetDataBuffer(zlyCursor* cur, Py_ssize_t iCol);
PyObject* GetDataDecimal(zlyCursor* cur, Py_ssize_t iCol);
PyObject* GetDataBit(zlyCursor* cur, Py_ssize_t iCol);
PyObject* GetDataLong(zlyCursor* cur, Py_ssize_t iCol);
PyObject* GetDataLongLong(zlyCursor* cur, Py_ssize_t iCol);
PyObject* GetDataDouble(zlyCursor* cur, Py_ssize_t iCol);
PyObject* GetDataTimestamp(zlyCursor* cur, Py_ssize_t iCol);


/**
 * If this sql type has a user-defined conversion, the index into the connection's `conv_funcs` array is returned.
 * Otherwise -1 is returned.
 */
stlInt32 GetUserConvIndex(zlyCursor* cur, SQLSMALLINT sql_type);

#endif /* _ZLYMGETDATA_H_ */

