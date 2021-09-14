/*******************************************************************************
 * ztpCParseFunc.c
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
#include <sqlext.h>
#include <ztpDef.h>
#include <ztpMisc.h>
#include <ztpCParseFunc.h>
#include <ztpcHostVar.h>
#include <ztpcTypeDef.h>
#include <ztpdConvert.h>
#include <ztpdRefine.h>
#include <ztpfPathControl.h>
#include <ztpfPathFunc.h>
#include <ztpfIncludeFileFunc.h>
#include <ztppExpandFile.h>
#include <ztpuFile.h>
#include <ztpuString.h>
#include <ztpvCVarFunc.h>
#include <ztpwPrecomp.h>

/**
 * @file ztpCParseFunc.c
 * @brief Parser Routines
 */

/**
 * @addtogroup ztpCParseFunc
 * @{
 */

YYSTYPE ztpMakeExecSqlStmtToC(ztpCParseParam   *aParam,
                              stlInt32          aStartPos,
                              stlInt32         *aEndPos)
{
    YYSTYPE        sSqlParseTree = NULL;

    sSqlParseTree = ztpMakeSqlStmt(aParam, aStartPos, aEndPos);

    STL_TRY(aParam->mErrorStatus == STL_SUCCESS);

    return sSqlParseTree;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return (YYSTYPE)NULL;
}

stlChar *ztpMakeStaticConnStringInC( ztpCParseParam  *aParam,
                                     stlChar         *aConnName )
{
    stlChar  *sConnName = NULL;

    sConnName = ztpuMakeStaticConnString( aParam->mAllocator,
                                          aParam->mErrorStack,
                                          aConnName );

    STL_TRY( sConnName != NULL );

    return sConnName;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return NULL;
}

stlStatus
ztpProcessCDeclaration( ztpCParseParam   *aParam,
                        ztpCDeclaration  *aCDeclaration,
                        stlBool           aIsParameter )
{
    ztpCDeclaratorList      *sCDeclaratorList     = NULL;
    zlplCDataType            sDataType            = ZLPL_C_DATATYPE_UNKNOWN;
    ztpCVariable            *sCVariable           = NULL;
    ztpCVariable            *sCVariableList       = NULL;
    ztpCVariable            *sCVariableListHead   = NULL;
    ztpCDeclarator          *sCDeclarator         = NULL;

    STL_TRY_THROW( aCDeclaration != NULL, RAMP_EXIT_FUNC );

    sCDeclaratorList = aCDeclaration->mDeclaratorList;
    sDataType        = aCDeclaration->mDataType;

    if( aIsParameter == STL_FALSE )
    {
        aCDeclaration->mIndent = 0;
        while(aParam->mBuffer[aCDeclaration->mStartPos - aCDeclaration->mIndent] != '\n')
        {
            aCDeclaration->mIndent ++;
        }
    }

    if(sCDeclaratorList != NULL)
    {
        while(sCDeclaratorList != NULL)
        {
            sCDeclarator = sCDeclaratorList->mCDeclarator;

            STL_TRY( ztpcMakeCVariableFromCDeclarator( sCDeclarator,
                                                       &sCVariable,
                                                       sDataType,
                                                       aParam->mAllocator,
                                                       aParam->mErrorStack )
                     == STL_SUCCESS );

            if( sCVariableList == NULL )
            {
                sCVariableListHead = sCVariable;
                sCVariableList = sCVariable;
            }
            else
            {
                sCVariableList->mNext = sCVariable;
                sCVariableList = sCVariable;
            }

            /*
             * Parsing된 C declarator로부터 생성된 C Variable이
             * 주어진 Datatype 에 따라서 유효 여부를 검사
             * Host 변수로 적합하면, sCVariable->mIsAvailHostVar == true
             * 적합하지 않으면, sCVariable->mIsAvailHostVar == false
             */
            (void)gRefineDTFunc[sDataType](sCVariable, aCDeclaration);

            sCDeclaratorList = sCDeclaratorList->mNext;
        }
    }

    if( aIsParameter == STL_FALSE )
    {
        STL_TRY(gCvtDTFunc[sDataType](aParam,
                                      aCDeclaration,
                                      sCVariableListHead)
                == STL_SUCCESS);
    }

    STL_TRY( ztpcAddSymbol( sCVariableListHead,
                            aParam->mIsTypedef )
             == STL_SUCCESS );

    STL_RAMP( RAMP_EXIT_FUNC );

    STL_TRY(aParam->mErrorStatus == STL_SUCCESS);

    return STL_SUCCESS;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return STL_FAILURE;
}

ztpCDeclaration *
ztpMakeCDeclaration( ztpCParseParam         *aParam,
                     stlInt32                aStartPos,
                     stlInt32                aEndPos,
                     stlInt32                aLineNo,
                     ztpCVariable           *aCVariable,
                     zlplCDataType           aCDataType,
                     stlChar                *aPrecision,
                     stlChar                *aScale,
                     ztpCDeclaratorList     *aList )
{
    ztpCDeclaration    *sCDeclaration = NULL;

    STL_TRY( stlAllocRegionMem( aParam->mAllocator,
                                STL_SIZEOF(ztpCDeclaration),
                                (void**)&sCDeclaration,
                                aParam->mErrorStack )
             == STL_SUCCESS );

    sCDeclaration->mStartPos              = aStartPos;
    sCDeclaration->mEndPos                = aEndPos;
    sCDeclaration->mLineNo                = aLineNo;
    sCDeclaration->mCVariable             = aCVariable;
    sCDeclaration->mDataType              = aCDataType;
    sCDeclaration->mPrecision             = aPrecision;
    sCDeclaration->mScale                 = aScale;
    sCDeclaration->mDeclaratorList        = aList;

    return sCDeclaration;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return NULL;
}

