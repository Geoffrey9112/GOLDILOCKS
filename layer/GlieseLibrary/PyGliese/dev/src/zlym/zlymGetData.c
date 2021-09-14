/*******************************************************************************
 * zlymGetData.c
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
 * @file zlymGetData.c
 * @brief Python Object for Gliese Python Database API
 */

/**
 * @addtogroup zlymGetData
 * @{
 */

/**
 * @brief Internal
 */


// The functions for reading a single value from the database using SQLGetData.  There is a different function for
// every data type.

#include <zlyDef.h>
#include <zlymModule.h>
#include <zlymCursor.h>
#include <zlymConnection.h>
#include <zlymErrors.h>
//#include "dbspecific.h"
//#include "sqlwchar.h"
#include <zlymPythonWrapper.h>

#include <datetime.h>

void GetData_init()
{
    PyDateTime_IMPORT;
}

stlStatus zlymInitDataBuffer( zlyDataBuffer *aDataBuffer,
                              SQLSMALLINT   dataType,
                              stlChar *stackBuffer,
                              SQLLEN stackBufferSize )
{
    // dataType
    //   The type of data we will be reading: SQL_C_CHAR, SQL_C_WCHAR, or SQL_C_BINARY.

    aDataBuffer->dataType = dataType;

    aDataBuffer->element_size = (stlInt32)((dataType == SQL_C_WCHAR)  ? sizeof(SQLWCHAR) : sizeof(char));
    aDataBuffer->null_size    = (aDataBuffer->dataType == SQL_C_BINARY) ? 0 : aDataBuffer->element_size;

    aDataBuffer->buffer        = stackBuffer;
    aDataBuffer->bufferSize    = stackBufferSize;
    aDataBuffer->usingStack    = STL_TRUE;
    aDataBuffer->bufferOwner   = NULL;
    aDataBuffer->bytesUsed     = 0;

    return STL_SUCCESS;
}

stlStatus zlymFiniDataBuffer( zlyDataBuffer *aDataBuffer )
{
    if( aDataBuffer->usingStack == STL_FALSE )
    {
        if( aDataBuffer->bufferOwner != NULL )
        {
            zlymPyDecRef( aDataBuffer->bufferOwner );
        }
        else
        {
            pyodbc_free( aDataBuffer->buffer );
        }
    }

    return STL_SUCCESS;
}

stlChar* zlymGetBuffer( zlyDataBuffer *aDataBuffer )
{
    STL_TRY( aDataBuffer->buffer != NULL );

    return aDataBuffer->buffer + aDataBuffer->bytesUsed;

    STL_FINISH;

    return NULL;
}

SQLLEN zlymGetBufferRemainingSize( zlyDataBuffer *aDataBuffer )
{
    // Returns the amount of data remaining in the buffer, ready to be passed to SQLGetData.
    return aDataBuffer->bufferSize - aDataBuffer->bytesUsed;
}

void zlymAddUsedBufferSize( zlyDataBuffer *aDataBuffer, SQLLEN cbRead )
{
    I(cbRead <= zlymGetBufferRemainingSize( aDataBuffer ));
    aDataBuffer->bytesUsed += (stlInt32)cbRead;
}

