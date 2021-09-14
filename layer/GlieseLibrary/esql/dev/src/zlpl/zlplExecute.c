/*******************************************************************************
 * zlplExecute.c
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
#include <zlpnConnStr.h>
#include <zlpuMisc.h>
#include <zlpxExecute.h>

/**
 * @file zlplExecute.c
 * @brief Gliese Embedded SQL in C precompiler SQL execution library functions
 */

/**
 * @addtogroup zlplExecute
 * @{
 */

/**
 * @brief External
 */

int  GOLDILOCKSESQL_EndTran( void       * aSqlContext,
                        sqlargs_t  * aSqlArgs,
                        int          aIsCommit,
                        int          aIsRelease )
{
    stlBool           sIsCommit = STL_FALSE;
    stlBool           sIsRelease = STL_FALSE;
    zlplSqlContext   *sSqlContext = (zlplSqlContext *)aSqlContext;

    STL_TRY( zlpnGetRealContext( sSqlContext,
                                 (zlplSqlArgs *)aSqlArgs,
                                 &sSqlContext )
             == STL_SUCCESS );

    STL_TRY_THROW( sSqlContext != NULL,
                   ERR_CONNECTION_NOT_EXIST );

    if(aIsCommit == 1)
    {
        sIsCommit = STL_TRUE;
    }
    if(aIsRelease == 1)
    {
        sIsRelease = STL_TRUE;
    }

    STL_TRY( zlpxEndTranContext( sSqlContext,
                                 (zlplSqlArgs *)aSqlArgs,
                                 sIsCommit,
                                 sIsRelease )
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

int  GOLDILOCKSESQL_Execute( void       * aSqlContext,
                        sqlargs_t  * aSqlArgs )
{
    zlplSqlContext  *sSqlContext = (zlplSqlContext *)aSqlContext;

    STL_TRY( zlpnGetRealContext( sSqlContext,
                                 (zlplSqlArgs *)aSqlArgs,
                                 &sSqlContext )
             == STL_SUCCESS );

    STL_TRY_THROW( sSqlContext != NULL,
                   ERR_CONNECTION_NOT_EXIST );

    STL_TRY( zlpxExecuteContext(sSqlContext,
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

int  GOLDILOCKSESQL_SetAutoCommit( void       * aSqlContext,
                              sqlargs_t  * aSqlArgs,
                              int          aIsOn )
{
    stlBool           sIsOn = STL_FALSE;
    zlplSqlContext   *sSqlContext = (zlplSqlContext *)aSqlContext;

    STL_TRY( zlpnGetRealContext( sSqlContext,
                                 (zlplSqlArgs *)aSqlArgs,
                                 &sSqlContext )
             == STL_SUCCESS );

    STL_TRY_THROW( sSqlContext != NULL,
                   ERR_CONNECTION_NOT_EXIST );

    if(aIsOn == 1)
    {
        sIsOn = STL_TRUE;
    }

    STL_TRY( zlpxSetAutocommitContext( sSqlContext,
                                       (zlplSqlArgs *)aSqlArgs,
                                       sIsOn )
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

/** @} */