ztpCDeclaratorList *
ztpMakeCDeclaratorList( ztpCParseParam       *aParam,
                        ztpCDeclarator       *aCDeclarator,
                        ztpCDeclaratorList   *aCDeclaratorList )
{
    ztpCDeclaratorList *sCDeclaratorList = NULL;

    STL_TRY( stlAllocRegionMem( aParam->mAllocator,
                                STL_SIZEOF(ztpCDeclaratorList),
                                (void**)&sCDeclaratorList,
                                aParam->mErrorStack )
             == STL_SUCCESS );

    sCDeclaratorList->mCDeclarator = aCDeclarator;
    sCDeclaratorList->mNext        = aCDeclaratorList;

    return sCDeclaratorList;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return NULL;
}

ztpCDeclarationList *
ztpMakeCDeclarationList( ztpCParseParam       *aParam,
                         ztpCDeclaration      *aCDeclaration,
                         ztpCDeclarationList  *aCDeclarationList )
{
    ztpCDeclarationList *sCDeclarationList = NULL;

    STL_TRY( stlAllocRegionMem( aParam->mAllocator,
                                STL_SIZEOF(ztpCDeclarationList),
                                (void**)&sCDeclarationList,
                                aParam->mErrorStack )
             == STL_SUCCESS );

    sCDeclarationList->mCDeclaration = aCDeclaration;
    sCDeclarationList->mNext         = aCDeclarationList;

    return sCDeclarationList;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return NULL;
}

/*
ztpCParameterDeclaration *
ztpMakeCParamDecl( ztpCParseParam   *aParam,
                   stlInt64          aDeclSpec,
                   ztpCDeclarator   *aCDeclarator )
{
    ztpCParameterDeclaration  *sCParameterDeclaration;

    STL_TRY( stlAllocRegionMem( aParam->mAllocator,
                                STL_SIZEOF(ztpCParameterDeclaration),
                                (void**)&sCParameterDeclaration,
                                aParam->mErrorStack )
             == STL_SUCCESS );

    sCParameterDeclaration->mDeclarationSpecifiers = aDeclSpec;
    sCParameterDeclaration->mCDeclarator           = aCDeclarator;

    return sCParameterDeclaration;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return NULL;
}

ztpCParameterList *ztpMakeCParameterList( ztpCParseParam           *aParam,
                                          ztpCParameterList        *aCParameterList,
                                          ztpCParameterDeclaration *aCParamDecl )
{
    ztpCParameterList  *sCParameterList = NULL;

    STL_TRY_THROW( aCParamDecl != NULL, RAMP_EXIT_FUNC );

    STL_TRY( stlAllocRegionMem( aParam->mAllocator,
                                STL_SIZEOF(ztpCParameterList),
                                (void**)&sCParameterList,
                                aParam->mErrorStack )
             == STL_SUCCESS );

    sCParameterList->mCParameterDeclaration = aCParamDecl;
    sCParameterList->mNext                  = NULL;

    if( aCParameterList != NULL )
    {
        aCParameterList->mNext              = sCParameterList;
        sCParameterList                     = aCParameterList;
    }

    STL_RAMP( RAMP_EXIT_FUNC );

    return sCParameterList;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return NULL;
}
*/

ztpCIdentifier *ztpMakeCIdentifier(ztpCParseParam   *aParam,
                                   stlChar          *aIdentifier)
{
    ztpCIdentifier  *sIdentifier;

    STL_TRY( stlAllocRegionMem( aParam->mAllocator,
                                STL_SIZEOF(ztpCIdentifier),
                                (void**)&sIdentifier,
                                aParam->mErrorStack )
             == STL_SUCCESS );

    sIdentifier->mLength = stlStrlen(aIdentifier);
    STL_TRY( stlAllocRegionMem( aParam->mAllocator,
                                sIdentifier->mLength + 1,
                                (void**)&sIdentifier->mName,
                                aParam->mErrorStack )
                  == STL_SUCCESS );

    stlStrncpy(sIdentifier->mName, aIdentifier, sIdentifier->mLength + 1);

    return sIdentifier;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return NULL;
}

/*
ztpCIdentifierList *ztpMakeCIdentifierList(ztpCParseParam      *aParam,
                                           ztpCIdentifierList  *aCIdentifierList,
                                           ztpCIdentifier      *aCIdentifier)
{
    ztpCIdentifierList  *sIdentifierList;

    STL_TRY( stlAllocRegionMem(aParam->mAllocator,
                               STL_SIZEOF(ztpCIdentifierList),
                               (void**)&sIdentifierList,
                               aParam->mErrorStack )
             == STL_SUCCESS );

    sIdentifierList->mCIdentifier = aCIdentifier;
    sIdentifierList->mNext = aCIdentifierList;

    return sIdentifierList;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return NULL;
}
*/

