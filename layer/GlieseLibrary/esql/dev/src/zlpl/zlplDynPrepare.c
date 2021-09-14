/*******************************************************************************
 * zlplDynPrepare.c
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
#include <zllExtendedDescribeParam.h>
#include <zlpeSqlca.h>
#include <zlpnConnStr.h>
#include <zlpuMisc.h>
#include <zlpxDynPrepare.h>

/**
 * @file zlplDynPrepare.c
 * @brief Gliese Embedded SQL in C precompiler SQL execution library functions
 */

/**
 * @defgroup zlplDynPrepare  PREPARE library
 * @ingroup DynamicSQL
 * @{
 */

/**
 * @brief (external) PREPARE 를 수행함
 * @param[in] aSqlContext    SQL Context
 * @param[in] aSqlArgs       SQL Arguments
 * @param[in] aStmtName      Statement Name
 * @remarks
 */
int GOLDILOCKSESQL_DynamicPrepare( void      * aSqlContext,
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

    STL_TRY( zlpxDynPrepare( sSqlContext,
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

/** @} zlplDynPrepare */
