/*******************************************************************************
 * zlpnConnect.c
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
#include <goldilocks.h>
#include <zlpeSqlca.h>
#include <zlpnConnect.h>
#include <zlpnConnStr.h>
#include <zlpuMisc.h>
#include <zlpySymTab.h>
#include <zllDbcNameHash.h>

/**
 * @file zlpnConnect.c
 * @brief Gliese Embedded SQL in C precompiler connect library functions
 */

/**
 * @addtogroup zlpnConnect
 * @{
 */

/* Global variables : DEFAULT connection 에서만 사용 */
SQLHENV         gEnv    = SQL_NULL_HANDLE;
zlplSqlContext *gDefaultContext;

stlThreadOnce   gThreadOnce = {STL_THREAD_ONCE_INIT};

stlStatus zlpnConnectContext(zlplSqlContext  **aSqlContext,
                             zlplSqlArgs      *aSqlArgs,
                             stlChar          *aConnString,
                             stlChar          *aUserName,
                             stlChar          *aPassword)
{
    stlInt32      sState = 0;
    zlplConnInfo *sConnInfo = NULL;
    SQLRETURN     sReturn;
    SQLSMALLINT   sConnStrLen;
    SQLSMALLINT   sStringLength2;
    stlChar       sFullConnString[ZLPL_MAX_CONN_STRING_LENGTH];

    zlplSqlContext     *sSqlContext = NULL;
    zlplSqlContext     *sDupSqlContext = NULL;
    stlBool             sAllocContext = STL_FALSE;
    stlBool             sIsDuplicated  = STL_FALSE;
    stlErrorStack      *sErrorStack;

    if( *aSqlContext == NULL )
    {
        /*
         * 반드시 Connection Name이 존재해야만 한다.
         * Named Context가 없고(ESQL에서 이전에 Connection한적이 없음),
         * Named DBC도 없다.(ODBC에서도 해당 이름으로 Connection 한적이 없음)
         */
        STL_DASSERT( aSqlArgs->conn != NULL );

        STL_TRY( zlpuAllocContextMem( &sSqlContext )
                 == STL_SUCCESS );
        sAllocContext = STL_TRUE;

        *aSqlContext = sSqlContext;
    }
    else
    {
        sSqlContext = *aSqlContext;
    }
    sErrorStack = ZLPL_CTXT_ERROR_STACK(sSqlContext);

    zlpnLockContext( sSqlContext );
    /*
     * Connection 유무 체크
     * : ESQL에서 새로 Connect를 하기 위해서는,
     *   반드시 해당 Context가 Connect되지 않았어야 한다.
     */
    ZLPL_INIT_SQLCA(sSqlContext, aSqlArgs->sql_ca);
    if( aSqlArgs->conn == NULL )
    {
        STL_TRY_THROW( sSqlContext->mIsConnected == STL_FALSE,
                       ERR_DUPLICATE_CONTEXT );
    }
    else
    {
        STL_TRY_THROW( sSqlContext->mIsConnected == STL_FALSE,
                       ERR_CONNECTION_NAME_IN_USE );
    }

    sConnInfo = &sSqlContext->mConnInfo;
    STL_TRY( zlpyCreateSymbolTable( sSqlContext )
             == STL_SUCCESS );
    sState = 1;

    /*
     * Connect
     */
    sReturn = SQLAllocHandle( SQL_HANDLE_DBC,
                              gEnv,
                              &sConnInfo->mDbc );
    STL_TRY( zlpeCheckError( sReturn,
                             SQL_HANDLE_ENV,
                             gEnv,
                             (zlplSqlca *)aSqlArgs->sql_ca,
                             (stlChar *)aSqlArgs->sql_state )
             == STL_SUCCESS );
    sState = 2;

    /* SQLConnect establishes connections to a driver and a data source */
    if( aConnString == NULL )
    {
        sConnStrLen = stlSnprintf( sFullConnString,
                                   ZLPL_MAX_CONN_STRING_LENGTH,
                                   "UID=%s;PWD=%s",
                                   aUserName,
                                   aPassword );
    }
    else
    {
        sConnStrLen = stlSnprintf( sFullConnString,
                                   ZLPL_MAX_CONN_STRING_LENGTH,
                                   "%s;UID=%s;PWD=%s",
                                   aConnString,
                                   aUserName,
                                   aPassword );
    }

    sReturn = SQLDriverConnect( sConnInfo->mDbc,
                                NULL,   /* aWindowHandle */
                                (SQLCHAR *)sFullConnString,
                                sConnStrLen,
                                NULL,   /* aOutConnString */
                                0,      /* aOutConnStringLength */
                                &sStringLength2,
                                SQL_DRIVER_NOPROMPT );
    STL_TRY( zlpeCheckError( sReturn,
                             SQL_HANDLE_DBC,
                             sConnInfo->mDbc,
                             (zlplSqlca *)aSqlArgs->sql_ca,
                             (stlChar *)aSqlArgs->sql_state )
             == STL_SUCCESS );
    sState = 3;

    sReturn = SQLSetConnectAttr( sConnInfo->mDbc,
                                 SQL_ATTR_AUTOCOMMIT,
                                 (SQLPOINTER)SQL_AUTOCOMMIT_OFF,
                                 SQL_IS_UINTEGER );
    STL_TRY( zlpeCheckError( sReturn,
                             SQL_HANDLE_DBC,
                             sConnInfo->mDbc,
                             (zlplSqlca *)aSqlArgs->sql_ca,
                             (stlChar *)aSqlArgs->sql_state )
             == STL_SUCCESS );

    if( aSqlArgs->conn != NULL )
    {
        stlStrncpy( sSqlContext->mConnName,
                    aSqlArgs->conn,
                    ZLPL_MAX_CONN_STRING_LENGTH );

        /*
         * Do Loop
         * - 현재 SqlContext를 Hash Table에 삽입(Unique Insert)
         * - 만약, 중복이 되었다면
         *    1) SqlContext가 Connection 되어 있으면, CONNECTION_NAME_IN_USE 에러
         *    2) Connect 되어 있지 않으면, Hash table의 Sql Context를 삭제
         * End Loop
         */
        do
        {
            STL_TRY( zlpnAddUniqueSqlContextToHash( sSqlContext,
                                                    &sIsDuplicated,
                                                    ZLPL_CTXT_ERROR_STACK(sSqlContext) )
                     == STL_SUCCESS );
            if( sIsDuplicated == STL_TRUE )
            {
                STL_TRY( zlpnLookupSqlContext( aSqlArgs->conn,
                                               &sDupSqlContext,
                                               ZLPL_CTXT_ERROR_STACK(sSqlContext) )
                 == STL_SUCCESS );

                if( sDupSqlContext != NULL )
                {
                    zlpnLockContext( sDupSqlContext );
                    if( sDupSqlContext->mIsConnected == STL_TRUE )
                    {
                        zlpnUnlockContext( sDupSqlContext );
                        STL_THROW( ERR_CONNECTION_NAME_IN_USE );
                    }

                    if( zlpnDeleteSqlContextFromHash( sDupSqlContext,
                                                      ZLPL_CTXT_ERROR_STACK(sSqlContext) )
                        != STL_SUCCESS )
                    {
                        zlpnUnlockContext( sDupSqlContext );
                        STL_TRY( STL_FALSE );
                    }

                    zlpnUnlockContext( sDupSqlContext );

                    STL_TRY( zlpuFreeContextMem( &sDupSqlContext ) == STL_SUCCESS );
                }
                else
                {
                    /*
                     * 위 Unique Insert와 Lookup 사이에, 다른 thread의 Disconnect로
                     * 인하여 Hash에 없는 경우가 발생할 수 있으므로, 이 경우는
                     * 다시 Insert를 시도한다.
                     */
                }
            }
        } while( sIsDuplicated == STL_TRUE );

        /*
         * ODBC의 Connection Name 해쉬 테이블에 DBC를 삽입한다.
         */
        STL_TRY( zllSetDbcName( sConnInfo->mDbc,
                                aSqlArgs->conn,
                                ZLPL_CTXT_ERROR_STACK(sSqlContext) )
                 == STL_SUCCESS );

        STL_TRY( zllAddDbcToNameHash( sConnInfo->mDbc,
                                      ZLPL_CTXT_ERROR_STACK(sSqlContext) )
                 == STL_SUCCESS );
    } /* end of if( aSqlArgs->conn != NULL ) */

    STL_TRY( zlpnPutContextList( sSqlContext )
             == STL_SUCCESS );

    sSqlContext->mIsConnected = STL_TRUE;
    zlpnUnlockContext( sSqlContext );

    return STL_SUCCESS;

    STL_CATCH( ERR_CONNECTION_NAME_IN_USE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_CONNECTION_NAME_IN_USE,
                      NULL,
                      sErrorStack,
                      aSqlArgs->conn );
    }
    STL_CATCH( ERR_DUPLICATE_CONTEXT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_DUPLICATE_CONTEXT,
                      NULL,
                      ZLPL_CTXT_ERROR_STACK(sSqlContext) );
    }
    STL_FINISH;

    switch(sState)
    {
        case 3:
            (void)SQLDisconnect(sConnInfo->mDbc);
        case 2:
            (void)SQLFreeHandle(SQL_HANDLE_DBC, sConnInfo->mDbc);
            sConnInfo->mDbc = SQL_NULL_HANDLE;
        case 1:
            (void)zlpyDestroySymbolTable(sSqlContext);
        default:
            break;
    }

    GOLDILOCKSESQL_SQLCA_SET_ERROR( sSqlContext, aSqlArgs );
    zlpnUnlockContext( sSqlContext );

    if( sAllocContext == STL_TRUE )
    {
        zlpuFreeContextMem( &sSqlContext );
        *aSqlContext = NULL;
    }

    return STL_FAILURE;
}

