/*******************************************************************************
 * zlfInfo.c
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

#include <cml.h>
#include <goldilocks.h>
#include <zlDef.h>
#include <zle.h>
#include <zls.h>
#include <zli.h>
#include <zlx.h>
#include <zllDef.h>

/**
 * @file zlfInfo.c
 * @brief ODBC API Internal Info Routines.
 */

/**
 * @addtogroup zlf
 * @{
 */

#if defined( ODBC_ALL )
#if defined( ODBC_SHARED )
#define ZLF_DRIVER_NAME STL_SHARED_LIBRARY_PREFIX"goldilockss"STL_SHARED_LIBRARY_SUFFIX
#else
#define ZLF_DRIVER_NAME STL_STATIC_LIBRARY_PREFIX"goldilocks"STL_STATIC_LIBRARY_SUFFIX
#endif
#elif defined( ODBC_DA )
#if defined( ODBC_SHARED )
#define ZLF_DRIVER_NAME STL_SHARED_LIBRARY_PREFIX"goldilocksas"STL_SHARED_LIBRARY_SUFFIX
#else
#define ZLF_DRIVER_NAME STL_STATIC_LIBRARY_PREFIX"goldilocksa"STL_STATIC_LIBRARY_SUFFIX
#endif
#elif defined( ODBC_CS )
#if defined( ODBC_SHARED )
#if (STL_SIZEOF_VOIDP == 8)
#if defined( BUILD_LEGACY_64_BIT_MODE )
#define ZLF_DRIVER_NAME STL_SHARED_LIBRARY_PREFIX"goldilockscs-ul32"STL_SHARED_LIBRARY_SUFFIX
#else
#define ZLF_DRIVER_NAME STL_SHARED_LIBRARY_PREFIX"goldilockscs-ul64"STL_SHARED_LIBRARY_SUFFIX
#endif
#else
#define ZLF_DRIVER_NAME STL_SHARED_LIBRARY_PREFIX"goldilockscs"STL_SHARED_LIBRARY_SUFFIX
#endif
#else
#define ZLF_DRIVER_NAME STL_STATIC_LIBRARY_PREFIX"goldilocksc"STL_STATIC_LIBRARY_SUFFIX
#endif
#else
#define ZLF_DRIVER_NAME ""
#endif

static stlUInt16 gZlfFunctions[] =
{
    SQL_API_SQLALLOCCONNECT,
    SQL_API_SQLALLOCENV,
    SQL_API_SQLALLOCHANDLE,
    SQL_API_SQLALLOCSTMT,
    SQL_API_SQLBINDCOL,
    SQL_API_SQLBINDPARAM,
    /* SQL_API_SQLCANCEL, */
    SQL_API_SQLCLOSECURSOR,
    SQL_API_SQLCOLATTRIBUTE,
    SQL_API_SQLCOLUMNS,
    SQL_API_SQLCONNECT,
    /* SQL_API_SQLCOPYDESC, */
    /* SQL_API_SQLDATASOURCES, */
    SQL_API_SQLDESCRIBECOL,
    SQL_API_SQLDISCONNECT,
    SQL_API_SQLENDTRAN,
    SQL_API_SQLERROR,
    SQL_API_SQLEXECDIRECT,
    SQL_API_SQLEXECUTE,
    SQL_API_SQLFETCH,
    SQL_API_SQLFETCHSCROLL,
    SQL_API_SQLFREECONNECT,
    SQL_API_SQLFREEENV,
    SQL_API_SQLFREEHANDLE,
    SQL_API_SQLFREESTMT,
    SQL_API_SQLGETCONNECTATTR,
    SQL_API_SQLGETCONNECTOPTION,
    SQL_API_SQLGETCURSORNAME,
    SQL_API_SQLGETDATA,
    SQL_API_SQLGETDESCFIELD,
    SQL_API_SQLGETDESCREC,
    SQL_API_SQLGETDIAGFIELD,
    SQL_API_SQLGETDIAGREC,
    SQL_API_SQLGETENVATTR,
    SQL_API_SQLGETFUNCTIONS,
    SQL_API_SQLGETINFO,
    SQL_API_SQLGETSTMTATTR,
    SQL_API_SQLGETSTMTOPTION,
    SQL_API_SQLGETTYPEINFO,
    SQL_API_SQLNUMRESULTCOLS,
    SQL_API_SQLPARAMDATA,
    SQL_API_SQLPREPARE,
    SQL_API_SQLPUTDATA,
    SQL_API_SQLROWCOUNT,
    SQL_API_SQLSETCONNECTATTR,
    SQL_API_SQLSETCONNECTOPTION,
    SQL_API_SQLSETCURSORNAME,
    SQL_API_SQLSETDESCFIELD,
    SQL_API_SQLSETDESCREC,
    SQL_API_SQLSETENVATTR,
    SQL_API_SQLSETPARAM,
    SQL_API_SQLSETSTMTATTR,
    SQL_API_SQLSETSTMTOPTION,
    SQL_API_SQLSPECIALCOLUMNS,
    SQL_API_SQLSTATISTICS,
    SQL_API_SQLTABLES,
    SQL_API_SQLTRANSACT,
    /* SQL_API_SQLALLOCHANDLESTD */
    /* SQL_API_SQLBULKOPERATIONS, */
    SQL_API_SQLBINDPARAMETER,
    /* SQL_API_SQLBROWSECONNECT, */
    SQL_API_SQLCOLATTRIBUTES,
    SQL_API_SQLCOLUMNPRIVILEGES,
    SQL_API_SQLDESCRIBEPARAM,
    SQL_API_SQLDRIVERCONNECT,
    /* SQL_API_SQLDRIVERS, */
    SQL_API_SQLEXTENDEDFETCH,
    SQL_API_SQLFOREIGNKEYS,
    SQL_API_SQLMORERESULTS,
    /* SQL_API_SQLNATIVESQL, */
    SQL_API_SQLNUMPARAMS,
    SQL_API_SQLPARAMOPTIONS,
    SQL_API_SQLPRIMARYKEYS,
    SQL_API_SQLPROCEDURECOLUMNS,
    SQL_API_SQLPROCEDURES,
    SQL_API_SQLSETPOS,
    /* SQL_API_SQLSETSCROLLOPTIONS, */
    SQL_API_SQLTABLEPRIVILEGES
};

