/*******************************************************************************
 * zlplDef.h
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

#ifndef _ZLPLDEF_H_
#define _ZLPLDEF_H_ 1

/**
 * @file zlplDef.h
 * @brief Gliese Embedded SQL in C precompiler library internal definition header file.
 */

#include <stl.h>
#include <goldilocks.h>
#include <goldilocksesql.h>
#include <sqlca.h>

/**
 * @defgroup  zlplCursorProperty  Cursor Property
 * @ingroup zlplCursor
 * @{
 */

/**
 * @brief Cursor Sensitivity
 */
/* ellCursorSensitivity과 맞춰야 함 */
typedef enum zlplCursorSensitivity
{
    ZLPL_CURSOR_SENSITIVITY_NA = 0,        /**< N/A */

    ZLPL_CURSOR_SENSITIVITY_INSENSITIVE,  /**< INSENSITIVE */
    ZLPL_CURSOR_SENSITIVITY_SENSITIVE,    /**< SENSITIVE */
    ZLPL_CURSOR_SENSITIVITY_ASENSITIVE,   /**< ASENSITIVE */

    ZLPL_CURSOR_SENSITIVITY_MAX
} zlplCursorSensitivity;

#define ZLPL_CURSOR_DEFAULT_SENSITIVITY        (ZLPL_CURSOR_SENSITIVITY_INSENSITIVE)


/**
 * @brief Cursor Scrollability
 */
/* ellCursorScrollability과 맞춰야 함 */
typedef enum zlplCursorScrollability
{
    ZLPL_CURSOR_SCROLLABILITY_NA = 0,          /**< N/A */

    ZLPL_CURSOR_SCROLLABILITY_NO_SCROLL,       /**< NO SCROLL */
    ZLPL_CURSOR_SCROLLABILITY_SCROLL,          /**< SCROLL */

    ZLPL_CURSOR_SCROLLABILITY_MAX
} zlplCursorScrollability;

#define ZLPL_CURSOR_DEFAULT_SCROLLABILITY      (ZLPL_CURSOR_SCROLLABILITY_NO_SCROLL)

/**
 * @brief Cursor Holdability
 */
/* ellCursorHoldability과 맞춰야 함 */
typedef enum zlplCursorHoldability
{
    ZLPL_CURSOR_HOLDABILITY_NA = 0,          /**< N/A */

    ZLPL_CURSOR_HOLDABILITY_WITH_HOLD,       /**< WITH HOLD */
    ZLPL_CURSOR_HOLDABILITY_WITHOUT_HOLD,    /**< WITHOUT HOLD */

    ZLPL_CURSOR_HOLDABILITY_MAX
} zlplCursorHoldability;

/**
 * @brief Cursor Updatability
 */
/* ellCursorUpdatability과 맞춰야 함 */
typedef enum zlplCursorUpdatability
{
    ZLPL_CURSOR_UPDATABILITY_NA = 0,          /**< N/A */

    ZLPL_CURSOR_UPDATABILITY_FOR_READ_ONLY,   /**< FOR_READ_ONLY */
    ZLPL_CURSOR_UPDATABILITY_FOR_UPDATE,      /**< FOR_UPDATE */

    ZLPL_CURSOR_UPDATABILITY_MAX
} zlplCursorUpdatability;

#define ZLPL_CURSOR_DEFAULT_UPDATABILITY      (ZLPL_CURSOR_UPDATABILITY_FOR_READ_ONLY)


/**
 * @brief Cursor Returnability
 */
/* ellCursorReturnability과 맞춰야 함 */
typedef enum zlplCursorReturnability
{
    ZLPL_CURSOR_RETURNABILITY_NA = 0,          /**< N/A */

    ZLPL_CURSOR_RETURNABILITY_WITH_RETURN,     /**< WITH RETURN */
    ZLPL_CURSOR_RETURNABILITY_WITHOUT_RETURN,  /**< WITHOUT RETURN */

    ZLPL_CURSOR_RETURNABILITY_MAX
} zlplCursorReturnability;

#define ZLPL_CURSOR_DEFAULT_RETURNABILITY      (ZLPL_CURSOR_RETURNABILITY_WITHOUT_RETURN)

/** @} */

/**
 * @addtogroup zlplCursor
 * @{
 */
