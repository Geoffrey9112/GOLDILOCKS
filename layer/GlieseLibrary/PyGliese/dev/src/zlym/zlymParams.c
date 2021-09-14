/*******************************************************************************
 * zlymObject.c
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
 * @file zlymObject.c
 * @brief Python Object for Gliese Python Database API
 */

/**
 * @addtogroup zlymObject
 * @{
 */

/**
 * @brief External
 */

#include <zlyDef.h>
#include <zlymModule.h>
#include <zlymParams.h>
#include <zlymCursor.h>
#include <zlymConnection.h>
#include <zlymBuffer.h>
#include <zlymErrors.h>
//#include "dbspecific.h"
//#include "sqlwchar.h"
#include <zlymPythonWrapper.h>

#include <datetime.h>

PyObject* null_binary;

inline zlyConnection* GetConnection(zlyCursor* cursor)
{
    return (zlyConnection*)cursor->cnxn;
}

static stlBool GetParamType(zlyCursor* cur, Py_ssize_t iParam, SQLSMALLINT *type);

static void FreeInfos(zlyParamInfo* a, Py_ssize_t count)
{
    Py_ssize_t i = 0;

    for(i = 0; i < count; i++)
    {
        if (a[i].allocated)
            pyodbc_free(a[i].ParameterValuePtr);
        zlymPyXDecRef(a[i].pParam);
    }
    pyodbc_free(a);
}

#define _MAKESTR(n) case n: return #n
static const char* SqlTypeName(SQLSMALLINT n)
{
    switch (n)
    {
        _MAKESTR(SQL_UNKNOWN_TYPE);
        _MAKESTR(SQL_CHAR);
        _MAKESTR(SQL_VARCHAR);
        _MAKESTR(SQL_LONGVARCHAR);
        _MAKESTR(SQL_NUMERIC);
        _MAKESTR(SQL_DECIMAL);
        _MAKESTR(SQL_INTEGER);
        _MAKESTR(SQL_SMALLINT);
        _MAKESTR(SQL_FLOAT);
        _MAKESTR(SQL_REAL);
        _MAKESTR(SQL_DOUBLE);
        _MAKESTR(SQL_DATETIME);
        _MAKESTR(SQL_WCHAR);
        _MAKESTR(SQL_WVARCHAR);
        _MAKESTR(SQL_WLONGVARCHAR);
        _MAKESTR(SQL_TYPE_DATE);
        _MAKESTR(SQL_TYPE_TIME);
        _MAKESTR(SQL_TYPE_TIMESTAMP);
        //_MAKESTR(SQL_SS_TIME2);
        //_MAKESTR(SQL_SS_XML);
        _MAKESTR(SQL_BINARY);
        _MAKESTR(SQL_VARBINARY);
        _MAKESTR(SQL_LONGVARBINARY);
    }
    return "unknown";
}

static const char* CTypeName(SQLSMALLINT n)
{
    switch (n)
    {
        _MAKESTR(SQL_C_CHAR);
        _MAKESTR(SQL_C_WCHAR);
        _MAKESTR(SQL_C_LONG);
        _MAKESTR(SQL_C_SHORT);
        _MAKESTR(SQL_C_FLOAT);
        _MAKESTR(SQL_C_DOUBLE);
        _MAKESTR(SQL_C_NUMERIC);
        _MAKESTR(SQL_C_DEFAULT);
        _MAKESTR(SQL_C_DATE);
        _MAKESTR(SQL_C_TIME);
        _MAKESTR(SQL_C_TIMESTAMP);
        _MAKESTR(SQL_C_TYPE_DATE);
        _MAKESTR(SQL_C_TYPE_TIME);
        _MAKESTR(SQL_C_TYPE_TIMESTAMP);
        _MAKESTR(SQL_C_INTERVAL_YEAR);
        _MAKESTR(SQL_C_INTERVAL_MONTH);
        _MAKESTR(SQL_C_INTERVAL_DAY);
        _MAKESTR(SQL_C_INTERVAL_HOUR);
        _MAKESTR(SQL_C_INTERVAL_MINUTE);
        _MAKESTR(SQL_C_INTERVAL_SECOND);
        _MAKESTR(SQL_C_INTERVAL_YEAR_TO_MONTH);
        _MAKESTR(SQL_C_INTERVAL_DAY_TO_HOUR);
        _MAKESTR(SQL_C_INTERVAL_DAY_TO_MINUTE);
        _MAKESTR(SQL_C_INTERVAL_DAY_TO_SECOND);
        _MAKESTR(SQL_C_INTERVAL_HOUR_TO_MINUTE);
        _MAKESTR(SQL_C_INTERVAL_HOUR_TO_SECOND);
        _MAKESTR(SQL_C_INTERVAL_MINUTE_TO_SECOND);
        _MAKESTR(SQL_C_BINARY);
        _MAKESTR(SQL_C_BIT);
        _MAKESTR(SQL_C_SBIGINT);
        _MAKESTR(SQL_C_UBIGINT);
        _MAKESTR(SQL_C_TINYINT);
        _MAKESTR(SQL_C_SLONG);
        _MAKESTR(SQL_C_SSHORT);
        _MAKESTR(SQL_C_STINYINT);
        _MAKESTR(SQL_C_ULONG);
        _MAKESTR(SQL_C_USHORT);
        _MAKESTR(SQL_C_UTINYINT);
        _MAKESTR(SQL_C_GUID);
    }
    return "unknown";
}

