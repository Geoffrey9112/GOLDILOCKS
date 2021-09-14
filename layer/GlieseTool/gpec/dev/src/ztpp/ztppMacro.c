/*******************************************************************************
 * ztppMacro.c
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
#include <dtl.h>
#include <goldilocks.h>
#include <sqlext.h>
#include <ztpDef.h>
#include <ztppMacro.h>
#include <ztppFunc.h>
#include <ztpMisc.h>
#include <ztphStaticHash.h>

/**
 * @file ztppMacro.c
 * @brief Preprocessor macro handler (translation phse 3~4)
 */

/**
 * @addtogroup ztppMacro
 * @{
 */

/**
 * @brief C Preprocessor의 Macro Symbol 의 key 를 비교한다.
 * @param[in] aKeyA    Key A
 * @param[in] aKeyB    Key B
 * @return
 * == 0 : equal
 * <> 0 : not-equal
 * @remarks
 */
stlInt32 ztppCompareMacroSymbolFunc(void *aKeyA, void *aKeyB)
{
    stlChar  *sKeyA = *(stlChar **)aKeyA;
    stlChar  *sKeyB = *(stlChar **)aKeyB;

    return stlStrcmp(sKeyA, sKeyB);
}

/**
 * @brief C Preprocessor의 Macro Symbol 의 Hash Value 를 얻는다.
 * @param[in]  aKey          Key
 * @param[in]  aBucketCount  Bucket Count
 * @return
 * hash value
 */
stlUInt32 ztppMacroSymbolHashFunc(void * aKey, stlUInt32 aBucketCount)
{
    stlUInt32 sHashValue;

    stlChar *sKey = *(stlChar **)aKey;
    sHashValue = ztphGetHashValueString( (stlChar*) sKey );
    return sHashValue % aBucketCount;
}

/**
 * @brief Macro Symbol을 관리할 Hash table을 생성한다.
 * @param[in]  aKey          Key
 * @param[in]  aBucketCount  Bucket Count
 * @return
 * STL_SUCCESS/STL_FAILURE
 */
stlStatus ztppCreateSymbolTable(stlAllocator   *aAllocator,
                                stlErrorStack  *aErrorStack)
{
    STL_TRY( ztphCreateStaticHash( aAllocator,
                                   aErrorStack,
                                   &gMacroSymTab,
                                   ZTP_MACROHASH_BUCKET_COUNT,
                                   STL_OFFSETOF(ztpMacroSymbol, mLink),
                                   STL_OFFSETOF(ztpMacroSymbol, mMacroName) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    gMacroSymTab = NULL;

    return STL_FAILURE;
}

stlStatus ztppDestroySymbolTable(stlErrorStack  *aErrorStack)
{
    stlUInt32             sBucketSeq;
    ztpMacroSymbol      *sMacroSymbol = NULL;

    STL_TRY_THROW( gMacroSymTab != NULL, RAMP_EXIT_FUNC );

    STL_TRY( ztphGetFirstItem( aErrorStack,
                               gMacroSymTab,
                               & sBucketSeq,
                               (void **)&sMacroSymbol )
             == STL_SUCCESS );
    while( sMacroSymbol != NULL )
    {
        /* 정리할 것 있으면 정리한다. */

        STL_TRY( ztphGetNextItem( aErrorStack,
                                  gMacroSymTab,
                                  & sBucketSeq,
                                  (void **)&sMacroSymbol )
                 == STL_SUCCESS );
    }

    gMacroSymTab = NULL;

    STL_RAMP( RAMP_EXIT_FUNC );

    return STL_SUCCESS;

    STL_FINISH;

    stlPushError( STL_ERROR_LEVEL_ABORT,
                  ZTP_ERRCODE_DESTROY_SYMTAB,
                  NULL,
                  aErrorStack );

    return STL_FAILURE;
}

stlStatus ztppAddMacroSymbol(stlErrorStack   *aErrorStack,
                             ztpMacroSymbol  *aMacroSymbol)
{
    STL_TRY( ztphInsertStaticHash( aErrorStack,
                                   gMacroSymTab,
                                   ztppMacroSymbolHashFunc,
                                   aMacroSymbol )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztppLookupMacroSymbol(stlErrorStack     *aErrorStack,
                                stlChar           *aMacroName,
                                ztpMacroSymbol   **aMacroSymbol)
{
    STL_TRY( ztphFindStaticHash( aErrorStack,
                                 gMacroSymTab,
                                 ztppMacroSymbolHashFunc,
                                 ztppCompareMacroSymbolFunc,
                                 &aMacroName,
                                 (void **)aMacroSymbol )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztppDeleteMacroSymbol(stlErrorStack   *aErrorStack,
                                ztpMacroSymbol  *aMacroSymbol)
{
    STL_TRY( ztphRemoveStaticHash( aErrorStack,
                                   gMacroSymTab,
                                   ztppMacroSymbolHashFunc,
                                   aMacroSymbol )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
