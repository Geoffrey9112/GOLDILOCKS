/*******************************************************************************
 * zlyDef.h
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

#ifndef _ZLYDEF_H_
#define _ZLYDEF_H_ 1

/**
 * @file zlyDef.h
 * @brief Gliese Python Database API definitions
 */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <malloc.h>
#else
#define _strcmpi strcasecmp
#ifdef __MINGW32__
  #include <windef.h>
  #include <malloc.h>
#else
  inline int max(int lhs, int rhs) { return (rhs > lhs) ? rhs : lhs; }
#endif
#endif

#ifdef __SUN__
#include <alloca.h>
#endif

#define PY_SSIZE_T_CLEAN 1

#include <Python.h>
#include <floatobject.h>
#include <longobject.h>
#include <boolobject.h>
#include <unicodeobject.h>
#include <structmember.h>

#include <dtl.h>
#include <goldilocks.h>
#include <goldilockstypes.h>

/*
#include <sql.h>
#include <sqlext.h>
*/

#if PY_VERSION_HEX < 0x02050000 && !defined(PY_SSIZE_T_MIN)
typedef int Py_ssize_t;
#define PY_SSIZE_T_MAX INT_MAX
#define PY_SSIZE_T_MIN INT_MIN
#define PyInt_AsSsize_t PyInt_AsLong
#define lenfunc inquiry
#define ssizeargfunc intargfunc
#define ssizeobjargproc intobjargproc
#endif

#ifndef _countof
#define _countof(a) (sizeof(a) / sizeof(a[0]))
#endif

#include <stdarg.h>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

// Building an actual debug version of Python is so much of a pain that it never happens.  I'm providing release-build
// versions of assertions.

#if defined(PYODBC_ASSERT) && defined(_MSC_VER)
  #include <crtdbg.h>
  inline void FailAssert(const char* szFile, size_t line, const char* szExpr)
  {
      printf("assertion failed: %s(%d)\n%s\n", szFile, line, szExpr);
      __debugbreak(); // _CrtDbgBreak();
  }
  #define I(expr) if (!(expr)) FailAssert(__FILE__, __LINE__, #expr);
  #define N(expr) if (expr) FailAssert(__FILE__, __LINE__, #expr);
#else
  #define I(expr)
  #define N(expr)
#endif

#define TRACE DebugTrace

#ifdef PYODBC_LEAK_CHECK
#define pyodbc_malloc(len) _pyodbc_malloc(__FILE__, __LINE__, len)
void* _pyodbc_malloc(const char* filename, int lineno, size_t len);
void pyodbc_free(void* p);
void pyodbc_leak_check();
#else
#define pyodbc_malloc malloc
#define pyodbc_free free
#endif

#include <zlymCompat.h>

#define HERE printf("%s(%d)\n", __FILE__, __LINE__)

/*********************************************/
/**
 * @brief Miscellaneous
 */

/**
 * @brief extern variables declaration
 */

extern PyObject* Error;
extern PyObject* Warning;
extern PyObject* InterfaceError;
extern PyObject* DatabaseError;
extern PyObject* InternalError;
extern PyObject* OperationalError;
extern PyObject* ProgrammingError;
extern PyObject* IntegrityError;
extern PyObject* DataError;
extern PyObject* NotSupportedError;

extern PyObject* null_binary;

extern PyObject* decimal_type;

extern HENV henv;

extern PyTypeObject RowType;
extern PyTypeObject CursorType;
extern PyTypeObject ConnectionType;
extern PyTypeObject CnxnInfoType;

// Thd pyodbc module.
extern PyObject* pModule;
extern Py_UNICODE chDecimal;

/**
 * @brief Module
 */

typedef struct zlyExcInfo
{
    const char* szName;
    const char* szFullName;
    PyObject** ppexc;
    PyObject** ppexcParent;
    const char* szDoc;
} zlyExcInfo;

typedef struct zlyKeywordMap
{
    const stlChar *mOldName;
    const stlChar *mNewName;
    PyObject      *mNewNameObject;    // PyString object version of newname, created as needed.
} zlyKeywordMap;

#define MAKECONST(v) { #v, v }

typedef struct zlyConstantDef
{
    const char* szName;
    int value;
} zlyConstantDef;

/**
 * @brief Connections
 */

typedef enum zlyGITypeEnum
{
    GI_YESNO,
    GI_STRING,
    GI_UINTEGER,
    GI_USMALLINT,
} zlyGITypeEnum;

typedef struct zlyGetInfoType
{
    SQLUSMALLINT  infotype;
    stlInt32      datatype; // GI_XXX
} zlyGetInfoType;