/**
 * @brief Cursor Origin Type
 */
/* ellCursorOriginType과 맞춰야 함 */
typedef enum zlplCursorOriginType
{
    ZLPL_CURSOR_ORIGIN_NA = 0,                     /**< N/A */

    ZLPL_CURSOR_ORIGIN_STANDING_CURSOR,            /**< Standing Cursor */
    ZLPL_CURSOR_ORIGIN_DECLARED_DYNAMIC_CURSOR,    /**< Declared Dynamic Cursor */
    ZLPL_CURSOR_ORIGIN_EXTENDED_STATEMENT_CURSOR,  /**< Extended Statement Cursor */
    ZLPL_CURSOR_ORIGIN_RECEIVED_CURSOR,            /**< Received Cursror */

    ZLPL_CURSOR_ORIGIN_MAX

} zlplCursorOriginType;


/**
 * @brief Cursor Standard Type
 */
/* ellCursorStandardType과 맞춰야 함 */
typedef enum zlplCursorStandardType
{
    ZLPL_CURSOR_STANDARD_NA = 0,                  /**< N/A */

    ZLPL_CURSOR_STANDARD_ISO,                     /**< ISO  */
    ZLPL_CURSOR_STANDARD_ODBC_STATIC,             /**< ODBC STATIC */
    ZLPL_CURSOR_STANDARD_ODBC_KEYSET,             /**< ODBC KEYSET */
    ZLPL_CURSOR_STANDARD_ODBC_DYNAMIC,            /**< ODBC DYNAMIC */

    ZLPL_CURSOR_STANDARD_MAX

} zlplCursorStandardType;


/**
 * @brief Fetch Orientation 유형
 */
typedef enum zlplFetchOrientation
{
    ZLPL_FETCH_NA = 0,                          /**< N/A */

    ZLPL_FETCH_NEXT,
    ZLPL_FETCH_PRIOR,
    ZLPL_FETCH_FIRST,
    ZLPL_FETCH_LAST,
    ZLPL_FETCH_CURRENT,
    ZLPL_FETCH_ABSOLUTE,
    ZLPL_FETCH_RELATIVE,

    ZLPL_FETCH_MAX

} zlplFetchOrientation;

/** @} */

/**
 * @defgroup GOLDILOCKSESQL Embedded SQL Execution Library
 * @{
 */

#define ZLPL_SUCCESS                       (0)
#define ZLPL_FAILURE                       (-1)

#define ZLPL_REGION_INIT_SIZE              (32*1024)  /* 32K */
#define ZLPL_REGION_NEXT_SIZE              (8192)       /* 8K */

#define ZLPL_DYNAMIC_INIT_SIZE              (32*1024)  /* 32K */
#define ZLPL_DYNAMIC_NEXT_SIZE              (8192)       /* 8K */

#define ZLPL_MAX_CONN_STRING_LENGTH        (1024)       /* 1K */

#define ZLPL_CURSORHASH_BUCKET_COUNT          (127)
#define ZLPL_SQLSTMTHASH_BUCKET_COUNT         (127)
#define ZLPL_DYNAMIC_STMT_HASH_BUCKET_COUNT   (127)
#define ZLPL_CONN_STRING_HASH_BUCKET_COUNT    (127)

#define ZLPL_ALIGN(aSize)                  STL_ALIGN_DEFAULT(aSize)

#define ZLPL_MARK_WARNING(aSqlca, aIdx)    ((aSqlca)->sqlwarn[(aIdx)] = 'W')
#define ZLPL_SET_WARNING_GENERAL(aSqlca)   (ZLPL_MARK_WARNING(aSqlca, 0))

#define ZLPL_INIT_CONTEXT(aSqlContext)                                  \
    {                                                                   \
        (aSqlContext)->mConnInfo.mDbc = SQL_NULL_HANDLE;                \
        STL_INIT_ERROR_STACK(&(aSqlContext)->mErrorStack);              \
        (aSqlContext)->mSqlca = NULL;                                   \
        (aSqlContext)->mCursorSymTab = NULL;                            \
        (aSqlContext)->mQuerySymTab = NULL;                             \
        (aSqlContext)->mDynStmtSymTab = NULL;                           \
        (aSqlContext)->mConnName[0] = '\0';                             \
        (aSqlContext)->mIsConnected = STL_FALSE;                        \
        (aSqlContext)->mInList = STL_FALSE;                             \
        stlInitSpinLock(&(aSqlContext)->mCtxtSpinLock);                 \
        STL_RING_INIT_DATALINK((aSqlContext), STL_OFFSETOF(zlplSqlContext, mListLink)); \
        STL_RING_INIT_DATALINK((aSqlContext), STL_OFFSETOF(zlplSqlContext, mHashLink)); \
    }

