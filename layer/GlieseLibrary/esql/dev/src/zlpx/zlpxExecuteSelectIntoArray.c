/*******************************************************************************
 * zlpxExecuteSelectIntoArray.c
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
#include <zlpbBind.h>
#include <zlpeSqlca.h>
#include <zlpnConnect.h>
#include <zlpuMisc.h>
#include <zlpxExecute.h>
#include <zlpxExecuteSelectIntoArray.h>
#include <zlpyQueryStmt.h>

/**
 * @file zlpxExecuteSelectIntoArray.c
 * @brief Gliese Embedded SQL in C precompiler SQL execution library functions
 */

/**
 * @addtogroup zlpxExecuteSelectIntoArray
 * @{
 */


stlStatus zlpxExecuteSelectIntoArray(zlplSqlContext   *aSqlContext,
                                     zlplSqlArgs      *aSqlArgs)
{
    SQLRETURN            sReturn;
    stlInt32             sOutParamCount;
    stlInt32             sInParamCount;
    zlplSqlStmtSymbol    sParamSymbol;
    zlplSqlStmtSymbol    sResultSymbol;
    zlplSqlHostVar      *sOutParamList = NULL;
    zlplSqlHostVar      *sInParamList = NULL;
    SQLLEN               sAffectedRowCount = 0;
    zlplConnInfo        *sConnInfo = ZLPL_CTXT_CONNECTION(aSqlContext);
    SQLHANDLE            sStmtHandle = SQL_NULL_HANDLE;
    stlChar             *sRefinedSqlStmt = NULL;
    stlInt32             sSqlLen;
    stlInt32             sIteration;
    stlInt32             sState = 0;

    sOutParamCount = zlpxGetParamCount( aSqlArgs->hvc,
                                        aSqlArgs->sqlhv,
                                        ZLPL_PARAM_IO_TYPE_OUT );

    sInParamCount = zlpxGetParamCount( aSqlArgs->hvc,
                                       aSqlArgs->sqlhv,
                                       ZLPL_PARAM_IO_TYPE_IN );

    STL_TRY( stlAllocDynamicMem( & aSqlContext->mDynMemory,
                                 STL_SIZEOF(zlplSqlHostVar) * sOutParamCount,
                                 (void **) & sOutParamList,
                                 ZLPL_CTXT_ERROR_STACK(aSqlContext) )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( stlAllocDynamicMem( & aSqlContext->mDynMemory,
                                 STL_SIZEOF(zlplSqlHostVar) * sInParamCount,
                                 (void **) & sInParamList,
                                 ZLPL_CTXT_ERROR_STACK(aSqlContext) )
             == STL_SUCCESS );
    sState = 2;

    STL_TRY( zlpxCopyParamList( sOutParamList,
                                aSqlArgs->sqlhv,
                                aSqlArgs->hvc,
                                ZLPL_PARAM_IO_TYPE_OUT )
             == STL_SUCCESS );

    STL_TRY( zlpxCopyParamList( sInParamList,
                                aSqlArgs->sqlhv,
                                aSqlArgs->hvc,
                                ZLPL_PARAM_IO_TYPE_IN )
             == STL_SUCCESS );

    STL_TRY( zlpuAllocStmt( & sStmtHandle, sConnInfo->mDbc, aSqlArgs ) == STL_SUCCESS);
    sState = 3;

    (void)zlpuGetMinArraySize( aSqlArgs->iters,
                               sOutParamCount,
                               sOutParamList,
                               &sIteration );

    STL_TRY( zlpxAllocHostVarTempBuffer( aSqlContext,
                                         &sResultSymbol,
                                         sIteration,
                                         sOutParamCount,
                                         sOutParamList )
             == STL_SUCCESS );

    sState = 4;

    STL_TRY( zlpxAllocHostVarTempBuffer( aSqlContext,
                                         &sParamSymbol,
                                         sIteration,
                                         sInParamCount,
                                         sInParamList )
             == STL_SUCCESS );
    sState = 5;

    STL_TRY( zlpbBindCols( sStmtHandle,
                           aSqlArgs,
                           sOutParamCount,
                           sOutParamList,
                           sResultSymbol.mValueBuf,
                           sResultSymbol.mParamLocalIndArray )
             == STL_SUCCESS );

    STL_TRY( zlpbBindParams( sStmtHandle,
                             aSqlArgs,
                             sInParamCount,
                             sInParamList,
                             sParamSymbol.mValueBuf,
                             sParamSymbol.mParamLocalIndArray,
                             NULL )
             == STL_SUCCESS );

    sSqlLen = stlStrlen( aSqlArgs->sqlstmt );
    STL_TRY( stlAllocDynamicMem( & aSqlContext->mDynMemory,
                                 sSqlLen + 1,
                                 (void **) & sRefinedSqlStmt,
                                 ZLPL_CTXT_ERROR_STACK(aSqlContext) )
             == STL_SUCCESS );
    sState = 6;

    STL_TRY( zlpxRemoveIntoClause( aSqlContext,
                                   aSqlArgs->sqlstmt,
                                   sRefinedSqlStmt )
             == STL_SUCCESS );

    STL_TRY( zlpbSetBeforeInfo( aSqlContext,
                                aSqlArgs,
                                sStmtHandle,
                                sInParamCount,
                                sInParamList,
                                1,
                                &sParamSymbol.mParamProcessed,
                                sParamSymbol.mParamStatusArray,
                                sParamSymbol.mValueBuf,
                                sParamSymbol.mParamLocalIndArray,
                                NULL,
                                STL_TRUE )   /* Bind Parameter */
             == STL_SUCCESS );

    sReturn = SQLExecDirect( sStmtHandle, (SQLCHAR *)sRefinedSqlStmt, SQL_NTS );
    STL_TRY(zlpeCheckError( sReturn,
                            SQL_HANDLE_STMT,
                            sStmtHandle,
                            (zlplSqlca *)aSqlArgs->sql_ca,
                            (stlChar *)aSqlArgs->sql_state )
            == STL_SUCCESS);

    STL_TRY( zlpbSetBeforeInfo( aSqlContext,
                                aSqlArgs,
                                sStmtHandle,
                                sOutParamCount,
                                sOutParamList,
                                sIteration,
                                &sResultSymbol.mParamProcessed,
                                sResultSymbol.mParamStatusArray,
                                sResultSymbol.mValueBuf,
                                sResultSymbol.mParamLocalIndArray,
                                NULL,
                                STL_FALSE )   /* Bind Cols */
             == STL_SUCCESS );

    sReturn = SQLFetch( sStmtHandle );
    STL_TRY(zlpeCheckError( sReturn,
                            SQL_HANDLE_STMT,
                            sStmtHandle,
                            (zlplSqlca *)aSqlArgs->sql_ca,
                            (stlChar *)aSqlArgs->sql_state )
            == STL_SUCCESS);

    /*
    if( sReturn == SQL_NO_DATA )
    {
        if( sAffectedRowCount > 0 )
        {
            INIT_SQLCA( aSqlArgs->sql_ca );
        }

        break;
    }
    */

    STL_TRY( zlpbSetAfterInfo( aSqlContext,
                               aSqlArgs,
                               sStmtHandle,
                               aSqlArgs->unsafenull,
                               sOutParamCount,
                               sOutParamList,
                               sIteration,
                               sResultSymbol.mValueBuf,
                               sResultSymbol.mParamLocalIndArray,
                               NULL )
             == STL_SUCCESS );

    /* Affected Row count set */
    sAffectedRowCount = sResultSymbol.mParamProcessed;
    ((zlplSqlca *)(aSqlArgs->sql_ca))->sqlerrd[2] = (int)sAffectedRowCount;

    sReturn = SQLCloseCursor( sStmtHandle );
    STL_TRY(zlpeCheckError( sReturn,
                            SQL_HANDLE_STMT,
                            sStmtHandle,
                            (zlplSqlca *)aSqlArgs->sql_ca,
                            (stlChar *)aSqlArgs->sql_state )
            == STL_SUCCESS);

    sReturn = SQLEndTran( SQL_HANDLE_DBC,
                          sConnInfo->mDbc,
                          SQL_COMMIT );
    STL_TRY(zlpeCheckError( sReturn,
                            SQL_HANDLE_DBC,
                            sConnInfo->mDbc,
                            (zlplSqlca *)aSqlArgs->sql_ca,
                            (stlChar *)aSqlArgs->sql_state )
            == STL_SUCCESS);

    sState = 5;
    STL_TRY( stlFreeDynamicMem( & aSqlContext->mDynMemory,
                                sRefinedSqlStmt,
                                ZLPL_CTXT_ERROR_STACK(aSqlContext) )
             == STL_SUCCESS );

    sState = 4;
    STL_TRY( zlpxFreeHostVarTempBuffer( aSqlContext,
                                        &sParamSymbol )
             == STL_SUCCESS );

    sState = 3;
    STL_TRY( zlpxFreeHostVarTempBuffer( aSqlContext,
                                        &sResultSymbol )
             == STL_SUCCESS );

    sState = 2;
    STL_TRY( zlpuFreeStmt( sStmtHandle ) == STL_SUCCESS);

    sState = 1;
    STL_TRY( stlFreeDynamicMem( & aSqlContext->mDynMemory,
                                sInParamList,
                                ZLPL_CTXT_ERROR_STACK(aSqlContext) )
             == STL_SUCCESS );

    sState = 0;
    STL_TRY( stlFreeDynamicMem( & aSqlContext->mDynMemory,
                                sOutParamList,
                                ZLPL_CTXT_ERROR_STACK(aSqlContext) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 6:
            (void)stlFreeDynamicMem( & aSqlContext->mDynMemory,
                                     sRefinedSqlStmt,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) );
        case 5:
            (void)zlpxFreeHostVarTempBuffer( aSqlContext,
                                             &sParamSymbol );
        case 4:
            (void)zlpxFreeHostVarTempBuffer( aSqlContext,
                                             &sResultSymbol );
        case 3:
            (void)zlpuFreeStmt( sStmtHandle );
        case 2:
            (void)stlFreeDynamicMem( & aSqlContext->mDynMemory,
                                     sInParamList,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) );
        case 1:
            (void)stlFreeDynamicMem( & aSqlContext->mDynMemory,
                                     sOutParamList,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) );
            break;
        default:
            break;
    }

    return STL_FAILURE;
}