ztpTypeSpecifierNode *ztpMakeTypeSpecifierNode(ztpCParseParam   *aParam,
                                               ztpTypeSpecifier  aTypeSpecifier,
                                               stlChar          *aPrecision,
                                               stlChar          *aScale,
                                               ztpCVariable     *aCVariable,
                                               stlInt32          aPos,
                                               stlInt32          aLen)
{
    ztpTypeSpecifierNode *sTypeSpecifierNode;

    STL_TRY( stlAllocRegionMem( aParam->mAllocator,
                                STL_SIZEOF(ztpTypeSpecifierNode),
                                (void**)&sTypeSpecifierNode,
                                aParam->mErrorStack )
             == STL_SUCCESS );
    sTypeSpecifierNode->mTypeSpecifier = aTypeSpecifier;
    sTypeSpecifierNode->mPrecision     = aPrecision;
    sTypeSpecifierNode->mScale         = aScale;
    sTypeSpecifierNode->mCVariable     = aCVariable;
    sTypeSpecifierNode->mPosition      = aPos;
    sTypeSpecifierNode->mLength        = aLen;

    return sTypeSpecifierNode;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return NULL;
}

ztpTypeSpecifierList *ztpMakeTypeSpecifierList(ztpCParseParam        *aParam,
                                               ztpTypeSpecifierList  *aTypeSpecifierList,
                                               ztpTypeSpecifierNode  *aTypeSpecifierNode)
{
    ztpTypeSpecifierList  *sTypeSpecifierList;

    STL_TRY( stlAllocRegionMem(aParam->mAllocator,
                               STL_SIZEOF(ztpTypeSpecifierList),
                               (void**)&sTypeSpecifierList,
                               aParam->mErrorStack )
             == STL_SUCCESS );

    sTypeSpecifierList->mTypeSpecifierNode = aTypeSpecifierNode;
    sTypeSpecifierList->mNext = aTypeSpecifierList;

    return sTypeSpecifierList;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return NULL;
}

stlChar *ztpMakeCAssignmentExpression( ztpCParseParam        *aParam,
                                       stlInt32               aStartPos,
                                       stlInt32               aEndPos )
{
    stlChar    *sResultStr;
    stlInt32    sLength;

    sLength = aEndPos - aStartPos;

    STL_TRY( stlAllocRegionMem(aParam->mAllocator,
                               sLength + 1,
                               (void**)&sResultStr,
                               aParam->mErrorStack )
             == STL_SUCCESS );

    stlStrncpy( sResultStr, aParam->mBuffer + aStartPos, sLength + 1);

    return sResultStr;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return NULL;
}

ztpCDirectDeclarator *ztpMakeCDirectDeclarator(ztpCParseParam       *aParam,
                                               ztpCIdentifier       *aCIdentifier,
                                               ztpCDeclarator       *aCDeclarator,
                                               ztpCDirectDeclarator *aCDirectDeclarator,
                                               stlChar              *aArrayValue,
                                               ztpCIdentifierList   *aCIdentifierList,
                                               ztpCParameterList    *aCParameterList )
{
    ztpCDirectDeclarator *sCDirectDeclarator;

    STL_TRY( stlAllocRegionMem( aParam->mAllocator,
                                STL_SIZEOF(ztpCDirectDeclarator),
                                (void**)&sCDirectDeclarator,
                                aParam->mErrorStack )
             == STL_SUCCESS );

    sCDirectDeclarator->mCIdentifier       = aCIdentifier;
    sCDirectDeclarator->mCDeclarator       = aCDeclarator;
    sCDirectDeclarator->mCDirectDeclarator = aCDirectDeclarator;
    sCDirectDeclarator->mArrayValue        = aArrayValue;
    sCDirectDeclarator->mCIdentifierList   = aCIdentifierList;
    sCDirectDeclarator->mCParameterList    = aCParameterList;

    return sCDirectDeclarator;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return NULL;
}

ztpCDeclarator *ztpMakeCDeclarator(ztpCParseParam       *aParam,
                                   ztpCDirectDeclarator *aDirectDeclarator,
                                   stlBool               aUsePointer)
{
    ztpCDeclarator *sDeclarator = NULL;

    STL_TRY_THROW( aDirectDeclarator != NULL, EXIT_FUNC );

    if( aParam->mCDeclarator == NULL )
    {
        STL_TRY(stlAllocRegionMem(aParam->mAllocator,
                                  STL_SIZEOF(ztpCDeclarator),
                                  (void**)&sDeclarator,
                                  aParam->mErrorStack)
                == STL_SUCCESS);
        sDeclarator->mCDirectDeclarator = NULL;
        sDeclarator->mAddrDepth = 0;
        aParam->mCDeclarator = sDeclarator;
    }
    else
    {
        sDeclarator = aParam->mCDeclarator;
    }

    sDeclarator->mCDirectDeclarator = aDirectDeclarator;
    if( aUsePointer == STL_TRUE )
    {
        sDeclarator->mAddrDepth ++;
    }

    STL_RAMP(EXIT_FUNC);

    return sDeclarator;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return NULL;
}

ztpCStructSpecifier *
ztpMakeCStructSpecifier( ztpCParseParam       *aParam,
                         ztpCIdentifier       *aTagName,
                         ztpCDeclarationList  *aList )
{
    ztpCStructSpecifier *sCStructSpecifier = NULL;

    STL_TRY( stlAllocRegionMem( aParam->mAllocator,
                                STL_SIZEOF(ztpCStructSpecifier),
                                (void**)&sCStructSpecifier,
                                aParam->mErrorStack )
             == STL_SUCCESS );

    sCStructSpecifier->mTagName           = aTagName;
    sCStructSpecifier->mCDeclarationList  = aList;

    return sCStructSpecifier;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return NULL;
}

