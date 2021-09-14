/*******************************************************************************
 * ztppCalc.y
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *    reference : http://c.comsci.us/syntax/index.html
 *    reference : ISO/IEC 9899:1999
 *
 ******************************************************************************/

%{
#include <stdio.h>
#include <string.h>

#include <stl.h>
#include <dtl.h>
#include <goldilocks.h>
#include <sqlext.h>
#include <ztpDef.h>
#include <ztpMisc.h>
#include <ztppCommon.h>
#include <ztpvCVarFunc.h>
#include <ztpvCVar.tab.h>

#define YYSTYPE void *
#define YYLTYPE stlInt32
#define YYLLOC_DEFAULT(Current, Rhs, N)         \
    do {                                        \
        if (N)                                  \
            (Current) = (Rhs)[1];               \
        else                                    \
            (Current) = (Rhs)[0];               \
    } while (0)


#if 1
#define ZTPV_PARSE_CHECK( aInstruction )                \
    {                                                   \
        aInstruction;                                   \
        if( aParam->mErrorStatus != STL_SUCCESS )       \
        {                                               \
            YYABORT;                                    \
        }                                               \
    }
#else
#define ZTPV_PARSE_CHECK( aInstruction )   ;
#endif

%}

%pure-parser
%locations

%parse-param { ztpCHostVarParseParam  * aParam  }
%parse-param { void                   * aScanner }
%lex-param   { yyscan_t               * aScanner }

%token
ZTPV_TOK_LITERAL
ZTPV_TOK_ID

ZTPV_TOK_NOT_APPLICABLE

ZTPV_TOK_SYM_LPAREN
ZTPV_TOK_SYM_RPAREN
ZTPV_TOK_SYM_LBRACKET
ZTPV_TOK_SYM_RBRACKET

ZTPV_TOK_SYM_COMMA
ZTPV_TOK_SYM_QUESTION
ZTPV_TOK_SYM_SEMICOLON
ZTPV_TOK_SYM_COLON
ZTPV_TOK_SYM_AMPERSAND
ZTPV_TOK_SYM_ASTERISK
ZTPV_TOK_SYM_PLUS
ZTPV_TOK_SYM_MINUS
ZTPV_TOK_SYM_TILDE
ZTPV_TOK_SYM_EXCLAMATION
ZTPV_TOK_SYM_CIRCUMFLEX
ZTPV_TOK_SYM_SOLIDUS
ZTPV_TOK_SYM_PERCENT
ZTPV_TOK_SYM_SL
ZTPV_TOK_SYM_SR
ZTPV_TOK_SYM_LT
ZTPV_TOK_SYM_GT
ZTPV_TOK_SYM_LE
ZTPV_TOK_SYM_GE
ZTPV_TOK_SYM_EQ
ZTPV_TOK_SYM_NE
ZTPV_TOK_SYM_VBAR
ZTPV_TOK_SYM_AND
ZTPV_TOK_SYM_OR
ZTPV_TOK_SYM_PERIOD
ZTPV_TOK_SYM_PTR
ZTPV_TOK_SYM_INC
ZTPV_TOK_SYM_DEC

%start host_variable
%debug

%%

/********************************************************************
 * Phrase structure grammar
 ********************************************************************/

host_variable
    : unary_expression
    {
        aParam->mHostVar = $1;
        aParam->mCHostVarLength = aParam->mCurrLoc;
        YYACCEPT;
    }
    | ZTPV_TOK_SYM_COLON
    {
        aParam->mErrorStatus = STL_FAILURE;
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTP_ERRCODE_CPP_PUNCTUATION_SEQUENCES,
                      NULL,
                      aParam->mErrorStack );
        YYABORT;
    }
    ;

constant_expression
    : conditional_expression
    ;

primary_expression
    : identifier
    {
        ZTPV_PARSE_CHECK( $$ = ztpvMakeHostVarNode( aParam,
                                                    ZTP_HOSTVAR_DESC_ID,
                                                    $1,
                                                    NULL ) );
    }
    | ZTPV_TOK_LITERAL
    | ZTPV_TOK_SYM_LPAREN constant_expression ZTPV_TOK_SYM_RPAREN
    ;

identifier
    : ZTPV_TOK_ID
    {
        $$ = $1;
    }
    ;