stlBool zlpxIsSelectIntoArray(zlplSqlContext   *aSqlContext,
                              zlplSqlArgs      *aSqlArgs)
{
    stlBool          sResult = STL_FALSE;
    stlInt32         i;
    zlplSqlHostVar  *sHostVar;

    if( aSqlArgs->sqltype == ZLPL_STMT_SELECT_INTO )
    {
        sResult = STL_TRUE;

        sHostVar = aSqlArgs->sqlhv;
        for( i = 0; i < aSqlArgs->hvc; i++ )
        {
            if( sHostVar[i].io_type == ZLPL_PARAM_IO_TYPE_OUT )
            {
                if( sHostVar[i].arr_size <= 0 )
                {
                    sResult = STL_FALSE;
                    break;
                }
            }
        }
    }

    return sResult;
}

stlInt32 zlpxGetParamCount(stlInt32          aHostVarCount,
                           zlplSqlHostVar   *aHostVar,
                           zlplParamIOType   aParamIoType)
{
    stlInt32         sParamCount = 0;
    stlInt32         i;

    for( i = 0; i < aHostVarCount; i ++ )
    {
        if( aHostVar[i].io_type == aParamIoType )
        {
            sParamCount ++;
        }
    }

    return sParamCount;
}

stlStatus zlpxCopyParamList( zlplSqlHostVar   *aDestHostVar,
                             zlplSqlHostVar   *aSrcHostVar,
                             stlInt32          aParamCount,
                             zlplParamIOType   aParamIoType )
{
    stlInt32         sSrcIdx;
    stlInt32         sDestIdx = 0;

    for( sSrcIdx = 0; sSrcIdx < aParamCount; sSrcIdx ++ )
    {
        if( aSrcHostVar[sSrcIdx].io_type == aParamIoType )
        {
            ZLPL_COPY_HOST_VAR( &aDestHostVar[sDestIdx], &aSrcHostVar[sSrcIdx] );
            sDestIdx ++;
        }
    }

    return STL_SUCCESS;
}