stlStatus zlfGetFunctions( zlcDbc        * aDbc,
                           stlUInt16       aFuncId,
                           stlUInt16     * aSupported,
                           stlErrorStack * aErrorStack )
{
    stlUInt16 sId;
    stlUInt16 sIdx;
    stlUInt16 sFuncSize;

    sFuncSize = STL_SIZEOF( gZlfFunctions ) / STL_SIZEOF( gZlfFunctions[0] );

    switch( aFuncId )
    {
        case SQL_API_ODBC3_ALL_FUNCTIONS :
            /* Clear and set bits in the 4000 bit vector */
            stlMemset( aSupported,
                       0x00,
                       STL_SIZEOF( stlUInt16) * SQL_API_ODBC3_ALL_FUNCTIONS_SIZE );

            for( sIdx = 0; sIdx < sFuncSize; sIdx++ )
            {
                sId = gZlfFunctions[sIdx];
                aSupported[sId >> 4] |= ( 1 << (sId & 0x000F) );
            }
            break;
        case SQL_API_ALL_FUNCTIONS :
            /* Clear and set elements in the SQLUSMALLINT 100 element array */
            stlMemset( aSupported,
                       0x00,
                       STL_SIZEOF( stlUInt16) * 100 );

            for( sIdx = 0; sIdx < sFuncSize; sIdx++ )
            {
                if( gZlfFunctions[sIdx] < 100 )
                {
                    aSupported[gZlfFunctions[sIdx]] = SQL_TRUE;
                }
            }
            break;
        default :
            *aSupported = STL_FALSE;

            for( sIdx = 0; sIdx < sFuncSize; sIdx++ )
            {
                if( gZlfFunctions[sIdx] == aFuncId )
                {
                    *aSupported = STL_TRUE;
                }
            }
            break;
    }

    return STL_SUCCESS;
}