static stlBool GetNullInfo(zlyCursor* cur, Py_ssize_t index, zlyParamInfo *info)
{
    if (!GetParamType(cur, index, &info->ParameterType))
        return STL_FALSE;

    info->ValueType     = SQL_C_DEFAULT;
    info->ColumnSize    = 1;
    info->StrLen_or_Ind = SQL_NULL_DATA;
    return STL_TRUE;
}

static stlBool GetNullBinaryInfo(zlyCursor* cur, Py_ssize_t index, zlyParamInfo *info)
{
    info->ValueType         = SQL_C_BINARY;
    info->ParameterType     = SQL_BINARY;
    info->ColumnSize        = 1;
    info->ParameterValuePtr = 0;
    info->StrLen_or_Ind     = SQL_NULL_DATA;
    return STL_TRUE;
}


static stlBool GetBytesInfo(zlyCursor* cur, Py_ssize_t index, PyObject* param, zlyParamInfo *info)
{
    // In Python 2, a bytes object (ANSI string) is passed as varchar.  In Python 3, it is passed as binary.

    Py_ssize_t len = PyBytes_GET_SIZE(param);

#if PY_MAJOR_VERSION >= 3
    info->ValueType = SQL_C_BINARY;
    info->ColumnSize = (SQLUINTEGER)max(len, 1);

    if (len <= cur->cnxn->binary_maxlength)
    {
        info->ParameterType     = SQL_VARBINARY;
        info->StrLen_or_Ind     = len;
        info->ParameterValuePtr = PyBytes_AS_STRING(param);
    }
    else
    {
        // Too long to pass all at once, so we'll provide the data at execute.
        info->ParameterType     = SQL_LONGVARBINARY;
        info->StrLen_or_Ind     = SQL_LEN_DATA_AT_EXEC((SQLLEN)len);
        info->ParameterValuePtr = param;
    }

#else
    info->ValueType = SQL_C_CHAR;
    info->ColumnSize = (SQLUINTEGER)max(len, 1);

    if (len <= cur->cnxn->varchar_maxlength)
    {
        info->ParameterType     = SQL_VARCHAR;
        info->StrLen_or_Ind     = len;
        info->ParameterValuePtr = PyBytes_AS_STRING(param);
    }
    else
    {
        // Too long to pass all at once, so we'll provide the data at execute.
        info->ParameterType     = SQL_LONGVARCHAR;
        info->StrLen_or_Ind     = SQL_LEN_DATA_AT_EXEC((SQLLEN)len);
        info->ParameterValuePtr = param;
    }
#endif

    return STL_TRUE;
}

/* TODOTODO: Unicode는 아직까지 지원하지 않음
static stlBool GetUnicodeInfo(zlyCursor* cur, Py_ssize_t index, PyObject* param, zlyParamInfo *info)
{
    Py_UNICODE* pch = PyUnicode_AsUnicode(param);
    Py_ssize_t  len = PyUnicode_GET_SIZE(param);

    info->ValueType  = SQL_C_WCHAR;
    info->ColumnSize = (SQLUINTEGER)max(len, 1);

    if (len <= cur->cnxn->wvarchar_maxlength)
    {
        if (SQLWCHAR_SIZE == Py_UNICODE_SIZE)
        {
            info->ParameterValuePtr = pch;
        }
        else
        {
            // SQLWCHAR and Py_UNICODE are not the same size, so we need to allocate and copy a buffer.
            if (len > 0)
            {
                info->ParameterValuePtr = SQLWCHAR_FromUnicode(pch, len);
                if (info->ParameterValuePtr == 0)
                    return STL_FALSE;
                info->allocated = STL_TRUE;
            }
            else
            {
                info->ParameterValuePtr = pch;
            }
        }

        info->ParameterType = SQL_WVARCHAR;
        info->StrLen_or_Ind = (SQLINTEGER)(len * sizeof(SQLWCHAR));
    }
    else
    {
        // Too long to pass all at once, so we'll provide the data at execute.

        info->ParameterType     = SQL_WLONGVARCHAR;
        info->StrLen_or_Ind     = SQL_LEN_DATA_AT_EXEC((SQLLEN)(len * sizeof(SQLWCHAR)));
        info->ParameterValuePtr = param;
    }

    return STL_TRUE;
}
*/

