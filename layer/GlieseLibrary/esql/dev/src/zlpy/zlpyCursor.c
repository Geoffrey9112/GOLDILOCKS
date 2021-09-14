/*******************************************************************************
 * zlpyCursor.c
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
#include <zlpyCursor.h>

/**
 * @file zlpyCursor.c
 * @brief Gliese Embedded SQL in C precompiler symbol table library functions
 */

/**
 * @addtogroup zlpySymTab
 * @{
 */

/******************************************************************************
 * Cursor Symbol
 ******************************************************************************/

stlInt32 zlpyCompareCursorSymbolFunc(void *aKeyA, void *aKeyB)
{
    stlChar  *sKeyA = (stlChar *)aKeyA;
    stlChar  *sKeyB = (stlChar *)aKeyB;

    return stlStrcmp(sKeyA, sKeyB);
}

stlUInt32 zlpyCursorSymbolHashFunc(void * aKey, stlUInt32 aBucketCount)
{
    stlUInt32 sHashValue;

    sHashValue = stlGetHashValueString( (stlChar*) aKey );
    return sHashValue % aBucketCount;
}


stlStatus zlpyAddCursorSymbol(zlplSqlContext   *aSqlContext,
                              zlplCursorSymbol *aCursorSymbol)
{
    STL_TRY_THROW( aSqlContext->mCursorSymTab != NULL, RAMP_NO_CONNECTION );

    STL_TRY( stlInsertStaticHash( aSqlContext->mCursorSymTab,
                                  zlpyCursorSymbolHashFunc,
                                  aCursorSymbol,
                                  ZLPL_CTXT_ERROR_STACK(aSqlContext) )
             == STL_SUCCESS);

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

stlStatus zlpyLookupCursorSymbol(zlplSqlContext    * aSqlContext,
                                 stlChar           * aCursorName,
                                 zlplCursorSymbol ** aCursorSymbol )
{
    zlplCursorSymbol * sSymbol = NULL;

    STL_TRY_THROW( aSqlContext->mCursorSymTab != NULL, RAMP_NO_CONNECTION );

    STL_TRY( stlFindStaticHash( aSqlContext->mCursorSymTab,
                                zlpyCursorSymbolHashFunc,
                                zlpyCompareCursorSymbolFunc,
                                aCursorName,
                                (void **) & sSymbol,
                                ZLPL_CTXT_ERROR_STACK(aSqlContext) )
             == STL_SUCCESS );

    /**
     * Output 설정
     */

    *aCursorSymbol = sSymbol;

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
