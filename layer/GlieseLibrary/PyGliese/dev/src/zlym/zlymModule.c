/*******************************************************************************
 * zlymModule.c
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
#include <zlymDbg.h>
#include <zlymModule.h>
#include <zlymConnection.h>
#include <zlymCursor.h>
/*
#include "row.h"
#include "wrapper.h"
*/
#include <zlymErrors.h>
#include <zlymGetData.h>
#include <zlymParams.h>
/*
#include "dbspecific.h"
*/
#include <zlymPythonWrapper.h>

/* python-dev header files */
#include <datetime.h>

/**
 * @file zlymModule.c
 * @brief Gliese Python Database API functions
 */

/**
 * @addtogroup zlymModule
 * @{
 */

/**
 * @brief External
 */

static PyObject* MakeConnectionString(PyObject* existing, PyObject* parts);

PyObject* pModule = 0;

static char module_doc[] =
    "A database module for accessing databases via ODBC.\n"
    "\n"
    "This module conforms to the DB API 2.0 specification while providing\n"
    "non-standard convenience features.  Only standard Python data types are used\n"
    "so additional DLLs are not required.\n"
    "\n"
    "Static Variables:\n\n"
    "version\n"
    "  The module version string.  Official builds will have a version in the format\n"
    "  `major.minor.revision`, such as 2.1.7.  Beta versions will have -beta appended,\n"
    "  such as 2.1.8-beta03.  (This would be a build before the official 2.1.8 release.)\n"
    "  Some special test builds will have a test name (the git branch name) prepended,\n"
    "  such as fixissue90-2.1.8-beta03.\n"
    "\n"
    "apilevel\n"
    "  The string constant '2.0' indicating this module supports DB API level 2.0.\n"
    "\n"
    "lowercase\n"
    "  A Boolean that controls whether column names in result rows are lowercased.\n"
    "  This can be changed any time and affects queries executed after the change.\n"
    "  The default is False.  This can be useful when database columns have\n"
    "  inconsistent capitalization.\n"
    "\n"
    "pooling\n"
    "  A Boolean indicating whether connection pooling is enabled.  This is a\n"
    "  global (HENV) setting, so it can only be modified before the first\n"
    "  connection is made.  The default is True, which enables ODBC connection\n"
    "  pooling.\n"
    "\n"
    "threadsafety\n"
    "  The integer 1, indicating that threads may share the module but not\n"
    "  connections.  Note that connections and cursors may be used by different\n"
    "  threads, just not at the same time.\n"
    "\n"
    "qmark\n"
    "  The string constant 'qmark' to indicate parameters are identified using\n"
    "  question marks.";

PyObject* Error;
PyObject* Warning;
PyObject* InterfaceError;
PyObject* DatabaseError;
PyObject* InternalError;
PyObject* OperationalError;
PyObject* ProgrammingError;
PyObject* IntegrityError;
PyObject* DataError;
PyObject* NotSupportedError;

#define MAKEEXCINFO(name, parent, doc) { #name, "PyGliese." #name, &name, &parent, doc }

static zlyExcInfo gExcInfos[] = {
    MAKEEXCINFO(Error, PyExc_Exception,
                "Exception that is the base class of all other error exceptions. You can use\n"
                "this to catch all errors with one single 'except' statement."),
    MAKEEXCINFO(Warning, PyExc_Exception,
                "Exception raised for important warnings like data truncations while inserting,\n"
                " etc."),
    MAKEEXCINFO(InterfaceError, Error,
                "Exception raised for errors that are related to the database interface rather\n"
                "than the database itself."),
    MAKEEXCINFO(DatabaseError, Error, "Exception raised for errors that are related to the database."),
    MAKEEXCINFO(DataError, DatabaseError,
                "Exception raised for errors that are due to problems with the processed data\n"
                "like division by zero, numeric value out of range, etc."),
    MAKEEXCINFO(OperationalError, DatabaseError,
                "Exception raised for errors that are related to the database's operation and\n"
                "not necessarily under the control of the programmer, e.g. an unexpected\n"
                "disconnect occurs, the data source name is not found, a transaction could not\n"
                "be processed, a memory allocation error occurred during processing, etc."),
    MAKEEXCINFO(IntegrityError, DatabaseError,
                "Exception raised when the relational integrity of the database is affected,\n"
                "e.g. a foreign key check fails."),
    MAKEEXCINFO(InternalError, DatabaseError,
                "Exception raised when the database encounters an internal error, e.g. the\n"
                "cursor is not valid anymore, the transaction is out of sync, etc."),
    MAKEEXCINFO(ProgrammingError, DatabaseError,
                "Exception raised for programming errors, e.g. table not found or already\n"
                "exists, syntax error in the SQL statement, wrong number of parameters\n"
                "specified, etc."),
    MAKEEXCINFO(NotSupportedError, DatabaseError,
                "Exception raised in case a method or database API was used which is not\n"
                "supported by the database, e.g. requesting a .rollback() on a connection that\n"
                "does not support transaction or has transactions turned off.")
};


PyObject* decimal_type;

HENV henv = SQL_NULL_HANDLE;

Py_UNICODE chDecimal = '.';

// Initialize the global decimal character and thousands separator character, used when parsing decimal
// objects.
//
static void init_locale_info()
{
    PyObject *sModule;
    PyObject *sLDict;
    PyObject *sValue;

    sModule = PyImport_ImportModule("locale");
    STL_TRY( sModule != NULL );

    sLDict = PyObject_CallMethod(sModule, "localeconv", 0);
    STL_TRY( sLDict != NULL );

    sValue = PyDict_GetItemString(sLDict, "decimal_point");
    if( sValue != NULL )
    {
        if( PyBytes_Check(sValue) && PyBytes_Size(sValue) == 1 )
        {
            chDecimal = (Py_UNICODE)PyBytes_AS_STRING(sValue)[0];
        }
        if( PyUnicode_Check(sValue) && PyUnicode_GET_SIZE(sValue) == 1 )
        {
            chDecimal = PyUnicode_AS_UNICODE(sValue)[0];
        }
    }

    return;

    STL_FINISH;

    PyErr_Clear();

    return;
}