static stlBool GetBooleanInfo(zlyCursor* cur, Py_ssize_t index, PyObject* param, zlyParamInfo *info)
{
    info->ValueType         = SQL_C_BIT;
    info->ParameterType     = SQL_BIT;
    info->StrLen_or_Ind     = 1;
    info->Data.ch           = (unsigned char)(param == Py_True ? 1 : 0);
    info->ParameterValuePtr = &info->Data.ch;
    return STL_TRUE;
}

static stlBool GetDateTimeInfo(zlyCursor* cur, Py_ssize_t index, PyObject* param, zlyParamInfo *info)
{
    stlInt32 precision;
    stlInt32 keep;

    info->Data.timestamp.year   = (SQLSMALLINT) PyDateTime_GET_YEAR(param);
    info->Data.timestamp.month  = (SQLUSMALLINT)PyDateTime_GET_MONTH(param);
    info->Data.timestamp.day    = (SQLUSMALLINT)PyDateTime_GET_DAY(param);
    info->Data.timestamp.hour   = (SQLUSMALLINT)PyDateTime_DATE_GET_HOUR(param);
    info->Data.timestamp.minute = (SQLUSMALLINT)PyDateTime_DATE_GET_MINUTE(param);
    info->Data.timestamp.second = (SQLUSMALLINT)PyDateTime_DATE_GET_SECOND(param);

    // SQL Server chokes if the fraction has more data than the database supports.  We expect other databases to be the
    // same, so we reduce the value to what the database supports.  http://support.microsoft.com/kb/263872

    precision = ((zlyConnection*)cur->cnxn)->datetime_precision - 20; // (20 includes a separating period)
    if (precision <= 0)
    {
        info->Data.timestamp.fraction = 0;
    }
    else
    {
        info->Data.timestamp.fraction = (SQLUINTEGER)(PyDateTime_DATE_GET_MICROSECOND(param) * 1000); // 1000 == micro -> nano

        // (How many leading digits do we want to keep?  With SQL Server 2005, this should be 3: 123000000)
        keep = (stlInt32)pow(10.0, 9-min(9, precision));
        info->Data.timestamp.fraction = info->Data.timestamp.fraction / keep * keep;
        info->DecimalDigits = (SQLSMALLINT)precision;
    }

    info->ValueType         = SQL_C_TIMESTAMP;
    info->ParameterType     = SQL_TIMESTAMP;
    info->ColumnSize        = (SQLUINTEGER)((zlyConnection*)cur->cnxn)->datetime_precision;
    info->StrLen_or_Ind     = sizeof(TIMESTAMP_STRUCT);
    info->ParameterValuePtr = &info->Data.timestamp;
    return STL_TRUE;
}

static stlBool GetDateInfo(zlyCursor* cur, Py_ssize_t index, PyObject* param, zlyParamInfo *info)
{
    info->Data.date.year  = (SQLSMALLINT) PyDateTime_GET_YEAR(param);
    info->Data.date.month = (SQLUSMALLINT)PyDateTime_GET_MONTH(param);
    info->Data.date.day   = (SQLUSMALLINT)PyDateTime_GET_DAY(param);

    info->ValueType         = SQL_C_TYPE_DATE;
    info->ParameterType     = SQL_TYPE_DATE;
    info->ColumnSize        = 10;
    info->ParameterValuePtr = &info->Data.date;
    info->StrLen_or_Ind     = sizeof(DATE_STRUCT);
    return STL_TRUE;
}

