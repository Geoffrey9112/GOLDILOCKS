/*******************************************************************************
 * ztpcHostVar.h
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


#ifndef _ZTPCHOSTVAR_H_
#define _ZTPCHOSTVAR_H_ 1

#include <dtl.h>
#include <zlplDef.h>

/**
 * @file ztpcHostVar.h
 * @brief Gliese Embedded SQL in C precompiler common functions on Host variable
 */

/**
 * @defgroup ztpcHostVar Gliese Embedded SQL in C precompiler common functions on Host variable
 * @ingroup ztp
 * @{
 */


/************************************************************************
 * Host Variable
 ************************************************************************/

stlStatus
ztpcAddSymbol( ztpCVariable *aCVariable,
               stlBool       aIsTypedef );

stlStatus
ztpcAddCVariableToSymTab(ztpSymTabHeader  *aSymTab,
                         ztpCVariable     *aCVariableList);

stlStatus
ztpcFindCVariableInSymTab(stlChar         *aSymName,
                          ztpCVariable   **aCVariable);

stlStatus
ztpcFindMemberNode( stlChar        *aSymbolName,
                    ztpCVariable   *aListHead,
                    ztpCVariable  **aNode );

void
ztpcSymbol2HostVar( ztpCVariable  *aDest,
                    ztpCVariable  *aSrc );

stlStatus
ztpcHostVar2CVariable( ztpCVariable     *aCVariable,
                       ztpHostVariable  *aHostVariableDesc,
                       stlBool          *aIsSimpleVar,
                       stlErrorStack    *aErrorStack );

stlStatus
ztpcSpreadStructMember( ztpCVariable     *aCVariable,
                        ztpCVariable    **aHostVariableList,
                        stlBool           aIn,
                        stlInt32         *aMemberCount,
                        stlAllocator     *aAllocator,
                        stlErrorStack    *aErrorStack );

stlStatus
ztpcProcessCDirectDeclarator( ztpCVariable         *aCVariable,
                              zlplCDataType         aCDataType,
                              ztpCDirectDeclarator *aCDirectDeclarator,
                              stlAllocator         *aAllocator,
                              stlErrorStack        *aErrorStack );

stlStatus
ztpcProcessCParameterDeclaration( ztpCParameterDeclaration  *aCParamDecl,
                                  stlAllocator              *aAllocator,
                                  stlErrorStack             *aErrorStack );

stlStatus
ztpcMakeCVariableFromCDeclarator( ztpCDeclarator   *aCDeclarator,
                                  ztpCVariable    **aCVariable,
                                  zlplCDataType     aCDataType,
                                  stlAllocator     *aAllocator,
                                  stlErrorStack    *aErrorStack );

/** @} */

#endif /* _ZTPCHOSTVAR_H_ */
