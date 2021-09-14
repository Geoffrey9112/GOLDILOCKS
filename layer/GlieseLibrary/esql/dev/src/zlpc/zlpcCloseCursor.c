/*******************************************************************************
 * zlpcCloseCursor.c
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
#include <zlpcCloseCursor.h>
#include <zlpyCursor.h>

/**
 * @file zlpcCloseCursor.c
 * @brief Gliese Embedded SQL in C precompiler SQL cursor library functions
 */

/**
 * @defgroup zlpcCloseCursor   CLOSE cursor
 * @addtogroup zlplCursor
 * @{
 */

/**
 * @brief CLOSE cursor 구문을 수행한다.
 * @param[in]  aSqlContext   SQL Context
 * @param[in]  aSqlArgs      SQL Arguments
 * @remarks
 */
stlStatus zlpcCloseCursor( zlplSqlContext * aSqlContext,
                           zlplSqlArgs    * aSqlArgs )
{
    SQLRETURN           sReturn;
    zlplCursorSymbol  * sCursorSymbol = NULL;

    ZLPL_INIT_SQLCA(aSqlContext, aSqlArgs->sql_ca);
    STL_TRY( zlpeIsInit() == STL_SUCCESS );

    STL_TRY( zlpyLookupCursorSymbol( aSqlContext,
                                     aSqlArgs->sqlcursor->sqlcname,
                                     & sCursorSymbol )
             == STL_SUCCESS );
    STL_TRY_THROW( sCursorSymbol != NULL, RAMP_ERR_LOOKUP_SYMBOL );

    sReturn = SQLCloseCursor( sCursorSymbol->mStmtHandle );

    STL_TRY(zlpeCheckError(sReturn,
                           SQL_HANDLE_STMT,
                           sCursorSymbol->mStmtHandle,
                           (zlplSqlca *)aSqlArgs->sql_ca,
                           (stlChar *)aSqlArgs->sql_state)
            == STL_SUCCESS);

    sCursorSymbol->mIsSetCursorName = STL_FALSE;

    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_LOOKUP_SYMBOL)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_CURSOR_IS_NOT_OPEN,
                      NULL,
                      ZLPL_CTXT_ERROR_STACK(aSqlContext),
                      aSqlArgs->sqlcursor->sqlcname );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/** @} zlpcCloseCursor */
