/*******************************************************************************
 * ztpcHostVar.c
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

#include <dtl.h>
#include <goldilocks.h>
#include <ztpDef.h>
#include <ztpcHostVar.h>
#include <ztpcTypeDef.h>
#include <ztpdConvert.h>
#include <ztpdRefine.h>
#include <ztpdType.h>
#include <ztpuFile.h>

/**
 * @file ztpcHostVar.c
 * @brief Gliese Embedded SQL in C precompiler common functions on Host variable
 */

/**
 * @addtogroup ztpcHostVar
 * @{
 */

/************************************************************************
 * Host Variable
 ************************************************************************/

stlStatus ztpcAddSymbol( ztpCVariable *aCVariable,
                         stlBool       aIsTypedef )
{
    {
        /* Debug */
        /*
        ztpCVariable  *sDebugCVar;

        stlPrintf( "Add Symbol : SymTab [%p]\n", gCurrSymTab );
        for( sDebugCVar = aCVariable;
             sDebugCVar != NULL;
             sDebugCVar = sDebugCVar->mNext )
        {
            stlPrintf( "   Symbol Name : [%s], NamePos : [%d]\n",
                       sDebugCVar->mName, sDebugCVar->mNamePos );
        }
        */
    }

    if( aIsTypedef == STL_TRUE )
    {
        {
            /* Debug */
            /*
            ztpCVariable  *sDebugCVar;

            stlPrintf( "Add TypeDef : SymTab [%p]\n", gCurrSymTab );
            for( sDebugCVar = aCVariable;
                 sDebugCVar != NULL;
                 sDebugCVar = sDebugCVar->mNext )
            {
                stlPrintf( "   Symbol Name : [%s]\n", sDebugCVar->mName );
            }
            */
        }

        STL_TRY(ztpcAddTypeDefToSymTab(gCurrSymTab,
                                       aCVariable)
                == STL_SUCCESS);
    }
    else
    {
        STL_TRY(ztpcAddCVariableToSymTab(gCurrSymTab,
                                         aCVariable)
                == STL_SUCCESS);
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztpcAddCVariableToSymTab(ztpSymTabHeader  *aSymTab,
                                   ztpCVariable     *aCVariableList)
{
    ztpCVariable   *sLast;

    if( aSymTab->mCVariableHead == NULL )
    {
        aSymTab->mCVariableHead = aCVariableList;
    }
    else
    {
        sLast = aSymTab->mCVariableHead;

        while( sLast->mNext != NULL )
        {
            sLast = sLast->mNext;
        }

        sLast->mNext = aCVariableList;
    }

    return STL_SUCCESS;
}

stlStatus ztpcFindCVariableInSymTab(stlChar         *aSymName,
                                    ztpCVariable   **aCVariable)
{
    ztpSymTabHeader  *sSymTab = NULL;
    ztpCVariable     *sSymEntry = NULL;

    *aCVariable = NULL;
    sSymTab = gCurrSymTab;
    while(sSymTab != NULL)
    {
        {
            /* Debug */
            /*
            stlPrintf( "Find Symbol : SymTab [%p], Name : [%s]\n",
                       sSymTab, aSymName );
            */
        }
        for(sSymEntry = sSymTab->mCVariableHead;
            sSymEntry != NULL;
            sSymEntry = sSymEntry->mNext)
        {
            /* Debug */
            /*
            stlPrintf( "    SymTabEntry : [%s], Avail : %d\n",
                       sSymEntry->mName,
                       sSymEntry->mIsAvailHostVar );
            */

            if(sSymEntry->mIsAvailHostVar == STL_FALSE)
            {
                continue;
            }

            if(stlStrncmp(sSymEntry->mName,
                          aSymName,
                          STL_MAX_SQL_IDENTIFIER_LENGTH) == 0)
            {
                *aCVariable = sSymEntry;
                STL_THROW(EXIT_FUNCTION);
            }
        }

        sSymTab = sSymTab->mPrevDepth;
    }

    STL_RAMP(EXIT_FUNCTION);
    STL_TRY(*aCVariable != NULL);

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus
ztpcFindMemberNode( stlChar        *aSymbolName,
                    ztpCVariable   *aStruct,
                    ztpCVariable  **aNode )
{
    ztpCVariable  *sMemberNode;

    *aNode = NULL;

    for( sMemberNode = aStruct->mStructMember;
         sMemberNode != NULL;
         sMemberNode = sMemberNode->mNext )
    {
        if( stlStrcmp( aSymbolName, sMemberNode->mName ) == 0 )
        {
            *aNode = sMemberNode;
            break;
        }
    }

    return STL_SUCCESS;
}

void ztpcSymbol2HostVar( ztpCVariable  *aDest,
                         ztpCVariable  *aSrc )
{
    aDest->mIsArray = aSrc->mIsArray;
    aDest->mIsStructMember = aSrc->mIsStructMember;
    aDest->mIsAvailHostVar = aSrc->mIsAvailHostVar;
    aDest->mParamIOType = aSrc->mParamIOType;
    aDest->mDataType = aSrc->mDataType;
    aDest->mAddrDepth = aSrc->mAddrDepth;
    aDest->mArrayValue = aSrc->mArrayValue;

    aDest->mCharLength = aSrc->mCharLength;
    aDest->mPrecision = aSrc->mPrecision;
    aDest->mScale = aSrc->mScale;

    aDest->mStructMember = aSrc->mStructMember;

    return;
}

stlStatus ztpcHostVar2CVariable( ztpCVariable     *aCVariable,
                                 ztpHostVariable  *aHostVariableDesc,
                                 stlBool          *aIsSimpleVar,
                                 stlErrorStack    *aErrorStack )
{
    ztpCVariable     *sSymbol = NULL;
    ztpCVariable     *sMemberNode = NULL;
    ztpHostVariable  *sCurrHostVarNode;

    *aIsSimpleVar = STL_TRUE;

    sCurrHostVarNode = aHostVariableDesc;
    while( sCurrHostVarNode != NULL )
    {
        switch( sCurrHostVarNode->mHostVarDesc )
        {
            case ZTP_HOSTVAR_DESC_ID:
                STL_TRY_THROW(ztpcFindCVariableInSymTab(sCurrHostVarNode->mValue,
                                                        &sSymbol)
                              == STL_SUCCESS, ERR_SYMBOL_NOT_FOUND);

                ztpcSymbol2HostVar( aCVariable, sSymbol );
                break;
            case ZTP_HOSTVAR_DESC_PERIOD:
            case ZTP_HOSTVAR_DESC_PTR:
                *aIsSimpleVar = STL_FALSE;
                ztpcFindMemberNode( sCurrHostVarNode->mValue,
                                    sSymbol,
                                    &sMemberNode );

                STL_TRY_THROW( sMemberNode != NULL, ERR_MEMBER_NOT_FOUND );

                ztpcSymbol2HostVar( aCVariable, sMemberNode );

                break;
            case ZTP_HOSTVAR_DESC_ARRAY:
                *aIsSimpleVar = STL_FALSE;
                aCVariable->mAddrDepth --;
                break;
            default:
                break;
        }

        sCurrHostVarNode = sCurrHostVarNode->mNext;
    }

    (void)gRefineDTHostVarFunc[aCVariable->mDataType]( aCVariable );
    if( aCVariable->mAddrDepth != 1 )
    {
        aCVariable->mIsArray = STL_FALSE;
        aCVariable->mArrayValue = NULL;
    }

    return STL_SUCCESS;

    STL_CATCH(ERR_SYMBOL_NOT_FOUND)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTP_ERRCODE_NOT_DECLARED_HOST_VARIABLE,
                      NULL,
                      aErrorStack,
                      sCurrHostVarNode->mValue );
    }
    STL_CATCH(ERR_MEMBER_NOT_FOUND)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTP_ERRCODE_MEMBER_NOT_EXIST,
                      NULL,
                      aErrorStack,
                      sSymbol->mName,
                      sCurrHostVarNode->mValue );
    }
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztpcSpreadStructMember( ztpCVariable     *aCVariable,
                                  ztpCVariable    **aHostVariableList,
                                  stlBool           aIn,
                                  stlInt32         *aMemberCount,
                                  stlAllocator     *aAllocator,
                                  stlErrorStack    *aErrorStack )
{
    ztpCVariable   *sMemberList = NULL;
    ztpCVariable   *sNewMember;
    ztpCVariable   *sNewList = NULL;
    ztpCVariable   *sNewListLast = NULL;
    stlInt32        sStructNameLen;
    stlInt32        sMemberNameLen;

    *aMemberCount = 0;

    sStructNameLen = stlStrlen( aCVariable->mName );
    if( aCVariable->mIsArray == STL_TRUE )
    {
        /*
         * length 3 means "[0]"
         */
        sStructNameLen += 3;
    }

    sMemberList = aCVariable->mStructMember;
    while( sMemberList != NULL )
    {
        STL_TRY( stlAllocRegionMem( aAllocator,
                                    STL_SIZEOF(ztpCVariable),
                                    (void**)&sNewMember,
                                    aErrorStack )
                 == STL_SUCCESS );
        ZTP_INIT_C_VARIABLE( sNewMember );

        ztpcSymbol2HostVar( sNewMember, sMemberList );

        (void)gRefineDTHostVarFunc[sNewMember->mDataType]( sNewMember );

        sMemberNameLen = stlStrlen( sMemberList->mName );

        STL_TRY( stlAllocRegionMem( aAllocator,
                                    sStructNameLen + sMemberNameLen + 3,
                                    (void**)&(sNewMember->mName),
                                    aErrorStack )
                 == STL_SUCCESS );

        if( aCVariable->mIsArray == STL_TRUE )
        {
            stlSnprintf( sNewMember->mName, sStructNameLen + sMemberNameLen + 2,
                         "%s[0].%s", aCVariable->mName, sMemberList->mName );
            sNewMember->mIsArray = STL_TRUE;
            sNewMember->mArrayValue = aCVariable->mArrayValue;
            sNewMember->mParentStructName = aCVariable->mName;
        }
        else if( aCVariable->mAddrDepth >= 1 )
        {
            stlSnprintf( sNewMember->mName, sStructNameLen + sMemberNameLen + 3,
                         "%s->%s", aCVariable->mName, sMemberList->mName );
        }
        else
        {
            stlSnprintf( sNewMember->mName, sStructNameLen + sMemberNameLen + 2,
                         "%s.%s", aCVariable->mName, sMemberList->mName );
        }

        if( aIn == STL_TRUE )
        {
            sNewMember->mParamIOType = ZLPL_PARAM_IO_TYPE_IN;
        }
        else
        {
            sNewMember->mParamIOType = ZLPL_PARAM_IO_TYPE_OUT;
        }

        sNewMember->mIsStructMember = STL_TRUE;
        sNewMember->mNext = NULL;
        sNewMember->mIndicator = NULL;
        sNewMember->mStructMember= NULL;

        if( sNewList == NULL )
        {
            sNewList = sNewMember;
            sNewListLast = sNewMember;
        }
        else
        {
            sNewListLast->mNext = sNewMember;
            sNewListLast = sNewMember;
        }

        (*aMemberCount) ++;
        sMemberList = sMemberList->mNext;
    }

    *aHostVariableList = sNewList;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztpcProcessCDirectDeclarator( ztpCVariable         *aCVariable,
                                        zlplCDataType         aCDataType,
                                        ztpCDirectDeclarator *aCDirectDeclarator,
                                        stlAllocator         *aAllocator,
                                        stlErrorStack        *aErrorStack )
{
    ztpCDeclarator    *sCDeclarator = NULL;
    /*
    ztpCParameterList *sCParameterList = NULL;
    */

    if( aCDirectDeclarator->mCDirectDeclarator != NULL )
    {
        STL_TRY( ztpcProcessCDirectDeclarator( aCVariable,
                                               aCDataType,
                                               aCDirectDeclarator->mCDirectDeclarator,
                                               aAllocator,
                                               aErrorStack )
                 == STL_SUCCESS);
    }

    if( aCDirectDeclarator->mCIdentifier != NULL )
    {
        aCVariable->mName = aCDirectDeclarator->mCIdentifier->mName;
    }
    else if( aCDirectDeclarator->mArrayValue != NULL )
    {
        aCVariable->mAddrDepth ++;

        if( aCVariable->mArrayValue == NULL )
        {
            aCVariable->mArrayValue = aCDirectDeclarator->mArrayValue;
        }
        else if( aCVariable->mCharLength == NULL )
        {
            aCVariable->mCharLength = aCDirectDeclarator->mArrayValue;
        }
        else
        {
            aCVariable->mIsAvailHostVar = STL_FALSE;
        }
    }
    else if( aCDirectDeclarator->mCDeclarator != NULL )
    {
        sCDeclarator = aCDirectDeclarator->mCDeclarator;
        STL_TRY( ztpcProcessCDirectDeclarator( aCVariable,
                                               aCDataType,
                                               sCDeclarator->mCDirectDeclarator,
                                               aAllocator,
                                               aErrorStack )
                 == STL_SUCCESS );
    }
    /*
    else if( aCDirectDeclarator->mCParameterList != NULL)
    {
        sCParameterList = aCDirectDeclarator->mCParameterList;

        while( sCParameterList != NULL )
        {
            STL_TRY( ztpcProcessCParameterDeclaration(
                         sCParameterList->mCParameterDeclaration,
                         aAllocator,
                         aErrorStack )
                     == STL_SUCCESS );

            sCParameterList = sCParameterList->mNext;
        }
    }
    */

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*
stlStatus
ztpcProcessCParameterDeclaration( ztpCParameterDeclaration  *aCParamDecl,
                                  stlAllocator              *aAllocator,
                                  stlErrorStack             *aErrorStack )
{
    ztpCVariable        *sCVariable;
    zlplCDataType        sCDataType;

    sCDataType = ztpdGetCDataType( aCParamDecl->mDeclarationSpecifiers );
    sCDataType = aCParamDecl->mCDataType;
    STL_TRY( ztpcMakeCVariableFromCDeclarator( aCParamDecl->mCDeclarator,
                                               &sCVariable,
                                               aCParamDecl->mCDataType,
                                               aAllocator,
                                               aErrorStack )
             == STL_SUCCESS );

    (void)gRefineDTFunc[sCDataType](sCVariable,
                                    NULL);

    STL_TRY( ztpcAddSymbol( sCVariable,
                            STL_FALSE )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
*/

stlStatus
ztpcMakeCVariableFromCDeclarator( ztpCDeclarator   *aCDeclarator,
                                  ztpCVariable    **aCVariable,
                                  zlplCDataType     aCDataType,
                                  stlAllocator     *aAllocator,
                                  stlErrorStack    *aErrorStack )
{
    ztpCVariable            *sCVariable           = NULL;
    ztpCDirectDeclarator    *sCDirectDeclarator   = NULL;

    STL_TRY( stlAllocRegionMem( aAllocator,
                                STL_SIZEOF(ztpCVariable),
                                (void**)&sCVariable,
                                aErrorStack )
             == STL_SUCCESS );
    ZTP_INIT_C_VARIABLE(sCVariable);

    sCVariable->mIsAvailHostVar = STL_TRUE;
    sCVariable->mAddrDepth      = aCDeclarator->mAddrDepth;
    sCVariable->mDataType       = aCDataType;

    sCDirectDeclarator = aCDeclarator->mCDirectDeclarator;
    if( sCDirectDeclarator != NULL )
    {
        STL_TRY( ztpcProcessCDirectDeclarator( sCVariable,
                                               aCDataType,
                                               sCDirectDeclarator,
                                               aAllocator,
                                               aErrorStack )
                 == STL_SUCCESS );
    }

    *aCVariable = sCVariable;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