static stlBool import_types()
{
    // In Python 2.5 final, PyDateTime_IMPORT no longer works unless the datetime module was previously
    // imported (among other problems).

    PyObject* decimalmod;
    PyObject* pdt = PyImport_ImportModule("datetime");

    if (!pdt)
        return STL_FALSE;

    PyDateTime_IMPORT;

    Cursor_init();
    CnxnInfo_init();
    GetData_init();
    if (!Params_init())
        return STL_FALSE;

    decimalmod = PyImport_ImportModule("decimal");
    if (!decimalmod)
    {
        PyErr_SetString(PyExc_RuntimeError, "Unable to import decimal");
        return STL_FALSE;
    }

    decimal_type = PyObject_GetAttrString(decimalmod, "Decimal");
    zlymPyDecRef(decimalmod);

    if (decimal_type == 0)
        PyErr_SetString(PyExc_RuntimeError, "Unable to import decimal.Decimal.");

    return decimal_type != 0;
}


static stlBool AllocateEnv()
{
    PyObject* pooling = PyObject_GetAttrString(pModule, "pooling");
    stlBool bPooling = pooling == Py_True;
    zlymPyDecRef(pooling);

    /*
    if (bPooling)
    {
        if (!SQL_SUCCEEDED(SQLSetEnvAttr(SQL_NULL_HANDLE, SQL_ATTR_CONNECTION_POOLING, (SQLPOINTER)SQL_CP_ONE_PER_HENV, sizeof(int))))
        {
            Py_FatalError("Unable to set SQL_ATTR_CONNECTION_POOLING attribute.");
            return STL_FALSE;
        }
    }
    */

    if (!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv)))
    {
        Py_FatalError("Can't initialize module pyodbc.  SQLAllocEnv failed.");
        return STL_FALSE;
    }

    if (!SQL_SUCCEEDED(SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, sizeof(int))))
    {
        Py_FatalError("Unable to set SQL_ATTR_ODBC_VERSION attribute.");
        return STL_FALSE;
    }

    return STL_TRUE;
}

// Map DB API recommended keywords to ODBC keywords.
static zlyKeywordMap gKeywordMaps[] =
{
    { "user",     "uid",    0 },
    { "password", "pwd",    0 },
    { "host",     "server", 0 },
};

static PyObject* mod_connect(PyObject* self, PyObject* args, PyObject* kwargs)
{

    PyObject  *pConnectString = NULL;
    PyObject  *sRet = NULL;
    stlInt32   fAutoCommit = 0;
    stlInt32   fAnsi = 0;              // force ansi
    stlInt32   fUnicodeResults = 0;
    stlInt32   fReadOnly = 0;
    long       timeout = 0;
    Py_ssize_t size;
    size_t     i = 0;

    UNUSED((void *)self);

    size = args ? PyTuple_Size(args) : 0;

    if (size > 1)
    {
        PyErr_SetString(PyExc_TypeError, "function takes at most 1 non-keyword argument");
        return 0;
    }

    if (size == 1)
    {
        if (!PyString_Check(PyTuple_GET_ITEM(args, 0)) && !PyUnicode_Check(PyTuple_GET_ITEM(args, 0)))
            return PyErr_Format(PyExc_TypeError, "argument 1 must be a string or unicode object");

        ZLY_ATTACH_PYOBJECT( pConnectString, PyUnicode_FromObject(PyTuple_GetItem(args, 0)) );
        STL_TRY( ZLY_IS_VALID_PYOBJECT(pConnectString) );
    }

    if (kwargs && PyDict_Size(kwargs) > 0)
    {
        PyObject  *partsdict;

        Py_ssize_t pos = 0;
        PyObject* key = 0;
        PyObject* value = 0;
        PyObject* str ;

        partsdict = PyDict_New();
        STL_TRY( ZLY_IS_VALID_PYOBJECT( partsdict ) == STL_TRUE );

        while (PyDict_Next(kwargs, &pos, &key, &value))
        {
            if (!Text_Check(key))
                return PyErr_Format(PyExc_TypeError, "Dictionary items passed to connect must be strings");

            // // Note: key and value are *borrowed*.
            //
            // // Check for the two non-connection string keywords we accept.  (If we get many more of these, create something
            // // table driven.  Are we sure there isn't a Python function to parse keywords but leave those it doesn't know?)
            // const char* szKey = PyString_AsString(key);

            if (Text_EqualsI(key, "autocommit"))
            {
                fAutoCommit = PyObject_IsTrue(value);
                continue;
            }
            if (Text_EqualsI(key, "ansi"))
            {
                fAnsi = PyObject_IsTrue(value);
                continue;
            }
            if (Text_EqualsI(key, "unicode_results"))
            {
                fUnicodeResults = PyObject_IsTrue(value);
                continue;
            }
            if (Text_EqualsI(key, "timeout"))
            {
                timeout = PyInt_AsLong(value);
                if (PyErr_Occurred())
                    return 0;
                continue;
            }
            if (Text_EqualsI(key, "readonly"))
            {
                fReadOnly = PyObject_IsTrue(value);
                continue;
            }

            // Map DB API recommended names to ODBC names (e.g. user --> uid).

            for (i = 0; i < _countof(gKeywordMaps); i++)
            {
                if (Text_EqualsI(key, gKeywordMaps[i].mOldName))
                {
                    if (gKeywordMaps[i].mNewNameObject == 0)
                    {
                        gKeywordMaps[i].mNewNameObject = PyString_FromString(gKeywordMaps[i].mNewName);
                        if (gKeywordMaps[i].mNewNameObject == 0)
                            return 0;
                    }

                    key = gKeywordMaps[i].mNewNameObject;
                    break;
                }
            }

            str = PyObject_Str(value); // convert if necessary
            if (!str)
                return 0;

            if (PyDict_SetItem(partsdict, key, str) == -1)
            {
                zlymPyXDecRef(str);
                return 0;
            }

            zlymPyXDecRef(str);
        }

        if (PyDict_Size(partsdict))
        {
            ZLY_ATTACH_PYOBJECT( pConnectString, MakeConnectionString(pConnectString, partsdict) );
        }
    }

    STL_TRY_THROW( ZLY_IS_VALID_PYOBJECT(pConnectString),
                   RAMP_ERR_NO_CONNECTION_INFO );

    if (henv == SQL_NULL_HANDLE)
    {
        if (!AllocateEnv())
            return 0;
    }

    return (PyObject*)Connection_New(pConnectString, fAutoCommit != 0, fAnsi != 0, fUnicodeResults != 0, timeout, fReadOnly != 0);

    STL_CATCH( RAMP_ERR_NO_CONNECTION_INFO )
    {
        sRet = PyErr_Format(PyExc_TypeError, "no connection information was passed");
    }
    STL_FINISH;

    return sRet;
}