ztpCVariable *
ztpProcessCStructSpecifier( ztpCParseParam      *aParam,
                            stlBool              aIsTypedef,
                            ztpCStructSpecifier *aCStructSpecifier )
{
    ztpCIdentifier       *sTagName = NULL;
    ztpCDeclarationList  *sCDeclarationList = NULL;
    ztpCDeclaration      *sCDeclaration = NULL;
    ztpCDeclaratorList   *sCDeclaratorList = NULL;
    ztpCDeclarator       *sCDeclarator = NULL;
    ztpCVariable         *sCVariable = NULL;
    ztpCVariable         *sCVariableMember = NULL;
    ztpCVariable         *sLastMember = NULL;
    stlChar              *sSymbolName = NULL;

    sTagName          = aCStructSpecifier->mTagName;
    sCDeclarationList = aCStructSpecifier->mCDeclarationList;

    if( sTagName != NULL )
    {
        STL_TRY( stlAllocRegionMem( aParam->mAllocator,
                                    sTagName->mLength + 8,
                                    (void**)&sSymbolName,
                                    aParam->mErrorStack )
                 == STL_SUCCESS );

        stlSnprintf( sSymbolName, sTagName->mLength + 8,
                     "struct %s", sTagName->mName );
    }
    else
    {
        sSymbolName = NULL;
    }

    if( sCDeclarationList == NULL )
    {
        ztpcFindTypeDefInSymTab(sSymbolName,
                                &sCVariable);
    }
    else
    {
        STL_TRY( stlAllocRegionMem( aParam->mAllocator,
                                    STL_SIZEOF(ztpCVariable),
                                    (void**)&sCVariable,
                                    aParam->mErrorStack )
                 == STL_SUCCESS );
        ZTP_INIT_C_VARIABLE( sCVariable );

        sCVariable->mName = sSymbolName;
        sCVariable->mDataType = ZLPL_C_DATATYPE_STRUCT;

        while( sCDeclarationList != NULL )
        {
            sCDeclaration = sCDeclarationList->mCDeclaration;
            sCDeclaratorList = sCDeclaration->mDeclaratorList;

            sCDeclaration->mIndent = 0;
            while(aParam->mBuffer[sCDeclaration->mStartPos - sCDeclaration->mIndent] != '\n')
            {
                sCDeclaration->mIndent ++;
            }

            while( sCDeclaratorList != NULL )
            {
                sCDeclarator = sCDeclaratorList->mCDeclarator;

                ztpcMakeCVariableFromCDeclarator( sCDeclarator,
                                                  &sCVariableMember,
                                                  sCDeclaration->mDataType,
                                                  aParam->mAllocator,
                                                  aParam->mErrorStack );

                (void)gRefineDTFunc[sCDeclaration->mDataType]( sCVariableMember,
                                                               sCDeclaration );

                if( sCVariable->mStructMember == NULL )
                {
                    sCVariable->mStructMember = sCVariableMember;
                }
                else
                {
                    sLastMember->mNext = sCVariableMember;
                }
                sLastMember = sCVariableMember;

                STL_TRY(gCvtDTFunc[sCDeclaration->mDataType](aParam,
                                                             sCDeclaration,
                                                             sCVariableMember)
                        == STL_SUCCESS);

                sCDeclaratorList = sCDeclaratorList->mNext;
            }

            sCDeclarationList = sCDeclarationList->mNext;
        }

        if( sCVariable->mName != NULL )
        {
            STL_TRY( ztpcAddSymbol( sCVariable,
                                    STL_TRUE )
                     == STL_SUCCESS );
        }
    }

    return sCVariable;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return NULL;
}

YYSTYPE ztpMakeSqlStmt(ztpCParseParam   *aParam,
                       stlInt32          aStartPos,
                       stlInt32         *aEndPos)
{
    STL_TRY(ztpExtractNParseSql(aParam,
                                aStartPos,
                                aEndPos)
            == STL_SUCCESS);

    return (YYSTYPE)aParam->mParseTree;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return (YYSTYPE)NULL;
}

YYSTYPE ztpMakeExecSqlInclude(ztpCParseParam   *aParam,
                              stlChar          *aFileName)
{
    ztpExecSqlInclude    *sExecSqlInclude;
    stlInt32              sFileNameLen;
    stlChar              *sFileName;

    STL_TRY( stlAllocRegionMem( aParam->mAllocator,
                                STL_SIZEOF(ztpExecSqlInclude),
                                (void**)&sExecSqlInclude,
                                aParam->mErrorStack )
             == STL_SUCCESS );

    sExecSqlInclude->mParseTreeType = ZTP_PARSETREE_TYPE_EXEC_SQL_INCLUDE;

    sFileName = aFileName;
    sFileNameLen = stlStrlen(aFileName);
    while( stlIsspace( *sFileName ) != 0 )
    {
        sFileNameLen --;
        sFileName ++;
    }

    stlStrncpy(sExecSqlInclude->mFileName,
               sFileName,
               sFileNameLen + 1);

    aParam->mParseTree = (ztpParseTree *)sExecSqlInclude;

    return (YYSTYPE)aParam->mParseTree;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return (YYSTYPE)NULL;
}

