/*******************************************************************************
 * zlpxDynExecuteImmediate.c
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
#include <zlpxDynExecuteImmediate.h>
#include <zlpxExecute.h>
#include <zlpyQueryStmt.h>

/**
 * @file zlpxDynExecuteImmediate.c
 * @brief Gliese Embedded SQL in C precompiler SQL execution library functions
 */

/**
 * @defgroup zlpxDynExecuteImmediate  EXECUTE IMMEDIATE library
 * @ingroup DynamicSQL
 * @{
 */


/**
 * @brief (internal) EXECUTE IMMEDIATE 를 수행함
 * @param[in] aSqlContext    SQL Context
 * @param[in] aSqlArgs       SQL Arguments
 * @remarks
 */
stlStatus zlpxDynExecuteImmediate( zlplSqlContext   * aSqlContext,
                                   zlplSqlArgs      * aSqlArgs )
{
    SQLRETURN           sReturn;

    SQLHANDLE           sStmtHandle = SQL_NULL_HANDLE;
    SQLLEN              sRowCnt = 0;
    SQLSMALLINT         sColCnt = 0;

    zlplSqlStmtKey         sSqlKey;
    zlplSqlStmtSymbol    * sSqlSymbol = NULL;
    stlBool                sIsFound = STL_FALSE;

    stlChar        * sStringSQL = NULL;
    SQLINTEGER       sStringLen = 0;
    zlplSqlHostVar * sHostSQL = NULL;
    zlplVarchar    * sVarcharHost = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSqlArgs != NULL, ZLPL_CTXT_ERROR_STACK(aSqlContext) );
    STL_PARAM_VALIDATE( aSqlArgs->sqltype == ZTPL_STMT_DYNAMIC_SQL_EXECUTE_IMMEDIATE,
                        ZLPL_CTXT_ERROR_STACK(aSqlContext) );

    /**
     * 초기화
     */

    ZLPL_INIT_SQLCA( aSqlContext, aSqlArgs->sql_ca );

    STL_TRY( zlpeIsInit() == STL_SUCCESS );

    /***************************************************************
     * 수행 준비
     ***************************************************************/

    /**
     * Query Statement 를 검색
     */

    sSqlKey.mLine     = aSqlArgs->sqlln;
    sSqlKey.mFileName = aSqlArgs->sqlfn;
    STL_TRY( zlpyLookupSqlStmtSymbol( aSqlContext,
                                      & sSqlKey,
                                      & sSqlSymbol,
                                      & sIsFound )
            == STL_SUCCESS);

    /**
     * ODBC Statemnet Handle 획득
     */

    if( sIsFound == STL_FALSE )
    {
        /**
         * ODBC Statement 할당
         */

        STL_TRY(zlpxCreateSqlSymbol( aSqlContext,
                                     aSqlArgs,
                                     STL_FALSE, /* aDoPrepare */
                                     &sSqlSymbol )
                == STL_SUCCESS);
    }
    else
    {
        /* nothing to do */
    }

    sStmtHandle = sSqlSymbol->mStmtHandle;

    /***************************************************************
     * 구문 수행
     ***************************************************************/

    /**
     * SQL String 정보 획득
     */

    if ( aSqlArgs->sqlstmt != NULL )
    {
        /**
         * EXECUTE IMMEDIATE 'sql string' 인 경우
         */

        STL_DASSERT( aSqlArgs->hvc == 0 );

        sStringSQL = aSqlArgs->sqlstmt;
        sStringLen = SQL_NTS;
    }
    else
    {
        /**
         * EXECUTE IMMEDIATE :sStringVariable 인 경우
         */
        STL_DASSERT( aSqlArgs->hvc == 1 );
        STL_DASSERT( aSqlArgs->sqlhv != NULL );

        /**
         * @todo Array Host Variable 고려해야 함
         */

        /**
         * C type 에 따른 String 정보 획득
         */

        sHostSQL = (zlplSqlHostVar *) aSqlArgs->sqlhv;
        if ( sHostSQL[0].data_type == ZLPL_C_DATATYPE_VARCHAR )
        {
            sVarcharHost = (zlplVarchar *) sHostSQL[0].value;
            sStringSQL = sVarcharHost->mArr;
            sStringLen = sVarcharHost->mLen;
        }
        else
        {
            sStringSQL = (stlChar *) sHostSQL[0].value;
            sStringLen = SQL_NTS;
        }
    }

    /**
     * SQL statement 를 수행
     */

    sReturn = SQLExecDirect( sStmtHandle,
                             (SQLCHAR *) sStringSQL,
                             sStringLen );

    STL_TRY( zlpeCheckError( sReturn,
                             SQL_HANDLE_STMT,
                             sStmtHandle,
                             (zlplSqlca *) aSqlArgs->sql_ca,
                             (stlChar *) aSqlArgs->sql_state )
             == STL_SUCCESS);

    /***************************************************************
     * 마무리 작업
     ***************************************************************/

    /**
     * Affected Row 정보 획득
     */

    sReturn = SQLRowCount( sStmtHandle, & sRowCnt );

    ((zlplSqlca *)(aSqlArgs->sql_ca))->sqlerrd[2] = (int)sRowCnt;

    /**
     * Select 인 경우 Cursor를 닫는다.
     */

    sReturn = SQLNumResultCols( sStmtHandle, & sColCnt );

    if ( sColCnt > 0 )
    {
        sReturn = SQLCloseCursor( sStmtHandle );
    }
    else
    {
        /* nothing to do */
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} zlpxDynExecuteImmediate */