static stlBool GetTimeInfo(zlyCursor* cur, Py_ssize_t index, PyObject* param, zlyParamInfo *info)
{
    info->Data.time.hour   = (SQLUSMALLINT)PyDateTime_TIME_GET_HOUR(param);
    info->Data.time.minute = (SQLUSMALLINT)PyDateTime_TIME_GET_MINUTE(param);
    info->Data.time.second = (SQLUSMALLINT)PyDateTime_TIME_GET_SECOND(param);

    info->ValueType         = SQL_C_TYPE_TIME;
    info->ParameterType     = SQL_TYPE_TIME;
    info->ColumnSize        = 8;
    info->ParameterValuePtr = &info->Data.time;
    info->StrLen_or_Ind     = sizeof(TIME_STRUCT);
    return STL_TRUE;
}

#if PY_MAJOR_VERSION < 3
static stlBool GetIntInfo(zlyCursor* cur, Py_ssize_t index, PyObject* param, zlyParamInfo *info)
{
    info->Data.l = PyInt_AsLong(param);

#if LONG_BIT == 64
    info->ValueType     = SQL_C_SBIGINT;
    info->ParameterType = SQL_BIGINT;
#elif LONG_BIT == 32
    info->ValueType     = SQL_C_LONG;
    info->ParameterType = SQL_INTEGER;
#else
    #error Unexpected LONG_BIT value
#endif

    info->ParameterValuePtr = &info->Data.l;
    return STL_TRUE;
}
#endif

static stlBool GetLongInfo(zlyCursor* cur, Py_ssize_t index, PyObject* param, zlyParamInfo *info)
{
    // TODO: Overflow?
    info->Data.i64 = (stlInt64)PyLong_AsLongLong(param);

    info->ValueType         = SQL_C_SBIGINT;
    info->ParameterType     = SQL_BIGINT;
    info->ParameterValuePtr = &info->Data.i64;
    return STL_TRUE;
}

static stlBool GetFloatInfo(zlyCursor* cur, Py_ssize_t index, PyObject* param, zlyParamInfo *info)
{
    // TODO: Overflow?
    info->Data.dbl = PyFloat_AsDouble(param);

    info->ValueType         = SQL_C_DOUBLE;
    info->ParameterType     = SQL_DOUBLE;
    info->ParameterValuePtr = &info->Data.dbl;
    info->ColumnSize = 15;
    return STL_TRUE;
}

static char* CreateDecimalString(long sign, PyObject* digits, long exp)
{
    long count = (long)PyTuple_GET_SIZE(digits);

    char  *pch;
    int    len;
    long   i = 0;

    if (exp >= 0)
    {
        // (1 2 3) exp = 2 --> '12300'

        len = sign + count + exp + 1; // 1: NULL
        pch = (char*)pyodbc_malloc((size_t)len);
        if (pch)
        {
            char* p = pch;
            if (sign)
                *p++ = '-';
            for (i = 0; i < count; i++)
                *p++ = (char)('0' + PyInt_AS_LONG(PyTuple_GET_ITEM(digits, i)));
            for (i = 0; i < exp; i++)
                *p++ = '0';
            *p = 0;
        }
    }
    else if (-exp < count)
    {
        // (1 2 3) exp = -2 --> 1.23 : prec = 3, scale = 2

        len = sign + count + 2; // 2: decimal + NULL
        pch = (char*)pyodbc_malloc((size_t)len);
        if (pch)
        {
            char* p = pch;

            if (sign)
                *p++ = '-';
            for (; i < (count + exp); i++)
                *p++ = (char)('0' + PyInt_AS_LONG(PyTuple_GET_ITEM(digits, i)));
            *p++ = '.';
            for (; i < count; i++)
                *p++ = (char)('0' + PyInt_AS_LONG(PyTuple_GET_ITEM(digits, i)));
            *p++ = 0;
        }
    }
    else
    {
        // (1 2 3) exp = -5 --> 0.00123 : prec = 5, scale = 5

        len = sign + -exp + 3; // 3: leading zero + decimal + NULL

        pch = (char*)pyodbc_malloc((size_t)len);
        if (pch)
        {
            char* p = pch;
            if (sign)
                *p++ = '-';
            *p++ = '0';
            *p++ = '.';

            for (i = 0; i < -(exp + count); i++)
                *p++ = '0';

            for (i = 0; i < count; i++)
                *p++ = (char)('0' + PyInt_AS_LONG(PyTuple_GET_ITEM(digits, i)));
            *p++ = 0;
        }
    }

    I(pch == 0 || (int)(strlen(pch) + 1) == len);

    return pch;
}