#define ZLPL_COPY_HOST_VAR(aDest, aSrc)             \
    {                                               \
        (aDest)->io_type = (aSrc)->io_type;         \
        (aDest)->data_type = (aSrc)->data_type;     \
        (aDest)->arr_size = (aSrc)->arr_size;       \
        (aDest)->el_size = (aSrc)->el_size;         \
        (aDest)->buflen = (aSrc)->buflen;           \
        (aDest)->precision = (aSrc)->precision;     \
        (aDest)->scale = (aSrc)->scale;             \
        (aDest)->value = (aSrc)->value;             \
        (aDest)->indicator = (aSrc)->indicator;     \
        (aDest)->ind_el_size = (aSrc)->ind_el_size; \
        (aDest)->ind_type = (aSrc)->ind_type;       \
    }

#define ZLPL_EQUAL_HOST_VAR(aDest, aSrc)                            \
    ((aDest)->value == (aSrc)->value ? STL_TRUE : STL_FALSE)

#define ZLPL_CTXT_ERROR_STACK(aSqlContext) (&(aSqlContext)->mErrorStack)
#define ZLPL_CTXT_ALLOCATOR(aSqlContext)   (&(aSqlContext)->mAllocator)
#define ZLPL_CTXT_CONNECTION(aSqlContext)  (&(aSqlContext)->mConnInfo)

#define INIT_SQLSTATE(aSqlState)                                \
    {                                                           \
        stlMemset((aSqlState), 0, 6);                           \
    }

#define INIT_SQLCA(aSqlca)                                      \
    {                                                           \
        stlMemcpy((aSqlca), &sqlca_init, STL_SIZEOF(sqlca_t));  \
    }

#define ZLPL_INIT_SQLCA(aSqlContext, aSqlca)                \
    {                                                       \
        INIT_SQLCA(aSqlca);                                 \
        STL_INIT_ERROR_STACK(&(aSqlContext)->mErrorStack);  \
        (aSqlContext)->mSqlca = aSqlca;                     \
    }

#define ZLPL_INIT_CURSOR_SYMBOL(aSymbol)                                \
    {                                                                   \
        (aSymbol)->mCursorName[0] = '\0';                               \
        (aSymbol)->mStmtHandle = SQL_NULL_HANDLE;                       \
        (aSymbol)->mQuery = NULL;                                       \
        (aSymbol)->mNext = NULL;                                        \
    }

typedef enum
{
    ZLPL_STMT_NA = 0,

    ZLPL_STMT_SELECT,
    ZLPL_STMT_SELECT_INTO,
    ZLPL_STMT_QUERY_EXPR,
    ZLPL_STMT_QUERY_EXPR_INTO,
    ZLPL_STMT_POSITIONED_CURSOR_DML,
    ZLPL_STMT_INSERT,
    ZLPL_STMT_UPDATE,
    ZLPL_STMT_DELETE,
    ZLPL_STMT_CREATE,
    ZLPL_STMT_DROP,
    ZLPL_STMT_ALTER,
    ZLPL_STMT_TRUNCATE,
    ZLPL_STMT_COMMIT,
    ZLPL_STMT_ROLLBACK,
    ZLPL_STMT_RELEASE,
    ZLPL_STMT_GRANT,
    ZLPL_STMT_REVOKE,
    ZLPL_STMT_SET,
    ZLPL_STMT_DECLARE_STATEMENT,
    ZLPL_STMT_DECLARE_CURSOR,
    ZLPL_STMT_OPEN_CURSOR,
    ZLPL_STMT_CLOSE_CURSOR,
    ZLPL_STMT_FETCH_CURSOR,
    ZLPL_STMT_COPY,
    ZLPL_STMT_LOCK,
    ZLPL_STMT_COMMENT,
    ZLPL_STMT_CALL,
    /*
     * Dynamic SQL
     */
    ZTPL_STMT_DYNAMIC_SQL_EXECUTE_IMMEDIATE,
    ZTPL_STMT_DYNAMIC_SQL_PREPARE,
    ZTPL_STMT_DYNAMIC_SQL_EXECUTE,

    ZLPL_STMT_MAX
} zlplStatementType;

