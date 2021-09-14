/*******************************************************************************
 * zlpySymTab.c
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
#include <zlpcOpenCursor.h>
#include <zlpcFetchCursor.h>
#include <zlpySymTab.h>
#include <zlpxDynExecute.h>
#include <zlpxExecute.h>

/**
 * @file zlpySymTab.c
 * @brief Gliese Embedded SQL in C precompiler symbol table library functions
 */

/**
 * @addtogroup zlpySymTab
 * @{
 */

/************************************************************
 * Symbol Table
 ************************************************************/

/**
 * @brief SQL Context 에 Symbol Table 들을 생성한다.
 * @param[in] aSqlContext SQL Context
 * @remarks
 */
stlStatus zlpyCreateSymbolTable(zlplSqlContext *aSqlContext)
{
    /**
     * Cursor Symbol Table 생성
     */

    STL_TRY( stlCreateStaticHash( &aSqlContext->mCursorSymTab,
                                  ZLPL_CURSORHASH_BUCKET_COUNT,
                                  STL_OFFSETOF(zlplCursorSymbol, mLink),
                                  STL_OFFSETOF(zlplCursorSymbol, mCursorName),
                                  STL_STATICHASH_MODE_BUCKETLOCK,
                                  ZLPL_CTXT_ERROR_STACK(aSqlContext) )
             == STL_SUCCESS );

    /**
     * Query Symbol Table 생성
     */

    STL_TRY( stlCreateStaticHash( &aSqlContext->mQuerySymTab,
                                  ZLPL_SQLSTMTHASH_BUCKET_COUNT,
                                  STL_OFFSETOF(zlplSqlStmtSymbol, mLink),
                                  STL_OFFSETOF(zlplSqlStmtSymbol, mKey),
                                  STL_STATICHASH_MODE_BUCKETLOCK,
                                  ZLPL_CTXT_ERROR_STACK(aSqlContext) )
             == STL_SUCCESS );

    /**
     * Dynamic Statement Symbol Table 생성
     */

    STL_TRY( stlCreateStaticHash( & aSqlContext->mDynStmtSymTab,
                                  ZLPL_DYNAMIC_STMT_HASH_BUCKET_COUNT,
                                  STL_OFFSETOF(zlplDynStmtSymbol, mLink),
                                  STL_OFFSETOF(zlplDynStmtSymbol, mStmtName),
                                  STL_STATICHASH_MODE_BUCKETLOCK,
                                  ZLPL_CTXT_ERROR_STACK(aSqlContext) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( aSqlContext->mCursorSymTab != NULL )
    {
        (void)stlDestroyStaticHash( &aSqlContext->mCursorSymTab,
                                    ZLPL_CTXT_ERROR_STACK(aSqlContext) );
        aSqlContext->mCursorSymTab = NULL;
    }

    if( aSqlContext->mQuerySymTab != NULL )
    {
        (void)stlDestroyStaticHash( &aSqlContext->mQuerySymTab,
                                    ZLPL_CTXT_ERROR_STACK(aSqlContext) );
        aSqlContext->mQuerySymTab = NULL;
    }

    if( aSqlContext->mDynStmtSymTab != NULL )
    {
        (void)stlDestroyStaticHash( &aSqlContext->mDynStmtSymTab,
                                    ZLPL_CTXT_ERROR_STACK(aSqlContext) );
        aSqlContext->mDynStmtSymTab = NULL;
    }

    stlPushError( STL_ERROR_LEVEL_ABORT,
                  ZLPL_ERRCODE_CREATE_SYMTAB,
                  NULL,
                  &aSqlContext->mErrorStack );

    return STL_FAILURE;
}

/**
 * @brief SQL Context 의 Symbol Table 들을 제거한다.
 * @param[in] aSqlContext SQL Context
 * @remarks
 */
stlStatus zlpyDestroySymbolTable(zlplSqlContext *aSqlContext)
{
    stlUInt32             sBucketSeq;

    zlplCursorSymbol    * sCursorSymbol = NULL;
    zlplSqlStmtSymbol   * sSqlSymbol = NULL;
    zlplDynStmtSymbol   * sDynStmtSymbol = NULL;

    /**
     * Cursor Symbol Table 의 statement handle free
     */

    STL_TRY_THROW( aSqlContext->mDynStmtSymTab != NULL,
                   RAMP_EXIT_FUNC );

    STL_TRY( stlGetFirstStaticHashItem( aSqlContext->mCursorSymTab,
                                        & sBucketSeq,
                                        (void **) & sCursorSymbol,
                                        ZLPL_CTXT_ERROR_STACK(aSqlContext) )
             == STL_SUCCESS );

    while( sCursorSymbol != NULL )
    {
        /* 이미 Free 됨 */
        /* zlplFreeStmt( sCursorSymbol->mStmtHandle ); */
        sCursorSymbol->mStmtHandle = NULL;

        if ( sCursorSymbol->mQueryString != NULL )
        {
            STL_TRY( stlFreeDynamicMem( & aSqlContext->mDynMemory,
                                        sCursorSymbol->mQueryString,
                                        ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                     == STL_SUCCESS );
        }
        sCursorSymbol->mQueryString = NULL;

        STL_TRY( zlpcFreeCursorSymbolParam( aSqlContext,
                                            sCursorSymbol )
                 == STL_SUCCESS );

        STL_TRY( zlpcFreeCursorSymbolColumn( aSqlContext,
                                             sCursorSymbol )
                 == STL_SUCCESS );

        STL_TRY( stlGetNextStaticHashItem( aSqlContext->mCursorSymTab,
                                           & sBucketSeq,
                                           (void **) & sCursorSymbol,
                                           ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
    }

    (void)stlDestroyStaticHash( &aSqlContext->mCursorSymTab,
                                ZLPL_CTXT_ERROR_STACK(aSqlContext) );
    aSqlContext->mCursorSymTab = NULL;

    /**
     * Query Symbol Table 의 statement handle free
     */

    STL_TRY_THROW( aSqlContext->mQuerySymTab != NULL,
                   RAMP_EXIT_FUNC );

    STL_TRY( stlGetFirstStaticHashItem( aSqlContext->mQuerySymTab,
                                        & sBucketSeq,
                                        (void **) & sSqlSymbol,
                                        ZLPL_CTXT_ERROR_STACK(aSqlContext) )
             == STL_SUCCESS );

    while( sSqlSymbol != NULL )
    {
        /* 이미 Free 됨 */
        /* zlplFreeStmt( sSqlSymbol->mStmtHandle ); */
        sSqlSymbol->mStmtHandle = NULL;

        STL_TRY( zlpxFreeHostVarTempBuffer(aSqlContext,
                                           sSqlSymbol)
                 == STL_SUCCESS );

        STL_TRY( stlGetNextStaticHashItem( aSqlContext->mQuerySymTab,
                                           & sBucketSeq,
                                           (void **) & sSqlSymbol,
                                           ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
    }

    (void)stlDestroyStaticHash( &aSqlContext->mQuerySymTab,
                                ZLPL_CTXT_ERROR_STACK(aSqlContext) );
    aSqlContext->mQuerySymTab = NULL;

    /**
     * Dynamic Statement Symbol Table 의 자원 해제
     */

    STL_TRY_THROW( aSqlContext->mDynStmtSymTab != NULL,
                   RAMP_EXIT_FUNC );

    STL_TRY( stlGetFirstStaticHashItem( aSqlContext->mDynStmtSymTab,
                                        & sBucketSeq,
                                        (void **) & sDynStmtSymbol,
                                        ZLPL_CTXT_ERROR_STACK(aSqlContext) )
             == STL_SUCCESS );

    while( sDynStmtSymbol != NULL )
    {
        /* 이미 Free 됨 */
        /* zlplFreeStmt( sDynStmtSymbol->mStmtHandle ); */
        sDynStmtSymbol->mStmtHandle = NULL;

        if ( sDynStmtSymbol->mPrevSQL != NULL )
        {
            STL_TRY( stlFreeDynamicMem( & aSqlContext->mDynMemory,
                                        sDynStmtSymbol->mPrevSQL,
                                        ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                     == STL_SUCCESS );
        }
        sDynStmtSymbol->mPrevSQL = NULL;

        STL_TRY( zlpxFreeDynStmtSymbolMemory( aSqlContext,
                                              sDynStmtSymbol )
                 == STL_SUCCESS );

        STL_TRY( stlGetNextStaticHashItem( aSqlContext->mDynStmtSymTab,
                                           & sBucketSeq,
                                           (void **) & sDynStmtSymbol,
                                           ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
    }

    (void)stlDestroyStaticHash( &aSqlContext->mDynStmtSymTab,
                                ZLPL_CTXT_ERROR_STACK(aSqlContext) );
    aSqlContext->mDynStmtSymTab = NULL;

    STL_RAMP( RAMP_EXIT_FUNC );

    return STL_SUCCESS;

    STL_FINISH;

    stlPushError( STL_ERROR_LEVEL_ABORT,
                  ZLPL_ERRCODE_DESTROY_SYMTAB,
                  NULL,
                  &aSqlContext->mErrorStack );

    return STL_FAILURE;
}


/** @} zlpySymTab */