static PyObject* mod_datasources(PyObject* self)
{
    PyObject* result = NULL;
    /*
    SQLCHAR szDSN[SQL_MAX_DSN_LENGTH];
    SWORD cbDSN;
    SQLCHAR szDesc[200];
    SWORD cbDesc;

    SQLUSMALLINT nDirection = SQL_FETCH_FIRST;

    SQLRETURN ret;

    UNUSED((void *)self);

    if (henv == SQL_NULL_HANDLE && !AllocateEnv())
        return 0;

    result = PyDict_New();
    if (!result)
        return 0;

    for (;;)
    {
        Py_BEGIN_ALLOW_THREADS
        ret = SQLDataSources(henv, nDirection, szDSN,  _countof(szDSN),  &cbDSN, szDesc, _countof(szDesc), &cbDesc);
        Py_END_ALLOW_THREADS
        if (!SQL_SUCCEEDED(ret))
            break;

        (void)PyDict_SetItemString(result, (const char*)szDSN, PyString_FromString((const char*)szDesc));
        nDirection = SQL_FETCH_NEXT;
    }

    if (ret != SQL_NO_DATA)
    {
        zlymPyDecRef(result);
        return RaiseErrorFromHandle("SQLDataSources", SQL_NULL_HANDLE, SQL_NULL_HANDLE);
    }

    */
    return result;
}


static PyObject* mod_timefromticks(PyObject* self, PyObject* args)
{
    PyObject *sLongObject;
    PyObject* num;
    time_t t;
    struct tm* fields;

    UNUSED((void *)self);

    if (!PyArg_ParseTuple(args, "O", &num))
        return 0;

    if (!PyNumber_Check(num))
        return PyErr_Format(PyExc_TypeError, "TimeFromTicks requires a number.");

    sLongObject = PyNumber_Long(num);
    STL_TRY( ZLY_IS_VALID_PYOBJECT( sLongObject ) == STL_TRUE );

    t = PyLong_AsLong(num);
    fields = localtime(&t);

    return PyTime_FromTime(fields->tm_hour, fields->tm_min, fields->tm_sec, 0);

    STL_FINISH;

    return NULL;
}


static PyObject* mod_datefromticks(PyObject* self, PyObject* args)
{
    UNUSED((void *)self);
    return PyDate_FromTimestamp(args);
}


static PyObject* mod_timestampfromticks(PyObject* self, PyObject* args)
{
    UNUSED((void *)self);
    return PyDateTime_FromTimestamp(args);
}


static char connect_doc[] =
    "connect(str, autocommit=False, ansi=False, timeout=0, **kwargs) --> Connection\n"
    "\n"
    "Accepts an ODBC connection string and returns a new Connection object.\n"
    "\n"
    "The connection string will be passed to SQLDriverConnect, so a DSN connection\n"
    "can be created using:\n"
    "\n"
    "  cnxn = pyodbc.connect('DSN=DataSourceName;UID=user;PWD=password')\n"
    "\n"
    "To connect without requiring a DSN, specify the driver and connection\n"
    "information:\n"
    "\n"
    "  DRIVER={SQL Server};SERVER=localhost;DATABASE=testdb;UID=user;PWD=password\n"
    "\n"
    "Note the use of braces when a value contains spaces.  Refer to SQLDriverConnect\n"
    "documentation or the documentation of your ODBC driver for details.\n"
    "\n"
    "The connection string can be passed as the string `str`, as a list of keywords,\n"
    "or a combination of the two.  Any keywords except autocommit, ansi, and timeout\n"
    "(see below) are simply added to the connection string.\n"
    "\n"
    "  connect('server=localhost;user=me')\n"
    "  connect(server='localhost', user='me')\n"
    "  connect('server=localhost', user='me')\n"
    "\n"
    "The DB API recommends the keywords 'user', 'password', and 'host', but these\n"
    "are not valid ODBC keywords, so these will be converted to 'uid', 'pwd', and\n"
    "'server'.\n"
    "\n"
    "Special Keywords\n"
    "\n"
    "The following specal keywords are processed by pyodbc and are not added to the\n"
    "connection string.  (If you must use these in your connection string, pass them\n"
    "as a string, not as keywords.)\n"
    "\n"
    "  autocommit\n"
    "    If False or zero, the default, transactions are created automatically as\n"
    "    defined in the DB API 2.  If True or non-zero, the connection is put into\n"
    "    ODBC autocommit mode and statements are committed automatically.\n"
    "   \n"
    "  ansi\n"
    "    By default, pyodbc first attempts to connect using the Unicode version of\n"
    "    SQLDriverConnectW.  If the driver returns IM001 indicating it does not\n"
    "    support the Unicode version, the ANSI version is tried.  Any other SQLSTATE\n"
    "    is turned into an exception.  Setting ansi to true skips the Unicode\n"
    "    attempt and only connects using the ANSI version.  This is useful for\n"
    "    drivers that return the wrong SQLSTATE (or if pyodbc is out of date and\n"
    "    should support other SQLSTATEs).\n"
    "   \n"
    "  timeout\n"
    "    An integer login timeout in seconds, used to set the SQL_ATTR_LOGIN_TIMEOUT\n"
    "    attribute of the connection.  The default is 0 which means the database's\n"
    "    default timeout, if any, is used.\n";