stlChar *ztpMakeIncludeFile(ztpCParseParam   *aParam,
                            stlInt32          aStartPos,
                            stlInt32         *aEndPos)
{
    stlChar   *sPathStr;
    stlInt32   sPathLen;

    STL_TRY( ztpfIncludeFileParseIt(aParam->mAllocator,
                                    aParam->mErrorStack,
                                    aParam->mBuffer + aStartPos,
                                    &sPathLen,
                                    &sPathStr )
             == STL_SUCCESS );

    STL_TRY(aParam->mErrorStatus == STL_SUCCESS);

    *aEndPos = aStartPos + sPathLen;

    return sPathStr;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return NULL;
}

ztpIncludePath *ztpMakePathList(ztpCParseParam   *aParam,
                                stlInt32          aStartPos,
                                stlInt32         *aEndPos)
{
    stlInt32         sPathLen;
    ztpIncludePath  *sIncludePath;

    STL_TRY( ztpfPathParseIt(aParam->mAllocator,
                             aParam->mErrorStack,
                             aParam->mBuffer + aStartPos,
                             &sPathLen,
                             &sIncludePath )
             == STL_SUCCESS );

    STL_TRY(aParam->mErrorStatus == STL_SUCCESS);

    *aEndPos = aStartPos + sPathLen;

    return sIncludePath;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return NULL;
}

YYSTYPE ztpMakeExecSqlException(ztpCParseParam        *aParam,
                                ztpExceptionCondition *aExceptionCondition,
                                ztpExceptionAction    *aExceptionAction)
{
    ztpExecSqlException    *sExecSqlException;

    STL_TRY(stlAllocRegionMem(aParam->mAllocator,
                              STL_SIZEOF(ztpExecSqlException),
                              (void**)&sExecSqlException,
                              aParam->mErrorStack)
            == STL_SUCCESS);

    sExecSqlException->mParseTreeType = ZTP_PARSETREE_TYPE_SQL_EXCEPTION;
    sExecSqlException->mCondition = aExceptionCondition;
    sExecSqlException->mAction = aExceptionAction;

    aParam->mParseTree = (ztpParseTree *)sExecSqlException;

    return (YYSTYPE)aParam->mParseTree;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return (YYSTYPE)NULL;
}

YYSTYPE ztpMakeExceptionCondition(ztpCParseParam            *aParam,
                                  ztpExceptionConditionType  aType,
                                  stlChar                   *aValue)
{
    stlInt32                 sSqlStateLen;
    ztpExceptionCondition   *sExceptionCondition;

    STL_TRY(stlAllocRegionMem(aParam->mAllocator,
                              STL_SIZEOF(ztpExceptionCondition),
                              (void**)&sExceptionCondition,
                              aParam->mErrorStack)
            == STL_SUCCESS);

    sExceptionCondition->mType = aType;

    if(aType == ZTP_EXCEPTION_COND_SQLSTATE)
    {
        sSqlStateLen = stlStrlen(aValue);
        STL_TRY((sSqlStateLen == 2) || (sSqlStateLen == 5));
        stlStrncpy(sExceptionCondition->mValue, aValue, sSqlStateLen + 1);
    }
    else
    {
        sExceptionCondition->mValue[0] ='\0';
    }

    return (YYSTYPE)sExceptionCondition;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return (YYSTYPE)NULL;
}

YYSTYPE ztpMakeExceptionAction(ztpCParseParam         *aParam,
                               ztpExceptionActionType  aType,
                               stlChar                *aValue)
{
    ztpExceptionAction   *sExceptionAction;

    STL_TRY(stlAllocRegionMem(aParam->mAllocator,
                              STL_SIZEOF(ztpExceptionAction),
                              (void**)&sExceptionAction,
                              aParam->mErrorStack)
            == STL_SUCCESS);

    sExceptionAction->mType = aType;
    sExceptionAction->mValue = aValue;

    return (YYSTYPE)sExceptionAction;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return (YYSTYPE)NULL;
}

ztpHostVariable *ztpMakeHostVariableInC( ztpCParseParam  *aParam,
                                         stlInt32         aStartPos,
                                         stlInt32        *aEndPos )
{
    ztpHostVariable *sHostVariable = NULL;
    stlErrorData    *sErrorData = NULL;
    stlChar          sErrorMsg[STL_MAX_ERROR_MESSAGE + 2];

    sHostVariable =  ztpvMakeHostVariable( aParam->mAllocator,
                                           aParam->mErrorStack,
                                           aParam->mBuffer,
                                           aStartPos,
                                           aEndPos );
    STL_TRY_THROW( sHostVariable != NULL, RAMP_ERR_SYMBOL );

    return (YYSTYPE)sHostVariable;

    STL_CATCH(RAMP_ERR_SYMBOL)
    {
        ztpCerror( NULL, aParam, aParam->mYyScanner, NULL );
        sErrorData = stlPopError( aParam->mErrorStack );

        /*
         * 반드시 ErrorStack이 비어있지 않아야 한다.
         */
        STL_DASSERT( sErrorData != NULL );

        stlSnprintf( sErrorMsg,
                     STL_MAX_ERROR_MESSAGE + 2,
                     ", %s",
                     sErrorData->mErrorMessage );
        (void)ztpuAppendErrorMsg( aParam->mAllocator,
                                  aParam->mErrorStack,
                                  &aParam->mErrorMsg,
                                  sErrorMsg );
    }
    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return (YYSTYPE)NULL;
}