static stlBool GetDecimalInfo(zlyCursor* cur, Py_ssize_t index, PyObject* param, zlyParamInfo *info)
{
    PyObject   *t = NULL;
    PyObject  *digits;
    long       sign;
    long       exp;

    Py_ssize_t count;

    // The NUMERIC structure never works right with SQL Server and probably a lot of other drivers.  We'll bind as a
    // string.  Unfortunately, the Decimal class doesn't seem to have a way to force it to return a string without
    // exponents, so we'll have to build it ourselves.

    t = PyObject_CallMethod(param, "as_tuple", 0);
    STL_TRY( ZLY_IS_VALID_PYOBJECT(t) == STL_TRUE );

    sign   = PyInt_AsLong(PyTuple_GET_ITEM(t, 0));
    digits = PyTuple_GET_ITEM(t, 1);
    exp    = PyInt_AsLong(PyTuple_GET_ITEM(t, 2));

    count = PyTuple_GET_SIZE(digits);

    info->ValueType     = SQL_C_CHAR;
    info->ParameterType = SQL_NUMERIC;

    if (exp >= 0)
    {
        // (1 2 3) exp = 2 --> '12300'

        info->ColumnSize    = (SQLUINTEGER)count + exp;
        info->DecimalDigits = 0;

    }
    else if (-exp <= count)
    {
        // (1 2 3) exp = -2 --> 1.23 : prec = 3, scale = 2
        info->ColumnSize    = (SQLUINTEGER)count;
        info->DecimalDigits = (SQLSMALLINT)-exp;
    }
    else
    {
        // (1 2 3) exp = -5 --> 0.00123 : prec = 5, scale = 5
        info->ColumnSize    = (SQLUINTEGER)(count + (-exp));
        info->DecimalDigits = (SQLSMALLINT)info->ColumnSize;
    }

    I(info->ColumnSize >= (SQLULEN)info->DecimalDigits);

    info->ParameterValuePtr = CreateDecimalString(sign, digits, exp);
    STL_TRY_THROW( info->ParameterValuePtr != NULL, RAMP_ERR_NO_MEMORY );
    info->allocated = STL_TRUE;

    info->StrLen_or_Ind = (SQLINTEGER)strlen((char*)info->ParameterValuePtr);

    return STL_TRUE;

    STL_CATCH( RAMP_ERR_NO_MEMORY )
    {
        PyErr_NoMemory();
    }
    STL_FINISH;

    return STL_FALSE;
}

#if PY_MAJOR_VERSION < 3
static stlBool GetBufferInfo(zlyCursor* cur, Py_ssize_t index, PyObject* param, zlyParamInfo *info)
{
    const char* pb;
    Py_ssize_t  cb = PyBuffer_GetMemory(param, &pb);

    info->ValueType = SQL_C_BINARY;

    if (cb != -1 && cb <= cur->cnxn->binary_maxlength)
    {
        // There is one segment, so we can bind directly into the buffer object.

        info->ParameterType     = SQL_VARBINARY;
        info->ParameterValuePtr = (SQLPOINTER)pb;
        info->BufferLength      = cb;
        info->ColumnSize        = (SQLUINTEGER)max(cb, 1);
        info->StrLen_or_Ind     = cb;
    }
    else
    {
        // There are multiple segments, so we'll provide the data at execution time.  Pass the PyObject pointer as
        // the parameter value which will be pased back to us when the data is needed.  (If we release threads, we
        // need to up the refcount!)

        info->ParameterType     = SQL_LONGVARBINARY;
        info->ParameterValuePtr = param;
        info->ColumnSize        = (SQLUINTEGER)PyBuffer_Size(param);
        info->BufferLength      = sizeof(PyObject*); // How big is ParameterValuePtr; ODBC copies it and gives it back in SQLParamData
        info->StrLen_or_Ind     = SQL_LEN_DATA_AT_EXEC(PyBuffer_Size(param));
    }

    return STL_TRUE;
}
#endif

#if PY_VERSION_HEX >= 0x02060000
static stlBool GetByteArrayInfo(zlyCursor* cur, Py_ssize_t index, PyObject* param, zlyParamInfo *info)
{
    Py_ssize_t cb = PyByteArray_Size(param);

    info->ValueType = SQL_C_BINARY;
    if (cb <= cur->cnxn->binary_maxlength)
    {
        info->ParameterType     = SQL_VARBINARY;
        info->ParameterValuePtr = (SQLPOINTER)PyByteArray_AsString(param);
        info->BufferLength      = cb;
        info->ColumnSize        = (SQLUINTEGER)max(cb, 1);
        info->StrLen_or_Ind     = cb;
    }
    else
    {
        info->ParameterType     = SQL_LONGVARBINARY;
        info->ParameterValuePtr = param;
        info->ColumnSize        = (SQLUINTEGER)cb;
        info->BufferLength      = sizeof(PyObject*); // How big is ParameterValuePtr; ODBC copies it and gives it back in SQLParamData
        info->StrLen_or_Ind     = SQL_LEN_DATA_AT_EXEC(cb);
    }
    return STL_TRUE;
}
#endif

