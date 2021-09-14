/*******************************************************************************
 * ztpCParseFunc.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztpCParseFunc.h 6342 2012-11-14 06:19:17Z mycomman $
 *
 * NOTES
 *
 *
 ******************************************************************************/


#ifndef _ZTPCPARSEFUNC_H_
#define _ZTPCPARSEFUNC_H_ 1

/**
 * @file ztpCParseFunc.h
 * @brief Command Parser for Gliese SQL
 */

/**
 * @defgroup ztpCParseFunc Command Parser
 * @ingroup ztp
 * @{
 */

#ifndef YYSTYPE
#define YYSTYPE       void *
#endif
#ifndef YYLTYPE
#define YYLTYPE       stlInt32
#endif
#ifndef YY_EXTRA_TYPE
#define YY_EXTRA_TYPE ztpCParseParam *
#endif

YYSTYPE ztpMakeExecSqlStmtToC(ztpCParseParam   *aParam,
                              stlInt32          aStartPos,
                              stlInt32         *aEndPos);

stlChar *ztpMakeStaticConnStringInC( ztpCParseParam  *aParam,
                                     stlChar         *aConnName );

stlStatus
ztpProcessCDeclaration( ztpCParseParam   *aParam,
                        ztpCDeclaration  *aCDeclaration,
                        stlBool           aIsParameter );

ztpCDeclaration *
ztpMakeCDeclaration( ztpCParseParam         *aParam,
                     stlInt32                aStartPos,
                     stlInt32                aEndPos,
                     stlInt32                aLineNo,
                     ztpCVariable           *aCVariable,
                     zlplCDataType           aCDataType,
                     stlChar                *aPrecision,
                     stlChar                *aScale,
                     ztpCDeclaratorList     *aList );

ztpCDeclaratorList *
ztpMakeCDeclaratorList( ztpCParseParam        *aParam,
                        ztpCDeclarator        *aCDeclarator,
                        ztpCDeclaratorList    *aCDeclaratorList );

ztpCDeclarationList *
ztpMakeCDeclarationList( ztpCParseParam       *aParam,
                         ztpCDeclaration      *aCDeclaration,
                         ztpCDeclarationList  *aCDeclarationList );

ztpCParameterDeclaration  *ztpMakeCParamDecl( ztpCParseParam   *aParam,
                                              stlInt64          aDeclSpec,
                                              ztpCDeclarator   *aCDeclarator );

ztpCParameterList *ztpMakeCParameterList( ztpCParseParam           *aParam,
                                          ztpCParameterList        *aCParameterList,
                                          ztpCParameterDeclaration *aCParamDecl );

ztpCIdentifier *ztpMakeCIdentifier(ztpCParseParam   *aParam,
                                   stlChar          *aIdentifier);
ztpCIdentifierList *ztpMakeCIdentifierList(ztpCParseParam      *aParam,
                                           ztpCIdentifierList  *aCIdentifierList,
                                           ztpCIdentifier      *aCIdentifier);

ztpTypeSpecifierNode *ztpMakeTypeSpecifierNode(ztpCParseParam   *aParam,
                                               ztpTypeSpecifier  aTypeSpecifier,
                                               stlChar          *aPrecision,
                                               stlChar          *aScale,
                                               ztpCVariable     *aCVariable,
                                               stlInt32          aPos,
                                               stlInt32          aLen);

ztpTypeSpecifierList *ztpMakeTypeSpecifierList(ztpCParseParam        *aParam,
                                               ztpTypeSpecifierList  *aTypeSpecifierList,
                                               ztpTypeSpecifierNode  *aTypeSpecifierNode);

stlChar *ztpMakeCAssignmentExpression( ztpCParseParam        *aParam,
                                       stlInt32               aStartPos,
                                       stlInt32               aEndPos );

ztpCDirectDeclarator *ztpMakeCDirectDeclarator(ztpCParseParam       *aParam,
                                               ztpCIdentifier       *aCIdentifier,
                                               ztpCDeclarator       *aCDeclarator,
                                               ztpCDirectDeclarator *aCDirectDeclarator,
                                               stlChar              *aArrayValue,
                                               ztpCIdentifierList   *aCIdentifierList,
                                               ztpCParameterList    *aCParameterList );

ztpCDeclarator *ztpMakeCDeclarator(ztpCParseParam       *aParam,
                                   ztpCDirectDeclarator *aDirectDeclarator,
                                   stlBool               aUsePointer);

ztpCStructSpecifier *
ztpMakeCStructSpecifier( ztpCParseParam       *aParam,
                         ztpCIdentifier       *aTagName,
                         ztpCDeclarationList  *aList );

ztpCVariable *
ztpProcessCStructSpecifier( ztpCParseParam      *aParam,
                            stlBool              aIsTypedef,
                            ztpCStructSpecifier *aCStructSpecifier );

YYSTYPE ztpMakeSqlStmt(ztpCParseParam   *aParam,
                       stlInt32          aStartPos,
                       stlInt32         *aEndPos);
YYSTYPE ztpMakeExecSqlInclude(ztpCParseParam   *aParam,
                              stlChar          *aFileName);
stlChar *ztpMakeIncludeFile(ztpCParseParam   *aParam,
                            stlInt32          aStartPos,
                            stlInt32         *aEndPos);
ztpIncludePath *ztpMakePathList(ztpCParseParam   *aParam,
                                stlInt32          aStartPos,
                                stlInt32         *aEndPos);
YYSTYPE ztpMakeExecSqlException(ztpCParseParam        *aParam,
                                ztpExceptionCondition *aExceptionCondition,
                                ztpExceptionAction    *aExceptionAction);
YYSTYPE ztpMakeExceptionCondition(ztpCParseParam            *aParam,
                                  ztpExceptionConditionType  aType,
                                  stlChar                   *aValue);
YYSTYPE ztpMakeExceptionAction(ztpCParseParam         *aParam,
                               ztpExceptionActionType  aType,
                               stlChar                *aValue);
ztpHostVariable *ztpMakeHostVariableInC( ztpCParseParam  *aParam,
                                         stlInt32         aStartPos,
                                         stlInt32        *aEndPos );
YYSTYPE ztpMakeSqlContextAction(ztpCParseParam         *aParam,
                                ztpParseTreeType        aParseTreeType,
                                ztpContextActionType    aContextActionType,
                                stlChar                *aName);
ztpAutoCommit *ztpMakeAutoCommit(ztpCParseParam *aParam,
                                 stlBool         aIsOn );

YYSTYPE ztpIncSymTabDepth(ztpCParseParam   *aParam);
YYSTYPE ztpDecSymTabDepth(ztpCParseParam   *aParam);

stlStatus ztpProcessCInclude(ztpCParseParam  *aParam,
                             stlChar         *aFileName);

stlStatus ztpSetSqlPrecompileError( ztpCParseParam  *aParam );

stlStatus ztpCParseIt(ztpConvertContext  *aContext,
                      ztpParseTree      **aParseTree);

stlInt32 ztpClex( YYSTYPE *, YYLTYPE *, void * );
stlInt32 ztpClex_init( void ** );
stlInt32 ztpClex_destroy( void * );
void     ztpCset_extra( YY_EXTRA_TYPE, void * );
stlInt32 ztpCparse( ztpCParseParam *, void * );

stlInt32 ztpCerror( YYLTYPE          * aLloc,
                    ztpCParseParam   * aParam,
                    void             * aScanner,
                    const stlChar    * aMsg );

stlInt32 ztpCMinlex( YYSTYPE *, YYLTYPE *, void * );
stlInt32 ztpCMinlex_init( void ** );
stlInt32 ztpCMinlex_destroy( void * );
void     ztpCMinset_extra( YY_EXTRA_TYPE, void * );
stlInt32 ztpCMinparse( ztpCParseParam *, void * );

stlInt32 ztpCMinerror( YYLTYPE          * aLloc,
                       ztpCParseParam   * aParam,
                       void             * aScanner,
                       const stlChar    * aMsg );

void ztpSetLexPos( void *aScanner, stlInt32  aPos );

//const ztpKeyWord *ztpScanCKeyWordLookup( stlChar * aIdentifier );

/** @} */

#endif /* _ZTPCPARSEFUNC_H_ */