YYSTYPE ztpMakeSqlContextAction(ztpCParseParam         *aParam,
                                ztpParseTreeType        aParseTreeType,
                                ztpContextActionType    aContextActionType,
                                stlChar                *aName)
{
    ztpSqlContext   *sSqlContext;

    STL_TRY(stlAllocRegionMem(aParam->mAllocator,
                              STL_SIZEOF(ztpSqlContext),
                              (void**)&sSqlContext,
                              aParam->mErrorStack)
            == STL_SUCCESS);

    sSqlContext->mParseTreeType = aParseTreeType;
    sSqlContext->mContextActionType = aContextActionType;
    stlStrncpy(sSqlContext->mName, aName, STL_MAX_SQL_IDENTIFIER_LENGTH);

    aParam->mParseTree = (ztpParseTree *)sSqlContext;

    return (YYSTYPE)sSqlContext;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return (YYSTYPE)NULL;
}

ztpAutoCommit *ztpMakeAutoCommit(ztpCParseParam *aParam,
                                 stlBool         aIsOn )
{
    ztpAutoCommit   *sAutoCommit = NULL;

    STL_TRY(stlAllocRegionMem(aParam->mAllocator,
                              STL_SIZEOF(ztpAutoCommit),
                              (void**)&sAutoCommit,
                              aParam->mErrorStack)
            == STL_SUCCESS);

    sAutoCommit->mParseTreeType = ZTP_PARSETREE_TYPE_SET_AUTOCOMMIT;
    sAutoCommit->mAutocommitOn = aIsOn;

    aParam->mParseTree = (ztpParseTree *)sAutoCommit;

    return sAutoCommit;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return NULL;
}

YYSTYPE ztpIncSymTabDepth(ztpCParseParam   *aParam)
{
    /*
     * Symbol Table의 depth를 1 증가시킨다. Symbol Table의 depth
     * 증가는 C 코드에서 { 가 시작될때이며, } 일 경우에는 Depth를
     * 감소시키게 된다. Symbol Table에서 이렇게 Depth를 관리하는 이유는
     * { }의 중첩 경우에 따라, Variable의 scope가 달라지기 때문이다.
     */
    ztpSymTabHeader   *sSymTabHeader;
    ztpSymTabHeader   *sPrevSymTabHeader;

    if(gCurrSymTab->mNextDepth == NULL)
    {
        /* Next depth의 Symbol Table이 없는 경우 */
        STL_TRY(stlAllocRegionMem(aParam->mAllocator,
                                  STL_SIZEOF(ztpSymTabHeader),
                                  (void**)&sSymTabHeader,
                                  aParam->mErrorStack)
                == STL_SUCCESS);

        sPrevSymTabHeader = gCurrSymTab;

        sSymTabHeader->mDepth = sPrevSymTabHeader->mDepth + 1;
        sSymTabHeader->mCVariableHead   = NULL;
        sSymTabHeader->mCursorListHead  = NULL;
        sSymTabHeader->mTypedefListHead = NULL;
        sSymTabHeader->mPrevDepth = sPrevSymTabHeader;
        sSymTabHeader->mNextDepth = NULL;

        sPrevSymTabHeader->mNextDepth = sSymTabHeader;

        gCurrSymTab = sSymTabHeader;
    }
    else
    {
        /* Next depth의 Symbol Table Header가 있는 경우 */
        gCurrSymTab = gCurrSymTab->mNextDepth;
    }

    return (YYSTYPE)NULL;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return (YYSTYPE)NULL;
}

YYSTYPE ztpDecSymTabDepth(ztpCParseParam   *aParam)
{
    ztpSymTabHeader   *sSymTabHeader;

    sSymTabHeader = gCurrSymTab;
    STL_TRY(sSymTabHeader->mDepth > 0);

    /*
     * Depth를 감소시킬때는, 현재 Depth의 Symbol Table이
     * 보유하고 있던 모든 Symbol 정보를 clear 한다.
     */
    sSymTabHeader->mCVariableHead = NULL;
    sSymTabHeader->mCursorListHead  = NULL;
    sSymTabHeader->mTypedefListHead = NULL;
    gCurrSymTab = sSymTabHeader->mPrevDepth;

    return (YYSTYPE)NULL;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return (YYSTYPE)NULL;
}