stlStatus zlpxRemoveIntoClause( zlplSqlContext   *aSqlContext,
                                stlChar          *aSrcSqlStmt,
                                stlChar          *aDestSqlStmt )
{
    stlInt32    sSqlLen;
    stlInt32    sBeginPos;
    stlInt32    sEndPos;
    stlInt32    i;
    stlChar    *sIntoPtr;
    stlChar    *sFromPtr;

    sSqlLen = stlStrlen( aSrcSqlStmt );

    stlStrncpy( aDestSqlStmt, aSrcSqlStmt, sSqlLen + 1 );

    for( i = 0; i < sSqlLen; i ++ )
    {
        aDestSqlStmt[i] = stlToupper( aDestSqlStmt[i] );
    }

    /*
     * Find 'INTO', 'FROM' keyword
     */
    sIntoPtr = stlStrstr( aDestSqlStmt, "INTO" );
    sFromPtr = stlStrstr( aDestSqlStmt, "FROM" );

    sBeginPos = sIntoPtr - aDestSqlStmt;
    sEndPos   = sFromPtr - aDestSqlStmt;

    /*
     * recover original sql stmt
     */
    stlStrncpy( aDestSqlStmt, aSrcSqlStmt, sSqlLen + 1 );

    /*
     * Remove Into Clause
     */
    for( i = sBeginPos; i < sEndPos; i ++ )
    {
        aDestSqlStmt[i] = ' ';
    }

    return STL_SUCCESS;
}

/** @} */