postfix_expression
    : primary_expression
    {
        $$ = $1;
    }
    | postfix_expression ZTPV_TOK_SYM_LBRACKET constant_expression ZTPV_TOK_SYM_RBRACKET
    {
        stlChar  *sConstExpr;

        sConstExpr = ztpvMakeString( aParam,
                                     @3,
                                     @4 );

        ZTPV_PARSE_CHECK( $$ = ztpvMakeHostVarNode( aParam,
                                                    ZTP_HOSTVAR_DESC_ARRAY,
                                                    sConstExpr,
                                                    $1 ) );
    }
    | postfix_expression ZTPV_TOK_SYM_PERIOD identifier
    {
        ZTPV_PARSE_CHECK( $$ = ztpvMakeHostVarNode( aParam,
                                                    ZTP_HOSTVAR_DESC_PERIOD,
                                                    $3,
                                                    $1 ) );
    }
    | postfix_expression ZTPV_TOK_SYM_PTR identifier
    {
        ZTPV_PARSE_CHECK( $$ = ztpvMakeHostVarNode( aParam,
                                                    ZTP_HOSTVAR_DESC_PTR,
                                                    $3,
                                                    $1 ) );
    }
    | postfix_expression ZTPV_TOK_SYM_INC
    {
        $$ = $1;
    }
    | postfix_expression ZTPV_TOK_SYM_DEC
    {
        $$ = $1;
    }
    ;

unary_expression
    : postfix_expression
    {
        $$ = $1;
    }
    | ZTPV_TOK_SYM_INC unary_expression
    {
        $$ = $2;
    }
    | ZTPV_TOK_SYM_DEC unary_expression
    {
        $$ = $2;
    }
    | unary_operator cast_expression
    {
        $$ = $2;
    }
    ;

unary_operator
    : ZTPV_TOK_SYM_AMPERSAND
    | ZTPV_TOK_SYM_ASTERISK
    | ZTPV_TOK_SYM_PLUS
    | ZTPV_TOK_SYM_MINUS
    | ZTPV_TOK_SYM_TILDE
    | ZTPV_TOK_SYM_EXCLAMATION
    ;

cast_expression
    : unary_expression
    {
        $$ = $1;
    }
    ;

multiplicative_expression
    : cast_expression
    | multiplicative_expression ZTPV_TOK_SYM_ASTERISK unary_expression
    | multiplicative_expression ZTPV_TOK_SYM_SOLIDUS unary_expression
    | multiplicative_expression ZTPV_TOK_SYM_PERCENT unary_expression
    ;

additive_expression
    : multiplicative_expression
    | additive_expression ZTPV_TOK_SYM_PLUS multiplicative_expression
    | additive_expression ZTPV_TOK_SYM_MINUS multiplicative_expression
    ;

shift_expression
    : additive_expression
    | shift_expression ZTPV_TOK_SYM_SL additive_expression
    | shift_expression ZTPV_TOK_SYM_SR additive_expression
    ;

relational_expression
    : shift_expression
    | relational_expression ZTPV_TOK_SYM_LT shift_expression
    | relational_expression ZTPV_TOK_SYM_GT shift_expression
    | relational_expression ZTPV_TOK_SYM_LE shift_expression
    | relational_expression ZTPV_TOK_SYM_GE shift_expression
    ;

equality_expression
    : relational_expression
    | equality_expression ZTPV_TOK_SYM_EQ relational_expression
    | equality_expression ZTPV_TOK_SYM_NE relational_expression
    ;

and_expression
    : equality_expression
    | and_expression ZTPV_TOK_SYM_AMPERSAND equality_expression
    ;

exclusive_or_expression
    : and_expression
    | exclusive_or_expression ZTPV_TOK_SYM_CIRCUMFLEX and_expression
    ;

inclusive_or_expression
    : exclusive_or_expression
    | inclusive_or_expression ZTPV_TOK_SYM_VBAR exclusive_or_expression
    ;

logical_and_expression
    : inclusive_or_expression
    | logical_and_expression ZTPV_TOK_SYM_AND inclusive_or_expression
    ;

logical_or_expression
    : logical_and_expression
    | logical_or_expression ZTPV_TOK_SYM_OR logical_and_expression
    ;

conditional_expression
    : logical_or_expression
    | logical_or_expression ZTPV_TOK_SYM_QUESTION constant_expression ZTPV_TOK_SYM_COLON conditional_expression
    ;

%%