static char timefromticks_doc[] =
    "TimeFromTicks(ticks) --> datetime.time\n"
    "\n"
    "Returns a time object initialized from the given ticks value (number of seconds\n"
    "since the epoch; see the documentation of the standard Python time module for\n"
    "details).";

static char datefromticks_doc[] =
    "DateFromTicks(ticks) --> datetime.date\n"  \
    "\n"                                                                \
    "Returns a date object initialized from the given ticks value (number of seconds\n" \
    "since the epoch; see the documentation of the standard Python time module for\n" \
    "details).";

static char timestampfromticks_doc[] =
    "TimestampFromTicks(ticks) --> datetime.datetime\n"  \
    "\n"                                                                \
    "Returns a datetime object initialized from the given ticks value (number of\n" \
    "seconds since the epoch; see the documentation of the standard Python time\n" \
    "module for details";

static char datasources_doc[] =
    "dataSources() -> { DSN : Description }\n" \
    "\n" \
    "Returns a dictionary mapping available DSNs to their descriptions.";


#ifdef PYODBC_LEAK_CHECK
static PyObject* mod_leakcheck(PyObject* self, PyObject* args)
{
    UNUSED((void *)self, args);
    pyodbc_leak_check();
    Py_RETURN_NONE;
}
#endif

#ifdef WINVER
static char drivers_doc[] = "drivers() -> [ driver, ... ]\n\nReturns a list of installed drivers";

static PyObject* mod_drivers(PyObject* self, PyObject* args)
{
    RegKey key;
    long ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\ODBC\\ODBCINST.INI\\ODBC Drivers", 0, KEY_QUERY_VALUE, &key.hkey);
    if (ret != ERROR_SUCCESS)
        return PyErr_Format(PyExc_RuntimeError, "Unable to access the driver list in the registry.  error=%ld", ret);

    PyObject  *results = NULL;
    DWORD index = 0;
    char name[255];
    DWORD length = _countof(name);
    PyObject* oname;

    UNUSED((void *)self, args);

    results = PyList_New(0);

    while (RegEnumValue(key, index++, name, &length, 0, 0, 0, 0) == ERROR_SUCCESS)
    {
        if (ret != ERROR_SUCCESS)
            return PyErr_Format(PyExc_RuntimeError, "RegEnumKeyEx failed with error %ld\n", ret);

        oname = PyString_FromStringAndSize(name, (Py_ssize_t)length);
        if (!oname)
            return 0;

        if (PyList_Append(results, oname) != 0)
        {
            zlymPyDecRef(oname);
            return 0;
        }
        length = _countof(name);
    }

    return results;
}
#endif

static PyMethodDef pyodbc_methods[] =
{
    { "connect",            (PyCFunction)mod_connect,            METH_VARARGS|METH_KEYWORDS, connect_doc },
    { "TimeFromTicks",      (PyCFunction)mod_timefromticks,      METH_VARARGS,               timefromticks_doc },
    { "DateFromTicks",      (PyCFunction)mod_datefromticks,      METH_VARARGS,               datefromticks_doc },
    { "TimestampFromTicks", (PyCFunction)mod_timestampfromticks, METH_VARARGS,               timestampfromticks_doc },
    { "dataSources",        (PyCFunction)mod_datasources,        METH_NOARGS,                datasources_doc },

#ifdef WINVER
    { "drivers", (PyCFunction)mod_drivers, METH_NOARGS, drivers_doc },
#endif

#ifdef PYODBC_LEAK_CHECK
    { "leakcheck", (PyCFunction)mod_leakcheck, METH_NOARGS, 0 },
#endif

    { 0, 0, 0, 0 }
};


static void ErrorInit()
{
    // Called during startup to initialize any variables that will be freed by ErrorCleanup.

    Error = 0;
    Warning = 0;
    InterfaceError = 0;
    DatabaseError = 0;
    InternalError = 0;
    OperationalError = 0;
    ProgrammingError = 0;
    IntegrityError = 0;
    DataError = 0;
    NotSupportedError = 0;
    decimal_type = 0;
}