typedef struct zlyConnection
{
    PyObject_HEAD

    // Set to SQL_NULL_HANDLE when the connection is closed.
	HDBC hdbc;

    // Will be SQL_AUTOCOMMIT_ON or SQL_AUTOCOMMIT_OFF.
    uintptr_t nAutoCommit;

    // The ODBC version the driver supports, from SQLGetInfo(DRIVER_ODBC_VER).  This is set after connecting.
    stlChar odbc_major;
    stlChar odbc_minor;

    // The escape character from SQLGetInfo.  This is not initialized until requested, so this may be zero!
    PyObject* searchescape;

    // Will be true if SQLDescribeParam is supported.  If false, we'll have to guess but the user will not be able
    // to insert NULLs into binary columns.
    stlBool supports_describeparam;

    // The column size of datetime columns, obtained from SQLGetInfo(), used to determine the datetime precision.
    stlInt32 datetime_precision;

    // If true, then the strings in the rows are returned as unicode objects.
    stlBool unicode_results;

    // The connection timeout in seconds.
    intptr_t timeout;

    // These are copied from cnxn info for performance and convenience.

    stlInt32 varchar_maxlength;
    stlInt32 wvarchar_maxlength;
    stlInt32 binary_maxlength;

    // Output conversions.  Maps from SQL type in conv_types to the converter function in conv_funcs.
    //
    // If conv_count is zero, conv_types and conv_funcs will also be zero.
    //
    // pyodbc uses this manual mapping for speed and portability.  The STL collection classes use the new operator and
    // throw exceptions when out of memory.  pyodbc does not use any exceptions.

    stlInt32 conv_count;             // how many items are in conv_types and conv_funcs.
    SQLSMALLINT* conv_types;            // array of SQL_TYPEs to convert
    PyObject** conv_funcs;      // array of Python functions
} zlyConnection;

typedef struct zlyCnxnInfo
{
    PyObject_HEAD

    // The description of these fields is in the connection structure.

    stlChar odbc_major;
    stlChar odbc_minor;

    stlBool supports_describeparam;
    stlInt32 datetime_precision;

    // These are from SQLGetTypeInfo.column_size, so the char ones are in characters, not bytes.
    stlInt32 varchar_maxlength;
    stlInt32 wvarchar_maxlength;
    stlInt32 binary_maxlength;
} zlyCnxnInfo;

/**
 * @brief Parameters
 */

typedef struct zlyColumnInfo
{
    SQLSMALLINT sql_type;

    // The column size from SQLDescribeCol.  For character types, this is the maximum length, not including the NULL
    // terminator.  For binary values, this is the maximum length.  For numeric and decimal values, it is the defined
    // number of digits. For example, the precision of a column defined as NUMERIC(10,3) is 10.
    //
    // This value can be SQL_NO_TOTAL in which case the driver doesn't know the maximum length, such as for LONGVARCHAR
    // fields.
    SQLULEN column_size;

    // Tells us if an integer type is signed or unsigned.  This is determined after a query using SQLColAttribute.  All
    // of the integer types are the same size whether signed and unsigned, so we can allocate memory ahead of time
    // without knowing this.  We use this during the fetch when converting to a Python integer or long.
    stlBool is_unsigned;
} zlyColumnInfo;

typedef struct zlyParamInfo
{
    // The following correspond to the SQLBindParameter parameters.
    SQLSMALLINT ValueType;
    SQLSMALLINT ParameterType;
    SQLULEN     ColumnSize;
    SQLSMALLINT DecimalDigits;

    // The value pointer that will be bound.  If `alloc` is true, this was allocated with malloc and must be freed.
    // Otherwise it is zero or points into memory owned by the original Python parameter.
    SQLPOINTER ParameterValuePtr;

    SQLLEN BufferLength;
    SQLLEN StrLen_or_Ind;

    // If true, the memory in ParameterValuePtr was allocated via malloc and must be freed.
    stlBool allocated;

    // The python object containing the parameter value.  A reference to this object should be held until we have
    // finished using memory owned by it.
    PyObject* pParam;

    // Optional data.  If used, ParameterValuePtr will point into this.
    union
    {
        unsigned char ch;
        long l;
        stlInt64 i64;
        double dbl;
        TIMESTAMP_STRUCT timestamp;
        DATE_STRUCT date;
        TIME_STRUCT time;
    } Data;
} zlyParamInfo;

typedef struct zlyNullParam
{
    PyObject_HEAD
} zlyNullParam;