static stlBool GetParameterInfo(zlyCursor* cur, Py_ssize_t index, PyObject* param, zlyParamInfo *info)
{
    // Determines the type of SQL parameter that will be used for this parameter based on the Python data type.
    //
    // Populates `info`.

    // Hold a reference to param until info is freed, because info will often be holding data borrowed from param.
    info->pParam = param;

    if (param == Py_None)
        return GetNullInfo(cur, index, info);

    if (param == null_binary)
        return GetNullBinaryInfo(cur, index, info);

    if (PyBytes_Check(param))
        return GetBytesInfo(cur, index, param, info);

    /* TODOTODO: Unicode는 아직까지 지원하지 않음
    if (PyUnicode_Check(param))
        return GetUnicodeInfo(cur, index, param, info);
    */
    if (PyBool_Check(param))
        return GetBooleanInfo(cur, index, param, info);

    if (PyDateTime_Check(param))
        return GetDateTimeInfo(cur, index, param, info);

    if (PyDate_Check(param))
        return GetDateInfo(cur, index, param, info);

    if (PyTime_Check(param))
        return GetTimeInfo(cur, index, param, info);

    if (PyLong_Check(param))
        return GetLongInfo(cur, index, param, info);

    if (PyFloat_Check(param))
        return GetFloatInfo(cur, index, param, info);

    if (PyDecimal_Check(param))
        return GetDecimalInfo(cur, index, param, info);

#if PY_VERSION_HEX >= 0x02060000
    if (PyByteArray_Check(param))
        return GetByteArrayInfo(cur, index, param, info);
#endif

#if PY_MAJOR_VERSION < 3
    if (PyInt_Check(param))
        return GetIntInfo(cur, index, param, info);

    if (PyBuffer_Check(param))
        return GetBufferInfo(cur, index, param, info);
#endif

    RaiseErrorV("HY105", ProgrammingError, "Invalid parameter type.  param-index=%zd param-type=%s", index, Py_TYPE(param)->tp_name);
    return STL_FALSE;
}

stlBool BindParameter(zlyCursor* cur, Py_ssize_t index, zlyParamInfo *info)
{
    SQLRETURN ret = -1;

    TRACE("BIND: param=%d ValueType=%d (%s) ParameterType=%d (%s) ColumnSize=%d DecimalDigits=%d BufferLength=%d *pcb=%d\n",
          (index+1), info->ValueType, CTypeName(info->ValueType), info->ParameterType, SqlTypeName(info->ParameterType), info->ColumnSize,
          info->DecimalDigits, info->BufferLength, info->StrLen_or_Ind);

    Py_BEGIN_ALLOW_THREADS
    ret = SQLBindParameter(cur->hstmt, (SQLUSMALLINT)(index + 1), SQL_PARAM_INPUT, info->ValueType, info->ParameterType, info->ColumnSize, info->DecimalDigits, info->ParameterValuePtr, info->BufferLength, &info->StrLen_or_Ind);
    Py_END_ALLOW_THREADS;

    if (GetConnection(cur)->hdbc == SQL_NULL_HANDLE)
    {
        // The connection was closed by another thread in the ALLOW_THREADS block above.
        RaiseErrorV(0, ProgrammingError, "The cursor's connection was closed.");
        return STL_FALSE;
    }

    if (!SQL_SUCCEEDED(ret))
    {
        RaiseErrorFromHandle("SQLBindParameter", GetConnection(cur)->hdbc, cur->hstmt);
        return STL_FALSE;
    }

    return STL_TRUE;
}


void FreeParameterData(zlyCursor* cur)
{
    // Unbinds the parameters and frees the parameter buffer.

    if (cur->paramInfos)
    {
        // MS ODBC will crash if we use an HSTMT after the HDBC has been freed.
        if (cur->cnxn->hdbc != SQL_NULL_HANDLE)
        {
            Py_BEGIN_ALLOW_THREADS
            SQLFreeStmt(cur->hstmt, SQL_RESET_PARAMS);
            Py_END_ALLOW_THREADS
        }

        FreeInfos(cur->paramInfos, cur->paramcount);
        cur->paramInfos = 0;
    }
}