typedef enum
{
    ZLPL_PARAM_IO_TYPE_IN = 0,
    ZLPL_PARAM_IO_TYPE_OUT,
    ZLPL_PARAM_IO_TYPE_INOUT,
    ZLPL_PARAM_IO_TYPE_MAX
} zlplParamIOType;

/*
 * 아래 항목이 변경될 경우 mapping을 맞춰야 할 것들:
 *   gInCDataType(zlpbBind.c)
 *   gOutCDataType(zlpbBind.c)
 *   gSqlType(zlpbBind.c)
 *   gRefineDTFunc(ztpdRefine.c)
 *   gRefineDTHostVarFunc(ztpdRefineHostVar.c)
 *   gCvtDTFunc(ztpdConvert.c)
 *   gPrecision(ztpdType.c)
 *   gScale(ztpdType.c)
 */
typedef enum
{
    ZLPL_C_DATATYPE_UNKNOWN,  /* = 0 */
    /* C native type */
    ZLPL_C_DATATYPE_SCHAR,
    ZLPL_C_DATATYPE_UCHAR,
    ZLPL_C_DATATYPE_SSHORT,
    ZLPL_C_DATATYPE_USHORT,
    ZLPL_C_DATATYPE_SINT,
    ZLPL_C_DATATYPE_UINT,
    ZLPL_C_DATATYPE_SLONG,
    ZLPL_C_DATATYPE_ULONG,
    ZLPL_C_DATATYPE_SLONGLONG,
    ZLPL_C_DATATYPE_ULONGLONG,  /* = 10 */
    ZLPL_C_DATATYPE_FLOAT,
    ZLPL_C_DATATYPE_DOUBLE,
    ZLPL_C_DATATYPE_CHAR,
    /* Gliese defined type */
    ZLPL_C_DATATYPE_VARCHAR,
    ZLPL_C_DATATYPE_LONGVARCHAR,
    ZLPL_C_DATATYPE_BINARY,
    ZLPL_C_DATATYPE_VARBINARY,
    ZLPL_C_DATATYPE_LONGVARBINARY,
    ZLPL_C_DATATYPE_BOOLEAN,
    ZLPL_C_DATATYPE_NUMBER,    /* = 20 */
    ZLPL_C_DATATYPE_DATE,
    ZLPL_C_DATATYPE_TIME,
    ZLPL_C_DATATYPE_TIMESTAMP,
    ZLPL_C_DATATYPE_TIMETZ,
    ZLPL_C_DATATYPE_TIMESTAMPTZ,
    ZLPL_C_DATATYPE_INTERVAL_YEAR,
    ZLPL_C_DATATYPE_INTERVAL_MONTH,
    ZLPL_C_DATATYPE_INTERVAL_DAY,
    ZLPL_C_DATATYPE_INTERVAL_HOUR,
    ZLPL_C_DATATYPE_INTERVAL_MINUTE,  /* = 30 */
    ZLPL_C_DATATYPE_INTERVAL_SECOND,
    ZLPL_C_DATATYPE_INTERVAL_YEAR_TO_MONTH,
    ZLPL_C_DATATYPE_INTERVAL_DAY_TO_HOUR,
    ZLPL_C_DATATYPE_INTERVAL_DAY_TO_MINUTE,
    ZLPL_C_DATATYPE_INTERVAL_DAY_TO_SECOND,
    ZLPL_C_DATATYPE_INTERVAL_HOUR_TO_MINUTE,
    ZLPL_C_DATATYPE_INTERVAL_HOUR_TO_SECOND,
    ZLPL_C_DATATYPE_INTERVAL_MINUTE_TO_SECOND,
    ZLPL_C_DATATYPE_SQL_CONTEXT,
    ZLPL_C_DATATYPE_STRUCT,  /* = 40 */
    ZLPL_C_DATATYPE_MAX
} zlplCDataType;