/**
 * @brief Cursor
 */

typedef struct zlyCursor
{
    PyObject_HEAD

    // The Connection object (which is a PyObject) that created this cursor.
    zlyConnection *cnxn;

    // Set to SQL_NULL_HANDLE when the cursor is closed.
    HSTMT hstmt;

    //
    // SQL Parameters
    //

    // If non-zero, a pointer to the previously prepared SQL string, allowing us to skip the prepare and gathering of
    // parameter data.
    PyObject* pPreparedSQL;

    // The number of parameter markers in pPreparedSQL.  This will be zero when pPreparedSQL is zero but is set
    // immediately after preparing the SQL.
    stlInt32 paramcount;

    // If non-zero, a pointer to an array of SQL type values allocated via malloc.  This is zero until we actually ask
    // for the type of parameter, which is only when a parameter is None (NULL).  At that point, the entire array is
    // allocated (length == paramcount) but all entries are set to SQL_UNKNOWN_TYPE.
    SQLSMALLINT* paramtypes;

    // If non-zero, a pointer to a buffer containing the actual parameters bound.  If pPreparedSQL is zero, this should
    // be freed using free and set to zero.
    //
    // Even if the same SQL statement is executed twice, the parameter bindings are redone from scratch since we try to
    // bind into the Python objects directly.
    zlyParamInfo* paramInfos;

    //
    // Result Information
    //

    // An array of ColumnInfos, allocated via malloc.  This will be zero when closed or when there are no query
    // results.
    zlyColumnInfo* colinfos;

    // The description tuple described in the DB API 2.0 specification.  Set to None when there are no results.
    PyObject* description;

    stlInt32 arraysize;

    // The Cursor.rowcount attribute from the DB API specification.
    stlInt32 rowcount;

    // A dictionary that maps from column name (PyString) to index into the result columns (PyInteger).  This is
    // constructued during an execute and shared with each row (reference counted) to implement accessing results by
    // column name.
    //
    // This duplicates some ODBC functionality, but allows us to use Row objects after the statement is closed and
    // should use less memory than putting each column into the Row's __dict__.
    //
    // Since this is shared by Row objects, it cannot be reused.  New dictionaries are created for every execute.  This
    // will be zero whenever there are no results.
    PyObject* map_name_to_index;
} zlyCursor;

typedef enum zlyCursorRequireEnum
{
    CURSOR_REQUIRE_CNXN    = 0x00000001,
    CURSOR_REQUIRE_OPEN    = 0x00000003, // includes _CNXN
    CURSOR_REQUIRE_RESULTS = 0x00000007, // includes _OPEN
    CURSOR_RAISE_ERROR     = 0x00000010,
} zlyCursorRequireEnum;

/**
 * @brief Row
 */

typedef struct zlyRow
{
    // A Row must act like a sequence (a tuple of results) to meet the DB API specification, but we also allow values
    // to be accessed via lowercased column names.  We also supply a `columns` attribute which returns the list of
    // column names.

    PyObject_HEAD

    // cursor.description, accessed as _description
    PyObject* description;

    // A Python dictionary mapping from column name to a PyInteger, used to access columns by name.
    PyObject* map_name_to_index;

    // The number of values in apValues.
    Py_ssize_t cValues;
    // The column values, stored as an array.
    PyObject** apValues;
} zlyRow;

#define Row_Check(op) PyObject_TypeCheck(op, &RowType)
#define Row_CheckExact(op) (Py_TYPE(op) == &RowType)

/**
 * @brief Buffer
 */

typedef struct zlyBufSegIterator
{
    PyObject   *pBuffer;
    Py_ssize_t  iSegment;
    Py_ssize_t  cSegments;
} zlyBufSegIterator;

/**
 * @brief Get Data
 */

typedef struct zlyDataBuffer
{
    SQLSMALLINT  dataType;

    stlChar     *buffer;
    Py_ssize_t   bufferSize;      // How big is the buffer.
    stlInt32     bytesUsed;       // How many elements have been read into the buffer?

    PyObject    *bufferOwner;     // If possible, we bind into a PyString, PyUnicode, or PyByteArray object.
    stlInt32     element_size;    // How wide is each character: ASCII/ANSI -> 1, Unicode -> 2 or 4, binary -> 1

    stlInt32     null_size;       // How much room, in bytes, to add for null terminator: binary -> 0, other -> same as a element_size
    stlBool      usingStack;      // Is buffer pointing to the initial stack buffer?
} zlyDataBuffer;

#endif /* _ZLYDEF_H_ */