stlBool zlymAllocateMoreDataBuffer( zlyDataBuffer *aDataBuffer, SQLLEN cbAdd )
{
    // cbAdd
    //   The number of bytes (cb --> count of bytes) to add.

    SQLLEN newSize = aDataBuffer->bufferSize + cbAdd;

    if (cbAdd == 0)
    {
        STL_THROW( RAMP_EXIT_FUNC );
    }

    if (aDataBuffer->usingStack)
    {
        // This is the first call and `buffer` points to stack memory.  Allocate a new object and copy the stack
        // data into it.

        stlChar * stackBuffer = aDataBuffer->buffer;

        if( aDataBuffer->dataType == SQL_C_CHAR )
        {
            aDataBuffer->bufferOwner = PyBytes_FromStringAndSize(0, newSize);
            aDataBuffer->buffer      = aDataBuffer->bufferOwner ? PyBytes_AS_STRING(aDataBuffer->bufferOwner) : 0;
        }
        else if( aDataBuffer->dataType == SQL_C_BINARY )
        {
#if PY_VERSION_HEX >= 0x02060000
            aDataBuffer->bufferOwner = PyByteArray_FromStringAndSize(0, newSize);
            aDataBuffer->buffer      = aDataBuffer->bufferOwner ? PyByteArray_AS_STRING(aDataBuffer->bufferOwner) : 0;
#else
            aDataBuffer->bufferOwner = PyBytes_FromStringAndSize(0, newSize);
            aDataBuffer->buffer      = aDataBuffer->bufferOwner ? PyBytes_AS_STRING(aDataBuffer->bufferOwner) : 0;
#endif
        }
        else if (sizeof(SQLWCHAR) == Py_UNICODE_SIZE)
        {
            // Allocate directly into a Unicode object.
            aDataBuffer->bufferOwner = PyUnicode_FromUnicode(0, newSize / aDataBuffer->element_size);
            aDataBuffer->buffer      = aDataBuffer->bufferOwner ? (char*)PyUnicode_AsUnicode(aDataBuffer->bufferOwner) : 0;
        }
        else
        {
            // We're Unicode, but SQLWCHAR and Py_UNICODE don't match, so maintain our own SQLWCHAR buffer.
            aDataBuffer->bufferOwner = NULL;
            aDataBuffer->buffer      = (char*)pyodbc_malloc((size_t)newSize);
        }

        STL_TRY( aDataBuffer->buffer != NULL );

        aDataBuffer->usingStack = STL_FALSE;

        memcpy(aDataBuffer->buffer, stackBuffer, (size_t)aDataBuffer->bufferSize);
        aDataBuffer->bufferSize = newSize;
    }
    else
    {
        if( (aDataBuffer->bufferOwner != NULL) &&
            (PyUnicode_CheckExact(aDataBuffer->bufferOwner) != 0) )
        {
            STL_TRY(PyUnicode_Resize(&aDataBuffer->bufferOwner, newSize / aDataBuffer->element_size) != -1);
            aDataBuffer->buffer = (char*)PyUnicode_AsUnicode(aDataBuffer->bufferOwner);
        }
#if PY_VERSION_HEX >= 0x02060000
        else if( (aDataBuffer->bufferOwner != NULL) &&
                 (PyByteArray_CheckExact(aDataBuffer->bufferOwner) != 0) )
        {
            STL_TRY(PyByteArray_Resize(aDataBuffer->bufferOwner, newSize) != -1);
            aDataBuffer->buffer = PyByteArray_AS_STRING(aDataBuffer->bufferOwner);
        }
#endif
        else if( (aDataBuffer->bufferOwner != NULL) &&
                 (PyBytes_CheckExact(aDataBuffer->bufferOwner) != 0) )
        {
            STL_TRY(_PyBytes_Resize(&aDataBuffer->bufferOwner, newSize) != -1);
            aDataBuffer->buffer = PyBytes_AS_STRING(aDataBuffer->bufferOwner);
        }
        else
        {
            stlChar *tmp = (stlChar *)realloc(aDataBuffer->buffer, (size_t)newSize);
            STL_TRY( tmp != NULL );
            aDataBuffer->buffer = tmp;
        }
        aDataBuffer->bufferSize = newSize;
    }

    STL_RAMP(RAMP_EXIT_FUNC);

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

PyObject* zlymDetachValueFromDataBuffer( zlyDataBuffer *aDataBuffer )
{
    // At this point, Trim should have been called by PostRead.
    PyObject  *result = NULL;

    if (aDataBuffer->bytesUsed == SQL_NULL_DATA || aDataBuffer->buffer == 0)
    {
        Py_RETURN_NONE;
    }

    if (aDataBuffer->usingStack)
    {
        if (aDataBuffer->dataType == SQL_C_CHAR)
        {
            result = PyBytes_FromStringAndSize(aDataBuffer->buffer, aDataBuffer->bytesUsed);
        }
        else if (aDataBuffer->dataType == SQL_C_BINARY)
        {
#if PY_VERSION_HEX >= 0x02060000
            result = PyByteArray_FromStringAndSize(aDataBuffer->buffer, aDataBuffer->bytesUsed);
#else
            result = PyBytes_FromStringAndSize(aDataBuffer->buffer, aDataBuffer->bytesUsed);
#endif
        }
        else if (sizeof(SQLWCHAR) == Py_UNICODE_SIZE)
        {
            result = PyUnicode_FromUnicode((const Py_UNICODE*)aDataBuffer->buffer, aDataBuffer->bytesUsed / aDataBuffer->element_size);
        }
        else
        {
            result = PyUnicode_FromSQLWCHAR((const SQLWCHAR*)aDataBuffer->buffer, aDataBuffer->bytesUsed / aDataBuffer->element_size);
        }
    }
    else
    {
        if( (aDataBuffer->bufferOwner != NULL) &&
            (PyUnicode_CheckExact(aDataBuffer->bufferOwner) != 0) )
        {
            if (PyUnicode_Resize(&aDataBuffer->bufferOwner, aDataBuffer->bytesUsed / aDataBuffer->element_size) == -1)
                return 0;
            result = aDataBuffer->bufferOwner;
            aDataBuffer->bufferOwner = 0;
            aDataBuffer->buffer      = 0;
        }
        else if( (aDataBuffer->bufferOwner != NULL) &&
                 (PyBytes_CheckExact(aDataBuffer->bufferOwner) != 0) )
        {
            if (_PyBytes_Resize(&aDataBuffer->bufferOwner, aDataBuffer->bytesUsed) == -1)
                return 0;
            result = aDataBuffer->bufferOwner;
            aDataBuffer->bufferOwner = 0;
            aDataBuffer->buffer      = 0;
        }
#if PY_VERSION_HEX >= 0x02060000
        else if( (aDataBuffer->bufferOwner != NULL) &&
                 (PyByteArray_CheckExact(aDataBuffer->bufferOwner) != 0) )
        {
            if (PyByteArray_Resize(aDataBuffer->bufferOwner, aDataBuffer->bytesUsed) == -1)
                return 0;
            result = aDataBuffer->bufferOwner;
            aDataBuffer->bufferOwner = 0;
            aDataBuffer->buffer      = 0;
        }
#endif
        else
        {
            // We have allocated our own SQLWCHAR buffer and must now copy it to a Unicode object.
            I(aDataBuffer->bufferOwner == 0);
            result = PyUnicode_FromSQLWCHAR((const SQLWCHAR*)aDataBuffer->buffer, aDataBuffer->bytesUsed / aDataBuffer->element_size);
            STL_TRY( result != NULL );

            pyodbc_free(aDataBuffer->buffer);
            aDataBuffer->buffer = NULL;
        }
    }

    return result;

    STL_FINISH;

    return NULL;
}

static PyObject* GetDataString(zlyCursor* cur, Py_ssize_t iCol)
{
    // Returns a string, unicode, or bytearray object for character and binary data.
    //
    // In Python 2.6+, binary data is returned as a byte array.  Earlier versions will return an ASCII str object here
    // which will be wrapped in a buffer object by the caller.
    //
    // NULL terminator notes:
    //
    //  * pinfo->column_size, from SQLDescribeCol, does not include a NULL terminator.  For example, column_size for a
    //    char(10) column would be 10.  (Also, when dealing with SQLWCHAR, it is the number of *characters*, not bytes.)
    //
    //  * When passing a length to PyString_FromStringAndSize and similar Unicode functions, do not add the NULL
    //    terminator -- it will be added automatically.  See objects/stringobject.c
    //
    //  * SQLGetData does not return the NULL terminator in the length indicator.  (Therefore, you can pass this value
    //    directly to the Python string functions.)
    //
    //  * SQLGetData will write a NULL terminator in the output buffer, so you must leave room for it.  You must also
    //    include the NULL terminator in the buffer length passed to SQLGetData.
    //
    // ODBC generalization:
    //  1) Include NULL terminators in input buffer lengths.
    //  2) NULL terminators are not used in data lengths.

    zlyColumnInfo *pinfo = &cur->colinfos[iCol];
    SQLSMALLINT    nTargetType;
    stlChar        sBuffer[4096];
    zlyDataBuffer  sDataBuffer;
    stlInt32       iDbg = 0;

    // Some Unix ODBC drivers do not return the correct length.
    if (pinfo->sql_type == SQL_GUID)
        pinfo->column_size = 36;

    switch (pinfo->sql_type)
    {
        case SQL_CHAR:
        case SQL_VARCHAR:
        case SQL_LONGVARCHAR:
        case SQL_GUID:
//    case SQL_SS_XML:
#if PY_MAJOR_VERSION < 3
            if (cur->cnxn->unicode_results)
                nTargetType  = SQL_C_WCHAR;
            else
                nTargetType  = SQL_C_CHAR;
#else
            nTargetType  = SQL_C_WCHAR;
#endif

            break;

        case SQL_WCHAR:
        case SQL_WVARCHAR:
        case SQL_WLONGVARCHAR:
            nTargetType  = SQL_C_WCHAR;
            break;

        default:
            nTargetType  = SQL_C_BINARY;
            break;
    }

    STL_TRY( zlymInitDataBuffer( &sDataBuffer, nTargetType, sBuffer, sizeof(sBuffer)) == STL_SUCCESS );

    for(iDbg = 0; iDbg < 10; iDbg++) // failsafe
    {
        SQLRETURN ret;
        SQLLEN    cbData = 0;
        SQLLEN    cbBuffer;

        Py_BEGIN_ALLOW_THREADS;
        ret = SQLGetData( cur->hstmt,
                          (SQLUSMALLINT)(iCol+1),
                          nTargetType,
                          zlymGetBuffer( &sDataBuffer ),
                          zlymGetBufferRemainingSize( &sDataBuffer ),
                          &cbData );
        Py_END_ALLOW_THREADS;

        if (cbData == SQL_NULL_DATA || (ret == SQL_SUCCESS && cbData < 0))
        {
            // HACK: FreeTDS 0.91 on OS/X returns -4 for NULL data instead of SQL_NULL_DATA (-1).  I've traced into the
            // code and it appears to be the result of assigning -1 to a SQLLEN:
            //
            //   if (colinfo->column_cur_size < 0) {
            //       /* TODO check what should happen if pcbValue was NULL * /
            //       *pcbValue = SQL_NULL_DATA;
            //
            // I believe it will be fine to treat all negative values as NULL for now.

            Py_RETURN_NONE;
        }

        if (!SQL_SUCCEEDED(ret) && ret != SQL_NO_DATA)
            return RaiseErrorFromHandle("SQLGetData", cur->cnxn->hdbc, cur->hstmt);

        // The SQLGetData behavior is incredibly quirky.  It doesn't tell us the total, the total we've read, or even
        // the amount just read.  It returns the amount just read, plus any remaining.  Unfortunately, the only way to
        // pick them apart is to subtract out the amount of buffer we supplied.

        cbBuffer = zlymGetBufferRemainingSize( &sDataBuffer ); // how much we gave SQLGetData

        if (ret == SQL_SUCCESS_WITH_INFO)
        {
            // There is more data than fits in the buffer.  The amount of data equals the amount of data in the buffer
            // minus a NULL terminator.

            SQLLEN cbRead;
            SQLLEN cbMore;

            if (cbData == SQL_NO_TOTAL)
            {
                // We don't know how much more, so just guess.
                cbRead = cbBuffer - sDataBuffer.null_size;
                cbMore = 8192;
            }
            else if (cbData >= cbBuffer)
            {
                // There is more data.  We supplied cbBuffer, but there was cbData (more).  We received cbBuffer, so we
                // need to subtract that, allocate enough to read the rest (cbData-cbBuffer).

                cbRead = cbBuffer - sDataBuffer.null_size;
                cbMore = cbData - cbRead;
            }
            else
            {
                // I'm not really sure why I would be here ... I would have expected SQL_SUCCESS
                cbRead = cbData - sDataBuffer.null_size;
                cbMore = 0;
            }

            zlymAddUsedBufferSize( &sDataBuffer, cbRead );
            if( zlymAllocateMoreDataBuffer( &sDataBuffer, cbMore ) == 0 )
            {
                return PyErr_NoMemory();
            }
        }
        else if (ret == SQL_SUCCESS)
        {
            // For some reason, the NULL terminator is used in intermediate buffers but not in this final one.
            zlymAddUsedBufferSize( &sDataBuffer, cbData );
        }

        if (ret == SQL_SUCCESS || ret == SQL_NO_DATA)
            return zlymDetachValueFromDataBuffer( &sDataBuffer );
    }

    // REVIEW: Add an error message.
    return 0;

    STL_FINISH;

    return NULL;
}

static PyObject* GetDataUser(zlyCursor* cur, Py_ssize_t iCol, stlInt32 conv)
{
    // conv
    //   The index into the connection's user-defined conversions `conv_types`.
    PyObject *value = NULL;
    PyObject *result = NULL;

    value = GetDataString(cur, iCol);
    if (value == 0)
        return 0;

    result = PyObject_CallFunction(cur->cnxn->conv_funcs[conv], "(O)", value);
    zlymPyDecRef(value);
    return result;
}


#if PY_VERSION_HEX < 0x02060000
static PyObject* GetDataBuffer(zlyCursor* cur, Py_ssize_t iCol)
{
    PyObject* str = GetDataString(cur, iCol);
    PyObject* buffer = NULL;

    if (str == Py_None)
        return str;

    if (str)
    {
        buffer = PyBuffer_FromObject(str, 0, PyString_GET_SIZE(str));
        zlymPyDecRef(str);         // If no buffer, release it.  If buffer, the buffer owns it.
    }

    return buffer;
}
#endif

/*
static PyObject* GetDataDecimalWChar(zlyCursor* cur, Py_ssize_t iCol)
{
    // The SQL_NUMERIC_STRUCT support is hopeless (SQL Server ignores scale on input parameters and output columns,
    // Oracle does something else weird, and many drivers don't support it at all), so we'll rely on the Decimal's
    // string parsing.  Unfortunately, the Decimal author does not pay attention to the locale, so we have to modify
    // the string ourselves.
    //
    // Oracle inserts group separators (commas in US, periods in some countries), so leave room for that too.
    //
    // Some databases support a 'money' type which also inserts currency symbols.  Since we don't want to keep track of
    // all these, we'll ignore all characters we don't recognize.  We will look for digits, negative sign (which I hope
    // is universal), and a decimal point ('.' or ',' usually).  We'll do everything as Unicode in case currencies,
    // etc. are too far out.

    // TODO: Is Unicode a good idea for Python 2.7?  We need to know which drivers support Unicode.

    SQLWCHAR buffer[100];
    SQLLEN cbFetched = 0; // Note: will not include the NULL terminator.

    SQLRETURN  ret;
    PyObject  *str;

    stlInt32   cch;
    stlInt32   i;

    Py_BEGIN_ALLOW_THREADS;
    ret = SQLGetData(cur->hstmt, (SQLUSMALLINT)(iCol+1), SQL_C_WCHAR, buffer, sizeof(buffer), &cbFetched);
    Py_END_ALLOW_THREADS;
    if (!SQL_SUCCEEDED(ret))
        return RaiseErrorFromHandle("SQLGetData", cur->cnxn->hdbc, cur->hstmt);

    if (cbFetched == SQL_NULL_DATA)
        Py_RETURN_NONE;

    // Remove non-digits and convert the databases decimal to a '.' (required by decimal ctor).
    //
    // We are assuming that the decimal point and digits fit within the size of SQLWCHAR.

    cch = (stlInt32)(cbFetched / sizeof(SQLWCHAR));

    for(i = (cch - 1); i >= 0; i--)
    {
        if (buffer[i] == chDecimal)
        {
            // Must force it to use '.' since the Decimal class doesn't pay attention to the locale.
            buffer[i] = '.';
        }
        else if ((buffer[i] < '0' || buffer[i] > '9') && buffer[i] != '-')
        {
            memmove(&buffer[i], &buffer[i] + 1, (cch - i) * sizeof(SQLWCHAR));
            cch--;
        }
    }

    I(buffer[cch] == 0);

    str = PyUnicode_FromSQLWCHAR(buffer, cch);
    STL_TRY( ZLY_IS_VALID_PYOBJECT(str) == STL_TRUE );

    return PyObject_CallFunction(decimal_type, "O", str);

    STL_FINISH;

    return NULL;
}
*/

static PyObject* GetDataDecimal(zlyCursor* cur, Py_ssize_t iCol)
{
    // 원래의 pyodbc source는 위의 GetDateDecimalWChar 함수로 변경
    // Gliese가 현재 unicode를 지원하지 않으므로 SQL_C_WCHAR를 쓰지 않도록 수정함
    SQLCHAR  buffer[100];
    SQLLEN   cbFetched = 0; // Note: will not include the NULL terminator.

    SQLRETURN  ret;
    PyObject  *str;

    /*
    stlInt32   cch;
    stlInt32   i;
    */

    Py_BEGIN_ALLOW_THREADS;
    ret = SQLGetData(cur->hstmt, (SQLUSMALLINT)(iCol+1), SQL_C_CHAR, buffer, sizeof(buffer), &cbFetched);
    Py_END_ALLOW_THREADS;
    if (!SQL_SUCCEEDED(ret))
        return RaiseErrorFromHandle("SQLGetData", cur->cnxn->hdbc, cur->hstmt);

    if (cbFetched == SQL_NULL_DATA)
        Py_RETURN_NONE;

    // Remove non-digits and convert the databases decimal to a '.' (required by decimal ctor).
    //
    // We are assuming that the decimal point and digits fit within the size of SQLWCHAR.

    /*
    cch = (stlInt32)(cbFetched / sizeof(SQLCHAR));

    for(i = (cch - 1); i >= 0; i--)
    {
        if (buffer[i] == chDecimal)
        {
            // Must force it to use '.' since the Decimal class doesn't pay attention to the locale.
            buffer[i] = '.';
        }
        else if ((buffer[i] < '0' || buffer[i] > '9') && buffer[i] != '-')
        {
            memmove(&buffer[i], &buffer[i] + 1, (cch - i) * sizeof(SQLCHAR));
            cch--;
        }
    }

    I(buffer[cch] == 0);
    */

    str = PyUnicode_FromString((const char *)buffer);
    STL_TRY( ZLY_IS_VALID_PYOBJECT(str) == STL_TRUE );

    return PyObject_CallFunction(decimal_type, "O", str);

    STL_FINISH;

    return NULL;
}

static PyObject* GetDataBit(zlyCursor* cur, Py_ssize_t iCol)
{
    SQLCHAR ch;
    SQLLEN cbFetched;
    SQLRETURN ret;

    Py_BEGIN_ALLOW_THREADS;
    ret = SQLGetData(cur->hstmt, (SQLUSMALLINT)(iCol+1), SQL_C_BIT, &ch, sizeof(ch), &cbFetched);
    Py_END_ALLOW_THREADS;

    if (!SQL_SUCCEEDED(ret))
        return RaiseErrorFromHandle("SQLGetData", cur->cnxn->hdbc, cur->hstmt);

    if (cbFetched == SQL_NULL_DATA)
        Py_RETURN_NONE;

    if (ch == SQL_TRUE)
        Py_RETURN_TRUE;

    Py_RETURN_FALSE;
}


static PyObject* GetDataLong(zlyCursor* cur, Py_ssize_t iCol)
{
    zlyColumnInfo* pinfo = &cur->colinfos[iCol];

    SQLINTEGER value;
    SQLLEN cbFetched;
    SQLRETURN ret;

    SQLSMALLINT nCType = pinfo->is_unsigned ? SQL_C_ULONG : SQL_C_LONG;

    Py_BEGIN_ALLOW_THREADS
        ret = SQLGetData(cur->hstmt, (SQLUSMALLINT)(iCol+1), nCType, &value, sizeof(value), &cbFetched);
    Py_END_ALLOW_THREADS
        if (!SQL_SUCCEEDED(ret))
            return RaiseErrorFromHandle("SQLGetData", cur->cnxn->hdbc, cur->hstmt);

    if (cbFetched == SQL_NULL_DATA)
        Py_RETURN_NONE;

    if (pinfo->is_unsigned)
        return PyInt_FromLong(*(SQLINTEGER*)&value);

    return PyInt_FromLong(value);
}


static PyObject* GetDataLongLong(zlyCursor* cur, Py_ssize_t iCol)
{
    zlyColumnInfo* pinfo = &cur->colinfos[iCol];

    SQLSMALLINT nCType = pinfo->is_unsigned ? SQL_C_UBIGINT : SQL_C_SBIGINT;
    SQLBIGINT   value;
    SQLLEN      cbFetched;
    SQLRETURN   ret;

    Py_BEGIN_ALLOW_THREADS;
    ret = SQLGetData(cur->hstmt, (SQLUSMALLINT)(iCol+1), nCType, &value, sizeof(value), &cbFetched);
    Py_END_ALLOW_THREADS;

    if (!SQL_SUCCEEDED(ret))
        return RaiseErrorFromHandle("SQLGetData", cur->cnxn->hdbc, cur->hstmt);

    if (cbFetched == SQL_NULL_DATA)
        Py_RETURN_NONE;

    if (pinfo->is_unsigned)
        return PyLong_FromUnsignedLongLong((unsigned PY_LONG_LONG)(SQLUBIGINT)value);

    return PyLong_FromLongLong((PY_LONG_LONG)value);
}


static PyObject* GetDataDouble(zlyCursor* cur, Py_ssize_t iCol)
{
    double value;
    SQLLEN cbFetched = 0;
    SQLRETURN ret;

    Py_BEGIN_ALLOW_THREADS;

    ret = SQLGetData(cur->hstmt, (SQLUSMALLINT)(iCol+1), SQL_C_DOUBLE, &value, sizeof(value), &cbFetched);
    Py_END_ALLOW_THREADS;

    if (!SQL_SUCCEEDED(ret))
        return RaiseErrorFromHandle("SQLGetData", cur->cnxn->hdbc, cur->hstmt);

    if (cbFetched == SQL_NULL_DATA)
        Py_RETURN_NONE;

    return PyFloat_FromDouble(value);
}

/*
static PyObject* GetSqlServerTime(zlyCursor* cur, Py_ssize_t iCol)
{
    SQL_SS_TIME2_STRUCT value;

    SQLLEN cbFetched = 0;
    SQLRETURN ret;

    Py_BEGIN_ALLOW_THREADS
        ret = SQLGetData(cur->hstmt, (SQLUSMALLINT)(iCol+1), SQL_C_BINARY, &value, sizeof(value), &cbFetched);
    Py_END_ALLOW_THREADS
        if (!SQL_SUCCEEDED(ret))
            return RaiseErrorFromHandle("SQLGetData", cur->cnxn->hdbc, cur->hstmt);

    if (cbFetched == SQL_NULL_DATA)
        Py_RETURN_NONE;

    stlInt32 micros = (stlInt32)(value.fraction / 1000); // nanos --> micros
    return PyTime_FromTime(value.hour, value.minute, value.second, micros);
}
*/

static PyObject* GetDataTimestamp(zlyCursor* cur, Py_ssize_t iCol)
{
    TIMESTAMP_STRUCT value;

    SQLLEN cbFetched = 0;
    SQLRETURN ret;
    stlInt32 micros;

    Py_BEGIN_ALLOW_THREADS;

    ret = SQLGetData(cur->hstmt, (SQLUSMALLINT)(iCol+1), SQL_C_TYPE_TIMESTAMP, &value, sizeof(value), &cbFetched);
    Py_END_ALLOW_THREADS;

    if (!SQL_SUCCEEDED(ret))
        return RaiseErrorFromHandle("SQLGetData", cur->cnxn->hdbc, cur->hstmt);

    if (cbFetched == SQL_NULL_DATA)
        Py_RETURN_NONE;

    switch (cur->colinfos[iCol].sql_type)
    {
        case SQL_TYPE_TIME:
        {
            micros = (stlInt32)(value.fraction / 1000); // nanos --> micros
            return PyTime_FromTime(value.hour, value.minute, value.second, micros);
        }

        case SQL_TYPE_DATE:
            return PyDate_FromDate(value.year, value.month, value.day);
    }

    micros = (stlInt32)(value.fraction / 1000); // nanos --> micros
    return PyDateTime_FromDateAndTime(value.year, value.month, value.day, value.hour, value.minute, value.second, micros);
}

stlInt32 GetUserConvIndex(zlyCursor* cur, SQLSMALLINT sql_type)
{
    // If this sql type has a user-defined conversion, the index into the connection's `conv_funcs` array is returned.
    // Otherwise -1 is returned.
    stlInt32 i = 0;

    for(i = 0; i < cur->cnxn->conv_count; i++)
    {
        if (cur->cnxn->conv_types[i] == sql_type)
        {
            return i;
        }
    }
    return -1;
}


PyObject* GetData(zlyCursor* cur, Py_ssize_t iCol)
{
    // Returns an object representing the value in the row/field.  If 0 is returned, an exception has already been set.
    //
    // The data is assumed to be the default C type for the column's SQL type.
    zlyColumnInfo* pinfo = NULL;
    stlInt32 conv_index;

    pinfo = &cur->colinfos[iCol];

    // First see if there is a user-defined conversion.
    conv_index = GetUserConvIndex(cur, pinfo->sql_type);
    if (conv_index != -1)
        return GetDataUser(cur, iCol, conv_index);

    switch (pinfo->sql_type)
    {
        case SQL_WCHAR:
        case SQL_WVARCHAR:
        case SQL_WLONGVARCHAR:
        case SQL_CHAR:
        case SQL_VARCHAR:
        case SQL_LONGVARCHAR:
        case SQL_GUID:
//    case SQL_SS_XML:
#if PY_VERSION_HEX >= 0x02060000
        case SQL_BINARY:
        case SQL_VARBINARY:
        case SQL_LONGVARBINARY:
#endif
            return GetDataString(cur, iCol);

#if PY_VERSION_HEX < 0x02060000
        case SQL_BINARY:
        case SQL_VARBINARY:
        case SQL_LONGVARBINARY:
            return GetDataBuffer(cur, iCol);
#endif

        case SQL_DECIMAL:
        case SQL_NUMERIC:
        {
            if (decimal_type == 0)
                break;

            return GetDataDecimal(cur, iCol);
        }

        case SQL_BIT:
            return GetDataBit(cur, iCol);

        case SQL_TINYINT:
        case SQL_SMALLINT:
        case SQL_INTEGER:
            return GetDataLong(cur, iCol);

        case SQL_BIGINT:
            return GetDataLongLong(cur, iCol);

        case SQL_REAL:
        case SQL_FLOAT:
        case SQL_DOUBLE:
            return GetDataDouble(cur, iCol);


        case SQL_TYPE_DATE:
        case SQL_TYPE_TIME:
        case SQL_TYPE_TIMESTAMP:
            return GetDataTimestamp(cur, iCol);

            /*
        case SQL_SS_TIME2:
            return GetSqlServerTime(cur, iCol);
            */
    }

    return RaiseErrorV("HY106", ProgrammingError, "ODBC SQL type %d is not yet supported.  column-index=%zd  type=%d",
                       (stlInt32)pinfo->sql_type, iCol, (stlInt32)pinfo->sql_type);
}

/**
 * @}
 */