void FreeParameterInfo(zlyCursor* cur)
{
    // Internal function to free just the cached parameter information.  This is not used by the general cursor code
    // since this information is also freed in the less granular free_results function that clears everything.

    zlymPyXDecRef(cur->pPreparedSQL);
    pyodbc_free(cur->paramtypes);
    cur->pPreparedSQL = 0;
    cur->paramtypes   = 0;
    cur->paramcount   = 0;
}

stlBool PrepareAndBind(zlyCursor* cur, PyObject* pSql, PyObject* original_params, stlBool skip_first)
{
    int        params_offset = skip_first ? 1 : 0;
    Py_ssize_t cParams       = original_params == 0 ? 0 : PySequence_Length(original_params) - params_offset;
    Py_ssize_t i = 0;

#if PY_MAJOR_VERSION >= 3
    if (!PyUnicode_Check(pSql))
    {
        PyErr_SetString(PyExc_TypeError, "SQL must be a Unicode string");
        return STL_FALSE;
    }
#endif

    //
    // Normalize the parameter variables.
    //

    // Since we may replace parameters (we replace objects with Py_True/Py_False when writing to a bit/bool column),
    // allocate an array and use it instead of the original sequence

    //
    // Prepare the SQL if necessary.
    //

    if (pSql != cur->pPreparedSQL)
    {
        SQLRETURN ret = 0;
        SQLSMALLINT cParamsT = 0;
        const char* szErrorFunc = "SQLPrepare";

        FreeParameterInfo(cur);

        if (PyUnicode_Check(pSql))
        {
            /* TODOTODO: Unicode는 현재 지원하지 않음
            SQLWChar sql(pSql);
            Py_BEGIN_ALLOW_THREADS
            ret = SQLPrepareW(cur->hstmt, sql, SQL_NTS);
            if (SQL_SUCCEEDED(ret))
            {
                szErrorFunc = "SQLNumParams";
                ret = SQLNumParams(cur->hstmt, &cParamsT);
            }
            Py_END_ALLOW_THREADS
            */
        }
#if PY_MAJOR_VERSION < 3
        else
        {
            TRACE("SQLPrepare(%s)\n", PyString_AS_STRING(pSql));
            Py_BEGIN_ALLOW_THREADS
            ret = SQLPrepare(cur->hstmt, (SQLCHAR*)PyString_AS_STRING(pSql), SQL_NTS);
            if (SQL_SUCCEEDED(ret))
            {
                szErrorFunc = "SQLNumParams";
                ret = SQLNumParams(cur->hstmt, &cParamsT);
            }
            Py_END_ALLOW_THREADS
        }
#endif

        if (cur->cnxn->hdbc == SQL_NULL_HANDLE)
        {
            // The connection was closed by another thread in the ALLOW_THREADS block above.
            RaiseErrorV(0, ProgrammingError, "The cursor's connection was closed.");
            return STL_FALSE;
        }

        if (!SQL_SUCCEEDED(ret))
        {
            RaiseErrorFromHandle(szErrorFunc, GetConnection(cur)->hdbc, cur->hstmt);
            return STL_FALSE;
        }

        cur->paramcount = (int)cParamsT;

        cur->pPreparedSQL = pSql;
        Py_INCREF(cur->pPreparedSQL);
    }

    if (cParams != cur->paramcount)
    {
        RaiseErrorV(0, ProgrammingError, "The SQL contains %d parameter markers, but %d parameters were supplied",
                    cur->paramcount, cParams);
        return STL_FALSE;
    }

    cur->paramInfos = (zlyParamInfo*)pyodbc_malloc(sizeof(zlyParamInfo) * cParams);
    if (cur->paramInfos == 0)
    {
        PyErr_NoMemory();
        return 0;
    }
    memset(cur->paramInfos, 0, sizeof(zlyParamInfo) * cParams);

    // Since you can't call SQLDesribeParam *after* calling SQLBindParameter, we'll loop through all of the
    // GetParameterInfos first, then bind.

    for(i = 0; i < cParams; i++)
    {
        // PySequence_GetItem returns a *new* reference, which GetParameterInfo will take ownership of.  It is stored
        // in paramInfos and will be released in FreeInfos (which is always eventually called).

        PyObject* param = PySequence_GetItem(original_params, i + params_offset);
        if (!GetParameterInfo(cur, i, param, &cur->paramInfos[i]))
        {
            FreeInfos(cur->paramInfos, cParams);
            cur->paramInfos = 0;
            return STL_FALSE;
        }
    }

    for(i = 0; i < cParams; i++)
    {
        if (!BindParameter(cur, i, &cur->paramInfos[i]))
        {
            FreeInfos(cur->paramInfos, cParams);
            cur->paramInfos = 0;
            return STL_FALSE;
        }
    }

    return STL_TRUE;
}