/*
stlStatus ztpProcessCInclude(ztpCParseParam  *aParam,
                             stlChar         *aFileName)
{
    stlInt32            sState = 0;
    ztpParseTree       *sParseTree;
    ztpConvertContext   sContext;
    stlBool             sFileExist = STL_FALSE;
    stlChar             sFileName[STL_MAX_FILE_NAME_LENGTH];
    stlChar             sFilePath[STL_MAX_FILE_PATH_LENGTH + 1];
    stlChar             sExpandFilePath[STL_MAX_FILE_PATH_LENGTH + 1];
    stlFile             sInFile;

    STL_TRY( ztpfSearchFile(aParam->mInFileName,
                            aFileName,
                            sFileName,
                            sFilePath,
                            &sFileExist,
                            aParam->mErrorStack)
             == STL_SUCCESS );

    sContext.mAllocator = aParam->mAllocator;
    sContext.mErrorStack = aParam->mErrorStack;
    sContext.mBuffer = NULL;
    sContext.mLength = 0;
    sContext.mInFile = NULL;
    sContext.mOutFile = NULL;
    sContext.mInFileName = sFileName;
    sContext.mOutFileName = NULL;
    sContext.mParseTree = NULL;
    sContext.mNeedPrint = STL_FALSE;

    //STL_TRY_THROW( sFileExist == STL_TRUE, RAMP_ERR_FILE_NOT_EXIST );
    STL_TRY_THROW( sFileExist == STL_TRUE, RAMP_EXIT_FUNC );

    STL_TRY(ztppExpandHeaderFile(aParam->mAllocator,
                                 aParam->mErrorStack,
                                 sFilePath,
                                 sExpandFilePath)
            == STL_SUCCESS);

    sContext.mInFileName = sExpandFilePath;

    STL_TRY(stlOpenFile(&sInFile,
                        sExpandFilePath,
                        STL_FOPEN_READ,
                        STL_FPERM_OS_DEFAULT,
                        sContext.mErrorStack)
            == STL_SUCCESS);
    sContext.mInFile = &sInFile;
    sState = 1;

    STL_TRY(ztpuReadFile(sContext.mInFile,
                         sContext.mAllocator,
                         sContext.mErrorStack,
                         &sContext.mBuffer,
                         &sContext.mLength)
            == STL_SUCCESS);

    STL_TRY(ztpCParseIt(&sContext,
                        &sParseTree)
            == STL_SUCCESS);

    STL_RAMP( RAMP_EXIT_FUNC );

    STL_TRY(aParam->mErrorStatus == STL_SUCCESS);

    return STL_SUCCESS;

    / *
    STL_CATCH( RAMP_ERR_FILE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTP_ERRCODE_FILE_NOT_EXIST,
                      NULL,
                      sContext.mErrorStack,
                      sFileName );
    }
    * /
    STL_FINISH;

    switch(sState)
    {
        case 1:
            stlCloseFile(sContext.mInFile,
                         sContext.mErrorStack);
            break;
        default:
            break;
    }

    //stlPrintf( "FilePath : %s\n", sFilePath );

    aParam->mErrorStatus = STL_FAILURE;
    return STL_FAILURE;
}
*/