static void ErrorCleanup()
{
    // Called when an error occurs during initialization to release any objects we may have accessed.  Make sure each
    // item released was initialized to zero.  (Static objects are -- non-statics should be initialized in ErrorInit.)

    zlymPyXDecRef(Error);
    zlymPyXDecRef(Warning);
    zlymPyXDecRef(InterfaceError);
    zlymPyXDecRef(DatabaseError);
    zlymPyXDecRef(InternalError);
    zlymPyXDecRef(OperationalError);
    zlymPyXDecRef(ProgrammingError);
    zlymPyXDecRef(IntegrityError);
    zlymPyXDecRef(DataError);
    zlymPyXDecRef(NotSupportedError);
    zlymPyXDecRef(decimal_type);
}

static const zlyConstantDef aConstants[] = {
    MAKECONST(SQL_UNKNOWN_TYPE),
    MAKECONST(SQL_CHAR),
    MAKECONST(SQL_VARCHAR),
    MAKECONST(SQL_LONGVARCHAR),
    MAKECONST(SQL_WCHAR),
    MAKECONST(SQL_WVARCHAR),
    MAKECONST(SQL_WLONGVARCHAR),
    MAKECONST(SQL_DECIMAL),
    MAKECONST(SQL_NUMERIC),
    MAKECONST(SQL_SMALLINT),
    MAKECONST(SQL_INTEGER),
    MAKECONST(SQL_REAL),
    MAKECONST(SQL_FLOAT),
    MAKECONST(SQL_DOUBLE),
    MAKECONST(SQL_BIT),
    MAKECONST(SQL_TINYINT),
    MAKECONST(SQL_BIGINT),
    MAKECONST(SQL_BINARY),
    MAKECONST(SQL_VARBINARY),
    MAKECONST(SQL_LONGVARBINARY),
    MAKECONST(SQL_TYPE_DATE),
    MAKECONST(SQL_TYPE_TIME),
    MAKECONST(SQL_TYPE_TIMESTAMP),
    //MAKECONST(SQL_SS_TIME2),
    //MAKECONST(SQL_SS_XML),
    MAKECONST(SQL_INTERVAL_MONTH),
    MAKECONST(SQL_INTERVAL_YEAR),
    MAKECONST(SQL_INTERVAL_YEAR_TO_MONTH),
    MAKECONST(SQL_INTERVAL_DAY),
    MAKECONST(SQL_INTERVAL_HOUR),
    MAKECONST(SQL_INTERVAL_MINUTE),
    MAKECONST(SQL_INTERVAL_SECOND),
    MAKECONST(SQL_INTERVAL_DAY_TO_HOUR),
    MAKECONST(SQL_INTERVAL_DAY_TO_MINUTE),
    MAKECONST(SQL_INTERVAL_DAY_TO_SECOND),
    MAKECONST(SQL_INTERVAL_HOUR_TO_MINUTE),
    MAKECONST(SQL_INTERVAL_HOUR_TO_SECOND),
    MAKECONST(SQL_INTERVAL_MINUTE_TO_SECOND),
    //MAKECONST(SQL_GUID),
    MAKECONST(SQL_NULLABLE),
    MAKECONST(SQL_NO_NULLS),
    MAKECONST(SQL_NULLABLE_UNKNOWN),
    // MAKECONST(SQL_INDEX_BTREE),
    // MAKECONST(SQL_INDEX_CLUSTERED),
    // MAKECONST(SQL_INDEX_CONTENT),
    // MAKECONST(SQL_INDEX_HASHED),
    // MAKECONST(SQL_INDEX_OTHER),
    MAKECONST(SQL_SCOPE_CURROW),
    MAKECONST(SQL_SCOPE_TRANSACTION),
    MAKECONST(SQL_SCOPE_SESSION),
    MAKECONST(SQL_PC_UNKNOWN),
    MAKECONST(SQL_PC_NOT_PSEUDO),
    MAKECONST(SQL_PC_PSEUDO),

    // SQLGetInfo
    MAKECONST(SQL_ACCESSIBLE_PROCEDURES),
    MAKECONST(SQL_ACCESSIBLE_TABLES),
    MAKECONST(SQL_ACTIVE_ENVIRONMENTS),
    MAKECONST(SQL_AGGREGATE_FUNCTIONS),
    MAKECONST(SQL_ALTER_DOMAIN),
    MAKECONST(SQL_ALTER_TABLE),
    MAKECONST(SQL_ASYNC_MODE),
    MAKECONST(SQL_BATCH_ROW_COUNT),
    MAKECONST(SQL_BATCH_SUPPORT),
    MAKECONST(SQL_BOOKMARK_PERSISTENCE),
    MAKECONST(SQL_CATALOG_LOCATION),
    MAKECONST(SQL_CATALOG_NAME),
    MAKECONST(SQL_CATALOG_NAME_SEPARATOR),
    MAKECONST(SQL_CATALOG_TERM),
    MAKECONST(SQL_CATALOG_USAGE),
    MAKECONST(SQL_COLLATION_SEQ),
    MAKECONST(SQL_COLUMN_ALIAS),
    MAKECONST(SQL_CONCAT_NULL_BEHAVIOR),
    MAKECONST(SQL_CONVERT_FUNCTIONS),
    MAKECONST(SQL_CONVERT_VARCHAR),
    MAKECONST(SQL_CORRELATION_NAME),
    MAKECONST(SQL_CREATE_ASSERTION),
    MAKECONST(SQL_CREATE_CHARACTER_SET),
    MAKECONST(SQL_CREATE_COLLATION),
    MAKECONST(SQL_CREATE_DOMAIN),
    MAKECONST(SQL_CREATE_SCHEMA),
    MAKECONST(SQL_CREATE_TABLE),
    MAKECONST(SQL_CREATE_TRANSLATION),
    MAKECONST(SQL_CREATE_VIEW),
    MAKECONST(SQL_CURSOR_COMMIT_BEHAVIOR),
    MAKECONST(SQL_CURSOR_ROLLBACK_BEHAVIOR),
    // MAKECONST(SQL_CURSOR_ROLLBACK_SQL_CURSOR_SENSITIVITY),
    MAKECONST(SQL_DATABASE_NAME),
    MAKECONST(SQL_DATA_SOURCE_NAME),
    MAKECONST(SQL_DATA_SOURCE_READ_ONLY),
    MAKECONST(SQL_DATETIME_LITERALS),
    MAKECONST(SQL_DBMS_NAME),
    MAKECONST(SQL_DBMS_VER),
    MAKECONST(SQL_DDL_INDEX),
    MAKECONST(SQL_DEFAULT_TXN_ISOLATION),
    MAKECONST(SQL_DESCRIBE_PARAMETER),
    MAKECONST(SQL_DM_VER),
    MAKECONST(SQL_DRIVER_HDESC),
    MAKECONST(SQL_DRIVER_HENV),
    MAKECONST(SQL_DRIVER_HLIB),
    MAKECONST(SQL_DRIVER_HSTMT),
    MAKECONST(SQL_DRIVER_NAME),
    MAKECONST(SQL_DRIVER_ODBC_VER),
    MAKECONST(SQL_DRIVER_VER),
    MAKECONST(SQL_DROP_ASSERTION),
    MAKECONST(SQL_DROP_CHARACTER_SET),
    MAKECONST(SQL_DROP_COLLATION),
    MAKECONST(SQL_DROP_DOMAIN),
    MAKECONST(SQL_DROP_SCHEMA),
    MAKECONST(SQL_DROP_TABLE),
    MAKECONST(SQL_DROP_TRANSLATION),
    MAKECONST(SQL_DROP_VIEW),
    MAKECONST(SQL_DYNAMIC_CURSOR_ATTRIBUTES1),
    MAKECONST(SQL_DYNAMIC_CURSOR_ATTRIBUTES2),
    MAKECONST(SQL_EXPRESSIONS_IN_ORDERBY),
    MAKECONST(SQL_FILE_USAGE),
    MAKECONST(SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES1),
    MAKECONST(SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES2),
    MAKECONST(SQL_GETDATA_EXTENSIONS),
    MAKECONST(SQL_GROUP_BY),
    MAKECONST(SQL_IDENTIFIER_CASE),
    MAKECONST(SQL_IDENTIFIER_QUOTE_CHAR),
    MAKECONST(SQL_INDEX_KEYWORDS),
    MAKECONST(SQL_INFO_SCHEMA_VIEWS),
    MAKECONST(SQL_INSERT_STATEMENT),
    MAKECONST(SQL_INTEGRITY),
    MAKECONST(SQL_KEYSET_CURSOR_ATTRIBUTES1),
    MAKECONST(SQL_KEYSET_CURSOR_ATTRIBUTES2),
    MAKECONST(SQL_KEYWORDS),
    MAKECONST(SQL_LIKE_ESCAPE_CLAUSE),
    MAKECONST(SQL_MAX_ASYNC_CONCURRENT_STATEMENTS),
    MAKECONST(SQL_MAX_BINARY_LITERAL_LEN),
    MAKECONST(SQL_MAX_CATALOG_NAME_LEN),
    MAKECONST(SQL_MAX_CHAR_LITERAL_LEN),
    MAKECONST(SQL_MAX_COLUMNS_IN_GROUP_BY),
    MAKECONST(SQL_MAX_COLUMNS_IN_INDEX),
    MAKECONST(SQL_MAX_COLUMNS_IN_ORDER_BY),
    MAKECONST(SQL_MAX_COLUMNS_IN_SELECT),
    MAKECONST(SQL_MAX_COLUMNS_IN_TABLE),
    MAKECONST(SQL_MAX_COLUMN_NAME_LEN),
    MAKECONST(SQL_MAX_CONCURRENT_ACTIVITIES),
    MAKECONST(SQL_MAX_CURSOR_NAME_LEN),
    MAKECONST(SQL_MAX_DRIVER_CONNECTIONS),
    MAKECONST(SQL_MAX_IDENTIFIER_LEN),
    MAKECONST(SQL_MAX_INDEX_SIZE),
    MAKECONST(SQL_MAX_PROCEDURE_NAME_LEN),
    MAKECONST(SQL_MAX_ROW_SIZE),
    MAKECONST(SQL_MAX_ROW_SIZE_INCLUDES_LONG),
    MAKECONST(SQL_MAX_SCHEMA_NAME_LEN),
    MAKECONST(SQL_MAX_STATEMENT_LEN),
    MAKECONST(SQL_MAX_TABLES_IN_SELECT),
    MAKECONST(SQL_MAX_TABLE_NAME_LEN),
    MAKECONST(SQL_MAX_USER_NAME_LEN),
    MAKECONST(SQL_MULTIPLE_ACTIVE_TXN),
    MAKECONST(SQL_MULT_RESULT_SETS),
    MAKECONST(SQL_NEED_LONG_DATA_LEN),
    MAKECONST(SQL_NON_NULLABLE_COLUMNS),
    MAKECONST(SQL_NULL_COLLATION),
    MAKECONST(SQL_NUMERIC_FUNCTIONS),
    MAKECONST(SQL_ODBC_INTERFACE_CONFORMANCE),
    MAKECONST(SQL_ODBC_VER),
    MAKECONST(SQL_OJ_CAPABILITIES),
    MAKECONST(SQL_ORDER_BY_COLUMNS_IN_SELECT),
    MAKECONST(SQL_PARAM_ARRAY_ROW_COUNTS),
    MAKECONST(SQL_PARAM_ARRAY_SELECTS),
    MAKECONST(SQL_PARAM_TYPE_UNKNOWN),
    MAKECONST(SQL_PARAM_INPUT),
    MAKECONST(SQL_PARAM_INPUT_OUTPUT),
    MAKECONST(SQL_PARAM_OUTPUT),
    MAKECONST(SQL_RETURN_VALUE),
    MAKECONST(SQL_RESULT_COL),
    MAKECONST(SQL_PROCEDURES),
    MAKECONST(SQL_PROCEDURE_TERM),
    MAKECONST(SQL_QUOTED_IDENTIFIER_CASE),
    MAKECONST(SQL_ROW_UPDATES),
    MAKECONST(SQL_SCHEMA_TERM),
    MAKECONST(SQL_SCHEMA_USAGE),
    MAKECONST(SQL_SCROLL_OPTIONS),
    MAKECONST(SQL_SEARCH_PATTERN_ESCAPE),
    MAKECONST(SQL_SERVER_NAME),
    MAKECONST(SQL_SPECIAL_CHARACTERS),
    MAKECONST(SQL_SQL92_DATETIME_FUNCTIONS),
    MAKECONST(SQL_SQL92_FOREIGN_KEY_DELETE_RULE),
    MAKECONST(SQL_SQL92_FOREIGN_KEY_UPDATE_RULE),
    MAKECONST(SQL_SQL92_GRANT),
    MAKECONST(SQL_SQL92_NUMERIC_VALUE_FUNCTIONS),
    MAKECONST(SQL_SQL92_PREDICATES),
    MAKECONST(SQL_SQL92_RELATIONAL_JOIN_OPERATORS),
    MAKECONST(SQL_SQL92_REVOKE),
    MAKECONST(SQL_SQL92_ROW_VALUE_CONSTRUCTOR),
    MAKECONST(SQL_SQL92_STRING_FUNCTIONS),
    MAKECONST(SQL_SQL92_VALUE_EXPRESSIONS),
    MAKECONST(SQL_SQL_CONFORMANCE),
    MAKECONST(SQL_STANDARD_CLI_CONFORMANCE),
    MAKECONST(SQL_STATIC_CURSOR_ATTRIBUTES1),
    MAKECONST(SQL_STATIC_CURSOR_ATTRIBUTES2),
    MAKECONST(SQL_STRING_FUNCTIONS),
    MAKECONST(SQL_SUBQUERIES),
    MAKECONST(SQL_SYSTEM_FUNCTIONS),
    MAKECONST(SQL_TABLE_TERM),
    MAKECONST(SQL_TIMEDATE_ADD_INTERVALS),
    MAKECONST(SQL_TIMEDATE_DIFF_INTERVALS),
    MAKECONST(SQL_TIMEDATE_FUNCTIONS),
    MAKECONST(SQL_TXN_CAPABLE),
    MAKECONST(SQL_TXN_ISOLATION_OPTION),
    MAKECONST(SQL_UNION),
    MAKECONST(SQL_USER_NAME),
    MAKECONST(SQL_XOPEN_CLI_YEAR),
};


