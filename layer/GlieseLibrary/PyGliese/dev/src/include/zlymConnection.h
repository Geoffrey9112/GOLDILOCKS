/*******************************************************************************
 * zlymConnection.h
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

#ifndef _ZLYMCONNECTION_H_
#define _ZLYMCONNECTION_H_ 1

/**
 * @file zlymConnection.h
 * @brief Python Connection for Gliese Python Database API
 */

#include <zlyDef.h>

//extern PyTypeObject ConnectionType;
//extern PyTypeObject CnxnInfoType;

#define Connection_Check(op) PyObject_TypeCheck(op, &ConnectionType)
#define Connection_CheckExact(op) (Py_TYPE(op) == &ConnectionType)

void CnxnInfo_init();

// Looks-up or creates a CnxnInfo object for the given connection string.  The connection string can be a Unicode or
// String object.

PyObject* GetConnectionInfo(PyObject* pConnectionString, zlyConnection* cnxn);

/*
 * Used by the module's connect function to create new connection objects.  If unable to connect to the database, an
 * exception is set and zero is returned.
 */
PyObject* Connection_New( PyObject*  pConnectString,
                          stlBool    fAutoCommit,
                          stlBool    fAnsi,
                          stlBool    fUnicodeResults,
                          long       timeout,
                          stlBool    fReadOnly );

/*
 * Used by the Cursor to implement commit and rollback.
 */
PyObject* Connection_endtrans(zlyConnection* cnxn, SQLSMALLINT type);

#endif /* _ZLYMCONNECTION_H_ */