stlStatus zlpnDisconnectContext(zlplSqlContext   *aSqlContext,
                                zlplSqlArgs      *aSqlArgs,
                                stlChar          *aConnObject)
{
    zlplConnInfo *sConnInfo = &aSqlContext->mConnInfo;
    SQLRETURN     sReturn;

    STL_TRY( zlpeIsInit() == STL_SUCCESS );

    zlpnLockContext( aSqlContext );
    ZLPL_INIT_SQLCA(aSqlContext, aSqlArgs->sql_ca);

    if( aSqlContext->mIsConnected == STL_TRUE )
    {
        if( aSqlArgs->conn != NULL )
        {
            STL_TRY( zllRemoveDbcFromNameHash( sConnInfo->mDbc,
                                               ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                     == STL_SUCCESS );

            STL_TRY( zlpnDeleteSqlContextFromHash( aSqlContext,
                                                   ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                     == STL_SUCCESS );
        }

        STL_TRY( zlpyDestroySymbolTable( aSqlContext )
                 == STL_SUCCESS );

        STL_TRY( zlpnUnlinkContextList( aSqlContext )
                 == STL_SUCCESS );

        sReturn = SQLDisconnect(sConnInfo->mDbc);
        STL_TRY(zlpeCheckError(sReturn,
                               SQL_HANDLE_DBC,
                               sConnInfo->mDbc,
                               (zlplSqlca *)aSqlArgs->sql_ca,
                               (stlChar *)aSqlArgs->sql_state)
                == STL_SUCCESS);

        sReturn = SQLFreeHandle(SQL_HANDLE_DBC, sConnInfo->mDbc);
        STL_TRY(zlpeCheckError(sReturn,
                               SQL_HANDLE_DBC,
                               sConnInfo->mDbc,
                               (zlplSqlca *)aSqlArgs->sql_ca,
                               (stlChar *)aSqlArgs->sql_state)
                == STL_SUCCESS);

        aSqlContext->mIsConnected = STL_FALSE;
    }
    else
    {
        /*
         * SqlContext가 connection 상태가 아닐때, 에러를 설정하기 위하여,
         * 실제 Disconnect 연산을 수행한다.
         */
        sReturn = SQLDisconnect(sConnInfo->mDbc);
        STL_TRY(zlpeCheckError(sReturn,
                               SQL_HANDLE_DBC,
                               sConnInfo->mDbc,
                               (zlplSqlca *)aSqlArgs->sql_ca,
                               (stlChar *)aSqlArgs->sql_state)
                == STL_SUCCESS);
    }

    zlpnUnlockContext( aSqlContext );

    (void)zlpuFreeContextMem( &aSqlContext );

    return STL_SUCCESS;

    STL_FINISH;

    aSqlContext->mIsConnected = STL_FALSE;

    zlpnUnlockContext( aSqlContext );
    (void)zlpuFreeContextMem( &aSqlContext );

    return STL_FAILURE;
}

stlStatus zlpnDisconnectAll(zlplSqlArgs      *aSqlArgs)
{
    zlplSqlContext  *sSqlContext;

    STL_TRY( zlpnGetContextList( &sSqlContext )
             == STL_SUCCESS );

    while( sSqlContext != NULL  )
    {
        STL_TRY( zlpnDisconnectContext( sSqlContext,
                                        aSqlArgs,
                                        NULL )
                 == STL_SUCCESS );

        STL_TRY( zlpnGetContextList( &sSqlContext )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlpnAllocContext(zlplSqlContext **aSqlContext,
                           zlplSqlArgs     *aSqlArgs)
{
    zlplSqlContext *sSqlContext = NULL;

    STL_TRY( zlpuAllocContextMem(&sSqlContext)
             == STL_SUCCESS );

    ZLPL_INIT_SQLCA(sSqlContext, aSqlArgs->sql_ca);

    *aSqlContext = sSqlContext;

    return STL_SUCCESS;

    STL_FINISH;

    *aSqlContext = NULL;

    return STL_FAILURE;
}

stlStatus zlpnFreeContext(zlplSqlContext  **aSqlContext,
                          zlplSqlArgs      *aSqlArgs)
{
    STL_TRY( zlpeIsInit() == STL_SUCCESS );

    INIT_SQLCA(aSqlArgs->sql_ca);
    STL_TRY( zlpuFreeContextMem(aSqlContext) == STL_SUCCESS);

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

void zlpnLockContext( zlplSqlContext *aSqlContext )
{
    stlUInt64   sMissCnt;

    stlAcquireSpinLock( &aSqlContext->mCtxtSpinLock, &sMissCnt );
}

void zlpnUnlockContext( zlplSqlContext *aSqlContext )
{
    stlReleaseSpinLock( &aSqlContext->mCtxtSpinLock );
}


/** @} */