stlStatus ztpSetSqlPrecompileError( ztpCParseParam  *aParam )
{
    stlChar    sErrLineMsgBuffer[STL_MAX_ERROR_MESSAGE];

    stlSnprintf( sErrLineMsgBuffer,
                 STL_MAX_ERROR_MESSAGE,
                 "\nError at line %d, in file %s",
                 aParam->mLineNo,
                 aParam->mInFileName );

    stlPushError( STL_ERROR_LEVEL_ABORT,
                  ZTP_ERRCODE_SYNTAX,
                  sErrLineMsgBuffer,
                  aParam->mErrorStack );

    stlPrintf( "\n" );
    ztpPrintErrorStack(aParam->mErrorStack);
    stlPrintf( "\n" );

    STL_INIT_ERROR_STACK(aParam->mErrorStack);

    aParam->mErrorStatus = STL_SUCCESS;
    aParam->mHasSQLError = STL_TRUE;

    /**
     * Pre-Compile Error 발생시 Compile Error 가 발생하도록 강제로 에러 코딩 추가
     */

    STL_TRY( ztpwSendStringToOutFile(
                 aParam,
                 "\n\n/* pre-compile error occurred */\n")
             == STL_SUCCESS );
    STL_TRY( ztpwSendStringToOutFile(
                 aParam,
                 "#error PRE-COMPILE ERROR OCCURRED !!!\n\n")
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;

    return STL_FAILURE;
}


/**
 * @note 이름을 변경할수 없다.
 */

stlInt32 ztpCMinerror( YYLTYPE          *aLloc,
                       ztpCParseParam * aParam,
                       void             * aScanner,
                       const stlChar    * aMsg )
{
    return ztpCerror( aLloc,
                      aParam,
                      aScanner,
                      aMsg );
}

stlInt32 ztpCerror( YYLTYPE          *aLloc,
                    ztpCParseParam * aParam,
                    void             * aScanner,
                    const stlChar    * aMsg )
{
    stlInt32   i;
    stlInt32   sCurLineNo = 1;
    stlChar  * sStartPtr = aParam->mBuffer;
    stlChar  * sEndPtr;
    stlChar  * sErrMsg;
    stlChar    sErrLineMsgBuffer[STL_MAX_ERROR_MESSAGE];
    stlInt32   sErrMsgLength;

    /**
     * 에러 메세지의 총길이는 (원본 SQL의 길이 + 에러 위치 메세지 길이 +
     * 에러 라인 메세지 길이)의 총합과 같다.
     * (에러 위치 메시지 길이 + 에러 라인 메세지 길이)를 정확히 예상할수도 있지만
     * 대략 원본 메세지 길이의 4배보다는 작음을 보장할수 있다.
     */
    sErrMsgLength = (aParam->mFileLength + 1) * 4;

    STL_TRY( stlAllocRegionMem( aParam->mAllocator,
                                sErrMsgLength,
                                (void**)&sErrMsg,
                                aParam->mErrorStack )
             == STL_SUCCESS );

    sErrMsg[0] = '\0';

    while( sCurLineNo < aParam->mLineNo )
    {
        if( *sStartPtr == '\n' )
        {
            sCurLineNo++;
        }
        sStartPtr++;
    }
    sEndPtr = sStartPtr;
    while( (*sEndPtr != '\n') && (*sEndPtr != '\0') )
    {
        sEndPtr++;
    }

    /**
     * 원본 SQL 저장
     */
    stlStrnncat( sErrMsg, "\n", sErrMsgLength, STL_UINT32_MAX );
    stlStrnncat( sErrMsg, sStartPtr, sErrMsgLength, sEndPtr - sStartPtr + 1 );
    stlStrnncat( sErrMsg, "\n", sErrMsgLength, STL_UINT32_MAX );

    /**
     * 에러 위치 메세지 저장
     */
    for( i = 1; i < aParam->mColumn; i++ )
    {
        stlStrnncat( sErrMsg, ".", sErrMsgLength, STL_UINT32_MAX );
    }
    stlStrnncat( sErrMsg, "^", sErrMsgLength, STL_UINT32_MAX );
    for( i = 0; i < aParam->mColumnLen - 2; i++ )
    {
        stlStrnncat( sErrMsg, " ", sErrMsgLength, STL_UINT32_MAX );
    }
    if( aParam->mColumnLen >= 2 )
    {
        stlStrnncat( sErrMsg, "^", sErrMsgLength, STL_UINT32_MAX );
    }
    stlStrnncat( sErrMsg, "\n", sErrMsgLength, STL_UINT32_MAX );

    /**
     * 에러 라인 메세지 저장
     */
    stlSnprintf( sErrLineMsgBuffer,
                 STL_MAX_ERROR_MESSAGE,
                 "Error at line %d, in file %s",
                 aParam->mLineNo,
                 aParam->mInFileName );
    stlStrnncat( sErrMsg, sErrLineMsgBuffer, sErrMsgLength, STL_UINT32_MAX );

    aParam->mErrorStatus = STL_FAILURE;
    aParam->mErrorMsg = sErrMsg;

    return 0;

    STL_FINISH;

    return 0;
}

stlStatus ztpCParseIt(ztpConvertContext  *aContext,
                      ztpParseTree      **aParseTree)
{
    stlInt32        i;
    ztpCParseParam  sParam;

    sParam.mNeedPrint          = aContext->mNeedPrint;
    sParam.mBuffer             = aContext->mBuffer;
    sParam.mAllocator          = aContext->mAllocator;
    sParam.mErrorStack         = aContext->mErrorStack;
    sParam.mLength             = aContext->mLength;
    sParam.mFileLength         = aContext->mLength;
    sParam.mPosition           = 0;
    sParam.mLineNo             = 1;
    sParam.mColumn             = 1;
    sParam.mColumnLen          = 0;
    sParam.mCurPosition        = 0;
    sParam.mParseTree          = NULL;
    sParam.mErrorMsg           = NULL;
    sParam.mSQLStartPos        = 0;
    sParam.mHasSQLError        = STL_FALSE;
    sParam.mInnerDeclSec       = STL_FALSE;
    sParam.mIsTypedef          = STL_FALSE;
    sParam.mBypassCToken       = STL_TRUE;
    sParam.mIsAtomic           = STL_FALSE;
    sParam.mErrorStatus        = STL_SUCCESS;
    sParam.mOutFile            = aContext->mOutFile;
    sParam.mInFileName         = aContext->mInFileName;
    sParam.mOutFileName        = aContext->mOutFileName;
    sParam.mCCodeStartLoc      = 0;
    sParam.mCCodeEndLoc        = 0;
    sParam.mCurrLoc            = 0;
    sParam.mNextLoc            = 0;
    sParam.mContext            = NULL;
    sParam.mIterationValue     = NULL;
    sParam.mConnStr            = NULL;

    sParam.mCDeclarator           = NULL;

    stlSnprintf(sParam.mCurrContext, STL_MAX_SQL_IDENTIFIER_LENGTH,
                "NULL");

    ZTP_INIT_TOKEN_STREAM( sParam.mTokenStream );
    STL_TRY(stlAllocRegionMem(sParam.mAllocator,
                              sParam.mFileLength,
                              (void **)&sParam.mTempBuffer,
                              sParam.mErrorStack)
            == STL_SUCCESS);

    for(i = 0; i < ZTP_EXCEPTION_COND_MAX; i ++)
    {
        sParam.mException[i] = NULL;
    }

    ztpCMinlex_init(&sParam.mYyScanner);
    ztpCMinset_extra(&sParam, sParam.mYyScanner);
    ztpCMinparse(&sParam, sParam.mYyScanner);
    ztpCMinlex_destroy(sParam.mYyScanner);

    *aParseTree = sParam.mParseTree;

    STL_TRY_THROW( (sParam.mErrorStatus == STL_SUCCESS) && (sParam.mHasSQLError == STL_FALSE),
                   RAMP_ERR_SYNTAX);

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SYNTAX )
    {
        if ( sParam.mErrorStatus == STL_SUCCESS )
        {
            /**
             * SQL syntax error 가 존재하는 경우
             * - 이미 출력함 
             */

            stlPushError( STL_ERROR_LEVEL_ABORT,
                          ZTP_ERRCODE_FATAL_ERROR_WHILE_DOING_EMBEDDED_SQL_PREPROCESSING,
                          NULL,
                          sParam.mErrorStack );
        }
        else
        {
            /**
             * C syntax error 인 경우
             */

            stlPushError( STL_ERROR_LEVEL_ABORT,
                          ZTP_ERRCODE_SYNTAX,
                          sParam.mErrorMsg,
                          sParam.mErrorStack );
        }
    }

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */

