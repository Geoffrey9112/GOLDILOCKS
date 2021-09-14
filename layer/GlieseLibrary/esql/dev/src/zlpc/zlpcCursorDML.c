/*******************************************************************************
 * zlpcCursorDML.c
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
#include <zlpcCursorDML.h>
#include <zlpxExecute.h>
#include <zlpyCursor.h>
#include <zlpbBind.h>

/**
 * @file zlpcCursorDML.c
 * @brief Gliese Embedded SQL in C precompiler SQL cursor library functions
 */

/**
 * @defgroup zlpcCursorDML   Positioned Cursor DML
 * @addtogroup zlplCursor
 * @{
 */


/**
 * @brief UPDATE/DELETE .. WHERE CURRENT OF cursor 구문을 수행한다.
 * @param[in]  aSqlContext   SQL Context
 * @param[in]  aSqlArgs      SQL Arguments
 * @remarks
 */
stlStatus zlpcPositionedCursorDML( zlplSqlContext * aSqlContext,
                                   zlplSqlArgs    * aSqlArgs )
{
    SQLRETURN           sReturn;
    zlplCursorSymbol  * sCursorSymbol = NULL;

    /* SQLULEN   sRowPos = 0; */
    SQLSETPOSIROW  sRowPos = 0;

    /**
     * 초기화 수행
     */

    ZLPL_INIT_SQLCA(aSqlContext, aSqlArgs->sql_ca);
    STL_TRY( zlpeIsInit() == STL_SUCCESS );

    STL_TRY( zlpbAdjustDataType( aSqlContext,
                                 aSqlArgs )
             == STL_SUCCESS );

    /**
     * Cursor Symbol 검색
     */

    STL_TRY( zlpyLookupCursorSymbol( aSqlContext,
                                     aSqlArgs->sqlcursor->sqlcname,
                                     & sCursorSymbol )
             == STL_SUCCESS );
    STL_TRY_THROW( sCursorSymbol != NULL, RAMP_ERR_LOOKUP_SYMBOL );

    /**
     * Updatable Cursor 인지 검사
     */

    STL_TRY_THROW( sCursorSymbol->mForUpdate == STL_TRUE, RAMP_ERR_NOT_UPDATABLE_CURSOR );
    /**
     * Cursor Position 설정
     */

    /*
    sReturn = SQLGetStmtAttr( sCursorSymbol->mStmtHandle,
                              SQL_ATTR_ROW_NUMBER,
                              & sRowPos,
                              STL_SIZEOF( SQLULEN ),
                              NULL );

    STL_TRY( zlpeCheckError( sReturn,
                             SQL_HANDLE_STMT,
                             sCursorSymbol->mStmtHandle,
                             (zlplSqlca *)aSqlArgs->sql_ca,
                             (stlChar *)aSqlArgs->sql_state )
            == STL_SUCCESS);
    */

    /**
     * @todo ARRAY 일 경우 고려 필요
     */
    sRowPos = 1;

    sReturn = SQLSetPos( sCursorSymbol->mStmtHandle,
                         sRowPos,
                         SQL_POSITION,
                         SQL_LOCK_NO_CHANGE );

    STL_TRY(zlpeCheckError( sReturn,
                            SQL_HANDLE_STMT,
                            sCursorSymbol->mStmtHandle,
                            (zlplSqlca *)aSqlArgs->sql_ca,
                            (stlChar *)aSqlArgs->sql_state )
            == STL_SUCCESS);

    /**
     * 이후 작업은 다른 DML 구문과 동일하게 처리함
     */

    STL_TRY( zlpxExecuteContext( aSqlContext, aSqlArgs ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_LOOKUP_SYMBOL)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_CURSOR_IS_NOT_OPEN,
                      NULL,
                      ZLPL_CTXT_ERROR_STACK(aSqlContext),
                      aSqlArgs->sqlcursor->sqlcname );
    }

    STL_CATCH(RAMP_ERR_NOT_UPDATABLE_CURSOR)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_CURSOR_IS_NOT_UPDATABLE,
                      NULL,
                      ZLPL_CTXT_ERROR_STACK(aSqlContext),
                      aSqlArgs->sqlcursor->sqlcname );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/** @} zlpcCursorDML */
