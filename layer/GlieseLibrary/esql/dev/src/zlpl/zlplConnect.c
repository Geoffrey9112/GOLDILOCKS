/*******************************************************************************
 * zlplConnect.c
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

/**
 * @file zlplConnect.c
 * @brief Gliese Embedded SQL in C precompiler connect library functions
 */

/**
 * @addtogroup zlplConnect
 * @{
 */

int  GOLDILOCKSESQL_Connect( void        * aSqlContext,
                        sqlargs_t   * aSqlArgs,
                        char        * aConnString,
                        char        * aUserName,
                        char        * aPassword )
{
    zlplSqlContext  *sSqlContext = NULL;

    STL_TRY_THROW( zlpnGetRealContext( aSqlContext,
                                       (zlplSqlArgs *)aSqlArgs,
                                       &sSqlContext )
                   == STL_SUCCESS, ERR_GET_CONTEXT );

    STL_TRY( zlpnConnectContext( &sSqlContext,
                                 aSqlArgs,
                                 aConnString,
                                 aUserName,
                                 aPassword )
             == STL_SUCCESS );

    return ZLPL_SUCCESS;

    STL_CATCH( ERR_GET_CONTEXT )
    {
        GOLDILOCKSESQL_SQLCA_SET_ERROR( sSqlContext, aSqlArgs );
    }
    STL_FINISH;

    return ZLPL_FAILURE;
}

int  GOLDILOCKSESQL_Disconnect( void       * aSqlContext,
                           sqlargs_t  * aSqlArgs,
                           char       * aConnObject,
                           int          aIsAll )
{
    zlplSqlContext  *sSqlContext = NULL;

    if( aIsAll == 0 )
    {
        STL_TRY( zlpnGetRealContext( aSqlContext,
                                     (zlplSqlArgs *)aSqlArgs,
                                     &sSqlContext )
                 == STL_SUCCESS );

        if( sSqlContext != NULL )
        {
            STL_TRY( zlpnDisconnectContext(sSqlContext,
                                           aSqlArgs,
                                           aConnObject )
                     == STL_SUCCESS);
        }
    }
    else
    {
        STL_TRY( zlpnDisconnectAll(aSqlArgs)
                 == STL_SUCCESS );
    }

    return ZLPL_SUCCESS;

    STL_FINISH;

    GOLDILOCKSESQL_SQLCA_SET_ERROR( sSqlContext, aSqlArgs );

    return ZLPL_FAILURE;
}

int  GOLDILOCKSESQL_AllocContext(void      ** aSqlContext,
                      sqlargs_t  * aSqlArgs )
{
    stlErrorStack    sErrorStack;

    STL_INIT_ERROR_STACK(&sErrorStack);
    STL_TRY_THROW(stlSetThreadOnce(&gThreadOnce,
                                   zlpuInitialize,
                                   &sErrorStack)
                  == STL_SUCCESS, RAMP_ERR_SET_THREAD_ONCE);

    STL_TRY(zlpnAllocContext((zlplSqlContext **)aSqlContext,
                             aSqlArgs)
            == STL_SUCCESS);

    return ZLPL_SUCCESS;

    STL_CATCH(RAMP_ERR_SET_THREAD_ONCE)
    {
        ((zlplSqlca *)aSqlArgs->sql_ca)->sqlcode = -1;
        zlpeSetErrorMsg((zlplSqlca *)aSqlArgs->sql_ca, "Fail to initialize.");
        zlpuPrintErrorStack(&sErrorStack);
        stlExit(-1);
    }
    STL_FINISH;

    return ZLPL_FAILURE;
}

int  GOLDILOCKSESQL_FreeContext( void      ** aSqlContext,
                      sqlargs_t  * aSqlArgs )
{
    STL_TRY(zlpnFreeContext((zlplSqlContext **)aSqlContext,
                            aSqlArgs)
            == STL_SUCCESS);

    return ZLPL_SUCCESS;

    STL_FINISH;

    GOLDILOCKSESQL_SQLCA_SET_ERROR( *aSqlContext, aSqlArgs );

    return ZLPL_FAILURE;
}

/** @} */