static stlBool GetParamType(zlyCursor* cur, Py_ssize_t index, SQLSMALLINT *type)
{
    // Returns the ODBC type of the of given parameter.
    //
    // Normally we set the parameter type based on the parameter's Python object type (e.g. str --> SQL_CHAR), so this
    // is only called when the parameter is None.  In that case, we can't guess the type and have to use
    // SQLDescribeParam.
    //
    // If the database doesn't support SQLDescribeParam, we return SQL_VARCHAR since it converts to most other types.
    // However, it will not usually work if the target column is a binary column.

    if (!GetConnection(cur)->supports_describeparam || cur->paramcount == 0)
    {
        *type = SQL_VARCHAR;
        return STL_TRUE;
    }

    if (cur->paramtypes == 0)
    {
        cur->paramtypes = (SQLSMALLINT *)(pyodbc_malloc(sizeof(SQLSMALLINT) * cur->paramcount));
        if (cur->paramtypes == 0)
        {
            PyErr_NoMemory();
            return STL_FALSE;
        }

        // SQL_UNKNOWN_TYPE is zero, so zero out all columns since we haven't looked any up yet.
        memset(cur->paramtypes, 0, sizeof(SQLSMALLINT) * cur->paramcount);
    }

    if (cur->paramtypes[index] == SQL_UNKNOWN_TYPE)
    {
        SQLULEN ParameterSizePtr;
        SQLSMALLINT DecimalDigitsPtr;
        SQLSMALLINT NullablePtr;
        SQLRETURN ret;

        Py_BEGIN_ALLOW_THREADS
        ret = SQLDescribeParam(cur->hstmt, (SQLUSMALLINT)(index + 1), &cur->paramtypes[index], &ParameterSizePtr, &DecimalDigitsPtr, &NullablePtr);
        Py_END_ALLOW_THREADS

        if (!SQL_SUCCEEDED(ret))
        {
            // This can happen with ("select ?", None).  We'll default to VARCHAR which works with most types.
            cur->paramtypes[index] = SQL_VARCHAR;
        }
    }

    *type = cur->paramtypes[index];
    return STL_TRUE;
}

static PyTypeObject NullParamType =
{
    PyVarObject_HEAD_INIT(NULL, 0)
    "pygliese.NullParam",       // tp_name
    sizeof(zlyNullParam),       // tp_basicsize
    0,                          // tp_itemsize
    0,                          // destructor tp_dealloc
    0,                          // tp_print
    0,                          // tp_getattr
    0,                          // tp_setattr
    0,                          // tp_compare
    0,                          // tp_repr
    0,                          // tp_as_number
    0,                          // tp_as_sequence
    0,                          // tp_as_mapping
    0,                          // tp_hash
    0,                          // tp_call
    0,                          // tp_str
    0,                          // tp_getattro
    0,                          // tp_setattro
    0,                          // tp_as_buffer
    Py_TPFLAGS_DEFAULT,         // tp_flags
    NULL,                       // tp_doc
    0,                          // tp_traverse
    0,                          // tp_clear
    0,                          // tp_richcompare
    0,                          // tp_weaklistoffset
    0,                          // tp_iter
    0,                          // tp_iternext
    NULL,                       // tp_methods
    0,                          // tp_members
    NULL,                       // tp_getset
    0,                          // tp_base
    0,                          // tp_dict
    0,                          // tp_descr_get
    0,                          // tp_descr_set
    0,                          // tp_dictoffset
    0,                          // tp_init
    0,                          // tp_alloc
    0,                          // tp_new
    0,                          // tp_free
    0,                          // tp_is_gc
    0,                          // tp_bases
    0,                          // tp_mro
    0,                          // tp_cache
    0,                          // tp_subclasses
    0,                          // tp_weaklist
    0,                          // tp_del
    0,                          // tp_version_tag
};

stlBool Params_init()
{
    if (PyType_Ready(&NullParamType) < 0)
        return STL_FALSE;

    null_binary = (PyObject*)PyObject_New(zlyNullParam, &NullParamType);
    if (null_binary == 0)
        return STL_FALSE;

    PyDateTime_IMPORT;

    return STL_TRUE;
}

/**
 * @}
 */
