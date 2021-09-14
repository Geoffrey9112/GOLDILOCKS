/*******************************************************************************
 * ztqOdbcBridge.c
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

#include <stl.h>
#include <dtl.h>
#include <zllDef.h>
#include <goldilocks.h>
#include <ztqDef.h>
#include <ztqOdbcBridge.h>
#include <ztqDisplay.h>

extern stlPaintAttr    gZtqErrorPaintAttr;
extern stlChar         gZtqDriver[];
extern SQLHDBC         gZtqDbcHandle;
extern SQLHENV         gZtqEnvHandle;
extern dtlCharacterSet gZtqCharacterSet;
extern stlBool         gZtqAdmin;
extern stlBool         gZtqPrintErrorMessage;

stlBool gZtqCommunicationLinkFailure = STL_FALSE;

/**
 * @file ztqOdbcBridge.c
 * @brief Display Routines
 */

/**
 * @addtogroup ztqOdbcBridge
 * @{
 */

ztqDriverFuncMap gZtqDriverMap[] =
{
    {
        NULL,
        (SQLRETURN (SQL_API*)())SQLAllocHandle,
        "SQLAllocHandle",
        "SQLAllocHandle"
    },
    {
        NULL,
        (SQLRETURN (SQL_API*)())SQLAllocEnv,
        "SQLAllocEnv",
        "SQLAllocEnv"
    },
    {
        NULL,
        (SQLRETURN (SQL_API*)())SQLAllocConnect,
        "SQLAllocConnect",
        "SQLAllocConnect"
    },
    {
        NULL,
        (SQLRETURN (SQL_API*)())SQLAllocStmt,
        "SQLAllocStmt",
        "SQLAllocStmt"
    },
    {
        NULL,
        (SQLRETURN (SQL_API*)())SQLConnect,
        "SQLConnect",
        "SQLConnectW"
    },
    {
        NULL,
        (SQLRETURN (SQL_API*)())SQLDriverConnect,
        "SQLDriverConnect",
        "SQLDriverConnectW"
    },
    {
        NULL,
        (SQLRETURN (SQL_API*)())SQLSetEnvAttr,
        "SQLSetEnvAttr",
        "SQLSetEnvAttr"
    },
    {
        NULL,
        (SQLRETURN (SQL_API*)())SQLPrepare,
        "SQLPrepare",
        "SQLPrepareW"
    },
    {
        NULL,
        (SQLRETURN (SQL_API*)())SQLExecute,
        "SQLExecute",
        "SQLExecute"
    },
    {
        NULL,
        (SQLRETURN (SQL_API*)())SQLFetch,
        "SQLFetch",
        "SQLFetch"
    },
    {
        NULL,
        (SQLRETURN (SQL_API*)())SQLNumResultCols,
        "SQLNumResultCols",
        "SQLNumResultCols"
    },
    {
        NULL,
        (SQLRETURN (SQL_API*)())SQLBindCol,
        "SQLBindCol",
        "SQLBindCol"
    },
    {
        NULL,
        (SQLRETURN (SQL_API*)())SQLDescribeCol,
        "SQLDescribeCol",
        "SQLDescribeColW"
    },
    {
        NULL,
        (SQLRETURN (SQL_API*)())SQLGetDiagRec,
        "SQLGetDiagRec",
        "SQLGetDiagRecW"
    },
    {
        NULL,
        (SQLRETURN (SQL_API*)())SQLError,
        "SQLError",
        "SQLErrorW"
    },
    {
        NULL,
        (SQLRETURN (SQL_API*)())SQLExecDirect,
        "SQLExecDirect",
        "SQLExecDirectW"
    },
    {
        NULL,
        (SQLRETURN (SQL_API*)())SQLEndTran,
        "SQLEndTran",
        "SQLEndTran"
    },
    {
        NULL,
        (SQLRETURN (SQL_API*)())SQLTransact,
        "SQLTransact",
        "SQLTransact"
    },
    {
        NULL,
        (SQLRETURN (SQL_API*)())SQLRowCount,
        "SQLRowCount",
        "SQLRowCount"
    },
    {
        NULL,
        (SQLRETURN (SQL_API*)())SQLFreeHandle,
        "SQLFreeHandle",
        "SQLFreeHandle"
    },
    {
        NULL,
        (SQLRETURN (SQL_API*)())SQLFreeEnv,
        "SQLFreeEnv",
        "SQLFreeEnv"
    },
    {
        NULL,
        (SQLRETURN (SQL_API*)())SQLFreeConnect,
        "SQLFreeConnect",
        "SQLFreeConnect"
    },
    {
        NULL,
        (SQLRETURN (SQL_API*)())SQLFreeStmt,
        "SQLFreeStmt",
        "SQLFreeStmt"
    },
    {
        NULL,
        (SQLRETURN (SQL_API*)())SQLBindParameter,
        "SQLBindParameter",
        "SQLBindParameter"
    },
    {
        NULL,
        (SQLRETURN (SQL_API*)())SQLSetConnectAttr,
        "SQLSetConnectAttr",
        "SQLSetConnectAttrW"
    },
    {
        NULL,
        (SQLRETURN (SQL_API*)())SQLGetConnectAttr,
        "SQLGetConnectAttr",
        "SQLGetConnectAttrW"
    },
    {
        NULL,
        (SQLRETURN (SQL_API*)())SQLSetConnectOption,
        "SQLSetConnectOption",
        "SQLSetConnectOptionW"
    },
    {
        NULL,
        (SQLRETURN (SQL_API*)())SQLDisconnect,
        "SQLDisconnect",
        "SQLDisconnect"
    },
    {
        NULL,
        (SQLRETURN (SQL_API*)())SQLCloseCursor,
        "SQLCloseCursor",
        "SQLCloseCursor"
    },
    {
        NULL,
        (SQLRETURN (SQL_API*)())SQLSetStmtAttr,
        "SQLSetStmtAttr",
        "SQLSetStmtAttrW"
    },
    {
        NULL,
        (SQLRETURN (SQL_API*)())SQLGetStmtAttr,
        "SQLGetStmtAttr",
        "SQLGetStmtAttrW"
    },
    {
        NULL,
        (SQLRETURN (SQL_API*)())SQLGetDescField,
        "SQLGetDescField",
        "SQLGetDescFieldW"
    },
    {
        NULL,
        (SQLRETURN (SQL_API*)())SQLSetDescField,
        "SQLSetDescField",
        "SQLSetDescFieldW"
    },    
};

typedef SQLRETURN (SQL_API* ZTQ_SQLALLOCHANDLE_FUNC)( SQLSMALLINT,
                                                      SQLHANDLE,
                                                      SQLHANDLE );
typedef SQLRETURN (SQL_API* ZTQ_SQLALLOCENV_FUNC)( SQLHENV* );
typedef SQLRETURN (SQL_API* ZTQ_SQLALLOCCONNECT_FUNC)( SQLHENV,
                                                       SQLHDBC* );
typedef SQLRETURN (SQL_API* ZTQ_SQLALLOCSTMT_FUNC)( SQLHDBC,
                                                    SQLHSTMT* );
typedef SQLRETURN (SQL_API* ZTQ_SQLCONNECT_FUNC)( SQLHDBC,
                                                  SQLCHAR*,
                                                  SQLSMALLINT,
                                                  SQLCHAR*,
                                                  SQLSMALLINT,
                                                  SQLCHAR*,
                                                  SQLSMALLINT );
typedef SQLRETURN (SQL_API* ZTQ_SQLDRIVERCONNECT_FUNC)( SQLHDBC,
                                                        SQLHWND,
                                                        SQLCHAR*,
                                                        SQLSMALLINT,
                                                        SQLCHAR*,
                                                        SQLSMALLINT,
                                                        SQLSMALLINT*,
                                                        SQLUSMALLINT );
typedef SQLRETURN (SQL_API* ZTQ_SQLSETENVATTR_FUNC)( SQLHENV,
                                                     SQLINTEGER,
                                                     SQLPOINTER,
                                                     SQLINTEGER );
typedef SQLRETURN (SQL_API* ZTQ_SQLPREPARE_FUNC)( SQLHSTMT,
                                                  SQLCHAR*,
                                                  SQLINTEGER );
typedef SQLRETURN (SQL_API* ZTQ_SQLEXECUTE_FUNC)( SQLHSTMT );
typedef SQLRETURN (SQL_API* ZTQ_SQLFETCH_FUNC)( SQLHSTMT );
typedef SQLRETURN (SQL_API* ZTQ_SQLNUMRESULTCOLS_FUNC)( SQLHSTMT,
                                                        SQLSMALLINT* );
typedef SQLRETURN (SQL_API* ZTQ_SQLBINDCOL_FUNC)( SQLHSTMT,
                                                  SQLUSMALLINT,
                                                  SQLSMALLINT,
                                                  SQLPOINTER,
                                                  SQLLEN,
                                                  SQLLEN* );
typedef SQLRETURN (SQL_API* ZTQ_SQLDESCRIBECOL_FUNC)( SQLHSTMT,
                                                      SQLUSMALLINT,
                                                      SQLCHAR*,
                                                      SQLSMALLINT,
                                                      SQLSMALLINT*,
                                                      SQLSMALLINT*,
                                                      SQLULEN*,
                                                      SQLSMALLINT*,
                                                      SQLSMALLINT* );
typedef SQLRETURN (SQL_API* ZTQ_SQLGETDIAGREC_FUNC)( SQLSMALLINT,
                                                     SQLHANDLE,
                                                     SQLSMALLINT,
                                                     SQLCHAR*,
                                                     SQLINTEGER*,
                                                     SQLCHAR*,
                                                     SQLSMALLINT,
                                                     SQLSMALLINT* );
typedef SQLRETURN (SQL_API* ZTQ_SQLERROR_FUNC)( SQLHENV,
                                                SQLHDBC,
                                                SQLHSTMT,
                                                SQLCHAR*,
                                                SQLINTEGER*,
                                                SQLCHAR*,
                                                SQLSMALLINT,
                                                SQLSMALLINT* );
typedef SQLRETURN (SQL_API* ZTQ_SQLEXECDIRECT_FUNC)( SQLHSTMT,
                                                     SQLCHAR*,
                                                     SQLINTEGER );
typedef SQLRETURN (SQL_API* ZTQ_SQLENDTRAN_FUNC)( SQLSMALLINT,
                                                  SQLHANDLE,
                                                  SQLSMALLINT );
typedef SQLRETURN (SQL_API* ZTQ_SQLTRANSACT_FUNC)( SQLHENV,
                                                   SQLHDBC,
                                                   SQLUSMALLINT );