static stlBool CreateExceptions()
{
    zlyExcInfo *info;
    PyObject   *classdict;
    PyObject   *doc;
    unsigned int i = 0;

    for (i = 0; i < _countof(gExcInfos); i++)
    {
        info = &gExcInfos[i];

        classdict = PyDict_New();
        STL_TRY( ZLY_IS_VALID_PYOBJECT(classdict) == STL_TRUE );

        doc = PyString_FromString(info->szDoc);
        STL_TRY_THROW( ZLY_IS_VALID_PYOBJECT(doc) == STL_TRUE,
                       RAMP_ERR_DEC_CLASSDICT );

        (void)PyDict_SetItemString(classdict, "__doc__", doc);
        zlymPyDecRef(doc);

        *info->ppexc = PyErr_NewException((char*)info->szFullName, *info->ppexcParent, classdict);
        STL_TRY_THROW( ZLY_IS_VALID_PYOBJECT(*info->ppexc) == STL_TRUE,
                       RAMP_ERR_DEC_CLASSDICT );

        // Keep a reference for our internal (C++) use.
        Py_INCREF(*info->ppexc);

        (void)PyModule_AddObject(pModule, (char*)info->szName, *info->ppexc);
    }

    return STL_TRUE;

    STL_CATCH( RAMP_ERR_DEC_CLASSDICT )
    {
        zlymPyDecRef(classdict);
    }
    STL_FINISH;

    return STL_FALSE;
}

