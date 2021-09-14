/*******************************************************************************
 * zlplDynExecute.c
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
#include <zlpxDynExecute.h>

/**
 * @file zlplDynExecute.c
 * @brief Gliese Embedded SQL in C precompiler SQL execution library functions
 */

/**
 * @defgroup zlplDynExecute  EXECUTE library
 * @ingroup DynamicSQL
 * @{
 */

/**
 * @brief (external) EXECUTE 를 수행함
 * @param[in] aSqlContext    SQL Context
 * @param[in] aSqlArgs       SQL Arguments
 * @param[in] aStmtName      Statement Name
 * @remarks
 */
int GOLDILOCKSESQL_DynamicExecute( void      * aSqlContext,
                              sqlargs_t * aSqlArgs,
                              char      * aStmtName )
{
    zlplSqlContext  *sSqlContext = (zlplSqlContext *)aSqlContext;

    STL_TRY( zlpnGetRealContext( sSqlContext,
                                 (zlplSqlArgs *)aSqlArgs,
                                 &sSqlContext )
             == STL_SUCCESS );

    STL_TRY_THROW( sSqlContext != NULL,
                   ERR_CONNECTION_NOT_EXIST );

    STL_TRY( zlpxDynExecute( sSqlContext,
                             (zlplSqlArgs *)aSqlArgs,
                             aStmtName )
             == STL_SUCCESS );

    return ZLPL_SUCCESS;

    STL_CATCH( ERR_CONNECTION_NOT_EXIST )
    {
        zlpeSetErrorConnectionDoesNotExist( aSqlArgs->sql_ca );
    }
    STL_FINISH;

    GOLDILOCKSESQL_SQLCA_SET_ERROR( sSqlContext, aSqlArgs );

    return ZLPL_FAILURE;
}

/** @} zlplDynExecute */