typedef SQLRETURN (SQL_API* ZTQ_SQLROWCOUNT_FUNC)( SQLHSTMT,
                                                   SQLLEN* );
typedef SQLRETURN (SQL_API* ZTQ_SQLFREEHANDLE_FUNC)( SQLSMALLINT,
                                                     SQLHANDLE );
typedef SQLRETURN (SQL_API* ZTQ_SQLFREEENV_FUNC)( SQLHENV );
typedef SQLRETURN (SQL_API* ZTQ_SQLFREECONNECT_FUNC)( SQLHDBC );
typedef SQLRETURN (SQL_API* ZTQ_SQLFREESTMT_FUNC)( SQLHSTMT,
                                                   SQLUSMALLINT );
typedef SQLRETURN (SQL_API* ZTQ_SQLBINDPARAMETER_FUNC)( SQLHSTMT,
                                                        SQLUSMALLINT,
                                                        SQLSMALLINT,
                                                        SQLSMALLINT,
                                                        SQLSMALLINT,
                                                        SQLULEN,
                                                        SQLSMALLINT,
                                                        SQLPOINTER,
                                                        SQLLEN,
                                                        SQLLEN* );
typedef SQLRETURN (SQL_API* ZTQ_SQLSETCONNECTATTR_FUNC)( SQLHDBC,
                                                         SQLINTEGER,
                                                         SQLPOINTER,
                                                         SQLINTEGER );
typedef SQLRETURN (SQL_API* ZTQ_SQLGETCONNECTATTR_FUNC)( SQLHDBC,
                                                         SQLINTEGER,
                                                         SQLPOINTER,
                                                         SQLINTEGER,
                                                         SQLINTEGER * );
typedef SQLRETURN (SQL_API* ZTQ_SQLSETCONNECTOPTION_FUNC)( SQLHDBC,
                                                           SQLUSMALLINT,
                                                           SQLULEN );
typedef SQLRETURN (SQL_API* ZTQ_SQLDISCONNECT_FUNC)( SQLHDBC );
typedef SQLRETURN (SQL_API* ZTQ_SQLCLOSECURSOR_FUNC)( SQLHSTMT );
typedef SQLRETURN (SQL_API* ZTQ_SQLSETSTMTATTR_FUNC)( SQLHSTMT,
                                                      SQLINTEGER,
                                                      SQLPOINTER,
                                                      SQLINTEGER );
typedef SQLRETURN (SQL_API* ZTQ_SQLGETSTMTATTR_FUNC)( SQLHSTMT,
                                                      SQLINTEGER,
                                                      SQLPOINTER,
                                                      SQLINTEGER,
                                                      SQLINTEGER* );
typedef SQLRETURN (SQL_API* ZTQ_SQLGETDESCFIELD_FUNC)( SQLHDESC,
                                                       SQLSMALLINT,
                                                       SQLSMALLINT,
                                                       SQLPOINTER,
                                                       SQLINTEGER,
                                                       SQLINTEGER* );

typedef SQLRETURN (SQL_API* ZTQ_SQLSETDESCFIELD_FUNC)( SQLHDESC,
                                                       SQLSMALLINT,
                                                       SQLSMALLINT,
                                                       SQLPOINTER,
                                                       SQLINTEGER );

typedef xa_switch_t * (*ZTQ_SQLGETXASWITCH_FUNC)();
typedef SQLHANDLE (*ZTQ_SQLGETXACONNECTIONHANDLE_FUNC)();

ZTQ_SQLGETXASWITCH_FUNC           gZtqGetXaSwitchFunc;
ZTQ_SQLGETXACONNECTIONHANDLE_FUNC gZtqGetXaConnectionHandleFunc;

#define ZTQ_SQLALLOCHANDLE      ((ZTQ_SQLALLOCHANDLE_FUNC)gZtqDriverMap[ZTQ_FUNC_ALLOCHANDLE].mFunction)
#define ZTQ_SQLALLOCENV         ((ZTQ_SQLALLOCENV_FUNC)gZtqDriverMap[ZTQ_FUNC_ALLOCENV].mFunction)
#define ZTQ_SQLALLOCCONNECT     ((ZTQ_SQLALLOCCONNECT_FUNC)gZtqDriverMap[ZTQ_FUNC_ALLOCCONNECT].mFunction)
#define ZTQ_SQLALLOCSTMT        ((ZTQ_SQLALLOCSTMT_FUNC)gZtqDriverMap[ZTQ_FUNC_ALLOCSTMT].mFunction)
#define ZTQ_SQLCONNECT          ((ZTQ_SQLCONNECT_FUNC)gZtqDriverMap[ZTQ_FUNC_CONNECT].mFunction)
#define ZTQ_SQLDRIVERCONNECT    ((ZTQ_SQLDRIVERCONNECT_FUNC)gZtqDriverMap[ZTQ_FUNC_DRIVERCONNECT].mFunction)
#define ZTQ_SQLSETENVATTR       ((ZTQ_SQLSETENVATTR_FUNC)gZtqDriverMap[ZTQ_FUNC_SETENVATTR].mFunction)
#define ZTQ_SQLPREPARE          ((ZTQ_SQLPREPARE_FUNC)gZtqDriverMap[ZTQ_FUNC_PREPARE].mFunction)
#define ZTQ_SQLEXECUTE          ((ZTQ_SQLEXECUTE_FUNC)gZtqDriverMap[ZTQ_FUNC_EXECUTE].mFunction)
#define ZTQ_SQLFETCH            ((ZTQ_SQLFETCH_FUNC)gZtqDriverMap[ZTQ_FUNC_FETCH].mFunction)
#define ZTQ_SQLNUMRESULTCOLS    ((ZTQ_SQLNUMRESULTCOLS_FUNC)gZtqDriverMap[ZTQ_FUNC_NUMRESULTCOLS].mFunction)
#define ZTQ_SQLBINDCOL          ((ZTQ_SQLBINDCOL_FUNC)gZtqDriverMap[ZTQ_FUNC_BINDCOL].mFunction)
#define ZTQ_SQLDESCRIBECOL      ((ZTQ_SQLDESCRIBECOL_FUNC)gZtqDriverMap[ZTQ_FUNC_DESCRIBECOL].mFunction)
#define ZTQ_SQLGETDIAGREC       ((ZTQ_SQLGETDIAGREC_FUNC)gZtqDriverMap[ZTQ_FUNC_GETDIAGREC].mFunction)
#define ZTQ_SQLERROR            ((ZTQ_SQLERROR_FUNC)gZtqDriverMap[ZTQ_FUNC_ERROR].mFunction)
#define ZTQ_SQLEXECDIRECT       ((ZTQ_SQLEXECDIRECT_FUNC)gZtqDriverMap[ZTQ_FUNC_EXECDIRECT].mFunction)
#define ZTQ_SQLENDTRAN          ((ZTQ_SQLENDTRAN_FUNC)gZtqDriverMap[ZTQ_FUNC_ENDTRAN].mFunction)
#define ZTQ_SQLTRANSACT         ((ZTQ_SQLTRANSACT_FUNC)gZtqDriverMap[ZTQ_FUNC_TRANSACT].mFunction)
#define ZTQ_SQLROWCOUNT         ((ZTQ_SQLROWCOUNT_FUNC)gZtqDriverMap[ZTQ_FUNC_ROWCOUNT].mFunction)
#define ZTQ_SQLFREEHANDLE       ((ZTQ_SQLFREEHANDLE_FUNC)gZtqDriverMap[ZTQ_FUNC_FREEHANDLE].mFunction)
#define ZTQ_SQLFREEENV          ((ZTQ_SQLFREEENV_FUNC)gZtqDriverMap[ZTQ_FUNC_FREEENV].mFunction)
#define ZTQ_SQLFREECONNECT      ((ZTQ_SQLFREECONNECT_FUNC)gZtqDriverMap[ZTQ_FUNC_FREECONNECT].mFunction)
#define ZTQ_SQLFREESTMT         ((ZTQ_SQLFREESTMT_FUNC)gZtqDriverMap[ZTQ_FUNC_FREESTMT].mFunction)
#define ZTQ_SQLBINDPARAMETER    ((ZTQ_SQLBINDPARAMETER_FUNC)gZtqDriverMap[ZTQ_FUNC_BINDPARAMETER].mFunction)
#define ZTQ_SQLSETCONNECTATTR   ((ZTQ_SQLSETCONNECTATTR_FUNC)gZtqDriverMap[ZTQ_FUNC_SETCONNECTATTR].mFunction)
#define ZTQ_SQLGETCONNECTATTR   ((ZTQ_SQLGETCONNECTATTR_FUNC)gZtqDriverMap[ZTQ_FUNC_GETCONNECTATTR].mFunction)
#define ZTQ_SQLSETCONNECTOPTION ((ZTQ_SQLSETCONNECTOPTION_FUNC)gZtqDriverMap[ZTQ_FUNC_SETCONNECTOPTION].mFunction)
#define ZTQ_SQLDISCONNECT       ((ZTQ_SQLDISCONNECT_FUNC)gZtqDriverMap[ZTQ_FUNC_DISCONNECT].mFunction)
#define ZTQ_SQLCLOSECURSOR      ((ZTQ_SQLCLOSECURSOR_FUNC)gZtqDriverMap[ZTQ_FUNC_CLOSECURSOR].mFunction)
#define ZTQ_SQLSETSTMTATTR      ((ZTQ_SQLSETSTMTATTR_FUNC)gZtqDriverMap[ZTQ_FUNC_SETSTMTATTR].mFunction)
#define ZTQ_SQLGETSTMTATTR      ((ZTQ_SQLGETSTMTATTR_FUNC)gZtqDriverMap[ZTQ_FUNC_GETSTMTATTR].mFunction)
#define ZTQ_SQLGETDESCFIELD     ((ZTQ_SQLGETDESCFIELD_FUNC)gZtqDriverMap[ZTQ_FUNC_GETDESCFIELD].mFunction)
#define ZTQ_SQLSETDESCFIELD     ((ZTQ_SQLSETDESCFIELD_FUNC)gZtqDriverMap[ZTQ_FUNC_SETDESCFIELD].mFunction)
#define ZTQ_SQLGETXASWITCH      ((ZTQ_SQLGETXASWITCH_FUNC)gZtqGetXaSwitchFunc)
#define ZTQ_SQLGETXACONNECTIONHANDLE   ((ZTQ_SQLGETXACONNECTIONHANDLE_FUNC)gZtqGetXaConnectionHandleFunc)