#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "pygliese",                   // m_name
    module_doc,
    -1,                         // m_size
    pyodbc_methods,             // m_methods
    0,                          // m_reload
    0,                          // m_traverse
    0,                          // m_clear
    0,                          // m_free
    };
  #define MODRETURN(v) v
#else
  #define MODRETURN(v)
#endif

PyMODINIT_FUNC
#if PY_MAJOR_VERSION >= 3
PyInit_pygliese()
#else
initpygliese(void)
#endif
{
    PyObject  *module = NULL;
    const char* szVersion;
    unsigned int i = 0;
    PyObject* binary_type;

    ErrorInit();

    if (PyType_Ready(&ConnectionType) < 0 || PyType_Ready(&CursorType) < 0 || PyType_Ready(&RowType) < 0 || PyType_Ready(&CnxnInfoType) < 0)
        return MODRETURN(0);

#if PY_MAJOR_VERSION >= 3
    ZLY_ATTACH_PYOBJECT( module, PyModule_Create(&moduledef) );
#else
    ZLY_ATTACH_PYOBJECT( module, Py_InitModule4("pygliese", pyodbc_methods, module_doc, NULL, PYTHON_API_VERSION) );
#endif

    pModule = module;

    if (!module || !import_types() || !CreateExceptions())
        return MODRETURN(0);

    init_locale_info();

    szVersion = TOSTRING(PYODBC_VERSION);
    (void)PyModule_AddStringConstant(module, "version", (char*)szVersion);

    (void)PyModule_AddIntConstant(module, "threadsafety", 1);
    (void)PyModule_AddStringConstant(module, "apilevel", "2.0");
    (void)PyModule_AddStringConstant(module, "paramstyle", "qmark");
    (void)PyModule_AddObject(module, "pooling", Py_True);
    Py_INCREF(Py_True);
    (void)PyModule_AddObject(module, "lowercase", Py_False);
    Py_INCREF(Py_False);

    (void)PyModule_AddObject(module, "Connection", (PyObject*)&ConnectionType);
    Py_INCREF((PyObject*)&ConnectionType);
    (void)PyModule_AddObject(module, "Cursor", (PyObject*)&CursorType);
    Py_INCREF((PyObject*)&CursorType);
    (void)PyModule_AddObject(module, "Row", (PyObject*)&RowType);
    Py_INCREF((PyObject*)&RowType);

    // Add the SQL_XXX defines from ODBC.
    for (i = 0; i < _countof(aConstants); i++)
        (void)PyModule_AddIntConstant(module, (char*)aConstants[i].szName, aConstants[i].value);

    (void)PyModule_AddObject(module, "Date", (PyObject*)PyDateTimeAPI->DateType);
    Py_INCREF((PyObject*)PyDateTimeAPI->DateType);
    (void)PyModule_AddObject(module, "Time", (PyObject*)PyDateTimeAPI->TimeType);
    Py_INCREF((PyObject*)PyDateTimeAPI->TimeType);
    (void)PyModule_AddObject(module, "Timestamp", (PyObject*)PyDateTimeAPI->DateTimeType);
    Py_INCREF((PyObject*)PyDateTimeAPI->DateTimeType);
    (void)PyModule_AddObject(module, "DATETIME", (PyObject*)PyDateTimeAPI->DateTimeType);
    Py_INCREF((PyObject*)PyDateTimeAPI->DateTimeType);
    (void)PyModule_AddObject(module, "STRING", (PyObject*)&PyString_Type);
    Py_INCREF((PyObject*)&PyString_Type);
    (void)PyModule_AddObject(module, "NUMBER", (PyObject*)&PyFloat_Type);
    Py_INCREF((PyObject*)&PyFloat_Type);
    (void)PyModule_AddObject(module, "ROWID", (PyObject*)&PyInt_Type);
    Py_INCREF((PyObject*)&PyInt_Type);

#if PY_VERSION_HEX >= 0x02060000
    binary_type = (PyObject*)&PyByteArray_Type;
#else
    binary_type = (PyObject*)&PyBuffer_Type;
#endif
    (void)PyModule_AddObject(module, "BINARY", binary_type);
    Py_INCREF(binary_type);
    (void)PyModule_AddObject(module, "Binary", binary_type);
    Py_INCREF(binary_type);

    I(null_binary != 0);        // must be initialized first
    (void)PyModule_AddObject(module, "BinaryNull", null_binary);

    (void)PyModule_AddIntConstant(module, "UNICODE_SIZE", sizeof(Py_UNICODE));
    (void)PyModule_AddIntConstant(module, "SQLWCHAR_SIZE", sizeof(SQLWCHAR));

    if (!PyErr_Occurred())
    {
        module = NULL;
    }
    else
    {
        ErrorCleanup();
    }

    return MODRETURN(pModule);
}

