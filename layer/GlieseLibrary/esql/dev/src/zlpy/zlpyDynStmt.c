/*******************************************************************************
 * zlpyDynStmt.c
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
#include <zlpyDynStmt.h>

/**
 * @file zlpyDynStmt.c
 * @brief Gliese Embedded SQL in C precompiler symbol table library functions
 */

/**
 * @addtogroup zlpySymTab
 * @{
 */

/************************************************************
 * Dynamic Statement Symbol
 ************************************************************/

/**
 * @brief Dynamic Statement Symbol 의 key 를 비교한다.
 * @param[in] aKeyA    Key A
 * @param[in] aKeyB    Key B
 * @return
 * == 0 : equal
 * <> 0 : not-equal
 * @remarks
 */
stlInt32 zlpyCompareDynStmtSymbolFunc(void *aKeyA, void *aKeyB)
{
    stlChar  *sKeyA = (stlChar *)aKeyA;
    stlChar  *sKeyB = (stlChar *)aKeyB;

    return stlStrcmp(sKeyA, sKeyB);
}

/**
 * @brief Dynamic Statement Symbol 의 Hash Value 를 얻는다.
 * @param[in]  aKey          Key
 * @param[in]  aBucketCount  Bucket Count
 * @return
 * hash value
 */
stlUInt32 zlpyDynStmtSymbolHashFunc(void * aKey, stlUInt32 aBucketCount)
{
    stlUInt32 sHashValue;

    sHashValue = stlGetHashValueString( (stlChar*) aKey );
    return sHashValue % aBucketCount;
}


/**
 * @brief Symbol Table 에 Dynamic Statement Symbol 을 추가한다.
 * @param[in]  aSqlContext    SQL Context
 * @param[in]  aDynStmtSymbol Dynamic Statement Symbol
 * @remarks
 */
stlStatus zlpyAddDynStmtSymbol( zlplSqlContext    * aSqlContext,
                                zlplDynStmtSymbol * aDynStmtSymbol )
{
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDynStmtSymbol != NULL, ZLPL_CTXT_ERROR_STACK(aSqlContext) );
    STL_TRY_THROW( aSqlContext->mDynStmtSymTab != NULL, RAMP_NO_CONNECTION );

    /**
     * Dynamic Symbol Table 에 추가
     */

    STL_TRY( stlInsertStaticHash( aSqlContext->mDynStmtSymTab,
                                  zlpyDynStmtSymbolHashFunc,
                                  aDynStmtSymbol,
                                  ZLPL_CTXT_ERROR_STACK(aSqlContext))
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

/**
 * @brief Symbol Table 에서 Dynamic Statement Symbol 을 검색한다.
 * @param[in]  aSqlContext    SQL Context
 * @param[in]  aDynStmtName   Dynamic Statement 의 이름
 * @param[out] aDynStmtSymbol Dynamic Statement Symbol
 * @remarks
 */
stlStatus zlpyLookupDynStmtSymbol( zlplSqlContext     * aSqlContext,
                                   stlChar            * aDynStmtName,
                                   zlplDynStmtSymbol ** aDynStmtSymbol )
{
    zlplDynStmtSymbol * sSymbol = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDynStmtName   != NULL, ZLPL_CTXT_ERROR_STACK(aSqlContext) );
    STL_PARAM_VALIDATE( aDynStmtSymbol != NULL, ZLPL_CTXT_ERROR_STACK(aSqlContext) );
    STL_TRY_THROW( aSqlContext->mDynStmtSymTab != NULL, RAMP_NO_CONNECTION );

    /**
     * Dynamic Symbol Table 에서 검색
     */

    STL_TRY( stlFindStaticHash( aSqlContext->mDynStmtSymTab,
                                zlpyDynStmtSymbolHashFunc,
                                zlpyCompareDynStmtSymbolFunc,
                                aDynStmtName,
                                (void **) & sSymbol,
                                ZLPL_CTXT_ERROR_STACK(aSqlContext) )
             == STL_SUCCESS );

    /**
     * Output 설정
     */

    *aDynStmtSymbol = sSymbol;

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