/**
 * @todo Windows 고려 필요
 */
#define ZTQ_GLIESECS_LIBRARY "libgoldilockscs"STL_SHARED_LIBRARY_SUFFIX

stlBool gZtqUnicodeDriver = STL_FALSE;
stlBool gZtqGliese        = STL_FALSE;

stlStatus ztqOpenDriver( stlChar        * aDriverStr,
                         void          ** aDriverHandle,
                         stlErrorStack  * aErrorStack )
{
    stlDsoHandle    sDriverHandle = NULL;
    stlDsoSymHandle sFunction;
    stlInt32    i;
    stlInt32    sState = 0;
    stlChar   * sPos = NULL;

    *aDriverHandle = NULL;

    /*
     * 사용자가 Driver 경로를 입력하지 않은 경우는
     * Gliese Static Library(C/S ODBC or Embedded ODBC)를
     * 사용한다.
     */ 
    if( stlStrlen( aDriverStr ) != 0 )
    {
        STL_TRY( stlOpenLibrary( aDriverStr,
                                 &sDriverHandle,
                                 aErrorStack )
                 == STL_SUCCESS );
        sState = 1;

        sPos = stlStrrchr( aDriverStr, STL_PATH_SEPARATOR_CHARACTER );

        if( sPos == NULL )
        {
            sPos = aDriverStr;
        }
        else
        {
            sPos++;
        }

        if( stlStrcmp( sPos, ZTQ_GLIESECS_LIBRARY ) == 0 )
        {
            gZtqGliese = STL_TRUE;
        }

        /*
         * Ansi ODBC 함수를 지원한다면 Ansi ODBC 함수들을 사용하고,
         * 그렇지 않다면 Unicode ODBC 함수의 사용을 시도한다.
         * CONNECT를 이용해서 Ansi ODBC를 지원하는지 여부를 판단한다.
         */
        if( stlGetSymbol( sDriverHandle,
                          gZtqDriverMap[ZTQ_FUNC_CONNECT].mFunctionName,
                          &sFunction,
                          aErrorStack )
            != STL_SUCCESS )
        {
            gZtqUnicodeDriver = STL_TRUE;
            (void)stlPopError( aErrorStack );
        }
        else
        {
            gZtqUnicodeDriver = STL_FALSE;
        }

        for( i = 0; i < ZTQ_MAX_FUNC; i++ )
        {
            sFunction = NULL;
            
            if( gZtqUnicodeDriver == STL_TRUE )
            {
                if( stlGetSymbol( sDriverHandle,
                                  gZtqDriverMap[i].mFunctionNameW,
                                  &sFunction,
                                  aErrorStack )
                    != STL_SUCCESS )
                {
                    (void)stlPopError( aErrorStack );
                }
            }
            else
            {
                if( stlGetSymbol( sDriverHandle,
                                  gZtqDriverMap[i].mFunctionName,
                                  &sFunction,
                                  aErrorStack )
                    != STL_SUCCESS )
                {
                    (void)stlPopError( aErrorStack );
                }
            }

            gZtqDriverMap[i].mFunction = (ztqOdbcBridgeFunc)sFunction;
        }

        if( stlGetSymbol( sDriverHandle,
                          "SQLGetXaSwitch",
                          &sFunction,
                          aErrorStack )
            != STL_SUCCESS )
        {
            gZtqGetXaSwitchFunc = NULL;
        }
        else
        {
            gZtqGetXaSwitchFunc = (ZTQ_SQLGETXASWITCH_FUNC)sFunction;
        }
        
        if( stlGetSymbol( sDriverHandle,
                          "SQLGetXaConnectionHandle",
                          &sFunction,
                          aErrorStack )
            != STL_SUCCESS )
        {
            gZtqGetXaConnectionHandleFunc = NULL;
        }
        else
        {
            gZtqGetXaConnectionHandleFunc = (ZTQ_SQLGETXACONNECTIONHANDLE_FUNC)sFunction;
        }
        
        *aDriverHandle = sDriverHandle;
    }
    else
    {
        for( i = 0; i < ZTQ_MAX_FUNC; i++ )
        {
            gZtqDriverMap[i].mFunction = gZtqDriverMap[i].mGlieseFunction;
        }

        gZtqGetXaSwitchFunc = SQLGetXaSwitch;
        gZtqGetXaConnectionHandleFunc = SQLGetXaConnectionHandle;
        gZtqGliese = STL_TRUE;
    }

    return STL_SUCCESS;
    
    STL_FINISH;

    if( sState == 1 )
    {
        (void)stlCloseLibrary( sDriverHandle, aErrorStack );
    }

    return STL_FAILURE;
}

