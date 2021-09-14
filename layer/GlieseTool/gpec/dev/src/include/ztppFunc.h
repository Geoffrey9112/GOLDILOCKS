/*******************************************************************************
 * ztppFunc.h
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


#ifndef _ZTPPFUNC_H_
#define _ZTPPFUNC_H_ 1

/**
 * @file ztppFunc.h
 * @brief C Preprocessor Parser for Gliese Embedded SQL
 */

/**
 * @defgroup ztppFunc C Preprocessor Parser
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
#define YY_EXTRA_TYPE ztpPPParseParam *
#endif

ztpArgNameList *ztppMakeArgNameList( ztpPPParseParam  *aParam,
                                     stlChar          *aArgName,
                                     ztpArgNameList   *aList );

stlStatus ztppMakeIncludeHeaderFile( ztpPPParseParam *aParam,
                                     stlChar         *aHeaderFileName );

stlStatus ztppMakeMacro( ztpPPParseParam *aParam,
                         stlChar         *aMacroName,
                         stlInt32         aArgCount,
                         stlBool          aIsVarArgs,
                         ztpArgNameList  *aArgNameList,
                         stlChar         *aReplacementList );

stlStatus ztppMakeFunctionLikeMacro( ztpPPParseParam *aParam,
                                     stlChar         *aMacroName,
                                     stlBool          aIsVarArgs,
                                     ztpArgNameList  *aArgNameList,
                                     stlChar         *aReplacementList );

stlStatus ztppProcessUndefMacro( ztpPPParseParam *aParam,
                                 stlChar         *aMacroName );

stlChar *ztppMakeStringByPPTokens( ztpPPParseParam  *aParam );

ztpTokenStream *ztppMakePPTokens( ztpPPParseParam  *aParam,
                                  stlInt32          aStartPos,
                                  stlInt32          aEndPos,
                                  stlBool           aIsLast );

ztpPPParameterList *ztppMakeParameterList( ztpPPParseParam     *aParam,
                                           ztpPPParameterList  *aParameterList,
                                           ztpPPConstExpr      *aConstExpr );

ztpPPPrimaryExpr *ztppMakePPPrimaryExpr( ztpPPParseParam     *aParam,
                                         ztpPPTerminalType    aType,
                                         stlInt64             aValue,
                                         ztpPPConstExpr      *aConstExpr,
                                         stlChar             *aMacroName,
                                         ztpPPParameterList  *aParameterList );

ztpPPUnaryExpr *ztppMakePPUnaryExpr( ztpPPParseParam     *aParam,
                                     ztpPPUnaryExpr      *aPPUnaryExpr,
                                     ztpPPPrimaryExpr    *aPPPrimaryExpr,
                                     ztpPPOperator        aOperator );

ztpPPMulExpr *ztppMakePPMulExpr( ztpPPParseParam     *aParam,
                                 ztpPPMulExpr        *aPPMulExpr,
                                 ztpPPUnaryExpr      *aPPUnaryExpr,
                                 ztpPPOperator        aOperator );

ztpPPAddExpr *ztppMakePPAddExpr( ztpPPParseParam     *aParam,
                                 ztpPPAddExpr        *aPPAddExpr,
                                 ztpPPMulExpr        *aPPMulExpr,
                                 ztpPPOperator        aOperator );

ztpPPShiftExpr *ztppMakePPShiftExpr( ztpPPParseParam     *aParam,
                                     ztpPPShiftExpr      *aPPShiftExpr,
                                     ztpPPAddExpr        *aPPAddExpr,
                                     ztpPPOperator        aOperator );

ztpPPRelationalExpr *ztppMakePPRelationalExpr( ztpPPParseParam     *aParam,
                                               ztpPPRelationalExpr *aPPRelationalExpr,
                                               ztpPPShiftExpr      *aPPShiftExpr,
                                               ztpPPOperator        aOperator );

ztpPPEqualExpr *ztppMakePPEqualExpr( ztpPPParseParam     *aParam,
                                     ztpPPEqualExpr      *aPPEqualExpr,
                                     ztpPPRelationalExpr *aPPRelationalExpr,
                                     ztpPPOperator        aOperator );

ztpPPBitAndExpr *ztppMakePPBitAndExpr( ztpPPParseParam     *aParam,
                                       ztpPPBitAndExpr     *aPPBitAndExpr,
                                       ztpPPEqualExpr      *aPPEqualExpr,
                                       ztpPPOperator        aOperator );

ztpPPBitXorExpr *ztppMakePPBitXorExpr( ztpPPParseParam     *aParam,
                                       ztpPPBitXorExpr     *aPPBitXorExpr,
                                       ztpPPBitAndExpr     *aPPBitAndExpr,
                                       ztpPPOperator        aOperator );

ztpPPBitOrExpr *ztppMakePPBitOrExpr( ztpPPParseParam     *aParam,
                                     ztpPPBitOrExpr      *aPPBitOrExpr,
                                     ztpPPBitXorExpr     *aPPBitXorExpr,
                                     ztpPPOperator        aOperator );

ztpPPAndExpr *ztppMakePPAndExpr( ztpPPParseParam     *aParam,
                                 ztpPPAndExpr        *aPPAndExpr,
                                 ztpPPBitOrExpr      *aPPBitOrExpr,
                                 ztpPPOperator        aOperator );

ztpPPOrExpr *ztppMakePPOrExpr( ztpPPParseParam     *aParam,
                               ztpPPOrExpr         *aPPOrExpr,
                               ztpPPAndExpr        *aPPAndExpr,
                               ztpPPOperator        aOperator );

ztpPPCondExpr *ztppMakePPCondExpr( ztpPPParseParam     *aParam,
                                   ztpPPOrExpr         *aPPOrExpr,
                                   ztpPPConstExpr      *aPPConstExpr,
                                   ztpPPCondExpr       *aPPCondExpr );

ztpPPConstExpr *ztppMakePPConstExpr( ztpPPParseParam     *aParam,
                                     ztpPPCondExpr       *aPPCondExpr );

stlInt64 ztppEvaluateExpression( ztpPPParseParam   *aParam,
                                 stlChar           *aExpression );

stlStatus ztppReplaceUndefinedMacro( ztpPPParseParam  *aParam,
                                     stlChar          *aReplacedMacro,
                                     stlInt32         *aReplacedMacroLen );

stlStatus ztppParseIt(ztpConvertContext  *aContext);

stlInt32 ztpPPlex( YYSTYPE *, YYLTYPE *, void * );
stlInt32 ztpPPlex_init( void ** );
stlInt32 ztpPPlex_destroy( void * );
void     ztpPPset_extra( YY_EXTRA_TYPE, void * );
stlInt32 ztpPPparse( ztpPPParseParam *, void * );

stlInt32 ztpPPerror( YYLTYPE          * aLloc,
                     ztpPPParseParam   * aParam,
                     void             * aScanner,
                     const stlChar    * aMsg );

void ztppGoPrevToken( ztpPPParseParam  *aParam );
void ztppPushedBackInputStream( ztpPPParseParam  *aParam,
                                stlChar          *aReplaceString,
                                stlInt32          aLength );

void ztppWritePPToken( ztpPPParseParam  *aParam,
                       stlChar          *aToken );

void ztppWritePPMacroToken( ztpPPParseParam  *aParam,
                            stlChar          *aMacro );

void ztppUnputToken( void *yyscanner, stlChar *aCur, stlInt32 aLen );
void ztppGoAndIgnoreGroup( ztpPPParseParam  *aParam,
                           void             *aScanner );

stlInt32 ztppGetNextToken( ztpPPParseParam  *aParam,
                           void             *aScanner );
stlBool  ztppIsLParen( stlInt32   aToken );
stlBool  ztppIsRParen( stlInt32   aToken );

/** @} */

#endif /* _ZTPPFUNC_H_ */
