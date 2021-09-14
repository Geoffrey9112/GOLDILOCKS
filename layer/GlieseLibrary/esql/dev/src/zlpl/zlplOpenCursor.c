/*******************************************************************************
 * zlplOpenCursor.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id:$
 *
 * NOTES
 *
 *
 ******************************************************************************/

#include <stl.h>
#include <goldilocks.h>
#include <zlpeSqlca.h>
#include <zlpnConnStr.h>
#include <zlpuMisc.h>
#include <zlpcOpenCursor.h>

/**
 * @file zlplOpenCursor.c
 * @brief Gliese Embedded SQL in C precompiler SQL cursor library functions
 */

/**
 * @defgroup zlplOpenCursor   OPEN cursor
 * @addtogroup zlplCursor
 * @{
 */


int  GOLDILOCKSESQL_OpenCursor( void       * aSqlContext,
                     sqlargs_t  * aSqlArgs )
{
    zlplSqlContext  *sSqlContext = (zlplSqlContext *)aSqlContext;

    STL_TRY( zlpnGetRealContext( sSqlContext,
                                 (zlplSqlArgs *)aSqlArgs,
                                 &sSqlContext )
             == STL_SUCCESS );

    STL_TRY_THROW( sSqlContext != NULL,
                   ERR_CONNECTION_NOT_EXIST );

    STL_TRY( zlpcOpenCursor( sSqlContext,
                             (zlplSqlArgs *)aSqlArgs )
            == STL_SUCCESS);

    return ZLPL_SUCCESS;

    STL_CATCH( ERR_CONNECTION_NOT_EXIST )
    {
        zlpeSetErrorConnectionDoesNotExist( aSqlArgs->sql_ca );
    }
    STL_FINISH;

    GOLDILOCKSESQL_SQLCA_SET_ERROR( sSqlContext, aSqlArgs );

    return ZLPL_FAILURE;
}



/** @} zlplOpenCursor */
