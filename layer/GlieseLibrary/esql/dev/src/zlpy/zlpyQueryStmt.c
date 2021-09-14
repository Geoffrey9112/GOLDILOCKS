/*******************************************************************************
 * zlpyQueryStmt.c
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
#include <zlpyQueryStmt.h>

/**
 * @file zlpyQueryStmt.c
 * @brief Gliese Embedded SQL in C precompiler symbol table library functions
 */

/**
 * @addtogroup zlpySymTab
 * @{
 */


/************************************************************
 * Query Statement Symbol
 ************************************************************/

stlInt32 zlpyCompareSqlStmtFunc(void *aKeyA, void *aKeyB)
{
    stlBool           sResult;
    zlplSqlStmtKey *sKeyA = (zlplSqlStmtKey *)aKeyA;
    zlplSqlStmtKey *sKeyB = (zlplSqlStmtKey *)aKeyB;

    if(sKeyA->mLine == sKeyB->mLine)
    {
        sResult = stlStrcmp(sKeyA->mFileName, sKeyB->mFileName);
    }
    else
    {
        sResult = 1;
    }

    return sResult;
}

stlUInt32 zlpySqlStmtHashFunc( void * aKey, stlUInt32 aBucketCount )
{
    return (*(stlUInt32*)aKey) % aBucketCount;
}


stlStatus zlpyAddSqlStmtSymbol(zlplSqlContext      *aSqlContext,
                               zlplSqlStmtSymbol   *aSqlStmtSymbol)
{
    STL_TRY_THROW( aSqlContext->mQuerySymTab != NULL, RAMP_NO_CONNECTION );

    STL_TRY( stlInsertStaticHash( aSqlContext->mQuerySymTab,
                                  zlpySqlStmtHashFunc,
                                  aSqlStmtSymbol,
                                  ZLPL_CTXT_ERROR_STACK(aSqlContext) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_NO_CONNECTION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_CONNECTION_NOT_OPEN,
                      NULL,
                      ZLPL_CTXT_ERROR_STACK(aSqlContext) );
    }
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlpyLookupSqlStmtSymbol(zlplSqlContext       *aSqlContext,
                                  zlplSqlStmtKey       *aSqlStmtKey,
                                  zlplSqlStmtSymbol   **aSqlStmtSymbol,
                                  stlBool              *aIsFound)
{
    STL_TRY_THROW( aSqlContext->mQuerySymTab != NULL, RAMP_NO_CONNECTION );

    STL_TRY( stlFindStaticHash( aSqlContext->mQuerySymTab,
                                zlpySqlStmtHashFunc,
                                zlpyCompareSqlStmtFunc,
                                aSqlStmtKey,
                                (void **)aSqlStmtSymbol,
                                ZLPL_CTXT_ERROR_STACK(aSqlContext) )
             == STL_SUCCESS );

    if(*aSqlStmtSymbol != NULL)
    {
        *aIsFound = STL_TRUE;
    }
    else
    {
        *aIsFound = STL_FALSE;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_NO_CONNECTION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_CONNECTION_NOT_OPEN,
                      NULL,
                      ZLPL_CTXT_ERROR_STACK(aSqlContext) );
    }
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlpyDeleteSqlStmtSymbol(zlplSqlContext       *aSqlContext,
                                  zlplSqlStmtSymbol    *aSqlStmtSymbol)
{
    STL_TRY_THROW( aSqlContext->mQuerySymTab != NULL, RAMP_NO_CONNECTION );

    STL_TRY( stlRemoveStaticHash( aSqlContext->mQuerySymTab,
                                  zlpySqlStmtHashFunc,
                                  aSqlStmtSymbol,
                                  ZLPL_CTXT_ERROR_STACK(aSqlContext) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_NO_CONNECTION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_CONNECTION_NOT_OPEN,
                      NULL,
                      ZLPL_CTXT_ERROR_STACK(aSqlContext) );
    }
    STL_FINISH;

    return STL_FAILURE;
}

/** @} zlpySymTab */