stlStatus ztqCloseDriver( void          * aDriverHandle,
                          stlErrorStack * aErrorStack )
{
    if( aDriverHandle != NULL )
    {
        STL_TRY( stlCloseLibrary( aDriverHandle,
                                  aErrorStack )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlInt32 ztqStrlenW( SQLWCHAR * aString )
{
    stlInt32 i = 0;
    
    while( aString[ i ] )
    {
        i++;
    }

    return i;
}

/**
 * @brief SQLCHAR를 SQLWCHAR로 변환시킨다.
 * @todo 현재는 Ascii 문자만을 Wide 문자로 변환시킨다.
 *  <BR> 즉, SQL 상의 한글은 지원하지 못한다.
 *  <BR> Multi-Byte(UTF8)를 Wide(UTF16)로 변경하기 위해서는 DataType Layer의
 *  <BR> Conversion 모듈이 개발되어야 한다.
 */
stlStatus ztqCharToWchar( SQLCHAR       * aSrc,
                          SQLWCHAR      * aDest,
                          SQLINTEGER      aSrcLength,
                          stlErrorStack * aErrorStack )
{
    stlInt32 i = 0;
    stlInt32 j = 0;
    
    if( aSrcLength == SQL_NTS )
    {
        aSrcLength = stlStrlen( (const stlChar*)aSrc ) + 1;
    }
    
    for ( i = 0, j = 0; i < aSrcLength && aSrc[j] != 0; i++ )
    {
        if( (aSrc[j] & 0x80) == 0 )
        {
            aDest[i] = aSrc[j];
            j += 1;
        }
        else if( (aSrc[j] & 0xe0) == 0xc0 )
        {
            aDest[i] = (((aSrc[j] & 0x1f) << 6) |
                        (aSrc[j+1] & 0x3f));
            j += 2;
        }
        else if( (aSrc[j] & 0xf0) == 0xe0 )
        {
            aDest[i] = (((aSrc[j] & 0x0f) << 12)  |
                        ((aSrc[j+1] & 0x3f) << 6) |
                        (aSrc[j+2] & 0x3f));
            j += 3;
        }
        else if( (aSrc[j] & 0xf8) == 0xf0 )
        {
            aDest[i] = (((aSrc[j] & 0x07) << 18)   |
                        ((aSrc[j+1] & 0x3f) << 12) |
                        ((aSrc[j+2] & 0x3f) << 6)  |
                        (aSrc[j+3] & 0x3f));
            j += 4;
        }
        else
        {
            STL_THROW( RAMP_ERR_INVALID_CONVERSION );
        }
    }

    aDest[i] = 0;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_CONVERSION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_CHARACTER_IN_CHARSET_REPERTOIRE,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}
                        
/**
 * @brief SQLWCHAR를 SQLCHAR로 변환시킨다.
 */
stlStatus ztqWcharToChar( SQLWCHAR   * aSrc,
                          SQLCHAR    * aDest,
                          SQLINTEGER   aSrcLength )
{
    stlInt32 i = 0;
    stlInt32 j = 0;
    
    if( aSrcLength == SQL_NTS )
    {
        aSrcLength = ztqStrlenW( aSrc ) + 1;
    }
    
    for ( i = 0, j = 0; i < aSrcLength && aSrc[i] != 0; i++ )
    {
        if( aSrc[i] <= 0x7F )
        {
            aDest[j] = aSrc[i];
            j += 1;
        }
        else if( aSrc[i] <= 0x7FF )
        {
            aDest[j] = 0xC0   | ((aDest[i] >> 6) & 0x1F);
            aDest[j+1] = 0x80 | (aDest[i] & 0x3F);
            j += 2;
        }
#ifdef SQL_WCHART_CONVERT
        else if( aSrc[i] <= 0xFFFF )
        {
            aDest[j] = 0xE0   | ((aSrc[i] >> 12) & 0x0F);
            aDest[j+1] = 0x80 | ((aSrc[i] >> 6) & 0x3F);
            aDest[j+2] = 0x80 | (aSrc[i] & 0x3F);
            j += 3;
        }
        else
        {
            aDest[j] = 0xF0   | ((aSrc[i] >> 18) & 0x07);
            aDest[j+1] = 0x80 | ((aSrc[i] >> 12) & 0x3F);
            aDest[j+2] = 0x80 | ((aSrc[i] >> 6) & 0x3F);
            aDest[j+3] = 0x80 | (aSrc[i] & 0x3F);
            j += 4;
        }
#endif
    }

    aDest[j] = 0;
    
    return STL_SUCCESS;
}
                        
stlStatus ztqAllocWcharBuffer( SQLCHAR       * aSrc,
                               SQLWCHAR     ** aDest,
                               SQLINTEGER      aSrcLength,
                               stlAllocator  * aAllocator,
                               stlErrorStack * aErrorStack )
{
    if( aSrcLength == SQL_NTS )
    {
        aSrcLength = stlStrlen( (const stlChar*)aSrc ) + 1;
    }
    
    STL_TRY( stlAllocRegionMem( aAllocator,
                                STL_SIZEOF(SQLWCHAR) * (aSrcLength + 1),
                                (void**)aDest,
                                aErrorStack )
             == STL_SUCCESS );
                          
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
                        
stlStatus ztqAllocCharBuffer( SQLWCHAR      * aSrc,
                              SQLCHAR      ** aDest,
                              SQLINTEGER      aSrcLength,
                              stlAllocator  * aAllocator,
                              stlErrorStack * aErrorStack )
{
    if( aSrcLength == SQL_NTS )
    {
        aSrcLength = ztqStrlenW( aSrc ) + 1;
    }
    
    STL_TRY( stlAllocRegionMem( aAllocator,
                                (STL_SIZEOF(SQLCHAR) *
                                 dtlGetMbMaxLength(gZtqCharacterSet)) *
                                (aSrcLength + 1),
                                (void**)aDest,
                                aErrorStack )
             == STL_SUCCESS );
                          
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlInt32 ztqGetNativeTopError( SQLHANDLE       aHandle,
                               SQLSMALLINT     aHandleType,
                               stlErrorStack * aErrorStack )
{
    SQLRETURN     sReturn;
    SQLCHAR       sSqlState[32];
    SQLINTEGER    sNativeError;
    SQLCHAR       sMessageText[1024];
    SQLSMALLINT   sMessageLength;
    SQLSMALLINT   sRecNumber = 1;
    stlInt32      sErrorCode = 0;
    
    ztqSQLGetDiagRec( aHandleType,
                      aHandle,
                      sRecNumber,
                      sSqlState,
                      &sNativeError,
                      sMessageText,
                      1024,
                      &sMessageLength,
                      &sReturn,
                      aErrorStack );
    
    if( sReturn != SQL_NO_DATA )
    {
        sErrorCode = sNativeError;
    }

    return sErrorCode;
}

stlInt32 ztqGetNativeErrorByRecNumber(SQLHANDLE       aHandle,
                                      SQLSMALLINT     aHandleType,
                                      SQLSMALLINT     aRecNumber,
                                      stlErrorStack * aErrorStack )
{
    SQLRETURN     sReturn;
    SQLCHAR       sSqlState[32];
    SQLINTEGER    sNativeError;
    SQLCHAR       sMessageText[1024];
    SQLSMALLINT   sMessageLength;
    stlInt32      sErrorCode = 0;
    
    ztqSQLGetDiagRec( aHandleType,
                      aHandle,
                      aRecNumber,
                      sSqlState,
                      &sNativeError,
                      sMessageText,
                      1024,
                      &sMessageLength,
                      &sReturn,
                      aErrorStack );
    
    if( sReturn != SQL_NO_DATA )
    {
        sErrorCode = sNativeError;
    }

    return sErrorCode;
}

stlStatus ztqPrintError( SQLHANDLE       aHandle,
                         SQLSMALLINT     aHandleType,
                         stlErrorStack * aErrorStack )
{
    SQLRETURN     sReturn;
    SQLCHAR       sSqlState[32];
    SQLINTEGER    sNativeError;
    SQLCHAR       sMessageText[1024];
    stlChar     * sRefinedMessageText;
    SQLSMALLINT   sMessageLength;
    SQLSMALLINT   sRecNumber = 1;
    SQLCHAR     * sEndPtr = NULL;
    stlInt32      sErrorCode;
    
    while( 1 )
    {
        STL_TRY( ztqSQLGetDiagRec( aHandleType,
                                   aHandle,
                                   sRecNumber,
                                   sSqlState,
                                   &sNativeError,
                                   sMessageText,
                                   1024,
                                   &sMessageLength,
                                   &sReturn,
                                   aErrorStack )
                 == STL_SUCCESS );

        if( sReturn == SQL_NO_DATA )
        {
            break;
        }

        sRecNumber++;

        /*
         * Error Message에서 new line을 제거한다.
         */
        sEndPtr = sMessageText + stlStrlen( (stlChar*)sMessageText ) - 1;
        while( stlIsspace( *sEndPtr ) == STL_TRUE )
        {
            sEndPtr--;
        }
        *(sEndPtr + 1) = '\0';

        /*
         * 외부 ODBC Driver를 사용하는 경우(Driver를 명시적으로 지정한 경우)는
         * Error Prefix를 제거하지 않는 반면,
         * 내부 gliese library(embedded odbc)와 goldilocks library(c/s odbc)를
         * 사용하는 경우는 Error Prefix([vendor][odbc component][data source])를
         * 제거해서 출력한다
         */
        if( gZtqGliese == STL_TRUE )
        {
            sSqlState[5] = '\0';
            
            /*
             * front vendor string 제거
             */
            sRefinedMessageText = ( stlStrstr( (stlChar*)sMessageText,
                                               ZLL_ODBC_COMPONENT_IDENTIFIER ) +
                                    stlStrlen( ZLL_ODBC_COMPONENT_IDENTIFIER ));
            /*
             * front data source 제거
             */
            sRefinedMessageText = ( stlStrstr( (stlChar*)sRefinedMessageText,
                                               ZLL_DATA_SOURCE_IDENTIFIER ) +
                                    stlStrlen( ZLL_DATA_SOURCE_IDENTIFIER ));
            
            while( stlIsspace( *sRefinedMessageText ) == STL_TRUE )
            {
                sRefinedMessageText++;
            }

            sErrorCode = sNativeError;

            if( gZtqCommunicationLinkFailure == STL_FALSE )
            {
                if( gZtqPrintErrorMessage == STL_TRUE )
                {
                    ztqPaintf( &gZtqErrorPaintAttr,
                               "ERR-%s(%d): %s\n",
                               sSqlState,
                               sErrorCode,
                               sRefinedMessageText );
                }
                else
                {
                    ztqPaintf( &gZtqErrorPaintAttr,
                               "ERR-%s(%d)\n",
                               sSqlState,
                               sErrorCode );
                }
            }
        }
        else
        {
            sSqlState[5] = '\0';
            if( gZtqCommunicationLinkFailure == STL_FALSE )
            {
                ztqPaintf( &gZtqErrorPaintAttr,
                           "ERR-%s: %s\n",
                           sSqlState,
                           sMessageText );
            }
        }

        /*
         * Communication link failure
         */
        if( stlStrcasecmp( (stlChar*)sSqlState, "08S01" ) == 0 )
        {
            gZtqCommunicationLinkFailure = STL_TRUE;            
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
                        
stlStatus ztqSQLAllocHandle( SQLSMALLINT     aHandleType,
                             SQLHANDLE       aInputHandle,
                             SQLHANDLE     * aOutputHandle,
                             stlErrorStack * aErrorStack )
{
    SQLRETURN sReturn = SQL_SUCCESS;

    if( ZTQ_SQLALLOCHANDLE == NULL )
    {
        switch( aHandleType )
        {
            case SQL_HANDLE_ENV:
                STL_TRY_THROW( ZTQ_SQLALLOCENV != NULL, RAMP_ERR_SYMBOL );
                sReturn = ZTQ_SQLALLOCENV( aOutputHandle );
                break;
            case SQL_HANDLE_DBC:
                STL_ASSERT( gZtqEnvHandle != NULL );
                STL_TRY_THROW( ZTQ_SQLALLOCCONNECT != NULL, RAMP_ERR_SYMBOL );
                sReturn = ZTQ_SQLALLOCCONNECT( gZtqEnvHandle,
                                               aOutputHandle );
                break;
            case SQL_HANDLE_STMT:
                STL_ASSERT( gZtqEnvHandle != NULL );
                STL_ASSERT( gZtqDbcHandle != NULL );
                STL_TRY_THROW( ZTQ_SQLALLOCSTMT != NULL, RAMP_ERR_SYMBOL );
                sReturn = ZTQ_SQLALLOCSTMT( gZtqDbcHandle,
                                            aOutputHandle );
                break;
            default:
                STL_ASSERT( 0 );
                break;
        }
    }
    else
    {
        sReturn = ZTQ_SQLALLOCHANDLE( aHandleType,
                                      aInputHandle,
                                      aOutputHandle );
    }

    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_ALLOCHANDLE );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_ALLOCHANDLE )
    {
        switch( aHandleType )
        {
            case SQL_HANDLE_DBC:
                (void)ztqPrintError( aInputHandle,
                                     SQL_HANDLE_ENV,
                                     aErrorStack );
                break;
            case SQL_HANDLE_STMT:
                (void)ztqPrintError( aInputHandle,
                                     SQL_HANDLE_DBC,
                                     aErrorStack );
                break;
            default:
                break;
        }
    }

    STL_CATCH( RAMP_ERR_SYMBOL )
    {
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqSQLConnect( SQLHDBC         aConnectionHandle,
                         SQLCHAR       * aServerName,
                         SQLSMALLINT     aNameLength1,
                         SQLCHAR       * aUserName,
                         SQLSMALLINT     aNameLength2,
                         SQLCHAR       * aAuthentication,
                         SQLSMALLINT     aNameLength3,
                         stlAllocator  * aAllocator,
                         stlErrorStack * aErrorStack )
{
    SQLRETURN   sReturn;
    SQLWCHAR  * sServerName;
    SQLWCHAR  * sUserName;
    SQLWCHAR  * sAuthentication;

    if( gZtqUnicodeDriver == STL_TRUE )
    {
        STL_TRY( ztqAllocWcharBuffer( aServerName,
                                      &sServerName,
                                      aNameLength1,
                                      aAllocator,
                                      aErrorStack )
                 == STL_SUCCESS );

        STL_TRY( ztqCharToWchar( aServerName,
                                 sServerName,
                                 aNameLength1,
                                 aErrorStack )
                 == STL_SUCCESS );
        
        STL_TRY( ztqAllocWcharBuffer( aUserName,
                                      &sUserName,
                                      aNameLength2,
                                      aAllocator,
                                      aErrorStack )
                 == STL_SUCCESS );

        STL_TRY( ztqCharToWchar( aUserName,
                                 sUserName,
                                 aNameLength2,
                                 aErrorStack )
                 == STL_SUCCESS );
        
        STL_TRY( ztqAllocWcharBuffer( aAuthentication,
                                      &sAuthentication,
                                      aNameLength3,
                                      aAllocator,
                                      aErrorStack )
                 == STL_SUCCESS );

        STL_TRY( ztqCharToWchar( aAuthentication,
                                 sAuthentication,
                                 aNameLength3,
                                 aErrorStack )
                 == STL_SUCCESS );
        
        sReturn = ZTQ_SQLCONNECT( aConnectionHandle,
                                  (SQLCHAR*)sServerName,
                                  aNameLength1,
                                  (SQLCHAR*)sUserName,
                                  aNameLength2,
                                  (SQLCHAR*)sAuthentication,
                                  aNameLength3 );
    }
    else
    {
        sReturn = ZTQ_SQLCONNECT( aConnectionHandle,
                                  aServerName,
                                  aNameLength1,
                                  aUserName,
                                  aNameLength2,
                                  aAuthentication,
                                  aNameLength3 );
    }

    switch( sReturn )
    {
        case SQL_SUCCESS :
            break;
        case SQL_SUCCESS_WITH_INFO :
            (void)ztqPrintError( aConnectionHandle,
                                 SQL_HANDLE_DBC,
                                 aErrorStack );
            break;
        default :
            STL_THROW( RAMP_ERR_CONNECTION );
            break;
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CONNECTION )
    {
        if( gZtqAdmin == STL_FALSE )
        {
            (void)ztqPrintError( aConnectionHandle,
                                 SQL_HANDLE_DBC,
                                 aErrorStack );
        }
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqSQLDriverConnect( SQLHDBC         aConnectionHandle,
                               SQLCHAR       * aConnString,
                               SQLSMALLINT     aConnStringLength,
                               stlAllocator  * aAllocator,
                               stlErrorStack * aErrorStack )
{
    SQLRETURN     sReturn;
    SQLWCHAR    * sConnString;
    SQLSMALLINT   sStringLength2 = 0;

    STL_TRY_THROW( ZTQ_SQLDRIVERCONNECT != NULL, RAMP_ERR_SYMBOL );

    if( gZtqUnicodeDriver == STL_TRUE )
    {
        STL_TRY( ztqAllocWcharBuffer( aConnString,
                                      &sConnString,
                                      aConnStringLength,
                                      aAllocator,
                                      aErrorStack )
                 == STL_SUCCESS );

        STL_TRY( ztqCharToWchar( aConnString,
                                 sConnString,
                                 aConnStringLength,
                                 aErrorStack )
                 == STL_SUCCESS );
        
        sReturn = ZTQ_SQLDRIVERCONNECT( aConnectionHandle,
                                        NULL,   /* aWindowHandle */
                                        (SQLCHAR*)sConnString,
                                        aConnStringLength,
                                        NULL,   /* aOutConnString */
                                        0,      /* aOutConnStringLength */
                                        &sStringLength2,
                                        SQL_DRIVER_NOPROMPT );
    }
    else
    {
        sReturn = ZTQ_SQLDRIVERCONNECT( aConnectionHandle,
                                        NULL,   /* aWindowHandle */
                                        aConnString,
                                        aConnStringLength,
                                        NULL,   /* aOutConnString */
                                        0,      /* aOutConnStringLength */
                                        &sStringLength2,
                                        SQL_DRIVER_NOPROMPT );
    }

    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_CONNECTION );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CONNECTION )
    {
        (void)ztqPrintError( aConnectionHandle,
                             SQL_HANDLE_DBC,
                             aErrorStack );
    }

    STL_CATCH( RAMP_ERR_SYMBOL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_NOT_SUPPORT_FUNCTION,
                      NULL,
                      aErrorStack,
                      "SQLDriverDisconnect" );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqSQLSetEnvAttr( SQLHENV         aEnvHandle,
                            SQLINTEGER      aAttribute,
                            SQLPOINTER      aValuePtr,
                            SQLINTEGER      aStringLength,
                            stlErrorStack * aErrorStack  )
{
    SQLRETURN sReturn;

    if( ZTQ_SQLSETENVATTR != NULL )
    {
        sReturn = ZTQ_SQLSETENVATTR( aEnvHandle,
                                     aAttribute,
                                     aValuePtr,
                                     aStringLength );

        STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_SETENVATTR );
    }
    else
    {
        /*
         * 해당 함수를 지원하지 않는다면 Environment 설정은 무시 가능하다.
         */
        sReturn = SQL_SUCCESS;
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SETENVATTR )
    {
        (void)ztqPrintError( aEnvHandle,
                             SQL_HANDLE_ENV,
                             aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqSQLPrepare( SQLHSTMT        aStmtHandle,
                         SQLCHAR       * aStmtText,
                         SQLINTEGER      aTextLength,
                         stlAllocator  * aAllocator,
                         stlErrorStack * aErrorStack )
{
    SQLRETURN   sReturn;
    SQLWCHAR  * sStmtTextW;
    
    if( gZtqUnicodeDriver == STL_TRUE )
    {
        STL_TRY( ztqAllocWcharBuffer( aStmtText,
                                      &sStmtTextW,
                                      aTextLength,
                                      aAllocator,
                                      aErrorStack )
                 == STL_SUCCESS );

        STL_TRY( ztqCharToWchar( aStmtText,
                                 sStmtTextW,
                                 aTextLength,
                                 aErrorStack )
                 == STL_SUCCESS );
        
        sReturn = ZTQ_SQLPREPARE( aStmtHandle,
                                  (SQLCHAR*)sStmtTextW,
                                  aTextLength );
    }
    else
    {
        sReturn = ZTQ_SQLPREPARE( aStmtHandle,
                                  aStmtText,
                                  aTextLength );
    }

    switch( sReturn )
    {
        case SQL_SUCCESS :
            break;
        case SQL_SUCCESS_WITH_INFO :
            (void)ztqPrintError( aStmtHandle,
                                 SQL_HANDLE_STMT,
                                 aErrorStack );
            break;
        default :
            STL_THROW( RAMP_ERR_PREPARE );
            break;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_PREPARE )
    {
        (void)ztqPrintError( aStmtHandle,
                             SQL_HANDLE_STMT,
                             aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqSQLExecute( SQLHSTMT        aStmtHandle,
                         SQLRETURN     * aReturn,
                         stlErrorStack * aErrorStack )
{
    SQLRETURN sReturn;
    
    sReturn = ZTQ_SQLEXECUTE( aStmtHandle );

    switch( sReturn )
    {
        case SQL_SUCCESS :
        case SQL_NO_DATA :
            break;
        case SQL_SUCCESS_WITH_INFO :
            (void)ztqPrintError( aStmtHandle,
                                 SQL_HANDLE_STMT,
                                 aErrorStack );
            break;
        default :
            STL_THROW( RAMP_ERR_EXECUTE );
            break;
    }

    *aReturn = sReturn;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_EXECUTE )
    {
        (void)ztqPrintError( aStmtHandle,
                             SQL_HANDLE_STMT,
                             aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqSQLFetch( SQLHSTMT        aStmtHandle,
                       SQLRETURN     * aReturn,
                       stlErrorStack * aErrorStack )
{
    SQLRETURN sReturn;
    
    sReturn = ZTQ_SQLFETCH( aStmtHandle );

    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ) || (sReturn == SQL_NO_DATA),
                   RAMP_ERR_FETCH );

    *aReturn = sReturn;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_FETCH )
    {
        (void)ztqPrintError( aStmtHandle,
                             SQL_HANDLE_STMT,
                             aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqSQLNumResultCols( SQLHSTMT        aStmtHandle,
                               SQLSMALLINT   * aColumnCount,
                               stlErrorStack * aErrorStack )
{
    SQLRETURN sReturn;
    
    sReturn = ZTQ_SQLNUMRESULTCOLS( aStmtHandle,
                                    aColumnCount );

    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_NUMRESULTCOLS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NUMRESULTCOLS )
    {
        (void)ztqPrintError( aStmtHandle,
                             SQL_HANDLE_STMT,
                             aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqSQLDescribeCol( SQLHSTMT        aStmtHandle,
                             SQLUSMALLINT    aColumnNumber,
                             SQLCHAR       * aColumnName,
                             SQLSMALLINT     aBufferLength,
                             SQLSMALLINT   * aNameLengthPtr,
                             SQLSMALLINT   * aDataTypePtr,
                             SQLULEN       * aColumnSizePtr,
                             SQLSMALLINT   * aDecimalDigitsPtr,
                             SQLSMALLINT   * aNullablePtr,
                             stlAllocator  * aAllocator,
                             stlErrorStack * aErrorStack )
{
    SQLRETURN   sReturn;
    SQLWCHAR  * sColumnNameW;
    
    if( gZtqUnicodeDriver == STL_TRUE )
    {
        STL_TRY( ztqAllocWcharBuffer( aColumnName,
                                      &sColumnNameW,
                                      aBufferLength,
                                      aAllocator,
                                      aErrorStack )
                 == STL_SUCCESS );

        sReturn = ZTQ_SQLDESCRIBECOL( aStmtHandle,
                                      aColumnNumber,
                                      (SQLCHAR*)sColumnNameW,
                                      aBufferLength,
                                      aNameLengthPtr,
                                      aDataTypePtr,
                                      aColumnSizePtr,
                                      aDecimalDigitsPtr,
                                      aNullablePtr );
        
        STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_DESCRIBECOL );
    
        STL_TRY( ztqWcharToChar( sColumnNameW,
                                 aColumnName,
                                 SQL_NTS )
                 == STL_SUCCESS );
    }
    else
    {
        sReturn = ZTQ_SQLDESCRIBECOL( aStmtHandle,
                                      aColumnNumber,
                                      aColumnName,
                                      aBufferLength,
                                      aNameLengthPtr,
                                      aDataTypePtr,
                                      aColumnSizePtr,
                                      aDecimalDigitsPtr,
                                      aNullablePtr );
    }

    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_DESCRIBECOL );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_DESCRIBECOL )
    {
        (void)ztqPrintError( aStmtHandle,
                             SQL_HANDLE_STMT,
                             aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqSQLBindCol( SQLHSTMT        aStmtHandle,
                         SQLUSMALLINT    aColumnNumber,
                         SQLSMALLINT     aTargetType,
                         SQLPOINTER      aTargetValuePtr,
                         SQLLEN          aBufferLength,
                         SQLLEN        * aIndicator,
                         stlErrorStack * aErrorStack )
{
    SQLRETURN sReturn;
    
    sReturn = ZTQ_SQLBINDCOL( aStmtHandle,
                              aColumnNumber,
                              aTargetType,
                              aTargetValuePtr,
                              aBufferLength,
                              aIndicator );

    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_BINDCOL );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_BINDCOL )
    {
        (void)ztqPrintError( aStmtHandle,
                             SQL_HANDLE_STMT,
                             aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztqSQLExecDirect( SQLHSTMT        aStmtHandle,
                            SQLCHAR       * aStmtText,
                            SQLINTEGER      aTextLength,
                            SQLRETURN     * aReturn,
                            stlAllocator  * aAllocator,
                            stlErrorStack * aErrorStack )
{
    SQLRETURN   sReturn;
    SQLWCHAR  * sStmtTextW;
    
    if( gZtqUnicodeDriver == STL_TRUE )
    {
        STL_TRY( ztqAllocWcharBuffer( aStmtText,
                                      &sStmtTextW,
                                      aTextLength,
                                      aAllocator,
                                      aErrorStack )
                 == STL_SUCCESS );

        STL_TRY( ztqCharToWchar( aStmtText,
                                 sStmtTextW,
                                 aTextLength,
                                 aErrorStack )
                 == STL_SUCCESS );
        
        sReturn = ZTQ_SQLEXECDIRECT( aStmtHandle,
                                     (SQLCHAR*)sStmtTextW,
                                     aTextLength );
    }
    else
    {
        sReturn = ZTQ_SQLEXECDIRECT( aStmtHandle,
                                     aStmtText,
                                     aTextLength );
    }

    switch( sReturn )
    {
        case SQL_SUCCESS :
        case SQL_NO_DATA :
            break;
        case SQL_SUCCESS_WITH_INFO :
            (void)ztqPrintError( aStmtHandle,
                                 SQL_HANDLE_STMT,
                                 aErrorStack );
            break;
        default :
            STL_THROW( RAMP_ERR_EXECDIRECT );
            break;
    }

    *aReturn = sReturn;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_EXECDIRECT )
    {
        (void)ztqPrintError( aStmtHandle,
                             SQL_HANDLE_STMT,
                             aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}



stlStatus ztqDescPrepare( SQLHSTMT        aStmtHandle,
                          SQLCHAR       * aStmtText,
                          SQLINTEGER      aTextLength,
                          SQLRETURN     * aReturn,
                          stlAllocator  * aAllocator,
                          stlErrorStack * aErrorStack )
{
    SQLRETURN   sReturn;
    SQLWCHAR  * sStmtTextW;
    
    if( gZtqUnicodeDriver == STL_TRUE )
    {
        STL_TRY( ztqAllocWcharBuffer( aStmtText,
                                      &sStmtTextW,
                                      aTextLength,
                                      aAllocator,
                                      aErrorStack )
                 == STL_SUCCESS );

        STL_TRY( ztqCharToWchar( aStmtText,
                                 sStmtTextW,
                                 aTextLength,
                                 aErrorStack )
                 == STL_SUCCESS );
        
        sReturn = ZTQ_SQLPREPARE( aStmtHandle,
                                  (SQLCHAR*)sStmtTextW,
                                  aTextLength );
    }
    else
    {
        sReturn = ZTQ_SQLPREPARE( aStmtHandle,
                                  aStmtText,
                                  aTextLength );
    }

    *aReturn = sReturn;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztqDescExecDirect( SQLHSTMT        aStmtHandle,
                             SQLCHAR       * aStmtText,
                             SQLINTEGER      aTextLength,
                             SQLRETURN     * aReturn,
                             stlAllocator  * aAllocator,
                             stlErrorStack * aErrorStack )
{
    SQLRETURN   sReturn;
    SQLWCHAR  * sStmtTextW;
    
    if( gZtqUnicodeDriver == STL_TRUE )
    {
        STL_TRY( ztqAllocWcharBuffer( aStmtText,
                                      &sStmtTextW,
                                      aTextLength,
                                      aAllocator,
                                      aErrorStack )
                 == STL_SUCCESS );

        STL_TRY( ztqCharToWchar( aStmtText,
                                 sStmtTextW,
                                 aTextLength,
                                 aErrorStack )
                 == STL_SUCCESS );
        
        sReturn = ZTQ_SQLEXECDIRECT( aStmtHandle,
                                     (SQLCHAR*)sStmtTextW,
                                     aTextLength );
    }
    else
    {
        sReturn = ZTQ_SQLEXECDIRECT( aStmtHandle,
                                     aStmtText,
                                     aTextLength );
    }

    *aReturn = sReturn;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqSQLEndTran( SQLSMALLINT     aHandleType,
                         SQLHANDLE       aHandle,
                         SQLSMALLINT     aCompletionType,
                         stlBool         aIgnoreFailure,
                         stlErrorStack * aErrorStack )
{
    SQLRETURN   sReturn;

    if( ZTQ_SQLENDTRAN == NULL )
    {
        STL_TRY_THROW( ZTQ_SQLTRANSACT != NULL, RAMP_ERR_SYMBOL );
        sReturn = ZTQ_SQLTRANSACT( gZtqEnvHandle,
                                   gZtqDbcHandle,
                                   aCompletionType );
    }
    else
    {
        sReturn = ZTQ_SQLENDTRAN( aHandleType,
                                  aHandle,
                                  aCompletionType );
    }

    if( aIgnoreFailure == STL_FALSE )
    {
        STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_ENDTRAN );
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_ENDTRAN )
    {
        (void)ztqPrintError( aHandle,
                             aHandleType,
                             aErrorStack );
    }

    STL_CATCH( RAMP_ERR_SYMBOL )
    {
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqSQLRowCount( SQLHSTMT        aStmtHandle,
                          SQLLEN        * aRowCountPtr,
                          stlErrorStack * aErrorStack )
{
    SQLRETURN   sReturn;
    
    sReturn = ZTQ_SQLROWCOUNT( aStmtHandle,
                               aRowCountPtr );

    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_ROWCOUNT );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_ROWCOUNT )
    {
        (void)ztqPrintError( aStmtHandle,
                             SQL_HANDLE_STMT,
                             aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqSQLFreeHandle( SQLSMALLINT     aHandleType,
                            SQLHANDLE       aHandle,
                            stlErrorStack * aErrorStack )
{
    SQLRETURN sReturn = SQL_SUCCESS;
    
    if( ZTQ_SQLFREEHANDLE == NULL )
    {
        switch( aHandleType )
        {
            case SQL_HANDLE_ENV:
                STL_TRY_THROW( ZTQ_SQLFREEENV != NULL, RAMP_ERR_SYMBOL );
                sReturn = ZTQ_SQLFREEENV( aHandle );
                break;
            case SQL_HANDLE_DBC:
                STL_ASSERT( gZtqEnvHandle != NULL );
                STL_TRY_THROW( ZTQ_SQLFREECONNECT != NULL, RAMP_ERR_SYMBOL );
                sReturn = ZTQ_SQLFREECONNECT( aHandle );
                break;
            case SQL_HANDLE_STMT:
                STL_ASSERT( gZtqEnvHandle != NULL );
                STL_ASSERT( gZtqDbcHandle != NULL );
                STL_TRY_THROW( ZTQ_SQLFREESTMT != NULL, RAMP_ERR_SYMBOL );
                sReturn = ZTQ_SQLFREESTMT( aHandle, SQL_DROP );
                break;
            default:
                STL_ASSERT( 0 );
                break;
        }
    }
    else
    {
        sReturn = ZTQ_SQLFREEHANDLE( aHandleType,
                                     aHandle );
    }

    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_FREEHANDLE );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_FREEHANDLE )
    {
        (void)ztqPrintError( aHandle,
                             aHandleType,
                             aErrorStack );
    }

    STL_CATCH( RAMP_ERR_SYMBOL )
    {
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqSQLFreeStmt( SQLHANDLE       aHandle,
                          SQLUSMALLINT    aOption,
                          stlErrorStack * aErrorStack )
{
    SQLRETURN sReturn = SQL_SUCCESS;

    /*
     * Parameter Validation
     */
    STL_ASSERT( gZtqEnvHandle != NULL );
    STL_ASSERT( gZtqDbcHandle != NULL );
    STL_ASSERT( (aOption == SQL_DROP) ||
                (aOption == SQL_CLOSE) ||
                (aOption == SQL_UNBIND) ||
                (aOption == SQL_RESET_PARAMS) );
                
    STL_TRY_THROW( ZTQ_SQLFREESTMT != NULL, RAMP_ERR_SYMBOL );

    /*
     * 옵션에 대한 SQLFreeStmt() 호출
     */
    
    sReturn = ZTQ_SQLFREESTMT( aHandle, aOption );

    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_FREEHANDLE );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_SYMBOL )
    {
    }

    STL_CATCH( RAMP_ERR_FREEHANDLE )
    {
        (void)ztqPrintError( aHandle,
                             SQL_HANDLE_STMT,
                             aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqSQLBindParameter( SQLHSTMT        aStmtHandle,
                               SQLUSMALLINT    aParameterNumber,
                               SQLSMALLINT     aInputOutputType,
                               SQLSMALLINT     aValueType,
                               SQLSMALLINT     aParameterType,
                               SQLULEN         aColumnSize,
                               SQLSMALLINT     aDecimalDigits,
                               SQLPOINTER      aParameterValuePtr,
                               SQLLEN          aBufferLength,
                               SQLLEN        * aIndicator,
                               stlErrorStack * aErrorStack )
{
    SQLRETURN   sReturn;
    
    sReturn = ZTQ_SQLBINDPARAMETER( aStmtHandle,
                                    aParameterNumber,
                                    aInputOutputType,
                                    aValueType,
                                    aParameterType,
                                    aColumnSize,
                                    aDecimalDigits,
                                    aParameterValuePtr,
                                    aBufferLength,
                                    aIndicator );

    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_BINDPARAMETER );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_BINDPARAMETER )
    {
        (void)ztqPrintError( aStmtHandle,
                             SQL_HANDLE_STMT,
                             aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqSQLSetConnectAttr( SQLHDBC         aConnectionHandle,
                                SQLINTEGER      aAttribute,
                                SQLPOINTER      aValuePtr,
                                SQLINTEGER      aStringLength,
                                stlErrorStack * aErrorStack )
{
    SQLRETURN   sReturn;
    SQLULEN     sValue;

    if( ZTQ_SQLSETCONNECTATTR == NULL )
    {
        STL_TRY_THROW( ZTQ_SQLSETCONNECTOPTION != NULL, RAMP_ERR_SYMBOL );

        sValue = *((SQLULEN*)aValuePtr);
        sReturn = ZTQ_SQLSETCONNECTOPTION( aConnectionHandle,
                                           aAttribute,
                                           sValue );
    }
    else
    {
        sReturn = ZTQ_SQLSETCONNECTATTR( aConnectionHandle,
                                         aAttribute,
                                         aValuePtr,
                                         aStringLength );
    }

    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_SETCONNECTATTR );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SETCONNECTATTR )
    {
        (void)ztqPrintError( aConnectionHandle,
                             SQL_HANDLE_DBC,
                             aErrorStack );
    }

    STL_CATCH( RAMP_ERR_SYMBOL )
    {
    }

    STL_FINISH;

    return STL_FAILURE;
}
                         
stlStatus ztqSQLGetConnectAttr( SQLHDBC         aConnectionHandle,
                                SQLINTEGER      aAttribute,
                                SQLPOINTER      aValuePtr,
                                SQLINTEGER      aBufferLength,
                                SQLINTEGER    * aStringLengthPtr,
                                stlErrorStack * aErrorStack )
{
    SQLRETURN sReturn;

    STL_TRY_THROW( ZTQ_SQLGETCONNECTATTR != NULL, RAMP_ERR_SYMBOL );
    
    sReturn = ZTQ_SQLGETCONNECTATTR( aConnectionHandle,
                                     aAttribute,
                                     aValuePtr,
                                     aBufferLength,
                                     aStringLengthPtr );

    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_GETCONNECTATTR );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_GETCONNECTATTR )
    {
        (void)ztqPrintError( aConnectionHandle,
                             SQL_HANDLE_DBC,
                             aErrorStack );
    }

    STL_CATCH( RAMP_ERR_SYMBOL )
    {
    }

    STL_FINISH;

    return STL_FAILURE;
}
                         
stlStatus ztqSQLDisconnect( SQLHDBC         aConnectionHandle,
                            stlErrorStack * aErrorStack )
{
    SQLRETURN   sReturn;
    
    sReturn = ZTQ_SQLDISCONNECT( aConnectionHandle );

    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_DISCONNECT );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_DISCONNECT )
    {
        (void)ztqPrintError( aConnectionHandle,
                             SQL_HANDLE_DBC,
                             aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqSQLCloseCursor( SQLHSTMT        aStmtHandle,
                             stlErrorStack * aErrorStack )
{
    SQLRETURN   sReturn;

    if( ZTQ_SQLCLOSECURSOR != NULL )
    {
        sReturn = ZTQ_SQLCLOSECURSOR( aStmtHandle );
    }
    else
    {
        sReturn = ZTQ_SQLFREESTMT( aStmtHandle, SQL_CLOSE );
    }

    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_CLOSECURSOR );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CLOSECURSOR )
    {
        (void)ztqPrintError( aStmtHandle,
                             SQL_HANDLE_STMT,
                             aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqSQLGetDiagRec( SQLSMALLINT     aHandleType,
                            SQLHANDLE       aHandle,
                            SQLSMALLINT     aRecNumber,
                            SQLCHAR       * aSqlState,
                            SQLINTEGER    * aNativeErrorPtr,
                            SQLCHAR       * aMessageText,
                            SQLSMALLINT     aBufferLength,
                            SQLSMALLINT   * aTextLengthPtr,
                            SQLRETURN     * aReturn,
                            stlErrorStack * aErrorStack )
{
    SQLWCHAR  sSqlStateW[32];
    SQLWCHAR  sMessageTextW[1024];
    SQLHANDLE sEnvHandle = NULL;
    SQLHANDLE sDbcHandle = NULL;
    SQLHANDLE sStmtHandle = NULL;
    SQLRETURN sReturn;

    if( ZTQ_SQLGETDIAGREC == NULL )
    {
        switch( aHandleType )
        {
            case SQL_HANDLE_ENV:
                sEnvHandle = gZtqEnvHandle;
                break;
            case SQL_HANDLE_DBC:
                sEnvHandle = gZtqEnvHandle;
                sDbcHandle = gZtqDbcHandle;
                break;
            case SQL_HANDLE_STMT:
                sEnvHandle = gZtqEnvHandle;
                sDbcHandle = gZtqDbcHandle;
                sStmtHandle = aHandle;
                break;
            default:
                STL_ASSERT( 0 );
                break;
        }
        
        sReturn = ZTQ_SQLERROR( sEnvHandle,
                                sDbcHandle,
                                sStmtHandle,
                                aSqlState,
                                aNativeErrorPtr,
                                aMessageText,
                                aBufferLength,
                                aTextLengthPtr );
    }
    else
    {
        if( gZtqUnicodeDriver == STL_TRUE )
        {
            sReturn = ZTQ_SQLGETDIAGREC( aHandleType,
                                         aHandle,
                                         aRecNumber,
                                         (SQLCHAR*)sSqlStateW,
                                         aNativeErrorPtr,
                                         (SQLCHAR*)sMessageTextW,
                                         1024,
                                         aTextLengthPtr );
        
            STL_TRY( ztqWcharToChar( sSqlStateW,
                                     aSqlState,
                                     SQL_NTS )
                     == STL_SUCCESS );
            
            STL_TRY( ztqWcharToChar( sMessageTextW,
                                     aMessageText,
                                     SQL_NTS ) == STL_SUCCESS );

            *aTextLengthPtr = stlStrlen( (stlChar*)aMessageText );
        }
        else
        {
            sReturn = ZTQ_SQLGETDIAGREC( aHandleType,
                                         aHandle,
                                         aRecNumber,
                                         aSqlState,
                                         aNativeErrorPtr,
                                         aMessageText,
                                         aBufferLength,
                                         aTextLengthPtr );
        }
    }

    *aReturn = sReturn;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqSQLSetStmtAttr( SQLHSTMT        aStmtHandle,
                             SQLINTEGER      aAttribute,
                             SQLPOINTER      aValuePtr,
                             SQLINTEGER      aStringLength,
                             stlErrorStack * aErrorStack )
{
    SQLRETURN   sReturn;
    
    sReturn = ZTQ_SQLSETSTMTATTR( aStmtHandle,
                                  aAttribute,
                                  aValuePtr,
                                  STL_SIZEOF( stlUInt16 ) );

    if( aAttribute < SQL_DRIVER_STMT_ATTR_BASE )
    {
        STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_SETSTMTATTR );
    }
    else
    {
        /*
         * Vendor specific attribute 라면 에러를 무시한다.
         */
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SETSTMTATTR )
    {
        (void)ztqPrintError( aStmtHandle,
                             SQL_HANDLE_STMT,
                             aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztqSQLGetStmtAttr( SQLHSTMT        aStmtHandle,
                             SQLINTEGER      aAttribute,
                             SQLPOINTER      aValuePtr,
                             SQLINTEGER      aBufferLength, 
                             SQLINTEGER    * aStringLength,
                             stlErrorStack * aErrorStack )
{
    SQLRETURN   sReturn;

    sReturn = ZTQ_SQLGETSTMTATTR( aStmtHandle,
                                  aAttribute,
                                  aValuePtr,
                                  aBufferLength,
                                  aStringLength );

    if( aAttribute < SQL_DRIVER_STMT_ATTR_BASE )
    {
        STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_GETSTMTATTR );
    }
    else
    {
        /*
         * Vendor specific attribute 라면 에러를 무시한다.
         */
        if( !SQL_SUCCEEDED( sReturn ) )
        {
            *aStringLength = 0;
        }
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_GETSTMTATTR )
    {
        (void)ztqPrintError( aStmtHandle,
                             SQL_HANDLE_STMT,
                             aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqSQLGetDescField( SQLHDESC        aDescriptorHandle,
                              SQLSMALLINT     aRecNumber,
                              SQLSMALLINT     aFieldIdentifier,
                              SQLPOINTER      aValuePtr,
                              SQLINTEGER      aBufferLength,
                              SQLINTEGER    * aStringLengthPtr,
                              stlErrorStack * aErrorStack )
{
    SQLRETURN   sReturn;
    SQLINTEGER  sStringLength = 0;
    
    sReturn = ZTQ_SQLGETDESCFIELD( aDescriptorHandle,
                                   aRecNumber,
                                   aFieldIdentifier,
                                   aValuePtr,
                                   aBufferLength,
                                   &sStringLength );

    if( aFieldIdentifier < SQL_DRIVER_DESC_FIELD_BASE )
    {
        STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_GETDESCFIELD );

        if( aStringLengthPtr != NULL )
        {
            *aStringLengthPtr = sStringLength;
        }
    }
    else
    {
        /*
         * Vendor specific attribute 라면 에러를 무시한다.
         */
        if( !SQL_SUCCEEDED( sReturn ) )
        {
            *aStringLengthPtr = 0;
        }
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_GETDESCFIELD )
    {
        (void)ztqPrintError( aDescriptorHandle,
                             SQL_HANDLE_DESC,
                             aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqSQLSetDescField( SQLHDESC        aDescriptorHandle,
                              SQLSMALLINT     aRecNumber,
                              SQLSMALLINT     aFieldIdentifier,
                              SQLPOINTER      aValuePtr,
                              SQLINTEGER      aBufferLength,
                              stlErrorStack * aErrorStack )
{
    SQLRETURN   sReturn;
    
    sReturn = ZTQ_SQLSETDESCFIELD( aDescriptorHandle,
                                   aRecNumber,
                                   aFieldIdentifier,
                                   aValuePtr,
                                   aBufferLength );

    if( aFieldIdentifier < SQL_DRIVER_DESC_FIELD_BASE )
    {
        STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_SETDESCFIELD );
    }
    else
    {
        /*
         * Vendor specific attribute 라면 에러를 무시한다.
         */
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SETDESCFIELD )
    {
        (void)ztqPrintError( aDescriptorHandle,
                             SQL_HANDLE_DESC,
                             aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

void ztqPushXaError( stlInt32         aXaErrorCode,
                     ztqCommandType   aCommandType,
                     stlErrorStack  * aErrorStack )
{
    stlInt32 sErrorCode = ZTQ_ERRCODE_XA_UNKNOWN;
    
    switch( aXaErrorCode )
    {
        case XA_RBROLLBACK :
            if( (aCommandType == ZTQ_COMMAND_TYPE_CMD_XA_START) ||
                (aCommandType == ZTQ_COMMAND_TYPE_CMD_XA_END) )
            {
                sErrorCode = ZTQ_ERRCODE_XA_RBROLLBACK_ROLLBACKONLY;
            }
            else
            {
                sErrorCode = ZTQ_ERRCODE_XA_RBROLLBACK;
            }
            break;
        case XA_RBCOMMFAIL :
            sErrorCode = ZTQ_ERRCODE_XA_COMMFAIL;
            break;
        case XA_RBDEADLOCK :
            sErrorCode = ZTQ_ERRCODE_XA_RBDEADLOCK;
            break;
        case XA_RBINTEGRITY :
            sErrorCode = ZTQ_ERRCODE_XA_RBINTEGRITY;
            break;
        case XA_RBOTHER :
            sErrorCode = ZTQ_ERRCODE_XA_RBOTHER;
            break;
        case XA_RBPROTO :
            sErrorCode = ZTQ_ERRCODE_XA_RBPROTO;
            break;
        case XA_RBTIMEOUT :
            sErrorCode = ZTQ_ERRCODE_XA_RBTIMEOUT;
            break;
        case XA_RBTRANSIENT :
            sErrorCode = ZTQ_ERRCODE_XA_RBTRANSIENT;
            break;
        case XA_NOMIGRATE :
            sErrorCode = ZTQ_ERRCODE_XA_NOMIGRATE;
            break;
        case XA_HEURHAZ :
            sErrorCode = ZTQ_ERRCODE_XA_HEURHAZ;
            break;
        case XA_HEURCOM :
            sErrorCode = ZTQ_ERRCODE_XA_HEURCOM;
            break;
        case XA_HEURRB :
            sErrorCode = ZTQ_ERRCODE_XA_HEURRB;
            break;
        case XA_HEURMIX :
            sErrorCode = ZTQ_ERRCODE_XA_HEURMIX;
            break;
        case XA_RETRY :
            sErrorCode = ZTQ_ERRCODE_XA_RETRY;
            break;
        case XA_RDONLY :
            sErrorCode = ZTQ_ERRCODE_XA_RDONLY;
            break;
        case XAER_ASYNC :
            sErrorCode = ZTQ_ERRCODE_XA_ASYNC;
            break;
        case XAER_RMERR :
            switch( aCommandType )
            {
                case ZTQ_COMMAND_TYPE_CMD_XA_OPEN :
                    sErrorCode = ZTQ_ERRCODE_XA_RMERR_OPEN;
                    break;
                case ZTQ_COMMAND_TYPE_CMD_XA_CLOSE :
                    sErrorCode = ZTQ_ERRCODE_XA_RMERR_CLOSE;
                    break;
                default:
                    sErrorCode = ZTQ_ERRCODE_XA_RMERR;
                    break;
            }
            break;
        case XAER_NOTA :
            sErrorCode = ZTQ_ERRCODE_XA_NOTA;
            break;
        case XAER_INVAL :
            sErrorCode = ZTQ_ERRCODE_XA_INVAL;
            break;
        case XAER_PROTO :
            sErrorCode = ZTQ_ERRCODE_XA_PROTO;
            break;
        case XAER_RMFAIL :
            sErrorCode = ZTQ_ERRCODE_XA_RMFAIL;
            break;
        case XAER_DUPID :
            sErrorCode = ZTQ_ERRCODE_XA_DUPID;
            break;
        case XAER_OUTSIDE :
            sErrorCode = ZTQ_ERRCODE_XA_OUTSIDE;
            break;
        default :
            STL_DASSERT( STL_FALSE );
            break;
    }
    
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  sErrorCode,
                  NULL,
                  aErrorStack );
}

stlStatus ztqGetXaConnectionHandle( SQLHANDLE     * aDbcHandle,
                                    stlErrorStack * aErrorStack )
{
    STL_TRY_THROW( gZtqGetXaConnectionHandleFunc != NULL, RAMP_ERR_NOT_SUPPORT );

    *aDbcHandle = gZtqGetXaConnectionHandleFunc();
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_SUPPORT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_NOT_SUPPORT_FUNCTION,
                      NULL,
                      aErrorStack,
                      "SQLGetXaConnectionHandle" );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqOpenXa( stlChar       * aXaInfo,
                     stlInt64        aFlags,
                     stlErrorStack * aErrorStack )
{
    stlInt32       sReturn;
    xa_switch_t  * sXaSwitch;

    STL_TRY_THROW( ZTQ_SQLGETXASWITCH != NULL, RAMP_ERR_NOT_SUPPORT );

    sXaSwitch = ZTQ_SQLGETXASWITCH();
    sReturn = sXaSwitch->xa_open_entry( aXaInfo, 0, aFlags );

    if( sReturn != XA_OK )
    {
        (void) ztqPushXaError( sReturn,
                               ZTQ_COMMAND_TYPE_CMD_XA_OPEN,
                               aErrorStack );
        STL_TRY( STL_FALSE );
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_SUPPORT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_NOT_SUPPORT_FUNCTION,
                      NULL,
                      aErrorStack,
                      "SQLGetXaSwitch" );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqCloseXa( stlChar       * aXaInfo,
                      stlInt64        aFlags,
                      stlErrorStack * aErrorStack )
{
    stlInt32       sReturn;
    xa_switch_t  * sXaSwitch;

    STL_TRY_THROW( ZTQ_SQLGETXASWITCH != NULL, RAMP_ERR_NOT_SUPPORT );

    sXaSwitch = ZTQ_SQLGETXASWITCH();
    sReturn = sXaSwitch->xa_close_entry( aXaInfo, 0, aFlags );

    if( sReturn != XA_OK )
    {
        (void) ztqPushXaError( sReturn,
                               ZTQ_COMMAND_TYPE_CMD_XA_OPEN,
                               aErrorStack );
        STL_TRY( STL_FALSE );
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_SUPPORT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_NOT_SUPPORT_FUNCTION,
                      NULL,
                      aErrorStack,
                      "SQLGetXaSwitch" );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqStartXa( XID           * aXid,
                      stlInt64        aFlags,
                      stlErrorStack * aErrorStack )
{
    stlInt32       sReturn;
    xa_switch_t  * sXaSwitch;

    STL_TRY_THROW( ZTQ_SQLGETXASWITCH != NULL, RAMP_ERR_NOT_SUPPORT );

    sXaSwitch = ZTQ_SQLGETXASWITCH();
    sReturn = sXaSwitch->xa_start_entry( aXid, 0, aFlags );

    if( sReturn != XA_OK )
    {
        (void) ztqPushXaError( sReturn,
                               ZTQ_COMMAND_TYPE_CMD_XA_START,
                               aErrorStack );
        STL_TRY( STL_FALSE );
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_SUPPORT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_NOT_SUPPORT_FUNCTION,
                      NULL,
                      aErrorStack,
                      "SQLGetXaSwitch" );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqEndXa( XID           * aXid,
                    stlInt64        aFlags,
                    stlErrorStack * aErrorStack )
{
    stlInt32       sReturn;
    xa_switch_t  * sXaSwitch;

    STL_TRY_THROW( ZTQ_SQLGETXASWITCH != NULL, RAMP_ERR_NOT_SUPPORT );

    sXaSwitch = ZTQ_SQLGETXASWITCH();
    sReturn = sXaSwitch->xa_end_entry( aXid, 0, aFlags );

    if( sReturn != XA_OK )
    {
        (void) ztqPushXaError( sReturn,
                               ZTQ_COMMAND_TYPE_CMD_XA_END,
                               aErrorStack );
        STL_TRY( STL_FALSE );
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_SUPPORT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_NOT_SUPPORT_FUNCTION,
                      NULL,
                      aErrorStack,
                      "SQLGetXaSwitch" );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqCommitXa( XID           * aXid,
                       stlInt64        aFlags,
                       stlErrorStack * aErrorStack )
{
    stlInt32       sReturn;
    xa_switch_t  * sXaSwitch;

    STL_TRY_THROW( ZTQ_SQLGETXASWITCH != NULL, RAMP_ERR_NOT_SUPPORT );

    sXaSwitch = ZTQ_SQLGETXASWITCH();
    sReturn = sXaSwitch->xa_commit_entry( aXid, 0, aFlags );

    if( sReturn != XA_OK )
    {
        (void) ztqPushXaError( sReturn,
                               ZTQ_COMMAND_TYPE_CMD_XA_COMMIT,
                               aErrorStack );
        STL_TRY( STL_FALSE );
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_SUPPORT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_NOT_SUPPORT_FUNCTION,
                      NULL,
                      aErrorStack,
                      "SQLGetXaSwitch" );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqRollbackXa( XID           * aXid,
                         stlInt64        aFlags,
                         stlErrorStack * aErrorStack )
{
    stlInt32       sReturn;
    xa_switch_t  * sXaSwitch;

    STL_TRY_THROW( ZTQ_SQLGETXASWITCH != NULL, RAMP_ERR_NOT_SUPPORT );

    sXaSwitch = ZTQ_SQLGETXASWITCH();
    sReturn = sXaSwitch->xa_rollback_entry( aXid, 0, aFlags );

    if( sReturn != XA_OK )
    {
        (void) ztqPushXaError( sReturn,
                               ZTQ_COMMAND_TYPE_CMD_XA_ROLLBACK,
                               aErrorStack );
        STL_TRY( STL_FALSE );
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_SUPPORT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_NOT_SUPPORT_FUNCTION,
                      NULL,
                      aErrorStack,
                      "SQLGetXaSwitch" );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqPrepareXa( XID           * aXid,
                        stlInt64        aFlags,
                        stlErrorStack * aErrorStack )
{
    stlInt32       sReturn;
    xa_switch_t  * sXaSwitch;

    STL_TRY_THROW( ZTQ_SQLGETXASWITCH != NULL, RAMP_ERR_NOT_SUPPORT );

    sXaSwitch = ZTQ_SQLGETXASWITCH();
    sReturn = sXaSwitch->xa_prepare_entry( aXid, 0, aFlags );

    if( sReturn != XA_OK )
    {
        (void) ztqPushXaError( sReturn,
                               ZTQ_COMMAND_TYPE_CMD_XA_PREPARE,
                               aErrorStack );
        STL_TRY( STL_FALSE );
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_SUPPORT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_NOT_SUPPORT_FUNCTION,
                      NULL,
                      aErrorStack,
                      "SQLGetXaSwitch" );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqForgetXa( XID           * aXid,
                       stlInt64        aFlags,
                       stlErrorStack * aErrorStack )
{
    stlInt32       sReturn;
    xa_switch_t  * sXaSwitch;

    STL_TRY_THROW( ZTQ_SQLGETXASWITCH != NULL, RAMP_ERR_NOT_SUPPORT );

    sXaSwitch = ZTQ_SQLGETXASWITCH();
    sReturn = sXaSwitch->xa_forget_entry( aXid, 0, aFlags );

    if( sReturn != XA_OK )
    {
        (void) ztqPushXaError( sReturn,
                               ZTQ_COMMAND_TYPE_CMD_XA_FORGET,
                               aErrorStack );
        STL_TRY( STL_FALSE );
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_SUPPORT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_NOT_SUPPORT_FUNCTION,
                      NULL,
                      aErrorStack,
                      "SQLGetXaSwitch" );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqRecoverXa( XID           * aXids,
                        stlInt64        aCount,
                        stlInt64        aFlags,
                        stlInt64      * aFetchCount,
                        stlErrorStack * aErrorStack )
{
    stlInt32       sReturn;
    xa_switch_t  * sXaSwitch;

    *aFetchCount = 0;
    
    STL_TRY_THROW( ZTQ_SQLGETXASWITCH != NULL, RAMP_ERR_NOT_SUPPORT );

    sXaSwitch = ZTQ_SQLGETXASWITCH();
    sReturn = sXaSwitch->xa_recover_entry( aXids, aCount, 0, aFlags );

    if( sReturn < 0 )
    {
        (void) ztqPushXaError( sReturn,
                               ZTQ_COMMAND_TYPE_CMD_XA_RECOVER,
                               aErrorStack );
        STL_TRY( STL_FALSE );
    }

    *aFetchCount = sReturn;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_SUPPORT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_NOT_SUPPORT_FUNCTION,
                      NULL,
                      aErrorStack,
                      "SQLGetXaSwitch" );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