#define ZLPL_IS_STRING_TYPE(aDataType)              \
    ( ((aDataType) == ZLPL_C_DATATYPE_SCHAR) ||     \
      ((aDataType) == ZLPL_C_DATATYPE_UCHAR) ||     \
      ((aDataType) == ZLPL_C_DATATYPE_CHAR) ||      \
      ((aDataType) == ZLPL_C_DATATYPE_VARCHAR) ) ?  \
    STL_TRUE : STL_FALSE

#define ZLPL_IS_INTEGER_TYPE(aDataType)                 \
    ( ((aDataType) == ZLPL_C_DATATYPE_SCHAR) ||         \
      ((aDataType) == ZLPL_C_DATATYPE_UCHAR) ||         \
      ((aDataType) == ZLPL_C_DATATYPE_SSHORT) ||        \
      ((aDataType) == ZLPL_C_DATATYPE_USHORT) ||        \
      ((aDataType) == ZLPL_C_DATATYPE_SINT) ||          \
      ((aDataType) == ZLPL_C_DATATYPE_UINT) ||          \
      ((aDataType) == ZLPL_C_DATATYPE_SLONG) ||         \
      ((aDataType) == ZLPL_C_DATATYPE_ULONG) ||         \
      ((aDataType) == ZLPL_C_DATATYPE_SLONGLONG) ||     \
      ((aDataType) == ZLPL_C_DATATYPE_ULONGLONG) ) ?    \
    STL_TRUE : STL_FALSE

#define ZLPL_IS_INTERVAL_TYPE(aDataType)                                \
    ( ((aDataType) == ZLPL_C_DATATYPE_INTERVAL_YEAR) ||                 \
      ((aDataType) == ZLPL_C_DATATYPE_INTERVAL_MONTH) ||                \
      ((aDataType) == ZLPL_C_DATATYPE_INTERVAL_DAY) ||                  \
      ((aDataType) == ZLPL_C_DATATYPE_INTERVAL_HOUR) ||                 \
      ((aDataType) == ZLPL_C_DATATYPE_INTERVAL_MINUTE) ||               \
      ((aDataType) == ZLPL_C_DATATYPE_INTERVAL_SECOND) ||               \
      ((aDataType) == ZLPL_C_DATATYPE_INTERVAL_YEAR_TO_MONTH) ||        \
      ((aDataType) == ZLPL_C_DATATYPE_INTERVAL_DAY_TO_HOUR) ||          \
      ((aDataType) == ZLPL_C_DATATYPE_INTERVAL_DAY_TO_MINUTE) ||        \
      ((aDataType) == ZLPL_C_DATATYPE_INTERVAL_DAY_TO_SECOND) ||        \
      ((aDataType) == ZLPL_C_DATATYPE_INTERVAL_HOUR_TO_MINUTE) ||       \
      ((aDataType) == ZLPL_C_DATATYPE_INTERVAL_HOUR_TO_SECOND) ||       \
      ((aDataType) == ZLPL_C_DATATYPE_INTERVAL_MINUTE_TO_SECOND) ) ?    \
    STL_TRUE : STL_FALSE

#define ZLPL_IS_NUMBER_TYPE(aDataType)             \
    ( ((aDataType) == ZLPL_C_DATATYPE_NUMBER) ) ?  \
    STL_TRUE : STL_FALSE

typedef sqlca_t     zlplSqlca;
typedef sqlargs_t   zlplSqlArgs;
typedef sqlhv_t     zlplSqlHostVar;

typedef struct zlplConnInfo
{
    SQLHDBC   mDbc;
} zlplConnInfo;

typedef struct zlplVarchar
{
    stlInt32     mLen;
    stlChar      mArr[1];
} zlplVarchar;

/** @} */

/**
 * @defgroup Symbol Symbol table entry
 * @ingroup GOLDILOCKSESQL
 * @{
 */

typedef struct zlplCursorSymbol
{
    stlChar               mCursorName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    zlplCursorOriginType  mCursorOriginType;
    stlChar               mDynStmtName[STL_MAX_SQL_IDENTIFIER_LENGTH];   /** Dynamic Cursor 의 Statement Name */

    SQLHSTMT              mStmtHandle;   /* statement handle */

    stlChar             * mQueryString;       /* query string */
    stlBool               mIsPrepared;        /* is prepared or not */
    stlBool               mIsSetCursorName;   /* is set cursor name or not */

    stlBool               mForUpdate;         /* for update or not */

    stlInt32           mArraySize;            /**< array size */
    SQLULEN            mParamProcessed;       /**< parameter processed count */
    SQLUSMALLINT     * mParamStatusArray;     /**< parameter status */

    SQLULEN            mRowFetched;           /**< the number of fetched rows */
    SQLUSMALLINT     * mRowStatusArray;       /**< row status */

    /**
     * Bind Parameter 재수행 여부
     * - Standing Cursor 인 경우: cur1 FOR SELECT * FROM t1 WHERE c1 = :sData1 AND c2 = :sData2;
     * - Dynamic  Cursor 인 경우: OPEN cur1 USING :sData1, :sData2;
     */

    stlInt32           mParamCnt;             /**< parameter count */
    stlChar         ** mParamAddrArray;       /**< bind parameter address array */
    stlChar         ** mParamIndArray;        /**< bind parameter indicator array */
    stlChar         ** mParamAddrValueArray;  /**< bind parameter address value array */
    stlChar         ** mParamIndValueArray;   /**< bind parameter indicator value array */
    SQLLEN          ** mParamLocalIndArray;   /**< local indicator array */

    /**
     * Bind Column 재수행 여부
     * - FETCH cur1 INTO :sData1, :sData2
     */

    stlInt32           mColumnCnt;            /**< result column count */
    stlChar         ** mColumnAddrArray;      /**< bind column addresss array */
    stlChar         ** mColumnIndArray;       /**< bind column indicator array */
    stlChar         ** mColumnAddrValueArray; /**< bind column addresss value array */
    stlChar         ** mColumnIndValueArray;  /**< bind column indicator value array */
    SQLLEN          ** mColumnLocalIndArray;  /**< local indicator array */

    stlRingEntry       mLink;
} zlplCursorSymbol;

typedef struct zlplSqlStmtKey
{
    stlInt32           mLine;
    stlChar           *mFileName;
} zlplSqlStmtKey;

typedef struct zlplSqlStmtSymbol
{
    SQLHSTMT            mStmtHandle;
    stlChar           * mQuery;
    stlBool             mIsPrepared;                /**< prepared or not */
    zlplSqlHostVar    * mHostVar;
    stlInt32            mArraySize;
    SQLULEN             mParamProcessed;       /**< parameter processed count */
    SQLUSMALLINT      * mParamStatusArray;     /**< parameter status */

    stlChar          ** mValueBuf;
    SQLLEN           ** mParamLocalIndArray;        /**< local indicator array */
    zlplSqlStmtKey      mKey;
    stlRingEntry        mLink;
} zlplSqlStmtSymbol;


/**
 * @brief Dynamic Statement Symbol
 */
typedef struct zlplDynStmtSymbol
{
    stlChar            mStmtName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLHSTMT           mStmtHandle;

    stlChar          * mPrevSQL;              /**< previous SQL string */
    stlBool            mIsPrepared;           /**< prepare or not */

    stlInt32           mArraySize;            /**< array size */
    SQLULEN            mParamProcessed;       /**< parameter processed count */
    SQLUSMALLINT     * mParamStatusArray;     /**< parameter status */

    stlInt32           mParamCnt;             /**< parameter count */
    stlChar         ** mParamAddrArray;       /**< bind parameter address array */
    stlChar         ** mParamIndArray;        /**< bind parameter indicator array */
    stlChar         ** mParamAddrValueArray;  /**< bind parameter address value array */
    stlChar         ** mParamIndValueArray;   /**< bind parameter indicator value array */
    SQLLEN          ** mParamLocalIndArray;   /**< local indicator array */

    zlplParamIOType  * mParamIOType;          /**< bind parameter I/O type */

    stlInt32           mColumnCnt;            /**< result column count */
    stlChar         ** mColumnAddrArray;      /**< bind column addresss array */
    stlChar         ** mColumnIndArray;       /**< bind column indicator array */
    stlChar         ** mColumnAddrValueArray; /**< bind column addresss value array */
    stlChar         ** mColumnIndValueArray;  /**< bind column indicator value array */
    SQLLEN          ** mColumnLocalIndArray;  /**< local indicator array */

    stlRingEntry       mLink;
} zlplDynStmtSymbol;


/**
 * @brief SQL Context
 */
typedef struct zlplSqlContext
{
    zlplSqlca      * mSqlca;
    zlplConnInfo     mConnInfo;

    stlErrorStack    mErrorStack;
    stlAllocator            mAllocator;
    stlDynamicAllocator     mDynMemory;

    /**
     * Symbol Table
     */

    stlStaticHash * mCursorSymTab;    /**< Cursor Symbol Table */
    stlStaticHash * mQuerySymTab;     /**< Static Statement Symbol Table */
    stlStaticHash * mDynStmtSymTab;   /**< Dynamic Statement Symbol Table */

    /**
     * Connection Name Hash
     */
    stlRingEntry     mHashLink;
    stlChar          mConnName[ZLPL_MAX_CONN_STRING_LENGTH];

    /**
     * Connection list
     */
    stlRingEntry     mListLink;
    stlBool          mIsConnected;
    stlBool          mInList;
    stlSpinLock      mCtxtSpinLock;
} zlplSqlContext;

/**
 * @brief SQL Context
 */
extern sqlca_t         sqlca_init;

/** @} */


/**
 * @defgroup DynamicSQL Dynamic SQL
 * @ingroup GOLDILOCKSESQL
 * @{
 */

/** @} DynamicSQL */

/**
 * @defgroup zlplError Error
 * @{
 */

/**
 * @brief syntax error
 */
#define ZLPL_ERRCODE_SYNTAX                                 \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_LIBRARY_GOLDILOCKSESQL, 0 )

/**
 * @brief feature not supported
 */
#define ZLPL_ERRCODE_NOT_SUPPORTED                          \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_LIBRARY_GOLDILOCKSESQL, 1 )

/**
 * @brief Bind variable \"%s\" was not declared
 */
#define ZLPL_ERRCODE_NOT_DECLARED_HOST_VARIABLE             \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_LIBRARY_GOLDILOCKSESQL, 2 )

/**
 * @brief Application can't be initialized
 */
#define ZLPL_ERRCODE_INITIALIZE                             \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_LIBRARY_GOLDILOCKSESQL, 3 )

/**
 * @brief Application can't be finalized
 */
#define ZLPL_ERRCODE_FINALIZE                               \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_LIBRARY_GOLDILOCKSESQL, 4 )

/**
 * @brief Service is not available
 */
#define ZLPL_ERRCODE_SERVICE_NOT_AVAILABLE                  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_LIBRARY_GOLDILOCKSESQL, 5 )

/**
 * @brief Statement did not prepared
 */
#define ZLPL_ERRCODE_NOT_PREPARED                           \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_LIBRARY_GOLDILOCKSESQL, 6 )

/**
 * @brief Client can't establish SQL-connection
 */
#define ZLPL_ERRCODE_CONNECTION_NOT_ESTABLISHED             \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_LIBRARY_GOLDILOCKSESQL, 7 )

/**
 * @brief cursor \"%s\" is not open
 */
#define ZLPL_ERRCODE_CURSOR_IS_NOT_OPEN                    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_LIBRARY_GOLDILOCKSESQL, 8 )

/**
 * @brief Application can't create semaphore
 */
#define ZLPL_ERRCODE_CREATE_SEMAPHORE                       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_LIBRARY_GOLDILOCKSESQL, 9 )

/**
 * @brief Application can't destroy semaphore
 */
#define ZLPL_ERRCODE_DESTROY_SEMAPHORE                          \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_LIBRARY_GOLDILOCKSESQL, 10 )

/**
 * @brief Application can't create allocator
 */
#define ZLPL_ERRCODE_CREATE_ALLOCATOR                           \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_LIBRARY_GOLDILOCKSESQL, 11 )

/**
 * @brief Application can't destroy allocator
 */
#define ZLPL_ERRCODE_DESTROY_ALLOCATOR                          \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_LIBRARY_GOLDILOCKSESQL, 12 )

/**
 * @brief Application can't create symbol table
 */
#define ZLPL_ERRCODE_CREATE_SYMTAB                              \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_LIBRARY_GOLDILOCKSESQL, 13 )

/**
 * @brief Application can't destroy symbol table
 */
#define ZLPL_ERRCODE_DESTROY_SYMTAB                             \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_LIBRARY_GOLDILOCKSESQL, 14 )

/**
 * @brief Driver's SQLAllocHandle on SQL_HANDLE_ENV failed
 */
#define ZLPL_ERRCODE_ALLOCATE_ENV                               \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_LIBRARY_GOLDILOCKSESQL, 15 )

/**
 * @brief Driver does not support ODBC 3.0
 */
#define ZLPL_ERRCODE_SET_ENV_ATTR                               \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_LIBRARY_GOLDILOCKSESQL, 16 )

/**
 * @brief Application can't allocate memory
 */
#define ZLPL_ERRCODE_ALLOCATE_MEM                               \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_LIBRARY_GOLDILOCKSESQL, 17 )

/**
 * @brief Statement \"%s\" was not prepared
 */
#define ZLPL_ERRCODE_STATEMEMT_NAME_NOT_PREPARED                    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_LIBRARY_GOLDILOCKSESQL, 18 )

/**
 * @brief Descriptor name is too long
 */
#define ZLPL_ERRCODE_DESCRIPTOR_NAME_IS_TOO_LONG                    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_LIBRARY_GOLDILOCKSESQL, 19 )

/**
 * @brief Not implemented feature, in a function %s
 */
#define ZLPL_ERRCODE_NOT_IMPLEMENTED_FEATURE                        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_LIBRARY_GOLDILOCKSESQL, 20 )

/**
 * @brief USING clause required for dynamic paramters
 */
#define ZLPL_ERRCODE_USING_CLAUSE_REQUIRED_FOR_DYNAMIC_PARAMTERS    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_LIBRARY_GOLDILOCKSESQL, 21 )

/**
 * @brief USING clause: illegal variable name/number
 */
#define ZLPL_ERRCODE_USING_CLAUSE_ILLEGAL_VARIABLE_NAME_NUMBER      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_LIBRARY_GOLDILOCKSESQL, 22 )

/**
 * @brief USING clause: not all variables bound
 */
#define ZLPL_ERRCODE_USING_CLAUSE_NOT_ALL_VARIABLES_BOUND           \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_LIBRARY_GOLDILOCKSESQL, 23 )

/**
 * @brief INTO clause required for result fields
 */
#define ZLPL_ERRCODE_INTO_CLAUSE_REQUIRED_FOR_RESULT_FIELDS         \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_LIBRARY_GOLDILOCKSESQL, 24 )

/**
 * @brief INTO clause: illegal variable name/number
 */
#define ZLPL_ERRCODE_INTO_CLAUSE_ILLEGAL_VARIABLE_NAME_NUMBER       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_LIBRARY_GOLDILOCKSESQL, 25 )

/**
 * @brief INTO clause: not all variables bound
 */
#define ZLPL_ERRCODE_INTO_CLAUSE_NOT_ALL_VARIABLES_BOUND            \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_LIBRARY_GOLDILOCKSESQL, 26 )

/**
 * @brief null value, no indicator parameter
 */
#define ZLPL_ERRCODE_NULL_VALUE_NO_INDICATOR_PARAMETER              \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_LIBRARY_GOLDILOCKSESQL, 27 )

/**
 * @brief cusor \"%s\" statement is not a query
 */
#define ZLPL_ERRCODE_STATEMENT_IS_NOT_A_QUERY                       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_LIBRARY_GOLDILOCKSESQL, 28 )

/**
 * @brief connection not open
 */
#define ZLPL_ERRCODE_CONNECTION_NOT_OPEN                            \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_LIBRARY_GOLDILOCKSESQL, 29 )


/**
 * @brief cursor \"%s\" is not updatable
 */
#define ZLPL_ERRCODE_CURSOR_IS_NOT_UPDATABLE                        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_LIBRARY_GOLDILOCKSESQL, 30 )

/**
 * @brief connection name is use
 */
#define ZLPL_ERRCODE_CONNECTION_NAME_IN_USE                         \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_LIBRARY_GOLDILOCKSESQL, 31 )

/**
 * @brief duplicate connection context
 */
#define ZLPL_ERRCODE_DUPLICATE_CONTEXT                              \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_LIBRARY_GOLDILOCKSESQL, 32 )


/** @} */

#endif /* _ZLPLDEF_H_ */