stlStatus zlfGetInfo( zlcDbc         * aDbc,
                      stlUInt16        aInfoType,
                      void           * aInfoValuePtr,
                      stlInt16         aBufferLength,
                      stlInt16       * aStringLengthPtr,
                      stlErrorStack  * aErrorStack )
{
    zlsStmt        * sStmt    = NULL;
    zldDiagElement * sDiagRec = NULL;

    stlUInt64 sIntegerValue = 0;
    stlChar   sCharacterValue[1024 + 1];

    SQLLEN    sIntegerValueInd   = 0;
    SQLLEN    sCharacterValueInd = 0;

    stlInt64  sAffectedRowCount = 0;
    stlBool   sSuccessWithInfo  = STL_FALSE;

    stlErrorStack sStmtErrorStack;
    stlChar     * sRefinedMessageText;
    
    STL_INIT_ERROR_STACK( &sStmtErrorStack );

    stlMemset( sCharacterValue, 0x00, STL_SIZEOF(sCharacterValue) );

    switch( aInfoType )
    {
        /*
         * 서버에서 얻어오는 정보
         */
        case SQL_CATALOG_LOCATION:
        case SQL_CONCAT_NULL_BEHAVIOR:
        case SQL_CORRELATION_NAME:
        case SQL_CURSOR_COMMIT_BEHAVIOR:
        case SQL_CURSOR_ROLLBACK_BEHAVIOR:
        case SQL_FETCH_DIRECTION :
        case SQL_GROUP_BY:
        case SQL_IDENTIFIER_CASE:
        case SQL_LOCK_TYPES :
        case SQL_MAX_CATALOG_NAME_LEN:
        case SQL_MAX_COLUMN_NAME_LEN:
        case SQL_MAX_COLUMNS_IN_GROUP_BY:
        case SQL_MAX_COLUMNS_IN_INDEX:
        case SQL_MAX_COLUMNS_IN_ORDER_BY:
        case SQL_MAX_COLUMNS_IN_SELECT:
        case SQL_MAX_COLUMNS_IN_TABLE:
        case SQL_MAX_CONCURRENT_ACTIVITIES:
        case SQL_MAX_CURSOR_NAME_LEN:
        case SQL_MAX_DRIVER_CONNECTIONS:
        case SQL_MAX_IDENTIFIER_LEN:
        case SQL_MAX_PROCEDURE_NAME_LEN:
        case SQL_MAX_SCHEMA_NAME_LEN:
        case SQL_MAX_TABLE_NAME_LEN:
        case SQL_MAX_TABLES_IN_SELECT:
        case SQL_MAX_USER_NAME_LEN:
        case SQL_NON_NULLABLE_COLUMNS:
        case SQL_NULL_COLLATION:
        case SQL_ODBC_API_CONFORMANCE :
        case SQL_ODBC_SQL_CONFORMANCE :
        case SQL_POS_OPERATIONS :
        case SQL_POSITIONED_STATEMENTS :
        case SQL_QUOTED_IDENTIFIER_CASE:
        case SQL_SCROLL_CONCURRENCY :
        case SQL_STATIC_SENSITIVITY :
        case SQL_TXN_CAPABLE:
        case SQL_AGGREGATE_FUNCTIONS:
        case SQL_ALTER_DOMAIN:
        case SQL_ALTER_TABLE:
        case SQL_BOOKMARK_PERSISTENCE:
        case SQL_CATALOG_USAGE:
        case SQL_CONVERT_BIGINT:
        case SQL_CONVERT_BINARY:
        case SQL_CONVERT_BIT:
        case SQL_CONVERT_CHAR:
        case SQL_CONVERT_GUID:
        case SQL_CONVERT_DATE:
        case SQL_CONVERT_DECIMAL:
        case SQL_CONVERT_DOUBLE:
        case SQL_CONVERT_FLOAT:
        case SQL_CONVERT_INTEGER:
        case SQL_CONVERT_INTERVAL_YEAR_MONTH:
        case SQL_CONVERT_INTERVAL_DAY_TIME:
        case SQL_CONVERT_LONGVARBINARY:
        case SQL_CONVERT_LONGVARCHAR:
        case SQL_CONVERT_NUMERIC:
        case SQL_CONVERT_REAL:
        case SQL_CONVERT_SMALLINT:
        case SQL_CONVERT_TIME:
        case SQL_CONVERT_TIMESTAMP:
        case SQL_CONVERT_TINYINT:
        case SQL_CONVERT_VARBINARY:
        case SQL_CONVERT_VARCHAR:
        case SQL_CONVERT_FUNCTIONS:
        case SQL_CREATE_ASSERTION:
        case SQL_CREATE_CHARACTER_SET:
        case SQL_CREATE_COLLATION:
        case SQL_CREATE_DOMAIN:
        case SQL_CREATE_SCHEMA:
        case SQL_CREATE_TABLE:
        case SQL_CREATE_TRANSLATION:
        case SQL_CREATE_VIEW:
        case SQL_CURSOR_SENSITIVITY:
        case SQL_DATETIME_LITERALS:
        case SQL_DDL_INDEX:
        case SQL_DEFAULT_TXN_ISOLATION:
        case SQL_DROP_ASSERTION:
        case SQL_DROP_CHARACTER_SET:
        case SQL_DROP_COLLATION:
        case SQL_DROP_DOMAIN:
        case SQL_DROP_SCHEMA:
        case SQL_DROP_TABLE:
        case SQL_DROP_TRANSLATION:
        case SQL_DROP_VIEW:
        case SQL_DYNAMIC_CURSOR_ATTRIBUTES1:
        case SQL_DYNAMIC_CURSOR_ATTRIBUTES2:
        case SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES1:
        case SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES2:
        case SQL_INDEX_KEYWORDS:
        case SQL_INFO_SCHEMA_VIEWS:
        case SQL_INSERT_STATEMENT:
        case SQL_KEYSET_CURSOR_ATTRIBUTES1:
        case SQL_KEYSET_CURSOR_ATTRIBUTES2:
        case SQL_MAX_BINARY_LITERAL_LEN:
        case SQL_MAX_CHAR_LITERAL_LEN:
        case SQL_MAX_INDEX_SIZE:
        case SQL_MAX_ROW_SIZE:
        case SQL_MAX_STATEMENT_LEN:
        case SQL_NUMERIC_FUNCTIONS:
        case SQL_OJ_CAPABILITIES:
        case SQL_OUTER_JOINS:
        case SQL_SCHEMA_USAGE:
        case SQL_SCROLL_OPTIONS:
        case SQL_SQL_CONFORMANCE:
        case SQL_SQL92_DATETIME_FUNCTIONS:
        case SQL_SQL92_FOREIGN_KEY_DELETE_RULE:
        case SQL_SQL92_FOREIGN_KEY_UPDATE_RULE:
        case SQL_SQL92_GRANT:
        case SQL_SQL92_NUMERIC_VALUE_FUNCTIONS:
        case SQL_SQL92_PREDICATES:
        case SQL_SQL92_RELATIONAL_JOIN_OPERATORS:
        case SQL_SQL92_REVOKE:
        case SQL_SQL92_ROW_VALUE_CONSTRUCTOR:
        case SQL_SQL92_STRING_FUNCTIONS:
        case SQL_SQL92_VALUE_EXPRESSIONS:
        case SQL_STANDARD_CLI_CONFORMANCE:
        case SQL_STATIC_CURSOR_ATTRIBUTES1:
        case SQL_STATIC_CURSOR_ATTRIBUTES2:
        case SQL_STRING_FUNCTIONS:
        case SQL_SUBQUERIES:
        case SQL_SYSTEM_FUNCTIONS:
        case SQL_TIMEDATE_ADD_INTERVALS:
        case SQL_TIMEDATE_DIFF_INTERVALS:
        case SQL_TIMEDATE_FUNCTIONS:
        case SQL_TXN_ISOLATION_OPTION:
        case SQL_UNION:
        case SQL_ACCESSIBLE_PROCEDURES:
        case SQL_ACCESSIBLE_TABLES:
        case SQL_CATALOG_NAME:
        case SQL_CATALOG_NAME_SEPARATOR:
        case SQL_CATALOG_TERM:
        case SQL_COLLATION_SEQ:
        case SQL_COLUMN_ALIAS:
        case SQL_DATA_SOURCE_READ_ONLY:
        case SQL_DATABASE_NAME:
        case SQL_DBMS_NAME:
        case SQL_DBMS_VER:
        case SQL_DESCRIBE_PARAMETER:
        case SQL_EXPRESSIONS_IN_ORDERBY:
        case SQL_IDENTIFIER_QUOTE_CHAR:
        case SQL_INTEGRITY:
        case SQL_KEYWORDS:
        case SQL_LIKE_ESCAPE_CLAUSE:
        case SQL_MAX_ROW_SIZE_INCLUDES_LONG:
        case SQL_MULT_RESULT_SETS:
        case SQL_MULTIPLE_ACTIVE_TXN:
        case SQL_NEED_LONG_DATA_LEN:
        case SQL_ORDER_BY_COLUMNS_IN_SELECT:
        case SQL_PROCEDURE_TERM:
        case SQL_PROCEDURES:
        case SQL_ROW_UPDATES:
        case SQL_SCHEMA_TERM:
        case SQL_SEARCH_PATTERN_ESCAPE:
        case SQL_SPECIAL_CHARACTERS:
        case SQL_TABLE_TERM:
        case SQL_USER_NAME:
        case SQL_XOPEN_CLI_YEAR:
            STL_TRY( zlsAlloc( aDbc,
                               (void**)&sStmt,
                               aErrorStack ) == STL_SUCCESS );

            STL_TRY( zliBindParameter( sStmt,
                                       1,
                                       SQL_PARAM_OUTPUT,
                                       SQL_C_UBIGINT,
                                       SQL_BIGINT,
                                       0,
                                       0,
                                       &sIntegerValue,
                                       0,
                                       &sIntegerValueInd,
                                       aErrorStack ) == STL_SUCCESS );

            STL_TRY( zliBindParameter( sStmt,
                                       2,
                                       SQL_PARAM_OUTPUT,
                                       SQL_C_CHAR,
                                       SQL_VARCHAR,
                                       1024,
                                       0,
                                       sCharacterValue,
                                       STL_SIZEOF(sCharacterValue),
                                       &sCharacterValueInd,
                                       aErrorStack ) == STL_SUCCESS );

            STL_TRY( zliBindParameter( sStmt,
                                       3,
                                       SQL_PARAM_INPUT,
                                       SQL_C_USHORT,
                                       SQL_INTEGER,
                                       0,
                                       0,
                                       &aInfoType,
                                       0,
                                       NULL,
                                       aErrorStack ) == STL_SUCCESS );

            STL_TRY( zlxExecDirect( sStmt,
                                    "SELECT INTEGER_VALUE, CHARACTER_VALUE "
                                    "INTO ?, ? "
                                    "FROM DICTIONARY_SCHEMA.IMPLEMENTATION_INFO "
                                    "WHERE IMPLEMENTATION_INFO_ID = ?",
                                    &sAffectedRowCount,
                                    &sSuccessWithInfo,
                                    aErrorStack ) == STL_SUCCESS );
            break;
        default:
            break;
    }

    switch( aInfoType )
    {
        /*
         * 서버에서 얻어오는 정보
         */
        case SQL_ODBC_API_CONFORMANCE :
        case SQL_ODBC_SQL_CONFORMANCE :
            *(SQLSMALLINT*)aInfoValuePtr = (SQLSMALLINT)sIntegerValue;
            break;
        case SQL_CATALOG_LOCATION :
        case SQL_CONCAT_NULL_BEHAVIOR :
        case SQL_CORRELATION_NAME :
        case SQL_CURSOR_COMMIT_BEHAVIOR :
        case SQL_CURSOR_ROLLBACK_BEHAVIOR :
        case SQL_GROUP_BY :
        case SQL_IDENTIFIER_CASE :
        case SQL_MAX_CATALOG_NAME_LEN :
        case SQL_MAX_COLUMN_NAME_LEN :
        case SQL_MAX_COLUMNS_IN_GROUP_BY :
        case SQL_MAX_COLUMNS_IN_INDEX :
        case SQL_MAX_COLUMNS_IN_ORDER_BY :
        case SQL_MAX_COLUMNS_IN_SELECT :
        case SQL_MAX_COLUMNS_IN_TABLE :
        case SQL_MAX_CONCURRENT_ACTIVITIES :
        case SQL_MAX_CURSOR_NAME_LEN :
        case SQL_MAX_DRIVER_CONNECTIONS :
        case SQL_MAX_IDENTIFIER_LEN :
        case SQL_MAX_PROCEDURE_NAME_LEN :
        case SQL_MAX_SCHEMA_NAME_LEN :
        case SQL_MAX_TABLE_NAME_LEN :
        case SQL_MAX_TABLES_IN_SELECT :
        case SQL_MAX_USER_NAME_LEN :
        case SQL_NON_NULLABLE_COLUMNS :
        case SQL_NULL_COLLATION :
        case SQL_QUOTED_IDENTIFIER_CASE :
        case SQL_TXN_CAPABLE :
            *(SQLUSMALLINT*)aInfoValuePtr = (SQLUSMALLINT)sIntegerValue;
            break;
        case SQL_FETCH_DIRECTION :
        case SQL_LOCK_TYPES :
        case SQL_POS_OPERATIONS :
        case SQL_POSITIONED_STATEMENTS :
        case SQL_SCROLL_CONCURRENCY :
        case SQL_STATIC_SENSITIVITY :
            *(SQLINTEGER*)aInfoValuePtr = (SQLINTEGER)sIntegerValue;
            break;
        case SQL_AGGREGATE_FUNCTIONS :
        case SQL_ALTER_DOMAIN :
        case SQL_ALTER_TABLE :
        case SQL_BOOKMARK_PERSISTENCE :
        case SQL_CATALOG_USAGE :
        case SQL_CONVERT_BIGINT :
        case SQL_CONVERT_BINARY :
        case SQL_CONVERT_BIT :
        case SQL_CONVERT_CHAR :
        case SQL_CONVERT_GUID :
        case SQL_CONVERT_DATE :
        case SQL_CONVERT_DECIMAL :
        case SQL_CONVERT_DOUBLE :
        case SQL_CONVERT_FLOAT :
        case SQL_CONVERT_INTEGER :
        case SQL_CONVERT_INTERVAL_YEAR_MONTH :
        case SQL_CONVERT_INTERVAL_DAY_TIME :
        case SQL_CONVERT_LONGVARBINARY :
        case SQL_CONVERT_LONGVARCHAR :
        case SQL_CONVERT_NUMERIC :
        case SQL_CONVERT_REAL :
        case SQL_CONVERT_SMALLINT :
        case SQL_CONVERT_TIME :
        case SQL_CONVERT_TIMESTAMP :
        case SQL_CONVERT_TINYINT :
        case SQL_CONVERT_VARBINARY :
        case SQL_CONVERT_VARCHAR :
        case SQL_CONVERT_FUNCTIONS :
        case SQL_CREATE_ASSERTION :
        case SQL_CREATE_CHARACTER_SET :
        case SQL_CREATE_COLLATION :
        case SQL_CREATE_DOMAIN :
        case SQL_CREATE_SCHEMA :
        case SQL_CREATE_TABLE :
        case SQL_CREATE_TRANSLATION :
        case SQL_CREATE_VIEW :
        case SQL_CURSOR_SENSITIVITY :
        case SQL_DATETIME_LITERALS :
        case SQL_DDL_INDEX :
        case SQL_DEFAULT_TXN_ISOLATION :
        case SQL_DROP_ASSERTION :
        case SQL_DROP_CHARACTER_SET :
        case SQL_DROP_COLLATION :
        case SQL_DROP_DOMAIN :
        case SQL_DROP_SCHEMA :
        case SQL_DROP_TABLE :
        case SQL_DROP_TRANSLATION :
        case SQL_DROP_VIEW :
        case SQL_DYNAMIC_CURSOR_ATTRIBUTES1 :
        case SQL_DYNAMIC_CURSOR_ATTRIBUTES2 :
        case SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES1 :
        case SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES2 :
        case SQL_INDEX_KEYWORDS :
        case SQL_INFO_SCHEMA_VIEWS :
        case SQL_INSERT_STATEMENT :
        case SQL_KEYSET_CURSOR_ATTRIBUTES1 :
        case SQL_KEYSET_CURSOR_ATTRIBUTES2 :
        case SQL_MAX_BINARY_LITERAL_LEN :
        case SQL_MAX_CHAR_LITERAL_LEN :
        case SQL_MAX_INDEX_SIZE :
        case SQL_MAX_ROW_SIZE :
        case SQL_MAX_STATEMENT_LEN :
        case SQL_NUMERIC_FUNCTIONS :
        case SQL_OJ_CAPABILITIES :
        case SQL_SCHEMA_USAGE :
        case SQL_SCROLL_OPTIONS :
        case SQL_SQL_CONFORMANCE :
        case SQL_SQL92_DATETIME_FUNCTIONS :
        case SQL_SQL92_FOREIGN_KEY_DELETE_RULE :
        case SQL_SQL92_FOREIGN_KEY_UPDATE_RULE :
        case SQL_SQL92_GRANT :
        case SQL_SQL92_NUMERIC_VALUE_FUNCTIONS :
        case SQL_SQL92_PREDICATES :
        case SQL_SQL92_RELATIONAL_JOIN_OPERATORS :
        case SQL_SQL92_REVOKE :
        case SQL_SQL92_ROW_VALUE_CONSTRUCTOR :
        case SQL_SQL92_STRING_FUNCTIONS :
        case SQL_SQL92_VALUE_EXPRESSIONS :
        case SQL_STANDARD_CLI_CONFORMANCE :
        case SQL_STATIC_CURSOR_ATTRIBUTES1 :
        case SQL_STATIC_CURSOR_ATTRIBUTES2 :
        case SQL_STRING_FUNCTIONS :
        case SQL_SUBQUERIES :
        case SQL_SYSTEM_FUNCTIONS :
        case SQL_TIMEDATE_ADD_INTERVALS :
        case SQL_TIMEDATE_DIFF_INTERVALS :
        case SQL_TIMEDATE_FUNCTIONS :
        case SQL_TXN_ISOLATION_OPTION :
        case SQL_UNION :
            *(SQLUINTEGER*)aInfoValuePtr = (SQLUINTEGER)sIntegerValue;
            break;
        case SQL_ACCESSIBLE_PROCEDURES :
        case SQL_ACCESSIBLE_TABLES :
        case SQL_CATALOG_NAME :
        case SQL_CATALOG_NAME_SEPARATOR :
        case SQL_CATALOG_TERM :
        case SQL_COLLATION_SEQ :
        case SQL_COLUMN_ALIAS :
        case SQL_DATA_SOURCE_READ_ONLY :
        case SQL_DBMS_NAME :
        case SQL_DBMS_VER :
        case SQL_DESCRIBE_PARAMETER :
        case SQL_EXPRESSIONS_IN_ORDERBY :
        case SQL_IDENTIFIER_QUOTE_CHAR :
        case SQL_INTEGRITY :
        case SQL_KEYWORDS :
        case SQL_LIKE_ESCAPE_CLAUSE :
        case SQL_MAX_ROW_SIZE_INCLUDES_LONG :
        case SQL_MULT_RESULT_SETS :
        case SQL_MULTIPLE_ACTIVE_TXN :
        case SQL_NEED_LONG_DATA_LEN :
        case SQL_ORDER_BY_COLUMNS_IN_SELECT :
        case SQL_OUTER_JOINS :
        case SQL_PROCEDURE_TERM :
        case SQL_PROCEDURES :
        case SQL_ROW_UPDATES :
        case SQL_SCHEMA_TERM :
        case SQL_SEARCH_PATTERN_ESCAPE :
        case SQL_SPECIAL_CHARACTERS :
        case SQL_TABLE_TERM :
        case SQL_USER_NAME :
        case SQL_XOPEN_CLI_YEAR :
            if( aInfoValuePtr != NULL )
            {
                stlStrncpy( aInfoValuePtr, sCharacterValue, aBufferLength );
            }

            if( aStringLengthPtr != NULL )
            {
                *aStringLengthPtr = sCharacterValueInd;
            }
            break;
            
            /*
             * driver가 제공하는 정보
             */

            /*
             * SQLUSMALLINT
             */
        case SQL_ACTIVE_ENVIRONMENTS :
            *(SQLUSMALLINT*)aInfoValuePtr = 0;
            break;
        case SQL_FILE_USAGE :
            *(SQLUSMALLINT*)aInfoValuePtr = SQL_FILE_NOT_SUPPORTED;
            break;
            
            /*
             * SQLUINTEGER
             */
        case SQL_ASYNC_DBC_FUNCTIONS :
            *(SQLUINTEGER*)aInfoValuePtr = SQL_ASYNC_DBC_NOT_CAPABLE;
            break;
        case SQL_ASYNC_MODE :
            *(SQLUINTEGER*)aInfoValuePtr = SQL_AM_NONE;
            break;
        case SQL_BATCH_ROW_COUNT :
            *(SQLUINTEGER*)aInfoValuePtr = 0;
            break;
        case SQL_BATCH_SUPPORT :
            *(SQLUINTEGER*)aInfoValuePtr = 0;
            break;
        case SQL_GETDATA_EXTENSIONS :
            *(SQLUINTEGER*)aInfoValuePtr = SQL_GD_ANY_ORDER | SQL_GD_BLOCK | SQL_GD_BOUND;
            break;
        case SQL_MAX_ASYNC_CONCURRENT_STATEMENTS :
            *(SQLUINTEGER*)aInfoValuePtr = 0;
            break;
        case SQL_ODBC_INTERFACE_CONFORMANCE :
            *(SQLUINTEGER*)aInfoValuePtr = SQL_OIC_CORE;
            break;
        case SQL_PARAM_ARRAY_ROW_COUNTS :
            *(SQLUINTEGER*)aInfoValuePtr = SQL_PARC_NO_BATCH;
            break;
        case SQL_PARAM_ARRAY_SELECTS :
            *(SQLUINTEGER*)aInfoValuePtr = SQL_PAS_NO_SELECT;
            break;
            /*
             * SQLULEN
             */
        case SQL_DRIVER_HDBC :
            *(SQLULEN*)aInfoValuePtr = 0;
            break;
        case SQL_DRIVER_HENV :
            *(SQLULEN*)aInfoValuePtr = 0;
            break;
        case SQL_DRIVER_HDESC :
            *(SQLULEN*)aInfoValuePtr = 0;
            break;
        case SQL_DRIVER_HLIB :
            *(SQLULEN*)aInfoValuePtr = 0;
            break;
        case SQL_DRIVER_HSTMT :
            *(SQLULEN*)aInfoValuePtr = 0;
            break;
            
            /*
             * SQLCHAR
             */
        case SQL_DATABASE_NAME:
            if (aInfoValuePtr != NULL)
            {
                stlStrncpy(aInfoValuePtr, "", aBufferLength);
            }

            if (aStringLengthPtr != NULL)
            {
                *aStringLengthPtr = 0;
            }
            break;
        case SQL_DATA_SOURCE_NAME :
        case SQL_SERVER_NAME :
            if( aInfoValuePtr != NULL )
            {
                if( aDbc->mServerName != NULL )
                {
                    stlStrncpy( aInfoValuePtr, aDbc->mServerName, aBufferLength );
                }
            }

            if( aStringLengthPtr != NULL )
            {
                if( aDbc->mServerName != NULL )
                {
                    *aStringLengthPtr = stlStrlen(aDbc->mServerName);
                }
            }
            break;
        case SQL_DM_VER :
            if( aInfoValuePtr != NULL )
            {
                stlStrncpy( aInfoValuePtr, "03.52.0002.0003", aBufferLength );
            }

            if( aStringLengthPtr != NULL )
            {
                *aStringLengthPtr = stlStrlen("03.52.0002.0003");
            }
            break;
        case SQL_DRIVER_NAME :
            if( aInfoValuePtr != NULL )
            {
                stlStrncpy( aInfoValuePtr, ZLF_DRIVER_NAME, aBufferLength );
            }
            
            if( aStringLengthPtr != NULL )
            {
                *aStringLengthPtr = stlStrlen( ZLF_DRIVER_NAME );
            }
            break;
        case SQL_DRIVER_ODBC_VER :
        case SQL_ODBC_VER :
            if( aInfoValuePtr != NULL )
            {
                stlStrncpy( aInfoValuePtr, "03.52", aBufferLength );
            }

            if( aStringLengthPtr != NULL )
            {
                *aStringLengthPtr = stlStrlen("03.52");
            }
            break;
        case SQL_DRIVER_VER :
            if( aInfoValuePtr != NULL )
            {
                stlSnprintf( aInfoValuePtr, aBufferLength,
                             "%02d.%02d.%04d",
                             STL_MAJOR_VERSION,
                             STL_MINOR_VERSION,
                             STL_PATCH_VERSION );
            }

            if( aStringLengthPtr != NULL )
            {
                *aStringLengthPtr = 10;
            }
            break;
        default :
            STL_DASSERT( STL_FALSE );
            break;
    }

    if( sStmt != NULL )
    {
        STL_TRY( zlsFree( sStmt,
                          STL_FALSE,
                          aErrorStack ) == STL_SUCCESS );
    }    
    
    sStmt = NULL;

    return STL_SUCCESS;

    STL_FINISH;

    if( sStmt != NULL )
    {
        /*
         * error stack 에 있던 에러가 statement의 diag record에 추가되어 error stack이 비어 있는 경우,
         * statement를 해제하기 전 statement의 diag record에서 에러 메세지를 갖고 온다.
         */

        if( STL_HAS_ERROR( aErrorStack ) == STL_FALSE )
        {
            STL_RING_FOREACH_ENTRY_REVERSE( &sStmt->mDiag.mDiagRing, sDiagRec )
            {
                if( (sDiagRec->mMessageText != NULL) &&
                    (sDiagRec->mMessageText[0] != 0) )
                {
                    /*
                     * front vendor string 제거
                     */
                    sRefinedMessageText = ( stlStrstr( sDiagRec->mMessageText,
                                                       ZLL_ODBC_COMPONENT_IDENTIFIER ) +
                                            stlStrlen( ZLL_ODBC_COMPONENT_IDENTIFIER ));
                    /*
                     * front data source 제거
                     */
                    sRefinedMessageText = ( stlStrstr( (stlChar*)sRefinedMessageText,
                                                       ZLL_DATA_SOURCE_IDENTIFIER ) +
                                            stlStrlen( ZLL_DATA_SOURCE_IDENTIFIER ));
                
                    stlPushError( STL_ERROR_LEVEL_ABORT,
                                  ZLE_ERRCODE_GENERAL_ERROR,
                                  sRefinedMessageText,
                                  aErrorStack );
                }
            }
        }
        
        (void)zlsFree( sStmt, STL_FALSE, &sStmtErrorStack );
    }
    
    return STL_FAILURE;
}

/** @} */