#ifdef WINVER
BOOL WINAPI DllMain(
  HINSTANCE hMod,
  DWORD fdwReason,
  LPVOID lpvReserved
  )
{
    UNUSED(NULL, hMod, fdwReason, lpvReserved);
    return STL_TRUE;
}
#endif


static PyObject* MakeConnectionString(PyObject* existing, PyObject* parts)
{
    Py_ssize_t length = 0;
    Py_ssize_t pos = 0;
    PyObject* key = 0;
    PyObject* value = 0;
    PyObject* result;
    Py_UNICODE* buffer;
    Py_ssize_t offset;


    // Creates a connection string from an optional existing connection string plus a dictionary of keyword value
    // pairs.
    //
    // existing
    //   Optional Unicode connection string we will be appending to.  Used when a partial connection string is passed
    //   in, followed by keyword parameters:
    //
    //   connect("driver={x};database={y}", user='z')
    //
    // parts
    //   A dictionary of text keywords and text values that will be appended.

    I(PyUnicode_Check(existing));

    length = 0;      // length in *characters*
    if (existing)
        length = Text_Size(existing) + 1; // + 1 to add a trailing semicolon

    while (PyDict_Next(parts, &pos, &key, &value))
    {
        length += Text_Size(key) + 1 + Text_Size(value) + 1; // key=value;
    }

    result = PyUnicode_FromUnicode(0, length);
    if (!result)
        return 0;

    buffer = PyUnicode_AS_UNICODE(result);
    offset = 0;

    if (existing)
    {
        offset += TextCopyToUnicode(&buffer[offset], existing);
        buffer[offset++] = (Py_UNICODE)';';
    }

    pos = 0;
    while (PyDict_Next(parts, &pos, &key, &value))
    {
        offset += TextCopyToUnicode(&buffer[offset], key);
        buffer[offset++] = (Py_UNICODE)'=';

        offset += TextCopyToUnicode(&buffer[offset], value);
        buffer[offset++] = (Py_UNICODE)';';
    }

    I(offset == length);

    return result;
}

/**
 * @}
 */
