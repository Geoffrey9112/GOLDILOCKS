/*******************************************************************************
 * zlpcDeclareCursor.c
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
#include <zlpcDeclareCursor.h>

/**
 * @file zlpcDeclareCursor.c
 * @brief Gliese Embedded SQL in C precompiler SQL cursor library functions
 */

/**
 * @defgroup zlpcDeclareCursor   DECLARE cursor
 * @addtogroup zlplCursor
 * @{
 */

/**
 * @brief DECLARE cursor 구문을 수행한다.
 * @param[in]  aSqlContext   SQL Context
 * @param[in]  aSqlArgs      SQL Arguments
 * @remarks
 * 모든 작업은 OPEN cursor 에서 이루어진다.
 * DECLARE cursor 구문은 에러 상태 정보만을 초기화한다.
 */
stlStatus zlpcDeclareCursor( zlplSqlContext * aSqlContext,
                             zlplSqlArgs    * aSqlArgs )
{
    ZLPL_INIT_SQLCA(aSqlContext, aSqlArgs->sql_ca);
    STL_TRY( zlpeIsInit() == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} zlpcDeclareCursor */
